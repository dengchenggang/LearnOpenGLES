#ifndef I_FILE_READER_H
#define I_FILE_READER_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

// 文件数据封装
struct FileData {
    std::vector<uint8_t> data;
    
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
    const uint8_t* ptr() const { return data.data(); }
    uint8_t* ptr() { return data.data(); }
    
    // 转为字符串
    std::string asString() const {
        if (data.empty()) return "";
        return std::string(reinterpret_cast<const char*>(data.data()), data.size());
    }
};

// 文件读取接口
class IFileReader {
public:
    virtual ~IFileReader() = default;
    
    // 初始化
    virtual void initialize(void* context) = 0;
    
    // 检查是否已初始化
    virtual bool isInitialized() const = 0;
    
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
