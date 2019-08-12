/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
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
 * @file rtxError.h
 * Error handling function and macro definitions.
 */
#ifndef __RTXERROR_H__
#define __RTXERROR_H__

#include "rtxsrc/rtxContext.h"
#include "rtxsrc/rtxErrCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Error handling */

/**
 * @defgroup ccfErr Error Formatting and Print Functions
 * @{
 *
 * Error formatting and print functions allow information about encode/decode
 * errors to be added to a context block structure and then printed when the
 * error is propagated to the top level.
 */
/**
 * This macro is used to log a run-time error in the context. It calls the \c
 * rtxErrSetData function to set the status and error parameters. The C
 * built-in __FILE__ and __LINE__ macros are used to record the position in the
 * source file of the error.
 *
 * @param pctxt        A pointer to a context structure.
 * @param stat         Error status value from \c rtxErrCodes.h
 */
#ifndef _COMPACT

#ifndef __MODULE__
#define LOG_RTERR(pctxt,stat) \
rtxErrSetData(pctxt,stat,__FILE__,__LINE__)

#define LOG_RTERRNEW(pctxt,stat) \
rtxErrSetNewData(pctxt,stat,__FILE__,__LINE__)

#else /* Should significantly decrease use of RAM in ARM systems */
#define LOG_RTERR(pctxt,stat) \
rtxErrSetData(pctxt,stat,__MODULE__,__LINE__)

#define LOG_RTERRNEW(pctxt,stat) \
rtxErrSetNewData(pctxt,stat,__MODULE__,__LINE__)
#endif /* not defined __MODULE__ */

/**
 * This macro is used to check an assertion. This is a condition that is
 * expected to be true. The \c rtxErrAssertionFailed function is called if the
 * condition is not true. The C built-in __FILE__ and __LINE__ macros are used
 * to record the position in the source file of the failure.
 *
 * @param condition    Condition to check (for example, "(ptr != NULL)")
 */
