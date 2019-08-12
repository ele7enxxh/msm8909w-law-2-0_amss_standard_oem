#ifndef __MVS_H__
#define __MVS_H__

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This header file defines function interfaces and corresponding data
  structures for invoking multi-mode voice services.

REFERENCES
  None.

Copyright (C) 2002-2010, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/mvs/mvs.h#1 $
$Author: mplcsds1 $

===========================================================================*/

#define MVS_VERSION_MAJOR ( 2 )
#define MVS_VERSION_MINOR ( 0 )

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "mvssup.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Definitions of MVS client type */
typedef enum {
  MVS_CLIENT_CDMA,
  MVS_CLIENT_WCDMA,
  MVS_CLIENT_GSM,
  MVS_CLIENT_VOIP,
  MVS_CLIENT_QCHAT,
  MVS_CLIENT_VMEMO,
  MVS_CLIENT_QVP,
  MVS_CLIENT_QVP_TEST,
  MVS_CLIENT_TEST,
  MVS_CLIENT_TDSCDMA,
  MVS_CLIENT_MAX,
  MVS_CLIENT_NONE = MVS_CLIENT_MAX
} mvs_client_type;

/* Definitions of context in which MVS packet exchange takes place */
typedef enum {
  MVS_PKT_CONTEXT_NONE,           /* no packet exchange requested */
  MVS_PKT_CONTEXT_ISR,            /* packets are exchanged in ISR context */
  MVS_PKT_CONTEXT_VOC,            /* packets are exchanged in VOC task context */
  MVS_PKT_CONTEXT_ISR_ASYNC_RSP,  /* packets are pushed / request in ISR context  */
  /* Response in client context */
  MVS_PKT_CONTEXT_SIO,            /* Using SIO port for packet exchange with clients */
  MVS_PKT_CONTEXT_MAX
} mvs_pkt_context_type;

/* Definitions of MVS loopback type */
typedef enum {
  MVS_LB_NONE,

  MVS_LB_UL2DL_VOC,
  MVS_LB_UL2DL_DSM,
  MVS_LB_UL2DL_PCM,

  MVS_LB_DL2UL_VOC,
  MVS_LB_DL2UL_PCM,

  MVS_LB_MAX
} mvs_loopback_type;

/* ==========================================================================
** Callback data structure definitions
** ==========================================================================
*/

/* Event type definitions */
typedef enum {
  MVS_EV_COMMAND,  /* command status */
  MVS_EV_MODE,     /* mode status    */
  MVS_EV_NOTIFY,   /* information notification initiated from mvs */
  MVS_EV_EAMR_SR   /* eAMR sample rate change notification */
} mvs_event_enum_type;

typedef struct {
  mvs_event_enum_type event;
  mvs_client_type client;
} mvs_ev_header_type;

/* Command event definitions */
typedef enum {
  MVS_CMD_FAILURE,
  MVS_CMD_BUSY,
  MVS_CMD_SUCCESS
} mvs_cmd_status_type;

typedef struct {
  mvs_ev_header_type hdr;
  mvs_cmd_status_type cmd_status;
} mvs_ev_command_type;

/* Mode event definitions */
typedef enum {
  MVS_MODE_NOT_AVAIL,
  MVS_MODE_INIT,
  MVS_MODE_READY
} mvs_mode_status_type;

typedef struct {
  mvs_ev_header_type hdr;
  mvs_mode_status_type mode_status;
  mvs_mode_type mode;
} mvs_ev_mode_type;

/* Notify event definitions */
typedef enum {
  MVS_NOTIFY_BUFFER_CFG,
  MVS_NOTIFY_BUFFER_STATUS
} mvs_notify_enum_type;

typedef struct {
  mvs_ev_header_type hdr;
  mvs_notify_enum_type notify_id;
} mvs_ev_notify_hdr_type;

typedef enum {
  MVS_BUFFER_DIR_RX,
  MVS_BUFFER_DIR_TX
} mvs_buffer_dir_enum_type;

typedef struct {
  mvs_ev_notify_hdr_type hdr;
  mvs_buffer_dir_enum_type dir;
  uint16 max_frames;
} mvs_ev_notify_buffer_cfg_type;

typedef enum {
  MVS_BUFFER_STATUS_INIT_FAIL,
  MVS_BUFFER_STATUS_OVERFLOW,
  MVS_BUFFER_STATUS_EMPTY,
  MVS_BUFFER_STATUS_LOCKED,
  MVS_BUFFER_STATUS_RD_SUCCESS,
  MVS_BUFFER_STATUS_WR_SUCCESS
} mvs_buffer_status_enum_type;

