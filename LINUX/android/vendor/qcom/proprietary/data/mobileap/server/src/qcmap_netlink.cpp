/******************************************************************************

                        QCMAP_NETLINK.C

******************************************************************************/

/******************************************************************************

  @file    qcmap_netlink.c
  @brief   QCMAP Netlink Messaging Implementation File

  DESCRIPTION
  Implementation file for QCMAP Netlink messaging functions.

  ---------------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        --------------------------------------------------------
10/01/13   pd         Created. Similar to qti_netlink.c, ipacm_netlink.c and
                      ipacm_neigh.c
01/11/14   sr         Added support for connected Devices in SoftAP
25/02/14   pm         Added handling of RTM_NEWADDR event
03/27/14   cp         Added support to DUN+SoftAP.
01/05/15   rk         qtimap offtarget support.
07/02/15   pm         Added a check for zero ipv6 addr
******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "qcmap_netlink.h"
#include "qcmap_cm_api.h"
#include "QCMAP_ConnectionManager.h"
#include <linux/kernel.h>
#include <linux/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <linux/filter.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>

/* Mask needed to ensure To ensure pid is 31 bits */
#define QCMAP_NL_PID_MASK                   (0x7FFFFFFF)
#define QCMAP_NL_PID                        (getpid() & QCMAP_NL_PID_MASK)
#define QCMAP_NL_GETNEIGH_SEQ               555666
#define QCMAP_NL_SOCK_PORTID                QCMAP_NL_PID + QCMAP_NL_GETNEIGH_SEQ
#define IP_NEIGH_FILE                       "/etc/neigh.txt"
#define TMP_FDB_TABLE                       "/tmp/tmpfdbtable.txt"
#define FDB_TABLE                           "/etc/fdbtable.txt"

/* Global QCMAP NL Listen Sock FD */
int qcmap_nl_listen_sock_fd=0;


/*===========================================================================
                              VARIABLE DEFINITIONS
===========================================================================*/
#define QCMAP_NLA_PARAM_NONE        (0x0000)
#define QCMAP_NDA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#define QCMAP_IFA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ifaddrmsg))))
#define QCMAP_RTN_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct rtmsg))))
extern unsigned int nl_qcmap_sockfd;
#define LOG_MSG_LEN           200

#define QCMAP_IFA_MIN_LINK_LOCAL_IP 0xa9fe0100 /* 169.254.1.0 */
#define QCMAP_IFA_MAX_LINK_LOCAL_IP 0xa9fefeff /* 169.254.254.255 */
#define QCMAP_IFA_IS_LINK_LOCAL_IP(x) \
        (htonl(x) >= QCMAP_IFA_MIN_LINK_LOCAL_IP && htonl(x) <= QCMAP_IFA_MAX_LINK_LOCAL_IP)

/*------------------------------------------------------------------------
  Structure to store USB TE Information
-------------------------------------------------------------------------*/
struct ether_addr qcmap_nl_usb_mac_addr;
struct ether_addr qcmap_nl_eth_mac_addr;
struct ether_addr qcmap_nl_wlan_mac_addr;


/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION GETDEVICETYPE()

DESCRIPTION

  This function is used to get the interface type (USB/WLAN/ETH) by parsing the
  fdb table

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_CM_SUCCESS on success
  QCMAP_CM_ERROR on failure


SIDE EFFECTS
  None

