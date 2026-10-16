#pragma once
#include <string>
#include <vector>
#include <memory>
#include "architecture_rule.hpp"

class PluginLoader {
public:
    ~PluginLoader();
    void loadPlugins(const std::string& pluginDir);
    const std::vector<std::shared_ptr<ArchitectureRule>>& getRules() const { return rules_; }

private:
    std::vector<void*> handles_;
    std::vector<std::shared_ptr<ArchitectureRule>> rules_;
};
