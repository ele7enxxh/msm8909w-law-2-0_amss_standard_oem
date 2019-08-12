/* =========================================================================

DESCRIPTION
  Implementation of routines for allocatio/de-allocation of memory buffers mapped to
  physical address space.

Copyright (c) 2005      by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/pheap.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/02/05  kk     Created original file
============================================================================ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "pheap.h"

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
mem_heap_type pmem_heap;
char pmem_buf[PHEAP_SIZE];

static uint32 pheap_initialized=FALSE;

/*===========================================================================
FUNCTION PMEM_INIT_HEAP

DESCRIPTION
Initializes the heap for allocating physical address mapped memory buffers.
===========================================================================*/
void pheap_init(void)
{
   mem_init_heap(&pmem_heap, pmem_buf, PHEAP_SIZE, NULL);
   pheap_initialized = TRUE;
} /* END pheap_init */

/*===========================================================================
FUNCTION PHEAP_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
void* pheap_malloc(
      size_t         size
         /* Number of bytes to allocate
         */
)
{
   if (!pheap_initialized)
      pheap_init();

   return mem_malloc(&pmem_heap, size);
} /* END pheap_malloc */

/*===========================================================================
FUNCTION PHEAP_REALLOC

DESCRIPTION
  Re-allocates a block of size bytes from the heap.

===========================================================================*/
void* pheap_realloc(
      void * ptr,
      size_t         size
         /* Number of bytes to allocate
         */
)
{
   if (!pheap_initialized)
      pheap_init();

   return mem_realloc(&pmem_heap, ptr, size);
} /* END pmem_malloc */


/*===========================================================================
FUNCTION PMEM_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
void pheap_free(
   void          *ptr
      /* Memory to free
      */
)
{
   mem_free(&pmem_heap, ptr);
} /* END pheap_free */
