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
/**
 * @file rtconv.h
 * ASN.1 type to string conversion functions.
 */
#ifndef _RTCONV_H_
#define _RTCONV_H_

#include "rtsrc/asn1type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup convfunc ASN.1 type to string conversion functions.
 * @{
 * This group of functions allows raw ASN.1 data fields to be converted
 * to a stringified representation.
 */
/**
 * This function converts an ASN.1 bit string to a string. The output is an
 * ASN.1 value notation for a binary string (for example, '10010'B).
 *
 * @param numbits      The number of bits in the data argument to format.
 * @param data         The buffer containing the bit string to be formatted
 *                       (note : in the case of BER/DER, this refers to the
 *                       actual point in the string where the data starts, not
 *                       where the contents field starts. The contents field
 *                       contains an extra byte at the beginning that specifies
 *                       the number of unused bits in the last byte).
 * @param buffer       A pointer to a buffer to receive the stringified value.
 * @param bufsiz       The size of the buffer to receive the stringified value.
 */
EXTERNRT const char* rtBitStrToString (OSSIZE numbits,
                                       const OSOCTET* data,
                                       char* buffer, OSSIZE bufsiz);

/**
 * This function converts an ASN.1 Boolean value to a string. The string value
 * is one of the keywords TRUE or FALSE.
 *
 * @param value        The value to convert.
 * @return             The converted value. This will be a string literal set
 *                       to either "TRUE" or "FALSE".
 */
EXTERNRT const char* rtBoolToString (OSBOOL value);

/**
 * This function converts an ASN.1 integer value to a string.
 *
 * @param value        The value to convert.
 * @param bufsiz       The size of the buffer to receive the stringified value.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtIntToString
   (OSINT32 value, char* buffer, OSSIZE bufsiz);

/**
 * This function converts a 64-bit ASN.1 integer value to a string.
 *
 * @param value        The value to convert.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @param bufsiz       The size f the buffer to receive the stringified value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtInt64ToString
   (OSINT64 value, char* buffer, OSSIZE bufsiz);

/**
 * This function converts an ASN.1 integer value to a string. In this case, the
 * ASN.1 value was represented in the C/C++ code as an unsigned integer based
 * on a constraint.
 *
 * @param value        The value to convert.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @param bufsiz       The size f the buffer to receive the stringified value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtUIntToString
   (OSUINT32 value, char* buffer, OSSIZE bufsiz);

/**
 * This function converts a 64-bit ASN.1 value to a string.
 *
 * @param value        The value to convert.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @param bufsiz       The size f the buffer to receive the stringified value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtUInt64ToString
   (OSUINT64 value, char* buffer, OSSIZE bufsiz);

/**
 * This function converts an ASN.1 OBJECT INDENTIFIER or RELATED-OID value to a
 * string. The output format is the ASN.1 value notation for an object
 * identifier (ex. { 0 1 222 333 }). All subidentifiers are shown as unsigned
 * integer numbers - no attempt is made to map the identifiers to symbolic
 * names.
 *
 * @param numids       The number of subidentifiers in the OID value.
 * @param data         The buffer containing the OID subidentifiers to be
 *                       formatted.
 * @param bufsiz       The size of the buffer to receive the stringified value.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtOIDToString
   (OSSIZE numids, OSUINT32* data, char* buffer, OSSIZE bufsiz);

/**
 * This function converts a 64-bit ASN.1 OBJECT INDENTIFIER or RELATED-OID
 * value to a string. The output format is the ASN.1 value notation for an
 * object indentifier (ex. { 0 1 222 333 }). All subidentifiers are shown as
 * unsigned integer numbers - no attempt is made to map the identifiers to
 * symbolic names.
 *
 * @param numids       The number of subidentifiers in the OID value.
 * @param data         The buffer containing the 64-bit OID subidentifiers to
 *                       be formatted.
 * @param bufsiz       The size of the buffer to receive the stringified value.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtOID64ToString
   (OSSIZE numids, OSUINT64* data, char* buffer, OSSIZE bufsiz);

/**
 * This function converts an ASN.1 octet string value to a string. The output
 * format is ASN.1 value notation for a hexadecimal string (for example,
 * '1F8A'H).
 *
 * @param numocts      The number of octets (bytes) in the data argument to
 *                       format.
 * @param data         The buffer containing the octet string to be formatted.
 * @param bufsiz       The size of the buffer to receive the stringified value.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtOctStrToString
   (OSSIZE numocts, const OSOCTET* data, char* buffer, OSSIZE bufsiz);

/**
 * This function converts an ANS.1 tag to a string. The tag is represented
 * using the ASN1C internal ASN1CTAG structure. The output is standard ASN.1
 * notation for a tag (for example, [0] = context 0 tag).
 *
 * @param tag          The tag value to be converted.
 * @param buffer       The pointer to a buffer to receive the stringified
 *                       value.
 * @param bufsiz       The size of buffer to receive the stringified value
 * @return             The converted value. This pointer will be equal to the
 *                       buffer argument that was passed in.
 */
EXTERNRT const char* rtTagToString (ASN1TAG tag, char* buffer, OSSIZE bufsiz);

/**
 * @} pfun
 */

#ifdef __cplusplus
}
#endif

#endif
