/*
===========================================================================
*/
/**
  @file ClockSources.c

  Interface implementation file for the sources management of the
  clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/src/ClockSources.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "HALclk.h"
#include "swevent.h"


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_InitSources
** =========================================================================*/
/*
  See DalClock.h
*/

DALResult Clock_InitSources
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockPropertyValueType PropVal;
  ClockSourceConfigType *pClockSourceFirstBSPData, *pClockSourceBSPData;
  uint32                 nSourceIndex, nSubSourceIndex;
  DALResult              eResult;
  uint32                 nSize;

  /*-----------------------------------------------------------------------*/
  /* Get the list of clock sources from the BSP/XML                        */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("ClockSources", &PropVal);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_GetPropertyValue failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Count the number of clock sources present in the BSP/XML              */
  /*-----------------------------------------------------------------------*/

  /*
   * Get the pointer to the clock sources data
   */
  pClockSourceFirstBSPData = (ClockSourceConfigType *)PropVal;
  pClockSourceBSPData = pClockSourceFirstBSPData;

  /*
   * Go through all sources until we hit the end marker and increment
   * the source count.
   */
  while ((pClockSourceBSPData->eSource != HAL_CLK_SOURCE_NULL) &&
         (pClockSourceBSPData->nFreqHz != 0))
  {
    pDrvCtxt->nNumSources++;
    pClockSourceBSPData++;
  }

  /*
   * Make sure we found at least one source.
   */
  if (pDrvCtxt->nNumSources == 0)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitSources found 0 sources.");
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate memory for the source nodes                                  */
  /*-----------------------------------------------------------------------*/

  nSize = pDrvCtxt->nNumSources * sizeof (*pDrvCtxt->aSources);

  eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aSources);

  if ((eResult != DAL_SUCCESS) || (pDrvCtxt->aSources == NULL) )
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to allocate %d bytes for %d sources.",
      nSize, pDrvCtxt->nNumSources);
    return DAL_ERROR;
  }

  memset((void *)pDrvCtxt->aSources, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Initialize the source nodes                                           */
  /*-----------------------------------------------------------------------*/

  /*
   * Invalidate the look-up array
   */
  for (nSourceIndex = 0; nSourceIndex < HAL_CLK_NUM_OF_SOURCES; nSourceIndex++)
  {
    pDrvCtxt->anSourceIndex[nSourceIndex] = 0xFF;
  }

  pClockSourceBSPData = pClockSourceFirstBSPData;
  for (nSourceIndex = 0; nSourceIndex < pDrvCtxt->nNumSources; nSourceIndex++)
  {
    /*
     * Store the index of this source in the look-up array
     */
    pDrvCtxt->anSourceIndex[pClockSourceBSPData->eSource] = (uint8)nSourceIndex;

    /*
     * Link the source node to its BSP/XML configuration data
     */
    pDrvCtxt->aSources[nSourceIndex].pBSPConfig = pClockSourceBSPData;
    pDrvCtxt->aSources[nSourceIndex].eSource = pClockSourceBSPData->eSource;

    // TODO: This is where we can do any other source init steps.  For example,
    // if a source needs to be initialized (.init_src), then we can do that here.

    /*
     * Move to the next source
     */
    pClockSourceBSPData++;
  }

  /*-----------------------------------------------------------------------*/
  /* Connect up sources that have sub-sources, like PLLs->XO               */
  /*-----------------------------------------------------------------------*/

  for (nSourceIndex = 0; nSourceIndex < pDrvCtxt->nNumSources; nSourceIndex++)
  {
    pClockSourceBSPData = pDrvCtxt->aSources[nSourceIndex].pBSPConfig;

    nSubSourceIndex =
      pDrvCtxt->anSourceIndex[pClockSourceBSPData->HALConfig.eSource];

    if (nSubSourceIndex != 0xFF)
    {
      pDrvCtxt->aSources[nSourceIndex].pSource =
        &pDrvCtxt->aSources[nSubSourceIndex];
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Source initialization complete                                        */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitSources */


/* =========================================================================
**  Function : Clock_SourceOn
** =========================================================================*/
/**
  Enables a source.

  This function performs the final operations required to turn on a
  clock source.  It handles deciding whether to use an NPA client request,
  a HAL source vote, or a direct HAL source enable call.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param nNPAState [in] -- State value to use in any NPA request.

  @return
  None.
*/

static void Clock_SourceOn
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  uint32               nNPAState
)
{
  /*-----------------------------------------------------------------------*/
  /* Assert the voltage requirement.                                       */
  /*-----------------------------------------------------------------------*/

  Clock_VoltageRequest(
    pDrvCtxt, &pSource->eVRegLevel, pSource->pBSPConfig->eVRegLevel, FALSE);

  /*-----------------------------------------------------------------------*/
  /* Enable the source using an NPA client if it exists, or HAL call       */
  /* otherwise.                                                            */
  /*-----------------------------------------------------------------------*/

  if (pSource->NPAHandle != NULL)
  {
    npa_issue_scalar_request(pSource->NPAHandle, nNPAState);
  }
  else if (pSource->pBSPConfig->nConfigMask & CLOCK_CONFIG_PLL_FSM_MODE_ENABLE)
  {
    HAL_clk_EnableSourceVote(pSource->eSource);
  }
  else
  {
    HAL_clk_EnableSource(pSource->eSource);
  }

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pSource, LOG_STATE_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_STATE_CHANGE))
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                      "source_%d (Enabled: 1)", pSource->eSource);
  }

  SWEVENT(CLOCK_EVENT_PLL_STATUS, pSource->eSource, 1);
} /* END Clock_SourceOn */


