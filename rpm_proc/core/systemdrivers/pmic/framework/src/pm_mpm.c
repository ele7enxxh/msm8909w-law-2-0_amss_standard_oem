/*! \file pm_mpm.c
*  \n
*  \brief This file implementes the MPM SPMI shutdown/wakeup command buffer.
*  \n
*  \n &copy; Copyright 2013-2015 Qualcomm Technologies Incorporated, All Rights Reserved
*
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/src/pm_mpm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   vtw     Added mock vdd_min support.
10/10/13   vtw     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm.h"
#include "pm_dal_prop_ids.h"
#include "pm_mpm_internal.h"
#include "pm_mpm_target.h"

#include "pm_target_information.h"
#include "pm_rpm_target.h"
#include "CoreVerify.h"

/*=========================================================================
      Definitions
==========================================================================*/

#define CEILING_DIV(a, b)  (((a)+((b)-1))/(b))

/*=========================================================================
      Variables.
==========================================================================*/

/**
 * Config data programs the MPM registers
 * for the SPMI sleep/wakeup command sequence.
 */
static pm_mpm_cfg_type* sleep_cfg = NULL;
static pm_mpm_cfg_type* active_cfg = NULL;

/* SPMI sequence index. */
static pm_mpm_cmd_index_type* cmd_index_ptr = NULL;

/* PM MPM Rails info. */
static pm_mpm_rails_info_type* pm_mpm_rails = NULL;

/* SPMI Delay table from MPM2 HPG
  4bit   # XO cycles     Delay in usec
  0000   0               0
  0001   192             10
  0010   480             25
  0011   960             50
  0100   1920            100
  0101   2880            150
  0110   3840            200
  0111   4800            250
  1000   5760            300
  1001   6720            350
  1010   7680            400
  1011   9600            500
  1100   11520           600
  1101   13440           700
  1110   16320           850
  1111   19200           1000
*/
static uint16 delay_table[16] =
{0, 10, 25, 50, 100, 150, 200, 250, 300, 350, 400, 500, 600, 700, 850, 1000};

static uint32 pm_mpm_max_delay = 0;

/* ============================================================================
**  Function : pm_mpm_get_delay_from_microvolts
** ============================================================================
*/
/*!
   Calculate the delay based on active and retention volttage.

   @param  pCfg - pointer to MPM config data buffer that sent to the PMIC.
   @retval None
   Note: This function is copied from MPM driver code.

*/
uint16 pm_mpm_get_delay_from_microvolts
(
  uint32 off_microvolts,
  uint32 on_microvolts
)
{
  uint32 desired_delay = 0;
  uint32 max = 16;
  uint32 min = 0;
  uint32 i = 0;
  uint32 mid = max / 2;

  // PM8026 step FTS2 and LDO @1.5mV/usec
  desired_delay = (uint32)CEILING_DIV((on_microvolts - off_microvolts), 1500);
  // Add 50uS of additional delay, per pmic systems
  desired_delay += 50;

  //max of 1000us delay
  if (desired_delay > 1000)
    return 15;

  for (i = 0; i < 4; i++)
  {
    if (delay_table[mid] == desired_delay)
      break;
    if (delay_table[mid] < desired_delay)
    {
      if (i == 3)
      {
        mid++;
        break;
      }
      min = mid;
    }
    else
    {
      if (i == 3)
      {
        break;
      }
      max = mid;
    }
    mid = ((max + min) / 2);
  }

  return mid;

} /* pm_mpm_get_delay_from_microvolts */

/* ============================================================================
**  Function : pm_mpm_cmd_rail
** ============================================================================
*/
/*!
    Program the SPMI sequence cmd for VDD_MX/VDD_CX voltage rail.

   @retval PM_ERR_FLAG__SUCCESS on success else PMIC error code.

*/
static pm_err_flag_type pm_mpm_cmd_rail
(
  uint32 sleep_uv,
  uint32 active_uv,
  pm_mpm_target_cmd_arr_index rail_arr_index
)
{
  pm_err_flag_type ret_val = PM_ERR_FLAG__SUCCESS;
  uint32 sleep_vset, active_vset = 0;
  uint32 delay = 0;
  uint8 sw_mode = 0;
  pm_mpm_cmd_index_type *rail_index_ptr = NULL;
  rpm_resource_type rsrc_type;
  uint32 rsrc_index = 0;

  if (rail_arr_index >= PM_MPM_NUM_INDICES)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  rail_index_ptr = &cmd_index_ptr[rail_arr_index];
  rsrc_type = pm_mpm_rails[rail_arr_index].resource_type;
  rsrc_index = pm_mpm_rails[rail_arr_index].resource_index;

  if ((rsrc_type == 0) || (rsrc_index == 0) || (rail_index_ptr == NULL))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Active vset */
  ret_val = pm_rpm_calculate_vset(rsrc_type, rsrc_index, active_uv, &active_vset);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* Delay for this command */
  delay = pm_mpm_get_delay_from_microvolts(sleep_uv, active_uv);

  /* Update pm_mpm_max_delay */
  if (delay > pm_mpm_max_delay)
  {
    pm_mpm_max_delay = delay;
  }

  /* Sleep vset */
  ret_val = pm_rpm_calculate_vset(rsrc_type, rsrc_index, sleep_uv, &sleep_vset);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }
  sleep_cfg[rail_index_ptr->sleep_level_index].data = sleep_vset;

  active_cfg[rail_index_ptr->active_level_index].data = active_vset;

  /* Restore SW mode */
  ret_val = pm_rpm_sw_mode_status_raw(rsrc_type, rsrc_index, &sw_mode);
  if (ret_val != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }
  active_cfg[rail_index_ptr->sw_mode_level_index].data = sw_mode;

  return ret_val;

}/* pm_mpm_cmd_rail */


