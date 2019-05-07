#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h" //Our HTML webpage contents

const char* ssid = "Lelit PL043MMI";
const char* password = "lelitlelit";

const unsigned long resetOverrideInterval = 10000;

const int BUTTON_PIN = 15; // connect to +
const int SELECTOR_PIN = 12; // connect to gnd
const int RELAY_PIN = 13;
const int BUZZER_PIN = 4;

unsigned long startTime = 0;
unsigned long interval = 0;
unsigned long interval1 = 0; // will read from eeprom
unsigned long interval2 = 0; // will read from eeprom
unsigned long interval1Default = 50; // 5 sec
unsigned long interval2Default = 75; // 7.5 sec
long intervalOverride = 0;
long overrideStart = 0;
unsigned long intervalRun = 0;
bool running = false;

ESP8266WebServer server(80); //Server on port 80

void beep(int beep_times, int beep_time) {
  for (int i = 0; i < beep_times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beep_time);
    digitalWrite(BUZZER_PIN, LOW);
    delay(beep_time);
  }
}

void handleRoot() {
  String s = MAIN_page;
  s.replace("__message__", "");
  s.replace("__coffe1__", String((float)interval1 / (float)1000));
  s.replace("__coffe2__", String((float)interval2 / (float)1000));
  server.send(200, "text/html", s);
}

void handleSubmit() {
  EEPROM.write(0, int(server.arg("coffe1").toFloat() * 10));
  EEPROM.write(1, int(server.arg("coffe2").toFloat() * 10));
  EEPROM.commit();

  interval1 = int(server.arg("coffe1").toFloat() * 1000);
  Serial.print("Interval 1 set to : ");
  Serial.println(interval1);

  interval2 = int(server.arg("coffe2").toFloat() * 1000);
  Serial.print("Interval 2 set to : ");
  Serial.println(interval2);

  String s = MAIN_page;
  s.replace("__message__", "Saved");
  s.replace("__coffe1__", String((float)interval1 / (float)1000));
  s.replace("__coffe2__", String((float)interval2 / (float)1000));
  server.send(200, "text/html", s);
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);

  interval1 = EEPROM.read(0);
  if (interval1 == 0) {
    interval1 = interval1Default;
  }
  interval2 = EEPROM.read(1);
  if (interval2 == 0) {
    interval2 = interval2Default;
  }

  interval1 = interval1 * 100;
  Serial.print("Interval 1 set to : ");
  Serial.println(interval1);

  interval2 = interval2 * 100;
  Serial.print("Interval 2 set to : ");
  Serial.println(interval2);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(SELECTOR_PIN, INPUT_PULLUP);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP:");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);

  server.begin();
  Serial.println("HTTP server started");

  beep(2, 150);
}

void loop() {
  // if button pressed
  if ( digitalRead(BUTTON_PIN) == HIGH ) {
    if (!running) {
      startTime = millis();
      running = true;
      // turn on relay
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("START");
    } else {
      if (intervalOverride > 0) {
        // if there is a leftover timeout use that
        interval = intervalOverride;
      } else {
        // check if should grind 1 or 2 coffe
        if ( digitalRead(SELECTOR_PIN) == LOW ) {
          interval = interval1;
        } else {
          interval = interval2;
        }
      }
      // if timer passed
      if (startTime + interval <= millis() && digitalRead(RELAY_PIN) == LOW)  {
        digitalWrite(RELAY_PIN, HIGH);
        Serial.print("AUTO STOP at ");
        Serial.println(interval);

        beep(3, 250);
      }
    }
  } else {
    // if button was just released
    if (running) {
      running = false;
      digitalWrite(RELAY_PIN, HIGH);
      overrideStart = millis();
      intervalRun = millis() - startTime;
      intervalOverride = interval - intervalRun;
      if (intervalOverride < 0) {
        intervalOverride = 0;
      }
      Serial.print("STOP after ");
      Serial.println(intervalRun );
      if (intervalOverride > 0) {
        Serial.print("Still need to run for ");
        Serial.println(intervalOverride);
      }
    } else {
      // reset intervalOverride to 0
      if (intervalOverride > 0 && overrideStart + resetOverrideInterval <= millis())  {
        intervalOverride = 0;
        Serial.println("Reset override after 10000");
        beep(1, 250);
      }
    }
  }
  server.handleClient();
}
