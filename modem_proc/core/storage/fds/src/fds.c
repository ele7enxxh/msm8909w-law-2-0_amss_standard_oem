/***********************************************************************
 * fds.c
 *
 * Implementation of the Flash Driver Service.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Implementation of the Service module that processes requests for the Flash
 * Driver Service.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-08-07   mj    Chech return status from qmi_sap_register.
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-12-12   rp    Allow disabling SCRUB feature at runtime.
2013-10-29   dks   Fix compiler issues.
2013-10-04   dks   Handle Fota Update notification from apps.
2013-10-03   dks   Add commands for Fota Update and Event registration.
2013-08-01   rp    Scrub-Master module cleanup.
2013-07-22   wek   Add a separate sequence number for QMI scrubbing.
2013-07-17   dks   Create.

===========================================================================*/

#include "qmi_csi.h"
#include "qmi_sap.h"
#include "assert.h"
#include "rex.h"
#include "time_svc.h"
#include "fds.h"
#include "fds_i.h"
#include "fds_config_i.h"
#include "fds_err.h"
#include "fds_util.h"
#include "flash_scrub.h"

/* Scrub related Response structures too big to fit on stack */

typedef struct
{
  /* Response structures too big to fit on stack */
  fds_scrub_start_resp_msg                 scrub_start_resp_msg;
  fds_scrub_get_status_resp_msg            scrub_get_status_resp_msg;
  fds_scrub_get_sys_info_resp_msg          scrub_get_sys_info_resp_msg;
  fds_handle_fota_update_resp_msg          scrub_fota_update_resp_msg;
  fds_indication_register_resp_msg         scrub_register_for_events_resp_msg;
  fds_scrub_apps_start_scrub_ind_msg       scrub_apps_scrub_start_ind_msg;
} fds_scrub_object_type;

enum fds_events_index
{
  FDS_APPS_SCRUB_START_EVENT = 0
};

typedef struct
{
  qmi_client_handle cli_handle;
  uint8 event_notify_settings[FDS_MAX_EVENTS_SUPPORTED];
  uint8 in_use;
} fds_client_info_obj_type;

typedef struct
{
  /* Service handle provided by framework at register time */
  qmi_csi_service_handle service_handle;

  /* qsap handle provided by framework at qsap_register time */
  qmi_sap_client_handle qsap_handle;

  /* tcb and signalling information that framework uses to invoke service
     handlers */
  qmi_csi_os_params os_params;

  /* State variables meant to help debugging issues involving timeout or
     failed process requests. */
  uint64 last_req_received_time_ms;  /* Time at which request was received */
  uint64 last_req_processed_time_ms; /* Time at which request was done
                                        procossing. Response can be a failure
                                        or success. */
  /* Scrub related members */
  fds_scrub_object_type fds_scrub_inst;

  /* Client contexts and count of contexts stored. */
  fds_client_info_obj_type fds_client_inst[FDS_MAX_CLIENTS_SUPPORTED];
  uint32 num_clients_connected;
} fds_object_type;
static fds_object_type fds_inst;

static qmi_csi_cb_error fds_scrub_start (qmi_req_handle req_handle,
                                         unsigned int msg_id, void *req_struct,
                                         unsigned int req_struct_len);

static qmi_csi_cb_error fds_scrub_get_status (qmi_req_handle req_handle,
                                              unsigned int msg_id,
                                              void *req_struct,
                                              unsigned int req_struct_len);

static qmi_csi_cb_error fds_scrub_get_sys_info (qmi_req_handle req_handle,
                                                unsigned int msg_id,
                                                void *req_struct,
                                                unsigned int req_struct_len);

static qmi_csi_cb_error fds_scrub_handle_fota_update (
                                                qmi_req_handle req_handle,
                                                unsigned int msg_id,
                                                void *req_struct,
                                                unsigned int req_struct_len);


static qmi_csi_cb_error fds_scrub_indication_register (
                                                fds_client_info_obj_type *cli,
                                                qmi_req_handle req_handle,
                                                unsigned int msg_id,
                                                void *req_struct,
                                                unsigned int req_struct_len);


/* Critical section and APIs for static data protection. */
static rex_crit_sect_type fds_crit_sect;

