#ifndef ESTK_PRIV_H
#define ESTK_PRIV_H

/*======================================================
FILE:  estk_priv.h

GENERAL DESCRIPTION:
    ESTK private header include file

        Copyright © 2008-2016 QUALCOMM Technologies, Inc(QTI).
        All Rights Reserved.
        QUALCOMM Technologies Proprietary
=====================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_priv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
06/15/15   vr      Remove TR delay logic for CLOSE CHANNEL
01/18/14   vr      estk_cm_cs_call_connected changed as counter
12/16/14   vr      Check WMS ready before sending SMS to WMS
11/06/14   shr     Fixed compilation error
09/24/14   dy      Use ephemeral ports for BIP session
08/26/14   gs      Disable the ESTK Specific Proacitve Commands based on the NV set
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
05/29/14   shr     Cycle through source ports for consecutive BIP sessions
05/27/14   dy      Remove usage of gstk_util_get_curr_access_tech() in ESTK
05/06/14   gs      Removed the ESTK featurization
03/03/14   shr     ESTK BIP clean-up and enhancements
01/30/14   vr      SIM initiated call control handling
01/23/14   am      Corrected MSG_ERROR with UIM_LEGACY_MACRO
01/09/14   kb      Added ESTK_RETURN_IF_NULL_PTR macro
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
09/13/13   gm      Support for Recovery and hot-swap
08/22/13   vr      Klocwork fixes
07/15/13   gs      Remove NV 5770 dependency for STK related tests except for GSM and DTM Support
06/20/13   vr      Triton TSTS changes
05/24/13   hn      Does APN mapping based on NV item
06/25/12   dd      Read CS_PS NV item at init
03/28/12   av      Replaced feature flag with NVITEM
12/23/11   bd      Support for FR 1479
11/29/11   dd      Additional info in TR if hold active calls fails
08/22/11   sg      Clean up after no_sim state
06/15/11   ea      Fix when STK Redial Duration TLV is absent
04/28/11   nk      Removed tmc.h include to fix warning as it is not used
04/19/11   nb      Added a variable to track connecetd call
03/31/11   ea      Add STK redial support
03/28/11   dd      Wait for icon_disp_rsp for network commands
11/18/10   sg      Add timer for delaying the TR
11/30/10   yt      Including private GSTK header
11/10/10   yt      Memory reduction changes
10/01/10   dd      Add filtering APIs
09/23/10   dd      Add support for raw BIP alpha
07/19/10   dd      Add tag to the raw and decoded hdr for Set Up Call
07/16/10   sg      Fix compilation error
07/07/10   sg      Fix Lint errors
06/30/10   dd      Icon display confirmation
02/15/10   nb      Moved Dual Slot Chnages
12/17/09   sg      Fix compilation errors
06/01/09   xz      Support removal of CM_MAX_NUMBER_CHARS
12/17/08   sk      Fixed lint error
09/16/08   sk      Added lint fixes
09/08/08   tml     Fixed auto header version generation
09/02/08   sk      Initial Creation.
=============================================================================*/

/*--------------------------------------------------------------------------
                             COMMON DEFINTION HEADER FILES
---------------------------------------------------------------------------*/
#include "uim_variation.h"
#include "customer.h"
#ifdef FEATURE_ESTK
#include "comdef.h"
#include "task.h"
#include "estk.h"
#include "cm.h"
#include "err.h"

/*--------------------------------------------------------------------------
                             INTERFACE HEADER FILES
---------------------------------------------------------------------------*/
#include "wms.h"
#include "ps_in.h"
#include "gstk_nv_p.h"

#include "dssdns.h"
#include "dssocket.h"
#include "dss_netpolicy.h"

/*--------------------------------------------------------------------------
                             MACRO DEFINTIONS
---------------------------------------------------------------------------*/
#if defined(CM_MAX_NUMBER_CHARS)
  #define ESTK_MAX_DIALLED_STRING CM_MAX_NUMBER_CHARS
