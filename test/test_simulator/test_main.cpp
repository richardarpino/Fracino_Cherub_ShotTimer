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
#include "../../lib/Services/WiFiService.cpp"
#include "../../lib/Services/OTAService.cpp"
#include "../../lib/Services/WarmingUpBlocker.cpp"
#include "../../lib/UI/StatusWidget.cpp"
#include "../../lib/UI/SensorWidget.cpp"
#include "../../lib/UI/GaugeWidget.cpp"
#include "../../lib/UI/BlockerWidget.cpp"

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

struct BlockerInfo {
    std::string name;
    IBlocker* blocker;
    struct State {
        std::string name;
        std::string title;
        std::string message;
        float progress;
        bool failed;
    };
    std::vector<State> states;
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
        {"TaredWeight", new TaredWeight(ws)}
    };

    std::vector<WidgetInfo> widgets = {
        {"SensorWidget", [](ISensor* s) { return new SensorWidget(s); }},
        {"GaugeWidget", [](ISensor* s) { return new GaugeWidget(s); }}
    };

    std::vector<BlockerInfo> blockers = {
        {
            "WiFiService", 
            new WiFiService(),
            {
                {"Connecting", "WiFi", "CONNECTING...", -1.0f, false},
                {"Connected", "WiFi", "CONNECTED: 192.168.1.50", 100.0f, false},
                {"Failed", "WiFi", "CONNECTION FAILED", -1.0f, true}
            }
        },
        {
            "OTAService",
            new OTAService("test"),
            {
                {"Ready", "OTA Update", "LISTENING...", 100.0f, false},
                {"Updating", "OTA Update", "UPDATING: 42%", 42.0f, false},
                {"Error", "OTA Update", "UPDATE FAILED", 0.0f, true}
            }
        },
        {
            "WarmingUpBlocker",
            nullptr,
            {
                {"Stage1", "Warming Up...", "Heating Cycle 1, currently 0.1bar", 5.0f, false},
                {"Stage2", "Warming Up...", "Heating Cycle 2, currently 0.8bar", 45.0f, false},
                {"Warm", "Warming Up...", "WARM", 100.0f, false}
            }
        }
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
        {"1x2", (uint32_t)TFT_HEIGHT / 2, (uint32_t)TFT_WIDTH, 2, 1},
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
                    
                    // Create parent of FULL grid size to force correct internal scaling math
                    lv_obj_t* parent = lv_obj_create(lv_scr_act());
                    lv_obj_set_size(parent, 240, 135); 
                    lv_obj_set_style_pad_all(parent, 0, 0);
                    lv_obj_set_style_border_width(parent, 0, 0);
                    lv_obj_set_style_radius(parent, 0, 0);

                    IWidget* widget = wInfo.create(sInfo.sensor);
                    lv_obj_t* root = widget->init(parent, sizes[0].cols, sizes[0].rows);
                    widget->applyTheme(themeInfo.theme);
                    widget->update(state.second);
                    
                    // Force the widget root to fill only the target cell area for snapshot
                    lv_obj_set_size(root, 240, 135); 
                    
                    std::string imgName = to_lower(sInfo.name + "-" + wInfo.name + "-" + themeInfo.name + "-" + state.first + ".bmp");
                    std::string fullPath = "lib/Sensors/examples/1x1/" + imgName;
                    HeadlessDriver::saveSnapshot(root, fullPath); // Target the widget root
                    
                    sensorReadme << "| ![" << state.first << "](../1x1/" << imgName << ") ";
                    delete widget;
                }

                // Scaled Previews (Init state only)
                sensorReadme << "| ";
                for (size_t i = 1; i < sizes.size(); ++i) {
                    const auto& size = sizes[i];
                    lv_obj_clean(lv_scr_act());

                    // Create parent of FULL grid size to force correct internal scaling math
                    lv_obj_t* parent = lv_obj_create(lv_scr_act());
                    lv_obj_set_size(parent, 240, 135);
                    lv_obj_set_style_pad_all(parent, 0, 0);
                    lv_obj_set_style_border_width(parent, 0, 0);
                    lv_obj_set_style_radius(parent, 0, 0);

                    IWidget* widget = wInfo.create(sInfo.sensor);
                    lv_obj_t* root = widget->init(parent, size.cols, size.rows);
                    widget->applyTheme(themeInfo.theme);
                    widget->update(meta.init);

                    // Force the widget root to fill only the target cell area for snapshot
                    lv_obj_set_size(root, size.w, size.h);

                    std::string imgName = to_lower(sInfo.name + "-" + wInfo.name + "-" + themeInfo.name + "-init.bmp");
                    std::string fullPath = "lib/Sensors/examples/" + size.name + "/" + imgName;
                    HeadlessDriver::saveSnapshot(root, fullPath); // Target the widget root

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
    galleryReadme << "# Documentation Gallery" << "\n\n";
    galleryReadme << "This directory contains automated visual documentation for all UI components in the system." << "\n\n";
    
    galleryReadme << "## Performance Dashboard (Sensors)" << "\n";
    galleryReadme << "Continuous data streams visualized across different widgets and themes." << "\n\n";
    for (const auto& doc : docs) {
        galleryReadme << "- [**" << doc.name << "**](" << doc.name << "/README.md) - " << doc.description << "\n";
    }

    galleryReadme << "\n## System Services (Blockers)" << "\n";
    galleryReadme << "Procedural services that manage system state and connectivity." << "\n\n";

    for (auto& bInfo : blockers) {
        std::string blockerDir = "lib/Sensors/examples/" + bInfo.name;
        ensure_dir(blockerDir);
        galleryReadme << "- [**" << bInfo.name << "**](" << bInfo.name << "/README.md)\n";

        std::ofstream blockerReadme(blockerDir + "/README.md");
        blockerReadme << "# " << bInfo.name << " Documentation" << "\n\n";
        
        blockerReadme << "## BlockerWidget" << "\n";
        blockerReadme << "| Theme | " << bInfo.states[0].name << " | " << bInfo.states[1].name << " | " << bInfo.states[2].name << " |" << "\n";
        blockerReadme << "| :--- | :---: | :---: | :---: |" << "\n";

        for (auto& themeInfo : themes) {
            blockerReadme << "| " << themeInfo.name << " ";
            for (const auto& state : bInfo.states) {
                lv_obj_clean(lv_scr_act());
                lv_obj_t* parent = lv_obj_create(lv_scr_act());
                lv_obj_set_size(parent, 240, 135);
                
                // We use a mock here since we can't easily drive the real hardware state in simulator loop for these specific labels
                BlockerWidget* widget = new BlockerWidget(nullptr); 
                lv_obj_t* root = widget->init(parent, 1, 1);
                widget->applyTheme(themeInfo.theme);
                
                // Manual drive for gallery purposes
                lv_obj_t* title_label = lv_obj_get_child(root, 0);
                lv_obj_t* status_label = lv_obj_get_child(root, 1);
                lv_obj_t* bar = lv_obj_get_child(root, 2);
                
                lv_label_set_text(title_label, state.title.c_str());
                lv_label_set_text(status_label, state.message.c_str());
                
                if (state.progress >= 0) {
                    lv_obj_clear_flag(bar, LV_OBJ_FLAG_HIDDEN);
                    lv_bar_set_value(bar, (int32_t)state.progress, LV_ANIM_OFF);
                } else {
                    lv_obj_add_flag(bar, LV_OBJ_FLAG_HIDDEN);
                }

                if (state.failed) {
                    // Force alert style (manually mimicking BlockerWidget::refresh logic)
                    lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_RED), 0);
                    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
                    lv_obj_set_style_text_color(status_label, lv_color_white(), 0);
                }

                std::string imgName = to_lower(bInfo.name + "-blocker-" + themeInfo.name + "-" + state.name + ".bmp");
                std::string fullPath = "lib/Sensors/examples/1x1/" + imgName;
                HeadlessDriver::saveSnapshot(root, fullPath);
                
                blockerReadme << "| ![" << state.name << "](../1x1/" << imgName << ") ";
                delete widget;
            }
            blockerReadme << "|" << "\n";
        }
    }

    galleryReadme << "\n---\n*Generated by the Sensor Simulator Framework.*\n";
    
    // Cleanup
    for(auto& t : themes) delete t.theme;
    for(auto& s : sensors) delete s.sensor;
    for(auto& b : blockers) delete b.blocker;
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_generate_examples);
    return UNITY_END();
}