void
fds_lock_init (void)
{
  memset (&fds_crit_sect, 0x0, sizeof (rex_crit_sect_type));
  rex_init_crit_sect (&fds_crit_sect);
}

void
fds_lock (void)
{
  rex_enter_crit_sect (&fds_crit_sect);
}

void
fds_unlock (void)
{
  rex_leave_crit_sect (&fds_crit_sect);
}

/* Check if client object is in valid address range and in use. Returns 1 if
   valid and 0 if not */
static int
fsd_is_client_inst_valid (fds_client_info_obj_type *cli_inst)
{
  fds_client_info_obj_type *start_address, *end_address;
  int result = 1;

  ASSERT (cli_inst != NULL);
  if (cli_inst == NULL)
  {
    FDS_MSG_ERR_0 ("FDS: NULL Client Context supplied");
    result = 0;
  }

  start_address = &(fds_inst.fds_client_inst[0]);
  end_address = &(fds_inst.fds_client_inst[FDS_MAX_CLIENTS_SUPPORTED]);

  ASSERT ((cli_inst >= start_address) && (cli_inst <= end_address));
  if ((cli_inst < start_address) || (cli_inst > end_address))
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS: Invalid context supplied [cli_inst,start,end]",
                 (int)cli_inst, (int)start_address, (int)end_address);
    result = 0;
  }

  ASSERT (cli_inst->in_use == 1);
  if (cli_inst->in_use != 1)
  {
    FDS_MSG_ERR_1 ("%d,FDS: Context not in use [cli_inst]", (int)cli_inst);
    result = 0;
  }

  return result;
}

/* Callback functions that service registers with framework */

/* Handles client connection. Is invoked when the client sends it first
   request message. */