/* ============================================================================
**  Function : pm_mpm_cmd_max_delay
** ============================================================================
*/
/*!
    Program the maximum delay needed at the end of active sequence where CX
    voltage is programmed.

   @retval PM_ERR_FLAG__SUCCESS on success else PMIC error code.

*/
static pm_err_flag_type pm_mpm_cmd_max_delay(void)
{
  pm_err_flag_type ret_val = PM_ERR_FLAG__SUCCESS;

  /* Configure max delay at the end of active config */
  if (pm_mpm_max_delay != 0)
  {
    active_cfg[PM_MPM_ACTIVE_CMD_END - 1].delay = pm_mpm_max_delay;
  }
  else
  {
    /* return error if pm_mpm_max_delay is 0 */
    ret_val = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  return ret_val;

} /* pm_mpm_cmd_max_delay */

/*----------------------------------------------------------------------------
 * Function : pm_mpm_cmd_init
 * -------------------------------------------------------------------------*/
/*!
   Get DAL device configuration. This code runs once.
     @dependencies
     pm_target_information_get_target_info
     @return
    PM_ERR_FLAG__SUCCESS success otherwise PMIC error.
*/
void pm_mpm_cmd_init (void)
{

  /* Init only happen once */
  if ( active_cfg && sleep_cfg )
  {
    return;
  }

  /* Get command device cfgs. */
  active_cfg = (pm_mpm_cfg_type*)pm_target_information_get_specific_info(PM_PROP_MPM_ACTIVE_CMDS);
  sleep_cfg =  (pm_mpm_cfg_type*)pm_target_information_get_specific_info(PM_PROP_MPM_SLEEP_CMDS);

  /* Get command index. */
  cmd_index_ptr = (pm_mpm_cmd_index_type*)pm_target_information_get_specific_info(PM_PROP_MPM_CMD_INDICES);

  /* MPM rails info. */
  pm_mpm_rails = (pm_mpm_rails_info_type*)pm_target_information_get_specific_info(PM_PROP_MPM_RAILS_INFO);

} /* pm_mpm_cmd_init */

/*----------------------------------------------------------------------------
 * Function : pm_mpm_rail_cmd
 * -------------------------------------------------------------------------*/
/*!
    Description: Return a pointer to a config data buffer for MPM voltage rail
                 command.
    @param
      mpm_cfg_ptr: pointer to the data structure that pointer to
                   the buffer which contains config data
                   for retention and active voltage.
    @return
    PM_ERR_FLAG__SUCCESS success otherwise PMIC error.

    @dependencies
    railway_get_corner_voltage()
    pm_rpm_calculate_vset()


    @sa None
*/
pm_err_flag_type pm_mpm_cmd_config
(
  pm_mpm_cmd_cfg_type * mpm_cmd_ptr
)
{
  pm_err_flag_type ret_val = PM_ERR_FLAG__SUCCESS;

  /* Missing call to pm_init API. */
  if ( (NULL == active_cfg) || (NULL == sleep_cfg) || 
       (NULL == cmd_index_ptr) || (NULL == pm_mpm_rails) )
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Initialize max delay */
  pm_mpm_max_delay = 0;

  /* CX rail config data for this request. */
  if ((ret_val = pm_mpm_cmd_rail(mpm_cmd_ptr->cx_sleep_microvolts, mpm_cmd_ptr->cx_active_microvolts, 
                                 PM_MPM_CX_INDEX)) != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* MX rail config data for this request. */
  if ((ret_val = pm_mpm_cmd_rail(mpm_cmd_ptr->mx_sleep_microvolts, mpm_cmd_ptr->mx_active_microvolts, 
                                 PM_MPM_MX_INDEX)) != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* MPM target based commands config. */
  if ((ret_val = pm_mpm_target_cmd_config(mpm_cmd_ptr, sleep_cfg, active_cfg, pm_mpm_rails, cmd_index_ptr, 
                                          &pm_mpm_max_delay)) != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* Configure maximum delay value */
  if ((ret_val = pm_mpm_cmd_max_delay()) != PM_ERR_FLAG__SUCCESS)
  {
    return ret_val;
  }

  /* Return pointer to config data. */
  mpm_cmd_ptr->sleep_cfg_ptr = sleep_cfg;
  mpm_cmd_ptr->active_cfg_ptr = active_cfg;

  return ret_val;

} /*  pm_mpm_cmd_config */











