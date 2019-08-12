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
 * @file rtpersrc/asn1per.h
 * ASN.1 runtime constants, data structure definitions, and functions to
 * support the Packed Encoding Rules (PER) as defined in the ITU-T X.691
 * standard.
 */
/**
 * @mainpage ASN1C PER Runtime Classes and Library Functions
 *
 * The <B>ASN.1 C++ runtime classes</B> are wrapper classes that provide
 * an object-oriented interface to the ASN.1 C runtime library functions.
 * The classes described in this manual are derived from the common
 * classes documented in the ASN1C C/C++ Common runtime manual.
 * They are specific to the Packed Encoding Rules (PER)as defined in
 * the X.691 ITU-T  standard. These PER specific C++ runtime classes
 * include the PER message buffer classes.
 *
 * The <B> ASN.1 PER Runtime Library</B> contains the low-level
 * constants, types, and functions that are assembled by the compiler
 * to encode/decode more complex structures.
 *
 * This library consists of the following items:
 *   <UL> <LI>A global include file ("asn1per.h") that is compiled into all
 * generated source files.</LI>
 *<LI> An object library of functions that are linked with the C functions
 * after compilation with a C complier.</LI></UL>
 *
 * In general, programmers will not need to be too concerned with the details
 * of these functions. The ASN.1 compiler generates calls to them in the C or
 * C++ source files that it creates. However, the functions in the library may
 * also be called on their own in applications requiring their specific
 * functionality.
 */

#ifndef _OSRTPER_H_
#define _OSRTPER_H_

#include "rtsrc/asn1type.h"
#include "rtsrc/asn1CharSet.h"
#include "rtxsrc/rtxBitEncode.h"
#include "rtxsrc/rtxBitDecode.h"
#include "rtxsrc/rtxBuffer.h"

/** @defgroup perruntime PER Runtime Library Functions.
 * @{
 * The ASN.1 Packed Encoding Rules (PER) runtime library contains the
 * low-level constants, types, and functions that are assembled by the
 * compiler to encode/decode more complex structures.
 * The PER low-level C encode/decode functions are identified by their
 * prefixes: pe_ for PER encode, pd_ for PERdecode, and pu_ for PER
 * utility functions.
 *
 */

/* The following constant is stored in an enumerated value if the       */
/* parsed value is not within the defined set of extended enum values   */
/* (note: this is only done in the case of PER because the actual value */
/* is not known - in BER, the actual value is stored)..                 */

#define ASN_K_EXTENUM   999

/* date/time format flags */
#define OSYEAR_BASIC     OSUINTCONST(0x8000000)
#define OSYEAR_PROLEPTIC OSUINTCONST(0x4000000)
#define OSYEAR_NEGATIVE  OSUINTCONST(0x2000000)
#define OSYEAR_L(n)      ((OSUINT32)(n) << 28)

#define OSYEAR_MASK \
(OSYEAR_BASIC|OSYEAR_PROLEPTIC|OSYEAR_NEGATIVE|OSYEAR_L(0xF))

#define OSANY      (OSYEAR_NEGATIVE|OSYEAR_L(5))
#define OSANY_MASK (OSYEAR_NEGATIVE|OSYEAR_L(0xF))

#define OSCENTURY  0x4000u
#define OSYEAR     0x2000u
#define OSMONTH    0x1000u
#define OSWEEK     0x0800u
#define OSDAY      0x0400u

#define OSHOURS    0x0200u
#define OSMINUTES  0x0100u
#define OSSECONDS  0x0080u
#define OSUTC      0x0040u
#define OSDIFF     0x0020u

#define OSFRACTION 0x000Fu

#define OSDURATION 0x0010u

/*
 * Structure to track encoded PER fields
 */
typedef struct PERField {
   const char* name;
   size_t      bitOffset;
   size_t      numbits;
   OSRTSList*  openTypeFields;
} PERField;

/*
 * Structure to track encoded PER fields
 */
typedef struct {
   unsigned char lb, lbm;
   char fmtBitBuffer[40], fmtHexBuffer[10], fmtAscBuffer[10];
   int  fmtBitCharIdx, fmtHexCharIdx, fmtAscCharIdx;
} BinDumpBuffer;

/* Macro to create a character set */

#define PU_SETCHARSET(csetvar, canset, abits, ubits) \
csetvar.charSet.nchars = 0; \
csetvar.canonicalSet = canset; \
csetvar.canonicalSetSize = sizeof(canset)-1; \
csetvar.canonicalSetBits = pu_bitcnt(csetvar.canonicalSetSize); \
csetvar.charSetUnalignedBits = ubits; \
csetvar.charSetAlignedBits = abits;


/* Macros */

#ifdef _TRACE
#define PU_INSLENFLD(pctxt)       pu_insLenField(pctxt)
#define PU_NEWFIELD(pctxt,suffix) pu_newField(pctxt,suffix)
#define PU_PUSHNAME(pctxt,name)   pu_pushName(pctxt,name)
#define PU_PUSHELEMNAME(pctxt,idx) pu_pushElemName(pctxt,idx)
#define PU_POPNAME(pctxt)         pu_popName(pctxt)
#define PU_SETBITOFFSET(pctxt)    pu_setFldBitOffset(pctxt)
#define PU_SETBITCOUNT(pctxt)     pu_setFldBitCount(pctxt)
#define PU_SETOPENTYPEFLDLIST(pctxt,flist) pu_setOpenTypeFldList(pctxt,flist)
#else
#define PU_INSLENFLD(pctxt)
#define PU_NEWFIELD(pctxt,suffix)
#define PU_PUSHNAME(pctxt,name)
#define PU_PUSHELEMNAME(pctxt,idx)
#define PU_POPNAME(pctxt)
#define PU_SETBITOFFSET(pctxt)
#define PU_SETBITCOUNT(pctxt)
#define PU_SETOPENTYPEFLDLIST(pctxt,flist)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SYMBIAN32__       /* For Symbian */
#define EXTPERCLASS

#ifdef BUILDASN1PERDLL
#define EXTERNPER       EXPORT_C
#define EXTPERMETHOD   EXPORT_C
#elif defined(USEASN1PERDLL)
#define EXTERNPER       IMPORT_C
#define EXTPERMETHOD   IMPORT_C
#else
#define EXTERNPER
#define EXTPERMETHOD
#endif /* BUILDASN1PERDLL */

#else                      /* Other O/S's */
#define EXTPERMETHOD

#ifdef BUILDASN1PERDLL
#define EXTERNPER       __declspec(dllexport)
#define EXTPERCLASS   __declspec(dllexport)
#elif defined (USEASN1PERDLL)
#define EXTERNPER       __declspec(dllimport)
#define EXTPERCLASS   __declspec(dllimport)
#else
#define EXTERNPER
#define EXTPERCLASS
#endif /* BUILDASN1PERDLL */

#endif

/* Function prototypes */

/* This macro is an optimized version of pd_bit function */

#define PD_BIT(pctxt,pvalue) DEC_BIT(pctxt,pvalue)

/* This macros sets the size constraint within the context QC_MOD */

#define PU_SETSIZECONSTRAINT_MACRO(pctxt,rootLower,rootUpper,extLower,extUpper) \
ACINFO(pctxt)->sizeConstraint.root.lower = rootLower; \
ACINFO(pctxt)->sizeConstraint.root.upper = rootUpper; \
ACINFO(pctxt)->sizeConstraint.ext.lower = extLower; \
ACINFO(pctxt)->sizeConstraint.ext.upper = extUpper


EXTERNPER void PU_SETSIZECONSTRAINT (OSCTXT* pctxt, OSUINT32 rootLower, OSUINT32 rootUpper, OSUINT32 extLower, OSUINT32 extUpper); 

/* This macro initializes the size constraint in the context */

#define PU_INITSIZECONSTRAINT(pctxt) PU_SETSIZECONSTRAINT(pctxt,0,0,0,0)

/* This macro is an optimized version of pu_getSizeConstraint function */

#define PU_GETSIZECONSTRAINT(pctxt,extbit) ((extbit) ? \
&ACINFO(pctxt)->sizeConstraint.ext : &ACINFO(pctxt)->sizeConstraint.root)

/* This macro calculates the relative bit offset to the current buffer
   position. */

#define PU_GETCTXTBITOFFSET(pctxt) \
(((pctxt)->buffer.byteIndex * 8) + (8 - (pctxt)->buffer.bitOffset))

/** This macro returns the number of padding bits in the last byte following
 * an encode or decode operation. */
#define PU_GETPADBITS(pctxt) \
(((pctxt)->buffer.bitOffset == 8) ? 0 : (pctxt)->buffer.bitOffset)

/* This macro sets the bit offset to the given value within the context */

#define PU_SETCTXTBITOFFSET(pctxt,_bitOffset) \
do { \
(pctxt)->buffer.byteIndex = (_bitOffset / 8); \
(pctxt)->buffer.bitOffset = (OSUINT16)(8 - (_bitOffset % 8)); \
} while(0)

/* These macros are an optimized version of pd_byte_align function */

#define PD_BYTE_ALIGN0(pctxt) \
((!(pctxt)->buffer.aligned) ? 0 : \
(((pctxt)->buffer.bitOffset != 8) ? ( \
(pctxt)->buffer.byteIndex++,       \
(pctxt)->buffer.bitOffset = 8,     \
0) : 0                    \
))

#ifdef _TRACE
#define PD_BYTE_ALIGN(pctxt) \
((!(pctxt)->buffer.aligned) ? 0 : \
(((pctxt)->buffer.bitOffset != 8) ? ( \
(pctxt)->buffer.byteIndex++,       \
(pctxt)->buffer.bitOffset = 8,     \
pu_setFldBitOffset(pctxt),         \
0) : 0                    \
))
#else
#define PD_BYTE_ALIGN PD_BYTE_ALIGN0
#endif

#define PD_CHECKSEQOFLEN(pctxt,numElements,minElemBits) \
((pctxt->buffer.size > 0) ?  \
(((numElements * minElemBits) > (pctxt->buffer.size * 8)) ? \
LOG_RTERR (pctxt,ASN_E_INVLEN) : 0) : 0)

#define PD_OPENTYPE_START(pctxt,pSavedSize,pSavedBitOff) \
pd_OpenTypeStart(pctxt,pSavedSize,pSavedBitOff);

#define PD_OPENTYPE_END(pctxt,savedSize,savedBitOff) \
pd_OpenTypeEnd(pctxt,savedSize,savedBitOff);

/** @defgroup perdecruntime PER C Decode Functions.
 * @{
 *
 * PER runtime library decode functions handle the decoding of the primitive
 * ASN.1 data types and length variables. Calls to these functions are
 * assembled in the C source code generated by the ASN1C complier to decode
 * complex ASN.1 structures. These functions are also directly callable from
 * within a user's application program if the need to decode a primitive item
 * exists.
 *
 * The procedure to decode a primitive data item is as follows:
 *   -# Call the pu_newContext or pu_initContext function to specify the
 * address of the buffer containing the encoded ASN.1 data to be decoded and
 * whether the data is aligned, or unaligned.
 *   -# Call the specific decode function to decode the value.
 */

