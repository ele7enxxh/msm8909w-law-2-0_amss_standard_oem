/*=========================================================================== 

DESCRIPTION:  Implementation of CLM Engine layer that monitors the CPU
Utilization by way of measuring the Utilized and Budget PCycles using the PMU
and QURT APIs. The measurement is done on a regular interval using a timer
interrupt. The Engine layer interfaces with the CLM Client layer to send out
measurement notifications. The Engine Layer can be considered as the
back-end while the Client Layer is the front end
 
=========================================================================== 
Copyright © 2013-2015 QUALCOMM Technologies, Incorporated. All Rights Reserved. 
QUALCOMM Confidential and Proprietary. 
===========================================================================  
 
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/clm/src/common/CLM_Engine.c#1 $

=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "CLM_Engine.h"
#include "ClockDefs.h"
#include "qurt.h"
#include "qurt_consts.h"
#include "qurt_event.h"
#include "sleep_lpr.h"
#include "timer.h"
#include "npa.h"
#include "npa_resource.h"
#include "msmhwiobase.h"
#include "HALhwio.h"
#include CLM_HWIO_H

/* PMU Event Config for 4HWT systems
 * 3e=# Cycles w/4 HW threads active; 3d=# Cycles w/3 HW threads active;
 * 3c=# Cycles w/2 HW threads active; 3b=# Cycles w/1 HW threads active
 */
#define CLM_PMU_EVT_CONFIG_4HWT 0x3e3d3c3b

/* PMU Event Config for 3HWT systems
 * 3d=# Cycles w/3 HW threads active; 3c=# Cycles w/2 HW threads active;
 * 3b=# Cycles w/1 HW threads active; 03=# of packets executed
 */
#define CLM_PMU_EVT_CONFIG_3HWT 0x033d3c3b

/* PMUCFG */
#define CLM_PMU_CONFIG 0xf

/* Macro for reading Clock Gating state */
#define CLM_CGC_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CGC_OVERRIDE_ADDR, m)

/* Global context for the CLM Engine */
struct
{
  /* Indicates whether CLM has initialized */
  uint32 initDone;
  
  CoreMutexType *lock;

  uint32 numClients;
  CLM_Client *clientList;

  /* Struct with fields required to compute/store util info */
  CLM_EngineCalculation calc;

  /* CLM timer */
  timer_type timer;
  timer_group_type timerGroup;

  /* Query Handle to the "/clk/cpu" resource */
  npa_query_handle clkQuery;

  /* Number of HW threads on this target */
  uint32 nHWThreads;

  /* Records when the CLM timer was last started */
  uint64 startSysTick;

  /* Indicates whether the CLM timer is started */
  uint32 timerStarted;

  /* Events to track Q6 clk changes */
  npa_event_handle preClkChangeEvent;
  npa_event_handle postClkChangeEvent;

  /* Interface between Engine and Client layers */
  CLM_ClientCB clientCb;

} clmEngineCtxt;


/* Variable to denote availability of the PMU (FALSE = available, TRUE = not) */
static uint32 clmPmuBusy = FALSE;

/* Set to one of the CLM_PMU_EVT_CONFIG_XXX values depending on the number
   of HW threads on this target */
static uint32 clmPmuEvtConfig;

/** 
 * <!-- CLM_EngineIsClockGatingDisabled -->
 *  
 * @brief Returns TRUE if ClockGating is disabled
 */
uint32 CLM_EngineIsClockGatingDisabled( void )
{
  /* In general, we rely on the PMU for load information. But if the PMU is
   * unavailable, we use the qurt_profile API. If Clock gating is disabled, 
   * this API cannot be used.
   */
  clmEngineCtxt.calc.clkGatingDisabled = 
    ( CLM_CGC_OVERRIDE_INM( HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_BMSK
                            | HWIO_MSS_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_BMSK )
                            != 0 );

  return clmEngineCtxt.calc.clkGatingDisabled;
}

/** 
 * <!-- CLM_EnginePMUInit -->
 * @brief Initialize the PMU for regular/nominal use
 */
static void CLM_EnginePMUInit( void )
{
  qurt_pmu_set( QURT_PMUEVTCFG, clmPmuEvtConfig );
  qurt_pmu_set( QURT_PMUCFG, CLM_PMU_CONFIG );

  INTLOCK();
  qurt_pmu_set( QURT_PMUCNT0, 0 );
  qurt_pmu_set( QURT_PMUCNT1, 0 );
  qurt_pmu_set( QURT_PMUCNT2, 0 );
  qurt_pmu_set( QURT_PMUCNT3, 0 ); 
  INTFREE();
}

