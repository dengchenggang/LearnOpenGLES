#ifndef C_VIDEO_PIPELINE_IMAGE_H
#define C_VIDEO_PIPELINE_IMAGE_H

#include "VideoPipeline.h"
#include "VideoFrameBuffer.h"
#include "utils/TaskPool.h"
#include "filesystem/FileSystem.h"
#include <atomic>
#include <vector>
#include "utils/BufferPool.hpp"

namespace framework {

class VideoPipelineImage : public VideoPipeline {
public:
    VideoPipelineImage(const std::string& assetName, int width, int height, VideoFormat format, float fps);
    ~VideoPipelineImage() override;

    VideoPipelineImage(const VideoPipelineImage&) = delete;
    VideoPipelineImage& operator=(const VideoPipelineImage&) = delete;

    void start() override;
    void stop() override;

private:
    void dispatchLoop();

private:
    std::string mAssetName;
    int mWidth;
    int mHeight;
    VideoFormat mFormat;
    float mFps;

    std::vector<uint8_t> mImageData;
    std::unique_ptr<TaskPool> mTaskPool;
    std::atomic<bool> mRunning{false};
    std::chrono::steady_clock::time_point mLastDispatchLoopTimePoint {};
    BufferPool<VideoFrameBuffer> mBufferPool {300 * 1024 * 1024};
};

} // namespace framework

#endif
