/*
   Copyright (C) 2014-2015 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mva/src/mva.c#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "err.h"
#include "msg.h"
#include "drv_api.h"
#include "mmstd.h"
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_memmgr.h"
#include "tdsl2external.h"
#ifndef WINSIM
#include "wcdma_ext_api.h"
#endif /* WIMSIM */

#include "mvs.h"
#include "mvs_module.h" /* For listen event and get timing offset APIs.
                           Should be removed once we decide to 
                           publish them. */
#include "mva_api.h"
#include "vs_task.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define MVA_HEAP_SIZE ( 1024 )

#define MVA_CMD_QUEUE_SIZE ( 30 )

#define MVA_HANDLE_TOTAL_BITS ( 16 )
#define MVA_HANDLE_INDEX_BITS ( 5 )
#define MVA_NUM_HANDLES ( 1 << MVA_HANDLE_INDEX_BITS )

/* Stmr tick count at which WCDMA reads DSM queue */
#define MVA_WCDMA_READS_UL_PACKET_AT_STMR_TICK_COUNT_V ( 30 )

/* TDSCDMA picks UL packet from DSM queue at 1000 micro sec from VFR. 
 */
#define MVA_TDSCDMA_READS_UL_PKT_FRM_DSM_AT_TIME_V ( 1000 )

/* Buffer time needed to put vocoder packet in DSM Queue.
 * Time in micro second. 
 */
#define MVA_BUF_TIME_FOR_PUTTING_UL_PACKET_IN_DSM_V ( 1000 )

#define MVA_DFLT_TIMER_OFFSET_V ( 0 )

/* The frame size in microseconds that each voice processing threads 
 * (vptx, vprx, encoder, decoder, decoder pp) operates on.
 */
#define MVA_VOICE_FRAME_SIZE_US_V ( 20000 ) 

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

#define MVA_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

typedef struct mva_wcdma_timing_param_t mva_wcdma_timing_param_t;
struct mva_wcdma_timing_param_t
{
  uint8_t cfn;
  uint8_t stmr_tick_count;
};

typedef struct mva_tdscdma_timing_param_t mva_tdscdma_timing_param_t;
struct mva_tdscdma_timing_param_t
{
  uint32_t tds_sys_time;
};

typedef union mva_timing_param_t mva_timing_param_t;
union mva_timing_param_t
{
  mva_wcdma_timing_param_t wcdma;
  mva_tdscdma_timing_param_t tdscdma;  
};

typedef enum mva_cmd_type_enum_t
{
  MVA_CMD_TYPE_ENUM_UNINITIALIZED,
  MVA_CMD_TYPE_ENUM_MVS_EVT,
  MVA_CMD_TYPE_ENUM_VFR_EVT,
  MVA_CMD_TYPE_ENUM_TIMER_EVT,
  MVA_CMD_TYPE_ENUM_INVALID,
}
  mva_cmd_type_enum_t;

typedef struct mva_cmd_header_t
{
   uint32_t handle;
      /**< The handle to the associated apr_objmgr_object_t instance. */
   mva_cmd_type_enum_t type;
      /**<
       * The object type defines the actual derived object.
       */
}
  mva_cmd_header_t;

typedef struct mva_mvs_evt_t
{
  mva_cmd_header_t header;
  mvs_client_type client;
  mvs_call_event_type event;
}
  mva_mvs_evt_t;

typedef struct mva_vfr_evt_t
{
  mva_cmd_header_t header;
  mvs_client_type client;
  mva_timing_param_t timing_param;
}
  mva_vfr_evt_t;

typedef struct mva_timer_evt_t
{
  mva_cmd_header_t header;
  mvs_client_type client;
}
  mva_timer_evt_t;

typedef union mva_cmd_t
{
  mva_cmd_header_t header;
  mva_mvs_evt_t mvs_evt;
  mva_vfr_evt_t vfr_evt;
  mva_timer_evt_t timer_evt;
}
  mva_cmd_t;

typedef struct mva_work_item_t
{
  apr_list_node_t link;
  mva_cmd_t* cmd;
}
  mva_work_item_t;

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static apr_lock_t mva_lock;
static apr_event_t mva_control_event;

