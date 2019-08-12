#ifndef PM_SMBC_H
#define PM_SMBC_H

/*! \file
*  \n
*  \brief  pm_smbc.h PMIC-SMBC MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SMBC (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SMBC) module includes a buck regulated 
*  battery charger with integrated switches. The SMBC module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy; Copyright 2011 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_smbc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/11   hs      Added modes to set the gain for battery current monitor
                   output to XOADC.
01/07/11   hs      Initial version. 
========================================================================== */
#include "comdef.h"
#include "pm_lib_err.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_smbc_cmd_type
   \brief Use these commands to enable or disable certain functionalities in
 *  the SMBC module.
 */
typedef enum
{
    /*! Disable the functionality.  */
   PM_SMBC_CMD__DISABLE,
    /*! Enable the functionality.   */
   PM_SMBC_CMD__ENABLE
}pm_smbc_cmd_type;
/*! \enum pm_smbc_vdd_default_type
   \brief The default VDD value.
 */
typedef enum 
{
   /*! Low VDD default   */
   PM_SMBC_VDD_DEFAULT__LOW, 
   /*! Standard VDD default   */
   PM_SMBC_VDD_DEFAULT__STANDARD
}pm_smbc_vdd_default_type;

/*! \enum pm_smbc_batt_temp_ref_gnd_type
   \brief The gound connection for battery thermistor.
 */
typedef enum 
{
   /*! Battery thermistor is inside battery pack.   */
   PM_SMBC_BATT_TEMP_REF_GND__PACK, 
   /*! Ground.   */
   PM_SMBC_BATT_TEMP_REF_GND__GND
}pm_smbc_batt_temp_ref_gnd_type;

/*! \enum pm_smbc_batt_temp_cold_thr_type
   \brief The cold battery temp threshold as fraction of VREG_THERM.
 */
typedef enum 
{
   /*! The cold battery temp threshold -- low.   */
   PM_SMBC_BATT_TEMP_COLD_THR__LOW, 
   /*! The cold battery temp threshold -- high.  */
   PM_SMBC_BATT_TEMP_COLD_THR__HIGH
}pm_smbc_batt_temp_cold_thr_type;

/*! \enum pm_smbc_batt_temp_hot_thr_type
   \brief The hot battery temp threshold as fraction of VREG_THERM.
 */
typedef enum 
{
   /*! The hot battery temp threshold -- low.   */
   PM_SMBC_BATT_TEMP_HOT_THR__LOW, 
   /*! The hot battery temp threshold -- high.  */
   PM_SMBC_BATT_TEMP_HOT_THR__HIGH
}pm_smbc_batt_temp_hot_thr_type;

/*! \enum pm_smbc_chg_cmd_type
   \brief The charge command
 */
typedef enum
{
   /*! Boot Failed  */
   PM_SMBC_MODE__BOOT_FAIL,
    /*! Boot complete  */
   PM_SMBC_MODE__BOOT_DONE,
   /*! Battery temperature sensing oFF.   */
   PM_SMBC_MODE__BATT_TEMP_SENSING_OFF,
    /*! Battery temperature sensing ON     */
   PM_SMBC_MODE__BATT_TEMP_SENSING_ON,
   /*! Low gain for battery current monitor output to XOADC     */
   PM_SMBC_MODE__IMON_GAIN_LOW,
   /*! High gain for battery current monitor output to XOADC     */
   PM_SMBC_MODE__IMON_GAIN_HIGH,
   /*! Charge OFF.   */
   PM_SMBC_MODE__CHARGE_OFF,
    /*! Charge ON     */
   PM_SMBC_MODE__CHARGE_ON,
    /*! Charge pause  */
   PM_SMBC_MODE__CHARGE_PAUSE,
    /*! Charge resume  */
   PM_SMBC_MODE__CHARGE_RESUME,
    /*! ATC force OFF  */
   PM_SMBC_MODE__FORCE_ATC_OFF,
    /*! ATC force ON  */
   PM_SMBC_MODE__FORCE_ATC_ON,
    /*! ATC Failed Clear ON  */
   PM_SMBC_MODE__ATC_FAILED_CLEAR_ON,
    /*! Chg Failed Clear ON  */
   PM_SMBC_MODE__CHG_FAILED_CLEAR_ON,
    /*! USB Suspended Mode OFF  */
   PM_SMBC_MODE__USB_SUSPENDED_OFF,
    /*! USB Suspended Mode ON  */
   PM_SMBC_MODE__USB_SUSPENDED_ON
}pm_smbc_mode_type;

