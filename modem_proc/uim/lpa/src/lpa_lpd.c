/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   L P D   C O M M A N D   F U N C T I O N S


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_lpd.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   ll      Peek into profile installation result 
06/21/16   av      Add SVN check for supporting phase1, phase1.1 and phase2
05/15/16   ll      ASN1 encoder code clean up
05/15/16   ll      ASN1 decoder enhancement
05/04/16   dd      Fixing some compiler warnings and adding more checks
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
#include "lpa_http.h"
#include "lpa_asn1_encoder.h"
#include "lpa_asn1_decoder.h"
#include <stringl/stringl.h>
#include "lpa_efs.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define LPA_GET_EUICC_INFO_APDU_DATA_P2                                     0x20
#define LPA_LOAD_BPP_APDU_DATA_P1                                           0x91
#define LPA_PREPARE_DOWNLOAD_APDU_DATA_P1                                   0x91
#define LPA_RECEIPT_RESULT_CODE_BYTE0_INSTALLATION_SUCCESS                  0x00
#define LPA_RECEIPT_RESULT_CODE_BYTE0_INSTALLATION_FAIL                     0x01
#define LPA_RECEIPT_RESULT_CODE_BYTE1_ICCID_EXISTS                          0x01
#define LPA_RECEIPT_RESULT_CODE_BYTE1_EUICC_INSUFFICIENT_MEMORY             0x02
#define LPA_RECEIPT_RESULT_CODE_BYTE1_FAIL_DUE_TO_INTERRUPTION              0x03

static q_type                             lpa_lpd_cmd_q;

typedef struct {
  lpa_activation_code_type                activation_code;
  uint8                                   euicc_challenge[LPA_EUICC_CHALLENGE_LEN];
  uint8                                   svn[LPA_SVN_LEN];
  lpa_data_type                           transaction_id;
  lpa_data_type                           cert_info;
  lpa_data_type                           euicc_info;
  lpa_util_device_info_type               device_info;
  lpa_bpp_tlvs_data_type                  bpp_tlvs;
  uint32                                  last_loaded_bpp_tlv_counter;
} lpa_lpd_info_type;

/* Contains information regarding the add profile procedure */
static lpa_lpd_info_type                  lpa_lpd_global_info[LPA_NUM_SLOTS];


/*===========================================================================

                                  FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION LPA_LPD_GET_PROFILE_INSTALLATION_RESULT

DESCRIPTION
  Function calls the asn1 installation result decoder and converts the result
  code to LPA status.
 
DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_get_profile_installation_result (
  uint32                                             data_len,
  uint8                                             *data_ptr
)
{
  lpa_lpd_profile_installation_result_code_array_type result_code_array;
  lpa_result_enum_type                                installation_result = LPA_SUCCESS;

  memset(result_code_array, 0x00, LPA_INSTALLATION_RESULT_CODE_LEN);

  if (LPA_SUCCESS != lpa_asn1_decode_profile_installation_result(data_len,
                                                                 data_ptr,
                                                                 result_code_array)) 
  {
    installation_result = LPA_GENERIC_ERROR;
  }

  if (LPA_RECEIPT_RESULT_CODE_BYTE0_INSTALLATION_FAIL == result_code_array[0]) 
  {
    switch (result_code_array[1]) 
    {
      case LPA_RECEIPT_RESULT_CODE_BYTE1_ICCID_EXISTS: 
        installation_result = LPA_ERROR_ICCID_EXISTS_ON_EUICC;
        break;
      case LPA_RECEIPT_RESULT_CODE_BYTE1_EUICC_INSUFFICIENT_MEMORY: 
        installation_result = LPA_ERROR_INSUFFICIENT_MEMORY_ON_EUICC;
        break;
      case LPA_RECEIPT_RESULT_CODE_BYTE1_FAIL_DUE_TO_INTERRUPTION: 
        installation_result = LPA_ERROR_PROFILE_INSTALLATION_INTERRUPTED_ON_EUICC;
        break;
      default:
        installation_result = LPA_ERROR_PROFILE_INSTALLATION_UNKNOWN_ERR;
        break;
    }
  }
  else if (LPA_RECEIPT_RESULT_CODE_BYTE0_INSTALLATION_SUCCESS != result_code_array[0]) 
  {
    installation_result = LPA_GENERIC_ERROR;
  }

  return installation_result;
}/*lpa_lpd_get_profile_installation_result*/


/*===========================================================================
FUNCTION LPA_LPD_CMD_INIT

DESCRIPTION
  This function, called in lpa_main, is called to initialize the LPD command
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
void lpa_lpd_cmd_init (
  void
)
{
  (void) q_init(&lpa_lpd_cmd_q);
} /* lpa_lpd_cmd_init */


/*===========================================================================
   FUNCTION:      LPA_LPA_FREE_RESPONSE_DATA

   DESCRIPTION:
     This function frees the response pointer that is mallocd (in lpa_mmgsdi or
     in lpa_http) in order to send a response back to LPA LPD via its callback

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_lpd_free_response_data(
  lpa_lpd_cmd_data_type                *lpa_response_data_ptr
)
{
  LPA_MSG_HIGH_0("lpa_lpd_free_response_data");

  if(lpa_response_data_ptr == NULL)
  {
    return;
  }

  switch(lpa_response_data_ptr->cmd)
  {
    case LPA_LPD_GET_EUICC_CHALLENGE_RESP_CMD:
    case LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD:
       break;
    case LPA_LPD_GET_EUICC_INFO_RESP_CMD:
      LPA_FREE(lpa_response_data_ptr->data.get_euicc_info_resp.cert_info.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.get_euicc_info_resp.euicc_info.data_ptr);
      break;
    case LPA_LPD_PREPARE_DOWNLOAD_RESP_CMD:
      LPA_FREE(lpa_response_data_ptr->data.prepare_download_resp.prepare_download.data_ptr);
      break;
    case LPA_LPD_LOAD_BPP_RESP_CMD:
      LPA_FREE(lpa_response_data_ptr->data.load_bpp_resp.load_bpp.data_ptr);
      break;
    case LPA_LPD_INITIATE_AUTH_RESP_CMD:
      LPA_FREE(lpa_response_data_ptr->data.initiate_auth_resp.transaction_id.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.initiate_auth_resp.dp_signed1.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.initiate_auth_resp.smdp_signature1.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.initiate_auth_resp.curve_to_be_used.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.initiate_auth_resp.smdp_certificate.data_ptr);
      break;
    case LPA_LPD_GET_BPP_RESP_CMD:
      LPA_FREE(lpa_response_data_ptr->data.get_bpp_resp.profile_metadata.data_ptr);
      LPA_FREE(lpa_response_data_ptr->data.get_bpp_resp.bpp.data_ptr);
      break;
    default:
       break;
  }
} /* lpa_lpd_free_response_data */


