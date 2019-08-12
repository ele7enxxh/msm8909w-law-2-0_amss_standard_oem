/**********************************************************************
 * classic.c
 *
 * This module implements the classic algorithm for 
 * Dynamic Clock and Voltage Scaling(DCVS).
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/algorithms/classic/classic.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ------------------------------------------------------------------- 
10/26/12   nj     Removed un-necessary header file, added 64 calculation bit support for B-Family 
                  Minor update to ULog printing
09/25/11   sg     Minor ulog formatting changes; fixed compiler warnings.
02/28/11   ss     Using memset to initialiize the allocated memory.
02/23/11   ss     Resetting the idle history in the classic algorithm when the mips calculated by the algorithm changes .
02/18/11   ss     Adding context based processing for DCVS.
01/21/11   ss     Removing the OAL component of the algorithm.
10/14/10   ss     Logging impulse and isoc request information from DCVS algorithm .
10/08/10   ss     Using the new DCVS clock plan data structure in the classic algorithm.
08/13/10   ss     Moving the mips and idle calculations of classic algorithm 
                  from kernel to OAL .
08/10/10   ss     Renaming DCVS algorithm file "default.c" to "classic.c".
06/17/10   ss     Changing the logging format to be compatible with
                  visualization tools.
05/04/10   ss     Using the idle override information while calculating
                  mips from the algorithm.
03/30/10   ss     Modifying "Classic" algorithm to account for clock changes.
02/21/10   ss     Adding algorithm specific logging. 
01/20/10   bjs    Cleanup
09/28/09   ss     Initial check in.
==========================================================================================*/

#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_ALGO_CLASSIC

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

#define DCVS_ALGO_CLASSIC_HIGH_WATERMARK_SAFETY (10)
#define DCVS_ALGO_CLASSIC_SET_BITS 2
#define DCVS_ALGO_CLASSIC_SET_BMASK 0x3

#define DCVS_ALGO_CLASSIC_SET_RAISE 1
#define DCVS_ALGO_CLASSIC_SET_LOWER 2
#define DCVS_ALGO_CLASSIC_SET_STAY 0

/* Macro for multiplication by 10 .
   Multiply the number by 8 and 2 and add the sum . */
#define MUL_TEN(x) \
  ((x<<3)+(x<<1))

typedef enum
{
  DCVS_PARAMS_ALG_CLASSIC_SAMPLE_PERIOD = 0,
  DCVS_PARAMS_ALG_CLASSIC_SAMPLES_RAISE,
  DCVS_PARAMS_ALG_CLASSIC_SAMPLES_LOWER,
} DCVS_PARAMS_ALG_CLASSIC;

static const DCVS_NV_PARAM_ENTRY dcvsParamsAlgDefault[] =
{
  {"SamplePeriod", DCVS_NV_PARAM_UINT32, (void *)50},
  {"SamplesToRaise", DCVS_NV_PARAM_UINT32, (void *)1},
  {"SamplesToLower", DCVS_NV_PARAM_UINT32, (void *)1},
  DCVS_NV_PARAM_LIST_END
};

/* Structure to maintain the low and high thresholds for each clock level */
typedef struct
{
  /* Low threshold mips with precision */
  uint32 lowMipsWithPrec;
  /* High threshold mips with precision */
  uint32 highMipsWithPrec;
}DCVS_ALGO_CLASSIC_CLK_THRESHOLD;

/* Default DCVS Algorithm specific members */
typedef struct
{
  /* Members specific to the classic DCVS algorithm */
  ULogHandle logHandle;

  /* mips calculated by the the algorithm. */
  uint32 mips;

  /* Structure which stores the clock pla4n for the particular target */
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* clockPlan;

  /* Query handle to NPA resource */
  npa_query_handle queryHandle;

  /* The resource handle of the NPA resource used by this algorithm.  */
  npa_resource *resource;

  /* Previous known state of the resource */
  uint32 prevState;

  /* Current state of the resource in MIPS */
  uint32 currState;

  uint32 timerBusy;
  uint32 sleepRegistered;

  uint32 sleepStart, sleepStop;
  uint32 systemStart, systemStop;

  /* Timer handle for the classic algorithm. */
  DCVSOSTimerHandle timerHandle;

  /* Flag to signal the starting of the timer for the algorithm. */
  DALBOOL timerStarted;

  /* the number of votes for lower/raise */
  uint32 lowerN;
  uint32 raiseN;

  /* lower/raise decision mask in idle_history */
  uint32 lowerMask;
  uint32 raiseMask;

  /* lower/raise decision bits in idle_history */
  uint32 lower;
  uint32 raise;

  /* the decision history */
  uint32 idleHistory;

  /* The current lookup index for the clock plan table as calculated by this DCVS algorithm */
  uint32 clockPlanCurrIndex;

  /* The maximum index for the clock plan table */
  uint32 clockPlanMaxIndex;

  /* The sampling timer period  for this algorithm */
  uint32 timerPeriod;

  /* The aggregated time elapsed and number of instructions executed till the timer expiry
     of algorithm taking into account the clock changes in ms */
  uint32 timeElapsed;

  /* Number of instructions for the interval. The unit is secs*MIPS  */
  uint32 insCount;

  /* Structure to maintain the low and high thresholds for each clock level. */
  DCVS_ALGO_CLASSIC_CLK_THRESHOLD* thresholdTable;

}DCVS_ALGO_CLASSIC;


