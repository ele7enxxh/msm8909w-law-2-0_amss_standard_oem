#ifndef PM_LBC_H
#define PM_LBC_H

/**
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
*
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
*
*/

/**
 * @file pm_lbc.h  PMIC LBC Module related declaration.
 *
 * @brief This file contains functions and variable declarations to support
 *        the PMIC LBC module.
 */

/*===========================================================================

                  P M    H E A D E R    F I L E

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.



when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   pxm     Creat
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_chg_common.h"


#define ENUM_TIMER_STOP_MASK 0x01
#define ENUM_TIMER_STOP_RUN  0x00
#define ENUM_TIMER_STOP_STOP 0x01


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_lbc_which_type
 * @brief LBC type.
 */
typedef enum
{
  PM_LBC_1,
  PM_LBC_INVALID
} pm_lbc_which_type;


/*===========================================================================

                    LBC DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @name pm_lbc_chg_set_battery_threshold
 *
 * @brief This function sets the battery alarm thresholds for weak battery and dead battery.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc LBC peripheral
 * @param [in] dead_battery_threshold The dead battery threshold is the voltage threshold in
 *             millivolts that when the when the battery is below this value will trigger the
 *             dead battery alarm
 * @note if the value of parameter dead_battery_threshold is out of the valid range that
 *        the PMIC HW can take, PM_ERR_FLAG__PAR1_OUT_OF_RANGE will be returned to indicate 
 *        that there was an out-of-bounds mV value parameter input.
 * @param [in] weak_battery_threshold. The weak battery threshold is the voltage threshold in
 *             millivolts that when the when the battery is below this value will trigger the weak
 *             battery alarm
 * @note if the value of parameter weak_battery_threshold is out of the valid range that
 *        the PMIC HW can take, PM_ERR_FLAG__PAR2_OUT_OF_RANGE
 *        will be returned to indicate that there was an out-of-bounds mV value parameter input.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 *
 * @note The battery alarm (if applicable) will be enabled in the same function after the battery
 *       thresholds are set.
 */

pm_err_flag_type pm_lbc_chg_set_battery_threshold(uint8 pmic_chip,
                                                    pm_lbc_which_type lbc,
                                                    uint32 dead_battery_threshold,
                                                    uint32 weak_battery_threshold);


/**
 * @name pm_lbc_chg_get_chg_option
 *
 * @brief Get Internal charger option.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 *
 * @param [out] internal_chager Get internal charger option.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_get_chg_option( uint8 pmic_chip,
                                            pm_lbc_which_type lbc,
                                            boolean *internal_chager);

/**
 * @name pm_lbc_chg_set_vdd_max
 *
 * @brief Set voltage for max charging capacity of Battery (VDD max).
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 * @param [in] vdd_max VDD max voltage for Battery.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_vdd_max(uint8 pmic_chip,
                                        pm_lbc_which_type lbc,
                                        uint32 vdd_max);

/**
 * @name pm_lbc_chg_set_vdd_safe
 *
 * @brief Set voltage for safe charge capacity of Battery (VDD safe).
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 * @param [in] vdd_safe VDD safe voltage for Battery.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_vdd_safe(uint8 pmic_chip,
                                        pm_lbc_which_type lbc,
                                        uint32 vdd_safe);

/**
 * @name pm_lbc_chg_set_ibat_max
 *
 * @brief This function sets maximum fast charge battery current limit (IBAT).
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 * @param [in] value_ma The maximum IBAT value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take,
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate that
 *       there was an out-of-bounds mA value parameter input.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 *
 * @note The default setting in PMIC HW is 325mA.
 */
pm_err_flag_type pm_lbc_chg_set_ibat_max(uint8 pmic_chip,
                                           pm_lbc_which_type lbc,
                                           uint32 value_ma);

