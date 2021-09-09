#include <Log.h>

namespace Logging
{
    static spdlog::logger *sLogger = nullptr;

    static void Init(void)
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);
        //console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
        file_sink->set_level(spdlog::level::trace);

        sLogger = new spdlog::logger("multi_sink", { console_sink, file_sink });
        sLogger->set_level(spdlog::level::debug);
    }

    spdlog::logger* Logger()
    {
        if (sLogger == nullptr)
        {
            Init();
        }
        return sLogger;
    }

} //namespace