/**
  @brief DCVSAlgoClassicCurrClockStateQuery
  
  This function is used to read the current state of the clock.
 
  @param context: The DCVS data context.
  @return : The current mips of the clock or invalid state . 

*/
uint32 DCVSAlgoClassicCurrClockStateQuery(DCVS_CONTEXT* context)
{
  npa_query_status status;
  npa_query_type queryResult;
  
  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;

  /* Query the current state of clock resource. */
  status = npa_query(algoDataContext->queryHandle, NPA_QUERY_CURRENT_STATE, &queryResult);
  if ( status == NPA_QUERY_SUCCESS )
  {
    /* Convert the frequency value into mips */
    return ( algoDataContext->clockPlan->KhzToMipsConvert(queryResult.data.state) ) ;
  }
  else
  {
    return DCVS_ALGO_CLASSIC_INVALID_CLK_STATE;
  }
}

/**
  @brief MaxMips
  
   The function returns the maximum of the 2 input mips 
   provided as inputs.

  @param x: The mips value.
  @param y: The mips value.

  @return : Max of the 2 mips values supplied.

*/
static uint32 MaxMips(uint32 x, uint32 y)
{
  uint32 mx;
  mx = (x>y?x:y);
  return mx;
}


/**
  @brief DCVSAlgoClassicReset
  
   This function is used to reset the members of the classic algorithm .

  @param  : DCVS data context.

  @return : The max mips supported by the target. 

*/
static uint32 DCVSAlgoClassicReset(void* param)
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;
  
  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;
  /* Return max mips. */
  return(algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels-1].reqMips);
}



/**
  @brief DCVSAlgoClassicTimerStart
  
  This function is used to start the DCVS sampling timer.

  @param time_ms : The time in ms for the timer.

  @param context: DCVS data context. 

  @return : None

*/
static void DCVSAlgoClassicTimerStart(uint32 time_ms, DCVS_CONTEXT* context)
{
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;
  algoDataContext->sleepStart = DCVSOSSleepTickGet(context);
  algoDataContext->systemStart = DCVSOSSystemTickGet(context);
  algoDataContext->timerBusy = 1;
  /* Start the DCVS sampling timer. */
  DCVSOSTimerStart(algoDataContext->timerHandle, algoDataContext->timerPeriod, context);
  algoDataContext->timerStarted = TRUE;
}

/**
  @brief DCVSAlgoClassicTimerStop
  
  This function is used to stop the DCVS sampling timer.

  @param context: The DCVS data context.

  @return : None

*/
static void DCVSAlgoClassicTimerStop(DCVS_CONTEXT* context)
{
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;
  algoDataContext->timerBusy = 0;
  DCVSOSTimerStop(algoDataContext->timerHandle, context);
  algoDataContext->timerStarted = FALSE;
}



/**
  @brief IntervalSystemSleepTimeGet
  
  This function is used to obtain the system time and sleep time 
  for the particular interval.

  @param delTime : The system time elapsed during the past timing interval.
  This value is filled by this function. 
  @param delSleep : The idle time during the past timing interval.
  This value is filled by this function. 
  @param context : The The DCVS data context. 

  @return : None

*/