/*===========================================================================
   FUNCTION:      LPA_LPD_RESET

   DESCRIPTION:
     This function resets/frees any globals in lpa lpd.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_lpd_reset (
  lpa_slot_id_enum_type                lpa_slot
)
{
  uint8                   i             = 0;

  LPA_MSG_HIGH_1("lpa_lpd_reset, slot_id=0x%x", lpa_slot);

  if(lpa_slot >= LPA_NUM_SLOTS)
  {
    return;
  }

  LPA_FREE(lpa_lpd_global_info[lpa_slot].activation_code.hashed_confirmation_code.data_ptr);
  LPA_FREE(lpa_lpd_global_info[lpa_slot].cert_info.data_ptr);
  LPA_FREE(lpa_lpd_global_info[lpa_slot].euicc_info.data_ptr);
  LPA_FREE(lpa_lpd_global_info[lpa_slot].transaction_id.data_ptr);
  while(i < (sizeof(lpa_lpd_global_info[lpa_slot].bpp_tlvs.tlvs_data)/sizeof(lpa_lpd_global_info[lpa_slot].bpp_tlvs.tlvs_data[0])) &&
        i < lpa_lpd_global_info[lpa_slot].bpp_tlvs.tlvs_num &&
        lpa_lpd_global_info[lpa_slot].bpp_tlvs.tlvs_data[i].data_ptr)
  {
    LPA_FREE(lpa_lpd_global_info[lpa_slot].bpp_tlvs.tlvs_data[i].data_ptr);
    i++;
  }

  memset(&lpa_lpd_global_info[lpa_slot], 0x00, sizeof(lpa_lpd_global_info[lpa_slot]));
} /* lpa_lpd_reset */


/*===========================================================================
   FUNCTION:      LPA_LPD_QUEUE_CMD

   DESCRIPTION:
     This function queues commands to LPA to processes the LPD procedures.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     lpa_result_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static lpa_result_enum_type  lpa_lpd_queue_cmd (
  lpa_lpd_response_data_type          *task_cmd_ptr
)
{
  lpa_result_enum_type    lpa_status    = LPA_SUCCESS;

  if (task_cmd_ptr == NULL)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  LPA_UTIL_ENTER_TASK_STATE_CRIT_SECT;

  if (!lpa_task_stopped)
  {
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    /* Put the message on the queue */
    q_put(&lpa_lpd_cmd_q, &task_cmd_ptr->link);

    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_LPA_TCB, LPA_LPD_CMD_SIG);
  }
  else
  {
    LPA_MSG_ERR_0("LPA task stopped, cannot queue command");
    lpa_status = LPA_GENERIC_ERROR;
  }

  LPA_UTIL_LEAVE_TASK_STATE_CRIT_SECT;
  return lpa_status;
} /* lpa_lpd_queue_cmd */


/*===========================================================================
FUNCTION LPA_LPD_HTTP_CB

DESCRIPTION

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_lpd_http_cb (
  lpa_result_enum_type                      status,
  const lpa_lpd_cmd_data_type              *resp_ptr,
  lpa_slot_id_enum_type                     slot_id
)
{
  lpa_lpd_response_data_type  *task_cmd_ptr = NULL;
  lpa_result_enum_type         lpa_status   = LPA_SUCCESS;

  LPA_MSG_HIGH_2("Received lpa_lpd_http_cb status=0x%x, slot=0x%x", status, slot_id);

  if(resp_ptr == NULL)
  {
    return;
  }

  /* Malloc task cmd pointers and any sub pointers, as needed, and copy
     the response data received from the caller of this callback */
  task_cmd_ptr = (lpa_lpd_response_data_type*)LPA_MALLOC(sizeof(lpa_lpd_response_data_type));
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->slot_id            = slot_id;
  task_cmd_ptr->status             = status;

  if(status == LPA_SUCCESS || status == LPA_ERROR_NETWORK)
  {
    if(resp_ptr != NULL)
    {
      task_cmd_ptr->cmd_data = *resp_ptr;
    }
    else
    {
      task_cmd_ptr->status = LPA_GENERIC_ERROR;
    }
  }

  /* MALLOC and copy any pointers inside resp_ptr */
  switch(resp_ptr->cmd)
  {
    case LPA_LPD_INITIATE_AUTH_RESP_CMD:
      if(resp_ptr->data.initiate_auth_resp.transaction_id.data_ptr &&
         resp_ptr->data.initiate_auth_resp.transaction_id.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.transaction_id.data_ptr =
          LPA_MALLOC(resp_ptr->data.initiate_auth_resp.transaction_id.data_len);
        if(task_cmd_ptr->cmd_data.data.initiate_auth_resp.transaction_id.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.transaction_id.data_len =
          resp_ptr->data.initiate_auth_resp.transaction_id.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.initiate_auth_resp.transaction_id.data_ptr,
                task_cmd_ptr->cmd_data.data.initiate_auth_resp.transaction_id.data_len,
                resp_ptr->data.initiate_auth_resp.transaction_id.data_ptr,
                resp_ptr->data.initiate_auth_resp.transaction_id.data_len);
      }
      if(resp_ptr->data.initiate_auth_resp.dp_signed1.data_ptr &&
         resp_ptr->data.initiate_auth_resp.dp_signed1.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.dp_signed1.data_ptr =
          LPA_MALLOC(resp_ptr->data.initiate_auth_resp.dp_signed1.data_len);
        if(task_cmd_ptr->cmd_data.data.initiate_auth_resp.dp_signed1.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.dp_signed1.data_len =
          resp_ptr->data.initiate_auth_resp.dp_signed1.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.initiate_auth_resp.dp_signed1.data_ptr,
                task_cmd_ptr->cmd_data.data.initiate_auth_resp.dp_signed1.data_len,
                resp_ptr->data.initiate_auth_resp.dp_signed1.data_ptr,
                resp_ptr->data.initiate_auth_resp.dp_signed1.data_len);
      }
      if(resp_ptr->data.initiate_auth_resp.smdp_signature1.data_ptr &&
         resp_ptr->data.initiate_auth_resp.smdp_signature1.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_signature1.data_ptr =
          LPA_MALLOC(resp_ptr->data.initiate_auth_resp.smdp_signature1.data_len);
        if(task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_signature1.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_signature1.data_len =
          resp_ptr->data.initiate_auth_resp.smdp_signature1.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_signature1.data_ptr,
                task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_signature1.data_len,
                resp_ptr->data.initiate_auth_resp.smdp_signature1.data_ptr,
                resp_ptr->data.initiate_auth_resp.smdp_signature1.data_len);
      }
      if(resp_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_ptr &&
         resp_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.cert_format_to_be_used.data_ptr =
          LPA_MALLOC(resp_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_len);
        if(task_cmd_ptr->cmd_data.data.initiate_auth_resp.cert_format_to_be_used.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.cert_format_to_be_used.data_len =
          resp_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.initiate_auth_resp.cert_format_to_be_used.data_ptr,
                task_cmd_ptr->cmd_data.data.initiate_auth_resp.cert_format_to_be_used.data_len,
                resp_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_ptr,
                resp_ptr->data.initiate_auth_resp.cert_format_to_be_used.data_len);
      }
      if(resp_ptr->data.initiate_auth_resp.curve_to_be_used.data_ptr &&
         resp_ptr->data.initiate_auth_resp.curve_to_be_used.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.curve_to_be_used.data_ptr =
          LPA_MALLOC(resp_ptr->data.initiate_auth_resp.curve_to_be_used.data_len);
        if(task_cmd_ptr->cmd_data.data.initiate_auth_resp.curve_to_be_used.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.curve_to_be_used.data_len =
          resp_ptr->data.initiate_auth_resp.curve_to_be_used.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.initiate_auth_resp.curve_to_be_used.data_ptr,
                task_cmd_ptr->cmd_data.data.initiate_auth_resp.curve_to_be_used.data_len,
                resp_ptr->data.initiate_auth_resp.curve_to_be_used.data_ptr,
                resp_ptr->data.initiate_auth_resp.curve_to_be_used.data_len);
      }
      if(resp_ptr->data.initiate_auth_resp.smdp_certificate.data_ptr &&
         resp_ptr->data.initiate_auth_resp.smdp_certificate.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_certificate.data_ptr =
          LPA_MALLOC(resp_ptr->data.initiate_auth_resp.smdp_certificate.data_len);
        if(task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_certificate.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_certificate.data_len =
          resp_ptr->data.initiate_auth_resp.smdp_certificate.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_certificate.data_ptr,
                task_cmd_ptr->cmd_data.data.initiate_auth_resp.smdp_certificate.data_len,
                resp_ptr->data.initiate_auth_resp.smdp_certificate.data_ptr,
                resp_ptr->data.initiate_auth_resp.smdp_certificate.data_len);
      }
      break;
    case LPA_LPD_GET_BPP_RESP_CMD:
      if(resp_ptr->data.get_bpp_resp.profile_metadata.data_ptr &&
         resp_ptr->data.get_bpp_resp.profile_metadata.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.get_bpp_resp.profile_metadata.data_ptr =
          LPA_MALLOC(resp_ptr->data.get_bpp_resp.profile_metadata.data_len);
        if(task_cmd_ptr->cmd_data.data.get_bpp_resp.profile_metadata.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.get_bpp_resp.profile_metadata.data_len =
          resp_ptr->data.get_bpp_resp.profile_metadata.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.get_bpp_resp.profile_metadata.data_ptr,
                task_cmd_ptr->cmd_data.data.get_bpp_resp.profile_metadata.data_len,
                resp_ptr->data.get_bpp_resp.profile_metadata.data_ptr,
                resp_ptr->data.get_bpp_resp.profile_metadata.data_len);
      }
      if(resp_ptr->data.get_bpp_resp.bpp.data_ptr &&
         resp_ptr->data.get_bpp_resp.bpp.data_len > 0)
      {
        task_cmd_ptr->cmd_data.data.get_bpp_resp.bpp.data_ptr =
          LPA_MALLOC(resp_ptr->data.get_bpp_resp.bpp.data_len);
        if(task_cmd_ptr->cmd_data.data.get_bpp_resp.bpp.data_ptr == NULL)
        {
          lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.get_bpp_resp.bpp.data_len =
          resp_ptr->data.get_bpp_resp.bpp.data_len;
        memscpy(task_cmd_ptr->cmd_data.data.get_bpp_resp.bpp.data_ptr,
                task_cmd_ptr->cmd_data.data.get_bpp_resp.bpp.data_len,
                resp_ptr->data.get_bpp_resp.bpp.data_ptr,
                resp_ptr->data.get_bpp_resp.bpp.data_len);
      }
      break;
    default:
      break;
  }

  if(lpa_lpd_queue_cmd(task_cmd_ptr) != LPA_SUCCESS)
  {
    lpa_lpd_free_response_data(&task_cmd_ptr->cmd_data);
    LPA_FREE(task_cmd_ptr);
  }
} /* lpa_lpd_http_cb */


