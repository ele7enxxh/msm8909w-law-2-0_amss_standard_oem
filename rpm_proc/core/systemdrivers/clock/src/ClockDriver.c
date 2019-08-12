/*
===========================================================================
*/
/**
  @file Clock.c

  Interface implementation file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2012 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/src/ClockDriver.c#2 $
  $DateTime: 2016/01/07 07:58:41 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "HALclk.h"
#include "Chipinfo.h"
#include "swevent.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Clock driver context.
 */
ClockDrvCtxt DrvCtxt;
ClockDrvCtxt* pDrvCtxt = &DrvCtxt;

/**
 * Clock handle property
 */
DALSYS_PROPERTY_HANDLE_DECLARE(hProp);

/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver.

  This function initializes the clock driver, it is the main init entry
  point.

  @param None

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverInit 
(
  void
)
{
  DALResult eResult;
  HAL_clk_ContextType HALClkCtxt = {0};

  /*-----------------------------------------------------------------------*/
  /* Get clock property handle.                                            */
  /*-----------------------------------------------------------------------*/

  eResult = DALSYS_GetDALPropertyHandle(DALDEVICEID_CLOCK, hProp);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*
   * Use DALSys busy wait function for HAL modules.
   */
  HALClkCtxt.BusyWait = DALSYS_BusyWait;

  /*
   * Get chip version to send to the HAL.
   */
  HALClkCtxt.nChipVersion = Chipinfo_GetVersion();
  HALClkCtxt.nFamily = Chipinfo_GetFamily();

  HAL_clk_Init(&HALClkCtxt);

  /*-----------------------------------------------------------------------*/
  /* Initialize the core driver.                                           */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitCore ();

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver.                                 */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitImage ();

  if (eResult != DAL_SUCCESS)
  {
    /* TODO: Remove this log once Clock_InitImage has proper logging */
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitImage failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_DriverInit */


/* =========================================================================
**  Function : Clock_InitCore
** =========================================================================*/
/**
  Initialize the core portion of the clock driver.

  This function initializes the common portion of the clock driver that is
  shared with all images.

  @param None

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitCore
(
  void
)
{
  uint32                         nClockDomainIndex;
  uint32                         nClockIndex;
  uint32                         nPowerDomainIndex;
  HAL_clk_ClockDomainHandleType  HALClockDomainHandle = NULL;
  HAL_clk_ClockHandleType        pHALClock = NULL;
  HAL_clk_PowerDomainHandleType  HALPowerDomainHandle = NULL;
  HAL_clk_ClockMuxConfigType     ClockMuxConfig;
  ClockPropertyValueType         PropVal;
  DALResult                      eResult;
  uint32                         nSize;
  uint32                         nClockCountDomain;

  /*-----------------------------------------------------------------------*/
  /* Request the number of clock domains from the HAL and allocate memory  */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->nNumClockDomains = HAL_clk_GetNumberOfClockDomains();

  if (pDrvCtxt->nNumClockDomains > 0)
  {
    nSize = pDrvCtxt->nNumClockDomains * sizeof(*pDrvCtxt->aClockDomains);

    eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aClockDomains);

    if ((eResult != DAL_SUCCESS) || (pDrvCtxt->aClockDomains == NULL))
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %d bytes for %d clock domains.",
        nSize, pDrvCtxt->nNumClockDomains);
      return DAL_ERROR;
    }

    memset((void *)pDrvCtxt->aClockDomains, 0x0, nSize);
  }

  /*-----------------------------------------------------------------------*/
  /* Request the number of clocks from the HAL and allocate memory         */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->nNumClockDomains == 0)
  {
    pDrvCtxt->nNumClocks = 0;
  }
  else
  {
    pDrvCtxt->nNumClocks = HAL_clk_GetNumberOfClocks();

    if (pDrvCtxt->nNumClocks == 0)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "HAL_clk_GetNumberOfClocks returned 0.");
      return DAL_ERROR;
    }

    nSize = pDrvCtxt->nNumClocks * sizeof(*pDrvCtxt->aClocks);

    eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aClocks);

    if ((eResult != DAL_SUCCESS) || (pDrvCtxt->aClocks == NULL))
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %d bytes for %d clocks.",
        nSize, pDrvCtxt->nNumClocks);
      return DAL_ERROR;
    }

    memset((void *)pDrvCtxt->aClocks, 0x0, nSize);
  }

  /*-----------------------------------------------------------------------*/
  /* Request the number of power domains from the HAL and allocate memory  */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->nNumPowerDomains = HAL_clk_GetNumberOfPowerDomains();

  if (pDrvCtxt->nNumPowerDomains > 0)
  {
    nSize = pDrvCtxt->nNumPowerDomains * sizeof (*pDrvCtxt->aPowerDomains);

    eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aPowerDomains);

    if ((eResult != DAL_SUCCESS) || (pDrvCtxt->aPowerDomains == NULL))
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %d bytes for %d power domains.",
        nSize, pDrvCtxt->nNumPowerDomains);
      return DAL_ERROR;
    }

    memset((void *)pDrvCtxt->aPowerDomains, 0x0, nSize);
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate memory for sources and initialize source nodes               */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitSources (pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize clock domain and clock nodes                               */
  /*-----------------------------------------------------------------------*/

  /* Request the first clock domain from the HAL */
  HAL_clk_GetNextClockDomain (NULL, &HALClockDomainHandle);

  /* Loop over all clock domains, making sure we don't overflow memory */
  nClockDomainIndex = 0;
  nClockIndex = 0;
  while ((HALClockDomainHandle != NULL) &&
         (nClockDomainIndex < pDrvCtxt->nNumClockDomains))
  {
    /* In RPM, mark all clocks as DOMAIN_CPU, so they can be suppressible for XO shutdownc. */
    CLOCK_FLAG_SET( &(pDrvCtxt->aClockDomains[nClockDomainIndex]), 
                    DOMAIN_CPU);		

    /* Store the HAL clock domain handle in the driver clock domain node */
    pDrvCtxt->aClockDomains[nClockDomainIndex].HALHandle =
      HALClockDomainHandle;

    pDrvCtxt->aClockDomains[nClockDomainIndex].pCallbacks = NULL;

    /* Get the current source for this domain from the HAL */
    HAL_clk_DetectClockMuxConfig(
      pDrvCtxt->aClockDomains[nClockDomainIndex].HALHandle, &ClockMuxConfig);

    /*
     * If the source is valid, controllable, and present in the BSP, link the
     * domain node to a source node
     */
    if ((ClockMuxConfig.eSource != HAL_CLK_SOURCE_NULL) &&
        (pDrvCtxt->anSourceIndex[ClockMuxConfig.eSource] != 0xFF))
    {
      pDrvCtxt->aClockDomains[nClockDomainIndex].pSource =
        &pDrvCtxt->aSources[pDrvCtxt->anSourceIndex[ClockMuxConfig.eSource]];
    }
    /*
     * HAL_clk_DetectClockMuxConfig will return a NULL source for the domains that are 
     * controlled by another processor, i.e., buses. These domains are now connected to 
     * XO for accurate source voting.  This change also assumes a single XO per target and/or 
     * for the domains that are implemented in this image. 
     */
    else if((ClockMuxConfig.eSource == HAL_CLK_SOURCE_NULL) &&
            (pDrvCtxt->anSourceIndex[HAL_CLK_SOURCE_XO] != 0xFF))
    {
      pDrvCtxt->aClockDomains[nClockDomainIndex].pSource =
        &pDrvCtxt->aSources[pDrvCtxt->anSourceIndex[HAL_CLK_SOURCE_XO]];
    }

    /*
     * This domain node is almost complete, now work on its clock nodes
     */

    /* Get the first clock for this domain from the HAL */
    nClockCountDomain = 0;
    pHALClock = HAL_clk_GetNextClockInDomain (pDrvCtxt->aClockDomains[nClockDomainIndex].HALHandle,
                                              nClockCountDomain);

    if (pHALClock == NULL)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
        "HAL_clk_GetNextClockInDomain returned 0 (ClockDomain Index: %d).", nClockDomainIndex);
    }

    /* Loop over all clocks, making sure we don't overflow memory */
    while ((pHALClock != NULL) && (nClockIndex < pDrvCtxt->nNumClocks))
    {
      /* Store the HAL clock handle in the driver clock node */
      pDrvCtxt->aClocks[nClockIndex].HALHandle = pHALClock;

      /* Get the clock name from the HAL */
      HAL_clk_GetClockName (pDrvCtxt->aClocks[nClockIndex].HALHandle,
                            &pDrvCtxt->aClocks[nClockIndex].szName);

      /* Link the domain node to the first set of BSP/XML configuration data */
      if (pDrvCtxt->aClockDomains[nClockDomainIndex].pBSPConfig == NULL)
      {
        eResult = Clock_GetPropertyValue(
          pDrvCtxt->aClocks[nClockIndex].szName, &PropVal);

        if (eResult == DAL_SUCCESS)
        {
          pDrvCtxt->aClockDomains[nClockDomainIndex].pBSPConfig =
            (ClockMuxConfigType *)PropVal;
        }
      }

      /* Link the clock node to its domain node */
      pDrvCtxt->aClocks[nClockIndex].pDomain =
        &pDrvCtxt->aClockDomains[nClockDomainIndex];

      /*
       * All reference counts for clocks, domains, and sources are initialized
       * to zero after we allocate memory for the corresponding data structures.
       * This happens regardless of the actual state of the associated hardware
       * following boot-up and it allows us to insist that the client drivers
       * make symmetrical calls to enable/disable clocks and power domains.
       */

      /*
       * This clock node is complete
       */
      nClockIndex++;
      nClockCountDomain++;

      /* Get the next clock for this domain from the HAL */
      pHALClock = HAL_clk_GetNextClockInDomain (pDrvCtxt->aClockDomains[nClockDomainIndex].HALHandle,
                                                nClockCountDomain);
    } /* End of clock loop */

    /*
     * If the clock list isn't NULL at this point, it means we exhausted the
     * memory allocated to the clock array, i.e. the HAL has more clocks defined
     * than the total number it reported to us.  This error would likely present
     * itself on the last domain, but the root cause could be in the HAL file
     * for an earlier domain.
     */
    if (pHALClock != NULL)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Clock_InitCore exceeded its memory allocation for clocks.");
      return DAL_ERROR;
    }

    /*
     * This domain node and its clock nodes are complete
     */

    /* Request the next clock domain from the HAL */
    HAL_clk_GetNextClockDomain (HALClockDomainHandle, &HALClockDomainHandle);
    nClockDomainIndex++;
  } /* End of clock domain loop */

  /*
   * If the clock domain handle isn't NULL at this point, it means we exhausted
   * the memory allocated to the clock domain array, i.e. the HAL has more
   * domains defined than the total number it reported to us.
   */
  if (HALClockDomainHandle != NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitCore exceeded its memory allocation for clock domains.");
    return DAL_ERROR;
  }

  /*
   * If the clock domain index isn't equal to the total number of domains
   * reported to us by the HAL, it means the HAL has less domains defined
   * than the total number it reported to us.
   */
  if (nClockDomainIndex != pDrvCtxt->nNumClockDomains)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitCore received too few clock domains from the HAL.");
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize power domain nodes                                         */
  /*-----------------------------------------------------------------------*/

  /* Request the first power domain from the HAL */
  HAL_clk_GetNextPowerDomain (NULL, &HALPowerDomainHandle);

  /* Loop over all power domains, making sure we don't overflow memory */
  nPowerDomainIndex = 0;
  while ((HALPowerDomainHandle != NULL) &&
         (nPowerDomainIndex < pDrvCtxt->nNumPowerDomains))
  {
    /* Store the HAL power domain handle in the driver power domain node */
    pDrvCtxt->aPowerDomains[nPowerDomainIndex].HALHandle =
      HALPowerDomainHandle;

    /* Get the power domain name from the HAL */
    HAL_clk_GetPowerDomainName (pDrvCtxt->aPowerDomains[nPowerDomainIndex].HALHandle,
                                &pDrvCtxt->aPowerDomains[nPowerDomainIndex].szName);

    /* Check if the domain is enabled */
    pDrvCtxt->aPowerDomains[nPowerDomainIndex].bEnabled =
      HAL_clk_IsPowerDomainEnabled (pDrvCtxt->aPowerDomains[nPowerDomainIndex].HALHandle);

    /* Request the next power domain from the HAL */
    HAL_clk_GetNextPowerDomain (HALPowerDomainHandle, &HALPowerDomainHandle);
    nPowerDomainIndex++;
  }

  /*
   * If the power domain handle isn't NULL at this point, it means we exhausted
   * the memory allocated to the power domain array, i.e. the HAL has more
   * domains defined than the total number it reported to us.
   */
  if (HALPowerDomainHandle != NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitCore exceeded its memory allocation for power domains.");
    return DAL_ERROR;
  }

  /*
   * If the power domain index isn't equal to the total number of domains
   * reported to us by the HAL, it means the HAL has less domains defined
   * than the total number it reported to us.
   */
  if (nPowerDomainIndex != pDrvCtxt->nNumPowerDomains)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitCore received too few power domains from the HAL.");
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Init voltage module.                                                  */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitVoltage(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    /* TODO: Remove this log once Clock_InitVoltage has proper logging */
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitVoltage failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Core initialization complete                                          */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitCore */


/* =========================================================================
**  Function : Clock_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param
  None

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverDeInit
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  /* Free memory.                                                          */
  /*-----------------------------------------------------------------------*/

  DALSYS_Free (pDrvCtxt->aClocks);
  pDrvCtxt->aClocks = NULL;

  DALSYS_Free (pDrvCtxt->aClockDomains);
  pDrvCtxt->aClockDomains = NULL;

  DALSYS_Free (pDrvCtxt->aSources);
  pDrvCtxt->aSources = NULL;

  DALSYS_Free (pDrvCtxt->aPowerDomains);
  pDrvCtxt->aPowerDomains = NULL;

  /*-----------------------------------------------------------------------*/
  /* Destroy the mutex object.                                             */
  /*-----------------------------------------------------------------------*/

  DALSYS_DestroyObject(pDrvCtxt->hLock);

  /*-----------------------------------------------------------------------*/
  /* Shut-down complete.                                                   */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_DriverDeInit */


/* =========================================================================
**  Function : Clock_GetClockId
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_GetClockId
(
  const char   *szClock,
  ClockIdType  *pnId
)
{
  uint32   nClockIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Search the array of clock nodes, break if we find a match             */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nClockIndex = 0; nClockIndex < pDrvCtxt->nNumClocks; nClockIndex++)
  {
    if (strcmp (szClock, pDrvCtxt->aClocks[nClockIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the clock node if found, otherwise return NULL    */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnId = (ClockIdType)&pDrvCtxt->aClocks[nClockIndex];
    return DAL_SUCCESS;
  }
  else
  {
    *pnId = (ClockIdType)NULL;
    return DAL_ERROR;
  }

} /* END Clock_GetClockId */

/* =========================================================================
**  Function : Clock_WaitForClockOn
** =========================================================================*/
/**
  Waits for a clock to report it is turned on.

  @param *pClock      [in] -- Pointer to a clock.
  @param nTimeoutUsec [in] -- approximate timeout in usec.

  @return
  DAL_SUCCESS -- The clock reported itself on within the alloted time.
  DAL_ERROR_NOT_SUPPORTED -- The clock does not support "IsOn" API.
  DAL_ERROR_TIMEOUT -- The clock did not report itself on within alloted time.
*/

DALResult Clock_WaitForClockOn
(
  ClockNodeType   *pClock,
  uint32          nTimeoutUsec
)
{
  uint32 waitTimeUsec = 0;

  /* Determine whether this clock supports "IsOn" API functionality */
  if ( !HAL_clk_IsClockSupported(pClock->HALHandle, HAL_CLK_CLOCK_API_ISON) )
  {
    return DAL_ERROR;
  }

  /* Wait for the clock to turn ON */
  while( TRUE )
  {
    if ( HAL_clk_IsClockOn(pClock->HALHandle) )
      return DAL_SUCCESS;

    if( waitTimeUsec >= nTimeoutUsec )
      break;

    waitTimeUsec++;
    DALSYS_BusyWait(1);
  }

  /* Timed-out waiting for clock to turn ON--log a warning. */
  DALSYS_LogEvent(
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
      "Unable to turn ON clock: %s.", pClock->szName);

  return DAL_ERROR;
}


/* =========================================================================
**  Function : Clock_EnableClockDomain
** =========================================================================*/
/**
  Enables a clock domain.

  This function enables a clock domain, generally as a result of a call to
  Clock_EnableClock().

  @param *pClockDomain [in] -- Pointer to a clock domain node.

  @return
  DAL_SUCCESS -- The domain was successfully enabled.
  DAL_ERROR -- The domain was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_EnableClockDomain
(
  ClockDomainNodeType *pClockDomain
)
{
  DALResult eResult;
  boolean   bGFXVReg;

  /* Make sure the domain handle is valid */
  if (pClockDomain == NULL)
  {
    return DAL_ERROR;
  }

  /* Domain with GFX voltage */
  bGFXVReg = CLOCK_FLAG_IS_SET( pClockDomain, DOMAIN_GFX_VREG );

  /* If the reference count is zero, we need to enable the domain */
  if (pClockDomain->nReferenceCount == 0)
  {
    /* If this domain has a controllable source, enable it first */
    if (pClockDomain->pSource != NULL)
    {
      eResult =
        Clock_EnableSource(
          pDrvCtxt, pClockDomain->pSource,
          CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_CPU));

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /* Make a voltage request if necessary */
    if (!CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
        pClockDomain->pActiveMuxConfig != NULL)
    {
      Clock_VoltageRequest(
        pDrvCtxt, &pClockDomain->eVRegLevel,
        pClockDomain->pActiveMuxConfig->eVRegLevel, bGFXVReg);
    }
  }

  /* Increment the reference count */
  pClockDomain->nReferenceCount++;

  return DAL_SUCCESS;

} /* END Clock_EnableClockDomain */


/* =========================================================================
**  Function : Clock_DisableClockDomain
** =========================================================================*/
/**
  Disables a clock domain.

  This function disables a clock domain, generally as a result of a call to
  Clock_DisableClock().

  @param *pClockDomain [in] -- Pointer to a clock domain node.

  @return
  DAL_SUCCESS -- The domain was successfully disabled.
  DAL_ERROR -- The domain was not disabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DisableClockDomain
(
  ClockDomainNodeType *pClockDomain
)
{
  DALResult eResult;
  boolean   bGFXVReg;

  /* Make sure the domain handle is valid */
  if (pClockDomain == NULL)
  {
    return DAL_ERROR;
  }

  /* Domain with GFX voltage */
  bGFXVReg = CLOCK_FLAG_IS_SET( pClockDomain, DOMAIN_GFX_VREG );

  /* If the reference count is one, we need to disable the domain */
  if (pClockDomain->nReferenceCount == 1)
  {
    /* If this domain has a controllable source, disable it */
    if (pClockDomain->pSource != NULL)
    {
      eResult =
        Clock_DisableSource(
          pDrvCtxt, pClockDomain->pSource,
          CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_CPU));

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /* Release voltage request if necessary */
    if (!CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST))
    {
      /* Turn VREG OFF if GFX rail */
      if ( bGFXVReg )
      {
        Clock_VoltageRequest( pDrvCtxt, 
                              &pClockDomain->eVRegLevel, CLOCK_VREG_LEVEL_OFF, bGFXVReg);
      }
      else
      {
        Clock_VoltageRequest( pDrvCtxt, 
                              &pClockDomain->eVRegLevel, CLOCK_VREG_LEVEL_LOW, bGFXVReg);
      }
    }
  }

  /* Decrement the reference count */
  if (pClockDomain->nReferenceCount > 0)
  {
    pClockDomain->nReferenceCount--;
  }

  return DAL_SUCCESS;

} /* END Clock_DisableClockDomain */


