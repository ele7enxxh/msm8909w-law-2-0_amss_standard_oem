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
 * @file rtxUTF8.h
 * Utility functions for handling UTF-8 strings.
 */
#ifndef _RTXUTF8_H_
#define _RTXUTF8_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UTF-8 string functions */
/**
 * @defgroup ccfUTF8 UTF-8 String Functions
 * @{
 *
 * The UTF-8 string functions handle string operations on UTF-8 encoded
 * strings. This is the default character string data type used for encoded XML
 * data. UTF-8 strings are represented in C as strings of unsigned characters
 * (bytes) to cover the full range of possible single character encodings.
 */
/**
 * This function converts a UTF-8 string to a Unicode string (UTF-16). The
 * Unicode string is stored as an array of 16-bit characters (unsigned short
 * integers).
 *
 * @param pctxt        A pointer to a context structure.
 * @param inbuf        UTF-8 string to convert.
 * @param outbuf       Output buffer to receive converted Unicode data.
 * @param outbufsiz    Size of the output buffer in bytes.
 * @return             Completion status of operation:
 *                       - number of Unicode characters in the string
 *                       - negative return value is error.
 */
EXTERNRT long rtxUTF8ToUnicode (OSCTXT* pctxt, const OSUTF8CHAR* inbuf,
                                 OSUNICHAR* outbuf, size_t outbufsiz);

/**
 * This function will validate a UTF-8 encoded string to ensure that it is
 * encoded correctly.
 *
 * @param pctxt        A pointer to a context structure.
 * @param inbuf        A pointer to the null-terminated UTF-8 encoded string.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxValidateUTF8 (OSCTXT* pctxt, const OSUTF8CHAR* inbuf);

/**
 * This function will return the length (in characters) of a null-terminated
 * UTF-8 encoded string.
 *
 * @param inbuf        A pointer to the null-terminated UTF-8 encoded string.
 * @return             Number of characters in string. Note that this may be
 *                       different than the number of bytes as UTF-8 characters
 *                       can span multiple-bytes.
 */
EXTERNRT size_t rtxUTF8Len (const OSUTF8CHAR* inbuf);
EXTERNRT size_t rtxCalcUTF8Len (const OSUTF8CHAR* inbuf, size_t inbufBytes);

/**
 * This function will return the length (in bytes) of a null-terminated UTF-8
 * encoded string.
 *
 * @param inbuf        A pointer to the null-terminated UTF-8 encoded string.
 * @return             Number of bytes in the string.
 */
EXTERNRT size_t rtxUTF8LenBytes (const OSUTF8CHAR* inbuf);

/**
 * This function will return the number of bytes needed to encode the given
 * 32-bit universal character value as a UTF-8 character.
 *
 * @param wc           32-bit wide character value.
 * @return             Number of bytes needed to encode as UTF-8.
 */
EXTERNRT int rtxUTF8CharSize (OS32BITCHAR wc);

/**
 * This function will convert a wide character into an encoded UTF-8 character
 * byte string.
 *
 * @param wc           32-bit wide character value.
 * @param buf          Buffer to receive encoded UTF-8 character value.
 * @param bufsiz       Size of the buffer ot receive the encoded value.
 * @return             Number of bytes consumed to encode character or
 *                       negative status code if error.
 */
EXTERNRT int rtxUTF8EncodeChar (OS32BITCHAR wc, OSOCTET* buf, size_t bufsiz);

/**
 * This function will convert an encoded UTF-8 character byte string into
 * a wide character value.
 *
 * @param pctxt        A pointer to a context structure.
 * @param pinbuf       Pointer to UTF-8 byte sequence to be decoded.
 * @param pInsize      Number of bytes that were consumed (i.e. size of the
 *                      character).
 * @return             32-bit wide character value.
 */
EXTERNRT int rtxUTF8DecodeChar
(OSCTXT* pctxt, const OSUTF8CHAR* pinbuf, int* pInsize);

/**
 * Thia function will convert a UTF-8 encoded character value into a wide
 * character.
 *
 * @param buf          Pointer to UTF-8 character value.
 * @param len          Pointer to integer to receive decoded size (in bytes) of
 *                       the UTF-8 character value sequence.
 * @return             Converted wide character value.
 */