/*===========================================================================
   FUNCTION:      LPA_LPA_MMGSDI_CALLBACK

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
void lpa_lpd_mmgsdi_cb (
  lpa_result_enum_type                             status,
  const lpa_mmgsdi_resp_data_type                 *mmgsdi_resp_ptr,
  const void                                      *user_data_ptr
)
{
  lpa_lpd_response_data_type             *task_cmd_ptr = NULL;

  LPA_MSG_HIGH_2("Received lpa_lpd_mmgsdi_cb status=0x%x,cmd=0x%x",
                 status, (lpa_lpd_cmd_enum_type)user_data_ptr);

  if(mmgsdi_resp_ptr == NULL)
  {
    return;
  }

  /* Malloc task cmd pointers and any sub pointers, as needed, and copy
     the response data received from the caller of this callback */
  task_cmd_ptr = (lpa_lpd_response_data_type*)LPA_MALLOC(sizeof(lpa_lpd_response_data_type));
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->slot_id            = mmgsdi_resp_ptr->slot_id;
  task_cmd_ptr->status             = status;
  task_cmd_ptr->cmd_data.cmd       = (lpa_lpd_cmd_enum_type)user_data_ptr;

  if(status == LPA_SUCCESS)
  {
    switch(mmgsdi_resp_ptr->cmd_type)
    {
      case LPA_MMGSDI_APDU_STORE_DATA:
        /* STORE DATA is sent for two operations: PrepareDownload and
           Load BPP */
        if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_PREPARE_DOWNLOAD_RESP_CMD)
        {
          if(mmgsdi_resp_ptr->data_len == 0 ||
             mmgsdi_resp_ptr->data_ptr == NULL)
          {
            status = LPA_GENERIC_ERROR;
            break;
          }
          task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_ptr =
            LPA_MALLOC(mmgsdi_resp_ptr->data_len);
          if(task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_ptr == NULL)
          {
            status = LPA_ERROR_HEAP_EXHAUSTED;
            break;
          }
          task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_len =
            mmgsdi_resp_ptr->data_len;
          memscpy(task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_ptr,
                  task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_len,
                  mmgsdi_resp_ptr->data_ptr,
                  mmgsdi_resp_ptr->data_len);
        }
        else if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_LOAD_BPP_RESP_CMD)
        {
          if(mmgsdi_resp_ptr->data_len == 0 ||
             mmgsdi_resp_ptr->data_ptr == NULL)
          {
            status = LPA_GENERIC_ERROR;
            break;
          }

          task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_ptr =
            LPA_MALLOC(mmgsdi_resp_ptr->data_len);
          if(task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_ptr == NULL)
          {
            status = LPA_ERROR_HEAP_EXHAUSTED;
            break;
          }
          task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_len =
            mmgsdi_resp_ptr->data_len;
          memscpy(task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_ptr,
                  task_cmd_ptr->cmd_data.data.prepare_download_resp.prepare_download.data_len,
                  mmgsdi_resp_ptr->data_ptr,
                  mmgsdi_resp_ptr->data_len);
        }
        else
        {
          status = LPA_GENERIC_ERROR;
        }
        break;
      case LPA_MMGSDI_APDU_GET_CHALLENGE:
        /* For GET EUICC CHALLENGE, we need to do some validation of the
           response received from the card - we must receive 16
           bytes in the data */
        task_cmd_ptr->cmd_data.cmd = LPA_LPD_GET_EUICC_CHALLENGE_RESP_CMD;
        if(mmgsdi_resp_ptr->data_len != LPA_EUICC_CHALLENGE_LEN ||
           mmgsdi_resp_ptr->data_ptr == NULL)
        {
          status = LPA_GENERIC_ERROR;
          break;
        }
        /* Copy the euicc Challenge that can be fed into the Initiate Auth
           request by LPD */
        memscpy(task_cmd_ptr->cmd_data.data.get_euicc_challenge_resp.euicc_challenge,
                sizeof(task_cmd_ptr->cmd_data.data.get_euicc_challenge_resp.euicc_challenge),
                mmgsdi_resp_ptr->data_ptr,
                mmgsdi_resp_ptr->data_len);
        break;
      case LPA_MMGSDI_APDU_GET_DATA:
        /* This response is for GET_DATA APDU sent for eUICCInfo. Lets validate
           the response */
        task_cmd_ptr->cmd_data.cmd = LPA_LPD_GET_EUICC_INFO_RESP_CMD;
        if(mmgsdi_resp_ptr->data_len < LPA_SVN_LEN ||
           mmgsdi_resp_ptr->data_ptr == NULL)
        {
          status = LPA_GENERIC_ERROR;
          break;
        }
        /* Copy the euicc Info that can be fed into the Initiate Auth
           request by LPD */
        task_cmd_ptr->cmd_data.data.get_euicc_info_resp.euicc_info.data_ptr =
          LPA_MALLOC(mmgsdi_resp_ptr->data_len);
        if(task_cmd_ptr->cmd_data.data.get_euicc_info_resp.euicc_info.data_ptr == NULL)
        {
          status = LPA_ERROR_HEAP_EXHAUSTED;
          break;
        }
        task_cmd_ptr->cmd_data.data.get_euicc_info_resp.euicc_info.data_len =
          mmgsdi_resp_ptr->data_len;
        memscpy(task_cmd_ptr->cmd_data.data.get_euicc_info_resp.euicc_info.data_ptr,
                task_cmd_ptr->cmd_data.data.get_euicc_info_resp.euicc_info.data_len,
                mmgsdi_resp_ptr->data_ptr,
                mmgsdi_resp_ptr->data_len);
        /* Parse SVN from the euicc Info that can be fed into the Initiate Auth
           request by LPD */
        status = lpa_asn1_decode_euicc_info_apdu(
                       mmgsdi_resp_ptr->data_len,
                       mmgsdi_resp_ptr->data_ptr,
                       &task_cmd_ptr->cmd_data.data.get_euicc_info_resp);
        break;
      default:
        status = LPA_GENERIC_ERROR;
        break;
    }
  }

  if(lpa_lpd_queue_cmd(task_cmd_ptr) != LPA_SUCCESS)
  {
    lpa_lpd_free_response_data(&task_cmd_ptr->cmd_data);
    LPA_FREE(task_cmd_ptr);
  }
} /* lpa_lpd_mmgsdi_cb */


