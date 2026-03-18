#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Singleton.hpp"
#include "IFileReader.h"
#include <memory>

// 文件系统代理类
class FileSystemProxy {
    friend class Singleton<FileSystemProxy>;
public:
    FileSystemProxy(const FileSystemProxy&) = delete;
    FileSystemProxy& operator=(const FileSystemProxy&) = delete;

    // 设置文件读取器（通常在初始化时调用一次）
    void setReader(std::unique_ptr<IFileReader> reader);

    // 获取文件读取器
    IFileReader* getReader() const;

    // 便捷方法：直接访问文件
    FileData readFile(const char* filePath);
    std::string readString(const char* filePath);
    bool exists(const char* filePath);
    size_t getFileSize(const char* filePath);

private:
    FileSystemProxy() = default;
    ~FileSystemProxy() = default;

private:
    std::unique_ptr<IFileReader> mReader;
};

// 全局访问宏
#define FileSystem Singleton<FileSystemProxy>::getInstance()

#endif // FILE_SYSTEM_H
