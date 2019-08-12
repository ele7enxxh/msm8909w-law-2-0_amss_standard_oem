/*
   Copyright (C) 2014-2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/vadapter/src/cva_module.c#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* CORE APIs. */
#include <stddef.h>
#include <string.h>
#include "err.h"
#include "msg.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_memmgr.h"

/* 1x APIs. */
#include "onex_voice_if.h"

/* CM APIs. */
#include "cm.h"
#include "cm_dualsim.h"

/* VSD APIs. */
#include "drv_api.h"
#include "vs_task.h"

#include "vs.h"

/* SELF APIs. */
#include "cva_if.h"
#include "cva_i.h"
#include "cvainit.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define CVA_MAX_NUM_OF_SESSIONS_V ( 2 )
#define CVA_NUM_OF_SUPPORTED_VSID_V ( 2 )


/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/


/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

static apr_lock_t cva_int_lock;
static apr_lock_t cva_thread_lock;
static apr_event_t cva_control_event;


static apr_memmgr_type cva_heapmgr;
static uint8_t cva_heap_pool[ CVA_HEAP_SIZE_V ];

static apr_objmgr_t cva_objmgr;
static apr_objmgr_object_t cva_object_table[ CVA_MAX_OBJECTS_V ];

static cva_gating_control_t cva_gating_work_pkt_q;
static apr_list_t cva_nongating_work_pkt_q;
static apr_list_t cva_free_work_pkt_q;
static cva_work_item_t cva_work_pkts[ CVA_NUM_WORK_PKTS_V ];

static apr_event_t cva_work_event;
static apr_thread_t cva_thread;
static uint8_t cva_task_stack[ CVA_TASK_STACK_SIZE ];

static cva_session_object_t *cva_session_list[ CVA_MAX_NUM_OF_SESSIONS_V ];
static apr_lock_t cva_session_lock_list[ CVA_MAX_NUM_OF_SESSIONS_V ];

static bool_t cva_is_initialized = FALSE; 

static bool_t cva_is_va_supported = FALSE; 

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void cva_int_lock_fn ( void )
{
  apr_lock_enter( cva_int_lock );
}

static void cva_int_unlock_fn ( void )
{
  apr_lock_leave( cva_int_lock );
}

static void cva_thread_lock_fn ( void )
{
  apr_lock_enter( cva_thread_lock );
}

static void cva_thread_unlock_fn ( void )
{
  apr_lock_leave( cva_thread_lock );
}


/****************************************************************************
 * CVA CMDs & EVENTs PACKET QUEUING FUNCTIONS                               *
 ****************************************************************************/

/**
 * Queues the cva_cmd_packet_t and cva_event_packet_t. In
 * case of failure to queue a apr packet, packet shall be
 * freed by the caller.
 */
CVA_INTERNAL uint32_t cva_queue_work_packet (
  cva_work_item_queue_type_t queue_type,
  cva_work_item_packet_type_t pkt_type,
  void* packet
)
{
  uint32_t rc = APR_EOK;
  cva_work_item_t* work_item = NULL;
  apr_list_t* work_queue = NULL;

  switch ( queue_type )
  {
   case CVA_WORK_ITEM_QUEUE_TYPE_NONGATING:
     work_queue = &cva_nongating_work_pkt_q;
     break;

   case CVA_WORK_ITEM_QUEUE_TYPE_GATING:
     work_queue = &cva_gating_work_pkt_q.cmd_q;
     break;

   default:
     rc = APR_EUNSUPPORTED;
     break;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &cva_free_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) );
    if ( rc )
    {
      rc = APR_ENORESOURCE;
      /* No free WORK packet structure is available. */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
             "cva_queue_work_packet(): Ran out of WORK packets, rc=0x%08x, "
             "cva_state=%d",   rc, cva_is_initialized );
      break;
    }

    if ( pkt_type == CVA_WORK_ITEM_PKT_TYPE_CMD )
    {
      work_item->pkt_type = CVA_WORK_ITEM_PKT_TYPE_CMD;
    }
    else if ( pkt_type == CVA_WORK_ITEM_PKT_TYPE_EVENT )
    {
     work_item->pkt_type = CVA_WORK_ITEM_PKT_TYPE_EVENT;
    }
    else
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
           "cva_queue_work_packet(): Invalid packet type!!!" );
      CVA_PANIC_ON_ERROR ( APR_ENOTEXIST );
    }

    work_item->packet = packet;


    /* Add to incoming request work queue */
    rc = apr_list_add_tail( work_queue, &work_item->link );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_queue_work_packet() - ERROR: rc=0x%08x", rc );
      /* Add back to cva_free_work_pkt_q */
      work_item->pkt_type = CVA_WORK_ITEM_PKT_TYPE_NONE;
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &cva_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      cva_signal_run();
    }

    break;
  } /* for loop ends. */

  return rc;
}  /* cva_queue_work_packet() ends. */


/****************************************************************************
 * CVA CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                  *
 ****************************************************************************/

CVA_INTERNAL uint32_t cva_free_cmd_packet (
  cva_cmd_packet_t* packet
)
{
  uint32_t rc = VS_EOK;

  if ( packet != NULL )
  {
    if( packet->params != NULL )
    {
      /* Free the memory - p_cmd_packet->params. */
      apr_memmgr_free( &cva_heapmgr, packet->params );
      packet->params = NULL;
    }

    /* Free the memeory - p_cmd_packet. */
    apr_memmgr_free( &cva_heapmgr, packet );
    packet = NULL;
  }

  return rc;
}

/**
 * This is a common routine facilitating to prepare and
 * dispatches a CMD PKT.
 */
CVA_INTERNAL uint32_t cva_prepare_and_dispatch_cmd_packet (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;
  cva_cmd_packet_t* packet = NULL;

  for ( ;; )
  {
    packet = ( ( cva_cmd_packet_t* ) apr_memmgr_malloc( &cva_heapmgr,
                                           sizeof( cva_cmd_packet_t ) ) );
    if ( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR( APR_ENORESOURCE );
      rc = APR_ENORESOURCE;
      break;
    }

    packet->cmd_id = cmd_id;
    packet->params = NULL;

    if ( ( size > 0 ) && ( params != NULL ) )
    {
      packet->params = apr_memmgr_malloc(  &cva_heapmgr, size );

      if ( packet->params == NULL )
      {
        rc = APR_ENORESOURCE;
        CVA_REPORT_FATAL_ON_ERROR( rc );
        ( void ) cva_free_cmd_packet( packet );
        break;
      }
      mmstd_memcpy( packet->params, size, params, size );
    }

    /* Queue the command packet for processing. */
    rc = cva_queue_work_packet( CVA_WORK_ITEM_QUEUE_TYPE_NONGATING,
                                CVA_WORK_ITEM_PKT_TYPE_CMD, ( void*) packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_prepare_and_dispatch_cmd_packet() - cmd pkt queuing failed. "
             "rc=(0x%08x)", rc );
      ( void ) cva_free_cmd_packet( packet );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "cva_prepare_and_dispatch_cmd_packet() cmd pkt queued with "
             "cmd_id=(0x%08x)", cmd_id );
    }

    break;
  }

  return rc;
}


CVA_INTERNAL uint32_t cva_free_event_packet (
  cva_event_packet_t* packet
)
{
  uint32_t rc = VS_EOK;

  if ( packet != NULL )
  {
    if( packet->params != NULL )
    {
      /* Free the memory - p_cmd_packet->params. */
      apr_memmgr_free( &cva_heapmgr, packet->params );
      packet->params = NULL;
    }

    /* Free the memeory - p_cmd_packet. */
    apr_memmgr_free( &cva_heapmgr, packet );
    packet= NULL;
  }

  return rc;
}

/**
 * This is a common routine facilitating to prepare and
 * dispatches a CMD PKT.
 */
CVA_INTERNAL uint32_t cva_prepare_and_dispatch_event_packet (
  void* session_context,
  uint32_t event_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* packet = NULL;

  for ( ;; )
  {
    packet = ( ( cva_event_packet_t* ) apr_memmgr_malloc( &cva_heapmgr,
                                         sizeof( cva_event_packet_t ) ) );
    if ( packet == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR( APR_ENORESOURCE );
      rc = APR_ENORESOURCE;
      break;
    }

    packet->session_context = session_context;
    packet->event_id = event_id;
    packet->params = NULL;

    if ( ( size > 0 ) && ( params != NULL ) )
    {
      packet->params = apr_memmgr_malloc(  &cva_heapmgr, size );

      if ( packet->params == NULL )
      {
        rc = APR_ENORESOURCE;
        CVA_REPORT_FATAL_ON_ERROR( rc );
        ( void ) cva_free_event_packet( packet );
        break;
      }
      mmstd_memcpy( packet->params, size, params, size );
    }

    /* Queue the command packet for processing. */
    rc = cva_queue_work_packet( CVA_WORK_ITEM_QUEUE_TYPE_NONGATING,
                                CVA_WORK_ITEM_PKT_TYPE_EVENT, ( void*) packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_prepare_and_dispatch_event_packet()-event pkt queuing failed "
             "rc=(0x%08x)", rc );
      ( void ) cva_free_event_packet( packet );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "cva_prepare_and_dispatch_event_packet()-event pkt queued with "
             "event_id=(0x%08x)", event_id );
    }

    break;
  }

  return rc;
}


/****************************************************************************
 * CVA OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES             *
 ****************************************************************************/

static uint32_t cva_mem_alloc_object (
  uint32_t size,
  cva_object_t** ret_object
)
{
  int32_t rc;
  cva_object_t* cva_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the CVA object. */
    cva_obj = apr_memmgr_malloc( &cva_heapmgr, size );
    if ( cva_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_mem_alloc_object(): Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the MVS object. */
    rc = apr_objmgr_alloc_object( &cva_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "cva_mem_alloc_object(): Out of objects, rc = (0x%08X)", rc );
      apr_memmgr_free( &cva_heapmgr, cva_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = cva_obj;

    /* Initialize the base MVS object header. */
    cva_obj->header.handle = objmgr_obj->handle;
    cva_obj->header.type = CVA_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = cva_obj;

  return APR_EOK;
}

static uint32_t cva_mem_free_object (
  cva_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &cva_objmgr, object->header.handle );
  apr_memmgr_free( &cva_heapmgr, object );

  return APR_EOK;
}


static uint32_t cva_create_session_object ( 
  cva_session_object_t** ret_session_obj )
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cva_mem_alloc_object( sizeof( cva_session_object_t ),
                             ( ( cva_object_t** ) &session_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    session_obj->header.type = CVA_OBJECT_TYPE_ENUM_SESSION;

    session_obj->asid = SYS_MODEM_AS_ID_NONE;
    session_obj->vsid = CVA_VSID_UNDEFINED_V;

    session_obj->va_cva_event_cb = NULL;
    session_obj->va_session_context =  NULL;
    session_obj->is_resources_granted = FALSE;

    session_obj->onex_handle = APR_NULL_V;
    session_obj->vocoder_type = CVA_VOCODER_ID_UNDEFINED_V;
    session_obj->avg_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
    session_obj->min_rate = CVA_MIN_MAX_RATE_UNDEFINED;
    session_obj->max_rate = CVA_MIN_MAX_RATE_UNDEFINED;
    session_obj->dtx_mode = FALSE;
    session_obj->is_onex_ready = FALSE;
  
    session_obj->vs_handle = APR_NULL_V;
    session_obj->vs_read_buf = NULL;
    session_obj->vs_write_buf = NULL;
    session_obj->is_vs_ready = FALSE;
  }


  *ret_session_obj = session_obj;

  return APR_EOK;
}

static uint32_t cva_create_simple_job_object (
  uint32_t parentjob_handle,
  cva_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  cva_simple_job_object_t* cva_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cva_mem_alloc_object( sizeof( cva_simple_job_object_t ),
                             ( ( cva_object_t** ) &cva_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    cva_obj->header.type = CVA_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    cva_obj->context_handle = parentjob_handle;
    cva_obj->is_completed = 0;
  }

  *ret_job_obj = cva_obj;

  return APR_EOK;
}

/* Disabling the Sequencer job create API as its currenty being unused.
 * One can enable it, if required in future.
 */
#if 0
static int32_t cva_create_sequencer_job_object (
  cva_sequencer_job_object_t** ret_job_obj
)
{
  int32_t rc;
  cva_sequencer_job_object_t* job_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cva_mem_alloc_object( sizeof( cva_sequencer_job_object_t ),
                             ( ( cva_object_t** ) &job_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the pending job object. */
    job_obj->header.type = CVA_OBJECT_TYPE_ENUM_SEQUENCER_JOB;

    job_obj->state = APR_NULL_V;
    job_obj->subjob_obj = NULL;
    job_obj->status = APR_UNDEFINED_ID_V;
  }

  *ret_job_obj = job_obj;

  return APR_EOK;
}
#endif

/****************************************************************************
 * CVA ONEX <> VS MAPPING  ROUTINES                                          *
 ****************************************************************************/

static uint32_t cva_map_vocevrca_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   uint32_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = 0x0000;
       break;

     case ONEX_VOC_COP_1:
       *vs_enc_rate = 0x000F;
       break;

     case ONEX_VOC_COP_2:
       *vs_enc_rate = 0x0007;
       break;

     case ONEX_VOC_COP_3:
       *vs_enc_rate = 0x0005;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = 0x0000;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }
  return rc;
}

static uint32_t cva_map_vocevrcb_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   vs_vocevrcb_avg_enc_rate_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_9_3_KBPS;
       break;

     case ONEX_VOC_COP_1:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_8_5_KBPS;
       break;

     case ONEX_VOC_COP_2:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_7_5_KBPS;
       break;

     case ONEX_VOC_COP_3:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_7_0_KBPS;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_6_6_KBPS;
       break;

     case ONEX_VOC_COP_5:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_6_2_KBPS;
       break;

     case ONEX_VOC_COP_6:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_5_8_KBPS;
       break;

     case ONEX_VOC_COP_7:
       *vs_enc_rate = VS_VOCEVRCB_AVG_ENC_RATE_4_8_KBPS;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }
  return rc;
}

