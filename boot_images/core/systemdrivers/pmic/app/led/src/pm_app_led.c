/*! \file
*  
*  \brief  pm_app_led.c
*  \details Implementation file for led resourece type.
*    
*  &copy; Copyright 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/17/14   pxm    Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"
#include "CoreVerify.h"
#include "pm_led.h"
#include "pm_chg_common.h"
#include "pm_err_flags.h"

/*
 * To enable a specified LED with default dim level defined in pm_config_target.c
 */
pm_err_flag_type pm_led_enable
(
    uint8 device_index,
    pm_led_type led_type,
    boolean enable
)
{
    pm_led_config_type* array = NULL;
    pm_led_info_type* led_info = NULL;

    if(led_type >= PM_LED_TYPE_INVALID)
    {
        return PM_ERR_FLAG__LED_NOT_EXIST;
    }

    if(PM_LED_ATC == led_type)
    {
        return pm_chg_set_atc_led(device_index, enable);
    }

    led_info = pm_led_get_led_info();
    CORE_VERIFY_PTR(led_info);

    array = led_info->led_array;
    CORE_VERIFY_PTR(array);

    while(array->type !=  PM_LED_TYPE_INVALID)
    {
        if(array->type == led_type)
        {
            break;
        }
        ++array;
    }
    if(PM_LED_TYPE_INVALID == array->type)
    {
        return PM_ERR_FLAG__LED_NOT_EXIST;
    }

    return pm_led_config(device_index, led_type, enable, array->dim_level);
}

/* 
 * We would define a default LED in pm_config_target.c so that callee could call this API to on/off
 * one LED without effort of knowing which LEDs are available.
 * It's just a lazy API
 */
pm_err_flag_type pm_led_default_led_enable
(
    uint8 device_index,
    boolean enable
)
{
    pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;
    pm_led_type led_default;
    pm_led_info_type* led_info = NULL;

    led_info = pm_led_get_led_info();
    CORE_VERIFY_PTR(led_info);

    led_default = led_info->default_led;

    err = pm_led_enable(device_index, led_default, enable);

    return err;
}

/*
 * Return the default LED defined in pm_config_target.c.
 * In some scenarios, callee may need to do more specified control instead of simply on/off.
 * For example, they may want to change the dim level.
 */
pm_led_type pm_led_get_default_led()
{
    pm_led_type led_default = PM_LED_TYPE_INVALID;
    pm_led_info_type* led_info = NULL;

    led_info = pm_led_get_led_info();
    if(NULL != led_info)
    {
        led_default = led_info->default_led;
    }

    return led_default;
}

/*
 * To turn on/off a LED. If turning on the LED, a reasonable dim_level is required for controlling
 * brightness.
 * For ATC_LED, dim_level would be ignored.
 */
pm_err_flag_type pm_led_config
(
    uint8 device_index,
    pm_led_type led_type,
    boolean enable,
    uint16 dim_level
)
{
    pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;
    pm_led_config_type* array = NULL;
    pm_led_rgb_config_type* configs = NULL;
    uint8 idx = 0;
    uint32 rgb_channel;
    pm_led_info_type* led_info = NULL;

    if(led_type >= PM_LED_TYPE_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    if(PM_LED_ATC == led_type)
    {
        return pm_chg_set_atc_led(device_index, enable);
    }

    led_info = pm_led_get_led_info();
    CORE_VERIFY_PTR(led_info);

    array = led_info->led_array;
    CORE_VERIFY_PTR(array);

    while(array->type !=  PM_LED_TYPE_INVALID)
    {
        if(array->type == led_type)
        {
            break;
        }
        ++array;
    }
    if(PM_LED_TYPE_INVALID == array->type)
    {
        return PM_ERR_FLAG__LED_NOT_EXIST;
    }

    if(PM_LED_KPD == led_type)
    {
        if(dim_level >= PM_MPP__I_SINK__LEVEL_INVALID)
        {
            return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
        }

        // call MPP API to turn on/off
        if(enable)
        {
            err = pm_mpp_config_i_sink(device_index,
                (pm_mpp_perph_index)(array->channel - PM_LED_CH_MPP_MIN),
                (pm_mpp_i_sink_level_type)dim_level,
                PM_MPP__I_SINK__SWITCH_ENA);
        }
        else
        {
            err = pm_mpp_enable(device_index,
                    (pm_mpp_perph_index)(array->channel - PM_LED_CH_MPP_MIN),
                    FALSE);
        }
    }
    else if(PM_LED_RED == led_type || PM_LED_GREEN == led_type || PM_LED_BLUE == led_type)
    {
        // For tri-color led, find whether it was connected to MPP or RGB module.
        // If connected to MPP, read the channel and then call MPP API
        // If connected to RGB, read the rgb config and then call rgb API.
        if(/*array[idx].channel >= PM_LED_CH_MPP_MIN && */array->channel <= PM_LED_CH_MPP_MAX)
        {
            if(dim_level >= PM_MPP__I_SINK__LEVEL_INVALID)
            {
                return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
            }

            // tri-color LED connect to MPP
            if(enable)
            {
                err = pm_mpp_config_i_sink(device_index,
                    (pm_mpp_perph_index)(array->channel - PM_LED_CH_MPP_MIN),
                    (pm_mpp_i_sink_level_type)dim_level,
                    PM_MPP__I_SINK__SWITCH_ENA);
            }
            else
            {
                err = pm_mpp_enable(device_index,
                        (pm_mpp_perph_index)(array->channel - PM_LED_CH_MPP_MIN),
                        FALSE);
            }
        }
        else if(PM_LED_CH_RED == array->channel
            || PM_LED_CH_GREEN == array->channel
            || PM_LED_CH_BLUE == array->channel)
        {
            if(dim_level >= PM_RGB_DIM_LEVEL_MAX)
            {
                return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
            }

            // tri-color LED connect to RGB module
            if(PM_LED_CH_RED == array->channel)
            {
                rgb_channel = PM_RGB_SEGMENT_R;
            }
            else if(PM_LED_CH_GREEN == array->channel)
            {
                rgb_channel = PM_RGB_SEGMENT_G;
            }
            else
            {
                rgb_channel = PM_RGB_SEGMENT_B;
            }

            configs = led_info->rgb_array;
            CORE_VERIFY_PTR(configs);

            idx = 0;
            while(idx < 3)
            {
                if(rgb_channel == configs[idx].rgb)
                {
                    break;
                }
                ++idx;
            }
            if(idx == 3)
            {
                return PM_ERR_FLAG__LED_RGB_CONFIG_ERROR;
            }

            // err = pm_rgb_led_enable(device_index, rgb_channel, enable);
            err = pm_rgb_led_config(device_index, rgb_channel, configs[idx].channel,
                                    configs[idx].source, dim_level, enable);
        }
    }

    return err;
}

/*
 * Return the struct pm_led_info_type defined in pm_config_target.c
 */
pm_led_info_type* pm_led_get_led_info()
{
    static pm_led_info_type* led_info = NULL;
    static boolean data_initialized = FALSE;

    if(!data_initialized)
    {
        led_info = (pm_led_info_type*)pm_target_information_get_specific_info((uint32)PM_PROP_LED_INFO);
        data_initialized = TRUE;
    }

    return led_info;
}


