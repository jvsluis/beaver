#pragma once

#include <atomic>

#include "beaver/core/platform.h"

#if BEAVER_PLATFORM_WINDOWS
    #include <io.h>
    #define RAW_WRITE(fd, buf, len) _write(fd, buf, (unsigned int)(len))
    #define STDERR_FD 2
#else
    #include <unistd.h>
    #define RAW_WRITE(fd, buf, len) write(fd, buf, len)
    #define STDERR_FD STDERR_FILENO
#endif

namespace bvr::core::logging {

enum class Level { Info,
                   Warning,
                   Error,
                   Fatal };

class Logger {
public:
    // Call this in the signal/crash handler to bypass locks
    static void set_crash_mode();

    static void log(Level level, const char* format, ...);

private:
    static const char*
    get_level_string(Level level) {
        switch (level) {
            case Level::Info:
                return "INFO ";
            case Level::Warning:
                return "WARN ";
            case Level::Error:
                return "ERROR";
            case Level::Fatal:
                return "FATAL";
            default:
                return "UNKWN";
        }
    }

    static std::atomic_flag s_spinlock;
    static std::atomic<bool> s_iscrashing;
};

}  // namespace bvr::core::logging

// #ifdef ENABLE_LOGGING
#define CORE_INFO(msg, ...) bvr::core::logging::Logger::log(bvr::core::logging::Level::Info, msg, ##__VA_ARGS__);
#define CORE_WARN(msg, ...) bvr::core::logging::Logger::log(bvr::core::logging::Level::Warning, msg, ##__VA_ARGS__);
#define CORE_ERROR(msg, ...) bvr::core::logging::Logger::log(bvr::core::logging::Level::Error, msg, ##__VA_ARGS__);
#define CORE_FATAL(msg, ...) bvr::core::logging::Logger::log(bvr::core::logging::Level::Fatal, msg, ##__VA_ARGS__);
// #endif
