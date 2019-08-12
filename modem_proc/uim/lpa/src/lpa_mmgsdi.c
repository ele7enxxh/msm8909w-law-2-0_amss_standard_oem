/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   M M G S D I   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the LPA MMGSDI/UIM handling functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_mmgsdi.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/16   ar      Fix to support bigger profile (> 65535) download
09/15/16   av      Fix CLA byte for iso7816 command GET CHALLENGE
08/03/16   av      Send STATUS to trigger REFRESH
08/01/16   ks      Send correct P2 for GET STATUS command
05/10/16   ll      Fix Store Data P2 0 block number for last block
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "queue.h"

#include "mmgsdilib_common.h"
#include "mmgsdilib_v.h"
#include "mmgsdisessionlib_v.h"

#include "lpalib.h"
#include "lpa_mmgsdi.h"
#include "lpa_util.h"
#include "uim_p.h"
#include "lpa.h"
#include "lpa_lpd.h"
#include <stringl/stringl.h>
#include "lpa_efs.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define LPA_MMGSDI_STATUS_SEND_APDU_LEN                       0x05
#define LPA_MMGSDI_GET_PROFILES_INFO_SEND_APDU_LEN            0x08
#define LPA_MMGSDI_ADD_PROFILE_GET_DATA_SEND_APDU_LEN         0x05
#define LPA_MMGSDI_GET_EID_GET_DATA_SEND_APDU_LEN             0x09
#define LPA_MMGSDI_ADD_PROFILE_GET_CHALLENGE_SEND_APDU_LEN    0x05
#define LPA_MMGSDI_DELETE_PROFILE_DELETE_SEND_APDU_LEN          18
#define LPA_MMGSDI_SEND_APDU_RESP_MIN_LEN                     0x02

#define LPA_MAX_BASIC_LOGICAL_CHANNELS                        0x04
#define LPA_MMGSDI_STATUS_INS                                 0xF2
#define LPA_MMGSDI_GET_STATUS_INS                             0xF2
#define LPA_MMGSDI_GET_CHALLENGE_INS                          0x84
#define LPA_MMGSDI_GET_DATA_INS                               0xCA
#define LPA_MMGSDI_STORE_DATA_INS                             0xE2
#define LPA_MMGSDI_DELETE_INS                                 0xE4

static q_type                lpa_mmgsdi_cmd_q;

static struct {
  mmgsdi_client_id_type      client_id;
  mmgsdi_session_id_type     session_id[LPA_NUM_SLOTS];
  uint8                      channel_id[LPA_NUM_SLOTS];
} lpa_mmgsdi_client_info;

typedef struct {
  lpa_apdu_cmd_enum_type        apdu_req_type;
  uint8                         p1;
  uint8                         p2;
  boolean                       le_present;
  lpa_data_type                 remaining_send_apdu_data;
  lpa_iccid_type                iccid;
  lpa_mmgsdi_callback_type      callback;
  void                          *user_data_ptr;
} lpa_mmgsdi_current_cmd_type;

static lpa_mmgsdi_current_cmd_type lpa_mmgsdi_cmd_info[LPA_NUM_SLOTS];

///static lpa_isdp_aid_type lpa_isdr_aid = {11, {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x06}};



/*===========================================================================

                                  FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION LPA_MMGSDI_FREE_CMD_DATA

DESCRIPTION
  This function frees the command data allocated for mmgsdi response handling

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_mmgsdi_free_cmd_data (
  lpa_mmgsdi_cmd_type          *task_cmd_ptr
)
{
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  switch(task_cmd_ptr->cmd_type)
  {
    case LPA_MMGSDI_CMD_RSP:
      switch(task_cmd_ptr->data.rsp_data.cnf)
      {
        case MMGSDI_SEND_APDU_EXT_CNF:
          LPA_FREE(task_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf.apdu_data.data_ptr);
          break;
        default:
          break;
      }
      break;
    case LPA_MMGSDI_EVENT:
      break;
    default:
      break;
  }
} /* lpa_mmgsdi_free_cmd_data */


