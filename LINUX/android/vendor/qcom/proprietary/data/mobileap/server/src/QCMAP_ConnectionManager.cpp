/*====================================================

FILE:  QCMAP_ConnectionManager.cpp

SERVICES:
   QCMAP Connection Manager Implementation

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/11/12   gk         9x25
  10/26/12   cp         Added support for Dual AP and different types of NAT.
  12/19/12   sb         Added support for RNDIS/ECM USB tethering.
  02/01/13   cp         Minor IPv6 changes.
  02/27/13   cp         Added support for deprecating of prefix when switching between
                        station mode and WWAN mode.
  03/12/13   sb         Minor hostapd fixes.
  04/09/13   cp         AR6003 changes.
  04/16/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
  06/12/13   sg         Added DHCP Reservation feature
  09/17/13   at         Added support to Enable/Disable ALGs
  01/11/14   sr         Added support for connected Devices in SoftAP
  25/02/14   pm         Changes regarding Sta Assoc
  02/24/14   vm         Changes to Enable/Disable Station Mode to be in accordance
                        with IoE 9x15
  03/27/14   cp         Added support to DUN+SoftAP.
  06/06/14   rk         Added support DHCP option 26 for mtu in dnsmasq/dhcp server..
  08/08/14   rk         kw fixes.
  08/26/14   ka         Code Restructuring Changes
  06/12/15   rk         Offtarget support.
  04/06/16   pm         Load/Unload SFE modules on Enable/Disable
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
#include <linux/if.h>
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
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <ctype.h>
#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qmi_client.h"
#include "ds_qmi_qcmap_msgr.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_LAN.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_MediaService.h"
#include <unistd.h>
// Define some constants.
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length
#define ICMP_HDRLEN 8  // ICMP header length for echo request, excludes data
#define ETH_P_IPV6 0x86DD

#define SUPPLICANT_CFG_PATH "/etc/wpa_supplicant.conf"

#define HOSTAPD_CTRL_PATH "/var/run/hostapd"
#define HOSTNAME_PATH "/etc/hostname.txt"

#define DECIMAL 10
#define MAX_CMD_SIZE 100
#define MAX_SCAN_SIZE 100

#define MIN_NOTIFY_INTERVAL 30
#define MAX_NOTIFY_INTERVAL 60000


#define QCMAP_MULTIPLE_SYSTEM_CALL_FILE "/etc/qcmap_multiple_system_call.sh"

#define FACTORY_CONF "/etc/factory_mobileap_cfg.xml"
#define TEMP_MOBILEAP_CFG "/etc/mobileap_cfg.tmp"
#define TEMP_FIREWALL_CFG "/etc/mobileap_firewall.tmp"
#define TEMP_IPACM_CFG "/etc/IPACM_cfg.tmp"

#define IPV6_DEFAULT_PREFERED_LIFETIME 2700;
#define IPV6_DEFAULT_VALID_LIFETIME 7200;
#define IPV6_MIN_PREFIX_LENGTH 64
#define IPV6_MAX_PREFIX_LENGTH 128
#define MAX_IPV6_PREFIX 40
#define IPTYPE_IPV6_ICMP        58
/*
#define QCMAP_DEBUG 1

#ifdef QCMAP_DEBUG
#undef LOG_MSG_INFO1
#define LOG_MSG_INFO1(fmtString, x, y, z) \
{ \
  if ( x != 0 && y !=0 && z != 0) \
    printf("\nINFO1:"fmtString"\n", x, y, z); \
  else if ( x != 0 && y != 0) \
    printf("\nINFO1:"fmtString"\n", x, y); \
  else if ( x != 0) \
    printf("\nINFO1:"fmtString"\n", x); \
  else \
    printf("\nINFO1:"fmtString"\n"); \
}
#endif
*/
extern struct sigevent sev;
bool QCMAP_ConnectionManager::flag= false;
QCMAP_ConnectionManager* QCMAP_ConnectionManager::object=NULL;
boolean QCMAP_ConnectionManager::qcmap_tear_down_in_progress = false;
char QCMAP_ConnectionManager::xml_path[] = {0};
int QCMAP_ConnectionManager::qcmap_cm_handle = 0;
char QCMAP_ConnectionManager::firewall_xml_path[] = {0};
QCMAP_ConnectionManager* QCMAP_ConnectionManager::Get_Instance
(
 char* xml_path,
 boolean obj_create
 )
{

  if (!flag && obj_create)
  {
    if (xml_path!=NULL)
    {
      strlcpy(QCMAP_ConnectionManager::xml_path, xml_path, QCMAP_CM_MAX_FILE_LEN);
    }
    else
    {
      LOG_MSG_INFO1("Updating XML path with default values %s",
                    QCMAP_DEFAULT_CONFIG,0,0);
      strlcpy(QCMAP_ConnectionManager::xml_path, QCMAP_DEFAULT_CONFIG,
              QCMAP_CM_MAX_FILE_LEN);
    }
    LOG_MSG_INFO1("Creating object : QCMAP",0, 0, 0);
    object = new QCMAP_ConnectionManager();
    QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(true);
    //Getting LAN Object along with QCMAP object
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}

/*======================================================
  FUNCTION doesFileExist
======================================================*/
/*!
  @brief
  Function checks if file is present the the specified path.

@return
  boolean

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================================================*/
boolean doesFileExist (char *filename)
{
  struct stat  statbuf;

  bzero( &statbuf, sizeof(statbuf) );
  if( (stat(filename, &statbuf) == 0) && (S_ISREG(statbuf.st_mode)))
    return true;
  else
    return false;
}

/*==========================================================
  FUNCTION    qcmap_match_mac_address
===========================================================*/
/*!
@brief
  Comparator function for matching MAC address

@parameters
  Two void pointers with the MAC addresses to be compared

@return
  0 - on a match
  1 - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*==========================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_mac_address
(
  const void *first,
  const void *second
)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  uint8* msg_mac_addr = (uint8*)first;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  return(memcmp( msg_mac_addr,
                 connectedDevicesList->mac_addr,
                 QCMAP_MSGR_MAC_ADDR_LEN_V01));
}

/*==========================================================
 FUNCTION    qcmap_match_device_type
===========================================================*/
/*!
@brief
  Comparator function for match ConnectedDevices device type

@parameters
  Two void pointers with the device type to be compared

@return
  0 - on a match
  1 - otherwise

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_device_type
(
  const void *first,
  const void *second
)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  qcmap_msgr_device_type_enum_v01* device_type = (qcmap_msgr_device_type_enum_v01*)first;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  return((*device_type == connectedDevicesList->device_type) ? 0 : 1 );
}

/*=====================================================
  FUNCTION    qcmap_match_v6_iid
======================================================*/
/*!
@brief
  Comparator function for matching IID

@parameters
  void *first - qcmap_cm_prefix_iid_info_t ptr
  void *second - qcmap_cm_client_data_info_t ptr to client info

@return
  QCMAP_CM_SUCCESS - on a match
  QCMAP_CM_ERROR - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*=====================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_v6_iid
(
  const void *first,
  const void *second
)
{
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  qcmap_cm_prefix_iid_info_t *prefix_iid_ptr = NULL;
  struct ps_in6_addr *client_iid_ptr = NULL;

  if( (first == NULL) || (second == NULL))
  {
    return QCMAP_CM_ERROR;
  }

  prefix_iid_ptr = (qcmap_cm_prefix_iid_info_t *)first;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  if ( prefix_iid_ptr->iid_ptr == NULL )
  {
    LOG_MSG_ERROR("IID addr ptr is NULL,cannot match "
                  "IID's", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  client_iid_ptr = (struct ps_in6_addr *)connectedDevicesList->link_local_v6_addr;

  /*Check whether IID's match*/
  if(!memcmp(&prefix_iid_ptr->iid_ptr->ps_s6_addr64[1],
             &client_iid_ptr->ps_s6_addr64[1],
             sizeof(uint64_t)))
  {
    LOG_MSG_INFO1("qcmap_match_v6_iid - IID match found\n", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    //IID's do not match
    return QCMAP_CM_ERROR;
  }
}

/*=====================================================
  FUNCTION    qcmap_match_v6_addr
======================================================*/
/*!
@brief
  Comparator function for matching v6 addresses

@parameters
  void *first - qcmap_cm_prefix_iid_info_t ptr
  void *second - qcmap_cm_client_data_info_t ptr to client info

@return
  QCMAP_CM_SUCCESS - on a match
  QCMAP_CM_ERROR - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*=====================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_v6_addr
(
  const void *first,
  const void *second
)
{
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  qcmap_cm_prefix_iid_info_t *v6_ptr = NULL;
  struct ps_in6_addr *client_iid_ptr = NULL;

  if( (first == NULL) || (second == NULL))
  {
    return QCMAP_CM_ERROR;
  }

  v6_ptr = (qcmap_cm_prefix_iid_info_t *)first;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  if ( v6_ptr->iid_ptr == NULL )
  {
    LOG_MSG_ERROR("IID addr ptr is NULL,cannot match "
                  "IID's", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  client_iid_ptr = (struct ps_in6_addr *)connectedDevicesList->ip_v6_addr;

  /*Check whether addresses match*/
  if(!memcmp(&v6_ptr->iid_ptr->ps_s6_addr64[1],
             &client_iid_ptr->ps_s6_addr64[1],
             sizeof(uint64_t)))
  {
    LOG_MSG_INFO1("qcmap_match_v6_iid - IID match found\n", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    //Addresses do not match
    return QCMAP_CM_ERROR;
  }
}

/*=====================================================
  FUNCTION    qcmap_match_proxy_entry
======================================================*/
/*!
@brief
  Comparator function for matching proxy entry

@parameters
  void *first - ptr to head of list
  void *proxyInfo - qcmap_cm_proxy_data_info_t struct

@return
  QCMAP_CM_SUCCESS - on a match
  QCMAP_CM_ERROR - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*=====================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_proxy_entry
(
  const void *first,
  const void *second
)
{
  qcmap_cm_proxy_data_info_t* proxyInfo_first = NULL;
  qcmap_cm_proxy_data_info_t* proxyInfo_second = NULL;

  if( (first == NULL) || (second == NULL))
  {
    return QCMAP_CM_ERROR;
  }

  proxyInfo_first = (qcmap_cm_proxy_data_info_t *)first;
  proxyInfo_second = (qcmap_cm_proxy_data_info_t *)second;

  /*Check whether IID's match*/
  if( !memcmp(proxyInfo_first->dst_ip_v6_addr, proxyInfo_second->dst_ip_v6_addr,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01) &&
      !memcmp(proxyInfo_first->src_ip_v6_addr, proxyInfo_second->src_ip_v6_addr,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01) )
  {
    LOG_MSG_INFO1("qcmap_match_proxy_entry - dst and src match", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    //no match
    return QCMAP_CM_ERROR;
  }
}

/*==========================================================
 FUNCTION    qcmap_v6_proxy_match
===========================================================*/
/*!
@brief
  Comparator function for match ConnectedDevices device type

@parameters
  Two void pointers with the device type to be compared

@return
  0 - on a match
  1 - otherwise

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
static long int QCMAP_ConnectionManager::qcmap_v6_proxy_match
(
  const void *first,
  const void *second
)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  LOG_MSG_ERROR("memcmp",0,0,0);

  return(memcmp(first, second, QCMAP_MSGR_IPV6_ADDR_LEN_V01));
}

/*=====================================================
  FUNCTION    fetchHostNameAndLeasetime
======================================================*/
/*!
@brief
  Function to fetch the Device name  and lease time from
  dnsmasq.leases file

@parameters
  fd of the file to be parsed
  Device MAC address whose hostname & lease expiry has to be found
  char pointer to store the hostname of device
  uint32 pointer to hold the expiry time (in minutes) of the DHCP lease

@return
  void

@note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*====================================================*/
static void
fetchHostNameAndLeasetime
(
  FILE* fd,
  uint8* mac_addr,
  char* hostName,
  uint32* leaseExpiryTime
)
{
  if ((fd == NULL) || (mac_addr == NULL) ||(hostName == NULL) ||
      (leaseExpiryTime == NULL))
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return;
  }
  char *expiryTime=NULL,*charMacAddr=NULL, *filehostName=NULL,*ptr;
  char stringline[MAX_COMMAND_STR_LEN];
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
  int bufferSize = QCMAP_EPOCH_TIME_LEN +
                   QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01 +
                   QCMAP_MSGR_DEVICE_NAME_MAX_V01;
  int expTime = 0;
  time_t expTimeFormat;
  //Reset the file ptr to start of file
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    return;
  }

  ds_mac_addr_ntop(mac_addr, mac_addr_string);
  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    expiryTime = strtok_r(stringline, " ", &ptr);
    charMacAddr = strtok_r(NULL, " ", &ptr);
    /* KW fix */
    if ( charMacAddr!=NULL && !strcmp(charMacAddr, mac_addr_string))
    {
      //If the Mac address matches, store the hostname
      filehostName = strtok_r(NULL, " ",&ptr);

      //Copy hostname to CDI list only if  CDI host name is empty

      if(filehostName && !strlen(hostName) )
        strlcpy( hostName,filehostName,QCMAP_MSGR_DEVICE_NAME_MAX_V01 );
      //Convert the expiry time from char format to interger
      expTime = ds_atoi((const)expiryTime);
      expTimeFormat = expTime;
      //Convert the time from Epoch format to readable format
      *leaseExpiryTime = (expTimeFormat - time(NULL))/60;

      if (!strncmp(hostName, "*\n", strlen("*\n")))
      {
        QCMAP_ConnectionManager::fetchHostName(hostName, mac_addr_string);
      }
      return;
    }
  }
  return;
}

/*=====================================================
  FUNCTION strtolower
======================================================*/
/*!
@brief
  Converts the string passed to lower case.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================================================*/

void  QCMAP_ConnectionManager::strtolower(uint8 *str)
{
  int counter = 0;
  char c;

  if( str!=NULL )
  {
    while ( str[counter] ) {
      c = str[counter];
      str[counter] = tolower(c);
      counter++;
    }
  }
}

/*=====================================================
  FUNCTION QCMAP_ConnectionManager_callback
======================================================*/
/*!
@brief
  Handles the mobile ap events and invokes the callback function
  for the respective indications to be sent.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================*/
