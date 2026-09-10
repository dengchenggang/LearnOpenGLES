#include "NVSComponent.h"
#include "Actor.h"
#include "utils/Log.h"

namespace module {

KEY_VALUE(TAG, NVSComponent)

NVSComponent::NVSComponent(engine::Actor& owner, engine::Image& image, const JsonConfigLoaderPtr& config)
    : engine::ActorComponent(owner)
    , mImage(image)
    , mConfig(config)
{
    auto url = mConfig->get<std::string>("video.url", "");
    auto width = mConfig->get<int32_t>("video.width", 0);
    auto height = mConfig->get<int32_t>("video.height", 0);
    auto format = mConfig->get<std::string>("video.format", "");

    if ((!url.empty()) && (width > 0) && (height > 0) && (!format.empty())) {
        VideoCapture.setVideoPipelineInfo(url, width, height, framework::toVideoFormat(format), 30.0f);
    }
    LogI("url=%s, width=%d, height=%d, format=%s", url.c_str(), width, height, format.c_str());
}

NVSComponent::~NVSComponent() = default;

void NVSComponent::onBeginPlay() {
    auto url = mConfig->get<std::string>("video.url", "");
    bool connected = VideoCapture.connect(url, TAG, std::bind(&NVSComponent::onVideoFrame, this, std::placeholders::_1));
    LogI("url=%s, connected=%d", url.c_str(), connected);
}

void NVSComponent::onUpdate(float deltaTime) {
    framework::VideoFramePtr newFrame;
    {
        std::lock_guard<std::mutex> lock(mFrameMutex);
        int readIndex = 1 - mWriteIndex;
        newFrame = std::move(mFrames[readIndex]);
    }

    mFrontFrame = std::move(newFrame);

    if (mFrontFrame) {
        int32_t channels = 0;
        switch (mFrontFrame->getFormat()) {
            case framework::VideoFormat::RGB_888:
                channels = 3;
                break;
            case framework::VideoFormat::RGBA_8888:
                channels = 4;
                break;
            default:
                return;
        }
        mImage.load(mFrontFrame->getData(), mFrontFrame->getWidth(), mFrontFrame->getHeight(), channels);
    }
}

void NVSComponent::onEndPlay() {
    auto url = mConfig->get<std::string>("video.url", "");
    VideoCapture.disconnect(url, TAG);
    LogI("url=%s", url.c_str());
}

void NVSComponent::onVideoFrame(const framework::VideoFramePtr& frame) {
    if (!frame) {
        LogE("frame is null");
        return;
    }

    std::lock_guard<std::mutex> lock(mFrameMutex);
    mFrames[mWriteIndex] = frame;
    mWriteIndex = 1 - mWriteIndex;
}

} // namespace module