qmi_csi_cb_error
fds_connect_cb (qmi_client_handle cli_handle, void *service_cookie,
                    void **connection_handle)
{
  fds_client_info_obj_type *cli_inst;
  uint32 i;

  (void) service_cookie;
  ASSERT (connection_handle != NULL);
  ASSERT (cli_handle != NULL);

  if (connection_handle == NULL || cli_handle == NULL)
  {
    FDS_MSG_ERR_2 ("%d,%d, FDS:Con failed,invalid handle[con_hdl,cli_hdl]",
                  (int)connection_handle, (int)cli_handle);
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if (fds_inst.num_clients_connected == FDS_MAX_CLIENTS_SUPPORTED)
  {
    FDS_MSG_ERR_3 ("%d,%d,%d,FDS:Con failed, no mem[con_hdl,cli_hdl,cli_cnt]",
                  (int)connection_handle, (int)cli_handle,
                  fds_inst.num_clients_connected);
    return QMI_CSI_CB_NO_MEM;
  }

  for (i = 0; i < FDS_MAX_CLIENTS_SUPPORTED; i++)
  {
    cli_inst = &(fds_inst.fds_client_inst[i]);
    if (cli_inst->in_use == 0)
    {
      break;
    }
  }

  ASSERT (i < FDS_MAX_CLIENTS_SUPPORTED);
  memset (cli_inst, 0x0, sizeof (fds_client_info_obj_type));
  cli_inst->in_use = 1;
  cli_inst->cli_handle = cli_handle;
  fds_inst.num_clients_connected++;

  *connection_handle = (void *) cli_inst;

  FDS_MSG_HIGH_3 ("%d,%d,%d,FDS:Connect request [con_hdl,cli_hdl,cli_cnt]",
                (int)connection_handle, (int)cli_handle,
                fds_inst.num_clients_connected);

  return QMI_CSI_CB_NO_ERR;
}


/* Handles client disconnection. Is invoked when client calls
   qmi_client_release API. */
void
fds_disconnect_cb (void *connection_handle, void *service_cookie)
{
  fds_client_info_obj_type *cli_inst;

  (void) service_cookie;

  cli_inst = (fds_client_info_obj_type *) connection_handle;
  if (fsd_is_client_inst_valid (cli_inst) == 0)
  {
    FDS_MSG_ERR_1 ("%d, FDS:Disconnect Req failed, invalid context[cli_inst]",
                 (int)cli_inst);
    return;
  }

  memset (cli_inst, 0x0, sizeof (fds_client_info_obj_type));
  fds_inst.num_clients_connected--;

  FDS_MSG_HIGH_2 ("%d, %d, FDS: Disconnect Request [con_hdl, serv_cki]",
               (int)connection_handle, (int)service_cookie);
}

/* Handles process requests from client. Invoked when clients send any
   request messages */
qmi_csi_cb_error
fds_process_req_cb (void *connection_handle, qmi_req_handle req_handle,
                    unsigned int msg_id, void *req_struct,
                    unsigned int req_struct_len, void *service_cookie)
{
  fds_client_info_obj_type *cli_inst;
  qmi_csi_cb_error err = QMI_CSI_CB_INTERNAL_ERR;

  (void) service_cookie;

  cli_inst = (fds_client_info_obj_type *) connection_handle;
  if (fsd_is_client_inst_valid (cli_inst) == 0)
  {
    FDS_MSG_ERR_1 ("%d, FDS:Process Req failed, invalid context[cli_inst]",
                 (int)cli_inst);
    return err;
  }

  fds_inst.last_req_received_time_ms = time_get_ms_native ();
  FDS_MSG_HIGH_2 ("%d, %d, FDS : Service request received [time_ms,msg_id]",
               (uint32)fds_inst.last_req_received_time_ms, msg_id);

  /* Input validation */
  if ((req_handle == NULL) || (req_struct == NULL))
  {
    FDS_MSG_ERR_3(
               "%d,%d,%d,FDS:Incorrect handles received[req_hdl,req_str,len]",
               (int)req_handle, (int)req_struct, req_struct_len);
    goto END;
  }

  switch (msg_id)
  {
    case FDS_SCRUB_START_REQ:
      err = fds_scrub_start (req_handle, msg_id, req_struct,
                             req_struct_len);
    break;

    case FDS_SCRUB_GET_STATUS_REQ:
      err = fds_scrub_get_status (req_handle, msg_id, req_struct,
                                  req_struct_len);
    break;

    case FDS_SCRUB_GET_SYS_INFO_REQ:
      err = fds_scrub_get_sys_info (req_handle, msg_id, req_struct,
                                    req_struct_len);
    break;

    case FDS_HANDLE_FOTA_UPDATE_REQ:
      err = fds_scrub_handle_fota_update (req_handle, msg_id, req_struct,
                                          req_struct_len);
    break;

    case FDS_INDICATION_REGISTER_REQ:
      err = fds_scrub_indication_register (connection_handle, req_handle,
                                           msg_id, req_struct, req_struct_len);
    break;

    default:
      FDS_MSG_ERR_1 ("%d, FDS: Unknown Msg ID received [msg_id]", msg_id);
    break;
  }

END:
  fds_inst.last_req_processed_time_ms = time_get_ms_native ();
  FDS_MSG_HIGH_3 ("%d,%d,%d, fds : Request processed [err, msg_id, time_ms]",
                 err, msg_id, (uint32)fds_inst.last_req_processed_time_ms);

  return err;
}

/* Initialize the service module. */
void
fds_init (void)
{
  qmi_csi_error err;
  qmi_sap_error sap_err;
  qmi_idl_service_object_type service_obj;

  fds_lock ();

  memset (&fds_inst, 0x0, sizeof (fds_object_type));

  service_obj = fds_get_service_object ();
  if (service_obj == NULL)
  {
    FDS_ERR_FATAL ("FDS: get_service_object failed. Check .c and .h",
                   0, 0, 0);
  }

  fds_inst.os_params.tcb = rex_self ();
  fds_inst.os_params.sig = FDS_HANDLE_EVENT;

  /* Registers function for handling QMI Message Query Requests */
  (void) qmi_si_register_object(service_obj,
                                0, /* Service Instance */
                                fds_get_service_impl ());

  err = qmi_csi_register (service_obj,
                          fds_connect_cb,
                          fds_disconnect_cb,
                          fds_process_req_cb,
                          NULL, /* Service_cookie : Service specific context.*/
                          &fds_inst.os_params,
                          &fds_inst.service_handle);
  if (err != QMI_CSI_NO_ERR)
  {
    FDS_ERR_FATAL ("%d, %d, FDS : qmi_csi_register failed [err, serv_obj]",
                  err, (int)service_obj, 0);
  }

  if (fds_inst.service_handle == NULL)
  {
    FDS_ERR_FATAL ("%d,%d,FDS:qmi_csi_register returned NULL hld[err, hld]",
                  err, (int)service_obj, 0);
  }

  sap_err = qmi_sap_register (service_obj, NULL, &fds_inst.qsap_handle);
  if (sap_err != QMI_SAP_NO_ERR)
  {
    FDS_ERR_FATAL ("%d,%d,%d,FDS : sap_register failed[err,serv_ob,qsap_hdl]",
                  sap_err, (int) service_obj, (int) fds_inst.qsap_handle);
  }
  if (fds_inst.qsap_handle == NULL)
  {
    FDS_ERR_FATAL ("%d,%d,FDS:qmi_sap_register returned NULL hld[err, hld]",
                  sap_err, (int)service_obj, 0);
  }

  FDS_MSG_HIGH_0 ("FDS : Init complete");

  fds_unlock ();
}

/* Call qmi_csi_handle_event to get appropriate callbacks */
void
fds_handle_event (void)
{
  qmi_csi_error err;

  FDS_MSG_HIGH_1 ("%d, FDS: Handle Event Called [time_ms]",
                (uint32)time_get_ms_native());

  /* Serialize requests from clients. No other client's request can be handled
     until this client's request is processed. */
  fds_lock ();

  err = qmi_csi_handle_event (fds_inst.service_handle,
                              &fds_inst.os_params);
  if (err != QMI_CSI_NO_ERR)
  {
    FDS_MSG_ERR_1 ("%d, FDS: Handle event error [err]", err);
  }

  fds_unlock ();
}


static qmi_csi_cb_error
fds_scrub_start (qmi_req_handle req_handle, unsigned int msg_id,
                 void *req_struct, unsigned int req_struct_len)
{
  int result;
  uint32 scrub_token;
  uint32 req_msg_len, resp_msg_len;
  fds_scrub_start_resp_msg *resp_msg;
  qmi_csi_error service_err;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;

  req_msg_len  = sizeof (fds_scrub_start_req_msg);
  resp_msg_len = sizeof (fds_scrub_start_resp_msg);

  resp_msg = &(fds_inst.fds_scrub_inst.scrub_start_resp_msg);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = FDS_RESULT_FAILURE;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:Malformed Req Msg received[msg_id,len,exp_len]",
                 msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = FDS_ERR_MALFORMED_MSG;
    goto END;
  }

  /* Message structure is good. Fill in default service error codes. */
  resp_msg->resp.error  = FDS_ERR_INTERNAL;

  result = flash_scrub_start_scrub_request_from_qmi (&scrub_token);
  if (result != 0)
  {
    FDS_MSG_ERR_1 ("%d, FDS: Scrub start failed [result]", result);
    resp_msg->resp.error = fds_map_scrub_error_to_fds_error (result);
    goto END;
  }

  /* Fix response for success. */
  resp_msg->resp.result = FDS_RESULT_SUCCESS;
  resp_msg->resp.error  = FDS_ERR_NONE;
  resp_msg->scrub_token_valid = 1;
  resp_msg->scrub_token = scrub_token;

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:qmi_csi_send_resp failed[err,msg_id,resp_msg]",
                 service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;
}

