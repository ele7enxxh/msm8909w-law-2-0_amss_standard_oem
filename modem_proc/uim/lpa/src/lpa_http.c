/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            L P A   H T T P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the http functions for sending HTTP requests
  from LPA to the SMDP server.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_http.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/16   ar      Introduce DS http post retry timer logic
10/19/16   ar      Fix to support bigger profile (> 65535) download
10/19/16   ar      Add support for configuring HTTP stack via LPA EFS item
09/15/16   av      Fix protocol check for SMDP address
06/02/16   ll      Setting cmd_type to QMI_HTTP or DS_HTTP respectively
06/02/16   ll      Check for session_id only for DS_HTTP
05/26/16   ll      Fix qmi_uim_http_transaction_ptr memset after initialized
05/16/16   av      Route HTTP request to QMI HTTP
05/15/16   ll      HTTP Chunking
05/15/16   av      LPA should wait for DS session open cb before sending post
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "ds_http_api.h"
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include <stringl/stringl.h>
#include "string.h"
#include "lpa_platform.h"

#include "lpalib.h"
#include "lpa.h"
#include "lpa_lpd.h"
#include "lpa_http.h"
#include "lpa_util.h"
#include "lpa_json.h"
#include "lpa_efs.h"
#include "qmi_uim_http.h"
#include "uim_common_efs.h"
#include "lpa_timer.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define LPA_SMDP_ADDRESS_HTTP_PREFIX    "http://"
#define LPA_SMDP_ADDRESS_HTTPS_PREFIX   "https://"
#define LPA_HTTP_GET_BPP_MAX_RETRIES                    1
#define LPA_HTTP_HANDLE_INSTALLATION_RESULT_MAX_RETRIES 2

static q_type                           lpa_http_cmd_q;

typedef struct
{
  uint32                                request_id;
  lpa_smdp_address_type                 smdp_address;
  lpa_lpd_callback_type                 callback;
  uint32                                session_id;
  qmi_uim_http_service_registry_id_type qmi_http_service_id;
  sint15                                session_status;
  lpa_lpd_cmd_enum_type                 orig_lpd_cmd;
  lpa_data_type                         pending_post_data;
  char*                                 pending_post_url;
  lpa_data_type                         rsp_content;
  uint8                                 retry_count;
} lpa_http_data_type;

static lpa_http_data_type           lpa_http_global_data[LPA_NUM_SLOTS];

/*=============================================================================

                       FUNCTIONS

=============================================================================*/

/*===========================================================================
   FUNCTION:      LPA_HTTP_USE_QMI_HTTP

   DESCRIPTION:
     This function determines whether the HTTP stack on modem or on the AP side
     needs to be used for sending/receiving http data. It reads the LPA EFS item
     and if needed, the chip info to determine that.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
===========================================================================*/
static boolean lpa_http_use_qmi_http (
  void
)
{
  uint8                          lpa_support_nv_content[UIM_COMMON_LPA_EFS_ITEM_LEN];
  uim_common_efs_context_type    efs_context    = UIM_COMMON_EFS_CONTEXT_0;
  lpa_http_stack_usage_enum_type efs_http_stack = LPA_HTTP_USE_STACK_BASED_ON_CHIP_ID;

  memset(&lpa_support_nv_content, 0x00, sizeof(lpa_support_nv_content));

  /* Read new EFS item here to determine which http(s) stack to use:
     modem or AP */
  if ((uim_common_efs_read(UIM_COMMON_EFS_LPA_SUPPORT,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           efs_context,
                           (uint8 *)lpa_support_nv_content,
                           sizeof(lpa_support_nv_content)) == UIM_COMMON_EFS_SUCCESS))
  {
    efs_http_stack = lpa_support_nv_content[UIM_COMMON_LPA_EFS_ITEM_HTTP_STACK_INDEX];
  }

  switch(lpa_support_nv_content[UIM_COMMON_LPA_EFS_ITEM_HTTP_STACK_INDEX])
  {
    case 1:
      efs_http_stack = LPA_HTTP_USE_MODEM_STACK;
      break;
    case 2:
      efs_http_stack = LPA_HTTP_USE_AP_STACK;
      break;
    case 0:
    default:
      efs_http_stack = LPA_HTTP_USE_STACK_BASED_ON_CHIP_ID;
      break;
  }

  /* If NV's value is:
     2, always use AP http stack
     1, always use modem http stack
     0, decide http stack based on chip id */
  if(efs_http_stack == LPA_HTTP_USE_AP_STACK ||
     (efs_http_stack == LPA_HTTP_USE_STACK_BASED_ON_CHIP_ID &&
      lpa_chip_id == DALCHIPINFO_ID_MSM8909W))
  {
    return TRUE;
  }

  return FALSE;
} /* lpa_http_use_qmi_http */


/*===========================================================================
   FUNCTION:      LPA_HTTP_QUEUE_RSP_CMD

   DESCRIPTION:
     This function allows to queue HTTP responses back to LPA task to be
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
static lpa_result_enum_type  lpa_http_queue_rsp_cmd (
  lpa_http_rsp_type     *task_cmd_ptr
)
{
  if(task_cmd_ptr == NULL)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  LPA_UTIL_ENTER_TASK_STATE_CRIT_SECT;

  /* Do not queue command if LPA task has been stopped */
  if(lpa_task_stopped)
  {
    LPA_MSG_ERR_0("LPA task stopped, cannot queue command");
    LPA_UTIL_LEAVE_TASK_STATE_CRIT_SECT;
    return LPA_GENERIC_ERROR;
  }

  (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

  /* Put the message on the queue */
  q_put(&lpa_http_cmd_q, &task_cmd_ptr->link);

  /* Set the command queue signal */
  (void) rex_set_sigs(UIM_LPA_TCB, LPA_HTTP_RSP_SIG);

  LPA_UTIL_LEAVE_TASK_STATE_CRIT_SECT;
  return LPA_SUCCESS;
} /* lpa_http_queue_rsp_cmd */


/*===========================================================================
FUNCTION LPA_HTTP_QMI_HTTP_CB_FCN

DESCRIPTION
  This is the http callback function called by QMI HTTP, in order to post
  the http response back to LPA HTTP.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_qmi_http_cb_fcn (
  const qmi_uim_http_transaction_resp_data_type* response_info_ptr
)
{
  lpa_http_rsp_type                          *lpa_http_rsp_ptr = NULL;
  uint16                                      i                = 0;

  LPA_MSG_ERR_0("lpa_qmi_http_cb_fcn");

  /* post back response data to HTTP queue */
  lpa_http_rsp_ptr = LPA_MALLOC(sizeof(lpa_http_rsp_type));
  if (lpa_http_rsp_ptr == NULL)
  {
    return;
  }

  memset(lpa_http_rsp_ptr, 0x00, sizeof(lpa_http_rsp_type));

  if(NULL == response_info_ptr)
  {
    (void)lpa_http_queue_rsp_cmd(lpa_http_rsp_ptr);
    return;
  }

  do
  {
    lpa_http_rsp_ptr->cmd_type                            = LPA_HTTP_QMI_HTTP_RSP;
    lpa_http_rsp_ptr->data.qmi_http_rsp_data.service_id   = (qmi_uim_http_service_registry_id_type)response_info_ptr->client_ref_ptr;
    lpa_http_rsp_ptr->data.qmi_http_rsp_data.http_status  = response_info_ptr->result;
    lpa_http_rsp_ptr->data.qmi_http_rsp_data.content_size = response_info_ptr->payload_len;
    lpa_http_rsp_ptr->data.qmi_http_rsp_data.num_headers  = response_info_ptr->num_cust_headers;

    if(response_info_ptr->payload_len > 0)
    {
      lpa_http_rsp_ptr->data.qmi_http_rsp_data.content_ptr =
        LPA_MALLOC(sizeof(uint8) * (response_info_ptr->payload_len + 1));
      if(lpa_http_rsp_ptr->data.qmi_http_rsp_data.content_ptr == NULL)
      {
        break;
      }

      memscpy(lpa_http_rsp_ptr->data.qmi_http_rsp_data.content_ptr,
              lpa_http_rsp_ptr->data.qmi_http_rsp_data.content_size,
              (uint8 *)response_info_ptr->payload_ptr,
              response_info_ptr->payload_len);
    }

    if(response_info_ptr->num_cust_headers > 0)
    {
      lpa_http_rsp_ptr->data.rsp_data.header_info_ptr =
         LPA_MALLOC(sizeof(lpa_http_response_header_type) * response_info_ptr->num_cust_headers);

      if (lpa_http_rsp_ptr->data.rsp_data.header_info_ptr == NULL)
      {
        break;
      }

      for(i = 0; i < response_info_ptr->num_cust_headers; i++)
      {
        (void)strlcpy(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr[i].name,
                      response_info_ptr->headers[i].name_ptr,
                      strlen(response_info_ptr->headers[i].name_ptr + 1));
        (void)strlcpy(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr[i].value,
                      response_info_ptr->headers[i].value_ptr,
                      strlen(response_info_ptr->headers[i].value_ptr + 1));
      }
    }
  } while(0);

  (void)lpa_http_queue_rsp_cmd(lpa_http_rsp_ptr);
} /* lpa_http_qmi_http_cb_fcn */