static void QCMAP_ConnectionManager_callback
(
  int               handle,               /* MobileAP Application id */
  qcmap_cm_event_e  event,                /* Type of MobileAP Event  */
  void             *qcmap_msgr_cb_user_data, /* Call back user data     */
  dsi_ce_reason_t   *callend_reason
)
{
  boolean cradle_sta_eth_bh_active;
  QCMAP_ConnectionManager *manager;
  qmi_qcmap_msgr_status_cb_data *cbPtr;
  qmi_qcmap_msgr_softap_handle_type *softApHandle =
                  (qmi_qcmap_msgr_softap_handle_type *) qcmap_msgr_cb_user_data;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];

  qcmap_msgr_wwan_call_end_type_enum_v01 call_end_type =
                                      QCMAP_MSGR_WWAN_CALL_END_TYPE_INVALID_V01;
  int call_end_reason_code=0;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char logmsg[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  int qcmap_cm_error;
  qcmap_msgr_device_type_enum_v01 dev_type ;
  char mac_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]={0};
  char host_str[QCMAP_MSGR_DEVICE_NAME_MAX_V01]={0};
  qcmap_msgr_ip_passthrough_mode_enum_v01 status;
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;

  if (softApHandle != NULL)
  {
    cbPtr = (qmi_qcmap_msgr_status_cb_data *) softApHandle->cb_ptr;
    manager = softApHandle->Mgr;
  } else {
     return;
  }

  LOG_MSG_INFO1("QCMAP AP Handle %04X Event %04X\n", handle, event,0);

  memset ( devname, 0, sizeof(devname));
  memset ( logmsg, 0, sizeof(logmsg));

  /*If connect to backhaul failed then callend_reason would be filled
    with reason and call type, same is sent to clients in failure response*/
  if( callend_reason != NULL )
  {
    if( callend_reason->reason_type == DSI_CE_TYPE_MOBILE_IP )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_MOBILE_IP_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_INTERNAL )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_INTERNAL_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_CALL_MANAGER_DEFINED )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_CALL_MANAGER_DEFINED_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_3GPP_SPEC_DEFINED )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_3GPP_SPEC_DEFINED_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_PPP )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_PPP_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_EHRPD )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_EHRPD_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_IPV6 )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_IPV6_V01;
    call_end_reason_code = callend_reason->reason_code;
  }

  cradle_sta_eth_bh_active = QCMAP_Backhaul::IsNonWWANBackhaulActive();
  switch ( event )
  {
  case QCMAP_CM_EVENT_ENABLED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_ENABLED\n",0,0,0);
    if (cbPtr && cbPtr->qcmap_cb)
    {
      cbPtr->qcmap_cb(cbPtr, QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01);
    }
    /* Block WWAN access for the clients. */
    QcMapBackhaulMgr->BlockIPv4WWANAccess();
    QcMapBackhaulMgr->BlockIPv6WWANAccess();
    break;
  case QCMAP_CM_EVENT_WAN_CONNECTING:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_CONNECTING\n",0,0,0);
    break;
  case QCMAP_CM_EVENT_WAN_CONNECTING_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_CONNECTING_FAIL\n",0,0,0);
    if (cbPtr && cbPtr->wwan_cb)
    {
     cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01,
                     call_end_type,call_end_reason_code);
    }
    if(qcmap_cm_get_ipv6_state(&qmi_err_num) < QCMAP_CM_V6_WAN_CONNECTING)
      delete QcMapBackhaulWWANMgr;

    break;
  case QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL\n",0,0,0);
    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01,
                     call_end_type,call_end_reason_code);
    }
    if (qcmap_cm_get_state(&qmi_err_num) < QCMAP_CM_WAN_CONNECTING)
      delete QcMapBackhaulWWANMgr;

    break;
  case QCMAP_CM_EVENT_WAN_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_CONNECTED\n",0,0,0);
    /* There is no need to modify the netmask of WWAN interface. If we modify the
     * the netmask existing routes installed by the embedded applications
     * will be deleted and their data transfer will be affected.
     */
    /* Now time to enable NAT, ALGs etc., on A5*/
    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
        QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
    {
      QcMapBackhaulWWANMgr->AddWWANIPv4SIPServerInfo();
      if (QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_WWAN,QcMapBackhaulMgr->current_backhaul))
      {
        if (QcMapBackhaulMgr->current_backhaul != BACKHAUL_TYPE_WWAN &&
            QcMapBackhaulMgr->current_backhaul != NO_BACKHAUL)
        {
          if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV4() ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
          {
            QcMapNatAlg->DisableNATonApps();
            QcMapFirewall->CleanIPv4MangleTable();
          }
          if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV6() ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
          {
            QcMapBackhaulMgr->DisableIPV6Forwarding(true);
            QcMapFirewall->CleanIPv6MangleTable();
            if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
            }
            if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
            {
              if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
                QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
              else
              {
                QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
                QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
              }
            }
            if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
            }
          }
        }
        QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_WWAN;
        QcMapNatAlg->EnableNATonApps();
        QCMAPLANMgr->EnableDNS();
      }
    }
    ds_system_call("echo QCMAP:WAN connected v4 > /dev/kmsg",\
                   strlen("echo QCMAP:WAN connected v4 > /dev/kmsg"));

    if ( qcmap_cm_get_dev_name(manager->qcmap_cm_handle,
                              QCMAP_MSGR_IP_FAMILY_V4_V01,
                              devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", 0,0,0);
      qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    }

    snprintf((char *)logmsg,
                     MAX_COMMAND_STR_LEN,
                     "echo %s > /proc/ipv4_iface_name",
                     devname);
    ds_system_call((char *)logmsg,strlen(logmsg));

    //Get IP Passthrough flag from XML
    if(QCMAPLANMgr->GetIPPassthroughConfig(&enable_state,&passthrough_config, &qmi_err_num))
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

#ifdef FEATURE_DATA_TARGET_MDM9607
    if (QcMapMgr->usb_clk_node_present)
    {
      if ( (QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
        QcMapMgr->CheckRatforLTE()))
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
      else
      {
        QcMapMgr->SetUSBClockFreq(false);
      }
    }
#endif

    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    break;

  case QCMAP_CM_EVENT_WAN_IPv6_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_IPv6_CONNECTED\n",0,0,0);
    if (QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_WWAN,QcMapBackhaulMgr->current_backhaul))
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
          QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
      {
        if (QcMapBackhaulMgr->current_backhaul != BACKHAUL_TYPE_WWAN &&
            QcMapBackhaulMgr->current_backhaul != NO_BACKHAUL)
      {
        if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
            QCMAP_Backhaul_WLAN::IsSTAAvailableV4() ||
            QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
        {
          QcMapNatAlg->DisableNATonApps();
          QcMapFirewall->CleanIPv4MangleTable();
        }
        if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
            QCMAP_Backhaul_WLAN::IsSTAAvailableV6() ||
            QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
        {
          QcMapBackhaulMgr->DisableIPV6Forwarding(true);
          QcMapFirewall->CleanIPv6MangleTable();
          if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
          {
            QcMapBackhaulMgr->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          }
          if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
          {
            if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
            else
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
            }
          }
          if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
          {
            QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
          }
        }
      }
      QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_WWAN;
      QcMapBackhaulMgr->EnableIPV6Forwarding();
      QcMapFirewall->EnableIPV6Firewall();
    }
    }
    ds_system_call("echo QCMAP:WAN connected v6 > /dev/kmsg",\
                   strlen("echo QCMAP:WAN connected v6 > /dev/kmsg"));

    if ( qcmap_cm_get_dev_name(manager->qcmap_cm_handle,
                              QCMAP_MSGR_IP_FAMILY_V6_V01,
                              devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", 0,0,0);
      qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    }

    snprintf((char *)logmsg,
                     MAX_COMMAND_STR_LEN,
                     "echo %s > /proc/ipv6_iface_name",
                     devname);
    ds_system_call((char *)logmsg,strlen(logmsg));

#ifdef FEATURE_DATA_TARGET_MDM9607
    if (QcMapMgr->usb_clk_node_present)
    {
      if ((QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
           QcMapMgr->CheckRatforLTE()))
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
      else
      {
        QcMapMgr->SetUSBClockFreq(false);
      }
    }
#endif

    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    break;
  case QCMAP_CM_EVENT_WAN_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_DISCONNECTED\n",0,0,0);
    /* 9x25, Disable NAT on A5 */
     if(QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN &&
        (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false) &&
        (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      QcMapBackhaulWWANMgr->DeleteWWANIPv4SIPServerInfo();
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
      if(qcmap_cm_get_ipv6_state(&qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED)
      {
        QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_WWAN,
                                                     true,true);
#ifdef FEATURE_DATA_TARGET_MDM9607
        if (QcMapMgr->usb_clk_node_present)
        {
          if (QcMapMgr->CheckWlanClientConnected()&&
              QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
          {
            QcMapMgr->SetUSBClockFreq(true);
          }
          else
          {
            QcMapMgr->SetUSBClockFreq(false);
          }
        }
#endif
      }
    }

    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    if(qcmap_cm_get_ipv6_state(&qmi_err_num) < QCMAP_CM_V6_WAN_CONNECTING)
      delete QcMapBackhaulWWANMgr;

    break;

  case QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED\n",0,0,0);
    if(QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN &&
       (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false) &&
       (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      //MAKE CHANGES TO SWITCH TO STA OR CRADLE
      if(qcmap_cm_get_state(&qmi_err_num) != QCMAP_CM_WAN_CONNECTED)
      {
        QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_WWAN,
                                                       true,true);
      }
#ifdef FEATURE_DATA_TARGET_MDM9607
      if (QcMapMgr->usb_clk_node_present)
      {
        if (QcMapMgr->conn_device_info.wlanclientpresent &&
            QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
        {
          QcMapMgr->SetUSBClockFreq(true);
        }
        else
        {
          QcMapMgr->SetUSBClockFreq(false);
        }
      }
#endif
    }
    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    if (qcmap_cm_get_state(&qmi_err_num) < QCMAP_CM_WAN_CONNECTING)
      delete QcMapBackhaulWWANMgr;
  break;
  case QCMAP_CM_EVENT_WAN_ADDR_RECONF:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_ADDR_RECONF\n", 0, 0, 0);
    if( !cradle_sta_eth_bh_active)
    {
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
      QcMapNatAlg->EnableNATonApps();
      QCMAPLANMgr->EnableDNS();
    }
  break;
  case QCMAP_CM_EVENT_WAN_IPv6_ADDR_RECONF:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_ADDR_RECONF\n", 0, 0, 0);
    if( !cradle_sta_eth_bh_active)
    {
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      QcMapBackhaulMgr->EnableIPV6Forwarding();
      QcMapFirewall->EnableIPV6Firewall();
    }
  break;
  case QCMAP_CM_EVENT_DISABLED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_DISABLED\n",0,0,0);
    if (cbPtr && cbPtr->qcmap_cb)
    {
      cbPtr->qcmap_cb(cbPtr, QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01);
    }
    pthread_mutex_lock(&manager->cm_mutex);
    QcMapMgr->qcmap_tear_down_in_progress = false;
    manager->DisableHandle();
    pthread_cond_signal(&manager->cm_cond);
    pthread_mutex_unlock(&manager->cm_mutex);
    LOG_MSG_INFO1("Teardown flag=false\n",0,0,0);
    break;
  case QCMAP_CM_EVENT_STA_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_CONNECTED\n",0,0,0);
    if (NULL != QcMapBackhaulWLANMgr)
    {
      if (QcMapBackhaulWLANMgr->sta_connected_event_received == true)
      {
        LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_CONNECTED - Duplicate Event \n",0,0,0);
        break;
      }
      if (QcMapWLANMgr->IsSSRInProgress())
    {
      LOG_MSG_INFO1("Ignoring QCMAP_CM_EVENT_STA_CONNECTED event -- since it"
                    " recieved during SSR",0,0,0);
      break;
    }
      QcMapBackhaulWLANMgr->sta_connected_event_received = true;
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        QcMapBackhaulWLANMgr->ProcessStaAssocForWLANBridge(qcmap_msgr_cb_user_data);
      }
      else
      {
        QcMapBackhaulWLANMgr->ProcessStaAssoc(qcmap_msgr_cb_user_data);
      }

      /* Send WLAN Status IND */
      if(QcMapWLANMgr && cbPtr && cbPtr->wlan_status_cb)
      {
        QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                    QCMAP_MSGR_WLAN_CONNECTING_V01;
        QcMapWLANMgr->SendWLANStatusIND();
      }
    }
    break;
  case QCMAP_CM_EVENT_STA_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_DISCONNECTED\n",0,0,0);
    if(NULL != QcMapBackhaulWLANMgr)
    {
      if (QcMapBackhaulWLANMgr->sta_connected_event_received == false)
      {
        LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_DISCONNECTED - Duplicate Event \n",0,0,0);
        break;
      }
      if (QcMapWLANMgr->IsSSRInProgress())
      {
        LOG_MSG_INFO1("Ignoring QCMAP_CM_EVENT_STA_DISCONNECTED event -- since"
                      " it recieved during SSR",0,0,0);
        break;
      }
      QcMapBackhaulWLANMgr->sta_connected_event_received = false;
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        QcMapBackhaulWLANMgr->ProcessStaDisAssocForWLANBridge();
      }
      else
      {
        QcMapBackhaulWLANMgr->ProcessStaDisAssoc();
      }
      if (cbPtr && cbPtr->sta_cb)
      {
          cbPtr->sta_cb(cbPtr, QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01);
      }

      /* Send WLAN Status IND */
      if(QcMapWLANMgr && cbPtr && cbPtr->wlan_status_cb)
      {
        QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                    QCMAP_MSGR_WLAN_DISCONNECTED_V01;
        QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                    QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
        QcMapWLANMgr->SendWLANStatusIND();
      }
    }
    break;
    case QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL\n",0,0,0);
    if(NULL != QcMapBackhaulWLANMgr)
    {
      QcMapBackhaulWLANMgr->ProcessStaAssocFail(qcmap_msgr_cb_user_data);
    }
    break;
    case QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL\n",0,0,0);
    if(NULL != QcMapBackhaulWLANMgr)
      QcMapBackhaulWLANMgr->ProcessStaDHCPIPFail(qcmap_msgr_cb_user_data);
    break;
  case QCMAP_CM_EVENT_CRADLE_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_CRADLE_CONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->cradle_cb)
    {
        cbPtr->cradle_cb(cbPtr, QCMAP_MSGR_CRADLE_CONNECTED_V01);
    }

    break;
  case QCMAP_CM_EVENT_CRADLE_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_CRADLE_DISCONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->cradle_cb)
    {
        cbPtr->cradle_cb(cbPtr, QCMAP_MSGR_CRADLE_DISCONNECTED_V01);
    }
    break;
    case QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->eth_backhaul_cb)
    {
        cbPtr->eth_backhaul_cb(cbPtr, QCMAP_MSGR_ETH_BACKHAUL_CONNECTED_V01);
    }
    break;
  case QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->eth_backhaul_cb)
    {
        cbPtr->eth_backhaul_cb(cbPtr, QCMAP_MSGR_ETH_BACKHAUL_DISCONNECTED_V01);
    }
    break;
  default:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_UNKNOWN %08X\n", event,0,0);
    break;
  }

  return;
}

/*======================================================
                             Class Definitions
  =====================================================*/