EXTERNRT OS32BITCHAR rtxUTF8CharToWC (const OSUTF8CHAR* buf, OSUINT32* len);

/**
 * This function finds a character in the given UTF-8 character string. It is
 * similar to the C \c strchr function.
 *
 * @param utf8str      Null-terminated UTF-8 string to be searched.
 * @param utf8char     32-bit Unicode character to find.
 * @return             Pointer to to the first occurrence of character in
 *                       string, or NULL if character is not found.
 */
EXTERNRT OSUTF8CHAR* rtxUTF8StrChr (OSUTF8CHAR* utf8str,
                                     OS32BITCHAR utf8char);

/**
 * This function creates a duplicate copy of the given UTF-8 character string.
 * It is similar to the C \c strdup function. Memory for the duplicated string
 * is allocated using the \c rtxMemAlloc function.
 *
 * @param pctxt        A pointer to a context structure.
 * @param utf8str      Null-terminated UTF-8 string to be duplicated.
 * @return             Pointer to duplicated string value.
 */
EXTERNRT OSUTF8CHAR* rtxUTF8Strdup (OSCTXT* pctxt, const OSUTF8CHAR* utf8str);

/**
 * This function creates a duplicate copy of the given UTF-8 character string.
 * It is similar to the \c rtxUTF8Strdup function except that it allows the
 * number of bytes to convert to be specified. Memory for the duplicated string
 * is allocated using the \c rtxMemAlloc function.
 *
 * @param pctxt        A pointer to a context structure.
 * @param utf8str      UTF-8 string to be duplicated.
 * @param nbytes       Number of bytes from \c utf8str to duplicate.
 * @return             Pointer to duplicated string value.
 */
EXTERNRT OSUTF8CHAR* rtxUTF8Strndup
(OSCTXT* pctxt, const OSUTF8CHAR* utf8str, size_t nbytes);

/**
 * This function check to see if the given UTF8 string pointer exists
 * on the memory heap.  If it does, its reference count is incremented;
 * otherwise, a duplicate copy is made.
 *
 * @param pctxt        A pointer to a context structure.
 * @param utf8str      Null-terminated UTF-8 string variable.
 * @return             Pointer to string value.  This will either be the
 *                       existing UTF-8 string pointer value (utf8str)
 *                       or a new value.
 */
EXTERNRT OSUTF8CHAR* rtxUTF8StrRefOrDup
(OSCTXT* pctxt, const OSUTF8CHAR* utf8str);

/**
 * This function compares two UTF-8 string values for equality.
 *
 * @param utf8str1     UTF-8 string to be compared.
 * @param utf8str2     UTF-8 string to be compared.
 * @return             TRUE if equal, FALSE if not.
 */
EXTERNRT OSBOOL rtxUTF8StrEqual
(const OSUTF8CHAR* utf8str1, const OSUTF8CHAR* utf8str2);

/**
 * This function compares two UTF-8 string values for equality.
 * It is similar to the \c rtxUTF8StrEqual function except that it allows the
 * number of bytes to compare to be specified.
 *
 * @param utf8str1     UTF-8 string to be compared.
 * @param utf8str2     UTF-8 string to be compared.
 * @param count        Number of bytes to compare.
 * @return             TRUE if equal, FALSE if not.
 */
EXTERNRT OSBOOL rtxUTF8StrnEqual (const OSUTF8CHAR* utf8str1,
                                   const OSUTF8CHAR* utf8str2,
                                   size_t count);

/**
 * This function compares two UTF-8 character strings and returns a trinary
 * result (equal, less than, greater than). It is similar to the C \c strcmp
 * function.
 *
 * @param utf8str1     UTF-8 string to be compared.
 * @param utf8str2     UTF-8 string to be compared.
 * @return             -1 if utf8str1 is less than utf8str2, 0 if the
 *                       two string are equal, and +1 if the utf8str1 is
 *                       greater than utf8str2.
 */
