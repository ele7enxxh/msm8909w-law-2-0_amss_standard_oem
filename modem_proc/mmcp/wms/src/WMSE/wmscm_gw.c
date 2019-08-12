/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscm_gw.c -- GW Interfaces with CM and NAS

  The WMS module which implements the User API for SMS. This source file
  implements the transport interfaces with Call Manager and NAS layers.

  -------------

    Copyright (c) 2008-2013 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscm_gw.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_GWSMS)
#include "wmscm_gw.h"
#include "msg.h"

#include "wmstsi.h"
#include "wmscm.h"
#include "wmsmsg.h"
#include "wmscfg.h"
#include "wmsutils.h"
#include "wmssim.h"

#include "sms_gmm.h"
#include "stringl.h"

#ifdef FEATURE_CMI
#include "mn_cnmtask_v.h"
#include "mmtask_v.h"
#ifdef FEATURE_GWSMS_BROADCAST
#include "cb_task_v.h"
#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_CMI */


#ifdef FEATURE_SMS_PWS_OVER_LTE
#include "lte_rrc_ext_msg.h"
#include "wms_msgr_msg.h"
#endif /* FEATURE_SMS_PWS_OVER_LTE */

#ifdef FEATURE_FUZZ_TEST
#include "wmsdbg.h"
#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
=============================================================================
========================  DATA TYPES  =======================================
=============================================================================
===========================================================================*/
static rex_crit_sect_type            wms_rpt_queue_crit_sect;

// Define a variable to hold the WMS task TCB.  This is done for
// a several reasons -- see definition of wms_myself in wms.c
static rex_tcb_type *wms_myself = NULL;  // To be initialized to rex_self()
                                         // when task is running

/* Generic report header type - all reportss to WMS must start with this */
typedef struct
{
  q_link_type          link;         /* Queue link */
  rex_tcb_type         *task_ptr;    /* Pointer to requesting task TCB */
  rex_sigs_type        sigs;         /* Signal to set upon cmd completion */
  q_type               *done_q_ptr;  /* Queue to place this cmd on when done */

} wms_rpt_hdr_type;



/* Union of all Lower layer report types.  The header is always present and*/
/* specifies the report type and attributes.  If the report has            */
/* arguments, they follow in the union.                                    */
typedef struct
{
  wms_rpt_hdr_type                    hdr;       /* header */
  CMCNas_Rpt                          rpt;
} wms_rpt_type;

//==============================================================================
// These globals are used to store NAS indications and subsequently service
// them in wms task context.
//==============================================================================
static q_type                        rpt_q;
static rex_sigs_type                 rpt_sig_mask;
static CMCNas*                       pCMCNas;
static int                           wms_rpts_cnt = WMS_MAX_RPTS;

static __inline
wms_report_status_e_type wmscmi_map_report_status(byte error_status)
{
   wms_report_status_e_type  report_status;

   switch( error_status )
   {
     case NO_ERROR_RP_ACK:
     case NO_ERROR_CP_ACK:   /* TODO : What is CP Ack for, is it OK
                             ** to assume a succesful MO on a CP Ack? */
       report_status = WMS_RPT_OK;
       break;

     case SMS_RP_ERROR:
       report_status = WMS_RPT_RP_ERROR;
       MSG_ERROR_0("Error during msg submit WMS_RPT_RP_ERROR");
       break;

     case SMS_CP_ERROR:
       report_status = WMS_RPT_CP_ERROR;
       MSG_ERROR_0("Error during msg submit WMS_RPT_CP_ERROR");
       break;

     case SMS_LL_ERROR:
       report_status = WMS_RPT_LL_ERROR;
       MSG_ERROR_0("Error during msg submit WMS_RPT_LL_ERROR");
       break;

     case SMS_NO_RESPONSE_FROM_NETWORK:
       report_status = WMS_RPT_NO_RESPONSE_FROM_NETWORK;
       MSG_ERROR_0("Error during msg submit WMS_RPT_NO_RESPONSE_FROM_NETWORK");
       break;


     case SMR_ERROR_NO_SM_RESOURCES:
       report_status = WMS_RPT_SMR_NO_RESOURCES;
       MSG_ERROR_0("Error during msg submit WMS_RPT_SMR_NO_RESOURCES");
       break;

     case SMR_ERROR_TR1M_TIMEOUT:
       report_status = WMS_RPT_SMR_TR1M_TIMEOUT;
       MSG_ERROR_0("Error during msg submit WMS_RPT_SMR_TR1M_TIMEOUT");
       break;

     case SMR_ERROR_TR2M_TIMEOUT:
       report_status = WMS_RPT_SMR_TR2M_TIMEOUT;
       MSG_ERROR_0("Error during msg submit WMS_RPT_SMR_TR2M_TIMEOUT");
       break;

  //   case NO_ERROR_ABORT_SM_COMPLETE:
    //   report_status = WMS_RPT_OK;
      // break;

     case NO_ERROR_SMMA_ABORT_SENT:
       report_status = WMS_RPT_LL_ERROR;
       MSG_ERROR_0("Error during msg submit WMS_RPT_LL_ERROR");
       break;

     case SMS_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT:
       report_status = WMS_RPT_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT;
       MSG_ERROR_0("NAS RLF when UE waits for final CP_ACK from NW in MT-SMS");
       break;

     case SMS_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT:
       report_status = WMS_RPT_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT;
       MSG_ERROR_0("NAS TC1M_MT timer expires for the second time");
       break;

     case SMS_RPT_RADIO_UNAVAILABLE:
       report_status = WMS_RPT_RADIO_UNAVAILABLE;
       MSG_ERROR_0("Error during msg submit WMS_RPT_RADIO_UNAVAILABLE");
       break;

     default:
       report_status = WMS_RPT_LL_ERROR;
       break;
    }

   return report_status;
}

