#pragma once
#include "common/ModuleBase.h"

namespace module {

class NVSModule : public ModuleBase {
public:
    explicit NVSModule(IModuleManager& moduleManager);
    ~NVSModule();
public:
    void initialize() override;
    void start() override;
    void stop() override;
    void deinitialize() override;
};
}
