#pragma once

#include "Actor.h"
#include "ActorComponent.h"
#include "Image.h"
#include "video/VideoCapture.h"
#include "video/VideoFrame.h"
#include <memory>
#include <mutex>
#include <vector>

namespace module {

class NVSComponent : public engine::ActorComponent {
public:
    explicit NVSComponent(engine::Actor& owner, engine::Image& image);
    ~NVSComponent() override;

    void onBeginPlay() override;
    void onUpdate(float deltaTime) override;
    void onEndPlay() override;

private:
    void onVideoFrame(const framework::VideoFramePtr& frame);
private:
    engine::Image& mImage;

    struct FrameInfo {
        std::vector<uint8_t> data;
        int32_t width = 0;
        int32_t height = 0;
        int32_t channels = 0;
    };

    std::mutex mFrameMutex;
    std::unique_ptr<FrameInfo> mPendingFrame;
};

} // namespace module
