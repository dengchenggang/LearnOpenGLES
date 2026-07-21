#ifndef C_FILE_WRITER_H
#define C_FILE_WRITER_H

#include <cstdint>
#include <string>

class FileWriter {
public:
    explicit FileWriter(const std::string& rootPath);
    ~FileWriter() = default;
    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;

    bool write(const std::string& filePath, const std::string& content);
    bool write(const std::string& filePath, const char* data, size_t length);
    bool write(const std::string& filePath, const uint8_t* data, size_t size);
    bool write(const std::string& filePath, int width, int height, int channels, const uint8_t* data, int quality = 90);

private:
    std::string mRootPath;
};

#endif
