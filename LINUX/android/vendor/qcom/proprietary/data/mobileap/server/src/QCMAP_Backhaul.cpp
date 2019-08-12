/*====================================================

FILE:  QCMAP_Backhaul.cpp

SERVICES:
   QCMAP Connection Manager Backhaul Specific Implementation

=====================================================

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created
  12/06/15   rk         Offtarget support.

======================================================*/
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <linux/netlink.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/ipv6_route.h>
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include "qcmap_cm_api.h"
#include "ds_string.h"
#include "ds_util.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_LAN.h"

// Define some constants.
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length
#define ICMP_HDRLEN 8  // ICMP header length for echo request, excludes data
#define ETH_P_IPV6 0x86DD
#define MAX_BACKAUL_COUNT 4
extern struct sigevent sev;
qcmap_backhaul_type QCMAP_Backhaul::current_backhaul = NO_BACKHAUL;
qcmap_backhaul_type QCMAP_Backhaul::preffered_backhaul_first;
qcmap_backhaul_type QCMAP_Backhaul::preffered_backhaul_second;
qcmap_backhaul_type QCMAP_Backhaul::preffered_backhaul_third;
qcmap_backhaul_type QCMAP_Backhaul::preffered_backhaul_fourth;

struct icmp6_ra_pkt {
  struct nd_router_advert router_adv;
  struct nd_opt_prefix_info prefix_info;
  struct nd_opt_mtu mtu_info;
}__attribute__((__packed__));

/* Request message to send to the kernel to retrieve prefix info. */
typedef struct qcmap_nl_getaddr_req_s
{
  struct nlmsghdr   nlh;
  struct ifaddrmsg  ifa;
} qcmap_nl_getaddr_req_t;

typedef struct qcmap_backhaul_list_s
{
  qcmap_backhaul_type backhaul_name;
  boolean             backhaul_availale;
} qcmap_backhaul_list_type;

#define PPP_CLIENT_MAC_ADDR "ff:ff:ff:ff:ff:ff"

/* Set Default Valid and Preferred Life Time Values. */
#define IPV6_DEFAULT_PREFERED_LIFETIME 2700;
#define IPV6_DEFAULT_VALID_LIFETIME 7200;
#define IPV6_MIN_PREFIX_LENGTH 64
#define IPV6_MAX_PREFIX_LENGTH 128
#define IPTYPE_IPV6_ICMP        58

#define DEFAULT_VALID_LIFETIME 2
#define DEFAULT_PREFERRED_LIFETIME 1
#define CARRY16(x) (((x) & 0x10000) ? 1 : 0)
/* Use this only when the containing type can't overflow due to the carry-overs */
#define CARRYS16(x) (((x) & (~0xffff)) >> 16)
#define IPTYPE_IPV6_ICMP        58
static uint16_t ones_comp_sum(uint16_t *ptr, size_t size)
{
  uint32_t ret = 0;
  size_t i;
  uint16_t *cur;
  for (cur = ptr, i = 0; i < size; i++) {
    ret += ntohs(cur[i]);
    ret = (ret + CARRY16(ret)) & 0xffff;
  }
  return ret;
}

uint16_t icmp6_checksum(struct ip6_hdr *ip6h, struct icmp6_header *icmp6h, uint16_t icmplen)
{
  uint32_t ret = 0;
  struct ip6_hdr tmph;

  tmph = *ip6h;
  tmph.ip6_nxt = IPTYPE_IPV6_ICMP;
  tmph.ip6_plen = icmplen;

  /* Checksum computation must contain a pseudo-header, comprised of:
   * - src and dst ip addresses
   * - payload length (32 bit value)
   * - next header
   */
  ret = ones_comp_sum((uint16_t *)&ip6h->ip6_src, sizeof(struct in6_addr)/sizeof(uint16_t));
  ret += ones_comp_sum((uint16_t *)&ip6h->ip6_dst, sizeof(struct in6_addr)/sizeof(uint16_t));
  ret += ntohs(ip6h->ip6_plen);

  if (icmplen != ntohs(ip6h->ip6_plen))
    LOG_MSG_INFO1("payload length %d does not match with icmplen %d", ip6h->ip6_plen, icmplen, 0);

  ret += (uint16_t) IPTYPE_IPV6_ICMP;
  ret += ones_comp_sum((uint16_t *)icmp6h, icmplen / sizeof(uint16_t));
  ret += CARRYS16(ret);

  return htons(~ret & 0xffff);
}


bool QCMAP_Backhaul::flag= false;
QCMAP_Backhaul* QCMAP_Backhaul::object=NULL;

qcmap_backhaul_type DetermineBackhaulType(char* token)
{
  qcmap_backhaul_type return_backhaul = NO_BACKHAUL;
  /*------------------------------------------------------------------------*/
  if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_WWAN;
  else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_CRADLE;
  else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_AP_STA_ROUTER;
  else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_ETHERNET;
  else
    LOG_MSG_INFO1("Unknown preffered_backhaul_first read from xml!!",0,0,0);

  return return_backhaul;
}

QCMAP_Backhaul* QCMAP_Backhaul::Get_Instance(boolean obj_create)
{
  char data[MAX_STRING_LENGTH] = {0};
  char *token=NULL;
  char *ptr;
  if(!flag && obj_create)
  {
//Create object for Firewall and NATALG here
    QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(true);
    QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(true);
    LOG_MSG_INFO1("Creating object : BACKHAUL",0,0,0);
    object = new QCMAP_Backhaul();
    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FIRST_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
    token = strtok_r(data, "\n",&ptr);
    if(token)
    {
      printf(" CONFIG_FIRST_PREFFERED_BACKHAUL : t%st\n",token);
      QCMAP_Backhaul::preffered_backhaul_first = DetermineBackhaulType(token);
    }
    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_SECOND_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
    token = strtok_r(data, "\n",&ptr);
    if(token)
    {
      printf(" CONFIG_SECOND_PREFFERED_BACKHAUL : t%st\n",token);
      QCMAP_Backhaul::preffered_backhaul_second = DetermineBackhaulType(token);
    }
    if (QCMAP_Backhaul::preffered_backhaul_second == QCMAP_Backhaul::preffered_backhaul_first)
      QCMAP_Backhaul::preffered_backhaul_second = NO_BACKHAUL;

    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_THIRD_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
    token = strtok_r(data, "\n",&ptr);
    if(token)
    {
      printf(" CONFIG_THIRD_PREFFERED_BACKHAUL : t%st\n",token);
      QCMAP_Backhaul::preffered_backhaul_third = DetermineBackhaulType(token);
    }
    if (QCMAP_Backhaul::preffered_backhaul_third == QCMAP_Backhaul::preffered_backhaul_first ||
        QCMAP_Backhaul::preffered_backhaul_third == QCMAP_Backhaul::preffered_backhaul_second)
      QCMAP_Backhaul::preffered_backhaul_third = NO_BACKHAUL;

    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FOURTH_PREFFERED_BACKHAUL,
                                                GET_VALUE,
                                                data, MAX_STRING_LENGTH);
    token = strtok_r(data, "\n",&ptr);
    if(token)
    {
      printf(" CONFIG_FOURTH_PREFFERED_BACKHAUL : t%st\n",token);

      QCMAP_Backhaul::preffered_backhaul_fourth = DetermineBackhaulType(token);
    }
    if (QCMAP_Backhaul::preffered_backhaul_fourth ==
                                   QCMAP_Backhaul::preffered_backhaul_first ||
        QCMAP_Backhaul::preffered_backhaul_fourth ==
                                   QCMAP_Backhaul::preffered_backhaul_second ||
        QCMAP_Backhaul::preffered_backhaul_fourth ==
                                    QCMAP_Backhaul::preffered_backhaul_third
        )
      QCMAP_Backhaul::preffered_backhaul_fourth = NO_BACKHAUL;

    LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_first %d: \n",QCMAP_Backhaul::preffered_backhaul_first,0,0);
    LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_second %d: \n",QCMAP_Backhaul::preffered_backhaul_second,0,0);
    LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_third %d: \n",QCMAP_Backhaul::preffered_backhaul_third,0,0);
    LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_fourth %d: \n",
                              QCMAP_Backhaul::preffered_backhaul_fourth,0,0);

    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*===================================================================
                             Class Definitions
  ===================================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Backhaul variables.

@parameters
none

@return
  true  - on success
  false - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/


