/*===========================================================================
FILE:  estk_cm_call.c


GENERAL DESCRIPTION : ENHANCED STK layer SetUp call and Send DTMF support

          Copyright © 2008-2015 QUALCOMM Technologies, Inc (QTI).
          All Rights Reserved.
          QUALCOMM Technologies Proprietary
=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_cm_call.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/16   vr      Allocating memory to additional info 2 ptr
05/30/16   gs      F3 logging improvements
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
09/07/15   gs      Move to cm_util_default_gw_orig_params_per_sub API
05/21/15   lm      UT framework changes
01/14/15   vr      Call orig after call end event only if setup call is with
                   GSTK_DISCONNECT_ACTIVE_CALLS
10/14/14   kr      Fix for memleaks due to DTMF handling
10/07/14   shr     Fix code alignment
10/01/14   bd      Removed feature GSTK_CFG_FEATURE_ME_ESTK_ICON
09/16/14   bd      Fixed the check for dcs validation
09/09/14   vr      Reduce stack usage
09/09/14   bd      Features clean up
07/21/14   sw      Display alpha response changes
07/11/14   shr     Add Support for NV Refresh
07/08/14   dy      Fixing compiler warnings
07/02/14   dy      Added missing break statements
07/01/14   kr      Removing tmc from UIM codebase
06/17/14   dy      Remove GSTK RAT use in ESTK setup call
06/17/14   bd      Remove unnecessary compiler directive usage
06/06/14   vr      Ignoes DTMF events if it is not requested by itself
05/27/14   bd      Changes to hold null alpha and no alpha indications
05/27/14   dy      Remove usage of gstk_util_get_curr_access_tech() in ESTK
05/26/14   vr      Initialize gw_orig_params_ptr using CM API
05/09/14   bd      Fixed UT related SEND DTMF issue for 3GPP RATs
05/06/14   gs      Removed the ESTK featurization
05/06/14   vr      Send send_dtmf alpha after preprocessing
04/03/14   vr      Do not send Alpha to client when IGNORE_ALPHA is enabled
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/05/14   dy      Fix double free of temp_DTMF_str_ptr
02/26/14   sw      Memory leak fixes
01/30/14   vr      SIM initiated call control handling
01/28/13   dy      Added support for UT network cmd testing
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/02/13   vr      Check for setup call is present or not on slot_owns_cm_cli
                   after call end event
09/23/13   vr      Fix crash because of alpha zero length in voice to ussd cc
09/13/13   gm      Support for Recovery and hot-swap
09/06/13   hh      Clear DTMF private data upon Call End and DTMF completion
09/05/13   hh      Fix UT failure due to implementation dependent signedness
09/03/13   gm      Fix to originate call on 3GPP2 RAT when access tech is 3GPP2
08/16/13   sw      Correction of USSD string length in estk_copy_result_from_cc()
08/26/13   vr      Fix for SETUP CALL with DTMF tones cleanup
08/06/13   vr      Centralized Call Control changes
07/15/13   gs      Remove NV 5770 dependency for STK related tests except for GSM
05/23/13   gm      compilation error in MSG_ERROR macro call
05/23/13   gm      handling duplicate user confirmation
05/20/13   sw      Update slot id for CM DTMF inband tone events
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/11/13   bd      Support for (e)HRPD access tech
12/04/12   hn      Resolve compilation error for FEATURE_DUAL_SIM
10/19/12   hk      Memory Optimization in ESTK through F3 removal
05/16/12   dd      Use CM event to start DTMF timer
05/01/12   dd      Send Usr Cnf message even when alpha is null
04/13/12   bd      Fixed incorrect Alpha in CC request action TLV of TR
04/10/12   bd      Enhancements for location information in CC
03/28/12   av      Replaced feature flag with NVITEM
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/30/12   bd      Additional logging & KW fixes
12/23/11   bd      Support for call control by card modifying the proactive
                   command SETUP CALL into a USSD
12/08/11   sg      Move ESTK-CM call event handling outside feature
11/29/11   dd      Additional info in TR if hold active calls fails
11/17/11   bd      Support for voice call modified to emergency call due to
                   call control
11/02/11   bd      Fixed merge error
10/18/11   bd      Fixed incorrect cmd_ref_id and indentation
09/14/11   xz      Fix the issue of SETUP CALL failure when redial is absent
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/16/11   dd      Set the additional info in TR for redial
08/08/11   sg      Send DTMF crash as dtmf_len is not set to 0 on cleanup
07/06/11   sg      Mark 112 call in CDMA as Voice and not emergency
06/15/11   ea      Fix when STK Redial Duration TLV is absent
06/13/11   sg      Move FEATURE_ESTK_IGNORE_ALPHA to NV
06/08/11   nb      Passed correct slot to CM events
06/02/11   js      Fix issue for null pointer access in SEND DTMF request
05/27/11   nb      Map slot from as_id for posting CM events
05/03/11   dd      Do not change ESTK state until command is validated
04/28/11   nk      Fixed initlization warnings
04/20/11   dd      Thread safe change to CM events
04/19/11   nb      Update call connected global with CM events
03/29/11   ea      Added STK Redial support
03/28/11   dd      Wait for icon_disp_rsp for network commands
01/31/11   dd      Add user confirmation with icon for BIP
01/31/11   sg      Set pointer variable to NULL after calling gstk_free
01/13/11   adp     SubAddress length can be zero for originating a GW call
01/10/11   xz      Fix compilation error/warning
12/23/09   adp     Allow to setup voice call when data call is active
12/07/10   adp     Fixed CCAT SETUP Call issue
11/01/10   yt      Including private GSTK header
12/01/10   adp     All calls originated because of a Proactive command will be
                   treated as non emergency, except 112, per spec
09/23/10   xz      KW Fixes
07/19/10   dd      Add tag to the raw and decoded hdr for Set Up Call
07/09/10   xz      Fix compilation error
06/30/10   dd      Icon display confirmation
05/27/09   bd      Calling new interface of CM for not to check FDN list
05/05/10   bd      Fixed issue of not sending STOP DTMF
04/28/10   bd      Fixed the issue of handling null data object in ALPHA TLV
04/26/10   bd      Fixed terminal response issue in case of call not allowed
04/08/10   dd      Fix issue of cmd leak
03/19/10   xz      Reset call_event_info struct to fix a crash
03/15/10   xz      Claim CM client ownership when process SEND DTMF
02/19/10   xz      Fix compilation error
02/15/10   nb      Moved Dual Slot Changes
10/13/09   kp      Fix for the compilation error
09/29/09   xz      Fix issue of display confirmation alpha if qualifier is
                   "only if not currently busy on another call"
09/26/09   kp      ZI memory reduction changes
09/24/09   sg      Pass cmd_ref_id for ESTK_CM_CALL_LIST_CMD
07/29/09   xz      1) Add support of CCP
                   2) Fix issue of unable to send error TR if timeout on
                      waiting for user confirmation
07/14/09   xz      Allow to setup voice call when data call is active
03/26/09   xz      Fix compilation warnings
01/29/09   xz      Fix compilation issue
01/20/09   xz      Fix issue of sending DTMF
11/03/08   sk      Featurized ESTK based on external interface support
09/16/08   sk      Added lint fixes
09/08/08   tml     Fixed auto header version generation
06/09/08   sk      Added support for SetUp Call and Send DTMF commands

=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>
#include "uim_msg.h"

#if defined(FEATURE_ESTK)
#include "estk_cm.h"
#include "gstk_p.h"

#if defined(FEATURE_UIM_TEST_FRAMEWORK)
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                             DEFINES

===========================================================================*/

#define ESTK_MIN(A,B) ( (A) < (B) ? (A):(B))
#define ESTK_MAX_ALPHA_LEN_FOR_CALL_ORIG CM_MAX_ALPHA_LEN_FOR_CALL_ORIG
#define ESTK_USER_BUSY_ADDL_INFO 0x91
#define ESTK_FACILITY_REJECTED 0x9D

/*===========================================================================

                             DECLARATIONS

===========================================================================*/
extern boolean estk_switch_to_instance(gstk_slot_id_enum_type slot_id);

extern void gstk_util_dump_byte_array(
  const char  *array_name_ptr,
  const byte  *byte_array_ptr,
  uint32      array_size
);

/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/
static void estk_cm_inband_err_cb(
  void * data_block_ptr,
  cm_inband_cmd_e_type inband_cmd,
  cm_inband_cmd_err_e_type inband_cmd_err);

static boolean estk_cm_orig_call(const gstk_cmd_from_card_type *cmd_ptr);

static boolean estk_is_any_call_active(void);

static estk_result_enum_type estk_process_confirmation_alpha (
  gstk_cmd_from_card_type *gstk_req_ptr,
  boolean                  *cmd_has_alpha_ptr);


static estk_result_enum_type estk_process_call_alpha (
  gstk_cmd_from_card_type *gstk_req_ptr,
  boolean                 *cmd_has_alpha_ptr);

static boolean estk_setup_call_redial_needed(void);

static estk_result_enum_type estk_check_if_redial_parameters_present(
  gstk_cmd_from_card_type  *gstk_req_ptr);

static void estk_clear_dtmf_str(void);

static cm_call_mode_info_e_type estk_map_cm_sys_mode_to_call_mode (
  sys_sys_mode_e_type cm_sys_mode);

/*=============================================================================
FUNCTION: estk_call_err_cmd_cb

DESCRIPTION:
  Command callback from CM to report the status of queuing a request to CM

PARAMETERS:
  *data_block_ptr : [Input]  estk data block pointer
  call_cmd        : [Input]  cm command type
  call_cmd_err    : [Input]  cm command error type

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void estk_call_err_cmd_cb (
  void                   *data_block_ptr,
  cm_call_cmd_e_type     call_cmd,
  cm_call_cmd_err_e_type call_cmd_err
)
{
  estk_cmd_type *task_cmd_ptr  = NULL;
  uint32         cmd_ref_id    = 0;
  if(!data_block_ptr)
  {
    UIM_MSG_ERR_0("data_block_ptr is NULL ");
    return;
  }

  if (!ESTK_IS_VALID_SLOT_ID(estk_shared_info.slot_owns_cm_cli))
  {
    UIM_MSG_ERR_1("estk_call_err_cmd_cb: invalid slot id 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return;
  }

  if (!estk_instances_ptr[(uint8)estk_shared_info.slot_owns_cm_cli-1]
      ->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_0("estk_call_err_cmd_cb: No CM cmd");
    return;
  }

  cmd_ref_id = *((uint32*)data_block_ptr);

  UIM_MSG_MED_1("data_block_ptr is 0x%x ", data_block_ptr);

  if (call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    UIM_MSG_ERR_2("estk_call_err_cmd_cb received result 0x%x for Call command %d",
                  call_cmd_err, call_cmd);
    /* Queue a fail command to ESTK for set up call since it is possible
       for this callback to be invoked in the CM context as well as in the
       client context
    */
    task_cmd_ptr = estk_get_cmd_buf();
    if(task_cmd_ptr == NULL)
    {
      return;
    }
    switch(call_cmd)
    {
    case CM_CALL_CMD_INFO_LIST_GET:
      task_cmd_ptr->hdr.cmd                         = ESTK_CM_CALL_LIST_CMD;
      break;
    case CM_CALL_CMD_ORIG:
      task_cmd_ptr->hdr.cmd                          = ESTK_CM_CALL_EVENT_CMD;
      task_cmd_ptr->cmd_data.call_event_info.evt_id  = ESTK_CM_CALL_ORIG_EVT;
      task_cmd_ptr->cmd_data.call_event_info.call_id = estk_shared_info.call_info.call_id;
      break;
    case CM_CALL_CMD_END:
      task_cmd_ptr->hdr.cmd                          = ESTK_CM_CALL_EVENT_CMD;
      task_cmd_ptr->cmd_data.call_event_info.evt_id  = ESTK_CM_CALL_END_EVT;
      task_cmd_ptr->cmd_data.call_event_info.call_id = estk_shared_info.call_info.call_id;
      break;
    default:
      /* free the task_cmd_ptr */
      estk_free_cmd_buf(task_cmd_ptr);
      return;
    }/* End of switch */
    task_cmd_ptr->hdr.cmd_ref_id    = cmd_ref_id;
    task_cmd_ptr->hdr.cli_user_data = 0;
    task_cmd_ptr->hdr.sim_slot_id   = estk_shared_info.slot_owns_cm_cli;
    if (CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED == call_cmd_err)
    {
      /* If call is rejected, lets have result as unsupported */
      task_cmd_ptr->result = ESTK_UNSUPPORTED;
    }
    else
    {
      task_cmd_ptr->result = ESTK_ERROR;
    }
    estk_put_cmd(task_cmd_ptr);
  }/* if (call_cmd_err != CM_CALL_CMD_ERR_NOERR) */
}/* estk_call_err_cmd_cb */

/*=============================================================================
FUNCTION: estk_call_err_cmd_cb_exten3

DESCRIPTION:
  Command callback from CM to report the status of queuing a request to CM

PARAMETERS:
  *data_block_ptr : [Input]  estk data block pointer
  call_cmd        : [Input]  cm command type
  call_cmd_err    : [Input]  cm command error type

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void estk_call_err_cmd_cb_exten3(
  void                          *data_block_ptr,
  cm_call_cmd_e_type            call_cmd,
  cm_call_cmd_err_e_type        call_cmd_err,
  cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
  cm_alpha_s_type               alpha
)
{
  (void)call_cmd_err_cause;
  (void)alpha;
  estk_call_err_cmd_cb(data_block_ptr, call_cmd, call_cmd_err);
} /* estk_call_err_cmd_cb_exten3 */

