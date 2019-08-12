/*
  Copyright (C) 2009 - 2015 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvs_module.c#1 $
  $Author: mplcsds1 $
*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/15   aim     Removed support for CDMA in MVS adaptor.
08/27/14   qc      Added feature: MVS adapter to VS. 
06/011/14  shr     Removed the state_handover from the MVS state machine.
                   Sending MVM stop command during handovers instead on MVM standby.
05/26/14   aim     Modified timing params for G, W and TD.
03/03/13   js      Added support for generating silence frame on tx side for WCDMA
                   and TDSCDMA.
02/10/14   aim     Modified thread priority.
12/09/13   js/sre  Added support for clearing the CVD sessions, after the end of 
                   each call. Code clean up, with added error check for accquire,
                   enable, standby and release. 
11/8/13   sre      Renamed the GUID VOICECFG_CMDID_GET_BOOL_ITEM to
                   VOICECFG_CMD_GET_BOOL_ITEM for consistency with VS commands.
10/31/13   sre     Fixed vocoder packet size calculation in
                   mvs_process_dl_fr_packet function.
09/20/13   sre     Fixed the banned QPSI memcpy and memmove APIs.
09/17/13   aim     Modified timing parameters for G,W & 1X to save on RTD.
06/29/13    sg     Fixed the banned QPSI memcpy APIs.
06/28/13    hl     Fixed the error handling code for handling APR communication
                   errors, in order to prevent crashes in MVS due to APR
                   communication errors.
06/13/13   rajatm  commented the usedbytes message as apr_memmgr uses memheap2.
06/12/13   sud     Merged the missing SSR phase3 changes.
06/10/13    sg     Merged the missing SSR phase3 changes.
06/01/13   sud     Fixed compiler warnings.
05/21/13    sg     Send sample rate notification ( 8 KHz ) to NAS/GHDI for AMR
05/21/13    sg     Added eAMR mode change detection
05/16/13   sud     Fixed compiler warnings.
07/10/12    ak     Added SSR phase 3 support.
04/30/12    qc     Removed the usage of MVS_IST_PRIO_ORDER.
04/24/12    nrc    Event call_back functions are not released properly during
                   call tear down.
04/11/12    hl     Added eAMR support.
03/13/12    at     Merged in TD-SCDMA support.
02/09/12    sb     Remove MVS timing parameters change and add RELEASE fix for SSR
11/02/11    ji     Make MVS timing parameters per network per vocoder type.
08/11/11    nrc    Fixes for Klocwork and compiler warnings.
07/11/11    hl     Added SSR phase 2+ support.
31/3/10     hl     Added SO73 support. Updated set timewarp for wideband vocoders.
10/10/10    jk     Added Featurized timing analysis code
03/11/10    sr     Sending MVS_MODE_NONE on release()
03/03/10    sr     Fixed a bug in MVS release();
02/25/10    jk     fixed CDMA only compilation issue
02/10/10    jk     Changed VOIP client to vfr mode none (need for FW changes)
02/03/10    sr     Support for MVS_MODE_NONE and Re-sync ctm bug fix.
01/27/10    jk     Added g722 support
11/18/09    jk     Fixed dsm activate sequencing problem

==========================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include <string.h>
#include "err.h"
#include "log.h"
#include "log_codes_umts.h"
#ifndef WINSIM
#include "blast.h"
#include "npa.h"
#endif /* !WINSIM */
#include "mmstd.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_misc.h"
#include "apr_lock.h"
#include "apr_memmgr.h"
#include "apr_objmgr.h"
#include "vocsvc_nv_api.h"
#include "voicecfg_api.h"
#include "voice_util_api.h"
#include "vs_voc_private_if.h"
#include "mvs.h"
#include "mvs_module_api.h"
#include "mvs_module.h"
#include "mvs_voc_def.h"
#include "mvsdsm.h"
#include "mvsamr.h"
#include "mvs_umts_chan_setup.h"
#include "mvs_log_packet.h"
#include "vs_task.h"

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
  /* Used for timing analysis. */
  #include "timetick.h"
  /*#include "clk.h"*/
  /*#include "stmr.h"*/
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

#define USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK
#ifdef USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK
  #include "queue.h"
#endif /* USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK */
#include "wcdmamvsif.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define MVS_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

#define MVS_MAX_JOBS_V ( 32 ) 
  /**< Maximum number of jobs which can be allocated at a time. */
#define MVS_CFGID_MVS_EVENT_SEQUENCE ( 0x00012EF5 )

#ifdef MVS_PROFILING_ENABLED
#define MVS_PROFILING_BUF_NUM_ENTRIES ( 1000 )
#endif /* MVS_PROFILING_ENABLED */

/* VSID for MVS */
#define MVS_VSID_CS_VOICE_MULTIMODE ( 0x10C01000 )

/* Indicate unsupported mode. */
#define MVS_UNSUPPORTED_MODE ( ( uint32_t ) -1 )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef enum mvs_thread_state_enum_t
{
  MVS_THREAD_STATE_ENUM_INIT,
  MVS_THREAD_STATE_ENUM_READY,
  MVS_THREAD_STATE_ENUM_EXIT
}
  mvs_thread_state_enum_t;

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
/* Logging struct. */
typedef struct 
{
  timetick_type ul_deliver_delay_max;
  timetick_type ul_processing_delay_max;
  uint32_t      ul_missed_slow_count;
  uint32_t      ul_missed_fast_count;
  timetick_type dl_deliver_delay_max;
  timetick_type dl_processing_delay_max;
  uint32_t      dl_missed_slow_count;
  uint32_t      dl_missed_fast_count;
} mvs_timing_info_type;

static mvs_timing_info_type  mvs_timing_info;   
static timetick_type mvs_ul_prev = 0;
static timetick_type mvs_dl_prev = 0;
static bool_t   mvs_debug_timing = 1;
static uint32_t mvs_timing_start_cnt_ul = 0;
static uint32_t mvs_timing_start_cnt_dl = 0;

static uint32_t mvs_timing_start_delay = 200;

/* For WCDMA timing. */
/* uint32_t mvs_curr_stmr_time_cx8;*/
/* STMR_STATUS_DUMP(STMR_STATUS_DUMP_CMD_IMMEDIATE);
mvs_curr_stmr_time_cx8 = STMR_GET_TX_FRAME_COUNT_STATUS(); */
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

typedef struct mvs_generic_item_t
{
  apr_list_node_t link;

  uint32_t handle;
    /**< Any custom handle. */
}
  mvs_generic_item_t;

#ifdef MVS_PROFILING_ENABLED
/* For code profiling. */
typedef struct
{
  uint64_t pcycle;
  uint32_t cpu_clk;
  uint32_t thread_id;
  mvs_profiling_tap_point_t tap_point;
 
  /* Profile code itself measurement. */
  uint64_t t0_ref;
  uint64_t t1_pcycle;
  uint64_t t2_cpu_clk;
  uint64_t t3_thread_id;
}
  mvs_profiling_buf_entry;
#endif /* MVS_PROFILING_ENABLED */

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/* Flag that prevents mvs from running before basic initializtion has been done.
 */
static bool_t mvs_is_initialized = FALSE;

