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
 * @file rtxDiagBitTrace.h
 * Common runtime functions for tracing bit patterns written to or read
 * from a stream.
 */
#ifndef _RTXDIAGBITTRACE_H_
#define _RTXDIAGBITTRACE_H_

#include <stdarg.h>
#include "rtxsrc/rtxMemBuf.h"
#include "rtxsrc/rtxSList.h"
#include "rtxsrc/rtxPrintToStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ccfDiag
 */
typedef struct {
   const char*  elemName;
   const char*  nameSuffix;
   size_t       bitOffset;
   size_t       numbits;
} OSRTDiagBitField;

typedef struct {
   OSUINT8 lb, lbm;
   char fmtBitBuffer[40], fmtHexBuffer[10], fmtAscBuffer[10];
   int  fmtBitCharIdx, fmtHexCharIdx, fmtAscCharIdx;
} OSRTDiagBinDumpBuffer;

typedef struct OSRTDiagBitFieldList {
   OSUINT16  disabledCount;
   OSRTSList fieldList; /* list of OSRTDiagBitField */
   OSRTMEMBUF* pCaptureBuf; /* capture buffer for streams */
   OSRTDiagBinDumpBuffer printBuffer;  /* formatted text buffer */
   OSRTSListNode* pLastPrinted; /* last node in list that was printed */
} OSRTDiagBitFieldList;

/* Macros */

#ifdef _TRACE
#define RTDIAG_INSBITFLDLEN(pctxt) \
rtxDiagInsBitFieldLen(pctxt->pBitFldList)

#define RTDIAG_NEWBITFIELD(pctxt,suffix) \
rtxDiagNewBitField(pctxt->pBitFldList,suffix)

#define RTDIAG_SETBITFLDOFFSET(pctxt) \
rtxDiagSetBitFldOffset(pctxt->pBitFldList)

#define RTDIAG_SETBITFLDCOUNT(pctxt) \
rtxDiagSetBitFldCount(pctxt->pBitFldList)
#else
#define RTDIAG_INSBITFLDLEN(pctxt)
#define RTDIAG_NEWBITFIELD(pctxt,suffix)
#define RTDIAG_SETBITFLDOFFSET(pctxt)
#define RTDIAG_SETBITFLDCOUNT(pctxt)
#endif

/**
 * This macro calculates the relative bit offset to the current
 * buffer position.
 */
#define RTDIAG_GETCTXTBITOFFSET(pctxt) \
(((pctxt)->buffer.byteIndex * 8) + (8 - (pctxt)->buffer.bitOffset))

/**
 * This function initializes the standard bit field list structure within
 * the context.
 *
 * @param pctxt       Pointer to a context structure.
 */
EXTERNRT int rtxDiagCtxtBitFieldListInit (OSCTXT* pctxt);

/**
 * This function initializes a bit field list structure.
 *
 * @param pctxt       Pointer to a context structure.
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagBitFieldListInit
(OSCTXT* pctxt, OSRTDiagBitFieldList* pBFList);

/**
 * This function inserts a special length field before the current record
 * in the bit field list.
 *
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagInsBitFieldLen (OSRTDiagBitFieldList* pBFList);

/**
 * This function allocates a new bit field structure and adds it to
 * the bit field list.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param nameSuffix  Suffix to append to the bit field name.
 * @return            Allocated bit field structure.
 */
EXTERNRT OSRTDiagBitField* rtxDiagNewBitField
(OSRTDiagBitFieldList* pBFList, const char* nameSuffix);

/**
 * This function is used to set the bit offset in the current bit field
 * structure.
 *
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagSetBitFldOffset (OSRTDiagBitFieldList* pBFList);

/**
 * This function is used to set the bit count in the current bit field
 * structure.
 *
 * @param pBFList     Pointer to bit field list structure.
 */
EXTERNRT void rtxDiagSetBitFldCount (OSRTDiagBitFieldList* pBFList);

/**
 * This function is used to set the name suffix in the current bit field
 * structure.  This text is printed after the element name when the
 * field is displayed.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param nameSuffix  Suffix to append to the bit field name.
 */
EXTERNRT void rtxDiagSetBitFldNameSuffix
(OSRTDiagBitFieldList* pBFList, const char* nameSuffix);

/**
 * This function increments or decrements the disabled count.  This
 * allows the list to be temporaily disabled to allow collection of
 * more bits to form larger, aggregate fields.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param value       Indicates if disabled count should be incremented
 *                      (TRUE) or decremented (FALSE).
 * @return            TRUE if field operations are still disabled.
 */
EXTERNRT OSBOOL rtxDiagSetBitFldDisabled
(OSRTDiagBitFieldList* pBFList, OSBOOL value);

/**
 * This function prints the bit field list to a an output stream.  By
 * default, the output goes to stdout; but this can be changed by
 * creating a print output stream within the context (see rtxPrintStream).
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param varname     A variable name that is prepended to each field.
 */
EXTERNRT void rtxDiagBitTracePrint
(OSRTDiagBitFieldList* pBFList, const char* varname);

/**
 * This function prints the bit field list to a an HTML document.
 *
 * @param filename    Name of HTML file to be written.
 * @param pBFList     Pointer to bit field list structure.
 * @param varname     A variable name that is prepended to each field.
 */
EXTERNRT void rtxDiagBitTracePrintHTML
(const char* filename, OSRTDiagBitFieldList* pBFList, const char* varname);

/**
 * This function appends the given name part to the element name
 * in the bit field.  A dot (.) separator character is added
 * after the existing name and before the name part.
 *
 * @param pBFList     Pointer to bit field list structure.
 * @param namePart    A name part that is appended to the field.
 */
EXTERNRT void rtxDiagBitFldAppendNamePart
(OSRTDiagBitFieldList* pBFList, const char* namePart);

#ifdef __cplusplus
}
#endif

#endif
