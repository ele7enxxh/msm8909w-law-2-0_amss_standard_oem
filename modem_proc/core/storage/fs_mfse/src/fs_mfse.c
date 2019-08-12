/***********************************************************************
 * fs_mfse.c
 *
 * Implementation of the External Modem Filesystem Service.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Implementation of the Service module that processes requests from off-chip
 * clients. This service currently supports messages to force a sync from
 * Modem filesystem and to query the status of a sync request.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/fs_mfse.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create.

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_MFSE

#include "fs_mfse_std_interface.h"
#include "fs_mfse_util.h"
#include "fs_mfse.h"
#include "fs_public.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_util.h"
#include "fs_osal.h"
#include "fs_sys_types.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "assert.h"

typedef struct
{
  /* Service handle provided by framework at register time */
  qmi_csi_service_handle service_handle;

  /* qsap handle provided by framework at qsap_register time */
  qmi_sap_client_handle qsap_handle;

  /* tcb and signalling information that framework uses to invoke service
     handlers */
  qmi_csi_os_params os_params;

  /* Response structures too big to fit on stack */
  fs_mfse_sync_no_wait_resp_msg     sync_no_wait_response;
  fs_mfse_sync_get_status_resp_msg  sync_get_status_response;

  /* State variables meant to help debugging issues involving timeout or
     failed process requests. */
  uint64 last_req_received_time_ms;  /* Time request was received */
  uint64 last_req_processed_time_ms; /* Time request was done procossing. Could
                                        be a failed or successful request. */
  /* State of the service. 1 implies connected to framework and 0 otherwise. */
  uint8 server_up;
} fs_mfse_object_type;
static fs_mfse_object_type fs_mfse_inst;

/* Critical section and APIs for static data protection. */
static fs_os_crit_sect_type fs_mfse_crit_sect;

void
fs_mfse_lock_init (void)
{
  memset (&fs_mfse_crit_sect, 0x0, sizeof (fs_os_crit_sect_type));
  fs_os_init_crit_sect (&fs_mfse_crit_sect);
}

void
fs_mfse_lock (void)
{
  fs_os_enter_crit_sect (&fs_mfse_crit_sect);
}

void
fs_mfse_unlock (void)
{
  fs_os_leave_crit_sect (&fs_mfse_crit_sect);
}

/* Callback functions that service registers with framework */

/* Handles client connection. Is invoked when the client sends it first
   request message. */
qmi_csi_cb_error
fs_mfse_connect_cb (qmi_client_handle cli_handle, void *service_cookie,
                    void **connection_handle)
{
  /* EFS does not maintain any client context. */
  (void) connection_handle;
  (void) cli_handle;
  (void) service_cookie;

  FS_MSG_HIGH ("%d,%d,%d,FS_MFSE: Connect Request [con_hdl, cli_hdl, srv_cki]",
               (int)connection_handle, (int)cli_handle, (int)service_cookie);

  return QMI_CSI_CB_NO_ERR;
}


/* Handles client disconnection. Is invoked when client calls
   qmi_client_release API. */
void
fs_mfse_disconnect_cb (void *connection_handle, void *service_cookie)
{
  /* EFS does not maintain client state info. Nothing to do. */
  (void) connection_handle;
  (void) service_cookie;

  FS_MSG_HIGH ("%d, %d, FS_MFSE: Disconnect Request [con_hdl, serv_cki]",
               (int)connection_handle, (int)service_cookie, 0);
}

