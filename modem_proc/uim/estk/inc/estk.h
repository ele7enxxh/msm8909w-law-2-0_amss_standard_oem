#ifndef ESTK_H
#define ESTK_H

/*======================================================
FILE:  estk.h

GENERAL DESCRIPTION:
    ESTK generic interface header include file

        Copyright © 2008 - 2015 QUALCOMM Technologies, Inc(QTI).
        All Rights Reserved.
        QUALCOMM Technologies Proprietary
=====================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/inc/estk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/15   vr      Correction in SEND SMS TR result values
12/16/14   vr      Check WMS ready before sending SMS to WMS
11/04/14   dy      Remove FEATURE_ESTK_WMS_INTERFACE
10/21/14   shr     Close channel kept open if next proactive cmd is unexpected
08/07/14   dy      Process WMS events in GSTK task context
07/28/14   gm      Proper free of memory allocated
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
07/01/14   kr      Removing tmc from UIM codebase
05/27/14   dy      Remove usage of gstk_util_get_curr_access_tech() in ESTK
05/06/14   gs      Removed the ESTK featurization
03/24/14   vr      Split ESTK init handling
03/04/14   shr     Made BIP Session in progress API slot specific
03/03/14   shr     ESTK BIP clean-up and enhancements
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
01/09/14   kb      Added estk_stop function
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
08/06/13   vr      Centralized Call Control Changes
07/23/13   hh      Removed unused/redundant estk_cmd_enum_type
07/15/13   gs      Remove NV 5770 dependency for STK related tests except for GSM
07/01/13   hn      Send command_number, additional_info and other_info to ESTK
06/20/13   vr      Triton TSTS changes
05/17/13   hn      Support for Alpha for Refresh
04/02/13   av      Move UIM tasks to group4 of RCINIT
03/08/13   xz      Fix compiler warning
08/10/12   av      Removed RPC support along with RPC style comments
05/16/12   dd      Use CM events to start DTMF timer
03/28/12   av      Replaced feature flag with NVITEM
12/23/11   bd      Support for FR 1479
11/29/11   dd      Additional info in TR if hold active calls fails
10/17/11   bd      Fixed incorrect use of pointers in CM’s context
09/11/11   dd      Clear data buffer in send data or listen state
07/13/11   dd      Clear the SCWS Tx/Rx buffers
05/05/11   nk      Reverted removal of tmc.h to resolve compilation error
04/28/11   nk      Warning Fix to remove tmc.h as it is not used
04/24/11   dd      SCWS support
04/22/11   yk      Fix for a card issue leading to card error during STATUS
                   command
01/31/11   dd      User conf with icon for BIP command support
01/12/11   sg      Dual slot scenarios cleanup on GSTK timer expiry
01/04/11   sg      Add API for cleanup on GSTK timer expiry
12/10/10   nb      Added evt_cb to estk_cmd_type
11/30/10   yt      Including private GSTK header
11/18/10   sg      Add ESTK_BIP_NETWORK_DISCONNECT_CMD for close channel
06/30/10   dd      Icon display confirmation
03/12/10   sg      Changed data type for socket event
02/15/10   nb      Moved Dual Slot Changes
12/17/09   sg      Fixed compilation errors when FEATURE_ESTK is enabled
09/16/08   sk      Fixed lint error
09/08/08   tml     Added header revision autogeneration
09/02/08   sk      Initial Creation.Add support for Setup call, Send DTMF
=============================================================================*/

/*--------------------------------------------------------------------------
                             COMMON DEFINTION HEADER FILES
---------------------------------------------------------------------------*/
#include "uim_variation.h"
#include "customer.h"
#ifdef FEATURE_ESTK
#include "comdef.h"
#include "task.h"
#include "intconv.h"
#include "cm_dualsim.h"
#include "cm.h"
#include "dss_iface_ioctl.h"

/*--------------------------------------------------------------------------
                             INTERFACE HEADER FILES
---------------------------------------------------------------------------*/
#include "wms.h"
#include "gstk_p.h"
#include "gstk_exp.h"

/*--------------------------------------------------------------------------
                             GLOBAL DECLERATIONS
---------------------------------------------------------------------------*/
extern uint8 estk_num_of_instances;

/*--------------------------------------------------------------------------
                             MACRO DEFINTIONS
---------------------------------------------------------------------------*/
/* 3GPP TS 23.003 */
#define ESTK_APN_MAX_LENGTH             100

