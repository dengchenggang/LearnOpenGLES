#ifndef I_FILE_READER_H
#define I_FILE_READER_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
#include <functional>

// 文件数据封装
struct FileData {
    std::unique_ptr<std::uint8_t[], std::function<void(std::uint8_t*)>> buffer;
    size_t bufferSize = 0;

    bool empty() const { return bufferSize == 0 || !buffer; }
    size_t size() const { return bufferSize; }
    const uint8_t* data() const { return buffer.get(); }
    uint8_t* data() { return buffer.get(); }
};

// 文件读取接口
class IFileReader {
public:
    virtual ~IFileReader() = default;

    // 读取整个文件
    virtual FileData readFile(const char* filePath) = 0;

    // 读取为字符串
    virtual std::string readString(const char* filePath) = 0;

    // 检查文件是否存在
    virtual bool exists(const char* filePath) = 0;

    // 获取文件大小
    virtual size_t getFileSize(const char* filePath) = 0;
};

#endif // I_FILE_READER_H
