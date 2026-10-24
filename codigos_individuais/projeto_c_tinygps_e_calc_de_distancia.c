/*
Simulador utilizado: velxio.dev
Este projeto tem a placa ESP32 e o GPS NEO-6M, está sendo utilizado a biblioteca TinyGPS para pegar a localização do GPS NEO-6M e converter, tem uma distância alvo pré definida e faz o cálculo de distância alvo - localização do GPS NEO-6M retornando quantos metros faltam e piscando a luz de diferentes formas conforme nos projetos anteriores (projeto_400m.c e projeto_c_gps.c).
*/

#include <TinyGPS++.h>

#define LED_BUILTIN_PIN 2   // Built-in blue LED on ESP32 DevKit
#define LED_EXT_PIN     4   // External red LED

#define GPS_RX 16
#define GPS_TX 17

TinyGPSPlus gps;

double destinoLat = 40.417800;
double destinoLng = -3.703800;
double distancia = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  pinMode(LED_EXT_PIN, OUTPUT);
  Serial.println("ESP32 Pronto!");
  Serial.print("Distancia ate o destino: ");
  Serial.print(distancia);
  Serial.println(" metros");
  
  Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("Iniciando GPS...");

}

void loop() {
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  if (gps.location.isUpdated()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);

    distancia = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), destinoLat, destinoLng);

    Serial.print("Distancia ate o destino: ");
    Serial.print(distancia);
    Serial.println("metros");

    Serial.println("---");
  }

  if (distancia > 500) {
    // Acima de 500m: Sem alerta
    digitalWrite(LED_BUILTIN_PIN, LOW);
    digitalWrite(LED_EXT_PIN, LOW);
  } else if (distancia > 300) {
    // 301-500m: 1 piscada
    digitalWrite(LED_BUILTIN_PIN, HIGH);
    digitalWrite(LED_EXT_PIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN_PIN, LOW);
    digitalWrite(LED_EXT_PIN, LOW);
    delay(2500);
  } else if (distancia > 100) {
    // 101-300m: 2 piscadas
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED_BUILTIN_PIN, HIGH);
      digitalWrite(LED_EXT_PIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN_PIN, LOW);
      digitalWrite(LED_EXT_PIN, LOW);
      delay(500);
    }
    delay(2500);
  } else if (distancia > 50) {
    // 51-100m: 3 piscadas
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN_PIN, HIGH);
      digitalWrite(LED_EXT_PIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN_PIN, LOW);
      digitalWrite(LED_EXT_PIN, LOW);
      delay(500);
    }
    delay(2500);
  } else {
    // Abaixo de 50m : 4 piscadas rápidas
    for (int i = 0; i < 4; i++) {
      digitalWrite(LED_BUILTIN_PIN, HIGH);
      digitalWrite(LED_EXT_PIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN_PIN, LOW);
      digitalWrite(LED_EXT_PIN, LOW);
      delay(100);
    }
    delay(2500);
  }
}