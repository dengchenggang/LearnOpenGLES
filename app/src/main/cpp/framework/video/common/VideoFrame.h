#ifndef C_VIDEO_FRAME_H
#define C_VIDEO_FRAME_H
#include "VideoFrameBase.h"
#include <memory>

struct VideoFrame {
    const std::shared_ptr<VideoFrameBase> frame;

    explicit VideoFrame(std::shared_ptr<VideoFrameBase> f) : frame(std::move(f)) {}
};

using VideoFramePtr = std::shared_ptr<VideoFrame>;
#endif
