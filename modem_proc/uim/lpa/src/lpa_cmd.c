/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   C O M M A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the LPA functions which handles LPA profile
  management requests coming from the user.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_cmd.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/16   ar      Introduce DS http post retry timer logic
08/03/16   av      Send STATUS to trigger REFRESH
04/28/16   av      Move LPA task init to same group as qmiuim
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "queue.h"

#include "lpa.h"
#include "lpa_platform.h"
#include "lpa_cmd.h"
#include "lpalib.h"
#include "lpa_state.h"
#include "lpa_mmgsdi.h"
#include "lpa_util.h"
#include "lpa_lpd.h"
#include "lpa_asn1_encoder.h"
#include "lpa_asn1_decoder.h"
#include "lpa_timer.h"
#include <stringl/stringl.h>
#include "lpa_efs.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define LPA_CMD_QUEUE_MAX_SIZE                  10
#define LPA_ENABLE_PROFILE_APDU_P1            0x88
#define LPA_ENABLE_PROFILE_APDU_DGI_0         0x3A
#define LPA_ENABLE_PROFILE_APDU_DGI_1         0x03
#define LPA_DISABLE_PROFILE_APDU_P1           0x88
#define LPA_DISABLE_PROFILE_APDU_DGI_0        0x3A
#define LPA_DISABLE_PROFILE_APDU_DGI_1        0x04
#define LPA_ADD_OR_UPDATE_NICKNAME_APDU_P1    0x90
#define LPA_CARD_MEMORY_RESET_APDU_P1         0x88
#define LPA_CARD_MEMORY_RESET_APDU_DGI_0      0x3A
#define LPA_CARD_MEMORY_RESET_APDU_DGI_1      0x09
#define LPA_CARD_MEMORY_RESET_APDU_DGI_LEN    0x03
#define LPA_CARD_MEMORY_RESET_APDU_TAG        0x82
#define LPA_CARD_MEMORY_RESET_APDU_LEN        0x01
#define LPA_CARD_MEMORY_RESET_APDU_VAL        0x01
#define LPA_ICCID_STORE_DATA_TAG              0x5A
#define LPA_DISABLE_PROFILE_APDU_DGI_LEN      0x0C
#define LPA_ENABLE_PROFILE_APDU_DGI_LEN       0x0C
#define LPA_GET_EID_APDU_DATA_P2              0x30
#define LPA_GET_EID_APDU_DATA_BYTE_1          0x5C
#define LPA_GET_EID_APDU_DATA_BYTE_2          0x01
#define LPA_GET_EID_APDU_DATA_BYTE_3          0x5A

#define LPA_DELETE_PROFILE_APDU_RESP_LEN      0x01
#define LPA_DELETE_PROFILE_APDU_RESP          0x00
#define LPA_GET_EID_APDU_RESP_LEN             0x15
#define LPA_GET_EID_APDU_RESP_DGI_0           0xBF
#define LPA_GET_EID_APDU_RESP_DGI_1           0x30
#define LPA_GET_EID_APDU_RESP_DGI_LEN         0x12
#define LPA_EID_APDU_TAG                      0x5A


static q_type                                 lpa_cmd_q[LPA_NUM_SLOTS];
static lpa_slot_id_enum_type                  lpa_last_cmd_q_processed = LPA_SLOT_2;

/*=============================================================================

                       FUNCTIONS

=============================================================================*/

/*===========================================================================
   FUNCTION:      LPA_UTIL_FREE_TASK_CMD_PTR

   DESCRIPTION:
     Utility to free LPA task cmd pointer

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
static void lpa_cmd_free_task_cmd_ptr(
  lpa_cmd_type                *lpa_cmd_ptr
)
{
  if(lpa_cmd_ptr == NULL)
  {
    return;
  }

  switch(lpa_cmd_ptr->cmd.cmd_enum)
  {
    case LPA_ENABLE_PROFILE_MSG:
    case LPA_DISABLE_PROFILE_MSG:
    case LPA_DELETE_PROFILE_MSG:
    case LPA_CARD_MEMORY_RESET_MSG:
    case LPA_GET_PROFILES_INFO_MSG:
    case LPA_GET_EID_MSG:
    case LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_MSG:
       /* Nothing to free */
       break;
    case LPA_ADD_PROFILE_MSG:
       LPA_FREE(lpa_cmd_ptr->cmd.cmd.add_profile_req.activation_code.data_ptr);
       LPA_FREE(lpa_cmd_ptr->cmd.cmd.add_profile_req.confirmation_code.data_ptr);
       break;
    default:
       break;
  }
} /* lpa_cmd_free_task_cmd_ptr */