static apr_memmgr_type mva_heapmgr;
static uint8_t mva_heap_pool[ MVA_HEAP_SIZE ];

static apr_objmgr_t mva_objmgr;
static apr_objmgr_object_t mva_objects[ MVA_NUM_HANDLES ];

static apr_list_t mva_used_cmd_q;
static apr_list_t mva_free_cmd_q;
static mva_work_item_t mva_cmds[ MVA_CMD_QUEUE_SIZE ];

static apr_thread_t mva_thread;
static apr_event_t mva_work_event;
static uint8_t mva_task_stack[ MVA_TASK_STACK_SIZE ];

static apr_timer_t mva_wcdma_timer;
static apr_timer_t mva_tdscdma_timer;

static mvs_timing_offset_type mva_wcdma_timing_offset;
static mvs_timing_offset_type mva_tdscdma_timing_offset;

/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/

static void mva_isr_lock_fn ( void )
{
  apr_lock_enter( mva_lock );
}

static void mva_isr_unlock_fn ( void )
{
  apr_lock_leave( mva_lock );
}

static void mva_signal_run ( void )
{
  apr_event_signal( mva_work_event );
}

static int32_t mva_mem_alloc_cmd (
  uint32_t size,
  mva_cmd_t** ret_cmd
)
{
  int32_t rc;
  mva_cmd_t* cmd;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  { 
    cmd = ( mva_cmd_t* ) apr_memmgr_malloc( &mva_heapmgr, size );
    if ( cmd == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_mem_alloc_cmd(): Out " \
             "of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    rc = apr_objmgr_alloc_object( &mva_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_mem_alloc_cmd(): Out " \
             "of objects, rc = (0x%08x)", rc );
      apr_memmgr_free( &mva_heapmgr, cmd );
      return APR_ENORESOURCE;
    }

    objmgr_obj->any.ptr = cmd;

    cmd->header.handle = objmgr_obj->handle;
    cmd->header.type = MVA_CMD_TYPE_ENUM_UNINITIALIZED;
  }

  //MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_mem_alloc_cmd(): " \
  //       "heap used size (%d), handle (0x%08x)",
  //       apr_memheap_used( ( apr_memheap_type* )&mva_heapmgr ), cmd->header.handle );

  *ret_cmd = cmd;

  return APR_EOK;
}

static int32_t mva_mem_free_cmd (
  mva_cmd_t* cmd
)
{
  uint32_t handle;

  if ( cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  handle = cmd->header.handle;
  ( void ) apr_objmgr_free_object( &mva_objmgr, cmd->header.handle );
  apr_memmgr_free( &mva_heapmgr, cmd );

  //MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_mem_free_cmd(): heap used size %d, handle 0x%08x",
  //       apr_memheap_used( ( apr_memheap_type* )&mvs_heapmgr ), handle );

  return APR_EOK;
}

/* Returns TRUE if successful and FALSE if unsuccessful. */
static bool_t mva_queue_incoming_cmd (
  mva_cmd_t* cmd
)
{
  int32_t rc;
  mva_work_item_t* work_item;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW, "mva_queue_incoming_cmd(): " \
        "handle = (0x%08x), type = (0x%08x)",
        cmd->header.handle, cmd->header.type );

  for ( ;; )
  {
    { /* Get a free command structure. */
      rc = apr_list_remove_head( &mva_free_cmd_q, ( ( apr_list_node_t** ) &work_item ) );
      if ( rc )
      { /* No free command structure is available. */
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_queue_incoming_cmd(): " \
             "Ran out of free incoming command queues" );
        ( void )mva_mem_free_cmd( cmd );
        break;
      }
    }
    { /* Queue the new command to the incoming command queue and signal do
       * work.
       */
      work_item->cmd = cmd;
      ( void ) apr_list_add_tail( &mva_used_cmd_q, &work_item->link );
      mva_signal_run( );
    }

    return TRUE;
  }

  return FALSE;
}

static int32_t mva_make_mvs_evt_cmd (
  mvs_client_type client,
  mvs_call_event_type event,
  mva_cmd_t** ret_cmd
)
{
  int32_t rc;
  mva_cmd_t* cmd;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mva_mem_alloc_cmd( sizeof( mva_cmd_t ), ( ( mva_cmd_t** ) &cmd ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  cmd->header.type = MVA_CMD_TYPE_ENUM_MVS_EVT;
  cmd->mvs_evt.client = client;
  cmd->mvs_evt.event = event;

  *ret_cmd = cmd;

  return APR_EOK;
}

static int32_t mva_make_vfr_evt_cmd (
  mvs_client_type client,
  mva_timing_param_t* timing_param,
  mva_cmd_t** ret_cmd
)
{
  int32_t rc;
  mva_cmd_t* cmd;

  if ( ( ret_cmd == NULL ) || ( timing_param == NULL ) )
  {
    return APR_EBADPARAM;
  }

  rc = mva_mem_alloc_cmd( sizeof( mva_cmd_t ), ( ( mva_cmd_t** ) &cmd ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  cmd->header.type = MVA_CMD_TYPE_ENUM_VFR_EVT;
  cmd->vfr_evt.client = client;
  mmstd_memcpy( ( void* )&cmd->vfr_evt.timing_param, sizeof( mva_timing_param_t ),
                ( void* )timing_param, sizeof( mva_timing_param_t ) );

  *ret_cmd = cmd;

  return APR_EOK;
}

static int32_t mva_make_timer_evt_cmd (
  mvs_client_type client,
  mva_cmd_t** ret_cmd
)
{
  int32_t rc;
  mva_cmd_t* cmd;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mva_mem_alloc_cmd( sizeof( mva_cmd_t ), ( ( mva_cmd_t** ) &cmd ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  cmd->header.type = MVA_CMD_TYPE_ENUM_TIMER_EVT;
  cmd->timer_evt.client = client;

  *ret_cmd = cmd;

  return APR_EOK;
}

static uint32_t mva_mvs_event_cb ( 
  mvs_client_type client,
  mvs_call_event_type event
)
{
  uint32_t rc = APR_EOK;
  mva_cmd_t* mva_cmd;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mva_mvs_event_cb(): " \
         "client = (%d), event = (%d)", client, event );

  switch ( client )
  {
  case MVS_CLIENT_WCDMA:
  case MVS_CLIENT_TDSCDMA:
    rc = mva_make_mvs_evt_cmd( client, event, &mva_cmd );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_mvs_event_cb(): " \
             "Failed to make MVS event command, client = (%d), rc = (0x%08x)",
             client, rc );
      return rc;
    }
    ( void )mva_queue_incoming_cmd( mva_cmd );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mva_mvs_event_cb(): " \
           "Unsupported client = (%d)", client );
    break;
  }

  return rc;
}

/*
 * Function registered with WCDMA, will be called at every 10ms that is aligned with 
 * DPDCH frame boundary.
 */
static void mva_wcdma_vfr_cb
(
  bool_t steady_state,
  uint8_t cfn,
  uint8_t stmr_tick_cnt,
  void* client_data
)
{
  uint32_t rc;
  mva_cmd_t* mva_cmd;
  mva_wcdma_timing_param_t timing_param;

  for ( ;; )
  {
    if ( steady_state == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_wcdma_vfr_cb(): " \
           "VFR state unstable" );
      rc = APR_EFAILED;
      break;
    }
  
    /* Return if cfn ( connection frame number ) is odd */
    /* TBD: Check the recent RTD issue, to see if this is actually valid. */
    if ( ( cfn & 1 ) )
    {
      break;
    }

    timing_param.cfn = cfn;
    timing_param.stmr_tick_count = stmr_tick_cnt;

    rc = mva_make_vfr_evt_cmd( MVS_CLIENT_WCDMA, ( mva_timing_param_t* )&timing_param, &mva_cmd );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_wcdma_vfr_cb(): " \
             "Failed to make VFR event command, rc = (0x%08x)", rc );
      break;    
    }
  
    ( void )mva_queue_incoming_cmd( mva_cmd );
    break;
  }
  
  return;
}

static void mva_tdscdma_vfr_cb
(
  tds_vfr_info_type vfr_info,
  void* client_data
)
{
  uint32_t rc;
  mva_cmd_t* mva_cmd;
  mva_tdscdma_timing_param_t timing_param;
   
  for ( ;; )
  {
    if ( vfr_info.vfr_state != TDS_VFR_STATE_READY )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mva_tdscdma_vfr_cb(): VFR state unstable" );
      rc = APR_EFAILED;
      break;
    }
  
    timing_param.tds_sys_time = vfr_info.tds_sys_time;

    rc = mva_make_vfr_evt_cmd( MVS_CLIENT_TDSCDMA, ( mva_timing_param_t* )&timing_param, &mva_cmd );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_tdscdma_vfr_cb(): " \
             "Failed to make VFR event command, rc = (0x%08x)", rc );
      break;    
    }
  
    ( void )mva_queue_incoming_cmd( mva_cmd );

    break;
  }
  
  return;
} 

static uint32_t mva_process_mvs_evt (
  mva_cmd_t* cmd
)
{
  int32_t rc;
  mvs_client_type client;
  mvs_call_event_type event;
#ifndef WINSIM
  wl1_modem_mvs_client_type wl1_vfr_registration;
  tdscdma_register_vfr_notification_t tds_register_param;
#endif /* WINSIM */

  if ( cmd == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_mvs_event(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  client = cmd->mvs_evt.client;
  event = cmd->mvs_evt.event;
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_mvs_event(): " \
         "client = (%d), event = (%d)", client, event );

  if ( ( ( client != MVS_CLIENT_WCDMA ) && ( client != MVS_CLIENT_TDSCDMA ) ) ||
       ( ( event != MVS_CALL_EVENT_START ) && ( event != MVS_CALL_EVENT_STOP ) ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,  "mva_process_mvs_event(): " \
           "Unsupported client/event" );
  }

#ifndef WINSIM
  switch ( client )
  {
  case MVS_CLIENT_WCDMA:
    if ( event == MVS_CALL_EVENT_START )
    { /* Register WCDMA VFR. Callback will be called by every 10ms 
         that is aligned with DPDCH frame boundary.
       */
      wl1_vfr_registration.wl1_modem_mvs_timing_cb_ptr = mva_wcdma_vfr_cb;
      wl1_vfr_registration.client_data_mvs = NULL;
      rc  = wcdma_ext_api( WCDMA_MVS_REGISTER_FOR_MODEM_TIMING, &wl1_vfr_registration,
                           sizeof( wl1_vfr_registration ) );
      if( rc != WCDMA_STATUS_GOOD )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_process_mvs_event(): " \
               "Failed to register WCDMA VFR, rc = (0x%08X)", rc );
        rc = APR_EFAILED;
      }
      else
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mva_process_mvs_event(): Register WCDMA VFR" );
        rc = APR_EOK;
      }
    }
    else /* MVS_CALL_EVENT_STOP */
    { 
      apr_timer_stop( mva_wcdma_timer );

      /* De-register WCDMA VFR. */
      wl1_vfr_registration.wl1_modem_mvs_timing_cb_ptr = NULL;
      wl1_vfr_registration.client_data_mvs = NULL;
      rc  = wcdma_ext_api( WCDMA_MVS_REGISTER_FOR_MODEM_TIMING, &wl1_vfr_registration,
                           sizeof( wl1_vfr_registration ) );
      if( rc != WCDMA_STATUS_GOOD )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_process_mvs_event(): " \
               "Failed to deregister WCDMA VFR, rc = (0x%08X)", rc );
        rc = APR_EFAILED;
      }
      else
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mva_process_mvs_event(): Deregister WCDMA VFR" );
        rc = APR_EOK;
      }
    }
    break;

  case MVS_CLIENT_TDSCDMA:
    if ( event == MVS_CALL_EVENT_START )
    { /* Register TDSCMDA VFR. */
      tds_register_param.service = mva_tdscdma_vfr_cb;
      tds_register_param.client_dt = NULL;
      rc = tdscdma_l2_api_call( TDSL2_CMD_VFR_NOTIFICATION_REGISTER, 
                                &tds_register_param, sizeof( tds_register_param ) );
      if( rc != TDS_EOK )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_process_mvs_event(): " \
               "Failed to register TDSCDMA VFR, rc = (0x%08X)", rc );
        rc = APR_EFAILED;
      }
      else
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mva_process_mvs_event(): Register TDSCDMA VFR" );
        rc = APR_EOK;
      }
    }
    else /* MVS_CALL_EVENT_STOP */
    { 
      apr_timer_stop( mva_tdscdma_timer );

      /* De-register TDSCDMA VFR. */
      tds_register_param.service = NULL;
      tds_register_param.client_dt = NULL;
      rc = tdscdma_l2_api_call( TDSL2_CMD_VFR_NOTIFICATION_REGISTER, 
                                &tds_register_param, sizeof( tds_register_param ) );
      if( rc != TDS_EOK )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_process_mvs_event(): " \
               "Failed to deregister TDSCDMA VFR, rc = (0x%08X)", rc );
        rc = APR_EFAILED;
      }
      else
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mva_process_mvs_event(): Deregister TDSCDMA VFR" );
        rc = APR_EOK;
      }
    }
    break;

  default:
    break;
  }
