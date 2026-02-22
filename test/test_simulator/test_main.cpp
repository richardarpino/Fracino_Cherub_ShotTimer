#include <unity.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "HeadlessDriver.h"
#include "DefaultTheme.h"
#include "CandyTheme.h"
#include "ChristmasTheme.h"
#include "../../test/_common/MockRawSource.h"
#include "../../test/_common/stubs/Arduino.cpp"
#include "../../test/_common/stubs/WiFi.cpp"

// Implementation files
#include "../../lib/Sensors/Hardware/ShotTimer.cpp"
#include "../../lib/Sensors/Hardware/WiFiSensor.cpp"
#include "../../lib/UI/StatusWidget.cpp"
#include "../../lib/UI/SensorWidget.cpp"

// Headers for header-only sensors/classes
#include "Hardware/BoilerPressure.h"
#include "Hardware/WeightSensor.h"
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>
#include <iomanip>

void setUp(void) {}
void tearDown(void) {}

struct ThemeInfo {
    std::string name;
    ITheme* theme;
};

struct SensorInfo {
    std::string name;
    ISensor* sensor;
};

struct WidgetInfo {
    std::string name;
    std::function<IWidget*(ISensor*)> create;
};

struct SensorDocs {
    std::string name;
    std::string description;
    SensorMetadata metadata;
};

std::string to_lower(std::string data) {
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return data;
}

void ensure_dir(const std::string& path) {
    mkdir(path.c_str(), 0777);
}

void test_generate_examples() {
    HeadlessDriver::init(240, 135);
    
    std::vector<ThemeInfo> themes = {
        {"Default", new DefaultTheme()},
        {"Candy", new CandyTheme()},
        {"Christmas", new ChristmasTheme()}
    };

    std::vector<SensorInfo> sensors = {
        {"BoilerPressure", new BoilerPressure(nullptr)},
        {"ShotTimer", new ShotTimer(10.0)},
        {"WeightSensor", new WeightSensor(nullptr, 0.001f)},
        {"WiFiSensor", new WiFiSensor()}
    };

    std::vector<WidgetInfo> widgets = {
        {"StatusWidget", [](ISensor* s) { return new StatusWidget(s); }},
        {"SensorWidget", [](ISensor* s) { return new SensorWidget(s); }}
    };

    ensure_dir("lib/Sensors/examples");
    std::vector<SensorDocs> docs;

    for (auto& sInfo : sensors) {
        std::string sensorDir = "lib/Sensors/examples/" + sInfo.name;
        ensure_dir(sensorDir);

        SensorMetadata meta = sInfo.sensor->getMetadata();
        docs.push_back({sInfo.name, "Visualizing " + sInfo.name + " data.", meta});

        std::ofstream sensorReadme(sensorDir + "/README.md");
        sensorReadme << "# " << sInfo.name << " Documentation" << "\n\n";
        sensorReadme << "Visualizing " << sInfo.name << " data." << "\n\n";

        for (auto& wInfo : widgets) {
            std::string widgetDir = sensorDir + "/" + wInfo.name;
            ensure_dir(widgetDir);

            std::cout << "Generating " << sInfo.name << " in " << wInfo.name << "..." << std::endl;

            sensorReadme << "## " << wInfo.name << "\n";
            sensorReadme << "| Theme | Zero (" << meta.low.value << ") | Max (" << meta.high.value << ") | Error |" << "\n";
            sensorReadme << "| :--- | :---: | :---: | :---: |" << "\n";

            for (auto& themeInfo : themes) {
                std::string themePrefix = to_lower(themeInfo.name);
                sensorReadme << "| " << themeInfo.name << " ";
                
                std::vector<std::pair<std::string, Reading>> states = {
                    {"zero", meta.low},
                    {"max", meta.high},
                    {"error", meta.error}
                };

                for (const auto& state : states) {
                    lv_obj_clean(lv_scr_act());
                    
                    IWidget* widget = wInfo.create(sInfo.sensor);
                    lv_obj_t* widgetObj = widget->init(lv_scr_act(), 1, 1);
                    widget->applyTheme(themeInfo.theme);
                    widget->update(state.second);
                    
                    std::string filename = widgetDir + "/" + themePrefix + "-" + state.first + ".bmp";
                    HeadlessDriver::saveSnapshot(widgetObj, filename);
                    
                    sensorReadme << "| ![" << state.first << "](" << wInfo.name << "/" << themePrefix << "-" << state.first << ".bmp) ";
                    
                    delete widget;
                }
                sensorReadme << "|" << "\n";
            }
            sensorReadme << "\n";
        }
    }

    // Top-level Gallery README
    std::ofstream galleryReadme("lib/Sensors/examples/README.md");
    galleryReadme << "# Sensor Documentation Gallery" << "\n\n";
    galleryReadme << "This directory contains automated visual documentation for all sensors in the system. Each sensor defines its own representative states (Zero, Max, Error), which are then rendered across all compatible UI widgets and themes." << "\n\n";
    galleryReadme << "## Available Sensors" << "\n\n";
    for (const auto& doc : docs) {
        galleryReadme << "- [**" << doc.name << "**](" << doc.name << "/README.md) - " << doc.description << "\n";
    }
    galleryReadme << "\n---\n*Generated by the Sensor Simulator Framework.*\n";
    
    // Cleanup
    for(auto& t : themes) delete t.theme;
    for(auto& s : sensors) delete s.sensor;
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_generate_examples);
    return UNITY_END();
}