==========================================================================*/
static int GetDeviceType
(
  uint8* mac_addr,
  qcmap_nl_iface_t *iface
)
{
  FILE* fd;
  char *charMacAddr = NULL, *netIfName=NULL,*ptr;
  char stringline[MAX_COMMAND_STR_LEN];
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
  int bufferSize = QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01 +
                   QCMAP_MSGR_DEVICE_NAME_MAX_V01;

  if ( mac_addr == NULL || iface == NULL )
  {
    LOG_MSG_ERROR("GetDeviceType - NULL argument passed ", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  fd = fopen(FDB_TABLE,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("GetDeviceType - Error in opening %s",
                  FDB_TABLE,0,0);
    return QCMAP_CM_ERROR;
  }

  //Reset the file ptr to start of file
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    fclose(fd);
    return QCMAP_CM_ERROR;;
  }

  ds_mac_addr_ntop(mac_addr, mac_addr_string);

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    charMacAddr = strtok_r(stringline, " ", &ptr);

    if (charMacAddr!=NULL && !strncmp(charMacAddr, mac_addr_string, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01))
    {
      netIfName = strtok_r(NULL, " ", &ptr);
      if (netIfName == NULL)
      {
        *iface = QCMAP_NL_INVALID_NW_INTF;
      }
      else if (!(strncmp(netIfName,QCMAP_NL_RNDIS_INTERFACE,strlen(QCMAP_NL_RNDIS_INTERFACE))) ||
          !(strncmp(netIfName,QCMAP_NL_ECM_INTERFACE,strlen(QCMAP_NL_ECM_INTERFACE))))
      {
        *iface = QCMAP_NL_USB;
      }
      else if (!(strncmp(netIfName,QCMAP_NL_WLAN0_INTERFACE,strlen(QCMAP_NL_WLAN0_INTERFACE))) ||
               !(strncmp(netIfName,QCMAP_NL_WLAN1_INTERFACE,strlen(QCMAP_NL_WLAN1_INTERFACE))) ||
               !(strncmp(netIfName,QCMAP_NL_WLAN2_INTERFACE,strlen(QCMAP_NL_WLAN2_INTERFACE))) ||
               !(strncmp(netIfName,QCMAP_NL_WLAN3_INTERFACE,strlen(QCMAP_NL_WLAN3_INTERFACE))))
      {
        *iface = QCMAP_NL_WLAN;
      }
      else if (!strncmp(netIfName,QCMAP_NL_ETH_INTERFACE,strlen(QCMAP_NL_ETH_INTERFACE)))
      {
        *iface = QCMAP_NL_ETH;
      }
      else
      {
        *iface = QCMAP_NL_INVALID_NW_INTF;
      }
      LOG_MSG_INFO1("GetDeviceType - Iface %d ", *iface, 0, 0);
      fclose(fd);
      return QCMAP_CM_SUCCESS;
    }
  }
  fclose(fd);
  *iface = QCMAP_NL_INVALID_NW_INTF;
  return QCMAP_CM_ERROR;
}
/*===========================================================================

FUNCTION QCMAP_NL_CM_SEND()

DESCRIPTION

  This function
  - Send a message to QCMAP CM Process

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None

=============================================================================*/
static int qcmap_nl_cm_send(qcmap_nl_sock_msg_t * buf)
{
  struct sockaddr_un nl_qcmap;
  int numBytes=0, len;

  nl_qcmap.sun_family = AF_UNIX;
  strlcpy(nl_qcmap.sun_path, QCMAP_NL_UDS_FILE, sizeof(QCMAP_NL_UDS_FILE));
  len = strlen(nl_qcmap.sun_path) + sizeof(nl_qcmap.sun_family);

  if((numBytes = sendto(nl_qcmap_sockfd, (void *)buf, sizeof(qcmap_nl_sock_msg_t), 0,(struct sockaddr *)&nl_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from  QCMAP Netlink thread context, errno:%d",errno, 0, 0);
    return QCMAP_NL_FAILURE;
  }

    return QCMAP_NL_SUCCESS;
}

/*===========================================================================

FUNCTION send_nl_to_qcmap

DESCRIPTION

  This function
  - This function is used to send the netlink message to QCMAP context.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
==========================================================================*/

static void send_nl_to_qcmap( qcmap_nl_sock_msg_t* pktbuf )
{
  if ( pktbuf == NULL )
  {
    LOG_MSG_ERROR("send_nl_to_qcmap - NULL argument passed ", 0, 0, 0);
    return;
  }
  if ( QCMAP_NL_FAILURE == qcmap_nl_cm_send( pktbuf ) )
  {
    LOG_MSG_ERROR("Send Failed from QCMAP Netlink thread context", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION QCMAP_NL_DECODE_NLMSG()

DESCRIPTION

  This function
  - decodes the netlink message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None

=============================================================================*/
int qcmap_nl_decode_nlmsg
(
  const char       *buffer,
  unsigned int     buflen,
  qcmap_nl_msg_t   *msg_ptr
)
{

  struct nlmsghdr *nlh = (struct nlmsghdr*)buffer;
  char dev_name[IF_NAME_LEN];
  char ipaddr[INET6_ADDRSTRLEN], ipv6addr[INET6_ADDRSTRLEN];
  int ret_val = QCMAP_NL_FAILURE;
  qcmap_nl_sock_msg_t pktbuf;
  char logmsg[LOG_MSG_LEN];
  uint16       addr_family;
  uint16       addr_size;
  uint8 nullMac[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  static boolean kpi_first_wlan_client = true;
  static boolean kpi_first_usb_client = true;
  static boolean kpi_first_eth_client = true;
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  char netif_mac[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  char curr_bridge_mac[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  static boolean eth_client_log_print_v4 = false;
  static boolean eth_client_log_print_v6 = false;
  static boolean usb_client_log_print_v4 = false;
  static boolean usb_client_log_print_v6 = false;
  static boolean wlan_client_log_print_v4 = false;
  static boolean wlan_client_log_print_v6 = false;
  boolean print_in_dmesg = false;
  uint8 zero_array[QCMAP_MSGR_IPV6_ADDR_LEN_V01] = {0};

  memset(&pktbuf, 0, sizeof(qcmap_nl_sock_msg_t));
  memset(nullMac, 0, sizeof(nullMac));
  memset(netif_mac, 0, sizeof(netif_mac));


  while(NLMSG_OK(nlh, buflen))
  {
    /*-------------------------------------------------------------------------
      Decode the message type
    -------------------------------------------------------------------------*/
    switch(nlh->nlmsg_type)
    {
      /*----------------------------------------------------------------------
        RTM NEWNEIGH
      ----------------------------------------------------------------------*/
      case RTM_NEWNEIGH:
        LOG_MSG_INFO1("RTM_NEWNEIGH recvd",0,0,0);
        #ifndef FEATURE_QTIMAP_OFFTARGET
          if(QCMAP_NL_SUCCESS != qcmap_nl_decode_rtm_neigh(buffer, buflen, &(msg_ptr->nl_neigh_info)))
          {
            LOG_MSG_ERROR("Failed to decode rtm neighbor message\n",0,0,0);
            return QCMAP_NL_FAILURE;
          }
        #endif

        if ( nlh->nlmsg_seq == QCMAP_NL_GETNEIGH_SEQ )
        {
          LOG_MSG_INFO1("Rcvd NEWNEIGH Event as a result of GETNEIGH event",0,0,0);
          if ((msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family == AF_INET6) &&
              (msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data[0] == 0x33) &&
              (msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data[1] == 0x33))
            {
              LOG_MSG_INFO1("Recieved Neigh Event for IPv6 Multicast MAC Address --- Ignore",0,0,0);
              break;
            }
        }

        ret_val = qcmap_nl_get_interface_details(dev_name,
                                 &(msg_ptr->nl_neigh_info.metainfo.ndm_ifindex),
                                 QCMAP_GET_IFNAME);
        if(ret_val != QCMAP_UTIL_SUCCESS)
        {
           LOG_MSG_ERROR("Error while getting interface index",0,0,0);
           return QCMAP_NL_FAILURE;
        }

        //This check is done prevent handling of netlink messages which have NULL MAC addr
        if(!(memcmp(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                    nullMac,sizeof(nullMac))))
        {
          LOG_MSG_ERROR("RTM_NEWNEIGH received with NULL MAC",0,0,0);
          break;
        }

        memset(netif_mac, 0, sizeof(netif_mac));
        if( msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family == 0 )
        {
          if (QcMapMgr->GetHWAddr( netif_mac, dev_name) != QCMAP_CM_SUCCESS)
          {
            LOG_MSG_ERROR("Could not fetch Netif MAC address.", 0, 0, 0);
          }
          if (!memcmp( netif_mac,
              msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
              QCMAP_MSGR_MAC_ADDR_LEN_V01))
          {
            LOG_MSG_INFO1("Rcvd NEWNEIGH Event with Netif MAC addr", 0, 0, 0);
            break;
          }
        }

        if ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_RNDIS_INTERFACE) !=
                                                           QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ECM_INTERFACE) !=
                                                           QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) !=
                                                             QCMAP_UTIL_SUCCESS)&&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ETH_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS))
        {
          LOG_MSG_INFO1("New Neigbour Message was not recieved"
                        " on USB/BRIDGE/ETHERNET/WLAN Interface",0,0,0);
          break;
        }

        // If new Neigh is received on USB interface and there is no IP Addresss.
        // Store the MAC address, since its client MAC and send NEWNEIGH to QCMAP
       if (((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_RNDIS_INTERFACE) ==
                                                           QCMAP_UTIL_SUCCESS) ||
             (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ECM_INTERFACE) ==
                                                          QCMAP_UTIL_SUCCESS)) &&
             (msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family == 0))
        {
          if ( kpi_first_usb_client )
          {
            memcpy((void *)&qcmap_nl_usb_mac_addr.ether_addr_octet,
                   msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                   sizeof(ether_addr));
            snprintf((char *)logmsg,
                     LOG_MSG_LEN,
                     "echo USB Client Mac Address is %s > /dev/kmsg",
                     ether_ntoa(&qcmap_nl_usb_mac_addr));
            ds_system_call((char *)logmsg,strlen(logmsg));
            kpi_first_usb_client = false;
            usb_client_log_print_v4 = true;
            usb_client_log_print_v6 = true;
         }
          break;
        }

        // If new Neigh is received on ETH interface and there is no IP Addresss.
        // Store the MAC address, since its client MAC and send NEWNEIGH to QCMAP
        if ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ETH_INTERFACE ) ==
                                                      QCMAP_UTIL_SUCCESS) &&
             (msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family == 0) )
        {
          if ( kpi_first_eth_client )
          {
            memcpy((void *)&qcmap_nl_eth_mac_addr.ether_addr_octet,
                    msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                    sizeof(ether_addr));
            snprintf((char *)logmsg,
                      LOG_MSG_LEN,
                      "echo ETHERNET Client Mac Address is %s > /dev/kmsg",
                      ether_ntoa(&qcmap_nl_eth_mac_addr));
            ds_system_call((char *)logmsg,strlen(logmsg));
            kpi_first_eth_client = false;
            eth_client_log_print_v4 = true;
            eth_client_log_print_v6 = true;
          }
          break;
        }

        // If new Neigh is received on any WLAN iface interface and there is no IP Addresss.
        // Store the MAC address, since its client MAC
        if ( ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) ==
                                                        QCMAP_UTIL_SUCCESS) ||
             ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) ==
                                                      QCMAP_UTIL_SUCCESS)) ||
             ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) ==
                                                      QCMAP_UTIL_SUCCESS)) ||
             ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) ==
                                                      QCMAP_UTIL_SUCCESS))) &&
             (msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family == 0) )
        {
          if ( kpi_first_wlan_client )
          {
            memcpy((void *)&qcmap_nl_wlan_mac_addr.ether_addr_octet,
                    msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                    sizeof(ether_addr));
            snprintf((char *)logmsg,
                      LOG_MSG_LEN,
                      "echo WLAN Client Mac Address is %s > /dev/kmsg",
                      ether_ntoa(&qcmap_nl_wlan_mac_addr));
            ds_system_call((char *)logmsg,strlen(logmsg));
            kpi_first_wlan_client =  false;
            wlan_client_log_print_v4 = true;
            wlan_client_log_print_v6 = true;
         }
        }

        /* If NEWNEIGH is recieved on Bridge Interface or USB Interface and family=0 OR ipaddr =0;ignore*/
        if (((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_RNDIS_INTERFACE) ==
                                                           QCMAP_UTIL_SUCCESS) ||
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ECM_INTERFACE) ==
                                                           QCMAP_UTIL_SUCCESS) ||
            (qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) ==
                                                          QCMAP_UTIL_SUCCESS)  ||
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ETH_INTERFACE) ==
                                                          QCMAP_UTIL_SUCCESS)  ||
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) ==
                                                          QCMAP_UTIL_SUCCESS)  ||
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) ==
                                                          QCMAP_UTIL_SUCCESS)  ||
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) ==
                                                          QCMAP_UTIL_SUCCESS)  ||
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) ==
                                                          QCMAP_UTIL_SUCCESS)) &&
             ((msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family == 0) ||
              (msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding == 0)))
        {
          ds_log_med(" NEWNEIGH Received on %s IFACE with IP family as %d and"
                     " ip_addr as %d ",dev_name,
                     msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family,
                     msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding);
          break;
        }

        //Populate netlink event type
        pktbuf.nl_event = QCMAP_NL_NEWNEIGH;

        //Copy the MAC address from Netlink event
        memcpy(pktbuf.nl_addr.mac_addr,
               msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
               QCMAP_MSGR_MAC_ADDR_LEN_V01);

        addr_family = msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family;
        addr_size = (addr_family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;

        if (addr_family == AF_INET)
        {
          //Copy the IPv4 address
          memcpy(&pktbuf.nl_addr.ip_addr,
                 msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
                 sizeof(uint32_t));

          if (pktbuf.nl_addr.ip_addr &&
              !QCMAP_IFA_IS_LINK_LOCAL_IP((pktbuf.nl_addr.ip_addr)))
          {
            pktbuf.nl_addr.isValidIPv4address = true;
          }
          else
          {
            break;
          }
        }
        else if (addr_family == AF_INET6)
        {
          //Copy the IPv6 address
          memcpy(pktbuf.nl_addr.ip_v6_addr,
                 msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
                 QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          pktbuf.nl_addr.isValidIPv6address = true;
        }
        else
        {
          LOG_MSG_ERROR("Incorrect Address Family type %d",addr_family,0,0);
          break;
        }

        print_in_dmesg = false;

        /* If Received event *DOES* have USB Client MAC and
               is generated on bridge interface then copy client IP */
        if ((qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) == QCMAP_UTIL_SUCCESS) &&
            (!memcmp(&qcmap_nl_usb_mac_addr.ether_addr_octet,
                     msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                     sizeof(qcmap_nl_usb_mac_addr))) && (usb_client_log_print_v4 ||
                     usb_client_log_print_v6))
        {
          if ( usb_client_log_print_v4 && pktbuf.nl_addr.isValidIPv4address )
          {
            usb_client_log_print_v4 = false;
            print_in_dmesg = true;
          }
          else if ( usb_client_log_print_v6 && pktbuf.nl_addr.isValidIPv6address )
          {
            usb_client_log_print_v6 = false;
            print_in_dmesg = true;
          }

          if ( print_in_dmesg )
          {
            inet_ntop(addr_family,
                      (void *)msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
                      ipaddr, addr_size);
            snprintf((char *)logmsg,
                     LOG_MSG_LEN,
                     "echo QCMAP:USB Client IP Addr %s > /dev/kmsg",
                     ipaddr);
            ds_system_call((char *)logmsg,strlen(logmsg));
          }
        }
        else if ((qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) == QCMAP_NL_SUCCESS ) &&
                 (!memcmp(&qcmap_nl_eth_mac_addr.ether_addr_octet,
                  msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                  sizeof(qcmap_nl_eth_mac_addr))) && (eth_client_log_print_v4 ||
                  eth_client_log_print_v6 ))
        {
          if ( eth_client_log_print_v4 && pktbuf.nl_addr.isValidIPv4address )
          {
            eth_client_log_print_v4 = false;
            print_in_dmesg = true;
          }
          else if ( eth_client_log_print_v6 && pktbuf.nl_addr.isValidIPv6address )
          {
            eth_client_log_print_v6 = false;
            print_in_dmesg = true;
          }

          if ( print_in_dmesg )
          {
            inet_ntop(addr_family,
                      (void *)msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
                      ipaddr, addr_size);
            snprintf((char *)logmsg,
                     LOG_MSG_LEN,
                     "echo QCMAP:Ethernet Client IP Addr %s > /dev/kmsg",
                     ipaddr);
            ds_system_call((char *)logmsg,strlen(logmsg));
          }
        }
        else if ((qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE)  == QCMAP_NL_SUCCESS)  &&
                 (!memcmp(&qcmap_nl_wlan_mac_addr.ether_addr_octet,
                 msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                 sizeof(qcmap_nl_wlan_mac_addr))) && ( wlan_client_log_print_v4 ||
                 wlan_client_log_print_v6) )
        {
          if ( wlan_client_log_print_v4 && pktbuf.nl_addr.isValidIPv4address )
          {
            wlan_client_log_print_v4 = false;
            print_in_dmesg = true;
          }
          else if ( wlan_client_log_print_v6 && pktbuf.nl_addr.isValidIPv6address )
          {
            wlan_client_log_print_v6 = false;
            print_in_dmesg = true;
          }

          if ( print_in_dmesg )
          {
            inet_ntop(addr_family,
                      (void *)msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
                      ipaddr, addr_size);
            snprintf( (char *)logmsg,
                      LOG_MSG_LEN,
                      "echo QCMAP:Wlan Client IP Addr %s > /dev/kmsg",
                      ipaddr );
            ds_system_call((char *)logmsg,strlen(logmsg));
          }
        }

        snprintf(logmsg, MAX_COMMAND_STR_LEN, "rm %s",FDB_TABLE);
        ds_system_call(logmsg, strlen(logmsg));

        snprintf(logmsg, MAX_COMMAND_STR_LEN, "rm %s",TMP_FDB_TABLE);
        ds_system_call(logmsg, strlen(logmsg));


        snprintf((char *)logmsg,
                 LOG_MSG_LEN,
                 "bridge fdb show > %s", TMP_FDB_TABLE);
        ds_system_call((char *)logmsg,strlen(logmsg));

        snprintf(logmsg, MAX_COMMAND_STR_LEN,
                 "cut -f1,3 -d ' ' %s > %s ",
                 TMP_FDB_TABLE, FDB_TABLE);
        ds_system_call(logmsg, strlen(logmsg));

        if( QCMAP_CM_ERROR ==  GetDeviceType(\
                               msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                               &pktbuf.nl_iface))
        {
          ds_mac_addr_ntop(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                           mac_addr_char);
          ds_log_med("Device Type not found %d for MAC %s", pktbuf.nl_iface,
                     mac_addr_char, 0);
          break;
        }

        if ( pktbuf.nl_iface == QCMAP_NL_INVALID_NW_INTF )
        {
          LOG_MSG_ERROR("Device Type is invalid %d", pktbuf.nl_iface, 0, 0);
          break;
        }

        send_nl_to_qcmap( &pktbuf );
        break;

