/*====================================================

FILE:  QCMAP_NATALG.cpp

SERVICES:
   QCMAP Connection Manager NAT ALG Specific Implementation

=====================================================

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/21/14   vm         9x45
  06/12/15   rk         Offtarget support.
==========================================================*/
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
#include "QCMAP_NATALG.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_LAN.h"
bool QCMAP_NATALG::flag= false;
QCMAP_NATALG* QCMAP_NATALG::object=NULL;

QCMAP_NATALG* QCMAP_NATALG::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : NATALG",0, 0, 0);
    object = new QCMAP_NATALG();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*==========================================================
                             Class Definitions
  =========================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes NAT variables and reads XML file for NAT related configuration.

@parameters

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_NATALG::QCMAP_NATALG()
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qmi_error_type_v01 qmi_err_num;

  memset(&(this->nat_config), 0, sizeof(this->nat_config));
  /*Symmetric NAT as default */
  this->nat_config.nat_type = QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01;
  this->nat_config.enable_ipsec_vpn_pass_through = true;
  this->nat_config.enable_l2tp_vpn_pass_through = true;
  this->nat_config.enable_pptp_vpn_pass_through = true;
  this->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
  this->nat_config.dmz_ip = 0;

    /* Initialize default timeout values. */
    this->nat_config.nat_entry_generic_timeout =
                                       QCMAP_NAT_ENTRY_DEFAULT_GENERIC_TIMEOUT;

    this->nat_config.nat_entry_icmp_timeout =
                                          QCMAP_NAT_ENTRY_DEFAULT_ICMP_TIMEOUT;

    this->nat_config.nat_entry_tcp_established_timeout =
                                           QCMAP_NAT_ENTRY_DEFAULT_TCP_TIMEOUT;

    this->nat_config.nat_entry_udp_timeout = QCMAP_NAT_ENTRY_DEFAULT_UDP_TIMEOUT;

    /* Set the timeout values. */
    this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01,
                            this->nat_config.nat_entry_generic_timeout,
                            &qmi_err_num);
    this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01,
                            this->nat_config.nat_entry_icmp_timeout,
                            &qmi_err_num);
    this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01,
                            this->nat_config.nat_entry_tcp_established_timeout,
                            &qmi_err_num);
    this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_UDP_V01,
                            this->nat_config.nat_entry_udp_timeout,
                            &qmi_err_num);
  /* Initialize alg enable disable mask to be zero */
  this->alg_enabled_disabled_mask = 0;
  if(this->ReadNATConfigFromXML(QcMapMgr->xml_path) == FALSE)
  {
    LOG_MSG_ERROR("Error in loading NAT Configuration\n", 0, 0, 0);
    return;
  }
  /* Set the timeout values. */
  this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01,
                          this->nat_config.nat_entry_generic_timeout,
                          &qmi_err_num);
  this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01,
                          this->nat_config.nat_entry_icmp_timeout,
                          &qmi_err_num);
  this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01,
                          this->nat_config.nat_entry_tcp_established_timeout,
                          &qmi_err_num);
  this->SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_UDP_V01,
                          this->nat_config.nat_entry_udp_timeout,
                          &qmi_err_num);
  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Frees memory allocated for NAT Entries.

@parameters

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_NATALG::~QCMAP_NATALG()
{
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  qcmap_nat_entry_list_t* natList = &(QcMapNatAlg->nat_config.port_fwding_entries);
  LOG_MSG_INFO1("Destroying NATALG object",0,0,0);
  /* Delete the NAT entry list */
  ds_dll_delete_all(natList->natEntryListHead);
  natList->natEntryListHead = NULL;
  natList->natEntryListTail = NULL;

  flag=false;
  object=NULL;
}

/*==========================================================
  FUNCTION GetNatTimeout
==========================================================*/
/*!
@brief
  Gets the NAT timeout value configured on A5. If NAT object is not present yet, obtain the
  values from XML

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          timeout_value

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
boolean QCMAP_NATALG::GetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 *timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  if (QcMapNatAlg)
  {
    if (!QcMapNatAlg->GetNatTimeoutOnApps(timeout_type, timeout_value, qmi_err_num))
    {
      return false;
    }
    return true;
  }

  switch ( timeout_type )
  {
    case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_GENERIC_TIMEOUT,
                                           GET_VALUE, data, MAX_STRING_LENGTH);
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT,
                                           GET_VALUE, data, MAX_STRING_LENGTH);
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_UDP_TIMEOUT,
                                           GET_VALUE, data, MAX_STRING_LENGTH);
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_ICMP_TIMEOUT,
                                           GET_VALUE, data, MAX_STRING_LENGTH);
    break;

    default:
      LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
  }
  *timeout_value = atoi(data);

  return true;
}


/*==========================================================
  FUNCTION SetNatTimeout
==========================================================*/
/*!
@brief
  Sets the NAT timeout on A5.

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          timeout_value

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
boolean QCMAP_NATALG::SetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  if (timeout_value < QCMAP_NAT_ENTRY_MIN_TIMEOUT)
  {
    LOG_MSG_ERROR("Timeout value should be greater than: %d Got: %d.\n",
                  QCMAP_NAT_ENTRY_MIN_TIMEOUT, timeout_value, 0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }

  if (QcMapNatAlg)
  {
    if (!QcMapNatAlg->SetNatTimeoutOnApps(timeout_type, timeout_value, qmi_err_num))
    {
      return false;
    }
  }
  snprintf(data, sizeof(data), "%d", timeout_value);
  switch ( timeout_type )
  {
    case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:

      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_GENERIC_TIMEOUT,
                                           SET_VALUE,
                                           data, strlen(data));
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT,
                                           SET_VALUE,
                                           data, strlen(data));
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_UDP_TIMEOUT,
                                           SET_VALUE,
                                           data, strlen(data));
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_ICMP_TIMEOUT,
                                           SET_VALUE,
                                           data, strlen(data));
    break;

    default:
      LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    break;
  }

  return true;
}

/*==========================================================
  FUNCTION GetNATEntryCountFromXML
==========================================================*/
/*!
@brief
  Returns the number NAT entries present in XML.

@parameters
  pugi::xml_document *xml_file

@return
  int: Count of NAT entries.

*/
/*=========================================================*/

int QCMAP_NATALG::GetNATEntryCountFromXML(pugi::xml_document *xml_file)
{
  pugi::xml_node root;

  root = xml_file->child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  return root.select_nodes(PortFwding_TAG).size();
}


