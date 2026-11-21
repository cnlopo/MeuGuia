// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows how to deserialize a JSON document with ArduinoJson.

#include <iostream>
#include "ArduinoJson.h"

int main() {
  JsonDocument doc;

  const char* json =
      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
    return 1;
  }

  const char* sensor = doc["sensor"];
  long time = doc["time"];
  double latitude = doc["data"][0];
  double longitude = doc["data"][1];

  std::cout << sensor << std::endl;
  std::cout << time << std::endl;
  std::cout << latitude << std::endl;
  std::cout << longitude << std::endl;

  return 0;
}
