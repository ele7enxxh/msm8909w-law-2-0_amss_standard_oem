/*====================================================

FILE:  QCMAP_Firewall.cpp

SERVICES:
   QCMAP Connection Manager Firewall Specific Implementation

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   vm         Created

===========================================================================*/
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
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "ds_qmi_qcmap_msgr.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"

#include "QCMAP_Firewall.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Backhaul_Ethernet.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_LAN.h"

bool QCMAP_Firewall::flag= false;
QCMAP_Firewall* QCMAP_Firewall::object=NULL;

QCMAP_Firewall* QCMAP_Firewall::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : FIREWALL",0, 0, 0);
    object = new QCMAP_Firewall();
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
  Initializes Firewall variables.

@parameters
None

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

QCMAP_Firewall::QCMAP_Firewall()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  memset(&this->firewall_config,0,sizeof(this->firewall_config));
  this->firewall_config.firewall_enabled = false;
  this->firewall_config.firewall_pkts_allowed = false;
  this->firewall_config.upnp_inbound_pinhole = false;
  this->firewall_config.num_firewall_entries=0;
  /* Read the configuration. */
  this->firewall_config.extd_firewall_entries.firewallEntryListHead = NULL;
  this->firewall_config.extd_firewall_entries.firewallEntryListTail = NULL;
  if (ReadFirewallXML())
  {
    LOG_MSG_INFO1("Reading QCMAP Firewall Read XML succeeded.\n",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Reading QCMAP Firewall Read XML failed.\n",0,0,0);
  }

  //initialize random number seed
  srand((unsigned) time(NULL));

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
Destroys the class object

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
/*=========================================================*/

QCMAP_Firewall::~QCMAP_Firewall()
{
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  qcmap_firewall_entry_list_t* firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);
  LOG_MSG_INFO1("Destroying Object: FIREWALL",0,0,0);
  /* Free memory allocated for firewall rules */
  if(firewallList->firewallEntryListHead && firewallList->firewallEntryListHead->next)
  {
    ds_dll_delete_all(firewallList->firewallEntryListHead);
    firewallList->firewallEntryListHead = NULL;
    firewallList->firewallEntryListTail = NULL;
  }
  flag=false;
  object=NULL;
}

/*===========================================================================
  FUNCTION addFirewallEntryToXML
==========================================================================*/
/*!
@brief
  Adds firewall entry to XML

@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf
@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

int QCMAP_Firewall::addFirewallEntryToXML
(
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf
)
{

  struct in_addr addr;
  struct in6_addr ip6_addr;
  char ip6_addr_input[INET6_ADDRSTRLEN];
  int next_hdr_prot;
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  pugi::xml_node parent, root, child, subchild;
  QCMAP_CM_LOG_FUNC_ENTRY();

  root = xml_file->child(System_TAG).child(MobileAPFirewallCfg_TAG);

  // add new Firewall entry here.
  parent = root.append_child(Firewall_TAG);

  //add handle
  child = parent.append_child(FirewallHandle_TAG);
  snprintf(data, sizeof(data), "%d", extd_firewall_conf->firewall_handle);
  child.append_child(pugi::node_pcdata).set_value(data);
  LOG_MSG_INFO1("Add handle %d",extd_firewall_conf->firewall_handle,0,0);

  //Add Pinhole_Entry
  child = parent.append_child(PinholeEntry_TAG);
  snprintf(data, MAX_COMMAND_STR_LEN, "%d", extd_firewall_conf->upnp_pinhole_handle);
  child.append_child(pugi::node_pcdata).set_value(data);

  //add IP version
  child = parent.append_child(IPFamily_TAG);
  snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_vsn);
  child.append_child(pugi::node_pcdata).set_value(data);

  LOG_MSG_INFO1("IP Version: %d", extd_firewall_conf->filter_spec.ip_vsn, 0, 0);
  if(extd_firewall_conf->filter_spec.ip_vsn == IP_V4)
  {
    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
    {
      child = parent.append_child(IPV4SourceAddress_TAG);
      subchild = child.append_child(IPV4SourceIPAddress_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.src.addr.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

      subchild = child.append_child(IPV4SourceSubnetMask_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

      debug_string= extd_firewall_conf->filter_spec.ip_hdr.v4.src.addr.ps_s_addr;
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
      LOG_MSG_INFO1("\nv4 Source Socket address:: '%s'",str,0,0);


      debug_string=extd_firewall_conf->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr;
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
      LOG_MSG_INFO1("\nv4 Source Mask address: '%s'",str,0,0);
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
    {
      child = parent.append_child(IPV4DestinationAddress_TAG);
      subchild = child.append_child(IPV4DestinationIPAddress_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

      subchild = child.append_child(IPV4DestinationSubnetMask_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
    {
      child = parent.append_child(IPV4TypeOfService_TAG);

      subchild = child.append_child(TOSValue_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v4.tos.val);
      subchild.append_child(pugi::node_pcdata).set_value(data);

      subchild = child.append_child(TOSMask_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v4.tos.mask);
      subchild.append_child(pugi::node_pcdata).set_value(data);

      LOG_MSG_INFO1("Tos: %d mask: %d ", extd_firewall_conf->filter_spec.ip_hdr.v4.tos.val,
                    extd_firewall_conf->filter_spec.ip_hdr.v4.tos.mask, 0);
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
    {
      child = parent.append_child(IPV4NextHeaderProtocol_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot);
      child.append_child(pugi::node_pcdata).set_value(data);
      LOG_MSG_INFO1("Next Header: %d ",extd_firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot, 0, 0);
      next_hdr_prot = extd_firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot;
    }

  }
  if (extd_firewall_conf->filter_spec.ip_vsn == IP_V6)
  {
    if(extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
    {
      child = parent.append_child(IPV6SourceAddress_TAG);
      memcpy(ip6_addr.s6_addr,
             extd_firewall_conf->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      bzero(&ip6_addr_input,INET6_ADDRSTRLEN);
      if(inet_ntop(AF_INET6,&ip6_addr,ip6_addr_input,INET6_ADDRSTRLEN) != NULL)
      {
        subchild = child.append_child(IPV6SourceIPAddress_TAG);
        subchild.append_child(pugi::node_pcdata).set_value(ip6_addr_input);
      }

      subchild = child.append_child(IPV6SourcePrefix_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.src.prefix_len);
      subchild.append_child(pugi::node_pcdata).set_value(data);

      printf("\n In:%s @: %d Source IPv6: %s  Mask: %s \n", __FUNCTION__, __LINE__,
             child.child("IPV6SourceIPAddress").child_value(),
             child.child("IPV6SourcePrefix").child_value());
    }
    if(extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
    {
      child = parent.append_child(IPV6DestinationAddress_TAG);
      memcpy(ip6_addr.s6_addr,
             extd_firewall_conf->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      bzero(&ip6_addr_input,INET6_ADDRSTRLEN);
      if(inet_ntop(AF_INET6,&ip6_addr,ip6_addr_input,INET6_ADDRSTRLEN) != NULL)
      {
        subchild = child.append_child(IPV6DestinationIPAddress_TAG);
        subchild.append_child(pugi::node_pcdata).set_value(ip6_addr_input);
      }

      subchild = child.append_child(IPV6DestinationPrefix_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.dst.prefix_len);
      subchild.append_child(pugi::node_pcdata).set_value(data);
     printf("\n In:%s @: %d Dest IPv6: %s  Mask: %s \n", __FUNCTION__, __LINE__,
             child.child("IPV6DestinationIPAddress").child_value(),
             child.child("IPV6DestinationPrefix").child_value());
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
    {
      child = parent.append_child(IPV6TrafficClass_TAG);

      subchild = child.append_child(TrfClsValue_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val);
      subchild.append_child(pugi::node_pcdata).set_value(data);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.trf_cls.mask);
      subchild = child.append_child(TrfClsMask_TAG);
      subchild.append_child(pugi::node_pcdata).set_value(data);
    }

    if( extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT )
    {
      child = parent.append_child(IPV6NextHeaderProtocol_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot);
      child.append_child(pugi::node_pcdata).set_value(data);
      next_hdr_prot = extd_firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot;
    }
  }

  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
      if (extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        child = parent.append_child(TCPSource_TAG);

        subchild = child.append_child(TCPSourcePort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCPSourceRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        LOG_MSG_INFO1( "Source port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,
                       0 );

      }
      if(extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        child = parent.append_child(TCPDestination_TAG);

        subchild = child.append_child(TCPDestinationPort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCPDestinationRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "Dest port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range,
                       0 );
      }
    break;

    case PS_IPPROTO_UDP:
      if (extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        child = parent.append_child(UDPSource_TAG);

        subchild = child.append_child(UDPSourcePort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(UDPSourceRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        LOG_MSG_INFO1( "UDP port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.range,
                       0 );
      }

      if(extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        child = parent.append_child(UDPDestination_TAG);

        subchild = child.append_child(UDPDestinationPort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(UDPDestinationRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        LOG_MSG_INFO1( "UDP Dst port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.range,
                       0 );
      }
    break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
      if ( extd_firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask
           & IPFLTR_MASK_ICMP_MSG_TYPE)
      {
        child = parent.append_child(ICMPType_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.icmp.type);
        child.append_child(pugi::node_pcdata).set_value(data);

      }

      if (extd_firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE)
      {
        child = parent.append_child(ICMPCode_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.icmp.code);
        child.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "ICMP type : %d  ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.icmp.code,
                       0,0 );
      }
    break;

    case PS_IPPROTO_ESP:
      if (extd_firewall_conf->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI)
      {
        child = parent.append_child(ESPSPI_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.esp.spi);
        child.append_child(pugi::node_pcdata).set_value(data);
      }
    break;

    case PS_IPPROTO_TCP_UDP:
      if(extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
         & IPFLTR_MASK_TCP_UDP_SRC_PORT)
      {
        child = parent.append_child(TCP_UDPSource_TAG);

        subchild = child.append_child(TCP_UDPSourcePort_TAG);
        snprintf( data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port);

        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCP_UDPSourceRange_TAG);
        snprintf( data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range);

        subchild.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "TCP UDP Src port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,
                       0 );
      }

      if( extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
          & IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        child = parent.append_child(TCP_UDPDestination_TAG);

        subchild = child.append_child(TCP_UDPDestinationPort_TAG);
        snprintf(data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCP_UDPDestinationRange_TAG);
        snprintf(data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "TCP UDP Src port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range,
                       0 );
       }
    break;

    default:
      break;
  }

  QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, xml_file);
  return 0;
}

/*===========================================================================
  FUNCTION addFirewallEntryToList
==========================================================================*/
/*!
@brief
  Adds firewall entry to List

@parameters
  qcmap_msgr_firewall_entry_conf_t* extd_firewall_conf,
  qcmap_msgr_firewall_entry_conf_t *new_firewall_entry

@return
   0 - on failure
   1 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Firewall::addFirewallEntryToList
(
  qcmap_msgr_firewall_entry_conf_t* extd_firewall_conf,
  qcmap_msgr_firewall_entry_conf_t *new_firewall_entry
)
{

  int next_hdr_prot;
  ds_dll_el_t * node = NULL;

  qcmap_firewall_entry_list_t* firewallList = &(this->firewall_config.extd_firewall_entries);

  /*****************************************************/

  /* save into the config */
  if (firewallList->firewallEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
           information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("addFirewallEntryToList - Error in allocating memory for node",
                     0,0,0);
      return false;
    }
    firewallList->firewallEntryListHead = node;
  }

  new_firewall_entry = (qcmap_msgr_firewall_entry_conf_t*)
                            ds_malloc(sizeof(qcmap_msgr_firewall_entry_conf_t));
  if( new_firewall_entry == NULL )
  {
    LOG_MSG_ERROR("addFirewallEntryToList - Error in allocating memory for"
                  "new Firewall entry",0,0,0);
    return false;
  }

  memset(new_firewall_entry, 0, sizeof(qcmap_msgr_firewall_entry_conf_t));

  memcpy(&(new_firewall_entry->filter_spec),
         &(extd_firewall_conf->filter_spec),
         sizeof(ip_filter_type));
  new_firewall_entry->firewall_handle = extd_firewall_conf->firewall_handle;

  //Store the firewall entry in the linked list
  if ((node = ds_dll_enq(firewallList->firewallEntryListHead,
                         NULL, (void*)new_firewall_entry )) == NULL)
  {
    LOG_MSG_ERROR("AddFireWallEntry - Error in adding a node",0,0,0);
    ds_free(new_firewall_entry);
    return false;
  }
  firewallList->firewallEntryListTail = node;

  LOG_MSG_INFO1("Added Firewall entry added to list",0,0,0);

  this->firewall_config.num_firewall_entries++;
  return true;
}

