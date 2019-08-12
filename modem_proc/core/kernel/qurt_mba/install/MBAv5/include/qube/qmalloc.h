#ifndef QMALLOC_H
#define QMALLOC_H
/*=============================================================================

                 qmalloc.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   qmalloc API
   Functions to take a block memory and convert it into a heap for dynamic
   memory allocation 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qmalloc.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <stddef.h>
#include <stdint.h>
	
#ifdef __cplusplus
	extern "C" {
#endif
	
/*=============================================================================
                      FUNCTION DECLARATIONS                                
=============================================================================*/

/**
* Initialize the heap
*
* The qmalloc_init is used to initialize a heap. After heap is initialized,
* memory can be allocated and freed using qmalloc and qfree functions.
*
* @param heap_id  [OUT] Heap id
* @param start          Starting address of the heap
* @param size           Size of the heap
*/
static inline void qmalloc_init(uint32_t *heap_id, void *start, size_t size) { return; }

/**
* Allocate memory in the heap
*
* @param heap_id  Heap id
* @param nbytes   Number of bytes to be allocated
* @return  Pointer to the buffer of nbytes size
*/
static inline void * qmalloc(uint32_t heap_id, size_t nbytes) { return qurt_malloc(nbytes); }

/**
* Reallocate memory in the heap
*
* @param heap_id  Heap id
* @param ptr      Old Pointer
* @param nbytes   Number of bytes to be allocated
* @return  Pointer to the buffer of nbytes size
*/
static inline void * qrealloc(uint32_t heap_id, void *ptr, size_t nbytes) { return qurt_realloc(ptr,nbytes);} 

/**
* Free allocated memory
*
* @param buf  Pointer to the buffer to be freed
*/
static inline void qfree(void *buf) { qurt_free(buf); }

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif  /* QMALLOC_H */
