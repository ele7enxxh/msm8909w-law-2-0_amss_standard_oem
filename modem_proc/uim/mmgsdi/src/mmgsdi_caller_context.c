/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I  P R O C E S S  I N  C L I E N T  C O N T E X T

                 D E F I N I T I O N S  A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains processing of command request in caller's context.
  It optimizes MMGSDI APIs to build confirmation if required info is available
  in MMGSDI cache first in the caller's context instead of queuing it to MMGSDI
  task context and then checking for it in the cache. The reason in that, it is
  possible that MMGSDI is stuck is a state where card is busy for a long time.
  Any new request that is queued to MMGSDI task context, will get onto the MMGSDI
  queue that is blocked. So, any command request will also have to wait in this
  queue until the card is functional again. All this can be avoided to a certain
  extent by doing some minimal processing in the caller's context. If the API
  is called from MMGSDI context (e.g., from QMI), we skip the optimizations
  because of complications related to critical sections.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_caller_context.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/08/16   ar      Added check for service table in caller context for access APIs. 
05/03/16   tkl     F3 log prints cleanup
04/05/16   vdc     Send act resp and session changed evt at same time
08/10/15   tl      Fix cache read for card sessions in caller's context
05/14/15   bcho    Populate rec num back into the cnf data after read handling
02/23/15   vdc     Allow reading of record based EFs from cache in caller context
01/09/15   ar      Cache file if req_len is more than file size
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/20/14   stv     Populate status word in cnf data even if read is in caller context
10/14/14   av      When processing in client context,set status in cnf header
10/14/14   am      Compile warnings elimination
08/16/14   lj      Optimize app capabilities with cached data in client context
08/11/14   bcho    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdi_caller_context.h"
#include "mmgsdiutil.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc.h"
#include "mmgsdicache.h"
#include "mmgsdilib_p.h"

