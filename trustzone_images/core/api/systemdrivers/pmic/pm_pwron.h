#ifndef PM_PWRON_H

#define PM_PWRON_H
/*! \file 
 *  \n
 *  \brief  pm_pwron.h ---- PMIC-PWRON MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the power-on driver module.
 *
 *
 *  \n &copy; Copyright (c) 2010-2011    by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_pwron.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/12   umr     Add PON Reason API for Badger PMIC family.
01/12/11   tdh     Added badger enumeration types
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
02/07/11   jtn     Change deprecation of enum to fix compiler warnings
01/17/11   jtn     Deprecate pm_pwr_key_delay_type and pm_power_key_setup().
                   Use pm_pwr_key_delay_set(uint32 microseconds) and 
                   pm_err_flag_type pm_pwr_key_pullup_set(boolean enable) instead
07/19/10   cwg     Added support for PMIC stay on bit 
07/09/10   jtn     Add API to return WDOG Status 
04/23/10   jtn     Add APIs for hard reset control 
03/15/10   fpe     CMI - Merge
02/02/10   jtn     Added API pm_pwron_hard_reset_enable(boolean enable)
02/01/10   jtn     Added API pm_pwron_soft_reset()
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
06/15/09   jtn     New file for SCMM
06/20/09   jtn     New file
===========================================================================*/

#ifdef __RVCT__
#define PM_PWRON_DEPRECATED __attribute__((__deprecated__))
#else
#define PM_PWRON_DEPRECATED 
#endif

#include "pm_lib_err.h"
#include "comdef.h"

/*============== POWER ON CONTROL TYPE DEFINITIONS ====================*/
typedef enum{
    PM_POWER_RESET_SIGNAL__INVALID
}pm_power_reset_signal;

typedef enum
{
    PM_PWR_ON_RST_ACTION_TYPE__COMPLETE_FULL_SHUTDOWN,
    PM_PWR_ON_RST_ACTION_TYPE__FULL_SHUTDOWN,
    PM_PWR_ON_RST_ACTION_TYPE__HARD_RESET,
    PM_PWR_ON_RST_ACTION_TYPE__POWER_OFF_NORMAL,
    PM_PWR_ON_RST_ACTION_TYPE__SOFT_RESET,
    PM_PWR_ON_RST_ACTION_TYPE__WARM_RESET,
    PM_PWR_ON_RST_ACTION_TYPE__INVALID  
}pm_power_reset_action_type;


/* Power Key hysteresis settings */
typedef enum
{
  PM_PWR_KEY_DELAY_EQ_0_msecs,         /*  0.00  ms */
  PM_PWR_KEY_DELAY_EQ_0p97_msecs,      /*  0.97  ms */
  PM_PWR_KEY_DELAY_EQ_1p95_msecs,      /*  1.95  ms */
  PM_PWR_KEY_DELAY_EQ_3p90_msecs,      /*  3.90  ms */
  PM_PWR_KEY_DELAY_EQ_7p81_msecs,      /*  7.81  ms */
  PM_PWR_KEY_DELAY_EQ_15p62_msecs,     /* 15.62  ms */
  PM_PWR_KEY_DELAY_EQ_31p25_msecs,     /* 31.25  ms */
  PM_PWR_KEY_DELAY_EQ_62p50_msecs,     /* 62.50  ms */
  PM_NUM_OF_PWR_KEY_DELAY              /* Number of power key hysteresis
                                          settings */
}pm_pwr_key_delay_type;  

/* Event definition for PMIC power on status */
#define  PM_PWR_ON_EVENT_KEYPAD     0x1
#define  PM_PWR_ON_EVENT_RTC        0x2
#define  PM_PWR_ON_EVENT_CABLE      0x4
#define  PM_PWR_ON_EVENT_SMPL       0x8
#define  PM_PWR_ON_EVENT_WDOG       0x10
#define  PM_PWR_ON_EVENT_USB_CHG    0x20
#define  PM_PWR_ON_EVENT_WALL_CHG   0x40

