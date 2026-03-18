#ifndef FILE_READER_H
#define FILE_READER_H

#include "IFileReader.h"
#include <mutex>
#include <string>

// C++17 filesystem 文件读取实现（用于桌面平台）
class FileReader : public IFileReader {
public:
    FileReader() = default;
    ~FileReader() override = default;

    // 禁止拷贝和移动
    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;
    FileReader(FileReader&&) = delete;
    FileReader& operator=(FileReader&&) = delete;

    // 初始化（传入根目录路径，可为 nullptr 使用当前目录）
    void initialize(void* context) override;

    // 检查是否已初始化
    bool isInitialized() const override;

    // 读取整个文件
    FileData readFile(const char* filePath) override;

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
    bool mInitialized = false;

    // 构建完整路径
    std::string buildFullPath(const char* filePath) const;
};

#endif // FILE_READER_H
