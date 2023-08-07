#ifndef Log_MY_H_
#define Log_MY_H_

//
// https://github.com/gabime/spdlog
//
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h" // must be included for user type format

//< https://hackingcpp.com/cpp/libs/fmt.html

namespace Logging
{
    spdlog::logger* Logger();
}

#define Log(...)  Logging::Logger()->info(__VA_ARGS__); 
#define Warn(...) Logging::Logger()->warn(__VA_ARGS__); 
#define Err(...)  Logging::Logger()->error(__VA_ARGS__);  assert(0);

#endif


#include <glm/glm.hpp>

template <> struct fmt::formatter<glm::vec3>
{
    // Presentation format: 'f' - fixed, 'e' - exponential.
    char presentation = 'f';

    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator
    {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be parsed,
        // e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Please also note that this character range may be empty, in case of
        // the "{}" format string, so therefore you should check ctx.begin()
        // for equality with ctx.end().

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}') ctx.on_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    auto format(const glm::vec3& p, format_context& ctx) const -> format_context::iterator
    {
        // ctx.out() is an output iterator to write to.
        return presentation == 'f'
            ? fmt::format_to(ctx.out(), "({:8.2f}, {:8.2f}, {:8.2f})", p.x, p.y, p.z)
            : fmt::format_to(ctx.out(), "({:8.2e}, {:8.2e}, {:8.2f})", p.x, p.y, p.z);
    }
};
