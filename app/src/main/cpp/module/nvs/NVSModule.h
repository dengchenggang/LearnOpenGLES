#pragma once
#include "level/Level.h"
#include "NVSActor.h"
#include "utils/config/JsonConfigLoader.h"
#include <memory>

namespace module {

class NVSModule : public engine::Level {
public:
    NVSModule(engine::ILevelManager& levelManager, engine::Camera& camera, const JsonConfigLoaderPtr& config);
    ~NVSModule() override;
protected:
    void onBeginPlay() override;
    void onEnter() override;
    void onExit() override;
    void onEndPlay() override;
private:
    JsonConfigLoaderPtr mConfig;
    NVSActor& mActor;
};

}
