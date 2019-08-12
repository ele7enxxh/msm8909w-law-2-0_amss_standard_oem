/*==========================================================================

                         AMSS Heap Debug Module

DESCRIPTION
  This file contains the AMSS heap debug functions and associated data.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The memdebug_init() function must be called before any other function
  in this module.

Copyright (c) 2006-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/amssheap_debug.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 11/12/14  ps     Remove Debug Array
 02/05/13  ps     Add debug information to the block header
 09/05/12  mg     Modem heap using memheap.
 08/29/06  jc     Initial release.

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "core_variation.h"
#include "memheap.h"
#include "rex.h"
#include "amssassert.h"
#ifdef FEATURE_MEM_DEBUG

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

/* Code to enter and exit critical sections */
#ifdef FEATURE_MEMHEAP_MT
#define BEGIN_CRITICAL_SECTION(heap) (heap)->lock_fnc_ptr(heap)
#define END_CRITICAL_SECTION(heap)   (heap)->free_fnc_ptr(heap)
#else
#define BEGIN_CRITICAL_SECTION(heap)
#define END_CRITICAL_SECTION(heap)
#endif




extern mem_block_header_type *mem_get_next_block
(
   const mem_heap_type         *heap_ptr,
     /*  The heap the given block belongs to -- may NOT be NULL!
     */
   const mem_block_header_type *block_ptr,
     /*  The block in the heap for which to return the following heap
         block
     */
   const unsigned int          check_next_section
);

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/


/*============================================================================
                          GLOBAL VARIABLES
============================================================================*/

/* All local module data */


/*============================================================================
                          LOCAL VARIABLES
============================================================================*/

/*============================================================================
                          MACRO DEFINITIONS
============================================================================*/

/*============================================================================
                          FORWARD DECLARATIONS
============================================================================*/

/*===========================================================================

FUNCTION memdebug_malloc

DESCRIPTION
  This function logs a memory block allocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_malloc
(
  mem_heap_type *heap_ptr,
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr,         /* Pointer to allocated memory block */
  uint32 stats_index /*Index to statistics array which can be used to get tcb information*/
)
{
  mem_block_header_type *theBlock;

  if (ptr == NULL)
  {
    return;
  }

  theBlock = (mem_block_header_type *)
                     ((char *) ptr - sizeof(mem_block_header_type));

/* Store debug information in the block header*/
	 theBlock->header_debug_info.caller_ptr = caller_ptr;
	 theBlock->header_debug_info.stats_index =  stats_index;
 
}

/*===========================================================================

FUNCTION memdebug_malloc_updateparams

DESCRIPTION
  This function updates the information about a memory block allocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_malloc_updateparams
(
  mem_heap_type *heap_ptr,
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr,         /* Pointer to allocated memory block */
  uint32 stats_index
)
{
  mem_block_header_type *theBlock;

  if (ptr == NULL)
  {
    return;
  }

  theBlock = (mem_block_header_type *)
                     ((char *) ptr - sizeof(mem_block_header_type));

  theBlock->header_debug_info.caller_ptr = caller_ptr;
  theBlock->header_debug_info.stats_index=stats_index;
 
}



void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr,size_t size)
{
  mem_block_header_type *theBlock;

  if (ptr == NULL)
  {
    return;
  }

  theBlock = (mem_block_header_type *)
                     ((char *) ptr - sizeof(mem_block_header_type));

  theBlock->header_debug_info.caller_ptr = caller_ptr;

}
/*===========================================================================

FUNCTION memdebug_free

DESCRIPTION
  This function logs a memory block free.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_free
(
  mem_heap_type *heap_ptr,
  void *ptr,         /* Pointer to allocated memory block */
  uint32 db_index
)
{
}

#endif  /* FEATURE_MEM_DEBUG */
