/*===========================================================================

                       P S _ D N S I _ M E M _ M G R . C

DESCRIPTION
  The Data Services DNS Subsystem memory manager module.

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

Copyright (c) 2007-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/ps_dnsi_mem_mgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   pp      Moved Local Heap allocations to Modem Heap.
03/26/07   hm      Initial development work.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "ps_dnsi_mem_mgr.h"
#include "err.h"
#include "msg.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

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
)
{
  uint32                     total_size;
  void * buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify parameter
  -------------------------------------------------------------------------*/
  if( 0 == size || 0xfffffff0U < size )
  {
    LOG_MSG_ERROR_1( "Invalid size %d in ps_dnsi_memalloc()", size );
    return NULL;
  }

  total_size = (sizeof(int32) - 1 + size) & (~(sizeof(int32) - 1));

  /* Get from Modem heap */
   PS_SYSTEM_HEAP_MEM_ALLOC(buf, total_size, void*);
   return buf;
} /* ps_dnsi_mem_alloc() */


/*===========================================================================
FUNCTION  PS_DNSI_MEM_FREE

DESCRIPTION
  Free memory allocated by ps_dnsi_mem_alloc() and sets the passed memory
  pointer to NULL.

PARAMTERS
  mem_ptr_ptr   - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ps_dnsi_mem_alloc().

SIDE EFFECTS
  May free the memory chunk/DSM item which contains the chunk to be freed.
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
void ps_dnsi_mem_free
(
  void *mem_ptr
)
{
   void ** mem_ptr_ptr = (void **)mem_ptr;            
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Verify parameter and return if NULL pointer passed
  ------------------------------------------------------------------------*/
  if( NULL == mem_ptr_ptr )
  {
    LOG_MSG_ERROR_0( "Null argument passed" );
    return;
  }

  if( NULL == *mem_ptr_ptr )
  {
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(*mem_ptr_ptr);

  return;
} /* ps_dnsi_mem_free() */


