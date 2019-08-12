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
 * @file rtxTBCD.h
 * Telephony binary-decimal conversion functions.
 */
#ifndef _RTXTBCD_H_
#define _RTXTBCD_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxTBCD Telephony Binary Coded Decimal (TBCD) Helper Functions
 * @{
 *
 * Telephony Binary Coded Decimal (TBCD) helper functions provide assistance 
 * in converting TBCD strings to and from binary form as document in 
 * standard ITU-T Q.825.
 */
/**
 * This function converts a Q.825 TBCD value to a standard null-terminated
 * string. Octet values can contain the filler digit to represent the odd
 * number of BCD digits.
 *
 * The encoding is as follows per Q.825:
 *
 * This type (Telephony Binary Coded Decimal String) is used to
 * represent digits from 0 through 9, *, #, a, b, c, two digits per
 * octet, each digit encoded 0000 to 1001 (0 to 9),
 * 1010 (*) 1011(#), 1100 (a), 1101 (b) or 1110 (c); 1111 (end of
 * pulsing signal-ST); 0000 is used as a filler when there is an odd
 * number of digits.
 *
 * @param numocts      The number of octets in the BCD value.
 * @param data         The pointer to the BCD value.
 * @param buffer       The destination buffer. Should not be less than bufsiz
 *                       argument is.
 * @param bufsiz       The size of the destination buffer (in octets). The
 *                       buffer size should be at least ((numocts * 2) + 1) to
 *                       hold the BCD to String conversion.
 * @return             Status of conversion: 0 = success, negative = error.
 * @since 6.6
 */
EXTERNRT int rtxQ825TBCDToString
(OSSIZE numocts, const OSOCTET* data, char* buffer, OSSIZE bufsiz);

/**
 * This function decodes a Q.825 TBCD value to a standard null-terminated
 * string. TBCD digits are read from the decode buffer/stream and converted
 * to their character equivalents.  See 'rtQ825TBCDToString' for a description
 * of the Q.825 TBCD format.
 *
 * @param pctxt        Pointer to a context structure block.
 * @param numocts      The number of octets in the BCD value to be read from
 *                       input stream.
 * @param buffer       The destination buffer. Should not be less than bufsiz
 *                       argument is.
 * @param bufsiz       The size of the destination buffer (in octets). The
 *                       buffer size should be at least ((numocts * 2) + 1) to
 *                       hold the BCD to String conversion.
 * @return             Status of conversion: 0 = success, negative = error.
 * @since 6.6
 */
EXTERNRT int rtxDecQ825TBCDString
(OSCTXT* pctxt, OSSIZE numocts, char* buffer, OSSIZE bufsiz);

/**
 * This function encodes a null-terminated string Q.825 TBCD string.
 * TBCD digits are converted and written to the encode buffer/stream.
 * See 'rtQ825TBCDToString' for a description of the Q.825 TBCD format.
 *
 * @param pctxt        Pointer to a context structure block.
 * @param str          Null-terminate string to be encoded.  This string
 *                       may only contain valid Q.825 TBCD characters.
 * @return             Status of operation: 0 = success, negative = error.
 * @since 6.6
 */
EXTERNRT int rtxEncQ825TBCDString (OSCTXT* pctxt, const char* str);

/**
 * This function converts a TBCD binary character into its ASCII equivalent.
 *
 * @param tbcdDigit     TBCD digit
 * @param pdigit        Pointer to character to receive converted character
 * @return              0 if conversion successful, or negative error code
 * @since 6.6
 */
EXTERNRT int rtxTBCDBinToChar (OSUINT8 bcdDigit, char* pdigit);

/**
 * This function converts a TBCD character ('0'-'9',"*#abc") into its
 * binary equivalent.
 *
 * @param digit         TBCD digit character ('0'-'9',"*#abc")
 * @param pbyte         Pointer to byte to receive binary result.
 * @return              0 if conversion successful, or negative error code
 * @since 6.6
 */
EXTERNRT int rtxTBCDCharToBin (char digit, OSUINT8* pbyte);

/**
 * @} rtxTBCD
 */

#ifdef __cplusplus
}
#endif

#endif
