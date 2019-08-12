#ifndef CORELIST_H
#define CORELIST_H

/*=============================================================================
  FILE:         CoreList.h
  
  OVERVIEW:     This file contains function and structure declaration for 
                CoreList. Currently, the implementation is not thread-safe.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/CoreList.h#1 $
$DateTime: 2015/02/25 02:53:43 $ 
$Author: pwbldsvc $
=============================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif
#include "list.h"

/*
===============================================================================

                           GLOBAL MACRO DEFINITIONS

===============================================================================
*/

/*
 * Users of the CoreList are recommended to use the macros below instead of
 * using equivalent list type/function directly.
 */
#define CoreListType                  list_type
#define CoreList_PopFront             list_pop_front
#define CoreList_PopItem              list_pop_item
#define CoreList_PeekFront(list)      \
                    (CoreListLinkType *)list_peek_front((list_type *)list)
#define CoreList_NextElement(x)       (CoreListLinkType *)x->link.next_ptr

/*
===============================================================================

                            GLOBAL TYPE DEFINITIONS

===============================================================================
*/

/**
 * CoreListResult
 *
 * @brief Enum to indicate return values from different List functions.
 */
typedef enum CoreListResult
{
  CORE_LIST_SUCCESS = 0,               /* Sucess of the function */
  CORE_LIST_VAL_NOT_FOUND,             /* Value being searched not found */
  CORE_LIST_NO_MEMORY,                 /* Memory allocation error */
  CORE_LIST_ERROR,                     /* General core list error */
  CORE_LIST_UINT32 = 0x7FFFFFF         /* Forces enum to be of 4 byte */
}CoreListResult;

/**
 * CoreListLinkType
 *
 * @brief Basic core list element type.
 */
typedef struct CoreListLinkType
{
  /* Link to the next element */
  list_link_type link;

  /* Pointer to data. */
  void *data;
}CoreListLinkType;

/**
 * CoreListCleanUpFuncType
 *
 * @brief This function will be called on data of each element of the list when
 *        CoreList_Clear function is called on the list.
 *
 * @param data: Pointer to data of the list element.
 */
typedef void (*CoreListCleanUpFuncType) (void *data);

/**
 * CoreListPredicateType
 *
 * @brief Pointer to function that will be used while removing elements from the
 *        list conditionally. 
 *
 * If this function returns true (nonzero value) for data part of the list 
 * element, that element will be removed from the list.
 *
 * @param data: Pointer to data of the list element.
 *
 * @return True (nonzero) if the data satisfies some condition else false (0).
 */
typedef int (*CoreListPredicateType) (void *data);

/*
===============================================================================

                          GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * CoreList_Init
 *
 * @brief Initializes the CoreListType object.
 *
 ******************************************************************************
 * Almost all CoreList functions assume that CoreListType is initialized and
 * hence this function must be called before calling other functions. This
 * function is called only once per list object usually.
 *
 * If list is re-initialized, it may result into loss of references of all the
 * existing list elements. 
 ******************************************************************************
 *
 * @param list: List that will be initialized
 */
void CoreList_Init(CoreListType *list);

/**
 * CoreList_Size
 *
 * @brief Returns the number of elements in the list passed as argument.
 *
 * @Dependencies: CoreList_Init
 * The list passed as an argument must be initialized (using CoreList_Init)
 * before calling this function. Not doing so may result into undefined behavior
 * (mostly crash).
 *
 * @param list: List for which we want to know the number of elements
 *
 * @return Number of elements in the list
 */
uint32 CoreList_Size(const CoreListType *list);

/**
 * CoreList_PushBack
 *
 * @brief Inserts the list element at the end of list containing data pointed
 *        by data.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: Pointer to list data to be inserted.
 * @param data: Pointer to data to be inserted in the list.
 *
 * @return Success if data is inserted successfully else error (as defined in
 *         CoreListResult).
 */
CoreListResult CoreList_PushBack(CoreListType *list, void *data);