QCMAP_Backhaul::QCMAP_Backhaul()
{
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  // Intialize IPv6 Prefix to Null
  memset(&this->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
  /* Intialize ipv4 address info. */
  this->ipv4_public_ip = 0;
  this->ipv4_default_gw_addr = 0;
  if (QcMapMgr->xml_path)
  {
   if (this->ReadBackhaulConfigFromXML())
   {
     strlcpy(this->wan_cfg.eri_config_file,QCMAP_ERI_CONFIG,(QCMAP_CM_MAX_FILE_LEN));
   }
   else
   {
     /* Read Config from XML failed. Use defaults. */
     LOG_MSG_INFO1("QCMAP Read XML failed.\n",0,0,0);
   }

  }
  /*initialize current backhaul*/
  this->current_backhaul = NO_BACKHAUL;

  memset(&this->external_client_addr_list, NULL, sizeof(qcmap_addr_info_list_t));

  return;
}

QCMAP_Backhaul::~QCMAP_Backhaul()
{
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  /* Delete NAT & Firewall Objects. */
  if (QcMapFirewall != NULL)
    delete QcMapFirewall;
  if (QcMapNatAlg != NULL)
    delete QcMapNatAlg;

  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: BACKHAUL",0,0,0);
}


/*===========================================================================
  FUNCTION IsCradleOrStationOrEthActive
==========================================================================*/
/*!
@brief
  Checks whether Cradle or WLAN Backhaul is active, if not returns true

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::IsNonWWANBackhaulActive()
{
  boolean cradle_wlan_eth_status = false;
  QCMAP_Backhaul* QcMapBackhaulMgr = QCMAP_Backhaul::Get_Instance(false);
  if(QcMapBackhaulMgr)
  {
    cradle_wlan_eth_status = (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE ||
                          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER ||
                          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE ||
                          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET);
  }
  return cradle_wlan_eth_status;
}
/*===========================================================================
  FUNCTION RemoveIPV6Address
==========================================================================*/
/*!
@brief
  Removes IPV6 address from the interface provided

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::RemoveIPV6Address(char *devname)
{
  char command[MAX_COMMAND_STR_LEN];
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  QCMAP_Backhaul* QcMapBackhaulMgr = QCMAP_Backhaul::Get_Instance(false);

  if (QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid)
  {
    LOG_MSG_ERROR("invalid Prefix Passed\n", 0, 0, 0);
    return false;
  }

  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);
  inet_ntop(AF_INET6,
            ((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr,
            ip6_addr, MAX_IPV6_PREFIX );
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip -6 addr del %s/%d dev %s",
           ip6_addr,QcMapBackhaulMgr->ipv6_prefix_info.prefix_len,
           devname);
  ds_system_call( command, strlen(command));

  memset(&(QcMapBackhaulMgr->ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));

  return true;
}


/*===========================================================================
  FUNCTION EnableIPV4
==========================================================================*/
/*!
@brief
  Enables IPV4 backhaul Functionality. If autoconnect is enabled,
  triggers the backhaul to get the IPv4 address.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::EnableIPV4(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0;
  struct timespec ts;
  boolean retval = true;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="1";//To update xml in case the backhaul object is not enabled
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                    QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();
  /* Cant continue if MobileAP is not enabled! */

  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }



  if(QcMapBackhaulMgr)
  {
    if (QcMapBackhaulMgr->enable_ipv4)
    {
      /* QCMAP IPV4 is already enabled */
      LOG_MSG_ERROR("QCMAP IPV4 already enabled\n",0,0,0);
      return true;
    }

    /* Enable IPV4 so that backhaul will succeed. */
    QcMapBackhaulMgr->enable_ipv4 = true;
  }
  /* Save the Configuration. */
  GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,SET_VALUE,enable,MAX_STRING_LENGTH);
  /* Check for auto-connect and bring-up backhaul. */
  ret = qcmap_cm_enable_ipv4(QcMapMgr->qcmap_cm_handle, qmi_err_num);
  if ( ret < 0 )
  {
    LOG_MSG_ERROR("QCMAP IPV4 enable failed: %d\n", qmi_err_num, 0, 0);
    /* Restore the state. */
    if (QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->enable_ipv4 = false;
    }
    retval = false;
  }

  if ( retval == true)
  {
    if ((NULL != QcMapBackhaulCradleMgr) &&
        (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
         QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
    {
      if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
      {
        /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for cradle iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client for cradle iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
      else
      {
        /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for cradle iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ECM_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ECM_IFACE);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client for cradle iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
      QcMapBackhaulCradleMgr->cradle_v4_available = false;
    }
    if (NULL != QcMapBackhaulEthMgr &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                   QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
    {
      /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for ETH iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ETH_IFACE);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client for ETH iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",ETH_IFACE);
        ds_system_call(command, strlen(command));
        QcMapBackhaulEthMgr->eth_v4_available = false;
    }
    if (NULL != QcMapBackhaulWLANMgr)
    {
      if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
      {
        /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for STA Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
                  BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",
                  BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client on STA Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",
                  BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
      else
      {
        /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for STA Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
                  QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",
                  QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client on STA Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",
                  QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
        ds_system_call(command, strlen(command));
      }
      QcMapBackhaulWLANMgr->sta_v4_available = false;
    }

    if(!QcMapNatAlg)
      LOG_MSG_ERROR("NAT object not present",0,0,0);

    LOG_MSG_INFO1("QCMAP IPV4 Enabled\n",0,0,0);
  }
  return retval;
}
/*===========================================================================
  FUNCTION DisableIPV4
==========================================================================*/
/*!
@brief
  Disables IPv4 Functionality.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul::DisableIPV4(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0, qcmap_cm_errno = QCMAP_CM_ENOERROR;
  char command[MAX_COMMAND_STR_LEN];
  boolean retval = true;
  char enable[MAX_STRING_LENGTH]="0";//To update xml in case the backhaul object is not enabled
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                             QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }


  if(QcMapBackhaulMgr)
  {
    if (!QcMapBackhaulMgr->enable_ipv4)
    {
      /* QCMAP IPV4 is not enabled */
      LOG_MSG_ERROR("QCMAP IPV4 not enabled\n",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }

      /* The logic here is to disconnect the existing V4 backhaul and set the
       * enable_ipv4 flag to false. This is required even when we are in STA mode.
       * Reason being if we don't bring down the call now and when we switch from
       * WLAN backhaul goes down we will still have IPV4 WWAN backhaul which should
       * not be the case.
       */

     /* Disconnect Backhaul. */
     ret = qcmap_cm_disable_ipv4(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, qmi_err_num);

     if (ret == QCMAP_CM_SUCCESS ||
         (ret == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK))
     {
       /* we just disable the IPV4 forwarding. So that none of the clients
        * will be able to access IPV4 backhaul. This is needed for both
        * station mode and regular mode as when WWAN backhaul cb is
        * hit the enable_ipv4 will not be set and so below functionality
        * will not be executed.
        */
       if(QcMapNatAlg)
         QcMapNatAlg->DisableNATonApps();
       else
         LOG_MSG_ERROR("NAT object not present",0,0,0);

       if(QcMapFirewall)
         QcMapFirewall->CleanIPv4MangleTable();
       else
         LOG_MSG_ERROR("FIREWALL object not present",0,0,0);

      if ((NULL != QcMapBackhaulCradleMgr) &&
          (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
           QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
      {
        if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
        {
          /* Kill dhcpcd intance for Cradle Iface*/
          LOG_MSG_INFO1("Killing DHCP Client for Cradle Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Kill dhcpcd intance for Cradle Iface*/
          LOG_MSG_INFO1("Killing DHCP Client for Cradle Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ECM_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ECM_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", ECM_IFACE);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(ECM_IFACE);
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        QcMapBackhaulCradleMgr->cradle_v4_available = false;
      }
      if (NULL != QcMapBackhaulEthMgr &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                   QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
      {
        LOG_MSG_INFO1("Killing DHCP Client for Ethernet Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", ETH_IFACE);
        ds_system_call(command, strlen(command));

        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(ETH_IFACE);
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        QcMapBackhaulEthMgr->eth_v4_available = false;
      }
      if (NULL != QcMapBackhaulWLANMgr)
      {
        if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
        {
          LOG_MSG_INFO1("Killing DHCP Client for Bridge Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Kill any stale dhcpcd intance */
          LOG_MSG_INFO1("Killing DHCP Client for STA Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up",
                    QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
        QcMapBackhaulWLANMgr->sta_v4_available = false;
      }
       QcMapBackhaulMgr->enable_ipv4 = false;
     }


     else
     {
       LOG_MSG_INFO1( "QCMAP IPV4 Disable Fail %d: %d\n",
                     qcmap_cm_errno, qmi_err_num, 0);
       return false;
     }
   }

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false &&
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
  {
    switch (QcMapBackhaulMgr->current_backhaul)
    {
      case BACKHAUL_TYPE_AP_STA_ROUTER:
        if (!QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QcMapBackhaulMgr->current_backhaul, false, true);
        }
      break;
      case BACKHAUL_TYPE_CRADLE:
        if (!QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QcMapBackhaulMgr->current_backhaul, false, true);
        }
      break;
      case BACKHAUL_TYPE_WWAN:
        if (qcmap_cm_get_ipv6_state(qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED)
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QcMapBackhaulMgr->current_backhaul, false, true);
        }
      break;
      case BACKHAUL_TYPE_ETHERNET:
       if (!QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QcMapBackhaulMgr->current_backhaul, false, true);
        }
      break;
      default:
        LOG_MSG_ERROR("Invalid Backhaul", 0, 0, 0);
      break;
    }
  }
   LOG_MSG_INFO1("QCMAP IPV4 Disabled\n",0,0,0);
   /* Save the Configuration. */
   GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,SET_VALUE,enable,strlen(enable));
   return true;
 }

/*===========================================================================
  FUNCTION GetIPV6PrefixInfo
==========================================================================*/
/*!
@brief
  Gets IP address and netmask assigned to the STA interface.

@parameters
  char *devname
  qcmap_cm_nl_prefix_info_t   *ipv6_prefix_info


@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void QCMAP_Backhaul::GetIPV6PrefixInfo(char *devname,
                                                qcmap_cm_nl_prefix_info_t   *ipv6_prefix_info)
{
  struct sockaddr_nl  src_addr;
  struct sockaddr_nl dest_addr;
  struct iovec iov;
  struct sockaddr_nl sa;
  struct msghdr msg = { (void *)&sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_in6 *sin6 = NULL;
  void *buf = NULL;
  qcmap_nl_getaddr_req_t req;
  int ret = -1;
  int sock_fd;
  int iface_idx;
  unsigned int buflen = 0;
  ssize_t recvsize;
  struct ifreq ifr;
  int socket_mtu;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);


  if ((sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: socket() failed: %d\n", errno, 0, 0);
    return -1;
  }

  /* Initialize the source address */
  memset(&src_addr, 0, sizeof(src_addr));

  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = 0;
  src_addr.nl_groups = 0; /* Interested in unicast messages */

  /* Bind the socket to our source address */
  if (bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: bind() failed: %d\n", errno, 0, 0);
    goto bail;
  }

  /* Initialize destination address structure */
  memset(&dest_addr, 0, sizeof(dest_addr));

  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid    = 0;  /* Addressed to kernel */
  dest_addr.nl_groups = 0;  /* This is a unicast message */

  /* Initialize the request message */
  memset(&req, 0, sizeof(req));

  /* Fill the netlink request message */
  req.nlh.nlmsg_len   = sizeof(req);
  req.nlh.nlmsg_pid   = 0;
  req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
  req.nlh.nlmsg_type  = RTM_GETADDR;

  /* Set the ip family and interface index for which the addresses are requested */
  req.ifa.ifa_family = AF_INET6;
  iface_idx = QcMapMgr->IsInterfaceEnabled(devname);
  if (iface_idx <= 0)
  {
    LOG_MSG_ERROR("Couldn't find interface %s Error:%d", devname, errno, 0);
    goto bail;
  }

  LOG_MSG_INFO1("Get Prefix Info for interface %s with ID: %d", devname, iface_idx, 0);

  if ( sendto(sock_fd, (void*) &req, sizeof(req), 0,
               (struct sockaddr*) &dest_addr, sizeof(dest_addr)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: bind() failed: %d\n", errno, 0, 0);
    goto bail;
  }

/* Max size of the netlink response message */
#define QCMAP_NL_MAX_MSG_SIZE  (1024 * 4)
  /* Allocate and initialize buffer to read message */
  buf = calloc(1, NLMSG_SPACE(QCMAP_NL_MAX_MSG_SIZE));
  if (NULL == buf)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: memory alloc failure: %d\n", errno, 0, 0);
    goto bail;
  }

  iov.iov_base = buf;
  iov.iov_len  = NLMSG_SPACE(QCMAP_NL_MAX_MSG_SIZE);

  /* Read message from kernel */
  if ((recvsize = recvmsg(sock_fd, &msg, 0)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: memory alloc failure: %d\n", errno, 0, 0);
    goto bail;
  }
  else
  {
    buflen = recvsize;
    LOG_MSG_INFO3("received response from kernel size=%d\n", buflen, 0, 0);
  }

  nlh = (struct nlmsghdr *)buf;

  /* Parse the message one header at a time */
  while (NLMSG_OK(nlh, buflen))
  {
    struct ifaddrmsg *ifa;
    struct rtattr *rta;
    int rtattrlen;
    struct ifreq ifr;

    ifa = (struct ifaddrmsg *)NLMSG_DATA(nlh);
    rta = (struct rtattr *)IFA_RTA(ifa);

    /* Make sure that the requested and received address family is the same */
    if ( AF_INET6 != ifa->ifa_family || RT_SCOPE_UNIVERSE != ifa->ifa_scope ||
         ifa->ifa_index != iface_idx)
    {
      LOG_MSG_ERROR("GetIPV6PrefixInfo: ip family %d, Scope %d or Index %d don't match\n",
                    ifa->ifa_family, ifa->ifa_scope, ifa->ifa_index);
      /* Advance to next header */
      nlh = NLMSG_NEXT(nlh, buflen);
      continue;
    }

    rtattrlen = IFA_PAYLOAD(nlh);

    /* Parse the RTM_GETADDR attributes */
    while (RTA_OK(rta, rtattrlen))
    {
      switch (rta->rta_type)
      {
        case IFA_ADDRESS:
           ipv6_prefix_info->prefix_len = ifa->ifa_prefixlen;
           sin6 = (struct sockaddr_in6 *)&ipv6_prefix_info->prefix_addr;
           memcpy(SASTORAGE_DATA(ipv6_prefix_info->prefix_addr),
                  RTA_DATA(rta),
                  sizeof(sin6->sin6_addr));
           ipv6_prefix_info->prefix_info_valid = true;
           break;
        case IFA_CACHEINFO:
           memcpy( &ipv6_prefix_info->cache_info,
                   RTA_DATA(rta),
                   sizeof(ipv6_prefix_info->cache_info) );
           LOG_MSG_INFO2( "GetIPV6PrefixInfo: Address Cache Info - prefered=%d valid=%d\n",
                           ipv6_prefix_info->cache_info.ifa_prefered,
                           ipv6_prefix_info->cache_info.ifa_valid, 0);
           break;
        default:
           LOG_MSG_INFO3("GetIPV6PrefixInfo: rta_type=%x\n", rta->rta_type, 0, 0);
           break;
      }
      rta = RTA_NEXT(rta, rtattrlen);
    }

    /* Break out from here as by this point we would have found the address. */
    break;
  }

  memset(&ifr, 0, sizeof(ifr));
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), devname);

  /* get mtu from device for ipv6 */
  if((socket_mtu = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Couldn't create socket to get mtu of ipv6 wwan err %d\n", errno,0,0);
    goto bail;
  }

  ifr.ifr_addr.sa_family = AF_INET6;

  if (ioctl(socket_mtu, SIOCGIFMTU, &ifr) < 0)
  {
    LOG_MSG_ERROR("Couldn't get mtu from ipv6 wwan iface. err %d\n", errno,0,0);
    goto bail;
  }

  ipv6_prefix_info->mtu = ifr.ifr_mtu;
  LOG_MSG_INFO1( "GetIPV6PrefixInfo: MTU = %d\n", ipv6_prefix_info->mtu,0,0);

bail:
  close(sock_fd);
  if (socket_mtu)
    close(socket_mtu);

  if ( buf != NULL )
    free(buf);

  return;


}


/*===========================================================================
  FUNCTION UpdateGlobalV6addr
==========================================================================*/
/*!
@brief
  Handles the RA packet

@parameters
  uint8_t ipv6_addr[]

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::UpdateGlobalV6addr
(
  qcmap_nl_addr_t* nl_addr
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);

  struct ps_in6_addr *dst_addr_ptr = NULL, *iid_ptr = NULL;
  struct ps_in6_addr *global_addr_ptr = NULL, *prefix_addr_ptr = NULL;
  struct ps_in6_addr * curr_prefix_info = NULL;
  qcmap_nl_addr_t qcmap_nl_buffer;
  qmi_error_type_v01 qmi_err_num;
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t *clientInfo = NULL;
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  uint8 ipv6PrefixLen = 0;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  qcmap_nl_sock_msg_t pktbuf;
  char ipv6addr[INET6_ADDRSTRLEN];
  qcmap_cm_nl_prefix_info_t  ipv6_prefix_info;
  int ret;

  LOG_MSG_INFO1("Entering UpdateGlobalV6addr\n", 0, 0, 0);
  if(!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("backhaul not UP\n", 0, 0, 0);
    return;
  }
  else  if(QcMapBackhaulMgr->enable_ipv6)
  {
    ret = QCMAP_ConnectionManager::GetDeviceName\
                          (devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);

    if( ret==BACKHAUL_TYPE_WWAN &&
        !QCMAP_Backhaul_WLAN::IsSTAAvailableV6() &&
        !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() &&
        !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() &&
        qcmap_cm_get_ipv6_state(&qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED )
    {
      /* QCMAP IPV6 backhaul not up */
      LOG_MSG_ERROR("QCMAP IPV6 backhaul not UP\n", 0, 0, 0);
      return;
    }

    dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ipv6_dst_addr;
    prefix_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;

    if (QCMAP_IN6_IS_PREFIX_LINKLOCAL(dst_addr_ptr->ps_s6_addr32))
    {
      LOG_MSG_INFO1("Destination based RA address received\n ", 0, 0, 0);
      memset(ipv6addr, 0, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6,
                  (void *)dst_addr_ptr->ps_s6_addr,
                  ipv6addr, INET6_ADDRSTRLEN);
      ds_log_med("UpdateGlobalV6addr- Destination V6 Address %s \n",ipv6addr);
      if ( addrList->addrListHead == NULL)
      {
        LOG_MSG_ERROR("UpdateGlobalV6addr() - Linked list head is NULL \n",
                      0, 0, 0);
        return ;
      }

      tempPrefixIidptr.iid_ptr = dst_addr_ptr;
      node = ds_dll_search (addrList->addrListHead , (void*)&tempPrefixIidptr,
                            QcMapMgr->qcmap_match_v6_iid);

      if ( node == NULL)
      {
        LOG_MSG_ERROR("UpdateGlobalV6addr() - No IID Match found. Check why??",\
                      0,0,0);
        return;
      }
      else
      {
        clientInfo = ( qcmap_cm_client_data_info_t* )ds_dll_data(node);
        if (clientInfo == NULL)
        {
          LOG_MSG_ERROR("UpdateGlobalV6addr - Error in fetching node data\n ",
                        0, 0, 0);
          return;
        }
        else
        {
          /* IID match is found Update Address the global v6 address*/
          iid_ptr = (struct ps_in6_addr *)clientInfo->link_local_v6_addr;
          global_addr_ptr = (struct ps_in6_addr *)clientInfo->ip_v6_addr;

          /* Update only if the prefix changes. */
          if ( !memcmp(&global_addr_ptr->ps_s6_addr64[0],
               &prefix_addr_ptr->ps_s6_addr64[0],sizeof(uint64)))
          {
            LOG_MSG_INFO1("UpdateGlobalV6addr - Prefix unchanged no need to \
                          update\n ",0, 0, 0);
          }
          else
          {
            global_addr_ptr->ps_s6_addr64[0] = prefix_addr_ptr->ps_s6_addr64[0];
            global_addr_ptr->ps_s6_addr64[1] = iid_ptr->ps_s6_addr64[1];

            if (QcMapTetheringMgr && ((clientInfo->device_type ==
                 QCMAP_MSGR_DEVICE_TYPE_USB_V01) &&
                (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
                 QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                 QCMAP_QTI_TETHERED_LINK_PPP)))
            {
              memcpy( QcMapTetheringMgr->ppp_ipv6_addr,
                      clientInfo->ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01 );
              QcMapBackhaulMgr->AddPPPIPv6Route();
            }
          }
        }
      }
    }
    else
    {
      if(!QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid)
      {
          if((NULL != QcMapBackhaulWLANMgr) ||
             (NULL != QcMapBackhaulCradleMgr) ||
             (NULL != QcMapBackhaulWWANMgr))
        {
          if (ret==BACKHAUL_TYPE_AP_STA_BRIDGE )
          {
            strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
          }
          else
          {
            if(!ret)
            {
              LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
              return false;
            }
          }
        }

        /* we only want to update the global prefix info if we are not in bridge mode,
           otherwise it will be updated in addrAssignment. Default rules will not be
           installed unless the prefix is updated in new address assignment */
        if (!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() &&
            !QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
        {
          memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = \
                                                 IPV6_DEFAULT_PREFERED_LIFETIME;
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
          QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&QcMapBackhaulMgr->ipv6_prefix_info);
        }
      }

      /* Check if the RA received on current backhaul interface. If not ignore the RA. */
      if ( !QcMapBackhaulMgr->ValidateGlobalV6addr(prefix_addr_ptr))
      {
        LOG_MSG_ERROR("Prefix not on the current backhaul. Ignore RA.\n",
                          0, 0, 0);
        return false;
      }

      LOG_MSG_INFO1(" Multicast RA received / Prefix update on EnableIpv6 forwarding",
                    0, 0, 0);
      if (!QcMapBackhaulMgr->AssignGlobalV6AddrAllClients(prefix_addr_ptr))
      {
        LOG_MSG_ERROR("Could not update Global Ipv6 address for clients\n",
                       0, 0, 0);
        return;
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("UpdateGlobalV6addr - Ipv6 is not enabled\n", 0, 0, 0);
  }
}
/*===========================================================================
  FUNCTION GetNetworkConfig
==========================================================================*/
/*!
@brief
  Gets the network configured value from WWAN or external hotspot.

@parameters
  in_addr_t  *public_ip,
  uint32     *primary_dns,
  in_addr_t  *secondary_dns,
  int        *err_num

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetNetworkConfig
(
  in_addr_t *public_ip,
  uint32 *primary_dns,
  in_addr_t *secondary_dns,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qmi_error;
  uint32 default_gw = 0;
  in_addr_t netmask = 0;
  FILE *file_ptr=NULL;
  char temp_str[16]="";
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char char_match[] = "nameserver "; //search for this pattern in file
  boolean pri_dns_found = false; // seperates out primary DNS and secondary DNS
  int ret;
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);

  ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num);
  if(!QcMapBackhaulMgr)
  {
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
     return false;
  }
  if (!QcMapBackhaulMgr->enable_ipv4)
  {
    /* QCMAP IPV4 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV4 not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    return false;
  }

  if(ret!=BACKHAUL_TYPE_WWAN)
  {
    if ( !QcMapBackhaulMgr->GetIP(public_ip, &netmask, devname))
    {
      LOG_MSG_ERROR("Unable to get the IP address",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    file_ptr = fopen("/etc/resolv.conf","r");
    if(file_ptr == NULL)
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      LOG_MSG_ERROR("Error in getting IPv4 net config %d\n", *qmi_err_num,0,0);
      return false;
    }
    while((fgets(temp_str,strlen(char_match)+1,file_ptr)!=NULL))
    {
      if(strncmp(temp_str,char_match,strlen(char_match))==0)
      {
        fscanf(file_ptr, "%s", temp_str);
        if(!pri_dns_found)
        {
          inet_aton(temp_str, (in_addr *)primary_dns);
          pri_dns_found = TRUE;
        }
        else
        {
          inet_aton(temp_str, (in_addr *)secondary_dns);
          break; // found sec dns also - no more looping
        }
      }
    }
    fclose(file_ptr);

  }

  else
  {
    if ( qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED )
    {
      /* The WWAN is not connected, so it doesn't have an IP or DNS serers*/
      *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
      return false;
    }

    qmi_error = qcmap_cm_get_ipv4_net_conf(QcMapMgr->qcmap_cm_handle,public_ip,
                                           primary_dns,secondary_dns,&default_gw,
                                           qmi_err_num);

    if ( qmi_error == QCMAP_CM_SUCCESS )
    {
      return true;
    }
    }

    if( (*public_ip == 0) && (*primary_dns == 0) && (*secondary_dns == 0))
    {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("Error in getting IPv4 net config %d\n", *qmi_err_num,0,0);
    return false;
  }

  return true;
}
/*===========================================================================
  FUNCTION GetIPv6NetworkConfig
==========================================================================*/
/*!
@brief
  Gets the IPv6 network configured value.

@parameters
  uint8_t public_ip[]
  uint8_t primary_dns[]
  uint8_t secondary_dns[]
  qmi_error_type_v01 *qmi_err_num

@return
  true	- on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetIPv6NetworkConfig
(
  uint8_t            public_ip[],
  uint8_t            primary_dns[],
  uint8_t            secondary_dns[],
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret;
  int qmi_error;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if(!QcMapBackhaulMgr)
  {
     *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
     return false;
  }

  if (!QcMapBackhaulMgr->enable_ipv6)
  {
    /* QCMAP IPV6 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV6 not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    return false;
  }
  ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V6_V01, qmi_err_num);

  if( ret!=BACKHAUL_TYPE_WWAN)
  {
    memset(&(QcMapBackhaulMgr->ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
           QcMapBackhaulMgr->GetIPV6PrefixInfo(devname, &QcMapBackhaulMgr->ipv6_prefix_info);

    if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid == true )
    {
      memcpy((in6_addr *)public_ip,
     ((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))\
      ->sin6_addr.s6_addr,sizeof(in6_addr));
    }
    else
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      LOG_MSG_ERROR("Error in getting v6 station "
                     "mode config %d\n", *qmi_err_num,0,0);
      return false;
    }
  }

  else
  {
    if ( qcmap_cm_get_ipv6_state(qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED )
    {
      /* The WWAN is not connected, so no IPv6 prefix or DNS servers */
      *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
      return false;
    }

  qmi_error = qcmap_cm_get_ipv6_net_conf(QcMapMgr->qcmap_cm_handle,
                                         public_ip, primary_dns,
                                         secondary_dns,
                                         qmi_err_num);

    if ( qmi_error == QCMAP_CM_SUCCESS )
    {
      return true;
    }
  }

  if(PS_IN6_IS_ADDR_UNSPECIFIED(public_ip) &&
   PS_IN6_IS_ADDR_UNSPECIFIED(primary_dns) &&
   PS_IN6_IS_ADDR_UNSPECIFIED(secondary_dns))
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("Error in getting IPv6 WWAN config %d\n", *qmi_err_num,0,0);
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetWebserverWWANAccess
==========================================================================*/
/*!
@brief
  Configures Webserver WWAN Access flag and updates in the mobileap
  configuration file.

@parameters
  boolean enable

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SetWebserverWWANAccess
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  char data[MAX_STRING_LENGTH] = {0};

  snprintf(data, sizeof(data), "%d", enable);
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_WEBSERVER_WWAN_ACCESS,SET_VALUE,
                                              data,strlen(data));
  if(QcMapBackhaulMgr)
  {
    QcMapBackhaulMgr->wan_cfg.enable_webserver_wwan_access = enable;
    if ( !QcMapBackhaulMgr->SetWebserverWWANAccessOnApps(enable, qmi_err_num) )
    {
      return false;
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION SetWebserverWWANAccessOnApps
==========================================================================*/
/*!
@brief
  Configures the Webserver WWAN Access.

@parameters
  boolean enable

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SetWebserverWWANAccessOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if (!QCMAP_ConnectionManager::GetDeviceName\
      (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return;
  }
  #ifndef FEATURE_QTIMAP_OFFTARGET
     /* Delete the existing rule, only if it was added .*/
    if( !this->wan_cfg.enable_webserver_wwan_access)
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t filter -D INPUT -i %s -p tcp --dport 80 -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t filter -D INPUT -i %s -p tcp --dport 443 -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
    }

    /* Add entry only in case of disable rule, by default it's accept*/
    if( !enable )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t filter -I INPUT -i %s -p tcp --dport 80 -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t filter -I INPUT -i %s -p tcp --dport 443 -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
    }
  #endif
  return true;
}