/*======================= Cable power On definitions ======================*/
typedef enum
{
  PM_CBL_PWR_ON__ENABLE,
  PM_CBL_PWR_ON__DISABLE,
  PM_CBL_PWR_ON__DIS_PERMANENTLY,
  PM_CBL_PWR_ON__INVALID
}pm_cbl_pwr_on_switch_type;

typedef enum
{
  PM_CBL_PWR_ON_PIN__0,
  PM_CBL_PWR_ON_PIN__1,
  PM_CBL_PWR_ON_PIN__INVALID
}pm_cbl_pwr_on_pin_type;

/*======================= End of Cable power on ===========================*/

/*===========================================================================
                 POWER ON CONTROL FUNCTION PROTOTYPES
===========================================================================*/

/**
 * @brief Turn the PMIC abort timer switch on and off 
 *  
 * @details Allow the user to turn the abort timer On and off
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param OnOff The On or Off command for the timer
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_start_up_abort_timer_switch(unsigned pmic_device_index, pm_switch_cmd_type OnOff);

/**
 * @brief Configure the PMIC power key delay setting 
 *  
 * @details Configures the time delay for KPD and CBL PON state
 *          change interrupt and triggering.
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param microseconds The PMIC power key delay setting.  The 
 *                     API will configure the PMIC to the
 *                     nearest value <= to the requested value.
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_pwr_key_delay_set(unsigned pmic_device_index, uint32 microseconds);
//#pragma message("Do not use pm_pwr_key_delay_set(), which is deprecated. Use pm_dev_pwr_key_delay_set() instead.")
pm_err_flag_type pm_pwr_key_delay_set(uint32 microseconds);

/**
 * @brief Enable or disable the PMIC power key pullup resistor
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param pull_up_en Boolean to enable/disable the pullup
 *                   
 *                   TRUE enables the pullup
 *                   FALSE disables the pullup
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_pwr_key_pullup_set(unsigned pmic_device_index, pm_switch_cmd_type pull_up_en);
//#pragma message("Do not use pm_pwr_key_pullup_set(), which is deprecated. Use pm_dev_pwr_key_pullup_set() instead.")
pm_err_flag_type pm_pwr_key_pullup_set(pm_switch_cmd_type pull_up_en);
/**
 * Return the PMIC watchdog reset status
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param dog_status Pointer to the watchdog reset status
 *                   
 *                   TRUE means that a watchdog reset has occurred.
 *                   FALSE means that no watchdog reset has occurred.
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_wdog_status_get(unsigned pmic_device_index, boolean *dog_status);
//#pragma message("Do not use pm_wdog_status_get(), which is deprecated. Use pm_dev_wdog_status_get() instead.")
pm_err_flag_type pm_wdog_status_get(boolean *dog_status);
/*~ FUNCTION  pm_wdog_status_get */
/*~ PARAM OUT dog_status POINTER */

/*===========================================================================

FUNCTION pm_dev_get_power_on_reason                   EXTERNAL FUNCTION

DESCRIPTION
    This function returns the phone power-on reason. Typically used in boot
    during early bootup and stored in memory for later access.

INPUT PARAMETERS

* @param pmic_device_index When the target has more than
*          one pmic, this is the index of the PMIC in which
*          the power on module is physically located. The device
*          index starts at zero.
  1) Parameter name: *pwr_on_reason
     - pointer to 64-bit unsigned integer that stores the all PON reasons
    including PON power on, Warm Reset Reason and POFF_REASON,
    SOFT_RESET_REASON.

      PON_REASON
      PON_WARM_RESET_REASON
      PON_POFF_REASON
      PON_SOFT_RESET_REASON


RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  pm_init() must have been called.

SIDE EFFECTS
  

===========================================================================*/
pm_err_flag_type pm_dev_get_power_on_reason(unsigned pmic_device_index, uint64* pwr_on_reason);

