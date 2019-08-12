#ifndef DYNAMIC_HEAP_H
#define DYNAMIC_HEAP_H
/**
  @file dynamic_heap.h
  @brief A heap manager for dynamically creating heap memory by using memory 
   allocated on the HLOS and mapping in the modem memory space.

*/

/*
Copyright (c) 1997-2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/*===========================================================================

                             Edit History

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/07/13   ps     File created
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "comdef.h"
#include <stddef.h>
#include "memheap.h"

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
typedef enum dynamic_mem_error {
   DYNAMIC_MEM_SUCCESS = 0,
   DYNAMIC_MEM_FAIL = 1,
   DYNAMIC_MEM_INIT_ALREADY = 2,
   DYNAMIC_MEM_FAIL_INVALID_CLIENT_ID =3,
   DYNAMIC_MEM_FAIL_INVALID_ATTRIBUTE = 4,
   DYNAMIC_MEM_DEINIT_FAIL_HEAP_BLOCK_USED =5,
   DYNAMIC_MEM_DEINIT_FAIL_INVALID_ARG = 6,
   DYNAMIC_MEM_DEINIT_FAIL_KERNEL_ERROR =7,
   DYNAMIC_MEM_FAIL_INVALID_SIZE=8,

}dynamic_mem_error_type;


typedef enum dynamic_mem_client_id_type {
   DYNAMIC_MEM_CLIENT_ID_MIN = -1,
   DYNAMIC_MEM_CLIENT_ID_GPS = 0,
   DYNAMIC_MEM_CLIENT_ID_DIAG = 1,
   DYNAMIC_MEM_CLIENT_ID_FTM = 2,
   DYNAMIC_MEM_CLIENT_ID_MAX = 3,
      
}dynamic_mem_client_id;

typedef enum dynamic_mem_attribute_type {
   DYNAMIC_MEM_ATTRIBUTE_MIN = -1, 
   DYNAMIC_MEM_ATTRIBUTE_CMA = 0, 
   DYNAMIC_MEM_ATTRIBUTE_NON_CMA = 1, 
   DYNAMIC_MEM_ATTRIBUTE_MAX = 2, 
}dynamic_mem_attribute;
/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif

/*!
    @brief
    Initializes the heap using the extra memory buffer.

    @detail
    Initializes a heap using the extra memory buffer.

    @return
    DYNAMIC_MEM_SUCCESS if successful
    DYNAMIC_MEM_FAIL if initialization fails for any reason.
    DYNAMIC_MEM_INIT_ALREADY if the heap is already initialized.

*/
/*=========================================================================*/

unsigned int dynamic_heap_init(void);


/*===========================================================================

  FUNCTION:  dynamic_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the dynamic heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* dynamic_alloc(size_t size);

/*===========================================================================

  FUNCTION:  dynamic_calloc

===========================================================================*/
/*!
    @brief
    Allocates enough space for elt_count elements each of elt_size bytes
    from the heap and initializes the space to zero.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated. If elt_count or elt_size is 0, the NULL pointer 
    will be silently returned.
*/
/*=========================================================================*/

void* dynamic_calloc(size_t elt_count, size_t elt_size);

/*===========================================================================

  FUNCTION:  dynamic_realloc

===========================================================================*/
/*!
    @brief
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.

    @detail
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.  If the block is shortened, bytes are discarded off the
    end.  If the block is lengthened, the new bytes added are not initialized
    and will have garbage values.  
    If ptr is NULL, the function behaves exactly like dynamic_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like dynamic_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call dynamic_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *dynamic_realloc(void *ptr, size_t size);

/*===========================================================================

  FUNCTION:  dynamic_free

===========================================================================*/
/*!
    @brief
    Deallocates the ptr block of memory. 

    @detail
    Deallocates the ptr block of memory.  If ptr is NULL, or ptr is outside 
    the range of memory managed by heap, then this function call does nothing
    (and is guaranteed to be harmless).

    @return
    None.
*/
/*=========================================================================*/
void dynamic_free(void *ptr);