/*----------------------------------------------------------------------
         RTM DELNEIGH
----------------------------------------------------------------------*/
      case RTM_DELNEIGH:
        LOG_MSG_INFO1("RTM_DELNEIGH recvd", 0, 0, 0);
        #ifndef FEATURE_QTIMAP_OFFTARGET
          if(QCMAP_NL_SUCCESS != qcmap_nl_decode_rtm_neigh(buffer,
                                                           buflen,
                                                           &(msg_ptr->nl_neigh_info)))
          {
            LOG_MSG_ERROR("Failed to decode rtm neighbor message\n",0,0,0);
            return QCMAP_NL_FAILURE;
          }
        #endif
        ret_val = qcmap_nl_get_interface_details(dev_name,
                                 &(msg_ptr->nl_neigh_info.metainfo.ndm_ifindex),
                                 QCMAP_GET_IFNAME);
        if(ret_val != QCMAP_UTIL_SUCCESS)
        {
           LOG_MSG_ERROR("Error while getting interface index",0,0,0);
           return QCMAP_NL_FAILURE;
        }

        //This check is done prevent handling of netlink messages which have NULL MAC addr
        if(!(memcmp(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                    nullMac,sizeof(nullMac))))
        {
          LOG_MSG_ERROR("RTM_DELNEIGH received with NULL MAC",0,0,0);
          break;
        }

        if ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_RNDIS_INTERFACE) !=
                                                           QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ECM_INTERFACE) !=
                                                           QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) !=
                                                             QCMAP_UTIL_SUCCESS)&&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ETH_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) !=
                                                             QCMAP_UTIL_SUCCESS))
        {
           LOG_MSG_INFO1("DEL Neigbour Message was not recieved on"
                         " USB/BRIDGE/ETHERNET/USB/WLAN Interface", 0, 0, 0);
           break;
        }

        addr_family = msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family;
        addr_size = (addr_family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;

        if (addr_family == AF_INET)
        {
          //Copy the IPv4 address
          memcpy(&pktbuf.nl_addr.ip_addr,
                 msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
                 sizeof(uint32_t));
          if (pktbuf.nl_addr.ip_addr &&
              !QCMAP_IFA_IS_LINK_LOCAL_IP((pktbuf.nl_addr.ip_addr)))
          {
            pktbuf.nl_addr.isValidIPv4address = true;
          }
          else
          {
            break;
          }

          if ( qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) == QCMAP_NL_SUCCESS )
          {
            pktbuf.nl_iface = QCMAP_NL_BRIDGE;
          }

          //Populate netlink event type
          pktbuf.nl_event = QCMAP_NL_DELNEIGH;

          //Copy the MAC address from Netlink event
          memcpy(pktbuf.nl_addr.mac_addr,
                 msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
                 QCMAP_MSGR_MAC_ADDR_LEN_V01);
        }
        else if (addr_family == AF_INET6)
        {
          if ( qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) == QCMAP_NL_SUCCESS ||
               qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) == QCMAP_NL_SUCCESS ||
               qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) == QCMAP_NL_SUCCESS ||
               qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) == QCMAP_NL_SUCCESS)
          {
            pktbuf.nl_iface = QCMAP_NL_WLAN;
          }
          else if (qcmap_nl_is_recv_on_dev(dev_name, ECM_IFACE) == QCMAP_NL_SUCCESS)
          {
            pktbuf.nl_iface = QCMAP_NL_USB;
          }
          else
          {
            LOG_MSG_ERROR("Delneigh on other iface", 0, 0, 0);
            break;
          }

          if (msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding != NULL &&
              memcmp(msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding, zero_array, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
          {
            pktbuf.nl_addr.isValidIPv6address = TRUE;
            memcpy(&pktbuf.nl_addr.ip_v6_addr, &msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding, sizeof(struct in6_addr));
            pktbuf.nl_addr.ip_vsn = IP_V6;
            IPV6_ADDR_MSG(pktbuf.nl_addr.ip_v6_addr);
          }
          else
          {
            LOG_MSG_ERROR("Received zero ipv6 addr",0,0,0);
            break;
          }

          pktbuf.nl_event = QCMAP_NL_DELNEIGH;
        }
        else
        {
          LOG_MSG_ERROR("Incorrect Address Family type %d for DELNEIGH",
          addr_family, 0, 0);
          break;
        }
        send_nl_to_qcmap( &pktbuf );

        break;

      case RTM_NEWADDR:
        LOG_MSG_INFO1("RTM_NEWADDR recvd",0,0,0);
        if(QCMAP_NL_SUCCESS != qcmap_nl_decode_rtm_addr(buffer,
                                                        buflen,
                                                        &(msg_ptr->nl_if_addr)))
        {
          LOG_MSG_ERROR("Failed to decode rtm address message\n",0,0,0);
          return QCMAP_NL_FAILURE;
        }
        ret_val = qcmap_nl_get_interface_details(dev_name,
                                    &(msg_ptr->nl_if_addr.metainfo.ifa_index),
                                    QCMAP_GET_IFNAME);
        if(ret_val != QCMAP_UTIL_SUCCESS)
        {
          LOG_MSG_ERROR("Error while getting interface name",0,0,0);
          return QCMAP_NL_FAILURE;
        }

        // Only listen to wlan, eth, ecm and bridge interface
        if ((qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) ==
                                                           QCMAP_UTIL_FAILURE) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) ==
                                                           QCMAP_UTIL_FAILURE) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) ==
                                                           QCMAP_UTIL_FAILURE) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) ==
                                                           QCMAP_UTIL_FAILURE) &&
            (qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) ==
                                                           QCMAP_UTIL_FAILURE) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ECM_INTERFACE) ==
                                                             QCMAP_UTIL_FAILURE) &&
            (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_ETH_INTERFACE) ==
                                                             QCMAP_UTIL_FAILURE))
        {
          LOG_MSG_INFO1("RTM_NEWADDR recvd on interface other than"
                        " WLAN/ETH/BRIDGE/ECM",0,0,0);
          break;
        }

        // NEWADDR received for IPv4 events
        if (msg_ptr->nl_if_addr.metainfo.ifa_family == AF_INET)
        {
          /* Ignore link local ip. */
          if (msg_ptr->nl_if_addr.attr_info.ifa_local_v4.s_addr &&
              !QCMAP_IFA_IS_LINK_LOCAL_IP(msg_ptr->nl_if_addr.attr_info.ifa_local_v4.s_addr))
          {
            pktbuf.nl_addr.isValidIPv4address = TRUE;
            pktbuf.nl_addr.ip_addr = msg_ptr->nl_if_addr.attr_info.ifa_local_v4.s_addr;
            pktbuf.nl_addr.ip_vsn = IP_V4;
            LOG_MSG_INFO1("IPV4 Addr of %s is ",dev_name,0,0);
            IPV4_ADDR_MSG(pktbuf.nl_addr.ip_addr);
          }
          else
          {
            break;
          }
        }
        // NEWADDR Received for IPv6 events
        else if (msg_ptr->nl_if_addr.metainfo.ifa_family == AF_INET6)
        {
          if (memcmp(msg_ptr->nl_if_addr.attr_info.ifa_local_v6.s6_addr16, zero_array, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
          {
            pktbuf.nl_addr.isValidIPv6address = TRUE;
            memcpy(&pktbuf.nl_addr.ip_v6_addr, &msg_ptr->nl_if_addr.attr_info.ifa_local_v6.s6_addr16, sizeof(struct in6_addr));
            pktbuf.nl_addr.ip_vsn = IP_V6;
            LOG_MSG_INFO1("IPV6 Addr of %s is ",dev_name,0,0);
            IPV6_ADDR_MSG(pktbuf.nl_addr.ip_v6_addr);
          }
          else
          {
            LOG_MSG_ERROR("Received zero ipv6 addr",0,0,0);
            break;
          }
        }

        else
        {
          break;
        }

        // Send Update to QCMAP
        pktbuf.nl_event = QCMAP_NL_NEWADDR;

        if (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN0_INTERFACE) == QCMAP_UTIL_SUCCESS ||
            qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN1_INTERFACE) == QCMAP_UTIL_SUCCESS ||
            qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN2_INTERFACE) == QCMAP_UTIL_SUCCESS ||
            qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_WLAN3_INTERFACE) == QCMAP_UTIL_SUCCESS)
          pktbuf.nl_iface = QCMAP_NL_WLAN;
        else if (qcmap_nl_is_recv_on_dev(dev_name, BRIDGE_IFACE) == QCMAP_UTIL_SUCCESS)
          pktbuf.nl_iface = QCMAP_NL_BRIDGE;
        else if (qcmap_nl_is_recv_on_dev(dev_name, ETH_IFACE) ==
                                                           QCMAP_UTIL_SUCCESS)
          pktbuf.nl_iface = QCMAP_NL_ETH;
        else
          pktbuf.nl_iface = QCMAP_NL_USB;

        LOG_MSG_INFO1("IPV6 Addr of Iface %d is ",pktbuf.nl_iface,0,0);
        IPV6_ADDR_MSG(pktbuf.nl_addr.ip_v6_addr);

        if (QCMAP_NL_FAILURE == qcmap_nl_cm_send(&pktbuf))
        {
          LOG_MSG_ERROR("Send Failed from QCMAP Netlink thread context",
                        0, 0, 0);
        }
        else
        {
          LOG_MSG_INFO1("Send succeeded in QCMAP Netlink thread context",
                        0, 0, 0);
        }
        break;

      /*----------------------------------------------------------------------
        RTM_NEWROUTE
      ----------------------------------------------------------------------*/
      case RTM_NEWROUTE:
        if(QCMAP_NL_SUCCESS != qcmap_nl_decode_rtm_route(buffer, buflen, &(msg_ptr->nl_route_info)))
        {
          LOG_MSG_ERROR("Failed to decode rtm neighbor message\n",0,0,0);
          return QCMAP_NL_FAILURE;
        }

        if ( !((msg_ptr->nl_route_info.attr_info.param_mask & QCMAP_NL_ROUTE_INFO_DST_ADDR) &&
             (msg_ptr->nl_route_info.attr_info.param_mask & QCMAP_NL_ROUTE_INFO_IFINDEX)) )
        {
          /* Not the required route message ignore and continue. */
          break;
        }

        ret_val = qcmap_nl_get_interface_details(dev_name,
                                       &(msg_ptr->nl_route_info.attr_info.ifindex),
                                       QCMAP_GET_IFNAME);
        if(ret_val != QCMAP_UTIL_SUCCESS)
        {
           LOG_MSG_ERROR("Error while getting interface index",0,0,0);
           return QCMAP_NL_FAILURE;
        }

        // Only listen to PPP interface
        if (qcmap_nl_is_recv_on_dev(dev_name, QCMAP_NL_PPP_INTERFACE) ==
                                                             QCMAP_UTIL_FAILURE)
        {
           LOG_MSG_INFO1("Route message was not recieved on PPP Interface",0,0,0);
           break;
        }

        if ( msg_ptr->nl_route_info.metainfo.rtm_family != AF_INET6)
        {
           LOG_MSG_INFO1("Route message was not recieved on PPP Interface",0,0,0);
           break;
        }

        // Send Update to QCMAP
        pktbuf.nl_event = QCMAP_NL_PPP_IPV6_ROUTE;
        pktbuf.nl_iface = QCMAP_NL_PPP;
        memcpy(&pktbuf.nl_addr.ip_v6_addr, &msg_ptr->nl_route_info.attr_info.dst_addr, sizeof(struct in6_addr));
        if (QCMAP_NL_FAILURE == qcmap_nl_cm_send(&pktbuf))
        {
          LOG_MSG_ERROR("Send Failed from PPP QCMAP Netlink thread context", 0, 0, 0);
        }
        else
        {
          LOG_MSG_INFO1("Send succeeded in PPP QCMAP Netlink thread context", 0, 0, 0);
        }
        break;
      default:
        break;
    }
    nlh = NLMSG_NEXT(nlh, buflen);
  }
  return QCMAP_NL_SUCCESS;
}


