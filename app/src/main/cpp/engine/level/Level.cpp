#include "Level.h"
#include "ILevelManager.h"
#include "component/camera/Camera.h"
#include <algorithm>

namespace engine {

uint64_t Level::sNextActorId = 1;

Level::Level(const std::string& name, ILevelManager& levelManager, Camera& camera,
             const glm::vec2& designSize, ScaleMode scaleMode)
    : mLevelManager(levelManager)
    , mCamera(camera)
    , mName(name)
    , mDesignSize(designSize)
    , mScaleMode(scaleMode)
    , mLogicalRect(0.0f, 0.0f, designSize.x, designSize.y)
    , mRenderRect(0.0f, 0.0f, designSize.x, designSize.y)
{
}

Level::~Level() {
}

void Level::onBeginPlay() {
    mBegunPlay = true;
    for (auto& actor : mActors) {
        if (!actor->getAttachParentActor()) {
            actor->onBeginPlay();
        }
    }
}

void Level::onEndPlay() {
    for (auto& actor : mActors) {
        if (!actor->getAttachParentActor()) {
            actor->onEndPlay();
        }
    }
    mBegunPlay = false;
}

void Level::setScaleMode(ScaleMode scaleMode) {
    if (mScaleMode == scaleMode) {
        return;
    }
    mScaleMode = scaleMode;
    updateAdaption(mLevelManager.getViewportWidth(), mLevelManager.getViewportHeight());
}

void Level::onResize(int32_t width, int32_t height) {
    updateAdaption(width, height);
}

void Level::onEnter(int32_t width, int32_t height) {
    updateAdaption(width, height);
}

void Level::updateAdaption(int32_t width, int32_t height) {
    if (width <= 0 || height <= 0 || mDesignSize.x <= 0.0f || mDesignSize.y <= 0.0f) {
        return;
    }

    float scaleX = static_cast<float>(width) / mDesignSize.x;
    float scaleY = static_cast<float>(height) / mDesignSize.y;

    if (mScaleMode == ScaleMode::Expand) {
        // 固定较短的一边，另一边按视口比例扩展，扩展出的空间向两侧均分使设计区域保持居中
        glm::vec2 logical = (scaleX > scaleY)
            ? glm::vec2(mDesignSize.y * width / height, mDesignSize.y)
            : glm::vec2(mDesignSize.x, mDesignSize.x * height / width);
        mLogicalRect = glm::vec4((mDesignSize.x - logical.x) * 0.5f,
                                 (mDesignSize.y - logical.y) * 0.5f,
                                 logical.x, logical.y);
        mRenderRect = glm::vec4(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    } else {
        float scale = (mScaleMode == ScaleMode::Fill) ? std::max(scaleX, scaleY)
                                                      : std::min(scaleX, scaleY);
        glm::vec2 scaled = mDesignSize * scale;
        mLogicalRect = glm::vec4(0.0f, 0.0f, mDesignSize.x, mDesignSize.y);
        // Fill 下 scaled 大于视口，偏移为负，超出部分由 GL 裁剪
        mRenderRect = glm::vec4((static_cast<float>(width) - scaled.x) * 0.5f,
                                (static_cast<float>(height) - scaled.y) * 0.5f,
                                scaled.x, scaled.y);
    }
}

void Level::onUpdate(int64_t deltaTime) {
    for (auto& actor : mActors) {
        if (!actor->getAttachParentActor() && actor->isEnabled()) {
            actor->onUpdate(deltaTime);
        }
    }
}

void Level::onRender() {
    for (auto& actor : mActors) {
        if (!actor->getAttachParentActor() && actor->isVisible()) {
            actor->onRender();
        }
    }
}

void Level::destroyActor(uint64_t actorId) {
    auto it = std::find_if(mActors.begin(), mActors.end(),
        [actorId](const ActorPtr& actor) { return actor->getId() == actorId; });
    if (it != mActors.end()) {
        (*it)->onEndPlay();
        (*it)->detachFromActor();
        mActors.erase(it);
    }
}

void Level::changeLevel(const std::string& levelName) {
    mLevelManager.changeLevel(levelName);
}

} // namespace engine