#define ESTK_APN_MAPPING_MAX            5

#define ESTK_DEFAULT_REDIAL_INTERVAL_MS 3000

/*--------------------------------------------------------------------------
                             ENUMERATIONS
---------------------------------------------------------------------------*/

/*===========================================================================

  STRUCT: ESTK_RESULT_ENUM_TYPE

  PARAMETERS:
    ESTK_SUCCESS       : Success
    ESTK_ERROR         : Error
    ESTK_UNSUPPORTED   : Operation not supported
    ESTK_BAD_PARAM     : Bad input parameter
    ESTK_RESULT_UNKNOWN: Result unknown

  PURPOSE:
    The enumeration indicates the various ESTK result types.
===========================================================================*/
typedef enum
{
  ESTK_SUCCESS,
  ESTK_ERROR,
  ESTK_UNSUPPORTED,
  ESTK_BAD_INPUT_PARAM,
  ESTK_RESULT_UNKNOWN          = 0x7FFFFFFF
}
estk_result_enum_type;

/*===========================================================================

  STRUCT: ESTK_CMD_ENUM_TYPE

  PARAMETERS:
    ESTK_NONE_CMD                : No valid command
    ESTK_CM_CALL_EVENT_CMD       : CM call event command
    ESTK_CM_CALL_LIST_CMD        : CM call list command
    ESTK_ICON_DISPLAY_CONF_CMD   : Icon display confirmation
    ESTK_CM_SUPS_EVENT_CMD       : CM sups event command
    ESTK_CM_INBAND_EVENT_CMD     : CM inband event command
  PURPOSE:
    The enumeration indicates the various ESTK command types
===========================================================================*/
typedef enum
{
  ESTK_NONE_CMD,
  ESTK_CM_CALL_EVENT_CMD,                  /* CM Call Event      */
  ESTK_CM_CALL_LIST_CMD,                   /* CM call List svent */
  ESTK_BIP_SOCK_EVT_CMD,                   /* Socket event from DS */
  ESTK_BIP_PROCESS_NET_CB_CMD,             /* net callback processing command */
  ESTK_BIP_KEEP_CH_OPEN_TIMER_EXP_CMD,     /* keep_ch_open timer expired command */
  ESTK_BIP_CELLULAR_OOS_TIMER_EXP_CMD,     /* Cellular OOS timer expiry command */
  ESTK_BIP_CELLULAR_STATUS_CHANGE_EVT_CMD, /* Cellular Service Status Change
                                              indication from GSTK */
  ESTK_BIP_WLAN_STATUS_CHANGE_EVT_CMD,     /* WLAN Status Change indication
                                              from GSTK */
  ESTK_BIP_WLAN_IP_ADDR_CHANGE_EVT_CMD,    /* WLAN IP Address change command */
  ESTK_SETUP_MENU_REQ_CMD,                 /* SETUP MENU proactive command */
  ESTK_MENU_SEL_REQ_CMD,                   /* MENU SELECTION envelope command */
  ESTK_ICON_DISPLAY_CONF_CMD,              /* Icon confirmation command */
  ESTK_GENERAL_USER_CNF_CMD,               /* General user conf command */
  ESTK_SCWS_CH_STATUS_REQ_CMD,             /* Ch Status udate for SCWS */
  ESTK_SCWS_DATA_AVAIL_REQ_CMD,            /* Data available req command for SCWS */
  ESTK_SCWS_OPEN_CH_RSP_CMD,               /* Open channel respones for SCWS */
  ESTK_SCWS_CLOSE_CH_RSP_CMD,              /* Close channel response for SCWS */
  ESTK_SCWS_SEND_DATA_RSP_CMD,             /* Send Data response for SCWS */
  ESTK_CM_SUPS_EVENT_CMD,                  /* CM Sups event */
  ESTK_CM_INBAND_EVENT_CMD,                /* Process Inband events from CM */
  ESTK_REFRESH_CNF_CMD,                    /* Refresh confirmation from MMGSDI */
  ESTK_WMS_MSG_SUBMIT_RPT_EVT_CMD,         /* Process WMS Message Submit Report event*/
  ESTK_WMS_CFG_EVT_CMD,                /* Process WMS Configuration event*/
  ESTK_USER_OR_DISP_CONF_TIMEOUT_CMD,      /* User confirmation/
                                              Display Alpha confirmation timeout*/
  ESTK_MAX_CMD = 0x7FFFFFFF
}
estk_cmd_enum_type;

