/******************************************************************************
  @file    qmi_sap_common.c
  @brief   The QMI Restricted Access Proxy (SAP) common header file

  DESCRIPTION
  QMI Restricted Access Proxy routines.  

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "comdef.h"
#include "msg.h"
#include "stdint.h"
#include "qmi_idl_lib.h"
#include "qmi_sap.h"
#include "qmi_sap_xport.h"
#include "qmi_sap_common.h"
#include "qmi_sap_target.h"
#include "qmi_sap_target_ext.h"
#include "qmi_client.h"
#include "qmi_common.h"
#include "common_v01.h"

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001
#define QMI_CTL_FLAG_REQUEST    0
#define QMI_SAP_MAX_XPORTS 16

typedef struct qmi_sap_registration_s
{
  /* links to prev and next in service list */
  LINK(struct qmi_sap_registration_s, link);
  /* XPORT Info */
  qmi_sap_xport_type *xport;
}qmi_sap_registration_type;

typedef struct qmi_sap_xport_list_s
{
  LINK(struct qmi_sap_xport_list_s, link);
  qmi_sap_xport_type *xport;
}qmi_sap_xport_list;

typedef struct qmi_sap_registered_services_s
{
  LINK(struct qmi_sap_registered_services_s, link);
  uint16_t service_id;
  LIST(qmi_sap_xport_list, xport_list);
}qmi_sap_registered_services;

static qmi_sap_lock_type service_list_lock;
static qmi_sap_lock_type registration_list_lock;
static LIST(qmi_sap_registration_type,registration_list);
static LIST(qmi_sap_registered_services, qsap_services_list);
static uint32_t inited = 0;
static uint32_t pre_inited = 0;
static uint32_t post_inited = 0;
static qmi_client_os_params qsap_os_task;

struct qmi_sap_xport_tbl_s
{
  qmi_sap_xport_ops_type *ops;
  void *xport_data;
};

typedef struct qmi_sap_async_type
{
  qmi_sap_client_type         *client;
  uint16_t                    tx_id;
  qmi_idl_service_object_type service_obj;
}qmi_sap_async_type;

static struct qmi_sap_xport_tbl_s xport_tbl[QMI_SAP_MAX_XPORTS];

static qmi_error_type_v01 qcci_err_conv(int rc)
{
  qmi_error_type_v01 ret = QMI_ERR_INTERNAL_V01;

  switch(rc)
  {
    case QMI_CLIENT_ALLOC_FAILURE:
      ret = QMI_ERR_NO_MEMORY_V01;
      break;
    case QMI_IDL_LIB_BUFFER_TOO_SMALL:
    case QMI_IDL_LIB_ARRAY_TOO_BIG:
    case QMI_IDL_LIB_TLV_DUPLICATED:
    case QMI_IDL_LIB_MISSING_TLV:
    case QMI_IDL_LIB_PARAMETER_ERROR:
    case QMI_IDL_LIB_UNKNOWN_MANDATORY_TLV:
    case QMI_IDL_LIB_RANGE_FAILURE:
      ret = QMI_ERR_ENCODING_V01;
      break;
    case QMI_IDL_LIB_MESSAGE_ID_NOT_FOUND:
      ret = QMI_ERR_INVALID_MESSAGE_ID_V01;
      break;
    default:
      ret = QMI_ERR_INTERNAL_V01;
      break;
  }
  return ret;
}

static qmi_sap_error qmi_sap_send_std_resp
(
  qmi_sap_client_type *client,
  qmi_result_type_v01 result,
  qmi_error_type_v01 error,
  uint16_t msg_id,
  uint16_t txn_id
)
{
  size_t msg_len = qmi_idl_get_std_resp_tlv_len();
  uint8_t *msg = QSAP_MALLOC(msg_len + QMI_HEADER_SIZE);
  if(!msg)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  if(qmi_idl_encode_resp_tlv(
        result,
        error,
        msg + QMI_HEADER_SIZE,
        msg_len) != QMI_IDL_LIB_NO_ERR)
  {
    QSAP_FREE(msg);
    return QMI_SAP_INTERNAL_ERR;
  }
  encode_header(msg, QMI_RESPONSE_CONTROL_FLAG, txn_id, msg_id, msg_len);
  msg_len += QMI_HEADER_SIZE;

  client->xport.xport->ops->send(client->xport.xport->handle, 
                                 client->xport.addr, msg, msg_len, NULL);
  QSAP_FREE(msg);
  return QMI_SAP_NO_ERR;
}