EXTERNRT int rtxUTF8Strcmp (const OSUTF8CHAR* utf8str1,
                             const OSUTF8CHAR* utf8str2);

/**
 * This function compares two UTF-8 character strings and returns a trinary
 * result (equal, less than, greater than). In this case, a maximum count of
 * the number of bytes to compare can be specified. It is similar to the C
 * \c strncmp function.
 *
 * @param utf8str1     UTF-8 string to be compared.
 * @param utf8str2     UTF-8 string to be compared.
 * @param count        Number of bytes to compare.
 * @return             -1 if utf8str1 is less than utf8str2, 0 if the
 *                       two string are equal, and +1 if the utf8str1 is
 *                       greater than utf8str2.
 */
EXTERNRT int rtxUTF8Strncmp (const OSUTF8CHAR* utf8str1,
                              const OSUTF8CHAR* utf8str2,
                              size_t count);
/**
 * This function copies a null-terminated UTF-8 string to a target buffer.
 * It is similar to the C \c strcpy function except more secure because
 * it checks for buffer overrun.
 *
 * @param dest         Pointer to destination buffer to receive string.
 * @param bufsiz       Size of the destination buffer.
 * @param src          Pointer to null-terminated string to copy.
 * @return             Pointer to destination buffer or NULL if copy failed.
 */
EXTERNRT OSUTF8CHAR* rtxUTF8Strcpy
(OSUTF8CHAR* dest, size_t bufsiz, const OSUTF8CHAR* src);

/**
 * This function copies the given number of characters from a UTF-8 string to
 * a target buffer. It is similar to the C \c strncpy function except more
 * secure because it checks for buffer overrun and ensures a null-terminator
 * is copied to the end of the target buffer
 *
 * @param dest         Pointer to destination buffer to receive string.
 * @param bufsiz       Size of the destination buffer.
 * @param src          Pointer to null-terminated string to copy.
 * @param nchars       Number of characters to copy.
 * @return             Pointer to destination buffer or NULL if copy failed.
 */
EXTERNRT OSUTF8CHAR* rtxUTF8Strncpy
(OSUTF8CHAR* dest, size_t bufsiz, const OSUTF8CHAR* src, size_t nchars);

/**
 * This function computes a hash code for the given string value.
 *
 * @param str          Pointer to string.
 * @return             Hash code value.
 */
EXTERNRT OSUINT32 rtxUTF8StrHash (const OSUTF8CHAR* str);

/**
 * This function concatanates up to five substrings together into a
 * single string.
 *
 * @param pctxt        Pointer to a context block structure.
 * @param str1         Pointer to substring to join.
 * @param str2         Pointer to substring to join.
 * @param str3         Pointer to substring to join.
 * @param str4         Pointer to substring to join.
 * @param str5         Pointer to substring to join.
 * @return             Composite string consisting of all parts.  Memory
 *                       is allocated for this string using rtxMemAlloc
 *                       and must be freed using either rtxMemFreePtr or
 *                       rtxMemFree.  If memory allocation for the string
 *                       fails, NULL is returned.
 */
EXTERNRT const OSUTF8CHAR* rtxUTF8StrJoin
(OSCTXT* pctxt, const OSUTF8CHAR* str1, const OSUTF8CHAR* str2,
 const OSUTF8CHAR* str3, const OSUTF8CHAR* str4, const OSUTF8CHAR* str5);

