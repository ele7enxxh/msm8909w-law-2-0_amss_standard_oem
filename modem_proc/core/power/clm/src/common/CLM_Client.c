/*=============================================================================
FILE:         CLM_Client.c

DESCRIPTION:  Implementation of CLM Client layer of the CPU Load Monitor. It 
implements the APIs defined in CLM.h
 
 
 
=========================================================================== 
Copyright © 2013-2014 QUALCOMM Technologies, Incorporated. All Rights Reserved. 
QUALCOMM Confidential and Proprietary. 
 
===========================================================================

                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/clm/src/common/CLM_Client.c#1 $

=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "CLM.h"
#include "CLM_Engine.h"
#include "CoreVerify.h"
#include "CoreMutex.h"
#include "ULogFront.h"

/* Defined here; for use by both this layer and the engine */
ULogHandle clmLog;

/** 
 * <!-- CLM_UpdateClient --> 
 * 
 * @brief Populates the client's loadInfo structure from engineCalc and
 * invokes the client's registered callback with this info.
 * 
 * @param client : pointer to the client
 * @param engineCalc : Load data from Engine
 * @param updateReason : One of CLM_UpdateReasonEnumType
 * 
 * @return None 
 */
static void CLM_UpdateClient( CLM_Client *client,
                              CLM_EngineCalculation *engineCalc,
                              CLM_UpdateReasonEnumType updateReason )
{
  uint32 i;
  uint32 nThreads = CLM_EngineGetNumHWThreads();

  if ( client->type == CLM_CLIENT_BASIC_CPUUTIL )
  {
    CLM_LoadInfoBasicStructType *loadInfo = client->loadInfo;

    loadInfo->updateReason = updateReason;
    loadInfo->currentClkKhz = engineCalc->curClkKhz;

    loadInfo->utilPctAtCurrClk = client->busyPctCurClk;
    loadInfo->utilPctAtMaxClk = client->busyPctMaxClk;
    loadInfo->timeElapsedUs = client->timeElapsedUs;
    
    ULOG_RT_PRINTF_3( clmLog, 
      "\tClient Update (utilPctAtCurrClk: %d) (utilPctAtMaxClk: %d) "
      "(updateReason: %d)",
      loadInfo->utilPctAtCurrClk, loadInfo->utilPctAtMaxClk,
      loadInfo->updateReason );
  }
  else if ( client->type == CLM_CLIENT_EXTENDED_CPUUTIL )
  {
    CLM_LoadInfoExtendedStructType *loadInfo = client->loadInfo;

    loadInfo->updateReason = updateReason;
    loadInfo->currentClkKhz = engineCalc->curClkKhz;
    loadInfo->utilPctAtCurrClk = client->busyPctCurClk;
    loadInfo->utilPctAtMaxClk = client->busyPctMaxClk;
    loadInfo->timeElapsedUs = client->timeElapsedUs;
    loadInfo->budgetCycles = client->budgetCycles;
    loadInfo->busyCycles = client->busyCycles;
    loadInfo->measPeriodUs = client->measPeriodUs;

    for ( i = 0; i < nThreads; i++ )
    {
      loadInfo->cyclesNThreadsBusy[i] = client->cyclesNThreadsBusy[i];
    }
    
    ULOG_RT_PRINTF_5( clmLog, 
      "\tClient Update (utilPctAtCurrClk: %u) (utilPctAtMaxClk: %u) "
      "(updateReason: %u) (curFreqKhz: %u) (maxFreqKhz: %u)",
      loadInfo->utilPctAtCurrClk, loadInfo->utilPctAtMaxClk,
      loadInfo->updateReason, engineCalc->curClkKhz, engineCalc->maxClkKhz );
  }
  else if ( client->type == CLM_CLIENT_PMU_INFO )
  {
    CLM_LoadInfoPMUInfoStructType *loadInfo = client->loadInfo;

    loadInfo->timeElapsedUs = client->timeElapsedUs;
    loadInfo->pmuDisabled = engineCalc->pmuDisabled;
    loadInfo->updateReason = updateReason;

    /* If PMU was/is disabled, clear data as it is invalid */
    if ( engineCalc->pmuDisabled )
    {
      for ( i = 0; i < nThreads; i++ )
      {
        loadInfo->cyclesNThreadsBusy[i] = 0;
      }
    }
    else
    {
      for ( i = 0; i < nThreads; i++ )
      {
        loadInfo->cyclesNThreadsBusy[i] = client->cyclesNThreadsBusy[i];
      }
    }

    ULOG_RT_PRINTF_5( clmLog, "\tClient Update (pmuDisabled: %d) "
      "(0x3b: %u) (0x3c: %u) (0x3d: %u) (0x3e/0x03: %u)",
      loadInfo->pmuDisabled,
      loadInfo->cyclesNThreadsBusy[0], loadInfo->cyclesNThreadsBusy[1],
      loadInfo->cyclesNThreadsBusy[2], loadInfo->cyclesNThreadsBusy[3] );
  }

  client->cbFunc( client->loadInfo, client->cbData );
}