/*===========================================================================

  FUNCTION:  dynamic_heap_deinit

===========================================================================*/
/*!
    @brief
    Deinitializes the heap and releases the memory back to the HLOS.

    @detail
    This function resets the heap structure. The client is responsible for freeing all the memory before calling
    this function. Failure to free all the memory will cause an assert. 

    @return
    None
*/
/*=========================================================================*/
void dynamic_heap_deinit(void);


/*!
    @brief
    Initializes the heap using the extra memory buffer.

    @detail
    Initializes a heap using the extra memory buffer.

	@param[in] client_id            client requesting for memory, value should be from the enum dynamic_mem_client_id 
	@param[in] attr                 client requesting for contiguous or non contiguous memory. value from enum dynamic_mem_attribute
	@param[in] size                 size of the memory requested
	@param[in] heap_ptr            Ptr to heap structure, if the memory is suppose to run as the heap, NULL otherwise. Heap will be initialized if Non NULL
   @param[inout] alloc_ptr              pointer which will the base pointer to start of allocated buffer
    @return
    DYNAMIC_MEM_SUCCESS if successful
    DYNAMIC_MEM_FAIL if initialization fails for any reason.
    DYNAMIC_MEM_INIT_ALREADY if the heap is already initialized.

*/
/*=========================================================================*/

unsigned int dynamic_heap_g_init(dynamic_mem_client_id client_id, dynamic_mem_attribute attr, size_t size, mem_heap_type *heap_ptr, void **alloc_ptr);


/*===========================================================================

  FUNCTION:  dynamic_g_querysize

===========================================================================*/
/*!
    @brief
    Queries the size which can be allocated for this particular client id
   @param[in] client_id            client querying for memorysize, value should be from the enum dynamic_mem_client_id 
	@param[inout] size              pointer which will have the value for the query if no error
    @return
    DYNAMIC_MEM_SUCCESS if successful
    DYNAMIC_MEM_FAIL_INVALID_CLIENT_ID if client is wrong
    
*/
/*=========================================================================*/
unsigned int dynamic_heap_g_querysize( dynamic_mem_client_id client_id,uint32_t* size);


/*===========================================================================

  FUNCTION:  dynamic_g_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the dynamic heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* dynamic_g_alloc( mem_heap_type *heap_ptr,size_t size);



/*===========================================================================

  FUNCTION:  dynamic_g_calloc

===========================================================================*/
/*!
    @brief
    Allocates enough space for elt_count elements each of elt_size bytes
    from the heap and initializes the space to zero.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated. If elt_count or elt_size is 0, the NULL pointer 
    will be silently returned.
*/
/*=========================================================================*/

void* dynamic_g_calloc(mem_heap_type *heap_ptr,size_t elt_count, size_t elt_size);



/*===========================================================================

  FUNCTION:  dynamic_g_realloc

===========================================================================*/
/*!
    @brief
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.

    @detail
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.  If the block is shortened, bytes are discarded off the
    end.  If the block is lengthened, the new bytes added are not initialized
    and will have garbage values.  
    If ptr is NULL, the function behaves exactly like dynamic_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like dynamic_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call dynamic_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *dynamic_g_realloc(mem_heap_type *heap_ptr,void *ptr, size_t size);




/*===========================================================================

  FUNCTION:  dynamic_g_free

===========================================================================*/
/*!
    @brief
    Deallocates the ptr block of memory. 

    @detail
    Deallocates the ptr block of memory.  If ptr is NULL, or ptr is outside 
    the range of memory managed by heap, then this function call does nothing
    (and is guaranteed to be harmless).

    @return
    None.
*/
/*=========================================================================*/
void dynamic_g_free(mem_heap_type *heap_ptr,void *ptr);




/*===========================================================================

  FUNCTION:  dynamic_heap_g_deinit

===========================================================================*/
/*!
    @brief
    Deinitializes the heap and releases the memory back to the HLOS.

    @detail
    This function resets the heap structure. The client is responsible for freeing all the memory before calling
    this function. Failure to free all the memory will cause an assert. 

    @return
    None
*/
/*=========================================================================*/
unsigned int dynamic_heap_g_deinit(dynamic_mem_client_id client_id,mem_heap_type *heap_ptr);

#ifdef __cplusplus
   }
#endif


#endif /* DYNAMIC_HEAP_H */
