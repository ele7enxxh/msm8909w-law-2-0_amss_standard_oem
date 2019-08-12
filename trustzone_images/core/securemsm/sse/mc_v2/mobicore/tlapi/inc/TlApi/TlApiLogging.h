/** @addtogroup TLAPI_LOGGING
 * @{
 * The MobiCore logging API interface provides functions to log events.
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
#ifndef __TLAPILOGGING_H__
#define __TLAPILOGGING_H__

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"

/** Formatted logging functions.
 *
 * tlApiLogvPrintf, tlApiLogPrintf
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
 */
_TLAPI_EXTERN_C void tlApiLogvPrintf(
        const char *fmt,
        va_list args);

_TLAPI_EXTERN_C void tlApiLogPrintf(
        const char *fmt,
        ...);


#if defined(__INCLUDE_DEBUG)

    #define tlDbgPrintf(...)   tlApiLogPrintf(__VA_ARGS__)
    #define tlDbgvPrintf(...)  tlApiLogvPrintf(__VA_ARGS__)

#else

   #define tlDbgPrintf(...)    DUMMY_FUNCTION()
   #define tlDbgvPrintf(...)   DUMMY_FUNCTION()

#endif // __INCLUDE_DEBUG

#endif // __TLAPILOGGING_H__



/** @} */
