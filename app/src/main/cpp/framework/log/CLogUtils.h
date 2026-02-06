//
// Created by banma-3361 on 2025/11/4.
//

#ifndef C_LOG_UTILS_H
#define C_LOG_UTILS_H
#include <android/log.h>

#define KEY(name) #name
#define STRING_ID(name) constexpr const char* name { KEY(name) };
#define KEY_VALUE(key, value) constexpr const char* key { KEY(value) };

STRING_ID(LearnOpenGLES)

#define LogD(...) __android_log_print(ANDROID_LOG_DEBUG, LearnOpenGLES, __VA_ARGS__)
#define LogI(...) __android_log_print(ANDROID_LOG_INFO,  LearnOpenGLES, __VA_ARGS__)
#define LogW(...) __android_log_print(ANDROID_LOG_WARN,  LearnOpenGLES, __VA_ARGS__)
#define LogE(...) __android_log_print(ANDROID_LOG_ERROR, LearnOpenGLES, __VA_ARGS__)


class CLogUtils {
private:
    CLogUtils() = default;
public:
    static CLogUtils& getInstance() {
        static CLogUtils log {};
        return log;
    }
public:
    void i(const char* fmt, ...);
    void e(const char* fmt, ...);
};

#define LogUtils CLogUtils::getInstance()
#endif //C_LOG_UTILS_H
