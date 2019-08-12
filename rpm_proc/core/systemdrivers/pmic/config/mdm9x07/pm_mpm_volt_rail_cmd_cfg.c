/*! \file
*  \n
*  \brief  pm_mpm_volt_rail_cmd_cfg.c
*  \n
*  \n This file contains pmic configuration data specific for PM8994 mpm shutdown/wakeup
*  \n voltage rail cmds.
*  \n
*  \n &copy; Copyright 2014-2015 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/mdm9x07/pm_mpm_volt_rail_cmd_cfg.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/15   sv      Updated mpm sleep config settings. (CR-902375)
08/01/15   ps      Added support for 9x07 target (CR-883336)
03/16/15   akt     Update the S3 retension for any MX voltage (CR-804591)
01/28.15   akt     Update the retension voltage for S3 and S4 (CR-779395)
12/12/14   akt     Update the PBS trigger to PBS_CLIENT4 in 8909, MX parent
02/12/14   vtw     Updated with 8994 MPM sequence settings.
10/31/13   vtw     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm.h"
#include "pm_mpm_internal.h"
#include "pm_mpm_target.h"

/*===========================================================================
Data Declarations
===========================================================================*/

/**
  * MPM PMIC sleep register settings, set dynamically by railway driver.
  */
pm_mpm_cfg_type
pm_mpm_sleep_cfg[] =
{
  [PM_MPM_SLEEP_CMD_CX_VOLT]         = {0x1, 0x01A41, 0x0, 0x0}, /* Set VDD_CX (S3)  */
  [PM_MPM_SLEEP_CMD_MX_VOLT]         = {0x1, 0x04B41, 0x0, 0x0}, /* Set VDD_MX (L12) */
  [PM_MPM_SLEEP_CMD_MX_PARENT_VOLT]  = {0x1, 0x01741, 0x0, 0x0}, /* Set VDD_MX PARENT (S2) */
  [PM_MPM_SLEEP_CMD_PBS_TRIG]        = {0xE, 0x07A42, 0x1, 0x6}, /* Program PBS SW trigger - PBS 9. */
  [PM_MPM_SLEEP_CMD_END]             = {0x0, 0x0,     0x0, 0x0}, /* End of sleep sequence. */
};

/**
  * MPM PMIC active register settings, set dynamically by railway driver.
  */
pm_mpm_cfg_type
pm_mpm_active_cfg[] =
{
  [PM_MPM_ACTIVE_CMD_PBS_TRIG]          = {0xE, 0x07A42, 0x1, 0x6},  /* Program PBS SW trigger. */
  [PM_MPM_ACTIVE_CMD_MX_PARENT_VOLT]    = {0x1, 0x01741, 0x0, 0x6},  /* Turn on MX PARENT with active voltage.  */
  [PM_MPM_ACTIVE_CMD_MX_VOLT]           = {0x1, 0x04B41, 0x0, 0x0},  /* Turn on MX with active voltage.  */
  [PM_MPM_ACTIVE_CMD_CX_VOLT]           = {0x1, 0x01A41, 0x0, 0x0},  /* Turn on CX with active voltage.  */
  [PM_MPM_ACTIVE_CMD_END]               = {0x0, 0x0,     0x0, 0x0},  /* End active sequence. */  
};

/**
  * MPM cmd index buffer.
  */
const pm_mpm_cmd_index_type pm_mpm_cmd_index[] =
{
                             /* rail retention index, rail active index, rail sw mode before sleep */
  [PM_MPM_CX_INDEX]        = {PM_MPM_SLEEP_CMD_CX_VOLT, PM_MPM_ACTIVE_CMD_CX_VOLT, PM_MPM_INVALID_CMD},
  [PM_MPM_MX_INDEX]        = {PM_MPM_SLEEP_CMD_MX_VOLT, PM_MPM_ACTIVE_CMD_MX_VOLT, PM_MPM_INVALID_CMD},
  [PM_MPM_MX_PARENT_INDEX] = {PM_MPM_SLEEP_CMD_MX_PARENT_VOLT, PM_MPM_ACTIVE_CMD_MX_PARENT_VOLT, PM_MPM_INVALID_CMD},
};

/**
  * MPM rails info.
  */
pm_mpm_rails_info_type pm_mpm_rails_info[] =
{
                             /* resource_type, resource_index, sleep_headroom_uv */
  [PM_MPM_CX_INDEX]        = {RPM_SMPS_A_REQ, 3, 0},
  [PM_MPM_MX_INDEX]        = {RPM_LDO_A_REQ, 12, 25000},   /* This value is active headroom for MX to be used - if MX+HR > xxx, apply MX+HR */
  [PM_MPM_MX_PARENT_INDEX] = {RPM_SMPS_A_REQ, 2, 1200000}, /* This value is hardcoded to have MIN of 1.2v at retension */
};
