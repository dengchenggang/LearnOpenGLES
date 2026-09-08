#include "NVSModule.h"
#include "NVSComponent.h"
#include "RectTransform.h"
#include "common/ModuleDef.h"
#include "utils/Log.h"
#include <glm/glm.hpp>

namespace module {

NVSModule::NVSModule(engine::ILevelManager& levelManager) : engine::Level(MODULE_NAME_NVS, levelManager) {}

NVSModule::~NVSModule() = default;

void NVSModule::init() {
    LogI("NVSModule init");
    auto& actor = createActor();
    auto& image = actor.addComponent<engine::Image>();
    auto& rectTransform = image.getRectTransform();
    rectTransform.setPosition(glm::vec3(400.0f, 300.0f, 0.0f));
    rectTransform.setSize(glm::vec2(800.0f, 600.0f));
    actor.addComponent<NVSComponent>(image);
}

void NVSModule::deInit() {
    LogI("NVSModule deInit");
}

}