/*===========================================================================

FUNCTION: estk_process_setup_call_req

DESCRIPTION:
  GSTK calls this ESTK function to process the set up call request.After
  receiving this command from GSTK, the command is parsed for dtmf digits
  and a subsequent command is queued to CM. If a request to place the call is
  successfully placed in the CM queue, ESTK then waits to receive the call
  connected event in order to send a Terminal Response. Other error events from
  CM received with respect to the call will be considered as a failure to set up
  the call and ESTK will send a suitable error Terminal Response to GSTK.

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS: If the processing of the command was successful
    ESTK_ERROR  : If the processing of the command was unsuccessful

COMMENTS:
  The caller function will return a Terminal Response to GSTK if the
  return value from this function indicates any error.

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_call_req(
  gstk_cmd_from_card_type      *gstk_req_ptr)
{
  uint32   cmd_ref_id     = 0;
  boolean  cmd_has_alpha  = FALSE;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL ptr");
    return ESTK_ERROR;
  }

  if (estk_shared_info.slot_owns_cm_cli != GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("CM client is busy (0x%x)!", estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }
  else
  {
    estk_shared_info.slot_owns_cm_cli = gstk_req_ptr->hdr_cmd.sim_slot_id;
  }

  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  /* Initialize the result to GSTK_COMMAND_PERFORMED_SUCCESSFULLY*/
  estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

  (void)estk_check_if_redial_parameters_present(gstk_req_ptr);

  UIM_MSG_HIGH_2("call_setup_requirement = 0x%x, user_cnf_alpha.length = 0x%x",
                 gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement,
                 gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);

  switch (gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement)
  {
    case GSTK_HOLD_ACTIVE_CALLS:
    case GSTK_DISCONNECT_ACTIVE_CALLS:
      break;
    case GSTK_NO_OTHER_CALLS:
      if(cm_mm_call_cmd_get_call_info_list(
           estk_call_err_cmd_cb,
           (void*)&(gstk_req_ptr->hdr_cmd.cmd_detail_reference),
           estk_shared_info.cm_client_id) == FALSE)
      {
        UIM_MSG_ERR_0("cm_mm_call_cmd_get_call_info_list() failed");
        return ESTK_ERROR;
      }
      estk_curr_inst_ptr->state = ESTK_PREPROCESS_COMMAND_ST;
      return ESTK_SUCCESS;
    default:
      UIM_MSG_ERR_1("Unknown setup requirement 0x%x",
                    gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement);
      return ESTK_ERROR;
  }/* End of switch */

  if (estk_process_confirmation_alpha(gstk_req_ptr, &cmd_has_alpha) != ESTK_SUCCESS)
  {
    return ESTK_ERROR;
  }

  /* Preprocess calling criteria and set up a call */
  if (!cmd_has_alpha)
  {
    return estk_cm_preprocess_call_orig_params(gstk_req_ptr);
  }

  return ESTK_SUCCESS;
}/* estk_process_send_setup_call_req */


/*=============================================================================
FUNCTION: estk_process_confirmation_alpha

DESCRIPTION:
  If command has alpha, try to send it to UI for confirmation.

PARAMETERS:
  gstk_req_ptr :      [Input]  pointer to SETUP CALL command
  cmd_has_alpha_ptr : [Output] indicator of whether or not command has alpha

DEPENDENCIES:
  None

RETURN VALUE:


COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
estk_result_enum_type estk_process_confirmation_alpha (
  gstk_cmd_from_card_type *gstk_req_ptr,
  boolean                 *cmd_has_alpha_ptr
)
{
  if (gstk_req_ptr == NULL || cmd_has_alpha_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL ptr: 0x%x, 0x%x", gstk_req_ptr, cmd_has_alpha_ptr);
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("Alpha length is 0x%x, icon present is 0x%x",
                 gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length,
                 gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_icon.present);

  *cmd_has_alpha_ptr = FALSE;

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == TRUE)
  {
    UIM_MSG_HIGH_0("GSTK_CFG_FEATURE_ESTK_IGNORE_ALPHA is ON");
    return ESTK_SUCCESS;
  }

  /* Process Alpha identifier (call set up phase) */
  if((gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length) > 1 &&
      (gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text))
  {
    *cmd_has_alpha_ptr = TRUE;

    /* Check for DCS validity */
    if(((gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length) > 0 &&
      (gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text)) &&
       ((gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.dcs != 0x04) &&
       (gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.dcs != 0x08)))
    {
      UIM_MSG_ERR_1("Unknown dcs 0x%x",
                    gstk_req_ptr->cmd.setup_call_pro_cmd_req.user_cnf_alpha.dcs);
      return ESTK_ERROR;
    }

    /* Check to see if any client has registered to handle UI related
       functionality
    */
    return estk_send_alpha_to_display_client(gstk_req_ptr, FALSE);
  }/* if alpha is present and not null */
  else if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
                                      GSTK_SLOT_ID_MAX) == TRUE)
  {
    *cmd_has_alpha_ptr = TRUE;
    return estk_send_alpha_to_display_client(gstk_req_ptr, FALSE);
  }

  return ESTK_SUCCESS;
} /* estk_process_confirmation_alpha */

/*===================================================================================
FUNCTION: estk_process_call_alpha

DESCRIPTION:
  If command has Alpha identifier (call set up phase) TLV, try to send it to UI to be
  displayed during the call.

PARAMETERS:
  gstk_req_ptr :      [Input]  pointer to SETUP CALL command
  cmd_has_alpha_ptr : [Output] indicator of whether or not command has alpha

DEPENDENCIES:
  None

RETURN VALUE:


COMMENTS:
  None

SIDE EFFECTS:
  None

===================================================================================*/
estk_result_enum_type estk_process_call_alpha (
  gstk_cmd_from_card_type *gstk_req_ptr,
  boolean                 *cmd_has_alpha_ptr
)
{
  if (gstk_req_ptr == NULL || cmd_has_alpha_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL ptr: 0x%x, 0x%x", gstk_req_ptr, cmd_has_alpha_ptr);
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("Alpha length is 0x%x, icon present is 0x%x",
                 gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length,
                 gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == TRUE)
  {
    UIM_MSG_HIGH_0("GSTK_CFG_FEATURE_ESTK_IGNORE_ALPHA is ON");
    *cmd_has_alpha_ptr = FALSE;
    return ESTK_SUCCESS;
  }

  *cmd_has_alpha_ptr = FALSE;
  if((gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length) > 1 &&
      (gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text))
  {
    *cmd_has_alpha_ptr = TRUE;

    /* Check for DCS validity */
    if(((gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length) > 0 &&
      (gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text)) &&
       ((gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.dcs != 0x04) &&
       (gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.dcs != 0x08)))
    {
      UIM_MSG_ERR_1("Unknown dcs 0x%x",
                    gstk_req_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.dcs);
      return ESTK_ERROR;
    }

    /* Check to see if any client has registered to handle UI related
       functionality
    */
    return estk_send_alpha_to_display_client(gstk_req_ptr, TRUE);
  }/* if alpha is present and not null */
  else if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
                                      GSTK_SLOT_ID_MAX) == TRUE)
  {
    *cmd_has_alpha_ptr = TRUE;
    return estk_send_alpha_to_display_client(gstk_req_ptr, TRUE);
  }

  return ESTK_SUCCESS;
} /* estk_process_call_alpha */

/*===========================================================================

FUNCTION: estk_cm_preprocess_call_orig_params

DESCRIPTION :
  This ESTK function will look at the calling criteria such as
    1. No other calls
    2. Disconnect all other calls
    3. Putting existing calls on hold
  and call the appropriate CM API.

PARAMETERS:
  gstk_req_ptr [Input]: GSTK request pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing call criteria is successful
    ESTK_ERROR   : If processing call criteria is not successful

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_cm_preprocess_call_orig_params(
   gstk_cmd_from_card_type   *gstk_req_ptr)
{
  boolean cmd_has_alpha;
  if(gstk_req_ptr ==  NULL)
  {
    UIM_MSG_ERR_0("Null Input Part gstk_req_ptr");
    return ESTK_ERROR;
  }
  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  switch(gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement)
  {
    case GSTK_HOLD_ACTIVE_CALLS:
    case GSTK_NO_OTHER_CALLS:
      break;
    case GSTK_DISCONNECT_ACTIVE_CALLS:
      if(cm_mm_call_cmd_get_call_info_list(
           estk_call_err_cmd_cb,
           (void*)&(gstk_req_ptr->hdr_cmd.cmd_detail_reference),
           estk_shared_info.cm_client_id) == FALSE)
      {
        UIM_MSG_ERR_0("cm_mm_call_cmd_get_call_info_list failed");
        return ESTK_ERROR;
      }
      estk_curr_inst_ptr->state = ESTK_PREPROCESS_COMMAND_ST;
      return ESTK_SUCCESS;
    default:
      UIM_MSG_ERR_1("Unknown setup requirement 0x%x",
                    gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement);
      return ESTK_ERROR;
  }/* End of switch */

  if(estk_cm_orig_call(gstk_req_ptr) == FALSE)
  {
    /* Return error for caller to send error TR to GSTK */
    return ESTK_ERROR;
  }

  /* Process Alpha identifier (call set up phase) */
  if (estk_process_call_alpha(gstk_req_ptr, &cmd_has_alpha) != ESTK_SUCCESS)
  {
    /* Return error for caller to send error TR to GSTK */
    return ESTK_ERROR;
  }
  return ESTK_SUCCESS;
}/* estk_cm_preprocess_call_orig_params */

/*===========================================================================

FUNCTION: estk_process_setup_call_user_cnf

DESCRIPTION :
  This function will be called by ESTK to process the user confirmation response
  received from the UI client.If the user confirmed for an affirmative action,
  the call will be set up, otherwise an appropriate Terminal Response will be
  sent to the card saying "The user rejected the call setup".
  If the user fails to respond within a certain time, a Terminal response of
  "No response from the user will be sent to the card.

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
estk_result_enum_type estk_process_setup_call_user_cnf_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  uint32                         cmd_ref_id                   = 0;
  gstk_cmd_from_card_type        *gstk_req_ptr                = NULL;
  gstk_general_result_enum_type  gen_rslt                     = GSTK_GENERAL_RESULT_MAX;
  boolean                        send_tr                      = FALSE;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  /* Step 1: Check input parameters */
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SETUP_CALL_REQ)
  {
    UIM_MSG_ERR_1("Incorrect command type 0x%x",
                  estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
    return ESTK_ERROR;
  }

  if(cmd_ptr->hdr.cmd != ESTK_GENERAL_USER_CNF_CMD)
  {
    UIM_MSG_ERR_1("Incorrect command id 0x%x", cmd_ptr->hdr.cmd);
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->state != ESTK_WAIT_FOR_USER_CNF_ST)
  {
    UIM_MSG_ERR_0("Ignoring Second User confirmation");
    return ESTK_ERROR;
  }

  /* reset the timer */
  (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));

  /* Since the reply was received, return the state back to
    preprocess */
  estk_curr_inst_ptr->state =  ESTK_PREPROCESS_COMMAND_ST;

  /* Step 2: Check user response. If User said "No", send TR and do not
     originate the call
  */
  estk_curr_inst_ptr->icon_disp = cmd_ptr->cmd_data.user_rsp.icon_disp;

  if(cmd_ptr->cmd_data.user_rsp.yes_no_rsp == 0)
  {
    gen_rslt = GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST;
    send_tr  = TRUE;
  }
  else /* If user responded with a yes */
  {
    /* Step 3: Check user response. If User said "Yes", originate the call
       through CM
    */
    gstk_req_ptr = &(estk_curr_inst_ptr->curr_cmd);

    if(estk_cm_preprocess_call_orig_params(gstk_req_ptr) == ESTK_ERROR)
    {
      gen_rslt = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      send_tr  = TRUE;
    }/* if(estk_cm_orig_call(gstk_req_ptr) == ESTK_ERROR) */
  }/* if(cmd_ptr->cmd_data.yes_no_rsp == 0) */

  if(send_tr)
  {
    (void)estk_send_terminal_response(
        cmd_ref_id,            /* command_details_ref */
        GSTK_SETUP_CALL_CNF,   /* command response */
        gen_rslt,              /* general_result */
        NULL,                  /* additional result */
        NULL                   /* extra param */ );
  }/* if(send_tr) */

  return ESTK_SUCCESS;
}/* estk_process_setup_call_user_cnf */

/*===========================================================================

FUNCTION: estk_cm_call_info_list_cb

DESCRIPTION :
  Callback function invoked as result of cm_mm_call_cmd_get_call_info_list().

PARAMETERS:
  list_ptr : [Input] Active call list from CM

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_call_info_list_cb (
  const cm_call_state_info_list_s_type *list_ptr
)
{
  estk_cmd_type          *task_cmd_ptr = NULL;
  int                    i             = 0;
  int                    j              = 0;
  gstk_slot_id_enum_type slot_id       = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_2("estk_cm_call_info_list_cb(): list_ptr=0x%x, "
                 "estk_shared_info.slot_owns_cm_cli=0x%x",
                 list_ptr, estk_shared_info.slot_owns_cm_cli);

  slot_id = estk_shared_info.slot_owns_cm_cli;
  if (!ESTK_IS_VALID_SLOT_ID(slot_id) || slot_id == GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot_id);
    return;
  }

  if (!estk_instances_ptr[(uint8)slot_id - 1]->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_0("estk_cm_call_info_list_cb: No CM cmd");
  }

  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->hdr.cli_user_data                              = 0;
  task_cmd_ptr->hdr.cmd                                        = ESTK_CM_CALL_LIST_CMD;
  task_cmd_ptr->hdr.sim_slot_id                                = slot_id;
  task_cmd_ptr->cmd_data.call_list_info.number_of_active_calls = 0;
  task_cmd_ptr->hdr.cmd_ref_id                                 =
    estk_instances_ptr[(uint8)slot_id - 1]->curr_cmd.hdr_cmd.cmd_detail_reference;

  if(list_ptr == NULL)
  {
    task_cmd_ptr->result                    = ESTK_ERROR;
    estk_put_cmd(task_cmd_ptr);
    return;
  }

  for(i = 0;
      (i < list_ptr->number_of_active_calls) && (i < ESTK_CALL_ID_MAX);
      i++)
  {
    UIM_MSG_HIGH_3("call_type = 0x%x, call_state = 0x%x, call_id = 0x%x",
                   list_ptr->info[i].call_type,
                   list_ptr->info[i].call_state,
                   list_ptr->info[i].call_id);
    if((list_ptr->info[i].call_type == CM_CALL_TYPE_VOICE) ||
       (list_ptr->info[i].call_type == CM_CALL_TYPE_CS_DATA))
    {
      if((list_ptr->info[i].call_state == CM_CALL_STATE_IDLE) ||
         (list_ptr->info[i].call_state == CM_CALL_STATE_NONE) ||
         (list_ptr->info[i].call_state == CM_CALL_STATE_MAX))
      {
        continue;
      }
      else
      {
        task_cmd_ptr->cmd_data.call_list_info.active_calls[j].call_id
          = list_ptr->info[i].call_id;
        task_cmd_ptr->cmd_data.call_list_info.active_calls[j].call_sys_mode
          = list_ptr->info[i].sys_mode;
        task_cmd_ptr->cmd_data.call_list_info.number_of_active_calls++;
        j+=1;
      }/* call state */
    }/* call type */
  }/* End of for */

  task_cmd_ptr->result  = ESTK_SUCCESS;
  estk_put_cmd(task_cmd_ptr);

}/* estk_cm_call_info_list_cb */