static qmi_csi_cb_error
fds_scrub_get_status (qmi_req_handle req_handle, unsigned int msg_id,
                      void *req_struct, unsigned int req_struct_len)
{
  int result;
  enum flash_scrub_status status;
  uint32 req_msg_len, resp_msg_len;
  fds_scrub_get_status_req_msg *req_msg;
  fds_scrub_get_status_resp_msg *resp_msg;
  qmi_csi_error service_err;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;

  req_msg_len  = sizeof (fds_scrub_get_status_req_msg);
  resp_msg_len = sizeof (fds_scrub_get_status_resp_msg);

  req_msg = (fds_scrub_get_status_req_msg *) req_struct;
  resp_msg = &(fds_inst.fds_scrub_inst.scrub_get_status_resp_msg);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = FDS_RESULT_FAILURE;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:Malformed Req Msg received[msg_id,len,exp_len]",
                 msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = FDS_ERR_MALFORMED_MSG;
    goto END;
  }

  /* Message structure is good. Fill in default service error codes. */
  resp_msg->resp.error  = FDS_ERR_INTERNAL;

  result = flash_scrub_get_scrub_status_from_qmi (req_msg->scrub_token,
                                                  &status);
  if (result != 0)
  {
    FDS_MSG_ERR_1 ("%d, FDS: Scrub get_status failed [result]", result);
    resp_msg->resp.error = fds_map_scrub_error_to_fds_error (result);
    goto END;
  }

  /* Fix response for success. */
  resp_msg->resp.result = FDS_RESULT_SUCCESS;
  resp_msg->resp.error  = FDS_ERR_NONE;
  resp_msg->scrub_status_valid = 1;
  resp_msg->scrub_status = fds_map_scrub_status_to_fds_scrub_status (status);

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:qmi_csi_send_resp failed[err,msg_id,resp_msg]",
                 service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;
}


