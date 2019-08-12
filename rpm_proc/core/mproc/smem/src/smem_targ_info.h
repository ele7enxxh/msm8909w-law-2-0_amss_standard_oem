#ifndef _SMEM_TARG_INFO_H_
#define _SMEM_TARG_INFO_H_

/*===========================================================================

           S H A R E D   M E M O R Y   T A R G E T   I N F O

DESCRIPTION
    Defines the private api that is used between SMEM module files regarding
    the memory mapping address and size.  This is a common file, and is not
    target-specific, but it defines types and functions used to dynamically
    determine target information.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem_targ_info.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/14/13   bt      Rename from IMEM to more-general Target Info since the 
                   structure's address is being moved to SMEM.
06/03/13   bt      Initial revision.

===========================================================================*/
/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_os.h"

/*===========================================================================
                        MACRO AND TYPE DEFINITIONS
===========================================================================*/
/* Identifier for the SMEM target info struct. 
 * "SIII" in little-endian. */
#define SMEM_TARG_INFO_IDENTIFIER     0x49494953

/*----------------------------------------------------------------------
 * The structure that is filled in by smem_boot_init() to 
 * maintain a single source that determines target information like the SMEM 
 * base physical address and size.  All images then use these values to set the 
 * base virtual address pointer at the beginning of smem_init().
 * 
 * Note: We cannot move these fields, and this struct must grow towards greater
 * addresses in order to support interprocessor versioning.  Badger only has a 
 * set 16-Bytes of reserved space, so any fields added to this struct cannot
 * be used on Badger targets.
 *----------------------------------------------------------------------*/
typedef struct
{
  uint32  identifier;
  uint32  smem_size;
  uint64  smem_base_phys_addr;
} smem_targ_info_type;

/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_map_smem_targ_info
===========================================================================*/
/**
  Map the SMEM target info region (if it has not been mapped by this function 
  already) and return the virtual base address of the structure.

  If this is not the first time this function has been called, it will simply 
  return the saved pointer, rather than attempting to do the mapping again.

  @return
  Pointer to the structure that holds the SMEM target info like physical 
  address and size.
*/
/*=========================================================================*/
smem_targ_info_type *smem_map_smem_targ_info( void );

/*===========================================================================
  FUNCTION  smem_save_targ_smem_info
===========================================================================*/
/**
  Saves the SMEM base physical address and size in a structure at a set 
  address.  This must be called from the beginning of smem_boot_init().

  The size and address can then be read by each image at the beginning of 
  smem_init.  This allows each other image to have a common binary for all 
  targets, irrespective of the memory map.

  @return
  None.
*/
/*=========================================================================*/
void smem_save_targ_smem_info( void );

#endif /* _SMEM_TARG_INFO_H_ */
