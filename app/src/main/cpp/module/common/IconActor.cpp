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

void IconActor::show(const std::string& path, int32_t x, int32_t y, int32_t width, int32_t height) {
    if (!mImage.setPath(path)) {
        LogE("%s show: setPath failed, path=%s", TAG, path.c_str());
        return;
    }

    mImage.getRectTransform().setPosition(glm::vec3(static_cast<float>(x), static_cast<float>(y), 0.0f));
    mImage.getRectTransform().setSize(glm::vec2(static_cast<float>(width), static_cast<float>(height)));
}

void IconActor::setColor(float r, float g, float b, float a) {
    mImage.setColor(r, g, b, a);
}

void IconActor::onBeginPlay() {
    LogI("%s onBeginPlay, id=%lu", TAG, getId());
    Actor::onBeginPlay();
}

void IconActor::onEndPlay() {
    LogI("%s onEndPlay, id=%lu", TAG, getId());
    Actor::onEndPlay();
}

} // namespace module
