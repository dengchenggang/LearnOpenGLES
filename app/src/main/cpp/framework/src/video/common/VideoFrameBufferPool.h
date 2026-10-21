#ifndef C_VIDEO_FRAME_BUFFER_POOL_H
#define C_VIDEO_FRAME_BUFFER_POOL_H

#include "common/Singleton.hpp"
#include "VideoFrameBuffer.h"
#include "utils/BufferPool.hpp"

namespace framework {

class VideoFrameBufferPool {
    friend class Singleton<VideoFrameBufferPool>;
public:
    template <typename... Args>
    std::pair<std::shared_ptr<VideoFrameBuffer>, bool> acquire(size_t size, Args&&... args) {
        return mBufferPool.acquire(size, std::forward<Args>(args)...);
    }

private:
    VideoFrameBufferPool() = default;

    BufferPool<VideoFrameBuffer> mBufferPool {300 * 1024 * 1024};
};

} // namespace framework

#define VideoFrameBufferPool framework::Singleton<framework::VideoFrameBufferPool>::getInstance()

#endif // C_VIDEO_FRAME_BUFFER_POOL_H
