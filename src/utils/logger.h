#pragma once

#include <string>
#include <format>

template<typename T>
concept StringLike = std::convertible_to<T, std::string> || std::convertible_to<T, std::string_view>;

namespace LogHelper {
    template<StringLike T>
    inline std::string formatLog(const T& msg) {
        return std::string(msg);
    }

    template<typename... Args>
    inline std::string formatLog(std::format_string<Args...> fmt, Args&&... args) {
        return std::format(fmt, std::forward<Args>(args)...);
    }
}

#ifdef kANDROID_LOG
#include <android/log.h>
#define TAG "DUMPA1N"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, LogHelper::formatLog(__VA_ARGS__).c_str())
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, LogHelper::formatLog(__VA_ARGS__).c_str())
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  TAG, LogHelper::formatLog(__VA_ARGS__).c_str())
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, LogHelper::formatLog(__VA_ARGS__).c_str())
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, LogHelper::formatLog(__VA_ARGS__).c_str())
#else
#define LOGD(...) printf("%s", ("D: " + LogHelper::formatLog(__VA_ARGS__)).c_str())
#define LOGI(...) printf("%s", ("I: " + LogHelper::formatLog(__VA_ARGS__)).c_str())
#define LOGW(...) printf("%s", ("W: " + LogHelper::formatLog(__VA_ARGS__)).c_str())
#define LOGE(...) printf("%s", ("E: " + LogHelper::formatLog(__VA_ARGS__)).c_str())
#define LOGF(...) printf("%s", ("F: " + LogHelper::formatLog(__VA_ARGS__)).c_str())
#endif
