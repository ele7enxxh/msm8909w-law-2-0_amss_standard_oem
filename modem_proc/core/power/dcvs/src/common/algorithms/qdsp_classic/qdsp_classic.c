/**********************************************************************
 * Qdsp_classic.c
 *
 * This module implements the qdsp_qdsp_classic algorithm for
 * Dynamic Clock and Voltage Scaling(DCVS).
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2015 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/algorithms/qdsp_classic/qdsp_classic.c#1 $
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    --------------------------------------------------- 
05/16/13   nj      Replaced the value to a macro for high busy MIPS requirement. 
04/11/13   sg      Updated algo to account for cycles wasted on L2 misses.
12/07/12   nj      Minor updates to ULog print messages
11/28/12   nj      Fixed a ULog printing bug
10/01/12   nj      Fixed ULog printing format for 64 bits
09/06/12   nj      Detecting PMU available and restarting sampling window 
09/26/12   nj      Fixed a bug in calculating OneThreadBusy percentage 
09/15/12   nj      Added code to deal with concurrent PMU usage
09/10/12   nj      INTLOCK/INTFREE sections when reading PMU 
09/10/12   nj      Corrected ULOG printing for 64 bit 
08/30/12   nj      Added New Scheme based Profiling on top of existing scheme 
                   Added ClkChgCB
08/29/12   nj      Fixed bug in 'old code' where timer is restarted from ClkChgCB 
                   w/o the updated timer interval value (always SAMPLE PERIOD was used) 
08/23/12   nj      Added PMU Profiling code 
03/29/12   nj      Added header macros that include target specific include files 
03/23/12   nj      Changed include files to be compatible with 8974
09/24/11   sg      Declare extern requestStateVector.
09/19/11   ss      Removing the notifications into the algorithm when the state of the 
                   clock changes. 
09/15/11   ss      Restarting the timers so that DCVS fires every sample period interval
                   taking into account the clock changes.
09/12/11   ss      Correcting a bug in identifying dedicated and non dedicated threads.
                   Adding additional logging for vector requests.
04/26/11   ss      Fixing Q6 compiler warnings.
04/14/11   ss      Fixing the system profiling functionality.
02/28/11   bjs     Initial check in.
========================================================================*/
#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_ALGO_QDSP_CLASSIC

#include "dcvs_core.h"
#include "dcvs_os.h"
#include "dcvs_algorithm.h"
#include "classic.h"
#include "dcvs_npa.h"
#include "dcvs_nv.h"
#include "ULogFront.h"
#include "npa.h"
#include "CoreVerify.h"
#include "npa_resource.h"
#include "max_stub.h"
#include "blast.h"
#include "msg.h"
#include "CoreTime.h"
#include "CLM.h"

#include DCVS_OS_HEADER1_H
#include DCVS_OS_HEADER2_H

typedef enum
{
  DCVS_PARAMS_ALG_QDSP_CLASSIC_SAMPLE_PERIOD = 0,
  DCVS_PARAMS_ALG_QDSP_CLASSIC_HIGH_THRESHOLD_BUSY_PCT,
} DCVS_PARAMS_ALG_QDSP_CLASSIC;

static const DCVS_NV_PARAM_ENTRY dcvsParamsAlgDefault[] =
{
  { "SamplePeriod", DCVS_NV_PARAM_UINT32, { (void *)50 } },
  { "HighThresholdBusyPct", DCVS_NV_PARAM_UINT32, { (void *)90 } },
  DCVS_NV_PARAM_LIST_END
};

#define DCVS_ALGO_QDSP_CLASSIC_HIGH_WATERMARK_SAFETY (10)

/* Used for identifying a truly busy use-case to skip the L2 cache cycles reduction */
#define CPU_MAXED_OUT_PCT 97

/* Macro for multiplication by 10 .
   Multiply the number by 8 and 2 and add the sum . */
#define MUL_TEN(x) \
  ((x<<3)+(x<<1))

/* Macro for PMU Event config register value */
// 03=MPPS; a0=ANY_DU_STALLS; 9f==DU_STALL_CYCLES 77=L2_IU_MISS;
#define PMU_EVT_CONFIG_VAL 0x03a09f77

