#include "../lib/Themes/CandyTheme.h"
#include "../lib/Themes/ChristmasTheme.h"
#include "../lib/Themes/DefaultTheme.h"
#include "ShotDisplay.h"
#include "ShotTimer.h"
#include "BoilerPressure.h"
#include <ArduinoOTA.h>
#include <WiFi.h>
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
ShotTimer shotTimer(DEBOUNCE_MS, MIN_SHOT_SECONDS);
BoilerPressure boilerPressure;

DefaultTheme defaultTheme;
// ... (rest of theme definitions)
CandyTheme candyTheme;
ChristmasTheme christmasTheme;

ITheme *themes[] = {&defaultTheme, &candyTheme, &christmasTheme};
const int numThemes = 3;
int currentThemeIndex = 0;

// Pass the initial theme
ShotDisplay shotDisplay(themes[currentThemeIndex]);

unsigned long lastButtonPress = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pumpPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP); // Active LOW
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH);

  // Initialize Pressure Sensor
  boilerPressure.begin(pressurePin);

  shotDisplay.init();
  shotDisplay.showInfo("CONNECTING...", ssid);

  // WiFi Deep Clean
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
  shotDisplay.updateTimer(0.0);
  shotDisplay.updateBoilerState(boilerPressure.getPressure(), boilerPressure.getEstimatedTemperature());
}

void loop() {
  ArduinoOTA.handle();

  unsigned long now = millis();

  // --- Theme Switching Logic ---
  if (digitalRead(buttonPin) == LOW) {
    if (now - lastButtonPress > 250) { // Simple debounce
      currentThemeIndex = (currentThemeIndex + 1) % numThemes;
      shotDisplay.setTheme(themes[currentThemeIndex]);
      lastButtonPress = now;

      // Force refresh if we are in READY state, as setTheme clears the screen
      if (shotTimer.getState() == TIMER_READY) {
        shotDisplay.clearScreen();
        shotDisplay.updateTimer(0.0);
        shotDisplay.updateBoilerState(boilerPressure.getPressure(), boilerPressure.getEstimatedTemperature());
      }
    }
  }

  // --- Timer Logic ---
  bool isPumpActive = (digitalRead(pumpPin) == LOW);

  TimerState prevState = shotTimer.getState();
  shotTimer.update(isPumpActive, now);
  TimerState currentState = shotTimer.getState();

  // Handle Display transitions
  if (prevState != currentState) {
    if (currentState == TIMER_RUNNING) {
      shotDisplay.clearScreen();
    } else if (currentState == TIMER_READY) {
      shotDisplay.clearScreen();
      shotDisplay.updateTimer(0.0);
      shotDisplay.updateBoilerState(boilerPressure.getPressure(), boilerPressure.getEstimatedTemperature());
    }
  }

  // Update Timer Display
  if (currentState == TIMER_RUNNING || currentState == TIMER_FINISHED) {
    shotDisplay.updateTimer(shotTimer.getCurrentTime());
  }

  // Always update pressure display
  shotDisplay.updateBoilerState(boilerPressure.getPressure(), boilerPressure.getEstimatedTemperature());
}