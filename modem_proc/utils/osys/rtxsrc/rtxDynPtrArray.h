/*
 * Copyright (C) 2008 by Objective Systems, Inc.
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
 * @file rtxDynPtrArray.h - Implementation of a dynamic pointer array.
 */
#ifndef _RTXDYNPTRARRAY_H_
#define _RTXDYNPTRARRAY_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
   OSUINT16     capacity;       /* Current size of the structure        */
   OSUINT16     count;          /* Number of pointers in array          */
   void**       data;           /* The actual pointer values            */
} OSRTDynPtrArray;

/**
 * This function initializes a new dynamic pointer array structure.  
 * Memory is allocated for the initial capacity of pointers.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pArray       Pointer to dynamic pointer array structure.
 * @param initialSize  Initial capacilty of the array.  The size will 
 *                       doubled on each expansion.  If zero is provided, 
 *                       a default size will be used.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynPtrArrayInit
(OSCTXT* pctxt, OSRTDynPtrArray* pArray, OSUINT16 initialSize);

/**
 * This function adds a pointer to the end of the array and expands the 
 * array if necessary.
 *
 * @param pctxt        Pointer to a context structure.
 * @param pArray       Pointer to dynamic pointer array structure.
 * @param ptr          Pointer to be added to the array.
 * @return             Status of operation: zero if success or a negative
 *                       error code if failure.
 */
EXTERNRT int rtxDynPtrArrayAppend 
(OSCTXT* pctxt, OSRTDynPtrArray* pArray, void* ptr);

#ifdef __cplusplus
}
#endif

#endif
