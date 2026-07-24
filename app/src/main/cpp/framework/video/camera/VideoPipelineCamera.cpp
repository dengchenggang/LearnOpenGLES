#include "VideoPipelineCamera.h"
#include "VideoFrameCamera.h"
#include "Log.h"
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraMetadata.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>
#include <android/native_window.h>

namespace {

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
}

VideoPipelineCamera::VideoPipelineCamera(int32_t cameraId, int32_t width, int32_t height, VideoFormat format)
    : mCameraId(cameraId)
    , mWidth(width)
    , mHeight(height)
    , mFormat(format) {
}

VideoPipelineCamera::~VideoPipelineCamera() {
    LOG_ENTER();
    if (mRunning) {
        stop();
    }
    LOG_EXIT();
}

void VideoPipelineCamera::start() {
    LOG_ENTER();
    if (mRunning) {
        LOG_EXIT();
        return;
    }

    if (!openCamera()) {
        stop();
        LOG_EXIT();
        return;
    }
    if (!createImageReader()) {
        stop();
        LOG_EXIT();
        return;
    }
    if (!createCaptureSession()) {
        stop();
        LOG_EXIT();
        return;
    }
    if (!startPreview()) {
        stop();
        LOG_EXIT();
        return;
    }

    mRunning = true;
    LOG_EXIT("camera %d started, %dx%d, format: %d", mCameraId, mWidth, mHeight, static_cast<int32_t>(mFormat));
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

    LogI("available cameras: %d", cameraIds->numCameras);
    for (int i = 0; i < cameraIds->numCameras; ++i) {
        LogI("  Camera[%d]: %s", i, cameraIds->cameraIds[i]);
    }

    if (mCameraId >= cameraIds->numCameras) {
        ACameraManager_deleteCameraIdList(cameraIds);
        LOG_EXIT("invalid camera id: %d, total cameras: %d", mCameraId, cameraIds->numCameras);
        return false;
    }

    const char* id = cameraIds->cameraIds[mCameraId];

    ACameraDevice_stateCallbacks deviceCallbacks = {
        .context = this,
        .onDisconnected = VideoPipelineCamera::onDeviceDisconnected,
        .onError = VideoPipelineCamera::onDeviceError
    };
    int ret = ACameraManager_openCamera(manager, id, &deviceCallbacks, reinterpret_cast<ACameraDevice**>(&mCameraDevice));
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
    if (!cameraIds || mCameraId >= cameraIds->numCameras) {
        if (cameraIds) {
            ACameraManager_deleteCameraIdList(cameraIds);
        }
        LOG_EXIT("cannot get camera id list for config check");
        return false;
    }

    const char* id = cameraIds->cameraIds[mCameraId];
    ACameraMetadata* metadata = nullptr;
    ACameraManager_getCameraCharacteristics(reinterpret_cast<ACameraManager*>(mCameraManager), id, &metadata);
    ACameraManager_deleteCameraIdList(cameraIds);

    if (!metadata) {
        LOG_EXIT("failed to get camera metadata");
        return false;
    }

    ACameraMetadata_const_entry entry;
    bool found = false;
    if (ACameraMetadata_getConstEntry(metadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry) == ACAMERA_OK) {
        LogI("supported stream configurations for camera %d:", mCameraId);
        for (size_t i = 0; i < entry.count; i += 4) {
            int32_t fmt = entry.data.i32[i];
            int32_t w = entry.data.i32[i + 1];
            int32_t h = entry.data.i32[i + 2];
            int32_t dir = entry.data.i32[i + 3];
            if (dir == 0) {
                LogI("  format=%s, %dx%d", formatToString(fmt), w, h);
                if (fmt == aFormat && w == mWidth && h == mHeight) {
                    found = true;
                }
            }
        }
    }
    ACameraMetadata_free(metadata);

    if (!found) {
        LOG_EXIT("stream config not supported: format=%s, %dx%d", formatToString(aFormat), mWidth, mHeight);
        return false;
    }

    media_status_t status = AImageReader_new(mWidth, mHeight, aFormat, 2, reinterpret_cast<AImageReader**>(&mImageReader));
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

    int seqId = 0;
    ACameraCaptureSession_setRepeatingRequest(reinterpret_cast<ACameraCaptureSession*>(mCaptureSession), nullptr, 1, &request, &seqId);

    ACameraOutputTarget_free(target);
    ACaptureRequest_free(request);
    LOG_EXIT();
    return true;
}

void VideoPipelineCamera::stop() {
    LOG_ENTER();
    if (!mRunning && !mCameraManager) {
        LOG_EXIT();
        return;
    }

    LogI("camera %d stopping", mCameraId);
    mRunning = false;

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
    LOG_EXIT();
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

void VideoPipelineCamera::handleDeviceDisconnected(ACameraDevice* device) {
    LOG_ENTER("camera device disconnected");
    ACameraDevice_close(device);
    LOG_EXIT();
}

void VideoPipelineCamera::handleDeviceError(ACameraDevice* device, int error) {
    LOG_ENTER("camera device error: %d", error);
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
    LOG_EXIT();
}

void VideoPipelineCamera::handleImageAvailable(AImageReader* reader) {
    LogD("enter");
    AImage* image = nullptr;
    media_status_t status = AImageReader_acquireLatestImage(reader, &image);
    if (status != AMEDIA_OK || !image) {
        LogE("exit: failed to acquire latest image, status: %d", status);
        return;
    }

    auto videoFrame = std::make_shared<VideoFrameCamera>(image);
    dispath(videoFrame);
    LogD("exit");
}
