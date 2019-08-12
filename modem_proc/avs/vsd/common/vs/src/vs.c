/**
  @file vs.c
  @brief This file contains the definitions of the public interfaces of VS.
  This file also includes some common function defintions.

*/

/* 
   Copyright (C) 2013-2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 
   
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/src/vs.c#1 $
   $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  05/12/15  aim   Added locks while accessing queues.
  30/06/14  sre   Added support for enabing eAMR over VS.
  10/18/13  rm    Replaced apr_list_init() with apr_list_init_v2().
                  Added vs_voc_cmd_thread_lock to make vs_voc_cmd_open thread
                  safe.
  08/09/13  sud   Renamed macro PC_ENV_SET to WINSIM.
                  Added session object cleanup in vs_deinit.
  07/30/13  sud   Updated software to report MSG_FATAL when fatal error
                  conditions occurs in client context.
  07/10/13  sg    Fixed the KW errors.
  06/29/13  sg    Fixed the banned QPSI memcpy APIs.
  06/26/13  sud   Updated debug messages and rearranged some of the functions
                  from vs_voc.c
  06/16/13  sud   Fixed some of the function defintions.
  06/13/13  hl    Added support for handling ADSP SSR.
  06/12/13  sud   Updated VS to handle errors and status propagation to client.
                  Updated VS to handle memory leak and memory corruption
                  issues.
                  Updated VS to handle command priorities and proper sequencing.
                  Updated VS to add proper debug messages
  06/01/13  sud   Fixed compiler warnings.
  04/16/13  sud   VS refactored code and fixed bugs.
  01/12/13  sud   Initial revision

*/

/******************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
#include <stdlib.h>

#include "msg.h"
#ifdef WINSIM
#include "drv_api.h"
#endif
#include "mmstd.h"

#include "voicecfg_api.h"
#include "voicecfg_items.h"
#include "apr_timer.h"

#include "vs.h"
#include "vs_api.h"
#include "vs_i.h"
#include "vs_voc_private_if.h"
#include "vs_task.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

char_t vs_my_dns[] = "qcom.audio.vs";
char_t vs_cvs_dns[] = "qcom.audio.cvs";
char_t vs_mvm_dns[] = "qcom.audio.mvm";

/* VS address is set at initialization. */
uint16_t vs_my_addr;
/* MVM address is set at initialization. */
uint16_t vs_mvm_addr;
/* CVS address is set at initialization. */
uint16_t vs_cvs_addr;

/* flag that indicates the state of vs task. */
bool_t vs_is_initialized = FALSE;
bool_t vs_is_cvd_service_up = TRUE;

/**
 * This variable indicates the ADSP/CVD timeout has been
 * triggred.
 *
 * This would only be set to false after Mproc reboots,
 * ADSP SSR/AUDIO-PD DOWN noitification or ADSP recovers and
 * provide response to VS over which it is waiting.
 */
bool_t vs_is_adsp_timeout_triggered = FALSE;

/**
 * Should be made false during initialization.
 *
 * vs_is_cvd_cleanup_done is common for cleanup during
 * ADSP SSR and during ADSP Timeout.
 */
bool_t vs_is_cvd_cleanup_done =  TRUE;

/* Temporarily added for debugging command throttling issue */
uint32_t timer_cntr = 0;
uint32_t timer_stop_cntr = 0;

extern apr_timer_t vs_voc_adsp_timeout_timer;

/* Flag to indicate if eAMR vocoder can be enabled or not. */
extern bool_t vs_voicecfg_is_2g_eamr_enabled;
extern bool_t vs_voicecfg_is_3g_eamr_enabled;
extern bool_t vs_voicecfg_is_td_eamr_enabled;

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/
/* Lock Management */
apr_lock_t vs_int_lock;   /* Lock for cmd & APR pkt queues */
apr_lock_t vs_thread_lock;
apr_lock_t vs_voc_cmd_thread_lock;  /* VS_VOC_CMD_OPEN lock */
apr_lock_t vs_voc_data_buf_lock;  /**< Data buf access lock. */
vs_session_lock_pool_t vs_session_lock_pool[VS_MAX_NUM_CLIENTS];

/* Heap Management */
apr_memmgr_type vs_heapmgr;
uint8_t vs_heap_pool[ VS_HEAP_SIZE_V ];

/* Object Management */
apr_objmgr_object_t vs_object_table[ VS_MAX_OBJECTS_V ];
apr_objmgr_t vs_objmgr;

/* WORK Packet Queue management */
vs_work_item_t vs_work_pkt_pool[ VS_NUM_WORK_PKTS_V ];
apr_list_t vs_free_work_pkt_q;

/**
 * Common queue for all type of incoming packets APR/CMD.
 * Highest priority VS worker thread dequeues and delegates
 * to appropriate threads.
 */
apr_list_t vs_work_pkt_q;

/**
 * NEW command/even queue.
 */
apr_list_t vs_med_priority_nongating_packet_q;
  /**< This is non-gating Medium VS WORK packet queue. */

vs_gating_control_t vs_low_priority_gating_packet_q;
  /**< This is gating Low VS APR packet queue. */

apr_list_t vs_low_priority_nongating_packet_q;
  /**< This is non-gating Low VS WORK packet queue. */

/* Managing Thread creation during boot up. */
apr_event_t vs_thread_create_event;

/**
 * Highest Priority thread management
 */
apr_event_t vs_high_event;
apr_thread_t vs_high_handle;
vs_thread_state_enum_t vs_high_state = VS_THREAD_STATE_ENUM_INIT;
static uint8_t vs_high_task_stack[ VS_HIGH_TASK_STACK_SIZE ];

/**
 * Medium Priority thread management
 */
apr_event_t vs_med_event;
apr_thread_t vs_med_handle;
vs_thread_state_enum_t vs_med_state = VS_THREAD_STATE_ENUM_INIT;
static uint8_t vs_med_task_stack[ VS_MED_TASK_STACK_SIZE ];

/**
 * Lowest Priority thread management
 */
apr_event_t vs_low_event;
apr_thread_t vs_low_handle;
vs_thread_state_enum_t vs_low_state = VS_THREAD_STATE_ENUM_INIT;
static uint8_t vs_low_task_stack[ VS_LOW_TASK_STACK_SIZE ];

/* VS APR handle */
uint32_t vs_apr_handle;

/* pool of linked list node data structure for sessions */
vs_generic_item_t vs_session_list_pool[ VS_MAX_NUM_SESSIONS_V ];
apr_list_t vs_free_session_q;  /* session tracking - free nodes */
apr_list_t vs_used_session_q;  /* session tracking - used nodes */

/**
 * apr_packet_t to faciliate RX packet exchnage for Multimode
 * clients.
 *
 * NOTE: cleanup is required in future as this is a hack to
 * faciliate the new private packet interface defined for MVS
 * and VS inetraction.
 */
aprv2_packet_t mmode_rx_packet;

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/
VS_INTERNAL void vs_int_lock_fn (
  void
)
{
  ( void )apr_lock_enter( vs_int_lock );
}

VS_INTERNAL void vs_int_unlock_fn (
  void
)
{
   ( void )apr_lock_leave( vs_int_lock );
}

static void vs_thread_lock_fn (
  void
)
{
  ( void )apr_lock_enter( vs_thread_lock );
}

static void vs_thread_unlock_fn (
  void
)
{
  ( void )apr_lock_leave( vs_thread_lock );
}

VS_INTERNAL void vs_voc_data_buf_lock_fn (
  void
)
{
  ( void )apr_lock_enter( vs_voc_data_buf_lock );
}

VS_INTERNAL void vs_voc_data_buf_unlock_fn (
  void
)
{
   ( void )apr_lock_leave( vs_voc_data_buf_lock );
}

void vs_assign_session_destroy_lock (
  uint32_t session_handle
)
{
  uint32_t i;
  for ( i = 0; i < VS_MAX_NUM_CLIENTS; ++i )
  {
    if( vs_session_lock_pool[i].session_handle == 0xFFFFFFFF )
    {
      vs_session_lock_pool[i].session_handle = session_handle;
      return;
    }
  }
  MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "vs_assign_session_destroy_lock() - No free session destroy lock" );
}

void vs_clear_session_destroy_lock (
  uint32_t session_handle
)
{
  uint32_t i;

  for ( i = 0; i < VS_MAX_NUM_CLIENTS; ++i )
  {
    if( vs_session_lock_pool[i].session_handle == session_handle )
    {
      vs_session_lock_pool[i].session_handle = 0xFFFFFFFF;
      return;
    }
  }
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_get_session_destroy_lock() - Invalid handle=(0x%08x)",
         session_handle );
}

/**
 * Returns client session lock for a given session handle.
 * Returns NULL for invalid session handle.
 */
apr_lock_t* vs_get_session_destroy_lock (
  uint32_t session_handle
)
{
  uint32_t i;

  for ( i = 0; i < VS_MAX_NUM_CLIENTS; ++i )
  {
    if( vs_session_lock_pool[i].session_handle == session_handle )
      return &vs_session_lock_pool[i].lock;
  }
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_get_session_destroy_lock() - Invalid handle=(0x%08x)",
         session_handle );
  return NULL;
}

VS_INTERNAL uint32_t vs_eamr_mode_call ( void )
{
  voicecfg_cmd_get_bool_item_t item;
  uint32_t rc = VS_EOK;

  {
    /* Vaidate if eAMR enabled for GSM */
    item.id = VOICECFG_CFG_IS_2G_EAMR_ENABLED;
    item.ret_value = FALSE;

    rc =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );
    if ( ! rc  && item.ret_value == TRUE )
    {
      vs_voicecfg_is_2g_eamr_enabled = TRUE;
    }
    else
    {
      vs_voicecfg_is_2g_eamr_enabled = FALSE;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_eamr_mode_call(): eAMR configuration for GSM is : ",
           vs_voicecfg_is_2g_eamr_enabled );
  }

  {
    /* Vaidate if eAMR enabled for WCDMA */
    item.id = VOICECFG_CFG_IS_3G_EAMR_ENABLED;
    item.ret_value = FALSE;

    rc =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );
    if ( ! rc  && item.ret_value == TRUE )
    {
      vs_voicecfg_is_3g_eamr_enabled = TRUE;
    }
    else
    {
      vs_voicecfg_is_3g_eamr_enabled = FALSE;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_eamr_mode_call(): eAMR configuration for WCDMA is : ",
           vs_voicecfg_is_3g_eamr_enabled );
  }

  {
    /* Vaidate if eAMR enabled for TDSCDMA */
    item.id = VOICECFG_CFG_IS_TDS_EAMR_ENABLED;
    item.ret_value = FALSE;

    rc =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );
    if ( ! rc  && item.ret_value == TRUE )
    {
      vs_voicecfg_is_td_eamr_enabled = TRUE;
    }
    else
    {
      vs_voicecfg_is_td_eamr_enabled = FALSE;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_eamr_mode_call(): eAMR configuration for TDSCDMA is : ",
           vs_voicecfg_is_td_eamr_enabled );
  }

  return rc;
}

/* Checks for state transition every 10 usec until times out */
static uint32_t vs_check_thread_state_timeout (
  vs_thread_state_enum_t* state,
  vs_thread_state_enum_t final_state,
  int32_t timeout
)
{
  while ( *state != final_state && timeout-- )
  {
    ( void ) apr_misc_sleep( 10000 ); /* Sleep for 10us. */
  }
  if( timeout <= 0 )
  {
    return VS_EFAILED;
  }
  return VS_EOK;
}  /* end of vs_check_state () */

/****************************************************************************
 * END of COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

VS_INTERNAL void vs_signal_run (
  vs_thread_priority_enum_t priority
)
{
  switch ( priority )
  {
    case VS_THREAD_PRIORITY_ENUM_HIGH:
      apr_event_signal ( vs_high_event );
      break;

    case VS_THREAD_PRIORITY_ENUM_MED:
      apr_event_signal ( vs_med_event );
      break;

    case VS_THREAD_PRIORITY_ENUM_LOW:
      apr_event_signal ( vs_low_event );
      break;

    default:
      break;
  }

  return;
}


VS_INTERNAL void vs_signal_run_thrd1 (
  void
)
{
  ( void )apr_event_signal( vs_high_event );
}

VS_INTERNAL void vs_signal_run_thrd2 (
  void
)
{
  ( void )apr_event_signal( vs_med_event );
}

VS_INTERNAL void vs_signal_run_thrd3 (
  void
)
{
  ( void )apr_event_signal( vs_low_event );
}

/****************************************************************************
 * VS ( CMD and APR ) PACKET QUEUING FUNCTIONS                              *
 ****************************************************************************/

/**
 * Queues the aprv2_packet_t to high priority thread. In
 * case of failure to queue a apr packet, packet will be
 * automatically freed.
 */