/*==========================================================
  FUNCTION AddStaticNatEntry
==========================================================*/
/*!
@brief
  Checks nat entry for the redundancy and if unique adds a new snat entry.

@parameters
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
boolean QCMAP_NATALG::AddStaticNatEntry
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i;
  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = NULL;
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL, *new_nat_entry = NULL;
  pugi::xml_document xml_file;
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (QcMapNatAlg)
  {
    natList = &(QcMapNatAlg->nat_config.port_fwding_entries);
    LOG_MSG_INFO1("Searching for NAT Entries in the list. ", 0, 0, 0);

    if ( QcMapNatAlg->nat_config.num_port_fwding_entries ==
                                              QCMAP_MSGR_MAX_SNAT_ENTRIES_V01 )
    {
      /* we reach MAX entries */
      LOG_MSG_ERROR("\nReached Max limit on SNAT Entries!!",0,0,0);
      *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
      return false;
    }

    if (natList->natEntryListHead)
    {

      node = ds_dll_next(natList->natEntryListHead,
                         (&curr_nat_entry));
      if (node == NULL)
      {
        LOG_MSG_ERROR("AddStaticNatEntry - NAT info is NULL"
                      "Cant fetch NAT info",0,0,0);
        return false;
      }
      for ( i=0; i < QcMapNatAlg->nat_config.num_port_fwding_entries; i++ )
      {
        if ((nat_entry->port_fwding_global_port == curr_nat_entry->port_fwding_global_port) &&
            (nat_entry->port_fwding_protocol == curr_nat_entry->port_fwding_protocol ))
        {
          /* we already added this one */
          LOG_MSG_ERROR("\nSNAT Entry already present!!",0,0,0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }
        node = ds_dll_next(node, (&curr_nat_entry));
      }
    }

    if(QcMapNatAlg->AddNATEntryToList(nat_entry) == false)
    {
      LOG_MSG_ERROR("Failed to Add NAT Entry to List", 0, 0, 0);
      return false;
    }

    if (nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP)
    {
      if (!QcMapNatAlg->AddSNATEntryOnApps(nat_entry, qmi_err_num))
      {
        LOG_MSG_ERROR("\nAdding SNAT entry on A5 Failed!!",0,0,0);
        return false;
      }
    }
    else
    {
      nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
      if (!QcMapNatAlg->AddSNATEntryOnApps(nat_entry, qmi_err_num))
      {
        LOG_MSG_ERROR("\nAdding SNAT entry on A5 Failed!!",0,0,0);
        return false;
      }
      nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;

      if(!QcMapNatAlg->AddSNATEntryOnApps(nat_entry, qmi_err_num))
      {
        LOG_MSG_ERROR("\nAdding SNAT entry on A5 Failed!!",0,0,0);
        return false;
      }
    }
  }
  /* If NAT is not enabled just add the entry to XML file. */
  if((QCMAP_NATALG::GetNATEntryCountFromXML(&xml_file)) >=
                                           QCMAP_MSGR_MAX_SNAT_ENTRIES_V01)
  {
    LOG_MSG_ERROR("Exceeds maximum number of NAT entries\n",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return QCMAP_MSGR_ENTRY_FULL;
  }

  if (QCMAP_NATALG::searchNATEntryInXML(&xml_file, nat_entry))
  {
    /* we already added this one */
    LOG_MSG_ERROR("\nSNAT Entry already present!!",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  LOG_MSG_INFO1("Adding configuration to XML file", 0, 0, 0);
  QCMAP_NATALG::addNATEntryToXML(&xml_file, nat_entry);

  return true;
}

/*==========================================================
  FUNCTION DeleteStaticNatEntry
==========================================================*/
/*!
@brief
  Deletes the snat entry on A5 and in mobileap configuration file.

@parameters
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

boolean QCMAP_NATALG::DeleteStaticNatEntry
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = NULL;
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  pugi::xml_document xml_file;
  pugi::xml_node parent, child ;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (QcMapNatAlg)
  {
    natList = &(QcMapNatAlg->nat_config.port_fwding_entries);

    LOG_MSG_INFO1( "DeleteStaticNatEntry - head: %x tail: %x nat: %d",
                   natList->natEntryListHead,
                   natList->natEntryListTail,
                   QcMapNatAlg->nat_config.num_port_fwding_entries);

    if ((node = ds_dll_delete(natList->natEntryListHead,
                               &(natList->natEntryListTail),
                               (void*)nat_entry,
                               qcmap_compare_nat_entries))== NULL)
    {
      LOG_MSG_ERROR("DeleteStaticNatEntry - Error in finding the node",0,0,0);
      return false;
    }

    //Free the memory for the device details structure
    curr_nat_entry = (qcmap_cm_port_fwding_entry_conf_t*)node->data;

    if (curr_nat_entry == NULL)
    {
      LOG_MSG_ERROR("DeleteStaticNatEntry - NAT Info is NULL", 0,0,0);
      //Free the memory of the linked list node
      ds_dll_free(node);
      node = NULL;
      return false;
    }

    //Free the device information structure
    ds_free (curr_nat_entry);
    curr_nat_entry = NULL;

    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;

    // Add logic to check if there are no more nodes in the list and free the head
    if (natList->natEntryListHead->next == NULL)
    {
      LOG_MSG_INFO1("\n No NAT entry exist\n",0,0,0);
      ds_dll_free(natList->natEntryListHead);
      natList->natEntryListHead = NULL;
    }

    QcMapNatAlg->nat_config.num_port_fwding_entries--;

    if ( nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP )
    {
      if (!QcMapNatAlg->DeleteSNATEntryOnApps(nat_entry, qmi_err_num))
        return false;
    }
    else
    {
      nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
      if (!QcMapNatAlg->DeleteSNATEntryOnApps(nat_entry, qmi_err_num))
        return false;

      nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;

      if (!QcMapNatAlg->DeleteSNATEntryOnApps(nat_entry, qmi_err_num))
        return false;
    }
  }

  /* If NAT is not enabled, just delete the entry from XML file and return. */
  if (child = QCMAP_NATALG::searchNATEntryInXML(&xml_file, nat_entry))
  {
    parent = child.parent();
    parent.remove_child(child);
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    LOG_MSG_INFO1("Deleted the NAT entry successfully.", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("DeleteStaticNatEntry - Error in finding the node",0,0,0);
    return false;
  }

  return true;
}

/*==========================================================
  FUNCTION addNATEntryToXML
==========================================================*/
/*!
@brief
  Add NAT Entry to the XML.

@parameters
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry
@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

int QCMAP_NATALG::addNATEntryToXML(pugi::xml_document *xml_file,
                                   qcmap_cm_port_fwding_entry_conf_t* nat_entry)
{

  struct in_addr addr;
  struct in6_addr ip6_addr;
  char data[MAX_STRING_LENGTH] = {0};
    char str[INET6_ADDRSTRLEN];
	uint32_t debug_string=0;

  pugi::xml_node root, child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  root = xml_file->child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);



 // add new NAT entry here.
  child = root.append_child(PortFwding_TAG);

  //IP Address
  subchild = child.append_child(PortFwdingPrivateIP_TAG);
  addr.s_addr = htonl(nat_entry->port_fwding_private_ip);
  subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

  //Port forwarding IP
  subchild = child.append_child(PortFwdingPrivatePort_TAG);
  snprintf(data, sizeof(data), "%d", nat_entry->port_fwding_private_port);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  // Global Port
  subchild = child.append_child(PortFwdingGlobalPort_TAG);
  snprintf(data, sizeof(data), "%d", nat_entry->port_fwding_global_port);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  // Protocol
  subchild = child.append_child(PortFwdingProtocol_TAG);
  snprintf(data, sizeof(data), "%d", nat_entry->port_fwding_protocol);
  subchild.append_child(pugi::node_pcdata).set_value(data);


  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, xml_file);

  debug_string=htonl(nat_entry->port_fwding_private_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nPortFwdingPrivateIP: '%s'",str,0,0);

  LOG_MSG_INFO1("PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d, PortFwdingProtocol: %d",
  nat_entry->port_fwding_private_port, nat_entry->port_fwding_global_port,nat_entry->port_fwding_protocol);
}

/*==========================================================
  FUNCTION GetStaticNATEntriesFromXML
==========================================================*/
/*!
@brief
  Get Static NAT Entry from the XML.

@parameters
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  unsigned int* num_entries

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

boolean QCMAP_NATALG::GetStaticNATEntriesFromXML
(
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  unsigned int* num_entries
)
{
  int count = 0;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  in_addr addr;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if ((!strcmp(child.name(),PortFwding_TAG)))
    {
      inet_aton(child.child_value(PortFwdingPrivateIP_TAG), &addr);
      snat_config[count].private_ip_addr = ntohl(addr.s_addr);

      snat_config[count].private_port = atoi(
                                    child.child_value(PortFwdingPrivatePort_TAG));

      snat_config[count].global_port = atoi(
                                     child.child_value(PortFwdingGlobalPort_TAG));

      snat_config[count].protocol = atoi(child.child_value(PortFwdingProtocol_TAG));

      count++;

      debug_string= htonl(snat_config[count].private_ip_addr );
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
      LOG_MSG_INFO1("\PortFwdingPrivateIP: '%s'",str,0,0);
      LOG_MSG_INFO1("PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d, PortFwdingProtocol: %d",snat_config[count].private_port,
                    snat_config[count].global_port,snat_config[count].protocol );

    }
  }

  *num_entries = count;
  return true;
}

/*==========================================================
  FUNCTION GetStaticNatEntries
==========================================================*/
/*!
@brief
  Gets all the snat entries configured.

@parameters
  qcmap_msgr_snat_entry_config_v01 *snat_config
  unsigned int                     *num_entries

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
boolean QCMAP_NATALG::GetStaticNatEntries
(
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  unsigned int* num_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i = 0;
  qcmap_nat_entry_list_t* natList = NULL;
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;
  ds_dll_el_t * node = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);
  pugi::xml_document xml_file;


  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(!QcMapNatAlg)
  {
     LOG_MSG_INFO1("Obtaining NAT Entries from XML. ", 0, 0, 0);
    QCMAP_NATALG::GetStaticNATEntriesFromXML(snat_config, num_entries);
    if (*num_entries <= 0)
    {
      LOG_MSG_ERROR("\nNo SNAT entries configured",0,0,0);
      *qmi_err_num = QMI_ERR_DISABLED_V01;
      return false;
    }
    return true;
  }

  natList = &(QcMapNatAlg->nat_config.port_fwding_entries);

  *num_entries = QcMapNatAlg->nat_config.num_port_fwding_entries;

  if (*num_entries <= 0)
  {
    LOG_MSG_ERROR("\nNo SNAT entries configured",0,0,0);
    *qmi_err_num = QMI_ERR_DISABLED_V01;
    return false;
  }

  if (natList->natEntryListHead)
  {
    node = natList->natEntryListHead;
    if (node == NULL)
    {
      LOG_MSG_ERROR("GetStaticNatEntries - NAT info is NULL"
                    "Cant fetch NAT info",0,0,0);
      return false;
    }
    node = ds_dll_next (node, (const void**)(&curr_nat_entry));

    for ( i=0; i < *num_entries && curr_nat_entry; i++ )
    {
      snat_config[i].private_ip_addr = curr_nat_entry->port_fwding_private_ip;
      snat_config[i].private_port = curr_nat_entry->port_fwding_private_port;
      snat_config[i].global_port = curr_nat_entry->port_fwding_global_port;
      snat_config[i].protocol = curr_nat_entry->port_fwding_protocol;
      node = ds_dll_next (node, (const void**)(&curr_nat_entry));
    }
  }
  return true;
}

/*==========================================================
  FUNCTION GetIpsecVpnPassthroughFlag
==========================================================*/
/*!
@brief
  Gets ipsec vpn pass through flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::GetSetPassthroughFlag
(
  qcmap_nat_config_enum pass_thru_type,
  qcmap_action_type action,
  uint8 *flag,
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  if (flag == NULL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (pass_thru_type < CONFIG_PPTP_VPN_PASSTHROUGH ||
      pass_thru_type > CONFIG_IPSEC_VPN_PASSTHROUGH)
  {
    LOG_MSG_ERROR(" Invalid pass through type. ", 0, 0, 0);
    return false;
  }

  if (action == SET_VALUE)
    snprintf(data, sizeof(data), "%d", *flag);


  switch (pass_thru_type)
  {
    case CONFIG_PPTP_VPN_PASSTHROUGH:
      if (action == SET_VALUE)
      {
        if (QcMapNatAlg)
        {
          if (!QcMapNatAlg->SetPPTPVpnPassThroughOnApps(*flag, qmi_err_num))
          {
            LOG_MSG_ERROR("Fail to set PPTP vpn passthrough", 0, 0, 0);
            return false;
          }
          QcMapNatAlg->nat_config.enable_pptp_vpn_pass_through = *flag;
        }
        QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_PPTP_VPN_PASSTHROUGH,
                                             SET_VALUE, data, MAX_STRING_LENGTH);
      }
      else
      {
        if (QcMapNatAlg)
        {
          *flag = QcMapNatAlg->nat_config.enable_pptp_vpn_pass_through;
        }
        else
        {
          QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_PPTP_VPN_PASSTHROUGH,
                                               GET_VALUE, data, MAX_STRING_LENGTH);
          *flag = atoi(data);
        }
      }
    break;
    case CONFIG_L2TP_VPN_PASSTHROUGH:
      if (action == SET_VALUE)
      {
        if (QcMapNatAlg)
        {
          if (!QcMapNatAlg->SetL2TPVpnPassThroughOnApps(*flag, qmi_err_num))
          {
            LOG_MSG_ERROR("Fail to set L2TP vpn passthrough", 0, 0, 0);
            return false;
          }
          QcMapNatAlg->nat_config.enable_l2tp_vpn_pass_through = *flag;
        }
        QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_L2TP_VPN_PASSTHROUGH,
                                             SET_VALUE, data, MAX_STRING_LENGTH);
      }
      else
      {
        if (QcMapNatAlg)
        {
          *flag = QcMapNatAlg->nat_config.enable_l2tp_vpn_pass_through;
        }
        else
        {
          QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_L2TP_VPN_PASSTHROUGH,
                                               GET_VALUE, data, MAX_STRING_LENGTH);
          *flag = atoi(data);
        }
      }
      break;

    case CONFIG_IPSEC_VPN_PASSTHROUGH:
      if (action == SET_VALUE)
      {
        if (QcMapNatAlg)
        {
          if (!QcMapNatAlg->SetIPSECVpnPassThroughOnApps(*flag, qmi_err_num))
          {
            LOG_MSG_ERROR("Fail to set IPSEC vpn passthrough", 0, 0, 0);
            return false;
          }
          QcMapNatAlg->nat_config.enable_ipsec_vpn_pass_through = *flag;
        }
        QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_IPSEC_VPN_PASSTHROUGH,
                                             SET_VALUE, data, MAX_STRING_LENGTH);
      }
      else
      {
        if (QcMapNatAlg)
        {
          *flag = QcMapNatAlg->nat_config.enable_ipsec_vpn_pass_through;
        }
        else
        {
          QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_IPSEC_VPN_PASSTHROUGH,
                                               GET_VALUE, data, MAX_STRING_LENGTH);
          *flag = atoi(data);
        }
      }
    break;
  }
  return true;
}


/*==========================================================
  FUNCTION SetIPSECVpnPassThroughOnApps
==========================================================*/
/*!
@brief
  Sets ipsec vpn pass through.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetIPSECVpnPassThroughOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if ( !QCMAP_ConnectionManager::GetDeviceName\
       (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

   /* Delete the existing rule, only if it was added .*/
  if (!QcMapNatAlg->nat_config.enable_ipsec_vpn_pass_through)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -p esp -i %s -j %s", devname, "DROP" );

    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disable rule, by default it's accept*/
  if( !enable )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -p esp -i %s -j %s", devname, "DROP" );

    ds_system_call(command, strlen(command));
  }

  return true;
}



