/*=========================================================================

                         Busywait Hardware QTIMER

GENERAL DESCRIPTION
  This file contains the implementation of the busywait() function for
  hardware based blocking waits that use the QTIMER as a
  reference.

EXTERNALIZED FUNCTIONS
  busywait

INITIALIZATION AND SEQUENCING REQUIREMENTS
 None

      Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/busywait/src/busywait_hw_qtimer.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $
  $Change: 8935858 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
11/23/11   pbitra     Initial version.

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

/*=========================================================================
      Variables
==========================================================================*/
  static uint32                           QTimerBase = 0;

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
  static DALSYSPropertyVar system_property_var; 
  /*declare property handle variable */
  static DALSYS_PROPERTY_HANDLE_DECLARE(busywait_property); 

  uint32  QTimerFrame = 0;
  uint32  QTimerACBase = 0;

  if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr("QTimer",busywait_property))
  {
      return DAL_ERROR;
  }
  DALSYS_GetPropertyValue(busywait_property,
                                 "QTIMER_BASE",
                         0,&system_property_var); 
  QTimerBase = system_property_var.Val.dwVal;

  DALSYS_GetPropertyValue(busywait_property,
                                 "QTIMER_FRAME",
                         0,&system_property_var); 
  QTimerFrame = system_property_var.Val.dwVal;

  DALSYS_GetPropertyValue(busywait_property,
                                 "QTIMER_AC_BASE",
                         0,&system_property_var); 
  QTimerACBase = system_property_var.Val.dwVal;

  HWIO_QTMR_AC_CNTACR_n_OUTI(QTimerACBase,QTimerFrame,0x3F);
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

  if (QTimerBase != NULL )
  {
    start_count = in_dword(QTimerBase);
    /*
     * Perform the delay and handle potential overflows of the timer.
     */
  
    delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ)/1000;
    while ((in_dword(QTimerBase) - start_count) < delay_count);
  }
}