QCMAP_ConnectionManager::QCMAP_ConnectionManager()
{
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_CM_LOG_FUNC_ENTRY();
  in_addr addr, usb_ip_addr;
  uint8_t i;
  qmi_error_type_v01 qmi_err_num;
  char command[MAX_COMMAND_STR_LEN];
  pugi::xml_document xml_file;
  int fd = 0;

  /*-----------------------------------------------------------------------------*/
  this->qcmap_enable = true;
  /* Populate default Config */
  memset(&this->cfg, 0, sizeof(this->cfg));
  this->qcmap_cm_handle = 0;
  this->qcmap_tear_down_in_progress = false;
  this->sta_iface_index = QCMAP_LAN_INVALID_IFACE_INDEX;
  this->ap_dev_num1 = QCMAP_LAN_INVALID_IFACE_INDEX;
  this->ap_dev_num2 = QCMAP_LAN_INVALID_IFACE_INDEX;
  pthread_mutex_init(&this->cm_mutex, NULL);
  pthread_cond_init(&this->cm_cond, NULL);

  // Initialize mobileap,wlan bootup values
  this->cfg.bootup_config.enable_wlan_at_bootup =false;
  this->cfg.bootup_config.enable_mobileap_at_bootup =false;

  if(QCMAP_ConnectionManager::CheckDUNSoftAPMode())
  {
     this->DUN_SoftAP_enable_flag = true;
  }
  else
  {
     this->DUN_SoftAP_enable_flag = false;
  }

  this->revert_ap_sta_router_mode = false;

  //Initialize Connected Devices Info to NULL
  memset (&this->conn_device_info, 0, sizeof(qcmap_cm_connected_info_t));
  memset (&this->addrList, NULL, sizeof(qcmap_addr_info_list_t));

  /* Kill dnsmasq which would get started on boot up */
  ds_system_call("killall -9 dnsmasq",strlen("killall -9 dnsmasq"));

  bzero(&command,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("Enabling SIP segmentation",0,0,0);
  ds_system_call( "echo 1 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation",
                  strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation"));
  /* Read configuration from XML file. */
  if (ReadQCMAPConfigFromXML(this->xml_path))
  {
    LOG_MSG_INFO1("QCMAP Read XML OK\n",0,0,0);
  }
  else
  {

    /* Read Config from XML failed. Use defaults. */
    LOG_MSG_INFO1("QCMAP Read XML failed.\n",0,0,0);
    LOG_MSG_ERROR("Unable to load XML file.Creating XML with defaults",0,0,0);
    QCMAP_ConnectionManager::WriteDefaultsToXML();
  }
  strlcpy( this->cfg.firewall_config_file,QCMAP_FIREWALL_CONFIG,
         ( QCMAP_CM_MAX_FILE_LEN));


  if (!xml_file.load_file(this->cfg.firewall_config_file))
  {
  strlcpy( QCMAP_ConnectionManager::firewall_xml_path,QCMAP_FIREWALL_CONFIG,
           ( QCMAP_CM_MAX_FILE_LEN));

    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    LOG_MSG_ERROR("Unable to load XML file.Creating XML with defaults",0,0,0);
    QCMAP_Firewall::WriteDefaultsToFirewallXML();
  }

  /* If the factory configuration file doesnot exist, copy the primary
     configuration file to factory configuration */
  if( !doesFileExist( FACTORY_CONF ))
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "cp %s %s ",this->xml_path,FACTORY_CONF);
    ds_system_call(command, strlen(command));
  }

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_int = AUTO_CONNECT_V4;
  if(timer_create(CLOCKID, &sev, &(this->timerid_v4))==-1)
    LOG_MSG_ERROR("timer_create failed ",0,0,0);
  this->auto_timer_value_v4=AUTO_CONNECT_TIMER;
  sev.sigev_value.sival_ptr =NULL;
  this->auto_connect_timer_running_v4=false;

  sev.sigev_value.sival_int = AUTO_CONNECT_V6;
  if(timer_create(CLOCKID, &sev, &(this->timerid_v6))==-1)
    LOG_MSG_ERROR("timer_create failed ",0,0,0);
  sev.sigev_value.sival_ptr = NULL;
  this->auto_timer_value_v6=AUTO_CONNECT_TIMER;
  this->auto_connect_timer_running_v6=false;

#ifdef FEATURE_DATA_TARGET_MDM9607
  fd = open(USB_CLK_UPDATE_NODE,O_WRONLY);
  if(fd < 0)
  {
    LOG_MSG_ERROR("Error in opening %s", USB_CLK_UPDATE_NODE,0,0);
    this->usb_clk_node_present = false;
    return;
  }
  else
  {
    LOG_MSG_INFO1("USB Clock file node present %s", USB_CLK_UPDATE_NODE,0,0);
    this->usb_clk_node_present = true;
    this->Dsd_client_init();
  }
  close(fd);
#endif
  return;
}

/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroying the QCMAP Object.

@parameters
  None

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

QCMAP_ConnectionManager::~QCMAP_ConnectionManager()
{
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_MediaService* QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  LOG_MSG_INFO1("Disabling SIP segmentation",0,0,0);
  ds_system_call( "echo 0 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation",
                  strlen("echo 0 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation"));
  delete QCMAPLANMgr;//Destroying LAN object here

  if (QcMapMediaServiceMgr)
    delete QcMapMediaServiceMgr;
  QCMAP_CM_LOG_FUNC_ENTRY();
  pthread_mutex_destroy(&this->cm_mutex);
  pthread_cond_destroy(&this->cm_cond);
#ifdef FEATURE_DATA_TARGET_MDM9607
  if (this->usb_clk_node_present)
  {
    if(qmi_dsd_client_handle != NULL)
    {
      qmi_client_release(qmi_dsd_client_handle);
    }
    this->usb_clk_node_present = false;
  }
#endif
  flag=false;
  object=NULL;
}

/*======================================================
  FUNCTION CleanIPtables
======================================================*/
/*!
@brief
  Clean IPTables NAT, Filter, Mangle and Raw tables

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
/*====================================================*/
void QCMAP_ConnectionManager::CleanIPtables(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  LOG_MSG_INFO1("CleanIPtables()",0,0,0);

  QcMapMgr->CleanIPv4NatTable();
  QcMapMgr->CleanIPv4FilterTableFwdChain();
  QcMapMgr->CleanIPv4FilterTableInChain();
  if (QcMapFirewall)
    QcMapFirewall->CleanIPv4MangleTable();
}

/*=====================================================
 FUNCTION CleanEbtables
======================================================*/
/*!
@brief
  Clean EbTables NAT, Filter and broute tables

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
/*====================================================*/
void QCMAP_ConnectionManager::CleanEbtables(void)
{
  char command[MAX_COMMAND_STR_LEN];
  FILE *access_fd = NULL;
  LOG_MSG_INFO1("CleanEbtables()",0,0,0);

  access_fd = fopen(QCMAP_MULTIPLE_SYSTEM_CALL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
     LOG_MSG_ERROR("Cannot open %s file for writing",QCMAP_MULTIPLE_SYSTEM_CALL_FILE,
                   0,0);
     return;
  }
  fprintf( access_fd,"#!/bin/sh \n");

  fprintf( access_fd,"ebtables -t nat --flush \n");
  fprintf( access_fd,"ebtables -t filter --flush \n");
  fprintf( access_fd,"ebtables -t broute --flush \n");
  fclose(access_fd);

  /* Changing permissions */
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
}

/*=====================================================
  FUNCTION CheckDUNSoftAPMode
=====================================================*/
/*!
@brief
  Checks if we are in DUN+SoftAP Mode or not.

@parameters
  None.

@return
  true  - If we are in DUN+SoftAP mode.
  false - if we are not in DUN+SoftAP mode.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*===================================================*/
boolean QCMAP_ConnectionManager:: CheckDUNSoftAPMode(void)
{
  int i = 0;
  FILE *cmd = NULL;
  char mode = 0;
  const char process[] = "cat /sbin/usb/softap_w_dun";

  cmd = popen(process, "r");
  if(cmd)
    mode = fgetc(cmd);

  if ( mode == 'Y' )
  {
    LOG_MSG_INFO1("In DUN+SoftAP mode.", 0, 0, 0);
    return TRUE;
  }
  else
  {
    LOG_MSG_INFO1(" Not in DUN+SoftAP mode.", 0, 0, 0);
    return FALSE;
  }
}

/*=====================================================
  FUNCTION Enable
======================================================*/
/*!
@brief
  Enables MobileAP based on the configuration.
  As a part of enable mobileap, registers mobileap event callback.
  It will bring up RmNet between Q6 and A5.
  If AutoConnect is enabled then initiates connect to backhaul.
  If station mode is configured then enables it.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::Enable
(
  int *handle,
  void *cb_user_data,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  struct timespec ts;
  char Kernel_ver[KERNEL_VERSION_LENGTH];
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if(!QcMapMgr)
  return false;
  ds_system_call("echo QCMAP:Enable mobileap > /dev/kmsg",
          strlen("echo QCMAP:Enable mobileap > /dev/kmsg"));
  QCMAP_CM_LOG_FUNC_ENTRY();

  ((qmi_qcmap_msgr_softap_handle_type *)cb_user_data)->Mgr = QcMapMgr;

  QcMapMgr->qcmap_cm_handle = *handle =qcmap_cm_enable(&QcMapMgr->cfg,
                                                       QCMAP_ConnectionManager_callback,
                                                       cb_user_data,
                                                       &qcmap_cm_errno,
                                                       qmi_err_num);


  if (QcMapMgr->qcmap_cm_handle <= 0)
  {
    LOG_MSG_ERROR("QCMAP Connection failed \n",0,0,0);
    return false;
  }

  /* Store all the Client/CB related information */
  QcMapMgr->qcmap_cb_handle =
         (void *)((qmi_qcmap_msgr_softap_handle_type *)cb_user_data)->cb_ptr;

  LOG_MSG_INFO1("QCMAP Enabled this->qcmap_cm_handle=%x \n",
                QcMapMgr->qcmap_cm_handle,0,0);

  ds_system_call("echo QCMAP:Enable mobileap done > /dev/kmsg",
          strlen("echo QCMAP:Enable mobileap done > /dev/kmsg"));

#ifdef FEATURE_DATA_TARGET_MDM9607

  ds_system_call("echo 16 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold",
          strlen("echo 16 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold"));

#endif

  if ( false == QcMapMgr->GetKernelVer(Kernel_ver) )
  {
    LOG_MSG_ERROR("Unable to get the kernel version info", 0, 0, 0);
    return false;
  }

  snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/extra/shortcut-fe.ko",Kernel_ver);
  ds_system_call(command, strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/extra/shortcut-fe-ipv6.ko",Kernel_ver);
  ds_system_call(command, strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/extra/shortcut-fe-cm.ko",Kernel_ver);
  ds_system_call(command, strlen(command));

  if (ds_get_target() == DS_TARGET_LE_ATLAS)
  {
    ds_system_call("echo 0 > /proc/sys/debug/sfe/aggr_on",
            strlen("echo 0 > /proc/sys/debug/sfe/aggr_on"));

    ds_system_call("echo 0 > /proc/sys/debug_v6/sfe_v6/v6_aggr_on",
            strlen("echo 0 > /proc/sys/debug_v6/sfe_v6/v6_aggr_on"));
  }

  return true;
}


/*=====================================================
  FUNCTION Disable
======================================================*/
/*!
@brief
  Disable Mobile ap will bring down the backhaul and wlan.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean QCMAP_ConnectionManager::Disable
(
  int *err_num,
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret = 0, qcmap_cm_errno;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                             QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::Disable",0,0,0);
  QCMAP_CM_LOG_FUNC_ENTRY();
  boolean retval;
  if(!QcMapMgr)
    return false;
  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  QcMapMgr->qcmap_tear_down_in_progress = true;
  pthread_cond_signal(&QcMapMgr->cm_cond);

  ret = qcmap_cm_disable(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, qmi_err_num);

  if (ret < 0)
  {
    LOG_MSG_ERROR("QCMAP Disable fails: %d\n", qcmap_cm_errno,0,0);
    retval = false;
  }
  else
  {
    *err_num = qcmap_cm_errno;
    LOG_MSG_ERROR("QCMAP Disable in progress\n",0,0,0);
    retval = true;
  }

  if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01)
    QCMAP_WLAN::DisableWLAN(qmi_err_num);

  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
      QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
    QcMapBackhaulCradleMgr->DisableCradle(qmi_err_num);

  if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() ||
      QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
    QcMapBackhaulEthMgr->DisableEthBackhaul(qmi_err_num);

#ifdef FEATURE_DATA_TARGET_MDM9607

  ds_system_call("echo 1 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold",
          strlen("echo 1 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold"));

#endif

  ds_system_call("rmmod shortcut_fe_cm",
          strlen("rmmod shortcut-fe_cm"));

  ds_system_call("rmmod shortcut_fe_ipv6",
          strlen("rmmod shortcut_fe_ipv6"));

  ds_system_call("rmmod shortcut_fe",
          strlen("rmmod shortcut_fe"));

  return retval;
}

/*=====================================================
  FUNCTION DisableHandle
======================================================*/
/*!
@brief
 Initialize mobileap handle to zero.

@return
  None.

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_ConnectionManager::DisableHandle()
{
  char command[MAX_COMMAND_STR_LEN];

  this->qcmap_cm_handle = 0;

  /* Flush the forward chain. This is needed to remove rule added
  * to block WWAN traffic.
  */
  this->CleanIPv4FilterTableFwdChain();
  this->CleanIPv6FilterTableFwdChain();
}



/*=====================================================
  FUNCTION IsInterfaceEnabled
======================================================*/
/*!
@brief
 This function returns if the interface is enabled.
@return
None
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
int QCMAP_ConnectionManager::IsInterfaceEnabled(char *interface)
{
  struct ifreq ifr;
  int sock;
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock < 0)
  {
    LOG_MSG_ERROR("%s: Unable to  socket %d", __func__, errno, 0);
    return -1;
  }
  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
  if (ioctl(sock,SIOCGIFINDEX,&ifr) < 0)
  {
    return -1;
  }
  LOG_MSG_INFO1( "QCMAP ifr.ifr_ifindex =%d ",ifr.ifr_ifindex,0,0);
  close(sock);
  return ifr.ifr_ifindex;
}


/*======================================================
  FUNCTION WriteConfigToXML
======================================================*/
/*!
@brief
  Writes the mobileap configuration file with the latest configuration values.

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
/*====================================================*/

boolean QCMAP_ConnectionManager::WriteConfigToXML
(
  qcmap_update_xml type,
  pugi::xml_document *xml_file
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  bzero(&command,MAX_COMMAND_STR_LEN);

  if (type == UPDATE_MOBILEAP_XML)
  {
    xml_file->save_file(TEMP_MOBILEAP_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_MOBILEAP_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"mv %s %s ",TEMP_MOBILEAP_CFG ,
              QcMapMgr->xml_path);
    ds_system_call(command, strlen(command));
  }
  else if (type == UPDATE_FIREWALL_XML)
  {
    xml_file->save_file(TEMP_FIREWALL_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_FIREWALL_CFG);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"mv %s %s ",TEMP_FIREWALL_CFG ,
             QcMapMgr->cfg.firewall_config_file);
    ds_system_call(command, strlen(command));
  }
  else if (type == UPDATE_IPACFG_XML)
  {
    xml_file->save_file(TEMP_IPACM_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_IPACM_CFG);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"mv %s %s ",TEMP_IPACM_CFG ,
             IPA_XML_PATH);
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_ERROR("Incorrect xml File type",0,0,0);
    return false;
  }
  return true;
}




