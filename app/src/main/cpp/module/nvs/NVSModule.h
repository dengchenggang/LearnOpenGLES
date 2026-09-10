#pragma once
#include "level/Level.h"
#include "utils/config/JsonConfigLoader.h"

namespace module {

class NVSModule : public engine::Level {
public:
    NVSModule(engine::ILevelManager& levelManager, const JsonConfigLoaderPtr& config);
    ~NVSModule() override;
protected:
    void onInit() override;
    void onDeInit() override;
private:
    JsonConfigLoaderPtr mConfig;
};

}