static uint32_t vs_queue_incoming_apr_packet (
  vs_thread_priority_enum_t priority,
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
  vs_work_item_t* p_work_item = NULL;
  apr_list_t* incoming_cmd_q;

  switch ( priority )
  {
    case VS_THREAD_PRIORITY_ENUM_HIGH:
      incoming_cmd_q = &vs_work_pkt_q;
      break;

    default:
      return APR_EUNSUPPORTED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &vs_free_work_pkt_q,
                               ( ( apr_list_node_t** ) &p_work_item ) );
    if ( rc )
    {
      rc = VS_ENORESOURCE;
      /* No free APR packet structure is available. */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
             "VS ran out of APR packets, rc=0x%08x, vs_state=%d",
             rc, vs_is_initialized );
      break;
    }

    /* Report command acceptance when requested. */
    rc = __aprv2_cmd_accept_command( vs_apr_handle, p_packet );
    if ( rc )
    {
      /* Can't report so abort the command. */
      MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "VS cannot report command acceptance, rc=0x%08x", rc );

      ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );
      rc = VS_EFAILED;
      break;
    }

    p_work_item->packet = ( ( void*) p_packet );
    p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_APR;

    /* Add to incoming request work queue */
    rc = apr_list_add_tail( incoming_cmd_q, &p_work_item->link );
    if ( rc )
    {
      rc = VS_EFAILED;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_queue_incoming_apr_packet() - ERROR: opcode=0x%08x, "
             "rc=0x%08x", p_packet->opcode, rc );

      /* Add back to vs_free_apr_pkt_q */
      p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
      p_work_item->packet = NULL;
      ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      vs_signal_run( priority );
    }

    break;
  } /* for loop ends. */

  return rc;
}  /* vs_queue_incoming_apr_packet() ends. */

/**
 * Queues the vs_cmd_packet_t to high priority thread.
 * Caller shall handle the memory clean in case of failure to
 * queue a command packet.
 */
VS_INTERNAL uint32_t vs_queue_incoming_cmd_packet (
  vs_thread_priority_enum_t priority,
  vs_cmd_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
  vs_work_item_t* p_work_item = NULL;
  apr_list_t* incoming_cmd_q;

  switch ( priority )
  {
    case VS_THREAD_PRIORITY_ENUM_HIGH:
      incoming_cmd_q = &vs_work_pkt_q;
      break;

    case VS_THREAD_PRIORITY_ENUM_MED:
      incoming_cmd_q = &vs_med_priority_nongating_packet_q;
      break;

    case VS_THREAD_PRIORITY_ENUM_LOW:
      incoming_cmd_q = &vs_low_priority_nongating_packet_q;
      break;

    default:
      return APR_EUNSUPPORTED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &vs_free_work_pkt_q,
                               ( ( apr_list_node_t** ) &p_work_item ) );
    if ( rc )
    {
      rc = VS_ENORESOURCE;
      /* No free command structure is available. */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
             "VS ran out of command packets, rc=0x%08x, vs_state=%d", rc,
             vs_is_initialized );
      break;
    }

    p_work_item->packet = ( ( void* ) p_packet );
    p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_CMD;

    /* Add to incoming request work queue */
    rc = apr_list_add_tail( incoming_cmd_q, &p_work_item->link );

    if ( rc )
    {
      rc = VS_EFAILED;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_queue_incoming_vs_command_packet()- ERROR: cmd_id=0x%08x, "
             "rc=0x%08x", p_packet->cmd_id, rc );

      /* Add back to vs_free_cmd_q */
      p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
      p_work_item->packet = NULL;
      ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
       vs_signal_run( priority );
    }

    break;
  }  /* for loop ends. */

  return rc;
}  /* vs_queue_incoming_cmd_packet() ends. */

/**
 * Queues the internal cmd to low priority thread.
 * Caller shall handle the memory clean in case of failure to
 * queue a command packet.
 */
VS_INTERNAL uint32_t vs_queue_pending_cmd_packet (
  vs_thread_priority_enum_t priority,
  vs_cmd_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
  vs_work_item_t* p_work_item = NULL;
  apr_list_t* pending_cmd_q;

  /* get the command params */
  if( p_packet == NULL )
  {
    VS_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    return VS_EFAILED;
  }

  switch ( priority )
  {
    case VS_THREAD_PRIORITY_ENUM_LOW:
      pending_cmd_q = &vs_low_priority_gating_packet_q.cmd_q;
      break;

    default:
      return APR_EUNSUPPORTED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &vs_free_work_pkt_q,
                               ( ( apr_list_node_t** ) &p_work_item ) );
    if ( rc )
    {
      rc = VS_ENORESOURCE;
      /* No free command structure is available. */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
             "VS ran out of command packets, rc=0x%08x, vs_state=%d", rc,
             vs_is_initialized );
      break;
    }

    p_work_item->packet = ( ( void* ) p_packet );
    p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_CMD;

    /* Add to incoming request work queue */
    rc = apr_list_add_tail( pending_cmd_q, &p_work_item->link );
    if ( rc )
    {
      rc = VS_EFAILED;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_queue_pending_cmd_packet()- ERROR: cmd_id=0x%08x, "
             "rc=0x%08x", p_packet->cmd_id, rc );

      /* Add back to vs_free_cmd_q */
      p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
      p_work_item->packet = NULL;
      ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
       vs_signal_run( priority );
    }

    break;
  }  /* for loop ends. */

  return rc;
}  /* vs_queue_pending_cmd_packet() ends. */

/**
 * Delegate the work packet requests (APR/CMD) to appropriate thread
 * assinged for it.
 */
static void vs_delegate_work_pkt_requests (
  void
)
{
  uint32_t rc = VS_EOK;
  uint32_t request_id = 0;
  vs_work_item_t* p_work_item = NULL;
  aprv2_packet_t*  p_apr_packet = NULL;
  vs_cmd_packet_t* p_cmd_packet = NULL;

  while( apr_list_remove_head( &vs_work_pkt_q,
                               ( ( apr_list_node_t** ) &p_work_item ) ) == VS_EOK )
  {
    if ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_APR )
    {
      p_apr_packet = ( ( aprv2_packet_t* ) p_work_item->packet );
      request_id = p_apr_packet->opcode;
    }
    else if ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_CMD )
    {
      p_cmd_packet = ( ( vs_cmd_packet_t* ) p_work_item->packet );
      request_id = p_cmd_packet->cmd_id;
    }
    else
    {
      VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );
    }

    switch ( request_id )
    {
    default:
      {
        /* Add to medium priority nongating queue - 2nd highest priority */
        rc = apr_list_add_tail( &vs_med_priority_nongating_packet_q,
                                &p_work_item->link );
        if ( rc )
        {
          rc = VS_EFAILED;
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vs_delegate_work_pkt_requests() - ERROR: opcode/ID = 0x%08x, "
                 "rc=0x%08x", request_id, rc );

          /* Add back to vs_free_apr_pkt_q */
          p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
          p_work_item->packet = NULL;
          ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );
        }
        else
        {
          /**
           * Signal medium priority thread.
           */
          vs_signal_run_thrd2( );
        }
      }
      break;
    }  /* switch case ends  */

    /**
     * Memory cleanup required
     * - For Unsupported request.
     * - When failed to queue vs_cmd_packet_t
     * - When failed to queue aprv2_packet_t.
     */
    if ( rc != VS_EOK )
    {
      if ( p_apr_packet != NULL )
      {
        rc = __aprv2_cmd_free( vs_apr_handle, p_apr_packet );
        VS_REPORT_FATAL_ON_ERROR( rc );
        p_apr_packet = APR_NULL_V;
      }

      if ( p_cmd_packet != NULL )
      {
        if ( p_cmd_packet->params != NULL )
        {
          apr_memmgr_free( &vs_heapmgr, p_cmd_packet->params );
          p_cmd_packet->params = NULL;
        }

        apr_memmgr_free( &vs_heapmgr, p_cmd_packet );
        p_cmd_packet = NULL;
      }
    }
  }  /* end of while () */

  return;
}  /* vs_delegate_work_pkt_requests () ends */

VS_INTERNAL vs_call_state_enum_t vs_get_call_state (
  vs_session_object_t* p_session_obj
)
{
  vs_call_state_enum_t call_state;
  VS_ACQUIRE_LOCK( p_session_obj->client_state_lock );
  call_state = p_session_obj->call_state;
  VS_RELEASE_LOCK( p_session_obj->client_state_lock );
  return call_state;
}

/****************************************************************************
 * VS event callback functions
 ****************************************************************************/
VS_INTERNAL uint32_t vs_common_send_event (
  uint32_t handle,
  uint32_t event_id,
  void* p_rsp,
  uint32_t rsp_size
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj = NULL;
  vs_common_event_cmd_response_t* p_cmd_resp = NULL;
  vs_common_event_eamr_mode_t* p_eamr_rsp = NULL ;

  /* Get the session object with respect to the handle */
  rc = vs_get_typed_object( handle, VS_OBJECT_TYPE_ENUM_SESSION,
                           ( ( vs_object_t** ) &p_session_obj ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR ( rc );
    return VS_EFAILED;
  }

  switch ( event_id )
  {
  case VS_COMMON_EVENT_READY:
  case VS_COMMON_EVENT_NOT_READY:
  case VS_VOC_EVENT_WRITE_REQUEST:
  case VS_VOC_EVENT_WRITE_BUFFER_RETURNED:
  case VS_VOC_EVENT_READ_AVAILABLE:
    {
      p_session_obj->event_cb( event_id, p_rsp, rsp_size,
                               p_session_obj->session_context );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_common_send_event(): event_id=0x%08x handle=0x%08x",
             event_id, handle );
    }
    break;

  case VS_COMMOM_EVENT_EAMR_MODE_CHANGE:
    {
      p_eamr_rsp =( ( vs_common_event_eamr_mode_t* ) p_rsp );
      p_session_obj->event_cb( event_id, p_rsp, rsp_size,
                               p_session_obj->session_context );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_common_send_event(): event = VS_COMMOM_EVENT_EAMR_MODE_CHANGE "
             "eamr_mode=0x%08x session handle=0x%08x" , p_eamr_rsp->mode, handle );
    }
    break;

  case VS_COMMON_EVENT_CMD_RESPONSE:
    {
      p_cmd_resp = ( (vs_common_event_cmd_response_t*) p_rsp );

      switch ( p_cmd_resp->cmd_id )
      {
      case VS_VOC_CMD_ENABLE:
      case VS_VOC_CMD_DISABLE:
      case VS_VOC_CMD_STANDBY:
      case VS_VOC_CMD_FLUSH_BUFFERS:
      case VS_VOC_CMD_CLOSE:
        break;

      default:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "Unexpected cmd_id=0x%08x ", p_cmd_resp->cmd_id );
        break;
      }  /* end of switch ( cmd->cmd_id ) */

      p_session_obj->event_cb( event_id, p_rsp, rsp_size,
                               p_session_obj->session_context );

      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_common_send_event(): cmd_id=0x%08x status=0x%08x handle=0x%08x",
             p_cmd_resp->cmd_id, p_cmd_resp->status_id, handle );
    }
    break;

  default:
    break;
  }  /* end of switch ( event_id ) */

  return rc;
}  /* end of vs_common_send_event () */

/****************************************************************************
 * APR PACKET DISPATCHING ROUTINES                                          *
 ****************************************************************************/

static int32_t vs_isr_dispatch_fn (
  aprv2_packet_t* p_packet,
  void* dispatch_data
)
{
  vs_queue_incoming_apr_packet( VS_THREAD_PRIORITY_ENUM_HIGH, p_packet );
  return ( int32_t )VS_EOK;
}  /* end of vs_isr_dispatch_fn() */

/****************************************************************************
 * VS CMD PACKET PREPARE/DISPATCHER/FREE  ROUTINES                          *
 ****************************************************************************/


/**
 * This is a common routine facilitating to prepare and
 * dispatches a CMD PKT.
 *
 * NOTE: This should only be used for commands that do not
 *       require any cmd params. In future it can be made more
 *       to create CMD PKT for CMD internal/external with or without
 *       params. (Note: Shouldn't be used for internal cmds)
 */