/*===========================================================================

FUNCTION: estk_end_all_calls

DESCRIPTION:
  This function issues a request to CM to end call(s).

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE:     The end request is sent, callback function
            will be called.
  FALSE:    The end request failed.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_end_all_calls(void)
{
  /* Hangup the call */
  cm_end_params_s_type           *end_params_ptr               = NULL;
  uint32                          index                        = 0;
  uint8                           i                            = 0;
  gstk_cmd_from_card_type        *gstk_req_ptr                 = NULL;
  boolean                         return_status                = FALSE;

  UIM_MSG_HIGH_0("In estk_end_all_calls()");

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, FALSE);
  gstk_req_ptr = &(estk_curr_inst_ptr->curr_cmd);

  end_params_ptr = (cm_end_params_s_type *)
                     GSTK_CALLOC(ESTK_CALL_ID_MAX * sizeof(cm_end_params_s_type));
  if(end_params_ptr == NULL)
  {
    return FALSE;
  }

  for ( index = 0; index < ESTK_CALL_ID_MAX; index++ )
  {
    if(estk_shared_info.call_info.call_list[index].call_active == TRUE)
    {
      memset( &end_params_ptr[i],
              CM_CALL_CMD_PARAM_DEFAULT_VALUE,
              sizeof ( cm_end_params_s_type ));
      end_params_ptr[i].info_type =
        estk_map_cm_sys_mode_to_call_mode(estk_shared_info.call_info.call_list[index].call_sys_mode);
      end_params_ptr[i].call_id = estk_shared_info.call_info.call_list[index].call_id;
      i++;
    }/* if(estk_curr_inst_ptr->call_list[index].call_active == TRUE) */
  }/* End of for */

  return_status = cm_mm_call_cmd_end(estk_call_err_cmd_cb,
                            &(gstk_req_ptr->hdr_cmd.cmd_detail_reference),
                            estk_shared_info.cm_client_id,
                            i,
                            end_params_ptr);
  gstk_free(end_params_ptr);
  end_params_ptr = NULL;
  return return_status;
} /* estk_end_all_calls */

/*===========================================================================

FUNCTION: estk_process_cm_call_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process CM events
  related to the following proactive commands.
  1. SETUP CALL

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of call event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_call_event_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  uint32                         cmd_ref_id           = 0;
  int                            i                    = 0;
  boolean                        send_rsp_to_gstk     = FALSE;
  gstk_additional_info_ptr_type *additional_info_ptr  = NULL;
  gstk_general_result_enum_type  gstk_rslt            = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  cm_call_end_e_type             end_status           = CM_CALL_END_NONE;
  uint8                          additional_info_data = 0;
  gstk_additional_info_ptr_type  additional_result    = {0x00};
  gstk_cmd_from_card_type        *gstk_req_ptr        = NULL;
  boolean                        cmd_has_alpha        = FALSE;
  gstk_slot_id_enum_type         slot                 = GSTK_SLOT_ID_MAX;
  boolean                        clear_dtmf           = FALSE;
  gstk_cmd_enum_type             cnf_type             = GSTK_SETUP_CALL_CNF;
  gstk_nv_items_data_type        nv_data;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  /* Step 1: Check input parameters */
  if(cmd_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  if(cmd_ptr->hdr.cmd != ESTK_CM_CALL_EVENT_CMD)
  {
    UIM_MSG_ERR_1("Incorrect command id 0x%x", cmd_ptr->hdr.cmd);
    return ESTK_ERROR;
  }
  gstk_req_ptr = &(estk_curr_inst_ptr->curr_cmd);
  slot = cmd_ptr->hdr.sim_slot_id;
  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return ESTK_ERROR;
  }

  /* Read Redial timer value */
  (void)gstk_nv_access_read(
          GSTK_NV_ME_REDIAL_CFG,
          GSTK_SLOT_ID_MAX,
          &nv_data);

  if(((cmd_ptr->result == ESTK_ERROR) || (cmd_ptr->result == ESTK_UNSUPPORTED)) &&
     ((estk_curr_inst_ptr->state == ESTK_PENDING_TR_ST) || (estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)) &&
     (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ))
  {
    if ((cmd_ptr->result == ESTK_ERROR) &&
        (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ) &&
        !(estk_setup_call_redial_needed()))
    {

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      }
      else
      {
        (void)estk_send_terminal_response(
                cmd_ref_id,                                       /* command_details_ref */
                GSTK_SETUP_CALL_CNF,                              /* command response */
                GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                NULL,                                             /* additional result */
                NULL);                                            /* extra param */
      }
      clear_dtmf = TRUE;
    }
    else if ((cmd_ptr->result == ESTK_UNSUPPORTED) &&
             (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ) &&
             !(estk_setup_call_redial_needed()))
    {
      /* Call control did not allow mo sms send or Call control error */
      additional_result.length = 1;
      additional_info_data = int32touint8((int32)GSTK_ACTION_NOT_ALLOWED);
      additional_result.additional_info_ptr =&additional_info_data;

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SETUP CALL error TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM;
        estk_cache_additional_info(&additional_result);
      }
      else
      {
        (void)estk_send_terminal_response(
                cmd_ref_id,                                                      /* command_details_ref */
                GSTK_SETUP_CALL_CNF,                                             /* command response */
                GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM, /* general_result */
                &additional_result,
                NULL);
      }
      clear_dtmf = TRUE;
    }
    else if(estk_setup_call_redial_needed())
    {
      UIM_MSG_HIGH_1("estk_process_cm_call_event_cmd(): setup reattempt timer for %d ms",
                     nv_data.redial_cfg);

      /* if redial needed, set flag to TRUE so estk_process_setup_call_req() again after timer expires */
      estk_shared_info.redial_delayed_cm_cmd = ESTK_REDIAL_CALL_REQ;

      /* set timer for reattempt */
      (void)rex_set_timer(&estk_shared_info.cm_cmd_reattempt_timer, nv_data.redial_cfg);

      /* set send_rsp_to_gstk so we do not send TR yet for redial case */
      send_rsp_to_gstk = FALSE;
    }
    UIM_MSG_ERR_1("estk_process_cm_call_event_cmd(): Unhandled result 0x%x!",
                  cmd_ptr->result);

    /* Clear DTMF digits and slot_owns_cm_cli */
    if (TRUE == clear_dtmf)
    {
      estk_clear_dtmf_str();
      estk_shared_info.call_info.call_id = 0xFF;
      estk_shared_info.call_info.call_sys_mode = SYS_SYS_MODE_NO_SRV;
      estk_shared_info.slot_owns_cm_cli = GSTK_SLOT_ID_MAX;
    }
    return ESTK_SUCCESS;
  }
  switch(cmd_ptr->cmd_data.call_event_info.evt_id)
  {
    case ESTK_CM_CALL_ORIG_EVT:
      UIM_MSG_HIGH_1("Received Call Orig Event for call id 0x%x",
                     cmd_ptr->cmd_data.call_event_info.call_id);
      break;

    case ESTK_CM_CALL_CONNECT_EVT:
      UIM_MSG_HIGH_3("Received Call Connect Event  cmd call id is 0x%x, estk call id is 0x%x, estk call type is 0x%x",
                     cmd_ptr->cmd_data.call_event_info.call_id,
                     estk_shared_info.call_info.call_id,
                     cmd_ptr->cmd_data.call_event_info.call_type);
      /*Update the estk_cm_cs_call_connected varibale only if the connected call is of CS type */
      if(cmd_ptr->cmd_data.call_event_info.call_type == (estk_cm_call_type_enum_type) CM_CALL_TYPE_VOICE ||
	     cmd_ptr->cmd_data.call_event_info.call_type == (estk_cm_call_type_enum_type) CM_CALL_TYPE_CS_DATA)
      {
        estk_instances_ptr[(uint8)slot - 1]->estk_cm_cs_call_connected++;
      }
      if(cmd_ptr->cmd_data.call_event_info.call_id == estk_shared_info.call_info.call_id)
      {
        if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ)
        {
          /* Send DTMF digits */
          if(estk_shared_info.call_info.dtmf_len > 0)
          {
            estk_send_DTMF(cmd_ref_id);
          }
        }

        /* Send Success Terminal Response */
        /* TODO: pending fix for STK app (CR169549),
         * comment out "if" for now */
        UIM_MSG_HIGH_1("In call connect event, ESTK state is 0x%x",
                       estk_curr_inst_ptr->state);

          send_rsp_to_gstk = TRUE;
          gstk_rslt = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      } /* call id belongs to ESTK */
      else
      {
        UIM_MSG_ERR_2("ESTK: Not my call 0x%x 0x%x ",
                      cmd_ptr->cmd_data.call_event_info.call_id,
                      estk_shared_info.call_info.call_id);
      }
      break;

    case ESTK_CM_CALL_END_EVT:
      UIM_MSG_HIGH_2("Received call end event for call id 0x%x, Active CS Calls 0x%x",
                     cmd_ptr->cmd_data.call_event_info.call_id,
                     estk_instances_ptr[(uint8)slot - 1]->estk_cm_cs_call_connected);
      if(cmd_ptr->cmd_data.call_event_info.call_type == (estk_cm_call_type_enum_type) CM_CALL_TYPE_VOICE ||
	     cmd_ptr->cmd_data.call_event_info.call_type == (estk_cm_call_type_enum_type) CM_CALL_TYPE_CS_DATA)
      {
         if(estk_instances_ptr[(uint8)slot - 1]->estk_cm_cs_call_connected != 0)
         {
           estk_instances_ptr[(uint8)slot - 1]->estk_cm_cs_call_connected--;
         }
      }
      if (!estk_switch_to_instance(estk_shared_info.slot_owns_cm_cli))
      {
        UIM_MSG_ERR_1("GSTK didn't have any CALL Request 0x%x",
                      estk_shared_info.slot_owns_cm_cli);
        return ESTK_ERROR;
      }

      if(estk_curr_inst_ptr->state == ESTK_PREPROCESS_COMMAND_ST)
      {
        if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SETUP_CALL_REQ)
        {
          UIM_MSG_ERR_1("unexpected estk state for cmd 0x%x",
                        estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
          return ESTK_ERROR;
        }
        for(i=0;i<ESTK_CALL_ID_MAX;i++)
        {
          if(cmd_ptr->cmd_data.call_event_info.call_id ==
            estk_shared_info.call_info.call_list[i].call_id)
          {
            if(estk_shared_info.call_info.call_list[i].call_active == TRUE)
            {
              estk_shared_info.call_info.call_list[i].call_active = FALSE;
            }
            else
            {
              UIM_MSG_ERR_1("Received end for call id 0x%x again!",
                            estk_shared_info.call_info.call_list[i].call_id);
              break;
            }
          }/* End of if */
        }/* End of for */
        if((estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.call_setup_requirement == 
               GSTK_DISCONNECT_ACTIVE_CALLS) &&
            estk_is_any_call_active() == FALSE)
        {
          if(estk_cm_orig_call( &(estk_curr_inst_ptr->curr_cmd)) == FALSE)
          {
            send_rsp_to_gstk = TRUE;
            clear_dtmf = TRUE;
          }
          else if (estk_process_call_alpha(gstk_req_ptr, &cmd_has_alpha) != ESTK_SUCCESS)
          {
            return ESTK_ERROR;
          }
        }
      }/* if(estk_curr_inst_ptr->state == ESTK_PREPROCESS_COMMAND) */
      else if((estk_curr_inst_ptr->state == ESTK_PENDING_TR_ST) ||
              (estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST))
      {
        if(cmd_ptr->cmd_data.call_event_info.call_id != estk_shared_info.call_info.call_id)
        {
          return ESTK_SUCCESS;
        }
        if(cmd_ptr->cmd_data.call_event_info.evt_payload.len > 0)
        {
          send_rsp_to_gstk =  TRUE;

          /* SETUP CALL REQ or
             SEND SS or SEND USSD and modified to voice call */
          if((estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ) ||
             ((estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_SS_REQ ||
               estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_USSD_REQ) &&
               estk_curr_inst_ptr->extra_param.present))
          {
            end_status = (cm_call_end_e_type)cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[0];
            UIM_MSG_HIGH_1("Call end status is 0x%x", end_status);
            switch(end_status)
            {
              case CM_CALL_END_CLIENT_END:
                gstk_rslt =
                  GSTK_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE;
                /* CLEAR DTMF DIGITS */
                clear_dtmf = TRUE;
                break;
              case CM_CALL_END_LL_CAUSE:
              case CM_CALL_END_NETWORK_END:
                /* check if redial is needed */
                if(estk_setup_call_redial_needed())
                {
                  /* if redial needed, set flag to TRUE so estk_process_setup_call_req() again after timer expires */
                  estk_shared_info.redial_delayed_cm_cmd = ESTK_REDIAL_CALL_REQ;

                  /* set timer for reattempt */
                  (void)rex_set_timer(&estk_shared_info.cm_cmd_reattempt_timer, nv_data.redial_cfg);

                  /* set send_rsp_to_gstk so we do not send TR yet for redial case */
                  send_rsp_to_gstk = FALSE;
                }
                else
                {
                  UIM_MSG_HIGH_0("Network unable to process command");
                  /* CLEAR DTMF DIGITS */
                  clear_dtmf = TRUE;
                  gstk_rslt = GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
                  additional_info_ptr = GSTK_CALLOC(sizeof(gstk_additional_info_ptr_type));
                  if (additional_info_ptr != NULL)
                  {
                    additional_info_ptr->length = 1;
                    if(cmd_ptr->cmd_data.call_event_info.evt_payload.len >= 2)
                    {
                      additional_info_data =
                        cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[1];
                    }
                    else if((gstk_nv_get_feature_status(
                               GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7,
                               estk_curr_inst_ptr->slot_id) == TRUE) &&
                            (estk_shared_info.call_info.hold_active_failed == TRUE) &&
                            (end_status == CM_CALL_END_LL_CAUSE))
                    {
                      additional_info_data = ESTK_FACILITY_REJECTED;
                    }
                    else
                    {
                      additional_info_data =
                        int32touint8((int32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
                    }
                    additional_info_ptr->additional_info_ptr = &additional_info_data;
                  }
                }
                break;
              default:
                UIM_MSG_HIGH_1("Unknown end status 0x%x", end_status);
                /* CLEAR DTMF DIGITS */
                clear_dtmf = TRUE;
                break;
            }/* End of switch */
          }/* set up call */
          else if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_DTMF_REQ)
          {
            /* CLEAR DTMF DIGITS, taken care by estk_send_terminal_response() */
            additional_info_ptr = GSTK_CALLOC(sizeof(gstk_additional_info_ptr_type));
            if(additional_info_ptr != NULL)
            {
              additional_info_ptr->length = 1;
              additional_info_data = int32touint8((int32)GSTK_NOT_IN_SPEECH_CALL);
              additional_info_ptr->additional_info_ptr = &additional_info_data;
            }
          }/* send dtmf */
        }/* End of if(cmd_ptr->cmd_data.call_event_info.evt_payload.len */
      } /* else if(estk_curr_inst_ptr->state == ESTK_PENDING_TR_ST) */
      else
      {
        if(cmd_ptr->cmd_data.call_event_info.call_id == estk_shared_info.call_info.call_id)
        {
          clear_dtmf = TRUE;
        }/* if ESTK call id */
      }
      break;

    case ESTK_CM_CALL_EVENT_MNG_CALLS_CONF_EVT:
      if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7, estk_curr_inst_ptr->slot_id) == TRUE)
      {
        UIM_MSG_HIGH_1("Received Call Evnt Mng calls conf evt, length is 0x%x",
                       cmd_ptr->cmd_data.call_event_info.evt_payload.len);
        if((estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SETUP_CALL_REQ) &&
           (estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.call_setup_requirement == GSTK_HOLD_ACTIVE_CALLS))
        {
          /* If there is a ss_error, this message should have size of 2 bytes */
          if(cmd_ptr->cmd_data.call_event_info.evt_payload.len == 2)
          {
            UIM_MSG_HIGH_2("error tag is 0x%x, error is 0x%x",
                           cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[0],
                           cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[1]);
            if((cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[0] == MN_SS_ERROR_TAG) &&
              (cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[1] == MN_CALL_HOLD_REJ))
            {
              UIM_MSG_HIGH_0("Setting estk_shared_info.call_info.hold_active_failed to TRUE");
              estk_shared_info.call_info.hold_active_failed = TRUE;
            }
          }
        }
      }
      else
      {
        UIM_MSG_HIGH_1("Received unhandled event 0x%x",
                       cmd_ptr->cmd_data.call_event_info.evt_id);
      }
      break;

    case ESTK_CM_CALL_MOD_TO_SS_EVT:
      if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SETUP_CALL_REQ)
      {
        break;
      }
      UIM_MSG_HIGH_2 ("Received call modified evt. ss_ref: 0x%x, ss_oper: 0x%x",
                      cmd_ptr->cmd_data.call_event_info.result_from_cc.voice_to_ss.ss_ref,
                      cmd_ptr->cmd_data.call_event_info.result_from_cc.voice_to_ss.ss_oper);
      if((cmd_ptr->cmd_data.call_event_info.call_id ==
            estk_shared_info.call_info.call_id) &&
         ((cmd_ptr->cmd_data.call_event_info.result_from_cc.call_control_result ==
            CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS) ||
         (cmd_ptr->cmd_data.call_event_info.result_from_cc.call_control_result ==
            CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD)))
      {
        estk_shared_info.call_info.call_id = 0xFF; /* not valid any more */
        estk_clear_dtmf_str();

        /* Allocate memory for sups_info_ptr & copy sups related info */
        estk_shared_info.sups_info_ptr =
          (estk_sups_info_type*) GSTK_CALLOC (sizeof(estk_sups_info_type));
        if (estk_shared_info.sups_info_ptr == NULL)
        {
          return ESTK_ERROR; /* Need to send error TR to card ?*/
        }
        estk_shared_info.sups_info_ptr->req_id =
          cmd_ptr->cmd_data.call_event_info.result_from_cc.voice_to_ss.ss_ref;
        estk_shared_info.sups_info_ptr->cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;

        if(cmd_ptr->cmd_data.call_event_info.result_from_cc.call_control_result ==
            CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD)
        {
          estk_shared_info.sups_info_ptr->ss_operation = ESTK_PROCESS_USSD_DATA;
        }
        else
        {
          switch(cmd_ptr->cmd_data.call_event_info.result_from_cc.voice_to_ss.ss_oper)
          {
             case activateSS:
               estk_shared_info.sups_info_ptr->ss_operation = ESTK_ACTIVATE_SS;
               break;
             case deactivateSS:
               estk_shared_info.sups_info_ptr->ss_operation = ESTK_DEACTIVATE_SS;
               break;
             case registerSS:
               estk_shared_info.sups_info_ptr->ss_operation = ESTK_REGISTER_SS;
               break;
             case eraseSS:
               estk_shared_info.sups_info_ptr->ss_operation = ESTK_ERASE_SS;
               break;
             case interrogateSS:
               estk_shared_info.sups_info_ptr->ss_operation = ESTK_INTERROGATE_SS;
               break;
             default:
               estk_shared_info.sups_info_ptr->ss_operation = ESTK_NULL_SS_OPERATION;
               break;
            }
        }

        memset(&estk_curr_inst_ptr->extra_param, 0,sizeof(estk_curr_inst_ptr->extra_param));
        estk_curr_inst_ptr->extra_param.present = TRUE;
        estk_curr_inst_ptr->extra_param.rsp_type = GSTK_SETUP_CALL_CNF;
        estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.has_cc_type_modification =
          TRUE;

      }
      else
      {
        UIM_MSG_HIGH_3("CALL_MOD_TO_SS_EVT cmd call id is 0x%x, estk call id is 0x%x, cc_result is 0x%x",
                       cmd_ptr->cmd_data.call_event_info.call_id,
                       estk_shared_info.call_info.call_id,
                       cmd_ptr->cmd_data.call_event_info.result_from_cc.call_control_result);
        send_rsp_to_gstk = TRUE;
      }
      break;

    default:
      UIM_MSG_HIGH_1("Received unhandled event 0x%x",
                     cmd_ptr->cmd_data.call_event_info.evt_id);
      break;
  }/* End of switch */
  if(send_rsp_to_gstk)
  {
    gstk_terminal_rsp_extra_param_type         *extra_param = NULL;
    switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
    {
      case GSTK_SEND_DTMF_REQ:
        cnf_type = GSTK_SEND_DTMF_CNF;
        break;
      case GSTK_SEND_SS_REQ:
        if(estk_curr_inst_ptr->extra_param.present)
        {
          cnf_type = GSTK_SEND_SS_CNF;
          extra_param = &estk_curr_inst_ptr->extra_param;
          extra_param->extra_param.send_ss_extra_param.command_result2 = gstk_rslt;
          extra_param->extra_param.send_ss_extra_param.text_string2.text = NULL;
          if(additional_info_ptr != NULL &&
             additional_info_ptr->length > 0)
          {
            extra_param->extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr
                    = GSTK_CALLOC(additional_info_ptr->length);
            if (extra_param->extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr != NULL)
            {
              (void)gstk_memcpy(extra_param->extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr,
                        additional_info_ptr->additional_info_ptr,
                        additional_info_ptr->length,
                        additional_info_ptr->length,
                        additional_info_ptr->length);
              extra_param->extra_param.send_ss_extra_param.additional_info2_ptr.length
                        = additional_info_ptr->length;
            }
            gstk_free(additional_info_ptr);
            additional_info_ptr = NULL;
          }
          gstk_rslt = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
        }
        break;
      case GSTK_SEND_USSD_REQ:
        if(estk_curr_inst_ptr->extra_param.present)
        {
          cnf_type = GSTK_SEND_USSD_CNF;
          extra_param = &estk_curr_inst_ptr->extra_param;
          extra_param->extra_param.send_ussd_extra_param.command_result2 = gstk_rslt;
          extra_param->extra_param.send_ussd_extra_param.data.text = NULL;
          if(additional_info_ptr != NULL &&
             additional_info_ptr->length > 0)
          {
            extra_param->extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr
                    = GSTK_CALLOC(additional_info_ptr->length);
            if (extra_param->extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr != NULL)
            {
              (void)gstk_memcpy(extra_param->extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr,
                        additional_info_ptr->additional_info_ptr,
                        additional_info_ptr->length,
                        additional_info_ptr->length,
                        additional_info_ptr->length);
              extra_param->extra_param.send_ussd_extra_param.additional_info2_ptr.length
                    = additional_info_ptr->length;
            }
            gstk_free(additional_info_ptr);
            additional_info_ptr = NULL;
          }
          gstk_rslt = GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM;
        }
        break;
      default:
        break;
    }

    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = gstk_rslt;
      estk_cache_additional_info(additional_info_ptr);
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
    }
    else
    {
      (void)estk_send_terminal_response(
              cmd_ref_id,          /* command_details_ref */
              cnf_type,            /* command response */
              gstk_rslt,           /* general_result */
              additional_info_ptr, /* additional result */
              extra_param);        /* extra param */
    }
  }/* if(send_err_rsp_to_gstk) */

  if (additional_info_ptr != NULL)
  {
    gstk_free(additional_info_ptr);
    additional_info_ptr = NULL;
  }

  /* Clear DTMF digits and slot_owns_cm_cli */
  if (TRUE == clear_dtmf)
  {
    estk_clear_dtmf_str();
    estk_shared_info.call_info.call_id = 0xFF;
    estk_shared_info.slot_owns_cm_cli = GSTK_SLOT_ID_MAX;
  }
  return ESTK_SUCCESS;
}/* estk_process_cm_call_event_cmd */