/** 
 * <!-- CLM_CheckBusyHighThreshold --> 
 *  
 * @brief Checks if utilization of atleast nThreadsForHigh HWTs
 * is above the client-specified highThreshold for the given window
 *
 * @param client : The client
 * @param engineCalc : Load data from Engine
 * 
 * @return TRUE, if utilization of atleast nThreadsForHigh HWTs
 * is above highThreshold
 */
static uint32 CLM_CheckBusyHighThreshold( CLM_Client *client,
                                          CLM_EngineCalculation *engineCalc )
{
  int i;
  uint64 busyCyclesThread;
  uint32 busyPctThread;
  uint32 nThreadsBusyHigh = 0;
  uint32 nHWThreads = CLM_EngineGetNumHWThreads();

  for ( i = 0; i < nHWThreads; i++ )
  {
    if ( client->budgetCycles / nHWThreads > client->idleCycles[i] )
    {
      busyCyclesThread = 
        ( client->budgetCycles / nHWThreads ) - client->idleCycles[i];
    }
    else
    {
      busyCyclesThread = 0;
    }

    busyPctThread = busyCyclesThread * nHWThreads * 100 / client->budgetCycles;

    /* Scale to max clock */
    busyPctThread = 
      busyPctThread * engineCalc->curClkKhz / engineCalc->maxClkKhz;

    if ( busyPctThread > client->highThresholdPct )
    {
      nThreadsBusyHigh++;
    }
  }

  return ( nThreadsBusyHigh >= client->nThreadsForHigh ) ? TRUE : FALSE;
}

/** 
 * <!-- CLM_CheckBusyLowThreshold -->
 * 
 * @brief Checks if utilization of atleast nThreadsForLow HWTs
 * is below the client-specified lowThreshold for the given window
 *
 * @param client : The client
 * @param engineCalc : Load data from Engine
 *
 * @return TRUE, if CPU Utilization of atleast nThreadsForLow HWTs
 * is below lowThreshold 
 */
static uint32 CLM_CheckBusyLowThreshold( CLM_Client *client,
                                         CLM_EngineCalculation *engineCalc )
{
  int i;
  uint64 busyCyclesThread;
  uint32 busyPctThread;
  uint32 nThreadsBusyLow = 0;
  uint32 nHWThreads = CLM_EngineGetNumHWThreads();

  for ( i = 0; i < nHWThreads; i++ )
  {
    if ( client->budgetCycles / nHWThreads > client->idleCycles[i] )
    {
      busyCyclesThread = 
        ( client->budgetCycles / nHWThreads ) - client->idleCycles[i];
    }
    else
    {
      busyCyclesThread = 0;
    }

    busyPctThread = busyCyclesThread * nHWThreads * 100 / client->budgetCycles;

    /* Scale to max clock */
    busyPctThread = 
      busyPctThread * engineCalc->curClkKhz / engineCalc->maxClkKhz;

    if ( busyPctThread <= client->lowThresholdPct )
    {
      nThreadsBusyLow++;
    }
  }

  return ( nThreadsBusyLow >= client->nThreadsForLow ) ? TRUE : FALSE;
}

