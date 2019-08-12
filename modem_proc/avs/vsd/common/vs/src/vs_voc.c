/**
  @file vs_voc.c
  @brief This file contains the definitions of the internal functions of VS.

*/

/*
   Copyright (C) 2013-2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/src/vs_voc.c#1 $
   $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  05/13/15  aim   Added locks while accessing queues.
  08/05/14  sre   Added support for enabing eAMR over VS.
  06/04/14  shr   Sending MVM stop command during handovers instead on MVM standby.
  12/09/13  js    Added support for clearing the CVD sessions, after the end of
                  each call.
  10/18/13  rm    Replaced apr_list_init() with apr_list_init_v2().
                  Added vs_voc_cmd_thread_lock to make vs_voc_cmd_open thread
                  safe.
  09/20/13  sre   Fixed the banned QPSI memcpy APIs.
  08/09/13  sud   Renamed macro PC_ENV_SET to WINSIM.
  07/30/13  sud   Updated software to report MSG_FATAL when fatal error
                  conditions occurs in client context.
  07/10/13  sg    Fixed the KW errors.
  06/29/13  sg    Fixed the banned QPSI memcpy APIs.
  06/26/13  sud   Updated debug messages and rearranged some of the functions
  06/16/13  sud   Updated VS to handle failed CVD command and to process
                  subsequent commands.
  06/13/13  hl    Added support for handling ADSP SSR.
  06/12/13  sud   Updated VS to handle errors and status propagation to client.
                  Updated VS to handle memory leak and memory corruption
                  issues.
                  Updated VS to handle command priorities and proper sequencing.
                  Updated VS to add proper debug messages
  06/01/13  sud   Fixed compiler warnings.
  05/16/13  sg    Fixed bug in close cmd
  04/16/13  sud   VS refactored code and fixed bugs.
  01/12/13  sg    Initial revision

*/

/****************************************************************************
 * HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "msg.h"

#ifndef WINSIM
#include "sys_m_messages.h"
#include "rcecb.h"
#endif /* !WINSIM */

#include "mmstd.h"

#include "apr_event.h"
#include "apr_thread.h"
#include "apr_timer.h"
#include "voicecfg_api.h"
#include "voicecfg_items.h"
#include "vss_private_if.h"
#include "vs_i.h"


/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
/* Time in ns for test porposes.
 */
#define VS_VOC_TEST_TIMER_DURATION_NS ( 10000000 )

/* Timeout value for ADSP/CVD response to gating commands.
 */
#define VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS ( 300000000 )


/* Name of CVD session created by VS for multimode voice system.
 * CVD session name is not changed to maintain backwards
 * compatability.
 */
static char_t vs_default_stream_name[] = "default modem voice";

/* Name of CVD session created by VS based off VSID */

#if 0
/* Multimode voice system - not used as of now */
static char_t vs_vsid_cs_voice_mm_stream_name[] = "10C01000";
#endif
  /* GSM only voice system (GSM2) */
static char_t vs_vsid_cs_voice_gsm2_stream_name[] = "10DC1000";

static uint32_t mmode_active_client = VS_VOC_CLIENT_UNDEFINED_V;
static uint32_t gsm_active_client = VS_VOC_CLIENT_UNDEFINED_V;


/* Flag to indicate if eAMR vocoder can be enabled or not. */
bool_t vs_voicecfg_is_2g_eamr_enabled = FALSE;
bool_t vs_voicecfg_is_3g_eamr_enabled = FALSE;
bool_t vs_voicecfg_is_td_eamr_enabled = FALSE;

/* The following are used to track the VS objects with type
 * VS_OBJECT_TYPE_ENUM_SIMPLE_JOB and VS_OBJECT_TYPE_ENUM_HANDLE, in order to
 * free them in case of ADSP SSR.
 */
static apr_list_t vs_voc_ssr_obj_tracking_free_q;
static apr_list_t vs_voc_ssr_obj_tracking_used_q;
static vs_generic_item_t vs_voc_ssr_obj_tracking_pool[ VS_MAX_OBJECTS_V ];
apr_lock_t vs_voc_obj_track_q_lock;

/**
 * Timer used for testing voice resource unavailability/availibility, during
 * voice call.
 */
static apr_timer_t vs_voc_test_timer;

/**
 * Timer used for tracking ADSP/CVD Timeout scenarios which may lead
 * VS priority3 thread to stuck indfinitely.
 * 
 * TIMEOUT Mechanism should only be used in VS LOW priority thread context.
 * LOW thread should not trigger more than one commands that involves the ADSP 
 * timeout mechanism.
 *
 * Post ADSP/CVD recovery cleanup and recovery of CVD needs to be guarented.
 */
apr_timer_t vs_voc_adsp_timeout_timer;


/****************************************************************************
 * EXTERN DECLARATIONS                                                      *
 ****************************************************************************/
extern uint16_t vs_my_addr;
extern uint16_t vs_mvm_addr;
extern uint16_t vs_cvs_addr;

extern apr_lock_t vs_voc_cmd_thread_lock;

extern apr_list_t vs_free_session_q;
extern apr_list_t vs_used_session_q;

extern bool_t vs_is_cvd_service_up;
extern bool_t vs_is_cvd_cleanup_done;
extern bool_t vs_is_adsp_timeout_triggered;

extern uint32_t vs_apr_handle;
extern apr_objmgr_t vs_objmgr;
extern apr_memmgr_type vs_heapmgr;

/* apr_packet_t to faciliate RX packet exchnage for Multimode clients. */
extern aprv2_packet_t mmode_rx_packet;

extern apr_list_t vs_med_priority_nongating_packet_q;
extern vs_gating_control_t vs_low_priority_gating_packet_q;
extern apr_list_t vs_low_priority_nongating_packet_q;

/* Temporarily added for debugging command throttling issue */
extern uint32_t timer_cntr;
extern uint32_t timer_stop_cntr;

/****************************************************************************
 * VS OBJECT CREATION AND DESTRUCTION ROUTINES                              *
 ****************************************************************************/

static uint32_t vs_voc_create_session_object (
  vs_session_object_t** p_ret_session_obj
)
{
  uint32_t rc = VS_EOK;
  uint32_t checkpoint = 0;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( p_ret_session_obj == NULL )
    {
      rc = VS_EBADPARAM;
      break;
    }

    rc = vs_mem_alloc_object( sizeof( vs_session_object_t ),
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }
    checkpoint = 1;

    /* Initialize the session object. */
    {
      p_session_obj->vsid = VS_VSID_UNDEFINED_V;
      p_session_obj->client_id = VS_VOC_CLIENT_UNDEFINED_V;
      p_session_obj->pending_media_id = VS_COMMON_MEDIA_ID_UNDEFINED_V;
      p_session_obj->media_id = VS_COMMON_MEDIA_ID_UNDEFINED_V;

      p_session_obj->event_cb = NULL;
      p_session_obj->session_context = NULL;

      /* Create session lock. */
      rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &p_session_obj->common_lock );
      rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &p_session_obj->client_state_lock );
      rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &p_session_obj->session_ctrl_lock );

      rc = apr_list_init_v2( &p_session_obj->alloc_buf_q, vs_voc_data_buf_lock_fn, vs_voc_data_buf_unlock_fn );

      rc = apr_list_init_v2( &p_session_obj->read_buf_q, vs_voc_data_buf_lock_fn, vs_voc_data_buf_unlock_fn );

      rc = apr_list_init_v2( &p_session_obj->write_buf_q, vs_voc_data_buf_lock_fn, vs_voc_data_buf_unlock_fn );

      ( void ) mmstd_memset( (void*)(&p_session_obj->cvd_session_name), 0,
                             sizeof( p_session_obj->cvd_session_name ) );

      /**
       * Till Resource synchronization is in place initialize this to
       * TRUE by default.
       */
      p_session_obj->is_resource_granted = TRUE;
      p_session_obj->call_state = VS_CALL_STATE_ENUM_RESET;

      /* Initialize voice timing settings */
      p_session_obj->voice_timing.vfr_mode = VS_VOC_VFR_MODE_SOFT_V;
      p_session_obj->voice_timing.vsid = VS_VSID_UNDEFINED_V;
      p_session_obj->voice_timing.enc_offset = VS_VOC_TIMING_GSM_ENC_OFFSET_V;
      p_session_obj->voice_timing.dec_req_offset = VS_VOC_TIMING_GSM_DEC_REQ_OFFSET_V;
      p_session_obj->voice_timing.dec_offset = VS_VOC_TIMING_GSM_DEC_OFFSET_V;

      /* Initialize vocoder properties - current */
      p_session_obj->current_voc.amr.codec_mode = VS_VOCAMR_CODEC_MODE_UNDEFINED_V;
      p_session_obj->current_voc.amr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      p_session_obj->current_voc.amrwb.codec_mode = VS_VOCAMRWB_CODEC_MODE_UNDEFINED_V;
      p_session_obj->current_voc.amrwb.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      p_session_obj->current_voc.efr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.fr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.hr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* EVRC-A */
      p_session_obj->current_voc.evrca.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.evrca.min_rate = ( uint16_t ) VS_VOCEVRCA_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrca.max_rate = ( uint16_t ) VS_VOCEVRCA_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrca.rate_modulation = VS_VOCEVRCA_RATE_MODULATION_UNDEFINED_V;

      /* EVRC-B */
      p_session_obj->current_voc.evrcb.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.evrcb.min_rate = ( uint16_t ) VS_VOCEVRCB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcb.max_rate = ( uint16_t ) VS_VOCEVRCB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcb.rate = VS_VOCEVRCB_RATE_UNDEFINED_V;

      /* EVRC-WB */
      p_session_obj->current_voc.evrcwb.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.evrcwb.min_rate = ( uint16_t ) VS_VOCEVRCWB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcwb.max_rate = ( uint16_t ) VS_VOCEVRCWB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcwb.rate = VS_VOCEVRCWB_RATE_UNDEFINED_V;

      /* EVRC-NW */
      p_session_obj->current_voc.evrcnw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw.min_rate = ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw.max_rate = ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw.rate = VS_VOCEVRCNW_RATE_UNDEFINED_V;

      /* EVRC-NW2K */
      p_session_obj->current_voc.evrcnw2k.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw2k.min_rate = ( uint16_t ) VS_VOCEVRCNW2K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw2k.max_rate = ( uint16_t ) VS_VOCEVRCNW2K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw2k.rate = VS_VOCEVRCNW2K_RATE_UNDEFINED_V;

      /* QCELP-13K */
      p_session_obj->current_voc.qcelp13k.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.qcelp13k.min_rate = ( uint16_t ) VS_VOCQCELP13K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.qcelp13k.max_rate = ( uint16_t ) VS_VOCQCELP13K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.qcelp13k.rate_modulation = VS_VOCQCELP13K_RATE_MODULATION_UNDEFINED_V;
      p_session_obj->current_voc.qcelp13k.rate = VS_VOCQCELP13K_RATE_UNDEFINED_V;

      /* G.729 */
      p_session_obj->current_voc.g729.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* G.711 A-LAW */
      p_session_obj->current_voc.g711alaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* G.711 MU-LAW */
      p_session_obj->current_voc.g711mulaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* Initialize vocoder properties - pending */
      p_session_obj->pending_voc.amr.codec_mode = VS_VOCAMR_CODEC_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.amr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      p_session_obj->pending_voc.amrwb.codec_mode = VS_VOCAMRWB_CODEC_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.amrwb.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      p_session_obj->pending_voc.efr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.fr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.hr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* EVRC - A */
      p_session_obj->pending_voc.evrca.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.evrca.min_rate = ( uint16_t ) VS_VOCEVRCA_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrca.max_rate = ( uint16_t ) VS_VOCEVRCA_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrca.rate_modulation = VS_VOCEVRCA_RATE_MODULATION_UNDEFINED_V;

      /* EVRC-B */
      p_session_obj->pending_voc.evrcb.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcb.min_rate = ( uint16_t ) VS_VOCEVRCB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcb.max_rate = ( uint16_t ) VS_VOCEVRCB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcb.rate = VS_VOCEVRCB_RATE_UNDEFINED_V;

      /* EVRC-WB */
      p_session_obj->pending_voc.evrcwb.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcwb.min_rate = ( uint16_t ) VS_VOCEVRCWB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcwb.max_rate = ( uint16_t ) VS_VOCEVRCWB_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcwb.rate = VS_VOCEVRCWB_RATE_UNDEFINED_V;

      /* EVRC-NW */
      p_session_obj->pending_voc.evrcnw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw.min_rate = ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw.max_rate = ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw.rate = VS_VOCEVRCNW_RATE_UNDEFINED_V;

      /* EVRC-NW2K */
      p_session_obj->pending_voc.evrcnw2k.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw2k.min_rate = ( uint16_t ) VS_VOCEVRCNW2K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw2k.max_rate = ( uint16_t ) VS_VOCEVRCNW2K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw2k.rate = VS_VOCEVRCNW2K_RATE_UNDEFINED_V;

      /* QCELP-13K */
      p_session_obj->pending_voc.qcelp13k.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->pending_voc.qcelp13k.min_rate = ( uint16_t ) VS_VOCQCELP13K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.qcelp13k.max_rate = ( uint16_t ) VS_VOCQCELP13K_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.qcelp13k.rate_modulation = VS_VOCQCELP13K_RATE_MODULATION_UNDEFINED_V;
      p_session_obj->pending_voc.qcelp13k.rate = VS_VOCQCELP13K_RATE_UNDEFINED_V;

      /* G.729 */
      p_session_obj->pending_voc.g729.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* G.711 A-LAW */
      p_session_obj->pending_voc.g711alaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* G.711 MU-LAW */
      p_session_obj->pending_voc.g711mulaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;

      /* TODO the initialization of session control. */
      p_session_obj->session_ctrl.transition_job_handle = APR_NULL_V;
      p_session_obj->session_ctrl.pendjob_handle = APR_NULL_V;
      p_session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET;
      p_session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
      p_session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_NONE;
      p_session_obj->session_ctrl.goal = VS_VOC_GOAL_ENUM_NONE;

      p_session_obj->stream_ready = FALSE;
      p_session_obj->cvs_port = APR_NULL_V;
      p_session_obj->mvm_port =  APR_NULL_V;
      p_session_obj->is_close_invoked = FALSE;
    }

    /* Complete initialization. */
    p_session_obj->header.type = VS_OBJECT_TYPE_ENUM_SESSION;
      /**
       * Mark the actual object type here to indicate that the object
       * has been fully initialized. Otherwise, on an error the
       * destructor called at the checkpoint handler would
       * inadvertently try to free resources in the object that have
       * not been allocated yet or have already been freed by the
       * clean up sequence.
       */

    *p_ret_session_obj = p_session_obj;

    return VS_EOK;
  }  /* end of for ( ;; ) */

  switch ( checkpoint )
  {
  case 1:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_create_session_object(): failed, checkpoint=(%d)", checkpoint );
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_session_obj );
    p_session_obj = NULL;
    /*-fallthru */

  default:
    break;
  }

  return rc;

}  /* end of vs_voc_create_session_object() */


/****************************************************************************
 * VS ROUTIINES FOR CONVERSION AND VALIDATION                               *
 ****************************************************************************/

static uint32_t vs_voc_update_voice_timing_info (
  vs_session_object_t* p_session_obj
)
{
  uint32_t rc = VS_EOK;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "vs_voc_update_voice_timing_info(): client_id=(0X%8X), vsid=(0X%8X)",
         p_session_obj->client_id, p_session_obj->vsid );

  switch ( p_session_obj->client_id )
  {
  case VS_VOC_CLIENT_GSM:
  case VS_VOC_CLIENT_MVS_GSM:
    {
      p_session_obj->voice_timing.vfr_mode = VS_VOC_VFR_MODE_HARD_V;
      p_session_obj->voice_timing.vsid = p_session_obj->vsid;
      p_session_obj->voice_timing.enc_offset = VS_VOC_TIMING_GSM_ENC_OFFSET_V;
      p_session_obj->voice_timing.dec_offset = VS_VOC_TIMING_GSM_DEC_OFFSET_V;
      p_session_obj->voice_timing.dec_req_offset = VS_VOC_TIMING_GSM_DEC_REQ_OFFSET_V;
    }
    break;

  case VS_VOC_CLIENT_CDMA:
  case VS_VOC_CLIENT_CDMA2:
    {
      p_session_obj->voice_timing.vfr_mode = VS_VOC_VFR_MODE_HARD_V;
      p_session_obj->voice_timing.vsid = p_session_obj->vsid;
      p_session_obj->voice_timing.enc_offset = VS_VOC_TIMING_CDMA_ENC_OFFSET_V;
      p_session_obj->voice_timing.dec_offset = VS_VOC_TIMING_CDMA_DEC_OFFSET_V;
      p_session_obj->voice_timing.dec_req_offset = VS_VOC_TIMING_CDMA_DEC_REQ_OFFSET_V;
    }
    break;

  case VS_VOC_CLIENT_WCDMA:
    {
      p_session_obj->voice_timing.vfr_mode = VS_VOC_VFR_MODE_HARD_V;
      p_session_obj->voice_timing.vsid = p_session_obj->vsid;
      p_session_obj->voice_timing.enc_offset = VS_VOC_TIMING_WCDMA_ENC_OFFSET_V;
      p_session_obj->voice_timing.dec_offset = VS_VOC_TIMING_WCDMA_DEC_OFFSET_V;
      p_session_obj->voice_timing.dec_req_offset = VS_VOC_TIMING_WCDMA_DEC_REQ_OFFSET_V;
    }
    break;

  case VS_VOC_CLIENT_TDSCDMA:
    {
      p_session_obj->voice_timing.vfr_mode = VS_VOC_VFR_MODE_HARD_V;
      p_session_obj->voice_timing.vsid = p_session_obj->vsid;
      p_session_obj->voice_timing.enc_offset = VS_VOC_TIMING_TDSCDMA_ENC_OFFSET_V;
      p_session_obj->voice_timing.dec_offset = VS_VOC_TIMING_TDSCDMA_DEC_OFFSET_V;
      p_session_obj->voice_timing.dec_req_offset = VS_VOC_TIMING_TDSCDMA_DEC_REQ_OFFSET_V;
    }
    break;

  case VS_VOC_CLIENT_TEST:
    {
      p_session_obj->voice_timing.vfr_mode = VS_VOC_VFR_MODE_SOFT_V;
      p_session_obj->voice_timing.vsid = p_session_obj->vsid;
      p_session_obj->voice_timing.enc_offset = VS_VOC_TIMING_ENC_OFFSET_V;
      p_session_obj->voice_timing.dec_offset = VS_VOC_TIMING_DEC_OFFSET_V;
      p_session_obj->voice_timing.dec_req_offset = VS_VOC_TIMING_DEC_REQ_OFFSET_V;
    }
    break;

  default:
    {
      rc = VS_EBADPARAM;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_update_voice_timing_info(): Unsupported client_id" );
    }
    break;
  }

  return rc;
}

static uint32_t vs_voc_validate_n_update_active_client (
  uint32_t cmd_id,
  vs_session_object_t* p_session_obj
)
{
  uint32_t rc = VS_EOK;

  switch ( p_session_obj->vsid )
  {
  case VS_VSID_CS_VOICE_MULTIMODE:
    {
      switch( cmd_id )
      {
      case VS_VOC_CMD_DISABLE:
        {
          if ( p_session_obj->client_id == mmode_active_client )
          {
            mmode_active_client = VS_VOC_CLIENT_UNDEFINED_V;
            rc = VS_EOK;
          }
        }
        break;

      case VS_VOC_CMD_ENABLE:
      case VS_VOC_CMD_STANDBY:
        {
          if ( ( mmode_active_client == VS_VOC_CLIENT_UNDEFINED_V ) ||
               ( mmode_active_client == p_session_obj->client_id ) )
          {
            mmode_active_client = p_session_obj->client_id;
            rc = VS_EOK;
          }
          else
          {
            rc = VS_EBUSY;
          }
        }
        break;
      } /* Inner switch. */
    }
    break;

  case VS_VSID_CS_VOICE_GSM2:
   {
     switch( cmd_id )
     {
     case VS_VOC_CMD_DISABLE:
       {
         if ( p_session_obj->client_id == gsm_active_client )
         {
           gsm_active_client = VS_VOC_CLIENT_UNDEFINED_V;
           rc = VS_EOK;
         }
       }
       break;

     case VS_VOC_CMD_ENABLE:
     case VS_VOC_CMD_STANDBY:
       {
         if ( ( gsm_active_client == VS_VOC_CLIENT_UNDEFINED_V ) ||
              ( gsm_active_client == p_session_obj->client_id ) )
         {
           gsm_active_client = p_session_obj->client_id;
           rc = VS_EOK;
         }
         else
         {
           rc = VS_EBUSY;
         }
       }
       break;
     } /* Inner switch. */
   }
   break;

  default:
    break;
  }

  return rc;
}

static uint32_t vs_voc_validate_media_id (
  uint32_t media_id,
  uint32_t client_id
)
{
  uint32_t rc = VS_EBADPARAM;

  switch ( media_id )
  {
   /**
    * TBD: Adding media_id verification for missing vocoder.
    *
    * AI: Shruhti.
    */
  case VS_COMMON_MEDIA_ID_EFR:
  case VS_COMMON_MEDIA_ID_FR:
  case VS_COMMON_MEDIA_ID_HR:
    {
       if ( ( client_id == VS_VOC_CLIENT_GSM ) ||
            ( client_id == VS_VOC_CLIENT_TEST )||
            ( client_id == VS_VOC_CLIENT_MVS_GSM ) )
       {
         rc = VS_EOK;
       }
    }
    break;

  case VS_COMMON_MEDIA_ID_AMR:
  case VS_COMMON_MEDIA_ID_AMRWB:
    {
       if ( ( client_id == VS_VOC_CLIENT_GSM ) ||
            ( client_id == VS_VOC_CLIENT_TEST ) ||
            ( client_id == VS_VOC_CLIENT_TDSCDMA ) ||
            ( client_id == VS_VOC_CLIENT_WCDMA ) ||
            ( client_id == VS_VOC_CLIENT_MVS_GSM ) )
       {
         rc = VS_EOK;
       }
    }
    break;

  case VS_COMMON_MEDIA_ID_EVRCA:
  case VS_COMMON_MEDIA_ID_EVRCB:
  case VS_COMMON_MEDIA_ID_EVRCWB:
  case VS_COMMON_MEDIA_ID_EVRCNW:
  case VS_COMMON_MEDIA_ID_EVRCNW2K:
  case VS_COMMON_MEDIA_ID_QCELP13K:
  case VS_COMMON_MEDIA_ID_G729:
  case VS_COMMON_MEDIA_ID_G711ALAW:
  case VS_COMMON_MEDIA_ID_G711MULAW:
    {
      rc = VS_EOK;
    }

  default:
    break;
  }

  return rc;
}

static uint32_t vs_voc_map_to_cvd_mode (
  uint32_t vs_media_id,
  uint32_t vs_client_id
)
{

  uint32_t media_id = VSS_MEDIA_ID_NONE;

  switch ( vs_media_id )
  {
   /**
    * TBD: Add missing vocoder support.
    * AI: Shruthi.
    */
  case VS_COMMON_MEDIA_ID_UNDEFINED_V:
    {
      media_id = VSS_MEDIA_ID_NONE;
    }
    break;

  case VS_COMMON_MEDIA_ID_AMR:
    {
      if ( ( vs_voicecfg_is_2g_eamr_enabled == TRUE ) &&
           ( ( vs_client_id == VS_VOC_CLIENT_GSM ) ||
             ( vs_client_id == VS_VOC_CLIENT_MVS_GSM ) ) )
      {
        media_id = VSS_MEDIA_ID_EAMR;
      }
      else if ( ( vs_voicecfg_is_3g_eamr_enabled == TRUE ) &&
                 ( vs_client_id == VS_VOC_CLIENT_WCDMA) )
      {
        media_id = VSS_MEDIA_ID_EAMR;
      }
      else if ( ( vs_voicecfg_is_td_eamr_enabled == TRUE ) &&
                ( vs_client_id == VS_VOC_CLIENT_TDSCDMA ) )
      {
        media_id = VSS_MEDIA_ID_EAMR;
      }
      else
      {
        media_id = VSS_MEDIA_ID_AMR_NB;
      }
    }
    break;

  case VS_COMMON_MEDIA_ID_AMRWB:
    {
      media_id = VSS_MEDIA_ID_AMR_WB;
    }
    break;

  case VS_COMMON_MEDIA_ID_EFR:
    {
      media_id = VSS_MEDIA_ID_EFR;
    }
    break;

  case VS_COMMON_MEDIA_ID_FR:
    {
      media_id = VSS_MEDIA_ID_FR;
    }
    break;

  case VS_COMMON_MEDIA_ID_HR:
    {
      media_id = VSS_MEDIA_ID_HR;
    }
    break;

  case VS_COMMON_MEDIA_ID_EVRCA:
    {
      media_id = VSS_MEDIA_ID_EVRC;
    }
    break;

  case VS_COMMON_MEDIA_ID_EVRCB:
    {
      media_id = VSS_MEDIA_ID_4GV_NB;
    }
    break;

  case VS_COMMON_MEDIA_ID_EVRCWB:
    {
      media_id = VSS_MEDIA_ID_4GV_WB;
    }
    break;

  case VS_COMMON_MEDIA_ID_EVRCNW:
    {
      media_id = VSS_MEDIA_ID_4GV_NW;
    }
    break;

  case VS_COMMON_MEDIA_ID_EVRCNW2K:
    {
      media_id = VSS_MEDIA_ID_4GV_NW2K;
    }
    break;

  case VS_COMMON_MEDIA_ID_QCELP13K:
    {
      media_id = VSS_MEDIA_ID_13K;
    }
    break;

  case VS_COMMON_MEDIA_ID_G729:
    {
      media_id = VSS_MEDIA_ID_G729;
    }
    break;

  case VS_COMMON_MEDIA_ID_G711ALAW:
    {
      media_id = VSS_MEDIA_ID_G711_ALAW;
    }
    break;

  case VS_COMMON_MEDIA_ID_G711MULAW:
    {
      media_id = VSS_MEDIA_ID_G711_MULAW;
    }
    break;

  default:
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "Received vs_media_id not supported currently..." );
    media_id = VSS_MEDIA_ID_NONE;
    break;
  }

  return media_id;
}  /* end of vs_voc_map_to_cvd_mode () */

