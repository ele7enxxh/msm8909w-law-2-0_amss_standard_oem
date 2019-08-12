/*============================================================================
  FILE:         sleep_holdoff.c
  
  OVERVIEW:     This file provides functions for supporting sleep holdoff 
                feature which will prevent processor to go in to any low
                power mode until all clients are ready.
 
                Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary

  $Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/sleep_holdoff_rcinit.c#1 $
============================================================================*/

#include "DALSys.h"
#include "CoreVerify.h"
#include "sleep_idle_plugin.h"
#include "npa.h"
#include "rcinit.h"
#include "rcevt_dal.h"

/*============================================================================
 *                             INTERNAL VARIABLES
 *===========================================================================*/

/**
 * @brief hSleepRCInitCompleteEvent
 *
 * DAL event handle used by RCInit framework to signal that it has finished
 * initialization.
 */
DALSYSEventHandle hSleepRCInitCompleteEvent;

/**
 * @brief gSleepRCInitCompleteSignal
 *
 * Signal object which will be registered with RCInit framework for 
 * initialization completion event.
 */
RCEVT_SIGEX_SIGDAL gSleepRCInitCompleteSignal;

/*===========================================================================
 *                             INTERNAL FUNCTIONS
 *==========================================================================*/

/**
 * @brief sleep_holdoff_release_cb
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
static void* sleep_holdoff_release_cb
( 
  void *cb_ctxt, 
  uint32 val,
  void *payload,
  uint32 payload_size
)
{
  /* It is observed that sleep_holdoff_release_cb is being called twice in MSS
   * from RCINIT framework and this is not expected. CR has been raised to 
   * ensure that the DAL Callback use case reception of spurious signal should
   * be corrected and once it is fix in RCINIT framework this check can be 
   * removed */
  static int bIsHoldOffReleased = FALSE;

  if(TRUE == bIsHoldOffReleased)
  {
    return NULL;
  }

  /* NPA client to release request when RCINIT finished initialization. */
  npa_client_handle sleep_holdoff_client = NULL;

  CORE_VERIFY_PTR( sleep_holdoff_client = 
                   npa_create_sync_client("/sleep/idle/plugin",
                                          "SleepRcinitHoldoff",
                                          SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF)
  );

  /* Releasing hold off from RCinit side */
  npa_issue_required_request( sleep_holdoff_client, 0 );
  bIsHoldOffReleased = TRUE; 
  return NULL;
}

/*===========================================================================
 *                              GLOBAL FUNCTIONS
 *==========================================================================*/

/**
 * @brief sleep_holdoff_reg_rcinit_comp_event
 *
 * Registers an initialization completion event and corresponding callback 
 * with RCInit framework.
 */
void sleep_holdoff_reg_rcinit_comp_event( void )
{
  RCEVT_HANDLE ret_handle;
  DALResult dal_result;

  /* Creating a DAL event */
  dal_result =
    DALSYS_EventCreate( DALSYS_EVENT_ATTR_NORMAL | 
                        DALSYS_EVENT_ATTR_CALLBACK_EVENT,     /* Attributes */
                        &hSleepRCInitCompleteEvent,           /* Event Handle */
                        NULL );                               /* Event Object */
  CORE_DAL_VERIFY( dal_result );

  /* Set up a call back for the DAL event created above */
  dal_result =
    DALSYS_SetupCallbackEvent( hSleepRCInitCompleteEvent,     /* Event handle */
                               sleep_holdoff_release_cb,      /* Callback */
                               NULL );                        /* Context */
  CORE_DAL_VERIFY( dal_result );


  /* Registering the DAL event as a signal with RCInit framework which will 
   * be generated after completing intialization */
  gSleepRCInitCompleteSignal.signal = hSleepRCInitCompleteEvent;
  ret_handle = 
    rcevt_register_sigex_name( RCINIT_RCEVT_INITGROUPS,       /* Event Name */
                               RCEVT_SIGEX_TYPE_SIGDAL,       /* Signal Type */
                               &gSleepRCInitCompleteSignal ); /* Signal Obj */
  CORE_VERIFY( RCEVT_NULL != ret_handle);

}

/**
 * @brief sleep_holdoff_no_mode
 *
 * @see sleep_idle_plugin.h file for details
 */
uint32 sleep_holdoff_no_mode( void )
{
  /* Due to some holdoff client, we cannot simply perform any low power modes.
   * Simply return. At some point, sleep function pointer would change when all
   * holdoff requests are released. Until then, we will keep coming here. */
  return 0;
}

