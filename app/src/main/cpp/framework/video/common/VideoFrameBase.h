#ifndef C_VIDEO_FRAME_BASE_H
#define C_VIDEO_FRAME_BASE_H
#include <cstdint>

enum class VideoFormat : int32_t {
    RGBA_8888 = 1,    // AIMAGE_FORMAT_RGBA_8888
    RGB_888 = 3,      // AIMAGE_FORMAT_RGB_888
    YUV_420_888 = 35, // AIMAGE_FORMAT_YUV_420_888
    Unknown = 0
};

class VideoFrameBase {
public:
    VideoFrameBase(int32_t width, int32_t height, VideoFormat format, int64_t timestamp, bool hasHardwareBuffer)
        : mWidth(width)
        , mHeight(height)
        , mFormat(format)
        , mSize(calculateSize(width, height, format))
        , mHasHardwareBuffer(hasHardwareBuffer)
        , mTimestamp(timestamp){}
    virtual ~VideoFrameBase() = default;
    VideoFrameBase(const VideoFrameBase&) = delete;
    VideoFrameBase& operator = (const VideoFrameBase&) = delete;
public:
    virtual const uint8_t* getData() const = 0;
    virtual void* getHardwareBuffer() const = 0;

    int32_t getWidth()          const { return mWidth; }
    int32_t getHeight()         const { return mHeight; }
    VideoFormat getFormat()     const { return mFormat; }
    int64_t getTimestamp()      const { return mTimestamp; }
    size_t  getSize()           const { return mSize; }
    bool    hasHardwareBuffer() const { return mHasHardwareBuffer; }

    void resetTimestamp(int64_t timestamp) { mTimestamp = timestamp; }
public:
    static size_t calculateSize(int32_t width, int32_t height, VideoFormat format) {
        switch (format) {
            case VideoFormat::RGBA_8888:
                return static_cast<size_t>(width) * height * 4;
            case VideoFormat::RGB_888:
                return static_cast<size_t>(width) * height * 3;
            case VideoFormat::YUV_420_888:
                return static_cast<size_t>(width) * height * 3 / 2;
            default:
                return static_cast<size_t>(width) * height * 4;
        }
    }
private:
    const int32_t mWidth;
    const int32_t mHeight;
    const VideoFormat mFormat;
    const size_t  mSize;
    const bool mHasHardwareBuffer;
    int64_t mTimestamp;
};

#endif
