/******************************************************************************

                        QMI_IP_MSG_HDLR.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_msg_hdlr.c
  @brief   QMI over IP Message handler

  DESCRIPTION


  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/19/14   sb         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_ip.h"


static qmi_ip_conf_t * qmi_ip_conf;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/


/*==========================================================================

FUNCTION QMI_IP_NETMGR_CB()

DESCRIPTION

  Handles interface UP and DOWN events

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None

==========================================================================*/
void qmi_ip_netmgr_cb
(
  netmgr_nl_events_t event,
  netmgr_nl_event_info_t * info,
  void * data
)
{
  if (NULL == info)
  {
      LOG_MSG_ERROR("NULL Info rcvd", 0,0,0);
      return;
  }

  if (((int) info->link) != qmi_ip_conf->embms_rmnet_link)
  {
    /*we are trying to map QMUX instances with netmgr
      instances here and need to process only embms events*/
    LOG_MSG_ERROR("Event is not for eMBMS link", 0,0,0);
    return;
  }

  LOG_MSG_INFO1("Netmgr event, %d", event,0,0);

  switch(event)
  {
    case NET_PLATFORM_UP_EV:
/*---------------------------------------------------------------------------
  Capture interface UP event. Get the iface name
----------------------------------------------------------------------------*/
    case NET_PLATFORM_RECONFIGURED_EV:
      if (!strncasecmp(info->dev_name,qmi_ip_conf->qmi_info.net_iface_name,
                       strlen(qmi_ip_conf->qmi_info.net_iface_name)))
      {
        LOG_MSG_INFO1("Netmgr UP, event %d, link %d", event, info->link, 0);
        qmi_ip_conf->qmi_info.net_iface_status =1;
      }
      else
      {
        LOG_MSG_INFO1("Received link up. Not on eMBMS iface", 0, 0, 0);
      }
      break;

/*---------------------------------------------------------------------------
  Capture interface DOWN event. Reset the iface name
----------------------------------------------------------------------------*/
    case NET_PLATFORM_DOWN_EV:
      if (!strncasecmp(info->dev_name,qmi_ip_conf->qmi_info.net_iface_name,
                       strlen(qmi_ip_conf->qmi_info.net_iface_name)))
      {
        LOG_MSG_INFO1("Netmgr DOWN, event %d, link %d", event, info->link, 0);
        qmi_ip_conf->qmi_info.net_iface_status =0;
        qmi_ip_conf->qmi_info.qmi_ip_wds_v4_call_handle = 0;
        memset(qmi_ip_conf->qmi_info.net_iface_name, 0, sizeof(qmi_ip_conf->qmi_info.net_iface_name));
      }
      else
      {
        LOG_MSG_INFO1("Received link down. Not on eMBMS iface", 0, 0, 0);
      }
      break;
  }

  return;
}

