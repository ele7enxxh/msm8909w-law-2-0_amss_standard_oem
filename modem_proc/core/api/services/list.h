#ifndef LIST_H
#define LIST_H
/**
  @file list.h
  @brief Linked list and ordered link list routines and definitions

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_list" group 
      description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================
   Lists are maintained as a singly linked list for unordered lists
   called "lists" and as doubly linked lists for ordered lists called
   "ordered lists".  An item in a list can be of any type as long as
   the type contains a field of type list_link_type as its first field.
   An item in an ordered list must contain a field of type
   ordered_list_link_type as its first field.

   In general, a list is a linked list of items with a pointer to the
   front of the list and a pointer to the end of the list.

   +-----+
   |List |
   +-----+     +---------+     +---------+
   |Front|---->|Link     |---->|Link     |---->NULL
   +-----+     +---------+     +---------+
   |Back |--+  |User Data|     |User Data|
   +-----+  |  +---------+     +---------+
   |Size |  |                       ^
   +-----+  +-----------------------+


   For ordered lists, the "Link" field as has a pointer to the
   previous item as well.

   Ordered lists are ordered by "weight" where weight is any 32 bit
   weighting value the users chooses.  Ordered lists can be
   ascending or descending.

   Ascending lists are ordered with smallest items at the front of
   the list and larger items at the back of the list.  

   e.g. If the weights arrive as 7, 5, 9, 2, 1 an ascending list
        would be stored as:
            1, 2, 5, 7, 9

        Conversely, a descending list is kept with larger weights
        at the head of the list.  Using the values from the previous
        example the list of descending type would be stored as:
            9, 7, 5, 2, 1

   The penalty for an ordered list occurs at insertion (which takes
   time linear in the size of the list) with deletions taking
   constant time.

   Items of equal weight are inserted based on an additional
   initialization flag indicating whether to insert equal weight items
   in "strictly less than order" or "less than or equal to order".

   The difference is illustrated in this example.  Say two items of
   weight 10 are to be pushed into the list.  One must arrive before
   the other, so let one of the items be called 10[1] and the other
   10[2] indicating the item 10[1] arrives before 10[2].  Then the
   lists after the push for each case would be:

   Strictly Less Than (<)

      10[1], 10[2]

   Less Than or Equal To (<=)

      10[2], 10[1]

   For linear searches, the algorithm is the same for ordered and 
   unordered lists, always starting at the item at the front of the
   list and proceeding to the item at the end of the list.
===========================================================================*/
/*
Copyright (c) 2000-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/list.h_v   1.0   24 May 2001 09:34:30   karthick  $
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/list.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/13/11   sri     Added functions list_destroy and ordered_list_destroy
08/02/11   ab      removed INTLOCKs and used mutexs in place of them
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File
08/22/00    gr     Modified some data structures and function signatures
08/09/00   jct     Created
============================================================================ */
/** @addtogroup utils_list
@{ */
/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "comdef.h"

#if defined(FEATURE_QUBE)
  #include <qube.h>
  /*  */
  #include "assert.h"
#elif defined(FEATURE_WINCE)
  #error code not present
#else 
  #include "rex.h"
#endif

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#ifndef NULL
   #define NULL 0
#endif

/* Uncomment the following line to enable sanity checks.
** Note that this will slow down the list operations considerably.
*/
// #define LIST_DEBUG

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

/** Size of the list. */
typedef unsigned long list_size_type;
/** Weight of the ordered list. */
typedef unsigned long ordered_list_weight_type;

/** @brief Contains the list link. */
typedef struct list_link_struct {
   struct list_link_struct *next_ptr;  /**<  Pointer to the next item. */
} list_link_type;

/** @brief Contains the ordered list link. */
typedef struct ordered_list_link_struct {
   struct ordered_list_link_struct *next_ptr;  /**<  Pointer to the next 
                                                     item. */
   struct ordered_list_link_struct *prev_ptr;  /**<  Pointer to the previous
                                                     item. */
   ordered_list_weight_type         weight;    /**<  Weight of the list. */
} ordered_list_link_type;