/*===========================================================================
  FUNCTION DeleteConntrackEntryForDropFirewallEntries()
==========================================================================*/
/*!
@brief
  Delete the client conntrack when we are adding a DROP firewall entry for
  that IPv4 address and port combination

@return
  bool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

bool QCMAP_Firewall::DeleteConntrackEntryForDropIPv4FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  char *protocol=NULL,*saddr=NULL, *daddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  char command[MAX_COMMAND_STR_LEN];
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char tmp_2[MAX_COMMAND_STR_LEN]={0};
  char regular_exp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  char *line = NULL;
  size_t len = 0;
  int read;
  bool check_ip = false;
  bool check_sport = false;
  bool check_dport = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port;
  uint32 subnet_mask, ip_addr;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  if (firewall_entry == NULL)
  {
    LOG_MSG_ERROR("NULL Arguments passed",0,0,0);
    return false;
  }

  if (protocol_num != PS_IPPROTO_TCP && protocol_num != PS_IPPROTO_UDP)
  {
    if ( protocol_num == PS_IPPROTO_TCP_UDP)
    {
      QcMapFirewall->DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry,PS_IPPROTO_TCP);
      QcMapFirewall->DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry,PS_IPPROTO_UDP);
    }
    return true;
  }

  /*The logic is to grep for the conntrack entries using firewall params
  so as to reduce the number of conntrack entries to be checked to a
  minimum value. We write these entries to a txt file and then read the
  text file, check if the entry needs to be deleted and then delete the entry*/

  /*We only need certain columns of conntrack entries for deletion,
  so we cut those particular columns*/

  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,"conntrack -L | cut -f1,9,11,12 -d ' ' ");
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,"conntrack -L | cut -f1,10,12,13 -d ' ' ");

  if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
  {
    snprintf(tmp, MAX_COMMAND_STR_LEN,"| grep src=");
    strlcat(command,tmp,MAX_COMMAND_STR_LEN);
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    subnet_mask = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);

    ip_addr = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);

    if (subnet_mask < 0xff000000)
      check_ip = true;

    /*We need to grep according to the subnet mask. If subnet mask
    is 255.0.0.0 / 255.255.0.0 / 255.255.255.0 / 255.255.255.255 ,
    then we can grep straight-away for the match. Otherwise, we grep for
    the minimum pattern that matches perfectly and check later whether
    we need to delete that conntrack entry or not*/

    else if ((subnet_mask >= 0xff000000) && (subnet_mask < 0xffff0000))
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN,"%d. ",(ip_addr & 0xff000000) >> 24);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xff000000)
        check_ip = true;
    }

    else if ((subnet_mask >= 0xffff0000) && (subnet_mask < 0xffffff00))
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN,"%d.%d. ",
               (ip_addr & 0xff000000)>>24,
               (ip_addr & 0x00ff0000) >> 16);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xffff0000)
        check_ip = true;
    }

    else if ((subnet_mask >= 0xffffff00) && (subnet_mask < 0xffffffff))
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN,"%d.%d.%d. ",
               (ip_addr & 0xff000000)>>24,
               (ip_addr & 0x00ff0000) >> 16,
               (ip_addr & 0x0000ff00) >> 8);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xffffff00)
        check_ip = true;
    }
    else if (subnet_mask == 0xffffffff)
    {
      readable_addr(AF_INET,&(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr),tmp);
      strlcat(tmp," ",MAX_COMMAND_STR_LEN);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }

  }

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      /*If sport / dport are defined, we use them directly, or else we set a flag
      to check the sport and dport later and delete conntrack entry only
      if matches the criterion*/

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
          check_sport = true;
        }
        else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
          check_sport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;
  }

  snprintf(tmp, MAX_COMMAND_STR_LEN,"> %s",CONNTRACK_ENTRIES);
  strlcat(command,tmp,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("%s",command,0,0);
  ds_system_call(command, strlen(command));
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  memset(command,0,MAX_COMMAND_STR_LEN);

  /*Open the conntrack.txt file to fetch protocol,saddr,dport*/
  fd = fopen(CONNTRACK_ENTRIES,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("DeleteConntrackEntries - Error in opening %s",
                  CONNTRACK_ENTRIES,0,0);
    return false;
  }

  /*Reset the file ptr to start of file*/
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    fclose (fd);
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    return false;
  }
  /*Read the file line-by-line and delete the entry if required*/
  while (((read = getline(&line, &len, fd)) != -1))
  {
    protocol = strtok_r(line, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);
    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);
    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);
    if(dport)
      *(dport + 5) = ' ';

    /*Check for subnet mask*/
    if (check_ip)
    {
      if(saddr)
        inet_pton(AF_INET, saddr+4, (void *)&ip_addr);
      if ((ip_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
          (firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr))
        continue;
    }

    /*Check for source port range*/
    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
        continue;
    }

    /*Check for destination port range*/
    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        continue;
    }

    memset(tmp,0,MAX_COMMAND_STR_LEN);

    /*Delete contrack entry*/
    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);

  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",CONNTRACK_ENTRIES);
  ds_system_call(command, strlen(command));

  return true;
}


/*======================================================
  FUNCTION:  DeleteConntrackEntryForAcceptFirewallEntries
  =====================================================*/
  /*!
      @brief
      Delete the client conntrack when we are adding a ACCEPT firewall entry for
      that IPv4 address and port combination

      @params
         IPv6 address
         Protocol
         UDP Port start number
         UDP Port Range
         TCP Port start number
         TCP Port Range

      @return
      void
  */
/*====================================================*/
bool QCMAP_Firewall::DeleteConntrackEntryForAcceptIPv4FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  uint8  ret = 0;
  int read;
  bool check_sport = false;
  bool check_dport = false;
  bool check_ip = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port;
  char *line = NULL;
  size_t len = 0;
  FILE *fd = NULL;
  uint32 subnet_mask, ip_addr;
  char *protocol=NULL,*saddr=NULL, *daddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  int j = 0;
  QCMAP_LAN* QCMAPLANMgr = QCMAP_LAN::Get_Instance(false);
  struct in_addr addr;
  char apps_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  LOG_MSG_INFO1("DeleteConntrackEntryForAcceptIPv4FirewallEntries \n",0,0,0);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    return;
  }

  if( QCMAPLANMgr == NULL )
  {
    LOG_MSG_ERROR("LAN object is NULL\n",0,0,0);
    return;
  }

  addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.apps_ip_addr);
  strlcpy(apps_ip, inet_ntoa(addr), INET_ADDRSTRLEN);

  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L | cut -f1,9,11,12 -d ' ' | grep -v src=%s ", apps_ip);
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,
            "conntrack -L | cut -f1,10,12,13 -d ' ' | grep -v src=%s ", apps_ip);

  if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
  {
    subnet_mask = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
    ip_addr = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
    check_ip = true;
  }

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        check_dport = true;
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        check_dport = true;
      }

      break;
  }

  if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
  {
    subnet_mask = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
    check_ip = true;
  }

  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", CONNTRACK_ENTRIES);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("%s",command,0,0);
  ds_system_call(command, strlen(command));

  /*Open the conntrack.txt file to fetch protocol,saddr,dport*/
  fd = fopen(CONNTRACK_ENTRIES,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("DeleteConntrackEntries - Error in opening %s",
                  CONNTRACK_ENTRIES,0,0);
    return false;
  }

  /*Reset the file ptr to start of file*/
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    return false;
  }

  /*Read the file line-by-line and delete the entry if required*/
  while (((read = getline(&line, &len, fd)) != -1))
  {
    protocol = strtok_r(line, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);

    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);

    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);

    if(dport)
      *(dport + 5) = ' ';

    /*Check for subnet mask*/
    if (check_ip)
    {
      if(saddr)
        inet_pton(AF_INET, saddr+4, (void *)&ip_addr);
      if ((ip_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
          (firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr))
        goto delete_entry;
    }

    /*Check for source port range*/
    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
        goto delete_entry;
    }

    /*Check for destination port range*/
    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        goto delete_entry;
    }

    /*check protocol*/
    if (firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP ||
        firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot == protocol_num ||
        firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot == 0)
      continue;

    /*Delete contrack entry*/
    delete_entry:
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);

  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",CONNTRACK_ENTRIES);
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION SetFirewallV4
==========================================================================*/
/*!
@brief
  Adds firewall entry by preparing, the command to add firewall entry based on the
  firewall configuration value and executes the same.
  It also generates delete ommand for the same.

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  uint8_t *del_command

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::SetFirewallV4
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  char command[MAX_COMMAND_STR_LEN]={0}, devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2]={0};
  char command_sec[MAX_COMMAND_STR_LEN];
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char str[INET6_ADDRSTRLEN];
  uint32_t addr;
  uint8 next_hdr_prot;

  if(!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul is not active, no reason to add firewall entry yet",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01 ;
    return QCMAP_CM_ERROR;
  }

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  if (firewall_entry->filter_spec.ip_vsn != IP_V4) {
      LOG_MSG_ERROR( "Incorrect firewall_entry version. IP Version = %x\n",
                     firewall_entry->filter_spec.ip_vsn,0,0 );
     *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
      return QCMAP_CM_ERROR;
  }

  if(qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED &&
     !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
     !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
     !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
  {
    strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
  }
  else if ( !QCMAP_ConnectionManager::GetDeviceName( devname,
       QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num ) )
  {
    return -1;
  }

   /*In comming traffic from wwan is handled in PREROUTING chain in mangle table */
   if ( add_rule )
   {
     snprintf(command,MAX_COMMAND_STR_LEN,"iptables -t mangle -I PREROUTING -i %s ",devname);
   }
   else
   {
     snprintf(command,MAX_COMMAND_STR_LEN,"iptables -t mangle -D PREROUTING -i %s ",devname);
   }

   if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
   {
     readable_addr(AF_INET,&(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr),(char *)&str);
     strlcat(command," -s ",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
     readable_addr(AF_INET,&(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr),(char *)&str);
     strlcat(command,"/",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
   }

   if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
   {
     snprintf( tmp,MAX_COMMAND_STR_LEN," -m tos --tos %x/%x ",
               firewall_entry->filter_spec.ip_hdr.v4.tos.val,
               firewall_entry->filter_spec.ip_hdr.v4.tos.mask );

     strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     memset(tmp,0,MAX_COMMAND_STR_LEN);
   }
   next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;

  switch( next_hdr_prot )
  {
    case PS_IPPROTO_TCP:
      strlcat(command," -p tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
                  ( firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.src.range));
        }
        else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

     if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
     {
       if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
       {
         snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                  firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
                 (firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port + \
                  firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range));
       }else
       {
         snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                  firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
       }
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     break;
   case PS_IPPROTO_UDP:

     strlcat(command," -p udp ",MAX_COMMAND_STR_LEN);

     if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
          IPFLTR_MASK_UDP_SRC_PORT )
     {
       if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
       {
         snprintf( tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port,
                  ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range+\
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port ) );
       }else
       {
         snprintf( tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port );
       }
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
     {
       if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
       {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,
                  (firewall_entry->filter_spec.next_prot_hdr.udp.dst.range + \
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port) );
       }else
       {
         snprintf( tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port );
       }
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     break;
   case PS_IPPROTO_TCP_UDP:

     strlcpy(command_sec, command, MAX_COMMAND_STR_LEN);
     strlcat(command, " -p tcp ", MAX_COMMAND_STR_LEN);
     strlcat(command_sec, " -p udp ", MAX_COMMAND_STR_LEN);

     if( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
         IPFLTR_MASK_TCP_UDP_SRC_PORT )
     {
       snprintf(tmp, MAX_COMMAND_STR_LEN, " --sport %d:%d ",
           firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
           firewall_entry->filter_spec.next_prot_hdr.tcp.src.range + \
           firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
       strlcat(command, tmp, MAX_COMMAND_STR_LEN);
       strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
          IPFLTR_MASK_TCP_UDP_DST_PORT )
     {
       snprintf(tmp, MAX_COMMAND_STR_LEN, " --dport %d:%d ",
           firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
           firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range + \
           firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
       strlcat(command, tmp, MAX_COMMAND_STR_LEN);
       strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }
     break;

   case PS_IPPROTO_ICMP:

     strlcat(command," -p icmp ",MAX_COMMAND_STR_LEN);

     if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask &
          IPFLTR_MASK_ICMP_MSG_TYPE )
     {
       snprintf(tmp,MAX_COMMAND_STR_LEN," --icmp-type %d",
                firewall_entry->filter_spec.next_prot_hdr.icmp.type);
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask &
          IPFLTR_MASK_ICMP_MSG_CODE )
     {
       snprintf(tmp,MAX_COMMAND_STR_LEN, "/%d ",
                firewall_entry->filter_spec.next_prot_hdr.icmp.code);
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }
     break;

   case PS_IPPROTO_ESP:

     strlcat(command," -p esp ",MAX_COMMAND_STR_LEN);
     if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
     {
       snprintf(tmp,MAX_COMMAND_STR_LEN," --espspi %d ",
                firewall_entry->filter_spec.next_prot_hdr.esp.spi);
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     }
     break;
   default:
     LOG_MSG_ERROR("Unsupported protocol \n",next_hdr_prot,0,0);
     break;
  }

   LOG_MSG_INFO1("SET FIREWALL pkts allowed = %d",
                 QcMapFirewall->firewall_config.firewall_pkts_allowed,0,0);

  if( QcMapFirewall->firewall_config.firewall_pkts_allowed )
    snprintf(tmp,MAX_COMMAND_STR_LEN," -j %s","ACCEPT");
  else
    snprintf(tmp,MAX_COMMAND_STR_LEN," -j %s","DROP");

  strlcat(command,tmp,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("\n %s \n",command,0,0);
  ds_system_call(command, strlen(command));

  if(next_hdr_prot == PS_IPPROTO_TCP_UDP)
  {
    strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
    ds_system_call(command_sec, strlen(command_sec));
    LOG_MSG_INFO1("\n %s \n",command,0,0);
  }


  /* The below API's are need to clear the conntrack entries which would in turn delete
    the SFE entries.
    firewall_pkt_allowed =  false we need to delete the conntrack entries for the firewall IP, Port
    and Protocol combination which is entered
    firewall_pkt_allowed =  true we need to delete all other conntrack entries except for the
    firewall IP, Port and Protocol combination which is entered
    This would be applicable for Only TCP, UDP and if no protocol is mentioned (if firewall is added
    only based on sport and dport)
   */
  if ( next_hdr_prot == PS_IPPROTO_UDP || next_hdr_prot == PS_IPPROTO_TCP ||
       next_hdr_prot == PS_IPPROTO_TCP_UDP || next_hdr_prot == PS_IPPROTO_NO_PROTO )
  {
    if (add_rule)
    {
      if (QcMapFirewall->firewall_config.firewall_pkts_allowed)
      {
        QcMapFirewall->DeleteConntrackEntryForAcceptIPv4FirewallEntries(firewall_entry,
                                                                        PS_IPPROTO_TCP);
        QcMapFirewall->DeleteConntrackEntryForAcceptIPv4FirewallEntries(firewall_entry,
                                                                        PS_IPPROTO_UDP);
      }
      else
      {
        if (next_hdr_prot == PS_IPPROTO_NO_PROTO)
          QcMapFirewall->DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry,
                                                                        PS_IPPROTO_TCP_UDP);
        else
          QcMapFirewall->DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry,
                                                                        next_hdr_prot);
      }
    }
  }
  return QCMAP_CM_SUCCESS;
}


