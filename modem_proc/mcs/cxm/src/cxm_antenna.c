/*!
  @file
  cxm_antenna.c

  @brief
  This file contains the message handlers and algorithms for sharing
  the diversity antenna with WLAN using WCI-2 Control Plane messaging

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_antenna.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/17/14   btl     Make timer non-deferrable
08/01/14   btl     New TRM APIs, account for IRAT
02/02/14   ckk     Adding antenna grant duration
11/17/13   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <rcevt_rex.h> /* RCEVT Signal Delivery by REX */
#include <trm.h>
#include <timer.h>
#include <npa.h>
#include <coexistence_service_v02.h>
#include <wci2_uart.h> /* UART NPA vote */
#include "cxm_utils.h" /* rex & task signals, MSG */
#include "cxm_trace.h"
#include "coex_algos_v2.h"
#include "cxm_antenna.h"

#define CXM_SCLK_PER_MS 33
/* give a 2ms buffer before release must happen (in 32kHz ticks) */
#define CXM_ANTENNA_RELEASE_BUFFER 2 * CXM_SCLK_PER_MS
/* minimum duration for requesting antenna in 32kHz ticks = 100ms */
#define CXM_ANTENNA_REQ_MIN_DUR (40 * CXM_SCLK_PER_MS + CXM_ANTENNA_RELEASE_BUFFER)
/* maximum duration representable in QMB msg (in ms) */
#define CXM_ANTENNA_MAX_DUR_MS 0xFFFF
/* maximum duration representable in TRM duration (timetick32) */
#define CXM_TRM_MAX_DUR_TICK 0xFFFFFFFF

/* antenna token has special states instead of the usual sync_state_token mask */
#define COEX_ANT_STATE_OFF    0x00
#define COEX_ANT_STATE_IRAT   0x01
#define COEX_ANT_STATE_ACTIVE 0x02

