#pragma once
#include "level/Level.h"

namespace module {

class NVSModule : public engine::Level {
public:
    explicit NVSModule(engine::ILevelManager& levelManager);
    ~NVSModule() override;
public:
    void init() override;
    void deInit() override;
};

}