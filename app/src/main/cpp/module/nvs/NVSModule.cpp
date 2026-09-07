#include "NVSModule.h"
#include "common/ModuleDef.h"
#include "utils/Log.h"

namespace module {

NVSModule::NVSModule(engine::ILevelManager& levelManager) : engine::Level(MODULE_NAME_NVS, levelManager) {}

NVSModule::~NVSModule() {}

void NVSModule::init() {
    LogI("NVSModule init");
}

void NVSModule::deInit() {
    LogI("NVSModule deInit");
}

}