/*=========================================================================

          Busywait Hardware QTIMER with address from sconscript

GENERAL DESCRIPTION
  This file contains the implementation of the busywait() function for
  hardware based blocking waits that use the QTIMER as a reference.
  The QTIMER address and QTIMER_AC address are acquired from the scons.

EXTERNALIZED FUNCTIONS
  busywait

INITIALIZATION AND SEQUENCING REQUIREMENTS
 None

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/busywait/src/busywait_qtimer_nondal.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $
  $Change: 8935858 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
26/07/12   pbitra     Initial version.

==========================================================================*/ 

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "DALSys.h"
#include "HALhwio.h"
#include "TimerHWIO.h"

/*==========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==========================================================================*/

/*=========================================================================
      Constants and Macros 
==========================================================================*/

#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 19200

/*==========================================================================

                         FUNCTIONS FOR MODULE

==========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT_INIT

  DESCRIPTION   This function gets the QTimer addresses from the
                properties file and programs the access control register
                to provide access to the QTimer.

  PARAMETERS    None

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None

==========================================================================*/

DALResult busywait_init(void)
{

  HWIO_QTMR_AC_CNTACR_n_OUTI(QTIMER_AC_ADDR,QTIMER_FRAME,0x3F);
  
  return DAL_SUCCESS;

}

/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  busywait_init should have already been called once to
                initialize the busywait driver.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void busywait (uint32 pause_time_us)
{
  uint32 start_count = 0;
  uint64 delay_count = 0;

  if (QTIMER_ADDR != NULL )
  {
    start_count = in_dword(QTIMER_ADDR);
    /*
     * Perform the delay and handle potential overflows of the timer.
     */
  
    delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ)/1000;
    while ((in_dword(QTIMER_ADDR) - start_count) < delay_count);
  }
}

