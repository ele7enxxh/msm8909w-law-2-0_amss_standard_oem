#ifndef PM_CHG_H
#define PM_CHG_H

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
 * @file pm_chg_common.h  PMIC CHARGER Module related declaration.
 *
 * @brief This file contains functions and variable declarations to support
 *   the PMIC charger module.
 */

/*===========================================================================

                  P M    H E A D E R    F I L E

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_chg_common.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/18/14   mr      Doxygen complaint PMIC Header (CR-602405)
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/**
 * @enum pm_chg_attached_chgr_type
 * @brief The attached charger type.
 */
typedef enum
{
   PM_CHG_ATTACHED_CHGR__NONE,     /*!< no charger. */
   PM_CHG_ATTACHED_CHGR__USB,      /*!< USB path charger attached. */
   PM_CHG_ATTACHED_CHGR__DCIN,     /*!< DCIN path charger attached. */
   PM_CHG_ATTACHED_CHGR__BOTH      /*!< Both USB and DCIN attached. */
}pm_chg_attached_chgr_type;

/**
 * @enum pm_chg_type
 * @brief The pmic charger type
 */
typedef enum
{
    PM_CHG_SMBB,
    PM_CHG_LBC,
//    PM_CHG_CHIP_EXTERNAL,
    PM_CHG_INVAILD,
}pm_chg_type;


/**
 * @enum pm_chg_mode_type
 * @brief The charging mode
 */
typedef enum
{
   PM_CHG_MODE__CHARGE_OFF,    /*!< Charge OFF. */
   PM_CHG_MODE__CHARGE_ON,     /*!< Charge ON. */
   PM_CHG_MODE__ATC_OFF,       /*!< ATC force OFF. */
   PM_CHG_MODE__ATC_ON         /*!< ATC force ON. */
}pm_chg_mode_type;

/**
 * @enum chg_data_index
 * @brief Used to index SBL charging related data.
 */
typedef enum
{
    PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD,    /*!< 3420 mv FLCB_OS_BOOT_UP_THRESHOLD */
    PM_CHG_FLCB_WEAK_BATTERY_THRESHOLD,  /*!< 3200 mv FLCB_WEAK_BATTERY_THRESHOLD */
    PM_CHG_FLCB_FAST_CHG_THRESHOLD,      /*!< 2900 mv */
    PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD,  /*!< 2800 mV FLCB_DEAD_BATTERY_THRESHOLD */
    PM_CHG_DC_CHARGER_IMAX,              /*!<  500 mA */
    PM_CHG_IMAX_DEFAULT,                 /*!< 1050 mA */
    PM_CHG_USB_IDEV_DCHG,                /*!< 1500 mA QHSUSB_CHG_IDEV_DCHG */
    PM_CHG_USB_IDEV_FLOAT,               /*!< 1500 mA for SMBC and 1440 for LBC */
    PM_CHG_USB_IDEV_SDP,                 /*!<  500 mA QHSUSB_CHG_IDEV_SDP - Maximum current
                                                      for Standard Downstream Port */
    PM_CHG_USB_IUNIT,                    /*!<   90 mA QHSUSB_CHG_IUNIT - Amount of current
                                                      allowed on USB when USB is not configured */
    PM_CHG_USB_IBAT_ATC_B_NORMAL,        /* 450 mA for LBC by defatul */
    PM_CHG_VBAT_MAX_MAX,                 //  4770 mV for 8916
    PM_CHG_VBAT_MAX_MIN,                 //  4000 mV for 8916
    PM_CHG_VBAT_MAX_STEP,                //    25 mV for 8916
    PM_CHG_IBAT_MAX_MAX,                 //  1440 mA for 8916
    PM_CHG_IBAT_MAX_MIN,                 //    90 mA for 8916
    PM_CHG_IBAT_MAX_STEP,                //    90 mA for 8916
    PM_CHG_IBAT_SAFE,                    //  1440 ma for 8916
    PM_CHG_VIN_MIN,                      //  4200 mv for 8916
    PM_CHG_VBAT_TRKL_MIN_THRESHOLD,      /*!< 2050 mV */
    PM_CHG_VBAT_TRKL_MAX_THRESHOLD,      /*!< 2800 mV */
    PM_CHG_VBAT_WEAK_MIN_THRESHOLD,      /*!< 2100 mV */
    PM_CHG_VBAT_WEAK_MAX_THRESHOLD,      /*!< 3600 mV */
    PM_CHG_FLCB_OS_BOOTUP_AFTER_USB_SUSPEND_THRESHOLD, /*!< 3500 mV */
    PM_CHG_PMIC_CFG_DEVICE_INDEX,        /*!< PMIC Device index: 0: Primary PMIC; 1: Secondary PMIC */
    PM_CHG_DATA_INVALID_INDEX
} chg_data_index;


