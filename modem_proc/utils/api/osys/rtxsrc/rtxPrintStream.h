/*
 * Copyright (c) 1997-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file rtxPrintStream.h
 * Functions that allow printing diagnostic message to a stream using a
 * callback function.
 */
#ifndef _RTXPRINTSTREAM_H_
#define _RTXPRINTSTREAM_H_

#ifndef _NO_STREAM

#include <stdarg.h>
#include "rtxsrc/rtxContext.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ccfDiag
 * @{
 */
/**
 * Callback function definition for print stream.
 */
typedef void (*rtxPrintCallback)
(void* pPrntStrmInfo, const char* fmtspec, va_list arglist);

/**
 * Structure to hold information about a global PrintStream.
 */
typedef struct OSRTPrintStream {
   rtxPrintCallback pfPrintFunc;
   void * pPrntStrmInfo;
} OSRTPrintStream;

#ifndef __SYMBIAN32__
/**
 * Global PrintStream
 */
extern OSRTPrintStream g_PrintStream;
#endif

/**
 * This function is for setting the callback function for a PrintStream. Once a
 * callback function is set, then all print and debug output ia sent to
 * the defined callback function.
 *
 * @param pctxt      Pointer to a context in which callback print function
 *                    will be set
 * @param myCallback Pointer to a callback print function.
 * @param pStrmInfo  Pointer to user defined PrintInfo structure where users
 *                    can store information required by the callback function
 *                    across calls. Ex. An open File handle for callbak
 *                    function which directs stream to a file.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxSetPrintStream
(OSCTXT *pctxt, rtxPrintCallback myCallback, void* pStrmInfo);

#ifndef __SYMBIAN32__
/**
 * This function is for setting the callback function for a PrintStream.
 * This version of the function sets a callback at the global level.
 *
 * @param myCallback Pointer to a callback print function.
 * @param pStrmInfo  Pointer to user defined PrintInfo structure where users
 *                    can store information required by the callback function
 *                    across calls. Ex. An open File handle for callbak
 *                    function which directs stream to a file.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxSetGlobalPrintStream
(rtxPrintCallback myCallback, void* pStrmInfo);
#endif

/**
 * Print-to-stream function which in turn calls the user registered callback
 * function of the context for printing. If no callback function is registered
 * it prints to standard output by default.
 *
 * @param pctxt      Pointer to context to be used.
 * @param fmtspec    A printf-like format specification string describing the
 *                    message to be printed (for example, "string %s, ivalue
 *                    %d\n").
 * @param             ... A variable list of arguments.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxPrintToStream (OSCTXT* pctxt, const char* fmtspec, ...);

/**
 * Diagnostics print-to-stream function.  This is the same as the
 * \c rtxPrintToStream function except that it checks if diagnostic tracing
 * is enabled before invoking the callback function.
 *
 * @param pctxt      Pointer to context to be used.
 * @param fmtspec    A printf-like format specification string describing the
 *                    message to be printed (for example, "string %s, ivalue
 *                    %d\n").
 * @param arglist    A variable list of arguments passed as va_list
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxDiagToStream
(OSCTXT *pctxt, const char* fmtspec, va_list arglist);

/**
 * This function releases the memory held by PrintStream in the context
 *
 * @param pctxt      Pointer to a context for which the memory has to be
 *                    released.
 *
 * @return           Completion status, 0 on success, negative value on failure
 */
EXTERNRT int rtxPrintStreamRelease (OSCTXT* pctxt);

/**
 * Standard callback function for use with print-to-stream for
 * writing to stdout.
 *
 * @param pPrntStrmInfo  User-defined information for use by the callback
 *                       function.  This is supplied by the user at the time
 *                       the callback is registered. In this case, no
 *                       user-defined information is required, so the
 *                       argument can be set to NULL when the callback is
 *                       registered.
 *
 * @param fmtspec        Format specification of the data to be printed.
 *                       This is supplied by the print-to-stream utility.
 *
 * @param arglist        Variable argument list.
 *                       This is supplied by the print-to-stream utility.
 */
EXTERNRT void rtxPrintStreamToStdoutCB
(void* pPrntStrmInfo, const char* fmtspec, va_list arglist);

/**
 * Standard callback function for use with print-to-stream for
 * writing to a file.
 *
 * @param pPrntStrmInfo  User-defined information for use by the callback
 *                       function.  This is supplied by the user at the time
 *                       the callback is registered. This parameter should
 *                       be set to the file pointer (FILE*) to which data
 *                       is to be written.
 *
 * @param fmtspec        Format specification of the data to be printed.
 *                       This is supplied by the print-to-stream utility.
 *
 * @param arglist        Variable argument list.
 *                       This is supplied by the print-to-stream utility.
 */
EXTERNRT void rtxPrintStreamToFileCB
(void* pPrntStrmInfo, const char* fmtspec, va_list arglist);

#ifdef __cplusplus
}
#endif
/**
 * @} ccfDiag
 */

#endif /* _NO_STREAM */
#endif /* _RTXPRINTSTREAM_H_ */

