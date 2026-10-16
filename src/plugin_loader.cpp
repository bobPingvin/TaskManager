#include "plugin_loader.hpp"
#include "logger.hpp"
#include <dlfcn.h>
#include <dirent.h>
#include <iostream>

PluginLoader::~PluginLoader() {
    rules_.clear();
    for (void* h : handles_) {
        if (h) dlclose(h);
    }
}

void PluginLoader::loadPlugins(const std::string& pluginDir) {
    DIR* dir = opendir(pluginDir.c_str());
    if (!dir) return;

    struct dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fname = entry->d_name;
        if (fname.size() > 3 && fname.substr(fname.size() - 3) == ".so") {
            std::string fullPath = pluginDir + "/" + fname;
            void* handle = dlopen(fullPath.c_str(), RTLD_NOW);
            if (!handle) {
                Logger::warn("Не удалось загрузить плагин " + fullPath + ": " + dlerror());
                continue;
            }

            dlerror();
            auto create_fn = reinterpret_cast<ArchitectureRule* (*)()>(dlsym(handle, "create_rule"));
            const char* err = dlerror();
            if (err || !create_fn) {
                Logger::warn("Точка входа create_rule() не найдена в " + fname);
                dlclose(handle);
                continue;
            }

            ArchitectureRule* rule = create_fn();
            if (rule) {
                handles_.push_back(handle);
                rules_.emplace_back(rule);
                Logger::info("[Плагин] Загружено правило: " + rule->name());
            } else {
                dlclose(handle);
            }
        }
    }
    closedir(dir);
}