/* =========================================================================
**  Function : Clock_EnableClock
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_EnableClockEx
(
  ClockIdType   nClock,
  boolean       bWaitForClkOn
)
{
  ClockNodeType   *pClock = NULL;
  DALResult        eResult;

  /* Cast nClock to a proper pointer */
  pClock = (ClockNodeType*)nClock;

  /* Make sure the clock handle is valid */
  if (pClock == NULL)
  {
    return DAL_ERROR;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the reference count is zero, we need to enable the clock */
  if (pClock->nReferenceCount == 0)
  {
    /* Enable the domain first */
    eResult = Clock_EnableClockDomain (pClock->pDomain);

    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return eResult;
    }

    /* Enable the clock */
    HAL_clk_EnableClock (pClock->HALHandle);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_STATE_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_STATE_CHANGE))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Enabled: 1)", pClock->szName);
    }

    if (bWaitForClkOn == TRUE)
    {
      Clock_WaitForClockOn(pClock, CLK_ON_WAIT_USEC);
    }
  }

  /* Increment the reference count */
  pClock->nReferenceCount++;

  // Log clock enable event
  SWEVENT( CLOCK_EVENT_STATUS, 
	   3,
           HAL_clk_GetTestClockId(pClock->HALHandle), 
           1,
	   pClock->nReferenceCount);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

}