/*======================================================
  FUNCTION GetWWANMTUInfo
======================================================*/
/*!
@brief
  This function will get mtu size from wwan ipv4 interface .This API is made static because
  it is being called during constructor execution.

@parameters
  int* mtu_wwan_ipv4

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetWWANMTUInfo(int* mtu_wwan_ipv4)
{
  int socket_mtu, af = AF_INET;
  struct ifreq ifr;
  int qcmap_cm_error;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  /* get mtu value for rmnet of ipv4 wwan backhaul starts */
  /* Get Device Name */
  /* Send the RS packet to the rmnet iface */
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  if(qcmap_cm_get_dev_name(QCMAP_ConnectionManager::qcmap_cm_handle,
                           QCMAP_MSGR_IP_FAMILY_V4_V01,
                           devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Couldn't get ipv4 rmnet name. error %d\n", qcmap_cm_error,0,0);
    return false;
  }

  memset(&ifr, 0, sizeof(ifr));
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), devname);
  /* get mtu from found device for ipv4 starts */
  if((socket_mtu = socket(af, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Couldn't create socket to get mtu of ipv4 wwan err %d\n", errno,0,0);
    return false;
  }
  ifr.ifr_addr.sa_family = AF_INET;
  if (ioctl(socket_mtu, SIOCGIFMTU, &ifr) < 0)
  {
    LOG_MSG_ERROR("Couldn't get mtu from ipv4 wwan iface. err %d\n", errno,0,0);
    close(socket_mtu);
    return false;
  }
  LOG_MSG_INFO1("Successfully obtained MTU of wwan IPV4 iface  %d",  ifr.ifr_mtu, 0, 0);
  close(socket_mtu);
  /* get mtu from found device for ipv4 wwan iface ends */
  if(ifr.ifr_mtu > 0 )
  {
    *mtu_wwan_ipv4=(int) ifr.ifr_mtu;
    return true;
  }
  else
  {
    LOG_MSG_ERROR("Couldn't get .mtu i.e not got it from wwan  %d\n", ifr.ifr_mtu,0,0);
    return false;
  }
}


/*======================================================
  FUNCTION StartMcastDaemon
======================================================*/
/*!
@brief
  Starts pimd service, which provides protocol independent multicast service.

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
/*====================================================*/
void QCMAP_ConnectionManager::StartMcastDaemon(void)
{
  ds_system_call("pimd", strlen("pimd"));
}

/*======================================================
  FUNCTION StopMcastDaemon
======================================================*/
/*!
@brief
  Stops pimd service.

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
/*====================================================*/
void QCMAP_ConnectionManager::StopMcastDaemon(void)
{
  ds_system_call("killall -15 pimd", strlen("killall -15 pimd"));
}


/*======================================================
  FUNCTION GetGatewayIP
======================================================*/
/*!
@brief
  Gets the Gateway IP assigned to the STA interface.

@parameters
  uint32 *gwIP

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetGatewayIP(uint32 *gwIP)
{
  FILE *fp = NULL;
  char gatewayIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  in_addr addr;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qmi_error_type_v01 qmi_err_num;
  int ret, retries;

  if ( gwIP == NULL )
  {
     LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
     return false;
  }

  if (NULL == QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul not available", 0, 0, 0);
    return false;
  }

  memset(gatewayIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(devname, 0, sizeof(devname));

  /*----------------------------------------------------------
    Get the default gateway info from dhcpcd-<iface>.info.
    The info file may take few seconds to populate.
    Hence add a retry mechanism
  ----------------------------------------------------------*/
  for ( retries=0; retries < MAX_DHCPCD_INFO_RETRY; retries++)
  {
    ds_system_call("rm -rf /etc/gateway.txt",
                   strlen("rm -rf /etc/gateway.txt"));

    ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num);

    snprintf(command, MAX_COMMAND_STR_LEN,
             "cat /var/lib/dhcpcd-%s.info | grep GATEWAYS | tr -d \"'\" | awk '{split($0,a,/=/); print a[2]}' > /etc/gateway.txt", devname);

    ds_system_call(command, strlen(command));
    fp = fopen("/etc/gateway.txt", "r");

    if ( fp == NULL )
    {
      LOG_MSG_ERROR("Error opening Gateway IP file err : %d. Retrying\n", errno, 0, 0);
      sleep(DHCPCD_INFO_RETRY_DELAY);
      continue;
    }

    if (fscanf(fp, "%s", gatewayIP) != 1)
    {
      LOG_MSG_ERROR("Error reading Getway IP file err: %d.Retrying\n", errno, 0, 0);
      fclose(fp);
      sleep(DHCPCD_INFO_RETRY_DELAY);
      continue;
    }
    else
    {
      fclose(fp);
      break;
    }
  }

  if (retries == MAX_DHCPCD_INFO_RETRY)
  {
    LOG_MSG_ERROR("Reached max number of retries in getting default gateway", 0, 0, 0);
    return false;
  }

  memset(&addr,0,sizeof(in_addr));
  if (inet_aton(gatewayIP, &addr))
  {
    *gwIP= addr.s_addr;
  }
  else
  {
    LOG_MSG_ERROR("Invalid Getway IP: %d.\n", errno,0,0);
    return false;
  }

  ds_log_med("STA Gateway IP: 0x%s\n", inet_ntoa(addr));

  return true;
}

/*=====================================================
  FUNCTION readable_addr
======================================================*/
/*!
@brief
  Converts the address iin intgeger to readable string.

@parameters
  int            domain
  const uint32_t *addr
  char           *str

@return
  -1 - on failure
   0 - on success
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
int QCMAP_ConnectionManager::readable_addr
(
  int domain,
  const uint32_t *addr,
  char *str
)
{
  if ( inet_ntop(domain, (void *)addr, str, INET6_ADDRSTRLEN) == NULL )
  {
    LOG_MSG_ERROR("\n Not in presentation format \n",0,0,0);
    return -1;
  }
  return 0;
}


/*======================================================
  FUNCTION ReadQCMAPConfigFromXML
======================================================*/
/*!
@brief
  Reads the QCMAP Configuration from XML

@parameters
  int            domain
  const uint32_t *addr
  char           *str

@return
  -1 - on failure
   0 - on success
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/

boolean QCMAP_ConnectionManager::ReadQCMAPConfigFromXML(char *xml_path)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!xml_file.load_file(xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);

  this->auto_connect = atoi(root.child(AutoConnect_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  strlcpy(this->cfg.firewall_config_file,root.child(Firewall_TAG).child_value(),
          QCMAP_CM_MAX_FILE_LEN);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  this->wlan_restart_delay = atoi(root.child(WlanRestartDelay_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPBootUp_TAG);
 this->cfg.bootup_config.enable_mobileap_at_bootup =
                             atoi(root.child(MobileAPEnable_TAG).child_value());
 this->cfg.bootup_config.enable_wlan_at_bootup =
                                atoi(root.child(WLANEnable_TAG).child_value());
  return true;
}


/*=====================================================
  FUNCTION GetMobileAPhandle
======================================================*/
/*!
@brief
  Gets the status of mobileap handle.

@parameters
  qcmap_msgr_get_firewall_entry_resp_msg_v01 *resp_msg
  uint32_t handle

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
int  QCMAP_ConnectionManager::GetMobileAPhandle(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (QcMapMgr)
    return QcMapMgr->qcmap_cm_handle;
  else
    return false;
}


/*======================================================
  FUNCTION SetQCMAPBootupConfig
======================================================*/
/*!
@brief
  Set QCMAP Bootup Configuration

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::SetQCMAPBootupConfig
(
qcmap_msgr_bootup_flag_v01 mobileap_enable,
qcmap_msgr_bootup_flag_v01 wlan_enable,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!QcMapMgr)
  {
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  LOG_MSG_INFO1( "QCMAP_ConnectionManager::SetQCMAPBootupConfig()\
                 Enter  mobileap_enable =%d, wlan_enable =%d",
                 mobileap_enable, wlan_enable,0 );

  // Write to MobileAP Bootup Flag
  if (mobileap_enable != QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01)
  {
    QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup =
               (mobileap_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01)? true : false;

    QCMAP_ConnectionManager::GetSetBootupConfigFromXML
                       (CONFIG_MOBILEAP,SET_VALUE,
                        QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup);
  }

  // Write to WLAN Bootup Flag
  if (wlan_enable != QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01)
  {
    QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup =
                   (wlan_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01)? true : false;

    QCMAP_ConnectionManager::GetSetBootupConfigFromXML
                            (CONFIG_WLAN,SET_VALUE,
                             QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup);
  }
  return true;
}


/*======================================================
  FUNCTION GetQCMAPBootupConfig
======================================================*/
/*!
@brief
  Get QCMAP Bootup Configuration

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetQCMAPBootupConfig
(
  qcmap_msgr_bootup_flag_v01 *mobileap_enable,
  qcmap_msgr_bootup_flag_v01 *wlan_enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!QcMapMgr)
  {
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }
   LOG_MSG_INFO1("QCMAP_ConnectionManager::SetQCMAPBootupConfig() Enter  "
                 "CFG mobileap_enable =%d, wlan_enable =%d",
                 QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup,
                 QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup,0);

   *mobileap_enable = QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup;
   *wlan_enable= QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup;
   return true;
  }


/*=====================================================
  FUNCTION GetKernelVer
======================================================*/
/*!
@brief
  Queries the kernel version.

@parameters
  char *version

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetKernelVer
(
  char *version
)
{
  FILE *fp = NULL;
  char command[MAX_COMMAND_STR_LEN];
  char kernel_ver[KERNEL_VERSION_LENGTH];

  if ( version == NULL )
  {
     LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
     return false;
  }
  memset(kernel_ver, 0, KERNEL_VERSION_LENGTH);

  snprintf( command, MAX_COMMAND_STR_LEN,
            "uname -r | awk '{print $1}' > /etc/kernel_ver.txt");
  ds_system_call(command, strlen(command));

  fp = fopen("/etc/kernel_ver.txt", "r");

  if ( fp == NULL )
  {
     LOG_MSG_ERROR("Error opening Kernel version file: %d.\n", errno, 0, 0);
     return false;
  }

  if (fscanf(fp, "%s", kernel_ver) != 1)
  {
    LOG_MSG_ERROR("Error reading Kernel version file: %d.\n", errno, 0, 0);
    fclose(fp);
    return false;
  }
  memcpy(version,kernel_ver,strlen(kernel_ver)+1);
  fclose(fp);
  return true;
}

/*======================================================
  FUNCTION RestoreFactoryConfig
======================================================*/
/*!
@brief
  Restore to Factory Configuration

@parameters
   none

@return
   returns succesful
/*====================================================*/
boolean QCMAP_ConnectionManager::RestoreFactoryConfig
(
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  bzero(&command,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("QCMAP_ConnectionManager::RestoreFactoryConfig()",0,0,0);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "cp %s %s ",FACTORY_CONF,QcMapMgr->xml_path);
  ds_system_call(command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN,
            "rm -f %s ",QcMapMgr->cfg.firewall_config_file);
  ds_system_call(command, strlen(command));

  /* sleep for 500 miliseconds */
  sleep(QCMAP_RESET_CONFIG_TIMEOUT);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "reboot",strlen("reboot"));
  ds_system_call(command, strlen(command));
  return true;
}
/*=====================================================
  FUNCTION MatchMacAddrInList
======================================================*/
/*!
@brief
  - Match the MAC address
  - If a match is found, store the IP address of the client in a Linked List
@parameters
  Pointer to qcmap_nl_addr_t
@return
  none
@note
- Dependencies
- None
- Side Effects
- None
*/
/*=====================================================*/
bool QCMAP_ConnectionManager::MatchMacAddrInList
(
  qcmap_nl_addr_t* nl_addr
)
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  qmi_error_type_v01 qmi_err_num;
  struct ps_in6_addr *prefix_ptr, *nl_prefix_ptr;
  uint8 origIPv6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8 ipv6PrefixLen = 0;
  int qcmap_cm_error;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qcmap_cm_nl_prefix_info_t   ipv6_del_prefix_info;
  in_addr addr;
  int ret;
  char ipv6addr[INET6_ADDRSTRLEN];
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  struct ps_in6_addr *global_addr_ptr = NULL, *iid_addr_ptr = NULL;
  struct in6_addr null_ipv6_address;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(origIPv6,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memset(ipv6addr, 0, INET6_ADDRSTRLEN);
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(&null_ipv6_address, 0, sizeof(struct in6_addr));

  ret = QCMAP_ConnectionManager::GetDeviceName( devname,
                                                QCMAP_MSGR_IP_FAMILY_V6_V01,
                                                &qmi_err_num );
  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("\nMatchMacAddrInList - Linked list head is NULL \n",0,0,0);
    return false;
  }

  ds_mac_addr_ntop(nl_addr->mac_addr, mac_addr_char);
  ds_log_med("match mac addr %s", mac_addr_char);

  node = ds_dll_search (addrList->addrListHead , (void*)nl_addr->mac_addr,
                        QcMapMgr->qcmap_match_mac_address);
  if( node == NULL)
  {
    LOG_MSG_ERROR("\nMatchMacAddrInList - No match for MAC address "
                  "found in the list\n",0,0,0);
    return false;
  }
  else
  {
    //When a node is found, update the IP address from the netlink event
    list_data = (qcmap_cm_client_data_info_t*)(node->data);
    if (list_data == NULL)
    {
      LOG_MSG_ERROR("MatchMacAddrInList - Error in fetching node data ",0,0,0);
      return false;
    }
    ds_mac_addr_ntop(list_data->mac_addr, mac_addr_char);
    ds_log_med(" QCMAP_ConnectionManager::MatchMacAddrInList() Found node with MAC addr %s \n",
               mac_addr_char);
    if(nl_addr->isValidIPv4address == true && nl_addr->ip_addr != 0)
    {
      if (list_data->ip_addr == nl_addr->ip_addr)
      {
        addr.s_addr = nl_addr->ip_addr;
        ds_log_med(" Duplicate NEWNEIGH Event recieved for IP %s \n",inet_ntoa(addr));
      }
      else
      {
        addr.s_addr = nl_addr->ip_addr;
        ds_log_med("Adding a new entry for IP %s \n",inet_ntoa(addr));
        list_data->ip_addr = nl_addr->ip_addr;
        if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
        {
          if (!QCMAPLANMgr->AddEbtablesRuleForBridgeMode(list_data))
            LOG_MSG_ERROR( "Unable to Add Ebtables rules for NEWNEIGH AP-STA Bridge Mode",
                           0,0,0 );
        }
      }
    }
    memcpy(origIPv6, nl_addr->ip_v6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    nl_prefix_ptr = (struct ps_in6_addr*)(nl_addr->ip_v6_addr);

    if (QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32)){
      LOG_MSG_INFO1("MatchMacAddrInList - LL address received ",0,0,0);
    }

    if((nl_addr->isValidIPv6address == true) &&
         !(QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32))&& QcMapBackhaulMgr != NULL && ret != 0)
    {

      {
        memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
        QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
        QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
        QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(QcMapBackhaulMgr->ipv6_prefix_info));
        prefix_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)&
                     (QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr);

        //Store the original Ipv6 address
        memcpy(origIPv6, nl_addr->ip_v6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        nl_prefix_ptr = (struct ps_in6_addr*)(nl_addr->ip_v6_addr);
        ipv6PrefixLen = QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;

        /* Zero out the non prefix bits. */
        if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
        {
          prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
          prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
          nl_prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
          nl_prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
        }
      }
      /* Zero out the IID part */
      prefix_ptr->ps_s6_addr64[1] = 0;
      nl_prefix_ptr->ps_s6_addr64[1] = 0;

      if (!memcmp(prefix_ptr->ps_s6_addr64,
                   nl_prefix_ptr->ps_s6_addr64,
                   sizeof(uint64)))
      {
        if (memcmp(&null_ipv6_address, (struct in6_addr *)list_data->link_local_v6_addr,
             sizeof(struct in6_addr) ) != 0)
        {
          /* Update only prefix part. IID part will be from LL address. */
          global_addr_ptr = (struct ps_in6_addr *)origIPv6;
          iid_addr_ptr = (struct ps_in6_addr *)list_data->link_local_v6_addr;
          global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
        }
        memcpy (list_data->ip_v6_addr,origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        inet_ntop(AF_INET6,(void *)list_data->ip_v6_addr,ipv6addr, INET6_ADDRSTRLEN);
            ds_log_med("MatchMacAddrInList- New V6 Address %s \n",ipv6addr);
      }
      else if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated)
      {
          if (qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED)
          {
            if (memcmp(&null_ipv6_address, (struct in6_addr *)list_data->link_local_v6_addr,
                 sizeof(struct in6_addr) ) != 0)
            {
              /* Update only prefix part. IID part will be from LL address. */
              global_addr_ptr = (struct ps_in6_addr *)origIPv6;
              iid_addr_ptr = (struct ps_in6_addr *)list_data->link_local_v6_addr;
              global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
            }
            memcpy (list_data->ip_v6_addr,origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
            inet_ntop(AF_INET6,(void *)list_data->ip_v6_addr,ipv6addr, INET6_ADDRSTRLEN);
            ds_log_med("MatchMacAddrInList- New V6 Address %s \n",ipv6addr);
          }
          else
          {
            memset(&ipv6_del_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
            ipv6_del_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
            ipv6_del_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
            ipv6_del_prefix_info.prefix_len =  (unsigned char) IPV6_MIN_PREFIX_LENGTH;
            memcpy(((struct sockaddr_in6*)&(ipv6_del_prefix_info.prefix_addr))->sin6_addr.s6_addr,
                   list_data->ip_v6_addr, ipv6PrefixLen);
          if( QcMapBackhaulMgr &&
              QcMapBackhaulMgr->UpdatePrefix(&ipv6_del_prefix_info,true, true,
              list_data->link_local_v6_addr) == -1)
            {
              LOG_MSG_ERROR("Error: Unable to deprecate prefix", 0, 0, 0);
              return false;
            }
          }
      }
  }
  else if ((nl_addr->isValidIPv6address == true) &&
            QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32))
  {
    //Copy the link local Ipv6 address of the client
    if ((memcmp(&null_ipv6_address, (struct in6_addr *)list_data->link_local_v6_addr,
         sizeof(struct in6_addr)) == 0))
      {
        memcpy (list_data->link_local_v6_addr,
                origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        if(QcMapBackhaulMgr)
          QcMapBackhaulMgr->SendRSForClient(list_data->link_local_v6_addr);
      }
      if ( memcmp(&null_ipv6_address, (struct in6_addr *)list_data->ip_v6_addr,
           sizeof(struct in6_addr) ) )
      {
        /* Update the global address with this IID. */
        global_addr_ptr = (struct ps_in6_addr *)list_data->ip_v6_addr;
        iid_addr_ptr = (struct ps_in6_addr *)list_data->link_local_v6_addr;
        global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
      }
      inet_ntop(AF_INET6,(void *)list_data->link_local_v6_addr,ipv6addr, INET6_ADDRSTRLEN);
      ds_log_med("MatchMacAddrInList- New LL V6 Address %s \n",ipv6addr);
    }
  }
  LOG_MSG_INFO1("bye match mac addr in list",0,0,0);
  return true;

}


