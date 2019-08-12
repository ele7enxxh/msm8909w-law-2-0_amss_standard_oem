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
 * @file rtxDList.h
 * Doubly-Linked List Utility Functions.
 */
#ifndef _RTXDLIST_H_
#define _RTXDLIST_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxCommonDefs.h"
/**
 * @defgroup ccfDList Doubly-Linked List Utility Functions
 *
 * The doubly-linked list utility functions provide common routines for
 * managing linked lists.  These lists are used to model XSD list and
 * repeating element types within the generated code.  This list type
 * contains forward and backward pointers allowing the list to be
 * traversed in either direction.
 *@{
 */

/**
 * @struct OSRTDListNode
 * This structure is used to hold a single data item within the list.
 * It contains a void pointer to point at any type of data item and
 * forward and backward pointers to the next and previous entries in
 * the list.
 */
typedef struct OSRTDListNode {
   void* data;            /**< Pointer to list data item. */
   struct OSRTDListNode* next; /**< Pointer to next node in list. */
   struct OSRTDListNode* prev; /**< Pointer to previous node in list. */
} OSRTDListNode;

/**
 * @struct OSRTDList
 * This is the main list structure.  It contains a count of the number
 * of elements in the list and pointers to the list head and tail
 * elements.
 */
typedef struct OSRTDList {
   OSSIZE count;               /**< Count of items in the list. */
   OSRTDListNode* head;        /**< Pointer to first entry in list. */
   OSRTDListNode* tail;        /**< Pointer to last entry in list. */
} OSRTDList;

struct OSCTXT;

typedef struct OSRTDListBuf {
   OSSIZE n;
   OSSIZE nMax;
   OSSIZE nAll;
   OSSIZE firstSegSz;
   OSSIZE   elemSize;
   OSRTDList tmplist;
   void** dataArray;
} OSRTDListBuf;

#ifndef DLISTBUF_SEG
#define DLISTBUF_SEG 16
#endif

typedef struct OSRTDListUTF8StrNode {
   OSRTDListNode node;
   OSUTF8CHAR    utf8chars[1];
} OSRTDListUTF8StrNode;