/**
 * This function decodes a variable of the ASN.1 INTEGER type. In this case,
 * the integer is assumed to be of a larger size than can fit in a C or C++
 * long type (normally 32 or 64 bits). For example, parameters used to
 * calculate security values are typically larger than these sizes. These
 * variables are stored in character string constant variables. They are
 * represented as hexadecimal strings starting with "0x" prefix. If it is
 * necessary to convert a hexadecimal string to another radix, then use the
 * ::rtxBigIntSetStr / ::rtxBigIntToString functions.
 *
 * @param pctxt        Pointer to context block structure.
 * @param ppvalue      Pointer to a character pointer variable to receive the
 *                       decoded unsigned value. Dynamic memory is allocated
 *                       for the variable using the ::rtxMemAlloc function. The
 *                       decoded variable is represented as a decimal string
 *                       starting with no prefix.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_BigInteger (OSCTXT *pctxt, const char** ppvalue);

/**
 * This variant of the pd_BigInteger function allows the user to select
 * the radix of the decoded integer string.
 *
 * @param pctxt        Pointer to context block structure.
 * @param ppvalue      Pointer to a character pointer variable to receive the
 *                       decoded unsigned value. Dynamic memory is allocated
 *                       for the variable using the ::rtxMemAlloc function. The
 *                       decoded variable is represented as a decimal string
 *                       starting with no prefix.
 * @param radix        Radix to be used for decoded string.  Valid values
 *                       are 2, 8, 10, or 16.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_BigIntegerEx (OSCTXT *pctxt, const char** ppvalue, int radix);

/**
 * This function decodes only the value portion of an integer field.  It
 * is assume the length was decode separately.
 *
 * @param pctxt        Pointer to context block structure.
 * @param ppvalue      Pointer to a character pointer variable to receive the
 *                       decoded unsigned value. Dynamic memory is allocated
 *                       for the variable using the ::rtxMemAlloc function. The
 *                       decoded variable is represented as a decimal string
 *                       starting with no prefix.
 * @param radix        Radix to be used for decoded string.  Valid values
 *                       are 2, 8, 10, or 16.
 * @param nbytes       Length in bytes of the value component.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_BigIntegerValue
(OSCTXT *pctxt, const char** ppvalue, int radix, OSUINT32 nbytes);

/**
 * This function will decode a value of the ASN.1 bit string type whose maximum
 * size is is known in advance. The ASN1C complier generates a call to this
 * function to decode bit string productions or elements that contain a size
 * constraint.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param numbits_p    Pointer to an unsigned integer variable to receive
 *                       decoded number of bits.
 * @param buffer       Pointer to a fixed-size or pre-allocated array of bufsiz
 *                       octets to receive a decoded bit string.
 * @param bufsiz       Length (in octets) of the buffer to receive the decoded
 *                       bit string.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_BitString
(OSCTXT* pctxt, OSUINT32* numbits_p, OSOCTET* buffer,
 OSUINT32 bufsiz);

EXTERNPER int pd_BitString32
(OSCTXT* pctxt, ASN1BitStr32* pbitstr, OSUINT32 lower, OSUINT32 upper);

/**
 * This function will decode a variable of the ASN.1 BMP character string. This
 * differs from the decode routines for the character strings previously
 * described in that the BMP string type is based on 16-bit characters. A
 * 16-bit character string is modeled using an array of unsigned short
 * integers.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pvalue       Pointer to character string structure to receive the
 *                       decoded result. The structure includes a count field
 *                       containing the number of characters and an array of
 *                       unsigned short integers to hold the 16-bit character
 *                       values.
 * @param permCharSet  A pointer to the constraining character set. This
 *                       contains an array containing all valid characters in
 *                       the set as well as the aligned and unaligned bit
 *                       counts required to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_BMPString
(OSCTXT* pctxt, ASN1BMPString* pvalue, Asn116BitCharSet* permCharSet);

/**
 * This function will decode a variable of the ASN.1 32-bit character string.
 * This differs from the decode routines for the character strings previously
 * described because the universal string type is based on 32-bit characters. A
 * 32-bit character string is modeled using an array of unsigned integers.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pvalue       Pointer to character string structure to receive the
 *                       decoded result. The structure includes a count field
 *                       containing the number of characters and an array of
 *                       unsigned short integers to hold the 32-bit character
 *                       values.
 * @param permCharSet  A pointer to the constraining character set. This
 *                       contains an array containing all valid characters in
 *                       the set as well as the aligned and unaligned bit
 *                       counts required to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_UniversalString
(OSCTXT* pctxt, ASN1UniversalString* pvalue, Asn132BitCharSet* permCharSet);

/**
 * This function will position the decode bit cursor on the next byte boundary.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_byte_align (OSCTXT* pctxt);

/**
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param object_p2    A pointer to an open type variable to receive the
 *                       decoded data.
 * @param pnumocts    A pointer to an unsigned buffer of bufsiz octets to
 *                       receive decoded data.
 */
EXTERNPER int pd_ChoiceOpenTypeExt
(OSCTXT* pctxt, const OSOCTET** object_p2, OSUINT32* pnumocts);

/**
 * This function will decode an integer constrained either by a value or value
 * range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to integer variable to receive decoded value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsInteger
(OSCTXT* pctxt, OSINT32* pvalue, OSINT32 lower, OSINT32 upper);

/**
 * This function will decode an 8-bit integer constrained either by a value or
 * value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 8-bit integer variable to receive decoded
 *                       value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsInt8
(OSCTXT* pctxt, OSINT8* pvalue, OSINT32 lower, OSINT32 upper);

/**
 * This function will decode a 16-bit integer constrained either by a value or
 * value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 16-bit integer variable to receive decoded
 *                       value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsInt16 (OSCTXT* pctxt,
                            OSINT16* pvalue, OSINT32 lower, OSINT32 upper);

/**
 * This function will decode a 64-bit integer constrained either by a value or
 * value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 64-bit integer variable to receive decoded
 *                       value.
 * @param lower        Lower range value, represented as 64-bit integer.
 * @param upper        Upper range value, represented as 64-bit integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsInt64 (OSCTXT* pctxt,
                            OSINT64* pvalue, OSINT64 lower, OSINT64 upper);

/**
 * This function will decode an unsigned integer constrained either by a value
 * or value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to unsigned integer variable to receive decoded
 *                       value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsUnsigned
(OSCTXT* pctxt, OSUINT32* pvalue, OSUINT32 lower, OSUINT32 upper);

/**
 * This function will decode an 8-bit unsigned integer constrained either by a
 * value or value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 8-bit unsigned integer variable to receive
 *                       decoded value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsUInt8 (OSCTXT* pctxt,
                            OSUINT8* pvalue, OSUINT32 lower, OSUINT32 upper);

/**
 * This function will decode a 16-bit unsigned integer constrained either by a
 * value or value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 16-bit unsigned integer variable to receive
 *                       decoded value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsUInt16
(OSCTXT* pctxt, OSUINT16* pvalue, OSUINT32 lower, OSUINT32 upper);

/**
 * This function will decode a 64-bit unsigned integer constrained either by a
 * value or value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 64-bit unsigned integer variable to receive
 *                       decoded value.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConsUInt64 (OSCTXT* pctxt, OSUINT64* pvalue,
                             OSUINT64 lower, OSUINT64 upper);

/**
 * This function decodes a constrained whole number as specified in Section
 * 10.5 of the X.691 standard.
 *
 * @param pctxt             Pointer to context block structure.
 * @param padjusted_value    Pointer to unsigned adjusted integer value to
 *                             receive decoded result. To get the final value,
 *                             this value is added to the lower boundary of the
 *                             range.
 * @param range_value        Unsigned integer value specifying the total size
 *                             of the range. This is obtained by subtracting
 *                             the lower range value from the upper range
 *                             value.
 * @return                   Completion status of operation:
 *                             - 0 (0) = success,
 *                             - negative return value is error.
 */
EXTERNPER int pd_ConsWholeNumber
(OSCTXT* pctxt, OSUINT32* padjusted_value, OSUINT32 range_value);

/**
 * This function decodes a constrained whole number as specified in Section
 * 10.5 of the X.691 standard, represented as 64-bit integer.
 *
 * @param pctxt             Pointer to context block structure.
 * @param padjusted_value    Pointer to 64-bit unsigned adjusted integer value
 *                             to receive decoded result. To get the final
 *                             value, this value is added to the lower boundary
 *                             of the range.
 * @param range_value        Unsigned 64-bit integer value specifying the total
 *                             size of the range. This is obtained by
 *                             subtracting the lower range value from the upper
 *                             range value.
 * @return                   Completion status of operation:
 *                             - 0 (0) = success,
 *                             - negative return value is error.
 */
EXTERNPER int pd_ConsWholeNumber64
(OSCTXT* pctxt, OSUINT64* padjusted_value, OSUINT64 range_value);

/**
 * This function decodes a constrained string value. This is a deprecated
 * version of the function provided for backward compatibility.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string       Pointer to const char* to receive decoded string. Memory
 *                       will be allocated for this variable using internal
 *                       memory management functions.
 * @param pCharSet     Pointer to a character set descriptor structure. This
 *                       contains an array containing all valid characters in
 *                       the set as well as the aligned and unaligned bit
 *                       counts required to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConstrainedString
(OSCTXT* pctxt, const char** string, Asn1CharSet* pCharSet);

/**
 * This function decodes a constrained string value. This version of the
 * function allows all of the required permitted alphabet constraint
 * parameters to be passed in as arguments.
 *
 * @param pctxt        Pointer to context block structure.
 * @param string       Pointer to const char* to receive decoded string. Memory
 *                       will be allocated for this variable using internal
 *                       memory management functions.
 * @param charSet      String containing permitted alphabet character set. Can
 *                       be null if no character set was specified.
 * @param abits        Number of bits in a character set character (aligned).
 * @param ubits        Number of bits in a character set character (unaligned).
 * @param canSetBits   Number of bits in a character from the canonical set
 *                       representing this string.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConstrainedStringEx
(OSCTXT* pctxt, const char** string, const char* charSet,
 OSUINT32 abits, OSUINT32 ubits, OSUINT32 canSetBits);

/**
 * This function decodes a constrained string value into a fixed-size
 * buffer. This function allows all of the required permitted alphabet
 * constraint parameters to be passed in as arguments.
 *
 * @param pctxt        Pointer to context block structure.
 * @param strbuf       Pointer to character array to receive decoded string.
 * @param bufsiz       Size of strbuf character array.
 * @param charSet      String containing permitted alphabet character set. Can
 *                       be null if no character set was specified.
 * @param abits        Number of bits in a character set character (aligned).
 * @param ubits        Number of bits in a character set character (unaligned).
 * @param canSetBits   Number of bits in a character from the canonical set
 *                       representing this string.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ConstrFixedLenStringEx
(OSCTXT* pctxt, char* strbuf, size_t bufsiz, const char* charSet,
 OSUINT32 abits,  OSUINT32 ubits,  OSUINT32 canSetBits);

/**
 * This function will encode a constrained ASN.1 character string. This
 * function is normally not called directly but rather is called from Useful
 * Type Character String encode functions that deal with 16-bit strings. The
 * only function that does not release is the pe_BMPString function.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pString      Character string to be encoded. The structure includes a
 *                       count field containing the number of characters to
 *                       encode and an array of unsigned short integers to hold
 *                       the 16-bit characters to be encoded.
 * @param pCharSet     Pointer to the constraining character set. This contains
 *                       an array containing all valid characters in the set as
 *                       well as the aligned and unaligned bit counts required
 *                       to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_16BitConstrainedString
(OSCTXT* pctxt, Asn116BitCharString* pString, Asn116BitCharSet* pCharSet);

/**
 * This function will encode a constrained ASN.1 character string. This
 * function is normally not called directly but rather is called from Useful
 * Type Character String encode functions that deal with 32-bit strings. The
 * only function that does not release is the pe_UniversalString function.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pString      Character string to be encoded. The structure includes a
 *                       count field containing the number of characters to
 *                       encode and an array of unsigned short integers to hold
 *                       the 32-bit characters to be encoded.
 * @param pCharSet     Pointer to the constraining character set. This contains
 *                       an array containing all valid characters in the set as
 *                       well as the aligned and unaligned bit counts required
 *                       to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_32BitConstrainedString
(OSCTXT* pctxt, Asn132BitCharString* pString, Asn132BitCharSet* pCharSet);

/**
 * This function will decode an ISO 8601 DATE type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Pointer to string variable to receive decoded value
 *                      in string form (YYYY:MM:DD) and ect.
 * @param flags       Set of flags: OSANY|OSCENTURY|OSYEAR|OSMONTH|OSWEEK|OSDAY.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_DateStr (OSCTXT* pctxt, const char** string, OSUINT32 flags);

/**
 * This function will decode an ISO 8601 DATE-TIME type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Pointer to string variable to receive decoded value
 *                      in string form (YYYY-MM-DDTHH:MM:SS) and ect.
 * @param flags       Set of flags: OSANY|OSCENTURY|OSYEAR|OSMONTH|OSWEEK|OSDAY|
                        OSHOURS|OSMINUTES|OSSECONDS|OSUTC|OSDIFF|n.
 *                      n - set digit number of fraction part.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_DateTimeStr
(OSCTXT* pctxt, const char** string, OSUINT32 flags);

/**
 * This function will decode an ISO 8601 DURATION types.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Pointer to string variable to receive decoded value
 *                      in string form (PnYnMnDTnHnMnS).
 * @param rec         Decode recursive interval (Rn/).
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_Duration (OSCTXT* pctxt, const char** string, OSBOOL rec);

/**
 * This function will decode a variable of thr ASN.1 BIT STRING type. This
 * function allocates dynamic memory t store the decoded result. The ASN1C
 * complier generates a call to this function to decode an unconstrained bit
 * string production or element.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pBitStr      Pointer to a dynamic bit string structure to receive the
 *                       decoded result. This structure contains a field to
 *                       hold the number of decoded bits and a pointer to an
 *                       octet string to hold the decoded data. Memory is
 *                       allocated by the decoder using the rtxMemAlloc
 *                       function. This memory is tracked within the context
 *                       and released when the pu_freeContext function is
 *                       invoked.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_DynBitString
(OSCTXT* pctxt, ASN1DynBitStr* pBitStr);

/**
 * This function will decode a value of the ASN.1 octet string type whose
 * maximum size is known in advance. The ASN1C complier generates a call to
 * this function to decode octet string productions or elements that contain a
 * size constraint.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pOctStr      A pointer to a dynamic octet string to receive the
 *                       decoded result.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_DynOctetString
(OSCTXT* pctxt, ASN1DynOctStr* pOctStr);

/**
 * This function gets the offset in bits to the data field within a
 * PER-encoded binary string (i.e a BIT or OCTET STRING).
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pnumbits    A pointer to an unsigned integer to receive the
 *                       bit count value.
 * @param bitStrFlag  TRUE if type being operaton is a BIT STRING;
 *                       FALSE if OCTET STRING.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_GetBinStrDataOffset
(OSCTXT* pctxt, OSUINT32* pnumbits, OSBOOL bitStrFlag);

/**
 * This function gets the total length of a PER-encoded component.  In
 * the case of a fragmented length, it will look ahead and add up each
 * of the individual length components.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param itemBits     The size of the specific entity.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_GetComponentLength (OSCTXT* pctxt, OSUINT32 itemBits);

/**
 * This function will decode an ISO 8601 INTERVAL type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Pointer to string variable to receive decoded value
 *                      in string form (start/end).
 * @param rec         Decode recursive interval (Rn/).
 * @param startFlags  Set format flags of interval start:
                        OSANY|OSCENTURY|OSYEAR|OSMONTH|OSWEEK|OSDAY|
                        OSHOURS|OSMINUTES|OSSECONDS|OSUTC|OSDIFF|n or OSDURATION.
 *                      n - set digit number of fraction part.
 * @param endFlags    Set format flags of interval end.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_Interval
(OSCTXT* pctxt, const char** string, OSBOOL rec,
 OSUINT32 startFlags, OSUINT32 endFlags);

/**
 * This function will decode a length determinant value.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pvalue       A pointer to an unsigned integer variable to receive the
 *                       decoded length value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_Length (OSCTXT* pctxt, OSUINT32* pvalue);

/**
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param bitOffset    The bit offset inside the message buffer.
 */