/* Macro for PMU config register value */
#define PMU_CONFIG_VAL 7


/* Structure to maintain the low and high thresholds for each clock level in OAL. */
typedef struct
{
  /* The low threshold for a clock level in terms of busy percentage */
  uint32 lowThresholdBusyPct;

} DCVS_ALGO_QDSP_CLASSIC_CLK_THRESHOLD;

/* Static variable to store the context handle */
static DCVS_CONTEXT *localCoreContext = NULL;

/* Default DCVS Algorithm specific members */
typedef struct
{
  /* Members specific to the default DCVS algorithm */
  ULogHandle logHandle;

  /* mips calculated by the OAL portion of the algorithm. */
  uint32 mips;

  /* Structure which stores the clock plan for the particular target */
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE *clockPlan;

  /* Handle to CLM client*/
  CLM_HandleType clmHandle;

  /* The high threshold busy percentage from NV */
  uint32 highThresholdBusyPct;

  /* Structure to maintain the low and high thresholds for each clock level in OAL. */
  DCVS_ALGO_QDSP_CLASSIC_CLK_THRESHOLD *thresholdTable;

  /* The current index for the clock plan as calculated by this DCVS algorithm/NPA Requests */
  uint32 clockPlanCurrIndex;

  /* Index for the clock plan determined by DCVS algorithm output ONLY*/
  uint32 clockPlanDCVSTimerIndex;

  /* Stores the total time elapsed in milli-second units*/
  uint32 totalTimeElapsed;

  /* CPU utilization percentage based on the clock rate when the window ended 
    Range: 0..100 */
  uint32 utilPctAtCurrClk;

  /* # of pcycles that were  used to execute instructions since last update. */
  uint64 utilizedPcycles;

  /* The sample period */
  uint32   samplePeriodMs;

} DCVS_ALGO_QDSP_CLASSIC;


/**
 * Determines the clock level to use based on the current utilization stats
 * @param busyPcycles 
 * @param context 
 * @return uint32 : clk level recommended
 */