/*===========================================================================
FUNCTION LPA_CMD_INIT

DESCRIPTION
  This function, called in lpa_main, is called to initialize the command
  module.

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_init (
  void
)
{
  (void) q_init(&lpa_cmd_q[LPA_SLOT_1]);
  (void) q_init(&lpa_cmd_q[LPA_SLOT_2]);
} /* lpa_cmd_init */


/*===========================================================================
   FUNCTION:      LPA_CMD_MMGSDI_CALLBACK

   DESCRIPTION:
     This is a callback function called by LPA_MMGSDI module used by it
     to report response for the MMGSDI bound request sent by LPA.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_cmd_mmgsdi_cb (
  lpa_result_enum_type                             status,
  const lpa_mmgsdi_resp_data_type                 *mmgsdi_resp_ptr,
  const void                                      *user_data_ptr
)
{
  lpa_response_data_type                        *resp_ptr   = NULL;
  uint32                                         offset     = 0;

  if(mmgsdi_resp_ptr == NULL)
  {
    return;
  }

  resp_ptr = (lpa_response_data_type*) LPA_MALLOC(sizeof(lpa_response_data_type));
  if(resp_ptr == NULL)
  {
    return;
  }

  resp_ptr->msg_type = (lpa_message_enum_type)user_data_ptr;

  if(status == LPA_SUCCESS)
  {
    /* If we are here because of ENABLE/DISABLE/DELETE/EUICCMEMORYRESET
       command respose from the eUICC, lets send a STATUS on the default
       channel to ensure proactive eUICC is able to trigger a refresh
       right away if possible. We do not care much about the result of
       the operation */
    if(resp_ptr->msg_type == LPA_ENABLE_PROFILE_MSG ||
       resp_ptr->msg_type == LPA_DISABLE_PROFILE_MSG ||
       resp_ptr->msg_type == LPA_DELETE_PROFILE_MSG ||
       resp_ptr->msg_type == LPA_CARD_MEMORY_RESET_MSG)
    {
      (void) lpa_mmgsdi_status(mmgsdi_resp_ptr->slot_id);
    }

    switch(mmgsdi_resp_ptr->cmd_type)
    {
      case LPA_MMGSDI_APDU_STORE_DATA:
        /* STORE DATA is sent for multiple operations EnableProfile,
           DisableProfile, AddOrUpdateProfileNickname... nothing
           expected in the response.. so, simply call the client
           callback */
        break;
      case LPA_MMGSDI_APDU_DELETE:
        /* For DELETE PROFILE, we need to do some validation of the
           response received from the card - we must receive only one
           byte in the data, and it shoould be 0x00 */
        if(mmgsdi_resp_ptr->data_ptr == NULL ||
           mmgsdi_resp_ptr->data_len != LPA_DELETE_PROFILE_APDU_RESP_LEN ||
           mmgsdi_resp_ptr->data_ptr[0] != LPA_DELETE_PROFILE_APDU_RESP)
        {
          /* According to SGP22, for DELETE APDU, the card returns one byte as
             0x00 to indicate no more data is present. If we didn't receive
             0x00 data from the card, we treat the command as failure */
          status = LPA_GENERIC_ERROR;
        }
        break;
      case LPA_MMGSDI_APDU_GET_STATUS:
        /* This response is for the GET_STATUS APDU sent for GetProfilesInfo.
           Lets decode the response, this is in ASN1 format. The following
           decoder could malloc Icon (which is the only pointer inside get
           profiles info rsp) */
        status = lpa_asn1_decode_get_profiles_info_apdu(mmgsdi_resp_ptr->data_len,
                                                        mmgsdi_resp_ptr->data_ptr,
                                                        resp_ptr);
        break;
      case LPA_MMGSDI_APDU_GET_DATA:
        /* This response is for GET_DATA APDU sent for GetEID. Lets validate
           the response and call the client callback */
        if(mmgsdi_resp_ptr->data_len != LPA_GET_EID_APDU_RESP_LEN ||
           mmgsdi_resp_ptr->data_ptr == NULL ||
           mmgsdi_resp_ptr->data_ptr[offset++] != LPA_GET_EID_APDU_RESP_DGI_0 ||
           mmgsdi_resp_ptr->data_ptr[offset++] != LPA_GET_EID_APDU_RESP_DGI_1 ||
           mmgsdi_resp_ptr->data_ptr[offset++] != LPA_GET_EID_APDU_RESP_DGI_LEN ||
           mmgsdi_resp_ptr->data_ptr[offset++] != LPA_EID_APDU_TAG ||
           mmgsdi_resp_ptr->data_ptr[offset++] != LPA_EID_LEN)
        {
          status = LPA_GENERIC_ERROR;
          break;
        }
        memscpy(resp_ptr->message.get_eid_resp.eid,
                sizeof(resp_ptr->message.get_eid_resp.eid),
                &mmgsdi_resp_ptr->data_ptr[offset],
                LPA_EID_LEN);
        break;
      default:
        status = LPA_GENERIC_ERROR;
        break;
    }
  }

  lpa_send_response(status,
                    resp_ptr,
                    mmgsdi_resp_ptr->slot_id,
                    TRUE);

  /* Free everything inside response_ptr */
  lpa_util_free_lpa_response_ptr(resp_ptr);
  LPA_FREE(resp_ptr);
} /* lpa_cmd_mmgsdi_cb */