/*===========================================================================
  FUNCTION GetWebserverWWANAccessFlag
==========================================================================*/
/*!
@brief
  Gets the status of whether Webserver can be accessed from WWAN.

@parameters
  uint8 *flag

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetWebserverWWANAccessFlag
(
  uint8 *flag,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  char enable[MAX_STRING_LENGTH] = {0};
  if ( flag == NULL )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  if(QcMapBackhaulMgr)
  *flag = QcMapBackhaulMgr->wan_cfg.enable_webserver_wwan_access;
  else
  {
    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_WEBSERVER_WWAN_ACCESS,GET_VALUE,enable,MAX_STRING_LENGTH);
    *flag=atoi(enable);
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableIPV6
==========================================================================*/
/*!
@brief
  Enables IPV6 Functionality. If autoconnect is enabled,
  triggers the backhaul to get the IPv6 address.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::EnableIPV6(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0;
  struct timespec ts;
  boolean retval = true;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="1";//To update xml in case the backhaul object is not enabled
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                   QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }


  if(QcMapBackhaulMgr)
  {
    if (QcMapBackhaulMgr->enable_ipv6)
    {
      /* QCMAP IPV6 is already enabled */
      LOG_MSG_ERROR("QCMAP IPV6 already enabled\n",0,0,0);
      return true;
    }

    /* Enable IPV6 so that backhaul will succeed. */
    QcMapBackhaulMgr->enable_ipv6 = true;
  }

  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);

  /* Save the Configuration. */
  GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,SET_VALUE,enable,MAX_STRING_LENGTH);
  /* Check for auto-connect and bring-up backhaul. */
  ret = qcmap_cm_enable_ipv6(QcMapMgr->qcmap_cm_handle, qmi_err_num);
  if ( ret < 0 )
  {
    LOG_MSG_ERROR("QCMAP IPV6 Enable Failed: %d\n", qmi_err_num, 0, 0);
    /* Restore the state. */
    if (QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->enable_ipv6 = false;
    }
    retval = false;
  }
  if ( retval == true )
  {
    /*Enable IPV6 for bridge iface everytime since iface is not
    brought down completely during tranisitions and hence old config
    might remain effective*/
    LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for Bridge Iface",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    if((NULL != QcMapBackhaulWLANMgr) ||
       (NULL != QcMapBackhaulCradleMgr) ||
       (NULL != QcMapBackhaulEthMgr))
    {
      if (NULL != QcMapBackhaulWLANMgr)
      {
        if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
        {
          /* Enable ipv6 for bridge0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for Bridge Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /*Set accept_ra flag to 2*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf(command, MAX_COMMAND_STR_LEN,
                   "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra_prefix_route", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          if ( NULL != QcMapBackhaulMgr)
          {
            QcMapBackhaulMgr->JoinMulticastGroup(QCMAP_V6_SOCK_BRIDGE, BRIDGE_IFACE);
          }

          ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding",
                         strlen("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding"));

          QcMapBackhaulMgr->SendRSOnBridgeIface();
        }
        else
        {
          /* Enable ipv6 for STA Iface */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for STA Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          if ( NULL != QcMapBackhaulMgr)
          {
            QcMapBackhaulMgr->JoinMulticastGroup(QCMAP_V6_SOCK_WLAN, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          }

          /*Set accept_ra flag to 2*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
          /* Dont set the dft route when information is recevied in an RA*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulWLANMgr->sta_v6_available = false;
      }

      if ((NULL != QcMapBackhaulCradleMgr) &&
          (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
           QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
      {
        if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
        {
          /* Enable ipv6 for bridge0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for ECM/Cradle Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          if ( NULL != QcMapBackhaulMgr)
          {
            QcMapBackhaulMgr->JoinMulticastGroup(QCMAP_V6_SOCK_BRIDGE, BRIDGE_IFACE);
          }

          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Enable ipv6 for ecm0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for ECM/Cradle Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ECM_IFACE);
          ds_system_call(command, strlen(command));

          if ( NULL != QcMapBackhaulMgr)
          {
            QcMapBackhaulMgr->JoinMulticastGroup(QCMAP_V6_SOCK_ECM, ECM_IFACE);
          }

          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                  ECM_IFACE);
          ds_system_call(command, strlen(command));
          /* Dont set the dft route when information is recevied in an RA*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                   ECM_IFACE);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulCradleMgr->cradle_v6_available = false;
      }
      if ((NULL != QcMapBackhaulEthMgr) &&
          (QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                                QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01))
      {
        LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for ETH"
                      " Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ETH_IFACE);
          ds_system_call(command, strlen(command));

          if ( NULL != QcMapBackhaulMgr)
          {
            QcMapBackhaulMgr->JoinMulticastGroup(QCMAP_V6_SOCK_ETH, ETH_IFACE);
          }

          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                  ETH_IFACE);
          ds_system_call(command, strlen(command));
          /* Dont set the dft route when information is recevied in an RA*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                   ETH_IFACE);
          ds_system_call(command, strlen(command));
          QcMapBackhaulEthMgr->eth_v6_available = false;
      }

    }
    LOG_MSG_INFO1("QCMAP IPV6 Enabled\n", 0, 0, 0);
  }

  return retval;
}

/*===========================================================================
  FUNCTION DisableIPV6
==========================================================================*/
/*!
@brief
  Disables IPv6 Functionality.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul::DisableIPV6(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0, qcmap_cm_errno = QCMAP_CM_ENOERROR;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="0";//To update xml in case the backhaul object is not enabled
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                   QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if(QcMapBackhaulMgr)
  {
    if (!QcMapBackhaulMgr->enable_ipv6)
    {
      /* QCMAP IPV6 is not enabled */
      LOG_MSG_ERROR("QCMAP IPV6 not enabled\n",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }

      /* The logic here is to disconnect the existing V6 backhaul and set the
       * enable_ipv6 flag to false. This is required even when we are in STA mode.
       * Reason being if we don't bring down the call now and when we switch from
       * WLAN backhaul goes down we will still have IPV6 WWAN backhaul which should
       * not be the case.
       */
      /* Disconnect Backhaul. */
      ret = qcmap_cm_disable_ipv6(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, qmi_err_num);

      if (ret == QCMAP_CM_SUCCESS ||
          (ret == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK))
      {
        if (NULL != QcMapBackhaulWLANMgr)
        {
          if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
          {
            /* Disable ipv6 for bridge0 */
            LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for Bridge Iface",0,0,0);
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                     BRIDGE_IFACE);
            ds_system_call(command, strlen(command));

            /*Set accept_ra flag to 1*/
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                     BRIDGE_IFACE);
            ds_system_call(command, strlen(command));

            if (QcMapBackhaulMgr->RemoveIPV6Address(BRIDGE_IFACE))
              LOG_MSG_ERROR("Error Removing IPV6 address from BRIDGE Iface",0,0,0);

            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
          }
          else
          {
            /* Disable ipv6 for STA Iface */
            LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for STA Iface",0,0,0);
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                     QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
            ds_system_call(command, strlen(command));

            /*Set accept_ra flag to 1*/
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                     QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
            ds_system_call(command, strlen(command));

            if (QcMapBackhaulMgr->RemoveIPV6Address(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface))
              LOG_MSG_ERROR("Error Removing IPV6 address from STA Iface",0,0,0);

            if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
            }

            LOG_MSG_INFO1("Installing iptable rule to block IPv6 traffic on STA Iface",0,0,0);
            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          }
          QcMapBackhaulWLANMgr->sta_v6_available = false;
        }

        if ((NULL != QcMapBackhaulCradleMgr) &&
            (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
             QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
        {
          if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
          {
            /* Disable ipv6 for bridge0 */
            LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for Bridge Iface",0,0,0);
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                     BRIDGE_IFACE);
            ds_system_call(command, strlen(command));

            /*Set accept_ra flag to 1*/
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                     BRIDGE_IFACE);
            ds_system_call(command, strlen(command));

            if (QcMapBackhaulMgr->RemoveIPV6Address(BRIDGE_IFACE))
              LOG_MSG_ERROR("Error Removing IPV6 address from BRIDGE Iface",0,0,0);
          }
          else
          {
            /* Disable ipv6 for ecm0 */
            LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for ECM/Cradle Iface",0,0,0);
            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   ECM_IFACE);
            ds_system_call(command, strlen(command));

            snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                     ECM_IFACE);
            ds_system_call(command, strlen(command));

            if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);

            if (QcMapBackhaulMgr->RemoveIPV6Address(ECM_IFACE))
              LOG_MSG_ERROR("Error Removing IPV6 address from Cradle Iface",0,0,0);
          }
          QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
          QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
          QcMapBackhaulCradleMgr->cradle_v6_available = false;
        }
        if ((NULL != QcMapBackhaulEthMgr) &&
          (QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                                QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01))
        {
          /* Disable ipv6 for ecm0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for ETH Iface",
                           0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ETH_IFACE);
          ds_system_call(command, strlen(command));

          snprintf(command, MAX_COMMAND_STR_LEN,
                   "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   ETH_IFACE);
          ds_system_call(command, strlen(command));

          if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET)
            QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);

          if (QcMapBackhaulMgr->RemoveIPV6Address(ETH_IFACE))
            LOG_MSG_ERROR("Error Removing IPV6 address from ETH Iface",0,0,0);
          QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
          QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
          QcMapBackhaulEthMgr->eth_v6_available = false;
        }

        QcMapBackhaulMgr->DisableIPV6Forwarding(true);
        QcMapFirewall->CleanIPv6MangleTable();
        QcMapBackhaulMgr->enable_ipv6= false;
      }
      else
      {
        LOG_MSG_INFO1( "QCMAP IPV6 Disable Fail %d: %d\n",
                        qcmap_cm_errno, qmi_err_num, 0);
        return false;
      }
    }

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false &&
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
      (NULL != QcMapBackhaulMgr))
  {
    switch (QcMapBackhaulMgr->current_backhaul)
    {
      case BACKHAUL_TYPE_AP_STA_ROUTER:
        if (!QCMAP_Backhaul_WLAN::IsSTAAvailableV4())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                                      QcMapBackhaulMgr->current_backhaul, true,
                                      false);
        }
      break;
      case BACKHAUL_TYPE_CRADLE:
        if (!QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                                      QcMapBackhaulMgr->current_backhaul, true,
                                      false);
        }
      break;
      case BACKHAUL_TYPE_WWAN:
        if (qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED)
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                                      QcMapBackhaulMgr->current_backhaul, true,
                                      false);
        }
      break;
      case BACKHAUL_TYPE_ETHERNET:
       if (!QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                                      QcMapBackhaulMgr->current_backhaul, true,
                                      false);
        }
      break;
      default:
        LOG_MSG_ERROR("Invalid Backhaul", 0, 0, 0);
      break;
    }
  }

    LOG_MSG_INFO1("QCMAP IPV6 Disabled\n", 0, 0, 0);
    /* Save the Configuration. */
    GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,SET_VALUE,enable,strlen(enable));
    return true;
}
/*===========================================================================
  FUNCTION EnableIPV6Forwarding
==========================================================================*/
/*!
@brief
  Enables Ipv6 forwarding and starts radish.

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul::EnableIPV6Forwarding()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  qmi_error_type_v01 qmi_err_num;
  qcmap_nl_addr_t nl_addr;
  int ret;
  struct in6_addr all_nodes_addr = { 0xff,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  char ipv6_addr[INET6_ADDRSTRLEN];

  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);

  LOG_MSG_INFO1("Entering IPV6 Forwarding",0,0,0);
  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  if (!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);

  bzero( QcMapBackhaulMgr->wan_cfg.ipv6_interface, QCMAP_MSGR_INTF_LEN );
  strlcpy( QcMapBackhaulMgr->wan_cfg.ipv6_interface, devname, QCMAP_MSGR_INTF_LEN );

  if (QcMapBackhaulMgr->enable_ipv6 )
  {
    if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      QcMapMgr->CleanIPv6FilterTableFwdChain();
    }

    /* Remove the prefix based v6 rule since we might get a new prefix */

    if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) &&
        (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile\
         == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) &&
         QcMapBackhaulWWANMgr && (QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added))
    {
      inet_ntop(AF_INET6,((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))\
                ->sin6_addr.s6_addr, ip6_addr, MAX_IPV6_PREFIX);
      snprintf( command, MAX_COMMAND_STR_LEN,
               "ebtables -D INPUT -i wlan%d -p IPv6 --ip6-destination %s/%d -j DROP",
                QcMapMgr->ap_dev_num2,ip6_addr,ipv6_prefix_info.prefix_len);
      ds_system_call( command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN,
               "ebtables -D OUTPUT -o wlan%d -p IPv6 --ip6-source %s/%d -j DROP",
                QcMapMgr->ap_dev_num2,ip6_addr,ipv6_prefix_info.prefix_len);
      ds_system_call( command, strlen(command));
      QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added = false;
    }
    if(QcMapBackhaulWWANMgr)
    {
      LOG_MSG_INFO1("Setting prefix delegation mode %d",\
                    QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation,0,0);
      QcMapBackhaulWWANMgr->prefix_delegation_activated =\
                               QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation;
    }
    LOG_MSG_INFO1("\nSetting forwarding for ipv6",0,0,0);
    if ( (!QcMapBackhaulWWANMgr ||
         !QcMapBackhaulWWANMgr->prefix_delegation_activated) ||
         QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {

      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra", devname);
      ds_system_call(command, strlen(command));
      ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/proxy_ndp",
                      strlen("echo 2 > /proc/sys/net/ipv6/conf/all/proxy_ndp"));
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_prefix_route", devname);
      ds_system_call(command, strlen(command));
    }

    ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding",
                   strlen("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding"));
