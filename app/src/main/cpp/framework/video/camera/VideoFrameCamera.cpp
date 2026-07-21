#include "VideoFrameCamera.h"
#include <android/hardware_buffer.h>

namespace {
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
}

VideoFrameCamera::VideoFrameCamera(AImage* image)
    : VideoFrameBase(
        [&]() { int32_t w = 0; if (image) AImage_getWidth(image, &w); return w; }(),
        [&]() { int32_t h = 0; if (image) AImage_getHeight(image, &h); return h; }(),
        [&]() { int32_t f = 0; if (image) AImage_getFormat(image, &f); return toVideoFormat(f); }(),
        [&]() { int64_t t = 0; if (image) AImage_getTimestamp(image, &t); return t; }(),
        true
      )
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

void* VideoFrameCamera::getHardwareBuffer() const {
    if (!mAImage) {
        return nullptr;
    }

    AHardwareBuffer* buffer = nullptr;
    AImage_getHardwareBuffer(mAImage, &buffer);
    return buffer;
}
