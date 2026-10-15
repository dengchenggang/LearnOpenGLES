#include "NilLevel.h"

namespace engine {

NilLevel::NilLevel(ILevelManager& levelManager, Camera& camera)
    : Level(NAME, levelManager, camera, {1.0f, 1.0f}, ScaleMode::Fit) {
}

void NilLevel::onExit() {
}

} // namespace engine
