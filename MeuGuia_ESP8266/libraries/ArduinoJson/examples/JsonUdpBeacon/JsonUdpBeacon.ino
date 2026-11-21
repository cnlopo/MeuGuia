// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License
//
// This example shows how to send a JSON document to a UDP socket.
// At regular interval, it sends a UDP packet that contains the status of
// analog and digital pins.
// It looks like that:
// {
//   "analog": [0, 76, 123, 158, 192, 205],
//   "digital": [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0]
// }
//
// If you want to test this program, you need to be able to receive the UDP
// packets.
// For example, you can run netcat on your computer
// $ ncat -ulp 8888
// See https://nmap.org/ncat/
//
// https://arduinojson.org/v7/example/udp-beacon/

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress remoteIp(192, 168, 0, 108);
unsigned short remotePort = 8888;
unsigned short localPort = 8888;
EthernetUDP udp;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    continue;

  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }

  udp.begin(localPort);
}

void loop() {
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

  Serial.print(F("Sending to "));
  Serial.print(remoteIp);
  Serial.print(F(" on port "));
  Serial.println(remotePort);
  serializeJson(doc, Serial);

  udp.beginPacket(remoteIp, remotePort);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();

  delay(10000);
}