static void IntervalSystemSleepTimeGet(uint32* delTime, uint32* delSleep, DCVS_CONTEXT* context)
{
  /* Variables to obtain the idle statistics */
  uint32 idleOverride, sleepOverride;

  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;

  // Read the stop values right now.
  algoDataContext->sleepStop = DCVSOSSleepTickGet(context);
  algoDataContext->systemStop = DCVSOSSystemTickGet(context);

  /* Obtain the system ticks/ sleep ticks for the previous interval */
  DCVSOSIdleInfo(&idleOverride);

  // System Tick Delta = (System Tick Stop - System Tick Start) in secs . 
  *delTime = (algoDataContext->systemStop - algoDataContext->systemStart); 

  // Sleep Tick Delta = (Sleep Tick Stop - Sleep Tick Start) in secs .
  *delSleep = (algoDataContext->sleepStop - algoDataContext->sleepStart);

  /* Incorporate idle override */
  sleepOverride = (*delTime * idleOverride)/100;
  
  if (sleepOverride < *delSleep)
  {
    *delSleep = sleepOverride;
  }
}



/**
  @brief DCVSAlgoClassicMipsCalculate
  
   Implementation of the DCVS Classic algorithm.
   This function decides the performance level and acts upon it. It
   first updates the decision as whether to stay, raise or lower (the
   performance level) for this timer interval in the decision
   data. The decision data is implemented in a sliding window fashion
   and it tracks the decisions for the past timer intervals.

   If the decisions for the past lowerN profiling cycle are all to
   "LOWER", the performance level will then be step down one level
   lower. If the decisions for the past raiseN profiling cycles are
   all to "RAISE", the performance level will be raise to the  maximum
   immediately.

  @param context: The DCVS data context.

  @return : void.

*/
static void DCVSAlgoClassicMipsCalculate(DCVS_CONTEXT* context)
{
  uint32 insCount = 0, newAlgoMips = 0;
  /* Variable to hold the average mips value with precision. */
  static uint32 avgMipsWithPrec;

  uint32 deltaSleep = 0, deltaTime = 0;

  /* Variable to hold the mips value for the interval. */
  uint32 intervalMips = 0;

  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;

  if(algoDataContext->currState != DCVS_ALGO_CLASSIC_INVALID_CLK_STATE)
  {
    /* Obtain  the system time and sleep time for the particular interval. */
    IntervalSystemSleepTimeGet(&deltaTime, &deltaSleep, context);

    /* Calculate the number of instructions for this time period. Use the
       current state of the resource since the timer expired. */
    if ( deltaTime > deltaSleep )
    {
      intervalMips = algoDataContext->prevState;

      /* Calculate the insCount with deltaTime and deltaSleep in ms 
         The unit of insCount wouldbe secs*MIPS */
      insCount = ((deltaTime - deltaSleep)*intervalMips ) ;
    } 

    /* Update the state variable which indicates the state of the
       physical clock before any clock change happened */
    algoDataContext->prevState = algoDataContext->currState;
  }

  /* if (deltaTime < deltaSleep), neglect the previous interval. */

  /* Aggregate the instructions for this time interval */
  algoDataContext->insCount += insCount;

  /* Aggregate the time */
  algoDataContext->timeElapsed += deltaTime;

  /* Calculate the MIPS of the system considering the clock changes
	    for the current timer period . */	
  if (algoDataContext->timeElapsed > 0)
  {
    avgMipsWithPrec = MUL_TEN((uint64)algoDataContext->insCount)/algoDataContext->timeElapsed;
  }


  /* Reset the time elapsed and instruction count */
  algoDataContext->timeElapsed = 0;
  algoDataContext->insCount = 0;


  /* idleHistory tracks the past decisions. Every 2 bits representing
     a decision. It kept up to 16 decisions. */
  algoDataContext->idleHistory <<= DCVS_ALGO_CLASSIC_SET_BITS;

  /* Compare the average MIPS for the previous interval with the 
     low and high threaholds */
  if (avgMipsWithPrec <= algoDataContext->thresholdTable[algoDataContext->clockPlanCurrIndex].lowMipsWithPrec)
  {
    algoDataContext->idleHistory |= DCVS_ALGO_CLASSIC_SET_LOWER;
  }
  else if (avgMipsWithPrec > algoDataContext->thresholdTable[algoDataContext->clockPlanCurrIndex].highMipsWithPrec)
  {
    algoDataContext->idleHistory |= DCVS_ALGO_CLASSIC_SET_RAISE;
  }
  else
  {
    algoDataContext->idleHistory |= DCVS_ALGO_CLASSIC_SET_STAY;
  }

  /* Lower the required mips if:
   * We are not already at min
   * The past N periods consistently agree
   */

  if ((algoDataContext->clockPlanCurrIndex > 0 ) &&
      ((algoDataContext->idleHistory & algoDataContext->lowerMask) == algoDataContext->lower))
  {
    /* go down to the next lower level of clock plan index */
    algoDataContext->clockPlanCurrIndex--;
  }

  /* raise the perf level if the past N periods consistent agree*/
  else if ((algoDataContext->clockPlanCurrIndex < algoDataContext->clockPlanMaxIndex ) &&
           ((algoDataContext->idleHistory & algoDataContext->raiseMask) == algoDataContext->raise))
  {
    /* go up to the next higher level clock plan index */
    algoDataContext->clockPlanCurrIndex++;
  }
  newAlgoMips = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips;
    
  ///* Logging for classic algorithm */
  ULogFront_RealTimePrintf(algoDataContext->logHandle, 
                           5,
                           "CLASSIC_ALGO(AvgMipsWPrec: %d) (SysTime: %d) (SleepTime: %d) (DeltaTime: %d) (DeltaSleep: %d)",                       
                           avgMipsWithPrec,
                           DCVSOSSystemTickGet(context),
                           DCVSOSSleepTickGet(context),
                           deltaTime,
                           deltaSleep
                           );
    

  if(newAlgoMips != algoDataContext->mips)
  {
    /* The newly calculated mips of the algorithm has changed. */
    algoDataContext->mips = newAlgoMips;
    /* Reset the idle history */
    algoDataContext->idleHistory = 0;
    DCVSTargetTimerFire(context);
  }
  // Start the next timer...
  DCVSAlgoClassicTimerStart(algoDataContext->timerPeriod, context);

}


