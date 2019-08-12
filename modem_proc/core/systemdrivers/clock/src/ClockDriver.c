/*
===========================================================================
*/
/**
  @file ClockDriver.c

  Interface implementation file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/src/ClockDriver.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "HALclk.h"
#include "DDIChipInfo.h"
#include "VCSDefs.h"

#include "ClockSWEVT.h"
#include "sys_m_smsm.h"


/*=========================================================================
      Macros
==========================================================================*/


/*
 * Event marker for the SMP2P SSR context creation
 */
#define CLOCK_SSR_SMP2P_INITED "/ssr/smp2p/inited"


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

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult               eResult;
  int32                   nSMSMResult;
  HAL_clk_ContextType     HALClkCtxt = {0};
  ClockPropertyValueType  PropVal;
  HAL_clk_HWIOBaseType   *pBases = NULL;

  /*-----------------------------------------------------------------------*/
  /* Create the driver synchronization object.                             */
  /*-----------------------------------------------------------------------*/

  eResult = DALSYS_SyncCreate (DALSYS_SYNC_ATTR_RESOURCE,
                               &pDrvCtxt->hLock, &pDrvCtxt->LockObj);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "DALSYS_SyncCreate failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the log.                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("ClockLogDefaults", &PropVal);

  if (eResult == DAL_SUCCESS)
  {
    ULogFront_RealTimeInit (&pDrvCtxt->hClockLog, "Clock Log",
                            ((ClockLogType *)PropVal)->nLogSize,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);

    pDrvCtxt->nGlobalFlags |= ((ClockLogType *)PropVal)->nGlobalLogFlags;
  }
  else
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
      "Missing clock log defaults. Logging is disabled.");

    ULogFront_RealTimeInit (&pDrvCtxt->hClockLog, "Clock Log",
                            0,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  }

  ULogCore_HeaderSet (pDrvCtxt->hClockLog,
                      "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Initialize the clock HAL.                                             */
  /*-----------------------------------------------------------------------*/

  /*
   * Map HAL regions.
   */
  HAL_clk_GetHWIOBases(&pBases);

  if (pBases != NULL)
  {
    while (pBases->pnBaseAddr != NULL)
    {
      Clock_MapHWIORegion(
        pBases->nPhysAddr, pBases->nSize, pBases->pnBaseAddr);
      pBases++;
    }
  }

  /*
   * Use DALSys busy wait function for HAL modules.
   */
  HALClkCtxt.BusyWait = DALSYS_BusyWait;

  /*
   * Get chip version to send to the HAL.
   */
  HALClkCtxt.nChipVersion = DalChipInfo_ChipVersion();
  HALClkCtxt.nChipId = DalChipInfo_ChipId();
  HALClkCtxt.nChipFamily = DalChipInfo_ChipFamily();

  HAL_clk_Init(&HALClkCtxt);

  /*-----------------------------------------------------------------------*/
  /* Mark driver initialization in progress.                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->nInitVotesRemaining = 1;

  /*-----------------------------------------------------------------------*/
  /* Initialize the core driver.                                           */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitCore (pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver.                                 */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitImage (pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    /* TODO: Remove this log once Clock_InitImage has proper logging */
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitImage failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Register a NULL callback for SMP2P SSR context creation marker event. */
  /* The callback is NULL since it's only purpose is to update the number  */
  /* of remaining votes before we invoke the SMSM API.                     */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_RegisterResourceCallback(pDrvCtxt,
                                           CLOCK_SSR_SMP2P_INITED,
                                           NULL,
                                           pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Decrement nInitVotesRemaining to mark end of driver init.             */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  if (pDrvCtxt->nInitVotesRemaining > 0)
  {
    pDrvCtxt->nInitVotesRemaining--;
  }

  /*-----------------------------------------------------------------------*/
  /* Invoke SMSM API to signal the driver has completed its initial votes  */
  /* if the number of remaining initial votes has already reached zero.    */
  /* A value of zero at this point means we are at the end of driver init  */
  /* and all the callbacks registered via Clock_RegisterResourceCallback   */
  /* have executed.                                                        */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->nInitVotesRemaining == 0)
  {
    nSMSMResult = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_PWR_CLK_RDY);

    if (nSMSMResult != SYS_M_SMP2P_SUCCESS)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
        "Unable to signal that the clock driver is done initial votes.");
    }
  }

  DALCLOCK_FREE(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Run the BIST if enabled.                                              */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->nBISTLogSize == 0)
  {
    pDrvCtxt->nBISTLogSize = CLOCK_BIST_DEFAULT_LOG_SIZE;
  }

  if (pDrvCtxt->bBISTEnabled)
  {
    Clock_BIST (pDrvCtxt);
  }

  
  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_DriverInit */


/* =========================================================================
**  Function : ClockStub_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver stub.

  This function initializes the clock driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver stub.                            */
  /*-----------------------------------------------------------------------*/

  eResult = ClockStub_InitImage(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "ClockStub_InitImage failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ClockStub_DriverInit */


/* =========================================================================
**  Function : Clock_InitCore
** =========================================================================*/
/**
  Initialize the core portion of the clock driver.

  This function initializes the common portion of the clock driver that is
  shared with all images.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitCore
(
  ClockDrvCtxt *pDrvCtxt
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

    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %lu bytes for %lu clock domains.",
        nSize, pDrvCtxt->nNumClockDomains);
      return eResult;
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

    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %lu bytes for %lu clocks.",
        nSize, pDrvCtxt->nNumClocks);
      return eResult;
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

    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %lu bytes for %lu power domains.",
        nSize, pDrvCtxt->nNumPowerDomains);
      return eResult;
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
    /* Store the HAL clock domain handle in the driver clock domain node */
    pDrvCtxt->aClockDomains[nClockDomainIndex].HALHandle =
      HALClockDomainHandle;

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
        "HAL_clk_GetNextClockInDomain returned 0 (ClockDomain Index: %lu).", nClockDomainIndex);
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
  /* Init clock/domain flags.                                              */
  /*-----------------------------------------------------------------------*/

  Clock_InitFlags(pDrvCtxt);

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

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
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
**  Function : ClockStub_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver stub.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Shut-down complete.                                                   */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ClockStub_DriverDeInit */


/* =========================================================================
**  Function : Clock_GetClockId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetClockId
(
  ClockDrvCtxt *pDrvCtxt,
  const char   *szClock,
  ClockIdType  *pnId
)
{
  uint32   nClockIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szClock || !pnId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize output parameter                                           */
  /*-----------------------------------------------------------------------*/

  *pnId = (ClockIdType)NULL;

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
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetClockId */


/* =========================================================================
**  Function : Clock_EnableClockDomain
** =========================================================================*/
/**
  Enables a clock domain.

  This function enables a clock domain, generally as a result of a call to
  Clock_EnableClock().

  @param *pDrvCtxt [in] -- Pointer to the driver context.
  @param *pClockDomain [in] -- Pointer to a clock domain node.
  @param  nClockFlags  [in] -- Flags for any clock attributes.

  @return
  DAL_SUCCESS -- The domain was successfully enabled.
  DAL_ERROR -- The domain was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_EnableClockDomain
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pClockDomain,
  uint32               nClockFlags
)
{
  DALResult             eResult;
  boolean               bSuppressibleRequest;
  ClockVRegRequestType  VRegRequest;

  /* Make sure the domain handle is valid */
  if (pClockDomain == NULL)
  {
    return DAL_ERROR;
  }

  /* Check if the clock or domain are suppressible */
  if ((nClockFlags & CLOCK_FLAG_SUPPRESSIBLE) != 0 ||
      CLOCK_FLAG_IS_SET(pClockDomain, SUPPRESSIBLE))
  {
    bSuppressibleRequest = TRUE;
  }
  else
  {
    bSuppressibleRequest = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Process insuppressible requests.                                      */
  /*-----------------------------------------------------------------------*/

  if (!bSuppressibleRequest)
  {
    /*
     * We enable the source for the first insuppressible clock even if
     * another suppressible clock has already enabled the clock domain
     * because we need to keep reference counting within the source.
     */
    if (pClockDomain->nReferenceCount == 0)
    {
      /* If this domain has a controllable source, enable it first */
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pClockDomain->pSource, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * When there is no suppressible reference count, make the initial voltage
       * request. When there is already a suppressible reference count, change
       * the request to suppressible.
       */
      if (!CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
          pClockDomain->pActiveMuxConfig != NULL)
      {
        VRegRequest.eVRegLevel = pClockDomain->pActiveMuxConfig->eVRegLevel;
        VRegRequest.bSuppressible = FALSE;
        Clock_VoltageRequest(pDrvCtxt, &pClockDomain->VRegRequest, &VRegRequest);
      }
    }

    /*
     * Increment the insuppressible reference count.
     */
    pClockDomain->nReferenceCount++;
  }

  /*-----------------------------------------------------------------------*/
  /* Process suppressible request.                                         */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * We enable the source for the first suppressible clock even if
     * another insuppressible clock has already enabled the clock domain
     * because we need to keep reference counting within the source.
     */
    if (pClockDomain->nReferenceCountSuppressible == 0)
    {
      /* If this domain has a controllable source, enable it first */
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pClockDomain->pSource, TRUE);
      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * Only make the voltage request if there is no insuppressible reference
     * count, since the insuppressible voltage requests take priority.
     */
    if (pClockDomain->nReferenceCount == 0 &&
        !CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
        pClockDomain->pActiveMuxConfig != NULL)
    {
      VRegRequest.eVRegLevel = pClockDomain->pActiveMuxConfig->eVRegLevel;
      VRegRequest.bSuppressible = TRUE;
      Clock_VoltageRequest(pDrvCtxt, &pClockDomain->VRegRequest, &VRegRequest);
    }
  }

    /*
     * Increment the reference count.
     */
    pClockDomain->nReferenceCountSuppressible++;
  }

  return DAL_SUCCESS;

} /* END Clock_EnableClockDomain */


