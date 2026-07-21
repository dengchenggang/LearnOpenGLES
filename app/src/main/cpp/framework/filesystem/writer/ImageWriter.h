#ifndef C_IMAGE_WRITER_H
#define C_IMAGE_WRITER_H

#include <cstdint>

class ImageWriter {
public:
    ImageWriter() = delete;
    ImageWriter(const ImageWriter&) = delete;
    ImageWriter& operator=(const ImageWriter&) = delete;

    static bool writePNG(const char* filename, int width, int height, int channels, const uint8_t* data);
    static bool writeJPG(const char* filename, int width, int height, int channels, const uint8_t* data, int quality = 90);
    static bool writeBMP(const char* filename, int width, int height, int channels, const uint8_t* data);
    static bool writeTGA(const char* filename, int width, int height, int channels, const uint8_t* data);
};

#endif