#endif /* !WINSIM */

  rc = mva_mem_free_cmd( cmd );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_mvs_event(): " \
         "Failed to free command" );
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_mvs_event(): " \
         "completed, rc = (0x%08x)", rc );

  return rc;
}

static uint32_t mva_deliver_wcdma_tx_frame (
 mva_wcdma_timing_param_t* timing_param
)
{
  uint32_t rc;
  int64_t tx_timing_offset_in_micro_sec = 0;  
  uint64_t tx_timing_offset;

  if ( timing_param == NULL ) 
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "mva_deliver_wcdma_tx_frame(): Invalid parameter" );
    return APR_EBADPARAM;
  }
  
  /* TBD, check recent RTD issue to see if this is true. */
  if ( ( timing_param->cfn & 1 ) )
  {
    return APR_EOK;
  }

  /* Calculate the timing offset in stmr timer tick count.*/
  tx_timing_offset_in_micro_sec = ( int64_t )( ( int64_t )( MVA_WCDMA_READS_UL_PACKET_AT_STMR_TICK_COUNT_V ) - 
                                               ( int64_t )( timing_param->stmr_tick_count ) );
 
 /* Converting ticks to microsec. 150 ticks equals 10 milli-sec, or 150 ticks equals 10000 micro-sec
  * or 1 tick equals 1000/15 or 200/3 microsec or 66.666 microsec .
  */
  tx_timing_offset_in_micro_sec = ( int64_t )( ( 1.0 * tx_timing_offset_in_micro_sec ) * 66.66 );

  tx_timing_offset_in_micro_sec = ( tx_timing_offset_in_micro_sec - 
                                    ( int64_t )( MVA_BUF_TIME_FOR_PUTTING_UL_PACKET_IN_DSM_V ) );

  if ( tx_timing_offset_in_micro_sec <= 0 )
  {
    if ( ( ( -1 ) * tx_timing_offset_in_micro_sec ) >= ( int64_t )( MVA_VOICE_FRAME_SIZE_US_V ) )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_deliver_wcdma_tx_frame(): " \
              "cfn = (0x%08X) is not valid ", timing_param->cfn );
       return APR_EFAILED;
    }
    tx_timing_offset_in_micro_sec = ( tx_timing_offset_in_micro_sec + 
                                      ( int64_t )( MVA_VOICE_FRAME_SIZE_US_V ) );
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mva_deliver_wcdma_tx_frame(): "
         "cfn = (%d), stmr tick count = (%d), timing_offset_in_micro_sec = (0x%016X)",
         timing_param->cfn,
         timing_param->stmr_tick_count,
         tx_timing_offset_in_micro_sec );

  /* Convert time in nano-sec, apr_timer expects time in nano-sec. */
  tx_timing_offset = ( tx_timing_offset_in_micro_sec * 1000 );

  rc = apr_timer_start( mva_wcdma_timer, tx_timing_offset );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mva_deliver_wcdma_tx_frame(): Failed to start timer, rc = (0x%08x)", rc );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mva_deliver_wcdma_tx_frame(): Start timer, to be fired in (%d)ns", tx_timing_offset );
  }
  
  return rc;
}