/** 
 * <!-- DcvsQdspPMUbusy -->
 *  
 * @brief Called by QURT to indicate to CLM that the PMU is free or busy.
 * Updates the global clmPmuBusy, used by EngineMeasure.
 *
 * @param isBusy: TRUE when PMU is being used
 */
void DcvsQdspPMUbusy( int isBusy )
{
  Core_MutexLock( clmEngineCtxt.lock );
  
  if ( isBusy )
  {
    clmPmuBusy = TRUE;
    
    /* Flag that PMU data cannot be used at timer expiry */
    clmEngineCtxt.calc.prevCyclesNThreadsBusy[0] = 0xFFFFFFFF;
  }
  else
  {
    clmPmuBusy = FALSE;
    /* Reinitialize the PMU for CLM */
    CLM_EnginePMUInit();
    
    /* But leave prevCyclesNThreadsBusy[0] set to 0xFFFFFFFF. We'll use the PMU
       from the next window, as we may be midway through the current one */
  }
  
  ULOG_RT_PRINTF_2( clmLog, "DcvsQdspPMUbusy (isBusy: %d) (clmPmuBusy: %d)",
                    isBusy, clmPmuBusy );

  Core_MutexUnlock( clmEngineCtxt.lock );
}

/**
 * @brief CLM_EngineGetClkState
 * 
 * @return : The current state of "/clk/cpu" in KHz
 */
static uint32 CLM_EngineGetClkState( void )
{
  npa_query_type result;
  CORE_VERIFY( 
    NPA_QUERY_SUCCESS == npa_query( clmEngineCtxt.clkQuery,
                                    NPA_QUERY_CURRENT_STATE, &result ) );

  return result.data.state;
}

/** 
 * <!--  CLM_EngineStartTimer -->
 * 
 * @brief Starts the CLM Engine Timer
 * @param Timer period in us
 */
static void CLM_EngineStartTimer( uint32 timeUs )
{
  timer_set_64( &clmEngineCtxt.timer, timeUs, 0, T_USEC );
  clmEngineCtxt.timerStarted = TRUE;
}

/** 
 * <!-- CLM_EngineFindNextTimerPeriod -->
 *  
 * @brief Finds the next timer period from the measurement window sizes of
 * clients.
 *
 * @param elapsedTimeUs : Time elapsed from when the timer was last set
 *  
 * @return Next timer expiry period
 */
static uint32 CLM_EngineFindNextTimerPeriod( int32 elapsedTimeUs )
{
  int minPeriodUs = 0x7FFFFFFF;
  CLM_Client *client = clmEngineCtxt.clientList;
  CORE_VERIFY_PTR( client );
  
  do
  {
    /* first subtract elapsedTime */
    client->pendingTimeUs -= elapsedTimeUs;

    /* reset timer period for client if measurement period has expired,
       accounting for a 250us inaccuracy in the timer subsystem */
    if ( client->pendingTimeUs <= CLM_TIMER_TOLERANCE )
    {
      client->pendingTimeUs = client->measPeriodUs;
    }

    minPeriodUs = MIN( minPeriodUs, client->pendingTimeUs );
    client = client->next;
  } while ( client );

  return minPeriodUs;
}

/** 
 * <!-- CLM_EngineClkChangeAction -->
 */
void CLM_EngineClkChangeAction( uint32 prevClkKhz, uint32 newClkKhz )
{
  const char *eventLogStr = 
    ( newClkKhz > prevClkKhz ) ? "EnginePreClockChangeEvent" :
                                 "EnginePostClockChangeEvent";

  Core_MutexLock( clmEngineCtxt.lock );
    
  clmEngineCtxt.calc.curClkKhz = newClkKhz;

  /* If clients are registered and this callback arrived while the timer
     was still running... */
  if ( clmEngineCtxt.clientList && clmEngineCtxt.timerStarted == 1 )
  {
    uint64 endSysTick = qurt_sysclock_get_hw_ticks();
    uint64 deltaSysTick = endSysTick - clmEngineCtxt.startSysTick;
    uint64 elapsedUs = deltaSysTick * 1000000 / TICKS_PER_SEC;

    clmEngineCtxt.calc.timeElapsedUs += elapsedUs;

    clmEngineCtxt.calc.sumBudgetCycles += 
      deltaSysTick * prevClkKhz * 1000 / TICKS_PER_SEC;

    /* Next sub-window starts */
    clmEngineCtxt.startSysTick = endSysTick;
  
    ULOG_RT_PRINTF_9( clmLog, "%s (prevClkKHz: %d) (newClkKhz: %d) "
                      "(totalTimeElapsedUs: %llu) "
                      "(sumBudgetCycles: %llu) "
                      "(startSysTick: 0x%llx)",
                      eventLogStr, prevClkKhz, clmEngineCtxt.calc.curClkKhz,
                      ULOG64_LOWWORD(clmEngineCtxt.calc.timeElapsedUs),
                      ULOG64_HIGHWORD(clmEngineCtxt.calc.timeElapsedUs),
                      ULOG64_LOWWORD(clmEngineCtxt.calc.sumBudgetCycles),
                      ULOG64_HIGHWORD(clmEngineCtxt.calc.sumBudgetCycles),
                      ULOG64_LOWWORD(clmEngineCtxt.startSysTick),
                      ULOG64_HIGHWORD(clmEngineCtxt.startSysTick) );
  }
  else
  {
    ULOG_RT_PRINTF_3( clmLog, "%s (prevClkKHz: %d) (newClkKhz: %d)",
                      eventLogStr, prevClkKhz, clmEngineCtxt.calc.curClkKhz );
  }

  Core_MutexUnlock( clmEngineCtxt.lock );
}