/*======================================================
  FUNCTION DeleteDeviceEntryInfo
=======================================================
@brief
  Remove the client entry (node) from the List

@parameters
  - MAC address of client
  - Device type to indicate device is USB or WLAN device
@return
  none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
bool QCMAP_ConnectionManager::DeleteDeviceEntryInfo
(
  void* mac_addr,
  qcmap_msgr_device_type_enum_v01 device_type,
  qcmap_cm_cdi_del_evt_t del_evt
)
{
  ds_dll_el_t * node;
  uint8 mac_addr_hex[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qmi_error_type_v01 qmi_err_num;
  in6_addr tmpipv6;
  char command[MAX_COMMAND_STR_LEN];


  LOG_MSG_INFO1("\n In DeleteDeviceEntryInfo \n",0,0,0);
  if (mac_addr == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - mac address pointer have NULL value \n",0,0,0);
    return false;
  }
  /*Removing entry for dnsmasq script cointaining this mac address*/
  snprintf(command, sizeof(command), "sed -i \"/%s/d\" %s",mac_addr,TEMP_HOST_FILE);
  ds_system_call(command, strlen(command));

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - Linked list head is NULL \n",0,0,0);
    return false;
  }

  if ((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01)||
      (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01))
  {
    ds_log_med("DeleteDeviceEntryInfo- Delete client mac %s, Device_type:%d \n",
               mac_addr, device_type);
    //Convert the MAC from char to hex
    if(!ds_mac_addr_pton((char*)mac_addr, mac_addr_hex))
    {
      LOG_MSG_ERROR("DeleteDeviceEntryInfo - Error in MAC address conversion",
                    0,0,0);
      return false;
    }
    mac_addr = (void*)mac_addr_hex;
  }
  else
  {
    memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
    ds_mac_addr_ntop(mac_addr, mac_addr_char);
    ds_log_med("DeleteDeviceEntryInfo- Delete client mac %s, Device_type:%d \n",
               mac_addr_char, device_type);
  }

  if ((node = ds_dll_search (addrList->addrListHead , (void*)mac_addr,
                              qcmap_match_mac_address)) == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - Error in finding the node",0,0,0);
    return false;
  }
  else
  {
    connectedDevicesList = (qcmap_cm_client_data_info_t*)ds_dll_data(node);
    if (connectedDevicesList == NULL)
    {
      LOG_MSG_ERROR("DeleteDeviceEntryInfo - The device information"
                     "node is NULL\n",0,0,0);
      return false;
    }

    if (del_evt == QCMAP_DELNEIGH)
    {
      if((connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01))
      {
        LOG_MSG_ERROR("DeleteDeviceEntryInfo - WLAN information will not be deleted"
                      "based on DELNEIGH",0,0,0);
        return false;
      }
    }
    else if (del_evt == QCMAP_AP_STA_DISCONNECTED)
    {
      if (connectedDevicesList->device_type != device_type)
      {
        LOG_MSG_ERROR("Current device type %d not same as present in the list %d",
                      device_type, connectedDevicesList->device_type, 0);

        return false;
      }
    }
  }


  //Delete the node from the list
  if ((node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),
       (void*)mac_addr, QcMapMgr->qcmap_match_mac_address))== NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - Error in finding the node",0,0,0);
    return false;
  }

  //Free the memory for the device details structure
  connectedDevicesList = (qcmap_cm_client_data_info_t*)ds_dll_data(node);

  if (connectedDevicesList == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - The device information"
                  "node is NULL\n",0,0,0);
    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
    return false;
  }

  //Delete the conntrack entry for the device
  QCMAP_Tethering::DelConntrack( connectedDevicesList->ip_addr );

  QcMapMgr->DelV6Conntrack(connectedDevicesList->ip_v6_addr,
                           connectedDevicesList->mac_addr);

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->DeleteDelegatedPrefix(connectedDevicesList->ip_v6_addr,
                                                &qmi_err_num);
  }
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if (QCMAPLANMgr->DelEbtablesRuleForBridgeMode(connectedDevicesList) == false)
    {
      ds_log_med("Could not Delete Ebtables rules for device %s\n",mac_addr);
    }
  }

  //Free the device information structure
  ds_free (connectedDevicesList);
  connectedDevicesList = NULL;

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
  QcMapMgr->conn_device_info.numOfNodes--;
  LOG_MSG_INFO1("\n Number of Connected Devices: %d\n",
                QcMapMgr->conn_device_info.numOfNodes, 0, 0);

  if (device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
       device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01 )
    {
      QcMapMgr->conn_device_info.numofWlanclients--;
    }

#ifdef FEATURE_DATA_TARGET_MDM9607
  if (QcMapMgr->usb_clk_node_present)
  {
    if ( (QcMapMgr->conn_device_info.numofWlanclients == 0))
    {
      QcMapMgr->conn_device_info.wlanclientpresent = false;
      if (!(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)&&
            (QcMapMgr->CheckRatforLTE()) &&
            ((qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED)||
             (qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED))))
      {
        QcMapMgr->SetUSBClockFreq(false);
      }
    }
  }
#endif
  return true;
}

/*======================================================
  FUNCTION AddNewDeviceEntry
======================================================*/
/*!
@brief
  Stores the MAC address and interface name of client in a list.

@parameters
  - MAC address of client
  - Device type
  - Ipv4 address
  - IPv6 address
  - bool to check valid ipv4 address
  - bool to check valid ipv6 address
@return
  none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
bool QCMAP_ConnectionManager::AddNewDeviceEntry
(
  void* mac_addr,
  qcmap_msgr_device_type_enum_v01 device_type,
  uint32* ip_addr,
  uint8* ipv6_addr,
  bool isIpv4valid,
  bool isIpv6valid
)
{
  ds_dll_el_t * node = NULL;
  qmi_error_type_v01 qmi_err_num;
  int qcmap_cm_error;
  int ret;
  struct ps_in6_addr *prefix_ptr, *nl_prefix_ptr;
  uint8 ipv6PrefixLen = 0;
  uint8 origIPv6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char ipv6addr[INET6_ADDRSTRLEN];
  struct ps_in6_addr *global_addr_ptr = NULL, *iid_addr_ptr = NULL;
  struct in6_addr null_ipv6_address;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  if(!QcMapMgr)
    return false;

  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  if(!QcMapWLANMgr)
  {
  LOG_MSG_INFO1("WLAN object absent",0,0,0);
  }
  if ( (device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
       device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) && !QcMapWLANMgr)
  {
    LOG_MSG_ERROR("\n WLAN is not enabled. Do not add this entry\n",
                   0 ,0, 0);
    return false;
  }

  connectedDevicesList =
  (qcmap_cm_client_data_info_t*)ds_malloc(sizeof(qcmap_cm_client_data_info_t));

  if( connectedDevicesList == NULL )
  {
    LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for"
                  "device information structure",0,0,0);
    return false;
  }

  memset (connectedDevicesList,0,sizeof(qcmap_cm_client_data_info_t));
  memset (origIPv6, 0 ,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(ipv6addr, 0, INET6_ADDRSTRLEN);
  memset(&null_ipv6_address, 0, sizeof(struct in6_addr));

  /* NOTE - With AP-AP-STA mode, we need to set the device type accordingly*/
  ds_mac_addr_ntop((char*)mac_addr, mac_addr_char);
  ds_log_med("AddNewDeviceEntry- Add new address for mac %s \n",mac_addr_char);


  // Store Client MAC Address in QCMAP context
  memcpy( connectedDevicesList->mac_addr, (uint8*)mac_addr,
          QCMAP_MSGR_MAC_ADDR_LEN_V01);

  //Store the device type to the QCMAP context
  connectedDevicesList->device_type = device_type;

  /* For USB Clients AddNewDeviceEntry() is called from NEWNEIGH events.
   * Hence update IP address too.*/
  if(device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    if (isIpv4valid == true)
    {
      connectedDevicesList->ip_addr = *ip_addr;
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        if (!QCMAPLANMgr->AddEbtablesRuleForBridgeMode(connectedDevicesList))
          LOG_MSG_ERROR("Unable to Add Ebtables rules for NEWNEIGH AP-STA Bridge Mode",
                        0,0,0);
      }
    }

    //Store the original Ipv6 address
    if (isIpv6valid == true)
    {
      memcpy(origIPv6, ipv6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      nl_prefix_ptr = (struct ps_in6_addr*)(ipv6_addr);
     if(!(QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32)) &&
         (QCMAP_Backhaul_WLAN::IsSTAAvailableV6() ||
          QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
          QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() ||
         (qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED)))
     {
        if(!(ret = QCMAP_ConnectionManager::GetDeviceName(devname,
            QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num)))
          return 0;
        if(QcMapBackhaulMgr)
        {
          memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
          QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(QcMapBackhaulMgr->ipv6_prefix_info));
          prefix_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)&
                  (QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr);

          ipv6PrefixLen = QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;

          /* Zero out the non prefix bits. */
          if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
          {
            prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
            prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
            nl_prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
            nl_prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
          }
        }
        /* Zero out the IID part */
        prefix_ptr->ps_s6_addr64[1] = 0;
        nl_prefix_ptr->ps_s6_addr64[1] = 0;

        if ((!memcmp(prefix_ptr->ps_s6_addr64,
                     nl_prefix_ptr->ps_s6_addr64,
                     sizeof(uint64))))
        {
          if (memcmp(&null_ipv6_address,
              (struct in6_addr *)connectedDevicesList->link_local_v6_addr,
               sizeof(struct in6_addr)) != 0)
          {
            /* Update only prefix part. IID part will be from LL address. */
            global_addr_ptr = (struct ps_in6_addr *)origIPv6;
            iid_addr_ptr = (struct ps_in6_addr *)connectedDevicesList->link_local_v6_addr;
            global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
          }
          memcpy(connectedDevicesList->ip_v6_addr,origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          inet_ntop(AF_INET6,(void *)connectedDevicesList->ip_v6_addr,
                    ipv6addr, INET6_ADDRSTRLEN);
          ds_log_med("AddNewDeviceEntry- New V6 Address %s \n",ipv6addr);
        }
      }
      else if (QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32))
      {
        //Copy the link local Ipv6 address of the client
        if (memcmp(&null_ipv6_address,
            (struct in6_addr *)connectedDevicesList->link_local_v6_addr,
            sizeof(struct in6_addr)) == 0)
        {
          memcpy (connectedDevicesList->link_local_v6_addr,origIPv6,
                  QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          if (QcMapBackhaulMgr)
            QcMapBackhaulMgr->SendRSForClient(connectedDevicesList->link_local_v6_addr);
        }
        memcpy(connectedDevicesList->link_local_v6_addr,origIPv6,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        if (memcmp(&null_ipv6_address, (struct in6_addr *)connectedDevicesList->ip_v6_addr,
            sizeof(struct in6_addr)) != 0)
        {
          /* Update the global address with this IID. */
          global_addr_ptr = (struct ps_in6_addr *)connectedDevicesList->ip_v6_addr;
          iid_addr_ptr = (struct ps_in6_addr *)connectedDevicesList->link_local_v6_addr;
          global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
        }
        /* Update the global address with this IID. */
        inet_ntop(AF_INET6,(void *)connectedDevicesList->link_local_v6_addr,
                  ipv6addr, INET6_ADDRSTRLEN);
        ds_log_med("AddNewDeviceEntry- New LL V6 Address %s \n",ipv6addr);
      }
    }
  }
  if (addrList->addrListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
           information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for node",
                    0,0,0);
      ds_free(connectedDevicesList);
      connectedDevicesList = NULL;
      return false;
    }
    addrList->addrListHead = node;
  }

  //Store the mac address, interface name in the linked list
  if ((node = ds_dll_enq(addrList->addrListHead,
                         NULL, (void*)connectedDevicesList )) == NULL)
  {
    LOG_MSG_ERROR("AddNewDeviceEntry - Error in adding a node",0,0,0);
    ds_free(connectedDevicesList);
    connectedDevicesList = NULL;
    return false;
  }
  QcMapMgr->conn_device_info.numOfNodes++;
  LOG_MSG_INFO1("\n Number of Connected Devices: %d\n",
                QcMapMgr->conn_device_info.numOfNodes, 0, 0);
  //Update the tail pointer when a new node is added everytime
  addrList->addrListTail = node;

  if (device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
       device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)
    {
      QcMapMgr->conn_device_info.numofWlanclients++;
  }
