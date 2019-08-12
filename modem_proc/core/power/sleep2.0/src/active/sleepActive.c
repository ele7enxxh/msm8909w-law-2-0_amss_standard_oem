/*==============================================================================
  FILE:         sleepActive.c
  
  OVERVIEW:     This file implements the Active Time solver thread and the 
                interfaces associated with it.
  
  DEPENDENCIES: None

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI).
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
=================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/active/sleepActive.c#1 $
$DateTime: 2016/12/13 07:59:23 $
===============================================================================*/
#include "stdint.h"
#include "npa.h"
#include "rcinit.h"
#include "sleepActive.h"
#include "sleep_log.h"
#include "sleep_solver.h"
#include "qurt_signal.h"
#include "sleep_osi.h"
#include "CoreMutex.h"
#include "CoreVerify.h"
#include "sleep_osi.h"

/*==============================================================================
                                 CONSTANTS
  =============================================================================*/
/* Active Time Solver depends on being able to query 
 * following nodes for evaluating low-power modes */
static const char* sleep_active_deps[] = 
{
  "/sleep/idle/plugin", 
  "/sleep/max_duration", 
  "/core/cpu/latency"
};

/*==============================================================================
                              INTERNAL DEFINITIONS
  =============================================================================*/
/* Signal Object used for communicating with Active-time solver */
static qurt_signal_t sleepActiveSigObj;

/* State of Solver Inputs that are handled by idle plugin */
static sleep_solver_input solverInput;

/* Query handle for the current state of idle/plugin node */
static npa_query_handle npaIdlePluginHandle;

/* Query handle for the current state of max_duation node */
static npa_query_handle npaMaxDurationHandle;

/* Query handle for the current state of latency node */
static npa_query_handle npaLatencyHandle;

/*==============================================================================
                           STATIC FUNCTION DEFINITIONS
  =============================================================================*/
/**
 * @brief Execute the Active Time solver and determine whether mode selection
 *        has changed
 *
 *        Performs the following operations:
 *        1. Execute solver based on last updated solver input
 *        2. If mode selection results in Cached Mode then
 *           unconfigure previous configured mode ( only if cached )
 *           and configure the newly selected cached mode
 *        3. If mode selection results in Uncached Mode then 
 *           unconfigure previous configured mode ( only if cached )
 *        4. Calls sleepOS_ConfigIdleMode to set the appropriate entry 
 *           depending on whether the selected mode is cached or uncached
 * 
 */
static void sleepActive_TriggerSolver( void )
{
  /* Last synthLPRM that was selected by idle plugin solver invocation */ 
  static sleep_synth_lprm* lastSynthMode;
  sleep_solver_output solver_output;
  uint32 curCpuFreqKhz;

  curCpuFreqKhz = sleepOS_getFrequency();

  if( TRUE != sleepStats_getTableEntry( curCpuFreqKhz, &solverInput.fLUT ) )
  {
    /* If the function returns !TRUE, we need to allocate a new fLUT and 
     * realloc all of the cache & statistic arrays */
    solverInput.fLUT =
      sleepStats_addFrequencyTable( solverInput.fLUT, curCpuFreqKhz );
  }

  sleepSolver_solveMode( &solverInput, &solver_output );

  /* Active-Time solver should always be able to choose a cached mode */
  CORE_VERIFY_PTR( solver_output.selected_synthLPRM );

  /* If Mode selection did not change, then return */
  if ( lastSynthMode == solver_output.selected_synthLPRM )
  {
    return;
  }

  /* Transition across Synth Modes if solver output has changed */
  if ( lastSynthMode != NULL && synthLPRM_isCacheable( lastSynthMode ) )
  {
    /* Unconfigure previously configured Cached Synth Mode */
    synthLPRM_exit( lastSynthMode , solverInput.fLUT );
  }
  
  if ( synthLPRM_isCacheable( solver_output.selected_synthLPRM ) )
  {
    /* Configure the selected Cacheable mode */
    synthLPRM_enter( solver_output.selected_synthLPRM,
                     UINT64_MAX, solverInput.fLUT );

    /* Configure OS Idle Mode for Cached Synth Mode  */
    sleepOS_configIdleMode( 0, SLEEP_OS_IDLE_MODE_OVERRIDE );
  }
  else
  {
    /* Configure OS Idle Mode for Uncached Synth Mode */
    sleepOS_configIdleMode( 0, SLEEP_OS_IDLE_MODE_DEFAULT );
  }

  /* Update the last selected Synth LPRM */
  lastSynthMode = solver_output.selected_synthLPRM;
}

