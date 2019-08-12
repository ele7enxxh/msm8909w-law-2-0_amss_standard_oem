#ifndef PHEAP_H
#define PHEAP_H

/**
  @file pheap.h
  @brief Basic memory allocation functions to return contiguous physical memory 
  buffer.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_memory" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*=========================================================================== 
Use the memory allocation/de-allocation functions provided in this file to 
 allocate and free memory buffers which can be directly accessed by DSP 
 services.

These memory functions are specifically useful in apiOne configuration when AMSS
 is running in virtual address space. These routines directly accesses the 
 kernel memory services to provide the memory buffers which are mapped to 
 physical address space.

===========================================================================*/
/*
Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/

/*===========================================================================

                                 Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/pheap.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/11/10   EBR     Doxygenated file.
02/02/05   kk      Original file created.
03/04/05   skc     Increased USB heap size
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap.h"

/** @addtogroup utils_memory
@{ */
/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */

/** Serves as a heap for the physical address mapped memory 
  buffers. */
extern mem_heap_type pmem_heap;

/** @cond
*/
#define PHEAP_SIZE   0x300000
/** @endcond */

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */
#ifdef __cplusplus
   extern "C"
   {
#endif

/**
  Allocates a block from the heap. If heap_ptr is NULL or size is 0, 
  the NULL pointer is silently returned.

  @param[in] size Number of bytes to be allocated.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block 
  could not be allocated.

  @dependencies
  None.
*/
void* pheap_malloc(
   size_t         size
);

/**
  Reallocates a block from the heap.

  @param[in] ptr  Pointer to the memory to be reallocated.
  @param[in] size Number of bytes to allocate.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block 
  could not be allocated.

  @dependencies
  None.
*/
void* pheap_realloc(
   void * ptr,
   size_t         size
);

/**
  Frees a block of memory.

  @param[in] ptr Pointer to the memory to be freed.

  @return
  None.

  @dependencies
  None.
*/
void pheap_free(
   void          *ptr
);

#ifdef __cplusplus
   }
#endif

/** @} */ /* end_addtogroup utils_memory */
#endif /* PHEAP_H */