DALResult Clock_EnableClock
(
  ClockIdType   nClock
)
{
  return Clock_EnableClockEx(nClock, TRUE);
} /* END Clock_EnableClock */


/* =========================================================================
**  Function : Clock_DisableClock
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_DisableClock
(
  ClockIdType   nClock
)
{
  ClockNodeType   *pClock = NULL;
  DALResult        eResult;

  /* Cast nClock to a proper pointer */
  pClock = (ClockNodeType*)nClock;

  /* Make sure the clock handle is valid */
  if (pClock == NULL)
  {
    return DAL_ERROR;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /*
   * If the reference count is one, we need to disable the clock.
   * Ignore if the DO_NOT_DISABLE bits are set.
   */
  if ((pClock->nReferenceCount == 1) &&
      !CLOCK_FLAG_IS_SET(pClock, DO_NOT_DISABLE) &&
      !CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_CLOCKS))
  {
    /* Disable the clock */
    HAL_clk_DisableClock (pClock->HALHandle);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_STATE_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_STATE_CHANGE))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Enabled: 0)", pClock->szName);
    }

    /* Disable the domain */
    eResult = Clock_DisableClockDomain (pClock->pDomain);

    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return eResult;
    }
  }

  /* Decrement the reference count */
  if (pClock->nReferenceCount > 0)
  {
    pClock->nReferenceCount--;
  }

  // Log clock disable event
  SWEVENT( CLOCK_EVENT_STATUS, 
	   3,
           HAL_clk_GetTestClockId(pClock->HALHandle), 
           0,
	   pClock->nReferenceCount);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;


} /* END Clock_DisableClock */


