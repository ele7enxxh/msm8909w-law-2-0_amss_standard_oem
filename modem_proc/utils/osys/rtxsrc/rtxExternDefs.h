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

#ifndef _RTXEXTERNDEFS_H_
#define _RTXEXTERNDEFS_H_

/**
 * @file rtxExternDefs.h
 * Common definitions of external function modifiers used to define the
 * scope of functions used in DLL's (Windows only).
 */

/* Symbian requires different exporting rules, and so we define different
 * constants to turn on or off the appropriate linkage types.  See
 * rtsrc/rtExternDefs.h for more detail. */
#if defined(__SYMBIAN32__)       /* OS: Symbian */
#define EXTRTCLASS               /* turn off class externs */

#ifndef EXTERN
#ifdef BUILDDLL
#define EXTERN EXPORT_C
#elif defined (USEDLL)
#define EXTERN IMPORT_C
#else
#define EXTERN
#endif /* BUILD/USE DLL */
#endif /* EXTERN */

#ifndef EXTERNRT
#if defined(BUILDASN1RTDLL) || defined(BUILDRTXDLL)
#define EXTERNRT     EXPORT_C
#define EXTRTMETHOD  EXPORT_C    /* turn on method externs */
#elif defined (USEASN1RTDLL) || defined(USERTXDLL)
#define EXTERNRT     IMPORT_C
#define EXTRTMETHOD  IMPORT_C
#else
#define EXTERNRT
#define EXTRTMETHOD
#endif /* BUILDASN1RTDLL */
#endif /* EXTERNRT */

#else                            /* OS: Win, Unix, Linux, ... */
#define EXTRTMETHOD              /* turn off method externs */

#ifdef _WIN32
#ifndef EXTERNRT
#if defined(BUILDASN1RTDLL) || defined(BUILDRTXDLL)
#define EXTERNRT        __declspec(dllexport)
#define EXTRTCLASS      __declspec(dllexport)
#elif defined (USERTXDLL) || defined(USEASN1RTDLL)
#define EXTERNRT        __declspec(dllimport)
#define EXTRTCLASS      __declspec(dllimport)
#else
#define EXTERNRT
#define EXTRTCLASS
#endif /* BUILDASN1RTDLL */
#endif /* EXTERNRT */

#ifdef BUILDDLL
#define EXTERN __declspec(dllexport)
#elif defined (USEDLL)
#define EXTERN __declspec(dllimport)
#else
#define EXTERN
#endif /* BUILDDLL */

#else /* not _WIN32 */
#define EXTERNRT
#define EXTRTCLASS
#define EXTERN
#endif

#endif
#endif
