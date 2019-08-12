/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           CMCNas_MN.c -- Internal modem api definitions

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/CMCNas/src/CMCNas_MN.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "stringl.h"

#if defined(FEATURE_GWSMS)

#include "CMCNas_MN.h"
#include "msg.h"

#ifdef FEATURE_CMI
#include "mn_cnmtask_v.h"
#include "mmtask_v.h"
#ifdef FEATURE_GWSMS_BROADCAST
#include "cb_task_v.h"
#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_CMI */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#include "wmsutils.h"

//==============================================================================
// Static Variable for MN Implementation
//==============================================================================
static CMCNas gCMCNas_MN;

//==============================================================================
// MN Queue Callback Function
//==============================================================================
static gs_status_T CMCNas_send_routine(
  void                              *msg_p
)
{
  CMCNas_Rpt *pCMCNasRpt = (CMCNas_Rpt*)msg_p;

  switch (pCMCNasRpt->message_header.message_id)
  {
    case UASMS_SM_DATA_IND:
      if (NULL != gCMCNas_MN.pfnSMCb)
      {
        return gCMCNas_MN.pfnSMCb(msg_p);
      }
      break;

    case UASMS_SM_REPORT_IND:
      // We let the wmscmi_send_routine() handle the SMMA report ind
      if ( (0xFFFF == pCMCNasRpt->rl_report_ind.short_message_identifier) &&
           (NULL != gCMCNas_MN.pfnLinkCb) )
      {
        return gCMCNas_MN.pfnLinkCb(msg_p);
      }
      else if (NULL != gCMCNas_MN.pfnSMCb)
      {
        return gCMCNas_MN.pfnSMCb(msg_p);
      }
      break;

#ifdef FEATURE_GWSMS_BROADCAST
    case CB_DATA_IND:
    case CB_CELL_CHANGE_IND:
    case CB_DUPLICATION_DETECTION_IND:
    case CB_ERROR_IND:
#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)
    case CB_ETWS_PRIM_NOTIFICATION_IND:
    case CB_ETWS_SEC_NOTIFICATION_IND:
#endif
      if (NULL != gCMCNas_MN.pfnCBCb)
      {
        return gCMCNas_MN.pfnCBCb(msg_p);
      }
      break;
#endif /* FEATURE_GWSMS_BROADCAST */

    case UASMS_SM_LINK_CONTROL_IND:
      if (NULL != gCMCNas_MN.pfnLinkCb)
      {
        return gCMCNas_MN.pfnLinkCb(msg_p);
      }
      break;

    default:
      break;
  }

  return GS_FAILURE;
} /* CMCNas_send_routine() */


//==============================================================================
// NAS Function Implementation to Send Data
//==============================================================================
static CMCNas_Err CMCNas_MN_SendData(
  sys_modem_as_id_e_type             as_id,
  nas_msg_transport_type_e_type      transport_type,
  cm_called_party_bcd_no_T          *pSCAddress,
  uint32                             uMsgID,
  uint32                             uTPDULength,
  uint8                             *pTPDUData
)
{
  mn_cnm_cmd_type *mn_cmd_ptr;

  if (uTPDULength > sizeof(mn_cmd_ptr->cmd.sm_rl_data_req.tpdu))
  {
    MSG_ERROR_0("TPDU length exceeded!");
    return CMC_NAS_ERR_BAD_PARM;
  }

  mn_cmd_ptr = mn_uasms_get_cmd_buf(UASMS_SM_DATA_REQ);

  if (NULL == mn_cmd_ptr)
  {
    MSG_ERROR_0("MN out of Command Buffer!");
    return CMC_NAS_ERR_OUT_OF_RESOURCES;
  }

  // Prepare for the message and send it to MN
  (void)memscpy(&mn_cmd_ptr->cmd.sm_rl_data_req.RP_DA,
                sizeof(cm_called_party_bcd_no_T),
                pSCAddress,
                sizeof(cm_called_party_bcd_no_T));

  mn_cmd_ptr->cmd.sm_rl_data_req.short_message_identifier = (word)uMsgID;
  mn_cmd_ptr->cmd.sm_rl_data_req.tpdu_length              = (uint8)uTPDULength;
  mn_cmd_ptr->cmd.sm_rl_data_req.as_id                    = as_id;
  mn_cmd_ptr->cmd.sm_rl_data_req.sms_transport_type       = transport_type;

  (void)memscpy(mn_cmd_ptr->cmd.sm_rl_data_req.tpdu,
                sizeof(mn_cmd_ptr->cmd.sm_rl_data_req.tpdu),
                pTPDUData,
                mn_cmd_ptr->cmd.sm_rl_data_req.tpdu_length);

  mn_cnm_put_cmd( mn_cmd_ptr);

  return CMC_NAS_ERR_NONE;
} /* CMCNas_MN_SendData() */

