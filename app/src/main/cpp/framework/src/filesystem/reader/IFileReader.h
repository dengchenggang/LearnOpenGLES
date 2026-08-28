#ifndef I_FILE_READER_H
#define I_FILE_READER_H

#include "filesystem/FileData.h"
#include <string>
#include <memory>

namespace framework {

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

} // namespace framework

#endif // I_FILE_READER_H
