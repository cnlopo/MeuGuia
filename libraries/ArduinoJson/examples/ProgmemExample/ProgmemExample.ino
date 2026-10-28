// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows the different ways you can use Flash strings with
// ArduinoJson.
//
// Use Flash strings sparingly, because ArduinoJson duplicates them in the
// JsonDocument. Prefer plain old char*, as they are more efficient in term of
// code size, speed, and memory usage.
//
// https://arduinojson.org/v7/example/progmem/

#include <ArduinoJson.h>

void setup() {
  JsonDocument doc;

  deserializeJson(doc, F("{\"sensor\":\"gps\",\"time\":1351824120,"
                         "\"data\":[48.756080,2.302038]}"));

  long time = doc[F("time")];

  doc[F("time")] = time;

  doc["sensor"] = F("gps");

  doc["sensor"] = serialized(F("\"gps\""));
  doc["sensor"] = serialized(F("\xA3gps"), 3);

  if (doc["sensor"] == F("gps")) {
  }
}

void loop() {
}