/*===========================================================================
FUNCTION LPA_LPD_START_ADD_PROFILE_PROCEDURE

DESCRIPTION

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_lpd_start_add_profile_procedure (
  const lpa_add_profile_req_type   *add_profile_req_ptr,
  lpa_slot_id_enum_type             slot_id
)
{
  lpa_result_enum_type       lpa_status = LPA_SUCCESS;

  if(add_profile_req_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_status = lpa_state_change(LPA_STATE_IDLE, LPA_STATE_INIT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  lpa_efs_cleanup(slot_id);

  /* Write raw activation code to the file */
  (void) lpa_efs_write(LPA_FILE_ACTIVATION_CODE_FROM_USER,
                       add_profile_req_ptr->activation_code.data_len,
                       add_profile_req_ptr->activation_code.data_ptr,
                       slot_id);

  /* Parse activation code and copy it to the global that can later be used
     to send the Initiate Auth request to SMDP */
  lpa_status = lpa_util_parse_activation_code(
                 &add_profile_req_ptr->activation_code,
                 &lpa_lpd_global_info->activation_code);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* If activation code says confirmation code is required, make sure it
     was provided by the user. If it was provided, hash it and save it
     for later */

  if(lpa_lpd_global_info->activation_code.confirmation_code_required)
  {
    if(add_profile_req_ptr->confirmation_code.data_len == 0 ||
       add_profile_req_ptr->confirmation_code.data_ptr == NULL)
    {
      LPA_MSG_ERR_0("Confirmation code required but not present!");
      return LPA_ERROR_CONFIRMATION_CODE_REQUIRED;
    }
    /* Confirmation code is required and is present. Hash it. Note that the
       hashing function below MALLOCs the out pointer. Make sure to free it
       when done */
    lpa_status = lpa_util_hash_sha256(add_profile_req_ptr->confirmation_code.data_ptr,
                                      add_profile_req_ptr->confirmation_code.data_len,
                                      &lpa_lpd_global_info->activation_code.hashed_confirmation_code);
    if(lpa_status != LPA_SUCCESS)
    {
      LPA_FREE(lpa_lpd_global_info->activation_code.hashed_confirmation_code.data_ptr);
      return lpa_status;
    }
    LPA_MSG_HIGH_0("Confirmation code hashing successful");
  }

  lpa_status = lpa_mmgsdi_get_challenge(slot_id,
                                        lpa_lpd_mmgsdi_cb,
                                        (void*) LPA_LPD_GET_EUICC_CHALLENGE_RESP_CMD);
  if(lpa_status == LPA_SUCCESS)
  {
    /* We should now send response to LPA client for the Add Profile request.
       Note that the remaining updates regarding the various sub procedures
       involved, we will send event to the LPA client.
       Send the response but do not clear timers etc (by passing FALSE into
       lpa_send_response() */
    lpa_response_data_type            *resp_ptr = NULL;

    resp_ptr = (lpa_response_data_type*) LPA_MALLOC(sizeof(lpa_response_data_type));
    if(resp_ptr != NULL)
    {
      resp_ptr->msg_type = LPA_ADD_PROFILE_MSG;
    }
    lpa_send_response(lpa_status,
                      resp_ptr,
                      slot_id,
                      FALSE);
    LPA_FREE(resp_ptr);
    /* Do not set the sig */
  }
  return lpa_status;
} /* lpa_lpd_start_add_profile_procedure */


