#ifndef __PM_NPA_VREG_H__
#define __PM_NPA_VREG_H__

/**
  @file pm_npa_vreg.h

  Header file for initializing VREG NPA vector requests.
*/
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_npa_vreg.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_lib_err.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/

/**
 * Macros to get the number of elements in the KVPS structures. Used as the
 * second parameter to npa_issue_vector_request.
 */
#define PM_NPA_SMPS_KVPS_ELEMS (sizeof(pm_npa_smps_kvps) / sizeof(unsigned))
#define PM_NPA_LDO_KVPS_ELEMS (sizeof(pm_npa_ldo_kvps) / sizeof(unsigned))
#define PM_NPA_VS_KVPS_ELEMS (sizeof(pm_npa_vs_kvps) / sizeof(unsigned))


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 *  pm_npa_smps_kvps
 *
 *  Vector request structure for SMPS NPA nodes.
 */
typedef struct
{
    // [Disable (default), Enable]  max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    // [AUTO (default), IPEAK, NPM] - max aggregation (left to right)
    unsigned smps_sw_mode_key; // PM_NPA_KEY_SMPS_SOFTWARE_MODE
    unsigned smps_sw_mode_size; // 4
    unsigned smps_sw_mode_value;

    //  [NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    //  [NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
    unsigned pin_ctrl_power_mode_key; // PM_NPA_KEY_PIN_CTRL_POWER_MODE
    unsigned pin_ctrl_power_mode_size;
    unsigned pin_ctrl_power_mode_value;

    // [X uV]  max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    //  [X uA]  max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;

    // [xx MHz]  max within a priority group
    //  Allows one execution environments vote to be higher or lower priority relative to another execution environment
    unsigned frequency_key; // PM_NPA_KEY_FREQUENCY
    unsigned frequency_size; // 4
    unsigned frequency_value;

    //Frequency_Reason  Optional  [xx]  Distinct indicators per execution environment
    //    One execution environment will not be allowed to use the indicators of another
    //    - Allows interspersed prioritization between execution environments for
    //    frequency aggregation
    //     Ex: Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority
    unsigned frequency_reason_key; // PM_NPA_KEY_FREQUENCY_REASON
    unsigned frequency_reason_size; // 4
    unsigned frequency_reason_value;

    // [None, Quiet, Super Quiet]  max aggregation (left to right)
    // Mask that allows SMPS to follow global quiet mode signal
    unsigned follow_quiet_mode_key; // PM_NPA_KEY_FOLLOW_QUIET_MODE
    unsigned follow_quiet_mode_size; // 4
    unsigned follow_quiet_mode_value;

    // [x uV] - max aggregation
    unsigned head_room_voltage_key; // PM_NPA_KEY_HEAD_ROOM
    unsigned head_room_voltage_size; // 4
    unsigned head_room_voltage_value;

    // [ALLOWED, DISALLOWED] - MAX value left to right
    unsigned bypass_allowed_key; // PM_NPA_KEY_BYPASS_ALLOWED_KEY
    unsigned bypass_allowed_size; // 4
    unsigned bypass_allowed_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_key; // PM_NPA_KEY_CORNER_VOLTAGE_KEY
    unsigned corner_size; // 4
    unsigned corner_value;
} pm_npa_smps_kvps;


/**
 *  pm_npa_ldo_kvps
 *
 *  Vector request structure for LDO NPA nodes.
 */
typedef struct
{
    // [Disable (default), Enable]  max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    // [IPEAK (default), NPM] - max aggregation (left to right)
    unsigned ldo_sw_mode_key; // PM_NPA_KEY_LDO_SOFTWARE_MODE
    unsigned ldo_sw_mode_size; // 4
    unsigned ldo_sw_mode_value;

    //  [NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    //  [NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
    unsigned pin_ctrl_power_mode_key; // PM_NPA_KEY_PIN_CTRL_POWER_MODE
    unsigned pin_ctrl_power_mode_size;
    unsigned pin_ctrl_power_mode_value;

    // [X uV]  max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    //  [X mA]  max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;

    // [x uV] - max aggregation
    unsigned head_room_voltage_key; // PM_NPA_KEY_HEAD_ROOM
    unsigned head_room_voltage_size; // 4
    unsigned head_room_voltage_value;

    // [ALLOWED, DISALLOWED] - MAX value left to right
    unsigned bypass_allowed_key; // PM_NPA_KEY_BYPASS_ALLOWED_KEY
    unsigned bypass_allowed_size; // 4
    unsigned bypass_allowed_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_key; // PM_NPA_KEY_CORNER_VOLTAGE_KEY
    unsigned corner_size; // 4
    unsigned corner_value;
} pm_npa_ldo_kvps;


/**
 *  pm_npa_vs_kvps
 *
 *  Vector request structure for VS NPA nodes.
 */
typedef struct
{
    // [Disable, Enable]  max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    //  [NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    // [X uV]  max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    //  [X uA]  max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;
} pm_npa_vs_kvps;


/*=========================================================================
      Function Definitions
==========================================================================*/

/**
  @name
    pm_npa_smps_kvps_init

  @description
    Initializes the vector request structure for use with SMPS NPA nodes.

  @param
    kvps - The kvps structure to be initialized.

  @return
    error flag type - pm_err_flag_type
*/
pm_err_flag_type pm_npa_smps_kvps_init (pm_npa_smps_kvps *kvps);


/**
  @name
    pm_npa_ldo_kvps_init

  @description
    Initializes the vector request structure for use with LDO NPA nodes.

  @param
    kvps - The kvps structure to be initialized.

  @return
    error flag type - pm_err_flag_type
*/
pm_err_flag_type pm_npa_ldo_kvps_init (pm_npa_ldo_kvps *kvps);


/**
  @name
    pm_npa_vs_kvps_init

  @description
    Initializes the vector request structure for use with VS NPA nodes.

  @param
    kvps - The kvps structure to be initialized.

  @return
    error flag type - pm_err_flag_type
*/
pm_err_flag_type pm_npa_vs_kvps_init (pm_npa_vs_kvps *kvps);


#endif /* __PM_NPA_VREG_H__ */