/** 
 * <!-- CLM_EnginePreClkChangeCallback -->
 *  
 * @brief NPA event callback invoked before a change in state of
 * /clk/cpu. This callback will be invoked synchronous to the clkChange.
 * If the clock is ramping up, we record an endSysTick, update budgetCycles
 * and mark the start of a new window here. This means that we are budgeting
 * at the new higher clk sooner than the clock is actually at that level,
 * but this prevents issues where busyCycles end up > budgetCycles, which
 * would happen if we budgeted in the postChange callback.
 * When we are ramping down, we do the reverse, record, update and mark
 * a start in the postChange callback.
 *
 * @param user_data Ignored
 * @param cb_event_type Event type
 * @param cb_data Event data containing details of the state change
 * @param size size of Event data
 */
void CLM_EnginePreClkChangeCallback( void *userData,  unsigned int eventType,
                                     void *eventData, unsigned int size )
{
  /* This callback is invoked before the clock is changed */
  npa_prepost_change_data *changeData = (npa_prepost_change_data *)eventData;
  uint32 prevClkKhz = clmEngineCtxt.calc.curClkKhz;
  uint32 newClkKhz = changeData->to_state;
  
  if ( newClkKhz > prevClkKhz )
  {
    CLM_EngineClkChangeAction( prevClkKhz, newClkKhz );
  }
}

/** 
 * <!-- CLM_EnginePostClkChangeCallback -->
 *  
 * @brief NPA event callback invoked after a change in state of
 * /clk/cpu. This callback will be invoked synchronous to the clkChange.
 * If the clock is ramping down, we record an endSysTick, update budgetCycles
 * and mark the start of a new window here.
 *
 * @param user_data Ignored
 * @param cb_event_type Event type
 * @param cb_data Event data containing details of the state change
 * @param size size of Event data
 */
void CLM_EnginePostClkChangeCallback( void *userData,  unsigned int eventType,
                                      void *eventData, unsigned int size )
{
  /* This callback is invoked after the clock is changed */
  npa_prepost_change_data *changeData = (npa_prepost_change_data *)eventData;
  uint32 prevClkKhz = clmEngineCtxt.calc.curClkKhz;
  uint32 newClkKhz = changeData->to_state;
  
  if ( newClkKhz < prevClkKhz )
  {
    CLM_EngineClkChangeAction( prevClkKhz, newClkKhz );
  }
  else if ( newClkKhz > prevClkKhz )
  { /* This handles the rare case where the Q6clk changed to a higher
     * value than the aggregated request. In the pre-change callback, we only
     * see the aggregated request as to_state. In the post-change, we see
     * the actual new state as to_state. Correct curClkKhz if != to_state.
     */
    clmEngineCtxt.calc.curClkKhz = newClkKhz;
    ULOG_RT_PRINTF_1( 
      clmLog, "EnginePostClockChangeEvent (correctedCurClkKhz: %d)",
      clmEngineCtxt.calc.curClkKhz );
  }
}

/** 
 * <!-- CLM_EngineMeasure --> 
 *  
 * @brief Core routine that collects PMU/other data and profiles load on
 * the system
 */