static uint32_t vs_voc_map_to_cvd_network (
  uint32_t client
)
{
  uint32_t network_id;

  switch ( client )
  {
  case VS_VOC_CLIENT_GSM:
  case VS_VOC_CLIENT_MVS_GSM:
    {
      network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
    }
    break;

  case VS_VOC_CLIENT_CDMA:
  case VS_VOC_CLIENT_CDMA2:
    {
      network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
    }
    break;

  case VS_VOC_CLIENT_WCDMA:
    {
      network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
    }
    break;

  case VS_VOC_CLIENT_TDSCDMA:
    {
      network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
    }
    break;

  default:
    network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
    break;
  }

  return network_id;

}  /* end of vs_map_to_cvd_network () */

/****************************************************************************
 * VS COMMAND AND RESPONSE PROCESSING ROUTINES                             *
 ****************************************************************************/

/**
 * These would take care of free the SIMPLE_JOB object created o
 * track the trasition.
 */
static void vs_voc_create_mvm_session_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* p_simple_obj = NULL;
  vs_session_object_t* p_session_obj = NULL;
  aprv2_ibasic_rsp_result_t* p_rsp_result = NULL;

  for ( ;; )
  {
    if ( p_packet == APR_NULL_V ) 
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }
    
    rc = vs_get_typed_object( p_packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &p_simple_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      ( void ) __aprv2_cmd_free( vs_apr_handle, p_packet );
      break;
    }

    p_rsp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, p_packet );    
    p_simple_obj->is_completed = TRUE;
    p_simple_obj->status = p_rsp_result->status;

    rc = vs_get_typed_object( p_simple_obj->context_handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    VS_REPORT_FATAL_ON_ERROR( rc );     

    /* Stop the timer triggered for ADSP/CVD timeout. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_voc_create_mvm_session_result_rsp_fn(): "
           "apr_timer_stop counter = %d", timer_stop_cntr++ );
    apr_timer_stop( vs_voc_adsp_timeout_timer );

    /* Q CMD for VS cleanup after ADSP/CVD recovers post timeout. */
    if ( vs_is_adsp_timeout_triggered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_create_mvm_session_result_rsp_fn(): ADSP recovered "
           "post timeout" );

      rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
              VS_VOC_CMDI_DISBALE_CVD_ON_ADSP_RECOVERY, NULL, 0 );
      vs_is_adsp_timeout_triggered = FALSE;
    }
    

    if ( p_session_obj != NULL )
    {
      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      p_session_obj->session_ctrl.status = p_simple_obj->status;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      if ( p_simple_obj->status == APR_EOK )
      {
        p_session_obj->mvm_port = p_packet->src_port;
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "MVM_PORT (%d)  set for session_handle=(0x%08x)",
               p_session_obj->mvm_port, p_session_obj->header.handle );
      }
      else
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_create_mvm_session_rsp_result_fn(): command (0x%08x) "
               "failed with rc=(0x%08x)", p_rsp_result->opcode,
               p_rsp_result->status  );
      }
    }

    /* Free the APR command response packet. */
    rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
    VS_REPORT_FATAL_ON_ERROR( rc );

    /* Free the simple job object. */
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );

    break;
  }

  vs_signal_run_thrd3();
  return;
}  /* end of vs_voc_create_mvm_session_result_rsp_fn () */

/**
 * These would take care of free the SIMPLE_JOB object created o
 * track the trasition.
 */
static void vs_voc_create_cvs_session_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* p_simple_obj = NULL;
  vs_session_object_t* p_session_obj = NULL;
  aprv2_ibasic_rsp_result_t* p_rsp_result = NULL;

  for ( ;; )
  {
    if ( p_packet == APR_NULL_V ) 
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    rc = vs_get_typed_object( p_packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &p_simple_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      ( void ) __aprv2_cmd_free( vs_apr_handle, p_packet );
      break;
    }

    p_rsp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, p_packet );
    p_simple_obj->is_completed = TRUE;
    p_simple_obj->status = p_rsp_result->status;

    rc = vs_get_typed_object( p_simple_obj->context_handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                            ( ( vs_object_t** ) &p_session_obj ) );
    VS_REPORT_FATAL_ON_ERROR( rc );

    /* Stop the timer triggered for ADSP/CVD timeout. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_voc_create_cvs_session_result_rsp_fn(): "
           "apr_timer_stop counter = %d", timer_stop_cntr++ );
    apr_timer_stop( vs_voc_adsp_timeout_timer );

    /* Q CMD for VS cleanup after ADSP/CVD recovers post timeout. */
    if ( vs_is_adsp_timeout_triggered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_create_cvs_session_result_rsp_fn(): ADSP recovered "
           "post timeout" );

      rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
              VS_VOC_CMDI_DISBALE_CVD_ON_ADSP_RECOVERY, NULL, 0 );
      vs_is_adsp_timeout_triggered = FALSE;
    }

    if ( p_session_obj != NULL )
    {
      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      p_session_obj->session_ctrl.status = p_rsp_result->status;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      if ( p_rsp_result->status == APR_EOK )
       {
         p_session_obj->cvs_port = p_packet->src_port;
         MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "CVS_PORT (%d)  set for session_handle=(0x%08x)",
                p_session_obj->cvs_port, p_session_obj->header.handle );
      }
      else
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_create_mvm_session_rsp_result_fn(): command (0x%08x) "
               "failed with rc=(0x%08x)", p_rsp_result->opcode,
               p_rsp_result->status  );
      }
    }

    rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
    VS_REPORT_FATAL_ON_ERROR( rc );

    /* Free the simple job object. */
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );

    break;
  }

  vs_signal_run_thrd3();
  return;
}  /* end of vs_voc_create_cvs_session_result_rsp_fn () */


/****************************************************************************
 * CVS STATE MACHINE TRANSITION ACTION FUNCTIONS                            *
 ****************************************************************************/


static uint32_t vs_voc_action_create_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_create_mvm_session_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_create_mvm(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                          VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_create_mvm(): apr_timer_start failed=(0X%08X) ", rc  );
  }
#endif

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, APR_NULL_V,
         job_obj->header.handle, VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION,
         session_obj->cvd_session_name,
         sizeof( session_obj->cvd_session_name ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_create_mvm(): failed with result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

static uint32_t vs_voc_action_destroy_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] =
              vs_simple_transition_result_rsp_fn;

  if ( session_obj->mvm_port == APR_NULL_V )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_voc_action_destroy_mvm(): NULL MVM PORT" );
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    /* Lock not needed as cmd is not sent yet */
    session_obj->session_ctrl.status = APR_EOK;
    return APR_EIMMEDIATE;
  }

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_destroy_mvm(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                         VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_destroy_mvm(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, APRV2_IBASIC_CMD_DESTROY_SESSION,
         NULL, 0 );
  if (rc)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_destroy_mvm(): failed with result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

static uint32_t vs_voc_action_create_cvs (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* p_job_obj = NULL;
  vss_istream_cmd_create_full_control_session_t* cvs_full_ctrl = NULL;
  aprv2_packet_t* p_packet = NULL;
  char_t* p_stream_name = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &p_job_obj );

  p_job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] =
     vs_voc_create_cvs_session_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = p_job_obj->header.handle;

  rc = __aprv2_cmd_alloc_ext(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, APR_NULL_V,
         p_job_obj->header.handle, VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION,
         ( sizeof( vss_istream_cmd_create_full_control_session_t ) +
           sizeof( session_obj->cvd_session_name ) ), &p_packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  if( ( p_packet == NULL ) || ( rc ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_create_cvs(): APR packet alloc failed  with "
           "result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_job_obj );    

    return APR_EIMMEDIATE;
  }

  cvs_full_ctrl = APRV2_PKT_GET_PAYLOAD(
                    vss_istream_cmd_create_full_control_session_t, p_packet );

  cvs_full_ctrl->direction = 2;
  cvs_full_ctrl->dec_media_type = VSS_MEDIA_ID_NONE;
  cvs_full_ctrl->enc_media_type = VSS_MEDIA_ID_NONE;
  cvs_full_ctrl->network_id = VSS_NETWORK_ID_DEFAULT;

  p_stream_name =
    ( char* ) APR_PTR_END_OF( cvs_full_ctrl,
                sizeof( vss_istream_cmd_create_full_control_session_t ) );

  mmstd_memcpy( p_stream_name, sizeof( session_obj->cvd_session_name ),
                session_obj->cvd_session_name,
                sizeof( session_obj->cvd_session_name ) );

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "vs_voc_action_create_cvs(): apr_timer_start counter = %d ",
         timer_cntr++ );

  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                          VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_create_cvs(): apr_timer_start failed=(0X%08X) ", rc  );
  }
#endif

  rc = __aprv2_cmd_forward( vs_apr_handle, p_packet );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_create_cvs(): failed with result=(0X%08X)", rc );
    
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 ) apr_timer_stop( vs_voc_adsp_timeout_timer );

    /* Free the APR PKT and the JOB object. */
    rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
    VS_REPORT_FATAL_ON_ERROR( rc );

    ( void ) vs_mem_free_object( ( vs_object_t* ) p_job_obj );

    return APR_EIMMEDIATE;
  }
  
  return APR_EOK;
}

static uint32_t vs_voc_action_destroy_cvs (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] =
              vs_simple_transition_result_rsp_fn;

  if ( session_obj->cvs_port == APR_NULL_V )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_voc_action_destroy_cvs(): NULL CVS PORT" );
    /* Lock not needed as cmd isn't sent yet */
    session_obj->session_ctrl.status = rc;

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_destroy_cvs(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                          VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_destroy_cvs(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, APRV2_IBASIC_CMD_DESTROY_SESSION,
         NULL, 0 );
  if (rc)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_destroy_cvs(): failed with result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

static uint32_t vs_voc_action_attach_stream (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] =
                     vs_simple_transition_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_attach_stream(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                         VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_attach_stream(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_ATTACH_STREAM,
         &session_obj->cvs_port, sizeof( session_obj->cvs_port ) );
  if (rc)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_attach_stream(): failed with result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

static uint32_t vs_voc_action_detach_stream (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] =
                     vs_simple_transition_result_rsp_fn;

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_detach_stream(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                         VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_detach_stream(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_DETACH_STREAM,
         &session_obj->cvs_port, sizeof( session_obj->cvs_port ) );
  if (rc)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_detach_stream(): failed with result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  return APR_EOK;
}

static uint32_t vs_voc_action_listen_eamr (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t _rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;
  vss_inotify_cmd_listen_for_event_class_t ctrl_args;
  voicecfg_cmd_get_bool_item_t item;

  item.id = VOICECFG_CFG_IS_EAMR_REPORT_MODE_CHANGE_ENABLED;
  item.ret_value = FALSE;
  _rc = voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof ( item ) );

  if ( ( !_rc ) && ( item.ret_value == TRUE ) )
  {
    rc = vs_create_autofree_token_object( &p_token_obj );

    ctrl_args.class_id = VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE;
    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
           vs_cvs_addr, session_obj->cvs_port,
           p_token_obj->header.handle, VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS,
           &ctrl_args, sizeof( ctrl_args ) );
    if (rc)
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_action_listen_eamr() - Unable to send command  "
             "over APR rc=(0x%08x), cvd_service_up=(0x%08x)",
             rc, vs_is_cvd_service_up );
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
    }
  }

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.status = APR_EOK;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );
  return APR_EIMMEDIATE;
}

static uint32_t vs_voc_action_set_media_type_on_stream (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj;
  vss_istream_cmd_set_media_type_t set_media_type;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] =
                     vs_simple_transition_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  set_media_type.rx_media_id = vs_voc_map_to_cvd_mode( session_obj->media_id,
                                                       session_obj->client_id );
  set_media_type.tx_media_id = vs_voc_map_to_cvd_mode( session_obj->media_id,
                                                       session_obj->client_id );

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_set_media_type_on_stream(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                         VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_set_media_type_on_stream(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_ISTREAM_CMD_SET_MEDIA_TYPE,
         &set_media_type, sizeof( set_media_type) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_set_media_type_on_stream() - Unable to send command "
           " over APR rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    /* Lock not needed as cmd failed & no response */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}


/**
 * Set network on MVM to be used for calibrating the attached
 * stream/vocproc.
 */
static uint32_t vs_voc_action_set_network_type_on_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;
  vss_imvm_cmd_set_cal_network_t set_network;

  rc = vs_create_autofree_token_object( &p_token_obj );

  set_network.network_id = vs_voc_map_to_cvd_network( session_obj->client_id );

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         p_token_obj->header.handle, VSS_IMVM_CMD_SET_CAL_NETWORK,
         &set_network, sizeof( set_network ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_set_network_type_on_mvm() - Unable to send command "
           " over APR rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
  }

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.status = APR_EOK;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );
  return APR_EIMMEDIATE;
}


static uint32_t vs_voc_action_set_media_type_on_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;
  vss_imvm_cmd_set_cal_media_type_t set_media_type;

  rc = vs_create_autofree_token_object( &p_token_obj );

  set_media_type.media_id = vs_voc_map_to_cvd_mode( session_obj->media_id,
                                                    session_obj->client_id );

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         p_token_obj->header.handle, VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE,
         &set_media_type, sizeof( set_media_type ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_set_media_type_on_mvm() - Unable to send command "
           " over APR rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
  }
  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.status = APR_EOK;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );

  return APR_EIMMEDIATE;
}


static uint32_t vs_voc_action_set_voice_timing_params (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;
  vss_icommon_cmd_set_voice_timing_v2_t timing_param_v2;

  rc = vs_create_autofree_token_object( &p_token_obj );

  timing_param_v2.mode = session_obj->voice_timing.vfr_mode;
  timing_param_v2.vsid = session_obj->voice_timing.vsid;
  timing_param_v2.enc_offset = session_obj->voice_timing.enc_offset;
  timing_param_v2.dec_req_offset = session_obj->voice_timing.dec_req_offset;
  timing_param_v2.dec_offset = session_obj->voice_timing.dec_offset;;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_voc_set_voice_timing_proc()-  handle=(0x%08x), vsid=(0x%08x),"
         " vfr_mode=(%d)", session_obj->header.handle, timing_param_v2.vsid,
         timing_param_v2.mode );
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_voc_set_voice_timing_proc()-  enc_off=(0x%08x), dec_req_off=(0x%08x),"
         "dec_off=(0x%08x)", timing_param_v2.enc_offset,
         timing_param_v2.dec_req_offset, timing_param_v2.dec_offset );

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         p_token_obj->header.handle, VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2,
         &timing_param_v2, sizeof( timing_param_v2 ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_set_voice_timing_params() - Unable to send command "
           " over APR rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
  }

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.status = APR_EOK;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );
  return APR_EIMMEDIATE;
}


VS_INTERNAL vs_voc_client_type_t vs_voc_check_client_type (
  uint32_t client_id
)
{
  vs_voc_client_type_t client_type;

  switch ( client_id )
  {
    case VS_VOC_CLIENT_GSM:
    case VS_VOC_CLIENT_CDMA:
    case VS_VOC_CLIENT_CDMA2:
        client_type = VS_VOC_CLIENT_TYPE_MODEM;
        break;

    case VS_VOC_CLIENT_WCDMA:
    case VS_VOC_CLIENT_TDSCDMA:
    case VS_VOC_CLIENT_TEST:
    case VS_VOC_CLIENT_MVS_GSM:
        client_type = VS_VOC_CLIENT_TYPE_MVS;
        break;

    default:
        client_type = VS_VOC_CLIENT_TYPE_INVALID;
        break;
  }
  return client_type;
}

VS_INTERNAL uint32_t vs_voc_action_set_cached_stream_properties (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  /* Don't need lock protection for media_id since update is sequential */
  switch( session_obj->media_id )
  {
  case VS_COMMON_MEDIA_ID_FR:
    rc = vs_vocfr_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_HR:
    rc = vs_vochr_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_EFR:
    rc = vs_vocefr_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_AMR:
    rc = vs_vocamr_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_AMRWB:
    rc = vs_vocamrwb_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_EVRCA:
    rc = vs_vocevrca_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_EVRCB:
    rc = vs_vocevrcb_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_EVRCWB:
    rc = vs_vocevrcwb_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_EVRCNW:
    rc = vs_vocevrcnw_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_EVRCNW2K:
    rc = vs_vocevrcnw2k_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_QCELP13K:
    rc = vs_vocqcelp13k_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_G729:
    rc = vs_vocg729_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_G711ALAW:
    rc = vs_vocg711alaw_set_cached_voc_property( session_obj->header.handle );
    break;

  case VS_COMMON_MEDIA_ID_G711MULAW:
    rc = vs_vocg711mulaw_set_cached_voc_property( session_obj->header.handle );
    break;

  default:
    break;
  }

  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_set_cached_stream_properties(): failed with "
           "result=(0X%08X)", rc );
  }

  return APR_EIMMEDIATE;
}

static uint32_t vs_voc_action_send_resync_ctm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;


  rc = vs_create_autofree_token_object( &p_token_obj );

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         p_token_obj->header.handle, VSS_ISTREAM_CMD_RESYNC_CTM,
         NULL, 0 );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_send_resync_ctm() - Unable to send command over APR "
           "rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
  }

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.status = APR_EOK;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );
  return APR_EIMMEDIATE;
}

static uint32_t vs_voc_action_send_start_voice (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] =
                     vs_simple_transition_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_send_start_voice(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                         VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_send_start_voice(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_MODEM_START_VOICE,
         NULL, 0 );
  if (rc)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_send_start_voice() - Unable to send command over APR "
           "rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

static uint32_t vs_voc_action_send_stop_voice (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t rc2 = APR_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] =
                     vs_simple_transition_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#ifndef WINSIM
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_action_send_stop_voice(): apr_timer_start counter = %d ", timer_cntr++ );
  rc2 = apr_timer_start ( vs_voc_adsp_timeout_timer,
                         VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS );
  if ( rc2 )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_action_send_stop_voice(): apr_timer_start "
          "failed=(0X%08X) ", rc  );
  }
#endif
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_MODEM_STOP_VOICE,
         NULL, 0 );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_send_stop_voice() - Unable to send command over APR "
           "rc=(0x%08x), cvd_service_up=(0x%08x)",
           rc, vs_is_cvd_service_up );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    /* Stop the timer triggered for ADSP/CVD timeout. */
    if ( !rc2 )
      apr_timer_stop( vs_voc_adsp_timeout_timer );

    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}


/****************************************************************************
 * VS VOICE SESSION HELPER FUNCTIONS                                        *
 ****************************************************************************/

static int32_t vs_voc_helper_create_new_goal_control (
  vs_voc_goal_enum_t new_goal,
  vs_session_object_t* session_obj,
  vs_object_t** ret_job_obj
)
{
  int32_t rc;
  vs_simple_job_object_t* job_obj;

  if ( ( ret_job_obj == NULL ) || ( session_obj == NULL ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_helper_create_new_goal_control(): NULL params passed "
           "ctrl=(0x%08x) and session_obj=(0x%08x)", ret_job_obj,
           session_obj );
    VS_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
    return APR_EUNEXPECTED;
  }

  if ( session_obj->session_ctrl.goal != VS_VOC_GOAL_ENUM_NONE )
  {
    /* New Goal cannot be created until previous GOAL is completed. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_helper_create_new_goal_control(): Cannot create new Goal "
           "current goal=(0x%08x)", session_obj->session_ctrl.goal );
    VS_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }
  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.goal = new_goal;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

    /* The goal is reset by the state machine on goal completion. */
  session_obj->session_ctrl.pendjob_handle = job_obj->header.handle;
    /* The pendjob_handle will signal completion. The pendjob_handle is
     * reset by the state machine on goal completion.
     */
  *ret_job_obj = ( ( vs_object_t* ) job_obj );
    /* sequencer_job.subjob_obj stores the job_obj to be checked for
     * goal completion and to be freed by the current gating command
     * control.
     */

  return APR_EOK;
}

static int32_t vs_voc_do_complete_goal (
  vs_session_object_t* session_obj
)
{
  int32_t rc =  APR_EOK;
  aprv2_ibasic_rsp_result_t res;

  /* Complete the pending command and stay in the same state. */
  res.opcode = APR_UNDEFINED_ID_V;
  /* Lock not needed as response is not expected */
  res.status = session_obj->session_ctrl.status;
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ), /* This src_port is used for debugging only. */
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ), /* This dst_port is used for debugging only. */
         session_obj->session_ctrl.pendjob_handle,
         APRV2_IBASIC_RSP_RESULT, &res, sizeof( res ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_do_complete_goal(): Sending response failed with "
           "result=0x%08X", rc );
    /* Inject ERR_FATAL as the VS gets into non recoverable state. */
    VS_PANIC_ON_ERROR( rc );
  }

  session_obj->session_ctrl.transition_job_handle = APR_NULL_V;
  session_obj->session_ctrl.pendjob_handle = APR_NULL_V;
  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.goal = VS_VOC_GOAL_ENUM_NONE;
  session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );
  session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_NONE;

  return APR_EOK;
}

/**
 * Helper State to modularize the CVD access.
 */
enum {
  VS_HELPER_SEQUENCER_ENUM_UNINITIALIZED,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY_WAIT,
  VS_HELPER_SEQUENCER_ENUM_COMPLETE,
  VS_HELPER_SEQUENCER_ENUM_INVALID
};