VS_INTERNAL uint32_t vs_prepare_and_dispatch_cmd_packet (
  vs_thread_priority_enum_t priority,
  uint32_t cmd_id,
  void* p_params,
  uint32_t params_size
)
{
  uint32_t rc = VS_EOK;
  vs_cmd_packet_t* p_cmd_packet = NULL;

  for ( ;; )
  {
    p_cmd_packet = ( ( vs_cmd_packet_t* ) apr_memmgr_malloc( &vs_heapmgr,
                                            sizeof( p_cmd_packet ) ) );
    if ( p_cmd_packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_ENORESOURCE );
      rc = VS_ENORESOURCE;
      break;
    }

    p_cmd_packet->cmd_id = cmd_id;
    p_cmd_packet->params = NULL;

    if ( ( params_size > 0 ) && ( p_params != NULL ) )
    {
      p_cmd_packet->params = apr_memmgr_malloc(  &vs_heapmgr, params_size );

      if ( p_cmd_packet->params == NULL )
      {
        rc = VS_ENORESOURCE;
        VS_REPORT_FATAL_ON_ERROR( rc );
        ( void ) vs_free_cmd_packet( p_cmd_packet );
        break;
      }
      mmstd_memcpy( p_cmd_packet->params, params_size, p_params, params_size );
    }

    /* Queue the command packet for processing. */
    rc = vs_queue_incoming_cmd_packet( priority, p_cmd_packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_prepare_and_dispatch_cmd_packets() - cmd pkt queuing failed. "
             "rc=(0x%08x)", rc );
      ( void ) vs_free_cmd_packet( p_cmd_packet );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_prepare_and_dispatch_cmd_packets() cmd pkt queued with "
             "cmd_id/opcode=(0x%08x)",cmd_id );
    }

    break;
  }

  return rc;
}

VS_INTERNAL uint32_t vs_free_cmd_packet (
  vs_cmd_packet_t* p_cmd_packet
)
{
  uint32_t rc = VS_EOK;

  if ( p_cmd_packet != NULL )
  {
    if( p_cmd_packet->params != NULL )
    {
      /* Free the memory - p_cmd_packet->params. */
      apr_memmgr_free( &vs_heapmgr, p_cmd_packet->params );
      p_cmd_packet->params = NULL;
    }

    /* Free the memeory - p_cmd_packet. */
    apr_memmgr_free( &vs_heapmgr, p_cmd_packet );
    p_cmd_packet = NULL;
  }

  return rc;
}


/****************************************************************************
 * GATING COMMAND ROUTINES                                                 *
 ****************************************************************************/

static uint32_t vs_gating_control_init (
  vs_gating_control_t* p_ctrl
)
{
  uint32_t rc = VS_EOK;

  if ( p_ctrl == NULL )
  {
    return VS_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q, vs_thread_lock_fn, vs_thread_unlock_fn );
  if ( rc )
  {
    return VS_EFAILED;
  }

  p_ctrl->state = VS_GATING_CMD_STATE_ENUM_FETCH;
  p_ctrl->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return VS_EOK;
}  /* end of vs_gating_control_init () */


static uint32_t vs_gating_control_destroy (
  vs_gating_control_t* p_ctrl
)
{
  if ( p_ctrl == NULL )
  {
    return VS_EBADPARAM;
  }

  ( void ) apr_list_destroy( &p_ctrl->cmd_q );

  return VS_EOK;
}  /* end of vs_gating_control_destroy () */

static void vs_response_fn_trampoline (
  uint32_t fn_index,
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
  vs_object_t* p_object = NULL;
  uint32_t msg_type;

  if ( p_packet != APR_NULL_V )
  {
    msg_type = APR_GET_FIELD( APRV2_PKT_MSGTYPE, p_packet->header );

    if ( msg_type == APRV2_PKT_MSGTYPE_EVENT_V )
    {
      rc = vs_get_object( p_packet->dst_port, &p_object );
    }
    else
    {
      rc = vs_get_object( p_packet->token, &p_object );
    }

    if ( rc == VS_EOK )
    {
      switch ( p_object->header.type )
      {
      case VS_OBJECT_TYPE_ENUM_HANDLE:
        p_object->handle.fn_table[ fn_index ]( p_packet );
        return;

      case VS_OBJECT_TYPE_ENUM_SIMPLE_JOB:
        p_object->simple_job.fn_table[ fn_index ]( p_packet );
        return;

      default:
        break;
      }
    }

    rc =  __aprv2_cmd_free( vs_apr_handle, p_packet );
    VS_REPORT_FATAL_ON_ERROR( rc );
    p_packet = APR_NULL_V;
  }
  else
  {
     VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
  }
}  /* end of vs_response_fn_trampoline () */


/****************************************************************************
 * MEDIUM PRIORITY REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS.                   *
 ****************************************************************************/