/*===========================================================================
   FUNCTION:      LPA_CMD_QUEUE_CMD

   DESCRIPTION:
     This function allows client to queue a LPA command to be processed
     in LPA task context.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type  lpa_cmd_queue_cmd (
  lpa_cmd_type                          *lpa_cmd_ptr,
  lpa_slot_id_enum_type                  lpa_slot
)
{
  lpa_result_enum_type  lpa_status  = LPA_SUCCESS;

  if(lpa_cmd_ptr == NULL ||
     lpa_slot >= LPA_NUM_SLOTS)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  LPA_UTIL_ENTER_TASK_STATE_CRIT_SECT;

  if(!lpa_task_stopped)
  {
    if(q_cnt(&lpa_cmd_q[lpa_slot]) < LPA_CMD_QUEUE_MAX_SIZE)
    {
      (void)q_link(lpa_cmd_ptr, &lpa_cmd_ptr->link);

      /* Put the message on the queue */
      q_put(&lpa_cmd_q[lpa_slot], &lpa_cmd_ptr->link);

      if(UIM_LPA_TCB)
      {
        /* Set the command queue signal */
        (void) rex_set_sigs(UIM_LPA_TCB, LPA_CMD_SIG);
      }
    }
    else
    {
      lpa_status = LPA_ERROR_CMD_QUEUE_FULL;
    }
  }
  else
  {
    LPA_MSG_ERR_0("LPA task stopped, cannot queue command");
    lpa_status = LPA_GENERIC_ERROR;
  }

  LPA_UTIL_LEAVE_TASK_STATE_CRIT_SECT;
  return lpa_status;
} /* lpa_cmd_queue_cmd */


