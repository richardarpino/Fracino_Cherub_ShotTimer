#ifndef WORKFLOW_FACTORY_H
#define WORKFLOW_FACTORY_H

#include "../Interfaces/IWorkflow.h"
#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/IBlocker.h"
#include <vector>

/**
 * Central factory for assembling machine workflows.
 * Enables discovery for documentation and runtime switching.
 */
class WorkflowFactory {
public:
    static IWorkflow* createSystemWorkflow(ISensorRegistry* registry, IBlocker* wifi, IBlocker* ota, IBlocker* warmup);
    static IWorkflow* createDashboardWorkflow(ISensorRegistry* registry);
    static IWorkflow* createShotWorkflow(ISensorRegistry* registry);

    static std::vector<IWorkflow*> createAllWorkflows(ISensorRegistry* registry, IBlocker* wifi, IBlocker* ota, IBlocker* warmup);
};

#endif