typedef struct {
  mvs_ev_notify_hdr_type hdr;
  mvs_buffer_dir_enum_type dir;
  mvs_buffer_status_enum_type status;
} mvs_ev_notify_buffer_status_type;

typedef union {
  mvs_ev_notify_hdr_type hdr;
  mvs_ev_notify_buffer_cfg_type buf_cfg;
  mvs_ev_notify_buffer_status_type buf_status;
} mvs_ev_notify_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_ev_notify_type DISC (_OBJ_.hdr.notify_id) */
/*~ CASE MVS_NOTIFY_BUFFER_CFG mvs_ev_notify_type.buf_cfg */
/*~ CASE MVS_NOTIFY_BUFFER_STATUS mvs_ev_notify_type.buf_status */
/*~ DEFAULT mvs_ev_notify_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* eAMR sample rate event definitions */
typedef enum {
  MVS_EAMR_SR_8000,  /* 8000 Hz  */
  MVS_EAMR_SR_16000  /* 16000 Hz */
} mvs_eamr_sr_type;

/* eAMR sample rate event structure */
typedef struct {
  mvs_ev_header_type hdr;
  mvs_eamr_sr_type sr;
} mvs_ev_eamr_sr_type;

/* Generic event data type definitions */
typedef union {
  mvs_ev_header_type hdr;
  mvs_ev_command_type cmd;
  mvs_ev_mode_type mode;
  mvs_ev_notify_type notify;
  /* This event is sent only when eAMR sample rate event
   * is turned on in the voice configuration file.
   */
  mvs_ev_eamr_sr_type eamr; 
} mvs_event_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_event_type DISC (_OBJ_.hdr.event) */
/*~ CASE MVS_EV_COMMAND mvs_event_type.cmd */
/*~ CASE MVS_EV_MODE mvs_event_type.mode */
/*~ CASE MVS_EV_NOTIFY mvs_event_type.notify */
/*~ DEFAULT mvs_event_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* ==========================================================================
** Callback function type definitions
** ==========================================================================
*/

