/* =========================================================================

DESCRIPTION
  Implementation of memory client for mba uncached memory.

Copyright (c) 2012-14      by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/mba_uncached_heap.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/07/14   rks     CR491964:Add dynamic sections support in memheap
09/19/12  mg     Created original file
============================================================================ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap.h"

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
mem_heap_type umem_heap;


#ifndef MEMHEAP_CRIT_SEC_SIZE 
#define MEMHEAP_CRIT_SEC_SIZE 64
#endif

static uint32 uheap_initialized=FALSE;

/*===========================================================================
FUNCTION UHEAP INIT

DESCRIPTION
Initializes the heap for uncached memory
===========================================================================*/
void uheap_init(void * start, uint32 size)
{
   
   mem_init_heap((mem_heap_type*)&umem_heap, start, size, NULL);
    

   uheap_initialized = TRUE;
} /* END pheap_init */

/*===========================================================================
FUNCTION UHEAP_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
void* uheap_malloc(
      size_t         size
         /* Number of bytes to allocate
         */
)
{
   if (!uheap_initialized)
      return NULL;

   return mem_malloc(&umem_heap, size);
} /* END uheap_malloc */

/*===========================================================================
FUNCTION uHEAP_REALLOC

DESCRIPTION
  Re-allocates a block of size bytes from the heap.

===========================================================================*/
void* uheap_realloc(
      void * ptr,
      size_t         size
         /* Number of bytes to allocate
         */
)
{
   if (!uheap_initialized)
      return NULL;

   return mem_realloc(&umem_heap, ptr, size);
} /* END pmem_malloc */


/*===========================================================================
FUNCTION UHEAP_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
void uheap_free(
   void          *ptr
      /* Memory to free
      */
)
{
   mem_free(&umem_heap, ptr);
} /* END pheap_free */