/*===========================================================================
FUNCTION LPA_LPD_PROCESS_GET_EUICC_CHALLENGE_RESP

DESCRIPTION
  Processes euicc challenge received from MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_get_euicc_challenge_resp (
  lpa_lpd_get_euicc_challenge_resp_type  *get_euicc_challenge_ptr,
  lpa_slot_id_enum_type                   slot_id
)
{
  lpa_data_type                           data;

  memset(&data, 0x00, sizeof(data));

  if(get_euicc_challenge_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  /* Copy euicc challenge over to the global that we will later use for
     sending request to SMDP  */
  memscpy(lpa_lpd_global_info[slot_id].euicc_challenge,
          sizeof(lpa_lpd_global_info[slot_id].euicc_challenge),
          get_euicc_challenge_ptr->euicc_challenge,
          sizeof(get_euicc_challenge_ptr->euicc_challenge));

  /* Also write it to the file */
  (void) lpa_efs_write(LPA_FILE_EUICC_CHAL_FROM_CARD,
                       sizeof(get_euicc_challenge_ptr->euicc_challenge),
                       get_euicc_challenge_ptr->euicc_challenge,
                       slot_id);

  /* Now lets get the euicc Info from the card... without leaving the current
     state, which is LPA_STATE_INIT */
  return lpa_mmgsdi_get_data(slot_id,
                             (uint8)LPA_GET_EUICC_INFO_APDU_DATA_P2,
                             data,
                             lpa_lpd_mmgsdi_cb,
                             (void*) LPA_LPD_GET_EUICC_INFO_RESP_CMD);
} /* lpa_lpd_process_get_euicc_challenge_resp */


/*===========================================================================
FUNCTION LPA_LPD_PROCESS_PREPARE_DOWNLOAD_RESP

DESCRIPTION
  Processes prepare download (STORE DATA) response from the card

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_process_prepare_download_resp (
  lpa_lpd_prepare_download_resp_type  *prepare_download_resp_ptr,
  lpa_slot_id_enum_type                slot_id
)
{
  lpa_result_enum_type       lpa_status = LPA_SUCCESS;
  lpa_lpd_get_bpp_req_type   get_bpp_req;

  if(prepare_download_resp_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  /* At this point we have retrieved prepare download response from the
     card. Both the card and the SMPD have been authenticated.
     Go ahead and send Get BPP request to SMDP */
  lpa_status = lpa_state_change(LPA_STATE_INITIATE_AUTH_RESP_RCVD, LPA_STATE_GET_BPP_SENT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    return lpa_status;
  }

  /* No need to decode the ASN1 formatted prepareDownloadResponse from the
     card... send it as-is into the GET_BPP request to SMDP */
  memset(&get_bpp_req, 0x00, sizeof(lpa_lpd_get_bpp_req_type));
  get_bpp_req.prepare_download_data = prepare_download_resp_ptr->prepare_download;
  get_bpp_req.slot_id = slot_id;
  get_bpp_req.lpd_callback = lpa_lpd_http_cb;
  get_bpp_req.transaction_id_data   = lpa_lpd_global_info[slot_id].transaction_id;

  /* Write the data received from card to EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_PREP_DOWNLOAD_RESP_FROM_CARD,
                       prepare_download_resp_ptr->prepare_download.data_len,
                       prepare_download_resp_ptr->prepare_download.data_ptr,
                       slot_id);

  return lpa_http_build_and_send_get_bpp(&get_bpp_req);
} /* lpa_lpd_process_process_prepare_download_resp */


/*===========================================================================
FUNCTION LPA_LPD_PROCESS_GET_EUICC_INFO_RESP

DESCRIPTION
  Processes euicc info received from MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_get_euicc_info_resp (
  lpa_lpd_get_euicc_info_resp_type            *get_euicc_info_ptr,
  lpa_slot_id_enum_type                        slot_id
)
{
  lpa_result_enum_type                    lpa_status            = LPA_SUCCESS;
  lpa_lpd_initiate_auth_req_type          initiate_auth_req;

  if(get_euicc_info_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  if(get_euicc_info_ptr->euicc_info.data_ptr == NULL||
     get_euicc_info_ptr->euicc_info.data_len == 0)
  {
    return LPA_GENERIC_ERROR;
  }

  /* Write the data received from card to EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_EUICC_INFO_FROM_CARD,
                       get_euicc_info_ptr->euicc_info.data_len,
                       get_euicc_info_ptr->euicc_info.data_ptr,
                       slot_id);

  /* Copy euicc info over to the global that we will later use for
     sending Initiate Auth request to SMDP */
  if(get_euicc_info_ptr->euicc_info.data_ptr)
  {
    /* Make sure we clean up the stale value from global before populating
       the new one */
    LPA_FREE(lpa_lpd_global_info[slot_id].euicc_info.data_ptr);
    memset(&lpa_lpd_global_info[slot_id].euicc_info,
           0x00,
           sizeof(lpa_lpd_global_info[slot_id].euicc_info));

    lpa_lpd_global_info[slot_id].euicc_info.data_ptr =
      LPA_MALLOC(get_euicc_info_ptr->euicc_info.data_len);
    if(lpa_lpd_global_info[slot_id].euicc_info.data_ptr == NULL)
    {
      return LPA_ERROR_HEAP_EXHAUSTED;
    }
    lpa_lpd_global_info[slot_id].euicc_info.data_len =
      get_euicc_info_ptr->euicc_info.data_len;
    memscpy(lpa_lpd_global_info[slot_id].euicc_info.data_ptr,
            lpa_lpd_global_info[slot_id].euicc_info.data_len,
            get_euicc_info_ptr->euicc_info.data_ptr,
            get_euicc_info_ptr->euicc_info.data_len);
  }
  memscpy(lpa_lpd_global_info[slot_id].svn,
          sizeof(lpa_lpd_global_info[slot_id].svn),
          get_euicc_info_ptr->svn,
          sizeof(get_euicc_info_ptr->svn));
  if(get_euicc_info_ptr->cert_info.data_ptr)
  {
    /* Make sure we clean up the stale value from global before populating
       the new one */
    LPA_FREE(lpa_lpd_global_info[slot_id].cert_info.data_ptr);
    memset(&lpa_lpd_global_info[slot_id].cert_info,
           0x00,
           sizeof(lpa_lpd_global_info[slot_id].cert_info));

    lpa_lpd_global_info[slot_id].cert_info.data_ptr =
      LPA_MALLOC(get_euicc_info_ptr->cert_info.data_len);
    if(lpa_lpd_global_info[slot_id].cert_info.data_ptr == NULL)
    {
      return LPA_ERROR_HEAP_EXHAUSTED;
    }
    lpa_lpd_global_info[slot_id].cert_info.data_len =
      get_euicc_info_ptr->cert_info.data_len;
    memscpy(lpa_lpd_global_info[slot_id].cert_info.data_ptr,
            lpa_lpd_global_info[slot_id].cert_info.data_len,
            get_euicc_info_ptr->cert_info.data_ptr,
            get_euicc_info_ptr->cert_info.data_len);
  }
  /* At this point we have retrieved all of the following:
     1. SMDP address
     2. EUICC Challenge
     3. SVN
     4. Entire EUICC Info/Cert Info
     So, we have everything needed to send Initiate Auth request to SMDP via
     HTTP request. Trigger Initiate Auth request and move the state */
  lpa_status = lpa_state_change(LPA_STATE_INIT, LPA_STATE_INITIATE_AUTH_SENT, slot_id);
  if(lpa_status != LPA_SUCCESS)
  {
    LPA_FREE(lpa_lpd_global_info[slot_id].cert_info.data_ptr);
    LPA_FREE(lpa_lpd_global_info[slot_id].euicc_info.data_ptr);
    return lpa_status;
  }

  memset(&initiate_auth_req, 0x00, sizeof(lpa_lpd_initiate_auth_req_type));

  memscpy(initiate_auth_req.euicc_challenge,
          sizeof(initiate_auth_req.euicc_challenge),
          lpa_lpd_global_info[slot_id].euicc_challenge,
          sizeof(lpa_lpd_global_info[slot_id].euicc_challenge));
  initiate_auth_req.euicc_info = lpa_lpd_global_info[slot_id].euicc_info;
  initiate_auth_req.cert_info = lpa_lpd_global_info[slot_id].cert_info;
  initiate_auth_req.smdp_address = lpa_lpd_global_info[slot_id].activation_code.smdp_address;
  memscpy(initiate_auth_req.svn,
          sizeof(initiate_auth_req.svn),
          lpa_lpd_global_info[slot_id].svn,
          sizeof(lpa_lpd_global_info[slot_id].svn));
  initiate_auth_req.slot_id = slot_id;
  initiate_auth_req.lpd_callback = lpa_lpd_http_cb;

  return lpa_http_build_and_send_initiate_auth(&initiate_auth_req);

} /* lpa_lpd_process_get_euicc_info_resp */