/*===========================================================================
   FUNCTION:      LPA_QUEUE_MMGSDI_CMD

   DESCRIPTION:
     This function allows  to queue MMGSDI responses and events to LPA task to be
     processed.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static lpa_result_enum_type  lpa_queue_mmgsdi_resp_cmd (
  lpa_mmgsdi_cmd_type           *task_cmd_ptr
)
{
  lpa_result_enum_type           lpa_status = LPA_SUCCESS;

  if(task_cmd_ptr == NULL)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  LPA_UTIL_ENTER_TASK_STATE_CRIT_SECT;

  do
  {
    /* Do not queue command if LPA task has been stopped */
    if(lpa_task_stopped)
    {
      LPA_MSG_ERR_0("LPA task stopped, cannot queue command");
      lpa_status = LPA_GENERIC_ERROR;
      break;
    }

    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    /* Put the message on the queue */
    q_put(&lpa_mmgsdi_cmd_q, &task_cmd_ptr->link);

    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_LPA_TCB, LPA_MMGSDI_CMD_SIG);
  } while(0);

  LPA_UTIL_LEAVE_TASK_STATE_CRIT_SECT;

  return LPA_SUCCESS;
} /* lpa_queue_mmgsdi_resp_cmd */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_EVT_CB

   DESCRIPTION:
     Callback function registered with MMGSDI for getting notified about
     MMGSDI events. It posts data back to the LPA task for processing.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void lpa_mmgsdi_evt_cb (
  const mmgsdi_event_data_type  * event_ptr
)
{
  lpa_mmgsdi_cmd_type *lpa_mmgsdi_cmd_ptr = NULL;

  if(event_ptr == NULL)
  {
    return;
  }

  if (event_ptr->evt != MMGSDI_CARD_INSERTED_EVT &&
      event_ptr->evt != MMGSDI_CARD_ERROR_EVT &&
      event_ptr->evt != MMGSDI_SESSION_CHANGED_EVT &&
      event_ptr->evt != MMGSDI_SESSION_CLOSE_EVT)
  {
    /* Ignore other MMGSDI events as they are not required */
    return;
  }

  lpa_mmgsdi_cmd_ptr = (lpa_mmgsdi_cmd_type*)LPA_MALLOC(sizeof(lpa_mmgsdi_cmd_type));
  if(lpa_mmgsdi_cmd_ptr == NULL)
  {
    return;
  }

  lpa_mmgsdi_cmd_ptr->cmd_type = LPA_MMGSDI_EVENT;
  memscpy((void *)&lpa_mmgsdi_cmd_ptr->data.event_data.mmgsdi_evt_data,
          sizeof(mmgsdi_event_data_type),
          event_ptr,
          sizeof(mmgsdi_event_data_type));

  (void)lpa_queue_mmgsdi_resp_cmd(lpa_mmgsdi_cmd_ptr);
} /* lpa_mmgsdi_evt_cb */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_SEND_RESPONSE_TO_CLIENT

   DESCRIPTION:
     Calls client callback with the LPA MMGSDI's response

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void lpa_mmgsdi_send_response_to_client(
  lpa_data_type                    data,
  lpa_result_enum_type             lpa_status,
  lpa_slot_id_enum_type            lpa_slot
)
{
  lpa_mmgsdi_resp_data_type        *resp_ptr = NULL;

  if(lpa_slot >= LPA_NUM_SLOTS ||
     lpa_mmgsdi_cmd_info[lpa_slot].callback == NULL)
  {
     LPA_MSG_HIGH_1("Unable to send response to some client on slot=0x%x. Maybe the client is already notified", lpa_slot);
     return;
  }

  resp_ptr = (lpa_mmgsdi_resp_data_type*) LPA_MALLOC(sizeof(lpa_mmgsdi_resp_data_type));
  if(resp_ptr == NULL)
  {
    return;
  }

  /* Call the callback to report the failure */
  resp_ptr->cmd_type = lpa_mmgsdi_cmd_info[lpa_slot].apdu_req_type;
  resp_ptr->slot_id = lpa_slot;
  resp_ptr->data_len = data.data_len;
  resp_ptr->data_ptr = data.data_ptr;

  lpa_mmgsdi_cmd_info[lpa_slot].callback(lpa_status,
                                         resp_ptr,
                                         lpa_mmgsdi_cmd_info[lpa_slot].user_data_ptr);
  lpa_mmgsdi_command_cleanup(lpa_slot);
  LPA_FREE(resp_ptr);
} /* lpa_mmgsdi_send_response_to_client */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_CMD_RSP_DUMMY_CB

   DESCRIPTION:
     Dummy callback function for various MMGSDI APIs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void lpa_mmgsdi_cmd_rsp_dummy_cb (
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  (void) status;
  (void) cnf;
  (void) cnf_ptr;
} /* lpa_mmgsdi_cmd_rsp_dummy_cb */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_CMD_RSP_CB

   DESCRIPTION:
     Callback function for various MMGSDI APIs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void lpa_mmgsdi_cmd_rsp_cb (
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  lpa_mmgsdi_cmd_type      *lpa_mmgsdi_cmd_ptr = NULL;
  lpa_result_enum_type      lpa_status         = LPA_SUCCESS;

  if (cnf_ptr == NULL)
  {
    return;
  }

  lpa_mmgsdi_cmd_ptr = (lpa_mmgsdi_cmd_type*)LPA_MALLOC(sizeof(lpa_mmgsdi_cmd_type));
  if (lpa_mmgsdi_cmd_ptr == NULL)
  {
    return;
  }

  switch (cnf)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.client_id_and_evt_reg_cnf =
        cnf_ptr->client_id_and_evt_reg_cnf;
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.session_open_cnf =
        cnf_ptr->session_open_cnf;
      break;

    case MMGSDI_SESSION_CLOSE_CNF:
      /* Do nothing for session close */
      break;

    case MMGSDI_SEND_APDU_EXT_CNF:
      lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf =
        cnf_ptr->send_apdu_ext_cnf;
      if(cnf_ptr->send_apdu_ext_cnf.apdu_data.data_len > 0 &&
         cnf_ptr->send_apdu_ext_cnf.apdu_data.data_ptr)
      {
        lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf.apdu_data.data_ptr =
          LPA_MALLOC(cnf_ptr->send_apdu_ext_cnf.apdu_data.data_len);
        if (lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf.apdu_data.data_ptr == NULL)
        {
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }
        lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf.apdu_data.data_len =
          cnf_ptr->send_apdu_ext_cnf.apdu_data.data_len;
        memscpy(lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf.apdu_data.data_ptr,
                lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf_data.send_apdu_ext_cnf.apdu_data.data_len,
                cnf_ptr->send_apdu_ext_cnf.apdu_data.data_ptr,
                cnf_ptr->send_apdu_ext_cnf.apdu_data.data_len);
      }
      break;

    default:
      LPA_MSG_ERR_1("Invalid MMGSDI CNF: 0x%x", cnf);
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  if(lpa_status != LPA_SUCCESS)
  {
    LPA_FREE(lpa_mmgsdi_cmd_ptr);
    return;
  }

  lpa_mmgsdi_cmd_ptr->cmd_type             = LPA_MMGSDI_CMD_RSP;
  lpa_mmgsdi_cmd_ptr->data.rsp_data.cnf    = cnf;
  lpa_mmgsdi_cmd_ptr->data.rsp_data.status = status;

  (void) lpa_queue_mmgsdi_resp_cmd(lpa_mmgsdi_cmd_ptr);
}/* lpa_mmgsdi_cmd_rsp_cb */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_SELECT_ISDR

   DESCRIPTION:
     This function calls the MMGSDI API for opening of the logical channel
     with the ISD-R and selecting the ISD-R.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static lpa_result_enum_type lpa_mmgsdi_select_isdr(
  lpa_slot_id_enum_type      slot_id
)
{
  mmgsdi_session_type_enum_type    mmgsdi_session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type          mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_non_prov_app_info_type    non_prov_app_data;
  lpa_isdp_aid_type                lpa_isdr_aid         = {16, {0xA0, 0x00, 0x00,
                                                           0x05, 0x59, 0x10, 0x10,
                                                           0xFF, 0xFF, 0xFF, 0xFF,
                                                           0x89, 0x00, 0x00, 0x01,
                                                           0x00}};

  memset(&non_prov_app_data, 0x00, sizeof(mmgsdi_non_prov_app_info_type));

  if (slot_id == LPA_SLOT_1)
  {
    mmgsdi_session_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
  }
  else if (slot_id == LPA_SLOT_2)
  {
    mmgsdi_session_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
  }
  else
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  non_prov_app_data.app_id_data.data_len = lpa_isdr_aid.data_len;
  non_prov_app_data.app_id_data.data_ptr = lpa_isdr_aid.data;
  non_prov_app_data.exclusive_channel    = TRUE;

  /* Open non prov session to the ISD-R */
  mmgsdi_status = mmgsdi_session_open(lpa_mmgsdi_client_info.client_id,
                                      mmgsdi_session_type,
                                      &non_prov_app_data,
                                      lpa_mmgsdi_evt_cb,
                                      FALSE,
                                      lpa_mmgsdi_cmd_rsp_cb,
                                      (mmgsdi_client_data_type) 0);

  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_select_isdr */


/*===========================================================================
   FUNCTION:      LPA_PROCESS_MMGSDI_SELECT_ISDR_RSP

   DESCRIPTION:
     LPA MMGSDI ISDR session open response handling. Peek into the original
     request type and trigger the original send apdu request (that caused
     selection of ISDR at the first place).

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static lpa_result_enum_type lpa_process_mmgsdi_select_isdr_rsp(
  lpa_slot_id_enum_type                            slot_id
)
{
  lpa_result_enum_type             lpa_status           = LPA_GENERIC_ERROR;

  if(slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  switch(lpa_mmgsdi_cmd_info[slot_id].apdu_req_type)
  {
    case LPA_MMGSDI_APDU_GET_STATUS:
      /* Create a Get STATUS APDU and send it to MMGSDI. */
      lpa_status = lpa_mmgsdi_get_status(
                     slot_id,
                     lpa_mmgsdi_cmd_info[slot_id].callback,
                     lpa_mmgsdi_cmd_info[slot_id].user_data_ptr);
      break;
    case LPA_MMGSDI_APDU_STORE_DATA:
      /* Create a STORE DATA APDU and send it to MMGSDI. */
      {
        /* The global "remaining_send_apdu_data.data_ptr" gets free'd inside
           lpa_mmgsdi_store_data... so, we need to create our own store_data
           data_ptr */
        lpa_data_type                    store_data;

        memset(&store_data, 0x00, sizeof(store_data));
        store_data.data_len = lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len;
        if(store_data.data_len > 0 &&
           lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr)
        {
          store_data.data_ptr = LPA_MALLOC(store_data.data_len);
          if(store_data.data_ptr)
          {
            memscpy(store_data.data_ptr,
                    store_data.data_len,
                    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr,
                    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len);
      lpa_status = lpa_mmgsdi_store_data(
                     slot_id,
                     lpa_mmgsdi_cmd_info[slot_id].p1,
                           store_data,
                     lpa_mmgsdi_cmd_info[slot_id].le_present,
                     lpa_mmgsdi_cmd_info[slot_id].callback,
                     lpa_mmgsdi_cmd_info[slot_id].user_data_ptr);
            LPA_FREE(store_data.data_ptr);
            store_data.data_len = 0;
          }
        }
      }
      break;
    case LPA_MMGSDI_APDU_DELETE:
      /* Create a DELETE APDU and send it to MMGSDI. */
      lpa_status = lpa_mmgsdi_delete(
                     slot_id,
                     lpa_mmgsdi_cmd_info[slot_id].iccid,
                     lpa_mmgsdi_cmd_info[slot_id].callback,
                     lpa_mmgsdi_cmd_info[slot_id].user_data_ptr);
      break;
    case LPA_MMGSDI_APDU_GET_DATA:
      /* Create a GET DATA APDU and send it to MMGSDI. */
      lpa_status = lpa_mmgsdi_get_data(
                     slot_id,
                     lpa_mmgsdi_cmd_info[slot_id].p2,
                     lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data,
                     lpa_mmgsdi_cmd_info[slot_id].callback,
                     lpa_mmgsdi_cmd_info[slot_id].user_data_ptr);
      break;
    case LPA_MMGSDI_APDU_GET_CHALLENGE:
      /* Create a GET CHALLENGE APDU and send it to MMGSDI. */
      lpa_status = lpa_mmgsdi_get_challenge(
                     slot_id,
                     lpa_mmgsdi_cmd_info[slot_id].callback,
                     lpa_mmgsdi_cmd_info[slot_id].user_data_ptr);
      break;
    default:
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  return lpa_status;
} /* lpa_process_mmgsdi_select_isdr_rsp */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_PROCSES_SEND_APDU_EXT_RSP

   DESCRIPTION:
     LPA MMGSDI send apdu response handling

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static lpa_result_enum_type lpa_mmgsdi_process_send_apdu_ext_rsp(
  mmgsdi_send_apdu_ext_cnf_type    send_apdu_cnf
)
{
  lpa_result_enum_type             lpa_status           = LPA_SUCCESS;
  lpa_slot_id_enum_type            lpa_slot             = LPA_SLOT_1;
  mmgsdi_sw_type                   sw;
  lpa_data_type                    resp_data;

  memset(&sw, 0x00, sizeof(sw));
  memset(&resp_data, 0x00, sizeof(resp_data));

  /* Convert MMGSDI status to LPA status and also make sure sw1/sw2 are
     ok... otherwise report failure to the LPA client */
  if(LPA_SUCCESS != lpa_util_convert_mmgsdi_slot_to_lpa_slot(
                     send_apdu_cnf.response_header.slot_id,
                     &lpa_slot))
  {
    return LPA_GENERIC_ERROR;
  }

  sw.sw1 = send_apdu_cnf.response_header.status_word.sw1;
  sw.sw2 = send_apdu_cnf.response_header.status_word.sw2;

  LPA_MSG_HIGH_2("send apdu response, sw1=0x%x,sw2=0x%x", sw.sw1, sw.sw2);

  /* For send_apdu request, even if MMGSDI returns MMGSDI_SUCCESS as the
     overall command response status, the card may actually have failed the
     send apdu request... in that case, the sw1/sw2 would contain the
     error sw1/sw2s */
  lpa_status = lpa_util_map_sw1_sw2_to_lpa_status(sw);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* APDU response data must atleast contain SW1 and SW2 (last two bytes
     of the APDU response) */
  if(send_apdu_cnf.apdu_data.data_len < LPA_MMGSDI_SEND_APDU_RESP_MIN_LEN)
  {
    return LPA_GENERIC_ERROR;
  }


  /* See if there is any remaining block of the APDU that needs to be processed.
     If so, send the request. Currently, only STORE DATA is expected to be
     processed in multiple blocks... hence, following check is done only for
     STORE DATA command */
  if(lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_len > 0 &&
     lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_ptr &&
     lpa_mmgsdi_cmd_info[lpa_slot].apdu_req_type == LPA_MMGSDI_APDU_STORE_DATA)
  {
    /* The global "remaining_send_apdu_data.data_ptr" gets free'd inside
       lpa_mmgsdi_store_data... so, we need to create our own store_data
       data_ptr */
    lpa_data_type                    store_data;

    lpa_status = LPA_GENERIC_ERROR;

    memset(&store_data, 0x00, sizeof(store_data));
    store_data.data_len = lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_len;
    if(store_data.data_len > 0 &&
       lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_ptr)
    {
      store_data.data_ptr = LPA_MALLOC(store_data.data_len);
      if(store_data.data_ptr)
      {
        memscpy(store_data.data_ptr,
                store_data.data_len,
                lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_ptr,
                lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_len);
        lpa_status = lpa_mmgsdi_store_data(
                       lpa_slot,
                                       lpa_mmgsdi_cmd_info[lpa_slot].p1,
                       store_data,
                                       lpa_mmgsdi_cmd_info[lpa_slot].le_present,
                                       lpa_mmgsdi_cmd_info[lpa_slot].callback,
                                       lpa_mmgsdi_cmd_info[lpa_slot].user_data_ptr);
        LPA_FREE(store_data.data_ptr);
        store_data.data_len = 0;
      }
    }
    return lpa_status;
  }

  /* Write the STORE send apdu response to the card */
  if(lpa_mmgsdi_cmd_info[lpa_slot].apdu_req_type == LPA_MMGSDI_APDU_STORE_DATA)
  {
    (void) lpa_efs_write(LPA_FILE_STORE_DATA_RESP_FROM_CARD,
                         send_apdu_cnf.apdu_data.data_len,
                         send_apdu_cnf.apdu_data.data_ptr,
                         lpa_slot);
  }

  /* Call the callback to report the result of SEND APDU */
  resp_data.data_len = (send_apdu_cnf.apdu_data.data_len - LPA_MMGSDI_SEND_APDU_RESP_MIN_LEN);
  resp_data.data_ptr = send_apdu_cnf.apdu_data.data_ptr;

  lpa_mmgsdi_send_response_to_client(resp_data, lpa_status, lpa_slot);

  return lpa_status;
} /* lpa_mmgsdi_process_send_apdu_ext_rsp */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_CHANNEL_TO_CLA_BYTE

   DESCRIPTION:
     LPA MMGSDI converts the channel id to the CLA byte

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uint8

   SIDE EFFECTS:
     None
