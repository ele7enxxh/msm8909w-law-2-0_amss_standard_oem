/*===========================================================================

                      S H A R E D   M E M O R Y   R E X

DESCRIPTION
    Defines the REX OS specific helper functions for SMEM API.

Copyright (c) 2011 - 2015 by Qualcomm Technologies, Incorporated.  All
Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/13   bt      Support both 32-bit and 64-bit architectures.
11/19/13   bt      Rename IMEM defines to more-generic target info.
06/12/13   bt      Remove smem_malloc(), it is defined in smem_dal.c or
                   not needed on SBL/TZ (would be defined in smem_boot.c).
05/14/13   bt      Fall back to SMEM defines if physical address and size were
                   not filled out by SBL in System IMEM.  Images using this
                   file assume default 1-to-1 memory mapping for SMEM.
04/24/13   pa      Added smem_malloc().
01/10/13   bt      Re-created after deletion for processors using target.builds
                   SMEM defines.
01/25/12   bt      Updated HWIO macro and base names as of Virtio v2.0.0.
01/03/12   bt      Integrated/ported HW mutex mapping to Rex.
02/17/11   bm      Initial version.
===========================================================================*/
/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include <stdint.h>
#include "smem_os.h"
#include "smem_os_common.h"
#include "smem_v.h"
#include "smem_target.h"
#include "smem_targ_info.h"

/*===========================================================================
                  PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_os_ext_get_smem_base
===========================================================================*/
/**
  Returns the shared memory base physical address.

  @return
  Shared memory base physical address
*/
/*=========================================================================*/
void *smem_os_ext_get_smem_base( void )
{
  smem_targ_info_type  *smem_targ_info_entry;

  /* Try to get SMEM base from smem target info struct. */
  smem_targ_info_entry = smem_map_smem_targ_info();

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();

  if(smem_targ_info_entry &&
     smem_targ_info_entry->identifier == SMEM_TARG_INFO_IDENTIFIER)
  {
    uint64  phys_addr;

#ifdef SMEM_RPM_PROC
    phys_addr = SMEM_DDR_SOC_TO_RPM_MAP(
                    smem_targ_info_entry->smem_base_phys_addr);
#else
    /* Cast uint64 to a pointer. */
    phys_addr = smem_targ_info_entry->smem_base_phys_addr;
#endif

    SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();

    return (void *)(uintptr_t)phys_addr;
  }
  SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();

  ERR_FATAL("smem_os_ext_get_smem_base: Cannot determine SMEM physical "
            "address!", 0, 0, 0);
  return SMEM_INVALID_ADDR;
}

/*===========================================================================
  FUNCTION  smem_os_ext_get_smem_size
===========================================================================*/
/**
  Returns the shared memory size.

  @return
  Shared memory size
*/
/*=========================================================================*/
uint32 smem_os_ext_get_smem_size( void )
{
  smem_targ_info_type *smem_targ_info_entry;

  /* Try to get SMEM base from smem target info struct. */
  smem_targ_info_entry = smem_map_smem_targ_info();

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  if(smem_targ_info_entry &&
     smem_targ_info_entry->identifier == SMEM_TARG_INFO_IDENTIFIER)
  {
    uint32  smem_size;

    smem_size = smem_targ_info_entry->smem_size;

    SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
    return smem_size;
  }
  SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();

  ERR_FATAL("smem_os_ext_get_smem_size: Cannot determine SMEM size!", 0, 0, 0);
  return 0;
}
