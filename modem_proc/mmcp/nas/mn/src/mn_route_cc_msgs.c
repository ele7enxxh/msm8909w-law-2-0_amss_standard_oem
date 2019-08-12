/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_route_cc_msgs.c_v   1.6   10 Apr 2002 04:41:06   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_route_cc_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/3/01      CD     Changed call to 
                        MN_handle_mmi_abort_cc_req,
                        MN_process_mmi_0_send  
                        MN_process_mmi_1_send
                        MN_process_mmi_2_send
                        MN_process_mmi_mo_end_call_req
                   due to function prototype change

5/3/01      CD     Changed MN_handle_MNCC_ERR_IND() calling sequence due to  
                   function prototype change
                   Changed MN_handle_MNCC_REEST_IND calling sequence due to 
                   function prototype change

5/9/01      CD     Removed unnecessary file header includes

5/25/01     CD     Removed cause.h and ms.h since they were not being used

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

03/12/02    AB     Added CM_4_SEND_REQ and CM_4_X_SEND_REQ to MN main 
                   message router for handling ECT and CD from CM/UI
                   respectively.

04/08/02    AB     Added MNCC_NOTIFY_DEFERRED_IND message handler to 
                   MN main message router.

07/30/01    AB     Added missing break in MN_route_CC_to_MN_messages(), when
                   MNCC_NOTIFY_DEFERRED_IND were introduced earlier.

10/12/02    AB     Added CM_MO_END_X_CALL message to abort a specific 
                   call from CM/UI.

11/02/04    AB     Added SS call related User-to-User Signaling feature.

05/10/05    HS     Added support for FEATURE_CCBS

===========================================================================*/

/* includes */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"



extern void MN_handle_cs_sig_conn_rel_ind(void);

/*
 *   Function name : MN_route_CC_CM_to_MN_primitives
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function provides the routing of messages
 * to the MN layer from the CM layer and routes them to
 * the appropriate CM message handling functions in MN-C
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_route_CC_CM_to_MN_primitives(IMH_T    *message)

{

   switch(message->message_id)
   {
  
     case CM_MO_NORMAL_CALL_REQ:
     case CM_MO_EMERGENCY_CALL_REQ:
  
        {
  
           MN_request_mo_call_setup(message);
           break;
  
        }
  
     case CM_START_DTMF_REQ:
  
        {
  
           MN_process_cm_start_dtmf_req(message);
           break;
  
        }
  
     case CM_STOP_DTMF_REQ:
  
        {
  
           MN_process_cm_stop_dtmf_req(message);
           break;
  
        }
  
     case CM_0_SEND_REQ:
  
        {
  
           MN_process_cm_0_send((cm_n_send_req_T *)message);
           break;
  
        }
  
     case CM_1_SEND_REQ:
  
        {
  
           MN_process_cm_1_send((cm_n_send_req_T *)message);
           break;
  
        }
  
     case CM_1_X_SEND_REQ:
  
        {
  
           MN_process_cm_1_x_send((cm_n_x_send_req_T *)message);
           break;
  
        }
  
     case CM_2_SEND_REQ:
  
        {
  
           MN_process_cm_2_send((cm_n_send_req_T *)message);
           break;
  
        }
  
     case CM_2_X_SEND_REQ:
  
        {
  
           MN_process_cm_2_x_send((cm_n_x_send_req_T *)message);
           break;
  
        }
  
     case CM_3_SEND_REQ:
  
        {
  
           MN_process_cm_3_send((cm_n_send_req_T *)message);
           break;
  
        }
  
     case CM_4_SEND_REQ:
  
        {
  
           MN_process_cm_4_send((cm_n_send_req_T *)message);
           break;
  
        }
  
     case CM_4_X_SEND_REQ:
  
        {
  
           MN_process_cm_4_x_send((cm_4_x_send_req_T *)message);
           break;
  
        }
  
     case CM_MO_END_CALL_REQ:
  
        {
  
           MN_process_cm_mo_end_call_req();
           break;
  
        }
  
     case CM_MO_END_X_CALL_REQ:
  
        {
  
           MN_process_cm_mo_end_x_call_req((cm_n_x_send_req_T *)message);
           break;
  
        }
  
     case CM_ABORT_CC_REQ:
  
        {
  
           MN_handle_cm_abort_cc_req((cm_abort_cc_req_T *)message);
           break;
  
        }
  
  #ifdef FEATURE_UUS
     case CM_MO_USER_DATA_REQ:
  
        {
  
           MN_process_cm_mo_user_data_req(message);
           break;
  
        }
  #endif
  
  #if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
     case CM_MO_MODIFY_REQ:
  
        {
  
           MN_handle_cm_mo_modify_req(message);
           break;
  
        }
  
     case CM_MT_MODIFY_RES:
  
        {
  
           MN_handle_cm_mt_modify_res(message);
           break;
  
        }
  #endif
  
     case CM_MT_CALL_RES:
  
        {
  
           MN_handle_cm_mt_call_res(message);
           break;
  
        }
  
     case CM_MT_SETUP_RES:
  
        {
  
           MN_handle_cm_mt_setup_res(message);
           break;
  
        }
  
  
  #ifdef FEATURE_CCBS
     case CM_5_SEND_REQ:
  
        {
  
           MN_process_cm_5_send((cm_5_send_req_T *)message);
           break;
  
        }
  
     case CM_RECALL_RSP:
  
        {
           MN_handle_cm_recall_rsp(message);
           break;
  
        }
  #endif /* FEATURE_CCBS */
  
