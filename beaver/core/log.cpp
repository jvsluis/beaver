#include "beaver/core/log.h"

#include <cstdarg>
#include <cstdio>

namespace bvr::core::logging {

std::atomic_flag Logger::s_spinlock = ATOMIC_FLAG_INIT;
std::atomic<bool> Logger::s_iscrashing{false};

void Logger::set_crash_mode() {
    s_iscrashing.store(true, std::memory_order_relaxed);
}

void Logger::log(Level level, const char* format, ...) {
    // Use a fixed stack buffer for formatting to avoid heap
    char buffer[1024];
    int offset = 0;

    const char* levelStr = get_level_string(level);
    offset = snprintf(buffer, sizeof(buffer), "%s ", levelStr);
    if (offset < 0) offset = 0;

    va_list args;
    va_start(args, format);
    int written = vsnprintf(buffer + offset, sizeof(buffer) - offset - 1, format, args);
    va_end(args);

    if (written > 0) {
        offset += written;
    }

    // Ensure truncation doesn't overflow and add newline
    if (offset >= sizeof(buffer) - 1) {
        offset = sizeof(buffer) - 2;
    }
    buffer[offset++] = '\n';
    buffer[offset] = '\0';

    bool bypassLock = s_iscrashing.load(std::memory_order_relaxed);

    if (!bypassLock) {
        // lightweight spinlock to avoid allocations in the logger
        while (s_spinlock.test_and_set(std::memory_order_acquire)) {
#if defined(__x86_64__) || defined(_M_X64)
            // Hint to CPU to save power during spin
            __builtin_ia32_pause();
#endif
        }
    }

    // Low-level write (async signal safe)
    RAW_WRITE(STDERR_FD, buffer, offset);

    if (!bypassLock) {
        s_spinlock.clear(std::memory_order_release);
    }
}

}  // namespace bvr::core::logging
