/*! \file pm_vs.c
*  
*  \brief Implementation file for VS resource type.  
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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/vs/src/pm_vs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_vs_driver.h"
#include "pm_resource_manager.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_vs_sw_mode_status(uint8 pmic_chip, uint8 perph_index, pm_sw_mode_type* sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);

    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_mode_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, perph_index, sw_mode);
    }
    return err_flag;
}

pm_err_flag_type pm_vs_pin_ctrled_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);
    uint8 select_pin = 0;
	
    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {  
        err_flag = pm_pwr_pin_ctrl_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, perph_index, on_off, &select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_vs_pin_ctrl_status(uint8 pmic_chip, uint8 perph_index, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);
    pm_on_off_type on_off = PM_INVALID;
	
    if (vs_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {  
        err_flag = pm_pwr_pin_ctrl_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, perph_index, &on_off, select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_vs_sw_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);
	
    if (vs_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, perph_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_vs_vreg_ok_status(uint8 pmic_chip, uint8 perph_index, boolean *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_vs_data_type *vs_ptr = pm_vs_get_data(pmic_chip);
	
    if (vs_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(vs_ptr->pm_pwr_data), vs_ptr->comm_ptr, perph_index, on_off);
    }
    return err_flag;
}
