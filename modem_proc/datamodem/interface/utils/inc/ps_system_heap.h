#ifndef PS_SYSTEM_HEAP_H
#define PS_SYSTEM_HEAP_H
/*===========================================================================

                       P S _ S Y S T E M _ H E A P . H

DESCRIPTION
  The Data Services Protocol Stack system heap wrapper.

EXTERNALIZED FUNCTIONS

  ps_system_heap_mem_alloc()
    Allocates requested amount of memory from the system heap.

  PS_SYSTEM_HEAP_MEM_FREE()
    Frees the memory allocated through ps_system_heap_mem_alloc().

INTIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_system_heap.h#1 $
  $DateTime: 2007/05/18 18:56:57

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/24/10    ua     Fixed compilation errors. 
02/26/09    sp     IPsec CMI Decoupling - Fixed linking errors.
02/09/09    hm     Created initial module.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "modem_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
MACRO PS_SYSTEM_HEAP_MEM_ALLOC / PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT

DESCRIPTION
  This macro will internally call modem_mem_alloc/ps_system_heap_alloc based
  on featurization. It additionally validates the buffer allocation, prints debug 
  info and conditionally ASSERTs.
  
PARAMETERS
  buf_ptr  - pointer to store the address of the memory allocated
  size - Size (in bytes) of the memory to be allocated.
  typecast_type - data type to which the pointer needs to be typecasted

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.  
===========================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#else
#define PS_SYSTEM_HEAP_MEM_ALLOC(buf_ptr,size,typecast_type) \
  buf_ptr = (typecast_type)modem_mem_alloc(size, MODEM_MEM_CLIENT_DATACOMMON); \
  ps_system_heap_validate_mem_alloc(buf_ptr, size, TRUE)


#define PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(buf_ptr,size,typecast_type) \
  buf_ptr = (typecast_type)modem_mem_alloc(size, MODEM_MEM_CLIENT_DATACOMMON); \
  ps_system_heap_validate_mem_alloc(buf_ptr, size, FALSE)

#endif


/*===========================================================================
FUNCTION  PS_SYSTEM_HEAP_VALIDATE_MEM_ALLOC

DESCRIPTION
   This function is a wrapper on top of the system's malloc function and 
   prints caller address for tracking

PARAMETERS
  buf_ptr  - pointer to the memory allocated
  size - Size (in bytes) of the memory to be allocated.
  is_graceful - boolean specifies to handle gracefully or not
  
RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_system_heap_validate_mem_alloc
(
   void * buf_ptr,
   unsigned long size,
  boolean     is_graceful
);

/*===========================================================================
FUNCTION  ps_system_heap_mem_alloc

DESCRIPTION
  This function is a wrapper on top of the system's malloc function.

PARAMETERS
  num_bytes - Size (in bytes) of the memory to be allocated.

RETURN VALUE
  Pointer to memory block if successful.
  NULL if memory could not be allocated.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void* ps_system_heap_mem_alloc
(
  unsigned long num_bytes
);

/*===========================================================================
MACRO PS_SYSTEM_HEAP_MEM_FREE

DESCRIPTION
  This macro frees the memory allocated by ps_system_heap_mem_alloc(). It 
  also sets the pointer variable to NULL. 

PARAMTERS
  mem_ptr - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ps_system_heap_mem_alloc().

SIDE EFFECTS
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
#define PS_SYSTEM_HEAP_MEM_FREE(mem_ptr)                                    \
  do                                                                        \
  {                                                                         \
    ps_system_heapi_mem_free((void *)mem_ptr);                              \
    mem_ptr = NULL;                                                         \
  } while (0)


/*===========================================================================
FUNCTION  ps_system_heapi_mem_free

DESCRIPTION
  Internal helper routine for PS_SYSTEM_HEAP_MEM_FREE 

PARAMTERS
  mem_ptr - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ps_system_heap_mem_alloc().

SIDE EFFECTS
  None.
===========================================================================*/
void ps_system_heapi_mem_free
(
  void *mem_ptr
);

/*===========================================================================
FUNCTION  ps_system_heap_init

DESCRIPTION
  Performs system heap initialization

PARAMTERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_system_heap_init
(
  void
);

/*===========================================================================
FUNCTION  ps_system_heap_deinit

DESCRIPTION
  Performs system heap cleanup

PARAMTERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_system_heap_deinit
(
  void
);

/*===========================================================================
FUNCTION  ps_system_heap_simulate_out_of_mem

DESCRIPTION
  Simulates out of memory scenario for test purposes. Only to be used in 
  test scenarios, not in production builds.

PARAMTERS
  out_of_mem  - If TRUE  - then simulate out of memory.
                If FALSE - then reset simulate out of memory flag.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_system_heap_simulate_out_of_mem
(
  boolean out_of_mem
);


/*===========================================================================
FUNCTION  ps_system_heap_get_alloc_buf_count

DESCRIPTION
  Returns the number of allocated buffers by the PS_SYSTEM_HEAP. Used on
  test environments to ensure no memory leak.

PARAMTERS
  None.

RETURN VALUE
  Count of currently allocated buffers from ps_system_heap.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ps_system_heap_get_alloc_buf_count
(
  void
);



#ifdef __cplusplus
}
#endif

#endif /* PS_SYSTEM_HEAP_H */

