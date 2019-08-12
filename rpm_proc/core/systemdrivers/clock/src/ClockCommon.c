/*
===========================================================================
*/
/**
  @file ClockCommon.c

  Common APIs to support between multilple chipset
*/
/*
  ====================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/src/ClockCommon.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "swevent.h"
#include "ClockCommon.h"
#include "Chipinfo.h"

/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_SetClockConfigEx
** =========================================================================*/
/**
  Extend function for set a clock configuration with QDSS clock event

  This function configures a clock domain to a particular configuration.
  It is used internally only.

  @param *pClock [in]     -- Clock to configure.
  @param *pNewConfig [in] -- New configuration to use.

  @return
  None

  @dependencies
  The clock mutex must be locked.
*/
void Clock_SetClockConfigEx
(
  ClockNodeType       *pClock,
  ClockMuxConfigType  *pNewConfig
)
{
  /* Call to Clock_SetClockConfig and assert if fails */
  assert( Clock_SetClockConfig(pClock->pDomain, pNewConfig) == DAL_SUCCESS );

  /* Log clock frequency */
  SWEVENT( CLOCK_EVENT_FREQ,
           HAL_clk_GetTestClockId(pClock->HALHandle),
           pNewConfig->nFreqHz );
}
