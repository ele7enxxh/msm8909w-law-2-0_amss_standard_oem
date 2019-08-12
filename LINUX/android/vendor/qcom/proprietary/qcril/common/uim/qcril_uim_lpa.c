/*===========================================================================

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/16   vr      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "qcrili.h"
#include "qcril_log.h"
#include "qcril_reqlist.h"
#include "qcril_uim.h"
#include "qcril_uim_util.h"
#include "qcril_uim_srvc.h"
#include "qcril_uim_lpa.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client.h"
#include "qmi_ril_platform_dep.h"
#include "user_identity_module_v01.h"
#include "lpa.pb.h"
#include "qcril_uim_lpa_misc.h"
#include "qcril_uim_lpa_socket.h"
#include "pb_decode.h"
#include "pb_encode.h"


/*===========================================================================
                        INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/* Synchronous message default timeout (in milli-seconds) */
#define QMI_UIM_DEFAULT_TIMEOUT 5000

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_LPA_CB_PARAMS_TYPE

   DESCRIPTION:
     Structure used to copy response received from the modem
-------------------------------------------------------------------------------*/
typedef struct
{
  unsigned int                         msg_id;
  void                               * msg_ptr;
  qmi_client_error_type                transp_err;
  qcril_uim_original_request_type    * orig_req_ptr;
} qcril_uim_lpa_cb_params_type;

/*---------------------------------------------------------------------------
  global variables
---------------------------------------------------------------------------*/
static uint8_t  qcril_uim_lpa_profileId_iccId[QMI_UIM_PROFILES_MAX_V01][QCRIL_UIM_ICCID_LEN] = {{0xFF}};
static boolean  qcril_uim_lpa_profiles_cached = FALSE;