/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_BUILD_AND_PROCESS_SRV_AVAILABLE_CNF

   DESCRIPTION:
     This function populates the confirmation information and processes
     the response/confirmation directly without queuing it to mmgsdi task

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

       MMGSDI_SUCCESS:          The response and cnf are built successfully
                                and client has been notified with the cnf
       MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_caller_context_build_and_process_srv_available_cnf(
  mmgsdi_client_id_type               client_id,
  mmgsdi_session_id_type              session_id,
  mmgsdi_slot_id_enum_type            slot_id,
  mmgsdi_return_enum_type             cmd_status,
  boolean                             srv_available,
  mmgsdi_callback_type                response_cb,
  mmgsdi_client_data_type             client_data
)
{
  mmgsdi_cnf_type                    *cnf_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(response_cb);

  /* Obtain memory for confirmation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cnf_ptr,
                                     sizeof(mmgsdi_cnf_type));
  if(cnf_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate the mmgsdi srv available cnf data */
  cnf_ptr->srv_available_cnf.srv_available = srv_available;

  /* Construct the mmgsdi cnf response header in order to notify the client */
  cnf_ptr->response_header.response_len        = uint32toint32(sizeof(mmgsdi_srv_available_cnf_type));
  cnf_ptr->response_header.payload_len         = cnf_ptr->response_header.response_len -
      uint32toint32(sizeof(mmgsdi_response_header_type));
  cnf_ptr->response_header.response_type       = MMGSDI_SRV_AVAILABLE_CNF;
  cnf_ptr->response_header.slot_id             = slot_id;
  cnf_ptr->response_header.client_data         = client_data;
  cnf_ptr->response_header.session_id          = session_id;
  cnf_ptr->response_header.client_id           = client_id;
  cnf_ptr->response_header.status_word.present = FALSE;
  cnf_ptr->response_header.mmgsdi_status       = cmd_status;

  /* Lets call the client callback with cnf information */
  (response_cb)(cmd_status,
                MMGSDI_SRV_AVAILABLE_CNF,
                cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

  return MMGSDI_SUCCESS;
}/* mmgsdi_caller_context_build_and_process_srv_available_cnf */


/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_CHECK_SERVICE_AND_NOTIFY_CLIENT

   DESCRIPTION:
     This function tries to retrieve service status from cached service table.
     If the function is invoked from MMGSDI tasks context we simply return
     without any processing.
     This function uses critical sections.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

      MMGSDI_SUCCESS:          The retrieval of service status from cached
                               service table succeeded and client has
                               notified with the cnf
      MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_caller_context_check_service_and_notify_client(
  const mmgsdi_task_cmd_type             *task_cmd_ptr
)
{
  mmgsdi_client_id_type              client_id        = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type         * app_info_ptr     = NULL;
  mmgsdi_slot_id_enum_type           slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_service_enum_type           service          = MMGSDI_NONE;
  mmgsdi_session_id_type             session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_client_data_type            client_data      = 0;
  mmgsdi_callback_type               response_cb      = NULL;
  mmgsdi_protocol_enum_type          protocol         = MMGSDI_NO_PROTOCOL;
  boolean                            srv_available    = FALSE;

  if((task_cmd_ptr == NULL) ||
     (task_cmd_ptr->cmd.cmd_enum != MMGSDI_SRV_AVAILABLE_REQ))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* In order to achieve srv available requests optimization,
     try getting the service status from cached service table first
     in caller's context. If that fails, continue with queuing
     service available request to MMGSDI. */
  if(mmgsdi_util_get_current_task() != MMGSDI_TASK_MAX)
  {
    return MMGSDI_ERROR;
  }

  session_id  = task_cmd_ptr->cmd.cmd.srv_available_req.request_header.session_id;
  response_cb = task_cmd_ptr->cmd.cmd.srv_available_req.request_header.response_cb;
  client_data = task_cmd_ptr->cmd.cmd.srv_available_req.request_header.client_data;
  service     = task_cmd_ptr->cmd.cmd.srv_available_req.service;

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      session_id,
                      &slot_id,
                      NULL,
                      &app_info_ptr,
                      NULL,
                      NULL,
                      NULL);

    if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Get the client id */
    mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    protocol = mmgsdi_util_determine_protocol(slot_id);
    if(protocol == MMGSDI_NO_PROTOCOL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Check service status */
    if(protocol == MMGSDI_UICC)
    {
      mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                    session_id,
                                                    MMGSDI_CHK_SRV_FROM_CACHE_ONLY,
                                                    service,
                                                    &srv_available);
    }
    else
    {
      mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                   MMGSDI_CHK_SRV_FROM_CACHE_ONLY,
                                                   service,
                                                   &srv_available);
    }
  }while (0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* service status is successfully retrieved from service table. */
    mmgsdi_status = mmgsdi_caller_context_build_and_process_srv_available_cnf(
                      client_id,
                      session_id,
                      slot_id,
                      mmgsdi_status,
                      srv_available,
                      response_cb,
                      client_data);
  }

  return mmgsdi_status;
}/* mmgsdi_caller_context_check_service_and_notify_client */