/*===========================================================================
FUNCTION LPA_CMD_CLEANUP

DESCRIPTION
  This function flushes the lpa cmd queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_cleanup (
  lpa_slot_id_enum_type                  lpa_slot
)
{
  lpa_cmd_type  *task_cmd_ptr = NULL;

  if(lpa_slot >= LPA_NUM_SLOTS)
  {
    return;
  }

  while(q_cnt(&lpa_cmd_q[lpa_slot]) != 0)
  {
    task_cmd_ptr = (lpa_cmd_type*)(q_get(&lpa_cmd_q[lpa_slot]));

    if (task_cmd_ptr != NULL)
    {
      if (task_cmd_ptr->lpa_cb_ptr != NULL)
      {
        LPA_MSG_LOW_0("Notify error to client");
        task_cmd_ptr->lpa_cb_ptr(LPA_GENERIC_ERROR,
                                 NULL,
                                 task_cmd_ptr->user_data_ptr);
      }

      LPA_FREE(task_cmd_ptr);
    }
  }
} /* lpa_cmd_cleanup */


/*===========================================================================
FUNCTION LPA_CMD_SET_SIG

DESCRIPTION
  This function sets the LPA command signal.

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_set_sig (
)
{
  if((q_cnt(&lpa_cmd_q[LPA_SLOT_1]) > 0 &&
      lpa_state_get_current(LPA_SLOT_1) == LPA_STATE_IDLE) ||
     (q_cnt(&lpa_cmd_q[LPA_SLOT_2]) > 0 &&
      lpa_state_get_current(LPA_SLOT_2) == LPA_STATE_IDLE))
  {
    /* Item present on atleast one of the two queues that is in
       IDLE state, that is, ready to process the command. Set the signal */
    (void) rex_set_sigs(UIM_LPA_TCB, LPA_CMD_SIG);
  }
} /* lpa_cmd_set_sig */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_GET_PROFILES_INFO_REQ

