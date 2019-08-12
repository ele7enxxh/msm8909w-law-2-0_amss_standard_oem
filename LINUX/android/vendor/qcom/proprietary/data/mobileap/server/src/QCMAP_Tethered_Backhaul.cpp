/*====================================================

FILE:  QCMAP_Tethered_Backhaul.cpp

SERVICES:
   QCMAP Connection Manager Tethered Backhaul Specific Implementation

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/31/16   jc         Created

======================================================*/
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "QCMAP_Firewall.h"
#include "ds_qmi_qcmap_msgr.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Backhaul_Ethernet.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_LAN.h"
#include "QCMAP_Tethered_Backhaul.h"
/*=====================================================
                             Class Definitions
  =====================================================*/

/*=====================================================
  FUNCTION Constructor
======================================================*/
/*!
@brief
  Initializes Tethered Backhaul variables.

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
/*====================================================*/

QCMAP_Tethered_Backhaul::QCMAP_Tethered_Backhaul()
{
}


/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroyes the Cradle Object.

@parameters
none

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/


QCMAP_Tethered_Backhaul::~QCMAP_Tethered_Backhaul()
{
}

/* For JO there is no IPA. */
  #ifndef FEATURE_DATA_TARGET_MDM9607

/*=====================================================
  FUNCTION SetIPACMCategory
======================================================*/
/*!
@brief
  Sets the category and mode in IPACM xml file

@parameters
  uint8_t status

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
boolean QCMAP_Tethered_Backhaul::SetIPACMCategory(
                                 char* iface_name,
                                 char* category,
                                 char* mode)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  QCMAP_ConnectionManager* QcMapMgr=
                           QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPACMIface_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strncasecmp(child.child(Name_TAG).child_value(),
                     iface_name, strlen(iface_name)))
    {
      child.child(Category_TAG).text() = category;
      child.child(Mode_TAG).text() = mode;
      QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
      return true;
    }
  }

  LOG_MSG_ERROR("ECM tag not found.",0,0,0);
  return false;
}

#endif /* FEATURE_DATA_TARGET_MDM9607 */

/*=====================================================
  FUNCTION GetSetConfigFromXML
======================================================*/
/*!
@brief
  Get and sets the Backhaul Ethernet/cradle Config from XML.

@parameters
  qcmap_action_type action,
  QCMAP_Tethered_backhaul_type backhaul_type,
  void *mode

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

static boolean QCMAP_Tethered_Backhaul::GetSetConfigFromXML(
                                    qcmap_action_type action,
                                    QCMAP_Tethered_backhaul_type backhaul_type,
                                    void *mode)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
  int backhaul_mode = 0;
/*------------------------------------------------------------------------*/
  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  if (QCMAP_TETH_CRADLE_BACKHAUL == backhaul_type)
  {
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(Cradle_TAG);
    child = root.child(CradleMode_TAG);
  }
  else if (QCMAP_TETH_ETH_BACKHAUL == backhaul_type)
  {
    root =
       xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(EthBackhaul_TAG);
    child = root.child(EthBackhaulMode_TAG);
  }
  else
  {
    LOG_MSG_ERROR("Invalid Backhaul type.",0,0,0);
    return false;
  }

  if(action == GET_VALUE)
  {
    backhaul_mode = atoi(child.child_value());
    memcpy((mode),&backhaul_mode,sizeof(backhaul_mode));
  }
  else
  {
    memcpy(&backhaul_mode,(mode),sizeof(backhaul_mode));
    snprintf(data, sizeof(data), "%d",backhaul_mode);
    child.text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
  }
  LOG_MSG_INFO1("Tethered backhaul mode: %d Action: %d",
                backhaul_mode,
                action,0);

  return true;
}

