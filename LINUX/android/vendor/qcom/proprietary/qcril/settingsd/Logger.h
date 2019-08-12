/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>
#include <memory>
#include <map>

extern "C" {
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
}

#define UNUSED(name) name __attribute__((unused))
namespace QcSettingsD {

static inline long my_gettid()
{
    return syscall(SYS_gettid);
}
#define LOG_VERBOSE(fmt, ...) Logger::log(LVL_VERBOSE, TAG, "[%lx]" fmt, my_gettid(), ##__VA_ARGS__)
#define   LOG_DEBUG(fmt, ...) Logger::log(LVL_DEBUG  , TAG, "[%lx]" fmt, my_gettid(), ##__VA_ARGS__)
#define    LOG_INFO(fmt, ...) Logger::log(LVL_INFO   , TAG, "[%lx]" fmt, my_gettid(), ##__VA_ARGS__)
#define    LOG_WARN(fmt, ...) Logger::log(LVL_WARN   , TAG, "[%lx]" fmt, my_gettid(), ##__VA_ARGS__)
#define    LOG_HIGH(fmt, ...) Logger::log(LVL_HIGH   , TAG, "[%lx]" fmt, my_gettid(), ##__VA_ARGS__)
#define   LOG_FATAL(fmt, ...) Logger::log(LVL_FATAL  , TAG, "[%lx]" fmt, my_gettid(), ##__VA_ARGS__)

#define   LOG_ENTER(fmt, ...) LOG_VERBOSE("> %s: " fmt, __func__, ##__VA_ARGS__)
#define   LOG_LEAVE(fmt, ...) LOG_VERBOSE("< %s: " fmt, __func__, ##__VA_ARGS__)

#define DEBUG_LOGGER(fmt, ...) ({\
    if (Logger::debugLogger) { \
    fprintf(stderr, "        LOGGER_DEBUG %s: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
    } \
})

enum LogLevel {
    LVL_VERBOSE,
    LVL_DEBUG,
    LVL_INFO,
    LVL_WARN,
    LVL_HIGH,
    LVL_FATAL,
    LVL_MAX /* Nothing will be logged if this is selected as level */
};

class LoggerImpl
{
    public:
    class Param;

    virtual void vlog(LogLevel lvl, const char *tag, const char *fmt, va_list ap) = 0;
    LoggerImpl(const std::string &name);
    virtual ~LoggerImpl() {}
    void setDebugLogger(bool debug) { debugLogger = debug; }
    bool getDebugLogger() { return debugLogger; }

    protected:
    int registerParam(const std::string &name,
                      const std::string &description);
    bool        debugLogger;

    private:
    std::string name;
};

class LoggerFactory
{
    public:
    virtual LoggerImpl *getLoggerImpl() = 0;
    static std::shared_ptr<LoggerFactory> getLoggerFactory(const std::string &name);
    static LoggerImpl *getLogger(const std::string &name);
    virtual ~LoggerFactory();

    protected:
    LoggerFactory(const std::string &name);
    static std::map< std::string, std::shared_ptr<LoggerFactory> > &loggerFactories();

    private:
    LoggerFactory(const LoggerFactory &);
    LoggerFactory(const LoggerFactory &&) = delete;
    LoggerFactory(LoggerFactory &) = delete;
    LoggerFactory(LoggerFactory &&) = delete;
    LoggerFactory &operator= (const LoggerFactory &) = delete;
    LoggerFactory &operator= (LoggerFactory &&) = delete;
    std::string name;
};

class StdoutLogger;
class StdoutLoggerFactory: virtual public LoggerFactory
{
    public:
    StdoutLoggerFactory(): LoggerFactory("stdout") {};
    virtual LoggerImpl *getLoggerImpl();
    virtual ~StdoutLoggerFactory() {}

    protected:

    private:
    static StdoutLogger theLogger;
};

class StdoutLogger: virtual public LoggerImpl
{
    friend class StdoutLoggerFactory;
    public:
    virtual void vlog(LogLevel lvl, const char *tag, const char *fmt, va_list ap);

    private:
    StdoutLogger();
    thread_local static char buf[1024];
};

class Logger
{
    friend class LoggerFactory;
    public:
    static void log(LogLevel lvl, const char *tag, const char *fmt, ...);
    static void setParams(const std::string &name, ...);
    static void setDefault(const std::string &name);
    static void setDebugLogger(bool debug) { debugLogger = debug; }
    static bool getDebugLogger() { return debugLogger; }
    static bool debugLogger;
    Logger() { init(); }

    private:
    static std::shared_ptr<LoggerImpl> defaultLogger;
    void init();
};
}

#endif
