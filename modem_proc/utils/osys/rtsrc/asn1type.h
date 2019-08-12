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
 /*The following is the text for the main C/C++ Runtime index page.*/
 /*! \mainpage C/C++ Common Runtime Classes and Library Functions
 *
 * The <B>ASN.1 C++ run-time classes</B> are wrapper classes that provide an
 * object-oriented interface to the ASN.1 C run-time library functions. The
 * categories of classes provided are as follows:
 * <UL>
 * <LI>Context Management classes manage the OSCTXT structure used to keep
 * track of the working variables required to encode or decode ASN.1
 * messages.</LI>
 * <LI>Message Buffer classes are used to manage message buffers.</LI>
 * <LI>ASN1C Control Base classes are wrapper classes that are used as the
 * base for compiler-generated ASN1C_ classes, including Date and Time
 * Run-time classes.</LI>
 * <LI>ASN.1 Type (ASN1T_) Base classes are used as the base for compiler-
 * generated ASN1T_ C++ data structures. </LI>
 * <LI>ASN.1 Stream classes are used to read and write ASN.1 messages from
 * and to files, sockets, memory buffer, etc.</LI>
 * <LI>TCP/IP or UDP Socket classes provide utility methods for doing socket
 * I/O. <LI>%Asn1NamedEventHandler classes include the base classes for
 * user-defined error handler and event handler classes.</LI> </UL>
 *
 * The <B>C run-time common library</B> contains common C functions used by
 * the encoding rules (BER/DER, PER, and XER) low-level encode/decode
 * functions. These functions are identified by their <I>rt</I> prefixes.
 * The categories of functions provided are as follows:<UL>
 * <LI>Memory Allocation macros and functions handle memory
 * management for the ASN1C run-time.</LI>
 * <LI>Context Management functions handle the allocation,
 * initialization, and destruction of context variables
 * (variables of type OSCTXT) that handle the working data used
 * during encoding or decoding a message.</LI>
 * <LI>Diagnostic Trace functions allow the output of trace messages
 * to standard output that trace the execution of complier generated
 * functions.</LI>
 * <LI>Error Formatting and Print functions allow information about
 * the encode/decode errors to be added to a context block structure
 * and printed out.</LI>
 * <LI>Memory Buffer Management functions handle the allocation,
 * expansion, and de-allocation of dynamic memory buffers used by some
 * encode/decode functions.</LI>
 * <LI>Object Identifier Helper functions provide assistance in working
 * with the object identifier ASN.1 type. </LI>
 * <LI>Linked List and Stack Utility functions are used to maintain
 * linked lists and stacks used within the ASN.1 run-time library
 * functions.</LI>
 * <LI>REAL Helper functions - REAL helper functions provide assistance
 * in working with the REAL ASN.1 type. Two functions are provided to
 * obtain the plus and minus infinity special values.  </LI>
 * <LI>Formatted Printing functions allow raw ASN.1 data fields to be
 * formatted and printed to standard output and other output devices.</LI>
 * <LI>Binary Coded Decimal (BCD) helper functions provide assistance in
 * working with BCD numbers.</LI>
 * <LI>Character String Conversion functions convert between standard
 * null-terminated C strings and different ASN.1 string types.</LI>
 * <LI>Big Integer Helper functions are arbitrary-precision integer
 * manipulating functions used to maintain big integers used within the
 * ASN.1 run-time functions. </LI>
 * <LI>Comparison functions allow comparison of the values of primitive
 * ASN.1 types.  They make it possible to compare complex structures and
 * determine what elements within those structures are different.</LI>
 * <LI>Comparison to Standard Output functions do the same actions as the
 * other comparison functions, but print the comparison results to standard
 * output instead of to the buffer. </LI>
 * <LI>Copy functions - This group of functions allows copying values of
 * primitive ASN.1 types. </LI>
 * <LI>Print Values to Standard Output functions print the output in a
 * "name=value" format, where the value format is obtained by calling
 *  one of the ToString functions with the given value.</LI>
 * </UL>
 */
/**
 * @file asn1type.h
 * Common ASN.1 runtime constants, data structure definitions, and run-time
 * functions to support the BER/DER/PER/XER as defined in the ITU-T standards.
 */
#ifndef _ASN1TYPE_H_
#define _ASN1TYPE_H_

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h> /*so that borland compilers don't choke on extern linkage*/
#ifndef _WIN32_WCE
#ifdef T_QDSP6 /* QC_MOD */
#include <common/time.h>
#else
#include <time.h>
#endif /*T_QDSP6*/
#endif

