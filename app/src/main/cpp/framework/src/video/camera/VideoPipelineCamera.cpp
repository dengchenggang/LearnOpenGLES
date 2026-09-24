#include "VideoPipelineCamera.h"
#include "VideoFrameCamera.h"
#include "utils/Log.h"
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraMetadata.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>
#include <android/native_window.h>
#include <algorithm>
#include <cmath>
#include <climits>

namespace framework {

int32_t toAImageFormat(VideoFormat format) {
    switch (format) {
        case VideoFormat::RGBA_8888:
            return AIMAGE_FORMAT_RGBA_8888;
        case VideoFormat::RGB_888:
            return AIMAGE_FORMAT_RGB_888;
        case VideoFormat::YUV_420_888:
            return AIMAGE_FORMAT_YUV_420_888;
        default:
            return AIMAGE_FORMAT_YUV_420_888;
    }
}

const char* formatToString(int32_t fmt) {
    switch (fmt) {
        case AIMAGE_FORMAT_RGBA_8888: return "RGBA_8888";
        case AIMAGE_FORMAT_RGB_888:   return "RGB_888";
        case AIMAGE_FORMAT_YUV_420_888: return "YUV_420_888";
        default: return "Unknown";
    }
}

VideoPipelineCamera::VideoPipelineCamera(int32_t cameraId, int32_t width, int32_t height, VideoFormat format, float fps, bool useHardwareBuffer)
    : VideoPipeline(useHardwareBuffer)
    , mCameraId(cameraId)
    , mWidth(width)
    , mHeight(height)
    , mFormat(format)
    , mFps(fps)
{

}

VideoPipelineCamera::~VideoPipelineCamera() {
    LOG_ENTER();
    stop();
    LOG_EXIT();
}

void VideoPipelineCamera::start() {
    LOG_ENTER();
    if (getState() != State::Idle) {
        LOG_EXIT();
        return;
    }

    setState(State::Started);
    mClosingByUs.store(false);
    mFirstFrameNotified.store(false);

    if (!openCamera()) {
        releaseCameraResources();
        setState(State::Idle);
        notify(VideoPipelineEvent::StartFailed, -1, "failed to open camera");
        LOG_EXIT();
        return;
    }
    if (!createImageReader()) {
        releaseCameraResources();
        setState(State::Idle);
        notify(VideoPipelineEvent::StartFailed, -1, "failed to create image reader");
        LOG_EXIT();
        return;
    }
    if (!createCaptureSession()) {
        releaseCameraResources();
        setState(State::Idle);
        notify(VideoPipelineEvent::StartFailed, -1, "failed to create capture session");
        LOG_EXIT();
        return;
    }

    notify(VideoPipelineEvent::Start);
    LOG_EXIT("camera %d started, %dx%d, format: %d", mCameraId, mWidth, mHeight, static_cast<int32_t>(mFormat));
}

void VideoPipelineCamera::resume() {
    LOG_ENTER();
    if (getState() != State::Started && getState() != State::Paused) {
        LOG_EXIT();
        return;
    }

    setState(State::Resumed);

    if (!startPreview()) {
        setState(getPreviousState());
        notify(VideoPipelineEvent::ResumeFailed, -1, "failed to resume camera preview");
        LOG_EXIT();
        return;
    }

    notify(VideoPipelineEvent::Resume);
    LOG_EXIT();
}

void VideoPipelineCamera::pause() {
    LOG_ENTER();
    if (getState() != State::Resumed) {
        LOG_EXIT();
        return;
    }

    setState(State::Paused);

    if (mCaptureSession) {
        ACameraCaptureSession_stopRepeating(reinterpret_cast<ACameraCaptureSession*>(mCaptureSession));
    }

    notify(VideoPipelineEvent::Pause);
    LOG_EXIT();
}

void VideoPipelineCamera::stop() {
    LOG_ENTER();
    if (getState() == State::Idle) {
        LOG_EXIT();
        return;
    }

    LogI("camera %d stopping", mCameraId);

    setState(State::Idle);
    mClosingByUs.store(true);
    releaseCameraResources();
    notify(VideoPipelineEvent::Stop);
    LOG_EXIT();
}

void VideoPipelineCamera::restart(bool hardRestart) {
    LOG_ENTER("hardRestart=%d", hardRestart);
    pause();
    if (hardRestart) {
        stop();
        start();
    }
    if (getConnectionCount() > 0) {
        resume();
    }
    LOG_EXIT();
}
void VideoPipelineCamera::handleImageAvailable(AImageReader* reader) {
    auto acquireStart = std::chrono::steady_clock::now();
    AImage* image = nullptr;
    media_status_t status = AImageReader_acquireLatestImage(reader, &image);
    auto acquireElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - acquireStart).count();
    if (status != AMEDIA_OK || !image) {
        int32_t count = mStreamingErrorCount.fetch_add(1) + 1;
        if (count >= 10) {
            mStreamingErrorCount.store(0);
            notify(VideoPipelineEvent::StreamingError, status, "continuous acquire failures");
        }
        LogE("exit: failed to acquire latest image, status: %d", status);
        return;
    }
    mStreamingErrorCount.store(0);

