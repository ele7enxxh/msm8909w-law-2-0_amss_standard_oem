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
#ifndef _RTXCOMMONDEFS_H_
#define _RTXCOMMONDEFS_H_

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#ifdef T_QDSP6 /* QC_MOD */
#include <common/time.h>
#else
#include <time.h>
#endif /*T_QDSP6*/
#include "rtxsrc/osMacros.h"

/* Constants */

#define OSRTMEMBUFSEG   1024    /* memory buffer extent size            */

/*
 * Definitions for common run-time functions that may not be availabe on
 * certain embedded platforms.  These definitions can be changed to
 * use custom functions for these operations.
 */
#if (!defined(BREW_MODULE) && !defined(FLAT_BREW))
#define OSCRTLMEMSET    memset
#define OSCRTLMEMCMP    memcmp
#define OSCRTLMEMCPY    memcpy
#define OSCRTLMEMMOVE   memmove
#define OSCRTLSTRCHR    strchr
#define OSCRTLSTRCPY    strcpy
#define OSCRTLSTRNCPY   strncpy
#define OSCRTLSTRCAT    strcat
#define OSCRTLSTRNCAT   strncat
#define OSCRTLSTRCMP    strcmp
#define OSCRTLSTRNCMP   strncmp
#define OSCRTLSTRLEN    strlen
#define OSCRTLMALLOC    malloc
#define OSCRTLREALLOC   realloc
#define OSCRTLFREE      free

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#define OSCRTLSTRNCPY_S(buf,bufsiz,src,nchars) \
strncpy_s(buf,bufsiz,src,nchars)

#define OSCRTLSTRICMP(str1,str2) _stricmp(str1,str2)
#else
#define OSCRTLSTRNCPY_S(buf,bufsiz,src,nchars) strncpy(buf,src,nchars)
#define OSCRTLSTRICMP(str1,str2) stricmp(str1,str2)
#endif

#else
#define OSCRTLMEMSET    MEMSET
#define OSCRTLMEMCMP    MEMCMP
#define OSCRTLMEMCPY    MEMCPY
#define OSCRTLMEMMOVE   MEMMOVE
#define OSCRTLSTRCHR    STRCHR
#define OSCRTLSTRCPY    STRCPY
#define OSCRTLSTRNCPY   STRNCPY
#define OSCRTLSTRNCPY_S STRNCPY
#define OSCRTLSTRCAT    STRCAT
#define OSCRTLSTRNCAT   STRNCAT
#define OSCRTLSTRCMP    STRCMP
#define OSCRTLSTRNCMP   STRNCMP
#define OSCRTLSTRLEN    STRLEN
#define OSCRTLMALLOC    MALLOC
#define OSCRTLREALLOC   REALLOC
#define OSCRTLFREE      FREE
#endif

/* Macros */

#define OSRTMAX(a,b)        (((a)>(b))?(a):(b))
#define OSRTMIN(a,b)        (((a)<(b))?(a):(b))

#define HEXCHARTONIBBLE(ch,b) do { \
if (ch >= '0' && ch <= '9') b = (unsigned char)(ch - '0'); \
else if (ch >= 'a' && ch <= 'f') b = (unsigned char)((ch - 'a') + 10); \
else if (ch >= 'A' && ch <= 'F') b = (unsigned char)((ch - 'A') + 10); \
else b = 0xFF; } while(0)

#define NIBBLETOHEXCHAR(b,ch) do { \
if (b >= 0 && b <= 9) ch = (char)(b + '0'); \
else if (b >= 0x0a && b <= 0x0f) ch = (char)((b - 10)+ 'a'); \
else ch = '?'; } while(0)

#define NIBBLETOUCHEXCHAR(b,ch) do { \
if (b >= 0 && b <= 9) ch = (char)(b + '0'); \
else if (b >= 0x0a && b <= 0x0f) ch = (char)((b - 10)+ 'A'); \
else ch = '?'; } while(0)

#define OSRTSAFECONSTCAST(towhere,ptr) ((towhere)((size_t)ptr))

#define OS_WHITESPACE_COLLAPSE 0
#define OS_WHITESPACE_PRESERVE 1
#define OS_WHITESPACE_REPLACE  2

#define OS_ISNULLPTR(ptr) (((const void*)0) == ptr)

#if defined(_MSC_VER)
/*  this disables 'conditional expression is constant' warning */
#pragma warning(disable: 4127)
/*  this disables 'unreferenced formal parameter' warning */
#pragma warning(disable: 4100)
/*  this disables 'automatic inline expansion' warning */
#pragma warning(disable: 4711)
#endif

#endif /* _RTXCOMMONDEFS_H_ */