#ifdef _NUCLEUS
#ifndef NUCLEUS
#include <nucleus.h>
#endif /* NUCLEUS */
#else

#if !defined(_MSC_VER) && !defined(__GNUC__) && !defined(_16BIT) && !defined(__SYMBIAN32__)
#include <wchar.h>
#endif

#endif /* _NUCLEUS */

#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxSList.h"
#include "rtxsrc/rtxStack.h"
#include "rtxsrc/rtxUTF8.h"


/**
 * @defgroup cruntime C Runtime Common Functions
 * The <B>C run-time common library</B> contains common C functions used by
 * the low-level encode/decode functions. These functions are identified
 * by their <I>rt</I> and  <I>rtx</I> prefixes.
 *
 * The categories of functions provided are as follows:<UL>
 * <LI>Context management functions handle the allocation,
 * initialization, and destruction of context variables
 * (variables of type OSCTXT) that handle the working data used
 * during the encoding or decoding of a message.</LI>
 * <LI>Memory allocation macros and functions provide an optimized memory
 * management interface.</LI>
 * <LI>Doubly linked list (DList) functions are used to manipulate linked
 * list structures that are used to model repeating XSD types and
 * elements.</LI>
 * <LI>UTF-8 and Unicode character string functions provide support
 * for conversions to and from these formats in C or C++.</LI>
 * <LI>Date/time conversion functions provide utilities for converting
 * system and structured numeric date/time values to XML schema standard
 * string format.</LI>
 * <LI>Pattern matching function compare strings against patterns specified
 * using regular expressions (regexp's).</LI>
 * <LI>Diagnostic trace functions allow the output of trace messages
 * to standard output.</LI>
 * <LI>Error formatting and print functions allow information about
 * encode/decode errors to be added to a context block structure
 * and printed out.</LI>
 * <LI>Memory buffer management functions handle the allocation,
 * expansion, and de-allocation of dynamic memory buffers used by some
 * encode/decode functions.</LI>
 * <LI>Formatted print functions allow binary data to be
 * formatted and printed to standard output and other output devices.</LI>
 * <LI>Big Integer helper functions are arbitrary-precision integer
 * manipulating functions used to maintain big integers.</LI>
 * </UL>
  * @{
 */
#include "rtsrc/asn1tag.h"
#include "rtsrc/asn1ErrCodes.h"
#include "rtsrc/asn1version.h"
#include "rtsrc/rtExternDefs.h"

/*
 * Flags used to determine or change the behavior of encoding and
 * decoding.
 */
#define XM_SEEK         0x01    /* seek match until found or end-of-buf */
#define XM_ADVANCE      0x02    /* advance pointer to contents on match */
#define XM_DYNAMIC      0x04    /* alloc dyn mem for decoded variable   */
#define XM_SKIP         0x08    /* skip to next field after parsing tag */
#define XM_OPTIONAL     0x10    /* tag test is for optional element     */

/* Sizing Constants */

#define ASN_K_MAXDEPTH  32      /* maximum nesting depth for messages   */
#define ASN_K_MAXENUM   100     /* maximum enum values in an enum type  */
#define ASN_K_MAXERRP   5       /* maximum error parameters             */
#define ASN_K_MAXERRSTK 8       /* maximum levels on error ctxt stack   */

#ifndef ASN_K_ENCBUFSIZ
#define ASN_K_ENCBUFSIZ 16*1024 /* dynamic encode buffer extent size    */
#endif

#ifndef ASN_K_MEMBUFSEG
#define ASN_K_MEMBUFSEG 1024    /* memory buffer extent size            */
#endif

#define OSRTINDENTSPACES        3       /* number of spaces for indent  */

/* Constants for encoding/decoding real values */

#define ASN1_K_PLUS_INFINITY    0x40
#define ASN1_K_MINUS_INFINITY   0x41

#define REAL_BINARY             0x80
#define REAL_SIGN               0x40
#define REAL_EXPLEN_MASK        0x03
#define REAL_EXPLEN_1           0x00
#define REAL_EXPLEN_2           0x01
#define REAL_EXPLEN_3           0x02
#define REAL_EXPLEN_LONG        0x03
#define REAL_FACTOR_MASK        0x0c
#define REAL_BASE_MASK          0x30
#define REAL_BASE_2             0x00
#define REAL_BASE_8             0x10
#define REAL_BASE_16            0x20
#define REAL_ISO6093_MASK       0x3F