#elif defined(CM_MAX_CALLED_INFO_CHARS_UMTS)
  #define ESTK_MAX_DIALLED_STRING CM_MAX_CALLED_INFO_CHARS_UMTS
#else
  #error "Neither CM_MAX_CALLED_INFO_CHARS_UMTS nor CM_MAX_NUMBER_CHARS is defined"
#endif /* CM_MAX_NUMBER_CHARS */

#define ESTK_MAX_BIP_CH                        7

#define ESTK_CALL_ID_MAX                       CM_CALL_ID_MAX
#define ESTK_USER_CONF_OR_ALPHA_DISP_TIMEOUT   30000

#define ESTK_IS_VALID_SLOT_ID(x)                                \
          ((gstk_slot_id_enum_type)(x) > GSTK_SLOT_AUTO &&      \
           (uint32)(x) <= estk_num_of_instances         &&      \
           (uint32)(x) <= GSTK_MAX_NUM_OF_INSTANCES)

#define ESTK_RETURN_IF_NULL_PTR(x)                              \
          if((x) == NULL)                                       \
          {                                                     \
            return;                                             \
          }

#define ESTK_RETURN_ERROR_IF_NULL_PTR(x, y)                     \
          if((x) == NULL)                                       \
          {                                                     \
            return y;                                           \
          }

/*--------------------------------------------------------------------------
                             ENUMERATIONS
---------------------------------------------------------------------------*/
/*===========================================================================

  STRUCT: ESTK_STATE_ENUM_TYPE

  PARAMETERS:
    ESTK_NOT_INIT_ST : Uninitialized state
      1. state where ESTK can send down Reg info and TP dl to GSTK
      2. state where ESTK cannot process any other GSTK or client command that
         comes to the ESTK queue directly except for the TP dl response.
    ESTK_IDLE_ST : Idle state
      1. state where ESTK has done suucessful TP dl and Reg info to GSTK.
      2. state where ESTK is in Idle state with no command from GSTK to process
      3. state where ESTK is not awaitng any response from UI client.
      4. state where no Terminal Response is pending in ESTK.
    ESTK_PREPROCESS_COMMAND_ST : Preprocessing a proactive command
      1. state indicates that ESTK is preprocessing a proactive command
         Eg. Checking for the presence of other calls for a set up call
         proactive command
         OR
         Eg. Waiting for user confirmation to proceed with a set up call
         proactive command.
      2. This is an optional state and may not be applicable to commands like
         Send SMS, Send SS or Send USSD.
   ESTK_PENDING_TR_ST : Pending Terminal Response
      1. state indicates that ESTK has successfully requested for execution
         of the modem functionality such as placing a voice call, data call
         sending sms etc.
      2. state indicates that ESTK is awaiting notification to know about the
         status of the request.
   ESTK_WAIT_FOR_USER_CNF_ST : Pending user confirmation
      1. Waiting for user confirmation to proceed with a setup call proactive cmd
  ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST: Pending 2nd alpha display confirmation
      1. If display alpha is present, waits for the UI to let know if icon
       was displayed
  PURPOSE:
    The enumeration indicates the various ESTK states.
===========================================================================*/
typedef enum
{
  ESTK_NOT_INIT_ST = 0x00,
  ESTK_IDLE_ST,
  ESTK_PREPROCESS_COMMAND_ST,
  ESTK_PENDING_TR_ST,
  ESTK_WAIT_FOR_USER_CNF_ST,
  ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST,
  ESTK_MAX_STATE    = 0x7FFFFFFF
}
estk_state_enum_type;

typedef enum
{
  ESTK_REDIAL_LIST_REQ     = 0x00,
  ESTK_REDIAL_CALL_REQ     = 0x01,
  ESTK_REDIAL_MAX_STATE    = 0x7FFFFFFF
}
estk_redial_enum_type;

