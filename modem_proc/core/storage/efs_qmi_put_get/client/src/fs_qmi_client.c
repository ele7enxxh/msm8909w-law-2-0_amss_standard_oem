/***********************************************************************
 * fs_qmi_client.c
 *
 * The EFS Remote Client Module for efs_put and efs_get APIs.
 * Copyright (C) 2011-2014 QUALCOMM Technologies, Inc.
 *
 * This file provides implementation for client module that remotes efs_get
 * and efs_put APIs to the modem processor. The client module runs on a
 * processor which does not have full EFS support and provides support for
 * only efs_get and efs_put APIs, by remoting these calls to the modem
 * processor over Qualcomm MSM Interface(QMI) using the
 * QMI Common Client Interface(QCCI) framework.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_qmi_put_get/client/src/fs_qmi_client.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-03-23   dks   Add remote processor access check for paths.
2011-11-14   dks   Fix security vulnerabilities.
2011-10-04   dks   Create.
===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QMI_PUT_GET_CLIENT

#include "fs_qmi_std_interface.h"
#include "fs_qmi_util.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_err.h"
#include "fs_osal.h"
#include "fs_util.h"
#include "qmi_client.h"
#include "assert.h"
#include "stringl/stringl.h"
#include "task.h"
#include <string.h>

/* The client module sends a request to a service instance to handle the
 * efs_put and efs_get commands from the remote filesystem. There can be
 * multiple service instances that can handle this. Currently we support only
 * one service instance. */
#define FS_QMI_EFS_MAX_SERVICES 1

/* Max msecs to wait for reply from the service. 0 is interpreted as infinity
   by QMI. Since there is no standard time which the service might take to
   process the request, let the calling client deal with the urgency. */
#define FS_QMI_EFS_MSG_TIMEOUT_MSEC  30000

struct fs_qmi_client_object_type
{
  /* QMI IDL defined request and response structures to be sent to and received
     from the service. QMI can only encode and decode these messages defined in
     IDL which are compiled into C structures in the generated header header
     file. Corresponding generated .c file handles encoding and decoding of
     these structures. The sizes of these structures are big and cannot be put
     on the stack. */
  fs_qmi_efs_put_req_msg  efs_put_request;
  fs_qmi_efs_put_resp_msg efs_put_response;
  fs_qmi_efs_get_req_msg  efs_get_request;
  fs_qmi_efs_get_resp_msg efs_get_response;

  /* Object to contains the tcb and signal information to be supplied to the
     QCCI framework. It is used */
  qmi_client_os_params os_params;

  /* QMI IDL service object, a handle to specify service object of interest.
     Used by service during registration to specify the services offered.
     Used by client to request service instance registered offering these
     services. */
  qmi_idl_service_object_type service_object;

  /* Address of the service object, needs to be shared between clients to open
     connection to the service instance every time. */
  qmi_service_info service_info [FS_QMI_EFS_MAX_SERVICES];

  /* Before the client can request connection to a service it needs to get the
     list of service instances from QCCI. If no service is registered, QCCI
     returns an empty list. If this is requested at boot time, this can cause
     big delay in the boot process. Hence this will be deferred until the first
     client call. client_inited is required to not do this on every API
     request. */
  int client_inited;

  /* Following are state variables meant to help debugging issues involving
     timeout or failed process requests. */

  /* Time in msecs when the client sent the last request. */
  uint64 last_req_sent_time_ms;

  /* Time in msecs when the service returned with the response. */
  uint64 last_resp_received_time_ms;
};
static struct fs_qmi_client_object_type fs_qmi_client_obj;


/* Critical section to protect structures in fs_qmi_client_obj from
   simultaneous access */
static fs_os_crit_sect_type fs_qmi_client_crit_sect;

/* APIs to control access to the critical section */
static void
fs_qmi_client_lock_init (void)
{
  /* On simulator, after catching fatal errors, globals will not get
     reinitialized */
  memset (&fs_qmi_client_crit_sect, 0x0, sizeof (fs_qmi_client_crit_sect));
  fs_os_init_crit_sect (&fs_qmi_client_crit_sect);
}

static void
fs_qmi_client_lock (void)
{
  fs_os_enter_crit_sect (&fs_qmi_client_crit_sect);
}

static void
fs_qmi_client_unlock (void)
{
  fs_os_leave_crit_sect (&fs_qmi_client_crit_sect);
}

/* fs_qmi_client_init
 * Initialization will be done by RCINIT / TMC. */
void
fs_qmi_client_init (void)
{
  /* On simulator, after catching fatal errors, globals will not get
     reinitialized */
  memset (&fs_qmi_client_obj, 0x0, sizeof (fs_qmi_client_obj));
  fs_qmi_client_lock_init ();
}

