#ifndef PHEAP_H
#define PHEAP_H
/*===========================================================================

DESCRIPTION
  Basic memory allocation functions to return contiguous physical memory buffer.
  Use the memory allocation/de-allocation functions provided in this file to allocate
  and free memory buffers which can be directly accessed by DSP services.

  These memory functions are specifically useful in apiOne configuration when AMSS
  is running in virtual address space. These routines directly accesses the kernel memory
  services to provide the memory buffers which are mapped to physical address space.
  
Copyright (c) 2005      by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                                 Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/pheap.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/2/05   kk     Original file created.
03/04/05  skc    Increased USB heap size
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap.h"

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
extern mem_heap_type pmem_heap;

#define PHEAP_SIZE   0x300000

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */
#ifdef __cplusplus
   extern "C"
   {
#endif

void* pheap_malloc(
   size_t         size                    // Number of bytes to allocate
);

void* pheap_realloc(
   void * ptr,
   size_t         size                    // Number of bytes to allocate
);

void pheap_free(
   void          *ptr   // Memory to free
);

#ifdef __cplusplus
   }
#endif

#endif /* PMEMHEAP_H */
