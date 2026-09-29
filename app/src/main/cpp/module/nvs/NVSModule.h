#pragma once
#include "level/Level.h"
#include "reference_t.hpp"
#include "utils/config/JsonConfigLoader.h"
#include <memory>

namespace module {

class NVSModule : public engine::Level {
public:
    NVSModule(engine::ILevelManager& levelManager, const JsonConfigLoaderPtr& config);
    ~NVSModule() override;
protected:
    void onBeginPlay() override;
    void onEnter() override;
    void onResize(int32_t width, int32_t height) override;
    void onExit() override;
    void onEndPlay() override;
private:
    JsonConfigLoaderPtr mConfig;
    engine::reference_ptr_t<engine::Actor> mActor;
};

}