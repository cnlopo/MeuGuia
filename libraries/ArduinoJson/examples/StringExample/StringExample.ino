// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows the different ways you can use String with ArduinoJson.
//
// Use String objects sparingly, because ArduinoJson duplicates them in the
// JsonDocument. Prefer plain old char[], as they are more efficient in term of
// code size, speed, and memory usage.
//
// https://arduinojson.org/v7/example/string/

#include <ArduinoJson.h>

void setup() {
  JsonDocument doc;

  String input =
      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  deserializeJson(doc, input);

  long time = doc[String("time")];

  doc[String("time")] = time;

  String sensor = doc["sensor"];

  sensor = doc["sensor"].as<String>();

  doc["sensor"] = sensor;

  doc["sensor"] = serialized(sensor);

  doc[String("sen") + "sor"] = String("gp") + "s";

  if (doc["sensor"] == sensor) {
  }

  String output;
  serializeJson(doc, output);
}

void loop() {
}
