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
 * @file rtxDecimal.h
 * Common runtime functions for working with xsd:decimal numbers.
 */
#ifndef _RTXDECIMAL_H_
#define _RTXDECIMAL_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxDecimal Decimal number utility functions
 * @{
 * Decimal utility function provide run-time functions for handling
 * decimal number types defined within a schema.
 */
/* These need to be documented.. */

EXTERNRT const char* rtxNR3toDecimal (OSCTXT *pctxt, const char* object_p);

/**
 * @} rtxDecimal
 */

#ifdef __cplusplus
}
#endif

#endif