/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/
/*=========================================================================

  FUNCTION:  qcril_uim_lpa_convert_slot_id_to_slot_type

===========================================================================*/
static boolean qcril_uim_lpa_convert_slot_id_to_slot_type
(
  uint8                 slot_id,
  uim_slot_enum_v01   * uim_slot_ptr
)
{
  if (uim_slot_ptr == NULL)
  {
    return FALSE;
  }

  switch (slot_id)
  {
     case 0:
       *uim_slot_ptr = UIM_SLOT_1_V01;
       break;

     case 1:
       *uim_slot_ptr = UIM_SLOT_2_V01;
       break;

    case 2:
      *uim_slot_ptr = UIM_SLOT_3_V01;
      break;

    default:
      QCRIL_LOG_ERROR( "Invalid _id returned: 0x%x\n", slot_id);
      return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_convert_slot_id_to_slot_type */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_iccid_to_byte

===========================================================================*/
static uint8_t qcril_uim_lpa_iccid_to_byte
(
  uint8_t       * iccid_byte_ptr,
  uint8_t         iccid_byte_len,
  const uint8_t * iccid_ptr,
  uint8_t         len
)
{
  uint8_t  i = 0;
  uint8_t  j = 0;
  uint8_t ch = 0;

  if (iccid_ptr == NULL ||
      iccid_byte_ptr == NULL ||
      len == 0 ||
      len > QCRIL_UIM_ICCID_LEN ||
      iccid_byte_len == 0 ||
      len > 2 * QCRIL_UIM_ICCID_LEN)
  {
      QCRIL_LOG_ERROR("iccid ptr or iccid_str_ptr parameter is NULL");
      return 0;
  }

  for ( i = 0; i < len && j < iccid_byte_len; i++ )
  {
    /* copy first digit */
    ch = iccid_ptr[i] & 0x0F;
    /* iccid bcd octets can have padded 0xf, need to ignore the same */
    if (0x0F == ch)
    {
      continue;
    }

    if (ch > 9)
    {
      return 0;
    }
    iccid_byte_ptr[j] = ch + '0';
    j++;

    if (j < iccid_byte_len)
    {
      /* copy second digit */
      ch = iccid_ptr[i] >> 4;
      /* iccid bcd octets can have padded 0xf, need to ignore the same */
      if (0x0F == ch)
      {
        continue;
      }

      if (ch > 9)
      {
          return 0;
      }
      iccid_byte_ptr[j] = ch + '0';
      j++;
    }
  }
  return j;
} /* qcril_uim_lpa_iccid_to_byte */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_byte_to_iccid

===========================================================================*/
static uint8_t qcril_uim_lpa_byte_to_iccid
(
  uint8_t       * iccid_ptr,
  uint8_t         iccid_len,
  const uint8_t * iccid_byte_ptr,
  uint8_t         byte_len
)
{
  uint8_t i = 0;
  uint8_t j = 0;
  int8_t ch = -1;

  if (iccid_ptr == NULL ||
      iccid_byte_ptr == NULL ||
      iccid_len == 0 ||
      iccid_len > QCRIL_UIM_ICCID_LEN ||
      byte_len == 0 ||
      byte_len > 2 * QCRIL_UIM_ICCID_LEN)
  {
    QCRIL_LOG_ERROR("iccid ptr or iccid_str_ptr parameter is NULL");
    return 0;
  }

  for ( i = 0; i < byte_len && j < iccid_len; j++ )
  {
    /* copy first bcd digit */
    ch = iccid_byte_ptr[i] - '0';
    if (ch < 0 || ch > 9)
    {
      return 0;
    }
    iccid_ptr[j] = ch;
    i++;

    /* copy second bcd digit */
    if ( i < byte_len )
    {
      ch = iccid_byte_ptr[i] - '0';
      if (ch < 0 || ch > 9)
      {
        return 0;
      }
      ch = ch << 4;
      iccid_ptr[j] |= ch;
      i++;
    }
    else
    {
      iccid_ptr[j] |= 0xF0;
    }
  }
  return j;
} /* qcril_uim_lpa_byte_to_iccid */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_get_profile_id_from_iccid

===========================================================================*/
static int8_t qcril_uim_lpa_get_profile_id_from_iccid
(
  uint8_t *iccid_ptr
)
{
  uint8 i = 0;

  if (iccid_ptr == NULL)
  {
    return -1;
  }

  for (i = 0; i < QMI_UIM_PROFILES_MAX_V01; i++)
  {
    if (memcmp(iccid_ptr, qcril_uim_lpa_profileId_iccId[i], QCRIL_UIM_ICCID_LEN) == 0)
    {
      /* Profile Id starts from 1, index starts from 0, index + 1 is profile ID*/
      return (i+1);
    }
  }

  return -1;
} /* qcril_uim_lpa_get_profile_id_from_iccid */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_copy_callback

===========================================================================*/
static qcril_uim_lpa_cb_params_type * qcril_uim_lpa_copy_callback
(
  unsigned int                         msg_id,
  void                               * resp_data_ptr,
  void                               * resp_cb_data_ptr,
  qmi_client_error_type                transp_err,
  uint32_t                           * out_len_ptr
)
{
  qcril_uim_lpa_cb_params_type   * out_ptr  = NULL;

  /* Sanity check */
  if ((resp_data_ptr    == NULL) ||
      (resp_cb_data_ptr == NULL) ||
      (out_len_ptr      == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot copy");
    return NULL;
  }

  /* Update size & allocate buffer */
  *out_len_ptr = sizeof(qcril_uim_lpa_cb_params_type);
  out_ptr = (qcril_uim_lpa_cb_params_type*)qcril_malloc(*out_len_ptr);
  if (out_ptr == NULL)
  {
    return NULL;
  }

  memset(out_ptr, 0, *out_len_ptr);

  /* Copy the response parameters */
  out_ptr->msg_id        = msg_id;
  out_ptr->msg_ptr       = resp_data_ptr;
  out_ptr->transp_err    = transp_err;
  out_ptr->orig_req_ptr  = (qcril_uim_original_request_type*)resp_cb_data_ptr;

  return out_ptr;
} /* qcril_uim_lpa_copy_callback */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_cmd_callback

===========================================================================*/
static void qcril_uim_lpa_cmd_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_lpa_rsp_ptr,
  unsigned int                   qmi_lpa_rsp_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  uint32_t                           params_len = 0;
  qcril_uim_lpa_cb_params_type     * params_ptr = NULL;
  IxErrnoType                        result     = E_FAILURE;

  QCRIL_NOTUSED(user_handle);
  QCRIL_NOTUSED(qmi_lpa_rsp_len);

  QCRIL_LOG_INFO( "qcril_uim_lpa_cmd_callback: msg_id = 0x%x ", msg_id);

  /* Sanity check */
  if ((qmi_lpa_rsp_ptr == NULL) || (resp_cb_data_ptr == NULL))
  {
    QCRIL_LOG_ERROR("NULL qmi_lpa_rsp_ptr or resp_cb_data_ptr");
    goto clean_up;
  }

  /* Process only the supported RESP messages */
  switch (msg_id)
  {
    case QMI_UIM_GET_PROFILE_INFO_RESP_V01:
    case QMI_UIM_GET_EID_RESP_V01:
    case QMI_UIM_DELETE_PROFILE_RESP_V01:
    case QMI_UIM_EUICC_MEMORY_RESET_RESP_V01:
    case QMI_UIM_ADD_PROFILE_RESP_V01:
    case QMI_UIM_UPDATE_PROFILE_NICKNAME_RESP_V01:
    case QMI_UIM_SET_SIM_PROFILE_RESP_V01:
    case QMI_UIM_GET_SIM_PROFILE_RESP_V01:
      params_ptr = qcril_uim_lpa_copy_callback(msg_id,
                                               qmi_lpa_rsp_ptr,
                                               resp_cb_data_ptr,
                                               transp_err,
                                               &params_len);
      break;

    default:
      QCRIL_LOG_ERROR("Unsupported QMI UIM LPA response: 0x%x", msg_id);
      break;
  }

  if ((params_ptr == NULL) || (params_len == 0))
  {
    QCRIL_LOG_ERROR("Error copying the response msg_id: 0x%x", msg_id);
    goto clean_up;
  }

  /* Post the event to process the response callback.
     Note: Upon successful posting of the event, necessary pointers will
     be freed in the response handler */
  result = qcril_event_queue( params_ptr->orig_req_ptr->instance_id,
                              params_ptr->orig_req_ptr->modem_id,
                              QCRIL_DATA_NOT_ON_STACK,
                              QCRIL_EVT_UIM_LPA_QMI_COMMAND_CALLBACK,
                              (void *)params_ptr,
                              params_len,
                              NULL);
  if (result == E_SUCCESS)
  {
    return;
  }

  QCRIL_LOG_ERROR( " qcril_event_queue failed, result: 0x%x\n", result);

clean_up:
  /* Free allocated pointers only if event queueing fails */
  QCRIL_UIM_FREE_IF_NOT_NULL(resp_cb_data_ptr);
  QCRIL_UIM_FREE_IF_NOT_NULL(qmi_lpa_rsp_ptr);
  QCRIL_UIM_FREE_IF_NOT_NULL(params_ptr);
} /* qcril_uim_lpa_cmd_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile

===========================================================================*/
static lpa_Error qcril_uim_lpa_add_profile
(
  const qcril_request_params_type *const params_ptr,
  char                            * activationCode_ptr,
  char                            * confirmationCode_ptr
)
{
  qmi_txn_handle                        txn_handle;
  qcril_uim_original_request_type     * orig_req_ptr           = NULL;
  int                                   qmi_err_code           = 0;
  uim_add_profile_req_msg_v01           add_profile_req;
  uim_add_profile_resp_msg_v01        * add_profile_resp_ptr   = NULL;
  uint8                                 slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&add_profile_req, 0x00, sizeof(add_profile_req));

  if (params_ptr == NULL ||
      activationCode_ptr == NULL ||
      strlen(activationCode_ptr) == 0 ||
      strlen(activationCode_ptr) >= QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &add_profile_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  add_profile_req.activation_code_len = strlen(activationCode_ptr);
  memset(add_profile_req.activation_code,'\0', add_profile_req.activation_code_len);
  strlcpy(add_profile_req.activation_code,
          activationCode_ptr,
          sizeof(add_profile_req.activation_code));

  if (confirmationCode_ptr != NULL &&
      strlen(confirmationCode_ptr) != 0 &&
      strlen(confirmationCode_ptr) < QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01)
  {
    add_profile_req.confirmation_code_valid = TRUE;
    add_profile_req.confirmation_code_len   = strlen(confirmationCode_ptr);
    memset(add_profile_req.confirmation_code, '\0' , add_profile_req.confirmation_code_len);
    strlcpy(add_profile_req.confirmation_code,
            (const char *)confirmationCode_ptr,
            sizeof(add_profile_req.confirmation_code));
  }
  else
  {
    add_profile_req.confirmation_code_valid = FALSE;
  }

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 params_ptr->event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate response pointer since it is an async command */
  add_profile_resp_ptr = (uim_add_profile_resp_msg_v01 *)
                             qcril_malloc(sizeof(uim_add_profile_resp_msg_v01));
  if (add_profile_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for add_profile_resp_ptr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_ADD_PROFILE_REQ_V01,
                       &add_profile_req,
                       sizeof(uim_add_profile_req_msg_v01),
                       (void *) add_profile_resp_ptr,
                       sizeof(uim_add_profile_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(add_profile_resp_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

   return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_add_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_profile

===========================================================================*/
static lpa_Error qcril_uim_lpa_get_profile
(
  const qcril_request_params_type      * const params_ptr,
  uint8                                * iccid_ptr,
  int                                    event_id
)
{
  qmi_txn_handle                        txn_handle;
  qcril_uim_original_request_type     * orig_req_ptr           = NULL;
  int                                   qmi_err_code           = 0;
  uim_get_sim_profile_req_msg_v01       get_profiles_req;
  uim_get_sim_profile_resp_msg_v01    * get_profiles_resp_ptr  = NULL;
  uint8                                 slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&get_profiles_req, 0, sizeof(uim_get_sim_profile_req_msg_v01));

  if (params_ptr == NULL)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &get_profiles_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  orig_req_ptr->data.lpa_profile_info.slot_id = (uint8)get_profiles_req.slot;

  /* cache the iccid */
  memset(orig_req_ptr->data.lpa_profile_info.iccid, 0xFF, QCRIL_UIM_ICCID_LEN);
  if (iccid_ptr != NULL)
  {
    memcpy(orig_req_ptr->data.lpa_profile_info.iccid,
           iccid_ptr,
           QCRIL_UIM_ICCID_LEN);
  }

  /* Allocate response pointer since it is an async command */
  get_profiles_resp_ptr = (uim_get_sim_profile_resp_msg_v01 *)
                             qcril_malloc(sizeof(uim_get_sim_profile_resp_msg_v01));
  if (get_profiles_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for get_profiles_resp_ptr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_GET_SIM_PROFILE_REQ_V01,
                       &get_profiles_req,
                       sizeof(uim_get_sim_profile_req_msg_v01),
                       (void *) get_profiles_resp_ptr,
                       sizeof(uim_get_sim_profile_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(get_profiles_resp_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_get_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_enable_disable_profile

===========================================================================*/
static lpa_Error qcril_uim_lpa_enable_disable_profile
(
  const qcril_request_params_type                *const params_ptr,
  uint8_t                                        * iccid_ptr,
  lpa_UimLpaUserEventId                            event_id,
  boolean                                          from_get_profile
)
{
  qmi_txn_handle                     txn_handle;
  qcril_uim_original_request_type  * orig_req_ptr            = NULL;
  int                                qmi_err_code            = 0;
  uim_set_sim_profile_req_msg_v01    en_dis_profile_req;
  uim_set_sim_profile_resp_msg_v01 * en_dis_profile_resp_ptr = NULL;
  uint8                              slot                    = QMI_UIM_MAX_CARD_COUNT;
  int8_t                             profile_id              = -1;

  memset(&en_dis_profile_req, 0, sizeof(uim_set_sim_profile_req_msg_v01));

  if (params_ptr == NULL ||
      iccid_ptr == NULL ||
      iccid_ptr[0] == 0xFF)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &en_dis_profile_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_profiles_cached &&
      !from_get_profile)
  {
    if (qcril_uim_lpa_get_profile(params_ptr,
                                  iccid_ptr,
                                  event_id) == lpa_Error_UIM_LPA_ERR_SUCCESS)
    {
      return lpa_Error_UIM_LPA_ERR_SUCCESS;
    }
  }

  profile_id = qcril_uim_lpa_get_profile_id_from_iccid(iccid_ptr);

  if (profile_id < 0 || profile_id > QMI_UIM_PROFILES_MAX_V01)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  en_dis_profile_req.profile_id = (uim_profile_id_enum_v01)profile_id;

  en_dis_profile_req.enable_profile_valid = TRUE;


  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (event_id == lpa_UimLpaUserEventId_UIM_LPA_ENABLE_PROFILE)
  {
    orig_req_ptr->data.en_dis_profile = TRUE;
    en_dis_profile_req.enable_profile = TRUE;
  }
  else
  {
    orig_req_ptr->data.en_dis_profile = FALSE;
    en_dis_profile_req.enable_profile = FALSE;
  }

  /* Allocate response pointer since it is an async command */
  en_dis_profile_resp_ptr = (uim_set_sim_profile_resp_msg_v01 *)
                              qcril_malloc(sizeof(uim_set_sim_profile_resp_msg_v01));
  if (en_dis_profile_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for del_profile_resp_ptr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_SET_SIM_PROFILE_REQ_V01,
                       &en_dis_profile_req,
                       sizeof(uim_set_sim_profile_req_msg_v01),
                       (void *) en_dis_profile_resp_ptr,
                       sizeof(uim_set_sim_profile_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(en_dis_profile_resp_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_enable_disable_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_delete_profile

===========================================================================*/
static lpa_Error qcril_uim_lpa_delete_profile
(
  const qcril_request_params_type * const params_ptr,
  uint8_t                         * iccid_ptr,
  boolean                           from_get_profile
)
{
  qmi_txn_handle                     txn_handle;
  qcril_uim_original_request_type  * orig_req_ptr         = NULL;
  int                                qmi_err_code         = 0;
  uim_delete_profile_req_msg_v01     del_profile_req;
  uim_delete_profile_resp_msg_v01  * del_profile_resp_ptr = NULL;
  uint8                              slot                 = QMI_UIM_MAX_CARD_COUNT;
  int8_t                             profile_id           = -1;

  memset(&del_profile_req, 0, sizeof(uim_delete_profile_req_msg_v01));

  if (params_ptr == NULL ||
      iccid_ptr == NULL ||
      iccid_ptr[0] == 0xFF)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &del_profile_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_profiles_cached &&
      !from_get_profile)
  {
    if (qcril_uim_lpa_get_profile(params_ptr,
                                  iccid_ptr,
                                  lpa_UimLpaUserEventId_UIM_LPA_DELETE_PROFILE) == lpa_Error_UIM_LPA_ERR_SUCCESS)
    {
      return lpa_Error_UIM_LPA_ERR_SUCCESS;
    }
  }

  profile_id = qcril_uim_lpa_get_profile_id_from_iccid(iccid_ptr);

  if (profile_id < 0 || profile_id > QMI_UIM_PROFILES_MAX_V01)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  del_profile_req.profile_id = (uim_profile_id_enum_v01)profile_id;

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 params_ptr->event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  orig_req_ptr->data.profile_id = profile_id;

  /* Allocate response pointer since it is an async command */
  del_profile_resp_ptr = (uim_delete_profile_resp_msg_v01 *)
                         qcril_malloc(sizeof(uim_delete_profile_resp_msg_v01));
  if (del_profile_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for del_profile_resp_ptr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_DELETE_PROFILE_REQ_V01,
                       &del_profile_req,
                       sizeof(uim_delete_profile_req_msg_v01),
                       (void *) del_profile_resp_ptr,
                       sizeof(uim_delete_profile_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(del_profile_resp_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_delete_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_memory_reset

===========================================================================*/
static lpa_Error qcril_uim_lpa_memory_reset
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_txn_handle                        txn_handle;
  qcril_uim_original_request_type     * orig_req_ptr           = NULL;
  int                                   qmi_err_code           = 0;
  uim_euicc_memory_reset_req_msg_v01    memory_reset_req;
  uim_euicc_memory_reset_resp_msg_v01 * memory_reset_resp_ptr  = NULL;
  uint8                                 slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&memory_reset_req, 0, sizeof(uim_euicc_memory_reset_req_msg_v01));

  if (params_ptr == NULL)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &memory_reset_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 params_ptr->event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate response pointer since it is an async command */
  memory_reset_resp_ptr = (uim_euicc_memory_reset_resp_msg_v01 *)
                             qcril_malloc(sizeof(uim_euicc_memory_reset_resp_msg_v01));
  if (memory_reset_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for memory_reset_resp_ptr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_EUICC_MEMORY_RESET_REQ_V01,
                       &memory_reset_req,
                       sizeof(uim_euicc_memory_reset_req_msg_v01),
                       (void *) memory_reset_resp_ptr,
                       sizeof(uim_euicc_memory_reset_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(memory_reset_resp_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_memory_reset */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_update_nickname

===========================================================================*/
static lpa_Error qcril_uim_lpa_update_nickname
(
  const qcril_request_params_type *const params_ptr,
  uint8_t                         * iccid_ptr,
  char                            * nick_name_ptr
)
{
  qmi_txn_handle                             txn_handle;
  qcril_uim_original_request_type          * orig_req_ptr       = NULL;
  int                                        qmi_err_code       = 0;
  uim_update_profile_nickname_req_msg_v01    nick_name_req;
  uim_update_profile_nickname_resp_msg_v01 * nick_name_resp_ptr = NULL;
  uint8                                      slot               = QMI_UIM_MAX_CARD_COUNT;
  int8_t                                     profile_id         = -1;

  memset(&nick_name_req, 0, sizeof(uim_update_profile_nickname_req_msg_v01));

  if (params_ptr == NULL ||
      iccid_ptr == NULL)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &nick_name_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  profile_id = qcril_uim_lpa_get_profile_id_from_iccid(iccid_ptr);

  if (profile_id < 0 || profile_id > QMI_UIM_PROFILES_MAX_V01)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  nick_name_req.profile_id = (uim_profile_id_enum_v01)profile_id;

  if (nick_name_ptr == NULL ||
      strlen(nick_name_ptr) > QMI_UIM_EUICC_NICKNAME_MAX_V01)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (strlen(nick_name_ptr) > 0)
  {
    nick_name_req.nickname_valid = TRUE;
    nick_name_req.nickname_len = strlen(nick_name_ptr);
    memset(nick_name_req.nickname, 0 , nick_name_req.nickname_len);
    memcpy(nick_name_req.nickname, nick_name_ptr, nick_name_req.nickname_len);
  }

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 params_ptr->event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate response pointer since it is an async command */
  nick_name_resp_ptr = (uim_update_profile_nickname_resp_msg_v01 *)
                         qcril_malloc(sizeof(uim_update_profile_nickname_resp_msg_v01));
  if (nick_name_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for nick_name_resp_ptr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_UPDATE_PROFILE_NICKNAME_REQ_V01,
                       &nick_name_req,
                       sizeof(uim_update_profile_nickname_req_msg_v01),
                       (void *) nick_name_resp_ptr,
                       sizeof(uim_update_profile_nickname_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(nick_name_resp_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_update_nickname */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_eid

===========================================================================*/
static lpa_Error qcril_uim_lpa_get_eid
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_txn_handle                        txn_handle;
  qcril_uim_original_request_type     * orig_req_ptr = NULL;
  int                                   qmi_err_code = 0;
  uim_get_eid_req_msg_v01               eid_req;
  uim_get_eid_resp_msg_v01            * eid_resp_ptr = NULL;
  uint8                                 slot         = QMI_UIM_MAX_CARD_COUNT;

  memset(&eid_req, 0, sizeof(uim_get_eid_req_msg_v01));

  if (params_ptr == NULL)
  {
    return lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &eid_req.slot))
  {
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 (RIL_Token)params_ptr->t,
                                                 params_ptr->event_id,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  /* Allocate response pointer since it is an async command */
  eid_resp_ptr = (uim_get_eid_resp_msg_v01 *)
                    qcril_malloc(sizeof(uim_get_eid_resp_msg_v01));
  if (eid_resp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for eid_resp_tr");
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  qmi_err_code = qmi_client_send_msg_async_with_shm(
                       qcril_uim.qmi_handle,
                       QMI_UIM_GET_EID_REQ_V01,
                       &eid_req,
                       sizeof(uim_get_eid_req_msg_v01),
                       (void *) eid_resp_ptr,
                       sizeof(uim_get_eid_resp_msg_v01),
                       qcril_uim_lpa_cmd_callback,
                       orig_req_ptr,
                       &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(eid_resp_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
    return lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  }

  return lpa_Error_UIM_LPA_ERR_SUCCESS;
} /* qcril_uim_lpa_get_eid */


/*===========================================================================

  FUNCTION:  qcril_lpa_uim_get_profile_info

===========================================================================*/
static boolean qcril_lpa_uim_get_profile_info
(
  lpa_UimLpaProfileInfo     ** profile_info_pptr,
  uim_slot_enum_v01                                     slot_id,
  uim_profile_id_enum_v01                               profile_id
)
{
  uim_get_profile_info_req_msg_v01                 profile_info_req;
  uim_get_profile_info_resp_msg_v01              * profile_info_resp_ptr = NULL;
  qmi_client_error_type                            rc                    = QMI_INTERNAL_ERR;
  qcril_binary_data_type                         * iccid_ptr             = NULL;
  char                                           * profile_name_ptr      = NULL;
  char                                           * nick_name_ptr         = NULL;
  char                                           * sp_name_ptr           = NULL;
  qcril_binary_data_type                         * icon_data_ptr         = NULL;
  lpa_UimLpaProfileInfo                          * profile_info_ptr      = NULL;

  memset(&profile_info_req, 0, sizeof(uim_get_profile_info_req_msg_v01));

  profile_info_req.slot       = slot_id;
  profile_info_req.profile_id = profile_id;

  if (profile_info_pptr == NULL ||
      profile_id == 0 ||
      profile_id > UIM_PROFILE_ID_8_V01 ||
      profile_id > QMI_UIM_PROFILES_MAX_V01)
  {
    return FALSE;
  }

  profile_info_ptr = (lpa_UimLpaProfileInfo *)qcril_malloc(sizeof(lpa_UimLpaProfileInfo));
  if (profile_info_ptr == NULL)
  {
    return FALSE;
  }
  memset(profile_info_ptr, 0x00, sizeof(lpa_UimLpaProfileInfo));
  *profile_info_pptr = profile_info_ptr;

  profile_info_resp_ptr = (uim_get_profile_info_resp_msg_v01 *)qcril_malloc(sizeof(uim_get_profile_info_resp_msg_v01));

  if (profile_info_resp_ptr == NULL)
  {
    return FALSE;
  }
  memset(profile_info_resp_ptr, 0x00, sizeof(uim_get_profile_info_resp_msg_v01));

  rc = qmi_client_send_msg_sync_with_shm( qcril_uim.qmi_handle,
                                 QMI_UIM_GET_PROFILE_INFO_REQ_V01,
                                 (void*) &profile_info_req,
                                 sizeof(uim_get_profile_info_req_msg_v01),
                                 (void*)profile_info_resp_ptr,
                                 sizeof(uim_get_profile_info_resp_msg_v01),
                                 QMI_UIM_DEFAULT_TIMEOUT);

  if ((rc != QMI_NO_ERR) || (profile_info_resp_ptr->resp.result != QMI_RESULT_SUCCESS_V01))
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
    return FALSE;
  }

  /* Copy state */
  if (profile_info_resp_ptr->profile_state_valid == 0x01)
  {
    switch (profile_info_resp_ptr->profile_state)
    {
      case UIM_PROFILE_STATE_INACTIVE_V01:
        profile_info_ptr->state = lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_INACTIVE;
        break;
      case UIM_PROFILE_STATE_ACTIVE_V01:
        profile_info_ptr->state = lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_ACTIVE;
        break;
      default:
        profile_info_ptr->state = lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_UNKNOWN;
        break;
    }
  }
  else
  {
    profile_info_ptr->state = lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_UNKNOWN;
  }
  /* Copy icon */
  if (profile_info_resp_ptr->profile_icon_type_valid == 0x01)
  {
    profile_info_ptr->has_iconType = TRUE;
    switch (profile_info_resp_ptr->profile_icon_type)
    {
      case UIM_PROFILE_ICON_TYPE_JPEG_V01:
        profile_info_ptr->iconType = lpa_UimLpaProfileInfo_IconType_UIM_LPA_ICON_TYPE_JPEG;
        break;
      case UIM_PROFILE_ICON_TYPE_PNG_V01:
        profile_info_ptr->iconType = lpa_UimLpaProfileInfo_IconType_UIM_LPA_ICON_TYPE_PNG;
        break;
      default:
        profile_info_ptr->iconType = lpa_UimLpaProfileInfo_IconType_UIM_LPA_ICON_TYPE_UNKNOWN;
        break;
    }
  }

  /* Copy icon data */
  if (profile_info_resp_ptr->profile_icon_data_valid)
  {
    if (profile_info_resp_ptr->profile_icon_data_len > QMI_UIM_EUICC_ICON_MAX_V01)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    icon_data_ptr = (qcril_binary_data_type *)qcril_malloc(sizeof(qcril_binary_data_type));
    if (icon_data_ptr == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    icon_data_ptr->data = qcril_malloc(profile_info_resp_ptr->profile_icon_data_len * sizeof(uint8_t));

    if (icon_data_ptr->data == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    memset(icon_data_ptr->data, 0, profile_info_resp_ptr->profile_icon_data_len * sizeof(uint8_t));

    memcpy(icon_data_ptr->data, profile_info_resp_ptr->profile_icon_data, profile_info_resp_ptr->profile_icon_data_len);
    icon_data_ptr->len = profile_info_resp_ptr->profile_icon_data_len;

    profile_info_ptr->icon.arg = icon_data_ptr;
  }

  /* Copy ICCID */
  if (profile_info_resp_ptr->profile_iccid_valid == 0x01)
  {
    uint8_t iccid_len = 0;

    if (profile_info_resp_ptr->profile_iccid_len > QMI_UIM_ICCID_LEN_MAX_V01)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    iccid_ptr = (qcril_binary_data_type *)qcril_malloc(sizeof(qcril_binary_data_type));
    if (iccid_ptr == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    iccid_ptr->data = qcril_malloc(2 * profile_info_resp_ptr->profile_iccid_len * sizeof(uint8_t));

    if (iccid_ptr->data == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    iccid_len = qcril_uim_lpa_iccid_to_byte(iccid_ptr->data,
                                            (2 * profile_info_resp_ptr->profile_iccid_len),
                                            profile_info_resp_ptr->profile_iccid,
                                            profile_info_resp_ptr->profile_iccid_len);

    if(iccid_len == 0)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    else
    {
      iccid_ptr->len = iccid_len;
    }

    profile_info_ptr->iccid.arg = iccid_ptr;

    /* Profile Id - 1 will be index for iccid cache*/
    memcpy(qcril_uim_lpa_profileId_iccId[profile_id - 1],
           profile_info_resp_ptr->profile_iccid,
           profile_info_resp_ptr->profile_iccid_len);
  }

  /* Copy profile name */
  if (profile_info_resp_ptr->profile_name_valid == 0x01)
  {
    if (profile_info_resp_ptr->profile_name_len >= QMI_UIM_EUICC_PROFILE_NAME_MAX_V01)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    profile_name_ptr = (char *)qcril_malloc(profile_info_resp_ptr->profile_name_len * sizeof(char) + 1);
    if (profile_name_ptr == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    memset(profile_name_ptr, '\0', profile_info_resp_ptr->profile_name_len * sizeof(char));

    strlcpy(profile_name_ptr, profile_info_resp_ptr->profile_name, profile_info_resp_ptr->profile_name_len + 1);

    profile_info_ptr->profileName.arg = profile_name_ptr;
  }

  /* Copy nick name */
  if (profile_info_resp_ptr->profile_nickname_valid == 0x01)
  {
    if (profile_info_resp_ptr->profile_nickname_len > QMI_UIM_EUICC_NICKNAME_MAX_V01)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }

    nick_name_ptr = qcril_malloc(profile_info_resp_ptr->profile_nickname_len * sizeof(char) + 1);
    if (nick_name_ptr == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    memset(nick_name_ptr, '\0', profile_info_resp_ptr->profile_nickname_len * sizeof(char));

    strlcpy(nick_name_ptr, profile_info_resp_ptr->profile_nickname, profile_info_resp_ptr->profile_nickname_len + 1);

    profile_info_ptr->nickname.arg = nick_name_ptr;
  }

  /* Copy SPN name */
  if (profile_info_resp_ptr->profile_spn_valid == 0x01)
  {
    if (profile_info_resp_ptr->profile_spn_len >= QMI_UIM_EUICC_SPN_MAX_V01)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }

    sp_name_ptr = qcril_malloc(profile_info_resp_ptr->profile_spn_len * sizeof(char) + 1);
    if (sp_name_ptr == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
      return FALSE;
    }
    memset(sp_name_ptr, '\0', profile_info_resp_ptr->profile_spn_len * sizeof(char));

    strlcpy(sp_name_ptr, profile_info_resp_ptr->profile_spn, profile_info_resp_ptr->profile_spn_len + 1);

    profile_info_ptr->spName.arg = sp_name_ptr;
  }

  QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
  return TRUE;
} /* qcril_lpa_uim_get_profile_info */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_profile_info_resp

===========================================================================*/
static void qcril_uim_lpa_get_profile_info_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type   *  orig_req_ptr            = NULL;
  uim_get_sim_profile_resp_msg_v01  *  get_profiles_rsp_ptr    = NULL;
  lpa_UimLpaUserResp                   transaction_resp;
  lpa_Error                            result                  = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  uint32_t                             no_of_profiles          = 0;
  uint32_t                             i                       = 0;
  lpa_UimLpaProfileInfo             ** profiles_data_pptr      = NULL;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr            = callback_params_ptr->orig_req_ptr;
  get_profiles_rsp_ptr    = (uim_get_sim_profile_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  transaction_resp.event  = lpa_UimLpaUserEventId_UIM_LPA_GET_PROFILE;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (get_profiles_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL enable_disable_profile_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("QMI response: 0x%x", get_profiles_rsp_ptr->resp.result);

  if (get_profiles_rsp_ptr->resp.result != QMI_RESULT_SUCCESS_V01 ||
      get_profiles_rsp_ptr->profile_info_valid == FALSE)
  {
    goto send_response;
  }

  no_of_profiles = get_profiles_rsp_ptr->profile_info_len;

  if (no_of_profiles > QMI_UIM_PROFILES_MAX_V01)
  {
    no_of_profiles = QMI_UIM_PROFILES_MAX_V01;
  }

  /* Protobuff encoder, runs the loop for encoding until it get null pointer.
     Allocate one extra pointer for null pointer termination */
  profiles_data_pptr = (lpa_UimLpaProfileInfo **)
                         qcril_malloc(sizeof(lpa_UimLpaProfileInfo *) * (no_of_profiles + 1));

  if (profiles_data_pptr == NULL)
  {
    goto send_response;
  }
  memset(profiles_data_pptr, 0x00, sizeof(lpa_UimLpaProfileInfo *) * (no_of_profiles + 1));

  QCRIL_LOG_DEBUG("No of profile 0x%x", no_of_profiles);

  for (i = 0; i < no_of_profiles && i < QMI_UIM_PROFILES_MAX_V01; i++)
  {
    if (qcril_lpa_uim_get_profile_info(&profiles_data_pptr[i],
                                       (uim_slot_enum_v01)orig_req_ptr->data.lpa_profile_info.slot_id,
                                       get_profiles_rsp_ptr->profile_info[i].profile_id) == FALSE)
    {
      /* Clean up cached ICCIDs */
      qcril_uim_lpa_cleanup_cached_iccids();
      goto send_response;
    }
  }
  transaction_resp.profiles.arg   = profiles_data_pptr;
  transaction_resp.result         = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  qcril_uim_lpa_profiles_cached   = TRUE;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

send_response:
  if (orig_req_ptr->request_id == lpa_UimLpaUserEventId_UIM_LPA_GET_PROFILE)
  {
    qcril_uim_lpa_socket_send(TRUE,
                              (RIL_Token)orig_req_ptr->token,
                              lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                              lpa_MessageId_UIM_LPA_USER_REQUEST,
                              TRUE,
                              result,
                              &transaction_resp,
                              sizeof(lpa_UimLpaUserResp));
  }
  else
  {
    qcril_request_params_type  params_data;

    params_data.instance_id = orig_req_ptr->instance_id;
    params_data.modem_id    = orig_req_ptr->modem_id;
    params_data.t           = orig_req_ptr->token;

    switch(orig_req_ptr->request_id)
    {
      case lpa_UimLpaUserEventId_UIM_LPA_ENABLE_PROFILE:
      case lpa_UimLpaUserEventId_UIM_LPA_DISABLE_PROFILE:
        result = qcril_uim_lpa_enable_disable_profile(&params_data,
                                                      orig_req_ptr->data.lpa_profile_info.iccid,
                                                      orig_req_ptr->request_id,
                                                      TRUE);
        break;
      case lpa_UimLpaUserEventId_UIM_LPA_DELETE_PROFILE:
        result = qcril_uim_lpa_delete_profile(&params_data,
                                              orig_req_ptr->data.lpa_profile_info.iccid,
                                              TRUE);
        break;
      default:
        break;
    }
    if (result != lpa_Error_UIM_LPA_ERR_SUCCESS)
    {
      transaction_resp.event  = orig_req_ptr->request_id;
      transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;
      qcril_uim_lpa_socket_send(TRUE,
                                (RIL_Token)orig_req_ptr->token,
                                lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                                lpa_MessageId_UIM_LPA_USER_REQUEST,
                                TRUE,
                                result,
                                &transaction_resp,
                                sizeof(lpa_UimLpaUserResp));
    }
  }

  if (profiles_data_pptr == NULL)
  {
    return;
  }

  /* Clean up memory allocated */
  for (i = 0; i < no_of_profiles; i++)
  {
    lpa_UimLpaProfileInfo  * profile_ptr = NULL;

    profile_ptr = profiles_data_pptr[i];

    if (profile_ptr != NULL)
    {
      if (profile_ptr->iccid.arg != NULL)
      {
        qcril_binary_data_type *iccid_ptr = (qcril_binary_data_type *)profile_ptr->iccid.arg;

        QCRIL_UIM_FREE_IF_NOT_NULL(iccid_ptr->data);
        QCRIL_UIM_FREE_IF_NOT_NULL(iccid_ptr);
      }

      if (profile_ptr->profileName.arg != NULL)
      {
        char *profileName_ptr = (char *)profile_ptr->profileName.arg;

        QCRIL_UIM_FREE_IF_NOT_NULL(profileName_ptr);
      }

      if (profile_ptr->nickname.arg != NULL)
      {
        char *nickname_ptr = (char *)profile_ptr->nickname.arg;

        QCRIL_UIM_FREE_IF_NOT_NULL(nickname_ptr);
      }

      if (profile_ptr->spName.arg != NULL)
      {
        char *spname_ptr = (char *)profile_ptr->spName.arg;

        QCRIL_UIM_FREE_IF_NOT_NULL(spname_ptr);
      }

      if (profile_ptr->icon.arg != NULL)
      {
        qcril_binary_data_type *icon_ptr = (qcril_binary_data_type *)profile_ptr->icon.arg;

        QCRIL_UIM_FREE_IF_NOT_NULL(icon_ptr->data);
        QCRIL_UIM_FREE_IF_NOT_NULL(icon_ptr);
      }
    }
    QCRIL_UIM_FREE_IF_NOT_NULL(profile_ptr);
  }
  QCRIL_UIM_FREE_IF_NOT_NULL(profiles_data_pptr);
} /* qcril_uim_lpa_get_profile_info_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_eid_resp

===========================================================================*/
static void qcril_uim_lpa_get_eid_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type  * orig_req_ptr = NULL;
  uim_get_eid_resp_msg_v01         * eid_rsp_ptr  = NULL;
  lpa_UimLpaUserResp                 transaction_resp;
  lpa_Error                          result       = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  qcril_binary_data_type             eid;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr = callback_params_ptr->orig_req_ptr;
  eid_rsp_ptr  = (uim_get_eid_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&eid, 0, sizeof(qcril_binary_data_type));
  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  transaction_resp.event = lpa_UimLpaUserEventId_UIM_LPA_GET_EID;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (eid_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL eid_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("EID QMI response: 0x%x, eid_valid: 0x%x eid_len = 0x%x",
                   eid_rsp_ptr->resp.result, eid_rsp_ptr->eid_value_valid, eid_rsp_ptr->eid_value_len);

  if (eid_rsp_ptr->resp.result!= QMI_RESULT_SUCCESS_V01 ||
      eid_rsp_ptr->eid_value_valid == FALSE ||
      eid_rsp_ptr->eid_value_len == 0 ||
      eid_rsp_ptr->eid_value_len > QMI_UIM_EID_LEN_V01)
  {
    goto send_response;
  }

  /* data allocated will be deleted by the proto buffer framework */
  eid.data = qcril_malloc(sizeof(uint8_t) * eid_rsp_ptr->eid_value_len);
  if (eid.data == NULL)
  {
    QCRIL_LOG_ERROR("NULL eid data");
    goto send_response;
  }

  memcpy(eid.data, eid_rsp_ptr->eid_value, eid_rsp_ptr->eid_value_len);
  eid.len = eid_rsp_ptr->eid_value_len;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  transaction_resp.eid.arg = &eid;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

send_response:
  qcril_uim_lpa_socket_send(TRUE,
                            (RIL_Token)orig_req_ptr->token,
                            lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                            lpa_MessageId_UIM_LPA_USER_REQUEST,
                            TRUE,
                            result,
                            &transaction_resp,
                            sizeof(lpa_UimLpaUserResp));

 /* Clean up memory allocated */
 QCRIL_UIM_FREE_IF_NOT_NULL(eid.data);
} /* qcril_uim_lpa_get_eid_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_enable_disable_profile_resp

===========================================================================*/
static void qcril_uim_lpa_enable_disable_profile_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type  * orig_req_ptr            = NULL;
  uim_set_sim_profile_resp_msg_v01 * en_dis_profile_rsp_ptr  = NULL;
  lpa_UimLpaUserResp                 transaction_resp;
  lpa_Error                          result                  = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr            = callback_params_ptr->orig_req_ptr;
  en_dis_profile_rsp_ptr  = (uim_set_sim_profile_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  if (orig_req_ptr->data.en_dis_profile)
  {
    transaction_resp.event  = lpa_UimLpaUserEventId_UIM_LPA_ENABLE_PROFILE;
  }
  else
  {
    transaction_resp.event  = lpa_UimLpaUserEventId_UIM_LPA_DISABLE_PROFILE;
  }
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (en_dis_profile_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL enable_disable_profile_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("QMI response: 0x%x", en_dis_profile_rsp_ptr->resp.result);

  if (en_dis_profile_rsp_ptr->resp.result!= QMI_RESULT_SUCCESS_V01)
  {
    goto send_response;
  }

  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

send_response:
  qcril_uim_lpa_socket_send(TRUE,
                            (RIL_Token)orig_req_ptr->token,
                            lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                            lpa_MessageId_UIM_LPA_USER_REQUEST,
                            TRUE,
                            result,
                            &transaction_resp,
                            sizeof(lpa_UimLpaUserResp));
} /* qcril_uim_lpa_enable_disable_profile_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_delete_profile_resp

===========================================================================*/
static void qcril_uim_lpa_delete_profile_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type * orig_req_ptr         = NULL;
  uim_delete_profile_resp_msg_v01 * del_profile_rsp_ptr  = NULL;
  lpa_UimLpaUserResp                transaction_resp;
  lpa_Error                         result               = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr         = callback_params_ptr->orig_req_ptr;
  del_profile_rsp_ptr  = (uim_delete_profile_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  transaction_resp.event  = lpa_UimLpaUserEventId_UIM_LPA_DELETE_PROFILE;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (del_profile_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL del_profile_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("Nick Name QMI response: 0x%x", del_profile_rsp_ptr->resp.result);

  if (del_profile_rsp_ptr->resp.result!= QMI_RESULT_SUCCESS_V01)
  {
    goto send_response;
  }

  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

  /* Reset the iccid cache for the deleted profile */
  if (orig_req_ptr->data.profile_id > 0 &&
      orig_req_ptr->data.profile_id <= QMI_UIM_PROFILES_MAX_V01)
  {
    memset(qcril_uim_lpa_profileId_iccId[orig_req_ptr->data.profile_id - 1], 0xFF, QCRIL_UIM_ICCID_LEN);
  }

send_response:
  qcril_uim_lpa_socket_send(TRUE,
                            (RIL_Token)orig_req_ptr->token,
                            lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                            lpa_MessageId_UIM_LPA_USER_REQUEST,
                            TRUE,
                            result,
                            &transaction_resp,
                            sizeof(lpa_UimLpaUserResp));
} /* qcril_uim_lpa_delete_profile_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_memory_reset_resp

===========================================================================*/
static void qcril_uim_lpa_memory_reset_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type      * orig_req_ptr   = NULL;
  uim_euicc_memory_reset_resp_msg_v01  * reset_rsp_ptr  = NULL;
  lpa_UimLpaUserResp                     transaction_resp;
  lpa_Error                              result         = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr   = callback_params_ptr->orig_req_ptr;
  reset_rsp_ptr  = (uim_euicc_memory_reset_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  transaction_resp.event = lpa_UimLpaUserEventId_UIM_LPA_EUICC_MEMORY_RESET;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (reset_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL reset_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("Memory reset QMI response: 0x%x", reset_rsp_ptr->resp.result);

  if (reset_rsp_ptr->resp.result!= QMI_RESULT_SUCCESS_V01)
  {
    goto send_response;
  }

  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

send_response:
  qcril_uim_lpa_socket_send(TRUE,
                            (RIL_Token)orig_req_ptr->token,
                            lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                            lpa_MessageId_UIM_LPA_USER_REQUEST,
                            TRUE,
                            result,
                            &transaction_resp,
                            sizeof(lpa_UimLpaUserResp));
} /* qcril_uim_lpa_memory_reset_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile_resp

===========================================================================*/
static void qcril_uim_lpa_add_profile_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type * orig_req_ptr         = NULL;
  uim_add_profile_resp_msg_v01    * add_profile_rsp_ptr  = NULL;
  lpa_UimLpaUserResp                transaction_resp;
  lpa_Error                         result               = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr         = callback_params_ptr->orig_req_ptr;
  add_profile_rsp_ptr  = (uim_add_profile_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  transaction_resp.event = lpa_UimLpaUserEventId_UIM_LPA_ADD_PROFILE;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (add_profile_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL add_profile_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("Add profile QMI response: 0x%x", add_profile_rsp_ptr->resp.result);

  if (add_profile_rsp_ptr->resp.result!= QMI_RESULT_SUCCESS_V01)
  {
    goto send_response;
  }

  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

send_response:
  qcril_uim_lpa_socket_send(TRUE,
                            (RIL_Token)orig_req_ptr->token,
                            lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                            lpa_MessageId_UIM_LPA_USER_REQUEST,
                            TRUE,
                            result,
                            &transaction_resp,
                            sizeof(lpa_UimLpaUserResp));
} /* qcril_uim_lpa_add_profile_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_update_profile_nickname_resp

===========================================================================*/
static void qcril_uim_lpa_update_profile_nickname_resp
(
  qcril_uim_lpa_cb_params_type      * callback_params_ptr
)
{
  qcril_uim_original_request_type           * orig_req_ptr         = NULL;
  uim_update_profile_nickname_resp_msg_v01  * nick_name_rsp_ptr    = NULL;
  lpa_UimLpaUserResp                          transaction_resp;
  lpa_Error                                   result               = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;

  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL callback_params_ptr");
    return;
  }

  orig_req_ptr         = callback_params_ptr->orig_req_ptr;
  nick_name_rsp_ptr    = (uim_update_profile_nickname_resp_msg_v01 *)callback_params_ptr->msg_ptr;

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL original_req_ptr");
    return;
  }

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  transaction_resp.event  = lpa_UimLpaUserEventId_UIM_LPA_UPDATE_NICKNAME;
  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;

  if (callback_params_ptr->transp_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Transp error (%d) recieved from QMI for RIL request",
                                            callback_params_ptr->transp_err);
    goto send_response;
  }

  if (nick_name_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL add_profile_rsp_ptr");
    goto send_response;
  }

  QCRIL_LOG_DEBUG("Nick Name QMI response: 0x%x", nick_name_rsp_ptr->resp.result);

  if (nick_name_rsp_ptr->resp.result!= QMI_RESULT_SUCCESS_V01)
  {
    goto send_response;
  }

  transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS;
  result = lpa_Error_UIM_LPA_ERR_SUCCESS;

send_response:
  qcril_uim_lpa_socket_send(TRUE,
                            (RIL_Token)orig_req_ptr->token,
                            lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                            lpa_MessageId_UIM_LPA_USER_REQUEST,
                            TRUE,
                            result,
                            &transaction_resp,
                            sizeof(lpa_UimLpaUserResp));
} /* qcril_uim_lpa_update_profile_nickname_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/
/*=========================================================================

  FUNCTION:  qcril_uim_lpa_cleanup_cached_iccids

===========================================================================*/
/*!
    @brief
    Resets the cached iccids.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_lpa_cleanup_cached_iccids
(
)
{
  int i = 0;

  QCRIL_LOG_DEBUG("qcril_uim_lpa_cleanup_cached_iccids");

  for (i = 0; i < QMI_UIM_PROFILES_MAX_V01; i++)
  {
    memset(qcril_uim_lpa_profileId_iccId[i], 0xFF, QCRIL_UIM_ICCID_LEN);
  }
  qcril_uim_lpa_profiles_cached = FALSE;
}


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_process_qmi_callback

===========================================================================*/
/*!
    @brief
    Handles MessageId_QCRIL_EVT_UIM_LPA_QMI_COMMAND_CALLBACK.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_lpa_process_qmi_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_lpa_cb_params_type  * callback_params_ptr = NULL;

  /* Sanity checks */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return;
  }

  callback_params_ptr = (qcril_uim_lpa_cb_params_type*)params_ptr->data;
  if (callback_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, NULL callback_params_ptr");
    return;
  }

  if (callback_params_ptr->msg_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL msg_ptr, cannot process response");
    goto clean_up;
  }

  if (callback_params_ptr->orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL orig_req_data, cannot process response");
    goto clean_up;
  }

  if ((callback_params_ptr->orig_req_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (callback_params_ptr->orig_req_ptr->modem_id    >= QCRIL_MAX_MODEM_ID))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, modem_id: 0x%x",
                    callback_params_ptr->orig_req_ptr->instance_id,
                    callback_params_ptr->orig_req_ptr->modem_id);
    goto clean_up;
  }

  QCRIL_LOG_DEBUG("%s: Response for msg_id: 0x%x, token=%d, transp_err: 0x%x",
                  __FUNCTION__,
                  callback_params_ptr->msg_id,
                  qcril_log_get_token_id((RIL_Token)callback_params_ptr->orig_req_ptr->token),
                  callback_params_ptr->transp_err);

  switch(callback_params_ptr->msg_id)
  {
    case QMI_UIM_GET_SIM_PROFILE_RESP_V01:
      qcril_uim_lpa_get_profile_info_resp(callback_params_ptr);
      break;

    case QMI_UIM_GET_EID_RESP_V01:
      qcril_uim_lpa_get_eid_resp(callback_params_ptr);
      break;

    case QMI_UIM_DELETE_PROFILE_RESP_V01:
      qcril_uim_lpa_delete_profile_resp(callback_params_ptr);
      break;

    case QMI_UIM_EUICC_MEMORY_RESET_RESP_V01:
      qcril_uim_lpa_memory_reset_resp(callback_params_ptr);
      break;

    case QMI_UIM_ADD_PROFILE_RESP_V01:
      qcril_uim_lpa_add_profile_resp(callback_params_ptr);
      break;

    case QMI_UIM_UPDATE_PROFILE_NICKNAME_RESP_V01:
      qcril_uim_lpa_update_profile_nickname_resp(callback_params_ptr);
      break;

    case QMI_UIM_SET_SIM_PROFILE_RESP_V01:
      qcril_uim_lpa_enable_disable_profile_resp(callback_params_ptr);
      break;

    default:
      /* This shouldn't happen since we never post for these msg ids */
      QCRIL_LOG_ERROR("Unsupported QMI UIM LPA response: 0x%x",
                      callback_params_ptr->msg_id);
      break;
  }

clean_up:
  /* Free memory allocated previously. Note that all the frees are
     performed here rather than at individual handler functions */
  QCRIL_UIM_FREE_IF_NOT_NULL(callback_params_ptr->orig_req_ptr);
  QCRIL_UIM_FREE_IF_NOT_NULL(callback_params_ptr->msg_ptr);
  QCRIL_UIM_FREE_IF_NOT_NULL(callback_params_ptr);
} /* qcril_uim_lpa_process_qmi_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile_ind

===========================================================================*/
void qcril_uim_lpa_add_profile_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
)
{
  lpa_UimLpaAddProfileProgressInd add_profile_ind;

  memset(&add_profile_ind, 0, sizeof(lpa_UimLpaAddProfileProgressInd));

  if (ind_param_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Null indication ptr");
    return;
  }

  switch(ind_param_ptr->ind_data.lpa_add_profile_ind.profile_download_status)
  {
    case QMI_UIM_PROFILE_DOWNLOAD_ERROR:
      add_profile_ind.status = lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_ERROR;
      break;
    case QMI_UIM_PROFILE_DOWNLOAD_IN_PROGRESS:
      add_profile_ind.status = lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS;
      break;
    case QMI_UIM_PROFILE_DOWNLOAD_COMPLETE_INSTALLATION_IN_PROGRESS:
      add_profile_ind.status = lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS;
      break;
    case QMI_UIM_PROFILE_INSTALLATION_COMPLETE:
      add_profile_ind.status = lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE;
      break;
    default:
      return;
  }

  add_profile_ind.has_cause    = ind_param_ptr->ind_data.lpa_add_profile_ind.error_cause_valid;
  switch(ind_param_ptr->ind_data.lpa_add_profile_ind.error_cause)
  {
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_GENERIC:
      add_profile_ind.cause = lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_GENERIC;
      break;
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_SIM:
      add_profile_ind.cause = lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_SIM;
      break;
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_NETWORK:
      add_profile_ind.cause = lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NETWORK;
      break;
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_MEMORY:
      add_profile_ind.cause = lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_MEMORY;
      break;
    default:
      add_profile_ind.cause = lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NONE;
      break;
  }

  add_profile_ind.has_progress = ind_param_ptr->ind_data.lpa_add_profile_ind.percentage_valid;
  add_profile_ind.progress     = (int32_t)ind_param_ptr->ind_data.lpa_add_profile_ind.percentage;

  qcril_uim_lpa_socket_send(FALSE,
                            0,
                            lpa_MessageType_UIM_LPA_MSG_INDICATION,
                            lpa_MessageId_UIM_LPA_ADD_PROFILE_PROGRESS_IND,
                            FALSE,
                            lpa_Error_UIM_LPA_ERR_SUCCESS,
                            &add_profile_ind,
                            sizeof(lpa_UimLpaAddProfileProgressInd));
} /* qcril_uim_lpa_add_profile_ind */


/*===========================================================================
  FUNCTION  qcril_qmi_uim_lpa_add_profile_ind_hdlr
===========================================================================*/
int qcril_qmi_uim_lpa_add_profile_ind_hdlr
(
  uim_add_profile_ind_msg_v01               * qcci_data_ptr,
  qmi_uim_indication_data_type              * qmi_data_ptr
)
{
  if ((qcci_data_ptr == NULL) || (qmi_data_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process indication");
    return QMI_SERVICE_ERR;
  }

  qmi_data_ptr->lpa_add_profile_ind.slot = (qmi_uim_slot_type)qcci_data_ptr->slot;
  qmi_data_ptr->lpa_add_profile_ind.profile_download_status =
         (qmi_uim_lpa_download_status_type)qcci_data_ptr->profile_download_status;
  qmi_data_ptr->lpa_add_profile_ind.error_cause_valid       =
                                        (boolean)qcci_data_ptr->error_cause_valid;
  qmi_data_ptr->lpa_add_profile_ind.error_cause             =
                (qmi_uim_lpa_download_error_cause_type)qcci_data_ptr->error_cause;
  qmi_data_ptr->lpa_add_profile_ind.percentage_valid        =
                                         (boolean)qcci_data_ptr->percentage_valid;
  qmi_data_ptr->lpa_add_profile_ind.percentage = qcci_data_ptr->percentage;

  return 0;
} /* qcril_qmi_uim_lpa_add_profile_ind_hdlr */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_user_request

===========================================================================*/
/*!
    @brief
    Handles MessageId_UIM_LPA_USER_REQUEST from the client.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_lpa_user_request
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  lpa_UimLpaUserReq                * in_ptr                     = NULL;
  lpa_UimLpaUserResp                 transaction_resp;
  lpa_Error                          result                     = lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE;
  char                             * activationCode_ptr         = NULL;
  char                             * confirmationCode_ptr       = NULL;
  qcril_binary_data_type           * iccid_ptr                  = NULL;
  uint8_t                            iccid[QCRIL_UIM_ICCID_LEN] = {0xFF};
  char                             * nick_name_ptr              = NULL;

  memset(&transaction_resp, 0, sizeof(lpa_UimLpaUserResp));
  memset(iccid, 0xFF, sizeof(iccid));

  /* Sanity check */
  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Parse input info */
  in_ptr = (lpa_UimLpaUserReq *)(params_ptr->data);
  if(in_ptr != NULL)
  {
    QCRIL_LOG_INFO("qcril_uim_lpa_transaction_request event : 0x%x",
                        in_ptr->event);

    transaction_resp.event = in_ptr->event;

    switch(in_ptr->event)
    {
      case lpa_UimLpaUserEventId_UIM_LPA_ADD_PROFILE:
        activationCode_ptr = (char *)in_ptr->activationCode.arg;
        if (activationCode_ptr != NULL && strlen(activationCode_ptr) != 0)
        {
          confirmationCode_ptr = (char *)in_ptr->confirmationCode.arg;
          result = qcril_uim_lpa_add_profile(params_ptr, activationCode_ptr, confirmationCode_ptr);
        }
        else
        {
          result = lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
        }
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_ENABLE_PROFILE:
      case lpa_UimLpaUserEventId_UIM_LPA_DISABLE_PROFILE:
        result = lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
        iccid_ptr = (qcril_binary_data_type *)in_ptr->iccid.arg;
        if (iccid_ptr->data != NULL && iccid_ptr->len != 0)
        {
          if (qcril_uim_lpa_byte_to_iccid(iccid,
                                          QCRIL_UIM_ICCID_LEN,
                                          iccid_ptr->data,
                                          iccid_ptr->len) > 0)
          {
            int i = 0;
            for ( i = 0;  i< QCRIL_UIM_ICCID_LEN ; i++)
            {
              QCRIL_LOG_INFO("iccid [%d]:0x%x", i, iccid[i]);
            }
            result = qcril_uim_lpa_enable_disable_profile(params_ptr, iccid, in_ptr->event, FALSE);
          }
        }
        else
        {
          result = lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
        }
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_DELETE_PROFILE:
        iccid_ptr = (qcril_binary_data_type *)in_ptr->iccid.arg;
        if (iccid_ptr->data != NULL && iccid_ptr->len != 0)
        {
          if (qcril_uim_lpa_byte_to_iccid(iccid,
                                          QCRIL_UIM_ICCID_LEN,
                                          iccid_ptr->data,
                                          iccid_ptr->len) > 0)
          {
            int i = 0;
            for ( i = 0;  i< QCRIL_UIM_ICCID_LEN ; i++)
            {
              QCRIL_LOG_INFO("iccid [%d]:0x%x", i, iccid[i]);
            }
            result = qcril_uim_lpa_delete_profile(params_ptr, iccid, FALSE);
          }
        }
        else
        {
          result = lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
        }
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_EUICC_MEMORY_RESET:
        result = qcril_uim_lpa_memory_reset(params_ptr);
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_GET_PROFILE:
        result = qcril_uim_lpa_get_profile(params_ptr,
                                           NULL,
                                           lpa_UimLpaUserEventId_UIM_LPA_GET_PROFILE);
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_UPDATE_NICKNAME:
        iccid_ptr = (qcril_binary_data_type *)in_ptr->iccid.arg;
        if (iccid_ptr->data != NULL && iccid_ptr->len != 0)
        {
          if (qcril_uim_lpa_byte_to_iccid(iccid,
                                          QCRIL_UIM_ICCID_LEN,
                                          iccid_ptr->data,
                                          iccid_ptr->len) > 0)
          {
            int i = 0;
            for ( i = 0;  i< QCRIL_UIM_ICCID_LEN ; i++)
            {
              QCRIL_LOG_INFO("iccid [%d]:0x%x", i, iccid[i]);
            }
            nick_name_ptr = (char *)in_ptr->nickname.arg;
            result = qcril_uim_lpa_update_nickname(params_ptr, iccid, nick_name_ptr);
          }
        }
        else
        {
          result = lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
        }
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_GET_EID:
        result = qcril_uim_lpa_get_eid(params_ptr);
        break;

      case lpa_UimLpaUserEventId_UIM_LPA_UNKNOWN_EVENT_ID:
        result = lpa_Error_UIM_LPA_ERR_NOT_SUPPORTED;
        break;

      default:
        result = lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER;
        break;
    }
  }

  if (result != lpa_Error_UIM_LPA_ERR_SUCCESS)
  {
    /* In any case of error, send response back to client */
    QCRIL_LOG_ERROR("Error in qcril_uim_lpa_user_request, result: 0x%x", result);
    transaction_resp.result = lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE;
    qcril_uim_lpa_socket_send(TRUE,
                              (RIL_Token)params_ptr->t,
                              lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                              lpa_MessageId_UIM_LPA_USER_REQUEST,
                              TRUE,
                              result,
                              &transaction_resp,
                              sizeof(lpa_UimLpaUserResp));
  }
} /* qcril_uim_lpa_user_request */