/* =========================================================================
**  Function : Clock_DisableClockDomain
** =========================================================================*/
/**
  Disables a clock domain.

  This function disables a clock domain, generally as a result of a call to
  Clock_DisableClock().

  @param *pDrvCtxt     [in] -- Pointer to the driver context.
  @param *pClockDomain [in] -- Pointer to a clock domain node.
  @param  nClockFlags  [in] -- Flags for any clock attributes.

  @return
  DAL_SUCCESS -- The domain was successfully disabled.
  DAL_ERROR -- The domain was not disabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DisableClockDomain
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pClockDomain,
  uint32               nClockFlags
)
{
  boolean               bIsOn;
  DALResult             eResult;
  ClockVRegRequestType  VRegRequest;
  uint32                nTimeout = 0;
  boolean               bSuppressibleRequest;

  /* Make sure the domain handle is valid */
  if (pClockDomain == NULL)
  {
    return DAL_ERROR;
  }

  /* Check if the clock or domain are suppressible */
  if ((nClockFlags & CLOCK_FLAG_SUPPRESSIBLE) != 0 ||
      CLOCK_FLAG_IS_SET(pClockDomain, SUPPRESSIBLE))
  {
    bSuppressibleRequest = TRUE;
  }
  else
  {
    bSuppressibleRequest = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* In some cases when disabling the last clock in a domain that is       */
  /* configured to a low frequency, it can take extra time for the clock   */
  /* root to receive the "off" signal.  If the "off" signal has not been   */
  /* received before the source is turned off, the clock domain can be     */
  /* left in an unexpected state, causing a hang when switching the domain */
  /* frequency.  To avoid disabling the source before the clock domain     */
  /* receives this off signal, if there are no more clocks enabled in this */
  /* domain, wait for the root-off signal from HW.                         */
  /*-----------------------------------------------------------------------*/

  if ((pClockDomain->nReferenceCount +
       pClockDomain->nReferenceCountSuppressible) == 1)
  {
    if (CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_WAIT_FOR_ROOT_OFF))
    {
      if (HAL_clk_IsClockDomainSupported(
            pClockDomain->HALHandle, HAL_CLK_CLOCK_DOMAIN_API_ISON))
      {
        bIsOn = HAL_clk_IsClockDomainOn(pClockDomain->HALHandle);
        
        while ((bIsOn == TRUE) && (nTimeout++ < 150))
        {
          DALSYS_BusyWait(1);
          bIsOn = HAL_clk_IsClockDomainOn(pClockDomain->HALHandle);
        }
        
        if (nTimeout == 150)
        {
          DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
            "Clock domain root did not turn off after all domain clocks\
            are off");
          return DAL_ERROR_INTERNAL;
        }
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Process insuppressible requests.                                      */
  /*-----------------------------------------------------------------------*/

  if (!bSuppressibleRequest)
  {
    /*
     * We disable the source for the last insuppressible clock even if
     * another suppressible clock is still enabled because we need to keep
     * reference counting within the source.
     */
    if (pClockDomain->nReferenceCount == 1)
    {
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pClockDomain->pSource, FALSE, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * If there is no suppressible reference count, just remove the voltage
       * request. If there is a suppressible reference count, then update the
       * voltage request to be suppressible.
       */
      if (!CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
          pClockDomain->pActiveMuxConfig != NULL)
      {
        if (pClockDomain->nReferenceCountSuppressible == 0)
        {
          VRegRequest.eVRegLevel = VCS_CORNER_OFF;
          VRegRequest.bSuppressible = FALSE;
        }
        else
        {
          VRegRequest.eVRegLevel = pClockDomain->pActiveMuxConfig->eVRegLevel;
          VRegRequest.bSuppressible = TRUE;
        }

        Clock_VoltageRequest(pDrvCtxt, &pClockDomain->VRegRequest, &VRegRequest);
      }
    }

    /*
     * Decrement insuppressible reference count.
     */
    if (pClockDomain->nReferenceCount > 0)
    {
      pClockDomain->nReferenceCount--;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Process suppressible request.                                         */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * We disable the source for the last suppressible clock even if
     * another insuppressible clock is still enabled because we need to
     * keep reference counting within the source.
     */
    if (pClockDomain->nReferenceCountSuppressible == 1)
    {
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pClockDomain->pSource, TRUE, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * Only remove the voltage request if there are no insuppressible
       * reference counts.
       */
      if (pClockDomain->nReferenceCount == 0 &&
          !CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST))
      {
        VRegRequest.eVRegLevel = VCS_CORNER_OFF;
        VRegRequest.bSuppressible = TRUE;
        Clock_VoltageRequest(pDrvCtxt, &pClockDomain->VRegRequest, &VRegRequest);
      }
    }

    /*
     * Decrement suppressible reference count.
     */
    if (pClockDomain->nReferenceCountSuppressible > 0)
    {
      pClockDomain->nReferenceCountSuppressible--;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_DisableClockDomain */


/* =========================================================================
**  Function : Clock_EnableClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableClock
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock
)
{
  ClockNodeType   *pClock = NULL;
  DALResult        eResult;
  boolean          bIsOn;
  uint32           nTimeout = 0;

  /* Cast nClock to a proper pointer */
  pClock = (ClockNodeType*)nClock;

  /* Make sure the clock handle is valid */
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the reference count is zero, we need to enable the clock */
  if (pClock->nReferenceCount == 0)
  {
    /* Enable the domain first */
    eResult =
      Clock_EnableClockDomain(pDrvCtxt, pClock->pDomain, pClock->nFlags);

    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
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

    /* Determine whether this clock supports "IsOn" API functionality */
    if(HAL_clk_IsClockSupported (pClock->HALHandle, HAL_CLK_CLOCK_API_ISON))
    {
      bIsOn = HAL_clk_IsClockOn(pClock->HALHandle);

      /* Wait for the clock to turn ON */
      while (!bIsOn && nTimeout++ < 150)
      {
        DALSYS_BusyWait(1);
        bIsOn = HAL_clk_IsClockOn(pClock->HALHandle);
      }

      /* Log a warning if the clock does not turn ON */
      if(!bIsOn)
      {
        DALSYS_LogEvent (
          DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
            "Unable to turn ON clock: %s.", pClock->szName);
      }
    }
  }

  /* Log clock enable event */
  Clock_SWEvent(
    CLOCK_EVENT_CLOCK_STATUS,
    3,
    HAL_clk_GetTestClockId(pClock->HALHandle),
    1, pClock->nReferenceCount);

  /* Increment the reference count */
  pClock->nReferenceCount++;

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_EnableClock */


/* =========================================================================
**  Function : Clock_DisableClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_DisableClock
(
  ClockDrvCtxt *pDrvCtxt,
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
    return DAL_ERROR_INVALID_PARAMETER;
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
    eResult =
      Clock_DisableClockDomain(pDrvCtxt, pClock->pDomain, pClock->nFlags);

    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }

  }

  /* Decrement the reference count */
  if (pClock->nReferenceCount > 0)
  {
    pClock->nReferenceCount--;
  }

  /* Log clock disable event */
  Clock_SWEvent(
    CLOCK_EVENT_CLOCK_STATUS,
    3,
    HAL_clk_GetTestClockId(pClock->HALHandle),
    0, pClock->nReferenceCount);

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
  if (pBSPConfig == NULL || pMatchingConfig == NULL)
  {
    return DAL_ERROR;
  }

  /* Loop over all configurations */
  while (pBSPConfig->nFreqHz != 0)
  {
    /*
     * Skip configurations that aren't for this chip or version, and configurations
     * that rely on an external source.
     */
    if (!Clock_IsBSPSupported(&pBSPConfig->HWVersion) ||
        ((pBSPConfig->HALConfig.eSource >= HAL_CLK_SOURCE_EXTERNAL) &&
         (pBSPConfig->HALConfig.eSource < HAL_CLK_NUM_OF_EXTERNAL_SOURCES)) )
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
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
)
{
  ClockSourceNodeType *pNewSource;
  DALResult            eResult;
  uint32               nSrcIdx;
  boolean              bVoltageRequest;
  ClockVRegRequestType VRegRequest;

  /* Validate parameters */
  if (pDomain == NULL || pNewConfig == NULL ||
      pNewConfig->HALConfig.eSource >= HAL_CLK_NUM_OF_SOURCES)
  {
    return DAL_ERROR;
  }

  /*
   * Determine if we should make a voltage request.  We do so if this
   * domain is not configured to make requests when enabled, or if it
   * is thus configured and is currently enabled.
   */
  bVoltageRequest =
    (pDomain->nReferenceCount > 0 || pDomain->nReferenceCountSuppressible > 0) &&
    !CLOCK_FLAG_IS_SET(pDomain, DOMAIN_HOLD_VREG_REQUEST);


  /*
   * Voltage requests to make if necessary.
   */
  VRegRequest.eVRegLevel = pNewConfig->eVRegLevel;
  VRegRequest.bSuppressible = (pDomain->nReferenceCount == 0);

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
  if (pNewSource != NULL && pNewConfig->pSourceFreqConfig != NULL)
  {
    eResult = Clock_ConfigSource(
      pDrvCtxt, pNewSource, pNewConfig->pSourceFreqConfig);

    if (eResult != DAL_SUCCESS)
    {
      // TODO error!
      return eResult;
    }
  }

  /*
   * If the domain is active, and the new source is different from the
   * current one then we enable the new source.
   */
  if (pDomain->pSource != pNewSource &&
      pNewSource != NULL)
  {
    /*
     * If there are insuppressible clocks enabled in this clock domain then we
     * need to make an insuppressible request on the new source.
     */
    if (pDomain->nReferenceCount > 0)
    {
      eResult = Clock_EnableSourceInternal(pDrvCtxt, pNewSource, FALSE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * If there are suppressible clocks enabled in this clock domain then we
     * need to make a suppressible request on the new source.
     */
    if (pDomain->nReferenceCountSuppressible > 0)
    {
      eResult = Clock_EnableSourceInternal(pDrvCtxt, pNewSource, TRUE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }
  }

  /*
   * If we are increasing the voltage requirement, do so now.
   */
  if ((bVoltageRequest == TRUE) && 
      (pNewConfig->eVRegLevel > pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDrvCtxt, &pDomain->VRegRequest, &VRegRequest);
  }

  /*
   * Configure the clock MUX
   */
  HAL_clk_ConfigClockMux(pDomain->HALHandle, &pNewConfig->HALConfig);

  /*
   * If we are decreasing the voltage requirement, do so now.
   */
  if ((bVoltageRequest == TRUE) &&
      (pNewConfig->eVRegLevel < pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDrvCtxt, &pDomain->VRegRequest, &VRegRequest);
  }

  /*
   * If the domain is active, and the new source is different from the
   * current then we disable the old source.
   */
  if (pDomain->pSource != pNewSource &&
      pDomain->pSource != NULL)
  {
    /*
     * If there are insuppressible clocks enabled in this clock domain then we
     * need to remove the insuppressible request on the old source.
     */
    if (pDomain->nReferenceCount > 0)
    {
      eResult = Clock_DisableSourceInternal(pDrvCtxt, pDomain->pSource, FALSE, FALSE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * If there are suppressible clocks enabled in this clock domain then we
     * need to remove the suppressible request on the old source.
     */
    if (pDomain->nReferenceCountSuppressible > 0)
    {
      eResult = Clock_DisableSourceInternal(pDrvCtxt, pDomain->pSource, TRUE, FALSE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }
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
  See DDIClock.h
*/

DALResult Clock_SetClockFrequency
(
  ClockDrvCtxt       *pDrvCtxt,
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

  /* Initialize the output parameter */
  if (pnResultFreq != NULL)
  {
    *pnResultFreq = 0;
  }

  /* Make sure the clock handle and domain are valid and supports frequency configuration. */
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pClock->pDomain == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }
  
  if (pClock->pDomain->pBSPConfig == NULL)
  {
    return DAL_ERROR_NOT_SUPPORTED;
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
    return DAL_ERROR_NOT_SUPPORTED;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the new config is the same as the active config, we're done */
  nResultFreq = 0;
  if (pNewConfig != pDomain->pActiveMuxConfig)
  {
    eResult = Clock_SetClockConfig(pDrvCtxt, pDomain, pNewConfig);

    if (eResult == DAL_SUCCESS)
    {
      nResultFreq = pNewConfig->nFreqHz / nMult;

      if (CLOCK_FLAG_IS_SET(pClock, LOG_FREQ_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
      {
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                          "%s (Freq: %lu)",
                          pClock->szName, pNewConfig->nFreqHz);
      }

      /*
       * Only send out SW event when successful.
       */
      Clock_SWEvent(
        CLOCK_EVENT_CLOCK_FREQ,
        2,
        HAL_clk_GetTestClockId(pClock->HALHandle),
        ((nResultFreq) * nMult) );
    }
    else
    {
      eResult = DAL_ERROR_INTERNAL;
    }
  }
  else
  {
    nResultFreq = pDomain->pActiveMuxConfig->nFreqHz;
  }

  if (pnResultFreq != NULL)
  {
    *pnResultFreq = nResultFreq;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return eResult;

} /* END Clock_SetClockFrequency */


/* =========================================================================
**  Function : Clock_CalcGCD
** =========================================================================*/
/**
  Calculates greatest common divisor.

  This function calculates the GCD (greatest common divisor) of two integers.
  This is the standard Euclid's GCD calculation algorithm.

  @param uim_a [in] -- First integer
  @param uim_b [in] -- Second integer

  @return
  uint32 - GCD value

  @dependencies
  None.
*/

static uint32 Clock_CalcGCD
(
  uint32 nA,
  uint32 nB
)
{
  uint32 nC;
  if ((0 == nA) || (0 == nB))
  {
    return 0;
  }
  do
  {
    nC = nA%nB;

    if(0 == nC)
    {
      break;
    }

    nA = nB;

    nB = nC;
    /* nB is always less than nA*/
  }while(nA > nB);

  return nB;

} /* END Clock_CalcGCD */


/* =========================================================================
**  Function : Clock_SetUARTBaudClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetUARTBaudClock
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockIdType         nClock,
  ClockIdType         nRefClock,
  uint32              nDI,
  uint32              nFI,
  uint32             *pnResultFreqHz
)
{
  ClockNodeType             *pClock;
  ClockNodeType             *pRefClock;
  ClockDomainNodeType       *pDomain;
  DALResult                  eResult;
  ClockMuxConfigType        *pClockUARTConfig;
  ClockMuxConfigType        *pActiveMuxConfig;
  uint32                     nSize;
  uint32                     nMVal, nNVal, nMNGCD;
  uint32                     nSrcIdx;
  ClockMuxConfigType        *pClockGSBIUARTBSPMuxConfig = NULL;
  ClockPropertyValueType     PropVal;
  uint32                     nResultFreqHz = 0;
  ClockSourceFreqConfigType *pSourceFreqConfig;

  /*-----------------------------------------------------------------------*/
  /* Cast nClock and nRefClock to proper pointers.                         */
  /*-----------------------------------------------------------------------*/

  pClock = (ClockNodeType *)nClock;
  pRefClock = (ClockNodeType *)nRefClock;

  /*-----------------------------------------------------------------------*/
  /* Initialize the output parameter.                                      */
  /*-----------------------------------------------------------------------*/

  if (pnResultFreqHz != NULL)
  {
    *pnResultFreqHz = 0;
  }

  /*-----------------------------------------------------------------------*/
  /* Check whether clock handle and domain are valid.                      */
  /*-----------------------------------------------------------------------*/

  if ((pClock == NULL) || (pRefClock == NULL))
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }
  
  if (pClock->pDomain == NULL                    ||
      pRefClock->pDomain == NULL                 ||
      pRefClock->pDomain->pActiveMuxConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the reference clock's active configuration.                       */
  /*-----------------------------------------------------------------------*/

  pActiveMuxConfig = pRefClock->pDomain->pActiveMuxConfig;

  /*-----------------------------------------------------------------------*/
  /* Get GSBI UART configuration.                                          */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("UARTClockConfig", &PropVal);

  if (eResult == DAL_SUCCESS)
  {
    pClockGSBIUARTBSPMuxConfig = (ClockMuxConfigType *)PropVal;
  }
  else
  {
    return DAL_ERROR_INTERNAL;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  pDomain = pClock->pDomain;

  /*-----------------------------------------------------------------------*/
  /* Find source associated with SIM configuration.                        */
  /*-----------------------------------------------------------------------*/

  nSrcIdx = pDrvCtxt->anSourceIndex[pActiveMuxConfig->HALConfig.eSource];
  if (nSrcIdx != 0xFF)
  {
    /*
     *  Compute new MND value base on:
     *  M = 16 * DI / ((GREATEST_COMMON_DIVISOR(M, N))
     *  N = nSimDivider * FI / (GREATEST_COMMON_DIVISOR(M, N))
     */
    nMVal = 16 * nDI;
    nNVal = (pActiveMuxConfig->HALConfig.nDiv2x * nFI) / 2;

    nMNGCD = Clock_CalcGCD(nMVal, nNVal);
    nMVal /= nMNGCD;
    nNVal /= nMNGCD;

    /*
     * Calculate final GSBI UART clock frequency.
     */
    pSourceFreqConfig = pDrvCtxt->aSources[nSrcIdx].pActiveFreqConfig;
    nResultFreqHz = pSourceFreqConfig->nFreqHz * nMVal / nNVal;

    /*
     * Allocate dynamic memory for this new configuration
     */
    nSize = sizeof(ClockMuxConfigType);
    eResult = DALSYS_Malloc(nSize, (void **)&pClockUARTConfig);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %lu bytes for UART clock configuration.", nSize);

      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INSUFFICIENT_MEMORY;
    }
    memset((void *)pClockUARTConfig, 0x0, nSize);

    /*
     * Update configuration parameters
     */
    pClockUARTConfig->nFreqHz            = nResultFreqHz;
    pClockUARTConfig->HALConfig.eSource  = pActiveMuxConfig->HALConfig.eSource;
    pClockUARTConfig->HALConfig.nDiv2x   = 0;
    pClockUARTConfig->HALConfig.nM       = nMVal;
    pClockUARTConfig->HALConfig.nN       = nNVal;
    pClockUARTConfig->HALConfig.n2D      = nNVal;
    pClockUARTConfig->eVRegLevel         = VCS_CORNER_NOMINAL;

    /*
     * Figure out what voltage we need to set for this frequency.
     *
     * NOTE: The following is not accurate since if our frequency value
     * is in between two different voltage levels then we could be picking up
     * higher voltage level than needed.  Better way is to have Fmax low, nominal
     * and high levels for UART in the BSP/XML and then doing comparison that way.
     * Our voltage level, for now though, is probably going to be VCS_CORNER_LOW
     * after following loop is executed - so this is sufficient for now.
     */
    while (pClockGSBIUARTBSPMuxConfig->nFreqHz != 0)
    {
      if(nResultFreqHz <= pClockGSBIUARTBSPMuxConfig->nFreqHz)
      {
        pClockUARTConfig->eVRegLevel = pClockGSBIUARTBSPMuxConfig->eVRegLevel;
        break;
      }
      pClockGSBIUARTBSPMuxConfig++;
    }

    /*
     * Finally, we can set the new configuration for this clock.
     */
    eResult = Clock_SetClockConfig(pDrvCtxt, pDomain, pClockUARTConfig);

    if (eResult == DAL_SUCCESS)
    {
      if (CLOCK_FLAG_IS_SET(pClock, LOG_FREQ_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
      {
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                          "%s (Freq: %lu)",
                          pClock->szName, pClockUARTConfig->nFreqHz);
      }

      /*
       * Set dynamic config flag
       */
      CLOCK_FLAG_SET(pDomain, DOMAIN_DYNAMIC_CONFIG);
    }
    else
    {
      /*
       * New configuration could not be set, so free up memory and
       * set resulting frequency value to 0.
       */
      DALSYS_Free (pClockUARTConfig);
      nResultFreqHz = 0;
      eResult = DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update resulting frequency.                                           */
  /*-----------------------------------------------------------------------*/

  if (pnResultFreqHz != NULL)
  {
    *pnResultFreqHz = nResultFreqHz;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return eResult;

} /* END Clock_SetUARTBaudClock */


/* =========================================================================
**  Function : Clock_SetClockDivider
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetClockDivider
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  uint32        nDivider
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure the hardware divider.                                       */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  HAL_clk_ConfigClockDivider(pClock->HALHandle, nDivider);

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pClock, LOG_CONFIG_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_CONFIG_CHANGE))
  {
    ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                      "%s (Divider: %lu)",
                      pClock->szName, nDivider);
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_SetClockDivider */


/* =========================================================================
**  Function : Clock_InvertClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_InvertClock
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  boolean       bInvert
)
{

  /*-----------------------------------------------------------------------*/
  /* Not supported.                                                        */
  /*-----------------------------------------------------------------------*/

  return DAL_ERROR_NOT_SUPPORTED;

} /* END Clock_InvertClock */


/* =========================================================================
**  Function : Clock_ResetClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ResetClock
(
  ClockDrvCtxt   *pDrvCtxt,
  ClockIdType     nClock,
  ClockResetType  eReset
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Reset clock.                                                          */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  if (eReset == CLOCK_RESET_ASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(pClock->HALHandle, TRUE);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_RESET) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_RESET))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Reset: 1)", pClock->szName);
    }
  }

  if (eReset == CLOCK_RESET_DEASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(pClock->HALHandle, FALSE);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_RESET) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_RESET))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Reset: 0)", pClock->szName);
    }
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_ResetClock */

/* =========================================================================
**  Function : Clock_ResetClock_STM
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ResetClock_STM
(
  ClockDrvCtxt   *pDrvCtxt,
  ClockIdType     nClock,
  ClockResetType  eReset
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Reset clock.                                                          */
  /*-----------------------------------------------------------------------*/

  if (eReset == CLOCK_RESET_ASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(pClock->HALHandle, TRUE);
  }
  if (eReset == CLOCK_RESET_DEASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(pClock->HALHandle, FALSE);
  }

  return DAL_SUCCESS;

} /* END Clock_ResetClock_STM */


/* =========================================================================
**  Function : Clock_IsClockEnabled
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_IsClockEnabled
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  boolean      *pbIsEnabled
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check both pointer arguments.                                  */
  /*-----------------------------------------------------------------------*/

  if (pbIsEnabled == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Initialize the output argument here.                                  */
  /*-----------------------------------------------------------------------*/

  *pbIsEnabled = FALSE;
  
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from the HAL.                                               */
  /*-----------------------------------------------------------------------*/

  *pbIsEnabled = HAL_clk_IsClockEnabled(pClock->HALHandle);

  return DAL_SUCCESS;

} /* END Clock_IsClockEnabled */


/* =========================================================================
**  Function : Clock_IsClockOn
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_IsClockOn
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  boolean      *pbIsOn
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check both pointer arguments.                                  */
  /*-----------------------------------------------------------------------*/

  if (pbIsOn == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the output argument here.                                  */
  /*-----------------------------------------------------------------------*/

  *pbIsOn = FALSE;
  
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from the HAL.                                               */
  /*-----------------------------------------------------------------------*/

  *pbIsOn = HAL_clk_IsClockOn(pClock->HALHandle);

  return DAL_SUCCESS;

} /* END Clock_IsClockOn */


/* =========================================================================
**  Function : Clock_GetClockFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetClockFrequency
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  uint32       *pnFrequencyHz
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;
  HAL_clk_ClockMuxConfigType  HALConfig;
  uint32 nFreqHz = 0;
  uint32 nSrcIdx;
  ClockSourceFreqConfigType *pSourceFreqConfig;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL || pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize output parameter                                           */
  /*-----------------------------------------------------------------------*/

  *pnFrequencyHz = 0;

  /*-----------------------------------------------------------------------*/
  /* Get state from current config either local copy or read from HW.      */
  /*-----------------------------------------------------------------------*/

  if (pClock->pDomain->pActiveMuxConfig != NULL)
  {
    nFreqHz = pClock->pDomain->pActiveMuxConfig->nFreqHz;
  }
  else
  {
    DALCLOCK_LOCK(pDrvCtxt);
    
    HAL_clk_DetectClockMuxConfig(pClock->pDomain->HALHandle, &HALConfig);

    /*
     * Here we could loop over supported BSP configs and if found use
     * the frequency there which might be more accurate in some cases.
     */

    nSrcIdx = pDrvCtxt->anSourceIndex[HALConfig.eSource];

    if (nSrcIdx != 0xFF)
    {
      pSourceFreqConfig = pDrvCtxt->aSources[nSrcIdx].pActiveFreqConfig;
      nFreqHz = pSourceFreqConfig->nFreqHz;

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

    DALCLOCK_FREE(pDrvCtxt);
  }

  /*-----------------------------------------------------------------------*/
  /* May need to store and use a clock divider in some cases...            */
  /*-----------------------------------------------------------------------*/

  *pnFrequencyHz = nFreqHz;

  return DAL_SUCCESS;

} /* END Clock_GetClockFrequency */


/* =========================================================================
**  Function : Clock_FindExternalSourceConfig
** =========================================================================*/
/**
  Finds a particular external source configuration in the BSP.

  This function finds a particular external source configuration in the BSP
  based on the specified frequency. The configuration returned should be for
  a frequency that is greater than or equal to the target frequency. However
  if the target frequency is higher than all frequencies listed in the BSP,
  we return the configuration for the highest listed frequency.

  @param *pBSPConfig [in] -- Pointer to a list of configurations
  @param nFreqHz [in] -- Frequency in Hz
  @param **pMatchingConfig [out] -- Matching configuration

  @return
  DAL_SUCCESS -- A matching configuration was found.
  DAL_ERROR -- A matching configuration was not found.

  @dependencies
  None.
*/

static DALResult Clock_FindExternalSourceConfig
(
  ClockMuxConfigType  *pBSPConfig,
  uint32               nFreqHz,
  ClockMuxConfigType **pMatchingConfig
)
{
  ClockMuxConfigType *pLastConfig = NULL;

  /* Make sure the list of configurations isn't empty */
  if (pBSPConfig == NULL || pMatchingConfig == NULL)
  {
    if (pMatchingConfig != NULL)
    {
      *pMatchingConfig = NULL;
    }
    return DAL_ERROR;
  }

  /* Loop over all configurations */
  while (pBSPConfig->nFreqHz != 0)
  {
    /*
     * Skip configurations that aren't for this chip or version, and configurations
     * that do not rely on an external source.
     */
    if (!Clock_IsBSPSupported(&pBSPConfig->HWVersion) ||
        (pBSPConfig->HALConfig.eSource < HAL_CLK_SOURCE_EXTERNAL) ||
        (pBSPConfig->HALConfig.eSource >= HAL_CLK_NUM_OF_EXTERNAL_SOURCES) )
    {
      pBSPConfig++;
      continue;
    }

    /*
     * Return the first configuration whose frequency is greater than or equal to
     * the target frequency.
     */
    if (pBSPConfig->nFreqHz >= nFreqHz)
    {
      *pMatchingConfig = pBSPConfig;
      return DAL_SUCCESS;
    }

    /* Save the last available configuration. */
    pLastConfig = pBSPConfig;

    /* Move to the next configuration */
    pBSPConfig++;
  }

  /* Return the last available configuration, or fail. */
  if (pLastConfig != NULL)
  {
    *pMatchingConfig = pLastConfig;
    return DAL_SUCCESS;
  }

  /* No match was found */
  *pMatchingConfig = NULL;

  return DAL_ERROR;

} /* END Clock_FindExternalSourceConfig */


/* =========================================================================
**  Function : Clock_SelectExternalSource
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SelectExternalSource
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  uint32        nFreqHz,
  uint32        nSource,
  uint32        nDiv2x,
  uint32        nM,
  uint32        nN,
  uint32        n2D
)
{
  HAL_clk_ClockMuxConfigType  Config;
  ClockNodeType              *pClock = (ClockNodeType *)nClock;
  ClockDomainNodeType        *pDomain;
  ClockMuxConfigType         *pNewConfig = NULL;
  boolean                     bVoltageRequest;
  ClockVRegRequestType        VRegRequest;

  /*-----------------------------------------------------------------------*/
  /* Since we are directly mapping input source value to HAL source        */
  /* enum, we need to be bit more careful as to not go beyond the max      */
  /* external HAL source enum.                                             */
  /*-----------------------------------------------------------------------*/

  uint32 nMaxExternalSrcCount = (uint32) (HAL_CLK_NUM_OF_EXTERNAL_SOURCES
                                          - HAL_CLK_SOURCE_EXTERNAL);

  /*-----------------------------------------------------------------------*/
  /* Sanity check the arguments.                                           */
  /*-----------------------------------------------------------------------*/

  if ((pClock            == NULL                ) ||
      (nSource           >= nMaxExternalSrcCount))
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pClock->pDomain == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  pDomain = pClock->pDomain;

  /*
   * Find the voltage corner data for the given frequency, if specified.
   * If the voltage corner data. or the frequency is not specified, then
   * assume the voltage does not matter.
   */
  if (nFreqHz != 0)
  {
    (void) Clock_FindExternalSourceConfig (pDomain->pBSPConfig, nFreqHz, &pNewConfig);
  }

  /*
   * Determine if we should make a voltage request.  We do so if this
   * domain is not configured to make requests when enabled, or if it
   * is thus configured and is currently enabled.
   */
  bVoltageRequest =
    (pDomain->nReferenceCount > 0 || pDomain->nReferenceCountSuppressible > 0) &&
    !CLOCK_FLAG_IS_SET(pDomain, DOMAIN_HOLD_VREG_REQUEST);

  /*
   * Voltage requests to make if necessary.
   */
  if (pNewConfig != NULL)
  {
    VRegRequest.bSuppressible = (pDomain->nReferenceCount == 0);
    VRegRequest.eVRegLevel = pNewConfig->eVRegLevel;
  }

  /*
   * If we are increasing the voltage requirement, do so now.
   */
  if ((pNewConfig != NULL) &&
      (bVoltageRequest == TRUE) &&
      (pNewConfig->eVRegLevel > pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDrvCtxt, &pDomain->VRegRequest, &VRegRequest);
  }

  /*-----------------------------------------------------------------------*/
  /* Define the mux configuration.                                         */
  /*-----------------------------------------------------------------------*/

  Config.eSource =
    (HAL_clk_SourceType)(nSource + (uint32)HAL_CLK_SOURCE_EXTERNAL);
  Config.nDiv2x  = nDiv2x;
  Config.nM      = nM;
  Config.nN      = nN;
  Config.n2D     = n2D;

  /*-----------------------------------------------------------------------*/
  /* Configure the clock to external source.                               */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigClockMux(pDomain->HALHandle, &Config);

  /*
   * If we are decreasing the voltage requirement, do so now.
   */
  if ((pNewConfig != NULL) &&
      (bVoltageRequest == TRUE) && 
      (pNewConfig->eVRegLevel < pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDrvCtxt, &pDomain->VRegRequest, &VRegRequest);
  }

  /*-----------------------------------------------------------------------*/
  /* Update the source tree.                                               */
  /*-----------------------------------------------------------------------*/

  if (pDomain->pSource != NULL && pDomain->nReferenceCount > 0)
  {
    Clock_DisableSourceInternal(
      pDrvCtxt, pDomain->pSource, CLOCK_FLAG_IS_SET(pDomain, SUPPRESSIBLE), FALSE);
  }

  pDomain->pSource = NULL;
  pDomain->pActiveMuxConfig = pNewConfig;

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_SelectExternalSource */


/* =========================================================================
**  Function : Clock_CalcClockFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_CalcClockFrequency
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nClock,
  uint32       *pnFrequencyHz
)
{
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the arguments.                                           */
  /*-----------------------------------------------------------------------*/

  if ((pClock == NULL) || (pnFrequencyHz == NULL))
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the output parameter.                                      */
  /*-----------------------------------------------------------------------*/

  *pnFrequencyHz = 0;

  /*-----------------------------------------------------------------------*/
  /* Call HAL to calculate clock frequency.                                */
  /*-----------------------------------------------------------------------*/

  HAL_clk_CalcClockFreq(pClock->HALHandle, pnFrequencyHz);

  return DAL_SUCCESS;

} /* END Clock_CalcClockFrequency */


/* =========================================================================
**  Function : Clock_GetPowerDomainId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetPowerDomainId
(
  ClockDrvCtxt           *pDrvCtxt,
  const char             *szPowerDomain,
  ClockPowerDomainIdType *pnPowerDomainId
)
{
  uint32   nPowerDomainIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szPowerDomain || !pnPowerDomainId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the output parameter.                                      */
  /*-----------------------------------------------------------------------*/

  *pnPowerDomainId = (ClockPowerDomainIdType)NULL;

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
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetPowerDomainId */


/* =========================================================================
**  Function : Clock_EnablePowerDomain
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnablePowerDomain
(
  ClockDrvCtxt           *pDrvCtxt,
  ClockPowerDomainIdType  nPowerDomainId
)
{
  ClockPowerDomainNodeType *pPowerDomain = NULL;

  /* Cast nPowerDomainId to a proper pointer */
  pPowerDomain = (ClockPowerDomainNodeType*)nPowerDomainId;

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the reference count is zero, we need to enable the power domain */
  if (pPowerDomain->nReferenceCount == 0)
  {
    /* Enable the power domain */
    if (!pPowerDomain->bEnabled)
    {
      HAL_clk_EnablePowerDomain (pPowerDomain->HALHandle);
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
/*
  See DDIClock.h
*/

DALResult Clock_DisablePowerDomain
(
  ClockDrvCtxt           *pDrvCtxt,
  ClockPowerDomainIdType  nPowerDomainId
)
{
  ClockPowerDomainNodeType *pPowerDomain = NULL;

  /* Cast nPowerDomainId to a proper pointer */
  pPowerDomain = (ClockPowerDomainNodeType*)nPowerDomainId;

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
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


/* =========================================================================
**  Function : Clock_ConfigClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ConfigClock
(
  ClockDrvCtxt     *pDrvCtxt,
  ClockIdType      nClock,
  ClockConfigType  eConfig
)
{
  DALResult eResult = DAL_SUCCESS;
  ClockNodeType *pClock = (ClockNodeType *)nClock;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Since we have mapped the lower bits of HAL_clk_ClockConfigType to be  */
  /* the same as ClockConfigType, we can just pass the value directly to   */
  /* HAL layer.  Thus, only doing range check here.                        */
  /*-----------------------------------------------------------------------*/

  if(((uint32)eConfig > 0) && (eConfig < CLOCK_CONFIG_TOTAL))
  {
    HAL_clk_ConfigClock(pClock->HALHandle, (HAL_clk_ClockConfigType) eConfig);
  }
  else
  {
    eResult = DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return eResult;

} /* END Clock_ConfigClock */


/* =========================================================================
**  Function : Clock_LogState
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_LogState
(
  ClockDrvCtxt        *pDrvCtxt,
  ULogHandle           hULog,
  uint32               nFlags
)
{
  ClockNodeType               *pClock;
  ClockPowerDomainNodeType    *pPowerDomain;
  uint32                       i, eSource, nFreqHz, nFreqHzConfig;
  int32                        nIsOn, nIsEnabled, nReset;
  boolean                      bIsEnabled, bIsOn, bCalcSupported;
  DALResult                    eResult;

  if(hULog == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  ULOG_RT_PRINTF_0(hULog, ",CLOCK,ON,ENABLED,RESET,REF COUNT,FREQ CONFIG,FREQ CALC,VOLTAGE,SOURCE");

  /*------------------------------------------------------------------------*/
  /* Log clock state.                                                       */
  /*------------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumClocks; i++)
  {
    pClock = &pDrvCtxt->aClocks[i];
    nIsEnabled = nIsOn = nReset = -1;
    eSource = nFreqHzConfig = 0;
    bCalcSupported = FALSE;

    /*
     * Get enabled state.
     */
    if (HAL_clk_IsClockSupported(pClock->HALHandle, HAL_CLK_CLOCK_API_ISENABLED))
    {
      Clock_IsClockEnabled(pDrvCtxt, (ClockIdType)pClock, &bIsEnabled);
      nIsEnabled = bIsEnabled ? 1 : 0;
    }

    /*
     * Get ON state.
     */
    if (HAL_clk_IsClockSupported(pClock->HALHandle, HAL_CLK_CLOCK_API_ISON))
    {
      Clock_IsClockOn(pDrvCtxt, (ClockIdType)pClock, &bIsOn);
      nIsOn = bIsOn ? 1 : 0;
    }

    /*
     * Get reset state.
     */
    if (HAL_clk_IsClockSupported(pClock->HALHandle, HAL_CLK_CLOCK_API_RESET))
    {
      nReset = HAL_clk_IsClockReset(pClock->HALHandle);
    }

    /*
     * Get frequency.
     */
    if (pClock->pDomain->pActiveMuxConfig != NULL)
    {
      nFreqHzConfig = pClock->pDomain->pActiveMuxConfig->nFreqHz;
    }

    /*
     * Calculate frequency.
     */
    if ((nFlags & CLOCK_LOG_STATE_CLOCK_FREQUENCY) &&
        HAL_clk_IsClockSupported(pClock->HALHandle, HAL_CLK_CLOCK_API_CALCFREQ))
    {
      bCalcSupported = TRUE;
      Clock_CalcClockFrequency(pDrvCtxt, (ClockIdType)pClock, &nFreqHz);
    }

    /*
     * Get source.
     */
    if (pClock->pDomain->pSource)
    {
      eSource = pClock->pDomain->pSource->eSource;
    }

    /*
     * Log results.
     */
    if (bCalcSupported)
    {
      eResult = ULOG_RT_PRINTF_9(hULog, ",%s,%d,%d,%d,%d,%u,%u,%d,%d",
        pClock->szName, nIsOn, nIsEnabled, nReset, pClock->nReferenceCount,
        nFreqHzConfig, nFreqHz, pClock->pDomain->VRegRequest.eVRegLevel, eSource);
    }
    else
    {
      eResult = ULOG_RT_PRINTF_8(hULog, ",%s,%d,%d,%d,%d,%u,-1,%d,%d",
        pClock->szName, nIsOn, nIsEnabled, nReset, pClock->nReferenceCount,
        nFreqHzConfig, pClock->pDomain->VRegRequest.eVRegLevel, eSource);
    }

    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*------------------------------------------------------------------------*/
  /* Log power domain state.                                                */
  /*------------------------------------------------------------------------*/

  ULOG_RT_PRINTF_0(hULog, "");
  ULOG_RT_PRINTF_0(hULog, "");
  ULOG_RT_PRINTF_0(hULog, ",POWER DOMAIN,ON,ENABLED,REF COUNT");

  for (i = 0; i < pDrvCtxt->nNumPowerDomains; i++)
  {
    pPowerDomain = &pDrvCtxt->aPowerDomains[i];
    bIsEnabled = HAL_clk_IsPowerDomainEnabled(pDrvCtxt->aPowerDomains[i].HALHandle);
    bIsOn = HAL_clk_IsPowerDomainOn(pDrvCtxt->aPowerDomains[i].HALHandle);

    eResult = ULOG_RT_PRINTF_4(hULog, ",%s,%d,%d,%d", pPowerDomain->szName, bIsOn,
      bIsEnabled, pPowerDomain->nReferenceCount);

    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_LogState */


/* =========================================================================
**  Function : Clock_InitFlags
** =========================================================================*/
/**
  Initialize the flag attributes of clock(s)/domain(s).

  This function initializes the flag attributes of particular clock(s)
  and domain(s) as specified in the BSP data.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Flag init was successful.
  DAL_ERROR -- Flag init failed.

  @dependencies
  None.
*/

DALResult Clock_InitFlags
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult                  eResult;
  ClockPropertyValueType     PropVal;
  ClockFlagInitType         *pClockInitFlags;
  ClockNodeType             *pClock;
  ClockPowerDomainNodeType  *pPowerDomain;
  ClockSourceNodeType       *pSource;

  /*-----------------------------------------------------------------------*/
  /* Get the list of clock flags to initialize.                            */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("ClockInitFlags", &PropVal);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
      "Clock_GetPropertyValue failed in Clock_InitFlags.");

    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Init the flag for each source/clock/domain present in the BSP/XML.    */
  /*-----------------------------------------------------------------------*/

  for (pClockInitFlags = (ClockFlagInitType *)PropVal;
       pClockInitFlags->eNodeType != CLOCK_FLAG_NODE_TYPE_NONE;
       pClockInitFlags++)
  {
    /*
     * Validate the target is supported.
     */
    if (pClockInitFlags->eNodeType >= CLOCK_FLAG_NODE_TYPE_MAX_SUPPORTED)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
        "ClockInitFlags contains invalid flags target.");

      /*
       * Skip this flag init and continue onward.
       */
      continue;
    }

    if (pClockInitFlags->eNodeType == CLOCK_FLAG_NODE_TYPE_CLOCK ||
    	pClockInitFlags->eNodeType == CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN)
    {
      /*
       * Get the clock ID.
       */
      eResult =
        Clock_GetClockId(
          pDrvCtxt,
          (const char *)pClockInitFlags->pID,
          (ClockIdType *)&pClock);

      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent (
          DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
          "Unable to get clock ID[%s]", (const char *)pClockInitFlags->pID);

        /*
         * Skip this flag init and continue onward.
         */
        continue;
      }

      /*
       * Update the clock or clock domain flags.
       */
      if (pClockInitFlags->eNodeType == CLOCK_FLAG_NODE_TYPE_CLOCK)
      {
        pClock->nFlags = pClockInitFlags->nFlag;
      }
      else
      {
        pClock->pDomain->nFlags = pClockInitFlags->nFlag;
      }
    }
    else if (pClockInitFlags->eNodeType == CLOCK_FLAG_NODE_TYPE_POWER_DOMAIN)
    {
      /*
       * Get the power domain ID.
       */
      eResult =
        Clock_GetPowerDomainId(
          pDrvCtxt,
          (const char *)pClockInitFlags->pID,
          (ClockPowerDomainIdType *)&pPowerDomain);

      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent (
          DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
          "Unable to get power domain ID[%s]", (const char *)pClockInitFlags->pID);

        /*
         * Skip this flag init and continue onward.
         */
        continue;
      }

      /*
       * Update the power domain flags.
       */
      pPowerDomain->nFlags = pClockInitFlags->nFlag;
    }
    else if (pClockInitFlags->eNodeType == CLOCK_FLAG_NODE_TYPE_SOURCE)
    {
      /*
       * Get the source ID.
       */
      eResult =
        Clock_GetSourceId(
          pDrvCtxt,
          (const char *)pClockInitFlags->pID,
          (SourceIdType *)&pSource);

      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent (
          DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
          "Unable to get source ID[%s]", (const char *)pClockInitFlags->pID);

        /*
         * Skip this flag init and continue onward.
         */
         continue;
      }

      /*
       * Update the source flags.
       */
      pSource->nFlags = pClockInitFlags->nFlag;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_InitFlags */


/* =========================================================================
**  Function : Clock_GetSourceId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetSourceId
(
  ClockDrvCtxt  *pDrvCtxt,
  const char    *szSource,
  SourceIdType  *pnId
)
{
  uint32   nSourceIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szSource || !pnId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize output parameter.                                          */
  /*-----------------------------------------------------------------------*/

  *pnId = (SourceIdType)NULL;

  /*-----------------------------------------------------------------------*/
  /* Search the array of source nodes, break if we find a match            */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nSourceIndex = 0; nSourceIndex < pDrvCtxt->nNumSources; nSourceIndex++)
  {
    if (strcmp (szSource, pDrvCtxt->aSources[nSourceIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the source node if found, otherwise return NULL   */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnId = (SourceIdType)&pDrvCtxt->aSources[nSourceIndex];
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetSourceId */


/* =========================================================================
**  Function : Clock_EnableSource
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableSource
(
  ClockDrvCtxt *pDrvCtxt,
  SourceIdType  nSource
)
{
  ClockSourceNodeType *pSource = NULL;
  DALResult            eResult;

  /* Cast nSource to a proper pointer */
  pSource = (ClockSourceNodeType*)nSource;

  /* Make sure the source handle is valid */
  if (pSource == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*
   * Check if we should bypass the resource's dependencies and directly
   * configure the resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_LOCK(pDrvCtxt);
  }

  /*
   * If the external reference count is zero then we need to call the internal
   * enable source API.
   */
  if (pSource->nReferenceCountExternal == 0)
  {
    eResult = Clock_EnableSourceInternal(pDrvCtxt, pSource, CLOCK_FLAG_IS_SET(pSource, SUPPRESSIBLE));
    if (eResult != DAL_SUCCESS)
    {
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
      {
      DALCLOCK_FREE(pDrvCtxt);
      }
      return DAL_ERROR_INTERNAL;
    }
  }

  /* Increment the reference count */
  pSource->nReferenceCountExternal++;

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_FREE(pDrvCtxt);
  }

  return DAL_SUCCESS;

} /* END Clock_EnableSource */


/* =========================================================================
**  Function : Clock_DisableSource
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_DisableSource
(
  ClockDrvCtxt *pDrvCtxt,
  SourceIdType  nSource
)
{
  ClockSourceNodeType *pSource = NULL;
  DALResult            eResult;

  /* Cast nSource to a proper pointer */
  pSource = (ClockSourceNodeType*)nSource;

  /* Make sure the source handle is valid */
  if (pSource == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*
   * Check if we should bypass the DAL driver and directly configure the
   * resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_LOCK(pDrvCtxt);
  }

  /*
   * If the external reference count is one then we need to call the internal
   * disable source API.
   */
  if (pSource->nReferenceCountExternal == 1)
  {
    eResult = Clock_DisableSourceInternal(pDrvCtxt, pSource, CLOCK_FLAG_IS_SET(pSource, SUPPRESSIBLE), FALSE);
    if (eResult != DAL_SUCCESS)
    {
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
      {
      DALCLOCK_FREE(pDrvCtxt);
      }
      return DAL_ERROR_INTERNAL;
    }
  }

  /* Decrement the reference count */
  if (pSource->nReferenceCountExternal > 0)
  {
    pSource->nReferenceCountExternal--;
  }

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
      {
  DALCLOCK_FREE(pDrvCtxt);
      }

  return DAL_SUCCESS;

} /* END Clock_DisableSource */


/* =========================================================================
**  Function : Clock_SetSourceFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetSourceFrequency
(
  ClockDrvCtxt       *pDrvCtxt,
  SourceIdType        nSource,
  uint32              nFreq,
  ClockFrequencyType  eMatch,
  uint32             *pnResultFreq
)
{
  ClockSourceNodeType        *pSource;
  ClockSourceFreqConfigType  *pNewConfig = NULL;
  DALResult                   eResult;
  uint32                      nFreqHz;
  uint32                      nMult;
  uint32                      nResultFreq;

  /* Cast nSource to a proper pointer */
  pSource = (ClockSourceNodeType *)nSource;

  /* Initialize output parameter */
  if (pnResultFreq != NULL)
  {
    *pnResultFreq = 0;
  }

  /* Make sure the source handle is valid and supports frequency configuration. */
  if (pSource == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

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
  eResult =
    Clock_FindSourceConfig(
      pSource->pBSPConfig->pSourceFreqConfig,
      nFreqHz,
      eMatch,
      &pNewConfig);

  if ((eResult != DAL_SUCCESS) || (pNewConfig == NULL))
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  /*
   * Check if we should bypass the resource's dependencies and directly
   * configure the resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_LOCK(pDrvCtxt);
  }

  /* If the new config is the same as the active config, we're done */
  nResultFreq = 0;
  if (pNewConfig != pSource->pActiveFreqConfig)
  {
    eResult = Clock_ConfigSource(pDrvCtxt, pSource, pNewConfig);

    if (eResult == DAL_SUCCESS)
    {
      nResultFreq = pNewConfig->nFreqHz / nMult;

      if (CLOCK_FLAG_IS_SET(pSource, LOG_FREQ_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
      {
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                          "%s (Freq: %lu)",
                          pSource->szName, pNewConfig->nFreqHz);
      }

      /*
       * Only send out SW event when successful.
       */
      Clock_SWEvent(
        CLOCK_EVENT_SOURCE_FREQ,
        2,
        pSource->eSource,
        ((nResultFreq) * nMult) );
    }
    else
    {
      eResult = DAL_ERROR_INTERNAL;
    }
  }
  else
  {
    nResultFreq = pSource->pActiveFreqConfig->nFreqHz;
  }

  if (pnResultFreq != NULL)
  {
    *pnResultFreq = nResultFreq;
  }

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_FREE(pDrvCtxt);
  }

  return eResult;

} /* END Clock_SetSourceFrequency */


/* =========================================================================
**  Function : Clock_GetSourceFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetSourceFrequency
(
  ClockDrvCtxt *pDrvCtxt,
  SourceIdType  nSource,
  uint32       *pnFrequencyHz
)
{
  ClockSourceNodeType *pSource = (ClockSourceNodeType *)nSource;
  HAL_clk_PLLConfigType HALConfigDetected;
  ClockSourceFreqConfigType *pSourceFreqConfig;
  uint32 nFreqHz = 0;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL || pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the output parameter.                                      */
  /*-----------------------------------------------------------------------*/

  *pnFrequencyHz = 0;

  /*-----------------------------------------------------------------------*/
  /* Get state from current config either local copy or read from HW.      */
  /*-----------------------------------------------------------------------*/

  /*
   * Check if we should bypass the resource's dependencies and directly
   * configure the resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_LOCK(pDrvCtxt);
  }

  if (pSource->pActiveFreqConfig != NULL)
  {
    nFreqHz = pSource->pActiveFreqConfig->nFreqHz;
  }
  else
  {
    HAL_clk_DetectPLLConfig(pSource->eSource, &HALConfigDetected);

    /*
     * Here we loop over supported BSP configs and if found use
     * the frequency there.
     */
    pSourceFreqConfig = pSource->pBSPConfig->pSourceFreqConfig;
    while (pSourceFreqConfig->nFreqHz != 0)
    {
      if (pSourceFreqConfig->HALConfig.nL == HALConfigDetected.nL &&
          pSourceFreqConfig->HALConfig.nM == HALConfigDetected.nM &&
          pSourceFreqConfig->HALConfig.nN == HALConfigDetected.nN &&
          pSourceFreqConfig->HALConfig.nPreDiv == HALConfigDetected.nPreDiv &&
          pSourceFreqConfig->HALConfig.nPostDiv == HALConfigDetected.nPostDiv)
      {
        nFreqHz = pSourceFreqConfig->nFreqHz;
        break;
      }

      /*
       * Go to the next source frequency config.
       */
      pSourceFreqConfig++;
    }
  }

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
  DALCLOCK_FREE(pDrvCtxt);
  }

  *pnFrequencyHz = nFreqHz;

  return DAL_SUCCESS;

} /* END Clock_GetSourceFrequency */


