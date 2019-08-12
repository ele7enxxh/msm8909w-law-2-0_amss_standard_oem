/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   E V T   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the event handling functions for MMGSDI

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_evt.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   ar      Send FDN event source along with FDN event
05/24/16   ar      Review of macros used by MMGSDI
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/11/16   sp      Reduce F3 logging
04/22/16   vdc     Send card error event when NO ATR error is received
04/11/16   av      Introduce LPA support
01/07/16   kv      UIM PSM handling updates
11/23/15   vdc     Send profile switching evt when profile has switched in card
10/21/15   vdc     Move event info type to heap while sending ECC event
09/25/15   vr      Added support for ERA-GLONASS
09/09/15   stv     Pack imsi_m activation status into subscription_ready_evt
08/10/15   stv     To send temporary unlock status indication
11/13/14   tl      Remove support for SAP pin event
11/12/14   vdc     Send card error evt if NO_ATR is received after slot power up
08/22/14   tl      Change client notification when app state is brought
                   down from SUB_READY TO PERSO_READY
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/25/14   av      Fix incorrect F3 message
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
06/30/14   vv      Remove Perso code
04/09/14   tl      Add function to send events to all sessions
03/27/14   yt      Ensure usage of correct slot id
03/25/14   tl      SIM Lock secondary revisions
03/18/14   tl      Introduce new SIM Lock feature
02/19/14   am      Added logic for clearing timers and queues
02/10/14   ar      Replace session checks with utility functions
01/27/14   tl      Unlink session close cause with AID availability
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/11/13   df      Remove unused memory paging feature
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
11/18/13   vv      Added support for parallel REFRESH
11/05/13   tkl     Add IMSI check for TDS test SIM
10/16/13   df      Remove unused function
10/04/13   rp      F3 message reduction
09/23/13   df      Support for SIM busy
08/30/13   tl      Convert channel info table from static to dynamic allocation
07/15/13   vs      EONS updates to support single baseband SGLTE
07/01/13   at      Support for additional info in EONS APIs
06/24/13   vdc     Added support for triple SIM
05/21/13   vv      Added support for session closed cause
05/21/13   vv      Fixed compiler warning
05/17/13   shr     Handle scenario where ongoing Session Provisioning
                   operation is interrupted by a CARD_ERROR on the slot
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/08/13   bcho    Free memory allocated for EONS event info
05/06/13   spo     Memory optimization for get response Cache
04/30/13   av      Added ICCID in the MMGSDI_CARD_INSERTED_EVT
04/12/13   spo     Do not send events to clients before Evt Reg CNF is sent
04/01/13   spo     Do not send session events before Session Open CNF is sent
03/21/13   bcho    Send EONS event to session who registered late
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
12/05/12   vv      Added support for parallel processing in MMGSDI
10/21/12   av      Added support for new ECC event
10/15/12   yt      Queue error response for pending commands at card error
10/13/12   abg     Clear common file cache when CARD_ERROR occurs
10/12/12   abg     Do not send provisioning events to non-prov sessions
09/03/12   bcho    Support for MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT
06/06/12   bcho    Send perso evt with status according to app state
05/23/12   vv      Added support for file update event notification
04/26/12   at      Send out CARD_ERROR event to clients on power down even if
                   slot is already in NO_CARD state
04/16/12   tl      Added traces displaying client name
03/29/12   shr     Added support for session open with MF,
                   support for non-prov. app selection using zero length and
                   partial AID
03/28/12   vv      Set valid num/unblock retries when building PIN events
03/21/12   bcho    MMGSDI_APP_STATE_READY_NO_SUB_EVT replaced with
                   MMGSDI_APP_STATE_EXT_PERSO_FAILED
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Added support for Get Operator Name API
10/13/11   nb      Hot Swap Support
09/23/11   nb      Async TP enhancement
08/10/11   vv      Added support to build PIN events for all sessions
05/17/11   shr     Updates for Thread safety
05/04/11   ms      Send the PERSO_EVT to the late clients in perso state
                   as WAIT_FOR_DEPERSO_S and INIT_ERROR_S
04/26/11   vs      Support for subscription ok request
02/22/11   ea      Fixed compiler warnings
01/12/11   yt      Fixed compiler warnings
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10  shr      Legacy ILLEGAL_CARD event needs to be sent only when
                   GW_PRI provisioning application is marked illegal
11/05/10   yt      Klocwork fixes
10/29/10   kk      Fixed NULL ptr check in FDN event
10/19/10   kk      Added support for sending FDN event when EF FDN is updated
10/13/10   nb      Fix for steering of roaming
09/24/10   shr     Update Session ID in the Illegal Subscription event info
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
08/24/10   shr     Do not send out multiple CARD_ERROR events for a slot
08/18/10   nmb     Removed compilation warnings
07/14/10   nmb     Include perso indicator retries in GSDI_PERSO events
07/14/10   js      Fixed compilation warnings
07/02/10   ssr     Fixed refresh for ICC card which have both GSM and RUIM
07/02/10   ssr     Fixed App ready state in card init complted evt
06/28/10   shr     Added support for Hidden key events
06/24/10   yt      Klocwork fixes
06/16/10   js      Revert changes for sending card init complete for
                   legacy clients who use client_id_and_evt_reg
06/04/10   js      MMGSDI should send card init complete event for
                   late event registration.
04/27/10   shr     Added Hiddenkey support
04/27/10   js      Allow only default app init refresh events to non-session
                   clients.
04/07/10   nb      SAP Enhancement
03/19/10   vs      Fixed event notification from state
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
12/18/09   nb      Moved Dual Slot Changes
12/15/09   ssr     Sending PIN event to all applications who have same pin
                   reference
12/08/09   shr     Read EF-DIR and cache app data before sending
                   TP request to GSTK
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
11/03/09   nb      Klockwork Fixes
10/27/09   sun     Removing FEATURE_MMGSDI_DUAL_SLOT featurization
10/22/09   mib     Print of event details in logs
10/19/09   ssr     Fix for Pin operation and Pin event
10/16/09   vs      Dual mode enablement
08/25/09   shr     Fixed compilation warnings
08/24/09   kp      ZI memory reduction changes
08/11/09   kk      Fixed UNIVERSAL PIN event if client registers late
07/13/09   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_gen.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/


/*===========================================================================
         F U N C T I O N S   T O   B E    O B S O L E L E
============================================================================*/

/* ==========================================================================
   FUNCTION:      MMGSDI_EVT_BUILD_SESSION_CLOSE

   DESCRIPTION:
     This function populate session close event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_evt_build_session_close(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_client_id_type      session_id,
  mmgsdi_int_app_info_type  *app_info_ptr,
  mmgsdi_event_data_type    *evt_info_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);

  if(!mmgsdi_util_is_slot_valid(slot))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  evt_info_ptr->evt                           = MMGSDI_SESSION_CLOSE_EVT;
  evt_info_ptr->client_id                     = session_id;
  evt_info_ptr->session_id                    = session_id;
  evt_info_ptr->data.session_close.slot       = slot;
  evt_info_ptr->data.session_close.session_id = session_id;

  memset(&evt_info_ptr->data.session_close.app_id,
         0x00,
         sizeof(mmgsdi_data_type));

  if(app_info_ptr == NULL)
  {
    evt_info_ptr->data.session_close.cause = MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;
    return MMGSDI_SUCCESS;
  }

  evt_info_ptr->data.session_close.cause = app_info_ptr->session_closed_cause;

  if ((app_info_ptr->app_data.aid.data_len > 0) &&
      (app_info_ptr->app_data.aid.data_len <= MMGSDI_MAX_AID_LEN) &&
      (app_info_ptr->app_data.aid.data_ptr != NULL))
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      evt_info_ptr->data.session_close.app_id.data_ptr,
      (app_info_ptr->app_data.aid.data_len));
    if (evt_info_ptr->data.session_close.app_id.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    evt_info_ptr->data.session_close.app_id.data_len =
      int32touint32(app_info_ptr->app_data.aid.data_len);

    mmgsdi_memscpy(evt_info_ptr->data.session_close.app_id.data_ptr,
      int32touint32(app_info_ptr->app_data.aid.data_len),
      app_info_ptr->app_data.aid.data_ptr,
      int32touint32(app_info_ptr->app_data.aid.data_len));
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_evt_build_session_close */