    float fps = 0.0f;
    float captureFps = 0.0f;

    int64_t captureTime = 0;
    AImage_getTimestamp(image, &captureTime);

    if (mLastCaptureTime != 0) {
        int64_t captureElapsedMs = (captureTime - mLastCaptureTime) / 1'000'000;
        captureFps = captureElapsedMs > 0 ? 1000.0f / static_cast<float>(captureElapsedMs) : 0.0f;
    }
    mLastCaptureTime = captureTime;

    auto currentTimePoint = std::chrono::steady_clock::now();
    if (mLastDispatchTimePoint.time_since_epoch().count() != 0) {
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - mLastDispatchTimePoint).count();
        fps = elapsedMs > 0 ? 1000.0f / static_cast<float>(elapsedMs) : 0.0f;
    }
    mLastDispatchTimePoint = currentTimePoint;

    int64_t escaped = std::chrono::duration_cast<std::chrono::nanoseconds>(
        currentTimePoint.time_since_epoch()).count();
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    auto dispatchStart = std::chrono::steady_clock::now();
    if (useHardwareBuffer()) {
        dispatchHardwareBuffer(image, timestamp, mLastCaptureTime);
    } else {
        dispatchVideoFrame(image, timestamp, mLastCaptureTime);
    }
    auto dispatchElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - dispatchStart).count();

    LogI("fps=%.2f, captureFps=%.2f, acquireTimeMs=%lld, dispatchTimeMs=%lld",
        fps, captureFps, acquireElapsedMs, dispatchElapsedMs);
}

void VideoPipelineCamera::dispatchVideoFrame(AImage* image, int64_t timestamp, int64_t escaped) {
    int32_t width = 0, height = 0;
    AImage_getWidth(image, &width);
    AImage_getHeight(image, &height);
    int32_t aFormat = 0;
    AImage_getFormat(image, &aFormat);
    auto videoFrame = std::make_shared<VideoFrameCamera>(image, width, height, toVideoFormat(aFormat));
    videoFrame->setTimestamp(timestamp);
    videoFrame->setEscaped(escaped);

    bool expected = false;
    if (mFirstFrameNotified.compare_exchange_strong(expected, true)) {
        notify(VideoPipelineEvent::FirstFrame);
    }

    dispatch(videoFrame);
}

void VideoPipelineCamera::dispatchHardwareBuffer(AImage* image, int64_t timestamp, int64_t escaped) {
    AHardwareBuffer* buffer = nullptr;
    AImage_getHardwareBuffer(image, &buffer);
    if (buffer) {
        auto hardwareBuffer = std::make_shared<VideoHardwareBuffer>(buffer, timestamp, escaped);

        bool expected = false;
        if (mFirstFrameNotified.compare_exchange_strong(expected, true)) {
            notify(VideoPipelineEvent::FirstFrame);
        }

        dispatch(hardwareBuffer);
    }
    AImage_delete(image);
}
void VideoPipelineCamera::handleDeviceDisconnected(ACameraDevice* device) {
    LOG_ENTER("camera device disconnected");
    notify(VideoPipelineEvent::SourceDisconnected, -1, "camera device disconnected");
    ACameraDevice_close(device);
    LOG_EXIT();
}

