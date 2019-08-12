/*===========================================================================

                    BOOT EXTERN PMIC DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external pmic drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_pmic_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/15   rk      Changed PMIC PS_HOLD config API call for multi-PMIC support.
01/06/15   pxm     Add generic PMIC LED API
05/22/13   yp      Add boot_pm_vib_on and boot_pm_vib_off
05/22/13   aus     Updated RAM dump LED to support multiple targets
04/02/13   dh      Add boot_pm_pon_ps_hold_cfg.
                   Change LED light to blue and brightness to high
03/29/13   plc     Add LED Functions
02/26/13   dh      Add boot_pm_init_smem
04/18/12   dh      change boot_pm_get_power_on_status to boot_pm_dev_get_power_on_reason
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_pmic_interface.h"
#include "boot_target.h"
#include "pm_led.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 
/*===========================================================================

**  Function :  boot_pm_device_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_device_init(void)
{
  return pm_device_init();
}

/*===========================================================================

**  Function :  boot_pm_dev_get_power_on_reason

** ==========================================================================
*/
/*!
 * @brief  This function returns the phone power-on reason. Typically used in boot
    during early bootup and stored in memory for later access.
 *
 * INPUT PARAMETERS
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *@param pwr_on_reason
 *    - pointer to 64-bit unsigned integer that stores the all PON reasons
 *   including PON power on, Warm Reset Reason and POFF_REASON,
 *   SOFT_RESET_REASON.
 *    PON_REASON
 *    PON_WARM_RESET_REASON
 *    PON_POFF_REASON
 *    PON_SOFT_RESET_REASON
 *
 * @return pm_err_flag_type.
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
 *                                       version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
 *                                       with the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 *@par Dependencies
 *      and pm_init() must have been called.
===========================================================================*/
pm_err_flag_type boot_pm_dev_get_power_on_reason(unsigned pmic_device_index, uint64* pwr_on_reason)
{
  return pm_pon_get_all_pon_reasons(pmic_device_index, pwr_on_reason);
}


/*===========================================================================

**  Function :  boot_pm_driver_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_driver_init(void)
{
  return pm_driver_init();
}


/*===========================================================================

**  Function :  boot_pm_driver_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_oem_init(void)
{
  return pm_oem_init();
}


/*===========================================================================

**  Function :  boot_pm_init_smem

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic smem initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_init_smem(void)
{
  return pm_smem_init();
}


/*===========================================================================

**  Function :  boot_pm_enable_led

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic led functions to turn on default LED
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_enable_led(boolean current_led_status)
{
    pm_led_type led_default = PM_LED_TYPE_INVALID;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    led_default = pm_led_get_default_led();
	do
	{
	    if(PM_LED_TYPE_INVALID != led_default)
	    {
	        // For the cases that LED hw unavailable or NOT specified in pm_config_target.c, we just return 0
	        err_flag = pm_led_enable(0, pm_led_get_default_led(), current_led_status);
			if(err_flag!=PM_ERR_FLAG__SUCCESS)
			{
				break;
			}
			err_flag = pm_led_enable(0, PM_LED_KPD, current_led_status);
	    }
	}while(0);

    return err_flag;
}


/*===========================================================================

**  Function :  boot_pm_pon_ps_hold_cfg

** ==========================================================================
*/
/**
 * @brief Configures PMIC to act on MSM PS_HOLD toggle. This
 *        is an app level API which handles all the required
 *        PS_HOLD config for all the applicable PMICs
 *        internally.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state. 
 * 
 * @param ps_hold_cfg 
 *          PMAPP_PS_HOLD_CFG_WARM_RESET,
 *          PMAPP_PS_HOLD_CFG_HARD_RESET,
 *          PMAPP_PS_HOLD_CFG_NORMAL_SHUTDOWN.
 *   
 *                   
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS = SUCCESS else 
 *         ERROR.
 */
pm_err_flag_type boot_pm_pon_ps_hold_cfg
(
  pmapp_ps_hold_cfg_type ps_hold_cfg
)
{
  return pmapp_ps_hold_cfg(ps_hold_cfg);
}

#ifdef FEATURE_BOOT_PON_VIBRATION

/*===========================================================================

**  Function :  boot_pm_vib_on

** ==========================================================================
*/
/*! 
 * @brief This function initializes vibration driver voltage and turn on it
 * @return Error flag.
 *
 */ 

pm_err_flag_type boot_pm_vib_on(void)
{
  pm_err_flag_type ret_val = PM_ERR_FLAG__SUCCESS;
  /* config vibrater driver voltage to 2200 mV. vibration dirver voltage range 1200mv~3100mv*/
  uint16  pm_vib_drv_volt = 2200; 

  ret_val = pm_vib_set_volt(0,PM_VIB__1,pm_vib_drv_volt);
  if (ret_val == PM_ERR_FLAG__SUCCESS)
  {
  	ret_val = pm_vib_enable(0,PM_VIB__1,PM_VIB_MODE__MANUAL,TRUE);
  }
	
  return ret_val;
    
}

/*===========================================================================

**  Function :  boot_pm_vib_off

** ==========================================================================
*/
/*! 
 * @brief This function turn off vibration
 * @return Error flag.
 *
 */ 

pm_err_flag_type boot_pm_vib_off(void)
{
  pm_err_flag_type ret_val = PM_ERR_FLAG__SUCCESS;
  /*Reset state is 0x0000 */
  uint16  pm_vib_drv_volt = 0;

  ret_val =pm_vib_enable(0,PM_VIB__1,PM_VIB_MODE__MANUAL,FALSE);
  if (ret_val == PM_ERR_FLAG__SUCCESS)
  {
  	ret_val = pm_vib_set_volt(0,PM_VIB__1,pm_vib_drv_volt); 
  }
  return ret_val;
}

#endif

/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status

** ==========================================================================
*/
/*! 
 * @brief This function provides warm reset status information
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_pon_warm_reset_status
(
  unsigned pmic_device_index, 
  boolean *status
)
{
  return pm_pon_warm_reset_status(pmic_device_index, status);
}


/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status_clear

** ==========================================================================
*/
/*! 
 * @brief This function clears the warm reset status
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_pon_warm_reset_status_clear
(
  unsigned pmic_device_index
)
{
  return pm_pon_warm_reset_status_clear(pmic_device_index);
}
