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
 * @file rtxPrint.h
 */
#ifndef _RTXPRINT_H_
#define _RTXPRINT_H_

#include <stdio.h>
#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxDList.h"

#define OSRTINDENTSPACES        3       /* number of spaces for indent  */

#ifdef __cplusplus
extern "C" {
#endif

/* Run-time print utility functions */

/**
 * @defgroup valsToStdout Print Functions
 * @{
 *
 * These functions simply print the output in a "name=value" format. The value
 * format is obtained by calling one of the ToString functions with the given
 * value.
 */
/**
 * This function converts a byte value into its hex string equivalent.
 *
 * @param byte          Byte to format.
 * @param buf           Output buffer.
 * @param bufsize       Output buffer size.
 */
EXTERNRT int rtxByteToHexChar (OSOCTET byte, char* buf, OSSIZE bufsize);

/**
 * Prints a boolean value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        Boolean value to print.
 */
EXTERNRT void rtxPrintBoolean (const char* name, OSBOOL value);

/**
 * Prints a date value to stdout.
 *
 * @param name         Name of the variable to print.
 * @param pvalue       Pointer to a structure that holds numeric
 *                        DateTime value to print.
 */
EXTERNRT void rtxPrintDate
   (const char* name, const OSNumDateTime *pvalue);

/**
 * Prints a time value to stdout.
 *
 * @param name         Name of the variable to print.
 * @param pvalue       Pointer to a structure that holds numeric
 *                        DateTime value to print.
 */
EXTERNRT void rtxPrintTime
   (const char* name, const OSNumDateTime *pvalue);

/**
 * Prints a dateTime value to stdout.
 *
 * @param name         Name of the variable to print.
 * @param pvalue       Pointer to a structure that holds numeric
 *                        DateTime value to print.
 */
EXTERNRT void rtxPrintDateTime
   (const char* name, const OSNumDateTime *pvalue);

EXTERNRT void rtxPrintGYear
   (const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintGYearMonth
   (const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintGMonth
   (const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintGMonthDay
   (const char* name, const OSNumDateTime* pvalue);

EXTERNRT void rtxPrintGDay
   (const char* name, const OSNumDateTime* pvalue);

/**
 * Prints an integer value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        Integer value to print.
 */
EXTERNRT void rtxPrintInteger (const char* name, OSINT32 value);

/**
 * Prints a 64-bit integer value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        64-bit integer value to print.
 */
EXTERNRT void rtxPrintInt64 (const char* name, OSINT64 value);

/**
 * Prints an unsigned integer value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        Unsigned integer value to print.
 */
EXTERNRT void rtxPrintUnsigned (const char* name, OSUINT32 value);

/**
 * Prints an unsigned 64-bit integer value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        Unsigned 64-bit integer value to print.
 */
EXTERNRT void rtxPrintUInt64 (const char* name, OSUINT64 value);

/**
 * This function prints the value of a binary string in hex format
 * to standard output.  If the string is 32 bytes or less, it is printed
 * on a single line with a '0x' prefix.  If longer, a formatted hex dump
 * showing both hex and ascii codes is done.
 *
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 */
EXTERNRT void rtxPrintHexStr
(const char* name, OSSIZE numocts, const OSOCTET* data);

/**
 * Prints an octet string value in hex binary format to stdout.
 *
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 */
EXTERNRT void rtxPrintHexBinary
(const char* name, OSSIZE numocts, const OSOCTET* data);

/**
 * Prints an ASCII character string value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param cstring      A pointer to the character string to be printed.
 */
EXTERNRT void rtxPrintCharStr (const char* name, const char* cstring);

/**
 * Prints a UTF-8 encoded character string value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param cstring      A pointer to the character string to be printed.
 */
EXTERNRT void rtxPrintUTF8CharStr
(const char* name, const OSUTF8CHAR* cstring);

/**
 * This function prints a Unicode string to standard output.  Characters
 * in the string that are within the normal Ascii range are printed as
 * single characters.  Characters outside the Ascii range are printed
 * as 4-byte hex codes (0xnnnn).
 *
 * @param name         The name of the variable to print.
 * @param str          Pointer to unicode sring to be printed.  String is
 *                       an array of C unsigned short data variables.
 * @param nchars       Number of characters in the string.  If value is
 *                       negative, string is assumed to be null-terminated
 *                       (i.e. ends with a 0x0000 character).
 */
EXTERNRT void rtxPrintUnicodeCharStr
(const char* name, const OSUNICHAR* str, int nchars);

/**
 * Prints a REAL (float, double, decimal) value to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        REAL value to print.
 */
EXTERNRT void rtxPrintReal (const char* name, OSREAL value);

/**
 * Prints a NULL value to stdout.
 *
 * @param name         The name of the variable to print.
 */
EXTERNRT void rtxPrintNull (const char* name);

/**
 * Prints a name-value pair to stdout.
 *
 * @param name         The name of the variable to print.
 * @param value        A pointer to name-value pair structure to print.
 */
EXTERNRT void rtxPrintNVP (const char* name, const OSUTF8NVP* value);

/**
 * This function prints the contents of a text file to stdout.
 *
 * @param filename     The name of the text file to print.
 * @return             Status of operation, 0 if success.
 */
EXTERNRT int rtxPrintFile (const char* filename);

#ifndef __SYMBIAN32__

/**
 * This function prints indentation spaces to stdout.
 */
EXTERNRT void rtxPrintIndent (void);

/**
 * This function increments the current indentation level.
 */
EXTERNRT void rtxPrintIncrIndent (void);

/**
 * This function decrements the current indentation level.
 */
EXTERNRT void rtxPrintDecrIndent (void);

/**
 * This function closes a braced region by decreasing the indent level,
 * printing indent spaces, and printing the closing brace.
 */
EXTERNRT void rtxPrintCloseBrace (void);

/**
 * This function opens a braced region by printing indent spaces,
 * printing the name and opening brace, and increasing the indent level.
 */
EXTERNRT void rtxPrintOpenBrace (const char*);

#endif

/**
 * This function outputs a hexadecimal dump of the current buffer contents to
 * the file with the given name.  The file is opened or created and then
 * closed after the writer operation is complete.
 *
 * @param filename     Full path to file to which data should be output.
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed
 */
EXTERNRT void rtxHexDumpToNamedFile
(const char* filename, const OSOCTET* data, OSSIZE numocts);

/**
 * This function outputs a hexadecimal dump of the current buffer contents to a
 * file.
 *
 * @param fp           A pointer to FILE structure. The file should be opened
 *                       for writing.
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed
 */
EXTERNRT void rtxHexDumpToFile
(FILE* fp, const OSOCTET* data, OSSIZE numocts);

/**
 * This function outputs a hexadecimal dump of the current buffer to a file,
 * but it may output the dump as an array of bytes, words, or double words.
 *
 * @param fp           A pointer to FILE structure. The file should be opened
 *                       for writing.
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed.
 * @param bytesPerUnit The number of bytes in one unit. May be 1 (byte), 2
 *                       (word), or 4 (double word).
 */
EXTERNRT void rtxHexDumpToFileEx
(FILE* fp, const OSOCTET* data, OSSIZE numocts, int bytesPerUnit);

/**
 * This function outputs a hexadecimal dump of the current buffer contents to
 * stdout.
 *
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed.
 */
EXTERNRT void rtxHexDump (const OSOCTET* data, OSSIZE numocts);

/**
 * This function outputs a hexadecimal dump of the current buffer contents to
 * stdout, but it may display the dump as an array or bytes, words, or double
 * words.
 *
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed.
 * @param bytesPerUnit The number of bytes in one unit. May be 1 (byte), 2
 *                       (word), or 4 (double word).
 */
EXTERNRT void rtxHexDumpEx
(const OSOCTET* data, OSSIZE numocts, int bytesPerUnit);

/**
 * This function formats a hexadecimal dump of the current buffer contents to a
 * string.
 *
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed.
 * @param buffer       The destination string buffer.
 * @param bufferIndex  The starting position in the destination buffer. The
 *                       formatting of the dump will begin at this position.
 * @param bufferSize   The total size of the destination buffer.
 * @return             The length of the final string.
 */
EXTERNRT int rtxHexDumpToString
(const OSOCTET* data, OSSIZE numocts, char* buffer, OSSIZE bufferIndex,
 OSSIZE bufferSize);

/**
 * This function formats a hexadecimal dump of the current buffer contents to a
 * string, but it may output the dump as an array of bytes, words, or double
 * words.
 *
 * @param data         The pointer to a buffer to be displayed.
 * @param numocts      The number of octets to be displayed.
 * @param buffer       The destination string buffer.
 * @param bufferIndex  The starting position in the destination buffer. The
 *                       formatting of the dump will begin at this position.
 * @param bufferSize   The total size of the destination buffer.
 * @param bytesPerUnit The number of bytes in one unit. May be 1 (byte), 2
 *                       (word), or 4 (double word).
 * @return             The length of the final string.
 */
EXTERNRT int rtxHexDumpToStringEx
(const OSOCTET* data, OSSIZE numocts, char* buffer,
 OSSIZE bufferIndex, OSSIZE bufferSize, int bytesPerUnit);

/**
 * This function outputs a hexadecimal dump of the contents of the named
 * file to stdout.
 *
 * @param inFilePath   Name of file to be dumped.
 */
EXTERNRT void rtxHexDumpFileContents (const char* inFilePath);

/**
 * This function outputs a hexadecimal dump of the contents of the named
 * file to a text file.
 *
 * @param inFilePath   Name of file to be dumped.
 * @param outFilePath  Name of file to which dump contents will be written.
 */
EXTERNRT void rtxHexDumpFileContentsToFile
(const char* inFilePath, const char* outFilePath);

/**
 * @} valsToStdout
 */
#ifdef __cplusplus
}
#endif

#endif