/*===========================================================================

FUNCTION: estk_process_cm_call_list_cmd

DESCRIPTION:
  This function will be called by ESTK to process CM response for the list
  of active calls.

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
estk_result_enum_type estk_process_cm_call_list_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  uint32                         cmd_ref_id           = 0;
  gstk_cmd_from_card_type       *gstk_req_ptr         = NULL;
  boolean                        send_err_rsp_to_gstk = FALSE;
  gstk_additional_info_ptr_type *additional_info_ptr  = NULL;
  gstk_general_result_enum_type  gstk_rslt            =
    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  uint32                         i                    = 0;
  uint8                          additional_info_data = 0;
  gstk_cmd_enum_type             cnf_type             = GSTK_CMD_ENUM_NOT_USE;
  boolean                        cmd_has_alpha        = FALSE;
  gstk_nv_items_data_type        nv_data;

  /* Step 1: Check input parameters */
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  if((estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SETUP_CALL_REQ) &&
     (estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SEND_DTMF_REQ ))
  {
    UIM_MSG_ERR_1("Incorrect command type 0x%x",
                  estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
    return ESTK_ERROR;
  }

  if(cmd_ptr->hdr.cmd != ESTK_CM_CALL_LIST_CMD)
  {
    UIM_MSG_ERR_1("Incorrect command id 0x%x", cmd_ptr->hdr.cmd);
    return ESTK_ERROR;
  }

  gstk_req_ptr = &(estk_curr_inst_ptr->curr_cmd);

  if(cmd_ptr->result == ESTK_ERROR)
  {
    send_err_rsp_to_gstk = TRUE;
    gstk_rslt             = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  }
  switch(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id)
  {
  case GSTK_SETUP_CALL_REQ:
    cnf_type = GSTK_SETUP_CALL_CNF;
    if(send_err_rsp_to_gstk == FALSE)
    {
      if(cmd_ptr->cmd_data.call_list_info.number_of_active_calls == 0)
      {
        if (gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement
           == GSTK_NO_OTHER_CALLS)
        {
          if (estk_process_confirmation_alpha(gstk_req_ptr, &cmd_has_alpha)
              == ESTK_SUCCESS)
          {
            if (!cmd_has_alpha)
            {
              if (estk_cm_orig_call(gstk_req_ptr) == FALSE)
              {
                UIM_MSG_ERR_0("Fail to setup call!");
                send_err_rsp_to_gstk = TRUE;
              }
              else if (estk_process_call_alpha(gstk_req_ptr, &cmd_has_alpha) != ESTK_SUCCESS)
              {
                return ESTK_ERROR;
              }
            }
          }
          else
          {
            send_err_rsp_to_gstk = TRUE;
          }
        }
        else
        {
          if(estk_cm_orig_call(gstk_req_ptr) == FALSE)
          {
            send_err_rsp_to_gstk = TRUE;
          }
          else if (estk_process_call_alpha(gstk_req_ptr, &cmd_has_alpha) != ESTK_SUCCESS)
          {
            return ESTK_ERROR;
          }
        }
      }/* if(cmd_ptr->cmd_data.payload.len == 0) */
      else if(cmd_ptr->cmd_data.call_list_info.number_of_active_calls > ESTK_CALL_ID_MAX)
      {
        send_err_rsp_to_gstk = TRUE;
      }
      else
      {
        if(gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement ==
           GSTK_NO_OTHER_CALLS)
        {
          /* check if redial is needed */
          if(estk_setup_call_redial_needed())
          {
            /* if redial needed, set flag to TRUE so estk_process_setup_call_req() again after timer expires */
            estk_shared_info.redial_delayed_cm_cmd = ESTK_REDIAL_LIST_REQ;

            /* Read Redial timer value */
            (void)gstk_nv_access_read(
                    GSTK_NV_ME_REDIAL_CFG,
                    GSTK_SLOT_ID_MAX,
                    &nv_data);

            /* set timer for reattempt */
            (void)rex_set_timer(&estk_shared_info.cm_cmd_reattempt_timer, nv_data.redial_cfg);

            /* set send_err_rsp_to_gstk to FALSE so we do not send TR yet for redial case */
            send_err_rsp_to_gstk = FALSE;
          }
          else
          {
            additional_info_ptr = GSTK_CALLOC(sizeof(gstk_additional_info_ptr_type));
            if(additional_info_ptr != NULL)
            {
              additional_info_ptr->length = 1;
              additional_info_data =
                int32touint8((int32)GSTK_ME_CURRENTLY_BUSY_ON_CALL);
              additional_info_ptr->additional_info_ptr = &additional_info_data;
            }
            send_err_rsp_to_gstk = TRUE;
          }
        }
        else if(gstk_req_ptr->cmd.setup_call_pro_cmd_req.call_setup_requirement
                  == GSTK_DISCONNECT_ACTIVE_CALLS)
        {
          for(i=0; i< cmd_ptr->cmd_data.call_list_info.number_of_active_calls; i++)
          {
            estk_shared_info.call_info.call_list[i].call_id =
              cmd_ptr->cmd_data.call_list_info.active_calls[i].call_id;
            estk_shared_info.call_info.call_list[i].call_sys_mode =
              cmd_ptr->cmd_data.call_list_info.active_calls[i].call_sys_mode;
            estk_shared_info.call_info.call_list[i].call_active = TRUE;
          }
          if(estk_end_all_calls() == FALSE)
            send_err_rsp_to_gstk = TRUE;
        }
      }
    }
    break;
  case GSTK_SEND_DTMF_REQ:
    cnf_type = GSTK_SEND_DTMF_CNF;
    if(send_err_rsp_to_gstk == FALSE)
    {
      if(cmd_ptr->cmd_data.call_list_info.number_of_active_calls == 0)
      {
        send_err_rsp_to_gstk = TRUE;
        additional_info_ptr = GSTK_CALLOC(sizeof(gstk_additional_info_ptr_type));
        if(additional_info_ptr != NULL)
        {
          additional_info_ptr->length = 1;
          additional_info_data = int32touint8((int32)GSTK_NOT_IN_SPEECH_CALL);
          additional_info_ptr->additional_info_ptr = &additional_info_data;
        }
      }
      else
      {
        estk_shared_info.call_info.call_id =
          cmd_ptr->cmd_data.call_list_info.active_calls[0].call_id;
        estk_shared_info.call_info.call_sys_mode =
          cmd_ptr->cmd_data.call_list_info.active_calls[0].call_sys_mode;
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

        /* Process Alpha Identifier */
        if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
        {
          /* Even in case of null data object in ALPHA (i.e. length = '00' and no value part),
             GSTK make sures at least NULL char is present in alpha text and length includes
             that NULL character. So the min length is one and hence we need to check for
             length more than one */
          if(((estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.length) > 1 &&
              (estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)) ||
             (gstk_nv_get_feature_status(
               GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
               GSTK_SLOT_ID_MAX) == TRUE))
          {
            gstk_cmd_from_card_type gstk_req;

            memset(&gstk_req, 0xFF, sizeof(gstk_req));
            /* Check for DCS validity */
            if(((estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.length) > 0 &&
                (estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)) &&
               ((estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.dcs != 0x04) &&
                (estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.dcs != 0x08)))
            {
              UIM_MSG_ERR_1("Unknown dcs 0x%x",
                            estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.dcs);
              send_err_rsp_to_gstk = TRUE;
              break;
            } /* DCS Validity */

            gstk_req.hdr_cmd = estk_curr_inst_ptr->curr_cmd.hdr_cmd;
            gstk_req.cmd.send_dtmf_pro_cmd_req = estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req;

            if (estk_send_alpha_to_display_client(&gstk_req, TRUE) != ESTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Alpha Display failed");
              send_err_rsp_to_gstk = TRUE;
              break;
            }
          } /* Check to send alpha or not */
        } /* Ignore alpha */
        estk_send_DTMF(cmd_ref_id);
      }
    }
    break;
  default:
    UIM_MSG_ERR_1("Invalid cmd type 0x%x for cm_call_list_cmd processing",
                  estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
    return ESTK_ERROR;
  }/* End of switch */

  if(send_err_rsp_to_gstk == TRUE)
  {
    (void)estk_send_terminal_response(
            cmd_ref_id,           /* command_details_ref */
            cnf_type,             /* command response */
            gstk_rslt,            /* general_result */
            additional_info_ptr,  /* additional result */
            NULL );               /* extra param */
    /* if(send_err_rsp_to_gstk == TRUE) */
  }

  if (additional_info_ptr != NULL)
  {
    gstk_free(additional_info_ptr);
    additional_info_ptr = NULL;
  }

  return ESTK_SUCCESS;
}/* estk_process_cm_call_list_cmd */

/*===========================================================================
FUNCTION:   estk_cm_call_event_cb

DESCRIPTION:
  Call event callback function, called when a call event is happening.
  This function populate the various call related indication type and
  put the command to the ESTK command queue.

PARAMETERS:
  call_event    : [Input] cm call event
  call_info_ptr : [Input] Pointer to the call info struct

DEPENDENCIES:
  gstk command buffer is available

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_call_event_cb (
  cm_call_event_e_type          call_event,
  const cm_mm_call_info_s_type *call_info_ptr
)
{
  estk_cmd_type            *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type    slot         = GSTK_SLOT_1;

  UIM_MSG_HIGH_2("estk_cm_call_event_cb: call_event=%d, call_info_ptr=0x%x",
                 call_event, call_info_ptr);

  if(call_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("call_info_ptr is NULL");
    return;
  }

  if (gstk_io_ctrl(
         GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
         &call_info_ptr->asubs_id,
         &slot) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get slot for as_id 0x%x!",
                  call_info_ptr->asubs_id);
    return;
  }

  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return;
  }

  if (!estk_instances_ptr[(uint8)slot - 1]->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_0("estk_cm_call_event_cb: No CM cmd");
  }

  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  switch(call_event)
  {
    case CM_CALL_EVENT_END:
      task_cmd_ptr->cmd_data.call_event_info.evt_id = ESTK_CM_CALL_END_EVT;
      memset(&(task_cmd_ptr->cmd_data.call_event_info.evt_payload),
             0x00,
             sizeof(estk_generic_data_type));
      task_cmd_ptr->cmd_data.call_event_info.evt_payload.len = 1;
      task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr =
        GSTK_CALLOC(task_cmd_ptr->cmd_data.call_event_info.evt_payload.len+1);
      if(!task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr)
      {
        /* release task_cmd_ptr*/
        estk_free_cmd_buf(task_cmd_ptr);
        return;
      }
      if (CM_CALL_END_CC_REJECT == call_info_ptr->end_status)
      {
        task_cmd_ptr->result = ESTK_UNSUPPORTED;
        break;
      }
      task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[0] =
        int32touint8((int32)call_info_ptr->end_status);
      if (call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS)
      {
        if (call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present)
        {
          if(call_info_ptr->mode_info.info.gw_cs_call.cc_cause.cause_value !=
           NORMAL_CALL_CLEARING)
          {
            task_cmd_ptr->cmd_data.call_event_info.evt_payload.len++;
            task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[1] =
              (0xFF & call_info_ptr->mode_info.info.gw_cs_call.cc_cause.cause_value) | 0x80;
          }
        }/* if (call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present) */
        else if (call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present)
        {
          if(call_info_ptr->mode_info.info.gw_cs_call.cc_reject.rej_type != MM_REJECT_CAUSE)
          {
            task_cmd_ptr->cmd_data.call_event_info.evt_payload.len++;
            task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[1] =
              (0xFF & call_info_ptr->mode_info.info.gw_cs_call.cc_reject.rej_value) | 0x80;
          }
        } /* if (call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present)  */
      }/* if (call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) */
      break;

    case CM_CALL_EVENT_ORIG:
      memset(&task_cmd_ptr->cmd_data.call_event_info,
             0x00,
             sizeof(task_cmd_ptr->cmd_data.call_event_info));
      task_cmd_ptr->cmd_data.call_event_info.evt_id = ESTK_CM_CALL_ORIG_EVT;
      break;

    case CM_CALL_EVENT_CONNECT:
      memset(&task_cmd_ptr->cmd_data.call_event_info,
             0x00,
             sizeof(task_cmd_ptr->cmd_data.call_event_info));
      task_cmd_ptr->cmd_data.call_event_info.evt_id = ESTK_CM_CALL_CONNECT_EVT;
      task_cmd_ptr->cmd_data.call_event_info.call_type = (estk_cm_call_type_enum_type) call_info_ptr->call_type;
      break;

    case CM_CALL_EVENT_MNG_CALLS_CONF:
      if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7, slot) == TRUE)
      {
        UIM_MSG_HIGH_1("call_info_ptr->mode_info.info.gw_cs_call.call_ss_success = 0x%x",
                       call_info_ptr->mode_info.info.gw_cs_call.call_ss_success);
        task_cmd_ptr->cmd_data.call_event_info.evt_id = ESTK_CM_CALL_EVENT_MNG_CALLS_CONF_EVT;
        if(call_info_ptr->mode_info.info.gw_cs_call.ss_error.present)
        {
          UIM_MSG_HIGH_3("ss error code tag 0x%x, ss error code 0x%x, ss error type 0x%x",
                         call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_code_tag,
                         call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_code,
                         call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_type);
          memset(&(task_cmd_ptr->cmd_data.call_event_info.evt_payload),
                   0x00,
                   sizeof(estk_generic_data_type));
          task_cmd_ptr->cmd_data.call_event_info.evt_payload.len =
              sizeof(call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_code_tag) + sizeof(call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_code);
          task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr =
          GSTK_CALLOC(task_cmd_ptr->cmd_data.call_event_info.evt_payload.len);
          if(!task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr)
          {
            /* release task_cmd_ptr*/
            estk_free_cmd_buf(task_cmd_ptr);
            return;
          }
          task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[0] =
             call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_code_tag;
          task_cmd_ptr->cmd_data.call_event_info.evt_payload.data_ptr[1] =
             call_info_ptr->mode_info.info.gw_cs_call.ss_error.error_code;
        }
        else
        {
          estk_free_cmd_buf(task_cmd_ptr);
          return;
        }
        break;
      }
      else
      {
        estk_free_cmd_buf(task_cmd_ptr);
        return;
      }

    case CM_CALL_EVENT_ORIG_MOD_TO_SS:
      UIM_MSG_HIGH_2("Call Modified to 0x%x. alpha dcs: 0x%x",
      	             call_info_ptr->result_from_cc.call_control_result,
                     call_info_ptr->result_from_cc.alpha.dcs);
      if(call_info_ptr->result_from_cc.voice_to_ss.uss_data.present)
      {
        UIM_MSG_HIGH_3("USS data: dcs from sim: 0x%x, uss dcs: 0x%x, uss data: 0x%x",
                     call_info_ptr->result_from_cc.voice_to_ss.uss_data.orig_dcs_from_sim,
                     call_info_ptr->result_from_cc.voice_to_ss.uss_data.uss_data_coding_scheme,
                     call_info_ptr->result_from_cc.voice_to_ss.uss_data.ussData);
        gstk_util_dump_byte_array("ussData",
                                  call_info_ptr->result_from_cc.voice_to_ss.uss_data.ussData,
                                  call_info_ptr->result_from_cc.voice_to_ss.uss_data.size);
      }
      memset(&task_cmd_ptr->cmd_data.call_event_info,
             0x00,
             sizeof(task_cmd_ptr->cmd_data.call_event_info));
      task_cmd_ptr->cmd_data.call_event_info.evt_id = ESTK_CM_CALL_MOD_TO_SS_EVT;
      (void)gstk_memcpy(&task_cmd_ptr->cmd_data.call_event_info.result_from_cc,
                     &call_info_ptr->result_from_cc,
                     sizeof(cm_result_from_cc_s_type),
                     sizeof(cm_result_from_cc_s_type),
                     sizeof(cm_result_from_cc_s_type));
      break;

    default:
      estk_free_cmd_buf(task_cmd_ptr);
      return;
  }
  task_cmd_ptr->hdr.cmd                          = ESTK_CM_CALL_EVENT_CMD;
  task_cmd_ptr->hdr.sim_slot_id                  = slot;
  task_cmd_ptr->cmd_data.call_event_info.call_id = call_info_ptr->call_id;
  task_cmd_ptr->hdr.cmd_ref_id                   =
    estk_instances_ptr[slot-1]->curr_cmd.hdr_cmd.cmd_detail_reference;
  task_cmd_ptr->hdr.cli_user_data                = 0;
  if (task_cmd_ptr->result != ESTK_UNSUPPORTED)
  {
  task_cmd_ptr->result                           = ESTK_SUCCESS;
  }

  estk_put_cmd(task_cmd_ptr);

}/* estk_cm_call_event_cb */