===========================================================================*/
static uint8 lpa_mmgsdi_channel_to_cla_byte (
  uint8                               channel_id,
  boolean                             is_inter_industry
)
{
  uint8           class_byte = 0xFF;

  if(channel_id < LPA_MAX_BASIC_LOGICAL_CHANNELS)
  {
    /* For basic logical channels,
         * b7 is 0.
         * for inter-industry, b8 is 0.
         * for non inter-industry (proprietary), b8 is 1. */
    class_byte = is_inter_industry ? channel_id : (0x80 + channel_id);
  }
  else if(channel_id < UIM_MAX_CHANNELS)
  {
    /* For extended logical channels,
         * b7 is 1.
         * for inter-industry, b8 is 0.
         * for non inter-industry (proprietary), b8 is 1. */
    class_byte = is_inter_industry ? (0x40 + channel_id - LPA_MAX_BASIC_LOGICAL_CHANNELS) : (0xC0 + channel_id - LPA_MAX_BASIC_LOGICAL_CHANNELS);
  }

  return class_byte;
} /* lpa_mmgsdi_channel_to_cla_byte */


/*===========================================================================
   FUNCTION:      LPA_PROCESS_MMGSDI_CMD_RSP

   DESCRIPTION:
     LPA MMGSDI processing function for MMGSDI responses

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_process_mmgsdi_cmd_rsp (
  const lpa_mmgsdi_cmd_rsp_data_type *rsp_data_ptr
)
{
  lpa_result_enum_type               lpa_status         = LPA_GENERIC_ERROR;
  lpa_slot_id_enum_type              lpa_slot           = LPA_NUM_SLOTS;
  lpa_data_type                      resp_data;

  memset(&resp_data, 0x00, sizeof(resp_data));

  if(rsp_data_ptr == NULL)
  {
    /* LPA client is not notified of this error here. The client will
       hear from LPA once the command timer expires */
    return;
  }

  lpa_status = lpa_util_convert_mmgsdi_status_to_lpa_status(rsp_data_ptr->status);

  if(lpa_status == LPA_SUCCESS)
  {
    switch(rsp_data_ptr->cnf)
    {
      case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
        /* Save the client id to the global for later use */
        lpa_mmgsdi_client_info.client_id =
          rsp_data_ptr->cnf_data.client_id_and_evt_reg_cnf.response_header.client_id;
        break;

      case MMGSDI_SESSION_OPEN_CNF:
        /* Save the session id and the channel id to the global for later
           use when sending apdus to the ISDR */
        if(LPA_SUCCESS != lpa_util_convert_mmgsdi_slot_to_lpa_slot(
                            rsp_data_ptr->cnf_data.response_header.slot_id,
                            &lpa_slot))
        {
          break;
        }
        lpa_mmgsdi_client_info.session_id[lpa_slot] =
          rsp_data_ptr->cnf_data.session_open_cnf.session_id;
        if(rsp_data_ptr->cnf_data.session_open_cnf.channel_id > 0 &&
           rsp_data_ptr->cnf_data.session_open_cnf.channel_id < UIM_MAX_CHANNELS)
        {
          lpa_mmgsdi_client_info.channel_id[lpa_slot]=
            (uint8) (rsp_data_ptr->cnf_data.session_open_cnf.channel_id);
        }
        else
        {
          LPA_MSG_ERR_1("Invalid logical channel:0x%x returned",
                        rsp_data_ptr->cnf_data.session_open_cnf.channel_id);
          lpa_status = LPA_GENERIC_ERROR;
          break;
        }
        lpa_status = lpa_process_mmgsdi_select_isdr_rsp(lpa_slot);
        break;

      case MMGSDI_SEND_APDU_EXT_CNF:
        if(LPA_SUCCESS != lpa_util_convert_mmgsdi_slot_to_lpa_slot(
                            rsp_data_ptr->cnf_data.response_header.slot_id,
                            &lpa_slot))
        {
          break;;
        }
        lpa_status = lpa_mmgsdi_process_send_apdu_ext_rsp(
                       rsp_data_ptr->cnf_data.send_apdu_ext_cnf);
        break;

      default:
        LPA_MSG_ERR_1("MMGSDI response (0x%x) received for a command LPA never sent to it",
                      rsp_data_ptr->cnf);
        lpa_status = LPA_GENERIC_ERROR;
        break;
    }
  }

  if(lpa_status != LPA_SUCCESS)
  {
    /* Call the callback to report the failure */
    (void) lpa_util_convert_mmgsdi_slot_to_lpa_slot(
             rsp_data_ptr->cnf_data.response_header.slot_id,
             &lpa_slot);
    resp_data.data_len = 0;
    resp_data.data_ptr = NULL;

    lpa_mmgsdi_send_response_to_client(resp_data, lpa_status, lpa_slot);
  }
} /* lpa_process_mmgsdi_cmd_rsp */


