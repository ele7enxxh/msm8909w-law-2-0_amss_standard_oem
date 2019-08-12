/*===========================================================================
FILE:  estk.c


GENERAL DESCRIPTION:ENHANCED STK layer
  This file provides the functions required to support the Enhanced STK
  functionality

          Copyright © 2009 - 2016 QUALCOMM Technologies, Inc(QTI).
          All Rights Reserved.
          QUALCOMM Technologies Proprietary
=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/31/16   gs      Fix invalid dereference
05/30/16   gs      Duplicate F3 reduction
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/06/16   gs      F3 logging improvements
03/11/16   gs      Init and activation with CM for Supplementary services
06/15/15   vr      Remove TR delay logic for CLOSE CHANNEL
01/14/15   vr      Initialize call list info
12/16/14   vr      Check WMS ready before sending SMS to WMS
12/09/14   vr      Correctly handle ESTK clean-up
12/08/14   dy      Re checking in backed out CR 743973
11/11/14   shr     Fixed compilation warnings
11/07/14   dy      Backing out fix for CR 743973 temporarily
11/04/14   dy      Remove FEATURE_ESTK_WMS_INTERFACE
11/03/14   dy      Fix channel reuse variable incorrectly reset
10/27/14   bd      Fixed TR for SETUP CALL when user doesn't respond for CNF Alpha
10/08/14   gm      Remove GSTK registration with WMS if FEATURE_ESTK is enabled
09/19/14   vr      Check correct proactive command before ESTK extra param clean up
09/11/14   gs      Enable Refresh registration by default while registering to GSTK
09/09/14   vr      Reduce stack usage
08/26/14   vr      Fix memory leaks
08/26/14   gs      Disable the ESTK Specific Proacitve Commands based on the NV set
08/07/14   dy      Process WMS events in GSTK task context
07/28/14   gm      Proper free of memory allocated
07/23/14   vr      Cleanup GSTK_CLIENT_INIT_EXT function
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
06/20/14   gm      Dynamically disable TP related to LTE
06/03/14   hn      Fix prev_open_ch memory leak
05/27/14   dy      Remove usage of gstk_util_get_curr_access_tech() in ESTK
05/13/14   gs      Enable ESTK sub features by default
05/06/14   gs      Removed the ESTK featurization
05/06/14   vr      Send send_dtmf alpha after preprocessing
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/24/14   vr      Split ESTK init handling
03/20/14   sw      Enable GSTK_CFG_FEATURE_ESTK_QMI and GSTK_CFG_FEATURE_REPORT_SW1_SW2
                   in feature_enabled flag if GSTK_CFG_QMI_CAT_CFG is enabled.
03/04/14   shr     Fixed ESTK user confirmation timer expiry processing
03/03/14   shr     ESTK BIP clean-up and enhancements
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
01/30/14   vr      SIM initiated call control handling
01/21/14   kb      Replace modem_mem api with gstk wrapper
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Added estk_stop function
11/25/13   sw      Send USSD TR when REL_USS_IND is received
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
11/07/13   vr      Don't check for GSTK_CFG_FEATURE_ESTK_ICON for icon display
10/04/13   gs      F3 Message reduction
09/13/13   gm      Support for Recovery and hot-swap
09/06/13   hh      Clean up DTMF string when TR is sent for SEND DTMF
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
07/15/13   gs      Remove NV 5770 dependency for STK related tests except for GSM
07/01/13   hn      Send command_number, additional_info and other_info to ESTK
06/20/13   vr      Triton TSTS changes
06/17/13   vr      Fix for SETUP CALL with DTMF tones
05/24/13   hn      Does APN mapping based on NV item
05/21/13   xz      Change TR result code when time out on user confirmation
05/17/13   hn      Support for Alpha for Refresh
05/07/13   vr      Send data buffer clean-up, if timer expires for send data
04/09/13   vr      Use safer versions of memcpy() and memmove()
04/02/13   av      Move UIM tasks to group4 of RCINIT
02/28/13   bd      Support for dynamic reg and dereg
12/21/12   hk      Disable all the BIP related bits turned on in estk_profile_value_list
11/29/12   bd      Clean up of redundant members from client table
11/15/12   xz      Fix issue of disabling BIP based on NV setting
09/10/12   sg      Handle SCWS alpha
08/15/12   sg      Fix cnf for close channel
06/25/12   dd      Read CS_PS NV item at init
05/16/12   dd      Use CM event to start DTMF timer
03/28/12   av      Replaced feature flag with NVITEM
03/16/12   yt      Modify reference to gstk_tcb
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/30/11   bd      Register for CM modified call type event
12/23/11   bd      Support for call control by card modifying the proactive
                   command SETUP CALL into a USSD
12/15/11   nb      Fixing incorrect loop leading to dog timeout
12/08/11   sg      Move ESTK-CM call event handling outside feature
12/05/11   sg      Clean up when NO_SIM in the middle of any estk command
11/29/11   dd      Additional info in TR if hold active calls fails
10/26/11   sg      Add TP support for EUTRAN, HDPA, UTRAN PS extended param
10/17/11   bd      Fixed incorrect use of pointers in CM’s context
09/30/11   nb      Swapped ESTKs registration sequence with WMS and CM so that even
                   if registration with CM fails we register with WMS
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/22/11   sg      Clean up after no_sim state
08/17/11   dd      Fix the ESTK state in Send DTMF
07/25/11   sg      Fix compiler warning
06/15/11   ea      Fix when STK Redial Duration TLV is absent
05/25/11   xz      Add more clean up mechanism
05/03/11   dd      Fix the issue with gstk_features being overwritten
04/19/11   sg      Fix BIP alpha display
04/26/11   yt      Replaced references to TMC heap with Modem heap
04/24/11   dd      SCWS support
04/20/11   ea      Add enable/disable support
04/19/11   nb      Initialize call connected flag at power up
03/31/11   ea      Add STK redial support
03/28/11   dd      Wait for icon_disp_rsp for network commands
03/15/11   bd      Fixed dangling pointer
03/16/11   nb      Pass slot info when sending raw Alpha ID to Display client
01/31/11   dd      Add user confirmation with icon for BIP
01/13/11   dd      Remove estk_util_config_io
01/12/11   sg      Dual slot scenarios cleanup on GSTK timer expiry
01/04/11   sg      Add API for cleanup on GSTK timer expiry
12/28/10   sg      Cleanup Open channel proactive command
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/18/10   sg      1) Propagate timer for delaying TR for BIP close channel
                   2) Propagate fix for Phys Link Down
11/19/10   yt      Fixed compilation error
11/10/10   yt      Memory reduction changes
09/29/10   xz      KW Fixes
09/17/10   bd      Ported the fix for issue of not sending envelope for menu selection
08/26/10   dd      Add GPRS to BIP TP DL
07/30/10   dd      Initialize slot_owns_cm_cli inside SUPPS feature as well
07/19/10   dd      Add tag to the raw and decoded hdr for Set Up Call
07/16/10   sg      Fix compilation error
07/15/10   dd      Fix sending of raw alpha to clients
07/15/10   sg      Fix Lint errors
07/14/10   dd      Remove display related events from TP for targets with no UI
06/30/10   dd      Icon display confirmation
03/30/10   sg      Fix memory allocation for APN, User Login and User Password
03/23/10   xz      1) Set TCP and UDP in TP
                   2) Only copies alphan when length is not zero
03/18/10   xz      Save entire bearer description information
03/12/10   sg      Adjusted TP DL number of items
02/21/10   xz      Fix link error
02/19/10   xz      Fix compilation error
02/15/10   nb      Moved Dual Slot Changes
02/05/09   sg      Bug fixes for ESTK BIP
12/17/09   sg      Added support for ESTK BIP
09/29/09   xz      Copy confirmation alpha for SETUP CALL cmd
09/26/09   kp      ZI memory reduction changes
07/29/09   xz      Fix issue of sending error TR for TP DL response
05/26/09   xz      1) Fix Compilation Warnings
                   2) Update extra TP bits if there is no UI
03/02/09   gg      Using correct defines for ESTK registration with GSTK
11/03/08   sk      Featurized ESTK based on external interface support
09/29/08   xz      Removed "static" for estk_gstk_init_cb()
09/16/08   sk      Added lint fixes
09/08/08   tml     Fixed auto header version generation and add Send SS
                   support
06/09/08   sk      Added support for terminal profile download and Send SMS

=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "uim_msg.h"

#if defined(FEATURE_ESTK)

#include "estk.h"
#include "estk_cm.h"
#include "estk_sms.h"
#include "estk_setup_menu.h"
#include "estk_refresh.h"
#include "estk_bip.h"
#include "gstk_exp.h"
#include "fs_lib.h"
#include "fs_public.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include "modem_mem.h"


/*===========================================================================

                             DEFINES

===========================================================================*/

/* Maximum number of commands on the queue */
#define ESTK_CMD_BUF_COUNT               0x0A

/* MACROS related to Terminal Profile Download */
#define ESTK_NUM_OF_ITEMS_IN_TP_LIST     0x05

/* Enable by defalut, based on the the NV setting disable, if required */
#define GSTK_SEND_SS_TP_VAL                  0x01
#define GSTK_SEND_USSD_TP_VAL             0x01
#define GSTK_SET_UP_CALL_TP_VAL          0x01
#define GSTK_SEND_DTMF_TP_VAL            0x01
#define GSTK_SEND_SMS_TP_VAL               0x01

#define RAW_APDU_BUFFER_SIZE 200

/*===========================================================================

                             DECLARATIONS

===========================================================================*/

/* ESTK Task command queue */
q_type estk_cmd_q;

/* Internal ESTK related module info */
estk_priv_info_type  *estk_instances_ptr[GSTK_MAX_NUM_OF_INSTANCES];
estk_priv_info_type  *estk_curr_inst_ptr;
estk_shared_info_type estk_shared_info;
uint8                 estk_num_of_instances = 1;

/* Defined types used locally in estk.c */
typedef struct
{
  gstk_cmd_enum_type                  proactive_cmd;
  gstk_additional_info_code_enum_type additional_code;
}estk_proactive_session_additional_code_map;

typedef struct
{
  gstk_cmd_enum_type  cmd_enum;
  gstk_cmd_enum_type  rsp_enum;
}estk_proactive_cmd_rsp_map;

/* ESTK Terminal Profile download list */
static gstk_profile_dl_support_ext_type estk_profile_value_list[] =
{
  {GSTK_SEND_SMS_TP_VAL,    GSTK_SEND_SHORT_MSG_SUPPORT, (uint8)GSTK_APP_NONE},
  {GSTK_SEND_SS_TP_VAL,     GSTK_SEND_SS_SUPPORT,        (uint8)GSTK_APP_NONE},
  {GSTK_SEND_USSD_TP_VAL,   GSTK_SEND_USSD_SUPPORT,      (uint8)GSTK_APP_NONE},
  {GSTK_SET_UP_CALL_TP_VAL, GSTK_SET_UP_CALL_SUPPORT,    (uint8)GSTK_APP_NONE},
  {GSTK_SEND_DTMF_TP_VAL,   GSTK_SEND_DTMF_SUPPORT,      (uint8)GSTK_APP_NONE},
  {GSTK_SEND_SMS_TP_VAL,    GSTK_CDMA_SEND_SMS_SUPPORT,  (uint8)GSTK_APP_NONE},
  {1,                       GSTK_CDMA_SMS_PP_SUPPORT,    (uint8)GSTK_APP_NONE},

  {1, GSTK_DATA_AVAIL_EVT_SUPPORT,               (uint8)GSTK_APP_NONE},
  {1, GSTK_CH_STATUS_EVT_SUPPORT,                (uint8)GSTK_APP_NONE},
  {1, GSTK_OPEN_CH_SUPPORT,                      (uint8)GSTK_APP_NONE},
  {1, GSTK_CLOSE_CH_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_RECEIVE_DATA_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SEND_DATA_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_CH_STATUS_SUPPORT,                (uint8)GSTK_APP_NONE},
  {7, GSTK_NUM_CH_SUPPORT,                       (uint8)GSTK_APP_NONE},
  {1, GSTK_TCP_SUPPORT,                          (uint8)GSTK_APP_NONE},
  {1, GSTK_UDP_SUPPORT,                          (uint8)GSTK_APP_NONE},
  {1, GSTK_GPRS_SUPPORT,                         (uint8)GSTK_APP_NONE},
  {1, GSTK_TCP_SERVER_MODE_SUPPORT,              (uint8)GSTK_APP_NONE}
#ifdef FEATURE_LTE
  ,{1, GSTK_USAT_EUTRAN_CLASS_E_SUPPORT_BYTE_17_BIT_7, (uint8)GSTK_APP_USIM}
#endif /* FEATURE_LTE */
#ifdef FEATURE_HSDPA
  ,{1, GSTK_USAT_HDPA_CLASS_E_SUPPORT_BYTE_17_BIT_8,   (uint8)GSTK_APP_USIM}
#endif /* FEATURE_HSDPA */
#ifdef FEATURE_WCDMA
  ,{1, GSTK_USAT_UTRAN_PS_EXT_PARAM_CLASS_E_SUPPORT_BYTE_22_BIT_1, (uint8)GSTK_APP_USIM}
#endif /* FEATURE_WCDMA */
};

/* ESTK Proactive command registration list */
static gstk_toolkit_cmd_reg_info_type estk_format_reg_list[] =
{
  {{GSTK_REG_CATEGORY_E_CMD, GSTK_REG_DATA_EVT},                    GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_E_CMD, GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT},GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_E_CMD, GSTK_REG_SETUP_EVT_CH_STATUS_EVT},     GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_0_CMD, GSTK_REG_SEND_SMS_EVT},      GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_0_CMD, GSTK_REG_SEND_SS_EVT},       GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_0_CMD, GSTK_REG_SEND_USSD_EVT},     GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_0_CMD, GSTK_REG_SETUP_CALL_EVT},    GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_0_CMD, GSTK_REG_SEND_DTMF_EVT},     GSTK_HANDLE_MODEM_FUNC_ONLY},
  {{GSTK_REG_CATEGORY_0_CMD, GSTK_REG_REFRESH_EVT},     GSTK_HANDLE_MODEM_FUNC_ONLY}
};

/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/
static void estk_gstk_evt_cb(
  gstk_client_pro_cmd_reg_data_type *req_ptr
);

static void estk_gstk_evt_cb_process_proactive_cmd(
   gstk_cmd_from_card_type  *gstk_req_rsp_ptr
);

static void estk_gstk_cmd_cb(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_ref_data_type         user_data
);

void estk_gstk_init_cb (
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
);

static void estk_build_and_send_pro_cmd_reg_to_gstk(uint32 inst_id);

static void estk_cleanup_priv_data(gstk_cmd_enum_type term_rsp_type);

static void estk_cm_sups_reg_cmd_cb(
  cm_client_id_type            client_id,
  cm_sups_cmd_e_type           cmd,
  cm_sups_cmd_err_e_type       cmd_err
);

static estk_result_enum_type estk_process_icon_display_cnf_cmd(
  const estk_cmd_type *cmd_ptr
);

static void estk_recovery_cleanup(gstk_slot_id_enum_type slot_id);
static int estk_find_noncontinuable_cmd(void *item_ptr, void *param_ptr);
static void estk_delete_noncontinuable_cmds_in_q(
   q_type                 *estk_cmd_q_ptr,
   gstk_slot_id_enum_type slot_id
);
static void estk_recovery_cleanup_priv_data_and_set_st(void);
static void estk_hot_swap_cleanup(gstk_slot_id_enum_type slot_id);
static void estk_delete_all_cmds_in_q(q_type *estk_cmd_q_ptr);
static void estk_reject_new_proactive_cmd(
   const gstk_cmd_from_card_type  *gstk_req_rsp_ptr
);
static void estk_disable_tp_and_registration(void);
static void estk_register_proactive_cmds(void);

