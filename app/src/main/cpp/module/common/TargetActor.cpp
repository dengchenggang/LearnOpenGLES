#include "TargetActor.h"
#include "Level.h"
#include "utils/Log.h"

namespace module {

KEY_VALUE(TAG, TargetActor)

namespace {
constexpr const char* FRAME_IMAGE = "image/target_frame.png";
constexpr const char* ALERT_IMAGE = "image/warning.png";
constexpr int32_t ALERT_SIZE = 32;

void colorForType(TargetActor::Type type, float& r, float& g, float& b, float& a) {
    a = 1.0f;
    switch (type) {
        case TargetActor::Type::Green:
            r = 0.0f; g = 1.0f; b = 0.0f;
            break;
        case TargetActor::Type::Yellow:
            r = 1.0f; g = 1.0f; b = 0.0f;
            break;
        case TargetActor::Type::Red:
            r = 1.0f; g = 0.0f; b = 0.0f;
            break;
    }
}
}

TargetActor::TargetActor(engine::Level& level, uint64_t id)
    : Actor(level, id)
    , mFrame(level.createActor<IconActor>())
    , mAlert(level.createActor<IconActor>())
{
    mFrame.attachToActor(*this);
    mAlert.attachToActor(*this);
    hide();
}

TargetActor::~TargetActor() = default;

void TargetActor::show(Type type, int32_t x, int32_t y, int32_t width, int32_t height) {
    setEnabled(true).setVisible(true);

    float r, g, b, a;
    colorForType(type, r, g, b, a);

    mFrame.setEnabled(true).setVisible(true);
    mFrame.show(FRAME_IMAGE, x, y, width, height);
    mFrame.setColor(r, g, b, a);

    if (type == Type::Red) {
        mAlert.setEnabled(true).setVisible(true);
        mAlert.show(ALERT_IMAGE, x, y + height, ALERT_SIZE, ALERT_SIZE);
    } else {
        mAlert.setEnabled(false).setVisible(false);
    }
}

void TargetActor::hide() {
    mFrame.setEnabled(false).setVisible(false);
    mAlert.setEnabled(false).setVisible(false);
    setEnabled(false).setVisible(false);
}

void TargetActor::onBeginPlay() {
    LogI("%s onBeginPlay, id=%lu", TAG, getId());
    Actor::onBeginPlay();
}

void TargetActor::onEndPlay() {
    LogI("%s onEndPlay, id=%lu", TAG, getId());
    hide();
    Actor::onEndPlay();
}

} // namespace module