#ifndef FEATURE_QTIMAP_OFFTARGET
    /* Deleting the default route. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del default dev %s metric 256", devname);
    ds_system_call(command, strlen(command));

    /* Add the default route with lower metric so that the packets
     *  will follow this route.
     */
    LOG_MSG_INFO1("\n Adding route for inet6 ::/0\n",0,0,0);
    /* If backhaul was STA or Cradle, delete the default route with gateway*/
    if ((QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) &&
                    QcMapBackhaulWLANMgr)
    {
      inet_ntop(AF_INET6, &QcMapBackhaulWLANMgr->ipv6_gateway_addr, ipv6_addr,
                               INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route add default via %s dev wlan%d",ipv6_addr, QcMapMgr->ap_dev_num1);
      ds_system_call(command, strlen(command));
    }
    else if((QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE) &&
                   QcMapBackhaulCradleMgr)
    {
      QcMapBackhaulCradleMgr->GetCradleMode(&cradle_mode,&qmi_err_num);
      if (cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
      {
        inet_ntop(AF_INET6, &QcMapBackhaulCradleMgr->ipv6_gateway_addr, ipv6_addr,
                               INET6_ADDRSTRLEN);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route add default via %s dev %s",ipv6_addr, ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
    else if ((QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET)
             && QcMapBackhaulEthMgr)
    {
      /* Need to add support for WAN bridge mode*/
      inet_ntop(AF_INET6, &QcMapBackhaulEthMgr->ipv6_gateway_addr, ipv6_addr,
                              INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route add default via %s dev %s",ipv6_addr, ETH_IFACE);
      ds_system_call(command, strlen(command));
    }
    else
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route add default dev %s metric 256", devname);
      ds_system_call(command, strlen(command));
    }
#endif
    /* Get the global ipv6 address. */
    memset(&(QcMapBackhaulMgr->ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
    QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
    QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
    QcMapBackhaulMgr->GetIPV6PrefixInfo(devname, &QcMapBackhaulMgr->ipv6_prefix_info);

    if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid == true &&
         ((!QcMapBackhaulWWANMgr || !QcMapBackhaulWWANMgr->prefix_delegation_activated)\
         || IsNonWWANBackhaulActive()))
    {
      QcMapBackhaulMgr->UpdatePrefix(&QcMapBackhaulMgr->ipv6_prefix_info, false, true, NULL);

      memset(&nl_addr, 0 ,sizeof(nl_addr));
      memcpy(nl_addr.ipv6_dst_addr, all_nodes_addr.s6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      memcpy(nl_addr.ip_v6_addr,
             ((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      nl_addr.isValidIPv6address = true;
      QcMapBackhaulMgr->UpdateGlobalV6addr(&nl_addr);
    }

    /* Restart Radish. */
    QcMapBackhaulMgr->RestartRadish();

    if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated &&
        !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      QcMapBackhaulMgr->SendRSForClients();
    }

    /* Add the prefix based v6 rule since we might get a new prefix */
    if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) &&
         (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) )
    {
      if (QcMapBackhaulWWANMgr && (!QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added) &&
           QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
      {
        /* Get new IPv6 Address  */
        memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);
        inet_ntop(AF_INET6,((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr, ip6_addr, MAX_IPV6_PREFIX );
        snprintf( command, MAX_COMMAND_STR_LEN,
                 "ebtables -A INPUT -i wlan%d -p IPv6 --ip6-destination %s/%d -j DROP",
                 QcMapMgr->ap_dev_num2,ip6_addr,QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,
                 "ebtables -A OUTPUT -o wlan%d -p IPv6 --ip6-source %s/%d -j DROP",
                 QcMapMgr->ap_dev_num2,ip6_addr,QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
        ds_system_call( command, strlen(command));
        QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added = true;
      }
      /* This was add to block data reaching A5, during embedded call scenario*/
      snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                QcMapMgr->ap_dev_num2);
      ds_system_call( command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv6 -j DROP \n",
                QcMapMgr->ap_dev_num2);
      ds_system_call( command, strlen(command));
    }
  }
  if(!QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if( QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable\
        == DDNS_ENABLED && qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED)
    {
      QcMapBackhaulWWANMgr->StartDDNS(&qmi_err_num);
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableIPV6Forwarding
==========================================================================*/
/*!
@brief
  This function will disable Ipv6 forwarding and stops radish service.

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::DisableIPV6Forwarding(boolean send_ra)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  int ret;
  char ip6_addr[INET6_ADDRSTRLEN];
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  qcmap_msgr_eth_backhaul_mode_v01 eth_mode=QCMAP_MSGR_ETH_BACKHAUL_LAN_ROUTER_V01;
  LOG_MSG_INFO1("Disable IPV6 Forwarding",0,0,0);


  ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);


  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  if (QcMapBackhaulMgr && QcMapBackhaulMgr->enable_ipv6 )
  {

    /* Stop Radish. */
    QcMapBackhaulMgr->StopRadish();

    /* Install the rule to block WWAN access. */
    QcMapBackhaulMgr->BlockIPv6WWANAccess();

    /* Deprecate old prefix. */
    if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid == true )
    {
      if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated)
        QcMapBackhaulMgr->DeprecateClientRA();
      else
        QcMapBackhaulMgr->UpdatePrefix(&QcMapBackhaulMgr->ipv6_prefix_info, true, send_ra, NULL);
    }
    else
    {
       /* In case of STA Mode, try to get the address again and deprecate the prefix. */
       if ( ret!=BACKHAUL_TYPE_WWAN)
       {
         //strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
         memset(&(QcMapBackhaulMgr->ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
         QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
         QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
         QcMapBackhaulMgr->GetIPV6PrefixInfo(devname, &QcMapBackhaulMgr->ipv6_prefix_info);
         if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid == true )
         {
           QcMapBackhaulMgr->UpdatePrefix(&QcMapBackhaulMgr->ipv6_prefix_info, true, send_ra, NULL);
         }
       }
    }

    /* Delete PPP IPV6 route if present. */
    QcMapBackhaulMgr->DeletePPPIPv6Route();

    LOG_MSG_INFO1("\nDisabling forwarding for ipv6",0,0,0);
    ds_system_call("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding",
                    strlen("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding"));
    ds_system_call("echo 0 > /proc/sys/net/ipv6/conf/all/proxy_ndp",
                   strlen("echo 0 > /proc/sys/net/ipv6/conf/all/proxy_ndp"));
#ifndef FEATURE_QTIMAP_OFFTARGET
    LOG_MSG_INFO1("\n Delete default route for inet6 ::/0\n",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del default metric 256");
    ds_system_call(command, strlen(command));
    /* If backhaul was STA or Cradle, delete the default route with gateway*/
    if ((ret == BACKHAUL_TYPE_AP_STA_ROUTER) && QcMapBackhaulWLANMgr)
    {
      inet_ntop(AF_INET6, &QcMapBackhaulWLANMgr->ipv6_gateway_addr, ip6_addr,
                               INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route del default via %s dev wlan%d",ip6_addr, QcMapMgr->sta_iface_index);
      ds_system_call(command, strlen(command));
    }
    else if((ret == BACKHAUL_TYPE_CRADLE) && QcMapBackhaulCradleMgr)
    {
      QcMapBackhaulCradleMgr->GetCradleMode(&cradle_mode,&qmi_err_num);
      if (cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
      {
        inet_ntop(AF_INET6, &QcMapBackhaulCradleMgr->ipv6_gateway_addr, ip6_addr,
                               INET6_ADDRSTRLEN);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del default via %s dev %s",ip6_addr, ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
    else if((ret == BACKHAUL_TYPE_ETHERNET) && QcMapBackhaulEthMgr)
    {
      QcMapBackhaulEthMgr->GetEthBackhaulMode(&eth_mode,&qmi_err_num);
      if (eth_mode == QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
      {
        inet_ntop(AF_INET6, &QcMapBackhaulEthMgr->ipv6_gateway_addr, ip6_addr,
                               INET6_ADDRSTRLEN);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del default via %s dev %s",ip6_addr, ETH_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
#endif
    if (ret!=BACKHAUL_TYPE_WWAN)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra", devname);
      ds_system_call(command, strlen(command));
    }

    //Enable the bridge forwarding for Ipv6
    snprintf(command, MAX_COMMAND_STR_LEN,
             "echo 1 > /proc/sys/net/ipv6/conf/bridge0/forwarding");
    ds_system_call(command, strlen(command));

    /*Reset the Global Ipv6 address from the connected devices information*/
    if (QcMapMgr->GetNumofConnectedDevices() > 0)
    {
      if (!(QcMapBackhaulMgr->DeleteGlobalipv6AddressInfo()))
      {
        LOG_MSG_ERROR("Could not delete Global IPv6 address from Connected"
                      "devices information", 0, 0, 0);
      }
    }
  }
  /* Add the prefix based v6 rule since we might get a new prefix */
  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) &&
      (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile\
      == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) && ret!=BACKHAUL_TYPE_WWAN)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv6 -j DROP \n",
              QcMapMgr->ap_dev_num2);
    ds_system_call( command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv6 -j DROP \n",
              QcMapMgr->ap_dev_num2);
    ds_system_call( command, strlen(command));
  }
  if(ret==BACKHAUL_TYPE_WWAN)
  {
    if(QcMapBackhaulWWANMgr &&  QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable  == DDNS_ENABLED &&
        qcmap_cm_get_state(&qmi_err_num) != QCMAP_CM_WAN_CONNECTED &&
        qcmap_cm_get_ipv6_state(&qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED )
    {
      QcMapBackhaulWWANMgr->StopDDNS();
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION UpdatePrefix
==========================================================================*/
/*!
@brief
  Updates the prefix.

@param
  qcmap_cm_nl_prefix_info_t *ipv6_prefix_info,
  boolean deprecate, boolean send_ra,
  uint8_t *dest_v6_ip

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

int QCMAP_Backhaul::UpdatePrefix
(
  qcmap_cm_nl_prefix_info_t *ipv6_prefix_info,
  boolean deprecate, boolean send_ra,
  uint8_t *dest_v6_ip
)
{
  struct icmp6_ra_pkt ra_pkt;
  struct nd_router_advert *router_adv = NULL;
  struct nd_opt_prefix_info *prefix_info = NULL;
  struct nd_opt_mtu *mtu_info = NULL;
  struct sockaddr_in6 dst_addr;
  struct ps_in6_addr *prefix_ptr, *addr_ptr, *addr_ptr1;
  int ret = -1;
  char ipaddr[MAX_IPV6_PREFIX + 1];
  char command[MAX_COMMAND_STR_LEN];
  int ppp_sockfd = 0;
  boolean is_multicast = false;
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  if ( ipv6_prefix_info == NULL )
  {
    LOG_MSG_ERROR("NULL Prefix Passed\n", 0, 0, 0);
    return ret;
  }

  if ( QCMAPLANMgr->bridge_sock < 0 )
  {
    LOG_MSG_ERROR("Error: Bridge Sock is not created\n", 0, 0, 0);
    return ret;
  }

  memset(&dst_addr, 0, sizeof(dst_addr));
  memset(&ra_pkt,0,sizeof(ra_pkt));

  router_adv = &(ra_pkt.router_adv);

  /* Initialize RA Packet. */
  router_adv->nd_ra_type = ND_ROUTER_ADVERT;
  router_adv->nd_ra_code = 0;
  router_adv->nd_ra_cksum = 0;
  router_adv->nd_ra_curhoplimit = 64;
  router_adv->nd_ra_flags_reserved = deprecate ? 0 : ND_RA_FLAG_OTHER;
  router_adv->nd_ra_router_lifetime = deprecate ? 0 : htons(0xffff);
  router_adv->nd_ra_reachable = 0;
  router_adv->nd_ra_retransmit = 0;

  prefix_info = &(ra_pkt.prefix_info);

  prefix_info->nd_opt_pi_type = ND_OPT_PREFIX_INFORMATION;
  prefix_info->nd_opt_pi_len = sizeof(struct nd_opt_prefix_info)/sizeof(uint64);
  prefix_info->nd_opt_pi_prefix_len = ipv6_prefix_info->prefix_len;
  prefix_info->nd_opt_pi_flags_reserved = ND_OPT_PI_FLAG_ONLINK | ND_OPT_PI_FLAG_AUTO;
  prefix_info->nd_opt_pi_valid_time = deprecate ? htonl(DEFAULT_VALID_LIFETIME) :
                                      htonl(ipv6_prefix_info->cache_info.ifa_valid);

  prefix_info->nd_opt_pi_preferred_time = deprecate ? htonl(DEFAULT_PREFERRED_LIFETIME):
                                          htonl(ipv6_prefix_info->cache_info.ifa_prefered);

  prefix_info->nd_opt_pi_reserved2 = 0;

  mtu_info = &(ra_pkt.mtu_info);

  mtu_info->nd_opt_mtu_type = ND_OPT_MTU;
  mtu_info->nd_opt_mtu_len = sizeof(struct nd_opt_mtu)/sizeof(uint64);
  mtu_info->nd_opt_mtu_reserved = 0;
  mtu_info->nd_opt_mtu_mtu = htonl(ipv6_prefix_info->mtu);

  memcpy(prefix_info->nd_opt_pi_prefix.s6_addr,
         ((struct sockaddr_in6 *)&(ipv6_prefix_info->prefix_addr))->sin6_addr.s6_addr,
         sizeof(prefix_info->nd_opt_pi_prefix.s6_addr));

  /* Zero the bits which are greater than Prefix length. */
  prefix_ptr = (struct ps_in6_addr *)&prefix_info->nd_opt_pi_prefix;
  if ( prefix_info->nd_opt_pi_prefix_len <= IPV6_MIN_PREFIX_LENGTH )
  {
    /* Zero out the non prefix bits. */
    prefix_ptr->ps_s6_addr64[0] >>=
                  (IPV6_MIN_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
    prefix_ptr->ps_s6_addr64[0] <<=
                   (IPV6_MIN_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
    /* Zero out the IID part */
    prefix_ptr->ps_s6_addr64[1] = 0;
  }
  else
  {
    /* No need to touch the first 64 bits. Just Modify the  */
    /* Zero out the non prefix bits. */
    prefix_ptr->ps_s6_addr64[1] >>=
                   (IPV6_MAX_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
    prefix_ptr->ps_s6_addr64[1] <<=
                   (IPV6_MAX_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
  }

  if (dest_v6_ip != NULL)
    memcpy(dst_addr.sin6_addr.s6_addr, dest_v6_ip,
           sizeof(dst_addr.sin6_addr.s6_addr));
  else
  {
    inet_pton(AF_INET6, "ff02::1", &dst_addr.sin6_addr);
    is_multicast = true;
  }

  dst_addr.sin6_family = AF_INET6;

  if (send_ra)
  {
    LOG_MSG_ERROR("Forwarding RA\n", 0, 0, 0);
    if ((ret = sendto(QCMAPLANMgr->bridge_sock,&ra_pkt,sizeof(ra_pkt),0,
         (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
    {
      LOG_MSG_ERROR("Error: Cannot send RA: %d.\n", errno, 0, 0);
    }

    if (QcMapTetheringMgr)
    {
      /* Check if PPP enabled and send the prefix. */
      addr_ptr = (struct ps_in6_addr *)dest_v6_ip;
      addr_ptr1 = (struct ps_in6_addr *)QcMapTetheringMgr->ppp_ipv6_iid;
      if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
         ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
           QCMAP_QTI_TETHERED_LINK_PPP) &&
         ((is_multicast == true) ||
         !memcmp(&addr_ptr->ps_s6_addr64[1],
                 &addr_ptr1->ps_s6_addr64[1],
                 sizeof(uint64))))
      {
        LOG_MSG_ERROR("Forwarding RA to PPP interface\n", 0, 0, 0);
        /* Send the prefix over the PPP interface. */
        ppp_sockfd = QCMAPLANMgr->CreateRawSocket(PPP_IFACE);

        if ( ppp_sockfd < 0 )
        {
          LOG_MSG_ERROR("Error: PPP Sock is not created\n", 0, 0, 0);
          return ret;
        }

        if ((ret = sendto(ppp_sockfd,&ra_pkt,sizeof(ra_pkt),0,
             (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
        {
          LOG_MSG_ERROR("Error: Cannot send RA to PPP interface: %d.\n", errno, 0, 0);
        }
        close(ppp_sockfd);
      }
    }
  }
  /* Based on the deprecate flag decide whether to add/delete the prefix based route. */
  memset(ipaddr, 0, MAX_IPV6_PREFIX + 1);
  inet_ntop(AF_INET6, (struct in6_addr *)prefix_ptr, ipaddr, MAX_IPV6_PREFIX);
  if ( deprecate )
  {
    LOG_MSG_INFO1("\n Delete the prefix based route for inet6\n", 0, 0, 0);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del %s/%d", ipaddr, prefix_info->nd_opt_pi_prefix_len);
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_INFO1( "\n First Delete the prefix based route for WWAN Interface\n",
                   0, 0, 0 );
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del %s/%d", ipaddr, prefix_info->nd_opt_pi_prefix_len);
    ds_system_call(command, strlen(command));
    LOG_MSG_INFO1("\n Add the prefix based route for Bridge Interface\n", 0, 0, 0);

    snprintf(command, MAX_COMMAND_STR_LEN,"ip -6 route add %s/%d dev %s",
             ipaddr, prefix_info->nd_opt_pi_prefix_len, BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
  }
  return ret;

}

/*===========================================================================
  FUNCTION StartRadish
==========================================================================*/
/*!
@brief
  Starts Radish with appropriate intrerface, this will ebanle Ipv6 multicast
  forwarding.

@param
  None

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::StartRadish(void)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  qmi_error_type_v01 qmi_err_num;

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("\n Starting radish \n",0,0,0);


  if (!QCMAP_ConnectionManager::GetDeviceName\
                             (devname,QCMAP_MSGR_IP_FAMILY_V6_V01,&qmi_err_num))
    {
      return;
    }


  if ( QcMapTetheringMgr &&
       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
       (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                    QCMAP_QTI_TETHERED_LINK_PPP))
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "radish -k -b %s -i %s -x -i %s -x -i %s -x > /dev/null 2>&1 &",
             devname, devname, BRIDGE_IFACE, PPP_IFACE);
  }
  else
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "radish -k -b %s -i %s -x -i %s -x > /dev/null 2>&1 &",
             devname, devname, BRIDGE_IFACE);
  }
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION StopRadish
==========================================================================*/
/*!
@brief
  This function will stop Radish.

@param
  None

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::StopRadish(void)
{
  LOG_MSG_INFO1("\n Stopping radish \n",0,0,0);
  ds_system_call("killall -s SIGTERM radish", strlen("killall -s SIGTERM radish"));
}


/*===========================================================================
  FUNCTION RestartRadish
==========================================================================*/
/*!
@brief
  This function will restart Radish.

@param
  None

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::RestartRadish(void)
{
  LOG_MSG_INFO1("\n Restarting radish \n",0,0,0);

  LOG_MSG_INFO1("\n Stopping radish \n",0,0,0);
  ds_system_call("killall -s SIGKILL radish", strlen("killall -s SIGKILL radish"));

  StartRadish();
}


/*===========================================================================
  FUNCTION IsRadishRunning
==========================================================================*/
/*!
@brief
  Checks to see if radish is running, waits up to 1 second

@parameters
  None

@return
  true  - process is running
  flase - process is not running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::IsRadishRunning()
{
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;
  const char process[] = "pidof radish";

  while (i++ < 5)
  {
    usleep(200000);
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pid = atoi(pid_s);
      pclose(cmd);
      if(pid != 0)
      {
        return true;
      }
    }
  }

  LOG_MSG_ERROR("Radish is not running after waiting 1 second\n", 0, 0, 0);
  return false;
}

/*===========================================================================
  FUNCTION GetIPv4State
==========================================================================*/
/*!
@brief
  Gets the IPv4 state.

@param


@parameters
  uint8_t *status,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetIPv4State
(
  uint8_t *status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  char enable[MAX_STRING_LENGTH]= {0};
  if(QcMapBackhaulMgr)
  *status=QcMapBackhaulMgr->enable_ipv4 ;
  else
  {
    GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,GET_VALUE,enable, MAX_STRING_LENGTH);
    *status=(uint8_t)atoi(enable);
  }
  return true;

}

/*===========================================================================
  FUNCTION GetIPv6State
==========================================================================*/
/*!
@brief
  Gets the IPv6 state.

@parameters
  uint8_t *status,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetIPv6State(uint8_t *status, qmi_error_type_v01 *qmi_err_num)
{
  char enable[MAX_STRING_LENGTH] = {0};
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  if(QcMapBackhaulMgr)
  *status=QcMapBackhaulMgr->enable_ipv6 ;
  else
  {
    GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,GET_VALUE,enable,MAX_STRING_LENGTH);
    *status=(uint8_t)atoi(enable);
  }
  return true;

}

/*===========================================================================
  FUNCTION DeletePPPIPv6Route
==========================================================================*/
/*!
@brief
  Deletes the destination based route to ppp interface.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::DeletePPPIPv6Route()
{
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  memset(ip6_addr, 0, MAX_IPV6_PREFIX+1);
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (!(QcMapTetheringMgr  &&
       (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
       QCMAP_QTI_TETHERED_LINK_PPP)))
  {
    /* QCMAP PPP not enabled */
    LOG_MSG_ERROR("QCMAP PPP not enabled\n",0,0,0);
    return;
  }

  if (!this->enable_ipv6)
  {
    /* QCMAP IPV6 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV6 not enabled\n",0,0,0);
    return;
  }

  /* Delete the destination based route. */
  inet_ntop(AF_INET6,(struct in6_addr *)QcMapTetheringMgr->ppp_ipv6_addr, ip6_addr, MAX_IPV6_PREFIX);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ip -6 route del %s dev %s", ip6_addr, PPP_IFACE);
  ds_system_call( command, strlen(command));

  /* Reset the address. */
  memset(QcMapTetheringMgr->ppp_ipv6_addr, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  return;
}

/*===========================================================================
  FUNCTION IsPPPdkilled
==========================================================================*/
/*!
@brief
  Waits 1 second for the process to die.

@parameters
  None

@return
  true  - process killed
  flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::IsPPPkilled()
{
  char process[MAX_COMMAND_STR_LEN];
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_COMMAND_STR_LEN];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  snprintf(process, MAX_COMMAND_STR_LEN, "pidof pppd");

  while (i++ < QCMAP_PROCESS_KILL_RETRY)
  {
    usleep(QCMAP_PROCESS_KILL_WAIT_MS);
    cmd = popen(process, "r");
    pid = 0;
    if(cmd)
    {
      fgets(pid_s, MAX_COMMAND_STR_LEN, cmd);
      pid = atoi(pid_s);
      LOG_MSG_INFO1("\n pid =%d \n", pid, 0, 0);
      pclose(cmd);
      /* If pid is zero we break from while*/
      if(pid == 0)
      {
        return true;
      }
    }
  }

  LOG_MSG_ERROR("PID still running after waiting 2 second\n", 0, 0, 0);
  return false;
}
/*===========================================================================
  FUNCTION AddPPPIPv6Route
==========================================================================*/
/*!
@brief
  Adds the route for PPP interface.

@parameters
  void

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::AddPPPIPv6Route()
{
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (!QcMapTetheringMgr)
  {
    LOG_MSG_INFO1("Tethering is not enabled.", 0, 0, 0);
    return;
  }
  /* Flush the existing routes. */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ip -6 route flush dev %s proto boot", PPP_IFACE);
  ds_system_call( command, strlen(command));

  inet_ntop( AF_INET6,(struct in6_addr *)QcMapTetheringMgr->ppp_ipv6_addr, ip6_addr,
             MAX_IPV6_PREFIX);

  snprintf( command, MAX_COMMAND_STR_LEN,
            "ip -6 route add %s dev %s", ip6_addr, PPP_IFACE);
  ds_system_call( command, strlen(command));

  return;
}
/*===========================================================================
  FUNCTION BlockIPv4WWANAccess
==========================================================================*/
/*!
@brief
  This function blocks IPv4 WWAN access for MobileAP clients.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::BlockIPv4WWANAccess()
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
/*===========================================================================
  Allow access between Bridge and Bridge.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -A FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

/*===========================================================================
  Allow access between Bridge and PPP.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, PPP_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -A FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, PPP_IFACE);
  ds_system_call(command, strlen(command));

  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -A FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

/*===========================================================================
  Block WWAN access to MobileAP clients.
==========================================================================*/
/* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
          "iptables -A FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
/*===========================================================================
    Block WWAN access to PPP client.
==========================================================================*/
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
          "iptables -A FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION BlockIPv6WWANAccess
=========================================================================*/
/*!
@brief
  This function blocks IPv6 WWAN access for MobileAP clients.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::BlockIPv6WWANAccess()
{
  char command[MAX_COMMAND_STR_LEN];
  int qcmap_cm_error=0;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
/*===========================================================================
  Allow access between Bridge and Bridge.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -A FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

/*===========================================================================
  Allow access between Bridge and PPP.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, PPP_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -A FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, PPP_IFACE);
  ds_system_call(command, strlen(command));

  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -A FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

/*===========================================================================
  Block WWAN access to MobileAP clients.
==========================================================================*/
  /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
          "ip6tables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
          "ip6tables -A FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

/*===========================================================================
    Block WWAN access to PPP client.
==========================================================================*/
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
          "ip6tables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,
          "ip6tables -A FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));
}

/*=====================================================
  FUNCTION DeleteGlobalipv6AddressInfo
======================================================*/
/*!
@brief
 Deletes the Global Ipv6 address from the connected
 devices linked list

@parameters
  - void

@return
  bool
  -true on successful deletion
  -false on failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_Backhaul:: DeleteGlobalipv6AddressInfo ()
{
  int i = 0;
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("DeleteGlobalipv6AddressInfo - Linked list head is NULL \n",
                  0, 0, 0);
    return false;
  }

  node = addrList->addrListHead->next;
  if ( node == NULL)
  {
    LOG_MSG_ERROR("DeleteGlobalipv6AddressInfo - Device info is NULL"
                  "Cant fetch linked list node", 0, 0, 0);
    return false;
  }

  for (i = 0; i < QcMapMgr->GetNumofConnectedDevices(); i++)
  {
    connectedDevicesList = ( qcmap_cm_client_data_info_t* )node->data;
    if (connectedDevicesList == NULL)
    {
      LOG_MSG_ERROR("DeleteGlobalipv6AddressInfo - Connected Device context for"
                    " node %d+1 is NULL", i, 0, 0);
      return false;
    }
    memset (connectedDevicesList->ip_v6_addr, 0,
            QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    node = node->next;
  }

  LOG_MSG_INFO1("DeleteGlobalipv6AddressInfo:Global Ipv6 address of clients "
                "successfully deleted", 0, 0, 0);
  return true;
}

/*=====================================================
  FUNCTION DeprecateClientRA
======================================================*/
/*!
@brief
  Deprecate Client RA

@parameters
  - void

@return
  -true on successful deletion
  -false on failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean QCMAP_Backhaul::DeprecateClientRA(void)
{
  ds_dll_el_t                 *node, *node2;
  qcmap_cm_nl_prefix_info_t   ipv6_prefix_info;
  qmi_error_type_v01          qmi_err_num;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_addr_info_list_t* addrList_prev = &(QcMapMgr->addrList_prev);
  struct ps_in6_addr null_ipv6_address;

  if(addrList->addrListHead == NULL && addrList_prev->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: Lists are empty", 0, 0, 0);
    return false;
  }

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_INFO1("Current device list is empty, use previous", 0, 0, 0);
    node = ds_dll_next(addrList_prev->addrListHead, (const void**)(&connectedDevicesList));
  }
  else if(addrList_prev->addrListHead == NULL){
    LOG_MSG_INFO1("Use Current device list", 0, 0, 0);
    node = ds_dll_next(addrList->addrListHead, (const void**)(&connectedDevicesList));
  }
  else{
    LOG_MSG_INFO1("Merging lists", 0, 0, 0);

    node = addrList->addrListHead;
    node = ds_dll_next (node, (const void**)(&connectedDevicesList));

    while (node != NULL)
    {
      if ((node2 = ds_dll_search(addrList_prev->addrListHead,
                                 (void*)connectedDevicesList->mac_addr,
                                 QcMapMgr->qcmap_match_mac_address)) == NULL)
      {
        /* mac_addr not found, so add to list */
        LOG_MSG_INFO1("Adding element to list", 0, 0, 0);
        if ((node2 = ds_dll_enq(addrList_prev->addrListHead,
                             NULL, (void*)connectedDevicesList )) == NULL)
        {
          LOG_MSG_ERROR("Error in adding a node",0,0,0);
          return false;
        }
        addrList_prev->addrListTail = node2;
      }
      node = ds_dll_next (node, (const void**)(&connectedDevicesList));
    }

    node = ds_dll_next(addrList_prev->addrListHead, (const void**)(&connectedDevicesList));
  }

  if (node == NULL)
  {
    LOG_MSG_ERROR("Error: first element in list is empty", 0, 0, 0);
    return false;
  }

  memset(&null_ipv6_address, 0, sizeof(struct ps_in6_addr));

  while (node != NULL)
  {
    if (connectedDevicesList == NULL)
    {
      LOG_MSG_ERROR("connectedDevicesList is NULL", 0, 0, 0);
      node = ds_dll_next (node, (const void**)(&connectedDevicesList));
      continue;
    }

    if (memcmp (connectedDevicesList->ip_v6_addr, &null_ipv6_address, sizeof(struct in6_addr)))
    {

      memset(&ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
      ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      ipv6_prefix_info.prefix_len =  (unsigned char) IPV6_MIN_PREFIX_LENGTH;
      memcpy(((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr,
             connectedDevicesList->ip_v6_addr, sizeof(connectedDevicesList->ip_v6_addr));

      if(this->UpdatePrefix(&this->ipv6_prefix_info, true, true, connectedDevicesList->link_local_v6_addr) == -1)
      {
        LOG_MSG_ERROR("Error: Unable to deprecate prefix", 0, 0, 0);
        return false;
      }
    }
    else
      LOG_MSG_ERROR("Node does not have global v6 addr, skipping", 0, 0, 0);

    node = ds_dll_next (node, (const void**)(&connectedDevicesList));
  }

  //Remove Delegated Prefix's
  if (!qcmap_cm_delete_ipv6_delegated_prefix(false, NULL, &qmi_err_num))
  {
    LOG_MSG_ERROR("Error: Unable flush prefix's %d", qmi_err_num, 0, 0);
  }

  /* Cleanup addrList_prev */
  if (addrList_prev->addrListHead != NULL)
  {
    while ((node = ds_dll_deq( addrList_prev->addrListHead, &(addrList_prev->addrListTail),
           (const void**) &connectedDevicesList)))
    {
      //Free the device information structure
      ds_free (connectedDevicesList);
      connectedDevicesList = NULL;
      ds_dll_free(node);
    }
    ds_dll_destroy(addrList_prev->addrListHead);
  }

  return true;
}

/*===========================================================================
  FUNCTION AssignGlobalV6AddrAllClients
==========================================================================*/
/*!
@brief
  Prepend the Ipv6 prefix to the Link local address for all clients

@parameters
  struct ps_in6_addr *prefix_addr_ptr - To get the Ipv6 prefix

@return
  bool

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_Backhaul::AssignGlobalV6AddrAllClients
(
  struct ps_in6_addr *prefix_addr_ptr
)
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* nodeInfo = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  struct ps_in6_addr *iid_ptr = NULL;
  struct ps_in6_addr *global_addr_ptr = NULL;
  const void   *dummy = NULL;


  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("AssignGlobalV6AddrAllClients - Linked list head is NULL \n",
                  0, 0, 0);
    return false;
  }

  node = addrList->addrListHead->next;
  if ( node == NULL)
  {
    LOG_MSG_ERROR("AssignGlobalV6AddrAllClients - Linked list node is NULL",
                  0, 0, 0);
    return false;
  }

  /*  Update Global v6 address for all the client with the prefix*/
  while(node != NULL)
  {
    nodeInfo = ( qcmap_cm_client_data_info_t* )ds_dll_data(node);

    iid_ptr = (struct ps_in6_addr *)nodeInfo->link_local_v6_addr;
    global_addr_ptr = (struct ps_in6_addr *)nodeInfo->ip_v6_addr;

    global_addr_ptr->ps_s6_addr64[0] = prefix_addr_ptr->ps_s6_addr64[0];
    global_addr_ptr->ps_s6_addr64[1] = iid_ptr->ps_s6_addr64[1];

    if (QcMapTetheringMgr && ((nodeInfo->device_type ==
        QCMAP_MSGR_DEVICE_TYPE_USB_V01) &&
       (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
       QCMAP_QTI_TETHERED_LINK_PPP)))
    {
      memcpy( QcMapTetheringMgr->ppp_ipv6_addr,
              nodeInfo->ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01 );
      this->AddPPPIPv6Route();
    }
    node = ds_dll_next(node, &dummy);
  }
  return true;
}
/*===========================================================================
  FUNCTION GetIP
==========================================================================*/
/*!
@brief
  Gets IP address and netmask assigned to the STA interface.

@parameters
  uint32 *staIP
  uint32 *netMask
  char* devname

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetIP(uint32 *retIP, uint32 *netMask, char *iface)
{
  int s, ret;
  struct ifreq buffer;
  struct sockaddr *sa;

  if ( retIP == NULL || netMask == NULL )
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return false;
  }

  /* Open a socket */
  s = socket(PF_INET, SOCK_DGRAM, 0);
  if ( s < 0 )
  {
    LOG_MSG_ERROR("Unable to open socket to get IP address.\n",0,0,0);
    return false;
  }
  /* Set up the interface request buffer for wlan0. */
  memset(&buffer, 0x00, sizeof(buffer));
  strlcpy(buffer.ifr_name, iface, IFNAMSIZ);

  /* Call the ioctl to get the address. */
  ret = ioctl(s, SIOCGIFADDR, &buffer);

  if ( ret < 0 )
  {
    LOG_MSG_ERROR("Unable to call ioctl to get IP address.\n",0,0,0);
    /* Close the socket handle. */
    close(s);
    return false;
  }

  /* Copy out the ip address for the interface. */
  sa = (struct sockaddr *)&(buffer.ifr_addr);
  *retIP = ((struct sockaddr_in *)sa)->sin_addr.s_addr;

  /* Set up the interface request buffer for wlan0. */
  memset(&buffer, 0x00, sizeof(buffer));
  strlcpy(buffer.ifr_name, iface, IFNAMSIZ);

  /* Call the ioctl to get the address. */
  ret = ioctl(s, SIOCGIFNETMASK, &buffer);

  if ( ret < 0 )
  {
    LOG_MSG_ERROR("Unable to call ioctl to get netmask.\n",0,0,0);
    /* Close the socket handle. */
    close(s);
    return false;
  }

  /* Copy out the netmask for the interface. */
  sa = (struct sockaddr *)&(buffer.ifr_netmask);
  *netMask = ((struct sockaddr_in *)sa)->sin_addr.s_addr;

  /* Close the socket handle. */
  close(s);

  return true;
}

/*===========================================================================
  FUNCTION ReadBackhaulConfigFromXML
==========================================================================*/
/*!
@brief
  Reads the Backhaul Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/


boolean QCMAP_Backhaul::ReadBackhaulConfigFromXML()
{
pugi::xml_document xml_file;
  pugi::xml_node root,child;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
  strlcpy(this->wan_cfg.eri_config_file, root.child(EriConfig_TAG).child_value(),
          QCMAP_CM_MAX_FILE_LEN);

root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
this->enable_ipv4 = atoi(root.child(EnableIPV4_TAG).child_value());
this->enable_ipv6 = atoi(root.child(EnableIPV6_TAG).child_value());

root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  this->wan_cfg.enable_webserver_wwan_access=
                  atoi(root.child(EnableWebserverWWANAccess_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
  this->tiny_proxy_enable_state= atoi(root.child(TINY_PROXY_Enable_TAG).child_value());
  LOG_MSG_INFO1("Tinyproxy state: %d", this->tiny_proxy_enable_state, 0, 0);

  return true;

}
/*===========================================================================
  FUNCTION SetBackhaulPrioConfigToXML()

  DESCRIPTION
    Helper function that writes Backhaul pref to XML

  PARAMETERS
    qcmap_msgr_backhaul_type_enum_v01 backhaul_type
    cmap_backhaul_config_enum backhaul_config

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
boolean QCMAP_Backhaul::SetBackhaulPrioConfigToXML(
                            qcmap_msgr_backhaul_type_enum_v01 backhaul_type,
                            qcmap_backhaul_config_enum backhaul_config)
{
  boolean ret_value = true;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_ERROR("Set backhaul %d and config %d",backhaul_type,backhaul_config,0);
  switch (backhaul_type)
  {
    case QCMAP_MSGR_WWAN_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,WWAN_BACKHAUL,sizeof(WWAN_BACKHAUL));
    break;
    case QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,USB_CRADLE_BACKHAUL,sizeof(USB_CRADLE_BACKHAUL));
    break;
    case QCMAP_MSGR_WLAN_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                    SET_VALUE,WLAN_BACKHAUL,sizeof(WLAN_BACKHAUL));
    break;
    case QCMAP_MSGR_ETHERNET_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,ETH_BACKHAUL,sizeof(ETH_BACKHAUL));
    break;
    default:
       ret_value = false;
    break;
  }
  return ret_value;
}
/*===========================================================================
  FUNCTION GetSetBackhaulConfigFromXML
==========================================================================*/
/*!
@brief
  Get or set the Backhaul Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul::GetSetBackhaulConfigFromXML
(
  qcmap_backhaul_config_enum conf,
  qcmap_action_type action,
  char *data,
  int datalen
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  const char *tag_ptr;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  switch(conf)
  {
    case CONFIG_ENABLE_IPV4:
      tag_ptr = EnableIPV4_TAG;
    break;

    case CONFIG_ENABLE_IPV6:
      tag_ptr = EnableIPV6_TAG;
    break;

    case CONFIG_WEBSERVER_WWAN_ACCESS:
      tag_ptr = EnableWebserverWWANAccess_TAG;
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);
    break;

    case CONFIG_ERI_CONFIG:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = EriConfig_TAG;
    break;

    case CONFIG_FIRST_PREFFERED_BACKHAUL:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = FirstPreferredBackhaul_TAG;
    break;

    case CONFIG_SECOND_PREFFERED_BACKHAUL:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = SecondPreferredBackhaul_TAG;
    break;

    case CONFIG_THIRD_PREFFERED_BACKHAUL:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = ThirdPreferredBackhaul_TAG;
    break;

  case CONFIG_FOURTH_PREFFERED_BACKHAUL:
      LOG_MSG_ERROR("Fourth backhaul %d, action %d.", CONFIG_FOURTH_PREFFERED_BACKHAUL, action, 0);
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = FourthPreferredBackhaul_TAG;
    break;

    default:
      LOG_MSG_ERROR("Invalid config type.", 0, 0, 0);
     return false;
    break;
  }

  if (action == SET_VALUE)
  {
    root.child(tag_ptr).text() = data;
    LOG_MSG_ERROR("Fourth backhaul %d, action %d data %s.", CONFIG_FOURTH_PREFFERED_BACKHAUL, action, data);
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
    strlcpy(data, root.child(tag_ptr).child_value(), datalen);
  ds_log_med("Set/Get Backhaul config succesful %s %s",tag_ptr,root.child(tag_ptr).child_value());
  return true;
}


/*===========================================================================
  FUNCTION SendRSForClients
==========================================================================*/
/*!
@brief
  Send RS to modem for ipv6 link local address, so that modem replies with
  RA, which will be used to parse and store Global Ipv6 Address.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SendRSForClients(void)
{
  ds_dll_el_t         *node;
  int                 qcmap_cm_error;
  int                 i, num_entries;
  char                devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  char                ipv6addr[INET6_ADDRSTRLEN];

  memset(ipv6addr, 0, INET6_ADDRSTRLEN);

  if(addrList->addrListHead != NULL && addrList->addrListHead->next != NULL)
  {
    //Get number of connected clients
    num_entries = QcMapMgr->conn_device_info.numOfNodes;
    node = ds_dll_next(addrList->addrListHead, (void **) &connectedDevicesList);

    //Send the RS packet to the rmnet iface
    memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
    if ( qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
                               QCMAP_MSGR_IP_FAMILY_V6_V01,
                               devname, &qcmap_cm_error) !=
         QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d\n", qcmap_cm_error,0,0);
      return false;
    }

    //Main loop, send RS for each connected client
    for (i = 0; i < num_entries; i++)
    {
      if(!SendRS(devname, connectedDevicesList->link_local_v6_addr))
      {
        inet_ntop(AF_INET6,
                  (void *)connectedDevicesList->link_local_v6_addr,
                  ipv6addr, INET6_ADDRSTRLEN);
        ds_log_med("Unable to send RS from src %s", ipv6addr);
        return false;
      }
      node = ds_dll_next(node, (void **) &connectedDevicesList);
    }
  }
  else
  {
    LOG_MSG_ERROR("No connected devices", 0,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SendRSForClient
==========================================================================*/
/*!
@brief
  Send RS to backhaul interface for ipv6 link local address, so that backhaul interface replies with
  RA, which will be used to parse and store Global Ipv6 Address.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SendRSForClient(uint8_t *src_v6_addr)
{
  char                devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char                ipv6addr[INET6_ADDRSTRLEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);


  memset(ipv6addr, 0, INET6_ADDRSTRLEN);

  if ( !QCMAP_ConnectionManager::GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01,
       &qmi_err_num))
  {
    return;
  }

  if(!QcMapBackhaulMgr->SendRS(devname, src_v6_addr))
  {
    inet_ntop(AF_INET6,
              (void *)src_v6_addr,
              ipv6addr, INET6_ADDRSTRLEN);
    ds_log_med("Unable to send RS from src %s", ipv6addr);
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION SendRSwithUnspecifiedSrc
==========================================================================*/
/*!
@brief
  Checks source against CDI then sends an unspecified RS out on wlan0.
  This forces external AP to send out a multicast RA.

@parameters
- Devname
- src_v6_addr

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::SendRSwithUnspecifiedSrc(qcmap_nl_addr_t* nl_addr)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  struct nd_opt_hdr *opt_hdr;
  struct ether_addr *source_mac_addr;
  int src_mac_set = 0;
  int packet_size;
  int payload_len;

  LOG_MSG_INFO1("SendUnspecifiedRS",0,0,0);

  //look through packet for mac address
  packet_size = sizeof(struct nd_router_solicit);
  opt_hdr = (struct nd_opt_hdr *) (nl_addr->nd_packet_buf +
                                   sizeof(struct ip6_hdr) +
                                   sizeof(struct nd_router_solicit));

  struct ip6_hdr *hdr;
  hdr = (struct ip6_hdr*) nl_addr->nd_packet_buf;
  payload_len = ntohs(hdr->ip6_plen);

  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0 && payload_len > 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
    {
      source_mac_addr = (struct ether_addr*) (nl_addr->nd_packet_buf +
                                              sizeof(struct ip6_hdr) +
                                              (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
      ds_log_med("src MAC %s", ether_ntoa(source_mac_addr));
      src_mac_set = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
    payload_len -= opt_hdr->nd_opt_len;
  }

  if (src_mac_set && ds_dll_search(addrList->addrListHead,
                                   (void*)source_mac_addr,
                                   QcMapMgr->qcmap_match_mac_address) != NULL)
  {
    QCMAP_Backhaul::SendRS(WLAN0, NULL);
  }
  else
  {
    LOG_MSG_ERROR("Source link address not in RS",0,0,0);
  }
}


/*===========================================================================
  FUNCTION SendRS
==========================================================================*/
/*!
@brief
  Send RS packet out on raw socket. Option to add source address, if not present
  then send out with unspecified address (::).

@parameters
- Devname
- src_v6_addr

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SendRS(char *devname, uint8_t *src_v6_addr)
{
  int                 frame_length, sd, bytes;
  struct ip6_hdr      iphdr;
  struct icmp6_hdr    icmphdr;
  struct sockaddr_ll  device;
  struct sockaddr_in6 dst_addr;
  struct ifreq        ifr;
  uint8               *frame = NULL;
  int status;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  if (src_v6_addr != NULL && !QcMapBackhaulMgr->IsRadishRunning())
  {
    /*Radish takes care of forwarding the RA, if it isn't running then
      sending a RS will do nothing*/
    LOG_MSG_ERROR("Radish is not running, not sending RS",0,0,0);
    return false;
  }

  //set mutlicast router destination
  memset(&dst_addr, 0, sizeof(dst_addr));
  inet_pton(AF_INET6, ROUTER_MULTICAST, &dst_addr.sin6_addr);
  dst_addr.sin6_family = AF_INET6;

  //open RAW socket for sending RS packet
  if ((sd = socket (AF_INET6, SOCK_RAW, IPPROTO_RAW)) < 0) {
    LOG_MSG_ERROR ("socket() failed to get socket descriptor for using ioctl() %d ", perror, 0, 0);
    return false;
  }

  memset(&ifr, 0, sizeof(ifr));
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), devname);
  setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));

  frame = (uint8 *) malloc(FRAME_SIZE);
  if (!frame)
  {
    LOG_MSG_ERROR("No memory %d ", errno, 0, 0);
    close(sd);
    return;
  }

  // IPv6 header
  memset(&iphdr, 0, sizeof(iphdr));

  // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
  iphdr.ip6_flow = htonl ((MAC_SIZE << FLOW_TOTAL) | (0 << FLOW_LABEL) | 0);

  // Payload length (16 bits): ICMP header + ICMP data
  iphdr.ip6_plen = htons (ICMP_HDRLEN);
  iphdr.ip6_nxt = IPPROTO_ICMPV6; //next header
  iphdr.ip6_hops = MAX_HOPS;

  // Source IPv6 address
  if (src_v6_addr == NULL)
  {
    inet_pton(AF_INET6, "::", iphdr.ip6_src.s6_addr);
  }
  else
  {
    memcpy(iphdr.ip6_src.s6_addr, src_v6_addr, sizeof(uint8_t)*QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  }

  // Destination IPv6 address
  if ((status = inet_pton (AF_INET6, ROUTER_MULTICAST, &(iphdr.ip6_dst))) != 1) {
    LOG_MSG_ERROR ("inet_pton failed", 0, 0, 0);
    free(frame);
    close(sd);
    return false;
  }

  // ICMP header
  memset(&icmphdr, 0, sizeof(icmphdr));

  icmphdr.icmp6_type = ND_ROUTER_SOLICIT;
  icmphdr.icmp6_code = MESSAGE_CODE;
  icmphdr.icmp6_cksum = ICMP_HEADER_CHKSUM;
  icmphdr.icmp6_cksum = icmp6_checksum(&iphdr, (struct icmp6_header*)&icmphdr, ICMP_HDRLEN);

  frame_length = IP6_HDRLEN + ICMP_HDRLEN;

  LOG_MSG_INFO1("Entered SendRS at %d", __LINE__, 0, 0);

  // IPv6 header
  memcpy (frame, &iphdr, IP6_HDRLEN * sizeof (uint8));

  // ICMP header
  memcpy (frame + IP6_HDRLEN, &icmphdr, ICMP_HDRLEN * sizeof (uint8));

  // Send ethernet frame to socket.
  if ((bytes = sendto (sd, frame, frame_length * sizeof(uint8), 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr))) <= 0) {
      LOG_MSG_ERROR("Send to failed: %d ", errno, 0, 0);
      free(frame);
      close(sd);
      return false;
  }
  LOG_MSG_INFO1("Sent num_bytes: %d frame length: %d ", bytes, frame_length, 0);

  // Free allocated memory.
  free(frame);
  close(sd);

  return true;
}

/*===========================================================================
  FUNCTION ValidateGlobalV6addr
==========================================================================*/
/*!
@brief
  Validates the prefix present in RA packet

@parameters
  struct ps_in6_addr *prefix_addr_ptr[]
  - Prefix to be validated.

@return
  TRUE - If it is a valid prefix.
  false - If it not.

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::ValidateGlobalV6addr
(
  struct ps_in6_addr *prefix_addr_ptr
)
{
  struct ps_in6_addr *prefix_info_ptr;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  int ret;
  qmi_error_type_v01 qmi_err_num;
  qcmap_cm_nl_prefix_info_t  wan_ipv6_prefix_info;
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("ValidateGlobalV6addr",0,0,0);

  if (!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul is not up, unable to validate prefix",0,0,0);
    return false;
  }

  ret = QCMAP_ConnectionManager::GetDeviceName
                            (devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);
  LOG_MSG_INFO1("Dev type = %d",ret,0,0);

  memset(&wan_ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));

  if (ret != NO_BACKHAUL)
  {
    ds_log_med("devname %d", devname);
  }

  if (ret==BACKHAUL_TYPE_CRADLE &&
      QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
  {
    LOG_MSG_INFO1("Get v6 prefix from cradle",0,0,0);
    /* Get the STA/WWAN prefixes if available. */
    if ( QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      }
      else
      {
        strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
      }
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with STA prefix when in cradle mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ( QcMapTetheringMgr &&
         QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
         QCMAP_QTI_TETHERED_LINK_ETH &&
         QcMapBackhaulEthMgr &&
         QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
    {
      strlcpy(devname,ETH_IFACE, QCMAP_MSGR_INTF_LEN); //Need to add support when bridge mode is activated
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
          &prefix_addr_ptr->ps_s6_addr64[0],
          sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with ETH Backhaul"
                      " prefix when in Cradle mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ((qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
                               QCMAP_MSGR_IP_FAMILY_V6_V01,
                               devname, &qcmap_cm_error)) ==
                               QCMAP_CM_SUCCESS )
    {
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with WWAN Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with WWAN prefix when in cradle mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
  }
  if (ret==BACKHAUL_TYPE_ETHERNET &&
      QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
  {
    if ( QcMapTetheringMgr &&
         QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].
                                   link_type == QCMAP_QTI_TETHERED_LINK_ECM &&
         QcMapBackhaulCradleMgr &&
         (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode ==
                                             QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
          QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode ==
                                     QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01) )
    {
      if ( QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() )
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      else
        strlcpy(devname,ECM_IFACE, QCMAP_MSGR_INTF_LEN);

      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
          &prefix_addr_ptr->ps_s6_addr64[0],
          sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with Cradle prefix"
                      " when in ETH Backhaul mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ( QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      }
      else
      {
        strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
      }
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with STA prefix"
                      " when in ETH Backhaul mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ((qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
                               QCMAP_MSGR_IP_FAMILY_V6_V01,
                               devname, &qcmap_cm_error)) ==
                               QCMAP_CM_SUCCESS )
    {
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with WWAN Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with WWAN prefix"
                      " when in ETH Backhaul mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }

  }
  else if ((QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER ||
            QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE) &&
           QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
  {
    LOG_MSG_INFO1("Get v6 prefix from external AP",0,0,0);
    /* Get the Cradle/WWAN prefixes if available. */
    if ( QcMapTetheringMgr &&
         QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
         QCMAP_QTI_TETHERED_LINK_ECM &&
         QcMapBackhaulCradleMgr &&
         QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 )
    {
      if ( QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() )
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      else
        strlcpy(devname,ECM_IFACE, QCMAP_MSGR_INTF_LEN);

      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
          &prefix_addr_ptr->ps_s6_addr64[0],
          sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with Cradle prefix when in STA mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ( QcMapTetheringMgr &&
         QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
         QCMAP_QTI_TETHERED_LINK_ETH &&
         QcMapBackhaulEthMgr &&
         QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
    {
      strlcpy(devname,ETH_IFACE, QCMAP_MSGR_INTF_LEN); //Need to add support when bridge mode is activated
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
          &prefix_addr_ptr->ps_s6_addr64[0],
          sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with ETH Backhaul"
                      " prefix when in STA mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ((qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
         QCMAP_MSGR_IP_FAMILY_V6_V01, devname, &qcmap_cm_error)) ==
         QCMAP_CM_SUCCESS )
    {
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with WWAN Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with WWAN prefix when in station mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
  }
  else if ((qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
            QCMAP_MSGR_IP_FAMILY_V6_V01, devname, &qcmap_cm_error)) ==
            QCMAP_CM_SUCCESS )
  {
    LOG_MSG_INFO1("Get v6 prefix from WWAN",0,0,0);
    /* Get the STA/Cradle prefixes if available. */
    if (QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
        QCMAP_QTI_TETHERED_LINK_ECM &&
        QcMapBackhaulCradleMgr &&
        QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 )
    {
      if ( QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() )
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      else
        strlcpy(devname,ECM_IFACE, QCMAP_MSGR_INTF_LEN);

      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)\
                       &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with Cradle prefix when in WWAN mode. Ignore RA.\n",
                       0, 0, 0);
        return false;
      }
    }
    if ( QcMapTetheringMgr &&
         QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
         QCMAP_QTI_TETHERED_LINK_ETH &&
         QcMapBackhaulEthMgr &&
         QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
    {
      strlcpy(devname,ETH_IFACE, QCMAP_MSGR_INTF_LEN); //Need to add support when bridge mode is activated
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
          &prefix_addr_ptr->ps_s6_addr64[0],
          sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with ETH Backhaul"
                      " prefix when in WWAN mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ( QcMapWLANMgr &&
         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      }
      else
      {
        strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
      }
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with STA prefix when in WWAN mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
  }
  return true;
}

/*===========================================================================
 FUNCTION ComparePriority
==========================================================================*/
/*!
@brief
  Compares priority of the two given interfaces.

@parameters
  intf1 : First interface
  intf2 : Second Interface

@return
  true : If priority of intf1 is higher than that of intf2
  false : If priority of intf1 is lower than that of intf2
@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::ComparePriority(qcmap_backhaul_type intf1, qcmap_backhaul_type intf2)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  /*Here, we compare the priority based on preffered backhual list.
  In the preffered backhaul list, we have only WWAN, AP-STA or Cradle.
  Hence, both AP-STA Router mode and Bridge mode have been covered
  in a single ENUM (BACKHAUL_TYPE_AP_STA_ROUTER) since at a time
  only one would be active. Therefore, we need to use the same ENUM
  for bridge mode also , for the sake of getting the correct priority*/

  if (intf1 == BACKHAUL_TYPE_AP_STA_BRIDGE)
    intf1 = BACKHAUL_TYPE_AP_STA_ROUTER;

  if (intf2 == BACKHAUL_TYPE_AP_STA_BRIDGE)
    intf2 = BACKHAUL_TYPE_AP_STA_ROUTER;

  LOG_MSG_INFO1("ComparePriority : Comparing priority for %d and %d",intf1,intf2,0);
  if (intf1 == intf2)
  {
    LOG_MSG_INFO1("Both interfaces are same. itf1 = %d, intf2 = %d",intf1,intf2,0);
    return true;
  }
  if (intf2 == NO_BACKHAUL)
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    return true;
  }

  else if (QcMapBackhaulMgr->preffered_backhaul_first == intf1)
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    return true;
  }
  else if ((QcMapBackhaulMgr->preffered_backhaul_second == intf1)
           &&
           ((QcMapBackhaulMgr->preffered_backhaul_third == intf2) ||
           (QcMapBackhaulMgr->preffered_backhaul_fourth == intf2)))
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    return true;
  }
  else if (QcMapBackhaulMgr->preffered_backhaul_third == intf1 &&
           QcMapBackhaulMgr->preffered_backhaul_fourth == intf2)
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    return true;
  }

  LOG_MSG_INFO1("Priority of Intf1 is lower than Intf2",0,0,0);
  return false;
}

