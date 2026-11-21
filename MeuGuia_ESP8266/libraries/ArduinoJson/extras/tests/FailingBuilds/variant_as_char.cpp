// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>

int main() {
  JsonDocument doc;
  doc["dummy"].as<char>();
}