/*! \enum pm_smbc_led_source_type
   \brief The power source for anode of change indicatior LED.
 */
typedef enum 
{
   /*! Ground       */
   PM_SMBC_LED_SRC__GROUND,
   /*! VPH_PWR       */
   PM_SMBC_LED_SRC__VPH_PWR,
   /*!Boost       */
   PM_SMBC_LED_SRC__BOOST,
   /*! Internal source       */
   PM_SMBC_LED_SRC__INTERNAL
}pm_smbc_led_source_type;

/*! \enum pm_smbc_attached_chgr_type
   \brief The attached charger type.
 */
typedef enum 
{
   /*! no charger.                  */
   PM_SMBC_ATTACHED_CHGR__NONE, 
   /*! USB path charger attached.   */
   PM_SMBC_ATTACHED_CHGR__USB,
   /*! DCIN path charger attached.  */
   PM_SMBC_ATTACHED_CHGR__DCIN,
   /*! Both USB and DCIN attached.  */
   PM_SMBC_ATTACHED_CHGR__BOTH
}pm_smbc_attached_chgr_type;

/*! \enum pm_smbc_chg_path_type
   \brief The charge path.
 */
typedef enum 
{
   /*! USB .                       */
   PM_SMBC_CHGR_PATH__USB,
   /*! DCIN.                       */
   PM_SMBC_CHGR_PATH__DCIN
}pm_smbc_chg_path_type;

/*! \enum pm_smbc_fsm_state_name_type
   \brief The charging state names in the FSM.
 */
typedef enum 
{
   /*! 0 - FSM entry point.                                            */
   PM_SMBC_FSM_ST__OFF,
   /*! 1 - power on from charger with high current.                    */
   PM_SMBC_FSM_ST__PWR_ON_CHGR,
   /*! 2 - ATC with low charging current.                              */
   PM_SMBC_FSM_ST__ATC_LOW_I,
   /*! 3 - power on from the battery.                                  */
   PM_SMBC_FSM_ST__PWR_ON_BATT,
   /*! 4 - ATC fail.                                                   */
   PM_SMBC_FSM_ST__ATC_FAIL = 4,
   /*! 6 - power on from the charger and the battery.                  */
   PM_SMBC_FSM_ST__PWR_ON_CHGR_AND_BATT = 6,
   /*! 7 - fast charge.                                                */
   PM_SMBC_FSM_ST__FAST_CHG,
   /*! 8 - trickle charge.                                             */
   PM_SMBC_FSM_ST__TRICKLE_CHG,
   /*! 9 - charge fail.                                                */
   PM_SMBC_FSM_ST__CHG_FAIL,
   /*! 10 - end-of-charge.                                             */
   PM_SMBC_FSM_ST__EOC = 10,
   /*! 13 - ATC pause.                                                 */
   PM_SMBC_FSM_ST__ATC_PAUSE = 13,
   /*! 14 - fast charge pause.                                         */
   PM_SMBC_FSM_ST__FAST_CHG_PAUSE,
   /*! 15 - trickle charge pause.                                      */
   PM_SMBC_FSM_ST__TRICKLE_CHG_PAUSE = 15,
   /*! 18 - ATC with high charging current.                            */
   PM_SMBC_FSM_ST__ATC_HIGH_I = 18,
   /*! 22 - FLCB.                                                      */
   PM_SMBC_FSM_ST__FLCB = 22
}pm_smbc_fsm_state_name_type;