static void qmi_sap_init(void)
{
  inited = 1;
}

/* self-init function. not reentrant */
void qmi_sap_post_init(void)
{
  qmi_sap_registration_type *service_reg;
  qmi_sap_xport_type *xport, *orig_xport; 
  qmi_idl_service_object_type service_obj;
  uint32_t service_id,service_version,max_msg_len;
  qmi_sap_registered_services *svc;
  qmi_sap_xport_list *xport_list;
  int i;
  QSAP_LOCK(&registration_list_lock);
  if(!post_inited)
  {
    post_inited = 1;
  }else
  {
    QSAP_UNLOCK(&registration_list_lock);
    return;
  }
  /* initialize locks */
  /* check if there are any pending registrations */
  while(LIST_CNT(registration_list))
  {
    service_reg = LIST_HEAD(registration_list);
    /* Get the relevant info out of the registration list */
    service_obj = service_reg->xport->service->service_obj;
    orig_xport = service_reg->xport;
    /* Remove the Service info from the list and free the memory */
    LIST_REMOVE(registration_list,service_reg,link);
    QSAP_FREE(service_reg);
    svc = QSAP_CALLOC(1, sizeof(qmi_sap_registered_services));
    if (!svc)
    {
      continue;
    }
    for(i=0;i<QMI_SAP_MAX_XPORTS;i++)
    {
      if(!xport_tbl[i].ops)
      {
        break;
      }      
      /* Register the Service info w/ QMUX */
      if(qmi_idl_get_service_id(service_obj, &service_id) != QMI_NO_ERR ||
         qmi_idl_get_idl_version(service_obj, &service_version) != QMI_NO_ERR ||
         qmi_idl_get_max_service_len(service_obj, &max_msg_len) != QMI_NO_ERR)
      {
        continue;
      }
      xport = QSAP_CALLOC(1,sizeof(qmi_sap_xport_type));
      if(!xport)
      {
        continue;
      }
      QMI_MEM_COPY(xport,sizeof(qmi_sap_xport_type),orig_xport,
                   sizeof(qmi_sap_xport_type));
      xport->ops = xport_tbl[i].ops;
      xport->addr_len = xport->ops->addr_len();
      xport->handle = xport->ops->open(xport_tbl[i].xport_data, xport, max_msg_len);
      if(!xport->handle)
      {
        QSAP_FREE(xport);
        continue;
      }
    
      if(xport->ops->reg(xport->handle, service_id, service_version)
         != QMI_SAP_NO_ERR)
      {
        xport->ops->close(xport->handle);
        QSAP_FREE(xport);
        continue;
      }
      xport_list = QSAP_CALLOC(1, sizeof(qmi_sap_registered_services));
      if (!xport_list)
      {
        continue;
      }
      xport_list->xport = xport; 
      LIST_ADD(svc->xport_list, xport_list, link);      
    }
    svc->service_id = service_id;
    LIST_ADD(qsap_services_list,svc,link);
  }
  QSAP_UNLOCK(&registration_list_lock);
}


void qmi_sap_post_init_task_info 
(
  qsap_os_task_type *task, 
  qsap_os_sig_type wait_sig, 
  qsap_os_sig_type timer_sig
)
{
  qsap_os_task.tcb = task;
  qsap_os_task.sig = wait_sig;
  qsap_os_task.timer_sig = timer_sig;
  qmi_sap_post_init();
}


/* Pre initialization function for synchronization with QMUX */
void qmi_sap_pre_init(void)
{
  pre_inited = 1;
  QSAP_LOCK_INIT(&registration_list_lock);
  QSAP_LOCK_INIT(&service_list_lock);
}

