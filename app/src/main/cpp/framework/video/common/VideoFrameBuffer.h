#ifndef C_VIDEO_FRAME_BUFFER_H
#define C_VIDEO_FRAME_BUFFER_H
#include "VideoFrame.h"
#include <cstring>
#include <memory>

class VideoFrameBuffer : public VideoFrame {
public:
    VideoFrameBuffer(const uint8_t* buffer, int32_t width, int32_t height, VideoFormat format, int64_t timestamp)
        : VideoFrame(width, height, format, timestamp, false)
        , mBuffer(std::make_unique<uint8_t[]>(getSize())) {
            std::memcpy(mBuffer.get(), buffer, getSize());
        }
    ~VideoFrameBuffer() override = default;
public:
    const uint8_t* getData() const override {
        return mBuffer.get();
    }

    void* getHardwareBuffer() const override {
        return nullptr;
    }

    void resetBuffer(const uint8_t* buffer, size_t size) {
        if (size > getSize()) {
            return;
        }
        std::memcpy(mBuffer.get(), buffer, size);
    }
private:
    std::unique_ptr<uint8_t[]> mBuffer;
};
#endif