/*===========================================================================

  STRUCT: ESTK_CM_CALL_EVENT_ENUM_TYPE

  PARAMETERS:
    ESTK_CM_CALL_NONE_EVT    : Invalid call event
    ESTK_CM_CALL_ORIG_EVT    : Call origination event
    ESTK_CM_CALL_CONNECT_EVT : Call connected event
    ESTK_CM_CALL_END_EVT     : Call end event
    ESTK_CM_CALL_EVENT_MNG_CALLS_CONF_EVT: Event for call holds
    ESTK_CM_CALL_MOD_TO_SS_EVT : Call modified event

  PURPOSE:
    The enumeration indicates the various CM call events.
===========================================================================*/
typedef enum
{
  ESTK_CM_CALL_NONE_EVT = 0x00,
  ESTK_CM_CALL_ORIG_EVT,
  ESTK_CM_CALL_CONNECT_EVT,
  ESTK_CM_CALL_END_EVT,
  ESTK_CM_CALL_EVENT_MNG_CALLS_CONF_EVT,
  ESTK_CM_CALL_MOD_TO_SS_EVT,
  ESTK_CM_CALL_MAX_EVT = 0x7FFFFFFF
}
estk_cm_call_event_enum_type;

/*===========================================================================

  STRUCT: ESTK_CM_SUPPS_SS_OPERATION_ENUM_TYPE

  PARAMETERS:
    ESTK_NULL_SS_OPERATION : NULL
    ESTK_REGISTER_SS       : Register operation
    ESTK_ERASE_SS          : Erase operation
    ESTK_ACTIVATE_SS       : Activate operation
    ESTK_DEACTIVATE_SS     : Deactivate operation
    ESTK_INTERROGATE_SS    : Interrogate operation
    ESTK_PROCESS_USSD_DATA : Process USSD

  PURPOSE:
    The enumeration indicates the various supplementarty services operations
===========================================================================*/
typedef enum
{
  ESTK_NULL_SS_OPERATION,
  ESTK_REGISTER_SS,
  ESTK_ERASE_SS,
  ESTK_ACTIVATE_SS,
  ESTK_DEACTIVATE_SS,
  ESTK_INTERROGATE_SS,
  ESTK_PROCESS_USSD_DATA,
  ESTK_MAX_SS_OPERATION =  0x7FFFFFFF
}
estk_sups_ss_operation_enum_type;

typedef enum
{
  ESTK_DS_WRITE_EVENT = 0x01,
  ESTK_DS_READ_EVENT  = 0x02,
  ESTK_DS_CLOSE_EVENT = 0x04,
  ESTK_DS_MAX_EVENT   = 0xFF
}
estk_ds_sock_event_enum_type;

typedef enum
{
  ESTK_CALL_TYPE_VOICE=0,
  ESTK_CALL_TYPE_CS_DATA=1,
  ESTK_CALL_TYPE_PS_DATA=2,
  ESTK_CALL_TYPE_SMS=3,
  ESTK_CALL_TYPE_PD=4,
  ESTK_CALL_TYPE_TEST=5,
  ESTK_CALL_TYPE_OTAPA=6,
  ESTK_CALL_TYPE_STD_OTASP=7,
  ESTK_CALL_TYPE_NON_STD_OTASP=8,
  ESTK_CALL_TYPE_EMERGENCY=9,
  ESTK_CALL_TYPE_SUPS=10,
  ESTK_CALL_TYPE_VT=11,
  ESTK_CALL_TYPE_VT_LOOPBACK,
  ESTK_CALL_TYPE_VS,
  ESTK_CALL_TYPE_PS_DATA_IS707B,
  ESTK_CALL_TYPE_UNKNOWN,
}
estk_cm_call_type_enum_type;

/*--------------------------------------------------------------------------
                             STRUCTURE DEFINITIONS
---------------------------------------------------------------------------*/
/*===========================================================================

  STRUCT: ESTK_GENERIC_DATA_TYPE

  PARAMETERS:
    len       : length of the buffer data_ptr
    data_ptr   : Pointer to data

  PURPOSE:
    The structure is to hold generic data
==========================================================================*/
typedef struct
{
  uint32  len;
  uint8  *data_ptr;
}
estk_generic_data_type;

