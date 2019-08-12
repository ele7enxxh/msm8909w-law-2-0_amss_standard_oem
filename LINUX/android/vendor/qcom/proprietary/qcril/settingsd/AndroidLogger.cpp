/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <iostream>
#include "Logger.h"
#include "AndroidLogger.h"
#include "log/log.h"

namespace QcSettingsD {

AndroidLoggerFactory *AndroidLoggerFactory::factory = new AndroidLoggerFactory();


AndroidLoggerFactory::~AndroidLoggerFactory()
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}
AndroidLoggerFactory::AndroidLoggerFactory(): LoggerFactory("android")
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}

LoggerImpl *AndroidLoggerFactory::getLoggerImpl()
{
    LoggerImpl *ret = NULL;

    DEBUG_LOGGER(">");
    ret = theLogger();
    DEBUG_LOGGER("< (%p)", ret);

    return ret;
}

AndroidLogger *AndroidLoggerFactory::theLogger()
{
    DEBUG_LOGGER(">");
    static AndroidLogger *logger = new AndroidLogger();

    DEBUG_LOGGER("< (%p)", logger);
    return logger;
}

AndroidLogger::AndroidLogger(): LoggerImpl(std::string("stdout"))
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}

static android_LogPriority getAndroidPrio(LogLevel lvl)
{
    DEBUG_LOGGER("> lvl: %d", (int) lvl);
    switch(lvl)
    {
        case LVL_VERBOSE: return ANDROID_LOG_VERBOSE;
        case LVL_DEBUG  : return ANDROID_LOG_DEBUG;
        case LVL_INFO   : return ANDROID_LOG_INFO;
        case LVL_WARN   : return ANDROID_LOG_WARN;
        case LVL_HIGH   : return ANDROID_LOG_ERROR;
        case LVL_FATAL  : return ANDROID_LOG_FATAL;
        case LVL_MAX    : return ANDROID_LOG_SILENT;
        default         : return ANDROID_LOG_DEFAULT;
    }
}
void AndroidLogger::vlog(LogLevel lvl, const char *tag, const char *fmt, va_list ap)
{
    DEBUG_LOGGER(">");
    LOG_PRI_VA(getAndroidPrio(lvl), tag, fmt, ap);
    DEBUG_LOGGER("<");
}

}
