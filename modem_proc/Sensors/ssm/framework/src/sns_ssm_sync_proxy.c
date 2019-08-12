#ifndef SNS_SSM_SYNC_PROXY_C
#define SNS_SSM_SYNC_PROXY_C
/*=============================================================================
  @file sns_ssm_sync_proxy

  Sensors Service Manager's Synce Proxy Service

  This file contains the functions required by the sync proxy

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ssm/framework/src/sns_ssm_sync_proxy.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2015-08-13  gju  Remove smr_util
  2015-06-21  hw   Port Sync proxy feature to SLPI
  2015-02-24  gju  Use SNS_OS_IS_ISLAND_FUNC macro
  2015-01-28  hw   Adding support of synchronous mode for Sync-Proxy; uImage
                   memory optimization.
  2014-09-23  jtl  Initial version

=============================================================================*/


/*-----------------------------------------------------------------------------
 * Include Files
 * ---------------------------------------------------------------------------*/

#include "stdbool.h"
#include "sns_common.h"
#include "sns_common_v01.h"
#include "sns_debug_api.h"
#include "sns_debug_str.h"
#include "sns_ssm_priv.h"
#include "sns_sync_proxy_v01.h"
#include "sns_ssm_sync_proxy.h"
#include "sns_osa.h"

#include "sns_sam_fast_amd_v01.h"
#include "sns_em.h"
#include "sns_usmr.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_rh.h"


/*-----------------------------------------------------------------------------
 * Definitions And Macros
 * ---------------------------------------------------------------------------*/
#define QTIMER_TO_SSC_TICK(x) ( ((x) << 4LL)    /  9375LL )
#define SSC_TO_QTIMER_TICK(x) ( ((x) *  9375LL) >> 4LL    )
#define QTIMER_TO_USEC (1/19.2f) // Convert Qtimer ticks to uSecond

#define IND_LATENCY_QTIMER (19200LL *1LL) /* Approximate latency of sending an indication
                                           * to a client on another processor. Now 1ms */
#ifdef SNS_SSM_DEBUG
#define SNS_SSM_DEBUG_PRINTF0(level,msg)          SNS_SSM_PRINTF0(level,msg)
#define SNS_SSM_DEBUG_PRINTF1(level,msg,p1)       SNS_SSM_PRINTF1(level,msg,p1)
#define SNS_SSM_DEBUG_PRINTF2(level,msg,p1,p2)    SNS_SSM_PRINTF2(level,msg,p1,p2)
#define SNS_SSM_DEBUG_PRINTF3(level,msg,p1,p2,p3) SNS_SSM_PRINTF3(level,msg,p1,p2,p3)
#else
#define SNS_SSM_DEBUG_PRINTF0(level,msg)
#define SNS_SSM_DEBUG_PRINTF1(level,msg,p1)
#define SNS_SSM_DEBUG_PRINTF2(level,msg,p1,p2)
#define SNS_SSM_DEBUG_PRINTF3(level,msg,p1,p2,p3)
#endif

/*-----------------------------------------------------------------------------
 * Global variables
 * ---------------------------------------------------------------------------*/

typedef struct
{
  /* Connection handle for SMGR connection */
  smr_client_hndl smgr_hndl;

  /* true if synchronization is enabled. When it's true, sync
     proxy will be in sync mode, under which it will try to
     synchronize with modem's wake-up. if this is false, sync
     proxy will be in the normal mode, thus, send out the
     indication to modem whenever it gets one from famd */
  bool sync_enabled;

  /* Synchronization timer */
  sns_em_timer_obj_t sync_timer;

  /* Sending-indication timer */
  sns_em_timer_obj_t send_ind_timer;

  /* Current motion detect state */
  sns_smgr_sensor_event_status_e_v02 md_state;
  /* Per-client data: */
  struct
  {
    /**   This connection handle is assigned to the service.
          The handle is used to send responses & indications to the client
          using this sync service */
    sns_ssm_qmi_connection_s *service_conn_hndl;

    /**   Connection handle for the Fast-AMD connection */
    smr_client_hndl                    famd_hndl;

    /**   Stationary to Motion state change detection latency requirements,
          in QTimer ticks */
    uint32_t s_to_m_latency;

    /**   Motion to Stationary state change detection latency requirements,
          in QTimer ticks */
    uint32_t m_to_s_latency;

    /**   For now, only handle one wakeup schedule */
    sns_sync_proxy_sync_schedule_s_v01 schedule;

    /* Last reported state to the client */
    sns_sync_proxy_motion_state_e_v01  current_state;

    /* Previous state to the client */
    sns_sync_proxy_motion_state_e_v01  prev_state;

    /* Timestamp of last reported state to the client */
    uint32_t last_timestamp;

    uint64_t next_wakeup;

  } cli[1]; /* Only support 1 client for now */

} sync_proxy_state_s;

static sync_proxy_state_s sync_proxy_state SNS_SSM_UIMAGE_DATA;