static uint32_t mva_deliver_tdscdma_tx_frame (
 mva_tdscdma_timing_param_t* timing_param
)
{
  uint32_t rc;
  int64_t tx_timing_offset_in_micro_sec = 0;
  uint32_t tds_sub_frame_count = 0;
  uint32_t tds_stmr_tick_count = 0;
  uint32_t tds_vfr_latency_in_micro_sec = 0;
  uint64_t tx_timing_offset;
 
  if ( timing_param == NULL ) 
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "mva_deliver_tdscdma_tx_frame(): Invalid parameter" );
    return APR_EBADPARAM;
  }

  /* bits [28:16] represents the sub frame count from TDS. */
  tds_sub_frame_count = ( ( timing_param->tds_sys_time & 0x1FFF0000 ) >> 16 );

  /* bits [15:0] represents the system tick for current TDS sub frame.
   * (512) ticks are equivalent to (50) usec.
   */
  tds_stmr_tick_count = ( timing_param->tds_sys_time & 0x0000FFFF );
  tds_vfr_latency_in_micro_sec = ( uint32_t )( ( 1.0 * tds_stmr_tick_count ) / 10.24 ); 

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mva_deliver_tdscdma_tx_frame(): tds_sub_frame_count = (%d),"
         "tds_stmr_tick_count = (%d), tds_vfr_latency_in_micro_sec = (%d)",
         tds_sub_frame_count, tds_stmr_tick_count, tds_vfr_latency_in_micro_sec );

  tx_timing_offset_in_micro_sec = ( int64_t )( ( int64_t )( MVA_TDSCDMA_READS_UL_PKT_FRM_DSM_AT_TIME_V ) - 
                                               ( int64_t )( tds_vfr_latency_in_micro_sec) );
  
  tx_timing_offset_in_micro_sec = ( int64_t )( ( int64_t )( tx_timing_offset_in_micro_sec ) - 
                                               ( int64_t )( MVA_BUF_TIME_FOR_PUTTING_UL_PACKET_IN_DSM_V ) );

  if ( tx_timing_offset_in_micro_sec <= 0 )
  {
    if ( ( ( -1 ) * tx_timing_offset_in_micro_sec ) >= ( int64_t )( MVA_VOICE_FRAME_SIZE_US_V ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mva_deliver_tdscdma_tx_frame(): tds_vfr_latency_in_micro_sec = (0x%08X) is invalid",
             tds_vfr_latency_in_micro_sec );
      return APR_EFAILED;
    }
    tx_timing_offset_in_micro_sec  = ( int64_t )( ( int64_t )( tx_timing_offset_in_micro_sec ) +
                                                  ( int64_t )( MVA_VOICE_FRAME_SIZE_US_V ) );
  }

  tx_timing_offset = ( tx_timing_offset_in_micro_sec * 1000 );

  rc = apr_timer_start( mva_tdscdma_timer, tx_timing_offset );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mva_deliver_tdscdma_tx_frame(): Failed to start timer, rc = (0x%08x)", rc );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mva_deliver_tdscdma_tx_frame(): Start timer, to be fired in (%d)ns", tx_timing_offset );
  }
  
  return rc;
}

