/*! \file
*  
*  \brief  pm_chg.c
*  \details Implementation file for charger driver. Currently for ATC_LED only
*    
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/14   pxm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_smbb.h"
#include "pm_lbc.h"
#include "pm_chg_common.h"
#include "pm_chg_driver.h"

/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/
pm_err_flag_type
pm_chg_set_atc_led
(
   uint32                          device_index,
   boolean                         atc_led_state 
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_atc_led(device_index, PM_LBC_1, atc_led_state);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_atc_led(device_index, PM_SMBB_1, atc_led_state);
    }
    else
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

