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

// Examples
#include "IExample.h"
#include "StatusWidget/StatusWidgetExample.h"
#include "SensorWidget/SensorWidgetExample.h"

void setUp(void) {}
void tearDown(void) {}

struct ThemeInfo {
    std::string name;
    ITheme* theme;
};

std::string to_lower(std::string data) {
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return data;
}

void test_generate_examples() {
    HeadlessDriver::init(240, 135);
    
    std::vector<ThemeInfo> themes = {
        {"Default", new DefaultTheme()},
        {"Candy", new CandyTheme()},
        {"Christmas", new ChristmasTheme()}
    };

    std::vector<IExample*> examples = {
        new StatusWidgetExample(),
        new SensorWidgetExample()
    };

    for (auto* example : examples) {
        std::string widgetName = example->getWidgetName();
        std::cout << "Processing " << widgetName << "..." << std::endl;

        for (auto& themeInfo : themes) {
            std::string themePrefix = to_lower(themeInfo.name);
            
            for (const auto& state : example->getStates()) {
                lv_obj_clean(lv_scr_act());
                
                lv_obj_t* widgetObj = example->createWidget(lv_scr_act());
                example->applyTheme(themeInfo.theme);
                example->updateWidget(state.reading);
                
                // Generate filename: lib/Sensors/examples/WidgetName/theme-state.bmp
                std::string filename = "lib/Sensors/examples/" + widgetName + "/" + themePrefix + "-" + state.name + ".bmp";
                HeadlessDriver::saveSnapshot(widgetObj, filename);
            }
        }
    }
    
    // Cleanup
    for(auto& t : themes) delete t.theme;
    for(auto* e : examples) delete e;
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_generate_examples);
    return UNITY_END();
}