static uint32_t mva_process_vfr_evt (
  mva_cmd_t* cmd
)
{
  int32_t rc;

  if ( cmd == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_vfr_evt(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_vfr_evt(): " \
         "client = (%d)", cmd->vfr_evt.client );

  switch ( cmd->vfr_evt.client )
  {
  case MVS_CLIENT_WCDMA:
    rc = mva_deliver_wcdma_tx_frame( ( mva_wcdma_timing_param_t* )&cmd->vfr_evt.timing_param );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_vfr_evt(): " \
             "Failed to deliver WCDMA Tx frame, rc = (0x%08x)", rc );
    }
    break;

  case MVS_CLIENT_TDSCDMA:
    rc = mva_deliver_tdscdma_tx_frame( ( mva_tdscdma_timing_param_t* )&cmd->vfr_evt.timing_param );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_vfr_evt(): " \
             "Failed to deliver TDSCDMA Tx frame, rc = (0x%08x)", rc );
    }
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_vfr_evt(): " \
           "Unsupported client = (%d)", cmd->vfr_evt.client );
    rc = APR_EUNSUPPORTED;
    break;
  }

  rc = mva_mem_free_cmd( cmd );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_vfr_evt(): " \
         "Failed to free command" );
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_vfr_evt(): " \
         "completed, rc = (0x%08x)", rc );

  return rc;
}

