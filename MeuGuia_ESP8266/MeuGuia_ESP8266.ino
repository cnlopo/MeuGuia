/*
  MeuGuia - ESP8266 + GPS NEO-6M + motores de vibracao
  Projeto Arduino IDE. Ajuste Wi-Fi e pinos na secao CONFIGURACAO.

  Hardware assumido (NodeMCU ESP8266):
    GPS TX -> D1 (RX do ESP8266)
    GPS RX -> D2 (TX do ESP8266, opcional)
    Motor 1 -> D5 via transistor/driver
    Motor 2 -> D6 via transistor/driver
  Nao conecte motores diretamente aos GPIOs; use transistor/driver e fonte adequada.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "navegacao.h"

// ===== CONFIGURACAO =====
const char* WIFI_SSID = "SEU_WIFI";
const char* WIFI_PASSWORD = "SUA_SENHA";
const char* NOMINATIM_USER_AGENT = "MeuGuia-ESP8266/1.0";

static const uint8_t GPS_RX_PIN = D1; // ligar ao TX do NEO-6M
static const uint8_t GPS_TX_PIN = D2; // ligar ao RX do NEO-6M (opcional)
static const uint8_t MOTOR_1_PIN = D5;
static const uint8_t MOTOR_2_PIN = D6;
static const uint32_t INTERVALO_GPS_MS = 1000;
// =========================

AsyncWebServer server(80);
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;
EstadoNavegacao estadoNavegacao;
bool destinoConfigurado = false;
String enderecoDestino = "";
double destinoLat = 0.0;
double destinoLng = 0.0;
uint32_t ultimaLeituraGPS = 0;
NivelProximidade nivelAnterior = NIVEL_LONGE;

const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>MeuGuia</title>
  <style>
    body{font-family:Arial,sans-serif;max-width:520px;margin:40px auto;padding:0 18px;line-height:1.5}
    input,button{box-sizing:border-box;width:100%;padding:12px;margin:8px 0;font-size:16px}
    button{cursor:pointer} .info{color:#444;font-size:14px}
  </style>
</head>
<body>
  <h1>MeuGuia</h1>
  <p>Digite o endereco do ponto de onibus onde deseja descer.</p>
  <form action="/query" method="get">
    <label for="rua">Endereco de destino</label>
    <input type="text" name="rua" id="rua" required placeholder="Rua, numero, cidade">
    <button type="submit">Definir destino</button>
  </form>
  <p class="info">Mantenha o dispositivo conectado ao Wi-Fi do ESP8266. O GPS precisa de sinal de satelite para obter a posicao.</p>
</body>
</html>
)rawliteral";

String codificarURL(const String& texto) {
  String resultado;
  const char hex[] = "0123456789ABCDEF";
  for (size_t i = 0; i < texto.length(); i++) {
    uint8_t c = (uint8_t)texto[i];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      resultado += (char)c;
    } else if (c == ' ') {
      resultado += "%20";
    } else {
      resultado += '%';
      resultado += hex[c >> 4];
      resultado += hex[c & 0x0F];
    }
  }
  return resultado;
}

void pararVibracao() {
  digitalWrite(MOTOR_1_PIN, LOW);
  digitalWrite(MOTOR_2_PIN, LOW);
}

// Padroes repetidos sem delay(), para nao bloquear a leitura do GPS.
void atualizarVibracao(NivelProximidade nivel) {
  static NivelProximidade nivelAtivo = NIVEL_LONGE;
  static uint32_t inicioCiclo = 0;
  static bool vibrando = false;
  static int pulso = 0;
  static uint32_t transicaoMs = 0;

  if (nivel != nivelAtivo) {
    nivelAtivo = nivel;
    inicioCiclo = millis();
    transicaoMs = inicioCiclo;
    pulso = 0;
    vibrando = false;
    pararVibracao();
  }

  if (nivel == NIVEL_LONGE || nivel == NIVEL_ENCERRADA) {
    pararVibracao();
    return;
  }

  // PREPARAR: 1 pulso; DAR_SINAL: 2; CHEGANDO: 3; PASSOU: 4 pulsos curtos uma vez.
  int quantidade = (nivel == NIVEL_PREPARAR) ? 1 :
                   (nivel == NIVEL_DAR_SINAL) ? 2 :
                   (nivel == NIVEL_CHEGANDO) ? 3 : 4;
  uint16_t duracaoPulso = (nivel == NIVEL_PASSOU) ? 120 : 300;
  uint16_t intervaloPulso = (nivel == NIVEL_PASSOU) ? 100 : 220;
  uint32_t agora = millis();

  if (nivel == NIVEL_PASSOU && pulso >= quantidade && !vibrando) {
    pararVibracao();
    return; // alerta de PASSOU ocorre apenas uma vez
  }

  if (vibrando) {
    if (agora - transicaoMs >= duracaoPulso) {
      pararVibracao();
      vibrando = false;
      transicaoMs = agora;
      pulso++;
    }
    return;
  }

  if (pulso < quantidade) {
    if (pulso == 0 || agora - transicaoMs >= intervaloPulso) {
      digitalWrite(MOTOR_1_PIN, HIGH);
      digitalWrite(MOTOR_2_PIN, HIGH);
      vibrando = true;
      transicaoMs = agora;
    }
  } else if (nivel != NIVEL_PASSOU && agora - inicioCiclo >= 1800) {
    // Reinicia o padrao enquanto estiver no mesmo nivel.
    pulso = 0;
    transicaoMs = agora;
  }
}

bool consultarEndereco(const String& endereco) {
  if (WiFi.status() != WL_CONNECTED) return false;

  // Nominatim aceita busca livre por q; o endereco deve incluir cidade/UF para melhorar o resultado.
  String url = "https://nominatim.openstreetmap.org/search?q=" + codificarURL(endereco) + "&format=json&limit=1";
  WiFiClientSecure client;
  client.setInsecure(); // simplifica a conexao TLS; para produto final, validar certificado e recomendavel.
  HTTPClient http;
  http.setTimeout(12000);
  http.begin(client, url);
  http.addHeader("User-Agent", NOMINATIM_USER_AGENT);
  Serial.println("Consultando geocodificacao do endereco...");
  int codigoHTTP = http.GET();
  bool sucesso = false;

  if (codigoHTTP == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(3072); // compatibilidade com ArduinoJson 6
    DeserializationError erro = deserializeJson(doc, payload);
    if (!erro && doc.is<JsonArray>() && doc.size() > 0) {
      JsonObject item = doc[0];
      destinoLat = String((const char*)item["lat"]).toDouble();
      destinoLng = String((const char*)item["lon"]).toDouble();
      enderecoDestino = endereco;
      iniciar_navegacao(&estadoNavegacao, destinoLat, destinoLng);
      destinoConfigurado = true;
      nivelAnterior = NIVEL_LONGE;
      sucesso = true;
      Serial.println("Destino definido: " + enderecoDestino);
      Serial.printf("Latitude: %.6f | Longitude: %.6f\n", destinoLat, destinoLng);
    } else {
      Serial.println("Endereco nao encontrado ou resposta JSON invalida.");
    }
  } else {
    Serial.printf("Falha na consulta HTTP: %d\n", codigoHTTP);
  }
  http.end();
  return sucesso;
}

void configurarRotasWeb() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html; charset=utf-8", MAIN_PAGE);
  });

  server.on("/query", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("rua")) {
      request->send(400, "text/plain; charset=utf-8", "Informe o endereco usando o campo rua.");
      return;
    }
    String endereco = request->getParam("rua")->value();
    endereco.trim();
    if (endereco.length() == 0) {
      request->send(400, "text/plain; charset=utf-8", "O endereco nao pode ficar vazio.");
      return;
    }

    // A consulta externa pode levar alguns segundos; por isso a resposta informa o resultado.
    bool sucesso = consultarEndereco(endereco);
    if (sucesso) {
      String resposta = "<!DOCTYPE html><html lang='pt-BR'><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'><body><h2>Destino configurado</h2><p>";
      resposta += enderecoDestino;
      resposta += "</p><p>Latitude: " + String(destinoLat, 6) + "<br>Longitude: " + String(destinoLng, 6) + "</p><a href='/'>Voltar</a></body></html>";
      request->send(200, "text/html; charset=utf-8", resposta);
    } else {
      request->send(502, "text/html; charset=utf-8", "<meta charset='UTF-8'><h2>Nao foi possivel encontrar o endereco.</h2><p>Verifique o endereco e a conexao Wi-Fi.</p><a href='/'>Tentar novamente</a>");
    }
  });
}

void setup() {
  pinMode(MOTOR_1_PIN, OUTPUT);
  pinMode(MOTOR_2_PIN, OUTPUT);
  pararVibracao();

  Serial.begin(115200);
  gpsSerial.begin(9600);
  Serial.println();
  Serial.println("Iniciando MeuGuia...");

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  uint32_t inicioWiFi = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicioWiFi < 20000) {
    delay(250);
    Serial.print('.');
    yield();
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP do MeuGuia: ");
    Serial.println(WiFi.localIP());
    if (MDNS.begin("meuguia")) Serial.println("Acesse http://meuguia.local (se suportado) ou use o IP acima.");
  } else {
    Serial.println("Wi-Fi nao conectado. Reinicie depois de conferir SSID e senha.");
  }

  configurarRotasWeb();
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
  while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());
  MDNS.update();

  uint32_t agora = millis();
  if (destinoConfigurado && gps.location.isValid() && gps.location.isUpdated() &&
      (agora - ultimaLeituraGPS >= INTERVALO_GPS_MS)) {
    ultimaLeituraGPS = agora;
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();
    double distancia = distancia_metros(latitude, longitude, destinoLat, destinoLng);
    NivelProximidade nivel = atualizar_navegacao(&estadoNavegacao, latitude, longitude, agora);

    Serial.printf("GPS: %.6f, %.6f | Distancia: %.1f m | Nivel: %d\n", latitude, longitude, distancia, (int)nivel);
    if (nivel != nivelAnterior) {
      Serial.printf("Mudanca de nivel: %d -> %d\n", (int)nivelAnterior, (int)nivel);
      nivelAnterior = nivel;
    }
  }

  // Atualiza o padrao de vibracao em todas as iteracoes, sem depender da frequencia do GPS.
  if (destinoConfigurado) atualizarVibracao(estadoNavegacao.nivel);
  else pararVibracao();
  yield();
}
