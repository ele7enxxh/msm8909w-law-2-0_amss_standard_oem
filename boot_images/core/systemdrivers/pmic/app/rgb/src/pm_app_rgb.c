/*! \file
*  
*  \brief  pm_app_rgb_led.c
*  \details Implementation file for rgb led resourece type.
*    
*  &copy; Copyright 2013-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/app/rgb/src/pm_app_rgb.c#1 $ 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/17/14   pxm    Update pm_rgb_led_config and add pm_rgb_led_enable API
06/24/14   aab     Cleaned up and added support for PMI8994
03/13/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rgb.h"
#include "pm_lpg.h"
#include "pm_led.h"
#include "pm_err_flags.h"
#include "pm_version.h"
#include "pm_target_information.h"
#include "CoreVerify.h"

pm_err_flag_type pm_rgb_led_enable
(
    uint8 device_index,
    uint32 rgb_mask,
    boolean enable
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_led_rgb_config_type* configs = NULL;
    pm_led_config_type* led_array = NULL;
    pm_led_type led_which;
    pm_led_info_type* led_info = NULL;
    uint8 idx = 0;

    if(PM_RGB_SEGMENT_R == rgb_mask)
    {
        led_which = PM_LED_RED;
    }
    else if(PM_RGB_SEGMENT_G == rgb_mask)
    {
        led_which = PM_LED_GREEN;
    }
    else if(PM_RGB_SEGMENT_B == rgb_mask)
    {
        led_which = PM_LED_BLUE;
    }
    else
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    led_info = pm_led_get_led_info();
    CORE_VERIFY_PTR(led_info);
    configs = led_info->rgb_array;
    CORE_VERIFY_PTR(configs);

    while(idx < 3)
    {
        if(rgb_mask == configs[idx].rgb)
        {
            break;
        }
        ++idx;
    }
    if(idx == 3)
    {
        return PM_ERR_FLAG__LED_RGB_CONFIG_ERROR;
    }
    
    led_array = led_info->led_array;
    CORE_VERIFY_PTR(led_array);
    
    while(led_array->type != PM_LED_TYPE_INVALID)
    {
        if(led_which == led_array->type)
        {
            break;
        }
        led_array++;
    }
    
    if(led_array->type == PM_LED_TYPE_INVALID)
    {
        return PM_ERR_FLAG__LED_NOT_EXIST;
    }

    err_flag = pm_rgb_led_config(device_index, rgb_mask, configs[idx].channel,
                        configs[idx].source, led_array->dim_level, enable);

    return err_flag;
}

pm_err_flag_type pm_rgb_led_config
(
    uint8                      device_index,
    uint32                     rgb_mask,
    pm_lpg_chan_type           lpg_channel,
    pm_rgb_voltage_source_type source_type,
    uint32                     dim_level,
    boolean                    enable_rgb
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if(rgb_mask != PM_RGB_SEGMENT_R && rgb_mask != PM_RGB_SEGMENT_G && rgb_mask != PM_RGB_SEGMENT_B)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    if(lpg_channel >= PM_LPG_CHAN_INVALID || lpg_channel == PM_LPG_CHAN_INVALID)
    {
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }

    if(source_type >= PM_RGB_VOLTAGE_SOURCE_INVALID)
    {
        return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
    }

    if(dim_level > PM_RGB_DIM_LEVEL_MAX)
    {
        return PM_ERR_FLAG__PAR5_OUT_OF_RANGE;
    }

    err_flag = pm_rgb_set_voltage_source(device_index, PM_RGB_1, source_type); //Select RGB voltage source
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    if(enable_rgb) 
    {
        //LPG Configuration
        err_flag = pm_lpg_pattern_config(device_index, lpg_channel, FALSE, FALSE, FALSE, FALSE, FALSE); //Configure for no pattern
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

        err_flag = pm_lpg_pwm_clock_sel(device_index, lpg_channel, PM_LPG_PWM_19_2_MHZ);  //Select 19.2 MHz clock
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

        err_flag = pm_lpg_pwm_set_pre_divide(device_index, lpg_channel, PM_LPG_PWM_PRE_DIV_5, PM_LPG_PWM_FREQ_EXPO_7); //Configure 390 Hz PWM frequency
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

        err_flag = pm_lpg_config_pwm_type(device_index, lpg_channel, FALSE, FALSE, FALSE, PM_LPG_PHASE_STAG_SHIFT_0_DEG);
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

        err_flag = pm_lpg_pwm_set_pwm_value(device_index, lpg_channel, dim_level);  //Configure DIM level
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

        err_flag = pm_lpg_set_pwm_bit_size(device_index, lpg_channel, PM_LPG_PWM_7BIT);    //Configure 7 bit mode
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

        err_flag = pm_lpg_pwm_enable(device_index, lpg_channel, TRUE); //enable LPG
        if (err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;
    }

    err_flag = pm_rgb_enable(device_index, PM_RGB_1, rgb_mask, enable_rgb, FALSE); //Turn ON/OFF RGB LED

    return err_flag;
}