/**
 * @brief Clear a signal or mask of signals on Active Time solver thread
 *
 * Signals set on the Active Time solver are used to determine 
 * execution conditions that have changed. Signals should be cleared
 * as soon they are processed.
 * 
 * @param sig: signal or signal mask to be cleared on Active Time solver
 */
static void sleepActive_ClearSignal( uint32 sig )
{
  qurt_signal_clear( &sleepActiveSigObj, sig );
}


/**
 * @brief Updates solver input based on set signals
 *
 * @param signals : Data signals that are set on Active Time Solver
 */
static void sleepActive_ProcessDataSignals( uint32 signals )
{
  npa_query_type  qres;

  /* Clear Data Signals, if any */
  sleepActive_ClearSignal( signals & ~SLEEP_SIGNAL_CONTROL );

  if ( signals & SLEEP_SIGNAL_LATENCY && npaLatencyHandle )
  {
    /* Get the latency budget from the latency node.  The latency node will
     * return the minimum latency budget being requested by all clients.
     * This lets us know how quickly we need to respond to an interrupt when
     * it occurs. */
    CORE_VERIFY( NPA_QUERY_SUCCESS == 
                 npa_query( npaLatencyHandle, NPA_QUERY_CURRENT_STATE,
                            &qres ) );
    solverInput.latency_budget = qres.data.value;
  }

  if ( signals & SLEEP_SIGNAL_SOFT_DURATION && npaMaxDurationHandle )
  {
    /* Get the sleep duration from the max duration node.  This will return the
     * "soft" max duration.  Soft wakeups are those that are hinted at by other
     * subsystems.  Soft wakeups allow us to calculate which mode we will enter,
     * so we don't waste time entering a mode that we are going to have to wake
     * up from soon anyway. */
    CORE_VERIFY( NPA_QUERY_SUCCESS == 
                 npa_query( npaMaxDurationHandle, NPA_QUERY_CURRENT_STATE,
                            &qres ) );
    solverInput.soft_duration =  qres.data.value;
  }

  if ( signals & SLEEP_SIGNAL_CPU_FREQUENCY )
  {
    /* TODO: Move CPU Frequency query here,
     * once cpu frequency change events are enabled */
  }

}

/**
 * @brief Updates solver signal mask based on set signals
 *
 * @param signals : Control signals that are set on Active Time Solver
 *
 * @return boolean denoting whether further processing should continue
 */
static boolean sleepActive_ProcessControlSignals( uint32 signals )
{
  boolean stopProcessing = 0;

  /* Process updated Control signals, if any */
  if ( signals & SLEEP_SIGNAL_CONTROL )
  {
    sleepOS_IdleModeType idleMode;
    npa_query_type  qres;

    /* Clear Control Signal */
    sleepActive_ClearSignal( SLEEP_SIGNAL_CONTROL );

    if ( npaIdlePluginHandle )
    {
      CORE_VERIFY( NPA_QUERY_SUCCESS == 
                   npa_query( npaIdlePluginHandle, NPA_QUERY_CURRENT_STATE,
                              &qres ) );
    
      idleMode = qres.data.value;
    }
    else
    {
      /* npaIdlePluginHandle is not initialized, HOLD_OFF until
       * handle becomes available */
      idleMode = SLEEP_OS_IDLE_MODE_HOLD_OFF;
    }

    /* If HOLD_OFF or UIMAGE are requested in idle/plugin then Active
     * Time solver should configure for these modes
     * and suspend further processing */
    if ( idleMode == SLEEP_OS_IDLE_MODE_HOLD_OFF ||
         idleMode == SLEEP_OS_IDLE_MODE_USLEEP )
    {
      /* Configure sleepOS appropriately */
      sleepOS_configIdleMode( 0, idleMode );
      
      stopProcessing = 1;
    }
    else
    {
      /* Since idle/plugin is not forcing a constraint, continue 
       * processing the data signals */
      stopProcessing = 0;
    }
  }

  return stopProcessing;
}

/**
 * @brief Returns any pending signals if Active Time solver is enabled
 *
 * sig_mask : Mask used to control whether Active Time solver is
 *            is listening to only:
 *            i. the control signal (SLEEP_SIGNAL_CONTROL)
 *            ii. all signals (SLEEP_SIGNAL_MASK_ALL)
 */
