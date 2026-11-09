
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncEventSource events("/events"); // event source (Server-Sent events)

const char* http_username = "admin";
const char* http_password = "admin";
const char* ssid = "Rpf1105";
const char* password = "12345678";
String endereco = "";

//estrutura para salvar ponto de onibus, ativo é caso ele será contabilizado ou não para os calculos de destinos
struct ponto_onibus {
  String endereco;
  float latitude;
  float longitude;
  bool ativo;
  struct ponto_onibus *next;
  };

//elaborar pagina depois de resolver salvamento SPIFFS para gerar recursivamente elementos html ao chamar
String getPage(){
  String mainpage = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
    <p>Digite a sua Rua de destino</p>
    <form action="/query">
        <input type="text" name="rua" id="rua">
        <input type="submit" value="Enviar">
    </form>
    </body>
    </html>
)rawliteral";
return mainpage;
}
bool shouldReboot = false;

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}
void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  //Handle upload
}
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  //Handle WebSocket event
}

void setup(){
  pinMode(D4, OUTPUT);
  Serial.begin(115200);
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  if (MDNS.begin("meuguia")) {
    Serial.println("mDNS iniciado!");
  }
  Serial.println(WiFi.localIP());

  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.addHandler(&events);
  //Index
    server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", getPage());
  });
     server.on("/query", HTTP_GET, [](AsyncWebServerRequest *request){
      if (request->hasParam("rua")) {
      endereco = request->getParam("rua")->value();
      Serial.print(endereco);
      }
      request->redirect("/");
  });
        

  // Catch-All Handlers
  // Any request that can not find a Handler that canHandle it
  // ends in the callbacks below.
  server.onNotFound(onRequest);
  server.onFileUpload(onUpload);
  server.onRequestBody(onBody);
  server.begin();
}

void loop(){
  MDNS.update();
  if(endereco!=""){
    getAddress(endereco);
    }
}

void getAddress(String rua){
  rua.replace(" ", "+");
  Serial.println(rua);
  String url = "https://nominatim.openstreetmap.org/search?street=" + rua + "&city=ribeirao+preto&format=json&limit=1";
  Serial.println(url);
  return requestOSM(url);
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
            String nome = resultado["name"] + ".json";
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
    endereco = "";
    delay(1000); 
  }
}
bool saveEndereco(String path, JsonObject resultado) {

    // Open file for writing
    File file = SPIFFS.open(path, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }

    // Serialize JSON to file
    if (serializeJson(resultado, file) == 0) {
        Serial.println("Failed to write JSON to file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("JSON saved successfully");
    return true;
}

// Function to read JSON from SPIFFS
bool readEndereco(String path) {
    File file = SPIFFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }
    // Deserialize JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.print("❌ Failed to parse JSON: ");
        Serial.println(error.c_str());
        file.close();
        return false;
    }
    file.close();
    // Print values
    Serial.println("📄 JSON content:");
    Serial.printf("SSID: %s\n", doc["wifi_ssid"].as<const char*>());
    Serial.printf("Password: %s\n", doc["wifi_password"].as<const char*>());
    Serial.printf("Device ID: %d\n", doc["device_id"].as<int>());
    Serial.printf("Enabled: %s\n", doc["enabled"].as<bool>() ? "true" : "false");
    return true;
}