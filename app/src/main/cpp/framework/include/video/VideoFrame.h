#ifndef C_VIDEO_FRAME_H
#define C_VIDEO_FRAME_H
#include "video/VideoDef.h"
#include <cstdint>
#include <memory>
#include <functional>

namespace framework {

class VideoFrame {
public:
    VideoFrame(int32_t width, int32_t height, VideoFormat format)
        : mWidth(width)
        , mStride(calculateStride(width, format))
        , mHeight(height)
        , mFormat(format)
        , mSize(calculateSize(width, height, format))
        , mTimestamp(0)
        , mEscaped(0)
    {
    }
    virtual ~VideoFrame() = default;
    VideoFrame(const VideoFrame&) = delete;
    VideoFrame& operator = (const VideoFrame&) = delete;
public:
    virtual const uint8_t* getData() const = 0;

    void setTimestamp(int64_t timestamp) { mTimestamp = timestamp; }
    void setEscaped(int64_t escaped)     { mEscaped = escaped; }

    int32_t getWidth()          const    { return mWidth; }
    int32_t getStride()         const    { return mStride; }
    int32_t getHeight()         const    { return mHeight; }
    VideoFormat getFormat()     const    { return mFormat; }
    int64_t getSize()           const    { return mSize; }
    int64_t getTimestamp()      const    { return mTimestamp; }
    int64_t getEscaped()        const    { return mEscaped; }
protected:
    void setStride(int32_t stride) { mStride = stride; }
    void setSize(int64_t size)     { mSize   = size; }
private:
    int32_t     mWidth;
    int32_t     mStride;
    int32_t     mHeight;
    VideoFormat mFormat;
    int64_t     mSize;
    int64_t     mTimestamp;
    int64_t     mEscaped;
};

using VideoFramePtr = std::shared_ptr<VideoFrame>;
using VideoFrameCallback = std::function<void(const VideoFramePtr&)>;
} // namespace framework

#endif
