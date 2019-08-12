#ifndef DS_SYSTEM_HEAP_H
#define DS_SYSTEM_HEAP_H
/*===========================================================================

                       D S _ S Y S T E M _ H E A P . H

DESCRIPTION
  The Data Services Modem handler system heap wrapper.

EXTERNALIZED FUNCTIONS

  ds_system_heap_mem_alloc()
    Allocates requested amount of memory from the system heap.

  ds_system_heap_mem_free()
    Frees the memory allocated through ds_system_heap_mem_alloc().

INTIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010 - 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds_system_heap.h#1 $
  $DateTime: 2010/06/12 18:56:57

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/10/12    nd     Created initial module.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "modem_mem.h"

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION  ds_system_heap_mem_alloc
          ds_system_heap_mem_alloc_crit //ONLY use in critical memory need

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
#define ds_system_heap_mem_alloc(NUM_BYTES) \
        ds_system_heap_mem_alloc_ext(NUM_BYTES, FALSE, __FILENAME__, __LINE__)

#define ds_system_heap_mem_alloc_crit(NUM_BYTES) \
        ds_system_heap_mem_alloc_ext(NUM_BYTES, TRUE, __FILENAME__, __LINE__)

INLINE void* ds_system_heap_mem_alloc_ext
(
  unsigned long num_bytes,
  boolean crit_flag,
  char * file_name,
  unsigned int line_number
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  void * tmp = modem_mem_alloc_ext(num_bytes,
                                   crit_flag ? MODEM_MEM_CLIENT_DATA_CRIT 
                                             : MODEM_MEM_CLIENT_DATA,
                                   file_name, line_number);
  MSG_MED("modem_mem_alloc 0x%x:FILE %s:LINE %d",(unsigned int)tmp,file_name,line_number);
  return tmp;
} /* ds_system_heap_mem_alloc_ext() */

/*===========================================================================
FUNCTION  ds_system_heap_mem_free

DESCRIPTION
  Frees the memory allocated by ds_system_heap_mem_alloc() 

PARAMTERS
  mem_ptr_ptr   - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ds_system_heap_mem_alloc().

SIDE EFFECTS
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
#define ds_system_heap_mem_free(PTR) \
        ds_system_heap_mem_free_ext(PTR, FALSE, __FILENAME__,__LINE__)

#define ds_system_heap_mem_free_crit(PTR) \
        ds_system_heap_mem_free_ext(PTR, TRUE, __FILENAME__,__LINE__)

#define DS_SYSTEM_HEAP_MEM_FREE(PTR) \
         ds_system_heap_mem_free_ext(PTR, FALSE, __FILENAME__,__LINE__)
 
#define DS_SYSTEM_HEAP_MEM_FREE_CRIT(PTR) \
         ds_system_heap_mem_free_ext(PTR, TRUE, __FILENAME__,__LINE__)

INLINE void ds_system_heap_mem_free_ext
(
  void *mem_ptr_ptr,
  boolean crit_flag,
  char * file_name,
  unsigned int line_number
)
{
  if (NULL != mem_ptr_ptr)
  {
    MSG_MED("modem_mem_free 0x%x:FILE %s:LINE %d",(unsigned int)mem_ptr_ptr,
                                                   file_name,
                                                   line_number);
    if(NULL != mem_ptr_ptr )
    {
      modem_mem_free_ext(mem_ptr_ptr,
                         crit_flag ? MODEM_MEM_CLIENT_DATA_CRIT 
                                   : MODEM_MEM_CLIENT_DATA,
                         file_name,line_number);
      mem_ptr_ptr = NULL;
    }
  }
} /* ds_system_heap_mem_free_ext() */

#endif /* DS_SYSTEM_HEAP_H */
