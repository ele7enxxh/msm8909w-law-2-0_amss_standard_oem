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
 * @file rtxArrayList.h
 * ArrayList functions.
 */
#ifndef _RTXARRAYLIST_H_
#define _RTXARRAYLIST_H_

#include "rtxsrc/rtxContext.h"

#define OSRT_ARRAYLIST_CAPACITY 10

typedef struct _OSRTArrayList {
   OSRTDList segments;
   OSSIZE    segmentSize;
   OSSIZE    size;
   OSSIZE    dataSize;
   OSBOOL    (*equalsFunc)(void*,void*,OSSIZE);
} OSRTArrayList;

typedef struct _OSRTArrayListIter {
   const OSRTArrayList* pArrayList;
   OSRTDListNode* pSegment;
   OSSIZE         index;
} OSRTArrayListIter;

/* Iteration is done as follows:
 * rtxArrayListInitIter (&iter, &arrayList, 0);
 * while (rtxArrayListHasNextItem (&iter)) {
 *    pdata = rtxArrayListNextItem (&iter);
 *    ...
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function initializes an array list structure.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param capacity      Initial capacity of the array or zero to use default.
 */
EXTERNRT void rtxArrayListInit
(OSRTArrayList* pArrayList, OSSIZE capacity);

/**
 * This function creates a new array list to hold the initial capacity of
 * elements.
 *
 * @param pctxt         Pointer to a context structure.
 * @param capacity      Initial capacity of the array or zero to use default.
 * @return              Allocated array list structure or NULL if insufficient
 *                        dynamic memory is available to hold the structure.
 */
EXTERNRT OSRTArrayList* rtxNewArrayList (OSCTXT* pctxt, OSSIZE capacity);

/**
 * This function frees all dynamic memory held by the array list.  It does
 * not free the array list structure itself, just the internal data array.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure.
 */
EXTERNRT void rtxFreeArrayList (OSCTXT* pctxt, OSRTArrayList* pArrayList);

/**
 * This function adds an element to an array list.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to add.
 * @param pindex        Pointer to index variable to receive index at
 *                        which entry was added.
 * @return              Zero if item was successfully added; a negative
 *                        status code if error.
 */
EXTERNRT int rtxArrayListAdd
(OSCTXT* pctxt, OSRTArrayList* pArrayList, void* pdata, OSSIZE* pindex);

/**
 * This function removes an element from an array list.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to remove.
 */
EXTERNRT void rtxArrayListRemove
(OSCTXT* pctxt, OSRTArrayList* pArrayList, void* pdata);

/**
 * This function removes the element at the given index from the array list.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param idx           Index of item to remove.  -1 indicates tail item
 *                        should be removed.
 */
EXTERNRT void rtxArrayListRemoveIndexed
(OSCTXT* pctxt, OSRTArrayList* pArrayList, OSSIZE idx);

/**
 * This function inserts an element at the given position in the array list.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to insert.
 * @param idx           Index of location where item should be inserted.
 * @return              Zero if item was successfully added; a negative
 *                        status code if error.
 */
EXTERNRT int rtxArrayListInsert
(OSCTXT* pctxt, OSRTArrayList* pArrayList, void* pdata, OSSIZE idx);

/**
 * This function replaces (overwrites) the element at the given position in
 * the array list with the new element.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to insert.
 * @param idx           Index of location where item should be inserted.
 * @return              Zero if item was successfully added; a negative
 *                        status code if error.
 */
EXTERNRT int rtxArrayListReplace
(OSRTArrayList* pArrayList, void* pdata, OSSIZE idx);

/**
 * This function gets the indexed data item from the array list.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param idx           Index of location where item should be inserted.
 * @return              Pointer to indexed data item or NULL if index
 *                        is greater than max index in list.
 */
EXTERNRT void* rtxArrayListGetIndexed
(const OSRTArrayList* pArrayList, OSSIZE idx);

/**
 * This function returns the index of the given data item in the list.
 * The 'equals' callback function is used to do comparisons.
 *
 * @param pArrayList    Pointer to array list structure to initialize.
 * @param pdata         Pointer to data item to find in list.
 * @return              Index of item in list or -1 if not found.
 */
EXTERNRT int rtxArrayListIndexOf (OSRTArrayList* pArrayList, void* pdata);

/* Iteration */

/**
 * This function initializes an array list iterator with the given start
 * index.
 *
 * @param piter         Pointer to array list iterator structure.
 * @param pArrayList    Pointer to array list structure.
 * @param startIndex    Index from which iteration is to start.
 * @return              Zero if successfully initialized or RTERR_OUTOFBND
 *                        if start index is beyond the current size of
 *                        the array list.
 */
EXTERNRT int rtxArrayListInitIter (OSRTArrayListIter* piter,
   const OSRTArrayList* pArrayList, OSSIZE startIndex);

/**
 * This function determines if another element exists at the next
 * sequential position in the array list.
 *
 * @param piter         Pointer to array list iterator structure.
 * @return              True if another element exists; false otherwise.
 */
EXTERNRT OSBOOL rtxArrayListHasNextItem (OSRTArrayListIter* piter);

/**
 * This function gets the next item from the array list.
 *
 * @param piter         Pointer to array list iterator structure.
 * @return              Pointer to next item or null if beyond the
 *                        end of the array.
 */
EXTERNRT void* rtxArrayListNextItem (OSRTArrayListIter* piter);

#ifdef __cplusplus
}
#endif

#endif
