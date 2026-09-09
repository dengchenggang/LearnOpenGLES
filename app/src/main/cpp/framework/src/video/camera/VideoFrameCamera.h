#ifndef C_VIDEO_FRAME_CAMERA_H
#define C_VIDEO_FRAME_CAMERA_H
#include "video/VideoFrame.h"
#include <media/NdkImage.h>

namespace framework {

VideoFormat toVideoFormat(int32_t aimageFormat);

class VideoFrameCamera : public VideoFrame {
public:
    VideoFrameCamera(AImage* image, int32_t width, int32_t height, VideoFormat format);
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
