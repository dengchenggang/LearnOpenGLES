#ifndef C_VIDEO_FRAME_CAMERA_H
#define C_VIDEO_FRAME_CAMERA_H
#include "VideoFrameBase.h"
#include <media/NdkImage.h>

class VideoFrameCamera : public VideoFrameBase {
public:
    explicit VideoFrameCamera(AImage* image);
    ~VideoFrameCamera() override;
    VideoFrameCamera(const VideoFrameCamera&) = delete;
    VideoFrameCamera& operator = (const VideoFrameCamera&) = delete;
public:
    const uint8_t* getData()  const override;
    void* getHardwareBuffer() const override;
private:
    AImage* mAImage;
};

#endif
