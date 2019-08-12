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

#ifndef _RTXTOKEN_H_
#define _RTXTOKEN_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
   const OSUTF8CHAR* pStr;
   const OSUTF8CHAR* pEndStr;
   const char* delimiters;
   size_t      delimSize;

   const OSUTF8CHAR* pLastToken;
   size_t      lastTokenSize;

   OSBOOL      bFinal;
} OSCONSTTOKENCTXT;

EXTERNRT OSBOOL
rtxTokIsWhiteSpaceConst (OSCONSTTOKENCTXT* pTokCtxt, OSUTF8CHAR ch);

EXTERNRT const OSUTF8CHAR*
rtxTokSkipWhiteSpacesConst (OSCONSTTOKENCTXT* pTokCtxt);

EXTERNRT const OSUTF8CHAR*
rtxTokGetNextConst (OSCONSTTOKENCTXT* pTokCtxt, size_t* pTokenLength);

EXTERNRT const OSUTF8CHAR*
rtxTokGetFirstConst (OSCONSTTOKENCTXT* pTokCtxt,
                     const OSUTF8CHAR* pStr, size_t strSize,
                     const char* delimiters, size_t delimSize,
                     size_t* pTokenLength, OSBOOL isFinal);

#ifdef __cplusplus
}
#endif

#endif
