/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   C R S M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of all Restricted SIM Access
  related functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_crsm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/14   hh      Fix compilation warnings
07/25/14   yt      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_msg.h"
#include "intconv.h"
#include "mmgsdi_crsm.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_file.h"

/*===========================================================================
                       F U N C T I O N S
============================================================================*/

/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_RESTRICTED_SIM_ACCESS_CB

  DESCRIPTION:
    Callback function for restricted SIM access requests. This function
    populates the restricted_sim_access_cnf and calls the client callback.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
static void mmgsdi_crsm_restricted_sim_access_cb
(
  mmgsdi_return_enum_type    mmgsdi_status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type    * cnf_ptr
)
{
  mmgsdi_cnf_type                        * client_cnf_ptr       = NULL;
  uint8                                    command              = 0;
  mmgsdi_slot_id_enum_type                 slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_client_id_type                    client_id            = 0;
  mmgsdi_client_data_type                  client_ref           = 0;
  mmgsdi_len_type                          requested_len        = 0;
  mmgsdi_callback_type                     response_cb          = NULL;
  mmgsdi_restricted_sim_access_data_type * crsm_client_data_ptr = NULL;

  if (cnf_ptr == NULL)
  {
    return;
  }

  crsm_client_data_ptr =
    (mmgsdi_restricted_sim_access_data_type *)cnf_ptr->response_header.client_data;
  if (crsm_client_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL client data for restricted SIM access");
    return;
  }

  command     = crsm_client_data_ptr->command;
  slot_id     = crsm_client_data_ptr->slot_id;
  client_id   = crsm_client_data_ptr->client_id;
  client_ref  = crsm_client_data_ptr->client_ref;
  response_cb = crsm_client_data_ptr->response_cb;

  if(crsm_client_data_ptr->apdu_params.params_valid)
  {
    requested_len = (mmgsdi_len_type)crsm_client_data_ptr->apdu_params.p3;
  }

  MMGSDIUTIL_TMC_MEM_FREE(crsm_client_data_ptr);

  if (command != READ_BINARY && command != UPDATE_BINARY &&
      command != READ_RECORD && command != UPDATE_RECORD &&
      command != GET_RESPONSE && command != STATUS)
  {
    UIM_MSG_ERR_1("Invalid command 0x%x for restricted SIM access", command);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(client_cnf_ptr,sizeof(mmgsdi_cnf_type));
  if (client_cnf_ptr == NULL)
  {
    return;
  }

  /* Populate the response header for cnf to be returned to client */
  client_cnf_ptr->response_header.client_id     = client_id;
  client_cnf_ptr->response_header.client_data   = client_ref;
  client_cnf_ptr->response_header.mmgsdi_status = mmgsdi_status;
  client_cnf_ptr->response_header.slot_id       = slot_id;
  client_cnf_ptr->response_header.response_type = MMGSDI_RESTRICTED_SIM_ACCESS_CNF;
  client_cnf_ptr->response_header.response_len  = uint32toint32(sizeof(mmgsdi_restricted_sim_access_cnf_type));

  /* If status words are not returned by MMGSDI, populate them for the following
     values of mmgsdi_status */
  if (mmgsdi_status == MMGSDI_SUCCESS && !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SIM_SW1_NORM_END;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SIM_SW2_NORM_END;
  }
  else if (mmgsdi_status == MMGSDI_NOT_FOUND && !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SW1_INCORRECT_PARAMETERS;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SW2_FILE_NOT_FOUND;
  }
  else if (mmgsdi_status == MMGSDI_ACCESS_DENIED && !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SW1_COMMAND_NOT_ALLOWED;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SW2_SECURITY_STATUS_NOT_SATISFIED;
  }
  else if (mmgsdi_status == MMGSDI_EF_INCONSISTENT && !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SW1_REFERENCE_MANAGEMENT;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SW2_FILE_INCONSISTENT_WITH_CMD;
  }
  else if (mmgsdi_status == MMGSDI_INCORRECT_LENGTH && !cnf_ptr->response_header.status_word.present)
  {
    client_cnf_ptr->response_header.status_word.present = TRUE;
    client_cnf_ptr->response_header.status_word.sw1     = GSDI_SW1_WRONG_LENGTH;
    client_cnf_ptr->response_header.status_word.sw2     = GSDI_SW2_NO_INFO_GIVEN;
  }
  else
  {
    client_cnf_ptr->response_header.status_word.present = cnf_ptr->response_header.status_word.present;
    client_cnf_ptr->response_header.status_word.sw1     = cnf_ptr->response_header.status_word.sw1;
    client_cnf_ptr->response_header.status_word.sw2     = cnf_ptr->response_header.status_word.sw2;
  }

  switch (cnf)
  {
    case MMGSDI_READ_CNF:
      if (cnf_ptr->read_cnf.read_data.data_len > 0)
      {
        if (cnf_ptr->read_cnf.read_data.data_len < requested_len)
        {
          client_cnf_ptr->response_header.status_word.present = TRUE;
          client_cnf_ptr->response_header.status_word.sw1     = GSDI_SW1_WRONG_LENGTH;
          client_cnf_ptr->response_header.status_word.sw2     = GSDI_SW2_NO_INFO_GIVEN;
        }
        else
        {
          client_cnf_ptr->restricted_sim_access_cnf.response_data.data_len =
              cnf_ptr->read_cnf.read_data.data_len;
          client_cnf_ptr->restricted_sim_access_cnf.response_data.data_ptr =
              cnf_ptr->read_cnf.read_data.data_ptr;
          client_cnf_ptr->response_header.response_len +=
            cnf_ptr->read_cnf.read_data.data_len;
        }
      }
      break;
    case MMGSDI_GET_FILE_ATTR_CNF:
      if (cnf_ptr->get_file_attr_cnf.raw_file_attr_data.data_len > 0)
      {
        if (requested_len > 0 &&
            requested_len < cnf_ptr->get_file_attr_cnf.raw_file_attr_data.data_len)
        {
          client_cnf_ptr->restricted_sim_access_cnf.response_data.data_len =
            requested_len;
        }
        else
        {
          client_cnf_ptr->restricted_sim_access_cnf.response_data.data_len =
              cnf_ptr->get_file_attr_cnf.raw_file_attr_data.data_len;
        }
        client_cnf_ptr->restricted_sim_access_cnf.response_data.data_ptr =
            cnf_ptr->get_file_attr_cnf.raw_file_attr_data.data_ptr;
        client_cnf_ptr->response_header.response_len +=
          cnf_ptr->get_file_attr_cnf.raw_file_attr_data.data_len;
      }
      break;
    case MMGSDI_CARD_STATUS_CNF:
      if (cnf_ptr->status_cnf.status_data.data_len > 0)
      {
        if (requested_len > 0 &&
            requested_len < cnf_ptr->status_cnf.status_data.data_len)
        {
          client_cnf_ptr->restricted_sim_access_cnf.response_data.data_len =
            requested_len;
        }
        else
        {
          client_cnf_ptr->restricted_sim_access_cnf.response_data.data_len =
              cnf_ptr->status_cnf.status_data.data_len;
        }
        client_cnf_ptr->restricted_sim_access_cnf.response_data.data_ptr =
            cnf_ptr->status_cnf.status_data.data_ptr;
        client_cnf_ptr->response_header.response_len +=
          cnf_ptr->status_cnf.status_data.data_len;
      }
      break;
    case MMGSDI_WRITE_CNF:
      /* No response data to copy for WRITE commands */
    default:
      break;
  }

  client_cnf_ptr->response_header.payload_len =
    client_cnf_ptr->response_header.response_len - uint32toint32(sizeof(mmgsdi_response_header_type));

  if (response_cb != NULL)
  {
    response_cb(mmgsdi_status,
                MMGSDI_RESTRICTED_SIM_ACCESS_CNF,
                client_cnf_ptr);
  }

  MMGSDIUTIL_TMC_MEM_FREE(client_cnf_ptr);
} /* mmgsdi_crsm_restricted_sim_access_cb */


/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_COPY_RESTRICTED_SIM_ACCESS_CLIENT_DATA

  DESCRIPTION:
    Allocate heap memory and store the client data passed via
    mmgsdi_restricted_sim_access() API.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_copy_restricted_sim_access_client_data
(
  mmgsdi_client_id_type                     client_id,
  mmgsdi_slot_id_enum_type                  slot_id,
  uint8                                     command,
  mmgsdi_apdu_params_type                   apdu_params,
  mmgsdi_callback_type                      response_cb_ptr,
  mmgsdi_client_data_type                   client_ref,
  mmgsdi_restricted_sim_access_data_type ** crsm_client_data_pptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(crsm_client_data_pptr);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*crsm_client_data_pptr,
                                     sizeof(mmgsdi_restricted_sim_access_data_type));
  if(*crsm_client_data_pptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  (*crsm_client_data_pptr)->client_id   = client_id;
  (*crsm_client_data_pptr)->slot_id     = slot_id;
  (*crsm_client_data_pptr)->command     = command;
  (*crsm_client_data_pptr)->response_cb = response_cb_ptr;
  (*crsm_client_data_pptr)->client_ref  = client_ref;

  (*crsm_client_data_pptr)->apdu_params.params_valid = apdu_params.params_valid;
  (*crsm_client_data_pptr)->apdu_params.p1           = apdu_params.p1;
  (*crsm_client_data_pptr)->apdu_params.p2           = apdu_params.p2;
  (*crsm_client_data_pptr)->apdu_params.p3           = apdu_params.p3;

  return MMGSDI_SUCCESS;
} /* mmgsdi_crsm_copy_restricted_sim_access_client_data */


/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_FIND_SESSION_FOR_ACCESS

  DESCRIPTION:
    This function finds the MMGSDI session that can be used for sending the
    command based on slot id and file path.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_find_session_for_access
(
  mmgsdi_slot_id_enum_type                 card_slot,
  mmgsdi_path_type                         file_id_and_path,
  uint8                                    command,
  mmgsdi_session_id_type                 * session_id_ptr,
  mmgsdi_app_enum_type                   * app_type_ptr
)
{
  boolean ef_under_mf = FALSE;
  uint8   slot_index  = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(session_id_ptr, app_type_ptr);

  if ((mmgsdi_util_get_slot_index(card_slot, &slot_index) != MMGSDI_SUCCESS) ||
      slot_index >= MMGSDI_MAX_NUM_SLOTS)
  {
    UIM_MSG_ERR_1("Invalid slot id 0x%x", card_slot);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* If only file id is present in the path buffer, check if it belongs to
     an EF directly under MF. In this case we need to use the card session. */
  if (file_id_and_path.path_len == 1 &&
      (file_id_and_path.path_buf[0] & 0xFF00) == 0x2F00 &&
      command != STATUS)
  {
    ef_under_mf = TRUE;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  if (ef_under_mf ||
      (file_id_and_path.path_len > 1 &&
       file_id_and_path.path_buf[1] != MMGSDI_GSM_LEVEL &&
       file_id_and_path.path_buf[1] != MMGSDI_ADF_LEVEL &&
       command != STATUS))
  {
    /* Use card slot session for accesses outside GSM DF or USIM ADF. */
    *session_id_ptr = mmgsdi_generic_data_ptr->slot_session_id[slot_index];
  }
  else
  {
    /* Use GW provisioning session for access within GSM DF or USIM ADF.
       STATUS command is meant to indicate the status of a currently-selected
       application/DF and requires a provisioning session. */
    (void)mmgsdi_util_find_gw_prov_session_for_slot(card_slot,
                                                    session_id_ptr,
                                                    app_type_ptr);
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return MMGSDI_SUCCESS;
} /* mmgsdi_crsm_find_session_for_access */


/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_CALCULATE_FILE_ACCESS

  DESCRIPTION:
    This function calculates the file access parameter to be sent in the
    MMGSDI command.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_calculate_file_access
(
  mmgsdi_app_enum_type                     app_type,
  mmgsdi_path_type                         file_id_and_path,
  mmgsdi_access_type                     * file_access_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_file_enum_type   file_enum     = MMGSDI_NO_FILE_ENUM;
  mmgsdi_path_type        path;

  MMGSDIUTIL_RETURN_IF_NULL(file_access_ptr);

  memset(&path, 0x00, sizeof(path));

  /* If only file id is present (path_len = 1), add path from MF depending on
     the app type */
  if (file_id_and_path.path_len == 1)
  {
    if (app_type == MMGSDI_APP_USIM &&
        (file_id_and_path.path_buf[0] & 0xFF00) == 0x6F00)
    {
      path.path_len = 3;
      path.path_buf[0] = (uint16)MMGSDI_MF_LEVEL;
      path.path_buf[1] = (uint16)MMGSDI_ADF_LEVEL;
      path.path_buf[2] = file_id_and_path.path_buf[0];
    }
    else if (app_type == MMGSDI_APP_SIM &&
             (file_id_and_path.path_buf[0] & 0xFF00) == 0x6F00)
    {
      path.path_len = 3;
      path.path_buf[0] = (uint16)MMGSDI_MF_LEVEL;
      path.path_buf[1] = (uint16)MMGSDI_GSM_LEVEL;
      path.path_buf[2] = file_id_and_path.path_buf[0];
    }
    else if ((file_id_and_path.path_buf[0] & 0xFF00) == 0x2F00)
    {
      /* For other app types assume the file to be under MF */
      path.path_len = 2;
      path.path_buf[0] = (uint16)MMGSDI_MF_LEVEL;
      path.path_buf[1] = file_id_and_path.path_buf[0];
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }
  else
  {
    mmgsdi_memscpy((void *)&path, sizeof(path), (void *)&file_id_and_path, sizeof(file_id_and_path));
  }

  mmgsdi_status = mmgsdi_file_get_file_from_path(app_type,
                                                 path,
                                                 &file_enum);

  if (mmgsdi_status == MMGSDI_SUCCESS && file_enum != MMGSDI_NO_FILE_ENUM)
  {
    file_access_ptr->access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access_ptr->file.file_enum = file_enum;
  }
  else if (mmgsdi_status == MMGSDI_NOT_FOUND)
  {
    /* Use BY_PATH access if file enum is not found */
    file_access_ptr->access_method = MMGSDI_BY_PATH_ACCESS;
    mmgsdi_memscpy((void *)&file_access_ptr->file.path_type, sizeof(mmgsdi_path_type),
                     (void *)&path, sizeof(mmgsdi_path_type));
  }
  else
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_crsm_calculate_file_access */


/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_QUEUE_RESTRICTED_SIM_ACCESS_CMD

  DESCRIPTION:
    This function translates the AT+CRSM request to an MMGSDI request and
    calls the appropriate API. An internal response callback function is
    specified from which the client callback is called.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_queue_restricted_sim_access_cmd
(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_access_type                       file_access,
  uint8                                    command,
  mmgsdi_apdu_params_type                  apdu_params,
  mmgsdi_data_type                         data,
  mmgsdi_restricted_sim_access_data_type * crsm_client_data_ptr
)
{
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_file_structure_enum_type  record_type   = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
  mmgsdi_rec_num_type              record_num    = 0;
  mmgsdi_len_type                  req_len       = 0;
  mmgsdi_offset_type               offset        = 0;
  mmgsdi_status_me_app_enum_type   me_app_status = MMGSDI_STATUS_APP_NO_INDICATION;
  mmgsdi_status_ret_data_enum_type ret_data      = MMGSDI_STATUS_DATA_SELECT;

  /* P1, P2, P3 parameters are mandatory for READ/UPDATE commands */
  if (apdu_params.params_valid == FALSE &&
      command != STATUS &&
      command != GET_RESPONSE)
  {
    UIM_MSG_ERR_1("Parameters P1, P2, P3 not valid for command 0x%x", command);
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch (command)
  {
    case STATUS:
      if (apdu_params.params_valid)
      {
        /* Indication of application status */
        switch (apdu_params.p1)
        {
          case 0x00:
            me_app_status = MMGSDI_STATUS_APP_NO_INDICATION;
            break;
          case 0x01:
            me_app_status = MMGSDI_STATUS_APP_INITIALIZED;
            break;
          case 0x02:
            me_app_status = MMGSDI_STATUS_APP_TERMINATED;
            break;
          default:
            return MMGSDI_INCORRECT_PARAMS;
        }

        /* Data returned in response */
        switch (apdu_params.p2)
        {
          case 0x00:
            ret_data = MMGSDI_STATUS_DATA_SELECT;
            break;
          case 0x01:
            ret_data = MMGSDI_STATUS_TLV_DATA;
            break;
          case 0x0C:
            ret_data = MMGSDI_STATUS_NO_DATA;
            break;
          default:
            return MMGSDI_INCORRECT_PARAMS;
        }
      }
      mmgsdi_status = mmgsdi_session_send_card_status(session_id,
                                                      me_app_status,
                                                      ret_data,
                                                      mmgsdi_crsm_restricted_sim_access_cb,
                                                      (mmgsdi_client_data_type)crsm_client_data_ptr);
      break;

    case READ_RECORD:
      record_num = (mmgsdi_rec_num_type)apdu_params.p1;
      if (record_num == 0)
      {
        return MMGSDI_INCORRECT_PARAMS;
      }
      req_len = (mmgsdi_len_type)apdu_params.p3;
      mmgsdi_status = mmgsdi_session_read_record(session_id,
                                                 file_access,
                                                 record_num,
                                                 req_len,
                                                 mmgsdi_crsm_restricted_sim_access_cb,
                                                 (mmgsdi_client_data_type)crsm_client_data_ptr);
      break;

    case READ_BINARY:
      offset  = ((mmgsdi_offset_type)apdu_params.p1 << 8) +
                (mmgsdi_offset_type)apdu_params.p2;
      req_len = (mmgsdi_len_type)apdu_params.p3;
      mmgsdi_status = mmgsdi_session_read_transparent(session_id,
                                                      file_access,
                                                      offset,
                                                      req_len,
                                                      mmgsdi_crsm_restricted_sim_access_cb,
                                                      (mmgsdi_client_data_type)crsm_client_data_ptr);
      break;

    case UPDATE_BINARY:
      if ((data.data_len <= 0) ||
          (data.data_ptr == NULL) ||
          (data.data_len != (mmgsdi_len_type)apdu_params.p3))
      {
        return MMGSDI_INCORRECT_PARAMS;
      }
      offset  = ((mmgsdi_offset_type)apdu_params.p1 << 8) +
                (mmgsdi_offset_type)apdu_params.p2;
      mmgsdi_status = mmgsdi_session_write_transparent(session_id,
                                                       file_access,
                                                       offset,
                                                       data,
                                                       mmgsdi_crsm_restricted_sim_access_cb,
                                                       (mmgsdi_client_data_type)crsm_client_data_ptr);
      break;

    case UPDATE_RECORD:
      record_num = (mmgsdi_rec_num_type)apdu_params.p1;
      if ((data.data_len <= 0) || (data.data_ptr == NULL) ||
          (record_num == 0) || (data.data_len != (mmgsdi_len_type)apdu_params.p3))
      {
        return MMGSDI_INCORRECT_PARAMS;
      }

      /* MMGSDI currently supports modes PREVIOUS (0x03) for Cyclic files and
         ABSOLUTE (0x04) for Linear Fixed files. */
      if (apdu_params.p2 == 0x03)
      {
        record_type = MMGSDI_CYCLIC_FILE;
      }
      else
      {
        record_type = MMGSDI_LINEAR_FIXED_FILE;
      }

      mmgsdi_status = mmgsdi_session_write_record(session_id,
                                                  file_access,
                                                  record_type,
                                                  record_num,
                                                  data,
                                                  mmgsdi_crsm_restricted_sim_access_cb,
                                                  (mmgsdi_client_data_type)crsm_client_data_ptr);
      break;

    case GET_RESPONSE:
      mmgsdi_status = mmgsdi_session_get_file_attr(session_id,
                                                   file_access,
                                                   mmgsdi_crsm_restricted_sim_access_cb,
                                                   (mmgsdi_client_data_type)crsm_client_data_ptr);
      break;

    default:
      /* As per 3GPP TS 27.007, other command values are not valid for AT+CRSM.
         RETRIEVE DATA and SET DATA are not currently supported in MMGSDI. */
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_crsm_queue_restricted_sim_access_cmd */