static void vs_process_med_priority_nongating_requests (
  void
)
{
  uint32_t rc = VS_EOK;
  uint32_t request_id = 0;
  vs_work_item_t* p_work_item = NULL;
  aprv2_packet_t*  p_apr_packet = NULL;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  vs_session_object_t* p_session_obj = NULL;
  vss_istream_evt_eamr_mode_changed_t eamr_rsp;
  apr_lock_t* session_lock_p = NULL;
  vs_voc_client_type_t client_type;

  /* locals For Packet Exchnage. */
  uint32_t voc_packet_size;
  vs_voc_buffer_item_t* p_wbuffer_node = NULL;
  vs_voc_buffer_t* p_wbuffer = NULL;
  vs_voc_buffer_item_t* p_rbuffer_node = NULL;

  while( apr_list_remove_head( &vs_med_priority_nongating_packet_q,
                                ( ( apr_list_node_t** ) &p_work_item ) )== VS_EOK )
  {
    /**
     * Validate the p_work_item. */
    if ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_APR )
    {
      p_apr_packet = ( ( aprv2_packet_t* ) p_work_item->packet );
      request_id = p_apr_packet->opcode;
    }
    else if ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_CMD )
    {
      p_cmd_packet = ( ( vs_cmd_packet_t* ) p_work_item->packet );
      request_id = p_cmd_packet->cmd_id;
    }
    else
    {
      VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );
    }

    /* Add back to vs_free_work_pkt_q. */
    p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
    p_work_item->packet = NULL;
    ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "vs_process_med_priority_nongating_requests(): Processing "
           "request_id=(0X%08X)", request_id );

    switch ( request_id )
    {
    case VS_VOC_CMD_ENABLE:
      if( p_cmd_packet != NULL )
        rc = vs_voc_cmd_enable_proc( p_cmd_packet );
      break;

    case VS_VOC_CMD_STANDBY:
      if( p_cmd_packet != NULL )
        rc = vs_voc_cmd_standby_proc( p_cmd_packet );
      break;

    case VS_VOC_CMD_DISABLE:
      if( p_cmd_packet != NULL )
        rc = vs_voc_cmd_disable_proc( p_cmd_packet );
      break;

    case VS_VOC_CMD_FLUSH_BUFFERS:
      if( p_cmd_packet != NULL )
        rc = vs_voc_cmd_flush_buffers_proc ( p_cmd_packet );
      break;

    case VS_VOC_CMD_CLOSE:
     if( p_cmd_packet != NULL )
        rc = vs_voc_cmd_close_proc( p_cmd_packet );
     break;

    case VS_VOC_CMDI_DESTROY_SESSION:
     if( p_cmd_packet != NULL )
        rc = vs_voc_cmdi_destroy_session_proc( p_cmd_packet );
     break;

    case APRV2_IBASIC_EVT_ACCEPTED:
      {
        vs_response_fn_trampoline( VS_RESPONSE_FN_ENUM_ACCEPTED, p_apr_packet );
        p_apr_packet = NULL;
      }
      break;

    case APRV2_IBASIC_RSP_RESULT:
      {
        vs_response_fn_trampoline( VS_RESPONSE_FN_ENUM_RESULT, p_apr_packet );
        p_apr_packet = NULL;
      }
      break;

    case VSS_ISTREAM_EVT_READY:
      {
        if ( p_apr_packet != NULL )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "VSS_ISTREAM_EVT_READY received. src_port=0x%08x, "
                 "dst_port=0x%08x, token=0x%08x", p_apr_packet->src_port,
                 p_apr_packet->dst_port, p_apr_packet->token );

          /* Get the session object with respect to the handle */
          rc = vs_get_typed_object( p_apr_packet->dst_port,
                                    VS_OBJECT_TYPE_ENUM_SESSION,
                                    ( ( vs_object_t** ) &p_session_obj ) );
          if ( rc )
          {
            VS_REPORT_FATAL_ON_ERROR( rc );
            break;
          }

          vs_common_send_event( p_session_obj->header.handle,
                                VS_COMMON_EVENT_READY, NULL, 0 );
          p_session_obj->stream_ready = TRUE;

          /* Lock session ctrl obj. */
          VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
          if ( p_session_obj->session_ctrl.goal == VS_VOC_GOAL_ENUM_NONE )
          {
            p_session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RUN;
          }
          /* Release session ctrl lock */
          VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

          /**
           * NOTE: Set the cached vocoder properties after recieving the
           * EVT_READY for the p_session_obj, this would make sure any chnages in
           * vocoder stream properties during the call setup are successfully
           * applied.
           */
          ( void ) vs_voc_action_set_cached_stream_properties ( p_session_obj );
        }
      }
      break;

    case VSS_ISTREAM_EVT_NOT_READY:
      {
        if ( p_apr_packet != NULL )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "VSS_ISTREAM_EVT_NOT_READY received. src_port=0x%08x, "
                 "dst_port=0x%08x, token=0x%08x", p_apr_packet->src_port,
                 p_apr_packet->dst_port, p_apr_packet->token );

          /* Get the session object with respect to the handle */
          rc = vs_get_typed_object( p_apr_packet->dst_port,
                                    VS_OBJECT_TYPE_ENUM_SESSION,
                                   ( ( vs_object_t** ) &p_session_obj ) );
          if ( rc )
          {
            VS_REPORT_FATAL_ON_ERROR( rc );
            break;
          }

          if ( p_session_obj->stream_ready == TRUE )
          {
            vs_common_send_event( p_session_obj->header.handle,
                                  VS_COMMON_EVENT_NOT_READY, NULL, 0 ) ;
            p_session_obj->stream_ready = FALSE;
          }

          VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
          if ( p_session_obj->session_ctrl.goal == VS_VOC_GOAL_ENUM_NONE )
          {
            p_session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_IDLE;
          }
          VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );
        }
      }
      break;

    case VSS_ISTREAM_EVT_EAMR_MODE_CHANGED:
      {
        if ( p_apr_packet != NULL )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                 "VSS_ISTREAM_EVT_EAMR_MODE_CHANGED received. src_port=0x%08x, "
                 "dst_port=0x%08x, token=0x%08x", p_apr_packet->src_port,
                 p_apr_packet->dst_port, p_apr_packet->token );

          /* Get the session object with respect to the handle */
          rc = vs_get_typed_object( p_apr_packet->dst_port,
                                    VS_OBJECT_TYPE_ENUM_SESSION,
                                   ( ( vs_object_t** ) &p_session_obj ) );
          if ( rc )
          {
             VS_REPORT_FATAL_ON_ERROR( rc );
             break;
          }
          eamr_rsp.mode =
            APRV2_PKT_GET_PAYLOAD( vss_istream_evt_eamr_mode_changed_t,
                                   p_apr_packet )->mode;

          if ( ( eamr_rsp.mode == VSS_ISTREAM_EAMR_MODE_NARROWBAND ) ||
               ( eamr_rsp.mode == VSS_ISTREAM_EAMR_MODE_WIDEBAND ) )
          {
            vs_common_send_event( p_session_obj->header.handle,
                                  VS_COMMOM_EVENT_EAMR_MODE_CHANGE,
                                  &eamr_rsp,
                                  sizeof( vs_common_event_eamr_mode_t ) );
          }
          else
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "VSS_ISTREAM_EVT_EAMR_MODE_CHANGED received with wrong mode "
                   "mode=0x%08x", eamr_rsp.mode );
          }
        }
      }
      break;

    case VS_VOCAMR_CMD_SET_CODEC_MODE:
      if( p_cmd_packet != NULL )
      {
       rc = vs_vocamr_cmd_set_codec_mode_proc(
              ( ( vs_vocamr_cmd_set_codec_mode_t* )p_cmd_packet->params )->handle,
              ( ( vs_vocamr_cmd_set_codec_mode_t* )p_cmd_packet->params )->codec_mode );
      }
     break;

    case VS_VOCAMRWB_CMD_SET_CODEC_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocamrwb_cmd_set_codec_mode_proc(
              ( ( vs_vocamrwb_cmd_set_codec_mode_t* )p_cmd_packet->params )->handle,
              ( ( vs_vocamrwb_cmd_set_codec_mode_t* )p_cmd_packet ->params )->codec_mode );
      }
      break;

    case VS_VOCAMR_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocamr_cmd_set_dtx_mode_proc(
               ( ( vs_vocamr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocamr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->enable_flag );
      }
      break;

    case VS_VOCAMRWB_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocamrwb_cmd_set_dtx_mode_proc(
              ( ( vs_vocamrwb_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
              ( ( vs_vocamrwb_cmd_set_dtx_mode_t* )p_cmd_packet->params )->enable_flag );
      }
      break;

    case VS_VOCEFR_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocefr_cmd_set_dtx_mode_proc(
               ( ( vs_vocefr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocefr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->enable_flag );
      }
      break;

    case VS_VOCFR_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocfr_cmd_set_dtx_mode_proc(
               ( ( vs_vocfr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocfr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->enable_flag );
      }
      break;

    case VS_VOCHR_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vochr_cmd_set_dtx_mode_proc(
               ( ( vs_vochr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vochr_cmd_set_dtx_mode_t* )p_cmd_packet->params )->enable_flag );
      }
      break;

    /* EVRC-A */
    case VS_VOCEVRCA_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrca_cmd_set_dtx_mode_proc(
               ( ( vs_vocevrca_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrca_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VS_VOCEVRCA_CMD_SET_FRAME_MINMAX_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrca_cmd_set_frame_minmax_rate_proc(
               ( ( vs_vocevrca_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrca_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->min_rate,
               ( ( vs_vocevrca_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->max_rate );
      }
      break;

    case VS_VOCEVRCA_CMD_SET_ENC_RATE_MODULATION:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrca_cmd_set_enc_rate_mod_proc(
               ( ( vs_vocevrca_cmd_set_enc_rate_mod_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrca_cmd_set_enc_rate_mod_t* )p_cmd_packet->params  )->rate_modulation );
      }
      break;

    /* EVRC-B */
    case VS_VOCEVRCB_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcb_cmd_set_dtx_mode_proc(
               ( ( vs_vocevrcb_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcb_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VS_VOCEVRCB_CMD_SET_FRAME_MINMAX_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcb_cmd_set_frame_minmax_rate_proc(
               ( ( vs_vocevrcb_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcb_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->min_rate,
               ( ( vs_vocevrcb_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->max_rate );
      }
      break;

    case VS_VOCEVRCB_CMD_SET_AVG_ENC_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcb_cmd_set_avg_enc_rate_proc(
               ( ( vs_vocevrcb_cmd_set_avg_enc_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcb_cmd_set_avg_enc_rate_t* )p_cmd_packet->params  )->rate );
      }
      break;

    /* EVRC-WB */
    case VS_VOCEVRCWB_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcwb_cmd_set_dtx_mode_proc(
               ( ( vs_vocevrcwb_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcwb_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VS_VOCEVRCWB_CMD_SET_FRAME_MINMAX_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcwb_cmd_set_frame_minmax_rate_proc(
               ( ( vs_vocevrcwb_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcwb_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->min_rate,
               ( ( vs_vocevrcwb_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->max_rate );
      }
      break;

    case VS_VOCEVRCWB_CMD_SET_AVG_ENC_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcwb_cmd_set_avg_enc_rate_proc(
               ( ( vs_vocevrcwb_cmd_set_avg_enc_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcwb_cmd_set_avg_enc_rate_t* )p_cmd_packet->params  )->rate );
      }
      break;

    /* EVRC-NW */
    case VS_VOCEVRCNW_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcnw_cmd_set_dtx_mode_proc(
               ( ( vs_vocevrcnw_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcnw_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VS_VOCEVRCNW_CMD_SET_FRAME_MINMAX_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcnw_cmd_set_frame_minmax_rate_proc(
               ( ( vs_vocevrcnw_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcnw_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->min_rate,
               ( ( vs_vocevrcnw_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->max_rate );
      }
      break;

    case VS_VOCEVRCNW_CMD_SET_AVG_ENC_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcnw_cmd_set_avg_enc_rate_proc(
               ( ( vs_vocevrcnw_cmd_set_avg_enc_rate_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocevrcnw_cmd_set_avg_enc_rate_t* )p_cmd_packet->params  )->rate );
      }
      break;

    /* EVRC-NW2K */
    case VS_VOCEVRCNW2K_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcnw2k_cmd_set_dtx_mode_proc(
             ( ( vs_vocevrcnw2k_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocevrcnw2k_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VS_VOCEVRCNW2K_CMD_SET_FRAME_MINMAX_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcnw2k_cmd_set_frame_minmax_rate_proc(
             ( ( vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->min_rate,
             ( ( vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->max_rate );
      }
      break;

    case VS_VOCEVRCNW2K_CMD_SET_AVG_ENC_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocevrcnw2k_cmd_set_avg_enc_rate_proc(
             ( ( vs_vocevrcnw2k_cmd_set_avg_enc_rate_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocevrcnw2k_cmd_set_avg_enc_rate_t* )p_cmd_packet->params  )->rate );
      }
      break;

    /* QCELP-13K */
    case VS_VOCQCELP13K_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocqcelp13k_cmd_set_dtx_mode_proc(
             ( ( vs_vocqcelp13k_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocqcelp13k_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VS_VOCQCELP13K_CMD_SET_FRAME_MINMAX_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocqcelp13k_cmd_set_frame_minmax_rate_proc(
             ( ( vs_vocqcelp13k_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocqcelp13k_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->min_rate,
             ( ( vs_vocqcelp13k_cmd_set_frame_minmax_rate_t* )p_cmd_packet->params  )->max_rate );
      }
      break;

    case VS_VOCQCELP13K_CMD_SET_AVG_ENC_RATE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocqcelp13k_cmd_set_avg_enc_rate_proc(
             ( ( vs_vocqcelp13k_cmd_set_avg_enc_rate_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocqcelp13k_cmd_set_avg_enc_rate_t* )p_cmd_packet->params  )->rate );
      }
      break;

    case VS_VOCQCELP13K_CMD_SET_ENC_RATE_MODULATION:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocqcelp13k_cmd_set_enc_rate_mod_proc(
             ( ( vs_vocqcelp13k_cmd_set_enc_rate_mod_t* )p_cmd_packet->params )->handle,
             ( ( vs_vocqcelp13k_cmd_set_enc_rate_mod_t* )p_cmd_packet->params  )->rate_modulation );
      }
      break;

    /* G.729 */
    case VS_VOCG729_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocg729_cmd_set_dtx_mode_proc(
               ( ( vs_vocg729_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocg729_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    /* G711-ALAW */
    case VS_VOCG711ALAW_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocg711alaw_cmd_set_dtx_mode_proc(
               ( ( vs_vocg711alaw_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocg711alaw_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }

      break;

    /* G711-MULAW */
    case VS_VOCG711MULAW_CMD_SET_DTX_MODE:
      if( p_cmd_packet != NULL )
      {
        rc = vs_vocg711mulaw_cmd_set_dtx_mode_proc(
               ( ( vs_vocg711mulaw_cmd_set_dtx_mode_t* )p_cmd_packet->params )->handle,
               ( ( vs_vocg711mulaw_cmd_set_dtx_mode_t* )p_cmd_packet->params  )->enable_flag );
      }
      break;

    case VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER:
      {
        if ( p_apr_packet != NULL )
        {
          rc = vs_get_typed_object( p_apr_packet->dst_port,
                                    VS_OBJECT_TYPE_ENUM_SESSION,
                                    ( ( vs_object_t** ) &p_session_obj ) );
          if ( rc )
          {
             VS_REPORT_FATAL_ON_ERROR( rc );
             break;
          }

          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER received. src_port = 0x%08x, "
                 "dst_port=0x%08x, token=0x%08x", p_apr_packet->src_port,
                 p_apr_packet->dst_port, p_apr_packet->token );

          session_lock_p = vs_get_session_destroy_lock( p_apr_packet->dst_port );
		  
          /* Session lock is acquired to avoid concurrent access of 
           * buffer queues from VS and VS client context.
           */
          if ( session_lock_p != NULL )
          {
            VS_ACQUIRE_LOCK( *session_lock_p );
          }

          if ( p_session_obj->call_state == VS_CALL_STATE_ENUM_ENABLED )
          {
             client_type = vs_voc_check_client_type( p_session_obj->client_id );

             /* This packet exchange interface is meant only for MVS clients. */
             if ( VS_VOC_CLIENT_TYPE_MVS == client_type )
             {
               vs_common_send_event( p_session_obj->header.handle,
                                     VS_VOC_EVENT_WRITE_REQUEST, NULL, 0 );

               /**
                * cache the details of REQUEST_DEC_PKT, so as to faciliate the
                * new packet exchnages protocol between MVS<-->VS over the RX
                * path.
                */
               mmode_rx_packet.dst_addr = p_apr_packet->dst_addr;
               mmode_rx_packet.dst_port = p_apr_packet->dst_port;
               mmode_rx_packet.src_addr = p_apr_packet->src_addr;
               mmode_rx_packet.src_port = p_apr_packet->src_port;
               mmode_rx_packet.token = p_apr_packet->token;
             }
             else if( apr_list_remove_head( &p_session_obj->write_buf_q,
                                      ( ( apr_list_node_t** ) &p_wbuffer_node ) )
                                       == VS_EOK )
             {
               /* Reset vocoder buffer type to VS_VOC_BUFFER_TYPE_NONE.
                * Add node back to alloc_buf_q.
                */
                if( p_wbuffer_node != NULL )
                {
                  p_wbuffer = ( ( vs_voc_buffer_t* ) p_wbuffer_node->buf );
                  p_wbuffer_node->buf_type = VS_VOC_BUFFER_TYPE_NONE;
                  ( void ) apr_list_add_tail( &p_session_obj->alloc_buf_q,
                                              &p_wbuffer_node->link );
                  p_wbuffer_node = NULL;
                }

                /**
                 * Get decoder packet from VS client.
                 * Add packet header. Send decoder packet to CVD
                 */
                rc = vs_voc_send_dec_packet( p_session_obj->header.handle,
                                             p_wbuffer, p_apr_packet );
                if ( rc )
                {
                  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                         "vs_voc_send_dec_packet() retuned ERROR. rc=0x%08x", rc );
                }

                vs_common_send_event( p_session_obj->header.handle,
                                      VS_VOC_EVENT_WRITE_BUFFER_RETURNED,
                                      p_wbuffer,
                                      sizeof( vs_voc_event_write_buffer_returned_t) );
             }
             else
             {
               MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                    "rx_packet_status = NO vocoder packet to give to ADSP."  );
             }
          }
          else
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "Droping VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER as client=0x%08x "
                   "not in VS_CALL_STATE_ENUM_ENABLED state",
                   p_session_obj->client_id );
          }
          if ( session_lock_p != NULL )
          {
            VS_RELEASE_LOCK( *session_lock_p );
          }
        }
      }
      break;

    case VSS_ISTREAM_EVT_SEND_ENC_BUFFER:
      {
        if ( p_apr_packet != NULL )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                 "VSS_ISTREAM_EVT_SEND_ENC_BUFFER received. src_port = 0x%08x, "
                 "dst_port=0x%08x, token=0x%08x", p_apr_packet->src_port,
                 p_apr_packet->dst_port, p_apr_packet->token );

          rc = vs_get_typed_object( p_apr_packet->dst_port,
                                    VS_OBJECT_TYPE_ENUM_SESSION,
                                    ( (vs_object_t** ) &p_session_obj ) );
          if ( rc )
          {
             VS_REPORT_FATAL_ON_ERROR( rc );
             break;
          }
          session_lock_p = vs_get_session_destroy_lock( p_apr_packet->dst_port );
		  
          /* Session lock is acquired to avoid concurrent access of 
           * buffer queues from VS and VS client context.
           */
          if ( session_lock_p != NULL )
          {
            VS_ACQUIRE_LOCK( *session_lock_p );
          }

          if ( p_session_obj->call_state == VS_CALL_STATE_ENUM_ENABLED )
          {
            voc_packet_size =
              ( APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( p_apr_packet->header ) -
                                   sizeof( vss_istream_evt_send_enc_buffer_t ) );

            if ( voc_packet_size > VS_VOC_MAX_PACKET_LENGTH_V )
            {
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "VSS_ISTREAM_EVT_SEND_ENC_BUFFER: unexpected vocoder pkt "
                     "length=%d, discarding packet", voc_packet_size );
              if ( session_lock_p != NULL )
              {
                VS_RELEASE_LOCK( *session_lock_p );
              }
              break;
            }

            /* Detach the node from alloc_buf_q if it is Primed. */
            p_rbuffer_node =
               ( vs_voc_buffer_item_t* )( &p_session_obj->alloc_buf_q.dummy );
            for ( ;; )
            {
              rc = apr_list_get_next( &p_session_obj->alloc_buf_q,
                                      ( ( apr_list_node_t* ) p_rbuffer_node ),
                                     ( ( apr_list_node_t** ) &p_rbuffer_node ) );

              /* If alloc_buf_q list is ended. */
              if ( rc ) break;

              /* Got read primed buffer from client. */
              if ( p_rbuffer_node->buf_type == VS_VOC_BUFFER_TYPE_PRIMED )
              {
                ( void ) apr_list_delete( &p_session_obj->alloc_buf_q,
                                          ( (apr_list_node_t* ) p_rbuffer_node ) );
                MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                       "Read Primed buffer available=0x%8X",  p_rbuffer_node->buf );
                break;
              }
            }

            if ( rc == APR_EOK )
            {
              rc = vs_voc_send_enc_packet( p_session_obj->header.handle,
                                           p_rbuffer_node, p_apr_packet );

              if ( rc )
              {
                /* Put the buffer back to alloc_buf_q */
                ( void ) apr_list_add_tail( &p_session_obj->alloc_buf_q,
                                            &( p_rbuffer_node->link ) );
                MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                       "vs_voc_send_enc_packet() retuned ERROR. rc=0x%08x", rc );
              }
            }
            else
            {
              MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "tx_packet_status = NO client buffer to copy vocoder packet "
                   "from ADSP." );
            }
          }
          else
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "Droping VSS_ISTREAM_EVT_SEND_ENC_BUFFER as VS session "
                   "handle=0X%08X not in VS_CALL_STATE_ENUM_ENABLED.",
                   p_session_obj->header.handle );
          }
          if ( session_lock_p != NULL )
          {
            VS_RELEASE_LOCK( *session_lock_p );
          }
        }
      }
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_process_med_priority_nongating_requests(): Unsupported "
             "request_id=(0X%08X)", request_id );
      break;
    } /* switch case ends */

    /**
     * Memory cleanup for all Supported and Unsupported request_id
     * is freed here, both for vs_cmd_packet_t aand aprv2_packet_t.
     */
    if ( p_apr_packet != APR_NULL_V )
    {
      rc = __aprv2_cmd_free( vs_apr_handle, p_apr_packet );
      VS_REPORT_FATAL_ON_ERROR( rc );
      p_apr_packet = NULL;
    }

    ( void ) vs_free_cmd_packet ( p_cmd_packet );
    p_cmd_packet = NULL;

    request_id = 0;

  } /* while loop ends */

  return;
} /* vs_process_med_priority_nongating_requests() ends */

static void vs_process_med_priority_commands_and_events (
  void
)
{
  vs_process_med_priority_nongating_requests( );
} /* vs_process_med_priority_commands_and_events() ends */

/****************************************************************************
 * LOW PRIORITY REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS.                   *
 ****************************************************************************/
static void vs_process_low_priority_nongating_requests (
  void
)
{
  uint32_t rc = VS_EOK;
  uint32_t request_id = 0;
  vs_work_item_t* p_work_item = NULL;
  aprv2_packet_t*  p_apr_packet = NULL;
  vs_cmd_packet_t* p_cmd_packet = NULL;

  while( apr_list_remove_head( &vs_low_priority_nongating_packet_q,
                                ( ( apr_list_node_t** ) &p_work_item ) )== VS_EOK )
  {
    /**
     * Validate the p_work_item. */
    if ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_APR )
    {
      p_apr_packet = ( ( aprv2_packet_t* ) p_work_item->packet );
      request_id = p_apr_packet->opcode;
    }
    else if ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_CMD )
    {
      p_cmd_packet = ( ( vs_cmd_packet_t* ) p_work_item->packet );
      request_id = p_cmd_packet->cmd_id;
    }
    else
    {
      VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );
    }

    /* Add back to vs_free_work_pkt_q. */
    p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
    p_work_item->packet = NULL;
    ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_process_low_priority_nongating_requests(): Processing "
           "request_id=(0X%08X", request_id );

    switch ( request_id )
    {
    case VS_VOC_RSPI_DESTROY_SESSION:
      rc = vs_voc_rspi_destroy_proc( p_cmd_packet );
      break;

    case VS_VOC_CMDI_ENABLE:
    case VS_VOC_CMDI_DISABLE:
    case VS_VOC_CMDI_STANDBY:
    case VS_VOC_CMDI_CLOSE:
    case VS_VOC_CMDI_FLUSH_BUFFERS:
    case VS_VOC_CMDI_RESTORE_CVD_STATE:
    case VS_VOC_CMDI_CLEANUP_CVD_REFERENCES:
    case VS_VOC_CMDI_DESTROY_CVD_ON_ADSP_RECOVERY:
    case VS_VOC_CMDI_DISBALE_CVD_ON_ADSP_RECOVERY:
      {
      rc = vs_queue_pending_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
                                        p_cmd_packet );
        if ( rc == APR_EOK )
        {
          /* As these packets are further queued so we need to prevent them from
           * getting freed. */
          p_cmd_packet = NULL;
        }
      }
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_process_low_priority_nongating_requests(): Unsupported "
             "request_id=(0X%08X)", request_id );
      break;
    } /* switch case ends */

    /**
     * Memory cleanup for all Supported and Unsupported request_id
     * is freeds here, both for vs_cmd_packet_t aand aprv2_packet_t.
     */
    if ( p_apr_packet != APR_NULL_V )
    {
      rc = __aprv2_cmd_free( vs_apr_handle, p_apr_packet );
          VS_REPORT_FATAL_ON_ERROR( rc );
      p_apr_packet = NULL;
    }
    vs_free_cmd_packet ( p_cmd_packet );
    p_cmd_packet = NULL;
    request_id = 0;
  } /* while loop ends */

  return;
} /* vs_process_low_priority_nongating_requests() ends */


static void vs_process_low_priority_gating_requests (
  void
)
{
  uint32_t rc = VS_EOK;
  uint32_t request_id = 0;
  vs_work_item_t* p_work_item = NULL;
  vs_cmd_packet_t* p_cmd_packet = NULL;
  aprv2_packet_t* p_apr_packet = NULL;
  vs_gating_control_t* ctrl = &vs_low_priority_gating_packet_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
    case VS_GATING_CMD_STATE_ENUM_FETCH:
      {
        /* Fetch the next gating command to execute. */
        rc = apr_list_remove_head( &ctrl->cmd_q,
                                   ( ( apr_list_node_t** ) &p_work_item ) );
        if ( rc ) return;

        if ( ( p_work_item->packet == NULL ) ||
             ( p_work_item->pkt_type == VS_WORK_ITEM_PKT_TYPE_NONE ) )
        {
          VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );
        }
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_process_low_priority_gating_requests(): "
             "VS_GATING_CMD_STATE_ENUM_FETCH");

        ctrl->packet = p_work_item->packet;
        ctrl->pkt_type = p_work_item->pkt_type;
        ctrl->state = VS_GATING_CMD_STATE_ENUM_EXECUTE;

        /* Add the vs_work_item_t back to vs_free_work_pkt_q. */
        p_work_item->packet = NULL;
        p_work_item->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
        ( void ) apr_list_add_tail( &vs_free_work_pkt_q, &p_work_item->link );
      }
      break;

    case VS_GATING_CMD_STATE_ENUM_EXECUTE:
    case VS_GATING_CMD_STATE_ENUM_CONTINUE:
      {
        if ( ctrl->pkt_type == VS_WORK_ITEM_PKT_TYPE_APR )
        {
          p_apr_packet = ( ( aprv2_packet_t* ) ctrl->packet );
          request_id = p_apr_packet->opcode;
        }
        else if ( ctrl->pkt_type == VS_WORK_ITEM_PKT_TYPE_CMD )
        {
          p_cmd_packet = ( ( vs_cmd_packet_t* ) ctrl->packet );
          request_id = p_cmd_packet->cmd_id;
        }
        else
        {
          VS_PANIC_ON_ERROR ( VS_EUNEXPECTED );
        }
        /**
         * For Supported commands, processing commands should take care
         * of releasing memory allocated for command and params.
         */
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
		                 "vs_process_low_priority_gating_requests(): request_id=(0x%08x)",
                request_id );

        switch ( request_id )
        {
        case VS_VOC_CMDI_ENABLE:
          rc = vs_voc_cmdi_enable_proc( ctrl );
          break;

        case VS_VOC_CMDI_STANDBY:
          rc = vs_voc_cmdi_standby_proc( ctrl );
          break;

        case VS_VOC_CMDI_DISABLE:
          rc = vs_voc_cmdi_disable_proc( ctrl );
          break;

        case VS_VOC_CMDI_CLOSE:
          rc  = vs_voc_cmdi_close_proc( ctrl );
          break;

        case VS_VOC_CMDI_FLUSH_BUFFERS:
          rc = vs_voc_cmdi_flush_buffers_proc( ctrl );
          break;

        case VS_VOC_CMDI_DISBALE_CVD_ON_ADSP_RECOVERY:
          rc = vs_voc_cmdi_disable_cvd_on_adsp_recovery_proc( ctrl );
          break;

        case VS_VOC_CMDI_DESTROY_CVD_ON_ADSP_RECOVERY:
          rc = vs_voc_cmdi_destroy_cvd_on_adsp_recovery_proc( ctrl );
          break;

        case VS_VOC_CMDI_CLEANUP_CVD_REFERENCES:
          rc  = vs_voc_cmdi_cleanup_cvd_reference_proc( ctrl );
          break;

        case VS_VOC_CMDI_RESTORE_CVD_STATE:
          rc = vs_voc_cmdi_restore_cvd_state_proc ( ctrl );
          break;

        default:
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "vs_process_low_priority_gating_requests(): Unsupported "
                   "request_id=(0X%08X)", request_id );

            /**
             * For unsupported request_id, memory cleanup required for
             * CMD packets and APR packets
             */
            if ( p_apr_packet != NULL )
            {
              rc = __aprv2_cmd_free( vs_apr_handle, p_apr_packet );
              VS_REPORT_FATAL_ON_ERROR( rc );
              p_apr_packet = APR_NULL_V;
            }
            ( void ) vs_free_cmd_packet ( p_cmd_packet );

            /* set to VS_EOK to fetch the next command in queue. */
            rc = VS_EOK;
          }
          break;
        }

        /* Evaluate the pending command completion status. */
        switch ( rc )
        {
        case VS_EOK:
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                   "vs_process_low_priority_gating_requests(): request_id=(0X%08X) "
                   "processed successfully ", request_id );
            ctrl->packet = NULL;
            ctrl->pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
            /* The current command is finished so fetch the next command. */
            ctrl->state = VS_GATING_CMD_STATE_ENUM_FETCH;
          }
          break;

        case VS_EPENDING:
          /**
           * Assuming the current pending command control routine returns
           * VS_EPENDING the overall progress stalls until one or more
           * external events or responses are received.
           */
          ctrl->state = VS_GATING_CMD_STATE_ENUM_CONTINUE;
          /**
           * Return from here so as to avoid unecessary looping till reponse
           * is recived.
           */
          return;

        default:
          VS_PANIC_ON_ERROR( VS_EUNEXPECTED );
          break;
        }
      }
      break;

    default:
      VS_PANIC_ON_ERROR( rc );
      break;
    }/* switch case ends. */
  } /* for loop ends. */

  return;
} /* vs_process_low_priority_gating_requests() ends */

static void vs_process_low_priority_commands_and_events (
  void
)
{
  vs_process_low_priority_nongating_requests( );
  vs_process_low_priority_gating_requests( );
} /* vs_process_low_priority_commands_and_events() ends */

/****************************************************************************
 * VS OBJECT ROUTINES...START                                               *
 ****************************************************************************/
static uint32_t vs_typecast_object (
  apr_objmgr_object_t* p_store,
  vs_object_type_enum_t type,
  vs_object_t** p_ret_obj
)
{

  vs_object_t* p_obj = NULL;

  if ( ( p_store == NULL ) || ( p_ret_obj == NULL ) )
  {
    return VS_EBADPARAM;
  }

  p_obj = ( ( vs_object_t* ) p_store->any.ptr );

  if ( p_obj->header.type != type )
  {
    return VS_EFAILED;
  }

  *p_ret_obj = p_obj;

  return VS_EOK;

}  /* end of vs_typecast_object () */

VS_INTERNAL uint32_t vs_get_object (
  uint32_t handle,
  vs_object_t** ret_obj
)
{

  uint32_t rc = VS_EOK;
  apr_objmgr_object_t* p_objmgr_obj = NULL;

  for ( ;; )
  {
    if ( ret_obj == NULL )
    {
      rc = VS_EBADPARAM;
      break;
    }

    rc = apr_objmgr_find_object( &vs_objmgr, handle, &p_objmgr_obj );
    if ( rc )
    {
      rc = VS_EFAILED;
      break;
    }

    *ret_obj = ( ( vs_object_t* ) p_objmgr_obj->any.ptr );
    break;
  }  /* end of for ( ;; ) */

  return rc;

}  /* end of vs_get_object () */

VS_INTERNAL uint32_t vs_get_typed_object (
  uint32_t handle,
  vs_object_type_enum_t type,
  vs_object_t** p_ret_obj
)
{

  uint32_t rc = VS_EOK;
  apr_objmgr_object_t* p_store = NULL;

  if ( handle == 0 )
  {
    return VS_EHANDLE;
  }

  rc = apr_objmgr_find_object( &vs_objmgr, handle, &p_store );
  if ( rc )
  {
    return ( uint32_t )rc;
  }

  rc = vs_typecast_object( p_store, type, p_ret_obj );
  if ( rc )
  {
    return ( uint32_t )rc;
  }

  return VS_EOK;

} /* end of vs_get_typed_object () */
/****************************************************************************
 * VS OBJECT ROUTINES...END                                                 *
 ****************************************************************************/


/****************************************************************************
 * VS OBJECT CREATION AND DESTRUCTION ROUTINES...START                      *
 ****************************************************************************/
VS_INTERNAL uint32_t vs_mem_alloc_object (
  uint32_t size,
  vs_object_t** p_ret_object
)
{
  uint32_t rc = VS_EOK;
  vs_object_t* p_vs_obj = NULL;
  apr_objmgr_object_t* p_objmgr_obj = NULL;

  if ( p_ret_object == NULL )
  {
    return VS_EBADPARAM;
  }

  { /* Allocate memory for the VS object. */
    p_vs_obj = apr_memmgr_malloc( &vs_heapmgr, size );
    if ( p_vs_obj == NULL )
    {
      rc = VS_ENORESOURCE;
      return rc;
    }

    /* Allocate a new handle for the VS object. */
    rc = apr_objmgr_alloc_object( &vs_objmgr, &p_objmgr_obj );
    if ( rc )
    {
      apr_memmgr_free ( &vs_heapmgr, p_vs_obj );
      rc = VS_ENORESOURCE;
      return rc;
    }

    /* Link the VS object to the handle. */
    p_objmgr_obj->any.ptr = p_vs_obj;

    /* Initialize the base VS object header. */
    p_vs_obj->header.handle = p_objmgr_obj->handle;
    p_vs_obj->header.type = VS_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *p_ret_object = p_vs_obj;

  return VS_EOK;
}  /* end of vs_mem_alloc_object () */


VS_INTERNAL uint32_t vs_mem_free_object (
  vs_object_t* p_object
)
{
  if ( p_object == NULL )
  {
    return VS_EBADPARAM;
  }

  if ( ( p_object->header.type == VS_OBJECT_TYPE_ENUM_SIMPLE_JOB ) ||
       ( p_object->header.type == VS_OBJECT_TYPE_ENUM_HANDLE ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_mem_free_object():  p_object=(0X%08X)", p_object );
    ( void ) vs_voc_ssr_untrack_object( p_object->header.handle );
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &vs_objmgr, p_object->header.handle );

  apr_memmgr_free( &vs_heapmgr, p_object );

  return VS_EOK;
}  /* end of vs_mem_free_object () */

static void vs_default_event_handler_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;

  /* The default event handler just drops the packet. A specific event
   * handler routine should be written to something more useful.
   */
  if ( p_packet != APR_NULL_V )
  {
    rc =  __aprv2_cmd_free( vs_apr_handle, p_packet );
    VS_REPORT_FATAL_ON_ERROR( rc );
    p_packet = APR_NULL_V;
  }
}  /* end of vs_default_event_handler_fn () */

static void vs_set_default_response_table (
  vs_response_fn_table_t table
)
{
  int i;

  if ( table == NULL )
  {
    return;
  }

  /* Initialize the state response handler function table. */
  for ( i = 0; i < VS_RESPONSE_FN_ENUM_MAX; ++i )
  {
    table[ i ] = vs_default_event_handler_fn;
  }

  return;
}  /* end of vs_set_default_response_table () */

static void vs_simple_self_destruct_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
  vs_token_object_t* p_token_obj = NULL;

  if ( p_packet == APR_NULL_V )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return;
  }

  rc = vs_get_typed_object( p_packet->token, VS_OBJECT_TYPE_ENUM_HANDLE,
                            ( ( vs_object_t** ) &p_token_obj ) );
  if ( rc == VS_EOK )
  {
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
   p_token_obj = NULL;
  }

  rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
  p_packet = APR_NULL_V;

  vs_signal_run_thrd3();
}  /* end of vs_simple_self_destruct_result_rsp_fn () */


/**
 * VS_OBJECT_TYPE_ENUM_SIMPLE_JOB created with below response
 * fucntion does not have the object reference destroyed.
 *
 * Once responce the available the simple object has to be
 * destoyred explicitly.
 */
VS_INTERNAL void vs_simple_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* p_simple_obj = NULL;

  for ( ;; )
  {
    if ( p_packet == APR_NULL_V )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    rc = vs_get_typed_object( p_packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &p_simple_obj ) );
    if( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );      
      break;
    }

    p_simple_obj->is_completed = TRUE;
    p_simple_obj->status =
       APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, p_packet )->status;
    
    break;
  }

  rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
  p_packet = APR_NULL_V;  

  vs_signal_run_thrd3();
  return;
}  /* end of vs_simple_result_rsp_fn ()  */