/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_BUILD_AND_PROCESS_READ_CNF

   DESCRIPTION:
     This function populates the confirmation information and processes
     the response/confirmation directly without queuing it to mmgsdi task

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

       MMGSDI_SUCCESS:          The response and cnf are built successfully
                                and client has been notified with the cnf
       MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_caller_context_build_and_process_read_cnf(
  mmgsdi_client_id_type               client_id,
  mmgsdi_session_id_type              session_id,
  mmgsdi_slot_id_enum_type            slot_id,
  mmgsdi_return_enum_type             cmd_status,
  mmgsdi_offset_type                  offset,
  mmgsdi_rec_num_type                 rec_num,
  mmgsdi_access_type                  file_access,
  mmgsdi_data_type                    cnf_data,
  mmgsdi_callback_type                response_cb,
  mmgsdi_client_data_type             client_data
)
{
  mmgsdi_cnf_type                    *cnf_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(response_cb);

  /* Obtain memory for confirmation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cnf_ptr,
                                     sizeof(mmgsdi_cnf_type));
  if(cnf_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate the mmgsdi read cnf data */
  cnf_ptr->read_cnf.accessed_rec_num   = rec_num;
  cnf_ptr->read_cnf.accessed_offset    = offset;
  cnf_ptr->read_cnf.read_data          = cnf_data;

  (void) mmgsdi_util_copy_access_type(&cnf_ptr->read_cnf.access,
                                      &file_access);

  /* Construct the mmgsdi cnf response header in order to notify the client */
  cnf_ptr->response_header.response_len        = uint32toint32(sizeof(mmgsdi_read_cnf_type)) +
      cnf_ptr->read_cnf.read_data.data_len;
  cnf_ptr->response_header.payload_len         = cnf_ptr->response_header.response_len -
      uint32toint32(sizeof(mmgsdi_response_header_type));
  cnf_ptr->response_header.response_type       = MMGSDI_READ_CNF;
  cnf_ptr->response_header.slot_id             = slot_id;
  cnf_ptr->response_header.client_data         = client_data;
  cnf_ptr->response_header.session_id          = session_id;
  cnf_ptr->response_header.client_id           = client_id;
  cnf_ptr->response_header.status_word.present = FALSE;
  cnf_ptr->response_header.mmgsdi_status       = cmd_status;

  if( MMGSDI_SUCCESS == cmd_status )
  {
    cnf_ptr->response_header.status_word.present = TRUE;
    cnf_ptr->response_header.status_word.sw1     = GSDI_SIM_SW1_NORM_END;
    cnf_ptr->response_header.status_word.sw2     = GSDI_SIM_SW2_NORM_END;
  }

  /* Lets call the client callback with cnf information */
  (response_cb)(cmd_status,
                MMGSDI_READ_CNF,
                cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

  return MMGSDI_SUCCESS;
} /* mmgsdi_caller_context_build_and_process_read_cnf */


/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_READ_CACHE_AND_NOTIFY_CLIENT

   DESCRIPTION:
     This function tries to read the item from MMGSDI cache. If the read
     succeeds or the item is not found on the card, a response is built
     immediately and cnf is sent to the client. App state must be ready;
     otherwise we simply return without doing anything.
     If the function is invoked from MMGSDI tasks context or if the access
     method is not BY_ENUM, we simply return without any processing.
     This function uses critical sections.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

      MMGSDI_SUCCESS:          The read from cache succeeded and client has
                               notified with the cnf
      MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_caller_context_read_cache_and_notify_client(
  const mmgsdi_task_cmd_type             *task_cmd_ptr
)
{
  mmgsdi_client_id_type              client_id        = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_info_type         * session_info_ptr = NULL;
  mmgsdi_slot_id_enum_type           slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_len_type                    cache_item_len   = 0;
  mmgsdi_cache_init_enum_type        cache_item_state = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_session_id_type             session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_offset_type                 offset           = 0;
  mmgsdi_len_type                    req_len          = 0;
  mmgsdi_client_data_type            client_data      = 0;
  mmgsdi_callback_type               response_cb      = NULL;
  boolean                            is_file_present  = FALSE;
  mmgsdi_access_type                 file_access;
  mmgsdi_data_type                   read_data;
  mmgsdi_int_app_info_type         * app_info_ptr     = NULL;

  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
  memset(&read_data, 0x00, sizeof(mmgsdi_data_type));

  /* We only support reading from cache (if avaialble) for read requests.
     For other requests, we return error. In case of record-based files,
     only the first record is stored in the session cache and hence here
     we try to read from cache only if record number is 1. */
  if((task_cmd_ptr == NULL) ||
     (task_cmd_ptr->cmd.cmd_enum != MMGSDI_READ_REQ) ||
     (task_cmd_ptr->cmd.cmd.read_req.file_type != MMGSDI_TRANSPARENT_FILE &&
      task_cmd_ptr->cmd.cmd.read_req.file_type != MMGSDI_LINEAR_FIXED_FILE) ||
     (task_cmd_ptr->cmd.cmd.read_req.file_type == MMGSDI_LINEAR_FIXED_FILE &&
      task_cmd_ptr->cmd.cmd.read_req.rec_num != 1))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  session_id  = task_cmd_ptr->cmd.cmd.read_req.request_header.session_id;
  offset      = task_cmd_ptr->cmd.cmd.read_req.offset;
  file_access = task_cmd_ptr->cmd.cmd.read_req.access;
  req_len     = task_cmd_ptr->cmd.cmd.read_req.data_len;
  response_cb = task_cmd_ptr->cmd.cmd.read_req.request_header.response_cb;
  client_data = task_cmd_ptr->cmd.cmd.read_req.request_header.client_data;

  /* In order to achieve read requests optimization,
     only when the state of the app corresponding to client's session id
     is READY, try getting the item from cache first (in caller's context).
     If that fails and for all other app states, continue with queuing
     read request to MMGSDI. As part of accessing the cache here, do not
     worry about checking for session type as that is checked anyways
     inside the cache function below.
     The reason why we bring the optimization into picture only for app
     state READY is that we want to keep the checks as light weight as
     possible in caller's context. For all other app states, we might
     need to do extra processing to figure out if the read should be
     allowed and we should avoid all that in caller's context.
     The reason for not continuing when access menthod is NOT BY_ENUM
     is that cache access is anyways going to be rejected if the access
     method is BY_PATH. So, we do not even attempt calling tha cache READ
     API and simple continue with queuing the READ request to MMGSDI tasks
     so that the request can be forwarded to the card */

  if(mmgsdi_util_get_current_task() != MMGSDI_TASK_MAX ||
     file_access.access_method != MMGSDI_EF_ENUM_ACCESS)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* Get the client id */
    mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Could not get client id corresponding to session id: 0x%x",
                    session_id);
      break;
    }

    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      session_id,
                      &slot_id,
                      NULL,
                      NULL,
                      NULL,
                      &session_info_ptr,
                      NULL);

    if(mmgsdi_status    != MMGSDI_SUCCESS ||
       session_info_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* The read request are allowed to read from the cache only for prov & card sessions.
       We return error for non-prov sessions.*/
    if(mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    is_file_present = mmgsdi_present_in_preinit_file_access_list(&task_cmd_ptr->cmd);

    /* session type is card session */
    if(mmgsdi_util_is_card_session(session_info_ptr->session_type))
    {
      if(is_file_present == FALSE)
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
    }
    else
    {
      mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                        NULL,
                                                        NULL,
                                                        &app_info_ptr);
      if((mmgsdi_status != MMGSDI_SUCCESS) ||
         (app_info_ptr == NULL) ||
         ((app_info_ptr->app_state != MMGSDI_APP_STATE_READY) &&
          (is_file_present == FALSE)))
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
    }

    /* Read item cache length */
    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                          &file_access,
                                          &cache_item_len,
                                          0,
                                          &cache_item_state);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }
     /* If req data_len is 0 or more than the file size, it means the client is
        requesting for the full contents of the file beginning from the offset.
        If req data_len is more than file_size then from MMGSDI task also we
        will return the entire file size */
    if(req_len > 0 && req_len < cache_item_len)
    {
      read_data.data_len = req_len;
    }
    else
    {
      read_data.data_len = cache_item_len;
    }

    mmgsdi_status = MMGSDI_ERROR;

    if(read_data.data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_data.data_ptr,
                                         read_data.data_len);
      if(read_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      mmgsdi_status = mmgsdi_session_read_cache_ext(session_id,
                                                    file_access.file.file_enum,
                                                    &read_data,
                                                    offset);
    }
  }while (0);

  /* Item is either not cached and service is not set or item is not chached
     and service is set. */
  if (mmgsdi_status == MMGSDI_NOT_INIT && app_info_ptr != NULL)
  {
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(client_id,
                                                        session_id,
                                                        app_info_ptr->app_data.app_type,
                                                        &file_access);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* service is set and cache not available we have to queue the
         command to mmgsdi. */
      mmgsdi_status = MMGSDI_NOT_INIT;
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if((mmgsdi_status == MMGSDI_SUCCESS) ||
     (mmgsdi_status == MMGSDI_NOT_FOUND && cache_item_state == MMGSDI_CACHE_NOT_FOUND) ||
     (mmgsdi_status == MMGSDI_NOT_SUPPORTED) ||
     (mmgsdi_status == MMGSDI_ACCESS_DENIED))
  {
    /* Item is either found in the cache or we figure that the item
       was not cached because it was not found on the card, or the service is
       not set or read of service table EF fails in service table when
       cache is not init, In all cases, we should return to the caller. */
    mmgsdi_status = mmgsdi_caller_context_build_and_process_read_cnf(client_id,
                                                                     session_id,
                                                                     slot_id,
                                                                     mmgsdi_status,
                                                                     offset,
                                                                     task_cmd_ptr->cmd.cmd.read_req.rec_num,
                                                                     file_access,
                                                                     read_data,
                                                                     response_cb,
                                                                     client_data);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_data.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_caller_context_read_cache_and_notify_client */


/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_BUILD_AND_PROCESS_APP_CAPABILITIES_CNF

   DESCRIPTION:
     This function populates the confirmation information and processes
     the response/confirmation directly without queuing it to mmgsdi task

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

       MMGSDI_SUCCESS:          The response and cnf are built successfully
                                and client has been notified with the cnf
       MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_caller_context_build_and_process_app_capabilities_cnf(
  mmgsdi_client_id_type               client_id,
  mmgsdi_session_id_type              session_id,
  mmgsdi_slot_id_enum_type            slot_id,
  boolean                             fdn_enabled,
  boolean                             bdn_enabled,
  boolean                             acl_enabled,
  boolean                             imsi_invalidated,
  mmgsdi_callback_type                response_cb,
  mmgsdi_client_data_type             client_data
)
{
  mmgsdi_cnf_type                    *cnf_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(response_cb);

  /* Obtain memory for confirmation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cnf_ptr,
                                     sizeof(mmgsdi_cnf_type));
  if(cnf_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate the mmgsdi app capabilities cnf data */
  cnf_ptr->session_app_capabilities_cnf.fdn_enabled = fdn_enabled;
  cnf_ptr->session_app_capabilities_cnf.bdn_enabled = bdn_enabled;
  cnf_ptr->session_app_capabilities_cnf.acl_enabled = acl_enabled;
  cnf_ptr->session_app_capabilities_cnf.imsi_invalidated = imsi_invalidated;

  /* Construct the mmgsdi cnf response header in order to notify the client */
  cnf_ptr->response_header.response_len        = uint32toint32(sizeof(mmgsdi_session_app_capabilities_cnf_type));
  cnf_ptr->response_header.payload_len         = cnf_ptr->response_header.response_len -
      uint32toint32(sizeof(mmgsdi_response_header_type));
  cnf_ptr->response_header.response_type       = MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF;
  cnf_ptr->response_header.slot_id             = slot_id;
  cnf_ptr->response_header.client_data         = client_data;
  cnf_ptr->response_header.session_id          = session_id;
  cnf_ptr->response_header.client_id           = client_id;
  cnf_ptr->response_header.status_word.present = FALSE;
  cnf_ptr->response_header.mmgsdi_status       = MMGSDI_SUCCESS;

  /* Lets call the client callback with cnf information */
  (response_cb)(MMGSDI_SUCCESS,
                MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF,
                cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

  return MMGSDI_SUCCESS;
}/* mmgsdi_caller_context_build_and_process_app_capabilities_cnf */


/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_CHECK_APP_CAPABILITIES_AND_NOTIFY_CLIENT

   DESCRIPTION:
     This function tries to retrieve app capabilites from cached data.
     If the function is invoked from MMGSDI tasks context we simply return
     without any processing.
     This function uses critical sections.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

      MMGSDI_SUCCESS:          The retrieval of service status from cached
                               service table succeeded and client has
                               notified with the cnf
      MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_caller_context_check_app_capabilities_and_notify_client(
  const mmgsdi_task_cmd_type             *task_cmd_ptr
)
{
  mmgsdi_client_id_type              client_id        = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type         * app_info_ptr     = NULL;
  mmgsdi_slot_id_enum_type           slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_id_type             session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_client_data_type            client_data      = 0;
  mmgsdi_callback_type               response_cb      = NULL;
  boolean                            fdn_enabled      = FALSE;
  boolean                            bdn_enabled      = FALSE;
  boolean                            acl_enabled      = FALSE;
  boolean                            imsi_invalidated = FALSE;


  if((task_cmd_ptr == NULL) ||
     (task_cmd_ptr->cmd.cmd_enum != MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(mmgsdi_util_get_current_task() != MMGSDI_TASK_MAX)
  {
    return MMGSDI_ERROR;
  }

  session_id  = task_cmd_ptr->cmd.cmd.session_get_app_cap_req.request_header.session_id;
  response_cb = task_cmd_ptr->cmd.cmd.session_get_app_cap_req.request_header.response_cb;
  client_data = task_cmd_ptr->cmd.cmd.session_get_app_cap_req.request_header.client_data;

  /* In order to achieve app capabilities requests optimization,
     try getting the app capabilities from cached data first
     in caller's context. If that fails, continue with queuing
     app capabilities request to MMGSDI. */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      session_id,
                      &slot_id,
                      NULL,
                      &app_info_ptr,
                      NULL,
                      NULL,
                      NULL);

    if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Get the client id */
    mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Could not get client id corresponding to session id: 0x%x",
                    session_id);
      break;
    }

    /* Check app capabilities */
    if(!app_info_ptr->app_capabilities.all_srv_init)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)
    {
      fdn_enabled = TRUE;
    }

    if(app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_ENABLE)
    {
      bdn_enabled = TRUE;
    }

    if(app_info_ptr->app_capabilities.acl_state == MMGSDI_SRV_ENABLE)
    {
      acl_enabled = TRUE;
    }

    imsi_invalidated = app_info_ptr->app_capabilities.imsi_invalidated;

  }while (0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* app capabilities is successfully retrieved from app info. */
    mmgsdi_status = mmgsdi_caller_context_build_and_process_app_capabilities_cnf(
                      client_id,
                      session_id,
                      slot_id,
                      fdn_enabled,
                      bdn_enabled,
                      acl_enabled,
                      imsi_invalidated,
                      response_cb,
                      client_data);
  }

  return mmgsdi_status;
}/* mmgsdi_caller_context_check_app_capabilities_and_notify_client */

