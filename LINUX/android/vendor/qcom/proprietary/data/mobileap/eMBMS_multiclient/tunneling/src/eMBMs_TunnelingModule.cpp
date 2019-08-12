/*====================================================

FILE:  eMBMs_TunnelingModule.cpp

SERVICES:
   eMBMs Tunneling Module Implementation

=====================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  04/25/14   pm         Created Module.
===========================================================================*/
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <net/ethernet.h> /* the L2 protocols */

//files for multi client filter
#include <sys/time.h>
#include <linux/filter.h>

#include "ds_string.h"
#include "ds_util.h"
#include "eMBMs_TunnelingModule.h"
#include "embms_tm_api.h"
using namespace std;
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

#define EMBMS_TM_LOG(...)                         \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define EMBMS_TM_LOG_FUNC_ENTRY()  \
    EMBMS_TM_LOG                   \
    (                              \
        "Entering function %s\n",  \
        __FUNCTION__               \
    )

#define EMBMS_TM_LOG_FUNC_EXIT()   \
    EMBMS_TM_LOG                   \
    (                              \
        "Exiting function %s\n",   \
        __FUNCTION__               \
    )

/*This module maintains a local TMGI to client mapping table.
This table will be used by the module to forward the data
received for a particular TMGI to the clients who have
registerred for that particular data stream. This table
will be modified whenever any client is registers/unregisters
for any TMGI, or the client goes down completely etc.
Changes to mapping table will be made only when an
update message is received from the qmi_ip module*/

fd_set rset;
int max_fd = 0;
embms_tmgi_to_client_info_internal *tmgi_to_client_mapping_table = NULL;

/*Server socket*/
unsigned int embms_control_sockfd = 0;
unsigned int embms_data_sockfd = SOCKET_INACTIVE;

/*Client socket*/
unsigned int control_embms_sockfd = 0;
unsigned int embms_send_sockfd = 0;

/*==========================================================================

FUNCTION create_udp_socket()

DESCRIPTION

  create a normal unix domain socket

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int create_udp_socket(unsigned int *sockfd)
{

  if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == EMBMS_TM_ERROR)
  {
    LOG_MSG_ERROR("Error creating socket, errno: %d", errno, 0, 0);
    return EMBMS_TM_ERROR;
  }

  if(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
  {
    LOG_MSG_ERROR("Couldn't set Close on Exec, errno: %d", errno, 0, 0);
  }

  return EMBMS_TM_SUCCESS;
}

/*==========================================================================

FUNCTION create_embms_control_socket()

DESCRIPTION

  Create a UNIX domain socket which is used
  to communicate with QMI_IP module

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int create_embms_control_socket()
{
  int val, rval;
  int len;
  struct sockaddr_un embms_control;
  struct timeval rcv_timeo;

  rval = create_udp_socket(&embms_control_sockfd);

  if (rval == EMBMS_TM_ERROR)
  {
    LOG_MSG_ERROR("Could not create udp socket",0,0,0);
    return EMBMS_TM_ERROR;
  }

  /*----------------------------------------------------
  Setting timeout for UDP socket
  -------------------------------------------------------*/
  rcv_timeo.tv_sec = SOCKET_RCV_TIMEOUT_SEC;
  rcv_timeo.tv_usec = SOCKET_RCV_TIMEOUT_USEC;
  setsockopt(embms_control_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(embms_control_sockfd, F_GETFL, 0);
  fcntl(embms_control_sockfd, F_SETFL, val | O_NONBLOCK);

  /*------------------------------------------------------
  binding socket to a file on device where data will be written for reading
  ---------------------------------------------------------*/
  embms_control.sun_family = AF_UNIX;
  strlcpy(embms_control.sun_path, EMBMS_TM_CONTROL_FILE, sizeof(embms_control.sun_path));
  unlink(embms_control.sun_path);
  len = strlen(embms_control.sun_path) + sizeof(embms_control.sun_family);
  if (bind(embms_control_sockfd, (struct sockaddr *)&embms_control, len) == EMBMS_TM_ERROR)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    close(embms_control_sockfd);
    return EMBMS_TM_ERROR;
  }

  /*--------------------------------------------------------
  set bit for select call
  ----------------------------------------------------------*/
  FD_SET(embms_control_sockfd, &rset);
  max_fd = MAX(max_fd,embms_control_sockfd);

  return EMBMS_TM_SUCCESS;
}


