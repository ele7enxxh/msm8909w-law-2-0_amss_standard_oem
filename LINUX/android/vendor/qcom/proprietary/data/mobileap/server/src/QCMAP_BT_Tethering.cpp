/*====================================================

FILE:  QCMAP_BT_Tethering.cpp

SERVICES:
   QCMAP Connection Manager BT Tethering Specific Implementation

=====================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  06/13/16   c_surajj         Created

===========================================================================*/
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"

#include "QCMAP_BT_Tethering.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_LAN.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_NATALG.h"
#include "ds_qmi_qcmap_msgr.h"

bool QCMAP_BT_TETHERING::flag= false;
QCMAP_BT_TETHERING* QCMAP_BT_TETHERING::object=NULL;

/*===================================================================
         Class Definitions
===================================================================*/

/*===========================================================================
FUNCTION  QCMAP_BT_TETHERING
==========================================================================*/
/*!
@brief
QCMAP_BT_TETHERING

@return
none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

QCMAP_BT_TETHERING::QCMAP_BT_TETHERING()
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  return;
}

/*===========================================================================
FUNCTION  ~QCMAP_BT_TETHERING
==========================================================================*/
/*!
@brief
~QCMAP_BT_TETHERING.

@return
none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

QCMAP_BT_TETHERING::~QCMAP_BT_TETHERING()
{
  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: BT Tethering",0,0,0);
}

/*===========================================================================
FUNCTION  Get_Instance
==========================================================================*/
/*!
@brief
Get_Instance.

@parameters
boolean obj_create

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

QCMAP_BT_TETHERING* QCMAP_BT_TETHERING::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : BT Tethering",0, 0, 0);
    object = new QCMAP_BT_TETHERING();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}


/*===========================================================================
FUNCTION  BringupBTTethering
==========================================================================*/
/*!
@brief
Bring up BT tethering.

@parameters
qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_BT_TETHERING::BringupBTTethering( qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  QCMAP_MediaService* QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 mode;
  qcmap_media_svc_status dlna_mode=MEDIA_SVC_MODE_DOWN;
  qcmap_media_svc_status upnp_mode=MEDIA_SVC_MODE_DOWN;
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_msgr_station_mode_config_v01 station_config;
  int qmi_err_num_internal;
  char command[MAX_COMMAND_STR_LEN];

  char devmode[16] = { '\0' };
  QCMAP_CM_LOG_FUNC_ENTRY();

  ds_system_call("echo QCMAP:Start Bring up BT tetherin > /dev/kmsg",\
  strlen("echo QCMAP:Start Bring up BT tetherin > /dev/kmsg"));

  /* If mobileap state state is invalid*/
  if ( QcMapMgr->qcmap_cm_handle <= 0 )
  {
    LOG_MSG_ERROR("\n MobileAP not enabled\n", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }


  if (QcMapBTTethMgr && QcMapBTTethMgr->bt_tethering_status ==
      QCMAP_MSGR_BT_TETHERING_MODE_UP_V01)
  {
    LOG_MSG_ERROR("\n BT Tethering is already UP \n", 0 ,0, 0);
    return true;
  }


  QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(true);//Creating BT Tethering object here

  /* Update Current BT Tethering mode. */
  QcMapBTTethMgr->bt_tethering_status = QCMAP_MSGR_BT_TETHERING_MODE_UP_V01;

  // bring up the BT Tethering
  snprintf(command,MAX_COMMAND_STR_LEN,"brctl addif bridge0 bt-pan");
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,
           "ifconfig bt-pan 169.254.5.1 netmask 255.255.255.0 up");
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,
           "route del -net 169.254.5.1 netmask 255.255.255.0 dev bt-pan");
  ds_system_call(command,strlen(command));

  //remove ipv6 LL route
  snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev bt-pan");
  ds_system_call(command, strlen(command));

  ds_system_call("echo QCMAP:BT Tethering Brought UP > /dev/kmsg",
                 strlen("echo QCMAP:BT Tethering Brought UP > /dev/kmsg"));

  return true;
}

/*=====================================================================
FUNCTION BringdownBTTethering
======================================================================*/
/*!
@brief
Bring down BT Tethering.

@parameters
qmi_error_type_v01 *qmi_err_num

@return
true  - on success
false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*===================================================================*/

boolean QCMAP_BT_TETHERING::BringdownBTTethering( qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  qcmap_msgr_wlan_mode_enum_v01     prev_wifi_mode;
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_MediaService* QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);



  if (!QcMapBTTethMgr || QcMapBTTethMgr->bt_tethering_status ==
      QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01)
  {
    LOG_MSG_ERROR("\n BT Tethering already down.\n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return true;
  }

  // bring down the BT Tethering
  snprintf(command,MAX_COMMAND_STR_LEN,"brctl delif bridge0 bt-pan");
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,
           "ifconfig bt-pan down");
  ds_system_call(command,strlen(command));

  ds_system_call("echo QCMAP:BT Tethering Brought DOWN > /dev/kmsg",
                 strlen("echo QCMAP:BT Tethering Brought DOWN > /dev/kmsg"));


  /* Reset BT Tethering mode */
  QcMapBTTethMgr->bt_tethering_status = QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01;

  delete QcMapBTTethMgr;//deleting bt tethering  object here
  return true;
}


/*=====================================================================
FUNCTION GetBTTetheringStatus
======================================================================*/
/*!
@brief
Gets the wlan configured mode.

@parameters
qcmap_msgr_wlan_mode_enum_v01 *wlan_mode

@return
true- on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_BT_TETHERING::GetBTTetheringStatus
(
qcmap_msgr_bt_tethering_status_enum_v01 *bt_teth_mode,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  if (bt_teth_mode == NULL )
  {
    LOG_MSG_ERROR("NULL params.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if(QcMapBTTethMgr)
  {
    *bt_teth_mode = QcMapBTTethMgr->bt_tethering_status;
  }
  else
  {
    *bt_teth_mode = QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01 ;
  }

  return true;
}