static uint32_t cva_map_vocevrcwb_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   vs_vocevrcwb_avg_enc_rate_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = VS_VOCEVRCWB_AVG_ENC_RATE_8_5_KBPS;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = VS_VOCEVRCWB_AVG_ENC_RATE_9_3_KBPS;
       break;

     case ONEX_VOC_COP_7:
       *vs_enc_rate = VS_VOCEVRCWB_AVG_ENC_RATE_4_8_KBPS;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }
  return rc;
}

static uint32_t cva_map_vocevrcnw_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   vs_vocevrcnw_avg_enc_rate_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_8_5_KBPS;
       break;

     case ONEX_VOC_COP_1:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_9_3_KBPS;
       break;

     case ONEX_VOC_COP_2:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_7_5_KBPS;
       break;

     case ONEX_VOC_COP_3:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_7_0_KBPS;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_6_6_KBPS;
       break;

     case ONEX_VOC_COP_5:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_6_2_KBPS;
       break;

     case ONEX_VOC_COP_6:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_5_8_KBPS;
       break;

     case ONEX_VOC_COP_7:
       *vs_enc_rate = VS_VOCEVRCNW_AVG_ENC_RATE_4_8_KBPS;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }
  return rc;
}

static uint32_t cva_map_vocevrcnw2k_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   vs_vocevrcnw2k_avg_enc_rate_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_8_5_KBPS;
       break;

     case ONEX_VOC_COP_1:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_9_3_KBPS;
       break;

     case ONEX_VOC_COP_2:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_7_5_KBPS;
       break;

     case ONEX_VOC_COP_3:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_2_4_KBPS;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_6_6_KBPS;
       break;

     case ONEX_VOC_COP_5:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_6_2_KBPS;
       break;

     case ONEX_VOC_COP_6:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_5_8_KBPS;
       break;

     case ONEX_VOC_COP_7:
       *vs_enc_rate = VS_VOCEVRCNW2K_AVG_ENC_RATE_4_8_KBPS;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }
  return rc;
}

static uint32_t cva_map_vocqcelp13k_enc_rate_onex_to_vs(
   uint32_t onex_encrate,
   vs_vocqcelp13k_avg_enc_rate_t* vs_enc_rate
)
{
  uint32_t rc = APR_EOK;
  switch (onex_encrate )
  {
     case ONEX_VOC_COP_0:
       *vs_enc_rate = VS_VOCQCELP13K_AVG_ENC_RATE_14_4_KBPS;
       break;

     case ONEX_VOC_COP_1:
       *vs_enc_rate = VS_VOCQCELP13K_AVG_ENC_RATE_12_2_KBPS;
       break;

     case ONEX_VOC_COP_2:
       *vs_enc_rate = VS_VOCQCELP13K_AVG_ENC_RATE_11_2_KBPS;
       break;

     case ONEX_VOC_COP_3:
       *vs_enc_rate = VS_VOCQCELP13K_AVG_ENC_RATE_9_0_KBPS;
       break;

     case ONEX_VOC_COP_4:
       *vs_enc_rate = VS_VOCQCELP13K_AVG_ENC_RATE_7_2_KBPS;
       break;

     default:
       rc = APR_EBADPARAM;
       *vs_enc_rate = CVA_AVG_ENC_RATE_UNDEFINED;
       break;
  }
  return rc;
}

/*------------------------------------------------------------------------------------------------------*/
static uint32_t cva_map_vocevrca_frame_rate_vs_to_onex(
  uint32_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case VS_VOCEVRCA_FRAME_BLANK_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case VS_VOCEVRCA_FRAME_EIGHTH_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case VS_VOCEVRCA_FRAME_HALF_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;
  
   case VS_VOCEVRCA_FRAME_FULL_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case VS_VOCEVRCA_FRAME_ERASURE_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;
  
   default:
     rc = APR_EBADPARAM;
	 *cva_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcb_frame_rate_vs_to_onex(
  uint32_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case VS_VOCEVRCB_FRAME_BLANK_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case VS_VOCEVRCB_FRAME_EIGHTH_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case VS_VOCEVRCB_FRAME_QUARTER_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_QUARTER;
     break;

   case VS_VOCEVRCB_FRAME_HALF_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;
  
   case VS_VOCEVRCB_FRAME_FULL_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case VS_VOCEVRCB_FRAME_ERASURE_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;
  
   default:
     rc = APR_EBADPARAM;
	 *cva_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcwb_frame_rate_vs_to_onex(
  uint32_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case VS_VOCEVRCWB_FRAME_BLANK_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case VS_VOCEVRCWB_FRAME_EIGHTH_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case VS_VOCEVRCWB_FRAME_QUARTER_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_QUARTER;
     break;

   case VS_VOCEVRCWB_FRAME_HALF_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;
  
   case VS_VOCEVRCWB_FRAME_FULL_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case VS_VOCEVRCWB_FRAME_ERASURE_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;
  
   default:
     rc = APR_EBADPARAM;
	 *cva_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcnw_frame_rate_vs_to_onex(
  uint32_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case VS_VOCEVRCNW_FRAME_BLANK_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case VS_VOCEVRCNW_FRAME_EIGHTH_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case VS_VOCEVRCNW_FRAME_QUARTER_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_QUARTER;
     break;

   case VS_VOCEVRCNW_FRAME_HALF_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;

   case VS_VOCEVRCNW_FRAME_FULL_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case VS_VOCEVRCNW_FRAME_ERASURE_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;

   case VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL;
     break;
  
   default:
     rc = APR_EBADPARAM;
	 *cva_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcnw2k_frame_rate_vs_to_onex(
  uint32_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case VS_VOCEVRCNW2K_FRAME_BLANK_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case VS_VOCEVRCNW2K_FRAME_QUARTER_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_QUARTER;
     break;

   case VS_VOCEVRCNW2K_FRAME_HALF_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;
  
   case VS_VOCEVRCNW2K_FRAME_FULL_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case VS_VOCEVRCNW2K_FRAME_ERASURE_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;

   case VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL;
     break;
  
   default:
     rc = APR_EBADPARAM;
	 *cva_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocqcelp13k_frame_rate_vs_to_onex(
  uint32_t vs_frame_rate,
  uint32_t* cva_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( vs_frame_rate )
  {
   case VS_VOCQCELP13K_FRAME_BLANK_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_BLANK;
     break;
  
   case VS_VOCQCELP13K_FRAME_EIGHTH_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_EIGHTH;
     break;
  
   case VS_VOCQCELP13K_FRAME_QUARTER_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_QUARTER;
     break;

   case VS_VOCQCELP13K_FRAME_HALF_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_HALF;
     break;
  
   case VS_VOCQCELP13K_FRAME_FULL_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_FULL;
     break;
  
   case VS_VOCQCELP13K_FRAME_ERASURE_RATE:
     *cva_frame_rate = ONEX_VOC_FRAME_RATE_ERASURE;
     break;
  
   default:
     rc = APR_EBADPARAM;
	 *cva_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrca_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  vs_vocevrca_frame_rate_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = VS_VOCEVRCA_FRAME_BLANK_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = VS_VOCEVRCA_FRAME_EIGHTH_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = VS_VOCEVRCA_FRAME_HALF_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = VS_VOCEVRCA_FRAME_FULL_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = VS_VOCEVRCA_FRAME_ERASURE_RATE;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcb_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  vs_vocevrcb_frame_rate_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = VS_VOCEVRCB_FRAME_BLANK_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = VS_VOCEVRCB_FRAME_EIGHTH_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_QUARTER:
     *vs_frame_rate = VS_VOCEVRCB_FRAME_QUARTER_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = VS_VOCEVRCB_FRAME_HALF_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = VS_VOCEVRCB_FRAME_FULL_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = VS_VOCEVRCB_FRAME_ERASURE_RATE;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcwb_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  vs_vocevrcwb_frame_rate_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = VS_VOCEVRCWB_FRAME_BLANK_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = VS_VOCEVRCWB_FRAME_EIGHTH_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_QUARTER:
     *vs_frame_rate = VS_VOCEVRCWB_FRAME_QUARTER_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = VS_VOCEVRCWB_FRAME_HALF_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = VS_VOCEVRCWB_FRAME_FULL_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = VS_VOCEVRCWB_FRAME_ERASURE_RATE;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcnw_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  vs_vocevrcnw_frame_rate_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_BLANK_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_EIGHTH_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_QUARTER:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_QUARTER_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_HALF_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_FULL_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_ERASURE_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL:
     *vs_frame_rate = VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocevrcnw2k_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  vs_vocevrcnw2k_frame_rate_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_BLANK_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_QUARTER:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_QUARTER_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_HALF_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_FULL_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_ERASURE_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL:
     *vs_frame_rate = VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocqcelp13k_frame_rate_onex_to_vs(
  uint32_t cva_frame_rate,
  vs_vocqcelp13k_frame_rate_t* vs_frame_rate
)
{
  uint32_t rc = APR_EOK;

  switch( cva_frame_rate )
  {
   case ONEX_VOC_FRAME_RATE_BLANK:
     *vs_frame_rate = VS_VOCQCELP13K_FRAME_BLANK_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_EIGHTH:
     *vs_frame_rate = VS_VOCQCELP13K_FRAME_EIGHTH_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_QUARTER:
     *vs_frame_rate = VS_VOCQCELP13K_FRAME_QUARTER_RATE;
     break;

   case ONEX_VOC_FRAME_RATE_HALF:
     *vs_frame_rate = VS_VOCQCELP13K_FRAME_HALF_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_FULL:
     *vs_frame_rate = VS_VOCQCELP13K_FRAME_FULL_RATE;
     break;
  
   case ONEX_VOC_FRAME_RATE_ERASURE:
     *vs_frame_rate = VS_VOCQCELP13K_FRAME_ERASURE_RATE;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_frame_rate = CVA_FRAME_RATE_UNDEFINED;
     break;
  }

  return rc;
}

static uint32_t cva_map_vocoder_type_onex_to_vs(
  uint32_t onex_vocoder_type
)
{
  uint32_t vs_media_id = CVA_VOCODER_ID_UNDEFINED_V;

  switch ( onex_vocoder_type )
  {
   case ONEX_VOC_ID_EVRCA:
     vs_media_id = VS_COMMON_MEDIA_ID_EVRCA;
     break;

   case ONEX_VOC_ID_EVRCB:
     vs_media_id = VS_COMMON_MEDIA_ID_EVRCB;
     break;

   case ONEX_VOC_ID_EVRCNW:
    vs_media_id = VS_COMMON_MEDIA_ID_EVRCNW;
    break;

   case ONEX_VOC_ID_EVRCNW2K:
    vs_media_id = VS_COMMON_MEDIA_ID_EVRCNW2K;
    break;

   case ONEX_VOC_ID_EVRCWB:
     vs_media_id = VS_COMMON_MEDIA_ID_EVRCWB;
     break;

   case ONEX_VOC_ID_QCELP13K:
     vs_media_id = VS_COMMON_MEDIA_ID_QCELP13K;
     break;

   default:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_map_vocoder_type_onex_to_vs(): vocoder type (0x%08x) unsupported by CVA", onex_vocoder_type );
        break;
  }

  return vs_media_id;
}

/****************************************************************************
 * CVA COMMON ROUTINES                                                      *
 ****************************************************************************/

static uint32_t cva_set_voc_min_max_rate (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK, rc1 = APR_EOK, rc2 = APR_EOK;
  vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t vocevrcnw2k_minmax_rate;
  vs_vocevrcnw_cmd_set_frame_minmax_rate_t vocevrcnw_minmax_rate;
  vs_vocevrca_cmd_set_frame_minmax_rate_t vocevrca_minmax_rate;
  vs_vocevrcb_cmd_set_frame_minmax_rate_t vocevrcb_minmax_rate;
  vs_vocevrcwb_cmd_set_frame_minmax_rate_t vocevrcwb_minmax_rate;
  vs_vocqcelp13k_cmd_set_frame_minmax_rate_t vocqcelp13k_minmax_rate;

  switch ( session_obj->vocoder_type )
  {
     case ONEX_VOC_ID_EVRCA:
       rc1 = cva_map_vocevrca_frame_rate_onex_to_vs(session_obj->min_rate, &vocevrca_minmax_rate.min_rate );
       rc2 = cva_map_vocevrca_frame_rate_onex_to_vs(session_obj->max_rate, &vocevrca_minmax_rate.max_rate );
       if ( !( rc1 | rc2) )
       {
         vocevrca_minmax_rate.handle  = session_obj->vs_handle;
         rc  = vs_call( VS_VOCEVRCA_CMD_SET_FRAME_MINMAX_RATE, ( void* )&vocevrca_minmax_rate,
                        sizeof( vocevrca_minmax_rate ) );
       }
       break;

    case ONEX_VOC_ID_EVRCB:
      rc1 = cva_map_vocevrcb_frame_rate_onex_to_vs(session_obj->min_rate, &vocevrcb_minmax_rate.min_rate );
      rc2 = cva_map_vocevrcb_frame_rate_onex_to_vs(session_obj->max_rate, &vocevrcb_minmax_rate.max_rate);
      if ( !( rc1 | rc2) )
      {
        vocevrcb_minmax_rate.handle  = session_obj->vs_handle;
        rc  = vs_call( VS_VOCEVRCB_CMD_SET_FRAME_MINMAX_RATE, ( void* )&vocevrcb_minmax_rate,
                      sizeof( vocevrcb_minmax_rate ) );
      }
      break;

   case ONEX_VOC_ID_EVRCNW:
     rc1 =cva_map_vocevrcnw_frame_rate_onex_to_vs(session_obj->min_rate, &vocevrcnw_minmax_rate.min_rate );
     rc2 =cva_map_vocevrcnw_frame_rate_onex_to_vs(session_obj->max_rate, &vocevrcnw_minmax_rate.max_rate );
     if ( !( rc1 | rc2) )
     {
       vocevrcnw_minmax_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCNW_CMD_SET_FRAME_MINMAX_RATE, ( void* )&vocevrcnw_minmax_rate,
                      sizeof( vocevrcnw_minmax_rate ) );
     }
     break;

   case ONEX_VOC_ID_EVRCWB:
     rc1 =cva_map_vocevrcwb_frame_rate_onex_to_vs(session_obj->min_rate, &vocevrcwb_minmax_rate.min_rate );
     rc2 =cva_map_vocevrcwb_frame_rate_onex_to_vs(session_obj->max_rate, &vocevrcwb_minmax_rate.max_rate );
     if ( !( rc1 | rc2) )
     {
       vocevrcwb_minmax_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCWB_CMD_SET_FRAME_MINMAX_RATE, ( void* )&vocevrcwb_minmax_rate,
                      sizeof( vocevrcwb_minmax_rate ) );
     }
     break;

   case ONEX_VOC_ID_EVRCNW2K:
     rc1 =cva_map_vocevrcnw2k_frame_rate_onex_to_vs(session_obj->min_rate, &vocevrcnw2k_minmax_rate.min_rate );
     rc2 =cva_map_vocevrcnw2k_frame_rate_onex_to_vs(session_obj->max_rate, &vocevrcnw2k_minmax_rate.max_rate );
     if ( !( rc1 | rc2) )
     {
       vocevrcnw2k_minmax_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCNW2K_CMD_SET_FRAME_MINMAX_RATE, ( void* )&vocevrcnw2k_minmax_rate,
                      sizeof( vocevrcnw2k_minmax_rate ) );
     }
     break;

   case ONEX_VOC_ID_QCELP13K:
     rc1 =cva_map_vocqcelp13k_frame_rate_onex_to_vs(session_obj->min_rate, &vocqcelp13k_minmax_rate.min_rate );
     rc2 =cva_map_vocqcelp13k_frame_rate_onex_to_vs(session_obj->max_rate, &vocqcelp13k_minmax_rate.max_rate );
     if ( !( rc1 | rc2) )
     {
       vocqcelp13k_minmax_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCQCELP13K_CMD_SET_FRAME_MINMAX_RATE, ( void* )&vocqcelp13k_minmax_rate,
                      sizeof( vocqcelp13k_minmax_rate ) );
     }
     break;

     default:
       rc1 = APR_EBADPARAM;
       rc2 = APR_EBADPARAM;
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_set_voc_min_max_rate(): Unsupported vocoder type = (0x%08x)",
           session_obj->vocoder_type );
     break;
  }

  if ( !( rc | rc1 | rc2 ) )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "cva_set_voc_min_max_rate(): vocoder_type = (0x%08x), min_rate = (0x%08x), max_rate = (0x%08x)"
           , session_obj->vocoder_type, session_obj->min_rate, session_obj->max_rate );
  }
  else
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_set_voc_min_max_rate(): Failed vocoder_type = (0x%08x), min_rate = (0x%08x), max_rate = (0x%08x)"
           , session_obj->vocoder_type, session_obj->min_rate, session_obj->max_rate );
  }

  return APR_EOK;
}


