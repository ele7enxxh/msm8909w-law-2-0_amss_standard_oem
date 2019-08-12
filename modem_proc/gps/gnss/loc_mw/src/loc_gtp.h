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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_gtp.h#1 $
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

#ifndef LOC_GTP_H
#define LOC_GTP_H

#include "loc_client.h"

/*===========================================================================
FUNCTION loc_gtp_InjectGtpClientDownloadedDataReq

DESCRIPTION
  This function handles QMI command to inject GTP client downloaded data 
 
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
int32 loc_gtp_InjectGtpClientDownloadedDataReq(const loc_client_info_s_type *const cpz_Client,
                                               const qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 *const cpz_Req,
                                               qmiLocGenRespMsgT_v02 *const pz_Resp);

/*===========================================================================
FUNCTION loc_gtp_WwanClientDloadedDataIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 
 
PARAMETERS 
 
  p_ipc_msg_in - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gtp_WwanClientDloadedDataIndHandler(const os_IpcMsgType *const p_ipc_msg_in);

/*===========================================================================
FUNCTION loc_gtp_ClientDownloadedDataIndProxy

DESCRIPTION
  This function Sends gtp ClientDownloadedData indication 
 
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
uint32 loc_gtp_ClientDownloadedDataIndProxy(const int32 client_handle, 
                                            const uint32 transaction_id, 
                                            const qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 * const p_ind);

#endif /* LOC_GTP_H */
