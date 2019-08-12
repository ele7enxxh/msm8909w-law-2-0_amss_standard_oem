/******************************************************************************
  @file    qmi_sap_client.c
  @brief   The QMI Restricted Access Proxy (SAP) common header file

  DESCRIPTION
  QMI Restricted Access Proxy routines.  

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "comdef.h"
#include "stdint.h"
#include <string.h>
#include "rex.h"
#include "msg.h"
#include "err.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_sap.h"
#include "qmi_sap_target.h"
#include "qmi_sap_target_ext.h"
#include "qmi_sap_common.h"
#include "service_access_proxy_v01.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "qmi_csi_target.h"
#include "qmi_client.h"
#include "qmi_common.h"
#include "rcinit.h"

#define QMI_SVC_WAIT_SIG 0x00010000
#define QMI_SAP_SERVICE_STACK_SIZE  (2048)
rex_tcb_type        qmi_sap_service_tcb;
unsigned char       qmi_sap_service_stack[QMI_SAP_SERVICE_STACK_SIZE];
static int32_t inited = 0;

/* This struct can hold more information 
   about the client that is connecting to the service
   if there is any reason to keep state information
   about clients */
typedef struct {
  qmi_client_handle clnt;
}client_info_type;

/* This struct is used to hold context of the server.  Cleanup is used 
   to signal the reader thread to clean up and die, num_requests just tracks
   how many requests the service has handled, and service_handle is passed
   into the qmi_csi_handle_event function
   An instance of this structure is passed to the qmi_csi_register function as
   the service_cookie, and this structure can be tailored to a specific services needs */
typedef struct {
  volatile int cleanup;
  qmi_csi_service_handle service_handle;
  int num_requests;
}service_context_type;

static service_context_type service_cookie;
static qmi_csi_os_params os_params;

void qmi_sap_service_init( void );
void qmi_sap_service_start( void );