/** 
 * <!-- CLM_AdjustForParallelism -->
 *  
 * @brief Adjusts busyCycles count by accounting for parallelism
 * on a multi-HWT system.
 *
 * @param client : The client
 */
static CLM_INLINE void CLM_AdjustForParallelism( CLM_Client *client )
{
  /* adjustedBusyCycles = busyCycles – 2/3 * 0x3b – 1/3 * 0x3c */
  uint64 adjustedBusyCycles = client->busyCycles - 
                              ( 2 * client->cyclesNThreadsBusy[0] / 3 ) - 
                              ( client->cyclesNThreadsBusy[1] / 3 );

#ifdef CLM_EXTRA_LOGS
  ULOG_RT_PRINTF_6( 
    clmLog, "\tAdjustForParallelism (busyCycles: %llu) "
    "(cycles1hwtBusy: %d) (cycles2hwtBusy: %d) (adjustedBusyCycles: %llu)",
    ULOG64_LOWWORD(client->busyCycles), ULOG64_HIGHWORD(client->busyCycles),
    client->cyclesNThreadsBusy[0], client->cyclesNThreadsBusy[1],
    ULOG64_LOWWORD(adjustedBusyCycles), ULOG64_HIGHWORD(adjustedBusyCycles) );
#endif

  client->busyCycles = adjustedBusyCycles;
}

/** 
 * <!-- CLM_ComputeUtil -->
 *  
 * @brief Computes utilization based on data in client window
 *
 * @param client : The client
 * @param engineCalc : Load data from Engine
 */
static CLM_INLINE void CLM_ComputeUtil( CLM_Client *client,
                                        CLM_EngineCalculation *engineCalc )
{
  client->busyPctCurClk = client->busyCycles * 100 / client->budgetCycles;

  if ( client->busyPctCurClk > 100 )
  {
    ULOG_RT_PRINTF_1( clmLog, 
    "\tWARN: CLM computes busyPctCurClk at %d; clip to 99",
    client->busyPctCurClk );
    client->busyPctCurClk = 99;
  }

  client->busyPctOneThread = 
    (uint64)client->cyclesNThreadsBusy[0] * 100 / client->budgetCycles;

  if ( client->busyPctOneThread > 100 )
  {
    ULOG_RT_PRINTF_1( clmLog,
    "\tWARN: CLM computes busyPctOneThread at %d; clip to 99",
    client->busyPctOneThread );
    client->busyPctOneThread = 99;
  }

  client->busyPctMaxClk = 
    client->busyPctCurClk * engineCalc->curClkKhz / engineCalc->maxClkKhz;

#ifdef CLM_EXTRA_LOGS
  ULOG_RT_PRINTF_8( clmLog, 
    "\tCompute Util (busyCycles: %llu) (budgetCycles : %llu) "
    "(cycles1hwtBusy : %d) (busyPctCurClk: %d) (busyPctMaxClk: %d) "
    "(busyPctOneThread: %d)",
    ULOG64_LOWWORD(client->busyCycles), ULOG64_HIGHWORD(client->busyCycles), 
    ULOG64_LOWWORD(client->budgetCycles), 
    ULOG64_HIGHWORD(client->budgetCycles),
    client->cyclesNThreadsBusy[0], client->busyPctCurClk, 
    client->busyPctMaxClk, client->busyPctOneThread );
#endif
}


/** 
 * <!-- CLM_ResetClientData -->
 *  
 * @brief Resets client measurement data in prep for next window.
 *
 * @param client : The client
 */