static uint32_t vs_voc_helper_vocoder_setup_ctrl (
  vs_sequencer_job_object_t* p_seq_obj,
  vs_session_object_t* p_session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* p_simple_obj = NULL;

  for ( ;; )
  {
    switch ( p_seq_obj->state )
    {
    case VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE:
      {
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_CREATE, p_session_obj,
               &p_seq_obj->subjob_obj );
        if ( rc )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          p_seq_obj->status = rc;
        }
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE_WAIT:
      {
        p_simple_obj = &p_seq_obj->subjob_obj->simple_job;

        if ( p_simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( p_session_obj );
          rc = APR_EPENDING;
          break;
        }
        p_seq_obj->status = p_simple_obj->status;

        /**
         * Initialize sequencer to appropiate state.
         */
        if ( ( vs_get_call_state( p_session_obj ) == VS_CALL_STATE_ENUM_RESET ) ||
             ( p_session_obj->is_resource_granted == FALSE ) ||
             ( vs_is_cvd_service_up == FALSE ) ||
             ( vs_is_cvd_cleanup_done == FALSE ) ||
             ( p_seq_obj->status != APR_EOK ) )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "vs_voc_helper_vocoder_setup_ctrl(): current seq state=(%d)",
                 p_seq_obj->state );
        }
        else
        {
          VS_ACQUIRE_LOCK( p_session_obj->common_lock );
          p_session_obj->media_id = p_session_obj->pending_media_id;
          VS_RELEASE_LOCK( p_session_obj->common_lock );
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE;
        }

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE:
      {
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_DISABLE, p_session_obj,
               &p_seq_obj->subjob_obj );
        if ( rc )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          p_seq_obj->status = rc;
        }
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE_WAIT:
      {
        p_simple_obj = &p_seq_obj->subjob_obj->simple_job;

        if ( p_simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( p_session_obj );
          rc = APR_EPENDING;
          break;
        }
        p_seq_obj->status = p_simple_obj->status;

        /**
         * Initialize sequencer to appropiate state.
         */
        if ( ( vs_is_cvd_service_up == FALSE ) ||
             ( vs_is_cvd_cleanup_done == FALSE ) )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "vs_voc_helper_vocoder_setup_ctrl(): current seq state=(%d)",
                 p_seq_obj->state );
        }
        else if ( ( vs_get_call_state( p_session_obj ) == VS_CALL_STATE_ENUM_RESET ) ||
                  ( p_session_obj->is_resource_granted == FALSE ) )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY;
        }
        else
        {
          VS_ACQUIRE_LOCK( p_session_obj->common_lock );
	  if ( p_session_obj->pending_media_id != p_session_obj->media_id )
          {
            p_session_obj->media_id = p_session_obj->pending_media_id;
            VS_RELEASE_LOCK( p_session_obj->common_lock );
            p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE;
          }
          else
          {
            VS_RELEASE_LOCK( p_session_obj->common_lock );
            p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "vs_voc_helper_vocoder_setup_ctrl(): current seq state=(%d)",
                 p_seq_obj->state );
          }
        }

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE:
      {
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_CONFIGURE, p_session_obj,
               &p_seq_obj->subjob_obj );
        if ( rc )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          p_seq_obj->status = rc;
        }
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE_WAIT:
      {
        p_simple_obj = &p_seq_obj->subjob_obj->simple_job;

        if ( p_simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( p_session_obj );
          rc = APR_EPENDING;
          break;
        }
        p_seq_obj->status = p_simple_obj->status;

        /**
         * Initialize sequencer to appropiate state.
         */
        if ( ( vs_get_call_state( p_session_obj ) != VS_CALL_STATE_ENUM_ENABLED ) ||
             ( p_session_obj->is_resource_granted == FALSE ) ||
             ( vs_is_cvd_service_up ==  FALSE ) ||
             ( vs_is_cvd_cleanup_done == FALSE ) )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "vs_voc_helper_vocoder_setup_ctrl(): current seq state=(%d)",
                 p_seq_obj->state );
        }
        else
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE;
        }

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE:
      {
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_ENABLE, p_session_obj,
               &p_seq_obj->subjob_obj );
        if ( rc )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          p_seq_obj->status = rc;
        }
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE_WAIT:
      {
        p_simple_obj = &p_seq_obj->subjob_obj->simple_job;

        if ( p_simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( p_session_obj );
          rc = APR_EPENDING;
          break;
        }

        p_seq_obj->status = p_simple_obj->status;
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY:
      {
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY_WAIT;
        rc = vs_voc_helper_create_new_goal_control(
               VS_VOC_GOAL_ENUM_DESTROY, p_session_obj,
               &p_seq_obj->subjob_obj );
        if ( rc )
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
          p_seq_obj->status = rc;
        }
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY_WAIT:
      {
        p_simple_obj = &p_seq_obj->subjob_obj->simple_job;

        if ( p_simple_obj->is_completed == FALSE )
        {
          ( void ) vs_voc_state_control( p_session_obj );
          rc = APR_EPENDING;
          break;
        }

        p_seq_obj->status = p_simple_obj->status;
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
      }
      continue;

    case VS_HELPER_SEQUENCER_ENUM_COMPLETE:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_helper_vocoder_setup_ctrl(): cmd status=(0X%08X)",
               p_seq_obj->status );
        rc = APR_EOK;
      }
      break;

    default:
      VS_PANIC_ON_ERROR( APR_EUNEXPECTED );
    } /* swtich case ends here. */

    break;
  } /* For( ;; ) loop ends here. */

  return rc;
}

static void vs_voc_log_state_info(
  vs_session_object_t* session_obj
)
{
  static vs_voc_state_enum_t prev_state = VS_VOC_STATE_ENUM_UNINITIALIZED;
  static vs_voc_goal_enum_t prev_goal =  VS_VOC_GOAL_ENUM_UNINITIALIZED;
  static vs_voc_action_enum_t prev_action = VS_VOC_ACTION_ENUM_UNINITIALIZED;
  vs_voc_state_enum_t cur_state;
  vs_voc_goal_enum_t cur_goal;
  vs_voc_action_enum_t cur_action;

  if ( session_obj == NULL) return;

  /* Lock session ctrl obj. */
  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  cur_state = session_obj->session_ctrl.state;
  cur_goal = session_obj->session_ctrl.goal;
  cur_action = session_obj->session_ctrl.action;
  /* Release session ctrl lock */
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );

  /* Show new state information. */
  if ( ( prev_state != cur_state ) ||
       ( prev_goal != cur_goal ) ||
       ( prev_action != cur_action ) )
  {
    /* Update information. */
    prev_state = cur_state;
    prev_goal = cur_goal;
    prev_action = cur_action;

    switch ( cur_goal )
    {
    case VS_VOC_GOAL_ENUM_NONE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_NONE" );
      break;

    case VS_VOC_GOAL_ENUM_CREATE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_CREATE" );
      break;

    case VS_VOC_GOAL_ENUM_DESTROY:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_DESTROY" );
      break;

    case VS_VOC_GOAL_ENUM_ENABLE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_ENABLE" );
      break;

    case VS_VOC_GOAL_ENUM_CONFIGURE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_CONFIGURE" );
      break;

    case VS_VOC_GOAL_ENUM_DISABLE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_DISABLE" );
      break;

    default:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_GOAL_ENUM_INVALID" );
      break;
    }

    switch ( cur_state )
    {
    case VS_VOC_STATE_ENUM_RESET_ENTRY:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_RESET_ENTRY" );
      break;

    case VS_VOC_STATE_ENUM_RESET:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_RESET" );
      break;

    case VS_VOC_STATE_ENUM_IDLE_ENTRY:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_IDLE_ENTRY" );
      break;

    case VS_VOC_STATE_ENUM_IDLE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_IDLE" );
      break;

    case VS_VOC_STATE_ENUM_RUN_ENTRY:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_RUN_ENTRY" );
      break;

    case VS_VOC_STATE_ENUM_RUN:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_RUN" );
      break;

    default:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_STATE_ENUM_INVALID" );
      break;
    }

    switch ( cur_action )
    {
    case VS_VOC_ACTION_ENUM_NONE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_NONE" );
      break;

    case VS_VOC_ACTION_ENUM_COMPLETE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_COMPLETE" );
      break;

    case VS_VOC_ACTION_ENUM_CONTINUE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_CONTINUE" );
      break;

    case VS_VOC_ACTION_ENUM_START_VOICE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_START_VOICE" );
      break;

    case VS_VOC_ACTION_ENUM_STOP_VOICE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_STOP_VOICE" );
      break;

    case VS_VOC_ACTION_ENUM_STANDBY_VOICE:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_STANDBY_VOICE" );
      break;

    case VS_VOC_ACTION_ENUM_CREATE_MVM:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_CREATE_MVM" );
      break;

    case VS_VOC_ACTION_ENUM_DESTROY_MVM:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_DESTROY_MVM" );
      break;

    case VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM" );
      break;

    case VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM" );
      break;

    case VS_VOC_ACTION_ENUM_SET_VOICE_TIMING:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_SET_VOICE_TIMING" );
      break;

    case VS_VOC_ACTION_ENUM_CREATE_CVS:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_CREATE_CVS" );
      break;

    case VS_VOC_ACTION_ENUM_DESTROY_CVS:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_DESTROY_CVS" );
      break;

    case VS_VOC_ACTION_ENUM_ATTACH_CVS:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_ATTACH_CVS" );
      break;

    case VS_VOC_ACTION_ENUM_DETACH_CVS:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_DETACH_CVS" );
      break;

    case VS_VOC_ACTION_ENUM_LISTEN_EAMR:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_LISTEN_EAMR" );
      break;

    case VS_VOC_ACTION_ENUM_CANCEL_LISTEN_EAMR:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_CANCEL_LISTEN_EAMR" );
      break;

    case VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_OVER_CVS:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_OVER_CVS" );
      break;

    case VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES" );
      break;

    case VS_VOC_ACTION_ENUM_RESYNC_CTM:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_RESYNC_CTM" );
      break;

    default:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS_VOC_ACTION_ENUM_INVALID" );
      break;
    }
  }

  return;
}

/****************************************************************************
 * VS VOCODER STATE MACHINE CONTROL FUNCTIONS                               *
 ****************************************************************************/

static int32_t vs_voc_state_reset_entry (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  uint32_t status;

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  status = session_obj->session_ctrl.status;
  /* Release session ctrl lock */
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );

  /**
   * End the goal the current for below Scenarios:
   * - If CVD/Audio-PD is DOWN or ADSP SSR triggered.
   */
  if ( ( vs_is_cvd_service_up == FALSE )  ||
       ( vs_is_adsp_timeout_triggered == TRUE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_state_reset_entry(): vs_is_adsp_timeout_triggered=(%d), "
           "vs_is_cvd_service_up=(%d)", vs_is_adsp_timeout_triggered,
           vs_is_cvd_service_up );
    /* Lock not needed as response is not expected */
    session_obj->session_ctrl.status = APR_ENOSERVICE;
    rc = vs_voc_do_complete_goal( session_obj );
  }
  else if ( status == APR_UNDEFINED_ID_V )
  {
    /* Stay in this state till response for
       last action is available. */
    return APR_EOK;
  }
  else if ( status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_state_reset_entry(): last action failed with result=(0X%08X)",
           status );
    rc = vs_voc_do_complete_goal( session_obj );
  }
  /* Lock not needed as goal isn't none */
  session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET;
  return APR_EIMMEDIATE;
}


static int32_t vs_voc_state_reset (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  switch ( session_obj->session_ctrl.goal )
  {
  case VS_VOC_GOAL_ENUM_NONE:
    break;

  case VS_VOC_GOAL_ENUM_CREATE:
    {
      /**
       * Create MVM/CVS sessions handles.
       * ( RESET to IDLE transition ).
       */
      switch ( session_obj->session_ctrl.action )
      {
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_CREATE_MVM:
        {
          /* Lock not needed for state as goal isn't none */
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CREATE_CVS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_create_mvm( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_CREATE_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_ATTACH_CVS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_create_cvs( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_ATTACH_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_LISTEN_EAMR;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_attach_stream( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_LISTEN_EAMR:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_NONE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc  = vs_voc_action_listen_eamr( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

  case VS_VOC_GOAL_ENUM_DESTROY:
    {
      switch ( session_obj->session_ctrl.action)
      {
      case VS_VOC_ACTION_ENUM_CONTINUE:
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_DESTROY_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_DESTROY_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_destroy_cvs( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_DESTROY_MVM:
        {
          session_obj->cvs_port = APR_NULL_V;
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_destroy_mvm( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          session_obj->mvm_port = APR_NULL_V;
          session_obj->session_ctrl.status = APR_EOK;
          rc = vs_voc_do_complete_goal( session_obj );
        }

      default:
        break;
      }
    }
    break;

  default:
    break;
  }

  return APR_EOK;
}


static int32_t vs_voc_state_idle_entry (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  uint32_t status;

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  status = session_obj->session_ctrl.status;
  /* Release session ctrl lock */
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );

  /**
   * End the goal the current for below Scenarios:
   *
   * - IF CVD/AUDIO-PD is down or ADSP SSR is trigerred.
   */
  if ( ( vs_is_cvd_service_up == FALSE )  ||
       ( vs_is_adsp_timeout_triggered == TRUE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_state_idle_entry(): last action "
           "vs_is_adsp_timeout_triggered=(%d), vs_is_cvd_service_up=(%d)",
           vs_is_adsp_timeout_triggered, vs_is_cvd_service_up );
    session_obj->session_ctrl.status = APR_ENOSERVICE;
    rc = vs_voc_do_complete_goal( session_obj );
  }
  else if ( status == APR_UNDEFINED_ID_V )
  {
    /* Stay in this state till response for
       last action is available. */
    return APR_EOK;
  }
  else if ( status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_state_idle_entry(): last action failed with "
           "result=(0X%08X)", status );
  }

  session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
  return APR_EIMMEDIATE;
}


static int32_t vs_voc_state_idle (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  switch ( session_obj->session_ctrl.goal )
  {
  case VS_VOC_GOAL_ENUM_NONE:
    break;

  case VS_VOC_GOAL_ENUM_CREATE:
    {
      /**
       * Complete the GOAL as we already have the CVD resource
       * handles.
       */
      session_obj->session_ctrl.status = APR_EOK;
      rc = vs_voc_do_complete_goal( session_obj );
    }
    break;

  case VS_VOC_GOAL_ENUM_CONFIGURE:
    {
      switch ( session_obj->session_ctrl.action )
      {
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_OVER_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_media_type_on_stream( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_network_type_on_mvm( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_VOICE_TIMING;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_media_type_on_mvm( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_SET_VOICE_TIMING:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_set_voice_timing_params( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_EOK;

          rc = vs_voc_action_set_cached_stream_properties( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          session_obj->session_ctrl.status = APR_EOK;
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

  case VS_VOC_GOAL_ENUM_ENABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_RESYNC_CTM:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_START_VOICE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_resync_ctm( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_START_VOICE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_start_voice( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_COMPLETE:
        {
          /**
           * Complete GOAL_ENABLE and remain in IDLE state
           * as stream was not ready.
           */
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

  case VS_VOC_GOAL_ENUM_DISABLE:
    {
      switch( session_obj->session_ctrl.action )
      {
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_STOP_VOICE:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_stop_voice( session_obj );
        }
        return rc;

      case VS_VOC_ACTION_ENUM_COMPLETE:
      case VS_VOC_ACTION_ENUM_CONTINUE:
        {
          /**
           * GOAL DISABLE could get trigger in either of the
           * vocoder state RUN or IDLE.
           */
          session_obj->session_ctrl.status = APR_EOK;
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

  case VS_VOC_GOAL_ENUM_DESTROY:
    {
      switch ( session_obj->session_ctrl.action )
      {
        /**
         * Transition to Reset as after below action stream will be no
         * more attached to MVM.
         */
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_DETACH_CVS:
        {
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_detach_stream( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;


  default:
    break;
  }

  return APR_EOK;
}

/**
 * This is state is just a place holder for completeness. IDLE
 * to RUN transition doesn't involves this state.
 */
static int32_t vs_voc_state_run_entry (
  vs_session_object_t* session_obj
)
{
  uint32_t status;

  VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
  status = session_obj->session_ctrl.status;
  /* Release session ctrl lock */
  VS_RELEASE_LOCK( session_obj->session_ctrl_lock );

  /**
   * End the goal the current for below Scenarios:
   *
   * - IF CVD/AUDIO-PD is down or ADSP SSR is trigerred.
   */
  if ( ( vs_is_cvd_service_up == FALSE )  ||
       ( vs_is_adsp_timeout_triggered == TRUE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_state_run_entry(): last action vs_is_adsp_timeout_triggered "
           "=(%d), vs_is_cvd_service_up=(%d)", vs_is_adsp_timeout_triggered,
           vs_is_cvd_service_up );

    session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
    session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
    session_obj->session_ctrl.status = APR_ENOSERVICE;
    return APR_EIMMEDIATE;
  }
  else if ( status == APR_UNDEFINED_ID_V )
  {
    /**
     * Stay in this state till response for
     * last action is available.
     */
    return APR_EOK;
  }
  else if ( status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_state_run_entry(): last action failed with result=0x%08X",
           status );
    session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
    session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;

    if ( session_obj->stream_ready == TRUE )
    {
      vs_common_send_event( session_obj->header.handle,
                            VS_COMMON_EVENT_NOT_READY, NULL, 0 ) ;
      session_obj->stream_ready = FALSE;
    }

    return APR_EIMMEDIATE;
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_state_run_entry(): last action result=0x%08X",
           status );
    if ( session_obj->stream_ready == TRUE )
    {
      session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RUN;
    }
    else
    {
      session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
      session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_COMPLETE;
    }
    return APR_EIMMEDIATE;
  }

}

static int32_t vs_voc_state_run (
  vs_session_object_t* session_obj
)
{
  int32_t rc;

  switch( session_obj->session_ctrl.goal )
  {
  case VS_VOC_GOAL_ENUM_NONE:
    break;

  case VS_VOC_GOAL_ENUM_ENABLE:
    {
      switch( session_obj->session_ctrl.action )
      {
      case VS_VOC_ACTION_ENUM_CONTINUE:
        {
          /* Complete GOAL_ENABLE and transition from IDLE -> RUN. */
          rc = vs_voc_do_complete_goal( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

  case VS_VOC_GOAL_ENUM_DISABLE:
    {
      switch( session_obj->session_ctrl.action )
      {
      case VS_VOC_ACTION_ENUM_NONE:
      case VS_VOC_ACTION_ENUM_STOP_VOICE:
        {
          /* Lock not needed to state as goal isn't none */
          session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE_ENTRY;
          session_obj->session_ctrl.action = VS_VOC_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = vs_voc_action_send_stop_voice( session_obj );
        }
        return rc;

      default:
        break;
      }
    }
    break;

  default:
    VS_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }

  return APR_EIMMEDIATE;
}

VS_INTERNAL uint32_t vs_voc_state_control (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;

  if ( session_obj == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_voc_state_control(): session_obj is NULL " );
    return VS_EBADPARAM;
  }

  do
  {
     vs_voc_log_state_info( session_obj );

     switch ( session_obj->session_ctrl.state )
     {
     case VS_VOC_STATE_ENUM_RESET_ENTRY:
       rc = vs_voc_state_reset_entry( session_obj );
       break;

     case VS_VOC_STATE_ENUM_RESET:
       rc = vs_voc_state_reset( session_obj );
       break;

     case VS_VOC_STATE_ENUM_IDLE_ENTRY:
       rc = vs_voc_state_idle_entry( session_obj );
       break;

     case VS_VOC_STATE_ENUM_IDLE:
       rc = vs_voc_state_idle( session_obj );
       break;

     case VS_VOC_STATE_ENUM_RUN_ENTRY:
       rc = vs_voc_state_run_entry( session_obj );
       break;

     case VS_VOC_STATE_ENUM_RUN:
       rc = vs_voc_state_run( session_obj );
       break;

     default:
       VS_PANIC_ON_ERROR( APR_EUNEXPECTED );
     }
  }
  while ( rc == APR_EIMMEDIATE );

  vs_voc_log_state_info( session_obj );

  return rc;
}

/****************************************************************************
 * VS Asynchronous non-blocking API functions.                              *
 ****************************************************************************/

VS_INTERNAL uint32_t vs_voc_cmd_enable (
  vs_voc_cmd_enable_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_voc_cmd_enable_t enable_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_enable() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /* validate the session handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_enable() - Invalid handle=(0x%08x)", params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    enable_args.handle = params->handle;
    enable_args.media_id = params->media_id;
    enable_args.client_context = params->client_context;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOC_CMD_ENABLE, &enable_args,
                                             sizeof( enable_args ) );

    break;
  }
  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmd_disable (
  vs_voc_cmd_disable_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_voc_cmd_disable_t disable_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_disable() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /* validate the session handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_disable(): Invalid handle=(0x%08x)", params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    disable_args.handle = params->handle;
    disable_args.client_context = params->client_context;

    rc  = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                              VS_VOC_CMD_DISABLE, &disable_args,
                                              sizeof( disable_args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}  /* end of vs_voc_cmd_disable () */


VS_INTERNAL uint32_t vs_voc_cmd_standby (
  vs_voc_cmd_standby_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_voc_cmd_standby_t standby_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_standby() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /* validate the session handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_standby(): Invalid handle=(0x%08x)", params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    standby_args.handle = params->handle;
    standby_args.media_id = params->media_id;
    standby_args.client_context = params->client_context;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOC_CMD_STANDBY, &standby_args,
                                             sizeof( standby_args ) );
    break;
  } /* for loop ends. */

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmd_flush_buffers (
  vs_voc_cmd_flush_buffers_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_voc_cmd_flush_buffers_t flush_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_flush_buffers - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object ( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                               ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_flush_buffers(): Invalid handle=(0x%08x)",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    flush_args.handle = params->handle;
    flush_args.client_context = params->client_context;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOC_CMD_FLUSH_BUFFERS,
                                             &flush_args, sizeof( flush_args ) );
    break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmd_close (
  vs_voc_cmd_close_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_voc_cmd_close_t close_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_close() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /* validate the session handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_close(): Invalid handle=(0x%08x)", params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    close_args.handle = params->handle;
    close_args.client_context = params->client_context;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOC_CMD_CLOSE, &close_args,
                                             sizeof( close_args ) );
    break;
  }

  return rc;
}


/****************************************************************************
 * VS Synchronous blocking API functions...START                            *
 ****************************************************************************/

VS_INTERNAL uint32_t vs_voc_cmd_open (
  vs_voc_cmd_open_t* params
)
{
  uint32_t rc = VS_EOK;
  uint32_t dst_len = 0;
  vs_session_object_t* p_session_obj = NULL;
  vs_generic_item_t* p_generic_item = NULL;

  /**
   * Make vs_voc_cmd_open() thread safe, during init process for
   * muliple VS_CLIENTS.
   */
  ( void )apr_lock_enter( vs_voc_cmd_thread_lock );

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_open() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /** verify if the ret_handle passed by client is not NULL. */
    if( params->ret_handle == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_open() - NULL ret_handle passed by client. " );
      rc = VS_EUNEXPECTED;
      break;
    }

    /**
     * verify if the VSID passed by client is valid/supported by the
     * System.
     */
    switch ( params->vsid )
    {
    case VS_VSID_CS_VOICE_MULTIMODE:
    case VS_VSID_CS_VOICE_GSM2:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "vs_voc_cmd_open() - VSID=(0x%08x), client=(0x%08x)",
               params->vsid, params->client_id );
      }
      break;

    default:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_open() - Unsupported VSID=(0x%08x), client=(0x%08x)",
               params->vsid, params->client_id );
        return VS_EBADPARAM;
      }
      break;
    }  /* end of switch ( params->vsid ) */

    /**
     * Verify if a session is already opened for a VSID/client_id combination.
     *
     * If already opened, fail the API call.
     */
    p_generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );
    for ( ;; )
    {
      rc = apr_list_get_next( &vs_used_session_q,
                              ( ( apr_list_node_t* ) p_generic_item ),
                              ( ( apr_list_node_t** ) &p_generic_item ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_cmd_open() - no opened session, rc=(0x%08x)", rc );
        break;
      }

      rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      if ( rc )
      {
        VS_REPORT_FATAL_ON_ERROR( rc );
        break;
      }

      if ( ( p_session_obj->vsid == params->vsid ) &&
           ( p_session_obj->client_id == params->client_id ) )
      {
        /* fail API if already a session opened for VSID/client */
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_open() - session already created, VSID=(0x%08x), "
               "client=(0x%08x)", params->vsid, params->client_id );
        rc = VS_EUNSUPPORTED;
        break;
      }
      else
      {
        /* Keep looking. */
        p_session_obj = NULL;
      }
    }  /* end of for ( ;; ) */

    /**
     * We did not find existing session object. Go ahead and create
     * a new session object for VSID/Client.
     */
    if ( p_session_obj == NULL )
    {
      /* verify if we have empty session objects. */
      if ( vs_used_session_q.size == VS_MAX_NUM_SESSIONS_V )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_open() - ran out of objects VSID=(0x%08x), "
               "client=(0x%08x)", params->vsid, params->client_id );
        rc = VS_ENORESOURCE;
        break;
      }

      /* create a session object for a VSID/Client combination. */
      rc = vs_voc_create_session_object( &p_session_obj );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_open() - create session object failed, rc=(0x%08x)",
               rc );
        break;
      }
      else
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "vs_voc_cmd_open()- new session object created. handle=(0x%08x)",
               p_session_obj->header.handle );
      }
      p_session_obj->event_cb = params->event_cb;
      p_session_obj->session_context = params->session_context;

      /** vsid/client_id would be used to fetch the timing info. */
      p_session_obj->client_id = params->client_id;
      p_session_obj->vsid = params->vsid;

      vs_assign_session_destroy_lock( p_session_obj->header.handle );

      /** Update the voice timing info.*/
      ( void ) vs_voc_update_voice_timing_info( p_session_obj );

      /** Update CVD session name based off VSID */
      switch ( p_session_obj->vsid )
      {
      case VS_VSID_CS_VOICE_MULTIMODE:
        dst_len = MMSTD_MIN( sizeof( vs_default_stream_name ),
                             sizeof( p_session_obj->cvd_session_name ) );
        mmstd_strlcpy( p_session_obj->cvd_session_name,
                       vs_default_stream_name, dst_len );
        break;

      case VS_VSID_CS_VOICE_GSM2:
        dst_len = MMSTD_MIN( sizeof( vs_vsid_cs_voice_gsm2_stream_name ),
                             sizeof( p_session_obj->cvd_session_name ) );
        mmstd_strlcpy( p_session_obj->cvd_session_name,
                       vs_vsid_cs_voice_gsm2_stream_name, dst_len );
        break;
      }

      /* Copy the handle to return to client */
      *params->ret_handle = p_session_obj->header.handle;

      /* Add the new session to the session tracking list. */
      rc = apr_list_remove_head( &vs_free_session_q,
                                 ( ( apr_list_node_t** ) &p_generic_item ) );
      if ( rc )
      {
        VS_REPORT_FATAL_ON_ERROR( rc );
        break;
      }

      p_generic_item->handle = p_session_obj->header.handle;
      ( void ) apr_list_add_tail( &vs_used_session_q, &p_generic_item->link );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_open() failed: session exist already for "
             "VSID=(0x%08x), client=(0x%08x)", params->vsid,
             params->client_id );
    }  /* end of if (p_session_obj == NULL) */

    break;
  }  /* end of for ( ;; ) */

  /* Unlock mutex to handle error case scenarios. */
  ( void )apr_lock_leave( vs_voc_cmd_thread_lock );

  /* errors occured */
  return rc;
}  /* end of vs_voc_cmd_open () */

VS_INTERNAL uint32_t vs_voc_cmd_alloc_buffer (
  vs_voc_cmd_alloc_buffer_t* params
)
{
  uint32_t rc = VS_EOK;
  uint32_t checkpoint = 0;
  vs_session_object_t* p_session_obj = NULL;
  vs_voc_buffer_t* p_voc_buf = NULL;
  vs_voc_buffer_item_t* p_voc_buf_node = NULL;
  void* p_frame_info = NULL;
  uint8_t* p_frame = NULL;
  apr_lock_t* client_session_lock_p = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_alloc_buffer() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }

    if ( params->ret_buffer == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_alloc_buffer() - params->ret_buffer is NULL" );
      rc = VS_EUNEXPECTED;
      break;
    }

    /* Lock session object destroy process */
    if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_alloc_buffer() - Invalid handle=(0x%08x)",
           params->handle );
      rc = VS_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( *client_session_lock_p );

    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_alloc_buffer() - Invalid handle=(0x%08x)",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "vs_voc_cmd_alloc_buffer() - handle=(0x%08x), buffer_size=(%d)",
           params->handle, params->req_max_frame_size );
    {
      /* Allocate memory for vs_voc_buffer_t structure */
      p_voc_buf =( vs_voc_buffer_t* ) apr_memmgr_malloc( &vs_heapmgr,
                                        sizeof( vs_voc_buffer_t ) );
      if ( p_voc_buf == NULL )
      {
        rc = VS_ENORESOURCE;
        VS_REPORT_FATAL_ON_ERROR( rc );
        break;
      }
      mmstd_memset( ( void* )p_voc_buf, 0, sizeof( vs_voc_buffer_t ) );
      checkpoint = 1;
    }

    {
      /* Allocate memory for vs_voc_buffer_t.frame_info.
       * since the exact size depends on vs_voc_buffer_t.media_id, allocate size
       * of the largest frame_info structure - vs_vocamrwb_frame_info_t .
       */
      p_frame_info =
         ( vs_vocamrwb_frame_info_t* ) apr_memmgr_malloc( &vs_heapmgr,
                                         sizeof( vs_vocamrwb_frame_info_t ) );
      if ( p_frame_info == NULL )
      {
        rc = VS_ENORESOURCE;
        VS_REPORT_FATAL_ON_ERROR( rc );
        break;
      }

      mmstd_memset( ( void* )p_frame_info, 0, sizeof( vs_vocamrwb_frame_info_t ) );
      p_voc_buf->frame_info = ( void* )p_frame_info;
      checkpoint = 2;
    }

    {
      /* Allocate memory for frame "vocoder packet" */
      p_frame = (uint8_t*) apr_memmgr_malloc( &vs_heapmgr,
                             params->req_max_frame_size );
      if ( p_frame == NULL )
      {
        rc = VS_ENORESOURCE;
        VS_REPORT_FATAL_ON_ERROR( rc );
        break;
      }

      mmstd_memset( ( void* )p_frame, 0, params->req_max_frame_size );
      p_voc_buf->max_size = params->req_max_frame_size;
      p_voc_buf->frame = p_frame;
      *params->ret_buffer = p_voc_buf;
      checkpoint = 3;
    }

    {
      /**
       * Allocate memory for node vs_voc_buffer_item_t. The vocoder
       * buffer added to this node and used to track allocated vocoder
       * buffer during packet exchange across alloc_buf_q, write_buf_q
       * and read_buf_q.
       */
      p_voc_buf_node =
        ( vs_voc_buffer_item_t* ) apr_memmgr_malloc( &vs_heapmgr,
                                    sizeof( vs_voc_buffer_item_t ) );
      if ( p_voc_buf_node == NULL )
      {
        rc = VS_ENORESOURCE;
        VS_REPORT_FATAL_ON_ERROR( rc );
        break;
      }

      p_voc_buf_node->buf = p_voc_buf;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_voc_cmd_alloc_buffer() - buffer allocated=(0x%08x)",
             p_voc_buf_node->buf );

      p_voc_buf_node->buf_type = VS_VOC_BUFFER_TYPE_NONE;
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &p_voc_buf_node->link );
      ( void ) apr_list_add_tail( &p_session_obj->alloc_buf_q,
                                  (apr_list_node_t*) &p_voc_buf_node->link );
      checkpoint = 0;
    }

    break;
  }  /* end of for ( ;; ) */
  if( client_session_lock_p != NULL )
    VS_RELEASE_LOCK( *client_session_lock_p );

  /**
   * Cleanup for allocated memory, checkpoint value indicates the
   * number of memory cleanup to be performed.
   */
  switch ( checkpoint )
  {
  case 3:
    if ( p_voc_buf->frame != NULL )
    {
      apr_memmgr_free( &vs_heapmgr, (void*) p_voc_buf->frame );
      p_voc_buf->frame = NULL;
      p_frame = NULL;
    }
    /* fall through. */

  case 2:
    if ( p_voc_buf->frame_info != NULL )
    {
      apr_memmgr_free( &vs_heapmgr, (void*) p_voc_buf->frame_info );
      p_voc_buf->frame_info = NULL;
      p_frame_info = NULL;
    }
    /*fall through */

  case 1:
    if ( p_voc_buf != NULL )
    {
      apr_memmgr_free( &vs_heapmgr, (void*) p_voc_buf );
      p_voc_buf = NULL;
    }
    /*fall through */

  case 0:
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_cmd_alloc_buffer() - checkpoint=(%d)", checkpoint );
    break;
  }  /* end of switch ( checkpoint ) */

  return rc;
}  /* end of vs_voc_cmd_alloc_buffer () */

VS_INTERNAL uint32_t vs_voc_cmd_free_buffer (
  vs_voc_cmd_free_buffer_t* params
)
{

  uint32_t rc = VS_EOK;
  bool_t is_buf_in_read_q = FALSE;
  bool_t is_buf_in_write_q = FALSE;
  bool_t is_buf_in_alloc_q = FALSE;
  vs_session_object_t* p_session_obj = NULL;
  vs_voc_buffer_item_t* p_voc_buf_node = NULL;
  apr_lock_t* client_session_lock_p = NULL;

  /* Validate the params provided by client. */
  if ( params == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_voc_cmd_free_buffer() - NULL params" );
    return VS_EUNEXPECTED;
  }
  /* Lock session object destroy process */
  if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_free_buffer() - Invalid handle=(0x%08x)",
           params->handle );
    return VS_EBADPARAM;
  }
  VS_ACQUIRE_LOCK( *client_session_lock_p );

  /* Get the session object */
  rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            ( ( vs_object_t** ) &p_session_obj ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_free_buffer() - Invalid handle=(0x%08x)",
           params->handle );
    VS_RELEASE_LOCK( *client_session_lock_p );
    return VS_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "vs_voc_cmd_free_buffer() - handle=(0x%08x), buffer=(0x%08x)",
         params->handle, params->buffer );
  /**
   * Allocated buffer should be present in one of session object
   * queues alloc_buf_q, write_buf_q or read_buf_q.
   */
  for ( ;; )
  {
    /* Traverse read_buf_q to find the vocoder buffer. */
    p_voc_buf_node =
       ( ( vs_voc_buffer_item_t* ) &p_session_obj->read_buf_q.dummy );
    for ( ;;)
    {
      rc = apr_list_get_next( &p_session_obj->read_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc )
      {
        is_buf_in_read_q = FALSE;
        rc = VS_EFAILED;
        break;
      }
      else if( params->buffer == p_voc_buf_node->buf )
      {
        is_buf_in_read_q = TRUE;
        break;
      }
    }
    if ( is_buf_in_read_q == TRUE )
    {
      /* Delete the buffer node from read_buf_q list */
      apr_list_delete( &p_session_obj->read_buf_q,
                       ( ( apr_list_node_t* ) p_voc_buf_node ) );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_voc_cmd_free_buffer() - buffer found in read_buf_q  " );
      break;
    }

    /* Traverse write_buf_q to find the vocoder buffer. */
    p_voc_buf_node =
       ( ( vs_voc_buffer_item_t* ) &p_session_obj->write_buf_q.dummy );
    for ( ;;)
    {
      rc = apr_list_get_next( &p_session_obj->write_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc )
      {
        is_buf_in_write_q = FALSE;
        rc = VS_EFAILED;
        break;
      }
      else if( params->buffer == p_voc_buf_node->buf )
      {
        is_buf_in_write_q = TRUE;
        break;
      }
    }
    if ( is_buf_in_write_q == TRUE )
    {
      /* Delete the buffer node from read_buf_q list */
      apr_list_delete( &p_session_obj->write_buf_q,
                       ( ( apr_list_node_t* ) p_voc_buf_node ) );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_voc_cmd_free_buffer() - buffer found in write_buf_q  " );
      break;
    }

    /* Traverse alloc_buf_q to find the vocoder buffer. */
  p_voc_buf_node =
            ( ( vs_voc_buffer_item_t* ) &p_session_obj->alloc_buf_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &p_session_obj->alloc_buf_q,
                            ( ( apr_list_node_t* ) p_voc_buf_node ),
                            ( ( apr_list_node_t** ) &p_voc_buf_node ) );
    if ( rc )
    {
        is_buf_in_alloc_q = FALSE;
      rc = VS_EFAILED;
      break;
    }
    else if( params->buffer == p_voc_buf_node->buf )
    {
        is_buf_in_alloc_q = TRUE;
      break;
    }
    }
    if ( is_buf_in_alloc_q == TRUE )
  {
      /* Delete the buffer node from alloc_buf_q list */
    apr_list_delete( &p_session_obj->alloc_buf_q,
                     ( ( apr_list_node_t* ) p_voc_buf_node ) );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_voc_cmd_free_buffer() - buffer found in alloc_buf_q  " );
       break;
    }

    break;
  }

  if ( is_buf_in_write_q || is_buf_in_read_q || is_buf_in_alloc_q )
  {
    /* free the buffer requested by client */
    if ( p_voc_buf_node != NULL )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_voc_cmd_free_buffer(): buffer freed buf=(0x%08x), "
             "buf->frame_info=(0x%08x), buf->frame=(0x%08x)", p_voc_buf_node->buf,
             p_voc_buf_node->buf->frame_info, p_voc_buf_node->buf->frame );

      if ( p_voc_buf_node->buf->frame_info != NULL )
      { /* Delete the vocoder buffer frame info memory. */
        apr_memmgr_free( &vs_heapmgr, p_voc_buf_node->buf->frame_info );
        p_voc_buf_node->buf->frame_info = NULL;
      }

      if ( p_voc_buf_node->buf->frame != NULL )
      { /* Delete the vocoder buffer frame memory. */
        apr_memmgr_free( &vs_heapmgr, p_voc_buf_node->buf->frame );
        p_voc_buf_node->buf->frame = NULL;
      }

      { /* Delete the vocoder buffer memory. */
        apr_memmgr_free( &vs_heapmgr, p_voc_buf_node->buf );
        p_voc_buf_node->buf = NULL;
      }

      { /* Delete the buffer node memory. */
        apr_memmgr_free( &vs_heapmgr, p_voc_buf_node );
        p_voc_buf_node = NULL;
      }
    }
    else
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    }
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_free_buffer() - invalid buffer=(0x%08x)", params->buffer );
  }  /* end of if ( is_found ) */

  VS_RELEASE_LOCK( *client_session_lock_p );
  return rc;

}  /* end of vs_voc_cmd_free_buffer () */

/****************************************************************************
 * VS Synchronous non-blocking API functions.                               *
 ****************************************************************************/

VS_INTERNAL uint32_t vs_voc_cmd_prime_read_buffer (
  vs_voc_cmd_prime_read_buffer_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj = NULL;
  vs_voc_buffer_item_t* p_voc_buf_node = NULL;
  bool_t is_found = FALSE;
  apr_lock_t* client_session_lock_p = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_prime_read_buffer() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }
    /* Lock session object destroy process */
    if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_alloc_buffer() - Invalid handle=(0x%08x)",
           params->handle );
      rc = VS_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( *client_session_lock_p );

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( (vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_prime_read_buffer(): Invalid handle=(0x%08x), "
             "buffer=(0x%08x), rc=(0x%08x)", params->handle,
             params->buffer, rc );
      rc = VS_EBADPARAM;
      break;
    }

    /* Check whether the given buffer is in the alloc_buf_q. */
    p_voc_buf_node =
      ( vs_voc_buffer_item_t* )( &p_session_obj->alloc_buf_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &p_session_obj->alloc_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc ) break;

      if ( params->buffer == p_voc_buf_node->buf )
      {
        is_found = TRUE;
        break;
      }
    } /* end of for ( ;; ) */

    if( is_found )
    {
      if ( p_voc_buf_node->buf_type == VS_VOC_BUFFER_TYPE_PRIMED )
      {
        /* Flag Error: if node containing params->buffer is already primed. */
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "vs_voc_cmd_prime_read_buffer(): buffer=(0x%08x) primed",
                params->buffer );
      }
      else
      {
        /* Mark this node containing params->buffer primed for TX path. */
         p_voc_buf_node->buf_type = VS_VOC_BUFFER_TYPE_PRIMED;
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                "vs_voc_cmd_prime_read_buffer(): buffer=(0x%08x) primed",
                params->buffer );
      }
    }
    else
    {
      rc = VS_ENOTEXIST;
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_prime_read_buffer(): Primed buffer invalid=(0x%08x) "
             "rc=(0x%08x), handle=(0x%08x)", params->buffer,
             rc, params->handle );
    } /* end of if( found ) */

    break;
  }
  if( client_session_lock_p != NULL )
    VS_RELEASE_LOCK( *client_session_lock_p );

  return rc;
}  /* end of vs_voc_cmd_prime_read_buffer () */