/*===========================================================================
FUNCTION LPA_LPD_PROCESS_LOAD_BPP_RESP

DESCRIPTION
  Function processes the LOAD BPP response from the eUICC.
  If there is any error in this function, we must FREE all the BPP pointers
  stored in the global as part of GET BPP to SMDP earlier.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_load_bpp_resp (
  lpa_lpd_load_bpp_resp_type                *load_bpp_resp_ptr,
  lpa_slot_id_enum_type                      slot_id
)
{
  lpa_result_enum_type         lpa_status = LPA_SUCCESS;

  do
  {
    if(load_bpp_resp_ptr == NULL ||
       slot_id >= LPA_NUM_SLOTS)
    {
      lpa_status = LPA_GENERIC_ERROR;
      break;
    }

    /* Write the data received from card to EFS file for testing purposes */
    (void) lpa_efs_write(LPA_FILE_LOAD_BPP_FROM_CARD,
                         load_bpp_resp_ptr->load_bpp.data_len,
                         load_bpp_resp_ptr->load_bpp.data_ptr,
                         slot_id);

    if(load_bpp_resp_ptr->load_bpp.data_len > 0 &&
       NULL != load_bpp_resp_ptr->load_bpp.data_ptr)
    {
      lpa_lpd_handle_installation_result_req_type   profile_installation_result_req;
      /* Card sends a non-null response only after the last LOAD BPP TLV
         is processed by it. Since we received non-null response here, treat
         it as completion of load BPP process.
         This response contains Response and Receipt that needs to be sent to
         the SMDP via build and handle installation result */

      lpa_status = lpa_state_change(LPA_STATE_GET_BPP_RESP_RCVD,
                                    LPA_STATE_HANDLE_INSTALLATION_RESULT_SENT,
                                    slot_id);
      if(lpa_status != LPA_SUCCESS)
      {
        break;
      }
      memset(&profile_installation_result_req,
             0x00,
             sizeof(lpa_lpd_handle_installation_result_req_type));
      profile_installation_result_req.profile_installation_result = load_bpp_resp_ptr->load_bpp;
      profile_installation_result_req.slot_id = slot_id;
      profile_installation_result_req.lpd_callback = lpa_lpd_http_cb;
      lpa_status = lpa_http_build_and_send_handle_installation_result(
                     &profile_installation_result_req);
      if(LPA_SUCCESS != lpa_status)
      {
        /* Failed to build or send installation result to SMDP
           is considered as a failure on the profile installation
           procedure to the end user in our implementation, with
           the consideration that SMDP/operation might do some
           operations after receiving the installation report*/
        break; 
      }
      lpa_status = lpa_lpd_get_profile_installation_result(load_bpp_resp_ptr->load_bpp.data_len,
                                                           load_bpp_resp_ptr->load_bpp.data_ptr);
    }
    /* Check global and see if there are any pending BPP TLVs that need to be
       sent to the card. If so, send the STORE DATA */
    else if(lpa_lpd_global_info[slot_id].last_loaded_bpp_tlv_counter < lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_num)
    {
      /* Stays in LPA_STATE_GET_BPP_RESP_RCVD state */
      lpa_status = lpa_mmgsdi_store_data(slot_id,
                                         LPA_LOAD_BPP_APDU_DATA_P1,
                                         lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_data[lpa_lpd_global_info[slot_id].last_loaded_bpp_tlv_counter],
                                         TRUE,
                                         lpa_lpd_mmgsdi_cb,
                                         (void*) LPA_LPD_LOAD_BPP_RESP_CMD);
      lpa_lpd_global_info[slot_id].last_loaded_bpp_tlv_counter ++;
    }
    else
    {
      lpa_status = LPA_GENERIC_ERROR;
    }
  }
  while (0);

  if(lpa_status != LPA_SUCCESS)
  {
    uint8 i = 0;

    /* Free the global that contains the BPPs retrieved earlier from SMDP.
       We no longer need that global data as we are reporting error to
       the client (via an event) */
    while(lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_data[i].data_ptr &&
          i < lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_num)
    {
      LPA_FREE(lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_data[i].data_ptr);
      i++;
    }
  }

  return lpa_status;
} /* lpa_lpd_process_load_bpp_resp */


/*===========================================================================
FUNCTION LPA_LPD_PROCSES_HANDLE_INSTALLATION_RESULT_RESP

DESCRIPTION
  Function processes the http response from SMDP for Handle Profile
  Installation result and processes it.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_handle_installation_result_resp (
  lpa_slot_id_enum_type                           slot_id
)
{
  uint32                       i          = 0;
  lpa_result_enum_type         lpa_status = LPA_SUCCESS;

  if(slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  do
  {
    lpa_status = lpa_state_change(LPA_STATE_HANDLE_INSTALLATION_RESULT_SENT, LPA_STATE_IDLE, slot_id);
    if(lpa_status != LPA_SUCCESS)
    {
      break;
    }

    /* Call the LPA client event callback one last time to indicate that the
       installation is complete */
    lpa_send_profile_download_event(lpa_status,
                                    slot_id,
                                    0x64,  /* 100% */
                                    TRUE,
                                    TRUE);
  }
  while (0);


  /* Free the global that contains the BPPs retrieved earlier from SMDP.
     We no longer need that global data as we are reporting error to
     the client (via an event) */
  while(lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_data[i].data_ptr &&
        i < lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_num)
  {
    LPA_FREE(lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_data[i].data_ptr);
    i++;
  }

  return lpa_status;
} /* lpa_lpd_process_handle_installation_result_resp */


