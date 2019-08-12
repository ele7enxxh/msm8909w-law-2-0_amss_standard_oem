#ifndef PM_PON__H
#define PM_PON__H

/*! \file pm_pon.h
 *  \n
 *  \brief  This file contains public API interface to support PowerON driver.
 *  \n
 *  \n &copy; Copyright 2012-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_pon.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/13   ps      Added PON Stage3 reset config API CR#495834 
04/10/13   kt      Added PMIC Watchdog config support  
02/27/13   kt      Added PON IRQ related APIs  
12/06/12   hw      Remove comdef.h and use com_dtypes.h
22/11/12   umr     Created PON API.
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_lib_err.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_pon_reset_cfg_type
   \brief Reset configure type.
 */
typedef enum
{
  PM_PON_RESET_CFG_WARM_RESET,
  PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  /* Shutdown to a state of main battery removal */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  /* Shutdown to a state of main battery and coin cell removal*/
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_HARD_RESET,
  /* Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Warm Reset and Main Batt/coin cell remove Shutdown */
  PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN,
  /* Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_INVALID  
}pm_pon_reset_cfg_type;

  
/*===========================================================================

                        HEADER FILES

===========================================================================*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg 
 *          PM_PON_RESET_CFG_WARM_RESET,
 *          PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
 *          PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN, Shutdown + dVdd_rb remove main battery
 *          PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN, Shutdown + xVdd_rb remove main and coin cell battery
 *          PM_PON_RESET_CFG_HARD_RESET,     Shutdown + Auto pwr up
 *   
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_ps_hold_cfg(unsigned pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg);


#endif // PM_PON__H