/*===========================================================================

FUNCTION pm_get_power_on_status                   EXTERNAL FUNCTION

DESCRIPTION
    This function returns the phone power-on status, and it should be called
at the earliest possible moment at boot up

INPUT PARAMETERS

* @param pmic_device_index When the target has more than
*          one pmic, this is the index of the PMIC in which
*          the power on module is physically located. The device
*          index starts at zero.
  1) Parameter name: *pwr_on_status
     - pointer to 32-bit unsigned integer that stores the power on status
    including keypad power on, RTC alarm trigger, cable power on, SMPL, 
    watchdog timeout, wall charger and USB charger with definitions as the 
    following: 

        PM_PWR_ON_EVENT_KEYPAD     0x1
        PM_PWR_ON_EVENT_RTC        0x2
        PM_PWR_ON_EVENT_CABLE      0x4
        PM_PWR_ON_EVENT_SMPL       0x8
        PM_PWR_ON_EVENT_WDOG       0x10
        PM_PWR_ON_EVENT_USB_CHG    0x20
        PM_PWR_ON_EVENT_WALL_CHG   0x40


RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_dev_get_power_on_status(unsigned pmic_device_index, uint32* pwr_on_status);
//#pragma message("Do not use pm_get_power_on_status(), which is deprecated. Use pm_dev_get_power_on_status() instead.")
pm_err_flag_type pm_get_power_on_status(uint32* pwr_on_status);

/*===========================================================================

FUNCTION pm_clear_power_on_status                   EXTERNAL FUNCTION

DESCRIPTION
    This function clears the phone power-on status, and it should be called
before the phone powers down.

INPUT PARAMETERS: 

* @param pmic_device_index When the target has more than
*          one pmic, this is the index of the PMIC in which
*          the power on module is physically located. The device
*          index starts at zero.

RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_dev_clear_power_on_status(unsigned pmic_device_index);
//#pragma message("Do not use pm_clear_power_on_status(), which is deprecated. Use pm_dev_clear_power_on_status() instead.")
pm_err_flag_type pm_clear_power_on_status(void);

/*===========================================================================
FUNCTION pm_watchdog_reset_detect_switch            EXTERNAL FUNCTION  
DESCRIPTION
    This function enables/disables the PMIC's watchdog reset detect 
  feature. If this feature is enabled then the PMIC resets and restarts
  if PS_HOLD pin goes low. It will also trigger an IRQ if the Watchdog 
  Timeout IRQ (PM_WDOG_TOUT_IRQ_HDL) is enabled. If this feature is 
  disabled then the PMIC shuts off if PS_HOLD pin goes low. The IRQ will 
  not trigger even if enabled.
  EXAMPLE:
  // Enable/Disable the watchdog reset detect feature. 
  err |= pm_watchdog_reset_detect_switch(PM_ON_CMD);
  // Find out if there was an PMIC API error. 
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
      MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
  }
INPUT PARAMETERS

* @param pmic_device_index When the target has more than
*          one pmic, this is the index of the PMIC in which
*          the power on module is physically located. The device
*          index starts at zero.
  Parameter name: OnOff.
  - Enable/disable wathdog reset detect feature.
  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Enable.
        PM_OFF_CMD = Disable (default).
RETURN VALUE
  Return value type: pm_err_flag_type.
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_dev_watchdog_reset_detect_switch(unsigned pmic_device_index, pm_switch_cmd_type OnOff);
//#pragma message("Do not use pm_watchdog_reset_detect_switch(), which is deprecated. Use pm_dev_watchdog_reset_detect_switch() instead.")
pm_err_flag_type pm_watchdog_reset_detect_switch(pm_switch_cmd_type OnOff);

/*===========================================================================

                 CABLE POWER ON DRIVER FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_cbl_pwr_on_switch                       EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to enable, disable and permanently disable
  cable power ON.
  
  EXAMPLE:
     
    // Blow the fuse so that cable power ON is always disable. Even after
    // power is removed. 
    err = pm_cbl_pwr_on_switch(PM_CBL_PWR_ON__DIS_PERMANENTLY);
    
INPUT PARAMETERS  

* @param pmic_device_index When the target has more than
*          one pmic, this is the index of the PMIC in which
*          the power on module is physically located. The device
*          index starts at zero.
  1) Parameter name: cmd
     - Enable, Disable or Blow fuse (to disable for ever) cable power on.  

     Parameter type: pm_cbl_pwr_on_switch_type
     - Valid inputs:
            PM_CBL_PWR_ON__ENABLE (default)= Enable Cable Power ON.
            PM_CBL_PWR_ON__DISABLE         = Disable Cable Power ON.
            PM_CBL_PWR_ON__DIS_PERMANENTLY = Blow fuse. Calling this function 
                                             once will disable cable power
                                             on for ever. This option is
                                             only available on PM6650
                                             Rev G and higher.
            

RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  1) Interrupts are disabled while communicating with PMIC.
  2) The microprocessor will be halted for 1 msec while blowing the fuse.
     Option: PM_CBL_PWR_ON__DIS_PERMANENTLY.

===========================================================================*/
pm_err_flag_type pm_dev_cbl_pwr_on_switch(unsigned pmic_device_index, pm_cbl_pwr_on_switch_type cmd);
//#pragma message("Do not use pm_cbl_pwr_on_switch(), which is deprecated. Use pm_dev_cbl_pwr_on_switch() instead.")
pm_err_flag_type pm_cbl_pwr_on_switch(pm_cbl_pwr_on_switch_type cmd);