#ifdef FEATURE_DATA_TARGET_MDM9607
  if (QcMapMgr->usb_clk_node_present)
  {
    if (!QcMapMgr->CheckWlanClientConnected())
      {
        QcMapMgr->conn_device_info.wlanclientpresent = true;
        if(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
        {
        QcMapMgr->SetUSBClockFreq(true);
        }
      }
    }
#endif
  return true;
}


/*=====================================================
  FUNCTION GetConnectedDevicesInfo
======================================================*/
/*!
@brief
 Fetches the list of devices connected to the Access point

@parameters
  - Pointer to qcmap_msgr_connected_device_info_v01
  - Number of device entries
  - Pointer to qmi_error_type_v01

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/


boolean QCMAP_ConnectionManager::GetConnectedDevicesInfo
(
 qcmap_msgr_connected_device_info_v01 *conn_devices,
 unsigned int* num_entries,
 qmi_error_type_v01 *qmi_err_num
)
{
  int i = 0;
  FILE *fd = NULL;
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  char command[MAX_COMMAND_STR_LEN];
  LOG_MSG_INFO1("QCMAP_ConnectionManager:GetConnectedDevicesInfo",0,0,0);

  if (conn_devices == NULL)
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return false;
  }

  //Fetch the number of nodes in linked list and store in num_entries
  *num_entries = QcMapMgr->conn_device_info.numOfNodes;
  if ( *num_entries == 0 )
  {
    LOG_MSG_INFO1("GetConnectedDevicesInfo - No devices present",0,0,0);
    return true;
  }

  //Delete the hostname.txt file if present
  snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
  ds_system_call(command, strlen(command));

  //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
  snprintf(command, MAX_COMMAND_STR_LEN,
           "cut -f1,2,4 -d ' ' /var/lib/misc/dnsmasq.leases > %s",
           HOSTNAME_PATH);
  ds_system_call(command, strlen(command));

  //Open the hostname.txt file to fetch lease time and hostname
  fd = fopen(HOSTNAME_PATH,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("GetConnectedDevicesInfo - Error in opening %s",
                  HOSTNAME_PATH,0,0);
    return false;
  }

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("GetConnectedDevicesInfo - Linked list head is NULL"
                  "Cant fetch connected devices info",0,0,0);
    fclose(fd);
    return false;
  }

  node = addrList->addrListHead;
  node = ds_dll_next (node, (const void**)(&connectedDevicesList));
  if ( node == NULL)
  {
    LOG_MSG_ERROR("GetConnectedDevicesInfo - Device info is NULL"
                  "Cant fetch connected devices info",0,0,0);
    fclose(fd);
    return false;
  }

  for ( i=0; i < *num_entries && connectedDevicesList; i++ )
  {

    fetchHostNameAndLeasetime(fd ,connectedDevicesList->mac_addr,
                              connectedDevicesList->host_name,
                              &conn_devices[i].lease_expiry_time);

    //Copy the MAC, device type, IPv4 and IPv6 address
    memcpy (conn_devices[i].client_mac_addr, connectedDevicesList->mac_addr,
            QCMAP_MSGR_MAC_ADDR_LEN_V01);

    //Copy CDI hostname to client responce buffer

    memcpy (conn_devices[i].host_name,connectedDevicesList->host_name,
            QCMAP_MSGR_DEVICE_NAME_MAX_V01);


    if( connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01 &&
        QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
    {
      conn_devices[i].device_type  = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
    }
    else
    {
      conn_devices[i].device_type = connectedDevicesList->device_type;
    }
    conn_devices[i].ipv4_addr = connectedDevicesList->ip_addr;
    memcpy (conn_devices[i].ll_ipv6_addr,
            connectedDevicesList->link_local_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    memcpy (conn_devices[i].ipv6_addr,
            connectedDevicesList->ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    //Point to the next node
    node = ds_dll_next (node, (const void**)(&connectedDevicesList));
  }

  //Close the hostname.txt file
  fclose(fd);
  return true;
}


/*=====================================================
  FUNCTION GetDeviceName
======================================================*/
/*!
@brief
 Returns current active backhaul

@parameters
  - devname
  - IP family
  - Pointer to qmi_error_type_v01

@return
  int
@note
  Use the return values to indicate the type of backhaul being used.

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

int QCMAP_ConnectionManager::GetDeviceName
(
  char *devname,
  qcmap_msgr_ip_family_enum_v01 family_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if ( qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
                              family_type,
                              devname, &qcmap_cm_errno) != QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", 0,0,0);
      *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
      return 0;
    }
    return BACKHAUL_TYPE_WWAN;
  }
  else if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    strlcpy(devname, ETH_IFACE, strlen(ETH_IFACE)+1);
    return BACKHAUL_TYPE_ETHERNET;
  }
  else if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
      strlcpy(devname, BRIDGE_IFACE, strlen(BRIDGE_IFACE)+1);
    else
      strlcpy(devname, ECM_IFACE, QCMAP_MSGR_INTF_LEN);

    return BACKHAUL_TYPE_CRADLE;
  }
  else if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE &&
           QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    strlcpy(devname, BRIDGE_IFACE, strlen(BRIDGE_IFACE)+1);
    return BACKHAUL_TYPE_AP_STA_BRIDGE;
  }
  else if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
    strlcpy( devname, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,\
             QCMAP_MSGR_INTF_LEN);
    return BACKHAUL_TYPE_AP_STA_ROUTER;
  }

  return 0;
}



/*=====================================================
  FUNCTION GetSetBootupConfigFromXML
======================================================*/
/*!
@brief
 Gets bootup config from XML

@parameters
  - enum variable of qcmap_bootup_config_enum
  - enum variable of qcmap_action_type
  - boolean to specify mode

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean QCMAP_ConnectionManager::GetSetBootupConfigFromXML
(
  qcmap_bootup_config_enum conf,
  qcmap_action_type action,
  boolean mode
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  const char *tag_ptr;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPBootUp_TAG);

  switch (conf)
  {
    case CONFIG_MOBILEAP:
      tag_ptr = MobileAPEnable_TAG;
    break;

    case CONFIG_WLAN:
      tag_ptr = WLANEnable_TAG;
    break;

    default:
      LOG_MSG_ERROR("Invalid TAG.", 0, 0, 0);
      return false;
    break;
  }

  if (action == SET_VALUE)
  {
    snprintf(data, sizeof(data), "%d", mode);
    root.child(tag_ptr).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    atoi(root.child(tag_ptr).child_value())? mode = true: mode =false;
  }

  LOG_MSG_INFO1("Config: %d Action: %d value: %d" , conf, action, mode);
  return true;
}

/*===========================================================================
 * FUNCTION GetHWAddr
==========================================================================*/
/*!
@brief
  Get Mac address for an interface

@parameters
   Char * to store retrieved MAC Address
   devname name of the iface

@return
   QCMAP_CM_SUCCESS
   QCMAP_CM_ERROR
@note

  - Dependencies
  - None

  - Side Effects
 - None
*/
/*=========================================================================*/
int QCMAP_ConnectionManager::GetHWAddr(char *mac,char* devname)
{
  int s, ret;
  struct ifreq buffer;
  struct sockaddr *sa;

  LOG_MSG_INFO1("GetHWAddr",0,0,0);

  if ( mac == NULL || devname == NULL )
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Open a socket */
  s = socket(PF_INET, SOCK_DGRAM, 0);
  if ( s < 0 )
  {
    LOG_MSG_ERROR("Unable to open socket to get IP address.\n",0,0,0);
    return QCMAP_CM_ERROR;
  }
  /* Set up the interface request buffer for wlan0. */
  memset(&buffer, 0x00, sizeof(buffer));
  strlcpy(buffer.ifr_name, devname, IFNAMSIZ);

  /* Call the ioctl to get the address. */
  ret = ioctl(s, SIOCGIFHWADDR, &buffer);

  if ( ret < 0 )
  {
    LOG_MSG_ERROR("Unable to call ioctl to get MAC address.\n",0,0,0);
    /* Close the socket handle. */
    return QCMAP_CM_ERROR;
  }

  /* Copy out the ip address for the interface. */
  sa = (struct sockaddr *)&(buffer.ifr_addr);
  memcpy(mac, buffer.ifr_hwaddr.sa_data, ETH_ALEN);

  ds_log_med("HW Address for dev %s is %02x:%02x:%02x:%02x:%02x:%02x \n",
                  devname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  close(s);
  return QCMAP_CM_SUCCESS;
}

/*======================================================
  FUNCTION:  WriteDefaultsToXML
  =====================================================*/
  /*!
      @brief
      Writes default values to XML in case if XML is not present

      @params

      @return
      QCMAP_CM_ERROR
      QCMAP_CM_SUCCESS
  */
/*====================================================*/
void QCMAP_ConnectionManager::WriteDefaultsToXML()
{
  FILE *fp = NULL;
  int i;
  struct in_addr addr;
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  uint8 *mac = NULL;
  char *xml_file=QCMAP_ConnectionManager::xml_path;
  uint32 dhcp_start_address;
  uint32 dhcp_end_address;
  uint32_t static_ip_addr;
  uint32_t static_dns_addr;
  uint32_t static_net_mask;
  uint32_t static_gw_ip;
  uint32_t dmz_ip;
  dhcp_start_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20;
  dhcp_end_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20 + MAX_WIFI_CLIENTS;
  static_ip_addr=0;
  static_net_mask=0;
  static_dns_addr=0;
  static_gw_ip=0;
  LOG_MSG_INFO1("Creating XML file using default values",0,0,0);
  fp = fopen(xml_file, "wb");

  /* File not found at the given path */
  if (NULL == fp)
  {
    return QCMAP_CM_ERROR;
  }

  fprintf(fp, "<?xml version=\"1.0\"?>\n");
  fprintf( fp, "<system xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
               " xsi:noNamespaceSchemaLocation=\"mobileap_cfg.xsd\">\n");
  fprintf(fp, "<MobileAPCfg>\n");
  // NAT Config
  fprintf(fp, "\t<MobileAPNatCfg>\n");
  fprintf(fp, "\t<Firewall>%s</Firewall>\n", QCMAP_FIREWALL_CONFIG);
  fprintf( fp, "\t<NatEntryGenericTimeout>%d</NatEntryGenericTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_GENERIC_TIMEOUT );
  fprintf( fp, "\t<NatEntryICMPTimeout>%d</NatEntryICMPTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_ICMP_TIMEOUT );
  fprintf( fp, "\t<NatEntryTCPEstablishedTimeout>%d</NatEntryTCPEstablishedTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_TCP_TIMEOUT );
  fprintf( fp, "\t<NatEntryUDPTimeout>%d</NatEntryUDPTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_UDP_TIMEOUT );

  addr.s_addr = htonl(dmz_ip);
  fprintf(fp, "\t<DmzIP>%s</DmzIP>\n",inet_ntoa(addr));
  fprintf(fp, "\t<EnableIPSECVpnPassthrough>%d</EnableIPSECVpnPassthrough>\n", true);
  fprintf(fp, "\t<EnablePPTPVpnPassthrough>%d</EnablePPTPVpnPassthrough>\n", true);
  fprintf(fp, "\t<EnableL2TPVpnPassthrough>%d</EnableL2TPVpnPassthrough>\n", true);
  fprintf(fp, "\t<EnableWebserverWWANAccess>%d</EnableWebserverWWANAccess>\n", false );
  fprintf(fp, "\t<NATType>SYM</NATType>\n");
  fprintf(fp, "\t<ALGCfg>\n");
  fprintf(fp, "\t\t<EnableRTSPAlg>%d</EnableRTSPAlg>\n", false );
  fprintf(fp, "\t\t<EnableSIPAlg>%d</EnableSIPAlg>\n", false );
  fprintf(fp, "\t</ALGCfg>\n");
  fprintf(fp, "</MobileAPNatCfg>\n");

  fprintf(fp, "<MobileAPLanCfg>\n");
  //Backhaul Config
  fprintf(fp, "\t<EnableIPV4>%d</EnableIPV4>\n",true);
  fprintf(fp, "\t<EnableIPV6>%d</EnableIPV6>\n",true);
  fprintf(fp,"\t<GatewayURL>%s</GatewayURL>\n",GATEWAY_URL);
  fprintf(fp,"\t<WlanMode>%s</WlanMode>\n",AP_TAG);
  fprintf(fp,"\t<WlanRestartDelay>%d</WlanRestartDelay>\n",WLAN_DEFAULT_DELAY);
  fprintf(fp, "\t<MobileAPSTABridgeEnable>%d</MobileAPSTABridgeEnable>\n", false);
  fprintf(fp, "\t<HostAPDCfg>%s</HostAPDCfg>\n", HOSTAPD_CFG_PATH);
  fprintf(fp, "\t<HostAPDEntropy>%s</HostAPDEntropy>\n", HOSTAPD_ENTROPY);
  fprintf(fp, "\t<STAModeHostAPDCfg>%s</STAModeHostAPDCfg>\n", STA_MODE_HOSTAPD_CFG_PATH);
  fprintf(fp, "\t<APIPAddr>%s</APIPAddr>\n", APPS_LAN_IP_ADDR);
  fprintf(fp, "\t<SubNetMask>%s</SubNetMask>\n", APPS_SUB_NET_MASK);

  //LAN Config
  fprintf(fp, "\t<EnableDHCPServer>%d</EnableDHCPServer>\n", true);
  fprintf(fp, "\t<DHCPCfg>\n");
  addr.s_addr = htonl(dhcp_start_address);
  fprintf(fp, "\t\t<StartIP>%s</StartIP>\n", inet_ntoa(addr));
  addr.s_addr = htonl(dhcp_end_address);
  fprintf(fp, "\t\t<EndIP>%s</EndIP>\n", inet_ntoa(addr));
  fprintf(fp, "\t\t<LeaseTime>%d</LeaseTime>\n", DHCP_LEASE_TIME);
  fprintf(fp, "\t</DHCPCfg>\n");
  fprintf(fp, "\t<IPPassthroughCfg>\n");
  fprintf(fp, "\t<IPPassthroughEnable>%d</IPPassthroughEnable>\n", false);
  fprintf(fp, "\t<IPPassthroughDevice>%d</IPPassthroughDevice>\n", -1);

  fprintf(fp, "\t<IPPassthroughUsbHost>NULL</IPPassthroughUsbHost>\n");
  fprintf(fp, "\t<IPPassthroughEthMac>NULL</IPPassthroughEthMac>\n");
  fprintf(fp, "\t</IPPassthroughCfg>\n");

  /* <GuestAPCfg> */
  fprintf(fp, "\t<GuestAPCfg>\n");
  fprintf(fp, "\t\t<HostAPDCfg>%s</HostAPDCfg>\n", HOSTAPD_CFG_PATH_SSID2);
  fprintf(fp, "\t\t<HostAPDEntropy>%s</HostAPDEntropy>\n",HOSTAPD_ENTROPY_SSID2);
  fprintf(fp, "\t\t<AccessProfile>%s</AccessProfile>\n",INTERNET_TAG);
  fprintf(fp, "\t</GuestAPCfg>\n");

  /* <StationModeCfg> */
  fprintf(fp, "\t<StationModeCfg>\n");
  fprintf(fp, "\t\t<SupplicantCfg>%s</SupplicantCfg>\n", SUPPLICANT_CFG_PATH);
  fprintf(fp, "\t\t<STAModeConnType>%d</STAModeConnType>\n",
          QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01);
  addr.s_addr = htonl(static_ip_addr);
  fprintf(fp, "\t\t<StaticIPAddr>%s</StaticIPAddr>\n", inet_ntoa(addr));
  addr.s_addr = htonl(static_dns_addr);
  fprintf(fp, "\t\t<StaticConfigDNSAddr>%s</StaticConfigDNSAddr>\n", inet_ntoa(addr));
  addr.s_addr = htonl(static_gw_ip);
  fprintf(fp, "\t\t<StaticConfigGWAddr>%s</StaticConfigGWAddr>\n", inet_ntoa(addr));
  addr.s_addr = htonl(static_net_mask);
  fprintf(fp, "\t\t<StaticConfigNetMask>%s</StaticConfigNetMask>\n", inet_ntoa(addr));
  fprintf(fp, "\t</StationModeCfg>\n");

  /* End MobileAPLanCfg tag */
  fprintf(fp, "</MobileAPLanCfg>\n");

  // WAN Config
  fprintf(fp, "<MobileAPWanCfg>\n");
  fprintf(fp, "\t<AutoConnect>%d</AutoConnect>\n", false);
  fprintf(fp, "\t<Roaming>%d</Roaming>\n", false);
  fprintf(fp, "\t<EriConfig>%s</EriConfig>\n", QCMAP_ERI_CONFIG);
  fprintf(fp, "\t<FirstPreferredBackhaul>eth</FirstPreferredBackhaul>\n");
  fprintf(fp, "\t<SecondPreferredBackhaul>usb_cradle</SecondPreferredBackhaul>\n");
  fprintf(fp, "\t<ThirdPreferredBackhaul>wlan</ThirdPreferredBackhaul>\n");
  fprintf(fp, "\t<FourthPreferredBackhaul>wwan</FourthPreferredBackhaul>\n");
  fprintf(fp, "\t<TECH>%s</TECH>\n", TECH_ANY_TAG);
  fprintf(fp, "\t<V4_UMTS_PROFILE_INDEX>%d</V4_UMTS_PROFILE_INDEX>\n", false);
  fprintf(fp, "\t<V4_CDMA_PROFILE_INDEX>%d</V4_CDMA_PROFILE_INDEX>\n", false);
  fprintf(fp, "\t<V6_UMTS_PROFILE_INDEX>%d</V6_UMTS_PROFILE_INDEX>\n", false);
  fprintf(fp, "\t<V6_CDMA_PROFILE_INDEX>%d</V6_CDMA_PROFILE_INDEX>\n", false);
  fprintf( fp, "\t<DefaultSIPServerConfigType>%s</DefaultSIPServerConfigType>\n",
            FQDN_TAG );
   fprintf( fp, "\t<DefaultSIPServerConfig>%s</DefaultSIPServerConfig>\n",
            DEFAULT_SIP_SERVER);
  fprintf(fp, "\t<PrefixDelegation>%d</PrefixDelegation>\n", false);
  fprintf(fp, "</MobileAPWanCfg>\n");

  //Daemon Services Config
  fprintf(fp, "<MobileAPSrvcCfg>\n");
  fprintf(fp, "\t<UPnP>%d</UPnP>\n", false);
  fprintf(fp, "\t<DLNA>%d</DLNA>\n", false);
  fprintf(fp, "\t<MDNS>%d</MDNS>\n", false);
  fprintf(fp, "</MobileAPSrvcCfg>\n");

  //DDNS Config
  fprintf(fp, "<DDNSCfg>\n");
  fprintf(fp, "\t<EnableDDNS>%d</EnableDDNS>\n",0);
  fprintf(fp, "\t<DDNSServer>%s</DDNSServer>\n",QCMAP_DYNDNS_SERVER_URL);
  fprintf(fp, "\t<DDNSProtocol>%s</DDNSProtocol>\n",QCMAP_DYNDNS_PROTOCOL);
  fprintf(fp, "\t<DDNSLogin>%s</DDNSLogin>\n",DDNS_DEFAULT_LOGIN);
  fprintf(fp, "\t<DDNSPassword>%s</DDNSPassword>\n",DDNS_DEFAULT_PASSWORD);
  fprintf(fp, "\t<DDNSHostname>%s</DDNSHostname>\n",DDNS_DEFAULT_HOSTNAME);
  fprintf(fp, "\t<DDNSTimeout>%d</DDNSTimeout>\n",DDNS_DEFAULT_TIMEOUT);
  fprintf(fp, "</DDNSCfg>\n");

  //TinyProxy
  fprintf(fp, "<TinyProxyCfg>\n");
  fprintf(fp, "\t<EnableTinyProxy>%d</EnableTinyProxy>\n",0);
  fprintf(fp, "</TinyProxyCfg>\n");

  /* Bootup Config Flags*/
  fprintf(fp, "<MobileAPBootUpCfg>\n");
  fprintf(fp, "\t<MobileAPEnableAtBootup>%d</MobileAPEnableAtBootup>\n", false );
  fprintf(fp, "\t<WLANEnableAtBootup>%d</WLANEnableAtBootup>\n", false );
  fprintf(fp, "</MobileAPBootUpCfg>\n");

  /* Cradle Flags */
  fprintf(fp, "<Cradle>\n");
  fprintf(fp, "\t<CradleMode>%d</CradleMode>\n",false);
  fprintf(fp, "</Cradle>\n");

  /* ETH Backhaul Flags */
  fprintf(fp, "<EthBackhaul>\n");
  fprintf(fp, "\t<EthBackhaulMode>%d</EthBackhaulMode>\n",
                        QCMAP_MSGR_ETH_BACKHAUL_LAN_ROUTER_V01);
  fprintf(fp, "</EthBackhaul>\n");

  fprintf(fp, "</MobileAPCfg>\n");
  fprintf(fp, "</system>\n");

  fclose(fp);
}

/*=====================================================================
  FUNCTION DeleteConnectedDevicesClients
======================================================================*/
/*!
@brief
  Delete all entries in Connected Devices module based on the device type passed.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================================*/
void QCMAP_ConnectionManager::DeleteConnectedDevicesClients
(
qcmap_msgr_device_type_enum_v01 device_type
)
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  qmi_error_type_v01 qmi_err_num;

  struct in_addr addr;
  char ip6_addr_buf[INET6_ADDRSTRLEN];
  in6_addr tmpipv6;
  char command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("DeleteConnectedDevicesClients() with device_type %d",\
                device_type,0,0);

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_INFO1("DeleteConnectedDevicesClients - Head Null -> No entries"\
                  " to delete \n",0,0,0);
    return;
  }

  memset(ip6_addr_buf, 0, INET6_ADDRSTRLEN);
  memset(command, 0, MAX_COMMAND_STR_LEN);
  memset(&tmpipv6, 0, sizeof(tmpipv6));

  // Delete devices from the connected device list
  node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),\
                     (void*)&device_type, QCMAPLANMgr->qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)ds_dll_data(node);
    // This case should not exist since we are getting the node based on node->data match in ds_dll_delete
    if (list_data == NULL)
    {
      LOG_MSG_ERROR("DeleteConnectedDevicesClients - The device information node is NULL\n",0,0,0);
    }
    else
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med("DeleteConnectedDevicesClients-- Deleting Client %s from"\
            "device connected list with type %d\n",inet_ntoa(addr),device_type);
      /* Delete connection track entries for wlan client */
      if ( addr.s_addr )
      {
        snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D --orig-src %s",\
          inet_ntoa(addr));
        ds_system_call(command, strlen(command));
        snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D --reply-src %s",\
          inet_ntoa(addr));
        ds_system_call(command, strlen(command));
      }

      if ( list_data->ip_v6_addr != NULL )
      {
        QcMapMgr->DelV6Conntrack(list_data->ip_v6_addr,
                                 list_data->mac_addr);
      }
      // Free the device information structure
      ds_free (list_data);
      list_data = NULL;
    }
    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
    QcMapMgr->conn_device_info.numOfNodes--;

      if ((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
           device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01))
      {
        QcMapMgr->conn_device_info.numofWlanclients--;
      }
#ifdef FEATURE_DATA_TARGET_MDM9607
    if (QcMapMgr->usb_clk_node_present)
    {
      if ( (QcMapMgr->conn_device_info.numofWlanclients == 0))
      {
        QcMapMgr->conn_device_info.wlanclientpresent = false;
        if (!(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)&&
            (QcMapMgr->CheckRatforLTE()) &&
            ((qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED)||
             (qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED))))
        {
          QcMapMgr->SetUSBClockFreq(false);
        }
      }
    }