/*===========================================================================

FUNCTION: estk_switch_to_instance

DESCRIPTION:
  Switch to an instance of ESTK specified by SIM slot ID

PARAMETERS:
  slot_id   The SIM slot ID specifying which instance to switch to

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_switch_to_instance(
  gstk_slot_id_enum_type slot_id
)
{
  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return FALSE;
  }

  UIM_MSG_HIGH_1("switch to ESTK instance 0x%x.", (uint32)slot_id - 1);
  estk_curr_inst_ptr = estk_instances_ptr[(uint32)slot_id - 1];

  return TRUE;
}

/*===========================================================================

FUNCTION:   estk_user_or_disp_conf_timeout

DESCRIPTION:
  Posts user confirmation timeout command to estk.

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
void estk_user_or_disp_conf_timeout(uint32 slot)
{
  estk_cmd_type          *task_cmd_ptr = NULL;

  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr != NULL)
  {
    task_cmd_ptr->hdr.sim_slot_id = (gstk_slot_id_enum_type)slot;
    task_cmd_ptr->hdr.cmd = ESTK_USER_OR_DISP_CONF_TIMEOUT_CMD;
    estk_put_cmd(task_cmd_ptr);
  }
}

/*===========================================================================

FUNCTION:   estk_handle_user_or_disp_conf_timeout

DESCRIPTION:
  This ESTK function builds a "No response from user" Terminal response when
  the user confirmation is not received within a certain time.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_handle_user_or_disp_conf_timeout()
{
  gstk_cmd_enum_type                   rsp         = GSTK_CMD_ENUM_MAX;
  int32                                index       = 0;
  gstk_general_result_enum_type        result      = GSTK_GENERAL_RESULT_MAX;
  gstk_terminal_rsp_extra_param_type   other_info;

  memset(&other_info, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));

  UIM_MSG_HIGH_2("In estk_handle_user_or_disp_conf_timeout(), command_id: 0x%x, state: 0x%x",
                 estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id, estk_curr_inst_ptr->state);

  switch (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
    case GSTK_SETUP_CALL_REQ:
      rsp = GSTK_SETUP_CALL_CNF;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_USER_CNF_ST)
      {
        result = GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST;
      }
      else
      {
        result = estk_curr_inst_ptr->result;
      }
      break;

    case GSTK_OPEN_CH_REQ:
      rsp = GSTK_OPEN_CH_CNF;
      result = GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST;
      estk_curr_inst_ptr->extra_param.present = TRUE;
      estk_curr_inst_ptr->extra_param.rsp_type = GSTK_OPEN_CH_CNF;
      estk_curr_inst_ptr->extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.buffer_size;
      (void)memscpy(&estk_curr_inst_ptr->extra_param.extra_param.open_ch_extra_param.bearer_description,
                    sizeof(gstk_bearer_description_type),
                    &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                    sizeof(gstk_bearer_description_type));
      (void)memscpy(&estk_curr_inst_ptr->extra_param.extra_param.open_ch_extra_param.bearer_description,
                    sizeof(gstk_bearer_description_type),
                    &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                    sizeof(gstk_bearer_description_type));
      break;

    case GSTK_REFRESH_REQ:
      rsp = GSTK_REFRESH_CNF;
      result = estk_curr_inst_ptr->result;
      break;

    case GSTK_SEND_SMS_REQ:
      rsp = GSTK_SEND_SMS_CNF;
      result = estk_curr_inst_ptr->result;
      break;

    case GSTK_SEND_SS_REQ:
      rsp = GSTK_SEND_SS_CNF;
      result = (estk_curr_inst_ptr->extra_param.present == TRUE) ?
             GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
             estk_curr_inst_ptr->result;
      break;

    case GSTK_SEND_USSD_REQ:
      rsp = GSTK_SEND_USSD_CNF;
      if(estk_shared_info.sups_info_ptr == NULL ||
         estk_shared_info.sups_info_ptr->ss_operation != ESTK_PROCESS_USSD_DATA)
      {
        result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
      }
      else
      {
        result = estk_curr_inst_ptr->result;
      }
      break;

    case GSTK_SEND_DTMF_REQ:
      rsp = GSTK_SEND_DTMF_CNF;
      result = estk_curr_inst_ptr->result;
      break;

    case GSTK_RECEIVE_DATA_REQ:
      if (estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      }
      else
      {
        return estk_send_receive_data_terminal_response(
        estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);
      }
      break;

    case GSTK_SEND_DATA_REQ:
      rsp = GSTK_SEND_DATA_CNF;
      result = estk_curr_inst_ptr->result;
      if(estk_convert_ch_id_to_index(estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id,
                                      &index)== ESTK_ERROR)
      {
        UIM_MSG_ERR_1("Couldn't find index for channel Id: 0x%x",
                      estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id);
        return ESTK_ERROR;
      }
      estk_bip_reset_curr_cmd_id_for_channel(index);
      break;

    case GSTK_CLOSE_CH_REQ:
      rsp = GSTK_CLOSE_CH_CNF;
      result = estk_curr_inst_ptr->result;
      if(estk_convert_ch_id_to_index(estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_id,
                                      &index) == ESTK_ERROR)
      {
        UIM_MSG_ERR_1("Couldn't find index for channel Id: 0x%x",
                      estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_id);
        return ESTK_ERROR;
      }

      if(!estk_curr_inst_ptr->keep_ch_open)
      {
        estk_bip_clean_up_ch_table(index);
      }
      break;

    default:
      UIM_MSG_ERR_0("There is no user/display cnf timeout handling for this command.");
      return ESTK_SUCCESS;
  }

  if (estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
  }
  else
  {
    return estk_send_terminal_response(
          estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
          rsp,
          result,
          &(estk_curr_inst_ptr->additional_result),
          &(estk_curr_inst_ptr->extra_param));
  }
  return ESTK_SUCCESS;
}/*estk_handle_user_or_disp_conf_timeout */

/*===========================================================================

FUNCTION: estk_cache_additional_info

DESCRIPTION:
  ESTK caches the TR information to be sent when alpha display response is received.
  Cache terminal response result, additional info, and other info.

PARAMETERS:
  additional_information_ptr : [Input] Additional information which needs to be filled in estk_curr_inst_ptr

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
  gstk_additional_info_ptr_type *additional_information_ptr)
{
  if((additional_information_ptr != NULL) && (additional_information_ptr->length != 0))
  {
    estk_curr_inst_ptr->additional_result.additional_info_ptr =
      (uint8 *)GSTK_CALLOC(additional_information_ptr->length);
    if(estk_curr_inst_ptr->additional_result.additional_info_ptr != NULL)
    {
      (void)gstk_memcpy(estk_curr_inst_ptr->additional_result.additional_info_ptr,
                  additional_information_ptr->additional_info_ptr,
                  additional_information_ptr->length,
                  additional_information_ptr->length,
                  additional_information_ptr->length);
      estk_curr_inst_ptr->additional_result.length = additional_information_ptr->length;
    }
  }
} /*estk_cache_additional_info */

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
void estk_init( void )
{
  UIM_MSG_LOW_0("In estk_init()");

  /* Initialize ESTK Cmd QUEUE */
  (void) q_init(&estk_cmd_q);

  memset(&estk_shared_info, 0x00, sizeof(estk_shared_info));

  estk_shared_info.call_info.call_id               = 0xFF;
  estk_shared_info.call_info.call_sys_mode = SYS_SYS_MODE_NO_SRV;
  estk_shared_info.call_info.hold_active_failed    = FALSE;  
  estk_shared_info.slot_owns_cm_cli                = GSTK_SLOT_ID_MAX;
  estk_shared_info.redial_timer_expired            = FALSE;
  estk_shared_info.redial_necessary                = FALSE;
  estk_shared_info.redial_in_progress              = FALSE;
  estk_shared_info.redial_timeout_present          = FALSE;
  estk_shared_info.redial_length_ms                = 0;
  estk_shared_info.cm_client_id                    = -1;
  estk_shared_info.wms_client_id                   = 0xFF;
  estk_shared_info.cmd_list.estk_gstk_reg_list_ptr = NULL;
} /* estk_init */

/*===========================================================================
FUNCTION: estk_register_proactive_cmds

DESCRIPTION:
  This ESTK function creates a set of Proactive commands that to be registered
  with GSTK based on the NV values set.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_register_proactive_cmds(void)
{
  uint64                         bitmask     = GSTK_REG_REFRESH_EVT;
  uint64                         bmsk        = 1;
  uint64                         evt         = 0;
  uint32                         size        = 0;
  uint8                          i           = 0;
  uint8                          num_entries = 0;

  /* Each of the NV condition is meant to enable the registration of respective
     proactive commands, this means that if the condition is satified mean
     that ESTK will register for that command. The below if conditions will
     become TRUE only when the corresponding NV bit is disabled.*/
  if(!(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE,
       GSTK_SLOT_ID_MAX) == TRUE))
  {
    bitmask = bitmask | GSTK_REG_SEND_USSD_EVT;
    bitmask = bitmask | GSTK_REG_SEND_SS_EVT;
  }
  if(!(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE,
       GSTK_SLOT_ID_MAX) == TRUE))
  {
     bitmask = bitmask | GSTK_REG_SETUP_CALL_EVT;
     bitmask = bitmask | GSTK_REG_SEND_DTMF_EVT;
  }
  if(!(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_WMS_INTERFACE,
       GSTK_SLOT_ID_MAX) == TRUE))
  {
     bitmask = bitmask | GSTK_REG_SEND_SMS_EVT;
  }
  if(!(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_BIP_INTERFACE,
       GSTK_SLOT_ID_MAX) == TRUE))
  {
     bitmask = bitmask | GSTK_REG_DATA_EVT;
     bitmask = bitmask | GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT;
     bitmask = bitmask | GSTK_REG_SETUP_EVT_CH_STATUS_EVT;
  }

  UIM_MSG_HIGH_1("estk_register_proactive_cmds: bitmask is 0x%x.", bitmask);

  if(bitmask > 0)
  {
    size = sizeof(estk_format_reg_list);
    estk_shared_info.cmd_list.estk_gstk_reg_list_ptr = (gstk_toolkit_cmd_reg_info_type *)GSTK_CALLOC(size);
    num_entries = size / sizeof(gstk_toolkit_cmd_reg_info_type);

    if (estk_shared_info.cmd_list.estk_gstk_reg_list_ptr == NULL)
    {
      return;
    }

    while (bmsk)
    {
      evt = bmsk & bitmask;
      if (evt)
      {
        for (i = 0; i < num_entries ; ++ i)
        {
          if (evt == estk_format_reg_list[i].reg_set.cmd_reg_bit_mask)
          {
            (void)gstk_memcpy(&estk_shared_info.cmd_list.estk_gstk_reg_list_ptr[estk_shared_info.cmd_list.estk_gstk_reg_list_num ++],
                           &estk_format_reg_list[i],
                           sizeof(gstk_toolkit_cmd_reg_info_type),
                           size,
                           sizeof(gstk_toolkit_cmd_reg_info_type));
            break;
          }
        }
      }
      bmsk <<= 1;
    }
  }
}

/*===========================================================================

FUNCTION: estk_gstk_init

DESCRIPTION:
  This ESTK Initialization functon includes memory allocation for instance
  specific data, initializing the requesting GSTK for client id to assist in
  toolkit command processing and estk timers.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_gstk_init(uint8 gstk_num_of_instances)
{
  uint32                  i           = 0;
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;

  UIM_MSG_HIGH_1("In estk_gstk_init(), num_of_instances: 0x%x", gstk_num_of_instances);

  estk_num_of_instances = gstk_num_of_instances;

  estk_register_proactive_cmds();

  for (i = 0; i < estk_num_of_instances; ++ i)
  {
    estk_instances_ptr[i] =
      (estk_priv_info_type *)GSTK_CALLOC(sizeof(estk_priv_info_type));

    if (estk_instances_ptr[i] == NULL)
    {
      ERR_FATAL("can't instantiate estk 0x%x!", i, 0, 0);
    }
    memset(estk_instances_ptr[i], 0x00, sizeof(estk_priv_info_type));
    estk_instances_ptr[i]->slot_id = (gstk_slot_id_enum_type)(i + 1);
    estk_instances_ptr[i]->state = ESTK_NOT_INIT_ST;

    estk_instances_ptr[i]->gstk_client_id = 0;
    estk_instances_ptr[i]->estk_cm_cs_call_connected = FALSE;

    rex_def_timer_ex(&(estk_instances_ptr[i]->cellular_oos_timer),
                     estk_bip_cellular_oos_timer_cb,
                     (i + 1));
    rex_def_timer_ex(&(estk_instances_ptr[i]->user_or_disp_conf_timer),
                     estk_user_or_disp_conf_timeout,
                     (i + 1));
    estk_instances_ptr[i]->curr_bip_cmd_index = -1;
    estk_bip_channel_table_init(i);

    estk_instances_ptr[i]->dns_addrs_len = 0;
    estk_instances_ptr[i]->keep_ch_open = FALSE;
    estk_instances_ptr[i]->ch_open_id = -1;
    estk_instances_ptr[i]->wms_msg_cfg.wms_ready_gw = FALSE;
    estk_instances_ptr[i]->wms_msg_cfg.wms_ready_cdma = FALSE;
    estk_instances_ptr[i]->wms_msg_cfg.wms_msg_ptr = NULL;
    /* Register as client of GSTK and get a client id */
    gstk_status = gstk_slot_client_init_ext (
                    (gstk_slot_id_enum_type)(i + 1),
                    GSTK_ESTK_TYPE,
                    estk_gstk_init_cb,
                    i);
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to queue client id request to GSTK 0x%x",
                    gstk_status);
      return;
    }
  }
} /* estk_gstk_init */


/*===========================================================================

FUNCTION: estk_cm_init

DESCRIPTION:
  This ESTK Initialization functon includes CM client registeration, call and
  dtmf, sups events registration, timers and NV related to CALL events and
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
void estk_cm_init( void )
{
  cm_client_status_e_type cm_status = CM_CLIENT_ERR_NONE;
  uint8                   index     = 0;

  /* Register with WMS for client id and events */
  estk_sms_reg();

  for(index = 0; index < ESTK_CALL_ID_MAX; index++)
  {
    estk_shared_info.call_info.call_list[index].call_id       = 0xFF;
    estk_shared_info.call_info.call_list[index].call_sys_mode = SYS_SYS_MODE_NO_SRV;
    estk_shared_info.call_info.call_list[index].call_active   = FALSE;
  }


  /* Register with CM if either of the feature is not disabled */
  if((gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE,
       GSTK_SLOT_ID_MAX) == FALSE) ||
      (gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE,
       GSTK_SLOT_ID_MAX) == FALSE))
  {
    cm_status = cm_client_init(CM_CLIENT_TYPE_GSTK,
      &(estk_shared_info.cm_client_id));

    if ( cm_status != CM_CLIENT_OK )
    {
      UIM_MSG_ERR_1("cm_client_init() returned bad status 0x%x",
                    (int32)cm_status);
      return;
    }

    /* CM Client Activation */
    cm_status = cm_client_act(estk_shared_info.cm_client_id);
    if ( cm_status != CM_CLIENT_OK )
    {
      UIM_MSG_ERR_1( "ESTK: Client cm_client_act Status 0x%x", cm_status );
      return;
    }

    /*  Register for call info list query results.  */
    cm_status = cm_mm_client_call_info_list_reg(estk_shared_info.cm_client_id,
                                                estk_cm_call_info_list_cb,
                                                CM_CLIENT_EVENT_REG,
                                                NULL );
    if ( cm_status != CM_CLIENT_OK )
    {
      UIM_MSG_ERR_1( "ESTK: Client call_info_list_reg Status 0x%x", cm_status );
      return;
    }
  }
  
  /* NOTE - An error in any of the function calls below will cause a return from this function.
          Please make sure no mandatory to execute code is below CMs registration
  */
  if(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE,
       GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* register for call events */
    cm_status = cm_mm_client_call_reg(
      estk_shared_info.cm_client_id,/* client id */
      estk_cm_call_event_cb, /* pointer to a cb function for call events */
      CM_CLIENT_EVENT_REG,   /* event registration */
      CM_CALL_EVENT_ORIG,    /* from this call event */
      CM_CALL_EVENT_CONNECT, /* to this call event */
      NULL );                /* pointer to a cb function for errors */
    if (cm_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_1("Failed to register for CM Call event 0x%x", cm_status);
      return;
    }

    /* register for Hold event */
    cm_status = cm_mm_client_call_reg(
      estk_shared_info.cm_client_id,/* client id */
      estk_cm_call_event_cb, /* pointer to a cb function for call events */
      CM_CLIENT_EVENT_REG,   /* event registration */
      CM_CALL_EVENT_MNG_CALLS_CONF,    /* from this call event */
      CM_CALL_EVENT_MNG_CALLS_CONF, /* to this call event */
      NULL );                /* pointer to a cb function for errors */
    if (cm_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_1("Failed to register for CM Call event 0x%x", cm_status);
      return;
    }
  
    rex_def_timer_ex(&estk_shared_info.cm_cmd_reattempt_timer,
                     estk_handle_cm_cmd_reattempt_timer,
                     0);

    /* register for modified call type event */
    cm_status = cm_mm_client_call_reg(
      estk_shared_info.cm_client_id,   /* client id */
      estk_cm_call_event_cb,           /* pointer to a cb function for call events */
      CM_CLIENT_EVENT_REG,             /* event registration */
      CM_CALL_EVENT_ORIG_MOD_TO_SS,    /* from this call event */
      CM_CALL_EVENT_ORIG_MOD_TO_SS,    /* to this call event */
      NULL );                          /* pointer to a cb function for errors */


    /* register for INBAND events */
    cm_status = cm_mm_client_inband_reg(
      estk_shared_info.cm_client_id,      /* client id */
      estk_cm_inband_event_cb,            /* pointer to a cb function for inband events */
      CM_CLIENT_EVENT_REG,                /* event registration */
      CM_INBAND_EVENT_REV_BURST_DTMF,     /* from this inband event */
      CM_INBAND_EVENT_REV_BURST_DTMF_ACK, /* to this inband event */
      NULL );                             /* pointer to a cb function for errors */
    if (cm_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_1("Failed to register for CM Inband event 0x%x", cm_status);
      return;
    }

    rex_def_timer_ex(&estk_shared_info.dtmf_timer,
                     estk_send_DTMF,
                     0);
    rex_def_timer_ex(&estk_shared_info.burst_dtmf_timer,
                     estk_handle_burst_DTMF,
                     0);
    rex_def_timer_ex(&estk_shared_info.redial_timer,
                     estk_handle_redial_timer,
                     0);
  }

  if(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE,
       GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* Register for Sups related events */
    cm_status = cm_client_sups_reg( estk_shared_info.cm_client_id, /* client id */
                                    estk_cm_sups_event_cb,
                                    CM_CLIENT_EVENT_REG,             /* event registration */
                                    CM_SUPS_EVENT_REGISTER,          /* from this sups event */
                                    CM_SUPS_EVENT_SS_MOD_TO_ORIG,    /* to this sups event */
                                    estk_cm_sups_reg_cmd_cb);
    if (cm_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_1("Failed to register for CM Sups event 0x%x", cm_status);
      return;
    }
  }
}/* estk_cm_init */