static uint8 mvs_MaskArray[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

static mvs_client_state_type mvs_client_table[ MVS_CLIENT_MAX ];
static mvs_client_type mvs_cur_client = MVS_CLIENT_NONE;

static apr_memmgr_type mvs_heapmgr;
static uint8_t mvs_heap_pool[ MVS_HEAP_SIZE ];

static mvs_work_item_t mvs_cmd_pool[ MVS_NUM_COMMANDS ];
static apr_list_t mvs_free_cmd_q;
static apr_list_t mvs_incoming_cmd_q;
static apr_list_t mvs_pending_cmd_q;

static apr_lock_t mvs_int_lock;

/* Thread Management. */
static apr_event_t mvs_ist_event;
static apr_thread_t mvs_ist_handle;
static mvs_thread_state_enum_t mvs_ist_state = MVS_THREAD_STATE_ENUM_INIT;
static uint8_t mvs_task_stack[ MVS_TASK_STACK_SIZE ];

/* Cached MVS vocoder properties. */
typedef struct{
  mvs_voc_rate_type max_voc_rate;
  mvs_voc_rate_type min_voc_rate;
  mvs_dtx_mode_type dtx_mode;
  uint16_t          rate_factor;
  mvs_amr_mode_type amr_ul_mode;
  mvs_amr_mode_type awb_ul_mode;
} mvs_vocoder_properties;

static mvs_vocoder_properties mvs_curr_props, mvs_new_props;

/* Session Management. */
static mvs_pending_control_t mvs_pending_ctrl;
static apr_objmgr_object_t mvs_object_table[ MVS_MAX_OBJECTS ];
static apr_objmgr_t mvs_objmgr;

#ifndef WINSIM
/* WCDMA Channels. */
extern mvssup_umts_chan_state_type mvs_ul_umts_chan_state;
extern mvssup_umts_chan_state_type mvs_dl_umts_chan_state;
#endif /* !WINSIM */

/* G711 Variables. */
static mvs_g711_mode_type mvs_current_g711_mode;

/* eAMR config. */
static vocsvc_nv_eamr_config_t mvs_eamr_config;


#ifdef MVS_PROFILING_ENABLED
static npa_query_handle mvs_npa_query_handle;
static bool_t mvs_profiling_tap_point_enable_table[ MVS_PROFILING_TAP_POINT_MAX_LEAVE ];
static apr_lock_t mvs_profiling_lock;
static uint32_t mvs_profiling_buf_index = -1;
static uint32_t mvs_profiling_cur_clk = 0;
static mvs_profiling_buf_entry mvs_profiling_buffer[ MVS_PROFILING_BUF_NUM_ENTRIES ];
#endif /* MVS_PROFILING_ENABLED */

/* Number of bytes required for the packet for EVRC.
 */
static uint32 mvslegacy_evrc_packet_size[ ] = {
  0,                                      /* Blank        */
  2,                                      /* Eighth rate  */
  0,                                      /* Quarter rate */
  10,                                     /* Half rate    */
  22,                                     /* Full rate    */
  0                                       /* Erasure      */
};

/* Number of bytes required for the packet for 13K.
 */
static uint32 mvslegacy_13k_packet_size[] = {
  0,                                      /* Blank        */
  3,                                      /* Eighth rate  */
  7,                                      /* Quarter rate */
  16,                                     /* Half rate    */
  34,                                     /* Full rate    */
  0                                       /* Erasure      */
};

/* Number of bytes required for the packet for 4GV.
 */
static uint32 mvslegacy_4gv_packet_size[] = {
  0,                                      /* Blank        */
  2,                                      /* Eighth rate  */
  5,                                      /* Quarter rate */
  10,                                     /* Half rate    */
  22,                                     /* Full rate    */
  0                                       /* Erasure      */
};

/* Number of bytes required for the packet for 4GV-WB.
 */
static uint32 mvslegacy_4gv_WB_packet_size[] = {
  0,                                      /* Blank        */
  2,                                      /* Eighth rate  */
  5,                                      /* Quarter rate */
  10,                                     /* Half rate    */
  22,                                     /* Full rate    */
  0                                       /* Erasure      */
};

/* Number of bytes required for the packet for 4GV-NW
 */
static uint32 mvslegacy_4gv_NW_packet_size[] = {
  0,                                      /* Blank        */
  2,                                      /* Eighth rate  */
  5,                                      /* Quarter rate */
  10,                                     /* Half rate    */
  22,                                     /* Full rate    */
  0,                                      /* Erasure      */
  2                                       /* Non critical eighth rate */
};

/* Frame type mapping for AMR-NB vocoder over the downlink path,
 * from MVS to VS.
 */
vs_vocamr_frame_type_t mvs_to_vs_amr_frame_table[ MVS_AMR_FRAME_TYPE_MAX ] = 
{
  VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD,     /* MVS_AMR_SPEECH_GOOD */
  VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED, /* MVS_AMR_SPEECH_DEGRADED */
  VS_VOCAMR_FRAME_TYPE_ONSET,           /* MVS_AMR_ONSET */
  VS_VOCAMR_FRAME_TYPE_SPEECH_BAD,      /* MVS_AMR_SPEECH_BAD */
  VS_VOCAMR_FRAME_TYPE_SID_FIRST,       /* MVS_AMR_SID_FIRST */
  VS_VOCAMR_FRAME_TYPE_SID_UPDATE,      /* MVS_AMR_SID_UPDATE */
  VS_VOCAMR_FRAME_TYPE_SID_BAD,         /* MVS_AMR_SID_BAD */
  VS_VOCAMR_FRAME_TYPE_NO_DATA,         /* MVS_AMR_NO_DATA */
  VS_VOCAMR_FRAME_TYPE_NO_DATA          /* MVS_AMR_SPEECH_LOST */ 
};

/* Frame type mapping for AMR-NB vocoder over the uplink path,
 * from VS to MVS.
 */
mvs_amr_frame_type vs_to_mvs_amr_frame_type[ VS_VOCAMR_FRAME_TYPE_NO_DATA + 1 ] =
{
  MVS_AMR_SPEECH_GOOD,     /* VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD */
  MVS_AMR_SPEECH_DEGRADED, /* VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED */
  MVS_AMR_ONSET,           /* VS_VOCAMR_FRAME_TYPE_ONSET */
  MVS_AMR_SPEECH_BAD,      /* VS_VOCAMR_FRAME_TYPE_SPEECH_BAD */
  MVS_AMR_SID_FIRST,       /* VS_VOCAMR_FRAME_TYPE_SID_FIRST */
  MVS_AMR_SID_UPDATE,      /* VS_VOCAMR_FRAME_TYPE_SID_UPDATE */
  MVS_AMR_SID_BAD,         /* VS_VOCAMR_FRAME_TYPE_SID_BAD */
  MVS_AMR_NO_DATA          /* VS_VOCAMR_FRAME_TYPE_NO_DATA */
};

/* Frame type mapping for AMR-WB vocoder over the downlink path,
 * from MVS to VS.
 */
vs_vocamrwb_frame_type_t mvs_to_vs_armwb_frame_table[ MVS_AMR_FRAME_TYPE_MAX ] =
{
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD,              /* MVS_AMR_SPEECH_GOOD */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED, /* MVS_AMR_SPEECH_DEGRADED */
  VS_VOCAMRWB_FRAME_TYPE_NO_DATA,                  /* MVS_AMR_ONSET */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD,               /* MVS_AMR_SPEECH_BAD */
  VS_VOCAMRWB_FRAME_TYPE_SID_FIRST,                /* MVS_AMR_SID_FIRST */
  VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE,               /* MVS_AMR_SID_UPDATE */
  VS_VOCAMRWB_FRAME_TYPE_SID_BAD,                  /* MVS_AMR_SID_BAD */
  VS_VOCAMRWB_FRAME_TYPE_NO_DATA,                  /* MVS_AMR_NO_DATA */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST               /* MVS_AMR_SPEECH_LOST */
};

/* Frame type mapping for AMR-WB vocoder over the uplink path,
 * from VS to MVS.
 */
mvs_amr_frame_type vs_to_mvs_amrwb_frame_type[ VS_VOCAMRWB_FRAME_TYPE_NO_DATA + 1 ] = 
{
  MVS_AMR_SPEECH_GOOD,     /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD */
  MVS_AMR_SPEECH_DEGRADED, /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED */
  MVS_AMR_SPEECH_LOST,     /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST */
  MVS_AMR_SPEECH_BAD,      /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD */
  MVS_AMR_SID_FIRST,       /* VS_VOCAMRWB_FRAME_TYPE_SID_FIRST */
  MVS_AMR_SID_UPDATE,      /* VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE */
  MVS_AMR_SID_BAD,         /* VS_VOCAMRWB_FRAME_TYPE_SID_BAD */
  MVS_AMR_NO_DATA          /* VS_VOCAMRWB_FRAME_TYPE_NO_DATA */
};

/* Silence frame generation.
*/

/* Event callback function for call start/stop notification. */
static mvs_event_fn_t mvs_client_event_cb = NULL;

/* W and TDS timing offset. They should be queried from VS. */
static mvs_timing_offset_type mvs_wcdma_timing_offset = { 0, 3300, 6300 };
static mvs_timing_offset_type mvs_tdscdma_timing_offset = { 8000, 3300, 8300 };

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

static int32_t mvs_free_object (
  mvs_object_t* object
);

static uint32_t mvs_prime_vs_read_buf(
  uint32_t handle,
  vs_voc_buffer_t* buffer
);

static uint32_t mvs_write_vs_buffer_through (
  void* frame_info,
  uint8_t* vocoder,
  uint32_t vocoder_size
);

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void mvs_int_lock_fn ( void )
{
  ( void ) apr_lock_enter( mvs_int_lock );
}

static void mvs_int_unlock_fn ( void )
{
  ( void ) apr_lock_leave( mvs_int_lock );
}

static void mvs_signal_run ( void )
{
#ifdef USE_SINGLE_AUDSVC_THREAD
  APR_INTERNAL void vocsvc_signal_task ( void );

  vocsvc_signal_task( );
#else
  apr_event_signal( mvs_ist_event );
#endif /* USE_SINGLE_AUDSVC_THREAD */
}

static int32_t mvs_get_object (
  uint32_t handle,
  mvs_object_t** ret_obj
)
{
  int32_t rc;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_objmgr_find_object( &mvs_objmgr, handle, &objmgr_obj );
  if ( rc )
  {
    return APR_EFAILED;
  }

  *ret_obj = ( ( mvs_object_t* ) objmgr_obj->any.ptr );

  return APR_EOK;
}

/****************************************************************************
 * MVS OBJECT CREATION AND DESTRUCTION ROUTINES                             *
 ****************************************************************************/

int32_t mvs_mem_alloc_object (
  uint32_t size,
  mvs_object_t** ret_object
)
{
  int32_t rc;
  mvs_object_t* mvs_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the MVS object. */
    mvs_obj = apr_memmgr_malloc( &mvs_heapmgr, size );
    if ( mvs_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mem_alloc_object(): Out " \
              "of memory, requested size (%d)", size);
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the MVS object. */
    rc = apr_objmgr_alloc_object( &mvs_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mem_alloc_object(): Out " \
              "of objects, rc = (0x%08X)", rc);
      apr_memmgr_free( &mvs_heapmgr, mvs_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = mvs_obj;

    /* Initialize the base MVS object header. */
    mvs_obj->header.handle = objmgr_obj->handle;
    mvs_obj->header.type = MVS_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = mvs_obj;

  return APR_EOK;
}

static int32_t mvs_mem_free_object (
  mvs_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &mvs_objmgr, object->header.handle );
  apr_memmgr_free( &mvs_heapmgr, object );

  return APR_EOK;
}

static int32_t mvs_create_simple_job_object (
  uint32_t parentjob_handle,
  mvs_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  mvs_simple_job_object_t* mvs_obj;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvs_mem_alloc_object( sizeof( mvs_simple_job_object_t ), ( ( mvs_object_t** ) &mvs_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    mvs_obj->header.type = MVS_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    mvs_obj->parentjob_handle = parentjob_handle;
    mvs_obj->is_completed = 0;
  }

  *ret_job_obj = mvs_obj;

  return APR_EOK;
}

static int32_t mvs_create_sequencer_job_object (
  mvs_sequencer_job_object_t** ret_job_obj
)
{
  int32_t rc;
  mvs_sequencer_job_object_t* job_obj;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvs_mem_alloc_object( sizeof( mvs_sequencer_job_object_t ),
                             ( ( mvs_object_t** ) &job_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the pending job object. */
    job_obj->header.type = MVS_OBJECT_TYPE_ENUM_SEQUENCER_JOB;

    job_obj->state = APR_NULL_V;
    job_obj->subjob_obj = NULL;
    job_obj->status = APR_UNDEFINED_ID_V;
  }

  *ret_job_obj = job_obj;

  return APR_EOK;
}

static int32_t mvs_free_object (
  mvs_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Perform object-specific clean-up necessary. */

  /* Free the object memory and object handle. */
  ( void ) mvs_mem_free_object( object );

  return APR_EOK;
}

static int32_t mvs_mem_alloc_cmd (
  uint32_t size,
  mvs_cmd_t** ret_cmd
)
{
  int32_t rc;
  mvs_cmd_t* cmd;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  { 
    cmd = ( mvs_cmd_t* ) apr_memmgr_malloc( &mvs_heapmgr, size );
    if ( cmd == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mem_alloc_cmd(): Out " \
             "of memory, requested size (%d)", size);
      return APR_ENORESOURCE;
    }

    rc = apr_objmgr_alloc_object( &mvs_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mem_alloc_cmd(): Out " \
             "of objects, rc = (0x%08x)", rc);
      apr_memmgr_free( &mvs_heapmgr, cmd );
      return APR_ENORESOURCE;
    }

    objmgr_obj->any.ptr = cmd;

    cmd->header.handle = objmgr_obj->handle;
    cmd->header.type = MVS_CMD_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_cmd = cmd;

  return APR_EOK;
}

static int32_t mvs_mem_free_cmd (
  mvs_cmd_t* cmd
)
{
  uint32_t handle;

  if ( cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  handle = cmd->header.handle;
  ( void ) apr_objmgr_free_object( &mvs_objmgr, cmd->header.handle );
  apr_memmgr_free( &mvs_heapmgr, cmd );

  return APR_EOK;
}

/****************************************************************************
 * MVS WORK QUEUE ROUTINES                                                  *
 ****************************************************************************/

static int32_t mvs_send_mode_event (
  mvs_client_type client,
  mvs_mode_status_type status,
  mvs_mode_type voc_mode
)
{
  mvs_event_type event_data;

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_mode_event(): Invalid " \
           "client (%d)", client );
    return APR_EBADPARAM;
  }

  if ( mvs_client_table[client].event_cb != NULL )
  {
    event_data.mode.hdr.event = MVS_EV_MODE;
    event_data.mode.hdr.client = client;
    event_data.mode.mode_status = status;
    event_data.mode.mode = voc_mode;
    mvs_client_table[client].event_cb( &event_data );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_mode_event(): " \
           "client = (%d), event_cb=NULL", client );
    return APR_EFAILED;
  }

  return APR_EOK;
}

static int32_t mvs_send_cmd_event (
  mvs_client_type client,
  mvs_cmd_status_type status
)
{
  mvs_event_type event_data;

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_cmd_event(): " \
           "Invalid client = (%d)", client );
    return APR_EBADPARAM;
  }

  if ( mvs_client_table[client].event_cb != NULL )
  {
    event_data.cmd.hdr.event = MVS_EV_COMMAND;
    event_data.cmd.hdr.client = client;
    event_data.cmd.cmd_status = status;
    mvs_client_table[client].event_cb( &event_data );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_cmd_event(): " \
            "client = (%d), event_cb = NULL", client );
    return APR_EFAILED ;
  }

  return APR_EOK;
}

static int32_t mvs_send_eamr_sr_event (
  mvs_client_type client,
  mvs_eamr_sr_type sr
)
{
  mvs_event_type event_data;

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_eamr_sr_event(): " \
           "Invalid client = (%d)", client );
    return APR_EBADPARAM;
  }

  if ( mvs_client_table[client].event_cb != NULL )
  {
    event_data.eamr.hdr.event = MVS_EV_EAMR_SR;
    event_data.eamr.hdr.client = client;
    event_data.eamr.sr = sr;
    mvs_client_table[client].event_cb( &event_data );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_eamr_sr_event(): " \
            "client = (%d), event_cb = NULL", client );
    return APR_EFAILED;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,  "mvs_send_eamr_sr_event(): " 
         "client = (%d), sample_rate = (%d)", client, sr);
  return APR_EOK;

}

static void mvs_queue_pending_cmd (
  mvs_cmd_t* cmd
)
{
  int32_t rc;
  mvs_work_item_t* work_item;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_queue_pending_cmd(): " \
         "handle = (0x%08x), type = (0x%08x)",
         cmd->header.handle, cmd->header.type );

  for ( ;; )
  {
    if ( mvs_is_initialized == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_queue_pending_cmd(): " \
           "Recieved a command before being properly initialized" );

      mvs_mem_free_cmd( cmd );
      break;
    }

    { /* Get a free command structure. */
      rc = apr_list_remove_head( &mvs_free_cmd_q, ( ( apr_list_node_t** ) &work_item ) );
      if ( rc )
      { /* No free command structure is available. */
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_queue_pending_cmd(): " \
             "Ran out of free pending command queues" );
        mvs_mem_free_cmd( cmd );
        break;
      }
    }
    { /* Queue the incoming command to the pending command queue. We don't
       * need to signal do work because after the incoming command
       * handler is done the pending command handler routine will be
       * called.
       */
      work_item->cmd = cmd;
      ( void ) apr_list_add_tail( &mvs_pending_cmd_q, &work_item->link );
      mvs_signal_run( );
    }

    return;
  }

  /* Not reporting the error. */
}

/* Returns TRUE if successful and FALSE if unsuccessful. */
static bool_t mvs_queue_incoming_cmd (
  mvs_cmd_t* cmd
)
{
  int32_t rc;
  mvs_work_item_t* work_item;

  mvs_profiling_save_entry( MVS_PROFILING_QUEUE_IN_PKT_ENTER );

  mvs_profiling_save_entry( MVS_PROFILING_IN_PKT_MSG_ENTER );  
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvs_queue_incoming_cmd(): " \
        "handle = (0x%08X), type = (0x%08X)",
        cmd->header.handle, cmd->header.type );
  mvs_profiling_save_entry( MVS_PROFILING_IN_PKT_MSG_LEAVE );

  //TODO: Fix INIT in mvs_enable()
  //TODO: READY/NOT_READY should check the mvs_cur_client and state, no need to check anything else

  for ( ;; )
  {
    if ( mvs_is_initialized == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_queue_incoming_cmd(): " \
           "Recieved a command before initialization");
      mvs_mem_free_cmd( cmd );
      break;
    }

    { /* Get a free command structure. */
      mvs_profiling_save_entry( MVS_PROFILING_REMOVE_HEAD_ENTER );
      rc = apr_list_remove_head( &mvs_free_cmd_q, ( ( apr_list_node_t** ) &work_item ) );
      mvs_profiling_save_entry( MVS_PROFILING_REMOVE_HEAD_LEAVE );
      if ( rc )
      { /* No free command structure is available. */
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_queue_incoming_cmd(): " \
             "MVS ran out of free incoming command queues" );
        mvs_mem_free_cmd( cmd );
        break;
      }
    }
    { /* Queue the new command to the incoming command queue and signal do
       * work.
       */
      work_item->cmd = cmd;

      mvs_profiling_save_entry( MVS_PROFILING_ADD_TAIL_ENTER );
      ( void ) apr_list_add_tail( &mvs_incoming_cmd_q, &work_item->link );
      mvs_profiling_save_entry( MVS_PROFILING_ADD_TAIL_LEAVE );

      mvs_profiling_save_entry( MVS_PROFILING_SIGNAL_RUN_ENTER );
      mvs_signal_run( );
      mvs_profiling_save_entry( MVS_PROFILING_SIGNAL_RUN_LEAVE );

      mvs_profiling_save_entry( MVS_PROFILING_QUEUE_IN_PKT_LEAVE );
    }

    return TRUE;
  }

  return FALSE;
}

static int32_t mvs_make_cmd (
  mvs_packet_type* mvs_cmd,
  mvs_cmd_t** ret_cmd
)
{
  int32_t rc;
  mvs_cmd_t* cmd;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvs_mem_alloc_cmd( sizeof( mvs_cmd_t ), ( ( mvs_cmd_t** ) &cmd ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  cmd->header.type = MVS_CMD_TYPE_ENUM_INTERNAL_CMD;
  mmstd_memcpy( &cmd->internal_cmd.type, sizeof( cmd->internal_cmd.type ), 
                mvs_cmd, sizeof( *mvs_cmd ) );

  *ret_cmd = cmd;

  return APR_EOK;
}

static int32_t mvs_make_vs_cmd_rsp (
  vs_common_event_cmd_response_t* cmd_rsp,
  mvs_cmd_t** ret_cmd
)
{
  int32_t rc;
  mvs_vs_cmd_rsp_t* vs_cmd_rsp;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvs_mem_alloc_cmd( sizeof( mvs_cmd_t ), ( ( mvs_cmd_t** ) &vs_cmd_rsp ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }
  
  vs_cmd_rsp->header.type = MVS_CMD_TYPE_ENUM_VS_CMD_RSP;
  vs_cmd_rsp->cmd_id = cmd_rsp->cmd_id;
  vs_cmd_rsp->status_id = cmd_rsp->status_id;
  vs_cmd_rsp->context = cmd_rsp->client_context;

  *ret_cmd = ( mvs_cmd_t* ) vs_cmd_rsp;

  return APR_EOK;
}

static int32_t mvs_make_vs_evt (
  uint32_t evt_id,
  void* session_context,
  mvs_cmd_t** ret_cmd
)
{
  int32_t rc;
  mvs_vs_evt_t* vs_evt;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvs_mem_alloc_cmd( sizeof( mvs_cmd_t ), ( ( mvs_cmd_t** ) &vs_evt ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  vs_evt->header.type = MVS_CMD_TYPE_ENUM_VS_EVT;
  vs_evt->evt_id = evt_id;
  vs_evt->context = session_context;
 
  *ret_cmd = ( mvs_cmd_t* ) vs_evt;

  return APR_EOK;
}

static int32_t mvs_make_vs_eamr_mode (
  vs_common_event_eamr_mode_t* eamr_mode,
  void* session_context,
  mvs_cmd_t** ret_cmd
)
{
  int32_t rc;
  mvs_vs_eamr_mode_t* vs_eamr_mode;

  if ( ret_cmd == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvs_mem_alloc_cmd( sizeof( mvs_cmd_t ), ( ( mvs_cmd_t** ) &vs_eamr_mode ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }
  
  vs_eamr_mode->header.type = MVS_CMD_TYPE_ENUM_VS_EAMR_MODE;
  vs_eamr_mode->mode = eamr_mode->mode;
  vs_eamr_mode->context = session_context;

  *ret_cmd = ( mvs_cmd_t* ) vs_eamr_mode;

  return APR_EOK;
}

/****************************************************************************
 * MVS CORE IMPLEMENTATION                                                  *
 ****************************************************************************/

static uint32_t mvs_map_vs_media_id (
  mvs_mode_type voc_mode
)
{
  uint32_t media_id;

  switch ( voc_mode )
  {
  default:
  case MVS_MODE_NONE:
    media_id = MVS_UNSUPPORTED_MODE;
    break;

  case MVS_MODE_LINEAR_PCM:/* Not supported. */
    media_id = MVS_UNSUPPORTED_MODE;
    break;

  case MVS_MODE_IS733:
    media_id = VS_COMMON_MEDIA_ID_QCELP13K;
    break;

  case MVS_MODE_IS127:
    media_id = VS_COMMON_MEDIA_ID_EVRCA;
    break;

  case MVS_MODE_4GV_NB:
    media_id = VS_COMMON_MEDIA_ID_EVRCB;
    break;

  case MVS_MODE_4GV_WB:
    media_id = VS_COMMON_MEDIA_ID_EVRCWB;
    break;

  case MVS_MODE_4GV_NW:
    media_id = VS_COMMON_MEDIA_ID_EVRCNW;
    break;

  case MVS_MODE_AMR:
    media_id = VS_COMMON_MEDIA_ID_AMR;
    break;

  case MVS_MODE_AMR_WB:
    media_id = VS_COMMON_MEDIA_ID_AMRWB;
    break;

  case MVS_MODE_EFR:
    media_id = VS_COMMON_MEDIA_ID_EFR;
    break;

  case MVS_MODE_FR:
    media_id = VS_COMMON_MEDIA_ID_FR;
    break;

  case MVS_MODE_HR:
    media_id = VS_COMMON_MEDIA_ID_HR;
    break;

  case MVS_MODE_G711:/* Not supported. */
    media_id = MVS_UNSUPPORTED_MODE;
    break;  

  case MVS_MODE_G711A:
    media_id = ( ( mvs_current_g711_mode == MVS_G711_MODE_MULAW ) ?
                 VS_COMMON_MEDIA_ID_G711MULAW :
                 VS_COMMON_MEDIA_ID_G711ALAW );
    break;

  case MVS_MODE_G722:/* Not supported. */
    media_id = MVS_UNSUPPORTED_MODE;
    break;

  case MVS_MODE_G729A:
    media_id = VS_COMMON_MEDIA_ID_G729;
    break;
  }

  return media_id;
}

boolean mvs_mod_mode_supported (
  mvs_mode_type mode
)
{
  switch ( mode )
  {
  case MVS_MODE_IS733:
  case MVS_MODE_IS127:
  case MVS_MODE_4GV_NB:
  case MVS_MODE_4GV_WB:
  case MVS_MODE_4GV_NW:
  case MVS_MODE_AMR:
  case MVS_MODE_AMR_WB:
  case MVS_MODE_EFR:
  case MVS_MODE_FR:
  case MVS_MODE_HR:
  case MVS_MODE_G711A:
  case MVS_MODE_G729A:
    return TRUE;

  case MVS_MODE_LINEAR_PCM:
  case MVS_MODE_G711:
  case MVS_MODE_G722:
  default:
    return FALSE;
  }
}

mvs_amr_mode_type mvs_get_ul_amr_mode ( void )
{
  return mvs_curr_props.amr_ul_mode;
}

mvs_amr_mode_type mvs_get_ul_awb_mode ( void )
{
  return mvs_curr_props.awb_ul_mode;
}

static uint32_t mvs_map_evrca_frame_rate (
  mvs_voc_rate_type in_rate,
  vs_vocevrca_frame_rate_t* out_rate
)
{
  if ( out_rate == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( in_rate )
  {
  case MVS_VOC_0_RATE:
    *out_rate = VS_VOCEVRCA_FRAME_BLANK_RATE;
    break;
  case MVS_VOC_8_RATE:
    *out_rate = VS_VOCEVRCA_FRAME_EIGHTH_RATE;
    break;
  case MVS_VOC_2_RATE:
    *out_rate = VS_VOCEVRCA_FRAME_HALF_RATE;
    break;
  case MVS_VOC_1_RATE:
    *out_rate = VS_VOCEVRCA_FRAME_FULL_RATE;
    break;
  case MVS_VOC_ERASURE:
    *out_rate = VS_VOCEVRCA_FRAME_ERASURE_RATE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}

static uint32_t mvs_mod_set_evrca_frame_rate (
  mvs_voc_rate_type min_rate,
  mvs_voc_rate_type max_rate
)
{
  uint32_t rc;
  uint32_t rc1;
  uint32_t rc2;
  vs_vocevrca_cmd_set_frame_minmax_rate_t frame_rate_cmd;

  frame_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  rc1 = mvs_map_evrca_frame_rate( min_rate, &frame_rate_cmd.min_rate );
  rc2 = mvs_map_evrca_frame_rate( max_rate, &frame_rate_cmd.max_rate );

  if ( rc1 == APR_EOK && rc2 == APR_EOK )
  {
    rc = vs_call( VS_VOCEVRCA_CMD_SET_FRAME_MINMAX_RATE, ( void* )&frame_rate_cmd, 
                  sizeof( frame_rate_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrca_frame_rate(): " \
             "Failed to set frame rate to VS, rc = (0x%08x)", rc );
    }
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrca_frame_rate(): " \
           "Invalid frame rate, min = (%d), max = (%d)",
           min_rate, max_rate );
    rc = APR_EBADPARAM;
  }

  return rc;
}

static uint32_t mvs_map_evrcb_frame_rate (
  mvs_voc_rate_type in_rate,
  vs_vocevrcb_frame_rate_t* out_rate
)
{  
  if ( out_rate == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( in_rate )
  {
  case MVS_VOC_0_RATE:
    *out_rate = VS_VOCEVRCB_FRAME_BLANK_RATE;
    break;
  case MVS_VOC_8_RATE:
    *out_rate = VS_VOCEVRCB_FRAME_EIGHTH_RATE;
    break;
  case MVS_VOC_4_RATE:
    *out_rate = VS_VOCEVRCB_FRAME_QUARTER_RATE;
    break;
  case MVS_VOC_2_RATE:
    *out_rate = VS_VOCEVRCB_FRAME_HALF_RATE;
    break;
  case MVS_VOC_1_RATE:
    *out_rate = VS_VOCEVRCB_FRAME_FULL_RATE;
    break;
  case MVS_VOC_ERASURE:
    *out_rate = VS_VOCEVRCB_FRAME_ERASURE_RATE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}

static uint32_t mvs_mod_set_evrcb_frame_rate (
  mvs_voc_rate_type min_rate,
  mvs_voc_rate_type max_rate
)
{
  uint32_t rc;
  uint32_t rc1;
  uint32_t rc2;
  vs_vocevrcb_cmd_set_frame_minmax_rate_t frame_rate_cmd;

  frame_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  rc1 = mvs_map_evrcb_frame_rate( min_rate, &frame_rate_cmd.min_rate );
  rc2 = mvs_map_evrcb_frame_rate( max_rate, &frame_rate_cmd.max_rate );

  if ( rc1 == APR_EOK && rc2 == APR_EOK )
  {
    rc = vs_call( VS_VOCEVRCB_CMD_SET_FRAME_MINMAX_RATE, ( void* )&frame_rate_cmd, 
                  sizeof( frame_rate_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcb_frame_rate(): " \
             "Failed to set frame rate to VS, rc = (0x%08x)", rc );
    }
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcb_frame_rate(): " \
           "Invalid frame rate, min = (%d), max = (%d)",
           min_rate, max_rate );
    rc = APR_EBADPARAM;
  }

  return rc;
}

static uint32_t mvs_map_evrcwb_frame_rate (
  mvs_voc_rate_type in_rate,
  vs_vocevrcwb_frame_rate_t* out_rate
)
{  
  if ( out_rate == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( in_rate )
  {
  case MVS_VOC_0_RATE:
    *out_rate = VS_VOCEVRCWB_FRAME_BLANK_RATE;
    break;
  case MVS_VOC_8_RATE:
    *out_rate = VS_VOCEVRCWB_FRAME_EIGHTH_RATE;
    break;
  case MVS_VOC_4_RATE:
    *out_rate = VS_VOCEVRCWB_FRAME_QUARTER_RATE;
    break;
  case MVS_VOC_2_RATE:
    *out_rate = VS_VOCEVRCWB_FRAME_HALF_RATE;
    break;
  case MVS_VOC_1_RATE:
    *out_rate = VS_VOCEVRCWB_FRAME_FULL_RATE;
    break;
  case MVS_VOC_ERASURE:
    *out_rate = VS_VOCEVRCWB_FRAME_ERASURE_RATE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}

static uint32_t mvs_mod_set_evrcwb_frame_rate (
  mvs_voc_rate_type min_rate,
  mvs_voc_rate_type max_rate
)
{
  uint32_t rc;
  uint32_t rc1;
  uint32_t rc2;
  vs_vocevrcwb_cmd_set_frame_minmax_rate_t frame_rate_cmd;

  frame_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  rc1 = mvs_map_evrcwb_frame_rate( min_rate, &frame_rate_cmd.min_rate );
  rc2 = mvs_map_evrcwb_frame_rate( max_rate, &frame_rate_cmd.max_rate );

  if ( rc1 == APR_EOK && rc2 == APR_EOK )
  {
    rc = vs_call( VS_VOCEVRCWB_CMD_SET_FRAME_MINMAX_RATE, ( void* )&frame_rate_cmd, 
                  sizeof( frame_rate_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcwb_frame_rate(): " \
             "Failed to set frame rate to VS, rc = (0x%08x)", rc );
    }
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcwb_frame_rate(): " \
           "Invalid frame rate, min = (%d), max = (%d)",
           min_rate, max_rate );
    rc = APR_EBADPARAM;
  }

  return rc;
}

static uint32_t mvs_map_evrcnw_frame_rate (
  mvs_voc_rate_type in_rate,
  vs_vocevrcnw_frame_rate_t* out_rate
)
{
  if ( out_rate == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( in_rate )
  {
  case MVS_VOC_0_RATE:
    *out_rate = VS_VOCEVRCNW_FRAME_BLANK_RATE;
    break;
  case MVS_VOC_8_RATE:
    *out_rate = VS_VOCEVRCNW_FRAME_EIGHTH_RATE;
    break;
  case MVS_VOC_4_RATE:
    *out_rate = VS_VOCEVRCNW_FRAME_QUARTER_RATE;
    break;
  case MVS_VOC_2_RATE:
    *out_rate = VS_VOCEVRCNW_FRAME_HALF_RATE;
    break;
  case MVS_VOC_1_RATE:
    *out_rate = VS_VOCEVRCNW_FRAME_FULL_RATE;
    break;
  case MVS_VOC_ERASURE:
    *out_rate = VS_VOCEVRCNW_FRAME_ERASURE_RATE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}

static uint32_t mvs_mod_set_evrcnw_frame_rate (
  mvs_voc_rate_type min_rate,
  mvs_voc_rate_type max_rate
)
{
  uint32_t rc;
  uint32_t rc1;
  uint32_t rc2;
  vs_vocevrcnw_cmd_set_frame_minmax_rate_t frame_rate_cmd;

  frame_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  rc1 = mvs_map_evrcnw_frame_rate( min_rate, &frame_rate_cmd.min_rate );
  rc2 = mvs_map_evrcnw_frame_rate( max_rate, &frame_rate_cmd.max_rate );

  if ( rc1 == APR_EOK && rc2 == APR_EOK )
  {
    rc = vs_call( VS_VOCEVRCNW_CMD_SET_FRAME_MINMAX_RATE, ( void* )&frame_rate_cmd, 
                  sizeof( frame_rate_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcnw_frame_rate(): " \
             "Failed to set frame rate to VS, rc = (0x%08x)", rc );
    }
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcnw_frame_rate(): " \
           "Invalid frame rate, min = (%d), max = (%d)",
           min_rate, max_rate );
    rc = APR_EBADPARAM;
  }

  return rc;
}

static uint32_t mvs_map_qcelp13k_frame_rate (
  mvs_voc_rate_type in_rate,
  vs_vocqcelp13k_frame_rate_t* out_rate
)
{
  if ( out_rate == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( in_rate )
  {
  case MVS_VOC_0_RATE:
    *out_rate = VS_VOCQCELP13K_FRAME_BLANK_RATE;
    break;
  case MVS_VOC_8_RATE:
    *out_rate = VS_VOCQCELP13K_FRAME_EIGHTH_RATE;
    break;
  case MVS_VOC_4_RATE:
    *out_rate = VS_VOCQCELP13K_FRAME_QUARTER_RATE;
    break;
  case MVS_VOC_2_RATE:
    *out_rate = VS_VOCQCELP13K_FRAME_HALF_RATE;
    break;
  case MVS_VOC_1_RATE:
    *out_rate = VS_VOCQCELP13K_FRAME_FULL_RATE;
    break;
  case MVS_VOC_ERASURE:
    *out_rate = VS_VOCQCELP13K_FRAME_ERASURE_RATE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}

static uint32_t mvs_mod_set_qcelp13k_frame_rate (
  mvs_voc_rate_type min_rate,
  mvs_voc_rate_type max_rate
)
{
  uint32_t rc;
  uint32_t rc1;
  uint32_t rc2;
  vs_vocqcelp13k_cmd_set_frame_minmax_rate_t frame_rate_cmd;

  frame_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  rc1 = mvs_map_qcelp13k_frame_rate( min_rate, &frame_rate_cmd.min_rate );
  rc2 = mvs_map_qcelp13k_frame_rate( max_rate, &frame_rate_cmd.max_rate );

  if ( rc1 == APR_EOK && rc2 == APR_EOK )
  {
    rc = vs_call( VS_VOCQCELP13K_CMD_SET_FRAME_MINMAX_RATE, ( void* )&frame_rate_cmd, 
                  sizeof( frame_rate_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_qcelp13k_frame_rate(): " \
             "Failed to set frame rate to VS, rc = (0x%08x)", rc );
    }
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_qcelp13k_frame_rate(): " \
           "Invalid frame rate, min = (%d), max = (%d)",
           min_rate, max_rate );
    rc = APR_EBADPARAM;
  }

  return rc;
}

int32_t mvs_mod_voc_set_frame_rate (
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
)
{
  int32_t rc;
  uint32_t vs_media_id;

  mvs_profiling_save_entry( MVS_PROFILING_SET_FRAME_RATE_ENTER );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_voc_set_frame_rate(): " \
         "max_rate (%d), min_rate (%d)", max_rate, min_rate );

  if ( max_rate == MVS_VOC_0_RATE )
  { /* Modem sending invalid rate */
    max_rate = MVS_VOC_1_RATE;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_voc_set_frame_rate(): " \
           "max_rate (%d) to be applied", max_rate );
  }

  if ( min_rate == MVS_VOC_0_RATE )
  { /* Modem sending invalid rate */
    min_rate = MVS_VOC_8_RATE;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_voc_set_frame_rate(): " \
           "min_rate (%d) to be applied", min_rate );
  }

  mvs_new_props.max_voc_rate = max_rate;
  mvs_new_props.min_voc_rate = min_rate;

  if ( mvs_cur_client != MVS_CLIENT_NONE )
  {
    if ( MVS_MODE_NONE == mvs_client_table[mvs_cur_client].config_mode )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_voc_set_frame_rate(): No config mode" );
      return APR_EOK;
    }
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_voc_set_frame_rate(): No current client" );
    return APR_EOK;
  }
  
  vs_media_id = mvs_map_vs_media_id( mvs_client_table[mvs_cur_client].config_mode );
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_voc_set_frame_rate(): current media ID " \
        "(0x%08x)", vs_media_id );

  switch ( vs_media_id )
  {
  case VS_COMMON_MEDIA_ID_EVRCA:
    rc = mvs_mod_set_evrca_frame_rate( min_rate, max_rate );
    break;
  case VS_COMMON_MEDIA_ID_EVRCB:
    rc = mvs_mod_set_evrcb_frame_rate( min_rate, max_rate );
    break;
  case VS_COMMON_MEDIA_ID_EVRCWB:
    rc = mvs_mod_set_evrcwb_frame_rate( min_rate, max_rate );
    break;
  case VS_COMMON_MEDIA_ID_EVRCNW:
    rc = mvs_mod_set_evrcnw_frame_rate( min_rate, max_rate );
    break;
  case VS_COMMON_MEDIA_ID_QCELP13K:
    rc = mvs_mod_set_qcelp13k_frame_rate( min_rate, max_rate );
    break;
  default:
    rc = APR_EFAILED;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mode_set_dtx_mode(): " \
           "Not supported media ID (0x%08x)", vs_media_id );
  }

  mvs_profiling_save_entry( MVS_PROFILING_SET_FRAME_RATE_LEAVE );

  return rc;
}

static uint32_t mvs_mod_set_qcelp13k_enc_rate (
  uint16_t rate
)
{
  uint32_t rc;
  vs_vocqcelp13k_cmd_set_avg_enc_rate_t set_enc_rate_cmd;

  switch ( rate )
  {
  case 0x0000:
    set_enc_rate_cmd.rate = VS_VOCQCELP13K_AVG_ENC_RATE_14_4_KBPS;
    break;

  case 0x0001:
    set_enc_rate_cmd.rate = VS_VOCQCELP13K_AVG_ENC_RATE_12_2_KBPS;
    break;

  case 0x0002:
    set_enc_rate_cmd.rate = VS_VOCQCELP13K_AVG_ENC_RATE_11_2_KBPS;
    break;

  case 0x0003:
    set_enc_rate_cmd.rate = VS_VOCQCELP13K_AVG_ENC_RATE_9_0_KBPS;
    break;

  case 0x0004:
    set_enc_rate_cmd.rate = VS_VOCQCELP13K_AVG_ENC_RATE_7_2_KBPS;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_qcelp13k_enc_rate(): " \
           "Invalid rate = (%d)", rate );
    break;
  }

  set_enc_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;

  rc = vs_call( VS_VOCQCELP13K_CMD_SET_AVG_ENC_RATE, ( void* )&set_enc_rate_cmd, 
                sizeof( set_enc_rate_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_qcelp13k_enc_rate(): " \
           "Failed to set rate, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrcb_enc_rate (
  uint16_t rate
)
{
  uint32_t rc;
  vs_vocevrcb_cmd_set_avg_enc_rate_t set_enc_rate_cmd;

  switch ( rate )
  {
  case 0x0000:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_9_3_KBPS;
    break;

  case 0x0001:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_8_5_KBPS;
    break;

  case 0x0002:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_7_5_KBPS;
    break;

  case 0x0003:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_7_0_KBPS;
    break;

  case 0x0004:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_6_6_KBPS;
    break;

  case 0x0005:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_6_2_KBPS;
    break;

  case 0x0006:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_5_8_KBPS;
    break;

  case 0x0007:
    set_enc_rate_cmd.rate = VS_VOCEVRCB_AVG_ENC_RATE_4_8_KBPS;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcb_enc_rate(): " \
           "Invalid rate (%d)", rate );
    break;
  }

  set_enc_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;

  rc = vs_call( VS_VOCEVRCB_CMD_SET_AVG_ENC_RATE, ( void* )&set_enc_rate_cmd, 
                sizeof( set_enc_rate_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcb_enc_rate(): " \
           "Failed to set rate, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrca_enc_rate_mod (
  uint16_t rate
)
{
  uint32_t rc;
  vs_vocevrca_cmd_set_enc_rate_mod_t set_enc_rate_mod_cmd;

  switch ( rate )
  {
  case 0x0000: /* Target 14.4 kbps (8/8 rate) on the average. */
    set_enc_rate_mod_cmd.rate_modulation = 0x0000;
    /* Bitmasks:
     *    0x0000 = Vocoder rate modulation disabled.
     */
    break;

  case 0x0001: /* Target 12.2 kbps (7/8 rate) on the average. */
    set_enc_rate_mod_cmd.rate_modulation = 0x000F;
    /* Bitmasks:
     *    0x0001 = Vocoder rate modulation enabled.
     *    0x0002 = Select S.
     *    0x000C = Rate limit factor S=3.
     */
    break;

  case 0x0002: /* Target 11.2 kbps (6/8 rate) on the average. */
    set_enc_rate_mod_cmd.rate_modulation = 0x0007;
    /* Bitmasks:
     *    0x0001 = Vocoder rate modulation enabled.
     *    0x0002 = Select S.
     *    0x0004 = Rate limit factor S=1.
     */
    break;

  case 0x0003: /* Target 9.0 kbps (5/8 rate) on the average. */
    set_enc_rate_mod_cmd.rate_modulation = 0x0005;
    /* Bitmasks:
     *    0x0001 = Vocoder rate modulation enabled.
     *    0x0000 = Select 1/S.
     *    0x0004 = Rate limit factor S=3.
     */
    break;

  case 0x0004: /* Target 7.2 kbps (4/8 rate) on the average. */
    set_enc_rate_mod_cmd.rate_modulation = 0x0000;
    /* Bitmasks:
     *    0x0000 = Vocoder rate modulation disabled. (Not supported).
     */
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrca_enc_rate_mod(): " \
           "Invalid rate = (%d)", rate );
    return APR_EBADPARAM;
  }

  set_enc_rate_mod_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;

  rc = vs_call( VS_VOCEVRCA_CMD_SET_ENC_RATE_MODULATION, ( void* )&set_enc_rate_mod_cmd, 
                sizeof( set_enc_rate_mod_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrca_enc_rate_mod(): " \
           "Failed to set rate modulation, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrcwb_enc_rate (
  uint16_t rate
)
{
  uint32_t rc;
  vs_vocevrcwb_cmd_set_avg_enc_rate_t set_enc_rate_cmd;

  switch ( rate )
  {
  case 0x0000:
    set_enc_rate_cmd.rate = VS_VOCEVRCWB_AVG_ENC_RATE_8_5_KBPS;
    break;

  case 0x0004:
    set_enc_rate_cmd.rate = VS_VOCEVRCWB_AVG_ENC_RATE_9_3_KBPS;
    break;

  case 0x0007:
    set_enc_rate_cmd.rate = VS_VOCEVRCWB_AVG_ENC_RATE_4_8_KBPS;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcwb_enc_rate(): " \
           "Invalid rate (%d)", rate );
    break;
  }

  set_enc_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;

  rc = vs_call( VS_VOCEVRCWB_CMD_SET_AVG_ENC_RATE, ( void* )&set_enc_rate_cmd, 
                sizeof( set_enc_rate_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcwb_enc_rate(): " \
           "Failed to set rate, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrcnw_enc_rate (
  uint16_t rate
)
{
  uint32_t rc;
  vs_vocevrcnw_cmd_set_avg_enc_rate_t set_enc_rate_cmd;

  switch ( rate )
  {
  case 0x0000:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_8_5_KBPS;
    break;

  case 0x0001:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_9_3_KBPS;
    break;

  case 0x0002:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_7_5_KBPS;
    break;

  case 0x0003:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_7_0_KBPS;
    break;

  case 0x0004:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_6_6_KBPS;
    break;

  case 0x0005:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_6_2_KBPS;
    break;

  case 0x0006:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_5_8_KBPS;
    break;

  case 0x0007:
    set_enc_rate_cmd.rate = VS_VOCEVRCNW_AVG_ENC_RATE_4_8_KBPS;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcnw_enc_rate(): " \
           "Invalid rate (%d)", rate );
    break;
  }

  set_enc_rate_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;

  rc = vs_call( VS_VOCEVRCNW_CMD_SET_AVG_ENC_RATE, ( void* )&set_enc_rate_cmd, 
                sizeof( set_enc_rate_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcnw_enc_rate(): " \
           "Failed to set rate, rc = (0x%08x)", rc );
  }

  return rc;
}

int32_t mvs_mod_voc_tx_rate_limit (
  uint16 rate_factor
)
{
  int32_t rc;
  uint16_t rate;
  uint32_t vs_media_id;

  mvs_profiling_save_entry( MVS_PROFILING_SET_TX_RATE_LIMIT_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_tx_rate_limit(): " \
        "rate_factor (%d)", rate_factor );

  mvs_new_props.rate_factor = rate_factor;

  if ( mvs_cur_client != MVS_CLIENT_NONE )
  {
    if ( MVS_MODE_NONE == mvs_client_table[mvs_cur_client].config_mode )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_voc_tx_rate_limit(): " \
           "No config mode" );
      return APR_EOK;
    }
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_voc_tx_rate_limit(): " \
         "No current client" );
    return APR_EOK;
  }

  vs_media_id = mvs_map_vs_media_id( mvs_client_table[mvs_cur_client].config_mode );
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mode_set_dtx_mode(): current media ID " \
        "(0x%08x)", vs_media_id );

  if ( mvs_client_table[mvs_cur_client].state == MVS_STATE_VOCODER )
  {
    { /* Historical notes:
       *
       *    MCCTC.C shifts the rate reduced mode field up a bit. This works
       *    for IS-96(A). TR-45 (13kbps) needs a value of 0..4 for its' rate
       *    reduced mode, so we compensate for the shift here.
       *
       * Translation:
       *
       *    1X modem code passes us the rate reduced level left-shifted by 2.
       */
      rate = ( ( rate_factor >> 1 ) & 7 );
        /* The mode is stored in b3..b1 of the service option control message
         * field.
         */
    }

    /* Send reduce rate level (average bit-rate) to the vocoder. */
    switch ( vs_media_id )
    {
    case VS_COMMON_MEDIA_ID_QCELP13K:
      rc = mvs_mod_set_qcelp13k_enc_rate( rate );
      break;

    case VS_COMMON_MEDIA_ID_EVRCB:
      rc = mvs_mod_set_evrcb_enc_rate( rate );
      break;

    case VS_COMMON_MEDIA_ID_EVRCWB:
      rc = mvs_mod_set_evrcwb_enc_rate ( rate );
      break;

    case VS_COMMON_MEDIA_ID_EVRCNW:
      rc = mvs_mod_set_evrcnw_enc_rate ( rate );
      break;

    case VS_COMMON_MEDIA_ID_EVRCA:
      rc = mvs_mod_set_evrca_enc_rate_mod( rate );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_voc_tx_rate_limit(): " \
             "Invalid media ID (0x%08x)", vs_media_id );
      rc = APR_EBADPARAM;      
      break;
    }
  }
  else
  {
    /* Caching in place */
    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_voc_tx_rate_limit(): " \
        "Caching values" );
    rc = APR_EOK;
  } /* if ( mvs_client_table[mvs_cur_client].state == MVS_STATE_VOCODER ) */

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_voc_tx_rate_limit(): complete - " \
        "rate-factor (%d)", rate_factor );

  mvs_profiling_save_entry( MVS_PROFILING_SET_TX_RATE_LIMIT_LEAVE );

  return rc;
}

static uint32_t mvs_mod_set_hr_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vochr_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCHR_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_hr_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_fr_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocfr_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCFR_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_fr_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_efr_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocefr_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCEFR_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_efr_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_amr_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocamr_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCAMR_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_amr_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_amrwb_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocamrwb_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCAMRWB_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_amrwb_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrca_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocevrca_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCEVRCA_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrca_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrcb_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocevrcb_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCEVRCB_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcb_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrcwb_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocevrcwb_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCEVRCWB_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcwb_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_evrcnw_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocevrcnw_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCEVRCNW_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_evrcnw_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_qcelp13k_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocqcelp13k_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCQCELP13K_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_qcelp13k_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_mod_set_g729_dtx_mode (
  mvs_dtx_mode_type dtx_mode  
)
{
  uint32_t rc;
  vs_vocg729_cmd_set_dtx_mode_t dtx_cmd;

  dtx_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  dtx_cmd.enable_flag = ( dtx_mode == MVS_DTX_ON );

  rc = vs_call( VS_VOCG729_CMD_SET_DTX_MODE, ( void* )&dtx_cmd, 
                sizeof( dtx_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_set_g729_dtx_mode(): " \
           "Failed to set dtx mode, rc = (0x%08x)", rc );
  }

  return rc;
}

int32_t mvs_mod_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
)
{
  int32_t rc;
  uint32_t vs_media_id;

  mvs_profiling_save_entry( MVS_PROFILING_SET_DTX_MODE_ENTER );

  mvs_new_props.dtx_mode = dtx_mode;

  if ( mvs_cur_client != MVS_CLIENT_NONE )
  {
    if ( MVS_MODE_NONE == mvs_client_table[mvs_cur_client].config_mode )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_set_dtx_mode(): No config mode" );
      return APR_EOK;
    }
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_set_dtx_mode():No current client" );
    return APR_EOK;
  }
  
  vs_media_id = mvs_map_vs_media_id( mvs_client_table[mvs_cur_client].config_mode );
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mode_set_dtx_mode(): media ID " \
        "(0x%08x)", vs_media_id );

  switch ( vs_media_id )
  {
  case VS_COMMON_MEDIA_ID_HR:
    rc = mvs_mod_set_hr_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_FR:
    rc = mvs_mod_set_fr_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_EFR:
    rc = mvs_mod_set_efr_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_AMR:
    rc = mvs_mod_set_amr_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_AMRWB:
    rc = mvs_mod_set_amrwb_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_EVRCA:
    rc = mvs_mod_set_evrca_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_EVRCB:
    rc = mvs_mod_set_evrcb_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_EVRCWB:
    rc = mvs_mod_set_evrcwb_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_EVRCNW:
    rc = mvs_mod_set_evrcnw_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_QCELP13K:
    rc = mvs_mod_set_qcelp13k_dtx_mode( dtx_mode );
    break;
  case VS_COMMON_MEDIA_ID_G729:
    rc = mvs_mod_set_g729_dtx_mode( dtx_mode );
    break;
  default:
    rc = APR_EFAILED;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mode_set_dtx_mode(): " \
           "Invalid media ID (0x%08x)", vs_media_id );
  }

  mvs_profiling_save_entry( MVS_PROFILING_SET_DTX_MODE_LEAVE );

  return rc;
}

int32_t mvs_mod_amr_set_scr_mode (
  mvs_scr_mode_type scr_mode
)
{
  int32_t rc;

  rc = mvs_mod_set_dtx_mode( (mvs_dtx_mode_type)scr_mode );
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_amr_set_scr_mode(): " \
        "mode = (%d), rc = (0x%08x)", scr_mode, rc);

  return rc;
}

int32_t mvs_mod_amr_set_awb_mode (
  mvs_amr_mode_type awb_ul_mode
)
{
  int32_t rc;
  vs_vocamrwb_cmd_set_codec_mode_t set_codec_mode_cmd;

  mvs_profiling_save_entry( MVS_PROFILING_SET_AWB_MODE_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_amr_set_awb_mode(): " \
        "mode (%d)", awb_ul_mode );

  if ( ( awb_ul_mode == MVS_AMR_MODE_MAX ) ||
       ( awb_ul_mode < MVS_AMR_MODE_0660 ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_amr_set_awb_mode(): " \
           "AMR_WB mode has not been set. Set to (%d)",
           MVS_AMR_MODE_2385 );
    awb_ul_mode =  MVS_AMR_MODE_2385;
  }

  mvs_new_props.awb_ul_mode = awb_ul_mode;

  if ( mvs_cur_client != MVS_CLIENT_NONE )
  {
    if ( MVS_MODE_NONE == mvs_client_table[mvs_cur_client].config_mode )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_amr_set_awb_mode(): No config mode" );
      return APR_EOK;
    }
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_amr_set_awb_mode(): No current client" );
    return APR_EOK;
  }

  set_codec_mode_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  set_codec_mode_cmd.codec_mode = (vs_vocamrwb_codec_mode_t)( awb_ul_mode - MVS_AMR_MODE_0660 );

  rc = vs_call( VS_VOCAMRWB_CMD_SET_CODEC_MODE, ( void* )&set_codec_mode_cmd,
                sizeof( set_codec_mode_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_amr_set_awb_mode(): " \
           "Failed to set AMRWB mode, rc = (0x%08x)", rc );
  }

  amrsup_set_default_dl_amr_mode( (amrsup_mode_type) mvs_new_props.awb_ul_mode );

  mvs_profiling_save_entry( MVS_PROFILING_SET_AWB_MODE_LEAVE );

  return rc;
}

int32_t mvs_mod_amr_set_amr_mode (
  mvs_amr_mode_type amr_ul_mode
)
{
  int32_t rc;
  vs_vocamr_cmd_set_codec_mode_t set_codec_mode_cmd;

  mvs_profiling_save_entry( MVS_PROFILING_SET_AMR_MODE_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_amr_set_amr_mode(): " \
        "mode = (%d)", amr_ul_mode );

  if ( ( amr_ul_mode == MVS_AMR_MODE_MAX ) ||
       ( amr_ul_mode > MVS_AMR_MODE_1220) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_amr_set_amr_mode(): AMR " \
           "mode has not been set. Set to (%d)", MVS_AMR_MODE_1220 );
    amr_ul_mode =  MVS_AMR_MODE_1220;
  }

  mvs_new_props.amr_ul_mode = amr_ul_mode;

  if ( mvs_cur_client != MVS_CLIENT_NONE )
  {
    if ( MVS_MODE_NONE == mvs_client_table[mvs_cur_client].config_mode )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_amr_set_amr_mode(): " \
           "No config mode" );
      return APR_EOK;
    }
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_amr_set_amr_mode(): " \
         "No current client" );
    return APR_EOK;
  }

  set_codec_mode_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  set_codec_mode_cmd.codec_mode = ( vs_vocamr_codec_mode_t )amr_ul_mode;

  rc = vs_call( VS_VOCAMR_CMD_SET_CODEC_MODE, ( void* )&set_codec_mode_cmd,
                sizeof( set_codec_mode_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_amr_set_amr_mode(): " \
           "Failed to set AMR mode, rc = (0x%08x)", rc );
  }

  amrsup_set_default_dl_amr_mode( (amrsup_mode_type) mvs_new_props.amr_ul_mode );

  mvs_profiling_save_entry( MVS_PROFILING_SET_AMR_MODE_ENTER );

  return rc;
}

/* TBD */
void mvs_mod_set_g711_mode (
  mvs_g711_mode_type mode
)
{
  mvs_current_g711_mode = mode;
}

/* TBD */
mvs_g711_mode_type mvs_mod_get_g711_mode ( void )
{
  return mvs_current_g711_mode;
}

/* Not implemented in FW */
mvs_g722_mode_type mvs_mod_get_g722_mode ( void )
{
  /* Not suppoprted. Set to max */
  return MVS_G722_MODE_MAX;
}

void mvs_mod_set_g722_mode (
  mvs_g722_mode_type mode
)
{
  /* Not suppoprted */
}

#ifndef WINSIM

static int32_t mvs_extract_channels_from_if1 (
  uint8 *AMRIF1Packet,
  amrsup_core_speech_type *speech_data
)
{
  int nResult = APR_EOK;
  int bitsReadForClass ;
  uint8* pbyDest  ;
  int32 CurrbitRead ;
  int32 CurrBitWrite ;
  int32 CurrByteRead ;
  int32 CurrByteWrite ;
  uint8* pbySrc ;
  uint8 pbyDesC ;
  uint8 CurrByte;
  uint8 CurrBit;
  int writeBitPos;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_extract_channels_from_if1()" );

  if ( AMRIF1Packet == NULL )
  {
    return APR_EBADPARAM;
  }

  //uint32 ClassABitsOffset = AmrFrameInfo.AMRCoreDataOffset ;
  memset((void*)(speech_data->data_a), 0, AMRSUP_CLASS_A_BYTES);
  memset((void*)(speech_data->data_b), 0, AMRSUP_CLASS_B_BYTES);
  memset((void*)(speech_data->data_c), 0, AMRSUP_CLASS_C_BYTES);

  bitsReadForClass = 0 ;
  pbyDest = speech_data->data_a ;
  CurrbitRead = 0 ;
  CurrBitWrite = 0 ;
  CurrByteRead = 0 ;
  CurrByteWrite = 0 ;
  pbySrc = NULL;
  pbySrc = AMRIF1Packet ;
  pbyDesC = 0x0;

  while ( bitsReadForClass < speech_data->num_bits_a )
  {
    CurrbitRead = 0 ;
    CurrBitWrite = 0 ;

    CurrByte = *(pbySrc + CurrByteRead) ;
    //munch 8 bits or if u have reached your quota for this Class of bits
    //change
    while ( (CurrbitRead < 8) && (bitsReadForClass < speech_data->num_bits_a) )
    {
      CurrBit = mvs_MaskArray[7-CurrbitRead] & CurrByte ; // got a bit here
      pbyDesC = pbyDesC | CurrBit ;            // put it together
      CurrbitRead++ ;
      CurrBitWrite++ ;
      bitsReadForClass++ ;
      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }

    }

    if ( CurrBitWrite == 8 )
    {
      *(pbyDest + CurrByteWrite) = pbyDesC ;
      CurrBitWrite = 0 ;
      CurrByteWrite++ ;
      pbyDesC = 0x0;
    }
    if ( CurrbitRead==8 )
    {
      CurrByteRead++ ;
      CurrbitRead = 0 ;
    }


  }

  bitsReadForClass = 0 ;
  *(pbyDest + CurrByteWrite) = pbyDesC ;
  pbyDesC = 0x0 ;
  CurrByteWrite = 0 ; // destination is changed
  CurrBitWrite = 0 ;
  pbyDest = speech_data->data_b ;
  //uint32 pRemain = CurrbitRead ;
  //CurrByteRead is not moved ahead

  while ( bitsReadForClass < speech_data->num_bits_b )
  {

    CurrByte = *(pbySrc + CurrByteRead) ;
    //munch 8 bits or if u have reached your quota for this Class of bits
    while ( (CurrbitRead < 8) && (bitsReadForClass < speech_data->num_bits_b) )
    {
      CurrBit = mvs_MaskArray[7-CurrbitRead] & CurrByte ;
      writeBitPos = CurrbitRead-CurrBitWrite ;
      if ( writeBitPos>0 )
      {
        CurrBit = CurrBit << (CurrbitRead-CurrBitWrite) ;
      }
      else
      {
        CurrBit = CurrBit >> (CurrBitWrite-CurrbitRead) ;
      }

      pbyDesC = pbyDesC | CurrBit ;
      CurrbitRead++ ;
      CurrBitWrite++ ;
      bitsReadForClass++ ;
      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }

    }

    if ( CurrbitRead == 8 )
    {
      CurrbitRead = 0 ;
      CurrByteRead++ ;
    }

    if ( CurrBitWrite == 8 )
    {
      *(pbyDest + CurrByteWrite) = pbyDesC ;
      CurrBitWrite = 0 ;
      CurrByteWrite++ ;
      pbyDesC  = 0x0 ;
    }


  }
  bitsReadForClass = 0 ;
  *(pbyDest + CurrByteWrite) = pbyDesC ;
  pbyDesC = 0x0 ;
  CurrByteWrite = 0 ; // destination is changed
  CurrBitWrite = 0 ;
  pbyDest = speech_data->data_c ;
  //CurrByteRead is not moved ahead

  while ( bitsReadForClass < speech_data->num_bits_c )
  {
    CurrByte = *(pbySrc + CurrByteRead) ;
    //munch 8 bits or if u have reached your quota for this Class of bits
    while ( (CurrbitRead < 8) && (bitsReadForClass < speech_data->num_bits_c) )
    {
      CurrBit = mvs_MaskArray[7-CurrbitRead] & CurrByte ;
      writeBitPos = CurrbitRead-CurrBitWrite ;
      if ( writeBitPos>0 )
      {
        CurrBit = CurrBit << (CurrbitRead-CurrBitWrite) ;
      }
      else
      {
        CurrBit = CurrBit >> (CurrBitWrite-CurrbitRead) ;
      }

      pbyDesC = pbyDesC | CurrBit ;
      CurrbitRead++ ;
      CurrBitWrite++ ;
      bitsReadForClass++ ;
      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }

    if ( CurrbitRead == 8 )
    {
      CurrbitRead = 0 ;
      CurrByteRead++ ;
    }

    if ( CurrBitWrite == 8 )
    {
      *(pbyDest + CurrByteWrite) = pbyDesC ;
      CurrBitWrite = 0 ;
      CurrByteWrite++ ;
      pbyDesC  = 0x0 ;
    }

  }

  *(pbyDest + CurrByteWrite) = pbyDesC ;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_extract_channels_from_if1(): " \
        "complete (%d)",
        nResult );

  return nResult ;

}

static int32_t mvs_make_if1_from_channels (
  uint8 *AMRIF1Packet,
  amrsup_core_speech_type *speech_data,
  int nIF1PacketLength,
  int *nIF1PacketLengthFormed
)
{
  int32_t nResult = APR_EOK;
  int32 CurrByteWrite  = 0 ;
  int32 CurrBitWrite = 0 ;
  uint8 *pbySrc = NULL;
  uint32 bitsReadForClass = 0 ;
  int32 CurrbitRead = 0 ;
  int32 CurrByteRead = 0 ;
  uint8 pbyDesC =0x0;
  uint8* pbyDest = NULL;
  uint8 CurrByte;
  uint8 CurrBit;
  int writeBitPos;


  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_make_if1_from_channels()" );

  if ( AMRIF1Packet == NULL )
    return APR_EBADPARAM;

  //Initialize vocoder packet to zero
  memset((void*)(AMRIF1Packet), 0, nIF1PacketLength);

  //Check for NO_DATA frame
  if ( speech_data->num_bits_a == 0 &&
       speech_data->num_bits_b == 0 &&
       speech_data->num_bits_c == 0 )
  {
    if ( nIF1PacketLengthFormed != NULL ) *nIF1PacketLengthFormed = 0;
    return APR_EOK;
  }

  CurrByteWrite  = 0 ;
  CurrBitWrite = 0 ;
  pbySrc = NULL;
  bitsReadForClass = 0 ;
  CurrbitRead = 0 ;
  CurrByteRead = 0 ;
  pbyDesC =0x0;
  pbyDest = NULL;

  if ( speech_data->num_bits_a )
  {
    pbySrc = speech_data->data_a ;

    /* pbyDest = (AMRIF1Packet+3)+CurrByteWrite ; */
    pbyDest = (AMRIF1Packet)+CurrByteWrite ;

    while ( bitsReadForClass < speech_data->num_bits_a )
    {
      CurrByte = *(pbySrc + CurrByteRead) ;
      while ( (CurrbitRead<8)&&(bitsReadForClass < speech_data->num_bits_a) )
      {
        CurrBit = mvs_MaskArray[7-CurrbitRead] & CurrByte ;

#if 0 /* TODO: Need to re-enable? */
        int writeBitPos = CurrbitRead-CurrBitWrite ;
        if (writeBitPos>0)
        {
          CurrBit = CurrBit >> (CurrbitRead-CurrBitWrite) ;
        }
        else
        {
          CurrBit = CurrBit << (CurrBitWrite-CurrbitRead) ;
        }
#endif /* 0 */

        pbyDesC = pbyDesC | CurrBit ;
        CurrbitRead++ ;
        CurrBitWrite++ ;
        bitsReadForClass++ ;
        if ( CurrBitWrite == 8 )
        {
          *(pbyDest + CurrByteWrite) = pbyDesC ;
          CurrBitWrite = 0 ;
          CurrByteWrite++ ;
          pbyDesC  = 0x0 ;
        }
      }//process 8 bits

      if ( CurrbitRead == 8 )
      {
        CurrbitRead = 0 ;
        CurrByteRead++ ;
      }

      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }//for all A bits

  }//processed all A channel bits
  bitsReadForClass = 0 ;

  //    *(pbyDest + CurrByteWrite) = pbyDesC ;
  CurrbitRead = 0 ;
  CurrByteRead = 0 ;
  if ( speech_data->num_bits_b )
  {
    pbySrc = speech_data->data_b ;
    if( speech_data->num_bits_a == 0 )
    {
      pbyDest = AMRIF1Packet;
    }
    //uint8* pbyDest = (AMRIF1Packet+4)+CurrByteWrite ;
    while ( bitsReadForClass < speech_data->num_bits_b )
    {
      CurrByte = *(pbySrc + CurrByteRead) ;
      while ( (CurrbitRead<8)&&(bitsReadForClass < speech_data->num_bits_b) )
      {
        CurrBit = mvs_MaskArray[7-CurrbitRead] & CurrByte ;
        writeBitPos = CurrBitWrite - CurrbitRead ;
        if ( writeBitPos>0 )
        {
          CurrBit = CurrBit >> (CurrBitWrite-CurrbitRead) ;
        }
        else
        {
          CurrBit = CurrBit << (CurrbitRead - CurrBitWrite ) ;
        }

        //CurrBit = CurrBit << CurrBitWrite ;

        pbyDesC = pbyDesC | CurrBit ;
        CurrbitRead++ ;
        CurrBitWrite++ ;
        bitsReadForClass++ ;
        if ( CurrBitWrite == 8 )
        {
          *(pbyDest + CurrByteWrite) = pbyDesC ;
          CurrBitWrite = 0 ;
          CurrByteWrite++ ;
          pbyDesC  = 0x0 ;
        }
      }//process 8 bits

      if ( CurrbitRead == 8 )
      {
        CurrbitRead = 0 ;
        CurrByteRead++ ;
      }

      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }//for all B bits

  }//processed all B channel bits
  bitsReadForClass = 0 ;
  CurrbitRead = 0 ;
  CurrByteRead = 0 ;
  if ( speech_data->num_bits_c )
  {
    if( speech_data->num_bits_a == 0 && speech_data->num_bits_b == 0 )
    {
      pbyDest = AMRIF1Packet;
    }
    pbySrc = speech_data->data_c ;
    //uint8* pbyDest = (AMRIF1Packet+4)+CurrByteWrite ;
    while ( bitsReadForClass < speech_data->num_bits_c )
    {
      CurrByte = *(pbySrc + CurrByteRead) ;
      while ( (CurrbitRead<8)&&(bitsReadForClass < speech_data->num_bits_c) )
      {
        CurrBit = mvs_MaskArray[7-CurrbitRead] & CurrByte ;
        writeBitPos = CurrBitWrite - CurrbitRead ;
        if ( writeBitPos>0 )
        {
          CurrBit = CurrBit >> (CurrBitWrite - CurrbitRead) ;
        }
        else
        {
          CurrBit = CurrBit << (CurrbitRead - CurrBitWrite ) ;
        }

        //CurrBit = CurrBit << CurrBitWrite ;

        pbyDesC = pbyDesC | CurrBit ;
        CurrbitRead++ ;
        CurrBitWrite++ ;
        bitsReadForClass++ ;
        if ( CurrBitWrite == 8 )
        {
          *(pbyDest + CurrByteWrite) = pbyDesC ;
          CurrBitWrite = 0 ;
          CurrByteWrite++ ;
          pbyDesC  = 0x0 ;
        }
      }//process 8 bits

      if ( CurrbitRead == 8 )
      {
        CurrbitRead = 0 ;
        CurrByteRead++ ;
      }

      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }//for all c bits

  }//processed all c channel bits

  *(pbyDest + CurrByteWrite) = pbyDesC ;
  if ( nIF1PacketLengthFormed != NULL ) *nIF1PacketLengthFormed = CurrByteWrite + 1;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_make_if1_from_channels(): " \
        "complete (%d)",
        nResult );

  return nResult ;
}

int32_t mvs_umts_dl_processing (
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
  int32_t nResult = APR_EOK;
  amrsup_core_speech_type amr_speech;
  mvs_amr_speech_buffer_type speech_data;
  mvs_amr_frame_type predicted_frame;
  boolean is_bad_frame;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_umts_dl_processing()" );

  if ( mvs_cur_client != MVS_CLIENT_WCDMA &&
       mvs_cur_client != MVS_CLIENT_TDSCDMA )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_umts_dl_processing(): current " \
          "client is not a UMTS client" );
    return APR_EFAILED;
  }

  if ( vocoder_packet == NULL || frame_info == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_umts_dl_processing(): Invalid " \
          "parameters" );
    return APR_EBADPARAM;
  }

  /* Initialize vocoder packet to zero. */
  memset((void*)(vocoder_packet), 0, VOICE_PACKET_LENGTH);

  mvs_amr_init_core_speech(&amr_speech, &speech_data);

  /* Get per channel speech data from modem via the DSM queues. */
  mvs_dsm_amr_dl_proc(&amr_speech, &predicted_frame, &is_bad_frame);

  /* Determine frame type and mode. */
  frame_info->amr_rate.dl_info.frame = (mvs_amr_frame_type)(amrsup_get_frame_type(&amr_speech));
  frame_info->amr_rate.dl_info.mode = (mvs_amr_mode_type)(amrsup_get_frame_mode(&amr_speech,
                                                                                (amrsup_frame_type)(frame_info->amr_rate.dl_info.frame)));

  /* TODO: Need to make use of the is_bad_frame in mvs_make_if1_from_channels()
   *       to generate SPEECH_BAD or SID_BAD frames.
   */

  /* Create vocoder packet from data in individual channels. */
  nResult = mvs_make_if1_from_channels(vocoder_packet, &amr_speech, VOICE_PACKET_LENGTH, NULL);

  if ( is_bad_frame==TRUE )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Bad frame prediction: (%d) (%d) " \
          "(%d)",
          predicted_frame,
          frame_info->amr_rate.dl_info.frame,
          frame_info->amr_rate.dl_info.mode);
  }

  if ( predicted_frame != frame_info->amr_rate.dl_info.frame )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "Wrong prediction: (%d) (%d)",
            predicted_frame, frame_info->amr_rate.dl_info.frame);
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_umts_dl_processing(): complete" );

  return nResult ;
}

int32_t mvs_umts_ul_processing (
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
  int32_t nResult = APR_EOK;
  amrsup_core_speech_type amr_speech;
  mvs_amr_speech_buffer_type speech_data;
  mvs_amr_chan_state_type chan_state;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_umts_ul_processing()" );

  if ( mvs_cur_client != MVS_CLIENT_WCDMA &&
       mvs_cur_client != MVS_CLIENT_TDSCDMA )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_umts_ul_processing(): current " \
          "client is not a UMTS client" );
    return APR_EFAILED;
  }
  if ( vocoder_packet == NULL || frame_info == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_umts_ul_processing(): Invalid " \
          "parameters" );
    return APR_EBADPARAM;
  }

  mvs_amr_init_core_speech(&amr_speech, &speech_data);

  /* Determine number of bits in each class. */
  amr_speech.num_bits_a = mvs_amr_class_A_bits(frame_info->amr_rate.ul_info.frame, frame_info->amr_rate.ul_info.mode);
  amr_speech.num_bits_b = mvs_amr_class_B_bits(frame_info->amr_rate.ul_info.frame, frame_info->amr_rate.ul_info.mode);
  amr_speech.num_bits_c = mvs_amr_class_C_bits(frame_info->amr_rate.ul_info.frame, frame_info->amr_rate.ul_info.mode);

  chan_state.has_chan_a = (amr_speech.num_bits_a > 0) ? TRUE : FALSE;
  chan_state.has_chan_b = (amr_speech.num_bits_b > 0) ? TRUE : FALSE;
  chan_state.has_chan_c = (amr_speech.num_bits_c > 0) ? TRUE : FALSE;

  if ( mvs_amr_is_subset_chan_state(&chan_state, &(mvs_ul_umts_chan_state.chan_state)) != TRUE )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "ERROR: inconsistent channel " \
            "setup for AMR mode (%d)",
            mvs_client_table[mvs_cur_client].config_mode );
    return APR_EFAILED;
  }

  /* Extract individual channel data from vocoder packet in order to put in separate DSM queues. */
  nResult = mvs_extract_channels_from_if1(vocoder_packet, &amr_speech);
  if ( nResult != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_umts_ul_processing(): " \
            "FAILED to extract individual channel data from vocoder packet " \
            "(%d)",
            nResult );
    return APR_EBADPARAM;
  }

  /* Transport individual AMR channels to modem via DSM queues. */
  mvs_dsm_amr_ul_proc(&amr_speech, chan_state);

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_umts_ul_processing(): complete " \
        "(%d)",
        nResult );

  return nResult;
}

#endif /* !WINSIM */

static int32_t mvs_set_cached_values ( void )
{
  switch ( mvs_client_table[mvs_cur_client].config_mode )
  {
  case MVS_MODE_AMR:
    ( void ) mvs_mod_amr_set_amr_mode( mvs_new_props.amr_ul_mode );
  case MVS_MODE_EFR:
  case MVS_MODE_HR:
  case MVS_MODE_FR:
    ( void ) mvs_mod_set_dtx_mode( mvs_new_props.dtx_mode ) ;
    break;

  case MVS_MODE_IS127:
  case MVS_MODE_IS733:
    ( void ) mvs_mod_voc_tx_rate_limit( mvs_new_props.rate_factor );
    ( void ) mvs_mod_voc_set_frame_rate( mvs_new_props.max_voc_rate, mvs_new_props.min_voc_rate );
    break;

  case MVS_MODE_4GV_NB:
  case MVS_MODE_4GV_WB:
  case MVS_MODE_4GV_NW:
    ( void ) mvs_mod_voc_tx_rate_limit( mvs_new_props.rate_factor );
    ( void ) mvs_mod_voc_set_frame_rate( mvs_new_props.max_voc_rate, mvs_new_props.min_voc_rate );
    ( void ) mvs_mod_set_dtx_mode( mvs_new_props.dtx_mode ) ;
    break;

  case MVS_MODE_AMR_WB:
    ( void ) mvs_mod_amr_set_awb_mode( mvs_new_props.awb_ul_mode );
    ( void ) mvs_mod_set_dtx_mode( mvs_new_props.dtx_mode ) ;
    break;

  default:
    /* Do nothing. */
    break;
  }

  return APR_EOK;
}

static int32_t mvs_change_state_reset (
  mvs_client_type client
)
{
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_change_state_reset(): " \
         "client = (%d)", client );

  switch ( mvs_client_table[client].pending_cmd.cur_cmd )
  {
  case MVS_CMD_ACQUIRE:
    mvs_client_table[client].state = MVS_STATE_IDLE;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_change_state_reset(): " \
           "Invalid command (%d)",
            mvs_client_table[client].pending_cmd.cur_cmd );
    break;
  }

  return APR_EOK;
}

static int32_t mvs_change_state_idle (
  mvs_client_type client
)
{
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_change_state_idle(): " \
         "client = (%d), cur_cmd = (%d)",
         client, mvs_client_table[client].pending_cmd.cur_cmd );

  switch ( mvs_client_table[client].pending_cmd.cur_cmd )
  {
  case MVS_CMD_RELEASE:
    mvs_client_table[client].state = MVS_STATE_RESET;
    break;

  case MVS_CMD_ENABLE:
    if ( mvs_client_table[client].config_mode != MVS_MODE_NONE )
    {
      mvs_client_table[client].state = MVS_STATE_VOCODER;
    }
    break;

  case MVS_CMD_STANDBY:
      mvs_client_table[client].state = MVS_STATE_IDLE;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_change_state_idle(): " \
           "Invalid command (%d)",
            mvs_client_table[client].pending_cmd.cur_cmd );
    break;
  }

  return APR_EOK;
}

static int32_t mvs_change_state_vocoder (
  mvs_client_type client
)
{
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_change_state_vocoder(): " \
         "client = (%d)", client );

  switch ( mvs_client_table[client].pending_cmd.cur_cmd )
  {
  case MVS_CMD_ACQUIRE:
    mvs_client_table[client].state = MVS_STATE_IDLE;
    break;

  case MVS_CMD_STANDBY:
      mvs_client_table[client].state = MVS_STATE_IDLE;
    break;

  case MVS_CMD_ENABLE:
    if ( mvs_client_table[client].config_mode == MVS_MODE_NONE )
    {
      mvs_client_table[client].state = MVS_STATE_IDLE;
    }
    break;

  case MVS_CMD_RELEASE:
    mvs_client_table[client].state = MVS_STATE_RESET;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_change_state_vocoder(): " \
           "Invalid command (%d)",
            mvs_client_table[client].pending_cmd.cur_cmd );
    break;
  }

  return APR_EOK;
}

/* NOTE: This routine updates the MVS state machine transition to the next
 *       state once an MVS control command (mvs_enable(), mvs_standby(),
 *       mvs_release()) has completed.
 */
static int32_t mvs_update_state (
  mvs_client_type client
)
{
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_update_state(): client=(%d)",
        client );

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_update_state(): invalid " \
            "client (%d)", client );
    return APR_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_update_state(): client=(%d), " \
         "cur_state=(%d)",
         client, mvs_client_table[client].state );

  /* TODO: Need to clean this up; it's not obvious what this logic really
   *       means by looking at the variable names. Basically the cmd_status
   *       set to TRUE means there's an MVS control command that's currently
   *       being processed (e.g. mvs_acquire(), mvs_standby(), mvs_enable(),
   *       mvs_release()). The cur_cmd is the command that's running. The
   *       cmd_status is redundant when cur_cmd is already sufficient. The
   *       cur_cmd is reset to MVS_CMD_INVALID when cmd_status if FALSE. Why
   *       would this be necessary?
   */
  if ( mvs_client_table[client].pending_cmd.is_done )
  {
    mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_INVALID;
    return APR_EOK;
  }

  switch ( mvs_client_table[client].state )
  {
  case MVS_STATE_RESET:
    ( void ) mvs_change_state_reset( client );
    break;

  case  MVS_STATE_IDLE:
    ( void ) mvs_change_state_idle( client );
    break;

  case MVS_STATE_VOCODER:
    ( void ) mvs_change_state_vocoder( client );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_update_state(): bad state " \
            "(%d)", mvs_client_table[client].state );
    break;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_update_state(): client=(%d), " \
         "new_state=(%d)",
         client, mvs_client_table[client].state );

  /* TODO: Need to clean this up; consuming the currently pending mvs
   *       command shouldn't be done by this function because it's very tough
   *       to find where commands are consumed outside of the top-level
   *       processing routine.
   */
  mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_INVALID;
  mvs_client_table[client].pending_cmd.is_done = TRUE;

  return APR_EOK;
}

void mvs_state_control_init ( void )
{
  int i = 0;

  for ( i=0; i< MVS_CLIENT_MAX; i++ )
  {
    mvs_client_table[i].state = MVS_STATE_RESET;
    mvs_client_table[i].config_mode = MVS_MODE_NONE;
    mvs_client_table[i].dl_cb = NULL;
    mvs_client_table[i].ul_cb = NULL;
    mvs_client_table[i].event_cb = NULL;
    mvs_client_table[i].pending_cmd.cur_cmd = MVS_CMD_INVALID;
    mvs_client_table[i].pending_cmd.is_done = TRUE;
    mvs_client_table[i].pending_cmd.job_token = 0xf0f0;
  }

  mvs_curr_props.awb_ul_mode = MVS_AMR_MODE_MAX;
  mvs_curr_props.amr_ul_mode = MVS_AMR_MODE_MAX;
  mvs_curr_props.dtx_mode = MVS_DTX_OFF;
  mvs_curr_props.max_voc_rate = MVS_VOC_0_RATE;
  mvs_curr_props.min_voc_rate = MVS_VOC_0_RATE;
  mvs_curr_props.rate_factor = 0;
  mvs_new_props.awb_ul_mode = MVS_AMR_MODE_MAX;
  mvs_new_props.amr_ul_mode = MVS_AMR_MODE_MAX;
  mvs_new_props.dtx_mode = MVS_DTX_OFF;
  mvs_new_props.max_voc_rate = MVS_VOC_0_RATE;
  mvs_new_props.min_voc_rate = MVS_VOC_0_RATE;
  mvs_new_props.rate_factor = 0;
}

int32_t mvs_mod_acquire (
  mvs_client_type client,
  mvs_event_cb_type cb_func
)
{
  int32_t rc;
  mvs_packet_type mvs_cmd;
  mvs_cmd_t* cmd = NULL;
  bool_t is_ok = FALSE;

  mvs_profiling_save_entry( MVS_PROFILING_ACQUIRE_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_acquire(): client = (%d)",
        client );

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_acquire(): " \
           "Invalid client = (%d)", client );
    return APR_EBADPARAM;
  }

  if ( mvs_is_initialized )
  {
    mvs_cmd.acquire.hdr.cmd = MVS_CMD_ACQUIRE;
    mvs_cmd.acquire.client = client;
    mvs_cmd.acquire.cb_func = cb_func;

    rc = mvs_make_cmd( &mvs_cmd, &cmd );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_acquire(): " \
           "Faile to make command" );
      is_ok = FALSE;
    }
    else 
    {
      is_ok = mvs_queue_incoming_cmd( cmd );
    }
  }

  if ( ( is_ok == FALSE ) && ( cb_func != NULL ) )
  {
    mvs_event_type event_data;

    event_data.cmd.hdr.event = MVS_EV_COMMAND;
    event_data.cmd.hdr.client = client;
    event_data.cmd.cmd_status = MVS_CMD_FAILURE;
    cb_func( &event_data );
  }

  mvs_profiling_save_entry( MVS_PROFILING_ACQUIRE_LEAVE );

  return APR_EOK;
}

static int32_t mvs_mod_acquire_proc (
  mvs_pending_control_t *ctrl
)
{
  int32_t rc = APR_EOK;
  mvs_packet_type* acquire_cmd;
  mvs_client_type client;
  mvs_event_cb_type cb_func;

  acquire_cmd = &ctrl->cmd->internal_cmd.type;

  if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_acquire_proc(): client " \
          "(%d)",
          acquire_cmd->acquire.client );

    client =  acquire_cmd->acquire.client;
    cb_func = acquire_cmd->acquire.cb_func;

    if ( client >= MVS_CLIENT_MAX )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_acquire_proc(): " \
             "Invalid client = (%d)", client );
      return APR_EBADPARAM;
    }

    if ( cb_func == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_acquire_proc(): cb_func " \
           "is NULL" );
      return APR_EBADPARAM;
    }

    mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_ACQUIRE;
    mvs_client_table[client].pending_cmd.is_done = FALSE;
    mvs_client_table[client].event_cb = cb_func;

    switch ( mvs_client_table[client].state )
    {
    case MVS_STATE_RESET:
      break;

    case MVS_STATE_IDLE:
    case MVS_STATE_VOCODER:
    default:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_acquire_proc(): " \
               "already acquired. Tell client CMD_SUCCESS. (%d)",
               mvs_client_table[client].state );

        mvs_client_table[client].pending_cmd.is_done = TRUE;
        ( void ) mvs_mem_free_cmd( ctrl->cmd );
        mvs_send_cmd_event( client, MVS_CMD_SUCCESS );
      }
      return APR_EOK;
    }

    ( void ) mvs_mem_free_cmd( ctrl->cmd );
    mvs_send_cmd_event( client, MVS_CMD_SUCCESS );
    mvs_update_state( client );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvs_mod_acquire_proc(): complete client (%d)", client );

    return rc;
  }
  else
  {
    MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  return rc;
}

enum {
  MVS_ENABLE_SEQUENCER_ENUM_UNINITIALIZED,
  MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS,
  MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS_WAIT,
  MVS_ENABLE_SEQUENCER_ENUM_FLUSH_VS_BUFFERS, /* Entry. */
  MVS_ENABLE_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT, /* Entry. */
  MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS, /* Entry. */
  MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS_WAIT, /* Exit. */
  MVS_ENABLE_SEQUENCER_ENUM_COMPLETE,
  MVS_ENABLE_SEQUENCER_ENUM_INVALID
};

static int32_t mvs_mod_enable_sequencer (
  mvs_pending_control_t* ctrl,
  bool_t is_standby,
  bool_t is_standby_only
)
{
  int32_t rc;
  uint32_t _rc;
  mvs_packet_type* cmd;  
  mvs_client_type client;
  mvs_mode_type mode;
  mvs_sequencer_job_object_t* seqjob_obj;
  mvs_simple_job_object_t* subjob_obj;
  vs_voc_cmd_enable_t vs_enable_cmd;
  vs_voc_cmd_standby_t vs_standby_cmd;
  vs_voc_cmd_flush_buffers_t vs_flush_buf_cmd;
  uint32_t vs_handle;
  vs_voc_buffer_t* vs_read_buf;

  cmd = &ctrl->cmd->internal_cmd.type;
  client = cmd->enable.client;
  mode = cmd->enable.mode;

  vs_handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
  vs_read_buf = mvs_client_table[mvs_cur_client].vs_ctrl.read_buf;

  seqjob_obj = &ctrl->rootjob_obj->sequencer_job;

  for ( ;; )
  {
    switch ( seqjob_obj->state )
    {
    case MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS:
      {
        rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

        vs_standby_cmd.handle = vs_handle;
        vs_standby_cmd.media_id = mvs_map_vs_media_id( mode );
        vs_standby_cmd.client_context = ( void* )&subjob_obj->header.handle;
        rc = vs_call( VS_VOC_CMD_STANDBY, ( void* )&vs_standby_cmd,
                      sizeof( vs_standby_cmd ) );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "mvs_mod_enable_sequencer(): Failed to standby VS, " \
                  "rc = (0x%08x)", rc );

          seqjob_obj->status = rc;
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_COMPLETE;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
        }
        else
        {
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS_WAIT;
        }
      }
      continue;

    case MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS_WAIT:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;
        seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_FLUSH_VS_BUFFERS;

        rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
      }
      continue;

    case MVS_ENABLE_SEQUENCER_ENUM_FLUSH_VS_BUFFERS:
      {
        rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

        vs_flush_buf_cmd.handle = vs_handle;
        vs_flush_buf_cmd.client_context = ( void* )&subjob_obj->header.handle;
        rc = vs_call( VS_VOC_CMD_FLUSH_BUFFERS, ( void* )&vs_flush_buf_cmd,
                      sizeof( vs_flush_buf_cmd ) );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "mvs_mod_enable_sequencer(): Failed to flush VS buffers, " \
                  "rc = (0x%08x)", rc );

          seqjob_obj->status = rc;
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_COMPLETE;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
        }
        else
        {
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT;
        }
      }
      continue;

    case MVS_ENABLE_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;

        if ( is_standby_only == TRUE )
        {
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_COMPLETE;
        }
        else
        {
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS;
        }

        rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
      }
      continue;

    case MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS:
      {
        /* Notify call start event. */
        if ( mvs_client_event_cb != NULL )
        {
          ( void ) mvs_client_event_cb( client, MVS_CALL_EVENT_START );
        }

        /* Prime VS read buffer. */
        rc = mvs_prime_vs_read_buf( vs_handle, vs_read_buf );
        if ( rc )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_mod_enable_sequencer(): " \
                 "Failed to prime read buffer, client (%d), rc = (0x%08x)", 
                 client, rc );        
        }
       
        /* Set cached properties. */
        ( void ) mvs_set_cached_values( );
       
        rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

        vs_enable_cmd.handle = vs_handle;
        vs_enable_cmd.media_id = mvs_map_vs_media_id( mode );
        vs_enable_cmd.client_context = ( void* )&subjob_obj->header.handle;
        rc = vs_call( VS_VOC_CMD_ENABLE, ( void* )&vs_enable_cmd,
                      sizeof( vs_enable_cmd ) );
        if ( rc )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "mvs_mod_enable_sequencer(): Failed to enable VS, " \
                  "client (%d), rc = (0x%08X)",
                  client, rc );

          seqjob_obj->status = rc;
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_COMPLETE;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
        }
        else
        {
          seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS_WAIT;
        }
      }
      continue;

    case MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS_WAIT:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;
        seqjob_obj->state = MVS_ENABLE_SEQUENCER_ENUM_COMPLETE;

        rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
      }
      continue;

    case MVS_ENABLE_SEQUENCER_ENUM_COMPLETE:
      {
        voicecfg_cmd_get_bool_item_t item;

        item.id = MVS_CFGID_MVS_EVENT_SEQUENCE;
        item.ret_value = FALSE;

        /* End the sequencer. */
        if ( seqjob_obj->status == APR_EOK )
        {
           _rc = voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof ( item ) );

           if ( !_rc && ( item.ret_value == TRUE ) )       
           {
             mvs_send_mode_event( client, MVS_MODE_INIT, mode ) ;
             if ( ( is_standby == FALSE ) || ( is_standby && mode == MVS_MODE_NONE ) )
             {
               mvs_send_mode_event( client, MVS_MODE_READY, mode) ;
             }
             mvs_send_cmd_event( client, MVS_CMD_SUCCESS );
           }
           else
           {
              mvs_send_cmd_event( client, MVS_CMD_SUCCESS );
              mvs_send_mode_event( client, MVS_MODE_INIT, mode );
              if ( ( is_standby == FALSE ) || ( is_standby && mode == MVS_MODE_NONE ) )
              {
                 mvs_send_mode_event( client, MVS_MODE_READY, mode);
              }
           }

          if ( ( ( client == MVS_CLIENT_WCDMA ) || ( client == MVS_CLIENT_TDSCDMA ) ) && 
               ( is_standby == FALSE ) )
          {
#ifndef WINSIM
            mvs_dsm_amr_activate( TRUE );
            MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_enable_sequencer(): " \
                 "DSM activate done" );
#endif /* !WINSIM */
          }
          /* Send sample rate notification (8 KHz) to NAS/GHDI whenever the
           * vocoder configured is AMR. This is in addition to the sample rate
           * notification that is sent by Modem during call set-up and
           * handovers. 
           *  
           * This is to avoid incorrect sample rate notification to AP during 
           * inter-RAT handovers, when eAMR is not enabled for the current 
           * client and eAMR is enabled for other clients. 
           */
          if ( ( mvs_client_table[mvs_cur_client].config_mode == MVS_MODE_AMR ) &&
               ( is_standby == FALSE ) )
          {
            if ( ( ( mvs_cur_client == MVS_CLIENT_GSM ) && 
                   ( mvs_eamr_config.enable_2g != TRUE ) ) ||
                 ( ( mvs_cur_client == MVS_CLIENT_WCDMA ) && 
                   ( mvs_eamr_config.enable_3g != TRUE ) ) ||
                 ( ( mvs_cur_client == MVS_CLIENT_TDSCDMA ) && 
                   ( mvs_eamr_config.enable_tdscdma != TRUE ) )
               )
            {
              ( void ) mvs_send_eamr_sr_event( mvs_cur_client, MVS_EAMR_SR_8000 );
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_enable_sequencer(): " \
                     "Sends AMR sample rate notification event, "
                     "client = (%d), sample rate = 8000 Hz", mvs_cur_client );
            }
          }
          mvs_update_state( client );
        }
        else
        {
          mvs_send_cmd_event( client, MVS_CMD_FAILURE );
        }

        rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        ctrl->rootjob_obj = NULL;

        ( void ) mvs_mem_free_cmd( ctrl->cmd );

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "mvs_mod_enable_sequencer(): complete - current client = (%d), mode = (%d)",
                mvs_cur_client, mvs_client_table[mvs_cur_client].config_mode );
      }
      return APR_EOK;

    default:
      MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }
  }

  return APR_EOK;
}

