#ifndef C_VIDEO_FRAME_H
#define C_VIDEO_FRAME_H
#include "VideoDef.h"
#include <cstdint>
#include <memory>
#include <functional>

namespace framework {

class VideoFrame {
public:
    VideoFrame(int32_t width, int32_t height, VideoFormat format, int64_t timestamp)
        : mWidth(width)
        , mHeight(height)
        , mFormat(format)
        , mSize(calculateSize(width, height, format))
        , mTimestamp(timestamp){}
    virtual ~VideoFrame() = default;
    VideoFrame(const VideoFrame&) = delete;
    VideoFrame& operator = (const VideoFrame&) = delete;
public:
    virtual const uint8_t* getData() const = 0;

    int32_t getWidth()          const { return mWidth; }
    int32_t getHeight()         const { return mHeight; }
    VideoFormat getFormat()     const { return mFormat; }
    int64_t getTimestamp()      const { return mTimestamp; }
    size_t  getSize()           const { return mSize; }
    void resetTimestamp(int64_t timestamp) { mTimestamp = timestamp; }
private:
    const int32_t mWidth;
    const int32_t mHeight;
    const VideoFormat mFormat;
    const size_t  mSize;
    int64_t mTimestamp;
};

using VideoFramePtr = std::shared_ptr<VideoFrame>;
using VideoFrameCallback = std::function<void(const VideoFramePtr&)>;
} // namespace framework

#endif
