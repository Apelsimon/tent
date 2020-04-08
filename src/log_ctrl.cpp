#include "log_ctrl.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace tent
{

log_ctrl::log_ctrl()
{
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/logs.txt");
    spdlog::set_default_logger(file_logger);
}

} // namespace tent