/*==========================================================================

FUNCTION create_embms_data_socket()

DESCRIPTION

  Create a packet socket on which tunneling module
  listens for eMBMS data

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int create_embms_data_socket(char *iface_name)
{
  int val,len;
  int if_index = 0;
  struct sockaddr_ll bindaddr;
  struct ifreq req_if_info;
  struct timeval rcv_timeo;

  LOG_MSG_INFO1("create_embms_data_sockfd : Enter",0,0,0);

  /*---------------------------------------------------------------
  Create Raw socket for receiving data on rmnet interface
  ------------------------------------------------------------------*/
  embms_data_sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
  if(embms_data_sockfd < 0 || embms_data_sockfd >= FD_SETSIZE)
  {
    LOG_MSG_ERROR( "Open embms data sock error = %0x\n", embms_data_sockfd, 0, 0);
    embms_data_sockfd = SOCKET_INACTIVE;
    return EMBMS_TM_ERROR;
  }

  LOG_MSG_INFO1( "embms_data_sockfd = 0x%0x\n", embms_data_sockfd, 0, 0);

  /*----------------------------------------------------------------
  Set timeout value for socket
  ------------------------------------------------------------------*/
  rcv_timeo.tv_sec = SOCKET_RCV_TIMEOUT_SEC;
  rcv_timeo.tv_usec = SOCKET_RCV_TIMEOUT_USEC;

  setsockopt(embms_data_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(embms_data_sockfd, F_GETFL, 0);
  fcntl(embms_data_sockfd, F_SETFL, val | O_NONBLOCK);

  /*----------------------------------------------------------------
  Bind to interface name provided by qmi_ip
  -------------------------------------------------------------------*/
  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, iface_name, sizeof(req_if_info.ifr_name));

  val = ioctl(embms_data_sockfd, SIOCGIFINDEX, &req_if_info);
  if_index = req_if_info.ifr_ifindex;

  if (val < 0)
  {
    LOG_MSG_ERROR("Could not get iface index, error : %d",val,0,0);
    close(embms_data_sockfd);
    embms_data_sockfd = SOCKET_INACTIVE;
    return EMBMS_TM_ERROR;
  }

  LOG_MSG_INFO1( "get rmnet ifIndex=%d, return %d.\n", if_index, val, 0);

  /*----------------------------------------------------
  Configure socket to receive multicast traffic only
  -------------------------------------------------------*/
  memset(&bindaddr, 0, sizeof(bindaddr));
  bindaddr.sll_family = AF_PACKET;
  bindaddr.sll_protocol = htons(ETH_P_IP);
  bindaddr.sll_ifindex = if_index;
  bindaddr.sll_pkttype = PACKET_MULTICAST;

  val = bind(embms_data_sockfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
  if(val < 0)
  {
    LOG_MSG_ERROR( "bind RawSocket on rmnet, return %d.\n", val, 0, 0);
    close(embms_data_sockfd);
    embms_data_sockfd = SOCKET_INACTIVE;
    return EMBMS_TM_ERROR;
  }
  else
  {
    LOG_MSG_INFO1( "bind RawSocket on rmnet, return %d.\n", val, 0, 0);
  }

  /*--------------------------------------------------------
  Add raw socket to list of sockets for select
  -----------------------------------------------------------*/
  FD_SET(embms_data_sockfd, &rset);
  max_fd = MAX(max_fd,embms_data_sockfd);

  return EMBMS_TM_SUCCESS;
}

/*==========================================================================

FUNCTION create_embms_send_socket()

DESCRIPTION

  Create a UDP socket used to send eMBMS data
  to clients from tunneling module.

DEPENDENCIES
  None.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int create_embms_send_socket(uint16 data_port)
{
  int val, n_read, n_write, reuse;
  char buf[READ_BUFFER_SIZE];
  uint32 send_sock_ip;
  struct ifreq req_if_info;
  struct sockaddr_in bindaddr, ClientAddr;
  struct sockaddr *send_sock_addr;

  /*-----------------------------------------------------
  Creating simple UDP socket for sending data to clients
  -------------------------------------------------------*/
  embms_send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(embms_send_sockfd < 0)
  {
    LOG_MSG_ERROR("Open send socket error = %0x\n", embms_send_sockfd, 0, 0);
    return EMBMS_TM_ERROR;
  }

  /*----------------------------------------------------
  Bind to bridge interface
  -------------------------------------------------------*/
  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, EMBMS_TM_BRIDGE_INTERFACE, sizeof(req_if_info.ifr_name));

  val = ioctl(embms_send_sockfd, SIOCGIFADDR, &req_if_info);
  if (val < 0)
  {
    LOG_MSG_ERROR("Error in getting if addr, errno %d",errno,0,0);
    close(embms_send_sockfd);
    return EMBMS_TM_ERROR;
  }
  send_sock_addr = (struct sockaddr *)&(req_if_info.ifr_addr);
  send_sock_ip = ((struct sockaddr_in *)send_sock_addr)->sin_addr.s_addr;
  LOG_MSG_INFO1( "get bridge0 ipaddr=0x%08x, return %d.\n", send_sock_ip, val, 0);

  reuse = 1;
  val = setsockopt(embms_send_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int));
  if(val < 0)
  {
    LOG_MSG_ERROR("set socket opt REUSEPORT failed ",0,0,0);
    close(embms_send_sockfd);
    return EMBMS_TM_ERROR;
  }


  memset(&bindaddr, 0, sizeof(bindaddr));
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = send_sock_ip;
  bindaddr.sin_port = data_port;

  val = bind(embms_send_sockfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
  if(val < 0)
  {
    LOG_MSG_ERROR( "bind SendSocket on bridge0, return %d.\n", val, 0, 0);
    close(embms_send_sockfd);
    return EMBMS_TM_ERROR;
  }
  else
  {
    LOG_MSG_INFO1( "bind SendSocket on bridge0, return %d.\n", val, 0, 0);
  }

  return EMBMS_TM_SUCCESS;
}