/*===========================================================================

                    CHARGER DRIVER FUNCTION DEFINITIONS

===========================================================================*/
/**
 * @name 
 *     pm_chg_is_charger_present
 *
 * @description
 *    This function checks for Charger precence
 * 
 * @param 
 *     boolean  charger_present
 *        TRUE:  charger is present
 *        FALSE: charger is NOT present
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_is_charger_present(boolean  *charger_present);

/**
 * @name pm_chg_set_battery_threshold
 *
 * @brief Set battery trickle threshold and weak threshold.
 * @note Available for SMBB, not support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] dead_battery_threshold Trickle charging threshold. in the unit of mv
 * @param [in] weak_battery_threshold Weak charging threshold. in the unit of mv
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_battery_threshold(uint32 pmic_device_index,
												uint32 dead_battery_threshold,
												uint32 weak_battery_threshold);

/**
 * @name pm_chg_get_weak_battery_status
 *
 * @brief Whether battery voltage less than weak threshold.
 * @note Available for SMBB, not support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] weak Get status whether battery voltage less than weak threshold.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_get_weak_battery_status(uint32 pmic_device_index, boolean *weak);

/**
 * @name pm_chg_set_ibat_max
 *
 * @brief Set ibat. The current limit that will charge into battery.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] value_ma The limit of current(ma) limit that would charge into battery.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_ibat_max(uint32 pmic_device_index, uint32 value_ma);

/**
 * @name pm_chg_set_ibat_safe
 *
 * @brief Set ibat safe. The safe limit for IBAT_MAX.
 * @note NOT available for SMBB in sw now, support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] value_ma The safe limit of IBAT_MAX.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_ibat_safe(uint32 pmic_device_index, uint32 value_ma);

/**
 * @name pm_chg_set_vin_min
 *
 * @brief This function sets the minimum input voltage regulation level on 
 * USB_IN pin.
 * @note NOT available for SMBB in sw now, support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] value_mv minimum input voltage regulation level on USB_IN pin.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_vin_min(uint32 pmic_device_index, uint32 value_mv);



/**
 * @name pm_chg_set_itrkl
 *
 * @brief Set trickle charging current.
 * @note Available for SMBB, not support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] value_ma Trickle charging current (ma).
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_itrkl(uint32 pmic_device_index, uint32 value_ma);

/**
 * @name pm_chg_set_mode
 *
 * @brief This function turns sets the charge mode, such as enabling charging and disable
 *        charging, etc.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mode This parameter is used to set certain mode in the BOOT CHG module.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_mode(uint32 pmic_device_index, pm_chg_mode_type mode);

/**
 * @name pm_disable_chg
 *
 * @brief This function configures PMIC to completely disable battery charging.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_disable_chg(uint32 pmic_device_index);

/**
 * @name pm_chg_get_battery_present
 *
 * @brief This function checks if the battery is present.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] batt_present Status of battery presence.
 *                           TRUE: battery is present
 *                           FALSE: battery is not present
 * @param [out] batt_id Status of battery ID.
 *                      TRUE: battery ID is present
 *                      FALSE: battery ID is not present
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
 pm_err_flag_type pm_chg_get_battery_present(uint32 pmic_device_index,
											   boolean *batt_present,
											   boolean *batt_id);

/**
 * @name pm_chg_set_iusb_max
 *
 * @brief Set the current (iUsb) limit that will be drawn from USB host/charger.
 * @note Available for SMBB, not support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] value_ma The limit of current (ma) limit that would be drawn from USB host/charger.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
 pm_err_flag_type pm_chg_set_iusb_max(uint32 pmic_device_index, uint32 value_ma);

/**
 * @name pm_chg_get_attached_charger
 *
 * @brief This function gets the attached charger type.
 * @note Available for SMBB, not support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] type Attached charger type.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_get_attached_charger(uint32 pmic_device_index, pm_chg_attached_chgr_type *type);

/**
 * @name pm_chg_set_atc_led
 *
 * @brief This function sets ATC LED state.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] atc_led_state State of ATC LED.
 *                           True:  LED ON
 *                           False: LED OFF
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_atc_led(uint32 pmic_device_index, boolean atc_led_state);

/**
 * @name pm_chg_get_chg_gone_rt_status
 *
 * @brief This function gets CHG_GONE RT Status.
 * @note Available for SMBB, not support in LBC.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] chg_gone_rt_status Status of CHG_GONE
 *                                 True:  CHG_GONE is True
 *                                 False: CHG_GONE is False
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_get_chg_gone_rt_status(uint32 pmic_device_index, boolean *chg_gone_rt_status);

/**
 * @name pm_chg_set_comp_override
 *
 * @brief This function set COMP_OVER1 to enable/disable
 *        VBAT_WEAK threshold configuration.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] comp_override_status Status of COMP_OVER1
 *                                 True: VBAT_WEAK threshould is overriden by a
 *                                       lower Vbat level to start Fast charging
 *                                 False: disable overriding of VBAT_WEAK threshould
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_comp_override(uint32 pmic_device_index, boolean comp_override_status);

/**
 * @name pm_chg_get_chg_option
 *
 * @brief This function read CHG_OPTION to determine whether to use
 *        internal or external charger
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param[out] internal_chager If internal charger used
 *                             True:  internal charger, pm8916
 *                             False: external charger, recommend smb1360.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_get_chg_option(uint32 pmic_device_index, boolean *internal_chager);

/**
 * @name pm_chg_set_ibat_atc_b
 *
 * @brief This function set ibat_atc_b.
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] value_ma Current drive strenght. 450ma or 990ma, the other is invalid.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_ibat_atc_b(uint32 pmic_device_index, uint32 value_ma);

/**
 * @name pm_chg_wdog_pet
 *
 * @brief This function pets charger wdog.
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_wdog_pet(uint32 pmic_device_index);

/**
 * @name pm_chg_wdog_enable
 *
 * @brief This function enables charger wdog.
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] enable Enable/Disable watch dog.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_wdog_enable(uint32 pmic_device_index, boolean enable);

/**
 * @name pm_chg_get_enum_timer_expired
 *
 * @brief This function query whether enum timer has expired.
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] enum_expired Whether expired. TRUE: has expired, FALSE: hasn't expired.
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_get_enum_timer_expired(uint32 pmic_device_index, boolean *enum_expired);

/**
 * @brief This function query whether enum timer is running. 
 * NOTE: lbc only.
 * 
 * @details
 * @param[in] device_index : Selects which PMIC
 * @param[out] run : whether enum timer is running
 *             TRUE: running
 *             FALSE: didn't run
 *
 *  \return Error flag.
 *  <b>Dependencies</b>
 *  \li The following function must have been called: pm_driver_init()
 */
pm_err_flag_type pm_chg_get_enum_timer_status(uint32 pmic_device_index, boolean *run);

/**
 * @name pm_chg_stop_enum_timer
 *
 * @brief This function stops enum timer.
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_stop_enum_timer(uint32 pmic_device_index);

/**
 * @name pm_chg_set_boot_done
 *
 * @brief This function sets boot done to avoid entering adaptive boot.
 * @note For LBC only.
 *
 * @param [in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *
 * @warning pm_driver_init() function must have been called.
 */
pm_err_flag_type pm_chg_set_boot_done(uint32 pmic_device_index);

/**
 * @name 
 *     pm_chg_get_peripheral_sub_type
 *
 * @description
 *    This function gets the charger sub type via register 0x1005
 * 
 * @return 
 *     pm_chg_type: PM_CHG_LBC or PM_CHG_SMBB
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_chg_type pm_chg_get_peripheral_sub_type(uint32 pmic_device_index);

#endif    /* PM_CHG_H */