VS_INTERNAL uint32_t vs_voc_cmd_read_buffer (
  vs_voc_cmd_read_buffer_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj;
  vs_voc_buffer_item_t* p_voc_buf_node;
  apr_lock_t* client_session_lock_p = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_read_buffer() - NULL  params" );
      rc = VS_EUNEXPECTED;
      break;
    }
    /* Lock session object destroy process */
    if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_read_buffer() - Invalid handle=(0x%08x)",
           params->handle );
      rc = VS_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( *client_session_lock_p );

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_read_buffer(): Invalid handle=(0x%08x), "
             "buffer=(0x%08x)",params->handle, (*params->ret_buffer) );
      rc = VS_EBADPARAM;
      break;
    }

    /**
     * Extract the encoder packet from read_buf_q if available form
     * ADSP, unprime the vocoder node by marking it UNSET and moving
     * it to alloc_buf_q.
     */
    rc = apr_list_remove_head( &p_session_obj->read_buf_q,
                               ( ( apr_list_node_t** ) &p_voc_buf_node ) );
    if ( rc )
    {
      rc = VS_ENOTEXIST;
      *params->ret_buffer = NULL;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_read_buffer(): tx_packet_status = vocoder packet NOT "
           "available from ADSP."  );
    }
    else
    {
      if ( p_voc_buf_node != NULL)
      {
        /* Return the read/TX buffer to client. */
         *params->ret_buffer = p_voc_buf_node->buf;

         /* Move the node back to alloc_buf_q since its read successfully. */
         p_voc_buf_node->buf_type = VS_VOC_BUFFER_TYPE_NONE;
         ( void ) apr_list_add_tail( &p_session_obj->alloc_buf_q,
                                     &p_voc_buf_node->link );
         p_voc_buf_node = NULL;
      }
      else
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_read_buffer(): p_voc_buf_node is NULL" );
      }
    }  /* end of if ( rc ) */

    break;
  }
  if ( client_session_lock_p != NULL )
    VS_RELEASE_LOCK( *client_session_lock_p );

  return rc;
}  /* end of vs_voc_cmd_read_buffer () */


VS_INTERNAL uint32_t vs_voc_cmd_write_buffer (
  vs_voc_cmd_write_buffer_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj;
  vs_voc_buffer_item_t* p_voc_buf_node;
  bool_t is_buf_in_alloc_q = FALSE;
  bool_t is_buf_in_write_q = FALSE;
  apr_lock_t* client_session_lock_p = NULL;
  vs_voc_client_type_t client_type;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }
    /* Lock session object destroy process */
    if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer() - Invalid handle=(0x%08x)",
           params->handle );
      rc = VS_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( *client_session_lock_p );

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( (vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_write_buffer(): Invalid handle=(0x%08x) "
             "buffer=(0x%08x)", params->handle, params->buffer );
      rc = VS_EBADPARAM;
      break;
    }

    /**
     * This interface is not meant for MVS clients. 
     */
    client_type = vs_voc_check_client_type( p_session_obj->client_id );
    
    if ( VS_VOC_CLIENT_TYPE_MODEM != client_type )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer() - MMODE client should not use this "
           "Interface" );
      rc = VS_EFAILED;
      break;
    }

    /* Check whether the given buffer is in the alloc buffer queue*/
    p_voc_buf_node =
      ( vs_voc_buffer_item_t* )( &p_session_obj->alloc_buf_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &p_session_obj->alloc_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc ) break;

      if ( params->buffer == p_voc_buf_node->buf )
      {
        is_buf_in_alloc_q = TRUE;
        break;
      }
    }  /* end of for ( ;; ) */

    if ( is_buf_in_alloc_q ==  TRUE )
    {
      /* Delete the buffer node from alloc_buf_q list */
      apr_list_delete( &p_session_obj->alloc_buf_q,
                       ( ( apr_list_node_t* ) p_voc_buf_node ) );

      if ( p_voc_buf_node->buf_type == VS_VOC_BUFFER_TYPE_PRIMED )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_write_buffer(): buffer=(0x%08x) is already primed "
               "for read Overridding the buffer status as per current request",
               params->handle, params->buffer );
      }
      /* Mark this node primed for RX path. */
      p_voc_buf_node->buf_type = VS_VOC_BUFFER_TYPE_WRITE;

      /* Move this node to write_buf_q for forwarding it to ADSP. */
      ( void ) apr_list_add_tail( &p_session_obj->write_buf_q,
                                  &p_voc_buf_node->link );
      break;
    }

    /**
     * If not found in alloc_buf_q, check whether the given buffer
     * is already written to write_buf_q.
     */
    p_voc_buf_node =
      ( vs_voc_buffer_item_t* )( &p_session_obj->write_buf_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &p_session_obj->write_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc ) break;

      if ( params->buffer == p_voc_buf_node->buf )
      {
        is_buf_in_write_q = TRUE;
        break;
      }
    }  /* end of for ( ;; ) */

    if ( is_buf_in_write_q ==  TRUE )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_write_buffer(): buffer already written=(0x%08x) "
             "rc=(0x%08x), handle=(0x%08x)", params->buffer, rc,
             params->handle );
    }

    break;
  }

  if ( client_session_lock_p != NULL )
    VS_RELEASE_LOCK( *client_session_lock_p );

  if ( ( is_buf_in_alloc_q == FALSE ) &&
       ( is_buf_in_write_q == FALSE ) && ( params != NULL ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer(): Invalid write buffer = (0x%08x) session "
           "handle= (0x%08x)", params->buffer, params->handle );
    rc = VS_EBADPARAM;
  }

  return rc;
}  /* end of vs_voc_cmd_write_buffer () */


VS_INTERNAL uint32_t vs_voc_cmd_write_buffer_through (
  vs_voc_cmd_write_buffer_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj;
  vs_voc_buffer_item_t* p_voc_buf_node;
  bool_t is_buf_in_alloc_q = FALSE;
  apr_lock_t* client_session_lock_p = NULL;
  vs_voc_client_type_t client_type;
  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer_through() - NULL params" );
      rc = VS_EUNEXPECTED;
      break;
    }
    /* Lock session object destroy process */
    if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer_through() - Invalid handle=(0x%08x)",
           params->handle );
      rc = VS_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( *client_session_lock_p );

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( (vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_write_buffer_through(): Invalid handle=(0x%08x) "
             "buffer=(0x%08x)", params->handle, params->buffer );
      rc = VS_EBADPARAM;
      break;
    }
    client_type = vs_voc_check_client_type( p_session_obj->client_id );

    /**
     * This interface is meant only for MVS clients. 
     */
    if ( VS_VOC_CLIENT_TYPE_MVS != client_type )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_write_buffer_through() - Interface meant only for "
           "MMODE clients" );
      rc = VS_EFAILED;
      break;
    }

    /* Check whether the given buffer is in the alloc buffer queue*/
    p_voc_buf_node =
      ( vs_voc_buffer_item_t* )( &p_session_obj->alloc_buf_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &p_session_obj->alloc_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc ) break;

      if ( params->buffer == p_voc_buf_node->buf )
      {
        is_buf_in_alloc_q = TRUE;
        break;
      }
    }  /* end of for ( ;; ) */

    if ( is_buf_in_alloc_q ==  TRUE )
    {
      if ( p_voc_buf_node->buf_type != VS_VOC_BUFFER_TYPE_NONE )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_write_buffer_through(): buffer=(0x%08x) type is not "
               "VS_VOC_BUFFER_TYPE_NONE for VS session with handle=(0x%08x)",
               params->buffer, params->handle );
        rc = VS_EFAILED;
        break;
      }

      rc = vs_voc_send_dec_packet( p_session_obj->header.handle,
                                   params->buffer, &mmode_rx_packet );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmd_write_buffer_through(): ERROR=0x%08x, while sending dec "
               "buffer to ADSP", rc );
        rc = VS_EOK;
      }
      p_session_obj->event_cb( VS_VOC_EVENT_WRITE_BUFFER_RETURNED,
                               params->buffer,
                               sizeof( vs_voc_event_write_buffer_returned_t ),
                               p_session_obj->session_context );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_write_buffer_through(): Invalid write buffer = (0x%08x) "
             "session handle= (0x%08x)", params->buffer, params->handle );
      rc = VS_EBADPARAM;
    }

    break;
  }

  if ( client_session_lock_p != NULL )
    VS_RELEASE_LOCK( *client_session_lock_p );

  return rc;
}  /* end of vs_voc_cmd_write_buffer_through () */

/****************************************************************************
 * VS Synchronous non-blocking API functions...END                          *
 ****************************************************************************/

