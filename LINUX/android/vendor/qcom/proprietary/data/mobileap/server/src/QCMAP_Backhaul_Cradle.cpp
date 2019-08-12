/*====================================================

FILE:  QCMAP_Backhaul_Cradle.cpp

SERVICES:
   QCMAP Connection Manager Backhaul (Cradle) Specific Implementation

=====================================================

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created

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
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */

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
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_LAN.h"


bool QCMAP_Backhaul_Cradle::flag= false;
QCMAP_Backhaul_Cradle* QCMAP_Backhaul_Cradle::object=NULL;

QCMAP_Backhaul_Cradle* QCMAP_Backhaul_Cradle::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(true);

    LOG_MSG_INFO1("Creating object : CRADLE",0, 0, 0);
    object = new QCMAP_Backhaul_Cradle();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*=====================================================
                             Class Definitions
  =====================================================*/

/*=====================================================
  FUNCTION Constructor
======================================================*/
/*!
@brief
  Initializes Backhaul Cradle variables.

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

QCMAP_Backhaul_Cradle::QCMAP_Backhaul_Cradle
(
)
{
  memset(&this->cradle_cfg,0,sizeof(this->cradle_cfg));
  this->cradle_v4_available = false;
  this->cradle_v6_available = false;
  this->cradle_connect_in_progress = false;
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(
                                              GET_VALUE,
                                              QCMAP_TETH_CRADLE_BACKHAUL,
                                              (void *)&(this->cradle_cfg.cradle_mode));

  return;
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


QCMAP_Backhaul_Cradle::~QCMAP_Backhaul_Cradle()
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=
                                     QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=
                                     QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  LOG_MSG_INFO1("Destroying Object: CRADLE",0,0,0);
  flag=false;
  object=NULL;

  if(!QcMapBackhaulWLANMgr && !QcMapBackhaulWWANMgr && !QcMapBackhaulEthMgr)
    delete QcMapBackhaulMgr;
}


/*===========================================================================
FUNCTION IsCradleBridgeActivated
==========================================================================*/
/*!
@brief
  Checks if the Cralde WAN/LAN Bridge is Activated

@parameters
  void

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

boolean QCMAP_Backhaul_Cradle::IsCradleBridgeActivated(void)
{
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);

  if (!QcMapBackhaulCradleMgr)
  {
    LOG_MSG_ERROR("Not in cradle mode",0,0,0);
    return false;
  }

  if (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01 ||
      QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01)
    return true;

  return false;
}


/*===========================================================================
FUNCTION IsCradleWANBridgeActivated
==========================================================================*/
/*!
@brief
  Checks if the Cralde WAN Bridge is Activated

@parameters
  void

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

boolean QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated(void)
{
  qcmap_msgr_cradle_mode_v01 mode;
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  mode= QCMAP_MSGR_CRADLE_DISABLED_V01;

  if (NULL != QcMapBackhaulCradleMgr)
    mode = QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode;

  if (mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01)
    return true;

  LOG_MSG_INFO1("Not in WAN Bridge mode", 0,0,0);
  return false;
}


/*=====================================================
  FUNCTION GetCradleMode
======================================================*/
/*!
@brief
  Retreives the usb cradle mode.

@parameters
  char *process

@return
  true  - cradle mode found
  flase - cradle mode not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
boolean QCMAP_Backhaul_Cradle::GetCradleMode
(
  qcmap_msgr_cradle_mode_v01 *mode,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  if (mode == NULL || qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    return false;
  }

  if(NULL != QcMapBackhaulCradleMgr)
    *mode = QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode;
  else
    QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                        QCMAP_TETH_CRADLE_BACKHAUL,
                        (void *)mode);

  return true;
}

/*======================================================
  FUNCTION SetCradleMode
=====================================================*/
/*!
@brief
  Sets the usb cradle mode.

@parameters
  char *process

@return
  true  - cradle mode set.
  flase - cradle mode set failed.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
boolean QCMAP_Backhaul_Cradle::SetCradleMode
(
  qcmap_msgr_cradle_mode_v01  mode,
  void                        *softApHandle,
  qmi_error_type_v01          *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=NULL;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_msgr_cradle_mode_v01 prev_mode;

  qmi_qcmap_msgr_softap_handle_type *handle = (qmi_qcmap_msgr_softap_handle_type*)softApHandle;
  char command[MAX_COMMAND_STR_LEN];
  int retval, err_num;

  *qmi_err_num = QMI_ERR_NO_EFFECT_V01;

  if (handle == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if((NULL != QcMapBackhaulCradleMgr) && mode == QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode)
  {
    LOG_MSG_INFO1("Cradle mode %d is already in effect", mode,0,0);
    return false;
  }

  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                                             QCMAP_TETH_CRADLE_BACKHAUL,
                                             (void *)&prev_mode);
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(SET_VALUE,
                                             QCMAP_TETH_CRADLE_BACKHAUL,
                                             (void *)&mode);
  /* Case that is being checked here:
    1. If the TETHERED link is not UP (i.e. QcMapTetheringMgr is NULL)
    2. USB Link is UP but type is not QCMAP_QTI_TETHERED_LINK_ECM
  */
  if ((!QcMapTetheringMgr) ||
      (QcMapTetheringMgr &&
       QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type !=
       QCMAP_QTI_TETHERED_LINK_ECM))
  {
    /* Since ECM is not connected, check IPA xml and write to config only */
    LOG_MSG_INFO1("TETHERED link is not ECM, wait for ecm connection",0,0,0);
    /* For JO there is no IPA. */
    #ifndef FEATURE_DATA_TARGET_MDM9607
      switch (mode)
      {
         case QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   LAN_CATEGORY,
                                                   BRIDGE_MODE);
           break;
         case QCMAP_MSGR_CRADLE_LAN_ROUTER_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   LAN_CATEGORY,
                                                   ROUTER_MODE);
           break;
         case QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   LAN_CATEGORY,
                                                   BRIDGE_MODE);
           break;
         case QCMAP_MSGR_CRADLE_WAN_ROUTER_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   WAN_CATEGORY,
                                                   ROUTER_MODE);
           break;
         default:
           LOG_MSG_ERROR("Unknown mode %d",mode,0,0);
           break;
      }
    #endif /* FEATURE_DATA_TARGET_MDM9607 */

    LOG_MSG_INFO1("Set cradle mode %d",mode,0,0);
    if(NULL != QcMapBackhaulCradleMgr)
      QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode = mode;

    return true;
  }

  switch (mode)
  {
    case QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01:
    case QCMAP_MSGR_CRADLE_LAN_ROUTER_V01:
    case QCMAP_MSGR_CRADLE_DISABLED_V01:
      LOG_MSG_INFO1("Seting cradle LAN Router/Bridge mode", 0, 0, 0);
      /* For JO there is no IPA. */
#ifndef FEATURE_DATA_TARGET_MDM9607
      switch (mode)
      {
         case QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   LAN_CATEGORY,
                                                   BRIDGE_MODE);
           break;
         case QCMAP_MSGR_CRADLE_LAN_ROUTER_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   LAN_CATEGORY,
                                                   ROUTER_MODE);
           break;
         default:
           LOG_MSG_ERROR("Unknown mode %d",mode,0,0);
           break;
      }
