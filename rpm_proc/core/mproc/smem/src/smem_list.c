/*===========================================================================

                         S H A R E D   M E M O R Y

DESCRIPTION
    Defines the public api that is used to create a generic list.

Copyright (c) 2004 - 2013 by Qualcomm Technologies, Incorporated.  All 
Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem_list.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/13   pa      Initial revision based on implementation by padelsba
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_os.h"
#include "smem_list.h"

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS
                      
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
void smem_list_init( smem_list_type *list )
{
  list->next  = NULL;
  list->count = 0;
}

/*===========================================================================
FUNCTION      smem_list_append
===========================================================================*/
/**
  Appends an element to a generic list.
 
  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.
   
  @return 
  N/A.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_append( smem_list_type *list, void *item )
{
  smem_list_link_type *link = ( smem_list_link_type * )list;

  /* get next item until link->next points to null */
  while( link->next )
  {
    link = link->next;
  }

  /* link->next points to null here, but we can point it to the new item */
  link->next = item;

  /* point link to the head of the new item */
  link = item;

  /* terminate the list */
  link->next = NULL;

  list->count++;
}

/*===========================================================================
FUNCTION      smem_list_delete
===========================================================================*/
/**
  Removes the first element in the list.
 
  @param[in] list       Pointer to the user list.
  @param[out] item      Pointer to store the removed element.
   
  @return 
  0 if the element is deleted, or a negative error code if the element
  is not found.
 
  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smem_list_delete( smem_list_type *list, void *item )
{
  smem_list_link_type *link = ( smem_list_link_type * )list;

  /* get next item until link points to item in question */
  while( link->next != item )
  {
    if( NULL == link->next )
    {
      return -1;
    }

    link = link->next;
  }

  link->next = ( ( smem_list_link_type * )item )->next;

  list->count--;

  return 0;
}

/*===========================================================================
FUNCTION      smem_list_first
===========================================================================*/
/**
  Retrieves the first element in the list without removing.
 
  @param[in] list       Pointer to the user list.
   
  @return 
  Pointer to the first element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_first( smem_list_type *list )
{
  /* return the first item in the list */
  return list->next;
}

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
void *smem_list_next( void * item )
{
  /* just return the item that this item points to */
  return ( ( smem_list_link_type * )item )->next;
}

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
int32 smem_list_count( smem_list_type *list )
{
  return list->count;
}

