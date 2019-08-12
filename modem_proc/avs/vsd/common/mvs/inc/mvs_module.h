#ifndef __MVS_MODULE_H__
#define __MVS_MODULE_H__

/*
  Copyright (C) 2009-2015 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/mvs_module.h#1 $
  $Author: mplcsds1 $
*/

/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/

#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "apr_list.h"
#include "vs.h"
#include "mvsi.h"

/****************************************************************************
  MVS DEFINES
****************************************************************************/

#define MVS_VERSION ( 0x1A2B3C45 )

#define MVS_HEAP_SIZE ( 10000 )

#define MVS_NUM_COMMANDS ( 30 )

#define MVS_HANDLE_TOTAL_BITS ( 16 )
#define MVS_HANDLE_INDEX_BITS ( 7 ) /* 7 bits = 128 handles. */

#define MVS_MAX_OBJECTS ( 1 << MVS_HANDLE_INDEX_BITS )

/****************************************************************************
  COMMAND RESPONSE FUNCTION TABLE
****************************************************************************/

/****************************************************************************
   MVS OBJECT DEFINITIONS
 ****************************************************************************/

typedef enum mvs_object_type_enum_t
{
   MVS_OBJECT_TYPE_ENUM_UNINITIALIZED,
   MVS_OBJECT_TYPE_ENUM_HANDLE,
   MVS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   MVS_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
   MVS_OBJECT_TYPE_ENUM_INVALID,
}
   mvs_object_type_enum_t;

typedef struct mvs_object_header_t
{
   uint32_t handle;
      /**< The handle to the associated apr_objmgr_object_t instance. */
   mvs_object_type_enum_t type;
      /**<
       * The object type defines the actual derived object.
       *
       * The derived object can be any custom object type. A session or a
       * command are two such custom object types. A free object entry is set
       * to MVS_OBJECT_TYPE_ENUM_FREE.
       */
}
  mvs_object_header_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE TOKEN OBJECT                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvs_token_object_t
{
  mvs_object_header_t header;
} mvs_token_object_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SIMPLE JOB OBJECT                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvs_simple_job_object_t
{
   mvs_object_header_t header;
   uint32_t parentjob_handle;
      /**<
       * The parent-job handle indicates this sub-job is part of a batch-job.
       *
       * Set this value to -1 when it is unused or when there is no parent.
       */
   bool_t is_completed;
      /**< The command completed response flag. 0 is false and 1 is true. */
   uint32_t status;
    /**< The status returned by the command completion. */
}
  mvs_simple_job_object_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SEQUENCER JOB OBJECT                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvs_sequencer_job_object_t
{
  mvs_object_header_t header;

  uint32_t state;
    /**< The generic state variable. */
  uint32_t helper_state;
   /**< Helper function State variable. */
  union mvs_object_t* subjob_obj;
    /**< The current sub-job object. */
  uint32_t status;
    /**< A status value. */
}
  mvs_sequencer_job_object_t;

typedef enum mvs_cmd_type_enum_t
{
  MVS_CMD_TYPE_ENUM_UNINITIALIZED,
  MVS_CMD_TYPE_ENUM_VS_CMD_RSP,
  MVS_CMD_TYPE_ENUM_VS_EVT,
  MVS_CMD_TYPE_ENUM_VS_EAMR_MODE,
  MVS_CMD_TYPE_ENUM_INTERNAL_CMD,
  MVS_CMD_TYPE_ENUM_INVALID,
}
  mvs_cmd_type_enum_t;

