#ifndef __PM_LED_H__
#define __PM_LED_H__


/*! \file pm_led.h
 *  \n
 *  \brief  PMIC led driver APIs
 *  \details  This file contains functions and variable declarations to support
 *   the led which controlled by PMIC modules.
 *
 *
 *  \n &copy; Copyright (c) 2015  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/03/14   pxm     create
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_lpg.h"
#include "pm_rgb.h"
#include "pm_mpp.h"

// Channel for KPD and tri-color LED
// KPD only connect to MPP
// tri-color LED could connect to MPP or RGB
typedef enum
{
    PM_LED_CH_MPP1,
    PM_LED_CH_MPP_MIN = PM_LED_CH_MPP1,
    PM_LED_CH_MPP2,
    PM_LED_CH_MPP3,
    PM_LED_CH_MPP4,
    PM_LED_CH_MPP5,
    PM_LED_CH_MPP6,
    PM_LED_CH_MPP7,
    PM_LED_CH_MPP8,
    PM_LED_CH_MPP_MAX = PM_LED_CH_MPP8,

    PM_LED_CH_RED,
    PM_LED_CH_GREEN,
    PM_LED_CH_BLUE,
    PM_LED_CH_INVALID
} pm_led_channel;


/*
 * PM_LED_ATC means the LED connected to LED_ATC pin
 * PM_LED_KPD refer to the LED under home-row key. Till now, we only connect it to one of MPP 
 * PM_LED_REG/PM_LED_GREEN/PM_LED_BLUE refer to the tri-coloer LED. Usually if the PMIC has RGB module,
 * they'll be connected to RGB module and be controlled via LPG driver. Otherwise it usually be connected to 3 MPPs.
 * Sometimes tri-color LED may connect to 3 GPIOs, in this case, we need to extend our driver to support it
 * if they connect to PMIC GPIOs. If MSM GPIOs are used, the driver wouldn't be involved in pmic driver.
 * 8909 LC device uses I2C to control tri-color LED, this kind of scenarios are out of PMIC control. 
 */
typedef enum
{
    PM_LED_ATC,
    PM_LED_KPD, // Home-row keypad light
    PM_LED_RED,
    PM_LED_GREEN,
    PM_LED_BLUE,
    PM_LED_TYPE_INVALID
} pm_led_type;

typedef struct
{
    pm_led_type type;
    pm_led_channel channel; // For MPP and LPG, NOT applicable for ATC_LED
    uint16 dim_level; // For MPP and LPG, NOT applicable for ATC_LED
    // Comment below member, we may need them in the future.
    // Now we just use a default setting in specified in the driver
//    pm_lpg_pwm_clock_type clock;
//    pm_lpg_pwm_pre_divide_type pre_divide;
//    pm_lpg_pwm_freq_expo_type exp;
//    pm_lpg_pwm_size_type bit_size;
} pm_led_config_type;

/*
 * Only if tri-color LEDs are connected to RGB module,
 * this struct could use to identify each one in tri-color LED.
 */
typedef struct
{
    uint32 rgb;
    pm_lpg_chan_type channel;
    pm_rgb_voltage_source_type source;
    // uint16 dim_level;
} pm_led_rgb_config_type;

typedef struct
{
    pm_led_config_type* led_array;
    pm_led_rgb_config_type* rgb_array;
    pm_led_type default_led;
} pm_led_info_type;

/*
 * To turn on/off default LED defined in pm_config_target.c
 *
 * Must be called after pm_init
 */
pm_err_flag_type pm_led_default_led_enable
(
    uint8 device_index,
    boolean enable
);

/*
 * To get default led for notificaiton in SBL1 which is specified in pm_config_target.c
 * Will return PM_LED_TYPE_INVALID for error situation or the LED unavailable.
 * @return: The return value PM_ERR_FLAG__LED_NOT_EXIST doesn't always mean a error.
 *              It's quite possible that in some platform, LED is hw unavailable or NOT specified in config file
 */
pm_led_type pm_led_get_default_led(void);

/*
 * To turn on/off a LED with the default setting defined in pm_config_target.c
 * led_channel:
 *     Valid value:
 *         PM_LED_ATC,
 *         PM_LED_KPD,
 *         PM_LED_RED,
 *         PM_LED_GREEN,
 *         PM_LED_BLUE,
 *
 * @return: The return value PM_ERR_FLAG__LED_NOT_EXIST doesn't always mean a error.
 *              It's quite possible that in some platform, LED is hw unavailable or NOT specified in config file
 * Must be called after pm_init
 */
pm_err_flag_type pm_led_enable
(
    uint8 device_index,
    pm_led_type led_channel,
    boolean enable
);

/*
 * To turn on/off a LED with given dim level.
 * led_channel:
 *     Valid value:
 *         PM_LED_ATC,
 *         PM_LED_KPD,
 *         PM_LED_RED,
 *         PM_LED_GREEN,
 *         PM_LED_BLUE,
 *
 * dim_level:
 *      Valid value:
 *          For MPP:
 *              PM_MPP__I_SINK__LEVEL_5mA,
 *              PM_MPP__I_SINK__LEVEL_10mA,
 *              PM_MPP__I_SINK__LEVEL_15mA,
 *              PM_MPP__I_SINK__LEVEL_20mA,
 *              PM_MPP__I_SINK__LEVEL_25mA,
 *              PM_MPP__I_SINK__LEVEL_30mA,
 *              PM_MPP__I_SINK__LEVEL_35mA,
 *              PM_MPP__I_SINK__LEVEL_40mA,
 *          For RGB:
 *              <= PM_RGB_DIM_LEVEL_MAX
 * Must be called after pm_init
 */
pm_err_flag_type pm_led_config
(
    uint8 device_index,
    pm_led_type led_channel,
    boolean enable,
    uint16 dim_level
);

/*
 * return the struct pm_led_info_type defined in pm_config_target.c
 * will return NULL if unavailable or error
 */
pm_led_info_type* pm_led_get_led_info(void);

#endif /* end of __PM_LED_H__ */
