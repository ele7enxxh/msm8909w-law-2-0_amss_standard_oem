/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   M A N A G E   C H A N N E L   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of converting Manage Channel
  streaming APDU to MMGSDI command

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_manage_channel.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/15   ar      Convert MANAGE CHANNEL APDU to higher level API for ICC card
05/29/15   hh      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdi.h"
#include "mmgsdi_manage_channel.h"
#include "mmgsdiutil.h"
#include "mmgsdi_gen.h"


/*===========================================================================
                       D E F I N E S
============================================================================*/
#define MMGSDI_MANAGE_CHANNEL_P1_OPEN   0x00
#define MMGSDI_MANAGE_CHANNEL_P1_CLOSE  0x80


/*===========================================================================
                       F U N C T I O N S
============================================================================*/

/* ============================================================================
  FUNCTION:      MMGSDI_APDU_MANAGE_CHANNEL_CB

  DESCRIPTION:
    Callback function for MANAGE CHANNEL APDU request. This function
    populates the send_apdu_ext_cnf and calls the client callback.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
static void mmgsdi_apdu_manage_channel_cb
(
  mmgsdi_return_enum_type    mmgsdi_status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type    * cnf_ptr
)
{
  mmgsdi_cnf_type                *client_cnf_ptr       = NULL;
  mmgsdi_apdu_params_type         apdu_params          = {0};
  uint8                           rsp_apdu_data[3]     = {0}; /* channel_id, sw1, sw2 */
  uint32                          response_len         = 0;
  mmgsdi_callback_type            response_cb          = NULL;
  mmgsdi_apdu_client_data_type   *apdu_client_data_ptr = NULL;
  mmgsdi_send_apdu_data_type     *apdu_data_ptr        = NULL;

  if (NULL == cnf_ptr)
  {
    return;
  }

  /* Retrieve client data and perform error checks */
  apdu_client_data_ptr =
    (mmgsdi_apdu_client_data_type *)cnf_ptr->response_header.client_data;
  if (NULL == apdu_client_data_ptr)
  {
    UIM_MSG_ERR_0("NULL client data for MANAGE CHANNEL APDU");
    return;
  }

  if (NULL == apdu_client_data_ptr->response_cb)
  {
    UIM_MSG_ERR_0("NULL response_cb for MANAGE CHANNEL APDU");
    return;
  }

  if ((uim_instrns_enum_type)apdu_client_data_ptr->command != MANAGE_CHANNEL)
  {
    UIM_MSG_ERR_1("Invalid command 0x%x for APDU access",
                  (uim_instrns_enum_type)apdu_client_data_ptr->command);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(client_cnf_ptr, sizeof(mmgsdi_cnf_type));
  if (NULL == client_cnf_ptr)
  {
    return;
  }

  /* Populate the response header for cnf to be returned to client */
  client_cnf_ptr->response_header.client_id     = apdu_client_data_ptr->client_id;
  client_cnf_ptr->response_header.client_data   = apdu_client_data_ptr->client_ref;
  client_cnf_ptr->response_header.slot_id       = apdu_client_data_ptr->slot_id;
  if (MMGSDI_SEND_APDU_EXT_REQ == apdu_client_data_ptr->orig_request_type)
  {
    client_cnf_ptr->response_header.response_type = MMGSDI_SEND_APDU_EXT_CNF;
    client_cnf_ptr->send_apdu_ext_cnf.apdu_option = apdu_client_data_ptr->apdu_option;
    client_cnf_ptr->response_header.response_len  =
                        uint32toint32(sizeof(mmgsdi_send_apdu_ext_cnf_type));
    apdu_data_ptr = &client_cnf_ptr->send_apdu_ext_cnf.apdu_data;
  }
  else
  {
    client_cnf_ptr->response_header.response_type = MMGSDI_SEND_APDU_CNF;
    client_cnf_ptr->send_apdu_cnf.channel_id      = apdu_client_data_ptr->channel_id;
    client_cnf_ptr->response_header.response_len  =
                        uint32toint32(sizeof(mmgsdi_send_apdu_cnf_type));
    apdu_data_ptr = &client_cnf_ptr->send_apdu_cnf.apdu_data;
  }
  response_cb = apdu_client_data_ptr->response_cb;
  apdu_params = apdu_client_data_ptr->apdu_params;

  /* Free allocated client data */
  MMGSDIUTIL_TMC_MEM_FREE(apdu_client_data_ptr);

  /* Populate status words
     If status words are not returned by MMGSDI, populate them for the following
     values of mmgsdi_status */
  if (MMGSDI_SUCCESS == mmgsdi_status && !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SIM_SW1_NORM_END;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SIM_SW2_NORM_END;
  }
  else if (MMGSDI_INCORRECT_PARAMS == mmgsdi_status &&
           !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SW1_INCORRECT_PARAMETERS;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SW2_INCORRECT_PARAMS_P1_OR_P2;
  }
  else
  {
    client_cnf_ptr->response_header.status_word.present = cnf_ptr->response_header.status_word.present;
    client_cnf_ptr->response_header.status_word.sw1     = cnf_ptr->response_header.status_word.sw1;
    client_cnf_ptr->response_header.status_word.sw2     = cnf_ptr->response_header.status_word.sw2;
  }

  /* If status words could not be populated in case of error, return error
     cnf to client. Else return success along with status words */
  if (FALSE == client_cnf_ptr->response_header.status_word.present &&
      mmgsdi_status != MMGSDI_SUCCESS)
  {
    client_cnf_ptr->response_header.mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
  }
  else
  {
    client_cnf_ptr->response_header.mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  
  /* Pack response data if SW1, SW2 is present */
  if (client_cnf_ptr->response_header.status_word.present)
  {
    rsp_apdu_data[1] = client_cnf_ptr->response_header.status_word.sw1;
    rsp_apdu_data[2] = client_cnf_ptr->response_header.status_word.sw2;
    response_len     = sizeof(rsp_apdu_data) - 1;

    /* No response data is expected for open specific channel or close channel,
       return status words only.
     */
    if (apdu_params.p1 != MMGSDI_MANAGE_CHANNEL_P1_OPEN || apdu_params.p2 != 0x00)
    {
      apdu_data_ptr->data_ptr = &rsp_apdu_data[1];
    }
    else
    {
      /* Return response data and status words in the same payload */
      response_len++;
      rsp_apdu_data[0]        = cnf_ptr->manage_channel_cnf.channel_id;
      apdu_data_ptr->data_ptr = rsp_apdu_data;
    }
  }

  apdu_data_ptr->data_len                       = response_len;
  client_cnf_ptr->response_header.response_len += response_len;
  client_cnf_ptr->response_header.payload_len   =
    client_cnf_ptr->response_header.response_len - uint32toint32(sizeof(mmgsdi_response_header_type));
  response_cb(mmgsdi_status, client_cnf_ptr->response_header.response_type, client_cnf_ptr);

  /* Free allocated memory */
  MMGSDIUTIL_TMC_MEM_FREE(client_cnf_ptr);
}/* mmgsdi_apdu_manage_channel_cb */


/* ============================================================================
  FUNCTION:      MMGSDI_APDU_MANAGE_CHANNEL

  DESCRIPTION:
    This function translates the MANAGE CHANNEL APDU streaming to a high-level
    MMGSDI API call.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_apdu_manage_channel
(
  mmgsdi_client_id_type                    client_id,
  mmgsdi_slot_id_enum_type                 slot_id,
  mmgsdi_send_apdu_options_enum_type       apdu_option,
  mmgsdi_send_apdu_data_type               apdu_data,
  mmgsdi_client_data_type                  client_ref,
  mmgsdi_callback_type                     client_response_cb,
  mmgsdi_cmd_enum_type                     req_type,
  boolean                                 *reject_adpu_ptr
)
{
  mmgsdi_return_enum_type          mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_channel_id_type           channel_id           = UIM_CHANNEL0;
  mmgsdi_channel_id_type           op_close_channel     = UIM_CHANNEL0;
  uim_mc_action_type               mc_action            = UIM_MC_OPEN;
  mmgsdi_apdu_params_type          apdu_params          = {0};
  mmgsdi_apdu_client_data_type    *apdu_client_data_ptr = NULL;
  mmgsdi_task_cmd_type            *task_cmd_ptr         = NULL;
  mmgsdi_manage_channel_req_type  *msg_ptr              = NULL;
  int32                            total_mem_len        = 0;
  int32                            channel_index        = -1;

  /* Error checking */
  MMGSDIUTIL_RETURN_IF_NULL(apdu_data.data_ptr);

  /* MANAGE CHANNEL is Case 1 or Case 2 APDU
     Currently UIM does not support extended Lc/Le field,
     return error for incorrect length
   */
  if (apdu_data.data_len != UIM_CASE1_7816_APDU_SIZE &&
      apdu_data.data_len != UIM_CASE2_7816_APDU_SIZE)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Some ICC cards in the market supports MANAGE CHANNEL,
     Parse and convert MANAGE CHANNEL APDU regardless of the protocol
   */

  /* Parse APDU */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  mmgsdi_status = mmgsdi_util_get_channel_id_from_cla(
                    apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET],
                    slot_id,
                    &channel_id);
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  op_close_channel         = apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET];
  apdu_params.params_valid = TRUE;
  apdu_params.p1           = apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET];
  apdu_params.p2           = apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET];
  switch (apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET])
  {
    case MMGSDI_MANAGE_CHANNEL_P1_OPEN:
      /* ISO/IEC 7816-4 11.1.2
         The open function opens an additional logical channel other than the
         basic logical channel. Options are provided for the card to assign a
         channel number, or for a channel number to be supplied to the card.
         If bits b8 and b7 of P1 are set to 00 (i.e. P1 set to '00' because
         the other six bits are RFU), then MANAGE CHANNEL shall open a logical
         channel numbered from one to nineteen as follows.
         1. If P2 is set to '00', then the Le field shall be '01' (short format)
            or '000001' (expanded format), and the response data field shall
            consist of a single byte for encoding a non-zero channel number
            assigned by the card from '01' to '13'.
         2. If P2 is set from '01' to '13', then it encodes an externally
            assigned non-zero channel number and the Le field shall be absent.
      */
      /* Open channel:
           if P2 != 0x00, Le should be absent
           otherwise, Le should be 1
           Le is 0x00 will be handled as regular streaming APDU since some cards support it)
        */
      if ((apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET] != 0x00 &&
           apdu_data.data_len > UIM_CASE1_7816_APDU_SIZE)
          ||
          (apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET] == 0x00 &&
           (apdu_data.data_len <= UIM_CASE1_7816_APDU_SIZE ||
            apdu_data.data_ptr[UIM_7816_APDU_MIN_SIZE] != 0x01))
          )
      {
        return MMGSDI_INCORRECT_PARAMS;
      }

      mc_action = UIM_MC_OPEN;
      if (0x00 == apdu_params.p2)
      {
        apdu_params.p3 = apdu_data.data_ptr[UIM_7816_APDU_MIN_SIZE];
      }
      break;

    case MMGSDI_MANAGE_CHANNEL_P1_CLOSE:
      /* ISO/IEC 7816-4 11.1.2
         The close function explicitly closes a logical channel other than the
         basic one. The Le field shall be absent. After closing, the logical
         channel shall be available for re-use.
         If bits b8 and b7 of P1 are set to 10 (i.e. P1 set to '80' because
         the other six bits are RFU), then MANAGE CHANNEL shall close a logical
         channel numbered from 1 to nineteen as follows.
         1. If P2 is set to '00', then the logical channel numbered in CLA
            (a non-zero channel number) shall be closed.
         2. If P2 is set from '01' to '13', then the logical channel numbered
            in P2 shall be closed.
      */
      /* Close non-basic channel: Le should be absent */
      if (apdu_data.data_len > UIM_CASE1_7816_APDU_SIZE)
      {
        return MMGSDI_INCORRECT_PARAMS;
      }

      /* if P2 == 0x00, channel id in CLA byte should be closed */
      if (0x00 == op_close_channel)
      {
        op_close_channel = channel_id;
        if (UIM_CHANNEL0 == channel_id)
        {
          /* This APDU should be rejected instead of continuing normal APDU processing */
          if (reject_adpu_ptr)
          {
            *reject_adpu_ptr = TRUE;
          }
          UIM_MSG_ERR_0("Closing default channel is not allowed");
          return MMGSDI_INCORRECT_PARAMS;
        }
      }

      /* Do not allow send APDU clients to close channels belong to others */ 
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
      mmgsdi_status = mmgsdi_util_match_channel_index(
                                            slot_id,
                                            op_close_channel,
                                            &channel_index);
      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
      if (MMGSDI_SUCCESS == mmgsdi_status)
      {
        /* This APDU should be rejected instead of continuing normal APDU processing */
        if (reject_adpu_ptr)
        {
          *reject_adpu_ptr = TRUE;
        }
        UIM_MSG_ERR_1("Closing channel 0x%x is not allowed",
                      op_close_channel);
        return MMGSDI_INCORRECT_PARAMS;
      }

      /* All checks are done, set mc_action */
      mc_action = UIM_MC_CLOSE;
      break;

    default:
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* Store client data on heap so that the response from MMGSDI can be
     returned back to the client later */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(apdu_client_data_ptr,
                                     sizeof(mmgsdi_apdu_client_data_type));
  if (NULL == apdu_client_data_ptr)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  apdu_client_data_ptr->client_id         = client_id;
  apdu_client_data_ptr->slot_id           = slot_id;
  apdu_client_data_ptr->channel_id        = channel_id;
  apdu_client_data_ptr->command           = MANAGE_CHANNEL;
  apdu_client_data_ptr->orig_request_type = req_type;
  apdu_client_data_ptr->apdu_params       = apdu_params;
  apdu_client_data_ptr->apdu_option       = apdu_option;
  apdu_client_data_ptr->response_cb       = client_response_cb;
  apdu_client_data_ptr->client_ref        = client_ref;

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (NULL == task_cmd_ptr)
  {
    MMGSDIUTIL_TMC_MEM_FREE(apdu_client_data_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.manage_channel_req;

  /*---------------------------------------------------------------------------
    Populate header
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_MANAGE_CHANNEL_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_MANAGE_CHANNEL_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_MANAGE_CHANNEL_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = slot_id;
  msg_ptr->request_header.channel_id        = channel_id;
  msg_ptr->request_header.client_data       = (mmgsdi_client_data_type)apdu_client_data_ptr;
  msg_ptr->request_header.response_cb       = mmgsdi_apdu_manage_channel_cb;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
     Populate command details
  ---------------------------------------------------------------------------*/
  msg_ptr->op_close_channel = op_close_channel;
  msg_ptr->mc_action        = mc_action;

  /*---------------------------------------------------------------------------
    Put on mmgsdi command queue
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(apdu_client_data_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_MANAGE_CHANNEL_REQ status 0x%x",
                 mmgsdi_status);
  return mmgsdi_status;
} /* mmgsdi_apdu_manage_channel */
