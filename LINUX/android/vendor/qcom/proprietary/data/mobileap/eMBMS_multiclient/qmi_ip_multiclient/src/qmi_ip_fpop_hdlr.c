/******************************************************************************

                        QMI_IP_FPOP_HDLR.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_fpop_hdlr.c
  @brief   QMI over IP FPOP Handler

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
06/26/14   sb         Initial version
10/01/14   mp         Added filter to read socket to read only FPOP packets

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_ip.h"

static qmi_ip_conf_t *qmi_ip_config;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION QMI_IP_FPOP_SIGNAL_HDLR()

DESCRIPTION

  Handles the FPOP timer expiry for client.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure

SIDE EFFECTS
  None

=========================================================================*/
void qmi_ip_fpop_signal_hdlr
(
  int          sig,
  siginfo_t    *si,
  void         *uc
)
{
  int       client_fd;
  int       ret_val;

/*-----------------------------------------------------------------------*/

  if (si == NULL || uc == NULL)
  {
    LOG_MSG_ERROR("Invalid arguements in fpop signal handler", 0, 0, 0);
    return;
  }

  if (sig != SIGUSR1)
  {
    LOG_MSG_ERROR("Unhandled signal %d", sig, 0, 0);
  }

  LOG_MSG_INFO1("FPOP timer expired for client fd %d",
                si->si_value.sival_int,
                0,
                0);

  client_fd = si->si_value.sival_int;
/*---------------------------------------------------------------------------
  Close connection associated with fd
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_close_connection(client_fd);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in closing connection for client fd %d", client_fd, 0, 0);
  }

  return;
}

/*===========================================================================

FUNCTION QMI_IP_FPOP_MESSAGE_HDLR()

DESCRIPTION

  Handles the client's state maintainence based on FPOP messages.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_message_hdlr
(
  char    *buf,
  int     buf_len
)
{
  qmi_ip_fpop_msg_t  *fpop_msg;
  int                client_index;
  uint32             client_ip;
  uint32             private_ip;
  uint16             private_port;
  int                ret_val;
  int                i;
  int                client_fd = 0;
  uint8              client_status = QMI_IP_CLIENT_INACTIVE;
  struct sockaddr_in client_addr;
  int                bytes_write;
/*--------------------------------------------------------------------------*/

  fpop_msg =(qmi_ip_fpop_msg_t *)buf;
  client_ip = fpop_msg->client_ip;
  private_ip = fpop_msg->private_ip;
  private_port = fpop_msg->private_port;


  LOG_MSG_INFO1("Client IP = ",0,0,0);
  IPV4_ADDR_MSG(client_ip);
/*---------------------------------------------------------------------------
  Check if the client IP is in the client table.
  Check if it is active.
----------------------------------------------------------------------------*/
  for (i = 0; i < qmi_ip_config->current_client_cnt; i++)
  {
    if ((qmi_ip_config->embms_client[i].client_ip.s_addr == client_ip) &&
        qmi_ip_config->embms_client[i].is_active)
    {
      LOG_MSG_INFO1("Setting Client public IP and port",0,0,0);
      qmi_ip_config->embms_client[i].private_ip.s_addr = private_ip;
      LOG_MSG_INFO1("Client Public IP = ",0,0,0);
      IPV4_ADDR_MSG(qmi_ip_config->embms_client[i].private_ip.s_addr);
      qmi_ip_config->embms_client[i].udp_data_port = private_port;
      LOG_MSG_INFO1("Client public port = %d",
                    qmi_ip_config->embms_client[i].udp_data_port,0,0);

      client_fd = qmi_ip_config->embms_client[i].socket_fd;
      client_status = QMI_IP_CLIENT_ACTIVE;
      LOG_MSG_INFO1("Client status is active for client id %d with fd %d", i, client_fd, 0);
      break;
    }
  }
/*---------------------------------------------------------------------------
  If client is not found then its inactive.
----------------------------------------------------------------------------*/
  if (client_fd == 0)
  {
    LOG_MSG_ERROR("Client IP not found or is inactive", 0, 0, 0);
  }
  else
  {
/*---------------------------------------------------------------------------
  Restart FPOP timer for the client
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_fpop_set_client_timer(client_fd);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Error in setting the timer for client fd %d", client_fd, 0, 0);
    }
  }

  LOG_MSG_INFO1("Client status %d for private ip 0x%x", client_status, client_ip, 0);
/*---------------------------------------------------------------------------
  Populate the FPOP response and send it back to client.
----------------------------------------------------------------------------*/
  memset(&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = private_port;
  client_addr.sin_addr.s_addr = private_ip;

  if (qmi_ip_config->socket_info.fpop_write_socket)
  {
    bytes_write = sendto(qmi_ip_config->socket_info.fpop_write_socket,
                         &client_status,
                         sizeof(uint8),
                         0,
                         (struct sockaddr *)&client_addr,
                         sizeof(client_addr));

    if (bytes_write < 0)
    {
      LOG_MSG_ERROR("Error in writing to FPOP write socket %d", errno, 0, 0);
    }

    LOG_MSG_INFO1("Successfully wrote %d bytes to FPOP write socket fd ",
                  bytes_write,
                  qmi_ip_config->socket_info.fpop_write_socket, 0);
  }
  else
  {
    LOG_MSG_ERROR("FPOP write socket %d not initialized",
                  qmi_ip_config->socket_info.fpop_write_socket, 0, 0);
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_FPOP_MESSAGE_WRITE()

DESCRIPTION

  Writes the FPOP contents to an FPOP socket.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_message_write
(
  uint32 client_ip,
  uint32 private_ip,
  uint16 private_port
)
{
  qmi_ip_fpop_msg_t     fpop_msg;
  struct sockaddr_un    ind_sock;
  int                   sock_fd;
  int                   len;
  int                   numBytes;
/*-------------------------------------------------------------------------*/

  memset(&fpop_msg, 0, sizeof(qmi_ip_fpop_msg_t));

  fpop_msg.client_ip = client_ip;
  fpop_msg.private_ip = private_ip;
  fpop_msg.private_port = private_port;

/*---------------------------------------------------------------------------
  Open Unix domain socket and send the FPOP data to FPOP hdlr
  socket. This is done to make sure all the FPOP messages are
  processed in QMI_IP context.
----------------------------------------------------------------------------*/
  ind_sock.sun_family = AF_UNIX;
  strlcpy(ind_sock.sun_path, QMI_IP_FPOP_FILE,sizeof(QMI_IP_FPOP_FILE));
  len = strlen(ind_sock.sun_path) + sizeof(ind_sock.sun_family);

  if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Error creating UNIX socket for sending FPOP msg, errno: %d", errno, 0, 0);
    return FAILURE;
  }

  if ((numBytes = sendto(sock_fd,
                        (void *)&fpop_msg,
                        sizeof(qmi_ip_fpop_msg_t),
                        0,
                        (struct sockaddr *)&ind_sock,
                        len)) == -1)
  {
    LOG_MSG_ERROR("Send failed from FPOP send msg context", 0, 0, 0);
    close(sock_fd);
    return FAILURE;
  }

  LOG_MSG_INFO1("Successfully wrote %d bytes to FPOP handler socket", numBytes, 0, 0);

  close(sock_fd);
  return SUCCESS;
}

/*=============================================================================

FUNCTION QMI_IP_FPOP_CREATE_WRITE_SOCKET()

DESCRIPTION

  Creates an FPOP write socket.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_fpop_create_socket
(
  int * write_fd
)
{

  int                 val;
  uint32              send_sock_ip;
  struct ifreq        req_if_info;
  struct sockaddr_in  bindaddr;
  struct sockaddr     *send_sock_addr;
  int                 sockfd;
  int                 reuse;

/*-----------------------------------------------------------------------------
  Creating simple UDP socket for sending FPOP response to clients
-----------------------------------------------------------------------------*/
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    LOG_MSG_ERROR("Open send socket err %d", errno, 0, 0);
    return FAILURE;
  }

/*----------------------------------------------------------------------------
  Bind to bridge interface
----------------------------------------------------------------------------*/
  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, BRIDGE_IFACE,sizeof(BRIDGE_IFACE));

  val = ioctl(sockfd, SIOCGIFADDR, &req_if_info);
  if(val < 0)
  {
    LOG_MSG_ERROR("Get bridge iface address failed",0,0,0);
    close(sockfd);
    return FAILURE;
  }

  send_sock_addr = (struct sockaddr *)&(req_if_info.ifr_addr);
  send_sock_ip = ((struct sockaddr_in *)send_sock_addr)->sin_addr.s_addr;
  LOG_MSG_INFO1( "Get bridge0 ipaddr=0x%08x, return %d", send_sock_ip, val, 0);

  memset(&bindaddr, 0, sizeof(bindaddr));
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = send_sock_ip;
  bindaddr.sin_port = qmi_ip_config->fpop_udp_port;

  reuse = 1;
  val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int));
  if(val < 0)
  {
    LOG_MSG_ERROR("set socket opt REUSEPORT failed ",0,0,0);
    close(sockfd);
    return FAILURE;
  }

  val = bind(sockfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
  if(val < 0)
  {
    LOG_MSG_ERROR("Bind send socket on bridge0 failed. err %d.", errno, 0, 0);
    close(sockfd);
    return FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Bind send socket on bridge0 succeeded", 0, 0, 0);
  }

  *write_fd = sockfd;

  return SUCCESS;

}