/*===========================================================================
FUNCTION LPA_LPD_PROCSES_GET_BPP_RESP

DESCRIPTION
  Function processes the http response from SMDP for GET BPP and processes it.
  The response received from SMDP is already in ASN1 format.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_get_bpp_resp (
  lpa_lpd_get_bpp_resp_type               *get_bpp_resp_ptr,
  lpa_slot_id_enum_type                    slot_id
)
{
  lpa_result_enum_type         lpa_status = LPA_SUCCESS;

  if(get_bpp_resp_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_lpd_global_info[slot_id].last_loaded_bpp_tlv_counter = 0;
  memset(&lpa_lpd_global_info[slot_id].bpp_tlvs, 0x00, sizeof(lpa_lpd_global_info[slot_id].bpp_tlvs));

   /* Write the data received from lpa_http to EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_JSON_DECODED_GET_BPP,
                       get_bpp_resp_ptr->bpp.data_len,
                       get_bpp_resp_ptr->bpp.data_ptr,
                       slot_id);

  /* We received the HTTP response for Get BPP which contains
     the profile info that needs to be sent in chunks to the card.
     The parsing function will store the parsed tlvs_num and tlvs_data
     to the global. From that point on, we need to sent separate STORE
     DATA commands for each of those tlvs */
  lpa_status = lpa_util_parse_bpp(get_bpp_resp_ptr->bpp,
                                  &lpa_lpd_global_info[slot_id].bpp_tlvs);

  if(lpa_status != LPA_SUCCESS ||
     lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_num == 0)
  {
    LPA_MSG_HIGH_1("Error parsing BPP, tlvs_num=0x%x",
                   lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_num);
    return LPA_GENERIC_ERROR;
  }

  lpa_status = lpa_state_change(LPA_STATE_GET_BPP_SENT, LPA_STATE_GET_BPP_RESP_RCVD, slot_id);

  lpa_status = lpa_mmgsdi_store_data(slot_id,
                                     LPA_LOAD_BPP_APDU_DATA_P1,
                                     lpa_lpd_global_info[slot_id].bpp_tlvs.tlvs_data[lpa_lpd_global_info->last_loaded_bpp_tlv_counter],
                                     TRUE,
                                     lpa_lpd_mmgsdi_cb,
                                     (void*) LPA_LPD_LOAD_BPP_RESP_CMD);
  if(lpa_status == LPA_SUCCESS)
  {
    lpa_lpd_global_info[slot_id].last_loaded_bpp_tlv_counter++;
  }

  return lpa_status;
} /* lpa_lpd_process_get_bpp_resp */


/*===========================================================================
FUNCTION LPA_LPD_PROCSES_INITIATE_AUTH_RESP

DESCRIPTION
  Function processes the http response for INITIATE AUTH and processes it.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_lpd_process_initiate_auth_resp (
  lpa_lpd_initiate_auth_resp_type            *initiate_auth_ptr,
  lpa_slot_id_enum_type                       slot_id
)
{
  lpa_result_enum_type                    lpa_status            = LPA_SUCCESS;
  lpa_data_type                           prepare_download_encoded;
  lpa_asn1_prepare_download_request_type  prepare_download_raw;

  if(initiate_auth_ptr == NULL ||
     slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  memset(&prepare_download_encoded, 0x00, sizeof(prepare_download_encoded));
  memset(&prepare_download_raw, 0x00, sizeof(prepare_download_raw));

  lpa_status = lpa_state_change(LPA_STATE_INITIATE_AUTH_SENT, LPA_STATE_INITIATE_AUTH_RESP_RCVD, slot_id);

  /* Save the transactionID in global for getBPP command */
  lpa_lpd_global_info[slot_id].transaction_id.data_len = initiate_auth_ptr->transaction_id.data_len;
  lpa_lpd_global_info[slot_id].transaction_id.data_ptr =
                                    (uint8 *)LPA_MALLOC(initiate_auth_ptr->transaction_id.data_len);
  if(NULL == lpa_lpd_global_info[slot_id].transaction_id.data_ptr)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }
  memscpy(lpa_lpd_global_info[slot_id].transaction_id.data_ptr,
          lpa_lpd_global_info[slot_id].transaction_id.data_len,
          initiate_auth_ptr->transaction_id.data_ptr,
          initiate_auth_ptr->transaction_id.data_len);

  /* We received the HTTP response for InitAuth which contains:
     transactionID
     dpSigned1
     smdpSignature1
     certFormatTobeUsed
     curveTobeUsed
     smdpCertificate.
     Call asn1 encoder to encode the PrepareDownload request... this function
     mallocs pointer inside prepare_download_data, so make sure to free it
     after sending STORE DATA to the card */
  prepare_download_raw.dp_signed1_raw = initiate_auth_ptr->dp_signed1;
  prepare_download_raw.smdp_signature1_raw = initiate_auth_ptr->smdp_signature1;
  prepare_download_raw.activation_code_token_raw.data_ptr =
    (uint8*) lpa_lpd_global_info[slot_id].activation_code.activation_token.data;
  prepare_download_raw.activation_code_token_raw.data_len =
    lpa_lpd_global_info[slot_id].activation_code.activation_token.data_len;
  prepare_download_raw.smdpoid_string =
    lpa_lpd_global_info[slot_id].activation_code.smdp_id;
  prepare_download_raw.hashed_confirmation_code_raw =
    lpa_lpd_global_info[slot_id].activation_code.hashed_confirmation_code;
  prepare_download_raw.cert_format_to_be_used_raw =
    initiate_auth_ptr->cert_format_to_be_used;
  prepare_download_raw.curve_to_be_used_raw = initiate_auth_ptr->curve_to_be_used;
  prepare_download_raw.smdp_certificate_raw = initiate_auth_ptr->smdp_certificate;
  lpa_status = lpa_util_get_device_info(&prepare_download_raw.device_info);
  if(lpa_status != LPA_SUCCESS)
  {
    LPA_MSG_HIGH_1("Error in lpa_util_get_device_info, status=0x%x",
                   lpa_status);
    return LPA_GENERIC_ERROR;
  }

  /* Following encoder may malloc prepare_download_encoded - so make sure
     to free it before exiting the function */
  lpa_status = lpa_asn1_encode_prepare_download_request(
                 prepare_download_raw, &prepare_download_encoded);

  /* Write the data received from DS to EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_ASN1_ENCODED_PREP_DOWNLOAD,
                       prepare_download_encoded.data_len,
                       prepare_download_encoded.data_ptr,
                       slot_id);

  /* Send the prepare download STORE DATA to the card */
  if(lpa_status == LPA_SUCCESS &&
     prepare_download_encoded.data_len > 0 &&
     prepare_download_encoded.data_ptr)
  {
    lpa_status = lpa_mmgsdi_store_data(slot_id,
                                       LPA_PREPARE_DOWNLOAD_APDU_DATA_P1,
                                       prepare_download_encoded,
                                       TRUE,
                                       lpa_lpd_mmgsdi_cb,
                                       (void*) LPA_LPD_PREPARE_DOWNLOAD_RESP_CMD);
  }
  else
  {
    LPA_MSG_HIGH_1("Error in lpa_asn1_encode_prepare_download_request, status=0x%x",
                   lpa_status);
  }

  LPA_FREE(prepare_download_encoded.data_ptr);

  return lpa_status;
} /* lpa_lpd_process_initiate_auth_resp */