#endif /* FEATURE_DATA_TARGET_MDM9607 */

      if (prev_mode == QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01 ||
          prev_mode == QCMAP_MSGR_CRADLE_LAN_ROUTER_V01 ||
          prev_mode == QCMAP_MSGR_CRADLE_DISABLED_V01)
      {
        //Nothing needs to be done when switching between these modes
        *qmi_err_num = QMI_ERR_NONE_V01;
        break;
      }

      /* Disable Cradle Mode */
      if ( QcMapTetheringMgr && !(retval = QCMAP_Tethering::BringDownTetheredLink(
          QCMAP_QTI_TETHERED_LINK_ECM,
          handle, &err_num)))
      {
        LOG_MSG_ERROR("BringDownTetheredLink failed %d %d", retval, err_num,0);
      }

      if(!(retval = QCMAP_Tethering::SetupTetheredLink(QCMAP_QTI_TETHERED_LINK_ECM, &err_num)))
      {
        LOG_MSG_ERROR("SetupTetheredLink failed %d %d", retval, err_num,0);
      }

      //Unbridge ecm0 and bridge0 before sending deprecate RA
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",
               BRIDGE_IFACE, ECM_IFACE);
      ds_system_call(command, strlen(command));

      //Bridge ecm0 and bridge0
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
               BRIDGE_IFACE, ECM_IFACE);
      ds_system_call(command, strlen(command));
      /* Need to return from here as the cradle object reference we are holding
      * is already deleted and we should not access it below.
      */
      return true;

    case QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01:
    case QCMAP_MSGR_CRADLE_WAN_ROUTER_V01:

      if (QcMapTetheringMgr)
      {
        if (!(retval = QCMAP_Tethering::BringDownTetheredLink(
              QCMAP_QTI_TETHERED_LINK_ECM,
              handle, &err_num)))
        {
          LOG_MSG_ERROR("BringDownTetheredLink failed %d %d", retval, err_num,0);
        }
      }
      /*there is an issue where cradle object is deleted as part of
        BringDownTetheredLink and that can lead to AP+STA being enabled.
        if we transistion from Cradle WAN to another Cradle WAN mode,
        then delete the cradle object which in turn result is setting the
        cradle_connect_in_progress flag to false. At this moement if AP+STA
        bridge is enabled, then its get enabled*/
        QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(true);

        if (QcMapBackhaulCradleMgr)
          QcMapBackhaulCradleMgr->cradle_connect_in_progress = true;


    /* For JO there is no IPA. */