/*=====================================================
  FUNCTION DisableV4V6RulesOnOtherBackhaul
======================================================*/
/*!
@brief
 Disable NAT, forwarding and Iface related V4 and V6 rules on the Backhaul
 other than the specified one.

@parameters
  qcmap_backhaul_type backhaul_type

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Tethered_Backhaul::DisableV4V6RulesOnOtherBackhaul(
                                        qcmap_backhaul_type backhaul_type)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  qmi_error_type_v01 qmi_err_num;
  char sta_iface[QCMAP_MSGR_INTF_LEN];
 /*------------------------------------------------------------------------*/
   if (!QcMapBackhaulMgr || !QcMapNatAlg || !QcMapFirewall)
   {
     LOG_MSG_ERROR("DisableRulesOnOtherBackhaul:"
                   "Invalid objects to proceed.",0,0,0);
     return;
   }

   if (QcMapBackhaulMgr->current_backhaul != backhaul_type &&
            QcMapBackhaulMgr->current_backhaul != NO_BACKHAUL)
   {
     if ( qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED ||
             QCMAP_Backhaul_WLAN::IsSTAAvailableV4()                 ||
          (backhaul_type != BACKHAUL_TYPE_CRADLE &&
                QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4()) ||
          (backhaul_type != BACKHAUL_TYPE_ETHERNET &&
                QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4()))
    {
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }
    if ( qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED ||
         QCMAP_Backhaul_WLAN::IsSTAAvailableV6()         ||
         (backhaul_type != BACKHAUL_TYPE_CRADLE &&
                QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6()) ||
          (backhaul_type != BACKHAUL_TYPE_ETHERNET &&
                QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6()))
    {
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
      {
        /* Delete external routes on WLAN0 */
        snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);
        QcMapBackhaulMgr->DeleteAllV6ExternalRoute(sta_iface);
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
      }
      if (backhaul_type != BACKHAUL_TYPE_CRADLE &&
          QcMapBackhaulCradleMgr &&
          QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
      {
        if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
          QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
        else
        {
          QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
          QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
        }
      }
      if (backhaul_type != BACKHAUL_TYPE_ETHERNET &&
          QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
      {
        QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
        QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
      }
    }
  }
}
/*=====================================================
  FUNCTION ProcAddrDissalocOnBackhaul
======================================================*/
/*!
@brief
 Disable NAT, forwarding and Iface related V4 and V6 rules on the Backhaul
 as part of address Disallocation.

@parameters
  qcmap_backhaul_type backhaul_type

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Tethered_Backhaul::ProcAddrDisAssocOnBackhaul(
                                        qcmap_backhaul_type backhaul_type)
{
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                            QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                            QCMAP_Backhaul_Ethernet::Get_Instance(false);
  qmi_error_type_v01 qmi_err_num;
  boolean backhaul_v4_available = false;
  boolean backhaul_v6_available = false;
  char iface_name[MAX_IFACE_NAME_LENGTH];
  char command[MAX_COMMAND_STR_LEN];
/*------------------------------------------------------------------------*/
  if ( !QcMapNatAlg)
  {
    LOG_MSG_ERROR("\n NATALG object not created.\n",0,0,0);
    return;
  }
  if ( !QcMapFirewall)
  {
    LOG_MSG_ERROR("\n Firewall object not created.\n",0,0,0);
    return;
  }

  memset(command,0,MAX_COMMAND_STR_LEN);
  switch (backhaul_type)
  {
    case BACKHAUL_TYPE_CRADLE:
    {
      if (QcMapBackhaulCradleMgr)
      {
        backhaul_v4_available = QcMapBackhaulCradleMgr->cradle_v4_available;
        backhaul_v6_available = QcMapBackhaulCradleMgr->cradle_v6_available;
        /*Reset Cradle V4 Available flag*/
        QcMapBackhaulCradleMgr->cradle_v4_available = false;
        /*Reset Cradle V6 Available flag*/
        QcMapBackhaulCradleMgr->cradle_v6_available = false;
        snprintf((char *)iface_name,MAX_IFACE_NAME_LENGTH,ECM_IFACE);
      }
      else
      {
        LOG_MSG_ERROR("ProcAddrDissalocOnBackhaul:"
                   "NULL cradle object.",0,0,0);
        return;
      }
      break;
    }
    case BACKHAUL_TYPE_ETHERNET:
    {
      if (QcMapBackhaulEthMgr)
      {
        backhaul_v4_available = QcMapBackhaulEthMgr->eth_v4_available;
        backhaul_v6_available = QcMapBackhaulEthMgr->eth_v6_available;
        QcMapBackhaulEthMgr->eth_v4_available = false;
        QcMapBackhaulEthMgr->eth_v6_available = false;
        snprintf((char *)iface_name,MAX_IFACE_NAME_LENGTH,ETH_IFACE);
      }
      else
      {
        LOG_MSG_ERROR("ProcAddrDissalocOnBackhaul:"
                   "NULL ETH backhaul object.",0,0,0);
        return;
      }
      break;
    }
    default:
      LOG_MSG_ERROR("ProcAddrDissalocOnBackhaul:"
                   "Backhaul type %d not supported.",backhaul_type,0,0);
      return;
      break;
  }

  if (QcMapBackhaulMgr->current_backhaul == backhaul_type)
  {
    if (backhaul_v4_available)
    {
      /* Disable NAT on A5 and clean IPV4 firewall */
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }
    if (backhaul_v6_available)
    {
      /* Disable IPV6 Forwarding and firewall */
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(iface_name);
      if (QCMAP_Backhaul_Cradle::IsCradleBridgeActivated()) {
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
      }
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(iface_name);
    }
  }
  /* stop dhcp client on ecm0 and bridge0 */

  if (QCMAP_Backhaul_Cradle::IsCradleBridgeActivated())
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);
  else
    QcMapBackhaulMgr->StopDHCPCD(iface_name);

  /*Set disable_ipv6 flag to 0 for ECM Iface */
  snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
           iface_name);
  ds_system_call(command, strlen(command));

  /*Set accept_ra flag to 1 for ECM Iface */
  snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
           iface_name);
  ds_system_call(command, strlen(command));

}