static uint32_t cva_set_voc_enc_rate (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  
  vs_vocevrcnw2k_cmd_set_avg_enc_rate_t vocevrcnw2k_enc_rate;
  vs_vocevrcnw_cmd_set_avg_enc_rate_t vocevrcnw_enc_rate;
  vs_vocevrca_cmd_set_enc_rate_mod_t vocevrca_enc_rate;
  vs_vocevrcb_cmd_set_avg_enc_rate_t vocevrcb_enc_rate;
  vs_vocevrcwb_cmd_set_avg_enc_rate_t vocevrcwb_enc_rate;
  vs_vocqcelp13k_cmd_set_avg_enc_rate_t vocqcelp13k_enc_rate;

  switch ( session_obj->vocoder_type )
  {
   case ONEX_VOC_ID_EVRCA:
     rc = cva_map_vocevrca_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &vocevrca_enc_rate.rate_modulation  );
     if ( !rc )
     {
       vocevrca_enc_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCA_CMD_SET_ENC_RATE_MODULATION, ( void* )&vocevrca_enc_rate,
                      sizeof( vocevrca_enc_rate ) );
     }
     break;

   case ONEX_VOC_ID_EVRCB:
     rc = cva_map_vocevrcb_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &vocevrcb_enc_rate.rate  );
     if ( !rc )
     {
       vocevrcb_enc_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCB_CMD_SET_AVG_ENC_RATE, ( void* )&vocevrcb_enc_rate,
                      sizeof( vocevrcb_enc_rate ) );
     }
     break;

   case ONEX_VOC_ID_EVRCNW:
     rc = cva_map_vocevrcnw_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &vocevrcnw_enc_rate.rate  );
     if ( !rc )
     {
       vocevrcnw_enc_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCNW_CMD_SET_AVG_ENC_RATE, ( void* )&vocevrcnw_enc_rate,
                      sizeof( vocevrcnw_enc_rate ) );
     }
     break;

   case ONEX_VOC_ID_EVRCWB:
     rc = cva_map_vocevrcwb_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &vocevrcwb_enc_rate.rate  );
     if ( !rc )
     {
       vocevrcwb_enc_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCWB_CMD_SET_AVG_ENC_RATE, ( void* )&vocevrcwb_enc_rate,
                      sizeof( vocevrcwb_enc_rate ) );
     }
     break;

   case ONEX_VOC_ID_EVRCNW2K:
     rc = cva_map_vocevrcnw2k_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &vocevrcnw2k_enc_rate.rate  );
     if ( !rc )
     {
       vocevrcnw2k_enc_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCEVRCNW2K_CMD_SET_AVG_ENC_RATE, ( void* )&vocevrcnw2k_enc_rate,
                      sizeof( vocevrcnw2k_enc_rate ) );
     }
     break;

   case ONEX_VOC_ID_QCELP13K:
     rc = cva_map_vocqcelp13k_enc_rate_onex_to_vs( session_obj->avg_enc_rate, &vocqcelp13k_enc_rate.rate  );
     if ( !rc )
     {
       vocqcelp13k_enc_rate.handle  = session_obj->vs_handle;
       rc  = vs_call( VS_VOCQCELP13K_CMD_SET_AVG_ENC_RATE, ( void* )&vocqcelp13k_enc_rate,
                      sizeof( vocqcelp13k_enc_rate ) );
     }
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_set_voc_enc_rate(): Unsupported vocoder type = (0x%08x)",
           session_obj->vocoder_type );
     break;
  }

  if ( rc == APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "cva_set_voc_enc_rate(): vocoder_type = (0x%08x), enc_rate = (0x%08x)"
           , session_obj->vocoder_type, session_obj->avg_enc_rate );
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_set_voc_enc_rate(): Failed vocoder_type = (0x%08x), enc_rate = (0x%08x)"
           , session_obj->vocoder_type, session_obj->avg_enc_rate );
  }

  return APR_EOK;
}

static uint32_t cva_set_voc_dtx_mode (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_vocevrcnw2k_cmd_set_dtx_mode_t vocevrcnw2k_dtx_cmd;
  vs_vocevrcnw_cmd_set_dtx_mode_t vocevrcnw_dtx_cmd;
  vs_vocevrca_cmd_set_dtx_mode_t vocevrca_dtx_cmd;
  vs_vocevrcb_cmd_set_dtx_mode_t vocevrcb_dtx_cmd;
  vs_vocevrcwb_cmd_set_dtx_mode_t vocevrcwb_dtx_cmd;
  vs_vocqcelp13k_cmd_set_dtx_mode_t vocqcelp13k_dtx_cmd;

  switch ( session_obj->vocoder_type )
  {
   case ONEX_VOC_ID_EVRCA:
     vocevrca_dtx_cmd.handle  = session_obj->vs_handle;
     vocevrca_dtx_cmd.enable_flag  = session_obj->dtx_mode;
     rc  = vs_call( VS_VOCEVRCA_CMD_SET_DTX_MODE, ( void* )&vocevrca_dtx_cmd,
                    sizeof( vocevrca_dtx_cmd ) );
     break;

   case ONEX_VOC_ID_EVRCB:
    vocevrcb_dtx_cmd.handle  = session_obj->vs_handle;
    vocevrcb_dtx_cmd.enable_flag  = session_obj->dtx_mode;
    rc  = vs_call( VS_VOCEVRCB_CMD_SET_DTX_MODE, ( void* )&vocevrcb_dtx_cmd,
                   sizeof( vocevrcb_dtx_cmd ) );
    break;

   case ONEX_VOC_ID_EVRCNW:
    vocevrcnw_dtx_cmd.handle  = session_obj->vs_handle;
    vocevrcnw_dtx_cmd.enable_flag  = session_obj->dtx_mode;
    rc  = vs_call( VS_VOCEVRCNW_CMD_SET_DTX_MODE, ( void*)&vocevrcnw_dtx_cmd,
                   sizeof( vocevrcnw_dtx_cmd ) );
    break;

   case ONEX_VOC_ID_EVRCWB:
    vocevrcwb_dtx_cmd.handle  = session_obj->vs_handle;
    vocevrcwb_dtx_cmd.enable_flag  = session_obj->dtx_mode;
    rc  = vs_call( VS_VOCEVRCWB_CMD_SET_DTX_MODE, ( void* )&vocevrcwb_dtx_cmd,
                   sizeof( vocevrcwb_dtx_cmd ) );
    break;

   case ONEX_VOC_ID_EVRCNW2K:
    vocevrcnw2k_dtx_cmd.handle  = session_obj->vs_handle;
    vocevrcnw2k_dtx_cmd.enable_flag  = session_obj->dtx_mode;
    rc  = vs_call( VS_VOCEVRCNW2K_CMD_SET_DTX_MODE, ( void* )&vocevrcnw2k_dtx_cmd,
                   sizeof( vocevrcnw2k_dtx_cmd ) );
    break;

   case ONEX_VOC_ID_QCELP13K:
    vocqcelp13k_dtx_cmd.handle  = session_obj->vs_handle;
    vocqcelp13k_dtx_cmd.enable_flag  = session_obj->dtx_mode;
    rc  = vs_call( VS_VOCQCELP13K_CMD_SET_DTX_MODE, ( void* )&vocqcelp13k_dtx_cmd,
                   sizeof( vocqcelp13k_dtx_cmd ) );
    break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_set_voc_dtx_mode(): Unsupported vocoder type = (0x%08x)",
           session_obj->vocoder_type );
     break;
  }

  if ( rc == APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "cva_set_voc_dtx_mode(): vocoder_type = (0x%08x), dtx_enable_flag = "
           "(%d)", session_obj->vocoder_type, session_obj->dtx_mode );
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_set_voc_dtx_mode(): Failed vocoder_type = (0x%08x), dtx_enable_flag = "
           "(%d)", session_obj->vocoder_type, session_obj->dtx_mode );
  }

  return APR_EOK;
}

