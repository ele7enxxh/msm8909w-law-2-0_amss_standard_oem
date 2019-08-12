/*! \file pm_ldo.c
*  \n
*  \brief Implementation file for LDO public APIs.
*  \n
*  &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/ldo/src/pm_ldo.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture  
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_ldo.h"
#include "pm_ldo_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     LOCAL FUNCTION PROTOTYPE

===========================================================================*/

static pm_err_flag_type pm_ldo_sw_mode_exit_bypass(pm_comm_info_type *comm_ptr, pm_register_address_type reg);

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_ldo_sw_mode(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(ldo_peripheral_index >= ldo_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (mode >= PM_SW_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type          reg = 0;
        pm_pwr_data_type                  *pwr_data = &(ldo_ptr->pm_pwr_data);
        pm_sw_mode_type                    curr_mode = PM_SW_MODE_INVALID;

        err_flag = pm_pwr_sw_mode_status_alg(pwr_data, ldo_ptr->comm_ptr, ldo_peripheral_index, &curr_mode);

        if (err_flag != PM_ERR_FLAG__SUCCESS)
        {
            return err_flag;
        }

        /* If the current mode status is same as the requested mode then return SUCCESS */
        if (curr_mode == mode) 
        {
            return PM_ERR_FLAG__SUCCESS;
        }
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[ldo_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: /* Low power mode */
          {
              /* the LDO is already in Bypass mode and will be changed to LPM */
              if (curr_mode == PM_SW_MODE_BYPASS)
              {
                  /* exit bypass properly */
                  err_flag = pm_ldo_sw_mode_exit_bypass(ldo_ptr->comm_ptr, reg);
                  if (err_flag != PM_ERR_FLAG__SUCCESS)
                  {
                      return err_flag;
                  }
              }

              err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x80, 0, 0);
          }
          break;
        case PM_SW_MODE_NPM: /* normal power mode */
          {
              /* the LDO is already in Bypass mode and will be changed to NPM */
              if (curr_mode == PM_SW_MODE_BYPASS)
              {
                  /* exit bypass properly */
                  err_flag = pm_ldo_sw_mode_exit_bypass(ldo_ptr->comm_ptr, reg);
                  if (err_flag != PM_ERR_FLAG__SUCCESS)
                  {
                      return err_flag;
                  }
              }
              err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x80, 0x80, 0);
          }
          break;
        case PM_SW_MODE_BYPASS: /* bypass mode */
          {
              err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x20, 0x20, 0);
          }
          break;
        default:
          err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
          break;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_ldo_sw_enable(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_pull_down(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pull_down_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}




pm_err_flag_type pm_ldo_sw_mode_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type* sw_mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, sw_mode);
    }
    return err_flag;
}


pm_err_flag_type pm_ldo_pin_ctrled(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, select_pin);
    }

    return err_flag;
}


pm_err_flag_type pm_ldo_volt_level(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_volt_level_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level);
    }

    return err_flag;
}

pm_err_flag_type pm_ldo_volt_level_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_volt_level_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level);
    }

    return err_flag;
}

pm_err_flag_type pm_ldo_vreg_ok_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *vreg_ok_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_vreg_ok_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, vreg_ok_status);
    }

    return err_flag;
}

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/

pm_err_flag_type pm_ldo_sw_mode_exit_bypass(pm_comm_info_type *comm_ptr, pm_register_address_type reg)
{
     pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   
    CORE_VERIFY_PTR(comm_ptr);

    /* step1: set BYPASS_ACT (bit 6) */
    err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x40, 0x40, 0);

    /* step2: wait 100uS */
    DALSYS_BusyWait(100);

    /* step3: clear BYPASS_ACT and BYPASS_EN (bit 6 and bit 5) */
    err_flag |= pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x60, 0x00, 0);

    return err_flag;
}


