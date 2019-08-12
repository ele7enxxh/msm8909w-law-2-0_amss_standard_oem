#ifndef PM_SMBB_H
#define PM_SMBB_H

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
 * @file pm_smbb.h  PMIC GPIO Module related declaration.
 *
 * @brief This file contains functions and variable declarations to support
 *   the PMIC SMBB module.
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
11/03/14   pmx     Creat
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_chg_common.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/**
 * @enum pm_smbb_which_type
 * @brief SMBB peripheral type.
 */
typedef enum
{
  PM_SMBB_1,
  PM_SMBB_INVALID
} pm_smbb_which_type;


/*===========================================================================

                    SMBB DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @name pm_smbb_chg_set_battery_threshold
 *
 * @brief This function sets the battery alarm thresholds for weak battery and dead battery.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] dead_battery_threshold The dead battery threshold is the voltage threshold in
 *             millivolts that when the when the battery is below this value will trigger the
 *             dead battery alarm
 * @note if the value of parameter dead_battery_threshold is out of the valid range that
 *        the PMIC HW can take, the value will be limited to the boundary and the PMIC HW
 *        will be set to that value; at the same time, PM_ERR_FLAG__PAR1_OUT_OF_RANGE
 *        will be returned to indicate that there was an out-of-bounds mV value parameter input.
 * @param [in] weak_battery_threshold. The weak battery threshold is the voltage threshold in
 *             millivolts that when the when the battery is below this value will trigger the weak
 *             battery alarm
 * @note if the value of parameter weak_battery_threshold is out of the valid range that
 *        the PMIC HW can take, the value will be limited to the boundary and the PMIC HW
 *        will be set to that value; at the same time, PM_ERR_FLAG__PAR2_OUT_OF_RANGE
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

pm_err_flag_type pm_smbb_chg_set_battery_threshold(uint8 pmic_chip,
                                                    pm_smbb_which_type smbb,
                                                    uint32 dead_battery_threshold,
                                                    uint32 weak_battery_threshold);

/**
 * @name pm_smbb_chg_get_weak_battery_status
 *
 * @brief This function checks if the the battery is weak.

 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral

 * @param [out] weak The status of the weak battery
 *                   TRUE: The battery voltage is below the weak battery threshold
 *                   FALSE: The battery voltage is above the weak battery threshold
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_chg_get_weak_battery_status(uint8 pmic_chip,
                                                     pm_smbb_which_type smbb,
                                                     boolean *weak);

/**
 * @name pm_smbb_chg_set_ibat_max
 *
 * @brief This function sets maximum fast charge battery current limit (IBAT).
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] value_ma The maximum IBAT value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take,
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate that
 *       there was an out-of-bounds mA value parameter input.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 *
 * @note The default setting in PMIC HW is 325mA.
 */
pm_err_flag_type pm_smbb_chg_set_ibat_max(uint8 pmic_chip,
                                          pm_smbb_which_type smbb,
                                          uint32 value_ma);

/**
 * @name pm_smbb_chg_set_itrkl
 *
 * @brief This function sets the system trickle charging current.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] value_ma The system trickle charging current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take,
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate
 *       that there was an out-of-bounds mA value parameter input.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 *
 * @note The default setting in PMIC HW is 50mA.
 *
 */
pm_err_flag_type pm_smbb_chg_set_itrkl(uint8 pmic_chip,
                                        pm_smbb_which_type smbb,
                                        uint32 value_ma);

/**
 * @name pm_smbb_chg_set_mode
 *
 * @brief  This function turns sets the charge mode, such as enabling charging and disable
 *         charging, etc.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] mode This parameter is used to set certain mode in the BOOT CHG module.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 *
 * @note The default setting in PMIC HW is PM_SMBC_MODE__CHARGE_OFF.
 */
pm_err_flag_type pm_smbb_chg_set_mode(uint8 pmic_chip,
                                      pm_smbb_which_type smbb,
                                      pm_chg_mode_type mode);

