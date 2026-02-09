#include "ThemeManager.h"
#include "../lib/Themes/DefaultTheme.h"
#include "../lib/Themes/CandyTheme.h"
#include "../lib/Themes/ChristmasTheme.h"
#include "ShotDisplay.h"
#include "ShotTimer.h"
#include "BoilerPressure.h"
#include "ADCRawSource.h"
#include "DigitalRawSource.h"
#include "BoilerTemperature.h"
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <vector>
#include "secrets.h"

// --- Credentials ---
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// --- Pins ---
const int pumpPin = 21;
const int backlightPin = 4;
const int buttonPin = 35; // Button 1 on T-Display
const int pressurePin = 34; // ADC1 Input Only

const float MIN_SHOT_SECONDS = 10.0;
const unsigned long DEBOUNCE_MS = 150;

// --- Objects ---
ADCRawSource pressureADC(pressurePin);
DigitalRawSource pumpInput(pumpPin);
DigitalRawSource buttonInput(buttonPin);

BoilerPressure boilerPressure(&pressureADC);
BoilerTemperature boilerTemp(&boilerPressure);
ShotTimer shotTimer(&pumpInput, DEBOUNCE_MS, MIN_SHOT_SECONDS);

std::vector<ISensor*> sensors;

DefaultTheme defaultTheme;
CandyTheme candyTheme;
ChristmasTheme christmasTheme;

ShotDisplay shotDisplay(&defaultTheme);
ThemeManager themeManager(&shotDisplay, &buttonInput);

void setup() {
  Serial.begin(115200);

  // Hardware Pins
  pinMode(backlightPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(backlightPin, HIGH);

  
  sensors.push_back(&boilerPressure);
  sensors.push_back(&boilerTemp);
  sensors.push_back(&shotTimer);

  themeManager.addTheme(&defaultTheme);
  themeManager.addTheme(&candyTheme);
  themeManager.addTheme(&christmasTheme);

  shotDisplay.init();
  shotDisplay.showInfo("CONNECTING...", ssid);

  // WiFi Setup
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname("Cherub-Timer");
    ArduinoOTA.begin();
    shotDisplay.showInfo("CONNECTED", WiFi.localIP().toString());
    delay(3000);
  } else {
    shotDisplay.showInfo("WIFI STATUS", "FAILED");
    delay(2000);
  }

  shotDisplay.clearScreen();
}

void loop() {
  ArduinoOTA.handle();

  // 1. Update Display & Sample Sensors generically for all measurements
  for (auto* sensor : sensors) {
    shotDisplay.update(sensor->getReading());
  }

  // 2. Encapsulated Theme Switching Logic
  themeManager.update();
}