/*===========================================================================
FUNCTION LPA_LPD_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, handles the processing of all the
  sub commands for achieving a profile download.

DEPENDENCIES
  LPA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_lpd_handle_cmd_sig (
  void
)
{
  lpa_lpd_response_data_type       *task_cmd_ptr  = NULL;
  lpa_result_enum_type              lpa_status    = LPA_GENERIC_ERROR;
  lpa_state_enum_type               lpa_state     = LPA_STATE_IDLE;

  task_cmd_ptr = (lpa_lpd_response_data_type*)(q_get(&lpa_lpd_cmd_q));

  (void) rex_clr_sigs(rex_self(), LPA_LPD_CMD_SIG);

  if(task_cmd_ptr == NULL)
  {
    return;
  }

  LPA_MSG_HIGH_2("LPA_LPD_CMD_SIG received for lpd command: 0x%x on slot_id: 0x%x",
                 task_cmd_ptr->cmd_data.cmd, task_cmd_ptr->slot_id);
  lpa_status = task_cmd_ptr->status;

  if (lpa_status == LPA_SUCCESS)
  {
    lpa_state = lpa_state_get_current(task_cmd_ptr->slot_id);

    switch(lpa_state)
    {
      case LPA_STATE_INIT:
        /* Trigger Initiate Auth */
        if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_GET_EUICC_CHALLENGE_RESP_CMD)
        {
          lpa_status = lpa_lpd_process_get_euicc_challenge_resp(
                         &task_cmd_ptr->cmd_data.data.get_euicc_challenge_resp,
                         task_cmd_ptr->slot_id);
        }
        else if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_GET_EUICC_INFO_RESP_CMD)
        {
          lpa_status = lpa_lpd_process_get_euicc_info_resp(
                         &task_cmd_ptr->cmd_data.data.get_euicc_info_resp,
                         task_cmd_ptr->slot_id);
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
        }
        break;

       case LPA_STATE_INITIATE_AUTH_SENT:
        if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_INITIATE_AUTH_RESP_CMD)
        {
          lpa_status = lpa_lpd_process_initiate_auth_resp(
                         &task_cmd_ptr->cmd_data.data.initiate_auth_resp,
                         task_cmd_ptr->slot_id);
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
        }
        break;

      case LPA_STATE_INITIATE_AUTH_RESP_RCVD:
        if (task_cmd_ptr->cmd_data.cmd == LPA_LPD_PREPARE_DOWNLOAD_RESP_CMD)
        {
          lpa_status = lpa_lpd_process_process_prepare_download_resp(
                         &task_cmd_ptr->cmd_data.data.prepare_download_resp,
                         task_cmd_ptr->slot_id);
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
        }
        break;

      case LPA_STATE_GET_BPP_SENT:
        if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_GET_BPP_RESP_CMD)
        {
          lpa_status = lpa_lpd_process_get_bpp_resp(
                         &task_cmd_ptr->cmd_data.data.get_bpp_resp,
                         task_cmd_ptr->slot_id);
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
        }
        break;

      case LPA_STATE_GET_BPP_RESP_RCVD:
        if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_LOAD_BPP_RESP_CMD)
        {
          lpa_status = lpa_lpd_process_load_bpp_resp(
                         &task_cmd_ptr->cmd_data.data.load_bpp_resp,
                         task_cmd_ptr->slot_id);
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
        }
        break;

      case LPA_STATE_HANDLE_INSTALLATION_RESULT_SENT:
        if(task_cmd_ptr->cmd_data.cmd == LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD)
        {
          /* ALL DONE. Call LPA client callback to indicate ADD PROFILE request
             is finished. Also clean up all the LPD related global data on the
             slot */
          lpa_status = lpa_lpd_process_handle_installation_result_resp(
                         task_cmd_ptr->slot_id);
        }
        else
        {
          lpa_status = LPA_GENERIC_ERROR;
        }
        break;

      default:
        lpa_status = LPA_GENERIC_ERROR;
        break;
    }
  }

  /* If the status is not successful, we must send the event back to the LPA
     client indicating that the add profile request is complete and has
     failed */
  if(lpa_status != LPA_SUCCESS)
  {
    LPA_MSG_HIGH_1("Error lpa_lpd_handle_cmd_sig, status=0x%x",
                   lpa_status);

    /* Send the event to LPA client and clear the timer */
    lpa_send_profile_download_event(lpa_status,
                                    task_cmd_ptr->slot_id,
                                    0x00,
                                    FALSE,
                                    TRUE);
  }

  if(lpa_state_get_current(task_cmd_ptr->slot_id) == LPA_STATE_IDLE)
  {
    lpa_reset(task_cmd_ptr->slot_id);

    /* Set the signal if there are one or more commands still in queue
       and LPA is currently in IDLE state. */
    lpa_cmd_set_sig();
  }

  lpa_lpd_free_response_data(&task_cmd_ptr->cmd_data);
  LPA_FREE(task_cmd_ptr);
} /* lpa_lpd_handle_cmd_sig */


/*===========================================================================
FUNCTION LPA_LPD_RETRIEVE_EUICC_PHASE

DESCRIPTION
  This function can be called to retrieve the phase conformance of the eUICC

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_lpd_retrieve_euicc_phase (
  lpa_slot_id_enum_type             slot_id,
  lpa_lpd_euicc_phase_enum_type    *phase_ptr
)
{
  lpa_result_enum_type              lpa_status = LPA_SUCCESS;

  if(slot_id > LPA_NUM_SLOTS ||
     phase_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  switch(lpa_lpd_global_info[slot_id].svn[0])
  {
    case 0x01:
      if(lpa_lpd_global_info[slot_id].svn[1] == 0x00)
      {
        *phase_ptr = LPA_LPD_PHASE1_EUICC;
      }
      else if(lpa_lpd_global_info[slot_id].svn[1] == 0x01)
      {
        *phase_ptr = LPA_LPD_PHASE1_1_EUICC;
      }
      else
      {
        lpa_status = LPA_GENERIC_ERROR;
      }
      break;
    case 0x02:
      *phase_ptr = LPA_LPD_PHASE2_EUICC;
      break;
    default:
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  return lpa_status;
} /* lpa_lpd_retrieve_euicc_phase */
