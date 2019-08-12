#ifndef QFPROM_SUBSYS_H
#define QFPROM_SUBSYS_H

/*===========================================================================

                       QFPROM  Driver Source  Code

DESCRIPTION
 Contains Subsystem specific defintions and APIs to setup clock/pmic.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/hw/core_2_0/qfprom_subsys.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/12   kedara  Initial revision for 9x25

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/



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
uint32 qfprom_clock_enable( void );


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
uint32 qfprom_clock_disable( void );

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
uint32 qfprom_write_set_voltage_dependent_settings( void );

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
uint32 qfprom_write_reset_voltage_dependent_settings( void );

#endif /* QFPROM_SUBSYS_H */