/**
 * @name pm_lbc_chg_set_ibat_safe
 *
 * @brief This function sets safe charge battery current limit (IBAT safe).
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 * @param [in] value_ma The safe IBAT value in mA.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_ibat_safe(uint8 pmic_chip,
                                            pm_lbc_which_type lbc,
                                            uint32 value_ma);

/**
 * @name pm_lbc_chg_set_ibat_safe
 *
 * @brief This function sets the minimum input voltage regulation level on 
 * USB_IN pin.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 * @param [in] value_mv The minimum input voltage on USB_IN, in the unit of mv
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_vin_min (uint8 pmic_chip,
                                            pm_lbc_which_type lbc,
                                            uint32 value_mv);


/**
 * @name pm_lbc_disable_chg
 *
 * @brief This function configures PMIC to completely disable battery charging.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_disable_chg(uint8 pmic_chip, pm_lbc_which_type lbc);

/**
 * @name pm_lbc_chg_set_atc_led
 *
 * @brief This function sets ATC LED state.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc Which LBC peripheral.
 * @param [in] atc_led_state State of ATC LED.
 *                           True:  LED ON
 *                           False: LED OFF
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_atc_led(uint8 pmic_chip,
                                           pm_lbc_which_type lbc,
                                           boolean atc_led_state);

/**
 * @name pm_lbc_chg_set_ibat_atc_b
 *
 * @brief This function sets ATC charge battery current limit.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 * @param [in] value_ma The ATC IBAT value in mA.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_ibat_atc_b(uint8 pmic_chip,
                                           pm_lbc_which_type lbc,
                                           uint32 value_ma);

/**
 * @name pm_lbc_chg_wdog_pet
 *
 * @brief This API pets watch dog while Battery charging.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_wdog_pet(uint8 pmic_chip, pm_lbc_which_type lbc);

/**
 * @name pm_lbc_chg_wdog_enable
 *
 * @brief This API Enable/Disable watch dog for Battery charging.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 * @param [in] enable Enable/Disable watch dog.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_wdog_enable(uint8 pmic_chip,
                                        pm_lbc_which_type lbc,
                                        boolean enable);

/**
 * @name pm_lbc_chg_set_comp_override
 *
 * @brief This function set COMP_OVER1 to enable/disable
 *        VBAT_WEAK threshold configuration.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 * @param [in] comp_override_status Status of COMP_OVER1
 *                                  True:  VBAT_WEAK threshould is overriden by a
 *                                         lower Vbat level to start Fast charging
 *                                  False: disable overriding of VBAT_WEAK
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_comp_override(uint8 pmic_chip,
                                               pm_lbc_which_type lbc,
                                               boolean comp_override_status);


/**
 * @name pm_lbc_chg_set_mode
 *
 * @brief This function turns sets the charge mode, such as enabling charging and disable
 *        charging, etc.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 * @param [in] mode This parameter is used to set certain mode in the BOOT CHG module.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_mode(uint8 pmic_chip,
                                       pm_lbc_which_type lbc,
                                       pm_chg_mode_type mode);

/**
 * @name pm_lbc_chg_get_battery_present
 *
 * @brief This function checks if the battery is present.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 *
 * @param [out] batt_present Status of battery presence.
 *                          TRUE:   battery is present
 *                          FALSE:  battery is not present
 * @param [out] batt_id Status of battery ID.
 *                     TRUE:   battery ID is present
 *                     FALSE:  battery ID is not present
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_get_battery_present(uint8 pmic_chip,
                                                pm_lbc_which_type lbc,
                                                boolean *batt_present,
                                                boolean *batt_id_presen);

/**
 * @name pm_lbc_chg_get_charger_present
 *
 * @brief This function checks if the charger is present.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 *
 * @param[out] charger_present Status of charger presence.
 *                             TRUE:   charger is present
 *                             FALSE:  charger is not present
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_get_charger_present(uint8 pmic_chip,
                                                pm_lbc_which_type lbc,
                                                boolean *charger_present);

/**
 * @name pm_lbc_chg_get_enum_timer_expired
 *
 * @brief This API used to get the status of ENUM timer.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 *
 * @param [out] enum_expired To check the status if Enum timer expired.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_get_enum_timer_expired(uint8 pmic_chip,
                                                   pm_lbc_which_type lbc,
                                                   boolean *enum_expired);
/**
 * @brief This function query whether enum timer is running. 
 * NOTE: lbc only.
 * 
 * @details
 * @param[in] device_index : Selects which PMIC
 * @param[in] lbc which LBC peripheral.
 * @param[out] run : whether enum timer is running
 *             TRUE: running
 *             FALSE: didn't run
 *
 *  \return Error flag.
 *  <b>Dependencies</b>
 *  \li The following function must have been called: pm_driver_init()
 */
pm_err_flag_type 
pm_lbc_chg_get_enum_timer_staus
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc,
   boolean                *run
);

/**
 * @name pm_lbc_chg_stop_enum_timer
 *
 * @brief This API used to stop ENUM timer when USB is enumerated.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_stop_enum_timer(uint8 pmic_chip, pm_lbc_which_type lbc);

/**
 * @name pm_lbc_chg_set_boot_done
 *
 * @brief This API set BOOT_DONE flag while charging by LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] lbc which LBC peripheral.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_lbc_chg_set_boot_done(uint8 pmic_chip, pm_lbc_which_type lbc);

#endif    /* PM_LBC_H */