/*===========================================================================
  FUNCTION SetFirewallV6
==========================================================================*/
/*!
@brief
  Adds firewall entry by preparing, the command to add firewall entry based on the
  firewall configuration value and executes the same.
  It also generates delete ommand for the same.

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  uint8_t *del_command

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::SetFirewallV6
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  char command[MAX_COMMAND_STR_LEN]={0}, devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2]={0};
  char command_sec[MAX_COMMAND_STR_LEN];
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char str[INET6_ADDRSTRLEN];
  uint32_t addr;
  uint8 next_hdr_prot;

  if(!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul is not active, no reason to add firewall entry yet",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01 ;
    return QCMAP_CM_ERROR;
  }

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  if (firewall_entry->filter_spec.ip_vsn != IP_V6) {
      LOG_MSG_ERROR( "Incorrect firewall_entry version. IP Version = %x\n",
                     firewall_entry->filter_spec.ip_vsn,0,0 );
     *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
      return QCMAP_CM_ERROR;
  }

  if(qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED &&
     !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
     !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
     !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
  {
    strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv6_interface,sizeof(devname));
  }
  else if ( !QCMAP_ConnectionManager::GetDeviceName( devname,
       QCMAP_MSGR_IP_FAMILY_V6_V01, qmi_err_num) )
  {
    return -1;
  }

   /*In comming traffic from wwan is handled in PREROUTING chain in mangle table */
   if ( add_rule )
   {
      snprintf(command,MAX_COMMAND_STR_LEN,"ip6tables -t mangle -I PREROUTING -i %s ",devname);
   }
   else
   {
      snprintf(command,MAX_COMMAND_STR_LEN,"ip6tables -t mangle -D PREROUTING -i %s ",devname);
   }


   if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
   {
     readable_addr(AF_INET6,(uint32 *)\
     &firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr32,(char *)&str);

     strlcat(command," -s ",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
     snprintf( tmp,MAX_COMMAND_STR_LEN,"/%d ",
               firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len);
     strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     memset(tmp,0,MAX_COMMAND_STR_LEN);
   }

   if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
   {
     readable_addr(AF_INET6,(uint32 *)\
     &firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr32,(char *)&str);
     strlcat(command," -d ",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
     snprintf( tmp,MAX_COMMAND_STR_LEN,"/%d ",
               firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len);
     strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     memset(tmp,0,MAX_COMMAND_STR_LEN);
   }

   if(firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
   {
     snprintf(tmp, MAX_COMMAND_STR_LEN," -m tos --tos %d/%d ",
              firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val,
              firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask);
     strlcat(command, tmp, MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
   }
   next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
   switch( next_hdr_prot )
   {
     case PS_IPPROTO_TCP:
       strlcat(command," -p tcp ",MAX_COMMAND_STR_LEN);

        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT )
        {
          if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
                     ( firewall_entry->filter_spec.next_prot_hdr.tcp.src.port + \
                     firewall_entry->filter_spec.next_prot_hdr.tcp.src.range));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
        {
          if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
                    (firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port + \
                    firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        break;
      case PS_IPPROTO_UDP:

        strlcat(command," -p udp ",MAX_COMMAND_STR_LEN);

        if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
        {
          if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.src.port,
                    (firewall_entry->filter_spec.next_prot_hdr.udp.src.range+\
                    firewall_entry->filter_spec.next_prot_hdr.udp.src.port));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.src.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
        {
          if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,
                     (firewall_entry->filter_spec.next_prot_hdr.udp.dst.range + \
                     firewall_entry->filter_spec.next_prot_hdr.udp.dst.port));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.dst.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        break;
      case PS_IPPROTO_TCP_UDP:

        strlcpy(command_sec, command, MAX_COMMAND_STR_LEN);
        strlcat(command, " -p tcp ", MAX_COMMAND_STR_LEN);
        strlcat(command_sec, " -p udp ", MAX_COMMAND_STR_LEN);

        if( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
            IPFLTR_MASK_TCP_UDP_SRC_PORT )
        {
          snprintf(tmp, MAX_COMMAND_STR_LEN, " --sport %d:%d ",
              firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
              firewall_entry->filter_spec.next_prot_hdr.tcp.src.range + \
              firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
          strlcat(command, tmp, MAX_COMMAND_STR_LEN);
          strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
             IPFLTR_MASK_TCP_UDP_DST_PORT )
        {
          snprintf(tmp, MAX_COMMAND_STR_LEN, " --dport %d:%d ",
              firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
              firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range + \
              firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
          strlcat(command, tmp, MAX_COMMAND_STR_LEN);
          strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }
        break;

      case PS_IPPROTO_ICMP6:

        strlcat(command," -p icmpv6 ",MAX_COMMAND_STR_LEN);

        if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & \
             IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --icmpv6-type %d",
                   firewall_entry->filter_spec.next_prot_hdr.icmp.type);
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & \
            IPFLTR_MASK_ICMP_MSG_CODE )
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN, "/%d ",
                   firewall_entry->filter_spec.next_prot_hdr.icmp.code);
          strlcat(command, tmp, MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }
        break;
      case PS_IPPROTO_ESP:

        strlcat(command," -p esp ",MAX_COMMAND_STR_LEN);
        if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & \
             IPFLTR_MASK_ESP_SPI )
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --espspi %d ",
                   firewall_entry->filter_spec.next_prot_hdr.esp.spi);
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        }
        break;
      default:
        LOG_MSG_ERROR("Unsupported protocol %d \n",next_hdr_prot,0,0);
        break;
    }

  if ( firewall_entry->upnp_pinhole_handle )
    snprintf(tmp,MAX_COMMAND_STR_LEN," -j %s","ACCEPT");
  else
  {
    if( QcMapFirewall->firewall_config.firewall_pkts_allowed )
      snprintf(tmp,MAX_COMMAND_STR_LEN," -j %s","ACCEPT");
    else
      snprintf(tmp,MAX_COMMAND_STR_LEN," -j %s","DROP");
  }
  strlcat(command,tmp,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("\n %s \n",command,0,0);
  ds_system_call(command, strlen(command));

  if(next_hdr_prot == PS_IPPROTO_TCP_UDP)
  {
    strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
    ds_system_call(command_sec, strlen(command_sec));
    LOG_MSG_INFO1("\n %s \n",command,0,0);
  }

  /* The below API's are need to clear the conntrack entries which would in turn delete
    the SFE entries.
    firewall_pkt_allowed =  false we need to delete the conntrack entries for the firewall IP, Port
    and Protocol combination which is entered
    firewall_pkt_allowed =  true we need to delete all other conntrack entries except for the
    firewall IP, Port and Protocol combination which is entered
    This would be applicable for Only TCP, UDP and if no protocol is mentioned (if firewall is added
    only based on sport and dport)
   */

  if ( next_hdr_prot == PS_IPPROTO_UDP || next_hdr_prot == PS_IPPROTO_TCP ||
       next_hdr_prot == PS_IPPROTO_TCP_UDP || next_hdr_prot == PS_IPPROTO_NO_PROTO )
  {
    if ( add_rule )
    {
      if ( QcMapFirewall->firewall_config.firewall_pkts_allowed )
      {
        QcMapFirewall->DeleteConntrackEntryForAcceptIPv6FirewallEntries(firewall_entry,
                                                                        PS_IPPROTO_TCP);
        QcMapFirewall->DeleteConntrackEntryForAcceptIPv6FirewallEntries(firewall_entry,
                                                                        PS_IPPROTO_UDP);
      }
      else if (!QcMapFirewall->firewall_config.firewall_pkts_allowed)
      {
        if ( next_hdr_prot == PS_IPPROTO_NO_PROTO )
        {
          QcMapFirewall->DeleteConntrackEntryForDropIPv6FirewallEntries( firewall_entry,
                                                                         PS_IPPROTO_TCP_UDP);
        }
        else
        {
          QcMapFirewall->DeleteConntrackEntryForDropIPv6FirewallEntries( firewall_entry,
                                                                         next_hdr_prot);
        }
      }
    }
  }
  return QCMAP_CM_SUCCESS;
}

/*===========================================================================
  FUNCTION SetFirewallConfig
==========================================================================*/
/*!
@brief
  Sets the frewall configuration

@parameters
  boolean enable_firewall,
  boolean pkts_allowed,
  qmi_error_type_v01  *qmi_err_num

@return
  -0 - on failure
   1- on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Firewall::SetFirewallConfig
(
  boolean enable_firewall,
  boolean pkts_allowed,
  boolean upnp_pinhole_flag,
  qmi_error_type_v01  *qmi_err_num
)
{
  uint i;
  ds_dll_el_t * node = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  qcmap_firewall_entry_list_t* firewallList = NULL;
  qcmap_msgr_firewall_entry_conf_t extd_firewall_entry;
  int firewall_count;
  pugi::xml_document xml_file;

  if (!xml_file.load_file(QCMAP_FIREWALL_CONFIG))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(QcMapFirewall)
  {
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);
    if( enable_firewall )
    {
      if(!QcMapFirewall->firewall_config.firewall_enabled)
      {
        LOG_MSG_INFO1("\n Firewall Was disabled Enabling the same \n",0,0,0);
        QcMapFirewall->firewall_config.firewall_enabled = enable_firewall;
        QcMapFirewall->firewall_config.firewall_pkts_allowed = pkts_allowed;
        QCMAP_Firewall::SetDefaultFirewall();

        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
        {
          QcMapFirewall->SetFirewall(curr_firewall_entry, true, qmi_err_num);
          //node = node->next;
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
      }

      if( QcMapFirewall->firewall_config.firewall_enabled &&
          QcMapFirewall->firewall_config.firewall_pkts_allowed != pkts_allowed)
      {
        LOG_MSG_INFO1("\n Firewall Was enabled changing the action\n",0,0,0);
        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));

        for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
        {
          QcMapFirewall->SetFirewall(curr_firewall_entry, false, qmi_err_num);
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
        QcMapFirewall->firewall_config.firewall_pkts_allowed = pkts_allowed;
        QcMapFirewall->SetDefaultFirewall();
        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
        {
          QcMapFirewall->SetFirewall(curr_firewall_entry, true, qmi_err_num);
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
      }
    }
    else
    {
      LOG_MSG_INFO1("\n In Disable firewall \n",0,0,0);
      if(QcMapFirewall->firewall_config.firewall_enabled)
      {
        LOG_MSG_INFO1("\n Firewall was enabled disabling the same \n",0,0,0);
        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
        {
          QcMapFirewall->SetFirewall(curr_firewall_entry, false, qmi_err_num);
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
      }
      QcMapFirewall->firewall_config.firewall_enabled = enable_firewall;
      QcMapFirewall->SetDefaultFirewall();
    }
    QcMapFirewall->firewall_config.upnp_inbound_pinhole = upnp_pinhole_flag;
    if (!upnp_pinhole_flag)
    {
      LOG_MSG_INFO1("If upnp flag disable when object created",0,0,0);
      node = firewallList->firewallEntryListHead;
      node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
      for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
      {
        if(curr_firewall_entry->upnp_pinhole_handle)
        {
          QCMAP_Firewall::DeleteFireWallEntry(curr_firewall_entry,
                                             curr_firewall_entry->firewall_handle, qmi_err_num);
        }
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
      }
    }
  }

  if (!upnp_pinhole_flag)
  {
      LOG_MSG_INFO1("UPNP flag is disabled when object NOT created",0,0,0);
  firewall_count = QCMAP_Firewall::GetFirewallEntryCountFromXML(&xml_file);
  LOG_MSG_INFO1(" Firewall Cont:%d ",firewall_count,0,0);
  for( i=0; i< firewall_count; i++)
  {
    if(QCMAP_Firewall::searchFirewallByIndexInXML(&xml_file, &extd_firewall_entry, i))
    {LOG_MSG_INFO1(" Insearch by index ",0,0,0);
      if( extd_firewall_entry.upnp_pinhole_handle == 1)
      {
        QCMAP_Firewall::DeleteFireWallEntry(&extd_firewall_entry,
                                            extd_firewall_entry.firewall_handle, qmi_err_num);
        LOG_MSG_INFO1("Firewall Handle num: %d ", extd_firewall_entry.firewall_handle, 0, 0);
      }
    }
  }

  }
  LOG_MSG_INFO1("\n &enable_firewall=%d, &pkts_allowed=%d,&upnp_pinhole_flag=%d \n",enable_firewall, pkts_allowed,upnp_pinhole_flag);
  QCMAP_Firewall::GetSetFirewallConfigFromXML(SET_VALUE, &enable_firewall, &pkts_allowed,&upnp_pinhole_flag);
  return true;
}

/*===========================================================================
  FUNCTION GetFirewallConfig
==========================================================================*/
/*!
@brief
  Gets the frewall configuration

@parameters
  boolean enable_firewall,
  boolean pkts_allowed,
  qmi_error_type_v01  *qmi_err_num

@return
  -0 - on failure
   1- on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Firewall::GetFirewallConfig
(
  boolean *enable_firewall,
  boolean *pkts_allowed,
  boolean *upnp_pinhole_flag,
  qmi_error_type_v01  *qmi_err_num
)
{
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  if (QcMapFirewall)
  {
    *enable_firewall = QcMapFirewall->firewall_config.firewall_enabled;

    if( QcMapFirewall->firewall_config.firewall_enabled )
      *pkts_allowed = QcMapFirewall->firewall_config.firewall_pkts_allowed;

    *upnp_pinhole_flag = QcMapFirewall->firewall_config.upnp_inbound_pinhole;
    LOG_MSG_INFO1( "\nenable_firewall = %d firewall_pkts_allowed =%d upnp_inbound_pinhole=%d\n",
               *enable_firewall,*pkts_allowed, *upnp_pinhole_flag);
    return true;
  }

  QCMAP_Firewall::GetSetFirewallConfigFromXML(GET_VALUE, enable_firewall, pkts_allowed,upnp_pinhole_flag);
  LOG_MSG_INFO1( "\nenable_firewall = %d firewall_pkts_allowed =%d upnp_inbound_pinhole=%d\n",
               *enable_firewall,*pkts_allowed, *upnp_pinhole_flag);

  return true;
}



/*===========================================================================
  FUNCTION DeleteFireWallEntry
==========================================================================*/
/*!
@brief
  Deletes the firewall entry on A5.

@parameters
  qcmap_msgr_firewall_conf_t* extd_firewall_conf
  int firewall_handle,
  qmi_error_type_v01  *qmi_err_num

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Firewall::DeleteFireWallEntry
(
   qcmap_msgr_firewall_entry_conf_t* extd_firewall_entry,
   int firewall_handle,
   qmi_error_type_v01  *qmi_err_num
)
{
  uint8_t qcmap_cm_errno;
  uint8_t ret_val,i,j;
  ds_dll_el_t * node = NULL;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  qcmap_firewall_entry_list_t* firewallList = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL;

  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child;

  if (!xml_file.load_file(QcMapMgr->cfg.firewall_config_file))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  if (!(child = QCMAP_Firewall::searchFirewallByHandleInXML(&xml_file,
                                ( extd_firewall_entry),
                                firewall_handle)))
  {
    LOG_MSG_ERROR("DeleteFireWallEntry - Error in finding the node",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
    return false;
  }

  if(QcMapFirewall)
  {
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);
    node = ds_dll_next(firewallList->firewallEntryListHead, (void *)&curr_firewall_entry);

    while(curr_firewall_entry->firewall_handle != firewall_handle && node)
    {
      node = ds_dll_next(node, (void *)&curr_firewall_entry);
    }

    if(!node)
    {
      LOG_MSG_ERROR("Invalid handle",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }

    if (node)
    {
      ds_dll_delete_node(firewallList->firewallEntryListHead,
                         &(firewallList->firewallEntryListTail),
                         node);

      //Free the firewall entry structure
      ds_free (curr_firewall_entry);
      curr_firewall_entry = NULL;

      //Free the memory of the linked list node
      ds_dll_free(node);
      node = NULL;
    }

    if( QcMapFirewall->SetFirewall(( extd_firewall_entry),
        FALSE, qmi_err_num) != QCMAP_CM_SUCCESS)
    {
      if(*qmi_err_num != QMI_ERR_INTERFACE_NOT_FOUND_V01)
      {
        LOG_MSG_ERROR("SetFirewall Entry failed\n",0,0,0);
        return  false;
      }
    }
    // Add logic to check if there are no more nodes in the list and free the head
    if (firewallList->firewallEntryListHead->next == NULL)
    {
      LOG_MSG_INFO1("\n No Firewall entry exist\n",0,0,0);
      ds_dll_free(firewallList->firewallEntryListHead);
      firewallList->firewallEntryListHead = NULL;
    }

    QcMapFirewall->firewall_config.num_firewall_entries--;
  }

  /* Update the XML file and return back if the object is not yet created. */
  parent = child.parent();
  parent.remove_child(child);

  QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);


  /* For QMI_ERR_INTERFACE_NOT_FOUND_V01 we will delete but indicate user with the error message*/
  if( *qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 )
  {
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDefaultFirewallRule
==========================================================================*/
/*!
@brief
  Set the default firewall rule based on pkts_allowed and firewall enable bits for various interfaces

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
/*=========================================================================*/
boolean QCMAP_Firewall::SetDefaultFirewallRule(qcmap_msgr_ip_family_enum_v01 interface)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2]={0};
  char command[MAX_COMMAND_STR_LEN]={0};
  qmi_error_type_v01 qmi_err_num;

  if  ( !QCMAP_ConnectionManager::GetDeviceName\
        (devname, interface, &qmi_err_num)) {
       LOG_MSG_ERROR( " Could not get backhaul interface for interface %d ",
                      interface,0,0 );
      return false;
  }

  LOG_MSG_INFO1( "SetDefaultFirewallRule:: Enter for Interface %x and devname\= %s",
                 interface,devname,0 );

  /* Default Firewall Configuration based on the mode*/
  switch (interface)
  {
    case QCMAP_MSGR_IP_FAMILY_V4_V01:
    {
      if ((QcMapFirewall->firewall_config.firewall_enabled == false) ||
         ((QcMapFirewall->firewall_config.firewall_enabled == true) &&
         (firewall_config.firewall_pkts_allowed == false)))
      {
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D PREROUTING -i %s  -j DROP",devname);

        ds_system_call(command,strlen(command));
      }
      else if ((QcMapFirewall->firewall_config.firewall_enabled == true) &&
               (firewall_config.firewall_pkts_allowed == true))
      {
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -A PREROUTING -i %s  -j DROP",devname);
        ds_system_call(command,strlen(command));
      }
    }
    break;
    case QCMAP_MSGR_IP_FAMILY_V6_V01:
    {
      if ((QcMapFirewall->firewall_config.firewall_enabled == false) ||
         ((QcMapFirewall->firewall_config.firewall_enabled == true) &&
          (firewall_config.firewall_pkts_allowed == false)))
      {
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D PREROUTING -i %s  -j DROP",devname);
        ds_system_call(command,strlen(command));
      }
      else if ((QcMapFirewall->firewall_config.firewall_enabled == true) &&
               (firewall_config.firewall_pkts_allowed == true))
      {
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -A PREROUTING -i %s  -j DROP",devname);
        ds_system_call(command,strlen(command));
      }
    }
    break;
    default:
    {
      LOG_MSG_ERROR("Incorrect Interface passed interface = %d",interface,0,0);
    }
    break;
  }

  return true;
}