/* EXTERNPER int pd_moveBitCursor (OSCTXT* pctxt, int bitOffset); */
#define pd_moveBitCursor(pctxt,bitOffset) rtxMoveBitCursor(pctxt,bitOffset)

/**
 * This function decodes a value of the ASN.1 object identifier type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to value to receive decoded result. The
 *                       ASN1OBJID structure contains an integer to hold the
 *                       number of subidentifiers and an array to hold the
 *                       subidentifier values.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_ObjectIdentifier (OSCTXT* pctxt, ASN1OBJID* pvalue);

/**
 * This function decodes a value of the ASN.1 object identifier type using
 * 64-bit subidentifiers.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to value to receive decoded result. The
 *                       ASN1OID64 structure contains an integer to hold the
 *                       number of subidentifiers and an array of 64-bit
 *                       unsigned integers to hold the subidentifier values.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_oid64 (OSCTXT* pctxt, ASN1OID64* pvalue);

/**
 * This function decodes a value of the ASN.1 RELATIVE-OID type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to value to receive decoded result. The
 *                       ASN1OBJID structure contains an integer to hold the
 *                       number of subidentifiers and an array to hold the
 *                       subidentifier values.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_RelativeOID (OSCTXT* pctxt, ASN1OBJID* pvalue);

/**
 * This function will decode a value of the ASN.1 octet string type whose
 * maximun size is known in advance. The ASN1C compiler generates a call to
 * this function to decode octet string productions or elements that contain a
 * size constraint.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pnumocts    A pointer to an unsigned buffer of bufsiz octets to
 *                       receive decoded data.
 * @param buffer       A pointer to a pre-allocated buffer of size octets to
 *                       receive the decoded data.
 * @param bufsiz       The size of the buffer to receive the decoded result.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_OctetString
(OSCTXT* pctxt, OSUINT32* pnumocts, OSOCTET* buffer,
 OSUINT32 bufsiz);

/**
 * This function will decode an ASN.1 open type. This used to be the ASN.1 ANY
 * type, but now is used in a variety of applications requiring an encoding that
 * can be interpreted by a decoder without prior knowledge of the type of the
 * variable.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pnumocts    A pointer to an unsigned buffer of bufsiz octets to
 *                       receive decoded data.
 * @param object_p2    A pointer to an open type variable to receive the
 *                       decoded data.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_OpenType
(OSCTXT* pctxt, const OSOCTET** object_p2, OSUINT32* pnumocts);

/**
 * This function will decode an ASN.1 open type extension. These are extra
 * fields in a version-2 message that may be present after the ... extension
 * marker. An open type structure (extElem1) is added to a message structure
 * that contains an extension marker but no extension elements. The
 * pd_OpenTypeExt function will populate this structure with the complete
 * extension information (optional bit or choice index, length and data). A
 * subsequent call to pe_OpenTypeExt will cause the saved extension fields to
 * be included in a newly encoded message of the given type.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param object_p2    A pointer to an open type variable to receive the
 *                       decoded data.
 * @param pnumocts    A pointer to an unsigned buffer of bufsiz octets to
 *                       receive decoded data.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_OpenTypeExt
(OSCTXT* pctxt, const OSOCTET** object_p2, OSUINT32* pnumocts);

#ifndef _NO_ASN1REAL
/**
 * This function will decode a value of the binary encoded ASN.1 real type.
 * This function provides support for the plus-infinity special real values.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all workings variables
 *                       that must be maintained between function calls.
 * @param pvalue       Pointer to an real variable to receive decoded
 *                       value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_Real (OSCTXT* pctxt, OSREAL* pvalue);
#endif

/**
 * This function will decode a normally small length determinant as specified
 * in 11.9 of the X.691 standard. This is a number that is expected to be
 * small, but whose size is potentially unlimited due to the presence of an
 * extension maker.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all workings variables
 *                       that must be maintained between function calls.
 * @param pvalue       Pointer to an unsigned integer value to receive decoded
 *                       results.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_SmallLength
(OSCTXT* pctxt, OSUINT32* pvalue);

/**
 * This function will decode a small non-negative whole number as specified in
 * Section 10.6 of the X.691 standard. This is a number that is expected to be
 * small, but whose size is potentially unlimited due to the presence of an
 * extension maker.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all workings variables
 *                       that must be maintained between function calls.
 * @param pvalue       Pointer to an unsigned integer value to receive decoded
 *                       results.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_SmallNonNegWholeNumber
(OSCTXT* pctxt, OSUINT32* pvalue);

/**
 * This function will decode a semi-constrained integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to integer variable to receive decoded value.
 * @param lower        Lower range value, represented as signed
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_SemiConsInteger
   (OSCTXT* pctxt, OSINT32* pvalue, OSINT32 lower);

/**
 * This function will decode a semi-constrained unsigned integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to unsigned integer variable to receive decoded
 *                       value.
 * @param lower        Lower range value, represented as unsigned
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_SemiConsUnsigned
   (OSCTXT* pctxt, OSUINT32* pvalue, OSUINT32 lower);

/**
 * This function will decode a semi-constrained 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 64-bit integer variable to receive decoded
 *                       value.
 * @param lower        Lower range value, represented as signed 64-bit
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_SemiConsInt64
   (OSCTXT* pctxt, OSINT64* pvalue, OSINT64 lower);

/**
 * This function will decode a semi-constrained unsigned 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to unsigned 64-bit integer variable to receive
 *                       decoded value.
 * @param lower        Lower range value, represented as unsigned 64-bit
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_SemiConsUInt64
   (OSCTXT* pctxt, OSUINT64* pvalue, OSUINT64 lower);

/**
 * This function will decode ISO 8601 TIME types.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Pointer to string variable to receive decoded value
 *                      in string form (HH:MM:SS) and ect.
 * @param flags       Set of flags: OSHOURS|OSMINUTES|OSSECONDS|OSUTC|OSDIFF|n.
 *                      n - set digit number of fraction part.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_TimeStr (OSCTXT* pctxt, const char** string, OSUINT32 flags);

/**
 * This function will decode an unconstrained integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue      Pointer to integer variable to receive decoded value.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_UnconsInteger (OSCTXT *pctxt, OSINT32* pvalue);

/**
 * This function will decode an unconstrained length value or a length
 * value with upper bound > 64k.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue      Pointer to integer variable to receive decoded value.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_UnconsLength (OSCTXT *pctxt, OSUINT32* pvalue);

/**
 * This function will decode an unconstrained integer into an unsigned type.
 * An error is returned if the value to be decoded cannot be represented by
 * *pvalue (it is either negative or too large).
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue      Pointer to unsigned integer variable to receive decoded
 *                       value.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_UnconsUnsigned (OSCTXT* pctxt, OSUINT32* pvalue);


/**
 * This function will decode an unconstrained 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to 64-bit integer variable to receive decoded
 *                       value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_UnconsInt64 (OSCTXT *pctxt, OSINT64* pvalue);

/**
 * This function will decode an unconstrained integer into an
 * unsigned 64-bit integer.
 * An error is returned if the value being decoded cannot be represented by
 * *pvalue (it is negative or too large).
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to unsigned 64-bit integer variable to receive
 *                       decoded value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_UnconsUInt64 (OSCTXT* pctxt, OSUINT64* pvalue);

/**
 * This function will decode a variable of the ASN.1 character string.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue      Pointer to a character pointer variable to receive the
 *                       decoded result. Dynamic memory is allocated
 *                       for the variable using the ::rtxMemAlloc function.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_VarWidthCharString (OSCTXT* pctxt, const char** pvalue);

/**
 * This function will decode an ISO 8601 YEAR type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue      Pointer to integer variable to receive decoded value.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_YearInt (OSCTXT* pctxt, OSINT32* pvalue);

#ifndef _NO_ASN1REAL
/**
 * This function will decode a value of the decimal encoded ASN.1 real type.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all workings variables
 *                       that must be maintained between function calls.
 * @param ppvalue      Pointer to a character pointer variable to receive the
 *                       decoded result. Dynamic memory is allocated
 *                       for the variable using the ::rtxMemAlloc function.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pd_Real10 (OSCTXT *pctxt, const char** ppvalue);
#endif

/**
 * This function peeks at the open type length to determine if it is fragmented.
 *
 * @param pctxt       Pointer to a context structure.
 */
EXTERNPER OSBOOL pd_isFragmented (OSCTXT* pctxt);

EXTERNPER void pd_OpenTypeStart
(OSCTXT* pctxt, OSUINT32* pSavedSize, OSINT16* pSavedBitOff);

EXTERNPER int pd_OpenTypeEnd
(OSCTXT* pctxt, OSUINT32 savedSize, OSINT16 savedBitOff);

