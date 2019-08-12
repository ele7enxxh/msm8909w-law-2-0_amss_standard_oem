#ifndef __CLOCKDRIVER_H__
#define __CLOCKDRIVER_H__
/*
===========================================================================
*/
/**
  @file ClockDriver.h

*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/clock/src/ClockDriver.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockTZ.h"
#include "ClockBSP.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Main clock driver data structure.
 */
typedef struct ClockDrvCtxt
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  ClockTZBSP *pBSP;
  boolean     bInitialized;
  boolean     bTZInitDone;
  boolean     bRPMAvailable;
} ClockDrvCtxtType;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_ClearSSRestart
** =========================================================================*/
/**
  Clear subsystem restart bits.

  This function clears the subsystem restart bits. They are generllay cleared
  by default, but some may be left asserted after a warm reset. TZ needs to
  clear them so the subsystem XPUs can be programmed.
*/
void Clock_ClearSSRestart (void);

/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/**
  Initialize the image specific portion of the clock driver.

  This function initializes the portion of the clock driver that is specific
  to the image we are running on.  This generally corresponds to a particular
  processor on a particular chipset, but also includes the various boot
  images.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitImage (ClockDrvCtxtType *pDrvCtxt);

#endif /* __CLOCKDRIVER_H__ */

