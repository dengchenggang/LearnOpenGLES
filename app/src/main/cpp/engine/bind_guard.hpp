namespace engine {
namespace utils {

template<typename T>
class bind_guard {
private:
    const T& mBinder;
public:
    bind_guard(const T& binder) : mBinder(binder)
    {
        mBinder.bind();
    }

    ~bind_guard() {
        mBinder.unbind();
    }

};

}  // namespace utils
}  // namespace engine