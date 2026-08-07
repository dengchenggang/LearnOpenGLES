#include "VideoPipelineImage.h"
#include <chrono>

namespace framework {
namespace video {

VideoPipelineImage::VideoPipelineImage(const std::string& assetName, int width, int height, VideoFormat format, float fps)
    : mAssetName(assetName)
    , mWidth(width)
    , mHeight(height)
    , mFormat(format)
    , mFps(fps)
    , mTaskPool(std::make_unique<TaskPool>()) {
}

VideoPipelineImage::~VideoPipelineImage() {
    if (mRunning.load()) {
        stop();
    }
}

void VideoPipelineImage::start() {
    if (mRunning.load()) {
        return;
    }

    auto data = FileSystem.readFile(mAssetName.c_str());
    if (!data || data->empty()) {
        return;
    }

    mImageData.assign(data->data(), data->data() + data->size());

    mTaskPool->start();
    mRunning.store(true);

    dispatchLoop();
}

void VideoPipelineImage::stop() {
    if (!mRunning.load()) {
        return;
    }

    mRunning.store(false);
    mTaskPool->stop();
}

void VideoPipelineImage::dispatchLoop() {
    if (!mRunning.load()) {
        return;
    }

    mLastDispatchLoopTimePoint = std::chrono::steady_clock::now();

    int64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    size_t size = VideoFrame::calculateSize(mWidth, mHeight, mFormat);
    auto pair = mBufferPool.acquire(size, mImageData.data(), mWidth, mHeight, mFormat, timestamp);
    if (!pair.second) {
        pair.first->resetTimestamp(timestamp);
        pair.first->resetBuffer(mImageData.data(), size);
    }

    auto videoFrame = pair.first;

    dispath(videoFrame);

    auto currentTimePoint = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - mLastDispatchLoopTimePoint).count();
    int64_t targetDelayMs = static_cast<int64_t>(1000.0f / mFps);
    int64_t delayMs = targetDelayMs - elapsedMs;
    if (delayMs < 0) {
        delayMs = 0;
    }
    mTaskPool->detachDelayed(delayMs, &VideoPipelineImage::dispatchLoop, this);
}

} // namespace video
} // namespace framework