static uint32_t cva_process_ul_packet_evrca( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;
  vs_vocevrca_frame_info_t* vs_vocevrca_frame_info = NULL;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  uint32_t frame_rate;
  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrca(): entering" );

  for( ;; )
  {
    if ( vs_buffer->media_id != VS_COMMON_MEDIA_ID_EVRCA )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrca(): media ID = (0x%08x)  not valid,",
             vs_buffer->media_id );
      break;
    }

    vs_vocevrca_frame_info = ( vs_vocevrca_frame_info_t* ) vs_buffer->frame_info;
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_vocevrca_frame_rate_vs_to_onex ( vs_vocevrca_frame_info->rate,
                                               &frame_rate );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrca(): Unsupported frame rate = (%d),",
             vs_vocevrca_frame_info->rate );
      break;
    }

    /* Update the vocoder id to UL ONEX buffer. */
    onex_buffer->vocoder_id = ONEX_VOC_ID_EVRCA;
    /* Update the vocoder frame type to UL ONEX buffer. */
    onex_voc_frame_info->frame_rate = frame_rate;
    /* Update the vocoder frame size to UL ONEX buffer. */
    onex_buffer->size = vs_buffer->size;
    /* Update the vocoder frame size to UL ONEX buffer. */
    mmstd_memcpy( onex_buffer->frame, vs_buffer->size,
                  vs_buffer->frame, vs_buffer->size );

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrca(): leaving" );

  return rc;
}


static uint32_t cva_process_ul_packet_evrcb( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;
  vs_vocevrcb_frame_info_t* vs_vocevrcb_frame_info = NULL;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  uint32_t frame_rate;
  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcb(): entering" );

  for( ;; )
  {
    if ( vs_buffer->media_id != VS_COMMON_MEDIA_ID_EVRCB )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcb(): media ID = (0x%08x)  not valid,",
             vs_buffer->media_id );
      break;
    }

    vs_vocevrcb_frame_info = ( vs_vocevrcb_frame_info_t* ) vs_buffer->frame_info;
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_vocevrcb_frame_rate_vs_to_onex ( vs_vocevrcb_frame_info->rate,
                                               &frame_rate );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcb(): Unsupported frame rate = (%d),",
             vs_vocevrcb_frame_info->rate );
      break;
    }

    /* Update the vocoder id to UL ONEX buffer. */
    onex_buffer->vocoder_id = ONEX_VOC_ID_EVRCB;
    /* Update the vocoder frame type to UL ONEX buffer. */
    onex_voc_frame_info->frame_rate = frame_rate;
    /* Update the vocoder frame size to UL ONEX buffer. */
    onex_buffer->size = vs_buffer->size;
    /* Update the vocoder frame size to UL ONEX buffer. */
    mmstd_memcpy( onex_buffer->frame, vs_buffer->size,
                  vs_buffer->frame, vs_buffer->size );

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcb(): leaving" );

  return rc;
}

static uint32_t cva_process_ul_packet_evrcwb( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;
  vs_vocevrcwb_frame_info_t* vs_vocevrcwb_frame_info = NULL;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  uint32_t frame_rate;
  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcwb(): entering" );

  for( ;; )
  {
    if ( vs_buffer->media_id != VS_COMMON_MEDIA_ID_EVRCWB )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcwb(): media ID = (0x%08x)  not valid,",
             vs_buffer->media_id );
      break;
    }

    vs_vocevrcwb_frame_info = ( vs_vocevrcwb_frame_info_t* ) vs_buffer->frame_info;
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_vocevrcwb_frame_rate_vs_to_onex ( vs_vocevrcwb_frame_info->rate,
                                               &frame_rate );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcwb(): Unsupported frame rate = (%d),",
             vs_vocevrcwb_frame_info->rate );
      break;
    }

    /* Update the vocoder id to UL ONEX buffer. */
    onex_buffer->vocoder_id = ONEX_VOC_ID_EVRCWB;
    /* Update the vocoder frame type to UL ONEX buffer. */
    onex_voc_frame_info->frame_rate = frame_rate;
    /* Update the vocoder frame size to UL ONEX buffer. */
    onex_buffer->size = vs_buffer->size;
    /* Update the vocoder frame size to UL ONEX buffer. */
    mmstd_memcpy( onex_buffer->frame, vs_buffer->size,
                  vs_buffer->frame, vs_buffer->size );

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcwb(): leaving" );

  return rc;
}

static uint32_t cva_process_ul_packet_evrcnw( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;
  vs_vocevrcnw_frame_info_t* vs_vocevrcnw_frame_info = NULL;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  uint32_t frame_rate;
  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcnw(): entering" );

  for( ;; )
  {
    if ( vs_buffer->media_id != VS_COMMON_MEDIA_ID_EVRCNW )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcnw(): media ID = (0x%08x)  not valid,",
             vs_buffer->media_id );
      break;
    }

    vs_vocevrcnw_frame_info = ( vs_vocevrcnw_frame_info_t* ) vs_buffer->frame_info;
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_vocevrcnw_frame_rate_vs_to_onex ( vs_vocevrcnw_frame_info->rate,
                                               &frame_rate );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcnw(): Unsupported frame rate = (%d),",
             vs_vocevrcnw_frame_info->rate );
      break;
    }

    /* Update the vocoder id to UL ONEX buffer. */
    onex_buffer->vocoder_id = ONEX_VOC_ID_EVRCNW;
    /* Update the vocoder frame type to UL ONEX buffer. */
    onex_voc_frame_info->frame_rate = frame_rate;
    /* Update the vocoder frame size to UL ONEX buffer. */
    onex_buffer->size = vs_buffer->size;
    /* Update the vocoder frame size to UL ONEX buffer. */
    mmstd_memcpy( onex_buffer->frame, vs_buffer->size,
                  vs_buffer->frame, vs_buffer->size );

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcnw(): leaving" );

  return rc;
}

static uint32_t cva_process_ul_packet_evrcnw2k( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;
  vs_vocevrcnw2k_frame_info_t* vs_vocevrcnw2k_frame_info = NULL;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  uint32_t frame_rate;
  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcnw2k(): entering" );

  for( ;; )
  {
    if ( vs_buffer->media_id != VS_COMMON_MEDIA_ID_EVRCNW2K )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcnw2k(): media ID = (0x%08x)  not valid,",
             vs_buffer->media_id );
      break;
    }

    vs_vocevrcnw2k_frame_info = ( vs_vocevrcnw2k_frame_info_t* ) vs_buffer->frame_info;
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_vocevrcnw2k_frame_rate_vs_to_onex ( vs_vocevrcnw2k_frame_info->rate,
                                               &frame_rate );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_evrcnw2k(): Unsupported frame rate = (%d),",
             vs_vocevrcnw2k_frame_info->rate );
      break;
    }

    /* Update the vocoder id to UL ONEX buffer. */
    onex_buffer->vocoder_id = ONEX_VOC_ID_EVRCNW2K;
    /* Update the vocoder frame type to UL ONEX buffer. */
    onex_voc_frame_info->frame_rate = frame_rate;
    /* Update the vocoder frame size to UL ONEX buffer. */
    onex_buffer->size = vs_buffer->size;
    /* Update the vocoder frame size to UL ONEX buffer. */
    mmstd_memcpy( onex_buffer->frame, vs_buffer->size,
                  vs_buffer->frame, vs_buffer->size );

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_evrcnw(): leaving" );

  return rc;
}

static uint32_t cva_process_dl_packet_evrcnw( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;

  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_vocevrcnw_frame_info_t* vs_vocevrcnw_frame_info = NULL;
  vs_vocevrcnw_frame_rate_t frame_rate;
  

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != ONEX_VOC_ID_EVRCNW )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcnw(): vocoder ID = (0x%08x)  not valid,",
             onex_buffer->vocoder_id );
      break;
    }

    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    vs_vocevrcnw_frame_info = ( vs_vocevrcnw_frame_info_t* ) vs_buffer->frame_info;

    rc = cva_map_vocevrcnw_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate,
                                             
   &frame_rate );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcnw(): Unsupported frame type = (%d),",
             onex_voc_frame_info->frame_rate );
      break;
    }

    /* Update the media_id to VS buffer. */
    vs_buffer->media_id = VS_COMMON_MEDIA_ID_EVRCNW;
    /* Update the vocoder frame type to VS buffer. */
    vs_vocevrcnw_frame_info->rate = ( vs_vocevrcnw_frame_rate_t ) frame_rate;
    /* Update the vocoder frame size to VS buffer. */
    vs_buffer->size = onex_buffer->size;
    /* Update the vocoder frame to VS buffer. */
    mmstd_memcpy( vs_buffer->frame, vs_buffer->size,
                  onex_buffer->frame, vs_buffer->size );

    break;
  }

  return rc;
}

static uint32_t cva_process_ul_packet_qcelp13k( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;
  vs_vocqcelp13k_frame_info_t* vs_vocqcelp13k_frame_info = NULL;
  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  uint32_t frame_rate;
  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_qcelp13k(): entering" );

  for( ;; )
  {
    if ( vs_buffer->media_id != VS_COMMON_MEDIA_ID_QCELP13K )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_qcelp13k(): media ID = (0x%08x)  not valid,",
             vs_buffer->media_id );
      break;
    }

    vs_vocqcelp13k_frame_info = ( vs_vocqcelp13k_frame_info_t* ) vs_buffer->frame_info;
    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;

    /* Map VS to ONEX frame type. */
    rc = cva_map_vocqcelp13k_frame_rate_vs_to_onex ( vs_vocqcelp13k_frame_info->rate,
                                               &frame_rate );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_ul_packet_qcelp13k(): Unsupported frame rate = (%d),",
             vs_vocqcelp13k_frame_info->rate );
      break;
    }

    /* Update the vocoder id to UL ONEX buffer. */
    onex_buffer->vocoder_id = ONEX_VOC_ID_QCELP13K;
    /* Update the vocoder frame type to UL ONEX buffer. */
    onex_voc_frame_info->frame_rate = frame_rate;
    /* Update the vocoder frame size to UL ONEX buffer. */
    onex_buffer->size = vs_buffer->size;
    /* Update the vocoder frame size to UL ONEX buffer. */
    mmstd_memcpy( onex_buffer->frame, vs_buffer->size,
                  vs_buffer->frame, vs_buffer->size );

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "cva_process_ul_packet_qcelp13k(): leaving" );

  return rc;
}

static uint32_t cva_process_dl_packet_evrca( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;

  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_vocevrca_frame_info_t* vs_vocevrca_frame_info = NULL;
  vs_vocevrca_frame_rate_t frame_rate;
  

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != ONEX_VOC_ID_EVRCA )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrca(): vocoder ID = (0x%08x)  not valid,",
             onex_buffer->vocoder_id );
      break;
    }

    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    vs_vocevrca_frame_info = ( vs_vocevrca_frame_info_t* ) vs_buffer->frame_info;

    rc = cva_map_vocevrca_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate,
                                             
   &frame_rate );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrca(): Unsupported frame type = (%d),",
             onex_voc_frame_info->frame_rate );
      break;
    }

    /* Update the media_id to VS buffer. */
    vs_buffer->media_id = VS_COMMON_MEDIA_ID_EVRCA;
    /* Update the vocoder frame type to VS buffer. */
    vs_vocevrca_frame_info->rate = ( vs_vocevrca_frame_rate_t ) frame_rate;
    /* Update the vocoder frame size to VS buffer. */
    vs_buffer->size = onex_buffer->size;
    /* Update the vocoder frame to VS buffer. */
    mmstd_memcpy( vs_buffer->frame, vs_buffer->size,
                  onex_buffer->frame, vs_buffer->size );

    break;
  }

  return rc;
}

static uint32_t cva_process_dl_packet_evrcb( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;

  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_vocevrcb_frame_info_t* vs_vocevrcb_frame_info = NULL;
  vs_vocevrcb_frame_rate_t frame_rate;
  

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != ONEX_VOC_ID_EVRCB )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcb(): vocoder ID = (0x%08x)  not valid,",
             onex_buffer->vocoder_id );
      break;
    }

    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    vs_vocevrcb_frame_info = ( vs_vocevrcb_frame_info_t* ) vs_buffer->frame_info;

    rc = cva_map_vocevrcb_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate,
                                             
   &frame_rate );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcb(): Unsupported frame type = (%d),",
             onex_voc_frame_info->frame_rate );
      break;
    }

    /* Update the media_id to VS buffer. */
    vs_buffer->media_id = VS_COMMON_MEDIA_ID_EVRCB;
    /* Update the vocoder frame type to VS buffer. */
    vs_vocevrcb_frame_info->rate = ( vs_vocevrcb_frame_rate_t ) frame_rate;
    /* Update the vocoder frame size to VS buffer. */
    vs_buffer->size = onex_buffer->size;
    /* Update the vocoder frame to VS buffer. */
    mmstd_memcpy( vs_buffer->frame, vs_buffer->size,
                  onex_buffer->frame, vs_buffer->size );

    break;
  }

  return rc;
}

