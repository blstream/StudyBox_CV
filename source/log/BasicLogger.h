#ifndef PATR_BASIC_LOGGER_H
#define PATR_BASIC_LOGGER_H

#include "Config.h"
#include "Layout.h"
#include <utility>
#include <sstream>

template<typename Target>
class LoggerOstream
{
public:
    LoggerOstream(LogConfig::Severity::LevelType severity, LogConfig::LogLevel level, Target& target, LoggerInfo& info, bool flushFlag) : target(target), severity(severity), level(level), info(info), flushFlag(flushFlag)
    {
    }

    LoggerOstream(LoggerOstream&&) = default;

    template<typename T>
    LoggerOstream& operator <<(T&& t)
    {
        if (severity & level)
        {
            stream << std::forward<T>(t);
        }

        return *this;
    }

    ~LoggerOstream()
    {
        if (severity & level)
        {
            target.notify(level, std::move(stream.str()), info, flushFlag);
        }
    }

private:
    std::ostringstream stream;
    Target& target;
    LogConfig::Severity::LevelType severity;
    LogConfig::LogLevel level;
    LoggerInfo& info;
    bool flushFlag;
};


template<typename Target, typename Formatter>
class BasicLogger : private Formatter // EBCO
{
public:
    enum class Level : unsigned int
    {
        Trace = 1,
        Debug = 2,
        Info = 4,
        Warn = 8,
        Error = 16,
        Fatal = 32
    };

    template<typename LoggerInfoU, typename... U>
    BasicLogger(Target& target, LogConfig::Severity::LevelType level, LoggerInfoU&& info, U&&... formatterArgs) : Formatter(std::forward<U>(formatterArgs)...), target(target), severity(level), loggerInfo(std::forward<LoggerInfoU>(info)), flushFlag(false)
    {
    }

    template<typename... Ts>
    void trace(Ts&&... ts)
    {
        log(LogConfig::LogLevel::Trace, std::forward<Ts>(ts)...);
    }
    template<typename... Ts>
    void debug(Ts&&... ts)
    {
        log(LogConfig::LogLevel::Debug, std::forward<Ts>(ts)...);
    }
    template<typename... Ts>
    void info(Ts&&... ts)
    {
        log(LogConfig::LogLevel::Info, std::forward<Ts>(ts)...);
    }
    template<typename... Ts>
    void warn(Ts&&... ts)
    {
        log(LogConfig::LogLevel::Warn, std::forward<Ts>(ts)...);
    }
    template<typename... Ts>
    void error(Ts&&... ts)
    {
        log(LogConfig::LogLevel::Error, std::forward<Ts>(ts)...);
    }
    template<typename... Ts>
    void fatal(Ts&&... ts)
    {
        log(LogConfig::LogLevel::Fatal, std::forward<Ts>(ts)...);
    }
    template<typename... Ts>
    void log(LogConfig::LogLevel level, Ts&&... ts)
    {
        if (severity & level)
        {
            target.notify(level, std::move(Formatter::format(std::forward<Ts>(ts)...)), loggerInfo, flushFlag);
            flushFlag = false;
        }
    }

    LoggerOstream<Target> log(LogConfig::LogLevel level)
    {
        return LoggerOstream<Target>(severity, level, target, loggerInfo, flushFlag);
    }

    LoggerOstream<Target> operator ()(LogConfig::LogLevel level)
    {
        return log(level);
    }

    BasicLogger& flush_next()
    {
        flushFlag = true;
        return *this;
    }

private:
    Target& target;
    LogConfig::Severity::LevelType severity;
    LoggerInfo loggerInfo;
    bool flushFlag;
};

template<typename Target>
class BasicLogManager
{
public:
    template<typename Sink, typename Layout, typename Timer>
    BasicLogManager(Sink&& sink, LogConfig::Severity::LevelType globalLevel, Layout&& layout, Timer&& timer) : target(std::forward<Sink>(sink), std::forward<Layout>(layout), std::forward<Timer>(timer)), globalLevel(globalLevel), loggerCount(0)
    {
    }

    template<typename Formatter, typename String>
    BasicLogger<Target, Formatter> get(String&& name, LogConfig::Severity level, Formatter&& formatter)
    {
        LoggerInfo info = { loggerCount++, std::forward<String>(name) };
        return BasicLogger<Target, Formatter>(target, level, std::move(info), std::forward<Formatter>(formatter));
    }

    template<typename Formatter, typename String>
    BasicLogger<Target, Formatter> get(String&& name, Formatter&& formatter)
    {
        LoggerInfo info = { loggerCount++, std::forward<String>(name) };
        return BasicLogger<Target, Formatter>(target, globalLevel, std::move(info), std::forward<Formatter>(formatter));
    }

    template<typename Formatter, typename String>
    BasicLogger<Target, Formatter> get(String&& name, LogConfig::Severity::LevelType level)
    {
        LoggerInfo info = { loggerCount++, std::forward<String>(name) };
        return BasicLogger<Target, Formatter>(target, level, std::move(info));
    }

    template<typename String>
    BasicLogger<Target, StringStreamFormatter> get(String&& name, LogConfig::Severity::LevelType level)
    {
        return get<StringStreamFormatter>(std::forward<String>(name), level);
    }

    template<typename Formatter, typename String>
    BasicLogger<Target, Formatter> get(String&& name)
    {
        LoggerInfo info = { loggerCount++, std::forward<String>(name) };
        return BasicLogger<Target, Formatter>(target, globalLevel, std::move(info));
    }

    template<typename String>
    BasicLogger<Target, StringStreamFormatter> get(String&& name)
    {
        return get<StringStreamFormatter>(std::forward<String>(name));
    }

    const LogConfig::Severity& level() const
    {
        return globalLevel;
    }

private:
    Target target;
    LogConfig::Severity::LevelType globalLevel;
    std::size_t loggerCount;
};

#endif // PATR_BASIC_LOGGER_H