/* =========================================================================
**  Function : Clock_SourceOff
** =========================================================================*/
/**
  Disables a source.

  This function performs the final operations required to turn off a
  clock source.  It handles deciding whether to use an NPA client request,
  a HAL source vote, or a direct HAL source disable call.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.

  @return
  None.
*/

static void Clock_SourceOff
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource
)
{
  /*-----------------------------------------------------------------------*/
  /* Disable the source using an NPA client if it exists, or HAL call      */
  /* otherwise.                                                            */
  /*-----------------------------------------------------------------------*/

  if (pSource->NPAHandle != NULL)
  {
    npa_issue_scalar_request(pSource->NPAHandle, CLOCK_SOURCE_NOT_REQUIRED);
  }
  else if (pSource->pBSPConfig->nConfigMask &
             CLOCK_CONFIG_PLL_FSM_MODE_ENABLE)
  {
    HAL_clk_DisableSourceVote(pSource->eSource);
  }
  else
  {
    HAL_clk_DisableSource(pSource->eSource);
  }

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pSource, LOG_STATE_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_STATE_CHANGE))
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                      "source_%d (Enabled: 0)", pSource->eSource);
  }

  SWEVENT(CLOCK_EVENT_PLL_STATUS, pSource->eSource, 0);

  /*-----------------------------------------------------------------------*/
  /* Deassert the voltage requirement.                                     */
  /*-----------------------------------------------------------------------*/

  Clock_VoltageRequest(pDrvCtxt, &pSource->eVRegLevel, CLOCK_VREG_LEVEL_LOW, FALSE);

} /* END Clock_SourceOff */


/* =========================================================================
**  Function : Clock_EnableSource
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_EnableSource
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bCPURequest
)
{
  /*-----------------------------------------------------------------------*/
  /* Make sure the source handle is valid.                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Process non-CPU requests.                                             */
  /*-----------------------------------------------------------------------*/

  if (!bCPURequest)
  {
    if (pSource->nReferenceCount == 0)
    {
      /*
       * If the source has a source, then recursively enable that.
       * If the current state is enabled for the CPU, then we need to
       * release the previously registered vote.
       */
      if (pSource->pSource != NULL)
      {
        Clock_EnableSource(pDrvCtxt, pSource->pSource, FALSE);
      }

      /*
       * Turn on the source if it is not already on.  If there is already
       * a CPU reference count we just need to update the NPA state to
       * '2' to indicate a non-CPU request is active.
       */
      if (pSource->nReferenceCountCPU == 0)
      {
        Clock_SourceOn(pDrvCtxt, pSource, CLOCK_SOURCE_REQUIRED);
      }
      else if (pSource->NPAHandle != NULL)
      {
        npa_issue_scalar_request(pSource->NPAHandle, CLOCK_SOURCE_REQUIRED);
      }
    }

    /*
     * Increment the reference count.
     */
    pSource->nReferenceCount++;
  }

  /*-----------------------------------------------------------------------*/
  /* Process CPU request.                                                  */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (pSource->nReferenceCountCPU == 0)
    {
      /*
       * If the source has a source, then recursively enable that.
       */
      if (pSource->pSource != NULL)
      {
        Clock_EnableSource(pDrvCtxt, pSource->pSource, TRUE);
      }

      /*
       * If the source is not already enabled by a non-CPU request then
       * enable it.  Use "1" for any NPA request to indicate CPU-based.
       */
      if (pSource->nReferenceCount == 0)
      {
        Clock_SourceOn(pDrvCtxt, pSource, CLOCK_SOURCE_REQUIRED_FOR_CPU);
      }
    }

    /*
     * Increment the reference count.
     */
    pSource->nReferenceCountCPU++;
  }

  return DAL_SUCCESS;

} /* END Clock_EnableSource */


