/***********************************************************************
 * fs_qmi_service.c
 * The EFS Service Module for remote apps efs_put efs_get client.
 *
 * Copyright (C) 2011-2014 QUALCOMM Technologies, Inc.
 *
 * This file provides implementation for service module that processes efs_put
 * and efs_get requests from the client module running on remote processor. The
 * remote processor does not have full EFS support and provides support for
 * efs_get and efs_put APIs. It does this by remoting these calls to the modem
 * processor over Qualcomm MSM Interface (QMI) using the
 * QMI Common Client Interface(QCCI) framework. With each request from the
 * client the service task gets signaled which then invokes the service module.
 * The service processes requests on the modem and sends responses to the
 * client using the QMI Common Service Interface (QCSI) framework.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_qmi_put_get/service/src/fs_qmi_service.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-01-28   dks   Added support to query commands/TLVs supported by service.
2012-03-23   dks   Add remote processor access check for paths.
2011-12-12   rp    Do not validate connection handle as we don't use it.
2011-11-21   dks   Fix service_cookie NULL check.
2011-11-14   dks   Fix security vulnerabilities.
2011-10-04   dks   Create.

===========================================================================*/
#include "fs_config_i.h"

#ifdef FEATURE_EFS_QMI_PUT_GET_SERVICE

#include "fs_qmi_std_interface.h"
#include "fs_qmi_service.h"
#include "fs_osal.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_osal.h"
#include "fs_qmi_util.h"
#include "qmi_csi.h"
#include "fs_util.h"
#include "task.h"
#include "assert.h"
#include <string.h>

struct fs_qmi_service_object_type
{
  /* The service_provider handle is provided to the service from the QCSI
     framework as a return value from the qmi_csi_register function. This
     handle is used by the framework to identify the service when the service
     calls the qmi_csi_handle_event, qmi_csi_unregister functions. */
  qmi_csi_service_handle service_provider;

  /* When the QCSI receives an event for a particular service, i.e., client
     connect, disconnect, or request message, the QCSI framework uses the
     signaling information that was provided in the os_params value that was
     passed in qmi_csi_register. It contains a signal and a tcb pointer. */
  qmi_csi_os_params os_params;

  /* QMI IDL defined response structures to be sent to the client in response
     to the process requests sent. QMI can only encode and decode the messages
     defined in IDL which are compiled into C structures in the generated
     header file. Corresponding .c file handles encoding and decoding of the
     structures. The sizes of these structures are big and cannot be put on the
     stack. */
  fs_qmi_efs_put_resp_msg efs_put_response;
  fs_qmi_efs_get_resp_msg efs_get_response;

  /* Following are state variables meant to help debugging issues involving
     timeout or failed process requests. */

  /* Time in msecs when the service received the last request. */
  uint64 last_req_received_time_ms;

 /* Time in msecs when the service is done processing the last request. */
  uint64 last_req_processed_time_ms;

  /* State of the service. TRUE implies connected to framework and FALSE
     implies otherwise. */
  boolean server_up;
};
static struct fs_qmi_service_object_type fs_qmi_service_obj;

/* Critical section to protect structures in fs_qmi_service_obj from
   simultaneous access. */
fs_os_crit_sect_type fs_qmi_service_crit_sect;
/* APIs to control access to the critical section. */
static void
fs_qmi_service_lock_init (void)
{
  /* On simulator, after catching fatal errors, globals will not get
     reinitialized */
  memset (&fs_qmi_service_crit_sect, 0x0, sizeof (fs_qmi_service_crit_sect));
  fs_os_init_crit_sect (&fs_qmi_service_crit_sect);
}

static void
fs_qmi_service_lock (void)
{
  fs_os_enter_crit_sect (&fs_qmi_service_crit_sect);
}

static void
fs_qmi_service_unlock (void)
{
  fs_os_leave_crit_sect (&fs_qmi_service_crit_sect);
}


/* fs_qmi_service_connect_cb
 * Connect callback function registered with the framework to be invoked when
 * client sends the first request message to be service.
 */