/**
 * Maps the EVRC-A vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrca_frame_rate_t cvd_to_vs_map_evrca_frame_rate (
  uint8_t rate )
{
  vs_vocevrca_frame_rate_t evrca_frame_rate;

  switch ( rate )
  {
    case 0:
      evrca_frame_rate = VS_VOCEVRCA_FRAME_BLANK_RATE;
      break;

    case 1:
      evrca_frame_rate = VS_VOCEVRCA_FRAME_EIGHTH_RATE;
      break;

    case 3:
      evrca_frame_rate = VS_VOCEVRCA_FRAME_HALF_RATE;
      break;

    case 4:
      evrca_frame_rate = VS_VOCEVRCA_FRAME_FULL_RATE;
      break;

    case 14:
      evrca_frame_rate = VS_VOCEVRCA_FRAME_ERASURE_RATE;
      break;

    default:
      evrca_frame_rate = ( vs_vocevrca_frame_rate_t ) -1;
  }

  return evrca_frame_rate;
}

/**
 * Maps the EVRC-A vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrca_frame_rate (
  vs_vocevrca_frame_rate_t rate )
{
  uint8_t evrca_frame_rate;

  switch ( rate )
  {
    case VS_VOCEVRCA_FRAME_BLANK_RATE:
      evrca_frame_rate = 0;
      break;

    case VS_VOCEVRCA_FRAME_EIGHTH_RATE:
      evrca_frame_rate = 1;
      break;

    case VS_VOCEVRCA_FRAME_HALF_RATE:
      evrca_frame_rate = 3;
      break;

    case VS_VOCEVRCA_FRAME_FULL_RATE:
      evrca_frame_rate = 4;
      break;

    case VS_VOCEVRCA_FRAME_ERASURE_RATE:
      evrca_frame_rate = 14;

    default:
      evrca_frame_rate = ( vs_vocevrca_frame_rate_t ) -1;
  }

  return evrca_frame_rate;
}

/**
 * Maps the EVRC-B vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcb_frame_rate_t cvd_to_vs_map_evrcb_frame_rate (
  uint8_t rate )
{
  vs_vocevrcb_frame_rate_t evrcb_frame_rate;

  switch ( rate )
  {
    case 0:
      evrcb_frame_rate = VS_VOCEVRCB_FRAME_BLANK_RATE;
      break;

    case 1:
      evrcb_frame_rate = VS_VOCEVRCB_FRAME_EIGHTH_RATE;
      break;

    case 2:
      evrcb_frame_rate = VS_VOCEVRCB_FRAME_QUARTER_RATE;
      break;

    case 3:
      evrcb_frame_rate = VS_VOCEVRCB_FRAME_HALF_RATE;
      break;

    case 4:
      evrcb_frame_rate = VS_VOCEVRCB_FRAME_FULL_RATE;
      break;

    case 14:
      evrcb_frame_rate = VS_VOCEVRCB_FRAME_ERASURE_RATE;
      break;

    default:
      evrcb_frame_rate = ( vs_vocevrcb_frame_rate_t ) -1;
  }

  return evrcb_frame_rate;
}

/**
 * Maps the EVRC-B vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcb_frame_rate (
  vs_vocevrcb_frame_rate_t rate )
{
  uint8_t evrcb_frame_rate;

  switch ( rate )
  {
    case VS_VOCEVRCB_FRAME_BLANK_RATE:
      evrcb_frame_rate = 0;
      break;

    case VS_VOCEVRCB_FRAME_EIGHTH_RATE:
      evrcb_frame_rate = 1;
      break;

    case VS_VOCEVRCB_FRAME_QUARTER_RATE:
      evrcb_frame_rate = 2;
      break;

    case VS_VOCEVRCB_FRAME_HALF_RATE:
      evrcb_frame_rate = 3;
      break;

    case VS_VOCEVRCB_FRAME_FULL_RATE:
      evrcb_frame_rate = 4;
      break;

    case VS_VOCEVRCB_FRAME_ERASURE_RATE:
      evrcb_frame_rate = 14;

    default:
      evrcb_frame_rate = ( vs_vocevrcb_frame_rate_t ) -1;
  }

  return evrcb_frame_rate;
}

/**
 * Maps the EVRC-WB vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcwb_frame_rate_t cvd_to_vs_map_evrcwb_frame_rate (
  uint8_t rate )
{
  vs_vocevrcwb_frame_rate_t evrcwb_frame_rate;

  switch ( rate )
  {
    case 0:
      evrcwb_frame_rate = VS_VOCEVRCWB_FRAME_BLANK_RATE;
      break;

    case 1:
      evrcwb_frame_rate = VS_VOCEVRCWB_FRAME_EIGHTH_RATE;
      break;

    case 2:
      evrcwb_frame_rate = VS_VOCEVRCWB_FRAME_QUARTER_RATE;
      break;

    case 3:
      evrcwb_frame_rate = VS_VOCEVRCWB_FRAME_HALF_RATE;
      break;

    case 4:
      evrcwb_frame_rate = VS_VOCEVRCWB_FRAME_FULL_RATE;
      break;

    case 14:
      evrcwb_frame_rate = VS_VOCEVRCWB_FRAME_ERASURE_RATE;
      break;

    default:
      evrcwb_frame_rate = ( vs_vocevrcwb_frame_rate_t ) -1;
  }

  return evrcwb_frame_rate;
}

/**
 * Maps the EVRC-WB vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcwb_frame_rate (
  vs_vocevrcwb_frame_rate_t rate )
{
  uint8_t evrcwb_frame_rate;

  switch ( rate )
  {
    case VS_VOCEVRCWB_FRAME_BLANK_RATE:
      evrcwb_frame_rate = 0;
      break;

    case VS_VOCEVRCWB_FRAME_EIGHTH_RATE:
      evrcwb_frame_rate = 1;
      break;

    case VS_VOCEVRCWB_FRAME_QUARTER_RATE:
      evrcwb_frame_rate = 2;
      break;

    case VS_VOCEVRCWB_FRAME_HALF_RATE:
      evrcwb_frame_rate = 3;
      break;

    case VS_VOCEVRCWB_FRAME_FULL_RATE:
      evrcwb_frame_rate = 4;
      break;

    case VS_VOCEVRCWB_FRAME_ERASURE_RATE:
      evrcwb_frame_rate = 14;

    default:
      evrcwb_frame_rate = ( vs_vocevrcwb_frame_rate_t ) -1;
  }

  return evrcwb_frame_rate;
}

/**
 * Maps the EVRC-NW vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcnw_frame_rate_t cvd_to_vs_map_evrcnw_frame_rate (
  uint8_t rate )
{
  vs_vocevrcnw_frame_rate_t evrcnw_frame_rate;

  switch ( rate )
  {
    case 0:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_BLANK_RATE;
      break;

    case 1:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_EIGHTH_RATE;
      break;

    case 2:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_QUARTER_RATE;
      break;

    case 3:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_HALF_RATE;
      break;

    case 4:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_FULL_RATE;
      break;

    case 14:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_ERASURE_RATE;
      break;

    case 255:
      evrcnw_frame_rate = VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL;
      break;

    default:
      evrcnw_frame_rate = ( vs_vocevrcnw_frame_rate_t ) -1;
  }

  return evrcnw_frame_rate;
}

/**
 * Maps the EVRC-NW vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcnw_frame_rate (
  vs_vocevrcnw_frame_rate_t rate )
{
  uint8_t evrcnw_frame_rate;

  switch ( rate )
  {
    case VS_VOCEVRCNW_FRAME_BLANK_RATE:
      evrcnw_frame_rate = 0;
      break;

    case VS_VOCEVRCNW_FRAME_EIGHTH_RATE:
      evrcnw_frame_rate = 1;
      break;

    case VS_VOCEVRCNW_FRAME_QUARTER_RATE:
      evrcnw_frame_rate = 2;
      break;

    case VS_VOCEVRCNW_FRAME_HALF_RATE:
      evrcnw_frame_rate = 3;
      break;

    case VS_VOCEVRCNW_FRAME_FULL_RATE:
      evrcnw_frame_rate = 4;
      break;

    case VS_VOCEVRCNW_FRAME_ERASURE_RATE:
      evrcnw_frame_rate = 14;

    case VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL:
      evrcnw_frame_rate = 255;

    default:
      evrcnw_frame_rate = ( vs_vocevrcnw_frame_rate_t ) -1;
  }

  return evrcnw_frame_rate;
}

/**
 * Maps the EVRC-NW2k vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcnw2k_frame_rate_t cvd_to_vs_map_evrcnw2k_frame_rate (
  uint8_t rate )
{
  vs_vocevrcnw2k_frame_rate_t evrcnw2k_frame_rate;

  switch ( rate )
  {
    case 0:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_BLANK_RATE;
      break;

    case 1:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE;
      break;

    case 2:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_QUARTER_RATE;
      break;

    case 3:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_HALF_RATE;
      break;

    case 4:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_FULL_RATE;
      break;

    case 14:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_ERASURE_RATE;
      break;

    case 255:
      evrcnw2k_frame_rate = VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL;
      break;

    default:
      evrcnw2k_frame_rate = ( vs_vocevrcnw2k_frame_rate_t ) -1;
  }

  return evrcnw2k_frame_rate;
}

/**
 * Maps the EVRC-NW2K vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcnw2k_frame_rate (
  vs_vocevrcnw2k_frame_rate_t rate )
{
  uint8_t evrcnw2k_frame_rate;

  switch ( rate )
  {
    case VS_VOCEVRCNW2K_FRAME_BLANK_RATE:
      evrcnw2k_frame_rate = 0;
      break;

    case VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE:
      evrcnw2k_frame_rate = 1;
      break;

    case VS_VOCEVRCNW2K_FRAME_QUARTER_RATE:
      evrcnw2k_frame_rate = 2;
      break;

    case VS_VOCEVRCNW2K_FRAME_HALF_RATE:
      evrcnw2k_frame_rate = 3;
      break;

    case VS_VOCEVRCNW2K_FRAME_FULL_RATE:
      evrcnw2k_frame_rate = 4;
      break;

    case VS_VOCEVRCNW2K_FRAME_ERASURE_RATE:
      evrcnw2k_frame_rate = 14;

    case VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL:
      evrcnw2k_frame_rate = 255;

    default:
      evrcnw2k_frame_rate = ( vs_vocevrcnw2k_frame_rate_t ) -1;
  }

  return evrcnw2k_frame_rate;
}

/**
 * Maps the QCELP-13K vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocqcelp13k_frame_rate_t cvd_to_vs_map_qcelp13k_frame_rate (
  uint8_t rate )
{
  vs_vocqcelp13k_frame_rate_t qcelp13k_frame_rate;

  switch ( rate )
  {
    case 0:
      qcelp13k_frame_rate = VS_VOCQCELP13K_FRAME_BLANK_RATE;
      break;

    case 1:
      qcelp13k_frame_rate = VS_VOCQCELP13K_FRAME_EIGHTH_RATE;
      break;

    case 2:
      qcelp13k_frame_rate = VS_VOCQCELP13K_FRAME_HALF_RATE;
      break;

    case 3:
      qcelp13k_frame_rate = VS_VOCQCELP13K_FRAME_HALF_RATE;
      break;

    case 4:
      qcelp13k_frame_rate = VS_VOCQCELP13K_FRAME_FULL_RATE;
      break;

    case 14:
      qcelp13k_frame_rate = VS_VOCQCELP13K_FRAME_ERASURE_RATE;
      break;

    default:
      qcelp13k_frame_rate = ( vs_vocqcelp13k_frame_rate_t ) -1;
  }

  return qcelp13k_frame_rate;
}

/**
 * Maps the QCELP-13K vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_qcelp13k_frame_rate (
  vs_vocqcelp13k_frame_rate_t rate )
{
  uint8_t qcelp13k_frame_rate;

  switch ( rate )
  {
    case VS_VOCQCELP13K_FRAME_BLANK_RATE:
      qcelp13k_frame_rate = 0;
      break;

    case VS_VOCQCELP13K_FRAME_EIGHTH_RATE:
      qcelp13k_frame_rate = 1;
      break;

    case VS_VOCQCELP13K_FRAME_QUARTER_RATE:
      qcelp13k_frame_rate = 1;
      break;

    case VS_VOCQCELP13K_FRAME_HALF_RATE:
      qcelp13k_frame_rate = 3;
      break;

    case VS_VOCQCELP13K_FRAME_FULL_RATE:
      qcelp13k_frame_rate = 4;
      break;

    case VS_VOCQCELP13K_FRAME_ERASURE_RATE:
      qcelp13k_frame_rate = 14;

    default:
      qcelp13k_frame_rate = ( vs_vocqcelp13k_frame_rate_t ) -1;
  }

  return qcelp13k_frame_rate;
}

VS_INTERNAL uint32_t vs_voc_send_enc_packet (
  uint32_t session_handle,
  vs_voc_buffer_item_t* p_rbuffer_node,
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = APR_EOK;
  vs_session_object_t* p_session_obj = NULL;

  uint32_t voc_packet_size = 0;
  uint8_t* p_payload = NULL;
  vs_voc_buffer_t* p_rbuffer = NULL;

  vs_vocamr_codec_mode_t amr_codec_mode;
  vs_vocamr_frame_type_t amr_frame_type;
  vs_vocamrwb_codec_mode_t amrwb_codec_mode;
  vs_vocamrwb_frame_type_t amrwb_frame_type;

  /* EVRC-A */
  uint8_t evrca_frame_rate;
  vs_vocevrca_frame_rate_t evrca_framerate;

  /* EVRC-B */
  uint8_t evrcb_frame_rate;
  vs_vocevrcb_frame_rate_t evrcb_framerate;

  /* EVRC-WB */
  uint8_t evrcwb_frame_rate;
  vs_vocevrcwb_frame_rate_t evrcwb_framerate;

  /* EVRC-NW */
  uint8_t evrcnw_frame_rate;
  vs_vocevrcnw_frame_rate_t evrcnw_framerate;

  /* EVRC-NW2K */
  uint8_t evrcnw2k_frame_rate;
  vs_vocevrcnw2k_frame_rate_t evrcnw2k_framerate;

  /* QCELP-13K */
  uint8_t qcelp13k_frame_rate;
  vs_vocqcelp13k_frame_rate_t qcelp13k_framerate;

  /* G.729 */
  //vs_vocg729_frame_type_t g729_frame1_type;
  //uint8_t g729_frame2_type;
  //uint8_t g729_frame2_loc;

  /* G.711 A-LAW */
  //uint8_t g711alaw_frame1_type;
  //uint8_t g711alaw_frame2_type;
  //uint8_t g711alaw_frame2_loc;

  /* G.711 MU-LAW */
  //uint8_t g711mulaw_frame1_type;
  //uint8_t g711mulaw_frame2_type;
  //uint8_t g711mulaw_frame2_loc;

  if ( p_rbuffer_node == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_voc_send_enc_packet(): NULL p_rbuffer_node" );
    return VS_EBADPARAM;
  }

  for ( ;; )
  {
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( (vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    voc_packet_size =
      ( APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet->header ) -
                                sizeof( vss_istream_evt_send_enc_buffer_t ) );

    p_payload = APR_PTR_END_OF( APRV2_PKT_GET_PAYLOAD( void, p_packet ),
                                sizeof( vss_istream_evt_send_enc_buffer_t ) );

    p_rbuffer = p_rbuffer_node->buf;
    p_rbuffer->media_id = p_session_obj->media_id;
    p_rbuffer_node->buf_type = VS_VOC_BUFFER_TYPE_READ;

    /**
     * Fail if voc_packet_size is larger than the client allocated
     * buffer.
     */
    if ( ( voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ) > p_rbuffer->max_size )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "CVS delivered encoder packet size (%d) is larger than the "
             "allocated buffer size (%d).",
             ( voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ),
             p_rbuffer->max_size );
      rc = VS_EBADPARAM;
      break;
    }

    switch ( p_rbuffer->media_id )
    {
    /**
     * TBD: Other missing vocoder support needs to be added.
     * AI: Shruthi.
     */
    case VS_COMMON_MEDIA_ID_FR:
    case VS_COMMON_MEDIA_ID_HR:
    case VS_COMMON_MEDIA_ID_EFR:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /** Update SID here. BFI, TAF and UFI (HR only) bits are
         *  applicable only on downlink.
         */
        ( ( vs_vocfr_frame_info_t* )p_rbuffer->frame_info )->sid =
                                      ( ( *p_payload ) & 0x06 ) >> 1 ;

        p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
        mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                      ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                      p_rbuffer->size );

        p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

        ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                    &( p_rbuffer_node->link ) );
        vs_common_send_event( p_session_obj->header.handle,
                              VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
      }
      break;

    case VS_COMMON_MEDIA_ID_AMR:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract amr codec mode and frame type */
        amr_codec_mode =
          ( vs_vocamr_codec_mode_t ) ( ( *p_payload ) & 0x0F );
        amr_frame_type =
          ( vs_vocamr_frame_type_t ) ( ( ( *p_payload ) & 0xF0 ) >> 4 );

        /* validate amr codec mode and frame type */
        if ( amr_codec_mode < VS_VOCAMR_CODEC_MODE_0475 ||
             amr_codec_mode > VS_VOCAMR_CODEC_MODE_1220 ||
             amr_frame_type < VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD ||
             amr_frame_type > VS_VOCAMR_FRAME_TYPE_NO_DATA )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: Invalid AMR packet header: "
                 "codec_mode=%d, frame_type=%d", amr_codec_mode, amr_frame_type );
        }
        else
        {
          ( ( vs_vocamr_frame_info_t* )p_rbuffer->frame_info )->codec_mode =
                                         amr_codec_mode;
          ( ( vs_vocamr_frame_info_t* )p_rbuffer->frame_info )->frame_type =
                                         amr_frame_type;

          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER (AMR): size=%d, "
                 "codec_mode=%d, frame_type=%d", p_rbuffer->size,
                 amr_codec_mode, amr_frame_type );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                      &p_rbuffer_node->link);
          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }  /* end of if ( mode < VS_VOCAMR_CODEC_MODE_0475 ||  */
      }
      break;

    case VS_COMMON_MEDIA_ID_AMRWB:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract mode and frame type */
        amrwb_codec_mode =
          ( vs_vocamrwb_codec_mode_t ) ( ( *p_payload ) & 0x0F );
        amrwb_frame_type =
          ( vs_vocamrwb_frame_type_t ) ( ( ( *p_payload ) & 0xF0 ) >> 4 );

        /* validate mode and frame type */
        if ( amrwb_codec_mode < VS_VOCAMRWB_CODEC_MODE_0660 ||
             amrwb_codec_mode > VS_VOCAMRWB_CODEC_MODE_2385 ||
             amrwb_frame_type < VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD ||
             amrwb_frame_type > VS_VOCAMRWB_FRAME_TYPE_NO_DATA )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: Invalid AMRWB packet header: "
                 "codec_mode=%d, frame_type=%d", amrwb_codec_mode, amrwb_frame_type );
        }
        else
        {
          ( ( vs_vocamrwb_frame_info_t* )p_rbuffer->frame_info )->codec_mode =
                                           amrwb_codec_mode;
          ( ( vs_vocamrwb_frame_info_t* )p_rbuffer->frame_info )->frame_type =
                                           amrwb_frame_type;

          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(AMR_WB):size=%d, "
                 "codec_mode=%d, frame_type=%d",p_rbuffer->size,
                 amrwb_codec_mode, amrwb_frame_type );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                      &p_rbuffer_node->link );
          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }  /* end of if ( mode < VS_VOCAMRWB_CODEC_MODE_0660 ||  */
      }
      break;

    case VS_COMMON_MEDIA_ID_EVRCA:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract the frame rate */
        evrca_frame_rate = ( uint8_t ) ( *p_payload );
        evrca_framerate = cvd_to_vs_map_evrca_frame_rate ( evrca_frame_rate );

        /* validate frame rate */
        if( evrca_framerate < VS_VOCEVRCA_FRAME_BLANK_RATE ||
            evrca_framerate > VS_VOCEVRCA_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: "
                 "Invalid EVRC-A packet header: "
                 "frame_rate=%d", evrca_framerate );
        }
        else
        {
          ( ( vs_vocevrca_frame_info_t* )p_rbuffer->frame_info )->rate =
                                           evrca_framerate;
          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(EVRC-A):"
                 "size=%d, frame_rate=%d",
                 p_rbuffer->size, evrca_framerate );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                      &p_rbuffer_node->link);
          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }
      } /* end of case VS_COMMON_MEDIA_ID_EVRCA */
      break;

    case VS_COMMON_MEDIA_ID_EVRCB:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract the frame rate */
        evrcb_frame_rate = ( uint8_t ) ( *p_payload );
        evrcb_framerate = cvd_to_vs_map_evrcb_frame_rate ( evrcb_frame_rate );

        /* validate frame rate */

        if( evrcb_framerate < VS_VOCEVRCB_FRAME_BLANK_RATE ||
            evrcb_framerate > VS_VOCEVRCB_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: "
                 "Invalid EVRC-B packet header: "
                 "frame_rate=%d", evrcb_framerate );
        }
        else
        {
          ( ( vs_vocevrcb_frame_info_t* )p_rbuffer->frame_info )->rate =
                                           evrcb_framerate;
          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(EVRC-B):"
                 "size=%d, frame_rate=%d",
                 p_rbuffer->size, evrcb_framerate );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                      &p_rbuffer_node->link);

          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }
      } /* end of case VS_COMMON_MEDIA_ID_EVRCB */
      break;

    case VS_COMMON_MEDIA_ID_EVRCWB:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract the frame rate */
        evrcwb_frame_rate = ( uint8_t ) ( *p_payload );
        evrcwb_framerate = cvd_to_vs_map_evrcwb_frame_rate ( evrcwb_frame_rate );

        /* validate frame rate */
        if( evrcwb_framerate < VS_VOCEVRCWB_FRAME_BLANK_RATE ||
            evrcwb_framerate > VS_VOCEVRCWB_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: "
                 "Invalid EVRC-WB packet header: "
                 "frame_rate=%d", evrcwb_framerate );
        }
        else
        {
          ( ( vs_vocevrcwb_frame_info_t* )p_rbuffer->frame_info )->rate =
                                           evrcwb_framerate;
          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(EVRC-WB):"
                 "size=%d, frame_rate=%d",
                 p_rbuffer->size, evrcwb_framerate );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                      &p_rbuffer_node->link);

          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }
      } /* end of case VS_COMMON_MEDIA_ID_EVRCWB */
      break;

    case VS_COMMON_MEDIA_ID_EVRCNW:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract the frame rate */
        evrcnw_frame_rate = ( uint8_t ) ( *p_payload );
        evrcnw_framerate = cvd_to_vs_map_evrcnw_frame_rate ( evrcnw_frame_rate );

        /* validate frame rate */
        if( evrcnw_framerate < VS_VOCEVRCNW_FRAME_BLANK_RATE ||
            evrcnw_framerate > VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: "
                 "Invalid EVRC-NW packet header: "
                 "frame_rate=%d", evrcnw_framerate );
        }
        else
        {
          ( ( vs_vocevrcnw_frame_info_t* )p_rbuffer->frame_info )->rate =
                                           evrcnw_framerate;
          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(EVRC-NW):"
                 "size=%d, frame_rate=%d",
                 p_rbuffer->size, evrcnw_framerate );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                           &p_rbuffer_node->link);

          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }
      } /* end of case VS_COMMON_MEDIA_ID_EVRCNW */
      break;

    case VS_COMMON_MEDIA_ID_EVRCNW2K:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract the frame rate */
        evrcnw2k_frame_rate = ( uint8_t ) ( *p_payload );
        evrcnw2k_framerate = cvd_to_vs_map_evrcnw2k_frame_rate ( evrcnw2k_frame_rate );

        /* validate frame rate */
        if( evrcnw2k_framerate < VS_VOCEVRCNW2K_FRAME_BLANK_RATE ||
            evrcnw2k_framerate > VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: "
                 "Invalid EVRC-NW2K packet header: "
                 "frame_rate=%d", evrcnw2k_framerate );
        }
        else
        {
          ( ( vs_vocevrcnw2k_frame_info_t* )p_rbuffer->frame_info )->rate =
                                           evrcnw2k_framerate;
          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(EVRC-NW2K):"
                 "size=%d, frame_rate=%d",
                 p_rbuffer->size, evrcnw2k_framerate );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                           &p_rbuffer_node->link);

          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }
      } /* end of case VS_COMMON_MEDIA_ID_EVRCNW2K */
      break;

    case VS_COMMON_MEDIA_ID_QCELP13K:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;

        /* extract the frame rate */
        qcelp13k_frame_rate = ( uint8_t ) ( *p_payload );
        qcelp13k_framerate = cvd_to_vs_map_qcelp13k_frame_rate ( qcelp13k_frame_rate );

        /* validate frame rate */
        if( qcelp13k_framerate < VS_VOCQCELP13K_FRAME_BLANK_RATE ||
            qcelp13k_framerate > VS_VOCQCELP13K_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: "
                 "Invalid QCELP-13K packet header: "
                 "frame_rate=%d", qcelp13k_framerate );
        }
        else
        {
          ( ( vs_vocqcelp13k_frame_info_t* )p_rbuffer->frame_info )->rate =
                                           qcelp13k_framerate;
          p_rbuffer->size = voc_packet_size - VS_VOC_PACKET_HDR_LENGTH_V ;
          mmstd_memcpy( p_rbuffer->frame,p_rbuffer->max_size,
                        ( p_payload + VS_VOC_PACKET_HDR_LENGTH_V ),
                        p_rbuffer->size );

          p_rbuffer->timestamp = VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V;

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER(QCELP-13K):"
                 "size=%d, frame_rate=%d",
                 p_rbuffer->size, qcelp13k_framerate );

          ( void ) apr_list_add_tail( &p_session_obj->read_buf_q,
                                           &p_rbuffer_node->link);

          vs_common_send_event( p_session_obj->header.handle,
                                VS_VOC_EVENT_READ_AVAILABLE, NULL, 0 );
        }
      } /* end of case VS_COMMON_MEDIA_ID_QCELP13K */
      break;

    case VS_COMMON_MEDIA_ID_G729:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;
      } /* end of case VS_COMMON_MEDIA_ID_G729 */
      break;

    case VS_COMMON_MEDIA_ID_G711ALAW:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;
      } /* end of case VS_COMMON_MEDIA_ID_G711ALAW */
      break;

    case VS_COMMON_MEDIA_ID_G711MULAW:
      {
        p_rbuffer->version = VS_VOC_BUFFER_MINOR_VERSION_V;
        p_rbuffer->flags = VS_VOC_BUFFER_VALID_SPEECH_FRAME;
      } /* end of case VS_COMMON_MEDIA_ID_G711ALAW */
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Media ID did not match any available one. media_id=0x%08x",
             p_rbuffer->media_id );
      break;
    }

    break;
  }

  return rc;
}

