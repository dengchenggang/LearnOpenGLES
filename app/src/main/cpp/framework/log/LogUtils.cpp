#include "LogUtils.h"
#include <android/log.h>
#include <cstdarg>

constexpr const char* TAG {"[LearnOpenGLES]"};

void LogUtils::i(const char* fmt, ...) {
    std::va_list ap;
    va_start(ap, fmt);
    LogI(fmt, ap);
    va_end(ap);
}

void LogUtils::e(const char* fmt, ...) {
    std::va_list ap;
    va_start(ap, fmt);
    LogE(fmt, ap);
    va_end(ap);
}