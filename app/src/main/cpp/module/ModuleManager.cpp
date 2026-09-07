#include "ModuleManager.h"
#include "nvs/NVSModule.h"
#include "common/ModuleDef.h"
#include "utils/Log.h"

#undef ModuleManager

namespace module {
ModuleManager::ModuleManager()
    : mModules {}
    , mActiveModule {nullptr}
{
    mModules.try_emplace(std::string(MODULE_NAME_NVS), std::make_unique<NVSModule>(*this));
}

ModuleManager::~ModuleManager() {}

void ModuleManager::initialize() {
    LogI("initialize enter.");
    mTaskPool.start();

    mTaskPool.detach([this]() {
        doInitialize();
    });
    LogI("initialize exit");
}

void ModuleManager::doInitialize() {
    LogI("doInitialize enter.");
    for (auto& module : mModules) {
        module.second->initialize();
    }
    doChangeModule(MODULE_NAME_NVS);
    LogI("doInitialize exit");
}

void ModuleManager::changeModule(const char* moduleName) {
    LogI("changeModule enter: %s", moduleName);
    mTaskPool.detach([this, moduleName]() {
        doChangeModule(moduleName);
    });
    LogI("changeModule exit: %s", moduleName);
}

void ModuleManager::doChangeModule(const char* moduleName) {
    LogI("doChangeModule enter: %s", moduleName);
    if (mActiveModule) {
        mActiveModule->stop();
    }

    for (auto& module : mModules) {
        if (module.first == moduleName) {
            module.second->start();
            mActiveModule = module.second.get();
        }
    }
    LogI("doChangeModule exit: %s", moduleName);
}

void ModuleManager::deinitialize() {
    LogI("deinitialize enter");
    auto future = mTaskPool.submit([this]() {
        doDeinitialize();
    });
    future.wait();
    mTaskPool.stop();
    LogI("deinitialize exit");
}

void ModuleManager::doDeinitialize() {
    LogI("doDeinitialize enter");
    if (mActiveModule) {
        mActiveModule->stop();
    }

    for (auto& module : mModules) {
        module.second->deinitialize();
    }
    LogI("doDeinitialize exit");
}

}