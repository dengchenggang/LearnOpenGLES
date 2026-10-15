#include "NVSActor.h"
#include "Level.h"
#include "NVSComponentVideo.h"
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
    , mTarget(addComponent<NVSComponentTarget>(level))
    , mVideo(addComponent<NVSComponentVideo>(mImage, mConfig))
{
}

NVSActor::~NVSActor() = default;

void NVSActor::onBeginPlay() {
    LogI("%s onBeginPlay", TAG);

    setEnabled(true).setVisible(true);

    const auto& designSize = mLevel.getDesignSize();
    auto& rectTransform = mImage.getRectTransform();
    rectTransform.setPosition(glm::vec3(designSize.x * 0.5f, designSize.y * 0.5f, 0.0f));
    rectTransform.setSize(designSize);

    Actor::onBeginPlay();
}

void NVSActor::onEndPlay() {
    LogI("%s onEndPlay", TAG);
    Actor::onEndPlay();
}

} // namespace module
