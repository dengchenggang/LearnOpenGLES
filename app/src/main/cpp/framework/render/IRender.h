#include <cstdint>

class IRender {
public:
    IRender() = default;
    virtual ~IRender() = default;
    IRender(const IRender&) = delete;
    IRender(IRender&&) = delete;
public:
    virtual void initialize(int width, int height) = 0;
    virtual void resize(int width, int height) = 0;
    virtual void update(std::int64_t deltaTime) = 0;
    virtual void render(std::int64_t deltaTime) = 0;
    virtual void destroy() = 0;
};
