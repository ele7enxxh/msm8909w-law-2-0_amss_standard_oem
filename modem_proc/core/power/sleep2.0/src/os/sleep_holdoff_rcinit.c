/*==============================================================================
  FILE:         sleep_holdoff.c
  
  OVERVIEW:     This file provides functions for supporting sleep holdoff 
                feature which will prevent processor to go in to any low
                power mode until all clients are ready.
 
  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/os/sleep_holdoff_rcinit.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALSys.h"
#include "CoreVerify.h"
#include "sleep_idle_plugin.h"
#include "npa.h"
#include "rcinit.h"
#include "rcevt_dal.h"

/*==============================================================================
                               EXTERNAL VARIABLES
 =============================================================================*/
extern volatile boolean g_sleepAllowLowPowerModes;

/*==============================================================================
                               INTERNAL VARIABLES
 =============================================================================*/
/**
 * @brief g_SleepRCInitCompleteEventHandle
 *
 * DAL event handle used by RCInit framework to signal that it has finished
 * initialization.
 */
DALSYSEventHandle g_SleepRCInitCompleteEventHandle;

/**
 * @brief g_SleepRCInitCompleteSignal
 *
 * Signal object which will be registered with RCInit framework for 
 * initialization completion event.
 */
RCEVT_SIGEX_SIGDAL g_SleepRCInitCompleteSignal;

/*==============================================================================
                               INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * @brief sleepHoldoff_releaseCB
 *
 * Callback function that gets called through DAL event which will be
 * generated as a signal when RCInit framework finishes initialization. Once 
 * called, it will issue request to release holdoff on Sleep on behalf of 
 * systems that gets initialized via RCInit. 
 *
 * @param cb_ctxt: Call back context used while registering it. 
 * @param val: Param used by Event generator 
 * @param payload: Payload passed by Event generator
 * @param payload_size: Size of the above payload
 *
 * @NOTE: None of the parameters passed is used for now. They are for
 *        complying with callback prototype.
 *
 * @return Nothing (to comply with callback prototype)
 */
static void* sleepHoldoff_releaseCB
( 
  void    *cb_ctxt,
  uint32  val,
  void    *payload,
  uint32  payload_size
)
{
  /* NPA client to release request when RCINIT finished initialization. */
  static npa_client_handle sleep_holdoff_client;

  if ( sleep_holdoff_client == NULL )
  {
    CORE_VERIFY_PTR( sleep_holdoff_client = 
                     npa_create_sync_client("/sleep/idle/plugin",
                                            "SleepRcinitHoldoff",
                                            SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF)
                   );
  }

  /* Releasing hold off from RCinit side if global sleep flag set to allow
   * low power modes. */
  if(TRUE == g_sleepAllowLowPowerModes)
  {
    npa_issue_required_request( sleep_holdoff_client, 0 );
  }

  return NULL;
}

/*==============================================================================
                               GLOBAL FUNCTIONS
 =============================================================================*/
/**
 * @brief sleepHoldoff_registerRCinitCompletionEvent
 *
 * Registers an initialization completion event and corresponding callback 
 * with RCInit framework.
 */
void sleepHoldoff_registerRCinitCompletionEvent( void )
{
  RCEVT_HANDLE  ret_handle;
  DALResult     dal_result;

  /* Creating a DAL event */
  dal_result =
    DALSYS_EventCreate( DALSYS_EVENT_ATTR_NORMAL | 
                        DALSYS_EVENT_ATTR_CALLBACK_EVENT,   /* Attributes */
                        &g_SleepRCInitCompleteEventHandle,  /* Event Handle */
                        NULL );                             /* Event Object */
  CORE_DAL_VERIFY( dal_result );

  /* Set up a call back for the DAL event created above */
  dal_result =
    DALSYS_SetupCallbackEvent( g_SleepRCInitCompleteEventHandle,/* Event handle */
                               sleepHoldoff_releaseCB,          /* Callback */
                               NULL );                          /* Context */
  CORE_DAL_VERIFY( dal_result );


  /* Registering the DAL event as a signal with RCInit framework which will 
   * be generated after completing intialization */
  g_SleepRCInitCompleteSignal.signal = g_SleepRCInitCompleteEventHandle;
  ret_handle = 
    rcevt_register_sigex_name( RCINIT_RCEVT_INITGROUPS,         /* Event Name */
                               RCEVT_SIGEX_TYPE_SIGDAL,         /* Signal Type */
                               &g_SleepRCInitCompleteSignal );  /* Signal Obj */
  CORE_VERIFY( RCEVT_NULL != ret_handle);

  return;
}

