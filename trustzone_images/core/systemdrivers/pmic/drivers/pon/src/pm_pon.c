/*! \file pm_pon.c
*  \n
*  \brief This file contains the implementation of the PowerON peripheral driver APIs.
*  \n
*  \n &copy; Copyright 2012-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/drivers/pon/src/pm_pon.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/13   ps      Added PON Stage3 reset config API CR#495834 
02/27/13   kt      Added PON Reasons and IRQ related APIs  
01/24/13   umr     Add new version support.
11/20/12   umr     Created
========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_pon.h"
#include "spmi_1_0_lite.h"
#include "pmio_pon.h"
#include "hw_module_type.h"
#include "device_info.h"
#include "DALSys.h"

/* PMIC Device Index IDs */
#define PMIC_A_DEVICE_ID   0
#define PMIC_B_DEVICE_ID   1

/* PMIC A Primary SlaveID */
#define PMIC_A_SLAVEID_PRIM                    0x00

#define PON_RESET_CFG_WARM_RESET_VAL                      0x01
#define PON_RESET_CFG_IMMEDIATE_X_VDD_SHUTDOWN_VAL        0x02
#define PON_RESET_CFG_NORMAL_SHUTDOWN_VAL                 0x04
#define PON_RESET_CFG_D_VDD_SHUTDOWN_VAL                  0x05
#define PON_RESET_CFG_X_VDD_SHUTDOWN_VAL                  0x06

#define PON_RESET_CFG_HARD_RESET_VAL                      0x07
#define PON_RESET_CFG_D_VDD_HARD_RESET_VAL                0x08
#define PON_RESET_CFG_X_VDD_HARD_RESET_VAL                0x09

#define PON_RESET_CFG_DVDD_HARD_RESET_VAL                 0x08 
#define PON_RESET_CFG_XVDD_HARD_RESET_VAL                 0x09 
#define PON_RESET_CFG_WARM_RESET_AND_DVDD_SHUTDOWN_VAL    0x0A 
#define PON_RESET_CFG_WARM_RESET_AND_XVDD_SHUTDOWN_VAL    0x0B 
#define PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN_VAL         0x0C 
#define PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET_VAL      0x0D 
#define PON_RESET_CFG_WARM_RESET_THEN_DVDD_HARD_RESET_VAL 0x0E 
#define PON_RESET_CFG_WARM_RESET_THEN_XVDD_HARD_RESET_VAL 0x0F

/* PON_DVDD_RB_SPARE reg Warm Reset indicating bit mask */
#define PON_DVDD_RB_SPARE_WARM_RESET_MASK                 0x01

typedef struct
{
  uint16 reset_ctl_addr;
  uint16 reset_ctl2_addr;
  uint16 reset_en_mask;
  uint16 reset_type_mask;
}pon_ps_hold_reg_info_type;


static pon_ps_hold_reg_info_type
pon_ps_hold_reg =
{
  PMIO_PON_PS_HOLD_RESET_CTL_ADDR,
  PMIO_PON_PS_HOLD_RESET_CTL2_ADDR,
  PMIO_PON_PS_HOLD_RESET_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_PS_HOLD_RESET_CTL_RESET_TYPE_BMSK
};

static boolean
pm_pon_initialized = FALSE;

pm_err_flag_type
pm_pon_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8              periph_type = 0;
  uint8              dig_major = 0;
  uint8              warm_reset_reason[2] = {0};

  /* Read PON Peripheral info and check if it's valid */
  err_flag = pm_spmi_lite_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_PERPH_TYPE_ADDR, &periph_type, 0);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  if (periph_type != PM_HW_MODULE_PON)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Read Dig Major Version info */
  err_flag = pm_spmi_lite_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_REVISION2_ADDR, &dig_major, 0);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  if (!dig_major)
  {
    /* Only needed on very first peripheral versions */
    pon_ps_hold_reg.reset_ctl2_addr = pon_ps_hold_reg.reset_ctl_addr;

  }

  /* Read Warm Reset reason */
  err_flag = pm_spmi_lite_read_byte_array(PMIC_A_SLAVEID_PRIM, PMIO_PON_WARM_RESET_REASON1_ADDR, 2, warm_reset_reason, 0);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Update the DVDD_RB_SPARE register bit <0> to 1 if Warm Reset occured */
  if ((warm_reset_reason[0] != 0) || (warm_reset_reason[1] != 0))
  {
    err_flag = pm_spmi_lite_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_PON_DVDD_RB_SPARE_ADDR, PON_DVDD_RB_SPARE_WARM_RESET_MASK, 0x01, 0);

    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }

  pm_pon_initialized = TRUE;
  return err_flag;
}


pm_err_flag_type
pm_pon_ps_hold_cfg(unsigned pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 ps_hold_cfg_val = PON_RESET_CFG_WARM_RESET_VAL;

  if (pmic_device_index > PMIC_A_DEVICE_ID)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }
  if (ps_hold_cfg >= PM_PON_RESET_CFG_INVALID)
  {
    return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
  }

  /* Disable reset */
  err_flag = pm_spmi_lite_write_byte_mask(PMIC_A_SLAVEID_PRIM,
                                          pon_ps_hold_reg.reset_ctl2_addr,
                                          pon_ps_hold_reg.reset_en_mask, 0x0, 0);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Requires 10 sleep clock cycles of delay */
  DALSYS_BusyWait(300);

  switch (ps_hold_cfg)
  {
     case PM_PON_RESET_CFG_WARM_RESET:
       ps_hold_cfg_val = PON_RESET_CFG_WARM_RESET_VAL;
       break;

     case PM_PON_RESET_CFG_NORMAL_SHUTDOWN:
       ps_hold_cfg_val = PON_RESET_CFG_NORMAL_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN:
       ps_hold_cfg_val = PON_RESET_CFG_D_VDD_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
       ps_hold_cfg_val = PON_RESET_CFG_X_VDD_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_HARD_RESET:
       ps_hold_cfg_val = PON_RESET_CFG_HARD_RESET_VAL;
       break;

     case PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
       ps_hold_cfg_val = PON_RESET_CFG_IMMEDIATE_X_VDD_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET:
       ps_hold_cfg_val = PON_RESET_CFG_D_VDD_HARD_RESET_VAL;
       break;

     case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
       ps_hold_cfg_val = PON_RESET_CFG_X_VDD_HARD_RESET_VAL;
       break;

     case PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
       ps_hold_cfg_val = PON_RESET_CFG_WARM_RESET_THEN_XVDD_HARD_RESET_VAL;
       break;

     default:
       /* Enable reset */
       err_flag = pm_spmi_lite_write_byte_mask(PMIC_A_SLAVEID_PRIM,
                                               pon_ps_hold_reg.reset_ctl2_addr,
                                               pon_ps_hold_reg.reset_en_mask, 0xFF, 0);

       return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  err_flag = pm_spmi_lite_write_byte_mask(PMIC_A_SLAVEID_PRIM,
                                          pon_ps_hold_reg.reset_ctl_addr,
                                          pon_ps_hold_reg.reset_type_mask,
                                          ps_hold_cfg_val, 0);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Enable reset */
  err_flag = pm_spmi_lite_write_byte_mask(PMIC_A_SLAVEID_PRIM,
                                          pon_ps_hold_reg.reset_ctl2_addr,
                                          pon_ps_hold_reg.reset_en_mask, 0xFF, 0);
  return err_flag;
}