qmi_csi_cb_error
fs_qmi_service_connect_cb (qmi_client_handle cli_handle, void *service_cookie,
                           void **connection_handle)
{
  (void) connection_handle;

  FS_MSG_HIGH_0 ("QMI Service received connect Request");

  /* QMI should not send NULL params. Validate params for future use. */
  if (!cli_handle || (service_cookie != NULL))
  {
    FS_MSG_ERROR_3 (
             "[%d] [%d] [%d] QMI Service got incorrect handles from QMI",
             (int) cli_handle, (int) connection_handle, (int) service_cookie);
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* There is no need to store client specific context information. */
  return QMI_CSI_CB_NO_ERR;
}


/* fs_qmi_service_disconnect_cb
 * Callback function registered with the framework to be invoked when a client
 * requests to remove connection with service.
 */
void
fs_qmi_service_disconnect_cb (void *conn_handle, void *service_cookie)
{
  (void) conn_handle;

  FS_MSG_HIGH_0 ("QMI Service received disconnect Request");

  /* QMI should not send NULL params. Validate params for future use. */
  if (service_cookie != NULL)
  {
    FS_MSG_ERROR_2 ("[%d,%d] QMI Service got incorrect handles from QMI",
             (int) conn_handle, (int) service_cookie);
  }

  /* Nothing to do. */
}


/* The max value that fs_qmi_oflag_type can have. FS_QMI_O_SFS_RSVD
   specifies the largest bit that can be set in the oflag. All bits lower
   than this can be set, but nothing higher than this bit. */
#define FS_QMI_O_MAX_EFS_QMI_OFLAG (FS_QMI_O_SFS_RSVD * 2)

/* fs_qmi_efs_put_handler
 * handle efs_put messages sent from the client.
 */
static qmi_csi_cb_error
fs_qmi_efs_put_handler (qmi_req_handle req_handle, unsigned int msg_id,
                        void *req_c_struct, unsigned int req_c_struct_len)
{
  int result;
  int32 oflag;
  qmi_csi_error err;
  fs_qmi_efs_put_resp_msg *resp_msg;
  fs_qmi_efs_put_req_msg *req_msg;

  req_msg = (fs_qmi_efs_put_req_msg *) req_c_struct;
  resp_msg = &fs_qmi_service_obj.efs_put_response;

  /* Clear response message. */
  memset (resp_msg, 0, sizeof (fs_qmi_service_obj.efs_put_response));

  /* Validate request fields. */
  if ((req_c_struct_len != sizeof (fs_qmi_efs_put_req_msg))  ||
      (!req_msg->path)                                       ||
      ((strlen (req_msg->path) + 1) > FS_QMI_MAX_FILE_PATH)  ||
      (!req_msg->data)                                       ||
      (req_msg->data_len > FS_QMI_MAX_ITEM_SIZE)             ||
      (req_msg->oflag == 0)                                  ||
      (req_msg->oflag >= FS_QMI_O_MAX_EFS_QMI_OFLAG))
  {
    /* Set response result status to failure to indicate any error not set by
       the API itself. Framework will ignore the rest of the fields in this
       case. */
    FS_MSG_ERROR_3 ("[%d] [%d] [%d] QMI Service got malformed msg from QMI",
             req_c_struct_len, (int) req_msg->path, (int) req_msg->data);

    resp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg->resp.error = QMI_ERR_MALFORMED_MSG_V01;

    goto cleanup;
  }

  /* When there is no param error set qmi_response to SUCCESS. */
  resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg->resp.error = QMI_ERR_NONE_V01;

  oflag = fs_qmi_get_efs_oflag (req_msg->oflag);

  if (fs_qmi_service_access_check (req_msg->path) == 1)
  {
    result = efs_put (req_msg->path, req_msg->data, req_msg->data_len,
                      oflag, req_msg->mode);
  }
  else
  {
    result = -1;
    efs_errno = EPERM;
  }

  if (result != 0)
  {
    resp_msg->efs_err_num_valid = TRUE;
    resp_msg->efs_err_num = fs_qmi_get_qmi_errno (efs_errno);
  }
  else
  {
    resp_msg->efs_err_num_valid = FALSE;
  }

cleanup:
  err = qmi_csi_send_resp (req_handle, msg_id, resp_msg,
                           sizeof (fs_qmi_efs_put_resp_msg));

  if (err != QMI_CSI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] qmi_csi_send_resp failed", err);
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    return QMI_CSI_CB_NO_ERR;
  }
}


