#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include "common/IModuleManager.h"
#include "common/Singleton.hpp"
#include "utils/TaskPool.h"

namespace module {
class ModuleBase;
class ModuleManager : public IModuleManager {
private:
    friend class framework::Singleton<ModuleManager>;
public:
    ModuleManager();
    ~ModuleManager();
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;
public:
    void initialize();
    void changeModule(const char* moduleName) override;
    void deinitialize();
    TaskPool& getTaskPool() { return mTaskPool; }
private:
    void doInitialize();
    void doChangeModule(const char* moduleName);
    void doDeinitialize();
private:
    std::map<std::string, std::unique_ptr<ModuleBase>> mModules;
    ModuleBase* mActiveModule;
    TaskPool mTaskPool;
};

}

#define ModuleManager framework::Singleton<module::ModuleManager>::getInstance()