#ifndef __SYMBIAN32__
#include <float.h>
#define ASN1REALMAX    (OSREAL)DBL_MAX
#define ASN1REALMIN    (OSREAL)-DBL_MAX
#endif

typedef void*           ASN1ANY;

/**
 * An enumerated list of the various string types:  hexadecimal, binary, and
 * character strings.
 */
typedef enum { ASN1HEX, ASN1BIN, ASN1CHR } ASN1StrType;

/**
 * An enumerated list of ASN.1 actions:  encode or decode.
 */
typedef enum { ASN1ENCODE, ASN1DECODE } ASN1ActionType;

/*
 * OBJECT IDENTIFIER
 */
/**
 * @defgroup objidhelpers Object Identifier Helper Functions
 * @{
 *
 * Object identifier helper functions provide assistance in working with the
 * object identifier ASN.1 type.
 */

#define ASN_K_MAXSUBIDS 128     /* maximum sub-id's in an object ID     */

/**
 * This structure describes an object identifier with 32-bit arcs.
 */
typedef struct {        /* object identifier */
   /** The number of sub-identifiers in the OID. */
   OSUINT32     numids;
   /** An array of sub-identifiers. */
   OSUINT32     subid[ASN_K_MAXSUBIDS];
} ASN1OBJID;

/**
 * This structure describes an object identifier with 64-bit arcs.
 */
typedef struct {        /* object identifier with 64-bit arcs */
   /** The number of sub-identifiers in the OID. */
   OSUINT32     numids;
   /** An array of sub-identifiers. */
   OSUINT64     subid[ASN_K_MAXSUBIDS];
} ASN1OID64;

