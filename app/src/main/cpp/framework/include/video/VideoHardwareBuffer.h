#ifndef C_VIDEO_HARDWARE_BUFFER_H
#define C_VIDEO_HARDWARE_BUFFER_H

#include <memory>
#include <functional>

typedef struct AHardwareBuffer AHardwareBuffer;

namespace framework {

class VideoHardwareBuffer {
public:
    VideoHardwareBuffer(AHardwareBuffer* buffer, int64_t timestamp, int64_t escaped);
    ~VideoHardwareBuffer();
    VideoHardwareBuffer(const VideoHardwareBuffer&) = delete;
    VideoHardwareBuffer& operator=(const VideoHardwareBuffer&) = delete;
public:
    AHardwareBuffer* get() const { return mBuffer; }
    int64_t getTimestamp() const { return mTimestamp; }
    int64_t getEscaped() const { return mEscaped; }
    bool valid() const { return mBuffer != nullptr; }

private:
    AHardwareBuffer* mBuffer;
    const int64_t mTimestamp = 0;
    const int64_t mEscaped = 0;

};

using VideoHardwareBufferPtr = std::shared_ptr<VideoHardwareBuffer>;
using VideoHardwareBufferCallback = std::function<void(const VideoHardwareBufferPtr&)>;
} // namespace framework

#endif