/**
 * This response function is associated to VS_OBJECT_TYPE_ENUM_SIMPLE_JOB
 * created for facilitating the vocoder state transition of a VS SESSION.
 *
 * Some VS commands needs CVD response to procees further with the
 * vocoder state machine, hence the ADSP/CVD timeout management and recovery
 * handling has to be done.
 */
VS_INTERNAL void vs_simple_transition_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = VS_EOK;
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
    p_simple_obj->status = p_rsp_result->status;
    p_simple_obj->is_completed = TRUE;

    /*
     * Scenarios where VS client closes the session before ADSP recovery
     * and SSR does not occured, need to avoid VS CRASH
     */
    rc = vs_get_typed_object( p_simple_obj->context_handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );

    VS_REPORT_FATAL_ON_ERROR( rc );

    /* Stop the timer triggered for ADSP/CVD timeout. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_simple_transition_result_rsp_fn(): apr_timer_stop counter = %d",
           timer_stop_cntr++ );
    apr_timer_stop( vs_voc_adsp_timeout_timer );

    /* Q CMD for VS cleanup after ADSP/CVD recovers post timeout. */
    if ( vs_is_adsp_timeout_triggered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_simple_transition_result_rsp_fn(): ADSP recovered post timeout" );

      rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_LOW,
              VS_VOC_CMDI_DISBALE_CVD_ON_ADSP_RECOVERY, NULL, 0 );
      vs_is_adsp_timeout_triggered = FALSE;
    }

    if ( p_session_obj != NULL )
    {
      VS_ACQUIRE_LOCK( p_session_obj->session_ctrl_lock );
      p_session_obj->session_ctrl.status = p_simple_obj->status;
      VS_RELEASE_LOCK( p_session_obj->session_ctrl_lock );

      /* If completed action failed, log the error. */
      if ( p_simple_obj->status != APR_EOK )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_simple_transition_result_rsp_fn(): command (0x%08x) failed "
               "with rc=(0x%08x)", p_rsp_result->opcode, p_rsp_result->status  );
      }
    }

    /* Free the apr cmd response packet. */
    rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
    VS_REPORT_FATAL_ON_ERROR( rc );

    /* free the simple job object. */
    ( void ) vs_mem_free_object( ( vs_object_t* ) p_simple_obj );

    break;
  }  

  vs_signal_run_thrd3();
  return;
}  /* end of vs_simple_transition_result_rsp_fn () */

