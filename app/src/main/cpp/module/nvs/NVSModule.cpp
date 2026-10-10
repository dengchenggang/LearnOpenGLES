#include "NVSModule.h"
#include "common/ModuleDef.h"
#include "utils/Log.h"

namespace module {

NVSModule::NVSModule(engine::ILevelManager& levelManager, const JsonConfigLoaderPtr& config)
    : engine::Level(MODULE_NAME_NVS, levelManager)
    , mConfig(config)
    , mActor(createActor<NVSActor>(config))
{
}

NVSModule::~NVSModule() = default;

void NVSModule::onBeginPlay() {
    LogI("NVSModule onBeginPlay");
}

void NVSModule::onEnter() {
    LogI("NVSModule onEnter");
}

void NVSModule::onResize(int32_t width, int32_t height) {
    LogI("NVSModule onResize: %dx%d", width, height);
    mActor.Resize(width, height);
}

void NVSModule::onExit() {
    LogI("NVSModule onExit");
}

void NVSModule::onEndPlay() {
    LogI("NVSModule onEndPlay");
}

}