/* Function to fetch a service instance to connect to and to open a connection
   with the framework. */
static qmi_client_error_type
fs_qmi_client_get_service_info (void)
{
  qmi_client_error_type err, err_notify;
  unsigned int num_entries;
  qmi_client_type notifier = NULL;
  unsigned int num_services = 0;

  /* Set OS params for receiving notification about service changes. */
  fs_qmi_client_obj.os_params.tcb = fs_os_self ();
  fs_qmi_client_obj.os_params.sig = FS_OP_COMPLETE_SIG;

  /* service_object of interest */
  fs_qmi_client_obj.service_object = fs_qmi_get_service_object ();
  if (!fs_qmi_client_obj.service_object)    /* Should not fail */
  {
    FS_ERR_FATAL ("QMI get service obj failed. Check .c and .h", 0, 0, 0);
  }

  /* Initialize a notifier with a service object. When a new service is
     registered supporting the service_obj, the signal specified in os_params
     will be set. Notifiers are used to signal changes to the service object
     interested in. The framework sends the registered signal every time a new
     service instance for the service object comes up or a registered service
     goes down. This is needed because the client should not halt the boot
     process waiting for a service to come up. The framework should notify the
     client about such events instead. */
  err = qmi_client_notifier_init (fs_qmi_client_obj.service_object,
                                  &fs_qmi_client_obj.os_params, &notifier);
  if (err != QMI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] QMI client notifier init failed", err);
    return err;
  }

  /* Max number of services to get information about. */
  num_entries = FS_QMI_EFS_MAX_SERVICES;

  /* get_service_list fills up the num_services field to the number of
     service instances with the service-Id in the service_object.
     Any instance can be used to connect to. In our case, there will be
     only one instance of the service. */
  err = qmi_client_get_service_list (fs_qmi_client_obj.service_object,
                                     fs_qmi_client_obj.service_info,
                                     &num_entries,
                                     &num_services);
  /* get_service_list returns QMI_SERVICE_ERR until num_services is 1 .ie.
     at least one service is up and registered with the framework. */
  if (err == QMI_SERVICE_ERR)
  {
    /* Wait for server to come up. */
    QMI_CCI_OS_SIGNAL_WAIT (&fs_qmi_client_obj.os_params, 0);
    num_entries = FS_QMI_EFS_MAX_SERVICES;
    num_services = 0;
    err = qmi_client_get_service_list (fs_qmi_client_obj.service_object,
                                       fs_qmi_client_obj.service_info,
                                       &num_entries,
                                       &num_services);
    if (err != QMI_NO_ERR)
    {
      FS_MSG_ERROR_1 ("[%d] QMI client get service list failed", err);
      return err;
    }
  }
  else if (err != QMI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] QMI client get service list failed", err);
    return err;
  }

  FS_MSG_HIGH_0 ("fs_qmi_client : Service instance registered");
  /* If we are here, then the service is up. Check for param sanity. */
  if (num_entries != FS_QMI_EFS_MAX_SERVICES)
  {
    FS_ERR_FATAL ("[%d]Incorrect num entries in service list\n", num_entries,
                  0, 0);
  }
  if (num_services != FS_QMI_EFS_MAX_SERVICES)
  {
    FS_ERR_FATAL ("[%d]Incorrect num services in service list\n", num_services,
                  0, 0);
  }
  if (fs_qmi_client_obj.service_info[0].info[0] == 0)
  {
    FS_ERR_FATAL ("[%d]Incorrect service info in service list\n",
                   fs_qmi_client_obj.service_info[0].info[0], 0, 0);
  }

  /* Found the service, release the notifier object. Don't need it anymore. */
  err_notify = qmi_client_release (notifier);
  if (err_notify != QMI_NO_ERR)
  {
    /* Lets try to continue running ignoring this error. */
    FS_MSG_ERROR_1 ("[%d] QMI release on notifier failed", err_notify);
  }

  fs_qmi_client_obj.client_inited = 1;

  return err;
}


/* fs_qmi_client_open
 * Opens a new connection with the service found from get_service_list. */
static qmi_client_type
fs_qmi_client_open (void)
{
  qmi_client_error_type err;
  qmi_client_type client_handle = NULL;

  /* Use new client's tcb. */
  memset (&fs_qmi_client_obj.os_params, 0x0, sizeof (qmi_client_os_params));
  fs_qmi_client_obj.os_params.tcb = fs_os_self ();
  fs_qmi_client_obj.os_params.sig = FS_OP_COMPLETE_SIG;


  /* Get client handle from framework for the specified service instance.
     This does not send connect request to service. The actual connect
     request goes when client sends the first request message. */
  err = qmi_client_init (&fs_qmi_client_obj.service_info[0],
                         fs_qmi_client_obj.service_object,
                         NULL, NULL, &fs_qmi_client_obj.os_params,
                         &client_handle);

  /* Error indicates service has gone down, which we do not how to handle. */
  if (err != QMI_NO_ERR)
  {
    client_handle = NULL;
    FS_MSG_ERROR_1 ("[%d] QMI client init failed", err);
  }

  return client_handle;
}


