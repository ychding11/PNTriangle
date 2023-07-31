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