/*===========================================================================

  STRUCT: ESTK_HEADER_TYPE

  PARAMETERS:
    cmd_ref_id      : GSTK command reference id
    cli_user_data   : client user data
    cmd             : ESTK command data
    command_number  : only valid for proactive command

  PURPOSE:
    The structure contains the header information for an estk command
==========================================================================*/
typedef struct
{
  gstk_slot_id_enum_type    sim_slot_id;
  uint32                    cmd_ref_id;
  uint32                    cli_user_data;
  estk_cmd_enum_type        cmd;
  uint8                     command_number;
}
estk_hdr_type;

/*===========================================================================

  STRUCT: ESTK_USER_RSP_TYPE

  PARAMETERS:
    yes_no_rsp      : YES(1) or NO(0) response from user
    icon_disp       : YES(1) or NO(0) response from UI

  PURPOSE:
    The structure contains the data pertaining toyes/no response from the
    user
==========================================================================*/
typedef struct
{
  boolean  yes_no_rsp;
  boolean   icon_disp;
}
estk_user_rsp_type;

/*===========================================================================

  STRUCT: ESTK_CM_CALL_EVENT_TYPE

  PARAMETERS:
    evt_id      : CM call event type
    evt_payload : CM call event payload
    call_id     : CM call id

  PURPOSE:
    The structure contains cm call event information
==========================================================================*/
typedef struct
{
  estk_cm_call_event_enum_type   evt_id;
  estk_generic_data_type         evt_payload;
  uint8                          call_id;
  cm_result_from_cc_s_type       result_from_cc;
  estk_cm_call_type_enum_type    call_type;
}
estk_cm_call_event_type;

/*===========================================================================

  STRUCT: ESTK_CALL_STATE_TYPE

  PARAMETERS:
    call_id        : CM call id
    call_sys_mode  : CM sys mode

  PURPOSE:
    The structure contains information for each active call from
    cm call list cb
==========================================================================*/
typedef struct
{
  cm_call_id_type      call_id;
  sys_sys_mode_e_type  call_sys_mode;
}
estk_call_state_type;

/*===========================================================================

  STRUCT: ESTK_CM_CALL_LIST_TYPE

  PARAMETERS:
    number_of_active_calls : number of active calls from CM call list
    active_calls           : information for each active call

  PURPOSE:
    The structure contains cm call list information from cm call list cb
==========================================================================*/
typedef struct
{
  uint8                 number_of_active_calls;
  estk_call_state_type  active_calls[CM_CALL_ID_MAX];
}
estk_cm_call_list_type;

/*===========================================================================

  STRUCT: ESTK_MMGSDI_REFRESH_CNF_TYPE

  PARAMETERS:
    refresh_result       : Refresh result

  PURPOSE:
    The structure contains cm call event information
==========================================================================*/
typedef struct
{
  gstk_general_result_enum_type  refresh_result;
    /**< General result. */
  gstk_additional_info_ptr_type      *additional_result_ptr;
    /**< Additional result. */
  gstk_terminal_rsp_extra_param_type *other_info_ptr;
    /**< Other info. */
}
estk_mmgsdi_refresh_cnf_type;

/*===========================================================================

  STRUCT: ESTK_MENU_SELECTION_REQ_TYPE

  PARAMETERS:
    chosen_item_id: item id to be chosen
    request_help  : indicator to help request

  PURPOSE:
    The structure contains cm call event information
==========================================================================*/
typedef struct
{
   uint8                              chosen_item_id;
   /* Optional */
   boolean                            request_help;
}
estk_menu_selection_req_type;

/*===========================================================================

  STRUCT: ESTK_SOCK_EVENT_TYPE

  PARAMETERS:
    evt_type    : type of socket event from DS
    sock_fd     : socket descriptor

  PURPOSE:
    The structure contains sock event information
==========================================================================*/
typedef struct
{
  uint32                       evt_type;
  sint15                       sock_fd;
}
estk_sock_event_type;

/*===========================================================================

  STRUCT: ESTK_PROCESS_NET_CB_TYPE

  PARAMETERS:
    dss_nethandle: DS-returned nethandle
    iface_id     : DS-returned iface_id (that matches with nethandle)
    error_num    : DS-returned error_num
  PURPOSE:
    The structure contains sock event information
==========================================================================*/
typedef struct
{
  sint15             dss_nethandle;
  dss_iface_id_type  iface_id;
  sint15             error_num;
}
estk_process_net_cb_type;