/*==========================================================
  FUNCTION SetPPTPVpnPassThroughOnApps
==========================================================*/
/*!
@brief
  Configure the PPTP VPN pass through flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetPPTPVpnPassThroughOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if (!QCMAP_ConnectionManager::GetDeviceName\
      (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

   /* Delete the existing rule, only if it was added .*/
  if (!QcMapNatAlg->nat_config.enable_pptp_vpn_pass_through)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -p gre -i %s -j %s", devname,"DROP" );

    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disabel rule, by default it's accept*/
  if( !enable )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -p gre -i %s -j %s", devname,"DROP" );

    ds_system_call(command, strlen(command));
    LOG_MSG_INFO1("%s\n", command,0,0);
  }

  return true;
}

/*==========================================================
  FUNCTION SetL2TPVpnPassThroughOnApps
==========================================================*/
/*!
@brief
  Configures the L2TP VPN pass through flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetL2TPVpnPassThroughOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

    if ( !QCMAP_ConnectionManager::GetDeviceName\
         (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

   /* Delete the existing rule, only if it was added .*/
  if( !QcMapNatAlg->nat_config.enable_l2tp_vpn_pass_through )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -p esp -i %s -j %s", devname,"DROP" );

    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disable rule, by default it's accept*/
  if( !enable )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -p esp -i %s -j %s", devname,"DROP" );

    ds_system_call(command, strlen(command));
  }

  return true;
}

/*==========================================================
  FUNCTION GetNatType
==========================================================*/
/*!
@brief
  Gets configured value of L2TP VPN passthrough flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::GetNatType
(
  qcmap_msgr_nat_enum_v01 *cur_nat_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  if ( cur_nat_type == NULL )
  {
    *qmi_err_num = QMI_ERR_DISABLED_V01;
    return false;
  }

  if(QcMapNatAlg)
  {
    *cur_nat_type = QcMapNatAlg->nat_config.nat_type;
  }
  else
  {
    QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_TYPE, GET_VALUE, data,
                                         MAX_STRING_LENGTH);
    if (strncasecmp(PRC_TAG, data, strlen(PRC_TAG)) == 0)
    {
      *cur_nat_type = QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01;
    }
    else if (strncasecmp(FULLCONE_TAG, data, strlen(FULLCONE_TAG)) == 0)
    {
      *cur_nat_type = QCMAP_MSGR_NAT_FULL_CONE_NAT_V01;
    }
    else if (strncasecmp(ARC_TAG, data, strlen(ARC_TAG)) == 0)
    {
      *cur_nat_type = QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01;
    }
    else
    {
      /* Default SYMMETRIC NAT. */
      *cur_nat_type = QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01;
    }
  }
  return true;
}

/*==========================================================
  FUNCTION ReadNATConfigFromXML
==========================================================*/
/*!
@brief
  Reads the NAT Config from the XML

@parameters
  char *xml_path

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

boolean QCMAP_NATALG::ReadNATConfigFromXML(char *xml_path)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child, subchild;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string =0;
  qcmap_cm_port_fwding_entry_conf_t nat_entry;

  if (!xml_file.load_file(xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  this->nat_config.nat_entry_generic_timeout =
                      atoi(root.child(NatEntryGenericTimeout_TAG).child_value());

  this->nat_config.nat_entry_icmp_timeout =
                         atoi(root.child(NatEntryICMPTimeout_TAG).child_value());

  this->nat_config.nat_entry_tcp_established_timeout =
               atoi(root.child(NatEntryTCPEstablishedTimeout_TAG).child_value());

  this->nat_config.nat_entry_udp_timeout =
                          atoi(root.child(NatEntryUDPTimeout_TAG).child_value());

  this->nat_config.dmz_ip = inet_addr(root.child(DmzIP_TAG).child_value());
  this->nat_config.dmz_ip = ntohl(this->nat_config.dmz_ip);

  this->nat_config.enable_ipsec_vpn_pass_through =
                   atoi(root.child(EnableIPSECVpnPassthrough_TAG).child_value());

  this->nat_config.enable_pptp_vpn_pass_through =
                     atoi(root.child(EnablePPTPVpnPassthrough_TAG).child_value());

  this->nat_config.enable_l2tp_vpn_pass_through =
                    atoi(root.child(EnableL2TPVpnPassthrough_TAG).child_value());

  if (atoi(root.child(ALGCfg_TAG).child(EnableRTSPAlg_TAG).child_value()) == 1)
    this->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_RTSP_ALG_V01;
  else
    this->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_RTSP_ALG_V01;

  if (atoi(root.child(ALGCfg_TAG).child(EnableSIPAlg_TAG).child_value()) == 1)
    this->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
  else
    this->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_SIP_ALG_V01;
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), PortFwding_TAG))
    {
      subchild=child;
      inet_aton(subchild.child(PortFwdingPrivateIP_TAG).child_value(),
                (in_addr *)&(nat_entry.port_fwding_private_ip));
      nat_entry.port_fwding_private_ip =
                                   ntohl(nat_entry.port_fwding_private_ip);
      nat_entry.port_fwding_private_port =
                        atoi(subchild.child(PortFwdingPrivatePort_TAG).child_value());

      nat_entry.port_fwding_global_port =
                         atoi(subchild.child(PortFwdingGlobalPort_TAG).child_value());

      nat_entry.port_fwding_protocol =
                           atoi(subchild.child(PortFwdingProtocol_TAG).child_value());

      debug_string= htonl(nat_entry.port_fwding_private_ip);
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);

      LOG_MSG_INFO1( "PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d, PortFwdingProtocol: %d",
                     nat_entry.port_fwding_private_port,
                     nat_entry.port_fwding_global_port,
                     nat_entry.port_fwding_protocol);

      if(this->AddNATEntryToList(&nat_entry) == false)
      {
        LOG_MSG_ERROR("Failed to Add NAT Entry to List", 0, 0, 0);
        return false;
      }
    }
  }

   if (strncasecmp(PRC_TAG, root.child(NATType_TAG).child_value(),
                   strlen(PRC_TAG)) == 0)
   {
     this->nat_config.nat_type = QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01;
   }
   else if (strncasecmp(FULLCONE_TAG,
                        root.child(NATType_TAG).child_value(),
                        strlen(FULLCONE_TAG)) == 0)
   {
     this->nat_config.nat_type = QCMAP_MSGR_NAT_FULL_CONE_NAT_V01;
   }
   else if (strncasecmp(ARC_TAG, root.child(NATType_TAG).child_value(),
                        strlen(ARC_TAG)) == 0)
   {
     this->nat_config.nat_type = QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01;
   }
   else
   {
     /* Default SYMMETRIC NAT. */
     this->nat_config.nat_type = QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01;
   }

  return true;
}