/*===========================================================================

FUNCTION QCMAP_NL_RECV_MSG()

DESCRIPTION

  This function
  - receives the netlink message associated with the netlink event which
  was received on select call
  - decodes the netlink message to find if the netlink message was found
  on the desired interface
  - Posts it to QCMAP

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_nl_recv_msg(int fd)
{
  struct msghdr                    *msghdr = NULL;
  struct sockaddr_nl               *nladdr = NULL;
  struct iovec                     *iov = NULL;
  unsigned int                     msglen = 0;
  qcmap_nl_msg_t                   *nlmsg = NULL;
  int                              interface_num,interface;

  /*--------------------------------------------------------------------------
    Allocate memory to decode the netlink message
  ---------------------------------------------------------------------------*/
  nlmsg = malloc(sizeof(qcmap_nl_msg_t));
  if(NULL == nlmsg)
  {
    LOG_MSG_ERROR("Failed alloc of nlmsg",0,0,0);
    goto error;
  }
  else
  {
    /*--------------------------------------------------------------------------
      Receive the netlink message
    ---------------------------------------------------------------------------*/
    if(QCMAP_UTIL_SUCCESS != qcmap_nl_recv(fd, &msghdr, &msglen))
    {
      LOG_MSG_ERROR("Failed to receive nl message",0,0,0);
      goto error;
    }

    if( msghdr == NULL )
    {
      LOG_MSG_ERROR("Netlink: NULL message received",0,0,0);
      goto error;
    }

    nladdr = msghdr->msg_name;
    iov = msghdr->msg_iov;

    /*--------------------------------------------------------------------------
      Decode the received netlink message
    ---------------------------------------------------------------------------*/
    memset(nlmsg, 0, sizeof(qcmap_nl_msg_t));
    if(QCMAP_NL_SUCCESS != qcmap_nl_decode_nlmsg((char*)iov->iov_base,
                                          msglen,
                                          nlmsg))
    {
      LOG_MSG_ERROR("Failed to decode nl message",0,0,0);
      goto error;
    }
  }

  if(msghdr)
  {
    qcmap_nl_release_msg(msghdr);
  }
  if(nlmsg)
  {
    free(nlmsg);
  }

  return QCMAP_NL_SUCCESS;