static uint32_t cva_process_dl_packet_evrcnw2k( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;

  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_vocevrcnw2k_frame_info_t* vs_vocevrcnw2k_frame_info = NULL;
  vs_vocevrcnw2k_frame_rate_t frame_rate;
  

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != ONEX_VOC_ID_EVRCNW2K )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcnw2k(): vocoder ID = (0x%08x)  not valid,",
             onex_buffer->vocoder_id );
      break;
    }

    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    vs_vocevrcnw2k_frame_info = ( vs_vocevrcnw2k_frame_info_t* ) vs_buffer->frame_info;

    rc = cva_map_vocevrcnw2k_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate,
                                             
   &frame_rate );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcnw2k(): Unsupported frame type = (%d),",
             onex_voc_frame_info->frame_rate );
      break;
    }

    /* Update the media_id to VS buffer. */
    vs_buffer->media_id = VS_COMMON_MEDIA_ID_EVRCNW2K;
    /* Update the vocoder frame type to VS buffer. */
    vs_vocevrcnw2k_frame_info->rate = ( vs_vocevrcnw2k_frame_rate_t ) frame_rate;
    /* Update the vocoder frame size to VS buffer. */
    vs_buffer->size = onex_buffer->size;
    /* Update the vocoder frame to VS buffer. */
    mmstd_memcpy( vs_buffer->frame, vs_buffer->size,
                  onex_buffer->frame, vs_buffer->size );

    break;
  }

  return rc;
}

static uint32_t cva_process_dl_packet_evrcwb( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;

  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_vocevrcwb_frame_info_t* vs_vocevrcwb_frame_info = NULL;
  vs_vocevrcwb_frame_rate_t frame_rate;
  

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != ONEX_VOC_ID_EVRCWB )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcwb(): vocoder ID = (0x%08x)  not valid,",
             onex_buffer->vocoder_id );
      break;
    }

    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    vs_vocevrcwb_frame_info = ( vs_vocevrcwb_frame_info_t* ) vs_buffer->frame_info;

    rc = cva_map_vocevrcwb_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate,
                                             
   &frame_rate );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_evrcwb(): Unsupported frame type = (%d),",
             onex_voc_frame_info->frame_rate );
      break;
    }

    /* Update the media_id to VS buffer. */
    vs_buffer->media_id = VS_COMMON_MEDIA_ID_EVRCWB;
    /* Update the vocoder frame type to VS buffer. */
    vs_vocevrcwb_frame_info->rate = ( vs_vocevrcwb_frame_rate_t ) frame_rate;
    /* Update the vocoder frame size to VS buffer. */
    vs_buffer->size = onex_buffer->size;
    /* Update the vocoder frame to VS buffer. */
    mmstd_memcpy( vs_buffer->frame, vs_buffer->size,
                  onex_buffer->frame, vs_buffer->size );

    break;
  }

  return rc;
}

static uint32_t cva_process_dl_packet_qcelp13k( 
  onex_vocoder_buffer_t* onex_buffer,
  vs_voc_buffer_t* vs_buffer
)
{
  uint32_t rc = APR_EOK;

  onex_voc_frame_info_t* onex_voc_frame_info = NULL;
  vs_vocqcelp13k_frame_info_t* vs_vocqcelp13k_frame_info = NULL;
  vs_vocqcelp13k_frame_rate_t frame_rate;
  

  for( ;; )
  {
    if ( onex_buffer->vocoder_id != ONEX_VOC_ID_QCELP13K )
    {
      rc = APR_EUNEXPECTED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_qcelp13k(): vocoder ID = (0x%08x)  not valid,",
             onex_buffer->vocoder_id );
      break;
    }

    onex_voc_frame_info = ( onex_voc_frame_info_t* ) onex_buffer->frame_info;
    vs_vocqcelp13k_frame_info = ( vs_vocqcelp13k_frame_info_t* ) vs_buffer->frame_info;

    rc = cva_map_vocqcelp13k_frame_rate_onex_to_vs ( onex_voc_frame_info->frame_rate,
                                             
   &frame_rate );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_process_dl_packet_qcelp13k(): Unsupported frame type = (%d),",
             onex_voc_frame_info->frame_rate );
      break;
    }

    /* Update the media_id to VS buffer. */
    vs_buffer->media_id = VS_COMMON_MEDIA_ID_QCELP13K;
    /* Update the vocoder frame type to VS buffer. */
    vs_vocqcelp13k_frame_info->rate = ( vs_vocqcelp13k_frame_rate_t ) frame_rate;
    /* Update the vocoder frame size to VS buffer. */
    vs_buffer->size = onex_buffer->size;
    /* Update the vocoder frame to VS buffer. */
    mmstd_memcpy( vs_buffer->frame, vs_buffer->size,
                  onex_buffer->frame, vs_buffer->size );

    break;
  }

  return rc;
}

/****************************************************************************
 * CVA VS SESSION ROUTINES                                                  *
 ****************************************************************************/
static void cva_log_event_info(
  cva_session_object_t* session_obj,
  uint32_t event_id
)
{
  switch( event_id )
  {
   case VS_COMMON_EVENT_CMD_RESPONSE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_COMMON_EVENT_CMD_RESPONSE session_obj=(0x%08x)", session_obj );
    }
    break;

   case VS_COMMON_EVENT_NOT_READY:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_COMMON_EVENT_NOT_READY session_obj=(0x%08x)", session_obj );
    }
    break;

   case VS_COMMON_EVENT_READY:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_COMMON_EVENT_READY session_obj=(0x%08x)", session_obj );
    }
    break;

   case VS_VOC_EVENT_READ_AVAILABLE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_VOC_EVENT_READ_AVAILABLE session_obj=(0x%08x)", session_obj );
    }
    break;

   case VS_VOC_EVENT_WRITE_BUFFER_RETURNED:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_VOC_EVENT_WRITE_BUFFER_RETURNED session_obj=(0x%08x)", session_obj );
    }
    break;

   case ONEX_VOC_EVENT_RQST_START:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "ONEX_VOC_EVENT_RQST_START session_obj=(0x%08x)", session_obj );
    }
    break;

   case ONEX_VOC_EVENT_RQST_STOP:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "ONEX_VOC_EVENT_RQST_STOP session_obj=(0x%08x)", session_obj );
    }
    break;

   case ONEX_VOC_EVENT_RQST_DTX_MODE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "ONEX_VOC_EVENT_RQST_DTX_MODE session_obj=(0x%08x)", session_obj );
    }
    break;

   case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "CM_PH_EVENT_SUBSCRIPTION_PREF_INFO session_obj=(0x%08x)", session_obj );
    }
    break;
    case ONEX_VOC_EVENT_RQST_AVG_ENC_RATE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "ONEX_VOC_EVENT_RQST_AVG_ENC_RATE session_obj=(0x%08x)", session_obj );
    }
    break;

    case ONEX_VOC_EVENT_RQST_MIN_MAX_RATE:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "ONEX_VOC_EVENT_RQST_MIN_MAX_RATE session_obj=(0x%08x)", session_obj );
    }
    break;

   default:
     break;
  }

  return;
}

static uint32_t cva_vs_event_cb(
 uint32_t event_id,
 void* params,
 uint32_t size,
 void* session_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_event_write_buffer_returned_t write_buf_evt;

  if ( cva_is_initialized == FALSE ) return APR_EOK;

  switch ( event_id )
  {
   case VS_COMMON_EVENT_CMD_RESPONSE:
   case VS_COMMON_EVENT_NOT_READY:
   case VS_COMMON_EVENT_READY:
   case VS_VOC_EVENT_READ_AVAILABLE:
   case VS_VOC_EVENT_WRITE_BUFFER_RETURNED:
    {
      (void) cva_log_event_info( ( cva_session_object_t* ) session_context,
                                 event_id );
      /* Super hack: Needs VS behaviour to be corrected. */
      if ( event_id == VS_VOC_EVENT_WRITE_BUFFER_RETURNED ) 
      {
        if ( params == NULL )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
                 "cva_vs_event_cb(): params NULL Ignoring event ID=(0x%08X)",
                 event_id );
          break;
        }
        write_buf_evt.buffer = ( vs_voc_buffer_t* ) params;
        params = ( void* ) &write_buf_evt;
      }

      rc = cva_prepare_and_dispatch_event_packet( session_context, event_id,
                                                  params, size );
    }
    break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "cva_vs_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t cva_vs_prime_read_buffer (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_prime_read_buffer_t prime_read_buf_cmd;

  prime_read_buf_cmd.handle = session_obj->vs_handle;
  prime_read_buf_cmd.buffer = session_obj->vs_read_buf;

  rc = vs_call( VS_VOC_CMD_PRIME_READ_BUFFER, ( void* ) &prime_read_buf_cmd,
                sizeof( prime_read_buf_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_vs_prime_read_buffer(): Failed to prime vs_read_buf, "
           "vs_read_buf=(0x%08x), rc=(0x%08x)", session_obj->vs_read_buf, rc );
  }
  else
  {
    session_obj->vs_read_buf = NULL;
  }

  return APR_EOK;
}

static uint32_t cva_vs_read_buffer (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_read_buffer_t read_buf_cmd;

  read_buf_cmd.handle = session_obj->vs_handle;
  read_buf_cmd.ret_buffer = &session_obj->vs_read_buf;

  rc = vs_call( VS_VOC_CMD_READ_BUFFER, ( void* )&read_buf_cmd,
                sizeof( read_buf_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_vs_read_buffer(): Failed to read vs_buffer, "
           "vs_read_buf=(0x%08x), rc=(0x%08x)", session_obj->vs_read_buf, rc );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "cva_vs_read_buffer(): read buffer available vs_read_buf=(0x%08x) ",
           session_obj->vs_read_buf );
  }

  return rc;
}

static uint32_t cva_vs_write_buffer (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_write_buffer_t write_buf_cmd;

  write_buf_cmd.handle = session_obj->vs_handle;
  write_buf_cmd.buffer = session_obj->vs_write_buf;

  rc = vs_call( VS_VOC_CMD_WRITE_BUFFER, ( void* )&write_buf_cmd,
                sizeof( write_buf_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_vs_write_buffer(): Failed to pass vs_write_buf, "
           "vs_write_buf=(0x%08x), rc=(0x%08x)", session_obj->vs_write_buf, rc );
  }
  else
  {
    session_obj->vs_write_buf = NULL;
  }

  return APR_EOK;
}

static uint32_t cva_vs_free_buffer (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_free_buffer_t free_buf_cmd;

  if ( session_obj->vs_read_buf != NULL )
  {
    free_buf_cmd.handle = session_obj->vs_handle;
    free_buf_cmd.buffer = session_obj->vs_read_buf;

    rc = vs_call( VS_VOC_CMD_FREE_BUFFER, ( void* )&free_buf_cmd,
                  sizeof( free_buf_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_free_buffer(): Failed to free vs_read_buf = (0x%08x), " 
             "rc = (0x%08x)", session_obj->vs_read_buf, rc );
    }
    session_obj->vs_read_buf = NULL;
  }

  if ( session_obj->vs_write_buf != NULL )
  {
    free_buf_cmd.handle = session_obj->vs_handle;
    free_buf_cmd.buffer = session_obj->vs_write_buf;

    rc = vs_call( VS_VOC_CMD_FREE_BUFFER, ( void* )&free_buf_cmd,
                  sizeof( free_buf_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_free_buffer(): Failed to free vs_write_buf = (0x%08x), " 
             "rc = (0x%08x)", session_obj->vs_write_buf, rc );
    }
    session_obj->vs_write_buf = NULL;
  }

  return rc;
}

static uint32_t cva_vs_alloc_buffer (
  cva_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_alloc_buffer_t alloc_buf_cmd;

  for ( ;; )
  {
    /* Allocate read buffer. */
    session_obj->vs_read_buf = NULL;
    alloc_buf_cmd.handle = session_obj->vs_handle;
    alloc_buf_cmd.ret_buffer = &session_obj->vs_read_buf;
    alloc_buf_cmd.req_max_frame_size = CVA_MAX_PACKET_SIZE; 
    rc = vs_call( VS_VOC_CMD_ALLOC_BUFFER, ( void* )&alloc_buf_cmd,
                  sizeof( alloc_buf_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_alloc_buffer(): Failed to allocate read buffer, "
             "rc = (0x%08x)", rc );
      break;
    }
    session_obj->primed_read_buf = session_obj->vs_read_buf;

    /* Allocate write buffer. */
    session_obj->vs_write_buf = NULL;
    alloc_buf_cmd.handle = session_obj->vs_handle;
    alloc_buf_cmd.ret_buffer = &session_obj->vs_write_buf;
    alloc_buf_cmd.req_max_frame_size = CVA_MAX_PACKET_SIZE;
    rc = vs_call( VS_VOC_CMD_ALLOC_BUFFER, ( void* )&alloc_buf_cmd,
                  sizeof( alloc_buf_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_alloc_buffer(): Failed to allocate write buffer, "
             "rc = (0x%08x)", rc );
      ( void ) cva_vs_free_buffer ( session_obj );
      break;
    }

    break;
  }

  return rc;
}

static uint32_t cva_vs_close_session (
  cva_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_close_t close_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    /* Free read and write buffers. */
    ( void ) cva_vs_free_buffer ( session_obj );

    close_cmd.handle = session_obj->vs_handle;
    close_cmd.client_context = client_context;

    rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&close_cmd, sizeof( close_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_close_session(): Failed to close VS session, " 
             "handle = (0x%08x), rc = (0x%08x)", session_obj->vs_handle, rc );
    }
    else
    {
      session_obj->vs_handle = APR_NULL_V;
      rc = APR_EPENDING;
    }

    break;
  }

  return rc;
}