static __inline
wms_domain_e_type wmscmi_map_domain(mmcc_domain_name_T srv_domain)
{
  wms_domain_e_type  domain;

  switch( srv_domain )
  {
    case MMCC_SMS_CS:
      domain = WMS_DOMAIN_CS;
      break;

    case MMCC_SMS_PS:
      domain = WMS_DOMAIN_PS;
      break;

    case MMCC_SMS_CS_PS:
      domain = WMS_DOMAIN_CS_PS;
      break;

    default:
      domain = WMS_DOMAIN_MAX;
      break;
  }

  return domain;
}

//==============================================================================
//==============================================================================
#ifdef TEST_FRAMEWORK
#error code not present
#else
static void wmscmi_sm_report_ind_proc(
#endif
  SM_RL_REPORT_IND_T *ind_ptr
)
{
  sys_modem_as_id_e_type    as_id      = SYS_MODEM_AS_ID_1;
  wms_report_status_e_type  rpt_status;
  wms_prov_app_info_s_type *appInfo_ptr = NULL;

  if (WMS_DEVICE_MODE_SS != wms_cfg_s_ptr()->device_mode)
  {
    as_id = ind_ptr->as_id;
  }

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  if (appInfo_ptr->mmgsdi_is_busy)
  {
    MSG_ERROR_0("Dropping SM Report Indication, SIM is busy");
    return;
  }

  rpt_status = wmscmi_map_report_status(ind_ptr->error_status);

  /* Spec GSM 03.40, section 9.2.3.6 specifies that the reference number or SMI (TP-MR)
     may possess values in the range 0 to 255. */
  if (WMS_MSG_MT_ERROR_ID_MASK == (ind_ptr->short_message_identifier & 0xFF00))
  {
    wms_msg_mt_error_ind_proc( as_id,
                               (wms_cm_conn_id_type)(ind_ptr->short_message_identifier & 0xFF),
                               rpt_status,
                               ind_ptr->cause_value );
    return;
  }

  if (WMS_MSG_ACK_REPORT_ID_MASK == (ind_ptr->short_message_identifier & 0xFF00))
  {
    wms_msg_ack_report_ind_proc( as_id,
                                 (wms_cm_conn_id_type)(ind_ptr->short_message_identifier & 0xFF),
                                 rpt_status );
    return;
  }

  if (WMS_MSG_SMMA_REQUEST_ID == ind_ptr->short_message_identifier)
  {
    wms_msg_SMMA_CONF_ind_proc( as_id,
                                rpt_status,
                                ind_ptr->cause_value );
    return;
  }

  wms_msg_sm_report_ind_proc( as_id,
                              ind_ptr->short_message_identifier,
                              rpt_status,
                              ind_ptr->cause_value,
                              ind_ptr->tpdu,
                              ind_ptr->tpdu_length,
                              ind_ptr->diagnostics_present,
                              ind_ptr->diagnostics,
                              ind_ptr->permanent_error,
                              ind_ptr->retry_interval.present,
                              (uint32)ind_ptr->retry_interval.value,
                              wmscmi_map_domain(ind_ptr->current_domain),
							  ind_ptr->ims_cause_value
							  );

  return;
} /* wmscmi_sm_report_ind_proc() */

//==============================================================================
// Allocate a WMS Task report buffer
//==============================================================================
static wms_rpt_type* wmscmi_get_rpt_buf(void)
{

  wms_rpt_type *rpt_ptr = NULL;

  rex_enter_crit_sect(&wms_rpt_queue_crit_sect);
  if(wms_rpts_cnt > 0)
  {
     rpt_ptr = (wms_rpt_type*) wms_mem_malloc(sizeof(wms_rpt_type));
  }

  if (rpt_ptr != NULL)
  {
     wms_rpts_cnt--;   // Decrement the counter
  }
  else
  {
     MSG_ERROR_1("wms_mem_malloc failied or max out the max rpt cmds = %d", wms_rpts_cnt);
  }
  rex_leave_crit_sect(&wms_rpt_queue_crit_sect);

  return rpt_ptr;
}

