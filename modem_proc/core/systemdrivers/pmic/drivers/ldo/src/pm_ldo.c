/*! \file pm_ldo.c
*  
*  \brief Implementation file for LDO resource type.  
*  
*  \details Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation.
*  
*  &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/ldo/src/pm_ldo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_ldo_driver.h"
#include "pm_resource_manager.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/
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

pm_err_flag_type pm_ldo_pin_ctrled_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    uint8 select_pin = 0;

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off, &select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_pin_ctrl_status(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);
    pm_on_off_type on_off = PM_INVALID;

    if (ldo_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_pin_ctrl_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, &on_off, select_pin);
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

pm_err_flag_type pm_ldo_vreg_ok_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_soft_reset_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_soft_reset_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, status);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_soft_reset_exit(uint8 pmic_chip, uint8 ldo_peripheral_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_soft_reset_exit_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_volt_calculate_vset(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type  volt_level, uint32* vset)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_calculate_vset_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, volt_level, vset);
    }
    return err_flag;
}

pm_err_flag_type pm_ldo_volt_level_stepper_done_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean *stepper_done)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_ldo_data_type *ldo_ptr = pm_ldo_get_data(pmic_chip);

    if (ldo_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_stepper_done_status_alg(&(ldo_ptr->pm_pwr_data), ldo_ptr->comm_ptr, ldo_peripheral_index, stepper_done);
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