VS_INTERNAL uint32_t vs_create_autofree_token_object (
  vs_token_object_t** p_ret_obj
)
{
  uint32_t rc = VS_EOK;
  vs_token_object_t* p_vs_obj = NULL;

  if ( p_ret_obj == NULL )
  {
    return VS_EBADPARAM;
  }

  rc = vs_mem_alloc_object( sizeof( vs_token_object_t ),
                            ( ( vs_object_t** ) &p_vs_obj ) );
  VS_PANIC_ON_ERROR( rc );

  {  /* Initialize the handle object. */
    p_vs_obj->header.type = VS_OBJECT_TYPE_ENUM_HANDLE;
    vs_set_default_response_table( p_vs_obj->fn_table );
    p_vs_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] =
                        vs_simple_self_destruct_result_rsp_fn;
  }

  { /* Track the allocated object in order to free in case of ADSP SSR. */
    rc = vs_voc_ssr_track_object( p_vs_obj->header.handle );
  }

  *p_ret_obj = p_vs_obj;

  return VS_EOK;
}  /* end of vs_create_autofree_token_object () */


VS_INTERNAL uint32_t vs_create_simple_job_object (
  uint32_t context_handle,
  vs_simple_job_object_t** p_ret_job_obj
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* p_vs_obj = NULL;

  if ( p_ret_job_obj == NULL )
  {
    return VS_EBADPARAM;
  }

  rc = vs_mem_alloc_object( sizeof( vs_simple_job_object_t ),
                           ( ( vs_object_t** ) &p_vs_obj ) );
  VS_PANIC_ON_ERROR( rc );

  { /* Initialize the simple job object. */
    p_vs_obj->header.type = VS_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    p_vs_obj->context_handle = context_handle;
    vs_set_default_response_table( p_vs_obj->fn_table );
    p_vs_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_simple_result_rsp_fn;
    p_vs_obj->is_accepted = FALSE;
    p_vs_obj->is_completed = FALSE;
  }

  { /* Track the allocated object in order to free in case of ADSP SSR. */
    rc = vs_voc_ssr_track_object( p_vs_obj->header.handle );
  }

  *p_ret_job_obj = p_vs_obj;

  return VS_EOK;
}  /* end of vs_create_simple_job_object () */

VS_INTERNAL uint32_t vs_create_sequencer_job_object (
  vs_sequencer_job_object_t** p_ret_job_obj
)
{
  uint32_t rc = VS_EOK;
  vs_sequencer_job_object_t* p_job_obj = NULL;

  if ( p_ret_job_obj == NULL )
  {
    return VS_EBADPARAM;
  }

  rc = vs_mem_alloc_object( sizeof( vs_sequencer_job_object_t ),
                             ( ( vs_object_t** ) &p_job_obj ) );
  VS_PANIC_ON_ERROR( rc );

  { /* Initialize the pending job object. */
    p_job_obj->header.type = VS_OBJECT_TYPE_ENUM_SEQUENCER_JOB;

    p_job_obj->state = APR_NULL_V;
    p_job_obj->subjob_obj = NULL;
    p_job_obj->status = APR_UNDEFINED_ID_V;
  }

  *p_ret_job_obj = p_job_obj;

  return VS_EOK;
}  /* end of vs_create_sequencer_job_object () */


/****************************************************************************
 * IST ROUTINES                                                             *
 ****************************************************************************/

static int32_t vs_run_p1_thread ( void )
{
  vs_delegate_work_pkt_requests( );
  return APR_EOK;
}

static int32_t vs_high (
  void* param
)
{
  uint32_t rc = VS_EOK;

  rc = ( uint32_t ) apr_event_create( &vs_high_event );
  VS_PANIC_ON_ERROR( rc );

  vs_high_state = VS_THREAD_STATE_ENUM_READY;
  apr_event_signal( vs_thread_create_event );

  do
  {
    rc = ( uint32_t )apr_event_wait( vs_high_event );
    #ifndef USE_SINGLE_AUDSVC_THREAD
    ( void ) vs_run_p1_thread( );
    #endif /* !USE_SINGLE_AUDSVC_THREAD */
  }
  while ( rc == VS_EOK );

  rc = ( uint32_t )apr_event_destroy( vs_high_event );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_high_state = VS_THREAD_STATE_ENUM_EXIT;

  return ( int32_t )VS_EOK;
}  /* end of vs_high () */

static int32_t vs_run_p2_thread ( void )
{
  vs_process_med_priority_commands_and_events( );
  return APR_EOK;
}

static int32_t vs_med (
  void* param
)
{
  uint32_t rc = VS_EOK;

  rc = ( uint32_t ) apr_event_create( &vs_med_event );
  VS_PANIC_ON_ERROR( rc );

  vs_med_state = VS_THREAD_STATE_ENUM_READY;
  apr_event_signal( vs_thread_create_event );

  do
  {
    rc = ( uint32_t )apr_event_wait( vs_med_event );
    #ifndef USE_SINGLE_AUDSVC_THREAD
    ( void ) vs_run_p2_thread( );
    #endif /* !USE_SINGLE_AUDSVC_THREAD */
  }
  while ( rc == VS_EOK );

  vs_med_state = VS_THREAD_STATE_ENUM_INIT;
  /* Wait for Low Thread to stop running */
  {
    int32_t timeout = 5000;
    while ( vs_low_state == VS_THREAD_STATE_ENUM_READY && timeout-- )
      ( void ) apr_misc_sleep( 10000 ); /* Sleep for 10us. */
    if( timeout <= 0 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_med(): vs_low failed to stop; vs_low_state=(%d)",
             vs_low_state );
    }
  }
  rc = ( uint32_t )apr_event_destroy( vs_med_event );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_med_state = VS_THREAD_STATE_ENUM_EXIT;

  return ( int32_t )VS_EOK;
}  /* end of vs_med () */