error:
  if(msghdr)
  {
    qcmap_nl_release_msg(msghdr);
  }
  if(nlmsg)
  {
    free(nlmsg);
  }

  return QCMAP_NL_FAILURE;
}


/*===========================================================================

FUNCTION QCMAP_OPEN_PACKET_SOCKET()

DESCRIPTION

  This function
  - opens paket sockets
  - binds the socket to listen to the required NA packets.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
static int qcmap_open_packet_socket
(
  qcmap_pf_sk_info_t     *sk_info
)
{
  int *packet_sock;
  int ret= -1;
  struct sockaddr_ll sa;
  struct sock_fprog fprog;

  struct sock_filter filter[] =
  {
    /*
     * {opcode, jump to offset if true, jump to offset if false, value}
     * 0x30 --> Opcode stands for offset value to move.
     * 0x15 --> Opcode to compare with value.
    */
    /* Move to offset 0 i.e check the first byte of IP packet.*/
    { 0x30, 0, 0, 0x00000000 },
    /* Compare if it is IPV6. If true move to next command, else move to end.*/
    { 0x15, 0, 7, 0x00000060},
    /* Move to offset 6 i.e to check if it is ICMPV6.*/
    { 0x30, 0, 0, 0x00000006 },
    /* Compare if it is ICMPV6. If true move to next command, else move to end.*/
    { 0x15, 0, 5, 0x0000003a },
    //{ 0x15, 0, 3, 0x0000003a },
    /* Move to offset 40 i.e to check if it is RS/RA/NS/NA packet.*/
    { 0x30, 0, 0, 0x00000028 },
    /* Compare if it is RS packet. If true return, else compare NS.*/
    { 0x15, 3, 0, 0x00000085 },
    /* Compare if it is RA packet. If true return, else compare NS.*/
    { 0x15, 2, 0, 0x00000086 },
    //{ 0x15, 0, 1, 0x00000086 },
    /* Compare if it is NS packet. If true return, else compare NA.*/
    { 0x15, 1, 0, 0x00000087 },
    /* Compare if it is NA packet. If true move to next command, else move to end.*/
    { 0x15, 0, 1, 0x00000088 },
    /* Return maximum of 0xffff bytes..*/
    { 0x6, 0, 0, 0x0000ffff },
    /* Return 0 bytes..*/
    { 0x6, 0, 0, 0x00000000 }
  };

  ds_assert(sk_info != NULL);

  fprog.len = (sizeof(filter)/sizeof(struct sock_filter));
  fprog.filter = filter;

  LOG_MSG_INFO1("Entering qcmap_open_packet_socket %d 0x%p", fprog.len, fprog.filter, 0);

  packet_sock = &(sk_info->sk_fd);