/*===========================================================================
  FUNCTION BlockWWANAccessOnIface
==========================================================================*/
/*!
@brief
  This function blocks IPv4/IPv6 Access on provided Iface for MobileAP clients.

@parameters
  ip_vsn : IP type for which Access has to be blocked
  devname : device (interface) for which access has to be blocked

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::BlockWWANAccessOnIface(ip_version_enum_type ip_vsn, char *devname)
{
  char command[MAX_COMMAND_STR_LEN];

  if (ip_vsn == IP_V4)
  {
    /*===========================================================================
    Block IPv4 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking Bridge IPv4 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
    Block IPv4 Access between PPP and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking PPP IPv4 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }

  else if (ip_vsn == IP_V6)
  {
    /*===========================================================================
    Block IPv6 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking Bridge IPv6 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
    Block IPv6 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking PPP IPv6 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }
}

/*===========================================================================
  FUNCTION AllowBackhaulAccessOnIface
==========================================================================*/
/*!
@brief
  This function allows IPv4/IPv6 Access on provided Iface for MobileAP clients.

@parameters
  ip_vsn : IP type for which Access has to be allow
  devname : device (interface) for which access has to be allowed

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::AllowBackhaulAccessOnIface(ip_version_enum_type ip_vsn, char *devname)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                   QCMAP_Backhaul_Ethernet::Get_Instance(false);

  if (ip_vsn == IP_V4)
  {
    /*===========================================================================
    Allow IPv4 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing IPv4 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    if (!(QcMapBackhaulEthMgr
        &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                          QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01))
    {
      /*Add rule to allow access */
      snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
      ds_system_call(command, strlen(command));
    }

    /*===========================================================================
    Allow IPv4 Access between PPP Iface and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing IPv4 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }

  else if (ip_vsn == IP_V6)
  {
    /*===========================================================================
    Allow IPv6 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing Bridge IPv6 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
    Allow IPv4 Access between PPP Iface and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing PPP IPv6 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }
}

/*===========================================================================
  FUNCTION ClearIfaceRelatedRulesforV4
==========================================================================*/
/*!
@brief
  This function blocks IPv4 Access on provided Iface for MobileAP clients.

@parameters
  devname : device (interface) for which access has to be allowed

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::ClearIfaceRelatedRulesForV4(char *devname)
{
  char         command[MAX_COMMAND_STR_LEN];

  /*Delete iface related ip4tables rules*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s ! -o %s -j DROP",
           BRIDGE_IFACE,
           devname);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s ! -o %s -j DROP",
           PPP_IFACE,
           devname);
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION ClearIfaceRelatedRulesforV6
==========================================================================*/
/*!
@brief
  This function blocks IPv6 Access on provided Iface for MobileAP clients.

@parameters
  devname : device (interface) for which access has to be allowed

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::ClearIfaceRelatedRulesForV6(char *devname)
{
  char         command[MAX_COMMAND_STR_LEN];

  /*Delete iface related ip6tables rules*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s ! -o %s -j DROP",
           BRIDGE_IFACE,
           devname);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s ! -o %s -j DROP",
           PPP_IFACE,
           devname);
  ds_system_call(command, strlen(command));
}
/*===========================================================================
  FUNCTION StopDHCPCD
==========================================================================*/
/*!
@brief
  Kills DHCPCD on given Iface

@parameters
  char* iface

@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::StopDHCPCD
(
  char* iface
)
{
  char command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("Killing dhcpcd process.\n",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
            iface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", iface);
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION SendRSOnBridgeIface
==========================================================================*/
/*!
@brief
  Generate a RS Message on bridge iface.

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::SendRSOnBridgeIface(void)
{
  int ret;
  struct nd_router_solicit rs;
  struct sockaddr_in6 dst_addr;
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  LOG_MSG_INFO1("SendRSOnBridgeIface",0,0,0);

  memset(&dst_addr, 0, sizeof(dst_addr));
  memset(&rs, 0, sizeof(nd_router_solicit));

  // Populate icmp6_hdr portion of router solicit struct.
  rs.nd_rs_hdr.icmp6_type = ND_ROUTER_SOLICIT;  // 133 (RFC 4861)
  rs.nd_rs_hdr.icmp6_code = MESSAGE_CODE;              // zero for router solicitation (RFC 4861)
  rs.nd_rs_hdr.icmp6_cksum = htons(0);      // zero when calculating checksum

  inet_pton(AF_INET6, "ff02::1", &dst_addr.sin6_addr);
  dst_addr.sin6_family = AF_INET6;

  if ((ret = sendto(QCMAPLANMgr->bridge_sock, &rs, sizeof(nd_router_solicit), 0,
                           (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
  {
    LOG_MSG_ERROR("Error: Cannot send RS on Bridge IFace error: %d.\n", errno, 0, 0);
  }
}


/*===========================================================================
  FUNCTION ProcessNS
==========================================================================*/
/*!
@brief
  Process the incoming NS packet.

@parameters
  nl_addr

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::ProcessNS(qcmap_nl_addr_t* nl_addr)
{
  struct ps_in6_addr *dst_addr_ptr = NULL, *src_addr_ptr = NULL;
  struct ps_in6_addr null_ipv6_address;
  ds_dll_el_t *dst_node = NULL, *src_node = NULL, *node = NULL;
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  struct nd_opt_hdr *opt_hdr;
  struct ether_addr *source_mac_addr;
  int src_mac_set = 0;
  int packet_size;
  struct timeval sys_time;
  char dst_str[INET6_ADDRSTRLEN], src_str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  qcmap_cm_proxy_data_info_t* proxyInfo = NULL;
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  if (QcMapBackhaulWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN Backhaul is not up", 0, 0, 0);
    return;
  }
  qcmap_addr_info_list_t* proxy_addrList = &(QcMapBackhaulWLANMgr->proxy_addr_list);

  if(cdi_addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: CDI list is empty", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO1("ProcessNS",0,0,0);

  src_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;
  memset(&null_ipv6_address, 0, sizeof(struct ps_in6_addr));
  if (memcmp (src_addr_ptr, &null_ipv6_address, sizeof(struct in6_addr)) == 0)
  {
    LOG_MSG_ERROR("SRC IP is emtpy, do nothing with NS",0,0,0);
    return;
  }

  packet_size = sizeof(struct nd_neighbor_solicit);
  opt_hdr = (struct nd_opt_hdr *) (nl_addr->nd_packet_buf + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_solicit));
  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
    {
      source_mac_addr = (struct ether_addr*) (nl_addr->nd_packet_buf + sizeof(struct ip6_hdr) + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
      ds_log_med("src MAC %s", ether_ntoa(source_mac_addr));
      src_mac_set = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
  }

  if (!src_mac_set)
  {
    LOG_MSG_ERROR("SRC MAC is emtpy, do nothing with NS",0,0,0);
    return;
  }

  src_node = ds_dll_search (cdi_addrList->addrListHead , (void*) source_mac_addr,
                              QcMapMgr->qcmap_match_mac_address);

  dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ipv6_dst_addr;
  tempPrefixIidptr.iid_ptr = dst_addr_ptr;
  dst_node = ds_dll_search (cdi_addrList->addrListHead , (void*)&tempPrefixIidptr,
                              QcMapMgr->qcmap_match_v6_iid);

  if ( dst_node == NULL || src_node == NULL )
  {
    if (memcmp(src_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
        memcmp(src_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
        memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
        memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0)
    {
      LOG_MSG_ERROR("Source or destination of bridge, do nothing",0,0,0);
      return;
    }

    /* From WLAN1 client to WLAN0 client */
    LOG_MSG_INFO2("Send NS to WLAN0 with bridge MAC",0,0,0);

    //add to table
    if (proxy_addrList->addrListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
             information. This is done to make use of the doubly linked list framework which
             is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for node",
                      0,0,0);
        return;
      }
      proxy_addrList->addrListHead = node;
    }

    if (gettimeofday(&sys_time, 0) != 0)
    {
      LOG_MSG_ERROR("Gettimeofday() failed", 0, 0, 0);
      return;
    }

    proxyInfo = (qcmap_cm_proxy_data_info_t*)ds_malloc(sizeof(qcmap_cm_proxy_data_info_t));
    if( proxyInfo == NULL )
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for"
                    "device information structure",0,0,0);
      return;
    }

    memset(proxyInfo,0,sizeof(qcmap_cm_proxy_data_info_t));
    memcpy((struct ps_in6_addr*) proxyInfo->dst_ip_v6_addr, dst_addr_ptr, sizeof(ps_in6_addr));
    memcpy((struct ps_in6_addr*) proxyInfo->src_ip_v6_addr, src_addr_ptr, sizeof(ps_in6_addr));
    memcpy(proxyInfo->src_mac_addr, source_mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
    proxyInfo->time = sys_time.tv_sec;

    //check if entry already exists
    if((node = ds_dll_search (proxy_addrList->addrListHead , (void*) proxyInfo,
                                QcMapMgr->qcmap_match_proxy_entry)) != NULL)
    {
      LOG_MSG_ERROR("Entry already exists, do not add",0,0,0);
      ds_free(proxyInfo);
      proxyInfo = NULL;

      /* update the time */
      proxyInfo = ( qcmap_cm_proxy_data_info_t* )ds_dll_data(node);
      proxyInfo->time = sys_time.tv_sec;
    }
    //Store the mac address, interface name in the linked list
    else if ((node = ds_dll_enq(proxy_addrList->addrListHead,
                           NULL, (void*)proxyInfo )) == NULL)
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in adding a node",0,0,0);
      ds_free(proxyInfo);
      proxyInfo = NULL;
    }
    else
    {
      /* Success New node added! */
      inet_ntop(AF_INET6, proxyInfo->dst_ip_v6_addr, dst_str, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6, proxyInfo->src_ip_v6_addr, src_str, INET6_ADDRSTRLEN);
      ds_log_med("New proxy entry with dst addr %s, src addr %s", dst_str, src_str);

      //Update the tail pointer when a new node is added everytime
      proxy_addrList->addrListTail = node;
    }
    /* Even if the entry already exists or we can't add node to LL we still need
       to send the NS*/
    QcMapBackhaulMgr->SendProxyNS(nl_addr->nd_packet_buf);
  }
  else
  {
    LOG_MSG_ERROR("CDI client to CDI client, do nothing",0,0,0);
  }
}


