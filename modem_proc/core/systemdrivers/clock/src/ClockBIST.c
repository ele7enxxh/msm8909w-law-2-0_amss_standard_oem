/*
===========================================================================
*/
/**
  @file ClockBIST.c

  Interface implementation file for the clock device driver
  Built-In-Self-Test (BIST).
*/
/*
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/src/ClockBIST.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "HALclk.h"
#include "npa_resource.h"


/*=========================================================================
      Definitions
==========================================================================*/

/*
 * The measured clock frequency must match the desired frequency within
 * the following error margin expressed in PPM of the desired
 * frequency, with a minimum error tolerance of 1 kHz.
 */
#define CLOCK_BIST_ERROR_MARGIN_PPM         100     /* Expressed in PPM */
#define CLOCK_BIST_ERROR_MINIMUM_TOLERANCE  1000    /* Expressed in Hz */

/*
 * Macro to return absolute value of the difference between two values
 */
#define ABS_DIFF(v1, v2)  ((v1 >= v2) ? (v1 - v2) : (v2 - v1))

/*
 * This is the maximum number of delay attempts that will be made when
 * waiting for a clock change (config, enable, disable, etc) to propagate
 * to the status register.
 */
#define CLOCK_BIST_CONFIG_DELAY_ATTEMPTS    150    /* Expressed in iterations */

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_TestClockFunctions
** =========================================================================*/
/**
  Perform a non-destructive built-in self test of all clock functions on the
  specified clock.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param ClockId [in] -- ID of the particular clock being tested.
  @param bIsClockSupportedIsOn [in] -- Specifies whether the clock supports
                                       "IsOn" API functionality
  @param bIsClockSupportedCalcFreq [in] -- Specifies whether the clock supports
                                           "CalcFreq" API functionality

  @return
  TRUE -- BIST was successful.
  FALSE -- BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the BIST.
*/