/*===========================================================================

FUNCTION estk_get_cmd_buf

DESCRIPTION:
  This function allocates a command buffer from the ESTK free memory queue.

PARAMETERS:
  None

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
estk_cmd_type* estk_get_cmd_buf( void )
{
  estk_cmd_type    *cmd_ptr = NULL;

  /* Get a command buffer from the free command queue.*/
  cmd_ptr = (estk_cmd_type *)GSTK_CALLOC(sizeof(estk_cmd_type));

  return( cmd_ptr );
} /* estk_get_cmd_buf */

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
void  estk_free_cmd_buf_data(estk_cmd_type *cmd_ptr )
{
  if (cmd_ptr == NULL)
  {
    return;
  }

  switch (cmd_ptr->hdr.cmd)
  {
  case ESTK_CM_CALL_EVENT_CMD:
    if(cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr != NULL)
    {
      gstk_free(cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr);
      cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr = NULL;
    }
    cmd_ptr->cmd_data.call_event_info.evt_payload.len = 0;
    break;
  case ESTK_CM_CALL_LIST_CMD:
  case ESTK_CM_INBAND_EVENT_CMD:
    /* Nothing to free */
    break;
  case ESTK_BIP_SOCK_EVT_CMD:
  case ESTK_BIP_PROCESS_NET_CB_CMD:
  case ESTK_BIP_KEEP_CH_OPEN_TIMER_EXP_CMD:
  case ESTK_BIP_CELLULAR_OOS_TIMER_EXP_CMD:
  case ESTK_BIP_CELLULAR_STATUS_CHANGE_EVT_CMD:
  case ESTK_BIP_WLAN_STATUS_CHANGE_EVT_CMD:
  case ESTK_BIP_WLAN_IP_ADDR_CHANGE_EVT_CMD:
    /*Nothing to free*/
    break;
  case ESTK_SCWS_CH_STATUS_REQ_CMD:
  case ESTK_SCWS_DATA_AVAIL_REQ_CMD:
  case ESTK_SCWS_OPEN_CH_RSP_CMD:
  case ESTK_SCWS_CLOSE_CH_RSP_CMD:
  case ESTK_SCWS_SEND_DATA_RSP_CMD:
    /* Nothing to free */
    break;
#if defined(FEATURE_ESTK_SETUP_MENU_INTERFACE)
  case ESTK_SETUP_MENU_REQ_CMD:
    estk_free_setup_menu_data(&cmd_ptr->cmd_data.setup_menu_req);
    break;
  case ESTK_MENU_SEL_REQ_CMD:
    break;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
  case ESTK_ICON_DISPLAY_CONF_CMD:
    break;
  case ESTK_CM_SUPS_EVENT_CMD:
    /* Nothing to free */
    break;
  case ESTK_WMS_MSG_SUBMIT_RPT_EVT_CMD:
    break;
  case ESTK_WMS_CFG_EVT_CMD:
    break;
  default:
    break;
  } /* End of switch */

} /* estk_free_cmd_buf_data */


/*===========================================================================

FUNCTION estk_free_cmd_buf

DESCRIPTION:
  This function puts the command back to the ESTK free queue.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer to be freed from the ESTK queue

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void  estk_free_cmd_buf( estk_cmd_type *cmd_ptr )
{
  if (cmd_ptr == NULL)
  {
    return;
  }
  gstk_free(cmd_ptr);
} /* estk_free_cmd_buf */

/*===========================================================================

FUNCTION estk_put_cmd

DESCRIPTION:
  This function puts a command on the ESTK command queue, and
  sets the GSTK_ESTK_CMD_SIG.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer to be put into the ESTK queue

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  The caller of this function should have previously allocated a command
  buffer by calling estk_get_cmd_buf().

SIDE EFFECTS:
  None

===========================================================================*/
void estk_put_cmd(estk_cmd_type *cmd_ptr)
{
  if (cmd_ptr == NULL)
  {
    return;
  }
  (void)q_link(cmd_ptr, &cmd_ptr->link);
  /* Put the message on the queue */
  q_put(&estk_cmd_q, &cmd_ptr->link);
  /* Set the command queue signal for GSTK task */
  (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_ESTK_CMD_SIG);

  UIM_MSG_HIGH_1("No of cmds on ESTK queue 0x%x", q_cnt(&estk_cmd_q));

  return;
} /* estk_put_cmd */

/*===========================================================================

FUNCTION: estk_gstk_init_cb

DESCRIPTION:
  Callback function from GSTK providing ESTK with client id if successful.

PARAMETERS:
  gstk_reg_status [Output]: gstk client id registration status
  client_id       [Output]: client id is registration is successful

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  GSTK client id table will have ESTK as a registered client if the
  client id registration is successful.

===========================================================================*/
void estk_gstk_init_cb
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
)
{
  if (!ESTK_IS_VALID_SLOT_ID(user_data + 1))
  {
    UIM_MSG_ERR_1("Invalid instance index 0x%x!", user_data);
    return;
  }

  /* Store client id */
  switch(gstk_reg_status)
  {
    case GSTK_SUCCESS:
      estk_instances_ptr[user_data]->gstk_client_id = client_id;
      break;
    case GSTK_CLIENT_SPACE_FULL:
    default:
      UIM_MSG_ERR_1("Unhandled gstk status for client id reg 0x%x",
                  gstk_reg_status);
      return;
  }/* switch */

  /* Register for proactive command upon receiving a valid client if
     from gstk */
  estk_build_and_send_pro_cmd_reg_to_gstk(user_data);
}/* estk_gstk_init_cb */

/*===========================================================================

FUNCTION: estk_get_terminal_profile_value_list

DESCRIPTION:
  Retreive ESTK's terminal profile value list.

PARAMETERS:
  gstk_profile_dl_support_ext_type    [Output]: estk profile value list
  num_of_tps_ptr                      [Output]: no. of profile updates

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
gstk_status_enum_type  estk_get_terminal_profile_value_list
(
  gstk_profile_dl_support_ext_type **estk_gstk_profile_value_list,
  uint8                             *num_of_tps_ptr
)
{
  boolean  cmd_enable           = FALSE;
  uint32   estk_disable_feature = FALSE;
  boolean  lte_supported        = FALSE;

  gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_BIP,&cmd_enable);

  estk_disable_feature =
     ((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_BIP_INTERFACE, GSTK_SLOT_ID_MAX) == TRUE) ||
      (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_WMS_INTERFACE, GSTK_SLOT_ID_MAX) == TRUE) ||
      (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE, GSTK_SLOT_ID_MAX) == TRUE) ||
      (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE, GSTK_SLOT_ID_MAX) == TRUE));

  lte_supported = gstk_util_find_modem_capability(GSTK_MODEM_CAPABILITY_FEATURE_LTE);

  if((!cmd_enable) || (estk_disable_feature) || (!lte_supported))
  {
    estk_disable_tp_and_registration();
  }

   /* Parameters check */
  if(num_of_tps_ptr == NULL || estk_gstk_profile_value_list == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  *num_of_tps_ptr = (uint8)(sizeof(estk_profile_value_list) /
                            sizeof(gstk_profile_dl_support_ext_type));

  *estk_gstk_profile_value_list = estk_profile_value_list;

  return GSTK_SUCCESS;
}/* estk_gstk_init_cb */

/*===========================================================================

FUNCTION: estk_build_and_send_pro_cmd_reg_to_gstk

DESCRIPTION:
  This function will build and send proactive command and event
  registration to GSTK.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  GSTK client id table will have ESTK as a registered client if the
  client id registration is successful.

===========================================================================*/
static void estk_build_and_send_pro_cmd_reg_to_gstk(
  uint32 inst_id
)
{
  gstk_status_enum_type  gstk_status  = GSTK_SUCCESS;

  /* Build registration command in the specified format */
  if((estk_shared_info.cmd_list.estk_gstk_reg_list_ptr) &&
     (estk_shared_info.cmd_list.estk_gstk_reg_list_num > 0))
  {
    gstk_status = gstk_client_toolkit_cmd_reg(
                    estk_instances_ptr[inst_id]->gstk_client_id,
                    inst_id,
                    GSTK_GSTK_FORMAT,
                    estk_shared_info.cmd_list.estk_gstk_reg_list_num, /* number of elements */
                    estk_shared_info.cmd_list.estk_gstk_reg_list_ptr,
                    estk_gstk_cmd_cb,
                    estk_gstk_evt_cb);

    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("ESTK reg for proactive command failed 0x%x", gstk_status);
    }
  }
}/* estk_build_and_send_pro_cmd_reg_to_gstk */

/*===========================================================================

FUNCTION estk_cm_sups_reg_cmd_cb

DESCRIPTION:
  This is the command callback function for the CM sups event registeration
  procedure.

PARAMETERS:
  client_id : [Input] CM client id
  cmd       : [Input] CM supplementary service command
  cmd_err   : [Input] command result

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_cm_sups_reg_cmd_cb(
  cm_client_id_type            client_id,
  cm_sups_cmd_e_type           cmd,
  cm_sups_cmd_err_e_type       cmd_err
)
{
  if (cmd_err != CM_SUPS_CMD_ERR_NOERR)
  {
    UIM_MSG_ERR_2("ESTK CM Sups cmd 0x%x failed 0x%x", cmd, cmd_err);
  }
} /* estk_cm_sups_reg_cmd_cb */

/*===========================================================================

FUNCTION: estk_gstk_cmd_cb

DESCRIPTION:
  Callback function from GSTK providing ESTK with client id if successful.

PARAMETERS:
  gstk_reg_status : [Input] gstk client id registration status
  user_data       : [Input]  user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  GSTK client id table will have ESTK as a registered client if the
  client id registration is successful.

===========================================================================*/
static void estk_gstk_cmd_cb(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_ref_data_type         user_data
)
{
   /* Dummy functionality */
}/* estk_gstk_cmd_cb */

/*===========================================================================

FUNCTION: estk_gstk_evt_cb

DESCRIPTION:
  Callback function from GSTK providing ESTK with the registered
  proactive command

PARAMETERS:
  req_ptr : [Input] proactive command information in the format client
                    requested (GSTK/RAW)

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  ESTK will build an appropriate terminal response after processing the
  proactive command

===========================================================================*/
static void estk_gstk_evt_cb(
  gstk_client_pro_cmd_reg_data_type *req_ptr
)
{
  gstk_slot_id_enum_type slot_id     = GSTK_SLOT_ID_MAX;

   if ((req_ptr == NULL) || 
      (req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr == NULL))
  {
    return;
  }

  UIM_MSG_HIGH_3("estk_gstk_evt_cb(): command_id=0x%x, "
                 "slot_id=0x%x, "
                 "data_format_type=0x%x",
                 req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id,
                 req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id,
                 req_ptr->data_format_type);

  if(req_ptr->data_format_type != GSTK_GSTK_FORMAT)
  {
    return;
  }

  slot_id = req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;
  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }
  (void)estk_switch_to_instance(slot_id);

  if(estk_curr_inst_ptr == NULL)
  {
    return;
  }

  /* 1. Handle GSTK_END_PROACTIVE_CMD_REQ First*/
  if (GSTK_END_PROACTIVE_CMD_REQ ==
      req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id)
  {
    UIM_MSG_HIGH_1("estk_gstk_evt_cb(): tag=0x%x",
                   req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.tag);
    switch (req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.tag)
    {
    case GSTK_TAG_RECOVERY:
      estk_recovery_cleanup(slot_id);
      break;
    case GSTK_TAG_CARD_ERROR:
      estk_hot_swap_cleanup(slot_id);
      break;
    case GSTK_TAG_POWER_DOWN:
      /* Cleanup will be done in estk_deinit function */
      break;
    default:
      /* Clear current proactive session related private data HERE
         since estk_save_gstk_cmd_details() overwrites
         estk_curr_inst_ptr->curr_cmd.hdr_cmd with gstk_req_ptr->hdr_cmd */
      estk_cleanup_priv_data(GSTK_END_PROACTIVE_CMD_REQ);
      estk_shared_info.in_legacy_mode = FALSE;
      break;
    }/* End of switch gstk_tag_enum_type */
  }/* GSTK_END_PROACTIVE_CMD_REQ */
  /* 2. Send error TR if there is an active proactive session */
  else if (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
    estk_reject_new_proactive_cmd(req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);
  }/* end ongoing proactive session */
  /* 3. Save and process registered proactive command */
  else
  {
    estk_gstk_evt_cb_process_proactive_cmd(
       req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);
  }
  return;
}/*estk_gstk_evt_cb */

