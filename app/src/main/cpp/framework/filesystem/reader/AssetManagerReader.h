#ifndef ASSET_MANAGER_READER_H
#define ASSET_MANAGER_READER_H

#include "IFileReader.h"
#include <mutex>

// Android Assets 文件读取实现
class AssetManagerReader : public IFileReader {
public:
    AssetManagerReader() = default;
    ~AssetManagerReader() override = default;

    // 禁止拷贝和移动
    AssetManagerReader(const AssetManagerReader&) = delete;
    AssetManagerReader& operator=(const AssetManagerReader&) = delete;
    AssetManagerReader(AssetManagerReader&&) = delete;
    AssetManagerReader& operator=(AssetManagerReader&&) = delete;

    // 初始化（传入 AAssetManager）
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

private:
    void* mAssetManager = nullptr;
    mutable std::mutex mMutex;
};

#endif // ASSET_MANAGER_READER_H
