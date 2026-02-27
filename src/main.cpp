#include <lvgl.h>
#include "ThemeManager.h"
#include "ShotDisplay.h"
#include "ScaleLogic.h"
#include "ShotMonitor.h"
#include "BoilerMonitor.h"
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
StartupLogic startupLogic(&factory);
ShotMonitor shotMonitor(factory.getPump(), factory.getManualPumpTimer(), factory.getRegistry());
BoilerMonitor boilerMonitor(factory.getBoilerTemp(), factory.getRegistry());
ScaleLogic scaleLogic(factory.getPump(), factory.getTaredWeight(), factory.getRegistry()); 

ShotDisplay shotDisplay;
ThemeManager themeManager(&shotDisplay, factory.getButtonRight());

void setupMainDashboard() {
  shotDisplay.resetLayout(2, 2);
  ScreenLayout* layout = shotDisplay.getLayout();

  layout->addWidget(new SensorWidget<BoilerPressureTag>()); 
  layout->addWidget(new SensorWidget<BoilerTempTag>());     
  layout->addWidget(new StatusWidget<ShotTimeTag>());       
  layout->addWidget(new SensorWidget<LastValidShotTag>());  // Restored business requirement
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
  startupLogic.update(); // Initial poll
}

void loop() {
  lv_timer_handler();
  delay(5);

  // 1. Hardware Poll Pass - REFRESH ALL INPUTS ONCE PER FRAME
  factory.getPump()->update();
  factory.getButtonLeft()->update();
  factory.getButtonRight()->update();
  
  WiFiService* wifi = factory.getWiFiSwitch();
  if (wifi) wifi->update();
  
  OTAService* ota = factory.getOTASwitch();
  if (ota) ota->update();

  WarmingUpBlocker* warmer = factory.getWarmingUpBlocker();
  if (warmer) warmer->update();

  // 2. Logic Poll Pass - FREEZE MACHINE STATE
  factory.getRegistry()->update();

  // 3. Orchestrator Pass - REACT TO FROZEN STATE
  startupLogic.update();
  if (startupLogic.justStarted()) {
    setupMainDashboard();
  }

  themeManager.update();
  shotMonitor.update();
  boilerMonitor.update();
  scaleLogic.update();

  // 4. UI Pass - RENDER STATE
  shotDisplay.update();
}