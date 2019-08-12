#ifndef __CVA_I_H__
#define __CVA_I_H__

/**
  @file  cva_i.h
  @brief This file contains internal definitions of VS
*/

/*
  ============================================================================

   Copyright (C) 2015 QUALCOMM Technologies Incorporated.

   All Rights Reserved.
   QUALCOMM Proprietary and Confidential.

  ============================================================================

                             Edit History

  $Header: cva_i.h
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------

  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

#include "sys.h"
#include "mmdefs.h"
#include "apr_list.h"
#include "vs.h"
#include "cva_icommon_if.h"
/****************************************************************************
  CVA DEFINES
****************************************************************************/

#define CVA_HEAP_SIZE_V ( 3000 )

/* Max packet size for 1X vocoders. */
#define CVA_MAX_PACKET_SIZE ( 40 )

/* Size of VS work packet queue. */
#define CVA_NUM_WORK_PKTS_V ( 40 )

#define CVA_HANDLE_TOTAL_BITS_V ( 16 )
#define CVA_HANDLE_INDEX_BITS_V ( 7 ) /**< 7 bits = 128 handles. */
#define CVA_MAX_OBJECTS_V ( 1 << CVA_HANDLE_INDEX_BITS_V )

#define CVA_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], cva_state=%d", \
                            rc, cva_is_initialized, 0 ); } }

#define CVA_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x], "\
                       "cva_state=%d", rc, cva_is_initialized ); } }

#define CVA_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define CVA_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* Definitions for error checking */
#define CVA_VOCODER_ID_UNDEFINED_V (0xFFFFFFFF)
#define CVA_VSID_UNDEFINED_V (0xFFFFFFFF)
#define CVA_MIN_MAX_RATE_UNDEFINED ( 0xFFFFFFFF )
#define CVA_AVG_ENC_RATE_UNDEFINED ( 0xFF )
#define CVA_FRAME_RATE_UNDEFINED ( 0xFF )

/* Internal events.*/
#define CVA_IVOICE_EVENTI_SUBS_PREF_INFO ( 0x0001318A )

typedef struct cva_eventi_subs_pref_info cva_eventi_subs_pref_info;

struct cva_eventi_subs_pref_info {

  bool_t is_sub_active;

  uint32_t vsid;
};

#define CVA_IVOICE_EVENTI_VS_OPEN ( 0x000131A5 )

#define CVA_IVOICE_EVENTI_VS_CLOSE ( 0x000131A6 )


/****************************************************************************
  CVA thread states
****************************************************************************/

typedef enum cva_thread_state_enum_t
{
  CVA_THREAD_STATE_ENUM_INIT,
  CVA_THREAD_STATE_ENUM_READY,
  CVA_THREAD_STATE_ENUM_EXIT
}
  cva_thread_state_enum_t;

/***************************************************************************
  CVA WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/

typedef struct cva_cmd_packet_t {
  
  uint32_t cmd_id;
    /**< Command id issued from client. */
  void* params;
    /**< Structure associated to each cmd_id. */
} cva_cmd_packet_t;

typedef struct cva_event_packet_t {
  
  void* session_context;
    /**< Reference to vocie adapter session object. */
  uint32_t event_id;
    /**< Event id issued from client. */
  void* params;
    /**< Structure associated to each event_id. */
} cva_event_packet_t;

typedef enum cva_work_item_packet_type_t
{
  CVA_WORK_ITEM_PKT_TYPE_NONE,
  CVA_WORK_ITEM_PKT_TYPE_CMD,
  CVA_WORK_ITEM_PKT_TYPE_EVENT

} cva_work_item_packet_type_t;

typedef enum cva_work_item_queue_type_t
{
  CVA_WORK_ITEM_QUEUE_TYPE_NONE,
  CVA_WORK_ITEM_QUEUE_TYPE_NONGATING,
  CVA_WORK_ITEM_QUEUE_TYPE_GATING,

} cva_work_item_queue_type_t;

typedef struct cva_work_item_t {
  
  apr_list_node_t link;
  
  cva_work_item_packet_type_t pkt_type;
  
  void* packet;
   /**<
     * This is generic work packet, based on the pkt_type it can
     * to cva_cmd_packet_t or cva_event_packet_t.
     */
} cva_work_item_t;


/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/

typedef enum cva_object_type_enum_t
{
   CVA_OBJECT_TYPE_ENUM_UNINITIALIZED,
   CVA_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   CVA_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
   CVA_OBJECT_TYPE_ENUM_SESSION,
   CVA_OBJECT_TYPE_ENUM_INVALID

} cva_object_type_enum_t;

typedef struct cva_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  cva_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */
} cva_object_header_t;


/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/

typedef struct cva_simple_job_object_t cva_simple_job_object_t;

struct cva_simple_job_object_t {

  cva_object_header_t header;