static qmi_csi_cb_error
fs_mfse_sync_no_wait_handler (qmi_req_handle req_handle, int msg_id,
                              void *req_struct, int req_struct_len)
{
  int result;
  const char *path;
  fs_sync_token_t sync_token;
  int req_msg_len, resp_msg_len;
  fs_mfse_sync_no_wait_req_msg *req_msg;
  fs_mfse_sync_no_wait_resp_msg *resp_msg;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error service_err = QMI_CSI_INTERNAL_ERR;
  enum fs_sync_result sync_status = FS_SYNC_PENDING;

  req_msg_len  = sizeof (fs_mfse_sync_no_wait_req_msg);
  resp_msg_len = sizeof (fs_mfse_sync_no_wait_resp_msg);

  req_msg = (fs_mfse_sync_no_wait_req_msg *) req_struct;
  resp_msg = &(fs_mfse_inst.sync_no_wait_response);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = QMI_RESULT_FAILURE_V01;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FS_ERR ("%d,%d,%d,FS_MFSE:Malformed Req Msg received[msg_id,len,exp_len]",
             msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    goto END;
  }

  /* Message structre is good. Fill in default service error codes. */
  resp_msg->resp.error  = QMI_ERR_CAUSE_CODE_V01;
  resp_msg->efs_err_num_valid = 1;

  path = fs_mfse_util_get_fs_path_for_id (req_msg->fs_id);
  if (path == NULL)
  {
    FS_ERR ("%d,%d,FS_MFSE : Unknown filesystem ID in request [msg_id, fs_id]",
             msg_id, req_msg->fs_id, 0);
    resp_msg->efs_err_num = FS_MFSE_EINVAL;
    goto END;
  }

  /* Fill default error codes */
  resp_msg->resp.error  = QMI_ERR_CAUSE_CODE_V01;
  resp_msg->efs_err_num_valid = 1;

  result = efs_sync_no_wait (path, &sync_token);
  if (result != 0)
  {
    FS_ERR ("%d,%d,%d, FS_MFSE: Sync_no_wait failed[req_msg, path, errno]",
            (int)req_msg, (int)path, efs_errno);
    resp_msg->efs_err_num = fs_mfse_util_get_mfse_errno (efs_errno);
    goto END;
  }

  result = efs_sync_get_status (path, sync_token, &sync_status);
  if (result != 0) /* Should not happen, but we have tracks covered */
  {
    FS_ERR ("%d,%d,%d,FS_MFSE:Get_status on sync failed[req_msg,path,errno]",
            (int)req_msg, (int)path, efs_errno);
    goto SUCCESS;
  }
  else
  {
    if (sync_status == FS_SYNC_COMPLETE)
    {
      resp_msg->efs_err_num = fs_mfse_util_get_mfse_errno (ENOTHINGTOSYNC);
      goto END;
    }
  }

SUCCESS:
  /* Fix response for success. */
  resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg->resp.error  = QMI_ERR_NONE_V01;
  resp_msg->sync_token_valid = 1;
  resp_msg->sync_token = sync_token;
  resp_msg->efs_err_num_valid = 0;

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FS_ERR ("%d,%d,%d,FS_MFSE : qmi_csi_send_resp failed[err,msg_id,resp_msg]",
            service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;
}

static qmi_csi_cb_error
fs_mfse_sync_get_status_handler (qmi_req_handle req_handle, int msg_id,
                                 void *req_struct, int req_struct_len)
{
  int result;
  const char *path;
  enum fs_sync_result sync_result;
  int req_msg_len, resp_msg_len;
  fs_mfse_sync_get_status_req_msg *req_msg;
  fs_mfse_sync_get_status_resp_msg *resp_msg;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error service_err = QMI_CSI_INTERNAL_ERR;

  req_msg_len  = sizeof (fs_mfse_sync_get_status_req_msg);
  resp_msg_len = sizeof (fs_mfse_sync_get_status_resp_msg);

  req_msg = (fs_mfse_sync_get_status_req_msg *) req_struct;
  resp_msg = &(fs_mfse_inst.sync_get_status_response);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = QMI_RESULT_FAILURE_V01;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FS_ERR ("%d,%d,%d,FS_MFSE:Malformed Req Msg received[msg_id,len,exp_len]",
             msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    goto END;
  }

  /* Message structure is good. Fill in default service error codes */
  resp_msg->resp.error  = QMI_ERR_CAUSE_CODE_V01;
  resp_msg->efs_err_num_valid = 1;

  path = fs_mfse_util_get_fs_path_for_id (req_msg->fs_id);
  if (path == NULL)
  {
    FS_ERR ("%d, %d, FS_MFSE:Unknown filesystem ID in req [msg_id, fs_id]",
             msg_id, req_msg->fs_id, 0);
    resp_msg->efs_err_num = FS_MFSE_EINVAL;
    goto END;
  }

  result = efs_sync_get_status (path, req_msg->sync_token, &sync_result);
  if (result != 0)
  {
    FS_ERR ("%d,%d,%d, FS_MFSE : Sync_get_status failed [req_msg,path,errno]",
             (int)req_msg, (int)path, efs_errno);
    resp_msg->efs_err_num = fs_mfse_util_get_mfse_errno (efs_errno);
    goto END;
  }

  resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg->resp.error  = QMI_ERR_NONE_V01;
  resp_msg->sync_status_valid = 1;
  resp_msg->sync_status = fs_mfse_util_get_mfse_sync_status (sync_result);
  resp_msg->efs_err_num_valid = 0;

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FS_ERR ("%d,%d,%d,FS_MFSE :qmi_csi_send_resp failed[err,msg_id,resp_msg]",
             service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;
}

/* Handles process requests from client. Invoked when clients send any
   request messages */
qmi_csi_cb_error
fs_mfse_process_req_cb (void *connection_handle, qmi_req_handle req_handle,
                        unsigned int msg_id, void *req_struct,
                        unsigned int req_struct_len, void *service_cookie)
{
  qmi_csi_cb_error err = QMI_CSI_CB_INTERNAL_ERR;

  (void) connection_handle;
  (void) service_cookie;

  fs_mfse_inst.last_req_received_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH ("%d, %d, FS_MFSE : Service request received [time_ms,msg_id]",
              (uint32)fs_mfse_inst.last_req_received_time_ms, msg_id, 0);

  /* Input validation */
  if ((req_handle == NULL) || (req_struct == NULL) || (req_struct_len == 0))
  {
    FS_ERR ("%d,%d,%d,FS_MFSE:Incorrect handles received[req_hdl,req_str,len]",
            (int)req_handle, (int)req_struct, req_struct_len);
    goto END;
  }

  switch (msg_id)
  {
    case FS_MFSE_SYNC_NO_WAIT_REQ:
      err = fs_mfse_sync_no_wait_handler (req_handle, msg_id,
                                          req_struct, req_struct_len);
      break;

    case FS_MFSE_SYNC_GET_STATUS_REQ:
      err = fs_mfse_sync_get_status_handler (req_handle, msg_id,
                                             req_struct, req_struct_len);
      break;

    default:
      FS_ERR ("%d, FS_MFSE: Unknown Msg ID received [msg_id]", msg_id, 0, 0);
      break;
  }

END:
  fs_mfse_inst.last_req_processed_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH ("%d,%d,%d, FS_MFSE : Request processed [err, msg_id, time_ms]",
               err, msg_id, (uint32)fs_mfse_inst.last_req_processed_time_ms);

  return err;
}


/* Initialize the service module. */
void
fs_mfse_init (void)
{
  qmi_csi_error err;
  qmi_idl_service_object_type service_obj;

  fs_mfse_lock ();

  memset (&fs_mfse_inst, 0x0, sizeof (fs_mfse_object_type));

  service_obj = fs_mfse_get_service_object ();
  if (service_obj == NULL)
  {
    FS_ERR_FATAL ("FS_MFSE: get_service_object failed. Check .c and .h",
                   0, 0, 0);
  }

  fs_mfse_inst.os_params.tcb = fs_os_self ();
  fs_mfse_inst.os_params.sig = FS_MFSE_HANDLE_EVENT;

  /* Registers function for handling QMI Message Query Requests */
  (void) qmi_si_register_object(service_obj,
                                0, /* Service Instance */
                                fs_mfse_get_service_impl ());

  err = qmi_csi_register (service_obj,
                          fs_mfse_connect_cb,
                          fs_mfse_disconnect_cb,
                          fs_mfse_process_req_cb,
                          NULL, /* Service_cookie : Service specific context.*/
                          &fs_mfse_inst.os_params,
                          &fs_mfse_inst.service_handle);
  if (err != QMI_CSI_NO_ERR)
  {
    FS_ERR_FATAL ("%d, %d, FS_MFSE : qmi_csi_register failed [err, serv_obj]",
                  err, (int)service_obj, 0);
  }
  ASSERT (fs_mfse_inst.service_handle != NULL);

  err = qmi_sap_register (service_obj, NULL, &fs_mfse_inst.qsap_handle);
  if (err != QMI_CSI_NO_ERR)
  {
    FS_ERR_FATAL ("%d,%d,%d,FS_MFSE:sap_register failed[err,serv_ob,qsap_hdl]",
                  err, (int) service_obj, (int) fs_mfse_inst.qsap_handle);
  }
  ASSERT (fs_mfse_inst.qsap_handle != NULL);

  fs_mfse_inst.server_up = 1;
  FS_MSG_HIGH ("FS_MFSE : Init complete", 0, 0, 0);

  fs_mfse_unlock ();
}

/* Call qmi_csi_handle_event to get appropriate callbacks */
void
fs_mfse_handle_event (void)
{
  qmi_csi_error err;

  FS_MSG_HIGH ("%d, FS_MFSE: Handle Event Called [time_ms]",
               (uint32)fs_time_ms_u64 (), 0, 0);

  /* Serialize requests from clients. No other client's request can be handled
     until this client's request is processed. */
  fs_mfse_lock ();

  err = qmi_csi_handle_event (fs_mfse_inst.service_handle,
                              &fs_mfse_inst.os_params);
  if (err != QMI_CSI_NO_ERR)
  {
    FS_ERR ("%d, FS_MFSE: Handle event error [err]", err, 0, 0);
  }

  fs_mfse_unlock ();
}

#endif /* FEATURE_EFS_MFSE */