static void CLM_EngineMeasure( void )
{
  uint32 pmuInput[clmEngineCtxt.nHWThreads];
  uint64 endSysTick;
  uint64 sum = 0;
  uint64 deltaSysTick;
  uint64 idleCyclesQurt[clmEngineCtxt.nHWThreads];
  uint64 elapsedUs;
  int i;
  uint32 *prevCyclesNThreadsBusy = clmEngineCtxt.calc.prevCyclesNThreadsBusy;

  memset(pmuInput, 0, sizeof(pmuInput));
  
  Core_MutexLock( clmEngineCtxt.lock );
  
  clmEngineCtxt.timerStarted = 0;
  
  /* If PMU data can be relied upon i.e. the PMU is available now and has been
     available since start of window */
  if ( !clmPmuBusy && prevCyclesNThreadsBusy[0] != 0xFFFFFFFF )
  {
    uint32 *cyclesNThreadsBusy = clmEngineCtxt.calc.cyclesNThreadsBusy;

    clmEngineCtxt.calc.pmuDisabled = FALSE;
    
    INTLOCK();
    for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
    {
      pmuInput[i] = qurt_pmu_get( i );
    }
    INTFREE();
    
    /* Mark end of the budget/measurement window */
    endSysTick = qurt_sysclock_get_hw_ticks();
    
    for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
    {
      if ( pmuInput[i] < prevCyclesNThreadsBusy[i] )
      {
        /* has wrapped */
        cyclesNThreadsBusy[i] = (0xFFFFFFFF - prevCyclesNThreadsBusy[i])
                                + pmuInput[i] + 1;
      }
      else
      {
        cyclesNThreadsBusy[i] = pmuInput[i] - prevCyclesNThreadsBusy[i];
      }
    }
    
    if ( clmEngineCtxt.nHWThreads == 4 )
    {
      ULOG_RT_PRINTF_4( clmLog, 
        "Engine Measure (0x3b: %u) (0x3c: %u) (0x3d: %u) (0x3e: %u)",
        pmuInput[0], pmuInput[1], pmuInput[2], pmuInput[3] );
      sum = (uint64)cyclesNThreadsBusy[0] + cyclesNThreadsBusy[1] +
         	  cyclesNThreadsBusy[2] + cyclesNThreadsBusy[3];
      ULOG_RT_PRINTF_6( clmLog, "\tCycles with (1hwt: %u) (2hwt: %u) "
	                      "(3hwt: %u) (4hwt: %u) busy. (sum: %llu)",
                        cyclesNThreadsBusy[0], cyclesNThreadsBusy[1], 
                        cyclesNThreadsBusy[2], cyclesNThreadsBusy[3], 
                        ULOG64_LOWWORD(sum), ULOG64_HIGHWORD(sum) );
    }
    else
    {
      ULOG_RT_PRINTF_3( clmLog, 
        "Engine Measure (0x3b: %u) (0x3c: %u) (0x3d: %u)",
        pmuInput[0], pmuInput[1], pmuInput[2] );
      sum = (uint64)cyclesNThreadsBusy[0] + cyclesNThreadsBusy[1] + 
	          cyclesNThreadsBusy[2];
      ULOG_RT_PRINTF_5( clmLog, "\tCycles with (1hwt: %u) (2hwt: %u) "
	                      "(3hwt: %u) busy. (sum: %llu)",
                        cyclesNThreadsBusy[0], cyclesNThreadsBusy[1], 
                        cyclesNThreadsBusy[2], 
                        ULOG64_LOWWORD(sum), ULOG64_HIGHWORD(sum) );
    }
    
    /* Busy Cycles */
    clmEngineCtxt.calc.busyCycles += sum;

    /* Idle Cycles for threshold updates.
       TODO: Confirm this data is valid irrespective of clkGatingState */
    qurt_profile_get_idle_pcycles( idleCyclesQurt );
    for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
    {
      clmEngineCtxt.calc.idleCyclesQurt[i] = 
        idleCyclesQurt[i] - clmEngineCtxt.calc.prevIdleCyclesQurt[i];
    }
  }
  else
  {
    uint32 clkGatingDisabled = CLM_EngineIsClockGatingDisabled();
    clmEngineCtxt.calc.pmuDisabled = TRUE;
    
    ULOG_RT_PRINTF_5( clmLog, 
      "Engine Measure (clmPmuBusy: %u) (prevCyclesNThreadsBusy[0]: 0x%x) "
      "(clkGatingDisabled: %u) (prevBusyCyclesQurt: 0x%llx)",
      clmPmuBusy, prevCyclesNThreadsBusy[0], clkGatingDisabled, 
      ULOG64_LOWWORD(clmEngineCtxt.calc.prevBusyCyclesQurt),
      ULOG64_HIGHWORD(clmEngineCtxt.calc.prevBusyCyclesQurt) );
    
    if ( clkGatingDisabled || 
         clmEngineCtxt.calc.prevBusyCyclesQurt == 0xFFFFFFFFFFFFFFFF )
    {
      /* PMU is unavailable, and clkGating is either disabled or was disabled
         at the start of the window... */
      clmEngineCtxt.calc.busyCycles = clmEngineCtxt.calc.sumBudgetCycles;
    }
    else
    {
      /* PMU not available (or reliable this window), but we can use
         Qurt profile data */
      uint64 busyCyclesQurt;

      /* Idle Cycles  */
      qurt_profile_get_idle_pcycles( idleCyclesQurt );
      for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
      {
        clmEngineCtxt.calc.idleCyclesQurt[i] = 
          idleCyclesQurt[i] - clmEngineCtxt.calc.prevIdleCyclesQurt[i];
      }

      /* Busy Cycles */
      busyCyclesQurt = qurt_get_core_pcycles();
      clmEngineCtxt.calc.busyCycles += 
        (busyCyclesQurt - clmEngineCtxt.calc.prevBusyCyclesQurt);
    }
    
    /* Mark end of the budget/measurement window */
    endSysTick = qurt_sysclock_get_hw_ticks();
  }
  
  /* Compute budget Cycles */
  deltaSysTick = endSysTick - clmEngineCtxt.startSysTick;
  elapsedUs = deltaSysTick * 1000000 / TICKS_PER_SEC;
  
  clmEngineCtxt.calc.timeElapsedUs += (uint32)elapsedUs;
 
  clmEngineCtxt.calc.sumBudgetCycles +=
    deltaSysTick * clmEngineCtxt.calc.curClkKhz * 1000 / TICKS_PER_SEC;
                            
  ULOG_RT_PRINTF_8( clmLog, "\t(endSysTick: 0x%llx) (totalTimeElapsedUs: %d) "
                    "(sumBudgetCycles: %llu) (busyCycles: %llu) "
                    "(curClkKhz: %d)",
                    ULOG64_LOWWORD(endSysTick), ULOG64_HIGHWORD(endSysTick),
                    clmEngineCtxt.calc.timeElapsedUs,
                    ULOG64_LOWWORD(clmEngineCtxt.calc.sumBudgetCycles),
                    ULOG64_HIGHWORD(clmEngineCtxt.calc.sumBudgetCycles),
                    ULOG64_LOWWORD(clmEngineCtxt.calc.busyCycles),
                    ULOG64_HIGHWORD(clmEngineCtxt.calc.busyCycles),
                    clmEngineCtxt.calc.curClkKhz );

  Core_MutexUnlock( clmEngineCtxt.lock );
}

