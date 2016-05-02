//
// Created by Matt Blair on 4/29/16.
//
#pragma once

#include <string>

namespace stock {

class Log {

public:

    enum class Level {
        verbose,
        debug,
        warning,
        error,
        assert,
    };

    // Log a message.
    static void write(Level level, const std::string& message);

    // Log a 'verbose' message.
    static void v(const std::string& message);

    // Log a 'debug' message.
    static void d(const std::string& message);

    // Log a 'warning' message.
    static void w(const std::string& message);

    // Log an 'error' message.
    static void e(const std::string& message);

    // Log an 'assert' message.
    static void a(const std::string& message);

    // Log a formatted message.
    static void writef(Level level, const char* format, ...);

    // Log a formatted 'verbose' message.
    static void vf(const char* format, ...);

    // Log a formatted 'debug' message.
    static void df(const char* format, ...);

    // Log a formatted 'warning' message.
    static void wf(const char* format, ...);

    // Log a formatted 'error' message.
    static void ef(const char* format, ...);

    // Log a formatted 'assert' message.
    static void af(const char* format, ...);

    // Set the minimum displayed log priority.
    static void setLevel(Level level);

    // Get the minimum displayed log priority.
    static auto getLevel() -> Level;

    // Format a string using 'printf' syntax.
    static auto format(const char* format, ...) -> std::string;

private:

    static Level s_level;

};

} // namespace stock