/* Search through the client list for a specific clid */
static qmi_sap_client_type *find_client_by_clid
(
 qmi_sap_service_type *service,
 uint8_t clid,
 void *handle
)
{
  qmi_sap_client_type *client;

  if(!service)
  {  
    return NULL;
  }
  
  client = LIST_HEAD(service->clients);
  while(client)
  {
    if(!memcmp(client->xport.addr, &clid, client->xport.xport->addr_len) && client->xport.xport->handle == handle)
      return client;
    client = client->link.next;
  }
  return NULL;
}

static void qmi_sap_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  qmi_sap_async_type *cb_data = (qmi_sap_async_type *)resp_cb_data;
  uint8_t *msg;
  uint16_t tx_id, header_msg_id, msg_len;
  qmi_sap_client_type *client = cb_data->client;

  tx_id = cb_data->tx_id;
  msg = (uint8_t *)buf - QMI_HEADER_SIZE;
  header_msg_id = (uint16_t)msg_id;
  msg_len = (uint16_t)len;

  if(transp_err != QMI_NO_ERR)
  {
    qmi_sap_send_std_resp(client, QMI_RESULT_FAILURE_V01, qcci_err_conv(transp_err), msg_id, tx_id);
  }
  else
  {
    encode_header(msg,2,tx_id,header_msg_id,msg_len);

    /* Send Message */
    //QMI_SAP_OS_LOG_TX(msg);
    client->xport.xport->ops->send(client->xport.xport->handle , 
                                   client->xport.addr, msg, msg_len+QMI_HEADER_SIZE, 
                                   NULL);
  }
  QSAP_FREE(cb_data);
  QSAP_FREE(msg);
}

/* Callback function registered w/ QMI Service to handle indications */
static void (qmi_sap_ind_cb)
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  qmi_sap_client_type *client = (qmi_sap_client_type*)ind_cb_data;
  uint8_t *msg                = QSAP_MALLOC(ind_buf_len+QMI_HEADER_SIZE);
  if (!msg)
  {
      return;
  }
  encode_header(msg,4,client->next_ind_txn_id++,msg_id,ind_buf_len);
  if (client->next_ind_txn_id == 0)
  {
      client->next_ind_txn_id++;
  }   
  QMI_MEM_COPY(msg+QMI_HEADER_SIZE,ind_buf_len,ind_buf,ind_buf_len);
  ind_buf_len += QMI_HEADER_SIZE;

  //QMI_SAP_OS_LOG_TX(msg_tmp);
  client->xport.xport->ops->send(client->xport.xport->handle , 
                 client->xport.addr, msg, ind_buf_len, NULL);
  QSAP_FREE(msg);
}

/* client list lock needs to be held */
static qmi_sap_client_type *create_client
(
 void *xport,
 void *addr
)
{
  qmi_sap_xport_type   *xp = (qmi_sap_xport_type*)xport;
  qmi_sap_service_type *service = xp->service;
  qmi_sap_client_type  *client;
  int rc;

  /* figure out if client address exists in client list */
  client = find_client_by_clid(service, *(uint8_t*)addr, xp->handle);

  if(client)
  {  
    return client;
  }

  /* client not found, create new connection */
  client = QSAP_CALLOC(sizeof(qmi_sap_client_type), 1);
  if(!client)
  {
    return NULL;
  }

  /* initialize client struct fields */
  client->service = service;
  client->xport.xport = xport;
  client->next_ind_txn_id = 1;
  QMI_MEM_COPY(client->xport.addr, MAX_ADDR_LEN, addr, xp->addr_len);

  /* Set QCCI Handle to NULL because of QMUX multiple task framework */
  client->user_handle = NULL;
  /* If qmux has provided proper task context info, connect to service*/
  if(qsap_os_task.tcb)
  {
    unsigned int num_services, num_entries=1;
    qmi_service_info info[1];
    /* Get QCCI handle */
    rc = qmi_client_get_service_list( service->service_obj, info, &num_entries, &num_services);
    if (num_services == 0)
    {
      /* ERROR HERE! */
      QSAP_FREE(client);
      return NULL;
    }
      
    /* Initialize a connection with the service */
    rc = qmi_client_init(&info[0], service->service_obj, (qmi_client_ind_cb)qmi_sap_ind_cb, client, &qsap_os_task, &client->user_handle);
    if (rc != QMI_IDL_LIB_NO_ERR)
    {
      /* ERROR HERE! */
      QSAP_FREE(client);
      return NULL;
    }
    if(service->conn_cb)
    {
      rc = service->conn_cb(client->user_handle);
      if(rc != QMI_SAP_CB_NO_ERR)
      {
        qmi_client_release(client->user_handle);
        QSAP_FREE(client);
        return NULL;
      }
    }
  }
  
  QSAP_LOCK(&service_list_lock);
  LIST_ADD(service->clients, client, link);
  QSAP_UNLOCK(&service_list_lock);
  return client;
}

