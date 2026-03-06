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
#include "../../lib/Logic/ManualPumpTimer.h"
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
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Interfaces/SensorTags.h"
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>
#include <iomanip>

// Headers for virtual sensors
#include "../../lib/Logic/BoilerTemperature.h"
#include "../../lib/Logic/TaredWeight.h"

void setUp(void) {}
void tearDown(void) {}

struct ThemeInfo {
    std::string name;
    ITheme* theme;
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

struct DocEntry {
    std::string name;
    std::function<SensorMetadata()> getMetadata;
    std::function<IWidget*()> createSensorWidget;
    std::function<IWidget*()> createGaugeWidget;
    std::function<void(SensorDispatcher*)> registerFunc;
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

template<typename Tag>
DocEntry createEntry(std::string name, HardwareSensor* hw = nullptr) {
    return {
        name,
        []() { return Tag::getMetadata(); },
        []() { return new SensorWidget<Tag>(); },
        []() { return new GaugeWidget<Tag>(); },
        [hw](SensorDispatcher* d) { if (hw) d->provide<Tag>(hw); }
    };
}

template<typename Tag>
BlockerInfo createServiceEntry(std::string name, IBlocker* b = nullptr) {
    ServiceMetadata meta = Tag::getMetadata();
    std::vector<BlockerInfo::State> states = {
        {"Pending", std::string(meta.pending.title), std::string(meta.pending.message), meta.pending.progress, meta.pending.isFailed},
        {"Active", std::string(meta.active.title), std::string(meta.active.message), meta.active.progress, meta.active.isFailed},
        {"Ready", std::string(meta.ready.title), std::string(meta.ready.message), meta.ready.progress, meta.ready.isFailed},
        {"Failed", std::string(meta.failed.title), std::string(meta.failed.message), meta.failed.progress, meta.failed.isFailed}
    };
    return { name, b, states };
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

    std::vector<DocEntry> docEntries = {
        createEntry<BoilerPressureReading>("BoilerPressure", bp),
        createEntry<BoilerTempReading>("BoilerTemperature"),
        createEntry<ShotTimeReading>("ManualPumpTimer"),
        createEntry<LastValidShotReading>("LastValidShot"),
        createEntry<WeightReading>("WeightSensor", ws),
        createEntry<TaredWeightReading>("TaredWeight")
    };

    SensorDispatcher dispatcher;

    std::vector<BlockerInfo> blockers = {
        createServiceEntry<WiFiStatus>("WiFiService", new WiFiService(&dispatcher)),
        createServiceEntry<OTAStatus>("OTAService", new OTAService(&dispatcher, "test")),
        createServiceEntry<WarmingUpStatus>("WarmingUpBlocker", nullptr)
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

    HeadlessDriver::init(320, 320);

    for (auto& entry : docEntries) {
        std::string sensorDir = "lib/Sensors/examples/" + entry.name;
        ensure_dir(sensorDir);

        SensorMetadata meta = entry.getMetadata();
        docs.push_back({entry.name, "Visualizing " + entry.name + " data.", meta});

        std::ofstream sensorReadme(sensorDir + "/README.md");
        sensorReadme << "# " << entry.name << " Documentation" << "\n\n";
        sensorReadme << "Visualizing " << entry.name << " data." << "\n\n";

        std::vector<std::pair<std::string, std::function<IWidget*()>>> widgetVariants = {
            {"SensorWidget", entry.createSensorWidget},
            {"GaugeWidget", entry.createGaugeWidget}
        };

        for (auto& wVariant : widgetVariants) {
            std::string wName = wVariant.first;
            std::cout << "Generating " << entry.name << " in " << wName << "..." << std::endl;

            sensorReadme << "## " << wName << "\n";
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

                    // Registry setup for each snapshot to ensure fresh data
                    SensorDispatcher tempDispatcher;
                    entry.registerFunc(&tempDispatcher);
                    
                    // Force the "current" reading to the one we want to snapshot
                    // Since dispatcher::update normally polls, we need to mock the registry behavior here
                    // Actually, the simplest way is to manually update the widget with state.second
                    
                    IWidget* widget = wVariant.second();
                    lv_obj_t* root = widget->init(parent, sizes[0].cols, sizes[0].rows);
                    widget->applyTheme(themeInfo.theme);
                    widget->setMetadata(meta); // Set scale range before update
                    widget->update(state.second); // Immediate update for snapshot
                    
                    // Force the widget root to fill only the target cell area for snapshot
                    lv_obj_set_size(root, 240, 135); 
                    
                    std::string imgName = to_lower(entry.name + "-" + wName + "-" + themeInfo.name + "-" + state.first + ".bmp");
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

                    IWidget* widget = wVariant.second();
                    lv_obj_t* root = widget->init(parent, size.cols, size.rows);
                    widget->applyTheme(themeInfo.theme);
                    widget->setMetadata(meta);
                    widget->update(meta.init);

                    // Force the widget root to fill only the target cell area for snapshot
                    lv_obj_set_size(root, size.w, size.h);

                    std::string imgName = to_lower(entry.name + "-" + wName + "-" + themeInfo.name + "-init.bmp");
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
        blockerReadme << "| Theme | Pending | Active | Ready | Failed |" << "\n";
        blockerReadme << "| :--- | :---: | :---: | :---: | :---: |" << "\n";

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
    delete bp;
    delete ws;
    for(auto& b : blockers) delete b.blocker;
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_generate_examples);
    return UNITY_END();
}
