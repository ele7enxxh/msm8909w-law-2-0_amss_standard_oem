/*===========================================================================


                       G S T K   E V T   D L   C M D

GENERAL DESCRIPTION
  This source file contains event download envelope command functions


FUNCTIONS
  gstk_cm_call_event_cb
    Call event callback function, called when a call event is happening.
    This function populate the various call related indication type and
    put the command to the GSTK command queue.

  gstk_mt_call_evt_ind
    This function builds the mt call command and sends it to UIM

  gstk_call_connected_evt_ind
    This function builds the call connected envelope command and sends it to UIM

  gstk_call_disconnected_evt_ind
    This function builds the call disconnected envelope command and sends it
    to UIM

  gstk_idle_scrn_evt_ind
    This function builds the idle screen command and sends it to UIM

  gstk_user_act_evt_ind
    This function builds the user activity command and sends it to UIM

  gstk_location_status_evt_ind
    This function builds the location status event download envelope command
    and sends it to UIM

  gstk_lang_selection_evt_ind
    This function builds the language selection event download envelope command
    and sends it to UIM

  gstk_browser_term_evt_ind
    This function builds the browser termination event download envelope command
    and sends it to UIM

  gstk_util_translate_cm_transaction_id_to_gstk_id
    This function converts the CM transaction ID into SAT/USAT format

  gstk_util_translate_cm_cause_to_gstk_cause
    This function converts the CM cause into SAT/USAT format

  gstk_hci_connectivity_evt_ind
    This function builds the HCI Connectvity command and sends it to UIM

  gstk_csg_evt_ind
    This function builds the CSG Cell Selection command and sends it to UIM


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_evt_dl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
11/12/15   vr      F3 Logging inprovements
12/19/14   gm      Added support to read envelope retry data from NV
10/17/14   bd      Fixed TI flag issue
10/07/14   sw      Send the right Cell Length
10/06/14   gm      Added support to send failure indication for OTASP
09/30/14   gs      Use card_protocol to determine length of cell id
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info 
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/15/14   gm      Fixed to send correct event list tag
07/11/14   shr     Add Support for NV Refresh
06/30/14   dy      Fixed medium compiler warning
06/03/14   sw      Implemented retry for call-control envelopes
05/28/14   gm      Support of CSG Rel-11 feature 
05/06/14   gs      GSTK Featurization enhancements and clean up
04/04/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/03/14   shr     ESTK BIP clean-up and enhancements
02/11/14   gm      Fix KW critical warning
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
11/21/13   sw      Envelope retry implemented for BIP envelopes
12/02/13   gm      Remove Lint error
11/04/13   yt      Source device id set incorrectly in hci connectivity
10/04/13   sw      Reduced F3 messages
08/12/13   sw      Corrected logging of number of envelope bytes
07/22/13   sw      Populate header info for CSG envelope UIM cmd
06/24/13   xz      Support IMS Registration event download 
06/20/13   vr      Triton TSTS changes
05/23/13   hh      Fix KW errors
05/23/13   hn      Added exception CM call end status for which not to increment 
                   OTASP retry counter  
05/16/13   hh      Fix KW errors
04/29/13   gm      Added signal fade scenario in call disconnect event download
04/16/13   bd      Invalidate the VPE cache while sending CDMA LS to card
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/21/13   hn      Enhancement of Loc Status and VPE retry 
01/22/13   hn      Support 1xRTT CSIM programming retries 
11/13/12   bd      Support for Closed Subscriber Group feature
10/25/12   hn      Process Call Connected Evt later when there is Set Up Call pending 
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
07/31/12   bd      Clean up the hack to calculate correct direction for call end event
                   download envelopes
05/24/12   dd      Fix NW Rej envelope
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Remove tmc.h inclusion
10/31/11   sg      Class L and M support
10/04/11   xz      Fix compile warning
09/28/11   dd      Clear data buffer in send data or listen state
09/27/11   xz      Fix the issue of call disconnect with CM_CALL_END_FADE
08/24/11   xz      Fix compile error
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/22/11   bd      Fixed incorrect message direction in envelope
07/13/11   dd      Clear the SCWS Tx/Rx buffers
06/16/11   sg      Fix simultaneous OTASP and BIP on a multi-modem environment
05/18/11   xz      Use MMGSDI evt to track apps enabled
05/18/11   nb      ThreadSafe - Removed Usage of gstk_util_save_env_cmd_info()
05/17/11   sg      Queue current access tech to TAL before sending LS
05/03/11   xz      Provide LTE only support
04/05/11   bd      Added envelope retry logic for Access Tech envelope
03/23/11   sg      1. Move GSTK_MAX_NUM_OF_EVT_DL_ENV_RETRY to gstk_envelope_cmd.h
                   2. Store NW_REJ event download for retry
01/26/11   xz      Implement to set con_ref in CDMA call event
01/26/11   xz      Only send MT call event for voice or CS data call
12/18/10   ssr     Dynamically allocate command buffer
10/28/10   dd      Fix LTE only build
10/11/10   sg      Port fix for multiple OTASP calls
09/29/10   xz      KW Fixes
07/09/10   dd      Check for CM Sups events
05/21/10   xz      Add support of network rejection event
05/17/10   bd      Fix to send location status envelope when UE goes to no service
05/10/10   bd      Added check for PS_DATA message to not send envelope for CONNECT
                   and DISCONNECT messages
03/10/01   xz      Pass correct client ID when process CM events
02/26/10   yt      Fixed KW errors
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
06/01/09   xz      Support removal of CM_MAX_NUMBER_CHARS
09/18/08   xz      Remove initialization of "cause" structure in
                   gstk_cm_call_event_cb()
09/14/08   sk      Fixed lint errors
05/06/08   sk      Added support for event download env retry
02/06/08   sk      Support for PLI UTRAN NMR
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Added Null ptr checks
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/02/06   tml     Fixed transaction ID issue and lint
05/18/06   sk      Fixed 1X warnings.
05/17/06   sk      Added support for Radio Link Failure Call disconnected evt
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related location information.
11/21/05   sk      Fixed lint errors
11/15/05   sk      Lint fixes
11/09/05   sk      Fixed Lint Errors
10/08/05   tml     Cell ID Length check
10/07/05   sk      LS fixes
10/04/05   sk      Added messages to gstk_location_status_evt_ind().
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added new fn gstk_access_tech_evt_ind()
                   for Access Technology envelope.
05/11/05   sst     Lint fixes
01/27/05   tml     Fixed potential memory leak issue
01/12/05   tml     Fixed compilation errors for 1X only target
12/20/04    sk     Added 2 new utility function
                   gstk_util_translate_cm_cause_to_gstk_cause()
                   & gstk_util_translate_cm_transaction_id_to_gstk_id_ext()
                   Added support to handle cause value correctly on a
                   disconnect event.
11/23/04    sk     Added SAT/USAT data support
10/01/04   tml     Fixed transaction id issue
08/16/04   tml     Added call direction fix
08/13/04   tml     Fixed source destination device ID for call connected
08/10/04   tml     Fixed transaction id
08/10/04   tml     Added checking in CALL CONNECTED event cb to determine if
                   the envelope cmd should be sent immediately to the card or
                   not
08/03/04   tml     Fixed transaction ID issue with Call Connected and Call
                   Disconnected event download, fixed command queue issue
05/26/04   tml     Added More debug messages
05/11/04   tml     Added timer management, lang notification, browser term
                   supports
04/16/04   tml     linted
04/02/04   tml     linted
03/01/04   tml     Merged from branch for location status env command fix
02/06/04   wli     Featurization for CDMA only build
05/16/03   tml     Linted
04/21/03   tml     Added NPI support in address
03/20/03   tml     Fixed offset for envelope responses
03/06/03   tml     Updated enum names
02/22/03   tml     Added support for transaction ID, near end and cause info
                   Event download user data fixes
02/18/03   tml     Enhanced CM related event download functions
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "gstk_envelope_cmd.h"
#include "uim.h"
#include "gstkutil.h"
#include "string.h"
#include "msg.h"
#include "err.h"
#include "uim_msg.h"
#include "gstkutil.h"
#if defined(FEATURE_ESTK)
#include "estk.h"
#endif /* FEATURE_ESTK */

#define GSTK_NORMAL_RADIO_LINK_RELEASE 165

typedef enum
{
  GSTK_CALL_MSG_DIRECTION_TO_ME,
  GSTK_CALL_MSG_DIRECTION_FROM_ME,
  GSTK_CALL_MSG_DIRECTION_MAX_ENUM = 0x7FFFFFFF
}gstk_message_direction_enum_type;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined(FEATURE_LTE)
/*===========================================================================
FUNCTION   gstk_util_translate_cm_cause_to_gstk_cause

DESCRIPTION
  This function converts CM cause value to GSTK cause value

PARAMETERS
  cause   : [Input/Output] GSTK cause value
  cm_cause: [Input] CM cause value

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_util_translate_cm_cause_to_gstk_cause(
  gstk_cause_type *gstk_cause,
  const ie_cm_cc_cause_s_type *cm_cause)
{
  uint8 len =0;
  uint8 cm_diag_len = 0;

  if((gstk_cause == NULL)||(cm_cause == NULL))
  {
    UIM_MSG_ERR_2("ERR:NULL pointer. gstk_cause=0x%x cm_cause=0x%x",
                  gstk_cause, cm_cause);
    return;
  }
  
  UIM_MSG_HIGH_3("cause present is 0x%x, cause value is 0x%x, coding std is 0x%x",
                 cm_cause->present,
                 cm_cause->cause_value,
                 cm_cause->coding_standard);

  UIM_MSG_HIGH_3("location is 0x%x, recommendation present is 0x%x, diagnostic_length is 0x%x",
                 cm_cause->location,
                 cm_cause->recommendation_present,
                 cm_cause->diagnostic_length);

  if(cm_cause->present == FALSE)
  {
    gstk_cause->present = FALSE;
    return;
  }

  gstk_util_dump_byte_array("diagnostics", cm_cause->diagnostics, cm_cause->diagnostic_length);

  if(cm_cause->cause_value == 6)
  {
    UIM_MSG_HIGH_0("CM Disconnected Cause : Radio Link Failure");
    gstk_cause->present = TRUE;
    gstk_cause->length = 0x00;
    return;
  }


  /* Coding of BYTE 0 */
  gstk_cause->cause[len] =  int16touint8(cm_cause->present << 7);
  gstk_cause->cause[len] |= (cm_cause->coding_standard);

  /* In BIT1 - Bit8, Bit 5 is a spare bit */

  gstk_cause->cause[len] |= (cm_cause->location);
  len++;

  /* If the 6th and the 7th bit are 1-1, "Standard defined for the GSMßPLMNS",
     do not read the 'recommendation' octet
  */

  /* Coding of BYTE 1 - may be present/absent depending on the coding standard in Byte 0*/
  if(cm_cause->coding_standard != 96)
  {
    /* shud I check here for the bit to be 1 or just copy the value from CM? */
    gstk_cause->cause[len] = int16touint8(cm_cause->recommendation_present << 7);
    gstk_cause->cause[len] |= cm_cause->recommendation;

    /* Recommendation byte was not accounted for when this function was called
       by gstk_cm_call_event_cb. Hence the length needs to be incremented by 1
    */
    len++;

  }

  /* Coding of BYTE 1/2 */
  gstk_cause->cause[len] = 128; /* Set Bit 8 - ext bit to 1 */
  gstk_cause->cause[len] |= cm_cause->cause_value;
  len++;

  while(cm_diag_len < cm_cause->diagnostic_length
        &&
        cm_diag_len < MAX_DIAGNOSTICS_LENGTH)
  {
    gstk_cause->cause[len] = cm_cause->diagnostics[cm_diag_len];
    cm_diag_len++;
    len++;
  }

  gstk_cause->present = TRUE;
  gstk_cause->length = len;

  return;
}

/*===========================================================================
FUNCTION   gstk_util_translate_cm_transaction_id_to_gstk_id

DESCRIPTION
  This function converts the CM transaction ID into SAT/USAT format.
  According to ETSI TS 124 007, Transaction ID format:
  bit 8          7 - 5        4 - 1
  TI Flag       TI Value       RFU

  TI Flag = 0 if the message belongs to transaction initiated by its sender
  TI Flag = 1 otherwise

PARAMETERS
  transaction_id: [Input/Output] GSTK transaction ID
  ti_flag: [Input] TI Flag
  cm_transaction_id: [Input] CM transaction ID

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_util_translate_cm_transaction_id_to_gstk_id(
  const gstk_transaction_id_type   *transaction_id,
  boolean                           ti_flag,
  const uint8                      *cm_transaction_id)
{
  int i = 0;

  if ((transaction_id == NULL) || (cm_transaction_id == NULL)) {
    UIM_MSG_ERR_0("Transaction ID input is null");
    return;
  }

  for(i = 0; i < transaction_id->length; i++)
  {
    transaction_id->transaction_list[i] = cm_transaction_id[i];
    /* remove the most significant bit of the nibble to get TI value to construct the
       byte as per ETSI TS 124 007 */
    if (transaction_id->transaction_list[i] & 8)
    {
      transaction_id->transaction_list[i] -= 8;
    }
    /* Shift the transaction ID up because bit 1-4 if RFU according to ETSI TS 124 007 */
    transaction_id->transaction_list[i] = uint16touint8(transaction_id->transaction_list[i] << 4);
    /* Set TI Flag */
    if (TRUE == ti_flag)
    {
      transaction_id->transaction_list[i] |= 0x80;
    }
    UIM_MSG_HIGH_3("transaction id, cm value: 0x%x, gstk value: 0x%x, ti_flag 0x%x ",
                   cm_transaction_id[i],
                   transaction_id->transaction_list[i],
                   ti_flag);
  }
} /* gstk_util_translate_cm_transaction_id_to_gstk_id */
#endif /*FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/