/*===========================================================================

FUNCTION: estk_gstk_evt_cb_process_proactive_cmd

DESCRIPTION:
  Helper function to estk_gstk_evt_cb() that processes the registered
  proactive commands

PARAMETERS:
  gstk_req_rsp_ptr : [Input]
                    New proactive command that needs to be saved and processed

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  ESTK will build an appropriate terminal response after processing the
  proactive command

===========================================================================*/
static void estk_gstk_evt_cb_process_proactive_cmd(
   gstk_cmd_from_card_type  *gstk_req_rsp_ptr
)
{
  estk_result_enum_type  estk_status = ESTK_SUCCESS;
  uint32                 cmd_ref_id  = 0;
  gstk_cmd_enum_type     cnf_type    = GSTK_CMD_ENUM_NOT_USE;
  gstk_slot_id_enum_type slot_id     = GSTK_SLOT_ID_MAX;

  if(!gstk_req_rsp_ptr)
  {
    return;
  }

  UIM_MSG_HIGH_1("estk_gstk_evt_cb_process_session_evt: command_id = 0x%x",
                     gstk_req_rsp_ptr->hdr_cmd.command_id);

  estk_status = estk_save_gstk_cmd_details(gstk_req_rsp_ptr);

  if (estk_status == ESTK_SUCCESS)
  {
    switch(gstk_req_rsp_ptr->hdr_cmd.command_id)
    {
    case GSTK_SEND_SMS_REQ:
      estk_status = estk_process_send_sms_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SEND_SMS_CNF;
      break;
    case GSTK_SETUP_CALL_REQ:
      estk_status = estk_process_setup_call_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SETUP_CALL_CNF;
      break;
    case GSTK_SEND_DTMF_REQ:
      estk_status = estk_process_send_dtmf_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SEND_DTMF_CNF;
      break;
    case GSTK_SEND_USSD_REQ:
      estk_status = estk_sups_process_send_ussd_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SEND_USSD_CNF;
      break;
    case GSTK_SEND_SS_REQ:
      estk_status = estk_process_send_ss_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SEND_SS_CNF;
      break;
    case GSTK_PROFILE_DL_IND_RSP:
      slot_id = gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;
      if (ESTK_IS_VALID_SLOT_ID(slot_id))
      {
        estk_instances_ptr[(uint32)slot_id - 1]->state = ESTK_IDLE_ST;
        /* Cleanup WMS flags */
        estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_ready_gw = FALSE;
        estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_ready_cdma = FALSE;
        if(estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_msg_ptr != NULL)
        {
          gstk_free(estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_msg_ptr);
          estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_msg_ptr = NULL;
        }
      }
      estk_bip_full_clean_up();
      estk_cleanup_priv_data(GSTK_END_PROACTIVE_CMD_REQ);
      break;
    case GSTK_OPEN_CH_REQ:
      estk_status = estk_process_open_ch_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_OPEN_CH_CNF;
      break;
    case GSTK_SEND_DATA_REQ:
      estk_status = estk_process_send_data_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SEND_DATA_CNF;
      break;
    case GSTK_RECEIVE_DATA_REQ:
      estk_status = estk_process_receive_data_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_RECEIVE_DATA_CNF;
      break;
    case GSTK_GET_CH_STATUS_REQ:
      estk_status = estk_process_get_ch_status_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_GET_CH_STATUS_CNF;
      break;
    case GSTK_CLOSE_CH_REQ:
      estk_status = estk_process_close_ch_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_CLOSE_CH_CNF;
      break;
    case GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY:
    case GSTK_SETUP_EVT_CH_STATUS_NOTIFY:
      estk_status = estk_process_setup_evt_list_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_SETUP_EVENT_LIST_CNF;
      /* Cleanup global data since no TR would be sent for SUCCESS case */
      estk_cleanup_priv_data(GSTK_SETUP_EVENT_LIST_CNF);
      break;
    case GSTK_REFRESH_REQ:
      estk_status = estk_process_refresh_req(gstk_req_rsp_ptr);
      cnf_type = GSTK_REFRESH_CNF;
      break;
    default:
      /* Clear saved command */
      (void)secure_memset(&estk_curr_inst_ptr->curr_cmd,
                          0x00,
                          sizeof(gstk_cmd_from_card_type));
      return;
    } /* End of switch */
  }/* if (estk_status == ESTK_SUCCESS) */

  if (estk_status != ESTK_SUCCESS)
  {
    cmd_ref_id = gstk_req_rsp_ptr->hdr_cmd.cmd_detail_reference;
    if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
    {
      UIM_MSG_ERR_1("Invalid cmd_ref_id 0x%x", cmd_ref_id);
      return;
    }
    if ((estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST) ||
        (estk_curr_inst_ptr->state == ESTK_WAIT_FOR_USER_CNF_ST))
    {
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    }
    else
    {
    (void)estk_send_terminal_response(
            cmd_ref_id,         /* command_details_ref */
            cnf_type,           /* command response */
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
            NULL,               /* additional result */
            NULL);              /* extra param */
    }
  }/*  if (estk_status != ESTK_SUCCESS) */
}/* estk_gstk_evt_cb_process_proactive_cmd */

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
gstk_status_enum_type estk_process_cmd(void)
{
  estk_cmd_type         *cmd_ptr                  = NULL;
  estk_result_enum_type  result                   = ESTK_SUCCESS;
  gstk_state_enum_type   gstk_st                  = GSTK_POWER_DOWN_S;
  gstk_slot_id_enum_type slot_id                  = GSTK_SLOT_ID_MAX;

  /* Enter a loop where the command queue is checked and processed */
  while ((cmd_ptr = (estk_cmd_type *) q_get(&estk_cmd_q)) != NULL)
  {
    slot_id = cmd_ptr->hdr.sim_slot_id;
    if (!ESTK_IS_VALID_SLOT_ID(slot_id) || !estk_switch_to_instance(slot_id))
    {
      UIM_MSG_ERR_2("ignore command: slot 0x%x, cmd 0x%x ",
                    slot_id, cmd_ptr->hdr.cmd);
      /* free the cmd buf data if any */
      estk_free_cmd_buf_data(cmd_ptr);
      /* Put the command back to ESTK free queue */
      estk_free_cmd_buf(cmd_ptr);
      continue;
    }
    /* check gstk state before processing queue item */
    if((gstk_st = gstk_get_state(slot_id)) != GSTK_IDLE_S)
    {
      UIM_MSG_ERR_1("GSTK in bad state 0x%x", gstk_st);
      estk_instances_ptr[(uint32)slot_id - 1]->state = ESTK_NOT_INIT_ST;
      /* free the cmd buf data if any */
      estk_free_cmd_buf_data(cmd_ptr);
      /* free priv data of pending cmd */
      (void)estk_clean_up_on_gstk_timer_expiry(slot_id);
      /* Put the command back to ESTK free queue */
      estk_free_cmd_buf(cmd_ptr);
      /* Cleanup WMS flags */
      estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_ready_gw = FALSE;
      estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_ready_cdma = FALSE;
      if(estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_msg_ptr != NULL)
      {
        gstk_free(estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_msg_ptr);
        estk_instances_ptr[slot_id - 1]->wms_msg_cfg.wms_msg_ptr = NULL;
      }
      result = ESTK_ERROR;
      continue;
    }
    UIM_MSG_HIGH_1("In estk_process_cmd(), Command is 0x%x", cmd_ptr->hdr.cmd);
    switch (cmd_ptr->hdr.cmd)
    {
    case ESTK_CM_CALL_EVENT_CMD:
      result= estk_process_cm_call_event_cmd(cmd_ptr);
      break;
    case ESTK_CM_CALL_LIST_CMD:
      result = estk_process_cm_call_list_cmd(cmd_ptr);
      break;
    case ESTK_CM_INBAND_EVENT_CMD:
      result = estk_process_cm_inband_event_cmd(cmd_ptr);
      break;
    case ESTK_BIP_SOCK_EVT_CMD:
      result = estk_bip_process_sock_evt(cmd_ptr);
      break;
    case ESTK_BIP_PROCESS_NET_CB_CMD:
      result = estk_process_net_cb_cmd(cmd_ptr);
      break;
    case ESTK_BIP_KEEP_CH_OPEN_TIMER_EXP_CMD:
      result = estk_process_keep_ch_open_expired_evt(cmd_ptr->hdr.cli_user_data);
      break;
    case ESTK_BIP_CELLULAR_OOS_TIMER_EXP_CMD:
      result = estk_process_cellular_oos_evt();
      break;
    case ESTK_BIP_CELLULAR_STATUS_CHANGE_EVT_CMD:
      result = estk_process_cellular_status_change_evt(cmd_ptr);
      break;
    case ESTK_BIP_WLAN_STATUS_CHANGE_EVT_CMD:
      result = estk_process_wlan_status_change_evt(cmd_ptr);
      break;
    case ESTK_BIP_WLAN_IP_ADDR_CHANGE_EVT_CMD:
      result = estk_process_wlan_ip_addr_change_evt();
      break;
#if defined(FEATURE_ESTK_SETUP_MENU_INTERFACE)
    case ESTK_SETUP_MENU_REQ_CMD:
      result = estk_process_setup_menu_req(cmd_ptr);
      break;
    case ESTK_MENU_SEL_REQ_CMD:
      result = estk_process_menu_sel_req(cmd_ptr);
      break;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
    case ESTK_ICON_DISPLAY_CONF_CMD:
      result = estk_process_icon_display_cnf_cmd(cmd_ptr);
      break;
    case ESTK_GENERAL_USER_CNF_CMD:
      UIM_MSG_HIGH_1("Command id is 0x%x",
                     estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
      if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ)
      {
        result = estk_process_setup_call_user_cnf_cmd(cmd_ptr);
      }
      else if (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id  == GSTK_OPEN_CH_REQ)
      {
        result = estk_process_bip_user_cnf_rsp(cmd_ptr);
      }
      else
      {
        /* Nothing */
      }
      break;
    case ESTK_SCWS_CH_STATUS_REQ_CMD:
      result = estk_process_scws_ch_status_req(cmd_ptr);
      break;
    case ESTK_SCWS_DATA_AVAIL_REQ_CMD:
      result = estk_process_scws_data_avail_req(cmd_ptr);
      break;
    case ESTK_SCWS_OPEN_CH_RSP_CMD:
      result = estk_process_scws_open_ch_rsp(cmd_ptr);
      break;
    case ESTK_SCWS_CLOSE_CH_RSP_CMD:
      result = estk_process_scws_close_ch_rsp(cmd_ptr);
      break;
    case ESTK_SCWS_SEND_DATA_RSP_CMD:
      result = estk_process_scws_send_data_rsp(cmd_ptr);
      break;
    case ESTK_CM_SUPS_EVENT_CMD:
      result = estk_process_cm_sups_event_cmd(cmd_ptr);
      break;
    case ESTK_REFRESH_CNF_CMD:
      /* Re-cache command_number since ESTK might have cleared out the cache already
          when it receives GSTK_PROFILE_DL_IND_RSP after REFRESH */
      estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number =
        cmd_ptr->hdr.command_number;
      result = estk_process_refresh_cnf(cmd_ptr);
      break;
    case ESTK_WMS_MSG_SUBMIT_RPT_EVT_CMD:
      result = estk_process_wms_msg_submit_rpt_event_cmd(cmd_ptr);
      break;
    case ESTK_WMS_CFG_EVT_CMD:
      result = estk_process_wms_cfg_event_cmd(cmd_ptr);
      break;
    case ESTK_USER_OR_DISP_CONF_TIMEOUT_CMD:
      result = estk_handle_user_or_disp_conf_timeout();
      break;
    default:
      result = ESTK_ERROR;
      break;
    } /* End of switch */

    /* free the cmd buf data if any */
    estk_free_cmd_buf_data(cmd_ptr);
    /* Put the command back to ESTK free queue */
    estk_free_cmd_buf(cmd_ptr);
    #ifndef TEST_FRAMEWORK
    if (result != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Error processing command 0x%x", cmd_ptr->hdr.cmd);
    }
    #endif /*!TEST_FRAMEWORK*/
  } /* while loop */

  if (result != ESTK_SUCCESS)
    return GSTK_ERROR;
  else
    return GSTK_SUCCESS;

}/* estk_process_cmd */

/*===========================================================================

FUNCTION estk_send_terminal_response

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
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_terminal_response (
  uint32                                     cmd_detail_ref_id,
  gstk_cmd_enum_type                         terminal_response_type,
  gstk_general_result_enum_type              general_result,
  gstk_additional_info_ptr_type              *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type         *other_info_ptr)
{
  gstk_status_enum_type               gstk_status                = GSTK_SUCCESS;
  /* Variables to represent no additional result or extra param */
  gstk_additional_info_ptr_type       additional_result_not_present;
  gstk_terminal_rsp_extra_param_type *other_info_not_present_ptr = NULL;
  /* Pointer variables to be passed to GSTK function */
  gstk_additional_info_ptr_type      *additional_result_gstk_ptr = NULL;
  gstk_terminal_rsp_extra_param_type *other_info_gstk_ptr        = NULL;
  uint32                             i                           = 0;

  UIM_MSG_HIGH_3("estk_send_terminal_response, cmd_detail_ref_id: 0x%x, terminal_response_type: 0x%x, general_result: 0x%x",
                 cmd_detail_ref_id, terminal_response_type, general_result);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(GSTK_CMD_REF_TABLE_INDEX(cmd_detail_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_detail_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND",
                  cmd_detail_ref_id);
    return ESTK_ERROR;
  }

  /*
    Always reset channel reuse flags when sending OPEN CHANNEL TR

    1) If current open channel reused channel, then flags should be reset
       in order for parallel open channels to work

    2) If current open channel did not reuse channel, then previous PDN
       context is brought down, and reuse flags should be reset
  */
  if (terminal_response_type == GSTK_OPEN_CH_CNF)
  {
    estk_curr_inst_ptr->keep_ch_open = FALSE;
    estk_curr_inst_ptr->ch_open_id = -1;
  }

  memset(&additional_result_not_present,
         0x00,
         sizeof(gstk_additional_info_ptr_type));

  additional_result_gstk_ptr =
    (gstk_additional_info_ptr_type*)additional_result_ptr;
  other_info_gstk_ptr        =
    (gstk_terminal_rsp_extra_param_type*)other_info_ptr;

  /* GSTK requires that even when there is no additional result or extra
     param, the input parameters for these 2 fields into
     gstk_send_terminal_response_ext remains as a non null pointers */
  if (additional_result_gstk_ptr == NULL)
  {
    additional_result_gstk_ptr = &additional_result_not_present;
  }
  if (other_info_gstk_ptr == NULL)
  {
    other_info_not_present_ptr = (gstk_terminal_rsp_extra_param_type*)GSTK_CALLOC
      (sizeof(gstk_terminal_rsp_extra_param_type));
    if(!other_info_not_present_ptr)
    {
      return ESTK_ERROR;
    }
    other_info_gstk_ptr = other_info_not_present_ptr;
    memset(other_info_gstk_ptr, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
  }

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DATA_CARD, GSTK_SLOT_ID_MAX) == TRUE)
  {
    if ((estk_curr_inst_ptr->icon_present == TRUE) &&
        (general_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY))
    {
      UIM_MSG_HIGH_0("Icon present and command performed successful, update result");
      general_result = GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED;
    }
  }
  else
  {
    if (((estk_curr_inst_ptr->icon_present == TRUE) || (estk_curr_inst_ptr->call_icon_present == TRUE))&&
        (estk_curr_inst_ptr->icon_disp == FALSE) &&
        (general_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY))
    {
      UIM_MSG_HIGH_0("Icon present but not displayed, and command performed successful, update result");
      general_result = GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED;
    }
  }

  gstk_status = gstk_send_terminal_response_ext(
                    estk_curr_inst_ptr->gstk_client_id,
                    0x00,                                   /* user data */
                    cmd_detail_ref_id,
                    estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number,
                    terminal_response_type,
                    general_result,
                    (gstk_additional_info_ptr_type*)additional_result_gstk_ptr,
                    other_info_gstk_ptr);

  if (other_info_not_present_ptr != NULL)
  {
    gstk_free(other_info_not_present_ptr);
  }

  switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
    case GSTK_SEND_USSD_REQ:
      if(estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr != NULL)
      {
        gstk_free(estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr);
        estk_curr_inst_ptr->extra_param.extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr = NULL;
      }
      break;
    case GSTK_SEND_SS_REQ:
      if(estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr != NULL)
      {
        gstk_free(estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr);
        estk_curr_inst_ptr->extra_param.extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr = NULL;
      }
      break;
    case GSTK_SETUP_CALL_REQ:
      if(estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.additional_info2_ptr.additional_info_ptr != NULL)
      {
        gstk_free(estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.additional_info2_ptr.additional_info_ptr);
        estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.additional_info2_ptr.additional_info_ptr = NULL;
      }
      break;
    case GSTK_RECEIVE_DATA_REQ:
      if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data != NULL)
      {
        gstk_free(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data);
        estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data = NULL;
      }
      break;
    default:
      break;
  }

  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("TR fail, 0x%x", gstk_status);
    /* Clear global variables from previous proactive session before RECOVERY
       It would be better if GSTK can return new error code such as
       GSTK_INVALID_REF_ID for this purpose */
    if (gstk_status != GSTK_BAD_PARAM)
    {
      return ESTK_ERROR;
    }
  }

  /* perform cleanup:
     dtmf_str_ptr needs to be cleared up 1st to reset slot_owns_cm_cli
     */
  estk_cleanup_priv_data(terminal_response_type);
  for (i = 0; i < estk_num_of_instances; ++ i)
  {
    if (estk_curr_inst_ptr == estk_instances_ptr[i])
      break;
  }
  if (i != estk_num_of_instances)
  {
    if ((uint32)estk_shared_info.slot_owns_cm_cli == (i + 1))
    {
       if (estk_shared_info.call_info.dtmf_len <= 0)
       {
          estk_shared_info.slot_owns_cm_cli = GSTK_SLOT_ID_MAX;
       }
    }

    if ((uint32)estk_shared_info.slot_owns_wms_cli == (i + 1))
    {
      estk_shared_info.slot_owns_wms_cli = GSTK_SLOT_ID_MAX;
    }
  }

  estk_curr_inst_ptr->state = ESTK_IDLE_ST;
  estk_shared_info.in_legacy_mode = FALSE;

  return ESTK_SUCCESS;
}/* estk_send_terminal_response */

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
)
{
  gstk_toolkit_evt_cb_func_type          evt_cb      = NULL;
  gstk_client_ref_data_type              user_data   = 0;
  gstk_client_pro_cmd_reg_data_type      reg_info;

  if (gstk_io_ctrl(
        GSTK_IO_GET_CLIENT_TABLE_NEW_CLIENT_CB,
        &client_id,
        &evt_cb) != GSTK_SUCCESS)
  {
    return ESTK_ERROR;
  }
  if (gstk_io_ctrl(
        GSTK_IO_GET_CLIENT_TABLE_USER_DATA,
        &client_id,
        &user_data) != GSTK_SUCCESS)
  {
    return ESTK_ERROR;
  }
  cmd_ptr->raw_hdr.user_data = user_data;

  if (evt_cb == NULL)
  {
    UIM_MSG_ERR_2("failed to sent alpha to raw cli 0x%x, 0x%x, 0x%x!",
                  client_id, evt_cb);
    return ESTK_ERROR;
  }

  reg_info.data_format_type = GSTK_RAW_FORMAT;
  reg_info.pro_cmd_reg_data.raw_req_rsp_ptr = cmd_ptr;

  (evt_cb)(&reg_info);

   return ESTK_SUCCESS;
} /*estk_send_alpha_to_raw_client*/

