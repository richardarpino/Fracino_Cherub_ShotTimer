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
StartupLogic startupLogic(&factory, factory.getRegistry());
ShotMonitor shotMonitor(factory.getManualPumpTimer(), factory.getRegistry());
BoilerMonitor boilerMonitor(factory.getBoilerTemp(), factory.getRegistry());
ScaleLogic scaleLogic(factory.getTaredWeight(), factory.getRegistry()); 

ShotDisplay shotDisplay;
ThemeManager themeManager(&shotDisplay, factory.getRegistry());

void setupMainDashboard() {
  shotDisplay.resetLayout(2, 2);
  ScreenLayout* layout = shotDisplay.getLayout();

  layout->addWidget(new SensorWidget<BoilerPressureTag>()); 
  layout->addWidget(new SensorWidget<BoilerTempTag>());     
  layout->addWidget(new SensorWidget<ShotTimeTag>());       
  layout->addWidget(new SensorWidget<LastValidShotTag>());  
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

  // 1. Hardware Poll Pass - REFRESH ALL INPUTS once per frame
  // This updates the raw buffers/physical pin states
  // In our new model, the Registry handles the hardware polling!
  // Wait, does it? MachineFactory provides sensors to the Dispatcher.
  // Dispatcher::update() calls sensor->getReading() for all provided sensors.
  
  // BUT we still need to call update() on the "services" (WiFi, OTA, Warmer) 
  // because they ARE the sensors/publishers.
  
  WiFiService* wifi = factory.getWiFiSwitch();
  if (wifi) wifi->update();
  
  OTAService* ota = factory.getOTASwitch();
  if (ota) ota->update();

  WarmingUpBlocker* warmer = factory.getWarmingUpBlocker();
  if (warmer) warmer->update();

  // 2. Logic Poll Pass - FREEZE MACHINE STATE
  // This calls getReading() on all Physical Sensors (BoilerPressure, etc.) 
  // AND DigitalSensors (Pump, Buttons).
  factory.getRegistry()->update();

  // 3. Orchestrator Pass - REACT TO FROZEN STATE
  // Each orchestrator maintains its own RegistrySwitch which pulls from the above freeze.
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