#include <lvgl.h>
#include "ThemeManager.h"
#include "ShotDisplay.h"
#include "MachineFactory.h"
#include <vector>
#include "pins.h"
#include "secrets.h"

// --- Factory & Configuration ---
MachineConfig config = {
  .otaHostname = OTA_HOSTNAME,
  .wifiSsid = WIFI_SSID,
  .wifiPassword = WIFI_PASSWORD,
  .debounceMs = 30
};

MachineFactory factory(config);

// --- Coordination & Logic ---
ShotDisplay shotDisplay;
ThemeManager themeManager(&shotDisplay, factory.getRegistry());
WorkflowEngine* workflowEngine = nullptr;
ScreenLayout* lastLayout = nullptr;

void setup() {
  Serial.begin(115200);

  // Hardware Pins managed by Factory
  digitalWrite(backlightPin, HIGH);
 
  for (ITheme* theme : factory.getThemes()) {
      themeManager.addTheme(theme);
  }
 
  shotDisplay.init();
  Serial.println("Display Initialized");
  
  // Get the Workflow Engine (wires up Startup and Dashboard)
  workflowEngine = factory.getWorkflowEngine();
  Serial.println("Workflow Engine Ready");
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

  // 3. Workflow & Logic Coordination
  if (workflowEngine) {
    // Sync UI with active workflow BEFORE update
    IScreen* activeScreen = workflowEngine->getActiveScreen();
    if (activeScreen && activeScreen->getLayout() != lastLayout) {
      Serial.println("Switching Screen Layout...");
      shotDisplay.setLayout(activeScreen->getLayout());
      lastLayout = activeScreen->getLayout();
    }

    workflowEngine->update();
  }

  themeManager.update();

  // 4. UI Pass - RENDER STATE
  shotDisplay.update();
}