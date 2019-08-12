/******************************************************************************

                        QMI_IP_CONN_MGMT.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_conn_mgmt.c
  @brief   QMI over IP Connection Management

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

static qmi_ip_conf_t *qmi_ip_config;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION PRINT_BUFFER()

DESCRIPTION

  This function prints the QMI buffer. Used for debugging purposes.

DEPENDENCIES
  None.

RETURN VALUE
  none


SIDE EFFECTS
  None

=========================================================================*/
void print_buffer
(
  char    *buf,
  int      size
)
{
  int i;

  for(i=0; i < size; i++)
  {
    if(i%8 == 0)
      printf("\n%02X ", buf[i]);
    else
      printf("%02X ", buf[i]);
  }
  printf("\n");
}


/*=============================================================================

FUNCTION QMI_IP_GET_CLIENT_ID_BY_IP()

DESCRIPTION
  This function gets the client index given the client IP.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_get_client_id_by_ip
(
  uint32    ip_addr
)
{
  int i;

  for(i=0; i< qmi_ip_config->current_client_cnt; i++)
  {
    if(ip_addr == qmi_ip_config->embms_client[i].client_ip.s_addr)
    {
      LOG_MSG_INFO1("Get client id for IP 0x%x is %d", ip_addr, i, 0);
      return i;
    }
  }

  LOG_MSG_INFO1("Get client id for IP 0x%x failed", ip_addr, 0, 0);
  return FAILURE;

}

/*=============================================================================

FUNCTION QMI_IP_CLOSE_CONNECTION()

DESCRIPTION
  This function closes client connections.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_close_connection
(
  int fd
)
{
  int                      i, j;
  int                      client_id;
  qmi_ip_tun_mgmt_info_t   tun_inst;
  int                      ret_val;
  FILE                    *fp;
  char                    *client_ip_addr;
/*---------------------------------------------------------------------------
  - Close FD
  - Clear FD
  - Remove FD and its mapping from FD list
----------------------------------------------------------------------------*/
  close(fd);

  FD_CLR(fd, &(qmi_ip_config->socket_info.fdset));

  for (i=0; i<qmi_ip_config->socket_info.num_of_fds; i++)
  {
    if (fd == qmi_ip_config->socket_info.sk_conf[i].socket_fd)
    {
      memset(&qmi_ip_config->socket_info.sk_conf[i], 0, sizeof(socket_conf_t));
      for (j=i; (j+1) <qmi_ip_config->socket_info.num_of_fds; j++)
      {
        memcpy(&qmi_ip_config->socket_info.sk_conf[j],
               &qmi_ip_config->socket_info.sk_conf[j+1],
               sizeof(socket_conf_t));
      }

      if (qmi_ip_config->socket_info.num_of_fds > 0)
      {
        memset(&qmi_ip_config->socket_info.sk_conf[qmi_ip_config->socket_info.num_of_fds -1],
               0,
               sizeof(socket_conf_t));
        qmi_ip_config->socket_info.num_of_fds--;
      }
      break;
    }
  }

  LOG_MSG_INFO1("Current client count is %d", qmi_ip_config->current_client_cnt, 0, 0);

  LOG_MSG_INFO1("removed fd %d, num of fds %d",
                fd,
                qmi_ip_config->socket_info.num_of_fds,
                0);

/*---------------------------------------------------------------------------
  Get client associated with FD
----------------------------------------------------------------------------*/
  client_id  = qmi_ip_get_client_id(fd);
  if (client_id == FAILURE)
  {
    LOG_MSG_ERROR("Failed to get client id", 0, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Deactivate TMGIs for the client
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_process_deactivate_client(fd);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in deactivating TMGIs for client fd %d", fd, 0, 0);
  }

/*---------------------------------------------------------------------------
  Delete tunnels associated with client
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_delete_client_tunnel(fd);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in deactivating tunnels for client fd %d", fd, 0, 0);
  }

/*---------------------------------------------------------------------------
  Delete FPOP timer for the client
----------------------------------------------------------------------------*/
  if (qmi_ip_config->mode == MULTI_CLIENT_MODE)
  {
    ret_val = qmi_ip_fpop_delete_client_timer(fd);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Error in deleting timer for client fd %d", fd, 0, 0);
    }
  }

