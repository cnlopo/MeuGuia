/*
Simulador utilizado: velxio.dev
Este projeto tem a placa ESP32 e o GPS NEO-6M, uma distância pré-definida, pisca uma luz de diferente forma conforme no projeto anterior (projeto_400m.c) e o diferencial é que ele printa a localização do GPS NEO-6M sem conversão ($GPGGA... -> GPS Fix Data).
*/

#define LED_BUILTIN_PIN 2   // Built-in blue LED on ESP32 DevKit
#define LED_EXT_PIN     4   // External red LED

#define GPS_RX 16
#define GPS_TX 17

float distancia = 450;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  pinMode(LED_EXT_PIN, OUTPUT);
  Serial.println("ESP32 Pronto!");
  
  Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("Iniciando GPS...");

}

void loop() {
  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.write(c);
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