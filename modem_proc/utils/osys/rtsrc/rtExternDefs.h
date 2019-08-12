/*
 * Copyright (c) 2003-2009 Objective Systems, Inc.
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

#ifndef _RTEXTERNDEFS_H_
#define _RTEXTERNDEFS_H_

/**
 * @file rtExternDefs.h
 * Common definitions of external function modifiers used to define the
 * scope of functions used in DLL's (Windows only).
 */

#if defined(__SYMBIAN32__)   /* OS: Symbian */
#define EXTRTCLASS

#ifndef EXTERN
#ifdef BUILDASN1RTDLL
#define EXTERN EXPORT_C
#elif defined (USEASN1RTDLL)
#define EXTERN IMPORT_C
#else
#define EXTERN
#endif /* BUILDASN1RTDLL */
#endif /* EXTERN */

#ifndef EXTERNRT
#ifdef BUILDASN1RTDLL
#define EXTERNRT     EXPORT_C
#define EXTRTMETHOD EXPORT_C
#elif defined (USEASN1RTDLL)
#define EXTERNRT     IMPORT_C
#define EXTRTMETHOD IMPORT_C
#else
#define EXTERNRT
#define EXTRTMETHOD
#endif /* BUILDASN1RTDLL */
#endif /* EXTERNRT */

#else                         /* OS: Win, Unix, Linux, ... */
#define EXTRTMETHOD

#ifndef EXTERNRT
#if defined(BUILDASN1RTDLL) || defined(BUILDRTXDLL)
#define EXTERNRT     __declspec(dllexport)
#define EXTRTCLASS __declspec(dllexport)
#elif defined (USEASN1RTDLL)
#define EXTERNRT     __declspec(dllimport)
#define EXTRTCLASS __declspec(dllimport)
#else
#define EXTERNRT
#define EXTRTCLASS
#endif /* BUILDRTDLL */
#endif

#endif

#endif