#endif

    node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),\
                               (void*)&device_type,\
                               QCMAPLANMgr->qcmap_match_device_type);
  }
  if( addrList->addrListHead->next == NULL )
  {
    LOG_MSG_INFO1("\n No device info exist- Delete the head node\n",0,0,0);
    ds_dll_free(addrList->addrListHead);
    addrList->addrListHead = NULL;
  }
}

/*===========================================================================
  FUNCTION cleanIPv4FilterTableFwdChain
==========================================================================*/
/*!
@brief
  Deleted all the ipv4 forward chain rules added.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_ConnectionManager::CleanIPv4FilterTableFwdChain()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(devname, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
      /* Drop unstatefull tcp packets */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -t filter -D FORWARD -i %s -p tcp -m state --state INVALID -j DROP",
            BRIDGE_IFACE );
  ds_system_call(command, strlen(command));

  if (QcMapBackhaulMgr)
  {
    strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
    /* Configure VPN PassThrough. */
    /* Delete the existing rule, only if it was added .*/
    if( !QcMapNatAlg->nat_config.enable_l2tp_vpn_pass_through )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -p esp -i %s -j %s", devname, "DROP" );
      ds_system_call(command, strlen(command));
    }

    if( !QcMapNatAlg->nat_config.enable_ipsec_vpn_pass_through )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -p esp -i %s -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
    }

    if( !QcMapNatAlg->nat_config.enable_pptp_vpn_pass_through )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -p gre -i %s -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
    }
  }

  /*===========================================================================
  Allow access between Bridge and Bridge.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));


/*===========================================================================
  Allow access between Bridge and PPP.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, PPP_IFACE);
  ds_system_call(command, strlen(command));


  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));


/*===========================================================================
  Block WWAN access to MobileAP clients.
==========================================================================*/
  /* First delete duplicate rule if any. */
  if (QcMapBackhaulWLANMgr)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
            ECM_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
           BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  /* First delete rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

/*===========================================================================
  Block WWAN access to PPP client.
==========================================================================*/
  /* First delete duplicate rule if any. */
  if (QcMapBackhaulWLANMgr)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
            ECM_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
           BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  /* First delete rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s -j DROP", PPP_IFACE);
  ds_system_call(command, strlen(command));

  return true ;
}

/*===========================================================================
  FUNCTION cleanIPv4FilterTableInChain
==========================================================================*/
/*!
@brief
  Deleted all the ipv4 Input tabel rules added.

@parameters
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_ConnectionManager::CleanIPv4FilterTableInChain()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(devname, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (QcMapBackhaulMgr)
  {
    #ifndef FEATURE_QTIMAP_OFFTARGET
      strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
      /* Delete the existing rule, only if it was added .*/
      snprintf( command, MAX_COMMAND_STR_LEN,
                  "iptables -t filter -D INPUT -i %s -p tcp --dport 80 -j DROP", devname);
      ds_system_call(command, strlen(command));
      LOG_MSG_INFO1("%s\n", command,0,0);
      snprintf( command, MAX_COMMAND_STR_LEN,
                  "iptables -t filter -D INPUT -i %s -p tcp --dport 443 -j DROP", devname);
      ds_system_call(command, strlen(command));
      LOG_MSG_INFO1("%s\n", command,0,0);
    #endif
    return true ;
  }

  return false;
}

/*===========================================================================
  FUNCTION cleanIPv4NatTable
==========================================================================*/
/*!
@brief
  Deleted all the ipv4 nat table rules added.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_ConnectionManager::CleanIPv4NatTable()
{
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  int i=0;
  ds_dll_el_t * node;
  pugi::xml_document xml_file;
  pugi::xml_node parent, child;
  qcmap_cm_port_fwding_entry_conf_t* nat_entry;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(devname, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    return false;
  }

  qcmap_nat_entry_list_t* natList = &(QcMapNatAlg->nat_config.port_fwding_entries);

  memset(publicIpAddr, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = QcMapBackhaulMgr->ipv4_public_ip;
  strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
   /* Delete SNAT entries */
  switch (QcMapNatAlg->nat_config.nat_type)
  {
     case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE --random", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D PREROUTING -i %s -j NATTYPE --mode dnat --type 1", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j NATTYPE --mode forward_out --type 1", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
     case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE --random", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
     case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j SNAT --to-source %s",
                 devname, publicIpAddr);
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
     case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE --random", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D PREROUTING -i %s -j NATTYPE --mode dnat --type 2", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j NATTYPE --mode forward_out --type 2", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
  }

  if (natList->natEntryListHead)
  {
    node = ds_dll_next(natList->natEntryListHead, (&nat_entry));
    if (node == NULL)
    {
      LOG_MSG_ERROR("HeadNode is empty",0,0,0);
      return false;
    }

    for( i = 0; i < QcMapNatAlg->nat_config.num_port_fwding_entries; i++ )
    {
      if ( nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP )
      {
        if (!QcMapNatAlg->DeleteSNATEntryOnApps(nat_entry, &qmi_err_num))
          return false;
      }
      else
      {
        nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
        if (!QcMapNatAlg->DeleteSNATEntryOnApps(nat_entry, &qmi_err_num))
          return false;

        nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;

        if (!QcMapNatAlg->DeleteSNATEntryOnApps(nat_entry, &qmi_err_num))
          return false;

        node = ds_dll_next(node, (&nat_entry));
      }
    }
  }

  /* Config DMZ IP.*/
  if( QcMapNatAlg->nat_config.dmz_ip != 0 )
  {
    QcMapNatAlg->DeleteDMZOnApps(QcMapNatAlg->nat_config.dmz_ip, &qmi_err_num);
  }

  return true ;
}