int32_t mvs_mod_enable (
  mvs_client_type client,
  mvs_mode_type voc_mode,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
)
{
  int32_t rc;
  mvs_packet_type mvs_cmd;
  mvs_cmd_t* cmd = NULL;
  bool_t is_ok = FALSE;

  mvs_profiling_save_entry( MVS_PROFILING_ENABLE_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_enable(): client = (%d)",
         client );

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_enable(): Invalid client = (%d)",
           client );
    return APR_EBADPARAM;
  }

  if ( mvs_is_initialized )
  {
    mvs_cmd.enable.hdr.cmd = MVS_CMD_ENABLE;
    mvs_cmd.enable.client = client;
    mvs_cmd.enable.mode = voc_mode;
    mvs_cmd.enable.ul_func = ul_func;
    mvs_cmd.enable.dl_func = dl_func;
    mvs_cmd.enable.context = context;

    rc = mvs_make_cmd( &mvs_cmd, &cmd );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_enable(): Failed to make command" );
      is_ok = FALSE;
    }
    else
    {
      is_ok = mvs_queue_incoming_cmd( cmd );
    }
  }

  if ( is_ok == FALSE )
  {
    /* TODO: Need a cleaner solution. If MVS is not initialized, then it
     *       would not be possible to access the mvs_client_table[] required
     *       by mvs_send_cmd_event() anyway.
     */
    mvs_send_cmd_event( client, MVS_CMD_FAILURE );
  }

  mvs_profiling_save_entry( MVS_PROFILING_ENABLE_LEAVE );

  return APR_EOK;
}

