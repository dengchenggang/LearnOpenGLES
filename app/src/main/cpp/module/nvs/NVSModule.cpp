#include "NVSModule.h"
#include "common/ModuleDef.h"
#include "utils/Log.h"

namespace module {

namespace {

glm::vec2 readDesignSize(const JsonConfigLoaderPtr& config) {
    return {static_cast<float>(config->get<int>("level.designWidth", 1024)),
            static_cast<float>(config->get<int>("level.designHeight", 683))};
}

engine::ScaleMode readScaleMode(const JsonConfigLoaderPtr& config) {
    auto mode = config->get<std::string>("level.scaleMode", "fit");
    if (mode == "fill") {
        return engine::ScaleMode::Fill;
    }
    if (mode == "expand") {
        return engine::ScaleMode::Expand;
    }
    return engine::ScaleMode::Fit;
}

} // namespace

NVSModule::NVSModule(engine::ILevelManager& levelManager, engine::Camera& camera, const JsonConfigLoaderPtr& config)
    : engine::Level(MODULE_NAME_NVS, levelManager, camera, readDesignSize(config), readScaleMode(config))
    , mConfig(config)
    , mActor(createActor<NVSActor>(config))
{
}

NVSModule::~NVSModule() = default;

void NVSModule::onBeginPlay() {
    LogI("NVSModule onBeginPlay");
    engine::Level::onBeginPlay();
}

void NVSModule::onEnter(int32_t width, int32_t height) {
    LogI("NVSModule onEnter: %dx%d", width, height);
    engine::Level::onEnter(width, height);
}

void NVSModule::onExit() {
    LogI("NVSModule onExit");
}

void NVSModule::onEndPlay() {
    LogI("NVSModule onEndPlay");
    engine::Level::onEndPlay();
}

}