/*===========================================================================
  FUNCTION ProcessNA
==========================================================================*/
/*!
@brief
  Process the incoming NA packet.

@parameters
  nl_addr

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::ProcessNA(qcmap_nl_addr_t* nl_addr)
{
  ds_dll_el_t *node = NULL, *temp_node = NULL;
  struct ps_in6_addr *src_addr_ptr = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  char dst_str[INET6_ADDRSTRLEN], src_str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  qcmap_cm_proxy_data_info_t *proxyInfo = NULL, *temp_proxyInfo = NULL;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  if (QcMapBackhaulWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN Backhaul is not up", 0, 0, 0);
    return false;
  }

  qcmap_addr_info_list_t* proxy_addrList = &(QcMapBackhaulWLANMgr->proxy_addr_list);

  if(cdi_addrList->addrListHead == NULL || proxy_addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: Lists are empty", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("ProcessNA",0,0,0);

  /* Get dst address */
  src_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;
  dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ipv6_dst_addr;

  node = proxy_addrList->addrListHead;
  node = ds_dll_next (node, (const void**)(&proxyInfo));

  inet_ntop(AF_INET6, src_addr_ptr, dst_str, INET6_ADDRSTRLEN);
  inet_ntop(AF_INET6, dst_addr_ptr, src_str, INET6_ADDRSTRLEN);
  ds_log_med("Searching for proxy entry with dst addr %s, src addr %s", dst_str, src_str);

  while (node != NULL)
  {
    inet_ntop(AF_INET6, proxyInfo->dst_ip_v6_addr, src_str, INET6_ADDRSTRLEN);
    ds_log_med("Proxy entry in list with src addr %s", src_str);

    /* We should only proxy NA's which are destined for bridge */
    if ((memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
         memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0) &&
         memcmp(src_addr_ptr, proxyInfo->dst_ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0)
    {
      QcMapBackhaulMgr->SendProxyNA( (struct ps_in6_addr*) proxyInfo->src_ip_v6_addr, nl_addr->nd_packet_buf, proxyInfo->src_mac_addr);

      /* save the next node before deleting current node if we are not at the tail */
      temp_node = ds_dll_next (node, (const void**)(&temp_proxyInfo));

      /* Delete node from list */
      ds_dll_delete_node(proxy_addrList->addrListHead, &(proxy_addrList->addrListTail), node);

      //Free the proxyInfo entry structure
      if (proxyInfo != NULL)
      {
        ds_free (proxyInfo);
        proxyInfo = NULL;
      }

      //Free the memory of the linked list node
      ds_dll_free(node);

      if (proxy_addrList->addrListHead->next == NULL)
      {
        LOG_MSG_INFO1("No more entries",0,0,0);
        ds_dll_free(proxy_addrList->addrListHead);
        proxy_addrList->addrListHead = NULL;
        break;
      }
      node = temp_node;
      proxyInfo = temp_proxyInfo;
    }
    else
    {
      node = ds_dll_next (node, (const void**)(&proxyInfo));
    }
  }
}