/*==========================================================
  FUNCTION SetNatType
==========================================================*/
/*!
@brief
  Configures the nat type to be used and enables the same.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetNatType
(
  qcmap_msgr_nat_enum_v01 nat_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int ret;

  QCMAP_Firewall* QcMapFirewall = QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg = QCMAP_NATALG::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr = QCMAP_Backhaul::Get_Instance(false);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  ret = QCMAP_ConnectionManager::GetDeviceName\
        (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num);

  if (nat_type < QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01 ||
       nat_type > QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01)
  {
    LOG_MSG_ERROR("Error: NAT type not supported.\n",0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if(QcMapNatAlg)
  {
    QcMapNatAlg->DisableNATonApps();
    if (QcMapFirewall)
      QcMapFirewall->CleanIPv4MangleTable();
    QcMapNatAlg->nat_config.nat_type = nat_type;

    if ( ( ret == BACKHAUL_TYPE_AP_STA_ROUTER) || (ret == BACKHAUL_TYPE_CRADLE) ||
         ( ret == BACKHAUL_TYPE_ETHERNET ))
    {
      if (QcMapBackhaulMgr)
        QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V4, devname);
    }
    QcMapNatAlg->EnableNATonApps();
  }

  switch(nat_type)
  {
    case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_TYPE,
                                           SET_VALUE, PRC_TAG,
                                           strlen(PRC_TAG));
    break;

    case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_TYPE,
                                           SET_VALUE, FULLCONE_TAG,
                                           strlen(FULLCONE_TAG));
    break;

    case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_TYPE,
                                           SET_VALUE, ARC_TAG,
                                           strlen(ARC_TAG));
    break;

    case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
      QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_NAT_TYPE,
                                           SET_VALUE, SYMMETRIC_TAG,
                                           strlen(SYMMETRIC_TAG));
    break;

    default:
      LOG_MSG_INFO1("Nat Type:%d not supported.\n", nat_type,0,0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    break;

  }

  return true;
}

/*==========================================================
  FUNCTION searchNATEntryInXML
==========================================================*/
/*!
@brief
  Searches the NAT Config in the XML

@parameters
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

pugi::xml_node QCMAP_NATALG::searchNATEntryInXML
(
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry
)
{
  pugi::xml_node root, child;
  qcmap_msgr_firewall_entry_conf_t firewall_conf;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;
  root = xml_file->child("system").child("MobileAPCfg").child("MobileAPNatCfg");

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(),PortFwding_TAG))
    {
      if(((atoi(child.child_value(PortFwdingGlobalPort_TAG)) ==
                                        nat_entry->port_fwding_global_port)) &&
          ((atoi(child.child_value(PortFwdingProtocol_TAG)) ==
                                             nat_entry->port_fwding_protocol)))
      {
        LOG_MSG_INFO1("Existing NAT Entry found", 0, 0, 0);
        return child;
      }
    }
  }

  debug_string= htonl(nat_entry->port_fwding_private_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\PortFwdingPrivateIP: '%s'",str,0,0);

  LOG_MSG_INFO1("PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d,\
                 PortFwdingProtocol: %d",nat_entry->port_fwding_private_port,\
                 nat_entry->port_fwding_global_port,nat_entry->port_fwding_protocol);

  return child;
}

/*==========================================================
  FUNCTION GetSetNATConfigFromXML
==========================================================*/
/*!
@brief
  Gets the NAT Config from the XML

@parameters
  qcmap_nat_config_enum conf,
  qcmap_action_type action,
  char *data,
  int data_len

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

boolean QCMAP_NATALG::GetSetNATConfigFromXML
(
  qcmap_nat_config_enum conf,
  qcmap_action_type action,
  char *data,
  int data_len
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  const char *tag_ptr;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);


  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  if (conf == CONFIG_RTSP_ALG)
  {
    if(action == SET_VALUE)
    {
      root.child(ALGCfg_TAG).child(EnableRTSPAlg_TAG).text() = data;
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    }
    else
    {
      strlcpy(data, root.child(ALGCfg_TAG).child(EnableRTSPAlg_TAG).child_value(),
              data_len);
    }
    return TRUE;
  }
  else if (conf == CONFIG_SIP_ALG)
  {
    if (action == SET_VALUE)
    {
      root.child(ALGCfg_TAG).child(EnableSIPAlg_TAG).text() = data;
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    }
    else
    {
      strlcpy(data, root.child(ALGCfg_TAG).child(EnableSIPAlg_TAG).child_value(),
              data_len);
    }
    return TRUE;
  }

  switch(conf)
  {
    case CONFIG_NAT_ENTRY_GENERIC_TIMEOUT:
      tag_ptr = NatEntryGenericTimeout_TAG;
    break;

    case CONFIG_NAT_ENTRY_ICMP_TIMEOUT:
      tag_ptr = NatEntryICMPTimeout_TAG;
    break;

    case CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT:
      tag_ptr = NatEntryTCPEstablishedTimeout_TAG;
    break;

    case CONFIG_NAT_ENTRY_UDP_TIMEOUT:
      tag_ptr = NatEntryUDPTimeout_TAG;
    break;

    case CONFIG_DMZ_IP:
        tag_ptr = DmzIP_TAG;
    break;

    case CONFIG_PPTP_VPN_PASSTHROUGH:
        tag_ptr = EnablePPTPVpnPassthrough_TAG;
    break;

    case CONFIG_L2TP_VPN_PASSTHROUGH:
        tag_ptr = EnableL2TPVpnPassthrough_TAG;
    break;

    case CONFIG_IPSEC_VPN_PASSTHROUGH:
        tag_ptr = EnableIPSECVpnPassthrough_TAG;
    break;


    case CONFIG_NAT_TYPE:
      tag_ptr = NATType_TAG;
    break;

    default:
      LOG_MSG_ERROR("Invalid Config type to set.", 0, 0, 0);
      return FALSE;
    break;
  }

  //Set/Get the value based on action.
  if (action == SET_VALUE)
  {
    root.child(tag_ptr).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
    strlcpy(data, root.child(tag_ptr).child_value(), data_len);

  LOG_MSG_INFO1("Config: %d Action: %d value: %d" , conf, action, atoi(data));

  return TRUE;
}

/*============================================================
  FUNCTION EnableMiscNATTasks
==========================================================================*/
/*!
@brief
  Adds SNAT, DMZ, VPN passthrough and firewall configurations.

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
boolean QCMAP_NATALG::EnableMiscNATTasks()
{
  int i = 0;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  ds_dll_el_t * node = NULL;

  qcmap_msgr_ip_family_enum_v01 interface;
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  qcmap_nat_entry_list_t* natList = &(QcMapNatAlg->nat_config.port_fwding_entries);
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  qcmap_firewall_entry_list_t* firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);
  qcmap_msgr_firewall_entry_conf_t *firewall_entry = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();

  node = natList->natEntryListHead;
  /* Config SNAT */
  if (node)
  {
    node = ds_dll_next (node, (const void**)(&curr_nat_entry));
    if (node == NULL)
    {
      LOG_MSG_ERROR("EnableMiscNATTasks - NAT info is NULL"
                    "Cant fetch NAT info",0,0,0);
      return false;
    }
    for ( i=0; i < QcMapNatAlg->nat_config.num_port_fwding_entries && curr_nat_entry; i++ )
    {
      if (curr_nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP )
      {
        AddSNATEntryOnApps(curr_nat_entry, &qmi_err_num);
      }
      else
      {
        curr_nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
        AddSNATEntryOnApps(curr_nat_entry, &qmi_err_num);
        curr_nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;
        AddSNATEntryOnApps(curr_nat_entry, &qmi_err_num);
        curr_nat_entry->port_fwding_protocol = PS_IPPROTO_TCP_UDP;
      }
      //node = node->next;
      node = ds_dll_next (node, (const void**)(&curr_nat_entry));
    }
  }

  /* Config DMZ IP.*/
  if ( QcMapNatAlg->nat_config.dmz_ip != 0 )
  {
    QcMapNatAlg->AddDMZOnApps(QcMapNatAlg->nat_config.dmz_ip, &qmi_err_num);
  }

   // Adding default firewall rules
   interface = QCMAP_MSGR_IP_FAMILY_V4_V01;
   if (!QcMapFirewall->SetDefaultFirewallRule(interface)) {
      LOG_MSG_ERROR("Default firewall rules not added for IPV4",0,0,0);
   }

  // Adding Firewall rules for IPV4
  if (QcMapFirewall->firewall_config.firewall_enabled)
  {
    node = firewallList->firewallEntryListHead;
    if (node)
    {
      node = ds_dll_next (node, (const void**)(&firewall_entry));
      if (node == NULL)
      {
        LOG_MSG_ERROR("AddStaticNatEntry - NAT info is NULL"
                      "Cant fetch NAT info",0,0,0);
        return false;
      }
    }
    for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries; i++) {
       if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V4 ) {
           QcMapFirewall->SetFirewallV4(firewall_entry, true, &qmi_err_num);
       }
       //node = node->next;
       node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }

  /* Configure VPN PassThrough. */
  QcMapNatAlg->SetIPSECVpnPassThroughOnApps(this->nat_config.enable_ipsec_vpn_pass_through, &qmi_err_num);
  QcMapNatAlg->SetL2TPVpnPassThroughOnApps(this->nat_config.enable_l2tp_vpn_pass_through, &qmi_err_num);
  QcMapNatAlg->SetPPTPVpnPassThroughOnApps(this->nat_config.enable_pptp_vpn_pass_through, &qmi_err_num);

  /* Configure Webserver WWAN Access. */
  if(QcMapBackhaulMgr)
  {
    QcMapBackhaulMgr->SetWebserverWWANAccessOnApps(QcMapBackhaulMgr->wan_cfg.enable_webserver_wwan_access, &qmi_err_num);
  }
  else
  {
    LOG_MSG_ERROR("Backhaul object not enabled",0,0,0)
  }
  /* Enable ALGs from XML file */
  if ( QcMapNatAlg->nat_config.enable_alg_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01 )
  {
    LOG_MSG_INFO1("QCMAP_NATALG::EnableRTSPAlg() Called",0,0,0)
    ret = QcMapNatAlg->EnableRTSPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to enable RTSP ALG, Ret value = %d error: %d.\n",
                     ret, qmi_err_num, 0);
    }
  }

  /*SIP Alg is enabled by default in kernel. So we to enable or disable based on
    the config explicitly*/
  if ( QcMapNatAlg->nat_config.enable_alg_mask & QCMAP_MSGR_MASK_SIP_ALG_V01 )
  {
    LOG_MSG_INFO1("QCMAP_NATALG::EnableSIPAlg() Called",0,0,0)
    ret = QcMapNatAlg->EnableSIPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to enable SIP ALG, Ret value = %d error: %d.\n",
                     ret, qmi_err_num, 0);
    }
  }
  else
  {
    LOG_MSG_INFO1("QCMAP_NATALG::DisableSIPAlg() Called",0,0,0)
    ret = QcMapNatAlg->DisableSIPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to Disable SIP ALG, Ret value%d Error: %d\n",
                     ret, qmi_err_num, 0);
    }
  }
}