static uint32_t cva_vs_open_session (
 cva_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;

  for ( ;; )
  {
    /* Open VS session. */
    open_cmd.ret_handle = &session_obj->vs_handle;
    open_cmd.vsid = session_obj->vsid;
	
    if ( SYS_MODEM_AS_ID_1 == session_obj->asid )
    {
      open_cmd.client_id = VS_VOC_CLIENT_CDMA;
    }
    else
    {
      open_cmd.client_id = VS_VOC_CLIENT_CDMA2;
    }
	
    open_cmd.session_context = ( void* )session_obj;
    open_cmd.event_cb = cva_vs_event_cb;

    rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_open_session(): failed to open VS session, "
             "client = (0x%08x), rc = (0x%08x)", open_cmd.client_id, rc );
      break;
    }

    rc = cva_vs_alloc_buffer( session_obj );
    if ( rc )
    {
      ( void ) cva_vs_close_session ( session_obj, NULL );
      
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_open_session(): failed to alloc VS buffers, "
             "client = (0x%08x), rc = (0x%08x)", open_cmd.client_id, rc );
      break;
    }

    break;
  }

  return APR_EOK;
}


static uint32_t cva_vs_flush_buffers (
  cva_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_flush_buffers_t vs_flush_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    vs_flush_cmd.handle = session_obj->vs_handle;
    vs_flush_cmd.client_context = client_context;
    rc = vs_call( VS_VOC_CMD_FLUSH_BUFFERS, &vs_flush_cmd,
                  sizeof( vs_flush_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_flush_buffers(): Failed to flush VS buffers, " 
             "handle = (0x%08x), rc = (0x%08x)", session_obj->vs_handle, rc );
    }
    else
    {
      rc = APR_EPENDING;
    }

    break;
  } /* For loop ends here. */

  return rc;
}


static uint32_t cva_vs_disable_vocoder (
  cva_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_disable_t vs_disable_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    vs_disable_cmd.handle = session_obj->vs_handle;
    vs_disable_cmd.client_context = client_context;
    rc = vs_call( VS_VOC_CMD_DISABLE, &vs_disable_cmd,
                  sizeof( vs_disable_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_disable_vocoder(): Failed to disable VS session, " 
             "handle = (0x%08x), rc = (0x%08x)", session_obj->vs_handle, rc );
    }
    else
    {
      rc = APR_EPENDING;
    }

    break;
  }/* For loop ends here. */

  return rc;
 }

static uint32_t cva_vs_enable_vocoder (
  cva_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  uint32_t media_id;
  vs_voc_cmd_enable_t vs_enable_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    media_id = cva_map_vocoder_type_onex_to_vs ( session_obj->vocoder_type );
    vs_enable_cmd.handle = session_obj->vs_handle;
    vs_enable_cmd.media_id = media_id;
    vs_enable_cmd.client_context = client_context;
    rc = vs_call( VS_VOC_CMD_ENABLE, &vs_enable_cmd,
                  sizeof( vs_enable_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_vs_enable_vocoder(): Failed to enable VS session, " 
             "handle = (0x%08x), rc = (0x%08x)", session_obj->vs_handle, rc );
    }
    else
    {
      rc = APR_EPENDING;
    }

    break;
  }/* For loop ends here. */

  return rc;
}


/*===========================================================================
  FUNCTION cva_subs_info_cb()

  DESCRIPTION
    Handle CM subs event callback

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void cva_subs_info_cb( 
  cm_ph_event_e_type event,
  const cm_subs_info_s_type *params 
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj;
  cva_eventi_subs_pref_info subs_event;

  if ( params == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
         "cva_subs_info_cb(): param are NULL!!");
    return;
  }

  switch ( event )
  {
     case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
     {
       session_obj = cva_session_list[ params->subscription_id ] ;

       subs_event.vsid = params->hw_id[SYS_VSID_APP_CS_VOICE]; 
       subs_event.is_sub_active = params->is_subphone_active;
       rc = cva_prepare_and_dispatch_event_packet( session_obj,
              CVA_IVOICE_EVENTI_SUBS_PREF_INFO, &subs_event, sizeof(subs_event) );

       ( void ) cva_log_event_info( session_obj, event );

       MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "cva_subs_info_cb: subscription id=%d, vsid=(0x%08x),is_active %d",
              params->subscription_id, params->hw_id[SYS_VSID_APP_CS_VOICE],
              params->is_subphone_active );
     }
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "cva_subs_info_cb(): Unsupported event (%d)", event );
  }

  return;
}

/****************************************************************************
 * CVA ONEX SESSION ROUTINES                                                 *
 ****************************************************************************/

static uint32_t cva_onex_event_cb(
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
)
{
  uint32_t rc = APR_EOK;

  switch ( event_id )
  {
   case ONEX_VOC_EVENT_RQST_START:
   case ONEX_VOC_EVENT_RQST_STOP:
   case ONEX_VOC_EVENT_RQST_DTX_MODE:
   case ONEX_VOC_EVENT_RQST_MIN_MAX_RATE:
   case ONEX_VOC_EVENT_RQST_AVG_ENC_RATE:
    (void) cva_log_event_info( ( cva_session_object_t*)session_context, event_id );
     rc = cva_prepare_and_dispatch_event_packet( session_context, event_id,
                                                 params, size );
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "cva_onex_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static void cva_onex_vocoder_ul_cb(
 void* session_context,
 onex_vocoder_buffer_t* onex_buffer
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  session_obj = ( cva_session_object_t* ) session_context;
  onex_buffer->flags = FALSE;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "cva_onex_vocoder_ul_cb(): entering" );

  CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

  if ( ( session_obj->is_vs_ready == TRUE ) &&
       ( session_obj->vs_read_buf != NULL ) )
  {
    switch ( session_obj->vocoder_type )
    {
      case ONEX_VOC_ID_EVRCA:
        rc = cva_process_ul_packet_evrca( onex_buffer, session_obj->vs_read_buf );
        break;

      case ONEX_VOC_ID_EVRCB:
        rc = cva_process_ul_packet_evrcb( onex_buffer, session_obj->vs_read_buf );
        break;

      case ONEX_VOC_ID_EVRCNW:
        rc = cva_process_ul_packet_evrcnw( onex_buffer, session_obj->vs_read_buf );
        break;

      case ONEX_VOC_ID_EVRCNW2K:
        rc = cva_process_ul_packet_evrcnw2k( onex_buffer, session_obj->vs_read_buf );
        break;

      case ONEX_VOC_ID_EVRCWB:
        rc = cva_process_ul_packet_evrcwb( onex_buffer, session_obj->vs_read_buf );
        break;

      case ONEX_VOC_ID_QCELP13K:
        rc = cva_process_ul_packet_qcelp13k( onex_buffer, session_obj->vs_read_buf );
        break;

      default:
        rc = APR_EUNEXPECTED;
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_onex_vocoder_ul_cb(): vocoder type unsupported by CVA" );
        break;
    }

    if ( rc == APR_EOK )
    {
      /* UL ONEX buffer successfully populated. */
      onex_buffer->flags = TRUE;
    }

    /* Prime vs read buffer with voice services for next uplink packet. */
    rc = cva_vs_prime_read_buffer( session_obj );
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_onex_vocoder_ul_cb(): UL vocoder Packet not transfered to ONEX, "
           "is_vs_ready=(%d), vs_read_buf=(0x%08x)", session_obj->is_vs_ready,
           session_obj->vs_read_buf );
  }

  CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "cva_onex_vocoder_ul_cb(): leaving" );

  return;
}

static void cva_onex_vocoder_dl_cb (
  void* session_context,
  onex_vocoder_buffer_t* onex_buffer
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  session_obj = ( cva_session_object_t* ) session_context;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "cva_onex_vocoder_dl_cb(): entering" );

  CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

  if ( ( session_obj->is_vs_ready == TRUE ) && ( onex_buffer->flags == TRUE ) &&
       ( session_obj->vs_write_buf != NULL ) )
  {
    /**
     * Pass the vocoder packet content avaialble form ONEX protocol 
     * software to VS.
     */
    switch ( session_obj->vocoder_type )
    {
      case ONEX_VOC_ID_EVRCA:
        rc = cva_process_dl_packet_evrca( onex_buffer, session_obj->vs_write_buf );
        break;

      case ONEX_VOC_ID_EVRCB:
        rc = cva_process_dl_packet_evrcb( onex_buffer, session_obj->vs_write_buf );
        break;

      case ONEX_VOC_ID_EVRCNW:
        rc = cva_process_dl_packet_evrcnw( onex_buffer, session_obj->vs_write_buf );
        break;

      case ONEX_VOC_ID_EVRCNW2K:
        rc = cva_process_dl_packet_evrcnw2k( onex_buffer, session_obj->vs_write_buf );
        break;

      case ONEX_VOC_ID_EVRCWB:
        rc = cva_process_dl_packet_evrcwb( onex_buffer, session_obj->vs_write_buf );
        break;

      case ONEX_VOC_ID_QCELP13K:
        rc = cva_process_dl_packet_qcelp13k( onex_buffer, session_obj->vs_write_buf );
        break;

      default:
        rc = APR_EUNEXPECTED;
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cva_onex_vocoder_dl_cb(): vocoder type unsupported by CVA" );
        break;
    }

    /**
     * Pass write buffer to VS, if dl packet processed successfully.
     *
     * TODO: Add internal event to write the buffer to VS/log the vocoder 
     *       packet.
     */
    if ( rc == APR_EOK )
    {
      rc = cva_vs_write_buffer( session_obj );
    }
  }
  else
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_onex_vocoder_dl_cb(): DL vocoder buffer not tranfered to VS, "
           "is_vs_ready = (%d), onex_buffer->flags = (0X%08X), "
           "vs_write_buf=(0X%08X)", session_obj->is_vs_ready, onex_buffer->flags,
           session_obj->vs_write_buf );
  }

  CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "cva_onex_vocoder_dl_cb(): leaving" );

  return;
}

static void cva_open_onex_session (
 cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  onex_voc_cmd_open_t open_cmd;

  open_cmd.ret_handle = &session_obj->onex_handle;
  open_cmd.asid = session_obj->asid;
  open_cmd.event_cb = cva_onex_event_cb;
  open_cmd.ul_cb = cva_onex_vocoder_ul_cb;
  open_cmd.dl_cb = cva_onex_vocoder_dl_cb;  
  open_cmd.session_context = ( void* ) session_obj;

#ifndef WINSIM

  rc = onex_voice_call ( ONEX_VOC_OPEN_CMD, &open_cmd, sizeof( open_cmd ) );
  if ( ( APR_EUNSUPPORTED != rc ) && ( APR_EOK != rc ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_onex_open_session(): Failed to open ONEX session, "
           "asid = (0x%08x), rc = (0x%08x)", open_cmd.asid, rc );
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "cva_onex_open_session(): Processed Successfully, asid = (0x%08x), "
           "rc = (0x%08x)", open_cmd.asid, rc );
    rc = APR_EOK;
  }

#endif
  return;
}

static uint32_t cva_close_onex_session (
  cva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  onex_voc_cmd_close_t close_cmd;
 
  close_cmd.handle = session_obj->onex_handle;

  rc = onex_voice_call ( ONEX_VOC_CLOSE_CMD, &close_cmd, sizeof( close_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_close_onex_session(): failed to close ONEX session, "
           "asid = (0x%08x), rc = (0x%08x)", session_obj->asid, rc );
  }
 
  return rc;
}


/****************************************************************************
 * CVA CMDs/EVENTs HANDLING ROUTINES                                        *
 ****************************************************************************/

static uint32_t cva_process_vs_cmd_response_event( 
 cva_event_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  vs_common_event_cmd_response_t* evt_params;

  for ( ;; )
  {
    session_obj = ( cva_session_object_t* ) packet->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = ( ( vs_common_event_cmd_response_t* ) packet->params );
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    simple_obj = ( cva_simple_job_object_t* ) evt_params->client_context;
    if( simple_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    simple_obj->is_completed = TRUE;
    simple_obj->status = evt_params->status_id;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, 
       "cva_process_vs_cmd_response_event(): cmd response event recived "
       "for VS cmd_id=(0x%08x), client_context=(0x%08x), status=(0x%08x)",
       evt_params->cmd_id, evt_params->client_context, evt_params->status_id );
    break;
  }

  cva_free_event_packet( packet );

  return rc;
}

static uint32_t cva_process_vs_ready_event( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* event_pkt = NULL;
  cva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    event_pkt = ( cva_event_packet_t* ) ctrl->packet;
    if( event_pkt == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
    
    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "cva_process_vs_ready_event(): vocoder session ready for packet "
         "exchange" );

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    session_obj->is_vs_ready = TRUE;

    rc = cva_vs_prime_read_buffer( session_obj );

    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );
    break;
  }

  cva_free_event_packet ( event_pkt );

  return rc;
}


