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
 
  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/busywait/src/busywait.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
  $Change: 7538753 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
11/23/11   pbitra     Initial version.

==========================================================================*/ 

/*=========================================================================
      Include Files
==========================================================================*/

#include "DALSys.h"
#include "DALDeviceId.h"
#include "HALhwio.h"
#include "QTimerHWIO.h"


/*=========================================================================
      Constants and Macros 
==========================================================================*/

#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 19200


/*=========================================================================
      Variables
==========================================================================*/

static uint32 busywait_qtimer_base = 0;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : busywait_init
** =========================================================================*/

void busywait_init (void)
{
  DALSYSPropertyVar system_property_var; 
  DALResult  result = DAL_SUCCESS;
  DALSYS_PROPERTY_HANDLE_DECLARE(timetick_property); 

  /* Get the QTimer Base address from timetick properties */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_TIMETICK,timetick_property);
  result = DALSYS_GetPropertyValue(timetick_property,
                                   "QTIMER_BASE",
                                   0, &system_property_var); 
  if (result == DAL_SUCCESS)
  {
    busywait_qtimer_base = system_property_var.Val.dwVal;
  }

} /* END busywait_init */


/* =========================================================================
**  Function : busywait
** =========================================================================*/

void busywait (uint32 pause_time_us)
{
  uint32 start_count = 0;
  uint64 delay_count = 0;

  if (busywait_qtimer_base != 0)
  {
    start_count = HWIO_QTMR_CNTPCT_LO_IN(busywait_qtimer_base);
    delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ) / 1000;
    while ((HWIO_QTMR_CNTPCT_LO_IN(busywait_qtimer_base) - start_count) < delay_count);
  }

} /* END busywait */

