/*
===========================================================================

FILE:         HALmpmVDDCommands.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM VDD commands.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/common/HALmpmVDDCommands.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <stdlib.h>
#include "CoreVerify.h"
#include "HALhwio.h"
#include "mpm2_hwio.h"
#include "HALmpm.h"
#include "pm_mpm.h"

#include "../common/HALmpmInternal.h"

extern HAL_mpm_PlatformType mpmPlatform;

static volatile uint32 num_sleep_commands = 0;
static volatile uint32 num_active_commands = 0;

/* -----------------------------------------------------------------------
**                           STATIC FUNCTIONS
** ----------------------------------------------------------------------- */

// SPMI Delay table from MPM2 HPG
// 4bit   # XO cycles     Delay in usec
// 0000   0               0
// 0001   192             10
// 0010   480             25
// 0011   960             50
// 0100   1920            100
// 0101   2880            150
// 0110   3840            200
// 0111   4800            250
// 1000   5760            300
// 1001   6720            350
// 1010   7680            400
// 1011   9600            500
// 1100   11520           600
// 1101   13440           700
// 1110   16320           850
// 1111   19200           1000
static uint16 delayTable[16]      = {0, 10, 25, 50, 100, 150, 200, 250, 300, 350, 400, 500, 600, 700, 850, 1000};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

void HAL_mpm_CfgVoltageCommands(uint32_t offMxMicrovolts, uint32_t onMxMicrovolts, 
                                uint32_t offCxMicrovolts, uint32_t onCxMicrovolts)
{
  //pmic structures needed to facilitate SPMI sequence retrieval
  pm_mpm_cmd_cfg_type mpm_cmds = {NULL, NULL, 0, 0, 0, 0};
  pm_mpm_cfg_type * sleep_cmds = NULL;
  pm_mpm_cfg_type * active_cmds = NULL;
  pm_err_flag_type ret_val;

  uint32 reg_val;
  uint32 num_sleep_commands = 0;
  uint32 num_active_commands = 0;

  uint32 on_cfg_addr   = HWIO_ADDR(MPM2_MPM_PMIC_VDD_ON_CFG_1);
  uint32 off_cfg_addr  = HWIO_ADDR(MPM2_MPM_PMIC_VDD_OFF_CFG_1);
  
  //get sequences from pmic
  mpm_cmds.mx_sleep_microvolts  = offMxMicrovolts;
  mpm_cmds.mx_active_microvolts = onMxMicrovolts;
  mpm_cmds.cx_sleep_microvolts  = offCxMicrovolts;
  mpm_cmds.cx_active_microvolts = onCxMicrovolts;
  ret_val = pm_mpm_cmd_config(&mpm_cmds);

  CORE_VERIFY(ret_val == PM_ERR_FLAG__SUCCESS);

  sleep_cmds = mpm_cmds.sleep_cfg_ptr;
  active_cmds = mpm_cmds.active_cfg_ptr;


  //set sleep commands
  while((num_sleep_commands < 12) && ((((sleep_cmds) + num_sleep_commands)->slave_id != 0) || 
                                       (((sleep_cmds) + num_sleep_commands)->slave_addr != 0) ||
                                       (((sleep_cmds) + num_sleep_commands)->data != 0) ||
                                       (((sleep_cmds) + num_sleep_commands)->delay != 0))) 
  {
    reg_val =  ((sleep_cmds) + num_sleep_commands)->data;
    reg_val |= (((sleep_cmds) + num_sleep_commands)->slave_addr << 8);
    reg_val |= (((sleep_cmds) + num_sleep_commands)->slave_id << 24);
    reg_val |= (((sleep_cmds) + num_sleep_commands)->delay << 28);

    out_dword((off_cfg_addr + (num_sleep_commands << 2)), reg_val);
    num_sleep_commands++;
  }

  //set active commands
  while((num_active_commands < 12) && ((((active_cmds) + num_active_commands)->slave_id != 0) || 
                                       (((active_cmds) + num_active_commands)->slave_addr != 0) ||
                                       (((active_cmds) + num_active_commands)->data != 0) ||
                                       (((active_cmds) + num_active_commands)->delay != 0))) 
  {

    reg_val =  ((active_cmds) + num_active_commands)->data;
    reg_val |= (((active_cmds) + num_active_commands)->slave_addr << 8);
    reg_val |= (((active_cmds) + num_active_commands)->slave_id << 24);
    reg_val |= (((active_cmds) + num_active_commands)->delay << 28);

    out_dword((on_cfg_addr + (num_active_commands << 2)), reg_val);

    num_active_commands++;
  }

  // Set num commands
  if (num_sleep_commands)
    HWIO_OUTF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_OFF, (num_sleep_commands - 1));
  if (num_active_commands)
    HWIO_OUTF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_ON,  (num_active_commands - 1));
}

uint32 HAL_mpm_GetVddMinEnterLatency(void)
{
  uint32 off_cfg = HWIO_ADDR(MPM2_MPM_PMIC_VDD_OFF_CFG_1);
  uint32 shift   = HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_1, DELAY);
  uint32 delay   = 0;
  uint32 num_cmds, val;
  uint32 i;

  // Get # of VDD_OFF_CFG commands
  num_cmds = HWIO_INF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_OFF) + 1;
  for (i = 0; i < num_cmds; i++)
  {
    val = *((uint32 *)(off_cfg + (i << 2)));
    delay += delayTable[(val >> shift)];
  }

  // Convert usec into xo timeticks
  delay = (delay * 19200) / 1000;

  return delay;
}

uint32 HAL_mpm_GetVddMinExitLatency(void)
{
  uint32 on_cfg  = HWIO_ADDR(MPM2_MPM_PMIC_VDD_ON_CFG_1);
  uint32 shift   = HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, DELAY);
  uint32 delay   = 0;
  uint32 num_cmds, val;
  uint32 i;

  // Get # of VDD_ON_CFG commands
  num_cmds = HWIO_INF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_ON) + 1;
  for (i = 0; i < num_cmds; i++)
  {
    val = *((uint32 *)(on_cfg + (i << 2)));
    delay += delayTable[(val >> shift)];
  }

  // Convert usec into xo timeticks
  delay = (delay * 19200) / 1000;

  // Add CXO delay (in sclks so convert this to xo ticks as well)
  delay += (mpmPlatform.mpmCfg->mpmCfg.wakeupDelays.cxoDelay * 586);

  delay += (HWIO_INF(MPM2_MPM_CXO_POWER_RAMPUP_TIME, POWER_RAMPUP_TIME) * 586);

  // Add unknown 600us
  delay += 11520;

  return delay;
}