static inline uint32 DCVSFindClockLevelForMeasuredUtil( uint64 busyPcycles,
                                                        DCVS_CONTEXT *context )
{
  uint32 nearestLevel;
  uint32 lowThresholdBusyPct = 0;

  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)context->algoDataContext;

  uint64 pcyclesScaledToFullSecond = 1000 * busyPcycles / (uint64)algoDataContext->totalTimeElapsed;

  ULogFront_RealTimePrintf( algoDataContext->logHandle, 4,
                            "QDSPCLASSIC (RawBusyPcycles: %u) (ScaledPcycles: %llu) (elapsedMs: %d)",
     (uint32)busyPcycles,
                            ULOG64_LOWWORD( pcyclesScaledToFullSecond ),
                            ULOG64_HIGHWORD( pcyclesScaledToFullSecond ),
                            algoDataContext->totalTimeElapsed );

  // find the lowest clock level that supports the utilized level
  // nearestLevel will be at MAX if we fall thru the loop
  for ( nearestLevel = 0;
        nearestLevel < algoDataContext->clockPlan->numLevels - 1; nearestLevel++ )
  {
    if ( (algoDataContext->clockPlan->clockPlanTable[nearestLevel].cpuFreq * 1000) >=
            pcyclesScaledToFullSecond )
    {
      break;
    }
  }

  if ( nearestLevel < algoDataContext->clockPlan->numLevels - 1 )
  {
    // if chosen level is greater than the current index, check high threshold
    if ( nearestLevel >= algoDataContext->clockPlanCurrIndex )
    {
      uint32 nearestClkFreq = algoDataContext->clockPlan->clockPlanTable[nearestLevel].cpuFreq;
        // cpuFreqKHz * 1000 * highThresholdBusyPct / 100
      uint32 nearestClk90pctCycles = nearestClkFreq * algoDataContext->highThresholdBusyPct * 10;

      // Jump to the next level if we've exceeded the high threshold.
      if ( pcyclesScaledToFullSecond >= nearestClk90pctCycles )
      {
        if ( nearestClkFreq == algoDataContext->clockPlan->clockPlanTable[nearestLevel+1].cpuFreq )
        {
          uint32 nearestClk97pctCycles = nearestClkFreq * 97 * 10;
          if ( pcyclesScaledToFullSecond >= nearestClk97pctCycles )
          {
            nearestLevel++;
          }
        }
        else
      {
        nearestLevel++;
      }
      }
      // else nearestLevel is good
    }
    else
    {
      uint32 currLevelClkFreq = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].cpuFreq;
      lowThresholdBusyPct = algoDataContext->thresholdTable[algoDataContext->clockPlanCurrIndex].lowThresholdBusyPct;
        // cpuFreqKHz * 1000 * lowThresholdBusyPct / 100
      uint32 lowThresCyclesForCurrLevel = currLevelClkFreq * lowThresholdBusyPct * 10;
                                          
      if ( algoDataContext->clockPlan->clockPlanTable[nearestLevel].cpuFreq ==
           algoDataContext->clockPlan->clockPlanTable[nearestLevel+1].cpuFreq )
      {
        uint32 lowThreshold2BusyPct = (lowThresholdBusyPct + 100 - DCVS_ALGO_QDSP_CLASSIC_HIGH_WATERMARK_SAFETY)/2;
        uint32 lowThres2CyclesForCurrLevel = currLevelClkFreq * lowThreshold2BusyPct * 10;
        
        /* If the next perf level has the same cpuFreq as the chosen (nearest) level and busyCycleCount is
           greater than lowThres2, use the next perf level (the one with higher b/w vote) */
        if ( pcyclesScaledToFullSecond > lowThres2CyclesForCurrLevel )
        {
          ++nearestLevel;
        }
        // else go with nearestLevel
        
        /* for logging purposes alone */
        lowThresholdBusyPct = lowThreshold2BusyPct;
      }
      else if ( pcyclesScaledToFullSecond > lowThresCyclesForCurrLevel )
      {
        nearestLevel = algoDataContext->clockPlanCurrIndex;
      }
      // else go with nearestLevel
    }
  }
  // else we'll use MAX clk
  
  ULogFront_RealTimePrintf( algoDataContext->logHandle, 2,
                            "QDSPCLASSIC (chosenLevel: %d) (LowThresholdPct: %d)",
                            nearestLevel, lowThresholdBusyPct );
  
  return nearestLevel;
}

/**
  @brief DCVSRunQdspClassic

  This function is used to run the actual qdsp_classic 
  algorithm. 
   
  This function is run every time the DCVSTimer expires 
  (ie:every SamplePeriod). We calculate the Budget PCycles that
  were available in the last sample period. then get the
  profiling data by reading the PMU Registers that tell us how
  for how many cycles did atleast one HW Thread run for. Based 
  on this, we calculate the busy percentage of the time where
  atleast one HW thread was running. Then compare this % with 
  high and low thresholds – if greater than high threshold, step 
  up the clock index (Performance Level) by 1, if lower than low 
  threshold, calculate the new clock index (Performance Level) 
  that atleast meets the clock frequency based on the busy 
  percentage. Restart the DCVSTimer with the SamplePeriod time 
  Return the mips based on the above caclulated clock plan index 

  @param context: The DCVS data context.

  @return : The mips calculated by the algorithm.
*/
static uint32 DCVSRunQdspClassic( DCVS_CONTEXT *context )
{
  // This should run every time the timer is fired.  It is run from an
  // asynchronous client, so already has the lock.
  // Never allow this to drop below activeFloor

  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = context->algoDataContext;

  /* Clock plan index before the algorithm is executed, to compare against */
  uint32 clockPlanOldIndex = algoDataContext->clockPlanCurrIndex;

  DCVSOSCriticalSectionEnter( context );

  uint64 busyPcycles = algoDataContext->utilizedPcycles;

  algoDataContext->clockPlanCurrIndex = DCVSFindClockLevelForMeasuredUtil( busyPcycles, context );
  algoDataContext->clockPlanDCVSTimerIndex = algoDataContext->clockPlanCurrIndex;
  if ( algoDataContext->clockPlanDCVSTimerIndex >= algoDataContext->clockPlan->numLevels )
  {
    algoDataContext->clockPlanDCVSTimerIndex = algoDataContext->clockPlan->numLevels - 1;
  }

  /* Check to see if the clock rate has changed or the active floor has changed . */
  if ( (context->resDataContext->activeFloor > 0) || (clockPlanOldIndex != algoDataContext->clockPlanCurrIndex) )
  {
    uint32 i;
    /* We make sure that the algorithm mips is greater than the active floor. Start from the current index
       because we are sure to be above the active floor till now. */
    for ( i = algoDataContext->clockPlanCurrIndex; i < algoDataContext->clockPlan->numLevels; i++ )
    {
      if ( algoDataContext->clockPlan->clockPlanTable[i].reqMips >= context->resDataContext->activeFloor )
      {
        break;
      }
    }
    if ( i >= algoDataContext->clockPlan->numLevels )
    {
      i = algoDataContext->clockPlan->numLevels - 1;
    }
    algoDataContext->clockPlanCurrIndex = i;
  }

  DCVSOSCriticalSectionExit( context );

  // Return current mips.
  return ( algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips );
}

