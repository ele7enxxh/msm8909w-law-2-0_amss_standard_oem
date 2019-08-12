#ifndef PM_BOOT_CHG_H
#define PM_BOOT_CHG_H

/*! \file
*  \n
*  \brief  pm_boot_chg.h PMIC - BOOT-CHG RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm boot charging. 
*  \n &copy; Copyright 2011 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_boot_chg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/11   hs      Initial version. 
========================================================================== */
#include "comdef.h"
#include "pm_lib_err.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_boot_chg_led_source_type
   \brief The power source for anode of change indicator LED.
 */
typedef enum 
{
   /*! Ground       */
   PM_BOOT_CHG_LED_SRC__GROUND,
   /*! VPH_PWR      */
   PM_BOOT_CHG_LED_SRC__VPH_PWR,
   /*! Boost        */
   PM_BOOT_CHG_LED_SRC__BOOST,
   /*! Internal source       */
   PM_BOOT_CHG_LED_SRC__INTERNAL
}pm_boot_chg_led_source_type;

/*! \enum pm_boot_chg_attached_chgr_type
   \brief The attached charger type.
 */
typedef enum 
{
   /*! no charger.                  */
   PM_BOOT_CHG_ATTACHED_CHGR__NONE, 
   /*! USB path charger attached.   */
   PM_BOOT_CHG_ATTACHED_CHGR__USB,
   /*! DCIN path charger attached.  */
   PM_BOOT_CHG_ATTACHED_CHGR__DCIN,
   /*! Both USB and DCIN attached.  */
   PM_BOOT_CHG_ATTACHED_CHGR__BOTH
}pm_boot_chg_attached_chgr_type;
/*! \enum pm_smbc_chg_mode_type
   \brief The charging mode
 */
typedef enum
{
   /*! Charge OFF.   */
   PM_BOOT_CHG_MODE__CHARGE_OFF,
    /*! Charge ON     */
   PM_BOOT_CHG_MODE__CHARGE_ON,
    /*! ATC force OFF  */
   PM_BOOT_CHG_MODE__ATC_OFF,
    /*! ATC force ON  */
   PM_BOOT_CHG_MODE__ATC_ON
}pm_boot_chg_mode_type;
/*===========================================================================

                 FUNCTION PROTOTYPES

===========================================================================*/
/*! \brief This function sets the battery alarm thresholds for weak battery and dead battery
 *  \param[in] dead_battery_threshold. The dead battery threshold is the voltage threshold in 
 *        millivolts that when the when the battery is below this value will trigger the dead
 *        battery alarm
 *  @note if the value of parameter dead_battery_threshold is out of the valid range that  
 *        the PMIC HW can take, the value will be limited to the boundary and the PMIC HW 
 *        will be set to that value; at the same time, PM_ERR_FLAG__PAR1_OUT_OF_RANGE  
 *        will be returned to indicate that there was an out-of-bounds mV value parameter input.
 *
 *  \param[in] weak_battery_threshold. The weak battery threshold is the voltage threshold in 
 *        millivolts that when the when the battery is below this value will trigger the weak
 *        battery alarm
 *  @note if the value of parameter weak_battery_threshold is out of the valid range that  
 *        the PMIC HW can take, the value will be limited to the boundary and the PMIC HW 
 *        will be set to that value; at the same time, PM_ERR_FLAG__PAR2_OUT_OF_RANGE  
 *        will be returned to indicate that there was an out-of-bounds mV value parameter input.
 *
 *  \return Error flag.
 *
 *  <b>Dependencies</b>
 *  \li The battery alarm (if applicable) will be enabled in the same function after the battery
 *  thresholds are set.
 */ 
pm_err_flag_type pm_boot_chg_set_battery_thresholds(uint32 dead_battery_threshold, 
                                                    uint32 weak_battery_threshold);

/*! \brief This function checks if the the battery is weak
 *  \param[out] weak The status of the weak battery:
 *  \li TRUE The battery voltage is below the weak battery threshold  -- battery is weak.
 *  \li FALSE The battery voltage is above the weak battery threshold -- battery is not weak.
 *
 *  \return Error flag.
 *
 *  <b>Dependencies</b>
 *  \li .
 */ 
pm_err_flag_type pm_boot_chg_get_weak_battery_status(boolean *weak);

