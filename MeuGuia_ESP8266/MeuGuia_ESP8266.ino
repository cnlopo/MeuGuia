#include <ESPAsyncUDP.h>
#include <ESPAsyncDNSServer.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "navegacao.h"
#include "paginas.h"

const char* NOMINATIM_USER_AGENT = "MeuGuia-ESP8266/1.0";

static const uint8_t GPS_RX_PIN = D6;
static const uint8_t GPS_TX_PIN = D5;
static const uint8_t MOTOR_PIN = D8;
static const uint32_t INTERVALO_GPS_MS = 1000;

AsyncWebServer server(80);
AsyncDNSServer dnsServer;
class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    bool canHandle(__unused AsyncWebServerRequest* request) const {
      return true;
    }
    void handleRequest(AsyncWebServerRequest* request) {
      AsyncResponseStream* response = request->beginResponseStream("text/html");
      response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
      response->print("<p>This is out captive portal front page.</p>");
      response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
#ifndef CONFIG_IDF_TARGET_ESP32H2
      response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
#endif
      response->print("</body></html>");
      request->send(response);
    }
};

String setEndereco = "";
String setSsid = "";
String setPass = "";
const IPAddress apIP(192, 168, 1, 1);
const IPAddress netmask(255, 255, 255, 0);
const byte dnsPort = 53;

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;
EstadoNavegacao estadoNavegacao;
bool destinoConfigurado = false;
String enderecoDestino = "";
double destinoLat = 0.0;
double destinoLng = 0.0;
uint32_t ultimaLeituraGPS = 0;
NivelProximidade nivelAnterior = NIVEL_LONGE;
volatile bool buscaPendente = false;
volatile bool buscando = false;
String enderecoPendente = "";
bool ultimaBuscaOk = false;
void onRequest(AsyncWebServerRequest *request){
  request->send(404);
}
String processor(const String& var){
  if(var == "WIFIPLACEHOLDER"){
    String status = "Conectado a rede:";
    status+=WiFi.SSID();
    status+="<br>";
    status+=WiFi.localIP().toString();
    if(WiFi.status() != WL_CONNECTED){
      status = "Sem conexão";
    }
    return status;
  }
  if(var == "ENDERECOSALVO"){
    return ("Endereco atual: %s", enderecoNome());
  }
  if(var == "STYLE"){
    return style;
  }
  return String();
}

String formatURL(String rua){
  rua.replace(" ", "+");
  Serial.println(rua);
  String url = "https://nominatim.openstreetmap.org/search?street=" + rua + "&city=ribeirao+preto&format=json&limit=1";
  Serial.println(url);
  return url;
}
void requestOSM(String url){
  if(WiFi.status()==WL_CONNECTED){
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure(); 
  http.addHeader("User-Agent", "MeuGuia (rogeriofilho1105@gmail.com)");
  Serial.println("requisição para o Nominatim");
  http.begin(client, url);
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0) {
      Serial.printf("[HTTP] Código de resposta: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();        
        JsonDocument doc; 
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          if (doc.is<JsonArray>() && doc.size() > 0) {
            JsonObject resultado = doc[0];
            const char* nome_exibicao = resultado["display_name"];
            const char* latitude = resultado["lat"];
            const char* longitude = resultado["lon"];
            String nome = "endereco.json";
            Serial.println("--- Resultado Encontrado ---");
            Serial.printf("Endereço= %s\n", nome_exibicao);
            Serial.printf("Latitude= %s\n", latitude);
            Serial.printf("Longitude= %s\n", longitude);
            saveEndereco(nome, resultado);
          } else {
            Serial.println("Nenhum local encontrado para este endereço");
          }
        } else {
          Serial.print("Falha ao ler o JSON: ");
          Serial.println(error.c_str());
        }
      }
    } else {
      Serial.printf("[HTTP] Falha na requisição: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    setEndereco = "";
    delay(1000); 
  }
}
bool saveEndereco(String path, JsonObject resultado) {
    File file = SPIFFS.open(path, "w");
    if (!file) {
        return false;
    }

    if (serializeJson(resultado, file) == 0) {
        file.close();
        return false;
    }
    file.close();
    return true;
}
bool readEndereco() {
    File file = SPIFFS.open("endereco.json", "r");
    if (!file) {
        return false;
    }
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        file.close();
        return false;
    }
    file.close();
    destinoLat = doc["lat"];
    destinoLng = doc["lon"];
    return true;
}
String enderecoNome(){
    File file = SPIFFS.open("endereco.json", "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
    }
    // Deserialize JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
    }
    String nome = doc["display_name"];
    file.close();
    return nome;
}
void listFiles(){
  Dir dir = SPIFFS.openDir("/endereco");
  while (dir.next()) {
    Serial.print("File: ");
    Serial.print(dir.fileName());
    delay(1000);
  }
}
JsonDocument readConfig(){
    File file = SPIFFS.open("Config.json", "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
    }
    // Deserialize JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
    }
    file.close();
    return doc;
}