/** @cond false */
#ifdef __cplusplus
#define EXTERN_C extern "C"
extern "C" {
#else
#define EXTERN_C extern
#endif
/** @endcond */

/**
 * This function populates an object identifier variable with data. It copies
 * data from a source variable to a target variable. Typically, the source
 * variable is a compiler-generated object identifier constant that resulted
 * from a object identifier value specification within an ASN.1 specification.
 *
 * @param ptarget      A pointer to a target object identifier variable to
 *                       receive object * identifier data. Typically, this is a
 *                       variable within a compiler-generated C structure.
 * @param psource      A pointer to a source object identifier variable to copy
 *                       to a target. Typically, this is a compiler-generated
 *                       variable corresponding to an ASN.1 value specification
 *                       in the ASN.1 source file.
 */
EXTERNRT void  rtSetOID (ASN1OBJID* ptarget, ASN1OBJID* psource);

/**
 * This function appends one object identifier to another one. It copies the
 * data from a source variable to the end of a target variable. Typically, the
 * source variable is a compiler-generated object identifier constant that
 * resulted from an object identifier value specification within an ASN.1
 * specification.
 *
 * @param ptarget      A pointer to a target object identifier variable to
 *                       receive object identifier data. Typically, this is a
 *                       variable within a compiler-generated C structure.
 * @param psource      A pointer to a source object identifier variable to copy
 *                       to a target. Typically, this is a compiler-generated
 *                       variable corresponding to an ASN.1 value specification
 *                       in the ASN.1 source file.
 */
EXTERNRT void  rtAddOID (ASN1OBJID* ptarget, ASN1OBJID* psource);

/**
 * This function compares two OID values for equality.
 *
 * @param pOID1         Pointer to first OID value to compare.
 * @param pOID2         Pointer to second OID value to compare.
 * @return              True if OID's are equal.
 */
EXTERNRT OSBOOL rtOIDsEqual (const ASN1OBJID* pOID1, const ASN1OBJID* pOID2);

/**
 * This function parses an OID dotted number string (n.n.n) which is the
 * from of OID XML content.  Data must be in the form of numbers and dots
 * only (i.e. OID components in other forms such as names or named
 * number will cause a parse failure).  Embedded whitespace will be
 * ignored.
 *
 * @param oidstr        OID string containing data to be parsed.
 * @param oidstrlen     Length of the string.
 * @param pvalue        Pointer to OID value to receive parsed OID.
 * @return              Status of operation: 0 = success, negative value
 *                        is failure.
 */
EXTERNRT int rtOIDParseDottedNumberString
(const char* oidstr, OSSIZE oidstrlen, ASN1OBJID* pvalue);

/**
 * This function determine if an OID value is valid according to ASN.1
 * rules.  In particular it checks a) if number of subidentifiers is
 * greater than or equal to 2, b) if the first subidentifier value is
 * less than or equal to 2, and c) if the first subidentifier is 2
 * that the second subidentifier is less than 40.
 *
 * @param pvalue        Pointer to OID value to validate.
 * @return              True if OID value is valid.
 */
EXTERNRT OSBOOL rtOIDIsValid (const ASN1OBJID* pvalue);

/**
 * @}
 */

/*
 * OCTET STRING
 */
typedef struct {        /* generic octet string structure */
   OSUINT32     numocts;
   OSOCTET      data[1];
} ASN1OctStr;

#define ASN1DynOctStr OSDynOctStr

/*
 * BIT STRING
 */
typedef struct {  /* generic bit string structure (dynamic) */
   OSUINT32     numbits;
   const OSOCTET* data;
} ASN1DynBitStr;

typedef struct {  /* fixed-size bit string that can hold up to 32 bits */
   OSUINT32     numbits;
   OSOCTET      data[4];
} ASN1BitStr32;

#include "rtxsrc/rtxBitString.h"

#define OSSETBIT(bitStr,bitIndex) \
rtxSetBit (bitStr.data, bitStr.numbits, bitIndex)

#define OSSETBITP(pBitStr,bitIndex) \
rtxSetBit ((pBitStr)->data, (pBitStr)->numbits, bitIndex)

#define OSCLEARBIT(bitStr,bitIndex) \
rtxClearBit (bitStr.data, bitStr.numbits, bitIndex)

#define OSCLEARBITP(pBitStr,bitIndex) \
rtxClearBit ((pBitStr)->data, (pBitStr)->numbits, bitIndex)

#define OSTESTBIT(bitStr,bitIndex) \
rtxTestBit (bitStr.data, bitStr.numbits, bitIndex)

#define OSTESTBITP(pBitStr,bitIndex) \
rtxTestBit ((pBitStr)->data, (pBitStr)->numbits, bitIndex)

/**
 * @defgroup timeutilf Time Helper Functions
 * @{
 * Utility functions for working with time strings in different
 * formats.  rtMake* functions create time strings, rtParse* functions
 * parse time strings into the C OSNumDateTime structure defined in
 * osSysTypes.h. Implementations of these functions exist for the
 * ASN.1 GeneralizedTime and UTCTime formats.
 */
/**
 * This function creates a time string in ASN.1 GeneralizedTime format as
 * specified in the X.680 ITU-T standard.
 *
 * @param pctxt        A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param dateTime     A pointer to a date/time structure that contains
 *                       components of the date and time.
 * @param outdata      A pointer to a pointer to a destination string. If
 *                       outdataSize is non-zero, it should be a pointer to a
 *                       pointer to an actual array. Otherwise, the memory will
 *                       be allocated and the pointer will be stored in the \c
 *                       outdata.
 * @param outdataSize  A size of outdata (in octets). If zero, the memory for
 *                       the \c outdata will be allocated. If not, the \c
 *                       outdata 's size should be big enough to receive the
 *                       generated time string. Otherwise, error code will be
 *                       returned.
 * @return             Completion status of operation:
 *                       - 0 (ASN_OK) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtMakeGeneralizedTime (OSCTXT* pctxt,
                                    const OSNumDateTime* dateTime,
                                    char** outdata,
                                    size_t outdataSize);

/**
 * This function creates a time string in ASN.1 UTCTime format as specified in
 * the X.680 ITU-T standard.
 *
 * @param pctxt        A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param dateTime     A pointer to a date/time structure that contains
 *                       components of the date and time.
 * @param outdata      A pointer to a pointer to a destination string. If
 *                       outdataSize is non-zero, it should be a pointer to a
 *                       pointer to an actual array. Otherwise, the memory will
 *                       be allocated and the pointer will be stored in the \c
 *                       outdata.
 * @param outdataSize  A size of outdata (in octets). If zero, the memory for
 *                       the \c outdata will be allocated. If not, the \c
 *                       outdata 's size should be big enough to receive the
 *                       generated time string. Otherwise, error code will be
 *                       returned.
 * @return             Completion status of operation:
 *                       - 0 (ASN_OK) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtMakeUTCTime (OSCTXT* pctxt,
                            const OSNumDateTime* dateTime,
                            char** outdata,
                            size_t outdataSize);
/**
 * This function parses a time string that is represented in ASN.1
 * GeneralizedTime format as specified in the X.680 ITU-T standard. It stores
 * the parsed result in a numeric date/time C structure.
 *
 * @param pctxt        A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param value        A pointer to the time string to be parsed.
 * @param dateTime     A pointer to the destination structure.
 * @return             Completion status of operation:
 *                       - 0 (ASN_OK) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtParseGeneralizedTime (OSCTXT *pctxt,
                                     const char* value,
                                     OSNumDateTime* dateTime);

/**
 * This function parses a time string that is represented in ASN.1 UTCTime
 * format as specified in the X.680 ITU-T standard. It stores the parsed
 * result in a numeric date/time C structure.
 *
 * @param pctxt        A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param value        A pointer to the time string to be parsed.
 * @param dateTime     A pointer to the destination date/time structure.
 * @return             Completion status of operation:
 *                       - 0 (ASN_OK) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtParseUTCTime (OSCTXT *pctxt,
                             const char* value,
                             OSNumDateTime* dateTime);

EXTERNRT void normalizeTimeZone(OSNumDateTime* pvalue);

/**
 * @}
 */

/*
 * SEQUENCE OF
 */
typedef struct {                /* generic sequence of structure        */
   OSUINT32     n;
   void*        elem;
} ASN1SeqOf;

typedef struct {                /* sequence of OCTET STRING             */
   OSUINT32     n;
   OSDynOctStr* elem;
} ASN1SeqOfOctStr;

/**
 * A generic open type structure.  This structure contains a number of
 * octets and the data that compose the open type information.
 */
typedef struct {                /* generic open type data structure     */
   OSUINT32     numocts;
   const OSOCTET* data;
} ASN1OpenType;

typedef struct {                /* generic table constraint value holder */
   ASN1OpenType encoded;
   void*        decoded;
   OSINT32      index;          /* table index */
} Asn1Object;

typedef Asn1Object ASN1Object;

/* ASN.1 useful type definitions */

typedef struct {
   OSUINT32       nchars;
   OSUNICHAR*     data;
} Asn116BitCharString;

typedef OSUNICHAR ASN116BITCHAR;

typedef struct {
   OSUINT32       nchars;
   OS32BITCHAR*   data;
} Asn132BitCharString;

typedef const char*   ASN1GeneralizedTime;
typedef const char*   ASN1GeneralString;
typedef const char*   ASN1GraphicString;
typedef const char*   ASN1IA5String;
typedef const char*   ASN1ISO646String;
typedef const char*   ASN1NumericString;
typedef const char*   ASN1ObjectDescriptor;
typedef const char*   ASN1PrintableString;
typedef const char*   ASN1TeletexString;
typedef const char*   ASN1T61String;
typedef const char*   ASN1UTCTime;
typedef const char*   ASN1VideotexString;
typedef const char*   ASN1VisibleString;

typedef const OSUTF8CHAR*   ASN1UTF8String;
typedef Asn116BitCharString ASN1BMPString;
typedef Asn132BitCharString ASN1UniversalString;

/* ASN.1 constrained string structures */

typedef struct {
   int          nchars;
   char         data[255];
} Asn1CharArray;

typedef struct {
   Asn1CharArray charSet;
   const char*  canonicalSet;
   int          canonicalSetSize;
   unsigned     canonicalSetBits;
   unsigned     charSetUnalignedBits;
   unsigned     charSetAlignedBits;
} Asn1CharSet;

typedef struct {
   Asn116BitCharString charSet;
   OSUINT16     firstChar, lastChar;
   unsigned     unalignedBits;
   unsigned     alignedBits;
} Asn116BitCharSet;

typedef struct {
   Asn132BitCharString charSet;
   OSUINT32     firstChar, lastChar;
   unsigned     unalignedBits;
   unsigned     alignedBits;
} Asn132BitCharSet;

/**
 * @defgroup charstrcon Character String Conversion Functions
 * @{
 *
 * Common utility functions are provided to convert between standard
 * null-terminated C strings and different ASN.1 string types.
 */

/* Helper functions. */

/**
 * This function ensures that a given string does not contain invalid
 * characters.
 *
 * @param   tag         The ASN.1 Tag that identifies the string.
 * @param   object_p    A pointer to the character string to be examined.
 *
 * @return              This function returns 0 if the string validates
 *                      or the tag is not associated with a string; it
 *                      otherwise returns the integer value of the character
 *                      that invalidates the string.
 */
EXTERNRT int rtValidateStr (ASN1TAG tag, const char *pdata);

/**
 * This function ensures that a given string does not contain invalid
 * characters.
 *
 * @param   tag         The ASN.1 Tag that identifies the string.
 * @param   pdata       A pointer to the character string to be examined.
 * @param   nchars      The number of characters in pdata.
 *
 * @return              This function returns 0 if the string validates
 *                      or the tag is not associated with a string; it
 *                      otherwise returns the integer value of the character
 *                      that invalidates the string.
 */
EXTERNRT int rtValidateChars (ASN1TAG tag, const char* pdata, size_t nchars);

/* 16-bit character functions */

/**
 * This function converts a BMP string into a null-terminated C string. Any
 * characters that are not 8-bit characters are discarded.
 *
 * @param pBMPString   A pointer to a BMP string structure to be converted.
 * @param cstring      A pointer to a buffer to receive the converted string.
 * @param cstrsize     The size of the buffer to receive the converted string.
 * @return             A pointer to the returned string structure. This is the
 *                       cstring argument parameter value.
 */
EXTERNRT const char* rtBMPToCString (ASN1BMPString* pBMPString,
                                      char* cstring, OSUINT32 cstrsize);

/**
 * This function converts a BMP string into a null-terminated C string. Any
 * characters that are not 8-bit characters are discarded. This function
 * allocates dynamic memory to hold the converted string using the standard C
 * run-time malloc function. The user is responsible for freeing this memory.
 *
 * @param pBMPString   A pointer to a BMP string structure to be converted.
 * @return             A pointer to the returned string structure. This is the
 *                       cstring argument parameter value.
 */
EXTERNRT const char* rtBMPToNewCString (ASN1BMPString* pBMPString);

/**
 * This function converts a BMP string into a null-terminated C string. Any
 * characters that are not 8-bit characters are discarded. In contrast to
 * rtBMPToNewCString, this function allocates dynamic memory to hold the
 * converted string using the rtMemAlloc function. The rtMemFreePtr should be
 * called to release the allocated memory or the rtmemFree function should be
 * called to release all memory allocated using the specified context block.
 *
 * @param pctxt        A pointer to a context structure.
 * @param pBMPString   A pointer to a BMP string structure to be converted.
 * @return             A pointer to the returned string structure. This is the
 *                       cstring argument parameter value.
 */
EXTERNRT const char* rtBMPToNewCStringEx
   (OSCTXT *pctxt, ASN1BMPString* pBMPString);

/**
 * This function converts a null-terminated C string into a 16-bit BMP string
 * structure.
 *
 * @param pctxt       A pointer to a context string.
 * @param cstring      A pointer to a null-terminated C string to be converted
 *                       into a BMP string.
 * @param pBMPString   A pointer to a BMP string structure to receive the
 *                       converted string.
 * @param pCharSet     A pointer to a character set structure describing the
 *                       character set currently associated with the BMP
 *                       character string type.
 * @return             A pointer to BMP string structure. This is the
 *                       pBMPString argument parameter value.
 */
EXTERNRT ASN1BMPString* rtCToBMPString (OSCTXT* pctxt,
                                         const char* cstring,
                                         ASN1BMPString* pBMPString,
                                         Asn116BitCharSet* pCharSet);

EXTERNRT OSBOOL rtIsIn16BitCharSet (OSUNICHAR ch,
                                     Asn116BitCharSet* pCharSet);

/* 32-bit character (UCS-4) functions */


/**
 * This function converts a Universal 32-bit string into a null-terminated C
 * string. Any characters that are not 8-bit characters are discarded.
 *
 * @param pUCSString   A pointer to a Universal string structure to be
 *                       converted.
 * @param cstring      A pointer to a buffer to receive a converted string.
 * @param cstrsize     The size of the buffer to receive the converted string.
 * @return             The pointer to the returned string. This is the cstring
 *                       argument parameter value.
 */
EXTERNRT const char* rtUCSToCString (ASN1UniversalString* pUCSString,
                                      char* cstring, OSUINT32 cstrsize);

/**
 * This function converts a Universal 32-bit string into a null-terminated C
 * string. Any characters that are not 8-bit characters are discarded. This
 * function allocates dynamic memory to hold the converted string using the
 * standard C run-time malloc function. The user is responsible for freeing
 * this memory.
 *
 * @param pUCSString   A pointer to a Universal 32-bit string structure to be
 *                       converted.
 * @return             A pointer to allocated null-terminated string. The user
 *                       is responsible for freeing this memory.
 */
EXTERNRT const char* rtUCSToNewCString (ASN1UniversalString* pUCSString);

/**
 * This function converts a Universal 32-bit string into a null-terminated C
 * string. Any characters that are not 8-bit characters are discarded. In
 * contrast to rtUSCToNewCString this function allocates dynamic memory to hold
 * the converted string using the rtMemAlloc function. The rtMemFreePtr should
 * be called to release the allocated memory or the rtMemFree function should
 * be called to release all memory allocated using the specified context block.
 *
 * @param pctxt        A pointer to a context block.
 * @param pUCSString   A pointer to a Universal 32-bit string structure to be
 *                       converted.
 * @return             A pointer to allocated null-terminated string. The user
 *                       is responsible for freeing this memory.
 */
EXTERNRT const char* rtUCSToNewCStringEx
(OSCTXT* pctxt, ASN1UniversalString* pUCSString);

/**
 * This function converts a null-terminated C string into a 32-bit UCS-4
 * (Universal Character Set, 4 byes) string structure.
 *
 * @param pctxt       A pointer to a context structure.
 * @param cstring      A pointer to a null-terminated C string to be converted
 *                       into a Universal string.
 * @param pUCSString   A pointer to a Universal string structure to receive the
 *                       converted string
 * @param pCharSet     A pointer to a character structure describing the
 *                       character set currently associated with the Universal
 *                       character string type.
 * @return             A pointer to a Universal string structure. This is the
 *                       pUCSString argument parameter value.
 */
EXTERNRT ASN1UniversalString* rtCToUCSString
(OSCTXT* pctxt, const char* cstring, ASN1UniversalString* pUCSString,
 Asn132BitCharSet* pCharSet);


EXTERNRT OSBOOL rtIsIn32BitCharSet (OS32BITCHAR ch,
                                     Asn132BitCharSet* pCharSet);

/**
 * This function converts a 32-bits encoded string to a wide character string.
 *
 * @param pUCSString   A pointer to a Universal string structure.
 * @param wcstring     The pointer to the buffer to receive the converted
 *                       string.
 * @param wcstrsize    The number of wide characters (wchar_t) the outbuffer
 *                       can hold.
 * @return             A character count or error status. This will be negative
 *                       if the conversion fails. If the result is positive,
 *                       the number of characters was written to scstrsize.
 */
EXTERNRT wchar_t* rtUCSToWCSString (ASN1UniversalString* pUCSString,
                                    wchar_t* wcstring, OSUINT32 wcstrsize);


/**
 * This function converts a wide-character string to a Universal 32-bits
 * encoded string.
 *
 * @param pctxt       A pointer to a context structure.
 * @param wcstring     The pointer to the wide-character (Unicode) string to
 *                       convert
 * @param pUCSString   The pointer to the Universal String structure to receive
 *                       the converted string.
 * @param pCharSet     The pointer to the character set structure describing
 *                       the character set currently associated with the
 *                       Universal character string type.
 * @return             If the conversion of the WCS to the UTF-8 was
 *                       successful, the number of bytes in the converted
 *                       string is returned. If the encoding fails, a negative
 *                       status value is returned.
 */
EXTERNRT ASN1UniversalString* rtWCSToUCSString
(OSCTXT* pctxt, wchar_t* wcstring,
 ASN1UniversalString* pUCSString, Asn132BitCharSet* pCharSet);

/**
 * This function converts an ASN.1 Universal String type (32-bit
 * characters) to UTF-8.
 *
 * @param pctxt       A pointer to a context structure.
 * @param pUnivStr    Pointer to universal string to be converted.
 * @param outbuf      Output buffer to receive UTF-8 characters.
 * @param outbufsiz   Output buffer size in bytes.
 * @return            Zero if conversion was successful, a negative
 *                       status code if failed.
 */
EXTERNRT int rtUnivStrToUTF8
(OSCTXT* pctxt, const ASN1UniversalString* pUnivStr, OSOCTET* outbuf,
 size_t outbufsiz);

/**
 * This function converts the given null-terminated UTF-8 string to
 * a bit string value.  The string consists of a series of '1' and '0'
 * characters. This is the dynamic version in which memory is allocated for
 * the returned binary string variable.  Bits are stored from MSB to LSB order.
 *
 * @param pctxt        Pointer to context block structure.
 * @param utf8str      Null-terminated UTF-8 string to convert
 * @param pvalue       Pointer to a variable to receive the decoded boolean
 *                       value.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtUTF8StrToASN1DynBitStr
   (OSCTXT* pctxt, const OSUTF8CHAR* utf8str, ASN1DynBitStr* pvalue);

/**
 * This function converts the given part of UTF-8 string to
 * a bit string value.  The string consists of a series of '1' and '0'
 * characters. This is the dynamic version in which memory is allocated for
 * the returned binary string variable.  Bits are stored from MSB to LSB order.
 *
 * @param pctxt        Pointer to context block structure.
 * @param utf8str      UTF-8 string to convert. Not necessary to be
 *                        null-terminated.
 * @param nbytes       Size in bytes of utf8Str.
 * @param pvalue       Pointer to a variable to receive the decoded boolean
 *                       value.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtUTF8StrnToASN1DynBitStr
   (OSCTXT* pctxt, const OSUTF8CHAR* utf8str, size_t nbytes,
   ASN1DynBitStr* pvalue);

/**
 * @}
 */

/* Big Integer */

typedef struct ASN1BigInt {
   size_t    numocts;           /* number of used octets in mag         */
   OSOCTET*  mag;               /* magnitude                            */
   int       sign;              /* sign, can be -1, 0, 1                */
   size_t    allocated;         /* number of allocated octets for mag   */
   OSBOOL    dynamic;           /* is buffer dynamic?                   */
} ASN1BigInt;

/* ASN.1 context control block - this structure is used to keep track   */
/* of pointer and length context values when decoding sets or sequence  */
/* of constructs.                                                       */

#define ASN1_K_CCBMaskSize      32
#define ASN1_K_NumBitsPerMask   16
#define ASN1_K_MaxSetElements   (ASN1_K_CCBMaskSize*ASN1_K_NumBitsPerMask)

typedef struct {                /* context control block        */
   OSOCTET*     ptr;            /* constructor pointer          */
   long         len;            /* constructor length           */
   int          seqx;           /* sequence element index       */
   OSUINT16     mask[ASN1_K_CCBMaskSize];  /* set mask value  */
   size_t       bytes;          /* bytes processed before block (for stream) */
   int          stat;           /* status, returned by BS_CHKEND */
} ASN1CCB;

/* ASN.1 dump utility callback function definition */

typedef int (*ASN1DumpCbFunc) (const char* text_p, void* cbArg_p);

/**
 * @ingroup cruntime
 *
 * Memory allocation functions and macros handle memory management for the
 * ASN1C run-time. Special algorithms are used for allocation and deallocation
 * of memory to improve the run-time performance. @{
 */
/**
 * Allocate a dynamic array. This macro allocates a dynamic array of records of
 * the given type. This version of the macro will return the RTERR_NOMEM error
 * status if the memory request cannot be fulfilled.
 *
 * @param pctxt        - Pointer to a context block
 * @param pseqof       - Pointer to a generated SEQUENCE OF array structure.
 *                       The <i>n</i> member variable must be set to the number
 *                       of records to allocate.
 * @param type         - Data type of an array record
 */
#define ALLOC_ASN1ARRAY(pctxt,pseqof,type) do {\
if (sizeof(type)*(pseqof)->n < (pseqof)->n) return RTERR_NOMEM; \
if (((pseqof)->elem = (type*) rtxMemHeapAlloc \
(&(pctxt)->pMemHeap, sizeof(type)*(pseqof)->n)) == 0) return RTERR_NOMEM; \
} while (0)