static void CLM_ResetClientData( CLM_Client *client )
{
  int i;
  uint32 nThreads = CLM_EngineGetNumHWThreads();

  client->budgetCycles = 0;
  client->busyCycles = 0;
  client->timeElapsedUs = 0;
  client->pendingTimeUs = client->measPeriodUs;

  for ( i = 0; i < nThreads; i++ )
  {
    client->idleCycles[i] = 0;
    client->cyclesNThreadsBusy[i] = 0;
  }
}


/** 
 * <!-- CLM_UpdateClientData -->
 *  
 * @brief Updates client-specific load data based on engine measurements
 * at every timer expiry.
 *
 * @param client : The client
 * @param engineCalc : Load data from Engine
 */
void CLM_UpdateClientData( CLM_Client *client,
                           CLM_EngineCalculation *engineCalc )

{
  int i;
  uint32 nHWThreads = CLM_EngineGetNumHWThreads();

  client->budgetCycles += engineCalc->sumBudgetCycles;
  client->busyCycles += engineCalc->busyCycles;
  client->timeElapsedUs += engineCalc->timeElapsedUs;

  for ( i = 0; i < nHWThreads; i++ )
  {
    client->idleCycles[i] = engineCalc->idleCyclesQurt[i];
    client->cyclesNThreadsBusy[i] = engineCalc->cyclesNThreadsBusy[i];
  }
}


/** 
 * <!-- CLM_UpdatePeriodicClient -->
 *  
 * @brief Update periodic clients whose measurement windows have expired
 *
 * @param client : The client
 * @param engineCalc : Load data from Engine
 */
static void CLM_UpdatePeriodicClient( CLM_Client *client,
                                      CLM_EngineCalculation *engineCalc )
{
  if ( client->timeElapsedUs >= client->measPeriodUs - CLM_TIMER_TOLERANCE )
  {
    CLM_ComputeUtil( client, engineCalc );

    /* We apply a correction factor to Util if the following are TRUE:
     * 1) The PMU was the source of measurement data
     * 2) The overall Busy/Util percentage is <= CPU_MAXED_OUT_PCT
     * 3) The 1HWT Busy/Util percentage is <= SINGLE_THREAD_MAXED_OUT_PCT
     */
    if ( !engineCalc->pmuDisabled &&
         ( client->busyPctCurClk <= CPU_MAXED_OUT_PCT ||
           client->busyPctOneThread <= ONE_THREAD_MAXED_OUT_PCT ) )
    {
      CLM_AdjustForParallelism( client );
      CLM_ComputeUtil( client, engineCalc );
    }
    else
    {
      ULOG_RT_PRINTF_3( clmLog, "\tUtilization NOT adjusted "
                                "(pmuDisabled: %d) (busyPctCurClk: %d) "
                                "(busyPctOneThread: %d)",
                                engineCalc->pmuDisabled, client->busyPctCurClk, 
                                client->busyPctOneThread );
    }

    CLM_UpdateClient( client, engineCalc, CLM_UPDATE_REASON_PERIODIC );
    CLM_ResetClientData( client );
  }
}

/** 
 * <!-- CLM_UpdateThresholdClient -->
 *  
 * @brief Updates threshold clients if utilization exceeds or falls below
 * given thresholds.
 *
 * @param client : The client
 * @param engineCalc : Load data from Engine
 */