/* fs_qmi_efs_get_handler
 * handle  efs_get messages sent from the client.
 */
static qmi_csi_cb_error
fs_qmi_efs_get_handler (qmi_req_handle req_handle, int msg_id,
                        void *req_c_struct, int req_c_struct_len)
{
  int result;
  qmi_csi_error err;
  fs_qmi_efs_get_resp_msg *resp_msg;
  fs_qmi_efs_get_req_msg *req_msg;

  req_msg = (fs_qmi_efs_get_req_msg *) req_c_struct;
  resp_msg = &fs_qmi_service_obj.efs_get_response;

  /* Clear response message. */
  memset (resp_msg, 0, sizeof (fs_qmi_service_obj.efs_get_response));

  /* Validate request fields. */
  if ((req_c_struct_len != sizeof (fs_qmi_efs_get_req_msg))   ||
      (!req_msg->path)                                        ||
      ((strlen (req_msg->path) + 1) > FS_QMI_MAX_FILE_PATH)   ||
      (!req_msg->data_length_valid)                           ||
      (req_msg->data_length > FS_QMI_MAX_ITEM_SIZE))
  {
    /* Set response result status to failure to indicate any error not set by
       the API itself. Framework will ignore the rest of the fields in this
       case. */
    FS_MSG_ERROR_3 ("[%d] [%d] [%d] QMI Service got malformed msg from QMI",
            req_c_struct_len, (int) req_msg->path, req_msg->data_length_valid);

    resp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg->resp.error = QMI_ERR_MALFORMED_MSG_V01;

    goto cleanup;
  }

  /* When there is no param error set qmi_response to SUCCESS. */
  resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg->resp.error = QMI_ERR_NONE_V01;

  if (fs_qmi_service_access_check (req_msg->path) == 1)
  {
    result = efs_get (req_msg->path, resp_msg->data, req_msg->data_length);
  }
  else
  {
    result = -1;
    efs_errno = EPERM;
  }

  if (result >= 0)
  {
    resp_msg->efs_err_num_valid = FALSE;
    resp_msg->data_valid = TRUE;
    resp_msg->data_len = result;
  }
  else
  {
    resp_msg->data_valid = FALSE;
    resp_msg->efs_err_num_valid = TRUE;
    resp_msg->efs_err_num = fs_qmi_get_qmi_errno (efs_errno);
  }

cleanup:
  err = qmi_csi_send_resp (req_handle, msg_id, resp_msg,
                           sizeof (fs_qmi_efs_get_resp_msg));

  if (err != QMI_CSI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] qmi_csi_send_resp failed", err);
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    return QMI_CSI_CB_NO_ERR;
  }
}

/* fs_qmi_service_process_req_cb
 * Callback function registered with the framework to be invoked when a client
 * send messages to be processed.
 */
qmi_csi_cb_error
fs_qmi_service_process_req_cb (void *connection_handle,
                               qmi_req_handle req_handle, unsigned int msg_id,
                               void *req_struct, unsigned int req_struct_len,
                               void *service_cookie)
{
  qmi_csi_cb_error err = QMI_CSI_CB_INTERNAL_ERR;

  (void) connection_handle;

  /* Service stores no client connection or context information but QMI should
     not return NULL handles. */
  if (!req_handle || !req_struct || (service_cookie != NULL))
  {
    FS_MSG_ERROR_3 (
             "[%d] [%d] [%d] QMI Service got incorrect handles from QMI",
             (int) connection_handle, (int) req_handle, (int) req_struct);
    return err;
  }

  fs_qmi_service_obj.last_req_received_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH_2 ("[%ul] [%ul] QMI Service request received",
            (uint32) fs_qmi_service_obj.last_req_received_time_ms, msg_id);

  if (msg_id == FS_QMI_EFS_PUT_REQ)
  {
    err = fs_qmi_efs_put_handler (req_handle, msg_id, req_struct,
                                  req_struct_len);
  }
  else if (msg_id == FS_QMI_EFS_GET_REQ)
  {
    err = fs_qmi_efs_get_handler (req_handle, msg_id, req_struct,
                                  req_struct_len);
  }
  else
  {
    FS_MSG_ERROR_1 ("[%d] QMI_EFS_SERVICE received unknown msg id", msg_id);
  }

  fs_qmi_service_obj.last_req_processed_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH_2 ("[%ul] [%ul] QMI Service request processed",
            (uint32) fs_qmi_service_obj.last_req_processed_time_ms, msg_id);

  return err;
}