/** @brief Contains the list. */
typedef struct {
   list_link_type *front_ptr;  /**< Pointer to the front of the list. */
   list_link_type *back_ptr;   /**< Pointer to the back of the list. */
   list_size_type  size;       /**< Size of the list. */
   uint32          list_initialized;
   /* List critical section object */
   #ifdef FEATURE_QDSP6
      qmutex_t pkcsListCriticalSection;
   #else
      #ifdef FEATURE_WINCE
         #error code not present
#else
         rex_crit_sect_type pkcsListCriticalSection;
      #endif   
   #endif
} list_type;

/** @brief Contains the ordered list. */
typedef struct {
   ordered_list_link_type  *front_ptr;  /**< Pointer to the front of the 
                                             list. */
   ordered_list_link_type  *back_ptr;   /**< Pointer to the back of the 
                                             list. */
   list_size_type           size;       /**< Size of the list. */
   unsigned long            type;       /**< Type of list. */
   uint32                   list_initialized;
   /* List critical section object */
   #ifdef FEATURE_QDSP6
      qmutex_t pkcsListCriticalSection;
   #else
      #ifdef FEATURE_WINCE
         #error code not present
#else
         rex_crit_sect_type pkcsListCriticalSection;
      #endif   
   #endif
} ordered_list_type;

/**
  Used by the searching functions to determine if an item is in the list. 
  A nonzero must be returned if the element is found or if the search is to be 
  terminated; otherwise, 0 is returned.

  @param[in] item_ptr    Item on which to perform the search.
  @param[in] compare_val Value to compare.

  @return
  Nonzero -- Either the element was found or the search is to be 
  terminated. \n
  0 -- Element was not found. 

  @dependencies
  None.
*/
typedef int (*list_compare_func_type)( void* item_ptr, void* compare_val );

typedef enum {
   ORDERED_LIST_ASCENDING  = 0x00000001,  /**< Ascending ordered list. */
   ORDERED_LIST_DESCENDING = 0x00000002,  /**< Descending ordered list. */
   ORDERED_LIST_PUSH_SLT   = 0x00001000,  /**< Push SLT ordered list. */
   ORDERED_LIST_PUSH_LTE   = 0x00002000,  /**< Push LTE ordered list. */
   /** @cond 
   */
   ORDERED_LIST_MAX = 0x7FFFFFFF
   /** @endcond */
} ordered_list_config_type;

/** @cond 
*/
#define ORDERED_LIST_ASCENDING_PUSH_SLT  ( ORDERED_LIST_ASCENDING  | ORDERED_LIST_PUSH_SLT )
#define ORDERED_LIST_ASCENDING_PUSH_LTE  ( ORDERED_LIST_ASCENDING  | ORDERED_LIST_PUSH_LTE )
#define ORDERED_LIST_DESCENDING_PUSH_SLT ( ORDERED_LIST_DESCENDING | ORDERED_LIST_PUSH_SLT )
#define ORDERED_LIST_DESCENDING_PUSH_LTE ( ORDERED_LIST_DESCENDING | ORDERED_LIST_PUSH_LTE )

#define ORDERED_LIST_SORT_ORDER_MASK 0x0000000F
#define ORDERED_LIST_PUSH_TYPE_MASK  0x0000F000
/** @endcond */

