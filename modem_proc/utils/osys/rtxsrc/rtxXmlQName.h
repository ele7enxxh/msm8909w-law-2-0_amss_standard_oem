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
 * @file rtxXmlQName.h
 * XML QName type definition and associated utility functions.
 */
#ifndef _RTXXMLQNAME_H_
#define _RTXXMLQNAME_H_

#include "rtxsrc/rtxContext.h"

/**
 * This version of QName contains complete namespace info (prefix + URI)
 */
typedef struct {
   const OSUTF8CHAR* nsURI;
   const OSUTF8CHAR* nsPrefix;
   const OSUTF8CHAR* localName;
} OSXMLFullQName;

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function creates a new full QName structure given the parts.
 * Memory is allocated for the structure using rtxMemAlloc.  Copies
 * are not made of the string variables - the pointers are stored.
 *
 * @param pctxt         Pointer to a context structure.
 * @param localName     Element local name.
 * @param prefix        Namespace prefix.
 * @param nsuri         Namespace URI.
 * @return              QName value.  Memory for the value will have been
 *                        allocated by rtxMemAlloc and thus must be freed
 *                        using one of the rtxMemFree functions.  The
 *                        value will be NULL if no dynamic memory was
 *                        available.
 */
EXTERNRT OSXMLFullQName* rtxNewFullQName
(OSCTXT* pctxt, const OSUTF8CHAR* localName, const OSUTF8CHAR* prefix,
 const OSUTF8CHAR* nsuri);

/**
 * This function allocates a new QName instance and makes a deep copy
 * of the given QName including the strings inside.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pqname        Pointer to QName to be copied.
 * @return              Deep copy of QName structure.
 */
EXTERNRT OSXMLFullQName* rtxNewFullQNameDeepCopy
(OSCTXT* pctxt, const OSXMLFullQName* pqname);

/**
 * This function makes a deep copy of the given QName including the
 * strings inside.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pdest         Pointer to QName to receive copied data.
 * @param psrc          Pointer to QName to be copied.
 */
EXTERNRT void rtxQNameDeepCopy
(OSCTXT* pctxt, OSXMLFullQName* pdest, const OSXMLFullQName* psrc);

/**
 * This function frees all memory within a QName structure,
 *
 * @param pctxt         Pointer to a context structure.
 * @param pqname        Pointer to QName in which memory will be freed.
 * @param dynamic       Boolean indicating if pqname is dynamic.  If true,
 *                        the memory for pqname is freed.
 */
EXTERNRT void rtxQNameFreeMem
(OSCTXT* pctxt, OSXMLFullQName* pqname, OSBOOL dynamic);

/**
 * This function computes a hash code for the given QName.
 *
 * @param pqname        Pointer to QName structure.
 * @return              Computed hash code.
 */
EXTERNRT OSUINT32 rtxQNameHash (const OSXMLFullQName* pqname);

/**
 * This function tests 2 QNames for equality.
 *
 * @param pqname1       Pointer to QName structure.
 * @param pqname2       Pointer to QName structure.
 * @return              True if names equal; false, otherwise.
 */
EXTERNRT OSBOOL rtxQNamesEqual
(const OSXMLFullQName* pqname1, const OSXMLFullQName* pqname2);

/**
 * This function returns the QName in the following stringified
 * format: {uri}/localName
 *
 * @param pqname        Pointer to QName structure.
 * @param buffer        Buffer into which to return name.
 * @param bufsiz        Size of buffer into which name is to be returned.
 *                        If name will not fit in buffer, it is truncated.
 * @return              Pointer to string (address of 'buffer' argument).
 */
EXTERNRT const OSUTF8CHAR* rtxQNameToString
(const OSXMLFullQName* pqname, OSUTF8CHAR* buffer, OSUINT32 bufsiz);

#ifdef __cplusplus
}
#endif

#endif