/*lint -e715 "call_info_ptr call_event not referenced . This lint error is
  suppressed when FEATURE_GSM or FEATURE_WCDMA is not defined." */
/*===========================================================================
FUNCTION   gstk_cm_call_event_cb

DESCRIPTION
  Call event callback function, called when a call event is happening.
  This function populate the various call related indication type and
  put the command to the GSTK command queue.

PARAMETERS
  call_event: [Input] cm call event
  call_info_ptr: [Input] Pointer to the call info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_call_event_cb (
  cm_call_event_e_type         call_event,
  const cm_mm_call_info_s_type *call_info_ptr
)
{
  gstk_task_cmd_type                 * task_cmd           = NULL;
  uint8                                length             = 0;
  gstk_transaction_id_direction_type   call_evt_user_data;
  boolean                              ti_flag            = FALSE;
  gstk_slot_id_enum_type               slot               = GSTK_SLOT_1;
  uint8                                transaction_list[10];
#ifdef FEATURE_GSTK_FUSION_REMOTE
  gstk_tal_uim_cmd_type                otasp_status_cmd;
#endif /*FEATURE_GSTK_FUSION_REMOTE*/

  if (call_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("call pointer null");
    return;
  }
#ifdef FEATURE_GSTK_FUSION_REMOTE
  memset(&otasp_status_cmd, 0, sizeof(otasp_status_cmd));
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
  memset(&call_evt_user_data, 0x00, sizeof(call_evt_user_data));
  memset(transaction_list, 0x00, sizeof(transaction_list));

  if (gstk_io_ctrl(
        GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
        &call_info_ptr->asubs_id,
        &slot) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to get slot for as_id 0x%x!",
                  call_info_ptr->asubs_id);
    return;
  }
  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return;
  }

  UIM_MSG_HIGH_3("gstk_cm_call_event_cb: event 0x%x, call type 0x%x, call id 0x%x",
                 call_event, call_info_ptr->call_type, call_info_ptr->call_id);
  switch (call_event)
  {
    case CM_CALL_EVENT_CONNECT:
    /* Set the gstk_cm_call_is_connected flag to TRUE only if ""  is CS Call. This flag is used to keep track of the MM_IDLE state
       in order to queue RRC messages  */
    if((call_info_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
       (call_info_ptr->call_type == CM_CALL_TYPE_VOICE))
    {
      gstk_instances_ptr[(uint8)slot - 1]->gstk_cm_call_is_connected = TRUE;
    }

    if((gstk_instances_ptr[(uint8)slot - 1]->gstk_evt_list[GSTK_CALL_CONNECTED] & 0x01) != 0x01 ||
        call_info_ptr->call_type == CM_CALL_TYPE_PS_DATA)
    {
      return;
    }

    /* mo or mt call, MT call -> near end connect message, MO call -> far end connect message */
    if(call_info_ptr->direction == CM_CALL_DIRECTION_MO)
    {
      call_evt_user_data.near_end = FALSE;
    }
    else if(call_info_ptr->direction == CM_CALL_DIRECTION_MT)
    {
      call_evt_user_data.near_end = TRUE;
    }
    else
    {
      UIM_MSG_ERR_1("Unknown call dir, 0x%x", call_info_ptr->direction);
      call_evt_user_data.near_end = FALSE;
    }

    /* transaction id */
    call_evt_user_data.transaction_id.length = 1;
    call_evt_user_data.transaction_id.transaction_list = transaction_list;
    if (call_evt_user_data.transaction_id.length > sizeof(transaction_list))
    {
      UIM_MSG_ERR_2("insuffecient transaction buffer! (0x%x vs 0x%x",
                    call_evt_user_data.transaction_id.length,
                    sizeof(transaction_list));
      return;
    }

    switch (call_info_ptr->mode_info.info_type)
    {
      case CM_CALL_MODE_INFO_GW_CS:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
        /* CONNECT is always sent to the transaction initiator. Hence ti_flag is set
           to TRUE always */
        gstk_util_translate_cm_transaction_id_to_gstk_id(
          &call_evt_user_data.transaction_id,
          TRUE,
          &(call_info_ptr->mode_info.info.gw_cs_call.transaction_id));
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/
        break;

      case CM_CALL_MODE_INFO_CDMA:
        call_evt_user_data.transaction_id.transaction_list[0] =
          call_info_ptr->mode_info.info.cdma_call.con_ref;
        break;

      default:
        UIM_MSG_ERR_1("invalid call type 0x%x",
                      call_info_ptr->mode_info.info_type);
    }

    /* Always queue the command immediately, whether there is Set Up Call pending we'll check later */
    /* get command buf */
    task_cmd = gstk_task_get_cmd_buf();
    if(task_cmd != NULL)
    {
      /* successfully obtained command buffer - build the call related envelope command download */
      task_cmd->cmd.call_connected_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint8)slot - 1]->client_id;
      task_cmd->cmd.call_connected_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      task_cmd->cmd.call_connected_envelope_ind.message_header.command_id    = (int)GSTK_CALL_CONNECTED_EVT_IND;
      task_cmd->cmd.call_connected_envelope_ind.message_header.sim_slot_id   = slot;
      task_cmd->cmd.call_connected_envelope_ind.message_header.user_data     = 0;

      /* transaction id */
      task_cmd->cmd.call_connected_envelope_ind.transaction_id.length = 1;
      task_cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list =
        GSTK_CALLOC(task_cmd->cmd.call_connected_envelope_ind.transaction_id.length);
      if (task_cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list == NULL)
      {
        /* put the queue back to the free queue */
        gstk_task_free_cmd_buf(task_cmd);
        return;
      }

      (void)memscpy(task_cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list,
      	    task_cmd->cmd.call_connected_envelope_ind.transaction_id.length,
            call_evt_user_data.transaction_id.transaction_list,
            task_cmd->cmd.call_connected_envelope_ind.transaction_id.length);
      /* mo or mt call */
      task_cmd->cmd.call_connected_envelope_ind.near_end = call_evt_user_data.near_end;
      gstk_task_put_cmd_buf(task_cmd);
    }
    break;

    case CM_CALL_EVENT_END: /* call disconnected */
      if((gstk_instances_ptr[(uint8)slot - 1]->otasp_call_id >= 0)      &&
         (int32touint8(gstk_instances_ptr[(uint8)slot - 1]->otasp_call_id) == call_info_ptr->call_id))
      {
        if (!gstk_instances_ptr[(uint8)slot - 1]->recv_otasp_commit)
        {
          UIM_MSG_HIGH_1("Received Call End without getting successful OTASP COMMIT,treating OTASP call as failure, end_staus = %x",
                         call_info_ptr->end_status);
          if (call_info_ptr->end_status != CM_CALL_END_FADE &&
              call_info_ptr->end_status != CM_CALL_END_FADE_SRV_INIT_FAIL &&
              call_info_ptr->end_status != CM_CALL_END_FADE_T50_EXP &&
              call_info_ptr->end_status != CM_CALL_END_FADE_T51_EXP &&
              call_info_ptr->end_status != CM_CALL_END_FADE_RL_ACK_TIMEOUT &&
              call_info_ptr->end_status != CM_CALL_END_FADE_BAD_FL &&
              call_info_ptr->end_status != CM_CALL_END_MAX_ACCESS_PROBE) 
          {
            gstk_instances_ptr[(uint8)slot - 1]->otasp_failure_cnt++;
            UIM_MSG_HIGH_1("Increment otasp_failure_cnt counter, new counter = %x",
                           gstk_instances_ptr[(uint8)slot - 1]->otasp_failure_cnt);
            if(gstk_instances_ptr[(uint8)slot - 1]->otasp_failure_cnt >= GSTK_MAX_OTASP_RETRY_CNT &&
               gstk_shared_data.gstk_otasp_status_cb != NULL)
            {
              (gstk_shared_data.gstk_otasp_status_cb)(slot, GSTK_OTASP_ACT_STATUS_PERM_FAIL);
            }
          }
        }
        gstk_instances_ptr[(uint8)slot - 1]->recv_otasp_commit = FALSE;
        gstk_instances_ptr[(uint8)slot - 1]->otasp_call_id = -1;
#ifdef FEATURE_GSTK_FUSION_REMOTE
        otasp_status_cmd.cmd_type = GSTK_SETUP_OTASP_CALL_CMD;
        otasp_status_cmd.user_data = 0;
        (void)gstk_tal_send_cmd(GSTK_TAL_REMOTE_CLIENT,
                                &otasp_status_cmd);
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
      }

      /* Set the gstk_cm_call_is_connected flag to FALSE only if ""  is CS Call. This flag is used to keep track of the MM_IDLE state
         in order to queue RRC messages  */
      if((call_info_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
         (call_info_ptr->call_type == CM_CALL_TYPE_VOICE))
      {
        gstk_instances_ptr[(uint8)slot - 1]->gstk_cm_call_is_connected = FALSE;
      }

      if ((gstk_instances_ptr[(uint8)slot - 1]->gstk_evt_list[GSTK_CALL_DISCONNECTED] & 0x01) != 0x01 ||
           call_info_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      {
        return;
      }

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL )
      {
        /* successfully obtained command buffer */
        task_cmd->cmd.call_disconnected_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint8)slot - 1]->client_id;
        task_cmd->cmd.call_disconnected_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        task_cmd->cmd.call_disconnected_envelope_ind.message_header.command_id    = (int)GSTK_CALL_DISCONNECTED_EVT_IND;
        task_cmd->cmd.call_disconnected_envelope_ind.message_header.user_data     = 0;
        task_cmd->cmd.call_disconnected_envelope_ind.message_header.sim_slot_id   = slot;

        /* transaction id */
        task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.length = 1;
        task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list =
        GSTK_CALLOC(task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.length);
        if (task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list == NULL)
        {
          /* put the queue back to the free queue */
          gstk_task_free_cmd_buf(task_cmd);
          return;
        }

        UIM_MSG_HIGH_2(" cc cause_present is 0x%x, cc reject_present is 0x%x",
                       call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present,
                       call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present);

        /* user end or nw end */
        if((call_info_ptr->end_status == CM_CALL_END_CLIENT_END)
#if defined(FEATURE_CDMA)
           || (call_info_ptr->end_status == CM_CALL_END_FADE)
           || (call_info_ptr->end_status == CM_CALL_END_FADE_SRV_INIT_FAIL)
           || (call_info_ptr->end_status == CM_CALL_END_FADE_T50_EXP)
           || (call_info_ptr->end_status == CM_CALL_END_FADE_T51_EXP)
           || (call_info_ptr->end_status == CM_CALL_END_FADE_RL_ACK_TIMEOUT)
           || (call_info_ptr->end_status == CM_CALL_END_FADE_BAD_FL)
#endif /* FEATURE_CDMA */
        )
        {
          task_cmd->cmd.call_disconnected_envelope_ind.near_end = TRUE;
        }
        else
        {
          task_cmd->cmd.call_disconnected_envelope_ind.near_end = FALSE;
        }
        UIM_MSG_HIGH_2("Near end is 0x%x and call dir is 0x%x",
                       task_cmd->cmd.call_disconnected_envelope_ind.near_end,
                       call_info_ptr->direction);
        /* Convert the transaction ID */
        switch (call_info_ptr->mode_info.info_type)
        {
        case CM_CALL_MODE_INFO_GW_CS:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
          /* Calculate ti_flag */
          if (call_info_ptr->direction == CM_CALL_DIRECTION_MO && 
              call_info_ptr->end_status != CM_CALL_END_CLIENT_END)
          {
            ti_flag = TRUE;
          }
          else if (call_info_ptr->direction == CM_CALL_DIRECTION_MT &&
                   call_info_ptr->end_status == CM_CALL_END_CLIENT_END)
          {
            ti_flag = TRUE;
          }
          gstk_util_translate_cm_transaction_id_to_gstk_id(
            &task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id,
            ti_flag,
            &(call_info_ptr->mode_info.info.gw_cs_call.transaction_id));
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/
          if(call_info_ptr->mode_info.info.gw_cs_call.cc_cause.present)
          {
            /* Byte 0 - for coding standard and location
               Byte 1 - for cause value
               Other bytes are for diagnostic information
               NOTE:- Recommendation Byte has not been taken into account here though we allocate
               an extra byte for it. Hence if the recommendation byte is present, the value of
               task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length
               has to be incremented  by 1 in the gstk_util_translate_cm_cause_to_gstk_cause() */
            task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length =
              call_info_ptr->mode_info.info.gw_cs_call.cc_cause.diagnostic_length + 2;
            task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.cause  =
              GSTK_CALLOC(
                    task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length+1);
            if (task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.cause == NULL)
            {
              if (task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list != NULL)
              {
                gstk_free(task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list);
                task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list = NULL;
              }
              return;
            }
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
            /* Convert CM cause value to GSTK cause value */
            gstk_util_translate_cm_cause_to_gstk_cause(
              &task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause,
              &(call_info_ptr->mode_info.info.gw_cs_call.cc_cause));
  #endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/
          }
          else if(call_info_ptr->mode_info.info.gw_cs_call.cc_reject.present)
          {
            UIM_MSG_HIGH_2("Reject value is present, cc reject type is 0x%x, cc reject value is 0x%x",
                           call_info_ptr->mode_info.info.gw_cs_call.cc_reject.rej_type,
                           call_info_ptr->mode_info.info.gw_cs_call.cc_reject.rej_value);
            if(call_info_ptr->mode_info.info.gw_cs_call.cc_reject.rej_value !=
               GSTK_NORMAL_RADIO_LINK_RELEASE)
            {
              /*Radio Link Failure for call disconnected*/
              task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.present =
                  TRUE;
              task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length =
                  0x00;
              /* In case of link drop, overwrite with near_end = TRUE */
              task_cmd->cmd.call_disconnected_envelope_ind.near_end = TRUE;
            }
          }
          break;

        case CM_CALL_MODE_INFO_CDMA:
          task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.present =
            FALSE;
          task_cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list[0] =
            call_info_ptr->mode_info.info.cdma_call.con_ref;
          break;
        default:
          UIM_MSG_ERR_1("invalid call type 0x%x",
                        call_info_ptr->mode_info.info_type);
        }

        gstk_task_put_cmd_buf(task_cmd);
      }
      break;

    case CM_CALL_EVENT_INCOM:
    case CM_CALL_EVENT_SETUP_IND:      /* phone received a setup ind message */
      if(call_info_ptr->call_type != CM_CALL_TYPE_VOICE   &&
         call_info_ptr->call_type != CM_CALL_TYPE_CS_DATA)
      {
        break;
      }

      switch(call_info_ptr->mode_info.info_type)
      {
        case CM_CALL_MODE_INFO_GW_PS:
        break;
        case CM_CALL_MODE_INFO_CDMA:
        case CM_CALL_MODE_INFO_GW_CS:
          if(call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS  &&
             call_event == CM_CALL_EVENT_INCOM)
          {
            /* for GW voice call, CM_CALL_EVENT_SETUP_IND is used */
            break;
          }

          /* get command buf */
          task_cmd = gstk_task_get_cmd_buf();
          if(task_cmd != NULL)
          {
            /* successfully obtained command buffer */
            task_cmd->cmd.mt_evt_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint8)slot - 1]->client_id;
            task_cmd->cmd.mt_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
            task_cmd->cmd.mt_evt_envelope_ind.message_header.command_id    = (int)GSTK_MT_CALL_EVT_IND;
            task_cmd->cmd.mt_evt_envelope_ind.message_header.user_data     = 0;
            task_cmd->cmd.mt_evt_envelope_ind.message_header.sim_slot_id   = slot;

            /* address */
            /* non BCD address */
            /*lint -e506 constant value boolean */
            if (call_info_ptr->num.len > 0)
            {
              task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.TON =
                         (gstk_ton_enum_type)call_info_ptr->num.number_type;
              task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.NPI =
                         (gstk_npi_enum_type)call_info_ptr->num.number_plan;
#if defined(CM_MAX_NUMBER_CHARS)
              length = MIN(CM_MAX_NUMBER_CHARS, GSTK_MAX_ADDRESS_LEN);
#elif defined(CM_MAX_CALLED_INFO_CHARS_UMTS)
              length = MIN(CM_MAX_CALLED_INFO_CHARS_UMTS, GSTK_MAX_ADDRESS_LEN);
#else
              length = GSTK_MAX_ADDRESS_LEN;
#endif /* CM_MAX_NUMBER_CHARS */
              length = MIN(length,call_info_ptr->num.len);
              task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.length = length;
              (void)memscpy(task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.address,
              	     task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.length,
                     call_info_ptr->num.buf,
                     task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.length);
            }

            /* subaddress */
            if (call_info_ptr->num.subaddr.num_char > 0)
            {
              length = MIN(call_info_ptr->num.subaddr.num_char,GSTK_MAX_SUBADDR_LEN);
              task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.length = length;
              (void)memscpy(task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.subaddress,
              	     task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.length,
                     call_info_ptr->num.subaddr.chari,
                     task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.length);
            }

            /* transaction id */
            task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.length = 1;
            task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list =
              GSTK_CALLOC(task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.length);
            if (task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list == NULL)
            {
              /* put the queue back to the free queue */
              gstk_task_free_cmd_buf(task_cmd);
             return;
            }

            switch (call_info_ptr->mode_info.info_type)
            {
              case CM_CALL_MODE_INFO_GW_CS:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
                /* TI flag is hardcoded to 0 because SETUP IND is always sent from
                   TI originator (NW) and recived by UE */
                gstk_util_translate_cm_transaction_id_to_gstk_id(
                  &task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id,
                  FALSE,
                  &(call_info_ptr->mode_info.info.gw_cs_call.transaction_id));
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
                break;
              case CM_CALL_MODE_INFO_CDMA:
                task_cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list[0] =
                  call_info_ptr->mode_info.info.cdma_call.con_ref;
                break;
              default:
                UIM_MSG_ERR_1("invalid call type 0x%x",
                              call_info_ptr->mode_info.info_type);
            }
            gstk_task_put_cmd_buf(task_cmd);
          }
          break;

        default:  /* CM_CALL_MODE_INFO_NONE, CM_CALL_MODE_INFO_MAX */
          break;
      }

      break;

    case CM_CALL_EVENT_OTASP_STATUS:
      if((gstk_instances_ptr[(uint8)slot - 1]->otasp_call_id >= 0) &&
         (int32touint8(gstk_instances_ptr[(uint8)slot - 1]->otasp_call_id) == call_info_ptr->call_id))
      {
        if (call_info_ptr->mode_info.info.cdma_call.otasp_status == CM_OTASP_STATUS_COMMITTED)
        {
          UIM_MSG_HIGH_0("Received OTASP COMMIT for OTASP call");
          gstk_instances_ptr[(uint8)slot - 1]->recv_otasp_commit = TRUE;
          if(gstk_shared_data.gstk_otasp_status_cb != NULL)
          {
            (gstk_shared_data.gstk_otasp_status_cb)(slot, GSTK_OTASP_ACT_STATUS_SUCCESS);
          }
        }
      }

      break;
    default:
      UIM_MSG_LOW_0("No relevant Call events");
      break;
  }
} /* gstk_cm_call_event_cb */
/*lint +e715 "call_info_ptr call_event not referenced . This lint error is
  suppressed when FEATURE_GSM or FEATURE_WCDMA is not defined." */

