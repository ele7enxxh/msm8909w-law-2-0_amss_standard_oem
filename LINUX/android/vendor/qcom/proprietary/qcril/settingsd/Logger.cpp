/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <iostream>
#include "Logger.h"
#include "commands/Command.h"
extern "C" {
#include <stdarg.h>
}

using namespace QcSettingsD;

/*
 * LoggerFactory
 */
std::map< std::string, std::shared_ptr<LoggerFactory> > &LoggerFactory::loggerFactories()
{
  static std::map< std::string, std::shared_ptr<LoggerFactory> >lf;

  return lf;
}
LoggerFactory::LoggerFactory(const std::string &name): name(name)
{
    DEBUG_LOGGER("> (\"%s\")",name.c_str());
    LoggerFactory::loggerFactories()[name] = std::shared_ptr<LoggerFactory>(this);
    DEBUG_LOGGER("<");
}

LoggerFactory::LoggerFactory(const LoggerFactory &o)
{
    DEBUG_LOGGER("> (\"%s\")",o.name.c_str());
    DEBUG_LOGGER("<");
}

LoggerFactory::~LoggerFactory()
{
    DEBUG_LOGGER("> (\"%s\")", name.c_str());
    DEBUG_LOGGER("< (\"%s\")", name.c_str());
}

std::shared_ptr<LoggerFactory> LoggerFactory::getLoggerFactory(const std::string &name)
{
    DEBUG_LOGGER("> (\"%s\")", name.c_str());
    std::shared_ptr<LoggerFactory> ret = loggerFactories()[name];
    DEBUG_LOGGER("< (empty? %s)", (bool)ret ? "false" : "true");
    return ret;
}

LoggerImpl *LoggerFactory::getLogger(const std::string &name)
{
    std::shared_ptr<LoggerFactory> fact = LoggerFactory::getLoggerFactory(name);
    LoggerImpl *impl = NULL;
    if (fact)
    {
        impl = fact->getLoggerImpl();
    }
    return impl;
}

/*
 * StdoutLogger
 */
thread_local char StdoutLogger::buf[1024];
StdoutLogger::StdoutLogger(): LoggerImpl(std::string("stdout"))
{
}

const char *LogLevel_names[] =
{
    [LVL_VERBOSE]   = "VERBOSE ",
    [LVL_DEBUG]     = "DEBUG   ",
    [LVL_INFO]      = "INFO    ",
    [LVL_WARN]      = "WARN    ",
    [LVL_HIGH]      = "HIGH    ",
    [LVL_FATAL]     = "FATAL   ",
    [LVL_MAX]       = "MAX     ",
};
void StdoutLogger::vlog(LogLevel lvl, const char *tag, const char *fmt, va_list ap)
{
    vsnprintf(buf, sizeof(buf), fmt, ap);
    printf("[%s:%s]: %s\n", LogLevel_names[lvl], tag, buf);
}

/*
 * StdoutLoggerFactory
 */
LoggerImpl *StdoutLoggerFactory::getLoggerImpl()
{
    static StdoutLogger *impl = new StdoutLogger();
    return impl;
}

StdoutLoggerFactory *factory = new StdoutLoggerFactory();

/*
 * LoggerImpl
 */
class LoggerImpl::Param
{
    public:
    Param(const std::string &name,
          const std::string &description):
                  name(name), description(description)
    {}
    unsigned id;
    std::string name;
    std::string description;
};
LoggerImpl::LoggerImpl(const std::string &name): name(name)
{
    registerParam("level",
                  "Set the maximum level to log at.");
}

int LoggerImpl::registerParam(const std::string &name,
                              const std::string &description)
{
    /* TODO: Implement */
    (void)name;
    (void)description;
    return 0;
}

/*
 * Logger
 */
std::shared_ptr<LoggerImpl> Logger::defaultLogger = std::shared_ptr<LoggerImpl>(LoggerFactory::getLoggerFactory("stdout")->getLoggerImpl());
bool Logger::debugLogger = false;

void Logger::log(LogLevel lvl, const char *tag, const char *fmt, ...)
{
    va_list ap;

    if (defaultLogger)
    {
        va_start(ap, fmt);
        defaultLogger->vlog(lvl, tag, fmt, ap);
        va_end(ap);
    }
}

void Logger::setParams(UNUSED(const std::string &name), ...)
{
}

void Logger::setDefault(const std::string &name)
{
    std::shared_ptr<LoggerFactory> f = LoggerFactory::getLoggerFactory(name);
    LoggerImpl *impl = NULL;
    DEBUG_LOGGER("> (\"%s\")", name.c_str());
    if (f)
    {
        impl = f->getLoggerImpl();
        if (impl)
        {
            log(LVL_INFO, TAG, "Setting logger to %s", name.c_str());
            defaultLogger = std::shared_ptr<LoggerImpl>(impl);
        }
        else
        {
            log(LVL_HIGH, TAG, "Unable to instantiate logger %s", name.c_str());
        }
    }
    else
    {
        log(LVL_HIGH, TAG, "Unable to find LoggerFactory for logger %s", name.c_str());
    }
    DEBUG_LOGGER("<");
}

static Logger theLogger;
void Logger::init()
{
    std::shared_ptr<Command> commands[] =
    {
        ActionCmd::make_cmd("debug_logger", "Enable debugging of the logger",
            "debug_logger <0|1>",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                bool ret = false;
                (void)id;
                (void)command;
                (void)results;

                if (pars.size() >= 1)
                {
                    const bool debug = pars[0].getBool();
                    LOG_DEBUG("Calling setDebugLogger with %s", debug?"true":"false");

                    setDebugLogger(debug);
                    ret = true;
                }
                return ret;
            }
        ),
    };

    for(std::shared_ptr<Command> cmd: commands)
    {
        CommandRegistry::registerCommand(cmd->getName(), cmd);
    }
}