/*===========================================================================
FUNCTION estk_send_alpha_to_display_client_legacy

DESCRIPTION:
  This function searches will convert alpha into:
  1) DISPLAY TEXT command if display_alpha is TRUE
  2) GET INKEY command if display_alpha is FALSE

  then send it to client

PARAMETERS:
  client:       [Input] client to recieve the alpha
  alpha:        [Input] The alpha to be sent
  display_alpha:[Input] indicate if it's display alpha or confirmation alpha
  raw_format:   [Input] indicate if raw format should be sent

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
estk_result_enum_type estk_send_alpha_to_display_client_legacy(
  gstk_client_id_type       client,
  const   gstk_string_type *alpha,
  boolean                   display_alpha,
  boolean                   raw_format
)
{
  gstk_cmd_from_card_type                          *gstk_command_ptr = NULL;
  gstk_io_call_client_evt_cb_param_type             call_cli_evt_cb;
  gstk_io_set_curr_session_ui_related_in_param_type set_ui_sess;
  uint32                                            user_data   = 0;
  gstk_slot_id_enum_type                            slot;
  uint32                                            len;
  gstk_raw_cmd_from_card_type                       raw_cmd;
  uint8                                             apdu_buff[RAW_APDU_BUFFER_SIZE];
  uint8                                             disp_txt_head[12] =
    {0xD0, 0x19, 0x81, 0x03, 0x01, 0x21, 0x80, 0x82, 0x02, 0x81, 0x02, 0x8D};
  uint8                                             get_inkey_head[12] =
    {0xD0, 0x19, 0x81, 0x03, 0x01, 0x22, 0x04, 0x82, 0x02, 0x81, 0x02, 0x8D};
  uint8                                             cmd_tail[3] =
    {0x02, 0x01, 0x05}; /* duration TLV */

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (raw_format)
  {
    len = 0;
    memset(apdu_buff, 0x00, sizeof(apdu_buff));
    if (display_alpha)
    {
      len = sizeof(disp_txt_head);
      if(gstk_memcpy(apdu_buff, disp_txt_head, len, RAW_APDU_BUFFER_SIZE, len) < len)
      {
        return ESTK_ERROR;
      }
    }
    else
    {
      len = sizeof(get_inkey_head);
      if(gstk_memcpy(apdu_buff, get_inkey_head, len, RAW_APDU_BUFFER_SIZE, len) < len)
      {
        return ESTK_ERROR;
      }
    }
    if ((sizeof(disp_txt_head) + 2 + alpha->length + sizeof(cmd_tail)) >
        RAW_APDU_BUFFER_SIZE)
    {
      UIM_MSG_ERR_2("apdu_buff is too small 0x%x vs 0x%x",
                    len + 2 + alpha->length, RAW_APDU_BUFFER_SIZE);
      return ESTK_ERROR;
    }
    apdu_buff[len++] = alpha->length;
    apdu_buff[len++] = alpha->dcs;
    if(gstk_memcpy(&apdu_buff[len],
                   alpha->text,
                   alpha->length,
                   RAW_APDU_BUFFER_SIZE - len,
                   alpha->length) <
       alpha->length)
    {
      return ESTK_ERROR;
    }
    len += alpha->length;
    if (display_alpha)
    {
      if(gstk_memcpy(&apdu_buff[len],
                     cmd_tail,
                     sizeof(cmd_tail),
                     RAW_APDU_BUFFER_SIZE - len,
                     sizeof(cmd_tail)) <
         sizeof(cmd_tail))
      {
        return ESTK_ERROR;
      }
      len += sizeof(cmd_tail);
    }

    /*adjust APDU length*/
    apdu_buff[1] = (uint8)uint32touint8(len - 1);

    raw_cmd.raw_hdr.cmd_ref_id  =
      estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
    raw_cmd.raw_hdr.sim_slot_id =
      estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    raw_cmd.payload.data_buffer_ptr = apdu_buff;
    raw_cmd.payload.data_len = (int32)len;
    if (estk_send_alpha_to_raw_client(client, &raw_cmd) != ESTK_SUCCESS)
    {
      return ESTK_ERROR;
    }
  }
  else
  {
    gstk_command_ptr = (gstk_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));
    if(gstk_command_ptr == NULL)
    {
      return ESTK_ERROR;
    }

    gstk_command_ptr->hdr_cmd.cmd_detail_reference =
      estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
    gstk_command_ptr->hdr_cmd.command_number = 1;
    gstk_command_ptr->hdr_cmd.sim_slot_id =
      estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    gstk_command_ptr->hdr_cmd.user_data =
      estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    if (display_alpha)
    {
      gstk_command_ptr->hdr_cmd.command_id = GSTK_DISPLAY_TEXT_REQ;
      gstk_command_ptr->cmd.display_text_pro_cmd_req.duration.present = TRUE;
      gstk_command_ptr->cmd.display_text_pro_cmd_req.duration.units = 1; /*seconds*/
      gstk_command_ptr->cmd.display_text_pro_cmd_req.duration.length = 5;
      gstk_command_ptr->cmd.display_text_pro_cmd_req.high_priority = TRUE;
      gstk_command_ptr->cmd.display_text_pro_cmd_req.immediate_response_required = FALSE;
      gstk_command_ptr->cmd.display_text_pro_cmd_req.text_string = *alpha;
      gstk_command_ptr->cmd.display_text_pro_cmd_req.user_control = FALSE;
    }
    else
    {
      gstk_command_ptr->hdr_cmd.command_id = GSTK_GET_INKEY_REQ;
      gstk_command_ptr->cmd.get_inkey_pro_cmd_req.help_available = FALSE;
      gstk_command_ptr->cmd.get_inkey_pro_cmd_req.response_format = GSTK_YES_NO;
      gstk_command_ptr->cmd.get_inkey_pro_cmd_req.text_string = *alpha;
    }
    call_cli_evt_cb.client_id = client;
    call_cli_evt_cb.gstk_req_ptr = gstk_command_ptr;
    if (gstk_io_ctrl(
          GSTK_IO_CALL_CLIENT_EVT_CB,
          &call_cli_evt_cb,
          &user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      gstk_free(gstk_command_ptr);
      return ESTK_ERROR;
    }
    gstk_free(gstk_command_ptr);
  }

  if (gstk_io_ctrl(
        GSTK_IO_GET_CLIENT_TABLE_SLOT_ID,
        &estk_curr_inst_ptr->gstk_client_id,
        &slot) != GSTK_SUCCESS || slot == GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_0("can't get slot id!");
    return ESTK_ERROR;
  }
  set_ui_sess.slot = slot;
  set_ui_sess.is_curr_session_ui_related = TRUE;
  (void)gstk_io_ctrl(
    GSTK_IO_SET_CURR_SESSION_UI_RELATED,
    &set_ui_sess,
    &user_data /* dummy data*/);

  estk_shared_info.in_legacy_mode = TRUE;

  return ESTK_SUCCESS;
} /*estk_send_alpha_to_display_client_legacy*/

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
  gstk_cmd_from_card_type *gstk_req_ptr,
  boolean                 display_alpha
)
{
  gstk_status_enum_type                    gstk_status = GSTK_SUCCESS;
  gstk_cmd_enum_type                       cmd_type;
  gstk_client_id_type                      client_id   = 0;
  gstk_string_type const                   *str_ptr;
  uint32                                   user_data   = 0;
  gstk_slot_id_enum_type                   slot;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;
  gstk_io_set_curr_session_ui_related_in_param_type set_ui_sess;
  gstk_string_type                         bip_str;
  int                                      i           = 0;
  gstk_alphabet_type                       *bip_alpha_ptr = NULL;
  boolean                                  raw_format  = FALSE;
  gstk_raw_cmd_from_card_type              raw_cmd;
  uint32                                   cmd_ref_id  = 0;
  estk_result_enum_type                    estk_status = ESTK_ERROR;
  /* Cannot do {0} due to compilation warning: enumerated type mixed
     with another type */
  gstk_client_pro_cmd_reg_data_type reg_info;

  memset(&reg_info, 0x00, sizeof(gstk_client_pro_cmd_reg_data_type));
  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&bip_str, 0x00, sizeof(gstk_string_type));

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  /* Check for input null parameter */
  if (gstk_req_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  cmd_type = gstk_req_ptr->hdr_cmd.command_id;

  /* Check to see if any client has registered to handle UI related
     functionality
  */
  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = cmd_type;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  &find_cli_reg,
                  &client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    find_cli_reg.format_type = GSTK_RAW_FORMAT;
    gstk_status = gstk_io_ctrl(
                    GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                    &find_cli_reg,
                    &client_id);
    if(gstk_status == GSTK_SUCCESS)
    {
      raw_format = TRUE;
    }
    else
    {
      UIM_MSG_HIGH_1("No client registered for alpha of cmd 0x%x, try legacy mode...",
                     cmd_type);
      if (display_alpha)
      {
        cmd_type = GSTK_DISPLAY_TEXT_REQ;
      }
      else
      {
        cmd_type = GSTK_GET_INKEY_REQ;
      }
      find_cli_reg.cli_func_type = GSTK_HANDLE_ALL_FUNC;
      find_cli_reg.cmd_type = cmd_type;
      find_cli_reg.format_type = GSTK_GSTK_FORMAT;
      gstk_status = gstk_io_ctrl(
                      GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                      &find_cli_reg,
                      &client_id);
      if(gstk_status != GSTK_SUCCESS)
      {
        find_cli_reg.format_type = GSTK_RAW_FORMAT;
        gstk_status = gstk_io_ctrl(
                        GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                        &find_cli_reg,
                        &client_id);
        if(gstk_status == GSTK_SUCCESS)
        {
          raw_format = TRUE;
        }
        else
        {
          UIM_MSG_ERR_0("can't send alpha to client!");
          /* it's ok that display alpha is not shown */
          if (display_alpha)
          {
            estk_status = ESTK_SUCCESS;
          }
          return estk_status;
        }
      }
      switch (gstk_req_ptr->hdr_cmd.command_id)
      {
      case GSTK_SEND_DTMF_REQ:
        str_ptr = &gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha;
        break;
      case GSTK_SEND_SS_REQ:
        str_ptr = &gstk_req_ptr->cmd.send_ss_pro_cmd_req.alpha;
        break;
      case GSTK_SEND_USSD_REQ:
        str_ptr = &gstk_req_ptr->cmd.send_ussd_pro_cmd_req.alpha;
        break;
      case GSTK_SEND_SMS_REQ:
        str_ptr = &gstk_req_ptr->cmd.send_sms_pro_cmd_req.alpha;
        break;
      case GSTK_SETUP_CALL_REQ:
        if (display_alpha)
        {
          str_ptr = &gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha;
        }
        else
        {
          str_ptr = &gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha;
        }
        break;
      case GSTK_OPEN_CH_REQ:
      case GSTK_SEND_DATA_REQ:
      case GSTK_RECEIVE_DATA_REQ:
      case GSTK_CLOSE_CH_REQ:
        if(gstk_req_ptr->hdr_cmd.command_id == GSTK_OPEN_CH_REQ)
        {
          bip_alpha_ptr = &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha);
        }
        else if(gstk_req_ptr->hdr_cmd.command_id == GSTK_SEND_DATA_REQ)
        {
          bip_alpha_ptr = &(gstk_req_ptr->cmd.send_data_pro_cmd_req.alpha);
        }
        else if(gstk_req_ptr->hdr_cmd.command_id == GSTK_RECEIVE_DATA_REQ)
        {
          bip_alpha_ptr = &(gstk_req_ptr->cmd.receive_data_pro_cmd_req.alpha);
        }
        else if(gstk_req_ptr->hdr_cmd.command_id == GSTK_CLOSE_CH_REQ)
        {
          bip_alpha_ptr = &(gstk_req_ptr->cmd.close_ch_pro_cmd_req.alpha);
        }
        bip_str.length = int32touint8(bip_alpha_ptr->length);
        bip_str.text = (uint8 *)GSTK_CALLOC(bip_str.length);
        if(bip_str.text == NULL)
        {
          return estk_status;
        }
        for(i=0; i < bip_str.length; i++)
        {
          bip_str.text[i] = uint16touint8(bip_alpha_ptr->alphabet[i]);
        }
        str_ptr = &bip_str;
        break;
      default:
        UIM_MSG_ERR_1("ESTK legacy mode not supported for cmd 0x%x!",
                      gstk_req_ptr->hdr_cmd.command_id);
        return estk_status;
      }

      estk_status = estk_send_alpha_to_display_client_legacy(client_id,
                                                             str_ptr,
                                                             display_alpha,
                                                             raw_format);
      if(bip_str.text)
      {
        gstk_free(bip_str.text);
        bip_str.text = NULL;
      }

      if (ESTK_SUCCESS == estk_status)
      {
        /* Start a timer to wait for the user/display response */
        (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));
        (void)rex_set_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer),
                            ESTK_USER_CONF_OR_ALPHA_DISP_TIMEOUT);
        if (!display_alpha)
        {
          estk_curr_inst_ptr->state = ESTK_WAIT_FOR_USER_CNF_ST;
        }
        else
        {
          estk_curr_inst_ptr->state = ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST;
        }
      }
      /* NOTE:
         If estk_send_alpha_to_display_client_legacy() returns value other than
         ESTK_SUCCESS and/or ESTK_ERROR, then set estk_status to ESTK_ERROR
         here before return*/
      return estk_status;
    }
  }

  /* valid callback function */
  if (raw_format)
  {
    cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
    if (gstk_io_ctrl(
          GSTK_IO_GET_CMD_RAW_APDU,
          &cmd_ref_id,
          &(raw_cmd.payload)) != GSTK_SUCCESS)
    {
      return estk_status;
    }

    raw_cmd.raw_hdr.cmd_ref_id  = cmd_ref_id;
    raw_cmd.raw_hdr.user_data   = user_data;
    raw_cmd.raw_hdr.sim_slot_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    raw_cmd.raw_hdr.tag = GSTK_TAG_ORIGINAL_CMD;
    if(display_alpha == FALSE)
    {
      raw_cmd.raw_hdr.tag  = GSTK_TAG_USER_CONF_ALPHA;
    }
    else
    {
      raw_cmd.raw_hdr.tag  = GSTK_TAG_DISPLAY_ALPHA;
    }

    if (estk_send_alpha_to_raw_client(client_id, &raw_cmd) != ESTK_SUCCESS)
    {
      gstk_free(raw_cmd.payload.data_buffer_ptr);
      return estk_status;
    }

    gstk_free(raw_cmd.payload.data_buffer_ptr);
  } /* raw_format == TRUE */
  else
  {
    UIM_MSG_HIGH_2("Sending the command 0x%x in GSTK format to client 0x%x",
                   cmd_type, client_id);
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)gstk_req_ptr;
    call_cli_evt_cb.gstk_req_ptr->hdr_cmd.tag = GSTK_TAG_ORIGINAL_CMD;
    if(display_alpha == FALSE)
    {
      call_cli_evt_cb.gstk_req_ptr->hdr_cmd.tag  = GSTK_TAG_USER_CONF_ALPHA;
    }
    else
    {
      call_cli_evt_cb.gstk_req_ptr->hdr_cmd.tag  = GSTK_TAG_DISPLAY_ALPHA;
    }

    if (gstk_io_ctrl(
          GSTK_IO_CALL_CLIENT_EVT_CB,
          &call_cli_evt_cb,
          &user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      return estk_status;
    }
  }


  /* Start a timer to wait for the user response */
  (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));
  (void)rex_set_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer),
                      ESTK_USER_CONF_OR_ALPHA_DISP_TIMEOUT);
  if (!display_alpha)
  {
    estk_curr_inst_ptr->state = ESTK_WAIT_FOR_USER_CNF_ST;
  }
  else
  {
    estk_curr_inst_ptr->state = ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST;
  }

  if (gstk_io_ctrl(
        GSTK_IO_GET_CLIENT_TABLE_SLOT_ID,
        &estk_curr_inst_ptr->gstk_client_id,
        &slot) != GSTK_SUCCESS || slot == GSTK_SLOT_ID_MAX)
  {
    return estk_status;
  }
  set_ui_sess.slot = slot;
  set_ui_sess.is_curr_session_ui_related = TRUE;
  (void)gstk_io_ctrl(
    GSTK_IO_SET_CURR_SESSION_UI_RELATED,
    &set_ui_sess,
    &user_data /* dummy data*/);

  return ESTK_SUCCESS;
}/* estk_send_alpha_to_display_client*/

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
    ESTK_ERROR  : If command could not be copied successfully

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_save_open_channel_cmd_details(
  const gstk_cmd_from_card_type *gstk_req_ptr,
  gstk_open_ch_req_type         *dst_ptr
)
{
  if((gstk_req_ptr == NULL) || (dst_ptr == NULL))
  {
    return ESTK_ERROR;
  }

  memset(dst_ptr, 0x00, sizeof(gstk_open_ch_req_type));

  dst_ptr->on_demand_link_est =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.on_demand_link_est;

  dst_ptr->alpha.alphabet_given =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.alphabet_given;

  dst_ptr->alpha.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length;

  /* Copy alpha if present */
  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.alphabet_given == TRUE &&
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length != 0)
  {
    dst_ptr->alpha.alphabet =
     (uint16 *)GSTK_CALLOC(((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length) * sizeof(uint16));

    if(dst_ptr->alpha.alphabet == NULL)
    {
      return ESTK_ERROR;
    }

    (void)memscpy(dst_ptr->alpha.alphabet,
           (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length * sizeof(uint16),
           gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.alphabet,
           (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length * sizeof(uint16));
  }

  /* Bearer TLV */
  (void)memscpy(&(dst_ptr->bearer),
          sizeof(gstk_bearer_description_type),
          &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.bearer),
          sizeof(gstk_bearer_description_type));

  /* Buffer size */
  dst_ptr->buffer_size = gstk_req_ptr->cmd.open_ch_pro_cmd_req.buffer_size;

  /* APN TLV */
  dst_ptr->access_point_name.alphabet_given =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given;

  dst_ptr->access_point_name.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given &&
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length != 0)
  {
    dst_ptr->access_point_name.alphabet =
      (uint16 *)GSTK_CALLOC(((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length) * sizeof(uint16));

   if(dst_ptr->access_point_name.alphabet == NULL)
   {
     return ESTK_ERROR;
   }

   (void)memscpy(dst_ptr->access_point_name.alphabet,
           (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16),
           gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
           (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16));
  }

  /* Local Address TLV */
  dst_ptr->local_address.address_type =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.address_type;

  dst_ptr->local_address.pkt_address.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length > 0)
  {
    dst_ptr->local_address.pkt_address.address =
     (uint8 *)GSTK_CALLOC((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length);

    if(dst_ptr->local_address.pkt_address.address == NULL)
    {
      return ESTK_ERROR;
    }

    if(dst_ptr->local_address.pkt_address.address != NULL)
    {
      (void)memscpy(dst_ptr->local_address.pkt_address.address,
              (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length,
              gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address,
              (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length * sizeof(uint8));
    }
  }

  /* User Login TLV */
  dst_ptr->text_string_user_login.alphabet_given =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet_given;

  dst_ptr->text_string_user_login.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet_given &&
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.length != 0)
  {
    dst_ptr->text_string_user_login.alphabet =
      (uint16 *)GSTK_CALLOC((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.length * sizeof(uint16));

    if(dst_ptr->text_string_user_login.alphabet == NULL)
    {
      return ESTK_ERROR;
    }

    (void)memscpy(dst_ptr->text_string_user_login.alphabet,
            (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.length * sizeof(uint16),
            gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet,
            (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_login.length * sizeof(uint16));
  }

  /* Password TLV */
  dst_ptr->text_string_user_passwd.alphabet_given =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet_given;

  dst_ptr->text_string_user_passwd.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet_given &&
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length != 0)
  {
    dst_ptr->text_string_user_passwd.alphabet =
      (uint16 *)GSTK_CALLOC((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length * sizeof(uint16));

    if(dst_ptr->text_string_user_passwd.alphabet == NULL)
    {
      return ESTK_ERROR;
    }

    (void)memscpy(dst_ptr->text_string_user_passwd.alphabet,
           (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length * sizeof(uint16),
           gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet,
           (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length * sizeof(uint16));
  }

  /* Transport Level TLV */
  (void)memscpy(&(dst_ptr->transport_level),
          sizeof(gstk_pkt_data_transport_level_type),
          &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level),
          sizeof(gstk_pkt_data_transport_level_type));

  /* Data Destination Address TLV */
  dst_ptr->data_dest_address.address_type =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.address_type;

  dst_ptr->data_dest_address.pkt_address.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length > 0)
  {
    dst_ptr->data_dest_address.pkt_address.address =
      (uint8 *)GSTK_CALLOC((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length);

    if(dst_ptr->data_dest_address.pkt_address.address == NULL)
    {
      return ESTK_ERROR;
    }

    (void)memscpy(dst_ptr->data_dest_address.pkt_address.address,
            (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length,
            gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address,
            (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length * sizeof(uint8));
  }

  /* DNS requested? */
  dst_ptr->dns_requested = gstk_req_ptr->cmd.open_ch_pro_cmd_req.dns_requested;

  return ESTK_SUCCESS;
} /* estk_save_open_channel_cmd_details */

/*===========================================================================

FUNCTION: estk_save_gstk_cmd_details

DESCRIPTION:
  This function will be called by ESTK to store the command details
  of the command received from GSTK. ESTK requires to store the
  command as GSTK releases the command memory when the ESTK callback
  function invloked by GSTK returns.

PARAMETERS:
  gstk_req_ptr : [Input] GSTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS: If commmand could be stored successfully
    ESTK_ERROR  : If command could not be stored successfully

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_save_gstk_cmd_details(
  const gstk_cmd_from_card_type *gstk_req_ptr
)
{
  gstk_setup_call_req_type *setup_call_ptr    = NULL;
  gstk_send_dtmf_req_type  *send_dtmf_ptr     = NULL;
  gstk_close_ch_req_type   *estk_close_ch_ptr = NULL;
  estk_result_enum_type     estk_status       = ESTK_SUCCESS;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_1("In estk_save_gstk_cmd_details() command_id 0x%x",
  	estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
  
  if(gstk_req_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  estk_shared_info.in_legacy_mode = FALSE;

  /* Populating global estk_curr_inst_ptr based on command type */
  estk_curr_inst_ptr->curr_cmd.hdr_cmd = gstk_req_ptr->hdr_cmd;

  switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
  case GSTK_SETUP_CALL_REQ:
    setup_call_ptr = &(estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req);
    /* Copy the address */
    (void)memscpy(&(setup_call_ptr->address),
           sizeof(gstk_address_type),
           &(gstk_req_ptr->cmd.setup_call_pro_cmd_req.address),
           sizeof(gstk_address_type));

    /* Copy the call set up requirement */
    setup_call_ptr->call_setup_requirement =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement;

    /* Copy the capability config */
    (void)memscpy(&(setup_call_ptr->capability_config),
           sizeof(gstk_cap_config_type),
           &(gstk_req_ptr->cmd.setup_call_pro_cmd_req.capability_config),
           sizeof(gstk_cap_config_type));

    /* Copy the subaddress */
    (void)memscpy(&(setup_call_ptr->destination_subaddress),
           sizeof(gstk_subaddress_type),
           &(gstk_req_ptr->cmd.setup_call_pro_cmd_req.destination_subaddress),
           sizeof(gstk_subaddress_type));
    if (setup_call_ptr->destination_subaddress.length >
        ESTK_MAX_SUBADDR_LEN)
    {
      setup_call_ptr->destination_subaddress.length =
        ESTK_MAX_SUBADDR_LEN;
    }

    /* Copy the redial details */
    (void)memscpy(&(setup_call_ptr->redial),
           sizeof(gstk_redial_type),
           &(gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial),
           sizeof(gstk_redial_type));

    /* Copy the confirmation alpha details */
    setup_call_ptr->user_cnf_alpha.dcs =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.dcs;
    setup_call_ptr->user_cnf_alpha.length =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length;
    if(setup_call_ptr->user_cnf_alpha.length > 0)
    {
      setup_call_ptr->user_cnf_alpha.text =
        (uint8 *)GSTK_CALLOC(setup_call_ptr->user_cnf_alpha.length);
      if(!setup_call_ptr->user_cnf_alpha.text)
      {
        return ESTK_ERROR;
      }
      /* Copy the alpha text */
      (void)memscpy(setup_call_ptr->user_cnf_alpha.text,
             setup_call_ptr->user_cnf_alpha.length,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);
    }/* if(setup_call_ptr->user_cnf_alpha.length > 0) */

    /* Copy the setup alpha details */
    setup_call_ptr->setup_call_display_alpha.dcs =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.dcs;
    setup_call_ptr->setup_call_display_alpha.length =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length;
    if(setup_call_ptr->setup_call_display_alpha.length > 0)
    {
      setup_call_ptr->setup_call_display_alpha.text =
        (uint8 *)GSTK_CALLOC(setup_call_ptr->setup_call_display_alpha.length);
      if(!setup_call_ptr->setup_call_display_alpha.text)
      {
        return ESTK_ERROR;
      }
      /* Copy the alpha text */
      (void)memscpy(setup_call_ptr->setup_call_display_alpha.text,
             setup_call_ptr->setup_call_display_alpha.length,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);
    }/* if(setup_call_ptr->setup_call_display_alpha.length > 0) */

    if ((gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) ||
        (gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present))
    {
      estk_curr_inst_ptr->icon_present = TRUE;
    }

    /* Copy the confirmation icon details */
    setup_call_ptr->user_cnf_icon.present =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.present;
    setup_call_ptr->user_cnf_icon.qualifier =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.qualifier;
    setup_call_ptr->user_cnf_icon.height =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.height;
    setup_call_ptr->user_cnf_icon.width =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.width;
    setup_call_ptr->user_cnf_icon.size =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.size;
    setup_call_ptr->user_cnf_icon.image_coding_scheme =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.image_coding_scheme;
    if(setup_call_ptr->user_cnf_icon.size > 0)
    {
      setup_call_ptr->user_cnf_icon.data =
        GSTK_CALLOC(gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
      if(!setup_call_ptr->user_cnf_icon.data)
      {
        return ESTK_ERROR;
      }
      /* Copy the icon data */
      (void)memscpy(setup_call_ptr->user_cnf_icon.data,
             setup_call_ptr->user_cnf_icon.size,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
    }
    setup_call_ptr->user_cnf_icon.rec_num =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.rec_num;

    /* Copy the call display icon details */
    setup_call_ptr->setup_call_display_icon.present =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present;
    setup_call_ptr->setup_call_display_icon.qualifier =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.qualifier;
    setup_call_ptr->setup_call_display_icon.height =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.height;
    setup_call_ptr->setup_call_display_icon.width =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.width;
    setup_call_ptr->setup_call_display_icon.size =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size;
    setup_call_ptr->setup_call_display_icon.image_coding_scheme =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.image_coding_scheme;
    if(setup_call_ptr->setup_call_display_icon.size > 0)
    {
      setup_call_ptr->setup_call_display_icon.data =
        GSTK_CALLOC(gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
      if(!setup_call_ptr->setup_call_display_icon.data)
      {
        return ESTK_ERROR;
      }
      /* Copy the icon data */
      (void)memscpy(setup_call_ptr->setup_call_display_icon.data,
             setup_call_ptr->setup_call_display_icon.size,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
             gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
    }
    setup_call_ptr->setup_call_display_icon.rec_num =
      gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.rec_num;

    if ((setup_call_ptr->setup_call_display_icon.present) &&
        (gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size > 0))
    {
      estk_curr_inst_ptr->call_icon_present = TRUE;
    }
    break;

  case GSTK_SEND_DTMF_REQ:
    send_dtmf_ptr = &(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req);

    /* Copy the address */
    if((gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.dtmf.length > 0) &&
        (gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf))
    {
      send_dtmf_ptr->dtmf.length =
        gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.dtmf.length;
      send_dtmf_ptr->dtmf.dtmf = (uint8 *)GSTK_CALLOC(send_dtmf_ptr->dtmf.length);
        if(!send_dtmf_ptr->dtmf.dtmf)
        {
          return ESTK_ERROR;
        }
        (void)memscpy(send_dtmf_ptr->dtmf.dtmf,
               sizeof(uint8)*send_dtmf_ptr->dtmf.length,
               gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
               sizeof(uint8)*gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.dtmf.length);
    }

    send_dtmf_ptr->alpha.length = 0;
    send_dtmf_ptr->alpha.text = NULL;

    /* Copy the alpha */
    if((gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha.length > 0) &&
        (gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha.text))
    {
      send_dtmf_ptr->alpha.dcs =
        gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha.dcs;

      send_dtmf_ptr->alpha.length =
        gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha.length;

      send_dtmf_ptr->alpha.text = (uint8 *)GSTK_CALLOC(send_dtmf_ptr->alpha.length);
      if(!send_dtmf_ptr->alpha.text)
      {
        send_dtmf_ptr->alpha.length = 0;
        return ESTK_ERROR;
      }

      (void)memscpy(send_dtmf_ptr->alpha.text,
             sizeof(uint8)*send_dtmf_ptr->alpha.length,
             gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha.text,
             sizeof(uint8)*gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.alpha.length);
    }
    else
    {
      UIM_MSG_HIGH_0("Zero alpha len");
    }

    if(gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.icon.present)
    {
      estk_curr_inst_ptr->icon_present = TRUE;
    }
    break;

  case GSTK_SEND_SS_REQ:
    if (gstk_req_ptr->cmd.send_ss_pro_cmd_req.icon.present)
    {
      estk_curr_inst_ptr->icon_present = TRUE;
    }
    break;

  case GSTK_SEND_USSD_REQ:
    if (gstk_req_ptr->cmd.send_ussd_pro_cmd_req.icon.present)
    {
      estk_curr_inst_ptr->icon_present = TRUE;
    }
    break;

  case GSTK_SEND_SMS_REQ:
    if (gstk_req_ptr->cmd.send_sms_pro_cmd_req.icon.present)
    {
      estk_curr_inst_ptr->icon_present = TRUE;
    }
    break;

  case GSTK_PROFILE_DL_IND_RSP:
    /* nothing to save */
    break;
  case GSTK_OPEN_CH_REQ:
    estk_status = estk_save_open_channel_cmd_details(
                    gstk_req_ptr,
                    &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req);

    if(estk_status != ESTK_SUCCESS)
    {
      return estk_status;
    }

    if (gstk_req_ptr->cmd.open_ch_pro_cmd_req.icon.present)
    {
      UIM_MSG_HIGH_0("Icon is present in Open Channel");
      estk_curr_inst_ptr->icon_present = TRUE;
    }
    break;

  case GSTK_SEND_DATA_REQ:
    estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id =
      gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_id;
    break;

  case GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY:
    break;

  case GSTK_RECEIVE_DATA_REQ:
    estk_curr_inst_ptr->curr_cmd.cmd.receive_data_pro_cmd_req.ch_id =
      gstk_req_ptr->cmd.receive_data_pro_cmd_req.ch_id;
    break;

  case GSTK_GET_CH_STATUS_REQ:
    break;

  case GSTK_CLOSE_CH_REQ:
    estk_close_ch_ptr = &estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req;
    estk_close_ch_ptr->ch_state = gstk_req_ptr->cmd.close_ch_pro_cmd_req.ch_state;
    estk_close_ch_ptr->ch_id = gstk_req_ptr->cmd.close_ch_pro_cmd_req.ch_id;
    break;
  case GSTK_REFRESH_REQ:
    if (gstk_req_ptr->cmd.refresh_pro_cmd_req.icon.present)
    {
      estk_curr_inst_ptr->icon_present = TRUE;
    }
    break;
  case GSTK_END_PROACTIVE_CMD_REQ:
    break;
  default:
    UIM_MSG_ERR_1("Unhandled command 0x%x",
                  estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
    return ESTK_ERROR;
  }/* End of switch */

  return ESTK_SUCCESS;
}/* estk_save_gstk_cmd_details */

/*===========================================================================

FUNCTION estk_cleanup_priv_data

DESCRIPTION:
  This function cleans up the ESTK private data stored with respect to the
  command that was just processed.

PARAMETERS:
  terminal_response_type : [Input] GSTK terminal response type

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_cleanup_priv_data(gstk_cmd_enum_type  terminal_response_type)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  switch(terminal_response_type)
  {
  case GSTK_OPEN_CH_CNF:
    estk_open_ch_cleanup(
      &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req);
    break;
  case GSTK_RECEIVE_DATA_CNF:
      estk_curr_inst_ptr->curr_cmd.cmd.receive_data_pro_cmd_req.ch_id = -1;
    break;
  case GSTK_SEND_DATA_CNF:
      estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id = -1;
    break;
  case GSTK_CLOSE_CH_CNF:
    estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_id = -1;
    break;
  case GSTK_SETUP_CALL_CNF:
    estk_setup_call_cleanup();
    break;

  case GSTK_SEND_SS_CNF:
  case GSTK_SEND_USSD_CNF:
    estk_sups_cleanup();
    break;

  case GSTK_SEND_DTMF_CNF:
    estk_send_dtmf_cleanup();
    break;

  case GSTK_END_PROACTIVE_CMD_REQ:
    /* Clean up all the extended info */
    switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
    {
    case GSTK_OPEN_CH_CNF:
      estk_open_ch_cleanup(
        &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req);
      break;
    case GSTK_RECEIVE_DATA_CNF:
        estk_curr_inst_ptr->curr_cmd.cmd.receive_data_pro_cmd_req.ch_id = -1;
      break;
    case GSTK_SEND_DATA_CNF:
        estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id = -1;
      break;
    case GSTK_CLOSE_CH_CNF:
        estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_id = -1;
      break;
    case GSTK_SETUP_CALL_REQ:
      estk_setup_call_cleanup();
      break;
    case GSTK_SEND_DTMF_REQ:
      estk_send_dtmf_cleanup();
      break;
    case GSTK_SEND_SS_REQ:
    case GSTK_SEND_USSD_REQ:
      estk_sups_cleanup();
      break;
    case GSTK_SEND_SMS_REQ:
      if(estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr != NULL)
      {
        gstk_free(estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr);
        estk_curr_inst_ptr->wms_msg_cfg.wms_msg_ptr = NULL;
      }
      break;
    default:
      break;
    }
    estk_curr_inst_ptr->state = ESTK_IDLE_ST;
    break;

  case GSTK_SEND_SMS_CNF:
  default:
    break;
  }/* End of switch */

  if(estk_curr_inst_ptr->additional_result.additional_info_ptr != NULL)
  {
    gstk_free(estk_curr_inst_ptr->additional_result.additional_info_ptr);
    estk_curr_inst_ptr->additional_result.additional_info_ptr = NULL;
  }
  memset(&estk_curr_inst_ptr->additional_result, 0x00, sizeof(gstk_additional_info_ptr_type));
  memset(&estk_curr_inst_ptr->extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
  estk_curr_inst_ptr->icon_present = FALSE;
  estk_curr_inst_ptr->icon_disp    = FALSE;
  estk_curr_inst_ptr->call_icon_present = FALSE;
  if (estk_shared_info.call_info.dtmf_len <= 0)
  {
    estk_shared_info.slot_owns_cm_cli = GSTK_SLOT_ID_MAX;
  }
  estk_shared_info.slot_owns_wms_cli = GSTK_SLOT_ID_MAX;
  /* reset the user conf/alpha display conf timer */
  (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));

  memset(&estk_curr_inst_ptr->curr_cmd, 0x00, sizeof(gstk_cmd_from_card_type));
}/* estk_cleanup_priv_data */

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
boolean estk_is_in_legacy_mode(void)
{
  if (estk_shared_info.in_legacy_mode)
  {
    estk_shared_info.in_legacy_mode = FALSE;
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION: estk_process_icon_display_cnf_cmd

DESCRIPTION :
  This function will be called by ESTK to check whether the icon was displayed
  by UI so that information can be used in Terminal Response

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_process_icon_display_cnf_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  int32                                index                       = 0;
  uint32                               cmd_ref_id       = 0;
  gstk_terminal_rsp_extra_param_type*  extra_param_ptr = NULL;
  gstk_additional_info_ptr_type*       additional_result           = NULL;
  gstk_general_result_enum_type        result          = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  gstk_cmd_enum_type                   command_id                  = GSTK_CMD_ENUM_NOT_USE;

  /* Step 1: Check input parameters */
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_3("In estk_process_icon_display_cnf_cmd() with icon_disp = 0x%x, estk state = 0x%x and Command_id 0x%x",
                 cmd_ptr->cmd_data.user_rsp.icon_disp,
                 estk_curr_inst_ptr->state,
                 estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);

  cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;

  if((cmd_ptr->hdr.cmd != ESTK_ICON_DISPLAY_CONF_CMD) ||
     (GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND))
  {
    UIM_MSG_ERR_2("Command id 0x%x, cmd_ref_id 0x%x", 
		 cmd_ptr->hdr.cmd, cmd_ref_id);
    return ESTK_ERROR;
  }
  if(rex_get_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer)) > 0)
    {
    UIM_MSG_HIGH_0("Clear user_or_disp_conf_timer on alpha/icon response");
    (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));
    }
  else
  {
    UIM_MSG_HIGH_0("ESTK is not waiting for any alpha/icon response!");
    return ESTK_ERROR;
  }

  /* Step 2: Check user response and store it into private data*/
  if((estk_curr_inst_ptr->icon_present) && !(estk_curr_inst_ptr->call_icon_present))
  {
    estk_curr_inst_ptr->icon_disp |= cmd_ptr->cmd_data.user_rsp.icon_disp;
  }
  else
  {
    estk_curr_inst_ptr->icon_disp = cmd_ptr->cmd_data.user_rsp.icon_disp;
  }

  /* Call connect was not received for Send_DTMF, set it to Pending TR state*/
  if((estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)||
     ((estk_curr_inst_ptr->state == ESTK_PREPROCESS_COMMAND_ST) &&
          (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_DTMF_REQ)))
  {
    UIM_MSG_HIGH_0("Changing ESTK state to pending TR");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    return ESTK_SUCCESS;
  }

  if(estk_curr_inst_ptr->state == ESTK_PENDING_TR_ST)
  {
    switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
      case GSTK_SETUP_CALL_REQ:
        command_id = GSTK_SETUP_CALL_CNF;
        result = (estk_curr_inst_ptr->extra_param.present == TRUE) ?
                 GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
                 estk_curr_inst_ptr->result;
        additional_result = &(estk_curr_inst_ptr->additional_result);
    extra_param_ptr = (estk_curr_inst_ptr->extra_param.present == TRUE) ?
        &(estk_curr_inst_ptr->extra_param):
        NULL;
        break;

      case GSTK_SEND_SMS_REQ:
        command_id = GSTK_SEND_SMS_CNF;
        result = estk_curr_inst_ptr->result;
        additional_result = &(estk_curr_inst_ptr->additional_result);
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
        break;
      case GSTK_SEND_SS_REQ:
        command_id = GSTK_SEND_SS_CNF;
     result = (estk_curr_inst_ptr->extra_param.present == TRUE) ?
           GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
           estk_curr_inst_ptr->result;
        additional_result = (estk_curr_inst_ptr->extra_param.present == TRUE) ?
                            NULL:
                            &(estk_curr_inst_ptr->additional_result);

    extra_param_ptr = (estk_curr_inst_ptr->extra_param.present == TRUE) ?
        &(estk_curr_inst_ptr->extra_param):
        NULL;
        break;

      case GSTK_SEND_USSD_REQ:
        command_id = GSTK_SEND_USSD_CNF;
    if(estk_shared_info.sups_info_ptr == NULL ||
       estk_shared_info.sups_info_ptr->ss_operation != ESTK_PROCESS_USSD_DATA)
    {
      result = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
    }
    else
    {
      result = estk_curr_inst_ptr->result;
    }
        additional_result = &(estk_curr_inst_ptr->additional_result);
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
        break;

      case GSTK_SEND_DTMF_REQ:
        command_id = GSTK_SEND_DTMF_CNF;
        result = estk_curr_inst_ptr->result;
        additional_result = &(estk_curr_inst_ptr->additional_result);
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
       break;

      case GSTK_REFRESH_REQ:
        command_id = GSTK_REFRESH_CNF;
        result = estk_curr_inst_ptr->result;
        additional_result = &(estk_curr_inst_ptr->additional_result);
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
        break;

      case GSTK_SEND_DATA_REQ:
        command_id = GSTK_SEND_DATA_CNF;
        result = estk_curr_inst_ptr->result;
        additional_result = &(estk_curr_inst_ptr->additional_result);
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
        if(estk_convert_ch_id_to_index(estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id,
                                       &index)== ESTK_ERROR)
        {
          UIM_MSG_ERR_1("Couldn't find index for channel Id: 0x%x",
                        estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.ch_id);
          return ESTK_ERROR;
  }
        estk_bip_reset_curr_cmd_id_for_channel(index);
        break;

      case GSTK_RECEIVE_DATA_REQ:
        return estk_send_receive_data_terminal_response(cmd_ref_id);

      case GSTK_CLOSE_CH_REQ:
        command_id = GSTK_CLOSE_CH_CNF;
        result = estk_curr_inst_ptr->result;
        additional_result = &(estk_curr_inst_ptr->additional_result);
        extra_param_ptr = &(estk_curr_inst_ptr->extra_param);
        if(estk_convert_ch_id_to_index(estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_id,
                                        &index) == ESTK_ERROR)
  {
          UIM_MSG_ERR_1("Couldn't find index for channel Id: 0x%x",
                        estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_id);
          return ESTK_ERROR;
  }
        estk_bip_reset_curr_cmd_id_for_channel(index);
        if(!estk_curr_inst_ptr->keep_ch_open)
  {
          estk_bip_clean_up_ch_table(index);
        }
        break;
      default:
        UIM_MSG_HIGH_0("Command not handled!");
        return ESTK_ERROR;
    }
    (void)estk_send_terminal_response(
                cmd_ref_id,                          /* command_details_ref */
        command_id,                               /* command response */
        result,                                   /* general_result */
        additional_result,                        /* additional result */
        extra_param_ptr);                         /* extra param */
  }

  return ESTK_SUCCESS;
}/* estk_process_icon_display_cnf_cmd */

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
  gstk_slot_id_enum_type    slot_id)
{
  estk_result_enum_type estk_result            = ESTK_SUCCESS;
  gstk_cmd_enum_type    terminal_response_type = GSTK_CMD_ENUM_NOT_USE;

  if(!estk_switch_to_instance(slot_id) || estk_curr_inst_ptr == NULL)
  {
    return GSTK_ERROR;
  }

  UIM_MSG_HIGH_1("estk_clean_up_on_gstk_timer_expiry for command id %d",
                 estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);

  switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
  case GSTK_OPEN_CH_REQ:
    estk_result = estk_clean_up_bip_open_ch_timer_exp();
    terminal_response_type = GSTK_OPEN_CH_CNF;
    break;
  case GSTK_SEND_DATA_REQ:
    estk_result = estk_clean_up_bip_send_data_timer_exp();
    terminal_response_type = GSTK_SEND_DATA_CNF;
    break;
  default: // Use in future for other commands
    terminal_response_type = GSTK_END_PROACTIVE_CMD_REQ;
    break;
  }

  estk_cleanup_priv_data(terminal_response_type);

  if (estk_result == ESTK_SUCCESS)
  {
    return GSTK_SUCCESS;
  }
  else
  {
    return GSTK_ERROR;
  }
}

/*===========================================================================
FUNCTION: estk_enable_evt_handling

DESCRIPTION:
  This function enable ESTK to register events that it's going to handle with
  GSTK. ESTK will only handle MODEM functionality of those proactive commands:
  1) SETUP CALL
  2) SEND DTMF
  3) SEND SMS
  4) SEND SS
  5) SEND USSD
  6) BIP

PARAMETERS:
  slot_id: [Input] the slot where ESTK event processing is enabled
                   GSTK_SLOT_AUTO means to enable event handling on all slots
  evt_bmsk:[Input] the bitmask of all events to be enabled

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
gstk_status_enum_type gstk_enable_estk_evt_handling(
   gstk_slot_id_enum_type slot,
   uint64                 evt_bmsk
)
{
  uint32                         i           = 0;
  gstk_status_enum_type          gstk_status = GSTK_SUCCESS;
  uint64                         bmsk        = 1;
  uint64                         evt         = 0;
  uint64                         temp_evt    = 0;
  uint32                         size        = 0;
  uint32                         num         = 0;
  uint32                         num_entries = 0;
  gstk_toolkit_cmd_reg_info_type *reg_list   = NULL;

  if (slot == GSTK_SLOT_AUTO)
  {
    for (i = 0; i < estk_num_of_instances; ++ i)
    {
      gstk_status =
        gstk_enable_estk_evt_handling((gstk_slot_id_enum_type)(i + 1), evt_bmsk);
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_2("Failed to enable ESTK: 0x%x 0x%x", gstk_status, i);
        return gstk_status;
      }
    }
  }
  else if (ESTK_IS_VALID_SLOT_ID(slot))
  {
    size = sizeof(estk_format_reg_list);
    num_entries = estk_shared_info.cmd_list.estk_gstk_reg_list_num;
    reg_list = (gstk_toolkit_cmd_reg_info_type *)GSTK_CALLOC(size);

    if (reg_list == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

    while (bmsk)
    {
      evt = bmsk & evt_bmsk;
      if (evt)
      {
        for (i = 0; i < num_entries; ++ i)
        {
          temp_evt = estk_shared_info.cmd_list.estk_gstk_reg_list_ptr[i].reg_set.cmd_reg_bit_mask;
          if (evt == temp_evt)
          {
            (void)gstk_memcpy(&reg_list[num ++],
                           &estk_shared_info.cmd_list.estk_gstk_reg_list_ptr[i],
                           sizeof(gstk_toolkit_cmd_reg_info_type),
                           size,
                           sizeof(gstk_toolkit_cmd_reg_info_type));
            break;
          }
        }
      }
      bmsk <<= 1;
    }
    if (num > 0)
    {
      gstk_status = gstk_client_toolkit_cmd_reg(
                      estk_instances_ptr[(uint32)slot - 1]->gstk_client_id,
                      (uint32)slot - 1,
                      GSTK_GSTK_FORMAT,
                      num, /* number of elements */
                      reg_list,
                      estk_gstk_cmd_cb,
                      estk_gstk_evt_cb);
      gstk_free(reg_list);
      return gstk_status;
    }
  }
  else
  {
    return GSTK_BAD_PARAM;
  }
  return gstk_status;
} /*estk_enable_evt_handling*/