static int32_t mvs_mod_enable_proc (
  mvs_pending_control_t* ctrl
)
{
  int32_t rc;
  mvs_packet_type* enable_cmd;
  mvs_client_type client;
  mvs_sequencer_job_object_t* seqjob_obj;
  static bool_t is_standby_only;

  enable_cmd = &ctrl->cmd->internal_cmd.type;

  client = enable_cmd->enable.client;

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_enable_proc(): Invalid client = (%d)",
           client );
    return APR_EBADPARAM;
  }

  if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_enable_proc(): client = " \
          "(%d) mode = (%d)",
          enable_cmd->enable.client, enable_cmd->enable.mode );

    if ( ( mvs_cur_client == MVS_CLIENT_NONE ) || ( mvs_cur_client == client ) )
    {
      mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_ENABLE; 
      mvs_client_table[client].pending_cmd.is_done = FALSE;
      mvs_cur_client = client;
    }
    else
    {
      /* MVS is still not released by other client, report back CMD_FAILURE.
       */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_enable_proc(): MVS " \
             "was not released by mvs_cur_client = (%d),"
             "when client = (%d) called enable_proc",
             mvs_cur_client, client );
      
      mvs_send_mode_event( client, MVS_MODE_NOT_AVAIL, MVS_MODE_NONE );
      mvs_send_cmd_event( client, MVS_CMD_FAILURE );

      mvs_client_table[client].pending_cmd.is_done = TRUE;

      ( void ) mvs_mem_free_cmd( ctrl->cmd );

      return APR_EOK;
    }

    rc = mvs_create_sequencer_job_object(
           ( mvs_sequencer_job_object_t** ) &ctrl->rootjob_obj );
    MVS_PANIC_ON_ERROR( rc );
    seqjob_obj = &ctrl->rootjob_obj->sequencer_job;

    switch ( mvs_client_table[client].state )
    {
    case MVS_STATE_IDLE:
      {
        /* Store the new packet exchange callbacks. */
        mvs_client_table[client].config_mode = enable_cmd->enable.mode;
        mvs_client_table[client].ul_cb = enable_cmd->enable.ul_func;
        mvs_client_table[client].dl_cb = enable_cmd->enable.dl_func;

        if ( enable_cmd->enable.mode != MVS_MODE_NONE )
        {
            ctrl->rootjob_obj->sequencer_job.state = MVS_ENABLE_SEQUENCER_ENUM_ENABLE_VS;

            is_standby_only = FALSE;
        }
        else /* MVS_MODE_NONE */
        {
          /* mvs_enable(mode = NONE) in IDLE state: nothing to. Reply
           * CMD_SUCCESS to client.
           */
          mvs_send_cmd_event( client, MVS_CMD_SUCCESS );

          mvs_client_table[client].pending_cmd.is_done = TRUE;

          rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
          MVS_PANIC_ON_ERROR( rc );
          ctrl->rootjob_obj = NULL;

          ( void ) mvs_mem_free_cmd( ctrl->cmd );

          return APR_EOK;
        }
      }
      break;

    case MVS_STATE_VOCODER:
      {
        /* Store the new packet exchange callbacks. */
        mvs_client_table[client].config_mode = enable_cmd->enable.mode;
        mvs_client_table[client].ul_cb = enable_cmd->enable.ul_func;
        mvs_client_table[client].dl_cb = enable_cmd->enable.dl_func;

        if ( enable_cmd->enable.mode != MVS_MODE_NONE )
        {
          /* mvs_enable(mode != NONE) while running: go through the
           * whole START_VOICE sequence.
           */
          ctrl->rootjob_obj->sequencer_job.state = MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS;

          /* Continue with call setup after standby is done. */
          is_standby_only = FALSE;
        }
        else /* MVS_MODE_NONE */
        {
          /* mvs_enable(mode = NONE) while running: STOP_VOICE. */
          ctrl->rootjob_obj->sequencer_job.state = MVS_ENABLE_SEQUENCER_ENUM_STANDBY_VS;

          /* Complete the sequencer when standby is done. */
          is_standby_only = TRUE;
        }
      }
      break;

    case MVS_STATE_RESET:
      {
        /* Client sent mvs_enable() without having acquired first. Can't do
         * that. Report back CMD_FAILURE.
         */
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_enable_proc(): " \
               "cannot enable in state (%d)",
               mvs_client_table[client].state );
        mvs_send_cmd_event( client, MVS_CMD_FAILURE );

        mvs_client_table[client].pending_cmd.is_done = TRUE;

        rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        ctrl->rootjob_obj = NULL;

        ( void ) mvs_mem_free_cmd( ctrl->cmd );

        return APR_EOK;
      }

    default:
      MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }
  } /* if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE ) */

  rc = mvs_mod_enable_sequencer( ctrl, FALSE, is_standby_only );

  return rc;
}

enum {
  MVS_STANDBY_SEQUENCER_ENUM_UNINITIALIZED, 
  MVS_STANDBY_SEQUENCER_ENUM_STANDBY_VS, /* Entry. */
  MVS_STANDBY_SEQUENCER_ENUM_STANDBY_VS_WAIT, /* Exit. */
  MVS_STANDBY_SEQUENCER_ENUM_FLUSH_VS_BUFFERS, /* Entry. */
  MVS_STANDBY_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT, /* Entry. */
  MVS_STANDBY_SEQUENCER_ENUM_COMPLETE,
  MVS_STANDBY_SEQUENCER_ENUM_INVALID
};

static int32_t mvs_mod_standby_sequencer (
  mvs_pending_control_t* ctrl,
  bool_t is_standby
)
{
  int32_t rc;
  uint32_t _rc;
  mvs_packet_type* cmd;  
  mvs_client_type client;
  mvs_mode_type mode;
  mvs_sequencer_job_object_t* seqjob_obj;
  mvs_simple_job_object_t* subjob_obj;
  uint32_t vs_handle;
  vs_voc_cmd_standby_t vs_standby_cmd;
  vs_voc_cmd_flush_buffers_t vs_flush_buf_cmd;

  cmd = &ctrl->cmd->internal_cmd.type;
  client = cmd->standby.client;
  mode = cmd->standby.mode;

  vs_handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;

  seqjob_obj = &ctrl->rootjob_obj->sequencer_job;

  for ( ;; )
  {
    switch ( seqjob_obj->state )
    {
    case MVS_STANDBY_SEQUENCER_ENUM_STANDBY_VS:
      {
        rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

        vs_standby_cmd.handle = vs_handle;
        vs_standby_cmd.media_id = mvs_map_vs_media_id( mode );
        vs_standby_cmd.client_context = ( void* )&subjob_obj->header.handle;
        rc = vs_call( VS_VOC_CMD_STANDBY, ( void* )&vs_standby_cmd,
                      sizeof( vs_standby_cmd ) );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "mvs_mod_standby_sequencer(): Failed to standby VS, " \
                  "rc = (0x%08X)", rc );

          seqjob_obj->status = rc;
          seqjob_obj->state = MVS_STANDBY_SEQUENCER_ENUM_COMPLETE;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
        }
        else
        {
          seqjob_obj->state = MVS_STANDBY_SEQUENCER_ENUM_STANDBY_VS_WAIT;
        }
      }
      continue;

    case MVS_STANDBY_SEQUENCER_ENUM_STANDBY_VS_WAIT:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;
        seqjob_obj->state = MVS_STANDBY_SEQUENCER_ENUM_FLUSH_VS_BUFFERS;

        rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
      }
      continue;

    case MVS_STANDBY_SEQUENCER_ENUM_FLUSH_VS_BUFFERS:
      {
        rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

        vs_flush_buf_cmd.handle = vs_handle;
        vs_flush_buf_cmd.client_context = ( void* )&subjob_obj->header.handle;
        rc = vs_call( VS_VOC_CMD_FLUSH_BUFFERS, ( void* )&vs_flush_buf_cmd,
                      sizeof( vs_flush_buf_cmd ) );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "mvs_mod_standby_sequencer(): Failed to flush VS buffers, " \
                  "rc = (0x%08X)", rc );

          seqjob_obj->status = rc;
          seqjob_obj->state = MVS_STANDBY_SEQUENCER_ENUM_COMPLETE;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
        }
        else
        {
          seqjob_obj->state = MVS_STANDBY_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT;
        }
      }
      continue;

    case MVS_STANDBY_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;
        seqjob_obj->state = MVS_STANDBY_SEQUENCER_ENUM_COMPLETE;

        rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
      }
      continue;

    case MVS_STANDBY_SEQUENCER_ENUM_COMPLETE:
      {
        voicecfg_cmd_get_bool_item_t item;

        item.id = MVS_CFGID_MVS_EVENT_SEQUENCE;
        item.ret_value = FALSE;

        /* End the sequencer. */
        if ( seqjob_obj->status == APR_EOK )
        {
           _rc = voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof ( item ) );

           if ( !_rc && ( item.ret_value == TRUE ) )       
           {
             mvs_send_mode_event( client, MVS_MODE_INIT, mode ) ;
             if ( ( is_standby == FALSE ) || ( is_standby && mode == MVS_MODE_NONE ) )
             {
               mvs_send_mode_event( client, MVS_MODE_READY, mode) ;
             }
             mvs_send_cmd_event( client, MVS_CMD_SUCCESS );
           }
           else
           {
              mvs_send_cmd_event( client, MVS_CMD_SUCCESS );
              mvs_send_mode_event( client, MVS_MODE_INIT, mode );
              if ( ( is_standby == FALSE ) || ( is_standby && mode == MVS_MODE_NONE ) )
              {
                 mvs_send_mode_event( client, MVS_MODE_READY, mode);
              }
           }

          if ( ( ( client == MVS_CLIENT_WCDMA ) || ( client == MVS_CLIENT_TDSCDMA ) ) && 
               ( is_standby == FALSE ) )
          {
#ifndef WINSIM
            mvs_dsm_amr_activate( TRUE );
            MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_standby_sequencer(): " \
            "DSM activate done" );
#endif /* !WINSIM */
          }
          /* Send sample rate notification (8 KHz) to NAS/GHDI whenever the
           * vocoder configured is AMR. This is in addition to the sample rate
           * notification that is sent by Modem during call set-up and
           * handovers. 
           *  
           * This is to avoid incorrect sample rate notification to AP during 
           * inter-RAT handovers, when eAMR is not enabled for the current 
           * client and eAMR is enabled for other clients. 
           */
          if ( ( mvs_client_table[mvs_cur_client].config_mode == MVS_MODE_AMR ) &&
               ( is_standby == FALSE ) )
          {
            if ( ( ( mvs_cur_client == MVS_CLIENT_GSM ) && 
                   ( mvs_eamr_config.enable_2g != TRUE ) ) ||
                 ( ( mvs_cur_client == MVS_CLIENT_WCDMA ) && 
                   ( mvs_eamr_config.enable_3g != TRUE ) ) ||
                 ( ( mvs_cur_client == MVS_CLIENT_TDSCDMA ) && 
                   ( mvs_eamr_config.enable_tdscdma != TRUE ) )
               )
            {
              ( void ) mvs_send_eamr_sr_event( mvs_cur_client, MVS_EAMR_SR_8000 );
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_standby_sequencer(): " \
                     "Sends AMR sample rate notification event, "
                     "client = (%d), sample rate = 8000 Hz", mvs_cur_client );
            }
          }
          mvs_update_state( client );
        }
        else
        {
          mvs_send_cmd_event( client, MVS_CMD_FAILURE );
        }

        rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        ctrl->rootjob_obj = NULL;

        ( void ) mvs_mem_free_cmd( ctrl->cmd );

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
               "mvs_mod_standby_proc(): complete - current client = (%d) and mode =(%d)",
               mvs_cur_client, mvs_client_table[mvs_cur_client].config_mode );

      }
      return APR_EOK;

    default:
      MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }
  }

  return APR_EOK;
}

int32_t mvs_mod_standby (
  mvs_client_type client,
  mvs_mode_type voc_mode
)
{
  int32_t rc;
  mvs_packet_type mvs_cmd;
  mvs_cmd_t* cmd = NULL;
  bool_t is_ok = FALSE;

  mvs_profiling_save_entry( MVS_PROFILING_STANDBY_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_standby(): client = (%d)",
        client );

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_standby(): Invalid client = (%d)",
            client );
    return APR_EBADPARAM;
  }

  if ( mvs_is_initialized )
  {
    mvs_cmd.standby.hdr.cmd = MVS_CMD_STANDBY;
    mvs_cmd.standby.client = client;
    mvs_cmd.standby.mode = voc_mode;

    rc = mvs_make_cmd( &mvs_cmd, &cmd );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_standby(): Failed to make command" );
      is_ok = FALSE;
    }
    else
    {
      is_ok = mvs_queue_incoming_cmd( cmd );
    }
  }

  if ( is_ok == FALSE )
  {
    /* TODO: Need a cleaner solution. If MVS is not initialized, then it
     *       would not be possible to access the mvs_client_table[] required
     *       by mvs_send_cmd_event() anyway.
     */
    mvs_send_cmd_event( client, MVS_CMD_FAILURE );
  }

  mvs_profiling_save_entry( MVS_PROFILING_STANDBY_LEAVE );

  return APR_EOK;
}

static int32_t mvs_mod_standby_proc (
  mvs_pending_control_t* ctrl
)
{
  int32_t rc;
  mvs_packet_type* standby_cmd;
  mvs_client_type client;
  mvs_sequencer_job_object_t* seqjob_obj;

  standby_cmd = &ctrl->cmd->internal_cmd.type;
  client = standby_cmd->standby.client;

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_standby_proc(): Invalid client = (%d)",
            client );
    return APR_EBADPARAM;
  }

  if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    if ( ( mvs_cur_client == MVS_CLIENT_NONE ) || ( mvs_cur_client == client ) )
    {
      mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_STANDBY;
      mvs_client_table[client].pending_cmd.is_done = FALSE;
      mvs_cur_client = client;
    }
    else
    {
      /* MVS is still not released by other client, report back CMD_FAILURE.
       */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_standby_proc(): MVS " \
             "was not released by mvs_cur_client = (%d),"
             "when client = (%d) called standby_proc",
             mvs_cur_client, client );
      
      mvs_send_mode_event( client, MVS_MODE_NOT_AVAIL, MVS_MODE_NONE );
      mvs_send_cmd_event( client, MVS_CMD_FAILURE );

      mvs_client_table[client].pending_cmd.is_done = TRUE;

      ( void ) mvs_mem_free_cmd( ctrl->cmd );

      return APR_EOK;
    }

    rc = mvs_create_sequencer_job_object(
           ( mvs_sequencer_job_object_t** ) &ctrl->rootjob_obj );
    MVS_PANIC_ON_ERROR( rc );
    seqjob_obj = &ctrl->rootjob_obj->sequencer_job;

    switch ( mvs_client_table[client].state )
    {
    case MVS_STATE_IDLE:
    case MVS_STATE_VOCODER:
      {
        mvs_client_table[client].config_mode = standby_cmd->standby.mode;
        mvs_client_table[client].pending_cmd.is_done = FALSE;

        ctrl->rootjob_obj->sequencer_job.state = MVS_STANDBY_SEQUENCER_ENUM_STANDBY_VS;
      }
      break;

    case MVS_STATE_RESET:
      {
        /* Client sent mvs_standby() without having acquired first. Can't do
         * that. Report back CMD_FAILURE.
         */
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_standby_proc(): " \
               "cannot enable in state (%d)",
               mvs_client_table[client].state );
        mvs_send_cmd_event(client, MVS_CMD_FAILURE);

        mvs_client_table[client].pending_cmd.is_done = TRUE;

        rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        ctrl->rootjob_obj = NULL;

        ( void ) mvs_mem_free_cmd( ctrl->cmd );

        return APR_EOK;
      }

    default:
      MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }
  } /* if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE ) */

  rc = mvs_mod_standby_sequencer( ctrl, TRUE );

  return rc;
}

