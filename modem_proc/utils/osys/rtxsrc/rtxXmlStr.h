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
 * @file rtxXmlStr.h
 */
#ifndef _RTXXMLSTR_H_
#define _RTXXMLSTR_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function creates an instance of XML UTF-8 character string structure
 * (OSXMLSTRING type) and initializes it by the passed values.
 * This function uses \c rtxMemAlloc to allocate the memory for the
 * OSXMLSTRING structure. String \c pStr is not copied: the pointer will be
 * assigned to "value" member of OSXMLSTRING structure.
 * To free memory, \c rtxMemFreePtr function may be used:
 *
 * <p><i>OSXMLSTRING* pStr = rtxCreateXmlStr (....);</i></p>
 * <p><i>....</i></p>
 * <p><i>rtxMemFreePtr (pctxt, pStr);</i></p>
 *
 * <p>Note, user is responsible to free <i>pStr->value</i> if it is not static
 * and was allocated dynamically.
 *
 * @param pctxt       Pointer to a context block
 * @param pStr        Pointer to a character string to be assigned.
 * @param cdata       This indicates if this string should be encoded as a
 *                      CDATA section in an XML document.
 * @return            The allocated and initialized instance of OSXMLSTRING
 *                      type.
 */
EXTERNRT OSXMLSTRING* rtxCreateXmlStr
   (OSCTXT* pctxt, const OSUTF8CHAR* pStr, OSBOOL cdata);

/**
 * This function creates an instance of XML UTF-8 character string structure
 * (OSXMLSTRING type) and initializes it by the passed values. In contrary to
 * \c rtxCreateXmlStr function, the string value is copied.
 * This function uses \c rtxMemAlloc to allocate the memory for the
 * OSXMLSTRING structure and for the string value being copied.
 * To free memory, \c rtxMemFreePtr function may be used for both value and
 * structure itself:
 *
 * <p><i>OSXMLSTRING* pStr = rtxCreateCopyXmlStr (....);</i></p>
 * <p><i>....</i></p>
 * <p><i>rtxMemFreePtr (pctxt, pStr->value);</i></p>
 * <p><i>rtxMemFreePtr (pctxt, pStr);</i></p>
 *
 * @param pctxt       Pointer to a context block
 * @param pStr        Pointer to a character string to be copied.
 * @param cdata       This indicates if this string should be encoded as a
 *                      CDATA section in an XML document.
 * @return            The allocated and initialized instance of OSXMLSTRING
 *                      type.
 */
EXTERNRT OSXMLSTRING* rtxCreateCopyXmlStr
   (OSCTXT* pctxt, const OSUTF8CHAR* pStr, OSBOOL cdata);

#ifdef __cplusplus
}
#endif

#endif /*_RTXXMLSTR_H_*/
