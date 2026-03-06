#include <lvgl.h>
#include "ThemeManager.h"
#include "ShotDisplay.h"
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
  .debounceMs = 150
};

MachineFactory factory(config);

// --- Coordination & Logic ---
StartupLogic startupLogic(&factory, factory.getRegistry());

ShotDisplay shotDisplay;
ThemeManager themeManager(&shotDisplay, factory.getRegistry());

void setupMainDashboard() {
  shotDisplay.resetLayout(2, 2);
  ScreenLayout* layout = shotDisplay.getLayout();

  layout->addWidget(new SensorWidget<BoilerPressureReading>()); 
  layout->addWidget(new SensorWidget<BoilerTempReading>());     
  layout->addWidget(new SensorWidget<ShotTimeReading>());       
  layout->addWidget(new SensorWidget<LastValidShotReading>());  
}

void setup() {
  Serial.begin(115200);

  // Hardware Pins managed by Factory
  digitalWrite(backlightPin, HIGH);
 
  for (ITheme* theme : factory.getThemes()) {
      themeManager.addTheme(theme);
  }
 
  shotDisplay.init();
  shotDisplay.setRegistry(factory.getRegistry());
  
  // Startup Layout (1x1)
  shotDisplay.resetLayout(1, 1);
  shotDisplay.getLayout()->addWidget(new BlockerWidget(&startupLogic));
}

void loop() {
  lv_timer_handler();
  delay(5);

  // 1. Hardware Poll Pass
  WiFiService* wifi = factory.getWiFiSwitch();
  if (wifi) wifi->update();
  
  OTAService* ota = factory.getOTASwitch();
  if (ota) ota->update();

  WarmingUpBlocker* warmer = factory.getWarmingUpBlocker();
  if (warmer) warmer->update();

  // 2. Registry Pass - Triggers all reactive processors
  factory.getRegistry()->update();

  // 3. Logic Coordination
  startupLogic.update();
  if (startupLogic.justStarted()) {
    setupMainDashboard();
  }

  themeManager.update();

  // 4. UI Pass - RENDER STATE
  shotDisplay.update();
}