static void remove_client
(
  qmi_sap_service_type *service,
  qmi_sap_client_type *client
)
{
  QSAP_LOCK(&service_list_lock);
  LIST_REMOVE(service->clients, client, link);
  QSAP_UNLOCK(&service_list_lock);
}

/*=============================================================================
                                PUBLIC FUNCTIONS
=============================================================================*/


/*=============================================================================
  FUNCTION qmi_sap_register_with_options
=============================================================================*/
qmi_sap_error qmi_sap_register_with_options
(
  qmi_idl_service_object_type service_obj,
  qmi_client_os_params        *os_params,
  qmi_sap_options             options,
  qmi_sap_client_handle       *user_handle
)
{
  qmi_sap_service_type *service;
  qmi_sap_xport_type *xport, *orig_xport;
  uint32_t service_id, service_version, max_msg_len;
  qmi_sap_registered_services *svc;
  qmi_sap_xport_list *xport_list;
  int i;

  if(!service_obj || !user_handle  || !pre_inited)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  service = QSAP_CALLOC(1,sizeof(qmi_sap_service_type));
  if(!service)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  orig_xport = QSAP_CALLOC(1,sizeof(qmi_sap_xport_type));
  if(!orig_xport)
  {
    QSAP_FREE(service);
    return QMI_SAP_INTERNAL_ERR;
  }
  service->service_obj = service_obj;
  if(qmi_idl_get_service_id(service_obj, &service_id) != QMI_NO_ERR ||
     qmi_idl_get_idl_version(service_obj, &service_version) != QMI_NO_ERR ||
     qmi_idl_get_max_service_len(service_obj, &max_msg_len) != QMI_NO_ERR)
  {
    QSAP_FREE(service);
    QSAP_FREE(orig_xport);
    return QMI_SAP_INTERNAL_ERR;
  }
  if(options.options_set & QMI_SAP_OPTIONS_CONNECT_CB_VALID)
  {
    service->conn_cb = options.conn_cb;
  }
  if(options.options_set & QMI_SAP_OPTIONS_DISCONNECT_CB_VALID)
  {
    service->disconn_cb = options.disconn_cb;
  }
  QSAP_LOCK(&registration_list_lock);
  /* Look to see if service has previously registered, do not
     try to register the same service ID multiple times */
  LIST_FIND(qsap_services_list,svc,link,svc->service_id == service_id);
  if (svc)
  {
    QSAP_FREE(service);
    QSAP_FREE(orig_xport);
    /* This handle is never useful again because you can't unregister
       from QMUX, so just make it non-null */
    *user_handle = svc;
    QSAP_UNLOCK(&registration_list_lock);
    return QMI_SAP_NO_ERR;
  }

  /* If QMUX is not up yet */
  if(!post_inited)
  {
    /* Add the service information to the registration list */
    qmi_sap_registration_type *reg_service;
    reg_service = QSAP_CALLOC(1,sizeof(qmi_sap_registration_type));
    if(!reg_service)
    {
      QSAP_UNLOCK(&registration_list_lock);
      QSAP_FREE(service);
      QSAP_FREE(orig_xport);
      return QMI_SAP_INTERNAL_ERR;
    }
    orig_xport->service = service;
    reg_service->xport = orig_xport;
    LIST_ADD(registration_list,reg_service,link);
    *user_handle = orig_xport;
    QSAP_UNLOCK(&registration_list_lock);
    return QMI_SAP_NO_ERR;
  }
  xport = NULL;
  svc = QSAP_CALLOC(1, sizeof(qmi_sap_registered_services));
  if (!svc)
  {
    QSAP_FREE(service);
    QSAP_FREE(orig_xport);
    QSAP_UNLOCK(&registration_list_lock);
    return QMI_SAP_INTERNAL_ERR;
  }    
  for(i=0;i<QMI_SAP_MAX_XPORTS;++i)
  {
    if(!xport_tbl[i].ops)
    {
      break;
    }    
    xport = QSAP_CALLOC(1,sizeof(qmi_sap_xport_type));
    if(!xport)
    {
      if (service->num_xports > 0)
      {
        continue;
      }
      QSAP_FREE(service);
      QSAP_FREE(orig_xport);
      QSAP_FREE(svc);
      QSAP_UNLOCK(&registration_list_lock);
      return QMI_SAP_INTERNAL_ERR;
    }
    QMI_MEM_COPY(xport,sizeof(qmi_sap_xport_type),orig_xport,
                 sizeof(qmi_sap_xport_type));
    xport->ops = xport_tbl[i].ops;
    xport->addr_len = xport->ops->addr_len();
    xport->service = service;
    xport->handle = xport->ops->open(xport_tbl[i].xport_data, xport, max_msg_len);
    if(!xport->handle)
    {
      if (service->num_xports > 0)
      {
        QSAP_FREE(xport);
        continue;
      }
      QSAP_FREE(orig_xport);
      QSAP_FREE(service);
      QSAP_FREE(xport);
      QSAP_FREE(svc);
      QSAP_UNLOCK(&registration_list_lock);
      return QMI_SAP_INTERNAL_ERR;
    }

    if(xport->ops->reg(xport->handle, service_id, service_version)
       != QMI_SAP_NO_ERR)
    {
      if (service->num_xports > 0)
      {
        xport->ops->close(xport->handle);
        QSAP_FREE(xport);
        continue;
      }
      xport->ops->close(xport->handle);
      QSAP_FREE(service);
      QSAP_FREE(orig_xport);
      QSAP_FREE(xport);
      QSAP_FREE(svc);
      QSAP_UNLOCK(&registration_list_lock);
      return QMI_SAP_INTERNAL_ERR;
    }
    xport_list = QSAP_CALLOC(1, sizeof(qmi_sap_xport_list));
    if(!xport_list)
    {
      if (service->num_xports > 0)
      {
        xport->ops->close(xport->handle);
        QSAP_FREE(xport);
        continue;
      }
      QSAP_FREE(service);
      QSAP_FREE(orig_xport);
      QSAP_FREE(xport);
      QSAP_FREE(svc);
      QSAP_UNLOCK(&registration_list_lock);
      return QMI_SAP_INTERNAL_ERR;
    }
    xport_list->xport = xport;
    LIST_ADD(svc->xport_list, xport_list, link);
    service->num_xports++;
  }

  svc->service_id = service_id;
  LIST_ADD(qsap_services_list,svc,link);
  QSAP_UNLOCK(&registration_list_lock);
  *user_handle = orig_xport;
  return QMI_SAP_NO_ERR;
}/* qmi_sap_register */