/*===========================================================================
FUNCTION:   estk_cm_orig_call

DESCRIPTION:
  Call event callback function, called when a call event is happening.
  This function populate the various call related indication type and
  put the command to the GSTK command queue.

PARAMETERS:
  cmd_ptr : [Input] gstk set up call command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  boolean
    TRUE:    If the command could be queued successfully to the gstk queue
    FALSE:   If there is an error trying to queue the command to GSTK.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static boolean estk_cm_orig_call(const gstk_cmd_from_card_type *cmd_ptr)
{
  gstk_setup_call_req_type    *req_ptr                            = NULL;
  /* Flag to parse digits/DTMFs */
  uint8                       *digit_ptr                          = NULL;
  /* Number buf going to CM.    */
  uint8                       num_buf[ESTK_MAX_DIALLED_STRING+1]  = {0};
  boolean                     start_dtmf                          = FALSE;
  int                         i                                   = 0;
  uint32                      *digit_count_ptr                    = NULL;
  /* Orig params for this call. */
  cm_gw_cs_orig_params_s_type *gw_orig_params_ptr                 = NULL;
  cm_cdma_orig_params_s_type  *cdma_orig_params_ptr               = NULL;
  /* Buffer holding alpha       */
  cm_orig_alpha_s_type        alpha_buf;
  /* Number of digits in num_buf */
  uint32                      digit_count                         = 0;
  cm_num_s_type               called_number;
  cm_num_s_type               calling_number;
  uint8                       dtmf_buf[ESTK_MAX_DIALLED_STRING+1] = {0};
  uint32                      dtmf_count                          = 0;
  sys_modem_as_id_e_type      as_id                               = SYS_MODEM_AS_ID_1;
  boolean                     ret                                 = FALSE;
  cm_cc_generic_params_s_type cc_generic_params;

  /* Initalize alpha_buf*/
  memset(&alpha_buf, 0, sizeof(cm_orig_alpha_s_type));

  UIM_MSG_HIGH_1("estk_cm_orig_call: cmd_ptr=0x%x", cmd_ptr);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, FALSE);

  if(!cmd_ptr)
  {
    return FALSE;
  }
  req_ptr = (gstk_setup_call_req_type*)&(cmd_ptr->cmd.setup_call_pro_cmd_req);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  cm_num_init( &called_number );           /* Setup the numbers to defaults */
  cm_num_init( &calling_number );          /* Setup the numbers to defaults */
  (void) memset( num_buf, 0, sizeof(num_buf));    /* No dialed number yet. */

  /* Check if the number is an international number */
  if(req_ptr->address.TON == GSTK_INTERNATIONAL_NUM)
    num_buf[digit_count++] = '+';

  /* Seperate the number from the dtmf digits and build the number */
  for (i = 0, digit_ptr = &num_buf[digit_count], digit_count_ptr = &digit_count;
       (*digit_count_ptr < ESTK_MAX_DIALLED_STRING) &&
       (i < req_ptr->address.length);
       i++)
  {
    switch (req_ptr->address.address[i])
    {
      /* Digits are treated as ASCII, */
      case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
      case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
        *digit_ptr++ = '0' + req_ptr->address.address[i];
        *digit_count_ptr = *digit_count_ptr + 1;
        break;

      /* 0x0a is the * key */
      case 0x0A:
        *digit_ptr++ = '*';
        *digit_count_ptr = *digit_count_ptr + 1;
        break;

      /* 0x0b is the # key */
      case 0x0B:
        *digit_ptr++ = '#';
        *digit_count_ptr = *digit_count_ptr + 1;
        break;

      /* 0x0c marks the end of the dialed digits,
       * additional digits are DTMFs, switch the
       * digit_ptr to point into the dtmf_buf
       * array and remember how many in num_buf
       * */
      case 0x0C:
        if(!start_dtmf)
        {
          digit_count_ptr = &dtmf_count;
          start_dtmf = TRUE;
          digit_ptr = &dtmf_buf[0];
        }
        else
        {
          *digit_ptr++ = 0xFF;
          *digit_count_ptr = *digit_count_ptr + 1;
        }
        break;

      /* The rest are really errors. */
      case 0x0D:
      case 0x0E:
      case 0x0F:
      default:
        UIM_MSG_ERR_2("Unexpected value 0x%x in dial string at offset 0x%x.",
                      req_ptr->address.address[i], i);
        break;
    } /* End of switch */
  }/* End of for */

  cm_num_fill(&called_number, (byte *) num_buf,
              (byte) digit_count, CM_DIGIT_MODE_4BIT_DTMF);
  called_number.number_plan = (uint8) CM_NUM_PLAN_ISDN;

  /* Clean up priv dtmf str ptr if not null */
  if(estk_shared_info.call_info.dtmf_str_ptr)
  {
    gstk_free(estk_shared_info.call_info.dtmf_str_ptr);
    estk_shared_info.call_info.dtmf_str_ptr = NULL;
  }

  /* Save the DTMF digit into private data */
  if(dtmf_count > 0)
  {
    estk_shared_info.call_info.dtmf_str_ptr =
      GSTK_CALLOC(dtmf_count+1);
    if(estk_shared_info.call_info.dtmf_str_ptr)
    {
      if(gstk_memcpy(estk_shared_info.call_info.dtmf_str_ptr,
                     dtmf_buf,
                     dtmf_count,
                     dtmf_count+1,
                     int32touint32(ESTK_MAX_DIALLED_STRING)+1) <
         dtmf_count)
      {
        gstk_free(estk_shared_info.call_info.dtmf_str_ptr);
        estk_shared_info.call_info.dtmf_str_ptr = NULL;
        return FALSE;
      }
      estk_shared_info.call_info.dtmf_len = uint32toint32(dtmf_count);
    }
    else
    {
      return FALSE;
    }
  }

  alpha_buf.len = ESTK_MIN(cmd_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length,
                           ESTK_MAX_ALPHA_LEN_FOR_CALL_ORIG);
  (void)memscpy(alpha_buf.buf,
                sizeof(alpha_buf.buf),
                cmd_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
                (int)cmd_ptr->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);

  if (gstk_io_ctrl(
        GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
        &estk_shared_info.slot_owns_cm_cli,
        &as_id) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                  estk_shared_info.slot_owns_cm_cli);
    return FALSE;
  }

  /* Fill both GW and CDMA call parameters */
  gw_orig_params_ptr = GSTK_CALLOC (sizeof (cm_gw_cs_orig_params_s_type));
  if (gw_orig_params_ptr == NULL)
  {
    return FALSE;
  }

  cm_util_default_gw_orig_params_per_sub(gw_orig_params_ptr, as_id);

  /* If the subaddress is in there, set that in the orig parameters. */
  if(req_ptr->destination_subaddress.length > 0)
  {
    gw_orig_params_ptr->called_party_subaddress.present = TRUE;
    (void) (void)memscpy(gw_orig_params_ptr->called_party_subaddress.data,
                         sizeof(gw_orig_params_ptr->called_party_subaddress.data),
                         req_ptr->destination_subaddress.subaddress,
                         req_ptr->destination_subaddress.length);
    gw_orig_params_ptr->called_party_subaddress.length =
      req_ptr->destination_subaddress.length;
  } /*  if (req_ptr->destination_subaddress.length > 0) */
  if (req_ptr->capability_config.length > 0)
  {
    if (gstk_util_parse_ccp(
        &req_ptr->capability_config,
        (uint8 *)&gw_orig_params_ptr->bearer_capability_1,
        sizeof(cm_bearer_capability_s_type)) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to parse CCP into CM struct!");
      if (gw_orig_params_ptr != NULL)
      {
        gstk_free(gw_orig_params_ptr);
      }
      return FALSE;
    }
  }

  cdma_orig_params_ptr = GSTK_CALLOC(sizeof(cm_cdma_orig_params_s_type));
  if (cdma_orig_params_ptr == NULL)
  {
    if (gw_orig_params_ptr != NULL)
    {
      gstk_free(gw_orig_params_ptr);
    }
    return FALSE;
  }

  cm_util_default_cdma_orig_params(cdma_orig_params_ptr);

  cc_generic_params.is_fdn_to_check = FALSE;
  ret = cm_mm_call_cmd_orig_exten5_per_subs (
           estk_call_err_cmd_cb_exten3,
           (void*)&cmd_ptr->hdr_cmd.cmd_detail_reference,
           estk_shared_info.cm_client_id,
           CM_CALL_TYPE_VOICE,
           CM_SRV_TYPE_AUTOMATIC,
           &calling_number,
           &called_number,
           &alpha_buf,
           cdma_orig_params_ptr,
           gw_orig_params_ptr,
           NULL,
           NULL,
           CM_CALL_ID_INVALID,
           &cc_generic_params,
           CM_ORIG_CLIENT_CATEG_CARD,
           TRUE,
           as_id,
           &(estk_shared_info.call_info.call_id));

  gstk_free(gw_orig_params_ptr);
  gw_orig_params_ptr = NULL;
  gstk_free(cdma_orig_params_ptr);
  cdma_orig_params_ptr = NULL;
  return ret;
}/* estk_cm_orig_call */