/*===========================================================================

FUNCTION pm_cbl_pwr_on_pull_up_switch                     EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to enable and disable CBL0PWR_N and 
    CBL1PWR_N pin pull up.  The pull up will is only effective when
    cable power on is enabled.  
  
  EXAMPLE:
     
    // Enable cable power on and disable CBL1PWR_N pin pull up. 
    err = pm_cbl_pwr_on_pull_up_switch(PM_ON_CMD, PM_CBL_PWR_ON__1);
    
    if (err != PM_ERR_FLAG__SUCCESS)
    {
        MSG_ERROR("PMIC API ERROR %d",err,0,0);
    }

INPUT PARAMETERS  

* @param pmic_device_index When the target has more than
*          one pmic, this is the index of the PMIC in which
*          the power on module is physically located. The device
*          index starts at zero.
  1) Parameter name: OnOff.
     - Enable/Disable Cable Power On Pull Up.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD
            PM_ON_CMD

  2) Parameter name: cable.
     - For which Cable Power On pin should we enable/disable the pull up. 

     Parameter type: pm_cbl_pwr_on_type (enum).
     - Valid Inputs:
        For PM7500 and PM6650:
            PM_CBL_PWR_ON__0 = Cable Power On pin 0.
            PM_CBL_PWR_ON__1 = Cable Power On pin 1.
        For Panoramix:
            PM_CBL_PWR_ON_PIN__0 = Cable Power On pin 0.
        For all others:
            Cable power on feature is not supported.
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_dev_cbl_pwr_on_pull_up_switch(
                                  unsigned pmic_device_index, 
                                  pm_switch_cmd_type      OnOff,
                                  pm_cbl_pwr_on_pin_type  cable);
//#pragma message("Do not use pm_cbl_pwr_on_pull_up_switch(), which is deprecated. Use pm_dev_cbl_pwr_on_pull_up_switch() instead.")
pm_err_flag_type pm_cbl_pwr_on_pull_up_switch(
    pm_switch_cmd_type      OnOff,
    pm_cbl_pwr_on_pin_type  cable);

/**
 *  @brief Issue a hard reset to the PMIC
 *  @detailed This API resets the PMIC to its default state.
 * This will reset all SSBI registers (except for some RTC
 * registers and trim registers) to default values. Note that the 
 * realtime clock (RTC) time and alarm values are stored 
 * internally, are not SSBI registers and are not reset.
 *
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * 
 * @return pm_err_flag  Returns PM_ERR_FLAG__SUCCESS is successful.
 */