/*===========================================================================
FUNCTION LPA_HTTP_CB_FCN

DESCRIPTION
  This is the http callback function called by DS, in order to post the http
  response back to LPA HTTP.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_cb_fcn (
  uint32                              session_id,
  uint32                              request_id,
  sint15                              error,
  const ds_http_response_info_s_type* response_info_ptr
)
{
  lpa_http_rsp_type         *lpa_http_rsp_ptr = NULL;
  uint32                     i                = 0;

  LPA_MSG_MED_1("lpa_http_cb_fcn; session_id=0x%x", session_id);

  /* post back response data to HTTP queue */
  lpa_http_rsp_ptr = LPA_MALLOC(sizeof(lpa_http_rsp_type));
  if (lpa_http_rsp_ptr == NULL)
  {
    return;
  }

  memset(lpa_http_rsp_ptr, 0x00, sizeof(lpa_http_rsp_type));
  lpa_http_rsp_ptr->data.rsp_data.session_id = session_id;
  lpa_http_rsp_ptr->data.rsp_data.request_id = request_id;
  lpa_http_rsp_ptr->data.rsp_data.error      = error;

  if(NULL == response_info_ptr)
  {
    (void)lpa_http_queue_rsp_cmd(lpa_http_rsp_ptr);
    return;
  }

  do
  {
    lpa_http_rsp_ptr->cmd_type                   = LPA_HTTP_RSP;
    lpa_http_rsp_ptr->data.rsp_data.http_status  = response_info_ptr->http_status;
    lpa_http_rsp_ptr->data.rsp_data.content_size = response_info_ptr->content_size;
    lpa_http_rsp_ptr->data.rsp_data.num_headers  = response_info_ptr->num_headers;

    if(response_info_ptr->content_size > 0)
    {
      lpa_http_rsp_ptr->data.rsp_data.content_ptr =
      LPA_MALLOC(sizeof(uint8) * (response_info_ptr->content_size + 1));
      if(lpa_http_rsp_ptr->data.rsp_data.content_ptr == NULL)
      {
        break;
      }

      memscpy(lpa_http_rsp_ptr->data.rsp_data.content_ptr,
              response_info_ptr->content_size,
              (uint8 *)response_info_ptr->content,
              response_info_ptr->content_size);
    }

    if(response_info_ptr->num_headers > 0)
    {
      lpa_http_rsp_ptr->data.rsp_data.header_info_ptr =
      LPA_MALLOC(sizeof(lpa_http_response_header_type) * response_info_ptr->num_headers);

      if(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr == NULL)
      {
        break;
      }

      for(i = 0; i < response_info_ptr->num_headers; i++)
      {
        (void)strlcpy(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr[i].name,
                      response_info_ptr->header_info[i].name,
                      sizeof(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr[i].name));
        (void)strlcpy(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr[i].value,
                      response_info_ptr->header_info[i].value,
                      sizeof(lpa_http_rsp_ptr->data.rsp_data.header_info_ptr[i].value));
      }
    }
  } while(0);

  (void)lpa_http_queue_rsp_cmd(lpa_http_rsp_ptr);
} /* lpa_http_cb_fcn */


/*===========================================================================
FUNCTION LPA_HTTP_BLOCK_CB_FCN

DESCRIPTION
  Callback function to notify block events that need input from the client.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_block_cb_fcn (
  uint32                         session_id,
  uint32                         request_id,
  ds_http_block_event_type       event_type,
  ds_http_block_event_info_type* event_info_ptr
)
{
  if (NULL == event_info_ptr)
  {
    return;
  }

  LPA_MSG_HIGH_3("lpa_http_block_cb_fcn - block event %d %d %d", session_id, request_id, event_type);

  switch(event_type)
  {
    case DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT:
    {
      /*
        Resolve SSL Certificate block by telling ds_http to either ignore the error
        and continue processing the request or abort and fail the request.
      */
      ds_http_resolve_ssl_cert_block(session_id, request_id, DS_HTTP_SSL_CONTINUE);
    }
    break;

    default:
      break;
  }
}/*lpa_http_block_cb_fcn*/


/*===========================================================================
FUNCTION LPA_HTTP_FREE_PENDING_POST_DATA

DESCRIPTION
  This function frees the pending_data global that is used for retrying the
  http post.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_free_pending_post_data (
  lpa_slot_id_enum_type                slot_id
)
{
  if(slot_id < LPA_NUM_SLOTS)
  {
    LPA_FREE(lpa_http_global_data[slot_id].pending_post_data.data_ptr);
    lpa_http_global_data[slot_id].pending_post_data.data_len = 0;
    LPA_FREE(lpa_http_global_data[slot_id].pending_post_url);
  }
} /* lpa_http_free_pending_post_data */


/*===========================================================================
FUNCTION LPA_HTTP_CREATE_PENDING_POST_DATA

DESCRIPTION
  This function copies data to the pending_data global that can be used
  for retrying the http post.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_http_create_pending_post_data (
  lpa_slot_id_enum_type                slot_id,
  const lpa_data_type                  *in_data_ptr,
  const char                           *url_address_ptr
)
{
  if(slot_id >= LPA_NUM_SLOTS || in_data_ptr == NULL ||
     url_address_ptr == NULL || in_data_ptr->data_ptr == NULL ||
     in_data_ptr->data_len == 0 || strlen(url_address_ptr) == 0)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  /* Make sure we start clean */
  lpa_http_free_pending_post_data(slot_id);

  lpa_http_global_data[slot_id].pending_post_data.data_ptr =
    (uint8*) LPA_MALLOC(in_data_ptr->data_len);
  if(lpa_http_global_data[slot_id].pending_post_data.data_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  lpa_http_global_data[slot_id].pending_post_url = (char*) LPA_MALLOC(strlen(url_address_ptr));
  if(lpa_http_global_data[slot_id].pending_post_url == NULL)
  {
    LPA_FREE(lpa_http_global_data[slot_id].pending_post_data.data_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  lpa_http_global_data[slot_id].pending_post_data.data_len = in_data_ptr->data_len;
  memscpy(lpa_http_global_data[slot_id].pending_post_data.data_ptr,
          lpa_http_global_data[slot_id].pending_post_data.data_len,
          in_data_ptr->data_ptr,
          in_data_ptr->data_len);

  memscpy(lpa_http_global_data[slot_id].pending_post_url,
          strlen(url_address_ptr),
          url_address_ptr,
          strlen(url_address_ptr));

  LPA_MSG_HIGH_0("lpa_http_create_pending_post_data SUCCESS");

  return LPA_SUCCESS;
} /* lpa_http_create_pending_post_data */


/*===========================================================================
FUNCTION LPA_HTTP_SET_RETRY_TIMER

DESCRIPTION
  This function sets the http retry timer that can be used for retrying the
  http post. The algorithm for retrying is like this:
  1. GetBPP or HandleProfileInstallation request post is attempted.
  2. Device loses connection (and hence DS returns a NETWORK_DOWN error
     to LPA).
  3. When handling that error from DS, LPA starts a n-minutes retry timer.
  4. If DS notifies LPA within those n-minutes that the connection is now
     available on the device side, and the timer is still not expired
     (and we have not reached max retry attempts), and there is a
     pending command, LPA re-sends the post.
  All along this, if the n-minutes retry timer expires, we clear the timer
  and in that case, no retry is attempted.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_http_set_retry_timer (
  lpa_slot_id_enum_type                slot_id
)
{
  lpa_result_enum_type         lpa_status  = LPA_SUCCESS;

  if(slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  LPA_MSG_HIGH_3("lpa_http_set_retry_timer: lpa_slot=0x%x; lpd_cmd=0x%x; retry_count=0x%x",
                 slot_id,
                 lpa_http_global_data[slot_id].orig_lpd_cmd,
                 lpa_http_global_data[slot_id].retry_count);

  if(lpa_http_global_data[slot_id].orig_lpd_cmd !=
       LPA_LPD_GET_BPP_RESP_CMD &&
     lpa_http_global_data[slot_id].orig_lpd_cmd !=
       LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD)
  {
    /* Retries are required only for the above commands. For any other
       command, do nothing*/
    return LPA_ERROR_NETWORK;
  }

  if(lpa_http_global_data[slot_id].retry_count == 0)
  {
    /* This is the first time the command is being requested to be retried...
       lets set the timer */
    if(lpa_http_global_data[slot_id].orig_lpd_cmd == LPA_LPD_GET_BPP_RESP_CMD)
    {
      lpa_timer_set(slot_id, LPA_TIMER_TYPE_GET_BPP_RETRY_TIMER);
    }
    else if(lpa_http_global_data[slot_id].orig_lpd_cmd == LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD)
    {
      lpa_timer_set(slot_id, LPA_TIMER_TYPE_HANDLE_INSTALLATION_RESULT_RETRY_TIMER);
    }
    lpa_http_global_data[slot_id].retry_count ++;
  }
  else if(lpa_http_global_data[slot_id].orig_lpd_cmd ==
            LPA_LPD_GET_BPP_RESP_CMD &&
          lpa_http_global_data[slot_id].retry_count <
            LPA_HTTP_GET_BPP_MAX_RETRIES)
  {
    lpa_http_global_data[slot_id].retry_count ++;
  }
  else if(lpa_http_global_data[slot_id].orig_lpd_cmd ==
            LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD &&
          lpa_http_global_data[slot_id].retry_count <
            LPA_HTTP_HANDLE_INSTALLATION_RESULT_MAX_RETRIES)
  {
    lpa_http_global_data[slot_id].retry_count ++;
  }
  else
  {
    /* Max retries for the command are over. Cannot retry any more.
       Return error. */
    lpa_status = LPA_ERROR_NETWORK;
  }

  return lpa_status;
} /* lpa_http_set_retry_timer */


