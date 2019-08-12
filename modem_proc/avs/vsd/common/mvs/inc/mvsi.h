#ifndef __MVSI_H__
#define __MVSI_H__

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This header file defines function interfaces and corresponding data
  structures for invoking multi-mode voice services.

REFERENCES
  None.

Copyright (C) 2009 - 2014 QUALCOMM Technologies, Inc.
All rights reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/mvsi.h#1 $
$Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
//#include "queue.h"
#include "apr_comdef.h"
#include "mvs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* MVS Command Queue Available Packets.
*/
typedef enum {
  MVS_CMD_ACQUIRE,
  MVS_CMD_ENABLE,
  MVS_CMD_STANDBY,
  MVS_CMD_LISTEN,
  MVS_CMD_RELEASE,
  MVS_CMD_MODE,
  MVS_CMD_UPDATE,
  MVS_CMD_TX,
  MVS_CMD_RX,
  MVS_CMD_SET_AMR_MODE,
  MVS_CMD_SET_AWB_MODE,
  MVS_CMD_SET_DTX_MODE,
  MVS_CMD_SET_CDMA_MINMAX_RATE,
  MVS_CMD_SET_CDMA_RATE_FACTOR,
  MVS_CMD_SEND_SILENCE_FRAME,
  MVS_CMD_INVALID
} mvs_cmd_type;

/* Header for MVS commands
*/
typedef struct
{
#if 0
  q_link_type            link;         /* Command queue link               */
#endif /* 0 */
  mvs_cmd_type           cmd;          /* MVS command to perform           */
} mvs_hdr_type;

/* Base packet type for MVS commands that use standard CB function and have
** no parameters.
*/
typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
} mvs_base_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_client_type        client;
} mvs_acquire_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_client_type        client;
  mvs_mode_type          mode;
  mvs_packet_ul_fn_type  ul_func;
  mvs_packet_dl_fn_type  dl_func;
  mvs_pkt_context_type   context;
} mvs_enable_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_client_type        client;
  mvs_mode_type          mode;
} mvs_standby_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_client_type        client;
  mvs_listen_fn_type     ul_func;
  mvs_listen_fn_type     dl_func;
} mvs_listen_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_client_type        client;
} mvs_release_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_mode_type          mode;
} mvs_mode_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_client_type        client;
  mvs_frame_q_type       *ul_qptr;     /* uplink voice frame buffer */
  mvs_packet_ul_fn_type  ul_func;      /* uplink callback function */
} mvs_tx_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  mvs_client_type        client;
  mvs_frame_q_type       *dl_qptr;     /* downlink voice frame buffer */
  mvs_packet_dl_fn_type  dl_func;      /* downlink callback function */
} mvs_rx_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_amr_mode_type      amr_mode;
} mvs_set_amr_mode_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_dtx_mode_type      dtx_mode;
} mvs_set_dtx_mode_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  mvs_voc_rate_type      max_rate;
  mvs_voc_rate_type      min_rate;
} mvs_set_cdma_minmax_rate_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */

  uint16                 rate_factor;
} mvs_set_cdma_rate_factor_type;

typedef union {
  mvs_hdr_type hdr;
  mvs_base_packet_type base;      /* Base MVS packet type */
  mvs_acquire_packet_type acquire;
  mvs_enable_packet_type enable;
  mvs_standby_packet_type standby;
  mvs_listen_packet_type listen;
  mvs_release_packet_type release;
  mvs_mode_packet_type mode;
  mvs_tx_packet_type tx;
  mvs_rx_packet_type rx;
  mvs_set_amr_mode_packet_type set_amr_mode;
  mvs_set_dtx_mode_packet_type set_dtx_mode;
  mvs_set_cdma_minmax_rate_packet_type set_cdma_minmax_rate;
  mvs_set_cdma_rate_factor_type set_cdma_rate_factor;
} mvs_packet_type;

/* For code profiling. 
   1. Tap points are enable/disabled via QXDM command. 
   2. Enter tap point is odd and leave tap pointer is even.
   3. QXDM command uses enter tap point to enable/disable the 
      corresponding enter and leave tap point pair. 
   4. Add new tap point pair to the end of the enum.
   5. Offline parser is used to parse the saved profiling
      circulr buffer. Currently the parser expects the enter 
      and leave tap point pair is NOT interlaved with other pairs. */