/* fs_qmi_client_close
 * Release the connection with the service. */
static void
fs_qmi_client_close (qmi_client_type client_handle)
{
  qmi_client_error_type err;

  err = qmi_client_release (client_handle);
  if (err != QMI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] QMI client release failed", err);
  }
}


/* efs_put
 * Implements the API to remote the efs_put call to the modem over QMI. */
#ifdef FS_UNIT_TEST
int
efs_qmi_put (const char *path, void *data, fs_size_t length, int oflag,
             int mode)
#else
int
efs_put (const char *path, void *data, fs_size_t length, int oflag, int mode)
#endif
{
  fs_qmi_efs_put_req_msg *req_msg;
  fs_qmi_efs_put_resp_msg *resp_msg;
  qmi_client_error_type err;
  qmi_client_type client_handle;
  uint32 path_len;
  int result = -1;

  if (!path || !data)
  {
    efs_errno = EINVAL;
    return result;
  }

  path_len = strlen (path);
  if(path_len == 0)
  {
    efs_errno = EINVAL;
    return result;
  }

  /* Length including terminating NULL should not be greater than allowed
     length. */
  if (path_len + 1 > FS_QMI_MAX_FILE_PATH)
  {
    efs_errno = ENAMETOOLONG;
    return result;
  }

  if (length > FS_QMI_MAX_ITEM_SIZE)
  {
    efs_errno = FS_ERANGE;
    return result;
  }

  fs_qmi_client_lock ();

  /* Check for service instance providing the services we are interested in */
  if (!fs_qmi_client_obj.client_inited)
  {
    err = fs_qmi_client_get_service_info ();
    if (err != QMI_NO_ERR)
    {
      result = -1;
      efs_errno = ENODEV;
      goto cleanup_before_open;
    }
  }

  /* Open new connection with framework. If client_handle is NULL, the service
     has gone down. */
  client_handle = fs_qmi_client_open ();
  if (!client_handle)
  {
    result = -1;
    efs_errno = ENODEV;
    goto cleanup_before_open;
  }

  req_msg = &fs_qmi_client_obj.efs_put_request;
  resp_msg = &fs_qmi_client_obj.efs_put_response;

  /* Clear old messages. */
  memset (req_msg, 0, sizeof (fs_qmi_efs_put_req_msg));
  memset (resp_msg, 0, sizeof (fs_qmi_efs_put_resp_msg));

  /* Create the request message. */
  req_msg->path_len = path_len + 1;
  req_msg->data_len = length;

  strlcpy (req_msg->path, path, sizeof (req_msg->path));
  fs_memscpy (req_msg->data, req_msg->data_len, data, req_msg->data_len);
  req_msg->oflag = fs_qmi_get_qmi_oflag (oflag);
  req_msg->mode = mode;

  /* The resulting oflag should be non-zero. */
  ASSERT (req_msg->oflag != 0);

  fs_qmi_client_obj.last_req_sent_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH_1 ("[%ul] fs_qmi_client : request message sent",
             (uint32) fs_qmi_client_obj.last_req_sent_time_ms);

  /* Send request message. */
  err = qmi_client_send_msg_sync (client_handle, FS_QMI_EFS_PUT_REQ,
                                  req_msg, sizeof (fs_qmi_efs_put_req_msg),
                                  resp_msg, sizeof (fs_qmi_efs_put_resp_msg),
                                  (unsigned int) FS_QMI_EFS_MSG_TIMEOUT_MSEC);

  fs_qmi_client_obj.last_resp_received_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH_2 ("[%ul] [%d] fs_qmi_client : response message received",
             (uint32) fs_qmi_client_obj.last_resp_received_time_ms, err);

  if (err != QMI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] QMI client send msg sync failed", err);
    result = -1;
    efs_errno = ENODEV;
    goto cleanup;
  }

  /* Check for service framework errors. */
  if (resp_msg->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    FS_MSG_ERROR_2 ("[%d] [%d] QMI service framework error. Check client",
             resp_msg->resp.result, resp_msg->resp.error);
    result = -1;
    efs_errno = EILSEQ; /* To indicate something wrong in QMI pipe. */
    goto cleanup;
  }

  /* qmi response is a SUCCESS; check for API errors. */
  if (resp_msg->efs_err_num_valid == TRUE)
  {
    efs_errno = fs_qmi_get_efs_errno (resp_msg->efs_err_num);
  }
  else     /* err_num_valid is set to FALSE, so the API was successful. */
  {
    result = 0;
  }