/*===========================================================================
   FUNCTION:      LPA_PROCESS_MMGSDI_EVENT

   DESCRIPTION:
     LPA MMGSDI processing function for MMGSDI Events

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_process_mmgsdi_event (
  const lpa_mmgsdi_evt_data_type *event_data_ptr
)
{
  lpa_slot_id_enum_type   lpa_slot = LPA_NUM_SLOTS;

  switch (event_data_ptr->mmgsdi_evt_data.evt)
  {
    case MMGSDI_CARD_ERROR_EVT:
      LPA_MSG_HIGH_1("LPA MMGSDI received MMGSDI_CARD_INSERTED_EVT for slot=0x%x",
                     event_data_ptr->mmgsdi_evt_data.data.card_error.slot);
      if(LPA_SUCCESS == lpa_util_convert_mmgsdi_slot_to_lpa_slot(
                          event_data_ptr->mmgsdi_evt_data.data.card_error.slot,
                          &lpa_slot))
      {
        /* If a command is in progress, send response to lpa_mmgsdi client
           for the lpa mmgsdi command in progress */
        lpa_data_type                    data = {0, NULL};

        lpa_mmgsdi_send_response_to_client(data,
                                           LPA_GENERIC_ERROR,
                                           lpa_slot);

        if(lpa_mmgsdi_client_info.session_id[lpa_slot])
        {
          lpa_mmgsdi_client_info.session_id[lpa_slot] = MMGSDI_SESSION_ID_ZERO;
        }
      }
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      LPA_MSG_HIGH_1("LPA MMGSDI received MMGSDI_SESSION_CHANGED_EVT; session_activated=0x%x",
                     event_data_ptr->mmgsdi_evt_data.data.session_changed.activated);
      if(!event_data_ptr->mmgsdi_evt_data.data.session_changed.activated)
      {
        for(lpa_slot = LPA_SLOT_1; lpa_slot < LPA_NUM_SLOTS; lpa_slot++)
      {
          if(lpa_mmgsdi_client_info.session_id[lpa_slot] ==
            event_data_ptr->mmgsdi_evt_data.session_id)
        {
          /* If a command is in progress, send response to lpa_mmgsdi client
             for the lpa mmgsdi command in progress */
          lpa_data_type                    data = {0, NULL};

          lpa_mmgsdi_send_response_to_client(data,
                                             LPA_GENERIC_ERROR,
                                               lpa_slot);
            lpa_mmgsdi_client_info.session_id[lpa_slot] = MMGSDI_SESSION_ID_ZERO;
            break;
        }
        }
      }
      break;

    case MMGSDI_SESSION_CLOSE_EVT:
      LPA_MSG_HIGH_0("LPA MMGSDI received MMGSDI_SESSION_CLOSE_EVT");
      for(lpa_slot = LPA_SLOT_1; lpa_slot < LPA_NUM_SLOTS; lpa_slot++)
      {
        if(lpa_mmgsdi_client_info.session_id[lpa_slot] ==
           event_data_ptr->mmgsdi_evt_data.session_id)
      {
        /* If a command is in progress, send response to lpa_mmgsdi client
           for the lpa mmgsdi command in progress */
        lpa_data_type                    data = {0, NULL};

        lpa_mmgsdi_send_response_to_client(data,
                                           LPA_GENERIC_ERROR,
                                             lpa_slot);
          lpa_mmgsdi_client_info.session_id[lpa_slot] = MMGSDI_SESSION_ID_ZERO;
          break;
      }
      }
      break;

    default:
      return;
  }

}/* lpa_process_mmgsdi_event */