static qmi_csi_cb_error qsap_register_service
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_idl_service_object_type svc_obj;
  qmi_sap_client_handle handle;
  sap_register_service_req_msg_v01 *req = (sap_register_service_req_msg_v01*)req_c_struct;
  sap_register_service_resp_msg_v01 resp;
  qmi_sap_error rc = QMI_SAP_NO_ERR;
  svc_obj = QSAP_CALLOC(sizeof(struct qmi_idl_service_object),1);
  if (!svc_obj)
  {
      return QMI_CSI_CB_NO_ERR;
  }
  memset(&resp,0,sizeof(sap_register_service_resp_msg_v01));

  svc_obj->service_id = req->service_obj.service_id;
  svc_obj->idl_version = req->service_obj.major_vers;
  svc_obj->max_msg_len = req->service_obj.max_msg_len;
  svc_obj->library_version = QMI_IDL_LIB_ENCDEC_VERSION;

  rc = qmi_sap_register_internal(svc_obj,NULL,&handle);
  if (rc != QMI_SAP_NO_ERR)
  {
    QSAP_FREE(svc_obj);
    if (rc != QMI_SAP_SVC_REGISTERED)
    {
      return QMI_CSI_CB_INTERNAL_ERR;
    }
  }

  qmi_csi_send_resp(req_handle,msg_id,&resp,sizeof(sap_register_service_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error qsap_deregister_service
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_cookie
  )
{
  sap_deregister_service_resp_msg_v01 resp;
  memset(&resp,0,sizeof(sap_deregister_service_resp_msg_v01));
  qmi_csi_send_resp(req_handle,msg_id,&resp,sizeof(sap_deregister_service_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/* Define a jump table to handle the dispatch of request handler functions. */
static qmi_csi_cb_error (* const req_handle_table[])
(
 client_info_type         *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                     *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_cookie
) = 
{
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x00 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x18 - 0x1F*/
  &qsap_register_service,                  /* Request handler for message ID 0x20 */
  &qsap_deregister_service                 /* Request handler for message ID 0x21 */
};

static qmi_csi_cb_error qsap_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
  )
{
  client_info_type *clnt_info;
  /* For any service where it is anticipated that there could be more than one client
         connected at any given time the clients should be stored in some kind of data structure
         to facilitate cleanup and the sending of indications. */
  clnt_info = QSAP_MALLOC(sizeof(client_info_type)); /* Freed in ping_disconnect_cb */
  if(!clnt_info)
    return QMI_CSI_CB_CONN_REFUSED;
  clnt_info->clnt = client_handle;
  *connection_handle = clnt_info;
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error qsap_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  client_info_type *clnt_info = (client_info_type*)connection_handle;
  if(!clnt_info)
  {
    return rc;
  }
  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    /* If the jump table has a valid entry at this message ID, call the handler function */
    if(req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id] (clnt_info,req_handle,msg_id,req_c_struct,req_c_struct_len,service_cookie);
    }else{
      //printf("No function defined to handle request for message ID: %d\n",msg_id);
    }
  }else{
    //printf("Message ID: %d greater than maximum known message ID.\n",msg_id);
  }

  return rc;
}

static void qsap_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_cookie
 )
{  
  qmi_sap_client_handle user_handle = NULL;

  /* TODO Add code to handle handles logically */
  qmi_sap_deregister(user_handle);
  /* Free up memory for the client */
  if(connection_handle)
    QSAP_FREE(connection_handle); /* Malloc in ping_connect_cb */
  
  return;
}
static void qmi_sap_service_thread(uint32 handle)
{
  int rc;
  //void *sp;

  os_params.tcb = rex_self();
  os_params.sig = QMI_SVC_WAIT_SIG;
  if(!inited)
  {
    rex_wait(QMI_SVC_WAIT_SIG);
    rex_clr_sigs(os_params.tcb, QMI_SVC_WAIT_SIG);
  }
  rc = qmi_csi_register(sap_get_service_object_v01(),qsap_connect_cb,
                        qsap_disconnect_cb, qsap_handle_req_cb, &service_cookie,
                        &os_params, &service_cookie.service_handle);
  
  if(rc != QMI_NO_ERR)
  {
    ERR_FATAL("Unable to start qsap service!", 0,0,0);
  }

  while(1)
  {
    rex_wait(QMI_SVC_WAIT_SIG);
    rex_clr_sigs(os_params.tcb, QMI_SVC_WAIT_SIG);
    qmi_csi_handle_event(service_cookie.service_handle, &os_params);
  }
}

void qmi_sap_service_init()
{
  if (!inited && os_params.tcb)
  {
    inited = 1;
    QMI_CSI_OS_SIGNAL_SET(&os_params);
  }
}

/*=============================================================================
  FUNCTION qmi_sap_start_service
=============================================================================*/
/*!
@brief
  This function starts 

@param[in]   service_obj        Pointer to the service object of the service being 
                                registered with QSAP 
@param[out]  user_handle        Pointer to a handle the client can use to deregister 
                                from QSAP 

@retval      QMI_SAP_NO_ERR     Success
*/
/*=========================================================================*/
void qmi_sap_service_start()
{
  unsigned int prio = 10; 
  #ifdef QMI_RCINIT_CENTRALIZED_THREAD_PRIORITY
    RCINIT_INFO info_handle = NULL;

    info_handle = rcinit_lookup("QMI_SAP_SVC");
    if (!info_handle) 
    {
      ERR_FATAL("QMI_SAP_SVC task info not found",0 , 0, 0);
    }
    else
    {
      prio = rcinit_lookup_prio_info(info_handle);
      if (prio > 255) 
      {
        ERR_FATAL("Invalid Priority:%d", prio, 0, 0);
      }
    }
  #endif

  rex_def_task_ext( &qmi_sap_service_tcb,
      qmi_sap_service_stack,
      QMI_SAP_SERVICE_STACK_SIZE,
      prio,
      qmi_sap_service_thread,
      0,
      "QMI_SAP_SVC",
      0 );
}

