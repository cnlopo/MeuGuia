// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows how to generate a JSON document with ArduinoJson.

#include <iostream>
#include "ArduinoJson.h"

int main() {
  JsonDocument doc;

  uint8_t input[] = {131, 166, 115, 101, 110, 115, 111, 114, 163, 103, 112, 115,
                     164, 116, 105, 109, 101, 206, 80,  147, 50,  248, 164, 100,
                     97,  116, 97,  146, 203, 64,  72,  96,  199, 58,  188, 148,
                     112, 203, 64,  2,   106, 146, 230, 33,  49,  169};

  DeserializationError error = deserializeMsgPack(doc, input);

  if (error) {
    std::cerr << "deserializeMsgPack() failed: " << error.c_str() << std::endl;
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
