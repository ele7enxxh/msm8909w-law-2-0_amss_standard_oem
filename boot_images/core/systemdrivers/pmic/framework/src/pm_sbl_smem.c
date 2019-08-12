/*! \file pm_sbl_smem.c
*  \n
*  \brief This file contains PMIC Shared Memory initialization function.
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/src/pm_sbl_smem.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/03/13   kt      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_pbs_info.h"
#include "pm_boot.h"
#include "pm_pon.h"
#include "smem.h"

/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/

static pm_err_flag_type pm_pon_reasons_store_smem (void) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint64*          pon_smem_ptr = NULL;
  uint64           pon_reasons = 0;

  //Get all the PON reasons
  err_flag = pm_pon_get_all_pon_reasons(0, &pon_reasons);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
     return err_flag;
  }
    
  /* Storing the PON reasons to shared memory */
  pon_smem_ptr = (uint64 *) smem_alloc( SMEM_POWER_ON_STATUS_INFO, sizeof(uint64));

  if (pon_smem_ptr == NULL)
  {
      return PM_ERR_FLAG__INVALID;
  }

  *pon_smem_ptr = pon_reasons;
  
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_smem_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  //store the PBS info in the global context
  err_flag = pm_pbs_info_store_glb_ctxt ();

  //store the PON reasons in the shared memory
  err_flag |= pm_pon_reasons_store_smem ();

  return err_flag; 
}

