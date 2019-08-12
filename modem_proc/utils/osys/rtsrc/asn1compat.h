/*
 * Copyright (c) 1997-2009 Objective Systems, Inc.
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
 * @file asn1compat.h
 * This header file contains deprecated definitions of ASN.1 runtime
 * constants, data structures, and other defintions.  It is included
 * with generated code when the '-compat' command-line switch is used.
 */
#ifndef _ASN1COMPAT_H_
#define _ASN1COMPAT_H_

#include "rtxsrc/osSysTypes.h"

#define ASN_OK          0

/* ASN.1 Primitive Type Definitions */

typedef char            ASN1CHAR;
typedef OSUINT8         ASN1OCTET;
typedef OSBOOL          ASN1BOOL;
typedef OSINT8          ASN1INT8;
typedef OSUINT8         ASN1UINT8;
typedef OSINT16         ASN1SINT;
typedef OSUINT16        ASN1USINT;
typedef OSINT32         ASN1INT;
typedef OSUINT32        ASN1UINT;
typedef OSINT32         ASN1ENUM;
typedef OSREAL          ASN1REAL;

typedef OSUNICHAR       ASN116BITCHAR;
typedef OS32BITCHAR     ASN132BITCHAR;

#define ASN1UINT_MAX    OSUINT32_MAX
#define ASN1INT_MAX     ((OSINT32)OSINT32_MAX)
#define ASN1INT_MIN     ((OSINT32)OSINT32_MIN)

/* 64-bit long integer type */

#ifndef ASN1INT64
#define ASN1INT64       OSINT64
#define ASN1UINT64      OSUINT64
#define ASN1INT64FMT    OSINT64FMT
#define ASN1UINT64FMT   OSUINT64FMT
#define ASN1INT64FMTX   OSINT64FMTX
#define ASN1INT64MAX    OSINT64MAX
#define ASN1UINT64MAX   OSUINT64MAX
#define ASN1INT64MIN    OSINT64MIN
#define ASN1I64CONST(val)  OSI64CONST(val)
#define ASN1UI64CONST(val) OSUI64CONST(val)
#endif

typedef const char* ASN1ConstCharPtr;
typedef const ASN1OCTET* ASN1ConstOctetPtr;
typedef const void* ASN1ConstVoidPtr;
typedef const ASN116BITCHAR* ASN1Const16BitCharPtr;
#define ASN1Const const

#define ASN1CTXT OSCTXT

#define rtInitContext rtAsn1InitContext
#define rtFreeContext rtAsn1FreeContext

#define xu_error rtxErrSetData
#define xu_addErrParm rtxErrAddStrParm
#define xu_addIntErrParm rtxErrAddIntParm
#define xu_addUnsignedErrParm rtxErrAddUIntParm
#define xu_log_error rtxErrLogUsingCB
#define xu_fmtErrMsg rtxErrGetText
#define xu_freeErrParms rtxErrFreeParms
#define xu_perror rtxErrPrint

#define xu_malloc rtxMemAlloc
#define xu_freeall rtxMemFree
#define ALLOC_ASN1ELEM rtxMemAllocType

#endif