/*--------------------------------------------------------------------------
  Open netlink socket for specified protocol
---------------------------------------------------------------------------*/
  if ((*packet_sock = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IPV6))) < 0)
  {
    LOG_MSG_ERROR("Cannot open packet socket",0,0,0);
    return QCMAP_NL_FAILURE;
  }

  LOG_MSG_INFO1("Socket open succeeds",0,0,0);

  /* bind this socket to specific iface */
  memset(&sa, 0, sizeof(sa));
  sa.sll_family = AF_PACKET;
  sa.sll_protocol = htons(ETH_P_IPV6);
  sa.sll_ifindex = 0;

  if (bind(*packet_sock, (struct sockaddr *)&sa, sizeof(sa)) != 0)
  {
    LOG_MSG_ERROR("couldn't bind socket %d to iface 0, errno=%d",
           *packet_sock, errno, 0);
    return QCMAP_NL_FAILURE;
  }

  /* install filter to only receive unsolicited ICMPv6 RA traffic */
  if (setsockopt(*packet_sock,
                 SOL_SOCKET,
                 SO_ATTACH_FILTER,
                 &fprog,
                 sizeof(fprog)) == -1)
  {
    LOG_MSG_ERROR("couldn't attach BPF filter on sock %d, errno=%d",
                  *packet_sock, errno, 0);
    return QCMAP_NL_FAILURE;
  }

  return QCMAP_NL_SUCCESS;

}