/*==========================================================================

FUNCTION QMI_IP_NETMGR_INIT()

DESCRIPTION

  Initializes the netmgr handle.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
static int qmi_ip_netmgr_init()
{
  if(netmgr_client_register(qmi_ip_netmgr_cb,
                            NULL,
                            &(qmi_ip_conf->qmi_info.netmgr_hndl)) == NETMGR_FAILURE)
  {
    LOG_MSG_ERROR("netmgr_client_register failed",0,0,0);
    return FAILURE;
  }

  return SUCCESS;
}


/*==========================================================================

FUNCTION QMI_IP_NETMGR_RELEASE()

DESCRIPTION

  Releases the netmgr handle.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_netmgr_release()
{
  if(netmgr_client_release(qmi_ip_conf->qmi_info.netmgr_hndl) == NETMGR_FAILURE)
  {
    LOG_MSG_ERROR("netmgr_client_release failed",0,0,0);
    return FAILURE;
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_SEND_TO_INDICATION_SOCKET()

DESCRIPTION
  This function sends the QMI indication to indication socket for
  handling.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_send_to_indication_socket
(
  qmi_ip_ind_buf_t      *qmi_ip_ind
)
{
  struct sockaddr_un    ind_sock;
  int                   sock_fd;
  int                   len;
  int                   numBytes;
/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Open Unix domain socket and send the indication data to indication
  socket. This is done to make sure all the indications are
  processed in QMI_IP context. ( since indications are received in QMUXD
  context)
----------------------------------------------------------------------------*/
  ind_sock.sun_family = AF_UNIX;
  strlcpy(ind_sock.sun_path, QMI_IP_IND_HDLR_FILE,sizeof(QMI_IP_IND_HDLR_FILE));
  len = strlen(ind_sock.sun_path) + sizeof(ind_sock.sun_family);

  if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Error creating UNIX socket, errno: %d", errno, 0, 0);
    return FAILURE;
  }

  if ((numBytes = sendto(sock_fd,
                        (void *)qmi_ip_ind,
                         sizeof(qmi_ip_ind_buf_t),
                         0,
                         (struct sockaddr *)&ind_sock,
                         len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from wds callback context", 0, 0, 0);
    close(sock_fd);
    return FAILURE;
  }

  close(sock_fd);

  return SUCCESS;
}

/*==========================================================================

FUNCTION QMI_IP_WDS_IND_CB()

DESCRIPTION

  Indication call back for WDS client: This function is registered during
  WDS client initialization and is invoked when any indication is sent from
  WDS service on modem to the client.

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None

==========================================================================*/
void qmi_ip_wds_ind_cb
(
  qmi_client_type               user_handle,      /* QMI user handle       */
  unsigned int                  msg_id,           /* Indicator message ID  */
  void                          *ind_buf,         /* Raw indication data   */
  int                           ind_buf_len,      /* Raw data length       */
  void                         *ind_cb_data      /* User call back handle */
)
{
  qmi_ip_ind_buf_t      qmi_ip_ind;
  int                   ret_val;

  if (user_handle == qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle)
  {
/*---------------------------------------------------------------------------
   Copy the indication contents to a buffer
----------------------------------------------------------------------------*/
    qmi_ip_ind.service_type = QMI_WDS_SERVICE_TYPE;
    qmi_ip_ind.message_id = msg_id;
    if (ind_buf_len > QMI_MAX_STD_MSG_SIZE)
    {
      LOG_MSG_ERROR("ind_buf_len len %d more than max buffer lenght %d", ind_buf_len, QMI_MAX_STD_MSG_SIZE, 0);
      return;
    }
    memcpy(qmi_ip_ind.ind_buf, ind_buf, ind_buf_len);
    qmi_ip_ind.ind_buf_len = ind_buf_len;
    qmi_ip_ind.client_id = -1;

/*---------------------------------------------------------------------------
   Send to indication socket
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Failed to send indication to ind socket", 0, 0, 0);
      return;
    }

    LOG_MSG_INFO1("Successfully sent WDS indication to ind socket", 0, 0, 0);
  }

  return;
}

/*==========================================================================

FUNCTION QMI_IP_WAIT_FOR_SRV_THEN_GET_INFO()

DESCRIPTION

  This function is used during QMI client initialization where we wait
  for the QMI service to come up and then we get the information about
  the service before we initialize a client for that service

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on success
  FALSE on failure


SIDE EFFECTS
  None

==========================================================================*/

static boolean qmi_ip_wait_for_srv_then_get_info
(
  qmi_idl_service_object_type svc_obj,
  qmi_service_info *qmi_svc_info,
  qmi_client_qmux_instance_type rmnet_instance
)
{
  qmi_client_os_params os_params;
  qmi_client_type notifier_handle;
  qmi_client_error_type err;
  boolean success = FALSE;
  uint32_t num_services = 0, num_entries = 0;
  qmi_service_info            info[10];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  -*/

  LOG_MSG_INFO1("Entering qti_qmi_wait_for_srv_then_get_info",0,0,0);
  err = qmi_cci_qmux_xport_register(rmnet_instance);

  /*-----------------------------------------------------------------------
  Initialize client notifier
  ------------------------------------------------------------------------*/
  err = qmi_client_notifier_init(svc_obj, &os_params, &notifier_handle);
  if (err != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Error %d while trying to initialize client notifier",
                  err,0,0);
  }
  else
  {
    err = qmi_client_get_service_instance(svc_obj,
                                          rmnet_instance,
                                          qmi_svc_info);

    if (err != QMI_NO_ERR)
    {
      LOG_MSG_ERROR("Error %d while trying to get service info", err,0,0);
    }
    else
    {
      LOG_MSG_INFO1("Got service instance success",0,0,0);
      success = TRUE;
    }

    /*----------------------------------------------------------------------
     We need to release the client notifier here. Client notifier is only
     used to notify the client when the QMI service comes up.However client
     notifier acts as an actual QMI service client even though it is not used
     as an actual client of the service. We need to release it here to avoid
     unnecessary overhead of maintaining another client.
    ------------------------------------------------------------------------*/
    err = qmi_client_release(notifier_handle);
    if (err != QMI_NO_ERR)
    {
      LOG_MSG_ERROR("Error %d while releasing client notifier handle", err,0,0);
    }
  }

  return success;
}


/*===========================================================================

FUNCTION QMI_IP_WDS_INIT()

DESCRIPTION

  This function initializes WDS client handle

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
===========================================================================*/

int qmi_ip_wds_init
(
  qmi_client_qmux_instance_type rmnet_instance
)
{
  qmi_service_info                      qmi_svc_info;
  qmi_client_os_params                  wds_os_params;
  qmi_client_error_type                 qmi_err;
  qmi_idl_service_object_type           wds_service_object;
/*-------------------------------------------------------------------------*/

  if ((qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle != 0))
  {
    LOG_MSG_ERROR("Handle already populated",0,0,0);
    return SUCCESS;
  }

/*---------------------------------------------------------------------------
  Get the service object
  -------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  memset(&qmi_svc_info,0,sizeof(qmi_service_info ));

  if(!qmi_ip_wait_for_srv_then_get_info(wds_service_object,
                                        &qmi_svc_info,
                                        rmnet_instance))
  {
    LOG_MSG_ERROR("Error getting info for QMI_IP service",0,0,0);
    return FAILURE;
  }

/*-------------------------------------------------------------------------
  Get handle for IPv4
--------------------------------------------------------------------------*/
  qmi_err = qmi_client_init_instance(wds_service_object,
                                     rmnet_instance,
                                     (void *) qmi_ip_wds_ind_cb,
                                     NULL,
                                     &wds_os_params,
                                     QMI_TIMEOUT,
                                     &(qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle));

  if(qmi_err != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Error while trying to initialize client %d", qmi_err, 0, 0);
    return FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Successfully allocated client init WDS service for v4",
                  0,0,0);
  }

  return SUCCESS;
}



/*===========================================================================

FUNCTION QMI_IP_WDS_RELEASE()

DESCRIPTION

  This function releases the WDS client handles

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
===========================================================================*/
void qmi_ip_wds_release()
{
  int ret;

  if (qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle == 0)
  {
    LOG_MSG_INFO1("Handle not populated",0,0,0);
    return;
  }

/*---------------------------------------------------------------------------
  Release the QMI WDS client.
----------------------------------------------------------------------------*/
  ret = qmi_client_release(qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle);
  if (ret != QMI_NO_ERR)
  {
     LOG_MSG_ERROR("Error %d while releasing wds v4 client handle", ret,0,0);
  }

  qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle = (qmi_client_type) NULL;

}

/*===========================================================================
FUNCTION QMI_IP_NAS_IND_CB()

DESCRIPTION
  Indication call back for NAS client: This function is registered during
  NAS client initialization and is invoked when any indication is sent from
  NAS service on modem to the client.

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
/*=========================================================================*/
void qmi_ip_nas_ind_cb
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{

  qmi_ip_ind_buf_t      qmi_ip_ind;
  struct sockaddr_un    ind_sock;
  int                   sock_fd;
  int                   len;
  int                   numBytes;
  int                   ret_val;


  if (user_handle == qmi_ip_conf->qmi_info.qmi_ip_nas_handle)
  {
/*---------------------------------------------------------------------------
  Copy the indication contents into a buffer
----------------------------------------------------------------------------*/
    qmi_ip_ind.service_type = QMI_NAS_SERVICE_TYPE;
    qmi_ip_ind.message_id = msg_id;
    if (ind_buf_len > QMI_MAX_STD_MSG_SIZE)
    {
      LOG_MSG_ERROR("ind_buf_len len %d more than max buffer lenght %d", ind_buf_len, QMI_MAX_STD_MSG_SIZE, 0);
      return;
    }
    memcpy(qmi_ip_ind.ind_buf, ind_buf, ind_buf_len);
    qmi_ip_ind.ind_buf_len = ind_buf_len;
    qmi_ip_ind.client_id = -1;

/*---------------------------------------------------------------------------
  Send the data to indication socket
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Failed to send indication to ind socket", 0, 0, 0);
      return;
    }

    LOG_MSG_INFO1("Successfully sent NAS indication to ind socket", 0, 0, 0);
  }

  return;
}


/*===========================================================================

FUNCTION QMI_IP_NAS_INIT()

DESCRIPTION

  This function initializes NAS client handle

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
===========================================================================*/

int qmi_ip_nas_init()
{
  qmi_client_os_params                  nas_os_params;
  qmi_client_error_type                 qmi_err;
  qmi_idl_service_object_type           nas_service_object;
/*-------------------------------------------------------------------------*/

  if ((qmi_ip_conf->qmi_info.qmi_ip_nas_handle != 0))
  {
    LOG_MSG_ERROR("Handle already populated",0,0,0);
    return SUCCESS;
  }

/*---------------------------------------------------------------------------
  Get the service object
----------------------------------------------------------------------------*/
  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
    return FAILURE;
  }

/*-------------------------------------------------------------------------
  Get handle for NAS
--------------------------------------------------------------------------*/
  qmi_err = qmi_client_init_instance(nas_service_object,
                                     QMI_CLIENT_INSTANCE_ANY,
                                     (void *) qmi_ip_nas_ind_cb,
                                     NULL,
                                     &nas_os_params,
                                     QMI_TIMEOUT,
                                     &(qmi_ip_conf->qmi_info.qmi_ip_nas_handle));

  if(qmi_err != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Error while trying to initialize client %d", qmi_err, 0, 0);
    return FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Successfully allocated client init NAS service",
                  0,0,0);
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_NAS_RELEASE()

DESCRIPTION

  This function releases the NAS client handles

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
===========================================================================*/
void qmi_ip_nas_release()
{
  int ret;

  if (qmi_ip_conf->qmi_info.qmi_ip_nas_handle == 0)
  {
    LOG_MSG_INFO1("Handle not populated",0,0,0);
    return;
  }

/*---------------------------------------------------------------------------
  Release the NAS client.
----------------------------------------------------------------------------*/
  ret = qmi_client_release(qmi_ip_conf->qmi_info.qmi_ip_nas_handle);
  if (ret != QMI_NO_ERR)
  {
     LOG_MSG_ERROR("Error %d while releasing wds v4 client handle", ret,0,0);
  }

  qmi_ip_conf->qmi_info.qmi_ip_nas_handle = (qmi_client_type) NULL;

}

/*===========================================================================

FUNCTION QMI_IP_SEND_WDS_MESSAGE_TO_MODEM()

DESCRIPTION

  This function encodes a message , sends it to modem
  and then decodes the response

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
qmi_ip_send_wds_qmi_message_to_modem
(
  uint16          req_type,
  void           *req_msg,
  uint16          req_msg_size,
  uint16          resp_type,
  void           *resp_msg,
  uint16          resp_msg_size
)
{
  qmi_idl_service_object_type wds_service_object;
  int ret_val = QMI_NO_ERR;
  int encoded_resp_msg_len, encoded_req_msg_len, max_msg_len;
  char resp_buf[MAX_QMI_IP_PKT_SIZE];
  char req_buf[MAX_QMI_IP_PKT_SIZE];
  qmi_ip_qmux_msg_s                        *qmux_msg;

  memset(req_buf,0,MAX_QMI_IP_PKT_SIZE);
  memset(resp_buf,0,MAX_QMI_IP_PKT_SIZE);

  qmux_msg = (qmi_ip_qmux_msg_s *)(&req_buf);

  wds_service_object = wds_get_service_object_v01();
  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }

  ret_val = qmi_idl_get_max_message_len(wds_service_object,
                                        QMI_IDL_REQUEST,
                                        req_type,
                                        &max_msg_len);
  if(ret_val != QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to get max_msg_len %d",0,0,0);
    return FAILURE;
  }

  ret_val = qmi_idl_message_encode(
                wds_service_object,
                QMI_IDL_REQUEST,
                req_type,
                req_msg,
                req_msg_size,
                req_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                max_msg_len,
                &encoded_req_msg_len);

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding set client ip pref message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  qmux_msg->qmux_hdr.length = QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES -1 + encoded_req_msg_len;
  qmux_msg->sdu.qmux.msg.msg_length = encoded_req_msg_len;
  qmux_msg->qmux_hdr.svc_type = QMI_WDS_SERVICE_TYPE;
  qmux_msg->sdu.qmux.hdr.svc_ctl_flags = QMI_REQ_CTRL_FLAGS;
  qmux_msg->sdu.qmux.hdr.txn_id = QMI_SAMPLE_TX_ID;

  /*-----------------------------------------------------------------------------
    Will send QMI message to modem
  -----------------------------------------------------------------------------*/
  ret_val = qmi_ip_send_message_to_modem(
                           qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                           req_type,
                           req_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           encoded_req_msg_len,
                           resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           &encoded_resp_msg_len);
  if (ret_val == FAILURE)
  {
    LOG_MSG_ERROR("Error in sending set ip pref message to modem %d", ret_val, 0, 0);
    return FAILURE;
  }

  /*-----------------------------------------------------------------------------
    Decode the QMI message response
  -----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                wds_service_object,
                QMI_IDL_RESPONSE,
                resp_type,
                resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                encoded_resp_msg_len,
                resp_msg,
                resp_msg_size);
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding set ip pref resp message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_GET_QMIPORT_CALL_STATUS()

DESCRIPTION

  This function binds mux port for embms call

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
qmi_ip_get_qmiport_call_status()
{
  uint16 pkt_srvc_status_v4, pkt_srvc_status_v6;
  int ret_val;
  wds_set_client_ip_family_pref_req_msg_v01   set_ip_pref_req;
  wds_set_client_ip_family_pref_resp_msg_v01  set_ip_pref_resp;
  wds_get_pkt_srvc_status_req_msg_v01         get_pkt_srvc_status_req;
  wds_get_pkt_srvc_status_resp_msg_v01        get_pkt_srvc_status_resp;

  pkt_srvc_status_v4 = WDS_DISCONNECTED;
  pkt_srvc_status_v6 = WDS_DISCONNECTED;

  memset(&set_ip_pref_req,0,sizeof(wds_set_client_ip_family_pref_req_msg_v01));
  memset(&set_ip_pref_resp,0,sizeof(wds_set_client_ip_family_pref_resp_msg_v01));
  memset(&get_pkt_srvc_status_req,0,sizeof(wds_get_pkt_srvc_status_req_msg_v01));
  memset(&get_pkt_srvc_status_resp,0,sizeof(wds_get_pkt_srvc_status_req_msg_v01));

  set_ip_pref_req.ip_preference = WDS_IP_FAMILY_PREF_IPV6;

  ret_val = qmi_ip_send_wds_qmi_message_to_modem(QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ_V01,
                                                 (void *)&set_ip_pref_req,
                                                 sizeof(wds_set_client_ip_family_pref_req_msg_v01),
                                                 QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_RESP_V01,
                                                 (void *)&set_ip_pref_resp,
                                                 sizeof(wds_set_client_ip_family_pref_resp_msg_v01));
  if (ret_val != SUCCESS ||
      set_ip_pref_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Failed to set client ip pref error %d",
                  set_ip_pref_resp.resp.error,0,0);
    return FAILURE;
  }

  ret_val = qmi_ip_send_wds_qmi_message_to_modem(QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
                                                 (void *)&get_pkt_srvc_status_req,
                                                 sizeof(wds_get_pkt_srvc_status_req_msg_v01),
                                                 QMI_WDS_GET_PKT_SRVC_STATUS_RESP_V01,
                                                 (void *)&get_pkt_srvc_status_resp,
                                                 sizeof(wds_get_pkt_srvc_status_resp_msg_v01));
  if (ret_val == SUCCESS &&
      get_pkt_srvc_status_resp.resp.result == QMI_RESULT_SUCCESS_V01)
  {
    pkt_srvc_status_v6 = get_pkt_srvc_status_resp.connection_status;
    LOG_MSG_INFO1("pkt_srvc_status_v6 = %d",pkt_srvc_status_v6,0,0);
  }
  else
  {
    /* this logic makes sure that in case of failure, the ip pref
    is set to v4 again*/
    LOG_MSG_ERROR("Failed to get pkt srvc status for v6 error %d",
                  get_pkt_srvc_status_resp.resp.error,0,0);
    pkt_srvc_status_v6 = 0;
  }

  memset(&set_ip_pref_req,0,sizeof(wds_set_client_ip_family_pref_req_msg_v01));
  memset(&set_ip_pref_resp,0,sizeof(wds_set_client_ip_family_pref_resp_msg_v01));
  memset(&get_pkt_srvc_status_req,0,sizeof(wds_get_pkt_srvc_status_req_msg_v01));
  memset(&get_pkt_srvc_status_resp,0,sizeof(wds_get_pkt_srvc_status_req_msg_v01));

  set_ip_pref_req.ip_preference = WDS_IP_FAMILY_PREF_IPV4;

  ret_val = qmi_ip_send_wds_qmi_message_to_modem(QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ_V01,
                                                 (void *)&set_ip_pref_req,
                                                 sizeof(wds_set_client_ip_family_pref_req_msg_v01),
                                                 QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_RESP_V01,
                                                 (void *)&set_ip_pref_resp,
                                                 sizeof(wds_set_client_ip_family_pref_resp_msg_v01));
  if (ret_val != SUCCESS ||
      set_ip_pref_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Failed to set client ip pref error %d",
                  set_ip_pref_resp.resp.error,0,0);
    return FAILURE;
  }

  ret_val = qmi_ip_send_wds_qmi_message_to_modem(QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
                                                 (void *)&get_pkt_srvc_status_req,
                                                 sizeof(wds_get_pkt_srvc_status_req_msg_v01),
                                                 QMI_WDS_GET_PKT_SRVC_STATUS_RESP_V01,
                                                 (void *)&get_pkt_srvc_status_resp,
                                                 sizeof(wds_get_pkt_srvc_status_resp_msg_v01));
  if (ret_val != SUCCESS ||
      get_pkt_srvc_status_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Failed to get pkt srvc status for v4 error %d",
                  get_pkt_srvc_status_resp.resp.error,0,0);
    return FAILURE;
  }

  pkt_srvc_status_v4 = get_pkt_srvc_status_resp.connection_status;
  LOG_MSG_INFO1("pkt_srvc_status_v4 = %d",pkt_srvc_status_v4,0,0);

  if( WDS_DISCONNECTED != pkt_srvc_status_v4 ||
      WDS_DISCONNECTED != pkt_srvc_status_v6)
  {
    LOG_MSG_ERROR("Call is already up for this mux port",0,0,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("MUX Port available",0,0,0);
  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_BIND_MUX_PORT()

DESCRIPTION

  This function binds mux port for embms call

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int qmi_ip_bind_mux_port(int iterator)
{
  int ret = 0;
  int epid_type = -1;
  int epid = -1;
  int embms_rmnet_link_id = 0;
  wds_bind_mux_data_port_req_msg_v01 bind_mux_data_port_req;
  wds_bind_mux_data_port_resp_msg_v01 bind_mux_data_port_resp;

  //call IOCTL RMNET_IOCTL_GET_EPID and bind this to the logical rmnet link-id
  #ifdef FEATURE_DATA_TARGET_MDM9607
    ds_get_epid(DEVICE_RMNET_DATA, &epid_type, &epid);
  #else
    ds_get_epid(DEVICE_RMNET_IPA, &epid_type, &epid);
  #endif

  if (epid == -1)
  {
    LOG_MSG_ERROR("Error in ds_get_epid",0,0,0);
    return FAILURE;
  }

  memset (&bind_mux_data_port_req, 0, sizeof(bind_mux_data_port_req));
  memset (&bind_mux_data_port_resp, 0, sizeof(bind_mux_data_port_resp));

  bind_mux_data_port_req.ep_id_valid = (epid != -1);
  bind_mux_data_port_req.ep_id.ep_type = (data_ep_type_enum_v01) epid_type;
  bind_mux_data_port_req.ep_id.iface_id = (uint32_t) epid;

  //get the logical link_id. mux_id = logical link id +1
  embms_rmnet_link_id = (int) (QMI_CLIENT_QMUX_RMNET_INSTANCE_7 - QMI_CLIENT_QMUX_RMNET_INSTANCE_0 - iterator);
  LOG_MSG_INFO1("WDS embedded logical instance %d",
                embms_rmnet_link_id,0,0);

  bind_mux_data_port_req.mux_id = (uint8_t)(embms_rmnet_link_id + 1);
  bind_mux_data_port_req.mux_id_valid = (bind_mux_data_port_req.mux_id != -1);

  ret = qmi_client_send_msg_sync(qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                                 QMI_WDS_BIND_MUX_DATA_PORT_REQ_V01,
                                 (void *)&bind_mux_data_port_req,
                                 sizeof(wds_bind_mux_data_port_req_msg_v01),
                                 (void*)&bind_mux_data_port_resp,
                                 sizeof(wds_bind_mux_data_port_resp_msg_v01),
                                 QMI_TIMEOUT);

  if((ret != QMI_NO_ERR) || (bind_mux_data_port_resp.resp.result != QMI_NO_ERR ))
  {
    LOG_MSG_ERROR("Error while sending QMI_WDS_BIND_MUX_DATA_PORT_REQ message: %d %d", ret,
                  bind_mux_data_port_resp.resp.result,0);
    return FAILURE;
  }
  else if (qmi_ip_get_qmiport_call_status() != SUCCESS)
  {
    LOG_MSG_ERROR("QMI Port is not free.",0,0,0);
    return FAILURE;
  }

  qmi_ip_conf->embms_rmnet_link = embms_rmnet_link_id;
  snprintf( qmi_ip_conf->qmi_info.net_iface_name, QMI_IP_MAX_INTERFACE_NAME,
            "%s%d", RMNET_INTERFACE, qmi_ip_conf->embms_rmnet_link);
  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_SEND_MESSAGE_TO_MODEM()

DESCRIPTION
  This function sends the QMI message to modem.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_send_message_to_modem
(
  qmi_client_type           user_handle,
  unsigned int              msg_id,
  void                      *req_buf,
  unsigned int              req_buf_len,
  void                      *resp_buf,
  unsigned int              *resp_buf_recv_len
)
{
  int         ret;
/*------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Send QMI message to modem.
----------------------------------------------------------------------------*/
  ret =  qmi_client_send_raw_msg_sync (user_handle,
                                       msg_id,
                                       req_buf,
                                       req_buf_len,
                                       resp_buf,
                                       QMI_MAX_STD_MSG_SIZE,
                                       resp_buf_recv_len,
                                       QMI_TIMEOUT);

  if (ret == QMI_NO_ERR)
  {
    LOG_MSG_INFO1("Sucessfully received response from modem", 0, 0, 0);
    return SUCCESS;
  }
  else
  {
    LOG_MSG_ERROR("Failed to send req to modem", 0, 0, 0);
    return FAILURE;
  }
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_QMI_IP_MESSAGE()

DESCRIPTION
  Processes QMI IP service messages

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/

int qmi_ip_proccess_qmi_ip_message
(
  int    active_fd,
  char   *msg,
  int    msg_len,
  int    message_id,
  char   *resp_buf,
  int    *resp_msg_len
)
{
  int                                      ret_val;
  int                                      ret;
  qmi_idl_service_object_type              qmi_ip_service_object;
  qmi_ip_client_session_init_req_msg_v01   qmi_ip_client_session_init_req;
  qmi_ip_client_session_init_resp_msg_v01  qmi_ip_client_session_init_resp;
  int                                      error_code = QMI_ERR_NONE_V01;
  int                                      client_id;
  int                                      struct_size;
  int                                      max_msg_len;
  int                                      client_index;
  char                                     *client_ip_addr;
  FILE                                     *fp = NULL;

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get QMI IP service object used for encode/decode
----------------------------------------------------------------------------*/
  qmi_ip_service_object = qmi_ip_get_service_object_v01();

  if(!qmi_ip_service_object)
  {
    LOG_MSG_ERROR("qmi_ip get service object failed",0,0,0);
  }

  switch (message_id)
  {
/*---------------------------------------------------------------------------
  Process QMI IP client INIT req
----------------------------------------------------------------------------*/
    case QMI_IP_CLIENT_SESSION_INIT_REQ_V01:
    {
      memset(&qmi_ip_client_session_init_req,
             0,
             sizeof(qmi_ip_client_session_init_req_msg_v01));
      if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
      {
        LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
        return FAILURE;
      }
/*---------------------------------------------------------------------------
  Decode the QMI message
----------------------------------------------------------------------------*/
      ret_val = qmi_idl_message_decode(
                  qmi_ip_service_object,
                  QMI_IDL_REQUEST,
                  message_id,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &qmi_ip_client_session_init_req,
                  sizeof(qmi_ip_client_session_init_req_msg_v01));

      if (ret_val < QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
        error_code = QMI_ERR_INTERNAL_V01;
        client_id = FAILURE;
        goto send_error;
      }

/*---------------------------------------------------------------------------
  Get the client ID associated with the connection
----------------------------------------------------------------------------*/
      client_id = qmi_ip_get_client_id(active_fd);
      if (client_id == FAILURE)
      {
        LOG_MSG_ERROR("Invalid fd passed %d", active_fd, 0, 0);
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }

/*---------------------------------------------------------------------------
  Get the contents of the message: client version info and IPv4 address.
  Store the IPv4 address of the client
----------------------------------------------------------------------------*/
      LOG_MSG_INFO1("QMI IP client version major %d minor %d",
                    qmi_ip_client_session_init_req.client_version.major_version,
                    qmi_ip_client_session_init_req.client_version.minor_version,
                    0);

      LOG_MSG_INFO1("QMI IP client IP type %d",
                    qmi_ip_client_session_init_req.ip_type, 0, 0);

      if (qmi_ip_client_session_init_req.client_ipv4_addr_valid)
      {
/*---------------------------------------------------------------------------
  Check if there is already an active connection for this client.
  If yes, close the existing connection and create a new connection: This is
  to handle cases where the client goes out of Wifi range and comes back and
  initiates client init within fpop timeout.
----------------------------------------------------------------------------*/
        client_index = qmi_ip_get_client_id_by_ip(
                            htonl(qmi_ip_client_session_init_req.client_ipv4_addr));
        if (client_index != FAILURE)
        {
          qmi_ip_close_connection(qmi_ip_conf->embms_client[client_index].socket_fd);

/*---------------------------------------------------------------------------
  Get the client ID associated with the connection
----------------------------------------------------------------------------*/
          client_id = qmi_ip_get_client_id(active_fd);
          if (client_id == FAILURE)
          {
            LOG_MSG_ERROR("Invalid fd passed %d", active_fd, 0, 0);
            error_code = QMI_ERR_INTERNAL_V01;
            goto send_error;
          }
        }

/*---------------------------------------------------------------------------
  Initialize new connection parameters.
----------------------------------------------------------------------------*/
        qmi_ip_conf->embms_client[client_id].client_ip.s_addr =
                  htonl(qmi_ip_client_session_init_req.client_ipv4_addr);
        LOG_MSG_INFO1("Client IPV4 address:", 0, 0, 0);
        IPV4_ADDR_MSG(qmi_ip_conf->embms_client[client_id].client_ip.s_addr);
        qmi_ip_conf->embms_client[client_id].is_active = TRUE;

/*---------------------------------------------------------------------------
  Create FPOP timer for client in multi client mode.
----------------------------------------------------------------------------*/
        if (qmi_ip_conf->mode == MULTI_CLIENT_MODE)
        {
          ret_val = qmi_ip_fpop_create_client_timer(qmi_ip_conf->embms_client[client_id].socket_fd);
          if (ret_val != SUCCESS)
          {
            LOG_MSG_ERROR("Error in creating timer for client fd %d",
                          qmi_ip_conf->embms_client[client_id].socket_fd, 0, 0);
          }
        }
      }

/*---------------------------------------------------------------------------
  Encode the response message:
  - Fill server version
  - FPOP time out and threshold.
----------------------------------------------------------------------------*/
      memset(&qmi_ip_client_session_init_resp,
             0,
             sizeof(qmi_ip_client_session_init_resp_msg_v01));

      qmi_ip_client_session_init_resp.resp.result = QMI_RESULT_SUCCESS_V01;
      qmi_ip_client_session_init_resp.resp.error = QMI_ERR_NONE_V01;
      qmi_ip_client_session_init_resp.server_version.major_version = QMI_IP_MAJOR_VERSION;
      qmi_ip_client_session_init_resp.server_version.minor_version = QMI_IP_MINOR_VERSION;
      if (qmi_ip_conf->mode == MULTI_CLIENT_MODE)
      {
        qmi_ip_client_session_init_resp.fpop_timeout = qmi_ip_conf->fpop_timeout;
        qmi_ip_client_session_init_resp.fpop_timeout_threshold = qmi_ip_conf->fpop_threshold;
      }

      struct_size= sizeof(qmi_ip_client_session_init_resp_msg_v01);
      ret_val = qmi_idl_get_max_message_len(qmi_ip_service_object,
                                            QMI_IDL_RESPONSE,
                                            message_id,
                                            &max_msg_len);
      if(ret_val != QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Failed to get max msg len %d", ret_val, 0, 0);
      }
/*---------------------------------------------------------------------------
  Encode the contents in QMI format
----------------------------------------------------------------------------*/
      ret_val = qmi_idl_message_encode(
                   qmi_ip_service_object,
                   QMI_IDL_RESPONSE,
                   message_id,
                   &qmi_ip_client_session_init_resp,
                   struct_size,
                   resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                   max_msg_len,
                   resp_msg_len);

      if (ret_val != QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in encoding response message. ret_val %d", ret_val, 0 ,0);
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      else
      {
        qmi_ip_conf->embms_client[client_id].init_done = TRUE;
        LOG_MSG_INFO1("Successfully encoded response message", 0, 0 ,0);

/*---------------------------------------------------------------------------
  Logging for demo
----------------------------------------------------------------------------*/
        client_ip_addr = inet_ntoa(qmi_ip_conf->embms_client[client_id].client_ip);

#ifdef FEATURE_DEMO_LOGGING
        fp = fopen("/usr/bin/out_logs.txt", "a");
        if (fp != NULL)
        {
          fprintf(fp, "\nDevice with IP %s enabled eMBMS over SoftAP\n", client_ip_addr);
          fclose(fp);
        }
        else
        {
          LOG_MSG_ERROR("Error writing in output log file",0,0,0);
        }
#endif
      }
    }
    break;

    default:
      LOG_MSG_ERROR("Invalid QMI IP message received", 0, 0, 0);
      error_code = QMI_ERR_MALFORMED_MSG_V01;
      client_id = FAILURE;
      goto send_error;
  }

  return SUCCESS;

/*---------------------------------------------------------------------------
  Handle the error case
----------------------------------------------------------------------------*/
send_error:

  if (client_id != FAILURE)
  {
    qmi_ip_conf->embms_client[client_id].init_done = FALSE;
  }
  ret_val = qmi_idl_encode_resp_tlv(
               QMI_RESULT_FAILURE_V01,
               error_code,
               resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
               MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

  if (ret_val != QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding response error message. ret_val %d", ret_val, 0 ,0);
    return FAILURE;
  }
  else
  {
    *resp_msg_len = QMI_RESP_TLV_LEN;
    LOG_MSG_INFO1("Successfully encoded response error message", 0, 0 ,0);
    return SUCCESS;
  }
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_CONFIG_EMBMS_MESSAGE()

DESCRIPTION
  This function handles the processing of NAS config embms
  QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_nas_config_embms_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  int                                      ret_val, i;
  int                                      action_taken;
  qmi_idl_service_object_type              nas_service_object;
  nas_config_embms_req_msg_v01             config_embms_req;
  nas_config_embms_resp_msg_v01            config_embms_resp;
  nas_embms_status_ind_msg_v01             embms_status_ind;
  qmi_ip_ind_buf_t                         qmi_ip_ind;
  int                                      client_id;
  int                                      error_code;

  /*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get client ID corresponding to FD
----------------------------------------------------------------------------*/
  client_id = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Validation to make sure client init message was received before config
  embms.
----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].init_done == FALSE)
  {
    LOG_MSG_ERROR("QMI IP Client Init Not Done. Can't send NAS message",0,0,0);
    error_code = QMI_ERR_INVALID_OPERATION_V01;
    return FAILURE;
  }

  memset(&config_embms_req, 0, sizeof(nas_config_embms_req_msg_v01));

/*---------------------------------------------------------------------------
  Get NAS service object used for encode/decode
----------------------------------------------------------------------------*/
  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  nas_service_object,
                  QMI_IDL_REQUEST,
                  QMI_NAS_CONFIG_EMBMS_REQ_MSG_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &config_embms_req,
                  sizeof(nas_config_embms_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding nas config embms req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("Config embms req %d", config_embms_req.enable, 0, 0);

/*---------------------------------------------------------------------------
  Decide whether the NAS config embms enable request needs to be sent to modem
----------------------------------------------------------------------------*/
  if (config_embms_req.enable == TRUE)
  {
/*---------------------------------------------------------------------------
  Dup req. Reply locally.
----------------------------------------------------------------------------*/
    if (qmi_ip_conf->embms_client[client_id].is_nce_active)
    {
      LOG_MSG_INFO1("Config embms already done for client. Respond locally", 0, 0, 0);
      action_taken = REPLY_LOCALLY;
    }
/*---------------------------------------------------------------------------
  Not the first req. Reply locally
----------------------------------------------------------------------------*/
    else if (qmi_ip_conf->qmi_info.nce_count > QMI_IP_INACTIVE)
    {
      LOG_MSG_INFO1("Config embms already done on modem. Respond locally", 0, 0, 0);
      action_taken = REPLY_LOCALLY;
    }
/*---------------------------------------------------------------------------
  First req. Send to modem.
----------------------------------------------------------------------------*/
    if (qmi_ip_conf->qmi_info.nce_count == QMI_IP_INACTIVE)
    {
      LOG_MSG_INFO1("First config embms enable. Will send to modem", 0, 0, 0);
      action_taken = SEND_TO_MODEM;
    }
  }
/*---------------------------------------------------------------------------
  Decide if the disable config embms req needs to be sent to modem.
----------------------------------------------------------------------------*/
  else if (config_embms_req.enable == FALSE)
  {
/*---------------------------------------------------------------------------
  Dup req/ client has not activated it yet. Reply locally.
----------------------------------------------------------------------------*/
    if (!qmi_ip_conf->embms_client[client_id].is_nce_active)
    {
      LOG_MSG_INFO1("Config embms not active for client. Respond locally", 0, 0, 0);
      action_taken = REPLY_LOCALLY;
    }
/*---------------------------------------------------------------------------
  Not the last req to disable. Reply locally.
----------------------------------------------------------------------------*/
    else if (qmi_ip_conf->qmi_info.nce_count > QMI_IP_ACTIVE)
    {
      LOG_MSG_INFO1("Client with fd %d not the last client for config embms",active_fd,0,0);
      action_taken = REPLY_LOCALLY;
    }
/*---------------------------------------------------------------------------
  Last req to disable. Send to modem.
----------------------------------------------------------------------------*/
    else if (qmi_ip_conf->qmi_info.nce_count == QMI_IP_ACTIVE)
    {
      LOG_MSG_INFO1("Client woth fd %d is the last client for config embms."
                    "Send to modem",active_fd,0,0);
      action_taken = SEND_TO_MODEM;
    }
  }

/*---------------------------------------------------------------------------
  Send the req to modem
----------------------------------------------------------------------------*/
  if (action_taken == SEND_TO_MODEM)
  {
/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                           qmi_ip_conf->qmi_info.qmi_ip_nas_handle,
                           QMI_NAS_CONFIG_EMBMS_REQ_MSG_V01,
                           msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending config embms message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                  nas_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_NAS_CONFIG_EMBMS_RESP_MSG_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &config_embms_resp,
                  sizeof(nas_config_embms_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding config embms resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }
    if (config_embms_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in performing config embms on modem. err %d",
                    config_embms_resp.resp.error, 0, 0);
      return FAILURE;
    }
  }
/*---------------------------------------------------------------------------
  Dont send the req to modem. Respond locally
----------------------------------------------------------------------------*/
  else if (action_taken == REPLY_LOCALLY)
  {
/*-----------------------------------------------------------------------------
    Populate the indication message and send to indication handler socket
-----------------------------------------------------------------------------*/
    memset(&embms_status_ind, 0, sizeof(nas_embms_status_ind_msg_v01));
    embms_status_ind.enabled = config_embms_req.enable;

    memset(&qmi_ip_ind, 0, sizeof(qmi_ip_ind));

    ret_val = qmi_idl_message_encode(
                  nas_service_object,
                  QMI_IDL_INDICATION,
                  QMI_NAS_EMBMS_STATUS_IND_V01,
                  &embms_status_ind,
                  sizeof(nas_embms_status_ind_msg_v01),
                  &(qmi_ip_ind.ind_buf),
                  QMI_MAX_STD_MSG_SIZE,
                  &(qmi_ip_ind.ind_buf_len));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding embms status ind message %d", ret_val, 0 ,0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Send to indication handler socket
-----------------------------------------------------------------------------*/
    qmi_ip_ind.client_id = active_fd;
    qmi_ip_ind.service_type = QMI_NAS_SERVICE_TYPE;
    qmi_ip_ind.message_id = QMI_NAS_EMBMS_STATUS_IND_V01;
    ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Failure to send embms status ind to ind socket", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
    memset(&config_embms_resp, 0, sizeof(nas_config_embms_resp_msg_v01));
    config_embms_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    config_embms_resp.resp.error = QMI_ERR_NONE_V01;

    ret_val = qmi_idl_message_encode(
                  nas_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_NAS_CONFIG_EMBMS_RESP_MSG_V01,
                  &config_embms_resp,
                  sizeof(nas_config_embms_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding config embms response message %d", ret_val, 0 ,0);
      return FAILURE;
    }
  }

/*---------------------------------------------------------------------------
  Update the NAS config embms (nce) status for the client
----------------------------------------------------------------------------*/
  if (config_embms_req.enable == TRUE &&
      !qmi_ip_conf->embms_client[client_id].is_nce_active)
  {
    qmi_ip_conf->embms_client[client_id].is_nce_active = TRUE;
    qmi_ip_conf->qmi_info.nce_count++;
  }

  if (config_embms_req.enable == FALSE &&
      qmi_ip_conf->embms_client[client_id].is_nce_active)
  {
    qmi_ip_conf->embms_client[client_id].is_nce_active = FALSE;
    qmi_ip_conf->qmi_info.nce_count--;
  }

  LOG_MSG_INFO1("NAS config embms status for client %d is %d",
                client_id,
                qmi_ip_conf->embms_client[client_id].is_nce_active,
                0);
  LOG_MSG_INFO1("NAS config embms count %d", qmi_ip_conf->qmi_info.nce_count, 0, 0);

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_GET_EMBMS_STATUS_MESSAGE()

DESCRIPTION
  This function handles the processing of NAS get embms status
  QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_nas_get_embms_status_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  int                                      ret_val;
  int                                      i = 0;
  qmi_idl_service_object_type              nas_service_object;
  nas_get_embms_status_resp_msg_v01        get_embms_status_resp;
  int                                      client_id;
  int                                      struct_size;
  int                                      max_msg_len;
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get NAS service object used for encode/decode
----------------------------------------------------------------------------*/
  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Get client ID for the fd
----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  If the client has enabled embms config: Send the req to modem so that
  modem responds that embms is enabled.
----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].is_nce_active)
  {
    LOG_MSG_INFO1("Sending get embms status request to modem", 0, 0, 0);
/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                           qmi_ip_conf->qmi_info.qmi_ip_nas_handle,
                           QMI_NAS_GET_EMBMS_STATUS_REQ_MSG_V01,
                           msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending get embms status message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                      nas_service_object,
                      QMI_IDL_RESPONSE,
                      QMI_NAS_GET_EMBMS_STATUS_RESP_MSG_V01,
                      resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      *resp_msg_len,
                      &get_embms_status_resp,
                      sizeof(nas_get_embms_status_resp_msg_v01));

    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding get embms status resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }

    if (get_embms_status_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in performing get embms status on modem. err %d",
                    get_embms_status_resp.resp.error, 0, 0);
      return FAILURE;
    }
  }
/*---------------------------------------------------------------------------
  If the client has not enabled embms config: Respond locally that embms is
  disabled.
----------------------------------------------------------------------------*/
  else
  {
    memset(&get_embms_status_resp, 0, sizeof(nas_get_embms_status_resp_msg_v01));
/*---------------------------------------------------------------------------
  Populate response
----------------------------------------------------------------------------*/
    get_embms_status_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    get_embms_status_resp.resp.error = QMI_ERR_NONE_V01;
    get_embms_status_resp.enabled_valid = TRUE;
    get_embms_status_resp.enabled = FALSE;

    struct_size= sizeof(nas_get_embms_status_resp_msg_v01);
    ret_val = qmi_idl_get_max_message_len(nas_service_object,
                                            QMI_IDL_RESPONSE,
                                            QMI_NAS_GET_EMBMS_STATUS_RESP_MSG_V01,
                                            &max_msg_len);
    if(ret_val != QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Failed to get max msg len %d", ret_val, 0, 0);
    }
/*---------------------------------------------------------------------------
  Encode the contents in QMI format
----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_encode(
                   nas_service_object,
                   QMI_IDL_RESPONSE,
                   QMI_NAS_GET_EMBMS_STATUS_RESP_MSG_V01,
                   &get_embms_status_resp,
                   struct_size,
                   resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                   max_msg_len,
                   resp_msg_len);

    if (ret_val != QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding response message. ret_val %d", ret_val, 0 ,0);
      return FAILURE;
    }
    else
    {
      LOG_MSG_INFO1("Successfully encoded response message", 0, 0 ,0);
    }
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_IND_REG_MESSAGE()

DESCRIPTION
  This function handles the processing of NAS ind reg
  QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_nas_ind_reg_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  int                                      ret_val,i;
  uint8                                    sys_info_reg = IND_REQ_NOT_REQUIRED;
  uint8                                    embms_status_reg = IND_REQ_NOT_REQUIRED;
  qmi_idl_service_object_type              nas_service_object;
  nas_indication_register_req_msg_v01      nas_ind_reg_req;
  nas_indication_register_resp_msg_v01     nas_ind_reg_resp;
  nas_indication_register_req_msg_v01      nas_indication_reg_req;
  nas_indication_register_resp_msg_v01     nas_indication_reg_resp;
  int                                      client_id;

/*-------------------------------------------------------------------------*/

  memset(&nas_ind_reg_req, 0, sizeof(nas_indication_register_req_msg_v01));
/*---------------------------------------------------------------------------
  Get NAS service obj for encode/decode
----------------------------------------------------------------------------*/
  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  nas_service_object,
                  QMI_IDL_REQUEST,
                  QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &nas_ind_reg_req,
                  sizeof(nas_indication_register_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding nas indication register req message %d", ret_val, 0 ,0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  Get client ID for fd
----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  Process sys info indication registrations
----------------------------------------------------------------------------*/
  if (nas_ind_reg_req.sys_info_valid)
  {
/*---------------------------------------------------------------------------
  If the req is to enable sys info indications process them acc.
----------------------------------------------------------------------------*/
    if (nas_ind_reg_req.sys_info == IND_ENABLE)
    {
/*---------------------------------------------------------------------------
  If the client has already enabled them, its a dup. Respond lcoally.
----------------------------------------------------------------------------*/
      if (qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active)
      {
        LOG_MSG_INFO1("Client fd %d already registered for sys info ind",active_fd,0,0);
        sys_info_reg = IND_REQ_NOT_REQUIRED;
      }
/*---------------------------------------------------------------------------
  If this is the first sys info ind reg for QMI IP, then send the req to modem.
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.nas_sys_info_ind_count == QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is first client for sys info ind",active_fd,0,0);
        sys_info_reg = IND_ENABLE;
      }
/*---------------------------------------------------------------------------
  If this is not the first sys info reg for QMI IP ( ie other clients have
  already registered) then respond lcoally and dont send to modem.
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.nas_sys_info_ind_count > QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Other clients have already registered to sys info event"
                      "Don't send to modem",0,0,0);
        sys_info_reg = IND_REQ_NOT_REQUIRED;
      }
    }
/*---------------------------------------------------------------------------
  Process sys info indication register disable
----------------------------------------------------------------------------*/
    else if (nas_ind_reg_req.sys_info == IND_DISABLE)
    {
/*---------------------------------------------------------------------------
  Dup req/ client has not enabled. respond locally with success
----------------------------------------------------------------------------*/
      if (!qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active)
      {
        LOG_MSG_INFO1("Client fd %d not registered for sys info ind",active_fd,0,0);
        sys_info_reg = IND_REQ_NOT_REQUIRED;
      }
/*---------------------------------------------------------------------------
  Last client disable sys info indication reg. so send req to modem.
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.nas_sys_info_ind_count == QMI_IP_ACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is the only client for sys info ind."
                      "Will send req to modem",0,0,0);
        sys_info_reg = IND_DISABLE;
      }
/*---------------------------------------------------------------------------
  If there are still other clients registered. respond locally with success
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.nas_sys_info_ind_count > QMI_IP_ACTIVE)
      {
        LOG_MSG_INFO1("Other clients have registered for sys info event."
                      "Don't send to modem",0,0,0);
        sys_info_reg = IND_REQ_NOT_REQUIRED;
      }
    }
  }

/*---------------------------------------------------------------------------
  Process embms status indication registrations
----------------------------------------------------------------------------*/
  if (nas_ind_reg_req.reg_embms_status_valid)
  {
/*---------------------------------------------------------------------------
  If the req is for enable process them acc
----------------------------------------------------------------------------*/
    if (nas_ind_reg_req.reg_embms_status == IND_ENABLE)
    {
/*---------------------------------------------------------------------------
  Dup req. Respond locally with success
----------------------------------------------------------------------------*/
      if (qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active)
      {
        LOG_MSG_INFO1("Client fd %d already registered for embms status ind",active_fd,0,0);
        embms_status_reg = IND_REQ_NOT_REQUIRED;
      }
/*---------------------------------------------------------------------------
  First req to enable embms status ind. send it to modem.
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_status_ind_count == QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is first client for embms status ind",active_fd,0,0);
        embms_status_reg = IND_ENABLE;
      }
/*---------------------------------------------------------------------------
  Not the first req to enable the ind. Dont send to modem. Respond locally
  with success.
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_status_ind_count > QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Other clients have already registered to embms status event."
                      "Don't send to modem",0,0,0);
        embms_status_reg = IND_REQ_NOT_REQUIRED;
      }
    }
/*---------------------------------------------------------------------------
  If the req is to disable indication
----------------------------------------------------------------------------*/
    else if (nas_ind_reg_req.reg_embms_status == IND_DISABLE)
    {
/*---------------------------------------------------------------------------
  Dup req/ client has not enabled. respond locally with success.
----------------------------------------------------------------------------*/
      if (!qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active)
      {
        LOG_MSG_INFO1("Client fd %d not registered for embms status ind",active_fd,0,0);
        embms_status_reg = IND_REQ_NOT_REQUIRED;
      }
/*---------------------------------------------------------------------------
  If its the last disable. Send to modem.
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_status_ind_count == QMI_IP_ACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is the only client for embms status ind."
                      "Will sned req to modem",0,0,0);
        embms_status_reg = IND_DISABLE;
      }
/*---------------------------------------------------------------------------
  If its not the last disable. Dont send to modem. Respond locally with success
----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_status_ind_count > QMI_IP_ACTIVE)
      {
        LOG_MSG_INFO1("Other clients have registered for embms status event."
                      "Don't send to modem",0,0,0);
        embms_status_reg = IND_REQ_NOT_REQUIRED;
      }
    }
  }

/*---------------------------------------------------------------------------
  Check if a QMI message needs to be sent to modem checking the status of
  sys info and embms status indication registrations.
----------------------------------------------------------------------------*/
  if ((sys_info_reg != IND_REQ_NOT_REQUIRED) ||
      (embms_status_reg != IND_REQ_NOT_REQUIRED))
  {
/*-----------------------------------------------------------------------------
  Populate the request based on the statuses processed above
-----------------------------------------------------------------------------*/
    memset(&nas_indication_reg_req, 0, sizeof(nas_indication_register_req_msg_v01));
    memset(msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
           0,
           MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

/*---------------------------------------------------------------------------
  Check if nas sys info needs to be sent to modem
----------------------------------------------------------------------------*/
    if ((sys_info_reg == IND_ENABLE) || (sys_info_reg == IND_DISABLE))
    {
      nas_indication_reg_req.sys_info_valid = TRUE;
      nas_indication_reg_req.sys_info = sys_info_reg;
    }
/*---------------------------------------------------------------------------
  Check if embms status ind needs to be sent to modem
----------------------------------------------------------------------------*/
    if ((embms_status_reg == IND_ENABLE) || (embms_status_reg == IND_DISABLE))
    {
      nas_indication_reg_req.reg_embms_status_valid = TRUE;
      nas_indication_reg_req.reg_embms_status = embms_status_reg;
    }

/*---------------------------------------------------------------------------
  Encode the req
----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_encode(
                  nas_service_object,
                  QMI_IDL_REQUEST,
                  QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                  &nas_indication_reg_req,
                  sizeof(nas_indication_register_req_msg_v01),
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding ind req request message %d", ret_val, 0 ,0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                           qmi_ip_conf->qmi_info.qmi_ip_nas_handle,
                           QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                           msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           msg_len,
                           resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending ind reg message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                  nas_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_NAS_INDICATION_REGISTER_RESP_MSG_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &nas_indication_reg_resp,
                  sizeof(nas_indication_register_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding ind reg resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }
    if (nas_indication_reg_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in performing ind reg on modem. err %d",
                    nas_indication_reg_resp.resp.error, 0, 0);
      return FAILURE;
    }
  }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
  memset(&nas_ind_reg_resp, 0, sizeof(nas_indication_register_resp_msg_v01));
  memset(resp_buf, 0, MAX_QMI_IP_PKT_SIZE);
  nas_ind_reg_resp.resp.result = QMI_RESULT_SUCCESS_V01;
  nas_ind_reg_resp.resp.error = QMI_ERR_NONE_V01;

  ret_val = qmi_idl_message_encode(
                nas_service_object,
                QMI_IDL_RESPONSE,
                QMI_NAS_INDICATION_REGISTER_RESP_MSG_V01,
                &nas_ind_reg_resp,
                sizeof(nas_indication_register_resp_msg_v01),
                resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                resp_msg_len);
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding config embms response message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Update the status of nas sys info ind for the client
----------------------------------------------------------------------------*/
  if (nas_ind_reg_req.sys_info_valid)
  {
    if (nas_ind_reg_req.sys_info == IND_ENABLE &&
        !qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active = TRUE;
      qmi_ip_conf->qmi_info.nas_sys_info_ind_count++;
    }
    else if (nas_ind_reg_req.sys_info == IND_DISABLE &&
             qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active = FALSE;
      qmi_ip_conf->qmi_info.nas_sys_info_ind_count--;
    }

    LOG_MSG_INFO1("NAS sys info ind status for client %d is %d",
                  client_id,
                  qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active,
                  0);
    LOG_MSG_INFO1("NAS sys info count %d",
                  qmi_ip_conf->qmi_info.nas_sys_info_ind_count, 0, 0);
  }

/*---------------------------------------------------------------------------
  Update the status of embms status ind for the client
----------------------------------------------------------------------------*/
  if (nas_ind_reg_req.reg_embms_status_valid)
  {
    if (nas_ind_reg_req.reg_embms_status == IND_ENABLE &&
        !qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active = TRUE;
      qmi_ip_conf->qmi_info.embms_status_ind_count++;
    }
    else if (nas_ind_reg_req.reg_embms_status == IND_DISABLE &&
             qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active = FALSE;
      qmi_ip_conf->qmi_info.embms_status_ind_count--;
    }

    LOG_MSG_INFO1("NAS embms ind status for client %d is %d",
                  client_id,
                  qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active,
                  0);
    LOG_MSG_INFO1("NAS embms status ind count %d",
                  qmi_ip_conf->qmi_info.embms_status_ind_count, 0, 0);
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_MESSAGE()

DESCRIPTION
  Processes NAS QMI message from clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
===========================================================================*/

int qmi_ip_proccess_nas_message
(
  int    active_fd,
  char   *msg,
  int    msg_len,
  int    message_id,
  char   *resp_buf,
  int    *resp_msg_len
)
{
  int                                      ret_val;
  int                                      error_code = QMI_ERR_NONE_V01;

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Process QMI IP supported NAS messages. Some need special processing
  per client. Some dont.
----------------------------------------------------------------------------*/
  switch (message_id)
  {
    case QMI_NAS_CONFIG_EMBMS_REQ_MSG_V01:
    {
      ret_val = qmi_ip_proccess_nas_config_embms_message(active_fd,
                                                         msg,
                                                         msg_len,
                                                         resp_buf,
                                                         resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      return SUCCESS;
    }

    case QMI_NAS_GET_EMBMS_STATUS_REQ_MSG_V01:
    {
      ret_val = qmi_ip_proccess_nas_get_embms_status_message(active_fd,
                                                             msg,
                                                             msg_len,
                                                             resp_buf,
                                                             resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      return SUCCESS;
    }

    case QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01:
    {
      ret_val = qmi_ip_proccess_nas_ind_reg_message(active_fd,
                                                    msg,
                                                    msg_len,
                                                    resp_buf,
                                                    resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      return SUCCESS;
    }

/*---------------------------------------------------------------------------
  Below messages do not need special processing based on embms client and
  can be sent to modem directly.
----------------------------------------------------------------------------*/
    case QMI_NAS_GET_SYS_INFO_REQ_MSG_V01:
    case QMI_NAS_GET_EMBMS_SIG_EXT_REQ_MSG_V01:
    case QMI_NAS_GET_EMBMS_SIG_REQ_MSG_V01:
    case QMI_NAS_GET_LTE_SIB16_NETWORK_TIME_REQ_MSG_V01:
      break;

/*---------------------------------------------------------------------------
  If we receive a QMI NAS message not supported by QMI IP respond with error
----------------------------------------------------------------------------*/
    default:
      LOG_MSG_ERROR("Invalid NAS QMI message received", 0, 0, 0);
      error_code = QMI_ERR_MALFORMED_MSG_V01;
      goto send_error;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  Send the QMI message to modem and get the response
----------------------------------------------------------------------------*/
  ret_val =  qmi_client_send_raw_msg_sync (qmi_ip_conf->qmi_info.qmi_ip_nas_handle,
                                           message_id,
                                           msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                           msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                           resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                           QMI_MAX_STD_MSG_SIZE,
                                           resp_msg_len,
                                           QMI_TIMEOUT);
  if (ret_val == QMI_NO_ERR)
  {
    LOG_MSG_INFO1("Successfully encoded response message. Resp len %d", *resp_msg_len, 0 ,0);
    return SUCCESS;
  }
  else
  {
    LOG_MSG_ERROR("Error in encoding response message. ret_val %d", ret_val, 0 ,0);
  }

/*---------------------------------------------------------------------------
  Handle error case
----------------------------------------------------------------------------*/
send_error:

  ret_val = qmi_idl_encode_resp_tlv(
               QMI_RESULT_FAILURE_V01,
               error_code,
               resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
               MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

  if (ret_val != QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding response error message. ret_val %d", ret_val, 0 ,0);
    return FAILURE;
  }
  else
  {
    *resp_msg_len = QMI_RESP_TLV_LEN;
    LOG_MSG_INFO1("Successfully encoded response error message", 0, 0 ,0);
    return SUCCESS;
  }
}



/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_LIST_QUERY_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS TMGI list query
  QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
==========================================================================*/
int qmi_ip_proccess_wds_tmgi_list_query_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type              wds_service_object;
  wds_embms_tmgi_list_query_req_msg_v01    tmgi_list_query_req;
  wds_embms_tmgi_list_query_resp_msg_v01   tmgi_list_query_resp;
  int                                      i,j;
  int                                      ret_val;

/*-------------------------------------------------------------------------*/

  memset(&tmgi_list_query_req, 0, sizeof(wds_embms_tmgi_list_query_req_msg_v01));

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_EMBMS_TMGI_LIST_QUERY_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &tmgi_list_query_req,
                  sizeof(wds_embms_tmgi_list_query_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding TMGI list query req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Check if req is for active or available TMGI. If req is for available TMGI
  send the req to modem. If the req is for active TMGI respond locally based
  on the TMGIs activated by client.
-----------------------------------------------------------------------------*/
  LOG_MSG_INFO1("Req list type is %d",
                 tmgi_list_query_req.list_type,
                 0, 0);

/*-----------------------------------------------------------------------------
  If the req is for available TMGI send to modem
-----------------------------------------------------------------------------*/
  if (tmgi_list_query_req.list_type == WDS_EMBMS_TMGI_LIST_AVAILABLE_V01)
  {
    LOG_MSG_INFO1("Available TMGI list query. Will send to modem", 0, 0, 0);
/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                             qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                             QMI_WDS_EMBMS_TMGI_LIST_QUERY_REQ_V01,
                             msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending TMGI list query message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_EMBMS_TMGI_LIST_QUERY_RESP_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &tmgi_list_query_resp,
                  sizeof(wds_embms_tmgi_list_query_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding tmgi list query resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }
    if (tmgi_list_query_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in processing TMGI list query on modem. err %d",
                    tmgi_list_query_resp.resp.error, 0, 0);
      return FAILURE;
    }
  }
/*-----------------------------------------------------------------------------
  If the req is for active TMGI list respond locally based on per client TMGI
  table maintained locally.
-----------------------------------------------------------------------------*/
  else if (tmgi_list_query_req.list_type == WDS_EMBMS_TMGI_LIST_ACTIVE_V01)
  {
    LOG_MSG_INFO1("Active TMGI list query. Respond locally", 0, 0, 0);

/*-----------------------------------------------------------------------------
  Populate the response locally by going through the list maintained locally.
-----------------------------------------------------------------------------*/
    memset(&tmgi_list_query_resp, 0, sizeof(wds_embms_tmgi_list_query_resp_msg_v01));
    tmgi_list_query_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    tmgi_list_query_resp.resp.error = QMI_ERR_NONE_V01;

    for (i=0; i < qmi_ip_conf->current_tmgi_inst_cnt; i++)
    {
      for (j=0; j< qmi_ip_conf->tmgi_instance[i].ref_count; j++)
      {
        if (active_fd == qmi_ip_conf->tmgi_instance[i].client_fds[j])
        {
          tmgi_list_query_resp.tmgi_list_valid = 1;
          tmgi_list_query_resp.tmgi_list.list_type = WDS_EMBMS_TMGI_LIST_ACTIVE_V01;
          memcpy(tmgi_list_query_resp.tmgi_list.tmgi_list[tmgi_list_query_resp.tmgi_list.tmgi_list_len].tmgi,
                 qmi_ip_conf->tmgi_instance[i].client_tmgi.tmgi,
                 sizeof(qmi_ip_embms_tmgi_t));
          tmgi_list_query_resp.tmgi_list.tmgi_list_len++;

        }
      }
    }
/*-----------------------------------------------------------------------------
  Encode the QMI message.
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_EMBMS_TMGI_LIST_QUERY_RESP_V01,
                  &tmgi_list_query_resp,
                  sizeof(wds_embms_tmgi_list_query_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding TMGI list query response message %d", ret_val, 0 ,0);
      return FAILURE;
    }
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_STOP_NETWORK_INTERFACE_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS stop network interface
  QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_wds_stop_network_interface_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type              wds_service_object;
  wds_stop_network_interface_req_msg_v01   stop_network_interface_req;
  wds_stop_network_interface_resp_msg_v01  stop_network_interface_resp;
  wds_pkt_srvc_status_ind_msg_v01          pkt_srvc_ind;
  qmi_ip_ind_buf_t                         qmi_ip_ind;
  int                                      ret_val;
  int                                      client_id;

/*-------------------------------------------------------------------------*/

  memset(&stop_network_interface_req, 0, sizeof(wds_stop_network_interface_req_msg_v01));

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &stop_network_interface_req,
                  sizeof(wds_stop_network_interface_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding stop network interface req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Check if the packet data handle is correct.
-----------------------------------------------------------------------------*/
  LOG_MSG_INFO1("Packet data handle is %d",
                 stop_network_interface_req.pkt_data_handle,
                 0, 0);

  if(!(stop_network_interface_req.pkt_data_handle ==
       qmi_ip_conf->qmi_info.qmi_ip_wds_v4_call_handle))
  {
    LOG_MSG_ERROR("Stop network interface received for invalid handle", 0, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Get client ID based on fd
-----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Reset the SNI state per client
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].is_sni_active)
  {
    qmi_ip_conf->embms_client[client_id].is_sni_active = FALSE;
    if (qmi_ip_conf->qmi_info.sni_count != 0)
    {
      qmi_ip_conf->qmi_info.sni_count--;
    }

    LOG_MSG_INFO1("WDS SNI status for client %d is %d",
                  client_id,
                  qmi_ip_conf->embms_client[client_id].is_sni_active,
                  0);
    LOG_MSG_INFO1("WDS SNI count %d",
                  qmi_ip_conf->qmi_info.sni_count, 0, 0);
  }

/*-----------------------------------------------------------------------------
  If this is the last stop network interface send the req to modem
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->qmi_info.sni_count == QMI_IP_INACTIVE)
  {
    LOG_MSG_INFO1("Last stop network interface. Will send to modem", 0, 0, 0);
/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                             qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                             QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01,
                             msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending stop network interface message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &stop_network_interface_resp,
                  sizeof(wds_stop_network_interface_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding stop network interface resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }
    if (stop_network_interface_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in performing stop network interface on modem. err %d",
                    stop_network_interface_resp.resp.error, 0, 0);
      return FAILURE;
    }
  }
/*-----------------------------------------------------------------------------
  If this is not the last stop network interface respond locally
-----------------------------------------------------------------------------*/
  else if (qmi_ip_conf->qmi_info.sni_count > QMI_IP_INACTIVE)
  {
    LOG_MSG_INFO1("Not last stop network interface. Respond locally", 0, 0, 0);

/*-----------------------------------------------------------------------------
  Populate the indication message and send to indication handler socket
-----------------------------------------------------------------------------*/
    memset(&pkt_srvc_ind, 0, sizeof(wds_pkt_srvc_status_ind_msg_v01));
    pkt_srvc_ind.status.connection_status = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
    pkt_srvc_ind.tech_name_valid = TRUE;
    pkt_srvc_ind.tech_name = WDS_TECHNOLOGY_NAME_EMBMS_V01;

    memset(&qmi_ip_ind, 0, sizeof(qmi_ip_ind));

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_PKT_SRVC_STATUS_IND_V01,
                  &pkt_srvc_ind,
                  sizeof(wds_pkt_srvc_status_ind_msg_v01),
                  &(qmi_ip_ind.ind_buf),
                  QMI_MAX_STD_MSG_SIZE,
                  &(qmi_ip_ind.ind_buf_len));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding pkt srvc ind message %d", ret_val, 0 ,0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Send to indication handler socket
-----------------------------------------------------------------------------*/
    qmi_ip_ind.client_id = active_fd;
    qmi_ip_ind.service_type = QMI_WDS_SERVICE_TYPE;
    qmi_ip_ind.message_id = QMI_WDS_PKT_SRVC_STATUS_IND_V01;
    ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Failure to send pkt srvc ind to ind socket", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
    memset(&stop_network_interface_resp, 0, sizeof(wds_stop_network_interface_resp_msg_v01));
    stop_network_interface_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    stop_network_interface_resp.resp.error = QMI_ERR_NONE_V01;

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01,
                  &stop_network_interface_resp,
                  sizeof(wds_stop_network_interface_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding stop network interface response message %d", ret_val, 0 ,0);
      return FAILURE;
    }
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_START_NETWORK_INTERFACE_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS start network interface
  QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_wds_start_network_interface_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type              wds_service_object;
  wds_start_network_interface_req_msg_v01  start_network_interface_req;
  wds_start_network_interface_resp_msg_v01 start_network_interface_resp;
  wds_pkt_srvc_status_ind_msg_v01          pkt_srvc_ind;
  qmi_ip_ind_buf_t                         qmi_ip_ind;
  int                                      ret_val;
  int                                      client_id;
  int                                      j = 0;
  int                                      error_code = QMI_ERR_NONE_V01;
/*-------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Get client ID by fd
-----------------------------------------------------------------------------*/
  client_id = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Validation check to make sure QMI IP client init message has been received
  prior to this.
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].init_done == FALSE)
  {
    LOG_MSG_ERROR("QMI IP Client Init Not Done. Can't send WDS message",0,0,0);
    error_code = QMI_ERR_INVALID_OPERATION_V01;
    return FAILURE;
  }

  memset(&start_network_interface_req, 0, sizeof(wds_start_network_interface_req_msg_v01));

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_START_NETWORK_INTERFACE_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &start_network_interface_req,
                  sizeof(wds_start_network_interface_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding start network interface req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Check if the call type is eMBMS
-----------------------------------------------------------------------------*/
  LOG_MSG_INFO1("Technology is %d",
                 start_network_interface_req.ext_technology_preference,
                 0, 0);

  if(!((start_network_interface_req.ext_technology_preference_valid) &&
      (start_network_interface_req.ext_technology_preference ==
       WDS_EXT_TECH_PREF_EMBMS_V01)))
  {
    LOG_MSG_ERROR("Technology preference set is not embms", 0, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  If its the first start network interface req send it to modem
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->qmi_info.sni_count == QMI_IP_INACTIVE)
  {
    for (j=0;j <= MAX_QMI_EMBD_INSTANCES; j++)
    {
      ret_val = qmi_ip_bind_mux_port(j);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in binding mux port",0,0,0);
        continue;
      }
      LOG_MSG_INFO1("First start network interface. Will send to modem", 0, 0, 0);
      /*-----------------------------------------------------------------------------
      Will send QMI message to modem
      -----------------------------------------------------------------------------*/
      ret_val = qmi_ip_send_message_to_modem(
                               qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                               QMI_WDS_START_NETWORK_INTERFACE_REQ_V01,
                               msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                               msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                               resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                               resp_msg_len);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in sending start network interface message to modem", 0, 0, 0);
        return FAILURE;
      }

      /*-----------------------------------------------------------------------------
      Decode the QMI message response
      -----------------------------------------------------------------------------*/
      ret_val = qmi_idl_message_decode(
                    wds_service_object,
                    QMI_IDL_RESPONSE,
                    QMI_WDS_START_NETWORK_INTERFACE_RESP_V01,
                    resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                    *resp_msg_len,
                    &start_network_interface_resp,
                    sizeof(wds_start_network_interface_resp_msg_v01));
      if (ret_val < QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in decoding start network interface resp message %d", ret_val, 0 ,0);
        return FAILURE;
      }

      if (start_network_interface_resp.resp.result != QMI_RESULT_SUCCESS_V01)
      {
        if (start_network_interface_resp.resp.error != QMI_ERR_POLICY_MISMATCH_V01)
        {
          LOG_MSG_ERROR("Error in performing start network interface on modem. err %d",
                        start_network_interface_resp.resp.error, 0, 0);
          break;
        }
        // We will iterate only if a call is already up on iface
        LOG_MSG_ERROR("Error %d in start network interface on modem.Call already up",
                      start_network_interface_resp.resp.error, 0, 0);
        continue;
      }
      else
        break;
    }
    if (start_network_interface_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in start network interface on modem. Mux ids not free",0,0,0);
      return FAILURE;
    }
    /*-----------------------------------------------------------------------------
      Store the packet data handle to send it to other embms clients later
    -----------------------------------------------------------------------------*/
    qmi_ip_conf->qmi_info.qmi_ip_wds_v4_call_handle = start_network_interface_resp.pkt_data_handle;
  }
/*-----------------------------------------------------------------------------
  If its not the first start network interface respond locally.
-----------------------------------------------------------------------------*/
  else if (qmi_ip_conf->qmi_info.sni_count > QMI_IP_INACTIVE)
  {
    LOG_MSG_INFO1("Start network interface already done on modem. Respond locally", 0, 0, 0);

/*-----------------------------------------------------------------------------
  Populate the indication message and send to indication handler socket
-----------------------------------------------------------------------------*/
    memset(&pkt_srvc_ind, 0, sizeof(wds_pkt_srvc_status_ind_msg_v01));
    pkt_srvc_ind.status.connection_status = WDS_CONNECTION_STATUS_CONNECTED_V01;
    pkt_srvc_ind.status.reconfiguration_required = 0;
    pkt_srvc_ind.tech_name_valid = TRUE;
    pkt_srvc_ind.tech_name = WDS_TECHNOLOGY_NAME_EMBMS_V01;

    memset(&qmi_ip_ind, 0, sizeof(qmi_ip_ind));

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_PKT_SRVC_STATUS_IND_V01,
                  &pkt_srvc_ind,
                  sizeof(wds_pkt_srvc_status_ind_msg_v01),
                  &(qmi_ip_ind.ind_buf),
                  QMI_MAX_STD_MSG_SIZE,
                  &(qmi_ip_ind.ind_buf_len));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding pkt srvc ind message %d", ret_val, 0 ,0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Send to indication handler socket
-----------------------------------------------------------------------------*/
    qmi_ip_ind.client_id = active_fd;
    qmi_ip_ind.service_type = QMI_WDS_SERVICE_TYPE;
    qmi_ip_ind.message_id = QMI_WDS_PKT_SRVC_STATUS_IND_V01;
    ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Failure to send pkt srvc ind to ind socket", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
    memset(&start_network_interface_resp, 0, sizeof(wds_start_network_interface_resp_msg_v01));
    start_network_interface_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    start_network_interface_resp.resp.error = QMI_ERR_NONE_V01;
    start_network_interface_resp.pkt_data_handle = qmi_ip_conf->qmi_info.qmi_ip_wds_v4_call_handle;

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_START_NETWORK_INTERFACE_RESP_V01,
                  &start_network_interface_resp,
                  sizeof(wds_start_network_interface_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding start network interface response message %d", ret_val, 0 ,0);
      return FAILURE;
    }
  }

/*-----------------------------------------------------------------------------
  Update the start network interface status per client.
-----------------------------------------------------------------------------*/
  if (!qmi_ip_conf->embms_client[client_id].is_sni_active)
  {
    qmi_ip_conf->embms_client[client_id].is_sni_active = TRUE;
    qmi_ip_conf->qmi_info.sni_count++;
  }

  LOG_MSG_INFO1("WDS SNI status for client %d is %d",
                 client_id,
                 qmi_ip_conf->embms_client[client_id].is_sni_active,
                 0);
  LOG_MSG_INFO1("WDS SNI count %d",
                 qmi_ip_conf->qmi_info.sni_count, 0, 0);

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_DEACTIVATE_ALL_TUNNELS()

DESCRIPTION
  This function handles the processing of deactivating all tunnels
  when embms call gets disconnected.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
=========================================================================*/
int qmi_ip_process_deactivate_all_tunnels()
{

  int                        i,j;
  int                        ret_val;
/*-----------------------------------------------------------------------*/

  LOG_MSG_INFO1("Will delete all tunnels", 0, 0, 0);
/*---------------------------------------------------------------------------
  Deactivate each tmgi
----------------------------------------------------------------------------*/
  for (i = 0; i < qmi_ip_conf->current_tmgi_inst_cnt; i++)
  {
    for (j = 0; j < qmi_ip_conf->tmgi_instance[i].ref_count; j++)
    {
      ret_val = qmi_ip_tunnel_deletion(qmi_ip_conf->tmgi_instance[i].client_fds[j],
                                       &qmi_ip_conf->tmgi_instance[i].client_tmgi);
      if (ret_val != SUCCESS)
      {
        LOG_MSG_ERROR("Failed to delete tunnel inst for tmgi", 0, 0, 0);
      }
    }
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_DEACTIVATE_CLIENT()

DESCRIPTION
  This function handles the processing of deactivating client associated
  QMI functionalities: deactivating TMGIs, stop network interface,
  disabling eMBMS functionalities specific to a client.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
=========================================================================*/
int qmi_ip_process_deactivate_client
(
  int   client_fd
)
{
  char                                     msg[MAX_QMI_IP_PKT_SIZE];
  qmi_idl_service_object_type              wds_service_object;
  qmi_idl_service_object_type              nas_service_object;
  wds_embms_tmgi_deactivate_req_msg_v01    tmgi_deactivate_req;
  wds_embms_tmgi_deactivate_resp_msg_v01   tmgi_deactivate_resp;
  nas_config_embms_req_msg_v01             nas_config_embms_req;
  nas_config_embms_resp_msg_v01            nas_config_embms_resp;
  nas_indication_register_req_msg_v01      nas_ind_register_req;
  nas_indication_register_resp_msg_v01     nas_ind_register_resp;
  wds_indication_register_req_msg_v01      wds_ind_reg_req;
  wds_indication_register_resp_msg_v01     wds_ind_reg_resp;
  wds_stop_network_interface_req_msg_v01   wds_sni_req;
  wds_stop_network_interface_resp_msg_v01  wds_sni_resp;
  int                                      i,j;
  int                                      req_msg_len;
  char                                     resp_buf[MAX_QMI_IP_PKT_SIZE];
  int                                      resp_msg_len;
  int                                      ret_val;
  int                                      client_id;

/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }

  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Get client ID based on fd
-----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(client_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", client_fd, 0, 0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Handle the deactivate of TMGIs for the the client
-----------------------------------------------------------------------------*/
  for (i=0; i < qmi_ip_conf->current_tmgi_inst_cnt; i++)
  {
    for (j=0; j < qmi_ip_conf->tmgi_instance[i].ref_count; j++)
    {
      if (qmi_ip_conf->tmgi_instance[i].client_fds[j] == client_fd)
      {
        memset(&tmgi_deactivate_req,
               0,
               sizeof(wds_embms_tmgi_deactivate_req_msg_v01));

        memset(&tmgi_deactivate_resp,
               0,
               sizeof(wds_embms_tmgi_deactivate_resp_msg_v01));

        req_msg_len = 0;
        resp_msg_len = 0;

        memset(msg,
               0,
               MAX_QMI_IP_PKT_SIZE);

        memset(resp_buf,
               0,
               MAX_QMI_IP_PKT_SIZE);

        LOG_MSG_INFO1("Will deactivate TMGI index %d for client fd %d", i, client_fd, 0);

        memcpy(tmgi_deactivate_req.tmgi.tmgi,
               qmi_ip_conf->tmgi_instance[i].client_tmgi.tmgi,
               sizeof(qmi_ip_embms_tmgi_t));

/*-----------------------------------------------------------------------------
  Encode the deactivate TMGI
-----------------------------------------------------------------------------*/
        ret_val = qmi_idl_message_encode(
                      wds_service_object,
                      QMI_IDL_REQUEST,
                      QMI_WDS_EMBMS_TMGI_DEACTIVATE_REQ_V01,
                      &tmgi_deactivate_req,
                      sizeof(wds_embms_tmgi_deactivate_req_msg_v01),
                      msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      &req_msg_len);
        if (ret_val < QMI_IDL_LIB_NO_ERR)
        {
          LOG_MSG_ERROR("Error in encoding TMGI deactivate req message %d", ret_val, 0 ,0);
        }

/*-----------------------------------------------------------------------------
  Take care of deactivate TMGI
-----------------------------------------------------------------------------*/
        ret_val = qmi_ip_proccess_wds_tmgi_deactivate_message(
                                            client_fd,
                                            msg,
                                            req_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                            &resp_buf,
                                            &resp_msg_len);
        if (ret_val != SUCCESS)
        {
          LOG_MSG_ERROR("Failed to desctivate TMGIs for client fd %d", client_fd, 0, 0);
        }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
        ret_val = qmi_idl_message_decode(
                      wds_service_object,
                      QMI_IDL_RESPONSE,
                      QMI_WDS_EMBMS_TMGI_DEACTIVATE_RESP_V01,
                      resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      resp_msg_len,
                      &tmgi_deactivate_resp,
                      sizeof(wds_embms_tmgi_deactivate_resp_msg_v01));
        if (ret_val < QMI_IDL_LIB_NO_ERR ||
            tmgi_deactivate_resp.resp.result == QMI_RESULT_FAILURE_V01)
        {
          LOG_MSG_ERROR("Error in TMGI deactivate resp message %d", ret_val, 0 ,0);
        }
      }
    }
  }


/*----------------------------------------------------------------------------
  Disable WDS messages for the client
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
  Deregister WDS indications
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active)
  {
    memset(&wds_ind_reg_req,
           0,
           sizeof(wds_indication_register_req_msg_v01));
    memset(&wds_ind_reg_resp,
           0,
           sizeof(wds_indication_register_resp_msg_v01));

    req_msg_len = 0;
    resp_msg_len = 0;

    memset(msg,
           0,
           MAX_QMI_IP_PKT_SIZE);

    memset(resp_buf,
           0,
           MAX_QMI_IP_PKT_SIZE);
/*-----------------------------------------------------------------------------
  Populate the indication messsage
-----------------------------------------------------------------------------*/
    wds_ind_reg_req.report_embms_tmgi_list_valid = 1;
    wds_ind_reg_req.report_embms_tmgi_list = 0;
    ret_val = qmi_idl_message_encode(wds_service_object,
                                     QMI_IDL_REQUEST,
                                     QMI_WDS_INDICATION_REGISTER_REQ_V01,
                                     &wds_ind_reg_req,
                                     sizeof(wds_indication_register_req_msg_v01),
                                     msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     &req_msg_len);

    if(ret_val != QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("QMI message encode failed for WDS ind reg %d", ret_val, 0, 0);
    }

/*----------------------------------------------------------------------------
  Take care of WDS ind disable
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_proccess_wds_ind_reg_message(
                                            client_fd,
                                            msg,
                                            req_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                            &resp_buf,
                                            &resp_msg_len);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed to deregister WDS ind for client fd %d", client_fd, 0, 0);
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                      wds_service_object,
                      QMI_IDL_RESPONSE,
                      QMI_WDS_INDICATION_REGISTER_RESP_V01,
                      resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      resp_msg_len,
                      &wds_ind_reg_resp,
                      sizeof(wds_indication_register_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR ||
        wds_ind_reg_resp.resp.result == QMI_RESULT_FAILURE_V01)
    {
      LOG_MSG_ERROR("Error in decoding NAS config embms resp message %d", ret_val, 0 ,0);
    }
  }

/*-----------------------------------------------------------------------------
  Perform stop network interface on behalf of client
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].is_sni_active)
  {
    memset(&wds_sni_req,
           0,
           sizeof(wds_stop_network_interface_req_msg_v01));
    memset(&wds_sni_resp,
           0,
           sizeof(wds_stop_network_interface_resp_msg_v01));

    req_msg_len = 0;
    resp_msg_len = 0;

    memset(msg,
           0,
           MAX_QMI_IP_PKT_SIZE);

    memset(resp_buf,
           0,
           MAX_QMI_IP_PKT_SIZE);
/*-----------------------------------------------------------------------------
  Populate the stop network interface messsage
-----------------------------------------------------------------------------*/
    wds_sni_req.pkt_data_handle = qmi_ip_conf->qmi_info.qmi_ip_wds_v4_call_handle;
    ret_val = qmi_idl_message_encode(wds_service_object,
                                     QMI_IDL_REQUEST,
                                     QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01,
                                     &wds_sni_req,
                                     sizeof(wds_stop_network_interface_req_msg_v01),
                                     msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     &req_msg_len);

    if(ret_val != QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("QMI message encode failed for WDS stop network interface %d", ret_val, 0, 0);
    }

/*----------------------------------------------------------------------------
  Process stop network interface message
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_proccess_wds_stop_network_interface_message(
                                            client_fd,
                                            msg,
                                            req_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                            &resp_buf,
                                            &resp_msg_len);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed to stop network interface for client fd %d", client_fd, 0, 0);
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                      wds_service_object,
                      QMI_IDL_RESPONSE,
                      QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01,
                      resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      resp_msg_len,
                      &wds_sni_resp,
                      sizeof(wds_stop_network_interface_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR ||
        wds_sni_resp.resp.result == QMI_RESULT_FAILURE_V01)
    {
      LOG_MSG_ERROR("Error in WDS stop network interface resp message %d",
                    ret_val,
                    wds_sni_resp.resp.error,
                    0);
    }
  }


/*---------------------------------------------------------------------------------
  Deregister NAS messages
----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
  Deregister NAS sys info and embms status indications
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active ||
      qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active)
  {
    memset(&nas_ind_register_req,
           0,
           sizeof(nas_indication_register_req_msg_v01));
    memset(&nas_ind_register_resp,
           0,
           sizeof(nas_indication_register_resp_msg_v01));

    req_msg_len = 0;
    resp_msg_len = 0;

    memset(msg,
           0,
           MAX_QMI_IP_PKT_SIZE);

    memset(resp_buf,
           0,
           MAX_QMI_IP_PKT_SIZE);

/*-----------------------------------------------------------------------------
  Populate the indication messsage
-----------------------------------------------------------------------------*/
    if (qmi_ip_conf->embms_client[client_id].is_embms_status_ind_active)
    {
      nas_ind_register_req.reg_embms_status_valid =1;
      nas_ind_register_req.reg_embms_status =0;
    }
    if (qmi_ip_conf->embms_client[client_id].is_nas_sys_info_ind_active)
    {
      nas_ind_register_req.sys_info_valid = 1;
      nas_ind_register_req.sys_info = 0;
    }
    ret_val = qmi_idl_message_encode(nas_service_object,
                                     QMI_IDL_REQUEST,
                                     QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                     &nas_ind_register_req,
                                     sizeof(nas_indication_register_req_msg_v01),
                                     msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     &req_msg_len);

    if(ret_val != QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("QMI message encode failed for NAS indication register %d", ret_val, 0, 0);
    }

/*----------------------------------------------------------------------------
  Take care of NAS ind disable
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_proccess_nas_ind_reg_message(
                                            client_fd,
                                            msg,
                                            req_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                            &resp_buf,
                                            &resp_msg_len);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed in NAS ind reg for client fd %d", client_fd, 0, 0);
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                      nas_service_object,
                      QMI_IDL_RESPONSE,
                      QMI_NAS_INDICATION_REGISTER_RESP_MSG_V01,
                      resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      resp_msg_len,
                      &nas_ind_register_resp,
                      sizeof(nas_indication_register_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR ||
        nas_ind_register_resp.resp.result == QMI_RESULT_FAILURE_V01)
    {
      LOG_MSG_ERROR("Error in NAS ind reg resp message %d", ret_val, 0 ,0);
    }
  }

/*-----------------------------------------------------------------------------
  Perform the embms config disable on behalf of client
-----------------------------------------------------------------------------*/
  if (qmi_ip_conf->embms_client[client_id].is_nce_active)
  {
    memset(&nas_config_embms_req,
           0,
           sizeof(nas_config_embms_req_msg_v01));
    memset(&nas_config_embms_resp,
           0,
           sizeof(nas_config_embms_resp_msg_v01));

    req_msg_len = 0;
    resp_msg_len = 0;

    memset(msg,
           0,
           MAX_QMI_IP_PKT_SIZE);

    memset(resp_buf,
           0,
           MAX_QMI_IP_PKT_SIZE);
/*-----------------------------------------------------------------------------
  Populate the messsage
-----------------------------------------------------------------------------*/
    nas_config_embms_req.enable = 0;
    ret_val = qmi_idl_message_encode(nas_service_object,
                                     QMI_IDL_REQUEST,
                                     QMI_NAS_CONFIG_EMBMS_REQ_MSG_V01,
                                     &nas_config_embms_req,
                                     sizeof(nas_config_embms_req_msg_v01),
                                     msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                     &req_msg_len);

    if(ret_val != QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("QMI message encode failed for NAS config embms %d", ret_val, 0, 0);
    }

/*----------------------------------------------------------------------------
  Take care of NAS config embms disable
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_proccess_nas_config_embms_message(
                                            client_fd,
                                            msg,
                                            req_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                            &resp_buf,
                                            &resp_msg_len);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed to disable NAS config embms for client fd %d", client_fd, 0, 0);
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                      nas_service_object,
                      QMI_IDL_RESPONSE,
                      QMI_NAS_CONFIG_EMBMS_RESP_MSG_V01,
                      resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                      resp_msg_len,
                      &nas_config_embms_resp,
                      sizeof(nas_config_embms_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR ||
        nas_config_embms_resp.resp.result == QMI_RESULT_FAILURE_V01)
    {
      LOG_MSG_ERROR("Error in NAS config embms resp message %d", ret_val, 0 ,0);
    }
  }

  return SUCCESS;
}
/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_ACT_DEACT_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS TMGI act_deact QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
=========================================================================*/
int qmi_ip_proccess_wds_tmgi_act_deact_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type              wds_service_object;
  wds_embms_tmgi_act_deact_req_msg_v01     tmgi_act_deact_req;
  wds_embms_tmgi_act_deact_resp_msg_v01    tmgi_act_deact_resp;
  uint32                                   multicast_ip;
  uint16                                   multicast_port;
  int                                      ret_val;
  int                                      tmgi_index;
  wds_embms_tmgi_activate_req_msg_v01      tmgi_activate_req;
  wds_embms_tmgi_activate_resp_msg_v01     tmgi_activate_resp;
  wds_embms_tmgi_deactivate_req_msg_v01    tmgi_deactivate_req;
  wds_embms_tmgi_deactivate_resp_msg_v01   tmgi_deactivate_resp;
  char                                     resp_buffer[MAX_QMI_IP_PKT_SIZE];
  int                                      request_msg_len;
  int                                      response_msg_len;
/*-------------------------------------------------------------------------*/

  memset(&tmgi_act_deact_req, 0, sizeof(wds_embms_tmgi_act_deact_req_msg_v01));
  memset(&tmgi_act_deact_resp, 0, sizeof(wds_embms_tmgi_act_deact_resp_msg_v01));
  memset(&tmgi_activate_req, 0, sizeof(wds_embms_tmgi_activate_req_msg_v01));
  memset(&tmgi_activate_resp, 0, sizeof(wds_embms_tmgi_activate_resp_msg_v01));
  memset(&tmgi_deactivate_req, 0, sizeof(wds_embms_tmgi_deactivate_req_msg_v01));
  memset(&tmgi_deactivate_resp, 0, sizeof(wds_embms_tmgi_deactivate_resp_msg_v01));

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_EMBMS_TMGI_ACT_DEACT_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &tmgi_act_deact_req,
                  sizeof(wds_embms_tmgi_act_deact_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding TMGI act_deact req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Handle the activate of TMGI
-----------------------------------------------------------------------------*/
  memset(msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
         0,
         MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);
  tmgi_activate_req.multicast_ip_type_valid =
    tmgi_act_deact_req.multicast_ip_type_valid;
  tmgi_activate_req.multicast_ip_type =
    tmgi_act_deact_req.multicast_ip_type;

  tmgi_activate_req.multicast_ipv4_addr_valid =
    tmgi_act_deact_req.multicast_ipv4_addr_valid;
  tmgi_activate_req.multicast_ipv4_addr =
    tmgi_act_deact_req.multicast_ipv4_addr;

  tmgi_activate_req.multicast_port_valid =
    tmgi_act_deact_req.multicast_port_valid;
  tmgi_activate_req.multicast_port =
    tmgi_act_deact_req.multicast_port;

  memcpy(&tmgi_activate_req.tmgi,
         &tmgi_act_deact_req.act_tmgi,
         sizeof(wds_embms_tmgi_type_v01));

/*-----------------------------------------------------------------------------
  Encode the activate TMGI
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_encode(
                wds_service_object,
                QMI_IDL_REQUEST,
                QMI_WDS_EMBMS_TMGI_ACTIVATE_REQ_V01,
                &tmgi_activate_req,
                sizeof(wds_embms_tmgi_activate_req_msg_v01),
                msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                &request_msg_len);
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding TMGI activate req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Take care of activate TMGI
-----------------------------------------------------------------------------*/
  ret_val = qmi_ip_proccess_wds_tmgi_activate_message(active_fd,
                                                      msg,
                                                      request_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                                      &resp_buffer,
                                                      &response_msg_len);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in activating TMGI for act_deact message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                wds_service_object,
                QMI_IDL_RESPONSE,
                QMI_WDS_EMBMS_TMGI_ACTIVATE_RESP_V01,
                resp_buffer + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                response_msg_len,
                &tmgi_activate_resp,
                sizeof(wds_embms_tmgi_activate_resp_msg_v01));
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding TMGI activate resp message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  if (tmgi_activate_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Error in activating TMGI.on modem err %d",
                  tmgi_activate_resp.resp.error, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Handle the deactivate of TMGI
-----------------------------------------------------------------------------*/

  memset(msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
         0,
         MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

  memcpy(&tmgi_deactivate_req.tmgi,
         &tmgi_act_deact_req.deact_tmgi,
         sizeof(wds_embms_tmgi_type_v01));

/*-----------------------------------------------------------------------------
  Encode the deactivate TMGI
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_encode(
                wds_service_object,
                QMI_IDL_REQUEST,
                QMI_WDS_EMBMS_TMGI_DEACTIVATE_REQ_V01,
                &tmgi_deactivate_req,
                sizeof(wds_embms_tmgi_deactivate_req_msg_v01),
                msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                &request_msg_len);
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding TMGI deactivate req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Take care of deactivate TMGI
-----------------------------------------------------------------------------*/
  ret_val = qmi_ip_proccess_wds_tmgi_deactivate_message(
                                      active_fd,
                                      msg,
                                      request_msg_len+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                      &resp_buffer,
                                      &response_msg_len);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in deactivating TMGI for act_deact message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                wds_service_object,
                QMI_IDL_RESPONSE,
                QMI_WDS_EMBMS_TMGI_DEACTIVATE_RESP_V01,
                resp_buffer + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                response_msg_len,
                &tmgi_deactivate_resp,
                sizeof(wds_embms_tmgi_deactivate_resp_msg_v01));
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding TMGI deactivate resp message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  if (tmgi_deactivate_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Error in deactivating TMGI.on modem err %d",
                  tmgi_deactivate_resp.resp.error, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
  memset(&tmgi_act_deact_resp, 0, sizeof(tmgi_act_deact_resp));
  tmgi_act_deact_resp.resp.result = QMI_RESULT_SUCCESS_V01;
  tmgi_act_deact_resp.resp.error = QMI_ERR_NONE_V01;

  ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_EMBMS_TMGI_ACT_DEACT_RESP_V01,
                  &tmgi_act_deact_resp,
                  sizeof(wds_embms_tmgi_act_deact_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding TMGI act_deact response message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Update the act_deact table which keeps track of activate and deactivate
  indications and proccesses them based on checking if they were part of
  act_deact message.
-----------------------------------------------------------------------------*/
  LOG_MSG_INFO1("Adding entry %d in act-deact table",
                qmi_ip_conf->act_deact_entry_count, 0, 0);
  qmi_ip_conf->act_deact_entry[qmi_ip_conf->act_deact_entry_count].fd = active_fd;
  memcpy(qmi_ip_conf->act_deact_entry[qmi_ip_conf->act_deact_entry_count].act_tmgi.tmgi,
         tmgi_act_deact_req.act_tmgi.tmgi,
         sizeof(wds_embms_tmgi_type_v01));
  memcpy(qmi_ip_conf->act_deact_entry[qmi_ip_conf->act_deact_entry_count].deact_tmgi.tmgi,
         tmgi_act_deact_req.act_tmgi.tmgi,
         sizeof(wds_embms_tmgi_type_v01));
  qmi_ip_conf->act_deact_entry[qmi_ip_conf->act_deact_entry_count].act_tmgi_ind_status = TRUE;
  qmi_ip_conf->act_deact_entry[qmi_ip_conf->act_deact_entry_count].deact_tmgi_ind_status = TRUE;
  qmi_ip_conf->act_deact_entry_count++;

  LOG_MSG_INFO1("Count in act-deact table %d",
                qmi_ip_conf->act_deact_entry_count, 0, 0);

  return SUCCESS;
}
/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_DEACTIVATE_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS TMGI deactivate QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_wds_tmgi_deactivate_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type              wds_service_object;
  wds_embms_tmgi_deactivate_req_msg_v01    tmgi_deactivate_req;
  wds_embms_tmgi_deactivate_resp_msg_v01   tmgi_deactivate_resp;
  wds_embms_tmgi_deactivate_ind_msg_v01    tmgi_deactivate_ind;
  qmi_ip_embms_tmgi_t                      temp_tmgi;
  qmi_ip_ind_buf_t                         qmi_ip_ind;
  int                                      ret_val;
  int                                      tmgi_index;
  int                                      client_id;
  FILE                                     *fp;

/*-------------------------------------------------------------------------*/

  memset(&temp_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
  memset(&tmgi_deactivate_req, 0, sizeof(wds_embms_tmgi_deactivate_req_msg_v01));
/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_EMBMS_TMGI_DEACTIVATE_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &tmgi_deactivate_req,
                  sizeof(wds_embms_tmgi_activate_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding TMGI deactivate req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("TMGI to be deactivated for client %d", active_fd, 0, 0);
  LOG_MSG_INFO1_6("TMGI to be deactivated %d %d %d %d %d %d",
                  tmgi_deactivate_req.tmgi.tmgi[0],
                  tmgi_deactivate_req.tmgi.tmgi[1],
                  tmgi_deactivate_req.tmgi.tmgi[2],
                  tmgi_deactivate_req.tmgi.tmgi[3],
                  tmgi_deactivate_req.tmgi.tmgi[4],
                  tmgi_deactivate_req.tmgi.tmgi[5]);
  memcpy(temp_tmgi.tmgi, tmgi_deactivate_req.tmgi.tmgi, sizeof(temp_tmgi.tmgi));

/*-----------------------------------------------------------------------------
  Check if TMGI is present in TMGI table. If no return failure
-----------------------------------------------------------------------------*/
  ret_val = qmi_ip_get_tmgi_id(&temp_tmgi);
  if (ret_val == FAILURE)
  {
    LOG_MSG_ERROR("Received TMGI deactivate for a non activated TMGI", 0, 0, 0);
    return FAILURE;
  }
  else
  {
    tmgi_index = ret_val;
/*-----------------------------------------------------------------------------
  Get client id based on fd
-----------------------------------------------------------------------------*/
    client_id = qmi_ip_get_client_id(active_fd);
    if (client_id == FAILURE)
    {
      LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Logging for demo.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_DEMO_LOGGING
    fp = fopen("/usr/bin/out_logs.txt", "a");
    if (fp != NULL)
    {
      fprintf(fp, "\nDevice: %s deactivating ",
              inet_ntoa(qmi_ip_conf->embms_client[client_id].client_ip));
      fprintf(fp, "channel#: %d, ",
              tmgi_deactivate_req.tmgi.tmgi[2]);
      fclose(fp);
    }
    else
    {
      LOG_MSG_ERROR("Could not open output log file for writing",0,0,0);
    }
#endif
/*-----------------------------------------------------------------------------
  If its the last deactivate for TMGI, send it to modem.
-----------------------------------------------------------------------------*/
    if(qmi_ip_conf->tmgi_instance[tmgi_index].ref_count == QMI_IP_ACTIVE)
    {
/*-----------------------------------------------------------------------------
    Deactivate TMGI in TMGI table
-----------------------------------------------------------------------------*/
      ret_val = qmi_ip_deactivate_tmgi_inst(active_fd,
                                            &temp_tmgi);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in deactivating TMGI in DB for client %d", active_fd, 0, 0);
        return FAILURE;
      }


      LOG_MSG_INFO1("Last deactivate for TMGI. Send message to modem", 0, 0, 0);
/*-----------------------------------------------------------------------------
    Will send QMI message to modem
-----------------------------------------------------------------------------*/
      ret_val = qmi_ip_send_message_to_modem(
                               qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                               QMI_WDS_EMBMS_TMGI_DEACTIVATE_REQ_V01,
                               msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                               msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                               resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                               resp_msg_len);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in sending TMGI deactivate message to modem", 0, 0, 0);
        return FAILURE;
      }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
      ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_EMBMS_TMGI_DEACTIVATE_RESP_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &tmgi_deactivate_resp,
                  sizeof(wds_embms_tmgi_deactivate_resp_msg_v01));
      if (ret_val < QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in decoding TMGI deactivate resp message %d", ret_val, 0 ,0);
        return FAILURE;
      }

      if (tmgi_deactivate_resp.resp.result != QMI_RESULT_SUCCESS_V01)
      {
        LOG_MSG_ERROR("Error in deactivating TMGI.on modem err %d",
                      tmgi_deactivate_resp.resp.error, 0, 0);
        return FAILURE;
      }
    }
/*-----------------------------------------------------------------------------
  If other clients have activated this TMGI, respond locally.
-----------------------------------------------------------------------------*/
    else if (qmi_ip_conf->tmgi_instance[tmgi_index].ref_count > QMI_IP_ACTIVE)
    {
      LOG_MSG_INFO1("Not the last deactivate for TMGI. Respond locally", 0, 0, 0);
/*-----------------------------------------------------------------------------
  Deactivate TMGI for client in TMGI table
-----------------------------------------------------------------------------*/
      ret_val = qmi_ip_deactivate_tmgi_inst(active_fd,
                                            &temp_tmgi);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in deactivating TMGI in DB for client %d", active_fd, 0, 0);
        return FAILURE;
      }

/*-----------------------------------------------------------------------------
  Populate the indication message and send to indication handler socket
-----------------------------------------------------------------------------*/
      memset(&tmgi_deactivate_ind, 0, sizeof(wds_embms_tmgi_deactivate_ind_msg_v01));
      tmgi_deactivate_ind.deactivate_status = WDS_TMGI_DEACTIVATE_SUCCESS_V01;
      memcpy(tmgi_deactivate_ind.tmgi.tmgi,
             temp_tmgi.tmgi,
             sizeof(qmi_ip_embms_tmgi_t));

      memset(&qmi_ip_ind, 0, sizeof(qmi_ip_ind));

      ret_val = qmi_idl_message_encode(
                    wds_service_object,
                    QMI_IDL_INDICATION,
                    QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01,
                    &tmgi_deactivate_ind,
                    sizeof(wds_embms_tmgi_deactivate_ind_msg_v01),
                    &(qmi_ip_ind.ind_buf),
                    QMI_MAX_STD_MSG_SIZE,
                    &(qmi_ip_ind.ind_buf_len));
      if (ret_val < QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in encoding TMGI deactivate ind message %d", ret_val, 0 ,0);
        return FAILURE;
      }

/*-----------------------------------------------------------------------------
  Send to indication handler socket
-----------------------------------------------------------------------------*/
      qmi_ip_ind.client_id = active_fd;
      qmi_ip_ind.service_type = QMI_WDS_SERVICE_TYPE;
      qmi_ip_ind.message_id = QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01;
      ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Failure to send TMGI deact ind to ind socket", 0, 0, 0);
        return FAILURE;
      }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
      memset(&tmgi_deactivate_resp, 0, sizeof(tmgi_deactivate_resp));
      tmgi_deactivate_resp.resp.result = QMI_RESULT_SUCCESS_V01;
      tmgi_deactivate_resp.resp.error = QMI_ERR_NONE_V01;

      ret_val = qmi_idl_message_encode(
                    wds_service_object,
                    QMI_IDL_RESPONSE,
                    QMI_WDS_EMBMS_TMGI_DEACTIVATE_RESP_V01,
                    &tmgi_deactivate_resp,
                    sizeof(wds_embms_tmgi_deactivate_resp_msg_v01),
                    resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                    MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                    resp_msg_len);
      if (ret_val < QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in encoding TMGI deactivate response message %d", ret_val, 0 ,0);
        return FAILURE;
      }
    }
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_ACTIVATE_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS TMGI activate QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_wds_tmgi_activate_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type              wds_service_object;
  wds_embms_tmgi_activate_req_msg_v01      tmgi_activate_req;
  wds_embms_tmgi_activate_resp_msg_v01     tmgi_activate_resp;
  wds_embms_tmgi_activate_ind_msg_v01      tmgi_activate_ind;
  qmi_ip_embms_tmgi_t                      temp_tmgi;
  uint32                                   multicast_ip;
  uint16                                   multicast_port;
  qmi_ip_ind_buf_t                         qmi_ip_ind;
  int                                      ret_val;
  int                                      i;
  boolean                                  sni_enabled = 0;
  int                                      client_id;
  FILE                                    *fp;
  struct in_addr                           mcast_ip;
  char                                    *tmgi_mcast_ip;
/*-------------------------------------------------------------------------*/

  memset(&temp_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
  memset(&tmgi_activate_req, 0, sizeof(wds_embms_tmgi_activate_req_msg_v01));
  memset(&tmgi_activate_resp, 0, sizeof(wds_embms_tmgi_activate_resp_msg_v01));

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_EMBMS_TMGI_ACTIVATE_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &tmgi_activate_req,
                  sizeof(wds_embms_tmgi_activate_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding TMGI activate req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Get client ID based on fd
-----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Check if SNI has been called for this client. If no return failure.
-----------------------------------------------------------------------------*/
  if (!qmi_ip_conf->embms_client[client_id].is_sni_active ||
      !qmi_ip_conf->qmi_info.net_iface_status)
  {
    LOG_MSG_ERROR("Start network interface not activated for client fd %d: %d, %d",
                  active_fd,
                  qmi_ip_conf->embms_client[client_id].is_sni_active,
                  qmi_ip_conf->qmi_info.net_iface_status);
    return FAILURE;
  }
/*-----------------------------------------------------------------------------
  Check the QMI IP mode and populate the multicast IP/port accordingly
-----------------------------------------------------------------------------*/
  if(qmi_ip_conf->mode == SINGLE_CLIENT_MODE)
  {
    multicast_ip = 0;
    multicast_port = 0;
  }
  else if (qmi_ip_conf->mode == MULTI_CLIENT_MODE &&
           tmgi_activate_req.multicast_ipv4_addr_valid &&
           tmgi_activate_req.multicast_port_valid)
  {
    multicast_ip = htonl(tmgi_activate_req.multicast_ipv4_addr);
    multicast_port = htons(tmgi_activate_req.multicast_port);
    LOG_MSG_INFO1("Multicast IPV4 address:", 0, 0, 0);
    IPV4_ADDR_MSG(multicast_ip);
    LOG_MSG_INFO1("Multicast port %d", multicast_port, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("QMI IP mode %d, multicast addr is valid %d, multicast port is valid %d",
                  qmi_ip_conf->mode,
                  tmgi_activate_req.multicast_ipv4_addr_valid,
                  tmgi_activate_req.multicast_port_valid);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Logging for demo.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_DEMO_LOGGING
  fp = fopen("/usr/bin/out_logs.txt", "a");
  if (fp != NULL)
  {
    fprintf(fp, "\nDevice: %s activating ",
            inet_ntoa(qmi_ip_conf->embms_client[client_id].client_ip));
    fprintf(fp, "channel#: %d, ",
            tmgi_activate_req.tmgi.tmgi[2]);
    mcast_ip.s_addr = multicast_ip;
    tmgi_mcast_ip = inet_ntoa(mcast_ip);
    fprintf(fp, "multicast IP:port=%s:%d \n",
                tmgi_mcast_ip, tmgi_activate_req.multicast_port);
    fclose(fp);
  }
  else
  {
    LOG_MSG_ERROR("Error writing in output log file",0,0,0);
  }
#endif
  LOG_MSG_INFO1("TMGI to be activated for client %d", active_fd, 0, 0);
  LOG_MSG_INFO1_6("TMGI to be activated %d %d %d %d %d %d",
                  tmgi_activate_req.tmgi.tmgi[0],
                  tmgi_activate_req.tmgi.tmgi[1],
                  tmgi_activate_req.tmgi.tmgi[2],
                  tmgi_activate_req.tmgi.tmgi[3],
                  tmgi_activate_req.tmgi.tmgi[4],
                  tmgi_activate_req.tmgi.tmgi[5]);

  memcpy(temp_tmgi.tmgi, tmgi_activate_req.tmgi.tmgi, sizeof(temp_tmgi.tmgi));
/*-----------------------------------------------------------------------------
  Check if TMGI is already in TMGI table. If no send to modem, if yes respond
  locally.
-----------------------------------------------------------------------------*/
  ret_val = qmi_ip_get_tmgi_id(&temp_tmgi);
  if (ret_val == FAILURE)
  {
    LOG_MSG_INFO1("TMGI not present in DB. Will initialize it", 0, 0, 0);
/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                             qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                             QMI_WDS_EMBMS_TMGI_ACTIVATE_REQ_V01,
                             msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                             resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending TMGI activate message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_EMBMS_TMGI_ACTIVATE_RESP_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &tmgi_activate_resp,
                  sizeof(wds_embms_tmgi_activate_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding TMGI activate resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }

    if (tmgi_activate_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in activating TMGI.on modem err %d",
                    tmgi_activate_resp.resp.error, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Activate TMGI in TMGI table
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_activate_tmgi_inst(active_fd,
                                        multicast_ip,
                                        multicast_port,
                                        &temp_tmgi);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in activating TMGI in DB for client %d", active_fd, 0, 0);
      return FAILURE;
    }

  }
/*-----------------------------------------------------------------------------
  TMGI already in table, respond locally.
-----------------------------------------------------------------------------*/
  else
  {
    LOG_MSG_INFO1("TMGI present in DB. Will add ref to client and respond", 0, 0, 0);
/*-----------------------------------------------------------------------------
  Activate TMGI for client
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_activate_tmgi_inst(active_fd,
                                        multicast_ip,
                                        multicast_port,
                                        &temp_tmgi);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in activating TMGI in DB for client %d", active_fd, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Populate the indication message and send to indication handler socket
-----------------------------------------------------------------------------*/
    memset(&tmgi_activate_ind, 0, sizeof(wds_embms_tmgi_activate_ind_msg_v01));
    tmgi_activate_ind.activate_status = WDS_TMGI_ACTIVATE_SUCCESS_V01;
    memcpy(tmgi_activate_ind.tmgi.tmgi,
           temp_tmgi.tmgi,
           sizeof(qmi_ip_embms_tmgi_t));

    memset(&qmi_ip_ind, 0, sizeof(qmi_ip_ind));

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01,
                  &tmgi_activate_ind,
                  sizeof(wds_embms_tmgi_activate_ind_msg_v01),
                  &(qmi_ip_ind.ind_buf),
                  QMI_MAX_STD_MSG_SIZE,
                  &(qmi_ip_ind.ind_buf_len));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding TMGI activate ind message %d", ret_val, 0 ,0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Send to indication handler socket
-----------------------------------------------------------------------------*/
    qmi_ip_ind.client_id = active_fd;
    qmi_ip_ind.service_type = QMI_WDS_SERVICE_TYPE;
    qmi_ip_ind.message_id = QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01;
    ret_val = qmi_ip_send_to_indication_socket(&qmi_ip_ind);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Failure to send TMGI act ind to ind socket", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
    memset(&tmgi_activate_resp, 0, sizeof(tmgi_activate_resp));
    tmgi_activate_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    tmgi_activate_resp.resp.error = QMI_ERR_NONE_V01;

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_EMBMS_TMGI_ACTIVATE_RESP_V01,
                  &tmgi_activate_resp,
                  sizeof(wds_embms_tmgi_activate_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding TMGI activate response message %d", ret_val, 0 ,0);
      return FAILURE;
    }
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_IND_REG_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS indication register QMI message.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_wds_ind_reg_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type             wds_service_object;
  int                                     ret_val;
  wds_indication_register_req_msg_v01     ind_reg_req;
  wds_indication_register_resp_msg_v01    ind_reg_resp;
  int                                     embms_list_reg = IND_REQ_NOT_REQUIRED;
  int                                     embms_sai_list_reg = IND_REQ_NOT_REQUIRED;
  int                                     i;
  int                                     client_id;
/*--------------------------------------------------------------------------*/
  memset(&ind_reg_req, 0, sizeof(wds_indication_register_req_msg_v01));

/*-----------------------------------------------------------------------------
  Get service obj for encode/decode.
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Decode the QMI message
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_INDICATION_REGISTER_REQ_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &ind_reg_req,
                  sizeof(wds_indication_register_req_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding wds indication register req message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Get client ID based on fd
-----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(active_fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid client fd passed %d", active_fd, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Process enable and disable of indication registration
-----------------------------------------------------------------------------*/
  if (ind_reg_req.report_embms_tmgi_list_valid)
  {
/*-----------------------------------------------------------------------------
  Process enable indication registration
-----------------------------------------------------------------------------*/
    if (ind_reg_req.report_embms_tmgi_list == IND_ENABLE)
    {
/*-----------------------------------------------------------------------------
  Dup req, respond locally
-----------------------------------------------------------------------------*/
      if (qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active)
      {
        LOG_MSG_INFO1("Client has already registered to embms list ind. Don't send to modem",0,0,0);
        embms_list_reg = IND_REQ_NOT_REQUIRED;
      }
/*-----------------------------------------------------------------------------
  If first indication register send to modem.
-----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_list_ind_count == QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is first client for embms list ind",active_fd,0,0);
        embms_list_reg = IND_ENABLE;
      }
/*-----------------------------------------------------------------------------
  If not first indication register respond locally.
-----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_list_ind_count > QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Other clients have already registered to embms list ind. Don't send to modem",0,0,0);
        embms_list_reg = IND_REQ_NOT_REQUIRED;
      }
    }
/*-----------------------------------------------------------------------------
  Process disable of indication registration
-----------------------------------------------------------------------------*/
    else if (ind_reg_req.report_embms_tmgi_list == IND_DISABLE)
    {
/*-----------------------------------------------------------------------------
  Dup req/ client has not enabled the ind.
-----------------------------------------------------------------------------*/
      if (!qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active)
      {
        LOG_MSG_INFO1("Client has not  registered to embms list ind. Don't send to modem",0,0,0);
        embms_list_reg = IND_REQ_NOT_REQUIRED;
      }
/*-----------------------------------------------------------------------------
  If last indication register then send to modem
-----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_list_ind_count == QMI_IP_ACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is the only client for embms list ind.will send req to modem",0,0,0);
        embms_list_reg = IND_DISABLE;
      }
/*-----------------------------------------------------------------------------
  If not last indication register respond locally
-----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_list_ind_count > QMI_IP_ACTIVE)
      {
        LOG_MSG_INFO1("Other clients have registered for embms list ind. Don't send to modem",0,0,0);
        embms_list_reg = IND_REQ_NOT_REQUIRED;
      }
    }
  }

/*-----------------------------------------------------------------------------
     Process enable and disable of SAI indication registration
-----------------------------------------------------------------------------*/
  if (ind_reg_req.report_embms_sai_list_valid)

  {
/*-----------------------------------------------------------------------------
   Process enable indication registration
-----------------------------------------------------------------------------*/
    if (ind_reg_req.report_embms_sai_list == IND_ENABLE)
    {
/*-----------------------------------------------------------------------------
   Dup req, respond locally
-----------------------------------------------------------------------------*/
      if (qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active)
      {
        LOG_MSG_INFO1("Client has already registered to embms sai list ind. Don't send to modem",0,0,0);
        embms_sai_list_reg = IND_REQ_NOT_REQUIRED;
      }
/*-----------------------------------------------------------------------------
     If first indication register send to modem.
  -----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_sai_list_ind_count == QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Client fd %d is first client for embms sai list ind",active_fd,0,0);
        embms_sai_list_reg = IND_ENABLE;
      }
/*-----------------------------------------------------------------------------
    If not first indication register respond locally.
-----------------------------------------------------------------------------*/
      else if (qmi_ip_conf->qmi_info.embms_sai_list_ind_count > QMI_IP_INACTIVE)
      {
        LOG_MSG_INFO1("Other clients have already registered to embms sai list ind. Don't send to modem",0,0,0);
        embms_sai_list_reg = IND_REQ_NOT_REQUIRED;
      }
    }
  /*-----------------------------------------------------------------------------
    Process disable of indication registration
  -----------------------------------------------------------------------------*/
    else if (ind_reg_req.report_embms_sai_list == IND_DISABLE)
    {
/*-----------------------------------------------------------------------------
   Dup req/ client has not enabled the ind.
  -----------------------------------------------------------------------------*/
      if (!qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active)
      {
        LOG_MSG_INFO1("Client has not  registered to embms sai list ind. Don't send to modem",0,0,0);
        embms_list_reg = IND_REQ_NOT_REQUIRED;
      }
/*-----------------------------------------------------------------------------
  If last indication register then send to modem
  -----------------------------------------------------------------------------*/
     else if (qmi_ip_conf->qmi_info.embms_sai_list_ind_count == QMI_IP_ACTIVE)
     {
       LOG_MSG_INFO1("Client fd %d is the only client for embms list ind.will send req to modem",0,0,0);
       embms_sai_list_reg = IND_DISABLE;
     }
/*-----------------------------------------------------------------------------
    If not last indication register respond locally
-----------------------------------------------------------------------------*/
     else if (qmi_ip_conf->qmi_info.embms_sai_list_ind_count > QMI_IP_ACTIVE)
     {
       LOG_MSG_INFO1("Other clients have registered for embms sai list ind. Don't send to modem",0,0,0);
       embms_sai_list_reg = IND_REQ_NOT_REQUIRED;
      }
   }
  }

/*-----------------------------------------------------------------------------
  Check if the req needs to be sent to modem or not
-----------------------------------------------------------------------------*/
  if (embms_list_reg != IND_REQ_NOT_REQUIRED || embms_sai_list_reg != IND_REQ_NOT_REQUIRED)
  {
/*-----------------------------------------------------------------------------
  Populate the request
-----------------------------------------------------------------------------*/
    memset(&ind_reg_req, 0, sizeof(wds_indication_register_req_msg_v01));
    memset(msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
           0,
           MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

    if ((embms_list_reg == IND_ENABLE) || (embms_list_reg == IND_DISABLE))
    {
      ind_reg_req.report_embms_tmgi_list_valid = TRUE;
      ind_reg_req.report_embms_tmgi_list = embms_list_reg;
    }
    if ((embms_sai_list_reg == IND_ENABLE) || (embms_sai_list_reg == IND_DISABLE))
    {
      ind_reg_req.report_embms_sai_list_valid = TRUE;
      ind_reg_req.report_embms_sai_list = embms_sai_list_reg;
    }

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_REQUEST,
                  QMI_WDS_INDICATION_REGISTER_REQ_V01,
                  &ind_reg_req,
                  sizeof(wds_indication_register_req_msg_v01),
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  &msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding ind req request message %d", ret_val, 0 ,0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Will send QMI message to modem
-----------------------------------------------------------------------------*/
    ret_val = qmi_ip_send_message_to_modem(
                           qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                           QMI_WDS_INDICATION_REGISTER_REQ_V01,
                           msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           msg_len,
                           resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                           resp_msg_len);
    if (ret_val == FAILURE)
    {
      LOG_MSG_ERROR("Error in sending ind reg message to modem", 0, 0, 0);
      return FAILURE;
    }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
    ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_INDICATION_REGISTER_RESP_V01,
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  *resp_msg_len,
                  &ind_reg_resp,
                  sizeof(wds_indication_register_resp_msg_v01));
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in decoding ind reg resp message %d", ret_val, 0 ,0);
      return FAILURE;
    }
    if (ind_reg_resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Error in performing ind reg on modem. err %d",
                    ind_reg_resp.resp.error, 0, 0);
      return FAILURE;
    }
  }
/*-----------------------------------------------------------------------------
  Respond locally
-----------------------------------------------------------------------------*/
  else
  {
/*-----------------------------------------------------------------------------
  Populate the response locally
-----------------------------------------------------------------------------*/
    memset(&ind_reg_resp, 0, sizeof(wds_indication_register_resp_msg_v01));
    ind_reg_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    ind_reg_resp.resp.error = QMI_ERR_NONE_V01;

    ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_RESPONSE,
                  QMI_WDS_INDICATION_REGISTER_REQ_V01,
                  &ind_reg_resp,
                  sizeof(wds_indication_register_resp_msg_v01),
                  resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  resp_msg_len);
    if (ret_val < QMI_IDL_LIB_NO_ERR)
    {
      LOG_MSG_ERROR("Error in encoding wds reg response message %d", ret_val, 0 ,0);
      return FAILURE;
    }
  }
/*-----------------------------------------------------------------------------
  Update the status of indication registration per client
-----------------------------------------------------------------------------*/
  if (ind_reg_req.report_embms_tmgi_list_valid)
  {
    if (ind_reg_req.report_embms_tmgi_list == IND_ENABLE &&
        !qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active = TRUE;
      qmi_ip_conf->qmi_info.embms_list_ind_count++;
    }
    else if (ind_reg_req.report_embms_tmgi_list == IND_DISABLE &&
             qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active = FALSE;
      qmi_ip_conf->qmi_info.embms_list_ind_count--;
    }

    LOG_MSG_INFO1("Embms list ind status for client %d is %d",
                  client_id,
                  qmi_ip_conf->embms_client[client_id].is_embms_list_ind_active,
                  0);
    LOG_MSG_INFO1("Embms list ind count %d",
                  qmi_ip_conf->qmi_info.embms_list_ind_count, 0, 0);
  }


  /*-----------------------------------------------------------------------------
  Update the SAI status of indication registration per client
-----------------------------------------------------------------------------*/
  if (ind_reg_req.report_embms_sai_list_valid)
  {
    if (ind_reg_req.report_embms_sai_list == IND_ENABLE &&
        !qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active = TRUE;
      qmi_ip_conf->qmi_info.embms_sai_list_ind_count++;
    }
    else if (ind_reg_req.report_embms_tmgi_list == IND_DISABLE &&
             qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active)
    {
      qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active = FALSE;
      qmi_ip_conf->qmi_info.embms_sai_list_ind_count--;
    }

    LOG_MSG_INFO1("Embms list SAI ind status for client %d is %d",
                  client_id,
                  qmi_ip_conf->embms_client[client_id].is_embms_sai_list_ind_active,
                  0);
    LOG_MSG_INFO1("Embms list SAI ind count %d",
                  qmi_ip_conf->qmi_info.embms_sai_list_ind_count, 0, 0);
  }


  return SUCCESS;

}
/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_MESSAGE()

DESCRIPTION
  This function handles the processing of WDS QMI messages.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/

int qmi_ip_proccess_wds_message
(
  int    active_fd,
  char   *msg,
  int    msg_len,
  int    message_id,
  char   *resp_buf,
  int    *resp_msg_len
)
{
  int                                      ret_val;
  int                                      error_code;

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Handle the different QMI WDS messages
----------------------------------------------------------------------------*/
  switch (message_id)
  {
    case QMI_WDS_START_NETWORK_INTERFACE_REQ_V01:
      ret_val = qmi_ip_proccess_wds_start_network_interface_message(active_fd,
                                                                    msg,
                                                                    msg_len,
                                                                    resp_buf,
                                                                    resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

    case QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01:
      ret_val = qmi_ip_proccess_wds_stop_network_interface_message(active_fd,
                                                                    msg,
                                                                    msg_len,
                                                                    resp_buf,
                                                                    resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

    case QMI_WDS_EMBMS_TMGI_ACTIVATE_REQ_V01:
      ret_val = qmi_ip_proccess_wds_tmgi_activate_message(active_fd,
                                                          msg,
                                                          msg_len,
                                                          resp_buf,
                                                          resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

    case QMI_WDS_EMBMS_TMGI_DEACTIVATE_REQ_V01:
      ret_val = qmi_ip_proccess_wds_tmgi_deactivate_message(active_fd,
                                                          msg,
                                                          msg_len,
                                                          resp_buf,
                                                          resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

    case QMI_WDS_EMBMS_TMGI_LIST_QUERY_REQ_V01:
      ret_val = qmi_ip_proccess_wds_tmgi_list_query_message(active_fd,
                                                          msg,
                                                          msg_len,
                                                          resp_buf,
                                                          resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

    case QMI_WDS_EMBMS_TMGI_ACT_DEACT_REQ_V01:
      ret_val = qmi_ip_proccess_wds_tmgi_act_deact_message(active_fd,
                                                          msg,
                                                          msg_len,
                                                          resp_buf,
                                                          resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

    case QMI_WDS_INDICATION_REGISTER_REQ_V01:
      ret_val = qmi_ip_proccess_wds_ind_reg_message(active_fd,
                                                    msg,
                                                    msg_len,
                                                    resp_buf,
                                                    resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;


/*-----------------------------------------------------------------------------
     Handling SAI List Query Request
-----------------------------------------------------------------------------*/

    case QMI_WDS_EMBMS_SAI_LIST_QUERY_REQ_V01:

      ret_val = qmi_ip_proccess_wds_embms_sai_list_query_request_message(active_fd,
                                                                 msg,
                                                                 msg_len,
                                                                 resp_buf,
                                                                 resp_msg_len);
      if (ret_val != SUCCESS)
      {
        error_code = QMI_ERR_INTERNAL_V01;
        goto send_error;
      }
      break;

/*-----------------------------------------------------------------------------
  Respond error for messages not supported by QMI IP
-----------------------------------------------------------------------------*/
    default:
      LOG_MSG_ERROR("Invalid WDS QMI message received", 0, 0, 0);
      error_code = QMI_ERR_MALFORMED_MSG_V01;
      goto send_error;
  }

  return SUCCESS;

/*---------------------------------------------------------------------------
  Handle error case
----------------------------------------------------------------------------*/
send_error:

  ret_val = qmi_idl_encode_resp_tlv(
               QMI_RESULT_FAILURE_V01,
               error_code,
               resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
               MAX_QMI_IP_PKT_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

  if (ret_val != QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in encoding response error message. ret_val %d", ret_val, 0 ,0);
    return FAILURE;
  }
  else
  {
    *resp_msg_len = QMI_RESP_TLV_LEN;
    LOG_MSG_INFO1("Successfully encoded response error message", 0, 0 ,0);
    return SUCCESS;
  }
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_CTRL_MSG()

DESCRIPTION

  This function processes QMI messages from LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/

int qmi_ip_process_ctrl_msg
(
  int     active_fd,
  void    *data,
  int     data_len
)
{
  int                 service_type;
  int                 qmi_message_id;
  int                 qmi_msg_length;
  char                req_buf[MAX_QMI_IP_PKT_SIZE];
  char                resp_buf[MAX_QMI_IP_PKT_SIZE];
  int                 resp_msg_len;
  qmi_ip_qmux_msg_s   *qmux_msg;
  int                 ret;
  int                 bytes_processed = 0;
  int                 message_length;
  int                 isLengthValid;
/*-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  The data received can contain multiple QMI messages. Process all of them.
----------------------------------------------------------------------------*/
  while (data_len > 0)
  {
    memset(req_buf, 0, sizeof(req_buf));
    memset(resp_buf, 0, sizeof(resp_buf));
/*---------------------------------------------------------------------------
  Get the message length
----------------------------------------------------------------------------*/
    qmux_msg = (qmi_ip_qmux_msg_s *)(data + bytes_processed);
    message_length = (uint16)qmux_msg->qmux_hdr.length;
/*---------------------------------------------------------------------------
  Copy over a single QMI message into a buffer to make things simple.
----------------------------------------------------------------------------*/
/*Using the concept of memscpy to prevent stack overflow*/
    isLengthValid = sizeof(req_buf) <= message_length ? 0 : 1;
    if(!isLengthValid)
    {
      LOG_MSG_ERROR("QMI IP : Message length is greater than sizeof req buf",0,0,0);
      return FAILURE;
    }
    memcpy(req_buf, (char *)(data + bytes_processed), (message_length+1));
    qmux_msg = (qmi_ip_qmux_msg_s *)req_buf;
/*---------------------------------------------------------------------------
  Decode the header parameters
----------------------------------------------------------------------------*/
    service_type = (uint8)qmux_msg->qmux_hdr.svc_type;
    qmi_message_id = (uint16)qmux_msg->sdu.qmux.msg.msg_id;

    LOG_MSG_INFO1("QMI IP : Message length %d Service type %d, message id %d",
                  message_length,
                  service_type,
                  qmi_message_id);

  /*---------------------------------------------------------------------------
    Process the message based on service type
  ----------------------------------------------------------------------------*/
    switch (service_type)
    {
      case QMI_WDS_SERVICE_TYPE:
        ret = qmi_ip_proccess_wds_message(active_fd,
                                          req_buf,
                                          message_length+1,
                                          qmi_message_id,
                                          resp_buf,
                                          &resp_msg_len);
        break;

      case QMI_NAS_SERVICE_TYPE:
        ret = qmi_ip_proccess_nas_message(active_fd,
                                          req_buf,
                                          message_length+1,
                                          qmi_message_id,
                                          resp_buf,
                                          &resp_msg_len);
        break;

      case QMI_IP_SERVICE_TYPE:
        ret = qmi_ip_proccess_qmi_ip_message(active_fd,
                                             req_buf,
                                             message_length+1,
                                             qmi_message_id,
                                             resp_buf,
                                             &resp_msg_len);
        break;

      default:
        LOG_MSG_ERROR("Incorrect service type: %d", service_type, 0, 0);
        ret = FAILURE;
        break;
    }

    if (ret == -1)
    {
      LOG_MSG_ERROR("Processing of message failed, type %d", service_type,0,0);
      resp_msg_len = 0;
    }
  /*---------------------------------------------------------------------------
    Prepare the response header fields
  ----------------------------------------------------------------------------*/
    qmux_msg->sdu.qmux.hdr.svc_ctl_flags = QMI_RESP_CTRL_FLAGS;
    qmux_msg->sdu.qmux.msg.msg_length = resp_msg_len;
    qmux_msg->qmux_hdr.length = QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + resp_msg_len - 1;

    memcpy(resp_buf, qmux_msg, QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES);

    qmi_ip_write_pkts (active_fd,
                       resp_buf,
                       QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + resp_msg_len);

    bytes_processed = bytes_processed + (message_length + 1);
    data_len = data_len - (message_length+1);
  }

  return SUCCESS;

}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_LIST_IND()

DESCRIPTION
  Processes WDS TMGI list ind before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_tmgi_list_ind
(
  char  *msg,
  int   buf_len,
  int   client_fd
)
{
  wds_embms_tmgi_list_ind_msg_v01       tmgi_list_ind;
  qmi_idl_service_object_type           wds_service_object;
  int                                   write_fd;
  int                                   i,j,k,l,m,n;
  int                                   ret_val;
  qmi_ip_qmux_msg_s                     *qmux_msg;
  boolean                               tmgi_status;
/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get WDS service object
----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
  }

  memset(&tmgi_list_ind, 0, sizeof(wds_embms_tmgi_list_ind_msg_v01));
/*---------------------------------------------------------------------------
  Decode the indication
----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_EMBMS_TMGI_LIST_IND_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  buf_len,
                  &tmgi_list_ind,
                  sizeof(wds_embms_tmgi_list_ind_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  If its active TMGI list ind populate ind locally and send. ignore the ind
  received from modem. Populate the active TMGIs per client locally.
  Also keep the TMGI table updated for any TMGI which have been deactivated
  by network
-----------------------------------------------------------------------------*/
  if (tmgi_list_ind.tmgi_list_valid &&
      tmgi_list_ind.tmgi_list.list_type == WDS_EMBMS_TMGI_LIST_ACTIVE_V01)
  {
    LOG_MSG_INFO1("Current active TMGIs %d",
                  tmgi_list_ind.tmgi_list.tmgi_list_len,
                  0,
                  0);

/*-----------------------------------------------------------------------------
  Keep the TMGI table updated by looking for any TMGI which have been
  deactivated by network.
-----------------------------------------------------------------------------*/
    for (l=0; l < qmi_ip_conf->current_tmgi_inst_cnt; l++)
    {
      LOG_MSG_INFO1("Checking if TMGI inst %d is active", l, 0, 0);

      tmgi_status = FALSE;
      for (m=0; m < tmgi_list_ind.tmgi_list.tmgi_list_len; m++)
      {
        if( memcmp(&qmi_ip_conf->tmgi_instance[l].client_tmgi,
                   &tmgi_list_ind.tmgi_list.tmgi_list[m].tmgi,
                   sizeof(qmi_ip_embms_tmgi_t))== 0)
        {
          LOG_MSG_INFO1("TMGI inst %d is active", l, 0, 0);
          tmgi_status = TRUE;
          break;
        }
      }
/*-----------------------------------------------------------------------------
  Will delete the inactive TMGI.
-----------------------------------------------------------------------------*/
      if (tmgi_status == FALSE)
      {
        LOG_MSG_INFO1("TMGI inst %d is inactive. Will delete TMGI from DB", l, 0, 0);
        while(qmi_ip_conf->tmgi_instance[l].ref_count > 0)
        {
          ret_val = qmi_ip_tunnel_deletion(qmi_ip_conf->tmgi_instance[l].client_fds[qmi_ip_conf->tmgi_instance[l].ref_count-1],
                                           &qmi_ip_conf->tmgi_instance[l].client_tmgi);
          if (ret_val != SUCCESS)
          {
            LOG_MSG_ERROR("Failed to delete tunnel inst for tmgi", 0, 0, 0);
          }
        }
      }
    }

/*-----------------------------------------------------------------------------
  If its active TMGI list ind populate ind locally and send. ignore the ind
  received from modem. Populate the active TMGIs per client locally.
-----------------------------------------------------------------------------*/
    for (i=0; i < qmi_ip_conf->current_client_cnt; i++)
    {
      if (qmi_ip_conf->embms_client[i].is_embms_list_ind_active)
      {
        memset(&tmgi_list_ind, 0, sizeof(wds_embms_tmgi_list_ind_msg_v01));
        tmgi_list_ind.tmgi_list_valid = 1;
        tmgi_list_ind.tmgi_list.list_type = WDS_EMBMS_TMGI_LIST_ACTIVE_V01;
        for (j=0; j < qmi_ip_conf->current_tmgi_inst_cnt; j++)
        {
          for (k=0; k < qmi_ip_conf->tmgi_instance[j].ref_count; k++)
          {
            if (qmi_ip_conf->embms_client[i].socket_fd
                 == qmi_ip_conf->tmgi_instance[j].client_fds[k])
            {
              memcpy(tmgi_list_ind.tmgi_list.tmgi_list[tmgi_list_ind.tmgi_list.tmgi_list_len].tmgi,
                     qmi_ip_conf->tmgi_instance[j].client_tmgi.tmgi,
                     sizeof(qmi_ip_embms_tmgi_t));
              tmgi_list_ind.tmgi_list.tmgi_list_len++;
            }
          }
        }
/*-----------------------------------------------------------------------------
    Encode the ind
-----------------------------------------------------------------------------*/
        ret_val = qmi_idl_message_encode(
                    wds_service_object,
                    QMI_IDL_INDICATION,
                    QMI_WDS_EMBMS_TMGI_LIST_IND_V01,
                    &tmgi_list_ind,
                    sizeof(wds_embms_tmgi_list_ind_msg_v01),
                    msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                    QMI_MAX_STD_MSG_SIZE,
                    &buf_len);
        if (ret_val < QMI_IDL_LIB_NO_ERR)
        {
          LOG_MSG_ERROR("Error in encoding TMGI list ind message %d", ret_val, 0 ,0);
          return FAILURE;
        }

        qmux_msg = (qmi_ip_qmux_msg_s *)msg;
        qmux_msg->qmux_hdr.length = buf_len + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES - 1;
        qmux_msg->sdu.qmux.hdr.txn_id = QMI_IND_CTRL_FLAGS;
        qmux_msg->sdu.qmux.msg.msg_length = buf_len;
/*-----------------------------------------------------------------------------
    Send the indication to clients
-----------------------------------------------------------------------------*/
        qmi_ip_write_pkts (qmi_ip_conf->embms_client[i].socket_fd,
                           msg,
                           QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
      }
    }
  }
/*-----------------------------------------------------------------------------
  If its available list indication send it to clients without altering it.
-----------------------------------------------------------------------------*/
  else
  {
    for (i=0; i < qmi_ip_conf->current_client_cnt; i++)
    {
      if (qmi_ip_conf->embms_client[i].is_embms_list_ind_active)
      {
        qmi_ip_write_pkts (qmi_ip_conf->embms_client[i].socket_fd,
                           msg,
                           QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
      }
    }
  }

  return SUCCESS;

}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_PKT_SRVC_IND()

DESCRIPTION
  Processes WDS packet service ind before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_pkt_srvc_ind
(
  char  *msg,
  int   buf_len,
  int   client_fd
)
{
  wds_pkt_srvc_status_ind_msg_v01       pkt_srvc_ind;
  qmi_idl_service_object_type           wds_service_object;
  int                                   write_fd;
  qmi_ip_embms_tmgi_t                   temp_tmgi;
  int                                   tmgi_index;
  int                                   i;
  int                                   ret_val;
  int                                   client_index;;
/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get WDS service object
----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
  }

  memset(&temp_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
  memset(&pkt_srvc_ind, 0, sizeof(wds_pkt_srvc_status_ind_msg_v01));
/*---------------------------------------------------------------------------
  Decode the indication
----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_PKT_SRVC_STATUS_IND_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  buf_len,
                  &pkt_srvc_ind,
                  sizeof(wds_pkt_srvc_status_ind_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("Recieved packet service ind. status %d",
                pkt_srvc_ind.status.connection_status, 0, 0);

/*---------------------------------------------------------------------------
  Check call type.
----------------------------------------------------------------------------*/
  if (pkt_srvc_ind.tech_name_valid)
  {
    if (pkt_srvc_ind.tech_name != WDS_EXT_TECH_PREF_EMBMS_V01)
    {
      LOG_MSG_ERROR("Tech type not embms", 0, 0, 0);
      return FAILURE;
    }
  }
/*---------------------------------------------------------------------------
  If client is unknown, it means indication was received from modem.
----------------------------------------------------------------------------*/
  if (client_fd == -1)
  {
/*-----------------------------------------------------------------------------
  Send WDS pkt srvc ind to all clients who have done SNI and are expected to
  receive the indication
-----------------------------------------------------------------------------*/
    for (i=0; i < qmi_ip_conf->current_client_cnt; i++)
    {
      if (qmi_ip_conf->embms_client[i].is_sni_active)
      {
        write_fd = qmi_ip_conf->embms_client[i].socket_fd;
        if (write_fd)
        {
/*-----------------------------------------------------------------------------
  Send indication to clients
-----------------------------------------------------------------------------*/
          qmi_ip_write_pkts (write_fd,
                             msg,
                             QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);

/*-----------------------------------------------------------------------------
  If we receive disconnected status from update client status accordingly.
-----------------------------------------------------------------------------*/
          if ((pkt_srvc_ind.status.connection_status ==
               WDS_CONNECTION_STATUS_DISCONNECTED_V01) &&
              qmi_ip_conf->embms_client[i].is_sni_active)
          {
            qmi_ip_conf->embms_client[i].is_sni_active = FALSE;
            qmi_ip_conf->qmi_info.sni_count --;
            qmi_ip_conf->qmi_info.qmi_ip_wds_v4_call_handle = 0;
            LOG_MSG_INFO1("Client %d SNI status %d",
                          i, qmi_ip_conf->embms_client[i].is_sni_active, 0);
            LOG_MSG_INFO1("Decremented sni count %d", qmi_ip_conf->qmi_info.sni_count, 0, 0);
          }
        }
        else
        {
          LOG_MSG_ERROR("WDS indication recipient unknown", 0, 0, 0);
          return FAILURE;
        }
      }
    }

    if (pkt_srvc_ind.status.connection_status ==
               WDS_CONNECTION_STATUS_DISCONNECTED_V01)
    {
      qmi_ip_process_deactivate_all_tunnels();
    }
  }
/*-----------------------------------------------------------------------------
  If the indication was generated locally, then send it to the appropriate
  client
-----------------------------------------------------------------------------*/
  else
  {
    write_fd = client_fd;
/*-----------------------------------------------------------------------------
  Get client index based on fd
-----------------------------------------------------------------------------*/
    client_index =  qmi_ip_get_client_id(write_fd);
    if (client_index == FAILURE)
    {
      LOG_MSG_ERROR("Invalid client fd passed %d", write_fd, 0, 0);
      return FAILURE;
    }

    if (write_fd)
    {
/*-----------------------------------------------------------------------------
  Send message to client.
-----------------------------------------------------------------------------*/
      qmi_ip_write_pkts (write_fd,
                         msg,
                         QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);

/*-----------------------------------------------------------------------------
  If packet srvc status is disconnected update per client accordingly.
-----------------------------------------------------------------------------*/
      if ((pkt_srvc_ind.status.connection_status ==
          WDS_CONNECTION_STATUS_DISCONNECTED_V01) &&
          qmi_ip_conf->embms_client[client_index].is_sni_active)
      {
        qmi_ip_conf->embms_client[client_index].is_sni_active = FALSE;
        qmi_ip_conf->qmi_info.sni_count--;
        LOG_MSG_INFO1("Client %d SNI status %d",
                      client_index,
                      qmi_ip_conf->embms_client[client_index].is_sni_active,
                      0);
        LOG_MSG_INFO1("Decremented sni count %d", qmi_ip_conf->qmi_info.sni_count, 0, 0);
      }
    }
    else
    {
      LOG_MSG_ERROR("WDS indication recipient unknown", 0, 0, 0);
      return FAILURE;
    }
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_ACT_DEACT_IND()

DESCRIPTION
  Processes WDS TMGI act deact indication before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_tmgi_act_deact_ind
(
  int                   client_fd,
  qmi_ip_embms_tmgi_t   act_tmgi,
  qmi_ip_embms_tmgi_t   deact_tmgi
)
{
  wds_embms_tmgi_act_deact_ind_msg_v01   act_deact_ind;
  int                                    i,j;
  char                                   msg[QMI_MAX_STD_MSG_SIZE];
  qmi_ip_qmux_msg_s                      *qmux_msg;
  qmi_idl_service_object_type            wds_service_object;
  int                                    ret_val;
  int                                    ret = FAILURE;
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Client ID is always known when we come here since this function is called
  after figuring out the client id
-----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  We need to generate the act_deact ind if necessary. So initialize the ind
  msg.
-----------------------------------------------------------------------------*/
  memset(msg, 0, sizeof(msg));
  qmux_msg = (qmi_ip_qmux_msg_s *)msg;

  LOG_MSG_INFO1("Act deact table entry count %d",
                qmi_ip_conf->act_deact_entry_count, 0, 0);
/*-----------------------------------------------------------------------------
  Check if the TMGI act/deact ind was for a previous act_deact msg.
-----------------------------------------------------------------------------*/
  for (i=0; i < qmi_ip_conf->act_deact_entry_count; i++)
  {
/*-----------------------------------------------------------------------------
  Check if TMGI act ind was a previous act_deact msg
-----------------------------------------------------------------------------*/
    if ((client_fd == qmi_ip_conf->act_deact_entry[i].fd) &&
        (memcmp(&act_tmgi,
                &qmi_ip_conf->act_deact_entry[i].act_tmgi,
                sizeof(qmi_ip_embms_tmgi_t))))
    {
      qmi_ip_conf->act_deact_entry[i].act_tmgi_ind_status = TRUE;
      LOG_MSG_INFO1("Updated act status for client fd %d in act_deact entry %d as %d",
                    client_fd,
                    i,
                    qmi_ip_conf->act_deact_entry[i].act_tmgi_ind_status);
    }
/*-----------------------------------------------------------------------------
  Check if TMGI deact ind was a previous act_deact msg
-----------------------------------------------------------------------------*/
    else if ((client_fd == qmi_ip_conf->act_deact_entry[i].fd) &&
             (memcmp(&deact_tmgi,
                     &qmi_ip_conf->act_deact_entry[i].deact_tmgi,
                     sizeof(qmi_ip_embms_tmgi_t))))
    {
      qmi_ip_conf->act_deact_entry[i].deact_tmgi_ind_status = TRUE;
      LOG_MSG_INFO1("Updated deact status for client fd %d in act_deact entry %d as %d",
                    client_fd,
                    i,
                    qmi_ip_conf->act_deact_entry[i].deact_tmgi_ind_status);
    }
/*-----------------------------------------------------------------------------
  If an act-deact msg has successfully received both act and deact ind then
  we need to send an act_deact ind to the client.
-----------------------------------------------------------------------------*/
    if (qmi_ip_conf->act_deact_entry[i].deact_tmgi_ind_status &&
        qmi_ip_conf->act_deact_entry[i].act_tmgi_ind_status)
    {
      LOG_MSG_INFO1("Found act_deact ind status as true for client fd %d in entry %d",
                    client_fd, i, 0);
/*-----------------------------------------------------------------------------
  Populate the contents of the ind.
-----------------------------------------------------------------------------*/
      memset(&act_deact_ind,
             0,
             sizeof(wds_embms_tmgi_act_deact_ind_msg_v01));
      act_deact_ind.act_status = WDS_TMGI_ACTIVATE_SUCCESS_V01;
      memcpy(act_deact_ind.act_tmgi.tmgi,
             act_tmgi.tmgi,
             sizeof(act_tmgi.tmgi));
      act_deact_ind.deact_status = WDS_TMGI_DEACTIVATE_SUCCESS_V01;
      memcpy(act_deact_ind.deact_tmgi.tmgi,
             &deact_tmgi.tmgi,
             sizeof(deact_tmgi.tmgi));
/*-----------------------------------------------------------------------------
  Get the service obj for encode
-----------------------------------------------------------------------------*/
      wds_service_object = wds_get_service_object_v01();
      if(!wds_service_object)
      {
        LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
      }
      ret_val = qmi_idl_message_encode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01,
                  &act_deact_ind,
                  sizeof(wds_embms_tmgi_act_deact_ind_msg_v01),
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  QMI_MAX_STD_MSG_SIZE,
                  &qmux_msg->sdu.qmux.msg.msg_length);
      if (ret_val < QMI_IDL_LIB_NO_ERR)
      {
        LOG_MSG_ERROR("Error in encoding TMGI act_deact ind message %d", ret_val, 0 ,0);
        return FAILURE;
      }
/*-----------------------------------------------------------------------------
  Populate the indication header.
-----------------------------------------------------------------------------*/
      qmux_msg->qmux_hdr.svc_type = QMI_WDS_SERVICE_TYPE;
      qmux_msg->qmux_hdr.length =
        QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES +(qmux_msg->sdu.qmux.msg.msg_length) -1;
      qmux_msg->sdu.qmux.hdr.svc_ctl_flags = QMI_IND_CTRL_FLAGS;
      qmux_msg->sdu.qmux.msg.msg_id = QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01;

/*-----------------------------------------------------------------------------
  Send the indication to client.
-----------------------------------------------------------------------------*/
      qmi_ip_write_pkts (qmi_ip_conf->act_deact_entry[i].fd,
                         msg,
                         QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + qmux_msg->sdu.qmux.msg.msg_length);

      memset(&qmi_ip_conf->act_deact_entry[i],
             0,
             sizeof(qmi_ip_act_deact_ind_t));
      for (j = i; (j+1) < qmi_ip_conf->act_deact_entry_count; j++)
      {
        memcpy(&qmi_ip_conf->act_deact_entry[j],
               &qmi_ip_conf->act_deact_entry[j+1],
               sizeof(qmi_ip_act_deact_ind_t));
      }
      memset(&qmi_ip_conf->act_deact_entry[qmi_ip_conf->act_deact_entry_count-1],
             0,
             sizeof(qmi_ip_act_deact_ind_t));
      qmi_ip_conf->act_deact_entry_count--;

      LOG_MSG_INFO1(" act_deact table entry count %d",
                    qmi_ip_conf->act_deact_entry_count, 0, 0);

      ret = SUCCESS;
    }
  }

  return ret;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_DEACTIVATE_IND()

DESCRIPTION
  Processes WDS TMGI deactivate indication before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_tmgi_deactivate_ind
(
  char  *msg,
  int buf_len,
  int   client_fd
)
{
  wds_embms_tmgi_deactivate_ind_msg_v01 tmgi_deactivate_ind;
  qmi_idl_service_object_type           wds_service_object;
  int                                   write_fd;
  qmi_ip_embms_tmgi_t                   temp_tmgi;
  qmi_ip_embms_tmgi_t                   act_tmgi;
  qmi_ip_embms_tmgi_t                   deact_tmgi;
  int                                   tmgi_index;
  int                                   ret_val;
  int                                   i;
/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get WDS service object
----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
  }

  memset(&temp_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
  memset(&tmgi_deactivate_ind, 0, sizeof(wds_embms_tmgi_deactivate_ind_msg_v01));
/*---------------------------------------------------------------------------
  Decode the indication
----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  buf_len,
                  &tmgi_deactivate_ind,
                  sizeof(wds_embms_tmgi_deactivate_ind_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  LOG_MSG_INFO1_6("TMGI to be deactivated ind %d %d %d %d %d %d",
                  tmgi_deactivate_ind.tmgi.tmgi[0],
                  tmgi_deactivate_ind.tmgi.tmgi[1],
                  tmgi_deactivate_ind.tmgi.tmgi[2],
                  tmgi_deactivate_ind.tmgi.tmgi[3],
                  tmgi_deactivate_ind.tmgi.tmgi[4],
                  tmgi_deactivate_ind.tmgi.tmgi[5]);

  memcpy(temp_tmgi.tmgi, tmgi_deactivate_ind.tmgi.tmgi, sizeof(temp_tmgi.tmgi));
/*---------------------------------------------------------------------------
  Get the TMGI instance associated with ind
----------------------------------------------------------------------------*/
  tmgi_index = qmi_ip_get_tmgi_id(&temp_tmgi);
  if (ret_val == FAILURE)
  {
    LOG_MSG_ERROR("Failed to associate TMGI act ind with TMGI", 0, 0, 0);
    return FAILURE;
  }
  if (tmgi_index < 0)
  {
    LOG_MSG_ERROR("Invalid index value", 0, 0, 0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  If client is unknown, it means indication was received from modem.
----------------------------------------------------------------------------*/
  if (client_fd == -1)
  {
    for (i = 0; i < qmi_ip_conf->tmgi_instance[tmgi_index].ref_count; i++)
    {
      write_fd = qmi_ip_conf->tmgi_instance[tmgi_index].client_fds[i];
      LOG_MSG_INFO1("TMGI fd to be deactivated %d", write_fd, 0, 0);
      if (write_fd)
      {
/*---------------------------------------------------------------------------
  Initiate tunnel deletion with tunneling module
----------------------------------------------------------------------------*/
        ret_val = qmi_ip_tunnel_deletion(write_fd, &temp_tmgi);
        if (ret_val != SUCCESS)
        {
          LOG_MSG_ERROR("Failed to delete tunnel inst for tmgi", 0, 0, 0);
        }

/*---------------------------------------------------------------------------
  Send it to act_deact processing which checks if the indication is
  for act_deact message and handles it accordingly.
----------------------------------------------------------------------------*/
        memset(&act_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
        memset(&deact_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
        memcpy(&deact_tmgi, &temp_tmgi, sizeof(qmi_ip_embms_tmgi_t));
        ret_val = qmi_ip_process_wds_tmgi_act_deact_ind(write_fd,
                                                        act_tmgi,
                                                        deact_tmgi);
        if (ret_val == SUCCESS)
        {
          LOG_MSG_INFO1("Indication handled as part of act_deact ind", 0, 0, 0);
          return SUCCESS;
        }

/*---------------------------------------------------------------------------
  Write indication to clients
----------------------------------------------------------------------------*/
        qmi_ip_write_pkts (write_fd,
                           msg,
                           QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
      }
    }
  }
/*---------------------------------------------------------------------------
  If client is known, it means indication was generated locally.
----------------------------------------------------------------------------*/
  else
  {
    write_fd = client_fd;
    LOG_MSG_INFO1("TMGI fd to be deactivated %d", write_fd, 0, 0);

    if (write_fd)
    {
/*---------------------------------------------------------------------------
  Initiate tunnel deletion with tunneling module
----------------------------------------------------------------------------*/
      ret_val = qmi_ip_tunnel_deletion(write_fd, &temp_tmgi);
      if (ret_val != SUCCESS)
      {
        LOG_MSG_ERROR("Failed to delete tunnel inst for tmgi", 0, 0, 0);
      }

/*---------------------------------------------------------------------------
  Send it to act_deact processing which checks if the indication is
  for act_deact message and handles it accordingly.
----------------------------------------------------------------------------*/
      memset(&act_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
      memset(&deact_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
      memcpy(&deact_tmgi, &temp_tmgi, sizeof(qmi_ip_embms_tmgi_t));
      ret_val = qmi_ip_process_wds_tmgi_act_deact_ind(write_fd,
                                                      act_tmgi,
                                                      deact_tmgi);
      if (ret_val == SUCCESS)
      {
        LOG_MSG_INFO1("Indication handled as part of act_deact ind", 0, 0, 0);
        return SUCCESS;
      }

/*---------------------------------------------------------------------------
  Write indication to clients
----------------------------------------------------------------------------*/
      qmi_ip_write_pkts (write_fd,
                         msg,
                         QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
    }
  }

  return SUCCESS;

}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_TMGI_ACTIVATE_IND()

DESCRIPTION
  Processes WDS TMGI activate indication before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_tmgi_activate_ind
(
  char  *msg,
  int   buf_len,
  int   client_fd
)
{
  wds_embms_tmgi_activate_ind_msg_v01   tmgi_activate_ind;
  qmi_idl_service_object_type           wds_service_object;
  int                                   write_fd;
  qmi_ip_embms_tmgi_t                   temp_tmgi;
  qmi_ip_embms_tmgi_t                   act_tmgi;
  qmi_ip_embms_tmgi_t                   deact_tmgi;
  int                                   tmgi_index;
  int                                   i;
  int                                   ret_val;
/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get WDS service object
----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
  }

  memset(&temp_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
  memset(&tmgi_activate_ind, 0, sizeof(wds_embms_tmgi_activate_ind_msg_v01));
/*---------------------------------------------------------------------------
  Decode the indication
----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  wds_service_object,
                  QMI_IDL_INDICATION,
                  QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  buf_len,
                  &tmgi_activate_ind,
                  sizeof(wds_embms_tmgi_activate_ind_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  LOG_MSG_INFO1_6("TMGI to be activated ind %d %d %d %d %d %d",
                  tmgi_activate_ind.tmgi.tmgi[0],
                  tmgi_activate_ind.tmgi.tmgi[1],
                  tmgi_activate_ind.tmgi.tmgi[2],
                  tmgi_activate_ind.tmgi.tmgi[3],
                  tmgi_activate_ind.tmgi.tmgi[4],
                  tmgi_activate_ind.tmgi.tmgi[5]);

  memcpy(temp_tmgi.tmgi, tmgi_activate_ind.tmgi.tmgi, sizeof(temp_tmgi.tmgi));
/*---------------------------------------------------------------------------
  Get the TMGI instance associated with ind
----------------------------------------------------------------------------*/
  tmgi_index = qmi_ip_get_tmgi_id(&temp_tmgi);
  if (ret_val == FAILURE)
  {
    LOG_MSG_ERROR("Failed to associate TMGI act ind with TMGI", 0, 0, 0);
    return FAILURE;
  }
  if (tmgi_index < 0)
  {
    LOG_MSG_ERROR("Invalid index value", 0, 0, 0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  If client is unknown, it means indication was received from modem.
----------------------------------------------------------------------------*/
  if (client_fd == -1)
  {
    LOG_MSG_INFO1("TMGI ref count %d", qmi_ip_conf->tmgi_instance[tmgi_index].ref_count, 0, 0);
    for (i=0; i < qmi_ip_conf->tmgi_instance[tmgi_index].ref_count; i++)
    {
      LOG_MSG_INFO1("TMGI client fd %d", qmi_ip_conf->tmgi_instance[tmgi_index].client_fds[i], 0, 0);
      write_fd = qmi_ip_conf->tmgi_instance[tmgi_index].client_fds[i];
      if (write_fd)
      {
/*---------------------------------------------------------------------------
  Based on the status in the indication clean up/add the tunnel.
  If TMGI activation failed clean up the TMGI information.
----------------------------------------------------------------------------*/
        if (tmgi_activate_ind.activate_status >=
            WDS_TMGI_ACTIVATE_FAILURE_UNKNOWN_V01)
        {
          LOG_MSG_ERROR("TMGI activate failed %d as per indication",
                        tmgi_activate_ind.activate_status, 0, 0);

          ret_val = qmi_ip_tunnel_deletion(write_fd, &temp_tmgi);
          if (ret_val != SUCCESS)
          {
            LOG_MSG_ERROR("Failed to delete tunnel inst for tmgi", 0, 0, 0);
          }
        }
        else
        {
/*---------------------------------------------------------------------------
  Initiate the tunnel addition with tunneling module if TMGI activation
  succeeded
----------------------------------------------------------------------------*/
          ret_val = qmi_ip_tunnel_addition(write_fd, &temp_tmgi);
          if (ret_val != SUCCESS)
          {
            LOG_MSG_ERROR("Failed to add tunnel inst for tmgi", 0, 0, 0);
          }
  /*---------------------------------------------------------------------------
    Send it to act_deact processing which checks if the indication is
    for act_deact message and handles it accordingly.
  ----------------------------------------------------------------------------*/
          memset(&act_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
          memcpy(&act_tmgi, &temp_tmgi, sizeof(qmi_ip_embms_tmgi_t));
          memset(&deact_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));

          ret_val = qmi_ip_process_wds_tmgi_act_deact_ind(write_fd,
                                                          act_tmgi,
                                                          deact_tmgi);
          if (ret_val == SUCCESS)
          {
            LOG_MSG_INFO1("Indication handled as part of act_deact ind", 0, 0, 0);
            return SUCCESS;
          }
        }
/*---------------------------------------------------------------------------
  Write indication to the clients
----------------------------------------------------------------------------*/
        qmi_ip_write_pkts (write_fd,
                           msg,
                           QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
      }
      else
      {
        LOG_MSG_ERROR("WDS indication recipient unknown", 0, 0, 0);
        return FAILURE;
      }
    }
  }
/*---------------------------------------------------------------------------
  Since the client id is known, indication was generated locally.
----------------------------------------------------------------------------*/
  else
  {
    write_fd = client_fd;
    if (write_fd)
    {
/*---------------------------------------------------------------------------
  Initiate the tunnel addition with tunneling module
----------------------------------------------------------------------------*/
      ret_val = qmi_ip_tunnel_addition(write_fd, &temp_tmgi);
      if (ret_val != SUCCESS)
      {
        LOG_MSG_ERROR("Failed to add tunnel inst for tmgi", 0, 0, 0);
      }

/*---------------------------------------------------------------------------
  Send it to act_deact processing which checks if the indication is
  for act_deact message and handles it accordingly.
----------------------------------------------------------------------------*/
      memset(&act_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
      memcpy(&act_tmgi, &temp_tmgi, sizeof(qmi_ip_embms_tmgi_t));
      memset(&deact_tmgi, 0, sizeof(qmi_ip_embms_tmgi_t));
      ret_val = qmi_ip_process_wds_tmgi_act_deact_ind(write_fd,
                                                      act_tmgi,
                                                      deact_tmgi);
      if (ret_val == SUCCESS)
      {
        LOG_MSG_INFO1("Indication handled as part of act_deact ind", 0, 0, 0);
        return SUCCESS;
      }
/*---------------------------------------------------------------------------
  Write indication to the clients
----------------------------------------------------------------------------*/
      qmi_ip_write_pkts (write_fd,
                         msg,
                         QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
    }
    else
    {
      LOG_MSG_ERROR("WDS indication recipient unknown", 0, 0, 0);
      return FAILURE;
    }
  }
  return SUCCESS;

}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_IND()

DESCRIPTION
  Processes WDS indication before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_ind
(
  int   message_id,
  void  *buf,
  int   buf_len,
  int   client_fd
)
{
  char                   msg[QMI_MAX_STD_MSG_SIZE];
  qmi_ip_qmux_msg_s      *qmux_msg;
  int                    ret_val;

/*-------------------------------------------------------------------------*/
  memset(msg, 0, sizeof(msg));
  qmux_msg = (qmi_ip_qmux_msg_s *)msg;

/*---------------------------------------------------------------------------
  Prepare the ind header fields
----------------------------------------------------------------------------*/
  qmux_msg->qmux_hdr.svc_type = QMI_WDS_SERVICE_TYPE;
  qmux_msg->qmux_hdr.length = QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len -1;
  qmux_msg->sdu.qmux.hdr.svc_ctl_flags = QMI_IND_CTRL_FLAGS;
  qmux_msg->sdu.qmux.msg.msg_id = message_id;
  qmux_msg->sdu.qmux.msg.msg_length = buf_len;

  /*---------------------------------------------------------------------------
  if buf_len is more than (QMI_MAX_STD_MSG_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  Then can cause buffer overflow in memcpy
  ----------------------------------------------------------------------------*/

  if (buf_len > (QMI_MAX_STD_MSG_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES))
  {
    LOG_MSG_ERROR("Invalid buffer lengh %d .Will cause buffer overflow", buf_len, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Copy the ind buffer contents
----------------------------------------------------------------------------*/
  memcpy(msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
         buf,
         buf_len);

  LOG_MSG_INFO1("Received WDS ind. ID %d", message_id, 0, 0);

/*---------------------------------------------------------------------------
  Processing of the indication message
----------------------------------------------------------------------------*/
  switch (message_id)
  {
    case QMI_WDS_PKT_SRVC_STATUS_IND_V01:
      ret_val = qmi_ip_process_wds_pkt_srvc_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing TMGI activate indication", 0, 0, 0);
        return FAILURE;
      }
      break;

    case QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01:
      ret_val = qmi_ip_process_wds_tmgi_activate_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing TMGI activate indication", 0, 0, 0);
        return FAILURE;
      }
      break;

    case QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01:
      ret_val = qmi_ip_process_wds_tmgi_deactivate_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing TMGI deactivate indication", 0, 0, 0);
        return FAILURE;
      }
      break;

/*---------------------------------------------------------------------------
  We dont send act_deact message to modem since we split act_deact to
  activate and deactivate messages.
----------------------------------------------------------------------------*/
    case QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01:
      LOG_MSG_ERROR("Unexpected act_deact ind received", 0, 0, 0);
      break;

    case QMI_WDS_EMBMS_TMGI_LIST_IND_V01:
      ret_val = qmi_ip_process_wds_tmgi_list_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing TMGI list indication", 0, 0, 0);
        return FAILURE;
      }
      break;

/*
---------------------------------------------------------------------------
  Handling SAI Incidation function
----------------------------------------------------------------------------*/

    case QMI_WDS_EMBMS_SAI_LIST_IND_V01:
      LOG_MSG_ERROR("processing SAI_LIST_IND list indication", 0, 0, 0);

      ret_val = qmi_ip_process_wds_sai_list_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing SAI list indication", 0, 0, 0);
        return FAILURE;
      }
      break;

    default:
      LOG_MSG_ERROR("Unexpected WDS ind received", 0, 0, 0);
      break;
  }


}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_EMBMS_STATUS_IND()

DESCRIPTION
  Processes NAS embms status ind before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_nas_embms_status_ind
(
  char  *msg,
  int   buf_len,
  int   client_fd
)
{
  int                                i;
  nas_embms_status_ind_msg_v01       embms_status_ind;
  qmi_idl_service_object_type        nas_service_object;
  int                                write_fd;
  int                                ret_val;
  int                                client_index;

/*---------------------------------------------------------------------------
  Get NAS service object
----------------------------------------------------------------------------*/
  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
  }

/*---------------------------------------------------------------------------
  Decode the indication
----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                  nas_service_object,
                  QMI_IDL_INDICATION,
                  QMI_NAS_EMBMS_STATUS_IND_V01,
                  msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                  buf_len,
                  &embms_status_ind,
                  sizeof(nas_embms_status_ind_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("EMBMS status ind %d", embms_status_ind.enabled, 0, 0);

/*---------------------------------------------------------------------------
  If client is unknown, it means indication was received from modem.
----------------------------------------------------------------------------*/
  if (client_fd == -1)
  {
    for (i=0; i < qmi_ip_conf->current_client_cnt; i++)
    {
      LOG_MSG_INFO1("Client %d nce status %d embms status ind %d",
                    i,
                    qmi_ip_conf->embms_client[i].is_nce_active,
                    qmi_ip_conf->embms_client[i].is_embms_status_ind_active);
/*---------------------------------------------------------------------------
  If client has enabled embms and has registered for indications send the
  indication
----------------------------------------------------------------------------*/
      if (qmi_ip_conf->embms_client[i].is_nce_active &&
          qmi_ip_conf->embms_client[i].is_embms_status_ind_active)
      {
        write_fd = qmi_ip_conf->embms_client[i].socket_fd;
        if (write_fd)
        {
/*---------------------------------------------------------------------------
  Send indication to client
----------------------------------------------------------------------------*/
          qmi_ip_write_pkts (write_fd,
                             msg,
                             QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
        }
        else
        {
          LOG_MSG_ERROR("NAS sys info indication recipient unknown", 0, 0, 0);
          return FAILURE;
        }
      }
/*---------------------------------------------------------------------------
  If embms got disabled, then update the status per client.
----------------------------------------------------------------------------*/
      if (embms_status_ind.enabled == FALSE &&
          qmi_ip_conf->embms_client[i].is_nce_active )
      {
        qmi_ip_conf->embms_client[i].is_nce_active = FALSE;
        qmi_ip_conf->qmi_info.nce_count--;
        LOG_MSG_INFO1("Client %d nce status %d",
                      i, qmi_ip_conf->embms_client[i].is_nce_active, 0);
        LOG_MSG_INFO1("nce count %d", qmi_ip_conf->qmi_info.nce_count, 0, 0);
      }
    }
  }
/*---------------------------------------------------------------------------
  If client is known, it means indication was generated locally and we know
  whom to send the indication to.
----------------------------------------------------------------------------*/
  else
  {
    write_fd = client_fd;
/*---------------------------------------------------------------------------
  Get client id based on fd
----------------------------------------------------------------------------*/
    client_index =  qmi_ip_get_client_id(write_fd);
    if (client_index == FAILURE)
    {
      LOG_MSG_ERROR("Invalid client fd passed %d", write_fd, 0, 0);
      return FAILURE;
    }

    LOG_MSG_INFO1("Client %d nce status %d embms status ind %d",
                  client_index,
                  qmi_ip_conf->embms_client[client_index].is_nce_active,
                  qmi_ip_conf->embms_client[client_index].is_embms_status_ind_active);
/*---------------------------------------------------------------------------
  If client has enabled embms and has registered for indications send the
  indication
----------------------------------------------------------------------------*/
    if (qmi_ip_conf->embms_client[client_index].is_nce_active &&
        qmi_ip_conf->embms_client[client_index].is_embms_status_ind_active)
    {
      if (write_fd)
      {
/*---------------------------------------------------------------------------
  Send indication to client
----------------------------------------------------------------------------*/
        qmi_ip_write_pkts (write_fd,
                           msg,
                           QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
      }
      else
      {
        LOG_MSG_ERROR("NAS sys info indication recipient unknown", 0, 0, 0);
        return FAILURE;
      }
    }
/*---------------------------------------------------------------------------
  If embms got disabled, then update the status per client.
----------------------------------------------------------------------------*/
    if (embms_status_ind.enabled == FALSE &&
        qmi_ip_conf->embms_client[client_index].is_nce_active )
    {
      qmi_ip_conf->embms_client[client_index].is_nce_active = FALSE;
      qmi_ip_conf->qmi_info.nce_count--;
      LOG_MSG_INFO1("Client %d nce status %d",
                    client_index,
                    qmi_ip_conf->embms_client[client_index].is_nce_active,
                    0);
      LOG_MSG_INFO1("nce count %d", qmi_ip_conf->qmi_info.nce_count, 0, 0);
    }
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_SYS_INFO_IND()

DESCRIPTION
  Processes NAS sys info list ind before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_nas_sys_info_ind
(
  char  *msg,
  int   buf_len,
  int   client_fd
)
{
  int                              i, write_fd, ret_val;
  nas_sys_info_ind_msg_v01         nas_sys_info_ind;
  qmi_idl_service_object_type      nas_service_object;
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get NAS service object
----------------------------------------------------------------------------*/
  nas_service_object = nas_get_service_object_v01();

  if(!nas_service_object)
  {
    LOG_MSG_ERROR("qmi_ip nas get service object failed",0,0,0);
    return FAILURE;
  }

  memset(&nas_sys_info_ind, 0, sizeof(nas_sys_info_ind_msg_v01));
/*---------------------------------------------------------------------------
  Decode the indication
----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(
                    nas_service_object,
                    QMI_IDL_INDICATION,
                    QMI_NAS_SYS_INFO_IND_MSG_V01,
                    msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                    buf_len,
                    &nas_sys_info_ind,
                    sizeof(nas_sys_info_ind_msg_v01));

  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding QMI message %d", ret_val, 0 ,0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  If client is unknown, it means indication was received from modem.
----------------------------------------------------------------------------*/
  if (client_fd == -1)
  {
    for (i=0; i < qmi_ip_conf->current_client_cnt; i++)
    {
      if (qmi_ip_conf->embms_client[i].is_nas_sys_info_ind_active)
      {
        write_fd = qmi_ip_conf->embms_client[i].socket_fd;
        if (write_fd)
        {
/*---------------------------------------------------------------------------
  Send indication to clients
----------------------------------------------------------------------------*/
          qmi_ip_write_pkts (write_fd,
                             msg,
                             QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
        }
        else
        {
          LOG_MSG_ERROR("NAS sys info indication recipient unknown", 0, 0, 0);
          return FAILURE;
        }
      }
    }
/*-----------------------------------------------------------------------
  If eMBMS service is lost clean up TMGI table.
------------------------------------------------------------------------*/
    if(nas_sys_info_ind.lte_embms_coverage_valid)
    {
      if (nas_sys_info_ind.lte_embms_coverage == FALSE)
      {
        qmi_ip_process_deactivate_all_tunnels();
      }
    }
  }
/*---------------------------------------------------------------------------
  If client id is known then it means indication was generated lcoally and
  we know to whom the indication should be sent.
----------------------------------------------------------------------------*/
  else
  {
    write_fd = client_fd;
    if (write_fd)
    {
/*---------------------------------------------------------------------------
  Send indication to client
----------------------------------------------------------------------------*/
      qmi_ip_write_pkts (write_fd,
                         msg,
                         QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
    }
    else
    {
      LOG_MSG_ERROR("NAS sys info indication recipient unknown", 0, 0, 0);
      return FAILURE;
    }
  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_PROCESS_NAS_IND()

DESCRIPTION

  Processes NAS indication

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_nas_ind
(
  int   message_id,
  void  *buf,
  int   buf_len,
  int  client_fd
)
{
  char                   msg[QMI_MAX_STD_MSG_SIZE];
  qmi_ip_qmux_msg_s      *qmux_msg;
  int                    write_fd;
  int                    ret_val;

/*---------------------------------------------------------------------------
  Populate the header fields
----------------------------------------------------------------------------*/
  memset(msg, 0, sizeof(msg));
  qmux_msg = (qmi_ip_qmux_msg_s *)msg;

  qmux_msg->qmux_hdr.svc_type = QMI_NAS_SERVICE_TYPE;
  qmux_msg->qmux_hdr.length = QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len -1;
  qmux_msg->sdu.qmux.hdr.svc_ctl_flags = QMI_IND_CTRL_FLAGS;
  qmux_msg->sdu.qmux.msg.msg_id = message_id;
  qmux_msg->sdu.qmux.msg.msg_length = buf_len;

 /*---------------------------------------------------------------------------
  if buf_len is more than (QMI_MAX_STD_MSG_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  Then can cause buffer overflow in memcpy
  ----------------------------------------------------------------------------*/

  if (buf_len > (QMI_MAX_STD_MSG_SIZE - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES))
  {
    LOG_MSG_ERROR("Invalid buffer lengh %d .Will cause buffer overflow", buf_len, 0, 0);
    return FAILURE;
  }

  memcpy(msg+QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
         buf,
         buf_len);

/*---------------------------------------------------------------------------
  Process each indication
----------------------------------------------------------------------------*/
  switch (message_id)
  {
    case QMI_NAS_EMBMS_STATUS_IND_V01:
      ret_val = qmi_ip_process_nas_embms_status_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing TMGI activate indication", 0, 0, 0);
        return FAILURE;
      }
      break;

    case QMI_NAS_SYS_INFO_IND_MSG_V01:
      ret_val = qmi_ip_process_nas_sys_info_ind(msg, buf_len, client_fd);
      if (ret_val == FAILURE)
      {
        LOG_MSG_ERROR("Error in processing TMGI activate indication", 0, 0, 0);
        return FAILURE;
      }
      break;

    default:
      LOG_MSG_ERROR("Received unsupported NAS indication", 0, 0, 0);
      break;
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_IND()

DESCRIPTION

  Processes different indications based on service types

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_ind
(
  void *data,
  int   data_len
)
{

  qmi_ip_ind_buf_t  *qmi_ind;
  int               service_type;
  int               message_id;
  int               client_fd;
  int               ret;
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Decode the data contents received.
----------------------------------------------------------------------------*/
  qmi_ind = (qmi_ip_ind_buf_t *)data;
  service_type = qmi_ind->service_type;
  message_id  = qmi_ind->message_id;
  client_fd = qmi_ind->client_id;

  LOG_MSG_INFO1("Received indication for service type %d, message id %d, client %d",
                service_type,
                message_id,
                client_fd);

/*---------------------------------------------------------------------------
  Process different indications based on service type
----------------------------------------------------------------------------*/
  switch (service_type)
  {
    case QMI_WDS_SERVICE_TYPE:
      ret = qmi_ip_process_wds_ind(message_id,
                                   qmi_ind->ind_buf,
                                   qmi_ind->ind_buf_len,
                                   client_fd);
      break;

    case QMI_NAS_SERVICE_TYPE:
      ret = qmi_ip_process_nas_ind(message_id,
                                   qmi_ind->ind_buf,
                                   qmi_ind->ind_buf_len,
                                   client_fd);
      break;

    default:
      LOG_MSG_ERROR("Invalid service type in indication", 0, 0, 0);
      return FAILURE;
  }

  if (ret != SUCCESS)
  {
    LOG_MSG_ERROR("Error in processing indication", 0, 0, 0);
    return FAILURE;
  }

  return SUCCESS;

}

/*===========================================================================

FUNCTION QMI_IP_QMI_CLIENT_INIT()

DESCRIPTION

  Initialize QMI clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_qmi_client_init()
{
  /* embedded instance always needed for embms call*/
  qmi_ip_wds_init(QMI_CLIENT_INSTANCE_ANY);
  qmi_ip_nas_init();
  qmi_ip_netmgr_init();

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_QMI_CLIENT_RELEASE()

DESCRIPTION

  Release QMI clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_qmi_client_release()
{
  qmi_ip_wds_release();
  qmi_ip_nas_release();

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_MSG_HDLR_INIT()

DESCRIPTION

  Initialize QMI message handler module

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_msg_hdlr_init
(
  qmi_ip_conf_t *qmi_ip_config
)
{
  int     ret_val;
  FILE *fp = NULL;
/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Logging for demo.
----------------------------------------------------------------------------*/
#ifdef FEATURE_DEMO_LOGGING
  fp= fopen("/usr/bin/out_logs.txt", "wb");
  if (fp != NULL)
  {
    fprintf(fp,"eMBMS _Over_SoftAP logs \n\n");
    fclose(fp);
    LOG_MSG_INFO1("Successfully opened a file for writing logs",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Error opening the file for writing logs",0,0,0);
  }
#endif

  qmi_ip_conf = qmi_ip_config;

/*---------------------------------------------------------------------------
  Initialize QMI clients
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_qmi_client_init();
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize QMI clients", 0, 0 ,0);
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_PROCESS_WDS_SAI_LIST_IND()

DESCRIPTION
  Processes WDS SAI list ind before sending it to LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_process_wds_sai_list_ind
(
  char  *msg,
  int   buf_len,
  int   client_fd
)
{
  int i;

  for (i=0; i < qmi_ip_conf->current_client_cnt; i++)
  {
    if (qmi_ip_conf->embms_client[i].is_embms_sai_list_ind_active)
    {
      qmi_ip_write_pkts (qmi_ip_conf->embms_client[i].socket_fd,
                         msg,
                         QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES + buf_len);
    }
  }

  LOG_MSG_INFO1("sucess Sending SAI list to client via indication ", 0, 0 ,0);

  return SUCCESS;

}

/*===========================================================================

FUNCTION qmi_ip_proccess_wds_embms_sai_list_message()

DESCRIPTION
  This function handles the processing of WDS embms sai list request  messages.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None
/*=========================================================================*/
int qmi_ip_proccess_wds_embms_sai_list_query_request_message
(
  int   active_fd,
  char  *msg,
  int   msg_len,
  char  *resp_buf,
  int   *resp_msg_len
)
{
  qmi_idl_service_object_type             wds_service_object;
  wds_embms_sai_list_query_req_msg_v01 sai_list_query_req;
  wds_embms_sai_list_query_resp_msg_v01 sai_list_query_resp;
  int i,j;
  int                                     ret_val;

/*-----------------------------------------------------------------------------
     Get service obj for encode/decode
-----------------------------------------------------------------------------*/
  wds_service_object = wds_get_service_object_v01();

  if(!wds_service_object)
  {
    LOG_MSG_ERROR("qmi_ip wds get service object failed",0,0,0);
    return FAILURE;
  }
  if (msg_len < QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES)
  {
    LOG_MSG_ERROR("msg length is less than QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES",0,0,0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
    Will send QMI message to modem
-----------------------------------------------------------------------------*/

  ret_val = qmi_ip_send_message_to_modem(qmi_ip_conf->qmi_info.qmi_ip_wds_v4_handle,
                                         QMI_WDS_EMBMS_SAI_LIST_QUERY_REQ_V01,
                                         msg + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                         msg_len - QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                         resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                         resp_msg_len);
  if (ret_val == FAILURE)
  {
    LOG_MSG_ERROR("Error in sending SAI list query message to modem", 0, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------------
  Decode the QMI message response
-----------------------------------------------------------------------------*/
  ret_val = qmi_idl_message_decode(wds_service_object,
                                   QMI_IDL_RESPONSE,
                                   QMI_WDS_EMBMS_SAI_LIST_QUERY_RESP_V01,
                                   resp_buf + QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES,
                                   *resp_msg_len,
                                   &sai_list_query_resp,
                                    sizeof(wds_embms_sai_list_query_resp_msg_v01));
  if (ret_val < QMI_IDL_LIB_NO_ERR)
  {
    LOG_MSG_ERROR("Error in decoding SAI list query resp message %d", ret_val, 0 ,0);
    return FAILURE;
  }
  if (sai_list_query_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Error in processing SAI list query on modem. err %d",
                  sai_list_query_resp.resp.error, 0, 0);
    return FAILURE;
  }
  LOG_MSG_INFO1("sucess  SAI list query  message  && IS freq_sai_list_valid=%d",
                sai_list_query_resp.freq_sai_list_valid, 0 , 0);
  return SUCCESS;

}