/*===========================================================================
  FUNCTION SendProxyNS
==========================================================================*/
/*!
@brief
  Generate a NS Message with bridge's MAC and send to dst.

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::SendProxyNS(char *nd_packet_buf)
{
  int ret, source_mac_changed = 0;
  int packet_size = 0;
  struct ip6_hdr *ipv6_packet;
  struct nd_opt_hdr *opt_hdr;
  struct icmp6_hdr *icmp6;
  struct sockaddr_in6 dst_addr;
  struct ether_addr bridge_mac_addr;
  struct ether_addr *target_mac_addr;
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  LOG_MSG_INFO1("SendProxyNS",0,0,0);

  icmp6 = (struct icmp6_hdr *) (nd_packet_buf + sizeof(struct ip6_hdr));

  memset(&dst_addr, 0, sizeof(dst_addr));
  ipv6_packet = (struct ip6_hdr *)nd_packet_buf;

  memcpy(&dst_addr.sin6_addr.s6_addr, (in6_addr *) ipv6_packet->ip6_dst.s6_addr, sizeof(in6_addr));
  dst_addr.sin6_family = AF_INET6;

  if( QCMAP_CM_ERROR == QcMapMgr->GetHWAddr((char*) &bridge_mac_addr.ether_addr_octet,
                        BRIDGE_IFACE))
  {
    LOG_MSG_ERROR("Error in fetching Bridge MAC",0,0,0);
    return;
  }

  packet_size = sizeof(struct nd_neighbor_solicit);
  opt_hdr = (struct nd_opt_hdr *) (nd_packet_buf + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_solicit));

  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
    {
      target_mac_addr = (struct ether_addr*) (nd_packet_buf + sizeof(struct ip6_hdr) + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
      ds_log_med("change %s to bridge MAC %s", ether_ntoa(target_mac_addr), ether_ntoa(&bridge_mac_addr));
      memcpy(target_mac_addr, &bridge_mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
      source_mac_changed = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
  }

  if (!source_mac_changed)
  {
    LOG_MSG_ERROR("source mac not present in NS packet",0,0,0);
    return;
  }

  /* explicitly set packet size from ip packet */
  packet_size = ntohs(ipv6_packet->ip6_plen);

  if ((ret = sendto(QCMAPLANMgr->bridge_sock, icmp6, packet_size, 0,
                           (struct sockaddr *)&dst_addr, sizeof(dst_addr))) < 0)
  {
    LOG_MSG_ERROR("Error: Cannot send NS on bridge Iface error: %d.\n", errno, 0, 0);
  }
}

/*===========================================================================
  FUNCTION SendProxyNA
==========================================================================*/
/*!
@brief
  Generate a NA Message with bridge's MAC and send to dst.

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::SendProxyNA(struct ps_in6_addr *dst_v6_addr, char *nd_packet_buf, uint8 *src_mac_addr)
{
  int ret;
  int target_mac_changed = 0;
  int packet_size = 0;
  struct nd_opt_hdr *opt_hdr;
  struct icmp6_hdr *icmp6;
  struct ip6_hdr *ipv6_packet;
  struct ether_addr bridge_mac_addr;
  struct sockaddr_in6 dst_addr;
  char str[INET6_ADDRSTRLEN];
  char *packet_buff_cpy;
  struct ether_addr *target_mac_addr;
  ds_dll_el_t *dst_node = NULL, *src_node = NULL;
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  LOG_MSG_INFO1("SendProxyNA",0,0,0);

  packet_buff_cpy = (char*) ds_malloc(MAX_ND_PKT_SIZE);
  if(packet_buff_cpy == NULL)
  {
    LOG_MSG_ERROR("Error in allocating packet buffer", 0, 0, 0);
    return;
  }
  memcpy(packet_buff_cpy, nd_packet_buf, MAX_ND_PKT_SIZE);

  icmp6 = (struct icmp6_hdr *) (packet_buff_cpy + sizeof(struct ip6_hdr));
  ipv6_packet = (struct ip6_hdr *)nd_packet_buf;

  memset(&dst_addr, 0, sizeof(struct sockaddr_in6));
  memcpy(&dst_addr.sin6_addr, dst_v6_addr, sizeof(struct ps_in6_addr));
  dst_addr.sin6_family = AF_INET6;

  opt_hdr = (struct nd_opt_hdr *) (packet_buff_cpy + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_advert));

  inet_ntop(AF_INET6, dst_v6_addr, str, INET6_ADDRSTRLEN);
  ds_log_med("Sending Proxy NA to %s", str);

  packet_size = sizeof(struct nd_neighbor_advert);
  opt_hdr = (struct nd_opt_hdr *) (packet_buff_cpy + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_advert));

  if( QCMAP_CM_ERROR == QcMapMgr->GetHWAddr((char*) &bridge_mac_addr.ether_addr_octet,
                        BRIDGE_IFACE))
  {
    LOG_MSG_ERROR("Error in fetching Bridge MAC",0,0,0);
    ds_free(packet_buff_cpy);
    return;
  }

  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_TARGET_LINKADDR)
    {
      target_mac_addr = (struct ether_addr*) (packet_buff_cpy + sizeof(struct ip6_hdr) + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));

      /* Check if both MAC address are in CDI, if so do not send Proxy NA
         This will hapen when tmp global addresses are used as they are not in CDI*/
      src_node = ds_dll_search (cdi_addrList->addrListHead , (void*) target_mac_addr,
                                QcMapMgr->qcmap_match_mac_address);
      dst_node = ds_dll_search (cdi_addrList->addrListHead , (void*) src_mac_addr,
                                QcMapMgr->qcmap_match_mac_address);

      if (src_node != NULL && dst_node != NULL)
      {
        LOG_MSG_ERROR("ND between local clients",0,0,0);
        break;
      }

      ds_log_med("change bridge mac %s to target MAC %s", ether_ntoa(target_mac_addr), ether_ntoa((struct ether_addr *) &bridge_mac_addr));
      memcpy(target_mac_addr, &bridge_mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
      target_mac_changed = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
  }

  if (!target_mac_changed)
  {
    LOG_MSG_ERROR("source mac not changed",0,0,0);
    ds_free(packet_buff_cpy);
    return;
  }

  packet_size = ntohs(ipv6_packet->ip6_plen);

  if ((ret = sendto(QCMAPLANMgr->bridge_sock, icmp6, packet_size, 0,
                           (struct sockaddr *)&dst_addr, sizeof(dst_addr))) < 0)
  {
    LOG_MSG_ERROR("Error: Cannot send NA on bridge Iface error: %d.\n", errno, 0, 0);
  }

  ds_free(packet_buff_cpy);
  ds_log_med("Proxy NA sent");
}


/*===========================================================================
  FUNCTION AddV6ExternalRoute
==========================================================================*/
/*!
@brief
  Add destination based route.

@parameters
  nl_addr
  iface

@return
  void

@note

 - Dependencies: AP-STA/Cradle v6 router mode
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::AddV6ExternalRoute(qcmap_nl_addr_t* nl_addr, char* iface)
{
  char external_ip[INET6_ADDRSTRLEN];
  char prefix_str[INET6_ADDRSTRLEN];
  char command[MAX_COMMAND_STR_LEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t *node_data = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  struct sockaddr_in6 multi_dst;
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList;

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("Backhaul is not enabled",0,0,0);
    return;
  }

  inet_pton(AF_INET6, "ff02::1", &multi_dst.sin6_addr);
  if (memcmp(nl_addr->ipv6_dst_addr, &multi_dst.sin6_addr, sizeof(multi_dst.sin6_addr)) == 0)
  {
    LOG_MSG_ERROR("NA is multicast, do nothing", 0,0,0);
    return;
  }

  if (strncmp(iface, nl_addr->iface_name, strlen(iface)) != 0)
  {
    ds_log_med("AddV6ExternalRoute: Error NA is not on correct iface %s %s", iface, nl_addr->iface_name);
    return;
  }

  /* In Dual Wifi WLAN0 is not always the STA iface */
  snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);

  if ((strncmp(iface, sta_iface, strlen(sta_iface)) == 0 && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) ||
      (strncmp(iface, ECM_IFACE, strlen(ECM_IFACE)) == 0 && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)||
      (strncmp(iface, ETH_IFACE, strlen(ETH_IFACE)) == 0 && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET))
  {
    if (memcmp(&(((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr),
                nl_addr->ip_v6_addr, sizeof(nl_addr->ip_v6_addr)/2) != 0)
    {
      inet_ntop(AF_INET6, &(nl_addr->ip_v6_addr), external_ip, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr,
                prefix_str, INET6_ADDRSTRLEN);
      ds_log_med("AddV6ExternalRoute: error invalid prefix %s, %s",external_ip,prefix_str);
      return;
    }

    addrList = &(QcMapBackhaulMgr->external_client_addr_list);

    /* Add IP to linked list for cleanup */
    if (addrList->addrListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
             information. This is done to make use of the doubly linked list framework which
             is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for node",
                      0,0,0);
        return;
      }
      addrList->addrListHead = node;
    }

    node_data = (qcmap_cm_client_data_info_t*)ds_malloc(sizeof(qcmap_cm_client_data_info_t));
    if( node_data == NULL )
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for"
                    "device information structure",0,0,0);
      return;
    }
    memset(node_data,0,sizeof(qcmap_cm_client_data_info_t));
    memcpy(node_data->ip_v6_addr, nl_addr->ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;
    tempPrefixIidptr.iid_ptr = dst_addr_ptr;

    //check if entry already exists
    if((node = ds_dll_search (addrList->addrListHead , (void*) &tempPrefixIidptr,
                              QcMapMgr->qcmap_match_v6_iid)) != NULL)
    {
      LOG_MSG_ERROR("Entry already exists, do not add",0,0,0);
      ds_free(node_data);
      node_data = NULL;
      return;
    }
    //Store the src address
    else if ((node = ds_dll_enq(addrList->addrListHead,
                           NULL, (void*)node_data )) == NULL)
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in adding a node",0,0,0);
      ds_free(node_data);
      node_data = NULL;
      return;
    }
    //Update the tail pointer when a new node is added everytime
    addrList->addrListTail = node;

    inet_ntop(AF_INET6, &(nl_addr->ip_v6_addr), external_ip, INET6_ADDRSTRLEN);
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route add %s dev %s",external_ip, iface);
    ds_system_call(command, strlen(command));

    ds_log_med("New destination route for %s", external_ip);
  }
  else
  {
    ds_log_med("Incorrect iface %s for backhaul",iface);
  }
}


