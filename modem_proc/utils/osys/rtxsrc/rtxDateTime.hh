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
#ifndef _RTXDATETIME_HH_
#define _RTXDATETIME_HH_

#include <math.h>
#include <time.h>
#include "rtxsrc/rtxDateTime.h"
#include "rtxsrc/rtxErrCodes.h"
#include "rtxsrc/rtxError.h"
#include "rtxsrc/rtxCtype.h"

/**
 * PARSE_2_DIGITS:
 * @num:  the integer to fill in
 * @cur:  an #OSUTF8CHAR *
 * @invalid: an integer
 *
 * Parses a 2-digits integer and updates @num with the value. @cur is
 * updated to point just after the integer.
 * In case of error, @invalid is set to %TRUE, values of @num and
 * @cur are undefined.
 */
#define PARSE_2_DIGITS(num, cur, invalid) \
if (OS_ISDIGIT(cur[0]) == 0 || OS_ISDIGIT(cur[1]) == 0) \
   invalid = 1; \
else \
   invalid = 0,num = (cur[0] - '0') * 10 + (cur[1] - '0'); \
cur += 2;

/**
 * IS_LEAP:
 * @y:  the integer year
 *
 * Calculate whether the @y year value is leap year or not
 * the value of IS_LEAP(y) set to %true else %false
 */
#define IS_LEAP(y) (((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0))

#define FQUOTIENT(a,b)  (int)(floor((double)a/(double)b))
#define MODULO(a,b)     (a - (FQUOTIENT(a,b))*b)
#define FQUOTIENT_RANGE(a,low,high)  (FQUOTIENT((a-low),(high-low)))
#define MODULO_RANGE(a,low,high)     ((MODULO((a-low),(high-low)))+low)
#define MAX_DAY_INMONTH(yr,mon) \
((IS_LEAP(yr)&& mon == 2)? (gDaysInMonth[mon-1] + 1) : gDaysInMonth[mon-1])

#ifdef __cplusplus
extern "C" {
#endif

extern const signed char gDaysInMonth[12];

EXTERNRT int rtxParseTimeZone
   (const OSUTF8CHAR* inpdata, size_t inpdatalen, OSNumDateTime* pvalue);

EXTERNRT int rtxTimeZoneToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

EXTERNRT int rtxDatePartToString
   (const OSNumDateTime* pvalue, OSUTF8CHAR* buffer, size_t bufsize);

#ifdef __cplusplus
}
#endif

#endif /* _RTXDATETIME_HH_ */