/*===========================================================================
  FUNCTION EnableNATonApps
==========================================================================*/
/*!
@brief
  Enables NAT on A5 based on the NAT type configured.

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
boolean QCMAP_NATALG::EnableNATonApps()
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char gwIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE], subnetIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netMaskIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  int qcmap_cm_error;
  in_addr_t public_ip = 0, sec_dns_addr = 0;
  in_addr_t default_gw_addr = 0, net_mask = 0, public_ip_wwan = 0;
  uint32 pri_dns_addr = 0;
  qmi_error_type_v01 qmi_err_num;
  int ret;
  char enable[MAX_STRING_LENGTH];
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall = QCMAP_Firewall::Get_Instance(false);
/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  /* Check for STA mode, before we start setting up NAT on A5 */
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(publicIpAddr, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gwIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(subnetIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(netMaskIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num);
  if(ret == BACKHAUL_TYPE_WWAN)
  {
      if( QCMAP_CM_SUCCESS !=
        qcmap_cm_get_ipv4_net_conf(QcMapMgr->qcmap_cm_handle,
                                   &public_ip, &pri_dns_addr, &sec_dns_addr,
                                   &default_gw_addr, &qmi_err_num) )
    {
      LOG_MSG_ERROR("Couldn't get public ip address. error %d\n", qmi_err_num,0,0);
      return false;
    }

    if ( false == QcMapBackhaulMgr->GetIP(&public_ip_wwan, &net_mask, devname) )
    {
      LOG_MSG_ERROR("Unable to get the IP address",0,0,0);
      return false;
    }
  }
  else if(ret > BACKHAUL_TYPE_WWAN)
  {
    if ( false == QcMapBackhaulMgr->GetIP(&public_ip, &net_mask, devname) )
    {
      LOG_MSG_ERROR("Unable to get the IP address",0,0,0);
      return false;
    }
    if ( false == QcMapMgr->GetGatewayIP(&default_gw_addr) )
    {
      LOG_MSG_ERROR("Unable to get the Gateway IP",0,0,0);
      /* continue by adding just the default route. */
    }
  }
  else
  {
    LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
    return false;
  }

  addr.s_addr = public_ip;
  strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = default_gw_addr;
  strlcpy(gwIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = public_ip & net_mask;
  strlcpy(subnetIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = net_mask;
  strlcpy(netMaskIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = public_ip & net_mask;
  strlcpy(subnetIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = net_mask;
  strlcpy(netMaskIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  if(QcMapBackhaulMgr)
  {
    /* Store ipv4 address info. */
    QcMapBackhaulMgr->ipv4_public_ip = public_ip;
    QcMapBackhaulMgr->ipv4_default_gw_addr = default_gw_addr;
    bzero(QcMapBackhaulMgr->wan_cfg.ipv4_interface,QCMAP_MSGR_INTF_LEN);
    strlcpy(QcMapBackhaulMgr->wan_cfg.ipv4_interface,devname,QCMAP_MSGR_INTF_LEN);
  }
  /* Delete the default route to backhaul. */
  #ifndef FEATURE_QTIMAP_OFFTARGET
    snprintf( command, MAX_COMMAND_STR_LEN, "route del default dev %s", devname);
    ds_system_call(command, strlen(command));

    /* Delete the subnet base route */
    snprintf( command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev %s",\
              subnetIP, netMaskIP, devname);
    ds_system_call(command, strlen(command));
  #endif
  /* Clear the existing connection track entries. */
  /* Delete the conntrack connections based on source NAT and destination NAT flags. */
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -n");
  ds_system_call(command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -g");
  ds_system_call(command, strlen(command));

  /*-----------------------------------------------------------------------
    To allow embedded call on same PDN, traffic from MobileAP clients should
    not be forwarded to WWAN interface. So we install a firewall rule in
    DisableNATonApps to not to forward traffic from bridge interface to WWAN
    interface. So if we flush the rules here even that entry will be deleted.
    To avoid it we do the flushing only when in WWANmode. When we are swithing
    between WWAN and station mode, we anyway first flush all the existing
    iptables rules in DisableNATonApps. So not doing it for station mode should
    affect anything.
  -----------------------------------------------------------------------*/
  if (ret == BACKHAUL_TYPE_WWAN)
  {
    QcMapMgr->CleanIPv4FilterTableFwdChain();
  }
  /* Flush Mangle table */
  if (QcMapFirewall)
    QcMapFirewall->CleanIPv4MangleTable();
  QcMapMgr->CleanIPv4NatTable();

   /* Check for enable ipv4 must be here to remove the default routes which
     DHCPCD adds */
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,GET_VALUE,\
                                              enable,MAX_STRING_LENGTH);
  if ( !((boolean)atoi(enable)) )
  {
    LOG_MSG_ERROR("Error: IPV4 not enabled. Not continuing further\n",0,0,0);
    return false;
  }

  /*-----------------------------------------------------------------------
    Set up forwarding and masquerading.
  -----------------------------------------------------------------------*/
  switch (QcMapNatAlg->nat_config.nat_type)
  {
    case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
      {
         snprintf( command, MAX_COMMAND_STR_LEN,
                   "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE --random", devname );
         ds_system_call(command, strlen(command));
         snprintf( command, MAX_COMMAND_STR_LEN,
                   "iptables -t nat -A PREROUTING -i %s -j NATTYPE --mode dnat --type 1", devname );
         ds_system_call(command, strlen(command));
         /*
              * When UL data path is through IPA, only one packet traverses
              * through SW. NATTYPE module requires at least 2 packets to create
              * a valid NATTYPE entry. As the NATTYPE entry is not created, DL
              * data fails in ARCN and FCN. Move NATTYPE forward mode to
              * POSTROUTING chain to create the NATTYPE entry with only one packet.
              */
         snprintf( command, MAX_COMMAND_STR_LEN,
                   "iptables -t nat -A POSTROUTING -o %s -j NATTYPE --mode forward_out"
                   " --type 1", devname );
         ds_system_call(command, strlen(command));
         break;
       }
     case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
       {
         snprintf( command, MAX_COMMAND_STR_LEN,
                   "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE --random",
                   devname );
         ds_system_call(command, strlen(command));
         break;
       }
     case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
       {
         snprintf( command, MAX_COMMAND_STR_LEN,
                   "iptables -t nat -A POSTROUTING -o %s -j SNAT --to-source %s",
                   devname, publicIpAddr);
         ds_system_call(command, strlen(command));
         break;
        }
     case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE --random",
                 devname );
       ds_system_call(command, strlen(command));
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -A PREROUTING -i %s -j NATTYPE --mode dnat"
                 " --type 2", devname );
       ds_system_call(command, strlen(command));
       /*
       * When UL data path is through IPA, only one packet traverses
       * through SW. NATTYPE module requires at least 2 packets to create
       * a valid NATTYPE entry. As the NATTYPE entry is not created, DL
       * data fails in ARCN and FCN. Move NATTYPE forward mode to
       * POSTROUTING chain to create the NATTYPE entry with only one packet.
       */
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -A POSTROUTING -o %s -j NATTYPE --mode forward_out"
                 " --type 2", devname );
       ds_system_call(command, strlen(command));
       break;
     }
  }


  /*-----------------------------------------------------------------------
    Enable packet forwarding
  -----------------------------------------------------------------------*/
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv4/ip_forward");
  ds_system_call(command, strlen(command));

  /*-----------------------------------------------------------------------
    Enable ARP Proxy on WAN interface
  -----------------------------------------------------------------------*/
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv4/conf/%s/proxy_arp", devname );
  ds_system_call(command, strlen(command));

  LOG_MSG_INFO1(" NAT Enabled.\n",0,0,0);

  /* Drop unstatefull tcp packets */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -t filter -A FORWARD -i %s -p tcp -m state --state INVALID -j DROP",
            BRIDGE_IFACE );
  ds_system_call(command, strlen(command));

  /*-----------------------------------------------------------------------
    Install the subnet route..
  -----------------------------------------------------------------------*/
  snprintf( command, MAX_COMMAND_STR_LEN, "route add -net %s netmask %s dev %s",\
            subnetIP, netMaskIP, devname);
  ds_system_call(command, strlen(command));

  /*-----------------------------------------------------------------------
    Install the default route..
  -----------------------------------------------------------------------*/
  #ifndef FEATURE_QTIMAP_OFFTARGET
    if ( default_gw_addr != 0 )
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "route add default gw %s dev %s",
                gwIP ,devname);
    }
    else
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "route add default dev %s",
                devname);
    }
    ds_system_call(command, strlen(command));
  #endif
  QcMapNatAlg->EnableMiscNATTasks();
  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) &&
       (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile
        == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) )
  {
    if(QcMapBackhaulMgr)
    addr.s_addr = QcMapBackhaulMgr->ipv4_public_ip;
    strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4\
              --ip-source %s -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4\
              --ip-source %s -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4\
              --ip-destination %s -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 \
              --ip-destination %s -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 \
              -j DROP \n",QcMapMgr->ap_dev_num2);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4\
              -j DROP \n",QcMapMgr->ap_dev_num2);
    ds_system_call(command, strlen(command));
  }

   if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if(QcMapBackhaulWWANMgr && (QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable\
       == DDNS_ENABLED) && (qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED) )
    {
      QcMapBackhaulWWANMgr->StartDDNS(&qmi_err_num);
    }
  }

  if(QcMapBackhaulMgr && (QcMapBackhaulMgr->tiny_proxy_enable_state == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01))
  {
    QCMAP_Backhaul_WWAN::EnableTinyProxy(&qmi_err_num);
  }

  return true;
}