static void mva_timer_cb (
  void* client_token
)
{
  uint32_t rc;
  mva_cmd_t* mva_cmd;
  mvs_client_type client;

  client = ( mvs_client_type )client_token;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_timer_cb(): " \
         "Timer fired, client = (%d)", client );  

  rc = mva_make_timer_evt_cmd( client, &mva_cmd );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_timer_cb(): " \
           "Failed to make timer event command, rc = (0x%08x)", rc );
  }
  
  ( void )mva_queue_incoming_cmd( mva_cmd );

  return;
}

static uint32_t mva_process_timer_evt (
  mva_cmd_t* cmd
)
{
  int32_t rc;

  if ( cmd == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_timer_evt(): " \
           "Invalid parameter" );
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_timer_evt(): " );

  rc = mvs_send_silence_frame( );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mva_process_timer_evt(): " \
         "Failed to send silence frame" );
  }

  rc = mva_mem_free_cmd( cmd );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_timer_evt(): " \
         "Failed to free command");
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mva_process_timer_evt(): " \
         "completed, rc = (0x%08x)", rc );

  return rc;
}

static void mva_task_process_events_and_nongating_commands ( void )
{
  int32_t rc = APR_EOK;
  mva_work_item_t* work_item;
  mva_cmd_t* cmd;

  while ( apr_list_remove_head( &mva_used_cmd_q,
                                ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    cmd = ( ( mva_cmd_t* ) work_item->cmd );
    ( void ) apr_list_add_tail( &mva_free_cmd_q, &work_item->link );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW, "mva_task_process_events_and_nongating_commands(): " \
           "Processing the non-gating queue - " \
           "handle=(0x%08x), type=(0x%08x)",
           cmd->header.handle, cmd->header.type );

    switch ( cmd->header.type )
    {
    case MVA_CMD_TYPE_ENUM_MVS_EVT:
      rc = mva_process_mvs_evt( cmd );
      break;

    case MVA_CMD_TYPE_ENUM_VFR_EVT:
      rc = mva_process_vfr_evt( cmd );
      break;

    case MVA_CMD_TYPE_ENUM_TIMER_EVT:
      rc = mva_process_timer_evt( cmd );
      break;

    default:
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_task_process_events_and_nongating_commands(): "
             "Unsupported command - " \
             "handle=(0x%08x), type=(0x%08x)",
             cmd->header.handle, cmd->header.type );      
      break;
    }
  }
}