/*===========================================================================
FUNCTION LPA_HTTP_CLEAR_RETRY_TIMER

DESCRIPTION
  This function clears the http retry timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_clear_retry_timer (
  lpa_slot_id_enum_type                slot_id
)
{
  if(slot_id >= LPA_NUM_SLOTS)
  {
    return;
  }

  LPA_MSG_HIGH_2("lpa_http_clear_retry_timer: lpa_slot=0x%x; lpd_cmd=0x%x",
                 slot_id, lpa_http_global_data[slot_id].orig_lpd_cmd);

  lpa_http_global_data[slot_id].retry_count = 0;
  if(lpa_http_global_data[slot_id].orig_lpd_cmd ==
       LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD)
  {
    lpa_timer_clear(slot_id,
                    LPA_TIMER_TYPE_HANDLE_INSTALLATION_RESULT_RETRY_TIMER);
  }
  else if(lpa_http_global_data[slot_id].orig_lpd_cmd ==
            LPA_LPD_GET_BPP_RESP_CMD)
  {
    lpa_timer_clear(slot_id,
                    LPA_TIMER_TYPE_GET_BPP_RETRY_TIMER);
  }
} /* lpa_http_clear_retry_timer */


/*===========================================================================
FUNCTION LPA_HTTP_HANDLE_HTTP_RETRY_TIMER_EXPIRE_SIG

DESCRIPTION
  This function, called in lpa_task, is called to process http retry timer
  expiration signal for a given slot.

DEPENDENCIES
  lpa_task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_http_handle_http_retry_timer_expire_sig (
  lpa_slot_id_enum_type               slot_id
)
{
  lpa_lpd_cmd_data_type  callback_data;
  uint32                 timer_sig = LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_1;

  memset(&callback_data, 0x00, sizeof(lpa_lpd_cmd_data_type));

  if(slot_id >= LPA_NUM_SLOTS)
  {
    return;
  }

  /* Send the response and clear the timer */
  LPA_MSG_HIGH_2("LPA_HTTP_RETRY_TIMER_EXPIRE_SIG: lpa_slot=0x%x, current lpd_cmd=0x%x",
                 slot_id, lpa_http_global_data[slot_id].orig_lpd_cmd);

  lpa_http_free_pending_post_data(slot_id);

  callback_data.cmd = lpa_http_global_data[slot_id].orig_lpd_cmd;

  if(lpa_http_global_data[slot_id].callback)
  {
    lpa_http_global_data[slot_id].callback(LPA_ERROR_NETWORK,
                                           &callback_data,
                                           slot_id);
  }

  if(slot_id == LPA_SLOT_1)
  {
    timer_sig = LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_1;
  }
  else if(slot_id == LPA_SLOT_2)
  {
    timer_sig = LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_2;
  }

  /* Clear the signal */
  (void)rex_clr_sigs(rex_self(), timer_sig);
} /* lpa_http_handle_http_retry_timer_expire_sig */


/*===========================================================================
FUNCTION LPA_HTTP_POST

DESCRIPTION
  This function posts the passed in JSON data via DS API

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  request_id

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_http_post(
  uint32                sessionID,
  const char*           smdpPlusAddress,
  const lpa_data_type*  json_data_ptr,
  lpa_slot_id_enum_type slot_id,
  uint32*               out_request_id_ptr
)
{
  sint15                      err  = 0;
  uint32                      request_id = DS_HTTP_ERROR;
  ds_http_header_s_type*      request_header_ptr;
  ds_http_content_info_s_type content_info;
  lpa_result_enum_type        lpa_status = LPA_ERROR_NETWORK;

  if(json_data_ptr == NULL  || out_request_id_ptr == NULL || slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_ERROR_INCORRECT_PARAMS;
  }

  request_header_ptr = (ds_http_header_s_type*) LPA_MALLOC(sizeof(ds_http_header_s_type));
  if(request_header_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  /* Configure request header */
  memset(request_header_ptr, 0, sizeof(ds_http_header_s_type));
  memscpy(request_header_ptr->content_type,
          DS_HTTP_HEADER_VALUE_MAX_LEN,
          "application/json",
          strlen("application/json")+1);
  request_header_ptr->num_cust_headers = 1;
  memscpy(request_header_ptr->cust_header_list[0].name,
          DS_HTTP_HEADER_NAME_MAX_LEN,
          "X-Admin-Protocol",
          strlen("X-Admin-Protocol")+1);
  memscpy(request_header_ptr->cust_header_list[0].value,
          DS_HTTP_HEADER_VALUE_MAX_LEN,
          "gsma/rsp1",
          strlen("gsma/rsp1")+1);

  /* Configure content info */
  memset(&content_info, 0, sizeof(ds_http_content_info_s_type));
  content_info.content      = json_data_ptr->data_ptr;
  content_info.content_size = json_data_ptr->data_len;

  request_id = ds_http_create_post_request(sessionID,
                                           smdpPlusAddress,
                                           request_header_ptr,
                                           &content_info,
                                           0,
                                           &err);
  if(DS_HTTP_ERROR == request_id)
  {
    LPA_MSG_ERR_1("ES9plusHTTPSPost DS_HTTP - Failed to create HTTP Post request %d", err);
    if(lpa_http_global_data[slot_id].orig_lpd_cmd !=
         LPA_LPD_GET_BPP_RESP_CMD &&
       lpa_http_global_data[slot_id].orig_lpd_cmd !=
         LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD)
    {
      LPA_FREE(request_header_ptr);
      return LPA_ERROR_NETWORK;
    }

    if(err == DS_HTTP_ERROR_NETDOWN)
    {
      /* For GET_BPP or HANDLE_PROFILE_INSTALLATION_RESULT, we shall retry as per
         the requirements if the failure was due to network being down on the
         device side  */
      lpa_status = lpa_http_set_retry_timer(slot_id);
    }
  }
  else
  {
    *out_request_id_ptr = request_id;
    lpa_status = LPA_SUCCESS;
  }

  LPA_FREE(request_header_ptr);

  return lpa_status;
}/* lpa_http_post */


/*===========================================================================
FUNCTION LPA_HTTP_QMI_HTTP_POST

DESCRIPTION
  This function post the passed in JSON data via QMI HTTP API

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  request_id

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_qmi_http_post(
  qmi_uim_http_service_registry_id_type  qmi_http_service_id,
  char*                                  smdp_plus_address_ptr,
  lpa_data_type*                         json_data_ptr
)
{
  char*                                   header_name_ptr             = "X-Admin-Protocol";
  char*                                   header_value_ptr            = "gsma/rsp1";
  lpa_result_enum_type                    lpa_status                  = LPA_SUCCESS;
  qmi_uim_http_transaction_req_data_type *qmi_uim_http_transaction_ptr = NULL;

  qmi_uim_http_transaction_ptr =
    (qmi_uim_http_transaction_req_data_type*) LPA_MALLOC(sizeof(qmi_uim_http_transaction_req_data_type));
  if(qmi_uim_http_transaction_ptr == NULL)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  /* Configure service ID */
  qmi_uim_http_transaction_ptr->service_id = qmi_http_service_id;

  /* Configure URL*/
  qmi_uim_http_transaction_ptr->url_ptr = smdp_plus_address_ptr;

  qmi_uim_http_transaction_ptr->header_ptr =
    (qmi_uim_http_transaction_header_info_type*) LPA_MALLOC(sizeof(qmi_uim_http_transaction_header_info_type));

  if(qmi_uim_http_transaction_ptr->header_ptr == NULL)
  {
    LPA_FREE(qmi_uim_http_transaction_ptr);
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  memscpy(qmi_uim_http_transaction_ptr->header_ptr->content_type,
          sizeof(qmi_uim_http_transaction_ptr->header_ptr->content_type),
          "application/json",
          strlen("application/json")+1);
  qmi_uim_http_transaction_ptr->header_ptr->num_headers = 1;

  qmi_uim_http_transaction_ptr->header_ptr->headers[0].name_ptr = header_name_ptr;
  qmi_uim_http_transaction_ptr->header_ptr->headers[0].value_ptr = header_value_ptr;

  /* Configure payload */
  qmi_uim_http_transaction_ptr->payload_len = json_data_ptr->data_len;
  qmi_uim_http_transaction_ptr->payload_ptr = json_data_ptr->data_ptr;

  /* Configure callback function */
  qmi_uim_http_transaction_ptr->cb_ptr = &lpa_http_qmi_http_cb_fcn;

  /* Configure the client ref ptr */
  qmi_uim_http_transaction_ptr->client_ref_ptr = (void*) qmi_http_service_id;

  /* Now call the QMI HTTP post API */
  if(QMI_UIM_HTTP_SUCCESS != qmi_uim_http_send_transaction_request(
                               qmi_uim_http_transaction_ptr))
  {
    LPA_MSG_ERR_0("ES9plusHTTPSPost QMI HTTP - Failed to create HTTP Post request %d");
    lpa_status = LPA_ERROR_NETWORK;
  }

  LPA_FREE(qmi_uim_http_transaction_ptr->header_ptr);
  LPA_FREE(qmi_uim_http_transaction_ptr);
  return lpa_status;
} /* lpa_http_qmi_http_post */