/*===========================================================================
  FUNCTION CleanIPv4MangleTable
==========================================================================*/
/*!
@brief
  Flush IPV4 firewall entries

@parameters
   none

@return
   none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Firewall::CleanIPv4MangleTable(void)
{
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  qmi_error_type_v01 qmi_err_num;
  char command[MAX_COMMAND_STR_LEN]={0};
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int i;
  qcmap_firewall_entry_list_t* firewallList;
  ds_dll_el_t * node = NULL;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::cleanIPv4MangleTable() Enter",0,0,0);

  if (!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    return false;
  }

  // Delete default firewall rules
  strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
  snprintf(command,MAX_COMMAND_STR_LEN,"iptables -t mangle -D PREROUTING -i %s  -j DROP",devname);
  ds_system_call(command,strlen(command));

  if (QcMapFirewall && QcMapFirewall->firewall_config.firewall_enabled)
  {
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
    {
      if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V4 )
      {
        QcMapFirewall->SetFirewallV4(firewall_entry,false, &qmi_err_num);
      }
      node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }
}

/*===========================================================================
  FUNCTION cleanIPv6MangleTable
==========================================================================*/
/*!
@brief
  Flush IPV6 firewall entries

@parameters
   none

@return
   none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Firewall::CleanIPv6MangleTable(void)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qcmap_msgr_firewall_entry_conf_t *firewall_entry = NULL;
  qmi_error_type_v01 qmi_err_num;
  char command[MAX_COMMAND_STR_LEN]={0};
  int i;
  qcmap_firewall_entry_list_t* firewallList;
  ds_dll_el_t * node = NULL;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::cleanIPv6MangleTable() Enter",0,0,0);

  if (!QcMapBackhaulMgr || !QcMapBackhaulMgr->enable_ipv6)
  {
    LOG_MSG_ERROR("IPv6 backhaul not up",0,0,0);
    return false;
  }

  strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv6_interface,sizeof(devname));
  snprintf(command,MAX_COMMAND_STR_LEN,"ip6tables -t mangle -D PREROUTING -i %s  -j DROP",devname);
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,"ip6tables -t mangle -D PREROUTING -i %s  -j ACCEPT",devname);
  ds_system_call(command,strlen(command));
  // Enter IPV6 Firewall rules
  if (QcMapFirewall && QcMapFirewall->firewall_config.firewall_enabled)
  {
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
    {
      if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V6 )
      {
        QcMapFirewall->SetFirewallV6(firewall_entry, false, &qmi_err_num);
      }
      node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }
}

/*===========================================================================
  FUNCTION EnableIPv6Firewall
==========================================================================*/
/*!
@brief
  EnableIPv6Firewall based on the WAN Connect

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
/*=========================================================================*/
boolean QCMAP_Firewall::EnableIPV6DefaultFirewall(void)
{
  LOG_MSG_INFO1("QCMAP_Firewall::EnableIPV6DefaultFirewall() Enter",0,0,0);
  qcmap_msgr_ip_family_enum_v01 interface;

  interface = QCMAP_MSGR_IP_FAMILY_V6_V01;
  if (!SetDefaultFirewallRule(interface))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV6",0,0,0);
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableIPv6Firewall
==========================================================================*/
/*!
@brief
  EnableIPv6Firewall based on the WAN Connect

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
/*=========================================================================*/
boolean QCMAP_Firewall::EnableIPV6Firewall(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);

  qcmap_msgr_ip_family_enum_v01  interface = QCMAP_MSGR_IP_FAMILY_V6_V01;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry = NULL;
  int i =0;
  qmi_error_type_v01 qmi_err_num;
  ds_dll_el_t * node = NULL;
  qcmap_firewall_entry_list_t* firewallList = NULL;

  if (QcMapBackhaulWLANMgr)
  {
    LOG_MSG_INFO1("QCMAP_Firewall::EnableIPV6Firewall() Enter is_sta_enabled  = %x",
                  QcMapBackhaulWLANMgr->IsSTAAvailableV6(),0,0);

    if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
    {
      LOG_MSG_INFO1("QCMAP_Firewall::EnableIPV6Firewall() Not needed in Bridge Mode",0,0,0);
      return true;
    }
  }

  // Flush firewall rules
  CleanIPv6MangleTable();

  // Enter default firewall rule only for IPV6
  interface = QCMAP_MSGR_IP_FAMILY_V6_V01;

  if ( (QcMapFirewall) && (!QcMapFirewall->SetDefaultFirewallRule(interface)))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV6",0,0,0);
  }

  if ( (QcMapFirewall) && (QcMapFirewall->firewall_config.firewall_enabled))
  {
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
    {
      if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V6 )
      {
        QcMapFirewall->SetFirewallV6(firewall_entry,true, &qmi_err_num);
      }
      node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDefaultFirewall(void)
==========================================================================*/
/*!
@brief
  EnableIPv6Firewall based on the WAN Connect

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
/*=========================================================================*/
static void QCMAP_Firewall::SetDefaultFirewall(void)
{

  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  qcmap_msgr_ip_family_enum_v01 interface;

  LOG_MSG_INFO1("QCMAP_Firewall::SetDefaultFirewall() Enter",0,0,0);

  // Adding default firewall rules for sta and non-sta mode
  interface = QCMAP_MSGR_IP_FAMILY_V4_V01;
  if (!QcMapFirewall->SetDefaultFirewallRule(interface))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV4",0,0,0);
  }
  interface = QCMAP_MSGR_IP_FAMILY_V6_V01;
  if (!QcMapFirewall->SetDefaultFirewallRule(interface))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV6",0,0,0);
  }
}