/*===========================================================================

  STRUCT: estk_scws_ch_status_req_type

  PARAMETERS:
    ch_id         : channel id
    ch_status     : channel status

  PURPOSE:
    The structure contains ch status information
==========================================================================*/
typedef struct
{
  uint32                       ch_id;
  gstk_scws_ch_state_enum_type ch_status;
}
estk_scws_ch_status_req_type;


/*===========================================================================

  STRUCT: estk_scws_data_avail_req_type

  PARAMETERS:
    ch_id            : channel id
    data_payload     : data payload

  PURPOSE:
    The structure contains scws data available information
==========================================================================*/
typedef struct
{
  uint32                            ch_id;
  estk_generic_data_type            data_payload;
}
estk_scws_data_avail_req_type;

/*===========================================================================

  STRUCT: estk_scws_open_ch_rsp_type

  PARAMETERS:
    ch_id         : channel id
    ch_status     : channel status

  PURPOSE:
    The structure contains scws open channel information
==========================================================================*/
typedef struct
{
  uint32                            ch_id;
  gstk_scws_ch_state_enum_type      ch_status;
}
estk_scws_open_ch_rsp_type;

/*===========================================================================

  STRUCT: estk_scws_close_ch_rsp_type

  PARAMETERS:
    ch_id         : channel id
    ch_status     : channel status

  PURPOSE:
    The structure contains scws close channel information
==========================================================================*/
typedef struct
{
  uint32                            ch_id;
  gstk_scws_ch_state_enum_type      ch_status;
}
estk_scws_close_ch_rsp_type;

/*===========================================================================

  STRUCT: estk_scws_send_data_rsp_type

  PARAMETERS:
    ch_id          : channel id
    is_success     : status of send data

  PURPOSE:
    The structure contains sock event information
==========================================================================*/
typedef struct
{
  uint32                             ch_id;
  boolean                            is_success;
}
estk_scws_send_data_rsp_type;

/*===========================================================================

  STRUCT: ESTK_SUPS_INFO_TYPE

  PARAMETERS:
    evt_id      : CM SUPS event type

  PURPOSE:
    The structure contains cm call event information
==========================================================================*/
typedef struct
{
  cm_sups_event_e_type         evt_id;
  cm_sups_info_s_type          evt_info;
}
estk_cm_sups_info_type;
/*===========================================================================

  STRUCT: ESTK_CM_INBAND_EVENT_TYPE

  PARAMETERS:
    event_type - Inband event type
    event_info - Inband event info
  time       - Time when the inband event occured

  PURPOSE:
    The structure contains the data pertaining to inband event
==========================================================================*/
typedef struct
{
  cm_inband_event_e_type  event_type;
  cm_call_id_type         call_id;
  time_type               time;
}
estk_cm_inband_event_type;

/*===========================================================================

  STRUCT: ESTK_CELLULAR_SERVICE_STATUS_CHANGE_EVENT_TYPE

  PARAMETERS:
    cellular_service_available : status of Cellular Service availability

  PURPOSE:
    The structure contains Cellular Service availability information
==========================================================================*/
typedef struct
{
  boolean    cellular_service_available;
}
estk_cellular_service_status_change_event_type;

/*===========================================================================

  STRUCT: ESTK_WLAN_STATUS_CHANGE_EVENT_TYPE

  PARAMETERS:
    wlan_available    : status of WLAN availability

  PURPOSE:
    The structure contains WLAN availability information
==========================================================================*/
typedef struct
{
  boolean    wlan_available;
}
estk_wlan_status_change_event_type;

/*===========================================================================

  STRUCT: ESTK_WMS_CFG_EVT_TYPE

  PARAMETERS:
    wms_cfg_event  - WMS READY or NOT READY event
    sys_type       - system GW/CDMA

  PURPOSE:
    The structure contains the data pertaining to
    the WMS of particular system is ready to handle SMS or not
==========================================================================*/
typedef struct
{
  wms_cfg_event_e_type wms_cfg_event;
  wms_cfg_sys_e_type   sys_type;
}
estk_wms_cfg_evt_type;

