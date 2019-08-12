/*===========================================================================

                       P S _ S Y S T E M _ H E A P . C

DESCRIPTION
  The Data Services Protocol Stack system heap wrapper.

EXTERNALIZED FUNCTIONS

  ps_system_heap_mem_alloc()
    Allocates requested amount of memory from the system heap.

  PS_SYSTEM_HEAP_MEM_FREE()
    Frees the memory allocated through ps_system_heap_mem_alloc().

INTIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_IEnv.h"
#include "ds_Utils_env.h"

#include "ps_system_heap.h"

#include "_debug.h"

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

static IEnv* piEnv = 0;

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
)
{
   void* pv = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)IEnv_ErrMalloc(piEnv, num_bytes, &pv);

  return pv;

} /* ps_system_heap_mem_alloc() */

/*===========================================================================
FUNCTION  ps_system_heapi_mem_free

DESCRIPTION
  Frees the memory allocated by ps_system_heap_mem_alloc()

PARAMTERS
  mem_ptr   - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ps_system_heap_mem_alloc().

SIDE EFFECTS
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
void ps_system_heapi_mem_free
(
  void *mem_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  IENV_FREEIF(piEnv, mem_ptr);

} /* ps_system_heapi_mem_free() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (AEE_SUCCESS != env_GetCurrent(&piEnv)) {
      ASSERT(0);
   }
   IEnv_AddRef(piEnv);
   
} /* ps_system_heap_mem_init */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   IQI_RELEASEIF(piEnv);
   
} /* ps_system_heap_deinit */