//==============================================================================
// NAS Function Implementation for Bearer Selection
//==============================================================================
static CMCNas_Err CMCNas_MN_SetBearerSelection(
  sys_modem_as_id_e_type             as_id,
  mm_sms_bearer_selection_enum_T     domain_pref
)
{
  mm_cmd_type *mm_cmd_ptr;

  mm_cmd_ptr = mm_get_cmd_buf();

  if (NULL == mm_cmd_ptr)
  {
    MSG_ERROR_0("MM out of cmd buffer!");
    return CMC_NAS_ERR_OUT_OF_RESOURCES;
  }

  /* Header info */
  mm_cmd_ptr->cmd.hdr.message_set     = MS_WMS_MM;
  mm_cmd_ptr->cmd.hdr.message_id      = MM_SMS_BEARER_SELECTION_CMD;
  mm_cmd_ptr->cmd.hdr.message_len_lsb = 1;
  mm_cmd_ptr->cmd.hdr.message_len_msb = 0;

  /* MM cmd data */
  mm_cmd_ptr->cmd.sms_bearer_selection_req.sms_bearer_selection = domain_pref;
  mm_cmd_ptr->cmd.sms_bearer_selection_req.as_id                = as_id;

  mm_put_cmd( mm_cmd_ptr );

  MSG_HIGH_1("GW Domain Pref = %d sent to MM", domain_pref);

  return CMC_NAS_ERR_NONE;
} /* CMCNas_MN_SetBearerSelection() */

//==============================================================================
// NAS Function Implementation for SMS status indication
//==============================================================================
static CMCNas_Err CMCNas_MN_SetSMSStatus(
  wms_mm_sms_call_info_T           wms_mm_sms_status
)
{
  mm_cmd_type *mm_cmd_ptr;

  mm_cmd_ptr = mm_get_cmd_buf();

  if (NULL == mm_cmd_ptr)
  {
    MSG_ERROR_0("MM out of cmd buffer!");
    return CMC_NAS_ERR_OUT_OF_RESOURCES;
  }

  /* MM cmd data */
  mm_cmd_ptr->cmd.wms_mm_sms_call_info = wms_mm_sms_status;

  /* Header info */
  mm_cmd_ptr->cmd.hdr.message_set     = MS_WMS_MM;
  mm_cmd_ptr->cmd.hdr.message_id      = MM_SMS_STATUS_CMD;
  mm_cmd_ptr->cmd.hdr.message_len_lsb = 1;
  mm_cmd_ptr->cmd.hdr.message_len_msb = 0;

  mm_put_cmd( mm_cmd_ptr );

  MSG_HIGH_0("SMS Status sent to MM");

  return CMC_NAS_ERR_NONE;
} /* CMCNas_MN_SetSMSStatus() */

