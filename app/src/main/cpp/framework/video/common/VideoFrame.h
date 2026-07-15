#ifndef C_VIDEO_FRAME_H
#define C_VIDEO_FRAME_H
#include "VideoFrameBase.h"
#include <memory>

struct VideoFrame {
    const std::unique_ptr<VideoFrameBase> frame;

    explicit VideoFrame(std::unique_ptr<VideoFrameBase> f) : frame(std::move(f)) {}
};

using VideoFramePtr = std::shared_ptr<VideoFrame>;
#endif