/*===========================================================================
         E V E N T    N O T I F I C A T I O N   F U N C T I O N S
============================================================================*/

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_VALIDATE_SESSION_NOTIFY_TYPE

  DESCRIPTION:
    This function validates the passed in input parameters for session
    notification purposes

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_evt_validate_session_notify_type(
  boolean                         card_evt,
  mmgsdi_evt_session_notify_type  notify_type,
  mmgsdi_slot_id_enum_type       *slot_id_ptr)
{
  mmgsdi_slot_id_enum_type slot  = MMGSDI_MAX_SLOT_ID_ENUM;

  switch(notify_type.notify_type)
  {
    case MMGSDI_EVT_NOTIFY_ALL_SESSIONS:
    case MMGSDI_EVT_NOTIFY_CARD_SESSION:
      if(card_evt)
      {
        UIM_MSG_ERR_0("Card based events cannot be Session specific");
        return MMGSDI_ERROR;
      }
      break;
    case MMGSDI_EVT_NOTIFY_SINGLE_SESSION:
      if(card_evt)
      {
        UIM_MSG_ERR_0("Card based events cannot be Session specific");
        return MMGSDI_ERROR;
      }
      if(mmgsdi_util_get_session_app_info(notify_type.session_id,
                                          &slot,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL) != MMGSDI_SUCCESS)
      {
        /* Invalid session id */
        return MMGSDI_ERROR;
      }
      if(slot_id_ptr != NULL)
      {
        /* populate slot id */
        *slot_id_ptr = slot;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_EVT_NOTIFY_NONE:
    case MMGSDI_MAX_EVT_NOTIFY_ENUM:
      UIM_MSG_ERR_1("Invalid notify_type 0x%x", notify_type.notify_type);
      return MMGSDI_ERROR;
  }

  /* Get here because it is either ALL client or All Session notification or
     single client notification.  Check the slot */
  switch(notify_type.slot_id)
  {
    case MMGSDI_SLOT_1:
    case MMGSDI_SLOT_2:
    case MMGSDI_SLOT_3:
      break;
    default:
      UIM_MSG_ERR_1("invalid slot for  notify_type 0x%x",
                    notify_type.notify_type);
      return MMGSDI_ERROR;
  }
  if(slot_id_ptr)
  {
    *slot_id_ptr = notify_type.slot_id;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_evt_validate_notify_type */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_FREE_DATA

  DESCRIPTION:
     This function free any memory allocation for the mmgsdi_evet_data_type
     data structure for different command

  DEPENDENCIES:
    None

  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_free_data(
  mmgsdi_event_data_type *event_info_ptr)
{
  if(event_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null event_info_ptr");
    return;
  }

  switch(event_info_ptr->evt)
  {
    case MMGSDI_SESSION_CLOSE_EVT:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(event_info_ptr->data.session_close.app_id.data_ptr);
      return;
    case MMGSDI_FDN_EVT:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(event_info_ptr->data.fdn.rec_data.data_ptr);
      return;
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
    case MMGSDI_CARD_INSERTED_EVT:
    case MMGSDI_SIM_BUSY_EVT:
    case MMGSDI_TEST_SIM_EVT:
    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
    case MMGSDI_REFRESH_EVT:
    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_SAP_DISCONNECT_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
    case MMGSDI_SUBSCRIPTION_READY_EVT:
    case MMGSDI_SESSION_CHANGED_EVT:
    case MMGSDI_PERSO_EVT:
    case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
    case MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT:
    case MMGSDI_HIDDENKEY_EVT:
    case MMGSDI_OTA_COMMIT_RSP_EVT:
    case MMGSDI_PERSO_TEMPORARY_UNLOCK_EVT:
    case MMGSDI_PROFILE_DATA_EVT:
    case MMGSDI_PROFILE_SWITCHING_EVT:
    case MMGSDI_MAX_EVT_ENUM:
      return;
    case MMGSDI_EONS_EVT:
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.spn.eons_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr);
      return;
    }
    case MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT:
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons_opl_pnn_cache_ready.opl_info.valid_record_list.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        event_info_ptr->data.eons_opl_pnn_cache_ready.pnn_info.valid_record_list.data_ptr);
      return;
    }
    case MMGSDI_FILE_UPDATE_EVT:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(event_info_ptr->data.file_update.file_data.data_ptr);
      return;
    case MMGSDI_ECC_EVT:
      if(event_info_ptr->data.ecc.ecc_data)
      {
        uint8 i = 0;

        for(i = 0; i < event_info_ptr->data.ecc.ecc_count; i++)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(event_info_ptr->data.ecc.ecc_data[i].data.data_ptr);
        }
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(event_info_ptr->data.ecc.ecc_data);
      }
      return;
  }
}/* mmgsdi_evt_free_data */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_CLIENT

  DESCRIPTION:
    This function sends notification event to registered clients

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_client(
  mmgsdi_event_data_type *event_info_ptr)
{
  int32                         i              = 0;
  int32                         j              = 0;
  mmgsdi_task_enum_type         mmgsdi_task    = MMGSDI_TASK_MAX;
  mmgsdi_task_state_enum_type   task_state     = MMGSDI_TASK_INACTIVE;

  if(event_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null event_info_ptr");
    return;
  }

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  /*---------------------------------------------------------------------------
    If gsdi task is not active, no event is needed to be sent to clients
  ---------------------------------------------------------------------------*/
  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    return;
  }

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    if (mmgsdi_client_id_reg_table[i]->evt_cb_ptr == NULL)
    {
      /* NULL callback */
      continue;
    }

    if (mmgsdi_client_id_reg_table[i]-> is_client_evt_reg_cnf_sent == FALSE)
    {
      continue;
    }

    /* In case of session_close event, use the session id */
    if (MMGSDI_SESSION_CLOSE_EVT == event_info_ptr->evt)
    {
      for (j = 0;
           (j < mmgsdi_client_id_reg_table[i]->num_session &&
            j < MMGSDI_MAX_SESSION_INFO);
           j++)
      {
        mmgsdi_session_info_type* session_ptr =
          mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

        if ((session_ptr != NULL) && (session_ptr->session_id ==
            event_info_ptr->data.session_close.session_id))
        {
          /* Use the session id */
          event_info_ptr->client_id  = mmgsdi_client_id_reg_table[i]->client_id;
          event_info_ptr->session_id = session_ptr->session_id;
          /* Execute callback */
          (mmgsdi_client_id_reg_table[i]->evt_cb_ptr)(event_info_ptr);
        }
      }
      continue;
    }

    /* Use the client id */
    event_info_ptr->client_id = mmgsdi_client_id_reg_table[i]->client_id;

    /* Execute callback */
    (mmgsdi_client_id_reg_table[i]->evt_cb_ptr)(event_info_ptr);
  }
} /* mmgsdi_evt_notify_client */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_SPECIFIC_CLIENT_ID

  DESCRIPTION:
    This function sends notification event to the passed in client id

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_specific_client_id(
  mmgsdi_client_id_type   client_id,
  mmgsdi_event_data_type *event_info_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  int32                   index         = MMGSDI_MAX_CLIENT_INFO;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id, &index);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (index >= MMGSDI_MAX_CLIENT_INFO) ||
     (index < 0) || mmgsdi_client_id_reg_table[index] == NULL)
  {
    return;
  }
  /* mmgsdi_util_get_client_id_index success guarantees a valid entry in client_id_reg_table */

  if (mmgsdi_client_id_reg_table[index]->is_client_evt_reg_cnf_sent == TRUE)
  {
    if(mmgsdi_client_id_reg_table[index]->evt_cb_ptr)
    {
      /* Traces */
      UIM_MSG_HIGH_4("Sending event 0x%x to client - id_low = 0x%x, id_high = 0x%x, Callback = 0x%x",
                     event_info_ptr->evt,
                     (client_id&0xFFFFFFFF),
                     (client_id >> 32 & 0xFFFFFFFF),
                     mmgsdi_client_id_reg_table[index]->evt_cb_ptr);
      mmgsdi_util_print_client_name(client_id);
     (mmgsdi_client_id_reg_table[index]->evt_cb_ptr)(event_info_ptr);
    }
  }
} /* mmgsdi_evt_notify_specific_client_id */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_SESSION

  DESCRIPTION:
    This function sends notification event to registered clients to their specific
    sessions

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_session(
  mmgsdi_event_data_type         *event_info_ptr,
  mmgsdi_slot_id_enum_type        slot_id,
  const mmgsdi_int_app_info_type *app_info_ptr,
  boolean                         is_prov_evt)
{
  int32                         i              = 0;
  int32                         j              = 0;
  mmgsdi_task_enum_type         mmgsdi_task    = MMGSDI_TASK_MAX;
  mmgsdi_task_state_enum_type   task_state     = MMGSDI_TASK_INACTIVE;

  if((event_info_ptr == NULL) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("event_info_ptr 0x%x or app_info_ptr 0x%x is null",
                  event_info_ptr, app_info_ptr);
    return;
  }

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  /*---------------------------------------------------------------------------
    If gsdi task is not active, no event is needed to be sent to clients
  ---------------------------------------------------------------------------*/
  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    return;
  }

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    for (j = 0; j < MMGSDI_MAX_SESSION_INFO; j++)
    {
      mmgsdi_session_info_type* session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_ptr != NULL && session_ptr->is_session_open_cnf_sent == FALSE)
      {
          /* Session open cnf is not sent to client.
             Hence do not send session events  */
          continue;
      }

      if ((session_ptr != NULL) &&
          (session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
          (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL) &&
          (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        if (session_ptr->session_evt_cb_ptr == NULL)
        {
          /* NULL callback */
          continue;
        }
        if (slot_id != mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->slot_id)
        {
          /* not for the same slot, so even if the app_data is the
             same, we are not talking about the same session */
          continue;
        }
        if (mmgsdi_util_is_card_session(session_ptr->session_type))
        {
          /* No session events are sent to card slot session holders.
             They will receive card events as client id holders */
          continue;
        }
        /* If the event is related to provisioning session and session type
           is non-provisioning , then no need to notify */
        if(mmgsdi_util_is_non_prov_session(session_ptr->session_type) &&
           (is_prov_evt))
        {
          continue;
        }
        if (mmgsdi_util_cmp_aid(
              &mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data,
              &app_info_ptr->app_data, FALSE) == MMGSDI_SUCCESS)
        {
          event_info_ptr->client_id  = mmgsdi_client_id_reg_table[i]->client_id;
          event_info_ptr->session_id = session_ptr->session_id;

          /* If the event is for an illegal subscription, indicate the session ID
             in the event info */
          if(event_info_ptr->evt == MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT)
          {
            event_info_ptr->data.illegal_session.session_id =
              session_ptr->session_id;
          }

          /* Execute callback */
          (session_ptr->session_evt_cb_ptr)(event_info_ptr);
        }
      }
    }
  }
}/* mmgsdi_evt_notify_session */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_CARD_SESSION

  DESCRIPTION:
    This function sends notification event to registered clients to their
    specific card slot sessions

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_card_session(
  mmgsdi_event_data_type         *event_info_ptr,
  mmgsdi_session_type_enum_type   session_type)
{
  int32                         i              = 0;
  int32                         j              = 0;
  mmgsdi_task_enum_type         mmgsdi_task    = MMGSDI_TASK_MAX;
  mmgsdi_task_state_enum_type   task_state     = MMGSDI_TASK_INACTIVE;

  if(event_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("event_info_ptr is null");
    return;
  }

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  /*---------------------------------------------------------------------------
    If gsdi task is not active, no event is needed to be sent to clients
  ---------------------------------------------------------------------------*/
  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    return;
  }

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }
    for (j = 0; j < MMGSDI_MAX_SESSION_INFO; j++)
    {
      mmgsdi_session_info_type* session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if (session_ptr != NULL)
      {
        if (session_ptr->session_evt_cb_ptr == NULL)
        {
          /* NULL callback */
          continue;
        }
        if (session_ptr->session_type == session_type)
        {
          event_info_ptr->client_id  = mmgsdi_client_id_reg_table[i]->client_id;
          event_info_ptr->session_id = session_ptr->session_id;

          if(session_ptr->is_session_open_cnf_sent == FALSE )
          {
            /* If the session open cnf is not sent do not execute callback */
            continue;
          }

          /* Execute callback */
          (session_ptr->session_evt_cb_ptr)(event_info_ptr);
        }
      }
    }
  }
}/* mmgsdi_evt_notify_card_session */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_SPECIFIC_SESSION_ID

  DESCRIPTION:
    This function sends notification event to the specific session id.  Used
    when the client registers late and mmgsdi tries to notify "missing event"
    to this client

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_specific_session_id(
  mmgsdi_session_id_type    session_id,
  mmgsdi_event_data_type   *event_info_ptr)
{
  mmgsdi_return_enum_type      mmgsdi_status   = MMGSDI_SUCCESS;
  int32                        client_index    = 0;
  int32                        session_index   = 0;
  mmgsdi_session_info_type *   session_ptr     = NULL;
  mmgsdi_task_enum_type        mmgsdi_task     = MMGSDI_TASK_MAX;
  mmgsdi_task_state_enum_type  task_state      = MMGSDI_TASK_INACTIVE;

  if(event_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null event_info_ptr");
    return;
  }

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  /*---------------------------------------------------------------------------
    If gsdi task is not active, no event is needed to be sent to clients
  ---------------------------------------------------------------------------*/
  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_client_and_session_index(session_id,
                                                           &client_index,
                                                           &session_index);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      client_index >= MMGSDI_MAX_CLIENT_INFO ||
      session_index >= MMGSDI_MAX_SESSION_INFO||
      mmgsdi_client_id_reg_table[client_index] == NULL)
  {
    return;
  }

  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if ((session_ptr != NULL) &&
      (mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]->session_evt_cb_ptr != NULL))
  {
    /* Send notification events only after session open confirmation is sent */
    if(session_ptr->is_session_open_cnf_sent == TRUE )
    {
      event_info_ptr->client_id  = mmgsdi_client_id_reg_table[client_index]->client_id;
      event_info_ptr->session_id = session_id;

      /* If the event is for an illegal subscription, indicate the session ID
         in the event info */
      if(event_info_ptr->evt == MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT)
      {
        event_info_ptr->data.illegal_session.session_id = session_id;
      }

      /* Traces */
      UIM_MSG_HIGH_4("Sending event 0x%x to client - id_low = 0x%x, session - id_low = 0x%x, cb = 0x%x",
                     event_info_ptr->evt,
                     (event_info_ptr->client_id&0xFFFFFFFF),
                     (event_info_ptr->session_id&0xFFFFFFFF),
                     mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]->session_evt_cb_ptr);
      mmgsdi_util_print_client_name(mmgsdi_client_id_reg_table[client_index]->client_id);

      /* Execute callback */
      (mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]->session_evt_cb_ptr)(event_info_ptr);
    }
  }
}/* mmgsdi_evt_notify_specific_session_id */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_BUILD_PIN

  DESCRIPTION:
    This function populates pin event for notification

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_evt_build_pin(
  mmgsdi_session_id_type      session_id,
  mmgsdi_pin_enum_type        pin,
  mmgsdi_pin_status_enum_type status,
  mmgsdi_event_data_type     *evt_info_ptr)
{
  mmgsdi_int_app_info_type *sel_app_ptr   = NULL;
  uint32                    num_aids      = 0;
  uint32                    i             = 0;
  mmgsdi_app_pin_info_type  *pin_info_ptr = NULL;
  mmgsdi_slot_id_enum_type  slot          = MMGSDI_MAX_SLOT_ID_ENUM;

  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);

  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  /*PIN events are build and hence broadcasted for all the sessions,
    including nonprov sessions*/

  /*Retrieve the app info for a session*/
  (void)mmgsdi_util_get_session_app_info(session_id,
                                         &slot,
                                         NULL,
                                         &sel_app_ptr,
                                         NULL,
                                         NULL,
                                         NULL);

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  /*If the selected app is USIM or any other, then just copy over
    num_aids will be 1 in this case*/

  /*Copy over the selected app's aid as well*/
  num_aids++;
  mmgsdi_memscpy(&evt_info_ptr->data.pin.aid_type[i++],
                 sizeof(evt_info_ptr->data.pin.aid_type[i]),
                 &sel_app_ptr->app_data,
                 sizeof(mmgsdi_aid_type));

  evt_info_ptr->data.pin.num_aids = num_aids;
  evt_info_ptr->data.pin.dir_index = sel_app_ptr->dir_index;

  switch (pin)
  {
    case MMGSDI_PIN1:
      pin_info_ptr = sel_app_ptr->pin1_ptr;
      evt_info_ptr->evt = MMGSDI_PIN1_EVT;
      if(pin_info_ptr == NULL)
      {
        UIM_MSG_ERR_0("Null pin1_ptr");
        return MMGSDI_ERROR;
      }
      break;
    case MMGSDI_PIN2:
      pin_info_ptr = &sel_app_ptr->pin2;
      evt_info_ptr->evt = MMGSDI_PIN2_EVT;
      break;
    case MMGSDI_UNIVERSAL_PIN:
      pin_info_ptr = sel_app_ptr->universal_pin_ptr;
      evt_info_ptr->evt = MMGSDI_UNIVERSAL_PIN_EVT;
      if(pin_info_ptr == NULL)
      {
        UIM_MSG_ERR_0("Null universal_pin_ptr");
        return MMGSDI_ERROR;
      }
      break;
    case MMGSDI_HIDDENKEY:
      pin_info_ptr = &sel_app_ptr->hiddenkey;
      evt_info_ptr->evt = MMGSDI_HIDDENKEY_EVT;
      break;
    default: /* Rest of the PIN to be handle in later stage */
      UIM_MSG_ERR_1("Invalid pin id 0x%x", pin);
      return MMGSDI_ERROR;
  }
  evt_info_ptr->session_id    = session_id;
  evt_info_ptr->data.pin.slot = slot;
  evt_info_ptr->data.pin.pin_info.pin_id = pin;
  evt_info_ptr->data.pin.pin_info.status = status;
  evt_info_ptr->data.pin.pin_info.num_retries = pin_info_ptr->num_retries;
  evt_info_ptr->data.pin.pin_info.num_unblock_retries = pin_info_ptr->num_unblock_retries;
  evt_info_ptr->data.pin.pin_info.valid_num_retries = pin_info_ptr->valid_num_retries;
  evt_info_ptr->data.pin.pin_info.valid_num_unblock_retries = pin_info_ptr->valid_num_unblock_retries;
  evt_info_ptr->data.pin.pin_info.pin_replacement = pin_info_ptr->pin_replacement;

  UIM_MSG_HIGH_3("PIN 0x%x with replacement value 0x%x has status 0x%x",
                 evt_info_ptr->data.pin.pin_info.pin_id,
                 evt_info_ptr->data.pin.pin_info.pin_replacement,
                 evt_info_ptr->data.pin.pin_info.status
                 );

  return MMGSDI_SUCCESS;

}/* mmgsdi_evt_build_pin */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_BUILD_AND_NOTIFY_CARD_EVT_FROM_STATE

  DESCRIPTION:
    This function builds the event to client who registered late based on
    the current state of the card.
    Multiple events may be sent out based on what is the currently state
    of MMGSDI

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_build_and_notify_card_evt_from_state(
  mmgsdi_slot_id_enum_type            slot,
  int32                               client_id_table_index)
{
  mmgsdi_slot_data_type        *slot_data_ptr = NULL;

  if ((client_id_table_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (client_id_table_index < 0))
  {
    UIM_MSG_ERR_1("Exceed MMGSDI_MAX_CLIENT_INFO 0x%x", client_id_table_index);
    return;
  }

  if (mmgsdi_client_id_reg_table[client_id_table_index] == NULL)
  {
    UIM_MSG_HIGH_1("Invalid client_id_table_index: %d", client_id_table_index);
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  switch (slot_data_ptr->mmgsdi_state)
  {
    case MMGSDI_STATE_CARD_INSERTED:
      mmgsdi_evt_build_and_send_card_inserted(
        FALSE,
        mmgsdi_client_id_reg_table[client_id_table_index]->client_id,
        slot);

       /* additional event sent if card is currently in a sim busy state */
       if (slot_data_ptr->sim_busy)
       {
          mmgsdi_evt_build_and_send_sim_busy(
            FALSE,
            mmgsdi_client_id_reg_table[client_id_table_index]->client_id,
            slot);
       }

      /* Send Test SIM event to client */
      mmgsdi_evt_build_and_send_test_sim_status(
        FALSE,
        mmgsdi_client_id_reg_table[client_id_table_index]->client_id,
        slot);
      break;

    case MMGSDI_STATE_NO_CARD:
      mmgsdi_evt_build_and_send_card_error(
        FALSE,
        mmgsdi_client_id_reg_table[client_id_table_index]->client_id,
        slot,
        MMGSDI_CARD_ERR_UNKNOWN_ERROR);
      mmgsdi_util_close_all_sessions_cleanup_and_notify(slot);
      mmgsdi_util_reset_app_data(FALSE,slot);
      break;

    case MMGSDI_STATE_NOT_INIT:
    case MMGSDI_STATE_ENUM_MAX:
      UIM_MSG_ERR_1("State = 0x%x, nothing to build and notify",
                    slot_data_ptr->mmgsdi_state);
      break;
  }
} /* mmgsdi_evt_build_and_notify_card_evt_from_state */


/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_BUILD_AND_NOTIFY_SESSION_EVT_FROM_STATE

  DESCRIPTION:
    This function builds the event to session who registered late based on
    the current state of the session.
    Multiple events may be sent out based on what is the currently state
    of MMGSDI

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_build_and_notify_session_evt_from_state(
  mmgsdi_session_id_type   session_id)
{
  mmgsdi_slot_id_enum_type       slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type      *app_info_ptr     = NULL;
  mmgsdi_session_info_type      *session_info_ptr = NULL;
  mmgsdi_return_enum_type        mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_evt_session_notify_type notify_type;
  mmgsdi_perso_status_enum_type  perso_status     = MMGSDI_PERSO_STATUS_NONE;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(
    session_id, &slot, NULL, &app_info_ptr, NULL, &session_info_ptr, NULL);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL) ||
      (session_info_ptr == NULL))
  {
    /* Unable to build any event */
    return;
  }

  if(session_info_ptr->session_evt_cb_ptr == NULL)
  {
    /* no need to notify because the event callback is null */
    return;
  }

  /* Build session events from state only for prov sessions */
  if (mmgsdi_util_is_prov_session(session_info_ptr->session_type) == FALSE)
  {
    return;
  }

  notify_type.notify_type = MMGSDI_EVT_NOTIFY_SINGLE_SESSION;
  notify_type.session_id  = session_id;

  switch(app_info_ptr->app_state)
  {
    case MMGSDI_APP_STATE_UNKNOWN:
    case MMGSDI_APP_STATE_ENUM_MAX:
      /* invalid app states for any events */
    case MMGSDI_APP_STATE_DEACTIVATED:
      /* no need to send session_changed deactivate because this
         is the first time this session id registered */
    case MMGSDI_APP_STATE_DETECTED:
      /* No event notification required to this app state because
         it should be part of the card event */
      return;

    case MMGSDI_APP_STATE_ILLEGAL:
      mmgsdi_evt_build_and_send_illegal_session(
        notify_type,
        (const mmgsdi_int_app_info_type*)app_info_ptr);
      return;

    case MMGSDI_APP_STATE_ACTIVATED:
      /* Do not trigger session changed if the nv refresh is in progress */
      if(!app_info_ptr->nv_refresh_in_progress)
      {
        /* Send session changed - activated */
        mmgsdi_evt_build_and_send_session_changed(
          notify_type, TRUE, (const mmgsdi_int_app_info_type*)app_info_ptr);
        /* Send ECC Event */
        mmgsdi_evt_build_and_send_ecc(notify_type);
      }
      return;

    case MMGSDI_APP_STATE_PIN_EVT_SENT:
    case MMGSDI_APP_STATE_READY_FOR_PERSO:
      /* non prov session type should not have an app in this state except if
         it is shared with a prov session type */
      /* Send session changed - activated */
      mmgsdi_evt_build_and_send_session_changed(
        notify_type, TRUE, (const mmgsdi_int_app_info_type*)app_info_ptr);
      /* Send ECC Event */
      mmgsdi_evt_build_and_send_ecc(notify_type);
      /* Send all the PINs event */
      if(app_info_ptr->pin1_ptr)
      {
        mmgsdi_evt_build_and_send_pin(
          notify_type,
          MMGSDI_PIN1,
          app_info_ptr->pin1_ptr->status,
          (const mmgsdi_int_app_info_type*)app_info_ptr);
      }
      mmgsdi_evt_build_and_send_pin(
        notify_type,
        MMGSDI_PIN2,
        app_info_ptr->pin2.status,
        (const mmgsdi_int_app_info_type*)app_info_ptr);
      if(app_info_ptr->universal_pin_ptr)
      {
        mmgsdi_evt_build_and_send_pin(
          notify_type,
          MMGSDI_UNIVERSAL_PIN,
          app_info_ptr->universal_pin_ptr->status,
          (const mmgsdi_int_app_info_type*)app_info_ptr);
      }

      if(app_info_ptr->perso_state == MMGSDI_SIMLOCK_UNLOCK_REQUIRED)
      {
        mmgsdi_evt_build_and_send_perso(
          notify_type,
          (const mmgsdi_int_app_info_type*)app_info_ptr,
          MMGSDI_PERSO_STATUS_LOCKED,
          app_info_ptr->perso_info.feature,
          app_info_ptr->perso_info.num_retries,
          app_info_ptr->perso_info.num_unblock_retries);
      }
      return;

    case MMGSDI_APP_STATE_PERSO_VERIFIED:
    case MMGSDI_APP_STATE_WAIT_FOR_SUB_OK:
    case MMGSDI_APP_STATE_EXT_PERSO_FAILED:
      /* non prov session type should not have an app in this state except if
         it is shared with a prov session type */
      /* Send session changed - activated */
      mmgsdi_evt_build_and_send_session_changed(
        notify_type, TRUE, (const mmgsdi_int_app_info_type*)app_info_ptr);
      /* Send ECC Event */
      mmgsdi_evt_build_and_send_ecc(notify_type);
      /* Send all the PINs event */
      if(app_info_ptr->pin1_ptr)
      {
        mmgsdi_evt_build_and_send_pin(
          notify_type,
          MMGSDI_PIN1,
          app_info_ptr->pin1_ptr->status,
          (const mmgsdi_int_app_info_type*)app_info_ptr);
      }
      mmgsdi_evt_build_and_send_pin(
        notify_type,
        MMGSDI_PIN2,
        app_info_ptr->pin2.status,
        (const mmgsdi_int_app_info_type*)app_info_ptr);
      if(app_info_ptr->universal_pin_ptr)
      {
        mmgsdi_evt_build_and_send_pin(
          notify_type,
          MMGSDI_UNIVERSAL_PIN,
          app_info_ptr->universal_pin_ptr->status,
          (const mmgsdi_int_app_info_type*)app_info_ptr);
      }
      /* Build perso */
      if(app_info_ptr->app_state == MMGSDI_APP_STATE_PERSO_VERIFIED)
      {
        perso_status = MMGSDI_PERSO_STATUS_DONE;
      }
      else if(app_info_ptr->app_state == MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)
      {
        perso_status = MMGSDI_PERSO_STATUS_WAIT_FOR_EXT_PERSO;
      }
      else
      {
        perso_status = MMGSDI_PERSO_STATUS_GENERIC_FAILURE;
      }

      mmgsdi_evt_build_and_send_perso(
        notify_type,
        (const mmgsdi_int_app_info_type*)app_info_ptr,
        perso_status,
        MMGSDI_MAX_PERSO_FEATURE_ENUM,
        0,
        0);
      return;

    case MMGSDI_APP_STATE_READY:
      /* Send ECC Event */
      mmgsdi_evt_build_and_send_ecc(notify_type);
      /* Send sub ready event */
      mmgsdi_evt_build_and_send_subscription_ready(
      notify_type, (const mmgsdi_int_app_info_type*)app_info_ptr);

      /* send EONS event */
      mmgsdi_eons_send_last_event(session_id);
      return;
  }
} /* mmgsdi_evt_build_and_notify_session_evt_from_state */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_CARD_INSERTED

  DESCRIPTION
    This function builds and sends the card inserted event to all relevant clients
    Input parameter client_id is mandatory when notify_all is FALSE

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_card_inserted(
  boolean                  notify_all,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot)
{
  mmgsdi_event_data_type       *evt_info_ptr    = NULL;
  mmgsdi_return_enum_type       mmgsdi_status   = MMGSDI_SUCCESS;
  uint32                        i               = 0;
  uint32                        evt_index       = 0;
  mmgsdi_slot_data_type        *slot_data_ptr   = NULL;
  mmgsdi_session_id_type        card_session_id = 0;
  mmgsdi_cache_init_enum_type   cache_state     = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_data_type              iccid;
  mmgsdi_access_type            access;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                     = MMGSDI_CARD_INSERTED_EVT;
  evt_info_ptr->data.card_inserted.slot = slot;

  /* Indicate if the card is an euicc or not */
  evt_info_ptr->data.card_inserted.is_euicc = slot_data_ptr->is_euicc;

  /*Send the event to clients with all the application info*/
  if(slot_data_ptr->app_info_pptr != NULL)
  {
    for(i = 0; i < MMGSDI_MAX_APP_INFO; i++)
    {
      if(slot_data_ptr->app_info_pptr[i] == NULL)
      {
        continue;
      }
      if(slot_data_ptr->app_info_pptr[i]->app_data.app_type != MMGSDI_APP_NONE)
      {
        mmgsdi_memscpy(&evt_info_ptr->data.card_inserted.aid_info[evt_index],
          sizeof(evt_info_ptr->data.card_inserted.aid_info[evt_index]),
          &slot_data_ptr->app_info_pptr[i]->app_data,
          sizeof(mmgsdi_aid_type));
        evt_info_ptr->data.card_inserted.num_aids_avail = ++evt_index;
      }
    }
  }

  /* Get the Card Slot Session ID corresponding to the slot */
  if(slot == MMGSDI_SLOT_1)
  {
    card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX];
  }
  else if(slot == MMGSDI_SLOT_2)
  {
    card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_2_INDEX];
  }
  else if(slot == MMGSDI_SLOT_3)
  {
    card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_3_INDEX];
  }

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&iccid, 0x00, sizeof(mmgsdi_data_type));

  evt_info_ptr->data.card_inserted.iccid_len = 0;

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_ICCID;

  mmgsdi_status = mmgsdi_cache_read_len(card_session_id,
                                        &access,
                                        &iccid.data_len,
                                        0,
                                        &cache_state);

  if (mmgsdi_status == MMGSDI_SUCCESS
      && cache_state != MMGSDI_CACHE_NOT_FOUND
      && iccid.data_len <= MMGSDI_ICCID_LEN)
  {
    evt_info_ptr->data.card_inserted.iccid_len = (uint8)iccid.data_len;
    iccid.data_ptr = &evt_info_ptr->data.card_inserted.iccid[0];

    mmgsdi_status = mmgsdi_cache_read(card_session_id,
                                      &access,
                                      &iccid,
                                      0,
                                      &cache_state);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      evt_info_ptr->data.card_inserted.iccid_len = 0;
      UIM_MSG_ERR_2("failed to read cache for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                    mmgsdi_status, cache_state);
    }
  }
  else
  {
    evt_info_ptr->data.card_inserted.iccid_len = 0;
    UIM_MSG_ERR_2("failed to read cache len for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                  mmgsdi_status, cache_state);
  }

  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    evt_info_ptr->client_id               = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_card_inserted */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_CARD_ERROR

  DESCRIPTION
    This function builds and sends the card error event to all relevant clients.
    Input parameter client_id is mandatory when notify_all is FALSE

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_card_error(
  boolean                        notify_all,
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       slot,
  mmgsdi_card_err_info_enum_type prev_err_cause)
{
  mmgsdi_event_data_type  *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;
  uint8                    slot_index    = MMGSDI_SLOT_1_INDEX;

  if((slot != MMGSDI_SLOT_1) &&
     (slot != MMGSDI_SLOT_2) &&
     (slot != MMGSDI_SLOT_3))
  {
    UIM_MSG_ERR_1("Invalid mmgsdi slot 0x%x", slot);
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

 /* --------------------------------------------------------------------------
    If slot is already in NO_CARD state (which indicates that a CARD_ERROR
    event has already been sent out for the slot), then do not send
    out another CARD_ERROR event.
    Exceptions:
    (1)If the Error is due to a Card removal or Power down, then
       CARD_ERROR event shall be sent irrespective of current slot status

    (2)If CARD_ERROR notification is for individual client (typically for
       late client registrations), send out CARD_ERROR event irrespective

    (3)a.Card is powered down. Send CARD_ERROR event with cause as power down.
       b.Remove the card from the slot.
       c.Client of MMGSDI sends power up command.
       d.As card is not present, MMGSDI receives error notif with no atr cause.
       e.Send the CARD_ERROR event to MMGSDI clients.
    ------------------------------------------------------------------------*/
  if(notify_all)
  {
    /* Queue error cnf for any pending commands that did not receive
       a response from UIM */
    mmgsdi_queue_error_cnf_for_pending_commands(slot);

    /* If a REFRESH is ongoing when the CARD_ERROR happens, REFRESH confirmation
       needs to be sent out and REFRESH clean-up needs to be done */
    if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
       (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id == slot))
    {
      UIM_MSG_ERR_0("CARD_ERROR occurred, hence stopping the current REFRESH");

      mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

      (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
              &mmgsdi_refresh_info_ptr[slot_index]->request_header);
    }

    /* Clear common file cache. The operation is performed before sending the event to
       make sure that cache is cleared also in the case where event is dropped because
       it was already sent out */
    (void)mmgsdi_cache_delete_common_file_cache(slot);

    /* Clear all entries in the Get Response Cache Table */
    (void)mmgsdi_gen_get_rsp_table_flush_all(slot);

    if(((prev_err_cause != MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY) ||
        (slot_data_ptr->card_error_condition != MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT)) &&
       (slot_data_ptr->card_error_condition != MMGSDI_CARD_ERR_CARD_REMOVED) &&
       (slot_data_ptr->card_error_condition != MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY) &&
       (slot_data_ptr->card_error_condition != MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT) &&
       (slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NO_CARD))
    {
      UIM_MSG_HIGH_1("MMGSDI_CARD_ERROR_EVT already sent out for slot: 0x%x",
                     slot);
      return;
    }
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                  = MMGSDI_CARD_ERROR_EVT;
  evt_info_ptr->data.card_error.slot = slot;

  /* Populate the Card Error Info */
  evt_info_ptr->data.card_error.info = slot_data_ptr->card_error_condition;

  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    evt_info_ptr->client_id            = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_card_error */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_SIM_BUSY

  DESCRIPTION
    This function builds and sends the sim busy event to all relevant clients
    Input parameter client_id is mandatory when notify_all is FALSE

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_sim_busy(
  boolean                  notify_all,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot)
{
  mmgsdi_event_data_type  *evt_info_ptr  = NULL;
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;

  if((slot != MMGSDI_SLOT_1) &&
     (slot != MMGSDI_SLOT_2) &&
     (slot != MMGSDI_SLOT_3))
  {
    UIM_MSG_ERR_1("Invalid mmgsdi slot 0x%x", slot);
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  /* allocate memory for the evt */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  /* populate the evt type and slot */
  evt_info_ptr->evt                = MMGSDI_SIM_BUSY_EVT;
  evt_info_ptr->data.sim_busy.slot = slot;

  /* Populate the sim busy flag */
  evt_info_ptr->data.sim_busy.sim_busy = slot_data_ptr->sim_busy;

  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    /* send to one specified client */
    evt_info_ptr->client_id            = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_sim_busy */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_SUBSCRIPTION_READY

  DESCRIPTION
    This function builds the subscription ready event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_subscription_ready(
  mmgsdi_evt_session_notify_type    notify_type,
  const mmgsdi_int_app_info_type   *app_info_ptr)
{
  mmgsdi_event_data_type    *evt_info_ptr     = NULL;
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type   slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_info_type  *session_info_ptr = NULL;

  if(app_info_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_info_from_app(&app_info_ptr->app_data,
                                                        slot_id,
                                                        &session_info_ptr);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    /* We did not find a session corresponding to the app... this is a
       serious failure that we should not ignore */
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt = MMGSDI_SUBSCRIPTION_READY_EVT;
  mmgsdi_memscpy(&evt_info_ptr->data.subscription_ready.app_info.app_data,
                 sizeof(evt_info_ptr->data.subscription_ready.app_info.app_data),
                 &app_info_ptr->app_data,
                 sizeof(mmgsdi_aid_type));
  evt_info_ptr->data.subscription_ready.app_info.cached = TRUE;

  if(app_info_ptr->pin1_ptr != NULL)
  {
    mmgsdi_util_set_pin_info(
        &evt_info_ptr->data.subscription_ready.app_info.pin1,
        app_info_ptr->pin1_ptr);
  }
  mmgsdi_util_set_pin_info(
      &evt_info_ptr->data.subscription_ready.app_info.pin2,
      &app_info_ptr->pin2);
  if(app_info_ptr->universal_pin_ptr != NULL)
  {
    mmgsdi_util_set_pin_info(
        &evt_info_ptr->data.subscription_ready.app_info.universal_pin,
        app_info_ptr->universal_pin_ptr);
  }
  mmgsdi_util_set_pin_info(
      &evt_info_ptr->data.subscription_ready.app_info.hiddenkey,
      &app_info_ptr->hiddenkey);

  evt_info_ptr->data.subscription_ready.app_info.slot     = slot_id;
  evt_info_ptr->data.subscription_ready.app_info.prov_app = TRUE;
  evt_info_ptr->data.subscription_ready.app_info.dir_index =
    app_info_ptr->dir_index;

  /* No need to do any error checking as that is done inside the
     utility function below already */
  evt_info_ptr->data.subscription_ready.activation_status =
    mmgsdi_util_check_activation_status(session_info_ptr->session_id);

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_subscription_ready */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_SESSION_CHANGED

  DESCRIPTION
    This function builds the session changed event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_session_changed(
  mmgsdi_evt_session_notify_type  notify_type,
  boolean                         activate,
  const mmgsdi_int_app_info_type *app_info_ptr)
{
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type  slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  if(app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null app_info_ptr");
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt = MMGSDI_SESSION_CHANGED_EVT;
  evt_info_ptr->data.session_changed.activated = activate;
  if(!activate)
  {
    evt_info_ptr->data.session_changed.cause = app_info_ptr->session_closed_cause;
  }
  else
  {
    evt_info_ptr->data.session_changed.cause = MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;
  }

  mmgsdi_memscpy(&evt_info_ptr->data.session_changed.app_info.app_data,
                 sizeof(evt_info_ptr->data.session_changed.app_info.app_data),
                 &app_info_ptr->app_data,
                 sizeof(mmgsdi_aid_type));

  if(app_info_ptr->pin1_ptr != NULL)
  {
    mmgsdi_util_set_pin_info(
        &evt_info_ptr->data.session_changed.app_info.pin1,
        app_info_ptr->pin1_ptr);
  }
  mmgsdi_util_set_pin_info(
      &evt_info_ptr->data.session_changed.app_info.pin2,
      &app_info_ptr->pin2);
  if(app_info_ptr->universal_pin_ptr != NULL)
  {
    mmgsdi_util_set_pin_info(
        &evt_info_ptr->data.session_changed.app_info.universal_pin,
        app_info_ptr->universal_pin_ptr);
  }
  mmgsdi_util_set_pin_info(
      &evt_info_ptr->data.session_changed.app_info.hiddenkey,
      &app_info_ptr->hiddenkey);

  evt_info_ptr->data.session_changed.app_info.prov_app =
    app_info_ptr->app_selected;
  evt_info_ptr->data.session_changed.app_info.cached =
    app_info_ptr->app_selected;
  evt_info_ptr->data.session_changed.app_info.slot = slot_id;
  evt_info_ptr->data.session_changed.app_info.dir_index =
    app_info_ptr->dir_index;

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_session_changed */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PIN

  DESCRIPTION
    This function builds the PIN1/PIN2/UPIN event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_pin(
  mmgsdi_evt_session_notify_type  notify_type,
  mmgsdi_pin_enum_type            pin_id,
  mmgsdi_pin_status_enum_type     pin_status,
  const mmgsdi_int_app_info_type *app_info_ptr)
{
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type  slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  boolean                   is_prov_event = FALSE;

  if(app_info_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  /* As Hiddenkey operations are prov events , setting is_prov_event to TRUE */
  if(pin_id == MMGSDI_HIDDENKEY)
  {
    is_prov_event = TRUE;
  }

  if(mmgsdi_evt_build_pin(notify_type.session_id,
                          pin_id,
                          pin_status,
                          evt_info_ptr) == MMGSDI_SUCCESS)
  {
    if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
    {
      /* broadcast to all */
      mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, is_prov_event);
    }
    else
    {
      mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
    }
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_pin */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PERSO

  DESCRIPTION
    This function builds and sends the Perso event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_perso(
  mmgsdi_evt_session_notify_type  notify_type,
  const mmgsdi_int_app_info_type *app_info_ptr,
  mmgsdi_perso_status_enum_type   status,
  mmgsdi_perso_feature_enum_type  feature,
  uint32                          num_retries,
  uint32                          num_unblock_retries)
{
  mmgsdi_event_data_type  *evt_info_ptr = NULL;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  if(app_info_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                            = MMGSDI_PERSO_EVT;
  evt_info_ptr->data.perso.feature             = feature;
  evt_info_ptr->data.perso.status              = status;
  evt_info_ptr->data.perso.num_retries         = num_retries;
  evt_info_ptr->data.perso.num_unblock_retries = num_unblock_retries;

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_perso */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_FDN

  DESCRIPTION
    This function builds the FDN event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    MMGSDI_SUCCESS: If the event can be built
    MMGSDI_ERROR:   If the event cannot be built

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_fdn(
  mmgsdi_evt_session_notify_type    notify_type,
  boolean                           is_enabled,
  const mmgsdi_int_app_info_type   *app_info_ptr,
  mmgsdi_rec_num_type               rec_num,
  const mmgsdi_data_type           *rec_data_ptr,
  mmgsdi_fdn_evt_source_enum_type   fdn_source)
{
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  if(app_info_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                 = MMGSDI_FDN_EVT;
  evt_info_ptr->data.fdn.enabled    = is_enabled;
  evt_info_ptr->data.fdn.slot       = slot_id;
  evt_info_ptr->data.fdn.rec_num    = rec_num;
  evt_info_ptr->data.fdn.evt_source = fdn_source;

  if(rec_data_ptr != NULL &&
     rec_data_ptr->data_len > 0 &&
     rec_data_ptr->data_ptr != NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      evt_info_ptr->data.fdn.rec_data.data_ptr,
      rec_data_ptr->data_len);
    if(evt_info_ptr->data.fdn.rec_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(evt_info_ptr->data.fdn.rec_data.data_ptr,
                     int32touint32(rec_data_ptr->data_len),
                     rec_data_ptr->data_ptr,
                     int32touint32(rec_data_ptr->data_len));
      evt_info_ptr->data.fdn.rec_data.data_len = rec_data_ptr->data_len;
    }
  }

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_CARD_SESSION)
  {
    if(notify_type.slot_id == MMGSDI_SLOT_1)
    {
      mmgsdi_evt_notify_card_session(evt_info_ptr, MMGSDI_CARD_SESSION_SLOT_1);
    }
    else if(notify_type.slot_id == MMGSDI_SLOT_2)
    {
      mmgsdi_evt_notify_card_session(evt_info_ptr, MMGSDI_CARD_SESSION_SLOT_2);
    }
    else if(notify_type.slot_id == MMGSDI_SLOT_3)
    {
      mmgsdi_evt_notify_card_session(evt_info_ptr, MMGSDI_CARD_SESSION_SLOT_3);
    }
  }
  else
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_fdn */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_ILLEGAL_SESSION

  DESCRIPTION
    This function builds the illegal session event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_illegal_session(
  mmgsdi_evt_session_notify_type  notify_type,
  const mmgsdi_int_app_info_type *app_info_ptr)
{
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type  slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  if(app_info_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt = MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT;

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_illegal_session */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_LEGAL_SESSION

  DESCRIPTION
    This function builds the legal session event which is sent out when an
    application earlier marked as illegal is updated back as legal application.
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_legal_session(
  mmgsdi_evt_session_notify_type  notify_type,
  const mmgsdi_int_app_info_type *app_info_ptr)
{
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type  slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  if(app_info_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt = MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT;

  mmgsdi_memscpy(&evt_info_ptr->data.legal_session.app_info.app_data,
                 sizeof(evt_info_ptr->data.legal_session.app_info.app_data),
                 &app_info_ptr->app_data,
                 sizeof(mmgsdi_aid_type));
  evt_info_ptr->data.legal_session.app_info.cached = TRUE;

  if(app_info_ptr->pin1_ptr != NULL)
  {
    mmgsdi_util_set_pin_info(&evt_info_ptr->data.legal_session.app_info.pin1,
                             app_info_ptr->pin1_ptr);
  }
  mmgsdi_util_set_pin_info(&evt_info_ptr->data.legal_session.app_info.pin2,
                           &app_info_ptr->pin2);
  if(app_info_ptr->universal_pin_ptr != NULL)
  {
    mmgsdi_util_set_pin_info(&evt_info_ptr->data.legal_session.app_info.universal_pin,
                             app_info_ptr->universal_pin_ptr);
  }
  mmgsdi_util_set_pin_info(&evt_info_ptr->data.legal_session.app_info.hiddenkey,
                           &app_info_ptr->hiddenkey);

  evt_info_ptr->data.legal_session.app_info.slot      = slot_id;
  evt_info_ptr->data.legal_session.app_info.prov_app  = TRUE;
  evt_info_ptr->data.legal_session.app_info.dir_index = app_info_ptr->dir_index;

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_legal_session */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_FILE_UPDATE

  DESCRIPTION
    This function builds the file update event
    The event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_file_update(
  mmgsdi_evt_session_notify_type       notify_type,
  mmgsdi_access_type                   access_type,
  const mmgsdi_write_extra_info_type * write_extra_ptr
)
{
  mmgsdi_event_data_type  *evt_info_ptr   = NULL;
  mmgsdi_return_enum_type  mmgsdi_status  = MMGSDI_SUCCESS;

  if (write_extra_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_evt_validate_session_notify_type(FALSE,
                                                          notify_type,
                                                          NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt = MMGSDI_FILE_UPDATE_EVT;
  mmgsdi_memscpy(&evt_info_ptr->data.file_update.file,
                 sizeof(evt_info_ptr->data.file_update.file),
                 &access_type,
                 sizeof(mmgsdi_access_type));
  evt_info_ptr->data.file_update.file_type = write_extra_ptr->file_type;
  switch(evt_info_ptr->data.file_update.file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      evt_info_ptr->data.file_update.file_index.rec_num = int32touint8(write_extra_ptr->rec_num);
      break;
    case MMGSDI_TRANSPARENT_FILE:
      evt_info_ptr->data.file_update.file_index.offset = int32touint16(write_extra_ptr->offset);
      break;
    default:
      UIM_MSG_ERR_1("Unexpected file type, 0x%x",
                    evt_info_ptr->data.file_update.file_type);
      MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
      return;
  }

  if(write_extra_ptr->data.data_len > 0 &&
     write_extra_ptr->data.data_ptr != NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      evt_info_ptr->data.file_update.file_data.data_ptr,
      write_extra_ptr->data.data_len);
    if(evt_info_ptr->data.file_update.file_data.data_ptr == NULL)
    {
      UIM_MSG_ERR_0("Mem alloc failed for file update data, sending evt without data");
    }
    else
    {
      mmgsdi_memscpy(evt_info_ptr->data.file_update.file_data.data_ptr,
                     int32touint32(write_extra_ptr->data.data_len),
                     write_extra_ptr->data.data_ptr,
                     int32touint32(write_extra_ptr->data.data_len));
      evt_info_ptr->data.file_update.file_data.data_len = write_extra_ptr->data.data_len;
    }
  }

  mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_file_update */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_ECC

  DESCRIPTION
    This function builds and sends out the ECC event.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_ecc(
  mmgsdi_evt_session_notify_type  notify_type)
{
  mmgsdi_event_data_type        *evt_info_ptr         = NULL;
  mmgsdi_session_info_type      *session_info_ptr     = NULL;
  mmgsdi_int_app_info_type      *app_info_ptr         = NULL;
  mmgsdi_return_enum_type        mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type       slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                         *invalid_data_ptr     = NULL;
  uint32                         num_valid_ecc_fields = 0;
  mmgsdi_rec_num_type            num_of_recs          = 0;
  mmgsdi_len_type                rec_len              = 0;
  mmgsdi_data_type               ecc_cache_data;
  uint8                          i                    = 0;

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    notify_type.session_id,
                    &slot_id,
                    NULL,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (session_info_ptr == NULL) ||
     (app_info_ptr == NULL) ||
     !mmgsdi_util_is_prov_session(session_info_ptr->session_type))
  {
    UIM_MSG_ERR_1("Invalid session id 0x%x", notify_type.session_id);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  memset(&ecc_cache_data, 0x00, sizeof(mmgsdi_data_type));

  switch (app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_CSIM:
      evt_info_ptr->data.ecc.file_enum = MMGSDI_CSIM_ECC;
      break;
    case MMGSDI_APP_RUIM:
      evt_info_ptr->data.ecc.file_enum = MMGSDI_CDMA_ECC;
      break;
    case MMGSDI_APP_SIM:
      evt_info_ptr->data.ecc.file_enum = MMGSDI_GSM_ECC;
      break;
    case MMGSDI_APP_USIM:
      evt_info_ptr->data.ecc.file_enum = MMGSDI_USIM_ECC;
      break;
     default:
       UIM_MSG_ERR_1("mmgsdi_evt_build_and_send_data - invalid app 0x%x",
                     app_info_ptr->app_data.app_type);
       MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
       return;
  }

  evt_info_ptr->evt                = MMGSDI_ECC_EVT;
  evt_info_ptr->data.ecc.slot      = slot_id;
  evt_info_ptr->data.ecc.ecc_count = 0;

 /* Get full ECC data from cache */
  mmgsdi_status = mmgsdi_cache_get_ecc_file_data(notify_type.session_id,
                                                 evt_info_ptr->data.ecc.file_enum,
                                                 &ecc_cache_data,
                                                 &rec_len);

  /* If ECC data is not cached yet, send ECC event without ecc data */
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     ecc_cache_data.data_ptr &&
     ecc_cache_data.data_len != 0)
  {
    switch(evt_info_ptr->data.ecc.file_enum)
    {
      case MMGSDI_USIM_ECC:
      {
        if(rec_len == 0)
        {
          break;
        }
        num_of_recs = ecc_cache_data.data_len/rec_len;
        /* Create data buffer with invalid data (all 0xFFs) and use this
           buffer to compare against every ECC record to figure out if it valid
           or not */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(invalid_data_ptr, rec_len);

        if(invalid_data_ptr != NULL)
        {
          memset(invalid_data_ptr, 0xFF, rec_len);
          /* Get the number of valid records so that we can allocate
             the memory for ECC event data accordingly */
          for (i = 0; i < num_of_recs; i++)
          {
            if(memcmp(&ecc_cache_data.data_ptr[i*rec_len], invalid_data_ptr, rec_len) != 0)
            {
              num_valid_ecc_fields ++;
            }
          }

          if(num_valid_ecc_fields == 0)
          {
            break;
          }

          /* Now create the ECC data payload */
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
            evt_info_ptr->data.ecc.ecc_data,
            (num_valid_ecc_fields * sizeof(mmgsdi_ecc_data_type)));

          if(evt_info_ptr->data.ecc.ecc_data == NULL)
          {
            evt_info_ptr->data.ecc.ecc_count = 0;
          }
          else
          {
            for (i = 0; i < num_of_recs && evt_info_ptr->data.ecc.ecc_count < num_valid_ecc_fields; i++)
            {
              /* If a record is valid, try to squeeze it into the event payload */
              if(memcmp(&ecc_cache_data.data_ptr[i*rec_len], invalid_data_ptr, rec_len) != 0)
              {
                /* Allocate memory for every (valid) ECC field */
                MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
                  evt_info_ptr->data.ecc.ecc_data[evt_info_ptr->data.ecc.ecc_count].data.data_ptr,
                  rec_len);

                if(evt_info_ptr->data.ecc.ecc_data[evt_info_ptr->data.ecc.ecc_count].data.data_ptr == NULL)
                {
                  evt_info_ptr->data.ecc.ecc_count = 0;
                  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(invalid_data_ptr);
                  break;
                }
                else
                {
                  /* Copy only the valid ECC records to the event data payload */
                  mmgsdi_memscpy(evt_info_ptr->data.ecc.ecc_data[evt_info_ptr->data.ecc.ecc_count].data.data_ptr,
                                 rec_len,
                                 &ecc_cache_data.data_ptr[i*rec_len],
                                 rec_len);
                  evt_info_ptr->data.ecc.ecc_data[evt_info_ptr->data.ecc.ecc_count].data.data_len = rec_len;
                  evt_info_ptr->data.ecc.ecc_data[evt_info_ptr->data.ecc.ecc_count].rec_num = i+1;
                }
                evt_info_ptr->data.ecc.ecc_count++;
              }
            } /* for (i = 0; i < num_of_recs; i++) */
          } /* if ECC event data ptr malloc succeeds */
        } /* if invalid data buffer malloc succeeds */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(invalid_data_ptr);
        break;
      }

      case MMGSDI_CDMA_ECC:
      case MMGSDI_CSIM_ECC:
      case MMGSDI_GSM_ECC:
        {
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
            evt_info_ptr->data.ecc.ecc_data,
            sizeof(mmgsdi_ecc_data_type));
          if(evt_info_ptr->data.ecc.ecc_data == NULL)
          {
            break;
          }

          /* Always set record number to 0 in case of GSM/CSIM/RUIM */
          evt_info_ptr->data.ecc.ecc_data->rec_num = 0;

          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
            evt_info_ptr->data.ecc.ecc_data->data.data_ptr,
            ecc_cache_data.data_len);
          if(evt_info_ptr->data.ecc.ecc_data->data.data_ptr != NULL)
          {
            /* Copy the raw ECC data to the event data payload */
            mmgsdi_memscpy(evt_info_ptr->data.ecc.ecc_data->data.data_ptr,
                           int32touint32(ecc_cache_data.data_len),
                           ecc_cache_data.data_ptr,
                           int32touint32(ecc_cache_data.data_len));

            evt_info_ptr->data.ecc.ecc_data->data.data_len = ecc_cache_data.data_len;
            /* Always set the ecc count to 1 in case of GSM/CSIM/RUIM */
            evt_info_ptr->data.ecc.ecc_count = 1;
          }
          break;
        } /* end case */

      default:
        UIM_MSG_HIGH_1("Invalid file enum 0x%x", evt_info_ptr->data.ecc.file_enum);
        break;
    } /* end switch */
  } /* end if */

  if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_ALL_SESSIONS)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, app_info_ptr, TRUE);
  }
  else if(notify_type.notify_type == MMGSDI_EVT_NOTIFY_SINGLE_SESSION)
  {
    mmgsdi_evt_notify_specific_session_id(notify_type.session_id, evt_info_ptr);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(ecc_cache_data.data_ptr);
  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(invalid_data_ptr);
} /* mmgsdi_evt_build_and_send_ecc */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_TEST_SIM_STATUS

  DESCRIPTION
    This function builds and sends out the Test SIM event.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_test_sim_status(
  boolean                  notify_all,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot)
{
  mmgsdi_event_data_type  *evt_info_ptr  = NULL;
  uint32                   i             = 0;
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                    = MMGSDI_TEST_SIM_EVT;
  evt_info_ptr->data.test_sim.slot     = slot;
  evt_info_ptr->data.test_sim.test_sim = 0;

  if (slot_data_ptr->app_info_pptr != NULL)
  {
    /*checking GCF test sim card */
    for(i = 0; i < MMGSDI_MAX_APP_INFO; i++)
    {
      if(slot_data_ptr->app_info_pptr[i] == NULL)
      {
          continue;
      }
      if (slot_data_ptr->app_info_pptr[i]->gprs_anite_mcc)
      {
        evt_info_ptr->data.test_sim.test_sim =
          evt_info_ptr->data.test_sim.test_sim | MMGSDI_TEST_CARD_GCF_MASK;
      }
      if (slot_data_ptr->app_info_pptr[i]->tds_test_imsi)
      {
        evt_info_ptr->data.test_sim.test_sim =
          evt_info_ptr->data.test_sim.test_sim | MMGSDI_TEST_CARD_TDS_MASK;
      }
    }
  }

  /* Send test event to client */
  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    evt_info_ptr->client_id = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_test_sim_status */

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_TEMP_UNLOCK_EVT

  DESCRIPTION
    This function builds the temporary unlock event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific client id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_temp_unlock_evt(
  boolean                          notify_all,
  mmgsdi_perso_temp_unlock_evt_info_type temp_unlock_info,
  mmgsdi_client_id_type            client_id)
{
  mmgsdi_event_data_type   *evt_info_ptr = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt              = MMGSDI_PERSO_TEMPORARY_UNLOCK_EVT;
  /* Copy the slot and feature information to event data */
  evt_info_ptr->data.perso_temp_unlock = temp_unlock_info;

  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    evt_info_ptr->client_id = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_temp_unlock_evt */


#ifdef FEATURE_UIM_MULTI_PROFILE
/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PROFILE_DATA_EVT

  DESCRIPTION
    This function builds the profile data event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific client id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_profile_data_evt(
  boolean                  notify_all,
  mmgsdi_profile_info_type profile_info,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_event_data_type   *evt_info_ptr   = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    evt_info_ptr,
    sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                       = MMGSDI_PROFILE_DATA_EVT;
  evt_info_ptr->data.profile_data.slot    = slot_id;
  evt_info_ptr->data.profile_data.profile = profile_info;

  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    evt_info_ptr->client_id = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_profile_data_evt */


/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PROFILE_SWITCHING_EVT

  DESCRIPTION
    This function builds the profile switching event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific client id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_profile_switching_evt(
  boolean                  notify_all,
  mmgsdi_profile_info_type profile_info,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_event_data_type   *evt_info_ptr   = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    evt_info_ptr,
    sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return;
  }

  evt_info_ptr->evt                         = MMGSDI_PROFILE_SWITCHING_EVT;
  evt_info_ptr->data.profile_switch.slot    = slot_id;
  evt_info_ptr->data.profile_switch.profile = profile_info;

  if(notify_all)
  {
    /* broadcast to all */
    mmgsdi_evt_notify_client(evt_info_ptr);
  }
  else
  {
    evt_info_ptr->client_id = client_id;
    mmgsdi_evt_notify_specific_client_id(client_id, evt_info_ptr);
  }

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
} /* mmgsdi_evt_build_and_send_profile_switching_evt */

#endif /* FEATURE_UIM_MULTI_PROFILE */


/*===========================================================================
FUNCTION MMGSDI_EVT_GET_SLOT_FROM_EVENT_DATA

DESCRIPTION
  Extract slot from event data based on the event enum

DEPENDENCIES
  None

RETURN VALUE
  slot id

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
mmgsdi_slot_id_enum_type mmgsdi_evt_get_slot_from_event_data (
  const mmgsdi_event_data_type *event_ptr)
{
  mmgsdi_slot_id_enum_type      slot = MMGSDI_MAX_SLOT_ID_ENUM;

  if (event_ptr == NULL)
  {
    return MMGSDI_MAX_SLOT_ID_ENUM;
  }

  switch(event_ptr->evt)
  {
    case MMGSDI_SESSION_CHANGED_EVT:
      return event_ptr->data.session_changed.app_info.slot;
    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
    case MMGSDI_HIDDENKEY_EVT:
      return event_ptr->data.pin.slot;
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      return event_ptr->data.subscription_ready.app_info.slot;
    case MMGSDI_CARD_INSERTED_EVT:
      return event_ptr->data.card_inserted.slot;
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
      return event_ptr->data.card_error.slot;
    case MMGSDI_PERSO_EVT:
      (void)mmgsdi_util_get_session_app_info(
                        event_ptr->session_id,
                        &slot,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL);
      return slot;
    default:
      break;
  }

  return MMGSDI_MAX_SLOT_ID_ENUM;
}/* mmgsdi_evt_get_slot_from_event_data*/