/**
  @brief DCVSImpulseQdspClassic

  This function is used to return the max mips of the target in response to the impulse client 
  request.

  @param  : DCVS data context.

  @return : The max mips supported by the target.
*/
static uint32 DCVSImpulseQdspClassic( DCVS_CONTEXT *context )
{
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)context->algoDataContext;

  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;

  // Return current mips.
  return ( algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips );
}

/**
  @brief DCVSFloorQdspClassic

  This function is used to reset the members of the qdsp_classic algorithm.

  @param  : DCVS data context.

  @return : The max mips supported by the target.
*/
static uint32 DCVSFloorQdspClassic( DCVS_CONTEXT *context, uint32 floor )
{
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = context->algoDataContext;

  uint32 result = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanDCVSTimerIndex].reqMips;
  // This will be run from a required client request, so it already
  // has the lock.

  if ( algoDataContext->mips < floor )
  {
    result = floor;
  }
  else
  {
    if ( result < floor )
    {
      result = floor;
    }
    else
    {
      // update algoDataContext->clockPlanCurrIndex to clockPlanDCVSTimerIndex because it's DCVS algo output
      // deciding the mips as floor is less
      algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlanDCVSTimerIndex;
    }
  }

  return ( result );
}

/**
  @brief DCVSAlgoQdspClassicReset

  This function is used to reset the members of the qdsp_classic algorithm.

  @param  : DCVS data context.

  @return : The max mips supported by the target.
*/
static uint32 DCVSAlgoQdspClassicReset( void *context )
{
  DCVS_CONTEXT *coreContext = (DCVS_CONTEXT *)context;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)coreContext->algoDataContext;

  /* Return max mips. */
  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;
  algoDataContext->clockPlanDCVSTimerIndex = algoDataContext->clockPlan->numLevels - 1;
  return ( algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips );
}

/**
  @brief DCVSAlgoQdspClassicThresholdTableSetup

  This function is used to calculate the low and high threshold
  values in terms of busy percentage for each clock level.

  @param  context: The DCVS data context.

  @return : None
*/
static void DCVSAlgoQdspClassicThresholdTableSetup( DCVS_CONTEXT *context )
{
  uint32 currClkMips, prevClkMips, highWaterMarkIdle, lowWaterMarkBusy;
  int32 i;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = context->algoDataContext;

  /* Allocate the memory for threshold table information for the resources */
  DALSYS_Malloc( sizeof(DCVS_ALGO_QDSP_CLASSIC_CLK_THRESHOLD) * (algoDataContext->clockPlan->numLevels),
                 (void **)&algoDataContext->thresholdTable );
  CORE_VERIFY_PTR( algoDataContext->thresholdTable );

  /* Precalculate the low/high thresholds for the busy percentage for different clock levels
     for the algorithm */
     
  /* High threshold busy pct is fixed according to the NV input. */
  
  for ( i = algoDataContext->clockPlan->numLevels - 1; i > 0; i-- )
    {
    /* Calculate the LOW threshold for the clock level */
      currClkMips = algoDataContext->clockPlan->clockPlanTable[i].reqMips;
      prevClkMips = algoDataContext->clockPlan->clockPlanTable[i - 1].reqMips;
    
    /* If 2 adjacent perf levels have the same MIPS value - and by implication,
       the same Q6 frequency - compute lowThreshold based on the next lower Mips */
    if ( prevClkMips == currClkMips && i-2 >= 0)
    {
      prevClkMips = algoDataContext->clockPlan->clockPlanTable[i - 2].reqMips;
    }
    
      /* Calculate the high watermark percent for idle for this clock level */
      highWaterMarkIdle = (((currClkMips - prevClkMips) * 100) / currClkMips) + DCVS_ALGO_QDSP_CLASSIC_HIGH_WATERMARK_SAFETY;
      /* The low watermark percentage for busy for this clock level */
      lowWaterMarkBusy = 100 - highWaterMarkIdle;
      algoDataContext->thresholdTable[i].lowThresholdBusyPct = lowWaterMarkBusy;
    }
  
  algoDataContext->thresholdTable[0].lowThresholdBusyPct = 0;  
}


