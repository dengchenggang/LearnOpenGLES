#ifndef C_VIDEO_PIPELINE_CAMERA_H
#define C_VIDEO_PIPELINE_CAMERA_H
#include "VideoPipelineBase.h"

struct AImageReader;
struct ACameraDevice;
struct ACameraCaptureSession;

class VideoPipelineCamera : public VideoPipelineBase {
public:
    VideoPipelineCamera(int32_t cameraId, int32_t width, int32_t height, VideoFormat format = VideoFormat::RGBA_8888);
    ~VideoPipelineCamera() override;
    VideoPipelineCamera(const VideoPipelineCamera&) = delete;
    VideoPipelineCamera& operator = (const VideoPipelineCamera&) = delete;
public:
    void start() override;
    void stop() override;
private:
    void handleImageAvailable(AImageReader* reader);
    void handleDeviceDisconnected(ACameraDevice* device);
    void handleDeviceError(ACameraDevice* device, int error);
    void handleSessionActive(ACameraCaptureSession* session);
    void handleSessionReady(ACameraCaptureSession* session);
    void handleSessionClosed(ACameraCaptureSession* session);

    bool openCamera();
    bool createImageReader();
    bool createCaptureSession();
    bool startPreview();

private:
    static void onImageAvailable(void* context, AImageReader* reader);
    static void onDeviceDisconnected(void* context, ACameraDevice* device);
    static void onDeviceError(void* context, ACameraDevice* device, int error);
    static void onSessionActive(void* context, ACameraCaptureSession* session);
    static void onSessionReady(void* context, ACameraCaptureSession* session);
    static void onSessionClosed(void* context, ACameraCaptureSession* session);

private:
    int32_t mCameraId;
    int32_t mWidth;
    int32_t mHeight;
    VideoFormat mFormat;

    void* mCameraManager = nullptr;
    void* mCameraDevice = nullptr;
    void* mCaptureSession = nullptr;
    void* mImageReader = nullptr;
    void* mNativeWindow = nullptr;
    bool mRunning = false;
};
#endif
