#ifndef CLK_H
#define CLK_H

/*===========================================================================

       C L O C K   S E R V I C E S   H E A D E R    F I L E

GENERAL DESCRIPTION
  The clock services component has been deprecated.  This header file
  contains limited functionality for accessing critical clock services until
  the transition to new components is complete.
  
EXTERNALIZED FUNCTIONS
  clk_busy_wait
  clk_set_mclk_parms
  clk_set_pause_timer_freq
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/clk/inc/clk.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
07/28/10   cmd     Ported from clk/rel/1.0 branch.

===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"
#include "DALSys.h"


/*==========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==========================================================================*/

/*===========================================================================
      Constants & Macros & Typedefs
===========================================================================*/


/*==========================================================================

                         FUNCTIONS FOR MODULE

==========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==========================================================================

  FUNCTION      CLK_BUSY_WAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

                NOTE: This function has been deprecated.  All code should
                now call DALSYS_BusyWait().

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

#define clk_busy_wait(pause_time_us) DALSYS_BusyWait(pause_time_us)


/*==========================================================================

  FUNCTION      BUSYWAIT_CONFIG_CPU_FREQ

  DESCRIPTION   This function is called to notify the busywait component of
                changes in the CPU frequency.

  PARAMETERS    freq_khz - CPU frequency in kHz

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The global variable for CPU frequency will be updated.

==========================================================================*/

extern void busywait_config_cpu_freq (uint32 freq_khz);


/*===========================================================================

  FUNCTION      CLK_SET_MCLK_PARMS

  DESCRIPTION   This function updates the clk_mclk_freq_mhz global variable.

                NOTE: This function has been deprecated.  All code should now
                call busywait_config_cpu_freq().

  PARAMETERS    freq_mhz - New mARM clock speed.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The clk_mclk_freq_mhz variable is updated.

===========================================================================*/

#define clk_set_mclk_parms(freq_mhz) busywait_config_cpu_freq(freq_mhz * 1000)


/*==========================================================================

  FUNCTION      BUSYWAIT_CONFIG_PAUSE_TIMER_FREQ

  DESCRIPTION   This function is called to notify the busywait component of
                changes in the pause timer frequency.

  PARAMETERS    freq_khz - Pause timer frequency in kHz

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The global variable for pause timer frequency will be
                updated.

==========================================================================*/

extern void busywait_config_pause_timer_freq (uint32 freq_khz);


/*===========================================================================

  FUNCTION      CLK_SET_PAUSE_TIMER_FREQ

  DESCRIPTION   This function updates the pause timer frequency variable.
                This variable is used to calculate how many clock period to
                wait.

  PARAMETERS    freq_mhz - Pause timer frequency.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The clk_pause_freq_mhz variable is updated.

===========================================================================*/

#define clk_set_pause_timer_freq(freq_mhz) busywait_config_pause_timer_freq(freq_mhz * 1000)


#ifdef __cplusplus
}
#endif

#endif /* CLK_H */