DESCRIPTION
  Function processes the get profiles info request. It returns all the
  profiles on the card.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_get_profiles_info_req (
  lpa_slot_id_enum_type         slot_id
)
{
  lpa_result_enum_type          lpa_status = LPA_SUCCESS;

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* Send GET STATUS to the card in order to retrive profiles info */
  return lpa_mmgsdi_get_status(slot_id,
                               lpa_cmd_mmgsdi_cb,
                               (void*) LPA_GET_PROFILES_INFO_MSG);
} /* lpa_cmd_process_get_profiles_info_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_ENABLE_PROFILE_REQ

DESCRIPTION
  Function processes the enable profile request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_enable_profile_req (
  lpa_slot_id_enum_type                        slot_id,
  const lpa_enable_profile_req_type           *enable_profile_req_ptr
)
{
  lpa_result_enum_type    lpa_status = LPA_SUCCESS;
  lpa_data_type           data;
  /* Data field of the STORE DATA APDU for enable_profile request is 15 bytes
     long - initial 5 bytes for Tags/Lengths, and remaining 10 bytes for ICCID */
  uint8                   enable_profile_apdu_data[5 + LPA_ICCID_LEN];

  if(enable_profile_req_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  memset(enable_profile_apdu_data, 0x00, sizeof(enable_profile_apdu_data));

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  if(!lpa_util_is_iccid_valid(enable_profile_req_ptr->iccid))
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  /* Malloc and create the Data buffer that needs to be sent with STORE DATA
     SEND APDU command to the card */
  memset(&data, 0x00, sizeof(data));
  data.data_len = sizeof(enable_profile_apdu_data);
  data.data_ptr = enable_profile_apdu_data;

  data.data_ptr[0] = LPA_ENABLE_PROFILE_APDU_DGI_0;    /* Data[0] */
  data.data_ptr[1] = LPA_ENABLE_PROFILE_APDU_DGI_1;    /* Data[1] */
  data.data_ptr[2] = LPA_ENABLE_PROFILE_APDU_DGI_LEN;  /* Data[2] */
  data.data_ptr[3] = LPA_ICCID_STORE_DATA_TAG;         /* Data[3] */
  data.data_ptr[4] = LPA_ICCID_LEN;                    /* Data[2] */
  memscpy(&data.data_ptr[5],
          LPA_ICCID_LEN,
          enable_profile_req_ptr->iccid.data,
          enable_profile_req_ptr->iccid.data_len);     /* Data[5] onwards contains ICCID */

  /* Send STORE DATA apdu to the card for enabling profile */
  lpa_status = lpa_mmgsdi_store_data(slot_id,
                                     LPA_ENABLE_PROFILE_APDU_P1,
                                     data,
                                     FALSE,
                                     lpa_cmd_mmgsdi_cb,
                                     (void*) LPA_ENABLE_PROFILE_MSG);

  return lpa_status;
} /* lpa_cmd_process_enable_profile_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_DISABLE_PROFILE_REQ

DESCRIPTION
  Function processes the disable profile request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_disable_profile_req (
  lpa_slot_id_enum_type                       slot_id,
  const lpa_disable_profile_req_type         *disable_profile_req_ptr
)
{
  lpa_result_enum_type    lpa_status = LPA_SUCCESS;
  lpa_data_type           data;
  /* Data field of the STORE DATA APDU for disable_profile request is 15 bytes
     long - initial 5 bytes for Tags/Lengths, and remaining 10 bytes for ICCID */
  uint8                   disable_profile_apdu_data[5 + LPA_ICCID_LEN];

  if(disable_profile_req_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  memset(disable_profile_apdu_data, 0x00, sizeof(disable_profile_apdu_data));

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  if(!lpa_util_is_iccid_valid(disable_profile_req_ptr->iccid))
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  /* Malloc and create the Data buffer that needs to be sent with STORE DATA
     SEND APDU command to the card */
  memset(&data, 0x00, sizeof(data));
  data.data_len = sizeof(disable_profile_apdu_data);
  data.data_ptr = disable_profile_apdu_data;

  data.data_ptr[0] = LPA_DISABLE_PROFILE_APDU_DGI_0;   /* Data[0] */
  data.data_ptr[1] = LPA_DISABLE_PROFILE_APDU_DGI_1;   /* Data[1] */
  data.data_ptr[2] = LPA_DISABLE_PROFILE_APDU_DGI_LEN; /* Data[2] */
  data.data_ptr[3] = LPA_ICCID_STORE_DATA_TAG;         /* Data[3] */
  data.data_ptr[4] = LPA_ICCID_LEN;                    /* Data[2] */
  memscpy(&data.data_ptr[5],
          LPA_ICCID_LEN,
          disable_profile_req_ptr->iccid.data,
          disable_profile_req_ptr->iccid.data_len);    /* Data[5] onwards contains ICCID */

  /* Send STORE DATA apdu to the card for enabling profile */
  lpa_status = lpa_mmgsdi_store_data(slot_id,
                                     LPA_DISABLE_PROFILE_APDU_P1,
                                     data,
                                     FALSE,
                                     lpa_cmd_mmgsdi_cb,
                                     (void*) LPA_DISABLE_PROFILE_MSG);

  return lpa_status;
} /* lpa_cmd_process_disable_profile_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_RESET_CARD_MEMORY_REQ

DESCRIPTION
  Function processes the reset card memory equest.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_reset_card_memory_req (
  lpa_slot_id_enum_type                 slot_id
)
{
  lpa_result_enum_type    lpa_status = LPA_SUCCESS;
  lpa_data_type           data;
  /* Data field of the STORE DATA APDU for reset_card_memory request is 6 bytes
     long - for the TLVs are defined in SGP.22 */
  uint8                   reset_card_memory_apdu_data[6];

  memset(reset_card_memory_apdu_data, 0x00, sizeof(reset_card_memory_apdu_data));

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* Create the Data buffer that needs to be sent with STORE DATA SEND APDU
     command to the card */
  memset(&data, 0x00, sizeof(data));
  data.data_len = sizeof(reset_card_memory_apdu_data);
  data.data_ptr = reset_card_memory_apdu_data;

  data.data_ptr[0] = LPA_CARD_MEMORY_RESET_APDU_DGI_0;   /* Data[0].. DGI Tag */
  data.data_ptr[1] = LPA_CARD_MEMORY_RESET_APDU_DGI_1;   /* Data[1].. DGI Tag */
  data.data_ptr[2] = LPA_CARD_MEMORY_RESET_APDU_DGI_LEN; /* Data[2].. DGI Tag Len */
  data.data_ptr[3] = LPA_CARD_MEMORY_RESET_APDU_TAG;     /* Data[3].. eUICCMemoryReset Tag */
  data.data_ptr[4] = LPA_CARD_MEMORY_RESET_APDU_LEN;     /* Data[4].. eUICCMemoryReset Len */
  data.data_ptr[5] = LPA_CARD_MEMORY_RESET_APDU_VAL;     /* Data[5].. eUICCMemoryReset Value
                                                            (= Delete all profiles) */

  /* Send STORE DATA apdu to the card for euicc reset */
  lpa_status = lpa_mmgsdi_store_data(slot_id,
                                     LPA_CARD_MEMORY_RESET_APDU_P1,
                                     data,
                                     FALSE,
                                     lpa_cmd_mmgsdi_cb,
                                     (void*) LPA_CARD_MEMORY_RESET_MSG);

  return lpa_status;
} /* lpa_cmd_process_reset_card_memory_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_DELETE_PROFILE_REQ

DESCRIPTION
  Function processes the DELETE PROFILE request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_delete_profile_req (
  lpa_slot_id_enum_type                       slot_id,
  const lpa_delete_profile_req_type          *delete_profile_req_ptr
)
{
  lpa_result_enum_type    lpa_status = LPA_SUCCESS;

  if(delete_profile_req_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* Send GET DATA apdu to the card for retrieving EID */
  return lpa_mmgsdi_delete(slot_id,
                           delete_profile_req_ptr->iccid,
                           lpa_cmd_mmgsdi_cb,
                           (void*) LPA_DELETE_PROFILE_MSG);
} /* lpa_cmd_process_delete_profile_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_GET_EID_REQ

DESCRIPTION
  Function processes the get eid request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_get_eid_req (
  lpa_slot_id_enum_type         slot_id
)
{
  lpa_result_enum_type    lpa_status = LPA_SUCCESS;
  lpa_data_type           data;
  /* Data field of the GET DATA APDU for get_eid request is 3 bytes
     long - for the TLVs are defined in SGP.22 */
  uint8                   get_eid_apdu_data[3];

  memset(get_eid_apdu_data, 0x00, sizeof(get_eid_apdu_data));

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* Create the Data buffer that needs to be sent with GET DATA SEND APDU
     command to the card */
  memset(&data, 0x00, sizeof(data));
  data.data_len = sizeof(get_eid_apdu_data);
  data.data_ptr = get_eid_apdu_data;

  data.data_ptr[0] = LPA_GET_EID_APDU_DATA_BYTE_1;   /* Data[0] */
  data.data_ptr[1] = LPA_GET_EID_APDU_DATA_BYTE_2;   /* Data[1] */
  data.data_ptr[2] = LPA_GET_EID_APDU_DATA_BYTE_3;   /* Data[2] */

  /* Send GET DATA apdu to the card for retrieving EID */
  return lpa_mmgsdi_get_data(slot_id,
                             LPA_GET_EID_APDU_DATA_P2,
                             data,
                             lpa_cmd_mmgsdi_cb,
                             (void*) LPA_GET_EID_MSG);
} /* lpa_cmd_process_get_eid_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_ADD_PROFILE_REQ

DESCRIPTION
  Function processes the add profile request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_add_profile_req (
  lpa_slot_id_enum_type                    slot_id,
  const lpa_add_profile_req_type          *add_profile_req_ptr
)
{
  return lpa_lpd_start_add_profile_procedure(
           add_profile_req_ptr,
           slot_id);
} /* lpa_cmd_process_add_profile_req */


