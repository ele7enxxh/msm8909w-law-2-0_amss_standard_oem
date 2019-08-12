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
 * @file rtpersrc/pe_common.hh
 */

#ifndef _PE_COMMON_HH_
#define _PE_COMMON_HH_

#include "rtpersrc/pu_common.hh"

int pe_bitsFromOctet (OSCTXT* pctxt, OSOCTET value, OSUINT32 nbits);
int pe_identifier (OSCTXT* pctxt, OSUINT32 ident);

#ifndef _NO_INT64_SUPPORT
int pe_identifier64 (OSCTXT* pctxt, OSUINT64 ident);
#endif

int pe_TimeDiff (OSCTXT* pctxt, const char* string);
int pe_DurationFromStr (OSCTXT* pctxt, const char* string);
int pe_DateFromStr (OSCTXT* pctxt, const char* string, OSUINT32 flags);
int pe_TimeFromStr (OSCTXT* pctxt, const char* string, OSUINT32 flags);

#endif

