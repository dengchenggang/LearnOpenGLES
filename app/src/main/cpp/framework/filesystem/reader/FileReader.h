#ifndef FILE_READER_H
#define FILE_READER_H

#include "IFileReader.h"
#include <mutex>
#include <string>

// C++17 filesystem 文件读取实现（用于桌面平台）
class FileReader : public IFileReader {
public:
    explicit FileReader(const std::string& rootPath) : mRootPath(rootPath) {}
    ~FileReader() override = default;
    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;
public:
    // 读取整个文件
    std::unique_ptr<FileData> readFile(const char* filePath) override;

    // 读取为字符串
    std::string readString(const char* filePath) override;

    // 检查文件是否存在
    bool exists(const char* filePath) override;

    // 获取文件大小
    size_t getFileSize(const char* filePath) override;

    // 设置根目录（可选）
    void setRootPath(const char* rootPath);

private:
    std::string mRootPath;
    mutable std::mutex mMutex;

    // 构建完整路径
    std::string buildFullPath(const char* filePath) const;
};

#endif // FILE_READER_H
