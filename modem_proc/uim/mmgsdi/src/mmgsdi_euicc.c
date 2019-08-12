/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   E  U I C C   R E L A T E D  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used for eUICCs.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_euicc.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/16   av      Initial revision
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "err.h"
#include "mmgsdiutil.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_session.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* ============================================================================
FUNCTION MMGSDI_EUICC_IS_ISDR_FOUND

DESCRIPTION
  This function attemmpts to select ISDR (which is present on an eUICC, but not
  on a non-eUICC).

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_euicc_is_isdr_found(
  mmgsdi_slot_id_enum_type           slot_id,
  boolean                            *is_isdr_found_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type      *select_ptr          = NULL;
  mmgsdi_get_file_attr_cnf_type      *select_cnf_ptr      = NULL;
  mmgsdi_session_type_enum_type      session_type         = MMGSDI_NON_PROV_SESSION_SLOT_1;
  mmgsdi_session_info_type           *session_info_ptr    = NULL;
  mmgsdi_request_header_type         req_header           = {0};
  mmgsdi_session_deactivate_cnf_type *cnf_ptr             = NULL;
  mmgsdi_channel_id_type             channel_id           = 0xff;
  int32                              channel_index        = MMGSDI_MAX_CHANNEL_INFO;
  uint8                              isdr_aid[]           = {0xA0, 0x00, 0x00, 0x05,
                                                             0x59, 0x10, 0x10, 0xFF,
                                                             0xFF, 0xFF, 0xFF, 0x89,
                                                             0x00, 0x00, 0x01, 0x00};

  if(mmgsdi_generic_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(is_isdr_found_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_non_prov_session_type_from_slot(
                    slot_id,
                    &session_type);
  if(MMGSDI_SUCCESS != mmgsdi_status)
  {
    return mmgsdi_status;
  }

  /* Generate Session ID and add to Client ID reg. table */
  session_info_ptr = mmgsdi_util_generate_and_add_session_to_client(
                       mmgsdi_generic_data_ptr->client_id,
                       session_type);

  if(session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  do
  {
    /* Open a channel for sending the SELECT ISDR on */
    req_header.client_id         = mmgsdi_generic_data_ptr->client_id;
    req_header.session_id        = session_info_ptr->session_id;
    req_header.channel_id        = 0;
    req_header.request_type      = MMGSDI_MANAGE_CHANNEL_REQ;
    req_header.slot_id           = slot_id;
    req_header.orig_request_type = MMGSDI_MANAGE_CHANNEL_REQ;

    mmgsdi_status = mmgsdi_uim_uicc_manage_channel_op(&req_header,
                                                      UIM_CHANNEL0,
                                                      UIM_MC_OPEN,
                                                      NULL,
                                                      TRUE,
                                                      &cnf_ptr,
                                                      &channel_id);
    if(mmgsdi_status != MMGSDI_SUCCESS ||
       cnf_ptr == NULL ||
       (cnf_ptr != NULL &&
        cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
    {
      UIM_MSG_ERR_1("Channel id open failed for channel:0x%x:", channel_id);
      mmgsdi_status = MMGSDI_ERROR;
    }
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Now that we have the channel_id, lets get or generate the corresponding
       entry in channel info table */
    mmgsdi_status = mmgsdi_util_create_channel_info_table_entry(
                      req_header.orig_request_type,
                      slot_id,
                      FALSE,
                      channel_id,
                      &channel_index);
    if(MMGSDI_SUCCESS != mmgsdi_status)
    {
      break;
    }

    /* Map the new channel table entry with the session */
    session_info_ptr->channel_info_index = (uint8) channel_index;

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Build the SELECT command for the ISDR AID */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
        sizeof(mmgsdi_get_file_attr_req_type));
      if(select_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        break;
      }

      select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
      select_ptr->request_header.session_id        = session_info_ptr->session_id;
      select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
      select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
      select_ptr->request_header.slot_id           = slot_id;
      select_ptr->request_header.client_data       = 0;
      select_ptr->request_header.response_cb       = NULL;
      select_ptr->activate_aid                     = TRUE;
      select_ptr->select_rsp_type                  = MMGSDI_SELECT_RSP_NO_DATA;
      select_ptr->select_rsp_requested             = FALSE;
      select_ptr->skip_uicc_arr                    = TRUE;
      select_ptr->access.access_method             = MMGSDI_BY_APP_ID_ACCESS;
      select_ptr->access.file.app_id.data_len      = sizeof(isdr_aid);
      mmgsdi_memscpy((void*)select_ptr->access.file.app_id.data_ptr,
                     sizeof(select_ptr->access.file.app_id.data_ptr),
                     (void*)isdr_aid,
                     sizeof(isdr_aid));

      mmgsdi_status = mmgsdi_uim_uicc_select (select_ptr,
                                              TRUE,
                                              &select_cnf_ptr,
                                              TRUE);
      if(select_cnf_ptr == NULL ||
         mmgsdi_status != MMGSDI_SUCCESS ||
         (select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS &&
          select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_NOT_FOUND))
      {
        UIM_MSG_ERR_0("Cnf Ptr Null or SELECT ISDR failed");
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if(select_cnf_ptr->response_header.mmgsdi_status == MMGSDI_NOT_FOUND)
      {
        *is_isdr_found_ptr = FALSE;
      }
      else
      {
        *is_isdr_found_ptr = TRUE;
      }

      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(select_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    }

    /* Now close the channel and clean up session_info_ptr */
    req_header.client_id         = mmgsdi_generic_data_ptr->client_id;
    req_header.session_id        = session_info_ptr->session_id;
    req_header.channel_id        = 0;
    req_header.request_type      = MMGSDI_MANAGE_CHANNEL_REQ;
    req_header.slot_id           = slot_id;
    req_header.orig_request_type = MMGSDI_MANAGE_CHANNEL_REQ;

    (void) mmgsdi_uim_uicc_manage_channel_op(&req_header,
                                             channel_id,
                                             UIM_MC_CLOSE,
                                             NULL,
                                             TRUE,
                                             &cnf_ptr,
                                             NULL);

    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
  }
  while(0);

  (void) mmgsdi_session_release_info(session_info_ptr, MMGSDI_SESSION_CLOSE_REQ);

  return mmgsdi_status;
} /* mmgsdi_euicc_is_isdr_found */