typedef enum
{
  ESTK_CH_CLOSED,
  ESTK_CH_PENDING,
  ESTK_CH_OPEN_WAIT,
  ESTK_CH_ACTIVE,
  ESTK_CH_CLOSE_IN_PROGRESS,
  ESTK_CH_DROPPED,
  ESTK_CH_FALLBACK_IN_PROGRESS,
  ESTK_CH_SCWS_OFFSET,
  ESTK_CH_SCWS_CLOSED = ESTK_CH_SCWS_OFFSET,
  ESTK_CH_SCWS_LISTEN,
  ESTK_CH_SCWS_ESTABLISHED
}estk_channel_status_type;

typedef enum {
  ESTK_NETWORK_NONE        = 0x00,
  ESTK_NETWORK_CELLULAR    = 0x01,
  ESTK_NETWORK_WLAN        = 0x02,
  ESTK_NETWORK_MAX         = 0xFFFF
}estk_network_type;

/*--------------------------------------------------------------------------
                             STRUCTURE DEFINITIONS
---------------------------------------------------------------------------*/

/*===========================================================================

  STRUCT: ESTK_ACTIVE_CALL_LIST_FROM_CM

  PARAMETERS:
    call_id        : The Call ID retrieved from CM
    call_active    : Is the call active or not
    call_sys_mode  : CM sys mode for call with call_id

  PURPOSE:
    The structure is used to hold the call list information retrieved from
    CM.
===========================================================================*/
typedef struct
{
  uint8                call_id;
  boolean              call_active;
  sys_sys_mode_e_type  call_sys_mode;
}
estk_call_list_from_cm;

/*===========================================================================

  STRUCT: ESTK_CALL_INFO_TYPE

  PARAMETERS:
    dtmf_str_ptr         : DTMF digits string to which the Send DTMF
                           function traverse through and operated on.
                           This is a null terminated string.  Digits are
                           represented in ASCII and Pause is represented
                           as 0xFF
    dtmf_len             : The valid length of the dtmf_digits
    curr_dtmf_pos_in_str : The current position in the dtmf_str_ptr that
                           ESTK has sent to CM
    start_dtmf           : Indicate whether the next action on the single
                           DTMF digit is "start" or "stop"
    call_list            : Contains the list call returned by CM
    call_id              : Call ID to which the GSTK setup call is
                           associated with.  The value is returned by CM
                           during call origination
    hold_active_failed   : Keeps track whether the hold active call failed
    call_sys_mode        : CM sys mode for call with call_id

  PURPOSE:
    The structure is used to hold the call and dtmf related information
===========================================================================*/
typedef struct
{
  char                       *dtmf_str_ptr;
  char                       *temp_DTMF_str_ptr;
  int32                       dtmf_len;
  int32                       curr_dtmf_pos_in_str;
  boolean                     start_dtmf;
  estk_call_list_from_cm      call_list[ESTK_CALL_ID_MAX];
  uint8                       call_id;
  boolean                     hold_active_failed;
  sys_sys_mode_e_type         call_sys_mode;
}
estk_call_info_type;

/*===========================================================================

  STRUCT: ESTK_SUPS_INFO_TYPE

  PARAMETERS:
    req_id       : The Supplementary Services or USSD request ID.  This is
                   assigned by ESTK and passes into CM sups APIs for the SS
                   or USSD related operations.  CM will return this value
                   in the ss_ref in its command callback or event callback.
    cmd_ref_id   : Command reference table ID into the GSTK command
                   reference table
    ss_operation : Type of SS operation

  PURPOSE:
    The structure is used to hold the ss and ussd related information
===========================================================================*/
typedef struct
{
  uint8                            req_id;
  uint32                           cmd_ref_id;
  estk_sups_ss_operation_enum_type ss_operation;
}
estk_sups_info_type;

/*===========================================================================

  STRUCT: ESTK_WMS_MSG_CFG_TYPE

  PARAMETERS:
    wms_ready_gw              : WMS ready for GW
    wms_ready_cdma            : WMS ready for CDMA
    wms_msg                   : cache wms_msg if WMS is not ready

  PURPOSE:
    The structure is used to cache the WMS msg and cfg
===========================================================================*/
typedef struct
{
  boolean                            wms_ready_gw;
  boolean                            wms_ready_cdma;
  wms_client_message_s_type         *wms_msg_ptr;
}
estk_wms_msg_cfg_type;

