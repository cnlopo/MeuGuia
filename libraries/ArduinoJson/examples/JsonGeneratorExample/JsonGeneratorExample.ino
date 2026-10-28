// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows how to generate a JSON document with ArduinoJson.
//
// https://arduinojson.org/v7/example/generator/

#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);
  while (!Serial)
    continue;

  JsonDocument doc;

  doc["sensor"] = "gps";
  doc["time"] = 1351824120;

  JsonArray data = doc["data"].to<JsonArray>();
  data.add(48.756080);
  data.add(2.302038);

  serializeJson(doc, Serial);

  Serial.println();

  serializeJsonPretty(doc, Serial);
}

void loop() {
}
