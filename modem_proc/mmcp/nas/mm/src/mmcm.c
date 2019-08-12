/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmcm.c_v   1.15   01 Jul 2002 09:39:44   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmcm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/01   jca     Renamed location_information -> mm_location_information.
05/15/01   sbk     In mm_pass_CM_data, the message is sent to rrc if umts mode
06/03/01   sbk     Global variables are renamed to use mm_ prefix
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/15/01   ab      Renamed CM to CNM.
08/22/01   jca     Removed some errant CNM renames.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
01/28/01   sbk     Checking for state validity of received L3 messages
02/20/02   sbk     TC handling routine added
04/14/02   ks      Setting mm_sig_low_priority to TRUE for PD_SMS before sending
                   the OTA message in mm_pass_CNM_data().
04/26/02   jca     Modifications to reflect changes to MM-CNM interface.
11/14/02   vdr     Added new function mm_send_ps_cnm_data() to transfer CNM data
                   requests over PS connection
02/10/03   cd      Fixed Lint errors
02/21/03   vdr     Changed mm_confirm_mm_connection() interface
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
08/05/03   cd      Resetting the mm_sig_low_priority to FALSE after PD_SMS in 
                   mm_send_ps_cnm_data()
04/06/04   cd      LCS support
07/07/04   cd      Fixed Lint errors
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
02/14/05   ks      Now calling mm_send_mmcnm_rel_cnf_nwdata() to CNM a REL_CNF
                   with NW DATA indication which puts MM from state 9 to 6.
06/14/06   nr      New arguments added to mm_notify_reestablishment_fail.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_cc_mm_v.h"
#include "mm_v.h"
#include "err.h"
#include "mm_umts_v.h"
#ifdef FEATURE_TC
#include "tctask.h"
#include "tctask_v.h"
#include "tc_mm.h"
#endif
#include "mm_coord.h"
#include "gmm_int_v.h"

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#endif

extern void mm_send_mmcnm_rel_cnf_nwdata (void);

/*===========================================================================

FUNCTION MM_SEND_PS_CNM_DATA

DESCRIPTION
  This function transfers the CNM messages toward either RRC or LLC according to
  the current MS operation mode when the phone is attached to the PS domain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_ps_cnm_data
(
  mm_cmd_type* msg_p
)
{
#ifdef FEATURE_GSM_GPRS

  if (is_gsm_mode())
  {
    word length ;

    /* Get message length */
    GET_IMH_LEN( length, (IMH_T*)(&msg_p->cmd.mmcnm_data_req)) ;

    gmmllc_gmm_send_ota_message(length,
                      ((byte *)(&msg_p->cmd.mmcnm_data_req) + sizeof(IMH_T))
#ifdef FEATURE_GPRS_PS_HANDOVER
                      ,FALSE
#endif  
                                                           ) ;
  }
  else

#endif

  if (is_umts_mode())
  {
    if (ps_session_status == MMRRC_ACTIVE)
    {
       if (GET_PD(msg_p->cmd.mmcnm_data_req.data[0]) == PD_SMS)
       {
          mm_sig_low_priority = TRUE;
       }

       mm_send_nas_msg_to_rrc_by_domain(
                   (byte *)(&msg_p->cmd.mmcnm_data_req),RRC_PS_DOMAIN_CN_ID ) ;

       mm_sig_low_priority = FALSE;
    }
    else
    {
       MSG_HIGH_DS_0(MM_SUB,"=MM= Trying to send PS Data when PS Session is not active");
    }

  }
  else
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Unsupported operation mode",0,0,0) ;
  }
} /* end mm_send_ps_cnm_data() */


