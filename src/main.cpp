#include <lvgl.h>
#include "ThemeManager.h"
#include "ShotDisplay.h"
#include "ScaleLogic.h"
#include "StartupLogic.h"
#include "MachineFactory.h"
#include <vector>
#include "pins.h"
#include "secrets.h"
#include "ScreenLayout.h"
#include "SensorWidget.h"
#include "GaugeWidget.h"
#include "StatusWidget.h"
#include "BlockerWidget.h"
 
// --- Factory & Configuration ---
MachineConfig config = {
  .otaHostname = OTA_HOSTNAME,
  .wifiSsid = WIFI_SSID,
  .wifiPassword = WIFI_PASSWORD,
  .debounceMs = 150,
  .minShotDuration = 10.0
};

MachineFactory factory(config);

// --- Coordination & Logic ---
StartupLogic startupLogic(&factory);
ScaleLogic scaleLogic(factory.getPump(), factory.getShotTimer(), nullptr); 

ShotDisplay shotDisplay;
ThemeManager themeManager(&shotDisplay, factory.getButtonInput());

void setupMainDashboard() {
  shotDisplay.resetLayout(2, 2);
  ScreenLayout* layout = shotDisplay.getLayout();
  layout->addWidget(new SensorWidget(factory.getBoilerPressure()));  // Slot 0 (TL)
  layout->addWidget(new SensorWidget(factory.getBoilerTemp()));       // Slot 1 (BL)
  layout->addWidget(new StatusWidget(factory.getShotTimer()));        // Slot 2 (TR)
  layout->addWidget(new SensorWidget(factory.getShotTimer()));        // Slot 3 (BR)
}

void setup() {
  Serial.begin(115200);

  // Hardware Pins
  pinMode(backlightPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(backlightPin, HIGH);
 
  for (ITheme* theme : factory.getThemes()) {
      themeManager.addTheme(theme);
  }
 
  shotDisplay.init();
  
  // Startup Layout (1x1)
  shotDisplay.resetLayout(1, 1);
  shotDisplay.getLayout()->addWidget(new BlockerWidget(&startupLogic));
}

void loop() {
  lv_timer_handler();
  delay(5);

  // Startup Transition
  if (startupLogic.justStarted()) {
    setupMainDashboard();
  }

  // Orchestrators handle their own constituent polling
  startupLogic.update();

  // All widgets pull their own data from their assigned sensors
  shotDisplay.update();

  themeManager.update();
  scaleLogic.update();
}