/**
  @brief DCVSAlgoClassicTimerCB
  
  This function is the callback from the DCVS sampling timer expiry.

  @param : None

  @return : None

*/
static void DCVSAlgoClassicTimerCB(DCVS_CONTEXT* context)
{
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;

  algoDataContext->timerBusy = 0; 
  DCVSOSCriticalSectionEnter(context);
  /* Receive the current state of the clock in MIPS */ 
  algoDataContext->currState = DCVSAlgoClassicCurrClockStateQuery(context);
  /* The DCVS sampling timer has fired calculate the new mips of the Classic algorithm */
  DCVSAlgoClassicMipsCalculate(context);
  DCVSOSCriticalSectionExit(context); 
 
}

/**
  @brief DCVSAlgoClassicClkChangeCB
  This function is invoked when there is a change in the state of
  clock node . If the change in state was due to the request
  of classic algorithm then it is ignored as the algorithm would capture 
  the stats when the algorithm timer fires.
  If the state of clock node changes due to an external request,
  this function captures the number of instructions executed during
  the timer interval upto the clock change and restarts the DCVS timer
  to expire at its slated time period slot.
  
  The number of instructions(I) is aggregated everytime this function is invoked 
  until the actual timer fire of DCVS algorithm. 
  
  For every interval 'i' the following calculations are used to calculate the 
  number of instructions executed during that interval . 

  Idle for every interval 'i' is calculated using,
  Idi = (Si/Ti);

  where Ti = The time period for the interval ( in secs )
        Si = The sleep period for the interval ( in secs ) 
  
  Busy for every interval would be ,
  Bsi = 1 - Idi 
      = ( Ti - Si )/Ti ;
  The number of instructions executed during an interval 'i' ,

       n
  Ii = ? (Bsi * Ti * Ci) instructions
       i=1
       n
     = ? ((Ti - Si) * Ci) instructions
       i=1
  where Bsi = the fraction of time the system was busy in the interval 'i'
        Ti = The time period for the interval ( in secs )
		Ci = The state of the clock during the interval 'i' ( in instructions /secs )
		This is the last known state of the clock before the clock change happened. 
		n = The number of callbacks which happen due to clock change.

  The final mips for the entire interval would be calculated as 

  M = I / T instructions/secs ;
  where 
            n
      I  =  ? Ii instructions
            i=1

			         n
      T  =  ? Ti secs
            i=1

  @param context: DCVS data context.

  @return : None

*/
static void DCVSAlgoClassicClkChange(DCVS_CONTEXT* context)
{
  /* The system time and sleep time for the interval in ms */
  uint32 deltaTime = 0, localTimeElapsed = 0;
  uint32 deltaSleep = 0;
  uint32 insCount = 0;
  /* Variable to hold the mips value of the interval */
  uint32 intervalMips = 0;
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;

  if(algoDataContext->timerStarted == TRUE && algoDataContext->currState != DCVS_ALGO_CLASSIC_INVALID_CLK_STATE)
  {

    /* The clock change events on the kernel side maybe queued up and not processed instantly if the
       system is busy. They may be queued up. And the system might be set at a particular mips 'X'.
       ( after changing to mips values indicated by the events).
       When the system is not busy the events are finally fired off  one by one, the system would still
       be at the same set mips 'X'. The query would always return mips 'X' although there are multiple
       event callbacks owing to the events fired off by the system.
       Check to see if the previous state of the clock is not the same as the latest query state */
    if (algoDataContext->currState != algoDataContext->prevState)
    {
	     /* Stop the DCVS timer */
      DCVSAlgoClassicTimerStop(context);

      /* Obtain  the system time and sleep time for the particular interval. */
      IntervalSystemSleepTimeGet(&deltaTime, &deltaSleep, context);

      /* Aggregate the time locally */
      localTimeElapsed = algoDataContext->timeElapsed + deltaTime;

      /* Convert the time into ms */
      localTimeElapsed = DCVSOSTicksToMsConvert(localTimeElapsed);

      /* If the future timer expiry is less than or equal to 6.25% of the algorithm timer period then the 
         timer expiry is indicated immediately since its very close to the actual timer expiry anyway. */ 
      if((algoDataContext->timerPeriod < localTimeElapsed)||
         ((algoDataContext->timerPeriod - localTimeElapsed) <= algoDataContext->timerPeriod >> 4))
      {
        DCVSAlgoClassicMipsCalculate(context);
      }
      else
      {
        /* Calculate the number of instructions for this time period */
        if ( deltaTime > deltaSleep )
        {
          intervalMips = algoDataContext->prevState;
	         /* Calculate the insCount with deltaTime and deltaSleep in ms 
             The unit of insCount wouldbe secs*MIPS */
          insCount = (deltaTime - deltaSleep)*intervalMips;
        } 
        /* if (deltaTime < deltaSleep) then insCount = 0 */ 
        /* Aggregate the instructions for this time interval */
        algoDataContext->insCount += insCount;

        /* Aggregate the time */
        algoDataContext->timeElapsed += deltaTime;
        
        /* Update the new state as the previous state of the resource */
        algoDataContext->prevState = algoDataContext->currState;
   
        /* Restart the DCVS timer to expire at its designated timer expiry */
        DCVSAlgoClassicTimerStart(algoDataContext->timerPeriod-localTimeElapsed, context);

      }
    }
  } /* end of if(algoDataContext->timerStarted == TRUE) */   
}