/*===========================================================================
  FUNCTION GetFireWallEntry
==========================================================================*/
/*!
@brief
  Gets the firewall entry pointed by the handle.

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
/*=========================================================================*/
boolean QCMAP_Firewall::GetFireWallEntry
(
  qcmap_msgr_get_firewall_entry_resp_msg_v01 *resp_msg,
  uint32_t handle,
  qmi_error_type_v01 *qmi_err_num
)
{
  uint8_t i = 0;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  uint8_t ip_vsn,next_hdr_prot=0;
  ds_dll_el_t * node = NULL;
    QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  qcmap_firewall_entry_list_t* firewallList = NULL;

  qcmap_msgr_firewall_entry_conf_t curr_firewall_entry;
  pugi::xml_document xml_file;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!xml_file.load_file(QcMapMgr->cfg.firewall_config_file))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }


  /* Update the XML file and return back if the object is not yet created. */
  if (!QcMapFirewall)
  {
    LOG_MSG_INFO1("Obtaining Firewall entry from XML ", 0, 0, 0);
    if(!(QCMAP_Firewall::searchFirewallByHandleInXML(&xml_file, &curr_firewall_entry, handle)))
    {
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }
    firewall_entry = &curr_firewall_entry;
  }
  else
  {
      LOG_MSG_INFO1("Obtaining Firewall entry from List", 0, 0, 0);
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);
    if (QcMapFirewall->firewall_config.num_firewall_entries == 0)
    {
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    while(firewall_entry->firewall_handle != handle && node)
      node = ds_dll_next (node, (const void**)(&firewall_entry));

    if(!node)
    {
      LOG_MSG_ERROR("Invalid handle",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }
  }

  QcMapFirewall->Dump_firewall_conf(firewall_entry);

  ip_vsn = firewall_entry->filter_spec.ip_vsn;

  resp_msg->ip_version_valid = TRUE;

  switch(ip_vsn)
  {
    LOG_MSG_INFO1("IP_VSN %d", ip_vsn , 0, 0);

    case IP_V4:

      resp_msg->ip_version = QCMAP_MSGR_IP_FAMILY_V4_V01;
      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_SRC_ADDR )
      {
        resp_msg->ip4_src_addr_valid = TRUE;
        resp_msg->ip4_src_addr.addr =
          (firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
        resp_msg->ip4_src_addr.subnet_mask =
          (firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
        LOG_MSG_INFO1("IP4 source address is:",0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_DST_ADDR )
      {
        resp_msg->ip4_dst_addr_valid = TRUE;
        resp_msg->ip4_dst_addr.addr =
          (firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr);
        resp_msg->ip4_dst_addr.subnet_mask =
          (firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
        LOG_MSG_INFO1("IP4 dst address is:",0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 dst subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_TOS )
      {
        resp_msg->ip4_tos_valid = TRUE;
        resp_msg->ip4_tos.value =
          firewall_entry->filter_spec.ip_hdr.v4.tos.val;
        resp_msg->ip4_tos.mask =
          firewall_entry->filter_spec.ip_hdr.v4.tos.mask;
        LOG_MSG_INFO1("IP4 TOS val %d mask %d",
            resp_msg->ip4_tos.value ,
            resp_msg->ip4_tos.mask, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_NEXT_HDR_PROT )
      {
        LOG_MSG_INFO1("IP4 next header protocol %d ", resp_msg->next_hdr_prot, 0, 0);
        resp_msg->next_hdr_prot_valid = TRUE;
        resp_msg->next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
      }
      break;

    case IP_V6:

      resp_msg->ip_version = QCMAP_MSGR_IP_FAMILY_V6_V01;
      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_SRC_ADDR )
      {
        memcpy(resp_msg->ip6_src_addr.addr,
               firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        resp_msg->ip6_src_addr.prefix_len =
          firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len;
        resp_msg->ip6_src_addr_valid = TRUE;

        LOG_MSG_INFO1("IPV6 src_addr is:", 0, 0, 0);
        IPV6_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IP6 prefix length %d  ",
                      resp_msg->ip6_src_addr.prefix_len, 0, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_DST_ADDR )
      {

        memcpy(resp_msg->ip6_dst_addr.addr,
               firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
        resp_msg->ip6_dst_addr.prefix_len =
          firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len;
        resp_msg->ip6_dst_addr_valid = TRUE;
        LOG_MSG_INFO1("IPV6 dst_addr is:", 0, 0, 0);
        IPV6_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IP6 prefix length %d  ",
                      resp_msg->ip6_dst_addr.prefix_len, 0, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_TRAFFIC_CLASS )
      {
        resp_msg->ip6_trf_cls.value =
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val;
        resp_msg->ip6_trf_cls.mask =
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask;
        resp_msg->ip6_trf_cls_valid = TRUE;
        LOG_MSG_INFO1("trf class val %d  mask %d",
                      resp_msg->ip6_trf_cls.value,
                      resp_msg->ip6_trf_cls.mask, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_NEXT_HDR_PROT )
      {
        resp_msg->next_hdr_prot_valid = TRUE;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
        resp_msg->next_hdr_prot = next_hdr_prot;
        LOG_MSG_INFO1("IP6 next header protocol  %d  ", next_hdr_prot,0, 0);
      }
      break;
    default:
      LOG_MSG_ERROR("Unsupported IP protocol %d", ip_vsn, 0, 0);
  }

  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        resp_msg->tcp_udp_src.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        resp_msg->tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        resp_msg->tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("TCP src port  %d  range %d",
                      resp_msg->tcp_udp_src.port,
                      resp_msg->tcp_udp_src.range, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_DST_PORT )
      {
        resp_msg->tcp_udp_dst.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
        resp_msg->tcp_udp_dst.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        resp_msg->tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("TCP dst port  %d  range %d",
                      resp_msg->tcp_udp_dst.port,
                      resp_msg->tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_UDP:

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_UDP_SRC_PORT )
      {
        resp_msg->tcp_udp_src.port =
          (firewall_entry->filter_spec.next_prot_hdr.udp.src.port);
        resp_msg->tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        resp_msg->tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("UDP src port  %d  range %d",
                      resp_msg->tcp_udp_src.port,
                      resp_msg->tcp_udp_src.range, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_UDP_DST_PORT )
      {
        resp_msg->tcp_udp_dst.port =
          (firewall_entry->filter_spec.next_prot_hdr.udp.dst.port);
        resp_msg->tcp_udp_dst.range =
          firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        resp_msg->tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("UDP dst port  %d  range %d",
                      resp_msg->tcp_udp_dst.port ,
                      resp_msg->tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:

      if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask
           & IPFLTR_MASK_ICMP_MSG_TYPE )
      {
        resp_msg->icmp_type =
         firewall_entry->filter_spec.next_prot_hdr.icmp.type;
        resp_msg->icmp_type_valid = TRUE;
        LOG_MSG_INFO1("ICMP type  %d  ", resp_msg->icmp_type , 0, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_ICMP_MSG_CODE )
      {
        resp_msg->icmp_code =
         firewall_entry->filter_spec.next_prot_hdr.icmp.code;
        resp_msg->icmp_code_valid = TRUE;
        LOG_MSG_INFO1("ICMP code  %d  ", resp_msg->icmp_code , 0, 0);
      }
      break;

    case PS_IPPROTO_ESP:

      if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask
           & IPFLTR_MASK_ESP_SPI )
      {
        resp_msg->esp_spi =
         (firewall_entry->filter_spec.next_prot_hdr.esp.spi);
        resp_msg->esp_spi_valid = TRUE;
        LOG_MSG_INFO1("ESP spi %d", resp_msg->esp_spi , 0, 0);
      }
      break;

    case PS_IPPROTO_TCP_UDP:

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
           & IPFLTR_MASK_TCP_UDP_SRC_PORT )
      {
        resp_msg->tcp_udp_src.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port);
        resp_msg->tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range;
        resp_msg->tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("TCP_UDP port %d range %d",
                      resp_msg->tcp_udp_src.port ,
                      resp_msg->tcp_udp_src.range, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
           & IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        resp_msg->tcp_udp_dst.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port);
        resp_msg->tcp_udp_dst.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range;
        resp_msg->tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("TCP_UDP port %d range %d",
                      resp_msg->tcp_udp_dst.port ,
                      resp_msg->tcp_udp_dst.range, 0);
      }
     break;

    default:
      LOG_MSG_ERROR("Unsupported next header protocol %d",next_hdr_prot, 0, 0);
  }

  return true;
}


/*===========================================================================
  FUNCTION Dump_firewall_conf
==========================================================================*/
/*!
@brief
  Displays the values of filrewall entry.

@parameters
  qcmap_msgr_firewall_entry_conf_t *firewall_entry

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static void QCMAP_Firewall::Dump_firewall_conf( qcmap_msgr_firewall_entry_conf_t *firewall_entry)
{
  uint8_t num_entry;
  int i;
  char str[INET6_ADDRSTRLEN];

  if( firewall_entry !=NULL )
  {
    LOG_MSG_INFO1("\n Start Displaying firewall configuration of handle =%d ", firewall_entry->firewall_handle,0,0);

      if ( firewall_entry->filter_spec.ip_vsn == IP_V4 )
      {
        LOG_MSG_INFO1("\nIp version : IPv4",0,0,0);

        if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
        {
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("\nSRC Addr : %s",str,0,0);
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("\nSRC Addr Mask : %s",str,0,0);
        }

        if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR )
        {
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("\nDest Addr : %s",str,0,0);
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("\nDest Addr Mask : %s",str,0,0);
        }

        if (  firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
        {
          LOG_MSG_INFO1("\nTos value : %x ",firewall_entry->filter_spec.ip_hdr.v4.tos.val,0,0);
          LOG_MSG_INFO1("\nTos Mask : %x ",firewall_entry->filter_spec.ip_hdr.v4.tos.mask,0,0);
        }

      }else
      {
        LOG_MSG_INFO1("\nIp version : Ipv6",0,0,0);

        if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR )
        {
          readable_addr(AF_INET6,(uint32 *)&firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr32,(char *)&str);
          LOG_MSG_INFO1("\nSrc Addr : %s ",str,0,0);
          LOG_MSG_INFO1("\nSrc Prefixlen : %d ",firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len,0,0);
        }

        if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR )
        {
          readable_addr(AF_INET6,(uint32 *)&firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr32,(char *)&str);
          LOG_MSG_INFO1("\nDst Addr : %s ",str,0,0);
          LOG_MSG_INFO1("\nDst Prefixlen : %d ",firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len,0,0);
        }
      }

       switch( firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot )
      {
        case PS_IPPROTO_TCP:

          LOG_MSG_INFO1("\nProtocol : TCP",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT )
          {
            LOG_MSG_INFO1("\nSrc port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,0,0);
            LOG_MSG_INFO1("\nSrc portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.src.range,0,0);
          }

          if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
          {
            LOG_MSG_INFO1("\nDst port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,0,0);
            LOG_MSG_INFO1("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range,0,0);
          }
          break;

        case PS_IPPROTO_UDP:

          LOG_MSG_INFO1("\nProtocol: UDP",0,0,0);

          if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
          {
            LOG_MSG_INFO1("\nSrc port : %d",firewall_entry->filter_spec.next_prot_hdr.udp.src.port,0,0);
            LOG_MSG_INFO1("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.udp.src.range,0,0);
          }

          if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
          {
            LOG_MSG_INFO1("\nDst port : %d",firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,0,0);
            LOG_MSG_INFO1("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.udp.dst.range,0,0);
          }
          break;

        case PS_IPPROTO_TCP_UDP:

          LOG_MSG_INFO1("\nProtocol: TCP_UDP",0,0,0);

          if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
          {
            LOG_MSG_INFO1("\nSrc port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,0,0);
            LOG_MSG_INFO1("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,0,0);
          }

          if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
          {
            LOG_MSG_INFO1("\nDst port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port,0,0);
            LOG_MSG_INFO1("\nDst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range,0,0);
          }
          break;
        case PS_IPPROTO_ICMP:

          LOG_MSG_INFO1("\nProtocol : ICMP",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
          {
            LOG_MSG_INFO1("\nIcmp Type: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.type,0,0);
          }
          break;

        case PS_IPPROTO_ICMP6:

          LOG_MSG_INFO1("\nProtocol : ICMP6",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
          {
            LOG_MSG_INFO1("\nICMPv6 type: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.type,0,0);
          }
          break;

        case PS_IPPROTO_ESP:

          LOG_MSG_INFO1("\nProtocol : ESP",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
          {
            LOG_MSG_INFO1("\nESP spi : %d",firewall_entry->filter_spec.next_prot_hdr.esp.spi,0,0);
          }
          break;

        default:

          LOG_MSG_INFO1("\nUnsupported protocol \n",0,0,0);
          break;
      }
    }
  if(firewall_entry)
  {
    LOG_MSG_INFO1("\n End of Firewall configuration of handle =%d ",firewall_entry->firewall_handle,0,0);
  }
}


/*===========================================================================
  FUNCTION AddFireWallEntry
==========================================================================*/
/*!
@brief
  Checks for the duplicate entries of firewall entryi, if unique
  adds the firewall entry and update the same in the mobileap configuration file.

@parameters
  qcmap_msgr_firewall_conf_t* extd_firewall_conf

@return
  0  - on success
  -1 - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::AddFireWallEntry
(
  qcmap_msgr_firewall_conf_t* extd_firewall_conf,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i, add_firewall_entry = 0;
  int next_hdr_prot;
  ds_dll_el_t * node = NULL;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL, *new_firewall_entry = NULL;
  qcmap_firewall_entry_list_t* firewallList = NULL;
  int firewall_count = 0;
  boolean enable_firewall, pkts_allowed, upnp_pinhole_flag;
  pugi::xml_document xml_file;


  *qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01;

  if (!xml_file.load_file(QcMapMgr->cfg.firewall_config_file))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return QCMAP_CM_ERROR;
  }
  LOG_MSG_ERROR("upnp_pinhole_handle: %d ip vsn=%d",
                extd_firewall_conf->extd_firewall_entry.upnp_pinhole_handle,extd_firewall_conf->extd_firewall_entry.filter_spec.ip_vsn,0);
//  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_Firewall::GetSetFirewallConfigFromXML(GET_VALUE, &enable_firewall, &pkts_allowed,&upnp_pinhole_flag);

/* CHECK FOR PINHOLE*/
  if(extd_firewall_conf->extd_firewall_entry.upnp_pinhole_handle &&
     extd_firewall_conf->extd_firewall_entry.filter_spec.ip_vsn == IP_V6)
  {
    if(upnp_pinhole_flag == FALSE )
    {
      LOG_MSG_ERROR("The UPNP Inbound Pinhole flag is not set. Adding Entry not allowed\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return QCMAP_CM_ERROR;
    }
  }

  extd_firewall_conf->extd_firewall_entry.firewall_handle = rand();
  if (QcMapFirewall)
  {
    LOG_MSG_INFO1("Adding Firewall entry to List ", 0, 0, 0);
    firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);

    if ( QcMapFirewall->firewall_config.num_firewall_entries >= QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01 )
    {
      /* we reach MAX entries */
      LOG_MSG_ERROR("Exceeds maximum number of firewall entries\n",0,0,0);
      *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
      return QCMAP_MSGR_ENTRY_FULL;
    }

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
    for ( i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries && node; i++ )
    {
      if ( (memcmp(&(extd_firewall_conf->extd_firewall_entry.filter_spec),
            &(curr_firewall_entry->filter_spec),
            sizeof(ip_filter_type)) == 0) )
      {
        LOG_MSG_ERROR("Firewall entry is already present\n",0,0,0);
        /* we already added this one */
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return QCMAP_MSGR_ENTRY_PRESENT;
      }
      node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
    }

    // Add firewall entry only if firewall is enabled or UPNP Pinhole is being added
    if ((QcMapFirewall->firewall_config.firewall_enabled ||
          (extd_firewall_conf->extd_firewall_entry.upnp_pinhole_handle &&
           extd_firewall_conf->extd_firewall_entry.filter_spec.ip_vsn == IP_V6))
          && QcMapFirewall->SetFirewall(&(extd_firewall_conf->extd_firewall_entry),
                                          TRUE, qmi_err_num ) != QCMAP_CM_SUCCESS )
    {
       if( *qmi_err_num != QMI_ERR_INTERFACE_NOT_FOUND_V01 )
       {
         LOG_MSG_ERROR("SetFirewall Entry failed\n",0,0,0);
         return  QCMAP_CM_ERROR;
       }
    }
    if( QcMapFirewall->addFirewallEntryToList(&(extd_firewall_conf->extd_firewall_entry),
        new_firewall_entry) == false)
    {
      LOG_MSG_ERROR("Adding Firewall Entry to Firewall list failed.\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return QCMAP_CM_ERROR;
    }
    /* If Firewall enabled then set qmi_err_num to NONE. */
    *qmi_err_num = QMI_ERR_NONE_V01;
    add_firewall_entry = 1;

    LOG_MSG_INFO1("Applied firewall entry.", 0, 0, 0);
  }

  /* if it is OK, call write to XML */
  /* Check if we have already added 50 Entries */
  firewall_count = QCMAP_Firewall::GetFirewallEntryCountFromXML(&xml_file);
  QCMAP_CM_LOG_FUNC_ENTRY();

  if(firewall_count >= QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01)
  {
    LOG_MSG_ERROR("Exceeds maximum number of firewall entries\n",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return QCMAP_MSGR_ENTRY_FULL;
  }

  if((add_firewall_entry ||!(QCMAP_Firewall::searchFirewallByIndexInXML(&xml_file,
      &(extd_firewall_conf->extd_firewall_entry),SEARCH_FIREWALL_ENTRY))))
  {
    LOG_MSG_INFO1("Adding Firewall entry to XML file.", 0, 0, 0);
    QCMAP_Firewall::addFirewallEntryToXML(&xml_file,
                                   &(extd_firewall_conf->extd_firewall_entry));
    QcMapFirewall->Dump_firewall_conf(&(extd_firewall_conf->extd_firewall_entry));
  }
  else
  {
    LOG_MSG_ERROR("Firewall entry is already present\n",0,0,0);
    /* we already added this one */
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return QCMAP_MSGR_ENTRY_PRESENT;
  }


  /*we will indicate the user that interface was down but entry is stored */
  if( *qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 )
  {
    LOG_MSG_ERROR("Entry is added but not reflected in kernel iptables yet\n",0,0,0);
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_SUCCESS;
}


/*===========================================================================
  FUNCTION SetFirewall
==========================================================================*/
/*!
@brief
  Adds firewall entry to the appropriate IP tables based on IPV4 and IPV6

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  boolean add_rule

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::SetFirewall
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num)
{

  uint32_t addr;
  uint8 next_hdr_prot;

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }


  if (firewall_entry->filter_spec.ip_vsn == IP_V4 )
  {
    if ( SetFirewallV4( firewall_entry, add_rule, qmi_err_num ) != QCMAP_CM_SUCCESS )
    {
      return QCMAP_CM_ERROR;
    }
  }
  else if ( firewall_entry->filter_spec.ip_vsn == IP_V6 )
  {
    if( SetFirewallV6( firewall_entry, add_rule, qmi_err_num ) != QCMAP_CM_SUCCESS )
    {
      return QCMAP_CM_ERROR;
    }
  }
  else
  {
    LOG_MSG_ERROR("Invalid IP Version %x",firewall_entry->filter_spec.ip_vsn,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_SUCCESS;
}

/*===========================================================================
  FUNCTION GetFirewallEntryCountFromXML
==========================================================================*/
/*!
@brief
  Get the firewall entry count from the XML.

@parameters
  pugi::xml_document *xml_file

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

int QCMAP_Firewall::GetFirewallEntryCountFromXML(pugi::xml_document *xml_file)
{
  pugi::xml_node root;

  root = xml_file->child(System_TAG).child(MobileAPFirewallCfg_TAG);

  return root.select_nodes(Firewall_TAG).size();
}
/*===========================================================================
  FUNCTION GetFireWallHandleList
==========================================================================*/
/*!
@brief
  Get all the handles of firewall entries.

@parameters
  qcmap_msgr_firewall_conf_t *get_handle_list

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
boolean QCMAP_Firewall::GetFireWallHandleList
(
  qcmap_msgr_firewall_conf_t *get_handle_list,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i,len = 0;

  ds_dll_el_t * node = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL;
  int family;
  family=get_handle_list->ip_family;
  qcmap_msgr_firewall_entry_conf_t extd_firewall_entry;
  pugi::xml_document xml_file;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  qcmap_firewall_entry_list_t* firewallList = NULL;
  int firewall_count = 0;

  if (!xml_file.load_file(QCMAP_FIREWALL_CONFIG))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }


  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Update the XML file and return back if the object is not yet created. */
  if (!QcMapFirewall)
  {
    firewall_count = QCMAP_Firewall::GetFirewallEntryCountFromXML(&xml_file);

    for( i=0; i< firewall_count; i++)
    {
      if(QCMAP_Firewall::searchFirewallByIndexInXML(&xml_file, &extd_firewall_entry, i))
      {
        if( family == extd_firewall_entry.filter_spec.ip_vsn)
        {
          get_handle_list->extd_firewall_handle_list.handle_list[len++] = extd_firewall_entry.firewall_handle;
          LOG_MSG_INFO1("Firewall Handle num: %d ", extd_firewall_entry.firewall_handle, 0, 0);
        }
      }
    }
    get_handle_list->extd_firewall_handle_list.num_of_entries = len;
    return true;
  }

  firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);

  node = firewallList->firewallEntryListHead;
  node = ds_dll_next (node, (const void**)(&curr_firewall_entry));

  for( i=0; i< QcMapFirewall->firewall_config.num_firewall_entries && node; i++)
  {
    if( family == curr_firewall_entry->filter_spec.ip_vsn)
    {
      get_handle_list->extd_firewall_handle_list.handle_list[len++] = curr_firewall_entry->firewall_handle;
    }
    //node = node->next;
    node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
  }
  get_handle_list->extd_firewall_handle_list.num_of_entries = len;
  return true;
}

/*===========================================================================
  FUNCTION GetFirewallEntryFromNode
==========================================================================*/
/*!
@brief
  Get the firewall entries from Node.

@parameters
  pugi::xml_node child,
  qcmap_msgr_firewall_entry_conf_t *firewall_conf

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

boolean QCMAP_Firewall::GetFirewallEntryFromXML
(
  pugi::xml_node child,
  qcmap_msgr_firewall_entry_conf_t *firewall_conf
)
{
  pugi::xml_node subchild;
  int next_hdr;
  int type = 0, code = 0;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  struct in6_addr ip6_addr;
  firewall_conf->filter_spec.ip_vsn = atoi(child.child_value(IPFamily_TAG));
  firewall_conf->firewall_handle = atoi(child.child(FirewallHandle_TAG).child_value());
  firewall_conf->upnp_pinhole_handle= atoi(child.child_value(PinholeEntry_TAG));
     if(firewall_conf->filter_spec.ip_vsn == IP_V4)
     {
       subchild = child.child(IPV4SourceAddress_TAG);

       if (subchild)
       {
         firewall_conf->filter_spec.ip_hdr.v4.field_mask|=IPFLTR_MASK_IP4_SRC_ADDR;
         firewall_conf->filter_spec.ip_hdr.v4.src.addr.ps_s_addr =\
           inet_addr(subchild.child(IPV4SourceIPAddress_TAG).child_value());

         firewall_conf->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr =\
           inet_addr(subchild.child(IPV4SourceSubnetMask_TAG).child_value());
       }

       subchild = child.child(IPV4DestinationAddress_TAG);

       if (subchild)
       {
         firewall_conf->filter_spec.ip_hdr.v4.field_mask |=IPFLTR_MASK_IP4_DST_ADDR;

         firewall_conf->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr =
          inet_addr(subchild.child(IPV4DestinationIPAddress_TAG).child_value());

         firewall_conf->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr =
         inet_addr(subchild.child(IPV4DestinationSubnetMask_TAG).child_value());
       }

       subchild = child.child(IPV4TypeOfService_TAG);

       if (subchild)
       {
         firewall_conf->filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
         firewall_conf->filter_spec.ip_hdr.v4.tos.val =
                               atoi(subchild.child(TOSValue_TAG).child_value());
         firewall_conf->filter_spec.ip_hdr.v4.tos.mask =
                                atoi(subchild.child(TOSMask_TAG).child_value());
        LOG_MSG_INFO1("TOS Value: %d, TOS Mask: %d",
                       firewall_conf->filter_spec.ip_hdr.v4.tos.val,
                       firewall_conf->filter_spec.ip_hdr.v4.tos.mask,0);
       }

       firewall_conf->filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
       firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot =
                    atoi(child.child(IPV4NextHeaderProtocol_TAG).child_value());
       next_hdr = firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot;

     }

    if(firewall_conf->filter_spec.ip_vsn == IP_V6)
    {
      subchild = child.child(IPV6SourceAddress_TAG);

      if (subchild)
      {
        firewall_conf->filter_spec.ip_hdr.v6.field_mask |=IPFLTR_MASK_IP6_SRC_ADDR;

        inet_pton(AF_INET6,subchild.child(IPV6SourceIPAddress_TAG).\
                  child_value(), &ip6_addr);

        memcpy(firewall_conf->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               ip6_addr.s6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        firewall_conf->filter_spec.ip_hdr.v6.src.prefix_len =
                   atoi(subchild.child(IPV6SourcePrefix_TAG).child_value());
        LOG_MSG_INFO1("IPV6SourcePrefix: %d",
                      firewall_conf->filter_spec.ip_hdr.v6.src.prefix_len,0,0);
      }

      subchild = child.child(IPV6DestinationAddress_TAG);
      if (subchild)
      {
        firewall_conf->filter_spec.ip_hdr.v6.field_mask |=
               IPFLTR_MASK_IP6_DST_ADDR;
        inet_pton(AF_INET6,subchild.child(IPV6DestinationIPAddress_TAG).\
                  child_value(), &ip6_addr);
        memcpy(firewall_conf->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               ip6_addr.s6_addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        firewall_conf->filter_spec.ip_hdr.v6.dst.prefix_len =
         atoi(subchild.child(IPV6DestinationPrefix_TAG).child_value());

        LOG_MSG_INFO1( "IPV6DestinationPrefix: %d",
                       firewall_conf->filter_spec.ip_hdr.v6.dst.prefix_len,0,0 );
      }

      subchild = child.child(IPV6TrafficClass_TAG);
      if (subchild)
      {
        firewall_conf->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
        firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val =
                            atoi(subchild.child(TrfClsValue_TAG).child_value());

        firewall_conf->filter_spec.ip_hdr.v6.trf_cls.mask =
                            atoi(subchild.child(TrfClsMask_TAG).child_value());
        LOG_MSG_INFO1( "TrfClsValue: %d , TrfClsMask: %d",
                       firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val ,
                       firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val,0 );
      }

      firewall_conf->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot =
                    atoi(child.child(IPV6NextHeaderProtocol_TAG).child_value());
      next_hdr = firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot;
    }

    switch(next_hdr)
    {
      case PS_IPPROTO_TCP:
      {
        subchild = child.child("TCPSource");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask |=
              IPFLTR_MASK_TCP_SRC_PORT;
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.port =
                           atoi(subchild.child("TCPSourcePort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.range =
                           atoi(subchild.child("TCPSourceRange").child_value());
          LOG_MSG_INFO1("TCPSourcePort: %d , TCPSourceRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,0);
        }

        subchild = child.child("TCPDestination");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask |=
              IPFLTR_MASK_TCP_DST_PORT;

          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port =
                      atoi(subchild.child("TCPDestinationPort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range =
                     atoi(subchild.child("TCPDestinationRange").child_value());
          LOG_MSG_INFO1( "TCPDestinationPort: %d , TCPDestinationRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,
                         0 );
        }
      }
      break;
      case PS_IPPROTO_UDP:
      {
        subchild = child.child("UDPSource");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.udp.field_mask |=
              IPFLTR_MASK_UDP_SRC_PORT;
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.port =
                           atoi(subchild.child("UDPSourcePort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.range =
                           atoi(subchild.child("UDPSourceRange").child_value());
          LOG_MSG_INFO1("UDPSourcePort: %d , UDPSourceRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,0);
        }

        subchild = child.child("UDPDestination");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.udp.field_mask |=
              IPFLTR_MASK_UDP_DST_PORT;
          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port =
                      atoi(subchild.child("UDPDestinationPort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range =
                      atoi(subchild.child("UDPDestinationRange").child_value());

          LOG_MSG_INFO1("UDPDestinationPort: %d , UDPDestinationRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,0);
        }
      }
      break;

       case PS_IPPROTO_ICMP:
       case PS_IPPROTO_ICMP6:
       {
         type = atoi(child.child(ICMPType_TAG).child_value());
         if (type)
         {
           firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask
                 |= IPFLTR_MASK_ICMP_MSG_TYPE;
           firewall_conf->filter_spec.next_prot_hdr.icmp.type = type;
         }

         code = atoi(child.child(ICMPCode_TAG).child_value());
         if (code)
         {
           firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask
                 |= IPFLTR_MASK_ICMP_MSG_CODE;
           firewall_conf->filter_spec.next_prot_hdr.icmp.code =
                                  atoi(child.child(ICMPCode_TAG).child_value());
         }
       }
       break;

       case PS_IPPROTO_ESP:
       {
         firewall_conf->filter_spec.next_prot_hdr.esp.spi =
                                    atoi(child.child(ESPSPI_TAG).child_value());
         firewall_conf->filter_spec.next_prot_hdr.esp.field_mask
                 |= IPFLTR_MASK_ESP_SPI;
       }
       break;

       case PS_IPPROTO_TCP_UDP:
       {
         subchild = child.child(TCP_UDPSource_TAG);
         if (subchild)
         {
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                   IPFLTR_MASK_TCP_UDP_SRC_PORT;
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port
                    = atoi(subchild.child(TCP_UDPSourcePort_TAG).child_value());

           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range
                   = atoi(subchild.child(TCP_UDPSourceRange_TAG).child_value());

           LOG_MSG_INFO1("TCP_UDPSourcePort: %d , TCP_UDPSourceRange: %d",
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,0);

         }
       }
       break;

       default:
       break;
     }

  return;
}

/*===========================================================================
  FUNCTION searchFirewallByHandleInXML
==========================================================================*/
/*!
@brief
  This API both searches and retrives a Firewall entry based on the handle.

@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf,
  int handle

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

static pugi::xml_node QCMAP_Firewall::searchFirewallByHandleInXML(pugi::xml_document *xml_file,
                    qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf, int handle)
{
  pugi::xml_node root, child ;
  qcmap_msgr_firewall_entry_conf_t firewall_conf;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  bzero(&firewall_conf,sizeof(firewall_conf));

  root = xml_file->child("system").child("MobileAPFirewallCfg");

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), "Firewall"))
    {
      memset(&firewall_conf, 0, sizeof(firewall_conf));
      QcMapFirewall->GetFirewallEntryFromXML(child, &firewall_conf);
      if(handle == firewall_conf.firewall_handle)
      {
        memcpy(extd_firewall_conf, &firewall_conf, sizeof(firewall_conf));
        return child;
      }
    }
  }
  return child;
}

/*===========================================================================
  FUNCTION searchFirewallByIndexInXML
==========================================================================*/
/*!
@brief
  This API both searches and retrives a Firewall entry based on the "entry"
variable. If entry = SEARCH_FIREWALL_ENTRY, it will try to search for an entry indicated by
extd_firewall_conf.

If entry != SEARCH_FIREWALL_ENTRY, this API would retrieve the corresponding Firewall entry
as indicated by the count.


@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf,
  int entry

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
static pugi::xml_node QCMAP_Firewall::searchFirewallByIndexInXML(pugi::xml_document *xml_file,
                    qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf, int entry)
{
  pugi::xml_node root, child ;
  qcmap_msgr_firewall_entry_conf_t firewall_conf;
  int count = 0;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  bzero(&firewall_conf,sizeof(firewall_conf));

  root = xml_file->child("system").child("MobileAPFirewallCfg");

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), "Firewall"))
    {
      if((entry == SEARCH_FIREWALL_ENTRY) || (entry == count))
      {
        memset(&firewall_conf, 0, sizeof(firewall_conf));
        QcMapFirewall->GetFirewallEntryFromXML(child, &firewall_conf);
        if(entry == count)
        {
          memcpy(extd_firewall_conf, &firewall_conf, sizeof(firewall_conf));
          return child;
        }
        else
        {
          if(memcmp(&(extd_firewall_conf->filter_spec), &(firewall_conf.filter_spec), sizeof(ip_filter_type)) == 0)
          {
            LOG_MSG_ERROR("Found entry with same spec",0,0,0);
            return child;
          }
        }
      }
      count++;
    }
  }
  return child;
}


/*===========================================================================
  FUNCTION ReadFirewallXML
==========================================================================*/
/*!
@brief
  Reads the firewall XML


@parameters
  None

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

boolean QCMAP_Firewall::ReadFirewallXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  qcmap_msgr_firewall_conf_t firewall_conf;
  qcmap_msgr_firewall_entry_conf_t *new_firewall_entry = NULL;
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);


  if (!xml_file.load_file(QcMapMgr->cfg.firewall_config_file))
  {
    LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPFirewallCfg_TAG);
  this->firewall_config.firewall_enabled = atoi(root.child(FirewallEnabled_TAG).child_value());
  this->firewall_config.firewall_pkts_allowed = atoi(root.child(FirewallPktsAllowed_TAG).child_value());
  this->firewall_config.upnp_inbound_pinhole= atoi(root.child(UPNPInboundPinhole_TAG).child_value());
  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), Firewall_TAG))
    {
      memset(&firewall_conf, 0, sizeof(qcmap_msgr_firewall_conf_t));
      this->GetFirewallEntryFromXML(child, &(firewall_conf.extd_firewall_entry));
      if(this->addFirewallEntryToList(&(firewall_conf.extd_firewall_entry), new_firewall_entry) == false)
      {
        LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
        return false;
      }
    }
  }
  LOG_MSG_INFO1("Firewall enabled: %d Pkts allowed: %d UPNP Pinhole Allowed: %d",this->firewall_config.firewall_enabled,
                this->firewall_config.firewall_pkts_allowed, this->firewall_config.upnp_inbound_pinhole);

  return true;
}


/*===========================================================================
  FUNCTION GetSetFirewallConfigFromXML
==========================================================================*/
/*!
@brief
  Get or set the firewall XML


@parameters
  qcmap_action_type action,
  boolean *enable_firewall,
  boolean *pkts_allowed

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

boolean QCMAP_Firewall::GetSetFirewallConfigFromXML
(
  qcmap_action_type action,
  boolean *enable_firewall,
  boolean *pkts_allowed,
  boolean *upnp_pinhole_allow
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (!xml_file.load_file(QcMapMgr->cfg.firewall_config_file))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPFirewallCfg_TAG);
  LOG_MSG_INFO1("action: %d enable: %d pkts: %d ", action, *enable_firewall, *pkts_allowed);
  switch (action)
  {
    case SET_VALUE:
      snprintf(data, sizeof(data), "%d", *enable_firewall);
      root.child(FirewallEnabled_TAG).text() = data;
      if(*enable_firewall!=0)
      {
        snprintf(data, sizeof(data), "%d", *pkts_allowed);
        root.child(FirewallPktsAllowed_TAG).text() = data;
      }
      snprintf(data, MAX_COMMAND_STR_LEN, "%d", *upnp_pinhole_allow);
      root.child(UPNPInboundPinhole_TAG).text() = data;
      QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
    break;
    case GET_VALUE:
      LOG_MSG_INFO1("GET VALUE",0,0,0);
      *enable_firewall = atoi(root.child(FirewallEnabled_TAG).child_value());
      *pkts_allowed = atoi(root.child(FirewallPktsAllowed_TAG).child_value());
      *upnp_pinhole_allow = atoi(root.child(UPNPInboundPinhole_TAG).child_value());
    break;

    default:
      LOG_MSG_ERROR("Invalid action", 0, 0, 0);
      return false;
    break;
  }
  return TRUE;
}


/*======================================================
  FUNCTION:  WriteDefaultsToFirewallXML
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
void QCMAP_Firewall::WriteDefaultsToFirewallXML()
{
  FILE *fp = NULL;
  char *xml_file=QCMAP_ConnectionManager::firewall_xml_path;

  LOG_MSG_INFO1("Creating XML file using default values",0,0,0);
  fp = fopen(xml_file, "wb");

  /* File not found at the given path */
  if (NULL == fp)
  {
    return QCMAP_CM_ERROR;
  }

  fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(fp,
		  "<system xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"mobileap_firewall_cfg.xsd\">\n");
  fprintf(fp, "\t<MobileAPFirewallCfg>\n");
  fprintf(fp, "\t<FirewallEnabled>%d</FirewallEnabled>\n", 0);
  fprintf(fp, "\t<FirewallPktsAllowed>%d</FirewallPktsAllowed>\n",0);
  fprintf(fp, "\t<UPNPInboundPinhole>%d</UPNPInboundPinhole>\n",0);

  fprintf(fp, "\t</MobileAPFirewallCfg>\n");
  fprintf(fp, "</system>\n");
  fclose(fp);
  return (QCMAP_CM_SUCCESS);
  }

/*======================================================
  FUNCTION:  ConntrackDeleteForDropFirewallEntries
  =====================================================*/
  /*!
      @brief
      Delete the client conntrack when we are adding a firewall entry for
      that IPv6 address and port combination

      @params
         IPv6 address
         Protocol
         UDP Port start number
         UDP Port Range
         TCP Port start number
         TCP Port Range

      @return
      void
  */
/*====================================================*/
void QCMAP_Firewall::DeleteConntrackEntryForDropIPv6FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  uint8 next_hdr_prot = 0;
  uint16 prefixbitvalue = 0;
  char command[MAX_COMMAND_STR_LEN]={0};
  char command_backup[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char v6str[INET6_ADDRSTRLEN]={0};
  char ip6_grep_buf[INET6_ADDRSTRLEN]={0};
  uint8 prefixWords = 0;
  uint8 prefixBits = 0;
  uint8  ret = 0;
  bool check_sport = false;
  bool check_dport = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port;
  FILE *fd = NULL;
  char *protocol=NULL,*saddr=NULL, *daddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  in6_addr tmpipv6, curr_ipv6_addr;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  int j = 0;
  bool processV6addr =  false;
  bool skipPrefixBitProcess = false;

  LOG_MSG_INFO1("DeleteConntrackEntryForDropIPv6FirewallEntries \n",0,0,0);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    return;
  }

  if (protocol_num != PS_IPPROTO_TCP && protocol_num != PS_IPPROTO_UDP)
  {
    if ( protocol_num == PS_IPPROTO_TCP_UDP)
    {
      QcMapFirewall->DeleteConntrackEntryForDropIPv6FirewallEntries(firewall_entry,PS_IPPROTO_TCP);
      QcMapFirewall->DeleteConntrackEntryForDropIPv6FirewallEntries(firewall_entry,PS_IPPROTO_UDP);
    }
    return true;
  }

  /*
   */
  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,9,11,12 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,10,12,13 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
          check_sport = true;
        }
        else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
          check_sport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;
  }

  if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    prefixWords = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len/(sizeof(uint16)*8);
    prefixBits = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len%(sizeof(uint16)*8);

    memcpy (&curr_ipv6_addr.s6_addr ,
            firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr16,
            QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    prefixbitvalue = curr_ipv6_addr.s6_addr[prefixWords] >> (16 - prefixBits);

    if ( prefixBits == 0 && prefixWords == 8 )
    {
      processV6addr = false;

      inet_ntop(AF_INET6,
               (void*)&curr_ipv6_addr, v6str, sizeof(v6str));
      snprintf(tmp, MAX_COMMAND_STR_LEN,"| grep src=%s", (char*)v6str);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    else
    {
      processV6addr = true;
    }
  }


  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPv6_CONNTRACK_FILTER_PATH);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  printf("Grep : %s \n", command);
  ds_system_call(command, strlen(command));

  fd = fopen(IPv6_CONNTRACK_FILTER_PATH,"r");


  if(fd == NULL)
  {
    LOG_MSG_ERROR("FetchPortInfoFromV6Conntrack - Error in opening ",0,0,0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    skipPrefixBitProcess = false;

    protocol = strtok_r(stringline, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);

    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);

    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);

    if(dport)
      *(dport + 5) = ' ';

      //Copy to temp str then pton and then check prefixword byte value after right shifting

    if (processV6addr)
    {
      if(saddr)
        inet_pton(AF_INET6, saddr + 4, (void *)&tmpipv6);

      if (memcmp (&curr_ipv6_addr.s6_addr16, &tmpipv6.s6_addr16,
                  (sizeof (uint16))*prefixWords) != 0)
      {
        skipPrefixBitProcess = true;
      }

      if ((skipPrefixBitProcess)||
           ( tmpipv6.s6_addr16[prefixWords] >> ((sizeof(uint16)*8)-prefixBits) != prefixbitvalue))
      {
        continue;
      }
    }

    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
      continue;
    }

    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        continue;
    }

    //Delete contrack entry
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);
  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPv6_CONNTRACK_FILTER_PATH);
  ds_system_call(command, strlen(command));
}


