#include "log_ctrl.hpp"

namespace tent
{

log_ctrl::log_ctrl()
{
    auto console = spdlog::stdout_color_mt("console");    

    auto file_logger = spdlog::basic_logger_mt("debug_logger", "logs/debug_logs.txt");
    spdlog::set_default_logger(file_logger);
    spdlog::flush_every(std::chrono::seconds(10));
    // spdlog::set_level(spdlog::level::debug);
    
    spdlog::get("debug_logger")->set_level(spdlog::level::debug);
}

} // namespace tent