/* =========================================================================
**  Function : Clock_FindClockConfig
** =========================================================================*/
/**
  Finds a particular clock configuration in the BSP.

  This function finds a particular clock configuration in the BSP based on
  the specified frequency and match criteria.

  @param *pBSPConfig [in] -- Pointer to a list of configurations
  @param nFreqHz [in] -- Frequency in Hz
  @param eMatch [in] -- Match criteria
  @param **pMatchingConfig [out] -- Matching configuration

  @return
  DAL_SUCCESS -- A matching configuration was found.
  DAL_ERROR -- A matching configuration was not found.

  @dependencies
  None.
*/

static DALResult Clock_FindClockConfig
(
  ClockMuxConfigType  *pBSPConfig,
  uint32               nFreqHz,
  ClockFrequencyType   eMatch,
  ClockMuxConfigType **pMatchingConfig
)
{
  uint32              nAtLeastFrequency = 0xFFFFFFFF;
  ClockMuxConfigType *pAtLeastConfig = NULL;
  uint32              nAtMostFrequency = 0;
  ClockMuxConfigType *pAtMostConfig = NULL;

  /* Make sure the list of configurations isn't empty */
  if (pBSPConfig == NULL)
  {
    *pMatchingConfig = NULL;
    return DAL_ERROR;
  }

  /* Loop over all configurations */
  while (pBSPConfig->nFreqHz != 0)
  {
    /*
     * Skip configurations that aren't for this chip or version, and configurations
     * that rely on an external source.
     */
    if ((!Clock_IsBSPSupported(pBSPConfig)) ||
        ((pBSPConfig->HALConfig.eSource >= HAL_CLK_SOURCE_EXTERNAL) &&
        (pBSPConfig->HALConfig.eSource < HAL_CLK_NUM_OF_EXTERNAL_SOURCES)))
    {
      pBSPConfig++;
      continue;
    }

    /* Stop if we find an exact match */
    if (pBSPConfig->nFreqHz == nFreqHz)
    {
      *pMatchingConfig = pBSPConfig;
      return DAL_SUCCESS;
    }

    /* Look for the lowest at-least frequency */
    if ((pBSPConfig->nFreqHz > nFreqHz) &&
        (pBSPConfig->nFreqHz < nAtLeastFrequency))
    {
      nAtLeastFrequency = pBSPConfig->nFreqHz;
      pAtLeastConfig = pBSPConfig;
    }
    /* Look for the highest at-most frequency */
    else if ((pBSPConfig->nFreqHz < nFreqHz) &&
             (pBSPConfig->nFreqHz > nAtMostFrequency))
    {
      nAtMostFrequency = pBSPConfig->nFreqHz;
      pAtMostConfig = pBSPConfig;
    }

    /* Move to the next configuration */
    pBSPConfig++;
  }

  /* Select a config based on the match criteria */
  switch (eMatch)
  {
    case CLOCK_FREQUENCY_HZ_AT_LEAST:
      if (pAtLeastConfig != NULL)
      {
        *pMatchingConfig = pAtLeastConfig;
        return DAL_SUCCESS;
      }
      break;

    case CLOCK_FREQUENCY_HZ_AT_MOST:
      if (pAtMostConfig != NULL)
      {
        *pMatchingConfig = pAtMostConfig;
        return DAL_SUCCESS;
      }
      break;

    case CLOCK_FREQUENCY_HZ_CLOSEST:
      if ((pAtLeastConfig != NULL) && (pAtMostConfig == NULL))
      {
        *pMatchingConfig = pAtLeastConfig;
        return DAL_SUCCESS;
      }
      else if ((pAtMostConfig != NULL) && (pAtLeastConfig == NULL))
      {
        *pMatchingConfig = pAtMostConfig;
        return DAL_SUCCESS;
      }
      else if ((pAtLeastConfig != NULL) && (pAtMostConfig != NULL))
      {
        /* Select the closest match, select the lowest in case of a tie */
        if ((nAtLeastFrequency - nFreqHz) < (nFreqHz - nAtMostFrequency))
        {
          *pMatchingConfig = pAtLeastConfig;
          return DAL_SUCCESS;
        }
        else
        {
          *pMatchingConfig = pAtMostConfig;
          return DAL_SUCCESS;
        }
      }
      break;

    default:
      break;
  }

  /* No match was found */
  *pMatchingConfig = NULL;

  return DAL_ERROR;

} /* END Clock_FindClockConfig */


