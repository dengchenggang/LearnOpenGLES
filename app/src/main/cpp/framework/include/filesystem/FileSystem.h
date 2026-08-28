#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Singleton.hpp"
#include "FileData.h"
#include <memory>
#include <vector>

namespace framework {

class IFileReader;
class FileWriter;

// 文件系统代理类
class FileSystem {
    friend class Singleton<FileSystem>;
public:
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;
public:
    // 设置文件读取器（通常在初始化时调用一次）
    void SetReader(void* context);
    void SetReader(const std::string& rootPath);
    bool SetWriter(const std::string& rootPath);

    // 重置文件系统（清空所有读取器和写入器）
    void reset();

    // 便捷方法：直接访问文件
    std::unique_ptr<FileData> readFile(const char* filePath);
    std::string readString(const char* filePath);
    bool exists(const char* filePath);
    size_t getFileSize(const char* filePath);

private:
    FileSystem();
    ~FileSystem();

private:
    std::vector<std::unique_ptr<IFileReader>> mReaders;
    std::unique_ptr<FileWriter> mWriter;
};

} // namespace framework

// 全局访问宏
#define FileSystem Singleton<framework::FileSystem>::getInstance()

#endif // FILE_SYSTEM_H