/**
  @brief DCVSAlgoQdspClassicInitialize

  This function is used to initialize the qdsp_classic DCVS algorithm specific members .

  @param  context: DCVS data context.

  @return : Initial mips of the system
*/
static uint32 DCVSAlgoQdspClassicInitialize( void *context )
{
  DCVS_CONTEXT *coreContext = (DCVS_CONTEXT *)context;
  CORE_VERIFY_PTR( coreContext );
  
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext;

  /* Allocate the memory for the algorithm data context. */
  DALSYS_Malloc( sizeof(DCVS_ALGO_QDSP_CLASSIC),
                 (void **)&coreContext->algoDataContext );
  CORE_VERIFY_PTR( coreContext->algoDataContext );

  algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)coreContext->algoDataContext;

  algoDataContext->logHandle = DCVSCoreLogHandleGet( coreContext );
  CORE_VERIFY_PTR( algoDataContext->logHandle );

  /* Initalize local variable to point to core context handle */
  localCoreContext = coreContext;

  /* Get the current clock plan of the target */
  algoDataContext->clockPlan = DCVSCoreClockPlanGet( coreContext );

  // Start at the max
  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlan->numLevels - 1;
  algoDataContext->clockPlanDCVSTimerIndex = algoDataContext->clockPlan->numLevels - 1;

  /* Set up the threshold table for each clock level. */
  DCVSAlgoQdspClassicThresholdTableSetup( coreContext );

  /* Set the inital mips to max */
  algoDataContext->mips = DCVSAlgoQdspClassicReset( coreContext );

  return ( algoDataContext->mips );
}

/**
  @brief DCVSAlgoQdspClassicBuildParams

  This function is used to get the parameters for the qdsp_classic DCVS algorithm.

  @param  : DCVS data context.

  @return : None
*/
static void DCVSAlgoQdspClassicBuildParams( void *context )
{
  DCVS_CONTEXT *coreContext = (DCVS_CONTEXT *)context;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = coreContext->algoDataContext;

  /* Read the timer period */
  DcvsNvParamUint32Read( 0,
                        "Default",
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_QDSP_CLASSIC_SAMPLE_PERIOD],
                         &algoDataContext->samplePeriodMs );
  DcvsNvParamUint32Read( 0,
                        "Default",
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_QDSP_CLASSIC_HIGH_THRESHOLD_BUSY_PCT],
                         &algoDataContext->highThresholdBusyPct );

}

static void DCVSAlgoQdspClassicCLMClient_callback( CLM_LoadInfoExtendedStructType *clmInfo,
                                                   void *clientData )
{
  DCVS_CONTEXT *context = (DCVS_CONTEXT *)clientData;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = context->algoDataContext;

  algoDataContext->utilizedPcycles = clmInfo->busyCycles;
  algoDataContext->totalTimeElapsed = (clmInfo->timeElapsedUs) / 1000;

  if ( clmInfo->updateReason == CLM_UPDATE_REASON_PERIODIC )
  {
    ULogFront_RealTimePrintf( context->coreDataContext->logHandle, 4, "CLM_UPDATE "
                              "(UtilAtCurr: %d) "
                              "(UtilAtMax:  %d) "
                              "(UpdatedAfter: %d) "
                              "(ClkKhz:  %d) "
                              , clmInfo->utilPctAtCurrClk
                              , clmInfo->utilPctAtMaxClk
                              , clmInfo->timeElapsedUs
                              , clmInfo->currentClkKhz );
    
    /* The DCVS sampling timer has fired calculate the new mips of the QDSP classic algorithm */
    DCVSIssueInternalRequest( context );
  }
}