/**
 * @name pm_smbb_disable_chg
 *
 * @brief  This function configures PMIC to completely disable battery charging.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_disable_chg(uint8 pmic_chip,
                                     pm_smbb_which_type smbb);

 /**
 * @name pm_smbb_chg_get_battery_present
 *
 * @brief This function checks if the battery is present.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 *
 * @param [out] batt_present Status of battery presence.
 *                           TRUE:   battery is present
 *                           FALSE:  battery is not present
 * @param [out] batt_therm Status of battery therm.
 *                         TRUE:   battery therm is present
 *                         FALSE:  battery therm in not present
 * @param [out] batt_id Status of battery ID.
 *                      TRUE:   battery ID is present
 *                      FALSE:  battery ID is not present
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_chg_get_battery_present(uint8 pmic_chip,
                                                 pm_smbb_which_type smbb,
                                                 boolean *batt_present,
                                                 boolean *batt_therm,
                                                 boolean *batt_id);

/**
 * @name pm_smbb_chg_set_iusb_max
 *
 * @brief This function sets maximum USB charge current.
 *
 * @details
 *  This function sets maximum USB charge current. USB charge current is the current
 *  going through the USB cable.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] value_ma The current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take,
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate that
 *       there was an out-of-bounds mA value parameter input.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 *
 * @note The default setting in PMIC HW is 100mA.
 */
pm_err_flag_type pm_smbb_chg_set_iusb_max (uint8 pmic_chip,
                                            pm_smbb_which_type smbb,
                                            uint32 value_ma);

/**
 * @name pm_smbb_chg_get_attached_charger
 *
 * @brief This function gets the attached charger type.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [out] type The attached charger type.
 *                   PM_BOOT_CHG_ATTACHED_CHGR__NONE: no charger attached
 *                   PM_BOOT_CHG_ATTACHED_CHGR__USB:  USB device attached
 *                   PM_BOOT_CHG_ATTACHED_CHGR__DCIN: DC wall charger attached
 *                   PM_BOOT_CHG_ATTACHED_CHGR__BOTH: bot USB device and DC wall charger attached
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_chg_get_attached_charger(uint8 pmic_chip,
                                                  pm_smbb_which_type smbb,
                                                  pm_chg_attached_chgr_type *type);

/**
 * @name pm_smbb_chg_set_atc_led
 *
 * @brief This function sets ATC LED state.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] atc_led_state State of ATC LED.
 *                           True:  LED ON
 *                           False: LED OFF
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_chg_set_atc_led(uint8 pmic_chip,
                                         pm_smbb_which_type smbb,
                                         boolean atc_led_state);


/**
 * @name pm_smbb_chg_get_chg_gone_rt_status
 *
 * @brief This function gets CHG_GONE RT Status.
 *
 *  This function gets CHG_GONE RT status.
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 *
 * @param [out] chg_gone_rt_status Status of CHG_GONE
 *                                 True:  CHG_GONE is True
 *                                 False: CHG_GONE is False
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_chg_get_chg_gone_rt_status(uint8 pmic_chip,
                                                    pm_smbb_which_type smbb,
                                                    boolean *chg_gone_rt_status);

/**
 * @name pm_smbb_chg_set_comp_override
 *
 * @brief This function set COMP_OVER1 to enable/disable VBAT_WEAK threshold configuration.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] smbb SMBB peripheral
 * @param [in] comp_override_status Status of COMP_OVER1
 *                                  True:  VBAT_WEAK threshould is overriden by a
 *                                         lower Vbat level to start Fast charging
 *                                  False: disable overriding of VBAT_WEAK threshould
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @warning The pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_smbb_chg_set_comp_override(uint8 pmic_chip,
                                                pm_smbb_which_type smbb,
                                                boolean comp_override_status);

#endif    /* PM_SMBB_H */

