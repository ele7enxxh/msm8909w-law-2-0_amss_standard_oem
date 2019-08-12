/**
 * @file wceAddon.h
 */

#if defined(_WIN32_WCE)

#ifndef _WCEADDON_H_
#define _WCEADDON_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TM_DEFINED
#define _TM_DEFINED
struct tm {
   int tm_sec;     /* seconds after the minute - [0,59] */
   int tm_min;     /* minutes after the hour - [0,59] */
   int tm_hour;    /* hours since midnight - [0,23] */
   int tm_mday;    /* day of the month - [1,31] */
   int tm_mon;     /* months since January - [0,11] */
   int tm_year;    /* years since 1900 */
   int tm_wday;    /* days since Sunday - [0,6] */
   int tm_yday;    /* days since January 1 - [0,365] */
   int tm_isdst;   /* daylight savings time flag */
};
#endif /* _TM_DEFINED */

EXTERNRT time_t      rtWCEMkTime (struct tm* ptm);
EXTERNRT struct tm*  rtWCELocalTime (const time_t *ptime);
EXTERNRT struct tm*  rtWCEGmtTime (const time_t *ptime);
EXTERNRT time_t      rtWCETime (time_t *ptime);
EXTERNRT int         rtWCEIsindst (struct tm *ptm);

EXTERNRT void        rtWCEAbort (void);

#ifdef __cplusplus
}
#endif

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED

/* Clock ticks macro - ANSI version */

#define CLOCKS_PER_SEC  1000
#endif

/* macros to replace wanting functions */
#define mktime    rtWCEMkTime
#define time      rtWCETime
#define localtime rtWCELocalTime
#define gmtime    rtWCEGmtTime
#define _isindst  rtWCEIsindst
#define abort     rtWCEAbort

#endif /*_WCEADDON_H_*/

#endif /* _WIN32_WCE */
