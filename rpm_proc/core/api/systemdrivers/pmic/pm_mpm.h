#ifndef __PM_MPM_H__
#define __PM_MPM_H__

/**
 * @file pm_mpm.h
 *
 * Header file for PMIC MPM VDD command driver.
 */
/*
  ====================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_mpm.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/

/*----------------------------------------------------------------------------
 *  Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "pm_err_flags.h"

/*----------------------------------------------------------------------------
 * Type Definitions
 * -------------------------------------------------------------------------*/

/** @addtogroup pm_mpm
@{ */

/**
 * MPM command configuration.
 */
typedef struct
{
  uint32 slave_id;    /**< Slave ID. */
  uint32 slave_addr;  /**< SPMI address. */
  uint32 data;        /**< Data. */
  uint32 delay;       /**< Delay. */
} pm_mpm_cfg_type;

/**
 * PMIC configuration data for the MPM wake/sleep sequence.
 */
typedef struct
{
  pm_mpm_cfg_type* active_cfg_ptr; /**< [OUT] Pointer to the SPMI command buffer for MPM wake. */
  pm_mpm_cfg_type* sleep_cfg_ptr;  /**< [OUT] Pointer to the SPMI command buffer for MPM sleep. */
  uint32 mx_sleep_microvolts;      /**< [IN] MX sleep voltage. */
  uint32 cx_sleep_microvolts;      /**< [IN] CX sleep voltage. */
  uint32 mx_active_microvolts;     /**< [IN] MX active voltage. */
  uint32 cx_active_microvolts;     /**< [IN] CX active voltage. */
} pm_mpm_cmd_cfg_type;

/*----------------------------------------------------------------------------
 * Function : pm_mpm_cmd_config
 * -------------------------------------------------------------------------*/
/**
    Returns a pointer to a configuration data buffer for the MPM voltage rail
    command.

    @param[out] mpm_cmd_ptr Pointer to the data structure that points to
                            the buffer that contains configuration data
                            for retention and the active voltage.

    @return 
    SUCCESS or Error -- See #pm_err_flag_type.

    @dependencies
    railway_get_corner_voltage() (core/power).
*/
pm_err_flag_type pm_mpm_cmd_config
(
  pm_mpm_cmd_cfg_type * mpm_cmd_ptr
);

/** @} */ /* end_addtogroup pm_mpm */

#endif /* __PM_MPM_H__ */

