#pragma once
#include <exception>
#include <iostream>
#include <string>

namespace Play
{
class Logger
{
public:
    virtual void debug(const std::string &message,
                       const std::string &class_name) = 0;
    virtual void info(const std::string &message,
                      const std::string &class_name) = 0;
    virtual void warn(const std::string &message,
                      const std::string &class_name) = 0;
    virtual void error(const std::string &message,
                       const std::string &class_name,
                       std::exception *ex = nullptr) = 0;
    virtual void trace(const std::string &message,
                       const std::string &class_name) = 0;
    virtual void fatal(const std::string &message,
                       const std::string &class_name) = 0;
};

enum LogLevel
{
    trace = 0,
    debug = 1,
    info = 2,
    warning = 3,
    error = 4,
    fatal = 5
};

class ConsoleLogger : public Logger
{
private:
    std::string get_time_stamp()
    {
        time_t now = time(nullptr);
        struct tm time_info;
        localtime_s(&time_info, &now);
        char buffer[25];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S.000", &time_info);
        return buffer;
    }

public:
    void trace(const std::string &message,
               const std::string &class_name) override
    {
        std::cout << get_time_stamp() << " TRACE: (" << class_name << ") - "
                  << message << std::endl;
    }

    void debug(const std::string &message,
               const std::string &class_name) override
    {
        std::cout << get_time_stamp() << " DEBUG: (" << class_name << ") - "
                  << message << std::endl;
    }

    void info(const std::string &message,
              const std::string &class_name) override
    {
        std::cout << get_time_stamp() << " INFO: (" << class_name << ") - "
                  << message << std::endl;
    }

    void warn(const std::string &message,
              const std::string &class_name) override
    {
        std::cout << get_time_stamp() << " WARN: (" << class_name << ") - "
                  << message << std::endl;
    }

    void error(const std::string &message,
               const std::string &class_name,
               std::exception *ex = nullptr) override
    {
        if (ex != nullptr)
        {
            std::cout << get_time_stamp() << " ERROR: (" << class_name << ") - "
                      << message << " [" << ex << "]" << std::endl;
        }
        else
        {
            std::cout << get_time_stamp() << " ERROR: (" << class_name << ") - "
                      << message << std::endl;
        }
    }

    void fatal(const std::string &message,
               const std::string &class_name) override
    {
        std::cout << get_time_stamp() << " FATAL: (" << class_name << ") - "
                  << message << std::endl;
    }
};

class Log
{
private:
    static Logger *_logger;
    static LogLevel _log_level;

public:
    static void setLogger(Logger *new_logger,
                          LogLevel new_log_level = LogLevel::trace)
    {
        _logger = new_logger;
        _log_level = new_log_level;
    }

    static void trace(const std::string &message, const std::string &class_name)
    {
        if (LogLevel::trace >= _log_level)
        {
            _logger->trace(message, class_name);
        }
    }

    static void debug(const std::string &message, const std::string &class_name)
    {
        if (LogLevel::debug >= _log_level)
        {
            _logger->debug(message, class_name);
        }
    }

    static void info(const std::string &message, const std::string &class_name)
    {
        if (LogLevel::info >= _log_level)
        {
            _logger->info(message, class_name);
        }
    }

    static void warn(const std::string &message, const std::string &class_name)
    {
        if (LogLevel::warning >= _log_level)
        {
            _logger->warn(message, class_name);
        }
    }

    static void error(const std::string &message, const std::string &class_name)
    {
        if (LogLevel::error >= _log_level)
        {
            _logger->error(message, class_name);
        }
    }

    static void error(const std::string &message,
                      const std::string &class_name,
                      std::exception *ex)
    {
        if (LogLevel::error >= _log_level)
        {
            _logger->error(message, class_name, ex);
        }
    }

    static void fatal(const std::string &message, const std::string &class_name)
    {
        if (LogLevel::fatal >= _log_level)
        {
            _logger->fatal(message, class_name);
        }
    }
};


} // namespace Play
