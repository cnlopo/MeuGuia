#ifndef serverFunc
#define serverFunc

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
bool readEndereco(String path, float *latitude, float *longitude) {
    File file = SPIFFS.open(path, "r");
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
    float lat = doc["lat"];
    float lon = doc["lon"];
    latitude = &lat;
    longitude = &lon;
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
  if(SPIFFS.open("Config.json", "r")){
    wifi = readConfig();
  }
  else saveConfig();
  const char *ssid = wifi["SSID"];
  const char *password = wifi["password"]; 
  WiFi.begin(ssid, password);
}