/*===========================================================================
FUNCTION LPA_CMD_PROCESS_ADD_OR_UPDATE_PROFILE_NICKNAME_REQ

DESCRIPTION
  Function processes the add or update profile nickname request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_cmd_process_add_or_update_profile_nickname_req (
  lpa_slot_id_enum_type                              slot_id,
  const lpa_add_or_update_profile_nickname_req_type *add_or_update_profile_nickname_req_ptr
)
{
  lpa_result_enum_type    lpa_status = LPA_SUCCESS;
  lpa_data_type           data;
  lpa_asn1_set_nickname_request_type  nickname;

  memset(&nickname, 0x00, sizeof(nickname));

  if(add_or_update_profile_nickname_req_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  nickname.iccid =  add_or_update_profile_nickname_req_ptr->iccid;
  nickname.profile_nickname =  add_or_update_profile_nickname_req_ptr->nickname;

  /* This encoder might malloc the data.data_ptr if the nickname is
     non null. Make sure to free it when applicable */
  lpa_asn1_encode_set_nickname_request(nickname, &data);

    /* Write raw activation code to the file */
  (void) lpa_efs_write(LPA_FILE_ASN1_ENCODED_NICKNAME,
                       data.data_len,
                       data.data_ptr,
                       slot_id);

  /* Send STORE DATA apdu to the card for euicc reset */
  lpa_status = lpa_mmgsdi_store_data(slot_id,
                                     LPA_ADD_OR_UPDATE_NICKNAME_APDU_P1,
                                     data,
                                     FALSE,
                                     lpa_cmd_mmgsdi_cb,
                                     (void*) LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_MSG);
  LPA_FREE(data.data_ptr);

  return lpa_status;
} /* lpa_cmd_process_add_or_update_profile_nickname_req */