/* ------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------ */
/** @cond 
*/
   #define LIST_INTERCEPT_LIST_INIT( list_ptr )
   #define LIST_INTERCEPT_LIST_DESTROY( list_ptr )
   #define LIST_INTERCEPT_LIST_PUSH_FRONT( list_ptr, item_link_ptr )
   #define LIST_INTERCEPT_LIST_POP_FRONT( list_ptr )
   #define LIST_INTERCEPT_LIST_PUSH_BACK( list_ptr, item_link_ptr )
   #define LIST_INTERCEPT_LIST_POP_BACK( list_ptr )
   #define LIST_INTERCEPT_LIST_SIZE( list_ptr )
   #define LIST_INTERCEPT_LIST_PEEK_FRONT( list_ptr )
   #define LIST_INTERCEPT_LIST_PEEK_BACK( list_ptr )
   #define LIST_INTERCEPT_LIST_PEEK_NEXT( list_ptr, item_after_which_to_peek )
   #define LIST_INTERCEPT_LIST_PEEK_PREV( list_ptr, item_before_which_to_peek )
   #define LIST_INTERCEPT_LIST_PUSH_BEFORE( list_ptr, item_to_push_ptr, item_to_push_before_ptr )
   #define LIST_INTERCEPT_LIST_PUSH_AFTER( list_ptr, item_to_push_ptr, item_to_push_after_ptr )
   #define LIST_INTERCEPT_LIST_POP_ITEM( list_ptr, item_to_pop_ptr )
   #define LIST_INTERCEPT_LIST_LINEAR_SEARCH( list_ptr, compare_func, compare_val )
   #define LIST_INTERCEPT_LIST_LINEAR_DELETE( list_ptr, compare_func, compare_val )

   #define LIST_INTERCEPT_ORDERED_LIST_INIT( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_DESTROY( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_PUSH_FRONT( list_ptr, item_link_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_POP_FRONT( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_PUSH_BACK( list_ptr, item_link_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_POP_BACK( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_SIZE( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_PEEK_FRONT( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_PEEK_BACK( list_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_PEEK_NEXT( list_ptr, item_after_which_to_peek )
   #define LIST_INTERCEPT_ORDERED_LIST_PEEK_PREV( list_ptr, item_before_which_to_peek )
   #define LIST_INTERCEPT_ORDERED_LIST_POP_ITEM( list_ptr, item_to_pop_ptr )
   #define LIST_INTERCEPT_ORDERED_LIST_LINEAR_SEARCH( list_ptr, compare_func, compare_val )
   #define LIST_INTERCEPT_ORDERED_LIST_LINEAR_DELETE( list_ptr, compare_func, compare_val )
   #define LIST_INTERCEPT_ORDERED_LIST_PUSH( list_ptr, item_link_ptr, weight )
/** @endcond */


#ifdef FEATURE_WINCE
	#error code not present
#endif

/* ===================================================================
MACRO
DESCRIPTION
====================================================================== */
/** @cond 
*/
#ifdef FEATURE_QDSP6
   #define LIST_MUTEX_LOCK(pkcs) qmutex_lock(pkcs)
#else /* FEATURE_QDSP6 */
   #ifdef FEATURE_WINCE
      #error code not present
#else
      #define LIST_MUTEX_LOCK(pkcs) rex_enter_crit_sect(&(pkcs))
   #endif /* FEATURE_WINCE */
#endif /* FEATURE_QDSP6 */
/** @endcond */
/* ===================================================================
MACRO
DESCRIPTION
====================================================================== */
/** @cond 
*/
#ifdef FEATURE_QDSP6
   #define LIST_MUTEX_FREE(pkcs) qmutex_unlock(pkcs)
#else /* FEATURE_QDSP6 */
   #ifdef FEATURE_WINCE
      #error code not present
#else /* FEATURE_WINCE */
      #define LIST_MUTEX_FREE(pkcs) rex_leave_crit_sect(&(pkcs))
#endif /* FEATURE_WINCE */
#endif /* FEATURE_QDSP6 */
/** @endcond */
/*===========================================================================
MACRO LIST_ASSERT

===========================================================================*/
/**
  Causes an assertion to occur when the given condition is FALSE 
  (action depends on the implementation).
 
  @param[in] xx_condition Condition to be tested.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  If the condition is FALSE, an ASSERT is raised.
*/
#ifdef LIST_DEBUG
   #ifdef T_WINNT
      #error code not present
#else
      extern void exit(int);
      #define LIST_ASSERT( xx_condition ) if (!(xx_condition)) exit(xx_condition)
   #endif
#else
   #define LIST_ASSERT( xx_condition )
#endif

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif
/** @} */ /* end_addtogroup utils_list */
/* ------------------------------------------------------------------------
** 
** Unordered Lists
**
** ------------------------------------------------------------------------ */
/** @addtogroup utils_list_unorder
@{ */

/**
  Initializes a list.

  @param[in] list_ptr Pointer to the list to initialize.

  @return
  None.

  @dependencies
  None.
*/
void list_init(
   list_type *list_ptr 
);

/** @addtogroup utils_list_unorder
@{ */

/**
  Determines if the list is initialized.

  @param[in] list_ptr List to verify.

  @return
  Zero -- List is not initialized. \n 
  One -- List is initialized. 

  @dependencies
  None.
*/
unsigned int
list_is_initialized(
   list_type *list_ptr
);