static uint32 sleepActive_WaitforSignal( uint32 sig_mask )
{
  uint32 sigs;
  
  sigs = qurt_signal_wait( &sleepActiveSigObj, sig_mask, QURT_SIGNAL_ATTR_WAIT_ANY );
  
  return sigs;
}

/**
 * @brief Called when dependencies of Active Time Solver become available 
 */
static void sleepActive_ResourceAvailCB( void* context, 
                                         unsigned int event, 
                                         void* resources,
                                         unsigned int resourcesSize )
{
  /* Initialize Query Handle */
  CORE_VERIFY_PTR( npaIdlePluginHandle =
                   npa_create_query_handle( "/sleep/idle/plugin" ) );

  /* Initialize Query Handle and Perform Query */
  CORE_VERIFY_PTR( npaMaxDurationHandle =
                   npa_create_query_handle( "/sleep/max_duration" ) );

  /* Initialize Query Handle and Perform Query */
  CORE_VERIFY_PTR( npaLatencyHandle =
                   npa_create_query_handle( "/core/cpu/latency" ) );

  /* Trigger the inputs corresponding to the available nodes */
  sleepActive_SetSignal( SLEEP_SIGNAL_CONTROL |
                         SLEEP_SIGNAL_SOFT_DURATION |
                         SLEEP_SIGNAL_LATENCY );
}

/*==============================================================================
                            GLOBAL FUNCTION DEFINITIONS
  =============================================================================*/
/**
 * @brief Required to be called as part of rcinit.
 *
 * Initializes the Active Time Solver 
 */
void sleepActive_Init( void )
{
  /* Initialize signal type */
  qurt_signal_init( &sleepActiveSigObj );

  /* Register to Callback when resources becomes available */
  npa_resources_available_cb( NPA_ARRAY(sleep_active_deps),
                              sleepActive_ResourceAvailCB,
                              NULL );

  /* Setup Solver Input Defaults */
  solverInput.fLUT = 0;
  solverInput.latency_budget = 1;
  solverInput.soft_duration = UINT64_MAX;
  solverInput.hard_duration = UINT64_MAX;
  solverInput.synth_attr_mask = SYNTH_MODE_ATTR_LPR_MASK_ANY; 
}

/**
 * @brief Trigger a signal or signal mask on Active Time solver thread
 *
 * Signals set on the Active Time solver are used to determine 
 * execution conditions that have changed. The Active Time thread
 * executes the sleep solver to determine whether these modified
 * inputs lead to a change in the selection of low-power mode.
 * 
 * @param sig: signal or signal mask to be set on Active Time solver
 */
void sleepActive_SetSignal( uint32 sig )
{
  qurt_signal_set( &sleepActiveSigObj, sig );
}

/**
 * @brief Processing loop for Active Time Solver.
 *
 * Executes from the "sleepsolver" thread context
 */
void sleepActive_Thread( unsigned long int unused_param )
{
  uint32 sig_mask = SLEEP_SIGNAL_CONTROL;
  uint32 signals = 0;
  boolean disableActiveTimeSolver;

  /* Initialize the Active-time Solver */
  sleepActive_Init( );

  /* Notify RCInit that all basic initialization is complete. */
  rcinit_handshake_startup();
  
  while(1)
  {
    signals = sleepActive_WaitforSignal( sig_mask );

    sleepLog_printf( SLEEP_LOG_LEVEL_REQUESTS, 1, 
                     "Active Solver Thread: (signals: 0x%x)",
                     signals );
    
    /*Process and Clear Control signals, if any */
    disableActiveTimeSolver = sleepActive_ProcessControlSignals( signals );

    /* Skip further processing of Data Signals if Control Signals
     * cause Active Time solver to be disabled.
     * Additionally, modify mask to only listed for Control Signal
     * changes until Active Timer solver is enabled again. */
    if ( disableActiveTimeSolver == TRUE )
    {
      sig_mask = SLEEP_SIGNAL_CONTROL;
      continue;
    }
    else
    {
      sig_mask = SLEEP_SIGNAL_MASK_ALL;
    }

    /*Process updated Input signals, if any */
    sleepActive_ProcessDataSignals( signals );
    
    /* Trigger solver with updated input. Solver will 
     * configure the appropriate sleepOS_IdleMode if required */
    sleepActive_TriggerSolver( );

  } /* while */
    
}
