#ifndef C_VIDEO_FRAME_CAMERA_H
#define C_VIDEO_FRAME_CAMERA_H
#include "VideoFrame.h"
#include <media/NdkImage.h>

namespace framework {

class VideoFrameCamera : public VideoFrame {
public:
    explicit VideoFrameCamera(AImage* image);
    ~VideoFrameCamera() override;
    VideoFrameCamera(const VideoFrameCamera&) = delete;
    VideoFrameCamera& operator = (const VideoFrameCamera&) = delete;
public:
    const uint8_t* getData() const override;
private:
    AImage* mAImage;
};

} // namespace framework

#endif