/*---------------------------------------------------------------------------
  Logging for demo
----------------------------------------------------------------------------*/
  client_ip_addr = inet_ntoa(qmi_ip_config->embms_client[client_id].private_ip);

#ifdef FEATURE_DEMO_LOGGING
  fp = fopen("/usr/bin/out_logs.txt", "a");
  if (fp != NULL)
  {
    fprintf(fp, "\nDevice with IP %s disabled eMBMS over SoftAP\n", client_ip_addr);
    fclose(fp);
  }
#endif
/*---------------------------------------------------------------------------
  Clear the client entry in client table
----------------------------------------------------------------------------*/
  memset(&qmi_ip_config->embms_client[client_id],
         0,
         sizeof(qmi_ip_client_ctrl_table_t));

  for (i=client_id; (i+1)<qmi_ip_config->current_client_cnt; i++)
  {
    memcpy(&qmi_ip_config->embms_client[i],
           &qmi_ip_config->embms_client[i+1],
           sizeof(qmi_ip_client_ctrl_table_t));
  }

  if (qmi_ip_config->current_client_cnt > 0)
  {
    memset(&qmi_ip_config->embms_client[qmi_ip_config->current_client_cnt -1],
           0,
           sizeof(qmi_ip_client_ctrl_table_t));
    qmi_ip_config->current_client_cnt--;
  }

  LOG_MSG_INFO1("Current client count is %d", qmi_ip_config->current_client_cnt, 0, 0);

  return SUCCESS;
}

/*=============================================================================

FUNCTION QMI_IP_MAP_SOCKET()

DESCRIPTION
  This function maps the socket with associated read and write functions.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
static int qmi_ip_map_socket
(
  int               socket_fd,
  qmi_ip_read_data  read_func,
  qmi_ip_write_data write_func
)
{
  if (qmi_ip_config->socket_info.num_of_fds < MAX_FDS)
  {
    qmi_ip_config->socket_info.sk_conf[qmi_ip_config->socket_info.num_of_fds].socket_fd =
                                                                                 socket_fd;
    if (read_func != NULL)
    {
      qmi_ip_config->socket_info.sk_conf[qmi_ip_config->socket_info.num_of_fds].read_ex =
                                                                                    read_func;
    }
    if (write_func != NULL)
    {
      qmi_ip_config->socket_info.sk_conf[qmi_ip_config->socket_info.num_of_fds].write_ex =
                                                                                    write_func;
    }
    qmi_ip_config->socket_info.max_fd = MAX(socket_fd,qmi_ip_config->socket_info.max_fd);
    FD_SET(socket_fd, &(qmi_ip_config->socket_info.fdset));
    qmi_ip_config->socket_info.num_of_fds++;

    LOG_MSG_INFO1("Updated socket info for fd %d", socket_fd, 0, 0);
    LOG_MSG_INFO1("Number of sockets %d", qmi_ip_config->socket_info.num_of_fds, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Exceeded max fd count %d",
                  qmi_ip_config->socket_info.num_of_fds, 0, 0);
    return FAILURE;
  }

  return SUCCESS;
}
/*=============================================================================

FUNCTION QMI_IP_READ_PKTS()

DESCRIPTION
  This function read QMI IP packets from LAN clients.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/


int qmi_ip_read_pkts
(
  int fd
)
{
  char           buf[MAX_QMI_IP_PKT_SIZE];
  int            bytes;
  int            ret=0;
/*--------------------------------------------------------------------------*/
  bytes = read(fd, buf, sizeof(buf));
  LOG_MSG_INFO1("Read %d bytes", bytes, 0, 0);
  if (bytes > 0)
  {
/*---------------------------------------------------------------------------
  Process QMI packets
----------------------------------------------------------------------------*/
    ret = qmi_ip_process_ctrl_msg(fd, (void *)buf, bytes);
    if(ret == -1)
    {
       LOG_MSG_ERROR("Could not process QMI Packets further",0,0,0);
       return FAILURE;
    }
  }
  else if (bytes == 0)
  {
/*---------------------------------------------------------------------------
  Close connection for client
----------------------------------------------------------------------------*/
    qmi_ip_close_connection(fd);
  }
  else if (bytes < 0)
  {
    LOG_MSG_ERROR("Error in reading bytes %d", errno, 0, 0);
    return FAILURE;
  }

  return SUCCESS;

}


