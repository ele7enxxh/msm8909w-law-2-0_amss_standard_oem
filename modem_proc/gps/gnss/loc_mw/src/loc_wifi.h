/******************************************************************************
  @file: loc_wifi.h
  @brief: Location API WiFi positioning module

  DESCRIPTION
   Qualcomm Location API WiFi Positioning Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_wifi.h#1 $
$DateTime: 2016/12/13 07:59:45 $

******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
01/11/16   yh       Loc_Wifi cleanup
07/24/09   dx       Initial version
======================================================================*/

#ifndef LOC_WIFI_H
#define LOC_WIFI_H

#include "loc_api_2.h"
#include "aries_os_api.h"

typedef struct
{
   loc_client_handle_type      l_ClientHandle;
   uint32                      q_QmiLocMsgId;
}locWifi_ModuleDataType;

typedef struct
{
   loc_client_handle_type l_ClientHandle;
   uint32                 q_CmdType;      /* QMI_LOC MsgID */
   uint32                 q_CmdInfo;      /* QMI_LOC Data, not used */
   uint32                 q_ParamStatus;
}locWifi_IndMsgType;

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to WIFI module

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locWifi_ProcessQmiRequest
(
   const loc_client_handle_type l_ClientHandle,
   const uint32                 q_QmiLocMsgId,
   const void*                  p_QmiLocMsgData,
   const uint32                 q_QmiLocMsgLen
);

/*---------------------------------------------------------------------------
@brief
  Function to send QMI_LOC indication to client for previous QMI_LOC request
  related to WIFI module

@param[in] p_IpcMsg : IPC Message containing the QMI_LOC indication data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locWifi_SendQmiIndication
(
   const os_IpcMsgType* const p_IpcMsg
);

boolean locWifi_Init(void);
#endif /* LOC_WIFI_H */
