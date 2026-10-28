// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows how to store your project configuration in a file.
// It uses the SD library but can be easily modified for any other file-system.
//
// The file contains a JSON document with the following content:
// {
//   "hostname": "examples.com",
//   "port": 2731
// }
//
// To run this program, you need an SD card connected to the SPI bus as follows:
// * MOSI <-> pin 11
// * MISO <-> pin 12
// * CLK  <-> pin 13
// * CS   <-> pin 4
//
// https://arduinojson.org/v7/example/config/

#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>

struct Config {
  char hostname[64];
  int port;
};

const char* filename = "/config.txt";
Config config;

void loadConfiguration(const char* filename, Config& config) {
  File file = SD.open(filename);

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  config.port = doc["port"] | 2731;
  strlcpy(config.hostname,
          doc["hostname"] | "example.com",
          sizeof(config.hostname));

  file.close();
}

void saveConfiguration(const char* filename, const Config& config) {
  SD.remove(filename);

  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  JsonDocument doc;

  doc["hostname"] = config.hostname;
  doc["port"] = config.port;

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  file.close();
}

void printFile(const char* filename) {
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  file.close();
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    continue;

  const int chipSelect = 4;
  while (!SD.begin(chipSelect)) {
    Serial.println(F("Failed to initialize SD library"));
    delay(1000);
  }

  Serial.println(F("Loading configuration..."));
  loadConfiguration(filename, config);

  Serial.println(F("Saving configuration..."));
  saveConfiguration(filename, config);

  Serial.println(F("Print config file..."));
  printFile(filename);
}

void loop() {
}
