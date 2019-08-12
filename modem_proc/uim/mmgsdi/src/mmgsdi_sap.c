/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2006, 2008 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_sap.c#1 $$ $DateTime: 2016/12/13 08:00:05 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   ar      Review of macros used by MMGSDI
05/03/16   tkl     F3 log prints cleanup
04/04/16   tl      Add error condition for SAP connect/disconnect in card error state
09/27/14   ar      Add support for multiple slots in BT-SAP
08/14/14   tl      Remove support for silent SAP PIN verficiation in modem
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
01/11/14   df      Fix off-target compilation error
01/06/14   df      Use v2.0 diag macros
10/16/13   tl      Remove MMGSDI_FEATURE_SAP
10/04/13   vdc     F3 message reduction
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/15/13   tl      Remove SAP init, dereg and pin verify functionality
10/11/12   spo     Removed unnecessary F3 messages
10/01/12   abg     Removed Problem determining protocol type F3 message
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
12/21/11   shr     Legacy GSDI removal updates
10/05/11   nb      Update to differentiate SAP Connect rejection due to active call
07/18/11   nb      Do not generate SAP event when connection failed
05/17/11   kk      Updates for Thread safety
05/04/11   nb      Pass slot information with SAP Event
11/30/10   yt      Including private UIMDRV header
10/14/10   yt      Updated handling of client req table entries in case of error
09/09/10   nb      Send SAP notification using client ID
06/24/10   yt      Klocwork fixes
04/07/10   nb      SAP Update
12/18/09   nb      Moved Dual Slot Changes
04/22/09   sun     Fixed includes
11/05/08   ssr     Resolved compiler error for RUIM enablement
09/22/08   nb      Resolved compiler warning
06/23/08   tml     Added featurization for BT in WM
09/10/07   tml     Used generic get ATR and card reader status
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
07/26/06   sun     Lint Fixes and Clean up
07/10/06   tml     compilation fix
07/07/06   sun     Lint Fixes
06/27/06   sun     Send UIM_RESET_SWITCH_UIM_PASSIVE_F during SAP_RESET
                   Fixed Alignment
02/22/06   tml     lint fix
12/20/05   sun     Populated extra_param for Sap_Disconnect and return proper
                   errors on invalid state
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     On Disconnect, do not do a refresh immediately
10/28/05   sun     Fixed Sap Disconnect
10/20/05   sun     Added support for BT Deregisteration
08/29/05   tml     Fixed compilation
08/26/05   tml     Fixed compilation issue
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Moved functions and data types relating to refresh to the
                   mmgsdi_gen module
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/28/05   sst     Fixed compile errors when SAP feature is not defined
06/06/05   sst     Initial version

===========================================================================*/