#ifdef __cplusplus
extern "C" {
#endif

/* Doubly-linked list functions */

/**
 * This function initializes a doubly linked list structure.  It sets the
 * number of elements to zero and sets all internal pointer values to NULL.
 * A doubly linked-list structure is described by the \c OSRTDList type.
 * Nodes of the list are of type \c OSRTDListNode.
 *
 * <p>Memory for the structures is allocated using the \c rtxMemAlloc
 * run-time function and is maintained within the context structure that
 * is a required parameter to all rtDList functions.  This memory is
 * released when \c rtxMemFree is called or the context is released.
 * Unless otherwise noted, all data passed into the list functions is
 * simply stored on the list by value (i.e. a deep-copy of the data is
 * not done).
 *
 * @param pList A pointer to a linked list structure to be initialized.
 */
EXTERNRT void rtxDListInit (OSRTDList* pList);

/**
 * This function appends an item to the linked list structure.  The data
 * item is passed into the function as a void pointer that can point to
 * an object of any type.  The \c rtxMemAlloc function is used to allocate
 * memory for the list node structure; therefore, all internal list memory
 * will be released whenever \c rtxMemFree is called.  The pointer
 * to the data item itself is stored in the node structure - a copy is
 * not made.
 *
 * @param pctxt         A pointer to a context structure.  This provides a
 *                        storage area for the function to store all working
 *                        variables that must be maintained between function
 *                        calls.
 * @param pList         A pointer to a linked list structure onto which the
 *                        data item will be appended.
 * @param pData         A pointer to the data item to be appended to the list.
 * @return              A pointer to an allocated node structure used to link
 *                        the given data value into the list.
 */
EXTERNRT OSRTDListNode* rtxDListAppend
(struct OSCTXT* pctxt, OSRTDList* pList, void* pData);

/**
 * This function appends an item to the linked list structure.  The data
 * item is passed into the function as a void pointer that can point to
 * an object of any type.  The \c rtxMemAlloc function is used to allocate
 * memory for the list node structure; therefore, all internal list memory
 * will be released whenever \c rtxMemFree is called.  The array passed in is
 * copied and a pointer to the copy is stored in the list.
 *
 * @param pctxt         A pointer to a context structure.  This provides a
 *                        storage area for the function to store all working
 *                        variables that must be maintained between function
 *                        calls.
 * @param pList         A pointer to a linked list structure onto which the
 *                        data item will be appended.
 * @param length        The size of the character array to be appended.                        
 * @param pData         A pointer to the character array.
 * @return              A pointer to an allocated node structure used to link
 *                        the given data value into the list.
 */
EXTERNRT OSRTDListNode* rtxDListAppendCharArray
(struct OSCTXT* pctxt, OSRTDList* pList, size_t length, char* pData);

/**
 * This function appends an \c OSRTDListNode to the linked list structure.  
 * The node data is a void pointer that can point to
 * an object of any type.  The \c rtxMemAlloc function is used to allocate
 * memory for the list node structure; therefore, all internal list memory
 * will be released whenever \c rtxMemFree is called.  The pointer
 * to the data item itself is stored in the node structure - a copy is
 * not made.
 *
 * @param pList         A pointer to a linked list structure onto which the
 *                        data item will be appended.
 * @param pListNode     A pointer to the node to be appended to the list.
 * @return              A pointer to an allocated node structure used to link
 *                        the given data value into the list.
 */
EXTERNRT OSRTDListNode* rtxDListAppendNode
(OSRTDList* pList, OSRTDListNode* pListNode);

/**
 * This function inserts an item into the linked list structure.  The data
 * item is passed into the function as a void pointer that can point to an
 * object of any type.  The \c rtxMemAlloc function is used to allocate
 * memory for the list node structure; therefore, all internal list memory
 * will be released when the \c rtxMemFree function is called.
 *
 * @param pctxt         A pointer to a context structure.  This provides a
 *                        storage area for the function to store all working
 *                        variables that must be maintained between function
 *                        calls.
 * @param pList         A pointer to a linked list structure into which the
 *                        data item is to be inserted.
 * @param idx           Zero-based index into list where the specified item
 *                        is to be inserted.
 * @param pData         A pointer to the data item to be inserted to the list.
 * @return              A pointer to an allocated node structure used to
 *                        link the given data value into the list.
 */
EXTERNRT OSRTDListNode* rtxDListInsert
(struct OSCTXT* pctxt, OSRTDList* pList, OSSIZE idx, void* pData);

EXTERNRT OSRTDListNode* rtxDListInsertNode
(OSRTDList* pList, OSSIZE idx, OSRTDListNode* pListNode);

/**
 * This function inserts an item into the linked list structure before the
 * specified element.  The \c rtxMemAlloc function is used to allocate
 * memory for the list node structure; therefore, all internal list memory
 * will be released when the \c rtxMemFree function is called.
 *
 * @param pctxt         A pointer to a context structure.  This provides a
 *                        storage area for the function to store all working
 *                        variables that must be maintained between function
 *                        calls.
 * @param pList         A pointer to a linked list structure into which the
 *                        data item is to be inserted.
 * @param node          The position in the list where the item is to be
 *                        inserted.  The item will be inserted before this
 *                        node or appended to the list if node is null.
 * @param pData         A pointer to the data item to be inserted to the list.
 * @return              A pointer to an allocated node structure used to
 *                        link the given data value into the list.
 */
EXTERNRT OSRTDListNode* rtxDListInsertBefore
(struct OSCTXT* pctxt, OSRTDList* pList, OSRTDListNode* node, void* pData);

/**
 * This function inserts an item into the linked list structure after the
 * specified element.  The \c rtxMemAlloc function is used to allocate
 * memory for the list node structure; therefore, all internal list memory
 * will be released when the \c rtxMemFree function is called.
 *
 * @param pctxt         A pointer to a context structure.  This provides a
 *                        storage area for the function to store all working
 *                        variables that must be maintained between function
 *                        calls.
 * @param pList         A pointer to a linked list structure into which the
 *                        data item is to be inserted.
 * @param node          The position in the list where the item is to be
 *                        inserted.  The item will be inserted after this
 *                        node or added as the head element if node is null.
 * @param pData         A pointer to the data item to be inserted to the list.
 * @return              A pointer to an allocated node structure used to
 *                        link the given data value into the list.
 */
EXTERNRT OSRTDListNode* rtxDListInsertAfter
(struct OSCTXT* pctxt, OSRTDList* pList, OSRTDListNode* node, void* pData);

/**
 * This function will return the node pointer of the indexed entry in
 * the list.
 *
 * @param pList         A pointer to a linked list structure.
 * @param idx           Zero-based index into list where the specified item
 *                        is located.  If the list contains fewer items
 *                        then the index, NULL is returned.
 * @return              A pointer to an allocated linked list node structure.
 *                        To get the actual data item, the \c data member
 *                        variable pointer within this structure must be
 *                        dereferenced.
 */
EXTERNRT OSRTDListNode*
rtxDListFindByIndex (const OSRTDList* pList, OSSIZE idx);

/**
 * This function will return the node pointer of the given data item
 * within the list or NULL if the item is not found.
 *
 * @param pList         A pointer to a linked list structure.
 * @param data          Pointer to the data item to search for.  Note that
 *                        comparison of pointer values is done; not the items
 *                        pointed at by the pointers.
 * @return              A pointer to an allocated linked list node structure.
 */
EXTERNRT OSRTDListNode*
rtxDListFindByData (const OSRTDList* pList, void* data);

/**
 * This function will return the index of the given data item
 * within the list or -1 if the item is not found.
 *
 * @param pList         A pointer to a linked list structure.
 * @param data          Pointer to the data item to search for.  Note that
 *                        comparison of pointer values is done; not the items
 *                        pointed at by the pointers.
 * @return              Index of item within the list or -1 if not found.
 */
EXTERNRT int rtxDListFindIndexByData (const OSRTDList* pList, void* data);

/**
 * This function will remove the given node from the list and free memory.
 * The data memory is not freed.  It might be released when the \c rtxMemFree
 * or \c rtFreeContext function is called with this context.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to a linked list structure.
 * @param node          Pointer to the list node to be removed.
 */
EXTERNRT void rtxDListFreeNode
   (struct OSCTXT* pctxt, OSRTDList* pList, OSRTDListNode* node);

/**
 * This function will remove the given node from the list.  The node
 * memory is not freed.  It will be released when the \c rtxMemFree
 * or \c rtFreeContext function is called with this context.
 *
 * @param pList         A pointer to a linked list structure.
 * @param node          Pointer to the list node to be removed.
 */
EXTERNRT void rtxDListRemove (OSRTDList* pList, OSRTDListNode* node);

/**
 * This function will free all of the dynamic memory used to hold
 * the list node pointers.  It does not free the data items because
 * it is unknown how the memory was allocated for these items.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to a linked list structure.
 */
EXTERNRT void rtxDListFreeNodes (struct OSCTXT* pctxt, OSRTDList* pList);

/**
 * This function will free all of the dynamic memory used to hold
 * the list node pointers and the data items.  In this case, it is
 * assumed that the \c rtxMemAlloc function was used to allocate
 * memory for the data items.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to a linked list structure.
 */
EXTERNRT void rtxDListFreeAll (struct OSCTXT* pctxt, OSRTDList* pList);

/**
 * This function converts a doubly linked list to an array.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to a linked list structure.
 * @param ppArray       A pointer to a pointer to the destination array.
 * @param pElemCount    A pointer to the number of elements already
 *                        allocated in \c ppArray.  If pElements is NULL, or
 *                        pElements is less than the number of nodes
 *                        in the list, then a new array is allocated
 *                        and the pointer is stored in \c ppArray.
 *                        Memory is allocated via calls to the \c rtxMemAlloc
 *                        function.
 * @param elemSize      The size of one element in the array.  Use the
 *                        \c sizeof() operator to pass this parameter.
 * @return              The number of elements in the returned array.
 */
EXTERNRT int rtxDListToArray
(struct OSCTXT* pctxt, OSRTDList* pList, void** ppArray,
 OSSIZE* pElemCount, OSSIZE elemSize);

/**
 * This function appends pointers to items in the given array to a doubly
 * linked list structure.  The array is assumed to hold an array of
 * values as opposed to pointers.  The actual address of each item in
 * the array is stored - a copy of each item is not made.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to the linked list structure onto which
 *                        the array items will be appended.
 * @param pArray        A pointer to the source array to be converted.
 * @param numElements   The number of elements in the array.
 * @param elemSize      The size of one element in the array.  Use the
 *                        \c sizeof() operator to pass this parameter.
 * @return              Completion status of operation: 0 (0) = success,
 *                        negative return value is error.
 */
EXTERNRT int rtxDListAppendArray
(struct OSCTXT* pctxt, OSRTDList* pList, void* pArray,
 OSSIZE numElements, OSSIZE elemSize);

/**
 * This function appends a copy of each item in the given array to a doubly
 * linked list structure.  In this case, the \c rtxMemAlloc function is
 * used to allocate memory for each item and a copy is made.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to the linked list structure onto which
 *                        the array items will be appended.
 * @param pArray        A pointer to the source array to be converted.
 * @param numElements   The number of elements in the array.
 * @param elemSize      The size of one element in the array.  Use the
 *                        \c sizeof() operator to pass this parameter.
 * @return              Completion status of operation: 0 (0) = success,
 *                        negative return value is error.
 */
EXTERNRT int rtxDListAppendArrayCopy
(struct OSCTXT* pctxt, OSRTDList* pList, const void* pArray,
 OSSIZE numElements, OSSIZE elemSize);

/**
 * This function concatanates all of the components in the given list
 * to form a UTF-8 string.  The list is assumed to contain null-terminated
 * character string components.  The given separator chraacter is
 * inserted after each list component.  The \c rtxMemAlloc function is
 * used to allocate memory for the output string.
 *
 * @param pctxt         A pointer to a context structure.
 * @param pList         A pointer to the linked list structure onto which
 *                        the array items will be appended.
 * @param ppstr         A pointer to a char pointer to hold output string.
 * @param sep           Separator character to add between string components.
 * @return              Completion status of operation: 0 (0) = success,
 *                        negative return value is error.
 */
EXTERNRT int rtxDListToUTF8Str
(struct OSCTXT* pctxt, OSRTDList* pList, OSUTF8CHAR** ppstr, char sep);



typedef int (*PEqualsFunc) (const void* a, const void* b,
                               const void* sortCtxt);

EXTERNRT OSRTDListNode* rtxDListInsertSorted
(struct OSCTXT* pctxt, OSRTDList* pList, void* pData, PEqualsFunc equalsFunc,
 void* sortCtxt);

EXTERNRT OSRTDListNode* rtxDListInsertNodeSorted
(OSRTDList* pList, OSRTDListNode* pListNode, PEqualsFunc equalsFunc,
 void* sortCtxt);

/**
 * @}
 */
#if defined(_MSC_VER)
/*  this disables 'conditional expression is constant' warnings */
/*  caused by using do { ... } while(0) in defines. */
#pragma warning(disable: 4127)
#endif

/* This rounds node size up to a 64-bit boundary to fix alignment issues */
#define OSRTDLISTNODESIZE ((sizeof(OSRTDListNode)+7)&(0xFFFFFFF8))

#define rtxDListAllocNodeAndData(pctxt,type,ppnode,ppdata) do { \
*ppnode = (OSRTDListNode*) \
rtxMemAlloc (pctxt, sizeof(type)+OSRTDLISTNODESIZE); \
if (0 != *ppnode) { \
(*ppnode)->data = (void*)((char*)(*ppnode)+OSRTDLISTNODESIZE); \
*ppdata = (type*)((*ppnode)->data); \
} else { *ppdata = 0; } \
} while (0)

#define rtxDListAppendData(pctxt,pList,pData) do { \
OSRTDListNode* _node = (OSRTDListNode*) \
(((char*)(pData)) - sizeof(OSRTDListNode)); \
_node->data = pData; \
rtxDListAppendNode (pList, _node); \
} while (0);

#define rtxDListFastInit(pList) do { \
if ((pList) != 0) { \
(pList)->head = (pList)->tail = (OSRTDListNode*) 0; \
(pList)->count = 0; } \
} while (0)

#define rtxDListFreeTailNode(pctxt,pList) \
rtxDListFreeNode(pctxt,pList,(pList)->tail)

#define rtxDListFreeHeadNode(pctxt,pList) \
rtxDListFreeNode(pctxt,pList,(pList)->head)

/* Doubly-linked list buffer functions */

EXTERNRT void rtxDListBufInit (OSRTDListBuf* pBuf,
   OSSIZE segSz, void** ppdata, OSSIZE elemSz);

EXTERNRT int rtxDListBufExpand (struct OSCTXT* pctxt, OSRTDListBuf* pBuf);

EXTERNRT int rtxDListBufToArray (struct OSCTXT* pctxt, OSRTDListBuf* pBuf);

#ifdef __cplusplus
}
#endif

#endif
