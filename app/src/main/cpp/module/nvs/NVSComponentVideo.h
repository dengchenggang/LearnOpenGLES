#pragma once

#include "Actor.h"
#include "ActorComponent.h"
#include "Image.h"
#include "video/VideoCapture.h"
#include "video/VideoFrame.h"
#include "utils/config/JsonConfigLoader.h"
#include <array>
#include <memory>
#include <mutex>

namespace module {

class NVSComponentVideo : public engine::ActorComponent {
public:
    explicit NVSComponentVideo(engine::Actor& owner, engine::Image& image, const JsonConfigLoaderPtr& config);
    ~NVSComponentVideo() override;

protected:
    void onBeginPlay() override;
    void onEnabledChanged(bool enabled) override {}
    void onUpdate(float deltaTime) override;
    void onEndPlay() override;

private:
    void onVideoFrame(const framework::VideoFramePtr& frame);
private:
    engine::Image& mImage;
    JsonConfigLoaderPtr mConfig;
private:
    std::mutex mFrameMutex;
    std::array<framework::VideoFramePtr, 2> mFrames;
    int mWriteIndex = 0;
    framework::VideoFramePtr mFrontFrame;
};

} // namespace module