void VideoPipelineCamera::handleDeviceError(ACameraDevice* device, int error) {
    LOG_ENTER("camera device error: %d", error);
    notify(VideoPipelineEvent::SourceError, error, "camera device error");
    ACameraDevice_close(device);
    LOG_EXIT();
}

void VideoPipelineCamera::handleSessionActive(ACameraCaptureSession* session) {
    LOG_ENTER("capture session active");
    LOG_EXIT();
}

void VideoPipelineCamera::handleSessionReady(ACameraCaptureSession* session) {
    LOG_ENTER("capture session ready");
    LOG_EXIT();
}

void VideoPipelineCamera::handleSessionClosed(ACameraCaptureSession* session) {
    LOG_ENTER("capture session closed");
    if (!mClosingByUs.load()) {
        notify(VideoPipelineEvent::StreamingError, -1, "capture session closed unexpectedly");
    }
    LOG_EXIT();
}
bool VideoPipelineCamera::openCamera() {
    LOG_ENTER();
    auto* manager = ACameraManager_create();
    if (!manager) {
        LOG_EXIT("failed to create ACameraManager");
        return false;
    }
    mCameraManager = manager;

    ACameraIdList* cameraIds = nullptr;
    ACameraManager_getCameraIdList(manager, &cameraIds);
    if (!cameraIds || cameraIds->numCameras == 0) {
        if (cameraIds) {
            ACameraManager_deleteCameraIdList(cameraIds);
        }
        LOG_EXIT("no camera available");
        return false;
    }

    const char* matchedId = nullptr;
    for (int i = 0; i < cameraIds->numCameras; ++i) {
        try {
            if (std::stoi(cameraIds->cameraIds[i]) == mCameraId) {
                matchedId = cameraIds->cameraIds[i];
            }
        } catch (...) {
            // ignore non-integer camera id
        }
    }

    if (!matchedId) {
        LogE("requested camera id=%d not found in available list, available cameras(%d):", mCameraId, cameraIds->numCameras);
        for (int i = 0; i < cameraIds->numCameras; ++i) {
            LogE("  Camera[%d]: %s", i, cameraIds->cameraIds[i]);
        }
        ACameraManager_deleteCameraIdList(cameraIds);
        LOG_EXIT("requested camera id=%d not found in available list", mCameraId);
        return false;
    }

    ACameraDevice_stateCallbacks deviceCallbacks = {
        .context = this,
        .onDisconnected = VideoPipelineCamera::onDeviceDisconnected,
        .onError = VideoPipelineCamera::onDeviceError
    };
    int ret = ACameraManager_openCamera(manager, matchedId, &deviceCallbacks, reinterpret_cast<ACameraDevice**>(&mCameraDevice));
    ACameraManager_deleteCameraIdList(cameraIds);

    if (ret != ACAMERA_OK || !mCameraDevice) {
        LOG_EXIT("failed to open camera %d, ret: %d", mCameraId, ret);
        return false;
    }
    LOG_EXIT();
    return true;
}

