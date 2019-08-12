#ifndef __PM_MPM_TARGET_H__
#define __PM_MPM_TARGET_H__

/*! \file pm_mpm_target.h
*  \n
*  \brief Header file for target specific PMIC MPM declarations.
*  \n
*  \n &copy; Copyright 2014-2015 Qualcomm Technologies Incorporated, All Rights Reserved
*
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/target/msm8909_pm8916/src/pm_mpm_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28.15   akt    Update the retension voltage for S3 and S4 (CR-779395)
11/20/14   kt     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm_internal.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/*This enum is in strict order. If you need to
add /delete any entry from the sleep_cfg array
this enum should be updated as well*/
typedef enum 
{
  PM_MPM_SLEEP_CMD_CX_VOLT,
  PM_MPM_SLEEP_CMD_MX_VOLT,
  PM_MPM_SLEEP_CMD_MX_PARENT_VOLT,
  PM_MPM_SLEEP_CMD_S4_VOLT,
  PM_MPM_SLEEP_CMD_PBS_TRIG, 
  PM_MPM_SLEEP_CMD_END,
}pm_mpm_sleep;

/*This enum is in strict order. If you need to
add /delete any entry from the active_cfg array
this enum should be updated as well*/
typedef enum 
{
  PM_MPM_ACTIVE_CMD_PBS_TRIG, 
  PM_MPM_ACTIVE_CMD_S4_VOLT,        /* S4 mode is taken care in PBS RAM */
  PM_MPM_ACTIVE_CMD_MX_PARENT_MODE,
  PM_MPM_ACTIVE_CMD_MX_MODE,
  PM_MPM_ACTIVE_CMD_CX_MODE,
  PM_MPM_ACTIVE_CMD_MX_PARENT_VOLT,
  PM_MPM_ACTIVE_CMD_MX_VOLT,
  PM_MPM_ACTIVE_CMD_CX_VOLT,
  PM_MPM_ACTIVE_CMD_END,
}pm_mpm_active;

/**
  * Enum to store the array index of the different rail types in
  * pm_mpm_cmd_index_type[] in target based config.
  */
typedef enum
{
   PM_MPM_CX_INDEX,
   PM_MPM_MX_INDEX,
   PM_MPM_MX_PARENT_INDEX,
   PM_MPM_S4_INDEX,   
   PM_MPM_NUM_INDICES,
} pm_mpm_target_cmd_arr_index;

/*----------------------------------------------------------------------------
 * Function : pm_mpm_target_cmd_config
 * -------------------------------------------------------------------------*/
/*!
    Description: Return a pointer to a config data buffer for target based
                 MPM voltage rail command.
    @param
      mpm_cfg_ptr: pointer to the data structure that pointer to
                   the buffer which contains target based config data
                   for retention and active voltage.
    @return
    PM_ERR_FLAG__SUCCESS success otherwise PMIC error.

    @dependencies
    railway_get_corner_voltage()
    pm_rpm_calculate_vset()


    @sa None
*/
pm_err_flag_type pm_mpm_target_cmd_config
(
  pm_mpm_cmd_cfg_type *mpm_cmd_ptr,
  pm_mpm_cfg_type *sleep_cfg_ptr, 
  pm_mpm_cfg_type *active_cfg_ptr, 
  pm_mpm_rails_info_type *mpm_rails_ptr, 
  pm_mpm_cmd_index_type *cmd_index_ptr,
  uint32 *mpm_max_delay_ptr
);

#endif /* __PM_MPM_TARGET_H__ */