#if defined(FEATURE_DUAL_SIM) 
  
     case CM_ASID_2_VSID_MAPPING_IND:
     {
        MN_handle_cm_asid_2_vsid_mapping_ind(message);
        
     }
     break;
  
#endif
  
     case CM_TRANSFER_SRVCC_CALL_CONTEXT_RES:
     {
        MN_handle_cm_transfer_srvcc_call_context_res(message);
     }
     break;
     
     default:
     {
        ERR("ERROR: Unexpected CM to MN message %d",message->message_id,0,0);
        break;
     }
   }
}

/*
 *   Function name : MN_route_CC_to_MN_primitives
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function provides the routing of messages
 * to the MN layer from the CM layer and routes them to
 * the appropriate CM message handling functions in MN-CC
 *
 *  Parameters:
 *  -----------
 *  IN *message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_route_CC_to_MN_messages(IMH_T    *message)
{

   switch(message->message_id)
   {
       case MNCC_ALERT_IND:

           MN_handle_MNCC_ALERT_IND(message);

           break;

       case MNCC_CALL_HOLD_CNF:

           MN_process_MNCC_CALL_HOLD_CNF(message);

           break;

       case MNCC_CALL_HOLD_REJ:

           MN_process_MNCC_CALL_HOLD_REJ(message);

           break;

       case MNCC_CALL_PROC_IND:

           MN_handle_MNCC_CALL_PROC_IND(message);

           break;

       case MNCC_CALL_RETRIEVE_CNF:

           MN_process_MNCC_CALL_RETRIEVE_CNF(message);

           break;

       case MNCC_CALL_RETRIEVE_REJ:

           MN_process_MNCC_CALL_RETRIEVE_REJ(message);

           break;

       case MNCC_DISC_IND:

           MN_handle_MNCC_DISC_IND(message);

           break;

       case MNCC_ERR_IND:

           MN_handle_MNCC_ERR_IND(
#if defined(FEATURE_DUAL_SIM )
                           message
#endif
                                  );

           break;

       case MNCC_FACILITY_IND:

           MN_process_MNCC_FACILITY_IND(message);

           break;

       case MNCC_NOTIFY_DEFERRED_IND:

           MN_process_MNCC_NOTIFY_DEFERRED_IND(message);
      
           break;

#ifdef FEATURE_UUS
       case MNCC_USER_DATA_DEFERRED_IND:

           MN_process_MNCC_USER_DATA_DEFERRED_IND(message);
      
           break;
#endif

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
       case MNCC_MODIFY_CNF:

           MN_handle_MNCC_MODIFY_CNF(message);

           break;

       case MNCC_MODIFY_IND:

           MN_handle_MNCC_MODIFY_IND(message);

           break;
#endif

       case MNCC_NOTIFY_IND:

           MN_handle_MNCC_NOTIFY_IND(message);

           break;

       case MNCC_PROGRESS_IND:

           MN_handle_MNCC_PROGRESS_IND(message);

           break;

       case MNCC_REEST_IND:

           MN_handle_MNCC_REEST_IND();

           break;

       case MNCC_REJ_IND:

           MN_handle_MNCC_REJ_IND(message);

           break;

       case MNCC_REL_CNF:

           MN_handle_MNCC_REL_CNF(message);

           break;

       case MNCC_REL_IND:

           MN_handle_MNCC_REL_IND(message);

           break;

       case MNCC_SETUP_CNF:

           MN_process_MNCC_SETUP_CNF(message);

           break;

       case MNCC_SETUP_COMPL_IND:

           MN_handle_mt_call_answer(message);

           break;

       case MNCC_SETUP_IND:

           MN_handle_mt_call_setup(message);

           break;

       case MNCC_START_DTMF_CNF:

           MN_process_MNCC_START_DTMF_CNF(message);

           break;

       case MNCC_STOP_DTMF_CNF:

           MN_process_MNCC_STOP_DTMF_CNF(message);

           break;

       case MNCC_SYNC_IND:

           MN_handle_MNCC_SYNC_IND(message);

           break;

       case MNCC_RAB_REL_IND:

           MN_put_cm_rab_released_ind(
#if defined(FEATURE_DUAL_SIM )
            message
#endif
           );

           break;

       case MNCC_CS_SIG_CONN_REL_IND:

           MN_handle_cs_sig_conn_rel_ind();

           break;           
    
#ifdef FEATURE_UUS
       case MNCC_USER_USER_DATA_IND:

           MN_process_MNCC_USER_USER_DATA_IND(message);

           break;

       case MNCC_CONGESTION_IND:

           MN_process_MNCC_CONGESTION_IND(message);

           break;
#endif

#ifdef FEATURE_CCBS
       case MNCC_RECALL_IND:

           MN_process_MNCC_RECALL_IND(message);

           break;

       case MNCC_CC_EST_IND:

           MN_process_MNCC_CC_EST_IND(message);

           break;
#endif /* FEATURE_CCBS */

       case MNCC_SRVCC_HANDOVER_COMPLETE_IND:

            MN_handle_srvcc_ho_complete(message);

           break;

       default:

           ERR("ERROR: Unexpected MNCC message %d",message->message_id,0,0);

           break;

   }

}
