/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef ANDROID_LOGGER_H_INCLUDED
#define ANDROID_LOGGER_H_INCLUDED

#include "Logger.h"

namespace QcSettingsD
{

class AndroidLogger;
class AndroidLoggerFactory: virtual public LoggerFactory
{
    public:
    AndroidLoggerFactory();
    virtual LoggerImpl *getLoggerImpl();
    virtual ~AndroidLoggerFactory();

    protected:

    private:
    AndroidLoggerFactory(const AndroidLoggerFactory &other) = delete;
    AndroidLoggerFactory(AndroidLoggerFactory &&other) = delete;
    AndroidLoggerFactory &operator= (const AndroidLoggerFactory &) = delete;
    AndroidLoggerFactory &operator= (AndroidLoggerFactory &&) = delete;
    static AndroidLogger *theLogger();
    static AndroidLoggerFactory *factory;
};

class AndroidLogger: virtual public LoggerImpl
{
    friend class AndroidLoggerFactory;
    public:
    virtual void vlog(LogLevel lvl, const char *tag, const char *fmt, va_list ap);

    private:
    AndroidLogger();
};

}
#endif