/*=========================================================================
**  Function : Clock_CommonNPACallback
** =========================================================================*/
/**
  A common callback invoked by the NPA framework when a particular resource
  becomes available. This is a wrapper function which is only used to
  invoke callbacks registered via Clock_RegisterResourceCallback.

  This function invokes the actual resource callback stored in the input
  context. When all callbacks have been invoked, we report we are done with
  our initial votes via the SMSM bit.

  @param *pContext         [in] -- Context passed in npa_define_node_cb
  @param  nEventType       [in] -- Zero.
  @param *pResourceName    [in] -- Name of the node being created.
  @param  nResourceNameLen [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_CommonNPACallback
(
  void  *pContext,
  uint32 nEventType,
  void  *pResourceName,
  uint32 nResourceNameLen
)
{
  int32                  nSMSMResult;
  ClockCallbackCtxtType *pClockCallbackCtxt;
  ClockDrvCtxt          *pDrvCtxt;

  /*-----------------------------------------------------------------------*/
  /* Invoke actual resource callback.                                      */
  /*-----------------------------------------------------------------------*/

  pClockCallbackCtxt = (ClockCallbackCtxtType *)pContext;

  if ((pClockCallbackCtxt == NULL)               ||
      (pClockCallbackCtxt->pDriverContext == NULL))
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "NPA callback context and/or driver context is null!");
  }
  else
  {
    if (pClockCallbackCtxt->pCallback != NULL)
    {
      pClockCallbackCtxt->pCallback(pClockCallbackCtxt->pCallbackContext);
    }

    pDrvCtxt = (ClockDrvCtxt *)pClockCallbackCtxt->pDriverContext;

    /*-----------------------------------------------------------------------*/
    /* Free the memory allocated in Clock_RegisterResourceCallback.          */
    /* These are the only two places we allocate and deallocate memory for   */
    /* the callback context.                                                 */
    /*-----------------------------------------------------------------------*/

    DALSYS_Free(pClockCallbackCtxt);

    /*-----------------------------------------------------------------------*/
    /* Ensure atomicity.                                                     */
    /*-----------------------------------------------------------------------*/

    DALCLOCK_LOCK(pDrvCtxt);

    /*-----------------------------------------------------------------------*/
    /* Decrement number of initial votes remaining.                          */
    /*-----------------------------------------------------------------------*/

    if (pDrvCtxt->nInitVotesRemaining > 0)
    {
      pDrvCtxt->nInitVotesRemaining--;
    }

    /*-----------------------------------------------------------------------*/
    /* Invoke SMSM API if the number of initial votes remaining reaches zero.*/
    /* This means all the registered callbacks have been invoked and         */
    /* driver initialization is complete.                                    */
    /*-----------------------------------------------------------------------*/

    if (pDrvCtxt->nInitVotesRemaining == 0)
    {
      nSMSMResult = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_PWR_CLK_RDY);

      if (nSMSMResult != SYS_M_SMP2P_SUCCESS)
      {
        DALSYS_LogEvent (
          DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
          "Unable to signal that the clock driver is done initial votes.");
      }
    }

    /*-----------------------------------------------------------------------*/
    /* Free mutex.                                                           */
    /*-----------------------------------------------------------------------*/

    DALCLOCK_FREE(pDrvCtxt);
  }

} /* END Clock_CommonNPACallback */