/*===========================================================================

  STRUCT: ESTK_CHANNEL_TYPE

  PURPOSE:
    The structure is used to hold info on the BIP channel opened
===========================================================================*/

typedef struct
{
  estk_channel_status_type                  status;
  int32                                     channel_id;
  gstk_pkt_data_transport_level_enum_type   transport_protocol;
  estk_network_type                         nw_type;
  boolean                                   first_dss_sendto_done;
  boolean                                   receive_data;
  uint16                                    port_number;
  uint16                                    me_port_num;
  int32                                     buffer_size;
  boolean                                   open_ch_param_modified;
  boolean                                   on_demand_link;
  int32                                     index;
  gstk_bearer_description_type              bearer_desc;
  uint8                                     *recv_buffer;
  uint8                                     *send_buffer;
  uint16                                    send_buf_len;
  int32                                     recv_buf_len;
  struct ps_sockaddr_storage                *src_addr;
  struct ps_sockaddr_storage                *dest_addr;
  uint32                                    apn_len;
  uint8                                     *apn;
  uint32                                    username_len;
  char                                     *username;
  uint32                                    passwd_len;
  char                                     *passwd;
  sint15                                    sock_fd;
  sint15                                    sock_nethandle;
  int                                       iface_id;
  sint15                                    ds_netcb_status_v4;
  sint15                                    ds_netcb_status_v6;
  sint15                                    sock_nethandle_v4;
  sint15                                    sock_nethandle_v6;
  dss_net_policy_info_type                  policy_info;
  int16                                     profile_3gpp;
  int16                                     profile_3gpp2;
  rex_timer_type                            *keep_ch_open_timer_p;
  boolean                                   send_data_timer_expired;
  boolean                                   card_has_auth_params;
  gstk_open_ch_req_type                     open_ch_cmd;
  gstk_cmd_enum_type                        current_command_id;
  boolean                                   is_scws_ch;
  uint8                                     *scws_overflow_buf;
  uint32                                    scws_overflow_buf_len;
  boolean                                   receive_data_allowed;
}estk_channel_type;

/*===========================================================================
  STRUCT: ESTK_GSTK_PROAC_CMD_LIST

  PARAMETERS:
    estk_gstk_reg_list_ptr     :  Contains the list of ESTK Proactive command entries to be registred with GSTK
    estk_gstk_reg_list_num   :  Maintains the number of entries in the estk_gstk_reg_list
===========================================================================*/
typedef struct
{
  gstk_toolkit_cmd_reg_info_type *estk_gstk_reg_list_ptr;
  uint32                          estk_gstk_reg_list_num;
}estk_gstk_proac_cmd_list;

