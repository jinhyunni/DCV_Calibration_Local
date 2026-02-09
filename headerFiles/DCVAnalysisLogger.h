#ifndef __DCVANALYSISLOGGER__
#define __DCVANALYSISLOGGER__

#include <iostream>
#include <mutex>
#include <sstream>
#include <string_view>

namespace logger
{
// ──────────────────────────────
// Color definitions
// ──────────────────────────────
constexpr const char* RESET  = "\033[0m";
constexpr const char* RED    = "\033[31m";
constexpr const char* GREEN  = "\033[32m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* GRAY   = "\033[90m";

// ──────────────────────────────
// Mutex for thread safety
// ──────────────────────────────
inline std::mutex& mx() { static std::mutex m; return m; }

// ──────────────────────────────
// (Optional) Extract only file name from full path
// ──────────────────────────────
inline std::string file_name_only(std::string_view path) {
    size_t pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? std::string(path) : std::string(path.substr(pos + 1));
}

// ──────────────────────────────
// Core logger function
// ──────────────────────────────

inline void write_casual(const char* tag_color,
                  const char* tag,
                  std::string_view msg,
                  const char* file = __FILE__, int line = __LINE__)
{
    std::lock_guard<std::mutex> lk(mx());
    std::cout << tag_color << "[" << tag << "]" << RESET
              << " " << msg
              << " " << GRAY << "(" << file_name_only(file) << ":" << line << ")" << RESET
              << std::endl;
}

inline void write_severe(const char* tag_color,
                  const char* tag,
                  std::string_view msg,
                  const char* file = __FILE__, int line = __LINE__)
{
    std::lock_guard<std::mutex> lk(mx());
    std::cerr << tag_color << "[" << tag << "]" << RESET
              << " " << msg
              << " " << GRAY << "(" << file_name_only(file) << ":" << line << ")" << RESET
              << std::endl;
}

} // namespace logger

// ──────────────────────────────
// Logging macros
// ──────────────────────────────
#define DCV_LOG_INFO(msg)  ::logger::write_casual(::logger::GREEN,  "info",  (std::ostringstream{} << msg).str(), __FILE__, __LINE__)
#define DCV_LOG_WARN(msg)  ::logger::write_casual(::logger::YELLOW, "warn",  (std::ostringstream{} << msg).str(), __FILE__, __LINE__)
#define DCV_LOG_ERROR(msg) ::logger::write_severe(::logger::RED,    "error", (std::ostringstream{} << msg).str(), __FILE__, __LINE__)
#define DCV_LOG_ABORT(msg) do { DCV_LOG_ERROR(msg); std::abort(); } while(0)

#endif // __DCVANALYSISLOGGER__