/*===========================================================================

  STRUCT: ESTK_WMS_MSG_SUBMIT_RPT_EVT_TYPE

  PARAMETERS:
    report_status - Report Status
    cause_info    - Cause info Report Status

  PURPOSE:
    The structure contains the data pertaining to
    the WMS message submit report event
==========================================================================*/
typedef struct
{
  wms_report_status_e_type report_status;
  wms_cause_info_s_type    cause_info;
}
estk_wms_msg_submit_rpt_evt_type;

/*===========================================================================

  STRUCT: ESTK_CMD_TYPE

  PARAMETERS:
    link                    : CM call event type
    hdr                     : CM call event payload
    result                  : CM call id
    call_event_info         : CM call event info
    payload                 : payload information.Contents depend on the context.
    user_rsp                : User confirmation response
    sock_evt                : Socket event
    setup_menu_req          : SETUP MENU proactive command
    menu_sel_req            : MENU SELECTION envelope command
    scws_ch_status_req      : Channel status for SCWS
    scws_data_avail_req     : Data Available for SCWS
    scws_open_ch_rsp_type   : SCWS Open Ch response
    scws_close_ch_rsp_type  : SCWS Close Ch response
    scws_send_data_rsp_type : SCWS Send Data response
    sups_info               : SUPS info
    estk_inband_event_type  : Inband_event
    refresh_cnf             : MMGSDI Refresh CNF
    cellular_status_evt     : Cellular status event
    wlan_status_evt         : WLAN status event
    net_cb_info             : DS Net callback processing command
    wms_msg_sumbit_rpt_evt  : WMS Message Sumbit Report event
  PURPOSE:
    The structure contains information pertaining to an estk command
==========================================================================*/
typedef struct
{
  q_link_type                                      link; /* Queue field */
  estk_hdr_type                                    hdr;
  estk_result_enum_type                            result;// need to evaluate
  gstk_evt_cb_funct_type                           estk_raw_evt_cb;
  union
  {
    estk_cm_call_event_type                        call_event_info;
    estk_user_rsp_type                             user_rsp;
    estk_sock_event_type                           sock_evt;
    gstk_setup_menu_req_type                       setup_menu_req;
    estk_menu_selection_req_type                   menu_sel_req;
    estk_scws_ch_status_req_type                   scws_ch_status_req;
    estk_scws_data_avail_req_type                  scws_data_avail_req;
    estk_scws_open_ch_rsp_type                     scws_open_ch_rsp;
    estk_scws_close_ch_rsp_type                    scws_close_ch_rsp;
    estk_scws_send_data_rsp_type                   scws_send_data_rsp;
    estk_cm_sups_info_type                         sups_info;
    estk_cm_inband_event_type                      inband_event;
    estk_mmgsdi_refresh_cnf_type                   refresh_cnf;
    estk_cellular_service_status_change_event_type cellular_status_evt;
    estk_wlan_status_change_event_type             wlan_status_evt;
    estk_process_net_cb_type                       net_cb_info;
    estk_cm_call_list_type                         call_list_info;
    estk_wms_msg_submit_rpt_evt_type               wms_msg_sumbit_rpt_evt;
    estk_wms_cfg_evt_type                          wms_cfg_evt;
  }cmd_data;
} estk_cmd_type;

/*--------------------------------------------------------------------------
                             FUNCTION PROTOTYPES
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION: estk_init

DESCRIPTION:
  This ESTK Initialization functon includes initializing the ESTK queue and
  initializing estk_shared_info global variable.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_init( void );


/*===========================================================================

FUNCTION: estk_gstk_init

DESCRIPTION:
  This ESTK Initialization functon includes memory allocation for instance
  specific data, initializing the requesting GSTK for client id to assist in
  toolkit command processing and initializing estk timers.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_gstk_init(uint8 gstk_num_of_instances);


/*===========================================================================

FUNCTION: estk_cm_init

DESCRIPTION:
  This ESTK Initialization functon includes CM client registeration, call,
  dtmf and sups events registration, timers and NV related to CALL events and
  WMS registration.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_init( void );

/*===========================================================================

FUNCTION: estk_get_cmd_buf

DESCRIPTION:
  This function allocates a command buffer from the ESTK free memory queue.

DEPENDENCIES:
  None

RETURN VALUE:
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_cmd_type*  estk_get_cmd_buf( void );

/*===========================================================================

FUNCTION: estk_put_cmd

DESCRIPTION:
  This function puts a command on the ESTK command queue, and
  sets the GSTK_ESTK_SIG.

DEPENDENCIES:
  None

COMMENTS:
  The caller of this function should have previously allocated a command
  buffer by calling estk_get_cmd_buf().

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_put_cmd(estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_cmd

DESCRIPTION:
  Called  from GSTK when ESTK signal is found to be set.

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  ESTK will build an appropriate terminal response after processing the
  proactive command

===========================================================================*/
gstk_status_enum_type estk_process_cmd(void);

