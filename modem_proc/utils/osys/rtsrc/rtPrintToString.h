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
 * @file rtsrc/rtPrintToString.h
 */
#ifndef _RTPRINTTOSTRING_H_
#define _RTPRINTTOSTRING_H_
#include <stdio.h>
#include "rtsrc/asn1type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtPrintToString Print values to text buffer functions.
 * @ingroup cruntime
 * Format the output value to string in a "name = value" format. The value
 * format is obtained by calling one of the "ToString" functions with the given
 * value.
 *
 * @{
 */
/**
 * @param name         The name of the variable to print.
 * @param value        ASN.1 value to print (Note: multiple arguments may be
 *                       used to represent the value- for example a bit string
 *                       would be represented by a numbits and data argument.
 *                       See the function prototype forthe exact calling
 *                       sequence).
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringBoolean (const char* name, OSBOOL value,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param value        ASN.1 value to print (Note: multiple arguments may be
 *                       used to represent the value- for example a bit string
 *                       would be represented by a numbits and data argument.
 *                       See the function prototype forthe exact calling
 *                       sequence).
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringInteger (const char* name, OSINT32 value,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param value        ASN.1 value to print (Note: multiple arguments may be
 *                       used to represent the value- for example a bit string
 *                       would be represented by a numbits and data argument.
 *                       See the function prototype forthe exact calling
 *                       sequence).
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringInt64 (const char* name, OSINT64 value,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param value        ASN.1 value to print (Note: multiple arguments may be
 *                       used to represent the value- for example a bit string
 *                       would be represented by a numbits and data argument.
 *                       See the function prototype for the exact calling
 *                       sequence).
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringUnsigned (const char* name, OSUINT32 value,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param value        ASN.1 value to print (Note: multiple arguments may be
 *                       used to represent the value- for example a bit string
 *                       would be represented by a numbits and data argument.
 *                       See the function prototype for the exact calling
 *                       sequence).
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringUInt64 (const char* name, OSUINT64 value,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param numbits      The number of bits to be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringBitStr (const char* name,
   OSSIZE numbits, const OSOCTET* data, const char* conn,
   char* buffer, OSSIZE bufferSize);

/**
 * This function prints the value of a bit string to a text buffer in
 * brace text format.
 *
 * @param name         The name of the variable to print.
 * @param numbits      The number of bits to be printed.
 * @param data         A pointer to the data to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringBitStrBraceText
(const char* name, OSSIZE numbits, const OSOCTET* data,
 char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOctStr (const char* name,
   OSSIZE numocts, const OSOCTET* data, const char* conn,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param cstring      A pointer to the character string to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringCharStr (const char* name,
   const char* cstring, char* buffer, OSSIZE bufferSize);

/**
 * @param name          The name of the variable to print.
 * @param ustring       A pointer to the UTF-8 string to be printed.
 * @param bufferSize    The size of the buffer to receive the printed value.
 * @param buffer        A pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringUTF8Str (const char *name,
      const OSUTF8CHAR *ustring, char *buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to a 16-bit string to print.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToString16BitCharStr (const char* name,
   const Asn116BitCharString* bstring, const char* conn,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to a 32-bit string to print.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToString32BitCharStr (const char* name,
   const Asn132BitCharString* bstring, const char* conn,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param value        ASN.1 value to print (Note: multiple arguments may be
 *                       used to represent the value- for example a bit string
 *                       would be represented by a numbits and data argument.
 *                       See the function prototype forthe exact calling
 *                       sequence).
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringReal (const char* name, OSREAL value,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param pOID         A pointer to a OID to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOID (const char* name, const ASN1OBJID* pOID,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param pOID         A pointer to a OID to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOID64 (const char* name, const ASN1OID64* pOID,
   char* buffer, OSSIZE bufferSize);

/**
 * @param bufferIndex  The index to the buffer.
 * @param pOID         A pointer to a OID to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOIDValue (const ASN1OBJID* pOID,
   char* buffer, OSSIZE bufferSize);

/**
 * @param bufferIndex  The index to the buffer.
 * @param pOID         A pointer to a OID to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOID64Value (const ASN1OID64* pOID,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOpenType (const char* name,
   OSSIZE numocts, const OSOCTET* data, const char* conn,
   char* buffer, OSSIZE bufferSize);

/**
 * @param name         The name of the variable to print.
 * @param pElemList    A pointer to the element to be printed.
 * @param bufferSize   The size of the buffer to receive the printed value.
 * @param buffer       Pointer to a buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOpenTypeExt (const char* name,
   const OSRTDList* pElemList, char* buffer, OSSIZE bufferSize);

/**
 * @param namebuf      A pointer to the buffer name.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToString
   (const char* namebuf, char* buffer, OSSIZE bufSize);

/**
 * This function prints the value of a binary string in hex format
 * to string buffer.  If the string is 32 bytes or less, it is printed
 * on a single line with a '0x' prefix.  If longer, a formatted hex dump
 * showing both hex and ascii codes is done.
 *
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringHexStr
(const char* name, OSSIZE numocts, const OSOCTET* data, char* buffer,
 OSSIZE bufSize);

/**
 * This function prints a Unicode string to string buffer. Characters
 * in the string that are within the normal Ascii range are printed as
 * single characters.  Characters outside the Ascii range are printed
 * as 4-byte hex codes (0xnnnn).
 *
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringUnicodeCharStr
(const char* name, const Asn116BitCharString* bstring, char* buffer,
 OSSIZE bufSize);

/**
 * This function prints a Universal string to string buffer. Characters
 * in the string that are within the normal Ascii range are printed as
 * single characters.  Characters outside the Ascii range are printed
 * as 8-byte hex codes (0xnnnnnnnn).
 *
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringUnivCharStr (const char* name,
   const Asn132BitCharString* bstring, char* buffer, OSSIZE bufSize);

#ifndef __SYMBIAN32__
/**
 * This function prints the value of an open type extension in brace
 * text format to buffer.
 *
 * @param name         The name of the variable to print.
 * @param pElemList    A pointer to an element of a list.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOpenTypeExtBraceText
(const char* name, const OSRTDList* pElemList, char* buffer, OSSIZE bufSize);

/**
 * This function prints indentation spaces to buffer.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringIndent (char* buffer, OSSIZE bufSize);

/**
 * This function increments the current indentation level.
 */
EXTERNRT void rtPrintToStringIncrIndent(void);

/**
 * This function decrements the current indentation level.
 */
EXTERNRT void rtPrintToStringDecrIndent (void);

/**
 * This function closes a braced region by decreasing the indent level,
 * printing indent spaces, and printing the closing brace.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringCloseBrace (char* buffer, OSSIZE bufSize);

/**
 * This function opens a braced region by printing indent spaces,
 * printing the name and opening brace, and increasing the indent level.
 * @param buffer       Pointer to a buffer to receive the printed value.
 * @param bufSize      The size of the buffer to receive the printed value.
 */
EXTERNRT int rtPrintToStringOpenBrace
(const char*, char* buffer, OSSIZE bufSize);

#endif

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif
