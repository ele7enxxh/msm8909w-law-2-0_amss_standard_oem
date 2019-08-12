/*===========================================================================

                       QFPROM  Driver Source  Code

DESCRIPTION
 Contains trustzone specific defintions and APIs to setup clock/pmic
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/hw/core_2_0/qfprom_tz.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/12   kedara  Initial revision for 9x25

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include QFPROM_TARGET_INCLUDE_H
#include "clk.h"
#include "pm.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
                                 LOCAL FUNCTION DEFINITIONS
=============================================================================*/


/*=============================================================================
                                 GLOBAL FUNCTION DEFINITIONS
=============================================================================*/


/*===========================================================================

FUNCTION  QFPROM_CLOCK_ENABLE

DESCRIPTION
  Enable/setup clock for qfprom fuse block.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while enabling qfprom block clock.

SIDE EFFECTS
  Turns on qfprom clock.

===========================================================================*/
uint32 qfprom_clock_enable( void )
{
  uint32 ret_val = QFPROM_NO_ERR;
   
  HAL_qfprom_EnableClock();

  return ret_val;
}

/*===========================================================================

FUNCTION  QFPROM_CLOCK_DISABLE

DESCRIPTION
  Disables /Restores to previous state the clock for qfprom fuse block

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while disabling qfprom block clock.

SIDE EFFECTS
  Disables /Restores to previous state the clock for qfprom fuse block.

===========================================================================*/
uint32 qfprom_clock_disable( void )
{
  uint32 ret_val = QFPROM_NO_ERR;
 
  HAL_qfprom_DisableClock();

  return ret_val;
}


/*===========================================================================

FUNCTION  qfprom_write_set_voltage_dependent_settings

DESCRIPTION
    This function shall set the appropriate voltage dependent settings for the qfprom block.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Turns on qfprom clock and enables VREG_GP5.

===========================================================================*/
uint32 qfprom_write_set_voltage_dependent_settings( void )
{
    uint32 ret_val = QFPROM_NO_ERR;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    /* Voltage is applied enable the logic to write.
      Set QFPROM_BLOW_VDD to 1.9v by turning on switch
      LOW_VOLTAGE_SWITCH_LVS_1_CONTROL.
      When pmic lib is available [when porting it to other images/modules],
      its generic api's should be used to subsitute below api.
      */
    errFlag = pm_enable_qfuse_power_rail();
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      ret_val = QFPROM_FAILED_TO_CHANGE_VOLTAGE_ERR;
    }

    return ret_val;
}


/*===========================================================================

FUNCTION  qfprom_write_reset_voltage_dependent_settings

DESCRIPTION
    This function shall reset the appropriate voltage dependent settings for the qfprom block.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Disables the vreg being used.

===========================================================================*/
uint32 qfprom_write_reset_voltage_dependent_settings( void )
{
    uint32 ret_val = QFPROM_NO_ERR;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    /*Disable power to the qfprom block  by turning off switch
           LOW_VOLTAGE_SWITCH_LVS_1_CONTROL*/
    errFlag = pm_disable_qfuse_power_rail();

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      ret_val = QFPROM_FAILED_TO_CHANGE_VOLTAGE_ERR;
    }
    /* Apply enough wait time here to stabilise the voltage */
    clk_busy_wait(1000);

    return ret_val;
}