/*===========================================================================

                 SMBC DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @brief This function sets the charge mode.
 * 
 * @details
 *  This function sets the charge mode, such as enabling charging, disable
 *  charging, pause charging, resume charging, etc.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] mode This parameter is used to set certain mode in the SMBC module. 
 *       -- PM_SMBC_MODE__BOOT_FAIL  This mode is used to set PMIC in "BOOT FAIL"
 *          mode; in this mode, the PMIC hardware charger state machine will turn 
 *          off VPH_PWR and go to ATC upon the hardware boot timer expires;
 *       -- PM_SMBC_MODE__BOOT_DONE  This mode is used to set PMIC in "BOOT DONE"
 *          mode; in this mode, PMIC stops the boot timer and allows the system to
 *          fully boot up; 
 *       -- PM_SMBC_MODE__BATT_TEMP_SENSING_OFF This mode is used to disable the 
 *          battery temperature sensing circuit in SMBC; in this mode, the SMBC
 *          hardware state machine will not detect the battery-temp out-of-range
 *          condition and charging will continue even if the battery temp is 
 *          actually out-of-range.
 *       -- PM_SMBC_MODE__BATT_TEMP_SENSING_ON This mode is used to enable the 
 *          battery temperature sensing circuit in SMBC module; in this mode, 
 *          the SMBC hardware state machine will monitor the battery temperature 
 *          during charging; if it is out-of-range, charging will be stopped 
 *          until the temperature comes back to normal range.
 *       -- PM_SMBC_MODE__CHARGE_OFF  This mode is used to set SMBC hardware state
 *          machine in "charge off" state.
 *       -- PM_SMBC_MODE__CHARGE_ON This mode is used to set SMBC hardware state
 *          machine in "charge on" state. 
 *       -- PM_SMBC_MODE__CHARGE_PAUSE This mode is used to set SMBC hardware state
 *          machine in "charge pause" state.If charging is paused from a charging 
 *          state, the SMBC hardware state machine will move to the corresponding 
 *          paused charging state. If not FROM a charging state, it will not enter
 *          enter a charging state until it is put in " Charge resume". Battery FET
 *          and buck enable will respond as if battery temperature is out of range.
 *          This mode should only be set when the SMBC hardware state machine was
 *          previously set to "Charge on".
 *       -- PM_SMBC_MODE__CHARGE_RESUME This mode is used to set SMBC hardware 
 *          state machine in "charge resume" state. This mode should only be set  
 *          when the SMBC hardware state machine was previously set to "Charge 
 *          pause".
 *       -- PM_SMBC_MODE__FORCE_ATC_OFF This mode is used to clear the  force start 
 *          of "Auto Trickle Charge" state.
 *       -- PM_SMBC_MODE__FORCE_ATC_ON This mode is used to set SMBC hardware state
 *          machine to force start of "Auto Trickle Charge (ATC)" state and override 
 *          "Fast Low Current Boot(FLCB)" state. 
 *       -- PM_SMBC_MODE__ATC_FAILED_CLEAR_ON This mode is used to set SMBC 
 *          hardware machine out of the "Auto Trickle Charge (ATC) failed" state
 *       -- PM_SMBC_MODE__CHG_FAILED_CLEAR_ON This mode is used to set SMBC 
 *          hardware machine out of the "Charge failed" state
 *       -- PM_SMBC_MODE__USB_SUSPENDED_ON This mode is used to set SMBC hardware
 *          state machine in the "USB Suspended" state.
 *       -- PM_SMBC_MODE__USB_SUSPENDED_OFF  This mode is used to clear the 
 *          "USB Suspended" state. 
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE  = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS            = SUCCESS.
 *
 * @note The default setting in PMIC HW is PM_SMBC_MODE__CHARGE_OFF. 
 *
 */
pm_err_flag_type pm_smbc_set_mode(int externalResourceIndex, 
                                  pm_smbc_mode_type mode);