/* fs_qmi_service_init
 * Called by the service task when task gets defined.
 */
int
fs_qmi_service_init (void)
{
  qmi_csi_error err;
  qmi_idl_service_object_type service_obj;

  /* Should not fail. */
  service_obj = fs_qmi_get_service_object ();
  if (!service_obj)
  {
    FS_ERR_FATAL ("QMI get service obj failed. Check .c and .h", 0, 0, 0);
  }

  /* Initialize and grab the lock. */
  fs_qmi_service_lock_init ();
  fs_qmi_service_lock ();

  /* On simulator, after catching fatal errors, globals will not get
     reinitialized */
  memset (&fs_qmi_service_obj, 0, sizeof (fs_qmi_service_obj));

  /* Initialize os_params. */
  fs_qmi_service_obj.os_params.tcb = fs_os_self ();
  fs_qmi_service_obj.os_params.sig = FS_QMI_EFS_SERVICE_HANDLE_EVENT;

  /* Registers function for handling QMI Message Query Requests */
  (void) qmi_si_register_object(service_obj,
                                0, /* Service Instance */
                                fs_qmi_get_service_impl_v01());

  err = qmi_csi_register (service_obj,
                          fs_qmi_service_connect_cb,
                          fs_qmi_service_disconnect_cb,
                          fs_qmi_service_process_req_cb,
                          NULL, /* no service specific context. */
                          &fs_qmi_service_obj.os_params,
                          &fs_qmi_service_obj.service_provider);
  /* Cannot fail registration. */
  if (err != QMI_CSI_NO_ERR)
  {
    FS_ERR_FATAL ("%d QMI CSI register failed", err, 0, 0);
  }
  ASSERT (fs_qmi_service_obj.service_provider != NULL);

  fs_qmi_service_obj.server_up = TRUE;

  fs_qmi_service_access_check_init ();

  FS_MSG_HIGH_0 ("fs_qmi_service init complete");

  fs_qmi_service_unlock ();

  return 0;
}


/* fs_qmi_service_close
 * Called by the service task when TASK_STOP signal is received.
 */
int
fs_qmi_service_close (void)
{
  qmi_csi_error err;
  int result = 0;

  fs_qmi_service_lock ();

  err = qmi_csi_unregister (fs_qmi_service_obj.service_provider);
  if (err != QMI_CSI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("%d QMI CSI unregister failed", err);
    result = -1;
  }

  fs_qmi_service_obj.server_up = FALSE;
  FS_MSG_HIGH_0 ("fs_qmi_service going down");

  fs_qmi_service_unlock ();

  return result;
}

/* fs_qmi_service_handle_event
 * Called by the service task when FS_QMI_EFS_SERVICE_HANDLE_EVENT signal is
 * received. Calling the handle_event function on the framework invokes the
 * connect disconnect or process request callback based on the client action.
 */
int
fs_qmi_service_handle_event (void)
{
  qmi_csi_error err;
  uint32 time;

  time = (uint32) fs_time_ms_u64 ();
  FS_MSG_HIGH_1 ("[%ul] fs_qmi_service : handle event received", time);

  /* Grab lock to serialize requests from clients. At a time only one client
     can initiate a client connect and once connected no other client's
     request can be handled until this client's connection is closed. */
  fs_qmi_service_lock ();

  err = qmi_csi_handle_event (fs_qmi_service_obj.service_provider,
                              &fs_qmi_service_obj.os_params);
  if (err != QMI_CSI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] fs_qmi_service handle evt error", err);
  }

  fs_qmi_service_unlock ();

  return 0;
}

#endif /* FEATURE_EFS_QMI_PUT_GET_SERVICE */