/*===========================================================================
  FUNCTION DisableNATonApps
==========================================================================*/
/*!
@brief
  This function will delete NAT entries on A5.

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
boolean QCMAP_NATALG::DisableNATonApps()
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int i = 0;
  boolean ret = false;
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnetIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char gwIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  in_addr_t public_ip = 0, sec_dns_addr = 0, default_gw_addr = 0, net_mask = 0;
  uint32 pri_dns_addr = 0;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN *QcMapLANMgr = QCMAP_LAN::Get_Instance(false);
  qcmap_msgr_device_type_enum_v01 dev_type ;

/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Cant continue if SoftAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  if ( !QcMapBackhaulMgr->enable_ipv4 )
  {
    LOG_MSG_ERROR("Error: IPV4 not enabled.\n",0,0,0);
    return false;
  }
  /*-----------------------------------------------------------------------
    Delete iptables entries and flush tables.
    -----------------------------------------------------------------------*/
  QcMapMgr->CleanIPv4FilterTableFwdChain();
  QcMapMgr->CleanIPv4FilterTableInChain();
  QcMapMgr->CleanIPv4NatTable();


  if(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
  {
    dev_type = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.device_type;
    QcMapLANMgr->DisableIPPassthrough(false);
    QcMapLANMgr->RestartTetheredClient(dev_type);
  }

  /* Install the rule to block WWAN access. */

  QcMapBackhaulMgr->BlockIPv4WWANAccess();

  /*-----------------------------------------------------------------------
    Disable ALGs with enable bit set in XML.
  -----------------------------------------------------------------------*/
  if (QcMapNatAlg->nat_config.enable_alg_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01)
  {
    LOG_MSG_INFO1("QCMAP_NATALG::DisableRTSPAlg() Called",0,0,0)
    ret = DisableRTSPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to Disable RTSP ALG, Ret value%d Error: %d\n",
                     ret, qmi_err_num, 0);
    }
  }

  if (QcMapNatAlg->nat_config.enable_alg_mask & QCMAP_MSGR_MASK_SIP_ALG_V01)
  {
    LOG_MSG_INFO1("QCMAP_NATALG::DisableSIPAlg() Called",0,0,0)
    ret = DisableSIPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to Disable SIP ALG, Ret value%d Error: %d\n",
                     ret, qmi_err_num, 0);
    }
  }

  /* Check for STA mode, before we start setting up NAT on A5 */
  memset(publicIpAddr, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gwIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  /* Gateway address of a Connected Interface, set in function EnableNATonApps */
  if(QcMapBackhaulMgr->ipv4_default_gw_addr != 0)
  {
    addr.s_addr = QcMapBackhaulMgr->ipv4_default_gw_addr;
    strlcpy(gwIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    /* First delete the default route. */
  #ifndef FEATURE_QTIMAP_OFFTARGET
    snprintf( command, MAX_COMMAND_STR_LEN, "route del default gw %s", gwIP);
    ds_system_call(command, strlen(command));
  #endif
  }
  else
  {
  #ifndef FEATURE_QTIMAP_OFFTARGET
    snprintf( command, MAX_COMMAND_STR_LEN, "route del default gw 0.0.0.0");
    ds_system_call(command, strlen(command));
  #endif
  }

  ret = QCMAP_ConnectionManager::GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num);
  if (ret)
  {
    /* Delete the default route to backhaul. */
    snprintf( command, MAX_COMMAND_STR_LEN, "route del default dev %s", devname);
    ds_system_call(command, strlen(command));
  }

  /* Delete the conntrack connections based on source NAT and destination NAT flags. */
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -n");
  ds_system_call(command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -g");
  ds_system_call(command, strlen(command));
  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) &&
       (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile
        == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) )
  {
    addr.s_addr = QcMapBackhaulMgr->ipv4_public_ip;
    strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 --ip-source %s -j DROP \n",
             QcMapMgr->ap_dev_num2, publicIpAddr);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 --ip-destination %s -j DROP \n",
             QcMapMgr->ap_dev_num2, publicIpAddr);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 -j DROP \n",
              QcMapMgr->ap_dev_num2);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 -j DROP \n",
             QcMapMgr->ap_dev_num2);
    ds_system_call(command, strlen(command));
  }

  /* Clear the ipv4 address info since disconnecting and the same
     will be reset in EnableNATonApps*/
  QcMapBackhaulMgr->ipv4_public_ip = 0;
  QcMapBackhaulMgr->ipv4_default_gw_addr = 0;

  if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if(QcMapBackhaulWWANMgr &&
       QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable == DDNS_ENABLED &&
       qcmap_cm_get_state(&qmi_err_num) != QCMAP_CM_WAN_CONNECTED
       && qcmap_cm_get_ipv6_state(&qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED)
    {
      QcMapBackhaulWWANMgr->StopDDNS();
    }

    if (qcmap_cm_get_state(&qmi_err_num) != QCMAP_CM_WAN_CONNECTED)
    {
      /* Cleanup resolv.con */
      ds_system_call(">/etc/resolv.conf", strlen(">/etc/resolv.conf"));
    }
  }
  if(QcMapBackhaulMgr->tiny_proxy_enable_state == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
  {
    QCMAP_Backhaul_WWAN::StopTinyProxy(&qmi_err_num);
  }
  return true;
}


/*===========================================================================
  FUNCTION GetNatTimeoutOnApps
==========================================================================*/
/*!
@brief
  Get the NAT timeout value for the requested nat type.

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          *timeout_value

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
boolean QCMAP_NATALG::GetNatTimeoutOnApps
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 *timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  FILE *fp = NULL;

  if ( timeout_value == NULL )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  switch ( timeout_type )
  {
     case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
       fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_generic_timeout", "r");
       break;
     case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
       fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_icmp_timeout", "r");
       break;
     case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
       fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established", "r");
       break;
     case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
       fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout", "r");
       break;
     default:
       LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
       *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
       return false;
  }

  if ( fp == NULL )
  {
     LOG_MSG_ERROR("Error opening timeout file: %d.\n", timeout_type,0,0);
     *qmi_err_num = QMI_ERR_INTERNAL_V01;
     return false;
  }

  if (fscanf(fp, "%d", timeout_value) != 1) {
    LOG_MSG_ERROR("Error reading timeout file: %d.\n", timeout_type,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  LOG_MSG_INFO1("Timeout Type: %d Timeout Value: %d.\n", timeout_type, *timeout_value,0);

  fclose(fp);

  return true;
}

/*===========================================================================
  FUNCTION AddNATEntryToList
==========================================================================*/
/*!
@brief
  Adds the NAT entry to the List.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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


boolean QCMAP_NATALG::AddNATEntryToList(qcmap_cm_port_fwding_entry_conf_t *nat_entry)
{


  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = &(this->nat_config.port_fwding_entries);
  qcmap_cm_port_fwding_entry_conf_t *new_nat_entry;

  if (natList->natEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
            information. This is done to make use of the doubly linked list framework which
            is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddNATEntryToList - Error in allocating memory for node",
                    0,0,0);
      return false;
    }
    natList->natEntryListHead = node;
  }

  new_nat_entry = (qcmap_cm_port_fwding_entry_conf_t*)ds_malloc(sizeof(qcmap_cm_port_fwding_entry_conf_t));

  if( new_nat_entry == NULL )
  {
    LOG_MSG_ERROR("AddNATEntryToList - Error in allocating memory for"
                  "new NAT entry",0,0,0);
    return false;
  }

  memset(new_nat_entry, 0, sizeof(qcmap_cm_port_fwding_entry_conf_t));
  /* save into the config */
  memcpy(new_nat_entry, nat_entry, sizeof(qcmap_cm_port_fwding_entry_conf_t));

  //Enque the node
  if ((node = ds_dll_enq(natList->natEntryListHead,
                          NULL, (void*)new_nat_entry )) == NULL)
  {
    LOG_MSG_ERROR("AddStaticNatEntry - Error in adding a node",0,0,0);
    ds_free(new_nat_entry);
    new_nat_entry = NULL;
    return false;
  }
  natList->natEntryListTail = node;

  this->nat_config.num_port_fwding_entries++;
  return true;
}

