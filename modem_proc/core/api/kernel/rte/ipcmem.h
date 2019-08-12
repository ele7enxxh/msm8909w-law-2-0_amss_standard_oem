#ifndef IPCMEM_H
#define IPCMEM_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

   S E C U R E   S O C K E T S   L A Y E R   D Y N A M I C   M E M O R Y 


GENERAL DESCRIPTION
  This module implements a simple dynamic memory management mechanism for
  the security services. It's main usage is for the porting of OpenSSL
  certificate parsing and verification routines, so that the memory
  allocation and free can be used as is in the OpenSSL code.

EXTERNALIZED FUNCTIONS
  ipcmem_pool_init() - Initialize the dynamic memory pool for security srv.
  ipcmem_malloc()    - Allocate a block of memory, like malloc.
  ipcmem_free()      - Free the allocated memory, like free.
  ipcmem_realloc()   - Reallocate a block of memory, like realloc
  ipcmem_remalloc()  - Free the existing block, and allocate a new block.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ipcmem_pool_init() has to be called exactly once before using any
  externalized functions provided by this module.

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/ipcmem.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/01   yll     Code cleanup.
05/16/01   yll     Added the high-watermark for the memory pool.
05/07/01   yll     Changes to support the OpenSSL certificate parsing and
                   verification.
11/28/00   yll     Ported from pdsmbuf module.

===========================================================================*/


/*===========================================================================

             Include Files for Module 

===========================================================================*/
#include "stddef.h"
#include "comdef.h"

/* ------------------------------------------------------------------------*/
/*                       Local Definitions And Data                        */
/* ------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION IPCMEM_POOL_INIT

DESCRIPTION
  This function is used to initialize the Dynamic buffer pools. It creates
  the memory pool based on the different pool id (size) and number of 
  buffers for this pool id, as defined in ipcmem_pool_info. As such, these
  numbers should be tuned to achieve minimum yet sufficient memory usage.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from SEC_TASK

===========================================================================*/
void ipcmem_pool_init( void );


/*===========================================================================

FUNCTION IPCMEM_ALLOCATE

DESCRIPTION
    This function is used for dynamic buffer allocation. It searches
    in the buffer pool corresponding to the size requested. If no more
    free space in this pool id, it searches for the next higher sized
    memory pool. At the most two pools are searched for a free buffer
    to avoid memory waistage.

RETURN VALUE
    If successful, a pointer to the allocated memory block is returned.
    Otherwise, a NULL pointer is returned.

DEPENDENCIES

===========================================================================*/
void *ipcmem_allocate( size_t size );


/*===========================================================================

FUNCTION IPCMEM_DEALLOCATE

DESCRIPTION
  This function is used for deallocating the memory. It also checks for 
  the possible memory corruptions.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void ipcmem_deallocate( void *buf );


/*===========================================================================

FUNCTION IPCMEM_REALLOC

DESCRIPTION
  This function changes the size of the block pointed to by ptr to
  size bytes and returns a pointer to the (possibly moved) block. 
  The contents are unchanged up to the lesser of the new and old sizes.
  If ptr is a null pointer, this function behaves like malloc() for 
  the specified size. If size is zero and ptr is not a null pointer, 
  the object it points to is freed.

RETURN VALUE
  If successful, a pointer to the re-allocated memory block is returned.
  Otherwise, a NULL pointer is returned.

DEPENDENCIES

===========================================================================*/
void *ipcmem_realloc( void *buf, size_t size );

/*===========================================================================

FUNCTION IPCMEM_VALIDATE_BUFFER

DESCRIPTION
   Validates if the given buffer is a valid ipcmem buffer, i.e. if the 
   address corresponds to a valid buffer, and if the canary bytes at 
   the end of the buffer (memory shim) are un-touched.

NOTE 
   The buffer passed in should be standing at IPC_MSG_EXT, 
   so that IPC_MSG_EXT_TO_INT maybe used to obtain the correct buffer start 
   and pool id (IPC_MSG_EXT_TO_INT(pMsg)- sizeof(int))

RETURN VALUE
   TRUE if the buffer is a valid ipcmem buffer AND canary bytes are untouched
   FALSE otherwise.  

DEPENDENCIES

===========================================================================*/
int ipcmem_validate_buffer(void* buffer);

#endif /* IPCMEM_H */