/**
  @brief DCVSAlgoClassicClkChangeCB
  
  This function is invoked when the state of the clock changes.

  @param event_id: Event which caused callback to be fired.
  @param pserver_data: optional pointer to data supplied by NPA.
  @param pclient_data: optional pointer to data passed by client at time 
  of registration.

*/
void DCVSAlgoClassicClkChangeCB( dcvs_npa_event event_id,
                                 void* pserver_data,
                                 void* pclient_data )
{
  
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)pclient_data;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;
  
  // printf(" DCVSAlgoClassicClkChangeCB before CS \n "); 

  /* State of the CPU clock has changed. */    
  DCVSOSCriticalSectionEnter(context);

  /* Obtain the current state of the clock */
  algoDataContext->currState = DCVSAlgoClassicCurrClockStateQuery(context);

  /* The DCVS sampling timer has still not fired, the clock has changed in the meantime. */
  DCVSAlgoClassicClkChange(context);

  DCVSOSCriticalSectionExit(context);
  
  // printf(" DCVSAlgoClassicClkChangeCB after CS \n ");
}

/**
  @brief DCVSAlgoClassicInitialize
  
  This function is used to initialize the classic DCVS algorithm specific members .

  @param  context: DCVS data context. 

  @return : Initial mips of the system

*/
static uint32 DCVSAlgoClassicInitialize(void* param)
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;
  
  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext;

  /* Allocate the memory for the algorithm data context. */
  if(DALSYS_Malloc(sizeof(DCVS_ALGO_CLASSIC),  
                   (void**) &context->algoDataContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }

  algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;
  /* Initialize the induvidual members of the classic algorithm. */
  memset(algoDataContext, 0, sizeof(DCVS_ALGO_CLASSIC));

  algoDataContext->logHandle = DCVSCoreLogHandleGet(context);
  CORE_VERIFY(algoDataContext->logHandle);

  /* Get the current clock plan of the target */
  algoDataContext->clockPlan = DCVSCoreClockPlanGet(context);

  /* Create a DCVS timer for periodic sampling. */
  algoDataContext->timerHandle = DCVSOSTimerCreate(DCVSAlgoClassicTimerCB, context);

  /* Register to be notified when the state of the physical clock changes .
     Pass in the The DCVS data context as a parameter. */
  dcvs_npa_events_register(DCVS_NPA_EVENT_CLK_CHANGE, DCVSAlgoClassicClkChangeCB, context);
  
  /* Set the inital mips to max */
  algoDataContext->mips = DCVSAlgoClassicReset(context);

  return(algoDataContext->mips);
}


