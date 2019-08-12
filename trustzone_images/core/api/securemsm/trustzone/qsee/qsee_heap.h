#ifndef QSEEHEAP_H
#define QSEEHEAP_H

/**
@file qsee_heap.h
@brief Provide heap management API wrappers
*/


/*===========================================================================

DESCRIPTION
  Wrapper for secure malloc and free calls for secure heap.

Copyright (c) 2011  by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                                 Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_heap.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ 
$Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     --------------------------------------------------------
 
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "comdef.h"

#if !defined(size_t) 
typedef unsigned int size_t ;
#endif

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif
/**
 * @brief   Allocates a block of size bytes from the heap.  If heap_ptr is NULL or size is 0, the NULL pointer will be silently returned.
 *
 * @param[in] size Number of bytes to allocate from the heap. 
 *
 * @return   Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.
 *
 */
void* qsee_malloc(size_t size);

/**
 * @brief  Deallocates the ptr block of memory. 
 *
 * @param[in] ptr pointer to block which will be freed 
 *
 */
void qsee_free(void *ptr);

#ifdef __cplusplus
   }
#endif

#endif /* QSEEHEAP_H */

