#ifndef PM_BOOST_BYP__H
#define PM_BOOST_BYP__H

/*! \file pm_boost_byp.h
 *  \n
 *  \brief This header file contains enums and API definitions for PMIC BOOST BYPASS
 *         power rail driver.
 *  \n
 *  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_boost_byp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/14   kt      Created
=============================================================================*/

/*===========================================================================

                 HEADER FILE INCLUDE

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*!
 *  \brief 
 *  BOOST BYPASS peripheral index. This enum type contains all boost bypass regulators that you may need. 
 */
enum
{
  PM_BOOST_BYP_1,
  PM_BOOST_BYP_2,
  PM_BOOST_BYP_INVALID
};

typedef enum 
{
   PM_BBYP_MODE_FORCE_BYPASS = 0,
   PM_BBYP_MODE_AUTO_BOOST = 1,
   PM_BBYP_MODE_INVALID
}pm_boost_byp_mode_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/**
 * @name 
 *     pm_boost_byp_sw_mode
 *
 * @description
 *     Switch between Force Bypass, Auto Boost, and other modes of a regulator.
 * 
 * @param 
 *     mode - Select the different mode of a regulator. Example: FORCE_BYPASS, AUTO_BOOST
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_boost_byp_sw_mode(uint8 pmic_chip, uint8 peripheral_index, pm_boost_byp_mode_type mode);

/**
 * @name pm_boost_byp_pin_ctrl_volt_level
 *
 * @brief This function programs the MODE_BB pin output voltage 
 *        level (in micro volts) for the selected boost bypass
 *        power rail.
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  peripheral_index:
 *                Starts from 0 (for first Boost bypass
 *                peripheral)
 * @param[in]  volt_level:
 *                Pin output volt level in micro volts (uint32).
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else ERROR.
 *
 */
pm_err_flag_type pm_boost_byp_pin_ctrl_volt_level(uint8 pmic_chip, uint8 peripheral_index, pm_volt_level_type volt_level);

/**
 * @name pm_boost_byp_pin_ctrl_volt_level_status
 *
 * @brief This function returns the MODE_BB pin output voltage 
 *        level (in micro volts) for the selected boost bypass
 *        power rail.
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  peripheral_index:
 *                Starts from 0 (for first Boost bypass
 *                peripheral)
 * @param[out] volt_level:
 *                Variable to return to the caller with Pin volt
 *                level status in micro volts (uint32).
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else ERROR.
 *
 */
pm_err_flag_type pm_boost_byp_pin_ctrl_volt_level_status(uint8 pmic_chip, uint8 peripheral_index, pm_volt_level_type *volt_level); 

#endif /* PM_BOOST_BYP__H */

