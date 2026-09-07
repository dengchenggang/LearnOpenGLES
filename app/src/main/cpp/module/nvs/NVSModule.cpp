#include "NVSModule.h"
#include "common/ModuleDef.h"
#include "utils/Log.h"

namespace module {

NVSModule::NVSModule(IModuleManager& moduleManager) : ModuleBase(MODULE_NAME_NVS, moduleManager) {}

NVSModule::~NVSModule() {}

void NVSModule::initialize() {
    LogI("initialize");
}

void NVSModule::start() {
    LogI("start: enter");
    LogI("start: exit");
}

void NVSModule::stop() {
    LogI("stop");
}

void NVSModule::deinitialize() {
    LogI("deinitialize");
}

}

