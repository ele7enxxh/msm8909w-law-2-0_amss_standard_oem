/******************************************************************************
  @file: loc_wle.h
  @brief:   This module handles interface between location middleware and WLE.
  

  DESCRIPTION
   Qualcomm Location API for WLE feature

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_wle.h#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
04/30/15   dc      Initial version
======================================================================*/

#ifndef LOC_WLE_H
#define LOC_WLE_H

#include "loc_client.h"
#include "aries_os_api.h"

/*===========================================================================
FUNCTION loc_wle_ApCacheIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectApCacheDataIndMsgT_v02 
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/

void loc_wle_ApCacheIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_wle_ApCacheIndProxy

DESCRIPTION
  This function Sends wle ApCache indication 
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Same transaction id passed in earlier by the request
  (input)
 
  p_ind  - The QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
uint32 loc_wle_ApCacheIndProxy(const int32 client_handle, 
                                            const uint32 transaction_id, 
                                            const qmiLocInjectApCacheDataIndMsgT_v02 * const p_ind);

/*===========================================================================
FUNCTION loc_wle_NoApCacheIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectNoApCacheDataIndMsgT_v02 
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/

void loc_wle_NoApCacheIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_wle_NoApCacheIndProxy

DESCRIPTION
  This function Sends wle NoApCache indication 
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Same transaction id passed in earlier by the request
  (input)
 
  p_ind  - The QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
uint32 loc_wle_NoApCacheIndProxy(const int32 client_handle, 
                                 const uint32 transaction_id, 
                                 const qmiLocInjectApDoNotCacheDataIndMsgT_v02 * const p_ind);

/*===========================================================================
FUNCTION loc_wle_InjectApCacheDataReq

DESCRIPTION
  This function handles QMI command to inject WLE ApCache data 
 
PARAMETERS 
 
  cpz_Client - Points to LocAPI client 
  (input)

  cpz_Req  - Points to QMI request
  (input)
 
  pz_Resp  - Points to QMI response
  (output)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
int32 loc_wle_InjectApCacheDataReq(const loc_client_info_s_type *const cpz_Client,
                                               const qmiLocInjectApCacheDataReqMsgT_v02 *const cpz_Req,
                                               qmiLocGenRespMsgT_v02 *const pz_Resp);

/*===========================================================================
FUNCTION loc_wle_InjectNoApCacheDataReq

DESCRIPTION
  This function handles QMI command to inject WLE NoApCache data 
 
PARAMETERS 
 
  cpz_Client - Points to LocAPI client 
  (input)

  cpz_Req  - Points to QMI request
  (input)
 
  pz_Resp  - Points to QMI response
  (output)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
int32 loc_wle_InjectNoApCacheDataReq(const loc_client_info_s_type *const cpz_Client,
                                     const qmiLocInjectApDoNotCacheDataReqMsgT_v02 *const cpz_Req,
                                     qmiLocGenRespMsgT_v02 *const pz_Resp);

#endif /* LOC_WLE_H */
