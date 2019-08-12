/*! \file pm_boost.c
*  
*  \brief Implementation file for BOOST resource type.  
*  
*  \details Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation.
*  
*  &copy; Copyright 2013-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/boost/src/pm_boost.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/22/13    kt       Adding status APIs for boost driver
1/29/13    aks      Adding support for 5v boost as a separate driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_boost.h"
#include "pm_boost_driver.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_boost_volt_level_status(uint8 pmic_chip, uint8 boost_peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_boost_data_type *boost_ptr = pm_boost_get_data(pmic_chip);
	
	if (boost_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_status_alg(&(boost_ptr->pm_pwr_data), boost_ptr->comm_ptr, boost_peripheral_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type pm_boost_sw_enable_status(uint8 pmic_chip, uint8 boost_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_boost_data_type *boost_ptr = pm_boost_get_data(pmic_chip);
	
	if (boost_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
		err_flag = pm_pwr_sw_enable_status_alg(&(boost_ptr->pm_pwr_data), boost_ptr->comm_ptr, boost_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_boost_vreg_ok_status(uint8 pmic_chip, uint8 boost_peripheral_index, boolean *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_boost_data_type *boost_ptr = pm_boost_get_data(pmic_chip);
	
	if (boost_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
		err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(boost_ptr->pm_pwr_data), boost_ptr->comm_ptr, boost_peripheral_index, on_off);
    }
    return err_flag;
}