/*===========================================================================
FUNCTION: estk_recovery_cleanup

DESCRIPTION:
  This function resets global variables and saved private data for proactive
  sessons that can not continue after the recovery occurs

PARAMETERS:
  slot_id: [Input] Slot on which recovery is in progress

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_recovery_cleanup(gstk_slot_id_enum_type slot_id)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  /* 1. Delete noncontinuable commands in estk_cmd_q*/
  estk_delete_noncontinuable_cmds_in_q(&estk_cmd_q, slot_id);

  /* 2. Clean up private data and set ESTK state for noncontinuable commands */
  estk_recovery_cleanup_priv_data_and_set_st();

  /* 3. Clean up BIP data */
  estk_bip_full_clean_up_slot(slot_id);
  estk_curr_inst_ptr->curr_bip_cmd_index = -1;
}/* estk_recovery_cleanup() */

/*===========================================================================
FUNCTION: estk_find_noncontinuable_cmd

DESCRIPTION:
  This function is called by q_linear_delete_new
  This function returns 1 for commmand that is NOT continuable after recovery

PARAMETERS:
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  ptr to slot id on which recovery is in progress

DEPENDENCIES:
  None

RETURN VALUE:
  0: Command is continuable, should NOT be deleted
  1: Command is NOT continuable, should be deleted

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static int estk_find_noncontinuable_cmd(
   void *item_ptr,
   void *param_ptr
)
{
  estk_cmd_type *cmd_ptr = NULL;
  int rc                 = 0;

  UIM_MSG_HIGH_2("estk_find_noncontinuable_cmd: item_ptr=0x%x, param_ptr=0x%x",
                 item_ptr, param_ptr);
  if ((!item_ptr) || (!param_ptr))
  {
    return rc;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, rc);

  cmd_ptr = (estk_cmd_type *)item_ptr;
  if (cmd_ptr->hdr.sim_slot_id != *((gstk_slot_id_enum_type *)param_ptr))
  {
    return rc;
  }

  switch (cmd_ptr->hdr.cmd)
  {
  case ESTK_CM_CALL_EVENT_CMD:
  case ESTK_CM_CALL_LIST_CMD:
  case ESTK_CM_INBAND_EVENT_CMD:
  case ESTK_CM_SUPS_EVENT_CMD:
    break;
  case ESTK_ICON_DISPLAY_CONF_CMD:
    switch (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
    {
    case GSTK_SEND_SMS_REQ:
    case GSTK_SEND_SS_REQ:
    case GSTK_SEND_USSD_REQ:
    case GSTK_SETUP_CALL_REQ:
    case GSTK_SEND_DTMF_REQ:
      break;
    default:
      rc = 1;
      break;
    }/* end switch current proactive cmd */
    break;
  case ESTK_GENERAL_USER_CNF_CMD:
    switch (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
    {
    case GSTK_SETUP_CALL_REQ:
      break;
    case GSTK_OPEN_CH_REQ:
    default:
      rc = 1;
      break;
    }/* end switch current proactive cmd */
    break;

  case ESTK_BIP_SOCK_EVT_CMD:
  case ESTK_BIP_PROCESS_NET_CB_CMD:
  case ESTK_BIP_KEEP_CH_OPEN_TIMER_EXP_CMD:
  case ESTK_BIP_CELLULAR_OOS_TIMER_EXP_CMD:
  case ESTK_BIP_CELLULAR_STATUS_CHANGE_EVT_CMD:
  case ESTK_BIP_WLAN_STATUS_CHANGE_EVT_CMD:
  case ESTK_BIP_WLAN_IP_ADDR_CHANGE_EVT_CMD:
#if defined(FEATURE_ESTK_SETUP_MENU_INTERFACE)
  case ESTK_SETUP_MENU_REQ_CMD:
  case ESTK_MENU_SEL_REQ_CMD:
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE */
  case ESTK_SCWS_CH_STATUS_REQ_CMD:
  case ESTK_SCWS_DATA_AVAIL_REQ_CMD:
  case ESTK_SCWS_OPEN_CH_RSP_CMD:
  case ESTK_SCWS_CLOSE_CH_RSP_CMD:
  case ESTK_SCWS_SEND_DATA_RSP_CMD:
  case ESTK_REFRESH_CNF_CMD:
  case ESTK_WMS_MSG_SUBMIT_RPT_EVT_CMD:
  case ESTK_WMS_CFG_EVT_CMD:
  default:
    rc = 1;
    break;
  }/* end switch cmd */
  return rc;
}/* estk_find_noncontinuable_cmd() */

