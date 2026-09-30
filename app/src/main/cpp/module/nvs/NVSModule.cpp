#include "NVSModule.h"
#include "NVSComponent.h"
#include "RectTransform.h"
#include "common/ModuleDef.h"
#include "RenderInterface.h"
#include "utils/Log.h"
#include <glm/glm.hpp>

namespace module {

NVSModule::NVSModule(engine::ILevelManager& levelManager, const JsonConfigLoaderPtr& config)
    : engine::Level(MODULE_NAME_NVS, levelManager)
    , mConfig(config)
    , mActor(createActor())
{

}

NVSModule::~NVSModule() = default;

void NVSModule::onBeginPlay() {
    LogI("NVSModule onBeginPlay");
    mActor.setEnabled(true).setVisible(true);
    auto& image = mActor.addComponent<engine::Image>();
    int32_t viewportX = 0;
    int32_t viewportY = 0;
    int32_t viewportWidth = 0;
    int32_t viewportHeight = 0;
    RenderInterface.getViewport(&viewportX, &viewportY, &viewportWidth, &viewportHeight);
    if (viewportWidth > 0 && viewportHeight > 0) {
        auto& rectTransform = image.getRectTransform();
        rectTransform.setPosition(glm::vec3(viewportX + viewportWidth / 2.0f, viewportY + viewportHeight / 2.0f, 0.0f));
        rectTransform.setSize(glm::vec2(viewportWidth, viewportHeight));
        LogI("NVSModule image size: %dx%d", viewportWidth, viewportHeight);
    } else {
        LogW("NVSModule invalid viewport: %dx%d", viewportWidth, viewportHeight);
    }
    mActor.addComponent<NVSComponent>(image, mConfig);
}

void NVSModule::onEnter() {
    LogI("NVSModule onEnter");
}

void NVSModule::onResize(int32_t width, int32_t height) {
    LogI("NVSModule onResize: %dx%d", width, height);
    if (width > 0 && height > 0) {
        auto image = mActor.getComponent<engine::Image>();
        if (image) {
            auto& rectTransform = image->getRectTransform();
            rectTransform.setPosition(glm::vec3(0.0f + width / 2.0f, 0.0f + height / 2.0f, 0.0f));
            rectTransform.setSize(glm::vec2(width, height));
            LogI("NVSModule image size: %dx%d", width, height);
        } else {
            LogW("NVSModule image component is null");
        }

    } else {
        LogW("NVSModule invalid viewport: %dx%d", width, height);
    }
}

void NVSModule::onExit() {
    LogI("NVSModule onExit");
}

void NVSModule::onEndPlay() {
    LogI("NVSModule onEndPlay");
}

}