static boolean Clock_TestClockFunctions
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   ClockId,
  boolean       bIsClockSupportedIsOn,
  boolean       bIsClockSupportedCalcFreq
)
{
  DALResult            eResult;
  boolean              bStatus;
  uint32               nFreqHz;
  uint32               nFreqError;
  uint32               nFreqErrorMargin;
  ClockNodeType       *pClock = (ClockNodeType *)ClockId;
  ClockMuxConfigType  *pBSPConfig;
  boolean              bBISTPassed = TRUE;
  uint32               nTimeout;

  /*-----------------------------------------------------------------------*/
  /* Turn on the clock.                                                    */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_EnableClock (pDrvCtxt, ClockId);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   EnableClock: ERROR");

    /* If this operation failed, there is no point in continuing the test */
    return FALSE;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   EnableClock: SUCCESS");
  }

  /*-----------------------------------------------------------------------*/
  /* Test whether the clock was enabled successfully.                      */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_IsClockEnabled (pDrvCtxt, ClockId, &bStatus);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockEnabled: ERROR");
  }
  else
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockEnabled: %d", bStatus);
  }

  /* If this operation failed, we need to clean up before returning */
  if (eResult != DAL_SUCCESS || bStatus == 0)
  {
    bBISTPassed = FALSE;
  }
  else
  {
    /* Verify the clock is "on" if this function is supported */
    if (bIsClockSupportedIsOn)
    {
      eResult = Clock_IsClockOn (pDrvCtxt, ClockId, &bStatus);

      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: ERROR");
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockOn: %d", bStatus);

        /*
         * Since the clock is enabled and IsOn API is supported,
         * if it's not on, we have an error.
         */
        if (!bStatus)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: status mismatch");
        }
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: NOT SUPPORTED");
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Test all defined frequencies for this clock if possible.              */
  /*-----------------------------------------------------------------------*/

  if (bBISTPassed == TRUE)
  {
    /* Make sure the clock handle is valid and supports frequency configuration */
    if (pClock == NULL || pClock->pDomain == NULL ||
        pClock->pDomain->pBSPConfig == NULL)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   SetClockFrequency: NOT SUPPORTED");

      /* Just calculate the current clock frequency and report it */
      if (bIsClockSupportedCalcFreq)
      {
        eResult = Clock_CalcClockFrequency (pDrvCtxt, ClockId, &nFreqHz);
        if (eResult != DAL_SUCCESS)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: ERROR");
        }
        else
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: %d Hz", nFreqHz);
        }
      }
      else
      {
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: NOT SUPPORTED");
      }
    }
    else
    {
      pBSPConfig = pClock->pDomain->pBSPConfig;

      /* Loop over all configurations */
      while (pBSPConfig->nFreqHz != 0)
      {
        /* Skip configurations that aren't for this chip or version. */
        if ( !Clock_IsBSPSupported(&pBSPConfig->HWVersion) )
        {
          pBSPConfig++;
          continue;
        }

        /* Select the new frequency to test */
        eResult = Clock_SetClockFrequency (pDrvCtxt,
                                           ClockId,
                                           pBSPConfig->nFreqHz,
                                           CLOCK_FREQUENCY_HZ_EXACT,
                                           &nFreqHz);

        /* Check results of the operation */
        if (eResult != DAL_SUCCESS)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   SetClockFrequency: ERROR");
        }
        else
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   SetClockFrequency:  %d Hz", nFreqHz);
        }

        /* The returned frequency must match the desired frequency exactly */
        if (pBSPConfig->nFreqHz != nFreqHz)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                            "   Frequency mismatch -- wanted: %d Hz vs got: %d Hz",
                            pBSPConfig->nFreqHz, nFreqHz);
        }

        /*
         * Allow sufficient time for the clock status to propagate after
         * applying a new configuration.  This is needed due to issues
         * seen on 9x15 where 0 MHz was measured on clocks after a slow
         * speed was configured but not fully propagated.
         */
        if (bIsClockSupportedIsOn)
        {
          nTimeout = 0;

          do
          {
            DALSYS_BusyWait(1);
            eResult = Clock_IsClockOn(pDrvCtxt, ClockId, &bStatus);

            /* Check results of the operation */
            if (eResult != DAL_SUCCESS)
            {
              ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: ERROR");
            }

          } while ( (eResult == DAL_SUCCESS) &&
                    (!bStatus              ) &&
                    (nTimeout++ < CLOCK_BIST_CONFIG_DELAY_ATTEMPTS) );

          /* Log a warning if the clock does not turn ON */
          if (eResult != DAL_SUCCESS || !bStatus)
          {
            bBISTPassed = FALSE;

            ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
                             "   Unable to turn ON clock: %s.",
                             pClock->szName);
          }
        }

        /* Measure the frequency of the clock */
        if (bIsClockSupportedCalcFreq)
        {
          eResult = Clock_CalcClockFrequency (pDrvCtxt, ClockId, &nFreqHz);

          /* Check results of the operation */
          if (eResult != DAL_SUCCESS)
          {
            bBISTPassed = FALSE;
            ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: ERROR");
          }
          else
          {
            ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: %d Hz", nFreqHz);
          }

          /* Compute the error rate between the desired and the measured values */
          nFreqError = ABS_DIFF(pBSPConfig->nFreqHz, nFreqHz);
          nFreqErrorMargin = pBSPConfig->nFreqHz / 1000000 * CLOCK_BIST_ERROR_MARGIN_PPM;

          /* Ignore any error within the permitted tolerance value */
          if (nFreqError > CLOCK_BIST_ERROR_MINIMUM_TOLERANCE)
          {
            /* The calculated frequency must match the desired frequency to some degree */
            if (nFreqError > nFreqErrorMargin)
            {
              bBISTPassed = FALSE;
              ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                                "   Frequency error %d Hz exceeds error margin of %d Hz",
                                nFreqError, nFreqErrorMargin);
            }
            else
            {
              ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                                "   Frequency error %d Hz is within error margin of %d Hz",
                                nFreqError, nFreqErrorMargin);
            }
          }
          else
          {
            ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                              "   Frequency error %d Hz is within error tolerance of %d Hz",
                              nFreqError, CLOCK_BIST_ERROR_MINIMUM_TOLERANCE);
          }
        }
        else
        {
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: NOT SUPPORTED");
        }

        /* Move to the next configuration */
        pBSPConfig++;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Turn off the clock after completing the test.                         */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DisableClock (pDrvCtxt, ClockId);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   DisableClock: ERROR");
    return FALSE;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   DisableClock: SUCCESS");
  }

  /*-----------------------------------------------------------------------*/
  /* Test whether the clock was disabled successfully.                     */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_IsClockEnabled (pDrvCtxt, ClockId, &bStatus);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockEnabled: ERROR");
  }
  else
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockEnabled: %d", bStatus);
  }

  /* Update the test result to be returned */
  if (eResult != DAL_SUCCESS || bStatus == 1)
  {
    bBISTPassed = FALSE;
  }
  else
  {
    /*
     * Allow sufficient time for clocks to turn off.  This is needed
     * due to issues seen on 9x15 where clocks with slow configurations
     * took longer for the off status to propagate.
     */
    if (bIsClockSupportedIsOn)
    {
      nTimeout = 0;

      do
      {
        DALSYS_BusyWait(1);
        eResult = Clock_IsClockOn (pDrvCtxt, ClockId, &bStatus);

        /* Check results of the operation */
        if (eResult != DAL_SUCCESS)
        {
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: ERROR");
        }

      } while ( (eResult == DAL_SUCCESS) &&
                (bStatus               ) &&
                (nTimeout++ < CLOCK_BIST_CONFIG_DELAY_ATTEMPTS) );

      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockOn: %d", bStatus);

      /* Log a warning if the clock does not turn OFF */
      if (eResult != DAL_SUCCESS || bStatus)
      {
        bBISTPassed = FALSE;

        ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
                         "   Unable to turn OFF clock: %s.",
                         pClock->szName);
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return final result.                                                  */
  /*-----------------------------------------------------------------------*/

  return bBISTPassed;

} /* END Clock_TestClockFunctions */



/* =========================================================================
**  Function : Clock_TestClock
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_TestClock
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   ClockId
)
{
  DALResult      eResult;
  boolean        bStatus;
  uint32         nFreqHz;
  boolean        bIsClockSupportedEnable;
  boolean        bIsClockSupportedIsOn;
  boolean        bIsClockSupportedCalcFreq;
  boolean        bBISTPassed = TRUE;
  ClockNodeType *pClock = (ClockNodeType *)ClockId;

  /*-----------------------------------------------------------------------*/
  /* Initialize the BIST log.                                              */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->hClockBISTLog == NULL)
  {
    ULogFront_RealTimeInit (&pDrvCtxt->hClockBISTLog, "Clock BIST Log",
                            pDrvCtxt->nBISTLogSize,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
    ULogCore_HeaderSet (pDrvCtxt->hClockBISTLog,
                        "Content-Type: text/tagged-log-1.0;");
  }

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog,
                      "   INVALID CONFIGURATION DETECTED -- test aborted");
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Identify the clock in the log by name and run the available tests.    */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog,
                    "Testing %s...", pClock->szName);

  /*
   * Determine whether the clock supports "Enable" API functionality
   */

  bIsClockSupportedEnable = HAL_clk_IsClockSupported (pClock->HALHandle,
                                                      HAL_CLK_CLOCK_API_ENABLE);

  /*
   * Determine whether the clock supports "IsOn" API functionality
   */

  bIsClockSupportedIsOn = HAL_clk_IsClockSupported (pClock->HALHandle,
                                                    HAL_CLK_CLOCK_API_ISON);

  /*
   * Determine whether the clock supports "CalcFreq" API functionality
   */

  bIsClockSupportedCalcFreq = HAL_clk_IsClockSupported (pClock->HALHandle,
                                                        HAL_CLK_CLOCK_API_CALCFREQ);

  /*
   * Determine if the clock is already enabled
   */

  eResult = Clock_IsClockEnabled (pDrvCtxt, ClockId, &bStatus);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockEnabled: ERROR");

    /* We must terminate the test here */
    return DAL_ERROR;
  }

  ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockEnabled: %d", bStatus);

  /*
   * If the clock is enabled, we'll assume it's in use and we can only verify
   * that it's on and check its frequency.
   */

  if (bStatus)
  {
    /* Verify the clock is "on" if this function is supported */
    if (bIsClockSupportedIsOn)
    {
      eResult = Clock_IsClockOn (pDrvCtxt, ClockId, &bStatus);

      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: ERROR");
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockOn: %d", bStatus);

        /*
         * Since the clock is enabled and IsOn API is supported,
         * if it's not on, we have an error.
         */
        if (!bStatus)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: status mismatch");
        }
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: NOT SUPPORTED");
    }

    if (bIsClockSupportedCalcFreq)
    {
      eResult = Clock_CalcClockFrequency (pDrvCtxt, ClockId, &nFreqHz);
      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: ERROR");
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: %d Hz", nFreqHz);
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: NOT SUPPORTED");
    }
  }
  else
  {
    /*
     * If the clock is disabled and it supports the "Enable" API, then it's not in use
     * and we can perform a complete test. Verify one more time that the clock is truly
     * "off" before proceeding to the complete test just in case the "Enable" API is
     * returning an erroneous result.
     */
    if (bIsClockSupportedEnable)
    {
      /* Verify the clock is truly "off" if this function is supported */
      if (bIsClockSupportedIsOn)
      {
        eResult = Clock_IsClockOn (pDrvCtxt, ClockId, &bStatus);

        if (eResult != DAL_SUCCESS)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   IsClockOn: ERROR");
        }
        else
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   IsClockOn: %d", bStatus);

          /*
           * If the clock is "on", we cannot continue with the complete test;
           * we must stop the test, but we cannot return failure because for
           * some clocks this is a legitimate state because these clocks may
           * be voted for by different masters. Therefore, we may think that
           * the clock is disabled because we voted for it to be disabled, but
           * in fact, the clock is "on" because one or more of the other masters
           * has voted for this clock to remain "on".
           */
          if (bStatus)
          {
            ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog,
                              "   IsClockOn: status mismatch -- test aborted");
            return DAL_SUCCESS;
          }
        }
      }

      /* Continue with the complete test if passed all the safety checks. */
      if (bBISTPassed)
      {
        bBISTPassed = Clock_TestClockFunctions (pDrvCtxt,
                                                ClockId,
                                                bIsClockSupportedIsOn,
                                                bIsClockSupportedCalcFreq);
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   EnableClock: NOT SUPPORTED");
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Log final result.                                                     */
  /*-----------------------------------------------------------------------*/

  if (bBISTPassed)
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock %s BIST Passed!",
                      ((ClockNodeType*)ClockId)->szName);
    return DAL_SUCCESS;
  }
  else
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock %s BIST Failed!",
                      ((ClockNodeType*)ClockId)->szName);
    return DAL_ERROR;
  }

} /* END Clock_TestClock */