/*===========================================================================

FUNCTION QMI_IP_FPOP_LISTENER_START()

DESCRIPTION

  Handles FPOP packet read and write.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_listener_start()
{
  int                       read_fd;
  int                       write_fd;
  int                       ret_val;
  int                       bytes_read;
  int                       bytes_write;
  char                      read_data_buf[QMI_IP_READ_BUF_SIZE];
  uint32                    src_addr;
  uint16                    src_port;
  uint8                     src_client_ip_type;
  uint32                    src_client_ip;
  int                       i,j, val, rval, fromlen, reuse;
  struct                    sockaddr_in bindaddr;
  struct                    timeval rcv_timeo;
  struct                    sockaddr_in sender_addr;
  socklen_t                 sender_len = sizeof(sender_addr);

/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Create FPOP read socket.
------------------------------------------------------------------------------*/

  ret_val = qmi_ip_fpop_create_socket(&read_fd);
  if( ret_val != SUCCESS )
  {
    LOG_MSG_ERROR("failed to create fpop read socket ",0,0,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("FPOP listening start",0,0,0);

  while (TRUE)
  {
    bytes_read = recvfrom(read_fd,
                          read_data_buf,
                          QMI_IP_READ_BUF_SIZE,
                          0,
                          (struct sockaddr *)&sender_addr,
                          &sender_len);

    if (bytes_read < 0)
    {
      LOG_MSG_ERROR("Error in reading data from FPOP read socket", 0, 0, 0);
    }
    else if (sender_len > sizeof(sender_addr))
    {
       LOG_MSG_ERROR("receive Invalid Sender socket length", 0, 0, 0);
    }
    else if (bytes_read == QMI_IP_FPOP_PKT_SIZE)
    {
      LOG_MSG_INFO1("Received FPOP from A7 middleware",0,0,0);
/*----------------------------------------------------------------------------
  Get the source IP of the FPOP packet: We send FPOP responses and tunneled
  multicast packets to this IP.
------------------------------------------------------------------------------*/

      src_addr = sender_addr.sin_addr.s_addr;
      LOG_MSG_INFO1("Src IP address:", 0, 0, 0);
      IPV4_ADDR_MSG(src_addr);
/*----------------------------------------------------------------------------
  Get the source port of the FPOP packet: We send FPOP responses and tunneled
  multicast packets to this port.
------------------------------------------------------------------------------*/
      src_port = sender_addr.sin_port;
      LOG_MSG_INFO1("Src port %d", ntohs(src_port), 0, 0);
/*----------------------------------------------------------------------------
  Parse the payload. Here IP type signifies the end client IP type
------------------------------------------------------------------------------*/
      src_client_ip_type =
        (uint8)read_data_buf[SRC_CLIENT_IP_TYPE_OFFSET];

      LOG_MSG_INFO1("Src IP client IP type %d", src_client_ip_type, 0, 0);

      if (src_client_ip_type != IP_V4)
      {
        LOG_MSG_ERROR("FPOP packet IP type not supported %d",
                      src_client_ip_type, 0, 0);
        continue;
      }
/*----------------------------------------------------------------------------
  Parse the payload. Client IP is extracted. It is used to match against the
  client status (active/inactive) and send FPOP responses accordingly.
------------------------------------------------------------------------------*/
      src_client_ip = 0;
      for (j =SRC_CLIENT_IP_END; j >= SRC_CLIENT_IP_START; j--)
      {
        src_client_ip = (src_client_ip << 8);
        src_client_ip = src_client_ip + ((int)read_data_buf[j]);
      }
      LOG_MSG_INFO1("Src Client IP address 0x%08x", src_client_ip, 0, 0);
      LOG_MSG_INFO1("Src Client IP address:", 0, 0, 0);
      IPV4_ADDR_MSG(src_client_ip);
/*----------------------------------------------------------------------------
  Send FPOP data to FPOP socket for processing.
------------------------------------------------------------------------------*/
      ret_val = qmi_ip_fpop_message_write(src_client_ip, src_addr, src_port);
      if (ret_val != SUCCESS)
      {
        LOG_MSG_ERROR("Error in sending FPOP msg to QMI IP thread", 0, 0, 0);
      }
    }
    else
    {
      LOG_MSG_ERROR("Received NON FPOP Packet",0,0,0);
    }
  }
/*----------------------------------------------------------------------------
  On exit of the loop close read and write sockets.
------------------------------------------------------------------------------*/
  if (read_fd)
  {
    close(read_fd);
  }

  if (qmi_ip_config->socket_info.fpop_write_socket)
  {
    close(qmi_ip_config->socket_info.fpop_write_socket);
  }

}

/*===========================================================================

FUNCTION QMI_IP_FPOP_LISTENER()

DESCRIPTION

  Starts the thread to listen to FPOP packets.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_listener()
{
  pthread_t  qmi_ip_fpop_thread = 0;
  int        ret;
/*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
  Create a thread to monitor FPOP messages
---------------------------------------------------------------------------*/
  ret = pthread_create(&qmi_ip_fpop_thread,
                       NULL,
                       qmi_ip_fpop_listener_start,
                       NULL);
  if (SUCCESS != ret)
  {
    LOG_MSG_ERROR("Unable to create FPOP thread for QMI_IP. Err: %d",ret,0,0);
    return FAILURE;
  }

  LOG_MSG_INFO1("created FPOP thread for QMI IP", 0, 0, 0);
  return SUCCESS;

}