/* Event callback function type */
typedef void ( *mvs_event_cb_type ) ( mvs_event_type* event );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK mvs_event_cb_type POINTER */
/*~ PARAM IN event POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Uplink packet transfer callback function type */
typedef void ( *mvs_packet_ul_fn_type ) (
  uint8* vocoder_packet,
  mvs_frame_info_type* frame_info,
  uint16 packet_length,
  mvs_pkt_status_type* status
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK mvs_packet_ul_fn_type POINTER */
/*~ PARAM IN vocoder_packet VARRAY LENGTH packet_length */
/*~ PARAM IN frame_info POINTER */
/*~ PARAM OUT status POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Downlink packet transfer callback function type */
typedef void ( *mvs_packet_dl_fn_type ) (
  uint8* vocoder_packet,
  mvs_frame_info_type* frame_info,
  mvs_pkt_status_type* status
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK mvs_packet_dl_fn_type POINTER */
/*~ PARAM OUT vocoder_packet ARRAY 320 */
/*~ PARAM INOUT frame_info POINTER */
/*~ PARAM OUT status POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Packet listening callback function type */
typedef void ( *mvs_listen_fn_type ) (
  uint8* vocoder_packet,
  mvs_frame_info_type* frame_info
);

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_mode_supported

DESCRIPTION
  This function determines if a requested MVS mode is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : supported.
  FALSE : not.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mvs_mode_supported (
  mvs_mode_type mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_mode_supported */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_set_loopback

DESCRIPTION
  This function will set up the MVS loopback mode:

  MVS_LB_NONE: no loopback

  MVS_LB_UL2DL_VOC: route uplink voice packets to downlink; bypass DSM queues
  MVS_LB_UL2DL_DSM: route uplink voice packets to downlink through DSM queues
  MVS_LB_UL2DL_PCM: route uplink PCM samples into downlink

  MVS_LB_DL2UL_VOC: route downlink voice packets back to uplink
  MVS_LB_DL2UL_PCM: route downlink PCM samples into uplink

  NOTE: if DSM queues are not utilized, MVS_LB_UL2DL_DSM functions the same as
        MVS_LB_UL2DL_VOC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_set_loopback (
  mvs_loopback_type lb_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_set_loopback */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_acquire

DESCRIPTION
  This function allows a client to acquire MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_acquire (
  mvs_client_type client,
  mvs_event_cb_type cb_func
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_acquire */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_enable

DESCRIPTION
  This function enables an MVS client to use the vocoder to process audio
packets of a given mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_enable (
  mvs_client_type client,
  mvs_mode_type mode,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/* FEATURE_HTORPC_METACOMMENTS */
/*~ FUNCTION mvs_enable */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_standby

DESCRIPTION
  This function will cause vocoder to download the requested image but stay
in IDLE mode until mvs_enable is invoked.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_standby(
  mvs_client_type client,
  mvs_mode_type mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/* FEATURE_HTORPC_METACOMMENTS */
/*~ FUNCTION mvs_standby */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_listen

DESCRIPTION
  This function enables an MVS client to listen to the packets sent to and from
the vocoder.  It is typically used for in-call recording.

DEPENDENCIES
  A client that has called mvs_acquire() cannot also call mvs_listen().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_listen (
  mvs_client_type client,
  mvs_event_cb_type cb_func,
  mvs_listen_fn_type ul_func,
  mvs_listen_fn_type dl_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_release

DESCRIPTION
  This function disables an MVS client, shutting down the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_release (
  mvs_client_type client
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_release */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_amr_set_amr_mode

DESCRIPTION
  This function sets the AMR transfer mode of an AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_amr_set_amr_mode (
  mvs_amr_mode_type amr_ul_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_amr_set_amr_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_amr_set_awb_mode

DESCRIPTION
  This function sets the AMR WB transfer mode of an AMR WBvocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_amr_set_awb_mode (
  mvs_amr_mode_type amr_ul_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_amr_set_awb_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*===========================================================================

FUNCTION mvs_amr_set_scr_mode

DESCRIPTION
  This function sets the SCR mode of the AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_amr_set_scr_mode (
  mvs_scr_mode_type scr_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_amr_set_scr_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_voc_set_frame_rate

DESCRIPTION
  This function sets the max. and min. transfer rates for IS733, IS127 modes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_voc_set_frame_rate (
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_voc_set_frame_rate */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_gsm_set_dtx_mode

DESCRIPTION
  This function sets the DTX mode of a GSM vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_gsm_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_gsm_set_dtx_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_g711a_set_dtx_mode

DESCRIPTION
This function sets the DTX mode of a g711a vocoder session.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
extern void mvs_g711a_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_g711a_set_dtx_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_g729a_set_dtx_mode

DESCRIPTION
  This function sets the DTX mode of a G.729A vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_g729a_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_g729a_set_dtx_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_get_g711_mode

DESCRIPTION
  This function gets the current G.711 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g711_mode_type mvs_get_g711_mode ( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_get_g711_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_set_g711_mode

DESCRIPTION
  This function sets the G.711 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_set_g711_mode (
  mvs_g711_mode_type mvs_g711_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_set_g711_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_get_g711a_mode

DESCRIPTION
  This function gets the current G.711 A1/A2 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g711a_mode_type mvs_get_g711a_mode ( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_get_g711a_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_set_g711a_mode

DESCRIPTION
  This function sets the G.711 A1/A2 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_set_g711a_mode (
  mvs_g711a_mode_type mvs_g711a_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_set_g711a_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_set_g722_mode

DESCRIPTION
  This function sets the G.722 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_set_g722_mode (
  mvs_g722_mode_type mvs_g722_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_set_g722_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_get_g722_mode

DESCRIPTION
  This function gets the current G.722 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  g722 mode

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g722_mode_type mvs_get_g722_mode ( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_get_g722_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_voc_tx_rate_limit

DESCRIPTION
  This function limits the vocoder transmit rate to ( 2s+1 ) / ( 2s+2 ).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_voc_tx_rate_limit (
  word rate_factor
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_voc_tx_rate_limit */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mvs_client_send_frames

DESCRIPTION
  This function is called by the client which will copy frames into the
  mvs queue.

DEPENDENCIES
  This function should only be used in the ISR_ASYNC_RSP context.
  Client needs to check not to overflow the buffer.
  Client should only call this function when they receive request for downlink packets.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_client_send_frames (
  mvs_client_type client,
  mvs_frame_type* frame_ptr,  /* array of frame structure  */
  uint16 mvs_frame_count      /* No of frame in frame_ptr  */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvs_client_send_frames */
/*~ PARAM IN frame_ptr VARRAY LENGTH mvs_frame_count */
/* frame_ptr is the array of frame */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef __cplusplus
}
#endif

#endif  /* !MVS_H */

