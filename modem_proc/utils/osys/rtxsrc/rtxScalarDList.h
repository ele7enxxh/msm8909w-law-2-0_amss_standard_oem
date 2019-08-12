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
 * @file rtxScalarDList.h
 * Doubly-linked list utility functions to hold scalar data variables.
 */
#ifndef _RTXSCALARDLIST_H_
#define _RTXSCALARDLIST_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct OSCTXT;

/* Kept externally to avoid breakage on intolerant compilers like aCC. */
typedef enum {
   ScalarDList_undefined,
   ScalarDList_double,
   ScalarDList_float,
   ScalarDList_int32,
   ScalarDList_uint32,
   ScalarDList_int64,
   ScalarDList_uint64
} e_ident;       /**< Data item identifier */

/**
 * @defgroup rtxScalarDList Scalar Doubly-Linked List Utility Functions
 *
 * The doubly-linked list utility functions provide common routines for
 * managing linked lists.  This module is identical to the rtxDList module
 * except that the data varaibles that can be added to the lists are
 * scalars (integer, double, float, etc.) whereas the standard rtxDList
 * type hold pointers to more complex data items.
 *@{
 */
/**
 * @struct OSRTScalarDListNode
 * This structure is used to hold a single data item within the list.
 * The data item is a union of all of the possible scalar types it can
 * hold. The node also contains forward and backward pointers to the
 * next and previous entries in the list.
 */
typedef struct OSRTScalarDListNode {
   e_ident ident;

   union {
      OSDOUBLE dfltval;   /**< Double prec floating point value */
      OSFLOAT  fltval;    /**< Single prec floating point value */
      OSINT32  i32val;    /**< 32-bit signed integer */
      OSUINT32 ui32val;   /**< 32-bit unsigned integer */
#if !defined(_NO_INT64_SUPPORT)
      OSINT64  i64val;    /**< 64-bit signed integer */
      OSUINT64 ui64val;   /**< 64-bit unsigned integer */
#endif
   } u;

   struct OSRTScalarDListNode* next; /**< Pointer to next node in list */
   struct OSRTScalarDListNode* prev; /**< Pointer to previous node in list */
} OSRTScalarDListNode;

/**
 * @struct OSRTScalarDList
 * This is the main list structure.  It contains a count of the number
 * of elements in the list and pointers to the list head and tail
 * elements.
 */
typedef struct OSRTScalarDList {
   OSUINT32 count;              /**< Count of items in the list  */
   OSRTScalarDListNode* head;   /**< Pointer to first entry in list  */
   OSRTScalarDListNode* tail;   /**< Pointer to last entry in list  */
} OSRTScalarDList;

/**
 * This function initializes a doubly linked list structure.  It sets the
 * number of elements to zero and sets all internal pointer values to NULL.
 * A doubly-linked scalar list structure is described by the \c
 * OSRTScalarDList type.  Nodes of the list are of type \c OSRTScalarDListNode.
 *
 * @param pList A pointer to a linked list structure to be initialized.
 */
EXTERNRT void rtxScalarDListInit (OSRTScalarDList* pList);

/**
 * This set of functions appends an item of the given scalar type to
 * the linked list structure.  Separate functions exist for all of the
 * different supported scalar types.
 *
 * @param pctxt         A pointer to a context structure.  This provides a
 *                        storage area for the function to store all working
 *                        variables that must be maintained between function
 *                        calls.
 * @param pList         A pointer to a linked list structure onto which the
 *                        data item will be appended.
 * @param value         Data item to be appended to the list.
 * @return              A pointer to an allocated node structure used to link
 *                        the given data value into the list.
 */
EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendDouble
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSDOUBLE value);

EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendFloat
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSFLOAT value);

EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendInt32
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSINT32 value);

EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendUInt32
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSUINT32 value);

#if !defined(_NO_INT64_SUPPORT)
EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendInt64
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSINT64 value);

EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendUInt64
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSUINT64 value);
#endif

/**
 * This function is used to append a node to the linked list.  This
 * can be used instead of a scalar value append function.  It requires
 * the user to allocate and populate the list node structure.
 *
 * @param pList         A pointer to a linked list structure onto which the
 *                        list node will be appended.
 * @param pListNode     List node structure to be appended to the list.  If
 *                        this memory is to be released with the standard
 *                        list memory free function, then it must be
 *                        allocated using the rtxMemAlloc function.
 * @return              A pointer to an allocated node structure used to link
 *                        the given data value into the list.  This is the
 *                        node structure that was passed in.
 */
EXTERNRT OSRTScalarDListNode* rtxScalarDListAppendNode
(OSRTScalarDList* pList, OSRTScalarDListNode* pListNode);

/**
 * This function is used to insert a node into the linked list.
 *
 * @param pList         A pointer to a linked list structure onto which the
 *                        list node will be appended.
 * @param idx           Zero-based index into list where the specified node
 *                        is to be inserted.
 * @param pListNode     List node structure to be appended to the list.  If
 *                        this memory is to be released with the standard
 *                        list memory free function, then it must be
 *                        allocated using the rtxMemAlloc function.
 * @return              A pointer to an allocated node structure used to link
 *                        the given data value into the list.  This is the
 *                        node structure that was passed in.
 */
EXTERNRT OSRTScalarDListNode* rtxScalarDListInsertNode
(OSRTScalarDList* pList, OSUINT32 idx, OSRTScalarDListNode* pListNode);

/**
 * This function will return the node pointer of the indexed entry in
 * the list.
 *
 * @param pList         A pointer to a linked list structure.
 * @param idx           Zero-based index into list where the specified item
 *                        is located.  If the list contains fewer items
 *                        then the index, NULL is returned.
 * @return              A pointer to an allocated linked list node structure.
 *                        To get the actual data item, the ident field
 *                        must be examined to determine what type of value
 *                        is stored in the union structure.
 */
EXTERNRT OSRTScalarDListNode*
rtxScalarDListFindByIndex (const OSRTScalarDList* pList, OSUINT32 idx);

/**
 * This function will remove the given node from the list and free memory.
 * It is assumed that memory for the list node structure was allocated
 * using the \c rtxMemAlloc function.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to a linked list structure.
 * @param node          Pointer to the list node to be removed.
 */
EXTERNRT void rtxScalarDListFreeNode
(struct OSCTXT* pctxt, OSRTScalarDList* pList, OSRTScalarDListNode* node);

/**
 * This function will remove the given node from the list.
 *
 * @param pList         A pointer to a linked list structure.
 * @param node          Pointer to the list node to be removed.
 */
EXTERNRT void rtxScalarDListRemove
(OSRTScalarDList* pList, OSRTScalarDListNode* node);

/**
 * This function will free all of the dynamic memory used to hold
 * the list node pointers.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to a linked list structure.
 */
EXTERNRT void rtxScalarDListFreeNodes
(struct OSCTXT* pctxt, OSRTScalarDList* pList);

#ifdef __cplusplus
}
#endif
/**
 * @} rtxScalarDList
 */
#endif