/**
  Destroys a list.

  @param[in] list_ptr Pointer to the list to be destroyed.

  @return
  None.

  @dependencies
  The list must be empty before destroying it.
*/
void list_destroy(
   list_type *list_ptr 
);


/**
  Inserts an item at the front of the list.

  @param[in] list_ptr      List in which the item is inserted.
  @param[in] item_link_ptr Item to be inserted.

  @return
  None.

  @dependencies
  None.
*/
void list_push_front(
   list_type      *list_ptr,
   list_link_type *item_link_ptr
);

/**
  Removes an item from the front of the list.

  @param[in] list_ptr List from which the item is removed.

  @return
  Pointer to the item.
  
  @dependencies
  None.
*/
void*
list_pop_front(
   list_type *list_ptr
);

/**
  Adds an item to the end of a list.

  @param[in] list_ptr      List to which the item is added.
  @param[in] item_link_ptr Item to be added to the end of the list.

  @return
  None.

  @dependencies
  None.
*/
void list_push_back(
   list_type      *list_ptr,
   list_link_type *item_link_ptr
);

/**
  Removes an item from the end of the list.

  @param[in] list_ptr List from which the item is removed.

  @return
  Pointer to the item.

  @dependencies
  None.
*/
void*
list_pop_back(
   list_type *list_ptr
);

/**
  Determines the size of the list.

  @param[in] list_ptr List to be sized.

  @return
  Size of the list.

  @dependencies
  None.
*/
list_size_type
list_size(
   list_type *list_ptr
);

/**
  Returns the front of the list, but does not modify the list.

  @param[in] list_ptr List at which to peek.

  @return
  Pointer to the head element of the list.

  @dependencies
  None.
*/
void*
list_peek_front(
   list_type *list_ptr
);

/**
  Returns the end of the list, but does not modify the list.

  @param[in] list_ptr List at which to peek. 

  @return
  Pointer to the tail element of the list.

  @dependencies
  None.
*/
void*
list_peek_back(
   list_type *list_ptr
);

/**
  Returns a pointer to the list element after the one passed in to the list.

  @param[in] list_ptr                 List to access.
  @param[in] item_after_which_to_peek Item before the one to be accessed.

  @return
  Pointer to that element of the list.

  @dependencies
  None.
*/
void*
list_peek_next(
   list_type      *list_ptr,
   list_link_type *item_after_which_to_peek
);

/**
  Returns a pointer to the list element before the one passed in to the list.

  @param[in] list_ptr                  List to access.
  @param[in] item_before_which_to_peek Item after the one to be accessed.

  @return
  Pointer to that element of the list.

  @dependencies
  None.
*/
void*
list_peek_prev(
   list_type      *list_ptr,
   list_link_type *item_before_which_to_peek
);

/**
  Pushes an item onto the list before the item passed in.

  @param[in] list_ptr                List to modify.
  @param[in] item_to_push_ptr        Item to be pushed onto the list.
  @param[in] item_to_push_before_ptr Item before which the new item appears.

  @return
  None.

  @dependencies
  None.
*/
void list_push_before(
   list_type *list_ptr,
   list_link_type *item_to_push_ptr,
   list_link_type *item_to_push_before_ptr
);

/**
  Pushes an item onto the list after the item passed in.

  @param[in] list_ptr               List to modify.
  @param[in] item_to_push_ptr       Item to be pushed onto the list.
  @param[in] item_to_push_after_ptr Item after which the new item appears.

  @return
  None.

  @dependencies
  None.
*/
void list_push_after(
   list_type *list_ptr,
   list_link_type *item_to_push_ptr,
   list_link_type *item_to_push_after_ptr
);                      

/**
  Removes a specific item from the list.

  @param[in] list_ptr        List to access.
  @param[in] item_to_pop_ptr Item to be removed from the list.

  @return
  None.

  @dependencies
  None.
*/
void list_pop_item(
   list_type      *list_ptr,
   list_link_type *item_to_pop_ptr
);

