#ifndef MARKDOWN_GENERATOR_H
#define MARKDOWN_GENERATOR_H

#include "../../lib/Interfaces/IWorkflow.h"
#include "../../lib/Interfaces/IScreen.h"
#include "WorkflowSnapshotter.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>

/**
 * Utility to generate Markdown documentation for simulated workflows.
 */
class MarkdownGenerator {
public:
    static void generateGalleries(const std::vector<WorkflowSnapshotter::WorkflowResult>& results, const std::string& baseDir = "docs/previews") {
        mkdir(baseDir.c_str(), 0777);
        generateIndex(results, baseDir);
        for (const auto& res : results) {
            generateDetail(res, baseDir + "/" + sanitize(res.name));
        }
    }

private:
    static void generateIndex(const std::vector<WorkflowSnapshotter::WorkflowResult>& results, const std::string& baseDir) {
        std::ofstream index(baseDir + "/README.md");
        
        index << "# Machine Journeys Gallery" << "\n\n";
        index << "This gallery contains automated visual documentation of all user journeys in the system." << "\n\n";
        index << "| Journey | Description | Steps |" << "\n";
        index << "| :--- | :--- | :---: |" << "\n";

        for (const auto& res : results) {
            std::string slug = sanitize(res.name);
            index << "| [" << res.name << "](" << slug << "/README.md) | " 
                  << "User journey for " << res.name << ". | " << res.screens.size() << " |" << "\n";
        }
    }

    static void generateDetail(const WorkflowSnapshotter::WorkflowResult& res, const std::string& wfDir) {
        mkdir(wfDir.c_str(), 0777);
        std::ofstream doc(wfDir + "/README.md");

        doc << "# Journey: " << res.name << "\n\n";
        doc << "## Sequence of Events" << "\n\n";
        doc << "| Step | Screen | Visualization | Exit Condition |" << "\n";
        doc << "| :---: | :--- | :--- | :--- |" << "\n";

        for (size_t i = 0; i < res.screens.size(); ++i) {
            const auto& s = res.screens[i];
            doc << "| " << i << " | **" << s.name << "** | ![" << s.name << "](" << s.imagePath << ") | " 
                << "_" << s.exitCondition << "_ |" << "\n";
        }
    }

    static std::string sanitize(std::string data) {
        std::string result = data;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        std::replace(result.begin(), result.end(), ' ', '-');
        result.erase(std::remove_if(result.begin(), result.end(), [](char c) {
            return !std::isalnum(c) && c != '-';
        }), result.end());
        return result;
    }
};

#endif
