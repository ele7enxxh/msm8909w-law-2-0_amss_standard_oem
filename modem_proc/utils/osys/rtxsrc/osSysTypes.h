/*
 * Copyright (c) 1997-2012 Objective Systems, Inc.
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

#ifndef _OSSYSTYPES_H_
#define _OSSYSTYPES_H_

/* Ensure that the appropriate limits are active. */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#if (!defined(BREW_MODULE) && !defined(FLAT_BREW))
#include <stdlib.h>
#else
/* Special include for Qualcomm BREW environment */
/* #include "AEEStdLib.h" */
#endif

#if !defined(_MSC_VER) && !defined(__BORLANDC__) && !defined(__WATCOMC__) \
   && !defined(__vxworks) && !defined(__SYMBIAN32__) && !defined(NIOS2)
#include <inttypes.h> /* does not exist in windows; use builtins instead */
#endif /* not windows */

#include <limits.h>

#include <float.h>

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

#define NumberOf(x)	(sizeof(x)/sizeof(x[0]))

typedef void            OSVoid;
typedef void*           OSVoidPtr;
typedef unsigned char   OSBOOL;
typedef signed char     OSINT8;
typedef unsigned char   OSUINT8;
typedef short           OSINT16;
typedef unsigned short  OSUINT16;
typedef OSUINT8		OSOCTET;
typedef OSUINT8         OSUTF8CHAR;
typedef OSUINT16        OSUNICHAR;
typedef double          OSREAL;
typedef double          OSDOUBLE;
typedef float           OSFLOAT;
#ifdef _16BIT
typedef long            OSINT32;
typedef unsigned long   OSUINT32;
#define OSINTCONST(val)  val##L
#define OSUINTCONST(val) val##UL
#else
typedef int             OSINT32;
typedef unsigned int    OSUINT32;
#define OSINTCONST(val)  val
#define OSUINTCONST(val) val##u
#endif /* _16BIT */
typedef OSUINT32        OS32BITCHAR;
typedef size_t          OSSIZE;

#define OSUINT32_MAX    ((OSUINT32)4294967295UL)
#define OSINT32_MAX     ((OSINT32)2147483647L)
#define OSINT32_MIN     ((OSINT32)(-OSINT32_MAX-1))

#define OSUINT16_MAX    ((OSUINT16)65535UL)
#define OSINT16_MAX     ((OSINT16)32767L)
#define OSINT16_MIN     ((OSINT16)(-OSINT16_MAX-1))

#define OSUINT8_MAX     ((OSUINT8)255U)
#define OSINT8_MAX      ((OSINT8)127L)
#define OSINT8_MIN      ((OSINT8)(-OSINT8_MAX-1))

#define OSREALMAX       ((OSREAL)DBL_MAX)
#define OSREALMIN       ((OSREAL)-DBL_MAX)

#define OSU32NULLIDX    OSUINT32_MAX
#define OSNULLINDEX     ((OSSIZE)-1)

/**
 * @struct OSNumDateTime
 * Numeric date/time structure
 */
typedef struct OSNumDateTime {
   OSINT32      year;
   OSUINT8      mon;            /* 1 <=  mon    <= 12   */
   OSUINT8      day;            /* 1 <=  day    <= 31   */
   OSUINT8      hour;           /* 0 <=  hour   <= 23   */
   OSUINT8      min;            /* 0 <=  min    <= 59	*/
   OSREAL       sec;
   OSBOOL       tz_flag;        /* is tzo explicitely set? */
   OSINT32      tzo;            /* -840 <= tzo <= 840 */
} OSNumDateTime;

/* 64-bit long integer type */

#ifndef OSINT64_
#if !defined(_NO_INT64_SUPPORT)
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#define OSINT64_
typedef __int64 OSINT64;
typedef unsigned __int64 OSUINT64;
#define OSINT64FMT  "%I64d"
#define OSUINT64FMT "%I64u"
#define OSINT64FMTX "%I64x"
#define OSINT64MAX  _I64_MAX
#define OSUINT64MAX _UI64_MAX
#define OSINT64MIN  _I64_MIN
#define OSI64CONST(val)  val##i64
#define OSUI64CONST(val) val##ui64

#elif defined(INT64_MAX) /* assume have ISO C99 standard types */
#define OSINT64_
typedef int64_t OSINT64;
typedef uint64_t OSUINT64;
#ifdef PRId64 /* have C99 format macros */
#define OSINT64FMT  "%"PRId64
#define OSUINT64FMT "%"PRIu64
#define OSINT64FMTX "%"PRIx64
#else /* have C99 types but not format macros, assume long long format */
#define OSINT64FMT  "%lld"
#define OSUINT64FMT "%llu"
#define OSINT64FMTX "%llx"
#endif /* PRId64 */
#define OSINT64MAX  INT64_MAX
#define OSUINT64MAX UINT64_MAX
#define OSINT64MIN  INT64_MIN
#define OSI64CONST(val)  INT64_C(val)
#define OSUI64CONST(val) UINT64_C(val)