/*===========================================================================
  FUNCTION DeleteV6ExternalRoute
==========================================================================*/
/*!
@brief
  Delete destination based route.

@parameters
  nl_addr
  iface

@return
  void

@note

 - Dependencies: AP-STA/Cradle v6 router mode
*/
/*=========================================================================*/
void QCMAP_Backhaul::DeleteV6ExternalRoute(uint8_t* v6_addr, char* iface)
{
  char command[MAX_COMMAND_STR_LEN];
  char v6_ip_addr[INET6_ADDRSTRLEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t *node_data = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList;

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("Backhaul is not enabled",0,0,0);
    return;
  }


  snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);
  if ((strncmp(iface, sta_iface, strlen(sta_iface)) == 0 && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) ||
      (strncmp(iface, ECM_IFACE, strlen(ECM_IFACE)) == 0 && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE) ||
      (strncmp(iface, ECM_IFACE, strlen(ETH_IFACE)) == 0 && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET))
  {
    addrList = &(QcMapBackhaulMgr->external_client_addr_list);

    dst_addr_ptr = (struct ps_in6_addr *)v6_addr;
    tempPrefixIidptr.iid_ptr = dst_addr_ptr;

    /* Delete the route */
    inet_ntop(AF_INET6, v6_addr, v6_ip_addr, INET6_ADDRSTRLEN);

    //Delete the node from the list
    if ((node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),
         (void*)&tempPrefixIidptr, QcMapMgr->qcmap_match_v6_addr))== NULL)
    {
      ds_log_med("DeleteDeviceEntryInfo - Error in finding the node %s on %s",v6_ip_addr,iface);
      return false;
    }

    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s dev %s",v6_ip_addr, iface);
    ds_system_call(command, strlen(command));

    node_data = (qcmap_cm_client_data_info_t*)ds_dll_data(node);

    if (node_data == NULL)
    {
      LOG_MSG_ERROR("node is NULL",0,0,0);
      //Free the memory of the linked list node
      ds_dll_free(node);
      node = NULL;
      return false;
    }

    //Free the device information structure
    ds_free (node_data);
    node_data = NULL;

    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;

    // Add logic to check if there are no more nodes in the list and free the head
    if ( addrList->addrListHead->next == NULL)
    {
      LOG_MSG_INFO1("\n No device info exist\n",0,0,0);
      ds_dll_free(addrList->addrListHead);
      addrList->addrListHead = NULL;
    }
  }
  else
  {
    ds_log_med("Incorrect iface %s for backhaul",iface);
  }
}

/*===========================================================================
  FUNCTION DeleteAllV6ExternalRoute
==========================================================================*/
/*!
@brief
  Remove all destination based routes.

@parameters
  iface

@return
  void

@note

 - Dependencies: AP-STA/Cradle v6 router mode
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::DeleteAllV6ExternalRoute(char* iface)
{
  char command[MAX_COMMAND_STR_LEN];
  char v6_ip_addr[INET6_ADDRSTRLEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  ds_dll_el_t *node, *temp_node;
  qcmap_cm_client_data_info_t *node_data = NULL, *temp_data;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList;

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("Backhaul is not enabled",0,0,0);
    return;
  }

  LOG_MSG_INFO1("DeleteAllV6ExternalRoute: delete all routes",0,0,0);
  snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->ap_dev_num1);

  if ((strncmp(iface, sta_iface, strlen(sta_iface)) == 0 &&
       QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) ||
      (strncmp(iface, ECM_IFACE, strlen(ECM_IFACE)) == 0 &&
       QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE))
  {

    addrList = &(QcMapBackhaulMgr->external_client_addr_list);

    if (addrList->addrListHead == NULL)
    {
      LOG_MSG_ERROR("Error: List is empty", 0, 0, 0);
      return false;
    }

    node = addrList->addrListHead;
    node = ds_dll_next (node, (const void**)(&node_data));

    while (node != NULL)
    {
      /* Delete the route */
      inet_ntop(AF_INET6, &(node_data->ip_v6_addr), v6_ip_addr, INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s dev %s",v6_ip_addr, iface);
      ds_system_call(command, strlen(command));

      temp_node = ds_dll_next (node, (const void**)(&temp_data));

      /* Delete node from list */
      ds_dll_delete_node(addrList->addrListHead, &(addrList->addrListTail), node);

      //Free the proxyInfo entry structure
      if (node_data != NULL)
      {
        ds_free (node_data);
        node_data = NULL;
      }

      //Free the memory of the linked list node
      ds_dll_free(node);

      if (addrList->addrListHead->next == NULL)
      {
        LOG_MSG_INFO1("No more entries",0,0,0);
        ds_dll_free(addrList->addrListHead);
        addrList->addrListHead = NULL;
        break;
      }
      node = temp_node;
      node_data = temp_data;
    }
  }
  else
  {
    ds_log_med("Incorrect iface %s for backhaul",iface);
  }
}

/*==========================================================================
  FUNCTION JoinMulticastGroup
==========================================================================*/
/*!
@brief
 Add iface to the Multicast group

@parameters
  iface
/*=========================================================================*/
void QCMAP_Backhaul::JoinMulticastGroup
(
  qcmap_v6_sock_iface_t iface_type,
  char* iface
)
{
  static int v6sock_bridge = -1, v6sock_ecm = -1, v6sock_wlan = -1;
  static int v6sock_eth = -1, iface_name_sockfd;
  struct ifreq req_if_info;
  struct ipv6_mreq mreq;
  static struct in6_addr all_routers_in6_addr;
  int *curr_v6_sockfd = NULL;

  if (iface_type < QCMAP_V6_SOCK_BRIDGE || iface_type > QCMAP_V6_SOCK_ECM)
  {
    LOG_MSG_ERROR( "Unsupported iface_type = %d\n", iface_type, 0, 0);
    return;
  }

  iface_name_sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
  if (iface_name_sockfd < 0)
  {
    LOG_MSG_ERROR("Open iface_name_sockfd data sock error = %d\n",
                  iface_name_sockfd, 0, 0);
    return;
  }

  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, iface, sizeof(req_if_info.ifr_name));

  if (ioctl(iface_name_sockfd, SIOCGIFINDEX, &req_if_info) < 0 )
  {
    LOG_MSG_ERROR("Couldn't get index. err %d\n", errno, 0, 0);
    close( iface_name_sockfd );
    return;
  }

  close( iface_name_sockfd );

  if (iface_type == QCMAP_V6_SOCK_BRIDGE)
  {
    curr_v6_sockfd = &v6sock_bridge;
  }
  else if (iface_type == QCMAP_V6_SOCK_WLAN)
  {
    curr_v6_sockfd = &v6sock_wlan;
  }
  else if (iface_type == QCMAP_V6_SOCK_ETH)
  {
     curr_v6_sockfd = &v6sock_eth;
  }
  else
  {
    curr_v6_sockfd = &v6sock_ecm;
  }

  if (*curr_v6_sockfd >=0)
  {
    close(*curr_v6_sockfd);
    *curr_v6_sockfd = -1;
  }

  *curr_v6_sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW);
  if (*curr_v6_sockfd < 0)
  {
    LOG_MSG_ERROR("Error creating raw socket %d",*curr_v6_sockfd, 0, 0);
    return;
  }

  inet_pton(AF_INET6, MULTICAST_ADDR, &all_routers_in6_addr);

  memset(&mreq, 0, sizeof(mreq));
  mreq.ipv6mr_multiaddr = all_routers_in6_addr;
  mreq.ipv6mr_interface = req_if_info.ifr_ifindex;

  if (setsockopt(*curr_v6_sockfd,
                 IPPROTO_IPV6,
                 IPV6_ADD_MEMBERSHIP,
                 &mreq,
                 sizeof(mreq)) < 0)
  {
    LOG_MSG_ERROR("Error joining MC group",0,0,0);
  }
}

/*===========================================================================
  FUNCTION SwitchToWWANBackhaul
==========================================================================*/
/*!
@brief
  Switches current backhaul to WWAN

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
  */
  /*=========================================================================*/
void QCMAP_Backhaul::SwitchToWWANBackhaul()
{
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;

  if (NULL != QcMapBackhaulMgr)
    QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_WWAN;

  if (NULL != QcMapBackhaulWWANMgr)
    QcMapBackhaulWWANMgr->AddWWANIPv4SIPServerInfo();

  if ( qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED )
  {
    if(QcMapNatAlg)
      QcMapNatAlg->EnableNATonApps();

    if (QCMAPLANMgr)
      QCMAPLANMgr->EnableDNS();

    //Get IP Passthrough Configuration from XML
    if(QCMAPLANMgr->GetIPPassthroughConfig
      (&enable_state,&passthrough_config,&qmi_err_num))
    {
      if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
      {
        QCMAPLANMgr->RestartTetheredClient(passthrough_config.device_type);
      }
    }
    else
    {
      LOG_MSG_ERROR("Get IP Passthrough Flag failed\n",0,0,0);
    }

  }
  if ( qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED )
  {
    if(QcMapBackhaulMgr)
      QcMapBackhaulMgr->EnableIPV6Forwarding();

    if(QcMapFirewall)
      QcMapFirewall->EnableIPV6Firewall();
  }
  /* Disable IPv4 MCAST */
  LOG_MSG_INFO1(" Stop PIMD to disable MCAST forwarding on STA interface.\n",0,0,0);
  if (QcMapMgr)
    QcMapMgr->StopMcastDaemon();
}

/*===========================================================================
  FUNCTION AddIPv6DefaultRouteInfo
==========================================================================*/
/*!
@brief
  Cache default gateway information from RA received.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
/*=========================================================================*/
void QCMAP_Backhaul::AddIPv6DefaultRouteInfo(qcmap_nl_addr_t *nladdr)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                  QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  char ip6_addr[INET6_ADDRSTRLEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  qcmap_msgr_eth_backhaul_mode_v01 eth_mode =
                                     QCMAP_MSGR_ETH_BACKHAUL_LAN_ROUTER_V01;
  qmi_error_type_v01 qmi_err_num;

  inet_ntop(AF_INET6, &nladdr->ipv6_src_addr, ip6_addr, INET6_ADDRSTRLEN);
  ds_log_med("QCMAP_Backhaul::AddIPv6DefaultRouteInfo iface %s gateway %s \n",
               nladdr->iface_name, ip6_addr);

  snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->ap_dev_num1);

  if ((strncmp(nladdr->iface_name, sta_iface, strlen(sta_iface)) == 0) &&
       QcMapBackhaulWLANMgr &&
       !QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if (!memcmp(&QcMapBackhaulWLANMgr->ipv6_gateway_addr.s6_addr,
          &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }

    memcpy(&QcMapBackhaulWLANMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else if ((strncmp(nladdr->iface_name, ECM_IFACE, strlen(ECM_IFACE)) == 0) &&
            QcMapBackhaulCradleMgr &&
            !QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
  {
    QcMapBackhaulCradleMgr->GetCradleMode(&cradle_mode,&qmi_err_num);
    if (cradle_mode != QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
    {
      LOG_MSG_INFO1("Ignoring RA - we're not in Cradle WAN Router mode current mode %d",
                     cradle_mode,0,0);
      return;
    }

    if (!memcmp(&QcMapBackhaulCradleMgr->ipv6_gateway_addr.s6_addr,
                  &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }

    memcpy(&QcMapBackhaulCradleMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else if ((strncmp(nladdr->iface_name, ETH_IFACE, strlen(ETH_IFACE)) == 0) &&
            QcMapBackhaulEthMgr)
  {
    QcMapBackhaulEthMgr->GetEthBackhaulMode(&eth_mode,&qmi_err_num);
    if (eth_mode != QCMAP_MSGR_ETH_BACKHAUL_WAN_ROUTER_V01)
    {
      LOG_MSG_INFO1("Ignoring RA - we're not in ETH Backhaul"
                    " WAN Router mode current mode %d",
                     eth_mode,0,0);
      return;
    }

    if (!memcmp(&QcMapBackhaulEthMgr->ipv6_gateway_addr.s6_addr,
                  &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }
    LOG_MSG_INFO1("Store %d for eth backhaul GW addr",
                     &nladdr->ipv6_src_addr,0,0);
    memcpy(&QcMapBackhaulEthMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else
  {
    ds_log_med("QCMAP_Backhaul::AddIPv6DefaultRouteInfo -Rcvd RA in incorrect mode %d",
                 QcMapBackhaulMgr->current_backhaul);
  }
}

/*=====================================================
  FUNCTION SwitchToOtherBackhaul
======================================================*/
/*!
@brief
 Switches to other backhaul from the specified backhaul.

@parameters
  qcmap_backhaul_type backhaul_type
  boolean allow_ipv4
  boolean allow_ipv6

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Backhaul::SwitchToOtherBackhaul(
                                        qcmap_backhaul_type backhaul_type,
                                        boolean allow_ipv4,
                                        boolean allow_ipv6)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                            QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                            QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=
                                QCMAP_Backhaul_WLAN::Get_Instance(false);
  qcmap_backhaul_list_type backhaul_prio_list[MAX_BACKAUL_COUNT];
  qmi_error_type_v01 qmi_err_num;
  int index = 0;
/*------------------------------------------------------------------------*/
  /* This needs to be optimized by storing the prio list in array in
     backhaul class*/
  backhaul_prio_list[0].backhaul_name =
                                    QCMAP_Backhaul::preffered_backhaul_first;
  backhaul_prio_list[1].backhaul_name =
                                    QCMAP_Backhaul::preffered_backhaul_second;
  backhaul_prio_list[2].backhaul_name =
                                    QCMAP_Backhaul::preffered_backhaul_third;
  backhaul_prio_list[3].backhaul_name =
                                    QCMAP_Backhaul::preffered_backhaul_fourth;


  for (int i =0; i < MAX_BACKAUL_COUNT; i++)
  {
    switch(backhaul_prio_list[i].backhaul_name)
    {
      case BACKHAUL_TYPE_WWAN:
        backhaul_prio_list[i].backhaul_availale =
           (allow_ipv4 &&
                qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED) ||
           (allow_ipv6 &&
            qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED);
      break;
      case BACKHAUL_TYPE_AP_STA_ROUTER:
         backhaul_prio_list[i].backhaul_availale =
                        (allow_ipv4 && QCMAP_Backhaul_WLAN::IsSTAAvailableV4())
                        ||
                        (allow_ipv6 && QCMAP_Backhaul_WLAN::IsSTAAvailableV6());
      break;
      case BACKHAUL_TYPE_CRADLE:
         backhaul_prio_list[i].backhaul_availale =
            (allow_ipv4 && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4())
              ||
            (allow_ipv6 && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6());
      break;
      case BACKHAUL_TYPE_ETHERNET:
        backhaul_prio_list[i].backhaul_availale =
            (allow_ipv4 && QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
            ||
            (allow_ipv6 && QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6());
      break;
      default:
        LOG_MSG_ERROR("SwitchToOtherBackhaul:"
                   "Invalid backhaul name.",0,0,0);
        return;
      break;
    }
  }

  while (index != MAX_BACKAUL_COUNT)
  {
     if (backhaul_type != backhaul_prio_list[index].backhaul_name)
     {
        if (backhaul_prio_list[index].backhaul_availale)
        {
        LOG_MSG_ERROR("SwitchToOtherBackhaul:Switch to %d backhaul\n",
                      backhaul_prio_list[index].backhaul_name,0,0);
          QCMAP_Backhaul::SwitchToBackhaul(
                             backhaul_prio_list[index].backhaul_name);
          return;
        }
     }
     index++;
  }
  LOG_MSG_ERROR("SwitchToOtherBackhaul:No Available Backhaul\n",
                      0,0,0);
  QcMapBackhaulMgr->current_backhaul = NO_BACKHAUL;
  return;
}
/*=====================================================
  FUNCTION SwitchToBackhaul
======================================================*/
/*!
@brief
 Switches to the specified backhaul.

@parameters
  qcmap_backhaul_type backhaul_type

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
boolean QCMAP_Backhaul::SwitchToBackhaul(
                                        qcmap_backhaul_type backhaul_type)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                            QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                            QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=
                                QCMAP_Backhaul_WLAN::Get_Instance(false);
  boolean return_value = false;
/*------------------------------------------------------------------------*/
  LOG_MSG_ERROR("SwitchToBackhaul:"
                   "Switch to %d backhaul_type.",backhaul_type,0,0);
  switch(backhaul_type)
  {
    case BACKHAUL_TYPE_WWAN:
      if (QcMapBackhaulMgr)
      {
        QcMapBackhaulMgr->SwitchToWWANBackhaul();
        return_value = true;
      }
    break;
    case BACKHAUL_TYPE_AP_STA_BRIDGE:
    case BACKHAUL_TYPE_AP_STA_ROUTER:
      if (QcMapBackhaulWLANMgr) {
        QcMapBackhaulWLANMgr->SwitchToAPSTABackhaul();
        return_value = true;
      }
    break;
    case BACKHAUL_TYPE_CRADLE:
      if(QcMapBackhaulCradleMgr)
      {
        QcMapBackhaulCradleMgr->SwitchToCradleBackhaul();
        return_value = true;
      }
    break;
    case BACKHAUL_TYPE_ETHERNET:
      if (QcMapBackhaulEthMgr)
      {
        QcMapBackhaulEthMgr->SwitchToEthBackhaul();
        return_value = true;
      }
    break;
    default:
      LOG_MSG_ERROR("SwitchToOtherBackhaul:"
                 "Invalid backhaul name.",0,0,0);
    break;
  }
  return return_value;
}