/**
 * This function decodes a constrained string value. This version supports
 * unaligned PER only. It allows all of the required permitted alphabet
 * constraint parameters to be passed in as arguments.
 *
 * @param pctxt        Pointer to context block structure.
 * @param string       Pointer to const char* to receive decoded string. Memory
 *                       will be allocated for this variable using internal
 *                       memory management functions.
 * @param charSet      String containing permitted alphabet character set. Can
 *                       be null if no character set was specified.
 * @param nbits        Number of bits in a character set character (unaligned).
 * @param canSetBits   Number of bits in a character from the canonical set
 *                       representing this string.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int uperDecConstrString
(OSCTXT* pctxt, const char** string, const char* charSet,
 OSUINT32 nbits, OSUINT32 canSetBits);

/**
 * This function decodes a constrained string value into a fixed-size
 * buffer. This version supports unaligned PER only. It allows all of the
 * required permitted alphabet constraint parameters to be passed in as
 * arguments.
 *
 * @param pctxt        Pointer to context block structure.
 * @param strbuf       Pointer to character array to receive decoded string.
 * @param bufsiz       Size of strbuf character array.
 * @param charSet      String containing permitted alphabet character set. Can
 *                       be null if no character set was specified.
 * @param nbits        Number of bits in a character set character (unaligned).
 * @param canSetBits   Number of bits in a character from the canonical set
 *                       representing this string.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int uperDecConstrFixedLenString
(OSCTXT* pctxt, char* strbuf, size_t bufsiz, const char* charSet,
 OSUINT32 nbits,  OSUINT32 canSetBits);

/** @} perdecruntime */

/* Encode functions */
/** @defgroup perencruntime PER C Encode Functions.
 * @{
 *
 * The Per low-level encode functions handle the PER encoding of the primitive
 * ASN.1 data types. Calls to these functions are assembled in the C source
 * code generated by the ASN1C complier to accomplish the encoding of complex
 * ASN.1 structures. These functions are also directly callable from within a
 * user's application program if the need to accomplish a low level encoding
 * function exists.
 *
 * The procedure to call a low-level encode function is the same as the
 * procedure to call a compiler generated encode function described above. The
 * pu_newContext function must first be called to set a pointer to the buffer
 * into which the variable is to be encoded. A static encode buffer is
 * specified by assigning a pointer to a buffer and a buffer size. Setting the
 * buffer address to NULL and buffer size to 0 specifies a dynamic buffer. The
 * encode function is then invoked. The result of the encoding will start at
 * the beginning of the specified buffer, or, if a dynamic buffer was used,
 * only be obtained by calling pe_GetMsgPtr. The length of the encoded compound
 * is obtained by calling pe_GetMsgLen.
 */

/**
 * This function will encode a constrained ASN.1 character string. This
 * function is normally not called directly but rather is called from Useful
 * Type Character String encode functions that deal with 16-bit strings. The
 * only function that does that in this release is the pe_BMPString function.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        Character string to be encoded. The structure includes a
 *                       count field containing the number of characters to
 *                       encode and an array of unsigned short integers to hold
 *                       the 16-bit characters to be encoded.
 * @param pCharSet     Pointer to the constraining character set. The contains
 *                       an array containing all valid characters in the set as
 *                       well as the aligned and unaligned bit counts required
 *                       to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_16BitConstrainedString
(OSCTXT* pctxt, Asn116BitCharString value, Asn116BitCharSet* pCharSet);

/**
 * This function will encode a constrained ASN.1 character string. This
 * function is normally not called directly but rather is called from Useful
 * Type Character String encode functions that deal with 32-bit strings. The
 * only function that does that in this release is the pe_UniversalString
 * function.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        Character string to be encoded. The structure includes a
 *                       count field containing the number of characters to
 *                       encode and an array of unsigned short integers to hold
 *                       the 32-bit characters to be encoded.
 * @param pCharSet     Pointer to the constraining character set. The contains
 *                       an array containing all valid characters in the set as
 *                       well as the aligned and unaligned bit counts required
 *                       to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_32BitConstrainedString
(OSCTXT* pctxt, Asn132BitCharString value, Asn132BitCharSet* pCharSet);

/**
 * This function encodes a two's complement binary integer as specified in
 * Section 10.4 of the X.691 standard.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Signed integer value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_2sCompBinInt
(OSCTXT* pctxt, OSINT32 value);

/**
 * This function encodes a two's complement binary 64-bit integer as specified
 * in Section 10.4 of the X.691 standard.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Signed 64-bit integer value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_2sCompBinInt64 (OSCTXT* pctxt, OSINT64 value);

/**
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       A pointer to a character string containing the value to
 *                       be encoded.
 * @param nocts        The number of octets.
 */
EXTERNPER int pe_aligned_octets
(OSCTXT* pctxt, OSOCTET* pvalue, OSUINT32 nocts);

/**
 * The pe_BigInteger function will encode a variable of the ASN.1 INTEGER type.
 * In this case, the integer is assumed to be of a larger size than can fit in
 * a C or C++ long type (normally 32 or 64 bits). For example, parameters used
 * to calculate security values are typically larger than these sizes. Items of
 * this type are stored in character string constant variables. They can be
 * represented as decimal strings (with no prefixes), as hexadecimal strings
 * starting with a "0x" prefix, as octal strings starting with a "0o" prefix or
 * as binary strings starting with a "0b" prefix. Other radixes currently are
 * not supported. It is highly recommended to use the hexadecimal or binary
 * strings for better performance.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       A pointer to a character string containing the value to
 *                       be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_BigInteger
(OSCTXT* pctxt, const char* pvalue);

/**
 * This function will encode a varialbe of the ASN.1 BOOLEAN type in single
 * bit,
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        The BOOLEAN value to be encoded.
 */
/* EXTERNPER int pe_bit
(OSCTXT* pctxt, OSBOOL value); */
#define pe_bit(pctxt,value) rtxEncBit(pctxt,value)

/**
 * This function encodes multiple bits.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Unsigned integer containing the bits to be encoded.
 * @param nbits        Number of bits in value to encode.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
/* EXTERNPER int pe_bits
(OSCTXT* pctxt, OSUINT32 value, OSUINT32 nbits); */
#define pe_bits(pctxt,value,nbits) rtxEncBits(pctxt,value,nbits)

/**
 * This function encodes multiple bits, using unsigned 64-bit integer to hold
 * bits.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Unsigned 64-bit integer containing the bits to be
 *                       encoded.
 * @param nbits        Number of bits in value to encode.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_bits64 (OSCTXT* pctxt, OSUINT64 value, OSUINT32 nbits);

/**
 * This function will encode a value of the ASN.1 bit string type.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param numocts      The number of bits n the string to be encoded.
 * @param data         Pointer to the bit string data to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_BitString
(OSCTXT* pctxt, OSUINT32 numocts, const OSOCTET* data);

EXTERNPER int pe_BitString32
(OSCTXT* pctxt, ASN1BitStr32* pbitstr, OSUINT32 lower, OSUINT32 upper);

/**
 * This function will encode a variable of the ASN.1 BMP character string. This
 * differs from the encode routines for the character strings previously
 * described in that the BMP string type is based on 16-bit characters. A
 * 16-bit character string is modeled using an array of unsigned short
 * integers.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param value        Character string to be encoded. This structure includes
 *                       a count field containing the number of characters to
 *                       encode and an array of unsigned short integers to hold
 *                       the 16-bit characters to be encoded.
 * @param permCharSet  Pointer to the constraining character set. This contains
 *                       an array containing all valid characters in the set as
 *                       well as the aligned and unaligned bit counts required
 *                       to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_BMPString
(OSCTXT* pctxt, ASN1BMPString value, Asn116BitCharSet* permCharSet);

/**
 * This function will encode a variable of the ASN.1 Universal character
 * string. This differs from the encode routines for the character strings
 * previously described in that the Universal string type is based on 32-bit
 * characters. A 32-bit character string is modeled using an array of unsigned
 * integers.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param value        Character string to be encoded. The structure includes a
 *                       count field containing all valid characters in the set
 *                       as well as the aligned and unaligned bit counts
 *                       required to encode the characters.
 * @param permCharSet  A pointer to the constraining character set. This
 *                       contains an array containing all valid characters in
 *                       the set as well as the aligned and the unaligned bit
 *                       counts required to encode the characters.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_UniversalString
(OSCTXT* pctxt, ASN1UniversalString value, Asn132BitCharSet* permCharSet);

/**
 * This function will position the encode bit cursor on the next byte boundry.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_byte_align (OSCTXT* pctxt);

/**
 * This function will determine if the given number of bytes will fit in the
 * encode buffer. If not, either the buffer is expanded (if it is a dynamic
 * buffer) or an error is signaled.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param nbytes       Number of bytes of space required to hold the variable
 *                       to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
/* EXTERNPER int pe_CheckBuffer (OSCTXT* pctxt, size_t nbytes); */
#define pe_CheckBuffer(pctxt,nbytes) rtxCheckOutputBuffer(pctxt,nbytes)

/**
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param numocts      Number of octets in the string to be encoded.
 * @param data         Pointer to octet string data to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ChoiceTypeExt
(OSCTXT* pctxt, OSUINT32 numocts, const OSOCTET* data);

/**
 * This function encodes an integer constrained either by a value or value
 * range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ConsInteger
(OSCTXT* pctxt, OSINT32 value, OSINT32 lower, OSINT32 upper);

/**
 * This function encodes a 64-bit integer constrained either by a value or
 * value range constraint.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded, represented as 64-bit integer.
 * @param lower        Lower range value, represented as 64-bit integer.
 * @param upper        Upper range value, represented as 64-bit integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ConsInt64 (OSCTXT* pctxt,
                            OSINT64 value, OSINT64 lower, OSINT64 upper);

/**
 * This function encodes a constrained string value. This is a deprecated
 * version of the function provided for backward compatibility.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string       Pointer to string to be encoded.
 * @param pCharSet     Pointer to a character set descriptor structure.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ConstrainedString
(OSCTXT* pctxt, const char* string, Asn1CharSet* pCharSet);

/**
 * This function encodes a constrained string value. This version of the
 * function allows all of the required permitted alphabet constraint parameters
 * to be passed in as arguments.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string       Pointer to string to be encoded.
 * @param charSet      String containing permitted alphabet character set. Can
 *                       be null if no character set was specified.
 * @param abits        Number of bits in a character set character (aligned).
 * @param ubits        Number of bits in a character set character (unaligned).
 * @param canSetBits   Number of bits in a character from the canonical set
 *                       representing this string.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ConstrainedStringEx
(OSCTXT* pctxt, const char* string, const char* charSet,
 OSUINT32 abits, OSUINT32 ubits, OSUINT32 canSetBits);

/**
 * This function encodes an unsigned integer constrained either by a value or
 * value range constraint. The constrained unsigned integer option is used if:
 *
 * 1. The lower value of the range is >= 0, and 2. The upper value of the range
 * is >= MAXINT
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded.
 * @param lower        Lower range value.
 * @param upper        Upper range value.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ConsUnsigned
(OSCTXT* pctxt, OSUINT32 value, OSUINT32 lower, OSUINT32 upper);

/**
 * This function encodes an unsigned 64-bit integer constrained either by a
 * value or value range constraint. The constrained unsigned integer option is
 * used if:
 *
 * 1. The lower value of the range is >= 0, and 2. The upper value of the range
 * is >= MAXINT
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded, represented as unsigned 64-bit
 *                       integer.
 * @param lower        Lower range value, represented as unsigned 64-bit
 *                       integer.
 * @param upper        Upper range value, represented as unsigned 64-bit
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ConsUInt64 (OSCTXT* pctxt, OSUINT64 value,
                             OSUINT64 lower, OSUINT64 upper);

/**
 * This function encodes a constrained whole number as specified in Section
 * 10.5 of the X.691 standard.
 *
 * @param pctxt            Pointer to context block structure.
 * @param adjusted_value    Unsigned adjusted integer value to be encoded. The
 *                            adjustment is done by subtracting the lower value
 *                            of the range from the value to be encoded.
 * @param range_value       Unsigned integer value specifying the total size of
 *                            the range. This is obtained by subtracting the
 *                            lower range value from the upper range value.
 * @return                  Completion status of operation:
 *                            - 0 (0) = success,
 *                            - negative return value is error.
 */
EXTERNPER int pe_ConsWholeNumber
(OSCTXT* pctxt, OSUINT32 adjusted_value, OSUINT32 range_value);

/**
 * This function encodes a constrained whole number as specified in Section
 * 10.5 of the X.691 standard, represented as 64-bit integer.
 *
 * @param pctxt            Pointer to context block structure.
 * @param adjusted_value    Unsigned adjusted integer value to be encoded. The
 *                            adjustment is done by subtracting the lower value
 *                            of the range from the value to be encoded.
 * @param range_value       Unsigned integer value specifying the total size of
 *                            the range. This is obtained by subtracting the
 *                            lower range value from the upper range value.
 * @return                  Completion status of operation:
 *                            - 0 (0) = success,
 *                            - negative return value is error.
 */