int32_t mvs_mod_release (
  mvs_client_type client
)
{
  int32_t rc;
  mvs_packet_type mvs_cmd;
  mvs_cmd_t* cmd = NULL;
  bool_t is_ok = FALSE;

  mvs_profiling_save_entry( MVS_PROFILING_RELEASE_ENTER );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_release(): client = (%d)",
        client );

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_release(): Invalid client = (%d)",
            client );
    return APR_EBADPARAM;
  }

  if ( mvs_is_initialized )
  {
    mvs_cmd.release.hdr.cmd = MVS_CMD_RELEASE;
    mvs_cmd.release.client = client;

    rc = mvs_make_cmd( &mvs_cmd, &cmd );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_release(): make cmd failed" );
      is_ok = FALSE;
    }
    else
    {
      is_ok = mvs_queue_incoming_cmd( cmd );
    }
  }

  if ( is_ok == FALSE )
  {
    /* TODO: Need a cleaner solution. If MVS is not initialized, then it
     *       would not be possible to access the mvs_client_table[] required
     *       by mvs_send_cmd_event() anyway.
     */
    mvs_send_cmd_event( client, MVS_CMD_FAILURE );
  }

  mvs_profiling_save_entry( MVS_PROFILING_RELEASE_LEAVE );

  return APR_EOK;
}

static int32_t mvs_mod_release_proc (
  mvs_pending_control_t* ctrl
)
{
  int32_t rc;
  mvs_client_type client;
  mvs_packet_type* release_cmd;
  mvs_sequencer_job_object_t* seqjob_obj;
  mvs_simple_job_object_t* subjob_obj;
  uint32_t vs_handle;
  vs_voc_cmd_disable_t vs_disable_cmd;
  vs_voc_cmd_flush_buffers_t vs_flush_buf_cmd;
  
  enum {
    MVS_SEQUENCER_ENUM_UNINITIALIZED,
    MVS_SEQUENCER_ENUM_DISABLE_VS,
    MVS_SEQUENCER_ENUM_DISABLE_VS_WAIT,
    MVS_SEQUENCER_ENUM_FLUSH_VS_BUFFERS,
    MVS_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT,
    MVS_SEQUENCER_ENUM_COMPLETE,
    MVS_SEQUENCER_ENUM_INVALID
  };

  release_cmd = &ctrl->cmd->internal_cmd.type;
  client = release_cmd->release.client;

  vs_handle = mvs_client_table[client].vs_ctrl.handle;

  if ( client >= MVS_CLIENT_MAX )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_release_proc(): Invalid client = (%d)",
           client );
    return APR_EBADPARAM;
  }

  if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_RELEASE;
    mvs_client_table[client].pending_cmd.is_done = FALSE;

    rc = mvs_create_sequencer_job_object(
           ( mvs_sequencer_job_object_t** ) &ctrl->rootjob_obj );
    MVS_PANIC_ON_ERROR( rc );
    seqjob_obj = &ctrl->rootjob_obj->sequencer_job;

    switch ( mvs_client_table[client].state )
    {
    case MVS_STATE_IDLE:
    case MVS_STATE_VOCODER:
      {
        /* Release process for the current client. */
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_release_proc(): " \
               "releasing in state = (%d), client = (%d)",
               mvs_client_table[client].state, client );

        mvs_send_mode_event( client, MVS_MODE_READY, MVS_MODE_NONE );
        mvs_send_cmd_event( client, MVS_CMD_SUCCESS );

        if ( client == mvs_cur_client )
        {
          /* Notify call stop event. */
          if ( mvs_client_event_cb != NULL )
          {
            ( void ) mvs_client_event_cb( client, MVS_CALL_EVENT_STOP );
          }
         
          ctrl->rootjob_obj->sequencer_job.state = MVS_SEQUENCER_ENUM_DISABLE_VS;
          mvs_cur_client = MVS_CLIENT_NONE;
        }
        else
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_release_proc(): " \
                 "mvs_cur_client = (%d) is not same as client = (%d)",
                 mvs_cur_client, client );
          ctrl->rootjob_obj->sequencer_job.state = MVS_SEQUENCER_ENUM_COMPLETE;
        }

        { /* Deregistering WM channels and clearing event callback. */
#ifndef WINSIM
          if ( mvs_client_table[client].config_mode == MVS_MODE_AMR ||
               mvs_client_table[client].config_mode == MVS_MODE_AMR_WB )
          {
            if ( client == MVS_CLIENT_WCDMA )
            { /* Deregistering WM channels for WCDMA client. */
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,   \
                     "mvs_mod_release_proc(): Deregistering WM channels for " \
                     "client (%d)", client );

              /* Do WCDMA DSM clean-up. */
              mvs_wcdma_set_ul_channel( 0, 0, 0, MVSSUP_UMTS_CHAN_CLASS_NONE );
              mvs_wcdma_set_dl_channel( 0, 0, 0, MVSSUP_UMTS_CHAN_CLASS_NONE, NULL );
              mvs_dsm_amr_activate( FALSE );
            }
            else if ( client == MVS_CLIENT_TDSCDMA )
            { /* Deregistering WM channels for TDSCDMA client. */
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,   \
                     "mvs_mod_release_proc(): Deregistering WM channels for " \
                     "client (%d)", client );
 
              /* Do TD-SCDMA DSM clean-up. */
              mvs_tdscdma_set_ul_channel( 0, 0, 0, MVSSUP_UMTS_CHAN_CLASS_NONE );
              mvs_tdscdma_set_dl_channel( 0, 0, 0, MVSSUP_UMTS_CHAN_CLASS_NONE, NULL );
              mvs_dsm_amr_activate( FALSE );
            }
          }
#endif /* !WINSIM */

          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_release_proc(): " \
                 "Clearing event callbacks for client (%d)", client );

          mvs_client_table[client].dl_cb = NULL;
          mvs_client_table[client].ul_cb = NULL;
          mvs_client_table[client].event_cb = NULL;
          mvs_client_table[client].config_mode = MVS_MODE_NONE;
          mvs_client_table[client].pending_cmd.cur_cmd = MVS_CMD_RELEASE;
          mvs_client_table[client].pending_cmd.is_done = FALSE;
        }
      }
      break;

    case MVS_STATE_RESET:
      {
        /* Client sent mvs_release() without having acquired. */
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_mod_release_proc(): " \
               "releasing in state (%d)",
               mvs_client_table[client].state );

        mvs_send_mode_event( client, MVS_MODE_READY, MVS_MODE_NONE );
        mvs_send_cmd_event( client, MVS_CMD_SUCCESS );

        mvs_client_table[client].event_cb = NULL;
        mvs_client_table[client].pending_cmd.is_done = TRUE;

        rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        ctrl->rootjob_obj = NULL;

        ( void ) mvs_mem_free_cmd( ctrl->cmd );

        return APR_EOK;
      }

    default:
      MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;

    }
  } /* end of if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE ) */

  seqjob_obj = &ctrl->rootjob_obj->sequencer_job;

  /* Ending the current session. */
    for ( ;; )
    {
      switch ( seqjob_obj->state )
      {
      case MVS_SEQUENCER_ENUM_DISABLE_VS:
        {
          rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
          seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

          vs_disable_cmd.handle = vs_handle;
          vs_disable_cmd.client_context = ( void* )&subjob_obj->header.handle;
          rc = vs_call( VS_VOC_CMD_DISABLE, ( void* )&vs_disable_cmd,
                        sizeof( vs_disable_cmd ) );
          if ( rc )
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_mod_release_proc(): " \
                   "Failed to disable VS, rc = (0x%08X)", rc );
            seqjob_obj->status = APR_EOK;
            seqjob_obj->state = MVS_SEQUENCER_ENUM_COMPLETE;
            rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
            MVS_PANIC_ON_ERROR( rc );
            continue;
          }

          seqjob_obj->state = MVS_SEQUENCER_ENUM_DISABLE_VS_WAIT;
        }
        continue;

      case MVS_SEQUENCER_ENUM_DISABLE_VS_WAIT:
        {
          subjob_obj = &seqjob_obj->subjob_obj->simple_job;

          if ( subjob_obj->is_completed == FALSE )
          {
            return APR_EPENDING;
          }

          seqjob_obj->status = subjob_obj->status;
          seqjob_obj->state = MVS_SEQUENCER_ENUM_FLUSH_VS_BUFFERS;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );

        }
        continue;

    case MVS_SEQUENCER_ENUM_FLUSH_VS_BUFFERS:
      {
        rc = mvs_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvs_object_t* ) subjob_obj );

        vs_flush_buf_cmd.handle = vs_handle;
        vs_flush_buf_cmd.client_context = ( void* )&subjob_obj->header.handle;
        rc = vs_call( VS_VOC_CMD_FLUSH_BUFFERS, ( void* )&vs_flush_buf_cmd,
                      sizeof( vs_flush_buf_cmd ) );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "mvs_mod_release_sequencer(): Failed to flush VS buffers, " \
                  "rc = (0x%08X)", rc );

          seqjob_obj->status = rc;
          seqjob_obj->state = MVS_SEQUENCER_ENUM_COMPLETE;

          rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
          MVS_PANIC_ON_ERROR( rc );
        }
        else
        {
          seqjob_obj->state = MVS_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT;
        }
      }
      continue;

    case MVS_SEQUENCER_ENUM_FLUSH_VS_BUFFERS_WAIT:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;
        seqjob_obj->state = MVS_SEQUENCER_ENUM_COMPLETE;

        rc = mvs_free_object( ( mvs_object_t* ) subjob_obj );
        MVS_PANIC_ON_ERROR( rc );
      }
      continue;

      case MVS_SEQUENCER_ENUM_COMPLETE:
        {
          /* End the sequencer. */
          mvs_update_state( client );

          rc = mvs_free_object( ( mvs_object_t* ) seqjob_obj );
          MVS_PANIC_ON_ERROR( rc );
          ctrl->rootjob_obj = NULL;

          ( void ) mvs_mem_free_cmd( ctrl->cmd );

          MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "mvs_mod_release_proc() - complete" );

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
          /* Print out time info at end of call. */
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "TIMING UL info: ul max " \
                 "deliver delay (%d), "
                 "ul max processing delay (%d)",
                 mvs_timing_info.ul_deliver_delay_max,
                 mvs_timing_info.ul_processing_delay_max );
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "ul slow misses count (%d), " \
                 "ul fast misses count (%d)",
                 mvs_timing_info.ul_missed_slow_count,
                 mvs_timing_info.ul_missed_fast_count );
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "TIMING DL info: dl max " \
                 "deliver delay (%d), "
                 "dl max processing delay (%d)",
                 mvs_timing_info.dl_deliver_delay_max,
                 mvs_timing_info.dl_processing_delay_max );
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "dl slow misses count (%d), " \
                 "dl fast misses count (%d)",
                 mvs_timing_info.dl_missed_slow_count,
                 mvs_timing_info.dl_missed_fast_count );

          /* Reset timing data. */  
          memset( ( ( void* ) &mvs_timing_info ), 0, sizeof( mvs_timing_info ) );
          mvs_ul_prev = 0;
          mvs_dl_prev = 0;
          mvs_timing_start_cnt_ul = 0;
          mvs_timing_start_cnt_dl = 0;
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

          return APR_EOK;
        }

      default:
        MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
        return APR_EFAILED;
      }

      break;
    }

  return APR_EPENDING;
}

static uint32_t mvs_process_dl_amr_packet (
  uint8 *vocoder_packet,
  mvs_amr_frame_info_type *amr_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  int mode;
  int frameType;
  vs_vocamr_frame_info_t amr_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_amr_packet()" );

  if ( vocoder_packet == NULL || amr_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_amr_packet(): "
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  mode = (int)(amr_rate->dl_info.mode);
  frameType = (int)(amr_rate->dl_info.frame);

  //Validate mode and type
  if ( mode < MVS_AMR_MODE_0475 || mode > MVS_AMR_MODE_1220 ||
       frameType < MVS_AMR_SPEECH_GOOD || frameType > MVS_AMR_NO_DATA )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_amr_packet(): "
           "Invalid frame_info: mode (%d), type (%d)", mode, frameType );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  frameType = mvs_to_vs_amr_frame_table[ frameType ];

  //Populate the packet header
  *dest_voc_pkt = ((frameType & 0x0F) << 4) | (mode & 0x0F);
  //keep room for packet header at 1st byte
  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,  
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH, 
                vocoder_packet, 
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH );  
  //Determine length of newly formed packet
  *vocoder_packet_size = mvs_amr_frame_len(amr_rate->dl_info.frame, amr_rate->dl_info.mode)
                                           + VOICE_PACKET_HDR_LENGTH;

  mmstd_memcpy( vocoder_packet, VOICE_PACKET_LENGTH, dest_voc_pkt, *vocoder_packet_size );

  /* VS writer buffer handling. */
  amr_frame_info.frame_type = ( vs_vocamr_frame_type_t ) frameType;
  amr_frame_info.codec_mode = ( vs_vocamr_codec_mode_t ) mode;

  nResult = mvs_write_vs_buffer_through( (void*)&amr_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_amr_packet(): " \
          "Failed to write VS buffer (%d)  mode = (%d)",
          nResult, mode );  
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_amr_packet(): " \
        "complete (%d)  mode = (%d)",
        nResult, mode );

  return nResult;
}

static uint32_t mvs_process_ul_amr_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_amr_frame_info_type *amr_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocamr_frame_info_t* amr_frame_info;
  int mode;
  int frameType;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_amr_packet()" );

  if ( vocoder_packet == NULL || amr_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_amr_packet(): " \
          "Invalid parameters" );
    return APR_EBADPARAM;
  }

  //Parse the mode and frame type
  amr_frame_info = ( vs_vocamr_frame_info_t* )vs_frame_info;
  frameType = amr_frame_info->frame_type;
  mode = amr_frame_info->codec_mode;

  frameType = vs_to_mvs_amr_frame_type[ frameType ];

  //Validate mode and type
  if ( mode < MVS_AMR_MODE_0475 || mode > MVS_AMR_MODE_1220 ||
       frameType < MVS_AMR_SPEECH_GOOD || frameType > MVS_AMR_NO_DATA )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_amr_packet(): " \
            "Invalid packet header: mode (%d), type (%d)",
            mode, frameType );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  amr_rate->ul_info.mode = (mvs_amr_mode_type)(mode);
  amr_rate->ul_info.frame = (mvs_amr_frame_type)(frameType);

  *vocoder_packet = ( ( frameType & 0x0F ) << 4 ) | ( mode & 0x0F);

  //Get pointer to the beginning of speech data
  *speechData = vocoder_packet + VOICE_PACKET_HDR_LENGTH;

  //Determine the packet length
  *vocoder_packet_size = mvs_amr_frame_len(amr_rate->dl_info.frame, amr_rate->dl_info.mode);

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_amr_packet(): " \
        "complete (%d) mode = (%d)",
        nResult, mode );

  return nResult;
}

