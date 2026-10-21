#pragma once
#include "level/Level.h"
#include "NVSActor.h"
#include "utils/config/JsonConfigLoader.h"
#include <memory>

namespace module {

class NVSModule : public engine::Level {
public:
    NVSModule(engine::LevelManager& levelManager, engine::Camera& camera, const JsonConfigLoaderPtr& config);
    ~NVSModule() override;
protected:
    void onBeginPlay() override;
    void onEnter(int32_t width, int32_t height) override;
    void onExit() override;
    void onEndPlay() override;
private:
    JsonConfigLoaderPtr mConfig;
    NVSActor& mActor;
};

}