bool VideoPipelineCamera::createImageReader() {
    LOG_ENTER();
    int32_t aFormat = toAImageFormat(mFormat);

    ACameraIdList* cameraIds = nullptr;
    ACameraManager_getCameraIdList(reinterpret_cast<ACameraManager*>(mCameraManager), &cameraIds);
    if (!cameraIds || cameraIds->numCameras == 0) {
        if (cameraIds) {
            ACameraManager_deleteCameraIdList(cameraIds);
        }
        LOG_EXIT("cannot get camera id list for config check");
        return false;
    }

    const char* matchedId = nullptr;
    for (int i = 0; i < cameraIds->numCameras; ++i) {
        try {
            if (std::stoi(cameraIds->cameraIds[i]) == mCameraId) {
                matchedId = cameraIds->cameraIds[i];
            }
        } catch (...) {
            // ignore non-integer camera id
        }
    }

    if (!matchedId) {
        ACameraManager_deleteCameraIdList(cameraIds);
        LOG_EXIT("requested camera id=%d not found in available list", mCameraId);
        return false;
    }

    ACameraMetadata* metadata = nullptr;
    ACameraManager_getCameraCharacteristics(reinterpret_cast<ACameraManager*>(mCameraManager), matchedId, &metadata);
    ACameraManager_deleteCameraIdList(cameraIds);

    if (!metadata) {
        LOG_EXIT("failed to get camera metadata");
        return false;
    }

    ACameraMetadata_const_entry entry;
    int32_t bestW = mWidth;
    int32_t bestH = mHeight;
    int32_t minDiff = INT_MAX;
    bool found = false;
    if (ACameraMetadata_getConstEntry(metadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry) == ACAMERA_OK) {
        for (size_t i = 0; i < entry.count; i += 4) {
            int32_t fmt = entry.data.i32[i];
            int32_t w = entry.data.i32[i + 1];
            int32_t h = entry.data.i32[i + 2];
            int32_t dir = entry.data.i32[i + 3];
            if (dir == 0) {
                if (fmt == aFormat) {
                    int32_t diff = std::abs(w - mWidth) + std::abs(h - mHeight);
                    if (diff < minDiff) {
                        minDiff = diff;
                        bestW = w;
                        bestH = h;
                        found = true;
                    }
                }
            }
        }
    }

    if (!found) {
        LogE("no supported stream config for format=%s, available configs:", formatToString(aFormat));
        if (ACameraMetadata_getConstEntry(metadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry) == ACAMERA_OK) {
            for (size_t i = 0; i < entry.count; i += 4) {
                int32_t fmt = entry.data.i32[i];
                int32_t w = entry.data.i32[i + 1];
                int32_t h = entry.data.i32[i + 2];
                int32_t dir = entry.data.i32[i + 3];
                if (dir == 0) {
                    LogE("  format=%s, %dx%d", formatToString(fmt), w, h);
                }
            }
        }
        ACameraMetadata_free(metadata);
        LOG_EXIT("no supported stream config for format=%s", formatToString(aFormat));
        return false;
    }
    ACameraMetadata_free(metadata);

    if (bestW != mWidth || bestH != mHeight) {
        LogI("stream config adjusted: %dx%d -> %dx%d (Manhattan distance)", mWidth, mHeight, bestW, bestH);
        mWidth = bestW;
        mHeight = bestH;
    }

    media_status_t status;
    if (useHardwareBuffer()) {
        status = AImageReader_newWithUsage(mWidth, mHeight, aFormat,
            AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE,
            2, reinterpret_cast<AImageReader**>(&mImageReader));
    } else {
        status = AImageReader_new(mWidth, mHeight, aFormat, 2, reinterpret_cast<AImageReader**>(&mImageReader));
    }
    if (status != AMEDIA_OK || !mImageReader) {
        LOG_EXIT("failed to create AImageReader, status: %d", status);
        return false;
    }

    AImageReader_getWindow(reinterpret_cast<AImageReader*>(mImageReader), reinterpret_cast<ANativeWindow**>(&mNativeWindow));

    AImageReader_ImageListener listener = {
        .context = this,
        .onImageAvailable = VideoPipelineCamera::onImageAvailable
    };
    AImageReader_setImageListener(reinterpret_cast<AImageReader*>(mImageReader), &listener);
    LOG_EXIT();
    return true;
}

bool VideoPipelineCamera::createCaptureSession() {
    LOG_ENTER();
    ACaptureSessionOutputContainer* container = nullptr;
    ACaptureSessionOutputContainer_create(&container);

    ACaptureSessionOutput* output = nullptr;
    ACaptureSessionOutput_create(reinterpret_cast<ANativeWindow*>(mNativeWindow), &output);
    ACaptureSessionOutputContainer_add(container, output);

    ACameraCaptureSession_stateCallbacks sessionCallbacks = {
        .context  = this,
        .onClosed = VideoPipelineCamera::onSessionClosed,
        .onReady  = VideoPipelineCamera::onSessionReady,
        .onActive = VideoPipelineCamera::onSessionActive,
    };
    int ret = ACameraDevice_createCaptureSession(reinterpret_cast<ACameraDevice*>(mCameraDevice), container, &sessionCallbacks, reinterpret_cast<ACameraCaptureSession**>(&mCaptureSession));

    ACaptureSessionOutputContainer_free(container);
    ACaptureSessionOutput_free(output);

    if (ret != ACAMERA_OK || !mCaptureSession) {
        LOG_EXIT("failed to create capture session, ret: %d", ret);
        return false;
    }
    LOG_EXIT();
    return true;
}

