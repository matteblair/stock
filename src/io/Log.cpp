//
// Created by Matt Blair on 4/29/16.
//
#include "io/Log.hpp"

namespace stock {

Log::Level Log::s_level = Log::Level::WARNING;

#define PRINT_STRING(str) do { \
    std::fprintf(stderr, "%s\n", str); \
} while (0)

#define PRINT_VARIADIC(format) do { \
    va_list args; \
    va_start(args, format); \
    std::vfprintf(stderr, format, args); \
    va_end(args); \
} while(0)

void Log::write(Level level, const std::string& message) {
    if (level < s_level) { return; }
    PRINT_STRING(message.c_str());
}

void Log::v(const std::string& message) {
    if (Level::VERBOSE < s_level) { return; }
    PRINT_STRING(message.c_str());
}

void Log::d(const std::string& message) {
    if (Level::DEBUGGING < s_level) { return; }
    PRINT_STRING(message.c_str());
}

void Log::w(const std::string& message) {
    if (Level::WARNING < s_level) { return; }
    PRINT_STRING(message.c_str());
}

void Log::e(const std::string& message) {
    if (Level::ERROR < s_level) { return; }
    PRINT_STRING(message.c_str());
}

void Log::a(const std::string& message) {
    if (Level::ASSERT < s_level) { return; }
    PRINT_STRING(message.c_str());
}

void Log::writef(Level level, const char* format, ...) {
    if (level < s_level) { return; }
    PRINT_VARIADIC(format);
}

void Log::vf(const char* format, ...) {
    if (Level::VERBOSE < s_level) { return; }
    PRINT_VARIADIC(format);
}

void Log::df(const char* format, ...) {
    if (Level::DEBUGGING < s_level) { return; }
    PRINT_VARIADIC(format);
}

void Log::wf(const char* format, ...) {
    if (Level::DEBUGGING < s_level) { return; }
    PRINT_VARIADIC(format);
}

void Log::ef(const char* format, ...) {
    if (Level::ERROR < s_level) { return; }
    PRINT_VARIADIC(format);
}

void Log::af(const char* format, ...) {
    if (Level::ASSERT < s_level) { return; }
    PRINT_VARIADIC(format);
}

void Log::setLevel(Level level) {
    s_level = level;
}

auto Log::getLevel() -> Level {
    return s_level;
}

std::string Log::format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    auto size = std::vsnprintf(nullptr, 0, format, args);
    std::string out;
    if (size > 0) {
        auto usize = static_cast<size_t>(size);
        out.reserve(usize);
        std::vsnprintf(&out[0], usize, format, args);
    }
    va_end(args);
    return out;
};

} //namespace stock