VS_INTERNAL uint32_t vs_voc_send_dec_packet (
  uint32_t session_handle,
  vs_voc_buffer_t* p_wbuffer,
  aprv2_packet_t* p_packet
)
{
  vs_session_object_t* p_session_obj = NULL;
  uint32_t rc = VS_EOK;
  aprv2_packet_t* p_packet1 = NULL;
  vss_istream_evt_send_dec_buffer_t* p_dec_buffer = NULL;
  uint8_t* p_voc_packet;
  vs_vocamr_frame_info_t* p_amr = NULL;
  vs_vocamr_frame_type_t amr_frame_type;
  vs_vocamr_codec_mode_t amr_codec_mode;
  vs_vocamrwb_frame_info_t* p_amrwb = NULL;
  vs_vocevrca_frame_info_t* p_evrca = NULL;
  vs_vocevrcb_frame_info_t* p_evrcb = NULL;
  vs_vocevrcwb_frame_info_t* p_evrcwb = NULL;
  vs_vocevrcnw_frame_info_t* p_evrcnw = NULL;
  vs_vocevrcnw2k_frame_info_t* p_evrcnw2k = NULL;
  vs_vocqcelp13k_frame_info_t* p_qcelp13k = NULL;
  //vs_vocg729_frame_info_t* p_g729 = NULL;
  //vs_vocg711alaw_frame_info_t* p_g711alaw = NULL;
  //vs_vocg711mulaw_frame_info_t* p_g711mulaw = NULL;
  vs_vocamrwb_frame_type_t amrwb_frame_type;
  vs_vocamrwb_codec_mode_t amrwb_codec_mode;
  vs_vocevrca_frame_rate_t evrca_frame_rate;
  vs_vocevrcb_frame_rate_t evrcb_frame_rate;
  vs_vocevrcwb_frame_rate_t evrcwb_frame_rate;
  vs_vocevrcnw_frame_rate_t evrcnw_frame_rate;
  vs_vocevrcnw2k_frame_rate_t evrcnw2k_frame_rate;
  vs_vocqcelp13k_frame_rate_t qcelp13k_frame_rate;
  //vs_vocg729_frame_type_t g729_frame_type;
  //vs_vocg711alaw_frame_type_t g711alaw_frame_type;
  //vs_vocg711mulaw_frame_type_t g711mulaw_frame_type;
  uint32_t packet_payload_size;
  /* 1 byte header + vocoder data */
  uint32_t frame_length = 0;

  if ( p_wbuffer == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_voc_send_dec_packet(): NULL p_wbuffer" );
    return VS_EBADPARAM;
  }

  for ( ;; )
  {
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    /* check media_id is valid or not */
    /* since gl1 is not sending correct media_id, commented this block
     * for test purpose.
     */
    #if 0
    if ( p_wbuffer->media_id != p_session_obj->media_id )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_send_dec_packet(): Incorrect media_id received = (0x%08x), "
             "actual configured media_id = (0x%08x)",
             p_wbuffer->media_id, p_session_obj->media_id);
      rc = VS_EBADPARAM;
      break;
    }
    #endif

    /* this line should be removed as well - <TBD>*/
    p_wbuffer->media_id = p_session_obj->media_id;

    /* check vocoder packet size */
    if ( p_wbuffer->size > p_wbuffer->max_size )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_send_dec_packet(): Incorrect packet_size=(%d), "
             "maximum size=(%d), session_handle=(0x%08x)",
             p_wbuffer->size, p_wbuffer->max_size, session_handle );
      rc = VS_EBADPARAM;
      break;
    }

    switch ( p_wbuffer->media_id )
    {
    case VS_COMMON_MEDIA_ID_AMR:
      {
        p_amr = ( vs_vocamr_frame_info_t* )( p_wbuffer->frame_info );

        /* validate amr frame_type  */
        amr_frame_type  = p_amr->frame_type;

        if ( amr_frame_type < VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD ||
             amr_frame_type > VS_VOCAMR_FRAME_TYPE_NO_DATA )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect AMR frame_type=(%d)",
                 amr_frame_type );
          rc = VS_EBADPARAM;
          break;
        }

        /* validate amr codec_mode */
        amr_codec_mode = p_amr->codec_mode;

        if ( amr_codec_mode < VS_VOCAMR_CODEC_MODE_0475 ||
             amr_codec_mode > VS_VOCAMR_CODEC_MODE_1220 )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect AMR codec_mode=(%d)",
                 amr_codec_mode );
          rc = VS_EBADPARAM;
          break;
        }
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): AMR codec_mode=(%d), frame_type=(%d), "
               "session_handle=(0x%08x)",
               amr_codec_mode, amr_frame_type, session_handle );

        frame_length = vs_vocamr_get_frame_len( amr_frame_type,
                                                amr_codec_mode );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( frame_length + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );

        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet data */
        *p_voc_packet = ( uint8_t ) ( ( ( amr_frame_type & 0x0F ) << 4 ) |
                                      ( amr_codec_mode & 0x0F ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                        p_wbuffer->frame, frame_length );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      }
      break;

    case VS_COMMON_MEDIA_ID_AMRWB:
      {
        p_amrwb = ( vs_vocamrwb_frame_info_t* )( p_wbuffer->frame_info );

        /* validate frame_type  */
        amrwb_frame_type  = p_amrwb->frame_type;

        if ( amrwb_frame_type < VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD ||
             amrwb_frame_type > VS_VOCAMRWB_FRAME_TYPE_NO_DATA )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Invalid AMRWB frame_type=(%d)",
                 amrwb_frame_type );
          rc = VS_EBADPARAM;
          break;
        }

        /* validate codec_mode */
        amrwb_codec_mode = p_amrwb->codec_mode;

        if ( amrwb_codec_mode < VS_VOCAMRWB_CODEC_MODE_0660 ||
             amrwb_codec_mode > VS_VOCAMRWB_CODEC_MODE_2385 )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect AMRWB codec_mode=(%d)",
                 amrwb_codec_mode );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): AMRWB codec_mode=(%d), frame_type=(%d), "
               "session_handle=(0x%08x)",
               amrwb_codec_mode, amrwb_frame_type, session_handle );

        frame_length = vs_vocamrwb_get_frame_len( amrwb_frame_type,
                                                  amrwb_codec_mode );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( frame_length + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed. " );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( amrwb_frame_type & 0x0F ) << 4 ) |
                        ( amrwb_codec_mode & 0x0F );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                        p_wbuffer->frame, frame_length );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );

      }
      break;

    case VS_COMMON_MEDIA_ID_EFR:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): EFR frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCEFR_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = (*( uint8_t* )( p_wbuffer->frame_info ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCEFR_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      }
      break;

    case VS_COMMON_MEDIA_ID_FR:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): FR frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCFR_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );

        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = (*( uint8_t* )( p_wbuffer->frame_info ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                        p_wbuffer->frame, VS_VOCFR_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      }
      break;

    case VS_COMMON_MEDIA_ID_HR:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): HR frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCHR_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if (  ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );

        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = (*( uint8_t* )( p_wbuffer->frame_info ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                        p_wbuffer->frame, VS_VOCHR_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      }
      break;

      case VS_COMMON_MEDIA_ID_EVRCA:
      {
        /* Extract frame rate from the write buffer */
        p_evrca = ( vs_vocevrca_frame_info_t* ) ( p_wbuffer->frame_info );
        evrca_frame_rate = p_evrca->rate;

        /* Validate EVRC-A frame type */
        if ( evrca_frame_rate < VS_VOCEVRCA_FRAME_BLANK_RATE ||
             evrca_frame_rate > VS_VOCEVRCA_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect EVRC-A frame_type=(%d)",
                 evrca_frame_rate );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): EVRC-A frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCEVRCA_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if (  ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( uint8_t )
                          ( vs_to_cvd_map_evrca_frame_rate ( evrca_frame_rate ) ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCEVRCA_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      } /* end of case VS_COMMON_MEDIA_ID_EVRCA */
      break;

      case VS_COMMON_MEDIA_ID_EVRCB:
      {
        /* Extract frame rate from the write buffer */
        p_evrcb = ( vs_vocevrcb_frame_info_t* ) ( p_wbuffer->frame_info );
        evrcb_frame_rate = p_evrcb->rate;

        /* Validate EVRC-B frame type */
        if ( evrcb_frame_rate < VS_VOCEVRCB_FRAME_BLANK_RATE ||
             evrcb_frame_rate > VS_VOCEVRCB_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect EVRC-B frame_type=(%d)",
                 evrcb_frame_rate );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): EVRC-B frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCEVRCB_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( uint8_t )
                          ( vs_to_cvd_map_evrcb_frame_rate ( evrcb_frame_rate ) ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCEVRCB_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      } /* end of case VS_COMMON_MEDIA_ID_EVRCB */
      break;

      case VS_COMMON_MEDIA_ID_EVRCWB:
      {
        /* Extract frame rate from the write buffer */
        p_evrcwb = ( vs_vocevrcwb_frame_info_t* ) ( p_wbuffer->frame_info );
        evrcwb_frame_rate = p_evrcwb->rate;

        /* Validate EVRC-WB frame type */
        if ( evrcwb_frame_rate < VS_VOCEVRCWB_FRAME_BLANK_RATE ||
             evrcwb_frame_rate > VS_VOCEVRCWB_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect EVRC-WB frame_type=(%d)",
                 evrcwb_frame_rate );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): EVRC-WB frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCEVRCWB_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( uint8_t )
                          ( vs_to_cvd_map_evrcwb_frame_rate ( evrcwb_frame_rate ) ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCEVRCWB_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      } /* end of case VS_COMMON_MEDIA_ID_EVRCWB */
      break;

      case VS_COMMON_MEDIA_ID_EVRCNW:
      {
        /* Extract frame rate from the write buffer */
        p_evrcnw = ( vs_vocevrcnw_frame_info_t* ) ( p_wbuffer->frame_info );
        evrcnw_frame_rate = p_evrcnw->rate;

        /* Validate EVRC-NW frame type */
        if ( evrcnw_frame_rate < VS_VOCEVRCNW_FRAME_BLANK_RATE ||
             evrcnw_frame_rate > VS_VOCEVRCNW_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect EVRC-NW frame_type=(%d)",
                 evrcnw_frame_rate );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): EVRC-NW frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCEVRCNW_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( uint8_t )
                          ( vs_to_cvd_map_evrcnw_frame_rate ( evrcnw_frame_rate ) ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCEVRCNW_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      } /* end of case VS_COMMON_MEDIA_ID_EVRCNW */
      break;

      case VS_COMMON_MEDIA_ID_EVRCNW2K:
      {
        /* Extract frame rate from the write buffer */
        p_evrcnw2k = ( vs_vocevrcnw2k_frame_info_t* ) ( p_wbuffer->frame_info );
        evrcnw2k_frame_rate = p_evrcnw2k->rate;

        /* Validate EVRC-NW2K frame type */
        if ( evrcnw2k_frame_rate < VS_VOCEVRCNW2K_FRAME_BLANK_RATE ||
             evrcnw2k_frame_rate > VS_VOCEVRCNW2K_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect EVRC-NW2K frame_type=(%d)",
                 evrcnw2k_frame_rate );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): EVRC-NW2K frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCEVRCNW2K_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( (p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( uint8_t )
                          ( vs_to_cvd_map_evrcnw2k_frame_rate ( evrcnw2k_frame_rate ) ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCEVRCNW2K_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      } /* end of case VS_COMMON_MEDIA_ID_EVRCNW2K */
      break;

      case VS_COMMON_MEDIA_ID_QCELP13K:
      {
        /* Extract frame rate from the write buffer */
        p_qcelp13k = ( vs_vocqcelp13k_frame_info_t* ) ( p_wbuffer->frame_info );
        qcelp13k_frame_rate = p_qcelp13k->rate;

        /* Validate QCELP-13K frame type */
        if ( qcelp13k_frame_rate < VS_VOCQCELP13K_FRAME_BLANK_RATE ||
             qcelp13k_frame_rate > VS_VOCQCELP13K_FRAME_ERASURE_RATE )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_send_dec_packet(): Incorrect QCELP-13K frame_type=(%d)",
                 qcelp13k_frame_rate );
          rc = VS_EBADPARAM;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "vs_voc_send_dec_packet(): QCELP-13K frame_info=(%d), session_handle = (0x%08x)",
               (*( uint8_t* )( p_wbuffer->frame_info ) ), session_handle );

        rc = __aprv2_cmd_alloc_ext(
               vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
               p_packet->dst_addr, p_packet->dst_port,
               p_packet->src_addr, p_packet->src_port,
               p_packet->token, VSS_ISTREAM_EVT_SEND_DEC_BUFFER,
               ( sizeof( vss_istream_evt_send_dec_buffer_t ) +
                 ( VS_VOCQCELP13K_MAX_PACKET_DATA_LENGTH + 1 ) ), &p_packet1 );
        VS_REPORT_FATAL_ON_ERROR( rc );

        if ( ( p_packet1 == NULL ) || rc )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_send_dec_packet(): __aprv2_cmd_alloc_ext() Failed!!" );
          rc = VS_ENORESOURCE;
          break;
        }
        p_dec_buffer = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_send_dec_buffer_t,
                                              p_packet1 );
        p_dec_buffer->media_id = vs_voc_map_to_cvd_mode( p_wbuffer->media_id,
                                                         p_session_obj->client_id );
        p_voc_packet = APR_PTR_END_OF( p_dec_buffer,
                                       sizeof( vss_istream_evt_send_dec_buffer_t ) );

        /* populate header and copy vocoder packet */
        *p_voc_packet = ( ( uint8_t )
                          ( vs_to_cvd_map_qcelp13k_frame_rate ( qcelp13k_frame_rate ) ) );

        packet_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_packet1->header )
                              - sizeof( vss_istream_evt_send_dec_buffer_t )
                              - VS_VOC_PACKET_HDR_LENGTH_V;

        mmstd_memcpy( ( p_voc_packet + VS_VOC_PACKET_HDR_LENGTH_V ), packet_payload_size,
                      p_wbuffer->frame, VS_VOCQCELP13K_MAX_PACKET_DATA_LENGTH );

        /* send vocoder packet to CVS */
        ( void ) __aprv2_cmd_forward( vs_apr_handle, p_packet1 );
      } /* end of case VS_COMMON_MEDIA_ID_QCELP13K */
      break;

      case VS_COMMON_MEDIA_ID_G729:
      {

      } /* end of case VS_COMMON_MEDIA_ID_G729 */
      break;

      case VS_COMMON_MEDIA_ID_G711ALAW:
      {

      } /* end of case VS_COMMON_MEDIA_ID_G711ALAW */
      break;

      case VS_COMMON_MEDIA_ID_G711MULAW:
      {

      } /* end of case VS_COMMON_MEDIA_ID_G711MULAW */
      break;

    default:
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_send_dec_packet(): Invalid media id" );
      rc = VS_EUNSUPPORTED;
      break;
    }  /* end of switch ( p_wbuffer->media_id ) */

    /* break always */
    break;
  }  /* end of for ( ;; ) */

  return rc;

}  /* end of vs_voc_send_dec_packet() */


/****************************************************************************
 * ( CALL SIGNALLING CMD ) / ( P1 GATING CMD ) PROCESSING FUNCTIONS         *
 ****************************************************************************/


VS_INTERNAL uint32_t vs_voc_cmd_enable_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = VS_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmdi_enable_t args;
  vs_voc_cmd_enable_t* p_enable_cmd_params = NULL;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    /* Do cmd_packet validation. */
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do cmd_param validation. */
    p_enable_cmd_params = ( vs_voc_cmd_enable_t * )p_cmd_packet->params;
    if( p_enable_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do session handle validation. */
    rc = vs_get_typed_object( p_enable_cmd_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      rsp_status = rc;
      break;
    }

    if ( p_session_obj->is_close_invoked )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_enable_proc(): VS_VOC_CMD_ENABLE isn't permitted "
             "after receiving VS_VOC_CMD_CLOSE!!!!" );
      rsp_status = VS_EFAILED;
      break;
    }

    if ( p_session_obj->call_state == VS_CALL_STATE_ENUM_ENABLED )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_enable_proc(): Calling VS_VOC_CMD_ENABLE more than once "
           "is not permitted!!!!" );
      rsp_status = VS_EFAILED;
      break;
    }

    rc = vs_voc_validate_n_update_active_client( VS_VOC_CMD_ENABLE,
                                                 p_session_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_enable_proc(): another cleint with VSID=(0x%08x) "
             " is already enabled", p_session_obj->vsid );
      rsp_status = rc;
      break;
    }

      /* Do media Id Validation as per the client id. */
    rc =  vs_voc_validate_media_id( p_enable_cmd_params->media_id,
                                    p_session_obj->client_id );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_enable_proc(): Unsupported Media_ID=(0x%08x)",
             p_enable_cmd_params->media_id );
      rsp_status = VS_EBADPARAM;
      break;
    }

    /* Update the media_id. */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    p_session_obj->pending_media_id = p_enable_cmd_params->media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    /* Update the call-state. */
    VS_ACQUIRE_LOCK( p_session_obj->client_state_lock );
    p_session_obj->call_state = VS_CALL_STATE_ENUM_ENABLED;
    VS_RELEASE_LOCK( p_session_obj->client_state_lock );


    /* CVD IS UP & Voice resource grant is given.
     * Q CMD for CVD SESSION STANDBY.
     */
    if ( ( TRUE == vs_is_cvd_cleanup_done ) &&
         ( TRUE == vs_is_cvd_service_up ) &&
         ( TRUE == p_session_obj->is_resource_granted ) )
    {
       args.handle = p_enable_cmd_params->handle;
       args.client_context = p_enable_cmd_params->client_context;
       args.is_rsp_pending = TRUE;
       rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_LOW,
                                                 VS_VOC_CMDI_ENABLE, &args,
                                                 sizeof( args ) );
       if ( rc ) rsp_status = rc;

    }
    else
    {
      rsp_status = VS_ENEEDMORE;
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_voc_cmd_enable_proc() - CVD SESSION CMD not Queued "
             "vs_is_cvd_cleanup_done=(0x%08x), cvd_service_up=(0x%08x),"
             "resource_granted=(0x%08x)", vs_is_cvd_cleanup_done,
             vs_is_cvd_service_up, p_session_obj->is_resource_granted );
    }

    break;
  }

  if ( rsp_status && ( p_enable_cmd_params != NULL ) )
  {
    /* Inform command status to client */
    cmd_rsp.cmd_id = VS_VOC_CMD_ENABLE;
    cmd_rsp.status_id = rsp_status;
    cmd_rsp.client_context = p_enable_cmd_params->client_context;
    vs_common_send_event( p_session_obj->header.handle,
                          VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
  }

  return APR_EOK;
}  /* end of vs_voc_cmd_enable_proc() */


VS_INTERNAL uint32_t vs_voc_cmd_standby_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = VS_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_voc_cmdi_standby_t args;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmd_standby_t* p_standby_cmd_params = NULL;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    p_standby_cmd_params = ( ( vs_voc_cmd_standby_t* )p_cmd_packet->params );
    if ( p_standby_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* validate the session handle */
    rc = vs_get_typed_object( p_standby_cmd_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      rsp_status = rc;
      break;
    }

    if ( p_session_obj->is_close_invoked )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_standby_proc(): VS_VOC_CMD_STANDBY isn't permitted "
           "after receiving VS_VOC_CMD_CLOSE!!!!" );
      rc = VS_EFAILED;
      break;
    }

    rc = vs_voc_validate_n_update_active_client( VS_VOC_CMD_STANDBY,
                                                 p_session_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_standby_proc(): another cleint with VSID=(0x%08x) "
             " is already enabled", p_session_obj->vsid );
      rsp_status = rc;
      break;
    }

    /* Do media Id Validation as per the client id. */
    rc =  vs_voc_validate_media_id( p_standby_cmd_params->media_id,
                                      p_session_obj->client_id );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_standby_proc(): Unsupported Media_ID=(0x%08x)",
             p_standby_cmd_params->media_id );
      rsp_status = VS_EBADPARAM;
      break;
    }

    /* Update the media_id. */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    p_session_obj->pending_media_id = p_standby_cmd_params->media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    /* Update the call-state. */
    VS_ACQUIRE_LOCK( p_session_obj->client_state_lock );
    p_session_obj->call_state = VS_CALL_STATE_ENUM_IDLE;
    VS_RELEASE_LOCK( p_session_obj->client_state_lock );

   /* CVD IS UP & Voice resource grant is given. *                              .
    * Q CMD for CVD SESSION STANDBY.
    */
   if ( ( TRUE == vs_is_cvd_cleanup_done ) &&
        ( TRUE == vs_is_cvd_service_up ) &&
        ( TRUE == p_session_obj->is_resource_granted ) )
   {
       args.handle = p_standby_cmd_params->handle;
       args.client_context = p_standby_cmd_params->client_context;
       args.is_rsp_pending = TRUE;
       rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_LOW,
                                                 VS_VOC_CMDI_STANDBY, &args,
                                                 sizeof( args ) );
       if ( rc ) rsp_status = rc;
   }
   else
   {
     rsp_status = VS_ENEEDMORE;
     MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "vs_voc_cmd_standby_proc() - CVD SESSION CMD not Queued "
            "vs_is_cvd_cleanup_done=(0x%08x), cvd_service_up=(0x%08x),"
            "resource_granted=(0x%08x)", vs_is_cvd_cleanup_done,
            vs_is_cvd_service_up, p_session_obj->is_resource_granted );
   }

   break;
  }

  if ( rsp_status && ( p_standby_cmd_params != NULL ) )
  {
    /* Inform command status to client */
    cmd_rsp.cmd_id = VS_VOC_CMD_STANDBY;
    cmd_rsp.status_id = rsp_status;
    cmd_rsp.client_context = p_standby_cmd_params->client_context;
    vs_common_send_event( p_session_obj->header.handle,
                          VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
  }

  return VS_EOK;
}  /* end of vs_voc_cmd_standby_proc() */


VS_INTERNAL uint32_t vs_voc_cmd_disable_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_voc_cmdi_disable_t args;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmd_disable_t* p_disable_cmd_params = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_token_object_t* p_token_obj = NULL;

  for ( ;; )
  {
    /* Do cmd_packet validation. */
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do cmd_param validation. */
    p_disable_cmd_params = ( vs_voc_cmd_disable_t * )p_cmd_packet->params;
    if( p_disable_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do session handle validation. */
    rc = vs_get_typed_object( p_disable_cmd_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      rsp_status = APR_EFAILED;
      break;
    }

    if ( p_session_obj->is_close_invoked )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_disable_proc(): VS_VOC_CMD_DIABLE isn't permitted "
           "after receiving VS_VOC_CMD_CLOSE!!!!" );
      rsp_status = VS_EFAILED;
      break;
    }

    ( void ) vs_voc_validate_n_update_active_client( VS_VOC_CMD_DISABLE,
                                                     p_session_obj );

    /* Update the call-state. */
    VS_ACQUIRE_LOCK( p_session_obj->client_state_lock );
    p_session_obj->call_state = VS_CALL_STATE_ENUM_RESET;
    VS_RELEASE_LOCK( p_session_obj->client_state_lock );

    /**
     * NOTE: Send a STOP voice commands immediatly, to stop vocoder.
     *       This is need to ensure that vocoder are put to stop, in case
     *       the internal queued commands processing get delayed due to
     *       other commands and low thread priority.
     */
    if ( p_session_obj->mvm_port != APR_NULL_V )
    {
      rc = vs_create_autofree_token_object( &p_token_obj );
      if ( rc )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_disable_proc() - Failed to allocate object" );
      }
      else
      {
        rc = __aprv2_cmd_alloc_send(
               vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
               vs_mvm_addr, p_session_obj->mvm_port,
               p_token_obj->header.handle, VSS_IMVM_CMD_MODEM_STOP_VOICE,
               NULL, 0 );

        if ( rc )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_voc_cmd_disable_proc() - Unable to send command over APR "
                 "rc=(0x%08x), cvd_service_up=(0x%08x)",
                 rc, vs_is_cvd_service_up );
          ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
        }
      }
    }

    args.handle = p_disable_cmd_params->handle;
    args.client_context = p_disable_cmd_params->client_context;
    args.is_rsp_pending = TRUE;
    rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_LOW,
                                              VS_VOC_CMDI_DISABLE, &args,
                                              sizeof( args ) );
    if ( rc ) rsp_status = rc;
    break;
  }

  if ( rsp_status && ( p_disable_cmd_params != NULL ) )
  {
    /* Inform command status to client */
    cmd_rsp.cmd_id = VS_VOC_CMD_DISABLE;
    cmd_rsp.status_id = rsp_status;
    cmd_rsp.client_context = p_disable_cmd_params->client_context;
    vs_common_send_event( p_session_obj->header.handle,
                          VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
  }

  return VS_EOK;
}  /*  end of vs_voc_cmd_disable_proc () */

