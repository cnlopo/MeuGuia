#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncEventSource events("/events"); // event source (Server-Sent events)

const char* ssid = "Rpf1105";
const char* password = "12345678";
const char* http_username = "admin";
const char* http_password = "admin";

//flag to use from web update to reboot the ESP
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
    request->send(200, "text/html", mainpage);
  });
     server.on("/query", HTTP_GET, [](AsyncWebServerRequest *request){
      if (request->hasParam("rua")) {
      String endereco;
      endereco = request->getParam("rua")->value();
      Serial.print(endereco);
      getAddress(endereco);
      }
      request->send(200, "text/html", mainpage);
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
}

void getAddress(String rua){
  rua.replace(" ", "+");
  Serial.println(rua);
  String url = "https://nominatim.openstreetmap.org/search?street=" + rua + "&format=json&limit=1";
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
            JsonObject primeiroResultado = doc[0];
            const char* nome_exibicao = primeiroResultado["display_name"];
            const char* latitude = primeiroResultado["lat"];
            const char* longitude = primeiroResultado["lon"];
            Serial.println("--- Resultado Encontrado ---");
            Serial.printf("Endereço: %s\n", nome_exibicao);
            Serial.printf("Latitude: %s\n", latitude);
            Serial.printf("Longitude: %s\n", longitude);
          } else {
            Serial.println("Nenhum local encontrado para este endereço.");
          }
        } else {
          Serial.print("Falha no parse do JSON: ");
          Serial.println(error.c_str());
        }
      }
    } else {
      Serial.printf("[HTTP] Falha na requisição: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    delay(1000); 
  }
String Index(){
  const char mainpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
    <p>Digite a sua Rua de destino</p>
    <form action="/query">
        <input type="text" name="rua" id="rua">
        <input type="submit" value="Enviar">
    </form>
)rawliteral";
 for (i=0, i>3)
  
}
mainpage+= "";
}