/*===========================================================================

FUNCTION MM_PASS_CNM_DATA

DESCRIPTION
  This function transfers messages between RR and CM. If the current
  state is IDLE, then send  MMCNM_EST_IND when a message is received
  from RR. Also, flag network_message_received, whenever a network
  message is received, and state is wait for network command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_pass_CNM_data( mm_cmd_type* msg_p )
{
   dword ota_msg_length;
   mmr_service_ind_s_type     mmr_service_ind;
   byte prev_mm_state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if ( (msg_p->cmd.hdr.message_set == MS_CC_MM ) &&
        (msg_p->cmd.hdr.message_id  == MMCNM_DATA_REQ ) )
   {
      if (GET_PD(msg_p->cmd.mmcnm_data_req.data[0]) == PD_SMS)
      {
         mm_sig_low_priority = TRUE;
      }

      GET_IMH_LEN(ota_msg_length , &msg_p->cmd.hdr);

      MSG_HIGH_DS_0(MM_SUB, "=MM= MM send CNM OTA message");
       
      mm_send_ota_message((byte *)&msg_p->cmd, ota_msg_length);
      
      /* Reset the Signaling Priority to HI after sending OTA message */
      mm_sig_low_priority = FALSE;

      mm_cnm_is_active = TRUE;
   }
   else if (is_OTA_message( msg_p ))
   {
      /*
       * This function is only called with this primitive when the content is
       * a message destined for CNM sub-layer. Therefore, we need only to send
       * the message on, and flag network_message_received
       */

      /*
       * Following are applicable states in which an MM conn can be
       * esablished. CNM Messages are ignored if received in other
       * states
       */
      boolean mm_can_recv_cnm_data_ind =
        ( mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION             ||
          mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION  ||
          mm_state == MM_CONNECTION_ACTIVE                           ||
          mm_state == MM_REESTABLISHMENT_INITIATED                   ||
          mm_state == MM_WAIT_FOR_NETWORK_COMMAND
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
                                                                     ||
          mm_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
        );

      /* If the message received is in applicable states, pass it on */
      if (mm_can_recv_cnm_data_ind == TRUE)
      {
         if (mm_state == MM_WAIT_FOR_NETWORK_COMMAND)
         {
           if(mm_cnm_rel_req_received)
           {
            /* ------------------------------------------------------------------------
             ** This function informs CNM that MM is moved to State 6 due to received
             ** NW DATA indication in state 9, this indicates CNM that MM connection is 
             ** now established by the Network.
             ** ----------------------------------------------------------------------*/
             mm_send_mmcnm_rel_cnf_nwdata(); 
           }
           /* 
           ** After ESR failure for an MT CSFB call, if UE moves to a non updated LA using REG request
           ** and Network sends call setup after LU is completed on the same connection then we 
           ** should update the UE with the current GW rat info (CR 385360).
           */
           if(mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
           {
             mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
             mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
             mm_send_mmr_reg_cnf();
             mm_reg_waiting_for_reg_cnf = FALSE;
             mm_ready_to_send_reg_cnf   = FALSE;
           }
		   prev_mm_state = mm_state;
         }

         msg_p->cmd.hdr.message_set = MS_CC_MM;
         msg_p->cmd.hdr.message_id = MMCNM_DATA_IND;

#ifdef FEATURE_DUAL_SIM
         /* Append AS ID */
         {
            uint2 length;

            GET_IMH_LEN(length, &msg_p->cmd.hdr);

            msg_p->cmd.rr_data_ind.layer3_message[length]
              = (byte) mm_sub_id;

            PUT_IMH_LEN((length + sizeof(byte)), &msg_p->cmd.hdr);
         }
#endif

         mm_stop_timer( TIMER_T3240 );  /* for test 34.2.1 step 112 */

         mm_state_control( MM_STD_NETWORK_MESSAGE_RECEIVED ); 
         mm_send_message((IMH_T *)&msg_p->cmd, GS_QUEUE_MN_CM );
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCNM_DATA_IND");

 
         if((mm_lu_start_reason == LTE_IRAT_LU) &&
            (prev_mm_state == MM_WAIT_FOR_NETWORK_COMMAND) &&
            (mm_state == MM_CONNECTION_ACTIVE) &&
            (((mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
               (mm_last_paging_cause == (byte)CS_PAGING)) ||
             ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS ||
               mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
              (mm_last_paging_cause != 0xFF))))
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= MT Page connected while LU retry ongoing");
          if (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                   gmm_stored_rai.routing_area_code,
                                   mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))) &&
                (gmm_update_status == GMM_GU1_UPDATED) &&
                (gmm_state == GMM_REGISTERED))
          {
            mm_csfb_service_ind_status = MM_CSFB_SEND_STATUS_SRV_CONNECTED;
            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
          }
          else
          {
            mm_csfb_service_ind_status = MM_CSFB_SEND_STATUS_LIMITED_REGIONAL;
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
          }
         
           MSG_HIGH_DS_1(MM_SUB ,"=MM= Set mm_csfb_service_ind_status flag [%d] ", mm_csfb_service_ind_status);
           mmr_service_ind.service_state = mm_serving_plmn.info;
           mm_send_mmr_service_ind( &mmr_service_ind );
        } 

         mm_cnm_is_active = TRUE;
         if((byte)GET_PD( msg_p->cmd.rr_data_ind.layer3_message[0])!=PD_SMS)
         {
           mm_stop_timer(TIMER_CSMT_GUARD);
           mm_waiting_for_csmt_response = FALSE;
         }
      } /* in valid states */
      else
      {
         MSG_HIGH_DS_3(MM_SUB, "=MM= Recved msg (%d-%d) in st %d - wrong mm state",
                   msg_p->cmd.rr_data_ind.layer3_message[0],
                   msg_p->cmd.rr_data_ind.layer3_message[1],
                   mm_state);
      }
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message %d",
          msg_p->cmd.hdr.message_set * 0x100 + msg_p->cmd.hdr.message_id, 0,0);
   }
}