/*===========================================================================
FUNCTION:   estk_is_any_call_active

DESCRIPTION:
  This ESTK function checks to see if any call is active

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  boolean
    TRUE:    If any call is active
    FALSE:   If no calls are active

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static boolean estk_is_any_call_active(void)
{
  int i = 0;
  for(i=0; i<ESTK_CALL_ID_MAX; i++)
  {
    if(estk_shared_info.call_info.call_list[i].call_active == TRUE)
      return TRUE;
  }
  return FALSE;
}/* estk_is_any_call_active */

/*===========================================================================
FUNCTION:   estk_setup_call_cleanup

DESCRIPTION:
  This ESTK function clears thse setup call related private data.

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
void estk_setup_call_cleanup(void)
{
  int              i                           = 0;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  /* Free sups info ptr private ESTK data, in case allocated for SETUP CALL
     call controlled converted to SUPS */
  if(estk_shared_info.sups_info_ptr)
  {
    UIM_MSG_HIGH_0("estk_sups_cleanup, estk_shared_info.sups_info_ptr != NULL ");
    gstk_free((void *)estk_shared_info.sups_info_ptr);
    estk_shared_info.sups_info_ptr = NULL;
  }

  estk_curr_inst_ptr->result = GSTK_GENERAL_RESULT_MAX;

  if(estk_curr_inst_ptr->extra_param.present &&
      estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.has_cc_type_modification)
  {
    if(estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.text_string2.text != NULL)
    {
      gstk_free(estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.text_string2.text);
      estk_curr_inst_ptr->extra_param.extra_param.setup_call_extra_param.text_string2.text = NULL;
    }
  }

  memset (&estk_curr_inst_ptr->extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));

  if(estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text)
  {
    gstk_free((void *)estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text);
    estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text =
      NULL;
  }
  if(estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.user_cnf_alpha.text)
  {
    gstk_free((void *)estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.user_cnf_alpha.text);
    estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.user_cnf_alpha.text = NULL;
  }
  if(estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.user_cnf_icon.data)
  {
    gstk_free((void *)estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.user_cnf_icon.data);
    estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.user_cnf_icon.data = NULL;
  }
  if(estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.setup_call_display_icon.data)
  {
    gstk_free((void *)estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.setup_call_display_icon.data);
    estk_curr_inst_ptr->curr_cmd.cmd.setup_call_pro_cmd_req.setup_call_display_icon.data = NULL;
  }

  memset(&estk_curr_inst_ptr->curr_cmd,0x00,sizeof(gstk_cmd_from_card_type));
  for(i=0; i< ESTK_CALL_ID_MAX; i++)
  {
    estk_shared_info.call_info.call_list[i].call_id = 0xFF;
    estk_shared_info.call_info.call_list[i].call_active = FALSE;
    estk_shared_info.call_info.call_list[i].call_sys_mode = SYS_SYS_MODE_NO_SRV;
    if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7, estk_curr_inst_ptr->slot_id) == TRUE)
    {
      estk_shared_info.call_info.hold_active_failed = FALSE;
    }
  }

  /* reset the timer */
  (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));
}/* estk_setup_call_cleanup */

/*===========================================================================
FUNCTION:   estk_send_dtmf_cleanup

DESCRIPTION:
  This ESTK function clears thse send dtmf related private data.

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
void estk_send_dtmf_cleanup(void)
{
  int i = 0;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.dtmf.dtmf)
  {
    gstk_free(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.dtmf.dtmf);
    estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.dtmf.dtmf = NULL;
  }

  if(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.length != 0 &&
     estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.text)
  {
    gstk_free(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.text);
    estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.text = NULL;
    estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.alpha.length = 0;
  }

  memset(&estk_curr_inst_ptr->curr_cmd,0x00,sizeof(gstk_cmd_from_card_type));
  for(i=0; i< ESTK_CALL_ID_MAX; i++)
  {
    estk_shared_info.call_info.call_list[i].call_id = 0xFF;
    estk_shared_info.call_info.call_list[i].call_active = FALSE;
    estk_shared_info.call_info.call_list[i].call_sys_mode = SYS_SYS_MODE_NO_SRV;
  }
  estk_clear_dtmf_str();
}/* estk_send_dtmf_cleanup */

/*===========================================================================
FUNCTION:   estk_clear_dtmf_str

DESCRIPTION:
  This ESTK function clears private data related to DTMF string.

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
static void estk_clear_dtmf_str(void)
{
  if(estk_shared_info.call_info.dtmf_str_ptr)
  {
    gstk_free(estk_shared_info.call_info.dtmf_str_ptr);
    estk_shared_info.call_info.dtmf_str_ptr = NULL;
    estk_shared_info.call_info.dtmf_len = 0;
    estk_shared_info.call_info.curr_dtmf_pos_in_str = 0;
  }
  if(estk_shared_info.call_info.temp_DTMF_str_ptr)
  {
    gstk_free(estk_shared_info.call_info.temp_DTMF_str_ptr);
    estk_shared_info.call_info.temp_DTMF_str_ptr = NULL;
  }
}/* estk_clear_dtmf_str */

/*===========================================================================
FUNCTION:   estk_send_DTMF

DESCRIPTION:
  This ESTK function finds the pauses in the DTMF string and tries to send the
  DTMF digits in bursts.

PARAMETERS:
  dummy : [INPUT] dmmy parameter

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_send_DTMF(uint32 dummy)
{
  int32                          i                    = 0;
  gstk_additional_info_ptr_type *additional_info_ptr  = NULL;
  gstk_general_result_enum_type  gstk_rslt            = GSTK_GENERAL_RESULT_MAX;
  boolean                        send_tr              = FALSE;
  uint8                          additional_info_data = 0;
  boolean                        clear_dtmf           = FALSE;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  UIM_MSG_HIGH_2("In estk_send_DTMF, curr dtmf len 0x%x, curr pos 0x%x",
                 estk_shared_info.call_info.dtmf_len,
                 estk_shared_info.call_info.curr_dtmf_pos_in_str);

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(estk_shared_info.call_info.dtmf_str_ptr == NULL)
  {
    UIM_MSG_ERR_0("DTMF String Ptr is null");
    return;
  }

  /* find any pause in the DTMF string? */
  for (i = estk_shared_info.call_info.curr_dtmf_pos_in_str;
       i < estk_shared_info.call_info.dtmf_len;
       i++)
  {
    /* char type signedness is implementation dependent, 0xFF needs to
       be of same type as *estk_shared_info.call_info.dtmf_str_ptr */
    if (estk_shared_info.call_info.dtmf_str_ptr[i] == (char) 0xFF)
    {
      UIM_MSG_HIGH_1("Found a pause 0x%x", i);
      break;
    }
  }/* End of for */

  i -= estk_shared_info.call_info.curr_dtmf_pos_in_str;
  UIM_MSG_HIGH_1("i value is 0x%x", i);

  if(i > 0)
  {
    /* This will be freed in the handle_burst_DTMF function */
    estk_shared_info.call_info.temp_DTMF_str_ptr =
      (char*)GSTK_CALLOC(sizeof(char)* (int32touint32(i)+1));

    if (estk_shared_info.call_info.temp_DTMF_str_ptr == NULL)
    {
      if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_DTMF_REQ)
      {
        additional_info_ptr = GSTK_CALLOC(sizeof(gstk_additional_info_ptr_type));
        if(additional_info_ptr != NULL)
        {
          additional_info_ptr->length = 1;
          additional_info_data =
            int32touint8((int32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
          additional_info_ptr->additional_info_ptr = &additional_info_data;
          gstk_rslt = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          send_tr = TRUE;
        }
      }/* If Send DTMF req */
      clear_dtmf = TRUE;
    }/* temp_DTMF_str_ptr == NULL */
    else /* if temp_DTMF_str_ptr is not NULL */
    {
      (void)gstk_memcpy(
           estk_shared_info.call_info.temp_DTMF_str_ptr,
           &(estk_shared_info.call_info.dtmf_str_ptr[estk_shared_info.call_info.curr_dtmf_pos_in_str]),
           int32touint32(i),
           int32touint32(i) + 1,
           int32touint32(i));

      /* update the current dtmf pos in string */
      estk_shared_info.call_info.curr_dtmf_pos_in_str += (i+1);

      gstk_util_dump_byte_array("temp_DTMF_str_ptr",
                                (const byte  *)estk_shared_info.call_info.temp_DTMF_str_ptr,
                                int32touint32(i));

      /* call cm command to send the dtmf */
      estk_send_burst_DTMF();
    }/* if temp_DTMF_str_ptr is not NULL */
  }
  else /* if i is zero */
  {
    /* update the current dtmf pos in string */
    estk_shared_info.call_info.curr_dtmf_pos_in_str += (i+1);
    /* check if this the end of the dtmf string */
    if(estk_shared_info.call_info.dtmf_len <= estk_shared_info.call_info.curr_dtmf_pos_in_str)
    {
      if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_SEND_DTMF_REQ)
      {
        UIM_MSG_HIGH_0("End of dtmf string");
        send_tr = TRUE;
        gstk_rslt = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      }/* if GSTK_SEND_DTMF_REQ */
      clear_dtmf = TRUE;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif
    }/* if end of dtmf string */
    else /* if not end of string */
    {
      UIM_MSG_HIGH_0("Setting legacy dtmf_timer");
      (void)rex_set_timer(&estk_shared_info.dtmf_timer, ESTK_DTMF_PAUSE_TIME);
#if defined(FEATURE_UIM_TEST_FRAMEWORK)
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      return;
    }
  }/* if i is zero */

  UIM_MSG_HIGH_1("ESTK state 0x%x", estk_curr_inst_ptr->state);

  if ((send_tr) &&
      (estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST))
  {
    UIM_MSG_HIGH_0("Postpone Send DTMF TR until icon API is returned");
    estk_curr_inst_ptr->result = gstk_rslt;
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_cache_additional_info(additional_info_ptr);
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
  else if(send_tr)
  {
    (void)estk_send_terminal_response(
            estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,/* command_details_ref */
            GSTK_SEND_DTMF_CNF,  /* command response */
            gstk_rslt,           /* general_result */
            additional_info_ptr, /* additional result */
            NULL );              /* extra param */
  }/* if(send_tr) */

  if (additional_info_ptr != NULL)
  {
    gstk_free(additional_info_ptr);
    additional_info_ptr = NULL;
  }

  /* Clear DTMF digits and slot_owns_cm_cli */
  if (TRUE == clear_dtmf)
  {
    estk_clear_dtmf_str();
    /* Clear slot_owns_cm_cli if TR is already sent */
    if (ESTK_IDLE_ST == estk_curr_inst_ptr->state)
    {
      estk_shared_info.slot_owns_cm_cli = GSTK_SLOT_ID_MAX;
    }
  }/* end clear_dtmf */
}/* estk_Send_DTMF */