/**
 * @brief This function allows/inhibits the SMBC module to send power-on-trigger
 *  to the PON module wrt the charge path.
 * 
 * @details
 *  This function allows/inhibits the SMBC module to send power-on-trigger 
 *  charger valid) to the Power On (PON) module when a charge device is  
 *  plugged into the phone.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] enable This parameter is used to enable/disable the power on trigger 
 *   -- PM_SMBC_CMD__ENABLE  Allows the SMBC module to send the
 *                           power-on-trigger to the Power on module;
 *   -- PM_SMBC_CMD__DISABLE Inhibits the SMBC module to send the
 *                           power-on-trigger to the Power on module; 
 *        
 * @param[in] path This parameter is used to specify which charge path to 
 *            enable/disable the power on trigger.
 *  -- PM_SMBC_CHGR_PATH__USB  The USB charge path;
 *  -- PM_SMBC_CHGR_PATH__DCIN The DC charger path; 
 *        
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__PAR3_OUT_OF_RANGE = Input Parameter three is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.
 *
 */
pm_err_flag_type pm_smbc_set_pwron_trig(int externalResourceIndex, 
                                        pm_smbc_cmd_type enable,
                                        pm_smbc_chg_path_type path);

/**
 * @brief This function gets the availability of the BATFET.
 * 
 * @details
 *  This function checks if the BATFET is present or not.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] present The statu of BATFET.  
 *        
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 */
pm_err_flag_type pm_smbc_get_batfet_present(int externalResourceIndex, 
                                            boolean *present);

/**
 * @brief This function sets the default VDD value.
 * 
 * @details
 *  This function sets the default VDD(power supply) value.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] vdd The default VDD in type of pm_smbc_vdd_default_type
 *           -- PM_SMBC_VDD_DEFAULT__LOW  Lower than standard default VDD;
 *           -- PM_SMBC_VDD_DEFAULT__STANDARD The standard default VDD; 
 *        
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.
 */
pm_err_flag_type pm_smbc_set_default_vdd(int externalResourceIndex, 
                                         pm_smbc_vdd_default_type vdd);

/**
 * @brief This function sets the battery temperature configurations.
 * 
 * @details
 *  This function sets the battery temp configurations, such as the ground 
 *  connection  for the battery thermistor, the cold battery temp threshold,
 *   the hod battery temp threshold, etc.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 * @param[in] ref_gnd The ground connection for battery thermistor. 
 * @param[in] cold_thr The cold battery temp threshold. 
 * @param[in] hot_thr The hot battery temp threshold. 
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__PAR3_OUT_OF_RANGE = Input Parameter three is out of range.
 *         PM_ERR_FLAG__PAR4_OUT_OF_RANGE = Input Parameter four is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default settings are device specific.
 */
pm_err_flag_type pm_smbc_set_batt_temp_config(int externalResourceIndex, 
                                      pm_smbc_batt_temp_ref_gnd_type  ref_gnd,
                                      pm_smbc_batt_temp_cold_thr_type cold_thr,
                                      pm_smbc_batt_temp_hot_thr_type  hot_thr);

/**
 * @brief This function gets the ground connection for battery thermistor.
 * 
 * @details
 *  This function gets the ground connection for battery thermistor.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] source The power source. 
 *  -- PM_SMBC_BATT_TEMP_REF_GND__PACK  The battery thermistor is inside 
 *                                      battery pack;
 *  -- PM_SMBC_BATT_TEMP_REF_GND__GND The battery thermistor is not in 
 *                                    battery pack; 
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 * @note The default setting in PMIC HW is device specific.
 *
 */
pm_err_flag_type pm_smbc_get_batt_therm_gnd(int externalResourceIndex, 
                                     pm_smbc_batt_temp_ref_gnd_type*  ref_gnd);
/**
 * @brief This function selects power source for anode of charge indicator LED.
 * 
 * @details
 *  This function selects power source for anode of charge indicator LED.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] source The power source. 
 *   -- PM_SMBC_LED_SRC__GROUND  The LED gets no power;
 *   -- PM_SMBC_LED_SRC__VPH_PWR The LED gets power from the main phone voltage 
 *      rail(VPH_PWR); 
 *   -- PM_SMBC_LED_SRC__BOOST  The LED gets power from certain boost regulator;
 *   -- PM_SMBC_LED_SRC__INTERNAL The LED gets power from the SMBC module
 *      internally; 
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 * @note The default setting in PMIC HW is device specific.
 *
 */