static void CLM_UpdateThresholdClient( CLM_Client *client,
                                       CLM_EngineCalculation *engineCalc )
{
  if ( client->timeElapsedUs >= client->measPeriodUs - CLM_TIMER_TOLERANCE )
  {
    if ( CLM_CheckBusyHighThreshold( client, engineCalc ) )
    {
      CLM_ComputeUtil( client, engineCalc );

      /* Inform client only when state transitions from low or middle to high */
      if ( client->lastKnownThreshold == CLM_LOW_THRESHOLD_LEVEL ||
           client->lastKnownThreshold == CLM_UNKNOWN_THRESHOLD_LEVEL )
      {
        client->lastKnownThreshold = CLM_HIGH_THRESHOLD_LEVEL;
        CLM_UpdateClient( client, engineCalc, 
                          CLM_UPDATE_REASON_HIGH_THRESHOLD );
      }
    }
    else if ( CLM_CheckBusyLowThreshold( client, engineCalc ) )
    {
      CLM_ComputeUtil( client, engineCalc );

      /* Inform client only when state transitions from high or middle to low */
      if ( client->lastKnownThreshold == CLM_HIGH_THRESHOLD_LEVEL ||
           client->lastKnownThreshold == CLM_UNKNOWN_THRESHOLD_LEVEL )
      {
        client->lastKnownThreshold = CLM_LOW_THRESHOLD_LEVEL;
        CLM_UpdateClient( client, engineCalc, 
                          CLM_UPDATE_REASON_LOW_THRESHOLD );
      }
    }

    CLM_ResetClientData( client );
  }
}


/** 
 * <!--  CLM_EngineUpdate --> 
 *  
 * @brief Callback function invoked by the CLM Engine whenever the timer
 * expires. This routine updates load data for all clients and then
 * updates specific clients with this information, if their notification
 * conditions are true.
 *
 * @param engineCalc : Load data from Engine
 */
void CLM_EngineUpdate( CLM_EngineCalculation *engineCalc )
{
  CLM_Client *client = CLM_EngineGetClients();
  CORE_VERIFY_PTR( client );

  do
  {
    ULOG_RT_PRINTF_1( clmLog, "Engine Update (client: \"%s\")", client->name );
    CLM_UpdateClientData( client, engineCalc );

    if ( !client->lowThresholdPct && !client->highThresholdPct )
    {
      CLM_UpdatePeriodicClient( client, engineCalc );
    }
    else
    {
      CLM_UpdateThresholdClient( client, engineCalc );
    }
    client = client->next;
  } while ( client );
}

/**
 * <!-- CLM_Initialize -->
 * 
 * @brief Initialize CLM; this function is invoked by RC_INIT
 */
void CLM_Initialize( void )
{
  CORE_DAL_VERIFY( 
    ULogFront_RealTimeInit( &clmLog, "CLMLog", CLM_LOG_BUFSIZE,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS ) );

  /* Create an instance of CLM Engine, passing in the function
     that will be the interface between the 2 layers */
  CLM_EngineCreate( CLM_EngineUpdate );
}


/*----------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

/**
 * <!-- CLM_RegisterClient -->
 * 
 * @brief Code common to registering either periodic or threshold clients.
 */
