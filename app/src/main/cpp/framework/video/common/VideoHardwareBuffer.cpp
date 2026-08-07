#include "VideoHardwareBuffer.h"

namespace framework {
namespace video {

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

} // namespace video
} // namespace framework