EXTERNPER int pe_ConsWholeNumber64
(OSCTXT* pctxt, OSUINT64 adjusted_value, OSUINT64 range_value);

/**
 * This function will encode an ISO 8601 DATE types.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Character string variable containing value to be
 *                      encoded in string form (YYYY:MM:DD) and ect.
 * @param flags       Set of flags: OSANY|OSCENTURY|OSYEAR|OSMONTH|OSWEEK|OSDAY.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_DateStr (OSCTXT* pctxt, const char* string, OSUINT32 flags);

/**
 * This function will encode an ISO 8601 DATE-TIME types.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Character string variable containing value to be
 *                      encoded in string form (YYYY-MM-DDTHH:MM:SS) and ect.
 * @param flags       Set of flags: OSANY|OSCENTURY|OSYEAR|OSMONTH|OSWEEK|OSDAY|
                        OSHOURS|OSMINUTES|OSSECONDS|OSUTC|OSDIFF|n.
 *                      n - set digit number of fraction part.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_DateTimeStr
(OSCTXT* pctxt, const char* string, OSUINT32 flags);

/**
 * This function will encode an ISO 8601 DURATION types.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Character string variable containing value to be
 *                      encoded in string form (PnYnMnDTnHnMnS).
 * @param rec         Encode recursive interval (Rn/).
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_Duration (OSCTXT* pctxt, const char* string, OSBOOL rec);

/**
 * @param value        Length value to be encoded.
 */
EXTERNPER OSUINT32 pe_GetIntLen (OSUINT32 value);

/**
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 */
EXTERNPER size_t pe_GetMsgBitCnt (OSCTXT* pctxt);

/**
 * This function will return the message pointer and length of an encoded
 * message. This function is called after a complier generated encode function
 * to get the pointer and length of the message. It is normally used when
 * dynamic encoding is specified because the message pointer is not known until
 * encoding is complete. If static encoding is used, the message starts at the
 * beginning of the specified buffer adn the pe_GetMsgLen function can be used
 * to obtain the lenght of the message.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pLength      Pointer to variable to receive length of the encoded
 *                       message.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER OSOCTET* pe_GetMsgPtr (OSCTXT* pctxt, OSINT32* pLength);

/**
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pLength      Pointer to variable to receive length of the encoded
 *                       message.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER OSOCTET* pe_GetMsgPtrU (OSCTXT* pctxt, OSUINT32* pLength);

/**
 * This function will encode an ISO 8601 INTERVAL type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Character string variable containing value to be
 *                      encoded in string form (start/end).
 * @param rec         Encode recursive interval (Rn/).
 * @param startFlags  Set format flags of interval start:
 *                      OSANY|OSCENTURY|OSYEAR|OSMONTH|OSWEEK|OSDAY|
 *                      OSHOURS|OSMINUTES|OSSECONDS|OSUTC|OSDIFF|n
 *                      or OSDURATION.
 *                      n - set digit number of fraction part.
 * @param endFlags    Set format flags of interval end.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_Interval
(OSCTXT* pctxt, const char* string, OSBOOL rec,
 OSUINT32 startFlags, OSUINT32 endFlags);

/**
 * This function will encode a length determinant value.
 *
 * @param pctxt       Pointer to a context structure. This provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        Length value to be encoded.
 * @return             If successful, the encoded number of bytes is returned;
 *                       otherwise, a negative error status code.
 */
EXTERNPER int pe_Length (OSCTXT* pctxt, OSUINT32 value);

/**
 * This function encodes a non-negative binary integer as specified in Section
 * 10.3 of the X.691 standard.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Unsigned integer value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_NonNegBinInt (OSCTXT* pctxt, OSUINT32 value);

/**
 * This function encodes a non-negative binary 64-bit integer as specified in
 * Section 10.3 of the X.691 standard.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Unsigned 64-bit integer value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_NonNegBinInt64 (OSCTXT* pctxt, OSUINT64 value);

/**
 * This function encodes a value of the ASN.1 object identifier type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to value to be encoded. The ASN1OBJID structure
 *                       contains a numids fields to hold the number of
 *                       subidentifiers and an array to hold the subidentifier
 *                       values.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_ObjectIdentifier (OSCTXT* pctxt, ASN1OBJID* pvalue);

#ifndef _NO_INT64_SUPPORT
/**
 * This function encodes a value of the ASN.1 object identifier type, using
 * 64-bit subidentifiers.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to value to be encoded. The ASN1OID64 structure
 *                       contains a numids fields to hold the number of
 *                       subidentifiers and an array of unsigned 64-bit
 *                       integers to hold the subidentifier values.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_oid64 (OSCTXT* pctxt, ASN1OID64* pvalue);
#endif

/**
 * This function encodes a value of the ASN.1 RELATIVE-OID type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param pvalue       Pointer to value to be encoded. The ASN1OBJID structure
 *                       contains a numids fields to hold the number of
 *                       subidentifiers and an array to hold the subidentifier
 *                       values.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_RelativeOID (OSCTXT* pctxt, ASN1OBJID* pvalue);

/**
 * This fuction will encode an array of octets. The Octets will be encoded
 * unaligned starting at the current bit offset within the encode buffer.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pvalue       A pointer to an array of octets to encode
 * @param nbits        The number of Octets to encode
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
/* EXTERNPER int pe_octets
(OSCTXT* pctxt, const OSOCTET* pvalue, OSUINT32 nbits); */
#define pe_octets(pctxt,pvalue,nbits) \
rtxEncBitsFromByteArray(pctxt,pvalue,nbits)

/**
 * This function will encode a value of the ASN.1 octet string type.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param numocts      Number of octets in the string to be encoded.
 * @param data         Pointer to octet string data to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_OctetString
(OSCTXT* pctxt, OSUINT32 numocts, const OSOCTET* data);

/**
 * This function will encode an ASN.1 open type. This used to be the ANY type,
 * but now is used in a variety of applications requiring an encoding that can
 * be interpreted by a decoder without a prior knowledge of the type of the
 * variable.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param numocts      Number of octets in the string to be encoded.
 * @param data         Pointer to octet string data to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_OpenType
(OSCTXT* pctxt, OSUINT32 numocts, const OSOCTET* data);

/**
 * This function will finish encoding extension in ASN.1 open type.
 * It will write open type length to saved position. If required function do
 * fragmentation of open type data.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pos          Position that was saved in pe_OpenTypeStart function.
 * @param pPerField    A pointer to bit field that was saved in pe_OpenTypeStart
 *                       function.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_OpenTypeEnd (OSCTXT* pctxt, OSUINT32 pos, void* pPerField);

/**
 * This function will encode an ASN.1 open type extension. An open type
 * extension field is the data that potentially resides after the ... marker in
 * a version-1 message. The open type structure contains a complete encoded bit
 * set including option element bits or choice index, length, and data.
 * Typically, this data is populated when a version-1 system decodes a
 * version-2 message. The extension fields are retained and can then be
 * re-encoded if a new message is to be sent out (for example, in a store and
 * forward system).
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pElemList    A pointer to the open type to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_OpenTypeExt
(OSCTXT* pctxt, OSRTDList* pElemList);

/**
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pElemList    A pointer to the open type to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_OpenTypeExtBits
(OSCTXT* pctxt, OSRTDList* pElemList);

/**
 * This function will start encoding extension in ASN.1 open type.
 * It will reserve place to open type length and return current buffer
 * position.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pPos         A pointer to return current buffer position.
 * @param ppPerField   A pointer to to return current bit field record.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_OpenTypeStart
   (OSCTXT* pctxt, OSUINT32* pPos, void** ppPerField);

#ifndef _NO_ASN1REAL
/**
 * This function will encode a value of the ASN.1 real type. This function
 * provides support for the plus-infinity and minus-infinity special real
 * values. Use the rtxGetPlusInfinity or rtxGetMinusInfinity functions to get
 * these special values.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        Value to be encoded. Special real values plus and minus
 *                       infinity are encoded by using the rtxGetPlusInfinity
 *                       and the rtxGetMinusInfinity functions to se the real
 *                       value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_Real (OSCTXT* pctxt, OSREAL value);
#endif

/**
 * This function will endcode a small, non-negative whole number as specified
 * in Section 10.6 of teh X.691 standard. This is a number that is expected to
 * be small, but whose size is potentially unlimited due to the presence of an
 * extension marker.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        An unsigned integer value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_SmallNonNegWholeNumber (OSCTXT* pctxt, OSUINT32 value);


/**
 * This function will encode a normally small length as specified
 * in Section 11.9 of the X.691 standard. This is a number that is expected to
 * be small, but whose size is potentially unlimited due to the presence of an
 * extension marker.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        An unsigned integer value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_SmallLength (OSCTXT* pctxt, OSUINT32 value);


/**
 * This function encodes an semi-constrained integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded.
 * @param lower        Lower range value, represented as signed
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_SemiConsInteger
   (OSCTXT* pctxt, OSINT32 value, OSINT32 lower);

#ifndef _NO_INT64_SUPPORT
/**
 * This function encodes an semi-constrained 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded, represented as 64-bit integer.
 * @param lower        Lower range value, represented as signed 64-bit
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_SemiConsInt64
   (OSCTXT* pctxt, OSINT64 value, OSINT64 lower);
#endif

/**
 * This function encodes an semi-constrained unsigned integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded.
 * @param lower        Lower range value, represented as unsigned
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_SemiConsUnsigned
   (OSCTXT* pctxt, OSUINT32 value, OSUINT32 lower);

#ifndef _NO_INT64_SUPPORT
/**
 * This function encodes an semi-constrained unsigned 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded, represented as unsigned 64-bit
 *                       integer.
 * @param lower        Lower range value, represented as unsigned 64-bit
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_SemiConsUInt64
   (OSCTXT* pctxt, OSUINT64 value, OSUINT64 lower);
#endif

/**
 * This function will encode an ISO 8601 TIME types.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Character string variable containing value to be
 *                      encoded in string form (HH:MM:SS) and ect.
 * @param flags       Set of flags: OSHOURS|OSMINUTES|OSSECONDS|OSUTC|OSDIFF|n.
 *                      n - set digit number of fraction part.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_TimeStr (OSCTXT* pctxt, const char* string, OSUINT32 flags);

/**
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value        Value to be encoded.
 * @return             If successful, the encoded number of bytes is returned;
 *                       otherwise, a negative error status code.
 */
EXTERNPER int pe_UnconsLength (OSCTXT* pctxt, OSUINT32 value);

/**
 * This function encodes an unconstrained integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_UnconsInteger (OSCTXT* pctxt, OSINT32 value);

#ifndef _NO_INT64_SUPPORT
/**
 * This function encodes an unconstrained 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded, represented as 64-bit integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_UnconsInt64 (OSCTXT* pctxt, OSINT64 value);
#endif

/**
 * This function encodes an unconstrained unsigned integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_UnconsUnsigned (OSCTXT* pctxt, OSUINT32 value);

#ifndef _NO_INT64_SUPPORT
/**
 * This function encodes an unconstrained unsigned 64-bit integer.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value        Value to be encoded, represented as unsigned 64-bit
 *                       integer.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_UnconsUInt64 (OSCTXT* pctxt, OSUINT64 value);
#endif

/**
 * This function will encode a ASN.1 character string.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param value       A pointer to a character string containing the value to
 *                       be encoded.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_VarWidthCharString (OSCTXT* pctxt, const char* value);

/**
 * This function will encode an ISO 8601 YEAR type.
 *
 * @param pctxt       Pointer to context block structure.
 * @param value       Value to be encoded.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_YearInt (OSCTXT* pctxt, OSINT32 value);

#ifndef _NO_ASN1REAL
/**
 * This function will encode a number from character string to ASN.1 real type
 * using decimal encoding. Number may be represented in integer, decimal, and
 * exponent formats.
 *
 * @param pctxt       A pointer to a context structure. The provides a storage
 *                       area for the function to store all working variables
 *                       that must be maintained between function calls.
 * @param pvalue       Value to be encoded.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pe_Real10 (OSCTXT* pctxt, const char* pvalue);
#endif

/**
 * This function encodes a constrained string value. This version supports
 * unaligned PER only. It allows all of the required permitted alphabet
 * constraint parameters to be passed in as arguments.
 *
 * @param pctxt       Pointer to context block structure.
 * @param string      Pointer to string to be encoded.
 * @param charSet     String containing permitted alphabet character set. Can
 *                       be null if no character set was specified.
 * @param nbits       Number of bits in a character set character (unaligned).
 * @param canSetBits  Number of bits in a character from the canonical set
 *                       representing this string.
 * @return            Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int uperEncConstrString
(OSCTXT* pctxt, const char* string, const char* charSet,
 OSUINT32 nbits, OSUINT32 canSetBits);

/** @} perencruntime */