/**
 * @brief This function checks if the battery is present.
 * 
 * @details
 *  This function checks if the battery is present.
 *
 * @param[out] present Status of battery presence.
 *            -- TRUE:   battery is present
 *            -- FALSE:  battery is not present
 *     
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_boot_chg_get_battery_present(boolean *present);

/**
 * @brief This function gets the attached charger type.
 * 
 * @details
 *  This function gets the attached charger type.
 *   
 * @param[out] type The attached charger type.
 *            -- PM_BOOT_CHG_ATTACHED_CHGR__NONE: no charger attached
 *            -- PM_BOOT_CHG_ATTACHED_CHGR__USB:  USB device attached
 *            -- PM_BOOT_CHG_ATTACHED_CHGR__DCIN: DC wall charger attached
 *            -- PM_BOOT_CHG_ATTACHED_CHGR__BOTH: bot USB device and DC wall charger attached
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_boot_chg_get_attached_charger(pm_boot_chg_attached_chgr_type *type);

/**
 * @brief This function sets maximum USB charge current.
 * 
 * @details
 *  This function sets maximum USB charge current. USB charge current is the current 
 *  going through the USB cable.
 *   
 * @param[in] value_ma The current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take, 
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate that  
 *       there was an out-of-bounds mA value parameter input.
 *     
 *  \return Error flag.
 *
 * @note The default setting in PMIC HW is 100mA.
 *
 */
pm_err_flag_type pm_boot_chg_set_iusb_max(uint32 value_ma);

/**
 * @brief This function sets maximum fast charge battery current limit (IBAT).
 * 
 * @details
 *  This function sets maximum fast charge battery current limit.
 *   
 * @param[in] value_ma The maximum IBAT value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take, 
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate that  
 *       there was an out-of-bounds mA value parameter input.
 *     
 *  \return Error flag.
 *
 * @note The default setting in PMIC HW is 325mA. 
 *
 */
pm_err_flag_type pm_boot_chg_set_ibat_max(uint32 value_ma);

/**
 * @brief This function sets the system trickle charging current.
 * 
 * @details
 *  This function sets the system trickle charging current.
 *   
 * @param[in] value_ma The system trickle charging current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the PMIC HW can take, 
 *       the value will be limited to the boundary and the PMIC HW will be set to that value; at
 *       the same time, PM_BOOT_CHG_ERROR_TYPE__INVALID_PARAMETER will be returned to indicate 
 *       that there was an out-of-bounds mA value parameter input.
 *     
 *  \return Error flag.
 *
 * @note The default setting in PMIC HW is 50mA. 
 *
 */
pm_err_flag_type pm_boot_chg_set_itrkl(uint32 value_ma);
/**
 * @brief This function sets the charge mode.
 * 
 * @details
 *  This function turns sets the charge mode, such as enabling charging and disable
 *  charging, etc.
 *   
 * @param[in] mode This parameter is used to set certain mode in the BOOT CHG module. 
 *           -- PM_BOOT_CHG_MODE__CHARGE_OFF  This mode is used to set BOOT-CHG hardware 
 *              state machine in "charge off" state.
 *           -- PM_BOOT_CHG_MODE__CHARGE_ON This mode is used to set BOOT-CHG hardware 
 *              state machine in "charge on" state. 
 *           -- PM_BOOT_CHG_MODE__ATC_OFF This mode is used to set BOOT-CHG hardware 
 *              state machine in "ATC (Auto Trickle Charge) off" state. 
 *           -- PM_BOOT_CHG_MODE__ATC_ON This mode is used to set BOOT-CHG hardware 
 *              state machine in "ATC (Auto Trickle Charge) on" state. 
 *     
 *  \return Error flag.
 *
 * @note The default setting in PMIC HW is PM_SMBC_MODE__CHARGE_OFF. 
 *
 */
pm_err_flag_type pm_boot_chg_set_mode(pm_boot_chg_mode_type mode);

/**
 * @brief This function sets the charge LED and the LED source if applicable.
 * 
 * @details
 *  This function sets the system trickle charging current.
 *   
 * @param[in] enable This parameter is used to enable/disable the charger LED.
 *     
 * @param[in] source This parameter is used to set the charger LED source if applicable.
 * @note Some PMICs might not have the dedicated LED source for BOOT CHG; if not, this
 * parameter will be ignored.
 *     
 *  \return Error flag.
 *
 * @note The default setting in PMIC HW is 50mA. 
 *
 */
pm_err_flag_type pm_boot_chg_enable_led (boolean enable, pm_boot_chg_led_source_type source);

#endif /* PM_BOOT_CHG_H */

