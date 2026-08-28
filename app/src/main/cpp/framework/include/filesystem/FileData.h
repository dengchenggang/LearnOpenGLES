#ifndef FILE_DATA_H
#define FILE_DATA_H

#include <cstdint>
#include <cstddef>
#include <memory>
#include <functional>

namespace framework {

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

} // namespace framework

#endif // FILE_DATA_H