/*=============================================================================
  FUNCTION qmi_sap_deregister
=============================================================================*/
qmi_sap_error qmi_sap_deregister
(
  qmi_sap_client_handle user_handle
)
{
  return QMI_SAP_NO_ERR;
}/* qmi_sap_deregister */

/*=============================================================================
  FUNCTION qmi_sap_register
=============================================================================*/
qmi_sap_error qmi_sap_register
(
  qmi_idl_service_object_type service_obj,
  qmi_client_os_params        *os_params,
  qmi_sap_client_handle       *user_handle
)
{
  qmi_sap_options options;
  qmi_sap_error rc;
  if(!user_handle)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  QMI_SAP_OPTIONS_INIT(options);
  rc = qmi_sap_register_with_options(service_obj,os_params,options,user_handle);
  return rc;
}/* qmi_sap_register */

/*=============================================================================
  FUNCTION qmi_sap_register_internal
=============================================================================*/
qmi_sap_error qmi_sap_register_internal
(
  qmi_idl_service_object_type service_obj,
  qmi_client_os_params        *os_params,
  qmi_sap_client_handle       *user_handle
)
{
  qmi_sap_registered_services *svc;
  qmi_sap_options options;
  qmi_sap_error rc;
  uint32_t service_id;

  if(qmi_idl_get_service_id(service_obj, &service_id) != QMI_NO_ERR)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  LIST_FIND(qsap_services_list,svc,link,svc->service_id == service_id);
  if (svc)
  {
    return QMI_SAP_SVC_REGISTERED;
  }
  QMI_SAP_OPTIONS_INIT(options);
  rc = qmi_sap_register_with_options(service_obj,os_params,options,user_handle);
  return rc;
}/* qmi_sap_register_internal */