/*======================================================
  FUNCTION:  ConntrackDeleteForAcceptFirewallEntries
  =====================================================*/
  /*!
      @brief
      Delete the client conntrack when we are adding a firewall entry for
      that IPv6 address and port combination

      @params
         IPv6 address
         Protocol
         UDP Port start number
         UDP Port Range
         TCP Port start number
         TCP Port Range

      @return
      void
  */
/*====================================================*/
void QCMAP_Firewall::DeleteConntrackEntryForAcceptIPv6FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  uint8 next_hdr_prot = 0;
  uint16 prefixbitvalue = 0;
  char command[MAX_COMMAND_STR_LEN]={0};
  char command_backup[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char v6str[INET6_ADDRSTRLEN]={0};
  char ip6_grep_buf[INET6_ADDRSTRLEN]={0};
  uint8 prefixWords = 0;
  uint8 prefixBits = 0;
  uint8  ret = 0;
  bool check_sport = false;
  bool check_dport = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port;
  FILE *fd = NULL;
  char *protocol=NULL,*saddr=NULL, *daddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  in6_addr tmpipv6, curr_ipv6_addr;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  int j = 0;
  bool processV6addr =  false;
  bool skipPrefixBitProcess = false;

  LOG_MSG_INFO1("DeleteConntrackEntryForAcceptIPv6FirewallEntries \n",0,0,0);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    return;
  }

  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,9,11,12 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,10,12,13 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        check_dport = true;
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        check_dport = true;
      }

      break;
  }

  if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    prefixWords = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len/(sizeof(uint16)*8);
    prefixBits = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len%(sizeof(uint16)*8);

    memcpy (&curr_ipv6_addr.s6_addr ,
            firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr16,
            QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    if (prefixWords < 8)
    {
      prefixbitvalue = curr_ipv6_addr.s6_addr[prefixWords] >> (16 - prefixBits);
    }

    processV6addr = true;
  }


  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPv6_CONNTRACK_FILTER_PATH);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  printf("Grep : %s \n", command);
  ds_system_call(command, strlen(command));

  fd = fopen(IPv6_CONNTRACK_FILTER_PATH,"r");


  if(fd == NULL)
  {
    LOG_MSG_ERROR("FetchPortInfoFromV6Conntrack - Error in opening ",0,0,0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    skipPrefixBitProcess = false;

    protocol = strtok_r(stringline, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);

    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);

    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);

    if(dport)
      *(dport + 5) = ' ';
      //Copy to temp str then pton and then check prefixword byte value after right shifting

    if (processV6addr)
    {
      if(saddr)
        inet_pton(AF_INET6, saddr + 4, (void *)&tmpipv6);
      if (memcmp (&curr_ipv6_addr.s6_addr16, &tmpipv6.s6_addr16,
                  (sizeof (uint16))*prefixWords) != 0)
      {
        goto delete_entry_v6;
      }

      if ((prefixWords < 8) &&
           (tmpipv6.s6_addr16[prefixWords] >> ((sizeof(uint16)*8)-prefixBits) != prefixbitvalue))
      {
        goto delete_entry_v6;
      }
    }

    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
        goto delete_entry_v6;
    }

    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        goto delete_entry_v6;
    }

    /*check protocol*/
    if (firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP ||
        firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot == protocol_num ||
        firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot == 0)
      continue;

    //Delete contrack entry
    delete_entry_v6:
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);

  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPv6_CONNTRACK_FILTER_PATH);
  ds_system_call(command, strlen(command));
}