static int32_t vs_run_p3_thread ( void )
{
  vs_process_low_priority_commands_and_events( );
  return APR_EOK;
}

static int32_t vs_low (
  void* param
)
{
  uint32_t rc = VS_EOK;

  rc = ( uint32_t ) apr_event_create( &vs_low_event );
  VS_PANIC_ON_ERROR( rc );

  vs_low_state = VS_THREAD_STATE_ENUM_READY;
  apr_event_signal( vs_thread_create_event );

  do
  {
    rc = ( uint32_t )apr_event_wait( vs_low_event );
    #ifndef USE_SINGLE_AUDSVC_THREAD
    ( void ) vs_run_p3_thread( );
    #endif /* !USE_SINGLE_AUDSVC_THREAD */
  }
  while ( rc == VS_EOK );

  vs_low_state = VS_THREAD_STATE_ENUM_INIT;
  /* Wait for Med Thread to stop running */
  {
    int32_t timeout = 1000;
    while ( vs_med_state == VS_THREAD_STATE_ENUM_READY && timeout-- )
      ( void ) apr_misc_sleep( 10000 ); /* Sleep for 10us. */
    if( timeout <= 0 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_low(): vs_med failed to stop; vs_med_state=(%d)",
             vs_med_state );
    }
  }
  rc = ( uint32_t )apr_event_destroy( vs_low_event );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_low_state = VS_THREAD_STATE_ENUM_EXIT;

  return ( int32_t )VS_EOK;
}  /* end of vs_high () */


/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static uint32_t vs_init (
  void
)
{
  uint32_t rc = VS_EOK;
  uint32_t i;
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "vs_init() Date %s Time %s", __DATE__, __TIME__ );

  /**
   * Lets create three locks, one for simple resource
   * synchronization, second one non pre-emptebile and the third
   * to make synchornous blocking calls thread safe.
   */
  {
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &vs_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_thread_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_voc_cmd_thread_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_voc_data_buf_lock );
  }

  /**
   * Lets create per client session locks,
   */
  {
    for ( i = 0; i < VS_MAX_NUM_CLIENTS; ++i )
    {
      vs_session_lock_pool[i].session_handle = 0xFFFFFFFF;
      rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_session_lock_pool[i].lock );
    }
  }

  /* Initialize the custom heap. */
  {
    apr_memmgr_init_heap( &vs_heapmgr, ( ( void* ) &vs_heap_pool ),
                           sizeof( vs_heap_pool ), NULL, NULL );
    /**
     * memheap must not be called from interrupt context. No locking
     * is required in task context because all commands are
     * serialized.
     */
  }

  /* Initialize the object manager. */
  {
    apr_objmgr_setup_params_t params;
    params.table = vs_object_table;
    params.total_bits = VS_HANDLE_TOTAL_BITS_V;
    params.index_bits = VS_HANDLE_INDEX_BITS_V;
    params.lock_fn = vs_int_lock_fn;
    params.unlock_fn = vs_int_unlock_fn;
    rc = apr_objmgr_construct( &vs_objmgr, &params );
  }

  /* Initialize the command queue management. */
  {
    /* Populate free VS WORK PKT queue structures */
    {
      rc = apr_list_init_v2( &vs_free_work_pkt_q, vs_int_lock_fn,
                             vs_int_unlock_fn );
      for ( i = 0; i < VS_NUM_WORK_PKTS_V; ++i )
      {
        ( void ) apr_list_init_node( ( apr_list_node_t* ) &vs_work_pkt_pool[i] );
        vs_work_pkt_pool[i].pkt_type = VS_WORK_ITEM_PKT_TYPE_NONE;
        vs_work_pkt_pool[i].packet = NULL;
        rc = apr_list_add_tail( &vs_free_work_pkt_q,
                                ( ( apr_list_node_t* ) &vs_work_pkt_pool[i] ) );
      }
    }

    /* Initialize work queue structures. */
    {
      rc = apr_list_init_v2( &vs_work_pkt_q, vs_int_lock_fn, vs_int_unlock_fn );
    }

    /* Initialize gating commond queue structures. */
    {
      rc = vs_gating_control_init( &vs_low_priority_gating_packet_q );
    }

    /* Initialize non-gating commond queue structures. */
    {
      rc = apr_list_init_v2( &vs_med_priority_nongating_packet_q, vs_int_lock_fn,
                             vs_int_unlock_fn );
      rc = apr_list_init_v2( &vs_low_priority_nongating_packet_q, vs_int_lock_fn,
                             vs_int_unlock_fn );
    }
  }

  /* Initialize the VS session tracking list. */
  {
    {
      rc = apr_list_init_v2( &vs_free_session_q, NULL, NULL );
      /* Populate the free session list. */
      for ( i = 0; i < VS_MAX_NUM_SESSIONS_V; ++i )
      {
        ( void ) apr_list_init_node( ( apr_list_node_t* ) &vs_session_list_pool[i] );
        rc = apr_list_add_tail(
               &vs_free_session_q,
               ( ( apr_list_node_t* ) &vs_session_list_pool[ i ] ) );
      }
    }
    rc = apr_list_init_v2( &vs_used_session_q, NULL, NULL );
  }

  /* Create the three different threads. */
  {
    rc = apr_event_create( &vs_thread_create_event );
    VS_PANIC_ON_ERROR( rc );

    {
      rc = apr_thread_create( &vs_high_handle, VS_HIGH_TASK_NAME, VS_HIGH_TASK_PRIORITY,
                              vs_high_task_stack, VS_HIGH_TASK_STACK_SIZE, vs_high, NULL );
      VS_PANIC_ON_ERROR( rc );

      /* Wait for service thread to be created. */
      ( void ) apr_event_wait( vs_thread_create_event );
    }

    {
      rc = apr_thread_create( &vs_med_handle, VS_MED_TASK_NAME, VS_MED_TASK_PRIORITY,
                              vs_med_task_stack, VS_MED_TASK_STACK_SIZE, vs_med, NULL );
      VS_PANIC_ON_ERROR( rc );

      /* Wait for service thread to be created. */
      ( void ) apr_event_wait( vs_thread_create_event );

    }

    {
      rc = apr_thread_create( &vs_low_handle, VS_LOW_TASK_NAME, VS_LOW_TASK_PRIORITY,
                              vs_low_task_stack, VS_LOW_TASK_STACK_SIZE, vs_low, NULL );
      VS_PANIC_ON_ERROR( rc );

      /* Wait for service thread to be created. */
      ( void ) apr_event_wait( vs_thread_create_event );

    }

    rc = apr_event_destroy( vs_thread_create_event );
    VS_REPORT_FATAL_ON_ERROR( rc );
  }

  {
#ifndef WINSIM
    /* Get eAMR config from voicecfg framework. */
    rc = vs_eamr_mode_call( );
#else
    rc = APR_EFAILED;
#endif /* !WINSIM */
  }

  /* Initialize VS VOC SSR management. */
  rc = vs_voc_ssr_init( );

  /* Initialize the APR resource. */
  {
    rc = __aprv2_cmd_local_dns_lookup(
           vs_mvm_dns, sizeof( vs_mvm_dns ), &vs_mvm_addr );

    rc = __aprv2_cmd_local_dns_lookup(
           vs_cvs_dns, sizeof( vs_cvs_dns ), &vs_cvs_addr );

    rc = __aprv2_cmd_register2(
           &vs_apr_handle, vs_my_dns, sizeof( vs_my_dns ), 0,
           vs_isr_dispatch_fn, NULL, &vs_my_addr );
  }

  vs_is_initialized = TRUE;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "vs_init(), rc=0x%08x", rc );
  return rc;

}  /* end of vs_init () */

static uint32_t vs_post_init (
  void
)
{
  return VS_EOK;
}  /* end of vs_post_init () */

static uint32_t vs_deinit (
  void
)
{
  vs_session_object_t* p_session_obj = NULL;
  vs_generic_item_t* p_generic_item = NULL;
  uint32_t rc = VS_EOK;
  uint32_t i;

  vs_is_initialized = FALSE;

  /* Release the APR resource. */
  ( void ) __aprv2_cmd_deregister( vs_apr_handle );

  ( void ) apr_event_signal_abortall( vs_high_event );
  ( void ) apr_event_signal_abortall( vs_med_event );
  ( void ) apr_event_signal_abortall( vs_low_event );

  /* Destroy VS_HIGH */
  {
    if( ( vs_check_thread_state_timeout( &vs_high_state,
                          VS_THREAD_STATE_ENUM_EXIT, 1000 ) ) != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_deinit(): vs_high failed to exit vs_high_state=(%d)",
             vs_high_state );
    }
    else
    ( void ) apr_thread_destroy( vs_high_handle );
  }

  /* Destroy VS_MED */
  {
    if( ( vs_check_thread_state_timeout( &vs_med_state,
                          VS_THREAD_STATE_ENUM_EXIT, 10000 ) ) != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_deinit(): vs_med failed to exit vs_med_state=(%d)",
             vs_med_state );
    }
    else
    ( void ) apr_thread_destroy( vs_med_handle );
  }

  /* Destroy VS_LOW */
  {
    if( ( vs_check_thread_state_timeout( &vs_low_state,
                          VS_THREAD_STATE_ENUM_EXIT, 1000 ) ) != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_deinit(): vs_low failed to exit vs_low_state=(%d)",
             vs_low_state );
    }
    else
    ( void ) apr_thread_destroy( vs_low_handle );
  }

  /* Destroy session objects */
  p_generic_item = ( ( vs_generic_item_t* ) &vs_used_session_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &vs_used_session_q,
                            ( ( apr_list_node_t* ) p_generic_item ),
                            ( ( apr_list_node_t** ) &p_generic_item ) );
    if ( rc )
    {
      break;
    }

    rc = vs_get_typed_object( p_generic_item->handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );

    rc = apr_list_destroy( &p_session_obj->alloc_buf_q );
    rc = apr_list_destroy( &p_session_obj->write_buf_q );
    rc = apr_list_destroy( &p_session_obj->read_buf_q );
    ( void ) apr_lock_destroy( p_session_obj->session_ctrl_lock );
    ( void ) apr_lock_destroy( p_session_obj->client_state_lock );
    ( void ) apr_lock_destroy( p_session_obj->common_lock );
    ( void ) vs_mem_free_object( (vs_object_t*) p_session_obj);

  }  /* end of for ( ;; ) */

  /* Release session queue structures. */
  ( void ) apr_list_destroy( &vs_used_session_q );
  ( void ) apr_list_destroy( &vs_free_session_q );

  /* Release the command queue management. */
  ( void ) apr_list_destroy( &vs_med_priority_nongating_packet_q );
  ( void ) apr_list_destroy( &vs_low_priority_nongating_packet_q );

  ( void ) apr_list_destroy( &vs_free_work_pkt_q );

  /* Release gating control structures */
  ( void ) vs_gating_control_destroy( &vs_low_priority_gating_packet_q );

  /* Release work queue */
  ( void ) apr_list_destroy( &vs_work_pkt_q );

  /* Release the object management. */
  ( void ) apr_objmgr_destruct( &vs_objmgr );

  /* Release the locks. */
  for ( i = 0; i < VS_MAX_NUM_CLIENTS; ++i )
    ( void ) apr_lock_destroy( vs_session_lock_pool[i].lock );

  ( void ) apr_lock_destroy( vs_voc_data_buf_lock );
  ( void ) apr_lock_destroy( vs_voc_cmd_thread_lock );
  ( void ) apr_lock_destroy( vs_thread_lock );
  ( void ) apr_lock_destroy( vs_int_lock );

  ( void ) vs_voc_ssr_deinit( );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "vs_deinit(), rc=VS_EOK" );
  return VS_EOK;
}  /* end of vs_deinit () */

static uint32_t vs_pre_deinit (
  void
)
{
  return VS_EOK;
} /* end of vs_pre_deinit () */

/**
 * Provides a single API entry-point into VS.
 *
 * \param[in] cmd_id The command identifier to execute.
 * \param[in] params The command parameters.
 * \param[out] size The size of the command parameters in bytes.
 *
 * \return VS_EOK (0) when successful.
 */