/*===========================================================================
FUNCTION LPA_HTTP_OPEN_SESSION

DESCRIPTION
  This function opens an HTTP session

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
   open session status

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_http_open_session(
  lpa_slot_id_enum_type       slot_id
)
{
  ds_http_iface_info_s_type   iface_info;

  if(lpa_http_use_qmi_http() == TRUE)
  {
    if(QMI_UIM_HTTP_SUCCESS != qmi_uim_http_service_register(
                                 &(lpa_http_global_data[slot_id].qmi_http_service_id)) ||
       lpa_http_global_data[slot_id].qmi_http_service_id < QMI_UIM_HTTP_SERVICE_ID_1 ||
       lpa_http_global_data[slot_id].qmi_http_service_id >= QMI_UIM_HTTP_MAX_SERVICE_ID)
    {
      LPA_MSG_ERR_1("Registration with QMI HTTP for slot=0x%x failed", slot_id);
      return LPA_ERROR_NETWORK;
    }
  }
  else
  {
    memset(&iface_info, 0, sizeof(ds_http_iface_info_s_type));

    if(ds_http_is_nw_srv_avail(DS_HTTP_IFACE_ANY))
    {
      iface_info.iface_type = DS_HTTP_IFACE_ANY;
      lpa_http_global_data[slot_id].session_id = ds_http_open_session_ex(lpa_http_cb_fcn,
                                                                         lpa_http_block_cb_fcn,
                                                                         &iface_info);
      if (DS_HTTP_ERROR == lpa_http_global_data[slot_id].session_id)
      {
        return LPA_ERROR_NETWORK;
      }
    }
    else if(ds_http_is_nw_srv_avail(DS_HTTP_IFACE_WLAN_LB))
    {
      iface_info.iface_type = DS_HTTP_IFACE_WLAN_LB;
      lpa_http_global_data[slot_id].session_id = ds_http_open_session_ex(lpa_http_cb_fcn,
                                                                         lpa_http_block_cb_fcn,
                                                                         &iface_info);
      if (DS_HTTP_ERROR == lpa_http_global_data[slot_id].session_id)
      {
        return LPA_ERROR_NETWORK;
      }
    }
    else
    {
      /* No service available on Modem DS. */
      return LPA_ERROR_NETWORK;
    }
  }
  return LPA_SUCCESS;
} /* lpa_http_open_session */


/*===========================================================================
FUNCTION LPA_HTTP_BUILD_SMDP_URL

DESCRIPTION
  This function is used to construct a complete URL to be used for posting
  each of the ES9plus requests to the SMDP server.
  When successful, it returns a NULL terminated URL.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_http_build_smdp_url (
  lpa_smdp_address_type                       smdp_address,
  const char*                                 smdp_url_path_ptr,
  char**                                      smdp_url_out_pptr
)
{
  uint16 temp_smdp_url_path_len = 0;
  boolean    add_protocol_prefix    = TRUE;
  uint8      https_prefix_len       = 0;

  if(smdp_address.data_len == 0 ||
     smdp_url_path_ptr == NULL ||
     smdp_url_out_pptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  if((smdp_address.data_len >= strlen(LPA_SMDP_ADDRESS_HTTP_PREFIX) &&
      memcmp(smdp_address.data,
             LPA_SMDP_ADDRESS_HTTP_PREFIX,
             strlen(LPA_SMDP_ADDRESS_HTTP_PREFIX)) == 0) ||
     (smdp_address.data_len >= strlen(LPA_SMDP_ADDRESS_HTTPS_PREFIX) &&
      memcmp(smdp_address.data,
             LPA_SMDP_ADDRESS_HTTPS_PREFIX,
             strlen(LPA_SMDP_ADDRESS_HTTPS_PREFIX)) == 0))
  {
    /* SMDP address already starts with either "http://" or "https://".
       So, no need to add the protocol again as prefix */
    add_protocol_prefix = FALSE;
  }

  if(add_protocol_prefix)
  {
    https_prefix_len = strlen(LPA_SMDP_ADDRESS_HTTPS_PREFIX);
  }

  temp_smdp_url_path_len = strlen(smdp_url_path_ptr);

  *smdp_url_out_pptr =
    (char*) LPA_MALLOC(https_prefix_len + smdp_address.data_len + temp_smdp_url_path_len + 1);
  if(NULL == *smdp_url_out_pptr)
  {
    return LPA_ERROR_HEAP_EXHAUSTED;
  }

  if(add_protocol_prefix)
  {
  memscpy(*smdp_url_out_pptr,
            https_prefix_len + smdp_address.data_len + temp_smdp_url_path_len,
            LPA_SMDP_ADDRESS_HTTPS_PREFIX,
            https_prefix_len);
  }

  memscpy(*smdp_url_out_pptr + https_prefix_len,
          smdp_address.data_len + temp_smdp_url_path_len,
          smdp_address.data,
          smdp_address.data_len);

  memscpy(*smdp_url_out_pptr + https_prefix_len + smdp_address.data_len,
          temp_smdp_url_path_len,
          smdp_url_path_ptr,
          temp_smdp_url_path_len);

  return LPA_SUCCESS;
} /* lpa_http_build_smdp_url */