pm_err_flag_type pm_smbc_set_led_source(int externalResourceIndex, 
                                        pm_smbc_led_source_type source);

/**
 * @brief This function checks if the battery is weak or not.
 * 
 * @details
 *  This function checks if the battery is weak or not by checking the battery 
 *  voltage againest the weak battery threshold; this threshold is set by
 *  pm_smbc_set_weak_vbatt_threshold()
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] weak The battery-weak status
 *                 -- TRUE: The battery is weak(below the threshold)
 *                 -- FALSE: The battery is normal (above the threshold)
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 *
 */
pm_err_flag_type pm_smbc_get_battery_weak(int externalResourceIndex, 
                                          boolean *weak);

/**
 * @brief This function gets the attached charger type.
 * 
 * @details
 *  This function gets the attached charger type.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] type The attached charger type.
 *   -- PM_SMBC_ATTACHED_CHGR__NONE: no charger attached
 *   -- PM_SMBC_ATTACHED_CHGR__USB:  USB device attached
 *   -- PM_SMBC_ATTACHED_CHGR__DCIN: DC wall charger attached
 *   -- PM_SMBC_ATTACHED_CHGR__BOTH: bot USB device and DC wall charger attached
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 *
 */
pm_err_flag_type pm_smbc_get_attached_charger(int externalResourceIndex, 
                                              pm_smbc_attached_chgr_type *type);

/**
 * @brief This function checks if the battery is present.
 * 
 * @details
 *  This function checks if the battery is present.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] present Status of battery presence.
 *            -- TRUE: battery is present
 *            -- FALSE:  battery is not present
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 *
 */
pm_err_flag_type pm_smbc_get_battery_present(int externalResourceIndex, 
                                             boolean *present);

/**
 * @brief This function sets maximum USB_IN current.
 * 
 * @details
 *  This function sets maximum USB_IN current. USB_IN current is the current 
 *  going through the USB cable.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_ma The current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the 
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that  
 *       there was an out-of-bounds mA value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.
 *
 */
pm_err_flag_type pm_smbc_set_iusb_max(int externalResourceIndex, 
                                      uint32 value_ma);

/**
 * @brief This function sets maximum VDD.
 * 
 * @details
 *  This function sets maximum VDD. VDD is the power supply to the phone.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_mv The VDD value in mV.
 * @note if the value of parameter value_mv is out of the valid range that 
 *       the PMIC HW can take, the value will be limited to the boundary 
 *       and the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that  
 *       there was an out-of-bounds mV value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is determined by device specific. 
 *
 */
pm_err_flag_type pm_smbc_set_vdd_max(int externalResourceIndex, 
                                     uint32 value_mv);

