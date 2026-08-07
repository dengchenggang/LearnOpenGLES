#ifndef C_VIDEO_HARDWARE_BUFFER_H
#define C_VIDEO_HARDWARE_BUFFER_H

#include <android/hardware_buffer.h>
#include <memory>

namespace framework {
namespace video {

class VideoHardwareBuffer {
public:
    explicit VideoHardwareBuffer(AHardwareBuffer* buffer);
    ~VideoHardwareBuffer();
    VideoHardwareBuffer(const VideoHardwareBuffer&) = delete;
    VideoHardwareBuffer& operator=(const VideoHardwareBuffer&) = delete;
public:
    AHardwareBuffer* get() const { return mBuffer; }
    bool valid() const { return mBuffer != nullptr; }

private:
    AHardwareBuffer* mBuffer = nullptr;
};

using VideoHardwareBufferPtr = std::shared_ptr<VideoHardwareBuffer>;

} // namespace video
} // namespace framework

#endif
