#include "NilLevel.h"

namespace engine {

NilLevel::NilLevel(LevelManager& levelManager, Camera& camera)
    : Level(NAME, levelManager, camera, {1.0f, 1.0f}, ScaleMode::Fit) {
}

} // namespace engine
