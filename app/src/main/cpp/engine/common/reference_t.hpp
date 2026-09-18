#include <memory>

namespace engine {

template <typename T>
struct reference_t
{
    T& ref;

    explicit reference_t(T& ref) : ref(ref) {}
};

template <typename T>
using reference_ptr_t = std::unique_ptr<reference_t<T>>;

}