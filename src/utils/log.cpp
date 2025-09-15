#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>

#include "utils/log.h"

static std::atomic<LogLevel> g_level{LogLevel::Info};
static std::mutex g_mu;
static LogSink g_sink = nullptr;

// Default sink: prints timestamp, level, and message to stderr.
static const char *lvl_name(LogLevel l)
{
  switch (l)
  {
  case LogLevel::Error:
    return "ERROR";
  case LogLevel::Warning:
    return "WARN ";
  case LogLevel::Info:
    return "INFO ";
  default:
    return "DEBUG";
  }
}

static void default_sink(LogLevel lvl, const char *msg)
{
  using namespace std::chrono;
  auto now = system_clock::now();
  std::time_t t = system_clock::to_time_t(now);
  std::tm tm{};
#if defined(_WIN32)
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif
  char ts[32];
  std::snprintf(ts, sizeof(ts), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min,
                tm.tm_sec);
  std::fprintf(stderr, "[%s] %s %s\n", ts, lvl_name(lvl), msg);
  std::fflush(stderr);
}

void log_set_level(LogLevel lvl)
{
  g_level.store(lvl, std::memory_order_relaxed);
}

LogLevel log_get_level() { return g_level.load(std::memory_order_relaxed); }

void log_set_sink(LogSink sink)
{
  std::lock_guard<std::mutex> lk(g_mu);
  g_sink = sink;
}

static void emit(LogLevel lvl, const char *line)
{
  if ((int)lvl > (int)log_get_level())
    return;
  std::lock_guard<std::mutex> lk(g_mu);
  (g_sink ? g_sink : default_sink)(lvl, line);
}

void log_vprintf(LogLevel lvl, const char *fmt, va_list ap)
{
  if ((int)lvl > (int)log_get_level())
    return;
  // format into dynamic buffer
  va_list ap2;
  va_copy(ap2, ap);
  int n = std::vsnprintf(nullptr, 0, fmt, ap2);
  va_end(ap2);
  if (n < 0)
    return;
  std::vector<char> buf((size_t)n + 1);
  std::vsnprintf(buf.data(), buf.size(), fmt, ap);
  emit(lvl, buf.data());
}

void log_printf(LogLevel lvl, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  log_vprintf(lvl, fmt, ap);
  va_end(ap);
}

void log_printf_tag(LogLevel lvl, const char *file, int line, const char *fmt,
                    ...)
{
  if ((int)lvl > (int)log_get_level())
    return;
  // format user message first
  va_list ap;
  va_start(ap, fmt);
  int n = std::vsnprintf(nullptr, 0, fmt, ap);
  va_end(ap);
  if (n < 0)
    return;
  std::string msg;
  msg.resize((size_t)n);
  va_start(ap, fmt);
  std::vsnprintf(msg.data(), msg.size() + 1, fmt, ap);
  va_end(ap);
  // prefix with file:line
  char head[256];
  std::snprintf(head, sizeof(head), "%s:%d %s", file, line, msg.c_str());
  emit(lvl, head);
}

void log_hex_preview(LogLevel lvl, const void *data, size_t len,
                     size_t max_bytes)
{
  if ((int)lvl > (int)log_get_level())
    return;
  const size_t n = len < max_bytes ? len : max_bytes;
  const uint8_t *p = static_cast<const uint8_t *>(data);
  std::string s;
  s.reserve(n * 3 + 16);
  char byte[4];
  for (size_t i = 0; i < n; ++i)
  {
    std::snprintf(byte, sizeof(byte), "%02X", p[i]);
    s += byte;
    if (i + 1 < n)
      s += ' ';
  }
  if (n < len)
    s += " …";
  emit(lvl, s.c_str());
}