/*===========================================================================
FUNCTION gstk_mt_call_evt_ind

DESCRIPTION

  This function builds the mt call command and sends it to UIM

PARAMETERS
  mt_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_mt_call_evt_ind(
        gstk_envelope_mt_evt_ind_type    * mt_evt_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_mt_evt_dl_command_type        STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;

   if(mt_evt_cmd == NULL)
   {
     UIM_MSG_ERR_0("mt_evt_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_mt_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_MT_CALL,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.event_download_hdr.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* transaction id tag */
   if(mt_evt_cmd->mt_evt_data.transaction_id.length > 0)
   {
     STK_envelope_cmd.transacton_id.transaction_list =
       GSTK_CALLOC(mt_evt_cmd->mt_evt_data.transaction_id.length);
     if(STK_envelope_cmd.transacton_id.transaction_list == NULL)
     {
       UIM_MSG_ERR_0("malloc failure");
       return GSTK_MEMORY_ERROR;
     }
   }
   gstk_status = gstk_packer_transaction_id_tlv(
       &mt_evt_cmd->mt_evt_data.transaction_id,
       &STK_envelope_cmd.transacton_id);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* address tag */
   if(mt_evt_cmd->mt_evt_data.address.length != 0) {
     gstk_status = gstk_packer_address_tlv(GSTK_ADDRESS_TAG,
                             &mt_evt_cmd->mt_evt_data.address,
                             &STK_envelope_cmd.address);
     if(gstk_status != GSTK_SUCCESS) {
       return gstk_status;
     }
   }
   if(mt_evt_cmd->mt_evt_data.subaddress.length != 0) {
     gstk_status = gstk_packer_subaddress_tlv(
          &mt_evt_cmd->mt_evt_data.subaddress,
          &STK_envelope_cmd.subaddress);
     if (gstk_status != GSTK_SUCCESS) {
         return gstk_status;
     }
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + transaction id data + 2
                        + address data + 2
                        + subaddress data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.transacton_id.length +
                             6;
   if(mt_evt_cmd->mt_evt_data.address.length != 0) {
     STK_envelope_cmd.event_download_hdr.length = STK_envelope_cmd.event_download_hdr.length +
                               STK_envelope_cmd.address.length +
                               GSTK_TAG_LENGTH_LEN;
   }
   if(mt_evt_cmd->mt_evt_data.subaddress.length != 0) {
     STK_envelope_cmd.event_download_hdr.length = STK_envelope_cmd.event_download_hdr.length +
                               STK_envelope_cmd.subaddress.length +
                               GSTK_TAG_LENGTH_LEN;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = mt_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                           + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.event_download_hdr.evt_list),
                                  offset,
                                  (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + 2),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.event_download_hdr.evt_list)) <
          (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + 2))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.event_download_hdr.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.evt_list.length + 2),
                                       (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
                                                + GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
               (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
                        + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy transaction id */
       /* tag, length */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.transacton_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                       GSTK_TAG_LENGTH_LEN,
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.transacton_id)) < GSTK_TAG_LENGTH_LEN)
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       offset = (int)(offset + GSTK_TAG_LENGTH_LEN);
       if(gstk_status == GSTK_SUCCESS &&
          STK_envelope_cmd.transacton_id.length != 0 &&
          (STK_envelope_cmd.transacton_id.transaction_list != NULL) &&
          ((size_t)offset <= (sizeof(uim_cmd_ptr->envelope.data) - STK_envelope_cmd.transacton_id.length)))
       {
         if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                    sizeof(uim_cmd_ptr->envelope.data)-offset,
                    STK_envelope_cmd.transacton_id.transaction_list,
                    (size_t)STK_envelope_cmd.transacton_id.length) <
            (size_t)STK_envelope_cmd.transacton_id.length)
         {
           UIM_MSG_ERR_0("gstk_mt_call_evt_ind(): gstk_byte_offset_memcpy failed");
           gstk_status = GSTK_MEMORY_ERROR;
         }
         offset = (int)(offset + STK_envelope_cmd.transacton_id.length);
       }

       /* copy address */
       if(gstk_status == GSTK_SUCCESS &&
          STK_envelope_cmd.address.length != 0 &&
          (offset + STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN - 1)
            < UIM_MAX_ENVELOPE_BYTES) { /* has address */
          if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                     sizeof(uim_cmd_ptr->envelope.data)-offset,
                     &STK_envelope_cmd.address,
                     (size_t)(STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN)) <
             (size_t)(STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN))
          {
            UIM_MSG_ERR_0("gstk_mt_call_evt_ind(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
         offset = (int)(offset + STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN);
       }

       /* copy subaddress */
       if(gstk_status == GSTK_SUCCESS &&
          STK_envelope_cmd.subaddress.length != 0 &&
          (offset + STK_envelope_cmd.subaddress.length + GSTK_TAG_LENGTH_LEN - 1)
            < UIM_MAX_ENVELOPE_BYTES) { /* has subaddress */
          if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                     sizeof(uim_cmd_ptr->envelope.data)-offset,
                     &STK_envelope_cmd.subaddress,
                     (size_t)(STK_envelope_cmd.subaddress.length + GSTK_TAG_LENGTH_LEN)) <
             (size_t)(STK_envelope_cmd.subaddress.length + GSTK_TAG_LENGTH_LEN))
          {
            UIM_MSG_ERR_0("gstk_mt_call_evt_ind(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
         offset = (int)(offset + STK_envelope_cmd.subaddress.length + GSTK_TAG_LENGTH_LEN);
       }
       if (gstk_status == GSTK_SUCCESS)
       {
       gstk_util_dump_byte_array("MT evt",
                                 &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                 uim_cmd_ptr->envelope.num_bytes);
       }

       /* free envelope mt ind and tlv */
       if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }

       gstk_util_release_upper_layer_cmd_memory(
         (gstk_cmd_type*)((void*)mt_evt_cmd));/*lint !e826 area too small */

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_mt_call_evt_ind*/

/*===========================================================================
FUNCTION gstk_call_connected_evt_ind

DESCRIPTION

  This function builds the call connected envelope command and sends it to UIM

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_call_connected_evt_ind(
        gstk_envelope_call_connected_evt_ind_type    * call_connected_evt_cmd)
{
   gstk_status_enum_type                                gstk_status = GSTK_SUCCESS;
   gstk_envelope_call_connected_evt_dl_command_type     STK_envelope_cmd;
   uim_cmd_type                                         *uim_cmd_ptr;
   int                                                  offset = 0;

   if(call_connected_evt_cmd == NULL)
   {
     UIM_MSG_ERR_0("call_connected_evt_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_call_connected_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_CALL_CONNECTED,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   if(call_connected_evt_cmd->near_end) {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }
   else {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_NETWORK_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* transaction id tag */
   if(call_connected_evt_cmd->transaction_id.length > 0)
   {
     STK_envelope_cmd.transacton_id.transaction_list =
       GSTK_CALLOC(call_connected_evt_cmd->transaction_id.length);
     if(STK_envelope_cmd.transacton_id.transaction_list == NULL)
     {
       UIM_MSG_ERR_0("malloc failure");
       return GSTK_MEMORY_ERROR;
     }
   }
   gstk_status = gstk_packer_transaction_id_tlv(
       &call_connected_evt_cmd->transaction_id,
       &STK_envelope_cmd.transacton_id);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + transaction id data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.transacton_id.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = call_connected_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                  sizeof(uim_cmd_ptr->envelope.data)-offset,
                  &STK_envelope_cmd.event_download_hdr.evt_list,
                  (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length
                    + GSTK_TAG_LENGTH_LEN)) <
          (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.event_download_hdr.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
               (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy transaction id */
       /* tag, length */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.transacton_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                       GSTK_TAG_LENGTH_LEN,
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.transacton_id)) < GSTK_TAG_LENGTH_LEN)
       {
         UIM_MSG_ERR_0("gstk_mt_call_evt_ind(): gstk_byte_offset_memcpy failed");
         gstk_status = GSTK_MEMORY_ERROR;
       }
       offset = (int)(offset + GSTK_TAG_LENGTH_LEN);
       if(gstk_status == GSTK_SUCCESS &&
          STK_envelope_cmd.transacton_id.length != 0
          &&
          (STK_envelope_cmd.transacton_id.transaction_list != NULL)
          &&
          ((size_t)offset <= (sizeof(uim_cmd_ptr->envelope.data)
                      - STK_envelope_cmd.transacton_id.length)))
       {
         if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                    sizeof(uim_cmd_ptr->envelope.data)-offset,
                    STK_envelope_cmd.transacton_id.transaction_list,
                    (size_t)STK_envelope_cmd.transacton_id.length) <
            (size_t)STK_envelope_cmd.transacton_id.length)
         {
           UIM_MSG_ERR_0("gstk_mt_call_evt_ind(): gstk_byte_offset_memcpy failed");
           gstk_status = GSTK_MEMORY_ERROR;
         }
         offset = (int)(offset + STK_envelope_cmd.transacton_id.length);
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Call connected evt",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);
       }

       /* free envelope call connected ind and tlv */
       if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }
       gstk_util_release_upper_layer_cmd_memory(
         (gstk_cmd_type*)((void*)call_connected_evt_cmd));/*lint !e826 area too small */

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_call_connected_evt_ind*/