/* Utility functions */


/**
 * @defgroup perutil PER C Utility Functions
 * @{
 *
 * The PER utility functions are common routines used by both the PER encode
 * and decode functions.
 */

/**
 * This function is used to add size to a context variable.
 *
 * @param pctxt       A pointer to a context structure. The referenced size
 *                       constraint is added to this structure for use by a
 *                       subsequent encode or decode function.
 * @param pSize        A pointer to the size constraint to add the context
 *                       variable.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pu_addSizeConstraint (OSCTXT* pctxt, const Asn1SizeCnst* pSize);

EXTERNPER OSBOOL pu_alignCharStr
(OSCTXT* pctxt, OSUINT32 len, OSUINT32 nbits, Asn1SizeCnst* pSize);

EXTERNPER OSUINT32 pu_bitcnt (OSUINT32 value);

EXTERNPER Asn1SizeCnst* pu_checkSize
(Asn1SizeCnst* pSizeList, OSUINT32 value, OSBOOL* pExtendable);

/**
 * This function checks if the given value falls within the root or
 * extension part of the given size constraint.
 *
 * @param pSizeCnst    A pointer to a size constraint structure.
 * @param value        The value to be checked.
 * @param pExtendable  Pointer to boolean indicating if size constraint is
 *                       extensible.
 * @param pSizeRange   Size range which value falls within.
 * @param pExtSize     Indicates if the size range is an extension range.
 * @return             Status of the operation.  0 = success or RTERR_CONSVIO
 *                       if size if not within the constraint bounds.
 */
EXTERNPER int pu_checkSizeExt
(Asn1SizeCnst* pSizeCnst, OSUINT32 value, OSBOOL* pExtendable,
 Asn1SizeValueRange* pSizeRange, OSBOOL* pExtSize);

/**
 * This function releases all dynamicmemeory associated with a context. This
 * function should be called even if the referenced context variable is not
 * dynamic. The reason is because it frees memory allocated within the context
 * as well as the context structure (it will only try to free the context
 * structure if it detects that it was previously allocated using the
 * pu_newContext function).
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 */
EXTERNPER void pu_freeContext (OSCTXT* pctxt);

EXTERNPER size_t pu_getMaskAndIndex (size_t bitOffset, unsigned char* pMask);

/**
 * This function will return the number of bytes in a PER message. This
 * function is called after a complier generated encode function is called to
 * get the byte count of the encoded component.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 *
 * @see ::pu_getMsgLenBits
 *
 * @return             Length (in bytes) of encoded message content.
 */
EXTERNPER size_t pu_getMsgLen (OSCTXT* pctxt);

/**
 * This function will return the number of bits in a PER message. This
 * function is called after a complier generated encode function is called to
 * get the bit count of the encoded component.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 *
 * @see ::pu_getMsgLen
 *
 * @return             Length (in bits) of encoded message content.
 */
EXTERNPER size_t pu_getMsgLenBits (OSCTXT* pctxt);

/**
 * This function provides a standard hesadecimal dump of the contents of the
 * buffer currently specified in the given context.
 *
 * @param pctxt       Pointer to a context structure. The contents of the
 *                       encode or decode buffer that was specified in the call
 *                       to pu_initContext or pu_newContext is dumped.
 */
EXTERNPER void pu_hexdump (OSCTXT* pctxt);

/**
 * This function is used to set the buffer pointer for PER encoding or
 * decoding.  For encoding, the buffer would either be a static byte
 * array in memory to receive the encoded message or, if bufaddr was
 * set to NULL, would indicate encoding is to be done to a dynamic
 * buffer.  For decoding, a buffer in memory would be specified which
 * contains the message to be decoded.
 *
 * @param pctxt       Pointer to a context structure.
 * @param bufaddr     Address of memory buffer.  For encoding, this
 *                      may be set to NULL to indicate a dynamic
 *                      buffer is to be used.
 * @param bufsiz      Size, in byte, of static memory buffer.
 * @param aligned     Indicate if aligned (true) or unaligned (false)
 *                      PER should be used for encoding or decoding.
 */
EXTERNPER int pu_setBuffer
(OSCTXT* pctxt, OSOCTET* bufaddr, size_t bufsiz, OSBOOL aligned);

#define pd_setp(pctxt, bufaddr, bufsiz, aligned) \
   pu_setBuffer(pctxt, bufaddr, bufsiz, aligned)

#define pe_setp(pctxt, bufaddr, bufsiz, aligned) \
   pu_setBuffer(pctxt, bufaddr, bufsiz, aligned)

#define pe_resetp(pctxt)   rtxResetContext(pctxt)
#define pd_resetp(pctxt)   rtxResetContext(pctxt)



/**
 * This function is used to initialize a pre-allocated OSCTXT structure. This
 * can be an OSCTXT variable declared on the stack or a pointer to an
 * OSCTXT structure that was previously allocated. This function sets all
 * internal variables within the structure to their initial values.
 *
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param bufaddr      For encoding, this is the address of a buffer to receive
 *                       the encoded PER message (note: this is optional, if
 *                       specified to NULL a dynamic buffer will be allocated).
 *                       For decoding, this is that address of the buffer that
 *                       contains the PER message to be decoded.
 * @param bufsiz       For encoding, this is the size of the encoded buffer
 *                       (note: this is optional, if the bufaddr arguement is
 *                       specified to NULL, then dynamic encoding is in effect
 *                       and the buffer size is indefinite). For decoding, this
 *                       is the length (in octets) of the PER message to be
 *                       decoded.
 * @param aligned      A Boolean value specifying whether aligned or unaligned
 *                       encoding should be performed.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pu_initContext
(OSCTXT* pctxt, OSOCTET* bufaddr, OSUINT32 bufsiz, OSBOOL aligned);

/**
 * This function is used to initialize the buffer of an OSCTXT structure with
 * buffer data from a second context structure. This function copies the buffer
 * information from the source context buffer to the destination structure. The
 * non-buffer related fields in the context remain untouched.
 *
 * @param pTarget      A pointer to the target context structure. Buffer
 *                       information within this structure is updated with data
 *                       from the source context.
 * @param pSource      A pointer to the source context structure. Buffer
 *                       information from the source context structure is
 *                       copied to the target structure.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNPER int pu_initContextBuffer (OSCTXT* pTarget, OSCTXT* pSource);

EXTERNPER const char* pu_getFullName
(OSCTXT* pctxt, const char* suffix);

EXTERNPER void pu_init16BitCharSet
(Asn116BitCharSet* pCharSet, OSUNICHAR first, OSUNICHAR last,
 OSUINT32 abits, OSUINT32 ubits);

/**
 * @param pctxt       A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 */
EXTERNPER void pu_insLenField (OSCTXT* pctxt);

/**
 * This function determines if a size constraint is fixed (i.e
 * allows only one value such as SIZE (2))
 *
 * @param pSizeCnst    A pointer to a size constraint structure.
 * @return             TRUE if size constraint is fixed.
 */
EXTERNPER OSBOOL pu_isFixedSize (const Asn1SizeCnst* pSizeList);

/**
 * This function is similar to the pu_initContext function in that it
 * initializes a context variable. The difference is that this function
 * allocates a new structure and then initializes it. It is equivalent to
 * calling malloc to allocate a context structure and then calling
 * pu_initContext to initialize it.
 *
 * @param bufaddr      For encoding, this is the address of a buffer to receive
 *                       the encoded PER message (note: this is optional, if
 *                       specified as NULL a dynamic buffer will be allocated).
 *                       For decoding, this is that address of the buffer that
 *                       contains the PER message to be decoded.
 * @param bufsiz       For encoding, this is the size of the encoded buffer
 *                       (note: this is optional, if the bufaddr argument is
 *                       specified to NULL, then dynamic encoding is in effect
 *                       and the buffer size is indefinite). For decoding, this
 *                       is the length (in octets) of the PER message to be
 *                       decoded.
 * @param aligned      A Boolean value specifying whether aligned or unaligned
 *                       encoding should be performed.
 * @return             A pointer to OSCTXT structure to receive the allocated
 *                       structure. NULL is returned if any error occurs in
 *                       allocating or initializing the context.
 */
EXTERNPER OSCTXT* pu_newContext
(OSOCTET* bufaddr, OSUINT32 bufsiz, OSBOOL aligned);

EXTERNPER PERField* pu_newField
(OSCTXT* pctxt, const char* nameSuffix);

EXTERNPER void pu_initFieldList (OSCTXT* pctxt, OSINT16 bitOffset);

/**
 * @param pctxt       A pointer to a context structure.
 */
EXTERNPER void pu_popName (OSCTXT* pctxt);

/**
 * Pushs an element on the stack.
 *
 * @param pctxt       A pointer to a context structure.
 * @param idx         The location to insert the element.
 */
EXTERNPER void pu_pushElemName (OSCTXT* pctxt, int idx);

/**
 * @param pctxt       A pointer to a context structure.
 * @param name        A pointer to the element to add to the stack.
 */
EXTERNPER void pu_pushName (OSCTXT* pctxt, const char* name);

/**
 * This function sets a permitted alphabet character set. This is the resulting
 * set of characters when the character associated with a standard character
 * string type is merged with a permitted alphabet constraint.
 *
 * @param pCharSet     A pointer to a character set structure describing the
 *                       character set currently associated with the character
 *                       string type. The resulting character set structure
 *                       after being merged with the permSet parameter.
 * @param permSet      A null-terminated string of permitted characters.
 */
EXTERNPER void pu_setCharSet (Asn1CharSet* pCharSet, const char* permSet);

/**
 * This function sets a permitted alphabet character set for 16-bit character
 * strings. This is the resulting set of character when the character
 * associated with a 16-bit string type is merged with a permitted alphabet
 * constraint.
 *
 * @param pctxt       Pointer to a context structure.
 * @param pCharSet     Pointer to a character set structure describing the
 *                       character set currently associated with the character
 *                       string type. The resulting character set structure
 *                       after being merged with the permSet parameter.
 * @param pAlphabet    Pointer to a structure describing the 16-bit permitted
 *                       alphabet.
 */
EXTERNPER void pu_set16BitCharSet
(OSCTXT* pctxt, Asn116BitCharSet* pCharSet, Asn116BitCharSet* pAlphabet);


/**
 * @param pCharSet     Pointer to a character set structure describing the
 *                       character set currently associated with the character
 *                       string type. The resulting character set structure
 *                       after being merged with the permSet parameter.
 * @param firstChar    The first character in the range.
 * @param lastChar     The last character in the range.
 */
EXTERNPER void pu_set16BitCharSetFromRange
(Asn116BitCharSet* pCharSet, OSUINT16 firstChar, OSUINT16 lastChar);

EXTERNPER void pu_setFldBitCount (OSCTXT* pctxt);

EXTERNPER void pu_setFldBitOffset (OSCTXT* pctxt);

EXTERNPER void pu_setFldListFromCtxt (OSCTXT* pctxt, OSCTXT* srcctxt);

EXTERNPER void pu_setOpenTypeFldList (OSCTXT* pctxt, OSRTSList* plist);

EXTERNPER OSBOOL pu_setTrace (OSCTXT* pCtxt, OSBOOL value);

EXTERNPER void pu_setAligned (OSCTXT* pctxt, OSBOOL value);

EXTERNPER void pu_deleteFieldList (OSCTXT* pctxt);