/*===========================================================================

  STRUCT: ESTK_PRIV_INFO_TYPE

  PARAMETERS:
    gstk_client_id         : Client ID to be used with any GSTK APIs
    slot_id                   : Slot ID corresponding to the private
                                data instance
    wms_client_id          : Client ID to be used with any WMS APIs
    cm_client_id           : Client ID to be used with any CM APIs
    curr_cmd                  : The current proactive command being processed
                                by ESTK. Only the header and command data
                                that is deemed relevant for subsequent
                                processing will be saved
    state                  : The current ESTK state
    call_info                 : Additional information required for Setup call
                                and Send DTMF proactive commands handling
    sups_info_ptr             : Additional information required for Send SS
                                and Send USSD proactive commands handling
    icon_present              : Whether the proactive command being processed
                                contains any icons or not
    icon_disp              : Whether icon was displayed by the UI
    call_icon_present      : Whether second alpha is present
    result                 : Used for SETUP CALL changing to USSD
    additional_result      : Additional result for Send SS
    extra_param            : Extra param for Send USSD
    estk_cm_cs_call_connected : Update when a CS Service is connected and ended
    cellular_oos_timer        : Timer to wait for Cellular OOS
    curr_bip_cmd_index        : Channel index of the current BIP command
    bip_channel_table         : BIP channel info
    dns_addrs              : DNS addresses retrieved from DS
    dns_addrs_len          : number of DNS addresses saved
    keep_ch_open              : Whether we have a Channel kept open from
                                previous session
    ch_open_id             : Channel ID for channel that was kept open
    prev_open_ch           : cache of previous OPEN CH
    wms_msg_cfg               : Cache WMS MSG and WMS CFG

  PURPOSE:
    The structure is used to hold all the private data for ESTK
===========================================================================*/
typedef struct
{
  gstk_client_id_type                gstk_client_id;
  gstk_slot_id_enum_type             slot_id;
  gstk_cmd_from_card_type            curr_cmd;
  estk_state_enum_type               state;
  boolean                            icon_present;  /* User confirmation icon */
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  gstk_setup_menu_req_type           setup_menu_cmd;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
  boolean                            icon_disp;
  boolean                            call_icon_present; /* Call display icon */
  gstk_general_result_enum_type      result;
  gstk_additional_info_ptr_type      additional_result;
  gstk_terminal_rsp_extra_param_type extra_param;
  uint8                              estk_cm_cs_call_connected;
  rex_timer_type                     cellular_oos_timer;
  int8                               curr_bip_cmd_index;
  estk_channel_type                  bip_channel_table[ESTK_MAX_BIP_CH];
  ip_addr_type                       dns_addrs[PS_IFACE_NUM_DNS_ADDRS * 2];
  uint32                             dns_addrs_len;
  boolean                            keep_ch_open;
  int32                              ch_open_id;
  gstk_open_ch_req_type              prev_open_ch;
  boolean                            ss_transaction;
  boolean                            ussd_transaction;
  rex_timer_type                     user_or_disp_conf_timer;
  estk_wms_msg_cfg_type              wms_msg_cfg;
}
estk_priv_info_type;

typedef struct
{
  wms_client_id_type          wms_client_id;
  gstk_slot_id_enum_type      slot_owns_wms_cli;
  cm_client_id_type           cm_client_id;
  gstk_slot_id_enum_type      slot_owns_cm_cli;
  estk_call_info_type         call_info;
  estk_sups_info_type         *sups_info_ptr;
  boolean                     in_legacy_mode;
  boolean                     redial_timer_expired;     /* indicates if redial timer is not expired yet */
  boolean                     redial_necessary;         /* redial is present in proactive cmd */
  boolean                     redial_timeout_present;   /* timeout duration is present in proactive cmd */
  rex_timer_cnt_type          redial_length_ms;         /* timeout duration in ms */
  boolean                     redial_in_progress;       /* keeps track if there is a pending redial */
  estk_redial_enum_type       redial_delayed_cm_cmd;
  gstk_cmd_from_card_type     redial_cmd;
  rex_timer_type              redial_timer;
  rex_timer_type              cm_cmd_reattempt_timer;
  rex_timer_type              dtmf_timer;
  rex_timer_type              burst_dtmf_timer;
  boolean                     wlan_unusable;
  estk_gstk_proac_cmd_list    cmd_list;
}
estk_shared_info_type;

/* Internal ESTK related module info */
extern estk_priv_info_type *estk_instances_ptr[GSTK_MAX_NUM_OF_INSTANCES];
extern estk_priv_info_type *estk_curr_inst_ptr;
extern estk_shared_info_type estk_shared_info;

extern q_type              estk_cmd_q;       /* ESTK Task command queue */

