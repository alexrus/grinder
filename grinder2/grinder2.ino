#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h"

const char *ssid = "Lelit PL043MMI";
const char *password = "lelitlelit";

const unsigned long resetOverrideInterval = 10000;

const int BUTTON_PIN = 15; // connect to +
const int RELAY_PIN = 13;
const int BUZZER_PIN = 4;

unsigned long startTime = 0;
unsigned long interval = 0;
int intervals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int intervalsLength = (sizeof(intervals) / sizeof(intervals[0]));
unsigned long intervalDefault = 100; // 10 sec
long intervalOverride = 0;
long overrideStart = 0;
unsigned long intervalRun = 0;
bool running = false;

ESP8266WebServer server(80); //Server on port 80

void beep(int beep_times, int beep_time) {
  for (int i = 0; i < beep_times; i++)  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beep_time);
    digitalWrite(BUZZER_PIN, LOW);
    delay(beep_time);
  }
  Serial.print("BEEP: ");
  Serial.println(beep_times);
}

void handleRoot() {
  String s = MAIN_page;

  for (int i = 1; i < intervalsLength; i++) {
    s.replace("__coffe" + String(i) + "__", String((float)intervals[i] / (float)1000));
    s.replace("__checked" + String(i) + "__", intervals[0] == i ? "checked" : "");
  }

  server.send(200, "text/html", s);
}

void handleSubmit() {
  if (int(server.arg("selectedcoffe").toInt()) > 0) {
    EEPROM.write(0, int(server.arg("selectedcoffe").toInt()));
  }

  for (int i = 1; i < intervalsLength; i++) {
    EEPROM.write(i, int(server.arg("coffe" + String(i)).toFloat() * 10));
  }

  EEPROM.commit();

  getSelectedInterval();

  for (int i = 1; i < intervalsLength; i++) {
    getInterval(i);
  }

  handleRoot();
  beep(4, 150);
}

void handleGrinder() {
  // if button pressed
  if (digitalRead(BUTTON_PIN) == HIGH)  {
    if (!running)    {
      startTime = millis();
      running = true;
      // turn on relay
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("START");
    } else {
      if (intervalOverride > 0)      {
        // if there is a leftover timeout use that
        interval = intervalOverride;
      } else {
        interval = intervals[intervals[0]];
      }
      // if timer passed
      if (startTime + interval <= millis() && digitalRead(RELAY_PIN) == LOW)      {
        digitalWrite(RELAY_PIN, HIGH);
        Serial.print("AUTO STOP at ");
        Serial.println(interval);

        beep(3, 250);
      }
    }
  }  else  {
    // if button was just released
    if (running)    {
      running = false;
      digitalWrite(RELAY_PIN, HIGH);
      overrideStart = millis();
      intervalRun = millis() - startTime;
      intervalOverride = interval - intervalRun;
      if (intervalOverride < 0)      {
        intervalOverride = 0;
      }
      Serial.print("STOP after ");
      Serial.println(intervalRun);
      if (intervalOverride > 0)      {
        Serial.print("Still need to run for ");
        Serial.println(intervalOverride);
      }
    } else {
      // reset intervalOverride to 0
      if (intervalOverride > 0 && overrideStart + resetOverrideInterval <= millis())      {
        intervalOverride = 0;
        Serial.println("Reset override after 10000");
        beep(1, 250);
      }
    }
  }
}

void getSelectedInterval() {
  intervals[0] = EEPROM.read(0);
  if (intervals[0] > intervalsLength - 1) {
    intervals[0] = 0;
  }
  Serial.print("selected Interval set to : ");
  Serial.println(intervals[0]);
}

void getInterval(int num) {
  intervals[num] = EEPROM.read(num);
  if (intervals[num] == 0)  {
    intervals[num] = intervalDefault;
  }
  intervals[num] = intervals[num] * 100;
  Serial.print("Interval ");
  Serial.print(num);
  Serial.print(" set to : ");
  Serial.println(intervals[num]);
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);

  getSelectedInterval();
  for (int i = 1; i < intervalsLength; i++) {
    getInterval(i);
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);

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
  handleGrinder();
  server.handleClient();
}

// config if app should be ap or client