/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_BUILD_AND_PROCESS_SAP_CARD_READER_STATUS_CNF

   DESCRIPTION:
     This function populates the confirmation information and processes
     the response/confirmation directly without queuing it to mmgsdi task for
     SAP_CARD_READER_STATUS

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

       MMGSDI_SUCCESS:          The response and cnf are built successfully
                                and client has been notified with the cnf
       MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_caller_context_build_and_process_sap_card_reader_status_cnf(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       slot_id,
  mmgsdi_callback_type           response_cb,
  mmgsdi_client_data_type        client_data,
  uint8                          card_reader_status_data
)
{
  mmgsdi_cnf_type               *cnf_ptr      = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(response_cb);

  /* Obtain memory for confirmation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cnf_ptr,
                                     sizeof(mmgsdi_cnf_type));
  if(cnf_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  cnf_ptr->response_header.response_len        = uint32toint32(sizeof(mmgsdi_sap_card_reader_status_cnf_type)) +
      sizeof(uint8);
  cnf_ptr->response_header.payload_len         = cnf_ptr->response_header.response_len -
      uint32toint32(sizeof(mmgsdi_response_header_type));
  cnf_ptr->response_header.response_type       = MMGSDI_SAP_CARD_READER_STATUS_CNF;
  cnf_ptr->response_header.slot_id             = slot_id;
  cnf_ptr->response_header.client_data         = client_data;
  cnf_ptr->response_header.client_id           = client_id;
  cnf_ptr->response_header.status_word.present = FALSE;
  cnf_ptr->response_header.mmgsdi_status       = MMGSDI_SUCCESS;
  cnf_ptr->sap_card_reader_status_cnf.card_reader_status_data.data_ptr = &card_reader_status_data;
  cnf_ptr->sap_card_reader_status_cnf.card_reader_status_data.data_len = sizeof(uint8);

  /* Lets call the client callback with cnf information */
  (response_cb)(MMGSDI_SUCCESS,
                MMGSDI_SAP_CARD_READER_STATUS_CNF,
                cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

  return MMGSDI_SUCCESS;
}/* mmgsdi_caller_context_build_and_process_sap_card_reader_status_cnf */


/* ==========================================================================
   FUNCTION:      MMGSDI_CALLER_CONTEXT_SAP_CARD_READER_STATUS_AND_NOTIFY_CLIENT

   DESCRIPTION:
     This function tries to retrieve card reader status if the slot_data_ptr is available for
     that particular Slot. This function is only used for SAP_CARD_READER_STATUS.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type:

      MMGSDI_SUCCESS:          The retrieval of service status from cached
                               service table succeeded and client has
                               notified with the cnf
      MMGSDI_ERROR:            Otherwise

   SIDE EFFECTS:

==========================================================================*/

mmgsdi_return_enum_type mmgsdi_caller_context_sap_card_reader_status_and_notify_client(
  const mmgsdi_task_cmd_type             *task_cmd_ptr
)
{
  mmgsdi_client_id_type      client_id               = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_client_data_type    client_data             = 0;
  mmgsdi_callback_type       response_cb             = NULL;
  mmgsdi_return_enum_type    mmgsdi_status           = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type   slot_id                 = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                      slot_index              = 0;
  uint8                      card_reader_status_data = 0x00;
  mmgsdi_slot_data_type     *slot_data_ptr           = NULL;

  if((task_cmd_ptr == NULL) ||
     (task_cmd_ptr->cmd.cmd_enum != MMGSDI_SAP_CARD_READER_STATUS_REQ))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  response_cb = task_cmd_ptr->cmd.cmd.sap_card_reader_status_req.request_header.response_cb;
  client_data = task_cmd_ptr->cmd.cmd.sap_card_reader_status_req.request_header.client_data;
  client_id   = task_cmd_ptr->cmd.cmd.sap_card_reader_status_req.request_header.client_id;
  slot_id     = task_cmd_ptr->cmd.cmd.sap_card_reader_status_req.request_header.slot_id;

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id, task_cmd_ptr->cmd.cmd_enum);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(mmgsdi_util_get_current_task() != MMGSDI_TASK_MAX)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_sap_connected[slot_index])
  {
    /*bits 1-3              = identity of card reader x.
      bit 4               0 = Card reader is not removable
                          1 = Card reader is removable
      bit 5               0 = Card reader is not present
                          1 = Card reader is present
      bit 6               0 = Card reader present is not ID-1 size
                          1 = Card reader present is ID-1 size
      bit 7               0 = No card present
                          1 = Card is present in reader
      bit 8               0 = No card powered
                          1 = Card in reader is powered
    */

    /* Card reader is present      = 1 <b5> */
    card_reader_status_data |= 0x10;

    /*Card Reader is ID-1 size     = 1 <b6>*/
    card_reader_status_data |= 0x20;

    /*Card is Present in the reader - Check gsdi_data*/
    /*Card in Reader is powered if it is present*/
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

    if(slot_data_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
    else
    {
      if(slot_data_ptr->mmgsdi_sap_state != MMGSDI_STATE_NO_CARD)
      {
        card_reader_status_data |= 0xC0;
      }
    }
  }
  else
  {
    UIM_MSG_ERR_0("SAP card reader status Req error: BT is not connected");
    mmgsdi_status = MMGSDI_ERROR;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Card Reader Status 0x%x", card_reader_status_data);

    mmgsdi_status = mmgsdi_caller_context_build_and_process_sap_card_reader_status_cnf(
                      client_id,
                      slot_id,
                      response_cb,
                      client_data,
                      card_reader_status_data);
  }

  return mmgsdi_status;
}/* mmgsdi_caller_context_sap_card_reader_status_and_notify_client */
