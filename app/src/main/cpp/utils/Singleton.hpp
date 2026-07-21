#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>

template<typename T>
class Singleton {
public:
    Singleton(const Singleton<T>&) = delete;
    Singleton(Singleton<T>&&) = delete;
    Singleton<T>& operator= (const Singleton<T>&) = delete;
    Singleton<T>& operator= (Singleton<T>&&) = delete;
public:
    static T& getInstance() {
        static T sInstance{};
        return sInstance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
};
#endif