/*===========================================================================
FUNCTION gstk_call_disconnected_evt_ind

DESCRIPTION

  This function builds the call disconnected envelope command and sends it
  to UIM

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_call_disconnected_evt_ind(
        gstk_envelope_call_disconnected_evt_ind_type    * call_disconnected_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_call_disconnected_evt_dl_command_type     STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;

   if(call_disconnected_evt_cmd == NULL)
   {
     UIM_MSG_ERR_0("call_disconnected_evt_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_call_disconnected_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_CALL_DISCONNECTED,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   if(call_disconnected_evt_cmd->near_end) {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }
   else {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_NETWORK_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* transaction id tag */
   if(call_disconnected_evt_cmd->call_disconnected_data.transaction_id.length > 0)
   {
     STK_envelope_cmd.transacton_id.transaction_list =
       GSTK_CALLOC(
                  call_disconnected_evt_cmd->call_disconnected_data.transaction_id.length);
     if(STK_envelope_cmd.transacton_id.transaction_list == NULL)
     {
       UIM_MSG_ERR_0("malloc failure");
       return GSTK_MEMORY_ERROR;
     }
   }
   gstk_status = gstk_packer_transaction_id_tlv(
       &call_disconnected_evt_cmd->call_disconnected_data.transaction_id,
       &STK_envelope_cmd.transacton_id);

   if (gstk_status != GSTK_SUCCESS) {
       if (STK_envelope_cmd.transacton_id.transaction_list != NULL)
       {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }
       return gstk_status;
   }

   /* cause tag */
   if(call_disconnected_evt_cmd->call_disconnected_data.cause.present == TRUE)
   {
     UIM_MSG_HIGH_0("Call Disconnected Cause present ");
     if(call_disconnected_evt_cmd->call_disconnected_data.cause.length > 0)
     {
       STK_envelope_cmd.cause.cause =
         GSTK_CALLOC(call_disconnected_evt_cmd->call_disconnected_data.cause.length);
       if(STK_envelope_cmd.cause.cause == NULL)
       {
         UIM_MSG_ERR_0("malloc failure");
         /* free envelope call disconnected ind and tlv */
         if(STK_envelope_cmd.transacton_id.transaction_list != NULL)
         {
           gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
         }
         return GSTK_MEMORY_ERROR;
       }
     }
     gstk_status = gstk_packer_cause_tlv(
         &call_disconnected_evt_cmd->call_disconnected_data.cause,
         &STK_envelope_cmd.cause);
     }

   if (gstk_status != GSTK_SUCCESS)
   {
     if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
       gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
     }
     return gstk_status;
   }


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = call_disconnected_evt_cmd->message_header.user_data;

       /* initialized to 3 for value portion of the TLVs
          i.e., the final offset will be either 0 or 1 depends
          on whether the total length is > 0x7F or not */
       offset = 3;

       /* copy evt list */
       (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
              sizeof(uim_cmd_ptr->envelope.data)-offset,
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length
                     + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length
                     + GSTK_TAG_LENGTH_LEN);
       STK_envelope_cmd.event_download_hdr.length =
                      STK_envelope_cmd.event_download_hdr.evt_list.length
                     + GSTK_TAG_LENGTH_LEN;

       /* copy device */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.event_download_hdr.device_id),
                                  offset,
                                  (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                  GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
          (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                   GSTK_TAG_LENGTH_LEN))
       {
         UIM_MSG_ERR_0("gstk_call_disconnected_evt_ind(): gstk_byte_offset_memcpy failed");
         gstk_status = GSTK_MEMORY_ERROR;
       }

       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
                   + GSTK_TAG_LENGTH_LEN);
                   
       STK_envelope_cmd.event_download_hdr.length +=
                         STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
                         + GSTK_TAG_LENGTH_LEN;

       /* copy transaction id */
       /* tag, length */
       if (gstk_status == GSTK_SUCCESS &&
           (offset + GSTK_TAG_LENGTH_LEN -1) < UIM_MAX_ENVELOPE_BYTES)
       {
         if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
              sizeof(uim_cmd_ptr->envelope.data)-offset,
              &STK_envelope_cmd.transacton_id,
              GSTK_TAG_LENGTH_LEN) < GSTK_TAG_LENGTH_LEN)
         {
           UIM_MSG_ERR_0("gstk_call_disconnected_evt_ind(): gstk_byte_offset_memcpy failed");
           gstk_status = GSTK_MEMORY_ERROR;
         }
         offset = (int)(offset + GSTK_TAG_LENGTH_LEN);
       }
       if(gstk_status == GSTK_SUCCESS &&
          STK_envelope_cmd.transacton_id.length != 0
          &&
          (offset + STK_envelope_cmd.transacton_id.length - 1) < UIM_MAX_ENVELOPE_BYTES
          &&
          STK_envelope_cmd.transacton_id.transaction_list != NULL)
       {
         if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                    sizeof(uim_cmd_ptr->envelope.data)-offset,
                    STK_envelope_cmd.transacton_id.transaction_list,
                    (size_t)STK_envelope_cmd.transacton_id.length) !=
            (size_t)STK_envelope_cmd.transacton_id.length)
         {
           UIM_MSG_ERR_0("gstk_call_disconnected_evt_ind(): gstk_byte_offset_memcpy failed");
           gstk_status = GSTK_MEMORY_ERROR;
         }
         offset = (int)(offset + STK_envelope_cmd.transacton_id.length);
       }
       if (gstk_status == GSTK_SUCCESS)
       {
       STK_envelope_cmd.event_download_hdr.length +=
                         STK_envelope_cmd.transacton_id.length
                         + GSTK_TAG_LENGTH_LEN;

       if((call_disconnected_evt_cmd->call_disconnected_data.cause.present == TRUE)
          &&
          (((STK_envelope_cmd.cause.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
            && (offset + 2) < UIM_MAX_ENVELOPE_BYTES)
           ||
           ((STK_envelope_cmd.cause.length <= GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
            && (offset + 1) < UIM_MAX_ENVELOPE_BYTES))) {
         /* copy cause */
         /* tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cause.tag;
         if (STK_envelope_cmd.cause.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.event_download_hdr.length++;
         }
         if(offset < UIM_MAX_ENVELOPE_BYTES)
         {
           uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cause.length;
         }
		 
         if(STK_envelope_cmd.cause.length != 0
            &&
            (offset + STK_envelope_cmd.cause.length -1) < UIM_MAX_ENVELOPE_BYTES
            &&
            STK_envelope_cmd.cause.cause != NULL)
         {
           (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                  sizeof(uim_cmd_ptr->envelope.data)-offset,
                  STK_envelope_cmd.cause.cause,
                  (int)STK_envelope_cmd.cause.length);
           offset = (int)(offset + STK_envelope_cmd.cause.length);
         }

         STK_envelope_cmd.event_download_hdr.length +=
                         STK_envelope_cmd.cause.length
                         + GSTK_TAG_LENGTH_LEN;
       }

       /* populate the envelope command info */
       /* total data length:  evt id data + 2
                    + device id data + 2 (2 is for the tag and length field)
                    + transaction id data + 2
                    + cause data + 2
       */
       if(STK_envelope_cmd.event_download_hdr.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
         /* account for 0x80 */
         uim_cmd_ptr->envelope.offset = 0;
         uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.event_download_hdr.tag;
         uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.event_download_hdr.length;
         STK_envelope_cmd.event_download_hdr.length++;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                           + GSTK_TAG_LENGTH_LEN;

       }
       else {
         uim_cmd_ptr->envelope.offset = 1;
         uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.event_download_hdr.tag;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.event_download_hdr.length;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                            + GSTK_TAG_LENGTH_LEN;
       }

       gstk_util_dump_byte_array("Call disconnected evt", 
                                 &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                                 uim_cmd_ptr->envelope.num_bytes);
       }

       /* free envelope call disconnected ind and tlv */
       if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }
       if(STK_envelope_cmd.cause.cause != NULL) {
         gstk_free(STK_envelope_cmd.cause.cause);
       }
       gstk_util_release_upper_layer_cmd_memory(
         (gstk_cmd_type*)((void*)call_disconnected_evt_cmd));/*lint !e826 area too small */

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_call_disconnected_evt_ind*/




/*===========================================================================
FUNCTION gstk_idle_scrn_evt_ind

DESCRIPTION

  This function builds the idle screen command and sends it to UIM

PARAMETERS
  idle_scrn_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_idle_scrn_evt_ind(
  const gstk_envelope_idle_scrn_evt_ind_type * idle_scrn_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_idle_scrn_evt_dl_command_type   STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;

   if(idle_scrn_cmd == NULL)
   {
     UIM_MSG_ERR_0("idle_scrn_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_idle_scrn_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_IDLE_SCRN_AVAILABLE,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_DISPLAY_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = idle_scrn_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.evt_list),
                                  offset,
                                  (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.evt_list)) <
          (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.device_id)) <
               (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Idle Screen Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* nothing to free envelope idle screen evt ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_idle_scrn_evt_ind */

/*===========================================================================
FUNCTION gstk_hci_connectivity_evt_ind

DESCRIPTION

  This function builds the HCI Connectvity command and sends it to UIM

PARAMETERS
  hci_conn_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_hci_connectivity_evt_ind(
  const gstk_envelope_hci_connectivity_ind_type *hci_conn_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_hci_conn_evt_dl_command_type    STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;

   if(hci_conn_cmd == NULL)
   {
     UIM_MSG_ERR_0("hci_conn_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_hci_conn_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_HCI_CONNECTIVITY,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = hci_conn_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.evt_list),
                                  offset,
                                  (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.evt_list)) <
          (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.device_id)) <
               (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("HCI Connectivity Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* nothing to free envelope hci connectivity evt ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_hci_connectivity_evt_ind */

/*===========================================================================
FUNCTION gstk_user_act_evt_ind

DESCRIPTION

  This function builds the user activity command and sends it to UIM

PARAMETERS
  user_act_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_user_act_evt_ind(
  const gstk_envelope_user_act_evt_ind_type * user_act_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_user_act_evt_dl_command_type    STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;

   if(user_act_cmd == NULL)
   {
     UIM_MSG_ERR_0("user_act_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_user_act_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_USER_ACTIVITY,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = user_act_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.evt_list),
                                  offset,
                                  (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.evt_list)) <
          (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.device_id)) <
               (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("User Activity Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* nothing to free envelope user ativity evt ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_user_act_evt_ind */

/*===========================================================================
FUNCTION gstk_process_and_store_env_cmd_for_retry

DESCRIPTION

  This function stores the event download envelope information before the
  envelope is attempted to be sent to the card at the very first time.

PARAMETERS
  data_ptr [Input] : envelope data bytes
  len      [Input] : length of envelope data
  ref_id   [Input]:  envelope reference id in the envelope reference table

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_SUCCESS  : if envelope info is stored successfully.
    GSTK_ERROR    : if there is an envelope already pending.
    GSTK_BAD_PARAM: bad parameter

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_process_and_store_env_cmd_for_retry(
  const byte *data_ptr,
  uint32      len,
  uint32      ref_id)
{
  uint32                           alloc_len    = 0;
  gstk_nv_items_data_type          nv_data;

  memset(&nv_data, 0, sizeof(nv_data));
  
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  (void)gstk_nv_access_read(GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,
                          gstk_curr_inst_ptr->slot_id,
                          &nv_data);

  if((nv_data.env_retry_ext.max_retries == 0) || (nv_data.env_retry_ext.duration == 0))
  {
    UIM_MSG_ERR_2("Envelope retry data max_retries 0x%x, timer duration 0x%x",
                  nv_data.env_retry_ext.max_retries, nv_data.env_retry_ext.duration);  
    return GSTK_ERROR;
  }

  if((!data_ptr)||(len == 0))
  {
    UIM_MSG_ERR_2("gstk_process_and_store_env_cmd_for_retry: Parameter error: data_ptr=0x%x len=%d",
                  data_ptr, len);
    return GSTK_BAD_PARAM;
  }

  if ((ref_id == GSTK_CMD_DETAILS_REF_TABLE_FULL) ||
      (ref_id > (GSTK_MAX_PROACTIVE_COMMAND-1)))
  {
    UIM_MSG_ERR_0("Invalid env ref id");
    return GSTK_BAD_PARAM;
  }
  if(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_NONE)
  {
    if(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr != NULL)
    {
      /* Should we rather free the ptr and allocate? */
      UIM_MSG_ERR_1("env apdu ptr should have been NULL : env_state 0x%x",
                    gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state);
      return GSTK_ERROR;
    }
    else
    {
      alloc_len = MIN(len, UIM_MAX_ENVELOPE_BYTES);
      if(data_ptr[0] == 0x00)
      {
        alloc_len = alloc_len + 1;
      }
      gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr = GSTK_CALLOC(alloc_len);
      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr == NULL )
      {
        return GSTK_MEMORY_ERROR;
      }
      if(gstk_memcpy(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr, /* dest ptr */
                     data_ptr,                                                                   /* src ptr */
                     alloc_len,                                                                  /* copy size */
                     alloc_len,                                                                  /* dest len */
                     UIM_MAX_ENVELOPE_BYTES) <                                                   /* src len */
         len)
      {
        UIM_MSG_ERR_0("gstk_process_and_store_env_cmd_for_retry(): gstk_memcpy failed");
        gstk_free(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr);
        gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr = NULL;
        return GSTK_MEMORY_ERROR;
      }
      gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_len      = uint32touint8(len);
      gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id                   = ref_id;
      return GSTK_SUCCESS;
    }
  }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_NONE) */
  else
  {
    /* We should not be processing another evt dl envelope when the state
       indicates that there is one already pending */
    UIM_MSG_ERR_1("Wrong env state 0x%x : Attemping to send 2nd evt dl envelope - LS to the card",
                  gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state);
    return GSTK_ERROR;
  }
}/* gstk_process_and_store_env_cmd_for_retry */

