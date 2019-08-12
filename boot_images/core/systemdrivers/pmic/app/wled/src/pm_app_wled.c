/*! \file
*  
*  \brief  pm_app_wled.c
*  \details Implementation file for wled resourece type.
*    
*  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/app/wled/src/pm_app_wled.c#1 $ 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
 04/25/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_wled.h"
#include "pm_err_flags.h"

#define PM_RGB_DIM_LEVEL_MAX 0xFFF

pm_err_flag_type pm_wled_display_backlight_config
(
   unsigned                   device_index,
   pm_wled_led_type           led_type,
   uint16                     dim_level,
   pm_on_off_type             enable_backlight )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if(led_type >= PM_WLED_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if(dim_level > PM_RGB_DIM_LEVEL_MAX)
    {
        err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    else
    {
        err_flag = pm_wled_enable_current_sink(device_index, led_type); //Configure current sink for selected strings
        if(err_flag != PM_ERR_FLAG__SUCCESS){ return err_flag;}
                                                                                
        err_flag = pm_wled_enable_modulator(device_index, led_type);   //Configure modulator of selected strings
        if(err_flag != PM_ERR_FLAG__SUCCESS){ return err_flag;}

        err_flag = pm_wled_set_led_duty_cycle(device_index, led_type, dim_level); //configure dim level
        if(err_flag != PM_ERR_FLAG__SUCCESS){ return err_flag;}

        err_flag = pm_wled_enable_module(device_index, enable_backlight);  //Enable/Disable backlight
        if(err_flag != PM_ERR_FLAG__SUCCESS){ return err_flag;}
    }

    return err_flag;
}


