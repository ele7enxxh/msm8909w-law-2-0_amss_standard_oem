/*! \file pm_mpm_target.c
*  \n
*  \brief This file implementes the target specific MPM SPMI shutdown/wakeup command buffer.
*  \n
*  \n &copy; Copyright 2014 Qualcomm Technologies Incorporated, All Rights Reserved
*
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/target/msm8952_pm8950/src/pm_mpm_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/20/14   kt     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm_target.h"

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
)
{
  pm_err_flag_type ret_val = PM_ERR_FLAG__SUCCESS;
  uint32 child_sleep_uv, child_active_uv = 0;
  uint32 parent_sleep_uv, parent_active_uv = 0;
  uint32 parent_sleep_vset, parent_active_vset = 0;
  uint32 child_sleep_hr = 0;
  uint32 delay = 0;
  uint8 sw_mode = 0;
  pm_mpm_target_cmd_arr_index child_arr_index;
  pm_mpm_target_cmd_arr_index parent_arr_index;
  pm_mpm_cmd_index_type *parent_index_ptr = NULL;
  rpm_resource_type rsrc_type;
  uint32 rsrc_index = 0;

  if ( (NULL == active_cfg_ptr) || (NULL == sleep_cfg_ptr) || (NULL == cmd_index_ptr) || 
       (NULL == mpm_rails_ptr) || (NULL == mpm_max_delay_ptr) )
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Configure MPM commands for MX (L3) parent rail S3 */
  child_sleep_uv = mpm_cmd_ptr->mx_sleep_microvolts;
  child_active_uv = mpm_cmd_ptr->mx_active_microvolts;
  child_arr_index = PM_MPM_MX_INDEX;
  parent_arr_index = PM_MPM_MX_PARENT_INDEX;

  if ((child_arr_index >= PM_MPM_NUM_INDICES) || (parent_arr_index >= PM_MPM_NUM_INDICES))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  parent_index_ptr = &cmd_index_ptr[parent_arr_index];
  rsrc_type = mpm_rails_ptr[parent_arr_index].resource_type;
  rsrc_index = mpm_rails_ptr[parent_arr_index].resource_index;

  /* Return SUCCESS if the rails have no parent */
  if ((rsrc_type == 0) || (rsrc_index == 0))
  {
    return PM_ERR_FLAG__SUCCESS;
  }

  if (parent_index_ptr == NULL)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get the current active voltage value which needs to be restored upon wake-up */
  ret_val = pm_rpm_volt_level_status(rsrc_type, rsrc_index, &parent_active_uv);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* Verify if the parent active voltage > child active voltage */
  CORE_VERIFY(parent_active_uv > child_active_uv);

  /* Active vset */
  ret_val = pm_rpm_calculate_vset(rsrc_type, rsrc_index, parent_active_uv, &parent_active_vset);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* Get the sleep headroom value from child rail info */
  child_sleep_hr = mpm_rails_ptr[child_arr_index].sleep_headroom_uv;

  /* Calculate the parent rail sleep voltage */
  parent_sleep_uv = child_sleep_uv + child_sleep_hr;

  /* Delay for this command */
  delay = pm_mpm_get_delay_from_microvolts(parent_sleep_uv, parent_active_uv);

  /* Update mpm_max_delay_ptr */
  if (delay > *mpm_max_delay_ptr)
  {
    *mpm_max_delay_ptr = delay;
  }

  /* Sleep vset */
  ret_val = pm_rpm_calculate_vset(rsrc_type, rsrc_index, parent_sleep_uv, &parent_sleep_vset);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  if (parent_index_ptr->sleep_level_index != PM_MPM_INVALID_CMD)
  {
    sleep_cfg_ptr[parent_index_ptr->sleep_level_index].data = parent_sleep_vset;
  }

  if (parent_index_ptr->active_level_index != PM_MPM_INVALID_CMD)
  {
    active_cfg_ptr[parent_index_ptr->active_level_index].data = parent_active_vset;
  }

  /* Restore SW mode */
  ret_val = pm_rpm_sw_mode_status_raw(rsrc_type, rsrc_index, &sw_mode);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  if (parent_index_ptr->sw_mode_level_index != PM_MPM_INVALID_CMD)
  {
    active_cfg_ptr[parent_index_ptr->sw_mode_level_index].data = sw_mode;
  }

  return ret_val;

} /*  pm_mpm_target_cmd_config */