/*==========================================================================

FUNCTION delete_tmgi_entry_from_table()

DESCRIPTION

  Delete the corressponding TMGI entry from the local
  table maintained by tunnelling module

DEPENDENCIES
  Called when tunnelling module receives Delete TMGI
  message from QMI_IP.

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int delete_tmgi_entry_from_table(embms_tmgi_to_client_info_update *info_update)
{
  int rval = 0;
  embms_tmgi_to_client_info_internal *temp_tmgi = NULL;
  embms_tmgi_to_client_info_internal *prev_tmgi = NULL;
  embms_client_info_internal *temp_client = NULL;
  embms_client_info_internal *prev_client = NULL;

  /*-------------------------------------------------------
  This function is used to delete a specific TMGI entry
  when that particular TMGI goes down
  ----------------------------------------------------------*/

  temp_tmgi = tmgi_to_client_mapping_table;
  prev_tmgi = tmgi_to_client_mapping_table;

  if (info_update == NULL)
  {
    LOG_MSG_ERROR("NULL Arguments passed",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }

  /*--------------------------------------------------------
  Search for the TMGI entry in our local table
  -----------------------------------------------------------*/
  while (temp_tmgi != NULL)
  {
    if ((temp_tmgi->tmgi_multicast_addr == info_update->multicast_addr) &&
        (temp_tmgi->tmgi_port == info_update->multicast_port))
    {
      LOG_MSG_INFO1("TMGI Entry found.",0,0,0);
      break;
    }

    prev_tmgi = temp_tmgi;
    temp_tmgi = temp_tmgi->next_tmgi;
  }

  if (temp_tmgi == NULL)
  {
    /*--------------------------------------------------------
    TMGI entry was not found in our local table
    -----------------------------------------------------------*/
    LOG_MSG_ERROR("Desired TMGI entry not found",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }

  if (temp_tmgi == tmgi_to_client_mapping_table)
  {
    /*---------------------------------------------------------
    If entry to be deleted is head of mapping table
    we need to change head pointer to next tmgi entry
    ------------------------------------------------------------*/
    LOG_MSG_INFO1("Deleted tmgi was head of mapping table",0,0,0);
    tmgi_to_client_mapping_table = tmgi_to_client_mapping_table->next_tmgi;
  }
  else
  {
    prev_tmgi->next_tmgi = temp_tmgi->next_tmgi;
  }

  /*-----------------------------------------------------------
  Freeing allocated memory
  --------------------------------------------------------------*/
  temp_client = temp_tmgi->client_info_head;
  prev_client = temp_tmgi->client_info_head;

  /*-----------------------------------------------------------
  We need to free memory allocated to client entries
  for a particular TMGI entry
  --------------------------------------------------------------*/

  while (temp_client != NULL)
  {
    prev_client = temp_client;
    temp_client = temp_client->next_client;

    prev_client->next_client = NULL;
    free(prev_client);
    prev_client = NULL;
  }

  /*-------------------------------------------------------------
  Free memory allocated to tmgi entry
  ---------------------------------------------------------------*/

  temp_tmgi->next_tmgi = NULL;
  free(temp_tmgi);
  temp_tmgi = NULL;

  LOG_MSG_INFO1("TMGI Entry deleted.",0,0,0);

  /*-------------------------------------------------------------
  close data socket if no more tmgi entries in table
  ----------------------------------------------------------------*/
  if (tmgi_to_client_mapping_table == NULL)
  {
    LOG_MSG_INFO1("Closing data socket since no more tmgi",0,0,0);
    FD_CLR(embms_data_sockfd, &rset);
    close(embms_data_sockfd);
    embms_data_sockfd = SOCKET_INACTIVE;
  }

  return EMBMS_TM_SUCCESS;
}

/*==========================================================================

FUNCTION delete_client_entry_from_all_tmgi()

DESCRIPTION

  Delete the corressponding client entry from all the
  TMGI lists to which the client had subscribed

DEPENDENCIES
  Called when we receive Delete Client from all
  tmgi message from QMI_IP

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int delete_client_entry_from_all_tmgi(embms_tmgi_to_client_info_update *info_update)
{
  embms_tmgi_to_client_info_internal *next_tmgi = NULL;
  embms_tmgi_to_client_info_internal *temp_tmgi = NULL;
  embms_client_info_internal *prev_client = NULL;
  embms_client_info_internal *temp_client = NULL;
  int rval = 0;
  ofstream fout;

  /*----------------------------------------------------
  We use this function when we want to delete any client entry
  from all TMGI entries. This scenario happens when any client
  disconnects and hence we need to clean all realted client enmtries
  in our mapping table
  --------------------------------------------------------*/

  if (info_update == NULL)
  {
    LOG_MSG_ERROR("NULL Arguments Passed",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }

  /*--------------------------------------------------------
  We start checking from first TMGI entry and if client entry is found
  in client entries of any TMGI, we clean up that client entry from that
  TMGI entry
  ----------------------------------------------------------*/

  temp_tmgi = tmgi_to_client_mapping_table;

  while(temp_tmgi != NULL)
  {
    next_tmgi = temp_tmgi->next_tmgi;

    temp_client = temp_tmgi->client_info_head;
    prev_client = temp_tmgi->client_info_head;

    while(temp_client != NULL)
    {
      if ((temp_client->addr == info_update->client_addr) &&
          (temp_client->port == info_update->client_port))
      {
        if(temp_client == temp_tmgi->client_info_head)
        {
          /*------------------------------------------------------
          Client to be deleted is head of client list for that TMGI
          hence we need to re-assign the head pointer to next client
          ------------------------------------------------------------*/
          LOG_MSG_INFO1("client to be deleted is the head of client list",0,0,0);
          temp_tmgi->client_info_head = temp_client->next_client;
        }
        else
        {
          prev_client->next_client = temp_client->next_client;
        }

        temp_client->next_client = NULL;

#ifdef FEATURE_DEMO_LOGGING
  /*--------------------------------------------------------
  Open output log file and log this instance
  ----------------------------------------------------------*/
        fout.open("/usr/bin/out_logs.txt", ios::out | ios::app);
        if (fout.is_open())
        {
          fout << "packets tunneled: " << temp_client->packets_sent << "\n";
          fout.close();
          LOG_MSG_INFO1("Opened output log file successfully",0,0,0);
        }
        else
        {
          LOG_MSG_ERROR("Error opening output log file",0,0,0);
        }
#endif

        /*------------------------------------------------
        Free memory for the client
        ----------------------------------------------------*/
        free(temp_client);
        temp_client = NULL;

        LOG_MSG_INFO1("client entry deleted from tmgi. TMGI port = %d",temp_tmgi->tmgi_port,0,0);
        IPV4_ADDR_MSG(temp_tmgi->tmgi_multicast_addr);

        break;
      }

      prev_client = temp_client;
      temp_client = temp_client->next_client;
    }

    if (temp_tmgi->client_info_head == NULL)
    {
      /*------------------------------------------------------
      If the deleted client entry was the last client for that TMGI
      we go ahead and delete the TMGI entry from the mapping table
      because qmi_ip would have deactivated this TMGI in this case
      For this we call our internal API to delete TMGI entry
      Since there are no clients, we need not keep this entry in our table
      --------------------------------------------------------*/
      info_update->action_type = EMBMS_TM_TMGI_DEACTIVATE;
      info_update->multicast_addr = temp_tmgi->tmgi_multicast_addr;
      info_update->multicast_port = temp_tmgi->tmgi_port;

      rval = delete_tmgi_entry_from_table(info_update);

    }

    temp_tmgi = next_tmgi;
  }

  LOG_MSG_INFO1("Successfully deleted client entry from all tmgi entries",0,0,0);
  return EMBMS_TM_SUCCESS;
}

/*==========================================================================

FUNCTION add_client_entry_to_table()

DESCRIPTION

  This function creates a TMGI entry if not otherwise present
  and adds the respective clients to it.
  If TMGI entry is already present, it just adds the client entry

DEPENDENCIES
  Called when we receive add client entry
  message from QMI_IP

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int add_client_entry_to_table(embms_tmgi_to_client_info_update *info_update)
{
  int ret = 0;
  embms_client_info_internal *new_client = NULL;
  embms_client_info_internal *temp_client = NULL;
  embms_client_info_internal *next_client = NULL;
  embms_tmgi_to_client_info_internal *new_tmgi = NULL;
  embms_tmgi_to_client_info_internal *temp_tmgi = NULL;
  embms_tmgi_to_client_info_internal *next_tmgi = NULL;

  LOG_MSG_INFO1("add_entry_to_table : Enter",0,0,0);

  if (info_update == NULL)
  {
    LOG_MSG_ERROR("NULL arguments passed",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }

  new_client = (embms_client_info_internal *)malloc(
                sizeof(embms_client_info_internal));
  if (new_client == NULL)
  {
    LOG_MSG_ERROR("Cannot allocate memory",0,0,0);
    return EMBMS_TM_ENOMEM;
  }

  new_client->addr = info_update->client_addr;
  LOG_MSG_INFO1("New client addr ",0,0,0);
  IPV4_ADDR_MSG(new_client->addr);
  new_client->port = info_update->client_port;
  LOG_MSG_INFO1("New client port : %d",info_update->client_port,0,0);
  new_client->next_client = NULL;

  if (tmgi_to_client_mapping_table == NULL)
  {
    /*---------------------------------------------------------------
    TMGI Client mapping table is empty. First client entry is being added
    ----------------------------------------------------------------*/

    LOG_MSG_INFO1("tmgi_to_client_mapping_table head is NULL",0,0,0);
    new_tmgi = (embms_tmgi_to_client_info_internal *)malloc(
                sizeof(embms_tmgi_to_client_info_internal));
    if (new_tmgi == NULL)
    {
      LOG_MSG_ERROR("Cannot allocate memory",0,0,0);
      free(new_client);
      new_client = NULL;
      return EMBMS_TM_ENOMEM;
    }
    LOG_MSG_INFO1("New multicast addr ",0,0,0);
    IPV4_ADDR_MSG(info_update->multicast_addr);
    LOG_MSG_INFO1("New multicast port : %d",info_update->multicast_port,0,0);

    new_tmgi->tmgi_multicast_addr = info_update->multicast_addr;
    new_tmgi->tmgi_port = info_update->multicast_port;
    new_tmgi->next_tmgi = NULL;
    new_tmgi->client_info_head = new_client;

    /*------------------------------------------------------------
    Create embms data socket if not already present.
    Since this is the first client entry data socket will be created.
    Data socket is created only when data is expected to arrive.
    This socket is not present during boot up
    -------------------------------------------------------------*/

    if (embms_data_sockfd == SOCKET_INACTIVE)
    {
      ret = create_embms_data_socket(info_update->iface_name);
      if (ret == EMBMS_TM_ERROR)
      {
        LOG_MSG_ERROR("Failed to create data socket",0,0,0);
        free(new_client);
        new_client = NULL;
        free(new_tmgi);
        new_tmgi = NULL;
        return EMBMS_TM_ERROR;
      }
    }

    /*------------------------------------------------------
    Once above steps are done successfully,
    we add tmgi entry to our local table
    --------------------------------------------------------*/

    tmgi_to_client_mapping_table = new_tmgi;
    return EMBMS_TM_SUCCESS;
  }

  else
  {
    /*-----------------------------------------------------
    In this case, table already has some entries
    and we need to search for the specific tmgi entry
    for which client entry is to be added
    --------------------------------------------------------*/

    temp_tmgi = tmgi_to_client_mapping_table;
    next_tmgi = tmgi_to_client_mapping_table;
    while (next_tmgi != NULL)
    {
      temp_tmgi = next_tmgi;

      if ((temp_tmgi->tmgi_multicast_addr == info_update->multicast_addr) &&
          (temp_tmgi->tmgi_port == info_update->multicast_port))
      {
        LOG_MSG_INFO1("Found matching tmgi entry. Adding client entry",0,0,0);
        LOG_MSG_INFO1("Multicast port of tmgi = %d , multicast addr = ",temp_tmgi->tmgi_port,0,0);
        IPV4_ADDR_MSG(temp_tmgi->tmgi_multicast_addr);
        if (temp_tmgi->client_info_head == NULL)
        {
          /*--------------------------------------------------------
          If there are no existing clients for the specified TMGI
          This case is not expected as we will be deleting a TMGI entry
          once it has no clients attached to it. But, keeping this check
          as a safeguard
          ----------------------------------------------------------*/

          temp_tmgi->client_info_head = new_client;
          LOG_MSG_INFO1("client entry added successfully.",0,0,0);
          return EMBMS_TM_SUCCESS;
        }
        else
        {
          temp_client = temp_tmgi->client_info_head;
          next_client = temp_tmgi->client_info_head;
          while (next_client != NULL)
          {
            if ((temp_client->addr == info_update->client_addr) &&
                (temp_client->port == info_update->client_port))
            {
              LOG_MSG_INFO1("Client Entry already present.",0,0,0);
              return EMBMS_TM_ENOEFFECT;
            }
            temp_client = next_client;
            next_client = temp_client->next_client;
          }

          /*-----------------------------------------------------------
          Adding client to the client list for the specified TMGI
          -------------------------------------------------------------*/
          temp_client->next_client = new_client;
          LOG_MSG_INFO1("client entry added successfully.",0,0,0);
          return EMBMS_TM_SUCCESS;
        }
      }

      next_tmgi = temp_tmgi->next_tmgi;
    }

    if (next_tmgi == NULL)
    {
      /*-----------------------------------------------------------
      TMGI specified in the message was not found in mapping table.
      Hence, we need to add a new entry for this TMGI and add the
      specified client to the client list
      -------------------------------------------------------------*/

      new_tmgi = (embms_tmgi_to_client_info_internal *)malloc(
                  sizeof(embms_tmgi_to_client_info_internal));
      if (new_tmgi == NULL)
      {
        LOG_MSG_ERROR("Cannot allocate memory",0,0,0);
        free(new_client);
        new_client = NULL;
        return EMBMS_TM_ENOMEM;
      }

      new_tmgi->tmgi_multicast_addr = info_update->multicast_addr;
      new_tmgi->tmgi_port = info_update->multicast_port;
      new_tmgi->next_tmgi = NULL;
      new_tmgi->client_info_head = new_client;

      temp_tmgi->next_tmgi = new_tmgi;
      LOG_MSG_INFO1("New multicast addr ",0,0,0);
      IPV4_ADDR_MSG(info_update->multicast_addr);
      LOG_MSG_INFO1("New multicast port : %d",info_update->multicast_port,0,0);
      LOG_MSG_INFO1("TMGI entry added successfully",0,0,0);
    }
  }
  return EMBMS_TM_SUCCESS;

}

