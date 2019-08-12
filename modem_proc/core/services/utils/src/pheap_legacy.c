/* =========================================================================

DESCRIPTION
  Implementation of routines for allocatio/de-allocation of memory buffers mapped to
  physical address space.

Copyright (c) 2005      by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/pheap_legacy.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/02/05  kk     Created original file
============================================================================ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "pheap.h"
#include "comdef.h"
#include "stdlib.h"

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

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
   return malloc(size);
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
   return realloc(ptr,size);
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
   free(ptr);
   return;
} /* END pheap_free */