VS_INTERNAL uint32_t vs_voc_cmd_flush_buffers_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_voc_cmdi_flush_buffers_t args;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmd_flush_buffers_t* p_flush_cmd_params = NULL;
  vs_session_object_t* p_session_obj = NULL;

  for( ;; )
  {
    /* Do cmd_packet validation. */
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do cmd_param validation. */
    p_flush_cmd_params = ( vs_voc_cmd_flush_buffers_t* )p_cmd_packet->params;
    if( p_flush_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do session handle validation. */
    rc = vs_get_typed_object( p_flush_cmd_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      rsp_status = APR_EFAILED;
      break;
    }

    if ( p_session_obj->is_close_invoked )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_cmd_flush_buffers_proc(): VS_VOC_CMD_FLUSH_BUFFERS isn't "
             "permitted after receiving VS_VOC_CMD_CLOSE!!!!" );
      rc = VS_EFAILED;
      break;
    }

    args.handle = p_flush_cmd_params->handle;
    args.client_context = p_flush_cmd_params->client_context;
    rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_LOW,
                                              VS_VOC_CMDI_FLUSH_BUFFERS, &args,
                                              sizeof( args ) );
    if ( rc ) rsp_status = rc;
    break;
  }

  if ( rsp_status && ( p_flush_cmd_params != NULL ) )
  {
    /* Inform command status to client */
    cmd_rsp.cmd_id = VS_VOC_CMD_FLUSH_BUFFERS;
    cmd_rsp.status_id = rsp_status;
    cmd_rsp.client_context = p_flush_cmd_params->client_context;
    vs_common_send_event( p_session_obj->header.handle,
                          VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
  }

  return APR_EOK;
}

VS_INTERNAL uint32_t vs_voc_cmd_close_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmdi_close_t args;
  vs_voc_cmd_close_t* p_close_cmd_params = NULL;
  vs_session_object_t* p_session_obj = NULL;

  for( ;; )
  {
    /* Do cmd_packet validation. */
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do cmd_param validation. */
    p_close_cmd_params = ( vs_voc_cmd_close_t* )p_cmd_packet->params;
    if( p_close_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = VS_EFAILED;
      break;
    }

    /* Do session handle validation. */
    rc = vs_get_typed_object( p_close_cmd_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      rsp_status = APR_EFAILED;
      break;
    }

    if ( p_session_obj->call_state != VS_CALL_STATE_ENUM_RESET )
    {
      rsp_status = VS_EFAILED;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_close_proc(): Client should issue VS_VOC_CMD_DISABLE "
           "before closing the VS session. " );
      break;
    }

    if ( p_session_obj->is_close_invoked )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_close_proc(): VS_VOC_CMD_CLOSE isn't permitted after "
           "receiving VS_VOC_CMD_CLOSE!!!!" );
      rsp_status = VS_EFAILED;
      break;
    }

    /**
     * After receiving close, no new commands.
     */
    p_session_obj->is_close_invoked = TRUE;

    /**
     * TODO: VS COMMAND Throttling.
     *
     * NOTE: During Scenario where ADSP gets unreponsive, this
     * VS_VOC_CMDI_CLOSE should not be throlled because this may
     * lead stale session in CVD if VS client closes the current
     * VS_SESSION.
     */

    /* Reuse the same command packet. */
    args.handle = p_close_cmd_params->handle;
    args.client_context = p_close_cmd_params->client_context;
    rc =  vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_LOW,
                                               VS_VOC_CMDI_CLOSE, &args,
                                               sizeof( args  ) );
    if ( rc ) rsp_status = rc;
    
    break;
  }

  if ( rsp_status && ( p_close_cmd_params != NULL ) )
  {
    /* Send response if not able to Q CMDI. */
    cmd_rsp.cmd_id = VS_VOC_CMD_CLOSE;
    cmd_rsp.status_id = rsp_status;
    cmd_rsp.client_context = p_close_cmd_params->client_context;
    vs_common_send_event( p_close_cmd_params->handle,
                          VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
  }

  return APR_EOK;
}

/****************************************************************************
 * P3 GATING COMMAND PROCESSING FUNCTIONS                                   *
 ****************************************************************************/


VS_INTERNAL uint32_t vs_voc_cmdi_enable_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_voc_cmdi_enable_t* p_cmdi_enable_params = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_sequencer_job_object_t* p_seq_obj = NULL;
  vs_voc_state_enum_t session_state;

  for ( ;; )
  {
    p_cmd_packet = ( vs_cmd_packet_t* ) ctrl->packet ;
    if ( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    p_cmdi_enable_params = ( ( vs_voc_cmdi_enable_t* ) p_cmd_packet->params );
    if ( p_cmdi_enable_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    /* validate the session handle */
    rc = vs_get_typed_object( p_cmdi_enable_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    VS_PANIC_ON_ERROR( rc );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      /* Verify if the call state is still IDLE or ACTIVE. */
      if ( vs_get_call_state( p_session_obj ) == VS_CALL_STATE_ENUM_RESET )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_enable_proc() - VS session with handle=(0X%08X) "
               "is in VS_CALL_STATE_ENUM_RESET", p_cmdi_enable_params->handle );
        break;
      }

      /* Verify if resource are still available, and CVD is UP. */
      if ( ( p_session_obj->is_resource_granted == FALSE ) ||
           ( vs_is_cvd_cleanup_done == FALSE ) ||
           ( vs_is_cvd_service_up == FALSE ) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_enable_proc() - either CVD service is Down or "
               "resource=(%d) granted to VS session wtih handle=(0X%08X)",
               vs_is_cvd_service_up, p_session_obj->is_resource_granted,
               p_cmdi_enable_params->handle );
        rsp_status = VS_ENEEDMORE;
        break;
      }

      /* Create the Sequencer job object to track CVD setup. */
      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );

      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      session_state = p_session_obj->session_ctrl.state;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      if ( session_state == VS_VOC_STATE_ENUM_RESET )
      {
        /* If not CVD resource created. */
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE;
      }
      else
      {
        VS_ACQUIRE_LOCK( p_session_obj->common_lock );
        if ( p_session_obj->pending_media_id != p_session_obj->media_id )
        {
          /* If change in media_id recongifure CVD. */
          p_session_obj->media_id = p_session_obj->pending_media_id;

          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE;
        }
        else
        {
          p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE;
        }
        VS_RELEASE_LOCK( p_session_obj->common_lock );
      }
    }

    p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

    rc = vs_voc_helper_vocoder_setup_ctrl( p_seq_obj, p_session_obj );

    break;
  }

  if ( rc == VS_EOK )
  {
    if ( p_seq_obj != NULL )
    {
      rsp_status = p_seq_obj->status;
      ( void ) vs_mem_free_object( ( vs_object_t* ) p_seq_obj );
      p_seq_obj = NULL;
    }

    if ( ( p_cmdi_enable_params != NULL ) &&
         ( p_cmdi_enable_params->is_rsp_pending == TRUE ) )
    {
     /* Inform command status to client */
       cmd_rsp.cmd_id = VS_VOC_CMD_ENABLE;
       cmd_rsp.status_id = rsp_status;
       cmd_rsp.client_context = p_cmdi_enable_params->client_context;
       vs_common_send_event( p_cmdi_enable_params->handle,
                             VS_COMMON_EVENT_CMD_RESPONSE,
                             &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    }

    ( void ) vs_free_cmd_packet ( p_cmd_packet );
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmdi_standby_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_sequencer_job_object_t* p_seq_obj = NULL;
  vs_voc_cmdi_standby_t* p_cmdi_standby_params = NULL;
  vs_voc_state_enum_t session_state;

  for ( ;; )
  {
    p_cmd_packet = ( vs_cmd_packet_t* ) ctrl->packet ;
    if ( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    p_cmdi_standby_params = ( ( vs_voc_cmdi_standby_t* ) p_cmd_packet->params );
    if ( p_cmdi_standby_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    rc = vs_get_typed_object( p_cmdi_standby_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    VS_PANIC_ON_ERROR( rc );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      if ( vs_get_call_state( p_session_obj ) == VS_CALL_STATE_ENUM_RESET )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_standby_proc() - VS session with handle=(0X%08X) "
               "is in VS_CALL_STATE_ENUM_RESET", p_session_obj->header.handle );
        break;
      }

      if ( ( p_session_obj->is_resource_granted == FALSE ) ||
           ( vs_is_cvd_service_up == FALSE ) ||
           ( vs_is_cvd_cleanup_done == FALSE ) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_standby_proc() - either CVD service is Down or "
               "resource=(%d) granted to VS session wtih handle=(0X%08X)",
               vs_is_cvd_service_up, p_session_obj->is_resource_granted,
               p_session_obj->header.handle );
        rsp_status = VS_ENEEDMORE;
        break;
      }

      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );

      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      session_state = p_session_obj->session_ctrl.state;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      if ( session_state == VS_VOC_STATE_ENUM_RESET )
      {
        /* If not CVD resource created. */
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE;
      }
      else
      {
        /**
         *  By default the first goal should be Disable, if CVD session
         *  are already created.
         */
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE;
      }
    }

    p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

    rc = vs_voc_helper_vocoder_setup_ctrl( p_seq_obj, p_session_obj );

    break;
  }

  if ( rc == VS_EOK )
  {
    if ( p_seq_obj != NULL )
    {
      rsp_status = p_seq_obj->status;
      ( void ) vs_mem_free_object( ( vs_object_t* ) p_seq_obj );
      p_seq_obj = NULL;
    }

    if ( ( p_cmdi_standby_params != NULL ) &&
         ( p_cmdi_standby_params->is_rsp_pending == TRUE ) )
    {
     /* Inform command status to client */
       cmd_rsp.cmd_id = VS_VOC_CMD_STANDBY;
       cmd_rsp.status_id = rsp_status;
       cmd_rsp.client_context = p_cmdi_standby_params->client_context;
       vs_common_send_event( p_cmdi_standby_params->handle,
                             VS_COMMON_EVENT_CMD_RESPONSE,
                             &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    }

    ( void ) vs_free_cmd_packet ( p_cmd_packet );
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmdi_disable_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK; /* return code. */
  uint32_t rsp_status = APR_EOK;/* response. */
  vs_common_event_cmd_response_t cmd_rsp;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_sequencer_job_object_t* p_seq_obj = NULL;
  vs_voc_cmdi_disable_t* p_cmdi_disable_params = NULL;
  vs_voc_state_enum_t session_state;

  for ( ;; )
  {
    p_cmd_packet = ( vs_cmd_packet_t* ) ctrl->packet ;
    if ( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    p_cmdi_disable_params = ( ( vs_voc_cmdi_disable_t* ) p_cmd_packet->params );
    if ( p_cmdi_disable_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    rc = vs_get_typed_object( p_cmdi_disable_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    VS_PANIC_ON_ERROR( rc );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      if ( ( vs_is_cvd_service_up == FALSE ) ||
           ( vs_is_cvd_cleanup_done == FALSE ) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_disable_proc() - either CVD service is Down or "
               "resource=(%d) granted to VS session wtih handle=(0X%08X)",
               vs_is_cvd_service_up, p_session_obj->is_resource_granted,
               p_session_obj->header.handle );
        rsp_status = VS_ENEEDMORE;
        break;
      }

      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );

      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      session_state = p_session_obj->session_ctrl.state;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      if ( session_state == VS_VOC_STATE_ENUM_RESET )
      {
        /* If not CVD resource created. */
        p_seq_obj->status = APR_EOK;
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
      }
      else
      {
        /**
         *  By default the first goal should be Disable, if CVD state is
         *  not VS_VOC_STATE_ENUM_RESET.
         */
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE;
      }
    }

    p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

    rc = vs_voc_helper_vocoder_setup_ctrl( p_seq_obj, p_session_obj );

    break;
  }

  if ( rc == VS_EOK )
  {
    if ( p_seq_obj != NULL )
    {
      rsp_status = p_seq_obj->status;
      ( void ) vs_mem_free_object( ( vs_object_t* ) p_seq_obj );
      p_seq_obj = NULL;
    }

    if ( ( p_cmdi_disable_params != NULL ) &&
         ( p_cmdi_disable_params->is_rsp_pending == TRUE ) )
    {
     /* Inform command status to client */
       cmd_rsp.cmd_id = VS_VOC_CMD_DISABLE;
       cmd_rsp.status_id = rsp_status;
       cmd_rsp.client_context = p_cmdi_disable_params->client_context;
       vs_common_send_event( p_cmdi_disable_params->handle,
                             VS_COMMON_EVENT_CMD_RESPONSE,
                             &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    }

    ( void ) vs_free_cmd_packet ( p_cmd_packet );
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmdi_close_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t rsp_status = APR_EOK;
  vs_voc_cmdi_destroy_session_t args;
  vs_common_event_cmd_response_t cmd_rsp;  
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_simple_job_object_t* p_simple_obj = NULL;
  vs_voc_cmdi_close_t* p_close_cmd_params = NULL;

  for ( ;; )
  {
    /* get the close command params */
    p_cmd_packet = ( vs_cmd_packet_t* ) ctrl->packet;
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    p_close_cmd_params = ( vs_voc_cmdi_close_t* )p_cmd_packet->params;
    if ( p_close_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      rsp_status = APR_EFAILED;
      break;
    }

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      /* validate the session handle */
      rc = vs_get_typed_object( p_close_cmd_params->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      if ( rc )
      {
        VS_REPORT_FATAL_ON_ERROR( rc );
        rsp_status = APR_EFAILED;
        break;
      }

      /* Create the simple job object to track cmd. */
      rc = vs_create_simple_job_object( p_session_obj->header.handle,
             ( vs_simple_job_object_t** ) &ctrl->rootjob_obj );

      p_simple_obj = &ctrl->rootjob_obj->simple_job;


      args.handle = p_close_cmd_params->handle;
      args.token = p_simple_obj->header.handle;
      args.client_context = p_close_cmd_params->client_context;
      rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_MED,
                                                VS_VOC_CMDI_DESTROY_SESSION,
                                                &args, sizeof( args ) );
      if ( rc )
      {
        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
        rc = VS_EFAILED;
      }
      else
      {
        rc = VS_EPENDING;
      }
    }
    else
    {
      p_simple_obj = &ctrl->rootjob_obj->simple_job;

      if ( p_simple_obj->is_completed == TRUE )
      {
        rsp_status = p_simple_obj->status;
        rc = VS_EOK;

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );
        ( void ) vs_free_cmd_packet ( p_cmd_packet );
        p_cmd_packet = NULL;
      }
      else
      {
        rc = VS_EPENDING;
      }
    }

    break;
  }

  if( ( rsp_status != VS_EOK ) && ( p_close_cmd_params != NULL ) )
  {
    /* Inform command status to client. */
    cmd_rsp.cmd_id = VS_VOC_CMD_CLOSE;
    cmd_rsp.status_id = rsp_status;
    cmd_rsp.client_context = p_close_cmd_params->client_context;
    vs_common_send_event( p_close_cmd_params->handle,
                          VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    ( void ) vs_free_cmd_packet ( p_cmd_packet );

    rc = VS_EOK;
  }

  return rc;
}  /* end of vs_voc_cmdi_close_proc() */

VS_INTERNAL uint32_t vs_voc_cmdi_destroy_session_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = APR_EOK;
  vs_voc_rspi_destroy_session_t args;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmdi_destroy_session_t* p_destroy_session_cmd_params = NULL;
  vs_generic_item_t* p_generic_item = NULL;
  vs_session_object_t* p_session_obj = NULL;
  apr_lock_t* client_session_lock_p = NULL;

  for ( ;; )
  {
    /* get the command params */
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    p_destroy_session_cmd_params = ( vs_voc_cmdi_destroy_session_t* )p_cmd_packet->params;
    if ( p_destroy_session_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }
    /* validate the session handle */
    rc = vs_get_typed_object( p_destroy_session_cmd_params->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    if ( p_session_obj != NULL )
    {
      /* Lock session object destroy process */
      client_session_lock_p = vs_get_session_destroy_lock( p_destroy_session_cmd_params->handle );
      if ( client_session_lock_p != NULL )
      {
        VS_ACQUIRE_LOCK( *client_session_lock_p );
      }

      /* Thread lock is acquired to avoid concurrent access of session queues
       * by VS LOW/MED/HIGH tasks.
       */
      ( void )apr_lock_enter( vs_voc_cmd_thread_lock );

      /* Destroy Session */
      p_generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );

      for ( ;; )
      {
        rc = apr_list_get_next( &vs_used_session_q,
                                ( ( apr_list_node_t* ) p_generic_item ),
                                ( ( apr_list_node_t** ) &p_generic_item ) );
        if ( rc ) break;

        if ( p_generic_item->handle == p_session_obj->header.handle )
        {
          ( void ) apr_list_delete( &vs_used_session_q,
                                    ( apr_list_node_t* ) p_generic_item );
          ( void ) apr_list_add_tail( &vs_free_session_q,
                                      &p_generic_item->link );
          break;
        }
      }  /* end of for ( ;; ) */

      ( void )apr_lock_leave( vs_voc_cmd_thread_lock );

      if ( p_session_obj->alloc_buf_q.size > 0 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmdi_destroy_session_proc() - memleak: (%d) vocoder "
               "buffer not freed by client", p_session_obj->alloc_buf_q.size );
      }

      ( void ) apr_list_destroy( &p_session_obj->alloc_buf_q );
      ( void ) apr_list_destroy( &p_session_obj->write_buf_q );
      ( void ) apr_list_destroy( &p_session_obj->read_buf_q );
      ( void ) apr_lock_destroy( p_session_obj->session_ctrl_lock );
      ( void ) apr_lock_destroy( p_session_obj->client_state_lock );
      ( void ) apr_lock_destroy( p_session_obj->common_lock );

      /* Inform command status to client */
      cmd_rsp.cmd_id = VS_VOC_CMD_CLOSE;
      cmd_rsp.status_id = VS_EOK;
      cmd_rsp.client_context = p_destroy_session_cmd_params->client_context;
      vs_common_send_event( p_destroy_session_cmd_params->handle,
                            VS_COMMON_EVENT_CMD_RESPONSE,
                            &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );

      /* Free the memory - p_args. */
      ( void ) vs_mem_free_object( (vs_object_t*) p_session_obj );
      p_session_obj = NULL;

      /* release client session lock. */
      if ( client_session_lock_p != NULL )
      {
        VS_RELEASE_LOCK( *client_session_lock_p );
      }
      vs_clear_session_destroy_lock( p_destroy_session_cmd_params->handle );

      /* Send response to VS_VOC_CMDI_DESTROY_SESSION. */
      args.status = VS_EOK;
      args.token = p_destroy_session_cmd_params->token;
      rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_LOW,
                                                VS_VOC_RSPI_DESTROY_SESSION,
                                                &args, sizeof( args ) );
      }

    break;
  }

  return APR_EOK;
}  /* end of vs_voc_cmdi_destroy_session_proc() */

VS_INTERNAL uint32_t vs_voc_rspi_destroy_proc (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = APR_EOK;
  vs_voc_rspi_destroy_session_t*  p_destroy_rsp_params;
  vs_simple_job_object_t* p_simple_obj;

  for ( ;; )
  {
    /* get the command params */
    if( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    p_destroy_rsp_params = ( vs_voc_rspi_destroy_session_t* ) p_cmd_packet->params;
    if ( p_destroy_rsp_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    rc = vs_get_typed_object( p_destroy_rsp_params->token,
                              VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &p_simple_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    p_simple_obj->is_completed = TRUE;
    p_simple_obj->status = p_destroy_rsp_params->status;

    break;
  }

  return APR_EOK;
}

VS_INTERNAL uint32_t vs_voc_cmdi_flush_buffers_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = VS_EOK;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_common_event_cmd_response_t cmd_rsp;
  vs_voc_cmdi_flush_buffers_t* p_flush_cmd_params = NULL;
  vs_voc_buffer_item_t* p_voc_buf_node = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_voc_buffer_t* p_voc_buffer = NULL;
  apr_lock_t* session_lock_p = NULL;

  for ( ;; )
  {
    /* Get the cmd packet. */
    p_cmd_packet = ( vs_cmd_packet_t*) ctrl->packet;
    if ( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    p_flush_cmd_params = ( vs_voc_cmdi_flush_buffers_t* )p_cmd_packet->params;
    if ( p_flush_cmd_params == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object ( p_flush_cmd_params->handle,
                               VS_OBJECT_TYPE_ENUM_SESSION,
                               ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }
    session_lock_p = vs_get_session_destroy_lock( p_flush_cmd_params->handle );
	
    /* Session lock is acquired to avoid concurrent access of 
     * buffer queues from VS and VS client context.
     */
    if ( session_lock_p != NULL )
    {
        VS_ACQUIRE_LOCK( *session_lock_p );
    }

    /* Make all node in alloc_buf_q as VS_VOC_BUFFER_TYPE_NONE.
     * None of the buffer available contians valid vocder frame
     * data.
     */
    p_voc_buf_node =
       ( vs_voc_buffer_item_t* )( &p_session_obj->alloc_buf_q.dummy );

    for( ;; )
    {
      rc = apr_list_get_next( &p_session_obj->alloc_buf_q,
                              ( ( apr_list_node_t* ) p_voc_buf_node ),
                              ( ( apr_list_node_t** ) &p_voc_buf_node ) );
      if ( rc ) break;
      p_voc_buf_node->buf_type = VS_VOC_BUFFER_TYPE_NONE;
    }

    /**
     * Only buffer marked as VS_VOC_BUFFER_TYPE_READ contains valid ADSP
     * encoder frame data. These are available only in read_buf_q.
     *
     * Send only one read available event irrespective of number of
     * buffers available in read_buf_q.
     *
     * Client should call VS_VOC_CMD_READ_BUFFER continuosuly until
     * it gets VS_NOTEXIST from VS.
     */
    if ( p_session_obj->read_buf_q.size > 0 )
    {
      p_session_obj->event_cb( VS_VOC_EVENT_READ_AVAILABLE, NULL, 0,
                               p_session_obj->session_context );
    }

    /* Flush voc_buffer from Write buffer queue if any */
    if( p_session_obj->write_buf_q.size > 0 )
    {
      for( ;; )
      {
        rc = apr_list_remove_head( &p_session_obj->write_buf_q,
                                   ( ( apr_list_node_t** ) &p_voc_buf_node ) );
        if ( rc )
        {
          rc = VS_EOK;
          break;
        }

        if ( p_voc_buf_node == NULL )
        {
          VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
          continue;
        }

        p_voc_buffer = ( ( vs_voc_buffer_t* ) p_voc_buf_node->buf );

        /* Send write buffer return event(s)
         * Number of EVENT_WRITE_BUFFER_RETURNED sent will be equal to
         * number of write buffers available in the queue.
         */
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "vs_voc_cmdi_flush_buffers_proc (): VS_VOC_EVENT_WRITE_BUFFER_RETURNED "
               "sent, buf=(0x%08x)", p_voc_buffer );

        vs_common_send_event( p_session_obj->header.handle,
                              VS_VOC_EVENT_WRITE_BUFFER_RETURNED,
                              p_voc_buffer,
                              sizeof( vs_voc_event_write_buffer_returned_t ) );

        p_voc_buf_node->buf_type = VS_VOC_BUFFER_TYPE_NONE;

        /* Move this node back to alloc_buf_q. */
        ( void ) apr_list_add_tail( &p_session_obj->alloc_buf_q,
                                    &p_voc_buf_node->link );
        p_voc_buf_node = NULL;

      } /* end of for( ;; ) */
    }
    else
    {
      /* Send one dummy write buffer return event to client if the write_buf_q
       * is empty.
       */
        vs_common_send_event( p_session_obj->header.handle,
                              VS_VOC_EVENT_WRITE_BUFFER_RETURNED, NULL, 0 );
    }  /* end of if( session_obj->write_buf_q.size > 0 ) */

    if ( session_lock_p != NULL )
    {
        VS_RELEASE_LOCK( *session_lock_p );
    }
    break;
  }  /* end of for ( ;; ) */

  if ( p_flush_cmd_params != NULL )
  {
  /* Now send the command response */
  cmd_rsp.cmd_id = VS_VOC_CMD_FLUSH_BUFFERS;
  cmd_rsp.status_id = VS_EOK;
  cmd_rsp.client_context = p_flush_cmd_params->client_context;
  vs_common_send_event( p_flush_cmd_params->handle,
                        VS_COMMON_EVENT_CMD_RESPONSE, &cmd_rsp,
                        sizeof( vs_common_event_cmd_response_t ) );
  }

  ( void ) vs_free_cmd_packet ( p_cmd_packet );

  return APR_EOK;
}  /* end of vs_voc_cmdi_flush_buffers_proc() */


VS_INTERNAL uint32_t vs_voc_cmdi_disable_cvd_on_adsp_recovery_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_generic_item_t* p_generic_item = NULL;
  vs_sequencer_job_object_t* p_seq_obj = NULL;
  vs_voc_state_enum_t session_state;

  for ( ;; )
  {
    p_cmd_packet = ( ( vs_cmd_packet_t* ) ctrl->packet );
    if ( p_cmd_packet == NULL ) VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      /* Verify if resource are still available, and CVD is UP. */
      if ( vs_is_cvd_service_up == FALSE )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_disable_cvd_on_adsp_recovery_proc(): "
               "CVD service is UP(1)/Down(0)=(%d)", vs_is_cvd_service_up );
        rc  = VS_EOK;

        ( void ) vs_free_cmd_packet ( p_cmd_packet );
        break;
      }

      p_generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );
      rc = apr_list_get_next( &vs_used_session_q,
                              ( ( apr_list_node_t* ) p_generic_item ),
                              ( ( apr_list_node_t** ) &p_generic_item ) );
      if ( rc )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_voc_cmdi_disable_cvd_on_adsp_recovery_proc(): ALL "
             "VS session traversed" );
        
        p_cmd_packet->params = NULL;
        rc  = VS_EOK;
        ( void ) vs_free_cmd_packet ( p_cmd_packet );
        break;
      }

      /**
       * We needs to traverse all the VS session objects, and perform
       * VS_VOC_ENUM_GOAL_DISABLE on them to stop the CVD sessions.
       *
       * In order to track the pivot node we will store it in the params
       * field of CMD packet.
       */
      p_cmd_packet->params = ( ( void* ) p_generic_item );

      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );

      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      VS_PANIC_ON_ERROR( rc );

      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      session_state = p_session_obj->session_ctrl.state;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      if ( session_state == VS_VOC_STATE_ENUM_RESET )
      {
        /* If not CVD resource created. */
        p_seq_obj->status = APR_EOK;
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
      }
      else
      {
        /**
         *  By default the first goal should be Disable, if CVD state is
         *  not VS_VOC_STATE_ENUM_RESET.
         */
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE;
      }
    }

    for ( ;; )
    {
      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      /* Retrieve the p_generic_item. */
      p_generic_item = ( ( vs_generic_item_t* ) p_cmd_packet->params );
      if ( p_generic_item == NULL ) VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );

      rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      VS_PANIC_ON_ERROR( rc );

      rc = vs_voc_helper_vocoder_setup_ctrl( p_seq_obj, p_session_obj );
      if ( rc ) break;

      rc = apr_list_get_next( &vs_used_session_q,
                              ( ( apr_list_node_t* ) p_generic_item ),
                              ( ( apr_list_node_t** ) &p_generic_item ) );
      /* Handling ADSP timeout during cleanup. */
      if ( rc || ( vs_is_adsp_timeout_triggered == TRUE ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_voc_cmdi_disable_cvd_on_adsp_recovery_proc(): ALL "
             "VS session traversed" );

        /* Ending the cleanup/recovery sequece as new timeout has been 
         * triggered. */
        if ( vs_is_adsp_timeout_triggered == FALSE )
        {
          ( void ) vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
                      VS_VOC_CMDI_DESTROY_CVD_ON_ADSP_RECOVERY, NULL, 0 );
        }
        
        p_cmd_packet->params = NULL;
        rc  = VS_EOK;

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_seq_obj );
        ( void ) vs_free_cmd_packet ( p_cmd_packet );

        break;
      }

      /* Cache the p_generic_item. */
      p_cmd_packet->params = ( ( void* ) p_generic_item );

      rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      VS_PANIC_ON_ERROR( rc );

      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      if ( p_session_obj->session_ctrl.state == VS_VOC_STATE_ENUM_RESET )
      {
        /* If not CVD resource created. */
        p_seq_obj->status = APR_EOK;
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
      }
      else
      {
        /**
         *  By default the first goal should be Disable, if CVD state is
         *  not VS_VOC_STATE_ENUM_RESET. */
        p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE;
      }
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );
    } /* inner for loop ends here. */

    break;
  }

  return rc;
}

