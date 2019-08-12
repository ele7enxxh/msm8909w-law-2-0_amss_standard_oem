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
#ifndef __ASN1VERSION_H
#define __ASN1VERSION_H

#define OS_ASN1RT_VERSION 665
#define OS_ASN1RT_VERSION_STR "6.65"


#define ACVERSION OS_ASN1RT_VERSION
#define OS_ASN1RT_MAJOR_VERSION (OS_ASN1RT_VERSION/100)
#define OS_ASN1RT_MINOR_VERSION (OS_ASN1RT_VERSION%100/10)
#define OS_ASN1RT_PATCH_VERSION (OS_ASN1RT_VERSION%10)

#define OS_ASN1RT_MAJOR_VERSION_STR (OS_ASN1RT_VERSION_STR[0])
#define OS_ASN1RT_MINOR_VERSION_STR (OS_ASN1RT_VERSION_STR[2])
#define OS_ASN1RT_PATCH_VERSION_STR (OS_ASN1RT_VERSION_STR[3])

#endif /* __ASN1VERSION_H */
