#include "CLogUtils.h"
#include <android/log.h>
#include <cstdarg>

constexpr const char* TAG {"[LearnOpenGLES]"};

void CLogUtils::i(const char* fmt, ...) {
    std::va_list ap;
    va_start(ap, fmt);
    LogI(fmt, ap);
    va_end(ap);
}

void CLogUtils::e(const char* fmt, ...) {
    std::va_list ap;
    va_start(ap, fmt);
    LogE(fmt, ap);
    va_end(ap);
}