/**
 * This function converts the given null-terminated UTF-8 string to
 * a boolean (true/false) value.  It is assumed the string
 * contains only the tokens 'true', 'false', '1', or '0'.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to boolean value to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToBool (const OSUTF8CHAR* utf8str, OSBOOL* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * a boolean (true/false) value.  It is assumed the string
 * contains only the tokens 'true', 'false', '1', or '0'.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to boolean value to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToBool
   (const OSUTF8CHAR* utf8str, size_t nbytes, OSBOOL* pvalue);

/**
 * This function converts the given null-terminated UTF-8 string to
 * a floating point (C/C++ double) value.  It is assumed the string
 * contains only numeric digits, special floating point characters
 * (+,-,E,.), and whitespace.  It is similar to the C atof function
 * except that the result is returned as a separate argument and
 * an error status value is returned if the conversion cannot be
 * performed successfully.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to double to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToDouble (const OSUTF8CHAR* utf8str, OSREAL* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * a double value.  It is assumed the string contains only numeric
 * digits, whitespace, and other special floating point characters.
 * It is similar to the C atof function except that the result is
 * returned as a separate argument and an error status value is returned
 * if the conversion cannot be performed successfully.
 *
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to double to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToDouble
   (const OSUTF8CHAR* utf8str, size_t nbytes, OSREAL* pvalue);

/**
 * This function converts the given null-terminated UTF-8 string
 * to an integer  value.  It is assumed the string contains only numeric
 * digits and whitespace.  It is similar to the C atoi function
 * except that the result is returned as a separate argument and
 * an error status value is returned if the conversion cannot be
 * performed successfully.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToInt (const OSUTF8CHAR* utf8str, OSINT32* pvalue);

#define rtxUTF8StrToInt32 rtxUTF8StrToInt

/**
 * This function converts the given part of UTF-8 string to
 * an integer value.  It is assumed the string contains only numeric
 * digits and whitespace.  It is similar to the C atoi function
 * except that the result is returned as a separate argument and
 * an error status value is returned if the conversion cannot be
 * performed successfully.
 *
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToInt
   (const OSUTF8CHAR* utf8str, size_t nbytes, OSINT32* pvalue);

/**
 * This function converts the given null-terminated UTF-8 string to
 * an unsigned integer value.  It is assumed the string contains only
 * numeric digits and whitespace.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToUInt (const OSUTF8CHAR* utf8str, OSUINT32* pvalue);

#define rtxUTF8StrToUInt32 rtxUTF8StrToUInt

/**
 * This function converts the given part of UTF-8 string to
 * an unsigned integer value.  It is assumed the string contains
 * only numeric digits and whitespace.
 *
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToUInt
   (const OSUTF8CHAR* utf8str, size_t nbytes, OSUINT32* pvalue);

/**
 * This function converts the given null-terminated UTF-8 string to
 * a size value (type size_t).  It is assumed the string contains only
 * numeric digits and whitespace.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to size_t value to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToSize (const OSUTF8CHAR* utf8str, size_t* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * a size value (type size_t).  It is assumed the string contains
 * only numeric digits and whitespace.
 *
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to size_t value to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToSize
   (const OSUTF8CHAR* utf8str, size_t nbytes, size_t* pvalue);

#ifndef _NO_INT64_SUPPORT
/**
 * This function converts the given null-terminated UTF-8 string to
 * a 64-bit integer value.  It is assumed the string contains only numeric
 * digits and whitespace.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToInt64 (const OSUTF8CHAR* utf8str, OSINT64* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * a 64-bit integer value.  It is assumed the string contains only numeric
 * digits and whitespace.
 *
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToInt64
   (const OSUTF8CHAR* utf8str, size_t nbytes, OSINT64* pvalue);

/**
 * This function converts the given null-terminated UTF-8 string to
 * an unsigned 64-bit integer value.  It is assumed the string contains
 * only numeric digits and whitespace.
 *
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToUInt64 (const OSUTF8CHAR* utf8str, OSUINT64* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * an unsigned 64-bit integer value.  It is assumed the string contains
 * only numeric digits and whitespace.
 *
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to integer to receive result
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrnToUInt64
   (const OSUTF8CHAR* utf8str, size_t nbytes, OSUINT64* pvalue);

#endif /* _NO_INT64_SUPPORT */