VS_INTERNAL uint32_t vs_voc_cmdi_destroy_cvd_on_adsp_recovery_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vs_generic_item_t* p_generic_item = NULL;
  vs_sequencer_job_object_t* p_seq_obj = NULL;

  for ( ;; )
  {
    p_cmd_packet = ( ( vs_cmd_packet_t* ) ctrl->packet );
    if ( p_cmd_packet == NULL ) VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {

      /* Verify if resource are still available, and CVD is UP. */
      if ( vs_is_cvd_service_up == FALSE )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vs_voc_cmdi_destroy_cvd_on_adsp_recovery_proc(): "
               "CVD service is UP(1)/Down(0)=(%d)", vs_is_cvd_service_up );
        rc  = VS_EOK;

        ( void ) vs_free_cmd_packet ( p_cmd_packet );
        break;
      }

      p_generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );
      rc = apr_list_get_next( &vs_used_session_q,
                              ( ( apr_list_node_t* ) p_generic_item ),
                              ( ( apr_list_node_t** ) &p_generic_item ) );
      if ( rc )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_voc_cmdi_destroy_cvd_on_adsp_recovery_proc(): ALL "
             "VS session traversed" );

        /* Ending the cleanup/recovery sequece as new timeout has been 
         * triggered. */
        if ( vs_is_adsp_timeout_triggered == FALSE )
        {
          vs_is_cvd_cleanup_done = TRUE;
        }

        p_cmd_packet->params = NULL;
        rc  = VS_EOK;
        ( void ) vs_free_cmd_packet ( p_cmd_packet );

        break;
      }

      /**
       * We needs to traverse all the VS session objects, and perform
       * VS_VOC_GOAL_ENUM_DESTROY on them to Destroy the CVD sessions
       * for VS cleanup.
       *
       * In order to track the pivot node we will store it in the params
       * field of CMD packet.
       */
      p_cmd_packet->params = ( ( void* ) p_generic_item );

      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );

      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      VS_PANIC_ON_ERROR( rc );

      /**
       * Execute the goal destroy for each VS session.
       */
      p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY;

    }

    for ( ;; )
    {
      p_seq_obj = &ctrl->rootjob_obj->sequencer_job;

      /* Retrieve the p_generic_item. */
      p_generic_item = ( ( vs_generic_item_t* ) p_cmd_packet->params );
      if ( p_generic_item == NULL ) VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );

      rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
      VS_PANIC_ON_ERROR( rc );

      rc = vs_voc_helper_vocoder_setup_ctrl( p_seq_obj, p_session_obj );

      if ( rc ) break;

      /* Get the next VS from the session queue. */
      rc = apr_list_get_next( &vs_used_session_q,
                              ( ( apr_list_node_t* ) p_generic_item ),
                              ( ( apr_list_node_t** ) &p_generic_item ) );
      
      /* Handling ADSP timeout during cleanup. */
      if ( rc || ( vs_is_adsp_timeout_triggered == TRUE ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_voc_cmdi_destroy_cvd_on_adsp_recovery_proc(): ALL "
             "VS session traversed" );

        /* Ending the cleanup/recovery sequece as new timeout has been 
         * triggered. */
        if ( vs_is_adsp_timeout_triggered == FALSE )
        {
          vs_is_cvd_cleanup_done = TRUE;
          ( void ) vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
                      VS_VOC_CMDI_RESTORE_CVD_STATE, NULL, 0 );
        }
        p_cmd_packet->params = NULL;
        rc  = VS_EOK;

        ( void ) vs_mem_free_object( ( vs_object_t* ) p_seq_obj );
        ( void ) vs_free_cmd_packet ( p_cmd_packet );

        break;
      }

      /* Cache the p_generic_item. */
      p_cmd_packet->params = ( ( void* ) p_generic_item );

      /**
       * Execute the goal destroy for each VS session.
       */
      p_seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY;
    } /*Inner loop ends here. */

    break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cmdi_cleanup_cvd_reference_proc (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  vs_cmd_packet_t* ssr_cleanup_cmd_pkt;
  vs_session_object_t* session_obj;
  vs_generic_item_t* generic_item;

  ssr_cleanup_cmd_pkt = ( ( vs_cmd_packet_t* ) ctrl->packet );

  /* Thread lock is acquired to avoid concurrent access of session queues
   * by VS LOW/MED/HIGH tasks.
   */
  ( void )apr_lock_enter( vs_voc_cmd_thread_lock );

  { /* Clean up the stale ADSP handles and all tracked objects. */
    generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &vs_used_session_q,
                              ( ( apr_list_node_t* ) generic_item ),
                              ( ( apr_list_node_t** ) &generic_item ) );
      if ( rc )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_voc_cmdi_cleanup_cvd_reference_proc(): ALL "
             "VS session traversed" );
        break;
      }

      rc = vs_get_typed_object( generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &session_obj ) );
      if ( rc )
      {
        VS_REPORT_FATAL_ON_ERROR( rc );
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_voc_cmdi_cleanup_cvd_reference_proc(): Invalid hanlde=(0X%8X) "
               "QUEUED to vs_used_session_q", generic_item->handle );
      }
      else
      {
        session_obj->cvs_port = APR_NULL_V;
        session_obj->mvm_port = APR_NULL_V;
        /**
         * Clean all the session ctrl variables
         */
        VS_ACQUIRE_LOCK( session_obj->session_ctrl_lock );
        session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET;
        VS_RELEASE_LOCK( session_obj->session_ctrl_lock );
        if ( session_obj->stream_ready == TRUE )
        {
          session_obj->stream_ready = FALSE;
          ( void ) vs_common_send_event( session_obj->header.handle,
                                         VS_COMMON_EVENT_NOT_READY, NULL, 0 );
        }
      }
    }
    ( void )apr_lock_leave( vs_voc_cmd_thread_lock );

    /* Ensure vs_voc_cmd_thread_lock is released before calling below function.
     * vs_voc_cmd_thread_lock is also used in vs_voc_ssr_track_object() and 
     * vs_voc_ssr_untrack_object().
     */
    rc = vs_voc_ssr_free_all_tracked_objects( );
  }

  if ( ssr_cleanup_cmd_pkt != NULL )
  {
    apr_memmgr_free( &vs_heapmgr, ssr_cleanup_cmd_pkt );
  }

  vs_is_cvd_cleanup_done = TRUE;

  return APR_EOK;
}

VS_INTERNAL uint32_t vs_voc_cmdi_restore_cvd_state_proc (
  vs_gating_control_t* ctrl
)
{
 uint32_t rc = APR_EOK;
 vs_voc_cmdi_enable_t enable_args;
 vs_voc_cmdi_standby_t standby_args;
 vs_session_object_t* p_session_obj = NULL;
 vs_generic_item_t* p_generic_item = NULL;
 vs_call_state_enum_t call_state;

 /**
  * Traverse the entire VS Session and queue the appropiate commands
  * as per the call state. ( CMDI_ENABLE for CALL_STATE_ENABLED ) and
  * ( CMDI_STANDBY for CALL_STATE_IDLE ).
  */

 /* Thread lock is acquired to avoid concurrent access of session queues
  * by VS LOW/MED/HIGH tasks.
  */
 ( void )apr_lock_enter( vs_voc_cmd_thread_lock );

 p_generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );
 for ( ;; )
 {
   rc = apr_list_get_next( &vs_used_session_q,
                           ( ( apr_list_node_t* ) p_generic_item ),
                           ( ( apr_list_node_t** ) &p_generic_item ) );
   if ( rc )
   {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "vs_voc_cmdi_restore_cvd_state_proc(): ALL VS session traversed" );
     break;
   }

   rc = vs_get_typed_object( p_generic_item->handle,
                                VS_OBJECT_TYPE_ENUM_SESSION,
                                ( ( vs_object_t** ) &p_session_obj ) );
   call_state = vs_get_call_state( p_session_obj );
   if( call_state == VS_CALL_STATE_ENUM_ENABLED )
   {
     enable_args.handle = p_session_obj->header.handle;
     rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
                                              VS_VOC_CMDI_ENABLE, &enable_args,
                                              sizeof( enable_args ) );
   }
   else if( call_state == VS_CALL_STATE_ENUM_IDLE )
   {
     standby_args.handle = p_session_obj->header.handle;
     rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
                                              VS_VOC_CMDI_STANDBY, &standby_args,
                                              sizeof( standby_args ) );
   }
 }
 ( void )apr_lock_leave( vs_voc_cmd_thread_lock );

 ( void ) vs_free_cmd_packet ( ctrl->packet );

 return APR_EOK;
}


/****************************************************************************
 * SYSTEM EVENTS HANDLERS ( CVD UP-DOWN )/( ADSP SSR )/( VOICE PD UP-DOWN ) *
 ****************************************************************************/

static void vs_voc_ssr_adsp_before_shutdown_handler (
  void
)
{
  uint32_t rc = VS_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "vs_voc_ssr_adsp_before_shutdown_handler is called." );

  /*
   * For hanlding Back to Back SSR we should make sure to skip it if the
   * first one is still not processed.
   *
   * Incase SSR is triggered post ADSP timeout, this should be taken care to
   * queue the VS_VOC_CMDI_CLEANUP_CVD_REFERENCES.
   */
  if( ( vs_is_cvd_cleanup_done == FALSE ) &&
      ( vs_is_adsp_timeout_triggered == FALSE ) )
  {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_ssr_adsp_before_shutdown_handler(): an SSR request queued "
          "earlier is under processing." );
    return;
  }

  vs_is_cvd_cleanup_done = FALSE;
  vs_is_cvd_service_up = FALSE;
  vs_is_adsp_timeout_triggered = FALSE;

  rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
                                           VS_VOC_CMDI_CLEANUP_CVD_REFERENCES,
                                           NULL, 0 );
  VS_REPORT_FATAL_ON_ERROR ( rc );

  return;
}

static void vs_voc_ssr_adsp_after_powerup_handler (
  void
)
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "vs_voc_ssr_adsp_after_powerup_handler is called." );

  /**
   * SSR UP event does not requires to restore the vocoder state of each VS
   * session as per the call state as of now.
   *
   * NOTE/TBD: This would be required when voice call continuity feature
   * needs to be supported and target supported PD -Restart and APR
   * Service UP/DOWN events.
   */


  /* For Target that do not have Multi-PD, there is no method to
   * detect if CVD Services are actually up post SSR.
   *
   * this variable is made default TRUE post SSR cleanup.
   */
  vs_is_cvd_service_up = TRUE;
}

/* Track the allocated VS objects with type VS_OBJECT_TYPE_ENUM_SIMPLE_JOB and
 * VS_OBJECT_TYPE_ENUM_HANDLE, in order to free them in case of ADSP SSR. VS
 * allocates job object with one of the above types when communicating with the
 * ADSP and frees the object once a response is received from the ADSP. If VS
 * has issued commands to the ADSP and ADSP SSR occurs, VS will not get
 * responses from the ADSP and thus the objects will not be freed. Therefore
 * VS needs to track these types of allocated objects and free them upon ADSP
 * SSR.
 */
VS_INTERNAL uint32_t vs_voc_ssr_track_object (
  uint32_t obj_handle
)
{
  int32_t rc = VS_EOK;
  vs_generic_item_t* generic_item = NULL;

  /* Thread lock is acquired to avoid concurrent access of track object queues
   * by VS LOW/MED/HIGH tasks.
   */
  ( void )apr_lock_enter( vs_voc_cmd_thread_lock );

  rc = apr_list_remove_head( &vs_voc_ssr_obj_tracking_free_q,
                             ( ( apr_list_node_t** ) &generic_item ) );
  VS_PANIC_ON_ERROR( rc );

  generic_item->handle = obj_handle;

  ( void ) apr_list_add_tail( &vs_voc_ssr_obj_tracking_used_q,
                              &generic_item->link );

  ( void )apr_lock_leave( vs_voc_cmd_thread_lock );

  return VS_EOK;
}

/* Untrack the VS objects with type VS_OBJECT_TYPE_ENUM_SIMPLE_JOB and
 * VS_OBJECT_TYPE_ENUM_HANDLE.
 */
VS_INTERNAL uint32_t vs_voc_ssr_untrack_object (
  uint32_t obj_handle
)
{
  int32_t rc = VS_EOK;
  vs_generic_item_t* generic_item = NULL;

  /* Thread lock is acquired to avoid concurrent access of track object queues
   * by VS LOW/MED/HIGH tasks.
   */
  ( void )apr_lock_enter( vs_voc_cmd_thread_lock );

  generic_item =
    ( ( vs_generic_item_t* ) &vs_voc_ssr_obj_tracking_used_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next( &vs_voc_ssr_obj_tracking_used_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    VS_PANIC_ON_ERROR( rc );

    if ( generic_item->handle == obj_handle )
    {
      ( void ) apr_list_delete( &vs_voc_ssr_obj_tracking_used_q,
                                &generic_item->link );
      ( void ) apr_list_add_tail( &vs_voc_ssr_obj_tracking_free_q,
                                  &generic_item->link );
      break;
    }
  }
  ( void )apr_lock_leave( vs_voc_cmd_thread_lock );

  return VS_EOK;
}


/* Free all of the tracked VS objects. */
VS_INTERNAL uint32_t vs_voc_ssr_free_all_tracked_objects (
  void
)
{
  int32_t apr_rc = APR_EOK;
  uint32_t vs_rc = VS_EOK;
  vs_generic_item_t* generic_item = NULL;
  vs_object_t* obj = NULL;

  for ( ;; )
  {
    apr_rc = apr_list_peak_head( &vs_voc_ssr_obj_tracking_used_q,
                                 ( ( apr_list_node_t** ) &generic_item ) );
    if ( apr_rc ) break;

    vs_rc = vs_get_object( generic_item->handle, &obj );
    VS_PANIC_ON_ERROR( vs_rc );

    /* Note that vs_mem_free_object will untrack the object. */
    ( void ) vs_mem_free_object ( obj );
  }

  return VS_EOK;
}

/**
 * Timer callback indicating the ADSP timeout, as VS has not recived the 
 * expected response with VS_VOC_CVD_RESPONSE_TIMEOUT_DURATION_NS.
 *
 * This will trigger VS Command Throtlling Mechanism, followed by cleanup and 
 * recovery if the awaited response is recieved.
 */
static void vs_voc_adsp_timeout_timer_cb(
 void* client_token
)
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
       "vs_voc_adsp_timeout_timer_cb(): triggered!! " );
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_adsp_timeout_timer_cb(): apr_timer start_counter = %d,"
          " stop_counter = %d ", timer_cntr, timer_stop_cntr );

  vs_is_cvd_cleanup_done = FALSE;
  vs_is_adsp_timeout_triggered = TRUE;

  return;
}

#if 0
/**
 * Note: This is just a test function, for testing the voice resource
 * unavailibility and availaibility during voice call.
 *
 * We can retain and reuse it later for simulating other on
 * target test scenarios.
 */
static void vs_voc_test_timer_cb(
 void* client_token
)
{
  uint32_t rc = APR_EOK;
  vs_cmd_packet_t* p_vs_cmd_pkt = NULL;
  vs_voc_cmdi_disable_t* p_args = NULL;
  vs_session_object_t* session_obj;
  vs_generic_item_t* generic_item;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "vs_voc_test_timer_cb(): triggered!! " );

#if 0
  generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &vs_used_session_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_test_timer_cb(): No VS session exist!! " );
      break;
    }

    rc = vs_get_typed_object( generic_item->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &session_obj ) );
    VS_PANIC_ON_ERROR( rc );

    /**
     * Allocate the memory for command packet and arguments.
     */
    p_args = ( vs_voc_cmdi_disable_t* ) apr_memmgr_malloc( &vs_heapmgr,
                                          sizeof( vs_voc_cmdi_disable_t ) );
    if ( p_args == NULL )
    {
      rc = VS_ENORESOURCE;
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }
    p_args->handle = generic_item->handle;

    p_vs_cmd_pkt = ( vs_cmd_packet_t* ) apr_memmgr_malloc( &vs_heapmgr,
                                                           sizeof( vs_cmd_packet_t ) );
    if( p_vs_cmd_pkt == NULL )
    {
      rc = VS_ENORESOURCE;
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    if ( session_obj->is_resource_granted == TRUE )
    {
      session_obj->is_resource_granted = FALSE;
      p_vs_cmd_pkt->cmd_id = VS_VOC_CMDI_DISABLE;
      p_vs_cmd_pkt->is_rsp_required = FALSE;
      p_vs_cmd_pkt->params = ( void* )p_args;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_test_timer_cb(): Voice resource revoked!! " );
    }
    else
    {
      session_obj->is_resource_granted = TRUE;
      p_vs_cmd_pkt->cmd_id = VS_VOC_CMDI_ENABLE;
      p_vs_cmd_pkt->is_rsp_required = FALSE;
      p_vs_cmd_pkt->params = ( void* )p_args;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_voc_test_timer_cb(): Voice resource granted!! " );
    }

    /* Queue the command packet for processing */
    rc = vs_queue_internal_cmd_packet( p_vs_cmd_pkt,
                                       &vs_low_priority_gating_packet_q.cmd_q,
                                       vs_signal_run_thrd3 );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_test_timer_cb() - cmd pkt queuing failed. rc=(0x%08x)", rc );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_voc_test_timer_cb() cmd pkt queued: VS session handle=(0x%08x)",
             p_args->handle );
    }
  }
#endif

  rc =  apr_timer_start( vs_voc_test_timer,
                         VS_VOC_TEST_TIMER_DURATION_NS );
  if( rc )
  {
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_voc_test_timer_cb(): apr_timer_start failed=(0X%08X) ", rc  );
  }


  return;
}
#endif

VS_INTERNAL void vs_obj_track_q_lock_fn (
  void
)
{
  ( void )apr_lock_enter( vs_voc_obj_track_q_lock );
}

VS_INTERNAL void vs_obj_track_q_unlock_fn (
  void
)
{
   ( void )apr_lock_leave( vs_voc_obj_track_q_lock );
}

VS_INTERNAL uint32_t vs_voc_ssr_init (
  void
)
{
  int32_t rc = VS_EOK;
  uint32_t index = 0;
#ifndef WINSIM
  RCECB_HANDLE ssr_handle;
#endif /* !WINSIM */

  vs_is_cvd_service_up = TRUE;
  vs_is_cvd_cleanup_done = TRUE;
  vs_is_adsp_timeout_triggered = FALSE;

#if 0 /* disabling test code */
  /**
   * Initialize the test timer.
   */
  rc = apr_timer_create( &vs_voc_test_timer, vs_voc_test_timer_cb, NULL );
  if ( rc )
  {
    MSG_ERROR ( "vs_voc_ssr_init():Failed to create  vs_voc_test_timer  with "
                "rc=(0x%08x)", rc, 0, 0 );
  }
#endif
  /**
   * Initialize the adsp/cvd timeout timer.
   */
  rc = apr_timer_create( &vs_voc_adsp_timeout_timer,
                         vs_voc_adsp_timeout_timer_cb, NULL );
  if ( rc )
  {
    MSG_ERROR ( "vs_voc_ssr_init():Failed to create  vs_voc_adsp_timeout_timer "
                "with rc=(0x%08x)", rc, 0, 0 );
  }

  { /* Initialize the object tracking list. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_voc_obj_track_q_lock );
    if( rc )
    {
      MSG_ERROR ( "vs_voc_ssr_init():Failed to create  vs_voc_obj_track_q_lock "
                  "with rc=(0x%08x)", rc, 0, 0 );
      return rc;
    }
    rc = apr_list_init_v2( &vs_voc_ssr_obj_tracking_free_q,
                           vs_obj_track_q_lock_fn, vs_obj_track_q_unlock_fn );
    for ( index = 0; index < VS_MAX_OBJECTS_V; ++index )
    {
      ( void ) apr_list_init_node(
                 ( apr_list_node_t* ) &vs_voc_ssr_obj_tracking_pool[ index ] );

      rc = apr_list_add_tail( &vs_voc_ssr_obj_tracking_free_q,
             ( ( apr_list_node_t* ) &vs_voc_ssr_obj_tracking_pool[ index ] ) );
    }

    rc = apr_list_init_v2( &vs_voc_ssr_obj_tracking_used_q,
                           vs_obj_track_q_lock_fn, vs_obj_track_q_unlock_fn );
  }

#ifndef WINSIM
  {  /* Register for system monitor SSR callbacks. */
    ssr_handle = rcecb_register_context_name( SYS_M_SSR_ADSP_BEFORE_SHUTDOWN,
                   vs_voc_ssr_adsp_before_shutdown_handler );
    if ( ssr_handle == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_ssr_init(): Failed to register for "
           "SYS_M_SSR_ADSP_BEFORE_SHUTDOWN callback." );
    }

    ssr_handle = rcecb_register_context_name( SYS_M_SSR_ADSP_AFTER_POWERUP,
                   vs_voc_ssr_adsp_after_powerup_handler );
    if ( ssr_handle == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_ssr_init(): Failed to register for "
           "SYS_M_SSR_ADSP_AFTER_POWERUP callback." );
    }
  }
#endif /* !WINSIM */

  return VS_EOK;
}


VS_INTERNAL uint32_t vs_voc_ssr_deinit (
  void
)
{
#ifndef WINSIM
  { /* Unregister for system monitor SSR callbacks. */
    ( void ) rcecb_unregister_context_name(
               SYS_M_SSR_ADSP_AFTER_POWERUP,
               vs_voc_ssr_adsp_after_powerup_handler );

    ( void ) rcecb_unregister_context_name(
               SYS_M_SSR_ADSP_BEFORE_SHUTDOWN,
               vs_voc_ssr_adsp_before_shutdown_handler );
  }
#endif /* !WINSIM */

  ( void ) apr_list_destroy( &vs_voc_ssr_obj_tracking_used_q );
  ( void ) apr_list_destroy( &vs_voc_ssr_obj_tracking_free_q );
  /* Release the locks. */
  ( void ) apr_lock_destroy( vs_voc_obj_track_q_lock );

  /**
   * Destroy the timers.
   */
  ( void ) apr_timer_destroy( vs_voc_test_timer );
  ( void ) apr_timer_destroy( vs_voc_adsp_timeout_timer );

  return VS_EOK;
}
