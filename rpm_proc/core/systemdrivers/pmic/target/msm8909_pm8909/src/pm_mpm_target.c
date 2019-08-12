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

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/target/msm8909_pm8909/src/pm_mpm_target.c#1 $

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

  /* No target based mpm commads */

  return ret_val;

} /*  pm_mpm_target_cmd_config */