static qmi_csi_cb_error
fds_scrub_get_sys_info (qmi_req_handle req_handle, unsigned int msg_id,
                        void *req_struct, unsigned int req_struct_len)
{
  int result;
  uint64 last_scrub_time_secs;
  uint64 min_time_between_secs;
  uint64 internal_timer_secs;
  uint32 req_msg_len, resp_msg_len;
  fds_scrub_get_sys_info_resp_msg *resp_msg;
  qmi_csi_error service_err;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;

  req_msg_len  = sizeof (fds_scrub_get_sys_info_req_msg);
  resp_msg_len = sizeof (fds_scrub_get_sys_info_resp_msg);

  resp_msg = &(fds_inst.fds_scrub_inst.scrub_get_sys_info_resp_msg);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = FDS_RESULT_FAILURE;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:Malformed Req Msg received[msg_id,len,exp_len]",
                 msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = FDS_ERR_MALFORMED_MSG;
    goto END;
  }

  /* Message structure is good. Fill in default service error codes. */
  resp_msg->resp.error  = FDS_ERR_INTERNAL;

  result = flash_scrub_get_time_status_from_qmi (&last_scrub_time_secs,
                                                 &min_time_between_secs,
                                                 &internal_timer_secs);
  if (result != 0)
  {
    FDS_MSG_ERR_1 ("%d, FDS: Scrub get sys info failed [result]", result);
    resp_msg->resp.error = fds_map_scrub_error_to_fds_error (result);
    goto END;
  }

  /* Fix response for success. */
  resp_msg->resp.result = FDS_RESULT_SUCCESS;
  resp_msg->resp.error  = FDS_ERR_NONE;
  resp_msg->fds_internal_scrub_timer_secs_valid = 1;
  resp_msg->fds_internal_scrub_timer_secs = internal_timer_secs;
  resp_msg->fds_minimum_secs_between_scrub_valid = 1;
  resp_msg->fds_minimum_secs_between_scrub = min_time_between_secs;
  resp_msg->fds_previous_scrub_time_posix_valid = 1;
  resp_msg->fds_previous_scrub_time_posix = last_scrub_time_secs;

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:qmi_csi_send_resp failed[err,msg_id,resp_msg]",
                 service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;
}

static qmi_csi_cb_error
fds_scrub_handle_fota_update (qmi_req_handle req_handle, unsigned int msg_id,
                              void *req_struct,unsigned int req_struct_len)
{
  uint32 req_msg_len, resp_msg_len;
  fds_handle_fota_update_resp_msg *resp_msg;
  qmi_csi_error service_err;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;

  req_msg_len  = sizeof (fds_handle_fota_update_req_msg);
  resp_msg_len = sizeof (fds_handle_fota_update_resp_msg);

  resp_msg = &(fds_inst.fds_scrub_inst.scrub_fota_update_resp_msg);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = FDS_RESULT_FAILURE;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:Malformed Req Msg received[msg_id,len,exp_len]",
                 msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = FDS_ERR_MALFORMED_MSG;
    goto END;
  }

  (void) flash_scrub_notify_fota_update ();

  resp_msg->resp.result = FDS_RESULT_SUCCESS;
  resp_msg->resp.error  = FDS_ERR_NONE;

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:qmi_csi_send_resp failed[err,msg_id,resp_msg]",
                 service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;

}