/**
 * This function provides a detailed binary dump of the contents of the buffer
 * currently specified in the given context. The list of fields dumped by this
 * function was previously built up within the context using calls pu_newField,
 * pu_pushName, and pu_popName. These calls are built into both
 * compiler-generated and low-level PER encode/decode functions to trace the
 * actual bit encoding of a given construct.
 *
 * @param pctxt       A pointer to a context structure. The contents of the
 *                       encode or decode buffer that was specified in the call
 *                       to pu_initContext or pu_newContext is dumped.
 * @param varname      The name of the top-level variable name of the structure
 *                       being dumped.
 */
EXTERNPER void pu_bindump (OSCTXT* pctxt, const char* varname);

EXTERNPER void pu_dumpField
(OSCTXT* pctxt, PERField* pField, const char* varname,
 size_t nextBitOffset, BinDumpBuffer* pbuf);

EXTERNPER void pu_init32BitCharSet
(Asn132BitCharSet* pCharSet, OS32BITCHAR first, OS32BITCHAR last,
 OSUINT32 abits, OSUINT32 ubits);

/**
 * This function sets a permitted alphabet character set for 32-bit character
 * strings. This is the resulting set of character when the character
 * associated with a 16-bit string type is merged with a permitted alphabet
 * constraint.
 *
 * @param pctxt       Pointer to a context structure.
 * @param pCharSet     Pointer to a character set structure describing the
 *                       character set currently associated with the character
 *                       string type. The resulting character set structure
 *                       after being merged with the permSet parameter.
 * @param pAlphabet    Pointer to a structure describing the 32-bit permitted
 *                       alphabet.
 */
EXTERNPER void pu_set32BitCharSet
(OSCTXT* pctxt, Asn132BitCharSet* pCharSet, Asn132BitCharSet* pAlphabet);

/**
 * @param pCharSet     Pointer to a character set structure describing the
 *                       character set currently associated with the character
 *                       string type. The resulting character set structure
 *                       after being merged with the permSet parameter.
 * @param firstChar    The first character in the range.
 * @param lastChar     The last character in the range.
 */
EXTERNPER void pu_set32BitCharSetFromRange
(Asn132BitCharSet* pCharSet, OSUINT32 firstChar, OSUINT32 lastChar);

/**
 * Returns numeric version of run-time library. The format of version is as
 * follows: MmP, where:
 *     M - major version number;
 *     m - minor version number;
 *     p - patch release number.
 * For example, the value 581 means the version 5.81.
 * @return             Version of run-time library in numeric format.
 */
EXTERNPER int pu_GetLibVersion (void);

/**
 * Returns information string describing the library. The string
 * contains name of library, its version and flags used for building
 * the library.
 *
 * @return     Information string
 */
EXTERNPER const char* pu_GetLibInfo (void);

/**
 * @} perutil
 */

#ifdef __cplusplus
}
#endif

/* Macros */

#define pd_bit(pctxt,pvalue) rtxDecBit(pctxt,pvalue)

#define pd_bits(pctxt,pvalue,nbits) rtxDecBits(pctxt,pvalue,nbits)

#define pd_octets(pctxt,pbuffer,bufsiz,nbits) \
rtxDecBitsToByteArray(pctxt,pbuffer,bufsiz,nbits)

#define pe_GeneralString(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_GraphicString(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_T61String(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_TeletexString(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_VideotexString(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_ObjectDescriptor(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_UTF8String(pctxt,value,permCharSet) \
pe_VarWidthCharString(pctxt, value)

#define pe_IA5String(pctxt,value,permCharSet) \
pe_ConstrainedStringEx (pctxt, value, permCharSet, 8, 7, 7)

#define pe_NumericString(pctxt,value,permCharSet) \
pe_ConstrainedStringEx (pctxt, value, \
(permCharSet == 0)?NUM_CANSET:permCharSet, 4, 4, 4)

#define pe_PrintableString(pctxt,value,permCharSet) \
pe_ConstrainedStringEx (pctxt, value, permCharSet, 8, 7, 7)

#define pe_VisibleString(pctxt,value,permCharSet) \
pe_ConstrainedStringEx (pctxt, value, permCharSet, 8, 7, 7)

#define pe_ISO646String      pe_IA5String
#define pe_GeneralizedTime   pe_IA5String
#define pe_UTCTime           pe_GeneralizedTime

#define pd_GeneralString(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_GraphicString(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_VideotexString(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_TeletexString(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_T61String(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_ObjectDescriptor(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_UTF8String(pctxt,pvalue,permCharSet) \
pd_VarWidthCharString (pctxt, pvalue)

#define pd_IA5String(pctxt,pvalue,permCharSet) \
pd_ConstrainedStringEx (pctxt, pvalue, permCharSet, 8, 7, 7)

#define pd_NumericString(pctxt,pvalue,permCharSet) \
pd_ConstrainedStringEx (pctxt, pvalue, \
(permCharSet == 0)?NUM_CANSET:permCharSet, 4, 4, 4)

#define pd_PrintableString(pctxt,pvalue,permCharSet) \
pd_ConstrainedStringEx (pctxt, pvalue, permCharSet, 8, 7, 7)

#define pd_VisibleString(pctxt,pvalue,permCharSet) \
pd_ConstrainedStringEx (pctxt, pvalue, permCharSet, 8, 7, 7)

#define pd_ISO646String      pd_IA5String
#define pd_GeneralizedTime   pd_IA5String
#define pd_UTCTime           pd_GeneralizedTime
#define pe_GetMsgLen         pu_getMsgLen

#define pe_ExpandBuffer(pctxt,nbytes) rtxCheckOutputBuffer(pctxt,nbytes)

/* Date/Time decode macros */

/* EXTERNPER int pd_AnyCentury (OSCTXT* pctxt, const char** string); */
#define pd_AnyCentury(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSCENTURY)

/* EXTERNPER int pd_AnyCenturyInt (OSCTXT* pctxt, OSINT32* pvalue); */
#define pd_AnyCenturyInt(pctxt,pvalue) \
pd_UnconsInteger (pctxt, pvalue)

/* EXTERNPER int pd_AnyDate (OSCTXT* pctxt, const char** string); */
#define pd_AnyDate(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR|OSMONTH|OSDAY)

/* EXTERNPER int pd_AnyYear (OSCTXT* pctxt, const char** string); */
#define pd_AnyYear(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR)

/* EXTERNPER int pd_AnyYearInt (OSCTXT* pctxt, OSINT32* pvalue); */
#define pd_AnyYearInt(pctxt,pvalue) \
pd_UnconsInteger (pctxt, pvalue)

/* EXTERNPER int pd_AnyYearDay (OSCTXT* pctxt, const char** string); */
#define pd_AnyYearDay(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR|OSDAY)

/* EXTERNPER int pd_AnyYearMonth (OSCTXT* pctxt, const char** string); */
#define pd_AnyYearMonth(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR|OSMONTH)

/* EXTERNPER int pd_AnyYearMonthDay (OSCTXT* pctxt, const char** string); */
#define pd_AnyYearMonthDay(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR|OSMONTH|OSDAY)

/* EXTERNPER int pd_AnyYearWeek (OSCTXT* pctxt, const char** string); */
#define pd_AnyYearWeek(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR|OSWEEK)

/* EXTERNPER int pd_AnyYearWeekDay (OSCTXT* pctxt, const char** string); */
#define pd_AnyYearWeekDay(pctxt,string) \
pd_DateStr (pctxt, string, OSANY|OSYEAR|OSWEEK|OSDAY)

/* EXTERNPER int pd_Century (OSCTXT* pctxt, const char** string); */
#define pd_Century(pctxt,string) \
pd_DateStr (pctxt, string, OSCENTURY)

/* EXTERNPER int pd_CenturyInt (OSCTXT* pctxt, OSINT32* pvalue); */
#define pd_CenturyInt(pctxt,pvalue) \
pd_ConsUInt8 (pctxt, pvalue, 0, 99)

/* EXTERNPER int pd_Date (OSCTXT* pctxt, const char** string); */
#define pd_Date(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR_BASIC|OSYEAR|OSMONTH|OSDAY);

/* EXTERNPER int pd_DateTime (OSCTXT* pctxt, const char** string); */
#define pd_DateTime(pctxt,string) \
pd_DateTimeStr (pctxt, string, \
OSYEAR_BASIC|OSYEAR|OSMONTH|OSDAY|OSHOURS|OSMINUTES|OSSECONDS);

/* EXTERNPER int pd_DurationInterval (OSCTXT* pctxt, const char** string); */
#define pd_DurationInterval(pctxt,string) \
pd_Duration (pctxt, string, FALSE)

/* EXTERNPER int pd_DurationEndDateInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_DurationEndDateInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, OSDURATION, flags)

/* EXTERNPER int pd_DurationEndTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_DurationEndTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, OSDURATION, flags)

/* EXTERNPER int pd_DurationEndDateTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_DurationEndDateTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, OSDURATION, flags)

#ifdef BINARY_HOURS

#define pd_Hours(pctxt,pvalue) \
pd_ConsInt8 (pctxt, pvalue, 0, 24)

#define pd_HoursUtc(pctxt,pvalue) \
pd_ConsInt8 (pctxt, pvalue, 0, 24)

#else /* BINARY_HOURS */

/* EXTERNPER int pd_Hours (OSCTXT* pctxt, const char** string); */
#define pd_Hours(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS)

/* EXTERNPER int pd_HoursUtc (OSCTXT* pctxt, const char** string); */
#define pd_HoursUtc(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSUTC)

#endif /* BINARY_HOURS */

/* EXTERNPER int pd_HoursAndDiff (OSCTXT* pctxt, const char** string); */
#define pd_HoursAndDiff(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSDIFF)

/* EXTERNPER int pd_HoursAndFraction (OSCTXT* pctxt, const char** string); */
#define pd_HoursAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|(n))

/* EXTERNPER int pd_HoursUtcAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_HoursUtcAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSUTC|(n))

/* EXTERNPER int pd_HoursAndDiffAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_HoursAndDiffAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSDIFF|(n))

/* EXTERNPER int pd_Minutes (OSCTXT* pctxt, const char** string); */
#define pd_Minutes(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES)

/* EXTERNPER int pd_MinutesUtc (OSCTXT* pctxt, const char** string); */
#define pd_MinutesUtc(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSUTC)

/* EXTERNPER int pd_MinutesAndDiff (OSCTXT* pctxt, const char** string); */
#define pd_MinutesAndDiff(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSDIFF)

/* EXTERNPER int pd_MinutesAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_MinutesAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|(n))

/* EXTERNPER int pd_MinutesUtcAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_MinutesUtcAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSUTC|(n))

/* EXTERNPER int pd_MinutesAndDiffAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_MinutesAndDiffAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSDIFF|(n))

/* EXTERNPER int pd_RecStartEndDateInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecStartEndDateInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, flags, flags)

/* EXTERNPER int pd_RecStartEndTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecStartEndTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, flags, flags)

/* EXTERNPER int pd_RecStartEndDateTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecStartEndDateTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, flags, flags)

/* EXTERNPER int pd_RecDurationInterval
(OSCTXT* pctxt, const char** string); */
#define pd_RecDurationInterval(pctxt,string) \
pd_Duration (pctxt, string, TRUE)

/* EXTERNPER int pd_RecStartDateDurationInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecStartDateDurationInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, flags, OSDURATION)

/* EXTERNPER int pd_RecStartTimeDurationInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecStartTimeDurationInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, flags, OSDURATION)

/* EXTERNPER int pd_RecStartDateTimeDurationInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecStartDateTimeDurationInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, flags, OSDURATION)

/* EXTERNPER int pd_RecDurationEndDateInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecDurationEndDateInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, OSDURATION, flags)

/* EXTERNPER int pd_RecDurationEndTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecDurationEndTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, OSDURATION, flags)

/* EXTERNPER int pd_RecDurationEndDateTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_RecDurationEndDateTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, TRUE, OSDURATION, flags)

/* EXTERNPER int pd_StartEndDateInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_StartEndDateInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, flags, flags)

/* EXTERNPER int pd_StartEndTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_StartEndTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, flags, flags)

/* EXTERNPER int pd_StartEndDateTimeInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_StartEndDateTimeInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, flags, flags)

/* EXTERNPER int pd_StartDateDurationInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_StartDateDurationInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, flags, OSDURATION)

/* EXTERNPER int pd_StartTimeDurationInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_StartTimeDurationInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, flags, OSDURATION)

/* EXTERNPER int pd_StartDateTimeDurationInterval
(OSCTXT* pctxt, const char** string, unsigned flags); */
#define pd_StartDateTimeDurationInterval(pctxt,string,flags) \
pd_Interval (pctxt, string, FALSE, flags, OSDURATION)