/*=========================================================================
**  Function : Clock_RegisterResourceCallback
** =========================================================================*/
/**
  Wrapper function to register a resource callback and increment the number
  of remaining initial votes in the driver context. This is used in
  conjunction with Clock_CommonNPACallback which decrements the remaining
  votes after the actual resource callback executes.

  An example use case is we register CX, MX, and PLL_SR2 callbacks with this
  API, thereby incrementing the number of remaining initial votes three times.
  When each resource becomes available, Clock_CommonNPACallback is invoked
  which in turn invokes the actual resource callback based on the context
  information passed in, and decrements the number of remaining votes. When
  the number of remaining votes reaches zero, we know all the resource
  callbacks registered via this API have been invoked. At that point we
  signal the resource master we have completed our initial votes.

  @param  *pDrvCtxt         [in] -- Handle to the DAL driver context.
  @param  *szResourceName   [in] -- Name of resource for which the callback
                                    is to be invoked.
  @param  (*pCallback)      [in] --  Pointer to the callback function.
  @param  *pCallbackContext [in] -- Context to be passed into the callback

  @return
  DAL_SUCCESS -- Successfully registered the NPA callback.
  DAL_ERROR -- Error allocating memory for the NPA callback context.

  @dependencies
  None.
*/

DALResult Clock_RegisterResourceCallback
(
  ClockDrvCtxt              *pDrvCtxt,
  const char                *szResourceName,
  void                      (*pCallback)(void *),
  void                      *pCallbackContext
)
{
  DALResult              eResult;
  uint32                 nSize;
  ClockCallbackCtxtType *pClockNPACallbackCtxt;

  /*-----------------------------------------------------------------------*/
  /* Ensure atomicity.                                                     */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Increment number of initial votes remaining.                          */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->nInitVotesRemaining++;

  /*-----------------------------------------------------------------------*/
  /* Free mutex.                                                           */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_FREE(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Register NPA callback.                                                */
  /*-----------------------------------------------------------------------*/

  nSize = sizeof(ClockCallbackCtxtType);

  eResult = DALSYS_Malloc(nSize, (void **)&pClockNPACallbackCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to allocate %lu bytes for Clock NPA callback Context.", nSize);

    return DAL_ERROR_INSUFFICIENT_MEMORY;
  }

  memset((void *)pClockNPACallbackCtxt, 0x0, nSize);

  pClockNPACallbackCtxt->pCallback = pCallback;
  pClockNPACallbackCtxt->pCallbackContext = pCallbackContext;
  pClockNPACallbackCtxt->pDriverContext = pDrvCtxt;

  npa_resource_available_cb(
    szResourceName,
    (npa_callback)Clock_CommonNPACallback,
    pClockNPACallbackCtxt);

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_RegisterResourceCallback */


/*=========================================================================
**  Function : Clock_IsBSPSupported
** =========================================================================*/
/**
 *
  See ClockDriver.h
*/

boolean Clock_IsBSPSupported
(
  ClockHWVersionType *pHWVersion
)
{
  uint32                   nHWVersion, nHWMaxVersion, nHWMinVersion;
  DalChipInfoIdType 	   eChipId;
  const DalChipInfoIdType *aeChipInfoId;

  /*-----------------------------------------------------------------------*/
  /* Validate argument pointer is usable.                                  */
  /*-----------------------------------------------------------------------*/

  if (pHWVersion == NULL)
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate chip family. If not provided then we do not check.           */
  /*-----------------------------------------------------------------------*/

  if ( (pHWVersion->eChipInfoFamily != 0) &&
       (pHWVersion->eChipInfoFamily != DalChipInfo_ChipFamily()) )
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Ensure the specific chip ID is supported.                             */
  /*-----------------------------------------------------------------------*/

  aeChipInfoId = pHWVersion->aeChipInfoId;

  if (aeChipInfoId != NULL)
  {
    eChipId = DalChipInfo_ChipId();

    while (*aeChipInfoId < DALCHIPINFO_NUM_IDS)
    {
      if (eChipId == *aeChipInfoId)
      {
        break;
      }

      aeChipInfoId++;
    }

    if (*aeChipInfoId == DALCHIPINFO_NUM_IDS)
    {
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Get the actual hardware revision.                                     */
  /*-----------------------------------------------------------------------*/

  nHWVersion = HAL_clk_GetChipVersion();

  nHWMinVersion =
    DALCHIPINFO_VERSION(pHWVersion->Min.nMajor, pHWVersion->Min.nMinor);
  nHWMaxVersion =
    DALCHIPINFO_VERSION(pHWVersion->Max.nMajor, pHWVersion->Max.nMinor);

  /*-----------------------------------------------------------------------*/
  /* No chip version data implies support in all HW versions.              */
  /*-----------------------------------------------------------------------*/

  if ( (nHWMinVersion == 0) && (nHWMaxVersion == 0) )
  {
    return TRUE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the HW version falls within the min/max.                     */
  /*-----------------------------------------------------------------------*/

  if ( (nHWVersion >= nHWMinVersion) && (nHWVersion < nHWMaxVersion) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