/*===========================================================================
FUNCTION LPA_CMD_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, is called to process the LPA command

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_cmd_handle_cmd_sig (
  void
)
{
  lpa_cmd_type                 *lpa_cmd_ptr              = NULL;
  lpa_result_enum_type          lpa_status               = LPA_GENERIC_ERROR;
  boolean                       lpa_process_slot_1_cmd_q = FALSE;
  boolean                       lpa_process_slot_2_cmd_q = FALSE;

  (void) rex_clr_sigs(rex_self(), LPA_CMD_SIG);

  /* If LPA is not in a valid state to handle a new command then
     leave command in queue and continue processing. In order to
     make sure this 'ignored' command is dequeued when the right
     time arrives, LPA will set the cmd signal once the current
     command finishes processing (that is, when the LPA goes back
     to IDLE state in which it can process a new command). */
  /* Alternate between SLOT1 and SLOT2 queues. */
  if(lpa_state_get_current(LPA_SLOT_1) == LPA_STATE_IDLE &&
     q_cnt(&lpa_cmd_q[LPA_SLOT_1]) > 0)
  {
    lpa_process_slot_1_cmd_q = TRUE;
  }
  if(lpa_state_get_current(LPA_SLOT_2) == LPA_STATE_IDLE &&
     q_cnt(&lpa_cmd_q[LPA_SLOT_2]) > 0)
  {
    lpa_process_slot_2_cmd_q = TRUE;
  }

  /* If there are commands waiting in both the queues, see what was the last
     queue from which the command was processed. This time we should
     alternate to the other queue */
  if(((lpa_process_slot_1_cmd_q == TRUE &&
       lpa_process_slot_2_cmd_q == TRUE) &&
      lpa_last_cmd_q_processed == LPA_SLOT_2)||
     (lpa_process_slot_1_cmd_q == TRUE &&
      lpa_process_slot_2_cmd_q == FALSE))
  {
    lpa_cmd_ptr = (lpa_cmd_type*)(q_get(&lpa_cmd_q[LPA_SLOT_1]));
    lpa_last_cmd_q_processed = LPA_SLOT_1;
  }
  else if(((lpa_process_slot_1_cmd_q == TRUE &&
            lpa_process_slot_2_cmd_q == TRUE) &&
           lpa_last_cmd_q_processed == LPA_SLOT_1)||
          (lpa_process_slot_1_cmd_q == FALSE &&
           lpa_process_slot_2_cmd_q == TRUE))
  {
    lpa_cmd_ptr = (lpa_cmd_type*)(q_get(&lpa_cmd_q[LPA_SLOT_2]));
    lpa_last_cmd_q_processed = LPA_SLOT_2;
  }

  /* If there is a command on either of the queues */
  if(lpa_cmd_ptr == NULL)
  {
    return;
  }

  LPA_MSG_HIGH_1("LPA_CMD_SIG received for command: 0x%x",
                 lpa_cmd_ptr->cmd.cmd_enum);

  /* Start overall sanity timer */
  lpa_timer_set(lpa_cmd_ptr->slot_id, LPA_TIMER_TYPE_SANITY_TIMER);

  /* Save client response callback etc to a global that can later be
     used to send the response to the caller */
  lpa_status = lpa_save_client_data(lpa_cmd_ptr->lpa_cb_ptr,
                                    lpa_cmd_ptr->event_cb_ptr,
                                    lpa_cmd_ptr->user_data_ptr,
                                    lpa_cmd_ptr->slot_id);
  if(lpa_status == LPA_SUCCESS)
  {
    switch (lpa_cmd_ptr->cmd.cmd_enum)
    {
      case LPA_GET_PROFILES_INFO_MSG:
        lpa_status = lpa_cmd_process_get_profiles_info_req(
                       lpa_cmd_ptr->slot_id);
        break;
      case LPA_ENABLE_PROFILE_MSG:
        lpa_status = lpa_cmd_process_enable_profile_req(
                       lpa_cmd_ptr->slot_id,
                       &lpa_cmd_ptr->cmd.cmd.enable_profile_req);
        break;
      case LPA_DISABLE_PROFILE_MSG:
        lpa_status = lpa_cmd_process_disable_profile_req(
                       lpa_cmd_ptr->slot_id,
                       &lpa_cmd_ptr->cmd.cmd.disable_profile_req);
        break;
      case LPA_DELETE_PROFILE_MSG:
        lpa_status = lpa_cmd_process_delete_profile_req(
                       lpa_cmd_ptr->slot_id,
                       &lpa_cmd_ptr->cmd.cmd.delete_profile_req);
        break;
      case LPA_CARD_MEMORY_RESET_MSG:
        lpa_status = lpa_cmd_process_reset_card_memory_req(
                       lpa_cmd_ptr->slot_id);
        break;
      case LPA_GET_EID_MSG:
        lpa_status = lpa_cmd_process_get_eid_req(
                       lpa_cmd_ptr->slot_id);
        break;
      case LPA_ADD_PROFILE_MSG:
        lpa_status = lpa_cmd_process_add_profile_req(
                       lpa_cmd_ptr->slot_id,
                       &lpa_cmd_ptr->cmd.cmd.add_profile_req);
        break;
      case LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_MSG:
        lpa_status = lpa_cmd_process_add_or_update_profile_nickname_req(
                       lpa_cmd_ptr->slot_id,
                       &lpa_cmd_ptr->cmd.cmd.add_or_update_profile_nickname_req);
        break;
      default:
        lpa_status = LPA_GENERIC_ERROR;
        break;
    }
  }

  if (lpa_status != LPA_SUCCESS)
  {
    /* Send the response and clear the timer */
    lpa_response_data_type            *resp_ptr = NULL;

    resp_ptr = (lpa_response_data_type*) LPA_MALLOC(sizeof(lpa_response_data_type));
    if(resp_ptr != NULL)
    {
      resp_ptr->msg_type = lpa_cmd_ptr->cmd.cmd_enum;
    }
    lpa_send_response(lpa_status,
                      resp_ptr,
                      lpa_cmd_ptr->slot_id,
                      TRUE);
    LPA_FREE(resp_ptr);
  }
  lpa_cmd_free_task_cmd_ptr(lpa_cmd_ptr);
  LPA_FREE(lpa_cmd_ptr);

  /* Set the signal if there are one or more commands still in either of the
     two queues and LPA is currently in IDLE state for that queue/slot. */
  lpa_cmd_set_sig();
} /* lpa_cmd_handle_cmd_sig */
