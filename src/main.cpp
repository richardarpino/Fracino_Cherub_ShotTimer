#include <lvgl.h>
#include "ThemeManager.h"
#include "../lib/Themes/DefaultTheme.h"
#include "../lib/Themes/CandyTheme.h"
#include "../lib/Themes/ChristmasTheme.h"
#include "ShotDisplay.h"
#include "Hardware/ShotTimer.h"
#include "Hardware/BoilerPressure.h"
#include "ADCRawSource.h"
#include "DigitalRawSource.h"
#include "Virtual/BoilerTemperature.h"
#include "Hardware/HardwareSwitch.h"
#include "Virtual/DebouncedSwitch.h"
#include "ScaleLogic.h"
#include "Hardware/WiFiSensor.h"
#include "StartupLogic.h"
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <vector>
#include "pins.h"
#include "secrets.h"
#include "ScreenLayout.h"
#include "SensorWidget.h"
#include "StatusWidget.h"
 
 // --- Credentials ---
 const char *ssid = WIFI_SSID;
 const char *password = WIFI_PASSWORD;
 
 // --- Configuration ---
 const float MIN_SHOT_SECONDS = 10.0;
 const unsigned long DEBOUNCE_MS = 150;

// --- Objects ---
ADCRawSource pressureADC(pressurePin);
DigitalRawSource pumpInput(pumpPin);
DigitalRawSource buttonInput(buttonPin);

HardwareSwitch pumpHw(&pumpInput, true);
DebouncedSwitch pumpSw(&pumpHw, DEBOUNCE_MS);

BoilerPressure boilerPressure(&pressureADC, pressureScalar);
BoilerTemperature boilerTemp(&boilerPressure);
ShotTimer shotTimer(MIN_SHOT_SECONDS);
WiFiSensor wifiSensor;
StartupLogic startupLogic(&wifiSensor);
ScaleLogic scaleLogic(&pumpSw, &shotTimer, nullptr); // Scale hardware coming soon

DefaultTheme defaultTheme;
CandyTheme candyTheme;
ChristmasTheme christmasTheme;

ShotDisplay shotDisplay(&defaultTheme);
ThemeManager themeManager(&shotDisplay, &buttonInput);

void setupMainDashboard() {
  shotDisplay.resetLayout(2, 2);
  ScreenLayout* layout = shotDisplay.getLayout();
  layout->addWidget(new SensorWidget(&boilerPressure));   // Slot 0 (TL)
  layout->addWidget(new SensorWidget(&boilerTemp));       // Slot 1 (BL)
  layout->addWidget(new StatusWidget(&shotTimer));        // Slot 2 (TR)
  layout->addWidget(new SensorWidget(&shotTimer, true));  // Slot 3 (BR)
}

void setup() {
  Serial.begin(115200);

  // Hardware Pins
  pinMode(backlightPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(backlightPin, HIGH);

  themeManager.addTheme(&defaultTheme);
  themeManager.addTheme(&candyTheme);
  themeManager.addTheme(&christmasTheme);

  shotDisplay.init();
  
  // Startup Layout (1x1)
  shotDisplay.resetLayout(1, 1);
  shotDisplay.getLayout()->addWidget(new StatusWidget(&wifiSensor, true));

  // WiFi Setup (Non-blocking)
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, password);
}

void loop() {
  ArduinoOTA.handle();
  lv_timer_handler();
  delay(5);

  // Startup Transition
  if (startupLogic.justFinished()) {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.begin();
    setupMainDashboard();
  }

  // Poll I/O once per logic frame
  pumpSw.update();
  startupLogic.update();

  // All widgets pull their own data from their assigned sensors
  shotDisplay.update();

  themeManager.update();
  scaleLogic.update();
}