/*=============================================================================
  FUNCTION  qmi_sap_xport_start
=============================================================================*/
void qmi_sap_xport_start(qmi_sap_xport_ops_type *ops, void *xport_data)
{
  int i;
  if(!ops)
  {
    return;
  }
  if(!inited)
    qmi_sap_init();

  for(i=0;i<QMI_SAP_MAX_XPORTS;i++)
  {
    if(!xport_tbl[i].ops)
    {
      xport_tbl[i].ops = ops;
      xport_tbl[i].xport_data = xport_data;
      break;
    }
  }
}

qmi_sap_error qmi_sap_xport_connect
(
 void *xport,
 void *addr
)
{
  qmi_sap_client_type *client;
  qmi_sap_xport_type *xp = (qmi_sap_xport_type*)xport;

  if(!xp || !addr || !xp->service)
  {
  return QMI_SAP_INTERNAL_ERR;
  }

  client = create_client(xp,addr);
  return (client ? QMI_SAP_NO_ERR : QMI_SAP_INTERNAL_ERR);
}

qmi_sap_error qmi_sap_xport_disconnect
(
  void *xport,
  void *addr
)
{
  qmi_sap_service_type *service;
  qmi_sap_client_type *client;
  qmi_sap_xport_type *xp = (qmi_sap_xport_type*)xport;

  if(!xp || !xp->service || !addr) 
    return QMI_SAP_INTERNAL_ERR;

  service = xp->service;
  client = find_client_by_clid(service,*(uint8_t*)addr, xp->handle);
  if(client)
  {
    /* Call disconnect callback if available */
    if(xp->service->disconn_cb)
    {
      xp->service->disconn_cb(client->user_handle);
    }
    /* remove client from active list */
    qmi_client_release(client->user_handle);
    remove_client(service, client);
    QSAP_FREE(client);
  }

  return QMI_SAP_NO_ERR;
}