/* EXTERNPER int pd_TimeOfDay (OSCTXT* pctxt, const char** string); */
#define pd_TimeOfDay(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS)

/* EXTERNPER int pd_TimeOfDayUtc (OSCTXT* pctxt, const char** string); */
#define pd_TimeOfDayUtc(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSUTC)

/* EXTERNPER int pd_TimeOfDayAndDiff (OSCTXT* pctxt, const char** string); */
#define pd_TimeOfDayAndDiff(pctxt,string) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSDIFF)

/* EXTERNPER int pd_TimeOfDayAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_TimeOfDayAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|(n))

/* EXTERNPER int pd_TimeOfDayUtcAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_TimeOfDayUtcAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSUTC|(n))

/* EXTERNPER int pd_TimeOfDayAndDiffAndFraction
(OSCTXT* pctxt, const char** string); */
#define pd_TimeOfDayAndDiffAndFraction(pctxt,string,n) \
pd_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSDIFF|(n))

/* EXTERNPER int pd_Year (OSCTXT* pctxt, const char** string); */
#define pd_Year(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR)

/* EXTERNPER int pd_YearDay (OSCTXT* pctxt, const char** string); */
#define pd_YearDay(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR|OSDAY)

/* EXTERNPER int pd_YearMonth (OSCTXT* pctxt, const char** string); */
#define pd_YearMonth(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR|OSMONTH)

/* EXTERNPER int pd_YearMonthDay (OSCTXT* pctxt, const char** string); */
#define pd_YearMonthDay(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR|OSMONTH|OSDAY);

/* EXTERNPER int pd_YearWeek (OSCTXT* pctxt, const char** string); */
#define pd_YearWeek(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR|OSWEEK)

/* EXTERNPER int pd_YearWeekDay (OSCTXT* pctxt, const char** string); */
#define pd_YearWeekDay(pctxt,string) \
pd_DateStr (pctxt, string, OSYEAR|OSWEEK|OSDAY)

/* Date/Time encode macros */

/* EXTERNPER int pe_AnyCentury (OSCTXT* pctxt, const char* string); */
#define pe_AnyCentury(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSCENTURY)

/* EXTERNPER int pe_AnyCenturyInt (OSCTXT* pctxt, OSINT32 value); */
#define pe_AnyCenturyInt(pctxt,value) \
pe_UnconsInteger (pctxt, value)

/* EXTERNPER int pe_AnyDate (OSCTXT* pctxt, const char* string); */
#define pe_AnyDate(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR|OSMONTH|OSDAY)

/* EXTERNPER int pe_AnyYear (OSCTXT* pctxt, const char* string); */
#define pe_AnyYear(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR)

/* EXTERNPER int pe_AnyYearInt (OSCTXT* pctxt, OSINT32 value); */
#define pe_AnyYearInt(pctxt,value) \
pe_UnconsInteger (pctxt, value)

/* EXTERNPER int pe_AnyYearDay (OSCTXT* pctxt, const char* string); */
#define pe_AnyYearDay(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR|OSDAY)

/* EXTERNPER int pe_AnyYearMonth (OSCTXT* pctxt, const char* string); */
#define pe_AnyYearMonth(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR|OSMONTH)

/* EXTERNPER int pe_AnyYearMonthDay (OSCTXT* pctxt, const char* string); */
#define pe_AnyYearMonthDay(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR|OSMONTH|OSDAY)

/* EXTERNPER int pe_AnyYearWeek (OSCTXT* pctxt, const char* string); */
#define pe_AnyYearWeek(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR|OSWEEK)

/* EXTERNPER int pe_AnyYearWeekDay (OSCTXT* pctxt, const char* string); */
#define pe_AnyYearWeekDay(pctxt,string) \
pe_DateStr (pctxt, string, OSANY|OSYEAR|OSWEEK|OSDAY)

/* EXTERNPER int pe_Century (OSCTXT* pctxt, const char* string); */
#define pe_Century(pctxt,string) \
pe_DateStr (pctxt, string, OSCENTURY)

/* EXTERNPER int pe_CenturyInt (OSCTXT* pctxt, OSUINT32 value); */
#define pe_CenturyInt(pctxt,value) \
pe_ConsUnsigned (pctxt, value, 0, 99)

/* EXTERNPER int pe_Date (OSCTXT* pctxt, const char* string); */
#define pe_Date(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR_BASIC|OSYEAR|OSMONTH|OSDAY)

/* EXTERNPER int pe_DateTime (OSCTXT* pctxt, const char* string); */
#define pe_DateTime(pctxt,string) \
pe_DateTimeStr (pctxt, string, \
OSYEAR_BASIC|OSYEAR|OSMONTH|OSDAY|OSHOURS|OSMINUTES|OSSECONDS)

/* EXTERNPER int pe_DurationInterval (OSCTXT* pctxt, const char* string); */
#define pe_DurationInterval(pctxt,string) \
pe_Duration (pctxt, string, FALSE)

/* EXTERNPER int pe_DurationEndDateInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_DurationEndDateInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, OSDURATION, flags)

/* EXTERNPER int pe_DurationEndTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_DurationEndTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, OSDURATION, flags)

/* EXTERNPER int pe_DurationEndDateTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_DurationEndDateTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, OSDURATION, flags)

#ifdef BINARY_HOURS

#define pe_Hours(pctxt,value) \
pe_ConsUnsigned (pctxt, value, 0, 24)

#define pe_HoursUtc(pctxt,value) \
pe_ConsUnsigned (pctxt, value, 0, 24)

#else /* BINARY_HOURS */

/* EXTERNPER int pe_Hours (OSCTXT* pctxt, const char* string); */
#define pe_Hours(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS)

/* EXTERNPER int pe_HoursUtc (OSCTXT* pctxt, const char* string); */
#define pe_HoursUtc(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSUTC)

#endif /* BINARY_HOURS */

/* EXTERNPER int pe_Hours (OSCTXT* pctxt, const char* string); */
#define pe_Hours(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS)

/* EXTERNPER int pe_HoursUtc (OSCTXT* pctxt, const char* string); */
#define pe_HoursUtc(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSUTC)

/* EXTERNPER int pe_HoursAndDiff (OSCTXT* pctxt, const char* string); */
#define pe_HoursAndDiff(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSDIFF)

/* EXTERNPER int pe_HoursAndFraction (OSCTXT* pctxt, const char* string); */
#define pe_HoursAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|(n))

/* EXTERNPER int pe_HoursUtcAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_HoursUtcAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSUTC|(n))

/* EXTERNPER int pe_HoursAndDiffAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_HoursAndDiffAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSDIFF|(n))

/* EXTERNPER int pe_Minutes (OSCTXT* pctxt, const char* string); */
#define pe_Minutes(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES)

/* EXTERNPER int pe_MinutesUtc (OSCTXT* pctxt, const char* string); */
#define pe_MinutesUtc(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSUTC)

/* EXTERNPER int pe_MinutesAndDiff (OSCTXT* pctxt, const char* string); */
#define pe_MinutesAndDiff(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSDIFF)

/* EXTERNPER int pe_MinutesAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_MinutesAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|(n))

/* EXTERNPER int pe_MinutesUtcAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_MinutesUtcAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSUTC|(n))

/* EXTERNPER int pe_MinutesAndDiffAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_MinutesAndDiffAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSDIFF|(n))

/* EXTERNPER int pe_RecStartEndDateInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecStartEndDateInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, flags, flags)

/* EXTERNPER int pe_RecStartEndTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecStartEndTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, flags, flags)

/* EXTERNPER int pe_RecStartEndDateTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecStartEndDateTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, flags, flags)

/* EXTERNPER int pe_RecDurationInterval
(OSCTXT* pctxt, const char* string); */
#define pe_RecDurationInterval(pctxt,string) \
pe_Duration (pctxt, string, TRUE)

/* EXTERNPER int pe_RecStartDateDurationInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecStartDateDurationInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, flags, OSDURATION)

/* EXTERNPER int pe_RecStartTimeDurationInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecStartTimeDurationInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, flags, OSDURATION)

/* EXTERNPER int pe_RecStartDateTimeDurationInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecStartDateTimeDurationInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, flags, OSDURATION)

/* EXTERNPER int pe_RecDurationEndDateInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecDurationEndDateInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, OSDURATION, flags)

/* EXTERNPER int pe_RecDurationEndTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecDurationEndTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, OSDURATION, flags)

/* EXTERNPER int pe_RecDurationEndDateTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_RecDurationEndDateTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, TRUE, OSDURATION, flags)

/* EXTERNPER int pe_StartEndDateInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_StartEndDateInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, flags, flags)

/* EXTERNPER int pe_StartEndTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_StartEndTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, flags, flags)

/* EXTERNPER int pe_StartEndDateTimeInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_StartEndDateTimeInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, flags, flags)

/* EXTERNPER int pe_StartDateDurationInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_StartDateDurationInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, flags, OSDURATION)

/* EXTERNPER int pe_StartTimeDurationInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_StartTimeDurationInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, flags, OSDURATION)

/* EXTERNPER int pe_StartDateTimeDurationInterval
(OSCTXT* pctxt, const char* string, unsigned flags); */
#define pe_StartDateTimeDurationInterval(pctxt,string,flags) \
pe_Interval (pctxt, string, FALSE, flags, OSDURATION)

/* EXTERNPER int pe_TimeOfDay (OSCTXT* pctxt, const char* string); */
#define pe_TimeOfDay(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS)

/* EXTERNPER int pe_TimeOfDayUtc (OSCTXT* pctxt, const char* string); */
#define pe_TimeOfDayUtc(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSUTC)

/* EXTERNPER int pe_TimeOfDayAndDiff (OSCTXT* pctxt, const char* string); */
#define pe_TimeOfDayAndDiff(pctxt,string) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSDIFF)

/* EXTERNPER int pe_TimeOfDayAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_TimeOfDayAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|(n))

/* EXTERNPER int pe_TimeOfDayUtcAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_TimeOfDayUtcAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSUTC|(n))

/* EXTERNPER int pe_TimeOfDayAndDiffAndFraction
(OSCTXT* pctxt, const char* string); */
#define pe_TimeOfDayAndDiffAndFraction(pctxt,string,n) \
pe_TimeStr (pctxt, string, OSHOURS|OSMINUTES|OSSECONDS|OSDIFF|(n))

/* EXTERNPER int pe_Year (OSCTXT* pctxt, const char* string); */
#define pe_Year(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR)

/* EXTERNPER int pe_YearDay (OSCTXT* pctxt, const char* string) */
#define pe_YearDay(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR|OSDAY)

/* EXTERNPER int pe_YearMonth (OSCTXT* pctxt, const char* string); */
#define pe_YearMonth(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR|OSMONTH)

/* EXTERNPER int pe_YearMonth (OSCTXT* pctxt, const char* string); */
#define pe_YearMonthDay(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR|OSMONTH|OSDAY)

/* EXTERNPER int pe_YearWeek (OSCTXT* pctxt, const char* string); */
#define pe_YearWeek(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR|OSWEEK)

/* EXTERNPER int pe_YearWeekDay (OSCTXT* pctxt, const char* string); */
#define pe_YearWeekDay(pctxt,string) \
pe_DateStr (pctxt, string, OSYEAR|OSWEEK|OSDAY)

/* The following functions are deprecated.  They only exist for backward
   compatibility with older versions.. */

EXTERNPER int pu_checkSizeConstraint (OSCTXT* pctxt, int size);
EXTERNPER Asn1SizeCnst* pu_getSizeConstraint (OSCTXT* pctxt, OSBOOL extbit);
EXTERNPER int pu_getBitOffset (OSCTXT* pctxt);
EXTERNPER void pu_setBitOffset (OSCTXT* pctxt, int bitOffset);

/** @} perruntime */

#endif