typedef enum
{
  MVS_PROFILING_TAP_POINT_INVALID = 0,
  MVS_PROFILING_SET_DTX_MODE_ENTER = 1,
  MVS_PROFILING_SET_DTX_MODE_LEAVE = 2,
  MVS_PROFILING_SET_AMR_MODE_ENTER = 3,
  MVS_PROFILING_SET_AMR_MODE_LEAVE = 4,
  MVS_PROFILING_SET_AWB_MODE_ENTER = 5,
  MVS_PROFILING_SET_AWB_MODE_LEAVE = 6,
  MVS_PROFILING_SET_FRAME_RATE_ENTER = 7,
  MVS_PROFILING_SET_FRAME_RATE_LEAVE = 8,
  MVS_PROFILING_SET_TX_RATE_LIMIT_ENTER = 9,
  MVS_PROFILING_SET_TX_RATE_LIMIT_LEAVE = 10,
  MVS_PROFILING_ACQUIRE_ENTER = 11,
  MVS_PROFILING_ACQUIRE_LEAVE = 12,
  MVS_PROFILING_ENABLE_ENTER = 13,
  MVS_PROFILING_ENABLE_LEAVE = 14,
  MVS_PROFILING_STANDBY_ENTER = 15,
  MVS_PROFILING_STANDBY_LEAVE = 16,
  MVS_PROFILING_RELEASE_ENTER = 17,
  MVS_PROFILING_RELEASE_LEAVE = 18,
  MVS_PROFILING_QUEUE_IN_PKT_ENTER = 19,
  MVS_PROFILING_QUEUE_IN_PKT_LEAVE = 20,
  MVS_PROFILING_QUEUE_PENDING_PKT_ENTER = 21,
  MVS_PROFILING_QUEUE_PENDING_PKT_LEAVE = 22,
  MVS_PROFILING_ACCEPT_CMD_ENTER = 23,
  MVS_PROFILING_ACCEPT_CMD_LEAVE = 24,
  MVS_PROFILING_ALLOC_SEND_ENTER = 25,
  MVS_PROFILING_ALLOC_SEND_LEAVE = 26,
  MVS_PROFILING_ALLOC_EXT_ENTER = 27,
  MVS_PROFILING_ALLOC_EXT_LEAVE = 28,
  MVS_PROFILING_SIGNAL_RUN_ENTER = 29,
  MVS_PROFILING_SIGNAL_RUN_LEAVE = 30,
  MVS_PROFILING_SET_DTX_MSG_ENTER = 31,
  MVS_PROFILING_SET_DTX_MSG_LEAVE = 32,
  MVS_PROFILING_IN_PKT_MSG_ENTER = 33,
  MVS_PROFILING_IN_PKT_MSG_LEAVE = 34,
  MVS_PROFILING_ADD_TAIL_ENTER = 35,
  MVS_PROFILING_ADD_TAIL_LEAVE = 36,
  MVS_PROFILING_REMOVE_HEAD_ENTER = 37,
  MVS_PROFILING_REMOVE_HEAD_LEAVE = 38,   
  /* To be expanded. Add new pair right above this line.*/
  MVS_PROFILING_TAP_POINT_MAX_ENTER,
  MVS_PROFILING_TAP_POINT_MAX_LEAVE
}
  mvs_profiling_tap_point_t;


APR_INTERNAL uint32_t mvs_profiling_enable_tap_point (
  mvs_profiling_tap_point_t tap_point
);

APR_INTERNAL uint32_t mvs_profiling_disable_tap_point (
  mvs_profiling_tap_point_t tap_point
);  

APR_INTERNAL void mvs_profiling_save_entry ( 
  mvs_profiling_tap_point_t tap_point
);

APR_INTERNAL void mvs_profiling_clear_buffer (
  void
);

APR_INTERNAL uint32_t mvs_profiling_save_buffer (
  void
);

/*===========================================================================

F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_init

DESCRIPTION
  This procedure initializes the MVS command queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_cmd_q
  mvs_free_q
  mvs_free_packet

===========================================================================*/
extern void mvs_init ( void );

#ifdef __cplusplus
}
#endif

#endif  /* __MVSI_H__ */