/** 
 * <!--  CLM_EngineInitPMUData -->
 *  
 * @brief Initialize PMU data variables before the first measurement
 */
static void CLM_EngineInitPMUData( void )
{
  int i;
  uint64 idleCyclesQurt[clmEngineCtxt.nHWThreads];

  /* Current Busy Cycles count from Qurt */
  clmEngineCtxt.calc.prevBusyCyclesQurt = qurt_get_core_pcycles();

  /* Current Idle Cycles count from Qurt */
  qurt_profile_get_idle_pcycles( idleCyclesQurt );

  INTLOCK();
  for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
  {
    clmEngineCtxt.calc.prevIdleCyclesQurt[i] = idleCyclesQurt[i];
    clmEngineCtxt.calc.prevCyclesNThreadsBusy[i] = qurt_pmu_get( i );
  }
  INTFREE();

  ULOG_RT_PRINTF_2( clmLog,
    "Engine PMU Data Initialized (prevBusyCyclesQurt: %llu)",
    ULOG64_LOWWORD(clmEngineCtxt.calc.prevBusyCyclesQurt),
	  ULOG64_HIGHWORD(clmEngineCtxt.calc.prevBusyCyclesQurt) );
}

/** 
 * <!-- CLM_EngineGetMaxFreqKhz -->
 *  
 * @brief Returns the max frequency supported on the target in KHz
 */
static uint32 CLM_EngineGetMaxFreqKhz( void )
{
  npa_query_type result;
  uint32 numLevels;

  /* Retrieve number of clock levels */
  CORE_VERIFY( 
    npa_query( clmEngineCtxt.clkQuery, CLOCK_NPA_QUERY_NUM_PERF_LEVELS, 
               &result ) == NPA_QUERY_SUCCESS );

  numLevels = result.data.value;

  CORE_VERIFY( npa_query( clmEngineCtxt.clkQuery,
                          CLOCK_NPA_QUERY_PERF_LEVEL_KHZ + numLevels - 1, 
                          &result ) == NPA_QUERY_SUCCESS );

  return result.data.value;
}

/** 
 * <!--  CLM_EnteringLowPowerMode -->
 *  
 * @brief Used as a placeholder
 */
void CLM_EnteringLowPowerMode( uint64_t wakeupTick )
{
}

/** 
 * <!--  CLM_ExitingLowPowerMode -->
 *  
 * @brief Record timestamp of last exit from CLM LPR
 */