qmi_sap_error qmi_sap_xport_recv
(
 void *xport,
 void *addr,
 uint8_t *buf,
 uint32_t len
)
{
  int rc; 
  uint32_t resp_max;
  uint8_t ctl_flag;
  uint16_t tx_id, msg_id, req_len;
  uint8_t *resp_msg, *req_msg;
  qmi_sap_xport_type *xp;
  qmi_sap_service_type *service;
  qmi_sap_client_type *client;
  unsigned int num_services, num_entries=1;
  qmi_txn_handle txn_handle;
  qmi_service_info info[1];
  qmi_sap_async_type *cb_data;
  rc = QMI_SAP_INTERNAL_ERR;
  if(!xport || !addr || !buf || len < QMI_HEADER_SIZE)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  
  xp = (qmi_sap_xport_type *)xport;
  service = xp->service;
  client = find_client_by_clid(service, *(uint8_t*)addr, xp->handle);
  if (!client)
  {
    return QMI_SAP_INTERNAL_ERR;
  }
  if(!client->user_handle)
  {
    /* This is the first message received, initialize a connection w/ QCCI */
    if (!qsap_os_task.tcb)
    {
      qsap_os_task.tcb = rex_self();      
      qsap_os_task.sig = QMI_CLNT_WAIT_SIG;
      qsap_os_task.timer_sig = QMI_CLNT_TIMER_SIG;      
    }else
    {
      /* If this task is not the same as the one that was provided in the pre_init
         return error, because messaging will not work */
      if(qsap_os_task.tcb != rex_self())
      {
        return QMI_SAP_INTERNAL_ERR;
      }
    }
    /* Get QCCI handle */
    rc = qmi_client_get_service_list( service->service_obj, info, &num_entries, &num_services);
    if (num_services == 0)
    {
      /* ERROR HERE! */
      return QMI_SAP_INTERNAL_ERR;
    }
      
    /* Initialize a connection with the service */
    rc = qmi_client_init(&info[0], service->service_obj, (qmi_client_ind_cb)qmi_sap_ind_cb, client, &qsap_os_task, &client->user_handle);
    if (rc != QMI_IDL_LIB_NO_ERR)
    {
      /* ERROR HERE! */
      return QMI_SAP_INTERNAL_ERR;
    }
  }

  req_msg = buf;
  decode_header(buf,&ctl_flag,&tx_id,&msg_id,&req_len);
  req_msg += QMI_HEADER_SIZE;

  if(ctl_flag != QMI_CTL_FLAG_REQUEST || req_len > (len - (QMI_HEADER_SIZE)))
  {
    qmi_sap_send_std_resp(client, QMI_RESULT_FAILURE_V01, QMI_ERR_MALFORMED_MSG_V01, msg_id, tx_id);
    return QMI_SAP_INTERNAL_ERR;
  }

  if(req_len < (len - QMI_HEADER_SIZE))
  {
    /* Start by ignoring the remaining messages. 
     * In future, this must be transitioned to send an error response. */
    MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_MED, "QSAP: Ignoring potentially bundled message of len=%d first packet len=%d\n", req_len, len);
  }

  rc = qmi_idl_get_max_service_len( service->service_obj, &resp_max );
  if(rc != QMI_IDL_LIB_NO_ERR)
  {
    /* Should never happen */
    return QMI_SAP_INTERNAL_ERR;
  }
  resp_msg = QSAP_MALLOC(resp_max + QMI_HEADER_SIZE);
  if(!resp_msg)
  {
    qmi_sap_send_std_resp(client, QMI_RESULT_FAILURE_V01, QMI_ERR_NO_MEMORY_V01, msg_id, tx_id);
    return QMI_SAP_INTERNAL_ERR;
  }

  /* Send the message through QCCI */
  cb_data = QSAP_MALLOC(sizeof(qmi_sap_async_type));
  if (!cb_data)
  {
    QSAP_FREE(resp_msg);
    qmi_sap_send_std_resp(client, QMI_RESULT_FAILURE_V01, QMI_ERR_NO_MEMORY_V01, msg_id, tx_id);
    return QMI_SAP_INTERNAL_ERR;
  }
  
  cb_data->client = client;
  cb_data->service_obj = service->service_obj;
  cb_data->tx_id = tx_id;
  rc = qmi_client_send_raw_msg_async(client->user_handle , msg_id, req_msg, req_len, resp_msg + QMI_HEADER_SIZE, 
                                    resp_max, qmi_sap_rx_cb, cb_data, &txn_handle);
  if (rc != QMI_NO_ERR)
  {
    QSAP_FREE(cb_data);
    QSAP_FREE(resp_msg);
    qmi_sap_send_std_resp(client, QMI_RESULT_FAILURE_V01, qcci_err_conv(rc), msg_id, tx_id);
    return QMI_SAP_INTERNAL_ERR;
  }
  return QMI_SAP_NO_ERR;
}
