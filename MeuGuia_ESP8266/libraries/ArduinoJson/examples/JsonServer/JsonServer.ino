// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows how to implement an HTTP server that sends a JSON document
// in the response.
// It uses the Ethernet library but can be easily adapted for Wifi.
//
// The JSON document contains the values of the analog and digital pins.
// It looks like that:
// {
//   "analog": [0, 76, 123, 158, 192, 205],
//   "digital": [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0]
// }
//
// https://arduinojson.org/v7/example/http-server/

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    continue;

  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }

  server.begin();

  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = server.available();

  if (!client)
    return;

  Serial.println(F("New client"));

  while (client.available())
    client.read();

  JsonDocument doc;

  JsonArray analogValues = doc["analog"].to<JsonArray>();
  for (int pin = 0; pin < 6; pin++) {
    int value = analogRead(pin);

    analogValues.add(value);
  }

  JsonArray digitalValues = doc["digital"].to<JsonArray>();
  for (int pin = 0; pin < 14; pin++) {
    int value = digitalRead(pin);

    digitalValues.add(value);
  }

  Serial.print(F("Sending: "));
  serializeJson(doc, Serial);
  Serial.println();

  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  serializeJsonPretty(doc, client);

  client.stop();
}
