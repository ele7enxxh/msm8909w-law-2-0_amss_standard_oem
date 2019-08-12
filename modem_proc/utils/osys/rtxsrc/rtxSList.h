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
 * @file rtxSList.h
 */
#ifndef _RTXSLIST_H_
#define _RTXSLIST_H_

#include "rtxsrc/rtxContext.h"

/**
 * @defgroup ccfSList Linked List Utility Functions
 *
 * Singly linked list structures have only a single link pointer and can
 * therefore only be traversed in a single direction (forward). The node
 * structures consume less memory than those of a doubly linked list.
 *
 * Another difference between the singly linked list implementation and doubly
 * linked lists is that the singly linked list uses conventional memory
 * allocation functions (C malloc and free) for less storage instead of the
 * rtxMem functions. Therefore, it is not a requirement to have an initialized
 * context structure to work with these lists. However, performance may suffer
 * if the lists become large due to the use of non-optimized memory management.
 *@{
 */

/**
 * @struct _OSRTSListNode
 * This structure is used to hold a single data item within the list.
 * It contains a void pointer to point at any type of data item and
 * forward pointer to the next entry in the list.
 */
typedef struct _OSRTSListNode {
   void* data;                     /**< Pointer to list data item. */
   struct _OSRTSListNode* next;  /**< Pointer to next node in list. */
} OSRTSListNode;

struct OSCTXT;

/**
 * @struct _OSRTSList
 * This is the main list structure.  It contains a count of the number
 * of elements in the list and pointers to the list head and tail
 * elements.
 */
typedef struct _OSRTSList {
   OSUINT32 count;              /**< Count of items in the list. */
   OSRTSListNode* head;         /**< Pointer to first entry in list. */
   OSRTSListNode* tail;         /**< Pointer to last entry in list. */
   struct OSCTXT* pctxt;
} OSRTSList;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function initializes a singly linked list structure. It sets the number
 * of elements to zero and sets all internal pointer values to NULL.
 *
 * @param pList        A pointer to a linked list structure to be initialized.
 */
EXTERNRT void  rtxSListInit (OSRTSList* pList);

/**
 * This function is similar to rtxSListInit but it also sets the pctxt (pointer
 * to a context structure member of OSRTSList structure). This context will be
 * used for further memory allocations; otherwise the standard malloc is used.
 *
 * @param pctxt        A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pList        A pointer to a linked list structure to be initialized.
 */
EXTERNRT void  rtxSListInitEx (OSCTXT* pctxt, OSRTSList* pList);

/**
 * This function removes all nodes from the linked list structure and releases
 * memory that was allocated for storing node structures (OSRTSListNode).
 * The data will not be freed.
 *
 * @param pList        A pointer to a linked list onto which the data item is
 *                       to be appended.
 */
EXTERNRT void  rtxSListFree (OSRTSList* pList);

/**
 * This function removes all nodes from the linked list structure and releases
 * memory that was allocated for storing node structures (OSRTSListNode).
 * It also frees the data structures which are assumed to have been
 * allocated using the rtxMemAlloc function.
 *
 * @param pList        A pointer to a linked list onto which the data item is
 *                       to be appended.
 */
EXTERNRT void  rtxSListFreeAll (OSRTSList* pList);

/**
 * This function creates a new linked list structure. It allocates memory for
 * the structure and calls rtxSListInit to initialize the structure.
 *
 * @return             A pointer to the allocated linked list structure.
 */
EXTERNRT OSRTSList* rtxSListCreate (void);

/**
 * The rtxSListAppend function appends an item to linked list structure. The
 * data is passed into the function as a void that can point to an object of
 * any type.
 *
 * @param pctxt        A pointer to a context structure. This provides a
 *                       storage area for the function to store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @return             A pointer to the allocated linked list structure.
 */
EXTERNRT OSRTSList* rtxSListCreateEx (OSCTXT* pctxt);

/**
 * This function appends an item to a linked list structure. The data item is
 * passed into the function as a void parameter that can point to an object of
 * any type.
 *
 * @param pList        A pointer to a linked list onto which the data item is
 *                       to be appended.
 * @param pData        A pointer to a data item to be appended to the list.
 * @return             A pointer to the allocated linked list structure.
 */
EXTERNRT OSRTSListNode* rtxSListAppend (OSRTSList* pList, void* pData);

/**
 * This function finds an item in the linked list structure. The data item is
 * passed into the function as a void pointer that can point to an object of
 * any type. If the appropriate node is found in the list this function returns
 * TRUE, otherwise FALSE.
 *
 * @param pList        A pointer to a linked list onto which the data item is
 *                       to be appended.
 * @param pData        A pointer to a data item to be appended to the list.
 * @return             TRUE, if the node is found, otherwise FALSE.
 */
EXTERNRT OSBOOL rtxSListFind (OSRTSList* pList, void* pData);

/**
 * This function finds an item in the linked list structure and removes it from
 * the list. The data item is passed into the function as a void pointer that
 * can point to an object of any type.
 *
 * @param pList        A pointer to a linked list onto which the data item is
 *                       to be appended.
 * @param pData        A pointer to a data item to be appended to the list.
 */
EXTERNRT void rtxSListRemove (OSRTSList* pList, void* pData);

/**
 * @}
 */

#define OSALLOCELEMSNODE(pctxt,type) \
(type*) (((char*)rtxMemAllocZ (pctxt, sizeof(type) + \
sizeof(OSRTSListNode))) + sizeof(OSRTSListNode))


#ifdef __cplusplus
}
#endif

#endif