/*===========================================================================
FUNCTION LPA_HTTP_BUILD_AND_SEND_INITIATE_AUTH

DESCRIPTION
  This function is used to send Initiate Auth request to SMDP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_build_and_send_initiate_auth (
  lpa_lpd_initiate_auth_req_type                *initiate_auth_req_ptr
)
{
  lpa_data_type             lpa_json_data;
  const char                url_cmd_path[]   = "/gsma/rsp1/es9plus/initiateAuthentication";
  lpa_result_enum_type      lpa_status       = LPA_GENERIC_ERROR;
  char*                     url_ptr          = NULL;

  memset(&lpa_json_data, 0x00, sizeof(lpa_json_data));

  if(initiate_auth_req_ptr == NULL ||
     initiate_auth_req_ptr->smdp_address.data_len == 0)
  {
    return LPA_GENERIC_ERROR;
  }

  /* We start off the AddProfile procedure fresh, that is, we must not have
     a ds session open at the time of processing initiate auth request */
  if(DS_HTTP_ERROR != lpa_http_global_data[initiate_auth_req_ptr->slot_id].session_id)
  {
    /* Simply return the error and don't worry about closing the ds session.
       It will automatically be closed */
    return LPA_GENERIC_ERROR;
  }

  /* Copy over the request data to the globals for later use */
  lpa_http_global_data[initiate_auth_req_ptr->slot_id].callback =
    initiate_auth_req_ptr->lpd_callback;
  lpa_http_global_data[initiate_auth_req_ptr->slot_id].smdp_address =
    initiate_auth_req_ptr->smdp_address;
  lpa_http_global_data[initiate_auth_req_ptr->slot_id].orig_lpd_cmd =
    LPA_LPD_INITIATE_AUTH_RESP_CMD;
  lpa_http_global_data[initiate_auth_req_ptr->slot_id].retry_count = 0;

  /* Construct the complete URL to be used for posting the request to the
     SMDP. This function mallocs url_ptr, so make sure to free it later */
  LPA_FREE(lpa_http_global_data[initiate_auth_req_ptr->slot_id].pending_post_url);
  LPA_FREE(lpa_http_global_data[initiate_auth_req_ptr->slot_id].pending_post_data.data_ptr);
  lpa_http_global_data[initiate_auth_req_ptr->slot_id].pending_post_data.data_len = 0;

  lpa_status = lpa_http_build_smdp_url (
                 lpa_http_global_data[initiate_auth_req_ptr->slot_id].smdp_address,
                 url_cmd_path,
                 &url_ptr);
  if(lpa_status != LPA_SUCCESS)
  {
    if(lpa_status != LPA_ERROR_HEAP_EXHAUSTED)
    {
      LPA_FREE(url_ptr);
    }
    return lpa_status;
  }

  LPA_MSG_SPRINTF_1("Sending Post for uri=%s", url_ptr);

  /* JSON encode the data that needs to be sent to the SMDP. This function
     could malloc lpa_json_data.data_ptr. So, make sure to free it when done */
  lpa_json_initiate_auth_encoder(&lpa_json_data, initiate_auth_req_ptr);

  /* Write the encoded content for EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_JSON_ENCODED_INIT_AUTH,
                       lpa_json_data.data_len,
                       lpa_json_data.data_ptr,
                       initiate_auth_req_ptr->slot_id);

  /* For DS http case, copy the encoded data pointer above to the pending data
     global so that the encoded data from the global can be used to send POST
     later once our callback for open session is called by DS. Once the
     response to the post is received from DS, the pending post data is free'd
     there.
     Note that for QMI HTTP case, we send POST right here, as there is no
     callback associated with QMI HTTP register service functionality. We
     don't need to copy the encoded data to the pending_post_data global in
     that case */
  if(!lpa_http_use_qmi_http())
  {
    lpa_status = lpa_http_create_pending_post_data(
                   initiate_auth_req_ptr->slot_id,
                   &lpa_json_data,
                   url_ptr);

  if(lpa_status != LPA_SUCCESS)
  {
      LPA_FREE(url_ptr);
      LPA_FREE(lpa_json_data.data_ptr);
      return lpa_status;
    }
  }

  lpa_status = lpa_http_open_session(initiate_auth_req_ptr->slot_id);
  if(lpa_status == LPA_SUCCESS &&
     lpa_http_use_qmi_http())
  {
    if (lpa_http_global_data[initiate_auth_req_ptr->slot_id].qmi_http_service_id ==
        QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE      ||
        lpa_http_global_data[initiate_auth_req_ptr->slot_id].qmi_http_service_id >=
        QMI_UIM_HTTP_MAX_SERVICE_ID)
    {
      lpa_status = LPA_ERROR_NETWORK;
    }
    else
    {
    /* For QMI HTTP case, we send POST right away, as there is no callback
         associated with QMI HTTP register service functionality. */
      lpa_status = lpa_http_qmi_http_post(
                     lpa_http_global_data[initiate_auth_req_ptr->slot_id].qmi_http_service_id,
                     url_ptr,
                                        &lpa_json_data);
    }
  }

  LPA_FREE(url_ptr);
  LPA_FREE(lpa_json_data.data_ptr);

  if(LPA_SUCCESS != lpa_status)
  {
    lpa_http_free_pending_post_data(initiate_auth_req_ptr->slot_id);
  }

  return lpa_status;
} /* lpa_http_build_and_send_initiate_auth */


/*===========================================================================
FUNCTION LPA_HTTP_BUILP_AND_SEND_GET_BPP

DESCRIPTION
  This function is used to send Get Bound Profile Package request to SMDP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_build_and_send_get_bpp (
  lpa_lpd_get_bpp_req_type                *get_bpp_req_ptr
)
{
  lpa_data_type lpa_json_data;
  const char                url_cmd_path[]   = "/gsma/rsp1/es9plus/getBoundProfilePackage";
  char*                     url_ptr          = NULL;
  lpa_result_enum_type      lpa_status       = LPA_GENERIC_ERROR;

  if(get_bpp_req_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_http_global_data[get_bpp_req_ptr->slot_id].callback = get_bpp_req_ptr->lpd_callback;
  lpa_http_global_data[get_bpp_req_ptr->slot_id].retry_count = 0;

  memset(&lpa_json_data, 0x00, sizeof(lpa_json_data));

  lpa_status = lpa_http_build_smdp_url (
                 lpa_http_global_data[get_bpp_req_ptr->slot_id].smdp_address,
                 url_cmd_path,
                 &url_ptr);
  if(lpa_status != LPA_SUCCESS)
  {
    if(lpa_status != LPA_ERROR_HEAP_EXHAUSTED)
    {
      LPA_FREE(url_ptr);
    }
    return lpa_status;
  }

  LPA_MSG_SPRINTF_1("Sending Post for uri=%s", url_ptr);

  lpa_http_global_data[get_bpp_req_ptr->slot_id].orig_lpd_cmd = LPA_LPD_GET_BPP_RESP_CMD;
  lpa_json_get_bound_profile_package_encoder(&lpa_json_data, get_bpp_req_ptr);

  /* Erite the encoded content for EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_JSON_ENCODED_GET_BPP,
                       lpa_json_data.data_len,
                       lpa_json_data.data_ptr,
                       get_bpp_req_ptr->slot_id);

  if(lpa_http_use_qmi_http())
  {
    if (lpa_http_global_data[get_bpp_req_ptr->slot_id].qmi_http_service_id ==
        QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE      ||
        lpa_http_global_data[get_bpp_req_ptr->slot_id].qmi_http_service_id >=
        QMI_UIM_HTTP_MAX_SERVICE_ID)
    {
      lpa_status = LPA_ERROR_NETWORK;
    }
    else
    {

    lpa_status = lpa_http_qmi_http_post(lpa_http_global_data[get_bpp_req_ptr->slot_id].qmi_http_service_id,
                                        url_ptr,
                                        &lpa_json_data);
    }
  }
  else
  {
    /* Session must be open already as part of Init Auth procedure! */
    if (DS_HTTP_ERROR == lpa_http_global_data[get_bpp_req_ptr->slot_id].session_id)
    {
      lpa_status = LPA_ERROR_NETWORK;
    }
    else
    {
      /* Copy the encoded data from above to the pending_post_data global that
         can be later used to retry the post incase the first post failed due
         to network down issue on the device side. This global on then free'd
         later if the network is back up before the retry timer expires, or it
         gets freed once the retry timer expires. */
      lpa_status = lpa_http_create_pending_post_data(get_bpp_req_ptr->slot_id,
                                                     &lpa_json_data,
                                                    (char*)url_ptr);
      if(LPA_SUCCESS == lpa_status)
      {
        lpa_http_global_data[get_bpp_req_ptr->slot_id].request_id = DS_HTTP_ERROR;
        lpa_status = lpa_http_post(
                       lpa_http_global_data[get_bpp_req_ptr->slot_id].session_id,
                        (char*)url_ptr,
                       &lpa_json_data,
                       get_bpp_req_ptr->slot_id,
                       &(lpa_http_global_data[get_bpp_req_ptr->slot_id].request_id));
      }

    if (DS_HTTP_ERROR == lpa_http_global_data[get_bpp_req_ptr->slot_id].request_id)
    {
        lpa_status = LPA_ERROR_NETWORK;
      }

      if(LPA_SUCCESS != lpa_status)
      {
        lpa_http_free_pending_post_data(get_bpp_req_ptr->slot_id);
      }
    }
  }

  LPA_FREE(lpa_json_data.data_ptr);
  LPA_FREE(url_ptr);

  return lpa_status;
} /* lpa_http_build_and_send_get_bpp */