/**
  @brief DCVSAlgoQdspClassicEnable

  This function is used to set the state of the qdsp_classic algorithm specific members
  when enabling the algorithm.

  @param context: DCVS data context.

  @return : The mips of the system after enabling the algorithm.
*/
static uint32 DCVSAlgoQdspClassicEnable( void *context )
{
  DCVS_CONTEXT *coreContext = (DCVS_CONTEXT *)context;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = coreContext->algoDataContext;

  CORE_VERIFY_PTR( 
    algoDataContext->clmHandle = CLM_RegisterPeriodicClient_Ex( "DCVS", CLM_CLIENT_EXTENDED_CPUUTIL,
                                           (algoDataContext->samplePeriodMs) * 1000,
                                                                CLM_ATTRIBUTE_DEFAULT,
                                                                (CLM_CallbackFuncPtr)DCVSAlgoQdspClassicCLMClient_callback, coreContext ) );

  algoDataContext->mips = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels - 1].reqMips;
  return ( algoDataContext->mips );
}

/**
  @brief DCVSAlgoQdspClassicDisable

  This function is used to reset the state of the qdsp_qdsp_classic algorithm specific members
  when disabling the algorithm.

  @param context: DCVS data context.

  @return : The mips of the system after disabling the algorithm.
*/
static uint32 DCVSAlgoQdspClassicDisable( void *context )
{
  DCVS_CONTEXT *coreContext = (DCVS_CONTEXT *)context;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = coreContext->algoDataContext;

  /* Return max mips */
  return ( algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels - 1].reqMips );
}

/**
  @brief DCVSAlgoQdspClassicDecideMips

  This function is used to incorporate external requests by the clients into the
  algorithm. The supported requests are impulse, floor etc.

  @param  context: DCVS data context.

  @return : Mips required
*/
static uint32 DCVSAlgoQdspClassicDecideMips( void *context )
{
  DCVS_CONTEXT *coreContext = (DCVS_CONTEXT *)context;
  DCVS_RES_DATA *resDataContext = coreContext->resDataContext;
  DCVS_ALGO_QDSP_CLASSIC *algoDataContext = (DCVS_ALGO_QDSP_CLASSIC *)coreContext->algoDataContext;

  if ( resDataContext->algoMipsChangedOrTimerFired )
  {
    // Clear the timer fired flag here.
    resDataContext->algoMipsChangedOrTimerFired = FALSE;
    algoDataContext->mips = DCVSRunQdspClassic( context );
  }
  /* If there is an impulse request, send it to the OAL component. */
  else if ( resDataContext->activeImpulse )
  {
    // Clear the impulse request flag here.
    resDataContext->activeImpulse = FALSE;
    algoDataContext->mips = DCVSImpulseQdspClassic( context );
  }
  // If it is not a timer, or an impulse, then the floor is being adjusted.
  else
  {
    algoDataContext->mips = DCVSFloorQdspClassic( context, resDataContext->activeFloor );
  }

  /* Logging for qdsp_classic algorithm */
  ULogFront_RealTimePrintf( algoDataContext->logHandle, 3,
                            "QDSPCLASSIC (UtilLevel: %d) (Floor: %d) (PerfLevel: %d)",
                           algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips,
                            resDataContext->activeFloor,
                            algoDataContext->mips );

  return algoDataContext->mips;
}

DCVS_CORE_ALGO qdspClassicAlgorithm =
{
  "qdsp_classic",
  dcvsParamsAlgDefault,
  DCVSAlgoQdspClassicInitialize,
  DCVSAlgoQdspClassicBuildParams,
  DCVSAlgoQdspClassicDecideMips,
  DCVSAlgoQdspClassicReset,
  DCVSAlgoQdspClassicEnable,
  DCVSAlgoQdspClassicDisable
};

#endif //DEFEATURE_DCVS_ALGO_QDSP_CLASSIC