typedef struct mvs_cmd_header_t
{
   uint32_t handle;
      /**< The handle to the associated apr_objmgr_object_t instance. */
   mvs_cmd_type_enum_t type;
      /**<
       * The object type defines the actual derived object.
       *
       * The derived object can be any custom object type. A session or a
       * command are two such custom object types. A free object entry is set
       * to MVS_OBJECT_TYPE_ENUM_FREE.
       */
}
  mvs_cmd_header_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE VS COMMAND RESPONSE                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvs_vs_cmd_rsp_t
{
  mvs_cmd_header_t header;

  uint32_t cmd_id;
    /**< The command that completed. */
  uint32_t status_id;
    /**< The status code returned. Refer to vs_errcodes.h. */
  void* context;
}
  mvs_vs_cmd_rsp_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE VS EVENT                                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvs_vs_evt_t
{
  mvs_cmd_header_t header;

  uint32_t evt_id;

  void* context;
}
  mvs_vs_evt_t;

typedef struct mvs_vs_eamr_mode_t
{
  mvs_cmd_header_t header;

  uint32_t mode;

  void* context;
}
  mvs_vs_eamr_mode_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE MVS INTERNAL COMMAND                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvs_internal_cmd_t
{
  mvs_cmd_header_t header;

  mvs_packet_type type;
    /**< QIN: TBD. */
}
  mvs_internal_cmd_t;

typedef union mvs_cmd_t
{
  mvs_cmd_header_t header;
  mvs_vs_cmd_rsp_t vs_cmd_rsp;
  mvs_vs_evt_t vs_evt;
  mvs_vs_eamr_mode_t vs_eamr_mode;
  mvs_internal_cmd_t internal_cmd;
}
  mvs_cmd_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE GENERIC MVS OBJECT                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef union mvs_object_t
{
   mvs_object_header_t header;
   mvs_token_object_t handle;
   mvs_simple_job_object_t simple_job;
   mvs_sequencer_job_object_t sequencer_job;

}
  mvs_object_t;

/****************************************************************************
 * PENDING MVS COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum mvs_pending_cmd_state_enum_t
{
   MVS_PENDING_CMD_STATE_ENUM_FETCH,
   MVS_PENDING_CMD_STATE_ENUM_EXECUTE,
   MVS_PENDING_CMD_STATE_ENUM_CONTINUE
}
   mvs_pending_cmd_state_enum_t;

typedef struct mvs_pending_control_t
{
   mvs_pending_cmd_state_enum_t state;
    /**<
     * The current state of the pending command control.
     *
     * This variable is managed by the pending command processor. The
     * individual pending command controls indicates to the pending command
     * processor to complete or to delay the completion of the current
     * pending command.
     */
   mvs_cmd_t* cmd;
    /**<
     * The current (command) packet being processed.
     */
   mvs_object_t* rootjob_obj;
    /**<
     * The rootjob_obj is a temporary storage for the current pending
     * command.
     */
}
   mvs_pending_control_t;

/****************************************************************************
  MVS INTERNAL DEFINITIONS
****************************************************************************/

typedef enum {
  MVS_STATE_RESET,
  MVS_STATE_IDLE,
  MVS_STATE_VOCODER,
} mvs_state_enum_type;

typedef struct {
  mvs_cmd_type cur_cmd;
  bool_t is_done;      /* 0 - completed and 1 - pending */
  uint32_t job_token;
} mvs_cmd_info_type;

typedef struct {
  uint32_t handle;
} mvs_vs_session_obj;

typedef struct {
  uint32_t handle;
  uint32_t client_id;
  vs_voc_buffer_t* read_buf;
  vs_voc_buffer_t* write_buf;
  vs_voc_buffer_t* read_buf_2;
    /**< For G711/G729 only */
  vs_common_event_callback_fn_t event_cb;
  bool_t is_write_buf_returned;
  bool_t is_vs_ready;
} mvs_vs_ctrl_type;

typedef struct {
  mvs_vs_ctrl_type vs_ctrl;
  mvs_state_enum_type state;
  mvs_event_cb_type event_cb;
  mvs_cmd_info_type pending_cmd;
  mvs_mode_type config_mode;
  mvs_packet_ul_fn_type ul_cb;
  mvs_packet_dl_fn_type dl_cb;
} mvs_client_state_type;

