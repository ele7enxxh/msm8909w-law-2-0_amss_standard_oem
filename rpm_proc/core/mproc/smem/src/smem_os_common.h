/*---------------------------------------------------------------------------
   SMEM_OS_COMMON.H  - Common Header for SMEM OS Implementations
---------------------------------------------------------------------------*/
/*!
  @file
    smem_os_common.h

  @brief
    This file contains the common prototypes for SMEM functions that must be
    implemented for all images depending on OS and available APIs.
*/

/*--------------------------------------------------------------------------
     Copyright  2012-2013 Qualcomm Technologies Incorporated. 
     All rights reserved.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem_os_common.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/24/13   pa      Added smem_malloc().
01/09/13   pa      Added smem_set_spin_lock_api.
12/04/12   bt      Add OS abstraction for retrieval of SMEM address and size.
02/09/12   bt      Initial revision.
===========================================================================*/
#ifndef SMEM_OS_COMMON_H
#define SMEM_OS_COMMON_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_os.h"

/*--------------------------------------------------------------------------
  Macro definitions
---------------------------------------------------------------------------*/
#define SMEM_SUCCESS    (0)
#define SMEM_ERROR     (-1)

/*--------------------------------------------------------------------------
  Function declarations
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  smem_timetick_init
===========================================================================*/
/**
  Creates the handle for the OS-specific timetick handle, if necessary.
 
  @return
  SMEM_ERROR on failure
  SMEM_SUCCESS on success
*/
/*=========================================================================*/
int32 smem_timetick_init( void );

/*===========================================================================
  FUNCTION  smem_timetick_get
===========================================================================*/
/**
  Returns the current timetick value.
 
  @return
  The current timetick value, measured in ticks.
  0 if an error occurred.
*/
/*=========================================================================*/
uint32 smem_timetick_get( void );

/*===========================================================================
  FUNCTION  smem_set_spin_lock_api
===========================================================================*/
/**
  Lookup chip family and version to determine whether to use one of two 
  spinlock mechanisms:
  For 9x25/8974 below version 2: 
    use exclusive memory access spinlock API
  Otherwise:
    use HW mutex spinlock API.
 
  @return
  None.
*/
/*=========================================================================*/
void smem_set_spin_lock_api( void );

/*===========================================================================
  FUNCTION  smem_os_ext_get_smem_base
===========================================================================*/
/**
  Returns the shared memory base physical address.

  @return
  Shared memory base physical address
*/
/*=========================================================================*/
void *smem_os_ext_get_smem_base( void );

/*===========================================================================
  FUNCTION  smem_os_ext_get_smem_size
===========================================================================*/
/**
  Returns the shared memory size.

  @return
  Shared memory size
*/
/*=========================================================================*/
uint32 smem_os_ext_get_smem_size( void );

/*===========================================================================
  FUNCTION  smem_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap. 

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void * smem_malloc(uint32 size);

#endif /* SMEM_OS_COMMON_H */