/*===========================================================================
FUNCTION gstk_location_status_evt_ind

DESCRIPTION

  This function builds the location status event download envelope command
  and sends it to UIM

PARAMETERS
  locaiton_status_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_location_status_evt_ind(
  const gstk_envelope_location_statue_ind_type    *location_status_evt_cmd)
{
  gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
  gstk_envelope_location_status_evt_dl_command_type       STK_envelope_cmd;
  uim_cmd_type                                            *uim_cmd_ptr;
  int                                                     offset = 0;
  gstk_location_info_rsp_type                             location_info;
  gstk_nv_items_data_type                                 nv_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(location_status_evt_cmd == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  /* initialize STK_envelope_cmd, location_info */
  memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_location_status_evt_dl_command_type));
  memset(&location_info, 0, sizeof(gstk_location_info_rsp_type));

  /* Pack various TLVs */
  STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

  /* event list */
  gstk_status = gstk_packer_evt_list_tlv(
      GSTK_LOCATION_STATUS,
      &STK_envelope_cmd.event_download_hdr.evt_list );

  if (gstk_status != GSTK_SUCCESS) {
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
        GSTK_ME_DEVICE,
        GSTK_UICC_SIM_DEVICE,
        &STK_envelope_cmd.event_download_hdr.device_id );

  if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
  }

  /* location status tag */
  gstk_status = gstk_packer_location_status_tlv(
      gstk_curr_inst_ptr->gstk_curr_location_status,
      &STK_envelope_cmd.location_st);

  if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
  }

  UIM_MSG_HIGH_2("Access Tech: 0x%x, Loc status: 0x%x",
                 location_status_evt_cmd->location_info.info_type,
                 location_status_evt_cmd->location_status);
  /* Check for Access Technology */
  switch(location_status_evt_cmd->location_info.info_type)
  {
  case GSTK_CDMA_LOCATION_INFO:
    /* Fill in location information if location status indicates Normal Service */
    if(location_status_evt_cmd->location_status == GSTK_NORMAL_SERVICE)
    {
#ifdef FEATURE_CDMA
      if (gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
            gstk_curr_inst_ptr->slot_id) == TRUE)
      {
        /* CDMA LS envelope triggers IMSI REFRESH. So, invalidate the cache to
           ensure VPE is sent when NAS tries for ATTACH on 3GPP next time when
           IMSI changes due to REFERSH */
        gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_INVALID;
        gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_INVALID;
        memset(gstk_curr_inst_ptr->gstk_last_loc_info_sent,
               0xFF,
               sizeof(gstk_curr_inst_ptr->gstk_last_loc_info_sent));
      }
      gstk_status = gstk_packer_3gpp2_location_info_tlv(
        &gstk_curr_inst_ptr->cdma_ext_loci_cache.data,
        &gstk_curr_inst_ptr->cdma_ext_loci_cache,
        &STK_envelope_cmd.location_info.cdma_ext_loc_info);
      if (gstk_status != GSTK_SUCCESS)
      {
        return gstk_status;
      }
#endif /* #ifdef FEATURE_CDMA */
    }
    break;

  case GSTK_GSM_UMTS_LOCATION_INFO:
    /* Fill in location information if location status indicates Normal Service */
    if(gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_NORMAL_SERVICE)
    {
      location_info.cell_id_len = gstk_curr_inst_ptr->gstk_curr_cell_id.cell_len;
      if(gstk_curr_inst_ptr->card_protocol == MMGSDI_ICC)
      {
        location_info.cell_id_len = GSTK_MIN_CELL_ID_LEN;
      }

      (void)gstk_memcpy(location_info.cell_id,
      	       gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id,
               location_info.cell_id_len,
      	       sizeof(location_info.cell_id),
               gstk_curr_inst_ptr->gstk_curr_cell_id.cell_len);
      (void)memscpy(location_info.mcc_and_mnc,
      	       sizeof(location_info.mcc_and_mnc),
      	       gstk_curr_inst_ptr->gstk_curr_mcc_mnc, 3);
      (void)memscpy(location_info.loc_area_code,
      	       sizeof(location_info.loc_area_code),
      	       gstk_curr_inst_ptr->gstk_curr_lac, 2);
      gstk_status = gstk_packer_location_info_tlv(
        &location_info,
        &STK_envelope_cmd.location_info.gsm_umts_location_info);

      if (gstk_status != GSTK_SUCCESS) {
        return gstk_status;
        }
      UIM_MSG_HIGH_2("  Envelope  Location status is 0x%x  cell id len 0x%x",
                     gstk_curr_inst_ptr->gstk_curr_location_status,
                     location_info.cell_id_len);
      UIM_MSG_HIGH_3("  Envelope  MNC_MCC is 0x%x 0x%x 0x%x",
                     location_info.mcc_and_mnc[0],
                     location_info.mcc_and_mnc[1],
                     location_info.mcc_and_mnc[2]);

      UIM_MSG_HIGH_2(" Envelope LAC is 0x%x 0x%x",
                     location_info.loc_area_code[0],
                     location_info.loc_area_code[1]);

      if(location_info.cell_id_len >= 2)
      {
        UIM_MSG_HIGH_2("Envelope Cell Id [0][1]: 0x%x 0x%x",
                       location_info.cell_id[0], location_info.cell_id[1]);
      }
      if(location_info.cell_id_len >= 4)
      {
          UIM_MSG_HIGH_2("Envelope Cell Id [2][3]: 0x%x 0x%x",
                         location_info.cell_id[2], location_info.cell_id[3]);
      }
    }
    break;
  case GSTK_NO_SRVC_LOCATION_INFO:
    break;
  default:
    UIM_MSG_ERR_0("Invalid Location Info Type");
    /* Need to reset pending envelope flag ? */
    return GSTK_ERROR;
  }/* End of switch location_status_evt_cmd->location_info.location_type */

  /* total data length:  evt id data + 2
                      + device id data + 2 (2 is for the tag and length field)
                      + location status data + 2
                      + location info + 2 (C)
  */
  STK_envelope_cmd.event_download_hdr.length =
                            STK_envelope_cmd.event_download_hdr.evt_list.length +
                            STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                            STK_envelope_cmd.location_st.length +
                            6;

  if(gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_NORMAL_SERVICE)
  {
    switch(location_status_evt_cmd->location_info.info_type)
    {
    case GSTK_GSM_UMTS_LOCATION_INFO:
      STK_envelope_cmd.event_download_hdr.length =
        STK_envelope_cmd.event_download_hdr.length +
          STK_envelope_cmd.location_info.gsm_umts_location_info.length + 2;
      break;

    case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CDMA
      STK_envelope_cmd.event_download_hdr.length =
        STK_envelope_cmd.event_download_hdr.length +
        STK_envelope_cmd.location_info.cdma_ext_loc_info.length + 2;
#endif /* #ifdef FEATURE_CDMA */
      break;

    case GSTK_NO_SRVC_LOCATION_INFO:
    case GSTK_IMEI_INFO:
    case GSTK_NMR_BCCH_INFO:
    case GSTK_NMR_UTRAN_INFO:
    case GSTK_DATE_TIME_ZONE_INFO:
    case GSTK_LANGUAGE_SETTING_INFO:
    case GSTK_TIMING_ADVANCE_INFO:
    case GSTK_ACCESS_TECHNOLOGY_INFO:
    case GSTK_ESN_INFO:
    case GSTK_IMEISV_INFO:
    case GSTK_BATTERY_STATUS_INFO:
    case GSTK_MEID_INFO:
    case GSTK_INVALID_LOC_INFO:
    case GSTK_NETWORK_SEARCH_MODE_INFO:
    case GSTK_CSG_CELL_INFO:
      UIM_MSG_ERR_1("Invalid general local info type 0x%x",
                    location_status_evt_cmd->location_info.info_type);
      return GSTK_ERROR;
    }
  }
  /* Send command to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
      /* set to 0 so that uim will get the data from index 0 */
      uim_cmd_ptr->envelope.offset = 0x00;

      /* set user_data */
      uim_cmd_ptr->hdr.user_data               = location_status_evt_cmd->message_header.user_data;

      /* populate the envelope command info */
      uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                      + GSTK_TAG_LENGTH_LEN;

      uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
      uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

      /* copy evt list */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &(STK_envelope_cmd.event_download_hdr.evt_list),
                                 offset,
                                 (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                          GSTK_TAG_LENGTH_LEN),
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.event_download_hdr.evt_list)) <
         (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* copy device */
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                      &(STK_envelope_cmd.event_download_hdr.device_id),
                                      offset +=
                                      (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                            GSTK_TAG_LENGTH_LEN),
                                      (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                               GSTK_TAG_LENGTH_LEN),
                                      sizeof(uim_cmd_ptr->envelope.data),
                                      sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
              (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* copy location status */
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                      &(STK_envelope_cmd.location_st),
                                      offset +=
                                      (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                            GSTK_TAG_LENGTH_LEN),
                                      (size_t)(STK_envelope_cmd.location_st.length + GSTK_TAG_LENGTH_LEN),
                                      sizeof(uim_cmd_ptr->envelope.data),
                                      sizeof(STK_envelope_cmd.location_st)) <
              (size_t)(STK_envelope_cmd.location_st.length + GSTK_TAG_LENGTH_LEN))
      {
        UIM_MSG_ERR_0("gstk_location_status_evt_ind(): gstk_byte_offset_memcpy failed");
        gstk_status = GSTK_MEMORY_ERROR;
      }

      offset = (int)(offset + STK_envelope_cmd.location_st.length + GSTK_TAG_LENGTH_LEN);

        /* copy location info */
      if(gstk_status == GSTK_SUCCESS &&
         gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_NORMAL_SERVICE)
      {
        switch(location_status_evt_cmd->location_info.info_type)
        {
        case GSTK_GSM_UMTS_LOCATION_INFO:
          if ((offset + STK_envelope_cmd.location_info.gsm_umts_location_info.length + GSTK_TAG_LENGTH_LEN) < UIM_MAX_ENVELOPE_BYTES)
          {
            if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                       sizeof(uim_cmd_ptr->envelope.data)-offset,
                       &STK_envelope_cmd.location_info.gsm_umts_location_info,
                       (size_t)(STK_envelope_cmd.location_info.gsm_umts_location_info.length + GSTK_TAG_LENGTH_LEN)) <
               (size_t)(STK_envelope_cmd.location_info.gsm_umts_location_info.length + GSTK_TAG_LENGTH_LEN))
            {
              UIM_MSG_ERR_0("gstk_location_status_evt_ind(): gstk_byte_offset_memcpy failed");
              gstk_status = GSTK_MEMORY_ERROR;
            }
            offset = (int)(offset + STK_envelope_cmd.location_info.gsm_umts_location_info.length);
          }
          break;

        case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CDMA
            if ((offset + GSTK_TAG_LENGTH_LEN) < UIM_MAX_ENVELOPE_BYTES)
            {
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.location_info.cdma_ext_loc_info.tag;
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.location_info.cdma_ext_loc_info.length;
            } 
            else 
            {
              UIM_MSG_ERR_0("gstk_location_status_evt_ind(): fail to copy CDMA location info");
              gstk_status = GSTK_MEMORY_ERROR;
            }

            if ((offset + STK_envelope_cmd.location_info.cdma_ext_loc_info.length) < UIM_MAX_ENVELOPE_BYTES)
            {
              if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                         sizeof(uim_cmd_ptr->envelope.data)-offset,
                         STK_envelope_cmd.location_info.cdma_ext_loc_info.loc_info,
                         (size_t)(STK_envelope_cmd.location_info.cdma_ext_loc_info.length)) <
                 (size_t)(STK_envelope_cmd.location_info.cdma_ext_loc_info.length))
              {
                UIM_MSG_ERR_0("gstk_location_status_evt_ind(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
              offset = (int)(offset + STK_envelope_cmd.location_info.cdma_ext_loc_info.length);
            } 
            else 
            {
              UIM_MSG_ERR_0("gstk_location_status_evt_ind(): fail to copy CDMA location info");
              gstk_status = GSTK_MEMORY_ERROR;
            }
#endif /* #ifdef FEATURE_CDMA */
          break;

        default:
          break;
        }
      }

      if (gstk_status == GSTK_SUCCESS)
      {
      gstk_util_dump_byte_array("Location info Env",
                                &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                uim_cmd_ptr->envelope.num_bytes);
      /* Only if envelope state is GSTK_NONE, change state to RSP_FROM_CARD
         and store the env payload
      */
      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
      {
        if ((gstk_nv_get_feature_status(
               GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
               gstk_curr_inst_ptr->slot_id) == TRUE) &&
            gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type == GSTK_LOCATION_STATUS_EVT_IND_RSP)
        {
          /* Cancel any Location Status envelope retry in progress */
          gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
        }

        gstk_status = gstk_process_and_store_env_cmd_for_retry(
                        uim_cmd_ptr->envelope.data,
                        (uint32)uim_cmd_ptr->envelope.num_bytes,
                        uim_cmd_ptr->hdr.user_data);
        if( gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
        }
        else
        {
          UIM_MSG_HIGH_0("FAIL - no retry will be attempted ");
          gstk_status = GSTK_SUCCESS;
        }
      }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
      else
      {
        UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
      }

      /* no free for envelope location info ind and tlv */
#if defined(FEATURE_GSTK_TAL)
      UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                     GSTK_TAL_LOCAL_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
      gstk_tal_notify_rat(GSTK_TAL_LOCAL_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#elif defined(FEATURE_GSTK_FUSION_REMOTE)
      UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                     GSTK_TAL_REMOTE_CLIENT,
                     gstk_curr_inst_ptr->gstk_curr_rat);
      gstk_tal_notify_rat(GSTK_TAL_REMOTE_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#endif /* FEATURE_GSTK_TAL */
      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
      else
      {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        uim_cmd_ptr = NULL;
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  if (gstk_status == GSTK_SUCCESS
      &&
      (gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
         gstk_curr_inst_ptr->slot_id) == TRUE))
  {
    gstk_util_remove_pending_env(GSTK_LOCATION_STATUS_EVT_IND);
  }

  if (location_status_evt_cmd->location_info.info_type == GSTK_CDMA_LOCATION_INFO)
  {
    gstk_util_location_info_tlv_cleanup(&STK_envelope_cmd.location_info.cdma_ext_loc_info);
  }
  return gstk_status;

} /* gstk_location_status_evt_ind*/

/*===========================================================================
FUNCTION gstk_lang_selection_evt_ind

DESCRIPTION

  This function builds the language selection event download envelope command
  and sends it to UIM

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_lang_selection_evt_ind(
  const gstk_envelope_lang_sel_evt_ind_type    * lang_select_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_language_evt_dl_command_type              STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;

   if(lang_select_evt_cmd == NULL)
   {
     UIM_MSG_ERR_0("lang_select_evt_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_language_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_LANGUAGE_SELECTION,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* lang code tag */
   gstk_status = gstk_packer_lang_sel_tlv(
       lang_select_evt_cmd->lang_code.lang_code,
       &STK_envelope_cmd.language);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + lang sel data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.language.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = lang_select_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
              sizeof(uim_cmd_ptr->envelope.data)-offset,
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length
                   + GSTK_TAG_LENGTH_LEN);
                   
       /* copy device */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.event_download_hdr.device_id),
                                  offset,
                                  (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                           GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
          (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       
       offset += (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN);
       /* copy language */
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                        &STK_envelope_cmd.language,
                                        offset,
                                        (size_t)(STK_envelope_cmd.language.length + GSTK_TAG_LENGTH_LEN),
                                        sizeof(uim_cmd_ptr->envelope.data)-offset,
                                        sizeof(STK_envelope_cmd.language)) <
                (size_t)(STK_envelope_cmd.language.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Lang select Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* no free for envelope location info ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_lang_selection_evt_ind*/


/*===========================================================================
FUNCTION gstk_browser_term_evt_ind

DESCRIPTION

  This function builds the browser termination event download envelope command
  and sends it to UIM

PARAMETERS
  browser_term_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_browser_term_evt_ind(
  const gstk_envelope_browser_term_evt_ind_type    * browser_term_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_browser_termination_evt_dl_command_type   STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_browser_termination_evt_dl_command_type));
   if (browser_term_evt_cmd == NULL) {
      return GSTK_ERROR;
   }

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_BROWSER_TERMINATION,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* browser termination cause tag */
   gstk_status = gstk_packer_browser_term_cause_tlv(
       &browser_term_evt_cmd->browser_termination_cause,
       &STK_envelope_cmd.cause);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + termination cause data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.cause.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = browser_term_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                   &STK_envelope_cmd.event_download_hdr.evt_list,
                                   offset,
                                   (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length
                                            + GSTK_TAG_LENGTH_LEN),
                                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                                   sizeof(STK_envelope_cmd.event_download_hdr.evt_list)) <
           (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.event_download_hdr.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
               (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       
       offset += (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN);
       /* copy cause */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.cause),
                                       offset,
                                       (size_t)(STK_envelope_cmd.cause.length + GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data)-offset,
                                       sizeof(STK_envelope_cmd.cause)) <
               (size_t)(STK_envelope_cmd.cause.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Browser Term Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* no free for envelope location info ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     UIM_MSG_ERR_0 ("no UIM cmd buf");
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_browser_term_evt_ind*/

/*===========================================================================
FUNCTION gstk_data_avail_evt_ind

DESCRIPTION

  This function builds the data available event download envelope command
  and sends it to UIM

PARAMETERS
  data_avail_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_data_avail_evt_ind(
  const gstk_envelope_data_avail_evt_ind_type *data_avail_evt_cmd
)
{
  gstk_status_enum_type                          gstk_status         = GSTK_SUCCESS;
  gstk_envelope_data_avail_evt_dl_command_type   STK_envelope_cmd;
  uim_cmd_type                                  *uim_cmd_ptr;
  int32                                          offset              = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_data_avail_evt_dl_command_type));

   if (data_avail_evt_cmd == NULL)
   {
      return GSTK_ERROR;
   }

   /* Pack various TLVs
   1. Event List TLV
   2. Device Id TLV
   3. Channel Status TLV
   4. Channel Data Length TLV
   */

   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* pack the event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_DATA_AVAILABLE,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS)
   {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS)
   {
       return gstk_status;
   }

   gstk_status = gstk_packer_ch_status_tlv(
     &data_avail_evt_cmd->ch_status_info,
     &STK_envelope_cmd.ch_status
     );

   if (gstk_status != GSTK_SUCCESS)
   {
       return gstk_status;
   }

   gstk_status = gstk_packer_ch_data_len_tlv(
     data_avail_evt_cmd->ch_data_len,
     &STK_envelope_cmd.ch_data_len);

   if (gstk_status != GSTK_SUCCESS)
   {
       return gstk_status;
   }

   /* DATA LENGTH :  evt id data + 2 (2 is for the tag and length field)
                   + device id data + 2 (2 is for the tag and length field)
                   + channel status + 2 (2 is for the tag and length field)
                   + channel data len + 2 (2 is for the tag and length field)
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.ch_status.ch_status_length +
                             STK_envelope_cmd.ch_data_len.ch_data_len_length +
                             8;
   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data      = data_avail_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                   &STK_envelope_cmd.event_download_hdr.evt_list,
                                   offset,
                                   (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                            GSTK_TAG_LENGTH_LEN),
                                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                                   sizeof(STK_envelope_cmd.event_download_hdr.evt_list)) <
           (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                    GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.event_download_hdr.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
               (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy channel status */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.ch_status),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.ch_status)) <
               (size_t)(STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       offset = (int)(offset + STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN);

       /* copy channel data length */
       if (gstk_status == GSTK_SUCCESS &&
           (offset + STK_envelope_cmd.ch_data_len.ch_data_len_length + GSTK_TAG_LENGTH_LEN - 1) <
           UIM_MAX_ENVELOPE_BYTES)
       {
         if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                    sizeof(uim_cmd_ptr->envelope.data)-offset,
                    &STK_envelope_cmd.ch_data_len,
                    (size_t)(STK_envelope_cmd.ch_data_len.ch_data_len_length + GSTK_TAG_LENGTH_LEN)) <
            (size_t)(STK_envelope_cmd.ch_data_len.ch_data_len_length + GSTK_TAG_LENGTH_LEN))
         {
           UIM_MSG_ERR_0("gstk_data_avail_evt_ind(): gstk_byte_offset_memcpy failed");
           gstk_status = GSTK_MEMORY_ERROR;
         }
         offset = (int)(offset + STK_envelope_cmd.ch_data_len.ch_data_len_length + GSTK_TAG_LENGTH_LEN);
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Data Avail Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);
         /* If already another Data Available event is put for retry, we don't need it any more, because this
            present command will inform the card about total data present with ESTK.*/
         if(GSTK_DATA_AVAIL_EVT_IND_RSP == gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type)
         {
           gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id)); 
         } 

         if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
         {
           gstk_status = gstk_process_and_store_env_cmd_for_retry(
                          uim_cmd_ptr->envelope.data,
                          (uint32)uim_cmd_ptr->envelope.num_bytes,
                          uim_cmd_ptr->hdr.user_data);
           if( gstk_status == GSTK_SUCCESS)
           {
             gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
           }
           else
           {
             UIM_MSG_HIGH_0("Data Avail Env - no retry will be attempted ");
             gstk_status = GSTK_SUCCESS;
           }
         }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
         else
         {
           UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
         }
         /* no free for data available ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else
   {
     gstk_status = GSTK_MEMORY_ERROR;
   }
   if(data_avail_evt_cmd->ch_status_info.is_scws_ch)
   {
     UIM_MSG_HIGH_1("Enabling receive data allowed for ch id 0x%x",
                    data_avail_evt_cmd->ch_status_info.ch_id);
     estk_scws_clear_receive_data_allowed(
       int32touint8(data_avail_evt_cmd->ch_status_info.ch_id),
       gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
   }
   return gstk_status;
}/* gstk_data_avail_evt_ind */