/**
  @brief DCVSAlgoClassicBitMaskCalculate
  
  This function is used to calculate::
  * Mask to lower the CPU frequency(lower/raise decision mask in idle_history).
  * Bit pattern to lower the CPU frequency(lower/raise decision bits in idle_history ).
  * Mask to raise the CPU frequency(lower/raise decision mask in idle_history).
  * Bit pattern to raise the CPU frequency(lower/raise decision bits in idle_history ).
  
  @param  context: The DCVS data context.

  @return : None

*/  
static void DCVSAlgoClassicBitMaskCalculate(DCVS_CONTEXT* context)
{
  uint32 tempLowerN, tempRaiseN;
    /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;

  tempLowerN = algoDataContext->lowerN;
  tempRaiseN = algoDataContext->raiseN;

  /* lowerN: lower the perf level if the last n period agrees to
             lower the perf level consistently
             calculate the mask and bits based on the lowerN
  */
  while (tempLowerN--)
  {
    /* the mask */
    algoDataContext->lowerMask <<= DCVS_ALGO_CLASSIC_SET_BITS;
    algoDataContext->lowerMask |= DCVS_ALGO_CLASSIC_SET_BMASK;

    /* the bits */
    algoDataContext->lower <<= DCVS_ALGO_CLASSIC_SET_BITS;
    algoDataContext->lower |= DCVS_ALGO_CLASSIC_SET_LOWER;
  }

  /* raiseN: raise the perf level if the last n period agrees to
             raise the perf level consistently
             calculate the mask and bits based on the raiseN
  */
  while (tempRaiseN--)
  {
    algoDataContext->raiseMask <<= DCVS_ALGO_CLASSIC_SET_BITS;
    algoDataContext->raiseMask |= DCVS_ALGO_CLASSIC_SET_BMASK;

    algoDataContext->raise <<= DCVS_ALGO_CLASSIC_SET_BITS;
    algoDataContext->raise |= DCVS_ALGO_CLASSIC_SET_RAISE;
  }

}


/**
  @brief DCVSAlgoClassicSleepCb
  
  This function is the callback from the sleep task for the events registered 
  by the classic DCVS algorithm.
  
  @param  context: DCVS data context.

  @return : None

*/  
#ifdef SLEEP_EVENT_HANDLING
void DCVSAlgoClassicSleepCb(sleep_event event_id, void* server_data, void* client_data)
{
  /* Obtain The DCVS data context*/
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)client_data;
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;

  switch (event_id)
  {
    case SLEEP_EVENT_POWER_REMOVE:
      /* The processor is about to enter Apps PC. Stop the DCVS active
         timer so we dont wake up from PowerCollapse */
      if ( algoDataContext->timerBusy )
      {
        DCVSOSTimerStop(algoDataContext->timerHandle); 
      }
      break;

    case SLEEP_EVENT_POWER_RESTORE:
      /* The processor is about to exit Apps PC . Start the DCVS timer again
        so we can continue with DCVS algorithm */
      DCVSOSTimerStart(algoDataContext->timerHandle, algoDataContext->timerPeriod);

      break;
  }
}

static void DCVSAlgoClassicSleepEventsRegister(DCVS_CONTEXT* context)
{
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;
  if (!algoDataContext->sleepRegistered)
  {
    /* register the sleep events call-back */
    sleep_events_register(SLEEP_EVENT_POWER_REMOVE, DCVSAlgoClassicSleepCb, NULL);
    sleep_events_register(SLEEP_EVENT_POWER_RESTORE, DCVSAlgoClassicSleepCb, NULL);

    algoDataContext->sleepRegistered = 1;
  }
}
#endif //SLEEP_EVENT_HANDLING