pm_err_flag_type pm_dev_pwron_soft_reset(unsigned pmic_device_index);
//#pragma message("Do not use pm_pwron_soft_reset(), which is deprecated. Use pm_dev_pwron_soft_reset() instead.")
pm_err_flag_type pm_pwron_soft_reset(void);


/**
 * 
 * 
 * @brief Enable the PMIC hardware reset
 *
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param enable FALSE: PMIC remains powered down following reset signal. Require another
 *               power-on trigger to start PON sequence.
 *               TRUE: PMIC will power-on again following reset
 * 
 * @return pm_err_flag_type  PM_ERR_FLAG__SUCCESS is successful
 */
pm_err_flag_type pm_dev_pwron_hard_reset_enable(unsigned pmic_device_index, boolean enable);
//#pragma message("Do not use pm_pwron_hard_reset_enable(), which is deprecated. Use pm_dev_pwron_hard_reset_enable() instead.")
pm_err_flag_type pm_pwron_hard_reset_enable(boolean enable);

/**
 * 
 * @brief  API to set the PMIC hard reset delay timer
 *
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param delay_timer_ms The value for the hard reset delay 
 *                       timer.  The allowable range for this
 *                       parameter is 0 to 2000 milliseconds,
 *                       although the PMIC will be set to one of
 *                       the following discrete values: 0, 10,
 *                       50, 100, 250, 500, 1000, 2000.  The
 *                       PMIC driver will set the delay timer to
 *                       the discrete value equal to or higher
 *                       than the passed value.  If the caller
 *                       requested 150 for example, the value
 *                       would be set to 250.
 * 
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
 */
pm_err_flag_type pm_dev_pwron_hard_reset_delay_timer_set(unsigned pmic_device_index, int delay_timer_ms);
//#pragma message("Do not use pm_pwron_hard_reset_delay_timer_set(), which is deprecated. Use pm_dev_pwron_hard_reset_delay_timer_set() instead.")
pm_err_flag_type pm_pwron_hard_reset_delay_timer_set(int delay_timer_ms);

/**
 * 
 * @brief API to set the PMIC hard reset debounce timer
 *
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param debounce_timer_ms The hard reset debounce timer in 
 *                          milliseconds.  This parameter can
 *                          range from 0 to 10256 mS, although
 *                          the PMIC will be set to one of the
 *                          following discrete values: 0, 32,
 *                          56, 80, 128, 184, 272, 408, 608,
 *                          904, 1352, 2048, 3072, 4480, 6720,
 *                          10256.  The PMIC driver will set the
 *                          debounce timer to the discrete value
 *                          equal to or higher than the passed
 *                          value.  If the caller requested 130
 *                          for example, the value would be set
 *                          to 184.
 *                       
 *  
 * 
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
 */
pm_err_flag_type pm_dev_pwron_hard_reset_debounce_timer_set(unsigned pmic_device_index, int debounce_timer_ms);
//#pragma message("Do not use pm_pwron_hard_reset_debounce_timer_set(), which is deprecated. Use pm_dev_pwron_hard_reset_debounce_timer_set() instead.")
pm_err_flag_type pm_pwron_hard_reset_debounce_timer_set(int debounce_timer_ms);

/**
 * 
 * @brief Enable the PMIC stay-on bit
 *
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param enable FALSE: Will clear PMIC stay-on bit 
 *              TRUE: Will set PMIC stay-on bit
 * 
 * @return pm_err_flag_type  PM_ERR_FLAG__SUCCESS is successful
 */
pm_err_flag_type pm_dev_pwron_stay_on_enable(unsigned pmic_device_index, boolean enable);
//#pragma message("Do not use pm_pwron_stay_on_enable(), which is deprecated. Use pm_dev_pwron_stay_on_enable() instead.")
pm_err_flag_type pm_pwron_stay_on_enable(boolean enable);


#endif
