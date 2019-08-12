/*
 * Copyright (c) 1997-2013 Objective Systems, Inc.
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
 * @file rtpersrc/pu_common.hh
 */

#ifndef _PU_COMMON_HH_
#define _PU_COMMON_HH_

#include <stdlib.h>
#include "rtpersrc/asn1per.h"
#include "rtsrc/asn1intl.h"
#include "rtsrc/rt_common.hh"
#include "rtxsrc/rtxDiag.h"
#include "rtxsrc/rtxUtil.h"

OSBOOL pu_BitAndOctetStringAlignmentTest
(const Asn1SizeCnst* pSizeCnst, OSUINT32 itemCount, OSBOOL bitStrFlag);

OSUINT32 pu_bitcnt64 (OSUINT64 value);

int pu_GetDateTimeStrSize (OSUINT32 flags);

#endif
