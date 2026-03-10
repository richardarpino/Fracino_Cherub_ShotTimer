#ifndef WORKFLOW_SNAPSHOTTER_H
#define WORKFLOW_SNAPSHOTTER_H

#include "../../lib/Interfaces/IWorkflow.h"
#include "../../lib/Interfaces/IScreen.h"
#include "../../lib/UI/LVGLPainter.h"
#include "../../lib/Registry/ISensorRegistry.h"
#include "HeadlessDriver.h"
#include <vector>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <algorithm>

/**
 * Utility to automatically traverse workflows and capture screenshots.
 */
class WorkflowSnapshotter {
public:
    WorkflowSnapshotter(LVGLPainter* painter, ISensorRegistry* registry)
        : _painter(painter), _registry(registry) {}

    void capture(IWorkflow* wf, const std::string& baseDir = "docs/previews") {
        if (!wf) return;

        std::string wfName = sanitize(wf->getName());
        std::string wfDir = baseDir + "/" + wfName;
        mkdir(baseDir.c_str(), 0777);
        mkdir(wfDir.c_str(), 0777);

        std::cout << "Capturing Workflow: " << wf->getName() << " -> " << wfDir << std::endl;

        int screenIndex = 0;
        while (!wf->isFinished()) {
            IScreen* screen = wf->getActiveScreen();
            if (screen) {
                std::string screenName = sanitize(screen->getName());
                char filename[256];
                snprintf(filename, sizeof(filename), "%02d-%s.bmp", screenIndex, screenName.c_str());
                std::string fullPath = wfDir + "/" + filename;

                _painter->draw(screen->getComposition(), _registry);
                
                // Allow LVGL to process layout and widget updates
                lv_timer_handler();
                addHardwareTime(50);
                lv_timer_handler();

                HeadlessDriver::saveSnapshot(_painter->getLayout()->getGrid(), fullPath);
                
                std::cout << "  [+] Captured Screen: " << screen->getName() << " to " << filename << std::endl;
            }
            wf->next();
            screenIndex++;
        }
    }

private:
    std::string sanitize(std::string data) {
        std::string result = data;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        std::replace(result.begin(), result.end(), ' ', '-');
        // Remove special characters that might be invalid in paths
        result.erase(std::remove_if(result.begin(), result.end(), [](char c) {
            return !std::isalnum(c) && c != '-';
        }), result.end());
        return result;
    }

    LVGLPainter* _painter;
    ISensorRegistry* _registry;
};

#endif