static uint32_t cva_process_vs_not_ready_event( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* event_pkt = NULL;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    event_pkt = ( cva_event_packet_t* ) ctrl->packet;
    if( event_pkt == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
  
    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );
    
    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "cva_process_vs_not_ready_event(): for session index=%d ", 
             session_obj->asid );

      session_obj->is_vs_ready = FALSE;

      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj= &ctrl->rootjob_obj->simple_job;

      rc = cva_vs_flush_buffers( session_obj, (void*)simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
      }
      else
      {
        /* Restore the read buffer reference if not returned from VS. */
        session_obj->vs_read_buf = session_obj->primed_read_buf;
      }
    }
    break;
  }

  if ( NULL != session_obj )
  {
    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );
  }

  if ( rc != APR_EPENDING )
  {
    ( void ) cva_mem_free_object ( ( cva_object_t*) simple_obj );
    ( void ) cva_free_event_packet ( event_pkt );
    rc = APR_EOK;
  }

  return rc;
}


static uint32_t cva_process_vs_open_event ( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* event_pkt = NULL;
  cva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    event_pkt = ( cva_event_packet_t* ) ctrl->packet;
    if( event_pkt == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
    
    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "cva_process_vs_open_event(): for session index=%d ", 
           session_obj->asid );

    ( void ) cva_vs_open_session( session_obj );

    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    break;
  }

  cva_free_event_packet ( event_pkt );

  return rc;
}

static uint32_t cva_process_vs_close_event ( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* event_pkt = NULL;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    event_pkt = ( cva_event_packet_t* ) ctrl->packet;
    if( event_pkt == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
    
    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "cva_process_vs_close_event(): for session index=%d ", 
             session_obj->asid );

      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      rc = cva_vs_close_session( session_obj, (void*) simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
      }
      else
      {
        session_obj->vs_handle = APR_NULL_V;
      }
    }
    break;
  }

  if ( NULL != session_obj )
  {
    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );
  }

  if ( rc != APR_EPENDING )
  {
    ( void ) cva_mem_free_object ( ( cva_object_t*) simple_obj );
    ( void ) cva_free_event_packet ( event_pkt );
    rc = APR_EOK;
  }

  return rc;
}


static uint32_t cva_process_vs_read_buf_available_event( 
 cva_event_packet_t* event_pkt 
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;

  session_obj = ( cva_session_object_t* ) event_pkt->session_context;

  CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

  /* This indicates that the vocoder buffer primed intialy, VS has returned 
     with valid vocoder content. */
  rc = cva_vs_read_buffer ( session_obj );

  CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );

  cva_free_event_packet ( event_pkt );

  return rc;
}

static uint32_t cva_process_vs_write_buf_returned_event( 
 cva_event_packet_t* event_pkt 
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  vs_voc_event_write_buffer_returned_t* evt_params = NULL;

  for ( ;; )
  {
    session_obj = ( cva_session_object_t* ) event_pkt->session_context;

    evt_params = ( vs_voc_event_write_buffer_returned_t* ) event_pkt->params;
    if ( evt_params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_process_vs_write_buf_returned_event(): evt_paramsr is NULL" );
      break;
    }

    if ( evt_params->buffer == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_process_vs_write_buf_returned_event(): ret_buffer is NULL" );
      break;
    }

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );
    
    /* Indicates that the vocoder buffer written to voice services has been 
     * successfully rendered to DSP.
     * Write buffer returned for next downlink vocoder packet.
     */
    session_obj->vs_write_buf = evt_params->buffer;

    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    break;
  }

  cva_free_event_packet ( event_pkt );

  return rc;
}

static uint32_t cva_process_onex_dtx_mode_event( 
 cva_event_packet_t* event_pkt 
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_dtx_mode_t* evt_params = NULL;

  session_obj = ( cva_session_object_t* ) event_pkt->session_context;
  evt_params = ( onex_voc_event_rqst_dtx_mode_t* ) event_pkt->params;
  
  if ( session_obj->dtx_mode != evt_params->enable_flag )
  {
    session_obj->dtx_mode = evt_params->enable_flag;

    rc = cva_set_voc_dtx_mode ( session_obj );
  }

  cva_free_event_packet ( event_pkt );

  return rc;
}

static uint32_t cva_process_onex_min_max_rate_event( 
 cva_event_packet_t* event_pkt 
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_min_max_rate_t* evt_params = NULL;

  session_obj = ( cva_session_object_t* ) event_pkt->session_context;
  evt_params = ( onex_voc_event_rqst_min_max_rate_t* ) event_pkt->params;

  if ( ( session_obj->min_rate != evt_params->min_rate ) ||
       ( session_obj->max_rate != evt_params->max_rate ) )
  {
    session_obj->min_rate = evt_params->min_rate;
    session_obj->max_rate = evt_params->max_rate;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "cva_process_onex_min_max_rate_event(): vocoder_type = (0x%08x), min_rate = (0x%08x), max_rate = (0x%08x)"
             , session_obj->vocoder_type, session_obj->min_rate, session_obj->max_rate );
    rc = cva_set_voc_min_max_rate ( session_obj );
  }

  cva_free_event_packet ( event_pkt );
  return rc;
}


static uint32_t cva_process_cm_sub_pref_event( 
  cva_event_packet_t* event_pkt 
)
{
  uint32_t rc = APR_EOK;
  cva_eventi_subs_pref_info* evt_params = NULL;
  cva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    evt_params = ( cva_eventi_subs_pref_info*) event_pkt->params;
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EBADPARAM);
      break;
    }

    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EBADPARAM );
      break;
    }

    CVA_ACQUIRE_LOCK ( cva_session_lock_list[ session_obj->asid ] );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "CVA: CM_SUBS_PREF_INFO: asid=%d, cur_vsid=0x%08x, new_vsid=0x%08x",
           session_obj->asid, session_obj->vsid, evt_params->vsid );

    /* If pending vsid is not same active VSID close the current VS session. */
    if( evt_params->vsid != session_obj->vsid )
    {
      rc = cva_prepare_and_dispatch_event_packet( 
             session_obj, CVA_IVOICE_EVENTI_VS_CLOSE, NULL, 0 );

      session_obj->vsid = evt_params->vsid;
      rc = cva_prepare_and_dispatch_event_packet( 
             session_obj, CVA_IVOICE_EVENTI_VS_OPEN, NULL, 0 );
    }

    break;
  }

  if ( NULL != session_obj )
  {
    CVA_RELEASE_LOCK ( cva_session_lock_list[ session_obj->asid ] );
  }

  ( void ) cva_free_event_packet ( event_pkt );

  return APR_EOK;
}

static uint32_t cva_process_onex_enc_rate_event( 
 cva_event_packet_t* event_pkt 
)
{
  uint32_t rc = APR_EOK;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_avg_enc_rate_t* evt_params = NULL;

  session_obj = ( cva_session_object_t* ) event_pkt->session_context;
  evt_params = ( onex_voc_event_rqst_avg_enc_rate_t* ) event_pkt->params;

  if ( session_obj->avg_enc_rate != evt_params->avg_enc_rate )
  {
    session_obj->avg_enc_rate = evt_params->avg_enc_rate;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "cva_process_onex_enc_rate_event(): vocoder_type = (0x%08x), enc_rate = (0x%08x)"
             , session_obj->vocoder_type, session_obj->avg_enc_rate );

    rc = cva_set_voc_enc_rate(session_obj);
  }
  cva_free_event_packet ( event_pkt );
  return rc;
}

static uint32_t cva_process_onex_vocoder_start_event( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* event_pkt = NULL;
  cva_session_object_t* session_obj = NULL;
  onex_voc_event_rqst_start_t* evt_params = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  onex_voc_cmd_start_t onex_cmd_start;

  for ( ;; )
  {
    event_pkt = ( cva_event_packet_t* ) ctrl->packet;
    if( event_pkt == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = ( onex_voc_event_rqst_start_t* ) event_pkt->params;
    if( evt_params == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if ( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      if ( session_obj->is_onex_ready == TRUE )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "cva_process_onex_vocoder_start_event() - b2b request start "
              "configured vocoder id =(0x%08x), provided vocoder id =(0x%08x)",
              session_obj->vocoder_type, evt_params->vocoder_id );
        rc = APR_EOK;
        break;
      }

      /* Update the CVA call state. */
      session_obj->is_onex_ready = TRUE;
      session_obj->vocoder_type = evt_params->vocoder_id;

      /* Create the Sequencer job object to track CVD setup. */
      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      /* Send a vocoder request event to voice agent. */

      /* Set cached vocoder properties. */
      ( void )cva_set_voc_dtx_mode ( session_obj );
      ( void )cva_set_voc_enc_rate ( session_obj );
      ( void )cva_set_voc_min_max_rate ( session_obj );

      rc = cva_vs_enable_vocoder( session_obj, (void*) simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
      }
      else
      {
       /* Send a vocoder release event to voice agent. */
      }
    }

    break;
  }

  if ( NULL != session_obj )
  {
    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );
  }

  if ( rc != APR_EPENDING )
  {
    if ( NULL != session_obj )
    {
      onex_cmd_start.handle = session_obj->onex_handle;
      rc = onex_voice_call( ONEX_VOC_START_CMD, &onex_cmd_start,
                            sizeof( onex_cmd_start ) );
    }

    ( void ) cva_mem_free_object ( ( cva_object_t*) simple_obj );
    ( void ) cva_free_event_packet ( event_pkt );
    rc = APR_EOK;
  }

  return rc;
}


static uint32_t cva_process_onex_vocoder_stop_event( 
 cva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  cva_event_packet_t* event_pkt = NULL;
  cva_session_object_t* session_obj = NULL;
  cva_simple_job_object_t* simple_obj = NULL;
  onex_voc_cmd_stop_t onex_cmd_stop;

  for ( ;; )
  {
    event_pkt = ( cva_event_packet_t* ) ctrl->packet;
    if( event_pkt == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( cva_session_object_t* ) event_pkt->session_context;
    if( session_obj == NULL )
    {
      CVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    CVA_ACQUIRE_LOCK( cva_session_lock_list[ session_obj->asid ] );

    if ( ctrl->state == CVA_GATING_CMD_STATE_EXECUTE )
    {
      if ( session_obj->is_onex_ready == FALSE )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "cva_process_onex_vocoder_stop_event() - vocoder session not "
               "running for session index=%d ", session_obj->asid );
        rc = APR_EOK;
        break;
      }

      /* Update the CVA call state. */
      session_obj->is_onex_ready = FALSE;

      /* Create the simple job object to track VS disable. */
      rc = cva_create_simple_job_object( session_obj->header.handle,
             ( cva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj= &ctrl->rootjob_obj->simple_job;

      rc = cva_vs_disable_vocoder( session_obj, (void*) simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
      }
      else
      {
       /* Send a vocoder release event to voice agent. */
      }
    }

    break;
  }

  if ( NULL != session_obj )
  {
    CVA_RELEASE_LOCK( cva_session_lock_list[ session_obj->asid ] );
  }

  if ( rc != APR_EPENDING ) 
  {
    if ( NULL != session_obj ) 
    {
      onex_cmd_stop.handle = session_obj->onex_handle;
      rc = onex_voice_call( ONEX_VOC_STOP_CMD, &onex_cmd_stop,
                            sizeof( onex_cmd_stop ) );
    }

    ( void ) cva_mem_free_object ( ( cva_object_t*) simple_obj );
    ( void ) cva_free_event_packet ( event_pkt );
    rc = APR_EOK;
  }

  return rc;
}

/****************************************************************************
 * NONGATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/

static void cva_task_process_nongating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t request_id = 0;
  cva_work_item_packet_type_t pkt_type;
  cva_work_item_t* work_item = NULL;
  void* packet = NULL;

  while( apr_list_remove_head( &cva_nongating_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    pkt_type = work_item->pkt_type;
    packet = work_item->packet;
    
    if ( pkt_type == CVA_WORK_ITEM_PKT_TYPE_EVENT )
    {
      request_id = ( ( cva_event_packet_t* ) packet )->event_id ;
    }
    else if ( pkt_type ==  CVA_WORK_ITEM_PKT_TYPE_CMD )
    {
      request_id = ( ( cva_cmd_packet_t* ) packet )->cmd_id;
    }
    else
    {
      CVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
    }

    /* Add back to vs_free_work_pkt_q. */
    work_item->pkt_type = CVA_WORK_ITEM_PKT_TYPE_NONE;
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &cva_free_work_pkt_q, &work_item->link );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "cva_task_process_nongating_work_items(): Processing "
           "request_id=(0X%08X)", request_id );

    switch( request_id )
    {
     /**
      * Handling routine for nongating work-items should take of release the
      * memory allocated for the CMD/EVENT packets.
      */
     case ONEX_VOC_EVENT_RQST_DTX_MODE:
       rc = cva_process_onex_dtx_mode_event( ( cva_event_packet_t* ) packet );
       break;

     case ONEX_VOC_EVENT_RQST_MIN_MAX_RATE:
       rc = cva_process_onex_min_max_rate_event( ( cva_event_packet_t* ) packet );
       break;

     case ONEX_VOC_EVENT_RQST_AVG_ENC_RATE:
       rc = cva_process_onex_enc_rate_event( ( cva_event_packet_t* ) packet );
       break;

     case VS_COMMON_EVENT_CMD_RESPONSE:
       rc = cva_process_vs_cmd_response_event( ( cva_event_packet_t* ) packet );
       break;

     case VS_VOC_EVENT_READ_AVAILABLE:
       rc = cva_process_vs_read_buf_available_event( ( cva_event_packet_t* ) packet );
       break;

     case VS_VOC_EVENT_WRITE_BUFFER_RETURNED:
       rc = cva_process_vs_write_buf_returned_event( ( cva_event_packet_t* ) packet );
       break;

     case CVA_IVOICE_EVENTI_SUBS_PREF_INFO:
       rc = cva_process_cm_sub_pref_event( ( cva_event_packet_t* ) packet );
       break;

     default:
       /* Add remaining work items to the gating work queue. */
       rc = cva_queue_work_packet ( CVA_WORK_ITEM_QUEUE_TYPE_GATING,
                                    pkt_type, packet );
       if ( rc )
       {
         if ( pkt_type == CVA_WORK_ITEM_PKT_TYPE_CMD )
         {
           cva_free_cmd_packet( ( cva_cmd_packet_t* ) packet );
         }
         else
         {
          cva_free_event_packet( ( cva_event_packet_t* ) packet );
         }
       }
       break;
    }
  }

  return;
}