/*===========================================================================
FUNCTION: estk_is_in_legacy_mode

DESCRIPTION:
  Check if ESTK is in legacy mdoe. If there is no client registering the display
  functionality of the original command, ESTK will try to send confirmation
  alpha in GET INKEY command and display alpha in DISPLAY TEXT command. Once the
  alpha has been sent successfully, ESTK will be in legacy mode.

  *NOTE: once this funciton is called, ESTK will exit legacy mode.

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  true if ESTK is in legacy mdoem.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_is_in_legacy_mode(void);

/*===========================================================================
FUNCTION: estk_clean_up_on_gstk_timer_expiry

DESCRIPTION:
  This function cleans up estk internals in the scenario where gstk timer
  expires e.g. for a proactive command. The function is directly called
  by gstk in as part of the gstk_uim_terminal_response_report in the cb
  context.

PARAMETERS:
  slot_id: [Input] Slot Id for which the timer expiry is applicable

DEPENDENCIES:
  None

RETURN VALUE:
 gstk_status_enum_type
   GSTK_SUCCESS
   GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
gstk_status_enum_type estk_clean_up_on_gstk_timer_expiry(
  gstk_slot_id_enum_type    slot_id);

/*===========================================================================
FUNCTION: estk_uim_is_bip_session_in_progress

DESCRIPTION:
  Return status indicating any BIP session is in progress on the slot requested

PARAMETERS:
  slot_id : [Input] slot on which the BIP session status is requested

DEPENDENCIES:
  None

RETURN VALUE:
  Return
  TRUE:  BIP session in progress
  FALSE: No BIP session in progress

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_uim_is_bip_session_in_progress(
  gstk_slot_id_enum_type slot_id
);

/*===========================================================================
FUNCTION: estk_reset_wlan_unusable_status

DESCRIPTION:
  Clears the WLAN unusable status in ESTK (w.r.t. BIP transactions)

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_reset_wlan_unusable_status(void);

/*===========================================================================
FUNCTION: estk_scws_clear_send_receive_buf

DESCRIPTION:
  Clear the Tx/Rx buffers

PARAMETERS:
  Channel id

DEPENDENCIES:
  None

RETURN VALUE:
  Return
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_send_receive_buf (
  uint8 ch_id);

/*===========================================================================
FUNCTION: estk_scws_clear_receive_data_allowed

DESCRIPTION :
  This function sets receive data allowed to true

PARAMETERS:
  ch_id   [Output]: Channel ID
  slot_id [Output]: Slot ID

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_receive_data_allowed(
  uint8                  ch_id,
  gstk_slot_id_enum_type slot_id);

/*===========================================================================

FUNCTION: estk_get_terminal_profile_value_list

DESCRIPTION:
  Retreive ESTK's terminal profile list.

PARAMETERS:
  gstk_profile_dl_support_ext_type  [Output]: estk profile value list
  num_of_tps_ptr                    [Output]: no. of profile updates

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_BAD_PARAM

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
gstk_status_enum_type estk_get_terminal_profile_value_list
(
  gstk_profile_dl_support_ext_type **estk_gstk_profile_value_list,
  uint8                             *num_of_tps_ptr
);

/*===========================================================================

FUNCTION: estk_stop

DESCRIPTION:
  This function is used to free the estk private resources.
  This is called when STK is powering down.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void estk_stop(void);

/*===========================================================================

FUNCTION: estk_notify_proactive_command

DESCRIPTION :
  This function allows GSTK to notify ESTK whenever a proactive command is
  received from the card

PARAMETERS:
  type_of_command : [Input] Type of proactive command
  slot_id         : [Input] Slot ID on which the proactive command is received

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void estk_notify_proactive_command
(
  uint8                  type_of_command,
  gstk_slot_id_enum_type slot_id
);

#endif /* FEATURE_ESTK */

#endif /* ESTK_H */

