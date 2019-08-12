/** @addtogroup DRAPI_LOGGING
 * @{
 * 
 * @file DrApiLogging.h
 * @brief  Secure Driver Logging API
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
 
#ifndef __DRAPILOGGING_H__
#define __DRAPILOGGING_H__

#include "DrApi/DrApiCommon.h"
#include "DrApi/DrApiError.h"

/** Formatted logging functions.
 *
 * drApiLogvPrintf, drApiLogPrintf
 *
 * Minimal printf-like function to print logging message to NWd log.
 *
 * Supported formatters:
 *   %s String, NULL value emit "<NULL>".
 *   %x %X hex
 *   %p pointer (hex with fixed width of 8)
 *   %d %i signed decimal
 *   %u unsigned decimal
 *   %t timestamp (if available in platform). NOTE: This does not consume any value in parameter list.
 *   %% outputs single %
 *
 *   %s, %x, %d, and %u support width (example %5s). Width is interpreted as minimum number of characters.
 *   Hex number is left padded using '0' to desired width.
 *   Decimal number is left padded using ' ' to desired width.
 *   String is right padded to desired length.
 *
 *   Newline is used to terminate logging line.
 *
 * @param fmt     Formatter
 * @param args    Argument list.
 */
_DRAPI_EXTERN_C void drApiLogvPrintf(
        const char *fmt,
        va_list args);

_DRAPI_EXTERN_C void drApiLogPrintf(
        const char *fmt,
        ...);
#if defined(DEBUG)
    #define __INCLUDE_DEBUG
#endif

#if defined(__INCLUDE_DEBUG)

    #define drDbgPrintf(...)   drApiLogPrintf(__VA_ARGS__)
    #define drDbgvPrintf(...)  drApiLogvPrintf(__VA_ARGS__)

// These are simple light weight debug functions. printf() is much more
// flexible, but also more expensive in thems of code and stack usage.
_EXTERN_C void __dbgC(const char ch);
_EXTERN_C void __dbgS(const char *s);
_EXTERN_C void __dbgD(int32_t d);
_EXTERN_C void __dbgB(uint32_t d);
_EXTERN_C void __dbgH(uint32_t h);

//------------------------------------------------------------------------
// debug a da data blob
_EXTERN_C void __dbgBlob(
                    const char    *szDescriptor,
                    const void    *blob,
                    uint32_t      sizeOfBlob);

//------------------------------------------------------------------------
// assert message function.
_EXTERN_C void __dbgAssertMsg(
    const char      *exp,
    const char      *file,
    const uint32_t  line);



#define dbgC(c)     __dbgC(c);
#define dbgS(s)     __dbgS(s);
#define dbgD(d)     __dbgD(d);
#define dbgH(h)     __dbgH(h);
#define dbgB(h)     __dbgB(h);

#define dbgBlob(...)        __dbgBlob(__VA_ARGS__)
#define dbgAssertMsg(...)   __dbgAssertMsg(__VA_ARGS__)
#else

   #define drDbgPrintf(...)    DUMMY_FUNCTION()
   #define drDbgvPrintf(...)   DUMMY_FUNCTION()
#define dbgC(...)           DUMMY_FUNCTION()
#define dbgS(...)           DUMMY_FUNCTION()
#define dbgD(...)           DUMMY_FUNCTION()
#define dbgB(...)           DUMMY_FUNCTION()
#define dbgH(...)           DUMMY_FUNCTION()

#define dbgBlob(...)        DUMMY_FUNCTION()
#define dbgAssertMsg(...)   DUMMY_FUNCTION()

#endif // __INCLUDE_DEBUG
// C=character, S=String, N=newLine, F=file, L=line,
// D=decimal value, H=hex value, P=pointer
#define dbgN()                  drDbgPrintf("\n")
#define dbgP(p)                 drDbgPrintf("%p", p)
#define dbgDC(d,c)              do{drDbgPrintf("%d%s", d, c);}while(FALSE)
#define dbgHC(h,c)              do{drDbgPrintf("%x%s", h, c);}while(FALSE)
#define dbgSN(s)                do{drDbgPrintf("%s\n", s);}while(FALSE)
#define dbgSD(s,d)              do{drDbgPrintf("%s%d", s, d);}while(FALSE)
#define dbgSDN(s,d)             do{drDbgPrintf("%s%d\n", s, d);}while(FALSE)
#define dbgSH(s,h)              do{drDbgPrintf("%s%x", s, h);}while(FALSE)
#define dbgSHN(s,h)             do{drDbgPrintf("%s%x\n", s, h);}while(FALSE)
#define dbgSHS(s1,h1,s2)        do{drDbgPrintf("%s%x%s", s1, h1, s2);}while(FALSE)
#define dbgSHSHN(s1,h1,s2,h2)   do{drDbgPrintf("%s%x%s%x", s1, h1, s2, h2);}while(FALSE)
#define dbgSP(s,p)              do{drDbgPrintf("%s%p", s, p);}while(FALSE)
#define dbgSPN(s,p)             do{drDbgPrintf("%s%p\n", s, p);}while(FALSE)
#define dbgSPSN(s1,p,s2)        do{drDbgPrintf("%s%p%s\n", s1, p, s2);}while(FALSE)
#define dbgFL(f,l)              do{drDbgPrintf("%s:%d", f,l);}while(FALSE)
#define dbgFLN(f,l)             do{drDbgPrintf("%s:%d\n", f,l);}while(FALSE)

#define drDbgPrintLnf(...)      do{drDbgPrintf(__VA_ARGS__);dbgS(EOL);}while(FALSE)
#endif // __DRAPILOGGING_H__



/** @} */
