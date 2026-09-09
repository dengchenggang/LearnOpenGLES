#include "video/VideoHardwareBuffer.h"
#include <android/hardware_buffer.h>

namespace framework {

VideoHardwareBuffer::VideoHardwareBuffer(AHardwareBuffer* buffer, int64_t timestamp, int64_t escaped)
    : mBuffer(buffer)
    , mTimestamp(timestamp)
    , mEscaped(escaped)
{
    if (mBuffer) {
        AHardwareBuffer_acquire(mBuffer);
    }
}

VideoHardwareBuffer::~VideoHardwareBuffer() {
    if (mBuffer) {
        AHardwareBuffer_release(mBuffer);
    }
}

} // namespace framework