/*==========================================================================

FUNCTION delete_client_entry_from_table()

DESCRIPTION

  This function deletes a TMGI entry or a client entry
  depending upon the information received from
  QMI over IP module

DEPENDENCIES
  Called when we receive delete client entry
  message from QMI_IP

RETURN VALUE
  EMBMS_TM_SUCCESS on success
  EMBMS_TM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
int delete_client_entry_from_table(embms_tmgi_to_client_info_update *info_update)
{
  embms_client_info_internal *temp_client = NULL;
  embms_client_info_internal *prev_client = NULL;
  embms_tmgi_to_client_info_internal *temp_tmgi = NULL;
  embms_tmgi_to_client_info_internal *prev_tmgi = NULL;
  int rval = 0;
  ofstream fout;

  temp_tmgi = tmgi_to_client_mapping_table;
  prev_tmgi = tmgi_to_client_mapping_table;

  if (info_update == NULL)
  {
    LOG_MSG_ERROR("NULL Arguments passed",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }

  /*-----------------------------------------------------------------
  Search for the TMGI entry
  -------------------------------------------------------------------*/
  while (temp_tmgi != NULL)
  {
    if ((temp_tmgi->tmgi_multicast_addr == info_update->multicast_addr) &&
        (temp_tmgi->tmgi_port == info_update->multicast_port))
    {
      LOG_MSG_INFO1("TMGI Entry found.",0,0,0);
      break;
    }

    prev_tmgi = temp_tmgi;
    temp_tmgi = temp_tmgi->next_tmgi;
  }

  if (temp_tmgi == NULL)
  {
    LOG_MSG_ERROR("Desired TMGI entry not found",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }
  /*-----------------------------------------------------------------
  Delete client entry for a specific tmgi.
  -------------------------------------------------------------------*/
  LOG_MSG_INFO1("Client to be deleted port : %d , addr = ", info_update->client_port,0,0);
  IPV4_ADDR_MSG(info_update->client_addr);

  prev_client = temp_tmgi->client_info_head;
  temp_client = temp_tmgi->client_info_head;
  while (temp_client != NULL)
  {
    if ((temp_client->addr == info_update->client_addr) &&
        (temp_client->port == info_update->client_port))
    {
      LOG_MSG_INFO1("Client Entry found.",0,0,0);
      break;
    }

    prev_client = temp_client;
    temp_client = temp_client->next_client;
  }

  if (temp_client == NULL)
  {
    /*---------------------------------------------------------------
    Specified client entry was not found in client list of  specified TMGI
    -----------------------------------------------------------------*/
    LOG_MSG_ERROR("Client Entry not found.",0,0,0);
    return EMBMS_TM_EBADPARAM;
  }

  if (temp_client == temp_tmgi->client_info_head)
  {
    /*---------------------------------------------------------------
    If client to be deleted is head of client list for that TMGI
    we need to change the head of client list to the next client entry
    -----------------------------------------------------------------*/
    LOG_MSG_INFO1("client to be deleted is client_info_head of tmgi",0,0,0);
    temp_tmgi->client_info_head = temp_client->next_client;
  }
  else
  {
    prev_client->next_client = temp_client->next_client;
  }

  temp_client->next_client = NULL;

#ifdef FEATURE_DEMO_LOGGING
  /*-----------------------------------------------------------------
  Open output log file
  -------------------------------------------------------------------*/
  fout.open("/usr/bin/out_logs.txt", ios::out | ios::app);
  if (fout.is_open())
  {
    fout << "packets tunneled: " << temp_client->packets_sent << "\n";
    fout.close();
    LOG_MSG_INFO1("Opened output log file successfully",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Error opening output log file",0,0,0);
  }
#endif

  free(temp_client);
  temp_client = NULL;

  if (temp_tmgi->client_info_head == NULL)
  {
    /*---------------------------------------------------------------
    If deleted client was the only client for that TMGI
    we need to delete TMGI entry from table
    -----------------------------------------------------------------*/
    LOG_MSG_INFO1("Deleted client was the last client for tmgi",0,0,0);
    LOG_MSG_INFO1("Deleting tmgi since it has zero clients",0,0,0);

    info_update->action_type = EMBMS_TM_TMGI_DEACTIVATE;
    rval = delete_tmgi_entry_from_table(info_update);

    if (rval != EMBMS_TM_SUCCESS)
    {
      LOG_MSG_ERROR("Could not delete tmgi entry from table",rval,0,0);
    }
    else
    {
      LOG_MSG_INFO1("Successfully deleted tmgi entry from table",0,0,0);
    }
  }

  LOG_MSG_INFO1("Client Entry deleted.",0,0,0);
  return EMBMS_TM_SUCCESS;
}

void sighandler(int signal)
{
  embms_tmgi_to_client_info_internal *temp_tmgi = NULL;
  embms_tmgi_to_client_info_internal *next_tmgi = NULL;
  embms_client_info_internal *temp_client = NULL;
  embms_client_info_internal *next_client = NULL;

  /*-----------------------------------------------------------------
  Once we receive a SIGTERM or SIGINT, we need to
  gracefully release assigned memory and exit from module
  -------------------------------------------------------------------*/

  switch (signal)
  {
    case SIGTERM:
    case SIGINT:

    /*---------------------------------------------------------------
    Clean up mapping table. We start from head of mapping table
    and clean up memory for each client and finally clean up memory for
    that particular TMGI
    -----------------------------------------------------------------*/
    temp_tmgi = tmgi_to_client_mapping_table;
    next_tmgi = tmgi_to_client_mapping_table;

    while(next_tmgi != NULL)
    {
      temp_tmgi = next_tmgi;
      temp_client = temp_tmgi->client_info_head;
      next_client = temp_tmgi->client_info_head;

      while(next_client != NULL)
      {
        /*-----------------------------------------------------------
        Free client memory
        -------------------------------------------------------------*/
        temp_client = next_client;
        next_client = temp_client->next_client;
        temp_client->next_client = NULL;
        free(temp_client);
        temp_client = NULL;
      }

      /*-------------------------------------------------------------
      Free TMGI memory
      ---------------------------------------------------------------*/
      temp_tmgi->client_info_head = NULL;
      next_tmgi = temp_tmgi->next_tmgi;
      temp_tmgi->next_tmgi = NULL;
      free(temp_tmgi);
      temp_tmgi = NULL;
    }

    tmgi_to_client_mapping_table = NULL;

    /*--------------------------------------------------------------
    Close all open sockets
    -----------------------------------------------------------------*/

    close(embms_send_sockfd);

    close(embms_data_sockfd);
    embms_data_sockfd = SOCKET_INACTIVE;

    /*---------------------------------------------------------------
    Close server socket
    -----------------------------------------------------------------*/
    close(embms_control_sockfd);

    /*---------------------------------------------------------------
    Close client socket
    -----------------------------------------------------------------*/
    close(control_embms_sockfd);

    exit(0);

    break;

    default: LOG_MSG_INFO1("Received unexpected signal %d",signal,0,0);
    break;
  }
}

int main()
{
  int ret = 0;
  int n_read = 0;
  int val, n_write;
  uint32 dst_addr = 0;
  uint16 dst_port = 0;
  fd_set master_fd_set;
  struct sockaddr_in ClientAddr;
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  char buf[MAX_BUF_LEN];
  char data_buf[READ_BUFFER_SIZE];
  embms_tmgi_to_client_info_update *embms_update_info_buffer = NULL;
  embms_tmgi_to_client_info_internal *temp_tmgi;
  embms_client_info_internal *temp_client;
  int sock_status;
  int numBytes=0, len;
  struct sockaddr_un qmi_ip_sock_info;

  LOG_MSG_INFO1("Enterring Tunneling Module",0,0,0);
  FD_ZERO(&rset);

  /*-----------------------------------------------------------------
  Register the sighandlers, so the module may be shutdown with a
  kill command.
  -------------------------------------------------------------------*/
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);

  /*-----------------------------------------------------------------
  Create embms control server socket
  -------------------------------------------------------------------*/
  ret = create_embms_control_socket();
  if (ret == EMBMS_TM_ERROR)
  {
    LOG_MSG_ERROR("Failed to create embms control server socket.Tunneling Module Exiting",0,0,0);
    return -1;
  }

  /*-----------------------------------------------------------------
  Create embms control client socket
  -------------------------------------------------------------------*/
  if (create_udp_socket(&control_embms_sockfd) != EMBMS_TM_SUCCESS)
  {
    LOG_MSG_ERROR("Error creating embms control client socket", 0, 0, 0);
    return -1;
  }

  /*-----------------------------------------------------------------
  Initialize the Diag for QXDM logs
  -------------------------------------------------------------------*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     EMBMS_TM_LOG("Diag_LSM_Init failed !!");
  }

  while(1)
  {
    master_fd_set = rset;
    ret = select(max_fd+1, &master_fd_set, NULL, NULL, NULL);

    if (ret < 0)
    {
      LOG_MSG_ERROR("Error in select, errno:%d", errno, 0, 0);
      if( errno == EINTR )
       continue;
      else
       return -1;
    }

    if (FD_ISSET(embms_control_sockfd, &master_fd_set))
    {
      /*-------------------------------------------------------------
      Data received on control Socket
      ---------------------------------------------------------------*/
      LOG_MSG_INFO1("Received message on control socket",0,0,0);
      if ((n_read = recvfrom(embms_control_sockfd, buf, MAX_BUF_LEN-1 , 0,
                             (struct sockaddr *)&their_addr, &addr_len)) <= 0)
      {
        if (n_read == 0)
        {
          LOG_MSG_INFO1("Completed full recv from control_thread context Recvd bytes# %d ",n_read, 0, 0);
        }
        else
        {
          LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
        }
      }
      else
      {
        embms_update_info_buffer = (embms_tmgi_to_client_info_update*)buf;
        LOG_MSG_INFO1("Control Message Info: action_type = %d",
                      embms_update_info_buffer->action_type, 0, 0);

        if (embms_update_info_buffer->action_type == EMBMS_TM_SOCK_INFO)
        {
          ret = create_embms_send_socket(embms_update_info_buffer->data_port);
          if (ret == EMBMS_TM_ERROR)
          {
            LOG_MSG_ERROR("Error creating embms send data socket",0,0,0);
            sock_status = FALSE;
          }
          else
          {
            LOG_MSG_INFO1("EMBMS send socket created successfully",0,0,0);
            sock_status = TRUE;
          }

          qmi_ip_sock_info.sun_family = AF_UNIX;
          strlcpy(qmi_ip_sock_info.sun_path, QMI_IP_CONTROL_FILE, sizeof(qmi_ip_sock_info.sun_path));
          len = strlen(qmi_ip_sock_info.sun_path) + sizeof(qmi_ip_sock_info.sun_family);

          if ((numBytes = sendto(control_embms_sockfd, (void *)&sock_status, sizeof(bool), 0,
               (struct sockaddr *)&qmi_ip_sock_info, len)) == -1)
          {
            LOG_MSG_ERROR("Send Failed from tunneling module context", 0, 0, 0);
            return 0;
          }
          {
            LOG_MSG_INFO1("Send succeeded from tunneling module context", 0, 0, 0);
          }
        }

        else if ((embms_update_info_buffer->action_type == EMBMS_TM_ADD_CLIENT_ENTRY) ||
            (embms_update_info_buffer->action_type == EMBMS_TM_CLIENT_ACTIVATE_ALL_TMGI))
        {
          ret = add_client_entry_to_table(embms_update_info_buffer);
          if (ret != EMBMS_TM_SUCCESS)
          {
            LOG_MSG_ERROR("Error in adding entry to table %d",ret,0,0);
          }
          else
          {
            LOG_MSG_INFO1("Successfully added entry to table",0,0,0);
          }
        }

        else if (embms_update_info_buffer->action_type == EMBMS_TM_DELETE_CLIENT_ENTRY)
        {
          ret = delete_client_entry_from_table(embms_update_info_buffer);
          if (ret != EMBMS_TM_SUCCESS)
          {
            LOG_MSG_ERROR("Error in deleting entry from table %d",ret,0,0);
          }
          else
          {
            LOG_MSG_INFO1("Successfully deleted entry from table",0,0,0);
          }
        }

        else if (embms_update_info_buffer->action_type == EMBMS_TM_CLIENT_DEACTIVATE_ALL_TMGI)
        {
          ret = delete_client_entry_from_all_tmgi(embms_update_info_buffer);
          if (ret != EMBMS_TM_SUCCESS)
          {
            LOG_MSG_ERROR("Error in deleting entry from table %d",ret,0,0);
          }
          else
          {
            LOG_MSG_INFO1("Successfully deleted entry from table",0,0,0);
          }
        }

        else if (embms_update_info_buffer->action_type == EMBMS_TM_TMGI_DEACTIVATE)
        {
          ret = delete_tmgi_entry_from_table(embms_update_info_buffer);
          if (ret != EMBMS_TM_SUCCESS)
          {
            LOG_MSG_ERROR("Error in deleting entry from table %d",ret,0,0);
          }
          else
          {
            LOG_MSG_INFO1("Successfully deleted entry from table",0,0,0);
          }
        }

      }
    }

    else if (FD_ISSET(embms_data_sockfd, &master_fd_set))
    {
      /*-------------------------------------------------------------
      Data received on Data socket
      ---------------------------------------------------------------*/
      memset(data_buf, 0, READ_BUFFER_SIZE);
      n_read = recvfrom(embms_data_sockfd, data_buf, READ_BUFFER_SIZE, 0, NULL, NULL);
      if (n_read <= 0)
      {
        if (n_read == 0)
        {
          LOG_MSG_INFO1("Received ZERO bytes on data socket",0,0,0);
        }
        else
        {
          LOG_MSG_ERROR("Error receiveing data on raw socket",0,0,0);
        }
      }
      else if (n_read > 0)
      {
        dst_addr = 0;
        for (int j = DST_IP_START; j <= DST_IP_END; j++)
        {
          dst_addr = (dst_addr << 8);
          dst_addr = dst_addr + ((int) data_buf[j]);
        }

        dst_addr = htonl(dst_addr);

        dst_port = 0;
        dst_port = (int) data_buf[DST_PORT_START];
        dst_port = (dst_port << 8) + ((int)data_buf[DST_PORT_END]);

        dst_port = htons(dst_port);

        temp_tmgi = tmgi_to_client_mapping_table;
        while (temp_tmgi != NULL)
        {
          /*---------------------------------------------------------
          Logic to compare the source address of the
          received packets with tmgi multicast address and port
          -----------------------------------------------------------*/

          if (((dst_addr == temp_tmgi->tmgi_multicast_addr) && (dst_port == temp_tmgi->tmgi_port))||
               ((0 == temp_tmgi->tmgi_multicast_addr)&&(0 ==  temp_tmgi->tmgi_port)))
          {
            break;
          }
          temp_tmgi = temp_tmgi->next_tmgi;
        }

        if(temp_tmgi == NULL)
        {
          LOG_MSG_ERROR("Packet received does not belong to any tmgi",0,0,0);
          LOG_MSG_ERROR("dst port of packet = %d , dst addr = ",ntohs(dst_port),0,0);
          IPV4_ADDR_MSG(dst_addr);
        }

        /*-----------------------------------------------------------
        Matching TMGI entry found. Sending data to clients
        -------------------------------------------------------------*/
        else if (temp_tmgi->client_info_head != NULL)
        {
          temp_client = temp_tmgi->client_info_head;
          while (temp_client != NULL)
          {
            memset(&ClientAddr, 0, sizeof(ClientAddr));
            ClientAddr.sin_family = AF_INET;
            ClientAddr.sin_port = temp_client->port;
            ClientAddr.sin_addr.s_addr = temp_client->addr;
            temp_client->packets_sent = temp_client->packets_sent + 1;

            n_write = sendto(embms_send_sockfd, data_buf, n_read, 0,
                             (struct sockaddr *)&ClientAddr, sizeof(ClientAddr));
            if (n_write < 0)
            {
              LOG_MSG_ERROR("Send failed to wifi client, errno = %d",errno,0,0);
            }
            temp_client = temp_client->next_client;
          }
        }
      }
    }

  }
  return 0;
}