bool saveConfig() {
    StaticJsonDocument<256> doc;
    doc["SSID"] = setSsid;
    doc["password"] = setPass;
    File file = SPIFFS.open("Config.json", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write JSON to file");
        file.close();
        return false;
    }
    file.close();
    Serial.println("JSON saved successfully");
    return true;
}

void wifiConnect(){
  JsonDocument wifi;
  if(SPIFFS.open("Config.json", "r")){
    wifi = readConfig();
  }
  else saveConfig();
  const char *ssid = wifi["SSID"];
  const char *password = wifi["password"]; 
  WiFi.begin(ssid, password);
}

void pararVibracao() {
  digitalWrite(MOTOR_PIN, LOW);
}

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

  int quantidade = (nivel == NIVEL_PREPARAR) ? 1 :
                   (nivel == NIVEL_DAR_SINAL) ? 2 :
                   (nivel == NIVEL_CHEGANDO) ? 3 : 4;
  uint16_t duracaoPulso = (nivel == NIVEL_PASSOU) ? 120 : 300;
  uint16_t intervaloPulso = (nivel == NIVEL_PASSOU) ? 100 : 220;
  uint32_t agora = millis();

  if (nivel == NIVEL_PASSOU && pulso >= quantidade && !vibrando) {
    pararVibracao();
    return;
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
      digitalWrite(MOTOR_PIN, HIGH);
      vibrando = true;
      transicaoMs = agora;
    }
  } else if (nivel != NIVEL_PASSOU && agora - inicioCiclo >= 1800) {
    pulso = 0;
    transicaoMs = agora;
    inicioCiclo = agora;
  }
}

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, HIGH);
  delay(3000);
  pararVibracao();
  Serial.begin(115200);
  gpsSerial.begin(9600);
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS falhou ao iniciar");
    return;
  }
  destinoConfigurado = readEndereco();
  JsonDocument wifi;
  wifiConnect();
  unsigned long startAttempt = millis();
  const unsigned long timeout = 8000;
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < timeout) {
    Serial.print(".");
    delay(500);
  }
  WiFi.softAPConfig(apIP, apIP, netmask);
  WiFi.softAP("MeuGUIA", "");
  dnsServer.start(dnsPort, "*", apIP);
  WiFi.setSleep(false);
  server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html; charset=utf-8", mainPage, processor);
  });
  server.on("/config", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html; charset=utf-8", configPage, processor);
  });
  server.on("/endereco", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html; charset=utf-8", enderecoPage, processor);
  });
  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid")) {
      setSsid = request->getParam("ssid")->value();
      Serial.print(setSsid);
    }
    if (request->hasParam("pass")) {
      setPass = request->getParam("pass")->value();
      Serial.print(setPass);
    }
      request->redirect("/");
  });
  server.on("/query", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("rua")) {
      setEndereco = request->getParam("rua")->value();
      Serial.print(setEndereco);
    }
  request->redirect("/");
  });
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
  Serial.println(destinoConfigurado);
}

void loop() {
  if(setEndereco!=""){
    String url = formatURL(setEndereco);
    requestOSM(url);
  }
  if(setSsid!=""){
    saveConfig();
    wifiConnect();
  }
  while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());
  uint32_t agora = millis();
  if (destinoConfigurado && gps.location.isValid() && gps.location.isUpdated() &&
      (agora - ultimaLeituraGPS >= INTERVALO_GPS_MS)) {
    ultimaLeituraGPS = agora;
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();
    double distancia = distancia_metros(latitude, longitude, destinoLat, destinoLng);
    Serial.println(latitude);
    NivelProximidade nivel = atualizar_navegacao(&estadoNavegacao, latitude, longitude, agora);

    Serial.printf("GPS: %.6f, %.6f | Distancia: %.1f m | Nivel: %d\n", latitude, longitude, distancia, (int)nivel);
    if (nivel != nivelAnterior) {
      Serial.printf("Mudanca de nivel: %d -> %d\n", (int)nivelAnterior, (int)nivel);
      nivelAnterior = nivel;
    }
  }

  if (destinoConfigurado) atualizarVibracao(estadoNavegacao.nivel);
  else pararVibracao();
  yield();
}