#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "mmgsdi_sap.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_uim_uicc.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "uim.h"
#include "uim_p.h"
#include "mmgsdi_sap_rsp.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_evt.h"


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_connect

  DESCRIPTION:
    Connects the UIM to the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_connect (
  const mmgsdi_sap_connect_req_type* req_ptr
)
{
  uim_cmd_type*                       uim_cmd_ptr     = NULL;
  uim_slot_type                       uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  int32                               index           = 0;
  mmgsdi_client_req_extra_info_type * extra_param_ptr = NULL;
  mmgsdi_event_data_type              event_data;
  boolean                             reject_req      = FALSE;
  mmgsdi_slot_data_type             * slot_data_ptr   = NULL;

  /* Parameter checks. */
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if (slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* SAP connection should not be valid, even in LPM mode, except when card is
     powered down. Similar to NFC. */
  if (slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NO_CARD &&
      slot_data_ptr->card_error_condition == MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY)
  {
    return MMGSDI_ERROR;
  }

  memset(&event_data, 0x00, sizeof(mmgsdi_event_data_type));

  event_data.data.sap_connect.sap_mode = MMGSDIBT_SERVER;
  event_data.evt                       = MMGSDI_SAP_CONNECT_EVT;
  event_data.data.sap_connect.slot     = req_ptr->request_header.slot_id;

  UIM_MSG_HIGH_3("Connection condition: 0x%x, voice_call_active: 0x%x, data_call_active = 0x%x",
                 req_ptr->condition,
                 mmgsdi_generic_data_ptr->voice_call_active,
                 mmgsdi_generic_data_ptr->data_call_active);

  /* Check respective call status & decide to proceed or not */
  switch (req_ptr->condition)
  {
    case MMGSDI_SAP_CONN_COND_BLOCK_DATA:
      reject_req = mmgsdi_generic_data_ptr->data_call_active ? TRUE : FALSE;
      break;

      case MMGSDI_SAP_CONN_COND_BLOCK_VOICE:
      reject_req = mmgsdi_generic_data_ptr->voice_call_active ? TRUE : FALSE;
      break;

    case MMGSDI_SAP_CONN_COND_BLOCK_NONE:
      /* Nothing to do */
      break;

    case MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA:
    default:
      reject_req = (mmgsdi_generic_data_ptr->voice_call_active ||
                    mmgsdi_generic_data_ptr->data_call_active) ? TRUE : FALSE;
      break;
  }

  if (reject_req)
  {
    UIM_MSG_ERR_0("Call is in progress, cannot connect");
    return MMGSDI_SIM_BUSY;
  }

  /* Convert to UIM slot. */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter.*/
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                                      MMGSDI_SAP_CONNECT_REQ,
                                      (void*)req_ptr,
                                      &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*----------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   -----------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command  = UIM_RESET_SWITCH_UIM_PASSIVE_F;
  uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot     = uim_slot;

  /*---------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   ----------------------------------------------------------------------------*/

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                 &req_ptr->request_header,
                                                                  uim_cmd_ptr,
                                                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data.*/
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  UIM_MSG_HIGH_0("Sending down SAP connect command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    event_data.data.sap_connect.connect_state = MMGSDI_SAP_CONNECTING;
    mmgsdi_evt_notify_client(&event_data);
  }

  return mmgsdi_status;
} /* mmgsdi_sap_sap_connect */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_disconnect

  DESCRIPTION:
    Disconnects the UIM from the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_disconnect (
  const mmgsdi_sap_disconnect_req_type* req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_ERROR;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  uint8                              slot_index      = 0;
  mmgsdi_slot_data_type            * slot_data_ptr   = NULL;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if (slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* SAP disconnection should not be valid, even in LPM mode, except when card is
     powered down. Similar to NFC. */
  if (slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NO_CARD &&
      slot_data_ptr->card_error_condition == MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* If SAP is not in connected mode for requested SLOT than no need
     to handle disconnect request */
  if(req_ptr->sap_mode == MMGSDIBT_CLIENT || mmgsdi_sap_connected[slot_index] == FALSE)
  {
    /*Nothing to do. Return from here. */
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_SAP_DISCONNECT_REQ,
                    (void*)req_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /*----------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -----------------------------------------------------------------------------------*/

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                  &req_ptr->request_header,
                                                                  NULL,
                                                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index,
                    MMGSDI_SAP_DISCONNECT_REQ,
                    mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;

} /* mmgsdi_sap_sap_disconnect *//*lint !e715 */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_send_apdu

  DESCRIPTION:
    Sends an APDU from the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_send_apdu (
  mmgsdi_sap_send_apdu_req_type* req_ptr,
  mmgsdi_protocol_enum_type      protocol
)
{
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  int32                              index           = 0;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type             *slot_data_ptr   = NULL;
  uint8                              slot_index      = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("BT is not connected");
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (mmgsdibt_pipe_apdu_ptr[slot_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdibt_pipe_apdu_ptr[slot_index],
                                       sizeof(mmgsdi_cnf_type));
    if (mmgsdibt_pipe_apdu_ptr[slot_index] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  if (req_ptr->data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == GET_RESPONSE)
  {
    /* The response APDU is buffered, no need to go to the SIM.*/
    mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_SAP_SEND_APDU_REQ,
                                                                (void*)req_ptr,
                                                                &extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      return mmgsdi_status;
    }

    mmgsdi_status =mmgsdi_util_populate_client_request_table_info(index,
                                                                 &req_ptr->request_header,
                                                                 NULL,
                                                                 extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      mmgsdi_util_free_client_request_table_index(index);
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(index, MMGSDI_SAP_SEND_APDU_REQ, MMGSDI_SUCCESS);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  else if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_send_apdu((mmgsdi_send_apdu_req_type*)req_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_uim_icc_send_apdu((mmgsdi_send_apdu_req_type*)req_ptr);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_send_apdu */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_on

  DESCRIPTION:
    Sends a SIM power on request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_on (
  const mmgsdi_sap_power_on_req_type* req_ptr
)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP power ON req error: BT is Not Connected");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot.*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate and populate extra parameter.*/
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_SAP_POWER_ON_REQ,
                                                             (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*------------------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   -------------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command  = UIM_POWER_UP_UIM_PASSIVE_F;
  uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot     = uim_slot;

  /*-------------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -------------------------------------------------------------------------------------*/

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                                 &req_ptr->request_header,
                                                                 uim_cmd_ptr,
                                                                 extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data.*/
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  UIM_MSG_HIGH_0("Sending down SAP power on command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_power_on */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_off

  DESCRIPTION:
    Sends a SIM power off request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_off (
  const mmgsdi_sap_power_off_req_type* req_ptr
)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP power OFF req error: BT is Not Connected");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot.*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /* Allocate and populate extra parameter.*/
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_SAP_POWER_OFF_REQ,
                                                              (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*-------------------------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   -------------------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command  = UIM_POWER_DOWN_F;
  uim_cmd_ptr->hdr.protocol  = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  uim_cmd_ptr->pdown.pdown_option = UIM_OPTION_NO_NOTIFY_PDOWN;

  /*-------------------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -------------------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                  &req_ptr->request_header,
                                                                  uim_cmd_ptr,
                                                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data.*/
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  UIM_MSG_HIGH_0("Sending down SAP power off command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_power_off */

/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_reset

  DESCRIPTION:
    Sends a SIM RESET request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_reset (
  const mmgsdi_sap_reset_req_type* req_ptr
)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP reset req error: BT is Not Connected");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot.*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*Allocate and populate extra parameter.*/
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param( MMGSDI_SAP_RESET_REQ,
                                                              (void*)req_ptr, &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*-------------------------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   --------------------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command   = UIM_RESET_SWITCH_UIM_PASSIVE_F;
  uim_cmd_ptr->hdr.protocol  = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot      = uim_slot;

  /*--------------------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -------------------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index,
                                                                  &req_ptr->request_header,
                                                                  uim_cmd_ptr,
                                                                  extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data. */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  UIM_MSG_HIGH_0("Sending down SAP RESET command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_reset */