void CLM_ExitingLowPowerMode( void )
{
  clmEngineCtxt.calc.lastLPRexitTS = qurt_sysclock_get_hw_ticks();
}

/** 
 * <!-- CLM_RegisterLPR -->
 *  
 * @brief Registers the CLM LPR with Sleep
 */
static void CLM_RegisterLPR( void *context, unsigned int event_type, 
                             void *data, unsigned int data_size )
{
  npa_client_handle sleepClient;

  CORE_VERIFY_PTR( 
    sleepClient = npa_create_sync_client( SLEEP_LPR_NODE_NAME,
                                          "CLM", NPA_CLIENT_REQUIRED ) );

  sleep_define_lpr_str( "CLM", sleepClient );

  /* Enable the LPR(M) */
  npa_issue_required_request( sleepClient, SLEEP_ALL_LPRMS );
}


/** 
 * <!--  CLM_EngineStart -->
 * 
 * @brief We have atleast one client. Init PMU data and start
 * the timer. This callback function is setup after the first client
 * is registered and triggered after CLM_EngineInit is complete.
 */
void CLM_EngineStart( void *context, unsigned int event_type, 
                      void *data,    unsigned int data_size )
{
  CLM_EngineInitPMUData();

  clmEngineCtxt.calc.curTimerPeriod = CLM_EngineFindNextTimerPeriod( 0 );

  /* Start the timer */
  CLM_EngineStartTimer( clmEngineCtxt.calc.curTimerPeriod );

  /* Mark the start of timer window */
  clmEngineCtxt.startSysTick = qurt_sysclock_get_hw_ticks();
  
  ULOG_RT_PRINTF_3( clmLog, "Timer Set "
                    "(startSysTick: 0x%llx) (periodUs: %d)",
                    ULOG64_LOWWORD(clmEngineCtxt.startSysTick),
                    ULOG64_HIGHWORD(clmEngineCtxt.startSysTick),
                    clmEngineCtxt.calc.curTimerPeriod );
}


/** 
 * <!-- CLM_EngineRegisterClient -->
 *  
 *  @brief Registers a CLM_Client with the Engine
 *
 *  @param newClient : Client to register
 */
void CLM_EngineRegisterClient( CLM_Client *newClient )
{
  CORE_VERIFY_PTR( newClient );

  Core_MutexLock( clmEngineCtxt.lock );

  if ( clmEngineCtxt.clientList == NULL )
  {
    clmEngineCtxt.clientList = newClient;
  }
  else
  {
    CLM_Client *client = clmEngineCtxt.clientList;

    while ( client->next )
    {
      client = client->next;
    }
    client->next = newClient;
  }

  if ( clmEngineCtxt.timerStarted )
  {
    /* A timer is already running; we need to align this client's window
       with the next timer expiry */

    /* Add remaining time on the timer to the pendingTimeUs on the client */
    newClient->pendingTimeUs = 
      newClient->measPeriodUs + timer_get( &clmEngineCtxt.timer, T_USEC );
  }
  else
  {
    newClient->pendingTimeUs = newClient->measPeriodUs;

    npa_resource_available_cb( "/clm/enabled", CLM_EngineStart, NULL );
  }

  Core_MutexUnlock( clmEngineCtxt.lock );
}

/** 
 * <!--  CLM_EngineUnregisterClient -->
 * 
 * @brief Unregisters the given client from the Engine
 * @param delClient : Client to unregister
 */
void CLM_EngineUnregisterClient( CLM_Client *delClient )
{
  int found = FALSE;
  CLM_Client *client;

  CORE_VERIFY_PTR( delClient );

  Core_MutexLock( clmEngineCtxt.lock );

  client = clmEngineCtxt.clientList;
  CORE_VERIFY_PTR( client );

  if ( client == delClient )
  {
    /* Unregister client at head of list */
    clmEngineCtxt.clientList = client->next;
    found = TRUE;
  }
  else
  {
    while ( client->next )
    {
      if ( client->next == delClient )
      {
        client->next = client->next->next;
        found = TRUE;
      }
      client = client->next;
    }
  }

  CORE_VERIFY( found );

  ULOG_RT_PRINTF_1( clmLog, "Client \"%s\" unregistered", delClient->name );

  /* Free up the memory */
  Core_Free( delClient );

  if ( clmEngineCtxt.clientList == NULL )
  {
    /* Stop the active timer */
    timer_clr( &clmEngineCtxt.timer, T_USEC );
    clmEngineCtxt.timerStarted = 0;
  }

  Core_MutexUnlock( clmEngineCtxt.lock );
}


/** 
 * <!-- CLM_EngineTimerCallback -->
 * 
 * @brief CLM's timer callback
 */