/**
 * CoreList_Clear
 * 
 * @brief Calls clear_func on data field of each element of list one by one and
 *        clears that element from the list.
 *
 * It should be noted that memory pointed by data field of each element in the
 * list is not freed by this routine. It is also possible that after calling
 * this function, caller may lose the references to the objects pointed by data
 * fields of list elements. It is upto the caller to manage that memory.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List the elements will be deleted from.
 * @param clear_func: Function to be called on each element before it is 
 *                    destroyed.
 */
void CoreList_Clear(CoreListType *list, CoreListCleanUpFuncType clear_func);

/**
 * CoreList_Copy
 *
 * @brief Copies all the elements from source list to target list.
 *
 * The target list is expected to be empty. If it is not, there are chances that
 * references to the elements are lost. So if caller wants to use the existing 
 * list, it is up to him to manage elements of that list.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param target: List where all elements are copied.
 * @param source: List whose elements will be copied.
 *
 * @return Success if data is inserted successfully else error (as defined in
 *         CoreListResult).
 */
CoreListResult CoreList_Copy(CoreListType *target, CoreListType *source);

/**
 * CoreList_RemoveElement
 *
 * @brief Removes the element from the list pointed by element.
 *
 * It should be noted that this function wil not free up the memory pointed
 * by data of the element. It is up to the caller to manage that memory.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List from the element will be removed.
 * @param element: Element to remove from the list.
 */
void CoreList_RemoveElement(CoreListType *list, CoreListLinkType *element);

/**
 * CoreList_RemoveVal
 *
 * @brief This function removes all the elements from the list for which value
 *        matches the one passed in argument.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List from which elements will be removed
 * @param val: val against which all elements' data will be compared.
 * @param clean_up: Pointer to clean up function that will be called for data 
 *                  part of each list elements (Can be used if user wants to 
 *                  free data while removing it from the list).
 */
void CoreList_RemoveVal(CoreListType *list, 
                        void *val, 
                        CoreListCleanUpFuncType clean_up);

/**
 * CoreList_RemoveIf
 *
 * @brief This function removes all the elements from the list for which
 *        list_predicate_fcn returns true.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List from which elements will be removed.
 * @param predicate: Pointer to function that will be called on each elements.
 * @param clean_up: Pointer to clean up function that will be called for data 
 *                  part of the list elements for which predicate function 
 *                  returns true (Can be used if user wants to free data while 
 *                  removing the corresponding element from the list).
 */
void CoreList_RemoveIf(CoreListType *list, 
                       CoreListPredicateType predicate,
                       CoreListCleanUpFuncType clean_up);

/**
 * CoreList_InsertBefore
 *
 * @brief This function inserts insert_val into the list just before 
 *        insert_before_val.
 *
 * In case of multiple elements are having insert_before_val, the insert_val 
 * will be inserted before the first one.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List in which we want to insert the data.
 * @param insert_val: Value which we want to insert in the list.
 * @param insert_before_val: Value before we want to insert insert_val.
 *
 * @return Success if data is inserted successfully else error (as defined in
 *         CoreListResult).
 */
CoreListResult CoreList_InsertBefore(CoreListType *list, 
                                     void *insert_val, 
                                     void *insert_before_val);


/**
 * CoreList_InsertAfter
 *
 * @brief This function inserts insert_val into the list just after 
 *        insert_after_val.
 *
 * If there are multiple elements having insert_after_val in the list, 
 * insert_val will be inserted after the first one.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List in which we want to insert the data.
 * @param insert_val: Value which we want to insert in the list.
 * @param insert_after_val: Value after we want to insert insert_val.
 *
 * @return Success if data is inserted successfully else error (as defined in
 *         CoreListResult).
 */
CoreListResult CoreList_InsertAfter(CoreListType *list, 
                                    void *insert_val, 
                                    void *insert_after_val);

/**
 * CoreList_Reverse
 *
 * @brief This function reverses the list passed as an argument.
 *
 * For example, if the list contains A -> B -> C -> D, calling this function
 * on that list will result in list with list elements D -> C -> B -> A.
 *
 * @Dependencies: CoreList_Init
 * As described in CoreList_Size.
 *
 * @param list: List we want to reverse.
 */
void CoreList_Reverse(CoreListType *list);

#ifdef __cplusplus
}
#endif

#endif