/*===========================================================================
FUNCTION gstk_ch_status_evt_ind

DESCRIPTION

  This function builds the channel status event download envelope command
  and sends it to UIM

PARAMETERS
  data_avail_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_ch_status_evt_ind(
  const gstk_envelope_ch_status_evt_ind_type *ch_status_evt_cmd
)
{
  gstk_status_enum_type                          gstk_status         = GSTK_SUCCESS;
  gstk_envelope_ch_status_evt_dl_command_type    STK_envelope_cmd;
  uim_cmd_type                                  *uim_cmd_ptr;
  int32                                          offset              = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_ch_status_evt_dl_command_type));

   if (ch_status_evt_cmd == NULL)
   {
      return GSTK_ERROR;
   }

   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* Pack various TLVs
   1. Event List TLV
   2. Device Id TLV
   3. Channel Status TLV
   */

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_CHANNEL_STATUS,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS)
   {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS)
   {
       return gstk_status;
   }

   /* Channel status TLV */
   gstk_status = gstk_packer_ch_status_tlv(
     &ch_status_evt_cmd->ch_status_info,
     &STK_envelope_cmd.ch_status
     );

   if (gstk_status != GSTK_SUCCESS)
   {
       return gstk_status;
   }

   /* DATA LENGTH :  evt id data + 2 (2 is for the tag and length field)
                   + device id data + 2 (2 is for the tag and length field)
                   + channel status + 2 (2 is for the tag and length field)
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.ch_status.ch_status_length +
                             6;
   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data      = ch_status_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                   &STK_envelope_cmd.event_download_hdr.evt_list,
                                   offset,
                                   (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                            GSTK_TAG_LENGTH_LEN),
                                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                                   sizeof(STK_envelope_cmd.event_download_hdr.evt_list)) <
           (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                    GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.event_download_hdr.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
               (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       /* copy channel status */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.ch_status),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.ch_status.ch_status_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.ch_status)) <
               (size_t)(STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Channel Status Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);
         /* If already another Channel Status event is put for retry, we don't need it any more, because this
            present command will inform the card about total data present with ESTK.*/
         if(GSTK_CH_STATUS_EVT_IND_RSP == gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type)
         {
           gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id)); 
         } 

         if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
         {
           gstk_status = gstk_process_and_store_env_cmd_for_retry(
                          uim_cmd_ptr->envelope.data,
                          (uint32)uim_cmd_ptr->envelope.num_bytes,
                          uim_cmd_ptr->hdr.user_data);
           if( gstk_status == GSTK_SUCCESS)
           {
             gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
           }
           else
           {
             UIM_MSG_HIGH_0("Channel status Env - no retry will be attempted ");
             gstk_status = GSTK_SUCCESS;
           }
         }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
         else
         {
          UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
         }
         /* no free for channel status ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else
   {
     gstk_status = GSTK_MEMORY_ERROR;
   }
   return gstk_status;
}/* gstk_ch_status_evt_ind */