/**
  Performs a linear search on the list.

  @param[in] list_ptr     List to search.
  @param[in] compare_func Function used to perform the comparison; 
                          nonzero indicates a match.
  @param[in] compare_val  Value to be compared; this is the second argument
                          to compare_func.

  @return
  The first match that is found, or NULL if no match is found.

  @dependencies
  None.
*/
void*
list_linear_search(
  list_type             *list_ptr,  
  list_compare_func_type compare_func,
  void                  *compare_val
);

/**
  Merges two lists by appending the second to the first.

  @note1hang After the function is complete, the first list has all the 
  elements, and the second list is empty.

  @param[in] list1_ptr First list to be merged.
  @param[in] list2_ptr Second list to be merged.

  @return
  None.

  @dependencies
  None.
*/
void list_merge(
   list_type      *list1_ptr,
   list_type      *list2_ptr
);

/**
  Splits a list into two lists at a specific item. The item and everything
  past it is placed on the second list.

  @param[in] list1_ptr                  List to be split.
  @param[in] item_at_which_to_split_ptr Item at which the list is to be split.
  @param[out] list2_ptr                 Second list to be generated. 
 
  @return
  None.

  @dependencies
  list2_ptr must be initialized and empty.
*/
void list_split(
   list_type      *list1_ptr,
   list_link_type *item_at_which_to_split_ptr,
   list_type      *list2_ptr
);

/** @} */ /* end_addtogroup utils_list_unorder */

/* ------------------------------------------------------------------------
**
** Ordered Lists
** - There is one function supported by ordered lists but not by
**   lists : ordered_list_push.
**   Functions such as push_before/after and push_back are not supported
**   for ordered lists as these would intefere with the ordering of the list.
** ------------------------------------------------------------------------ */

/** @addtogroup utils_list_order
@{ */

/**
  Initializes an ordered list.

  @param[out] list_ptr    List to initialize.
  @param[in] sort_order   Order in which the list is sorted. 
  @param[in] compare_type What to use to compare the elements.

  @return
  None.

  @dependencies
  None.
*/
void ordered_list_init(
   ordered_list_type       *list_ptr,
   ordered_list_config_type sort_order,
   ordered_list_config_type compare_type
);

/** @addtogroup utils_list_order
@{ */

/**
  Determines if the ordered list is initialized.

  @param[in] list_ptr List to check.

  @return
  Zero -- List is not initialized. \n 
  One -- List is initialized. 

  @dependencies
  None.
*/
unsigned int ordered_list_is_initialized(
   ordered_list_type       *list_ptr
);
/**
  Destroys an ordered list.

  @param[in] list_ptr    List to be destroyed.

  @return
  None.

  @dependencies
  The list must be empty before destroying it.
*/
void ordered_list_destroy(
   ordered_list_type       *list_ptr
);


/**
  Pushes a new item onto the list.

  @param[in] list_ptr      List to access.
  @param[in] item_link_ptr Item to add to the list.
  @param[in] weight        Weight with which to add the new item.

  @return
  None.

  @dependencies
  None.
*/
void ordered_list_push(
   ordered_list_type        *list_ptr,
   ordered_list_link_type   *item_link_ptr,
   ordered_list_weight_type  weight
);

/**
  Removes an item from the front of the list.

  @param[in] list_ptr List from which the item is removed.

  @return
  Pointer to the item.

  @dependencies
  None.
*/
void*
ordered_list_pop_front(
   ordered_list_type *list_ptr
);

/**
  Removes an item from the back of the list.

  @param[in] list_ptr List from which the item is removed.

  @return
  Pointer to the item.

  @dependencies
  None.
*/
void*
ordered_list_pop_back(
   ordered_list_type *list_ptr
);

/**
  Determines the size of the list.

  @param[in] list_ptr List to access.

  @return
  Size of the list.

  @dependencies
  None.
*/
list_size_type
ordered_list_size(
   ordered_list_type *list_ptr
);

/**
  Returns the front of the list, but does not modify the list.

  @param[in] list_ptr List at which to peek. 

  @return
  Pointer to the head element of the list.

  @dependencies
  None.
*/
void*
ordered_list_peek_front(
   ordered_list_type *list_ptr
);

/**
  Returns the end of the list, but does not modify the list.

  @param[in] list_ptr List at which to peek. 

  @return
  Pointer to the tail element of the list.

  @dependencies
  None.
*/
void*
ordered_list_peek_back(
   ordered_list_type *list_ptr
);

