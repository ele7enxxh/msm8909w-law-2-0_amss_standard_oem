/*
 * Copyright (c) 2013 Objective Systems, Inc.
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
 * @file rtxHashMapStr2Int.h
 * String-to-integer hash map interface.  This relates a STRING key structure
 * (const OSUTF8CHAR*) to a 32-bit signed integer value (OSINT32).  It uses
 * the rtxHashMap .h/.c file as a template.
 */
#ifndef _RTXHASHMAPSTR2INT_H_
#define _RTXHASHMAPSTR2INT_H_

#define HASHMAPTYPENAME  OSRTHashMapStr2Int
#define HASHMAPENTRYTYPE OSRTHashMapStr2IntEntry
#define HASHMAPKEYTYPE   const OSUTF8CHAR*
#define HASHMAPVALUETYPE OSINT32
#define HASHMAPINITFUNC  rtxHashMapStr2IntInit
#define HASHMAPNEWFUNC   rtxNewHashMapStr2Int
#define HASHMAPCOPYFUNC  rtxHashMapStr2IntCopy
#define HASHMAPFREEFUNC  rtxHashMapStr2IntFree
#define HASHMAPINSERTFUNC rtxHashMapStr2IntInsert
#define HASHMAPPUTFUNC   rtxHashMapStr2IntPut
#define HASHMAPSEARCHFUNC rtxHashMapStr2IntSearch
#define HASHMAPREMOVEFUNC rtxHashMapStr2IntRemove
#define HASHMAPSORTFUNC  rtxHashMapStr2IntSort

#include "rtxsrc/rtxHashMap.h"

#endif
