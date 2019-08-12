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
 * @file rtxBase64.h
 */
#ifndef _RTXBASE64_H_
#define _RTXBASE64_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode binary data into base64 string form to a dynamic buffer.
 *
 * @param pctxt        Pointer to context structure.
 * @param pSrcData     Pointer to binary data to encode.
 * @param srcDataSize  Length of the binary data in octets.
 * @param ppDstData    Pointer to pointer variable to hold address of
 *                       dynamically allocated buffer the encoded base64
 *                       string.
 * @return             Completion status of operation:
 *                       - number of binary bytes written
 *                       - negative return value is error.
 */
EXTERNRT long rtxBase64EncodeData (OSCTXT* pctxt, const char* pSrcData,
                                    size_t srcDataSize, OSOCTET** ppDstData);

/**
 * Decode base64 string to binary form into a dynamic buffer.
 *
 * @param pctxt        Pointer to context structure.
 * @param pSrcData     Pointer to base64 string to decode.
 * @param srcDataSize  Length of the base64 string.
 * @param ppDstData    Pointer to pointer variable to hold address of
 *                       dynamically allocated buffer to hold data.
 * @return             Completion status of operation:
 *                       - number of binary bytes written
 *                       - negative return value is error.
 */
EXTERNRT long rtxBase64DecodeData
(OSCTXT* pctxt, const char* pSrcData, size_t srcDataSize, OSOCTET** ppDstData);

/**
 * Decode base64 string to binary form into a fixed-size buffer.
 *
 * @param pctxt        Pointer to context structure.
 * @param pSrcData     Pointer to base64 string to decode.
 * @param srcDataSize  Length of the base64 string.
 * @param buf          Address of buffer to receive decoded binary data.
 * @param bufsiz       Size of output buffer.
 * @return             Completion status of operation:
 *                       - number of binary bytes written
 *                       - negative return value is error.
 */
EXTERNRT long rtxBase64DecodeDataToFSB
(OSCTXT* pctxt, const char* pSrcData, size_t srcDataSize,
 OSOCTET* buf, size_t bufsiz);

/**
 * Calculate number of byte required to hold a decoded base64 string
 * in binary form.
 *
 * @param pSrcData     Pointer to base64 string to decode.
 * @param srcDataSize  Length of the base64 string.
 * @return             Completion status of operation:
 *                       If success, positive value is number of bytes,
 *                       If failure, negative status code.
 */
EXTERNRT long rtxBase64GetBinDataLen
(const char* pSrcData, size_t srcDataSize);

#ifdef __cplusplus
}
#endif

#endif /* RTXBASE64 */