/* =========================================================================
**  Function : Clock_SetClockConfig
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_SetClockConfig
(
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
)
{
  ClockSourceNodeType *pNewSource;
  DALResult            eResult;
  uint32               nSrcIdx;
  boolean              bCPUDomain;
  boolean              bVoltageRequest;
  boolean              bGFXVReg;
  ClockCallbackType    pCallback;
  int                  i;
  uint32               nCurClkKHz = 0, nNewClkKHz = 0;

  /* Make sure the domain handle is valid */
  if ((pDomain == NULL) || (pNewConfig == NULL) || 
        (pNewConfig->HALConfig.eSource >= HAL_CLK_NUM_OF_SOURCES))
  {
    return DAL_ERROR;
  }

  if( pDomain->pActiveMuxConfig != NULL ) {
    nCurClkKHz = pDomain->pActiveMuxConfig->nFreqHz / 1000;
  }
  nNewClkKHz = pNewConfig->nFreqHz / 1000;

  /* Determine if this is a CPU domain */
  bCPUDomain = CLOCK_FLAG_IS_SET(pDomain, DOMAIN_CPU);

  /* Domain with GFX voltage */
  bGFXVReg = CLOCK_FLAG_IS_SET( pDomain, DOMAIN_GFX_VREG );

  /*
   * Determine if we should make a voltage request.  We do so if this
   * domain is not configured to make requests when enabled, or if it
   * is thus configured and is currently enabled.
   */
  bVoltageRequest =
    (pDomain->nReferenceCount > 0) &&
    !CLOCK_FLAG_IS_SET(pDomain, DOMAIN_HOLD_VREG_REQUEST);

  /* Get the new source pointer. */
  nSrcIdx = pDrvCtxt->anSourceIndex[pNewConfig->HALConfig.eSource];

  if (nSrcIdx != 0xFF)
  {
    pNewSource = &pDrvCtxt->aSources[nSrcIdx];
  }
  else
  {
    pNewSource = NULL;
  }

  /*
   * Configure the source if required.
   */
  if (pNewSource != NULL && pNewConfig->pSourceConfig != NULL)
  {
    eResult = Clock_ConfigSource(
      pDrvCtxt, pNewSource, pNewConfig->pSourceConfig);

    if (eResult != DAL_SUCCESS)
    {
      // TODO error!
      return eResult;
    }
  }
  /*
   * If we are increasing the voltage requirement, do so now.
   */
  if (bVoltageRequest && pNewConfig->eVRegLevel > pDomain->eVRegLevel)
  {
    Clock_VoltageRequest(
      pDrvCtxt, &pDomain->eVRegLevel, pNewConfig->eVRegLevel, bGFXVReg);
  }

  /*
   * If the domain is active, and the new source is different from the
   * current one then we enable the new source.
   */
  if (pDomain->nReferenceCount > 0 &&
      pDomain->pSource != pNewSource &&
      pNewSource != NULL)
  {
    eResult = Clock_EnableSource(pDrvCtxt, pNewSource, bCPUDomain);

    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }
  }

  /* Call the callbacks if any are registered */
  if( pDomain->pCallbacks != NULL )
  {
    for(i=0; i<CLOCK_CALLBACK_MAX; i++)
    {
      if( pDomain->pCallbacks[i].pCallback != NULL )
      {
        pCallback = pDomain->pCallbacks[i].pCallback;
        pCallback( CLOCK_PRE_CALLBACK, nCurClkKHz, nNewClkKHz, 
                      pDomain->pCallbacks[i].pCallbackData );
      }
    }
  }

  /*
   * Configure the clock MUX
   */
  HAL_clk_ConfigClockMux(pDomain->HALHandle, &pNewConfig->HALConfig);

  /* Call the callbacks if any are registered */
  if( pDomain->pCallbacks != NULL )
  {
    for(i=0; i<CLOCK_CALLBACK_MAX; i++)
    {
      if( pDomain->pCallbacks[i].pCallback != NULL )
      {
        pCallback = pDomain->pCallbacks[i].pCallback;
        pCallback( CLOCK_POST_CALLBACK, nCurClkKHz, nNewClkKHz, 
                      pDomain->pCallbacks[i].pCallbackData );
      }
    }
  }

  /*
   * If the domain is active, and the new source is different from the
   * current then we disable the old source.
   */
  if (pDomain->nReferenceCount > 0 &&
      pDomain->pSource != pNewSource &&
      pDomain->pSource != NULL)
  {
    Clock_DisableSource(pDrvCtxt, pDomain->pSource, bCPUDomain);
  }
  /*
   * If we are decreasing the voltage requirement, do so now.
   */
  if (bVoltageRequest && pNewConfig->eVRegLevel < pDomain->eVRegLevel)
  {
    Clock_VoltageRequest(
      pDrvCtxt, &pDomain->eVRegLevel, pNewConfig->eVRegLevel, bGFXVReg);
  }

  /* Link the domain node to the new source node and update the active config */
  pDomain->pSource = pNewSource;

  /*
   * Check if the current configuration was setup dynamically
   */
  if(CLOCK_FLAG_IS_SET(pDomain, DOMAIN_DYNAMIC_CONFIG))
  {
    /*
     * If new configuration is different from the current one then
     * free up memory.
     */
    if(pDomain->pActiveMuxConfig != pNewConfig)
    {
      CLOCK_FLAG_CLEAR(pDomain, DOMAIN_DYNAMIC_CONFIG);
      DALSYS_Free (pDomain->pActiveMuxConfig);
    }
  }

  pDomain->pActiveMuxConfig = pNewConfig;

  return DAL_SUCCESS;

} /* END Clock_SetClockConfig */


