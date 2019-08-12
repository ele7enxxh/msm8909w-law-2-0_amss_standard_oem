/*! \file pm_mpm_volt_rail_cmd_cfg.c
*  \n
*  \brief This file contains pmic configuration data specific for mpm shutdown/wakeup voltage rail cmds.
*  \n
*  \n &copy; Copyright 2014-2015 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/mdm9x45/pm_mpm_volt_rail_cmd_cfg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/14   vtw     Updated with 8994 MPM sequence settings.
10/31/13   vtw     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm.h"
#include "pm_mpm_internal.h"
#include "pm_mpm_target.h"


/**
  * MPM PMIC sleep register settings.
  */
pm_mpm_cfg_type
pm_mpm_sleep_cfg[] =
{
  [PM_MPM_SLEEP_CMD_CX_VOLT]         = {0x1, 0x02341, 0x0, 0x0}, /* Set VDD_CX retention. */
  [PM_MPM_SLEEP_CMD_MX_VOLT]         = {0x1, 0x04841, 0x0, 0x0}, /* Set VDD_MX retention */
  [PM_MPM_SLEEP_CMD_PBS_TRIG]        = {0xE, 0x07A42, 0x1, 0x6}, /* Program PBS SW trigger. */
  [PM_MPM_SLEEP_CMD_END]             = {0x0, 0x0,     0x0, 0x0}, /* End of sleep sequence. */
};

/**
  * MPM PMIC active register settings.
  */
pm_mpm_cfg_type
pm_mpm_active_cfg[] =
{
  [PM_MPM_ACTIVE_CMD_PBS_TRIG]          = {0xE, 0x07A42, 0x1, 0x6},  /* Program PBS SW trigger. */
  [PM_MPM_ACTIVE_CMD_MX_PARENT_MODE]    = {0x1, 0x01A45, 0x0, 0x0},  /* Retore MX PARENT to mode before sleep. */
  [PM_MPM_ACTIVE_CMD_MX_MODE]           = {0x1, 0x04845, 0x0, 0x0},  /* Retore MX to mode before sleep. */
  [PM_MPM_ACTIVE_CMD_CX_MODE]           = {0x1, 0x02345, 0x0, 0x0},  /* Retore CX to mode before sleep. */
  [PM_MPM_ACTIVE_CMD_MX_VOLT]           = {0x1, 0x04841, 0x0, 0x0},  /* Turn on MX with active voltage.  */
  [PM_MPM_ACTIVE_CMD_CX_VOLT]           = {0x1, 0x02341, 0x0, 0x0},  /* Turn on CX with active voltage.  */
  [PM_MPM_ACTIVE_CMD_END]               = {0x0, 0x0,     0x0, 0x0},  /* End active sequence. */  
};

/**
  * MPM cmd index buffer.
  */
const pm_mpm_cmd_index_type pm_mpm_cmd_index[] =
{
                             /* rail retention index, rail active index, rail sw mode before sleep */
  [PM_MPM_CX_INDEX]        = {PM_MPM_SLEEP_CMD_CX_VOLT, PM_MPM_ACTIVE_CMD_CX_VOLT, PM_MPM_ACTIVE_CMD_CX_MODE},
  [PM_MPM_MX_INDEX]        = {PM_MPM_SLEEP_CMD_MX_VOLT, PM_MPM_ACTIVE_CMD_MX_VOLT, PM_MPM_ACTIVE_CMD_MX_MODE},
  [PM_MPM_MX_PARENT_INDEX] = {PM_MPM_INVALID_CMD, PM_MPM_INVALID_CMD, PM_MPM_ACTIVE_CMD_MX_PARENT_MODE},
};

/**
  * MPM rails info.
  */
pm_mpm_rails_info_type pm_mpm_rails_info[] =
{
                             /* resource_type, resource_index, sleep_headroom_uv */
  [PM_MPM_CX_INDEX]        = {RPM_SMPS_A_REQ, 5, 0},
  [PM_MPM_MX_INDEX]        = {RPM_LDO_A_REQ, 9, 25000},
  [PM_MPM_MX_PARENT_INDEX] = {RPM_SMPS_A_REQ, 3, 0}, 
};
