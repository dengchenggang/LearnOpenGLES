#include "VideoFrameCamera.h"
#include <android/hardware_buffer.h>

namespace framework {
VideoFormat toVideoFormat(int32_t aimageFormat) {
    switch (aimageFormat) {
        case AIMAGE_FORMAT_RGBA_8888:
            return VideoFormat::RGBA_8888;
        case AIMAGE_FORMAT_RGB_888:
            return VideoFormat::RGB_888;
        case AIMAGE_FORMAT_YUV_420_888:
            return VideoFormat::YUV_420_888;
        default:
            return VideoFormat::Unknown;
    }
}

VideoFrameCamera::VideoFrameCamera(AImage* image, int32_t width, int32_t height, VideoFormat format)
    : VideoFrame(width, height, format)
    , mAImage(image) {
}

VideoFrameCamera::~VideoFrameCamera() {
    if (mAImage) {
        AImage_delete(mAImage);
    }
}

const uint8_t* VideoFrameCamera::getData() const {
    if (!mAImage) {
        return nullptr;
    }

    int32_t format = 0;
    AImage_getFormat(mAImage, &format);

    switch (format) {
        case AIMAGE_FORMAT_RGBA_8888:
        case AIMAGE_FORMAT_RGB_888: {
            uint8_t* data = nullptr;
            int32_t len = 0;
            AImage_getPlaneData(mAImage, 0, &data, &len);
            return data;
        }
        default:
            return nullptr;
    }
}

} // namespace framework


