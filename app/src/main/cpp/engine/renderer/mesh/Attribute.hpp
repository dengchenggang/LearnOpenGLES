#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include "IRenderInterface.h"
#include <cstddef>

namespace engine {
namespace renderer {


// 顶点属性定义
struct Attribute {
    uint32_t location;      // 着色器 location
    int32_t  size;          // 分量数 (1-4)
    DataType type;          // 数据类型
    bool     normalized;    // 是否归一化

    Attribute(uint32_t loc, int32_t sz, DataType t, bool norm = false)
        : location(loc), size(sz), type(t), normalized(norm) {}
};

// 便捷预设
namespace Attr {
    inline Attribute Position(uint32_t loc = 0) {
        return Attribute(loc, 3, DataType::Float, false);
    }
    inline Attribute Color(uint32_t loc = 1) {
        return Attribute(loc, 4, DataType::Float, false);
    }
    inline Attribute TexCoord(uint32_t loc = 2) {
        return Attribute(loc, 2, DataType::Float, false);
    }
    inline Attribute Normal(uint32_t loc = 3) {
        return Attribute(loc, 3, DataType::Float, false);
    }
}


} // namespace renderer
} // namespace engine

#endif