/*===========================================================================
FUNCTION QCMAP_PACKET_SOCKET_INIT()

DESCRIPTION

  This function initializes packet socket to listen for RA packets.
DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None
=========================================================================*/
int qcmap_packet_socket_init
(
  qcmap_sk_fd_set_info_t * sk_fdset,
  qcmap_sock_thrd_fd_read_f read_f
)
{
  qcmap_pf_sk_info_t     sk_info;
  int                    ret_val;

  LOG_MSG_INFO1("Initialize packet socket for QCMAP",0,0,0);
  memset(&sk_info, 0, sizeof(qcmap_pf_sk_info_t));

  /*---------------------------------------------------------------------------
    Open netlink sockets
  ----------------------------------------------------------------------------*/

  if( qcmap_open_packet_socket( &sk_info ) != QCMAP_NL_SUCCESS)
  {
    LOG_MSG_ERROR("Open: QCMAP packet socket failed",0,0,0);
    return QCMAP_NL_FAILURE;
  }

  /*--------------------------------------------------------------------------
    Add the NETLINK socket to the list of sockets that the listener
    thread should listen on.
  ---------------------------------------------------------------------------*/
  if( qcmap_nl_addfd_map(sk_fdset,sk_info.sk_fd,read_f,
                         QCMAP_NL_MAX_NUM_OF_FD) != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("cannot add packet sock for reading",0,0,0);
    close(sk_info.sk_fd);
    return QCMAP_NL_FAILURE;
  }

  LOG_MSG_INFO1("add fd map succeeds FD: %d ",sk_info.sk_fd, 0, 0);

  return QCMAP_NL_SUCCESS;
}
/*===========================================================================

FUNCTION QCMAP_PACKET_SOCK_RECV_MSG()

DESCRIPTION

  This function
  - receives the packet socket message.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_packet_sock_recv_msg(int fd)
{
  struct ip6_hdr *ipv6_packet;
  struct icmp6_hdr *icmp6;
  struct nd_opt_prefix_info *prefix_info;/* prefix information */
  struct nd_opt_hdr *curr_option;
  char nd_packet_buf[MAX_ND_PKT_SIZE], *option_buf;
  ssize_t pkt_size;
  int i=0;
  char ipaddr[INET6_ADDRSTRLEN];
  char iface_name[IF_NAME_LEN];
  int index;
  qcmap_nl_sock_msg_t pktbuf;
  boolean link_address_set = false;
  uint8 nullMac[QCMAP_MSGR_MAC_ADDR_LEN_V01] = {0};
  struct sockaddr rec_addr;
  socklen_t addr_len = sizeof(struct sockaddr_storage);
  struct sockaddr_ll *sll;

  /*--------------------------------------------------------------------------
    Read the packet over the socket.
  ---------------------------------------------------------------------------*/
  LOG_MSG_INFO1("ND packet received", 0, 0, 0);
  memset(nd_packet_buf, 0, sizeof(nd_packet_buf));
  pkt_size = recvfrom(fd, nd_packet_buf, MAX_ND_PKT_SIZE, 0, &rec_addr, &addr_len);

  if ( pkt_size <= 0 )
  {
    LOG_MSG_ERROR("Not the expected packet %d: ignore", errno, 0, 0);
    return QCMAP_NL_FAILURE;
  }

  /* Check if the packet is ICMPV6. */
  ipv6_packet = (struct ip6_hdr *)nd_packet_buf;
  if ( ipv6_packet->ip6_nxt != IPPROTO_ICMPV6 )
  {
    LOG_MSG_ERROR("Not ICMPV6 packet 0x%x: ignore", ipv6_packet->ip6_nxt, 0, 0);
    return QCMAP_NL_FAILURE;
  }

  icmp6 = (struct icmp6_hdr *) (nd_packet_buf + sizeof(struct ip6_hdr));
  LOG_MSG_INFO1("type = %d, code = %d", icmp6->icmp6_type, icmp6->icmp6_code, 0);

  memset(&pktbuf, 0, sizeof(qcmap_nl_sock_msg_t));
  pktbuf.nl_iface = QCMAP_NL_ANY;

  /* iface num for ap-sta router mode */
  sll = (struct sockaddr_ll *)&rec_addr;
  index = sll->sll_ifindex;
  if(qcmap_nl_get_interface_details(iface_name, &index, QCMAP_GET_IFNAME) != QCMAP_NL_SUCCESS)
  {
    LOG_MSG_ERROR("Unable to get iface name", 0, 0, 0);
    return QCMAP_NL_FAILURE;
  }
  strlcpy(pktbuf.nl_addr.iface_name, iface_name, sizeof(iface_name));

  if (icmp6->icmp6_type == ND_NEIGHBOR_SOLICIT || icmp6->icmp6_type == ND_NEIGHBOR_ADVERT ||
      icmp6->icmp6_type == ND_ROUTER_SOLICIT)
  {
    memcpy(pktbuf.nl_addr.nd_packet_buf, nd_packet_buf, MAX_ND_PKT_SIZE);
  }

  /* NS or NA */
  if (icmp6->icmp6_type == ND_NEIGHBOR_SOLICIT)
  {
    struct nd_neighbor_solicit *ns;
    ns = (struct nd_neighbor_solicit *) (nd_packet_buf + sizeof(struct ip6_hdr));
    memcpy(pktbuf.nl_addr.ipv6_dst_addr, ns->nd_ns_target.s6_addr, sizeof(struct in6_addr));
    pktbuf.nl_event = QCMAP_NL_NS;
  }
  else if (icmp6->icmp6_type == ND_NEIGHBOR_ADVERT) /* NA */
  {
    memcpy(pktbuf.nl_addr.ipv6_dst_addr,ipv6_packet->ip6_dst.s6_addr, sizeof(struct in6_addr));
    pktbuf.nl_event = QCMAP_NL_NA;
  }
  else if (icmp6->icmp6_type == ND_ROUTER_ADVERT) /* RA */
  {
    memcpy(pktbuf.nl_addr.ipv6_dst_addr,ipv6_packet->ip6_dst.s6_addr, sizeof(struct in6_addr));
    /* Copy the SRC IP from recvd RA*/
    memcpy(pktbuf.nl_addr.ipv6_src_addr,ipv6_packet->ip6_src.s6_addr, sizeof(struct in6_addr));
    pktbuf.nl_event = QCMAP_NL_RA;
  }
  else if (icmp6->icmp6_type == ND_ROUTER_SOLICIT ) /* RS */
  {
    pktbuf.nl_event = QCMAP_NL_RS;

    /* No packet info is needed for RS packets */
    if (QCMAP_NL_FAILURE == qcmap_nl_cm_send(&pktbuf))
    {
      LOG_MSG_ERROR("Send Failed from QCMAP Netlink thread context",
                    0, 0, 0);
    }
    return QCMAP_NL_SUCCESS;
  }

  option_buf =
          (nd_packet_buf + sizeof(struct ip6_hdr) + sizeof(struct nd_router_advert));

  /* Decrement Length and see if we have room for options. */
  pkt_size -= (sizeof(struct ip6_hdr) + sizeof(struct nd_router_advert));


  /* Look for Link Address option. */
  while (pkt_size > 0)
  {
    if ( pkt_size > sizeof(struct nd_opt_hdr) )
    {
      if (icmp6->icmp6_type != ND_NEIGHBOR_SOLICIT && icmp6->icmp6_type != ND_NEIGHBOR_ADVERT)
      {
        curr_option = (struct nd_opt_hdr *)option_buf;
        #define OCTET_SIZE 8
        if ( (curr_option->nd_opt_type != ND_OPT_PREFIX_INFORMATION) ||
             (((curr_option->nd_opt_len)*OCTET_SIZE)  != (sizeof(struct nd_opt_prefix_info))) )
        {
          pkt_size -= (curr_option->nd_opt_len*OCTET_SIZE);
          option_buf += (curr_option->nd_opt_len*OCTET_SIZE);
          continue;
        }

        prefix_info = (struct nd_opt_prefix_info *)curr_option;
        /* Ignore deprecated prefix. */
        #define DEPRECATED_PREFERRED_LIFE_TIME 1
        if ( prefix_info->nd_opt_pi_preferred_time == htonl(DEPRECATED_PREFERRED_LIFE_TIME))
        {
          LOG_MSG_ERROR("RA to deprecate prefix. Ignore!!", 0, 0, 0);
          return QCMAP_NL_FAILURE;
        }

        memcpy(&pktbuf.nl_addr.ip_v6_addr,prefix_info->nd_opt_pi_prefix.s6_addr, sizeof(struct in6_addr));
      }
      else
      {
        /* NS/NA packet we can just copy the src address */
        memcpy(&pktbuf.nl_addr.ip_v6_addr,ipv6_packet->ip6_src.s6_addr, sizeof(struct in6_addr));
      }
      pktbuf.nl_addr.isValidIPv6address = true;

      if (QCMAP_NL_FAILURE == qcmap_nl_cm_send(&pktbuf))
      {
        LOG_MSG_ERROR("Send Failed from QCMAP Netlink thread context",
                      0, 0, 0);
      }
      break;
    }
    else
    {
      break;
    }
  }

  return QCMAP_NL_SUCCESS;
}