/*===========================================================================
FUNCTION LPA_HTTP_BUILP_AND_SEND_HANDLE_INSTALLATION_RESULT

DESCRIPTION
  This function is used to send the Handle Installation Result to SMDP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_build_and_send_handle_installation_result (
  lpa_lpd_handle_installation_result_req_type         *profile_installation_result_req_ptr
)
{
  lpa_data_type lpa_json_data;
  const char*               url_cmd_path     = "/gsma/rsp1/es9plus/handleProfileInstallationResult";
  char*                     url_ptr          = NULL;
  lpa_result_enum_type      lpa_status       = LPA_GENERIC_ERROR;

  if(profile_installation_result_req_ptr == NULL)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_http_global_data[profile_installation_result_req_ptr->slot_id].callback = profile_installation_result_req_ptr->lpd_callback;
  lpa_http_global_data[profile_installation_result_req_ptr->slot_id].retry_count = 0;

  memset(&lpa_json_data, 0x00, sizeof(lpa_json_data));

  lpa_status = lpa_http_build_smdp_url (
                 lpa_http_global_data[profile_installation_result_req_ptr->slot_id].smdp_address,
                 url_cmd_path,
                 &url_ptr);
  if(lpa_status != LPA_SUCCESS)
  {
    if(lpa_status != LPA_ERROR_HEAP_EXHAUSTED)
    {
      LPA_FREE(url_ptr);
    }
    return lpa_status;
  }

  LPA_MSG_SPRINTF_1("Sending Post for uri=%s", url_ptr);

  lpa_http_global_data[profile_installation_result_req_ptr->slot_id].orig_lpd_cmd = LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD;
  lpa_json_handle_profile_install_result_encoder(&lpa_json_data, profile_installation_result_req_ptr);

  /* Write the encoded content for EFS file for testing purposes */
  (void) lpa_efs_write(LPA_FILE_JSON_ENCODED_HANDLE_INST_RSLT,
                       lpa_json_data.data_len,
                       lpa_json_data.data_ptr,
                       profile_installation_result_req_ptr->slot_id);

  if(lpa_http_use_qmi_http())
  {
    if (lpa_http_global_data[profile_installation_result_req_ptr->slot_id].qmi_http_service_id ==
        QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE      ||
        lpa_http_global_data[profile_installation_result_req_ptr->slot_id].qmi_http_service_id >=
        QMI_UIM_HTTP_MAX_SERVICE_ID)
    {
      lpa_status = LPA_ERROR_NETWORK;
    }
    else
    {
      lpa_status = lpa_http_qmi_http_post(
                     lpa_http_global_data[profile_installation_result_req_ptr->slot_id].qmi_http_service_id,
                                        url_ptr,
                                        &lpa_json_data);
    }
  }
  else
  {
    /* Session must be open already as part of Init Auth procedure! */
    if(DS_HTTP_ERROR ==
         lpa_http_global_data[profile_installation_result_req_ptr->slot_id].session_id)
    {
      lpa_status = LPA_ERROR_NETWORK;
    }
    else
    {
      /* Copy the encoded data from above to the pending_post_data global that
         can be later used to retry the post incase the first post failed due
         to network down issue on the device side. This global on then free'd
         later if the network is back up before the retry timer expires, or it
         gets freed once the retry timer expires. */
      lpa_status = lpa_http_create_pending_post_data(
                     profile_installation_result_req_ptr->slot_id,
                     &lpa_json_data,
                     url_ptr);
      if(LPA_SUCCESS == lpa_status)
      {
        lpa_http_global_data[profile_installation_result_req_ptr->slot_id].request_id = DS_HTTP_ERROR;
        lpa_status = lpa_http_post(
                       lpa_http_global_data[profile_installation_result_req_ptr->slot_id].session_id,
                        (char*)url_ptr,
                       &lpa_json_data,
                       profile_installation_result_req_ptr->slot_id,
                       &(lpa_http_global_data[profile_installation_result_req_ptr->slot_id].request_id));
      }

    if (DS_HTTP_ERROR == lpa_http_global_data[profile_installation_result_req_ptr->slot_id].request_id)
    {
        lpa_status = LPA_ERROR_NETWORK;
    }

      if(LPA_SUCCESS != lpa_status)
      {
        lpa_http_free_pending_post_data(profile_installation_result_req_ptr->slot_id);
  }
    }
  }

  LPA_FREE(lpa_json_data.data_ptr);
  LPA_FREE(url_ptr);

  return lpa_status;
} /* lpa_http_build_and_send_handle_installation_result */


/*===========================================================================
FUNCTION LPA_HTTP_CONVERT_DS_STATUS_TO_LPA_STATUS

DESCRIPTION
  This function converts the DS error type to lpa error type

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_convert_ds_status_to_lpa_status (
  sint15                     ds_error
)
{
  lpa_result_enum_type          lpa_status = LPA_GENERIC_ERROR;

  switch(ds_error)
  {
    case DS_HTTP_STATUS_OK:
      lpa_status = LPA_SUCCESS;
      break;

    case DS_HTTP_ERROR_NETWORK_DOWN:
      lpa_status = LPA_ERROR_NETWORK;
      break;

    case DS_HTTP_ERROR_SSL:
      lpa_status = LPA_ERROR_NETWORK_SSL;
      break;

    case DS_HTTP_ERROR_DNS:
      lpa_status = LPA_ERROR_NETWORK_DNS;
      break;

    case DS_HTTP_ERROR_SOCKET:
      lpa_status = LPA_ERROR_NETWORK_SOCKET;
      break;

    case DS_HTTP_ERROR_TIMEOUT:
      lpa_status = LPA_ERROR_COMMAND_TIMEOUT;
      break;
    default:
      break;
  }

  return lpa_status;
} /* lpa_http_convert_ds_status_to_lpa_status */


/*===========================================================================
FUNCTION LPA_HTTP_CONVERT_QMI_HTTP_STATUS_TO_LPA_STATUS

DESCRIPTION
  This function converts the QMI HTTP error type to lpa error type

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static lpa_result_enum_type lpa_http_convert_qmi_http_status_to_lpa_status (
  qmi_uim_http_transaction_result_type      qmi_http_srv_status
)
{
  lpa_result_enum_type          lpa_status = LPA_GENERIC_ERROR;

  switch(qmi_http_srv_status)
  {
    case QMI_UIM_HTTP_TRANSACTION_SUCCESS:
      lpa_status = LPA_SUCCESS;
      break;

    case QMI_UIM_HTTP_SERVER_ERROR:
    case QMI_UIM_HTTP_NETWORK_ERROR:
      lpa_status = LPA_ERROR_NETWORK;
      break;

    case QMI_UIM_HTTP_TLS_ERROR:
      lpa_status = LPA_ERROR_NETWORK_SSL;
      break;
    default:
      break;
  }

  return lpa_status;
} /* lpa_http_convert_qmi_http_status_to_lpa_status */


