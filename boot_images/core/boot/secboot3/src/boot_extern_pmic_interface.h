#ifndef BOOT_EXTERN_PMIC_INTERFACE_H
#define BOOT_EXTERN_PMIC_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN PMIC DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external pmic drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_pmic_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/15   rk      Changed PMIC PS_HOLD config API call for multi-PMIC support.
05/24/13   yp      Add boot_pm_vib_on and boot_pm_vib_off.
04/02/13   dh      Add boot_pm_pon_ps_hold_cfg.
03/29/13   plc     Add LED functions
02/26/13   dh      Add boot_pm_init_smem
09/26/12   dh      Add boot_pm_oem_init and boot_pm_driver_init
04/18/12   dh      change boot_pm_get_power_on_status to boot_pm_dev_get_power_on_reason
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "pm.h"
#include "pm_rgb.h"
#include "boot_logger.h"
#include "boot_logger_timer.h"


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
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_device_init(void);
#else
  static inline pm_err_flag_type boot_pm_device_init(void)
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif


/*===========================================================================

**  Function :  boot_pm_get_power_on_status

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
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_dev_get_power_on_reason(unsigned pmic_device_index, uint64* pwr_on_reason);
#else
  static inline pm_err_flag_type boot_pm_dev_get_power_on_reason(unsigned pmic_device_index, uint64* pwr_on_reason)
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif


/*===========================================================================

**  Function :  boot_pm_driver_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic sw driver initialization
 * @return Error flag.
 *
 */ 
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_driver_init(void);
#else
  static inline pm_err_flag_type boot_pm_driver_init(void)
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif


/*===========================================================================

**  Function :  boot_pm_oem_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic sw driver initialization
 * @return Error flag.
 *
 */ 
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_oem_init(void);
#else
  static inline pm_err_flag_type boot_pm_oem_init(void)
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif


/*===========================================================================

**  Function :  boot_pm_init_smem

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic smem initialization
 * @return Error flag.
 *
 */ 
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_init_smem(void);
#else
  static inline pm_err_flag_type boot_pm_init_smem(void)
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif



/*===========================================================================

**  Function :  boot_pm_enable_led

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic rgb led functions to turn on an LED
 * @return Error flag.
 *
 */ 
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_enable_led(boolean);
#else
  static inline pm_err_flag_type boot_pm_enable_led(boolean enable)
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif



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
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_pon_ps_hold_cfg(pmapp_ps_hold_cfg_type);
#else
  static inline pm_err_flag_type boot_pm_pon_ps_hold_cfg
  (
    pmapp_ps_hold_cfg_type ps_hold_cfg
  )
  {
    return PM_ERR_FLAG__SUCCESS;
  }
#endif

/*===========================================================================

**  Function :  boot_pm_vib_on

** ==========================================================================
*/
/*! 
* @brief This function initializes vibration driver voltage and turn on it
* @return Error flag.
*
*/
#if (defined(FEATURE_BOOT_EXTERN_PMIC_COMPLETED) && defined(FEATURE_BOOT_PON_VIBRATION))
  pm_err_flag_type boot_pm_vib_on(void);
#else
  static inline pm_err_flag_type boot_pm_vib_on
  (
    void
  )
  {
	return PM_ERR_FLAG__SUCCESS;
  }
#endif
/*===========================================================================

**  Function :  boot_pm_vib_off

** ==========================================================================
*/
/*! 
 * @brief This function turn off vibration
 * @return Error flag.
 *
 */ 
#if (defined(FEATURE_BOOT_EXTERN_PMIC_COMPLETED) && defined(FEATURE_BOOT_PON_VIBRATION))
  pm_err_flag_type boot_pm_vib_off(void);
#else
  static inline pm_err_flag_type boot_pm_vib_off
  (
    void
  )
  {
  	return PM_ERR_FLAG__SUCCESS;
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
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED
  pm_err_flag_type boot_pm_pon_warm_reset_status(unsigned pmic_device_index, boolean *status);
#else
  static inline pm_err_flag_type boot_pm_pon_warm_reset_status
  (
    unsigned pmic_device_index, 
    boolean *status
  )
  {
  	return PM_ERR_FLAG__SUCCESS;
  }
#endif

/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status_clear

** ==========================================================================
*/
/*! 
 * @brief This function clears the warm reset status
 * @return Error flag.
 *
 */ 
#ifdef FEATURE_BOOT_EXTERN_PMIC_COMPLETED 
  pm_err_flag_type boot_pm_pon_warm_reset_status_clear(unsigned pmic_device_index);
#else
  static inline pm_err_flag_type boot_pm_pon_warm_reset_status_clear
  (
    unsigned pmic_device_index
  )
  {
  	return PM_ERR_FLAG__SUCCESS;
  }
#endif
#endif /* BOOT_EXTERN_PMIC_INTERFACE_H */