static uint32_t mvs_process_dl_amr_wb_packet (
  uint8 *vocoder_packet,
  mvs_amr_frame_info_type *amr_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int mode;
  int frameType;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocamrwb_frame_info_t amrwb_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_amr_wb_packet()" );

  if ( vocoder_packet == NULL || amr_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_amr_wb_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  mode = (int)(amr_rate->dl_info.mode);
  frameType = (int)(amr_rate->dl_info.frame);

  //Validate mode and type
  if ( mode < MVS_AMR_MODE_0660 || mode > MVS_AMR_MODE_2385 ||
       frameType < MVS_AMR_SPEECH_GOOD || frameType > MVS_AMR_SPEECH_LOST )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_amr_wb_packet(): " \
            "Invalid frame_info: mode (%d), type (%d)",
            mode, frameType );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  frameType = mvs_to_vs_armwb_frame_table[ frameType ];

  //Populate the packet header
  mode -= (int)(MVS_AMR_MODE_0660);
  *dest_voc_pkt = ((frameType & 0x0F) << 4) | (mode & 0x0F);

  mmstd_memcpy(dest_voc_pkt +VOICE_PACKET_HDR_LENGTH,VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH ,vocoder_packet, VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH);
  //Determine length of newly formed packet
  *vocoder_packet_size = mvs_amr_frame_len(amr_rate->dl_info.frame, amr_rate->dl_info.mode)
                                           + VOICE_PACKET_HDR_LENGTH;
  mmstd_memcpy(vocoder_packet,VOICE_PACKET_LENGTH,dest_voc_pkt,
               *vocoder_packet_size);

  /* VS write buffer handling. */
  amrwb_frame_info.frame_type = ( vs_vocamrwb_frame_type_t ) frameType;
  amrwb_frame_info.codec_mode = ( vs_vocamrwb_codec_mode_t ) mode;

  nResult = mvs_write_vs_buffer_through( (void*)&amrwb_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_amr_wb_packet(): " \
          "Failed to write VS buffer (%d)  mode = (%d)",
          nResult, mode );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_amr_wb_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_ul_amr_wb_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_amr_frame_info_type *amr_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocamrwb_frame_info_t* amrwb_frame_info;
  int mode;
  int frameType;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_amr_wb_packet()" );

  if ( vocoder_packet == NULL || amr_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_amr_wb_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  //Parse the mode and frame type
  amrwb_frame_info = ( vs_vocamrwb_frame_info_t* )vs_frame_info;
  frameType = amrwb_frame_info->frame_type;
  mode = amrwb_frame_info->codec_mode;

  mode += (int)(MVS_AMR_MODE_0660);
  frameType = vs_to_mvs_amrwb_frame_type[ frameType ];

  //Validate mode and type
  if ( mode < MVS_AMR_MODE_0660 || mode > MVS_AMR_MODE_2385 ||
       frameType < MVS_AMR_SPEECH_GOOD || frameType > MVS_AMR_SPEECH_LOST )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_amr_wb_packet(): " \
            "Invalid packet header: mode (%d), type (%d)",
            mode, frameType );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  amr_rate->ul_info.mode = (mvs_amr_mode_type)(mode);
  amr_rate->ul_info.frame = (mvs_amr_frame_type)(frameType);

  *vocoder_packet = ( ( frameType & 0x0F ) << 4 ) | ( mode & 0x0F);

  //Get pointer to the beginning of speech data
  *speechData = vocoder_packet + VOICE_PACKET_HDR_LENGTH;

  //Determine the packet length
  *vocoder_packet_size = mvs_amr_frame_len(amr_rate->dl_info.frame, amr_rate->dl_info.mode);

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_amr_wb_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_dl_efr_packet (
  uint8 *vocoder_packet,
  mvs_gsm_frame_info_type *gsm_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int bfi;
  int sid;
  int taf;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocefr_frame_info_t efr_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_efr_packet()" );

  if ( vocoder_packet == NULL || gsm_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_efr_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  taf = (int)(gsm_rate->dl_info.taf);

  switch ( gsm_rate->dl_info.frame )
  {
  case MVS_GSM_SID:
    sid = 2; //Valid SID frame
    bfi = 0;
    break;

  case MVS_GSM_SPEECH_GOOD:
    sid = 0; //speech frame
    bfi = 0;
    break;

  case MVS_GSM_BFI:
    sid = 0; //speech frame
    bfi = 1;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_efr_packet(): " \
            "Invalid frame: (%d)",
            gsm_rate->dl_info.frame );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( (( 0  & 0x0F) << 4) | //BIT 7-4 : Reserved
                     ((taf & 0x01) << 3) | //BIT 3   : Time Allignment Flag (TAF)
                     ((sid & 0x03) << 1) | //BIT 2-1 : SID Frame Indication
                     ((bfi & 0x01)) );     //BIT 0   : Bad Frame Indicator (BFI)

  mmstd_memcpy(dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH,vocoder_packet, VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH);

  *vocoder_packet_size = EFR_MAX_PACKET_DATA_LENGTH + VOICE_PACKET_HDR_LENGTH;

  mmstd_memcpy(vocoder_packet,VOICE_PACKET_LENGTH,dest_voc_pkt, *vocoder_packet_size);

  /* VS writer buffer handling. */
  mmstd_memcpy( &efr_frame_info, sizeof( efr_frame_info ),
                dest_voc_pkt, sizeof( efr_frame_info ) );

  nResult = mvs_write_vs_buffer_through( (void*)&efr_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_efr_packet(): " \
          "Failed to write VS buffer, rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_efr_packet(): " \
        "complete (%d)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_efr_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_gsm_frame_info_type *gsm_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int sid;
  vs_vocefr_frame_info_t* efr_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_efr_packet()" );

  if ( vocoder_packet == NULL || gsm_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_efr_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  //Parse the packet header
  efr_frame_info = ( vs_vocefr_frame_info_t* )vs_frame_info;
  sid = efr_frame_info->sid;

  gsm_rate->ul_info.rate = MVS_GSM_RATE_EFR;

  switch ( sid )
  {
  case 0: //speech frame
    gsm_rate->ul_info.frame = MVS_GSM_SPEECH_GOOD;
    break;

  case 1: //Invalid SID frame
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_efr_packet(): " \
          "Received Invalid SID frame from the encoder. "
          "This should never happen" );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;

  case 2: //Valid SID frame
    gsm_rate->ul_info.frame = MVS_GSM_SID;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_process_ul_efr_packet(): " \
           "Invalid packet header: sid (%d),",
           sid );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  *vocoder_packet = *( uint8_t* )efr_frame_info;

  //Get pointer to the beginning of speech data
  *speechData = vocoder_packet + VOICE_PACKET_HDR_LENGTH;

  *vocoder_packet_size = EFR_MAX_PACKET_DATA_LENGTH;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_efr_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_dl_fr_packet (
  uint8 *vocoder_packet,
  mvs_gsm_frame_info_type *gsm_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int bfi;
  int sid;
  int taf;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocfr_frame_info_t fr_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_fr_packet()" );

  if ( vocoder_packet == NULL || gsm_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_fr_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  taf = (int)(gsm_rate->dl_info.taf);

  switch ( gsm_rate->dl_info.frame )
  {
  case MVS_GSM_SID:
    sid = 2; //Valid SID frame
    bfi = 0;
    break;

  case MVS_GSM_SPEECH_GOOD:
    sid = 0; //speech frame
    bfi = 0;
    break;

  case MVS_GSM_BFI:
    sid = 0; //speech frame
    bfi = 1;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_fr_packet(): " \
            "Invalid frame: (%d)",
            gsm_rate->dl_info.frame );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( (( 0  & 0x0F) << 4) | //BIT 7-4 : Reserved
                     ((taf & 0x01) << 3) | //BIT 3   : Time Allignment Flag (TAF)
                     ((sid & 0x03) << 1) | //BIT 2-1 : SID Frame Indication
                     ((bfi & 0x01)) );     //BIT 0   : Bad Frame Indicator (BFI)

  mmstd_memcpy(dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH,vocoder_packet, VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH);

  *vocoder_packet_size = FR_MAX_PACKET_DATA_LENGTH + VOICE_PACKET_HDR_LENGTH;

  mmstd_memcpy(vocoder_packet,VOICE_PACKET_LENGTH,dest_voc_pkt, *vocoder_packet_size);

  /* VS writer buffer handling. */
  mmstd_memcpy( &fr_frame_info, sizeof( fr_frame_info ),
                dest_voc_pkt, sizeof( fr_frame_info ) );

  nResult = mvs_write_vs_buffer_through( (void*)&fr_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_fr_packet(): " \
          "Failed to write VS buffer, rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_fr_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_ul_fr_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_gsm_frame_info_type *gsm_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_fr_packet()" );

  //FR uplink packet processing is currently identical to EFR packet processing, except...
  nResult = mvs_process_ul_efr_packet(vocoder_packet, vs_frame_info, gsm_rate, speechData, vocoder_packet_size);

  //...for gsm_rate->ul_info.rate setting and packet length which we override here
  gsm_rate->ul_info.rate = MVS_GSM_RATE_FR;
  *vocoder_packet_size = FR_MAX_PACKET_DATA_LENGTH;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_fr_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_dl_hr_packet (
  uint8 *vocoder_packet,
  mvs_hr_frame_info_type *hr_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int bfi;
  int sid;
  int taf;
  int ufi;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vochr_frame_info_t hr_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_hr_packet()" );

  if ( vocoder_packet == NULL || hr_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_hr_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  bfi = (int)(hr_rate->dl_info.bfi);
  taf = (int)(hr_rate->dl_info.taf);
  ufi = (int)(hr_rate->dl_info.ufi);

  switch ( hr_rate->dl_info.frame )
  {
  case MVS_HR_SID:
    sid = 2; //Valid SID frame
    break;

  case MVS_HR_INVALID_SID:
    sid = 1; //Invalid SID frame
    break;

  case MVS_HR_SPEECH_GOOD:
  case MVS_HR_SPEECH_BAD:
    sid = 0; //speech frame
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_hr_packet(): " \
            "Invalid frame: (%d)",
            hr_rate->dl_info.frame );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( (( 0  & 0x07) << 5) | //BIT 7-3 : Reserved
                     ((ufi & 0x01) << 4) | //BIT 4   : Unreliable Frame Indicator (UFI)
                     ((taf & 0x01) << 3) | //BIT 3   : Time Allignment Flag (TAF)
                     ((sid & 0x03) << 1) | //BIT 2-1 : SID Frame Indication
                     ((bfi & 0x01)) );     //BIT 0   : Bad Frame Indicator (BFI)

  //Make room for packet header
  mmstd_memcpy(dest_voc_pkt+VOICE_PACKET_HDR_LENGTH,VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH ,vocoder_packet, VOICE_PACKET_LENGTH 
               - VOICE_PACKET_HDR_LENGTH);
  *vocoder_packet_size = HR_MAX_PACKET_DATA_LENGTH + VOICE_PACKET_HDR_LENGTH;
  mmstd_memcpy(vocoder_packet,VOICE_PACKET_LENGTH,dest_voc_pkt,
               *vocoder_packet_size );

  /* VS writer buffer handling. */
  mmstd_memcpy( &hr_frame_info, sizeof( hr_frame_info ),
                dest_voc_pkt, sizeof( hr_frame_info ) );

  nResult = mvs_write_vs_buffer_through( (void*)&hr_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_hr_packet(): " \
          "Failed to write VS buffer, rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_hr_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_ul_hr_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_hr_frame_info_type *hr_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int sid;
  vs_vochr_frame_info_t* hr_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_hr_packet()" );

  if ( vocoder_packet == NULL || hr_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_hr_packet(): " \
          "Invalid parameters." );
    return APR_EBADPARAM;
  }

  //Parse the packet header
  hr_frame_info = ( vs_vochr_frame_info_t* )vs_frame_info;
  sid = hr_frame_info->sid;

  hr_rate->ul_info.enc_err_flag = 0;

  switch ( sid )
  {
  case 0: //speech frame
    hr_rate->ul_info.frame = MVS_HR_SPEECH_GOOD;
    break;

  case 1: //Invalid SID frame
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_hr_packet(): " \
          "Received Invalid SID frame from "
          "the encoder. This should never happen" );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;

  case 2: //Valid SID frame
    hr_rate->ul_info.frame = MVS_HR_SID;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_hr_packet(): " \
            "Invalid packet header: sid (%d),",
            sid );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  *vocoder_packet = *( uint8_t* )hr_frame_info;

  //Get pointer to the beginning of speech data
  *speechData = vocoder_packet + VOICE_PACKET_HDR_LENGTH;

  *vocoder_packet_size = HR_MAX_PACKET_DATA_LENGTH;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_hr_packet(): " \
        "complete (%d)",
        nResult );

  return nResult;
}

static uint32_t mvs_process_dl_evrca_packet (
  uint8 *vocoder_packet,
  mvs_voc_frame_info_type *voc_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  uint8  pkt_rate = 0;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocevrca_frame_info_t evrca_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrca_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrca_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  switch ( voc_rate->rx_info.rate )
  {
  case MVS_VOC_1_RATE:
    pkt_rate = 4;
    break;
  case MVS_VOC_2_RATE:
    pkt_rate = 3;
    break;
  case MVS_VOC_8_RATE:
    pkt_rate = 1;
    break;
  case MVS_VOC_0_RATE:
    pkt_rate = 0;
    break;
  case MVS_VOC_ERASURE:
    pkt_rate = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrca_packet(): " \
           "Invalid voc_rate : (%d),",
           voc_rate->rx_info.rate );
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( 0x0F & pkt_rate ); //BIT (0-3): Rate; BIT (4-7): Reserved - 0's
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrca_packet() " \
         "voc_rate->rx_info.rate (%d)",
         voc_rate->rx_info.rate );

  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH, 
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH, 
                vocoder_packet, EVRC_MAX_PACKET_DATA_LENGTH );
  *vocoder_packet_size = mvslegacy_evrc_packet_size[voc_rate->rx_info.rate]
                         + VOICE_PACKET_HDR_LENGTH;
  mmstd_memcpy( vocoder_packet, VOICE_PACKET_LENGTH, 
                dest_voc_pkt, *vocoder_packet_size );

  /* VS writer buffer handling. */
  ( void ) mvs_map_evrca_frame_rate( voc_rate->rx_info.rate, &evrca_frame_info.rate );

  nResult = mvs_write_vs_buffer_through( (void*)&evrca_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrca_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrca_packet(): " \
         "complete rc = (0x%08x)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_evrca_packet (
  uint8* vocoder_packet,
  void* vs_frame_info,
  mvs_voc_frame_info_type* voc_rate,
  uint8** speechData,
  uint32* vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocevrca_frame_info_t* evrca_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrca_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrca_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  evrca_frame_info = ( vs_vocevrca_frame_info_t* )vs_frame_info;

  switch ( evrca_frame_info->rate )
  {
  case VS_VOCEVRCA_FRAME_BLANK_RATE:
    voc_rate->tx_info.rate = ( mvs_voc_rate_type )MVS_VOC_0_RATE;
    *vocoder_packet = 0;
    break;
  case VS_VOCEVRCA_FRAME_EIGHTH_RATE:
    voc_rate->tx_info.rate = ( mvs_voc_rate_type )MVS_VOC_8_RATE;
    *vocoder_packet = 1;
    break;
  case VS_VOCEVRCA_FRAME_HALF_RATE:
    voc_rate->tx_info.rate = ( mvs_voc_rate_type )MVS_VOC_2_RATE;
    *vocoder_packet = 3;
    break;
  case VS_VOCEVRCA_FRAME_FULL_RATE:
    voc_rate->tx_info.rate = ( mvs_voc_rate_type )MVS_VOC_1_RATE;
    *vocoder_packet = 4;
    break;
  case VS_VOCEVRCA_FRAME_ERASURE_RATE:
    voc_rate->tx_info.rate = ( mvs_voc_rate_type )MVS_VOC_ERASURE;
    *vocoder_packet = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrca_packet(): " \
           "Invalid rate: (%d)", evrca_frame_info->rate );
    return APR_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrca_packet(): " \
         "rate = (%d)", voc_rate->tx_info.rate );

  //Get pointer to the beginning of speech data
  *speechData = ( uint8* ) (vocoder_packet + VOICE_PACKET_HDR_LENGTH );

  *vocoder_packet_size = mvslegacy_evrc_packet_size[ voc_rate->tx_info.rate ];
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrca_packet(): " \
        "complete (%d)", nResult );

  return nResult;
}

static uint32_t mvs_process_dl_qcelp13k_packet (
  uint8 *vocoder_packet,
  mvs_voc_frame_info_type *voc_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  uint8  pkt_rate = 0;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocqcelp13k_frame_info_t qcelp13k_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_qcelp13k_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_qcelp13k_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  switch ( voc_rate->rx_info.rate )
  {
  case MVS_VOC_1_RATE:
    pkt_rate = 4;
    break;
  case MVS_VOC_2_RATE:
    pkt_rate = 3;
    break;
  case MVS_VOC_8_RATE:
    pkt_rate = 1;
    break;
  case MVS_VOC_4_RATE:
    pkt_rate = 2;
    break;
  case MVS_VOC_0_RATE:
    pkt_rate = 0;
    break;
  case MVS_VOC_ERASURE:
    pkt_rate = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_qcelp13k_packet(): " \
           "Invalid voc_rate : (%d),",
           voc_rate->rx_info.rate );
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( 0x0F & pkt_rate ); //BIT (0-3): Rate
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_qcelp13k_packet() " \
         "voc_rate->rx_info.rate (%d)",
         voc_rate->rx_info.rate );

  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH,
                vocoder_packet, IS733_MAX_PACKET_DATA_LENGTH );
  *vocoder_packet_size = mvslegacy_13k_packet_size[voc_rate->rx_info.rate]
                         + VOICE_PACKET_HDR_LENGTH;
  mmstd_memcpy( vocoder_packet, VOICE_PACKET_LENGTH,
                dest_voc_pkt, *vocoder_packet_size );

  /* VS writer buffer handling. */
  ( void ) mvs_map_qcelp13k_frame_rate( voc_rate->rx_info.rate, &qcelp13k_frame_info.rate );

  nResult = mvs_write_vs_buffer_through( (void*)&qcelp13k_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_qcelp13k_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_qcelp13k_packet(): " \
         "complete rc = (0x%08x)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_qcelp13k_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_voc_frame_info_type *voc_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocqcelp13k_frame_info_t* qcelp13k_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_13k_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_qcelp13k_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  qcelp13k_frame_info = ( vs_vocqcelp13k_frame_info_t* )vs_frame_info;

  switch ( qcelp13k_frame_info->rate )
  {
  case VS_VOCQCELP13K_FRAME_BLANK_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_0_RATE;
    *vocoder_packet = 0;
    break;
  case VS_VOCQCELP13K_FRAME_EIGHTH_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_8_RATE;
    *vocoder_packet = 1;
    break;
  case VS_VOCQCELP13K_FRAME_QUARTER_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_4_RATE;
    *vocoder_packet = 2;
    break;
  case VS_VOCQCELP13K_FRAME_HALF_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_2_RATE;
    *vocoder_packet = 3;
    break;
  case VS_VOCQCELP13K_FRAME_FULL_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_1_RATE;
    *vocoder_packet = 4;
    break;
  case VS_VOCQCELP13K_FRAME_ERASURE_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_ERASURE;
    *vocoder_packet = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_qcelp13k_packet(): " \
           "Invalid rate: (%d)", qcelp13k_frame_info->rate );
    return APR_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_qcelp13k_packet(): " \
         "rate (%d)", voc_rate->tx_info.rate );

  //Get pointer to the beginning of speech data
  *speechData = (uint8*)(vocoder_packet + VOICE_PACKET_HDR_LENGTH);

  *vocoder_packet_size = mvslegacy_13k_packet_size[voc_rate->tx_info.rate];

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_qcelp13k_packet(): " \
         "complete (%d)", nResult );

  return nResult;
}

static uint32_t mvs_process_dl_evrcb_packet (
  uint8 *vocoder_packet,
  mvs_voc_frame_info_type *voc_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  uint8  pkt_rate = 0;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocevrcb_frame_info_t evrcb_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcb_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcb_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  switch ( voc_rate->rx_info.rate )
  {
  case MVS_VOC_1_RATE:
    pkt_rate = 4;
    break;
  case MVS_VOC_2_RATE:
    pkt_rate = 3;
    break;
  case MVS_VOC_8_RATE:
    pkt_rate = 1;
    break;
  case MVS_VOC_4_RATE:
    pkt_rate = 2;
    break;
  case MVS_VOC_0_RATE:
    pkt_rate = 0;
    break;
  case MVS_VOC_ERASURE:
    pkt_rate = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcb_packet(): " \
           "Invalid voc_rate : (%d),",
           voc_rate->rx_info.rate );
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( 0x0F & pkt_rate );  //BIT (0-3): Rate
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcb_packet() " \
         "voc_rate->rx_info.rate (%d)",
         voc_rate->rx_info.rate );

  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH, 
                vocoder_packet, FOURGV_MAX_PACKET_DATA_LENGTH);

  *vocoder_packet_size = mvslegacy_4gv_packet_size[voc_rate->rx_info.rate]
                         + VOICE_PACKET_HDR_LENGTH;
  mmstd_memcpy(vocoder_packet,VOICE_PACKET_LENGTH,dest_voc_pkt,
               *vocoder_packet_size);

  /* VS writer buffer handling. */
  ( void ) mvs_map_evrcb_frame_rate( voc_rate->rx_info.rate, &evrcb_frame_info.rate );

  nResult = mvs_write_vs_buffer_through( (void*)&evrcb_frame_info,
                                         vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                         *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcb_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcb_packet(): " \
         "complete rc = (0x%08x)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_evrcb_packet (
  uint8* vocoder_packet,
  void* vs_frame_info,
  mvs_voc_frame_info_type *voc_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocevrcb_frame_info_t* evrcb_frame_info;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcb_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrcb_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  evrcb_frame_info = ( vs_vocevrcb_frame_info_t* )vs_frame_info;

  switch ( evrcb_frame_info->rate )
  {
  case VS_VOCEVRCB_FRAME_BLANK_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_0_RATE;
    *vocoder_packet = 0;
    break;
  case VS_VOCEVRCB_FRAME_EIGHTH_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_8_RATE;
    *vocoder_packet = 1;
    break;
  case VS_VOCEVRCB_FRAME_QUARTER_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_4_RATE;
    *vocoder_packet = 2;
    break;
  case VS_VOCEVRCB_FRAME_HALF_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_2_RATE;
    *vocoder_packet = 3;
    break;
  case VS_VOCEVRCB_FRAME_FULL_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_1_RATE;
    *vocoder_packet = 4;
    break;
  case VS_VOCEVRCB_FRAME_ERASURE_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_ERASURE;
    *vocoder_packet = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrcb_packet(): " \
           "rate (%d)", evrcb_frame_info->rate );
    return APR_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcb_packet(): " \
         "rate (%d)", voc_rate->tx_info.rate );

  //Get pointer to the beginning of speech data
  *speechData = (uint8*)(vocoder_packet + VOICE_PACKET_HDR_LENGTH);

  *vocoder_packet_size = mvslegacy_4gv_packet_size[voc_rate->tx_info.rate];

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcb_packet(): " \
         "complete rc = (0x%08x)", nResult );

  return nResult;
}

static uint32_t mvs_process_dl_evrcwb_packet (
  uint8 *vocoder_packet,
  mvs_voc_frame_info_type *voc_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  uint8  pkt_rate = 0;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocevrcwb_frame_info_t evrcwb_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcwb_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcwb_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  switch ( voc_rate->rx_info.rate )
  {
  case MVS_VOC_1_RATE:
    pkt_rate = 4;
    break;
  case MVS_VOC_2_RATE:
    pkt_rate = 3;
    break;
  case MVS_VOC_8_RATE:
    pkt_rate = 1;
    break;
  case MVS_VOC_4_RATE:
    pkt_rate = 2;
    break;
  case MVS_VOC_0_RATE:
    pkt_rate = 0;
    break;
  case MVS_VOC_ERASURE:
    pkt_rate = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcwb_packet(): " \
           "Invalid voc_rate : (%d),",
           voc_rate->rx_info.rate );
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( 0x0F & pkt_rate );//BIT (0-3): Rate

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcwb_packet() " \
         "voc_rate->rx_info.rate (%d)",
         voc_rate->rx_info.rate );

  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH,
                vocoder_packet, FOURGV_MAX_PACKET_DATA_LENGTH );
  *vocoder_packet_size = mvslegacy_4gv_WB_packet_size[voc_rate->rx_info.rate]
                         + VOICE_PACKET_HDR_LENGTH;

  mmstd_memcpy(vocoder_packet,VOICE_PACKET_LENGTH,dest_voc_pkt, *vocoder_packet_size);

  /* VS writer buffer handling. */
  ( void ) mvs_map_evrcwb_frame_rate( voc_rate->rx_info.rate, &evrcwb_frame_info.rate );

  nResult = mvs_write_vs_buffer_through( (void*)&evrcwb_frame_info,
                                          vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                          *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcwb_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcwb_packet(): " \
         "complete rc = (0x%08x)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_evrcwb_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_voc_frame_info_type *voc_rate,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocevrcwb_frame_info_t* evrcwb_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcwb_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrcwb_packet(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  evrcwb_frame_info = ( vs_vocevrcwb_frame_info_t* )vs_frame_info;

  switch ( evrcwb_frame_info->rate )
  {
  case VS_VOCEVRCWB_FRAME_BLANK_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_0_RATE;
    *vocoder_packet = 0;
    break;
  case VS_VOCEVRCWB_FRAME_EIGHTH_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_8_RATE;
    *vocoder_packet = 1;
    break;
  case VS_VOCEVRCWB_FRAME_QUARTER_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_4_RATE;
    *vocoder_packet = 1;
    break;
  case VS_VOCEVRCWB_FRAME_HALF_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_2_RATE;
    *vocoder_packet = 3;
    break;
  case VS_VOCEVRCWB_FRAME_FULL_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_1_RATE;
    *vocoder_packet = 4;
    break;
  case VS_VOCEVRCWB_FRAME_ERASURE_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_ERASURE;
    *vocoder_packet = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrcwb_packet(): " \
           "rate (%d),", evrcwb_frame_info->rate );
    return APR_EBADPARAM;
  }
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcwb_packet(): " \
         "rate (%d)", voc_rate->tx_info.rate );

  //Get pointer to the beginning of speech data
  *speechData = (uint8*)(vocoder_packet + VOICE_PACKET_HDR_LENGTH);

  *vocoder_packet_size = mvslegacy_4gv_WB_packet_size[voc_rate->tx_info.rate];

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcwb_packet(): " \
         "complete (%d)", nResult );

  return nResult;
}

static uint32_t mvs_process_dl_evrcnw_packet (
  uint8 *vocoder_packet,
  mvs_voc_frame_info_type *voc_rate,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  uint8  pkt_rate = 0;
  uint8 dest_voc_pkt[VOICE_PACKET_LENGTH];
  vs_vocevrcnw_frame_info_t evrcnw_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcnw_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcnw_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  switch ( voc_rate->rx_info.rate )
  {
  case MVS_VOC_1_RATE:
    pkt_rate = 4;
    break;
  case MVS_VOC_2_RATE:
    pkt_rate = 3;
    break;
  case MVS_VOC_8_RATE:
    pkt_rate = 1;
    break;
  case MVS_VOC_4_RATE:
    pkt_rate = 2;
    break;
  case MVS_VOC_0_RATE:
    pkt_rate = 0;
    break;
  case MVS_VOC_ERASURE:
    pkt_rate = 14;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcnw_packet(): " \
           "Invalid voc_rate : (%d)",
           voc_rate->rx_info.rate );
    return APR_EBADPARAM;
  }

  //Populate packet header
  *dest_voc_pkt = ( 0x0F & pkt_rate ); //BIT (0-3): Rate
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcnw_packet() " \
         "voc_rate->rx_info.rate (%d)",
         voc_rate->rx_info.rate );
  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH,
                vocoder_packet, FOURGV_MAX_PACKET_DATA_LENGTH );

  *vocoder_packet_size = mvslegacy_4gv_NW_packet_size[voc_rate->rx_info.rate]
                         + VOICE_PACKET_HDR_LENGTH;
  mmstd_memcpy( vocoder_packet,VOICE_PACKET_LENGTH, dest_voc_pkt, 
               *vocoder_packet_size );

  /* VS writer buffer handling. */
  ( void ) mvs_map_evrcnw_frame_rate( voc_rate->rx_info.rate, &evrcnw_frame_info.rate );

  nResult = mvs_write_vs_buffer_through( (void*)&evrcnw_frame_info,
                                          vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                                          *vocoder_packet_size - VOICE_PACKET_HDR_LENGTH );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_evrcnw_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_evrcnw_packet(): " \
         "complete rc = (0x%08x)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_evrcnw_packet (
  uint8* vocoder_packet,
  void* vs_frame_info,
  mvs_voc_frame_info_type* voc_rate,
  uint8** speechData,
  uint32* vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  vs_vocevrcnw_frame_info_t* evrcnw_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcnw_packet()" );

  if ( vocoder_packet == NULL || voc_rate == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrcnw_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  evrcnw_frame_info = ( vs_vocevrcnw_frame_info_t* ) vs_frame_info;

  switch ( evrcnw_frame_info->rate )
  {
  case VS_VOCEVRCNW_FRAME_BLANK_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_0_RATE;
    *vocoder_packet = 0;
    break;
  case VS_VOCEVRCNW_FRAME_EIGHTH_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_8_RATE;
    *vocoder_packet = 1;
    break;
  case VS_VOCEVRCNW_FRAME_QUARTER_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_4_RATE;
    *vocoder_packet = 2;
    break;
  case VS_VOCEVRCNW_FRAME_HALF_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_2_RATE;
    *vocoder_packet = 3;
    break;
  case VS_VOCEVRCNW_FRAME_FULL_RATE:
    voc_rate->tx_info.rate =(mvs_voc_rate_type) MVS_VOC_1_RATE;
    *vocoder_packet = 4;
    break;
  case VS_VOCEVRCNW_FRAME_ERASURE_RATE:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_ERASURE;
    *vocoder_packet = 14;
    break;
  case VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL:
    voc_rate->tx_info.rate = (mvs_voc_rate_type)MVS_VOC_8_RATE_NONCRITICAL;
    *vocoder_packet = 255;
    break;
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_evrcnw_packet(): " \
           "Invalid rate: (%d)", evrcnw_frame_info->rate );
    return APR_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcnw_packet(): " \
         "Rate (%d)", voc_rate->tx_info.rate );

  //Get pointer to the beginning of speech data
  *speechData = (uint8*)(vocoder_packet + VOICE_PACKET_HDR_LENGTH);

  *vocoder_packet_size = mvslegacy_4gv_NW_packet_size[voc_rate->tx_info.rate];

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_evrcnw_packet(): " \
         "complete (%d)", nResult );

  return nResult;
}

#if 0
static uint32_t mvs_map_g711alaw_frame_type (
  mvs_g711a_frame_type mvs_frame_type, 
  vs_vocg711alaw_frame_type_t* out_type
)
{
  if ( out_type == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( mvs_frame_type )
  {
  case MVS_G711A_SPEECH_GOOD:
    *out_type = VS_VOCG711ALAW_FRAME_SPEECH_TYPE;
    break;
  case MVS_G711A_SID:
    *out_type = VS_VOCG711ALAW_FRAME_SID_TYPE;
    break;
  case MVS_G711A_NO_DATA:
    *out_type = VS_VOCG711ALAW_FRAME_NTX_TYPE;
    break;
  case MVS_G711A_ERASURE:
    *out_type = VS_VOCG711ALAW_FRAME_ERASURE_TYPE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}

static uint32_t mvs_map_g711mulaw_frame_type (
  mvs_g711a_frame_type mvs_frame_type,
  vs_vocg711mulaw_frame_type_t* out_type
)
{
  if ( out_type == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( mvs_frame_type )
  {
  case MVS_G711A_SPEECH_GOOD:
    *out_type = VS_VOCG711MULAW_FRAME_SPEECH_TYPE;
    break;
  case MVS_G711A_SID:
    *out_type = VS_VOCG711MULAW_FRAME_SID_TYPE;
    break;
  case MVS_G711A_NO_DATA:
    *out_type = VS_VOCG711MULAW_FRAME_NTX_TYPE;
    break;
  case MVS_G711A_ERASURE:
    *out_type = VS_VOCG711MULAW_FRAME_ERASURE_TYPE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}
#endif

/* Special handling for G711a DL:
   Two DL packets are concatenatd and send down. 
   Frame type and mode information is embedded into packet. 
*/
static uint32_t mvs_process_dl_g711a_packet (
  uint8 *vocoder_packet,
  mvs_g711a_frame_info_type *g711a_mode,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int mode;
  int frame_type;
  int frame2_offset;
  uint8 pkt_buf[ VOICE_PACKET_LENGTH ];
  uint8 dest_voc_pkt[ VOICE_PACKET_LENGTH ];
  mvs_frame_info_type mvs_frame_info;
  mvs_pkt_status_type mvs_pkt_status;
  mvs_g711a_vs_frame_info_t g711a_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_g711a_packet()" );

  if ( vocoder_packet == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g711a_packet(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  mode = (int)g711a_mode->dl_info.g711a_mode;
  frame_type = (int)g711a_mode->dl_info.frame;

  if ( mode > MVS_G711A_MODE_ALAW || mode < MVS_G711A_MODE_MULAW ||
       frame_type > MVS_G711A_NO_DATA || frame_type < MVS_G711A_SPEECH_GOOD )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g711a_packet(): " \
           "Invalid frame info: mode = (%d),  frameType = (%d)",
           mode, frame_type );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  *dest_voc_pkt = ( ( ( mode << 2 ) & 0x0F ) | ( frame_type & 0x03 ) );
  
  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH, 
                vocoder_packet, 
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH );

  /* Get the second packet. */
  if ( mvs_client_table[mvs_cur_client].dl_cb )
  {
    mvs_client_table[ mvs_cur_client ].dl_cb( ( uint8* )pkt_buf, 
                                              &mvs_frame_info, 
                                              &mvs_pkt_status );

    frame2_offset = G711_MAX_PACKET_DATA_LENGTH  + VOICE_PACKET_HDR_LENGTH;

    mmstd_memcpy( dest_voc_pkt + frame2_offset + VOICE_PACKET_HDR_LENGTH,
                  VOICE_PACKET_LENGTH - frame2_offset - VOICE_PACKET_HDR_LENGTH,
                  ( void* )pkt_buf, G711_MAX_SG_DATA_LENGTH );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_process_dl_g711a_packet(): "\
         "DL callback function is not set" );
    *vocoder_packet_size = 0;
    return APR_EFAILED;
  }

  mode = (int)mvs_frame_info.g711a_rate.dl_info.g711a_mode;
  frame_type = (int)mvs_frame_info.g711a_rate.dl_info.frame;

  if ( mvs_pkt_status == MVS_PKT_STATUS_SLOW )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_process_dl_g711a_packet(): " \
                                          "Modem did not give second frame, "
                                          "Sending an erasure frame" );
    frame_type = ( int )MVS_G711A_ERASURE;
  }

  if ( mode > MVS_G711A_MODE_ALAW || mode < MVS_G711A_MODE_MULAW ||
       frame_type > MVS_G711A_ERASURE || frame_type < MVS_G711A_SPEECH_GOOD )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g711a_packet(): " \
           "Invalid frame info: mode = (%d),  frameType = (%d)",
           mode, frame_type );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;  
  }

  *( dest_voc_pkt + frame2_offset ) = ( ( ( mode << 2 ) & 0x0F ) | ( frame_type & 0x03 ));

  *vocoder_packet_size = G711_MAX_PACKET_DATA_LENGTH * 2 +  VOICE_PACKET_HDR_LENGTH * 2;
  mmstd_memcpy( vocoder_packet, VOICE_PACKET_LENGTH, dest_voc_pkt, *vocoder_packet_size );

  /* Specical handling here:
     Frame type and mode information is embedded in packet.
     Put dummy frame info for VS. */
  g711a_frame_info.alaw.frame_type = ( vs_vocg711alaw_frame_type_t )( -1 );

  nResult = mvs_write_vs_buffer_through( (void*)&g711a_frame_info,
                                         ( uint8_t* )vocoder_packet, 
                                         *vocoder_packet_size );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g711a_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_g711a_packet(): " \
         "complete (%d)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_g711a_packet (
  uint8 *vocoder_packet,
  void* vs_frame_info,
  mvs_g711a_frame_info_type *g711a_mode,
  uint8 **speechData,
  uint32 *vocoder_packet_size
)
{
  mvs_g711a_mode_type mode;
  mvs_g711a_frame_type frame_type;

  if ( vocoder_packet == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g711a_packet(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  mode = ( mvs_g711a_mode_type )( ( mvs_current_g711_mode == MVS_G711_MODE_MULAW ) ? MVS_G711_MODE_MULAW : MVS_G711_MODE_ALAW );
  frame_type = ( mvs_g711a_frame_type ) ( ( vs_vocg711alaw_frame_info_t* )vs_frame_info )->frame_type;

  g711a_mode->ul_info.g711a_mode = mode;
  g711a_mode->ul_info.frame = frame_type;

  *speechData = vocoder_packet + VOICE_PACKET_HDR_LENGTH;

  *vocoder_packet = ( ( ( mode << 2 ) & 0x0F ) | ( frame_type & 0x03 ) );

  *vocoder_packet_size = G711_MAX_SG_DATA_LENGTH;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvs_process_dl_g711a_packet(): completed" );

  return APR_EOK;
}

#if 0
static uint32_t mvs_map_g729_frame_type (
  mvs_g729a_frame_type mvs_frame_type,
  vs_vocg729_frame_type_t* out_type
)
{
  if ( out_type == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( mvs_frame_type )
  {
  case MVS_G729A_NO_DATA:
    *out_type = VS_VOCG729_FRAME_BLANK_TYPE;
    break;
  case MVS_G729A_SPEECH_GOOD:
    *out_type = VS_VOCG729_FRAME_SPEECH_TYPE;
    break;
  case MVS_G729A_SID:
    *out_type = VS_VOCG729_FRAME_SID_TYPE;
    break;
  default:
    return APR_EFAILED;
  }

  return APR_EOK;
}
#endif

/* Special handling for G729 DL:
   Two DL packets are concatenatd and send down. 
   Frame type and mode information is embedded into packet. 
*/
static uint32_t mvs_process_dl_g729_packet (
  uint8 *vocoder_packet,
  mvs_g729a_frame_info_type *g729a_mode,
  uint32 *vocoder_packet_size
)
{
  uint32_t nResult = APR_EOK;
  int frame_type;
  int frame2_offset;
  uint8 pkt_buf[ VOICE_PACKET_LENGTH ];
  uint8 dest_voc_pkt[ VOICE_PACKET_LENGTH ];
  mvs_frame_info_type mvs_frame_info;
  mvs_pkt_status_type mvs_pkt_status;
  vs_vocg729_frame_info_t g729_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_g729_packet()" );

  if ( vocoder_packet == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g729_packet(): " \
         "Invalid parameters." );
    return APR_EBADPARAM;
  }

  frame_type = (int)g729a_mode->dl_info.frame;
  if ( frame_type >  MVS_G729A_SID  || frame_type < MVS_G729A_NO_DATA )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_process_dl_g729_packet(): " \
           "Invalid frame frameType: (%d)", frame_type );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  *dest_voc_pkt = ( frame_type & 0x03 );

  mmstd_memcpy( dest_voc_pkt + VOICE_PACKET_HDR_LENGTH,
                VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH, 
                vocoder_packet, VOICE_PACKET_LENGTH - VOICE_PACKET_HDR_LENGTH );

  /* Get the second packet. */
  if ( mvs_client_table[ mvs_cur_client ].dl_cb )
  {
    mvs_client_table[ mvs_cur_client ].dl_cb( ( uint8* )pkt_buf, 
                                              &mvs_frame_info, 
                                              &mvs_pkt_status );

    frame2_offset = G729A_MAX_FRAME_DATA_LENGTH  + VOICE_PACKET_HDR_LENGTH;

    mmstd_memcpy( dest_voc_pkt + frame2_offset + VOICE_PACKET_HDR_LENGTH,
                  VOICE_PACKET_LENGTH - frame2_offset - VOICE_PACKET_HDR_LENGTH,
                  ( void* )pkt_buf, G729A_MAX_FRAME_DATA_LENGTH );
  
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g729_packet(): DL " \
          "callback function is not set" );
    *vocoder_packet_size = 0;
    return APR_EFAILED;
  }

  frame_type = (int)mvs_frame_info.g729a_rate.dl_info.frame;

  if ( mvs_pkt_status == MVS_PKT_STATUS_SLOW )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g729_packet(): Modem did " \
          "not give second frame, "
          "Sending an erasure frame" );
    frame_type = (int)MVS_G729A_ERASURE;
  }

  if ( frame_type > MVS_G729A_ERASURE || frame_type < MVS_G729A_NO_DATA )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g729_packet(): " \
           "Invalid frameType (%d)", frame_type );
    *vocoder_packet_size = 0;
    return APR_EBADPARAM;
  }

  *( dest_voc_pkt + frame2_offset ) = ( frame_type & 0x03 );

  /* Specical handling here:
     Frame type information is embedded in packet.
     Put dummy frame info for VS. */
  *vocoder_packet_size = G729A_MAX_PACKET_DATA_LENGTH * 2 + VOICE_PACKET_HDR_LENGTH * 2;
  mmstd_memcpy( vocoder_packet, VOICE_PACKET_LENGTH, 
                dest_voc_pkt, *vocoder_packet_size );

  /* VS writer buffer handling. */
  g729_frame_info.frame_type = ( vs_vocg729_frame_type_t)( -1 );

  nResult = mvs_write_vs_buffer_through( (void*)&g729_frame_info,
                                         ( uint8* )vocoder_packet, 
                                         *vocoder_packet_size );
  if ( nResult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_dl_g729_packet(): " \
           "Failed to write VS buffer rc = (0x%08x)", nResult );  
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_dl_g729_packet(): " \
         "complete (%d)", nResult );

  return nResult;
}

static uint32_t mvs_process_ul_g729_packet (
  uint8* vocoder_packet,
  void* vs_frame_info,
  mvs_g729a_frame_info_type* g729a_mode,
  uint8** speechData,
  uint32* vocoder_packet_size
)
{
  mvs_g729a_frame_type frame_type;
  vs_vocg729_frame_info_t* g729_frame_info;

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_ul_g729_packet()" );

  if ( vocoder_packet == NULL || speechData == NULL || vocoder_packet_size == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_g729_packet(): " \
          "Invalid parameter" );
    return APR_EBADPARAM;
  }

  g729_frame_info = ( vs_vocg729_frame_info_t* )vs_frame_info;

  switch ( g729_frame_info->frame_type )
  {
    case VS_VOCG729_FRAME_BLANK_TYPE:
      frame_type = MVS_G729A_NO_DATA;
      break;
    case VS_VOCG729_FRAME_SPEECH_TYPE:
      frame_type = MVS_G729A_SPEECH_GOOD;
      break;
    case VS_VOCG729_FRAME_SID_TYPE:
      frame_type = MVS_G729A_SID;
      break;
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_ul_g729_packet(): " \
             "Invalid frame_info: frame type = (%d)", g729_frame_info->frame_type );
      *vocoder_packet_size = 0;
      return APR_EBADPARAM;
  }

  *speechData = vocoder_packet + VOICE_PACKET_HDR_LENGTH;

  *vocoder_packet = ( frame_type & 0x03 );

  g729a_mode->ul_info.frame = frame_type;

  *vocoder_packet_size = G729A_MAX_FRAME_DATA_LENGTH ;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvs_process_ul_g729_packet(): completed" );

  return APR_EOK;
}

static int32_t mvs_deliver_ul_packet (
  void* vocoder_packet,
  //uint32 vocoder_packet_size
  void* vs_frame_info
)
{
  mvs_frame_info_type mvs_frame_info;
  mvs_pkt_status_type mvs_pkt_status;
  uint32 newPacketLength = 0;
  uint8* speechData = NULL;
  uint32_t nRes;
  int32_t logresult = APR_EOK;
#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
  timetick_type timetick;
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

  /* Verify parameters. */
  if ( vocoder_packet == NULL )
  {
    return APR_EBADPARAM;
  }

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_deliver_ul_packet()");

  //Convert packet to format expected by modem (strip off the header)
  switch ( mvs_client_table[mvs_cur_client].config_mode )
  {
  case MVS_MODE_AMR:
    nRes = mvs_process_ul_amr_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.amr_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_AMR_UL;
    break;

  case MVS_MODE_AMR_WB:
    nRes = mvs_process_ul_amr_wb_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.amr_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_AMR_UL;
    break;

  case MVS_MODE_EFR:
    nRes = mvs_process_ul_efr_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.gsm_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_GSM_UL;
    break;

  case MVS_MODE_FR:
    nRes = mvs_process_ul_fr_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.gsm_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_GSM_UL;
    break;

  case MVS_MODE_HR:
    nRes = mvs_process_ul_hr_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.hr_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_HR_UL;
    break;

  case MVS_MODE_IS127:
    nRes = mvs_process_ul_evrca_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.voc_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_TX;
    break;

  case MVS_MODE_IS733:
    nRes = mvs_process_ul_qcelp13k_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.voc_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_TX;
    break;

  case MVS_MODE_4GV_NB:
    nRes = mvs_process_ul_evrcb_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.voc_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_TX;
    break;

  case MVS_MODE_4GV_WB:
    nRes = mvs_process_ul_evrcwb_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.voc_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_TX;
    break;

  case MVS_MODE_4GV_NW:
    nRes = mvs_process_ul_evrcnw_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.voc_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_TX;
    break;

  case MVS_MODE_G711A:
    nRes = mvs_process_ul_g711a_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.g711a_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_G711_UL;
    break;

  case MVS_MODE_G729A:
    nRes = mvs_process_ul_g729_packet((uint8*)(vocoder_packet), vs_frame_info, &(mvs_frame_info.g729a_rate), &speechData, &newPacketLength);
    mvs_frame_info.hdr.frame_mode =  MVS_FRAME_MODE_G729A_UL;
    break;

  default:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_NONE;
    return APR_EUNSUPPORTED;
  }

  mvs_frame_info.hdr.mode = mvs_client_table[mvs_cur_client].config_mode;

  if ( nRes != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_ul_packet(): " \
            "Failed to process uplink vocoder packet with result (%d)",
            nRes );
    return APR_EFAILED;
  }

  /* Give vocoder packet to modem via callback funtion provided by the client. */
  if ( mvs_client_table[mvs_cur_client].ul_cb )
  {
#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
    timetick = timetick_get( );
    /* Check to see if we have largest processing delay in uplink. */
    if ( ( mvs_timing_start_cnt_ul > mvs_timing_start_delay ) && 
         ( timetick - mvs_ul_prev ) > mvs_timing_info.ul_processing_delay_max )
    {  
      mvs_timing_info.ul_processing_delay_max = timetick - mvs_ul_prev;
    }
    if ( mvs_debug_timing )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_deliver_ul_packet(): " \
             "timetick of UL packet before sending to client (%d)",
             timetick );
    }
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

    mvs_client_table[mvs_cur_client].ul_cb(
      speechData, &mvs_frame_info, ( ( uint16_t ) newPacketLength ),
      &mvs_pkt_status );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_deliver_ul_packet(): " \
           "UL packet status: (%d)", mvs_pkt_status );

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
    if ( mvs_pkt_status == MVS_PKT_STATUS_FAST )
    {
      if ( mvs_timing_start_cnt_ul > mvs_timing_start_delay )
      {
        ++mvs_timing_info.ul_missed_fast_count;
      }
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_ul_packet(): " \
           "UL packet to modem was too fast" );
    }
    if ( mvs_pkt_status == MVS_PKT_STATUS_SLOW )
    {
      if ( mvs_timing_start_cnt_ul > mvs_timing_start_delay )
      {
        ++mvs_timing_info.ul_missed_slow_count;
      }
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_ul_packet(): " \
           "UL packet to modem was too slow" );
    }
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */
  }
  else
  {
    return APR_EFAILED;
  }

  switch ( mvs_client_table[mvs_cur_client].config_mode )
  {
  case MVS_MODE_IS127:
  case MVS_MODE_IS733:
  case MVS_MODE_4GV_NB:
  case MVS_MODE_4GV_WB:
  case MVS_MODE_4GV_NW:
    if ( log_status(LOG_VOC_REV_C) )
    {
      logresult = LogCDMATxPacket(mvs_client_table[mvs_cur_client].config_mode,
                                  mvs_frame_info.voc_rate.tx_info.rate,(void*)vocoder_packet);
    }
    break;
  case MVS_MODE_AMR:
  case MVS_MODE_AMR_WB:
  case MVS_MODE_EFR:
  case MVS_MODE_FR:
  case MVS_MODE_HR:
    if ( log_status(LOG_UMTS_TX_VOCODER_PACKET_DSP_C) )
    {
      logresult = LogUMTSTxPacket(mvs_client_table[mvs_cur_client].config_mode,mvs_frame_info,
                                  (void*)vocoder_packet);
    }
    break;
  default:
    logresult = APR_EBADPARAM;
    break;
  }
  if ( APR_EOK != logresult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_deliver_ul_packet(): " \
           "Packet was not logged result (%d)", logresult );
  }

  return APR_EOK ;
}

static uint32_t mvs_deliver_dl_packet (
  void *vocoder_packet
)
{
  uint8 pkt_buf[ VOICE_PACKET_LENGTH ];
  uint16 pkt_size;
  uint32_t vocoder_packet_size = 0;
  mvs_frame_info_type mvs_frame_info;
  mvs_pkt_status_type mvs_pkt_status;
  uint32_t nRes;
  int32_t logresult = APR_EOK;
#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
  timetick_type timetick;
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

  /* Verify parameters. */
  if ( vocoder_packet == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  //MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_deliver_dl_packet():" );

  /* Initializing mvs frame info header info */
  mvs_frame_info.hdr.mode = mvs_client_table[mvs_cur_client].config_mode;

  switch ( mvs_client_table[mvs_cur_client].config_mode )
  {
  case MVS_MODE_AMR:
  case MVS_MODE_AMR_WB:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_AMR_DL;
    break;

  case MVS_MODE_EFR:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_GSM_DL;
    break;

  case MVS_MODE_FR:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_GSM_DL;
    break;

  case MVS_MODE_HR:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_HR_DL;
    break;

  case MVS_MODE_IS127:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_RX;
    break;

  case MVS_MODE_IS733:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_RX;
    break;

  case MVS_MODE_4GV_NB:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_RX;
    break;

  case MVS_MODE_4GV_WB:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_RX;
    break;

  case MVS_MODE_4GV_NW:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_VOC_RX;
    break;

  case MVS_MODE_G711A:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_G711A_DL;
    break;

  case MVS_MODE_G729A:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_G729A_DL;
    break;

  default:
    mvs_frame_info.hdr.frame_mode = MVS_FRAME_MODE_NONE;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): " \
           "Invalid mode = (%d)", mvs_client_table[mvs_cur_client].config_mode );
    return APR_EUNSUPPORTED;
  }

  /* Obtain downlink vocoder packet from modem via callback function provided by the client. */
  if ( mvs_client_table[mvs_cur_client].dl_cb )
  {
    mvs_client_table[mvs_cur_client].dl_cb((uint8*)pkt_buf, &mvs_frame_info, &mvs_pkt_status);

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
    timetick = timetick_get( );

    /* Check to see if we have largest delay in uplink. */
    if ( ( mvs_timing_start_cnt_dl > mvs_timing_start_delay ) &&
         ( mvs_dl_prev != 0 ) &&
         ( ( timetick - mvs_dl_prev ) > mvs_timing_info.dl_deliver_delay_max ) )
    {  
      mvs_timing_info.dl_deliver_delay_max = timetick - mvs_dl_prev;
    }

    if ( mvs_cur_client == MVS_CLIENT_WCDMA )
    {
      if ( mvs_debug_timing )
      {
       /*STMR_STATUS_DUMP(STMR_STATUS_DUMP_CMD_IMMEDIATE);
        mvs_curr_stmr_time_cx8 = STMR_GET_TX_FRAME_COUNT_STATUS();
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "DL WCDMA  DPCCH Chipx8 (%d)",
               mvs_curr_stmr_time_cx8  );*/
      }
    }

    /* Set prev to current time tick. */
    ++mvs_timing_start_cnt_dl;
    mvs_dl_prev = timetick;

    if ( mvs_debug_timing )
    {
      //MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_deliver_dl_packet(): " \
      //       "sending from client at timetick (%d)", timetick );
    }
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

    /* Biggest size Q6 allowed is only VOICE_PACKET_LENGTH. */
    pkt_size = VOICE_PACKET_LENGTH;
    mmstd_memcpy((void*)(vocoder_packet), pkt_size, (void*)(pkt_buf), pkt_size);

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_deliver_dl_packet(): "
           "DL packet status: (%d)", mvs_pkt_status );

    if ( mvs_pkt_status == MVS_PKT_STATUS_SLOW )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): " \
           "Modem did not give any packet. Nothing to give DSP" );

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
      if ( mvs_timing_start_cnt_dl > mvs_timing_start_delay )
      {
        ++mvs_timing_info.dl_missed_slow_count;
      }
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

      vocoder_packet_size = 0;
      return APR_EOK;
    }

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
    if ( mvs_pkt_status == MVS_PKT_STATUS_FAST)
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): " \
           "DL packet to modem was too fast" );

      if ( mvs_timing_start_cnt_dl > mvs_timing_start_delay )
      {
        mvs_timing_info.dl_missed_fast_count++;
      }
    }
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): " 
         "DL callback function is not set" );
    vocoder_packet_size = 0;
    return APR_EFAILED;
  }

  //Convert packet to format expected by decoder (add the header)
  switch ( mvs_client_table[mvs_cur_client].config_mode )
  {
  case MVS_MODE_AMR:
    nRes = mvs_process_dl_amr_packet((uint8*)(vocoder_packet), &(mvs_frame_info.amr_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_AMR_WB:
    nRes = mvs_process_dl_amr_wb_packet((uint8*)(vocoder_packet), &(mvs_frame_info.amr_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_EFR:
    nRes = mvs_process_dl_efr_packet((uint8*)(vocoder_packet), &(mvs_frame_info.gsm_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_FR:
    nRes = mvs_process_dl_fr_packet((uint8*)(vocoder_packet), &(mvs_frame_info.gsm_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_HR:
    nRes = mvs_process_dl_hr_packet((uint8*)(vocoder_packet), &(mvs_frame_info.hr_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_IS127:
    nRes = mvs_process_dl_evrca_packet((uint8*)(vocoder_packet), &(mvs_frame_info.voc_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_IS733:
    nRes = mvs_process_dl_qcelp13k_packet((uint8*)(vocoder_packet), &(mvs_frame_info.voc_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_4GV_NB:
    nRes = mvs_process_dl_evrcb_packet((uint8*)(vocoder_packet), &(mvs_frame_info.voc_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_4GV_WB:
    nRes = mvs_process_dl_evrcwb_packet((uint8*)(vocoder_packet), &(mvs_frame_info.voc_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_4GV_NW:
    nRes = mvs_process_dl_evrcnw_packet((uint8*)(vocoder_packet), &(mvs_frame_info.voc_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_G711A:
    nRes = mvs_process_dl_g711a_packet((uint8*)(vocoder_packet), &(mvs_frame_info.g711a_rate), &vocoder_packet_size);
    break;

  case MVS_MODE_G729A:
    nRes = mvs_process_dl_g729_packet((uint8*)(vocoder_packet), &(mvs_frame_info.g729a_rate), &vocoder_packet_size);
    break;

  default:
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): Invalid " \
          "mode" );
    return APR_EUNSUPPORTED;
  }

  if ( nRes != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_deliver_dl_packet(): Failed to " \
            "process downlink vocoder packet with result: (%d)",
            nRes );
    vocoder_packet_size = 0;
    return APR_EOK;
  }

  switch ( mvs_client_table[mvs_cur_client].config_mode )
  {
  case MVS_MODE_IS127:
  case MVS_MODE_IS733:
  case MVS_MODE_4GV_NB:
  case MVS_MODE_4GV_WB:
  case MVS_MODE_4GV_NW:
    if ( log_status(LOG_VOC_FOR_C) )
    {
      logresult =  LogCDMARxPacket(mvs_client_table[mvs_cur_client].config_mode,
                                   mvs_frame_info.voc_rate.rx_info.rate ,vocoder_packet);
    }
    break;
  case MVS_MODE_AMR:
  case MVS_MODE_AMR_WB:
  case MVS_MODE_EFR:
  case MVS_MODE_FR:
  case MVS_MODE_HR:
    if ( log_status(LOG_UMTS_RX_VOCODER_PACKET_DSP_C) )
    {
      logresult = LogUMTSRxPacket(mvs_client_table[mvs_cur_client].config_mode,
                                  mvs_frame_info,vocoder_packet);
    }
    break;
  default:
    logresult = APR_EBADPARAM;
    break;
  }
  if ( APR_EOK != logresult )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_deliver_dl_packet(): " \
           "Packet was not logged result (%d)", logresult);
  }

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
  timetick = timetick_get( );

  /* Check to see if we have largest delay in uplink. */
  if ( ( mvs_timing_start_cnt_dl > mvs_timing_start_delay ) && 
       ( ( timetick - mvs_dl_prev ) > mvs_timing_info.dl_processing_delay_max ) )
  {
    mvs_timing_info.dl_processing_delay_max = timetick - mvs_dl_prev;
  }

  if ( mvs_debug_timing )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_deliver_dl_packet(): " \
           "timetick of DL packet before sending to DSP    (%d)", timetick );
  }
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

  return APR_EOK;
}

/****************************************************************************
 * SILENCE FRAME GENERATION                                                 *
 ****************************************************************************/

/* This function is only intended to be used by MVA.
   MVA shall only call this function once.
 */
uint32_t mvs_listen_for_event (
  mvs_event_fn_t event_cb
)
{
  if ( event_cb == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_listen_for_event(): " \
         "Callback function is NULL, treat as un-listen" );
  }

  if ( mvs_is_initialized )
  {
    mvs_client_event_cb = event_cb;
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_listen_for_event(): " \
         "MVS not initialized yet" );    
    return APR_ENOTREADY;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_listen_for_event() completed" );

  return APR_EOK;
}

uint32_t mvs_get_timing_offset (
  mvs_client_type client,
  mvs_timing_offset_type* offset
)
{
  if ( offset == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_get_timing_offset(): " \
         "Invalid parameter" );
    return APR_EBADPARAM;
  }

  switch ( client )
  {
  case MVS_CLIENT_WCDMA:
    offset->dec_offset = mvs_wcdma_timing_offset.dec_offset;
    offset->dec_req_offset = mvs_wcdma_timing_offset.dec_req_offset;
    offset->enc_offset = mvs_wcdma_timing_offset.enc_offset;
    break;

  case MVS_CLIENT_TDSCDMA:
    offset->dec_offset = mvs_tdscdma_timing_offset.dec_offset;
    offset->dec_req_offset = mvs_tdscdma_timing_offset.dec_req_offset;
    offset->enc_offset = mvs_tdscdma_timing_offset.enc_offset;
    break;

  default:
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_get_timing_offset(): " \
         "Unsupported client" );
    return APR_EUNSUPPORTED;
  }

  return APR_EOK;
}

uint32_t mvs_send_silence_frame (
  void 
)
{
  int32_t rc;
  mvs_packet_type mvs_cmd;
  mvs_cmd_t* cmd = NULL;
  bool_t is_ok = FALSE;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_send_silence_frame()" );

  if ( mvs_is_initialized )
  {
    mvs_cmd.hdr.cmd = MVS_CMD_SEND_SILENCE_FRAME;

    rc = mvs_make_cmd( &mvs_cmd, &cmd );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_send_silence_frame(): " \
           "Faile to make command" );
      is_ok = FALSE;
    }
    else 
    {
      is_ok = mvs_queue_incoming_cmd( cmd );
    }
  }

  if ( is_ok == TRUE )
  {
    rc = APR_EOK;
  }
  else
  {
    rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t mvs_mod_send_silence_frame_proc (
  mvs_pending_control_t *ctrl
)
{
  uint32_t rc;
  mvs_vs_ctrl_type* vs_ctrl;
  uint8_t packet[ VOICE_PACKET_LENGTH ];
  uint32_t size;
  uint32_t media_id;
  void* frame_info;
  vs_vocamr_frame_info_t amr_frame_info;
  vs_vocamrwb_frame_info_t amrwb_frame_info;

  vs_ctrl = &mvs_client_table[ mvs_cur_client ].vs_ctrl;

  media_id = mvs_map_vs_media_id( mvs_client_table[mvs_cur_client].config_mode );

  if ( ctrl->state == MVS_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    for ( ;; )
    {
      if ( vs_ctrl->is_vs_ready == TRUE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvs_mod_send_silence_frame_proc(): " \
             "Skip silence frame generation" );
        rc = APR_EOK;
        break;
      }
      else
      {
        if ( media_id == VS_COMMON_MEDIA_ID_AMR )
        {
          amr_frame_info.frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD;
          amr_frame_info.codec_mode = ( vs_vocamr_codec_mode_t )mvs_new_props.amr_ul_mode;
          frame_info = ( void* )&amr_frame_info;
        }
        else if ( media_id == VS_COMMON_MEDIA_ID_AMRWB )
        {
          amrwb_frame_info.frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
          amrwb_frame_info.codec_mode = (vs_vocamrwb_codec_mode_t)( mvs_new_props.awb_ul_mode - MVS_AMR_MODE_0660 );
          frame_info = ( void* )&amrwb_frame_info;
        }
        else
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvs_mod_send_silence_frame_proc(): " \
                 "Unsupprted media type (0x%08x)", media_id );
          rc = APR_EOK;
          break;
        }

        rc = voice_util_get_homing_frame( media_id, frame_info, packet, &size );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_mod_send_silence_frame_proc(): " \
                 "Failed to get silence frame, rc = (0x%08x)", rc );
          rc = APR_EOK;
          break;
        }

        rc = mvs_deliver_ul_packet( ( void* ) packet, frame_info );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_mod_send_silence_frame_proc(): " \
                 "Failed to deliver silence frame, rc = (0x%08x)", rc );
          rc = APR_EOK;
          break;
        }
      }

      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_mod_send_silence_frame_proc(): " \
           "Silence frame delivered" );

      break;
    }
  }
  else
  {
    MVS_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  ( void ) mvs_mem_free_cmd( ctrl->cmd );

  return rc;
}

/****************************************************************************
 * VS EVENT HANDLING ROUTINES                                               *
 ****************************************************************************/

static uint32_t mvs_process_read_available (
  void* context
)
{
  uint32_t rc;
  uint8 vocoder_packet[ VOICE_PACKET_LENGTH ];
  vs_voc_cmd_read_buffer_t read_buf_cmd;
  vs_voc_buffer_t* read_buf;
  mvs_vs_ctrl_type* vs_ctrl;

  vs_ctrl = ( mvs_vs_ctrl_type* )context;
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_read_available(): " \
         "VS handle = (0x%08x), cur client = (%d)", vs_ctrl->handle, mvs_cur_client );

  read_buf = vs_ctrl->read_buf;
  read_buf_cmd.handle = vs_ctrl->handle;
  read_buf_cmd.ret_buffer = &read_buf;

  rc = vs_call( VS_VOC_CMD_READ_BUFFER, ( void* )&read_buf_cmd,
                sizeof( read_buf_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available(): " \
           "Failed to read buffer, rc = (0x%08x)", rc );
  }
  else
  {
    if ( read_buf->size > VOICE_PACKET_LENGTH )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available():" \
             "VS buffer size (%d) is too big", read_buf->size );

      rc = mvs_prime_vs_read_buf( vs_ctrl->handle, read_buf );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available(): " \
               "Failed to prime read buffer, rc = (0x%08x)", rc );
      }

      return APR_EFAILED;
    }

    if ( vs_ctrl->is_vs_ready == TRUE )
    {
      mmstd_memcpy( vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                    VOICE_PACKET_LENGTH, read_buf->frame, read_buf->size );
      mvs_deliver_ul_packet( ( void* ) &vocoder_packet, read_buf->frame_info );
    }
  }
        
  rc = mvs_prime_vs_read_buf( vs_ctrl->handle, read_buf );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available(): " \
           "Failed to prime read buffer, rc = (0x%08x)", rc );
  }

  /* Get 2nd buffer for G711A and G729A */
  if ( ( mvs_client_table[mvs_cur_client].config_mode == MVS_MODE_G711A ) || 
       ( mvs_client_table[mvs_cur_client].config_mode == MVS_MODE_G729A )  )
  {
    read_buf = vs_ctrl->read_buf_2;
    read_buf_cmd.ret_buffer = &read_buf;
    rc = vs_call( VS_VOC_CMD_READ_BUFFER, ( void* )&read_buf_cmd,
                sizeof( read_buf_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available(): " \
           "Failed to read buffer, rc = (0x%08x)", rc );
    }
    else
    {
      if ( read_buf->size > VOICE_PACKET_LENGTH )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available():" \
               "VS buffer size (%d) is too big", read_buf->size );

        rc = mvs_prime_vs_read_buf( vs_ctrl->handle, read_buf );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available(): " \
               "Failed to prime read buffer, rc = (0x%08x)", rc );
        }

        return APR_EFAILED;
      }

      if ( vs_ctrl->is_vs_ready == TRUE )
      {
        mmstd_memcpy( vocoder_packet + VOICE_PACKET_HDR_LENGTH, 
                      VOICE_PACKET_LENGTH, read_buf->frame, read_buf->size );
        mvs_deliver_ul_packet( ( void* ) &vocoder_packet, read_buf->frame_info );
      }
    }
        
    rc = mvs_prime_vs_read_buf( vs_ctrl->handle, read_buf );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_read_available(): " \
             "Failed to prime read buffer, rc = (0x%08x)", rc );
    }
  }

  return rc;
}

static uint32_t mvs_process_write_request ( 
  void* context
)
{
  uint32_t rc = APR_EOK;
  uint8 vocoder_packet[ VOICE_PACKET_LENGTH ];
  mvs_vs_ctrl_type* vs_ctrl;

  vs_ctrl = ( mvs_vs_ctrl_type* )context;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_write_request(): " \
         "VS handle = (0x%08x)", vs_ctrl->handle );

  if ( vs_ctrl->is_vs_ready == TRUE )
  {
    rc = mvs_deliver_dl_packet( ( ( void* ) &vocoder_packet ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_write_request(): " \
             "Failed to deliver DL packet, rc = (0x%08x)", rc );
    }
  }

  return rc;
}

static uint32_t mvs_process_write_buf_returned ( 
  void* context	
)
{
  mvs_vs_ctrl_type* vs_ctrl;

  vs_ctrl = ( mvs_vs_ctrl_type* )context;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_write_buf_returned_evt(): " \
         "VS handle = (0x%08x)", vs_ctrl->handle );

  vs_ctrl->is_write_buf_returned = TRUE;
  return APR_EOK;
}

static uint32_t mvs_process_evt_ready ( 
  void* context	
)
{
  mvs_vs_ctrl_type* vs_ctrl;

  vs_ctrl = ( mvs_vs_ctrl_type* )context;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_evt_ready(): " \
         "VS handle = (0x%08x)", vs_ctrl->handle );

  vs_ctrl->is_vs_ready = TRUE;

  return APR_EOK;
}

static uint32_t mvs_process_evt_not_ready ( 
  void* context	
)
{
  mvs_vs_ctrl_type* vs_ctrl;

  vs_ctrl = ( mvs_vs_ctrl_type* )context;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_evt_not_ready(): " \
         "VS handle = (0x%08x)", vs_ctrl->handle );

  vs_ctrl->is_vs_ready = FALSE;

  return APR_EOK;
}

static uint32_t mvs_process_vs_eamr_mode_change ( 
  mvs_cmd_t* cmd
)
{
  mvs_vs_ctrl_type* vs_ctrl;
  mvs_eamr_sr_type sr = MVS_EAMR_SR_8000;
  bool_t is_sr_valid = TRUE;

  vs_ctrl = ( mvs_vs_ctrl_type* )cmd->vs_eamr_mode.context;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_vs_eamr_mode_change(): " \
         "VS handle = (0x%08x)", vs_ctrl->handle );

  if ( cmd->vs_eamr_mode.mode == VS_COMMON_EAMR_MODE_NARROWBAND )
  {
    sr = MVS_EAMR_SR_8000;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvs_process_vs_eamr_mode_change(): " \
           "eAMR mode changed, sampling rate = (%d)", 8000 );
  }
  else if ( cmd->vs_eamr_mode.mode == VS_COMMON_EAMR_MODE_WIDEBAND )
  {
    sr = MVS_EAMR_SR_16000;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvs_process_vs_eamr_mode_change(): " \
           "eAMR mode changed, sampling rate = (%d)", 16000 );
  }
  else
  {
    is_sr_valid = FALSE;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_process_vs_eamr_mode_change(): " \
           "Unsupported eAMR mode event, mode = (%d)",
           cmd->vs_eamr_mode.mode );
  }

  if ( ( mvs_cur_client != MVS_CLIENT_NONE )
        && ( mvs_client_table[mvs_cur_client].state == MVS_STATE_VOCODER )
        && ( is_sr_valid == TRUE ) )
  {
    ( void ) mvs_send_eamr_sr_event( mvs_cur_client, sr );
  }

  return APR_EOK;
}

static void mvs_process_vs_cmd_rsp (
  mvs_cmd_t* cmd
)
{
  int32_t rc;
  mvs_object_t* object;
  mvs_simple_job_object_t* job_obj;
  void* context;

  context = cmd->vs_cmd_rsp.context;
  if ( context == 0 )
  { /* Don't care the response, free it. */
    ( void ) mvs_mem_free_cmd( cmd );
    return;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_vs_cmd_rsp(): " \
         "cmd_id = (0x%08X), context = (0x%08X)", cmd->vs_cmd_rsp.cmd_id, context );

  rc = mvs_get_object( *( uint32_t*)context, &object );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_vs_cmd_rsp():" \
           "Invalid handle, context = (0x%08X)", context );
  }
  else
  {
    job_obj = ( mvs_simple_job_object_t* )object;
    job_obj->is_completed = TRUE;
    job_obj->status = cmd->vs_cmd_rsp.status_id;
  }

  rc = mvs_mem_free_cmd( cmd );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_vs_cmd_rsp(): " \
         "Failed to free command response" );
  }
}

static void mvs_process_vs_evt (
  mvs_cmd_t* cmd
)
{
  int32_t rc;
  uint32_t evt_id;

  evt_id = cmd->vs_evt.evt_id;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_process_vs_evt(): " \
         "evt_id = (0x%08X)", evt_id );

  switch ( evt_id )
  {
  case VS_VOC_EVENT_READ_AVAILABLE:
    rc = mvs_process_read_available( cmd->vs_evt.context );
    break;
  case VS_VOC_EVENT_WRITE_REQUEST:
    rc = mvs_process_write_request( cmd->vs_evt.context );
    break;
  case VS_VOC_EVENT_WRITE_BUFFER_RETURNED:
    rc = mvs_process_write_buf_returned( cmd->vs_evt.context );
    break;
  case VS_COMMON_EVENT_READY:
    rc = mvs_process_evt_ready( cmd->vs_evt.context );
    break;
  case VS_COMMON_EVENT_NOT_READY:
    rc = mvs_process_evt_not_ready( cmd->vs_evt.context );
    break;
  default:
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_vs_evt(): " \
           "Invalid VS event handle=(0x%08X), evt_id=(0x%08X)",
           cmd->header.handle, evt_id );
    rc = APR_EFAILED;    
  }

  rc = mvs_mem_free_cmd( cmd );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_process_vs_evt(): " \
         "Failed to free event" );
  }
}

/* Process high priority events, responses, and non-gating commands in task context. */
static void mvs_task_process_events_and_nongating_commands ( void )
{
  mvs_work_item_t* work_item;
  mvs_cmd_t* cmd;
  bool_t queue_to_pending_flag;

  while ( apr_list_remove_head( &mvs_incoming_cmd_q,
                                ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    cmd = ( ( mvs_cmd_t* ) work_item->cmd );
    ( void ) apr_list_add_tail( &mvs_free_cmd_q, &work_item->link );
    queue_to_pending_flag = FALSE;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "Processing the non-gating queue - " \
          "handle=(0x%08X), type=(0x%08X)",
          cmd->header.handle, cmd->header.type );

    switch ( cmd->header.type )
    {
    case MVS_CMD_TYPE_ENUM_VS_CMD_RSP:
      mvs_process_vs_cmd_rsp( cmd );
      break;
    case MVS_CMD_TYPE_ENUM_VS_EVT:
      mvs_process_vs_evt( cmd );
      break;
    case MVS_CMD_TYPE_ENUM_VS_EAMR_MODE:
      mvs_process_vs_eamr_mode_change( cmd );
      break;
    case MVS_CMD_TYPE_ENUM_INTERNAL_CMD:
      queue_to_pending_flag = TRUE;
      break;

    default:
      queue_to_pending_flag = TRUE;
      break;
    }

    if ( queue_to_pending_flag == TRUE )
    {
      mvs_queue_pending_cmd( cmd );
    }
  }
}

/* Process normal priority sequentially executed pending commands. */
static void mvs_task_process_pending_commands ( void )
{
  int32_t rc;
  mvs_work_item_t* work_item;
  mvs_cmd_t* cmd;
  bool_t is_unsupported;

  for ( ;; )
  {
    switch ( mvs_pending_ctrl.state )
    {
    case MVS_PENDING_CMD_STATE_ENUM_FETCH:
      {
        { /* Fetch the next pending command to execute. */
          rc = apr_list_remove_head( &mvs_pending_cmd_q,
                                     ( ( apr_list_node_t** ) &work_item ) );
          if ( rc )
          { /* Return when the pending command queue is empty. */
            return;
          }
          mvs_pending_ctrl.cmd = work_item->cmd;
          ( void ) apr_list_add_tail( &mvs_free_cmd_q, &work_item->link );

          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "Fetched a gating command - " \
                "handle=(0x%08X), type=(0x%08X)",
                mvs_pending_ctrl.cmd->header.handle,
                mvs_pending_ctrl.cmd->header.type );
        }
        mvs_pending_ctrl.state = MVS_PENDING_CMD_STATE_ENUM_EXECUTE;
      }
      break;

    case MVS_PENDING_CMD_STATE_ENUM_EXECUTE:
    case MVS_PENDING_CMD_STATE_ENUM_CONTINUE:
      {
        is_unsupported = FALSE;

        cmd = mvs_pending_ctrl.cmd;

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "Running the gating command - " \
              "handle=(0x%08X), type=(0x%08X)",
              cmd->header.handle, cmd->header.type );

        switch ( cmd->internal_cmd.type.hdr.cmd )
        {
        case MVS_CMD_ACQUIRE:
          rc = mvs_mod_acquire_proc( &mvs_pending_ctrl );
          break;

        case MVS_CMD_ENABLE:
          rc = mvs_mod_enable_proc( &mvs_pending_ctrl );
          break;

        case MVS_CMD_STANDBY:
          rc = mvs_mod_standby_proc( &mvs_pending_ctrl );
          break;

        case MVS_CMD_RELEASE:
          rc = mvs_mod_release_proc( &mvs_pending_ctrl );
          break;

        case MVS_CMD_SEND_SILENCE_FRAME:
           rc = mvs_mod_send_silence_frame_proc( &mvs_pending_ctrl );
           break;

        default:
          is_unsupported = TRUE;
          break;
        }
      }

      if ( is_unsupported == TRUE )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "Received an unsupported command - " \
               "handle=(0x%08X), cmd=(0x%08X)",
              cmd->header.handle, cmd->header.type );
        ( void ) mvs_mem_free_cmd( mvs_pending_ctrl.cmd );

         /* Go process the next command. */
         rc = APR_EOK;
      }

      if ( rc != APR_EPENDING )
      { /* The current command finished so fetch the next command. */
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "Finished the gating command - " \
                 "handle=(0x%08X), type=(0x%08X), rc=(0x%08X)",
                 cmd->header.handle, cmd->header.type, rc );
          mvs_pending_ctrl.state = MVS_PENDING_CMD_STATE_ENUM_FETCH;
      }
      else
      { /* Assuming the current pending command control routine returns
         * APR_EPENDING the overall progress stalls until one or more
         * external events or responses are received.
         */
        mvs_pending_ctrl.state = MVS_PENDING_CMD_STATE_ENUM_CONTINUE;
        return;
      }
      break;

    default:
      /* Assert when the pending state doesn't exist. */
      return;
    }
  }
}

/****************************************************************************
 * PENDING COMMAND ROUTINES                                                 *
 ****************************************************************************/

static int32_t mvs_pending_control_init (
  mvs_pending_control_t* ctrl
)
{
  if ( ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  ctrl->state = MVS_PENDING_CMD_STATE_ENUM_FETCH;
  ctrl->cmd = NULL;
  ctrl->rootjob_obj = NULL;

  return APR_EOK;
}

static int32_t mvs_pending_control_destroy (
  mvs_pending_control_t* ctrl
)
{
  if ( ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  return APR_EOK;
}

/****************************************************************************
 * CODE PROFILING ROUTINES                                                  *
 ****************************************************************************/

#ifdef MVS_PROFILING_ENABLED
APR_INTERNAL uint32_t mvs_profiling_enable_tap_point (
  mvs_profiling_tap_point_t tap_point
)
{
  uint32_t rc;

  if ( ( tap_point > MVS_PROFILING_TAP_POINT_INVALID ) &&
       ( tap_point < MVS_PROFILING_TAP_POINT_MAX_ENTER ) )
  { /* What exposed to QXDM command is the start tap point.
       And we always enable tap points in pair.*/
    mvs_profiling_tap_point_enable_table[ tap_point ] = TRUE; /* Start tap point. */
    mvs_profiling_tap_point_enable_table[ tap_point + 1 ] = TRUE; /* Leave tap point. */

    rc = APR_EOK;
  }
  else
  {
    rc = APR_EBADPARAM;
  }

  return rc;
}

APR_INTERNAL uint32_t mvs_profiling_disable_tap_point (
  mvs_profiling_tap_point_t tap_point
)
{
  uint32_t rc;

  if ( ( tap_point > MVS_PROFILING_TAP_POINT_INVALID ) &&
       ( tap_point < MVS_PROFILING_TAP_POINT_MAX_ENTER ) )
  { /* What exposed to QXDM command is the start tap point.
       And we always disable tap points in pair.*/
    mvs_profiling_tap_point_enable_table[ tap_point ] = FALSE;
    mvs_profiling_tap_point_enable_table[ tap_point + 1 ] = FALSE;

    rc = APR_EOK;
  }
  else
  {
    rc = APR_EBADPARAM;
  }

  return rc;
}  
#endif /* MVS_PROFILING_ENABLED */

APR_INTERNAL void mvs_profiling_save_entry ( 
  mvs_profiling_tap_point_t tap_point
)
{
#ifdef MVS_PROFILING_ENABLED
  mvs_profiling_buf_entry* entry;
  npa_query_status status;
  npa_query_type result;

  if ( mvs_profiling_tap_point_enable_table[ tap_point ] == FALSE )
  {
    return;
  }
  else
  {
    ( void ) apr_lock_enter( mvs_profiling_lock );
    mvs_profiling_buf_index = ( ( mvs_profiling_buf_index + 1 ) % MVS_PROFILING_BUF_NUM_ENTRIES );
    ( void ) apr_lock_leave( mvs_profiling_lock );
	
    entry = &mvs_profiling_buffer[ mvs_profiling_buf_index ];

    if ( tap_point % 2 /* It is a start tap point, do self measurement. */ )
    {   
      /* Get pcycle measurement. */
      entry->t0_ref = blast_get_core_pcycles( );
      entry->t1_pcycle = blast_get_core_pcycles( );

      /* Get clock measurement. */
      status = npa_query( mvs_npa_query_handle, NPA_QUERY_CURRENT_STATE, &result );
      if ( status == NPA_QUERY_SUCCESS )
      {
        mvs_profiling_cur_clk = result.data.state;
      }
      else
      {
        mvs_profiling_cur_clk = -1;
      }
      entry->t2_cpu_clk = blast_get_core_pcycles( );

      /* Get thread ID measurement. */
      blast_thread_myid( );
      entry->t3_thread_id = blast_get_core_pcycles( );

    }
    else
    {
       /* Do not reset the self measurement values, in the case
          of leave tap point, in order to reduce profiling overhead. 
          We don't care about the values anyways. */
    }

    entry->pcycle = blast_get_core_pcycles( );
    entry->thread_id = blast_thread_myid( );
    entry->tap_point = tap_point;
    entry->cpu_clk = mvs_profiling_cur_clk;

    return;
  }
#endif /* MVS_PROFILING_ENABLED */
}

#ifdef MVS_PROFILING_ENABLED
APR_INTERNAL void mvs_profiling_clear_buffer (
  void
)
{
  mmstd_memset( mvs_profiling_buffer, 0, sizeof( mvs_profiling_buffer ) );
  mvs_profiling_buf_index = -1;

  return;
}

APR_INTERNAL uint32_t mvs_profiling_save_buffer (
  void
)
{
  uint32_t rc;
  vocsvc_nv_save_mvs_profiling_buffer_t params;

  params.buffer = ( uint8_t* ) mvs_profiling_buffer;
  params.size = sizeof( mvs_profiling_buffer );

  rc = vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_SAVE_MVS_PROFILING_BUFFER, 
                       &params, sizeof( params ) );

  return rc;
}
#endif /* MVS_PROFILING_ENABLED */

/****************************************************************************
 * IST ROUTINES                                                             *
 ****************************************************************************/

static int32_t mvs_mod_run ( void )
{
  mvs_task_process_events_and_nongating_commands( );
  mvs_task_process_pending_commands( );

  return APR_EOK;
}

static int32_t mvs_ist ( void* param )
{
  int32_t rc;

  rc = apr_event_create( &mvs_ist_event );
  MVS_PANIC_ON_ERROR( rc );

  mvs_ist_state = MVS_THREAD_STATE_ENUM_READY;

  do
  {
    rc = apr_event_wait( mvs_ist_event );
#ifndef USE_SINGLE_AUDSVC_THREAD
   ( void ) mvs_mod_run( );
#endif /* !USE_SINGLE_AUDSVC_THREAD */
  }
  while ( rc == APR_EOK );

  rc = apr_event_destroy( mvs_ist_event );
  MVS_PANIC_ON_ERROR( rc );

  mvs_ist_state = MVS_THREAD_STATE_ENUM_EXIT;

  return APR_EOK;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static int32_t mvs_mod_init ( void )
{
  int32_t rc;
  uint32_t i;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_init() Date %s Time %s",
        __DATE__, __TIME__ );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &mvs_int_lock );
#ifdef MVS_PROFILING_ENABLED
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &mvs_profiling_lock );
#endif /* MVS_PROFILING_ENABLED */
  }
  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &mvs_heapmgr, ( ( void* ) &mvs_heap_pool ), sizeof( mvs_heap_pool ), NULL, NULL );
  }
  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;

    params.table = mvs_object_table;
    params.total_bits = MVS_HANDLE_TOTAL_BITS;
    params.index_bits = MVS_HANDLE_INDEX_BITS;
    params.lock_fn = mvs_int_lock_fn;
    params.unlock_fn = mvs_int_unlock_fn;
    rc = apr_objmgr_construct( &mvs_objmgr, &params );
    /* TODO: When the ISR and task context contentions becomes a problem
     *       the objmgr could be split into ISR/task and task-only stores
     *       to reduce the bottleneck.
     */
  }
  { /* Initialize the session management. */
    rc = mvs_pending_control_init( &mvs_pending_ctrl );
  }
  { /* Populate free command structures */
    rc = apr_list_init_v2( &mvs_free_cmd_q, mvs_int_lock_fn, mvs_int_unlock_fn );
    for ( i = 0; i < MVS_NUM_COMMANDS; ++i )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &mvs_cmd_pool[ i ] );
      ( void ) apr_list_add_tail( &mvs_free_cmd_q, ( ( apr_list_node_t* ) &mvs_cmd_pool[ i ] ) );
    }
  }
  {
    rc = apr_list_init_v2( &mvs_incoming_cmd_q, mvs_int_lock_fn, mvs_int_unlock_fn );
    rc = apr_list_init_v2( &mvs_pending_cmd_q, mvs_int_lock_fn, mvs_int_unlock_fn );
  }