/****************************************************************************
  MVS WORK QUEUE DEFINITIONS
****************************************************************************/

typedef struct mvs_work_item_t
{
  apr_list_node_t link;
  mvs_cmd_t* cmd;
}
  mvs_work_item_t;

/****************************************************************************
  MVS INTERNAL ROUTINES
****************************************************************************/

int32_t mvs_mod_acquire (
  mvs_client_type client,
  mvs_event_cb_type cb_func
);
int32_t mvs_mod_enable (
  mvs_client_type client,
  mvs_mode_type mode,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
);
int32_t mvs_mod_standby (
  mvs_client_type client,
  mvs_mode_type mode
);
int32_t mvs_mod_release (
  mvs_client_type client
);

boolean mvs_mod_mode_supported (
  mvs_mode_type mode
);

int32_t mvs_mod_voc_set_frame_rate (
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
);

int32_t mvs_mod_voc_tx_rate_limit (
  uint16 rate_factor
);

int32_t mvs_mod_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
);

int32_t mvs_mod_amr_set_scr_mode (
  mvs_scr_mode_type scr_mode
);

void mvs_mod_set_g711_mode (
  mvs_g711_mode_type mode
);

mvs_g711_mode_type mvs_mod_get_g711_mode ( void );

void mvs_mod_set_g722_mode (
  mvs_g722_mode_type mode
);

mvs_g722_mode_type mvs_mod_get_g722_mode ( void );


int32_t mvs_mod_amr_set_amr_mode (
  mvs_amr_mode_type amr_ul_mode
);

int32_t mvs_mod_amr_set_awb_mode (
  mvs_amr_mode_type awb_ul_mode
);

mvs_amr_mode_type mvs_get_ul_amr_mode ( void );

mvs_amr_mode_type mvs_get_ul_awb_mode ( void );

mvs_g711a_mode_type map_g711_mode_to_g711a (
  mvs_g711_mode_type g711_mode
);

mvs_g711_mode_type map_g711a_mode_to_g711 (
  mvs_g711a_mode_type g711a_mode
);

mvs_voc_rate_type mvs_mod_get_voc_max_rate( void );

mvs_voc_rate_type mvs_mod_get_voc_min_rate( void );

int32_t mvs_umts_dl_processing (
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

int32_t mvs_umts_ul_processing (
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

typedef union mvs_g711a_vs_frame_info_t
{
  vs_vocg711alaw_frame_info_t alaw;
  vs_vocg711mulaw_frame_info_t mulaw;
}
  mvs_g711a_vs_frame_info_t;


/****************************************************************************
  MVS INTERNAL, FOR SILENCE FRAME GENERATION
****************************************************************************/

typedef enum mvs_call_event_type
{
   MVS_CALL_EVENT_START = 0,
   MVS_CALL_EVENT_STOP = 1,
   MVS_CALL_EVENT_MAX   
} 
  mvs_call_event_type;

typedef uint32_t ( *mvs_event_fn_t ) ( 
  mvs_client_type client,
  mvs_call_event_type event
);

/* The intended user is MVA, to register callbacks
   for notification of call events.
   Shall only call once.
*/
uint32_t mvs_listen_for_event (
  mvs_event_fn_t event_cb
);

/* Timing offset parameter */
typedef struct mvs_timing_offset_type
{
  uint16_t enc_offset;
  uint16_t dec_req_offset;
  uint16_t dec_offset;
}
  mvs_timing_offset_type;

/* Get MVS client's timing offset parameter. */
uint32_t mvs_get_timing_offset (
  mvs_client_type client,
  mvs_timing_offset_type* offset
);

/* The intended user is MVA, to notify MVS to
   deliver encoder packet to MVS client. */
uint32_t mvs_send_silence_frame (
  void
);
  
#endif /* _MVS_MODULE_H_*/