void CLM_EngineTimerCallback( timer_cb_data_type data )
{
  int i;
  uint32 *prevCyclesNThreadsBusy = clmEngineCtxt.calc.prevCyclesNThreadsBusy;

  /* Gather data, do some of the arithmetic */
  CLM_EngineMeasure();
  
  /* Call into the client layer */
  clmEngineCtxt.clientCb( &clmEngineCtxt.calc );
  
  Core_MutexLock( clmEngineCtxt.lock );

  /* Compute next timer period */
  clmEngineCtxt.calc.curTimerPeriod = 
    CLM_EngineFindNextTimerPeriod( clmEngineCtxt.calc.timeElapsedUs );

  /* Start the timer */
  CLM_EngineStartTimer( clmEngineCtxt.calc.curTimerPeriod );
  
  clmEngineCtxt.timerStarted = 1;

  /* Mark the start of this timer window */
  clmEngineCtxt.startSysTick = qurt_sysclock_get_hw_ticks();
  
  /* Reset some of the data */
  clmEngineCtxt.calc.sumBudgetCycles = 0;
  clmEngineCtxt.calc.timeElapsedUs = 0;
  clmEngineCtxt.calc.busyCycles = 0;

  /* TODO: Moved from EngineMeasure, but is the below neccessary? */
  for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
  {
    clmEngineCtxt.calc.cyclesNThreadsBusy[i] = 0;
  }
  
  ULOG_RT_PRINTF_3( clmLog, "Timer Set (startSysTick: 0x%llx) (periodUs: %d)",
                    ULOG64_LOWWORD(clmEngineCtxt.startSysTick),
                    ULOG64_HIGHWORD(clmEngineCtxt.startSysTick),
                    clmEngineCtxt.calc.curTimerPeriod );

  /* If PMU is available, gather data to compare against at timer expiry */
  if ( !clmPmuBusy )
  {
    INTLOCK();
    for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
    {
      prevCyclesNThreadsBusy[i] = qurt_pmu_get( i );
    }
    INTFREE();

    if ( clmEngineCtxt.nHWThreads == 4 )
    {
      ULOG_RT_PRINTF_4( clmLog,
        "\t(0x3b: %u) (0x3c: %u) (0x3d: %u) (0x3e: %u)",
        prevCyclesNThreadsBusy[0], prevCyclesNThreadsBusy[1], 
        prevCyclesNThreadsBusy[2], prevCyclesNThreadsBusy[3] );
    }
    else
    {
      ULOG_RT_PRINTF_3( clmLog, "\t(0x3b: %u) (0x3c: %u) (0x3d: %u)",
                        prevCyclesNThreadsBusy[0], prevCyclesNThreadsBusy[1],
                        prevCyclesNThreadsBusy[2] );
    }
  }
  else
  {
    ULOG_RT_PRINTF_0( clmLog, "\t(clmPmuBusy: 1)" );
    /* Flag that PMU data cannot be used at timer expiry */
    prevCyclesNThreadsBusy[0] = 0xFFFFFFFF;
  }
  
  if ( !CLM_EngineIsClockGatingDisabled() )
  {
    uint64 idleCyclesQurt[clmEngineCtxt.nHWThreads];
    
    /* Gather Idle Cycles to compare against (for threshold based updates) at
       timer expiry */
    qurt_profile_get_idle_pcycles( idleCyclesQurt );
    for ( i = 0; i < clmEngineCtxt.nHWThreads; i++ )
    {
      clmEngineCtxt.calc.prevIdleCyclesQurt[i] = idleCyclesQurt[i];
    }

    /* Gather Busy Cycles to compare against (if PMU is unavailable) at
       timer expiry */
    clmEngineCtxt.calc.prevBusyCyclesQurt = qurt_get_core_pcycles();
  }
  else
  {
    /* TODO: Do something about prevIdleCyclesQurt here as well? */
    clmEngineCtxt.calc.prevBusyCyclesQurt = 0xFFFFFFFFFFFFFFFF;
  }
  
  ULOG_RT_PRINTF_2( clmLog, "\t(prevBusyCyclesQurt: %llu)",
    ULOG64_LOWWORD(clmEngineCtxt.calc.prevBusyCyclesQurt),
    ULOG64_HIGHWORD(clmEngineCtxt.calc.prevBusyCyclesQurt) );
  
  Core_MutexUnlock( clmEngineCtxt.lock );
}


/** 
 * <!-- CLM_EngineInit -->
 *  
 * @brief Callback function triggered on availability of dependencies.
 * We initialize CLM here.
 */