//==============================================================================
// Queues messages to wms
//==============================================================================
static void wmscmi_put_rpt(wms_rpt_type* rpt_ptr)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   MSG_HIGH_1( "wms rpt: %d", rpt_ptr->rpt.message_header.message_id);
   (void) q_link( rpt_ptr, & rpt_ptr->hdr.link ); /* init link */
   q_put( & rpt_q, & rpt_ptr->hdr.link );         /* and queue it */

   if (NULL != wms_myself)
   {
     // If WMS task isn't started, no signal is set
     (void) rex_set_sigs(wms_myself, rpt_sig_mask);   // signal the task
   }

   MSG_LOW_1("WMS_RPTS_CNT = %d", wms_rpts_cnt);
}

//==============================================================================
// returns the number of pending reports to WM
//==============================================================================
static int wmscmi_get_rpt_count(void)
{
   MSG_LOW_1("rpt_q cnt=%d", q_cnt(&rpt_q));
   return q_cnt(&rpt_q);
}


//==============================================================================
// This is the WMS-specific routine used when other layers call gs_send_message
//==============================================================================
gs_status_T wmscmi_send_routine(
  CMCNas_Rpt *msg_p
)
{
  gs_status_T   ret_status = GS_FAILURE;

  do
  {
    void     *cmd_ptr;
    size_t    copy_size = 0;

    if (NULL == msg_p)
    {
      MSG_ERROR_0("wmscmi_send_routine() Bad parm");
      break;
    }

    /* Fill this in with command processing for WMS */
    MSG_HIGH_1("Received NAS report %d for WMS", msg_p->message_header.message_id);

    switch (msg_p->message_header.message_id)
    {
    case UASMS_SM_DATA_IND:
      copy_size = sizeof(SM_RL_DATA_IND_T);
      break;

    case UASMS_SM_REPORT_IND:
      copy_size = sizeof(SM_RL_REPORT_IND_T);
      break;

    case UASMS_SM_LINK_CONTROL_IND:
      copy_size = sizeof(SM_RL_LINK_CONTROL_REPORT_T);
      break;

#ifdef FEATURE_GWSMS_BROADCAST
    case CB_DATA_IND:
      copy_size = sizeof(cb_data_ind_type);
      break;

    case CB_CELL_CHANGE_IND:
      copy_size = sizeof(cb_cell_change_ind_type);
      break;

    case CB_DUPLICATION_DETECTION_IND:
      copy_size = sizeof(cb_duplication_detection_ind_type);
      break;

    case CB_ERROR_IND:
      copy_size = sizeof(cb_error_indication_type);
      break;

#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)
    case CB_ETWS_PRIM_NOTIFICATION_IND:
      copy_size = sizeof(cb_etws_prim_notification_ind_type);
      break;

    case CB_ETWS_SEC_NOTIFICATION_IND:
      copy_size = sizeof(cb_etws_sec_notification_ind_type);
      break;
#endif /* defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)*/
#endif /*  FEATURE_GWSMS_BROADCAST */

    default:
      break;
    } // switch (msg_p->message_header.message_id)

    if (0 == copy_size)
    {
      break;
    }

    cmd_ptr = wmscmi_get_rpt_buf();

    if (NULL == cmd_ptr)
    {
      MSG_ERROR_0("wmscmi_send_routine() out of rpt_buf");
      break;
    }

    /* Copy data to command buffer */
    (void)memscpy( &((wms_rpt_type *)cmd_ptr)->rpt,
                   copy_size,
                   (void *)msg_p,
                   copy_size );

    /* Put message in destination task queue */
    wmscmi_put_rpt(cmd_ptr);

    ret_status = GS_SUCCESS;

  } while (0);

  return ret_status;
} /* wmscmi_send_routine() */

