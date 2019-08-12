/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                   M M G S D I   F I L E   U P D A T E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of all file update related commands

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2012-2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_file_update.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/16/16   ar      F3 reduction
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
02/10/14   ar      Replace session checks with utility functions
12/16/13   tkl     decouple caching with uim item type
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
08/30/13   tl      Convert channel info table from static to dynamic allocation
06/24/13   vdc     Added support for triple SIM
05/16/13   vdc     Replace memcpy with safer version memscpy
10/11/12   spo     Removed unnecessary F3 messages
09/28/12   abg     Updated ERR to MSG_ERROR
05/23/12   vv      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "mmgsdi.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_file.h"
#include "mmgsdi_file_update.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_cnf.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* =============================================================================
  FUNCTION:      MMGSDI_FILE_UPDATE_REQ_PROCESS_REG

  DESCRIPTION:
    This function processes file update reg/dereg requests

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
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_file_update_req_process_reg
(
  mmgsdi_session_register_file_update_req_type * req_ptr
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_session_info_type *session_ptr   = NULL;
  mmgsdi_access_type       *file_ptr      = NULL;
  int32                     client_index  = 0;
  int32                     session_index = 0;

  UIM_MSG_HIGH_0("MMGSDI_FILE_UPDATE_PROCESS_REG");

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /*Get the session pointer*/
  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    req_ptr->request_header.session_id,
                    &client_index,
                    &session_index);

  if ((mmgsdi_status != MMGSDI_SUCCESS) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index >= MMGSDI_MAX_SESSION_INFO))
  {
    UIM_MSG_ERR_0("Bad Client ID");
    return MMGSDI_ERROR;
  }

  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_ptr == NULL)
  {
    UIM_MSG_ERR_1("NULL session_ptr, session_id=0x%x",
                req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* Allow client registration only if it has registered a valid
     event callback */
  if(session_ptr->session_evt_cb_ptr == NULL)
  {
    UIM_MSG_ERR_1("NULL session event callback, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* If the client has already registered, clear the previous list */
  file_ptr = session_ptr->file_data.file_list_ptr;
  if (file_ptr != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(file_ptr);
    file_ptr = NULL;
    session_ptr->file_data.file_list_ptr = NULL;
  }

  if (req_ptr->file_update_list.num_files > 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(file_ptr,
      (sizeof(mmgsdi_access_type)* req_ptr->file_update_list.num_files));
    if (file_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* copy */
    mmgsdi_memscpy(file_ptr,
      sizeof(mmgsdi_access_type)* req_ptr->file_update_list.num_files,
      req_ptr->file_update_list.file_list_ptr,
      sizeof(mmgsdi_access_type) * req_ptr->file_update_list.num_files);
  }

  session_ptr->file_data.file_list_ptr = file_ptr;
  session_ptr->file_data.num_files = req_ptr->file_update_list.num_files;

  return MMGSDI_SUCCESS;
} /* mmgsdi_file_update_req_process_reg */


/* =============================================================================
  FUNCTION:      MMGSDI_PROCESS_FILE_UPDATE_REGISTRATION

  DESCRIPTION:
    This function handles all file update reg/dereg requests

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
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_process_file_update_registration
(
  mmgsdi_session_register_file_update_req_type * req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_SUCCESS;
  int32                              index                = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /*Fill the client table*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* Process the reg/de-reg */
  mmgsdi_status = mmgsdi_file_update_req_process_reg(req_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                     index, MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ,
                     mmgsdi_status);
  }
  else
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_process_file_update_registration */


/* ============================================================================
  FUNCTION:      MMGSDI_PROCESS_FILE_UPDATE_REGISTRATION_RSP

  DESCRIPTION:
    This function will process file update reg/dereg response

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The function was successfull in sending the response
    MMGSDI_ERROR:            Could not get client index/
                             Could not build confirmation/
                             Other error
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_process_file_update_registration_rsp
(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;
  mmgsdi_request_header_type         req_header;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  report_status = (uim_rsp_ptr->is_uim_success)?
                  MMGSDI_SUCCESS :
                  uim_rsp_ptr->mmgsdi_error_status;

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status, &req_header,
                                             NULL, NULL,
                                             FALSE, /* sw not required */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_process_file_update_registration_rsp */


/* ============================================================================
  FUNCTION:      MMGSDI_FILE_UPDATE_IS_FILE_REGISTERED_BY_PATH

  DESCRIPTION:
    The function is a utility to determine if the file updated is registered by
    a client using the file path

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    TRUE   :    If registered by path
    FALSE  :    If not registered or not registered by path

  SIDE EFFECTS:
    None
=============================================================================*/
static boolean mmgsdi_file_update_is_file_registered_by_path(
  const mmgsdi_session_info_type *session_ptr,
  mmgsdi_path_type                file_path
)
{
  uint8   num_files = 0;

  if ((session_ptr == NULL) ||
      (session_ptr->file_data.file_list_ptr == NULL) ||
      (file_path.path_len <= 1) ||
      (file_path.path_len > MMGSDI_MAX_PATH_LEN))
  {
    return FALSE;
  }

  for (num_files = 0; num_files < session_ptr->file_data.num_files;
        num_files++)
  {
    if (session_ptr->file_data.file_list_ptr[num_files].access_method == MMGSDI_BY_PATH_ACCESS)
    {
      if ((file_path.path_len == session_ptr->file_data.file_list_ptr[num_files].file.path_type.path_len) &&
          (!memcmp(&file_path.path_buf,
                   &session_ptr->file_data.file_list_ptr[num_files].file.path_type.path_buf,
                   (int32touint32(session_ptr->file_data.file_list_ptr[num_files].file.path_type.path_len) * sizeof(uint16)))))
      {
        return TRUE;
      }
    }
  }

  return FALSE;
} /* mmgsdi_file_update_is_file_registered_by_path */


/* ============================================================================
  FUNCTION:      MMGSDI_FILE_UPDATE_IS_FILE_REGISTERED

  DESCRIPTION:
    The function is a utility to determine if the file updated is registered by
    a client using the file enum

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    TRUE   :    If registered by enum
    FALSE  :    If not registered or not registered by enum

  SIDE EFFECTS:
    None
=============================================================================*/
static boolean mmgsdi_file_update_is_file_registered_by_enum(
  const mmgsdi_session_info_type *session_ptr,
  mmgsdi_file_enum_type           file
)
{
  uint8 num_files  = 0;

  if ((session_ptr == NULL) ||
      (session_ptr->file_data.file_list_ptr == NULL))
  {
    return FALSE;
  }

  if (file == MMGSDI_MAX_FILE_ENUM)
  {
    return FALSE;
  }

  for (num_files = 0; num_files < session_ptr->file_data.num_files;
        num_files++)
  {
    if (session_ptr->file_data.file_list_ptr[num_files].access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      if (file == session_ptr->file_data.file_list_ptr[num_files].file.file_enum)
      {
        return TRUE;
      }
    }
  }

  return FALSE;
} /* mmgsdi_file_update_is_file_registered_by_enum */


/* ============================================================================
  FUNCTION:      MMGSDI_BUILD_FILE_UPDATE_EVENT

  DESCRIPTION:
    This function will process file update event notification

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
void mmgsdi_build_file_update_event(
  mmgsdi_return_enum_type              status,
  const mmgsdi_write_extra_info_type * write_extra_ptr,
  mmgsdi_session_id_type               session_id,
  mmgsdi_slot_id_enum_type             slot
)
{
  mmgsdi_file_enum_type           file_enum                   = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_return_enum_type         mmgsdi_enum_to_path_status  = MMGSDI_SUCCESS;
  int32                           client_index                = 0;
  int32                           session_index               = 0;
  mmgsdi_len_type                 num_sessions                = 0;
  mmgsdi_session_info_type       *session_ptr                 = NULL;
  mmgsdi_int_app_info_type       *sel_app_ptr                 = NULL;
  boolean                         send_evt                    = FALSE;
  mmgsdi_path_type                path;
  mmgsdi_access_type              file_access;
  mmgsdi_evt_session_notify_type  notify_type;

  if (write_extra_ptr == NULL)
  {
    return;
  }

  if (status != MMGSDI_SUCCESS)
  {
    return;
  }

  status = mmgsdi_util_get_session_app_info(session_id,NULL,NULL,
                                            &sel_app_ptr,NULL,NULL,NULL);
  if((status != MMGSDI_SUCCESS) ||
     (sel_app_ptr == NULL))
  {
    UIM_MSG_ERR_1("Invalid session id 0x%x.", session_id);
    return;
  }

  memset(&path, 0x00, sizeof(mmgsdi_path_type));
  if (write_extra_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    mmgsdi_enum_to_path_status = mmgsdi_file_get_path_from_file(
                                            write_extra_ptr->access.file.file_enum,
                                            &path);
    file_enum = write_extra_ptr->access.file.file_enum;
  }
  else if (write_extra_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS)
  {
    /* Copy the file path */
     mmgsdi_memscpy(&path,
                    sizeof(path),
                    &write_extra_ptr->access.file.path_type,
                    sizeof(mmgsdi_path_type));

    /* Convert to file enum */
    file_enum = mmgsdi_util_convert_path_to_enum(session_id, path);
  }
  else
  {
    /* Unexpected access method type */
    return;
  }

  /* The updated file is known by enum or by path or both. So check if
     any client registered for it */
  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[client_index]->num_session;
    for (session_index = 0; (num_sessions > 0) &&
          (session_index < MMGSDI_MAX_SESSION_INFO); session_index++)
    {
      session_ptr = mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
      if (session_ptr == NULL)
      {
        continue;
      }
      num_sessions--;

      /* Check if there are files registered */
      if (session_ptr->file_data.num_files == 0)
      {
        continue;
      }

      /* Check if this session is for an app on the same slot */
      if((session_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] == NULL) ||
         (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->slot_id != slot))
      {
        continue;
      }

      /* Check if the file updated is outside ADF. If not, don't
         send the event to card sessions */
      if (!mmgsdi_file_is_access_outside_adf(&write_extra_ptr->access) &&
          (mmgsdi_util_is_card_session(session_ptr->session_type)))
      {
        continue;
      }

      /* Check if the file updated is inside ADF. If yes, don't send event
         to sessions opened on a different aid */
      if(!mmgsdi_file_is_access_outside_adf(&write_extra_ptr->access) &&
         (session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL) &&
         ((mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len !=
            sel_app_ptr->app_data.aid.data_len) ||
          (memcmp(&mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_ptr,
                  &sel_app_ptr->app_data.aid.data_ptr,
                  int32touint32(mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len)))))
      {
        continue;
      }

      /* Check if the client who updated the file also registered
         for it. If yes, skip sending the event */
      if(session_ptr->session_id == session_id)
      {
        continue;
      }

      /* Check if client has valid event callback */
      if (session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));
      notify_type.notify_type = MMGSDI_EVT_NOTIFY_SINGLE_SESSION;
      notify_type.session_id  = session_ptr->session_id;

      /* Check if the client registered by enum */
      send_evt = mmgsdi_file_update_is_file_registered_by_enum(session_ptr, file_enum);
      if(send_evt)
      {
        memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
        file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
        file_access.file.file_enum = file_enum;
        mmgsdi_evt_build_and_send_file_update(notify_type, file_access, write_extra_ptr);
      }

      /* Check if the path is valid */
      if ((mmgsdi_enum_to_path_status == MMGSDI_SUCCESS) &&
          (path.path_len > 1))
      {
        /* Check if the client registered by path */
        send_evt = mmgsdi_file_update_is_file_registered_by_path(session_ptr, path);
        if(send_evt)
        {
          memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
          file_access.access_method  = MMGSDI_BY_PATH_ACCESS;
          mmgsdi_memscpy(&file_access.file.path_type,
                         sizeof(file_access.file.path_type),
                         &path,
                         sizeof(mmgsdi_path_type));
          mmgsdi_evt_build_and_send_file_update(notify_type, file_access, write_extra_ptr);
        }
      }
    }
  }
} /* mmgsdi_build_file_update_event */
