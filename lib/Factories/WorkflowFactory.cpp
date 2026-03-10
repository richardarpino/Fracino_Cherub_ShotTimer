#include "WorkflowFactory.h"
#include "../Logic/Workflows/BasicWorkflow.h"
#include "../Logic/Workflows/GenericScreen.h"
#include "../Registry/WidgetTags.h"
#include "../Interfaces/SensorTags.h"

IWorkflow* WorkflowFactory::createSystemWorkflow(ISensorRegistry* registry, IBlocker* wifi, IBlocker* ota, IBlocker* warmup) {
    BasicWorkflow* wf = new BasicWorkflow("System Startup", "Mandatory system initialization and warming sequence.");

    if (wifi) {
        wf->addScreen(new GenericScreen(
            ScreenComposition(1, 1).add(BlockerWidgetTag::NAME, wifi->getTagName()),
            registry,
            "WiFi Setup",
            "Connecting to the local network or providing hotspot for configuration.",
            "Wait for WiFi connection",
            wifi
        ));
    }

    if (ota) {
        wf->addScreen(new GenericScreen(
            ScreenComposition(1, 1).add(BlockerWidgetTag::NAME, ota->getTagName()),
            registry,
            "OTA Update Check",
            "Checking for and applying firmware updates over the air.",
            "Wait for OTA check completion",
            ota
        ));
    }

    if (warmup) {
        wf->addScreen(new GenericScreen(
            ScreenComposition(1, 1).add(BlockerWidgetTag::NAME, warmup->getTagName()),
            registry,
            "Warming Up",
            "The boiler is heating up to the target temperature.",
            "Wait for target temperature",
            warmup
        ));
    }

    return wf;
}

IWorkflow* WorkflowFactory::createDashboardWorkflow(ISensorRegistry* registry) {
    BasicWorkflow* wf = new BasicWorkflow("Main Dashboard", "The default operating screen showing machine status.");

    wf->addScreen(new GenericScreen(
        ScreenComposition(2, 2)
            .add(GaugeWidgetTag::NAME, BoilerPressureReading::NAME)
            .add(SensorWidgetTag::NAME, BoilerTempReading::NAME)
            .add(SensorWidgetTag::NAME, HeatingCycleReading::NAME)
            .add(SensorWidgetTag::NAME, LastValidShotReading::NAME),
        registry,
        "Dashboard",
        "Overview of boiler pressure, temperature, and shot history.",
        "Constant monitoring (Background)"
    ));

    return wf;
}

IWorkflow* WorkflowFactory::createShotWorkflow(ISensorRegistry* registry) {
    BasicWorkflow* wf = new BasicWorkflow("Shot Progress", "Visualizes the active espresso extraction.");

    wf->addScreen(new GenericScreen(
        ScreenComposition(2, 1)
            .add(SensorWidgetTag::NAME, LastValidShotReading::NAME)
            .add(ShotTimerWidgetTag::NAME, ShotTimeReading::NAME),
        registry,
        "Shot Timer",
        "Active timer and weight tracking during a shot.",
        "Wait for pump stop"
    ));

    return wf;
}
