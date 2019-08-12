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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/src/smem_os_common.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/09/13   bt      Add smem_map_base_va and smem_map_page_attr.
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
  FUNCTION  smem_map_base_va
===========================================================================*/
/**
  Map the memory region and return the virtual address of the mapped (physical)
  base address of SMEM.

  @param[in]    phys_addr     Memory region base (physical) address.
  @param[in]    size          Size of the SMEM region.

  @return
  Virtual address of the mapped SMEM region.
*/
/*=========================================================================*/
char *smem_map_base_va
(
  char         *phys_addr, 
  uint32        size
);

/*===========================================================================
  FUNCTION  smem_map_page_attr
===========================================================================*/
/**
  Map the page of SMEM (or a whole address range) at offset page_offset and 
  size of at least page_size.  The cached argument allows the SMEM module to 
  dynamically decide upon each address range's cacheability.
  
  The mapping will be contiguous, from SMEM's base physical address + 
  page_offset to SMEM's base virtual address + page_offset, with "RW"
  permisions.  Use the returned actual page-aligned size to determine where
  the mapping stops and when a new page must be mapped.

  @param[in]    page_offset   The offset from the base address to map (will be 
                              rounded down to a 4kB alignment).
  @param[in]    page_size     Size of this SMEM page or address range (will be
                              rounded up to a 4kB multiple).
  @param[in]    cached        Should this address range be cached memory?

  @return
  The actual page size mapped, rounded up to a 4kB multiple.
*/
/*=========================================================================*/
uint32 smem_map_page_attr
(
  uint32                page_offset, 
  uint32                page_size, 
  boolean               cached
);

#endif /* SMEM_OS_COMMON_H */
