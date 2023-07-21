#include "logger_interface.hpp"

using namespace Play;
Logger *Log::_logger = new ConsoleLogger();
LogLevel Log::_log_level = LogLevel::trace;
