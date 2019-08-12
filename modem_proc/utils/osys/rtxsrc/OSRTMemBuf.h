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
 * @file OSRTMemBuf.h
 */
/* memory buffer */

#ifndef _OSRTMEMBUF_H_
#define _OSRTMEMBUF_H_

/* Disable MS VC++ Level 4 warning about unreferenced inline functions */
#ifdef _MSC_VER
#pragma warning(disable: 4514)
#endif /* _MSC_VER */

#include "rtxsrc/rtxMemBuf.h"

/**
 * Memory Buffer class. This is the base class for generated C++ data type
 * classes for XSD string types (string, token, NMTOKEN, etc.).
 */
class EXTRTCLASS OSRTMemBuf : public OSRTMEMBUF {
 public:
   EXTRTMETHOD OSRTMemBuf ();
   EXTRTMETHOD ~OSRTMemBuf ();
   inline const OSOCTET* getData() { return buffer + startidx; }
   inline size_t getDataLen() { return usedcnt - startidx; }
} ;

#endif // _OSRTMEMBUF_H_
