#ifndef PM_APP_LED_H
#define PM_APP_LED_H 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M    LED DRIVER

GENERAL DESCRIPTION
  This file contains PMIC code for LED 

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2013           by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/13   aks     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_led_source_type
   \brief The power source for anode of change indicator LED.
 */
typedef enum pm_led_volt_source_type
{
  /*! Ground       */
  PM_LED_SRC__GROUND,
  /*! VPH_PWR      */
  PM_LED_SRC__VPH_PWR,
  /*! Boost        */
  PM_LED_SRC__BOOST,
  /*! Internal source       */
  PM_LED_SRC__INTERNAL
}pm_led_volt_source;
/*! \enum pm_led_dim_level_type
   \brief Setting the brightness for the LED.
 */
typedef enum pm_led_dim_level_type
{ 
  PM_LED_DIM_LEVEL_LOW,     
  PM_LED_DIM_LEVEL_MID,  
  PM_LED_DIM_LEVEL_HIGH    
}pm_led_dim_level;
/*! \enum pm_rgb_led_type
   \brief The type of led RED, BLUE or GREEN.
 */
typedef enum pm_rgb_led_type
{
  PM_LED_RED,
  PM_LED_GREEN,
  PM_LED_BLUE
}pm_led_rgb;

/**
 * @brief This function sets the selected  LED, LED dim level and LED source if applicable.
 *   
 * @param[in] enable This parameter is used to enable/disable the  LED.
 *     
 * @param[in] led_rgb This parameter is used to select the led type to turn on/off.
 *
 * @param[in] src_type This parameter is used to set voltage source type for LED 
 * @note Some PMICs might not have the dedicated LED source ; if not, this
 * parameter will be ignored.
 * 
 * @param[in] dim_level This parameter is used to set the charger LED dim level 
 
 *     
 *  \return Error flag.
 *
 * @note The default setting in PMIC HW is 50mA. 
 *     Usage Example:  pm_rpm_enable_led_with_dim_ctrl(PM_LED_RED , true, PM_LED_SRC__VPH_PWR,DIM_LEVEL_LOW);
 *
 */
pm_err_flag_type  pm_rpm_enable_led_with_dim_ctrl(pm_led_rgb led_rgb, boolean enable, pm_led_volt_source led_volt_src, pm_led_dim_level dim_level);

#endif //PM_APP_LED_H
