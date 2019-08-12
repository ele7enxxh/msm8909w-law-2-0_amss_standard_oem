/*===========================================================================

                    BOOT EXTERN PCIe DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external pcie drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_pcie_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/14   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_pcie_interface.h"
#include "pcie.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/


/* ============================================================================
**  Function : boot_pcie_reg_dump
** ============================================================================
*/
/**
  Calls the core dump.
  
  @param[in]     dump_addr  Address where the registers are dumped . 

  @return
  void.
  
  @dependencies
  None.
  
  
*/
void boot_pcie_reg_dump(void *dump_addr, uint32 max_size)
{
  pcie_reg_dump(dump_addr, max_size);
}


