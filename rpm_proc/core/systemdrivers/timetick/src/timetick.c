/*
===========================================================================
*/
/**
  @file timetick.c

  Interface implementation file for the simple timetick driver.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/timetick/src/timetick.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "timetick.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "HALhwio.h"
#include "QTimerHWIO.h"


/*=========================================================================
      Variables
==========================================================================*/

static uint32 timetick_qtimer_base = 0;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : timetick_init
** =========================================================================*/

void timetick_init (void)
{
  DALSYSPropertyVar system_property_var; 
  DALResult result;
  DALSYS_PROPERTY_HANDLE_DECLARE(timetick_property); 
  uint32 qtimer_ac_base;

  /*
   * Get the timetick property handle.
   */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_TIMETICK, timetick_property);

  /*
   * Get the QTimer access control base address from timetick properties
   */
  result =
    DALSYS_GetPropertyValue(
      timetick_property, "QTIMER_AC_BASE", 0, &system_property_var); 

  /*
   * Enable all access to the timer.
   */
  if (result == DAL_SUCCESS)
  {
    qtimer_ac_base = system_property_var.Val.dwVal;
    HWIO_QTMR_AC_CNTACR_n_OUTI(qtimer_ac_base, 1, 33);
  }

  /*
   * Get the QTimer base address from timetick properties
   */
  result =
    DALSYS_GetPropertyValue(
      timetick_property, "QTIMER_BASE", 0, &system_property_var); 

  if (result == DAL_SUCCESS)
  {
    timetick_qtimer_base = system_property_var.Val.dwVal;
  }

} /* END timetick_init */


/* =========================================================================
**  Function : timetick_get
** =========================================================================*/

timetick_type timetick_get (void)
{
  return HWIO_QTMR_CNTPCT_LO_IN(timetick_qtimer_base);

} /* END timetick_get */


/* =========================================================================
**  Function : timetick_get_safe
** =========================================================================*/

timetick_type timetick_get_safe (void)
{
  return timetick_get();

} /* END timetick_get_safe */


/* =========================================================================
**  Function : timetick_get64
** =========================================================================*/

timetick_type64 timetick_get64 (void)
{
   uint32   nCountLow, nCountHi1,nCountHi2;
   timetick_type64 ticks;

   do
   {
     nCountHi1 = HWIO_QTMR_CNTPCT_HI_IN(timetick_qtimer_base);
     nCountLow = HWIO_QTMR_CNTPCT_LO_IN(timetick_qtimer_base);

     /* repeat the above until the same value is read from the   
       high count register successively */
     nCountHi2 = HWIO_QTMR_CNTPCT_HI_IN(timetick_qtimer_base);
   }while (nCountHi1 != nCountHi2);

   ticks = (((timetick_type64)nCountHi1 <<32)| nCountLow);
   return ticks;
} /* END timetick_get64 */


/* =========================================================================
**  Function : timetick_get_freq
** =========================================================================*/

uint32 timetick_get_freq(void)
{
   return HWIO_QTMR_CNTFRQ_IN(timetick_qtimer_base);
}