/**
  @brief DCVSAlgoClassicThresholdTableSetup
  
  This function is used to calculate the low and high threshold values in mips
  for each clock level.
  
  @param  context: The DCVS data context.

  @return : None

*/
static void DCVSAlgoClassicThresholdTableSetup(DCVS_CONTEXT* context)
{
  uint32 currClkMips, prevClkMips, highWaterMarkIdle, lowWaterMarkBusy;
  int32 i;
  DCVS_ALGO_CLASSIC* algoDataContext = context->algoDataContext;

  //CORE_VERIFY_PTR(algoDataContext->thresholdTable);
  /* Allocate the memory for threshold table information for the resources */
  if(DALSYS_Malloc(sizeof(DCVS_ALGO_CLASSIC_CLK_THRESHOLD)*(algoDataContext->clockPlanMaxIndex+1),  
                   (void**) &algoDataContext->thresholdTable)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);
  }


  /* Precalculate the low/high thresholds for the different clock levels 
     for the algorithm */
  for ( i = algoDataContext->clockPlanMaxIndex; i >= 0 ; i-- )
  {
    /* Calculate the HIGH threshold for the clock level
   	   High threshold is fixed at 90% of mips at the current clock level. */
    algoDataContext->thresholdTable[i].highMipsWithPrec = (uint32)( 9 * (algoDataContext->clockPlan->clockPlanTable[i].reqMips)) ;

    /* Calculate the LOW threshold for the clock level
       High threshold is fixed at 90% of mips at the current clock level. */

    if ( i == 0 )
    {
      algoDataContext->thresholdTable[i].lowMipsWithPrec = 0;
    }
    else
    {
      currClkMips = algoDataContext->clockPlan->clockPlanTable[i].reqMips;
      prevClkMips = algoDataContext->clockPlan->clockPlanTable[i-1].reqMips;  
      /* Calculate the high watermark percent for idle for this clock level */
      highWaterMarkIdle = (((currClkMips - prevClkMips) * 100) / currClkMips) + DCVS_ALGO_CLASSIC_HIGH_WATERMARK_SAFETY;
      /* The low watermark percentage for busy for this clock level */
      lowWaterMarkBusy = 100 - highWaterMarkIdle;
	  
      /* Calculate the low threshold mips for the current clock level */
      algoDataContext->thresholdTable[i].lowMipsWithPrec = (uint32)(lowWaterMarkBusy * MUL_TEN(algoDataContext->clockPlan->clockPlanTable[i].reqMips))/100;
    }
  }  
  /* Set the inital mips to max */
  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlanMaxIndex;
  algoDataContext->mips = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips;
  algoDataContext->prevState = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips;
}



/**
  @brief DCVSAlgoClassicBuildParams
  
  This function is used to get the parameters for the classic DCVS algorithm .

  @param  : DCVS data context.

  @return : None

*/
static void DCVSAlgoClassicBuildParams(void* param)
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;

  /* Read the parameters from nv */

  /* Read the timer period */
  DcvsNvParamUint32Read(context->coreId, 
                        "Default", 
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_CLASSIC_SAMPLE_PERIOD], 
                        &algoDataContext->timerPeriod);

  /* Read the number of consecutive votes to lower the processor frequency*/
  DcvsNvParamUint32Read(context->coreId, 
                        "Default", 
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_CLASSIC_SAMPLES_LOWER],
                        &algoDataContext->lowerN);

  /* Read the number of consecutive votes to higher the processor frequency*/
  DcvsNvParamUint32Read(context->coreId, 
                        "Default", 
                        &dcvsParamsAlgDefault[DCVS_PARAMS_ALG_CLASSIC_SAMPLES_RAISE], 
                        &algoDataContext->raiseN);

  

  /* The maximum index of the clock plan for this target.*/
  algoDataContext->clockPlanMaxIndex = algoDataContext->clockPlan->numLevels-1;

  /* Set up the threshold table for each clock level. */
  DCVSAlgoClassicThresholdTableSetup(context);

  /* Create a query handle to the clock resource */ 
  CORE_VERIFY_PTR(algoDataContext->queryHandle = npa_create_query_handle(context->clockDependency));

  CORE_VERIFY_PTR(algoDataContext->resource = npa_query_get_resource(algoDataContext->queryHandle));

  DCVSAlgoClassicBitMaskCalculate(context);

  /* Register for sleep events. Comment for now. */
  /* DCVSAlgoClassicSleepEventsRegister(context); */

  /* Set up the initial state of the clock. */
  algoDataContext->prevState = DCVSAlgoClassicCurrClockStateQuery(context);

}/* DCVSAlgoClassicBuildParams */