/*===========================================================================
FUNCTION:   estk_send_burst_DTMF

DESCRIPTION:
  This ESTK function sets a timer to handle the DTMF in bursts

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
void estk_send_burst_DTMF()
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  UIM_MSG_HIGH_0("In estk_send_burst_DTMF()");
  estk_shared_info.call_info.start_dtmf = TRUE;
  /* First clear timer */
  (void)rex_clr_timer(&estk_shared_info.burst_dtmf_timer);
  (void)rex_set_timer(&estk_shared_info.burst_dtmf_timer, 0);
#if defined(FEATURE_UIM_TEST_FRAMEWORK)
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}/* estk_send_burst_DTMF */

/*===========================================================================
FUNCTION:   estk_handle_burst_DTMF

DESCRIPTION:
  This ESTK function sends DTMF digits to CM in bursts

PARAMETERS:
  param : [Input] DTMF string address

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_handle_burst_DTMF(uint32 dummy)
{
  char                         *temp_DTMF_str_ptr = estk_shared_info.call_info.temp_DTMF_str_ptr;
  static int                   i                  = 0x00;
  rex_timer_cnt_type           timer              = 0x00;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  UIM_MSG_HIGH_2("In estk_handle_burst_DTMF(): start_dtmf=0x%x, "
                 "temp_DTMF_str_ptr=0x%x",
                 estk_shared_info.call_info.start_dtmf,
                 estk_shared_info.call_info.temp_DTMF_str_ptr);

  if(temp_DTMF_str_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null temp_DTMF_str_ptr");
    return;
  }

  if(estk_shared_info.call_info.start_dtmf)
  {
    if (!cm_mm_inband_cmd_start_cont_dtmf(estk_cm_inband_err_cb,
                                          NULL,
                                          estk_shared_info.cm_client_id,
                                          estk_shared_info.call_info.call_id,
                                          temp_DTMF_str_ptr[i]))
    {
      UIM_MSG_HIGH_0("Err cm_mm_inband_cmd_start_cont_dtmf ");
      gstk_free(temp_DTMF_str_ptr);
      temp_DTMF_str_ptr = NULL;
      estk_shared_info.call_info.temp_DTMF_str_ptr = NULL;
      return;
    }
    timer = 65;
    estk_shared_info.call_info.start_dtmf = FALSE;
    i++;
  }
  else
  {
    if (!cm_mm_inband_cmd_stop_cont_dtmf(estk_cm_inband_err_cb,
                                         NULL,
                                         estk_shared_info.cm_client_id,
                                         estk_shared_info.call_info.call_id,
                                         ESTK_DTMF_DURATION))
    {
      UIM_MSG_HIGH_0("Err cm_mm_inband_cmd_start_cont_dtmf ");
      gstk_free(temp_DTMF_str_ptr);
      temp_DTMF_str_ptr = NULL;
      estk_shared_info.call_info.temp_DTMF_str_ptr = NULL;
      return;
    }
    timer = 90;
    estk_shared_info.call_info.start_dtmf = TRUE;
  }

  /* Check if we are done */
  if (estk_shared_info.call_info.start_dtmf && temp_DTMF_str_ptr[i] == '\0')
  {
    /* free the temp_DTMF_str_ptr */
    i = 0;
    gstk_free(temp_DTMF_str_ptr);
    temp_DTMF_str_ptr = NULL;
    estk_shared_info.call_info.temp_DTMF_str_ptr = NULL;
    UIM_MSG_HIGH_1("Reached end of substring,Mode is 0x%x",
                   estk_shared_info.call_info.call_sys_mode);

    if(estk_shared_info.call_info.call_sys_mode != SYS_SYS_MODE_GSM)
    {
        UIM_MSG_HIGH_0("Setting the dtmf pause timer");
        (void)rex_set_timer(&estk_shared_info.dtmf_timer, ESTK_DTMF_PAUSE_TIME);
    }
    else
    {
      UIM_MSG_HIGH_0("Will set the dtmf pause timer after CM inband event");
    }

#if defined(FEATURE_UIM_TEST_FRAMEWORK)
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

    return;
  } /* done with DTMF sub-string */

  /* First clear timer */
  (void)rex_clr_timer(&estk_shared_info.burst_dtmf_timer);
  (void)rex_set_timer(&estk_shared_info.burst_dtmf_timer, timer);
#if defined(FEATURE_UIM_TEST_FRAMEWORK)
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}/* estk_handle_burst_DTMF */

/*===========================================================================
FUNCTION :  estk_handle_redial_timer

DESCRIPTION:
  This ESTK function clears the redial timer

PARAMETERS:
  dummy : [Input] dummy parameter

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_handle_redial_timer(uint32 dummy)
{
  UIM_MSG_HIGH_0( "Redial Timer Expired");
  /* clear timer */
  (void)rex_clr_timer(&estk_shared_info.redial_timer);
  estk_shared_info.redial_timer_expired = TRUE;
  estk_shared_info.redial_in_progress = FALSE;
} /* estk_handle_redial_timer */

/*===========================================================================
FUNCTION :  estk_handle_cm_cmd_reattempt_timer

DESCRIPTION:
  This ESTK function will send a reattempt cm cmd

PARAMETERS:
  dummy : [Input] dummy parameter

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_handle_cm_cmd_reattempt_timer(uint32 dummy)
{
  uint8                               addi_res        = 0;
  gstk_additional_info_ptr_type       additional_info;

  UIM_MSG_HIGH_2( "Reattempt Timer Expired: redial_delayed_cm_cmd=0x%x, "
                  "redial_in_progress=0x%x",
                  estk_shared_info.redial_delayed_cm_cmd,
                  estk_shared_info.redial_in_progress);

  memset(&additional_info, 0x00, sizeof(additional_info));

  /* clear timer */
  (void)rex_clr_timer(&estk_shared_info.cm_cmd_reattempt_timer);

  /* reattempt setup call req with saved cmd from first try */
  if((estk_shared_info.redial_delayed_cm_cmd == ESTK_REDIAL_LIST_REQ) &&
     (estk_shared_info.redial_in_progress == TRUE))
  {
    UIM_MSG_HIGH_0( "Reattempt cm_mm_call_cmd_get_call_info_list");
    /* send the cached copy of the cmd to cm*/
    if(cm_mm_call_cmd_get_call_info_list(
         estk_call_err_cmd_cb,
         (void*)&(estk_shared_info.redial_cmd.hdr_cmd.cmd_detail_reference),
         estk_shared_info.cm_client_id) == FALSE)
    {
      UIM_MSG_ERR_0( "cm_mm_call_cmd_get_call_info_list redial failed");
    }
    estk_curr_inst_ptr->state = ESTK_PREPROCESS_COMMAND_ST;
  }
  else if((estk_shared_info.redial_delayed_cm_cmd == ESTK_REDIAL_CALL_REQ) &&
          (estk_shared_info.redial_in_progress == TRUE))
  {
    UIM_MSG_HIGH_0( "Reattempt estk_cm_orig_call");
    if(estk_cm_orig_call(&(estk_shared_info.redial_cmd)) == FALSE)
    {
      UIM_MSG_ERR_0( "cm_mm_call_cmd_get_call_info_list redial failed");
    }
  }
  else if(estk_shared_info.redial_in_progress == FALSE)
  {
    addi_res = ESTK_USER_BUSY_ADDL_INFO;
    additional_info.additional_info_ptr = &addi_res;
    additional_info.length = 1;
    UIM_MSG_HIGH_0("Redial Timer expired.  Sending TR.");
    (void)estk_send_terminal_response(
            estk_shared_info.redial_cmd.hdr_cmd.cmd_detail_reference, /* command_details_ref */
            GSTK_SETUP_CALL_CNF,                                      /* command response */
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,         /* general_result */
            &additional_info,                                         /* additional result */
            NULL);                                                    /* extra param */
  }
  else
  {
    UIM_MSG_ERR_0( "INVALID redial request");
  }
} /* estk_handle_cm_cmd_reattempt_timer */

/*===========================================================================
FUNCTION :  estk_setup_call_redial_needed

DESCRIPTION:
  This routine checks if redial is needed.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  boolean.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static boolean estk_setup_call_redial_needed(void)
{
  /* check if redial is present */
  if(estk_shared_info.redial_necessary)
  {
    /* check if redial duration is present */
    if(estk_shared_info.redial_timeout_present)
    {
      /* check if timer expired */
      if(!estk_shared_info.redial_timer_expired)
      {
        UIM_MSG_HIGH_0( "Redial Needed - redial timer expired");
        return TRUE;
      }
      else
      {
        UIM_MSG_HIGH_0( "Redial Not Needed Yet - redial timer has not expired");
        return FALSE;
      }
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    /* no redial specified in the command */
    UIM_MSG_HIGH_0( "No Redial Necessary");
    return FALSE;
  }
} /* estk_setup_call_redial_needed */

/*=============================================================================
FUNCTION: estk_cm_inband_err_cb

DESCRIPTION:
  Command callback from CM to report errors queuing dtmf request to CM.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void estk_cm_inband_err_cb(
  void                   * data_block_ptr,
  cm_inband_cmd_e_type     inband_cmd,
  cm_inband_cmd_err_e_type inband_cmd_err)
{

  UIM_MSG_HIGH_3("estk_cm_inband_err_cb received result 0x%x for Inband command %d, 0x%x",
                 inband_cmd_err, inband_cmd, data_block_ptr);
}/* estk_cm_inband_err_cb */

/*===========================================================================
FUNCTION: estk_process_send_dtmf_req

DESCRIPTION:
  GSTK calls this ESTK function to process the send dtmf request.After
  receiving this command from GSTK, the command is parsed for dtmf digits
  and a subsequent command is queued to CM.

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_dtmf_req(
  gstk_cmd_from_card_type  *gstk_req_ptr)
{
  uint32  cmd_ref_id     = 0x00;

  UIM_MSG_HIGH_1("estk_shared_info.slot_owns_cm_cli=0x%x",
                 estk_shared_info.slot_owns_cm_cli);

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL ptr");
    return ESTK_ERROR;
  }

  if (estk_shared_info.slot_owns_cm_cli != GSTK_SLOT_ID_MAX)
  {
    UIM_MSG_ERR_1("CM client is busy (0x%x)!",
                  estk_shared_info.slot_owns_cm_cli);
    return ESTK_ERROR;
  }
  else
  {
    estk_shared_info.slot_owns_cm_cli = gstk_req_ptr->hdr_cmd.sim_slot_id;
  }

  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  if(gstk_req_ptr->cmd.send_dtmf_pro_cmd_req.dtmf.length == 0)
  {
    UIM_MSG_ERR_0("DTMF len = 0");
    return ESTK_ERROR;
  }
  return estk_process_DTMF();
}/* estk_process_send_dtmf_req */

