/*! \file
*  
*  \brief  pm_app_pon.c
*  \details Implementation file for rgb led resourece type.
*    
*  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/app/pon/src/pm_app_pon.c#1 $
$Date: 2015/09/01 $ 
$Change: 8935858 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/03/15   rk      Added pmapp_ps_hold_cfg API for multi-pmic config
07/10/14   akm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon.h"
#include "pm_err_flags.h"
#include "device_info.h"

#define PON_SPARE_BIT_7 0x80

pm_err_flag_type 
pm_pon_uvlo_reset_status (uint8 pmic_device_index,boolean * status)
{
    pm_err_flag_type err_flag   = PM_ERR_FLAG__SUCCESS;
    uint8 pmic_spare_bit        = 0;
    uint8 warm_reset_reason_bit = 0;
    pm_pon_poff_reason_type poff_reason;
    pm_pon_warm_reset_reason_type warm_reset_reason;
    uint8 reg_data;
	
    pm_pon_get_poff_reason(pmic_device_index, &poff_reason);
    pm_pon_get_warm_reset_reason(pmic_device_index,&warm_reset_reason);
    pm_pon_get_spare_reg_data( pmic_device_index,PM_PON_XVDD_RB_SPARE ,&reg_data); 


    /*'pmic_spare_bit' bit<7> of XVDD_RB_SPARE reg is used as a flag for UVLO reset*/
	
    if( PON_SPARE_BIT_7 == reg_data)
    {
       pmic_spare_bit = 1;
    }

    /* Check if there is any warm reset reason bit is set*/
    warm_reset_reason_bit = warm_reset_reason.gp1 || warm_reset_reason.gp2 || 
                            warm_reset_reason.kpdpwr || warm_reset_reason.kpdpwr_and_resin || 
                            warm_reset_reason.pmic_wd || warm_reset_reason.ps_hold || 
                            warm_reset_reason.reserved || warm_reset_reason.reserved1 || 
                            warm_reset_reason.resin || warm_reset_reason.soft || 
                            warm_reset_reason.tft ;  

    /* If poff reason is UVLO and no warm_reset_reason and XVDD_SPARE_REG bit<7> is set, then return 
           UVLO reset status as TRUE */
    if (((poff_reason.gp1 ==1 ) || (poff_reason.uvlo == 1)) && (warm_reset_reason_bit == 0) && (pmic_spare_bit == 1))
    {
     *status = TRUE;
    }
    else
    {
     *status = FALSE;
    }

    return err_flag;
}

pm_err_flag_type 
pmapp_ps_hold_cfg(pmapp_ps_hold_cfg_type ps_hold_cfg)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_reset_cfg_type pon_reset_cfg = PM_PON_RESET_CFG_INVALID;

    switch(ps_hold_cfg)
    {
       case PMAPP_PS_HOLD_CFG_WARM_RESET:
          pon_reset_cfg = PM_PON_RESET_CFG_WARM_RESET;
          break;
       case PMAPP_PS_HOLD_CFG_HARD_RESET:
          pon_reset_cfg = PM_PON_RESET_CFG_HARD_RESET;
          break;
       case PMAPP_PS_HOLD_CFG_NORMAL_SHUTDOWN:
          pon_reset_cfg = PM_PON_RESET_CFG_NORMAL_SHUTDOWN;
          break;
       default:
          return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_pon_ps_hold_cfg(PM_GLOBAL_SID_PMIC_INDEX, pon_reset_cfg);

    return err_flag;
}