/*===========================================================================

FUNCTION MM_CONFIRM_MM_CONNECTION

DESCRIPTION
  This function sends the confirmation of connection establishment to CM.
  This process also increments the count of MM connections

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_confirm_mm_connection(byte message_set)
{
   mmcnm_est_cnf_T       mmcnm_est_cnf;

   mmcnm_est_cnf.header.message_set = message_set;
   mmcnm_est_cnf.header.message_id  = MMCNM_EST_CNF;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( mmcnm_est_cnf_T ) - sizeof(IMH_T), &mmcnm_est_cnf.header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCNM_EST_CNF");
   
   if(message_set == MS_CC_MM_PS)
   {
     mm_stop_timer(TIMER_PS_CONN_REL);
   }

   if(mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL)
   {
      mm_connection_type = EMERGENCY_MM_CONNECTION;
   }
   else
   {
      mm_connection_type =  NORMAL_MM_CONNECTION;
   }
   MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent MMCNM_EST_CNF with mm connection type %d ",mm_connection_type);
   mm_send_message( (IMH_T *)&mmcnm_est_cnf, GS_QUEUE_MN_CM );
}


/*===========================================================================

FUNCTION MM_CONFIRM_REESTABLISH

DESCRIPTION
  This function confirms the reestablishment of former connection(s)
  to CM, as well as to flag the event to the main MM control

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_confirm_reestablish( void )
{
   mm_send_mmcc_reest_cnf();
   mm_stop_timer( TIMER_T3230 );
}


/*===========================================================================

FUNCTION MM_NOTIFY_CNM_MM_CONNECTION_FAILURE

DESCRIPTION
  This function notifies CNM that a request for a MM connection has failed.
  If reestablishment is allowed, then this is indicated in the message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_notify_cnm_mm_connection_failure( boolean reestablishment_enabled )
{
   boolean reestablishment_possible = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (reestablishment_enabled)
   {
      if (mm_system_information.reestablishment_allowed)
      {
         reestablishment_possible = TRUE;
      }
   }

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMCNM_REL_IND");

   mm_send_mmcnm_rel_ind( OTHER_CAUSE, reestablishment_possible );
}


/*===========================================================================

FUNCTION MM_NOTIFY_REESTABLISHMENT_FAIL

DESCRIPTION
  This function notifies the CNM layer of a failure of reestablishment.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_notify_reestablishment_fail( reject_type_enum_T cause_type, reject_cause_T cause_value)
{
   mm_stop_timer( TIMER_T3230 );
   mm_send_mmcc_reest_rej(cause_type, cause_value);
}


/*===========================================================================

FUNCTION MM_SEND_CM_SERVICE_ABORT

DESCRIPTION
  This function sends a CM_SERVICE_ABORT message to the network

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_cm_service_abort(void)
{
   cm_service_abort_msg_T cm_service_abort_msg;
   dword                  ota_msg_length = 2; /* PD and Type */

   memset(&cm_service_abort_msg, 0x00, sizeof(cm_service_abort_msg_T));
   cm_service_abort_msg.msg_head.IMH.message_set = MS_MM_RR;
   cm_service_abort_msg.msg_head.IMH.message_id  = (int)RR_DATA_REQ;

   cm_service_abort_msg.msg_head.skip_pd = PD_MM;
   cm_service_abort_msg.msg_head.type = CM_SERVICE_ABORT;

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_ABORT");

   mm_send_ota_message( (byte*)&cm_service_abort_msg, ota_msg_length );
}


