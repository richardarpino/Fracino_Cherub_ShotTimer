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
  .debounceMs = 30,
#ifdef VERBOSE_BOOT
  .verboseBoot = (VERBOSE_BOOT == 1)
#else
  .verboseBoot = false
#endif
};

MachineFactory* factory = nullptr;

#include "LVGLPainter.h"
#include "../lib/Factories/LVGLWidgetFactory.h"

// --- Coordination & Logic ---
ShotDisplay shotDisplay;
ThemeManager* themeManager = nullptr;
WorkflowEngine* workflowEngine = nullptr;
LVGLPainter lvglPainter;

void setup() {
  Serial.begin(115200);
  Serial.println("--- BOOTING FRACINO CHERUB SHOT TIMER ---");

  factory = new MachineFactory(config);

  // Hardware Pins managed by Factory
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH);
 
  themeManager = new ThemeManager(&shotDisplay, factory->getRegistry());
  for (ITheme* theme : factory->getThemes()) {
      themeManager->addTheme(theme);
  }
 
  shotDisplay.init();
  Serial.println("Display Initialized");
  
  // Get the Workflow Engine (wires up Startup and Dashboard)
  workflowEngine = factory->getWorkflowEngine();
  Serial.println("Workflow Engine Ready");
  
  // Initialize painter with widget factory from the machine factory
  lvglPainter.init(lv_scr_act(), nullptr, factory->getWidgetFactory()); 

  // Register Heartbeat to keep UI/Logic alive during blocking OTA downloads
  factory->setHeartbeat([]() {
    static IScreen* lastScreenH = nullptr;
    if (workflowEngine) {
        // Sync UI with active workflow
        IScreen* activeScreen = workflowEngine->getActiveScreen();
        if (activeScreen) {
            activeScreen->paint(lvglPainter);
            if (activeScreen != lastScreenH) {
                shotDisplay.setLayout(lvglPainter.getLayout());
                lastScreenH = activeScreen;
            }
        }
        workflowEngine->update();
    }
    lv_timer_handler();
    shotDisplay.update();
  });
}

void loop() {
  if (!factory) return; // Safety guard

  lv_timer_handler();
  delay(5);

  // Update network services and blockers
  factory->update();

  // 2. Registry Pass - Triggers all reactive processors
  factory->getRegistry()->update();

  // 3. Workflow & Logic Coordination
  if (workflowEngine) {
    // Sync UI with active workflow BEFORE update
    IScreen* activeScreen = workflowEngine->getActiveScreen();
    
    static IScreen* lastScreen = nullptr;
    if (activeScreen) {
        // Provide the painter to the logic screen so it can draw itself
        activeScreen->paint(lvglPainter);
        
        if (activeScreen != lastScreen) {
#ifdef VERBOSE_BOOT
            Serial.print("[UI] Screen Switch: ");
            Serial.print(lastScreen ? lastScreen->getName() : "NONE");
            Serial.print(" -> ");
            Serial.println(activeScreen->getName());
#endif
            shotDisplay.setLayout(lvglPainter.getLayout());
            lastScreen = activeScreen;
        }
    }

    workflowEngine->update();
  }

  if (themeManager) {
    themeManager->update();
  }

  // 4. UI Pass - RENDER STATE
  shotDisplay.update();
}