/* =========================================================================
**  Function : Clock_SetClockFrequency
** =========================================================================*/
/*
  See Clock.h
*/

DALResult Clock_SetClockFrequency
(
  ClockIdType         nClock,
  uint32              nFreq,
  ClockFrequencyType  eMatch,
  uint32             *pnResultFreq
)
{
  ClockNodeType       *pClock;
  ClockDomainNodeType *pDomain;
  ClockMuxConfigType  *pNewConfig = NULL;
  DALResult            eResult;
  uint32               nFreqHz;
  uint32               nMult;
  uint32               nResultFreq;

  /* Cast nClock to a proper pointer */
  pClock = (ClockNodeType *)nClock;

  /* Make sure the clock handle is valid and supports frequency configuration. */
  if (pClock == NULL || pClock->pDomain == NULL ||
      pClock->pDomain->pBSPConfig == NULL)
  {
    if (pnResultFreq != NULL)
    {
      *pnResultFreq = 0;
    }
    return DAL_ERROR;
  }

  pDomain = pClock->pDomain;

  /* Convert to Hz if necessary. */
  if (eMatch >= CLOCK_FREQUENCY_MHZ_AT_LEAST)
  {
    nMult = 1000000;
    nFreqHz = nFreq * 1000000;
    eMatch -= CLOCK_FREQUENCY_MHZ_AT_LEAST;
  }
  else if (eMatch >= CLOCK_FREQUENCY_KHZ_AT_LEAST)
  {
    nMult = 1000;
    nFreqHz = nFreq * 1000;
    eMatch -= CLOCK_FREQUENCY_KHZ_AT_LEAST;
  }
  else
  {
    nMult = 1;
    nFreqHz = nFreq;
  }

  /* Look for a valid configuration */
  eResult = Clock_FindClockConfig (pDomain->pBSPConfig, nFreqHz, eMatch,
                                   &pNewConfig);

  if ((eResult != DAL_SUCCESS) || (pNewConfig == NULL))
  {
    if (pnResultFreq != NULL)
    {
      *pnResultFreq = 0;
    }
    return DAL_ERROR;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the new config is the same as the active config, we're done */
  if (pNewConfig != pDomain->pActiveMuxConfig)
  {
    eResult = Clock_SetClockConfig(pDomain, pNewConfig);

    if (eResult == DAL_SUCCESS)
    {
      if (CLOCK_FLAG_IS_SET(pClock, LOG_FREQ_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
      {
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                          "%s (Freq: %lu)",
                          pClock->szName, pNewConfig->nFreqHz);
      }
    }
  }
  else
  {
    eResult = DAL_SUCCESS;
  }

  if (eResult == DAL_SUCCESS)
  {
    nResultFreq = pNewConfig->nFreqHz / nMult;
  }
  else
  {
    nResultFreq = 0;
  }

  if (pnResultFreq != NULL)
  {
    *pnResultFreq = nResultFreq;
  }

  SWEVENT( CLOCK_EVENT_FREQ,
           HAL_clk_GetTestClockId(pClock->HALHandle),
           nResultFreq * nMult );

  DALCLOCK_FREE(pDrvCtxt);

  return eResult;

} /* END Clock_SetClockFrequency */


/* =========================================================================
**  Function : Clock_IsClockEnabled
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_IsClockEnabled
(
  ClockIdType   nClock,
  boolean      *pbIsEnabled
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    *pbIsEnabled = FALSE;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from the HAL.                                               */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  *pbIsEnabled = HAL_clk_IsClockEnabled(pClock->HALHandle);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_IsClockEnabled */


/* =========================================================================
**  Function : Clock_IsClockOn
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_IsClockOn
(
  ClockIdType   nClock,
  boolean      *pbIsOn
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    *pbIsOn = FALSE;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from the HAL.                                               */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  *pbIsOn = HAL_clk_IsClockOn(pClock->HALHandle);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_IsClockOn */


/* =========================================================================
**  Function : Clock_GetClockFrequency
** =========================================================================*/
/*
  See Clock.h
*/

DALResult Clock_GetClockFrequency
(
  ClockIdType   nClock,
  uint32       *pnFrequencyHz
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;
  HAL_clk_ClockMuxConfigType  HALConfig;
  uint32 nFreqHz = 0;
  uint32 nSrcIdx;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    *pnFrequencyHz = 0;
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from current config either local copy or read from HW.      */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  if (pClock->pDomain->pActiveMuxConfig != NULL)
  {
    nFreqHz = pClock->pDomain->pActiveMuxConfig->nFreqHz;
  }
  else
  {
    HAL_clk_DetectClockMuxConfig(pClock->pDomain->HALHandle, &HALConfig);

    /*
     * Here we could loop over supported BSP configs and if found use
     * the frequency there which might be more accurate in some cases.
     */

    nSrcIdx = pDrvCtxt->anSourceIndex[HALConfig.eSource];

    if (nSrcIdx != 0xFF)
    {
      nFreqHz = pDrvCtxt->aSources[nSrcIdx].pBSPConfig->nFreqHz;

      if (HALConfig.nDiv2x > 2)
      {
        nFreqHz = 2 * (nFreqHz / HALConfig.nDiv2x);
      }

      if (HALConfig.nM > 1)
      {
        nFreqHz *= HALConfig.nM;
      }

      if (HALConfig.nN > 1)
      {
        nFreqHz /= HALConfig.nN;
      }
    }
  }

  // May need to store and use a clock divider in some cases...

  DALCLOCK_FREE(pDrvCtxt);

  *pnFrequencyHz = nFreqHz;

  return DAL_SUCCESS;

} /* END Clock_GetClockFrequency */


/* =========================================================================
**  Function : Clock_GetPropertyValue
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_GetPropertyValue
(
  const char              *szName,
  ClockPropertyValueType  *pValue
)
{
  DALResult         eRes;
  DALSYSPropertyVar Var;

  eRes = DALSYS_GetPropertyValue(hProp, szName, 0, &Var); 

  if (eRes != DAL_SUCCESS) return eRes;

  *pValue = (ClockPropertyValueType)Var.Val.pStruct;

  return DAL_SUCCESS;

} /* END Clock_GetPropertyValue */


/* =========================================================================
**  Function : Clock_GetPowerDomainId
** =========================================================================*/
/**
  Gets the power domain identifier.

  This function looks up and returns the identifier for a specified power
  domain. This identifier is used for the power domain control functions.
  The string used to get the identifier can be found in the software frequency 
  plan, e.g., VDD_GRP.

  @param szPowerDomain     [in]  Power domain name.
  @param pnPowerDomainId   [out] Pointer to the ID to be filled in. 

  @return
  DAL_SUCCESS -- Power domain was valid, and the ID was filled in.
  DAL_ERROR   -- Power domain was invalid.

  @dependencies
  None.
*/

DALResult Clock_GetPowerDomainId
(
  const char             *szPowerDomain,
  ClockPowerDomainIdType *pnPowerDomainId
)
{
  uint32   nPowerDomainIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Search the array of power domain nodes, break if we find a match      */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nPowerDomainIndex = 0; nPowerDomainIndex < pDrvCtxt->nNumPowerDomains; nPowerDomainIndex++)
  {
    if (strcmp (szPowerDomain, pDrvCtxt->aPowerDomains[nPowerDomainIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the power domain node if found, otherwise NULL    */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnPowerDomainId =
      (ClockPowerDomainIdType)&pDrvCtxt->aPowerDomains[nPowerDomainIndex];
    return DAL_SUCCESS;
  }
  else
  {
    *pnPowerDomainId = (ClockPowerDomainIdType)NULL;
    return DAL_ERROR;
  }

} /* END Clock_GetPowerDomainId */


/* =========================================================================
**  Function : Clock_EnablePowerDomain
** =========================================================================*/
/**
  Enables a power domain.

  @param nPowerDomainId [in] - Power domain to enable.
         bAsync [in]         - Async function call.  Return right away without
                               without looping for checking enable power bit

  @return
  DAL_SUCCESS -- Power domain was enabled.
  DAL_ERROR   -- Power domain identifier was invalid.

  @dependencies
  None.

  @sideeffects
  Any clocks inside the power domain are enabled during the enable
  sequence. The state is restored afterward.

  @sa
  DalClock_GetPowerDomainId(), DalClock_DisablePowerDomain()
*/

DALResult Clock_EnablePowerDomain
(
  ClockPowerDomainIdType  nPowerDomainId,
  boolean                 bAsync
)
{
  ClockPowerDomainNodeType *pPowerDomain = NULL;

  /* Cast nPowerDomainId to a proper pointer */
  pPowerDomain = (ClockPowerDomainNodeType*)nPowerDomainId;

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return DAL_ERROR;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the reference count is zero, we need to enable the power domain */
  if (pPowerDomain->nReferenceCount == 0)
  {
    /* Enable the power domain */
    if (!pPowerDomain->bEnabled)
    {
      HAL_clk_EnablePowerDomain (pPowerDomain->HALHandle, bAsync);
      pPowerDomain->bEnabled = TRUE;

      /* Logging */
      if (CLOCK_FLAG_IS_SET(pPowerDomain, LOG_STATE_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_POWER_DOMAIN_STATE_CHANGE))
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                          "%s (Enabled: 1)", pPowerDomain->szName);
      }
    }
  }

  /* Increment the reference count */
  pPowerDomain->nReferenceCount++;

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_EnablePowerDomain */


/* =========================================================================
**  Function : Clock_DisablePowerDomain
** =========================================================================*/
/**  Disables a power domain.

  @param nPowerDomainId   [in] Power domain to disable.
  
  @return  
  DAL_SUCCESS -- Power domain was disabled. 
  DAL_ERROR   -- Power domain identifier was invalid.

  @dependencies
  Any bus interfaces for this power domain must be disabled prior to
  collapsing the core or the bus may lock up.

  @sideeffects
  Any clocks inside the power domain are enabled during the collapse
  sequence.  

  @sa  DalClock_GetPowerDomainId(), DalClock_EnablePowerDomain()
*/

DALResult Clock_DisablePowerDomain
(
  ClockPowerDomainIdType  nPowerDomainId
)
{
  ClockPowerDomainNodeType *pPowerDomain = NULL;

  /* Cast nPowerDomainId to a proper pointer */
  pPowerDomain = (ClockPowerDomainNodeType*)nPowerDomainId;

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return DAL_ERROR;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /*
   * If the reference count is one or zero, we need to disable the power domain.
   * Ignore if the DO_NOT_DISABLE bits are set.
   */
  if ((pPowerDomain->nReferenceCount <= 1) &&
      !CLOCK_FLAG_IS_SET(pPowerDomain, DO_NOT_DISABLE) &&
      !CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_POWER_DOMAINS))
  {
    /* Disable the power domain */
    if (pPowerDomain->bEnabled)
    {
      HAL_clk_DisablePowerDomain (pPowerDomain->HALHandle);
      pPowerDomain->bEnabled = FALSE;

      /* Logging */
      if (CLOCK_FLAG_IS_SET(pPowerDomain, LOG_STATE_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_POWER_DOMAIN_STATE_CHANGE))
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                          "%s (Enabled: 0)", pPowerDomain->szName);
      }
    }
  }

  /* Decrement the reference count */
  if (pPowerDomain->nReferenceCount > 0)
  {
    pPowerDomain->nReferenceCount--;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_DisablePowerDomain */


/*=========================================================================
**  Function : Clock_IsPowerDomainEnabled
** =========================================================================*/
/**  Check if a power domain is enable or not.

  @param nPowerDomainId   [in] Power domain to check.
  
  @return  
  FALSE -- Power domain was disabled. 
  TRUE  -- Power domain was enabled. 

  @dependencies
  None

  @sideeffects
  None 

  @sa  Clock_GetPowerDomainId()
*/

boolean Clock_IsPowerDomainEnabled
(
  ClockPowerDomainIdType  nPowerDomainId
)
{

  ClockPowerDomainNodeType *pPowerDomain = NULL;

  /* Cast nPowerDomainId to a proper pointer */
  pPowerDomain = (ClockPowerDomainNodeType*)nPowerDomainId;

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return FALSE;
  }
  
  return HAL_clk_IsPowerDomainEnabled (pPowerDomain->HALHandle);
}


/*=========================================================================
**  Function : Clock_GetDrvCtxt
** =========================================================================*/
/**
  Returns driver context.

  This function is used to get the driver context when direct calls are made to 
  internal debug functions.

  @return 
  Pointer to clock driver context.

  @dependencies
  None.
*/

ClockDrvCtxt *Clock_GetDrvCtxt(void)
{
  return pDrvCtxt;
}


/*=========================================================================
**  Function : Clock_EnableDCVS
** =========================================================================*/
/**
  Enable DCVS (Dynamic Clock Voltage Scaling)

  This function is used to set DCVS enable or disable.  

  @param bEnablle - TRUE  - DCVS enable
                    FALSE - DCVS disable

  @return 
  None

  @dependencies
  None.
*/

void Clock_EnableDCVS(boolean bEnable)
{
    pDrvCtxt->bDCVSEnabled = bEnable;
}


/*=========================================================================
**  Function : Clock_IsBSPSupported
** =========================================================================*/
/**

  This function is used for checking if BSP config is supported based on
  its chip version.
    BSP ChipVersion =  0          ==> Support all HW version.  Return TRUE
    If BSP data version is greater or equal to min, and less than HW version, return TRUE.
    else return FALSE

  @param pBSPConfig [in] - BSP for checking

  @return 
  TRUE - BSP config is supported.
  FALSE - BSP config is not supported.

  @dependencies
  None.
*/

boolean Clock_IsBSPSupported( ClockMuxConfigType *pBSPConfig )
{
  uint32 nHWVersion, nBSPMaxVersion, nBSPMinVersion;

  /* Support if ChipVersion = 0, and ChipFamily = 0 (Unknown) */
  if ( (pBSPConfig->nChipVersion == 0) && (pBSPConfig->nFamily == CHIPINFO_FAMILY_UNKNOWN) )
  {
    return TRUE;
  }
  else if (pBSPConfig->nChipVersion == BSP_NOT_SUPPORTED)
  {
    return FALSE;
  }

  if (pBSPConfig->nFamily != 0)
  {
    if ( HAL_clk_GetChipFamily() != pBSPConfig->nFamily )
    {
      pBSPConfig->nChipVersion = BSP_NOT_SUPPORTED;
      return FALSE;
    }
    else
    {
      /* Match current chip family, change to default for skip checking next time */
      pBSPConfig->nFamily = CHIPINFO_FAMILY_UNKNOWN;
    }
  }

  /* Support if ChipVersion = 0 */
  if ( pBSPConfig->nChipVersion == 0 )
  {
    return TRUE;
  }

  /* Check BSP base on chip version between Min and Max */
  nHWVersion      = HAL_clk_GetChipVersion();
  nBSPMinVersion  = BSP_HW_VER_MIN( pBSPConfig->nChipVersion );
  nBSPMaxVersion  = BSP_HW_VER_MAX( pBSPConfig->nChipVersion );

  if ( (nHWVersion >= nBSPMinVersion) && (nHWVersion < nBSPMaxVersion) )
  {
    pBSPConfig->nChipVersion = BSP_SUPPORTED;
    return TRUE;
  }
  else
  {
    pBSPConfig->nChipVersion = BSP_NOT_SUPPORTED;
    return FALSE;
  }
}

/*=========================================================================
**  Function : Clock_LogState
** =========================================================================*/
/**
  This function is used for logging all the clocks status

  @param 
      None

  @return 
      None

  @dependencies
      None.
*/

void Clock_LogState ( void )
{
  ClockNodeType *pClock;
  uint32        i, nFreqHz;
  int32         nIsOn, nRefCount;
  boolean       bIsOn = FALSE;

  /*------------------------------------------------------------------------*/
  /* Log clock state.                                                       */
  /*------------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumClocks; i++)
  {
    pClock = &pDrvCtxt->aClocks[i];
    nIsOn = -1;
    nFreqHz = 0;

    /*
     * Get ReferenceCount.
     */
    nRefCount = pClock->nReferenceCount;

    /*
     * Get ON state.
     */
    if (HAL_clk_IsClockSupported(pClock->HALHandle, HAL_CLK_CLOCK_API_ISON))
    {
      Clock_IsClockOn((ClockIdType)pClock, &bIsOn);
      nIsOn = bIsOn ? 1 : 0;
    }

    /*
     * Get frequency.
     */
    if ( ((nRefCount > 0) || bIsOn) &&
         (pClock->pDomain->pActiveMuxConfig != NULL) )
    {
      nFreqHz = pClock->pDomain->pActiveMuxConfig->nFreqHz;
    }

    SWEVENT ( SYSTEM_DB_CLK_STATUS, (uint32)pClock->szName, nIsOn, nRefCount, nFreqHz );
  }
} /* END Clock_LogState */