static int32_t mva_run ( void )
{
  mva_task_process_events_and_nongating_commands( );

  return APR_EOK;
}

static int32_t mva_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &mva_work_event );
  apr_event_signal( mva_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( mva_work_event );
    if ( rc ) break;

    mva_run( );
  }

  apr_event_destroy( mva_work_event );
  apr_event_signal( mva_control_event );

  return APR_EOK;
}

static void mva_create_work_queues (
  apr_list_t* used_q,
  apr_list_t* free_q,
  mva_work_item_t* store,
  uint32_t store_size
)
{
  uint32_t index;

  apr_list_init_v2( used_q, mva_isr_lock_fn, mva_isr_unlock_fn );
  apr_list_init_v2( free_q, mva_isr_lock_fn, mva_isr_unlock_fn );

  for ( index = 0; index < store_size; ++index )
  {
    ( void ) apr_list_init_node( ( apr_list_node_t* ) &store[ index ] );
    ( void ) apr_list_add_tail( free_q, &store[ index ].link );
  }
}

static int32_t mva_init ( void )
{
  { /* Initialize basic OS resources for staging the setup. */
    apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &mva_lock );
    apr_event_create( &mva_control_event );
  }
  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &mva_heapmgr, ( ( void* ) &mva_heap_pool ), 
                          sizeof( mva_heap_pool ), NULL, NULL );
  }
  { /* Initialize the handle table. */
    apr_objmgr_setup_params_t setup_args;

    setup_args.table = mva_objects;
    setup_args.total_bits = MVA_HANDLE_TOTAL_BITS;
    setup_args.index_bits = MVA_HANDLE_INDEX_BITS;
    setup_args.lock_fn = mva_isr_lock_fn;
    setup_args.unlock_fn = mva_isr_unlock_fn;

    apr_objmgr_construct( &mva_objmgr, &setup_args );
  }
  { /* Initialize work queues. */
    mva_create_work_queues( &mva_used_cmd_q, &mva_free_cmd_q,
                                  mva_cmds, MVA_CMD_QUEUE_SIZE );
  }
  { /* Create a worker thread for processing incoming commands. */
    apr_thread_create( &mva_thread, MVA_TASK_NAME,
                       MVA_TASK_PRIORITY, mva_task_stack, MVA_TASK_STACK_SIZE,
                       mva_worker_thread_fn, NULL );
    apr_event_wait( mva_control_event );
  }
  { /* Create timers. */
    apr_timer_create( &mva_wcdma_timer, mva_timer_cb, ( void* )MVS_CLIENT_WCDMA );
    apr_timer_create( &mva_tdscdma_timer, mva_timer_cb, ( void* )MVS_CLIENT_TDSCDMA );
  }

  return APR_EOK;
}

