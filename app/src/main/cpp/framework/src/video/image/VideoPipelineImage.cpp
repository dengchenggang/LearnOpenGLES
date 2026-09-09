#include "VideoPipelineImage.h"
#include <chrono>
#include "filesystem/ImageData.h"
#include <android/hardware_buffer.h>
#include <cstring>

namespace framework {

namespace {
AHardwareBuffer* createHardwareBufferFromPixels(int width, int height, VideoFormat format, const uint8_t* data) {
    AHardwareBuffer_Desc desc = {};
    desc.width = width;
    desc.height = height;
    desc.layers = 1;
    desc.usage = AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN | AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE;

    switch (format) {
        case VideoFormat::RGBA_8888:
            desc.format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
            break;
        case VideoFormat::RGB_888:
            desc.format = AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM;
            break;
        default:
            return nullptr;
    }

    AHardwareBuffer* buffer = nullptr;
    if (AHardwareBuffer_allocate(&desc, &buffer) != 0) {
        return nullptr;
    }

    void* ptr = nullptr;
    if (AHardwareBuffer_lock(buffer, desc.usage, -1, nullptr, &ptr) == 0) {
        size_t size = calculateSize(width, height, format);
        std::memcpy(ptr, data, size);
        AHardwareBuffer_unlock(buffer, nullptr);
    } else {
        AHardwareBuffer_release(buffer);
        return nullptr;
    }

    return buffer;
}
}

VideoPipelineImage::VideoPipelineImage(const std::string& assetName, int width, int height, VideoFormat format, float fps, bool useHardwareBuffer)
    : VideoPipeline(useHardwareBuffer)
    , mAssetName(assetName)
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

    if (auto imageData = dynamic_cast<ImageData*>(data.get())) {
        mWidth = imageData->width;
        mHeight = imageData->height;
    }

    mImageData = std::move(data);

    if (useHardwareBuffer()) {
        AHardwareBuffer* buffer = createHardwareBufferFromPixels(mWidth, mHeight, mFormat, mImageData->data());
        if (buffer) {
            mHardwareBuffer = std::make_shared<VideoHardwareBuffer>(buffer, 0, 0);
            AHardwareBuffer_release(buffer);
        }
    }

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

    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t escaped   = std::chrono::duration_cast<std::chrono::nanoseconds>(mLastDispatchLoopTimePoint.time_since_epoch()).count();

    if (useHardwareBuffer()) {
        if (mHardwareBuffer) {
            auto hardwareBuffer = std::make_shared<VideoHardwareBuffer>(mHardwareBuffer->get(), timestamp, escaped);
            dispath(hardwareBuffer);
        }
    } else {
        size_t size = calculateSize(mWidth, mHeight, mFormat);
        auto pair = mBufferPool.acquire(size, mImageData->data(), mWidth, mHeight, mFormat);
        if (!pair.second) {
            pair.first->setTimestamp(timestamp);
            pair.first->setEscaped(escaped);
        }

        auto videoFrame = pair.first;

        dispath(videoFrame);
    }

    auto currentTimePoint = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - mLastDispatchLoopTimePoint).count();
    int64_t targetDelayMs = static_cast<int64_t>(1000.0f / mFps);
    int64_t delayMs = targetDelayMs - elapsedMs;
    if (delayMs < 0) {
        delayMs = 0;
    }
    mTaskPool->detachDelayed(delayMs, &VideoPipelineImage::dispatchLoop, this);
}

} // namespace framework
