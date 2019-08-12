#ifndef CMCNAS_H
#define CMCNAS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           CMCNas.h -- Internal modem api definitions

  This file contains WMS API function prototypes used internally within
  the MODEM AU.

  -------------

    Copyright (c) 2008,2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/CMCNas/inc/CMCNas.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_GWSMS)

//==============================================================================
// Include Files
//==============================================================================
#include "cm.h"
#include "nas_exp.h"
#include "mn_uasms.h"
#include "gs.h"
#include "sms_gmm.h"
#include "cbwmsif.h"

//==============================================================================
// Type Definitions
//==============================================================================
typedef  union {
   IMH_T                                 message_header;
   SM_RL_DATA_IND_T                      rl_data_ind;
   SM_RL_REPORT_IND_T                    rl_report_ind;
   SM_RL_LINK_CONTROL_REPORT_T           rl_link_report_ind;
#ifdef FEATURE_GWSMS_BROADCAST
   cb_data_ind_type                      cb_data_ind;
   cb_cell_change_ind_type               cb_cell_change;
   cb_duplication_detection_ind_type     cb_duplicate_ind;
   cb_error_indication_type              cb_error_ind;
#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)
   cb_etws_prim_notification_ind_type    cb_etws_prim_notification_ind;
   cb_etws_sec_notification_ind_type     cb_etws_sec_notification_ind;
#endif /* defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS) */
#endif /* FEATURE_GWSMS_BROADCAST */
} CMCNas_Rpt;

typedef enum {
   CMC_NAS_ERR_NONE,
   CMC_NAS_ERR_OUT_OF_RESOURCES,
   CMC_NAS_ERR_BAD_PARM,
   CMC_NAS_ERR_UNSUPPORTED
} CMCNas_Err;

//==============================================================================
// Type Definitions - Function Pointers
//==============================================================================
typedef gs_status_T (*CMCNas_Cb)(CMCNas_Rpt*);

typedef CMCNas_Err (*CMCNasImpl_SendData)(sys_modem_as_id_e_type         as_id,
                                          nas_msg_transport_type_e_type  transport_type,
                                          cm_called_party_bcd_no_T      *pSCAddress,
                                          uint32                         uMsgId,
                                          uint32                         uTPDULength,
                                          uint8                         *pTPDUData);

typedef CMCNas_Err (*CMCNasImpl_SendReport)(sys_modem_as_id_e_type   as_id,
                                            uint32                   uConnectionID,
                                            boolean                  bSuccess,
                                            uint8                    uRPCause,
                                            uint32                   uTPDULength,
                                            uint8                   *pTPDUData);

typedef CMCNas_Err (*CMCNasImpl_SendMemoryAvailable)(sys_modem_as_id_e_type     as_id,
                                                     word                           wMsgId,
                                                     boolean                        bAbort,
                                                     nas_msg_transport_type_e_type  transport_type);

typedef CMCNas_Err (*CMCNasImpl_SetBearerSelection)(sys_modem_as_id_e_type           as_id,
                                                    mm_sms_bearer_selection_enum_T   domain_pref);

typedef CMCNas_Err (*CMCNasImpl_SetSMSStatus)(wms_mm_sms_call_info_T);

//==============================================================================
// Type Definitions - CMCNas Structure
//==============================================================================
typedef struct
{
   CMCNas_Cb                      pfnSMCb;
   CMCNas_Cb                      pfnLinkCb;
   CMCNas_Cb                      pfnCBCb;
   CMCNasImpl_SendData            pfnSendData;
   CMCNasImpl_SendReport          pfnSendReport;
   CMCNasImpl_SendMemoryAvailable pfnSendMemoryAvailable;
   CMCNasImpl_SetBearerSelection  pfnSetBearerSelection;
   CMCNasImpl_SetSMSStatus        pfnSetSMSStatus;
} CMCNas;

//==============================================================================
// Generic Function to Send Data
//==============================================================================
static __inline
CMCNas_Err CMCNas_SendData(
  CMCNas                        *me,
  sys_modem_as_id_e_type         as_id,
  nas_msg_transport_type_e_type  transport_type,
  cm_called_party_bcd_no_T      *pSCAddress,
  uint32                         uMsgID,
  uint32                         uTPDULength,
  uint8                         *pTPDUData
)
{
   return me->pfnSendData( as_id, transport_type, pSCAddress, uMsgID, uTPDULength, pTPDUData );
}

//==============================================================================
// Generic Function to Send Report
//==============================================================================
static __inline
CMCNas_Err CMCNas_SendReport(
  CMCNas                   *me,
  sys_modem_as_id_e_type    as_id,
  uint32                    uConnectionID,
  boolean                   bSuccess,
  uint8                     uRPCause,
  uint32                    uTPDULength,
  uint8                    *pTPDUData
)
{
   return me->pfnSendReport( as_id, uConnectionID, bSuccess, uRPCause, uTPDULength, pTPDUData );
}

//==============================================================================
// Generic Function to Send Memory Available
//==============================================================================
static __inline
CMCNas_Err CMCNas_SendMemoryAvailable(
  CMCNas                        *me,
  sys_modem_as_id_e_type         as_id,
  word                           wMsgId,
  boolean                        bAbort,
  nas_msg_transport_type_e_type  transport_type
)
{
   return me->pfnSendMemoryAvailable( as_id, wMsgId, bAbort, transport_type );
}

//==============================================================================
// Generic Function to set Domain Selection
//==============================================================================
static __inline
CMCNas_Err CMCNas_SetBearerSelection(
  CMCNas                           *me,
  sys_modem_as_id_e_type            as_id,
  mm_sms_bearer_selection_enum_T    domain_pref
)
{
    return me->pfnSetBearerSelection( as_id, domain_pref );
}

//==============================================================================
// Generic Function to send sms status indication to MM.
//==============================================================================
static __inline
CMCNas_Err CMCNas_Send_sms_status_to_mm(
  CMCNas                           *me,
  wms_mm_sms_call_info_T            mm_sms_status
)
{
    return me->pfnSetSMSStatus(mm_sms_status);
}

#endif /* FEATURE_GWSMS */

#endif /* CMCNAS_H */
