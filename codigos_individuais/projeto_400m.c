#define LED_BUILTIN_PIN 2
#define LED_EXT_PIN     4

float distancia = 400;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  pinMode(LED_EXT_PIN, OUTPUT);
  Serial.println("ESP32 Pronto!");
}

void loop() {
  if (distancia > 500) {
    digitalWrite(LED_BUILTIN_PIN, LOW);
    digitalWrite(LED_EXT_PIN, LOW);
  } else if (distancia > 300) {
    digitalWrite(LED_BUILTIN_PIN, HIGH);
    digitalWrite(LED_EXT_PIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN_PIN, LOW);
    digitalWrite(LED_EXT_PIN, LOW);
    delay(2500);
  } else if (distancia > 100) {
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
