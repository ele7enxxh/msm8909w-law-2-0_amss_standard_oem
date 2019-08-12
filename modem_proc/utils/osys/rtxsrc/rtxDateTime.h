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
 * @file rtxDateTime.h
 * Common runtime functions for converting to and from various standard
 * date/time formats.
 */
#ifndef _RTXDATETIME_H_
#define _RTXDATETIME_H_

#ifdef T_QDSP6 /* QC_MOD */
#include <common/time.h>
#else
#include <time.h>
#endif /*T_QDSP6*/

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ccfDateTime Date/time conversion functions
 * @{
 *
 * These functions handle the conversion of date/time to and from various
 * internal formats to XML schema standard string forms.
 */
/**
 * This function formats a numeric date value consisting of individual date
 * components (year, month, day) into XML schema standard format (CCYY-MM-DD).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     date components to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is at least nine bytes long to hold the formatted
 *                     date and a null-terminator character.
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       <ul>
 *                       <li>0(RT_OK) = success,</li>
 *                       <li>negative return value is error</li>
 *                       </ul>
 */
EXTERNRT int rtxDateToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a numeric time value consisting of individual time
 * components (hour, minute, second, fraction-of-second, time zone) into
 * XML schema standard format (HH:MM:SS[.frac][TZ]).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     time components to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string.
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxTimeToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a numeric date/time value of all components
 * in the OSNumDateTime structure into XML schema standard format
 * (CCYY-MM-DDTHH:MM:SS[.frac][TZ]).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     date/time components to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string.
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxDateTimeToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a gregorian year value to a string (CCYY).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     year value to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string
 *                     (in this case, at least 5 characters long).
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxGYearToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a gregorian year and month value to a string
 * (CCYY-MM).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     year and month value to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string
 *                     (in this case, at least 8 characters long).
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxGYearMonthToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a gregorian month value to a string (MM).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     month value to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string
 *                     (in this case, at least 3 characters long).
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxGMonthToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a gregorian month and day value to a string
 * (MM-DD).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     month and day value to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string
 *                     (in this case, at least 6 characters long).
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxGMonthDayToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function formats a gregorian day value to a string (DD).
 *
 * @param pvalue      Pointer to OSNumDateTime structure containing
 *                     day value to be formatted.
 * @param buffer       Buffer into which date is to be formatted.  This is
 *                     a fixed-sized buffer.  The user must provide a buffer
 *                     that is large enough to hold the formatted time string
 *                     (in this case, at least 3 characters long).
 * @param bufsize      Size of the buffer to receive the formatted date.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 */
EXTERNRT int rtxGDayToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

/**
 * This function reads the system date and time and stores the value in
 * the given OSNumDateTime structure variable.
 *
 * @param pvalue       Pointer to OSNumDateTime structure.
 * @return             Completion status of operation:
 *                     - 0 in case success
 *                     - negative in case failure
 */
EXTERNRT int rtxGetCurrDateTime (OSNumDateTime* pvalue);

/**
 * This function compares the date part of two OSNumDateTime structures and
 * returns the result of the comparison.
 *
 * @param pvalue1       Pointer to OSNumDateTime structure.
 * @param pvalue2       Pointer to OSNumDateTime structure.
 * @return              Completion status of operation:
 *                      - 0 Dates are same
 *                      - +1 First Date/Time is greater than second.
 *                      - -1 First Date/Time is less than second.
 */
EXTERNRT int rtxCmpDate
   (const OSNumDateTime* pvalue1, const OSNumDateTime* pvalue2);

/**
 * This function compares the date part of OSNumDateTime structure and
 * date components, specified as parameters.
 *
 * @param pvalue        Pointer to OSNumDateTime structure.
 * @param year          Year (-inf..inf)
 * @param mon           Month (1..12)
 * @param day           Day  (1..31)
 * @param tzflag        TRUE, if time zone offset is set (see tzo parameter).
 * @param tzo           Time zone offset (-840..840).
 * @return              Completion status of operation:
 *                      - 0 Dates are same
 *                      - +1 First Date/Time is greater than second.
 *                      - -1 First Date/Time is less than second.
 */
EXTERNRT int rtxCmpDate2
   (const OSNumDateTime* pvalue,
    OSINT32 year, OSUINT8 mon, OSUINT8 day, OSBOOL tzflag, OSINT32 tzo);

/**
 * This function compares the time part of two OSNumDateTime structures and
 * returns the result of the comparison.
 *
 * @param pvalue1       Pointer to OSNumDateTime structure.
 * @param pvalue2       Pointer to OSNumDateTime structure.
 * @return              Completion status of operation:
 *                      - 0 Times are same
 *                      - +1 First Date/Time is greater than second.
 *                      - -1 First Date/Time is less than second.
 */
EXTERNRT int rtxCmpTime
   (const OSNumDateTime* pvalue1, const OSNumDateTime* pvalue2);

/**
 * This function compares the time part of OSNumDateTime structure and
 * time components, specified as parameters.
 *
 * @param pvalue        Pointer to OSNumDateTime structure.
 * @param hour          Hour (0..23)
 * @param min           Minutes (0..59)
 * @param sec           Seconds  (0.0..59.(9))
 * @param tzflag        TRUE, if time zone offset is set (see tzo parameter).
 * @param tzo           Time zone offset (-840..840).
 * @return              Completion status of operation:
 *                      - 0 Times are same
 *                      - +1 First Date/Time is greater than second.
 *                      - -1 First Date/Time is less than second.
 */
EXTERNRT int rtxCmpTime2
   (const OSNumDateTime* pvalue,
    OSUINT8 hour, OSUINT8 min, OSREAL sec, OSBOOL tzflag, OSINT32 tzo);

/**
 * This function compares two OSNumDateTime structures and
 * returns the result of the comparison.
 *
 * @param pvalue1       Pointer to OSNumDateTime structure.
 * @param pvalue2       Pointer to OSNumDateTime structure.
 * @return              Completion status of operation:
 *                      - 0 Dates are same
 *                      - +1 First Date/Time is greater than second.
 *                      - -1 First Date/Time is less than second.
 */
EXTERNRT int rtxCmpDateTime
   (const OSNumDateTime* pvalue1, const OSNumDateTime* pvalue2);

/**
 * This function compares the OSNumDateTime structure and
 * dateTime components, specified as parameters.
 *
 * @param pvalue        Pointer to OSNumDateTime structure.
 * @param year          Year (-inf..inf)
 * @param mon           Month (1..12)
 * @param day           Day  (1..31)
 * @param hour          Hour (0..23)
 * @param min           Minutes (0..59)
 * @param sec           Seconds  (0.0..59.(9))
 * @param tzflag        TRUE, if time zone offset is set (see tzo parameter).
 * @param tzo           Time zone offset (-840..840).
 * @return              Completion status of operation:
 *                      - 0 Dates are same
 *                      - +1 First Date/Time is greater than second.
 *                      - -1 First Date/Time is less than second.
 */
EXTERNRT int rtxCmpDateTime2
   (const OSNumDateTime* pvalue,
    OSINT32 year, OSUINT8 mon, OSUINT8 day,
    OSUINT8 hour, OSUINT8 min, OSREAL sec, OSBOOL tzflag, OSINT32 tzo);

/**
 * This function decodes a date value from a supplied string and sets the
 * given OSNumDateTime argument to the decoded date value.
 *
 * @param inpdata      Date string to be parsed/decoded as OSNumDateTime.<ul>
 *                     <li>The format of date is CCYY-MM-DD</li>
 *                     <li>The value of CCYY is from 0000-9999</li>
 *                     <li>The value of MM is 01 - 12</li>
 *                     <li>The value of DD is 01 - XX
 *                     (where XX is the Days in MM month in CCYY year)</li>
 *                     </ul>
 * @param inpdatalen   For decoding, consider inpdata string up to this length.
 * @param pvalue       The OSNumDateTime structure variable will be set
 *                     to the decoded date value.
 *                     - Only year, month,day value will be set.
 *                     - The value of pvalue->year is in range 0 to 9999
 *                     - The value of pvalue->mon is in range 1 to 12
 *                     - The value of pvalue->day is in range 1 to XX
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseDateString
(const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function decodes a time value from a supplied string and sets the
 * given OSNumDateTime structure to the decoded time value.
 *
 * @param inpdata      The inpdata is a time string to be parsed/decoded as
 *                     OSNumDateTime.
 *                     - The format of date is hh:mm:ss.ss   (1)
 *                                 or hh:mm:ss.ssZ           (2)
 *                                 or hh:mm:ss.ss+HH:MM      (3)
 *                                 or hh:mm:ss.ss-HH:MM      (4)
 *                     - The value of hh is from 00-23
 *                     - The value of mm is 00 - 59
 *                     - The value of ss.ss is 00.00 - 59.99
 *                     - The value of HH:MM is 00.00 - 24.00
 * @param inpdatalen   For decoding, consider the inpdata string up to
 *                     this length.
 * @param pvalue       The OSNumDateTime structure variable will be
 *                     set to the decoded time value.
 *                     - Only hour, min, sec value will be set.
 *                     - The value of pvalue->hour is in range 0 to 23
 *                     - The value of pvalue->mon is in range 0 to 59
 *                     - The value of pvalue->day is in range 0 to 59.99
 *                     - The value of pvalue->tz_flag is FALSE for
 *                       format(1) otherwise TRUE
 *                     - The value of pvalue->tzo is 0 for format(2)
 *                       otherwise Calculation of pvalue->tzo for
 *                       format (3),(4) is HH*60+MM
 *                     - The value of pvalue->tzo is -840 <= tzo <= 840 for
 *                       format(3),(4) otherwise
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseTimeString
(const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);


/**
 * This function decodes a datetime value from a supplied string and sets the
 * given OSNumDateTime to the decoded date and time value.
 *
 * @param inpdata      Input date/time string to be parsed.
 * @param inpdatalen   For decoding, consider the inpdata string up to this
 *                     length.
 * @param pvalue       The pointed OSNumDateTime structure variable will be
 *                     set to the decoded date and time value.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseDateTimeString
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function decodes a gregorian year value from a supplied string
 * and sets the year in the given OSNumDateTime to the decoded value.
 *
 * @param inpdata      Input string to be parsed.
 * @param inpdatalen   For decoding, consider the inpdata string up to this
 *                     length.
 * @param pvalue       The year field in the given OSNumDateTime structure
 *                     variable will be set to the decoded value.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseGYearString
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function decodes a gregorian year and month value from a supplied
 * string and sets the year and month fields in the given OSNumDateTime to the
 * decoded values.
 *
 * @param inpdata      Input string to be parsed.
 * @param inpdatalen   For decoding, consider the inpdata string up to this
 *                     length.
 * @param pvalue       The year and month fields in the given OSNumDateTime
 *                     variable will be set to the decoded value.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseGYearMonthString
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function decodes a gregorian month value from a supplied
 * string and sets the month field in the given OSNumDateTime to the
 * decoded value.
 *
 * @param inpdata      Input string to be parsed.
 * @param inpdatalen   For decoding, consider the inpdata string up to this
 *                     length.
 * @param pvalue       The month field in the given OSNumDateTime
 *                     variable will be set to the decoded value.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseGMonthString
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function decodes a gregorian month and day value from a supplied
 * string and sets the month and day fields in the given OSNumDateTime to the
 * decoded values.
 *
 * @param inpdata      Input string to be parsed.
 * @param inpdatalen   For decoding, consider the inpdata string up to this
 *                     length.
 * @param pvalue       The month and day fields in the given OSNumDateTime
 *                     variable will be set to the decoded values.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseGMonthDayString
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function decodes a gregorian day value from a supplied
 * string and sets the day field in the given OSNumDateTime to the
 * decoded value.
 *
 * @param inpdata      Input string to be parsed.
 * @param inpdatalen   For decoding, consider the inpdata string up to this
 *                     length.
 * @param pvalue       The day field in the given OSNumDateTime
 *                     variable will be set to the decoded value.
 * @return             Completion status of operation:
 *                       - 0(RT_OK) = success,
 *                       - negative return value is error
 *                         (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_BADVALUE).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxParseGDayString
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

/**
 * This function converts millisecs to a duration string with format
 * "PnYnMnDTnHnMnS". In case of negative duration a minus sign is prepended
 * to the output string
 * @param msecs       Number of milliseconds.
 * @param buf         Output buffer to recieve formatted duration.
 * @param bufsize     Output buffer size.
 * @return              Completion status of operation:
 *                      0 successful are same
 *                     -1 unsuccessul
 */
EXTERNRT int rtxMSecsToDuration
(OSINT32 msecs, OSUTF8CHAR* buf, OSUINT32 bufsize);

/**
 * This function converts a duration string to milliseconds. In the case
 * of a string prepended with a minus sign (-) the duration in milliseconds
 * will have negative value.
 *
 * @param buf         Pointer to OSUTF8CHAR array.
 * @param bufsize     OSINT32 indicates the bufsize to be read.
 * @param msecs       OSINT32 updated after calculation.
 * @return            Completion status of operation:
 *                  - 0(RT_OK) = success,
 *                  - negative return value is error
 *                    (RTERR_NOTINIT/RTERR_INVFORMAT/RTERR_TOOBIG).
 *                       Return value is taken from rtxErrCodes.h header file
 */
EXTERNRT int rtxDurationToMSecs
(OSUTF8CHAR* buf, OSUINT32 bufsize, OSINT32* msecs);

/**
 * This function converts a structure of type 'struct tm' to an
 * OSNumDateTime structure.
 *
 * @param pvalue      The pointed OSNumDateTime structure variable will be
 *                       set to time value.
 * @param timeStruct  A pointer to tm structure to be converted.
 * @return            Completion status of operation:
 *                  - 0(RT_OK) = success,
 *                  - negative return value is error.
 */
EXTERNRT int rtxSetDateTime (OSNumDateTime* pvalue, struct tm* timeStruct);

/**
 * This function converts a local date and time value to an OSNumDateTime
 * structure.
 *
 * @param pvalue      The pointed OSNumDateTime structure variable will be
 *                       set to time value.
 * @param timeMs      A calendar time encoded as a value of type time_t.
 * @return            Completion status of operation:
 *                  - 0(RT_OK) = success,
 *                  - negative return value is error.
 */
EXTERNRT int rtxSetLocalDateTime (OSNumDateTime* pvalue, time_t timeMs);

/**
 * This function converts a UTC date and time value to an OSNumDateTime
 * structure.
 *
 * @param pvalue      The pointed OSNumDateTime structure variable will be
 *                      set to time value.
 * @param timeMs      A calendar time encoded as a value of type time_t.
 *                      The time is represented as seconds elapsed since
 *                      midnight (00:00:00), January 1, 1970, coordinated
 *                      universal time (UTC).
 * @return            Completion status of operation:
 *                  - 0(RT_OK) = success,
 *                  - negative return value is error.
 */
EXTERNRT int rtxSetUtcDateTime (OSNumDateTime* pvalue, time_t timeMs);

/**
 * This function converts an OSNumDateTime structure to a calendar time
 * encoded as a value of type time_t.
 *
 * @param pvalue      The pointed OSNumDateTime structure variable to be
 *                       converted.
 * @param timeMs      A pointer to time_t value to be set.
 * @return            Completion status of operation:
 *                  - 0(RT_OK) = success,
 *                  - negative return value is error.
 */
EXTERNRT int rtxGetDateTime (const OSNumDateTime* pvalue, time_t* timeMs);

/**
 * This function verifies that date members (year, month, day, timezone)
 * of the OSNumDateTime structure contains valid values.
 *
 * @param pvalue      Pointer to OSNumDateTime structure to be checked.
 * @return            Boolean result: true means data is valid.
 */
EXTERNRT OSBOOL rtxDateIsValid (const OSNumDateTime* pvalue);

/**
 * This function verifies that time members (hour, minute, second, timezone)
 * of the OSNumDateTime structure contains valid values.
 *
 * @param pvalue      Pointer to OSNumDateTime structure to be checked.
 * @return            Boolean result: true means data is valid.
 */
EXTERNRT OSBOOL rtxTimeIsValid (const OSNumDateTime* pvalue);

/**
 * This function verifies that all members of the OSNumDateTime structure
 * contains valid values.
 *
 * @param pvalue      Pointer to OSNumDateTime structure to be checked.
 * @return            Boolean result: true means data is valid.
 */
EXTERNRT OSBOOL rtxDateTimeIsValid (const OSNumDateTime* pvalue);

/**
 * @} ccfDateTime
 */

#ifdef __cplusplus
}
#endif

#endif
