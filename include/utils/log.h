#pragma once
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>
#include <functional>

// in a header before your LOG_* macros
#if defined(_WIN32)
  #define __FILENAME__ (std::strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
  #define __FILENAME__ (std::strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/')  + 1 : __FILE__)
#endif

enum class LogLevel : uint8_t { Error=0, Warning=1, Info=2, Debug=3 };

// External sink signature. Receives a ready-to-print UTF-8 line (no newline).
using LogSink = std::function<void(LogLevel level, const char* message)>;

// Configuration
void        log_set_level(LogLevel lvl);
LogLevel    log_get_level();
void        log_set_sink(LogSink sink);

// Emission
void        log_printf(LogLevel lvl, const char* fmt, ...);
void        log_vprintf(LogLevel lvl, const char* fmt, va_list ap);

// Convenience macros with file:line tag
void        log_printf_tag(LogLevel lvl, const char* file, int line, const char* fmt, ...);

#define LOG_SET_LEVEL(lvl)     log_set_level(lvl)
#define LOG_ERROR(...)        log_printf_tag(LogLevel::Error,   __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)         log_printf_tag(LogLevel::Warning, __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)         log_printf_tag(LogLevel::Info,    __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)        log_printf_tag(LogLevel::Debug,   __FILENAME__, __LINE__, __VA_ARGS__)

// Hex preview helper (prints up to max_bytes as hex)
void log_hex_preview(LogLevel lvl, const void* data, size_t len, size_t max_bytes = 64);