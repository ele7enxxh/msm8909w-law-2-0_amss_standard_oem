/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

           M U L T I P R O C E S S O R   P O W E R   M A N A G E R

GENERAL DESCRIPTION

  This module contains routines for the multiprocessor power manager (MPM).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  mpm_init( ) must be called once, before any other mpm_ function.



             Copyright (c) 2005-2009 by Qualcomm Technologies, Inc.  
                               All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/src/mpm.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
--------   ---   -----------------------------------------------------------
4/5/12     mda    Initial MPMv2 

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "mpm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "HALmpm.h"
#include "CoreVerify.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/* Active and sleep voltages to be programmed into the MPM HW. */
static uint32_t mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_COUNT];
static uint32_t mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_COUNT];

/*=============================================================================

FUNCTION MPM_CONFIG_RAIL_FOR_SLEEP

DESCRIPTION
  This function is used to program the MPM with the settings required to put
  the core voltage rail to sleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void mpm_config_rails_for_sleep(void) 
{
  CORE_VERIFY(mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_VDD_MEM] <= mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_VDD_MEM]);
  CORE_VERIFY(mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_VDD_DIG] <= mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_VDD_DIG]);

  /* Program the HW with the rail's information. */
  HAL_mpm_CfgVoltageRailSleep(mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_VDD_MEM], mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_VDD_MEM],
                              mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_VDD_DIG], mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_VDD_DIG]);

} /* mpm_config_rails_for_sleep */

/*============================================================================

FUNCTION MPM_INIT

DESCRIPTION
  This function initializes the MPM registers to known values.

DEPENDENCIES
  Must be called with the I bit set to prevent timing issues.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void mpm_init (void)
{
  char **ppszVersion    = NULL;

  HAL_mpm_Init( ppszVersion );

  HAL_mpm_EnableCXO();

} /* mpm_init */

void mpm_init_xtal(void)
{
    HAL_mpm_Init_Xtal_Latency();
}

void mpm_config_ebi1_freeze_io(bool enable)
{
  HAL_mpm_CfgEbi1SwCtl(enable);
}

void mpm_config_freeze_ios(bool enable)
{
  HAL_mpm_CfgFreezeIOs(enable);
}

void mpm_setup_chip_sleep()
{
  // configure CXO for sleep
  HAL_mpm_CfgCXOSleep();

  mpm_config_rails_for_sleep();
}

void mpm_setup_sleep_timer(uint64_t duration_sclk)
{
  HAL_mpm_SetWakeUpTime(duration_sclk);
}

void mpm_teardown_sleep (void)
{
  /* Clear out the hardware. */
  HAL_mpm_ClearSleepModes();
}

/*=============================================================================


FUNCTION MPM_SET_VMIN_VOLTAGE

DESCRIPTION
  This function is used to tell the MPM driver what the voltages are for 
  entering vdd min and exiting vdd min of a

PARAMETERS
  mx_off_uv - vdd min MX voltage level
  mx_on_uv - vdd min exit MX voltage level
  cx_off_uv - vdd min CX voltage level
  cx_on_uv - vdd min exit CX voltage level

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void mpm_set_vmin_voltages
(
  uint32_t mx_off_uv, uint32_t mx_on_uv, uint32_t cx_off_uv, uint32_t cx_on_uv
)
{

  /* Record the voltages to be used for vdd_min (off) and for wakeup after vdd_min (on) */
  mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_VDD_MEM] = mx_off_uv;
  mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_VDD_MEM] = mx_on_uv;

  mpm_sleep_voltages_uv[MPM_VOLTAGE_RAIL_VDD_DIG] = cx_off_uv;
  mpm_active_voltages_uv[MPM_VOLTAGE_RAIL_VDD_DIG] = cx_on_uv;
}

/*=============================================================================

FUNCTION MPM_GET_TIMETICK

DESCRIPTION
  Read the MPM TIMETICK counter, and return the current count. NOTE:
  the counter tracks time in slow clock counts, from 0 to 0xFFFFFFFF 
  (~1.5 days)

DEPENDENCIES
  Must be called from an INTLOCK'd context.

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/
uint64_t mpm_get_timetick( void )
{
  /* Sleep Xtal Time Tick count */
  uint64_t                   tick;

  HAL_mpm_GetCurrentSclkCount( &tick );

  return tick;

} /* mpm_get_timetick */

uint64_t mpm_deep_sleep_enter_latency()
{
  return (uint64_t)HAL_mpm_GetDeepSleepEnterLatency();
}

uint64_t mpm_deep_sleep_exit_latency()
{
  return (uint64_t)HAL_mpm_GetDeepSleepExitLatency();
}

void mpm_sw_done( uint64_t sleep_duration )
{
  HAL_mpm_SWDone(sleep_duration);
}