/*===========================================================================
FUNCTION: estk_process_DTMF

DESCRIPTION:
  This function processes the DTMF request

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_DTMF(void)
{
  byte                      *temp_DTMF_str_ptr   = NULL;
  int                        temp_DTMF_len       = 0;
  byte                       lower_byte          = 0;
  byte                       higher_byte         = 0;
  int                        i,j                 = 0;
  gstk_send_dtmf_req_type   *dtmf_req_ptr        = NULL;
  sys_modem_as_id_e_type     as_id               = SYS_MODEM_AS_ID_1;

  UIM_MSG_HIGH_0("In estk_process_DTMF()");

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  /* Check the input params */
  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id != GSTK_SEND_DTMF_REQ)
  {
    UIM_MSG_ERR_1("Invalid command id 0x%x",
                  estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id);
    return ESTK_ERROR;
  }
  if((estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.dtmf.length == 0) ||
     !(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req.dtmf.dtmf))
  {
    UIM_MSG_ERR_0("0 DTMF length or Null DTMF buf");
    return ESTK_ERROR;
  }
  estk_curr_inst_ptr->state = ESTK_PREPROCESS_COMMAND_ST;
  dtmf_req_ptr = &(estk_curr_inst_ptr->curr_cmd.cmd.send_dtmf_pro_cmd_req);

  /* bcd len * 2 + 1 for null terminated data*/
  temp_DTMF_len     = (dtmf_req_ptr->dtmf.length * 2) + 1;
  temp_DTMF_str_ptr = (byte*)GSTK_CALLOC(int32touint32(temp_DTMF_len));
  if (temp_DTMF_str_ptr == NULL)
  {
    return ESTK_ERROR;
  }
  /* Set the DTMF */
  for (i = 0; i< dtmf_req_ptr->dtmf.length; i++)
  {
    lower_byte = dtmf_req_ptr->dtmf.dtmf[i] & 0x0F;
    higher_byte = (dtmf_req_ptr->dtmf.dtmf[i] & 0xF0) >> 4;

    switch (lower_byte)
    {
    case 0x0A:
      temp_DTMF_str_ptr[j++] = 0x2A;
      break;
    case 0x0B:
      temp_DTMF_str_ptr[j++] = 0x23;
      break;
    case 0x0C:
      temp_DTMF_str_ptr[j++] = 0xFF;
      break;
    case 0x0D:
    case 0x0E:
    case 0x0F:
      UIM_MSG_HIGH_1("Skip non DTMF related digit 0x%x", lower_byte);
      break;
    default:
      temp_DTMF_str_ptr[j++] = 0x30 + lower_byte;
      break;
    }/* switch (lower_byte)  */

    switch (higher_byte)
    {
    case 0x0A:
      temp_DTMF_str_ptr[j++] = 0x2A;
      break;
    case 0x0B:
      temp_DTMF_str_ptr[j++] = 0x23;
      break;
    case 0x0C:
      temp_DTMF_str_ptr[j++] = 0xFF;
      break;
    case 0x0D:
    case 0x0E:
    case 0x0F:
      UIM_MSG_HIGH_1("Skip non DTMF related digit 0x%x", higher_byte);
      break;
    default:
      temp_DTMF_str_ptr[j++] = 0x30 + higher_byte;
      break;
    }/* switch (higher_byte) */
  }/* End of for */

  temp_DTMF_len = uint32toint32(strlen((char*)temp_DTMF_str_ptr) + 1);
  if(temp_DTMF_len <= 1)
  {
    UIM_MSG_ERR_1("temp_DTMF_len too small 0x%x", temp_DTMF_len);
    gstk_free(temp_DTMF_str_ptr);
    temp_DTMF_str_ptr = NULL;
    return ESTK_ERROR;
  }

  /* Free data before malloc to ensure there is no "dangling" pointer */
  if(estk_shared_info.call_info.dtmf_str_ptr)
  {
    gstk_free(estk_shared_info.call_info.dtmf_str_ptr);
    estk_shared_info.call_info.dtmf_str_ptr = NULL;
  }

  estk_shared_info.call_info.dtmf_str_ptr =
    (char*)GSTK_CALLOC(int32touint32(temp_DTMF_len));

  if (!estk_shared_info.call_info.dtmf_str_ptr)
  {
    gstk_free(temp_DTMF_str_ptr);
    temp_DTMF_str_ptr = NULL;
    return ESTK_ERROR;
  }
  estk_shared_info.call_info.curr_dtmf_pos_in_str = 0;
  estk_shared_info.call_info.dtmf_len= int32touint8(temp_DTMF_len - 1);

  /* memscpy only the digit, the last byte is set to null terminated value in
     the memset already */
  (void)memscpy(estk_shared_info.call_info.dtmf_str_ptr,
               temp_DTMF_len,
               temp_DTMF_str_ptr,
               int32touint32(estk_shared_info.call_info.dtmf_len));

  gstk_free(temp_DTMF_str_ptr);
  temp_DTMF_str_ptr = NULL;

    if (gstk_io_ctrl(
          GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
          &estk_shared_info.slot_owns_cm_cli,
          &as_id) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("failed to get as_id for slot 0x%x!",
                    estk_shared_info.slot_owns_cm_cli);
      return ESTK_ERROR;
    }

  /* Check for the presence of active calls */
  if(cm_mm_call_cmd_get_call_info_list_per_subs(
       estk_call_err_cmd_cb,
       &(estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference),
       estk_shared_info.cm_client_id,
       as_id) == FALSE)
  {
    UIM_MSG_ERR_0("cm_mm_call_cmd_get_call_info_list failed");
    return ESTK_ERROR;
  }
  return ESTK_SUCCESS;
}/* estk_process_DTMF */

/*===========================================================================
FUNCTION: estk_cm_inband_event_cb

DESCRIPTION:
  This is the event callback function registered with CM for inband events

PARAMETERS:
  inband_event    - CM Inband event
  inband_info_ptr - CM Inband info

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_inband_event_cb(
  cm_inband_event_e_type        inband_event,
  const cm_inband_info_s_type*  inband_info_ptr)
{
  estk_cmd_type          *task_cmd_ptr = NULL;
  /* We need to find out the proper slot from the inband_info_ptr */
  gstk_slot_id_enum_type  slot         = GSTK_SLOT_1;

  UIM_MSG_HIGH_3("In estk_cm_inband_event_cb() inband_event=0x%x, "
                 "inband_info_ptr=0x%x "
                 "slot=0x%x",
                 inband_event,
                 inband_info_ptr,
                 estk_shared_info.slot_owns_cm_cli);

  if(inband_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null inband_info_ptr from CM");
    return;
  }
  UIM_MSG_HIGH_2("CM Call id is 0x%x, ESTK Call id is 0x%x",
                       inband_info_ptr->call_id,
                       estk_shared_info.call_info.call_id);

  if(estk_shared_info.call_info.call_id != inband_info_ptr->call_id)
  {
    UIM_MSG_HIGH_0("Not ESTK call id");
    return;
  }

  slot = estk_shared_info.slot_owns_cm_cli;
  if (!ESTK_IS_VALID_SLOT_ID(slot) || (slot == GSTK_SLOT_ID_MAX))
  {
    return;
  }

  if (!estk_instances_ptr[(uint8)slot - 1]->curr_cmd.hdr_cmd.command_id)
  {
    UIM_MSG_ERR_0("estk_cm_inband_event_cb: No CM cmd");
  }

  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  switch(inband_event)
  {
    case CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF:
      /* Set the command header */
      task_cmd_ptr->hdr.cli_user_data    = 0;
      task_cmd_ptr->hdr.cmd              = ESTK_CM_INBAND_EVENT_CMD;
      task_cmd_ptr->hdr.sim_slot_id      = slot;
      /* Set the event, call id and time stamp */
      task_cmd_ptr->cmd_data.inband_event.event_type  = inband_event;
      task_cmd_ptr->cmd_data.inband_event.call_id = inband_info_ptr->call_id;
      time_get_uptime_ms(task_cmd_ptr->cmd_data.inband_event.time);
      task_cmd_ptr->result  = ESTK_SUCCESS;
      estk_put_cmd(task_cmd_ptr);
      break;
    default:
      estk_free_cmd_buf(task_cmd_ptr);
      break;
  }
}

/*===========================================================================

FUNCTION: estk_process_cm_inband_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process the inband event command

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
estk_result_enum_type estk_process_cm_inband_event_cmd(
  estk_cmd_type *cmd_ptr)
{
  time_type                     current_uptime;
  qword                         time_diff;
  uint32                        timer_adjustment = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(current_uptime, 0x00, sizeof(time_type));
  memset(time_diff, 0x00, sizeof(qword));

  if(estk_shared_info.call_info.call_sys_mode != SYS_SYS_MODE_GSM)
  {
    UIM_MSG_HIGH_0("RAT is 0x%x, this command is only for GSM/WCDMA");
    return ESTK_SUCCESS;
  }

  /* Step 1: Check input parameters */
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  if(cmd_ptr->hdr.cmd != ESTK_CM_INBAND_EVENT_CMD)
  {
    UIM_MSG_ERR_1("Incorrect command id 0x%x", cmd_ptr->hdr.cmd);
    return ESTK_ERROR;
  }

  if(cmd_ptr->cmd_data.inband_event.event_type != CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF)
  {
    UIM_MSG_ERR_1("Unexpected Inband event 0x%x",
                  cmd_ptr->cmd_data.inband_event.event_type);
  }

  UIM_MSG_HIGH_2("estk_shared_info.call_info.call_id=0x%x, inband_event.call_id=0x%x",
                 estk_shared_info.call_info.call_id,
                 cmd_ptr->cmd_data.inband_event.call_id);

  if(estk_shared_info.call_info.call_id != cmd_ptr->cmd_data.inband_event.call_id)
  {
    return ESTK_ERROR;
  }

  time_get_uptime_ms(current_uptime);
  UIM_MSG_HIGH_2("Curr uptime is %d, prev uptime is %d",
                 qw_lo(current_uptime),
                 qw_lo(cmd_ptr->cmd_data.inband_event.time));
  qw_sub(time_diff, current_uptime, cmd_ptr->cmd_data.inband_event.time);
  timer_adjustment = qw_lo(time_diff);
  UIM_MSG_HIGH_1("Timer adjustment is %d decimal ms", timer_adjustment);

  /* reset the timer */
  (void)rex_clr_timer(&estk_shared_info.dtmf_timer);
  /* Start the timer */
  if((ESTK_DTMF_PAUSE_TIMER_VALUE - timer_adjustment) > 0)
  {
    (void)rex_set_timer(&estk_shared_info.dtmf_timer, ESTK_DTMF_PAUSE_TIMER_VALUE - timer_adjustment);
  }
  else
  {
    (void)rex_set_timer(&estk_shared_info.dtmf_timer, 0);
  }

  return ESTK_SUCCESS;
}/* estk_process_cm_inband_event_cmd */

/*===========================================================================
FUNCTION: estk_check_if_redial_parameters_present

DESCRIPTION:
  This function processes the DTMF request

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_check_if_redial_parameters_present(
  gstk_cmd_from_card_type  *gstk_req_ptr)
{
  uint32 tmp_units;
  uint32 tmp_len;
  estk_result_enum_type estk_result = ESTK_SUCCESS;

  if (!gstk_req_ptr)
  {
    UIM_MSG_ERR_0("estk_check_if_redial_parameters_present: NULL gstk_req_ptr!");
    return ESTK_ERROR;
  }

  /* Check if redial parameters are present */
  if((gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.necessary) &&
     (!estk_shared_info.redial_in_progress))
  {
    UIM_MSG_HIGH_0( "Redial necessary");
    estk_shared_info.redial_necessary = TRUE;
    /* keep a copy of the cmd for when we redial */
    (void)memscpy(&estk_shared_info.redial_cmd,
            sizeof(gstk_cmd_from_card_type),
            gstk_req_ptr,
            sizeof(gstk_cmd_from_card_type));
    /* Check if redial timeout parameters are present */
    if(gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.present)
    {
      tmp_len = (uint32) gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.length;
      tmp_units = (uint32) gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.units;

      /* Check if units are valid.  If not, then we default to no redial. */
      if(tmp_units < 3)
      {
        UIM_MSG_HIGH_1( "Redial timeout present with valid units %x",
                        tmp_units);
        estk_shared_info.redial_timeout_present = TRUE;
        switch(tmp_units)
        {
        case 0:
          estk_shared_info.redial_length_ms = 60000*tmp_len;
          estk_shared_info.redial_in_progress = TRUE;
          UIM_MSG_HIGH_1( "redial length/units 0x%x/0x%x",
                          estk_shared_info.redial_length_ms);
          break;
        case 1:
          estk_shared_info.redial_length_ms = 1000*tmp_len;
          estk_shared_info.redial_in_progress = TRUE;
          UIM_MSG_HIGH_1( "redial length/units 0x%x/0x%x",
                          estk_shared_info.redial_length_ms);
          break;
        case 2:
          estk_shared_info.redial_length_ms = 100*tmp_len;
          estk_shared_info.redial_in_progress = TRUE;
          UIM_MSG_HIGH_1( "redial length/units 0x%x/0x%x",
                          estk_shared_info.redial_length_ms);
          break;
        default:
          estk_shared_info.redial_necessary = FALSE;
          UIM_MSG_ERR_2( "Invalid redial length/units 0x%x/0x%x",
                         gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.length,
                         gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.units);
          break;
        }
        /* Set redial timer if redial is in progress */
        if(estk_shared_info.redial_in_progress)
        {
            /* set timer for redial */
          (void)rex_set_timer(&estk_shared_info.redial_timer, estk_shared_info.redial_length_ms);
          estk_shared_info.redial_timer_expired = FALSE;
        }
      }
      else
      {
        estk_shared_info.redial_necessary = FALSE;
        estk_result                       = ESTK_ERROR;

        UIM_MSG_ERR_2( "Invalid redial length/units 0x%x/0x%x",
                       gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.length,
                       gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.units);
      }
    } /* if(gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.timeout.present) */
    else
    {
      /* set redial timeout to 10 minutes */
      estk_shared_info.redial_timeout_present = FALSE;
      estk_shared_info.redial_length_ms       = 60000 * 10;
      estk_shared_info.redial_in_progress     = TRUE;
      UIM_MSG_HIGH_1( "Redial duration not present.Setting timeout to 0x%x ms",
                      estk_shared_info.redial_length_ms);

        /* set timer for redial */
      (void)rex_set_timer(&estk_shared_info.redial_timer, estk_shared_info.redial_length_ms);
      estk_shared_info.redial_timer_expired = FALSE;
    }
  } /* (gstk_req_ptr->cmd.setup_call_pro_cmd_req.redial.necessary) && (!estk_shared_info.redial_in_progress)) */
  else
  {
    estk_shared_info.redial_necessary       = FALSE;
    estk_shared_info.redial_timeout_present = FALSE;
    estk_shared_info.redial_length_ms       = 0;
    estk_result                             = ESTK_ERROR;
    UIM_MSG_HIGH_0( "Redial timeout not necessary");
  }
  return estk_result;
} /* estk_check_if_redial_parameters_present */

/*===========================================================================
FUNCTION: map_cm_sys_mode_to_call_mode

DESCRIPTION:
  This function maps cm sys mode from call list to cm call mode

PARAMETERS:
  cm_sys_mode : [Input] sys mode to be mapped

DEPENDENCIES:
  None

RETURN VALUE:
  cm_call_mode_info_e_type
    the call mode

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static cm_call_mode_info_e_type estk_map_cm_sys_mode_to_call_mode (
  sys_sys_mode_e_type cm_sys_mode)
{
  cm_call_mode_info_e_type  cm_call_mode = CM_CALL_MODE_INFO_NONE;

  switch (cm_sys_mode)
  {
  case SYS_SYS_MODE_CDMA:
  case SYS_SYS_MODE_HDR:
    cm_call_mode = CM_CALL_MODE_INFO_CDMA;
    break;
  case SYS_SYS_MODE_GSM:
  case SYS_SYS_MODE_WCDMA:
  case SYS_SYS_MODE_GW:
  case SYS_SYS_MODE_GWL:
    cm_call_mode = CM_CALL_MODE_INFO_GW_CS;
    break;
  case SYS_SYS_MODE_WLAN:
    cm_call_mode = CM_CALL_MODE_INFO_IP;
    break;
  case SYS_SYS_MODE_LTE:
    cm_call_mode = CM_CALL_MODE_INFO_LTE;
    break;
  default:
    cm_call_mode = CM_CALL_MODE_INFO_NONE;
    break;
  }

  return cm_call_mode;
}

#endif /* FEATURE_ESTK */