cleanup:
  fs_qmi_client_close (client_handle);
cleanup_before_open:
  fs_qmi_client_unlock ();
  return result;
}


/* efs_get
 * Implements the API to remote the efs_get call to the modem. */
#ifdef FS_UNIT_TEST
int
efs_qmi_get (const char *path, void *data, fs_size_t length)
#else
int
efs_get (const char *path, void *data, fs_size_t length)
#endif
{
  fs_qmi_efs_get_req_msg *req_msg;
  fs_qmi_efs_get_resp_msg *resp_msg;
  qmi_client_error_type err;
  qmi_client_type client_handle;
  uint32 path_len;
  int result = -1;

  if (!path || !data)
  {
    efs_errno = EINVAL;
    return result;
  }

  path_len = strlen(path);
  if (path_len == 0)
  {
    efs_errno = EINVAL;
    return result;
  }

  /* Length including terminating NULL should not be greater than allowed
     length. */
  if (path_len + 1 > FS_QMI_MAX_FILE_PATH)
  {
    efs_errno = ENAMETOOLONG;
    return result;
  }

  if (length > FS_QMI_MAX_ITEM_SIZE)
  {
    efs_errno = FS_ERANGE;
    return result;
  }

  fs_qmi_client_lock ();

  /* Initialize client on the first call. */
  if (!fs_qmi_client_obj.client_inited)
  {
    err = fs_qmi_client_get_service_info ();
    if (err != QMI_NO_ERR)
    {
      result = -1;
      efs_errno = ENODEV;
      goto cleanup_before_open;
    }
  }

  /* Open new connection with framework. */
  client_handle = fs_qmi_client_open ();
  if (!client_handle)
  {
    result = -1;
    efs_errno = ENODEV;
    goto cleanup_before_open;
  }

  req_msg = &fs_qmi_client_obj.efs_get_request;
  resp_msg = &fs_qmi_client_obj.efs_get_response;

  /* Clear old messages. */
  memset (req_msg, 0, sizeof (fs_qmi_efs_get_req_msg));
  memset (resp_msg, 0, sizeof (fs_qmi_efs_get_resp_msg));

  /* create request message */
  req_msg->path_len = path_len + 1;
  req_msg->data_length_valid = TRUE;
  req_msg->data_length = length;
  strlcpy (req_msg->path, path, sizeof (req_msg->path));

  fs_qmi_client_obj.last_req_sent_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH_1 ("[%ul] fs_qmi_client : request message sent",
             (uint32) fs_qmi_client_obj.last_req_sent_time_ms);

  /* Send request message. */
  err = qmi_client_send_msg_sync (client_handle, FS_QMI_EFS_GET_REQ,
                                  req_msg, sizeof (fs_qmi_efs_get_req_msg),
                                  resp_msg, sizeof (fs_qmi_efs_get_resp_msg),
                                  (unsigned int) FS_QMI_EFS_MSG_TIMEOUT_MSEC);

  fs_qmi_client_obj.last_resp_received_time_ms = fs_time_ms_u64 ();
  FS_MSG_HIGH_2 ("[%ul] [%d] fs_qmi_client : response message received",
             (uint32) fs_qmi_client_obj.last_resp_received_time_ms, err);

  if (err != QMI_NO_ERR)
  {
    FS_MSG_ERROR_1 ("[%d] QMI client send msg sync failed", err);
    result = -1;
    efs_errno = ENODEV;
    goto cleanup;
  }

  /* Check for service framework errors. */
  if (resp_msg->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    FS_MSG_ERROR_2 ("[%d] [%d] QMI service framework error. Check client",
             resp_msg->resp.result, resp_msg->resp.error);
    result = -1;
    efs_errno = EILSEQ; /* To indicate something wrong in QMI pipe. */
    goto cleanup;
  }

  /* qmi response is a SUCCESS; check for API errors. */
  if (resp_msg->efs_err_num_valid == TRUE)
  {
    efs_errno = fs_qmi_get_efs_errno (resp_msg->efs_err_num);
  }
  else
  {
    /* QMI sync send was a success. Make sure optional fields are filled. */
    if (resp_msg->data_valid == FALSE)
    {
      FS_ERR_FATAL ("err_num & data are both invalid", 0, 0, 0);
    }
    if (resp_msg->data_len != length)
    {
      FS_ERR_FATAL ("[%d], [%d] data len != length", resp_msg->data_len,
                     length, 0);
    }

    fs_memscpy (data, length, resp_msg->data, length);
    result = length;
  }

cleanup:
  fs_qmi_client_close (client_handle);
cleanup_before_open:
  fs_qmi_client_unlock ();
  return result;
}

#endif /* FEATURE_EFS_QMI_PUT_GET_CLIENT */
