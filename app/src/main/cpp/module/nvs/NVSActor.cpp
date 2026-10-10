#include "NVSActor.h"
#include "NVSComponent.h"
#include "RectTransform.h"
#include "RenderInterface.h"
#include "utils/Log.h"
#include <glm/glm.hpp>

namespace module {

KEY_VALUE(TAG, NVSActor)

NVSActor::NVSActor(engine::Level& level, uint64_t id, const JsonConfigLoaderPtr& config)
    : engine::Actor(level, id)
    , mConfig(config)
    , mImage(addComponent<engine::Image>())
{
    addComponent<NVSComponent>(mImage, mConfig);
}

NVSActor::~NVSActor() = default;

void NVSActor::Resize(int32_t width, int32_t height) {
    if (width > 0 && height > 0) {
        auto& rectTransform = mImage.getRectTransform();
        rectTransform.setPosition(glm::vec3(width / 2.0f, height / 2.0f, 0.0f));
        rectTransform.setSize(glm::vec2(width, height));
        LogI("%s image resized: %dx%d", TAG, width, height);
    } else {
        LogW("%s invalid resize: %dx%d", TAG, width, height);
    }
}

void NVSActor::onBeginPlay() {
    LogI("%s onBeginPlay", TAG);

    setEnabled(true).setVisible(true);

    int32_t viewportX = 0, viewportY = 0, viewportWidth = 0, viewportHeight = 0;
    RenderInterface.getViewport(&viewportX, &viewportY, &viewportWidth, &viewportHeight);
    if (viewportWidth > 0 && viewportHeight > 0) {
        auto& rectTransform = mImage.getRectTransform();
        rectTransform.setPosition(glm::vec3(viewportX + viewportWidth / 2.0f, viewportY + viewportHeight / 2.0f, 0.0f));
        rectTransform.setSize(glm::vec2(viewportWidth, viewportHeight));
        LogI("%s image size: %dx%d", TAG, viewportWidth, viewportHeight);
    } else {
        LogW("%s invalid viewport: %dx%d", TAG, viewportWidth, viewportHeight);
    }

    Actor::onBeginPlay();
}

void NVSActor::onEndPlay() {
    LogI("%s onEndPlay", TAG);
    Actor::onEndPlay();
}

} // namespace module
