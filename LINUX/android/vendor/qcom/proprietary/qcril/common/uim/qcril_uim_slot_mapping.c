/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_QCRIL_UIM_QMI)

#include "ril.h"
#include "IxErrno.h"
#include "qcrili.h"
#include "qcril_log.h"
#include "qcril_reqlist.h"
#include "qcril_uim.h"
#include "qcril_uim_queue.h"
#include "qcril_uim_qcci.h"
#include "qcril_uim_util.h"
#include "qcril_uim_slot_mapping.h"

/*===========================================================================

                               GLOBAL VARIABLES

===========================================================================*/


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_get_physical_slot_mapping_resp

===========================================================================*/
/*!
    @brief
    Processes the response for physical slot mapping command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_physical_slot_mapping_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err                  = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qcril_uim_original_request_type * original_request_ptr     = NULL;
  uint8                             slot_id                  = 0;
  RIL_PhySlotMap                    logical_phy_slot_mapping;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  memset(&logical_phy_slot_mapping, 0, sizeof(logical_phy_slot_mapping));

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_get_physical_slot_mapping_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  logical_phy_slot_mapping.array_len = params_ptr->qmi_rsp_data.rsp_data.phy_slot_mapping_rsp.num_slots;

  if(logical_phy_slot_mapping.array_len <= QMI_UIM_MAX_CARD_COUNT)
  {
    for (slot_id = 0; slot_id < logical_phy_slot_mapping.array_len; slot_id++)
    {
      switch(params_ptr->qmi_rsp_data.rsp_data.phy_slot_mapping_rsp.logical_slot[slot_id])
      {
        case QMI_UIM_PHY_SLOT_1:
          logical_phy_slot_mapping.slot_map[slot_id] = 0;
          break;
        case QMI_UIM_PHY_SLOT_2:
          logical_phy_slot_mapping.slot_map[slot_id] = 1;
          break;
        case QMI_UIM_PHY_SLOT_3:
          logical_phy_slot_mapping.slot_map[slot_id] = 2;
          break;
        default:
          ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
          break;
      }
    }
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Send the response */
  if(ril_err == RIL_E_SUCCESS)
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       &logical_phy_slot_mapping,
                       sizeof(logical_phy_slot_mapping),
                       TRUE,
                       NULL);
  }
  else
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       NULL,
                       0,
                       TRUE,
                       NULL);
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_get_physical_slot_mapping_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_switch_slot_resp

===========================================================================*/
/*!
    @brief
    Processes the response for switch physical slot command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_switch_slot_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_switch_slot_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* Send the response actual response will be in payload */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     RIL_E_SUCCESS,
                     &ril_err,
                     sizeof(RIL_Errno),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_switch_slot_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_uim_request_get_physical_slot_mapping

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ request from the
    framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_physical_slot_mapping
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type * original_req_ptr = NULL;
  uint8                             dummy            = 0;
  RIL_Errno                         ril_err          = QCRIL_UIM_RIL_E_INTERNAL_ERR;  

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity check */
  if ((params_ptr == NULL) ||
      (ret_ptr    == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                     params_ptr->modem_id,
                                                     params_ptr->t,
                                                     params_ptr->event_id,
                                                     0);
  if (original_req_ptr != NULL)
  {
    if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_GET_PHYSICAL_SLOT_MAPPING,
                                     qcril_uim.qmi_handle,
                                     &dummy,
                                     qmi_uim_callback,
                                     (void*)original_req_ptr) >= 0)
    {
      return;
    }
    ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
  }

  /* Send the response */
  qcril_uim_response( params_ptr->instance_id,
                      params_ptr->t,
                      ril_err,
                      NULL,
                      0,
                      TRUE,
                      NULL );

  /* Clean up any original request if allocated */
  if (original_req_ptr)
  {
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
} /* qcril_uim_request_get_physical_slot_mapping */


/*===========================================================================

  FUNCTION:  qcril_uim_request_switch_slot

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SWITCH_SLOT_REQ request from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_switch_slot
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type   * original_req_ptr  = NULL;
  qmi_uim_physical_slot_mapping_type  params;
  uint8                               slot_id           = 0;
  RIL_PhySlotMap                    * logic_phy_mapping = NULL;
  RIL_Errno                           ril_error         = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity check */
  if ((params_ptr == NULL) ||
      (ret_ptr    == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  if(params_ptr->data != NULL)
  {
    logic_phy_mapping = (RIL_PhySlotMap *)params_ptr->data;
    params.num_slots  = logic_phy_mapping->array_len;
    if(params.num_slots <= QMI_UIM_MAX_CARD_COUNT)
    {
      for(slot_id = 0; slot_id < params.num_slots; slot_id++)
      {
        /* Find slot info */
        switch(logic_phy_mapping->slot_map[slot_id])
        {
          case 0:
            params.logical_slot[slot_id] = QMI_UIM_PHY_SLOT_1;
            break;
          case 1:
            params.logical_slot[slot_id] = QMI_UIM_PHY_SLOT_2;
            break;
          case 2:
            params.logical_slot[slot_id] = QMI_UIM_PHY_SLOT_3;
            break;
          default:
            /* Send the response, actual response will be in payload */
            ril_error = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
            qcril_uim_response( params_ptr->instance_id,
                                params_ptr->t,
                                RIL_E_SUCCESS,
                                &ril_error,
                                sizeof(RIL_Errno),
                                TRUE,
                                NULL );
            return;
        }
      }

      /* Allocate original request */
      original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         params_ptr->modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         0);
      if (original_req_ptr != NULL)
      {
        if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SWITCH_SLOT_EXT,
                                         qcril_uim.qmi_handle,
                                         &params,
                                         qmi_uim_callback,
                                         (void*)original_req_ptr) >= 0)
        {
          return;
        }
        ril_error = QCRIL_UIM_RIL_E_MODEM_ERR;
      }
      else
      {
        ril_error = QCRIL_UIM_RIL_E_NO_MEMORY;
      }
    }
  }

  /* Send the response, actual response will be in payload */
  qcril_uim_response( params_ptr->instance_id,
                      params_ptr->t,
                      RIL_E_SUCCESS,
                      &ril_error,
                      sizeof(RIL_Errno),
                      TRUE,
                      NULL );

  /* Clean up any original request if allocated */
  if (original_req_ptr)
  {
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
} /* qcril_uim_request_switch_slot */


#endif /* defined (FEATURE_QCRIL_UIM_QMI) */