/**
  Removes a specific item from the list.

  @param[in] list_ptr    List to access.
  @param[in] item_to_pop Item to be removed from the list.

  @return
  None.

  @dependencies
  None.
*/
void ordered_list_pop_item(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_to_pop
);

/**
  Returns a pointer to the list element after the one passed in to the list.

  @param[in] list_ptr                 List to access.
  @param[in] item_after_which_to_peek Item before the one you want to access.

  @return
  Pointer to that element of the list.

  @dependencies
  None.
*/
void*
ordered_list_peek_next(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_after_which_to_peek
);

/**
  Returns a pointer to the list element before the one passed in to the list.

  @param[in] list_ptr                  List to access.
  @param[in] item_before_which_to_peek Item after the one you want to access.

  @return
  Pointer to that element of the list.

  @dependencies
  None.
*/
void*
ordered_list_peek_prev(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_before_which_to_peek
);

/**
  Performs a linear search on the list.

  @param[in] list_ptr     List to search.
  @param[in] compare_func Function used to perform the comparison; 
                          nonzero indicates a match.
  @param[in] compare_val  Value to be compared; this is the second argument
                          to compare_func.

  @return
  The first match that is found, or NULL if no match is found.

  @dependencies
  None.
*/
void*
ordered_list_linear_search(
  ordered_list_type      *list_ptr,  
  list_compare_func_type  compare_func,
  void                   *compare_val
);

/**
  Merge two lists by appending the second to the first.

  @note1hang After the function is complete, the first list will have all the 
  elements, and the second list will be empty.

  @param[in] list1_ptr First list to be merged.
  @param[in] list2_ptr Second list to be merged.

  @return
  None.

  @dependencies
  None.
*/
void ordered_list_merge(
   ordered_list_type *list1_ptr,
   ordered_list_type *list2_ptr
);

/**
  Splits a list into two lists at a specific item. The item and everything
  past it is placed on the second list.

  @param[in] list1_ptr                  List to be split.
  @param[in] item_at_which_to_split_ptr Item at which the list is to be split.
  @param[out] list2_ptr                 Second list to be generated.

  @return
  None.

  @dependencies
  list2_ptr must be initialized and empty.
*/
void ordered_list_split(
   ordered_list_type      *list1_ptr,
   ordered_list_link_type *item_at_which_to_split_ptr,
   ordered_list_type      *list2_ptr
);

/** @} */ /* end_addtogroup utils_list_order */
#ifdef LIST_DEBUG

/** @addtogroup utils_list_unorder
@{ */

/**
  Determines if the list is valid.

  @param[in] list_ptr List to verify.

  @return
  Zero -- List is not valid. \n
  One -- List is valid.

  @dependencies
  None.
*/
unsigned long
list_is_valid(
   list_type *list_ptr
);

/**
  Determines if a specific item is in the list.

  @param[in] list_ptr List to query.
  @param[in] item_ptr Item to look for in the list.

  @return
  Zero -- Item is not in the list. \n
  One -- Item is in the list.

  @dependencies
  None.
*/
unsigned long
item_is_in_list(
   list_type      *list_ptr,
   list_link_type *item_ptr
);

/** @} */ /* end_addtogroup utils_list_unorder */

/** @addtogroup utils_list_order
@{ */

/**
  Determines if the list is valid.

  @param[in] list_ptr List to verify.

  @return
  Zero -- List is not valid. \n
  One -- List is valid.

  @dependencies
  None.
*/
unsigned long
ordered_list_is_valid(
   ordered_list_type *list_ptr
);

/**
  Determines if a specific item is in the list.

  @param[in] list_ptr List to query.
  @param[in] item_ptr Item to look for in the list.

  @return
  Zero -- Item is not in the list. \n
  One -- Item is in the list.

  @dependencies
  None.
*/
unsigned long
item_is_in_ordered_list(
   ordered_list_type      *list_ptr,
   ordered_list_link_type *item_ptr
);

/** @} */ /* end_addtogroup utils_list_order */

#endif /* LIST_DEBUG */

#ifdef __cplusplus
   }
#endif

#endif /* LIST_H */
