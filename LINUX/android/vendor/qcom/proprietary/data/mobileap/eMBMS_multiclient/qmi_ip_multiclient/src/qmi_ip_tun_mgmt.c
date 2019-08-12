/******************************************************************************

                        QMI_IP_TUN_MGMT.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_tun_mgmt.c
  @brief   QMI over IP : Tunnel Management

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

/*==========================================================================

FUNCTION QMI_IP_CREATE_TM_INFO_SOCKET()

DESCRIPTION

  Create a socket to receive message from tunneling module

DEPENDENCIES
  None.

RETURN VALUE
  0 on succes
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/

int qmi_ip_create_tm_info_socket(unsigned int *sockfd)
{
  int                         val, rval, len;
  struct sockaddr_un          qmi_ip_control_sock;
  struct timeval              rcv_timeo;

  if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == FAILURE)
  {
    LOG_MSG_ERROR("Error creating socket, errno: %d", errno, 0, 0);
    return FAILURE;
  }

  if(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
  {
    LOG_MSG_ERROR("Couldn't set Close on Exec, errno: %d", errno, 0, 0);
    return FAILURE;
  }

  /*---------------------------------------------------------------------------
    Make the socket non blocking
  ----------------------------------------------------------------------------*/
  val = fcntl(*sockfd, F_GETFL, 0);
  val = val & (~O_NONBLOCK);
  fcntl(*sockfd, F_SETFL, val);

  /*---------------------------------------------------------------------------
    Associate the socket eith a file to read from.
    Tunneling module will write to this file.
    ----------------------------------------------------------------------------*/

  qmi_ip_control_sock.sun_family = AF_UNIX;
  strlcpy(qmi_ip_control_sock.sun_path, QMI_IP_CONTROL_FILE, sizeof(QMI_IP_CONTROL_FILE));
  unlink(qmi_ip_control_sock.sun_path);
  len = strlen(qmi_ip_control_sock.sun_path) + sizeof(qmi_ip_control_sock.sun_family);
  if (bind(*sockfd, (struct sockaddr *)&qmi_ip_control_sock, len) == FAILURE)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    return FAILURE;
  }
  return SUCCESS;
}

/*==========================================================================

FUNCTION QMI_IP_SEND_MSG_TUNNEL_MODULE()

DESCRIPTION

  Send message to tunneling module

DEPENDENCIES
  None.

RETURN VALUE
  0 on succes
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/

int qmi_ip_send_msg_tunnel_module
(
  qmi_ip_tun_mgmt_info_t  *tun_inst
)
{
  struct sockaddr_un    tun_sock;
  int                   len;
  int                   sock_fd;
  int                   numBytes;

/*---------------------------------------------------------------------------
  Open Unix domain socket and send the tunneling information to tunneling
  module
----------------------------------------------------------------------------*/
  tun_sock.sun_family = AF_UNIX;

  strlcpy(tun_sock.sun_path,
         QMI_IP_TUNNEL_MODULE_FILE,sizeof(QMI_IP_TUNNEL_MODULE_FILE));

  len = strlen(tun_sock.sun_path) + sizeof(tun_sock.sun_family);

  if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Error creating UNIX socket, errno: %d", errno, 0, 0);
    return;
  }

  if ((numBytes = sendto(sock_fd,
                         (void *)tun_inst,
                         sizeof(qmi_ip_tun_mgmt_info_t),
                         0,
                         (struct sockaddr *)&tun_sock,
                         len)) == -1)
  {
    LOG_MSG_ERROR("Send failed to tunneling module", 0, 0, 0);
    close(sock_fd);
    return FAILURE;
  }
  close(sock_fd);
  return SUCCESS;
}

