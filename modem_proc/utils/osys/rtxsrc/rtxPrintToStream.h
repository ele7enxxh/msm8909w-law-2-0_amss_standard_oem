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
 * @file rtxPrintToStream.h
 */
#ifndef _RTXPRINTTOSTREAM_H_
#define _RTXPRINTTOSTREAM_H_

#ifndef _NO_STREAM

#include <stdio.h>
#include "rtxsrc/rtxContext.h"

#define OSRTINDENTSPACES        3       /* number of spaces for indent  */

#ifdef __cplusplus
extern "C" {
#endif

/* Run-time print utility functions */

/**
 * @defgroup prtToStrm Print-To-Stream Functions
 * @{
 *
 * These functions print typed data in a "name=value" format. The output
 * is redirected to the print stream defined within the context or to a
 * global print stream.  Print streams are set using the rtxSetPrintStream
 * or rtxSetGlobalPrintStream function.
 */
/**
 * Prints a boolean value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        Boolean value to print.
 */
EXTERNRT void rtxPrintToStreamBoolean
   (OSCTXT* pctxt, const char* name, OSBOOL value);

/**
 * Prints a date value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         Name of the variable to print.
 * @param pvalue       Pointer to a structure that holds numeric
 *                        DateTime value to print.
 */
EXTERNRT void rtxPrintToStreamDate
   (OSCTXT* pctxt, const char* name, const OSNumDateTime *pvalue);

/**
 * Prints a time value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         Name of the variable to print.
 * @param pvalue       Pointer to a structure that holds numeric
 *                        DateTime value to print.
 */
EXTERNRT void rtxPrintToStreamTime
   (OSCTXT* pctxt, const char* name, const OSNumDateTime *pvalue);

/**
 * Prints a dateTime value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         Name of the variable to print.
 * @param pvalue       Pointer to a structure that holds numeric
 *                        DateTime value to print.
 */
EXTERNRT void rtxPrintToStreamDateTime
   (OSCTXT* pctxt, const char* name, const OSNumDateTime *pvalue);

EXTERNRT void rtxPrintToStreamGYear
   (OSCTXT* pctxt, const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintToStreamGYearMonth
   (OSCTXT* pctxt, const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintToStreamGMonth
   (OSCTXT* pctxt, const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintToStreamGMonthDay
   (OSCTXT* pctxt, const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintToStreamGDay
   (OSCTXT* pctxt, const char* name, const OSNumDateTime* pvalue);

/**
 * Prints an integer value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        Integer value to print.
 */
EXTERNRT void rtxPrintToStreamInteger
   (OSCTXT* pctxt, const char* name, OSINT32 value);

/**
 * Prints a 64-bit integer value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        64-bit integer value to print.
 */
EXTERNRT void rtxPrintToStreamInt64
   (OSCTXT* pctxt, const char* name, OSINT64 value);

/**
 * Prints an unsigned integer value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        Unsigned integer value to print.
 */
EXTERNRT void rtxPrintToStreamUnsigned
   (OSCTXT* pctxt, const char* name, OSUINT32 value);

/**
 * Prints an unsigned 64-bit integer value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        Unsigned 64-bit integer value to print.
 */
EXTERNRT void rtxPrintToStreamUInt64
   (OSCTXT* pctxt, const char* name, OSUINT64 value);

/**
 * This function prints the value of a binary string in hex format
 * to standard output.  If the string is 32 bytes or less, it is printed
 * on a single line with a '0x' prefix.  If longer, a formatted hex dump
 * showing both hex and ascii codes is done.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 */
EXTERNRT void rtxPrintToStreamHexStr
   (OSCTXT* pctxt, const char* name, size_t numocts, const OSOCTET* data);

/**
 * Prints an octet string value in hex binary format to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 */
EXTERNRT void rtxPrintToStreamHexBinary
   (OSCTXT* pctxt, const char* name, size_t numocts, const OSOCTET* data);

/**
 * Prints an ASCII character string value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param cstring      A pointer to the character string to be printed.
 */
EXTERNRT void rtxPrintToStreamCharStr
   (OSCTXT* pctxt, const char* name, const char* cstring);

/**
 * Prints a UTF-8 encoded character string value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param cstring      A pointer to the character string to be printed.
 */
EXTERNRT void rtxPrintToStreamUTF8CharStr
   (OSCTXT* pctxt, const char* name, const OSUTF8CHAR* cstring);

/**
 * This function prints a Unicode string to standard output.  Characters
 * in the string that are within the normal Ascii range are printed as
 * single characters.  Characters outside the Ascii range are printed
 * as 4-byte hex codes (0xnnnn).
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param str          Pointer to unicode sring to be printed.  String is
 *                       an array of C unsigned short data variables.
 * @param nchars       Number of characters in the string.  If value is
 *                       negative, string is assumed to be null-terminated
 *                       (i.e. ends with a 0x0000 character).
 */
EXTERNRT void rtxPrintToStreamUnicodeCharStr
   (OSCTXT* pctxt, const char* name, const OSUNICHAR* str, int nchars);

/**
 * Prints a REAL (float, double, decimal) value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        REAL value to print.
 */
EXTERNRT void rtxPrintToStreamReal
   (OSCTXT* pctxt, const char* name, OSREAL value);

/**
 * Prints a NULL value to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 */
EXTERNRT void rtxPrintToStreamNull (OSCTXT* pctxt, const char* name);

/**
 * Prints a name-value pair to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param name         The name of the variable to print.
 * @param value        A pointer to name-value pair structure to print.
 */
EXTERNRT void rtxPrintToStreamNVP
   (OSCTXT* pctxt, const char* name, const OSUTF8NVP* value);

/**
 * This function prints the contents of a text file to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param filename     The name of the text file to print.
 * @return             Status of operation, 0 if success.
 */
EXTERNRT int rtxPrintToStreamFile (OSCTXT* pctxt, const char* filename);

#ifndef __SYMBIAN32__
/**
 * This function prints indentation spaces to a print stream.
 */
EXTERNRT void rtxPrintToStreamIndent (OSCTXT* pctxt);

/**
 * This function increments the current indentation level.
 *
 * @param pctxt         A pointer to a context data structure that holds the
 *                      print stream.
 */
EXTERNRT void rtxPrintToStreamIncrIndent (OSCTXT* pctxt);

/**
 * This function decrements the current indentation level.
 *
 * @param pctxt         A pointer to a context data structure that holds the
 *                      print stream.
 */
EXTERNRT void rtxPrintToStreamDecrIndent (OSCTXT* pctxt);

/**
 * This function closes a braced region by decreasing the indent level,
 * printing indent spaces, and printing the closing brace.
 */
EXTERNRT void rtxPrintToStreamCloseBrace (OSCTXT* pctxt);

/**
 * This function opens a braced region by printing indent spaces,
 * printing the name and opening brace, and increasing the indent level.
 */
EXTERNRT void rtxPrintToStreamOpenBrace (OSCTXT* pctxt, const char*);

#endif

/**
 * This function outputs a hexadecimal dump of the current buffer contents
 * to a print stream.
 *
 * @param pctxt        A pointer to a context structure.
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed
 */
EXTERNRT void rtxHexDumpToStream
   (OSCTXT* pctxt, const OSOCTET* data, size_t numocts);

/**
 * This function outputs a hexadecimal dump of the current buffer to a
 * print stream, but it may output the dump as an array of bytes, words,
 * or double words.
 *
 * @param pctxt        A pointer to a context structure.
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed.
 * @param bytesPerUnit The number of bytes in one unit. May be 1 (byte), 2
 *                       (word), or 4 (double word).
 */
EXTERNRT void rtxHexDumpToStreamEx
   (OSCTXT* pctxt, const OSOCTET* data, size_t numocts, int bytesPerUnit);

/**
 * @} prtToStrm
 */
#ifdef __cplusplus
}
#endif

#endif /* _NO_STREAM */
#endif /* _RTXPRINTTOSTREAM_H_ */
