#include "NVSComponent.h"
#include "Actor.h"
#include "utils/Log.h"

namespace module {

KEY_VALUE(TAG, NVSComponent)

NVSComponent::NVSComponent(engine::Actor& owner, engine::Image& image)
    : engine::ActorComponent(owner)
    , mImage(image) {}

NVSComponent::~NVSComponent() = default;

void NVSComponent::onBeginPlay() {
    VideoCapture.connect(
        "image://textures/background.png/1024/683/RGB_888/30",
        TAG,
        [this](const framework::VideoFramePtr& frame) { onVideoFrame(frame); }
    );
}

void NVSComponent::onUpdate(float deltaTime) {
    std::unique_ptr<FrameInfo> frame;
    {
        std::lock_guard<std::mutex> lock(mFrameMutex);
        frame = std::move(mPendingFrame);
    }

    if (frame) {
        mImage.load(frame->data.data(), frame->width, frame->height, frame->channels);
    }
}

void NVSComponent::onEndPlay() {
    VideoCapture.disconnect(
        "image://textures/background.png/1024/683/RGB_888/30",
        TAG
    );
}

void NVSComponent::onVideoFrame(const framework::VideoFramePtr& frame) {
    int32_t channels = 0;
    switch (frame->getFormat()) {
        case framework::VideoFormat::RGB_888:
            channels = 3;
            break;
        case framework::VideoFormat::RGBA_8888:
            channels = 4;
            break;
        default:
            LogW("%s unsupported video format: %d", TAG, static_cast<int32_t>(frame->getFormat()));
            return;
    }

    auto info = std::make_unique<FrameInfo>();
    info->data.assign(frame->getData(), frame->getData() + frame->getSize());
    info->width = frame->getWidth();
    info->height = frame->getHeight();
    info->channels = channels;

    std::lock_guard<std::mutex> lock(mFrameMutex);
    mPendingFrame = std::move(info);
}

} // namespace module