/*===========================================================================
FUNCTION LPA_MMGSDI_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, is called to process response from
  MMGSDI, for both command responses and for events.

DEPENDENCIES
  LPA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_mmgsdi_handle_cmd_sig (
  void
)
{
  lpa_mmgsdi_cmd_type          *task_cmd_ptr  = NULL;

  task_cmd_ptr = (lpa_mmgsdi_cmd_type*)(q_get(&lpa_mmgsdi_cmd_q));

  /* If there is a command on either of the queues */
  if(task_cmd_ptr != NULL)
  {
    LPA_MSG_HIGH_1("LPA_MMGSDI_CMD_SIG received, cmd_type 0x%x",
                   task_cmd_ptr->cmd_type);

    switch (task_cmd_ptr->cmd_type)
    {
      case LPA_MMGSDI_CMD_RSP:
        lpa_process_mmgsdi_cmd_rsp(&task_cmd_ptr->data.rsp_data);
        break;
      case LPA_MMGSDI_EVENT:
        lpa_process_mmgsdi_event(&task_cmd_ptr->data.event_data);
        break;
      default:
        break;
    }
  }

  (void) rex_clr_sigs(rex_self(), LPA_MMGSDI_CMD_SIG);

  /* Set the signal if there are one or more commands to look at */
  if (q_cnt(&lpa_mmgsdi_cmd_q) > 0)
  {
    /*  Items on the queue. Set the signal */
    (void) rex_set_sigs(rex_self(), LPA_MMGSDI_CMD_SIG);
  }

  lpa_mmgsdi_free_cmd_data(task_cmd_ptr);
  LPA_FREE(task_cmd_ptr);
} /* lpa_mmgsdi_handle_cmd_sig */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_INIT

   DESCRIPTION:
     LPA MMGSDI init function called during LPA task init

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_mmgsdi_init (
)
{
  memset(&lpa_mmgsdi_cmd_info, 0x00, sizeof(lpa_mmgsdi_cmd_info));
  memset(&lpa_mmgsdi_client_info, 0x00, sizeof(lpa_mmgsdi_client_info));

  (void) q_init(&lpa_mmgsdi_cmd_q);

  if(!lpa_mmgsdi_client_info.client_id)
  {
     /* client id reg */
    (void) mmgsdi_client_id_and_evt_reg(
             lpa_mmgsdi_evt_cb,
             lpa_mmgsdi_cmd_rsp_cb,
             (mmgsdi_client_data_type) 0);
  }
} /* lpa_mmgsdi_init */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_GET_STATUS

   DESCRIPTION:
     API creates and sends a GET STATUS APDU to the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_get_status (
  lpa_slot_id_enum_type         slot_id,
  lpa_mmgsdi_callback_type      callback,
  void                          *user_data_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type     mmgsdi_slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_send_apdu_data_type   apdu_data;

  if(LPA_SUCCESS != lpa_util_convert_lpa_slot_to_mmgsdi_slot(slot_id, &mmgsdi_slot_id))
  {
    return LPA_GENERIC_ERROR;
  }

  /* Save command related input data to the global which we can use during
     handling of the response from MMGSDI */
  lpa_mmgsdi_cmd_info[slot_id].apdu_req_type  = LPA_MMGSDI_APDU_GET_STATUS;
  lpa_mmgsdi_cmd_info[slot_id].callback       = callback;
  lpa_mmgsdi_cmd_info[slot_id].user_data_ptr  = user_data_ptr;

  LPA_MSG_HIGH_1("LPA MMGSDI Get Status request for slot_id:0x%x", slot_id);

  if(!lpa_mmgsdi_client_info.session_id[slot_id])
  {
    /* Open session and select ISDR if not done already */
    return lpa_mmgsdi_select_isdr(slot_id);
  }

  /* Create a Get STATUS APDU for GetProfilesInfo and send it to MMGSDI */
  memset(&apdu_data, 0x00, sizeof(apdu_data));

  apdu_data.data_len = LPA_MMGSDI_GET_PROFILES_INFO_SEND_APDU_LEN;
  apdu_data.data_ptr = (uint8*)LPA_MALLOC(apdu_data.data_len);

  if(apdu_data.data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET]     = lpa_mmgsdi_channel_to_cla_byte(
                                                         lpa_mmgsdi_client_info.channel_id[slot_id],
                                                         FALSE);
  apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET]    = LPA_MMGSDI_GET_STATUS_INS; /* INS */
  apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET]        = 0x40;                      /* P1 */
  apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]        = 0x02;                      /* P2 */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET]     = 0x02;                      /* Lc */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1] = 0x4F;                      /* Data[0] */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 2] = 0x00;                      /* Data[1] */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 3] = 0x00;                      /* Le */

  mmgsdi_status = mmgsdi_send_apdu_ext(lpa_mmgsdi_client_info.client_id,
                                       mmgsdi_slot_id,
                                       apdu_data,
                                       MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                       lpa_mmgsdi_cmd_rsp_cb,
                                       (mmgsdi_client_data_type) 0);

  LPA_FREE(apdu_data.data_ptr);
  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_get_status */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_STORE_DATA

   DESCRIPTION:
     Utility called to send STORE DATA command to the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_store_data (
  lpa_slot_id_enum_type         slot_id,
  uint8                         p1,
  lpa_data_type                 data,
  boolean                       le_present,
  lpa_mmgsdi_callback_type      callback,
  void                          *user_data_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type     mmgsdi_slot_id     = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_send_apdu_data_type   apdu_data;
  uint8                        effective_data_len = 0;

  if(LPA_SUCCESS != lpa_util_convert_lpa_slot_to_mmgsdi_slot(slot_id, &mmgsdi_slot_id))
  {
    return LPA_GENERIC_ERROR;
  }

  if(data.data_len == 0 ||
     data.data_ptr == NULL)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  lpa_mmgsdi_cmd_info[slot_id].apdu_req_type      = LPA_MMGSDI_APDU_STORE_DATA;
  lpa_mmgsdi_cmd_info[slot_id].p1                 = p1;
  lpa_mmgsdi_cmd_info[slot_id].le_present         = le_present;
  lpa_mmgsdi_cmd_info[slot_id].callback           = callback;
  lpa_mmgsdi_cmd_info[slot_id].user_data_ptr      = user_data_ptr;

  LPA_MSG_HIGH_1("LPA MMGSDI Store Data request slot_id:0x%x", slot_id);

  /* Open session and select ISDR if not done already */
  if(!lpa_mmgsdi_client_info.session_id[slot_id])
  {
    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr = LPA_MALLOC(data.data_len);
    if (lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr == NULL)
    {
      return LPA_ERROR_HEAP_EXHAUSTED;
    }

    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len = data.data_len;
    memscpy(lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr,
            lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len,
            data.data_ptr,
            data.data_len);
    return lpa_mmgsdi_select_isdr(slot_id);
  }

  /* Note that we can send only a 261-byte long APDU to the card, which means
     that if we subtract 6 bytes for CLA, INS, P1, P2, Lc, Le, the DATA part of
     the APDU can at max be 255 bytes long. So, check if the incoming data_len
     is greater than 255. If so, create chunks of at max 255 bytes each and
     call MMGSDI send apdu API for each of those chunks individually */
  if(data.data_len > UIM_MAX_TRANS_SIZE_OUT)
  {
    effective_data_len = UIM_MAX_TRANS_SIZE_OUT;
  }
  else
  {
    effective_data_len = (uint8) data.data_len;
  }
  /* Create a STORE DATA APDU and sent it to MMGSDI */
  memset(&apdu_data, 0x00, sizeof(apdu_data));

  apdu_data.data_len = UIM_CASE2_7816_APDU_SIZE + effective_data_len;
  if(le_present)
  {
    apdu_data.data_len += 1; /* One extra byte for Le in this case */
  }

  apdu_data.data_ptr = (uint8*)LPA_MALLOC(apdu_data.data_len);

  if (apdu_data.data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  /* STORE DATA as per 4.1.1.1 of SGP-02-v3.0 */
  apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET]   = lpa_mmgsdi_channel_to_cla_byte(
                                                       lpa_mmgsdi_client_info.channel_id[slot_id],
                                                       FALSE);
  apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET]  = LPA_MMGSDI_STORE_DATA_INS;/* INS */
  apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET]      = p1;                       /* P1 */
  apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]      = 0x00;                     /* P2 */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET]   = effective_data_len;       /* Lc */

  /* Starting 6th byte, Copy at max 255 bytes to the DATA portion of the APDU */
  memscpy(&apdu_data.data_ptr[UIM_CASE2_7816_APDU_SIZE],
          (apdu_data.data_len - UIM_CASE2_7816_APDU_SIZE),
          data.data_ptr,
          effective_data_len);

  if(le_present)
  {
    apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + effective_data_len + 1] = 0x00;/* Le... = 00 */
  }

  LPA_FREE(lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr);
  lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len = 0;

  if(data.data_len > effective_data_len)
  {
    /* Save the remaining input data in the global for processing as subsequent
       block */
    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len = (uint32) (data.data_len - effective_data_len);
    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr =
      LPA_MALLOC(lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len);

    if(lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr == NULL)
    {
      LPA_FREE(apdu_data.data_ptr);
      return LPA_ERROR_HEAP_EXHAUSTED;
    }

    memscpy(lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr,
            lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len,
            (data.data_ptr + effective_data_len),
            lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len);

    /* Update P1 and P2 for the current APDU to reflect that there are more
       blocks to be processed */
    apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET] = lpa_mmgsdi_cmd_info[slot_id].p2;
    lpa_mmgsdi_cmd_info[slot_id].p2 += 1;
    apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET] = 0x11; /* P1... more blocks */
  }
  else if (lpa_mmgsdi_cmd_info[slot_id].p2 > 0)
  {
    apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET] = lpa_mmgsdi_cmd_info[slot_id].p2;
  }

  mmgsdi_status = mmgsdi_send_apdu_ext(lpa_mmgsdi_client_info.client_id,
                                       mmgsdi_slot_id,
                                       apdu_data,
                                       MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                       lpa_mmgsdi_cmd_rsp_cb,
                                       (mmgsdi_client_data_type) 0);

  LPA_FREE(apdu_data.data_ptr);
  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_store_data */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_DELETE

   DESCRIPTION:
     API used to send a DELETE APDU to the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_delete (
  lpa_slot_id_enum_type         slot_id,
  lpa_iccid_type                iccid,
  lpa_mmgsdi_callback_type      callback,
  void                          *user_data_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_send_apdu_data_type   apdu_data;
  mmgsdi_slot_id_enum_type     mmgsdi_slot_id     = MMGSDI_MAX_SLOT_ID_ENUM;

  if(LPA_SUCCESS != lpa_util_convert_lpa_slot_to_mmgsdi_slot(slot_id, &mmgsdi_slot_id))
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_mmgsdi_cmd_info[slot_id].iccid               = iccid;
  lpa_mmgsdi_cmd_info[slot_id].callback            = callback;
  lpa_mmgsdi_cmd_info[slot_id].user_data_ptr       = user_data_ptr;

  LPA_MSG_HIGH_1("LPA MMGSDI Delete request slot_id:0x%x", slot_id);

  if(!lpa_mmgsdi_client_info.session_id[slot_id])
  {
    /* Open session and select ISDR if not done already */
    return lpa_mmgsdi_select_isdr(slot_id);
  }

  /* Create a DELETE APDU and sent it to MMGSDI */
  memset(&apdu_data, 0x00, sizeof(apdu_data));

  apdu_data.data_len = LPA_MMGSDI_DELETE_PROFILE_DELETE_SEND_APDU_LEN;
  apdu_data.data_ptr = (uint8*)LPA_MALLOC(apdu_data.data_len);

  if (apdu_data.data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }
  apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET]     = lpa_mmgsdi_channel_to_cla_byte(
                                                         lpa_mmgsdi_client_info.channel_id[slot_id],
                                                         FALSE);
  apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET]    = LPA_MMGSDI_DELETE_INS;/* INS for DELETE */
  apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET]        = 0x00;                 /* P1 */
  apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]        = 0x40;                 /* P2 */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET]     = 0x0C;                 /* Lc */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1] = 0x5A;                 /* Data[0] */

  memscpy(&apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 2],
          (apdu_data.data_len - UIM_CASE3_4_7816_LC_OFFSET + 2),
          &iccid,
          sizeof(iccid));
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 2 + sizeof(iccid)] = 0x00; /* Le */

  lpa_mmgsdi_cmd_info[slot_id].apdu_req_type = LPA_MMGSDI_APDU_DELETE;
  mmgsdi_status = mmgsdi_send_apdu_ext(lpa_mmgsdi_client_info.client_id,
                                       mmgsdi_slot_id,
                                       apdu_data,
                                       MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                       lpa_mmgsdi_cmd_rsp_cb,
                                       (mmgsdi_client_data_type) 0);

  LPA_FREE(apdu_data.data_ptr);
  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_delete */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_GET_CHALLENGE

   DESCRIPTION:
     Utility called to send GET CHALLENGE command to the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_get_challenge (
  lpa_slot_id_enum_type         slot_id,
  lpa_mmgsdi_callback_type      callback,
  void                          *user_data_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_send_apdu_data_type   apdu_data;
  mmgsdi_slot_id_enum_type     mmgsdi_slot_id = MMGSDI_MAX_SLOT_ID_ENUM;

  if(LPA_SUCCESS != lpa_util_convert_lpa_slot_to_mmgsdi_slot(slot_id, &mmgsdi_slot_id))
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_mmgsdi_cmd_info[slot_id].apdu_req_type   = LPA_MMGSDI_APDU_GET_CHALLENGE;
  lpa_mmgsdi_cmd_info[slot_id].callback        = callback;
  lpa_mmgsdi_cmd_info[slot_id].user_data_ptr   = user_data_ptr;

  LPA_MSG_HIGH_1("LPA MMGSDI Get Challenge request slot_id:0x%x", slot_id);

  if(!lpa_mmgsdi_client_info.session_id[slot_id])
  {
    /* Open session and select ISDR if not done already */
    return lpa_mmgsdi_select_isdr(slot_id);
  }

  /* Create a GET CHALLENGE APDU and send it to MMGSDI */
  memset(&apdu_data, 0x00, sizeof(apdu_data));

  /* The APDU is 5 bytes long: INS+CLA+P1+P2+Le*/
  apdu_data.data_len = UIM_CASE2_7816_APDU_SIZE;
  apdu_data.data_ptr = (uint8*)LPA_MALLOC(apdu_data.data_len);

  if (apdu_data.data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  /* GET CHALLENGE per 11.1.18 in 102221 */
  apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET]  = lpa_mmgsdi_channel_to_cla_byte(
                                                      lpa_mmgsdi_client_info.channel_id[slot_id],
                                                      TRUE);
  apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] = LPA_MMGSDI_GET_CHALLENGE_INS; /* INS */
  apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET]     = 0x00; /* P1 */
  apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]     = 0x00; /* P2 */
  apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET]  = 0x10; /* Le... I am requesting for
                                                    a random number that is
                                                    16 bytes long */

  mmgsdi_status = mmgsdi_send_apdu_ext(lpa_mmgsdi_client_info.client_id,
                                       mmgsdi_slot_id,
                                       apdu_data,
                                       MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                       lpa_mmgsdi_cmd_rsp_cb,
                                       (mmgsdi_client_data_type) 0);

  LPA_FREE(apdu_data.data_ptr);
  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_get_challenge */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_GET_DATA

   DESCRIPTION:
     Utility called to send GET DATA command to the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_get_data (
  lpa_slot_id_enum_type         slot_id,
  uint8                         p2,
  lpa_data_type                 data,
  lpa_mmgsdi_callback_type      callback,
  void                          *user_data_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_send_apdu_data_type   apdu_data;
  mmgsdi_slot_id_enum_type     mmgsdi_slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  if(LPA_SUCCESS != lpa_util_convert_lpa_slot_to_mmgsdi_slot(slot_id, &mmgsdi_slot_id) ||
     data.data_len > UIM_MAX_TRANS_SIZE_OUT)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_mmgsdi_cmd_info[slot_id].apdu_req_type       = LPA_MMGSDI_APDU_GET_DATA;
  lpa_mmgsdi_cmd_info[slot_id].callback            = callback;
  lpa_mmgsdi_cmd_info[slot_id].user_data_ptr       = user_data_ptr;

  LPA_MSG_HIGH_1("LPA MMGSDI Get Data request slot_id:0x%x", slot_id);

  if(!lpa_mmgsdi_client_info.session_id[slot_id])
  {
    /* Open session and select ISDR if not done already */
    lpa_mmgsdi_cmd_info[slot_id].p2 = p2;
    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr = LPA_MALLOC(data.data_len);
    if (lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr == NULL)
    {
      return LPA_ERROR_HEAP_EXHAUSTED;
    }

    lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len = data.data_len;
    memscpy(lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_ptr,
            lpa_mmgsdi_cmd_info[slot_id].remaining_send_apdu_data.data_len,
            data.data_ptr,
            data.data_len);
    return lpa_mmgsdi_select_isdr(slot_id);
  }

  /* Create a GET DATA APDU and sent it to MMGSDI */
  memset(&apdu_data, 0x00, sizeof(apdu_data));

  /* The APDU is atleast 5 bytes long: INS+CLA+P1+P2+Le*/
  apdu_data.data_len = UIM_CASE2_7816_APDU_SIZE;
  if(data.data_len > 0 &&
     data.data_ptr != NULL)
  {
    /* There is non-zero Data that needs to be sent as part of the APDU.
       The "1" here is for the Lc byte itself */
    apdu_data.data_len += (1 + data.data_len);
  }

  apdu_data.data_ptr = (uint8*)LPA_MALLOC(apdu_data.data_len);

  if (apdu_data.data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET]  = lpa_mmgsdi_channel_to_cla_byte(
                                                      lpa_mmgsdi_client_info.channel_id[slot_id],
                                                      FALSE);
  apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] = LPA_MMGSDI_GET_DATA_INS; /* INS */
  apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET]     = 0xBF;                    /* P1 */
  apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]     = p2;                      /* P2 */

  if(data.data_len > 0 &&
     data.data_ptr != NULL)
  {
    apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET] = (uint8) data.data_len;
    memscpy(&apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1],
            (apdu_data.data_len - UIM_CASE3_4_7816_LC_OFFSET),
            data.data_ptr,
            data.data_len);
    apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1 + data.data_len] = 0x00; /* Le */
  }
  else
  {
    apdu_data.data_ptr[UIM_CASE3_4_7816_LC_OFFSET] = 0x00;                    /* Le */
  }

  mmgsdi_status = mmgsdi_send_apdu_ext(lpa_mmgsdi_client_info.client_id,
                                       mmgsdi_slot_id,
                                       apdu_data,
                                       MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                       lpa_mmgsdi_cmd_rsp_cb,
                                       (mmgsdi_client_data_type) 0);

  LPA_FREE(apdu_data.data_ptr);
  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_get_data */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_STATUS

   DESCRIPTION:
     API creates and sends a STATUS APDU to the card on the default channel.
     It does not allow the caller to provide a callback; it just attempts
     sending the STATUS to the card and ignores the response.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_status (
  lpa_slot_id_enum_type         slot_id
)
{
  mmgsdi_return_enum_type      mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type     mmgsdi_slot_id  = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_send_apdu_data_type   apdu_data;
  uint8                        status_apdu_data[LPA_MMGSDI_STATUS_SEND_APDU_LEN]
                                               = {0};

  if(LPA_SUCCESS != lpa_util_convert_lpa_slot_to_mmgsdi_slot(slot_id, &mmgsdi_slot_id))
  {
    return LPA_GENERIC_ERROR;
  }

  memset(&apdu_data, 0x00, sizeof(apdu_data));
  apdu_data.data_len = sizeof(status_apdu_data);
  apdu_data.data_ptr = status_apdu_data;

  apdu_data.data_ptr[0] = 0x80; /* Default channel */
  apdu_data.data_ptr[1] = LPA_MMGSDI_STATUS_INS; /* INS */
  apdu_data.data_ptr[2] = 0x00; /* P1.. no indication */
  apdu_data.data_ptr[3] = 0x0C; /* P2.. no data returned */
  apdu_data.data_ptr[4] = 0x00; /* Le */

  /* We could have used mmgsdi_session_send_card_status() here
     but since that API does not allow the user to enforce the
     sending of the APDU to a  particular (DEFAULT in this case)
     channel, we are using the send apdu approach */
  mmgsdi_status = mmgsdi_send_apdu_ext(lpa_mmgsdi_client_info.client_id,
                                       mmgsdi_slot_id,
                                       apdu_data,
                                       MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                       lpa_mmgsdi_cmd_rsp_dummy_cb,
                                       (mmgsdi_client_data_type) 0);

  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_status */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_COMMAND_CLEANUP

   DESCRIPTION:
     LPA API to cleanup current command related global

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_command_cleanup(
  lpa_slot_id_enum_type                  lpa_slot
)
{
  if(lpa_slot >= LPA_NUM_SLOTS)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  if(lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_len > 0 &&
     lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_ptr)
  {
    LPA_FREE(lpa_mmgsdi_cmd_info[lpa_slot].remaining_send_apdu_data.data_ptr);
  }
  memset(&lpa_mmgsdi_cmd_info[lpa_slot], 0x00, sizeof(lpa_mmgsdi_cmd_info[lpa_slot]));

  return LPA_SUCCESS;
} /* lpa_mmgsdi_command_cleanup */


/*===========================================================================
   FUNCTION:      LPA_MMGSDI_RESET

   DESCRIPTION:
     LPA API to cleanup MMGSDI context

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_mmgsdi_reset(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  lpa_slot_id_enum_type   lpa_slot      = LPA_NUM_SLOTS;

  for(lpa_slot = LPA_SLOT_1; lpa_slot < LPA_NUM_SLOTS; lpa_slot ++)
  {
    /* Close the non prov session to ISDR */
    if (lpa_mmgsdi_client_info.session_id[lpa_slot])
    {
      mmgsdi_status = mmgsdi_session_close(lpa_mmgsdi_client_info.session_id[lpa_slot],
                                           lpa_mmgsdi_cmd_rsp_cb,0);
    }

    lpa_mmgsdi_command_cleanup(lpa_slot);
  }
  lpa_mmgsdi_client_info.client_id = MMGSDI_CLIENT_ID_ZERO;

  return lpa_util_convert_mmgsdi_status_to_lpa_status(mmgsdi_status);
} /* lpa_mmgsdi_reset */