/****************************************************************************
 * GATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/
 
static void cva_task_process_gating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t request_id = 0;
  cva_work_item_t* work_item;
  cva_cmd_packet_t* cmd_pkt;
  cva_event_packet_t* event_pkt;
  cva_gating_control_t* ctrl = &cva_gating_work_pkt_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
     case CVA_GATING_CMD_STATE_FETCH:
       {
          /* Fetch the next gating command to execute. */
          rc = apr_list_remove_head( &ctrl->cmd_q,
                                     ( ( apr_list_node_t** ) &work_item ) );
          if ( rc ) return;

          if ( ( work_item->packet == NULL ) ||
               ( work_item->pkt_type == CVA_WORK_ITEM_PKT_TYPE_NONE ) )
          {
            CVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
          }

          MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "cva_task_process_gating_work_items(): VS_GATING_CMD_STATE_ENUM_FETCH" );

          ctrl->packet = work_item->packet;
          ctrl->pkt_type = work_item->pkt_type;
          ctrl->state = CVA_GATING_CMD_STATE_EXECUTE;

          /* Add the vs_work_item_t back to vs_free_work_pkt_q. */
          work_item->packet = NULL;
          work_item->pkt_type = CVA_WORK_ITEM_PKT_TYPE_NONE;
          ( void ) apr_list_add_tail( &cva_free_work_pkt_q, &work_item->link );
       }
       break;

     case CVA_GATING_CMD_STATE_EXECUTE:
     case CVA_GATING_CMD_STATE_CONTINUE:
       {
         if ( ctrl->pkt_type == CVA_WORK_ITEM_PKT_TYPE_CMD )
         {
           cmd_pkt = ( ( cva_cmd_packet_t* ) ctrl->packet );
           request_id = cmd_pkt->cmd_id;
         }
         else if ( ctrl->pkt_type == CVA_WORK_ITEM_PKT_TYPE_EVENT )
         {
           event_pkt = ( ( cva_event_packet_t* ) ctrl->packet );
           request_id = event_pkt->event_id;
         }
         else
         {
           CVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
         }

         /**
          * For Supported request_id, handler should take care of releasing 
          * memory allocated for packets.
          */
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                 "cva_task_process_gating_work_items(): request_id=(0x%08x)",
                 request_id );

         switch ( request_id )
         {
          case ONEX_VOC_EVENT_RQST_START:
            rc = cva_process_onex_vocoder_start_event( ctrl );
            break;

          case ONEX_VOC_EVENT_RQST_STOP:
            rc = cva_process_onex_vocoder_stop_event( ctrl );
            break;

          case VS_COMMON_EVENT_READY:
            rc = cva_process_vs_ready_event( ctrl );
            break;

          case VS_COMMON_EVENT_NOT_READY:
            rc = cva_process_vs_not_ready_event( ctrl );
            break;

          case CVA_IVOICE_EVENTI_VS_OPEN:
            rc = cva_process_vs_open_event( ctrl );
            break;

          case CVA_IVOICE_EVENTI_VS_CLOSE:
            rc = cva_process_vs_close_event( ctrl );
            break;

          default:
            {
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "cva_task_process_gating_work_items(): Unsupported "
                     "request_id=(0X%08X)", request_id );

              /** For unsupported request_id, memory cleanup required for
               *  CMD/EVENT packets. */
              if( ctrl->pkt_type == CVA_WORK_ITEM_PKT_TYPE_CMD )
              {
                ( void ) cva_free_cmd_packet ( cmd_pkt );
              }

              if( ctrl->pkt_type == CVA_WORK_ITEM_PKT_TYPE_EVENT )
              {
                ( void ) cva_free_event_packet ( event_pkt );
              }

              /* set to VS_EOK to fetch the next command in queue. */
              rc = APR_EOK;
            }
           break;
         }

         /* Evaluate the pending command completion status. */
         switch ( rc )
         {
          case APR_EOK:
            {
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                     "cva_task_process_gating_work_items(): request_id = "
                     "(0X%08X) processed successfully ", request_id );
              ctrl->packet = NULL;
              ctrl->pkt_type = CVA_WORK_ITEM_PKT_TYPE_NONE;
              /* The current command is finished so fetch the next command. */
              ctrl->state = CVA_GATING_CMD_STATE_FETCH;
            }
            break;
         
          case APR_EPENDING:
            /**
             * Assuming the current pending command control routine returns
             * APR_EPENDING the overall progress stalls until one or more
             * external events or responses are received.
             */
            ctrl->state = CVA_GATING_CMD_STATE_CONTINUE;
            /**
             * Return from here so as to avoid unecessary looping till reponse
             * is recived.
             */
            return;
         
          default:
            CVA_PANIC_ON_ERROR( APR_EUNEXPECTED );
            break;
         }
       }
       break;

     default:
      CVA_PANIC_ON_ERROR( rc );
      break;
    }/* switch case ends. */
  }/* for loop ends. */

  return;
}

/****************************************************************************
 * CVA TASK ROUTINES                                                        *
 ****************************************************************************/

CVA_INTERNAL void cva_signal_run ( void )
{
  apr_event_signal( cva_work_event );
}

static int32_t cva_run ( void )
{
  cva_task_process_nongating_work_items( );
  cva_task_process_gating_work_items( );

  return APR_EOK;
}

static int32_t cva_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &cva_work_event );
  apr_event_signal( cva_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( cva_work_event );
    if ( rc ) break;

    cva_run( );
  }

  apr_event_destroy( cva_work_event );
  apr_event_signal( cva_control_event );

  return APR_EOK;
}


/****************************************************************************
 * CVA BOOT-UP and POWER-DOWN ROUTINES                                      *
 ****************************************************************************/

static uint32_t cva_gating_control_init (
  cva_gating_control_t* p_ctrl
)
{
  uint32_t rc = APR_EOK;

  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q,
                         cva_thread_lock_fn, cva_thread_unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  p_ctrl->state = CVA_GATING_CMD_STATE_FETCH;
  p_ctrl->pkt_type = CVA_WORK_ITEM_PKT_TYPE_NONE;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return APR_EOK;
}  /* end of cva_gating_control_init () */

static uint32_t cva_gating_control_destroy (
  cva_gating_control_t* p_ctrl
)
{
  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_list_destroy( &p_ctrl->cmd_q );

  return APR_EOK;
}  /* end of vs_gating_control_destroy () */


/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static int32_t cva_init ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t i;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "cva_init(): Date %s Time %s", __DATE__, __TIME__ );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &cva_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &cva_thread_lock );
    apr_event_create( &cva_control_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &cva_heapmgr, ( ( void* ) &cva_heap_pool ),
                          sizeof( cva_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = cva_object_table;
    params.total_bits = CVA_HANDLE_TOTAL_BITS_V;
    params.index_bits = CVA_HANDLE_INDEX_BITS_V;
    params.lock_fn = cva_int_lock_fn;
    params.unlock_fn = cva_int_unlock_fn;
    rc = apr_objmgr_construct( &cva_objmgr, &params );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &cva_free_work_pkt_q, 
                           cva_int_lock_fn, cva_int_unlock_fn );
    for ( i = 0; i < CVA_NUM_WORK_PKTS_V; ++i )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &cva_work_pkts[i] );
      cva_work_pkts[i].pkt_type = CVA_WORK_ITEM_PKT_TYPE_NONE;
      cva_work_pkts[i].packet = NULL;
      rc = apr_list_add_tail( &cva_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &cva_work_pkts[i] ) );
    }

    rc = apr_list_init_v2( &cva_nongating_work_pkt_q,
                           cva_int_lock_fn, cva_int_unlock_fn );
  }

  { /* Initialize gating work pkt queue. */
    rc = cva_gating_control_init( &cva_gating_work_pkt_q );
  }

  { /* Create the CVA task worker thread. */
    rc = apr_thread_create( &cva_thread, CVA_TASK_NAME, CVA_TASK_PRIORITY,
                            cva_task_stack, CVA_TASK_STACK_SIZE, 
                            cva_worker_thread_fn , NULL );
    CVA_PANIC_ON_ERROR( rc );

    apr_event_wait( cva_control_event );
  }
  cva_is_initialized = TRUE;

  return rc;
}

static int32_t cva_postinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;
  bool_t ret_val;

  cm_client_id_type cva_client_id = (cm_client_id_type)  -1;
  for ( index =0; index < CVA_MAX_NUM_OF_SESSIONS_V; index++ )
  {
    /* Initialize the CVA session object. */
    rc = cva_create_session_object( &cva_session_list[ index ] );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &cva_session_lock_list[ index ] );
    CVA_PANIC_ON_ERROR( rc );

    /* CVA OPEN's ONEX voice instance for all available ASID by CM. */
    cva_session_list[ index ]->asid = ( sys_modem_as_id_e_type ) index;
    cva_open_onex_session( cva_session_list[ index ] );

    if ( cva_is_va_supported == FALSE )
    {
      cva_session_list[ index ]->is_resources_granted = TRUE;
    }
  }
  rc = cm_client_init( CM_CLIENT_TYPE_VA, &cva_client_id );

  if ( rc == CM_CLIENT_OK )
  {
    rc = cm_client_subs_reg(cva_client_id, cva_subs_info_cb);
    if ( rc != CM_CLIENT_OK ) 
    {
       CVA_REPORT_FATAL_ON_ERROR( rc );
    }
  }
  else
  {
    CVA_REPORT_FATAL_ON_ERROR( rc );
  }
  
  index = 0;
  while ( index < CVA_MAX_NUM_OF_SESSIONS_V )
  {
    ret_val = cm_ph_cmd_get_subs_pref_info( NULL, NULL, cva_client_id,
                                              ( sys_modem_as_id_e_type ) index );
    if( ret_val == FALSE ) CVA_REPORT_FATAL_ON_ERROR( APR_ENORESOURCE );

    index++;
  }

  return rc;
}


static int32_t cva_predeinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;

  for ( index =0; index < CVA_MAX_NUM_OF_SESSIONS_V; index++ )
  {
    if ( cva_is_va_supported == FALSE )
    {
      cva_session_list[ index ]->is_resources_granted = FALSE;
      cva_session_list[ index ]->vsid = SYS_MODEM_AS_ID_NONE;
      rc = cva_vs_close_session( cva_session_list[ index ], NULL );
    }

    /* CVA CLOSE's ONEX voice instance for all available ASID. */
    rc = cva_close_onex_session ( cva_session_list[ index ] );

    /* re-initialize the CVA session object. */
    rc = cva_mem_free_object( ( cva_object_t* ) cva_session_list[ index ] );
  }

  return rc;
}


static int32_t cva_deinit ( void )
{
  uint32_t rc = APR_EOK;

  cva_is_initialized = FALSE;

  apr_event_signal_abortall( cva_work_event );
  apr_event_wait( cva_control_event );


  /* Release gating control structures */
  ( void ) cva_gating_control_destroy( &cva_gating_work_pkt_q );

  /* Release work queue */
  ( void ) apr_list_destroy( &cva_free_work_pkt_q );
  ( void ) apr_list_destroy( &cva_nongating_work_pkt_q );


  /* Deinitialize the object handle table. */
  apr_objmgr_destruct( &cva_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  apr_event_destroy( cva_control_event );
  apr_lock_destroy( cva_int_lock );
  apr_lock_destroy( cva_thread_lock );

  return rc;
}


CVA_EXTERNAL uint32_t cva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;

  switch ( cmd_id )
  {
  case DRV_CMDID_INIT:
    rc = cva_init( );
    break;

  case DRV_CMDID_POSTINIT:
    rc = cva_postinit( );
    break;

  case DRV_CMDID_PREDEINIT:
    rc = cva_predeinit( );
    break;

  case DRV_CMDID_DEINIT:
    rc = cva_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

void cva_task
(
  dword ignored
)
{
  uint32_t rc = APR_EOK;
  rc = cva_call( DRV_CMDID_INIT, NULL, 0 );
  rc = cva_call( DRV_CMDID_POSTINIT, NULL, 0 );

  CVA_PANIC_ON_ERROR( rc );

  return;
}