/**
 * @brief This function sets VBATDET.
 * 
 * @details
 *  This function sets VBATDET (BATtery Voltage DETect). VBATDET comparator is 
 *  used to restart charging when battery voltage is less than VBATDET value;
 *  When the battery voltage is greater than the VBATDET threshold, charging 
 *  stops.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_mv The VBATDET value in mV.
 * @note if the value of parameter value_mv is out of the valid range that the 
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds mV value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_smbc_set_vbatdet(int externalResourceIndex, 
                                     uint32 value_mv);

/**
 * @brief This function sets maximum fast charge battery current limit.
 * 
 * @details
 *  This function sets maximum fast charge battery current limit.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_ma The maximum IBAT value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the 
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that  
 *       there was an out-of-bounds mA value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_ibat_max(int externalResourceIndex, 
                                      uint32 value_ma);

/**
 * @brief This function sets minimum charger input voltage.
 * 
 * @details
 *  This function sets minimum charger input voltage.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_mv The minimum input voltage regulation value in mV.
 * @note if the value of parameter value_mv is out of the valid range that the  
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that  
 *       there was an out-of-bounds mV value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on 
 *         this version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED  = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_smbc_set_vin_min(int externalResourceIndex, 
                                     uint32 value_mv);

/**
 * @brief This function gets the battery current sense resistor value.
 * 
 * @details
 *  This function gets the battery current sense resistor value.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] value_mOhm The battery current sense resistor value in mOHM.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_smbc_get_batt_sense_r(int externalResourceIndex, 
                                          uint32* value_mOhm);

/**
 * @brief This function sets the system weak battery threshold.
 * 
 * @details
 *  This function sets the system weak battery threshold. This is the battery
 *  threshold that high current trickle charging starts.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_mv The system weak battery threshold value in mV.
 * @note if the value of parameter value_mv is out of the valid range that the  
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds mV value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_weak_vbatt_threshold(int externalResourceIndex, 
                                                  uint32 value_mv);

/**
 * @brief This function sets the low battery threshold. 
 * 
 * @details
 *  This function sets the system low battery threshold. This is the battery  
 *  threshold that low current automatic trickle charging (ATC) starts.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_mv The system low battery threshold value in mV.
 * @note if the value of parameter value_mv is out of the valid range that  
 *       the PMIC HW can take, the value will be limited to the boundary and 
 *       the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds mV value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_low_vbatt_threshold(int externalResourceIndex, 
                                                 uint32 value_mv);

/**
 * @brief This function sets the system trickle charging current.
 * 
 * @details
 *  This function sets the system trickle charging current.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_ma The system trickle charging current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that 
 *       the PMIC HW can take,  the value will be limited to the boundary and
 *       the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds mA value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_itrkl(int externalResourceIndex, 
                                   uint32 value_ma);

/**
 * @brief This function sets system end-of-charge current.
 * 
 * @details
 *  This function sets system end-of-charge (EOC) current. This is the current
*   used to generate the EOC interrupt and terminate charging.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_ma The system end-of-charge current value in mA.
 * @note if the value of parameter value_ma is out of the valid range that the 
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that  
 *       there was an out-of-bounds mA value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS              = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_iterm(int externalResourceIndex, uint32 value_ma);

/**
 * @brief This function sets maximum time for trickle charging.
 * 
 * @details
 *  This function sets maximum time for initiated trickle charging, including 
 *  software initiated trickle chargign as well as hardware controlled 
 *  auto-trickle_charging(ATC).
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_minutes The maximum time value in minutes for trickle 
 *            charging.
 * @note if the value of parameter value_minutes is out of the valid range that  
 *       the PMIC HW can take, the value will be limited to the boundary and 
 *       the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds minute value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_ttrkl(int externalResourceIndex, 
                                   uint32 value_minutes);

/**
 * @brief This function sets maximum time for software initiated charging.
 * 
 * @details
 *  This function sets maximum time for software initiated charging, including  
 *  trickle and fast phases.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] value_minutes The maximum time value in minutes for SW initiated
*             charging.
 * @note if the value of parameter value_minutes is out of the valid range that  
 *       the PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds minute value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific.  
 *
 */
pm_err_flag_type pm_smbc_set_tchg(int externalResourceIndex, 
                                  uint32 value_minutes);

