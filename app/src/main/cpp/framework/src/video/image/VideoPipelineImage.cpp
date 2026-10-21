#include "VideoPipelineImage.h"
#include "VideoFrameBufferPool.h"
#include <android/hardware_buffer.h>
#include <cstring>
#include <chrono>
#include "utils/Log.h"
#include "filesystem/ImageData.h"

namespace framework {

KEY_VALUE(TAG, VideoPipelineImage)

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
        LogE("%s failed to allocate AHardwareBuffer: %dx%d", TAG, width, height);
        return nullptr;
    }

    void* ptr = nullptr;
    if (AHardwareBuffer_lock(buffer, desc.usage, -1, nullptr, &ptr) == 0) {
        size_t size = calculateSize(width, height, format);
        std::memcpy(ptr, data, size);
        AHardwareBuffer_unlock(buffer, nullptr);
        LogI("%s AHardwareBuffer created: %dx%d, format=%d", TAG, width, height, static_cast<int>(format));
    } else {
        LogE("%s failed to lock AHardwareBuffer", TAG);
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
    LogI("%s created: asset=%s, size=%dx%d, format=%d, fps=%.1f, hwBuffer=%d", TAG, assetName.c_str(), width, height, static_cast<int>(format), fps, useHardwareBuffer);
    mTaskPool->start();
}

VideoPipelineImage::~VideoPipelineImage() {
    LogI("%s destroyed", TAG);
    stop();
    mTaskPool->stop();
}

void VideoPipelineImage::start() {
    if (getState() != State::Idle) {
        LogW("%s start failed: not idle, currentState=%d", TAG, static_cast<int>(getState()));
        return;
    }

    LogI("%s starting: asset=%s", TAG, mAssetName.c_str());
    setState(State::Started);
    mFirstFrameNotified.store(false);

    auto data = FileSystem.readFile(mAssetName.c_str());
    if (!data || data->empty()) {
        LogE("%s start failed: cannot read file: %s", TAG, mAssetName.c_str());
        setState(State::Idle);
        notify(VideoPipelineEvent::StartFailed, -1, "failed to read image: " + mAssetName);
        return;
    }

    if (auto imageData = dynamic_cast<ImageData*>(data.get())) {
        mWidth = imageData->width;
        mHeight = imageData->height;
        LogI("%s image parsed: %dx%d", TAG, mWidth, mHeight);
    }

    mImageData = std::move(data);

    if (useHardwareBuffer()) {
        AHardwareBuffer* buffer = createHardwareBufferFromPixels(mWidth, mHeight, mFormat, mImageData->data());
        if (buffer) {
            mHardwareBuffer = std::make_shared<VideoHardwareBuffer>(buffer, 0, 0);
            AHardwareBuffer_release(buffer);
            LogI("%s hardware buffer created", TAG);
        } else {
            LogE("%s start failed: cannot create hardware buffer", TAG);
            mImageData.reset();
            setState(State::Idle);
            notify(VideoPipelineEvent::StartFailed, -1, "failed to create hardware buffer");
            return;
        }
    }

    LogI("%s started", TAG);
    notify(VideoPipelineEvent::Start);
}

void VideoPipelineImage::resume() {
    auto prevState = getState();
    if (prevState != State::Started && prevState != State::Paused) {
        LogW("%s resume failed: invalid state=%d", TAG, static_cast<int>(prevState));
        return;
    }

    LogI("%s resuming from state=%d", TAG, static_cast<int>(prevState));
    setState(State::Resumed);
    mLastFrameTime = std::chrono::steady_clock::now();
    dispatchLoop();
    notify(VideoPipelineEvent::Resume);
}

void VideoPipelineImage::pause() {
    if (getState() != State::Resumed) {
        LogW("%s pause failed: not resumed, currentState=%d", TAG, static_cast<int>(getState()));
        return;
    }

    LogI("%s paused", TAG);
    setState(State::Paused);
    notify(VideoPipelineEvent::Pause);
}

void VideoPipelineImage::stop() {
    if (getState() == State::Idle) {
        LogW("%s stop: already idle", TAG);
        return;
    }

    LogI("%s stopping", TAG);
    setState(State::Idle);
    mImageData.reset();
    mHardwareBuffer.reset();
    notify(VideoPipelineEvent::Stop);
}

void VideoPipelineImage::restart(bool hardRestart) {
    LogI("%s restart: hard=%d", TAG, hardRestart);
    pause();
    if (hardRestart) {
        stop();
        start();
    }
    if (getConnectionCount() > 0) {
        resume();
    }
}

void VideoPipelineImage::dispatchLoop() {
    if (getState() != State::Resumed) {
        LogW("%s dispatchLoop skipped: not resumed, state=%d", TAG, static_cast<int>(getState()));
        return;
    }

    auto now = std::chrono::steady_clock::now();
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t escaped   = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    LogD("%s dispatch frame: timestamp=%lld, escaped=%lld", TAG, static_cast<long long>(timestamp), static_cast<long long>(escaped));

    if (useHardwareBuffer()) {
        if (mHardwareBuffer) {
            auto hardwareBuffer = std::make_shared<VideoHardwareBuffer>(mHardwareBuffer->get(), timestamp, escaped);
            bool expected = false;
            if (mFirstFrameNotified.compare_exchange_strong(expected, true)) {
                notify(VideoPipelineEvent::FirstFrame);
            }
            dispatch(hardwareBuffer);
        }
    } else {
        size_t size = calculateSize(mWidth, mHeight, mFormat);
        auto pair = VideoFrameBufferPool.acquire(size, mImageData->data(), mWidth, mHeight, mFormat);
        if (!pair.second) {
            pair.first->setTimestamp(timestamp);
            pair.first->setEscaped(escaped);
        }

        auto videoFrame = pair.first;

        bool expected = false;
        if (mFirstFrameNotified.compare_exchange_strong(expected, true)) {
            notify(VideoPipelineEvent::FirstFrame);
        }
        dispatch(videoFrame);
    }

    auto intervalMs = std::chrono::milliseconds(static_cast<int64_t>(1000.0f / mFps));
    mLastFrameTime += intervalMs;
    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(mLastFrameTime - std::chrono::steady_clock::now());
    if (delay < std::chrono::milliseconds(0)) {
        delay = std::chrono::milliseconds(0);
        mLastFrameTime = std::chrono::steady_clock::now();
    }
    LogD("%s next frame delay: %lld ms", TAG, static_cast<long long>(delay.count()));
    mTaskPool->detachDelayed(delay.count(), &VideoPipelineImage::dispatchLoop, this);
}

} // namespace framework
