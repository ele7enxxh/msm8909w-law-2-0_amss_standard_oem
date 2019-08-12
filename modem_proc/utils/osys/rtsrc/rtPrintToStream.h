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
 * @file rtPrintToStream.h
 * Functions to print the output in a "name=value" format.
 */
#ifndef _RTPRINTTOSTREAM_H_
#define _RTPRINTTOSTREAM_H_
#include <stdio.h>
#include "rtsrc/asn1type.h"
#include "rtxsrc/rtxPrintStream.h"
#include "rtxsrc/rtxPrintToStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup valsToStream Print Values to user specified Stream
 * @ingroup cruntime
 * @{
 * These functions print the output in a "name=value" format.
 */
#define rtPrintToStreamBoolean rtxPrintToStreamBoolean
#define rtPrintToStreamInteger rtxPrintToStreamInteger
#define rtPrintToStreamInt64   rtxPrintToStreamInt64
#define rtPrintToStreamUnsigned rtxPrintToStreamUnsigned
#define rtPrintToStreamUInt64  rtxPrintToStreamUInt64
#define rtPrintToStreamCharStr rtxPrintToStreamCharStr
#define rtPrintToStreamReal    rtxPrintToStreamReal
#define rtPrintToStreamHexStr  rtxPrintToStreamHexStr
#define rtPrintToStreamIndent  rtxPrintToStreamIndent
#define rtPrintToStreamIncrIndent rtxPrintToStreamIncrIndent
#define rtPrintToStreamDecrIndent rtxPrintToStreamDecrIndent
#define rtPrintToStreamCloseBrace rtxPrintToStreamCloseBrace
#define rtPrintToStreamOpenBrace  rtxPrintToStreamOpenBrace

/**
 * Prints an ASN.1 bit string value to a print stream.
 *
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param numbits      The number of bitsto be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamBitStr
(OSCTXT *pctxt, const char* name, OSSIZE numbits, const OSOCTET* data,
 const char* conn);

/**
 * This function prints the value of a bit string to a stream in brace text
 * format.
 *
 * @param pctxt        Pointer to a context initialized for printing.
 * @param name         The name of the variable to print.
 * @param numbits      The number of bitsto be printed.
 * @param data         A pointer to the data to be printed.
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamBitStrBraceText
(OSCTXT *pctxt, const char* name, OSSIZE numbits, const OSOCTET* data);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         A pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOctStr
(OSCTXT *pctxt, const char* name, OSSIZE numocts, const OSOCTET* data,
 const char* conn);

/**
 * Prints a 16-bit (Unicode) character string value to a print stream.
 *
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStream16BitCharStr
(OSCTXT *pctxt, const char* name, const Asn116BitCharString* bstring,
 const char* conn);

/**
 * Prints a 32-bit (Universal) character string value to a print stream.
 *
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStream32BitCharStr
(OSCTXT *pctxt, const char* name, const Asn132BitCharString* bstring,
 const char* conn);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param pOID         A pointer to the OID to be printed.
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOID
(OSCTXT *pctxt, const char* name, const ASN1OBJID* pOID);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param pOID         A pointer to the OID to be printed.
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOIDValue (OSCTXT *pctxt, const ASN1OBJID* pOID);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param pOID         A pointer to the OID to be printed.
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOID64
(OSCTXT *pctxt, const char* name, const ASN1OID64* pOID);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param pOID         A pointer to the OID to be printed.
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOID64Value (OSCTXT *pctxt, const ASN1OID64* pOID);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param numocts      The number of octets to be printed.
 * @param data         The pointer to the data to be printed.
 * @param conn         A pointer to the connector between the name and the
 *                       value. This points to either -> or .
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOpenType
(OSCTXT *pctxt, const char* name, OSSIZE numocts, const OSOCTET* data,
 const char* conn);

/**
 * @param pctxt        Pointer to an ASN1 context initialized for printing.
 * @param name         The name of the variable to print.
 * @param pElemList    A pointer to an element of a list.
 *
 * @return             Completion status, 0 on success and -ve on failure
 */
EXTERNRT int rtPrintToStreamOpenTypeExt
(OSCTXT *pctxt, const char* name, const OSRTDList* pElemList);

/**
 * This function prints a Universal string to stream output.  Characters
 * in the string that are within the normal Ascii range are printed as
 * single characters.  Characters outside the Ascii range are printed
 * as 8-byte hex codes (0xnnnnnnnn).
 * @param pctxt        Pointer to an ASN1 context initialized for stream
 *                     printing.
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 */
EXTERNRT int rtPrintToStreamUnivCharStr
   (OSCTXT *pctxt, const char* name, const Asn132BitCharString* bstring);

/**
 * This function prints a Unicode string to stream output.  Characters
 * in the string that are within the normal Ascii range are printed as
 * single characters.  Characters outside the Ascii range are printed
 * as 4-byte hex codes (0xnnnn).
 * @param pctxt        Pointer to an ASN1 context initialized for stream
 *                     printing.
 * @param name         The name of the variable to print.
 * @param bstring      A pointer to the bit string to be printed.
 */
EXTERNRT int rtPrintToStreamUnicodeCharStr
   (OSCTXT *pctxt, const char* name, const Asn116BitCharString* bstring);

/**
 * This function prints the value of an open type extension in brace
 * text format to stream.
 * @param pctxt        Pointer to an ASN1 context initialized for stream
 *                     printing.
 * @param name         The name of the variable to print.
 * @param pElemList    A pointer to an element of a list.
 */
EXTERNRT int rtPrintToStreamOpenTypeExtBraceText
   (OSCTXT *pctxt, const char* name, const OSRTDList* pElemList);

/**
 * These macro definitions provide backward compatible definitions for
 * ASN1C v5.xx applications.
 */
#define rtPrintCallback  rtxPrintCallback
#define rtSetPrintStream rtxSetPrintStream
#define rtSetGlobalPrintStream rtxSetGlobalPrintStream
#define rtPrintToStream rtxPrintToStream
#define rtDiagToStream  rtxDiagToStream
#define rtPrintStreamRelease rtxPrintStreamRelease

/**
 * @} valsToStream
 */
#ifdef __cplusplus
}
#endif
#endif