/*==========================================================================

FUNCTION QMI_IP_GET_TUNNEL_MODULE_SOCKET_STATUS()

DESCRIPTION

 Create a socket to receive message from tunneling module

DEPENDENCIES
  None.

RETURN VALUE
  0 on succes
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/

int qmi_ip_get_tunnel_module_socket_status()
{
  int                      ret, bytes_read;
  int                      sock_status;
  unsigned int             control_sockfd;
  qmi_ip_tun_mgmt_info_t   tun_inst;
  char                     read_data_buf[QMI_IP_READ_BUF_SIZE];

  ret= qmi_ip_create_tm_info_socket(&control_sockfd);
  if(ret != SUCCESS)
  {
    LOG_MSG_ERROR("Unable to create read socket for control info", 0, 0 ,0);
    close(control_sockfd);
	return FAILURE;
  }

  tun_inst.action_type = OPEN_SEND_SOCKET;
  tun_inst.source_data_port = qmi_ip_conf->fpop_udp_port;
  ret = qmi_ip_send_msg_tunnel_module(&tun_inst);
  if(ret != SUCCESS)
  {
    LOG_MSG_ERROR("Unable to send msg to tunnel module", 0, 0 ,0);
	close(control_sockfd);
    return FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Successfully sent message to tunneling module. Waiting for reply",0,0,0);
  }

  while (TRUE)
  {
    bytes_read = recvfrom(control_sockfd,
                          read_data_buf,
                          QMI_IP_READ_BUF_SIZE,
                          0,
                          NULL,
                          NULL);
    if (bytes_read < 0)
    {
      LOG_MSG_ERROR("Error in reading data from FPOP read socket", 0, 0, 0);
    }
    else if (bytes_read == 1)
    {
      sock_status = (int)read_data_buf[0];
      if (sock_status)
      {
        LOG_MSG_INFO1("Socket opened in tunnelling module successfully",0,0,0);
        break;
      }
      else
      {
        LOG_MSG_ERROR("Error opening socket in tunneling module.",0,0,0);
		close(control_sockfd);
        return FAILURE;
      }
    }
  }
  close(control_sockfd);
}

/*==========================================================================

FUNCTION QMI_IP_GET_TMGI_ID()

DESCRIPTION

  Get the TMGI ID corresponding to a TMGI

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_get_tmgi_id
(
  qmi_ip_embms_tmgi_t *temp_tmgi
)
{
  int i;

  for(i=0; i< qmi_ip_conf->current_tmgi_inst_cnt; i++)
  {
    if(memcmp(temp_tmgi->tmgi,
              qmi_ip_conf->tmgi_instance[i].client_tmgi.tmgi,
              sizeof(qmi_ip_embms_tmgi_t))== 0)
    {
      LOG_MSG_INFO1("Get tmgi id succeeded : %d", i, 0, 0);
      return i;
    }
  }

  LOG_MSG_INFO1("Get tmgi id failed", 0, 0, 0);
  return FAILURE;
}

/*==========================================================================

FUNCTION QMI_IP_GET_CLIENT_ID()

DESCRIPTION

  Get the client connection ID corresponding to connection socket


DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_get_client_id
(
  int sock_fd
)
{
  int i;

  for(i=0; i< qmi_ip_conf->current_client_cnt; i++)
  {
    if(sock_fd == qmi_ip_conf->embms_client[i].socket_fd)
    {
      LOG_MSG_INFO1("Client ID for fd %d is : %d", sock_fd, i, 0);
      return i;
    }
  }

  LOG_MSG_INFO1("Get client id for fd %d failed", sock_fd, 0, 0);
  return FAILURE;
}


/*==========================================================================

FUNCTION QMI_IP_TUNNEL_ADDITION()

DESCRIPTION

  Enable the TMGI in TMGI DB. Initialize the tunnel by sending info to
  tunneling module. This happens based on activate QMI indication.
  This is reqd for the TMGI to be fully enabled.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_tunnel_addition
(
  int                      sock_fd,
  qmi_ip_embms_tmgi_t      *tmgi
)
{
  int                       tmgi_index;
  qmi_ip_tun_mgmt_info_t    tun_inst;
  int                       client_id;
  int                       ret_val;

/*---------------------------------------------------------------------------
  Get the TMGI ID for the TMGI
----------------------------------------------------------------------------*/
  tmgi_index = qmi_ip_get_tmgi_id(tmgi);
  if(tmgi_index == FAILURE)
  {
    LOG_MSG_ERROR("Error in retrieving TMGI index", 0, 0, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("TMGI to be added for client %d", sock_fd, 0, 0);
  LOG_MSG_INFO1_6("TMGI to be added tun %d %d %d %d %d %d",
                  tmgi->tmgi[0],
                  tmgi->tmgi[1],
                  tmgi->tmgi[2],
                  tmgi->tmgi[3],
                  tmgi->tmgi[4],
                  tmgi->tmgi[5]);
/*---------------------------------------------------------------------------
  Get client id based on fd
----------------------------------------------------------------------------*/
  client_id = qmi_ip_get_client_id(sock_fd);
  if(client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid socket fd %d passed", sock_fd, 0, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("Creating tunnel for TMGI index %d. Client id %d",
                tmgi_index,
                client_id,
                0);
/*---------------------------------------------------------------------------
  Prepare the contents to send the data to tunneling module
----------------------------------------------------------------------------*/
  memset(&tun_inst, 0, sizeof(qmi_ip_tun_mgmt_info_t));
  tun_inst.tmgi_multicast_addr = qmi_ip_conf->tmgi_instance[tmgi_index].multicast_addr;
  tun_inst.tmgi_multicast_port = qmi_ip_conf->tmgi_instance[tmgi_index].multicast_port;
  if (qmi_ip_conf->mode == MULTI_CLIENT_MODE)
  {
    tun_inst.client_addr = qmi_ip_conf->embms_client[client_id].private_ip.s_addr;
    tun_inst.client_port = qmi_ip_conf->embms_client[client_id].udp_data_port;
    tun_inst.action_type = ADD_ENTRY;
  }
/*---------------------------------------------------------------------------
  In single client mode since there wont be any FPOP, contents are populated
  acccordingly.
----------------------------------------------------------------------------*/
  else if(qmi_ip_conf->mode == SINGLE_CLIENT_MODE)
  {
    tun_inst.client_addr = qmi_ip_conf->embms_client[client_id].client_ip.s_addr;
    tun_inst.client_port = qmi_ip_conf->single_client_udp_port;
    tun_inst.action_type = CLIENT_ACTIVATE_ALL_TMGI;
  }

  tun_inst.source_data_port = qmi_ip_conf->fpop_udp_port;
  memcpy(tun_inst.interface_name,
         qmi_ip_conf->qmi_info.net_iface_name,
         sizeof(qmi_ip_conf->qmi_info.net_iface_name));


  LOG_MSG_INFO1("Multicast IPV4 address to be added:", 0, 0, 0);
  IPV4_ADDR_MSG(tun_inst.tmgi_multicast_addr);
  LOG_MSG_INFO1("Multicast port to be added %d", ntohs(tun_inst.tmgi_multicast_port), 0, 0);
  LOG_MSG_INFO1("CLient IPV4 address to be added:", 0, 0, 0);
  IPV4_ADDR_MSG(tun_inst.client_addr);
  LOG_MSG_INFO1("Client port to be added %d", ntohs(tun_inst.source_data_port), 0, 0);

  LOG_MSG_INFO1("To tunneling module: action type %d,client port %d",
                tun_inst.action_type,
                tun_inst.client_port,
                0);
  LOG_MSG_INFO1("Client IPV4 address:", 0, 0, 0);
  IPV4_ADDR_MSG(tun_inst.client_addr);
/*---------------------------------------------------------------------------
  Send to tunnleing module
----------------------------------------------------------------------------*/
  if ((qmi_ip_conf->current_client_cnt == 1) &&
      (qmi_ip_conf->current_tmgi_inst_cnt == 1))
  {
    LOG_MSG_INFO1("Adding First client tunnel",0,0,0);
    ret_val = ioctl(qmi_ip_conf->embms_device_fd,
                    START_EMBMS_TUNNELING,
                    &tun_inst);

    if(ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Unable to start tunnel module", 0, 0 ,0);
      return FAILURE;
    }
  }

  LOG_MSG_INFO1("Sending data to fd %d",qmi_ip_conf->embms_device_fd,0,0);
  ret_val = ioctl(qmi_ip_conf->embms_device_fd,
                  ADD_EMBMS_TUNNEL_FOR_CLIENT,
                  &tun_inst);

  if(ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Unable to send msg to tunnel module", 0, 0 ,0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  Enable the TMGI
----------------------------------------------------------------------------*/
  qmi_ip_conf->tmgi_instance[tmgi_index].tmgi_status = QMI_IP_TUNN_INST_ENABLED;

  return SUCCESS;
}

/*==========================================================================

FUNCTION QMI_IP_ACTIVATE_TMGI_INST()

DESCRIPTION

  Activate the TMGI in TMGI DB. This happens based on QMI activate req.
  Need to wait for indication for TMGI to be fully enabled.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_activate_tmgi_inst
(
  int                     sock_fd,
  uint32                  multicast_addr,
  uint16                  multicast_port,
  qmi_ip_embms_tmgi_t     *embms_tmgi
)
{
  int                      cur_cnt;
  int                      client_id;
  int                      ref_cnt;
  int                      i;
  int                      tmgi_index;

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Get client id based on fd
----------------------------------------------------------------------------*/
  client_id = qmi_ip_get_client_id(sock_fd);
  if(client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid socket fd %d passed", sock_fd, 0, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("TMGI to be activated for client %d", sock_fd, 0, 0);
  LOG_MSG_INFO1_6("TMGI to be activated tun %d %d %d %d %d %d",
                  embms_tmgi->tmgi[0],
                  embms_tmgi->tmgi[1],
                  embms_tmgi->tmgi[2],
                  embms_tmgi->tmgi[3],
                  embms_tmgi->tmgi[4],
                  embms_tmgi->tmgi[5]);
/*---------------------------------------------------------------------------
  Get TMGI ID based on tmgi
----------------------------------------------------------------------------*/
  tmgi_index = qmi_ip_get_tmgi_id(embms_tmgi);
  if(tmgi_index != FAILURE)
  {
    ref_cnt = qmi_ip_conf->tmgi_instance[tmgi_index].ref_count;
    for(i=0; i < ref_cnt; i++)
    {
      if(qmi_ip_conf->tmgi_instance[tmgi_index].client_fds[i] == sock_fd)
      {
        LOG_MSG_ERROR("Duplicate TMGI activate req from client fd %d", sock_fd, 0, 0);
        return FAILURE;
      }
    }
/*---------------------------------------------------------------------------
  Increment ref count if TMGI already present in DB
----------------------------------------------------------------------------*/
    LOG_MSG_INFO1("TMGI already present in DB.Will increment ref cnt for TMGI %d",
                   tmgi_index, 0, 0);

    qmi_ip_conf->tmgi_instance[tmgi_index].client_fds[ref_cnt] = sock_fd;
    qmi_ip_conf->tmgi_instance[tmgi_index].ref_count++;
    LOG_MSG_INFO1("Current TMGI ref count  %d",
                  qmi_ip_conf->tmgi_instance[tmgi_index].ref_count, 0, 0);
  }
  else
  {
/*---------------------------------------------------------------------------
  Return error if we have reached the max# number supported by modem.
----------------------------------------------------------------------------*/
    if(qmi_ip_conf->current_tmgi_inst_cnt == MAX_TMGI_INSTANCES)
    {
      LOG_MSG_ERROR("Reached the max# of TMGI instances modem can support %d",
                    MAX_TMGI_INSTANCES,
                    0,
                    0);
      return FAILURE;
    }
/*---------------------------------------------------------------------------
  Return error if we have reached the max# configured by user.
----------------------------------------------------------------------------*/
    if(qmi_ip_conf->current_tmgi_inst_cnt == qmi_ip_conf->tmgi_count_allowed)
    {
      LOG_MSG_ERROR("Reached the max# of TMGI instances configured by user %d",
                    qmi_ip_conf->tmgi_count_allowed,
                    0,
                    0);
      return FAILURE;
    }
/*---------------------------------------------------------------------------
  If TMGI not present in DB, add to DB
----------------------------------------------------------------------------*/
    LOG_MSG_INFO1("TMGI not present in DB. Will add instance %d",
                  qmi_ip_conf->current_tmgi_inst_cnt, 0, 0);

    cur_cnt = qmi_ip_conf->current_tmgi_inst_cnt;
    memset(&qmi_ip_conf->tmgi_instance[cur_cnt],
           0,
           sizeof(qmi_ip_tmgi_ctrl_t));
    memcpy(qmi_ip_conf->tmgi_instance[cur_cnt].client_tmgi.tmgi,
           embms_tmgi->tmgi,
           sizeof(qmi_ip_embms_tmgi_t));

    qmi_ip_conf->tmgi_instance[cur_cnt].multicast_addr =
                                                      multicast_addr;

    qmi_ip_conf->tmgi_instance[cur_cnt].multicast_port =
                                                      multicast_port;


    ref_cnt = qmi_ip_conf->tmgi_instance[cur_cnt].ref_count;
    qmi_ip_conf->tmgi_instance[cur_cnt].client_fds[ref_cnt] = sock_fd;
    qmi_ip_conf->tmgi_instance[cur_cnt].ref_count++;
    LOG_MSG_INFO1("Current TMGI ref count  %d",
                  qmi_ip_conf->tmgi_instance[cur_cnt].ref_count, 0, 0);

    qmi_ip_conf->tmgi_instance[cur_cnt].tmgi_status = QMI_IP_TUNN_INST_ENABLING;

    qmi_ip_conf->current_tmgi_inst_cnt++;
    LOG_MSG_INFO1("Current TMGI inst count  %d", qmi_ip_conf->current_tmgi_inst_cnt, 0, 0);
  }

  LOG_MSG_INFO1("Successfully activated TMGI for client %d", sock_fd, 0, 0);

  return SUCCESS;
}

/*==========================================================================

FUNCTION QMI_IP_DELETE_CLIENT_TUNNEL()

DESCRIPTION

  Deletes the tunnels associated with the client.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_delete_client_tunnel
(
  int  sock_fd
)
{
  qmi_ip_tun_mgmt_info_t   tun_inst;
  int                      client_id;
  int                      ret_val;

/*-----------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Get client id based on fd
----------------------------------------------------------------------------*/
  client_id = qmi_ip_get_client_id(sock_fd);
  if(client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid socket fd %d passed for tunnel deletion for client", sock_fd, 0, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("Delete tunnels for client id %d", client_id, 0, 0);
/*---------------------------------------------------------------------------
  Send message to tunneling module indicating the client disconnected.
----------------------------------------------------------------------------*/
  memset(&tun_inst, 0, sizeof(qmi_ip_tun_mgmt_info_t));
  tun_inst.tmgi_multicast_addr = 0;
  tun_inst.tmgi_multicast_port = 0;
  if(qmi_ip_conf->mode == MULTI_CLIENT_MODE)
  {
    tun_inst.client_addr = qmi_ip_conf->embms_client[client_id].private_ip.s_addr;
    tun_inst.client_port = qmi_ip_conf->embms_client[client_id].udp_data_port;
  }
/*---------------------------------------------------------------------------
  In single mode this function is called when last TMGI deactivates.
----------------------------------------------------------------------------*/
  else if(qmi_ip_conf->mode == SINGLE_CLIENT_MODE)
  {
    tun_inst.client_addr = qmi_ip_conf->embms_client[client_id].client_ip.s_addr;
    tun_inst.client_port = qmi_ip_conf->single_client_udp_port;
  }
  memcpy(tun_inst.interface_name,
         qmi_ip_conf->qmi_info.net_iface_name,
         sizeof(qmi_ip_conf->qmi_info.net_iface_name));
  tun_inst.action_type = CLIENT_DEACTIVATE_ALL_TMGI;

/*---------------------------------------------------------------------------
    Send it to tunneling module
----------------------------------------------------------------------------*/
  ret_val = ioctl(qmi_ip_conf->embms_device_fd,
                  CLIENT_DEACTIVATE,
                  &tun_inst);

  if(ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to send msg to tunnel module", 0, 0, 0);
    return FAILURE;
  }

  return SUCCESS;
}
/*==========================================================================

FUNCTION QMI_IP_TUNNEL_DELETION()

DESCRIPTION

  Deletes the TMGI based on deactivate indication.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_tunnel_deletion
(
  int                      sock_fd,
  qmi_ip_embms_tmgi_t      *tmgi
)
{
  int                      tmgi_id;
  int                      client_id;
  int                      i,j;
  qmi_ip_tun_mgmt_info_t   tun_inst;
  int                      ret_val;

/*---------------------------------------------------------------------------
  Get the TMGI ID based on TMGI
----------------------------------------------------------------------------*/
  tmgi_id = qmi_ip_get_tmgi_id(tmgi);
  if(tmgi_id == FAILURE)
  {
    LOG_MSG_ERROR("Failed to get TMGI ID", 0, 0, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("TMGI to be deleted for client %d", sock_fd, 0, 0);
  LOG_MSG_INFO1_6("TMGI to be deleted tun %d %d %d %d %d %d",
                  tmgi->tmgi[0],
                  tmgi->tmgi[1],
                  tmgi->tmgi[2],
                  tmgi->tmgi[3],
                  tmgi->tmgi[4],
                  tmgi->tmgi[5]);
/*---------------------------------------------------------------------------
  Get client id based on fd.
  Dont return on failure to handle cases where the client has closed
  connection, we still need to handle TMGI clean up
----------------------------------------------------------------------------*/
  client_id = qmi_ip_get_client_id(sock_fd);

  if(client_id == FAILURE)
  {
    LOG_MSG_ERROR("Invalid socket fd %d passed", sock_fd, 0, 0);
  }
/*---------------------------------------------------------------------------
  In single client mode, only if all the TMGI have been deactivated,
  send message to tunneling module to deactivate all TMGIs for the client.
----------------------------------------------------------------------------*/
  else if(qmi_ip_conf->mode == SINGLE_CLIENT_MODE &&
          qmi_ip_conf->current_tmgi_inst_cnt == 1)
  {
    ret_val = qmi_ip_delete_client_tunnel(sock_fd);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Error in deactivating tunnels for client fd %d", sock_fd, 0, 0);
    }
  }
/*---------------------------------------------------------------------------
  In multi client mode sent message to tunneling module to stop tunneling
  TMGI to a client.
----------------------------------------------------------------------------*/
  else if(qmi_ip_conf->mode == MULTI_CLIENT_MODE)
  {
    LOG_MSG_INFO1("Delete tunnel for TMGI id is %d", tmgi_id, 0, 0);
/*---------------------------------------------------------------------------
  Populate the contents to send to tunneling module
----------------------------------------------------------------------------*/
    memset(&tun_inst, 0, sizeof(qmi_ip_tun_mgmt_info_t));
    tun_inst.tmgi_multicast_addr = qmi_ip_conf->tmgi_instance[tmgi_id].multicast_addr;
    tun_inst.tmgi_multicast_port = qmi_ip_conf->tmgi_instance[tmgi_id].multicast_port;
    tun_inst.client_addr = qmi_ip_conf->embms_client[client_id].private_ip.s_addr;
    tun_inst.client_port = qmi_ip_conf->embms_client[client_id].udp_data_port;
    tun_inst.action_type = DELETE_ENTRY;

    memcpy(tun_inst.interface_name,
           qmi_ip_conf->qmi_info.net_iface_name,
           sizeof(qmi_ip_conf->qmi_info.net_iface_name));

    LOG_MSG_INFO1("Multicast IPV4 address to be deleted:", 0, 0, 0);
    IPV4_ADDR_MSG(tun_inst.tmgi_multicast_addr);
    LOG_MSG_INFO1("Multicast port to be deleted %d", ntohs(tun_inst.tmgi_multicast_port), 0, 0);
    LOG_MSG_INFO1("CLient IPV4 address to be deleted:", 0, 0, 0);
    IPV4_ADDR_MSG(tun_inst.client_addr);
    LOG_MSG_INFO1("Client port to be deleted %d", ntohs(tun_inst.client_port), 0, 0);
/*---------------------------------------------------------------------------
    Send it to tunneling module
----------------------------------------------------------------------------*/
    ret_val = ioctl(qmi_ip_conf->embms_device_fd,
                    DELETE_EMBMS_TUNNEL_FOR_CLIENT,
                    &tun_inst);

    if(ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Failed to send msg to tunnel module", 0, 0, 0);
      return FAILURE;
    }
  }

  LOG_MSG_INFO1("Delete tunnel for TMGI id is %d", tmgi_id, 0, 0);
/*---------------------------------------------------------------------------
  If not last deactivate ind decrement TMGI client ref count
----------------------------------------------------------------------------*/
  if(qmi_ip_conf->tmgi_instance[tmgi_id].ref_count > 1)
  {
    LOG_MSG_INFO1(" Will decrement ref cnt for TMGI ID %d, ref cnt %d",
                  tmgi_id,
                  qmi_ip_conf->tmgi_instance[tmgi_id].ref_count,
                  0);
    for(i = 0; i < qmi_ip_conf->tmgi_instance[tmgi_id].ref_count; i++)
    {
      if(qmi_ip_conf->tmgi_instance[tmgi_id].client_fds[i] == sock_fd)
      {
        qmi_ip_conf->tmgi_instance[tmgi_id].client_fds[i] = 0;
        for(j=i; (j+1) < qmi_ip_conf->tmgi_instance[tmgi_id].ref_count; j++)
        {
          qmi_ip_conf->tmgi_instance[tmgi_id].client_fds[j] =
            qmi_ip_conf->tmgi_instance[tmgi_id].client_fds[j+1];
        }
        qmi_ip_conf->tmgi_instance[tmgi_id].client_fds[qmi_ip_conf->tmgi_instance[tmgi_id].ref_count -1] = 0;
        qmi_ip_conf->tmgi_instance[tmgi_id].ref_count--;
        break;
      }
    }

    LOG_MSG_INFO1("Decremented ref cnt for TMGI ID %d, ref cnt %d",
                  tmgi_id,
                  qmi_ip_conf->tmgi_instance[tmgi_id].ref_count,
                  0);
  }
  else if(qmi_ip_conf->tmgi_instance[tmgi_id].ref_count == 1)
  {
    LOG_MSG_INFO1("Last client for TMGI ID %d", tmgi_id, 0, 0);
/*---------------------------------------------------------------------------
  If  last deactivate ind delete TMGI
----------------------------------------------------------------------------*/
    qmi_ip_conf->tmgi_instance[tmgi_id].ref_count = 0;
    memset(&qmi_ip_conf->tmgi_instance[tmgi_id], 0, sizeof(qmi_ip_tmgi_ctrl_t));
    for(i=tmgi_id; (i+1) < qmi_ip_conf->current_tmgi_inst_cnt; i++)
    {
      memcpy(&qmi_ip_conf->tmgi_instance[i],
             &qmi_ip_conf->tmgi_instance[i+1],
             sizeof(qmi_ip_tmgi_ctrl_t));
    }

    if(qmi_ip_conf->current_tmgi_inst_cnt > 0)
    {
      memset(&qmi_ip_conf->tmgi_instance[qmi_ip_conf->current_tmgi_inst_cnt-1],
             0,
             sizeof(qmi_ip_tmgi_ctrl_t));
      qmi_ip_conf->current_tmgi_inst_cnt--;
    }
    LOG_MSG_INFO1("TMGI curr cnt %d",
                  qmi_ip_conf->current_tmgi_inst_cnt,
                  0,
                  0);
  }

  return SUCCESS;
}

/*==========================================================================

FUNCTION QMI_IP_DEACTIVATE_TMGI_INST()

DESCRIPTION

  Deactivate based on dectivate req. Need to wait till deactivate indication
  to fully delete TMGI

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_deactivate_tmgi_inst
(
  int                     sock_fd,
  qmi_ip_embms_tmgi_t     *embms_tmgi
)
{

  int    tmgi_id;
  int    i;
  int    ref_cnt;
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Get TMGI id based on fd
----------------------------------------------------------------------------*/
  tmgi_id = qmi_ip_get_tmgi_id(embms_tmgi);
  if(tmgi_id == FAILURE)
  {
    LOG_MSG_ERROR("Failed to get TMGI ID", 0, 0, 0);
    return FAILURE;
  }
/*---------------------------------------------------------------------------
  See if the client has activated this TMGI before deactivating on its behalf.
----------------------------------------------------------------------------*/
  ref_cnt = qmi_ip_conf->tmgi_instance[tmgi_id].ref_count;
  for(i=0; i < ref_cnt; i++)
  {
    if(qmi_ip_conf->tmgi_instance[tmgi_id].client_fds[i] == sock_fd)
    {
      LOG_MSG_INFO1("TMGI %d to be deactivated for client fd %d found", tmgi_id, sock_fd, 0);
      break;
    }
  }

  if(i == ref_cnt)
  {
    LOG_MSG_ERROR("Client fd %d has not activated TMGI %d", sock_fd, tmgi_id, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("TMGI to be deactivated for client %d", sock_fd, 0, 0);
  LOG_MSG_INFO1_6("TMGI to be deactivated tun %d %d %d %d %d %d",
                  embms_tmgi->tmgi[0],
                  embms_tmgi->tmgi[1],
                  embms_tmgi->tmgi[2],
                  embms_tmgi->tmgi[3],
                  embms_tmgi->tmgi[4],
                  embms_tmgi->tmgi[5]);
/*---------------------------------------------------------------------------
  Mark it for deactivating. But nothing much is done based on req.
  Most of the processing is done based on indication.
----------------------------------------------------------------------------*/
  if(qmi_ip_conf->tmgi_instance[tmgi_id].ref_count == QMI_IP_ACTIVE)
  {
    LOG_MSG_INFO1("Disabling TMGI id is %d", tmgi_id, 0, 0);
    qmi_ip_conf->tmgi_instance[tmgi_id].tmgi_status = QMI_IP_TUNN_INST_DISABLING;
  }
  else if(qmi_ip_conf->tmgi_instance[tmgi_id].ref_count > QMI_IP_ACTIVE)
  {
    LOG_MSG_INFO1("Deactivate TMGI id is %d, Will decrement count once ind is received",
                  tmgi_id,
                  0,
                  0);
  }

  return SUCCESS;
}


/*==========================================================================

FUNCTION QMI_IP_TUN_MGMT_INIT()

DESCRIPTION

  Initialize tunneling module

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_tun_mgmt_init
(
  qmi_ip_conf_t *qmi_ip_cfg
)
{
  int retry_count = 0;
  int ret_val = 0;
  qmi_ip_conf = qmi_ip_cfg;
  qmi_ip_tun_mgmt_info_t    tun_inst;

  while ( retry_count < 10)
  {
    qmi_ip_conf->embms_device_fd = open(EMBMS_DEVICE_FILE, O_RDWR);

    if (qmi_ip_conf->embms_device_fd < 0)
    {
      LOG_MSG_INFO1("Try %d : Can't open device file: %s , Error : %d \n",
					  retry_count + 1,EMBMS_DEVICE_FILE,errno);
      sleep(1);
      retry_count++;
      continue;
    }
    else
    {
      LOG_MSG_INFO1("Successfully opened peripheral device file. FD is %d \n",
                    qmi_ip_conf->embms_device_fd,0,0);
      break;
    }
  }

  if (retry_count == 10)
  {
    LOG_MSG_ERROR("Error opening embms tm device",0,0,0);
    return FAILURE;
  }

  return SUCCESS;
}

