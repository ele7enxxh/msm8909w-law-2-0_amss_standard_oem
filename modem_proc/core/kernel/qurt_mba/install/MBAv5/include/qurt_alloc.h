#ifndef QURT_ALLOC_H
#define QURT_ALLOC_H

/*======================================================================
                        qurt_alloc.h 

GENERAL DESCRIPTION
  Prototypes of Kernel memory allocation  API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_alloc.h#1 $ 
 *  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2011-02-25   op      Add Header file
 ======================================================================*/

/*======================================================================*/
/**
  FUNCTION        qurt_malloc

  DESCRIPTION     Wrap the C library, but with a lock.
                  allocates size bytes and returns a pointer to the allocated memory. 

  
	@param size [IN] Bytes to allocate 

  DEPENDENCIES    None

  @return void * [OUT] 
                   returns a void pointer to the allocated space or NULL if there is insufficient 
                   memory available
    
 
  SIDE EFFECTS    Users are recommended to deprecate using these functions, switching to plain
                   malloc/free call from C library since they are going to be thread safe too.
 */
/* ======================================================================*/
void *qurt_malloc( unsigned int size);

/*======================================================================*/
/**
  FUNCTION        qurt_calloc

  DESCRIPTION     allocates space for an array of nelem elements of size elsize. The space is 
                  initialized to zero. wrap the C library, but with a lock

	@param elsize [IN] the size of each element
	@param num    [IN] number of elements

  DEPENDENCIES   None    

  @return void * [OUT] 
                  If successful, it returns a pointer to space suitably aligned for storage of any type
                  of object. If there is no available memory, it returns a null pointer
  
  SIDE EFFECTS   Users are recommended to deprecate using these functions, switching to plain
                  malloc/free call from C library since they are going to be thread safe too.
 
 */
 /* ======================================================================*/
void *qurt_calloc(unsigned int elsize, unsigned int num);

/*======================================================================*/
/**
  FUNCTION        qurt_realloc

  DESCRIPTION     Reallocate memory block. Wrap the C library, but with a lock
									The size of the memory block pointed to by the ptr parameter is changed to the 
									size bytes, expanding or reducing the amount of memory available in the block.
									The function may move the memory block to a new location, in which case the new
									location is returned. The content of the memory block is preserved up to the lesser
									of the new and old sizes, even if the block is moved. If the new size is larger, the 
									value of the newly allocated portion is indeterminate.
	                In case that ptr is NULL, the function behaves exactly as malloc, assigning a new
	                block of size bytes and returning a pointer to the beginning of it.
	                In case that the size is 0, the memory previously allocated in ptr is deallocated as
	                if a call to free was made, and a NULL pointer is returned.

	@param *ptr [IN] Pointer to a memory block previously allocated with malloc, calloc or realloc to be
	                reallocated. If this is NULL, a new block is allocated and a pointer to it is returned
	                by the function.
	@param newsize [IN] size for the memory block, in bytes.
	                If it is 0 and ptr points to an existing block of memory, the memory block pointed by
	                ptr is deallocated and a NULL pointer is returned. 
	
  DEPENDENCIES   None

  @return void * [OUT] 
                  A pointer to the reallocated memory block, which may be either the same as the
                  ptr argument or a new location.
	                The type of this pointer is void*, which can be cast to the desired type of data 
	                pointer in order to be dereferenceable.
	                If the function failed to allocate the requested block of memory, a NULL pointer is
	                returned, and the memory block pointed to by argument ptr is left unchanged.
	 
  SIDE EFFECTS    Users are recommended to deprecate using these functions, switching to plain
                  * malloc/free call from C library since they are going to be thread safe too.
 
 */
 /* ======================================================================*/
void *qurt_realloc(void *ptr,  int newsize);

/*======================================================================*/
/**
  FUNCTION        qurt_free

  DESCRIPTION     deallocates the space pointed to by ptr, freeing it up for future use. 
                  wrap the C library, but with a lock

	@param *ptr [IN] deallocated pointer
	
  DEPENDENCIES   ptr must have been used in a previous call to qurt_malloc(), qurt_calloc(), 
                  or qurt_realloc()

  @return         None
  
  SIDE EFFECTS    Users are recommended to deprecate using these functions, switching to plain
                  malloc/free call from C library since they are going to be thread safe too.
 */
 /* ======================================================================*/
void qurt_free( void *ptr);

#endif /* QURT_ALLOC_H */

