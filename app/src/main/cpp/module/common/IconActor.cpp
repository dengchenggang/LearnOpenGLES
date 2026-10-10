#include "IconActor.h"
#include "RectTransform.h"
#include "utils/Log.h"
#include <glm/glm.hpp>

namespace module {

using namespace engine;

KEY_VALUE(TAG, IconActor)

IconActor::IconActor(Level& level, uint64_t id)
    : Actor(level, id)
    , mImage(addComponent<Image>())
{
}

IconActor::~IconActor() = default;

bool IconActor::setPath(const std::string& path) {
    return mImage.setPath(path);
}

bool IconActor::setHardwareBuffer(const std::string& url, AHardwareBuffer* buffer) {
    return mImage.setHardwareBuffer(url, buffer);
}

void IconActor::setPosition(float x, float y) {
    mImage.getRectTransform().setPosition(glm::vec3(x, y, 0.0f));
}

void IconActor::setSize(float width, float height) {
    mImage.getRectTransform().setSize(glm::vec2(width, height));
}

void IconActor::onBeginPlay() {
    LogI("%s onBeginPlay, id=%llu", TAG, getId());
    Actor::onBeginPlay();
}

void IconActor::onEndPlay() {
    LogI("%s onEndPlay, id=%llu", TAG, getId());
    Actor::onEndPlay();
}

} // namespace module