/*--------------------------------------------------------------------------
                             FUNCTION PROTOTYPES
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION: estk_cache_additional_info

DESCRIPTION:
  ESTK caches the TR information to be sent when alpha display response
  is received.
  Cache terminal response result, additional info, and other info.

PARAMETERS:
  additional_information_ptr : [Input] Additional information which needs
                                       to be filled in estk_curr_inst_ptr

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cache_additional_info(
  gstk_additional_info_ptr_type *additional_information_ptr
);

/*===========================================================================

FUNCTION: estk_free_cmd_buf

DESCRIPTION:
  This function puts the command back to the ESTK free queue.

PARAMETERS:
  cmd_ptr : [INPUT] ESTK command pointer to be freed

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_free_cmd_buf( estk_cmd_type *cmd_ptr );

/*===========================================================================

FUNCTION estk_free_cmd_buf_data

DESCRIPTION:
  This function frees data allocated for the estk cmd buf pointer.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer whose data needs to be freed.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void  estk_free_cmd_buf_data( estk_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION: estk_send_terminal_response

DESCRIPTION:
  This function is served as a utility function to the estk client, so that
  the client can call this function in sending a terminal response to
  a proactive command.

PARAMETERS:
  cmd_detail_ref_id:      [Input] Specifies the reference ID for command details of
                                  the proactive command that the client is sending
                                  a terminal response to.  The reference id value
                                  is sent to the client earlier in the structure
                                  of the proactive command request, inside the
                                  gstk_exp_hdr_type
  terminal_response_type: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result:         [Input] Specifies the result of the proactive command
                                  operation
  additional_result_ptr:  [Input] Pointer specifies the optional additional result
                                  for the proactive command, the length field in
                                  the gstk_additonal_info_type specifies number of
                                  additional_info in the structure
  other_info_ptr:         [Input] A union pointer of various extra parameters for the
                                  various proactive command response, please refer to
                                  gstk_terminal_rsp_param_type for details

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS,
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_terminal_response (
  uint32                                      cmd_detail_ref_id,
  gstk_cmd_enum_type                          terminal_response_type,
  gstk_general_result_enum_type               general_result,
  gstk_additional_info_ptr_type               *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type          *other_info_ptr
);

/*===========================================================================

FUNCTION: estk_save_open_channel_cmd_details

DESCRIPTION:
  This function copies the OPEN CHANNEL command details from the input GSTK
  command to the destination.

PARAMETERS:
  gstk_req_ptr : [Input]  GSTK command pointer
  dst_ptr      : [Output] Destination pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS: If commmand could be copied successfully
    ESTK_ERROR  : If command could not be copiedd successfully

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_save_open_channel_cmd_details(
  const gstk_cmd_from_card_type *gstk_req_ptr,
  gstk_open_ch_req_type         *dst_ptr
);

/*===========================================================================

FUNCTION: estk_save_gstk_cmd_details

DESCRIPTION:
  This function will be called by ESTK to store the command details
  of the command received from GSTK. ESTK requires to store the
  command as GSTK releases the command memory when the ESTK callback
  function invloked by GSTK returns.

PARAMETERS:
  gstk_req_ptr[Input]: GSTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR:

COMMENTS:
  This function should only be called after estk_client_init has been called

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_save_gstk_cmd_details(
  const gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION estk_send_alpha_to_display_client

DESCRIPTION:
  This function searches for display client for a particular command.  Upon
  finding a client that has registered for it, it will send the command to the
  client.

PARAMETERS:
  gstk_req_ptr: [Input] proactive command in GSTK format
  display_alpha:[Input] indicate if the alpha is for display or confirmation

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS,
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called
  If this function is called as part of the event callback handling for the
  original proactive command, it should be called before the return of the
  callback function to avoid the gstk_req_ptr being freed by the gstk

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_alpha_to_display_client(
  gstk_cmd_from_card_type       *gstk_req_ptr,
  boolean                        display_alpha
);

/*===========================================================================
FUNCTION estk_send_alpha_to_raw_client

DESCRIPTION:
  This function searches for display client for a particular command.  Upon
  finding a client that has registered for it, it will send the command to the
  client.

PARAMETERS:
  gstk_req_ptr: [Input] proactive command in GSTK format
  display_alpha:[Input] indicate if the alpha is for display or confirmation

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS,
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called
  If this function is called as part of the event callback handling for the
  original proactive command, it should be called before the return of the
  callback function to avoid the gstk_req_ptr being freed by the gstk

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_alpha_to_raw_client(
  gstk_client_id_type               client_id,
  gstk_raw_cmd_from_card_type       *cmd_ptr
);
#endif /* FEATURE_ESTK */

#endif /* ESTK_PRIV_H */