#else /* ?? __WORDSIZE != 64 */
#ifdef __GNUC__ // warning is a GNU compiler extension
#warning "Using long long for 64-bit integers."
#endif
#define OSINT64_
typedef long long OSINT64;
typedef unsigned long long OSUINT64;
#define OSINT64FMT  "%lld"
#define OSUINT64FMT "%llu"
#define OSINT64FMTX "%llx"
#define OSI64CONST(val)  val##LL
#define OSUI64CONST(val) val##ULL

/* May throw an error if __STDC_LIMIT_MACROS is not defined */
#ifdef LLONG_MAX
#define OSINT64MAX  LLONG_MAX
#define OSUINT64MAX ULLONG_MAX
#define OSINT64MIN  LLONG_MIN
#elif defined LONG_LONG_MAX /* special code for gcc 3.3.3 */
#define OSINT64MAX  LONG_LONG_MAX
#define OSUINT64MAX ULONG_LONG_MAX
#define OSINT64MIN  LONG_LONG_MIN
#elif defined __INT64_MAX /* VMS */
#define OSINT64MAX  __INT64_MAX
#define OSUINT64MAX __UINT64_MAX
#define OSINT64MIN  __INT64_MIN
#else
#warning "Using LONG_MAX for 64-bit maximum."
#define OSINT64MAX LONG_MAX
#define OSUINT64MAX ULONG_MAX
#define OSINT64MIN LONG_MIN
#endif

#endif /* _MSC_VER ... */
#endif /* !defined(_NO_INT64_SUPPORT) */

#ifndef OSINT64_
/* if OSINT64 is still not defined - define it as long/unsigned long */
#define OSINT64_
typedef long OSINT64;
typedef unsigned long OSUINT64;
#define OSINT64FMT  "%ld"
#define OSUINT64FMT "%lu"
#define OSINT64FMTX "%lx"
#define OSINT64MAX  LONG_MAX
#define OSUINT64MAX ULONG_MAX
#define OSINT64MIN  LONG_MIN
#define OSI64CONST(val)  val##L
#define OSUI64CONST(val) val##UL
#endif /* OSINT64 ... */

#define OSU64NULLIDX    OSUINT64MAX

#endif /* OSINT64 */

/**
 * @struct OSDynOctStr osSysTypes.h
 * Dynamic binary string structure
 *
 * This structure is used in generated code for XSD hexBinary and
 * base64Binary types.
 */
typedef struct OSDynOctStr {
   OSUINT32     numocts;
   const OSOCTET* data;
} OSDynOctStr;

typedef OSDynOctStr OSOpenType;

/**
 * @struct OSXSDAny
 * Structure to hold xsd:any data in binary and XML text form.
 */
typedef enum { OSXSDAny_binary, OSXSDAny_xmlText } OSXSDAnyAlt;
typedef struct OSXSDAny {
   OSXSDAnyAlt t;
   union {
      OSOpenType* binary;
      const OSUTF8CHAR* xmlText;
   } u;
} OSXSDAny;

/* UTF-8 name/value pair */

typedef struct OSUTF8NVP {
   const OSUTF8CHAR* name;
   const OSUTF8CHAR* value;
} OSUTF8NVP;

typedef OSUTF8NVP OSAnyAttr;
typedef OSUTF8NVP OSAnyElement;

/* XML string */

/**
 * @struct OSXMLSTRING osSysTypes.h
 * XML UTF-8 character string structure
 *
 * This structure is used in generated code for XML string types.
 */
typedef struct OSXMLSTRING {
   OSBOOL cdata;                /* encode as a CDATA section */
   const OSUTF8CHAR* value;
} OSXMLSTRING;

/**
 * @struct OSXSDDateTime
 * Numeric date/time structure
 *
 * This structure is used in generated code for XSD date/time types
 * when code generation is configured to use numeric date/time types
 * (-numDateTime command-line option).
 */
typedef OSNumDateTime OSXSDDateTime;

/**
 * @struct OSBitMapItem
 * Named bit in a bit map.
 *
 * This structure is used to equate a name with a bit in a bit map.
 */
typedef struct OSBitMapItem {
   const OSUTF8CHAR* name;
   OSUINT16          bitno;
   OSUINT16          namelen;
} OSBitMapItem;

/**
 * @struct OSUTF8NameAndLen
 * UTF-8 name and length structure.
 *
 * This structure holds a pointer to UTF-8 text (it does not need
 * to be null-terminated) and a variable containing the length of the
 * string.  The primary use of this structure is to improve performance
 * in token matching by not having to calculate string length every time.
 */
typedef struct OSUTF8NameAndLen {
   const OSUTF8CHAR* name;
   OSSIZE len;
} OSUTF8NameAndLen;

#endif