/*===========================================================================
FUNCTION   gstk_nw_rej_evt_ind

DESCRIPTION
  This function builds the envelope of network rejection event

PARAMETERS
  nw_rej_ptr: [Input] The struct of network rejection event

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_nw_rej_evt_ind(
  const gstk_nw_rej_evt_ind_type *nw_rej_ptr
)
{
  gstk_envelope_event_dl_cmd_header_type env_hdr;
  uim_cmd_type                           *uim_cmd_ptr;
  int32                                  offset        = 0;
  gstk_status_enum_type                  gstk_status   = GSTK_SUCCESS;
  gstk_access_technology_type            rat           = GSTK_ACCESS_NONE;

  env_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
  gstk_status = gstk_packer_evt_list_tlv(GSTK_NW_REJECTION, &env_hdr.evt_list);

  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  if (!gstk_util_is_valid_rat(
         (sys_radio_access_tech_e_type)nw_rej_ptr->nw_rej_evt.rat, &rat))
  {
    UIM_MSG_ERR_1("invalid rat 0x%x!", nw_rej_ptr->nw_rej_evt.rat);
    return GSTK_ERROR;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(GSTK_NETWORK_DEVICE,
                                       GSTK_UICC_SIM_DEVICE,
                                       &env_hdr.device_id );

  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }
  env_hdr.length = env_hdr.device_id.device_tag_length +
                   env_hdr.evt_list.length + 4;

  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL)
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F,
                                                      &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
      /* set to 0 so that uim will get the data from index 0 */
      uim_cmd_ptr->envelope.offset = 0x00;

      /* set user_data */
      uim_cmd_ptr->hdr.user_data      = nw_rej_ptr->message_header.user_data;

      /* populate the envelope command info */
      uim_cmd_ptr->envelope.data[offset++] = env_hdr.tag;
      uim_cmd_ptr->envelope.data[offset++] = env_hdr.length;

      /* copy evt list */
      if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                     &env_hdr.evt_list,
                    (size_t)(env_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN),
                     UIM_MAX_ENVELOPE_BYTES - offset,
                     sizeof(gstk_evt_list_tlv_type)) <
         (size_t)(env_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        return GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + env_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN);

      /* copy device */
      if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                     &env_hdr.device_id,
                     (size_t)(env_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN),
                     UIM_MAX_ENVELOPE_BYTES - offset,
                     sizeof(gstk_device_identities_tag_tlv_type)) <
         (size_t)(env_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        return GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + env_hdr.device_id.device_tag_length
                     + GSTK_TAG_LENGTH_LEN);

      /* copy loca info, RAI or TAI */
      switch(nw_rej_ptr->nw_rej_evt.nw_rej_type)
      {
      case GSTK_LOCATION_UPDATE_REJ:
        uim_cmd_ptr->envelope.data[offset++] =
          (uint8)GSTK_LOCATION_INFORMATION_TAG;
        uim_cmd_ptr->envelope.data[offset++] = 5;
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         nw_rej_ptr->nw_rej_evt.mcc_mnc,
                         GSTK_MAX_PLMN_LEN,
                         UIM_MAX_ENVELOPE_BYTES - offset,
                         GSTK_MAX_PLMN_LEN) <
             GSTK_MAX_PLMN_LEN)
          {
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            return GSTK_MEMORY_ERROR;
          }

          offset += GSTK_MAX_PLMN_LEN;
        }
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         nw_rej_ptr->nw_rej_evt.lac,
                         GSTK_MAX_LAC_LEN,
                         UIM_MAX_ENVELOPE_BYTES - offset,
                         GSTK_MAX_LAC_LEN) <
             GSTK_MAX_LAC_LEN)
          {
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            return GSTK_MEMORY_ERROR;
          }
          offset += GSTK_MAX_LAC_LEN;
        }
        break;
      case GSTK_GPRS_ATTACH_REJ:
      case GSTK_ROUTING_AREA_UPDATE_REJ:
        uim_cmd_ptr->envelope.data[offset++] =
          (uint8)GSTK_ROUTING_AREA_TAG;
        uim_cmd_ptr->envelope.data[offset++] = 6;
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         nw_rej_ptr->nw_rej_evt.mcc_mnc,
                         GSTK_MAX_PLMN_LEN,
                         UIM_MAX_ENVELOPE_BYTES - offset,
                         GSTK_MAX_PLMN_LEN) <
             GSTK_MAX_PLMN_LEN)
          {
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            return GSTK_MEMORY_ERROR;
          }
          offset += GSTK_MAX_PLMN_LEN;
        }
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         nw_rej_ptr->nw_rej_evt.lac,
                         GSTK_MAX_LAC_LEN,
                         UIM_MAX_ENVELOPE_BYTES - offset,
                         GSTK_MAX_LAC_LEN) <
             GSTK_MAX_LAC_LEN)
          {
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            return GSTK_MEMORY_ERROR;
          }
          offset += GSTK_MAX_LAC_LEN;
        }
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          uim_cmd_ptr->envelope.data[offset++] = nw_rej_ptr->nw_rej_evt.rac;
        }
        break;
      case GSTK_EMM_ATTACH_REJ:
      case GSTK_TRACKING_AREA_UPDATE_REJ:
        uim_cmd_ptr->envelope.data[offset++] =
          (uint8)GSTK_TRACKING_AREA_TAG;
        uim_cmd_ptr->envelope.data[offset++] = 5;
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         nw_rej_ptr->nw_rej_evt.mcc_mnc,
                         GSTK_MAX_PLMN_LEN,
                         UIM_MAX_ENVELOPE_BYTES - offset,
                         GSTK_MAX_PLMN_LEN) <
             GSTK_MAX_PLMN_LEN)
          {
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            return GSTK_MEMORY_ERROR;
          }
          offset += GSTK_MAX_PLMN_LEN;
        }
        if (offset < UIM_MAX_ENVELOPE_BYTES)
        {
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         nw_rej_ptr->nw_rej_evt.lac,
                         GSTK_MAX_LAC_LEN,
                         UIM_MAX_ENVELOPE_BYTES - offset,
                         GSTK_MAX_LAC_LEN) <
             GSTK_MAX_LAC_LEN)
          {
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            return GSTK_MEMORY_ERROR;
          }
          offset += GSTK_MAX_LAC_LEN;
        }
        break;
      default:
        UIM_MSG_ERR_1("invalid nw_rej_type: 0x%x",
                      nw_rej_ptr->nw_rej_evt.nw_rej_type);
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        return GSTK_ERROR;
      }

      /* copy RAT */
      if ((offset + 2) < UIM_MAX_ENVELOPE_BYTES)
      {
        uim_cmd_ptr->envelope.data[offset++] = (uint8)GSTK_ACCESS_TECHNOLOGY_TAG;
        uim_cmd_ptr->envelope.data[offset++] = 1;
        uim_cmd_ptr->envelope.data[offset++] = rat;
      }

      /* copy update/attach type */
      if ((offset + 2) < UIM_MAX_ENVELOPE_BYTES)
      {
        uim_cmd_ptr->envelope.data[offset++] = (uint8)GSTK_UPDATE_ATTACH_TYPE_TAG;
        uim_cmd_ptr->envelope.data[offset++] = 1;
        uim_cmd_ptr->envelope.data[offset++] = nw_rej_ptr->nw_rej_evt.upd_attach_type;
      }

      /* copy rejection cause code */
      if ((offset + 2) < UIM_MAX_ENVELOPE_BYTES)
      {
        uim_cmd_ptr->envelope.data[offset++] = (uint8)GSTK_REJECTION_CAUSE_TAG;
        uim_cmd_ptr->envelope.data[offset++] = 1;
        uim_cmd_ptr->envelope.data[offset++] = nw_rej_ptr->nw_rej_evt.rej_cause;
      }

      /* set length of envelope and length of UIM data */
      uim_cmd_ptr->envelope.data[1] = int32touint8(offset - 2);
      uim_cmd_ptr->envelope.num_bytes = int32touint8(offset);

      gstk_util_dump_byte_array("NW REJ EVT",
                                &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                uim_cmd_ptr->envelope.num_bytes);

      /* Only if envelope state is GSTK_NONE, change state to RSP_FROM_CARD
       and store the env payload
      */
      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
      {
        gstk_status = gstk_process_and_store_env_cmd_for_retry(
                        uim_cmd_ptr->envelope.data,
                        (uint32)uim_cmd_ptr->envelope.num_bytes,
                        uim_cmd_ptr->hdr.user_data);
        if( gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state =
            GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
        }
        else
        {
          UIM_MSG_HIGH_0("FAIL - no retry will be attempted ");
          gstk_status = GSTK_SUCCESS;
        }
      }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
      else
      {
        UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
      }
      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
    else
    {
      UIM_MSG_ERR_0("failed to populate UIM cmd hdr!");
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      return GSTK_ERROR;
    }
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }

  return gstk_status;
} /*gstk_nw_rej_evt_ind*/


/*===========================================================================
FUNCTION gstk_access_tech_evt_ind

DESCRIPTION

  This function builds the access tech event download envelope command
  and sends it to UIM

PARAMETERS
  access_tech_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_access_tech_evt_ind(
  const gstk_envelope_access_tech_ind_type    *access_tech_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_access_tech_evt_dl_command_type           STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;

   if(access_tech_evt_cmd == NULL)
   {
     UIM_MSG_ERR_0("access_tech_evt_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }


   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_access_tech_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_ACCESS_TECH,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS)
   {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS)
   {
       return gstk_status;
   }

   /* access technology */
   gstk_status = gstk_packer_access_technology_tlv(
       &access_tech_evt_cmd->access_tech,
       &STK_envelope_cmd.access_tech);

   if (gstk_status != GSTK_SUCCESS)
   {
     return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + lang sel data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.access_tech.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr == NULL )
   {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
   if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
   {
     /* set to 0 so that uim will get the data from index 0 */
     uim_cmd_ptr->envelope.offset = 0x00;

     /* set user_data */
     uim_cmd_ptr->hdr.user_data               = access_tech_evt_cmd->message_header.user_data;

     /* populate the envelope command info */
     uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

     uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
     uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

     /* copy evt list */
     if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                sizeof(uim_cmd_ptr->envelope.data)-offset,
                &STK_envelope_cmd.event_download_hdr.evt_list,
                (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN)) <
        (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length))
     {
       gstk_status = GSTK_MEMORY_ERROR;
     }
     /* copy device */
     else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                     &(STK_envelope_cmd.event_download_hdr.device_id),
                                     offset+=
                                     (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                           GSTK_TAG_LENGTH_LEN),
                                     (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                              GSTK_TAG_LENGTH_LEN),
                                     sizeof(uim_cmd_ptr->envelope.data),
                                     sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
             (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                      GSTK_TAG_LENGTH_LEN))
     {
       gstk_status = GSTK_MEMORY_ERROR;
     }
     /* copy  access technology info */
     else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                     &(STK_envelope_cmd.access_tech),
                                     offset +=
                                     (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
                                           + GSTK_TAG_LENGTH_LEN),
                                     (size_t)(STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN),
                                     sizeof(uim_cmd_ptr->envelope.data),
                                     sizeof(STK_envelope_cmd.access_tech)) <
             (size_t)(STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN))
     {
       gstk_status = GSTK_MEMORY_ERROR;
     }

     offset = (int)(offset + STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN);

     if (gstk_status == GSTK_SUCCESS)
     {
       gstk_util_dump_byte_array("Access Technology evt",
                                 &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                 uim_cmd_ptr->envelope.num_bytes);
       if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
       {
         gstk_status = gstk_process_and_store_env_cmd_for_retry(
                         uim_cmd_ptr->envelope.data,
                         (uint32)uim_cmd_ptr->envelope.num_bytes,
                         uim_cmd_ptr->hdr.user_data);
       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state =
           GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
       }
       else
       {
         UIM_MSG_HIGH_1("gstk_access_tech_evt_ind(): Env retry is not attempted: %d",
                        gstk_status);
         gstk_status = GSTK_SUCCESS;
       }
     } /* if(gstk_curr_envelope_info_ptr->retry_info.retry_flag == TRUE) */
       else
       {
         UIM_MSG_HIGH_0("gstk_access_tech_evt_ind(): Env retry is not attempted. retry_flag is FALSE");
       }
       /* no free for envelope location info ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
     else
     {
       gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
       uim_cmd_ptr = NULL;
     }
   }

   return gstk_status;
} /* gstk_access_tech_evt_ind*/