/*===========================================================================
  FUNCTION CleanIPv6FilterTableFwdChain
==========================================================================*/
/*!
@brief
  Deleted all the ipv6 forward table  rules added.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_ConnectionManager::CleanIPv6FilterTableFwdChain()
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);

  bzero(command, MAX_COMMAND_STR_LEN);

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", BRIDGE_IFACE, PPP_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  if (QcMapBackhaulWLANMgr)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE, ECM_IFACE);
  ds_system_call(command, strlen(command));

  if (QcMapBackhaulWLANMgr)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE, ECM_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s -j DROP", PPP_IFACE);
  ds_system_call(command, strlen(command));

  return true ;
}
/*=====================================================
  FUNCTION UpdateDeviceEntryInfo
======================================================*/
/*!
@brief
  - Update the Connected Device Entry info as needed with:
    a)device type

@parameters
  Pointer to qcmap_nl_addr_t
  Device type

@return
  bool
/*=====================================================*/
bool QCMAP_ConnectionManager::UpdateDeviceEntryInfo
(
  qcmap_nl_addr_t*                 nl_addr,
  qcmap_msgr_device_type_enum_v01  device_entry_type
)
{
  QCMAP_ConnectionManager*      QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  ds_dll_el_t *                 node = NULL;
  qcmap_addr_info_list_t*       addrList = &(QcMapMgr->addrList);
  uint8                         mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_cm_client_data_info_t*  list_data = NULL;

  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);

  ds_mac_addr_ntop(nl_addr->mac_addr, mac_addr_char);
  ds_log_med("Update device entry info for mac addr %s", mac_addr_char);

  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Update device entry info:Linked list head is NULL",0,0,0);
    return false;
  }

  node = ds_dll_search (addrList->addrListHead , (void*)nl_addr->mac_addr,
                        QcMapMgr->qcmap_match_mac_address);
  if( node == NULL)
  {
    LOG_MSG_ERROR("Update device entry info:MAC address not found in list",0,0,0);
    return false;
  }
  else
  {
    list_data = (qcmap_cm_client_data_info_t*)(node->data);
    if (list_data == NULL)
    {
      LOG_MSG_ERROR("Update device entry info: Error in fetching node data ",0,0,0);
      return false;
    }

    ds_mac_addr_ntop(list_data->mac_addr, mac_addr_char);
    ds_log_med("Update device entry info: Found node with MAC addr %s \n",
               mac_addr_char);

    /*Update device type if needed*/
    if (list_data->device_type != device_entry_type )
    {
      LOG_MSG_INFO1("Previous device type %d, current device type %d",
                    list_data->device_type,
                    device_entry_type, 0);
      list_data->device_type = device_entry_type;
      LOG_MSG_INFO1("Reset IP address since client connected over new interface",
                    0, 0, 0);
      memset(&(list_data->ip_addr), 0, sizeof(list_data->ip_addr));
      memset(&(list_data->ip_v6_addr), 0, sizeof(list_data->ip_v6_addr));
      memset(&(list_data->link_local_v6_addr), 0, sizeof(list_data->link_local_v6_addr));

      /* Update device entry info with IP address as well */
      Getclientaddr(nl_addr);
      LOG_MSG_INFO1("UpdateDeviceEntryInfo:Updating IP address for the client",0, 0, 0);
      if(!(QCMAP_ConnectionManager::MatchMacAddrInList(nl_addr)))
      {
        LOG_MSG_ERROR("UpdateDeviceEntryInfo: No match found for the new client MAC",0, 0, 0);
      }
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION DelV6Conntrack
==========================================================================*/
/*!
@brief
  Deleted all the conntrack entries for the Ipv6 address

@parameters
  - Ipv6 address pointer

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::DelV6Conntrack
(
  uint8* ipv6addr,
  uint8* macaddr
)
{
  in6_addr tmpipv6, nullipv6;
  char command[MAX_COMMAND_STR_LEN];
  char mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  FILE *fp = NULL;
  char stringline[MAX_COMMAND_STR_LEN];
  int buffersize = MAX_COMMAND_STR_LEN;
  char *v6charAddr=NULL, *ptr;
  uint8 v6Addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01] ;
  struct ps_in6_addr *prefix_ptr;

  memset(command, 0, MAX_COMMAND_STR_LEN);
  memset(&tmpipv6, 0, sizeof(tmpipv6));
  memset(&nullipv6, 0, sizeof(nullipv6));
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(v6Addr, 0, sizeof(v6Addr));


  if ( NULL == ipv6addr )
  {
    LOG_MSG_ERROR("DelV6Conntrack(), client Ipv6 addr"
                  " not present", 0, 0, 0);
    return;
  }
  else
  {
    memcpy ( &tmpipv6.s6_addr, ipv6addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01 );
    if (memcmp( &tmpipv6 , &nullipv6, sizeof(tmpipv6)) == 0 )
    {
      LOG_MSG_INFO1("DelV6Conntrack(), client Ipv6 addr not present",
                    0, 0, 0);
      return;
    }
    else
    {
      ds_mac_addr_ntop( macaddr, mac_addr_char);
      snprintf(command, MAX_COMMAND_STR_LEN,"ip -6 neighbour show |grep %s > %s",
         mac_addr_char,IPV6_NEIGH_ADDR_TABLE);
      ds_system_call(command, strlen(command));

       /* open the file */
      fp = fopen(IPV6_NEIGH_ADDR_TABLE, "r");
      if (fp == NULL)
      {
        LOG_MSG_ERROR("Couldn't open IPV6_NEIGH_ADDR_TABLE file for reading %d", errno, 0, 0);
        return;
      }

      while(fgets( stringline, MAX_COMMAND_STR_LEN, fp) != NULL)
      {
        v6charAddr = strtok_r( stringline, " ", &ptr);

        if ( NULL != v6charAddr )
        {
          if(!inet_pton(AF_INET6, v6charAddr,&v6Addr))
          {
            LOG_MSG_INFO1("Not a valid IPV6 addr ",0,0,0);
          }
          else
          {
            prefix_ptr = (struct ps_in6_addr*)(v6Addr);
            if (!QCMAP_IN6_IS_PREFIX_LINKLOCAL(prefix_ptr->ps_s6_addr32))
            {
              snprintf(command,MAX_COMMAND_STR_LEN,"echo Deleting Client Conntrack IPv6 :%s > /dev/kmsg",v6charAddr);
              ds_system_call(command,strlen(command));

              snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --orig-src %s",v6charAddr);
              ds_system_call(command, strlen(command));

              snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --reply-src %s",v6charAddr);
              ds_system_call(command, strlen(command));
            }
          }
        }
      }

      fclose(fp);
      snprintf(command, MAX_COMMAND_STR_LEN,"rm %s",IPV6_NEIGH_ADDR_TABLE);
      ds_system_call(command, strlen(command));
    }
  }

  return;
}

void QCMAP_ConnectionManager::fetchHostName
(
  char *host,
  char *mac_str
)
{
  char data[MAX_COMMAND_STR_LEN];
  char command[MAX_COMMAND_STR_LEN];
  FILE *in=NULL;

  snprintf(data, MAX_COMMAND_STR_LEN,
           "cat %s | grep %s  | awk {'print $3'}", TEMP_HOST_FILE, mac_str);

  if(!(in = popen(data, "r")))
  {
    return;
  }

  //reading buffer return by popen
  if(fgets(data, sizeof(data), in)!=NULL && (data[0] != '\n') && (data[0] != '*'))
  {
    strlcpy( host,data,strlen(data)+1 );
  }
   pclose(in);
  snprintf(command,MAX_COMMAND_STR_LEN, "rm -f %s", TEMP_HOST_FILE);
  ds_system_call(command, strlen(command));

}

#ifdef FEATURE_DATA_TARGET_MDM9607
/*===========================================================================
  FUNCTION Dsd_client_init
==========================================================================*/
/*!
@brief
  Initialize DSD client

@parameters
  -None

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::Dsd_client_init()
{
  qmi_client_error_type qmi_error;
  qmi_cci_os_signal_type qmi_dsd_os_params;
  qmi_idl_service_object_type qmi_dsd_service_object;
  dsd_system_status_change_req_msg_v01 req;
  dsd_system_status_change_resp_msg_v01 resp;

  /* DSD Client Handle*/
  qmi_dsd_client_handle = NULL;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  qmi_dsd_service_object = dsd_get_service_object_v01();
  if(qmi_dsd_service_object == NULL)
  {
    LOG_MSG_ERROR("DSD service object is not available\n", 0, 0, 0);
    return;
  }
  LOG_MSG_INFO1("Got DSD service object\n", 0, 0, 0);

  LOG_MSG_INFO1("Start initing DSD qmi client with timeout: %d s\n", QMI_MAX_TIMEOUT_MS/1000, 0, 0);


  qmi_error = qmi_client_init_instance(qmi_dsd_service_object,
                                       QMI_CLIENT_INSTANCE_ANY,
                                       QCMAP_ConnectionManager::Dsd_client_cb,
                                       NULL,
                                       &qmi_dsd_os_params,
                                       QMI_MAX_TIMEOUT_MS,
                                       &qmi_dsd_client_handle);


  if(qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to init DSD qmi client %d\n", qmi_error, 0, 0);
    if(qmi_dsd_client_handle != NULL)
    {
      qmi_client_release(qmi_dsd_client_handle);
      qmi_dsd_client_handle = NULL;
    }
    return;
  }
  LOG_MSG_INFO1("DSD qmi client is inited with handle %d\n",
                qmi_dsd_client_handle, 0, 0);

  req.limit_so_mask_change_ind_valid=1;
  req.limit_so_mask_change_ind = 1;

  qmi_error = qmi_client_send_msg_sync(qmi_dsd_client_handle,
                                       QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                                       (void *)&req,
                                       sizeof(req),
                                       (void*)&resp,
                                       sizeof(resp),
                                       QMI_MAX_TIMEOUT_MS);

  if (qmi_error != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Failed to register indication with qmi_error=%d result=%d\n",
                  qmi_error, resp.resp.result, 0);
    qmi_client_release(qmi_dsd_client_handle);
    qmi_dsd_client_handle = NULL;
  }
  return;
}

/*===========================================================================
  FUNCTION Dsd_client_cb
==========================================================================*/
/*!
@brief
  Registered call back function to handle QMI_DSD_SYSTEM_STATUS_IND_V01

@parameters
  -Wds client handle
-QMI user handle
-Indicator message ID
-Raw indication data
-Raw data length
-User call back handle

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::Dsd_client_cb
(
  qmi_client_type user_handle,
  unsigned int    msg_id,
  void           *ind_buf,
  unsigned int    ind_buf_len,
  void           *ind_cb_data
)
{
  qcmap_cm_dsd_status_info_t data = {0};
  qmi_client_error_type qmi_error;
  void* dest;
  int dest_len = 0;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qmi_error_type_v01          qmi_err_num;

  if(ind_buf == NULL)
  {
    LOG_MSG_ERROR("Failed to receive any data in qmi message\n", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO1("Dsd_client_cb: user_handle %X msg_id %d ind_buf_len %d\n",
                 user_handle, msg_id, ind_buf_len);

  switch(msg_id)
  {
    case QMI_DSD_SYSTEM_STATUS_IND_V01:
      LOG_MSG_INFO1("Receive QMI_DSD_SYSTEM_STATUS_IND_V01\n",
                    0, 0, 0);
      dest = (void*)&data;
      dest_len = sizeof(data);
      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            dest,
                                            dest_len);
      if(qmi_error != QMI_NO_ERR)
      {
        LOG_MSG_ERROR("Failed to decode qmi message\n", 0, 0, 0);
        return;
      }

      LOG_MSG_ERROR("data.avail_sys_valid %d, data.avail_sys_len %d\n",
                    data.avail_sys_valid, data.avail_sys[0].rat_value, 0);

      LOG_MSG_ERROR("data.avail_sys[0].technology %d, data.avail_sys[0].rat_value %d\n",
                    data.avail_sys[0].technology, data.avail_sys[0].rat_value, 0);

      if (data.avail_sys_valid && data.avail_sys_len > 0 &&
          data.avail_sys[0].technology == DSD_SYS_NETWORK_3GPP_V01 &&
          data.avail_sys[0].rat_value == DSD_SYS_RAT_EX_3GPP_LTE_V01)
      {
        if ((QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)) &&
            ((qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED)||
            (qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED)))
        {
          QcMapMgr->SetUSBClockFreq(true);
        }
        else
        {
          QcMapMgr->SetUSBClockFreq(false);
        }
      }
      break;

    default:
      LOG_MSG_ERROR("Receive unexpected msg_id: %d\n", msg_id, 0, 0);
      break;
  }
  return;
}

/*===========================================================================
  FUNCTION SetUSBClockFreq
==========================================================================*/
/*!
@brief
   Set the New Clock frequency

@parameters
  - Bool to indicate to set to NOM freq

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::SetUSBClockFreq( boolean setNOM )
{

  char command[MAX_COMMAND_STR_LEN];

  if ( setNOM )
  {
    LOG_MSG_INFO1("Setting Clock Speed to NOMINAL\n", 0, 0, 0);

    snprintf(command, MAX_COMMAND_STR_LEN,
           "echo enable > /sys/class/udc/msm_hsusb/device/perf_mode");
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_INFO1("Setting Clock Speed to SVS\n", 0, 0, 0);

    snprintf(command, MAX_COMMAND_STR_LEN,
           "echo disable > /sys/class/udc/msm_hsusb/device/perf_mode");
    ds_system_call(command, strlen(command));
  }
  return;
}

/*=====================================================
  FUNCTION CheckRatforLTE
======================================================*/
/*!
@brief
  Check whether the current RAT is LTE

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean QCMAP_ConnectionManager::CheckRatforLTE()
{
  dsd_get_system_status_req_msg_v01 get_system_status_req_msg;
  dsd_get_system_status_resp_msg_v01 get_system_status_resp_msg;
  qmi_client_error_type qmi_error;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  int retVal = 0;
  qmi_error_type_v01 qmi_err_num;

  LOG_MSG_INFO1("CheckRatforLTE\n",0,0,0);

  qmi_error = qmi_client_send_msg_sync(QcMapMgr->qmi_dsd_client_handle,
                                       QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                       &get_system_status_req_msg,
                                       sizeof(dsd_get_system_status_req_msg_v01),
                                       &get_system_status_resp_msg,
                                       sizeof(dsd_get_system_status_resp_msg_v01),
                                       QMI_MAX_TIMEOUT_MS);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_system_status_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get current RAT %d : %d",
        qmi_error, get_system_status_resp_msg.resp.error,0);
    qmi_err_num = get_system_status_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_ERROR("avail_sys_valid %d, avail_sys_len %d\n",
                    get_system_status_resp_msg.avail_sys_valid, get_system_status_resp_msg.avail_sys[0].rat_value, 0);

  LOG_MSG_ERROR("avail_sys[0].technology %d, avail_sys[0].rat_value %d\n",
                    get_system_status_resp_msg.avail_sys[0].technology, get_system_status_resp_msg.avail_sys[0].rat_value, 0);

  if (get_system_status_resp_msg.avail_sys_valid &&
      get_system_status_resp_msg.avail_sys_len > 0 &&
      get_system_status_resp_msg.avail_sys[0].technology == DSD_SYS_NETWORK_3GPP_V01 &&
      get_system_status_resp_msg.avail_sys[0].rat_value == DSD_SYS_RAT_EX_3GPP_LTE_V01)
  {
    retVal = 1;
  }

  if (retVal == 1)
    return true;
  else
    return false;
}
#endif