/* set fields within sync_state_token */
#define COEX_ANT_TOKEN_DUR_MASK  0x1F
#define COEX_ANT_TOKEN_DUR_SHIFT 0
#define COEX_ANT_TOKEN_DUR_DIV_FACTOR 5
#define COEX_ANT_STATE_MASK  0x03
#define COEX_ANT_STATE_SHIFT 6
#define COEX_BITFLD_VAL(var,field) ( \
  ((var) & (field##_MASK << field##_SHIFT)) >> field##_SHIFT )
/* take a numerical value and shift and mask it into field position */
#define COEX_BITFLD_FVAL(var,field) ( \
  ((var) & field##_MASK) << field##_SHIFT )
/* read, mask in supplied data, then write back */
#define COEX_BITFLD_SET(var,val,field) (var)=    \
  ( ((var) & ~(field##_MASK << field##_SHIFT)) | \
    (((val) & field##_MASK) << field##_SHIFT) )

static cxm_antenna_state_s cxm_antenna_state = 
{
  CXM_ANTENNA_UNUSED, NULL, NULL, NULL, 0, 0, 0, 0
};

/* @brief Antenna npa client for voting uart power state on/off */
static npa_client_handle cxm_antenna_uart_client;

/*! @brief Timer to track antenna duration grants */
static timer_type cxm_antenna_timer;

/* trm callback declarations */
void cxm_antenna_trm_async_event_cb(
  trm_async_callback_information_type* event_data );

/*=============================================================================

                      INTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  cxm_get_antenna_state_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to the global cxm_antenna_state struct for diag logging.

    @return
    cxm_antenna_state_s *
*/
/*===========================================================================*/
cxm_antenna_state_s *cxm_get_antenna_state_ptr( void )
{
  return &cxm_antenna_state;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_update_state_info

=============================================================================*/
/*!
    @brief
    Update stored antenna sharing state info and send a new state notification
    if anything changed

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_update_state_info( void )
{
  /*-----------------------------------------------------------------------*/
  npa_issue_required_request( cxm_antenna_uart_client, WCI2_UART_NPA_REQ_ON );
  cxm_antenna_update_duration();
  coex_send_diversity_antenna_state_ind_v02();
  npa_issue_required_request( cxm_antenna_uart_client, WCI2_UART_NPA_REQ_OFF );
  CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_ANTENNA_STATE );

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_request_and_notify

=============================================================================*/
/*!
    @brief
    Request antenna on behalf of WLAN

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_request_and_notify( void )
{
  trm_request_notify_input_info params;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( MED, "Antenna: Released, new req for %d", CXM_ANTENNA_REQ_MIN_DUR );

  params.client_id = TRM_WLAN;
  params.resource = TRM_RX_BEST;
  params.duration = CXM_ANTENNA_REQ_MIN_DUR;
  params.reason = TRM_ACCESS;
  /* Need at least 1 band, which band irrelevant since all bands conflict */
  params.freq_info.num_bands = 1;
  params.freq_info.bands[0].band = SYS_BAND_CLASS_MAX;
  params.wakeup_identifier = 0; /* not used */
  params.tag = 0; /* not used */
  trm_request_and_notify( &params );
  CXM_TRACE( CXM_TRC_ANT_RELEASE_AND_REQUEST, TRM_WLAN, 
             TRM_RX_BEST, CXM_ANTENNA_REQ_MIN_DUR,
             CXM_TRC_AND_PKT_EN, CXM_LOG_ANTENNA_STATE );

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_grant_callback

=============================================================================*/
/*!
    @brief
    Process the Req & notify event callback from TRM. This is one of the
    events handled by the TRM Asynchronous event callback.

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_grant_callback(
  trm_req_n_not_return_data *data 
)
{
  /*-----------------------------------------------------------------------*/
  CXM_TRACE( CXM_TRC_ANT_GRANT_CB, 0, data->grant, 0, CXM_TRC_EN );
  /* check if we were granted the chain */
  if( TRM_GRANTED == data->grant )
  {
    /* real grant info comes from unlock cb */
    cxm_antenna_state.grant_dur = CXM_TRM_MAX_DUR_TICK;
    /* signal task for processing in our context */
    (void)rex_set_sigs( &cxm_tcb, CXM_ANTENNA_GRANT_SIG );
  }
  else
  {
    /* request denied, request again */
    /* Should not happen with request_and_notify */
    CXM_MSG_0( ERROR, "TRM request denied" );
    cxm_antenna_state.state = CXM_ANTENNA_PENDING;
    cxm_antenna_request_and_notify();
  }

  return;
}

/*============================================================================
FUNCTION: cxm_antenna_unlock_callback

CALLBACK TRM_UNLOCK_CALLBACK_T

DESCRIPTION
  Process the Unlock event from the TRM asynchronous event callback

DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void cxm_antenna_unlock_callback(
  trm_unlock_callback_data *data
)
{
  uint64        dur;
  timetick_type timer_val;
  /*-----------------------------------------------------------------------*/
  dur = data->unlock_by_sclk - timetick_get();
  CXM_TRACE( CXM_TRC_ANT_UNLOCK_CB, data->event, dur, data->unlock_by_sclk, 
             CXM_TRC_EN );
  CXM_MSG_4( MED, "Antenna: unlock req event=%d unlock_in=%u sclk (winner=%d reason=%d)",
             data->event, dur, data->winning_client_info.client_id, 
             data->winning_client_info.reason );
  if( TRM_UNLOCK_BY == data->event && dur > CXM_ANTENNA_RELEASE_BUFFER )
  {
    /* don't need to unlock now, but need to set timer and unlock
     * before the given timestamp */
    cxm_antenna_state.grant_dur = dur;
    timer_val = dur - CXM_ANTENNA_RELEASE_BUFFER;
    timer_set( &cxm_antenna_timer, timer_val, 0, T_TICK );
  }
  else if( TRM_UNLOCK_CANCELLED == data->event )
  {
    /* don't need to unlock. clear any existing unlock timer */
    cxm_antenna_state.grant_dur = CXM_TRM_MAX_DUR_TICK;
    (void) timer_clr( &cxm_antenna_timer, T_TICK );
  }
  else
  {
    /* clear timer since we're releasing now */
    (void) timer_clr( &cxm_antenna_timer, T_TICK );
    /* unlock now. signal task to do dirty work */
    (void) rex_set_sigs( &cxm_tcb, CXM_ANTENNA_RELEASE_SIG );
  }

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_irat_callback

=============================================================================*/
/*!
    @brief
    IRAT callback lock release info from TRM -- notify CXM that a tech
    is doing IRAT and we need to release the antenna

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_irat_callback( trm_irat_event_conflict_data *data )
{
  /*-----------------------------------------------------------------------*/
  /* signal task to process -- same as unlock? */
  CXM_MSG_2( MED, "IRAT: tech=%d dur=%u", data->irat_client, 
             data->irat_duration );
  cxm_antenna_state.irat_dur = data->irat_duration;
  cxm_antenna_state.irat_start_ts = timetick_get_sclk64();

  /* clear timer since we're releasing now */
  (void) timer_clr( &cxm_antenna_timer, T_TICK );
  (void) rex_set_sigs( &cxm_tcb, CXM_ANTENNA_RELEASE_SIG );
  CXM_TRACE( CXM_TRC_ANT_IRAT_CB, data->irat_client, 0, data->irat_duration,
             CXM_TRC_EN );

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_trm_async_event_cb

=============================================================================*/
/*!
    @brief
    Callback from TRM to handle asynchronous events. There are currently 3:
      1. Grant
      2. Unlock
      3. IRAT

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_trm_async_event_cb(
  trm_async_callback_information_type* event_data
)
{
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( TRM_WLAN == event_data->client );
  CXM_MSG_1( MED, "TRM CB: event %d", event_data->cb_type );

  /* interpret callback data (union) based on event type */
  switch( event_data->cb_type )
  {
    case TRM_REQUEST_AND_NOTIFY_ASYNC_INFO:
      cxm_antenna_grant_callback( &event_data->data.chain_req_n_not_info );
      break;

    case TRM_UNLOCK_CHAIN_ASYNC_INFO:
      cxm_antenna_unlock_callback( &event_data->data.unlock_info );
      break;

    case TRM_IRAT_EVENT_ASYNC_INFO:
      cxm_antenna_irat_callback( &event_data->data.irat_event_info );
      break;

    default:
      CXM_MSG_1( ERROR, "Unhandled TRM event %d", event_data->cb_type );
      break;
  }

  return;
}

/*=============================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  cxm_antenna_init

=============================================================================*/
/*!
    @brief
    Initialize resources and start diversity antenna resource request

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_init
(
  coex_tech_sync_state_v02 *sync_state_p,
  coex_antenna_state_v02   *state_info_p,
  boolean                  *token_wrap_flag_p
)
{
  trm_async_event_cb_input_type cb_inputs;
  /*-----------------------------------------------------------------------*/
  /* get relevant pointers to global coex_algos_v2 data structures 
   * for local use */
  CXM_ASSERT( sync_state_p != NULL && 
              state_info_p != NULL &&
              token_wrap_flag_p != NULL );
  cxm_antenna_state.info = state_info_p;
  cxm_antenna_state.sync = sync_state_p;
  cxm_antenna_state.token_wrap = token_wrap_flag_p;

  /* initialize state information */
  cxm_antenna_state.info->adv_notice = 0;
  cxm_antenna_state.info->duration = CXM_ANTENNA_MAX_DUR_MS;
  cxm_antenna_state.sync->sync_state_token = 0;
  cxm_antenna_state.grant_dur = 0;
  cxm_antenna_state.irat_dur = 0;
  cxm_antenna_state.grant_start_ts = 0;
  cxm_antenna_state.irat_start_ts= 0;

  /* create npa client for turning on/off uart */
  cxm_antenna_uart_client = npa_create_sync_client(
                              "/modem/mcs/cxm_uart",
                              "cxm_antenna",
                              NPA_CLIENT_REQUIRED );

  /* define timer to keep track of grant duration */
  timer_def( &cxm_antenna_timer, &cxm_nondeferrable_timer_group, 
             &cxm_tcb, CXM_ANTENNA_RELEASE_SIG, NULL, 0 );

  /* wait for TRM to enter system before starting antenna sharing */
  (void) rcevt_wait_name("TRM_INIT_COMPLETE");

  /* register TRM async callback */
  cb_inputs.client_id = TRM_WLAN;
  cb_inputs.operation = TRM_ASYNC_CB_REGISTER;
  cb_inputs.data.reg_info.client_id = TRM_WLAN;
  cb_inputs.data.reg_info.events_bitmask = TRM_ASYNC_IRAT_EVENT_INFO;
  cb_inputs.data.reg_info.callback_ptr = cxm_antenna_trm_async_event_cb;
  trm_handle_async_event_cb( &cb_inputs );

  /* begin antenna sharing request */
  cxm_antenna_state.state = CXM_ANTENNA_PENDING;
  cxm_antenna_request_and_notify();

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_deinit

=============================================================================*/
/*!
    @brief
    Release resources associated with WWAN/WLAN antenna sharing

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_deinit( void )
{
  trm_release_chain_input_info trm_info = { TRM_WLAN, 0 };
  /*-----------------------------------------------------------------------*/

  /* clear all timers (if any pending) */
  (void) timer_clr( &cxm_antenna_timer, T_TICK );
  trm_release_chain( &trm_info );
  cxm_antenna_state.state = CXM_ANTENNA_UNUSED;

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_update_duration

=============================================================================*/
/*!
    @brief
    Update the grant duration saved in the QMB antenna state struct, based
    on the original grant duration and how much time has passed since grant

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_update_duration( void )
{
  uint64 ts, elapsed;
  uint32 dur, dif_ms;
  uint16 token_dur;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( cxm_antenna_state.state != CXM_ANTENNA_UNUSED );
  if( cxm_antenna_state.state == CXM_ANTENNA_GRANTED )
  {
    /* if not infinite duration grant, update state with time remaining */
    dur = cxm_antenna_state.grant_dur;
    if( dur < CXM_TRM_MAX_DUR_TICK )
    {
      ts = timetick_get_sclk64();
      elapsed = ts - cxm_antenna_state.grant_start_ts;
      if( elapsed < dur - CXM_ANTENNA_RELEASE_BUFFER )
      {
        dif_ms = dur - CXM_ANTENNA_RELEASE_BUFFER - elapsed;
        dif_ms = timetick_cvt_from_sclk( dif_ms, T_MSEC );
        cxm_antenna_state.info->duration = (dif_ms > CXM_ANTENNA_MAX_DUR_MS) ? 
          CXM_ANTENNA_MAX_DUR_MS : (uint16) dif_ms;
      }
      else
      {
        /* not enough duration left -- inside buffer zone or already expired */
        cxm_antenna_state.info->duration = 0;
      }
    }
    else
    {
      cxm_antenna_state.info->duration = CXM_ANTENNA_MAX_DUR_MS;
    }
  }
  else if( cxm_antenna_state.state == CXM_ANTENNA_IRAT )
  {
    /* for IRAT, we have a rough idea of how long tech will hold antenna */
    dur = cxm_antenna_state.irat_dur;
    ts = timetick_get_sclk64();
    elapsed = ts - cxm_antenna_state.irat_start_ts;
    if( elapsed < dur )
    {
      dif_ms = dur - elapsed;
      dif_ms = timetick_cvt_from_sclk( dif_ms, T_MSEC );
      cxm_antenna_state.info->duration = (dif_ms > CXM_ANTENNA_MAX_DUR_MS) ? 
        CXM_ANTENNA_MAX_DUR_MS : (uint16) dif_ms;
    }
    else
    {
      /* IRAT durn indicated by TRM exceeded; now treat as normal pending */
      cxm_antenna_state.state = CXM_ANTENNA_PENDING;
      COEX_BITFLD_SET( cxm_antenna_state.sync->sync_state_token,
                       COEX_ANT_STATE_OFF, COEX_ANT_STATE );
      cxm_antenna_state.info->duration = CXM_ANTENNA_MAX_DUR_MS;
    }
  }
  else
  {
    /* if not IRAT, we have no idea how long tech will hold antenna for */
    cxm_antenna_state.info->duration = CXM_ANTENNA_MAX_DUR_MS;
  }

  /* update sync state token with duration info: first 5 bits */
  token_dur = cxm_antenna_state.info->duration / COEX_ANT_TOKEN_DUR_DIV_FACTOR;
  if( token_dur >= COEX_ANT_TOKEN_DUR_MASK )
  {
    token_dur = COEX_ANT_TOKEN_DUR_MASK;
  }
  COEX_BITFLD_SET( cxm_antenna_state.sync->sync_state_token,
                   token_dur, COEX_ANT_TOKEN_DUR );

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_handle_grant

=============================================================================*/
/*!
    @brief
    We have been granted the WLAN antenna; execute appropriate actions and
    send message to WLAN

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_handle_grant( void )
{
  trm_retain_chain_lock_input_info lock_params;
  /*-----------------------------------------------------------------------*/
  /* save timestamp for duration tracking across sleep states */
  cxm_antenna_state.grant_start_ts = timetick_get_sclk64();

  /* You have been granted usage an RF chain! Hold the antenna */
  /* TRM notifies us of grant info from the context of retain_chain_lock */
  cxm_antenna_state.state = CXM_ANTENNA_GRANTED;
  lock_params.client = TRM_WLAN;
  lock_params.tag = 0;
  lock_params.unlock_tag = 0;
  trm_retain_chain_lock( &lock_params );

  /* grant info should've been updated by TRM by here, inside retain_lock.
   * IF not, we go with the defaults (infinite) */

  /* tell WCN the good news */
  COEX_BITFLD_SET( cxm_antenna_state.sync->sync_state_token,
                   COEX_ANT_STATE_ACTIVE, COEX_ANT_STATE );
  cxm_antenna_update_state_info();

  return;
}

/*=============================================================================

  FUNCTION:  cxm_antenna_handle_release

=============================================================================*/
/*!
    @brief
    We have been asked us to release Antenna. Can be caused two ways:
      1. TRM asked us to release through callback
      2. Our grant duration is up and our timer fired

    @detail
    Submit a new request and notify WLAN that we lost it.

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_handle_release( void )
{
  uint8  state_mask;
  uint64 ts, elapsed;
  /*-----------------------------------------------------------------------*/
  /* need to unlock the antenna chain now */
  /* tell WLAN they lost the antenna */
  cxm_antenna_state.state = CXM_ANTENNA_PENDING;
  state_mask = COEX_ANT_STATE_OFF;

  /* is this an unlock due to IRAT? */
  if( cxm_antenna_state.irat_dur < CXM_TRM_MAX_DUR_TICK && 
      cxm_antenna_state.irat_dur > 0 )
  {
    ts = timetick_get_sclk64();
    elapsed = ts - cxm_antenna_state.irat_start_ts;
    if( elapsed < cxm_antenna_state.irat_dur )
    {
      /* the answer is yes. */
      cxm_antenna_state.state = CXM_ANTENNA_IRAT;
      state_mask |= COEX_ANT_STATE_IRAT;
    }
  }

  COEX_BITFLD_SET( cxm_antenna_state.sync->sync_state_token,
                   state_mask, COEX_ANT_STATE );
  cxm_antenna_update_state_info();

  /* make new request. will implicitly release currently-held chain */
  CXM_MSG_1( HIGH, "Antenna release sig, IRAT=%d", state_mask );
  cxm_antenna_request_and_notify();

  return;
}