#ifdef FEATURE_TC
/*===========================================================================

FUNCTION MM_HANDLE_TEST_CONTROL_DATA

DESCRIPTION
  This function handles the TC task related primtives

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_test_control_data(mm_cmd_type *msg_p)
{
   if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
        ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
         && get_tdscdma_interface()
#endif
)) &&
       (msg_p->cmd.hdr.message_id == (int)MM_DATA_IND))
   {
      tc_cmd_type *tc_cmd = tc_get_cmd_buf();
      uint32 msg_len;

      if (tc_cmd == NULL)
      {
         MSG_FATAL_DS(MM_SUB, "=MM= Could not allocate buffer for TC command", 0,0,0);
      }

      GET_IMH_LEN(msg_len , &msg_p->cmd.hdr);

      /* Set the header info */
      tc_cmd->header.cmd_id = MMTC_UNITDATA_IND;
      tc_cmd->header.message_set = MS_MM_TC;

      /* Fill in the data */
      tc_cmd->cmd.mm_unitdata_ind.message_len = (byte)MIN(msg_len,gmm_max_L3_msg_length(MM_CS_DOMAIN));
      (void)memscpy( (void *)tc_cmd->cmd.mm_unitdata_ind.data,gmm_max_L3_msg_length(MM_CS_DOMAIN),
                    (void *)(msg_p->cmd.rr_data_ind.layer3_message),tc_cmd->cmd.mm_unitdata_ind.message_len);

      mm_stop_timer( TIMER_T3240 );
      /* Send to  TC */
      tc_put_cmd(tc_cmd);

      mm_tc_is_active = TRUE;

   }
   if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
        ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
         && get_tdscdma_interface()
#endif
)) &&
       (msg_p->cmd.hdr.message_id == (int)RRC_SYNC_IND))
   {
      /* Tell TC only when RAB is established */
      if( msg_p->cmd.rrc_sync_ind.rab_id_present &&
          msg_p->cmd.rrc_sync_ind.rab_info.action == RAB_ESTABLISHED)
      {
         tc_cmd_type *tc_cmd = tc_get_cmd_buf();

         if (tc_cmd == NULL)
         {
            MSG_FATAL_DS(MM_SUB, "=MM= Could not allocate buffer for TC command", 0,0,0);
         }

         /* Set the header info */
         tc_cmd->header.cmd_id = RABMTC_RAB_SETUP_IND;
         tc_cmd->header.message_set = MS_MM_TC;

         tc_cmd->cmd.mm_rab_est_ind.rab_id =
            (byte)msg_p->cmd.rrc_sync_ind.rab_info.rab_id;

         /* Send to  TC */
         tc_put_cmd(tc_cmd);
      }
   }
   else if((msg_p->cmd.hdr.message_set == MS_MM_TC)  &&
           (msg_p->cmd.hdr.message_id == MMTC_UNITDATA_REQ))
   {
      if((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) ||
         (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
         (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
         (mm_state == MM_CONNECTION_ACTIVE))
      {
         uint32 msg_len;
         GET_IMH_LEN(msg_len , &msg_p->cmd.hdr);

         MSG_HIGH_DS_0(MM_SUB, "=MM= MM send TC OTA message");

         mm_send_ota_message((byte *)&msg_p->cmd, msg_len);
      }
      else
      {
         /* Send ERR_IND as MMTC_UNITDATA_REQ can not be honoured */
         tc_cmd_type *tc_cmd = tc_get_cmd_buf();

         if (tc_cmd == NULL)
         {
            MSG_FATAL_DS(MM_SUB, "=MM= Could not allocate buffer for TC command", 0,0,0);
         }

         MSG_HIGH_DS_1(MM_SUB ,"=MM= Rejecting TC unit data req at st %d", mm_state);

         /* Set the header info */
         tc_cmd->header.cmd_id = MMTC_ERROR_IND;
         tc_cmd->header.message_set = MS_MM_TC;

         tc_cmd->cmd.mm_error_ind.cause = 0;

         /* Send to  TC */
         tc_put_cmd(tc_cmd);
      }
   }
}
#endif /* FEATURE_TC */