/* =========================================================================
**  Function : Clock_BIST
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_BIST
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult     eResult;
  uint32        nClockIndex;
  ClockIdType   ClockId;
  boolean       bBISTPassed = TRUE;
  uint32        nFailedTests = 0;

  /*-----------------------------------------------------------------------*/
  /* Initialize the BIST log.                                              */
  /*-----------------------------------------------------------------------*/

  ULogFront_RealTimeInit (&pDrvCtxt->hClockBISTLog, "Clock BIST Log",
                          pDrvCtxt->nBISTLogSize,
                          ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  ULogCore_HeaderSet (pDrvCtxt->hClockBISTLog,
                      "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Test each clock node.                                                 */
  /*-----------------------------------------------------------------------*/

  for (nClockIndex = 0; nClockIndex < pDrvCtxt->nNumClocks; nClockIndex++)
  {
    /* Get the ClockId for this node */
    ClockId = (ClockIdType)&pDrvCtxt->aClocks[nClockIndex];

    eResult = Clock_TestClock (pDrvCtxt, ClockId);

    if (eResult != DAL_SUCCESS)
    {
      bBISTPassed = FALSE;
      nFailedTests++;
    }
  }

  /* Run the image-specific BIST. */
  Clock_ImageBIST(pDrvCtxt, &bBISTPassed, &nFailedTests);

  /*-----------------------------------------------------------------------*/
  /* Log final result.                                                     */
  /*-----------------------------------------------------------------------*/

  if (bBISTPassed)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "Clock BIST Passed!");
    return DAL_SUCCESS;
  }
  else
  {
    ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                      "Clock BIST Failed %d out of %d tests!",
                      nFailedTests, nClockIndex);
    return DAL_ERROR;
  }

} /* END Clock_BIST */