static CLM_HandleType 
CLM_RegisterClient( const char            *clientName,
                    CLM_ClientOutputType   clientType, 
                    uint32                 measPeriodUs,
                    uint32                 attributes,
                    CLM_CallbackFuncPtr		cbFunc,
                    void                  *cbData )
{
  CLM_Client *newClient = NULL;
  uint32 size = 0;
  void *p;
  uint32 nHWThreads = CLM_EngineGetNumHWThreads();

  /* Verify parameters */
  CORE_VERIFY( clientName );
  CORE_VERIFY( strlen( clientName ) <= CLM_MAX_NAME_LENGTH );
  CORE_VERIFY( measPeriodUs != 0 );
  CORE_VERIFY_PTR( cbFunc );

  newClient = (CLM_Client *)Core_Malloc( sizeof(CLM_Client) );
  CORE_VERIFY_PTR( newClient );
  memset( newClient, 0, sizeof(CLM_Client) );

	/* Determine the right loadInfo struct to allocate */
	if ( clientType == CLM_CLIENT_BASIC_CPUUTIL ) 
	{
    size = sizeof(CLM_LoadInfoBasicStructType);
	}
	else if (clientType == CLM_CLIENT_EXTENDED_CPUUTIL)
	{
	  size = sizeof(CLM_LoadInfoExtendedStructType);
	}
  else if (clientType == CLM_CLIENT_PMU_INFO)
	{
	  size = sizeof(CLM_LoadInfoPMUInfoStructType);
	}
  else
  {
    /* err_fatal */
    CORE_VERIFY( 0 );
  }

  newClient->loadInfo = Core_Malloc( size );
  CORE_VERIFY_PTR( newClient->loadInfo );
  memset( newClient->loadInfo, 0, size );

  size = nHWThreads * ( sizeof(uint64) + sizeof(uint32) );
  p = Core_Malloc( size );
  CORE_VERIFY_PTR( p );
  memset( p, 0, size );

  newClient->idleCycles = p;
  newClient->cyclesNThreadsBusy = 
    (uint32 *)(newClient->idleCycles + nHWThreads );

  newClient->name = clientName;
  newClient->type = clientType;
  newClient->measPeriodUs = measPeriodUs;
  newClient->attributes = attributes;
  newClient->cbFunc = cbFunc;
  newClient->cbData = cbData;

  /* Initialize threshold to UNKNOWN, irrespective of type of client */
  newClient->lastKnownThreshold = CLM_UNKNOWN_THRESHOLD_LEVEL;

  return newClient;
}


/** 
 * <!-- CLM_RegisterPeriodicClient -->
 *  
 * Clients that need CPU load information periodically must register with
 * this API. 
 *  
 * Periodic clients will only receive callbacks with reason code 
 * CLM_UPDATE_REASON_PERIODIC. 
 *  
 * @param clientName : Name of client; should be limited to CLM_MAX_NAME_LENGTH
 * characters (including the terminating NULL)
 * 
 * @param clientType : Type of client; one of CLM_ClientOutputType.
 *
 * @param measPeriodUs : Measurement period of client 
 *
 * @param cbFunc : Client callback function of type CLM_CallbackFuncPtr 
 * 
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 * 
 * @return A non-NULL client handle is returned on successful registration.
 */
CLM_HandleType
CLM_RegisterPeriodicClient( const char            *clientName,
                            CLM_ClientOutputType   clientType, 
                            uint32                 measPeriodUs,
		            uint32                  attributes,
                            CLM_CallbackFuncPtr		 cbFunc,
                            void                  *cbData )
{
  CLM_Client *newClient = CLM_RegisterClient( clientName, clientType,
                                              measPeriodUs, 
                                              CLM_ATTRIBUTE_DEFAULT,
                                              cbFunc, cbData );
  CLM_EngineRegisterClient( newClient );
  return newClient;
}

/** 
 * <!-- CLM_RegisterPeriodicClient_Ex -->
 *  
 * A version of the RegisterPeriodicClient API that takes an additional
 * attributes parameter.
 *  
 * Periodic clients will only receive callbacks with reason code 
 * CLM_UPDATE_REASON_PERIODIC. 
 *  
 * @param clientName : Name of client; should be limited to CLM_MAX_NAME_LENGTH
 * characters (including the terminating NULL)
 * 
 * @param clientType : Type of client; one of CLM_ClientOutputType.
 *
 * @param measPeriodUs : Measurement period of client 
 *
 * @param attributes : Attributes to modify CLM behavior for this client
 *
 * @param cbFunc : Client callback function of type CLM_CallbackFuncPtr 
 * 
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 * 
 * @return A non-NULL client handle is returned on successful registration.
 */
CLM_HandleType 
CLM_RegisterPeriodicClient_Ex( const char            *clientName,
                               CLM_ClientOutputType   clientType, 
                               uint32                 measPeriodUs,
                               uint32                 attributes,
                               CLM_CallbackFuncPtr		cbFunc,
                               void                  *cbData )
{
  CLM_Client *newClient = CLM_RegisterClient( clientName, clientType,
                                              measPeriodUs, attributes,
                                              cbFunc, cbData );
  CLM_EngineRegisterClient( newClient );
  return newClient;
}