/**
  @brief DCVSAlgoClassicEnable
  
  This function is used to set the state of the classic algorithm specific members
  when enabling the algorithm .

  @param  : DCVS data context.

  @return : The mips of the system after enabling the algorithm.

*/
static uint32 DCVSAlgoClassicEnable(void* param)
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;

  /* Clear the idle history */
  algoDataContext->idleHistory = 0;

  /* Set the initial mips to max */
  algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlanMaxIndex;
  algoDataContext->mips = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips;

  /* Start the DCVS algorithm timer. */
  DCVSAlgoClassicTimerStart(algoDataContext->timerPeriod, context);
  
  return(algoDataContext->mips);
}


/**
  @brief DCVSAlgoClassicDisable
  
  This function is used to disable the DCVS algorithm .

  @param  : DCVS data context.

  @return : The mips of the system after disabling the algorithm.

*/
static uint32 DCVSAlgoClassicDisable(void* param)
{
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;
  
  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;

  DCVSAlgoClassicTimerStop(context);
  /* Return max mips */
  return(algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels-1].reqMips);
}

/**
  @brief DCVSAlgoClassicDecideMips
  
   Implementation of the DCVS classic algorithm for a single core.
   This function decides the performance level and acts upon it. It
   first updates the decision as whether to stay, raise or lower (the
   performance level) for this timer interval in the decision
   data. The decision data is implemented in a sliding window fashion
   and it tracks the decisions for the past 16 timer intervals.

   If the decisions for the past 12 profiling cycle are all to
   "LOWER", the performance level will then be step down one level
   lower. If the decisions for the past 6 profiling cycle are
   all to "RAISE", the performance level will be raise to the  maximum
   immediately.

  @param  context: The DCVS data context.

  @return : Mips required

*/
static uint32 DCVSAlgoClassicDecideMips(void* param)
{
  uint32 retMips;

  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;

  /* Obtain the client context from the resource. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Obtain the algorithm data context. */
  DCVS_ALGO_CLASSIC* algoDataContext = (DCVS_ALGO_CLASSIC*)context->algoDataContext;

  if(resDataContext->algoMipsChangedOrTimerFired)
  {
    /* The mips calculated by the algorithm has changed. */
  }
  /* If there is an impulse request */
  if(resDataContext->activeImpulse)
  {
    /* We have an impulse input. */
    /* Stop the DCVS sampling timer */
    DCVSOSTimerStop(algoDataContext->timerHandle, context);      
    DCVSOSCriticalSectionEnter(context);
    /* Kick the system high. */
    algoDataContext->clockPlanCurrIndex = algoDataContext->clockPlanMaxIndex;
    algoDataContext->mips = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlanCurrIndex].reqMips;
    DCVSOSCriticalSectionExit(context);
    /* Restart the DCVS sampling timer */
    DCVSOSTimerStart(algoDataContext->timerHandle, algoDataContext->timerPeriod, context);

  }  
 
  if (resDataContext->activeIsochronous)
  {
    algoDataContext->mips = DCVSAlgoClassicReset(context);
  }

  retMips = MaxMips(algoDataContext->mips, resDataContext->activeFloor);

  /* Logging for classic algorithm */
  ULogFront_RealTimePrintf(algoDataContext->logHandle, 
                           4,
                           "CLASSIC(mips: %d) (activeFloor: %d) (activeImpulse: %d) (activeIsochronous: %d) ",                       
                           retMips,
                           resDataContext->activeFloor,
                           resDataContext->activeImpulse,
                           resDataContext->activeIsochronous);
  
  return retMips;
}/* DCVSAlgoClassicDecideMips */


DCVS_CORE_ALGO defaultAlgorithm = 
{
  "Classic",
  dcvsParamsAlgDefault,
  DCVSAlgoClassicInitialize,
  DCVSAlgoClassicBuildParams,
  DCVSAlgoClassicDecideMips,
  DCVSAlgoClassicReset,
  DCVSAlgoClassicEnable,
  DCVSAlgoClassicDisable
};

#endif //DEFEATURE_DCVS_ALGO_CLASSIC

