#ifndef PM_LDO__H
#define PM_LDO__H
/*! \file pm_ldo.h
 *  \n
 *  \brief This header file contains enums and API definitions for PMIC voltage
 *         regulator driver.
 *  \n
 *  \n &copy; Copyright 2009-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_ldo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
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
 *  LDO peripheral index. This enum type contains all ldo regulators that you may need. 
 */
enum
{
  PM_LDO_1,
  PM_LDO_2,
  PM_LDO_3,
  PM_LDO_4,
  PM_LDO_5,
  PM_LDO_6,
  PM_LDO_7,
  PM_LDO_8,
  PM_LDO_9,
  PM_LDO_10,
  PM_LDO_11,
  PM_LDO_12,
  PM_LDO_13,
  PM_LDO_14,
  PM_LDO_15,
  PM_LDO_16,
  PM_LDO_17,
  PM_LDO_18,
  PM_LDO_19,
  PM_LDO_20,
  PM_LDO_21,
  PM_LDO_22,
  PM_LDO_23,
  PM_LDO_24,
  PM_LDO_25,
  PM_LDO_26,
  PM_LDO_27,
  PM_LDO_28,
  PM_LDO_29,
  PM_LDO_30,
  PM_LDO_31,
  PM_LDO_32,
  PM_LDO_33,
  PM_LDO_INVALID
};

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/**
 * @name 
 *     pm_ldo_pull_down
 *
 * @description
 *     Allows you to enable disable active pulldown.
 * 
 * @param 
 *     on_off - turn on and off active pulldown
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_ldo_pull_down
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off);

/**
 * @name 
 *     pm_ldo_sw_mode
 *
 * @description
 *     Switch between HPM, LPM, and other modes of a regulator.
 * 
 * @param 
 *     sw_mode - Select the different mode of a regulator. Example, HPM, LPM
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_ldo_sw_mode
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type sw_mode);

/** 
 * @name pm_ldo_sw_mode_status 
 *  
 * @brief This function returns the mode status (LPM, NPM, AUTO, BYPASS)
 *        of the selected power rail. Note, the mode of a regulator
 *        changes dynamically.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out] sw_mode:  
 *                Variable to return to the caller with mode status.
 *                Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_ldo_sw_mode_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type* sw_mode);

/**
 * @name 
 *     pm_ldo_pin_ctrled
 *
 * @description
 *     select the pin ( connected to external signal ) that you would like to use
 *     to effect the state ( on/off ) and mode ( HPM, LPM etc ) of a regulator
 * 
 * @param 
 *     select_pin - Select a pin
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_ldo_pin_ctrled
(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 select_pin);

/**
 * @name 
 *     pm_ldo_sw_enable
 *
 * @description
 *     enable or disable a regulator or voltage switcher.
 * 
 * @param 
 *     on_off - turn on and off the regulator.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_ldo_sw_enable
(uint8 pmic_chip, uint8  ldo_peripheral_index, pm_on_off_type  on_off);

/**
 * @name 
 *     pm_ldo_volt_level
 *
 * @description
 *     set the voltage of a regulator.
 *     Note, differnt type ( LDO, HFS etc ) may have different programmable voltage steps.
 *     Only support the correct programmable steps. Not rounding voltages if the voltage selected
 *     is not part of the programmable steps.
 * 
 * @param 
 *     volt_level - select the voltage in micro volts.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_ldo_volt_level
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level);

/** 
 * @name pm_ldo_volt_level_status 
 *  
 * @brief This function returns the voltage level (in micro 
 *        volts) of the selected power rail.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out] volt_level:  
 *                Variable to return to the caller with volt
 *                level status in micro volts (uint32).
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_ldo_volt_level_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type* volt_level);

/** 
 * @name pm_ldo_vreg_ok_status 
 *  
 * @brief This function returns the vreg_ok status.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out] status:  
 *                Variable to return to the caller with vreg_ok status 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_ldo_vreg_ok_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean* status);


#endif /* PM_LDO__H */

