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

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/ldo/src/pm_ldo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/13   hs     init version
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
    uint8 periph_type = 0;

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

        periph_type = pwr_data->pwr_specific_info[ldo_peripheral_index].periph_type;
        
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[ldo_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: /* Low power mode */
          {
              /* the LDO is already in Bypass mode and will be changed to LPM */
              if (ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                  /* exit bypass properly */
                  err_flag = pm_ldo_sw_mode_exit_bypass(ldo_ptr->comm_ptr, reg);
                  if (err_flag == PM_ERR_FLAG__SUCCESS)
                  {
                      /* update the bypass LUT */
                      ldo_ptr->in_bypass[ldo_peripheral_index] = 0;
                  }
              }

              err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x80, 0, 0);
          }
          break;
        case PM_SW_MODE_NPM: /* normal power mode */
          {
              /* the LDO is already in Bypass mode and will be changed to NPM */
              if (ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                  /* exit bypass properly */
                  err_flag = pm_ldo_sw_mode_exit_bypass(ldo_ptr->comm_ptr, reg);
                  if (err_flag == PM_ERR_FLAG__SUCCESS)
                  {
                      /* update the bypass LUT */
                      ldo_ptr->in_bypass[ldo_peripheral_index] = 0;
                  }
              }
              err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x80, 0x80, 0);
          }
          break;
        case PM_SW_MODE_BYPASS: /* bypass mode */
          {
              if (!ldo_ptr->in_bypass[ldo_peripheral_index])
              {
                  err_flag = pm_comm_write_byte_mask(ldo_ptr->comm_ptr->slave_id, reg, 0x20, 0x20, 0);
                  if (err_flag == PM_ERR_FLAG__SUCCESS)
                  {
                      /* update the bypass LUT */
                      ldo_ptr->in_bypass[ldo_peripheral_index] = 1;
                  }
              }
          }
          break;
        default:
          err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
          break;
        }
    }

    if (PM_ERR_FLAG__SUCCESS == err_flag)
    {
        PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_MODE, pmic_chip, periph_type, ldo_peripheral_index, mode);
    }
    else
    {
        PM_LOG_MSG_ERROR(PMIC_DRV_PWR_MODE_ERROR, pmic_chip, periph_type, ldo_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_ldo_sw_mode_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type *mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, mode);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_sw_mode_status_raw(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 *mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_mode_status_raw_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, mode);
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

pm_err_flag_type pm_ldo_sw_enable_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type *on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_vreg_ok_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean* vreg_ok) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, vreg_ok);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_calculate_vset(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level, uint32 *vset) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    pm_volt_level_type dummy_ceiling = 0;

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_volt_calculate_vset_celing_uv(&(ldo_ptr->pm_pwr_data), 
        	              ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level, vset,&dummy_ceiling);
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