/*===========================================================================
FUNCTION gstk_csg_evt_ind

DESCRIPTION

  This function builds the CSG Cell Selection event download envelope command
  and sends it to UIM

PARAMETERS
  csg_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_csg_evt_ind(
  const gstk_csg_change_ind_type* csg_evt_cmd
)
{
  gstk_status_enum_type                   gstk_status       = GSTK_SUCCESS;
  uim_cmd_type*                           uim_cmd_ptr       = NULL;
  int                                     offset            = 0;
  gstk_envelope_csg_evt_dl_command_type   STK_envelope_cmd;


  if (csg_evt_cmd == NULL)
  {
    UIM_MSG_ERR_0("csg_evt_cmd is NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_envelope_cmd */
  memset(&STK_envelope_cmd, 0x00, sizeof STK_envelope_cmd);

  /* Pack various TLVs */
  STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

  /* Pack event list */
  gstk_status = gstk_packer_evt_list_tlv(
    GSTK_CSG_CELL_SELECTION,
    &STK_envelope_cmd.event_download_hdr.evt_list);    
  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* Pack device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_envelope_cmd.event_download_hdr.device_id);
  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  if((csg_evt_cmd->cmd.selection_status.general_info.is_camped) &&
     (csg_evt_cmd->cmd.csg_id != SYS_CSG_ID_INVALID))
  {
    /* Pack access tech */
    gstk_status = gstk_packer_access_technology_tlv(
        (uint8*)&csg_evt_cmd->cmd.access_tech,
      &STK_envelope_cmd.access_tech);
    if (gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }
    /* Pack CSG ID */
    gstk_status = gstk_packer_csg_id_tlv(
      &csg_evt_cmd->cmd.csg_id,
      &STK_envelope_cmd.csg_id);
    if (gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }
    if(csg_evt_cmd->cmd.len > 0)
    {
      /* Pack HNB name */
      gstk_status = gstk_packer_hnb_name_tlv(
      csg_evt_cmd->cmd.hnb_name,
      csg_evt_cmd->cmd.len,
      &STK_envelope_cmd.hnb_name);
      if (gstk_status != GSTK_SUCCESS)
      {
         return gstk_status;
      }
    }    
  }
  
  /* Pack CSG Cell Selection status */
  gstk_status = gstk_packer_csg_cell_selection_status_tlv(
    &csg_evt_cmd->cmd.selection_status,
    &STK_envelope_cmd.selection_status);
  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* total data length:  evt list data + 2 (2 is for the tag and length field)
                       + device id data + 2
                       + access tech data + 2 (optional)
                       + CSG cell selection status data + 2
                       + CSG ID data + 2 (optional)
                       + HNB name data + 2 (optional)
  */

  STK_envelope_cmd.event_download_hdr.length = 
    STK_envelope_cmd.event_download_hdr.evt_list.length + 2
    + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + 2
    + STK_envelope_cmd.selection_status.selection_status_len + 2;

  /* add lengths of optional fields */
  if((csg_evt_cmd->cmd.selection_status.general_info.is_camped) &&
     (csg_evt_cmd->cmd.csg_id != SYS_CSG_ID_INVALID))
  {
    STK_envelope_cmd.event_download_hdr.length += 
      STK_envelope_cmd.access_tech.length + 2
      + STK_envelope_cmd.csg_id.csg_id_len + 2;
    if(STK_envelope_cmd.hnb_name.hnb_name_len > 0)
    {
      STK_envelope_cmd.event_download_hdr.length += STK_envelope_cmd.hnb_name.hnb_name_len + 2;
    }      
  }

  /* Send cmd to STK application on card */
  uim_cmd_ptr = gstk_get_uim_buf();
  if(uim_cmd_ptr != NULL )
  {
    (void)gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    /* set to 0 so that uim will get the data from index 0 */
    uim_cmd_ptr->envelope.offset = 0x00;
    /* set user_data */
    uim_cmd_ptr->hdr.user_data = csg_evt_cmd->message_header.user_data;
    /* populate the envelope command info */
    uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length +
                                      GSTK_TAG_LENGTH_LEN;
    uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
    uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;
    /* copy evt list */
    if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
               sizeof(uim_cmd_ptr->envelope.data)-offset,
               &STK_envelope_cmd.event_download_hdr.evt_list,
               (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                        GSTK_TAG_LENGTH_LEN)) <
       (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length))
    {
      UIM_MSG_ERR_0("gstk_csg_evt_ind(): gstk_byte_offset_memcpy failed");
      gstk_status = GSTK_MEMORY_ERROR;
    }
    /* copy device */
    else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &(STK_envelope_cmd.event_download_hdr.device_id),
                                    offset +=
                                    (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                          GSTK_TAG_LENGTH_LEN),
                                    (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
            (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                     GSTK_TAG_LENGTH_LEN))
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }

    offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
             + GSTK_TAG_LENGTH_LEN);

    if((gstk_status == GSTK_SUCCESS) &&
       (STK_envelope_cmd.access_tech.length > 0))
    {
      /* copy access tech */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.access_tech,
                                            offset,
                                            (size_t)STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN,
                                            sizeof(uim_cmd_ptr->envelope.data),
                                            sizeof(STK_envelope_cmd.access_tech)) <
         (size_t)(STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + STK_envelope_cmd.access_tech.length +
                     GSTK_TAG_LENGTH_LEN);
    }

    /* Copy CSG cell selection status */
    if(gstk_status == GSTK_SUCCESS &&
       STK_envelope_cmd.selection_status.selection_status_len > 0)
    {
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.selection_status,
                                 offset,
                                 (size_t)STK_envelope_cmd.selection_status.selection_status_len + GSTK_TAG_LENGTH_LEN,
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.selection_status)) <
         (size_t)(STK_envelope_cmd.selection_status.selection_status_len + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + STK_envelope_cmd.selection_status.selection_status_len
               + GSTK_TAG_LENGTH_LEN);
    }

    if((gstk_status == GSTK_SUCCESS) &&
       (STK_envelope_cmd.csg_id.csg_id_len > 0))
    {
      /* Copy CSG ID */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.csg_id,
                                 offset,
                                 (size_t)(STK_envelope_cmd.csg_id.csg_id_len + GSTK_TAG_LENGTH_LEN),
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.csg_id)) <
         (size_t)(STK_envelope_cmd.csg_id.csg_id_len + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + STK_envelope_cmd.csg_id.csg_id_len
               + GSTK_TAG_LENGTH_LEN);
      /* Copy HNB Name */
      if ((gstk_status == GSTK_SUCCESS) &&
          (STK_envelope_cmd.hnb_name.hnb_name_len > 0))
      {
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.hnb_name,
                                 offset,
                                 (size_t)(STK_envelope_cmd.hnb_name.hnb_name_len + GSTK_TAG_LENGTH_LEN),
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.hnb_name)) <
            (size_t)(STK_envelope_cmd.hnb_name.hnb_name_len + GSTK_TAG_LENGTH_LEN))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset = (int)(offset + STK_envelope_cmd.hnb_name.hnb_name_len
                       + GSTK_TAG_LENGTH_LEN);
      }
    }

    if (gstk_status == GSTK_SUCCESS)
    {
      gstk_util_dump_byte_array("CSG Cell Selection status evt",
                                &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                uim_cmd_ptr->envelope.num_bytes);
      if(GSTK_CSG_CELL_SELECTION_EVT_IND_RSP == gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type)
      {
        gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id)); 
      } 
 
      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
      {
        gstk_status = gstk_process_and_store_env_cmd_for_retry(
                       uim_cmd_ptr->envelope.data,
                       (uint32)uim_cmd_ptr->envelope.num_bytes,
                       uim_cmd_ptr->hdr.user_data);
        if( gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
        }
        else
        {
          UIM_MSG_HIGH_0("CSG selection env - no retry will be attempted ");
          gstk_status = GSTK_SUCCESS;
        }
      }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
      else
      {
        UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
      }                                
      gstk_send_cmd_to_uim_server(uim_cmd_ptr);
    }
    else
    {
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Out of memory");
    gstk_status = GSTK_MEMORY_ERROR;
  }
  return gstk_status;
} /* gstk_csg_evt_ind */

/*===========================================================================
FUNCTION   gstk_ims_registration_evt_ind

DESCRIPTION
  This function builds the envelope of IMS Registration event

PARAMETERS
  ims_registration_evt_cmd: [Input] The struct of IMS Registration

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_ims_registration_evt_ind(
  gstk_envelope_ims_registration_evt_ind_type* ims_reg_evt_cmd
)
{
  gstk_status_enum_type                     gstk_status       = GSTK_SUCCESS;
  uim_cmd_type*                             uim_cmd_ptr       = NULL;
  int                                       offset            = 0;
  boolean                                   success_status_code = FALSE;
  gstk_envelope_ims_reg_evt_dl_command_type STK_envelope_cmd;


  if (ims_reg_evt_cmd == NULL)
  {
    UIM_MSG_ERR_0("ims_reg_evt_cmd is NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_envelope_cmd */
  memset(&STK_envelope_cmd, 0x00, sizeof STK_envelope_cmd);

  /* Pack various TLVs */
  STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

  /* Pack event list */
  gstk_status = gstk_packer_evt_list_tlv(
    GSTK_IMS_REGISTRATION,
    &STK_envelope_cmd.event_download_hdr.evt_list);
  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* Pack device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_envelope_cmd.event_download_hdr.device_id);
  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* SUCCESS Status Code = '200' */
  if (ims_reg_evt_cmd->ims_reg.status_code_len == 3 &&
      ims_reg_evt_cmd->ims_reg.status_code &&
      ims_reg_evt_cmd->ims_reg.status_code[0] == '2' &&
      ims_reg_evt_cmd->ims_reg.status_code[1] == '0' &&
      ims_reg_evt_cmd->ims_reg.status_code[2] == '0')
  {
    UIM_MSG_HIGH_0("gstk_ims_registration_evt_ind(): Status Code is '200'!");
    success_status_code = TRUE;  
  }

  /* Only pack IMPU List if there is no IMS Status Code or Status Code = '200' */
  /* Note that IMPU List can be empty when IMS Registration is successful */
  if (success_status_code || ims_reg_evt_cmd->ims_reg.status_code_len == 0)
  {
    /* Format IMPU List buffer according to 131.111 spec */
    if (gstk_util_convert_impu_list(&ims_reg_evt_cmd->ims_reg.impu_list, &ims_reg_evt_cmd->ims_reg.impu_list_len) != TRUE)
    {
      return GSTK_ERROR;
    }
    /* Pack IMPU List */
    gstk_status = gstk_packer_impu_list_tlv(
      ims_reg_evt_cmd->ims_reg.impu_list,
      ims_reg_evt_cmd->ims_reg.impu_list_len,
      &STK_envelope_cmd.impu_list);
    if (gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }
  }
  else
  {
    /* Pack IMS Status Code */
    gstk_status = gstk_packer_ims_status_code_tlv(
        ims_reg_evt_cmd->ims_reg.status_code,
        ims_reg_evt_cmd->ims_reg.status_code_len,
        &STK_envelope_cmd.status_code);
    if (gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }
  }

  /* total data length:  evt list data + 2 (2 is for the tag and length field)
                       + device id data + 2
                       + IMPU List data + 2 (Conditional: only in SUCCESS cases)
                       + Status Code data + 2 (Conditional: only in FAILURE cases)
  */

  STK_envelope_cmd.event_download_hdr.length = 
    STK_envelope_cmd.event_download_hdr.evt_list.length + 2
    + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + 2;

  /* add lengths of optional fields */
  if (success_status_code || ims_reg_evt_cmd->ims_reg.status_code_len == 0)
  {
    STK_envelope_cmd.event_download_hdr.length += 
	  STK_envelope_cmd.impu_list.impu_list_len + 2;
  }
  else
  {
    STK_envelope_cmd.event_download_hdr.length += 
	  STK_envelope_cmd.status_code.status_code_len + 2;
  }

  if ((STK_envelope_cmd.impu_list.impu_list_len > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT &&
      STK_envelope_cmd.event_download_hdr.length + GSTK_TAG_LENGTH_LEN + 1 >= UIM_MAX_ENVELOPE_BYTES) ||
      (STK_envelope_cmd.impu_list.impu_list_len <= GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT &&
      STK_envelope_cmd.event_download_hdr.length + GSTK_TAG_LENGTH_LEN >= UIM_MAX_ENVELOPE_BYTES))
  {
    UIM_MSG_ERR_0("Envelope length exceeds UIM_MAX_ENVELOPE_BYTES");
    return GSTK_MEMORY_ERROR;
  }
  
  /* Send command to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if(uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status != GSTK_SUCCESS) || (uim_cmd_ptr == NULL)) 
    {
      UIM_MSG_ERR_0("Fail to populate Envelope header");
      return gstk_status;
    }
       
    /* set to 0 so that uim will get the data from index 0 */
    uim_cmd_ptr->envelope.offset = 0x00;
    /* set user_data */
    uim_cmd_ptr->hdr.user_data = ims_reg_evt_cmd->message_header.user_data;
    /* populate the envelope command info */
    uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length +
                                      GSTK_TAG_LENGTH_LEN;
    uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
    uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;
    /* copy evt list */
    if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
               sizeof(uim_cmd_ptr->envelope.data)-offset,
               &STK_envelope_cmd.event_download_hdr.evt_list,
               (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length
                        + GSTK_TAG_LENGTH_LEN)) <
       (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN))
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }
    /* copy device */
    else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &(STK_envelope_cmd.event_download_hdr.device_id),
                                    offset +=
                                    (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                          GSTK_TAG_LENGTH_LEN),
                                    (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
            (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                     GSTK_TAG_LENGTH_LEN))
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }

    offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
             + GSTK_TAG_LENGTH_LEN);

    if (success_status_code || ims_reg_evt_cmd->ims_reg.status_code_len == 0)
    {
      if (gstk_status == GSTK_SUCCESS)
      {
        /* Set the tag and the length */
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.impu_list.tag;

        /* If IMPU List is greater than 127 bytes, extend one more byte for length field */
        if (STK_envelope_cmd.impu_list.impu_list_len > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
        {
          uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;        
          uim_cmd_ptr->envelope.num_bytes++;
        }
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.impu_list.impu_list_len;
	  
        /* copy IMPU List data */
        if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                   STK_envelope_cmd.impu_list.impu_list,
                   (size_t)STK_envelope_cmd.impu_list.impu_list_len) <
           (size_t)STK_envelope_cmd.impu_list.impu_list_len)
        {
          UIM_MSG_ERR_0("gstk_ims_registration_evt_ind(): gstk_byte_offset_memcpy failed");
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset += (int)(STK_envelope_cmd.impu_list.impu_list_len);
      }
    }
    else
    {
      if (gstk_status == GSTK_SUCCESS)
      {
        /* Set the tag and the length, total 2 bytes */
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.status_code.tag;
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.status_code.status_code_len;
	  
        /* copy IMS Status Code data */
        if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                   STK_envelope_cmd.status_code.status_code,
                   (size_t)STK_envelope_cmd.status_code.status_code_len) <
           (size_t)STK_envelope_cmd.status_code.status_code_len)
        {
          UIM_MSG_ERR_0("gstk_ims_registration_evt_ind(): gstk_byte_offset_memcpy failed");
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset += (int)(STK_envelope_cmd.status_code.status_code_len);
      }
    }

    if (gstk_status == GSTK_SUCCESS)
    {
      gstk_util_dump_byte_array("IMS Registration evt",
                                &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                uim_cmd_ptr->envelope.num_bytes);
    }

    /* free envelope impu_list/status_code ind and tlv */
    if(STK_envelope_cmd.impu_list.impu_list != NULL) {
      gstk_free(STK_envelope_cmd.impu_list.impu_list);
    }
    if(STK_envelope_cmd.status_code.status_code != NULL) {
      gstk_free(STK_envelope_cmd.status_code.status_code);
    }
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)((void*)ims_reg_evt_cmd));

    if (gstk_status == GSTK_SUCCESS)
    {
      gstk_send_cmd_to_uim_server(uim_cmd_ptr);
    }
    else
    {
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
    }
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;
}
/*lint -e506 lonstant value boolean */