/*===========================================================================
  FUNCTION SetNatTimeoutOnApps
==========================================================================*/
/*!
@brief
  Will set the NAT timeout value for the identified nat type.

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          timeout_value

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
boolean QCMAP_NATALG::SetNatTimeoutOnApps
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];

  switch ( timeout_type )
  {
     case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
       snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_generic_timeout", timeout_value);
       this->nat_config.nat_entry_generic_timeout = timeout_value;
       break;
     case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
       snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_icmp_timeout", timeout_value);
       this->nat_config.nat_entry_icmp_timeout = timeout_value;
       break;
     case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
       snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established", timeout_value);
       this->nat_config.nat_entry_tcp_established_timeout = timeout_value;
       break;
     case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
       snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout", timeout_value);
       ds_system_call(command, strlen(command));
       snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout_stream", timeout_value);
       this->nat_config.nat_entry_udp_timeout = timeout_value;
       break;
     default:
       LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
       *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
       return false;
  }
  ds_system_call(command, strlen(command));
  return true;
}


/*===========================================================================
  FUNCTION AddSNATEntryOnApps
==========================================================================*/
/*!
@brief
  Prepares  command based on the nat configuration to add SNAT
  entries to kernel and execute the same.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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
boolean QCMAP_NATALG::AddSNATEntryOnApps
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char tmp[MAX_COMMAND_STR_LEN];
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);


  QCMAP_CM_LOG_FUNC_ENTRY();

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (!QcMapBackhaulMgr  || !QcMapBackhaulMgr->enable_ipv4)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  /* Verify ARGs */
  if( nat_entry == NULL )
  {
    LOG_MSG_ERROR("NULL nat_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if ( !QCMAP_ConnectionManager::GetDeviceName\
       (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return FALSE;
  }

  bzero(command,MAX_COMMAND_STR_LEN);
  bzero(tmp,MAX_COMMAND_STR_LEN);

  if( ( QcMapNatAlg->nat_config.nat_type == QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01 )||
      ( QcMapNatAlg->nat_config.nat_type == QCMAP_MSGR_NAT_FULL_CONE_NAT_V01 ))
  {
    /*The below rule will add SNAT rules after the first rule in PREROUTING chain
      which will make sure that ARC nat rule will be at the begining of the
      PREROUTING chain rules */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -I PREROUTING 2 -i %s ",devname);
  }
  else
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -I PREROUTING -i %s ",devname);
  }

  if ( nat_entry->port_fwding_protocol!=0 )
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," -p %d ",nat_entry->port_fwding_protocol);
    strlcat(command, tmp, sizeof (command));
  }

  if ( nat_entry->port_fwding_global_port!=0 )
  {
    bzero(tmp,MAX_COMMAND_STR_LEN);
    snprintf( tmp, MAX_COMMAND_STR_LEN," --dport %d ",nat_entry->port_fwding_global_port);
    strlcat(command, tmp, sizeof (command));
  }

  if ( nat_entry->port_fwding_private_ip!=0 )
  {
    strlcat(command, " -j DNAT ", sizeof (command));
    bzero(tmp,MAX_COMMAND_STR_LEN);
    addr.s_addr = htonl(nat_entry->port_fwding_private_ip);
    strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
    snprintf(tmp, MAX_COMMAND_STR_LEN," --to-destination %s",tempIP);
    strlcat(command, tmp, sizeof (command));
    if ( nat_entry->port_fwding_private_port!=0 )
    {
      bzero(tmp,MAX_COMMAND_STR_LEN);
      snprintf( tmp, MAX_COMMAND_STR_LEN,":%d ",nat_entry->port_fwding_private_port);
      strlcat(command, tmp, sizeof (command));
    }
  }
  ds_system_call(command, strlen(command));
  return true;
}

/*===========================================================================
  FUNCTION DeleteSNATEntryOnApps
==========================================================================*/
/*!
@brief
  Prepares command based on the nat entry to be deleted.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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
boolean QCMAP_NATALG::DeleteSNATEntryOnApps
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  char tmp[MAX_COMMAND_STR_LEN];
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapBackhaulMgr  || !QcMapBackhaulMgr->enable_ipv4)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  /* Verify ARGs */
  if( nat_entry == NULL )
  {
    LOG_MSG_ERROR("NULL nat_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if(qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED &&
     !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
     !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
     !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
  {
    strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
  }
  else if ( !QCMAP_ConnectionManager::GetDeviceName\
       (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

  bzero(command,MAX_COMMAND_STR_LEN);
  bzero(tmp,MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN,
      "iptables -t nat -D PREROUTING -i %s ",devname);

  if( nat_entry->port_fwding_protocol!=0 )
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," -p %d ",nat_entry->port_fwding_protocol);
    strlcat(command, tmp, sizeof (command));
  }

  if( nat_entry->port_fwding_global_port !=0 )
  {
    bzero(tmp,MAX_COMMAND_STR_LEN);
    snprintf( tmp, MAX_COMMAND_STR_LEN," --dport %d ",nat_entry->port_fwding_global_port);
    strlcat(command, tmp, sizeof (command));
  }

  if( nat_entry->port_fwding_private_ip !=0 )
  {
    strlcat(command, " -j DNAT ", sizeof (command));
    bzero(tmp,MAX_COMMAND_STR_LEN);
    addr.s_addr = htonl(nat_entry->port_fwding_private_ip);
    strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
    snprintf(tmp, MAX_COMMAND_STR_LEN," --to-destination %s",tempIP);
    strlcat(command, tmp, sizeof (command));
    if( nat_entry->port_fwding_private_port !=0 )
    {
      bzero(tmp,MAX_COMMAND_STR_LEN);
      snprintf( tmp, MAX_COMMAND_STR_LEN,":%d ",nat_entry->port_fwding_private_port);
      strlcat(command, tmp, sizeof (command));
    }
  }
  ds_system_call(command, strlen(command));
  return true;
}

/*===========================================================================
  FUNCTION AddDMZOnApps
==========================================================================*/
/*!
@brief
  Adds DMZ IP address. Adding DMZ ipaddress will allow incomming packets,
  which are not intended to be received by softap clients to be
  captured on DMZ interface.

@parameters
  uint32       dmzIP

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
boolean QCMAP_NATALG::AddDMZOnApps
(
  uint32 dmzIP,
 qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  int qcmap_cm_error;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

/*-------------------------------------------------------------------------*/
  LOG_MSG_ERROR("Entering ADDDMZ on Apps",0,0,0);
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapBackhaulMgr || !QcMapBackhaulMgr->enable_ipv4)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  /* Verify DMZ IP */
  if( dmzIP == 0 )
  {
    LOG_MSG_ERROR("Invalid DMZ IP\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  /* Convert IP to ASCII format */
  addr.s_addr = htonl(dmzIP);
  strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
      QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",
             ECM_IFACE, tempIP );
  }
  else if(QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",\
              QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
              tempIP );
  }
  else if(QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",\
              ETH_IFACE,
              tempIP );
  }
  else
  {
    if( qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED &&
        !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
        !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
        !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
    {
        strlcpy(devname,QcMapBackhaulMgr->wan_cfg.ipv4_interface,sizeof(devname));
    }
    else if(qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,QCMAP_MSGR_IP_FAMILY_V4_V01,\
        devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS)
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
      *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
      return false;
    }
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",
              devname,
              tempIP );
  }
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION AddDMZ
==========================================================================*/
/*!
@brief
  Configures DMZ ipaddress and updates the configuration file
  with the DMZ IP .

@parameters
  uint32       dmzIP

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
boolean QCMAP_NATALG::AddDMZ(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  in_addr addr;
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  uint32 ip_addr;

  QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_DMZ_IP, GET_VALUE, data, MAX_STRING_LENGTH);

  inet_aton(data, (in_addr *)&(ip_addr));
  ip_addr = ntohl(ip_addr);
  if (!ip_addr)
  {
    addr.s_addr = htonl(dmz_ip);
    QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_DMZ_IP, SET_VALUE, inet_ntoa(addr),
                                 strlen(inet_ntoa(addr)));
  }
  else
  {
    LOG_MSG_ERROR("DMZ is already configured",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  if(QcMapNatAlg)
  {
    QcMapNatAlg->nat_config.dmz_ip = dmz_ip;

    if( !QcMapNatAlg->AddDMZOnApps(dmz_ip, qmi_err_num) )
    {
      if( *qmi_err_num != QMI_ERR_INTERFACE_NOT_FOUND_V01 )
      {
        LOG_MSG_ERROR("Add DMZ on Apps failed",0,0,0);
        return false;
      }
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION DeleteDMZOnApps
==========================================================================*/
/*!
@brief
  Ths is function will remove the DMZ ipaddress entry.

@parameters
  uint32       dmzIP

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
boolean QCMAP_NATALG::DeleteDMZOnApps
(
  uint32 dmzIP,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  int qcmap_cm_error;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY();

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  /* Verify DMZ IP */
  if( dmzIP == 0 )
  {
    LOG_MSG_ERROR("Invalid DMZ IP\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  /* Convert IP to ASCII format */
  addr.s_addr = htonl(dmzIP);
  strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));

  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
      QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s", ECM_IFACE, tempIP );
  }
  else if (QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
           QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
  snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",\
            QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
            tempIP );
  }
  else if(QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",\
              ETH_IFACE,
              tempIP );
  }
  else
  {
    if(qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED)
    {
      strlcpy(devname, QcMapBackhaulMgr->wan_cfg.ipv4_interface, sizeof(devname));
    }
    else if ( qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle, QCMAP_MSGR_IP_FAMILY_V4_V01,\
         devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS)
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
      *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
      return false;
    }
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",
              devname,
              tempIP );
  }

  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION DeleteDMZ