/**
 * Allocate a dynamic array. This macro allocates a dynamic array of records of
 * the given type. This version of the macro will set the internal parameters
 * of the SEQUENCE OF structure to NULL if the memory request cannot be
 * fulfilled.
 *
 * @param pctxt        - Pointer to a context block
 * @param pseqof       - Pointer to a generated SEQUENCE OF array structure.
 *                       The <i>n</i> member variable must be set to the number
 *                       of records to allocate.
 * @param type         - Data type of an array record
 */
#define ALLOC_ASN1ARRAY1(pctxt,pseqof,type) do {\
if (sizeof(type)*(pseqof)->n < (pseqof)->n) (pseqof)->elem = 0; \
else (pseqof)->elem = (type*) rtxMemHeapAlloc \
(&(pctxt)->pMemHeap, sizeof(type)*(pseqof)->n); \
} while (0)

/**
 * @}
 */

#define ASN1NUMOCTS(nbits)      ((nbits>0)?(((nbits-1)/8)+1):0)

#ifdef __cplusplus
}
#endif

#include "rtsrc/rtContext.h"
#include "rtxsrc/rtxCommonDefs.h"
#include "rtxsrc/rtxError.h"
#include "rtxsrc/rtxMemory.h"

/**
 * @}
 */

#endif