VS_EXTERNAL uint32_t vs_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = VS_EOK;

  switch ( cmd_id )
  {
  #if defined WINSIM
  case DRV_CMDID_INIT:
  #else
  case VS_CALLINDEX_ENUM_INIT:
  #endif
    {
      rc = vs_init( );
    }
    break;

  #if defined WINSIM
  case DRV_CMDID_POSTINIT:
  #else
  case VS_CALLINDEX_ENUM_POSTINIT:
  #endif
    {
      rc = vs_post_init( );
    }
    break;

  #if defined WINSIM
  case DRV_CMDID_PREDEINIT:
  #else
  case VS_CALLINDEX_ENUM_PREDEINIT:
  #endif
    {
      rc = vs_pre_deinit( );
    }
    break;

  #if defined WINSIM
  case DRV_CMDID_DEINIT:
  #else
  case VS_CALLINDEX_ENUM_DEINIT:
  #endif
    {
      rc = vs_deinit( );
    }
    break;

  case VS_CALLINDEX_ENUM_RUN:
    {
      rc = vs_run_p1_thread( );
      rc = vs_run_p2_thread( );
      rc = vs_run_p3_thread( );
    }
    break;

  case VS_VOC_CMD_OPEN:
    {
      rc = vs_voc_cmd_open( ( vs_voc_cmd_open_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_OPEN cmd_id=0x%08x, rc=0x%08x",
             cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_CLOSE:
    {
      rc = vs_voc_cmd_close( ( vs_voc_cmd_close_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_CLOSE cmd_id=0x%08x, rc=0x%08x",
             cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_ENABLE:
    {
      rc = vs_voc_cmd_enable( ( vs_voc_cmd_enable_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_ENABLE cmd_id=0x%08x, rc=0x%08x",
             cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_DISABLE:
    {
      rc = vs_voc_cmd_disable( ( vs_voc_cmd_disable_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_DISABLE cmd_id=0x%08x, rc=0x%08x",
             cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_STANDBY:
    {
      rc = vs_voc_cmd_standby( ( vs_voc_cmd_standby_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_STANDBY cmd_id=0x%08x, rc=0x%08x",
             cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_ALLOC_BUFFER:
    {
      rc = vs_voc_cmd_alloc_buffer( ( vs_voc_cmd_alloc_buffer_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_ALLOC_BUFFER "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_FREE_BUFFER:
    {
      rc = vs_voc_cmd_free_buffer( ( vs_voc_cmd_free_buffer_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_FREE_BUFFER "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOC_CMD_PRIME_READ_BUFFER:
    {
      rc = vs_voc_cmd_prime_read_buffer(
               ( vs_voc_cmd_prime_read_buffer_t* ) params );
    }
    break;

  case VS_VOC_CMD_READ_BUFFER:
    {
      rc = vs_voc_cmd_read_buffer( ( vs_voc_cmd_read_buffer_t* ) params );
    }
    break;

  case VS_VOC_CMD_WRITE_BUFFER:
    {
      rc = vs_voc_cmd_write_buffer( ( vs_voc_cmd_write_buffer_t* ) params );
    }
    break;

  case VS_VOC_CMD_WRITE_BUFFER_THROUGH:
    {
    rc = vs_voc_cmd_write_buffer_through( ( vs_voc_cmd_write_buffer_t* ) params );
    }
    break;

  case VS_VOC_CMD_FLUSH_BUFFERS:
    {
      rc = vs_voc_cmd_flush_buffers( ( vs_voc_cmd_flush_buffers_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_call(): VS_VOC_CMD_FLUSH_BUFFERS "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCAMR_CMD_SET_CODEC_MODE:
    {
      rc = vs_vocamr_cmd_set_codec_mode(
             ( vs_vocamr_cmd_set_codec_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCAMR_CMD_SET_CODEC_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCAMR_CMD_GET_CODEC_MODE:
    {
      rc = vs_vocamr_cmd_get_codec_mode(
             ( vs_vocamr_cmd_get_codec_mode_t* ) params );
    }
    break;

  case VS_VOCAMR_CMD_SET_DTX_MODE:
    {
      rc = vs_vocamr_cmd_set_dtx_mode(
             ( vs_vocamr_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCAMR_CMD_SET_DTX_MODE "
             "md_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCAMR_CMD_GET_DTX_MODE:
    {
    rc = vs_vocamr_cmd_get_dtx_mode(
               ( vs_vocamr_cmd_get_dtx_mode_t* ) params );
    }
    break;

  case VS_VOCAMRWB_CMD_SET_CODEC_MODE:
    {
      rc = vs_vocamrwb_cmd_set_codec_mode(
             ( vs_vocamrwb_cmd_set_codec_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCAMRWB_CMD_SET_CODEC_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCAMRWB_CMD_GET_CODEC_MODE:
    {
      rc = vs_vocamrwb_cmd_get_codec_mode(
             ( vs_vocamrwb_cmd_get_codec_mode_t* ) params );
    }
    break;

  case VS_VOCAMRWB_CMD_SET_DTX_MODE:
    {
      rc = vs_vocamrwb_cmd_set_dtx_mode(
             ( vs_vocamrwb_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCAMRWB_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCAMRWB_CMD_GET_DTX_MODE:
    {
      rc = vs_vocamrwb_cmd_get_dtx_mode(
             ( vs_vocamrwb_cmd_get_dtx_mode_t* ) params );
    }
    break;

  case VS_VOCEFR_CMD_SET_DTX_MODE:
    {
      rc = vs_vocefr_cmd_set_dtx_mode(
             ( vs_vocefr_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEFR_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEFR_CMD_GET_DTX_MODE:
    {
      rc = vs_vocefr_cmd_get_dtx_mode(
             ( vs_vocefr_cmd_get_dtx_mode_t* ) params );
    }
    break;

  case VS_VOCFR_CMD_SET_DTX_MODE:
    {
      rc = vs_vocfr_cmd_set_dtx_mode(
             ( vs_vocfr_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCFR_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCFR_CMD_GET_DTX_MODE:
    {
      rc = vs_vocfr_cmd_get_dtx_mode(
             ( vs_vocfr_cmd_get_dtx_mode_t* ) params );
    }
    break;

  case VS_VOCHR_CMD_SET_DTX_MODE:
    {
      rc = vs_vochr_cmd_set_dtx_mode(
             ( vs_vochr_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCHR_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCHR_CMD_GET_DTX_MODE:
    {
      rc = vs_vochr_cmd_get_dtx_mode(
             ( vs_vochr_cmd_get_dtx_mode_t* ) params );
    }
    break;

  /* EVRC-A */
  case VS_VOCEVRCA_CMD_SET_DTX_MODE:
    {
      rc = vs_vocevrca_cmd_set_dtx_mode(
             ( vs_vocevrca_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCA_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCA_CMD_SET_FRAME_MINMAX_RATE:
    {
      rc = vs_vocevrca_cmd_set_frame_minmax_rate(
             ( vs_vocevrca_cmd_set_frame_minmax_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCA_CMD_SET_FRAME_MINMAX_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCA_CMD_SET_ENC_RATE_MODULATION:
    {
      rc = vs_vocevrca_cmd_set_enc_rate_mod(
             ( vs_vocevrca_cmd_set_enc_rate_mod_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCA_CMD_SET_ENC_RATE_MODULATION"
             " cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  /* EVRC-B */
  case VS_VOCEVRCB_CMD_SET_DTX_MODE:
    {
      rc = vs_vocevrcb_cmd_set_dtx_mode(
             ( vs_vocevrcb_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCB_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCB_CMD_SET_FRAME_MINMAX_RATE:
    {
      rc = vs_vocevrcb_cmd_set_frame_minmax_rate(
             ( vs_vocevrcb_cmd_set_frame_minmax_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCB_CMD_SET_FRAME_MINMAX_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCB_CMD_SET_AVG_ENC_RATE:
    {
      rc = vs_vocevrcb_cmd_set_avg_enc_rate(
             ( vs_vocevrcb_cmd_set_avg_enc_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCB_CMD_SET_AVG_ENC_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  /* EVRC-WB */
  case VS_VOCEVRCWB_CMD_SET_DTX_MODE:
    {
      rc = vs_vocevrcwb_cmd_set_dtx_mode(
             ( vs_vocevrcwb_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCWB_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCWB_CMD_SET_FRAME_MINMAX_RATE:
    {
      rc = vs_vocevrcwb_cmd_set_frame_minmax_rate(
             ( vs_vocevrcwb_cmd_set_frame_minmax_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCWB_CMD_SET_FRAME_MINMAX_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCWB_CMD_SET_AVG_ENC_RATE:
    {
      rc = vs_vocevrcwb_cmd_set_avg_enc_rate(
             ( vs_vocevrcwb_cmd_set_avg_enc_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCWB_CMD_SET_AVG_ENC_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  /* EVRC-NW */
  case VS_VOCEVRCNW_CMD_SET_DTX_MODE:
    {
      rc = vs_vocevrcnw_cmd_set_dtx_mode(
             ( vs_vocevrcnw_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCNW_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCNW_CMD_SET_FRAME_MINMAX_RATE:
    {
      rc = vs_vocevrcnw_cmd_set_frame_minmax_rate(
             ( vs_vocevrcnw_cmd_set_frame_minmax_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCNW_CMD_SET_FRAME_MINMAX_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCNW_CMD_SET_AVG_ENC_RATE:
    {
      rc = vs_vocevrcnw_cmd_set_avg_enc_rate(
             ( vs_vocevrcnw_cmd_set_avg_enc_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCNW_CMD_SET_AVG_ENC_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  /* EVRC-NW2K */
  case VS_VOCEVRCNW2K_CMD_SET_DTX_MODE:
    {
      rc = vs_vocevrcnw2k_cmd_set_dtx_mode(
             ( vs_vocevrcnw2k_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCNW2K_CMD_SET_DTX_MODE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCNW2K_CMD_SET_FRAME_MINMAX_RATE:
    {
      rc = vs_vocevrcnw2k_cmd_set_frame_minmax_rate(
             ( vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCNW2K_CMD_SET_FRAME_MINMAX_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCEVRCNW2K_CMD_SET_AVG_ENC_RATE:
    {
      rc = vs_vocevrcnw2k_cmd_set_avg_enc_rate(
             ( vs_vocevrcnw2k_cmd_set_avg_enc_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCEVRCNW2K_CMD_SET_AVG_ENC_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  /* QCELP-13K */
  case VS_VOCQCELP13K_CMD_SET_DTX_MODE:
    {
      rc = vs_vocqcelp13k_cmd_set_dtx_mode(
             ( vs_vocqcelp13k_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCQCELP13K_CMD_SET_DTX_MODE cmd_id=0x%08x, "
             "rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCQCELP13K_CMD_SET_FRAME_MINMAX_RATE:
    {
      rc = vs_vocqcelp13k_cmd_set_frame_minmax_rate(
             ( vs_vocqcelp13k_cmd_set_frame_minmax_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCQCELP13K_CMD_SET_FRAME_MINMAX_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCQCELP13K_CMD_SET_AVG_ENC_RATE:
    {
      rc = vs_vocqcelp13k_cmd_set_avg_enc_rate(
             ( vs_vocqcelp13k_cmd_set_avg_enc_rate_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCQCELP13K_CMD_SET_AVG_ENC_RATE "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  case VS_VOCQCELP13K_CMD_SET_ENC_RATE_MODULATION:
    {
      rc = vs_vocqcelp13k_cmd_set_enc_rate_mod(
             ( vs_vocqcelp13k_cmd_set_enc_rate_mod_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCQCELP13K_CMD_SET_ENC_RATE_MODULATION "
             "cmd_id=0x%08x, rc=0x%08x", cmd_id, rc );
    }
    break;

  /* G.729 */
  case VS_VOCG729_CMD_SET_DTX_MODE:
    {
      rc = vs_vocg729_cmd_set_dtx_mode(
             ( vs_vocg729_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCG729_CMD_SET_DTX_MODE cmd_id=0x%08x, "
             "rc=0x%08x", cmd_id, rc );
    }
    break;

  /* G.711 A-LAW */
  case VS_VOCG711ALAW_CMD_SET_DTX_MODE:
    {
      rc = vs_vocg711alaw_cmd_set_dtx_mode(
             ( vs_vocg711alaw_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCG711ALAW_CMD_SET_DTX_MODE cmd_id=0x%08x, "
             "rc=0x%08x", cmd_id, rc );
    }
    break;

  /* G.711 MU-LAW */
  case VS_VOCG711MULAW_CMD_SET_DTX_MODE:
    {
      rc = vs_vocg711mulaw_cmd_set_dtx_mode(
             ( vs_vocg711mulaw_cmd_set_dtx_mode_t* ) params );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_call(): VS_VOCG711MULAW_CMD_SET_DTX_MODE cmd_id=0x%08x, "
             "rc=0x%08x", cmd_id, rc );
    }
    break;

  default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Unsupported callindex (%x)", cmd_id );
      /* index vs cmd_id - TBD */
      rc = VS_EUNSUPPORTED;
    }
    break;
  }  /* end of switch ( cmd_id )*/

  return rc;
}  /* end of vs_call () */
