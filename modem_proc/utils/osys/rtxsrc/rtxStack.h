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
 * @file rtxStack.h
 * Simple FIFO stack for storing void pointers to any type of data.
 */
#ifndef _RTXSTACK_H_
#define _RTXSTACK_H_

#include "rtxsrc/rtxContext.h"

/**
 * @defgroup ccfStack Stack Utility Functions
 *
 * This is a simple stack structure with supporting push and pop functions.
 * Different initialization routines are provided to permit different
 * memory management schemes.
 *
 *@{
 */
/**
 * @struct _OSRTStack
 * This is the main stack structure.  It uses a linked list structure.
 */
typedef struct _OSRTStack {
   OSCTXT*   pctxt;
   OSRTDList dlist;
} OSRTStack;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function creates a new stack structure. It allocates memory for the
 * structure and calls rtxStackInit to initialize the structure.
 *
 * @param pctxt        A pointer to the context with which the stack is
 *                      associated.
 * @return             A pointer to an allocated stack structure.
 */
EXTERNRT OSRTStack* rtxStackCreate (OSCTXT* pctxt);

/**
 * This function initializes a stack structure. It sets the number of elements
 * to zero and sets all internal pointer values to NULL.
 *
 * @param pctxt        A pointer to the context with which the stack is
 *                      associated.
 * @param pStack       A pointer to a stack structure to be initialized.
 */
EXTERNRT void  rtxStackInit (OSCTXT* pctxt, OSRTStack* pStack);

/**
 * This macro tests if the stack is empty.
 *
 * @param stack        Stack structure variable to be tested.
 */
#define rtxStackIsEmpty(stack) (OSBOOL)((stack).dlist.count == 0)

/**
 * This function pops an item off the stack.
 *
 * @param pStack       The pointer to the stack structure from which the value
 *                       is to be popped. Pointer to the updated stack
 *                       structure.
 * @return             The pointer to the item popped from the stack
 */
EXTERNRT void* rtxStackPop (OSRTStack* pStack);

/**
 * This function pushes an item onto the stack.
 *
 * @param pStack       A pointer to the structure onto which the data item is
 *                       to be pushed. The pointer updated to the stack
 *                       structure
 * @param pData        A pointer to the data item to be pushed on the stack.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxStackPush (OSRTStack* pStack, void* pData);

/**
 * This functions returns the data item on the top of the stack.
 *
 * @param pStack       A pointer to the structure onto which the data item is
 *                       to be pushed. The pointer updated to the stack
 *                       structure
 * @return             Pointer to data item at top of stack or NULL if
 *                       stack empty.
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNRT void* rtxStackPeek (OSRTStack* pStack);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif
