//
// Created by banma-3361 on 2025/11/4.
//

#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#include <android/log.h>

#define KEY(name) #name
#define STRING_ID(name) constexpr const char* name { KEY(name) };
#define KEY_VALUE(key, value) constexpr const char* key { KEY(value) };

STRING_ID(LearnOpenGLES)

#define _LogD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LearnOpenGLES, fmt, ##__VA_ARGS__)
#define _LogI(fmt, ...) __android_log_print(ANDROID_LOG_INFO,  LearnOpenGLES, fmt, ##__VA_ARGS__)
#define _LogW(fmt, ...) __android_log_print(ANDROID_LOG_WARN,  LearnOpenGLES, fmt, ##__VA_ARGS__)
#define _LogE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LearnOpenGLES, fmt, ##__VA_ARGS__)

#define LogD(fmt, ...) _LogD("%s:%d %s: " fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)
#define LogI(fmt, ...) _LogI("%s:%d %s: " fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)
#define LogW(fmt, ...) _LogW("%s:%d %s: " fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)
#define LogE(fmt, ...) _LogE("%s:%d %s: " fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)

#define __LOG_ENTER__(_unused, fmt, ...) _LogI("%s:%d %s enter" fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_ENTER(...) __LOG_ENTER__(0, __VA_ARGS__)

#define __LOG_EXIT__(_unused, fmt, ...) _LogI("%s:%d %s exit" fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_EXIT(...) __LOG_EXIT__(0, __VA_ARGS__)

#endif // LOG_UTILS_H