==========================================================================*/
/*!
@brief
  Will disable DMZ feature.

@parameters
  void

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
boolean QCMAP_NATALG::DeleteDMZ(qmi_error_type_v01 *qmi_err_num)
{
  in_addr addr;
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  if(QcMapNatAlg)
  {
    if ( !QcMapNatAlg->DeleteDMZOnApps(QcMapNatAlg->nat_config.dmz_ip, qmi_err_num) )
    {
      if(*qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 ||
         *qmi_err_num == QMI_ERR_INVALID_ARG_V01)
      {
        return false;
      }
    }
    QcMapNatAlg->nat_config.dmz_ip=0;
  }

  addr.s_addr = 0;
  QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_DMZ_IP, SET_VALUE, inet_ntoa(addr),
                                    strlen(inet_ntoa(addr)));

  return true;
}

/*===========================================================================
  FUNCTION GetDMZ
==========================================================================*/
/*!
@brief
  Gets the DMZ ipaddress configured.

@parameters
  uint32_t *dmz_ip

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
boolean QCMAP_NATALG::GetDMZ(uint32_t *dmz_ip, qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  if(QcMapNatAlg)
  {
    if ( QcMapNatAlg->nat_config.dmz_ip == 0 )
    {
      LOG_MSG_ERROR("\nDMZ IP not configured!!",0,0,0);
      *qmi_err_num = QMI_ERR_DISABLED_V01;
      return false;
    }
    *dmz_ip = QcMapNatAlg->nat_config.dmz_ip;
    return true;
  }

  QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_DMZ_IP, GET_VALUE, data, MAX_STRING_LENGTH);
  inet_aton(data, (in_addr *)dmz_ip);
  *dmz_ip=ntohl(*dmz_ip);
  if (*dmz_ip == 0)
  {
    LOG_MSG_ERROR("\nDMZ IP not configured!!",0,0,0);
    *qmi_err_num = QMI_ERR_DISABLED_V01;
    return false;
  }

  return true;
}



/*===========================================================================
  FUNCTION EnableAlg
==========================================================================*/
/*!
@brief
  Enables Algs Functionality.

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
boolean QCMAP_NATALG::EnableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  boolean ret_rtsp, ret_sip;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  char data[MAX_STRING_LENGTH] = {0};
  boolean enable = TRUE;

  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  snprintf(data, sizeof(data), "%d", enable);

  if(alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
  {
    QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_RTSP_ALG, SET_VALUE,
                                         data, strlen(data));
  }
  else
  {
    QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_SIP_ALG, SET_VALUE,
                                         data, strlen(data));
  }

  if (QcMapNatAlg)
  {
    if( !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() )
    {
      if ( qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED )
      {
        LOG_MSG_ERROR("IPv4 backhaul down:cannot enable ALG now, but will change cfg",
                       0, 0, 0);
        *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;

        if (alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_RTSP_ALG_V01 ;
        }
        else if (alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
        }
        return false;
      }
    }

    if( (alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01) ==
         QCMAP_MSGR_MASK_RTSP_ALG_V01 )
    {
      /*-----------------------------------------------------------------------
           Install RTSP ALG Kernel Module
          ----------------------------------------------------------------------*/
      ret_rtsp = QcMapNatAlg->EnableRTSPAlg(qmi_err_num);
      if ( ret_rtsp != true )
      {
        LOG_MSG_ERROR("Fail to enable RTSP ALG: = %d.\n",
                     ret_rtsp, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully enabled RTSP ALG", 0, 0, 0);
        // Set mask bit for RTSP ALG module enabled
        QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_RTSP_ALG_V01 ;
      }
    }

    if(alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
    {
      ret_sip = QcMapNatAlg->EnableSIPAlg(qmi_err_num);
      if ( ret_sip != true )
      {
        LOG_MSG_ERROR("Fail to enable SIP ALG, Ret value = %d",
                      ret_sip, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully enabled SIP ALG", 0, 0, 0);
        // Set mask for ALG module enabled
        QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
      }
    }

    if((ret_rtsp == false) && (ret_sip == false))
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    else if((ret_rtsp == false)|| (ret_sip == false))
    {
      *qmi_err_num = QMI_ERR_OP_PARTIAL_FAILURE_V01;
      return false;
    }
    else
    {
      return true;
    }
  }
}
/*===========================================================================
  FUNCTION DisableAlg
==========================================================================*/
/*!
@brief
  Disables Algs Functionality.

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

boolean QCMAP_NATALG::DisableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret_rtsp, ret_sip;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  char data[MAX_STRING_LENGTH] = {0};
  boolean enable = false;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  snprintf(data, sizeof(data), "%d", enable);

  if(alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
  {
    QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_RTSP_ALG, SET_VALUE,
                                         data, strlen(data));
  }
  else
  {
    QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_SIP_ALG, SET_VALUE,
                                         data, strlen(data));
  }


  if(QcMapNatAlg)
  {
    if( !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() )
    {
      if ( qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED )
      {
        LOG_MSG_ERROR(" IPv4 backhaul down: cannot disable ALG", 0,0,0);
        *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;

        if (alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_RTSP_ALG_V01;
        }
        else if (alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_SIP_ALG_V01;
        }
        return false;
      }
    }

    if(alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      /*-----------------------------------------------------------------------
             Unload the RTSP ALG module from Kernel
             -----------------------------------------------------------------------*/
      ret_rtsp = QcMapNatAlg->DisableRTSPAlg(qmi_err_num);
      if ( ret_rtsp != true )
      {
        LOG_MSG_ERROR("Fail to disable RTSP ALG. error = %d.\n",
                      ret_rtsp, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully disabled RTSP ALG", 0, 0, 0);
        // Reset mask bit for RTSP ALG module
        QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_RTSP_ALG_V01 ;
      }
    }

    if(alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
    {
      ret_sip = QcMapNatAlg->DisableSIPAlg(qmi_err_num);
      if ( ret_sip != true )
      {
        LOG_MSG_ERROR("Fail to disable SIP ALG, Ret value = %d.\n",
                      ret_sip, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully disabled SIP ALG", 0, 0, 0);
        // Reset mask bit for SIP ALG module
        QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_SIP_ALG_V01 ;
      }
    }

    if((ret_rtsp == false) && (ret_sip == false))
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    else if((ret_rtsp == false)|| (ret_sip == false))
    {
      *qmi_err_num = QMI_ERR_OP_PARTIAL_FAILURE_V01;
      return false;
    }
    else
    {
      return true;
    }
  }
}
/*===========================================================================
  FUNCTION EnableRTSPAlg
==========================================================================*/
/*!
@brief
  Enables RTSP Alg Functionality.

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
boolean QCMAP_NATALG::EnableRTSPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  int enable = TRUE;
  int ret = QCMAP_CM_SUCCESS;
  char Kernel_ver[KERNEL_VERSION_LENGTH];
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);


  snprintf(data, sizeof(data), "%d", enable);
  QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_RTSP_ALG, SET_VALUE , data, strlen(data));

  if (QcMapNatAlg)
  {
    if ((QcMapNatAlg->alg_enabled_disabled_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01) ==
        QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      LOG_MSG_ERROR("RTSP ALG is already enabled.\n", 0, 0, 0);
      return false;
    }
    else
    {
      if ( false == QcMapMgr->GetKernelVer(Kernel_ver) )
      {
        LOG_MSG_ERROR("Unable to get the kernel version info", 0, 0, 0);
        return false;
      }
      /*-----------------------------------------------------------------------
          Install RTSP ALG Kernel Module
       -----------------------------------------------------------------------*/
      snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/kernel/drivers/net/nf_conntrack_rtsp.ko",Kernel_ver);
      ds_system_call(command, strlen(command));

      snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/kernel/drivers/net/nf_nat_rtsp.ko",Kernel_ver);
      ds_system_call(command, strlen(command));

      QcMapNatAlg->alg_enabled_disabled_mask = QcMapNatAlg->alg_enabled_disabled_mask |
                                        QCMAP_MSGR_MASK_RTSP_ALG_V01;
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableRTSPAlg
==========================================================================*/
/*!
@brief
  Disables RTSP Alg Functionality.

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

boolean QCMAP_NATALG::DisableRTSPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  int enable = false;
  int ret = QCMAP_CM_SUCCESS;
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  snprintf(data, sizeof(data), "%d", enable);

  if (QcMapNatAlg)
  {
    if ((QcMapNatAlg->alg_enabled_disabled_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01) ==
         QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      snprintf(command,MAX_COMMAND_STR_LEN, "rmmod  nf_nat_rtsp");
      ds_system_call(command, strlen(command));

      snprintf(command,MAX_COMMAND_STR_LEN, "rmmod nf_conntrack_rtsp");
      ds_system_call(command, strlen(command));

      QcMapNatAlg->alg_enabled_disabled_mask = (QcMapNatAlg->alg_enabled_disabled_mask &
                                       ~(QCMAP_MSGR_MASK_RTSP_ALG_V01));
    }
    else
    {
      LOG_MSG_ERROR("RTSP ALG is already disabled.\n", 0, 0, 0);
      return false;
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableSIPAlg
==========================================================================*/
/*!
@brief
  - Enables SIP Alg Functionality.
  - Writes to a proc/sys entry which indicates netfilter to enable SIP ALG
    processing (corresponding code added in kernel)

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
boolean QCMAP_NATALG::EnableSIPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  int enable = TRUE;
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  snprintf(data, sizeof(data), "%d", enable);
  QCMAP_NATALG::GetSetNATConfigFromXML(CONFIG_SIP_ALG, SET_VALUE, data, strlen(data));

  if (QcMapNatAlg)
  {
    ds_system_call("echo 0 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg",
                   strlen("echo 0 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg"));

    /*Enable media streaming between non signalling endpoints */
    ds_system_call("echo 0 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media",
                   strlen("echo 0 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media"));

    QcMapNatAlg->alg_enabled_disabled_mask = QcMapNatAlg->alg_enabled_disabled_mask |
                                      QCMAP_MSGR_MASK_SIP_ALG_V01;
  }
  return true;
}


/*===========================================================================
  FUNCTION DisableSIPAlg
==========================================================================*/
/*!
@brief
  - Disables SIP Alg Functionality.
  - Writes to a proc/sys entry which indicates netfilter to bypass SIP ALG
    processing (corresponding code added in kernel)

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

boolean QCMAP_NATALG::DisableSIPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  ds_system_call("echo 1 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg",
                 strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg"));

  /*Disable media streaming between non signalling endpoints */
  ds_system_call("echo 1 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media",
                 strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media"));

  LOG_MSG_INFO1("SIP ALG disabled", 0, 0, 0);

  QcMapNatAlg->alg_enabled_disabled_mask = (QcMapNatAlg->alg_enabled_disabled_mask &
                                     ~(QCMAP_MSGR_MASK_SIP_ALG_V01));

  return true;
}

/*===========================================================================
  FUNCTION qcmap_compare_nat_entries
==========================================================================*/
/*!
@brief
  - To compare two NAT enteries

@param
  const void *first,
  const void *second

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

long int qcmap_compare_nat_entries
(
    const void *first,
    const void *second
)
{
    qcmap_cm_port_fwding_entry_conf_t* entry_to_delete = (qcmap_cm_port_fwding_entry_conf_t*)first;
    qcmap_cm_port_fwding_entry_conf_t* nat_entry = (qcmap_cm_port_fwding_entry_conf_t*)second;

  if((entry_to_delete->port_fwding_private_ip) ==
     nat_entry->port_fwding_private_ip &&
     entry_to_delete->port_fwding_private_port ==
     nat_entry->port_fwding_private_port &&
     entry_to_delete->port_fwding_global_port ==
     nat_entry->port_fwding_global_port &&
     entry_to_delete->port_fwding_protocol ==
     nat_entry->port_fwding_protocol)
  {
    return 0;
  }
  return 1;
}
