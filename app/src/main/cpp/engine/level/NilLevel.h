#pragma once
#include "Level.h"

namespace engine {

class NilLevel : public Level {
public:
    static constexpr const char* NAME = "NilLevel";

    NilLevel(LevelManager& levelManager, Camera& camera);
    ~NilLevel() override = default;
};

} // namespace engine