void CLM_EngineInit( void *context, unsigned int event_type, 
                     void *data, unsigned int data_size )
{
  qurt_sysenv_max_hthreads_t hwtQurt;
  uint32 nHWThreads;
  void *p;
  uint32 size;

  Core_MutexLock( clmEngineCtxt.lock );

  /* Query the number of HW threads on this target */
  qurt_sysenv_get_max_hw_threads( &hwtQurt );
  nHWThreads = clmEngineCtxt.nHWThreads = hwtQurt.max_hthreads;

  if ( clmEngineCtxt.nHWThreads == 3 )
  {
    clmPmuEvtConfig = CLM_PMU_EVT_CONFIG_3HWT;
  }
  else
  {
    clmPmuEvtConfig = CLM_PMU_EVT_CONFIG_4HWT;
  }

  CLM_EnginePMUInit();

  /* Create a query handle to /clk/cpu */
  CORE_VERIFY_PTR( 
    clmEngineCtxt.clkQuery = npa_create_query_handle( "/clk/cpu" ) );

  clmEngineCtxt.calc.maxClkKhz = CLM_EngineGetMaxFreqKhz();
  clmEngineCtxt.calc.curClkKhz = CLM_EngineGetClkState();

  /* Allocate memory */
  size = nHWThreads * 
         ( sizeof(uint64) + sizeof(uint64) + sizeof(uint32) + sizeof(uint32) );
  p = Core_Malloc( size );
  memset( p, 0, size );

  clmEngineCtxt.calc.idleCyclesQurt = p;
  clmEngineCtxt.calc.prevIdleCyclesQurt = 
    clmEngineCtxt.calc.idleCyclesQurt + nHWThreads;
  clmEngineCtxt.calc.cyclesNThreadsBusy = 
    (uint32 *)(clmEngineCtxt.calc.prevIdleCyclesQurt + nHWThreads);
  clmEngineCtxt.calc.prevCyclesNThreadsBusy =
    clmEngineCtxt.calc.cyclesNThreadsBusy + nHWThreads;

  /* Create the timer */
  timer_def( &clmEngineCtxt.timer, &clmEngineCtxt.timerGroup,
             NULL, 0, CLM_EngineTimerCallback, 0 );

  /* Set the timer as deferrable */
  timer_group_set_deferrable( &clmEngineCtxt.timerGroup, TRUE );
  
  /* Register for the clk change events */
  CORE_VERIFY_PTR( clmEngineCtxt.postClkChangeEvent =
    npa_create_custom_event( "/clk/cpu", "CLM_ClkPostChangeEvent",
                             NPA_TRIGGER_POST_CHANGE_EVENT,
                             NULL, CLM_EnginePostClkChangeCallback, NULL ) );
                             
  CORE_VERIFY_PTR( clmEngineCtxt.preClkChangeEvent =
    npa_create_custom_event( "/clk/cpu", "CLM_ClkPreChangeEvent",
                             NPA_TRIGGER_PRE_CHANGE_EVENT,
                             NULL, CLM_EnginePreClkChangeCallback, NULL ) );

  Core_MutexUnlock( clmEngineCtxt.lock );

  npa_define_marker( "/clm/enabled" );
}

/** 
 * <!--  CLM_EngineCreate -->
 *  
 * @brief Set up callbacks to complete init, after dependencies are available.
 *
 * @param clientCb : Function that is to be the interface between the
 * engine and client layers
 */
void CLM_EngineCreate( CLM_ClientCB clientCb )
{
  static const char *basicDeps[] =
  {
    "/clk/cpu",
    "/init/timer"
  };

  clmEngineCtxt.clientCb = clientCb;

  CORE_VERIFY_PTR( 
    clmEngineCtxt.lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );

  /* Register to be notified when basic resource dependencies are available */
  npa_resources_available_cb( NPA_ARRAY( basicDeps ), 
                              CLM_EngineInit, NULL );

  /* Register to be notified when the sleep LPR node is available */
  npa_resource_available_cb( SLEEP_LPR_NODE_NAME, CLM_RegisterLPR, NULL );
}


/**
 * <!-- CLM_EngineGetClients -->
 * @return Pointer to the list of clients
 */
CLM_Client* CLM_EngineGetClients( void )
{
  return clmEngineCtxt.clientList;
}

/**
 * <!-- CLM_EngineGetNumHWThreads -->
 * @return The number of HW threads on the target
 */
uint32 CLM_EngineGetNumHWThreads( void )
{
  return clmEngineCtxt.nHWThreads;
}

/**
 * <!-- CLM_EngineGetLastLPRExitTime -->
 * @brief Returns the time of the last LPR exit 
 *
 * @return Timetick of last PC exit
 */
uint64 CLM_EngineGetLastLPRExitTime( void )
{
  return clmEngineCtxt.calc.lastLPRexitTS;
}
