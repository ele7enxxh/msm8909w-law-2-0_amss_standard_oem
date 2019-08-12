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
 * @file rtxFloat.h
 */

#ifndef _RTXFLOAT_H_
#define _RTXFLOAT_H_

#include "rtxsrc/rtxCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _NO_ULP_EQUAL

/* Fall back to normal equality test */
#define rtxFloatEqual(a, b)           (a == b)
#define rtxFloatGreater(a, b)         (a >  b)
#define rtxFloatGreaterOrEqual(a, b)  (a >= b)
#define rtxFloatSmaller(a, b)         (a <  b)
#define rtxFloatSmallerOrEqual(a, b)  (a <= b)
#ifndef __SYMBIAN32__
#define rtxDoubleEqual(a, b)          (a == b)
#define rtxDoubleGreater(a, b)        (a >  b)
#define rtxDoubleGreaterOrEqual(a, b) (a >= b)
#define rtxDoubleSmaller(a, b)        (a <  b)
#define rtxDoubleSmallerOrEqual(a, b) (a <= b)
#endif /* not building for Symbian */

#else /* _NO_ULP_EQUAL */

#define rtxFloatEqual(a, b) rtxFloatEqualImpl(a, b)
#define rtxFloatGreater(a, b)         ((a>b) && !rtxFloatEqual(a,b))
#define rtxFloatGreaterOrEqual(a, b)  ((a>b) ||  rtxFloatEqual(a,b))
#define rtxFloatSmaller(a, b)         ((a<b) && !rtxFloatEqual(a,b))
#define rtxFloatSmallerOrEqual(a, b)  ((a<b) ||  rtxFloatEqual(a,b))
#ifndef __SYMBIAN32__
#define rtxDoubleEqual(a, b) rtxDoubleEqualImpl(a, b)
#define rtxDoubleGreater(a, b)        ((a>b) && !rtxDoubleEqual(a,b))
#define rtxDoubleGreaterOrEqual(a, b) ((a>b) ||  rtxDoubleEqual(a,b))
#define rtxDoubleSmaller(a, b)        ((a<b) && !rtxDoubleEqual(a,b))
#define rtxDoubleSmallerOrEqual(a, b) ((a<b) ||  rtxDoubleEqual(a,b))
#endif /* not building for Symbian */

#endif /* _NO_ULP_EQUAL */

EXTERNRT OSBOOL rtxFloatEqualImpl(float a, float b);

#ifndef __SYMBIAN32__
EXTERNRT OSBOOL rtxDoubleEqualImpl(double a, double b);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTXFLOAT_H_ */

