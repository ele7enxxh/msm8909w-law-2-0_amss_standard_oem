#ifndef SMEM_LIST_H
#define SMEM_LIST_H

/**
 * @file smem_list.h
 *
 * Public data types and functions to implement a generic 
 * list. 
 */ 

/*==============================================================================
     Copyright (c) 2013-2015 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/mproc/smem_list.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/15   an      Added smem_list_insert.
01/16/14   bt      Correct and clarify function header documentation.
03/28/13   pa      Initial revision based on implementation by padelsba
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/** @addtogroup smem
@{ */


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** Defines the structure to link to the SMEM list */
typedef struct smem_list_link
{
  struct smem_list_link *next;
} smem_list_link_type;


/** Defines the generic SMEM list type */
typedef struct smem_list
{
  smem_list_link_type *next;
  smem_list_link_type *last;
  int32 count;
} smem_list_type;

/** Compares given argumenbt to a list item. 
    It should return TRUE if condition is met. */
typedef boolean (*smem_list_comparator_fn)(void* item, void* list_item);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smem_list_init
===========================================================================*/
/**
  Creates a generic list.

  @param[in/out] list   Pointer to the user list.

  @return
  N/A.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_init( smem_list_type *list );

/*===========================================================================
FUNCTION      smem_list_append
===========================================================================*/
/**
  Appends an element to the end of a generic list.
  
  The element must contain a smem_list_link_type at its base address.
  The element should not be already present in any list.

  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.

  @return
  N/A.

  @dependencies
  The element must contain a smem_list_link_type at its base address.
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_append( smem_list_type *list, void *item );

/*===========================================================================
  FUNCTION      smem_list_insert
===========================================================================*/
/**
  Inserts an element into a generic list when comparator condition is met.
  
  The element must contain a smem_list_link_type at its base address.
  The element should not be already present in any list.
  The item is inserted ahead of the list element for which comparator returns 
  TRUE

  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.
  @param[in] comparator Caller provided comparator.

  @return
  N/A.

  @dependencies
  The element must contain a smem_list_link_type at its base address.
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_insert( smem_list_type *list, void *item, smem_list_comparator_fn comp);


/*===========================================================================
FUNCTION      smem_list_delete
===========================================================================*/
/**
  Removes the specified element from a generic list.

  The element may be present at any point in the list.

  @param[in] list       Pointer to the user list.
  @param[in] item       Element to be removed.

  @return
  0 if the element is deleted, or a negative error code if the element
  is not found.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smem_list_delete( smem_list_type *list, void *item );

/*===========================================================================
FUNCTION      smem_list_first
===========================================================================*/
/**
  Retrieves the first element in the list without removing it.

  @param[in] list       Pointer to the user list.

  @return
  Pointer to the first element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_first( smem_list_type *list );

/*===========================================================================
FUNCTION      smem_list_next
===========================================================================*/
/**
  Retrieves the next element in the list.

  @param[in] list       Pointer to the user list.

  @return
  Pointer to the next element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_next( void * item );

/*===========================================================================
FUNCTION      smem_list_count
===========================================================================*/
/**
  Gets the number of elements in the list.

  @param[in] list       Pointer to the user list.

  @return
  Count of elements in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smem_list_count( smem_list_type *list );

#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup smem */

#endif /* SMEM_LIST_H */