/**
 * This function converts the given UTF-8 string to a Unicode string.
 * Memory is allocated for the Unicode string using the rtxMemAlloc
 * function.  This memory will be freed when the context is freed
 * (rtxFreeContext) or it can be freed using rtxMemFreePtr.
 *
 * @param pctxt        A pointer to a context structure.
 * @param utf8str      UTF-8 string to convert, null-terminated.
 * @param ppdata       Pointer to pointer to receive output string.
 * @param pnchars      Pointer to integer to receive number of chars decoded.
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8ToDynUniStr
   (OSCTXT* pctxt, const OSUTF8CHAR* utf8str,
    const OSUNICHAR** ppdata, OSUINT32* pnchars);

/**
 * This function removes leading and trailing whitespace from a
 * string.
 *
 * @param utf8instr    Input UTF-8 string from which to removed whitespace.
 * @param nbytes       Size in bytes of utf8instr.
 * @param putf8outstr  Pointer to receive result string.
 * @return             Positive value = length of result string,
 *                     negative value = error code.
 */
EXTERNRT int rtxUTF8RemoveWhiteSpace
(const OSUTF8CHAR* utf8instr, size_t nbytes, const OSUTF8CHAR** putf8outstr);

/**
 * Compare UTF-8 string to a string literal.
 *
 * @param name UTF-8 string variable.
 * @param lstr C string literal value (quoted contant such as "a")
 */
#define RTUTF8STRCMPL(name,lstr) \
rtxUTF8Strcmp(name,(const OSUTF8CHAR*)lstr)

/**
 * This function converts the given null-terminated UTF-8 string to
 * a octet string value.  The string consists of a series of hex digits.
 * This is the dynamic version in which memory is allocated for
 * the returned octet string variable.
 *
 * @param pctxt        Pointer to context block structure.
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to a variable to receive the decoded octet
 *                       string value.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxUTF8StrToDynHexStr
(OSCTXT* pctxt, const OSUTF8CHAR* utf8str, OSDynOctStr* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * a octet string value.  The string consists of a series of hex digits.
 * This is the dynamic version in which memory is allocated for
 * the returned octet string variable.
 *
 * @param pctxt        Pointer to context block structure.
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to a variable to receive the decoded octet
 *                       string value.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxUTF8StrnToDynHexStr
(OSCTXT* pctxt, const OSUTF8CHAR* utf8str, size_t nbytes, OSDynOctStr* pvalue);

/**
 * This function converts the given null-terminated UTF-8 string
 * to named bit items.  The token-to-bit mappings are defined by
 * a bit map table that is passed into the function.  It is assumed
 * the string contains a space-separated list of named bit token
 * values.
 *
 * @param pctxt        Context structure
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pBitMap      Bit map defining bit to otken mappings
 * @param pvalue       Pointer to byte array to receive result.
 * @param pnbits       Pointer to integer to received number of bits.
 * @param bufsize      Size of byte array to received decoded bits.
 * @return             Status: 0 = OK, negative value = error
 */
EXTERNRT int rtxUTF8StrToNamedBits (OSCTXT* pctxt, const OSUTF8CHAR* utf8str,
   const OSBitMapItem* pBitMap, OSOCTET* pvalue, OSUINT32* pnbits,
   OSUINT32 bufsize);

/**
 * This function returns the next whitespace-separated token from the
 * input string.  It also returns a pointer to the first non-whitespace
 * chracter after the parsed token.  Note that the input string is
 * altered in the operation as null-terminators are insterted to mark
 * the token boundaries.
 *
 * @param utf8str      Null-terminated UTF-8 string to parse.  This
 *                       string will be altered.  Use rtxUTF8Strdup to
 *                       make a copy of original string before calling
 *                       this function if the original string cannot
 *                       be altered.
 * @param ppNext       Pointer to receive next location in string after
 *                       parsed token.  This can be used as input to get
 *                       the next token.  If NULL returned, all tokens in
 *                       in the string have been parsed.
 * @return             Pointer to next parsed token.  NULL if no more tokens.
 */
EXTERNRT const OSUTF8CHAR* rtxUTF8StrNextTok
(OSUTF8CHAR* utf8str, OSUTF8CHAR** ppNext);

#define OSRTCHKUTF8LEN(str,lower,upper,stat) \
do { size_t nchars = rtxUTF8Len (str); \
stat = (nchars >= lower && nchars <= upper) ? 0 : RTERR_CONSVIO; } while(0)

/**
 * @} ccfUTF8
 */
#ifdef __cplusplus
}
#endif

#endif