#ifndef FEATURE_DATA_TARGET_MDM9607
      switch (mode)
      {
         case QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   LAN_CATEGORY,
                                                   BRIDGE_MODE);
           break;
         case QCMAP_MSGR_CRADLE_WAN_ROUTER_V01:
           QCMAP_Backhaul_Cradle::SetIPACMCategory(ECM_IFACE,
                                                   WAN_CATEGORY,
                                                   ROUTER_MODE);
           break;
         default:
           LOG_MSG_ERROR("Unknown mode %d",mode,0,0);
           break;
      }
#endif /* FEATURE_DATA_TARGET_MDM9607 */
      if (!(retval = QCMAP_Tethering::SetupTetheredLink(QCMAP_QTI_TETHERED_LINK_ECM,
           &err_num)))
      {
        LOG_MSG_ERROR("SetupTetheredLink failed %d %d", retval, err_num,0);
        delete QcMapTetheringMgr;
        delete QcMapBackhaulCradleMgr;

        if (!QcMapBackhaulWLANMgr && !QcMapBackhaulWWANMgr)
        {
          delete QcMapBackhaulMgr;
        }
      }
      /* Need to get the new object reference here as in BringDownTetheredLink object will be deleted. */
      QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
      break;
    default:
      /* Unknown mode */
      LOG_MSG_INFO1("Cradle mode unknown %d",mode,0,0);
      return false;
    break;
  }

  LOG_MSG_INFO1("Set cradle mode %d",mode,0,0);
  if (NULL != QcMapBackhaulCradleMgr)
    QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode = mode;

  QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  if ((NULL != QcMapBackhaulCradleMgr) &&
      (NULL != QcMapBackhaulMgr))
  {
    if (!QcMapBackhaulMgr->enable_ipv6)
    {
      if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
      {
        /* Disable ipv6 for Bridge Iface */
        LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for STA Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
      else
      {
        /* Disable ipv6 for ecm Iface */
        LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for STA Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
    else
    {
      /*Set accept_ra flag to 2*/
      if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
      {
        LOG_MSG_INFO1("Setting accept_ra flag to 2",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
      else
      {
        LOG_MSG_INFO1("Setting accept_ra flag to 2",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 ECM_IFACE);
        ds_system_call(command, strlen(command));
        /* Dont set the dft route when information is recevied in an RA*/
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                 ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
    }

  }

  LOG_MSG_INFO1("prev_mode = %d",prev_mode,0,0);

  if (QcMapBackhaulMgr &&
      (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE ||
       QcMapBackhaulMgr->current_backhaul == NO_BACKHAUL))
  {
    LOG_MSG_INFO1("Removing iface related rules",0,0,0);
    if (prev_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
    {
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(ECM_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
      QcMapBackhaulMgr->BlockIPv4WWANAccess();
      QcMapBackhaulMgr->BlockIPv6WWANAccess();
    }
    else if (prev_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01)
    {
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
    }
  }
  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}
/*======================================================
FUNCTION ProcessCradleAddrAssign
======================================================*/
/*!
@brief
Process the address assignment from Cradle

@parameters
devname
ip_vsn (ip versio)

@return
void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

void QCMAP_Backhaul_Cradle::ProcessCradleAddrAssign(char* devname,
                                                   ip_version_enum_type ip_vsn)
{
  char command[MAX_COMMAND_STR_LEN];
  in_addr craIp, craMask, craGwIp;
  in_addr_t default_gw_addr=0;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  boolean switch_backhaul_required = false;
  char gw[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char iface_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qcmap_backhaul_type prev_backhaul;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  memset(iface_ip, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gw, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  LOG_MSG_INFO1("Entering ProcessCradleAddrAssign for further handling",0,0,0);

  if ( !QcMapBackhaulCradleMgr )
  {
    LOG_MSG_ERROR("Unable to get Cradle Backhaul object",0,0,0);
    return;
  }

  /*Logic to decide whether backhaul needs to be switched or not*/
  if (QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_CRADLE,
                                        QcMapBackhaulMgr->current_backhaul))
  {
    switch_backhaul_required = true;
  }

  if (ip_vsn == IP_V4)
  {
    ret = QcMapBackhaulMgr->GetIP(&craIp.s_addr, &craMask.s_addr, devname);
    if ( true == ret )
    {
      LOG_MSG_INFO1("Device Connected to cradle for V4.\n",0,0,0);

      if (QcMapBackhaulCradleMgr->cradle_v4_available &&
          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
      {
        LOG_MSG_ERROR("ProcessCradleAddrAssign : V4 Addr already assigned",0,0,0);
        return;
      }
      QcMapBackhaulCradleMgr->cradle_v4_available = true;


      /* Switch to Cradle interface. */
      /*Disconnect from WWAN */
      if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false &&
          QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
          switch_backhaul_required)
      {
        /*there is a case where when once switch from Cradle WAN bridge
        mode to Cradle WAN router mode, then AP+STA or WWAN backhaul gets
        enabled. we need to make sure that we call disableNAT and IPv6
        forwarding. the below logic also handles all the cases when one
        switches to Cradle WAN mode*/
        QCMAP_Tethered_Backhaul::DisableV4V6RulesOnOtherBackhaul(
                                                     BACKHAUL_TYPE_CRADLE);

        QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V4, devname);
        QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_CRADLE;
        QcMapNatAlg->EnableNATonApps();
        QcMapBackhaulCradleMgr->cradle_connect_in_progress = false;

        /* Enable IPv4 MCAST */
        LOG_MSG_INFO1(" Restart PIMD to enable MCAST forwarding for Cradle \n",0,0,0);
        QcMapMgr->StopMcastDaemon();
        QcMapMgr->StartMcastDaemon();
        LOG_MSG_INFO1("ProcessCradleAddrAssign : V4 Addr Assigned successfully",0,0,0);
      }
      else if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true)/* Brige Mode Activated. Start DHCP Relay */
      {
          QCMAP_Tethered_Backhaul::DisableV4V6RulesOnOtherBackhaul(
                                                     BACKHAUL_TYPE_CRADLE);
        QCMAPLANMgr->StopDHCPD();
        QcMapBackhaulCradleMgr->cradle_connect_in_progress = false;
        QcMapBackhaulCradleMgr->cradle_v4_available = true;
        prev_backhaul = QcMapBackhaulMgr->current_backhaul;
        QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_CRADLE;
        if (false == QcMapMgr->GetGatewayIP(&default_gw_addr))
        {
          if (prev_backhaul != BACKHAUL_TYPE_CRADLE)
            QcMapBackhaulMgr->current_backhaul = prev_backhaul;
          LOG_MSG_ERROR("Unable to get the Gateway IP",0,0,0);
        }
        else
        {
          snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -I FORWARD -p IPv4 --ip-proto udp --ip-dport 67 -j DROP");
          ds_system_call(command, strlen(command));
          snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -I FORWARD -p IPv4 --ip-proto udp --ip-dport 68 -j DROP");
          ds_system_call(command, strlen(command));

          craGwIp.s_addr = default_gw_addr;
          strlcpy(iface_ip, inet_ntoa(craIp), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
          strlcpy(gw, inet_ntoa(craGwIp), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
          ds_log_med("Cradle Bridge with IP %s\n",iface_ip);
          ds_log_med("Cradle Bridge with gateway IP %s\n",gw);
          snprintf( command,MAX_COMMAND_STR_LEN,"dnsmasq -i %s -I lo -z --dhcp-relay=%s,%s,%s --dhcp-proxy",
                    BRIDGE_IFACE, iface_ip, gw, BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          QcMapWLANMgr->RestartWLAN();

          QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V4, devname);
        }
      }
    }
    else
    {
      LOG_MSG_INFO1("Error: GetIP Failed. Bailing out.\n",0,0,0);
      return;
    }
  }
  else if (ip_vsn == IP_V6)
  {
    //Add check to ensure that device got connected to sta
    /*Logic to decide whether backhaul needs to be switched or not*/
    LOG_MSG_INFO1("Device Connected to cradle for V6.\n",0,0,0);
    if (QcMapBackhaulCradleMgr->cradle_v6_available &&
        QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
    {
      LOG_MSG_ERROR("ProcessCradleAddrAssign : V6 Addr already assigned",0,0,0);
      return;
    }
    QcMapBackhaulCradleMgr->cradle_v6_available = true;
    QcMapBackhaulCradleMgr->cradle_connect_in_progress = false;
    /* Switch to Cradle interface. */
    /*Disconnect from WWAN */
    if ((switch_backhaul_required ||
         QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true) &&
        (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      QCMAP_Tethered_Backhaul::DisableV4V6RulesOnOtherBackhaul(
                                                     BACKHAUL_TYPE_CRADLE);

      QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V6, devname);
      QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_CRADLE;
      /* TODO: we should do explicit prefix delegation in Cradle WAN
        bridge mode. EnableIpv6Forwarding does lot many things an hence
        not efficient*/
      QcMapBackhaulMgr->EnableIPV6Forwarding();

      if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
      QcMapFirewall->EnableIPV6Firewall();

      LOG_MSG_INFO1("ProcessCradleAddrAssign : V6 Addr Assigned successfully",0,0,0);
    }
  }

  qcmap_cm_process_cradle_ind(CRADLE_CONNECTED);

  return;
}
/*======================================================
FUNCTION ProcessCradleDisAssoc
======================================================*/
/*!
@brief
Bring down the Cradle iface

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

void QCMAP_Backhaul_Cradle::ProcessCradleDisAssoc()
{
  struct in_addr addr;
  char apps_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  LOG_MSG_INFO1("Entering ProcessCradleDisAssoc",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }
  if ( !QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("\n Backhaul object not created.\n",0,0,0);
    return;
  }
  if ( !QcMapBackhaulCradleMgr)
  {
    LOG_MSG_ERROR("\n Cradle object not created.\n",0,0,0);
    return;
  }

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

  this->ProcAddrDisAssocOnBackhaul(BACKHAUL_TYPE_CRADLE);

  if (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode ==
                                      QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01)
  {
    if (QcMapWLANMgr)
    {
      QcMapWLANMgr->RestartWLAN();
    }

    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.apps_ip_addr);
    strlcpy(apps_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.sub_net_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
             BRIDGE_IFACE, apps_ip, netmask);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -D FORWARD -p IPv4 --ip-proto udp --ip-dport 67 -j DROP");
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -D FORWARD -p IPv4 --ip-proto udp --ip-dport 68 -j DROP");
    ds_system_call(command, strlen(command));

    QCMAPLANMgr->StopDHCPD();
    QCMAPLANMgr->StartDHCPD();

    // Assign Original MAC Address to Bridge
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig bridge0 hw ether %s",
               ether_ntoa(&QCMAPLANMgr->bridge_local_mac_addr));
    ds_system_call(command, strlen(command));

    /*Set disable_ipv6 flag to 0 for Bridge Iface */
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /*Set accept_ra flag to 1 for Bridge Iface */
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
  }

  if(QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE &&
     QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
  {
    /* Delete external routes if needed */
    if (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 &&
        QcMapBackhaulCradleMgr->IsCradleBackhaulAvailableV6())
    {
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
    }

    QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_CRADLE,
                                                   true, true);
  }

  qcmap_cm_process_cradle_ind(CRADLE_DISCONNECTED);

  return;
}

/*=====================================================
FUNCTION DisableCradle
======================================================*/
/*!
@brief
Bring down the Cradle iface

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
/*=====================================================*/

void QCMAP_Backhaul_Cradle::DisableCradle(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);


  LOG_MSG_INFO1("Entering DisableCradle",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }
  if(!QcMapBackhaulCradleMgr)
  {
    LOG_MSG_ERROR("\n Cradle object not created.\n",0,0,0);
    return;
  }
  if ( !(QcMapBackhaulCradleMgr->cradle_v4_available ||
         QcMapBackhaulCradleMgr->cradle_v6_available))
  {
    LOG_MSG_ERROR("\n Cradle is not connected.\n",0,0,0);
    return;
  }// can remove this if the above check is the same

  if ( QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_DISABLED_V01 )
  {
    LOG_MSG_ERROR("\n Cradle is already in disabled state.\n",0,0,0);
    return;
  }

  if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    if (QcMapBackhaulCradleMgr->cradle_v4_available &&
        (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false))
    {
      /* Disable NAT on A5 and clean IPV4 firewall */
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }

    if (QcMapBackhaulCradleMgr->cradle_v6_available)
    {
      /* Disable IPV6 Forwarding and firewall */
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);

      if ((QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false))
        QcMapFirewall->CleanIPv6MangleTable();

      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
    }
  }

  /*Reset Cradle available flag*/
  QcMapBackhaulCradleMgr->cradle_v4_available = false;
  QcMapBackhaulCradleMgr->cradle_v6_available = false;

  /*Set accept_ra flag to 1*/
  if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -D FORWARD -p IPv4 --ip-proto udp --ip-dport 67 -j DROP");
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -D FORWARD -p IPv4 --ip-proto udp --ip-dport 68 -j DROP");
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
  }
  else
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
            ECM_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             ECM_IFACE);
    ds_system_call(command, strlen(command));
  }

  /* stop dhcp client on ecm0 */
  LOG_MSG_INFO1("Killing dhcpcd process.\n",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
           ECM_IFACE);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ECM_IFACE);
  ds_system_call(command, strlen(command));

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true)
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);

  /* Disable IPv4 MCAST */
  LOG_MSG_INFO1(" Stop PIMD to disable MCAST forwarding on Cradle interface.\n",0,0,0);
  QcMapMgr->StopMcastDaemon();

  return;
}