/*-----------------------------------------------------------------------------
 * Functions
 * ---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_calc_next_wakeup
 *
 * Calculates the next wakeup period. Returns the relative QTimer value (in
 * Qtimer ticks) after which the modem will wake up.
 * This assumes that this is called shortly after a Fast-AMD determination,
 * and will calculate the latency based on that.
 * ---------------------------------------------------------------------------*/
SNS_SYNC_PROXY_UIMAGE_CODE static uint64_t sns_ssm_sync_proxy_calc_next_wakeup( void )
{
  uint64_t current_latency, now, now_qtimer;
  int64_t next_wakeup, numIntervals = -1;

  if( sync_proxy_state.cli[0].current_state == SNS_SYNC_PROXY_MOTION_MOVE_V01 )
  {
    current_latency = sync_proxy_state.cli[0].m_to_s_latency;
  }
  else
  {
    current_latency = sync_proxy_state.cli[0].s_to_m_latency;
  }

  (void)sns_em_get_timestamp64(&now);

  now_qtimer = SSC_TO_QTIMER_TICK(now);

  SNS_SSM_DEBUG_PRINTF2( HIGH, "SSM: Sync Proxy next_wakeup current time in qtimer %x, %x",
                         (now_qtimer >> 32), now_qtimer );

  SNS_SSM_DEBUG_PRINTF3( HIGH, "SSM: Sync Proxy now in SSC %d, absolute_qtimer %x, qtimer_interval %x",
                         now,
                         sync_proxy_state.cli[0].schedule.absolute_qtimer_value,
                         sync_proxy_state.cli[0].schedule.qtimer_interval );

  if( (now_qtimer + current_latency) < sync_proxy_state.cli[0].schedule.absolute_qtimer_value )
  {
    next_wakeup = sync_proxy_state.cli[0].schedule.absolute_qtimer_value - now_qtimer;
  }
  else
  {
    numIntervals = (now_qtimer + current_latency - sync_proxy_state.cli[0].schedule.absolute_qtimer_value)
      / sync_proxy_state.cli[0].schedule.qtimer_interval;
    next_wakeup =
      sync_proxy_state.cli[0].schedule.absolute_qtimer_value +
      sync_proxy_state.cli[0].schedule.qtimer_interval * numIntervals -
      now_qtimer;
  }

  SNS_SSM_DEBUG_PRINTF3( HIGH, "SSM: Sync Proxy current_latency %x, next_wakeup %d, numIntervals %d",
                         current_latency, next_wakeup, numIntervals );
  return next_wakeup;
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_sched_wakeup
 *
 * Schedules a timer for the next wakeup period.
 * Assumes that this this called at a Fast-AMD evaluation period. That is,
 * it doesn't check the current time vs. the last time Fast-AMD ran.
 * ---------------------------------------------------------------------------*/
SNS_SYNC_PROXY_UIMAGE_CODE static void sns_ssm_sync_proxy_sched_wakeup( void )
{
  uint64_t next_wakeup;

   /* Schedule next wakeup */
  next_wakeup = sns_ssm_sync_proxy_calc_next_wakeup();
  sns_em_register_timer( sync_proxy_state.sync_timer,
                          QTIMER_TO_SSC_TICK(next_wakeup));
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_sync_timer_cb
 *
 * Called at the modem sync time
 * ---------------------------------------------------------------------------*/
SNS_SSM_UIMAGE_CODE static void sns_ssm_sync_proxy_sync_timer_cb( void* unused )
{
  /* NOTICE: This is an uImage function. DO NOT access any data in bigImage */
  UNREFERENCED_PARAMETER( unused );
  /* call SMGR modem sync function */
  sns_rh_uimg_modem_sync_event();
}

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_send_ind
*
* Send Sync Proxy indication to client. This function should be called only
* when in BigImage mode
* ---------------------------------------------------------------------------*/
static void sns_ssm_sync_proxy_send_ind( void )
{
  qmi_csi_error qmi_err;
  sns_sync_proxy_motion_ind_msg_v01 ind_msg;

#ifdef SNS_SSM_DEBUG
  uint64_t now, now_qtimer;
  (void)sns_em_get_timestamp64(&now);
  now_qtimer = SSC_TO_QTIMER_TICK(now );
#endif

  ind_msg.timestamp = sync_proxy_state.cli[0].last_timestamp;
  ind_msg.state = sync_proxy_state.cli[0].current_state;
  qmi_err = qmi_csi_send_ind( sync_proxy_state.cli[0].service_conn_hndl->client_handle,
                              SNS_SYNC_PROXY_MOTION_IND_V01,
                              &ind_msg, sizeof( ind_msg ) );
  SNS_SSM_DEBUG_PRINTF1( HIGH, "send_ind callback latency %d qtimer ticks",
                         now_qtimer - sync_proxy_state.cli[0].next_wakeup );

  if( QMI_CSI_NO_ERR != qmi_err )
  {
    SNS_SSM_PRINTF1(ERROR, "SSM: sync proxy error sending ind %d", qmi_err);
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_sched_send_ind
 *
 * Schedules a timer for the next wakeup period so that it will send indication
 * when Modem is awake.
 * ---------------------------------------------------------------------------*/
SNS_SYNC_PROXY_UIMAGE_CODE static void sns_ssm_sync_proxy_sched_send_ind( void )
{
  uint64_t now, now_qtimer;
  int64_t next_wakeup, numIntervals;
  (void)sns_em_get_timestamp64(&now);
  now_qtimer = SSC_TO_QTIMER_TICK(now);

  if( now_qtimer < sync_proxy_state.cli[0].schedule.absolute_qtimer_value )
  {
    next_wakeup = sync_proxy_state.cli[0].schedule.absolute_qtimer_value - now_qtimer;
  }
  else
  {
    numIntervals = ( (now_qtimer - sync_proxy_state.cli[0].schedule.absolute_qtimer_value) /
                   sync_proxy_state.cli[0].schedule.qtimer_interval + 1 );
    next_wakeup = ( sync_proxy_state.cli[0].schedule.absolute_qtimer_value +
                  (sync_proxy_state.cli[0].schedule.qtimer_interval * numIntervals) -
                  now_qtimer );
  }

  if( next_wakeup < IND_LATENCY_QTIMER )
  {
    // It is right before the next wakeup. Just send the inditation now
    SNS_SSM_DEBUG_PRINTF1( HIGH, "Sending indication now. next modem wakeup in %d SSC ticks",
                     QTIMER_TO_SSC_TICK(next_wakeup) );
    if( SNS_OS_IS_ISLAND_FUNC(sns_ssm_sync_proxy_sched_send_ind) )
    {
      sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_BIG);
      sns_ssm_sync_proxy_send_ind();
      sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_NOCLIENT);
    }
    else
    {
      sns_ssm_sync_proxy_send_ind();
    }
  }
  else
  {
    next_wakeup -= IND_LATENCY_QTIMER;

    sns_em_register_timer( sync_proxy_state.send_ind_timer,
                            QTIMER_TO_SSC_TICK(next_wakeup));
    SNS_SSM_DEBUG_PRINTF2( HIGH, "SSM: Sync Proxy sched_send_ind now %x, next_wakeup %d",
                           now, next_wakeup );
    sync_proxy_state.cli[0].next_wakeup = next_wakeup + now_qtimer;
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_sync_send_ind_timer_cb
 *
 * Called at the modem sync time
 * ---------------------------------------------------------------------------*/
SNS_SSM_UIMAGE_CODE static void sns_ssm_sync_proxy_send_ind_timer_cb( void* unused )
{
  UNREFERENCED_PARAMETER( unused );
  uint8_t os_err;
  if( SNS_OS_IS_ISLAND_FUNC(sns_ssm_sync_proxy_send_ind) )
  {
    sns_ssm_sync_proxy_send_ind();
  }
  else
  {
    sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_BIG);
    sns_os_sigs_post( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_SEND_IND_SIG,
                      OS_FLAG_SET, &os_err );
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_check_motion_state
 *  
 * Check the current motion state. If different than previous one, schedule an
 * indication sending to the client. Also update the wakeup schedule if appropriate
 * ---------------------------------------------------------------------------*/
SNS_SYNC_PROXY_UIMAGE_CODE static void sns_ssm_sync_proxy_check_motion_state(void)
{
  if( sync_proxy_state.cli[0].prev_state
      != sync_proxy_state.cli[0].current_state )
  {
    sync_proxy_state.cli[0].prev_state = sync_proxy_state.cli[0].current_state;
    //Schedule a timer for next wake-up to send out indication, so that the indication
    //arrives on modem during their wake-up
    sns_ssm_sync_proxy_sched_send_ind();
  }
  if( sync_proxy_state.md_state != SNS_SMGR_SENSOR_EVENT_STATUS_ENABLED_V02
      && sync_proxy_state.sync_enabled )
  {
    sns_ssm_sync_proxy_sched_wakeup();
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_famd_ind_cb
 *
 * Callback function when sync proxy receives indication from Fast-AMD
 * ---------------------------------------------------------------------------*/
SNS_SYNC_PROXY_UIMAGE_CODE static void sns_ssm_sync_proxy_famd_ind_cb
(
 smr_client_hndl client_handle,
 unsigned int msg_id,
 void *ind_c_struct,
 unsigned int ind_struct_len,
 void *ind_cb_data
)
{
  sns_sam_fast_amd_report_ind_msg_v01 *famd_ind = ind_c_struct;
  uint8_t os_err;

#ifdef SNS_SSM_DEBUG
  uint64_t now, now_qtimer;
  sns_em_get_timestamp64(&now);
  now_qtimer = SSC_TO_QTIMER_TICK(now);
  SNS_SSM_PRINTF2( HIGH, "SSM: sync proxy Fast-AMD ind cb. msg id 0x%x, ind_state %d",
                   msg_id, famd_ind->state );
  SNS_SSM_PRINTF3( HIGH, "SSM: sync proxy Fast-AMD ind cb. now %d, %d, %x",
                   (now>>32), now, now_qtimer );
#endif

  switch( msg_id )
  {
    case SNS_SAM_FAST_AMD_REPORT_IND_V01:
      sync_proxy_state.cli[0].last_timestamp = famd_ind->timestamp;
      sync_proxy_state.cli[0].current_state = (sns_sync_proxy_motion_state_e_v01)famd_ind->state;
      if( SNS_OS_IS_ISLAND_FUNC(sns_ssm_sync_proxy_check_motion_state) )
      {
        sns_ssm_sync_proxy_check_motion_state();
      }
      else
      {
        sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_BIG);
        sns_os_sigs_post( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_CHECK_MOTION_STATE_SIG,
                          OS_FLAG_SET, &os_err );
      }
      break;
    default:
      SNS_SSM_PRINTF1(ERROR, "SSM: sync proxy FAMD ind cb. UNKNOWN msg id %d", msg_id);
      break;
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_check_md_state
 *  
 * Check Motion Detection interrupt state and update the wakeup scheduler accordingly
 * ---------------------------------------------------------------------------*/
SNS_SYNC_PROXY_UIMAGE_CODE static void sns_ssm_sync_proxy_check_md_state(void)
{
  switch( sync_proxy_state.md_state )
  {
    case SNS_SMGR_SENSOR_EVENT_STATUS_ENABLED_V02:
      /* Motion interrupt programmed. Cancel any timers */
      sns_em_cancel_timer( sync_proxy_state.sync_timer );
      break;
    case SNS_SMGR_SENSOR_EVENT_STATUS_DISABLED_V02:
    case SNS_SMGR_SENSOR_EVENT_STATUS_OCCURRED_V02:
      /* Motion interrupt occurred. If in sync mode, find the proper modem
       * sync period before the stationary->motion latency time */
      if( sync_proxy_state.sync_enabled )
      {
        sns_ssm_sync_proxy_sched_wakeup();
      }
      break;
    default:
      break;
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_smgr_ind_cb
 *
 * Callback function called when sync proxy receives indication from SMGR
 * ---------------------------------------------------------------------------*/
SNS_SSM_UIMAGE_CODE STATIC void sns_ssm_sync_proxy_smgr_ind_cb
(
 smr_client_hndl client_handle,
 unsigned int msg_id,
 void *ind_c_struct,
 unsigned int ind_struct_len,
 void *ind_cb_data
)
{
  sns_smgr_sensor_event_ind_msg_v02 *event_ind = ind_c_struct;
  sns_smgr_sensor_status_monitor_ind_msg_v02 *status_ind = ind_c_struct;
  uint8_t os_err;

  SNS_SSM_DEBUG_PRINTF1(LOW, "SSM: sync proxy SMGR ind cb. msg id 0x%x", msg_id);

  switch( msg_id )
  {
    case SNS_SMGR_SENSOR_EVENT_IND_V02:
      {
        if( event_ind->sensor_event == SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02 )
        {
          if( event_ind->event_status_valid )
          {
            SNS_SSM_DEBUG_PRINTF2( HIGH, "SSM: sync proxy rx motion detect. status %d, sync_enabled %d",
                                   event_ind->event_status, sync_proxy_state.sync_enabled );
            sync_proxy_state.md_state = event_ind->event_status;
            if( SNS_OS_IS_ISLAND_FUNC(sns_ssm_sync_proxy_check_md_state) )
            {
              sns_ssm_sync_proxy_check_md_state();
            }
            else
            {
              sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_BIG);
              sns_os_sigs_post( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_CHECK_MD_STATE_SIG,
                                OS_FLAG_SET, &os_err );
            }
          }
        }
      }
      break;
    case SNS_SMGR_SENSOR_STATUS_MONITOR_IND_V02:
      {
        if( status_ind->sensor_id == SNS_SMGR_ID_ACCEL_V01 &&
            status_ind->is_accurate_ts_client_present_valid == true )
        {
          if( status_ind->is_accurate_ts_client_present == true )
          {
            SNS_SSM_DEBUG_PRINTF0(HIGH, "SSM: sync proxy - accurate TS client present");
            sync_proxy_state.sync_enabled = false;
            sns_em_cancel_timer( sync_proxy_state.sync_timer );
          }
          else
          {
            SNS_SSM_DEBUG_PRINTF0(HIGH, "SSM: sync proxy - accurate TS client NOT present");
            sync_proxy_state.sync_enabled = true;
          }
        }
      }
      break;
    default:
      SNS_SSM_PRINTF1(ERROR, "SSM: sync proxy ind cb. UNKNOWN msg id %d", msg_id);
      break;
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_prox_resp_cb
 *  
 * Response callback function when Sync Proxy registering for FAMD or SMGR 
 * ---------------------------------------------------------------------------*/
static void sns_ssm_sync_prox_resp_cb
(
 smr_client_hndl client_handle,
 unsigned int msg_id,
 void *resp_c_struct,
 unsigned int resp_c_struct_len,
 void *resp_cb_data,
 smr_err transp_err
 )
{
  uint8_t os_err;
  sns_os_sigs_post( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_RESP_MSG_SIG,
                    OS_FLAG_SET, &os_err );
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_err_cb
 *
 * Error callback function when Sync Proxy registering with FAMD or SMGR services
 * ---------------------------------------------------------------------------*/
void sns_ssm_sync_proxy_err_cb
(
 smr_client_hndl client_handle,
 smr_err error,
 void *err_cb_data
 )
{
  SNS_SSM_PRINTF0(ERROR, "SSM: sync proxy err cb" );
  /* TODO: Implement client init error callback */
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_release_cb
 *
 * Release callback function when Sync Proxy release smr connections
 * ---------------------------------------------------------------------------*/
void sns_ssm_sync_proxy_release_cb( void* data )
{
  SNS_SSM_PRINTF0(HIGH, "SSM: sync proxy discon cb" );

  if( data != NULL )
  {
    *((smr_client_hndl*)data) = NULL;
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_con_smgr
 *
 * Register Sync Proxy with SMGR for below services:
 *   1. Motion Detection Interrupt event
 *   2. Sensor Status Monitor event
 * ---------------------------------------------------------------------------*/
static void sns_ssm_sync_proxy_con_smgr( void )
{
  uint8_t os_err;
  smr_err err;
  OS_FLAGS wakeup_flags = 0;
  OS_FLAGS pending_flags = 0;
  sns_smgr_sensor_event_req_msg_v02 smgr_event_req;
  sns_smgr_sensor_event_resp_msg_v02 smgr_event_resp;

  sns_smgr_sensor_status_monitor_req_msg_v02 smgr_monitor_req;
  sns_smgr_sensor_status_monitor_resp_msg_v02 smgr_monitor_resp;
  smr_txn_handle txn_handle;

  SNS_SSM_PRINTF0(HIGH, "SSM: Connecting to SMGR internal" );
  err = smr_client_init( SNS_SMGR_INTERNAL_SVC_get_service_object_v02(),
                         SMR_CLIENT_INSTANCE_ANY,
                         sns_ssm_sync_proxy_smgr_ind_cb, NULL /* void *CBDATA */,
                         0 /* timeout */, sns_ssm_sync_proxy_err_cb,
                         NULL /* void *CBDATA */,
                         &(sync_proxy_state.smgr_hndl), true );
  if( err != SMR_NO_ERR )
  {
    SNS_SSM_PRINTF1(ERROR, "SSM: smr_client_init error %d", err );
    return;
  }

  /* Enable SMGR events for motion interrupt */
  sync_proxy_state.md_state = SNS_SMGR_SENSOR_EVENT_STATUS_DISABLED_V02;
  smgr_event_req.sensor_event = SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02;
  smgr_event_req.registering = true;
  smgr_event_req.shortest_interval = 1;

  err = smr_client_send_req( sync_proxy_state.smgr_hndl,
                             SNS_SMGR_SENSOR_EVENT_REQ_V02,
                             &smgr_event_req, sizeof(smgr_event_req),
                             &smgr_event_resp, sizeof(smgr_event_resp),
                             sns_ssm_sync_prox_resp_cb,
                             NULL /* CBDATA */, &txn_handle );
  if( err != SMR_NO_ERR )
  {
    SNS_SSM_PRINTF1(ERROR, "SSM: smr_client_send_req error %d", err );
    smr_client_release( sync_proxy_state.smgr_hndl,
                        sns_ssm_sync_proxy_release_cb, &(sync_proxy_state.smgr_hndl) );
    return;
  }
  /* Wait for response */
  /* TODO: Fix sns_os_sigs_pend */
  /* sns_os_sigs_pend is broken and will wake up upon any signal. Work around that here */
  /* Also, the timeout in sns_os_sigs_pend is not honored, so this could lead to the thread
   * being blocked if SMGR doesn't respond */
  while( (wakeup_flags & SNS_SSM_SYNC_PROXY_RESP_MSG_SIG) == 0)
  {
    wakeup_flags = sns_os_sigs_pend( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_RESP_MSG_SIG,
                                     OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                     1000 /* timeout */, &os_err );
    pending_flags |= (wakeup_flags & ~SNS_SSM_SYNC_PROXY_RESP_MSG_SIG);
  }
  if( pending_flags != 0 )
  {
    SNS_SSM_PRINTF1(HIGH, "SSM: unexpected sigs set while awaiting SMGR resp 0x%X.", wakeup_flags );
    sns_os_sigs_post( sns_ssm.ssm_flag_grp, pending_flags, OS_FLAG_SET, &os_err );
  }

  /* Enable SMGR monitoring for accurate TS client */
  smgr_monitor_req.sensor_id = SNS_SMGR_ID_ACCEL_V01;
  smgr_event_req.registering = true;

  err = smr_client_send_req( sync_proxy_state.smgr_hndl,
                             SNS_SMGR_SENSOR_STATUS_MONITOR_REQ_V02,
                             &smgr_monitor_req, sizeof(smgr_monitor_req),
                             &smgr_monitor_resp, sizeof(smgr_monitor_resp),
                             sns_ssm_sync_prox_resp_cb,
                             NULL /* CBDATA */, &txn_handle );
  if( err != SMR_NO_ERR )
  {
    SNS_SSM_PRINTF1( ERROR, "SSM: smr_client_send_req error %d", err );
    smr_client_release( sync_proxy_state.smgr_hndl,
                        sns_ssm_sync_proxy_release_cb, &(sync_proxy_state.smgr_hndl) );
    return;
  }
  /* Wait for response */
  /* TODO: Fix sns_os_sigs_pend */
  /* sns_os_sigs_pend is broken and will wake up upon any signal. Work around that here */
  /* Also, the timeout in sns_os_sigs_pend is not honored, so this could lead to the thread
   * being blocked if SMGR doesn't respond */
  while( (wakeup_flags & SNS_SSM_SYNC_PROXY_RESP_MSG_SIG) == 0)
  {
    wakeup_flags = sns_os_sigs_pend( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_RESP_MSG_SIG,
                                     OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                     1000 /* timeout */, &os_err );
    pending_flags |= (wakeup_flags & ~SNS_SSM_SYNC_PROXY_RESP_MSG_SIG);
  }
  if( pending_flags != 0 )
  {
    SNS_SSM_PRINTF1( HIGH, "SSM: unexpected sigs set while awaiting SMGR resp 0x%X.", wakeup_flags );
    sns_os_sigs_post( sns_ssm.ssm_flag_grp, pending_flags, OS_FLAG_SET, &os_err );
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_con_famd
 *
 * Register Sync Proxy with Fast-AMD service
 * ---------------------------------------------------------------------------*/
static void sns_ssm_sync_proxy_con_famd( void )
{
  uint8_t os_err;
  smr_err err;
  OS_FLAGS wakeup_flags = 0;
  OS_FLAGS pending_flags = 0;
  sns_sam_fast_amd_enable_req_msg_v01 fast_amd_enable_req;
  sns_sam_fast_amd_enable_resp_msg_v01 fast_amd_enable_resp;
  smr_txn_handle txn_handle;

  SNS_SSM_PRINTF0(HIGH, "SSM: Connecting to Fast-AMD" );
  err = smr_client_init( SNS_SAM_FAST_AMD_SVC_get_service_object_v01(),
                         SMR_CLIENT_INSTANCE_ANY,
                         sns_ssm_sync_proxy_famd_ind_cb, NULL /* void *CBDATA */,
                         0 /* timeout */, sns_ssm_sync_proxy_err_cb,
                         NULL /* void *CBDATA */,
                         &(sync_proxy_state.cli[0].famd_hndl), true );
  if( err != SMR_NO_ERR )
  {
    SNS_SSM_PRINTF1(ERROR, "SSM: smr_client_init error %d", err );
    return;
  }

  /* Send enable request to Fast-AMD */
  fast_amd_enable_req.report_mode = SNS_SAM_SYNC_REPORT_V01;
  fast_amd_enable_req.detection_period_in_motion_state =
    sync_proxy_state.cli[0].m_to_s_latency * QTIMER_TO_USEC;
  fast_amd_enable_req.detection_period_in_stationary_state =
    sync_proxy_state.cli[0].s_to_m_latency * QTIMER_TO_USEC;
  fast_amd_enable_req.state_detection_threshold_valid = 0;

  err = smr_client_send_req( sync_proxy_state.cli[0].famd_hndl,
                             SNS_SAM_FAST_AMD_ENABLE_REQ_V01,
                             &fast_amd_enable_req, sizeof(fast_amd_enable_req),
                             &fast_amd_enable_resp, sizeof(fast_amd_enable_resp),
                             sns_ssm_sync_prox_resp_cb,
                             NULL /* CBDATA */, &txn_handle );
  if( err != SMR_NO_ERR )
  {
    SNS_SSM_PRINTF1(ERROR, "SSM: smr_client_send_req error %d", err );
    smr_client_release( sync_proxy_state.cli[0].famd_hndl,
                        sns_ssm_sync_proxy_release_cb, &(sync_proxy_state.cli[0].famd_hndl) );
    return;
  }
  /* Wait for response */
  while( (wakeup_flags & SNS_SSM_SYNC_PROXY_RESP_MSG_SIG) == 0)
  {
    wakeup_flags = sns_os_sigs_pend( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_RESP_MSG_SIG,
                                     OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                     1000 /* timeout */, &os_err );
    pending_flags |= (wakeup_flags & ~SNS_SSM_SYNC_PROXY_RESP_MSG_SIG);
  }
  if( pending_flags != 0 )
  {
    SNS_SSM_PRINTF1(HIGH, "SSM: unexpected sigs set while awaiting AMD resp 0x%X.", wakeup_flags );
    sns_os_sigs_post( sns_ssm.ssm_flag_grp, pending_flags, OS_FLAG_SET, &os_err );
  }
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_discon_clients
 *
 * Post a signal to Sync Proxy for disconnecting with its client.
 * ---------------------------------------------------------------------------*/
static void sns_ssm_sync_proxy_discon_clients( void )
{
  uint8_t os_err;

  if( sync_proxy_state.cli[0].service_conn_hndl == NULL )
  {
    SNS_SSM_PRINTF0(HIGH, "SSM: client already disconnected" );
    return;
  }

  sns_os_sigs_post( sns_ssm.ssm_flag_grp, SNS_SSM_SYNC_PROXY_DISCON_SIG,
                    OS_FLAG_SET, &os_err );
}

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_proc_discon
*
* Process the disconnect request. Thus release the smr connection with
* Fast-AMD and SMGR.
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_sync_proxy_proc_discon(void)
{
  smr_client_release( sync_proxy_state.cli[0].famd_hndl,
                      sns_ssm_sync_proxy_release_cb, &(sync_proxy_state.cli[0].famd_hndl) );
  smr_client_release( sync_proxy_state.smgr_hndl,
                      sns_ssm_sync_proxy_release_cb, &(sync_proxy_state.smgr_hndl) );
  sync_proxy_state.cli[0].service_conn_hndl = NULL;
  return SNS_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * sns_ssm_sync_proxy_proc_req
 *
 * Sync Proxy process requests from client.
 * ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_sync_proxy_proc_req(sns_ssm_qmi_connection_s *connection_handle,
                                           qmi_req_handle req_handle,
                                           unsigned int msg_id,
                                           void *req_c_struct )

{
  union {
    sns_common_cancel_resp_msg_v01 cancel;
    sns_common_version_resp_msg_v01 version;
    sns_sync_proxy_motion_enable_resp_msg_v01 enable;
  }resp_msg;
  unsigned int resp_msg_sz = 0;

  SNS_SSM_PRINTF1(LOW, "SSM: sync proxy proc req msg_id 0x%x", msg_id );

  switch( msg_id )
  {
    case SNS_SYNC_PROXY_CANCEL_REQ_V01:
      SNS_SSM_PRINTF0(LOW, "SSM: sync_proxy_cancel_req" );
      sns_ssm_sync_proxy_discon_clients();
      resp_msg.cancel.resp.sns_result_t = 0;
      resp_msg.cancel.resp.sns_err_t = SENSOR1_SUCCESS;
      resp_msg_sz = sizeof( sns_common_cancel_resp_msg_v01 );
      break;
    case SNS_SYNC_PROXY_VERSION_REQ_V01:
      resp_msg.version.resp.sns_result_t = 0;
      resp_msg.version.resp.sns_err_t = SENSOR1_SUCCESS;
      resp_msg.version.interface_version_number = SNS_SYNC_PROXY_SVC_V01_IDL_MINOR_VERS;
      resp_msg.version.max_message_id = SNS_SYNC_PROXY_SVC_V01_MAX_MESSAGE_ID;      
      resp_msg_sz = sizeof( sns_common_version_resp_msg_v01 );
      break;
    case SNS_SYNC_PROXY_MOTION_ENABLE_REQ_V01:
      {
        sns_sync_proxy_motion_enable_req_msg_v01 *enable_req = req_c_struct;
        if( sync_proxy_state.cli[0].service_conn_hndl == connection_handle ) //Question: connection_handle could be shared?
        {
          int schedules_processed = 0;
          if( enable_req->sync_schedule_len != 0 )
          {
            sync_proxy_state.cli[0].schedule = enable_req->sync_schedule[0];
            schedules_processed++;
          }
          sync_proxy_state.cli[0].s_to_m_latency =
            enable_req->stationary_to_motion_latency;
          sync_proxy_state.cli[0].m_to_s_latency =
            enable_req->motion_to_stationary_latency;

          /* Re-request enable from existing client */
          SNS_SSM_PRINTF0(HIGH, "SSM: Sync Proxy enable from existing client" );
          resp_msg.enable.Resp.sns_result_t = 0;
          resp_msg.enable.Resp.sns_err_t = SENSOR1_SUCCESS;
          resp_msg.enable.num_sync_schedules_processed_valid = true;
          resp_msg.enable.num_sync_schedules_processed = schedules_processed;
          resp_msg_sz = sizeof( sns_sync_proxy_motion_enable_resp_msg_v01 );
        }
        else if( sync_proxy_state.cli[0].service_conn_hndl == NULL )
        {
          int schedules_processed = 0;
          uint64_t now;
          sns_em_get_timestamp64(&now);
          if( enable_req->sync_schedule_len != 0 )
          {
            sync_proxy_state.cli[0].schedule = enable_req->sync_schedule[0];
            schedules_processed++;

            SNS_SSM_PRINTF3(HIGH, "SSM: Sync Proxy enable absolute_qtimer 0x%x, qtimer_interval 0x%x qtimer_now %d",
                            sync_proxy_state.cli[0].schedule.absolute_qtimer_value,
                            sync_proxy_state.cli[0].schedule.qtimer_interval,
                            now);
          }

          if( 0 == sync_proxy_state.cli[0].schedule.qtimer_interval )
          {
            /* TODO: for debugging purpose. Since current sync proxy app will enter both values of
             *       absolute_qtimer_value and qtimer_interval as 0. So assign them some meaningful
             *       value here for testing purpose. will remove this once sync proxy test app is ready.
             */
            sync_proxy_state.cli[0].schedule.absolute_qtimer_value = SSC_TO_QTIMER_TICK(now);
            sync_proxy_state.cli[0].schedule.qtimer_interval = 24576000; //1.28 seconds
          }
          SNS_SSM_PRINTF2(HIGH, "SSM: Sync Proxy enable s_to_m_ltcy %x, m_to_s_ltcy %x",
                enable_req->stationary_to_motion_latency,
                enable_req->motion_to_stationary_latency );

          sync_proxy_state.cli[0].s_to_m_latency =
            enable_req->stationary_to_motion_latency;
          sync_proxy_state.cli[0].m_to_s_latency =
            enable_req->motion_to_stationary_latency;

          sync_proxy_state.cli[0].service_conn_hndl = connection_handle;

          /* Set the initial motion state value to -1. This will get updated once Sync Proxy gets
           * indication from Fast-AMD */
          sync_proxy_state.cli[0].current_state = -1;
          sync_proxy_state.cli[0].prev_state = -1;
        
          sns_ssm_sync_proxy_con_famd();
          sns_ssm_sync_proxy_con_smgr();

          resp_msg.enable.Resp.sns_result_t = 0;
          resp_msg.enable.Resp.sns_err_t = SENSOR1_SUCCESS;
          resp_msg.enable.num_sync_schedules_processed_valid = true;
          resp_msg.enable.num_sync_schedules_processed = schedules_processed;
          resp_msg_sz = sizeof( sns_sync_proxy_motion_enable_resp_msg_v01 );
        }
        else
        {
          /* There is already a sync proxy conneciton */
          /* TODO: Support multiple connections */
          SNS_SSM_PRINTF0(HIGH, "SSM: Disallowing multiple Sync proxy connection requests" );
          return SNS_ERR_NOTALLOWED;
        }
      }
      break;
    default:
      SNS_SSM_PRINTF1(HIGH, "SSM: sync proxy proc req -- bad msg id %d", msg_id );
      return SNS_ERR_BAD_MSG_ID;
      break;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, resp_msg_sz);
  
  return SNS_SUCCESS;
}

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_proc_signals
*
* Check the signals received by SSM and process them accordingly
* ---------------------------------------------------------------------------*/
void sns_ssm_sync_proxy_proc_signals(OS_FLAGS sigFlag)
{
  if(SSM_BIT_TEST(sigFlag, SNS_SSM_SYNC_PROXY_DISCON_SIG))
  {
    sns_ssm_sync_proxy_proc_discon();
  }
  if(SSM_BIT_TEST(sigFlag, SNS_SSM_SYNC_PROXY_SEND_IND_SIG))
  {
    sns_ssm_sync_proxy_send_ind();
    sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_NOCLIENT);
  }
  if(SSM_BIT_TEST(sigFlag, SNS_SSM_SYNC_PROXY_CHECK_MOTION_STATE_SIG))
  {
    sns_ssm_sync_proxy_check_motion_state();
    sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_NOCLIENT);
  }
  if(SSM_BIT_TEST(sigFlag, SNS_SSM_SYNC_PROXY_CHECK_MD_STATE_SIG))
  {
    sns_ssm_sync_proxy_check_md_state();
    sns_pm_vote_img_mode(sns_ssm_pm_handle, SNS_IMG_MODE_NOCLIENT);
  }
}

/*-----------------------------------------------------------------------------
* sns_ssm_sync_proxy_init
*
* Initialization of Sync Proxy.
* ---------------------------------------------------------------------------*/
sns_err_code_e sns_ssm_sync_proxy_init(void)
{
  memset( (void*)&sync_proxy_state, 0, sizeof( sync_proxy_state ) );

  if( SNS_SUCCESS != sns_em_create_timer_obj( sns_ssm_sync_proxy_sync_timer_cb,
                                               NULL,
                                               SNS_EM_TIMER_TYPE_ONESHOT,
                                               &(sync_proxy_state.sync_timer) ) )
  {
    SNS_SSM_PRINTF0(ERROR, "SSM: sync proxy error creating timer");
    return SNS_ERR_FAILED;
  }
  if( SNS_SUCCESS != sns_em_create_timer_obj( sns_ssm_sync_proxy_send_ind_timer_cb,
                                               NULL,
                                               SNS_EM_TIMER_TYPE_ONESHOT,
                                               &(sync_proxy_state.send_ind_timer) ) )
  {
    SNS_SSM_PRINTF0(ERROR, "SSM: sync proxy error creating send ind timer");
    return SNS_ERR_FAILED;
  }
  return SNS_SUCCESS;
}

#endif /* #ifndef SNS_SSM_SYNC_PROXY_C */
