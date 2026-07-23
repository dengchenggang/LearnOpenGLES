#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "ImageEncoder.h"

bool ImageEncoder::writePNG(const char* filename, int width, int height, int channels, const uint8_t* data) {
    return stbi_write_png(filename, width, height, channels, data, width * channels) != 0;
}

bool ImageEncoder::writeJPG(const char* filename, int width, int height, int channels, const uint8_t* data, int quality) {
    return stbi_write_jpg(filename, width, height, channels, data, quality) != 0;
}

bool ImageEncoder::writeBMP(const char* filename, int width, int height, int channels, const uint8_t* data) {
    return stbi_write_bmp(filename, width, height, channels, data) != 0;
}

bool ImageEncoder::writeTGA(const char* filename, int width, int height, int channels, const uint8_t* data) {
    return stbi_write_tga(filename, width, height, channels, data) != 0;
}