/*===========================================================================

FUNCTION QCMAP_NL_SEND_GETNEIGH_EVENT()

DESCRIPTION

  This function
  - Sends a RTM_NEWNEIGH message for bridge iface.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None
==========================================================================*/
int qcmap_nl_send_getneigh_event(void)
{
  ssize_t            sndbytes=0;
  int ifindex=-1;
  struct {
    struct nlmsghdr n;
    struct ndmsg r;
  } req;

  ds_log_med("%s %d \n",__func__,__LINE__);

  if (qcmap_nl_get_interface_details(BRIDGE_IFACE, &ifindex,
                                     QCMAP_GET_IFINDEX) == QCMAP_NL_SUCCESS)
  {
    LOG_MSG_INFO1("Bridge Iface Index = %d",ifindex,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Bridge Iface retrival error Index = %d",ifindex,0,0);
    return QCMAP_NL_FAILURE;
  }

  /* Send the GETNEIGH message to the kernel*/
  memset(&req, 0, sizeof(req));
  req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ndmsg));
  req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT | NLM_F_REQUEST;
  req.n.nlmsg_seq = QCMAP_NL_GETNEIGH_SEQ;
  req.n.nlmsg_pid = QCMAP_NL_SOCK_PORTID;
  req.n.nlmsg_type = RTM_GETNEIGH;
  req.r.ndm_state = NUD_REACHABLE | NUD_STALE | NUD_DELAY;
  req.r.ndm_family = AF_INET6;
  req.r.ndm_ifindex = ifindex;

  ds_log_med(" Dump getneigh len=%d , flags =%d, seq=%d, pid=%d, type=%d, family=%d ifindex %d\n",
               req.n.nlmsg_len, req.n.nlmsg_flags, req.n.nlmsg_seq, req.n.nlmsg_pid, req.n.nlmsg_type, req.r.ndm_family,  req.r.ndm_ifindex);
  sndbytes = send(qcmap_nl_listen_sock_fd, &req, req.n.nlmsg_len, 0);

  if (sndbytes < 0)
  {
    ds_log_med("Send GETNEIGH failed: %s \n",strerror(errno));
    return QCMAP_NL_FAILURE;
  }
  else
  {
    ds_log_med("Send GETNEIGH succeded: %d bytes send \n",sndbytes);
  }

  return QCMAP_NL_SUCCESS;
}

/*===========================================================================

FUNCTION Getclientaddr

DESCRIPTION

  This function
  - Uses mac address to find out the public ip address of the connect device.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
==========================================================================*/

void Getclientaddr(qcmap_nl_addr_t* nl_addr)
{
  char command[MAX_COMMAND_STR_LEN];
  FILE *fp = NULL;
  int read;
  char *ptr = NULL;
  char *line = NULL;
  size_t len = 0;
  int count =0;
  unsigned char dst[sizeof(struct in6_addr)];
  uint8_t  ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  struct ps_in6_addr *nl_prefix_ptr;
  char mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];

  ds_mac_addr_ntop(nl_addr->mac_addr, mac_addr_char);

  snprintf(command, MAX_COMMAND_STR_LEN,"ip neighbour show |grep %s > %s",
      mac_addr_char,IP_NEIGH_FILE);
  ds_system_call(command, strlen(command));
  /* open the file */
  fp = fopen(IP_NEIGH_FILE, "r");
  if (fp == NULL) {
    LOG_MSG_ERROR("Couldn't open NEIGH file for reading %d", errno, 0, 0);
    return;
  }

  while (((read = getline(&line, &len, fp)) != -1))
  {
    ptr=line;
    while ( *ptr != ' ' )
    {
      ptr++;
    }
    *ptr='\0';

    LOG_MSG_INFO1("Ip address = %s ",line,0,0);

    if(!inet_pton(AF_INET,line,&nl_addr->ip_addr))
    {
      LOG_MSG_ERROR("\n Not a valid IPV4 addr \n",0,0,0);
    }
    else
    {
      if (!QCMAP_IFA_IS_LINK_LOCAL_IP((nl_addr->ip_addr)))
      {
        LOG_MSG_INFO1("Valid IPv4 address",0,0,0);
        nl_addr->isValidIPv4address=true;
        continue;
      }
    }

    if(!inet_pton(AF_INET6, line,&ip_v6_addr))
    {
      LOG_MSG_INFO1("Not a valid IPV6 addr ",0,0,0);
    }
    else
    {
      nl_prefix_ptr = (struct ps_in6_addr*)(ip_v6_addr);
      if (QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32))
      {
        memcpy( nl_addr->ip_v6_addr, ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        LOG_MSG_INFO1("Valid IPv6 Link local address ",0,0,0);
        nl_addr->isValidIPv6address=true;
        continue;
      }
    }
  }
  snprintf(command, MAX_COMMAND_STR_LEN,"rm %s",IP_NEIGH_FILE);
  ds_system_call(command, strlen(command));

  if (line)
    free(line);

  if(fp)
    fclose(fp);
}