bool VideoPipelineCamera::startPreview() {
    LOG_ENTER();
    ACaptureRequest* request = nullptr;
    ACameraDevice_createCaptureRequest(reinterpret_cast<ACameraDevice*>(mCameraDevice), TEMPLATE_PREVIEW, &request);
    if (!request) {
        LOG_EXIT("failed to create capture request");
        return false;
    }

    ACameraOutputTarget* target = nullptr;
    ACameraOutputTarget_create(reinterpret_cast<ANativeWindow*>(mNativeWindow), &target);
    ACaptureRequest_addTarget(request, target);

    int32_t targetFps = std::max(1, static_cast<int32_t>(std::lround(mFps)));
    int32_t fpsRange[] = {targetFps, targetFps};
    if (ACaptureRequest_setEntry_i32(request, ACAMERA_CONTROL_AE_TARGET_FPS_RANGE, 2, fpsRange) != ACAMERA_OK) {
        ACameraOutputTarget_free(target);
        ACaptureRequest_free(request);
        LOG_EXIT("failed to set camera fps: %d", targetFps);
        return false;
    }

    int seqId = 0;
    int ret = ACameraCaptureSession_setRepeatingRequest(
        reinterpret_cast<ACameraCaptureSession*>(mCaptureSession), nullptr, 1, &request, &seqId);

    ACameraOutputTarget_free(target);
    ACaptureRequest_free(request);
    if (ret != ACAMERA_OK) {
        LOG_EXIT("failed to start repeating request, ret: %d", ret);
        return false;
    }
    LOG_EXIT();
    return true;
}

void VideoPipelineCamera::releaseCameraResources() {
    if (mCaptureSession) {
        ACameraCaptureSession_stopRepeating(reinterpret_cast<ACameraCaptureSession*>(mCaptureSession));
        ACameraCaptureSession_close(reinterpret_cast<ACameraCaptureSession*>(mCaptureSession));
        mCaptureSession = nullptr;
    }

    if (mImageReader) {
        AImageReader_delete(reinterpret_cast<AImageReader*>(mImageReader));
        mImageReader = nullptr;
    }

    mNativeWindow = nullptr;

    if (mCameraDevice) {
        ACameraDevice_close(reinterpret_cast<ACameraDevice*>(mCameraDevice));
        mCameraDevice = nullptr;
    }

    if (mCameraManager) {
        ACameraManager_delete(reinterpret_cast<ACameraManager*>(mCameraManager));
        mCameraManager = nullptr;
    }
}
void VideoPipelineCamera::onImageAvailable(void* context, AImageReader* reader) {
    auto* pipeline = static_cast<VideoPipelineCamera*>(context);
    if (pipeline) {
        pipeline->handleImageAvailable(reader);
    }
}

void VideoPipelineCamera::onDeviceDisconnected(void* context, ACameraDevice* device) {
    auto* pipeline = static_cast<VideoPipelineCamera*>(context);
    if (pipeline) {
        pipeline->handleDeviceDisconnected(device);
    }
}

void VideoPipelineCamera::onDeviceError(void* context, ACameraDevice* device, int error) {
    auto* pipeline = static_cast<VideoPipelineCamera*>(context);
    if (pipeline) {
        pipeline->handleDeviceError(device, error);
    }
}

void VideoPipelineCamera::onSessionActive(void* context, ACameraCaptureSession* session) {
    auto* pipeline = static_cast<VideoPipelineCamera*>(context);
    if (pipeline) {
        pipeline->handleSessionActive(session);
    }
}

void VideoPipelineCamera::onSessionReady(void* context, ACameraCaptureSession* session) {
    auto* pipeline = static_cast<VideoPipelineCamera*>(context);
    if (pipeline) {
        pipeline->handleSessionReady(session);
    }
}

void VideoPipelineCamera::onSessionClosed(void* context, ACameraCaptureSession* session) {
    auto* pipeline = static_cast<VideoPipelineCamera*>(context);
    if (pipeline) {
        pipeline->handleSessionClosed(session);
    }
}

} // namespace framework