/**
 * @brief This function sets the delay and the timeout for the hardware WDOG 
 *  timer in the PMIC SMBC module.
 * 
 * @details
 *  This function sets the delay between the hardware WDOG IRQ and stop charging 
 *  and the timeout for the  hardware WDOG timer in the PMIC SMBC module.
 *  The software initiated charging will stop when this hardware timer expires.  
 *  Use this API to set the timeout in order to restart the hardware WDOG timer 
 *  (pet the dog). This timer can be used along with a software periodic timer 
 *  (for example, heartbeat) to ensure that charging will stop in time when the 
 *  software crashes. The timeout of the software periodic timer should be set 
 *  a little shorter than the timeout of the hardware timer in orde to be able 
 *  to restart the hardware WDOG timer before it times out.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] delay_seconds The delay in seconds between WDOG IRQ and stop 
 *            charging.
 * @note if the value of parameter delay_seconds is out of the valid range that 
 *       the PMIC HW can take, the value will be limited to the boundary and 
 *       the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds second value parameter input.
 *     
 * @param[in] timeout_seconds The WDOG timer timeout value in seconds.
 * @note if the value of parameter delay_seconds is out of the valid range that  
 *       the PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR3_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds second value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE  = Input Parameter two is out of range.
 *         PM_ERR_FLAG__PAR3_OUT_OF_RANGE  = Input Parameter three is out of range.
 *         PM_ERR_FLAG__SUCCESS            = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_smbc_set_twdog(int externalResourceIndex, 
                                   uint32 delay_seconds, 
                                   uint32 timeout_seconds);

/**
 * @brief This function sets charging temperature thresholds.
 *
 * @details
 *  This function sets charging temperature thresholds. Charging will stop 
 *  (pass device turns off) when the charging temperature is higher than the  
 *   maximum charging temperature threshold; charging will resume 
 *  (pass device turns on) when the charging temperature has fallen below the 
 *  resume-charging temperature threshold.
 * 
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] stop_deg_c   stop-charging temperature value in Celsius C.).
 * @note if the value of parameter stop_deg_c is out of the valid range that  
 *       the PMIC HW can take, the value will be limited to the boundary and 
 *       the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds Celsius value parameter input.
 * @param[in] resume_deg_c Resume-charging temperature value in Celsius(C.).
 * @note if the value of parameter resume_deg_c is out of the valid range that 
 *       the PMIC HW can take,  the value will be limited to the boundary and 
 *       the PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR3_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds Celsius value parameter input.
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default settings are device specific. 
 */
pm_err_flag_type pm_smbc_set_temp_threshold(int externalResourceIndex, 
                                            uint32 stop_deg_c, 
                                            uint32 resume_deg_c);

/**
 * @brief This function sets the charging path.
 * 
 * @details
 *  This function sets the charging path. The charging path will either be 
 *  DC wall or USB, but not both at the same time.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] path The desired charging path.
 *               -- PM_SMBC_CHGR_PATH__USB USB device
 *               -- PM_SMBC_CHGR_PATH__DCIN DC wall charger
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 */
pm_err_flag_type pm_smbc_set_charge_path(int externalResourceIndex, 
                                         pm_smbc_chg_path_type path);

/**
 * @brief This function gets the charging path.
 * 
 * @details
 *  This function gets the charging path. This is the charging path that's  
 *  set by pm_smbc_set_charge_path().
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] path The current charging path.
 *               -- PM_SMBC_CHGR_PATH__USB USB device
 *               -- PM_SMBC_CHGR_PATH__DCIN DC wall charger
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 *
 */
pm_err_flag_type pm_smbc_get_charge_path(int externalResourceIndex, 
                                         pm_smbc_chg_path_type *path);

/**
 * @brief This function sets the FSM charge state.
 * 
 * @details
 *  This function gets the FSM charge state.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[in] state The desired FSM charge state.
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 */
pm_err_flag_type pm_smbc_set_charge_state(int externalResourceIndex, 
                                          pm_smbc_fsm_state_name_type state);

/**
 * @brief This function gets the current FSM charge state.
 * 
 * @details
 *  This function gets the current FSM charge state.
 *   
 * @param[in] externalResourceIndex The external SMBC ID.
 *
 * @param[out] state The current FSM charge state.
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the SMBC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_SMBC_INDEXED = The internal resource ID for the 
 *         SMBC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER = Input Parameter two is a NULL pointer.
 *         PM_ERR_FLAG__SUCCESS         = SUCCESS.
 *
 */
pm_err_flag_type pm_smbc_get_charge_state(int externalResourceIndex, 
                                          pm_smbc_fsm_state_name_type *state);

#endif /* PM_SMBC_H */