/*===========================================================================
FUNCTION LPA_HTTP_PROCESS_RSP

DESCRIPTION
  This function process HTTP response from DS

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_process_rsp (
  lpa_http_response_data_type  rsp_data
)
{
  lpa_lpd_cmd_data_type                callback_data;
  lpa_slot_id_enum_type                slot_id          = LPA_SLOT_1;
  lpa_result_enum_type                 lpa_status       = LPA_GENERIC_ERROR;
  lpa_data_type                        lpa_json_data;
  uint8*                               temp_content_ptr = NULL;
  ds_http_session_config_value_u_type  session_config_value;
  uint32                               remaining_time   = 0;

  memset(&callback_data, 0x00, sizeof(lpa_lpd_cmd_data_type));
  memset(&lpa_json_data, 0x00, sizeof(lpa_data_type));

  /* Process response and send response back to LPD, by calling its callback stored
     in our global.
     For proper routing, we need to find out the slot which is response belongs to.
     That mapping can be done based on the request id */
  for(slot_id = LPA_SLOT_1; slot_id < LPA_NUM_SLOTS; slot_id++)
  {
    if(lpa_http_global_data[slot_id].session_id == rsp_data.session_id &&
       lpa_http_global_data[slot_id].callback != NULL)
    {
      break;
    }
  }

  if(slot_id == LPA_NUM_SLOTS)
  {
    LPA_MSG_ERR_0("Catastrophic Error! LPD will never be notified of this error.");
    return;
  }

  if(rsp_data.error == DS_HTTP_ERROR_NETWORK_DOWN &&
     LPA_SUCCESS == lpa_http_set_retry_timer(slot_id))
  {
    /* http post failed because network is down. Lets set the timer and retry
       the post once network is available and the retry counter has still
       not expired by then. Retry is performed only for GET_BPP and
       HANDLE_INSTALLATION_RESULT procedures */
    return;
  }

  callback_data.cmd = lpa_http_global_data[slot_id].orig_lpd_cmd;

  if(rsp_data.error == DS_HTTP_READY &&
     lpa_http_global_data[slot_id].pending_post_data.data_ptr &&
     lpa_http_global_data[slot_id].orig_lpd_cmd == LPA_LPD_INITIATE_AUTH_RESP_CMD)
  {
    /* We are here because we had attempted a DS open session as part of
       handling initiate auth request from LPA LPD. DS returned DS_HTTP_READY
       indicating the session is ready to be used for sending http POST */
    LPA_MSG_SPRINTF_1("DS session is now open. Sending pending post for uri=%s",
                      lpa_http_global_data[slot_id].pending_post_url);

    /* Config DS session for chunking */
    memset(&session_config_value, 0, sizeof(ds_http_session_config_value_u_type));
    session_config_value.forward_oversize_response = TRUE;
    ds_http_set_session_config(lpa_http_global_data[slot_id].session_id,
                               DS_HTTP_SESSION_CONFIG_FORWARD_OVERSIZE_RESPONSE,
                               &session_config_value);
  }

  if(rsp_data.error == DS_HTTP_READY &&
     lpa_http_global_data[slot_id].pending_post_data.data_ptr &&
     (lpa_http_global_data[slot_id].orig_lpd_cmd == LPA_LPD_INITIATE_AUTH_RESP_CMD ||
      (lpa_http_global_data[slot_id].orig_lpd_cmd == LPA_LPD_GET_BPP_RESP_CMD &&
       lpa_http_global_data[slot_id].retry_count <= LPA_HTTP_GET_BPP_MAX_RETRIES &&
       LPA_SUCCESS == lpa_timer_get(slot_id,
                                    LPA_TIMER_TYPE_GET_BPP_RETRY_TIMER,
                                    &remaining_time) &&
       remaining_time > 0) ||
      (lpa_http_global_data[slot_id].orig_lpd_cmd == LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD &&
       lpa_http_global_data[slot_id].retry_count <= LPA_HTTP_HANDLE_INSTALLATION_RESULT_MAX_RETRIES &&
       LPA_SUCCESS == lpa_timer_get(slot_id,
                                    LPA_TIMER_TYPE_HANDLE_INSTALLATION_RESULT_RETRY_TIMER,
                                    &remaining_time) &&
       remaining_time > 0)))
  {
    /* DS returned DS_HTTP_READY. It means that we have a valid session with
       DS now. We could be here either due to first time opening of session
       with DS or due to network reconnection, in which case this is a RETRY.
       POST the request to SMDP */
    lpa_status = lpa_http_post(
                  lpa_http_global_data[slot_id].session_id,
                    lpa_http_global_data[slot_id].pending_post_url,
                  &(lpa_http_global_data[slot_id].pending_post_data),
                  slot_id,
                  &(lpa_http_global_data[slot_id].request_id));

    if(lpa_status != LPA_SUCCESS ||
       DS_HTTP_ERROR == lpa_http_global_data[slot_id].request_id)
    {
      /* Notify the client that the post could not be sent to SMDP
         successfully */
      lpa_http_free_pending_post_data(slot_id);
      lpa_http_clear_retry_timer(slot_id);
      if(lpa_http_global_data[slot_id].callback)
    {
      lpa_http_global_data[slot_id].callback(LPA_ERROR_NETWORK,
                                             &callback_data,
                                             slot_id);
    }
    }
    return;
  }

  lpa_http_free_pending_post_data(slot_id);
  lpa_http_clear_retry_timer(slot_id);

  LPA_MSG_HIGH_3("DS HTTP response rsp_data.error=0x%x received for LPD request=0x%x from slot_id=0x%x",
                 rsp_data.error, lpa_http_global_data[slot_id].orig_lpd_cmd, slot_id);

  /* We are here because we received response from SMDP/DS for
     some prior POST_HTTP request we sent to them */
  switch(lpa_http_global_data[slot_id].orig_lpd_cmd)
  {
    case LPA_LPD_INITIATE_AUTH_RESP_CMD:
      callback_data.data.initiate_auth_resp.http_response = rsp_data.http_status;
      break;
    case LPA_LPD_GET_BPP_RESP_CMD:
      callback_data.data.get_bpp_resp.http_response = rsp_data.http_status;
      break;
    case LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD:
      callback_data.data.handle_installation_result_resp.http_response = rsp_data.http_status;
      break;
    default:
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  lpa_status = lpa_http_convert_ds_status_to_lpa_status(rsp_data.error);

  /* Partial http chunk received */
  /* Malloc buffer and append new data */
  LPA_MSG_HIGH_2("Got partial http chunk of size 0x%x, total received size 0x%x",
                  rsp_data.content_size,
                  (lpa_http_global_data[slot_id].rsp_content.data_len + rsp_data.content_size));
  if ( 0 != rsp_data.content_size )
  {
    if ( lpa_http_global_data[slot_id].rsp_content.data_len + rsp_data.content_size
         < LPA_MAX_PROFILE_SIZE )
    {
      temp_content_ptr = (uint8*)LPA_MALLOC( lpa_http_global_data[slot_id].rsp_content.data_len +
                                             rsp_data.content_size );
      if ( NULL != temp_content_ptr )
      {
        if ( NULL != lpa_http_global_data[slot_id].rsp_content.data_ptr )
        {
          memscpy(temp_content_ptr,
                  lpa_http_global_data[slot_id].rsp_content.data_len + rsp_data.content_size,
                  lpa_http_global_data[slot_id].rsp_content.data_ptr,
                  lpa_http_global_data[slot_id].rsp_content.data_len);
        }

        memscpy(temp_content_ptr + lpa_http_global_data[slot_id].rsp_content.data_len,
                rsp_data.content_size,
                rsp_data.content_ptr,
                rsp_data.content_size);
        LPA_MSG_HIGH_0("Appended chunk");
        LPA_FREE(lpa_http_global_data[slot_id].rsp_content.data_ptr);
        lpa_http_global_data[slot_id].rsp_content.data_len += rsp_data.content_size;
        lpa_http_global_data[slot_id].rsp_content.data_ptr = temp_content_ptr;
        if ( DS_HTTP_NOTIFY_PARTIAL_BODY == rsp_data.error )
        {
          LPA_MSG_HIGH_0("Partial chunk received, return");
          return;
        }
        else if ( DS_HTTP_STATUS_OK == rsp_data.error )
        {
          LPA_MSG_HIGH_0("Full http rsp received, continue parsing");
          lpa_json_data.data_ptr = lpa_http_global_data[slot_id].rsp_content.data_ptr;
          lpa_json_data.data_len = lpa_http_global_data[slot_id].rsp_content.data_len;
        }
      }
      else
      {
        LPA_MSG_ERR_0("Failed to malloc for http response chunk");
        lpa_status = LPA_ERROR_HEAP_EXHAUSTED;
      }
    }
    else
    {
      LPA_MSG_ERR_1("HTTP data content exceed Max size = %d",
                    lpa_http_global_data[slot_id].rsp_content.data_len);
      LPA_FREE(lpa_http_global_data[slot_id].rsp_content.data_ptr);
      lpa_http_global_data[slot_id].rsp_content.data_len = 0;
      lpa_status = LPA_GENERIC_ERROR;
    }
  }

  if((rsp_data.error != DS_HTTP_STATUS_OK &&
      rsp_data.error              != DS_HTTP_ERROR_HTTP_HEADER &&
      DS_HTTP_NOTIFY_PARTIAL_BODY != rsp_data.error) ||
      lpa_status != LPA_SUCCESS)
  {
    /* callback with error  */
    LPA_MSG_ERR_3("error 0x%x, http_status 0x%x, content_size 0x%x",
                  rsp_data.error, rsp_data.http_status, rsp_data.content_size);

    /* Call LPD callback with error */
    if(lpa_http_global_data[slot_id].callback)
    {
    (lpa_http_global_data[slot_id].callback)(lpa_status,
                                             &callback_data,
                                             slot_id);
    }

    /* clean up */
    LPA_FREE(lpa_http_global_data[slot_id].rsp_content.data_ptr);
    lpa_http_global_data[slot_id].rsp_content.data_len = 0;
    return;
  }

  /* HTTP GET successfully completed. Call the appropriate response handlers */
  switch (lpa_http_global_data[slot_id].orig_lpd_cmd)
  {
    case LPA_LPD_INITIATE_AUTH_RESP_CMD:
      if(rsp_data.content_size > 0)
      {
        /* Write the data received from DS to EFS file for testing purposes */
        (void) lpa_efs_write(LPA_FILE_JSON_INIT_AUTH_FROM_DS,
                             lpa_json_data.data_len,
                             lpa_json_data.data_ptr,
                             slot_id);
        lpa_status = lpa_json_initiate_auth_decoder(&lpa_json_data, &callback_data.data.initiate_auth_resp);
      }
      else
      {
        lpa_status = LPA_GENERIC_ERROR;
      }
      break;
    case LPA_LPD_GET_BPP_RESP_CMD:
      if(rsp_data.content_size > 0)
      {
        /* Write the data received from DS to EFS file for testing purposes */
        (void) lpa_efs_write(LPA_FILE_JSON_GET_BPP_FROM_DS,
                             lpa_json_data.data_len,
                             lpa_json_data.data_ptr,
                             slot_id);
        lpa_status = lpa_json_get_bound_profile_package_decoder(&lpa_json_data, &callback_data.data.get_bpp_resp);
      }
      else
      {
        lpa_status = LPA_GENERIC_ERROR;
      }
      break;
    case LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD:
      /* We don't receive any response for this */
      lpa_status = LPA_SUCCESS;
      break;
    default:
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  /* Call the LPD callback to inform it about the HTTP response from SMDP */
  if(lpa_http_global_data[slot_id].callback)
  {
    lpa_http_global_data[slot_id].callback(lpa_status,
                                           &callback_data,
                                           slot_id);
  }
  else
  {
    LPA_MSG_ERR_0("lpa_http: NULL callback - unexpected!");
  }
  LPA_FREE(lpa_http_global_data[slot_id].rsp_content.data_ptr);
  lpa_http_global_data[slot_id].rsp_content.data_len = 0;
  lpa_lpd_free_response_data(&callback_data);
} /* lpa_http_process_rsp */


/*===========================================================================
FUNCTION LPA_HTTP_PROCESS_QMI_HTTP_RSP

DESCRIPTION
  This function process HTTP response from QMI HTTP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_http_process_qmi_http_rsp (
  lpa_http_qmi_http_response_data_type  rsp_data
)
{
  lpa_lpd_cmd_data_type            callback_data;
  lpa_slot_id_enum_type            slot_id        = LPA_SLOT_1;
  lpa_result_enum_type             lpa_status     = LPA_SUCCESS;
  lpa_data_type                    lpa_json_data;

  memset(&callback_data, 0x00, sizeof(lpa_lpd_cmd_data_type));
  memset(&lpa_json_data, 0x00, sizeof(lpa_data_type));

  /* Process response and send response back to LPD, by calling its callback stored
     in our global.
     For proper routing, we need to find out the slot which is response belongs to.
     That mapping can be done based on the request id */
  for(slot_id = LPA_SLOT_1; slot_id < LPA_NUM_SLOTS; slot_id++)
  {
    if(lpa_http_global_data[slot_id].qmi_http_service_id == rsp_data.service_id &&
       lpa_http_global_data[slot_id].callback != NULL)
    {
      break;
    }
  }

  if(slot_id == LPA_NUM_SLOTS)
  {
    LPA_MSG_ERR_0("Catastrophic Error! LPD will never be notified of this error.");
    return;
  }

  callback_data.cmd = lpa_http_global_data[slot_id].orig_lpd_cmd;

  LPA_MSG_HIGH_3("QMI HTTP response rsp_data.http_status=0x%x received for LPD request=0x%x from slot_id=0x%x",
                 rsp_data.http_status, lpa_http_global_data[slot_id].orig_lpd_cmd, slot_id);

  /* We are here because we received response from SMDP/SD for
     some prior POST_HTTP request we sent to them */
  switch(lpa_http_global_data[slot_id].orig_lpd_cmd)
  {
    case LPA_LPD_INITIATE_AUTH_RESP_CMD:
      callback_data.data.initiate_auth_resp.http_response = (uint32) rsp_data.http_status;
      break;
    case LPA_LPD_GET_BPP_RESP_CMD:
      callback_data.data.get_bpp_resp.http_response = (uint32) rsp_data.http_status;
      break;
    case LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD:
      callback_data.data.handle_installation_result_resp.http_response = (uint32) rsp_data.http_status;
      break;
    default:
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  lpa_status = lpa_http_convert_qmi_http_status_to_lpa_status(rsp_data.http_status);

  if(rsp_data.http_status != QMI_UIM_HTTP_TRANSACTION_SUCCESS)
  {
    /* callback with error */
    LPA_MSG_ERR_2("http_status 0x%x, content_size 0x%x",
                  rsp_data.http_status, rsp_data.content_size);

    /* Call LPD callback with error */
    (lpa_http_global_data[slot_id].callback)(LPA_ERROR_NETWORK,
                                             &callback_data,
                                             slot_id);

    return;
  }

  lpa_json_data.data_ptr = rsp_data.content_ptr;
  lpa_json_data.data_len = rsp_data.content_size;
  /* HTTP GET successfully completed. Call the appropriate response handlers */
  switch (lpa_http_global_data[slot_id].orig_lpd_cmd)
  {
    case LPA_LPD_INITIATE_AUTH_RESP_CMD:
      if(rsp_data.content_size > 0)
      {
        /* Write the data received from DS to EFS file for testing purposes */
        (void) lpa_efs_write(LPA_FILE_JSON_INIT_AUTH_FROM_DS,
                             lpa_json_data.data_len,
                             lpa_json_data.data_ptr,
                             slot_id);
        lpa_status = lpa_json_initiate_auth_decoder(&lpa_json_data, &callback_data.data.initiate_auth_resp);
      }
      else
      {
        lpa_status = LPA_GENERIC_ERROR;
      }
      break;
    case LPA_LPD_GET_BPP_RESP_CMD:
      if(rsp_data.content_size > 0)
      {
        /* Write the data received from DS to EFS file for testing purposes */
        (void) lpa_efs_write(LPA_FILE_JSON_GET_BPP_FROM_DS,
                             lpa_json_data.data_len,
                             lpa_json_data.data_ptr,
                             slot_id);
        lpa_status = lpa_json_get_bound_profile_package_decoder(&lpa_json_data, &callback_data.data.get_bpp_resp);
      }
      else
      {
        lpa_status = LPA_GENERIC_ERROR;
      }
      break;
    case LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD:
      /* We don't receive any response for this */
      lpa_status = LPA_SUCCESS;
      break;
    default:
      lpa_status = LPA_GENERIC_ERROR;
      break;
  }

  /* Call the LPD callback to inform it about the HTTP response from SMDP */
  if(lpa_http_global_data[slot_id].callback)
  {
    lpa_http_global_data[slot_id].callback(lpa_status,
                                           &callback_data,
                                           slot_id);
  }
  else
  {
    LPA_MSG_ERR_0("lpa_http: NULL callback - unexpected!");
  }
  lpa_lpd_free_response_data(&callback_data);
} /* lpa_http_process_qmi_http_rsp */


/*===========================================================================
FUNCTION LPA_HTTP_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, is called to process response from DS

DEPENDENCIES
  LPA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_http_handle_cmd_sig (
  void
)
{
  lpa_http_rsp_type          *task_cmd_ptr  = NULL;

  task_cmd_ptr = (lpa_http_rsp_type*)(q_get(&lpa_http_cmd_q));

  if(task_cmd_ptr != NULL)
  {
    LPA_MSG_HIGH_0("LPA_HTTP_RSP_SIG received");

    switch (task_cmd_ptr->cmd_type)
    {
      case LPA_HTTP_RSP:
        lpa_http_process_rsp(task_cmd_ptr->data.rsp_data);
        LPA_FREE(task_cmd_ptr->data.rsp_data.content_ptr);
        LPA_FREE(task_cmd_ptr->data.rsp_data.header_info_ptr);
        break;
      case LPA_HTTP_QMI_HTTP_RSP:
        lpa_http_process_qmi_http_rsp(task_cmd_ptr->data.qmi_http_rsp_data);
        LPA_FREE(task_cmd_ptr->data.qmi_http_rsp_data.content_ptr);
        LPA_FREE(task_cmd_ptr->data.qmi_http_rsp_data.header_info_ptr);
        break;
      default:
        break;
    }
    LPA_FREE(task_cmd_ptr);
  }

  (void) rex_clr_sigs(rex_self(), LPA_HTTP_RSP_SIG);

  /* Set the signal if there are one or more commands to look at */
  if(q_cnt(&lpa_http_cmd_q) > 0)
  {
    /*  Items on the queue. Set the signal */
    (void) rex_set_sigs(rex_self(), LPA_HTTP_RSP_SIG);
  }
} /* lpa_http_handle_cmd_sig */


