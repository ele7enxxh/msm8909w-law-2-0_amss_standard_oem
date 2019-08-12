/******************************************************************************
  @file: loc_gtp.h
  @brief:   This module handles interface between location middleware and GTP.
  

  DESCRIPTION
   Qualcomm Location API for GTP feature

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
Copyright (c) 2015 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/inc/loc_gdt.h#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/11/14   ah      Initial version
======================================================================*/

#ifndef LOC_GDT_H
#define LOC_GDT_H

#include "loc_client.h"
#include "aries_os_api.h"
#include "gnss_gdt.h"


/*===========================================================================
FUNCTION loc_gdt_UploadBeginStatusReq

DESCRIPTION
  This function handles QMI command to process gdt upload begin status request.
  MP has sent a session begin to AP, and now the response from AP is recieved
  through this request
 
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS:          If the command has been handled successfully
  LOC_API_GENERAL_FAILURE : Otherwise
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_UploadBeginStatusReq(const loc_client_info_s_type * cpz_Client,
                                   const qmiLocGdtUploadBeginStatusReqMsgT_v02 *const cpz_Req,
                                   qmiLocGenRespMsgT_v02 *const pz_Resp);

/*===========================================================================
FUNCTION loc_gdt_UploadEndStatusReq

DESCRIPTION
  This function handles QMI command to process gdt upload end status request 
 
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS:          If the command has been handled successfully
  LOC_API_GENERAL_FAILURE : Otherwise
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_UploadEndStatusReq(const loc_client_info_s_type *const cpz_Client,
                                 const qmiLocGdtUploadEndReqMsgT_v02 *const cpz_Req,
                                 qmiLocGenRespMsgT_v02 *const pz_Resp);

/*===========================================================================
FUNCTION loc_gdt_SendBeginRequest

DESCRIPTION
  This function sends a GDT begin indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDT session ID
  (input)
 
  q_FilePathLen - The number valid chars in file path
  (input)
 
  c_FilePath  - Points to the file path
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the access request
                 to AP to get the permission
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_SendBeginRequest(e_GDTServiceIDType e_ServiceId,
                                              uint32 q_SessionId, uint32 q_FilePathLen,
                                              const char * c_FilePath);

/*===========================================================================
FUNCTION loc_gdt_SendEndRequest

DESCRIPTION
  This function sends a GDT END indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDT session ID
  (input)
 
  e_Status   - End Status (session succeeded or aborted)
  (input)
 
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the indication
                 to AP
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_SendEndRequest(e_GDTServiceIDType e_ServiceId,
                                            uint32 q_SessionId, 
                                            e_GDTResponseType e_Status);

/*===========================================================================
FUNCTION loc_gdt_BeginIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02.
  MP is sending a GDT begin message as an indication to AP.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if ipc has been processed successfully  
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_BeginIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_gdt_EndIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadEndIndMsgT_v02
  MP is sending a GDT end message as an indication to AP.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_EndIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_gdt_EndReqIndProxy

DESCRIPTION
  This function sends gdt end request status indication.
  MP has recievd a request from AP and now this function sends the corresponding
  indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_EndReqIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtUploadEndIndMsgT_v02 * const p_ind
);

/*===========================================================================
FUNCTION loc_gdt_BeginReqIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadBeginStatusIndMsgT_v02
  MP has received the request for GDT begin response from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_BeginReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_gdt_EndReqIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadEndIndMsgT_v02 
  MP has received the request for GDT end response from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_EndReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_gdt_BeginReqIndProxy

DESCRIPTION
  This function sends gdt begin request status indication.
  MP has recievd a request from AP and now this function sends the corresponding
  indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_BeginReqIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtUploadBeginStatusIndMsgT_v02 * const p_ind
);
#endif /* LOC_GDT_H */
