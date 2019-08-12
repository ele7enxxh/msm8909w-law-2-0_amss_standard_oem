
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


               M M G S D I   U I M   C O M M O N   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the protocol processing support that is common
  to both UICC and ICC protocols before the command is sent to UIM Module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uim_common.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   ar      Review of macros used by MMGSDI
05/16/16   ar      F3 reduction
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
04/22/14   at      Critical KW fixes
01/22/14   yt      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uim_common.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_common_rsp.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called to send Compute IP Auth request to UIM.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_slot_id_enum_type   mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;
  mmgsdi_channel_id_type     channel_id       = MMGSDI_DEFAULT_CHANNEL;
  uim_channel_type           uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }
  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot,&uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    /* Convert to UIM Channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  uim_cmd_ptr->hdr.command               = UIM_COMPUTE_IP_AUTH_F;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel;
  uim_cmd_ptr->hdr.protocol              = (protocol == MMGSDI_UICC) ?
                                           UIM_UICC : UIM_CDMA;

  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed when processing the
  response
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* ------------------------------------------------------------------------
      Set COMPUTE IP Authentication data
    ----------------------------------------------------------------------*/
  switch(req_ptr->compute_ip_data.operation_type)
  {
    case MMGSDI_CMPT_IP_SIP_CHAP:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_CHAP;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge_length =
        int32touint8(req_ptr->compute_ip_data.data.sip_chap_data.chap_challenge_length);
      mmgsdi_memscpy(
        (void*)uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge,
        sizeof(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge),
        (void*)req_ptr->compute_ip_data.data.sip_chap_data.chap_challenge,
        uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_challenge_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.chap_id =
        req_ptr->compute_ip_data.data.sip_chap_data.chap_id;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.chap_data.nai_entry_index =
        req_ptr->compute_ip_data.data.sip_chap_data.nai_entry_index;
      break;

    case MMGSDI_CMPT_IP_MIP_MN_HA:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MN_HA_AUTH;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data_length =
        int32touint16(req_ptr->compute_ip_data.data.mn_ha_data.registration_data_length);
      mmgsdi_memscpy(
        (void*)uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data,
        sizeof(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data),
        (void*)req_ptr->compute_ip_data.data.mn_ha_data.registration_data_ptr,
        uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index =
        req_ptr->compute_ip_data.data.mn_ha_data.nai_entry_index;
      break;

    case MMGSDI_CMPT_IP_MIP_RRQ:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MIP_RRQ;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data_length =
        int32touint16(req_ptr->compute_ip_data.data.rrq_data.rrq_data_length);
      mmgsdi_memscpy(
        (void*)uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
        sizeof(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data),
        (void*)req_ptr->compute_ip_data.data.rrq_data.rrq_data_ptr,
        uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data_length);
      break;

    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MN_AAA_AUTH;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge_length =
        int32touint8(req_ptr->compute_ip_data.data.mn_aaa_data.challenge_length);
      mmgsdi_memscpy(
        (void*)uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge,
        sizeof(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge),
        (void*)req_ptr->compute_ip_data.data.mn_aaa_data.challenge_ptr,
        uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.mn_aaa_data.nai_entry_index =
        req_ptr->compute_ip_data.data.mn_aaa_data.nai_entry_index;
      break;
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_HRPD_AN_CHAP;
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge_length =
        int32touint8(req_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_length);
      mmgsdi_memscpy(
        (void*)uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge,
        sizeof(uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge),
        (void*)req_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
        uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_challenge_length);
      uim_cmd_ptr->compute_ip_auth.cmpt_ip_data.hrpd_data.chap_id =
        req_ptr->compute_ip_data.data.hrpd_chap_data.chap_id;
      break;

    default:
      UIM_MSG_ERR_0("Invalid operation type");
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_common_compute_ip_auth */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_RUN_CAVE

   DESCRIPTION
     This function is called to send RUN CAVE ALGORITHM request to UIM.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_cave_esn_type*      temp_esn_ptr     = NULL;
  mmgsdi_slot_id_enum_type   mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;
  mmgsdi_channel_id_type     channel_id       = MMGSDI_DEFAULT_CHANNEL;
  uim_channel_type           uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }
  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot,&uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    /* Convert to UIM Channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  uim_cmd_ptr->hdr.command               = UIM_RUN_CAVE_F;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel;
  uim_cmd_ptr->hdr.protocol              = (protocol == MMGSDI_UICC) ?
                                           UIM_UICC : UIM_CDMA;

  /* ------------------------------------------------------------------------
      Set RUN CAVE Algo data
    ----------------------------------------------------------------------*/

  uim_cmd_ptr->run_cave.dig_len             = req_ptr->dig_len;
  /* req_ptr->digits is a an array of uint8s */
  mmgsdi_memscpy(uim_cmd_ptr->run_cave.digits,
                 sizeof(uim_cmd_ptr->run_cave.digits),
                 req_ptr->digits,
                 sizeof(mmgsdi_cave_digits_type));
  /* Need to move data to temp variable since qw_esn can't take const variable. We
     are not using & of esn type in assignment since it is originally defined as static
     array type in qw.h  */
  temp_esn_ptr = (mmgsdi_cave_esn_type*)(req_ptr->esn);
  qw_equ(uim_cmd_ptr->run_cave.esn, (*temp_esn_ptr));
  uim_cmd_ptr->run_cave.process_control     = req_ptr->process_control;
  uim_cmd_ptr->run_cave.rand_chal           = req_ptr->rand_chal;
  uim_cmd_ptr->run_cave.rand_type           = req_ptr->rand_type;

  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed when processing the
  response
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_run_cave */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_CDMA_AKA_AUTH

   DESCRIPTION
     This function is called to send CDMA AKA Authentication request to UIM.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_cdma_aka_auth(
  const mmgsdi_cdma_aka_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_slot_id_enum_type   mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;
  mmgsdi_channel_id_type     channel_id       = MMGSDI_DEFAULT_CHANNEL;
  uim_channel_type           uim_channel      = UIM_CHANNEL0;

  UIM_MSG_HIGH_0("MMGSDI UIM COMMON CDMA AKA AUTH");

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr            - Cannot be NULL
    2. auth token pointer - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->autn.data_ptr);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }
  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot,&uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    /* Convert to UIM Channel ID */
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  if (req_ptr->autn.data_len > UIM_MAX_AKA_AUTH_BYTES)
  {
    UIM_MSG_ERR_1("Invalid authentication token length: 0x%x",req_ptr->autn.data_len);
    return MMGSDI_ERROR;
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  uim_cmd_ptr->hdr.command               = UIM_AKA_AUTH_F;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel;
  uim_cmd_ptr->hdr.protocol              = (protocol == MMGSDI_UICC) ?
                                           UIM_UICC : UIM_ICC;

  /* ------------------------------------------------------------------------
      Set CDMA AKA Authentication data
    ----------------------------------------------------------------------*/
  switch (req_ptr->aka_cmd)
  {
    case MMGSDI_CDMA_AKA_AUTH_3G:
      uim_cmd_ptr->aka_auth.op_type = UIM_AKA_AUTH_3G;
      break;
    case MMGSDI_CDMA_AKA_AUTH_WLAN:
      uim_cmd_ptr->aka_auth.op_type = UIM_AKA_AUTH_WLAN;
      break;
    default:
      UIM_MSG_ERR_1("Invalid CDMA AKA authentication op type: 0x%x", req_ptr->aka_cmd);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return MMGSDI_ERROR;
  }
  memscpy(uim_cmd_ptr->aka_auth.randa,
          sizeof(uim_cmd_ptr->aka_auth.randa),
          req_ptr->randa,
          MMGSDI_CDMA_AKA_RANDA_LEN);
  uim_cmd_ptr->aka_auth.autn_len                    = (byte)req_ptr->autn.data_len;
  memscpy(uim_cmd_ptr->aka_auth.autn,
          sizeof(uim_cmd_ptr->aka_auth.autn),
          req_ptr->autn.data_ptr,
          req_ptr->autn.data_len);

  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed when processing the
  response
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_cdma_aka_auth */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_GENERATE_KEY_VPM

   DESCRIPTION
     This function is called to send Generate Key VPM request to UIM.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_generate_key_vpm(
  const mmgsdi_generate_key_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                    protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type *             uim_cmd_ptr      = NULL;
  uim_slot_type              uim_slot         = UIM_SLOT_NONE;
  int32                      index            = 0;
  mmgsdi_slot_id_enum_type   mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type  *app_info_ptr     = NULL;
  mmgsdi_channel_id_type     channel_id       = MMGSDI_DEFAULT_CHANNEL;
  uim_channel_type           uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }
  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot,&uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  uim_cmd_ptr->hdr.command               = UIM_GENERATE_KEYS_F;
  uim_cmd_ptr->hdr.slot                  = uim_slot;
  uim_cmd_ptr->hdr.channel               = uim_channel;
  uim_cmd_ptr->hdr.protocol              = (protocol == MMGSDI_UICC) ?
                                            UIM_UICC : UIM_ICC;

  /* ------------------------------------------------------------------------
      Set Generate Key / VPM data
    ----------------------------------------------------------------------*/
  uim_cmd_ptr->gen_keys.vpm_first_octet    = req_ptr->first_octet;
  uim_cmd_ptr->gen_keys.vpm_last_octet     = req_ptr->last_octet;

  /*---------------------------------------------------------------------------
  CLIENT REQUEST TABLE :- Store information
  1) Get the index
  2) If not able to get the index, free the uim_cmd_q, but keep the request
  ptr and the uim_rsp_data buffer so that it can be freed when processing the
  response
  3) Populate client request table with req_ptr, data len and uim_rsp_data buffer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                &req_ptr->request_header,
                                                uim_cmd_ptr,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_generate_key_vpm */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_BLOCK

  DESCRIPTION:
    This function will format the OTASP block request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP Block
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_block(
  const mmgsdi_session_otasp_block_req_type *req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr= NULL;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot,&uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                                               MMGSDI_SESSION_OTASP_BLOCK_REQ,
                                               (void*)req_ptr,
                                               &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_0("Population of extra param is failed in UIM UICC OTASP block");
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;

  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  switch(req_ptr->block_op_type)
  {
    case MMGSDI_OTASP_CONFIG:  /* Perform Config */
      /* It does not require block data, ignore if we have any block length */
      uim_cmd_ptr->hdr.command              = UIM_CONFIG_REQ_F;
      uim_cmd_ptr->configuration.block_id   = req_ptr->block_id;
      break;

    case MMGSDI_OTASP_DOWNLOAD: /* Perform Download */
      uim_cmd_ptr->hdr.command        = UIM_DOWNLOAD_REQ_F;
      uim_cmd_ptr->download.block_id  = req_ptr->block_id;
      uim_cmd_ptr->download.block_len = int32touint8(req_ptr->block_data.data_len);
      if (req_ptr->block_data.data_ptr)
      {
        mmgsdi_memscpy((void*)uim_cmd_ptr->download.data,
                       sizeof(uim_cmd_ptr->download.data),
                       (void*)req_ptr->block_data.data_ptr,
                       uim_cmd_ptr->download.block_len);
      }
      break;

    case MMGSDI_OTASP_SSPR_DOWNLOAD: /* Perform SSPR Download */
      uim_cmd_ptr->hdr.command              = UIM_SSPR_DOWNLOAD_REQ_F;
      uim_cmd_ptr->sspr_download.block_id   = req_ptr->block_id;
      uim_cmd_ptr->sspr_download.block_len  = int32touint8(req_ptr->block_data.data_len);
      if (req_ptr->block_data.data_ptr)
      {
        mmgsdi_memscpy((void*)uim_cmd_ptr->sspr_download.data,
                       sizeof(uim_cmd_ptr->sspr_download.data),
                       (void*)req_ptr->block_data.data_ptr,
                       uim_cmd_ptr->sspr_download.block_len);
      }
      break;

    case MMGSDI_OTASP_VALIDATE: /* Perform Validate */
      uim_cmd_ptr->hdr.command           = UIM_VALIDATE_F;
      uim_cmd_ptr->validate.block_id     = req_ptr->block_id;
      uim_cmd_ptr->validate.block_length = int32touint8(req_ptr->block_data.data_len);
      if (req_ptr->block_data.data_ptr)
      {
        mmgsdi_memscpy((void*)uim_cmd_ptr->validate.data,
                       sizeof(uim_cmd_ptr->validate.data),
                       (void*)req_ptr->block_data.data_ptr,
                       uim_cmd_ptr->validate.block_length);
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid value of otasp block op type 0x%x",
                    req_ptr->block_op_type);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      return MMGSDI_INCORRECT_PARAMS;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    UIM_MSG_HIGH_1("Send MMGSDI UIM COMMON OTASP Block 0x%x Request to UIM",
                   req_ptr->block_op_type);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_otasp_block */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_UICC_OTASP_OTAPA

  DESCRIPTION:
    This function will format the OTAPA request in the form required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending the store
                             ESN command to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_otapa (
  const mmgsdi_session_otasp_otapa_req_type *req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
    2. Random seed and ESN shall not be 0x00
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  /* No need to check randseed and esn,
     already checked when preparing mmgsdi command */

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                         req_ptr->request_header.session_id,
                                         &mmgsdi_slot,
                                         &channel_id,
                                         &app_info_ptr,
                                         NULL,
                                         NULL,
                                         NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  if (protocol == MMGSDI_UICC)
  {
    uim_cmd_ptr->hdr.protocol             = UIM_UICC;
    uim_cmd_ptr->hdr.channel              = uim_channel;
    uim_cmd_ptr->otapa_req.meid_available = TRUE;
  }
  else
  {
    uim_cmd_ptr->hdr.protocol             = UIM_CDMA;
    uim_cmd_ptr->hdr.channel              = UIM_CHANNEL0;
    /* check for meid avaliablity */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    if (req_ptr->esn)
    {
      uim_cmd_ptr->otapa_req.meid_available =
        mmgsdi_util_get_meid_availability(req_ptr->request_header.session_id);
    }
#else
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
  uim_cmd_ptr->hdr.slot                 = uim_slot;
  uim_cmd_ptr->hdr.command              = UIM_OTAPA_REQ_F;
  uim_cmd_ptr->otapa_req.start_stop     = req_ptr->start_stop;
  uim_cmd_ptr->otapa_req.randseed       = req_ptr->randseed;
  uim_cmd_ptr->otapa_req.esn            = req_ptr->esn;
  uim_cmd_ptr->hdr.options              = UIM_OPTION_NONE;

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
    index, &req_ptr->request_header, uim_cmd_ptr, NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_otasp_otapa */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_COMMIT

  DESCRIPTION:
    This function will format the OTASP Commit request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_commit (
  const mmgsdi_session_otasp_commit_req_type *req_ptr,
  mmgsdi_protocol_enum_type                   protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_COMMIT_F;

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_otasp_commit */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will format the OTASP SSPR Config request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP SSPR
                             Config CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_sspr_config (
  const mmgsdi_session_otasp_sspr_config_req_type *req_ptr,
  mmgsdi_protocol_enum_type                        protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_SSPR_CONFIG_REQ_F;
  uim_cmd_ptr->sspr_config.block_id         = req_ptr->block_id;
  uim_cmd_ptr->sspr_config.req_offset       = req_ptr->req_offset;
  uim_cmd_ptr->sspr_config.req_max_size     = req_ptr->req_max_size;

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_otasp_sspr_config */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_KEY_GEN

  DESCRIPTION:
    This function will format the OTASP KEY GEN request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP KEY
                             GEN CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_key_gen (
  const mmgsdi_session_otasp_key_gen_req_type *req_ptr,
  mmgsdi_protocol_enum_type                    protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_KEY_GEN_REQ_F;
  uim_cmd_ptr->key_gen.bs_result_len        = int32touint8(req_ptr->bs_result.data_len);
  mmgsdi_memscpy((void*)uim_cmd_ptr->key_gen.bs_result,
                 sizeof(uim_cmd_ptr->key_gen.bs_result),
                 (void*)req_ptr->bs_result.data_ptr,
                 uim_cmd_ptr->key_gen.bs_result_len);

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_otasp_key_gen */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_MS_KEY

  DESCRIPTION:
    This function will format the OTASP MS KEY request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP MS KEY
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_ms_key (
  const mmgsdi_session_otasp_ms_key_req_type *req_ptr,
  mmgsdi_protocol_enum_type                   protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;

  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_MS_KEY_REQ_F;
  uim_cmd_ptr->ms_key_req.a_key_p_rev       = req_ptr->a_key_p_rev;
  uim_cmd_ptr->ms_key_req.param_p_len       = int32touint8(req_ptr->param_p.data_len);
  uim_cmd_ptr->ms_key_req.param_g_len       = int32touint8(req_ptr->param_g.data_len);

  mmgsdi_memscpy((void*)uim_cmd_ptr->ms_key_req.randseed,
                 sizeof(uim_cmd_ptr->ms_key_req.randseed),
                 (void*)req_ptr->randseed.data_ptr,
                 int32touint32(req_ptr->randseed.data_len));
  mmgsdi_memscpy((void*)uim_cmd_ptr->ms_key_req.param_p,
                 sizeof(uim_cmd_ptr->ms_key_req.param_p),
                 (void*)req_ptr->param_p.data_ptr,
                 uim_cmd_ptr->ms_key_req.param_p_len);
  mmgsdi_memscpy((void*)uim_cmd_ptr->ms_key_req.param_g,
                 sizeof(uim_cmd_ptr->ms_key_req.param_g),
                 (void*)req_ptr->param_g.data_ptr,
                 uim_cmd_ptr->ms_key_req.param_g_len);

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_otasp_ms_key */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_BS_CHAL

  DESCRIPTION:
    This function will format the BS CHAL request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_bs_chal (
  const mmgsdi_session_bs_chal_req_type *req_ptr,
  mmgsdi_protocol_enum_type              protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_BS_CHAL_F;
  uim_cmd_ptr->bs_chal.randseed             = req_ptr->randseed;

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_bs_chal */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_SSD_CONFIRM

  DESCRIPTION:
    This function will format the OTASP SSD Confrim request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_ssd_confirm (
  const mmgsdi_session_ssd_confirm_req_type *req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_CONFIRM_SSD_F;
  uim_cmd_ptr->confirm_ssd.authbs           = req_ptr->authbs;

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_ssd_confirm */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_SSD_UPDATE

  DESCRIPTION:
    This function will format the SSD Update request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending SSD Update
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_ssd_update (
  const mmgsdi_session_ssd_update_req_type *req_ptr,
  mmgsdi_protocol_enum_type                 protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type         mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;

  /*---------------------------------------------------------------------------
   FUNCTION PARAMETER CHECKS
   1. req_ptr        - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                                           req_ptr->request_header.session_id,
                                           &mmgsdi_slot,
                                           &channel_id,
                                           &app_info_ptr,
                                           NULL,
                                           NULL,
                                           NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(mmgsdi_slot, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  /*---------------------------------------------------------------------------
  Populate UIM Block Cmd data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command                  = UIM_SSD_UPDATE_F;
  uim_cmd_ptr->ssd_update.randssd[0]        = req_ptr->randssd[0];
  uim_cmd_ptr->ssd_update.randssd[1]        = req_ptr->randssd[1];
  uim_cmd_ptr->ssd_update.process_control   = req_ptr->process_control;
  uim_cmd_ptr->ssd_update.esn               = req_ptr->esn;

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                                       index,
                                                       &req_ptr->request_header,
                                                       uim_cmd_ptr,
                                                       NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data                = int32touint16(index);

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_ssd_update */


/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_STORE_ESN

  DESCRIPTION:
    This function will format the store ESN request in the form required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending the store
                             ESN command to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_store_esn (
  mmgsdi_session_store_esn_req_type    *req_ptr,
  boolean                               sync_process,
  mmgsdi_session_store_esn_cnf_type   **cnf_pptr,
  mmgsdi_protocol_enum_type             protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  uim_cmd_type                   * uim_cmd_ptr      = NULL;
  uim_slot_type                    uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;
  int32                            index            = 0;
  mmgsdi_int_app_info_type       * app_info_ptr     = NULL;
  mmgsdi_channel_id_type           channel_id       = 0;
  uim_channel_type                 uim_channel      = UIM_CHANNEL0;
  mmgsdi_task_enum_type            mmgsdi_task      = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_1("MMGSDI UIM COMMON STORE ESN, sync process is 0x%x",sync_process);

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
    req_ptr->request_header.session_id,
    &req_ptr->request_header.slot_id,
    &channel_id,
    &app_info_ptr,
    NULL,
    NULL,
    NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if((protocol == MMGSDI_UICC && app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM) ||
     (protocol == MMGSDI_ICC && app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x",app_info_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Convert to UIM Channel ID */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
  Populate UIM Cmd pointer data
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.slot                     = uim_slot;
  uim_cmd_ptr->hdr.options                  = UIM_OPTION_NONE;
  uim_cmd_ptr->hdr.channel                  = uim_channel;
  uim_cmd_ptr->hdr.protocol                 = (protocol == MMGSDI_UICC) ?
                                               UIM_UICC : UIM_ICC;
  uim_cmd_ptr->store_meid.len_and_usage =
      req_ptr->esn_meid_info.esn_length | req_ptr->esn_meid_info.esn_usage;

  if (req_ptr->esn_meid_info.meid_available)
  {
    uim_cmd_ptr->hdr.command = UIM_STORE_ESN_MEID_ME_F;
    qw_equ(uim_cmd_ptr->store_meid.meid, req_ptr->esn_meid_info.meid);
    uim_cmd_ptr->store_meid.meid_available = TRUE;
  }
  else
  {
    uim_cmd_ptr->hdr.command = UIM_STORE_ESN_ME_F;
    uim_cmd_ptr->store_esn.esn = req_ptr->esn_meid_info.esn;
  }

  /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
    index, &req_ptr->request_header, uim_cmd_ptr, NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  if (sync_process)
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    /* Synchronous internal processing, retrieve the confirmation data */
    if (sync_process)
    {
      mmgsdi_task = mmgsdi_util_get_current_task();
      if (mmgsdi_task != MMGSDI_TASK_MAX &&
          mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
      {
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data_index = index;
        mmgsdi_status = mmgsdi_common_rsp_store_esn(
               mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], cnf_pptr, TRUE, protocol);
        if (mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
        }
      }
      else
      {
        UIM_MSG_ERR_1("Sync process for request at index 0x%x has null mmgsdi_internal_synch_uim_rpt_rsp",
                      index);
        mmgsdi_status = MMGSDI_ERROR;
      }
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  /* Deallocate the uim cmd from heap will be done when the release client info
     has been called*/
  return mmgsdi_status;
#else
  (void)req_ptr;
  (void)sync_process;
  (void)cnf_pptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_uim_common_store_esn */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_UNBLOCK_PIN
   DESCRIPTION:
     This function packages the UNBLOCK PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_unblock_pin (
  const mmgsdi_pin_operation_req_type        * req_ptr,
  mmgsdi_num_retry_enum_type                   retry_type,
  mmgsdi_protocol_enum_type                    protocol
)
{
  uim_cmd_type *                       uim_cmd_ptr      = NULL;
  uim_slot_type                        uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  int32                                index            = 0;
  mmgsdi_channel_id_type               channel_id       = 0;
  mmgsdi_client_req_extra_info_type   *extra_param_ptr  = NULL;
  uim_channel_type                     uim_channel_id   = UIM_CHANNEL0;
  mmgsdi_static_data_type             *aid_ptr          = NULL;
  mmgsdi_int_app_info_type            *app_info_ptr     = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol == MMGSDI_ICC &&
      (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
       req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN ||
       req_ptr->new_pin.data_len < MMGSDI_PIN_MIN_LEN ||
       req_ptr->new_pin.data_len > MMGSDI_PIN_MAX_LEN))
  {
     UIM_MSG_ERR_0("UNBLOCK_PIN_REQ - CHV/PIN invalid");
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                   req_ptr->request_header.session_id,
                   NULL,
                   &channel_id,
                   &app_info_ptr,
                   NULL,
                   NULL,
                   NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    aid_ptr = &app_info_ptr->app_data.aid;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  extra_param_ptr->pin_data.retry_type = retry_type;

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_UNBLOCK_CHV_F
    2) protocol = UIM_WCDMA/UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_UNBLOCK_CHV_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  uim_cmd_ptr->hdr.protocol        = (protocol == MMGSDI_UICC) ?
                                      UIM_UICC : UIM_ICC;

  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.session_id,
                                          req_ptr->pin_id,
                                          &uim_cmd_ptr->unblk_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  if (req_ptr->pin_puk.data_len >= MMGSDI_PIN_MIN_LEN &&
      req_ptr->pin_puk.data_len <= MMGSDI_PIN_MAX_LEN)
  {
    mmgsdi_memscpy(uim_cmd_ptr->unblk_chv.chv_digits,
                   sizeof(uim_cmd_ptr->unblk_chv.chv_digits),
                   req_ptr->pin_puk.data_ptr,
                   int32touint32(req_ptr->pin_puk.data_len));
  }
  else
  {
    uim_cmd_ptr->unblk_chv.chv_digits[0] = '\0';
  }

  if (req_ptr->new_pin.data_len >= MMGSDI_PIN_MIN_LEN &&
      req_ptr->new_pin.data_len <= MMGSDI_PIN_MAX_LEN)
  {
    mmgsdi_memscpy(uim_cmd_ptr->unblk_chv.new_chv_digits,
                   sizeof(uim_cmd_ptr->unblk_chv.new_chv_digits),
                   req_ptr->new_pin.data_ptr,
                   int32touint32(req_ptr->new_pin.data_len));
  }
  else
  {
    uim_cmd_ptr->unblk_chv.new_chv_digits[0] = '\0';
  }

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* aid*/
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_memscpy(uim_cmd_ptr->unblk_chv.aid.data,
                   sizeof(uim_cmd_ptr->unblk_chv.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->unblk_chv.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_0("Sending down UNBLOCK CHV/PIN command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_common_unblock_pin */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_VERIFY_PIN

   DESCRIPTION:
     This function packages the VERIFY PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_verify_pin (
  const mmgsdi_pin_operation_req_type        * req_ptr,
  mmgsdi_num_retry_enum_type                   retry_type,
  mmgsdi_protocol_enum_type                    protocol
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_CHANNEL0;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol == MMGSDI_ICC &&
      (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
       req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN))
  {
     UIM_MSG_ERR_0("Invalid CHV/PIN Length, Not sending the Command down");
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      req_ptr->request_header.session_id,
                      NULL,
                      &channel_id,
                      &app_info_ptr,
                      NULL,
                      NULL,
                      NULL);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    aid_ptr = &app_info_ptr->app_data.aid;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  extra_param_ptr->pin_data.retry_type = retry_type;

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_VERIFY_CHV_F
    2) protocol = UIM_WCDMA/UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) item
    5) path
    6) offset
    7) data len and data
    8) user data: the req_ptr
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_VERIFY_CHV_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  uim_cmd_ptr->hdr.protocol        = (protocol == MMGSDI_UICC) ?
                                      UIM_UICC : UIM_ICC;

  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.session_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->verify_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  if (req_ptr->pin_puk.data_len >= MMGSDI_PIN_MIN_LEN &&
      req_ptr->pin_puk.data_len <= MMGSDI_PIN_MAX_LEN)
  {
    mmgsdi_memscpy(uim_cmd_ptr->verify_chv.chv_digits,
                   sizeof(uim_cmd_ptr->verify_chv.chv_digits),
                   req_ptr->pin_puk.data_ptr,
                   int32touint32(req_ptr->pin_puk.data_len));
  }
  else
  {
    uim_cmd_ptr->verify_chv.chv_digits[0] = '\0';
  }

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* aid */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_memscpy(uim_cmd_ptr->verify_chv.aid.data,
                   sizeof(uim_cmd_ptr->verify_chv.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->verify_chv.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_0("Sending down VERIFY CHV/PIN command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_common_verify_pin */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_ENABLE_PIN
   DESCRIPTION:
     This function packages the ENABLE PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_enable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr,
  mmgsdi_protocol_enum_type                         protocol
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_CHANNEL0;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      req_ptr->request_header.session_id,
                      NULL,
                      &channel_id,
                      &app_info_ptr,
                      NULL,
                      NULL,
                      NULL);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    aid_ptr = &app_info_ptr->app_data.aid;
  }

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN)
  {
     UIM_MSG_ERR_0("ENABLE_PIN_REQ - CHV/PIN invalid");
     return MMGSDI_ERROR;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_ENABLE_CHV_F
    2) protocol = UIM_WCDMA/UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_ENABLE_CHV_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  uim_cmd_ptr->hdr.protocol        = (protocol == MMGSDI_UICC) ?
                                      UIM_UICC : UIM_ICC;

  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.session_id,
                                          req_ptr->pin_id,
                                          &uim_cmd_ptr->enable_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_memscpy(uim_cmd_ptr->enable_chv.chv_digits,
                 sizeof(uim_cmd_ptr->enable_chv.chv_digits),
                 req_ptr->pin_puk.data_ptr,
                 int32touint32(req_ptr->pin_puk.data_len));

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* aid */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_memscpy(uim_cmd_ptr->enable_chv.aid.data,
                   sizeof(uim_cmd_ptr->enable_chv.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->enable_chv.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_0("Sending down ENABLE CHV/PIN command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_common_enable_pin */


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_DISABLE_PIN
   DESCRIPTION:
     This function packages the DISABLE PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_disable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr,
  mmgsdi_protocol_enum_type                         protocol
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_CHANNEL0;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;
  uim_chv_type                       replace_chv;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      req_ptr->request_header.session_id,
                      NULL,
                      &channel_id,
                      &app_info_ptr,
                      NULL,
                      NULL,
                      NULL);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    aid_ptr = &app_info_ptr->app_data.aid;
  }

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN)
  {
     UIM_MSG_ERR_0("DISABLE_PIN_REQ - CHV/PIN invalid");
     return MMGSDI_ERROR;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_DISABLE_CHV_F
    2) protocol = UIM_WCDMA/UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_DISABLE_CHV_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  uim_cmd_ptr->hdr.protocol        = (protocol == MMGSDI_UICC) ?
                                      UIM_UICC : UIM_ICC;

  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.session_id,
                                          req_ptr->pin_id,
                                         &uim_cmd_ptr->disable_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_memscpy(uim_cmd_ptr->disable_chv.chv_digits,
                 sizeof(uim_cmd_ptr->disable_chv.chv_digits),
                 req_ptr->pin_puk.data_ptr,
                 int32touint32(req_ptr->pin_puk.data_len));

  /*Set the P2 Variable if Required*/
  if(protocol == MMGSDI_UICC && req_ptr->pin_op == MMGSDI_PIN_OP_DISABLE_AND_REPLACE)
  {
    mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.session_id,
                                            MMGSDI_UNIVERSAL_PIN,
                                            &replace_chv);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return mmgsdi_status;
    }
    uim_cmd_ptr->disable_chv.param_p2 = (byte)replace_chv;
  }

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* aid */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_memscpy(uim_cmd_ptr->disable_chv.aid.data,
                   sizeof(uim_cmd_ptr->disable_chv.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->disable_chv.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_0("Sending down DISABLE CHV/PIN command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_common_disable_pin */

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_CHANGE_PIN
   DESCRIPTION:
     This function packages the CHANGE PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_change_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr,
  mmgsdi_protocol_enum_type                         protocol
)
{
  uim_cmd_type *                     uim_cmd_ptr      = NULL;
  uim_slot_type                      uim_slot         = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  mmgsdi_channel_id_type             channel_id       = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  uim_channel_type                   uim_channel_id   = UIM_CHANNEL0;
  mmgsdi_static_data_type           *aid_ptr          = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr     = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr        - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id,
                                               &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                   req_ptr->request_header.session_id,
                   NULL,
                   &channel_id,
                   &app_info_ptr,
                   NULL,
                   NULL,
                   NULL);

    if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_convert_uim_channel(channel_id, &uim_channel_id);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    aid_ptr = &app_info_ptr->app_data.aid;
  }

  if (req_ptr->pin_puk.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->pin_puk.data_len > MMGSDI_PIN_MAX_LEN ||
      req_ptr->new_pin.data_len < MMGSDI_PIN_MIN_LEN ||
      req_ptr->new_pin.data_len > MMGSDI_PIN_MAX_LEN)
  {
     UIM_MSG_ERR_0("CHANGE_PIN_REQ - PIN invalid");
     return MMGSDI_ERROR;
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
          MMGSDI_PIN_OPERATION_REQ, (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Get a UIM buffer for the request
    2) Check for if the buffer is NULL or not
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populate UIM information
    1) command = UIM_CHANGE_CHV_F
    2) protocol = UIM_WCDMA/UIM_GSM  [ GSM and CDMA share the same protocol ]
    3) slot
    4) pin data len and data
    5) user data: the req_ptr
    6) aid
  ---------------------------------------------------------------------------*/
  uim_cmd_ptr->hdr.command         = UIM_CHANGE_CHV_F;
  uim_cmd_ptr->hdr.slot            = uim_slot;
  uim_cmd_ptr->hdr.channel         = uim_channel_id;
  uim_cmd_ptr->hdr.protocol        = (protocol == MMGSDI_UICC) ?
                                      UIM_UICC : UIM_ICC;

  mmgsdi_status = mmgsdi_util_get_key_ref(req_ptr->request_header.session_id,
                                          req_ptr->pin_id,
                                          &uim_cmd_ptr->change_chv.chv);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_memscpy(uim_cmd_ptr->change_chv.chv_digits,
                 sizeof(uim_cmd_ptr->change_chv.chv_digits),
                 req_ptr->pin_puk.data_ptr,
                 int32touint32(req_ptr->pin_puk.data_len));

  mmgsdi_memscpy(uim_cmd_ptr->change_chv.new_chv_digits,
                 sizeof(uim_cmd_ptr->change_chv.new_chv_digits),
                 req_ptr->new_pin.data_ptr,
                 int32touint32(req_ptr->new_pin.data_len));

   /*---------------------------------------------------------------------------
  Store Information into the client req info table
  1) Get the index
  2) Populate client request table
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    2) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                 &req_ptr->request_header,
                                                 uim_cmd_ptr,
                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table
    2) Release extra param pointer pin data (Since client table has not been
                                    populated yet)
    3) Release extra param pointer (Since client table has not been populated
                                    yet)
    4) Release uim cmd pointer  (Since client table has not been populated yet)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* aid */
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_memscpy(uim_cmd_ptr->change_chv.aid.data,
                   sizeof(uim_cmd_ptr->change_chv.aid.data),
                   aid_ptr->data_ptr,
                   int32touint32(aid_ptr->data_len));
    uim_cmd_ptr->change_chv.aid.aid_length = int32touint8(aid_ptr->data_len);
  }

  /* Assign uim_cmd_ptr's user data */
  uim_cmd_ptr->hdr.user_data = int32touint16(index);

  UIM_MSG_HIGH_0("Sending down CHANGE CHV/PIN command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release the client table (Others pointer will be released in the
       function)
    ---------------------------------------------------------------------------*/
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_uim_common_change_pin */