#ifndef WINSIM
  {
    mvs_dsm_amr_init();
    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_init(): DSM init done" );
  }
#endif /* !WINSIM */
  {
    mvs_state_control_init();

    mvs_current_g711_mode = MVS_G711_MODE_MULAW; //default to mulaw
  }
  {
#ifndef WINSIM
    /* Get eAMR config from vocsvc_nv service. */
    rc = vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_GET_EAMR_CONFIG, 
                         &mvs_eamr_config, sizeof( mvs_eamr_config ) );
#else
    rc = APR_EFAILED;
#endif /* !WINSIM */
    if ( rc )
    { /* Not able to get eAMR config from vocsvc_nv, e.g. NV item file doesn't
       * exist or there are file read errors. eAMR will be disabled.
       */
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,  "mvs_mod_init(): Not able to get " \
          "eAMR config from voice NV,"
          "disabling eAMR by default." );
      mvs_eamr_config.enable_2g = FALSE;
      mvs_eamr_config.enable_3g = FALSE;
      mvs_eamr_config.enable_tdscdma = FALSE;
    }
  }
  { /* Create the MVS task. */
    rc = apr_thread_create( &mvs_ist_handle, MVS_TASK_NAME, MVS_TASK_PRIORITY,
                            mvs_task_stack, MVS_TASK_STACK_SIZE, mvs_ist, NULL );
    MVS_PANIC_ON_ERROR( rc );

    while ( mvs_ist_state != MVS_THREAD_STATE_ENUM_READY )
    {
      rc = apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }
  }

