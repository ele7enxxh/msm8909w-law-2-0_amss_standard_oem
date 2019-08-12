/*
 * Copyright (c) 2013-2013 Objective Systems, Inc.
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
 * @file rtxBitDecode.h
 * Bit decode functions.
 */
#ifndef _RTXBITDECODE_H_
#define _RTXBITDECODE_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/*QC_MOD*/
#define DEC_BIT_MACRO(pctxt,pvalue) \
(((pctxt)->buffer.byteIndex >= (pctxt)->buffer.size) ? \
   rtxDecBit (pctxt, pvalue) \
   : \
   (*(pvalue) = (pctxt)->buffer.data[(pctxt)->buffer.byteIndex], \
   --(pctxt)->buffer.bitOffset == 0 ? \
      ((pctxt)->buffer.bitOffset = 8, (pctxt)->buffer.byteIndex++) \
      : \
      (*(pvalue) = (OSBOOL)((*(pvalue)) >> (pctxt)->buffer.bitOffset)), \
   *(pvalue) &= 1, \
   0))


EXTERNRT int DEC_BIT(OSCTXT*  pctxt, OSBOOL* pvalue);		/*QC_MOD*/
EXTERNRT int rtxMoveBitCursor (OSCTXT* pctxt, int bitOffset);

/**
 * This function will decode a single bit
 * and return the result in an OSBOOL value.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pvalue       A pointer to the BOOLEAN value to receive the decoded
 *                       result.  A null pointer value may be passed to
 *                       skip the bit.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxDecBit (OSCTXT* pctxt, OSBOOL* pvalue);

/**
 *  This function decodes up to sizeof(unsigned) bits
 *  and returns the result in an unsigned integer value.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pvalue      Pointer to value to be receive decoded result.
 * @param nbits       Number of bits to read from decode buffer.
 * @return            Status of the operation.  Zero if successful; a
 *                       negative status code if failed.
 */
EXTERNRT int rtxDecBits (OSCTXT* pctxt, OSUINT32* pvalue, OSSIZE nbits);

/**
 * This function decodes bits and returns the result in a byte
 * (octet) value.  Bits are shifted to the right in the decode byte
 * to remove unused bits.  For example, if a single bit is decoded from
 * octet 0x80, the decoded byte value will be 1.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pvalue      Pointer to byte value to receive decoded data.
 * @param nbits       Number of bits to read from decode buffer.  The
 *                       maximum that can be read is eight.
 * @return            Status of the operation.  Zero if successful; a
 *                       negative status code if failed.
 */
EXTERNRT int rtxDecBitsToByte
(OSCTXT* pctxt, OSUINT8* pvalue, OSUINT8 nbits);

/**
 * This function decodes bits and returns the result in an unsigned
 * 16-bit (short) value.  Bits are shifted to the right in the decode byte
 * to remove unused bits.  For example, if a single bit is decoded from
 * octet 0x80, the decoded byte value will be 1.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pvalue      Pointer to byte value to receive decoded data.
 * @param nbits       Number of bits to read from decode buffer.  The
 *                       maximum that can be read is sixteen.
 * @return            Status of the operation.  Zero if successful; a
 *                       negative status code if failed.
 */
EXTERNRT int rtxDecBitsToUInt16
(OSCTXT* pctxt, OSUINT16* pvalue, OSUINT8 nbits);

/**
 * This function decodes bits and returns the result in an octet array.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pbuffer     Address of buffer to receive decoded binary data.
 * @param bufsiz      Size of output buffer.
 * @param nbits       Number of bits to read from decode buffer.
 * @return            Status of the operation.  Zero if successful; a
 *                       negative status code if failed.
 */
EXTERNRT int rtxDecBitsToByteArray
(OSCTXT* pctxt, OSOCTET* pbuffer, OSSIZE bufsiz, OSSIZE nbits);

/**
 * This function decodes the bit at the current position and the resets
 * the bit cursor back to the original position.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pvalue       A pointer to the BOOLEAN value to receive the decoded
 *                       result.  A null pointer value may be passed to
 *                       skip the bit.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxPeekBit (OSCTXT* pctxt, OSBOOL* pvalue);

/**
 *  This function skips the given number of bits.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param nbits       Number of bits to skip.
 * @return            Status of the operation.  Zero if successful; a
 *                       negative status code if failed.
 */
EXTERNRT int rtxSkipBits (OSCTXT* pctxt, OSSIZE nbits);

#ifdef __cplusplus
}
#endif

#endif