/*===========================================================================
  FUNCTION IsCradleBackhaulAvailableV4
==========================================================================*/
/*!
@brief
  Utility API that Checks if Cradle is Enabled and Cradle Backhaul is
  available for v4 connectivity

@parameters
  uint8_t *status

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
boolean QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4()
{
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  if((NULL != QcMapBackhaulCradleMgr) && QcMapBackhaulCradleMgr->cradle_v4_available)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*===========================================================================
  FUNCTION IsCradleBackhaulAvailableV6
==========================================================================*/
/*!
@brief
  Utility API that Checks if Cradle is Enabled and Cradle Backhaul is
  available for v6 connectivity

@parameters
  uint8_t *status

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
boolean QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6()
{
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  if((NULL != QcMapBackhaulCradleMgr) && QcMapBackhaulCradleMgr->cradle_v6_available)
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*=====================================================
  FUNCTION SwitchToCradleBackhaul()
======================================================*/
/*!
@brief
  Switches current backhaul to Cradle

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================*/

void QCMAP_Backhaul_Cradle::SwitchToCradleBackhaul()
{
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (NULL != QcMapBackhaulMgr)
    QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_CRADLE;

  if (QcMapBackhaulCradleMgr->cradle_v4_available)
  {
    if (!QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
      QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V4, ECM_IFACE);

    QcMapNatAlg->EnableNATonApps();
  }
  if (QcMapBackhaulCradleMgr->cradle_v6_available)
  {
    if (!QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated() && QcMapBackhaulMgr)
      QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V6, ECM_IFACE);

    if(QcMapBackhaulMgr)
      QcMapBackhaulMgr->EnableIPV6Forwarding();

    if(QcMapFirewall)
      QcMapFirewall->EnableIPV6Firewall();
  }
  /* Enable IPv4 MCAST */
  LOG_MSG_INFO1(" Restart PIMD to enable MCAST forwarding for CRADLE \n",0,0,0);
  QcMapMgr->StopMcastDaemon();
  QcMapMgr->StartMcastDaemon();
}