/*=============================================================================

FUNCTION QMI_IP_WRITE_PKTS()

DESCRIPTION
  This function sends QMI IP packets to LAN clients.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_write_pkts
(
  int   fd,
  char  *data,
  int   data_len
)
{
  int            bytes;

  LOG_MSG_INFO1("Will write %d bytes to socket fd %d", data_len, fd, 0);

  bytes = write(fd, data, data_len);
  if (bytes < 0)
  {
    LOG_MSG_ERROR("Error in writing to socket %d", errno, 0, 0);
    return FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Successfully wrote %d bytes", bytes, 0, 0);
  }

  return SUCCESS;

}

/*=============================================================================

FUNCTION QMI_IP_PROCESS_CONNECTION_REQ()

DESCRIPTION
  This function accepts TCP connections from LAN clients.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/

static int qmi_ip_process_connection_req
(
  int fd
)
{
  struct sockaddr_in  cli_addr;
  int                 clilen;
  int                 newsockfd;
  int                 val;
  int                 ret_val;

  clilen = sizeof(cli_addr);

/*---------------------------------------------------------------------------
  Accept TCP socket connections
----------------------------------------------------------------------------*/
  newsockfd = accept(fd,
                     (struct sockaddr *)&cli_addr,
                     &clilen);
  if (newsockfd < 0)
  {
    LOG_MSG_ERROR("Error on conn accept. err %d", errno, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Close socket if the number of clients exceeds the number that WLAN+USB
  can support. This is limited by the WLAN and USB limitation.
----------------------------------------------------------------------------*/
  if (qmi_ip_config->current_client_cnt == MAX_LAN_CLIENTS)
  {
    LOG_MSG_ERROR("MDM cant support more than %d clients",
                  qmi_ip_config->current_client_cnt,0,0);
    close(newsockfd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Close socket if max lan clients allowed are already connected. This number
  is user configured.
----------------------------------------------------------------------------*/
  if (qmi_ip_config->current_client_cnt == qmi_ip_config->lan_clients_allowed)
  {
    LOG_MSG_ERROR("Can't have more than %d clients as per user configuration",
                  qmi_ip_config->lan_clients_allowed,0,0);
    close(newsockfd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Set the socket as non blocking
----------------------------------------------------------------------------*/
  val = fcntl(newsockfd, F_GETFL, 0);
  fcntl(newsockfd, F_SETFL, val | O_NONBLOCK);

/*---------------------------------------------------------------------------
  Associate read and write functions for socket
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_map_socket(newsockfd, qmi_ip_read_pkts, qmi_ip_write_pkts);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to map socket with read/write functions", 0, 0, 0);
    close(newsockfd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Add client to the client list
----------------------------------------------------------------------------*/

  qmi_ip_config->embms_client[qmi_ip_config->current_client_cnt].socket_fd =
                                                                        newsockfd;
  qmi_ip_config->current_client_cnt++;

  LOG_MSG_INFO1("Updated client info for fd %d", newsockfd, 0, 0);
  LOG_MSG_INFO1("Current client count %d", qmi_ip_config->current_client_cnt, 0, 0);

  return SUCCESS;

}

/*=============================================================================

FUNCTION QMI_IP_INIT_MASTER_CONN_SERVER()

DESCRIPTION
  This function initializes the master connection server which listens to the
  connections from LAN clients.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_init_master_conn_server
(
  qmi_ip_conf_t *qmi_ip_cfg
)
{
  int                 socket_fd;
  struct ifreq        ifr;
  struct sockaddr_in  serv_addr;
  struct sockaddr_in *bridge_addr;
  int                 reuse = 1;
  int                 ret_val;
  int                 val;

/*--------------------------------------------------------------------------*/

  qmi_ip_config = qmi_ip_cfg;

/*---------------------------------------------------------------------------
  Open master server socket
----------------------------------------------------------------------------*/
  socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd < 0)
  {
    LOG_MSG_ERROR("Failed to create master conn socket. err %d", errno, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Bind socket to bridge iface
----------------------------------------------------------------------------*/
  memset(&ifr, 0, sizeof(ifr));
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), BRIDGE_IFACE);

  val = ioctl(socket_fd, SIOCGIFADDR, &ifr);
  bridge_addr = (struct sockaddr_in *)&(ifr.ifr_addr);

  LOG_MSG_INFO1("Bridge Address : ",0,0,0);
  IPV4_ADDR_MSG((bridge_addr->sin_addr).s_addr);

/*---------------------------------------------------------------------------
  Make it a reuseable socket
----------------------------------------------------------------------------*/
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,(char*)&reuse, sizeof(int)) == -1)
  {
    LOG_MSG_ERROR("Failed to set socket as reuseable. err %d", errno, 0, 0);
    close (socket_fd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Make it a non blocking socket
----------------------------------------------------------------------------*/
  val = fcntl(socket_fd, F_GETFL, 0);
  fcntl(socket_fd, F_SETFL, val | O_NONBLOCK);

/*---------------------------------------------------------------------------
  Bind the socket to server port
----------------------------------------------------------------------------*/
  memset(&serv_addr, 0x00, sizeof(struct sockaddr_in));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = qmi_ip_config->tcp_server_ctrl_port;
  serv_addr.sin_addr.s_addr = (bridge_addr->sin_addr).s_addr;
  ret_val = bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret_val < 0)
  {
    LOG_MSG_ERROR("Failed to bind socket for master conn. err %d", errno, 0, 0);
    close (socket_fd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Start listening to client connections.
----------------------------------------------------------------------------*/
  ret_val = listen(socket_fd, 3);
  if (ret_val < 0)
  {
    LOG_MSG_ERROR("Failed to create a listening socket for master conn. err %d",
                  errno, 0, 0);
    close (socket_fd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Add associated read function.
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_map_socket(socket_fd, qmi_ip_process_connection_req, NULL);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to map socket with read/write functions", 0, 0, 0);
    close(socket_fd);
    return FAILURE;
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_IND_RECV()

DESCRIPTION

  This function receives QMI indications

DEPENDENCIES
  None.

RETURN VALUE
  none


SIDE EFFECTS
  None

/*=========================================================================*/
void qmi_ip_ind_recv
(
  int fd
)
{
  int   ret_val;
  int   bytes;
  char  buf[MAX_QMI_IP_PKT_SIZE];

  bytes = read(fd, buf, sizeof(buf));

  LOG_MSG_INFO1("Read %d bytes from indication socket", bytes, 0, 0);
  if (bytes < 0)
  {
    LOG_MSG_ERROR("Error in reading indication message %d", errno, 0, 0);
  }
  else
  {
/*---------------------------------------------------------------------------
  Process QMI indications
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_process_ind(buf, bytes);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Error in processing indication", 0, 0, 0);
    }
  }
}


/*===========================================================================

FUNCTION QMI_IP_IND_HDLR_INIT()

DESCRIPTION

  This function initializes QMI indication socket

DEPENDENCIES
  None.

RETURN VALUE
  none


SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_ind_hdlr_init()
{
  int                 sockfd;
  int                 val;
  struct sockaddr_un  sock_addr;
  int                 len;
  int                 ret_val;

/*---------------------------------------------------------------------------
  Open unix domain socket
----------------------------------------------------------------------------*/
  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Error creating ind hdlr socket, errno: %d", errno, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Bind it to a file path
----------------------------------------------------------------------------*/
  sock_addr.sun_family = AF_UNIX;
  strlcpy(sock_addr.sun_path, QMI_IP_IND_HDLR_FILE,sizeof(QMI_IP_IND_HDLR_FILE));
  unlink(sock_addr.sun_path);
  len = strlen(sock_addr.sun_path) + sizeof(sock_addr.sun_family);
  if (bind(sockfd, (struct sockaddr *)&sock_addr, len) == -1)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    close (sockfd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Make the socket non blocking
----------------------------------------------------------------------------*/
  val = fcntl(sockfd, F_GETFL, 0);
  fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

/*---------------------------------------------------------------------------
  Add associated read and write functions
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_map_socket(sockfd, qmi_ip_ind_recv, NULL);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to map socket with read/write functions", 0, 0, 0);
    close(sockfd);
    return FAILURE;
  }

  return SUCCESS;

}

/*===========================================================================

FUNCTION QMI_IP_FPOP_MSG_RECV()

DESCRIPTION

  This function receives FPOP message from FPOP thread

DEPENDENCIES
  None.

RETURN VALUE
  none


SIDE EFFECTS
  None

/*=========================================================================*/
void qmi_ip_fpop_msg_recv
(
  int fd
)
{
  int   ret_val;
  int   bytes;
  char  buf[MAX_QMI_IP_PKT_SIZE];

  bytes = read(fd, buf, sizeof(buf));

  if (bytes < 0)
  {
    LOG_MSG_INFO1("Read %d bytes from FPOP socket", bytes, 0, 0);
    LOG_MSG_ERROR("Error in reading FPOP message %d", errno, 0, 0);
  }
  else
  {
/*---------------------------------------------------------------------------
  Process FPOP message
----------------------------------------------------------------------------*/
    ret_val = qmi_ip_fpop_message_hdlr(&buf, bytes);
    if (ret_val != SUCCESS)
    {
      LOG_MSG_ERROR("Error in processing FPOP message", 0, 0, 0);
    }
  }
}

/*===========================================================================

FUNCTION QMI_IP_FPOP_SOCKET_INIT()

DESCRIPTION

  This function initializes FPOP socket

DEPENDENCIES
  None.

RETURN VALUE
  0 for SUCCESS
  -1 for FAILURE


SIDE EFFECTS
  None

/*=========================================================================*/

int qmi_ip_fpop_socket_init()
{
  int                 sockfd;
  int                 val;
  struct sockaddr_un  sock_addr;
  int                 len;
  int                 ret_val;

/*---------------------------------------------------------------------------
  Open unix domain socket
----------------------------------------------------------------------------*/
  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Error creating fpop socket, errno: %d", errno, 0, 0);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Bind it to a file path
----------------------------------------------------------------------------*/
  sock_addr.sun_family = AF_UNIX;
  strlcpy(sock_addr.sun_path, QMI_IP_FPOP_FILE,sizeof(QMI_IP_FPOP_FILE));
  unlink(sock_addr.sun_path);
  len = strlen(sock_addr.sun_path) + sizeof(sock_addr.sun_family);
  if (bind(sockfd, (struct sockaddr *)&sock_addr, len) == -1)
  {
    LOG_MSG_ERROR("Error binding the fpop socket, errno: %d", errno, 0, 0);
    close (sockfd);
    return FAILURE;
  }

/*---------------------------------------------------------------------------
  Make the socket non blocking
----------------------------------------------------------------------------*/
  val = fcntl(sockfd, F_GETFL, 0);
  fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

/*---------------------------------------------------------------------------
  Add associated read and write functions
----------------------------------------------------------------------------*/
  ret_val = qmi_ip_map_socket(sockfd, qmi_ip_fpop_msg_recv, NULL);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Failed to map socket with read/write functions", 0, 0, 0);
    close(sockfd);
    return FAILURE;
  }

  return SUCCESS;

}
