#include "video/VideoHardwareBuffer.h"
#include <android/hardware_buffer.h>

namespace framework {

VideoHardwareBuffer::VideoHardwareBuffer(AHardwareBuffer* buffer)
    : mBuffer(buffer) {
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