/**
 * <!-- CLM_RegisterThresholdClient -->
 * 
 * Clients that need CPU Load information when a specific threshold is crossed
 * must register using this API.
 *
 * Note that all threshold values will be compared against CPU utilization
 * scaled to MAX clock for the given target.
 *  
 * Clients that have specified a low/high threshold will only receive callbacks 
 * with reason code CLM_UPDATE_REASON_BELOW_LOW_THRESHOLD or 
 * CLM_UPDATE_REASON_BELOW_HIGH_THRESHOLD. 
 *  
 * It is legal to set no thresholds, only a low or a high threshold or to set 
 * both thresholds. If no thresholds are specified, the client is treated as 
 * PERIODIC.
 *
 * @param clientName : Name of client; should be limited to CLM_MAX_NAME_LENGTH
 * characters (including the terminating NULL)
 * 
 * @param clientType : Type of client; one of CLM_ClientOutputType.
 *
 * @param measPeriodUs : Measurement period of client 
 *
 * @param params : Pointer to a CLM_ThresholdClientRegStruct.
 * Will be copied over by CLM during registration.
 *
 * @param attributes : Attributes to modify CLM behavior for this client
 *   
 * @param cbFunc : Client callback function of type CLM_CallbackFuncPtr 
 * 
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 * 
 * @return A non-NULL client handle is returned on successful registration.
 */
CLM_HandleType 
CLM_RegisterThresholdClient( const char            *clientName,
                             CLM_ClientOutputType   clientType,
                             uint32                 measPeriodUs,
                             CLM_ThresholdClientRegStruct *params,
                             uint32                 attributes,
                             CLM_CallbackFuncPtr    cbFunc,
                             void                  *cbData )
{
  uint32 nHWThreads = CLM_EngineGetNumHWThreads();

  CORE_VERIFY_PTR( params );
  CORE_VERIFY( params->nThreadsForHigh <= nHWThreads );
  CORE_VERIFY( params->nThreadsForLow <= nHWThreads );

  CLM_Client *newClient = CLM_RegisterClient( clientName, clientType,
                                              measPeriodUs, attributes,
                                              cbFunc, cbData );

  newClient->nThreadsForHigh = params->nThreadsForHigh;
  newClient->nThreadsForLow = params->nThreadsForLow;
  newClient->lowThresholdPct = params->lowThresholdPct;
  newClient->highThresholdPct = params->highThresholdPct;

  /* lastKnownThreshold is already initialized to UNKNOWN in RegisterClient */

  CLM_EngineRegisterClient( newClient );
  return newClient;
}

/**
 * <!-- CLM_UnregisterClient -->
 * 
 * @brief Unregister a client from CLM
 * 
 * @param handle : Handle to client
 */
void CLM_UnregisterClient( CLM_HandleType handle )
{
  CLM_EngineUnregisterClient( handle );
}


/**
 * <!-- CLM_GetLastLPRExitTime -->
 * 
 * @brief Returns the timetick recorded at the last CLM LPR exit
 * @return Timetick of last CLM LPR exit
 */
uint64 CLM_GetLastLPRExitTime( void )
{
  return CLM_EngineGetLastLPRExitTime();
}

/**
 * <!-- CLM_SetClientMeasurementPeriod -->
 * 
 * @brief Modify the measurement period of the client. Must be invoked
 * from within the client' callback, not outside.
 *  
 * @param client : Client whose measPeriod is to be modified
 * 
 * @param measPeriodUs : New measurement period in usec
 */
void CLM_SetClientMeasurementPeriod( CLM_HandleType client, uint32 measPeriodUs )
{
  CORE_VERIFY_PTR( client );
  CORE_VERIFY( measPeriodUs != 0 );

  client->measPeriodUs = measPeriodUs;
}