//==============================================================================
//==============================================================================
static void wmscmi_process_rpt(wms_rpt_type* rpt_ptr)
{
#ifdef FEATURE_GSTK
   if (wms_cfg_s_ptr()->refresh_in_progress)
   {
      /* Do not process message indications when refresh command is in progress */
      MSG_HIGH_0("Dropping reports from NAS while SIM is in refresh");
      return;
   }
#endif

   switch (rpt_ptr->rpt.message_header.message_id) {
   case UASMS_SM_DATA_IND:
      {
         sys_modem_as_id_e_type asid = SYS_MODEM_AS_ID_1;

         /* with 7+5 enabled mode, SMS over IMS can come on any SUB */
         if (WMS_DEVICE_MODE_SS != wms_cfg_s_ptr()->device_mode)
         {
           asid = rpt_ptr->rpt.rl_data_ind.as_id;
         }

         wms_msg_sm_data_ind_proc( asid,
                                   rpt_ptr->rpt.rl_data_ind.connection_id,
                                   WMS_DUMMY_TRANSACTION_ID,
                                   &rpt_ptr->rpt.rl_data_ind.RP_OA,
                                   rpt_ptr->rpt.rl_data_ind.tpdu,
                                   rpt_ptr->rpt.rl_data_ind.tpdu_length,
                                   rpt_ptr->rpt.rl_data_ind.sms_transport_type );
      }
      break;

   case UASMS_SM_REPORT_IND:
      wmscmi_sm_report_ind_proc(&rpt_ptr->rpt.rl_report_ind);
      break;

   case UASMS_SM_LINK_CONTROL_IND:
      wms_cfg_link_control_ind_proc(&rpt_ptr->rpt.rl_link_report_ind);
      break;

 #ifdef FEATURE_GWSMS_BROADCAST
   case CB_DATA_IND:
      {
         sys_modem_as_id_e_type asid = SYS_MODEM_AS_ID_1;

         if (WMS_DEVICE_MODE_SS != wms_cfg_s_ptr()->device_mode)
         {
           asid = rpt_ptr->rpt.cb_data_ind.as_id;
         }

         wms_msg_gw_cb_data_ind_proc( asid,
                                      rpt_ptr->rpt.cb_data_ind.cb_page,
                                      rpt_ptr->rpt.cb_data_ind.page_length,
                                      FALSE );
      }
      break;

   case CB_CELL_CHANGE_IND:
      {
         sys_modem_as_id_e_type asid = SYS_MODEM_AS_ID_1;

         if (WMS_DEVICE_MODE_SS != wms_cfg_s_ptr()->device_mode)
         {
           asid = rpt_ptr->rpt.cb_cell_change.as_id;
         }

         wms_cfg_cell_change_ind_proc( asid,
                                       rpt_ptr->rpt.cb_cell_change.cell_change_type );
      }
      break;

   case CB_DUPLICATION_DETECTION_IND:
      wms_msg_gw_cb_duplicate_ind_proc(&rpt_ptr->rpt.cb_duplicate_ind);
      break;

   case CB_ERROR_IND:
      wms_cfg_gw_cb_error_ind_proc(&rpt_ptr->rpt.cb_error_ind);
      break;
#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)
   case CB_ETWS_PRIM_NOTIFICATION_IND:
      wms_msg_etws_prim_notification_ind_proc(&rpt_ptr->rpt.cb_etws_prim_notification_ind);
      break;
   case CB_ETWS_SEC_NOTIFICATION_IND:
      wms_msg_etws_sec_notification_ind_proc(&rpt_ptr->rpt.cb_etws_sec_notification_ind);
      break;
#endif /* defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)*/
 #endif /*  FEATURE_GWSMS_BROADCAST */

   default:
      MSG_ERROR_1("Invalid rpt: %d", rpt_ptr->rpt.message_header.message_id);
      break;
   }
}

/*===========================================================================
FUNCTION wms_cm_gw_map_nas_transport

DESCRIPTION
  Maps WMS GW transport type to NAS transport type

DEPENDENCIES
  none

RETURN VALUE
  nas_msg_transport_type_e_type  Mapped NAS Transport type enum 

SIDE EFFECTS
  None.
===========================================================================*/
static nas_msg_transport_type_e_type wms_cm_gw_map_nas_transport(
  wms_msg_transport_type_e_type transport_type
)
{
  nas_msg_transport_type_e_type nas_transport_type = NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;

  if (WMS_MSG_TRANSPORT_TYPE_CS == transport_type)
  {
    nas_transport_type = NAS_MSG_TRANSPORT_TYPE_GW;
  }

  return nas_transport_type;
} /* wms_cm_gw_map_nas_transport */

static __inline wms_cmd_err_e_type WMSCmdErr_FromCMCNas(CMCNas_Err cmnas_err)
{
   switch (cmnas_err) {
   case CMC_NAS_ERR_NONE:
      return WMS_CMD_ERR_NONE;
   case CMC_NAS_ERR_OUT_OF_RESOURCES:
      return WMS_CMD_ERR_NO_RESOURCE;
   case CMC_NAS_ERR_BAD_PARM:
      return WMS_CMD_ERR_BAD_PARM;
   case CMC_NAS_ERR_UNSUPPORTED:
      return WMS_CMD_ERR_UNSUPPORTED;
   default:
      return WMS_CMD_ERR_UNSUPPORTED; /* TODO : Map extra errors here */
   }
}

