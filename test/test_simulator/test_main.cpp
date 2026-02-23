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
#include "../../lib/UI/GaugeWidget.cpp"

// Headers for header-only sensors/classes
#include "Hardware/BoilerPressure.h"
#include "Hardware/WeightSensor.h"
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>
#include <iomanip>

// Headers for virtual sensors
#include "../../lib/Sensors/Virtual/BoilerTemperature.h"
#include "../../lib/Sensors/Virtual/TaredWeight.h"

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

    BoilerPressure* bp = new BoilerPressure(nullptr);
    WeightSensor* ws = new WeightSensor(nullptr, 0.001f);

    std::vector<SensorInfo> sensors = {
        {"BoilerPressure", bp},
        {"BoilerTemperature", new BoilerTemperature(bp)},
        {"ShotTimer", new ShotTimer(10.0)},
        {"WeightSensor", ws},
        {"TaredWeight", new TaredWeight(ws)},
        {"WiFiSensor", new WiFiSensor()}
    };

    std::vector<WidgetInfo> widgets = {
        {"StatusWidget", [](ISensor* s) { return new StatusWidget(s); }},
        {"SensorWidget", [](ISensor* s) { return new SensorWidget(s); }},
        {"GaugeWidget", [](ISensor* s) { return new GaugeWidget(s); }}
    };

    ensure_dir("lib/Sensors/examples");
    ensure_dir("lib/Sensors/examples/1x1");
    ensure_dir("lib/Sensors/examples/1x2");
    ensure_dir("lib/Sensors/examples/2x2");
    
    std::vector<SensorDocs> docs;

    struct DisplaySize {
        std::string name;
        uint32_t w;
        uint32_t h;
        uint8_t cols;
        uint8_t rows;
    };

    std::vector<DisplaySize> sizes = {
        {"1x1", (uint32_t)TFT_HEIGHT, (uint32_t)TFT_WIDTH, 1, 1},
        {"1x2", (uint32_t)TFT_HEIGHT, (uint32_t)TFT_WIDTH / 2, 1, 2},
        {"2x2", (uint32_t)TFT_HEIGHT / 2, (uint32_t)TFT_WIDTH / 2, 2, 2}
    };

    HeadlessDriver::init(320, 320);

    for (auto& sInfo : sensors) {
        std::string sensorDir = "lib/Sensors/examples/" + sInfo.name;
        ensure_dir(sensorDir);

        SensorMetadata meta = sInfo.sensor->getMetadata();
        docs.push_back({sInfo.name, "Visualizing " + sInfo.name + " data.", meta});

        std::ofstream sensorReadme(sensorDir + "/README.md");
        sensorReadme << "# " << sInfo.name << " Documentation" << "\n\n";
        sensorReadme << "Visualizing " << sInfo.name << " data." << "\n\n";

        for (auto& wInfo : widgets) {
            std::cout << "Generating " << sInfo.name << " in " << wInfo.name << "..." << std::endl;

            sensorReadme << "## " << wInfo.name << "\n";
            sensorReadme << "| Theme | Low (" << meta.low.value << ") | Init (" << meta.init.value << ") | High (" << meta.high.value << ") | Error | Scaled (Init) |" << "\n";
            sensorReadme << "| :--- | :---: | :---: | :---: | :---: | :---: |" << "\n";

            for (auto& themeInfo : themes) {
                sensorReadme << "| " << themeInfo.name << " ";
                
                // 1x1 States (Full set)
                std::vector<std::pair<std::string, Reading>> states = {
                    {"low", meta.low},
                    {"init", meta.init},
                    {"high", meta.high},
                    {"error", meta.error}
                };

                for (const auto& state : states) {
                    lv_obj_clean(lv_scr_act());
                    
                    // Create parent of target size to force widget scaling
                    lv_obj_t* parent = lv_obj_create(lv_scr_act());
                    lv_obj_set_size(parent, 240, 135);
                    lv_obj_set_style_pad_all(parent, 0, 0);
                    lv_obj_set_style_border_width(parent, 0, 0);
                    lv_obj_set_style_radius(parent, 0, 0);

                    IWidget* widget = wInfo.create(sInfo.sensor);
                    lv_obj_t* widgetObj = widget->init(parent, 1, 1);
                    widget->applyTheme(themeInfo.theme);
                    widget->update(state.second);
                    
                    std::string imgName = to_lower(sInfo.name + "-" + wInfo.name + "-" + themeInfo.name + "-" + state.first + ".bmp");
                    std::string fullPath = "lib/Sensors/examples/1x1/" + imgName;
                    HeadlessDriver::saveSnapshot(parent, fullPath);
                    
                    sensorReadme << "| ![" << state.first << "](../1x1/" << imgName << ") ";
                    delete widget;
                }

                // Scaled Previews (Init state only)
                sensorReadme << "| ";
                for (size_t i = 1; i < sizes.size(); ++i) {
                    const auto& size = sizes[i];
                    lv_obj_clean(lv_scr_act());

                    // Create parent of target size to force widget scaling
                    lv_obj_t* parent = lv_obj_create(lv_scr_act());
                    lv_obj_set_size(parent, size.w, size.h);
                    lv_obj_set_style_pad_all(parent, 0, 0);
                    lv_obj_set_style_border_width(parent, 0, 0);
                    lv_obj_set_style_radius(parent, 0, 0);

                    IWidget* widget = wInfo.create(sInfo.sensor);
                    lv_obj_t* widgetObj = widget->init(parent, size.cols, size.rows);
                    widget->applyTheme(themeInfo.theme);
                    widget->update(meta.init);

                    std::string imgName = to_lower(sInfo.name + "-" + wInfo.name + "-" + themeInfo.name + "-init.bmp");
                    std::string fullPath = "lib/Sensors/examples/" + size.name + "/" + imgName;
                    HeadlessDriver::saveSnapshot(parent, fullPath);

                    sensorReadme << "![" << size.name << "](../" << size.name << "/" << imgName << ") ";
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