#define OSRTASSERT(condition) \
if (!(condition)) { rtxErrAssertionFailed(#condition,__LINE__,__FILE__); }

/**
 * This macro check a condition but takes no action.  Its main use is
 * to supress VC++ level 4 "argument not used" warnings.
 *
 * @param condition    Condition to check (for example, "(ptr != NULL)")
 */
#define OSRTCHECKPARAM(condition) if (condition) { /* do nothing */ }
#else
#define LOG_RTERR(pctxt,stat) \
rtxErrSetData(pctxt,stat,0,0)

#define LOG_RTERRNEW(pctxt,stat) \
rtxErrSetNewData(pctxt,stat,0,0)

#define OSRTASSERT(condition)
#define OSRTCHECKPARAM(condition)
#endif /* _COMPACT */

#define LOG_RTERR1(pctxt,stat,a) \
(a,LOG_RTERR (pctxt, stat),stat)

#define LOG_RTERRNEW1(pctxt,stat,a) \
(a,LOG_RTERRNEW (pctxt, stat),stat)

#define LOG_RTERR2(pctxt,stat,a,b) \
(a,b,LOG_RTERR (pctxt, stat),stat)

#define LOG_RTERRNEW2(pctxt,stat,a,b) \
(a,b,LOG_RTERRNEW (pctxt, stat),stat)


typedef int (*OSErrCbFunc) (const char* ptext, void* cbArg_p);

/**
 * This function adds the contents of the context buffer to the
 * error information structure in the context.  The buffer contents
 * are assumed to contain only printable characters.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddCtxtBufParm (OSCTXT* pctxt);

/**
 * This function adds a double parameter to an error information
 * structure.
 *
 * @param pctxt        A pointer to a context structure.
 * @param errParm      The double error parameter.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddDoubleParm (OSCTXT* pctxt, double errParm);

/**
 * This function adds a set of error codes to the global error table.
 * It is called within context initialization functions to add errors
 * defined for a specific domain (for example, ASN.1 encoding/decoding)
 * to be added to the global list of errors.
 *
 * @param ppStatusText Pointer to table of error status text messages.
 * @param minErrCode   Minimum error status code.
 * @param maxErrCode   Maximum error status code.
 * @return             The status of the operation (TRUE if entry
 *                       sucessfully added to table).
 */
EXTERNRT OSBOOL rtxErrAddErrorTableEntry
#if !defined(_ARMTCC) && !defined(__SYMBIAN32__)
(const char* const* ppStatusText, OSINT32 minErrCode, OSINT32 maxErrCode);
#else
(void);
#endif

/**
 * This function adds an element name parameter to the context error
 * information structure.  The element name is obtained from the context
 * element name stack.  If the stack is empty, a question mark characetr
 * (?) is inserted for the name.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddElemNameParm (OSCTXT* pctxt);

/**
 * This function adds an integer parameter to an error information structure.
 * Parameter substitution is done in much the same way as it is done in C
 * printf statments. The base error message specification that goes along with
 * a particular status code may have variable fields built in using '%'
 * modifiers. These would be replaced with actual parameter data.
 *
 * @param pctxt        A pointer to a context structure.
 * @param errParm      The integer error parameter.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddIntParm (OSCTXT* pctxt, int errParm);

#if !defined(_NO_INT64_SUPPORT)
/**
 * This function adds a 64-bit integer parameter to an error information
 * structure.
 * Parameter substitution is done in much the same way as it is done in C
 * printf statments. The base error message specification that goes along with
 * a particular status code may have variable fields built in using '%'
 * modifiers. These would be replaced with actual parameter data.
 *
 * @param pctxt        A pointer to a context structure.
 * @param errParm      The 64-bit integer error parameter.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddInt64Parm (OSCTXT* pctxt, OSINT64 errParm);
#endif /* !defined(_NO_INT64_SUPPORT) */

/**
 * This function adds a character string parameter to an error information
 * structure.
 *
 * @param pctxt        A pointer to a context structure.
 * @param pErrParm     The character string error parameter.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddStrParm (OSCTXT* pctxt, const char* pErrParm);

/**
 * This function adds a given number of characters from a character string
 * parameter to an error information structure.
 *
 * @param pctxt        A pointer to a context structure.
 * @param pErrParm     The character string error parameter.
 * @param nchars       Number of characters to add from pErrParm.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddStrnParm
   (OSCTXT* pctxt, const char* pErrParm, size_t nchars);

/**
 * This function adds an unsigned integer parameter to an error information
 * structure.
 *
 * @param pctxt        A pointer to a context structure.
 * @param errParm      The unsigned integer error parameter.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddUIntParm (OSCTXT* pctxt, unsigned int errParm);

#if !defined(_NO_INT64_SUPPORT)
/**
 * This function adds an unsigned 64-bit integer parameter to an error
 * information structure.
 *
 * @param pctxt        A pointer to a context structure.
 * @param errParm      The unsigned 64-bit integer error parameter.
 * @return             The status of the operation (TRUE if the parameter was
 *                       sucessfully added).
 */
EXTERNRT OSBOOL rtxErrAddUInt64Parm (OSCTXT* pctxt, OSUINT64 errParm);
#endif /* !defined(_NO_INT64_SUPPORT) */

/**
 * This function is used to record an assertion failure. It is used in
 * conjunction with the \c OTRTASSERT macro. It outputs information on the
 * condition to \c stderr and then calls \c exit to terminate the program.
 *
 * @param conditionText    The condition that failed (for example, ptr != NULL)
 * @param lineNo           Line number in the program of the failure.
 * @param fileName         Name of the C source file in which the assertion
 *                           failure occurred.
 */
EXTERNRT void rtxErrAssertionFailed
(const char* conditionText, int lineNo, const char* fileName);

/**
 * This function formats a given error structure from the context into
 * a finished status message including substituted parameters.
 *
 * @param pErrInfo     Pointer to error information structure.
 * @param bufp         Pointer to a destination buffer to receive text.
 * @param bufsiz       Size of the buffer.
 * @return             Pointer to the buffer (bufp).
 */
EXTERNRT const char* rtxErrFmtMsg
(OSRTErrInfo* pErrInfo, char* bufp, size_t bufsiz);

/**
 * This function is used to free dynamic memory that was used in the recording
 * of error parameters. After an error is logged, this function should be
 * called to prevent memory leaks.
 *
 * @param pctxt        A pointer to a context structure.
 */
EXTERNRT void rtxErrFreeParms (OSCTXT* pctxt);

/**
 * This function returns error text in a memory buffer. If buffer pointer and
 * buffer size are specified in parameters (not NULL) then error text will be
 * copied in the passed buffer. Otherwise, this function allocates memory
 * using the 'rtxMemAlloc' function. This memory is automatically freed
 * at the time the 'rtxMemFree' or 'rtxFreeContext' functions are called.
 * The calling function may free the memory by using 'rtxMemFreePtr' function.
 *
 * @param pctxt        A pointer to a context structure.
 * @param pBuf         A pointer to a destination buffer to obtain the error
 *                       text. If NULL, dynamic buffer will be allocated.
 * @param pBufSize     A pointer to buffer size. If pBuf is NULL and this
 *                       parameter is not, it will receive the size of
 *                       allocated dynamic buffer.
 * @return             A pointer to a buffer with error text. If pBuf is not
 *                       NULL, the return pointer will be equal to it.
 *                       Otherwise, returns newly allocated buffer with error
 *                       text. NULL, if error occurred.
 */
EXTERNRT char* rtxErrGetText (OSCTXT* pctxt, char* pBuf, size_t* pBufSize);

/**
 * This function returns error text in the given fixed-size memory buffer.
 * If the text will not fit in the buffer, it is truncated.
 *
 * @param pctxt        A pointer to a context structure.
 * @param pbuf         Pointer to a destination buffer to receive text.
 * @param bufsiz       Size of the buffer.
 * @return             Pointer to the buffer (pbuf).
 */
EXTERNRT char* rtxErrGetTextBuf (OSCTXT* pctxt, char* pbuf, size_t bufsiz);

/**
 * This function creates a new empty error record for the passed context.
 * \c rtxErrSetData function call with bAllocNew = FALSE should be used to
 * set the data for this node.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             A pointer to a newly allocated error record; NULL, if
 *                     error occurred.
 */
EXTERNRT OSRTErrInfo* rtxErrNewNode (OSCTXT* pctxt);

/**
 * This function is a one-time initialization function that must be
 * called before any other error processing functions can be called.
 * It adds the common error status text codes to the global error
 * table.
 */
EXTERNRT void rtxErrInit (void);

/**
 * This function is used to reset the error state recorded in the context to
 * successful. It is used in the generated code in places where automatic error
 * correction can be done.
 *
 * @param pctxt        A pointer to a context structure.
 */
EXTERNRT int rtxErrReset (OSCTXT* pctxt);

/**
 * This function allows error information to be logged using a user-defined
 * callback routine. The callback routine is invoked IMMEDIATELY; it is not
 * a "callback" in the ordinary use of that word.  The callback routine is
 * invoked with error information in the context allowing the user to do
 * application-specific handling (for example, it can be written to an error
 * log or a Window display). After invoking the callback, this method will
 * invoked rtxErrFreeParms to free memory associated with the error information.
 *
 * The prototype of the callback function to be passed is as follows:
 *
 * <p><i>int cb (const char* ptext, void* cbArg_p);</i></p>
 *
 * <p>where <i>ptext</i> is a pointer to the formatted error text string and
 * <i>cbArg_p</i> is a pointer to a user-defined callback argument.
 *
 * @param pctxt        A pointer to a context structure.
 * @param cb           Callback function pointer.
 * @param cbArg_p      Pointer to a user-defined argument that is passed to the
 *                       callback function.
 */
EXTERNRT void rtxErrLogUsingCB (OSCTXT* pctxt, OSErrCbFunc cb, void* cbArg_p);

/**
 * This function is used to print the error information stored in the context
 * to the standard output device. Parameter substitution is done so that
 * recorded parameters are added to the output message text.
 *
 * @param pctxt        A pointer to a context structure.
 */
EXTERNRT void rtxErrPrint (OSCTXT* pctxt);

/**
 * This function is used to print the error information stored in the error
 * information element to the standard output device. Parameter substitution
 * is done so that recorded parameters are added to the output message text.
 *
 * @param pErrInfo        A pointer to an error information element.
 */
EXTERNRT void rtxErrPrintElement (OSRTErrInfo* pErrInfo);

/**
 * This function is used to record an error in the context structure. It is
 * typically called via the \c LOG_RTERR macro in the generated code to trap
 * error conditions.
 *
 * @param pctxt        A pointer to a context structure.
 * @param status       The error status code from \c rtxErrCodes.h
 * @param module       The C source file in which the error occurred.
 * @param lineno       The line number within the source file of the error.
 * @return             The status code that was passed in.
 */
EXTERNRT int rtxErrSetData (OSCTXT* pctxt, int status,
                             const char* module, int lineno);

/**
 * This function is used to record an error in the context structure. It is
 * typically called via the \c LOG_RTERRNEW macro in the generated code to trap
 * error conditions. It always allocates new error record.
 *
 * @param pctxt        A pointer to a context structure.
 * @param status       The error status code from \c rtxErrCodes.h
 * @param module       The C source file in which the error occurred.
 * @param lineno       The line number within the source file of the error.
 * @return             The status code that was passed in.
 */
EXTERNRT int rtxErrSetNewData (OSCTXT* pctxt, int status,
                                const char* module, int lineno);
/**
 * This function returns the error code, stored in the first error record.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             The first status code; zero if no error records exist.
 */
EXTERNRT int rtxErrGetFirstError (const OSCTXT* pctxt);

/**
 * This function returns the error code, stored in the last error record.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             The last status code; zero if no error records exist.
 */
EXTERNRT int rtxErrGetLastError (const OSCTXT* pctxt);

/**
 * This function returns the total number of error records.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             The total number of error records in the context.
 */
EXTERNRT OSSIZE rtxErrGetErrorCnt (const OSCTXT* pctxt);

/**
 * This function returns the status value from the context.  It
 * examines the error list to see how many errors were logged.  If
 * none, OK (zero) is returned; if one, then the status value in the
 * single error record is returned; if more than one, the special code
 * RTERR_MULTIPLE is returned to indicate that multiple errors occurred.
 *
 * @param pctxt        A pointer to a context structure.
 * @return             Status code corresponding to errors in the context.
 */
EXTERNRT int rtxErrGetStatus (const OSCTXT* pctxt);

/**
 * This function resets last 'errorsToReset` errors in the context.
 *
 * @param pctxt          A pointer to a context structure.
 * @param errorsToReset  A number of errors to reset, starting from the
 *                       last record.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxErrResetLastErrors (OSCTXT* pctxt, int errorsToReset);

/**
 * This function copies error information from one context into another.
 *
 * @param pDestCtxt    Pointer to destination context structure to
 *                       receive the copied error information.
 * @param pSrcCtxt     Pointer to source context from which error
 *                       information will be copied.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxErrCopy (OSCTXT* pDestCtxt, const OSCTXT* pSrcCtxt);

/**
 * @} ccfErr
 */

#ifdef __cplusplus
}
#endif

#endif