/* =========================================================================
**  Function : Clock_RegisterCallback
** =========================================================================*/
/**
  This function registers a callback that will be called before and after every 
  clock switch.

  @param nClock [in] - Clock to register callback.
  @param pHandle [out] - Returned handle to the callback.
  @param pCallback [in] - Function pointer to the callback function.
  @param pCallbackData [in] - Can be used to pass any 32-bit pointer to the callback

  DAL_SUCCESS -- Clock was valid. The frequency may or may not have been 
                 retrieved (see function description). \n
  DAL_ERROR -- Clock identifier was invalid.
  @return

  @dependencies
  None.

  @sa
  None
*/

DALResult Clock_RegisterCallback( 
    ClockIdType nClock,
    ClockCallbackHandle *pHandle,
    ClockCallbackType pCallback,
    void *pCallbackData )
{
  ClockNodeType        *pClock = NULL;
  ClockDomainNodeType  *pDomain = NULL;
  int                  i;

  /* Cast nClock to a proper pointer */
  pClock = (ClockNodeType*)nClock;
  if(pClock == NULL) return DAL_ERROR;
  if(pHandle == NULL) return DAL_ERROR;

  /* Retrieve the domain */
  pDomain = pClock->pDomain;
  if(pDomain == NULL) return DAL_ERROR;

  if(pDomain->pCallbacks == NULL)
  {
    pDomain->pCallbacks = malloc(CLOCK_CALLBACK_MAX * sizeof(ClockDomainCallbackType) );
    if(pDomain->pCallbacks) 
    {
      memset(pDomain->pCallbacks, 0, CLOCK_CALLBACK_MAX * sizeof(ClockDomainCallbackType) );
    }
  }

  if(pDomain->pCallbacks == NULL) return DAL_ERROR;

  for(i=0; i<CLOCK_CALLBACK_MAX; i++)
  {
    if( pDomain->pCallbacks[i].pCallback == NULL )
    {
      pDomain->pCallbacks[i].pCallback = pCallback;
      pDomain->pCallbacks[i].pCallbackData = pCallbackData;
      *pHandle = &pDomain->pCallbacks[i];
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;
}

/* =========================================================================
**  Function : Clock_UnregisterCallback
** =========================================================================*/
/**
  This function removes the registered callback.

  @param nClock [in] - Clock to register callback.
  @param pHandle [out] - Returned handle to the callback.

  @return
  DAL_SUCCESS -- Clock was valid. The frequency may or may not have been 
                 retrieved (see function description). \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  None
*/
DALResult Clock_UnregisterCallback( ClockIdType nClock, ClockCallbackHandle pHandle )
{
  ClockDomainCallbackType *pDomainCallback;
  if(pHandle != NULL)
  {
    pDomainCallback = (ClockDomainCallbackType*)pHandle;
    pDomainCallback->pCallback = NULL;
    pDomainCallback->pCallbackData = NULL;
    return DAL_SUCCESS;
  }

  return DAL_ERROR;
}