/* =========================================================================
**  Function : Clock_DisableSource
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_DisableSource
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bCPURequest
)
{
  /*-----------------------------------------------------------------------*/
  /* Make sure the source handle is valid.                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Process non-CPU requests.                                             */
  /*-----------------------------------------------------------------------*/

  if (!bCPURequest)
  {
    /*
     * Decrement non-cpu reference count.
     */
    if (pSource->nReferenceCount > 0)
    {
      pSource->nReferenceCount--;
    }

    /*
     * If the count is zero and the DO_NOT_DISABLE bits aren't set, we can
     * disable the source.
     */
    if ((pSource->nReferenceCount == 0) &&
        !CLOCK_FLAG_IS_SET(pSource, DO_NOT_DISABLE) &&
        !CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_SOURCES))
    {
      /*
       * If there is no CPU reference count then disable the source.
       * If the CPU still has a request on this source then we just set
       * the NPA state to '1'.
       */
      if (pSource->nReferenceCountCPU == 0)
      {
        Clock_SourceOff(pDrvCtxt, pSource);
      }
      else if (pSource->NPAHandle != NULL)
      {
        npa_issue_scalar_request(
          pSource->NPAHandle, CLOCK_SOURCE_REQUIRED_FOR_CPU);
      }

      /*
       * If the source has a source, then recursively disable that.
       */
      if (pSource->pSource != NULL)
      {
        Clock_DisableSource(pDrvCtxt, pSource->pSource, FALSE);
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Process CPU request.                                                  */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (pSource->nReferenceCountCPU > 0)
    {
      pSource->nReferenceCountCPU--;
    }

    if ((pSource->nReferenceCountCPU == 0) &&
        !CLOCK_FLAG_IS_SET(pSource, DO_NOT_DISABLE) &&
        !CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_SOURCES))
    {
      /*
       * Disable the source if there are no non-CPU references.
       */
      if (pSource->nReferenceCount == 0)
      {
        Clock_SourceOff(pDrvCtxt, pSource);
      }

      /*
       * If the source has a source, then recursively remove the CPU request.
       */
      if (pSource->pSource != NULL)
      {
        Clock_DisableSource(pDrvCtxt, pSource->pSource, TRUE);
      }
    }
  }

  return DAL_SUCCESS;

} /* END Clock_DisableSource */


/* =========================================================================
**  Function : Clock_ConfigSource
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_ConfigSource
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockSourceNodeType   *pSource,
  ClockSourceConfigType *pConfig
)
{
  /*-----------------------------------------------------------------------*/
  /* Make sure the source handle is valid.                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL || pConfig == NULL || pSource->pBSPConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Ensure source is idle and owned locally.                              */
  /*-----------------------------------------------------------------------*/

  if (pSource->nReferenceCount != 0)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure the PLL                                                     */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigPLL(pSource->eSource, &pConfig->HALConfig);

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  pSource->pBSPConfig = pConfig;

  return DAL_SUCCESS;

} /* END Clock_ConfigSource */



/* =========================================================================
**  Function : Clock_InitSource
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitSource
(
  ClockDrvCtxt      *pDrvCtxt,
  HAL_clk_SourceType eSource
)
{
  uint8                  nIndex;
  ClockSourceNodeType   *pSource;

  /*-----------------------------------------------------------------------*/
  /* Verify configuration.                                                 */
  /*-----------------------------------------------------------------------*/

  nIndex = pDrvCtxt->anSourceIndex[eSource];

  if (nIndex == 0xFF)
  {
    return DAL_ERROR;
  }

  pSource = &pDrvCtxt->aSources[nIndex];

  if (pSource == NULL || pSource->pBSPConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure PLL frequency to match BSP.                                 */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigPLL(
    pSource->eSource, &pSource->pBSPConfig->HALConfig);

  /*-----------------------------------------------------------------------*/
  /* Configure PLL parameters as well.                                     */
  /*-----------------------------------------------------------------------*/

  if (pSource->pBSPConfig->nConfigMask & CLOCK_CONFIG_PLL_FSM_MODE_ENABLE)
  {
    HAL_clk_ConfigSource(eSource, HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE);
  }

  if (pSource->pBSPConfig->nConfigMask & CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(eSource, HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE);
  }

  if (pSource->pBSPConfig->nConfigMask & CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(eSource, HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE);
  }

  if (pSource->pBSPConfig->nConfigMask & CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(eSource, HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE);
  }

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitSource */

