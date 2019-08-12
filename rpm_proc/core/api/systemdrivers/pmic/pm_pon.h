#ifndef PM_PON__H
#define PM_PON__H

/** @file pm_pon.h 
 *  
 *   This file contains functions and variable declarations to support 
 *   the pon driver module.
*/
/*
Copyright (C) 2012-2014 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_pon.h#1 $
  $DateTime: 2015/02/25 02:53:43 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   leo	   (Tech Pubs) Edited/added Doxygen comments and markup.
04/10/13   kt      Added PMIC Watchdog config support  
02/27/13   kt      Added PON IRQ related APIs  
12/06/12   hw      Remove comdef.h and use com_dtypes.h
22/11/12   umr     Created PON API.
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/** @addtogroup pm_pon
@{ */

/** Reset configuration type.
 */
typedef enum
{
  PM_PON_RESET_CFG_NO_RESET_LOOP,
  /**< Reset option disabled. */  

  PM_PON_RESET_CFG_WARM_RESET,
  /**< Warm reset. */

  PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
    /**< Normal shutdown. */

  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  /**< Shutdown to a state of main battery removal. */

  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /**< Shutdown to a state of main battery and coin cell removal. */

  PM_PON_RESET_CFG_HARD_RESET,
  /**< Shutdown and auto power up. */

  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  /**< Shutdown to a state of main battery removal and 
       auto power up. */

  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  /**< Shutdown to a state of main battery and coinccell
       removal and auto power up. */

  PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /**< Shutdown to a state of main battery and coin cell removal. @newpage */

  PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN,
  /**< Warm reset the first time the reset triggers. Shut down
       to a state of main battery removal if the reset triggers a
       subsequent time. */

  PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /**< Warm reset the first time the reset triggers. Shut down to
       a state of main battery and coin cell removal if the reset
       triggers a subsequent time. */

  PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN,
  /**< Warm reset the first time the reset triggers. Normal
       shutdown if the reset triggers a subsequent time. */

  PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  /**< Warm reset the first time the reset triggers. Normal
       shutdown and auto power up if the reset triggers a
       subsequent time. */

  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  /**< Warm reset the first time the reset triggers. Normal
       shutdown to a state of battery removal and auto power up
       if the reset triggers a subsequent time. */

  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  /**< Warm reset the first time the reset triggers. Normal
       shutdown to a state of battery and coin cell removal, then
       auto power up if the reset triggers a subsequent time. 
   */

  PM_PON_RESET_CFG_INVALID  
}pm_pon_reset_cfg_type;
  
/*===========================================================================

                        HEADER FILES

===========================================================================*/

/**
 *  Configures the PON PMIC Watchdog reset source.
 *
 * @datatypes
 *  #pm_pon_reset_cfg_type
 * 
 * @param[in] pmic_device_index When the target has more than one PMIC,
 *                              this is the index of the PMIC in which
 *                              the power on module is physically located.
 *                              The device index starts at zero.
 * @param[in] s1_timer          Select the S1 timer setting in ms for the
 *                              Watchdog bark. The supported time in seconds
 *                              is 0 to 127.
 * @param[in] s2_timer          Select the S2 timer setting in seconds for the
 *                              Watchdog bite. This is the time after the bark
 *                              (S1). The supported time in seconds is 0 to 127.
 * @param[in] reset_cfg_type    Configure the type of reset to be 
 *                              performed on the event. See
 *                              #pm_pon_reset_cfg_type for valid values.
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type
pm_pon_wdog_cfg(uint8 pmic_device_index, uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type);

/**
 *  PON PMIC Watchdog reset control (enable/disable).
 * 
 * @param[in] pmic_device_index When the target has more than one PMIC,
 *                              this is the index of the PMIC in which
 *                              the power on module is physically located.
 *                              The device index starts at zero.
 * @param[in] enable            Enable or disable PMIC Watchdog reset control.
 * 
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type
pm_pon_wdog_enable(uint8 pmic_device_index, pm_on_off_type enable);

/** Clears the PMIC Watchdog timer (pets the PMIC Watchdog).
 * 
 * @param[in] pmic_device_index When the target has more than one PMIC,
 *                              this is the index of the PMIC in which
 *                              the power on module is physically located.
 *                              The device index starts at zero.
 * 
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type
pm_pon_wdog_pet(uint8 pmic_device_index);

/**
 * Gets the PON PMIC Watchdog configuration. 
 * 
 * @param[in] pmic_device_index When the target has more than
 *            one pmic, this is the index of the PMIC in which
 *            the power on module is physically located. The device
 *            index starts at zero.
 *  
 * @param[in] s1_timer Pointer to the S1 timer in seconds for bark.
 *               Supported time in seconds: 0 to 127.
 *  
 * @param[in] s2_timer Pointer to the S2 timer in seconds for bite. This is
 *               the time after bark S1.
 *               Supported time in seconds: 0 to 127
 *
 * @param[in] reset_type Pointer to the wdog reset reason type. 
 *              See #pm_pon_reset_cfg_type.
 *  
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type
pm_pon_wdog_get_cfg
(
  uint8 pmic_device_index,
  uint8* s1_timer,
  uint8* s2_timer,
  uint8* reset_type
);

/**
 * Sets the Qualcomm Debug Subsystem (QDSS) debug flag. 
 * 
 * @param[in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1. 
 *
 * @return 
 * PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature is not
 * available on this version of the PMIC. \n
 * PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device index is out of range. \n
 * PM_ERR_FLAG__SUCCESS = SUCCESS. \n
 * See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pon_qdss_set_flag(uint8 pmic_device_index);

/** Clears the QDSS debug flag.
 * 
 * @param[in] pmic_device_index Primary PMIC: 0 Secondary PMIC: 1.
 *
 * @return 
 * PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature is not
 * available on this version of the PMIC. \n
 * PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device index is out of
 * range. \n
 * PM_ERR_FLAG__SUCCESS = SUCCESS. \n
 * See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pon_qdss_clear_flag(uint8 pmic_device_index);

/** Returns the QDSS debug flag status indicating if debugging
 *  is enabled. This status must be cleared using the
 *  pm_pon_qdss_clear_flag() function.
 * 
 * @param[in]  pmic_device_index Primary PMIC -- 0; secondary PMIC -- 1.
 * @param[out] status            QDSS debug status (returns TRUE if QDSS
 *                               debug is enabled, otherwise FALSE).
 *
 *
 * @return   
 * PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 * in as an argument. \n
 * PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature is not
 * available on this version of the PMIC. \n
 * PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device index is out of
 * range. \n
 * PM_ERR_FLAG__SUCCESS = SUCCESS. \n
 * See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pon_qdss_get_flag(uint8 pmic_device_index, boolean *status);

/** @} */ /* end_group pm_pon */
#endif // PM_PON__H