  uint32_t context_handle;
   /**<
    * The parent-job handle indicates this sub-job is part of a batch-job.
    *
    * Set this value to -1 when it is unused or when there is no parent.
    */
  bool_t is_completed;
   /**< The command completed response flag. 0 is false and 1 is true. */
  uint32_t status;
   /**< The status returned by the command completion. */
};


/****************************************************************************
  THE SEQUENCER JOB OBJECT
****************************************************************************/

typedef struct cva_sequencer_job_object_t cva_sequencer_job_object_t;

struct cva_sequencer_job_object_t {

  cva_object_header_t header;

  uint32_t state;
   /**< The generic state variable. */
  union cva_object_t* subjob_obj;
   /**< The current sub-job object. */
  uint32_t status;
   /**< A status value. */
};


/****************************************************************************
  THE SESSION OBJECT
****************************************************************************/

typedef struct cva_session_object_t cva_session_object_t;

struct cva_session_object_t {

  cva_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */

  uint32_t vsid;
    /**< System level published/documented Voice System ID. */

  cva_icommon_event_callback_fn_t va_cva_event_cb;
    /**<
      * Callback function registered by voice agent to recieve command
      * responses and events.
      */
  void* va_session_context;
    /**< Session context provided during CVA_IRESOURCE_CMD_OPEN. */
  
  bool_t is_resources_granted;
    /**< Indicates that vocoder resource access is granted. */

  /**<
    * Reference to GSM voice instance.
    * Applicable if adapter is configured with asid to vsid mapping.
    */
  uint32_t onex_handle;
    /**< Return handle for GSM voice instance. */

  uint32_t vocoder_type;
    /**< Indicates the active vocoder type for the call. */

  uint32_t avg_enc_rate;
    /**< Indicates the enc rate requested for the call. */

  uint32_t min_rate;

  uint32_t max_rate;

  bool_t dtx_mode;
    /**< Indicates the active vocoder type for the call. */

  bool_t is_onex_ready;
    /**< Indicates the readiness of GSM protocol software for voice 
      * traffic exchange.
      */

  /**<
    * Reference to VS session control.
    * Applicable if adapter is configured with asid to vsid mapping.
    */
  uint32_t vs_handle;
    /**< Return handle for vocoder session. */

  vs_voc_buffer_t* vs_read_buf;

  vs_voc_buffer_t* primed_read_buf;
    /**< TODO: Requires cleanup once the VS behaviour for FLUSH is corrected.
               Added to avoding memory leak during VS_VOC_CMD_CLOSE.*/

  vs_voc_buffer_t* vs_write_buf;

  bool_t is_vs_ready;
    /**< Inidcates vocoder session is running. */
};

/****************************************************************************
  THE GENERIC CVA OBJECT
****************************************************************************/

typedef union cva_object_t {
  
  cva_object_header_t header;
  cva_simple_job_object_t simple_job;
  cva_sequencer_job_object_t sequencer_job;
  cva_session_object_t session_obj;

} cva_object_t;


/****************************************************************************
 * PENDING MVS COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum cva_gating_cmd_state_t
{
  CVA_GATING_CMD_STATE_FETCH,
  CVA_GATING_CMD_STATE_EXECUTE,
  CVA_GATING_CMD_STATE_CONTINUE

} cva_gating_cmd_state_t;

typedef struct cva_gating_control_t {
  
  apr_list_t cmd_q;
  /**< The gating (cva_work_item_t) queue. */
  cva_gating_cmd_state_t state;
  /**<
   * The current state of the gating command control.
   *
   * This variable is managed by the gating command processor. The
   * individual gating command controls indicates to the gating command
   * processor to complete or to delay the completion of the current
   * gating command.
   */
  cva_work_item_packet_type_t pkt_type;
  /**<
   * This is required for Gating commands to identify the packet type
   * below mentioned.
   */
  void* packet;
  /**<
   * The current (command) packet being processed.
   * This is generic reference to command packet, which could be
   * cva_cmd_packet_t or cva_event_packet_t.
   */
  cva_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
} cva_gating_control_t;


/****************************************************************************
 * CVA INTERNAL ROUTINES                                                    *
 ****************************************************************************/

APR_INTERNAL uint32_t cva_queue_work_packet (
  cva_work_item_queue_type_t queue_type,
  cva_work_item_packet_type_t pkt_type,
  void* packet
);


APR_INTERNAL uint32_t cva_free_cmd_packet (
  cva_cmd_packet_t* cmd_packet
);


APR_INTERNAL uint32_t cva_prepare_and_dispatch_cmd_packet (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);


APR_INTERNAL uint32_t cva_free_event_packet (
  cva_event_packet_t* event_packet
);


APR_INTERNAL uint32_t cva_prepare_and_dispatch_event_packet (
  void* session_context,
  uint32_t event_id,
  void* params,
  uint32_t size
);


APR_INTERNAL void cva_signal_run ( 
  void 
);


#endif  /* __CVA_I_H__ */
