/*! \file
*  \n
*  \brief  pm_tz_config.c
*  \n
*  \n This file contains pmic configuration data specific for trust zone image.
*  \n
*  \n &copy; Copyright 2012--2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/config/msm8916_pm8916/src/pm_config_pm8916.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/08/13   vtw     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "comdef.h"
#include "pm_npa_vreg.h"
#include "pm_npa_device.h"

/*===========================================================================
Data Declarations
===========================================================================*/

/*
 * Configuration data to disable qfprom rail.
 */
const pm_npa_ldo_kvps
pm_pam_qfprom_rail[] =
{
  {
    PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
    PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
    PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
    PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
    PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
    PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
    PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
    PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
  },

  /*
   * Configuration data to enable qfprom rail.
   */
  {
    PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
    PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
    PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
    PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
    PM_NPA_KEY_MICRO_VOLT, 4, 1900000,  /**< [X uV] -> max aggregation */
    PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
    PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
    PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
  }
};
