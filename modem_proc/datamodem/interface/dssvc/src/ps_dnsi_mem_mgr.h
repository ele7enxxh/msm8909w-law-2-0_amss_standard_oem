#ifndef PS_DNSI_MEM_MGR_H
#define PS_DNSI_MEM_MGR_H
/*===========================================================================

                       P S _ D N S I _ M E M _ M G R . H

DESCRIPTION
  The Data Services DNS Subsystem memory manager module header file.
  The following functions are exported to the DNS subsystem.

EXTERNALIZED FUNCTIONS
  ps_dnsi_mem_init()
    Initializes the DNS memory manager subsystem during powerup.

  ps_dnsi_mem_alloc()
    Allocates dynamic memory from the heap.

  ps_dnsi_mem_free()
    Frees the memory allocated by ps_dnsi_mem_alloc()

INTIALIZATION AND SEQUENCING REQUIREMENTS
  The DNS memory manager subsytem should be intialized using
  ps_dnsi_mem_init() funtion during powerup.

Copyright (c) 2007-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "ps_system_heap.h"

/*===========================================================================

                         PUBLIC DEFINITIONS AND DATA

===========================================================================*/
/*---------------------------------------------------------------------------
This macro has been added to get rid of the compiler warning
warning: dereferencing type-punned pointer will break strict-aliasing rules.
C99/C89 rules on strict aliasing indictate which pointers may alias and the 
code violates this rule in C99.
ps_mem_dnsi_free() API should take an argument of opaque-type and which in C/C++
standard is void *. This implies you can pass any type to it as long as 
the caller & callee are aware of it. The caller is aware that the callee will
be recovering void ** type internally
---------------------------------------------------------------------------*/
/*===========================================================================
MACRO PS_DNSI_MEM_FREE(buf)

DESCRIPTION
  This macro is used for deallocating the memory.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_DNSI_MEM_FREE(buf)                                               \
{                                                                           \
  ps_dnsi_mem_free((void*)&buf);                                            \
}

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_MEM_ALLOC

DESCRIPTION
  The function is passed the amount of memory required.  If it finds a
  chunck of memory of suitable size to service the request it returns that
  otherwise it returns a NULL.

  This function may be called from tasks other then PS and therefore must
  be thread safe.

PARAMETERS
  size    - Size (in bytes) of the memory to be allocated.

RETURN VALUE
  Pointer to memory block if successful.
  NULL if could not get memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  May allocate a large DSM item.  The DSM item is not freed until all memory
  allocated from it is freed.
===========================================================================*/
void* ps_dnsi_mem_alloc
(
  uint32 size
);

/*===========================================================================
FUNCTION  PS_DNSI_MEM_FREE

DESCRIPTION
  Free memory allocated by dnsi_memalloc() and sets the passed memory
  pointer to NULL.

PARAMTERS
  mem_ptr_ptr   - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  dnsi_memalloc().

SIDE EFFECTS
  May free the memory chunk/DSM item which contains the chunk to be freed.
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
void ps_dnsi_mem_free
(
  void *mem_ptr
);

#endif /* PS_DNSI_MEM_MGR_H */
