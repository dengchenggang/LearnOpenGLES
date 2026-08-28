#ifndef C_IMAGE_ENCODER_H
#define C_IMAGE_ENCODER_H

#include <cstdint>

namespace framework {

class ImageEncoder {
public:
    ImageEncoder() = delete;
    ImageEncoder(const ImageEncoder&) = delete;
    ImageEncoder& operator=(const ImageEncoder&) = delete;

    static bool writePNG(const char* filename, int width, int height, int channels, const uint8_t* data);
    static bool writeJPG(const char* filename, int width, int height, int channels, const uint8_t* data, int quality = 90);
    static bool writeBMP(const char* filename, int width, int height, int channels, const uint8_t* data);
    static bool writeTGA(const char* filename, int width, int height, int channels, const uint8_t* data);
};

} // namespace framework

#endif
