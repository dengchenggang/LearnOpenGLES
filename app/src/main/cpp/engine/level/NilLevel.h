#pragma once
#include "Level.h"

namespace engine {

class NilLevel : public Level {
public:
    static constexpr const char* NAME = "NilLevel";

    NilLevel(ILevelManager& levelManager, Camera& camera);
    ~NilLevel() override = default;
protected:
    void onExit() override;
};

} // namespace engine
