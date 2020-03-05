/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Use nodemcu esp-12e, wire PIR to   GPIO 7
 *                           Relay to GPIO 6
 */
/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "SirenAccessPoint"
#define APPSK  "MySecretPassword"
#endif

/*
static const uint8_t D0 = 16;
static const uint8_t D1 = 5;
static const uint8_t D2 = 4;
static const uint8_t D3 = 0;
static const uint8_t D4 = 2;
static const uint8_t D5 = 14;
static const uint8_t D6 = 12;
static const uint8_t D7 = 13;
static const uint8_t D8 = 15;
static const uint8_t D9 = 3;
static const uint8_t D10 = 1;
*/

#define RelayPin D7 // GPIO13
#define PIRInput D6 // GPIO12
#define bleep 100 // Bleep siren
 
/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

int siren = 0; // Siren on or off
int alarm = 0; // Alarm armed or disabled
/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}
void handleArm() {
  server.send(200, "text/html", "<h1>Alarm will arm in 1 minute</h1>");
  alarm = 1;
  digitalWrite(RelayPin, 1);
  delay(bleep); // bleep siren
  digitalWrite(RelayPin, 0);
}
void handleDisable() {
  server.send(200, "text/html", "<h1>Alarm is disabled</h1>");
  alarm = 0;
}
void handleSirenOn() {
  server.send(200, "text/html", "<h1>Siren is ON!</h1>");
  siren = 1;
  digitalWrite(RelayPin, 1);
}
void handleSirenOff() {
  server.send(200, "text/html", "<h1>Siren off</h1>");
  siren = 0;
  digitalWrite(RelayPin, 0);
}

void setup() {
  pinMode(RelayPin, FUNCTION_3); // 2020-02-27 
  pinMode(RelayPin, OUTPUT); // 2020-02-25 set relay gpio to o/p
  pinMode(PIRInput, FUNCTION_3);  // 2020-02-27
  pinMode(PIRInput, INPUT);  // 2020-02-25 set PIR gpio to i/p

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password, 5); // Use channel 5

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disable", handleDisable);
  server.on("/sirenon", handleSirenOn);
  server.on("/sirenoff", handleSirenOff);
  server.begin();
  Serial.println("HTTP server started");
  
  digitalWrite(RelayPin, 1);
  delay(bleep); // loose a 5s blast of siren
  digitalWrite(RelayPin, 0);
}

int old_PIR = 0;
int PIR = 0;
void loop() {
  server.handleClient();
  PIR = digitalRead(PIRInput);
  if (PIR != old_PIR) {
    if (PIR) {
      Serial.println("PIR high");
      if (alarm) {
        Serial.println("Alarm is armed - siren on!");
        digitalWrite(RelayPin, 1);
        delay(1000); // loose a 5s blast of siren
        delay(1000);
        delay(1000);
        delay(1000);
        delay(1000);
      }  
    }   
    else {
      Serial.println("PIR low - Siren off");
      digitalWrite(RelayPin, 0);
      delay(1000);
    }
    old_PIR = PIR;
  }
}
