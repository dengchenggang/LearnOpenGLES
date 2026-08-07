#ifndef I_FILE_READER_H
#define I_FILE_READER_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
#include <functional>

namespace framework {
namespace filesystem {

// 文件数据封装
using FileDataBufferType = std::unique_ptr<std::uint8_t[], std::function<void(std::uint8_t*)>>;

struct FileData {
    const FileDataBufferType buffer;
    const size_t bufferSize;

    FileData(FileDataBufferType&& buffer, size_t bufferSize)
        : buffer(std::move(buffer))
        , bufferSize(bufferSize) {}

    bool empty() const { return bufferSize == 0 || !buffer; }
    size_t size() const { return bufferSize; }
    const uint8_t* data() const { return buffer.get(); }
};

enum class EImageDataFormat {
    RGB_888,
    RGBA_8888
};

struct ImageData : public FileData {
    const int32_t width;
    const int32_t height;
    const EImageDataFormat format;

    ImageData(FileDataBufferType&& buffer, size_t bufferSize, const int32_t width, const int32_t height, const EImageDataFormat format)
        : FileData(std::move(buffer), bufferSize)
        , width(width)
        , height(height)
        , format(format) {}
};

// 文件读取接口
class IFileReader {
public:
    virtual ~IFileReader() = default;

    // 读取整个文件
    virtual std::unique_ptr<FileData> readFile(const char* filePath) = 0;

    // 读取为字符串
    virtual std::string readString(const char* filePath) = 0;

    // 检查文件是否存在
    virtual bool exists(const char* filePath) = 0;

    // 获取文件大小
    virtual size_t getFileSize(const char* filePath) = 0;
};

} // namespace filesystem
} // namespace framework

#endif // I_FILE_READER_H