static qmi_csi_cb_error
fds_scrub_indication_register (fds_client_info_obj_type *cli_inst,
                               qmi_req_handle req_handle,
                               unsigned int msg_id, void *req_struct,
                               unsigned int req_struct_len)
{
  uint32 req_msg_len, resp_msg_len;
  fds_indication_register_req_msg *req_msg;
  fds_indication_register_resp_msg *resp_msg;
  qmi_csi_error service_err;
  qmi_csi_cb_error cb_err = QMI_CSI_CB_INTERNAL_ERR;

  req_msg_len  = sizeof (fds_indication_register_req_msg);
  resp_msg_len = sizeof (fds_indication_register_resp_msg);

  req_msg = (fds_indication_register_req_msg *) req_struct;
  resp_msg = &(fds_inst.fds_scrub_inst.scrub_register_for_events_resp_msg);
  memset (resp_msg, 0, resp_msg_len);

  /* Set default QMI error */
  resp_msg->resp.result = FDS_RESULT_FAILURE;

  /* Check for malformed msg */
  if (req_struct_len != req_msg_len)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:Malformed Req Msg received[msg_id,len,exp_len]",
                 msg_id, req_struct_len, req_msg_len);
    resp_msg->resp.error  = FDS_ERR_MALFORMED_MSG;
    goto END;
  }

  if (req_msg->need_apps_start_scrub_indication_valid)
  {
    cli_inst->event_notify_settings[FDS_APPS_SCRUB_START_EVENT] =
                         req_msg->need_apps_start_scrub_indication;
  }

  resp_msg->resp.result = FDS_RESULT_SUCCESS;
  resp_msg->resp.error  = FDS_ERR_NONE;

END :
  service_err = qmi_csi_send_resp (req_handle, msg_id, resp_msg, resp_msg_len);
  if (service_err != QMI_CSI_NO_ERR)
  {
    FDS_MSG_ERR_3 (
                 "%d,%d,%d,FDS:qmi_csi_send_resp failed[err,msg_id,resp_msg]",
                 service_err, msg_id, (int)resp_msg);
    return cb_err;
  }

  cb_err = QMI_CSI_CB_NO_ERR;
  return cb_err;
}

int
fds_scrub_send_apps_scrub_notification (uint32 scrub_token)
{
  qmi_csi_error service_err;
  fds_scrub_apps_start_scrub_ind_msg *ind_msg;
  uint32 ind_msg_len, i;
  int result = 0;
  fds_client_info_obj_type *cli_inst = NULL;

  ind_msg = &fds_inst.fds_scrub_inst.scrub_apps_scrub_start_ind_msg;
  ind_msg_len = sizeof (fds_scrub_apps_start_scrub_ind_msg);
  memset (ind_msg, 0x0, ind_msg_len);
  ind_msg->scrub_token = scrub_token;

  for (i = 0; i < FDS_MAX_CLIENTS_SUPPORTED; i++)
  {
    cli_inst = &fds_inst.fds_client_inst[i];
    if (cli_inst->in_use == 1)
    {
      if (cli_inst->event_notify_settings[FDS_APPS_SCRUB_START_EVENT] == 1)
      {
        service_err = qmi_csi_send_ind (cli_inst->cli_handle,
                                        FDS_SCRUB_APPS_START_SCRUB_IND,
                                        ind_msg, ind_msg_len);
        if (service_err != QMI_CSI_NO_ERR)
        {
          FDS_MSG_ERR_3 (
                       "%d,%d,%d,FDS:qmi_csi_send_ind err[err,msg_id,pkt]",
                       service_err, FDS_SCRUB_APPS_START_SCRUB_IND,
                       (int)ind_msg);
          result = -1;
        }
        else
        {
          FDS_MSG_HIGH_0 ("FDS: Apps scrub indication sent");
        }
      }
    }
  }

  return result;
}