//==============================================================================
// NAS Function Implementation to Send Report
//==============================================================================
static CMCNas_Err CMCNas_MN_SendReport(
  sys_modem_as_id_e_type             as_id,
  uint32                             uConnectionID,
  boolean                            bSuccess,
  uint8                              uRPCause,
  uint32                             uTPDULength,
  uint8                             *pTPDUData
)
{
  mn_cnm_cmd_type *mn_cmd_ptr;

  if (uTPDULength > sizeof(mn_cmd_ptr->cmd.sm_rl_report_req.tpdu))
  {
    MSG_ERROR_0("TPDU length exceeded!");
    return CMC_NAS_ERR_BAD_PARM;
  }

  mn_cmd_ptr = mn_uasms_get_cmd_buf(UASMS_SM_REPORT_REQ);

  if (NULL == mn_cmd_ptr)
  {
    MSG_ERROR_0("MN out of Command Buffer!");
    return CMC_NAS_ERR_OUT_OF_RESOURCES;
  }

  // Prepare for the Report and send it to MN
  mn_cmd_ptr->cmd.sm_rl_report_req.sm_rl_success = bSuccess;

  if (FALSE == bSuccess)
  {
    mn_cmd_ptr->cmd.sm_rl_report_req.cause_value = uRPCause;
  }

  mn_cmd_ptr->cmd.sm_rl_report_req.diagnostics_present  = FALSE;
  mn_cmd_ptr->cmd.sm_rl_report_req.connection_id        = (connection_id_T)uConnectionID;
  mn_cmd_ptr->cmd.sm_rl_report_req.tpdu_length          = (uint8)uTPDULength;
  mn_cmd_ptr->cmd.sm_rl_report_req.as_id                = as_id;

  (void)memscpy(mn_cmd_ptr->cmd.sm_rl_report_req.tpdu,
                sizeof(mn_cmd_ptr->cmd.sm_rl_report_req.tpdu),
                pTPDUData,
                mn_cmd_ptr->cmd.sm_rl_report_req.tpdu_length);

  mn_cnm_put_cmd( mn_cmd_ptr);

  return CMC_NAS_ERR_NONE;
} /* CMCNas_MN_SendReport() */

//==============================================================================
// NAS Function Implementation to Send Memory Available
//==============================================================================
static CMCNas_Err CMCNas_MN_SendMemoryAvailable(
  sys_modem_as_id_e_type             as_id,
  word                               wMsgId,
  boolean                            bAbort,
  nas_msg_transport_type_e_type      transport_type
)
{
  mn_cnm_cmd_type *mn_cmd_ptr;

  mn_cmd_ptr = mn_uasms_get_cmd_buf(UASMS_MEMORY_AVAIL_NOTIFICATION_REQ);

  if (NULL == mn_cmd_ptr)
  {
    MSG_ERROR_0("MN out of Command Buffer!");
    return CMC_NAS_ERR_OUT_OF_RESOURCES;
  }

  // Prepare for the Memory Available and send it to MN
  mn_cmd_ptr->cmd.sm_rl_memory_available_req.short_message_identifier = wMsgId;
  mn_cmd_ptr->cmd.sm_rl_memory_available_req.abort                    = bAbort;
  mn_cmd_ptr->cmd.sm_rl_memory_available_req.as_id                    = as_id;
  mn_cmd_ptr->cmd.sm_rl_memory_available_req.sms_transport_type       = transport_type;

  mn_cnm_put_cmd( mn_cmd_ptr);

  return CMC_NAS_ERR_NONE;
} /* CMCNas_MN_SendMemoryAvailable() */

//==============================================================================
// Function to Create Instance of CMCNas for MN and Return Object
//
// NOTE: Use NULL for CMCNas_Cb is not interested
//==============================================================================
boolean CMCNas_MN_New(
  CMCNas_Cb pfnSM,
  CMCNas_Cb pfnLink,
  CMCNas_Cb pfnCB,
  CMCNas**ppOut
)
{
  // Assign Static NAS Object
  CMCNas *me = &gCMCNas_MN;

  // Assign Functions only if not NULL
  if (NULL != pfnSM)
  {
    me->pfnSMCb = pfnSM;
  }

  if (NULL != pfnLink)
  {
    me->pfnLinkCb = pfnLink;
  }

  if (NULL != pfnCB)
  {
    me->pfnCBCb = pfnCB;
  }

  me->pfnSendData            = CMCNas_MN_SendData;
  me->pfnSendReport          = CMCNas_MN_SendReport;
  me->pfnSendMemoryAvailable = CMCNas_MN_SendMemoryAvailable;
  me->pfnSetBearerSelection  = CMCNas_MN_SetBearerSelection;
  me->pfnSetSMSStatus        = CMCNas_MN_SetSMSStatus;

  {
    /* Initialize gs message passing for this queue */
    gs_status_T  gs_status;

    gs_status = gs_init_for_send_only(GS_QUEUE_UASMS, CMCNas_send_routine);

    if (GS_SUCCESS != gs_status)
    {
      MSG_ERROR_1("gs_init_for_send_only() failed: %d", gs_status);
      return FALSE;
    }
  }

  *ppOut = (void *)me;

  return TRUE;
} /* CMCNas_MN_New() */

#endif /* FEATURE_GWSMS */