#ifdef USE_MVS_PACKET_TIMING_ANALYSIS
  /* Intialize timing struct. */
  memset( &mvs_timing_info, 0, sizeof( mvs_timing_info ) );
#endif /* USE_MVS_PACKET_TIMING_ANALYSIS */

#ifdef MVS_PROFILING_ENABLED
  /* Register to NPA. */
  mvs_npa_query_handle = npa_create_query_handle( "/clk/cpu" );
  if ( mvs_npa_query_handle == NULL )
  {
    MSG_ERROR( "Unable to create npa query handle", 0, 0, 0 );
  }
#endif /* MVS_PROFILING_ENABLED */

  mvs_is_initialized = TRUE;
  mvs_cur_client = MVS_CLIENT_NONE;

  return APR_EOK;
}

static uint32_t mvs_prime_vs_read_buf(
  uint32_t handle,
  vs_voc_buffer_t* buffer
)
{
  int32_t rc;
  vs_voc_cmd_prime_read_buffer_t prime_buf_cmd;

  if ( buffer == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_prime_vs_read_buf(): " \
         "Invalid paramter" );    
    return APR_EBADPARAM;
  }

  prime_buf_cmd.handle = handle;
  prime_buf_cmd.buffer = buffer;

  rc = vs_call( VS_VOC_CMD_PRIME_READ_BUFFER, ( void* )&prime_buf_cmd,
                sizeof( prime_buf_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_prime_vs_read_buf(): " \
           "Failed to prime read buffer, rc = (0x%08x)", rc );
  }

  return rc;
}

static uint32_t mvs_write_vs_buffer_through (
  void* frame_info,
  uint8_t* vocoder,
  uint32_t vocoder_size
)
{
  uint32_t rc;
  vs_voc_cmd_write_buffer_t write_buf_cmd;
  vs_voc_buffer_t* write_buf;

  if ( mvs_client_table[mvs_cur_client].vs_ctrl.is_write_buf_returned == TRUE )
  {
    write_buf = mvs_client_table[mvs_cur_client].vs_ctrl.write_buf;

    if ( write_buf == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_write_vs_buffer_through(): " \
           "Invalid VS write buffer" );  
      return APR_EFAILED;    
    }

    write_buf->frame_info = frame_info; 
    write_buf->media_id = mvs_map_vs_media_id( mvs_client_table[mvs_cur_client].config_mode );
    write_buf->size = vocoder_size;
    mmstd_memcpy( write_buf->frame, VOICE_PACKET_LENGTH, vocoder, vocoder_size );

    write_buf_cmd.handle = mvs_client_table[mvs_cur_client].vs_ctrl.handle;
    write_buf_cmd.buffer = write_buf;
    rc = vs_call( VS_VOC_CMD_WRITE_BUFFER_THROUGH, ( void* )&write_buf_cmd,
                  sizeof( write_buf_cmd ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_write_vs_buffer_through(): " \
             "Failed to write VS buffer,  rc = (%d)", rc );  
      return rc;
    }

  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_write_vs_buffer_through(): " \
         "VS write buffer is not returned, skip writing" );  
    rc = APR_EFAILED;
  }

  return rc;
}

static uint32 mvs_vs_event_cb (
  uint32_t event_id,
  void* params,
  uint32_t size,
  void* session_context
)
{
  int32_t rc;
  mvs_cmd_t* cmd;

  switch ( event_id )
  {
    case VS_COMMON_EVENT_CMD_RESPONSE:
    {
      vs_common_event_cmd_response_t* cmd_response;
      cmd_response = ( vs_common_event_cmd_response_t* )params;

      if ( cmd_response->cmd_id == VS_VOC_CMD_CLOSE )
      {
        return APR_EOK;
      }

      rc = mvs_make_vs_cmd_rsp( cmd_response, &cmd );
      if ( rc )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_vs_event_cb(): " \
             "Make VS command response failed" );
        return rc;
      }

      ( void ) mvs_queue_incoming_cmd( cmd );

      break;
    }

    case VS_COMMOM_EVENT_EAMR_MODE_CHANGE:
    {
      vs_common_event_eamr_mode_t* eamr_mode;
      eamr_mode = ( vs_common_event_eamr_mode_t* )params;

      rc = mvs_make_vs_eamr_mode( eamr_mode, session_context, &cmd );
      if ( rc )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_vs_event_cb(): " \
             "Make VS eAMR mode change event failed" );
        return rc;
      }

      ( void ) mvs_queue_incoming_cmd( cmd );

      break;
    }

    case VS_COMMON_EVENT_READY:
    case VS_COMMON_EVENT_NOT_READY:
    case VS_VOC_EVENT_READ_AVAILABLE:
    case VS_VOC_EVENT_WRITE_REQUEST:
    case VS_VOC_EVENT_WRITE_BUFFER_RETURNED:
    {
      rc = mvs_make_vs_evt( event_id, session_context, &cmd );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_vs_event_cb(): " \
               "Make VS event failed, rc = (0x%08x)", rc );
        return rc;
      }

      ( void ) mvs_queue_incoming_cmd( cmd );

      break;
    }

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_vs_event_cb(): " \
           "Unsupported event (%d)", event_id );
      rc = VS_EFAILED;
      break;
  }

  return rc;
}

static uint32_t mvs_free_vs_buffer (
  uint32_t handle,
  vs_voc_buffer_t* buf
)
{
  uint32 rc;
  vs_voc_cmd_free_buffer_t free_buf_cmd;

  free_buf_cmd.handle = handle;
  free_buf_cmd.buffer = buf;

  rc = vs_call( VS_VOC_CMD_FREE_BUFFER, ( void* )&free_buf_cmd,
                sizeof( free_buf_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_free_vs_buffer(): " \
           "Failed to free VS buffer, " \
           "handle = (0x%08x), rc = (0x%08x)", handle, rc );
  }

  return rc;
}

static uint32_t mvs_close_vs_session
(
  mvs_vs_ctrl_type* vs_ctrl,
  bool_t is_buf_allocated
)
{
  uint32 rc;
  vs_voc_cmd_close_t close_cmd;

  /* Free read and write buffers. */
  if ( is_buf_allocated == TRUE )
  {
    ( void ) mvs_free_vs_buffer ( vs_ctrl->handle, vs_ctrl->read_buf );
    ( void ) mvs_free_vs_buffer ( vs_ctrl->handle, vs_ctrl->write_buf );

    if ( vs_ctrl->read_buf_2 != NULL )
    {
      ( void ) mvs_free_vs_buffer ( vs_ctrl->handle, vs_ctrl->read_buf_2 );
    }
  }

  close_cmd.handle = vs_ctrl->handle;
  close_cmd.client_context = 0;/* Don't care the response. */

  rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&close_cmd,
                sizeof( close_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_close_vs_session(): " \
           "Failed to close VS session, " \
           "handle = (0x%08x), rc = (0x%08x)", vs_ctrl->handle, rc );
  }

  return rc;
}

static uint32_t mvs_open_vs_session (
  mvs_vs_ctrl_type* vs_ctrl,
  uint32_t vs_client_id
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;
  vs_voc_cmd_alloc_buffer_t alloc_cmd;
  uint32_t checkpoint = 0;

  for ( ;; )
  {
    /* Open VS session. */
    vs_ctrl->client_id = vs_client_id;
    vs_ctrl->is_vs_ready = FALSE;
    vs_ctrl->is_write_buf_returned = TRUE;

    open_cmd.ret_handle = &vs_ctrl->handle;
    open_cmd.vsid = MVS_VSID_CS_VOICE_MULTIMODE;
    open_cmd.client_id = vs_client_id;
    open_cmd.session_context = ( void* )vs_ctrl;
    open_cmd.event_cb = mvs_vs_event_cb;

    rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_open_vs_session(): " \
             "Failed to open VS session, " \
             "client = (0x%08x), rc = (0x%08x)", open_cmd.client_id, rc );
      break;
    }
    checkpoint = 1;

    /* Allocate read buffer. */
    alloc_cmd.handle = vs_ctrl->handle;
    alloc_cmd.ret_buffer = &vs_ctrl->read_buf;
    alloc_cmd.req_max_frame_size = VOICE_PACKET_LENGTH; 
    rc = vs_call( VS_VOC_CMD_ALLOC_BUFFER, ( void* )&alloc_cmd,
                  sizeof( alloc_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_open_vs_session(): " \
             "Failed to allocate read buffer, " \
             "client = (0x%08x), rc = (0x%08x)", open_cmd.client_id, rc );
      break;
    }
    checkpoint = 2;

    /* Allocate write buffer. */
    alloc_cmd.ret_buffer = &vs_ctrl->write_buf;
    rc = vs_call( VS_VOC_CMD_ALLOC_BUFFER, ( void* )&alloc_cmd,
                  sizeof( alloc_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_open_vs_session(): " \
             "Failed to allocate write buffer, " \
             "client = (0x%08x), rc = (0x%08x)", open_cmd.client_id, rc );
      break;
    }
    checkpoint = 3;

    /* Allocate 2nd read buffer. */
    if ( vs_client_id == VS_VOC_CLIENT_TEST )
    {
      alloc_cmd.ret_buffer = &vs_ctrl->read_buf_2;
      rc = vs_call( VS_VOC_CMD_ALLOC_BUFFER, ( void* )&alloc_cmd,
                    sizeof( alloc_cmd ) );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_open_vs_session(): " \
               "Failed to allocate 2nd read buffer, " \
               "client = (0x%08x), rc = (0x%08x)", open_cmd.client_id, rc );
        vs_ctrl->read_buf_2 = NULL; 
      }
    }

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 2:
    ( void ) mvs_free_vs_buffer( vs_ctrl->handle, vs_ctrl->read_buf );
    /*fallthru */
  case 1:
    ( void ) mvs_close_vs_session( vs_ctrl, FALSE );
    /*fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

static bool_t mvs_is_client_supported_by_vs (
  mvs_client_type client_type
)
{
  switch ( client_type )
  {
  case MVS_CLIENT_CDMA:
  case MVS_CLIENT_WCDMA:
  case MVS_CLIENT_GSM:
  case MVS_CLIENT_TDSCDMA:
  case MVS_CLIENT_TEST:
    return TRUE;
  default:
    return FALSE;
  }
}

static int32_t mvs_init_vs( void )
{
  uint32_t idx;

  /* Open VS session for WCMDA client. */
  ( void ) mvs_open_vs_session( &mvs_client_table[MVS_CLIENT_WCDMA].vs_ctrl,
                                VS_VOC_CLIENT_WCDMA );

  /* Open VS session for GSM client. */
  ( void ) mvs_open_vs_session( &mvs_client_table[MVS_CLIENT_GSM].vs_ctrl,
                                VS_VOC_CLIENT_MVS_GSM );

  /* Open VS session for TDSCMDA client. */
  ( void ) mvs_open_vs_session( &mvs_client_table[MVS_CLIENT_TDSCDMA].vs_ctrl,
                                VS_VOC_CLIENT_TDSCDMA );

  /* Open VS session for TEST client. */
  ( void ) mvs_open_vs_session( &mvs_client_table[MVS_CLIENT_TEST].vs_ctrl,
                                VS_VOC_CLIENT_TEST );

  /* Other MVS clients share the VS session for TEST client. */
  for ( idx = 0; idx < MVS_CLIENT_MAX; ++idx )
  {
    if ( mvs_is_client_supported_by_vs( idx ) == FALSE )
    {
      mvs_client_table[idx].vs_ctrl = mvs_client_table[MVS_CLIENT_TEST].vs_ctrl;
    }
  }

  return APR_EOK;
}

static int32_t mvs_deinit_vs( void )
{
  /* Close VS session for TEST client. */
  ( void ) mvs_close_vs_session ( &mvs_client_table[MVS_CLIENT_TEST].vs_ctrl, TRUE );

  /* Close VS session for TDSCMDA client. */
  ( void ) mvs_close_vs_session ( &mvs_client_table[MVS_CLIENT_TDSCDMA].vs_ctrl, TRUE );

  /* Close VS session for GSM client. */
  ( void ) mvs_close_vs_session ( &mvs_client_table[MVS_CLIENT_GSM].vs_ctrl, TRUE );

  /* Close VS session for WCMDA client. */
  ( void ) mvs_close_vs_session ( &mvs_client_table[MVS_CLIENT_WCDMA].vs_ctrl, TRUE );

  
  return APR_EOK;
}

static int32_t mvs_mod_post_init( void )
{
  ( void ) mvs_init_vs( );

  return APR_EOK;
}

static int32_t mvs_mod_pre_deinit( void )
{
  ( void ) mvs_deinit_vs( );

  return APR_EOK;
}

static int32_t mvs_mod_deinit ( void )
{
  int32_t rc;

  /* TODO: Ensure no race conditions on deregister. */

  { /* Destroy the MVS IST. */
    rc = apr_event_signal_abortall( mvs_ist_event );

    while ( mvs_ist_state != MVS_THREAD_STATE_ENUM_EXIT )
    {
      rc = apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }

    rc = apr_thread_destroy( mvs_ist_handle );
  }
  { /* Release the command queue management. */
    rc = apr_list_destroy( &mvs_pending_cmd_q );
    rc = apr_list_destroy( &mvs_incoming_cmd_q );
    rc = apr_list_destroy( &mvs_free_cmd_q );
  }
  { /* Release the session management. */
    rc = mvs_pending_control_destroy( &mvs_pending_ctrl );
  }
  { /* Release the object management. */
    rc = apr_objmgr_destruct( &mvs_objmgr );
  }
  { /* Release the locks. */
#ifdef MVS_PROFILING_ENABLED
    rc = apr_lock_destroy( mvs_profiling_lock );
#endif /* MVS_PROFILING_ENABLED */
    rc = apr_lock_destroy( mvs_int_lock );
  }
#ifdef MVS_PROFILING_ENABLED
  /* Destroy the npa query handle */
  if ( mvs_npa_query_handle != NULL )
  {
    npa_destroy_query_handle( mvs_npa_query_handle );
  }
#endif /* MVS_PROFILING_ENABLED */

  { /* Print out debug info. on object sizes. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "apr_objmgr_object_t size = (%d)",
          sizeof( apr_objmgr_object_t ) );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_object_t size = (%d)",
          sizeof( mvs_object_t ) );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_mod_deinit rc = (0x%08x)", rc );
  }

  return APR_EOK;
}

/* TBD: may need VS internal API */
mvs_voc_rate_type mvs_mod_get_voc_max_rate ( void )
{
  return mvs_curr_props.max_voc_rate;
}

/* TBD: may need VS internal API */
mvs_voc_rate_type mvs_mod_get_voc_min_rate ( void )
{
  return mvs_curr_props.min_voc_rate;
}

APR_EXTERNAL int32_t mvs_call (
  mvs_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
  case MVS_CALLINDEX_ENUM_INIT:
    rc = mvs_mod_init( );
    break;

  case MVS_CALLINDEX_ENUM_POSTINIT:
    rc = mvs_mod_post_init( );
    break;

  case MVS_CALLINDEX_ENUM_PREDEINIT:
    rc = mvs_mod_pre_deinit( );
    break;

  case MVS_CALLINDEX_ENUM_DEINIT:
    rc = mvs_mod_deinit( );
    break;

  case MVS_CALLINDEX_ENUM_RUN:
    rc = mvs_mod_run( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_call(): " \
           "Unsupported callindex (%d)", index );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