/*===========================================================================

FUNCTION QMI_IP_FPOP_SET_CLIENT_TIMER()

DESCRIPTION

  Sets the FPOP timer associated with the client.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_set_client_timer
(
  int client_fd
)
{
  struct            itimerspec its;
  int               client_index;
/*------------------------------------------------------------------------*/

  client_index = qmi_ip_get_client_id(client_fd);
  if (client_index == FAILURE)
  {
    LOG_MSG_ERROR("Incorrect client fd passed %d", client_fd, 0, 0);
    return FAILURE;
  }
/*--------------------------------------------------------------------------
  Populate timer contents
--------------------------------------------------------------------------*/
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec =0;
  its.it_value.tv_sec = ((qmi_ip_config->fpop_threshold)*(qmi_ip_config->fpop_timeout)) + QMI_IP_FPOP_GRACE_PERIOD;

  LOG_MSG_INFO1("FPOP timeout %d FPOP threshold %d",
                qmi_ip_config->fpop_threshold,
                qmi_ip_config->fpop_timeout,
                0);
  LOG_MSG_INFO1("Timer set for %d seconds for client %d", its.it_value.tv_sec, client_fd, 0);
/*--------------------------------------------------------------------------
  Set the FPOP timer.
--------------------------------------------------------------------------*/
  if (timer_settime(qmi_ip_config->embms_client[client_index].timerid,
                    0,
                    &its,
                    NULL) == -1)
  {
    LOG_MSG_ERROR("failed to set timer for client fd %d", client_fd, 0, 0);
    return FAILURE;
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_FPOP_CREATE_CLIENT_TIMER()

DESCRIPTION

  Creates the FPOP timer associated with the client.

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_create_client_timer
(
  int client_fd
)
{
  struct sigevent sev;
  int    client_index;
  int    ret_val;

/*----------------------------------------------------------------------*/

  client_index = qmi_ip_get_client_id(client_fd);
  if (client_index == FAILURE)
  {
    LOG_MSG_ERROR("Incorrect client fd passed %d", client_fd, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------
  Create the timer
------------------------------------------------------------------------*/
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGUSR1;
  sev.sigev_value.sival_int = client_fd;
  if (timer_create(CLOCK_REALTIME,
                   &sev,
                   &(qmi_ip_config->embms_client[client_index].timerid)) == -1)
  {
    LOG_MSG_ERROR("Error in creating a timer for client fd %d. errno %d",
                  client_fd, errno, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("Timer ID is 0x%lx for client fd %d",
                (long) qmi_ip_config->embms_client[client_index].timerid,
                client_fd,
                0);
/*-----------------------------------------------------------------------
  Set the timer
------------------------------------------------------------------------*/
  ret_val = qmi_ip_fpop_set_client_timer(client_fd);
  if (ret_val !=SUCCESS)
  {
    LOG_MSG_ERROR("Error is setting timer for client fd %", client_fd, 0, 0);
    return FAILURE;
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_FPOP_DELETE_CLIENT_TIMER()

DESCRIPTION

  Deletes the FPOP timer associated with the client

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_fpop_delete_client_timer
(
  int client_fd
)
{
  int    client_index;

/*----------------------------------------------------------------------*/

  client_index = qmi_ip_get_client_id(client_fd);
  if (client_index == FAILURE)
  {
    LOG_MSG_ERROR("Incorrect client fd passed %d", client_fd, 0, 0);
    return FAILURE;
  }

/*-----------------------------------------------------------------------
  Check if the timer is set.
------------------------------------------------------------------------*/
  if (!qmi_ip_config->embms_client[client_index].init_done)
  {
    LOG_MSG_ERROR("Init not done. So timer not set", 0, 0, 0);
    return FAILURE;
  }
/*-----------------------------------------------------------------------
  Delete the timer
------------------------------------------------------------------------*/
  if (timer_delete(qmi_ip_config->embms_client[client_index].timerid) == -1)
  {
    LOG_MSG_ERROR("Error in deleting a timer for client fd %d. errno %d",
                  client_fd, errno, 0);
    return FAILURE;
  }

  return SUCCESS;
}


/*===========================================================================

FUNCTION QMI_IP_FPOP_SIGNAL_HDLR_INIT()

DESCRIPTION

  Initializes the FPOP timer signal handler

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

===========================================================================*/
int qmi_ip_fpop_signal_hdlr_init()
{

  struct sigaction sa;
/*-----------------------------------------------------------------------
  Populate the callback function and signal for timer expiry
------------------------------------------------------------------------*/
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = qmi_ip_fpop_signal_hdlr;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, NULL) == -1)
  {
    LOG_MSG_ERROR("Error in registering signal handler", 0, 0, 0);
    return FAILURE;
  }

  return SUCCESS;

}

/*===========================================================================

FUNCTION QMI_IP_FPOP_HDLR_INIT()

DESCRIPTION

  Initializes the FPOP handler module

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for failure


SIDE EFFECTS
  None

===========================================================================*/
int qmi_ip_fpop_hdlr_init
(
  qmi_ip_conf_t *qmi_ip_cfg
)
{
  int ret_val;
/*-----------------------------------------------------------------------
  Initialize signal handler for FPOP timer expiry
------------------------------------------------------------------------*/
  qmi_ip_config = qmi_ip_cfg;
  ret_val =  qmi_ip_fpop_signal_hdlr_init();
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in registering signal handler function", 0, 0, 0);
  }
/*-----------------------------------------------------------------------
  Create the write socket for FPOP responses.
------------------------------------------------------------------------*/
  ret_val = qmi_ip_fpop_create_socket(
                              &(qmi_ip_config->socket_info.fpop_write_socket));
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to create FPOP write socket", 0, 0, 0);
    return FAILURE;
  }

  LOG_MSG_INFO1("Successfully created FPOP write socket %d",
                qmi_ip_config->socket_info.fpop_write_socket, 0, 0);
/*-----------------------------------------------------------------------
  Start the FPOP listener
------------------------------------------------------------------------*/
  ret_val = qmi_ip_fpop_listener();
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in starting fpop listener", 0, 0, 0);
  }

  return SUCCESS;
}