/*===========================================================================
FUNCTION:      LPA_HTTP_INIT

DESCRIPTION:
  LPA HTTP init function called during task init

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void lpa_http_init (
  void
)
{
  uint8 i = 0;
  (void)q_init(&lpa_http_cmd_q);
  for (i = 0; i < LPA_NUM_SLOTS; i++)
  {
    lpa_http_global_data[i].session_id = LPA_SESSION_ID_NULL;
    lpa_http_global_data[i].qmi_http_service_id = QMI_UIM_HTTP_MAX_SERVICE_ID;
  }
} /* lpa_http_init */


/*===========================================================================
FUNCTION:      LPA_HTTP_CLEANUP

DESCRIPTION:
  LPA HTTP cleanup function called to clean up refrerence data

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  lpa_result_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
lpa_result_enum_type lpa_http_cleanup (
  lpa_slot_id_enum_type             slot_id
)
{
  LPA_MSG_HIGH_0("lpa_http_cleanup");

  if(slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }
  /* Close the opened session */
  if(lpa_http_use_qmi_http())
  {
    if (lpa_http_global_data[slot_id].qmi_http_service_id != QMI_UIM_HTTP_MAX_SERVICE_ID)
    {
      qmi_uim_http_service_release(lpa_http_global_data[slot_id].qmi_http_service_id);
    }
  }
  else
  {
    if(lpa_http_global_data[slot_id].session_id != DS_HTTP_ERROR &&
       lpa_http_global_data[slot_id].session_status == DS_HTTP_READY)
    {
      ds_http_close_session(lpa_http_global_data[slot_id].session_id);
    }
  }

  lpa_http_free_pending_post_data(slot_id);
  memset(&lpa_http_global_data[slot_id], 0x00, sizeof(lpa_http_global_data[slot_id]));
  lpa_http_global_data[slot_id].session_id = DS_HTTP_ERROR;
  lpa_http_global_data[slot_id].session_status = DS_HTTP_ERROR_HTTP_STATUS;

  lpa_http_global_data[slot_id].qmi_http_service_id = QMI_UIM_HTTP_MAX_SERVICE_ID;

  return LPA_SUCCESS;
} /* lpa_http_cleanup */
