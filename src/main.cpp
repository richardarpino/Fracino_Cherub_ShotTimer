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
ScaleLogic scaleLogic(&pumpSw, &shotTimer, nullptr); // Scale hardware coming soon

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

  themeManager.addTheme(&defaultTheme);
  themeManager.addTheme(&candyTheme);
  themeManager.addTheme(&christmasTheme);

  shotDisplay.init();
  
  // Register Widgets (Late-Parenting: 0=TL, 1=BL, 2=TR, 3=BR)
  ScreenLayout* layout = shotDisplay.getLayout();
  layout->addWidget(new SensorWidget(&boilerPressure));   // Slot 0 (TL)
  layout->addWidget(new SensorWidget(&boilerTemp));       // Slot 1 (BL)
  layout->addWidget(new StatusWidget(&shotTimer));        // Slot 2 (TR)
  layout->addWidget(new SensorWidget(&shotTimer, true));  // Slot 3 (BR)

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
  lv_timer_handler();
  delay(5);

  // Poll I/O once per logic frame
  pumpSw.update();

  // All widgets pull their own data from their assigned sensors
  shotDisplay.update();

  themeManager.update();
  scaleLogic.update();
}