static int32_t mva_postinit ( void )
{
  uint32_t rc;

  { /* Register with MVS for call state event. */
    rc = mvs_listen_for_event( mva_mvs_event_cb );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_postinit(): " \
             "Failed to listen for MVS event, rc = (0x%08x)", rc );
    }
  }

  { /* Get WCDMA timing offset. */
    rc = mvs_get_timing_offset( MVS_CLIENT_WCDMA, &mva_wcdma_timing_offset );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_postinit(): " \
             "Failed to get WCDMA timing offset, rc = (0x%08x)", rc );
    }
  }

  { /* Get TDSCDMA timing offset. */
    rc = mvs_get_timing_offset( MVS_CLIENT_TDSCDMA, &mva_tdscdma_timing_offset );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_postinit(): " \
             "Failed to get TDSCDMA timing offset, rc = (0x%08x)", rc );
    }
  }

  return rc;
}

static int32_t mva_predeinit ( void )
{
  /* De-register with MVS for call state event. */
  ( void ) mvs_listen_for_event( NULL );

  return APR_EOK;
}

static int32_t mva_deinit ( void )
{
  /* Destroy timers. */
  apr_timer_stop( mva_wcdma_timer );
  apr_timer_destroy( mva_wcdma_timer );
  apr_timer_stop( mva_tdscdma_timer );
  apr_timer_destroy( mva_tdscdma_timer );

  /* Destroy the worker thread for processing incoming commands. */
  apr_event_signal_abortall( mva_work_event );
  apr_event_wait( mva_control_event );

  /* Deinitialize work queues. */
  apr_list_destroy( &mva_free_cmd_q );
  apr_list_destroy( &mva_used_cmd_q );

  /* Deinitialize the handle table. */
  apr_objmgr_destruct( &mva_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  apr_event_destroy( mva_control_event );
  apr_lock_destroy( mva_lock );

  return APR_EOK;
}

APR_EXTERNAL uint32_t mva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  switch ( cmd_id )
  {
  case DRV_CMDID_INIT:
    rc = mva_init( );
    break;

  case DRV_CMDID_POSTINIT:
    rc = mva_postinit( );
    break;

  case DRV_CMDID_PREDEINIT:
    rc = mva_predeinit( );
    break;

  case DRV_CMDID_DEINIT:
    rc = mva_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mva_call(): " \
           "Unsupported cmd ID (0x%08x)", cmd_id );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