/*===========================================================================
FUNCTION wms_msg_send_gw_rl_data

DESCRIPTION
  Send request to lower layers.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cmd_err_e_type wms_cm_gw_send_rl_data
(
   wms_msg_gw_mo_record_type *mo_rec_ptr,
   wms_address_s_type        *sc_address_ptr,
   uint8                      message_number,
   wms_raw_ts_data_s_type    *raw_ts_ptr
)
{
   wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;
   CMCNas_Err         cmnas_err;
   cm_called_party_bcd_no_T  sc_address_bcd;
   
   if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_rec_ptr->transport_type)
   {
     wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( mo_rec_ptr->as_id );
     if (transport_ptr != NULL && transport_ptr->is_mn_registered &&
         (WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV == transport_ptr->nw_reg_status ||
          wms_cfg_s_ptr()->admin_sms_only) &&
         !wms_is_gw_admin_msg(raw_ts_ptr)) 
     {
       MSG_ERROR_0("Message not allowed on Limited Service. Error");
       return  WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED;
     }
   }

   /* Check for SC Address Length */
   if( sc_address_ptr->number_of_digits > WMS_GW_ADDRESS_MAX )
   {
     MSG_ERROR_0("SC Address exceeds Max Digits. Error");
     return WMS_CMD_ERR_MSG_ENCODE;
   }

   if (raw_ts_ptr == NULL)
   {
      MSG_ERROR_0("raw_ts_ptr is NULL in wms_msg_send_gw_rl_data");
      return WMS_CMD_ERR_NULL_PTR;
   }

   /* prepare for the message and send it to MN */
   wms_ts_convert_to_bcd_address(sc_address_ptr,
                                 &sc_address_bcd);

   cmnas_err = CMCNas_SendData(pCMCNas,
                               mo_rec_ptr->as_id,
                               wms_cm_gw_map_nas_transport( mo_rec_ptr->transport_type ),
                               &sc_address_bcd, message_number,
                               raw_ts_ptr->len, raw_ts_ptr->data);

   cmd_err = WMSCmdErr_FromCMCNas(cmnas_err);

   if (WMS_CMD_ERR_NONE == cmd_err)
   {
     wms_prov_app_info_s_type *appInfo_ptr = NULL;

     appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(mo_rec_ptr->as_id, WMS_MESSAGE_MODE_GW);

     if (NULL != appInfo_ptr )
     {
       if (0 == appInfo_ptr->mo_sms_count)
       {
         wms_mm_sms_call_info_T  mm_status;

         memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

         mm_status.as_id = mo_rec_ptr->as_id;

         if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_rec_ptr->transport_type)
         {
           mm_status.sms_over_ip_status = TRUE;
         }
         else
         {
           mm_status.sms_over_nas_status = TRUE;
         }

         (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
       }
       appInfo_ptr->mo_sms_count++;

     }
   }

   MSG_HIGH_1("MO SMS Message Sent to MN %d", cmd_err);

   return cmd_err;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_send_rl_report(
   sys_modem_as_id_e_type     as_id,
   wms_cm_conn_id_type        connection_id,
   boolean                    is_success,      /* */
   uint8                      rp_cause_value,  /* */
   uint8*                     tpdu_ptr,
   uint8                      tpdu_len
)
{
   wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;
   CMCNas_Err cmnas_err = CMC_NAS_ERR_NONE;

#ifdef FEATURE_FUZZ_TEST
   if (WMS_DBG_CONNECTION_ID != connection_id)
   {

      cmnas_err = CMCNas_SendReport( pCMCNas,
                                     as_id,
                                     connection_id, is_success,
                                     rp_cause_value, tpdu_len,
                                     tpdu_ptr );
   }
   else
   {
      MSG_HIGH_0("Drop the ack for debug mt message");
      wms_msg_ack_report_ind_proc(as_id, connection_id, WMS_RPT_OK);
   }
#else

   cmnas_err = CMCNas_SendReport( pCMCNas,
                                  as_id,
                                  connection_id, is_success,
                                  rp_cause_value, tpdu_len,
                                  tpdu_ptr );
#endif

   cmd_err = WMSCmdErr_FromCMCNas(cmnas_err);

   MSG_HIGH_1("RL_REPORT Sent to MN %d", cmd_err);

   return cmd_err;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_send_rl_memory_available(
                           wms_msg_gw_mo_record_type *mo_rec_ptr,
                           boolean is_abort,
                           wms_msg_transport_type_e_type transport_type)
{
   wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;

   CMCNas_Err cmnas_err;

   if (NULL == mo_rec_ptr)
   {
      MSG_ERROR_0("NULL pointer in wms_cm_gw_send_rl_memory_available()");
      return WMS_CMD_ERR_NULL_PTR;
   }
   cmnas_err = CMCNas_SendMemoryAvailable(pCMCNas,
                                          mo_rec_ptr->as_id,
                                          WMS_MSG_SMMA_REQUEST_ID,
                                          is_abort,
                                          wms_cm_gw_map_nas_transport( transport_type ));

   cmd_err = WMSCmdErr_FromCMCNas(cmnas_err);

   MSG_HIGH_1("SMMA Request Sent to MN %d", cmd_err);

   return cmd_err;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_bearer_selection(
   sys_modem_as_id_e_type     as_id,
   wms_gw_domain_pref_e_type gw_domain_pref)
{
   CMCNas_Err cmnas_err;
   wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;

   mm_sms_bearer_selection_enum_T mm_bearer_selection = (mm_sms_bearer_selection_enum_T) gw_domain_pref;

   cmnas_err = CMCNas_SetBearerSelection (pCMCNas,
                                          as_id,
                                          mm_bearer_selection);

   if (CMC_NAS_ERR_NONE == cmnas_err)
   {
      MSG_HIGH_0("GW Bearer Selection Sent to MN");
   }

   cmd_err = WMSCmdErr_FromCMCNas(cmnas_err);

   return cmd_err;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_send_sms_status_to_mm(
  wms_mm_sms_call_info_T     mm_sms_status)
{
  CMCNas_Err cmnas_err;
  wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;

  cmnas_err = CMCNas_Send_sms_status_to_mm (pCMCNas,
    mm_sms_status);

  if (CMC_NAS_ERR_NONE == cmnas_err)
  {
    MSG_HIGH_0("SMS status indication Sent to MN");
  }

  cmd_err = WMSCmdErr_FromCMCNas(cmnas_err);

  return cmd_err;
} /* wms_cm_gw_send_sms_status_to_mm() */

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_rl_link_control(
   sys_modem_as_id_e_type     as_id,
   wms_cfg_link_control_mode_e_type link_control_mode,
   uint8 link_control_timer)
{
   mn_cnm_cmd_type* mn_cmd_ptr = NULL;

   mn_cmd_ptr = mn_uasms_get_cmd_buf(UASMS_SM_LINK_CONTROL_REQ);
   if (NULL == mn_cmd_ptr) {
      MSG_ERROR_0("MM out of cmd buffer!");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   /* prepare the message and send it to MN */
   mn_cmd_ptr->cmd.sm_rl_link_control_req.mode               = (sms_link_control_mode_T)link_control_mode;
   mn_cmd_ptr->cmd.sm_rl_link_control_req.timeout_in_seconds = link_control_timer;
   mn_cmd_ptr->cmd.sm_rl_link_control_req.as_id              = as_id;

   mn_cnm_put_cmd(mn_cmd_ptr);

   /* done */
   return WMS_CMD_ERR_NONE;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_rl_update_timer_f(
  sys_modem_as_id_e_type as_id,
  wms_msg_transport_f_timer_type timer_f_val)
{
   mn_cnm_cmd_type* mn_cmd_ptr = NULL;

   mn_cmd_ptr = mn_uasms_get_cmd_buf(UASMS_SM_UPDATE_TIMERF);
   if (NULL == mn_cmd_ptr) {
      MSG_ERROR_0("MM out of cmd buffer!");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   /* prepare the message and send it to MN */
   mn_cmd_ptr->cmd.sm_rl_update_timerf.timerf_value          = timer_f_val;
   mn_cmd_ptr->cmd.sm_rl_update_timerf.as_id                 = as_id;

   mn_cnm_put_cmd(mn_cmd_ptr);

   /* done */
   return WMS_CMD_ERR_NONE;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_init(void)
{
   (void) q_init(&rpt_q);

   wms_myself = rex_self(); // Will never change after this

   rex_init_crit_sect(&wms_rpt_queue_crit_sect);

   rpt_sig_mask = WMS_RPT_Q_SIG;

   /* Initialize gs message passing for this queue */
   if (FALSE == CMCNas_MN_New(wmscmi_send_routine, wmscmi_send_routine,
                              wmscmi_send_routine, &pCMCNas))
   {
      MSG_ERROR_0("CMCNas_MN_New failed");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   return WMS_CMD_ERR_NONE;
}

//==============================================================================
//==============================================================================
boolean wms_cm_gw_dispatch_one(void)
{
   wms_rpt_type* rpt_ptr;
   rpt_ptr = (wms_rpt_type *)q_get(&rpt_q);
   if ((wms_rpt_type*)0 == rpt_ptr) {
      return FALSE;
   }

   wmscmi_process_rpt(rpt_ptr);     /* actually handle it */
   rex_enter_crit_sect(&wms_rpt_queue_crit_sect);
   wms_mem_free (rpt_ptr);
   wms_rpts_cnt++;
   rex_leave_crit_sect(&wms_rpt_queue_crit_sect);

   return TRUE;
}

//==============================================================================
//==============================================================================
boolean wms_cm_gw_is_idle(void)
{
   return (0 == wmscmi_get_rpt_count());
}

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_disable(
  sys_modem_as_id_e_type as_id
)
{
   wms_cmd_err_e_type err = WMS_CMD_ERR_NONE;

#ifdef FEATURE_GSM_CB
   cb_search_list_req_type  * list_ptr;
   /* Send search list to NAS */
   list_ptr = (cb_search_list_req_type*)cb_get_cmd(MS_WMS_CB, (byte)CB_SEARCH_LIST_REQ);

   if(NULL == list_ptr) {
      MSG_ERROR_0("NAS put of cmd buffer!");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   /* Fill in command headers */
   list_ptr->message_header.message_set = MS_WMS_CB;
   list_ptr->message_header.message_id  = (byte)CB_SEARCH_LIST_REQ;
   PUT_IMH_LEN( sizeof(cb_search_list_req_type) - sizeof(IMH_T),
                & list_ptr->message_header );
   list_ptr->search_list_length   = 0;
#ifdef FEATURE_UMTS_BMC
   list_ptr->reading_advised = FALSE;
   list_ptr->reading_optional = FALSE;
#endif /* FEATURE_UMTS_BMC */

   list_ptr->as_id = as_id;

   /* Send command */
   cb_put_cmd((cb_command_body_type *)list_ptr);
   MSG_HIGH_0("WMS -> CB: Search List Cmd");

   /* Free the dynamic buffer back to the heap */
   modem_mem_free(list_ptr, MODEM_MEM_CLIENT_NAS);
#endif /* FEATURE_GSM_CB */

#ifdef FEATURE_SMS_PWS_OVER_LTE
   {
      lte_rrc_msg_id_list_req_s  msg_id_list_req;
      errno_enum_type            msgr_error = E_SUCCESS;

      MSG_HIGH_0("In wms_cm_gw_cb_disable()");

      msgr_init_hdr((msgr_hdr_struct_type *)&msg_id_list_req,
                    MSGR_WMS_LTE,
                    LTE_RRC_MSG_ID_LIST_REQ);

      msg_id_list_req.msg_id_range_list_size = 0;
      memset(msg_id_list_req.msg_id_range_list, 0xFF, sizeof(msg_id_list_req.msg_id_range_list));

#ifndef TEST_FRAMEWORK /* ASSERT in msgr_send() fails under QTF */
      msgr_error = msgr_send((msgr_hdr_struct_type*)&msg_id_list_req,
                             sizeof(lte_rrc_msg_id_list_req_s));
#endif /* TEST_FRAMEWORK */

      MSG_HIGH_1("msgr_send() returns %d", msgr_error);

      err = (E_SUCCESS == msgr_error) ?  WMS_CMD_ERR_NONE : WMS_CMD_ERR_NO_RESOURCE;
   }
#endif /* FEATURE_SMS_PWS_OVER_LTE*/

   /* done */
   return err;

}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_search_list(
   sys_modem_as_id_e_type as_id,
   wms_gw_cb_srv_id_type* list1_ptr,
   uint16 len1, /* number of nv_sms_gw_cb_service_table_type */
   nv_sms_gw_cb_service_table_type* list2_ptr,
   uint16 len2,  /* number of nv_sms_gw_cb_service_table_type */
   boolean reading_advised,
   boolean reading_optional,
   uint32* num_selected_ptr /* OUT */
)
{
   uint32 i, j=0;
   wms_cmd_err_e_type err = WMS_CMD_ERR_NONE;

#ifdef FEATURE_GSM_CB
   cb_search_list_req_type  * list_ptr;

   /* Send search list to NAS */
   list_ptr = (cb_search_list_req_type*)cb_get_cmd(MS_WMS_CB, (byte)CB_SEARCH_LIST_REQ);

   if(NULL == list_ptr) {
      MSG_ERROR_0("NAS put of cmd buffer!");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   /* Fill in command headers */
   list_ptr->message_header.message_set = MS_WMS_CB;
   list_ptr->message_header.message_id  = (byte)CB_SEARCH_LIST_REQ;
   PUT_IMH_LEN( sizeof(cb_search_list_req_type) - sizeof(IMH_T),
                & list_ptr->message_header );
#ifdef FEATURE_UMTS_BMC
   list_ptr->reading_advised = reading_advised;
   list_ptr->reading_optional = reading_optional;
#endif /* FEATURE_UMTS_BMC */

   /* Fill up CBMID Entries */
   for (i=0, j=0;
         i<len1 && j<CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED ; i++, j++)
   {
      list_ptr->search_list[j].current_message_id_type = CB_SINGLE_MESSAGE_ID;
      list_ptr->search_list[j].single_message_id.message_id = list1_ptr[i];
      MSG_HIGH_2( "msg_id_range_list[ %d ] msg_id = %d",
                  j,
                  list1_ptr[i] );
   }

   /* Fill in the search list */
   for( i=0; i<len2 && j<CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED ; i++ )
   {
      if( list2_ptr[i].selected == TRUE )
      {
        if( list2_ptr[i].service_from == list2_ptr[i].service_to )
        {
          list_ptr->search_list[j].current_message_id_type =
                            CB_SINGLE_MESSAGE_ID;
          list_ptr->search_list[j].single_message_id.message_id =
                            list2_ptr[i].service_from;
        }
        else
        {
          list_ptr->search_list[j].current_message_id_type =
                            CB_MESSAGE_ID_RANGE;
          list_ptr->search_list[j].message_id_range.min_message_id =
                            list2_ptr[i].service_from;
          list_ptr->search_list[j].message_id_range.max_message_id =
                            list2_ptr[i].service_to;
        }
        MSG_HIGH_3( "msg_id_range_list[ %d ] (min_msg_id %d, max_msg_id %d)",
                    j,
                    list2_ptr[i].service_from,
                    list2_ptr[i].service_to );
        j ++;
      } /* if selected */
   } /* for loop */

   list_ptr->search_list_length = (byte)j;

   list_ptr->as_id = as_id;

   /* Send command */
   cb_put_cmd((cb_command_body_type *)list_ptr);
   MSG_HIGH_0("WMS -> CB: Search List Cmd");

   /* Free the dynamic buffer back to the heap */
   modem_mem_free(list_ptr, MODEM_MEM_CLIENT_NAS);

   if (0 != num_selected_ptr) {
      *num_selected_ptr = j;
   }

#endif /* FEATURE_GSM_CB */

#ifdef FEATURE_SMS_PWS_OVER_LTE
   {
      lte_rrc_msg_id_list_req_s  msg_id_list_req;
      errno_enum_type            msgr_error = E_SUCCESS;

      msgr_init_hdr((msgr_hdr_struct_type *)&msg_id_list_req,
                    MSGR_WMS_LTE,
                    LTE_RRC_MSG_ID_LIST_REQ);
      MSG_HIGH_1("Length of search list 1 = %d", len1);
      j = 0;
      /* Fill up CBMID Entries */
      for (i = 0, j = 0;
            i < len1 && j < LTE_RRC_MAX_MSG_ID_LIST_SIZE; i++, j++)
      {
         msg_id_list_req.msg_id_range_list[j].min_msg_id = list1_ptr[i];
         msg_id_list_req.msg_id_range_list[j].max_msg_id = list1_ptr[i];
         MSG_HIGH_2( "msg_id_range_list[ %d ] msg_id = %d",
                     j,
                     list1_ptr[i] );
      }

      MSG_HIGH_1("Length of search list 2 = %d", len2);
      /* Fill in the search list */
      for (i = 0; i < len2 && j < LTE_RRC_MAX_MSG_ID_LIST_SIZE; i++)
      {
         if (list2_ptr[i].selected)
         {
             msg_id_list_req.msg_id_range_list[j].min_msg_id =
                               list2_ptr[i].service_from;
             msg_id_list_req.msg_id_range_list[j].max_msg_id =
                               list2_ptr[i].service_to;
             MSG_HIGH_3( "msg_id_range_list[ %d ] (min_msg_id %d, max_msg_id %d)",
                         j,
                         list2_ptr[i].service_from,
                         list2_ptr[i].service_to );
             j++;
         } /* if selected */
      } /* for loop */

      msg_id_list_req.msg_id_range_list_size = (uint8) j;
      MSG_HIGH_1("In wms_cm_gw_cb_search_list(): search list size is %d", msg_id_list_req.msg_id_range_list_size);

#ifndef TEST_FRAMEWORK /* ASSERT in msgr_send() fails under QTF */
      msgr_error = msgr_send((msgr_hdr_struct_type*)&msg_id_list_req,
                             sizeof(lte_rrc_msg_id_list_req_s));
#endif /* TEST_FRAMEWORK */

      MSG_HIGH_1("msgr_send() returns %d", msgr_error);

      err = (E_SUCCESS == msgr_error) ?  WMS_CMD_ERR_NONE : WMS_CMD_ERR_NO_RESOURCE;
   }
#endif /* FEATURE_SMS_PWS_OVER_LTE */

   /* done */
   return err;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_delete_message(
   sys_modem_as_id_e_type as_id,
   uint16 message_id, /* message id */
   uint16 message_code, /* message code */
   uint8 page_number /* page number */
)
{
#ifdef FEATURE_GSM_CB
   cb_delete_page_req_type  * req_ptr;

   req_ptr = (cb_delete_page_req_type*)cb_get_cmd(MS_WMS_CB, (byte)CB_DELETE_PAGE_REQ);
   if (NULL == req_ptr) {
      MSG_ERROR_0("NAS out of Command Buffer");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   /* Fill in command headers */
   req_ptr->message_header.message_set = MS_WMS_CB;
   req_ptr->message_header.message_id  = (byte)CB_DELETE_PAGE_REQ;
   PUT_IMH_LEN( sizeof(cb_delete_page_req_type) - sizeof(IMH_T),
               & req_ptr->message_header );
   /* Fill in command data */
   req_ptr->message_id = message_id;
   req_ptr->message_code = message_code;
   req_ptr->page_number = page_number;

   req_ptr->as_id = as_id;

   /* Send command */
   cb_put_cmd((cb_command_body_type*)(void*)req_ptr);
   MSG_HIGH_0("WMS -> CB: Delete Page Cmd");

   /* Free the dynamic buffer back to the heap */
   modem_mem_free(req_ptr, MODEM_MEM_CLIENT_NAS);
#endif /* FEATURE_GSM_CB */
   /* done */
   return WMS_CMD_ERR_NONE;
}

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_delete_all_messages(
  sys_modem_as_id_e_type as_id
)
{
#ifdef FEATURE_GSM_CB
   cb_delete_all_req_type  * req_ptr;

   req_ptr = (cb_delete_all_req_type*)cb_get_cmd(MS_WMS_CB, (byte)CB_DELETE_ALL_REQ);
   if (NULL == req_ptr) {
      MSG_ERROR_0("NAS out of cmd buffer!");
      return WMS_CMD_ERR_NO_RESOURCE;
   }

   /* Fill in command headers */
   req_ptr->message_header.message_set = MS_WMS_CB;
   req_ptr->message_header.message_id  = (byte)CB_DELETE_ALL_REQ;
   PUT_IMH_LEN( sizeof(cb_delete_all_req_type) - sizeof(IMH_T),
                & req_ptr->message_header );

   req_ptr->as_id = as_id;

   /* Send command */
   cb_put_cmd((cb_command_body_type*)(void*)req_ptr);
   MSG_HIGH_0("WMS -> CB: Delete All Cmd");

   /* Free the dynamic buffer back to the heap */
   modem_mem_free(req_ptr, MODEM_MEM_CLIENT_NAS);
#endif /* FEATURE_GSM_CB */
   /* done */
   return WMS_CMD_ERR_NONE;
}
#endif /* FEATURE_GWSMS_BROADCAST */