/*===========================================================================
FUNCTION: estk_delete_noncontinuable_cmds_in_q

DESCRIPTION:
  This function deletes noncontinuable commands in estk_cmd_q upon recovery

PARAMETERS:
  estk_cmd_q_ptr: [Input] pointer to estk_cmd_q
  slot_id:        [Input] Slot on which recovery is in progress

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_delete_noncontinuable_cmds_in_q(
   q_type                 *estk_cmd_q_ptr,
   gstk_slot_id_enum_type slot_id)
{
  estk_cmd_type *cmd_ptr = NULL;

  if (!estk_cmd_q_ptr)
  {
    return;
  }
  while ((cmd_ptr = (estk_cmd_type *) q_linear_delete_new(
                                          estk_cmd_q_ptr,
                                          estk_find_noncontinuable_cmd,
                                          &slot_id,
                                          NULL,
                                          NULL)) != NULL)
  {
    /* free the cmd buf data if any */
    estk_free_cmd_buf_data(cmd_ptr);
    /* Put the command back to ESTK free queue */
    estk_free_cmd_buf(cmd_ptr);
  }/* end while */
}/* estk_delete_noncontinuable_cmds_in_q() */

/*===========================================================================
FUNCTION: estk_recovery_cleanup_priv_data_and_set_st

DESCRIPTION:
  This function executes the following for noncontinuable proactive session
  upon recovery:
  1. Cleans up private data related to the proactive session
  2. Sets ESTK state to ESTK_IDLE_ST

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
static void estk_recovery_cleanup_priv_data_and_set_st(void)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
  case GSTK_SEND_SMS_REQ:
  case GSTK_SEND_SS_REQ:
  case GSTK_SEND_USSD_REQ:
  case GSTK_SETUP_CALL_REQ:
  case GSTK_SEND_DTMF_REQ:
    break;
  case GSTK_OPEN_CH_CNF:
    estk_open_ch_cleanup(
      &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req);
  default:
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
    estk_free_setup_menu_data(&estk_curr_inst_ptr->setup_menu_cmd);
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
    estk_curr_inst_ptr->icon_present = FALSE;
    estk_curr_inst_ptr->icon_disp    = FALSE;
    estk_curr_inst_ptr->call_icon_present = FALSE;
    estk_shared_info.slot_owns_wms_cli = GSTK_SLOT_ID_MAX;
    /* For SETUP CALL, DTMF may not have completed yet after TR is sent */
    if (estk_shared_info.call_info.dtmf_len <= 0)
    {
      estk_shared_info.slot_owns_cm_cli  = GSTK_SLOT_ID_MAX;
    }
    (void)secure_memset(&estk_curr_inst_ptr->curr_cmd,
                        0x00,
                        sizeof(estk_curr_inst_ptr->curr_cmd));
    estk_curr_inst_ptr->state = ESTK_IDLE_ST;
    break;
  }/* end switch command_id */
}/* estk_recovery_cleanup_priv_data_and_set_st() */

/*===========================================================================
FUNCTION: estk_hot_swap_cleanup

DESCRIPTION:
  This function resets global variables and saved private data upon hot swap

PARAMETERS:
  slot_id: [Input] Slot on which hot swap is in progress

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_hot_swap_cleanup(gstk_slot_id_enum_type slot_id)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  /* 1. Delete all commands in estk_cmd_q*/
  estk_delete_all_cmds_in_q(&estk_cmd_q);

  /* 2. Clean up saved private data */
  estk_cleanup_priv_data(GSTK_END_PROACTIVE_CMD_REQ);
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  estk_free_setup_menu_data(&estk_curr_inst_ptr->setup_menu_cmd);
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
  estk_curr_inst_ptr->estk_cm_cs_call_connected = FALSE;
  estk_shared_info.in_legacy_mode = FALSE;
  estk_shared_info.call_info.call_id = 0xFF;
  estk_shared_info.call_info.call_sys_mode = SYS_SYS_MODE_NO_SRV;
  estk_shared_info.slot_owns_cm_cli = GSTK_SLOT_ID_MAX;
  if(estk_shared_info.call_info.dtmf_str_ptr)
  {
    gstk_free(estk_shared_info.call_info.dtmf_str_ptr);
    estk_shared_info.call_info.dtmf_str_ptr = NULL;
    estk_shared_info.call_info.dtmf_len = 0;
  }
  estk_shared_info.call_info.curr_dtmf_pos_in_str = 0;
  estk_shared_info.redial_timer_expired      = FALSE;
  estk_shared_info.redial_necessary          = FALSE;
  estk_shared_info.redial_in_progress        = FALSE;
  estk_shared_info.redial_timeout_present    = FALSE;
  estk_shared_info.redial_length_ms          = 0;

  /* 3. Clean up BIP data */
  estk_bip_full_clean_up_slot(slot_id);
  estk_curr_inst_ptr->curr_bip_cmd_index = -1;

  /* 4. set ESTK state to ESTK_NOT_INIT_ST */
  estk_curr_inst_ptr->state = ESTK_NOT_INIT_ST;
}/* estk_hot_swap_cleanup() */

/*===========================================================================
FUNCTION: estk_delete_all_cmds_in_q

DESCRIPTION:
  This function deletes all commands in estk_cmd_q upon hot swap

PARAMETERS:
  estk_cmd_q_ptr: [Input] pointer to estk_cmd_q

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_delete_all_cmds_in_q(q_type *estk_cmd_q_ptr)
{
  estk_cmd_type *cmd_ptr = NULL;

  if (!estk_cmd_q_ptr)
  {
    return;
  }

  while ((cmd_ptr = (estk_cmd_type *) q_get(estk_cmd_q_ptr)) != NULL)
  {
    /* free the cmd buf data if any */
    estk_free_cmd_buf_data(cmd_ptr);
    /* Put the command back to ESTK free queue */
    estk_free_cmd_buf(cmd_ptr);
  }/* end while */
}/* estk_delete_all_cmds_in_q() */

/*===========================================================================
FUNCTION: estk_reject_new_proactive_cmd

DESCRIPTION:
  This function is used to send GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  terminal response to GSTK when a new proactive command is received by ESTK
  while the previous proactive command is not complete yet.
  e.g. One of the following proactive sessions is allowed to continue if
  the command is processed by ESTK before the card RECOVERY:
  1) SETUP CALL
  2) SEND DTMF
  3) SEND SMS
  4) SEND SS
  5) SEND USSD

PARAMETERS:
  gstk_req_rsp_ptr: [Input] New proactive command that needs to be rejected

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void estk_reject_new_proactive_cmd(
   const gstk_cmd_from_card_type  *gstk_req_rsp_ptr)
{
  gstk_cmd_enum_type                  cnf_type       = GSTK_CMD_ENUM_NOT_USE;
  gstk_terminal_rsp_extra_param_type  other_info;
  gstk_additional_info_ptr_type       additional_info_ptr;
  gstk_additional_info_code_enum_type additional_code =
                                      GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  uint8                               i = 0;

  const estk_proactive_session_additional_code_map pro_addi_code_map[] =
  {
    /* No additional code for GSTK_SEND_SMS_REQ */
    {GSTK_SEND_SS_REQ,    GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION},
    {GSTK_SEND_USSD_REQ,  GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION},
    {GSTK_SETUP_CALL_REQ, GSTK_ME_CURRENTLY_BUSY_ON_CALL},
    {GSTK_SEND_DTMF_REQ,  GSTK_ME_CURRENTLY_BUSY_ON_SEND_DTMF_COMMAND},
  }; /* pro_addi_code_map */

  const estk_proactive_cmd_rsp_map pro_cmd_rsp_map[] =
  {
    {GSTK_REFRESH_REQ,                  GSTK_REFRESH_CNF},
    {GSTK_SEND_SMS_REQ,                 GSTK_SEND_SMS_CNF},
    {GSTK_SEND_SS_REQ,                  GSTK_SEND_SS_CNF},
    {GSTK_SEND_USSD_REQ,                GSTK_SEND_USSD_CNF},
    {GSTK_SETUP_CALL_REQ,               GSTK_SETUP_CALL_CNF},
    {GSTK_SEND_DTMF_REQ,                GSTK_SEND_DTMF_CNF},
    {GSTK_OPEN_CH_REQ,                  GSTK_OPEN_CH_CNF},
    {GSTK_CLOSE_CH_REQ,                 GSTK_CLOSE_CH_CNF},
    {GSTK_SEND_DATA_REQ,                GSTK_SEND_DATA_CNF},
    {GSTK_RECEIVE_DATA_REQ,             GSTK_RECEIVE_DATA_CNF},
    {GSTK_GET_CH_STATUS_REQ,            GSTK_GET_CH_STATUS_CNF},
    {GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY,  GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY},
    {GSTK_SETUP_EVT_CH_STATUS_NOTIFY,   GSTK_SETUP_EVENT_LIST_CNF},
  }; /* pro_cmd_rsp_map */

  UIM_MSG_HIGH_1("estk_reject_new_proactive_cmd: gstk_req_rsp_ptr=0x%x",
                 gstk_req_rsp_ptr);

  if (!gstk_req_rsp_ptr)
  {
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  (void)secure_memset(&additional_info_ptr, 0x00, sizeof(additional_info_ptr));

  /* 1. Set up additional error code */
  for (i = 0;
        i < (sizeof(pro_addi_code_map)/(sizeof(pro_addi_code_map[0])));
        i++)
  {
    if (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id ==
        pro_addi_code_map[i].proactive_cmd)
    {
      additional_code = pro_addi_code_map[i].additional_code;
      break;
    }
  }/* end for i */
  additional_info_ptr.length = (additional_code > 0xFF)?
                                sizeof(additional_code):1;
  if (NULL == (additional_info_ptr.additional_info_ptr =
               (uint8 *)GSTK_CALLOC(additional_info_ptr.length)))
  {
    return;
  }
  if (additional_code > 0xFF)
  {
    (void)memscpy(additional_info_ptr.additional_info_ptr,
                  additional_info_ptr.length,
                  &additional_code,
                  sizeof(additional_code));
  }
  else
  {
    *(additional_info_ptr.additional_info_ptr) = (uint8)additional_code;
  }

  /* 2. Set up response type */
  for (i = 0;
        i < (sizeof(pro_cmd_rsp_map)/sizeof(pro_cmd_rsp_map[0]));
        i++)
  {
    if (gstk_req_rsp_ptr->hdr_cmd.command_id == pro_cmd_rsp_map[i].cmd_enum)
    {
      cnf_type = pro_cmd_rsp_map[i].rsp_enum;
      break;
    }
  }/* end for i */

  if (GSTK_CMD_ENUM_NOT_USE == cnf_type)
  {
    /* Unhandled Command */
    if (additional_info_ptr.additional_info_ptr)
    {
      gstk_free((void *)(additional_info_ptr.additional_info_ptr));
      additional_info_ptr.additional_info_ptr = NULL;
    }
    return;
  }/* end Unhandled Command */

  /* 3. Send Error TR */
  (void)secure_memset(&other_info, 0x00, sizeof(other_info));
  (void) gstk_send_terminal_response_ext(
                        estk_curr_inst_ptr->gstk_client_id,
                        0x00,
                        gstk_req_rsp_ptr->hdr_cmd.cmd_detail_reference,
                        gstk_req_rsp_ptr->hdr_cmd.command_number,
                        cnf_type,
                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                        &additional_info_ptr,
                        &other_info);

  if (additional_info_ptr.additional_info_ptr)
  {
    gstk_free((void *)(additional_info_ptr.additional_info_ptr));
    additional_info_ptr.additional_info_ptr = NULL;
  }
}/* estk_reject_new_proactive_cmd() */

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
void estk_stop(void)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}/* estk_stop */

/*===========================================================================
FUNCTION: estk_disable_tp_and_registration

DESCRIPTION:
  This function will disable the Voice, DTMF, SS, USSD, SMS and BIP related bits in the TP download and
  also unregisters for these functionlities based on the NV values set.

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
static void estk_disable_tp_and_registration(void)
{
  uint8          i                  = 0;
  uint8          n                  = 0;
  boolean        cmd_enable         = FALSE;
  boolean        is_estk_bip_enable = FALSE;
  boolean        is_lte_available   =
                         gstk_util_find_modem_capability(GSTK_MODEM_CAPABILITY_FEATURE_LTE);

  if((gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_ME_DISABLE_ESTK_BIP_INTERFACE, GSTK_SLOT_ID_MAX) == TRUE))
  {
    is_estk_bip_enable = TRUE;
  }

  gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_BIP, &cmd_enable);

  n = sizeof(estk_profile_value_list) / sizeof(gstk_profile_dl_support_ext_type);

  for(i = 0; i < n; i++)
  {
    switch(estk_profile_value_list[i].profile_dl_bitmask)
    {
    /* Disables SS and USSD service support in TP based on the NV*/
      case GSTK_SEND_SS_SUPPORT:
      case GSTK_SEND_USSD_SUPPORT:
      case GSTK_USSD_DO_IN_CC_SUPPORT:
        if(gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE,
             GSTK_SLOT_ID_MAX) == TRUE)
        {
           estk_profile_value_list[i].value = 0;
        }
        break;
      /* Disables Voice and DTMF service support in TP based on the NV*/
      case GSTK_SET_UP_CALL_SUPPORT:
      case GSTK_SEND_DTMF_SUPPORT:
      case GSTK_CSD_SUPPORT:
      case GSTK_MT_CALL_EVT_SUPPORT:
      case GSTK_CALL_CONNECTED_EVT_SUPPORT:
      case GSTK_CALL_DISCONNECTED_EVT_SUPPORT:
      case GSTK_SUPPORT_FOR_ALPHA_IN_CC:
        if(gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE,
             GSTK_SLOT_ID_MAX) == TRUE)
        {
          estk_profile_value_list[i].value = 0;
        }
        break;
      /* Disables SMS service support in TP based on the NV*/
      case GSTK_CDMA_SEND_SMS_SUPPORT:
      case GSTK_CELL_BROADCAST_DATA_DL_SUPPORT:
      case GSTK_MO_SMS_CONTROL_SUPPORT:
      case GSTK_SMS_PP_DL_SUPPORT:
      case GSTK_SEND_SHORT_MSG_SUPPORT:
      case GSTK_CDMA_SMS_PP_SUPPORT:
        if(gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_ME_DISABLE_ESTK_WMS_INTERFACE,
             GSTK_SLOT_ID_MAX) == TRUE)
        {
          estk_profile_value_list[i].value = 0;
        }
        break;
      /* Disables BIP service support in TP based on the NV*/
      case GSTK_DATA_AVAIL_EVT_SUPPORT:
      case GSTK_CH_STATUS_EVT_SUPPORT:
      case GSTK_OPEN_CH_SUPPORT:
      case GSTK_CLOSE_CH_SUPPORT:
      case GSTK_RECEIVE_DATA_SUPPORT:
      case GSTK_SEND_DATA_SUPPORT:
      case GSTK_GET_CH_STATUS_SUPPORT:
      case GSTK_NUM_CH_SUPPORT:
      case GSTK_TCP_SUPPORT:
      case GSTK_UDP_SUPPORT:
      case GSTK_GPRS_SUPPORT:
      case GSTK_USAT_HDPA_CLASS_E_SUPPORT_BYTE_17_BIT_8:
      case GSTK_USAT_UTRAN_PS_EXT_PARAM_CLASS_E_SUPPORT_BYTE_22_BIT_1:
      case GSTK_TCP_SERVER_MODE_SUPPORT:
        if((!cmd_enable) || (is_estk_bip_enable))
        {
          estk_profile_value_list[i].value = 0;
        }
        break;
      /* Disables BIP service support in TP based on the NV and LTE availability*/
      case GSTK_USAT_EUTRAN_CLASS_E_SUPPORT_BYTE_17_BIT_7:
        if((!cmd_enable) || (is_estk_bip_enable) || (!is_lte_available))
        {
          estk_profile_value_list[i].value = 0;
          UIM_MSG_LOW_0("Disable TP BYTE_17_BIT_7");
        }
        break;

      default:
        break;
   }
  }
}
#endif /* FEATURE_ESTK */
