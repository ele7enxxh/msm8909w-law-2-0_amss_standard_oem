/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnmlog.c_v   1.12   18 Jul 2002 12:39:24   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnmlog.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
8/8/01      CD     Initial version

8/9/01      CD     Added message direction to CNM_log_message
                   Added CNM_log_decode_message_type, CNM_log_CNM_service_type

08/14/01   AB      Renamed connection manager references to CNM from CM.

8/22/01    CD      Added cmll.h

                   Divided message_id switch statement for CM<->MN messages
                   in two to account for the different definitions of message_id 
                   in mn_cm.h and cmll.h

                   Deleted cases for CM_EMPTY_SS_CONF, CM_MT_DATA_CONNECT_IND
                   since they are now obsolete

9/13/01    CD      Modified processing to avoid passing %s to MSG_HIGH
                   Added comments and documented function descriptions
                   
18-Sep-2001  VT    In CNM_log_message_type(), optimized the text being
                    printed, to avoid the message name getting chopped
                    off

10/01/01    CD     Added CNM_log_rp_mti
                   Added case MS_TIMER to CNM_log_message

9-Oct-2001  VT     In CNM_log_message_type(), wherever necessary, split 
                     each MSG_HIGH into 2, to avoid the message name 
                     getting chopped off

9-Jan-2002  VT    In CNM_log_CNM_service_type(), added cases for:
                    MO_CALL_ESTABLISHMENT_FULL_SPEECH,
                    MO_CALL_ESTABLISHMENT_HALF_SPEECH,
                    MO_CALL_ESTABLISHMENT_DATA_9600,
                    MO_CALL_ESTABLISHMENT_DATA_4800,  and
                    MO_CALL_ESTABLISHMENT_DATA_2400

04/19/02    CD    Deleted call to log CNM service type for a MMCNM_EST_CNF

                  Added case MMCC_REEST_REJ, MMCNM_EST_REJ, MMCNM_REL_CNF

                  Deleted processing of MMCNM_ABORT_IND, MMCNM_ERR_IND

                  Fixed typo in MSG_HIGH

                  Updated Copyright

7/10/02    AB     Replaced cmll.h with cmwll.h.

7/16/02    AB     Replaced sent/received with arrows, "<-, ->", to help 
                    F3 messages being truncated by the tools's 39 char 
                    limitation.

7/29/02    CD     In CNM_log_message() added logging info for a MMCNM_REL_IND

02/14/03   AB     Added supports for new MS_CC_MM_PS message set/group. Also,
                    removed lint errors.

04/16/03   AB     Corrected the F3 logging/reporting of the MMCNM_REL_REQ 
                    message.

06/10/03   AB     Updated the call statistics events log function, 
                    CNM_log_event_calls_statistics(), not to generate
                    ABNORMAL_CALL_ENDED on CC_STATE_U0 to CC_STATE_U0 state transistion.
                     
01/14/04   AB     Added cnm_log_rp_event(), to log SMS RP events.

05/16/05   HS     Added support for FEATURE_CCBS

06/15/05   HS     Added CCBS related call events in CNM_log_event_calls_statistics()
===========================================================================*/


/* Include Files */
#include "mmcp_variation.h"
#include <stdio.h>
#include <string.h>
#include "customer.h"
#include "rex.h"
#include "msg.h"
#include "err.h"
#include "mn.h"
#include "mnglobal.h"
#include "timers_v.h"
#include "ms.h"
#include "ms_timer_v.h"
#include "naslog_v.h"            /* LOGs and EVENTS IDs */
#include "cc_cc.h"
#include "ms_cc_mm_v.h"
#include "com_iei_v.h"
#include "cnm.h"
/* ----------------------------------------------------------------------- 
** REMOVED compiler warning from inclusion of memheap.h 
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MEM_DEBUG
#undef FEATURE_MEM_DEBUG
#endif
#include "cmwll.h"
#include <stringl/stringl.h>



/*******************************************************************************
 *
 *  Function name: CNM_log_rp_event
 *  --------------------------------
 *  Description: This function logs the message type indicator of an RP message
 *  
 *
 *  Parameters:
 *  -----------
 *  IN:   mti   byte containing the type of RP message (RP_DATA, RP_ACK, RP_ERROR,
 *              RP_SMMA)
 *  
 *  OUT:  (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/

VOID_FUNC CNM_log_rp_event(byte mti, byte mref, byte mcause)
{
    byte rp_event_data[3];
   
    switch(mti)
    {
        case RP_DATA_MS:
        case RP_DATA_N:
        case RP_ACK_MS:
        case RP_ACK_N:
        case RP_SMMA_MS:
            mcause = FALSE;
            break;

        case RP_ERROR_MS:
        case RP_ERROR_N:
        default:
            break;
    }

    rp_event_data[0] = mti;
    rp_event_data[1] = mref;
    rp_event_data[2] = mcause;

    event_report_payload(EVENT_SMS_STATISTICS, 3, rp_event_data);

    MSG_MED_DS_3(MN_SUB,"=MNCNM= SMS: mti=%d, mref=%d, mcause=%d", mti, mref, mcause);

}

/*******************************************************************************
 *
 *  Function name: CNM_log_message
 *  ------------------------------
 *  Description:  This function logs the receipt or sending of OTA or intertask
 *  ------------  messages for the CNM or MN subtasks.
 *                It bases the subtask on the message_set, and the message direction
 *                on the message_id.
 *                For OTA messages, this function also calls other routines to log the
 *                message type or the CNM service type as appropriate.
 *  
 *                OTA messages are logged as MSG_HIGH when received and sent.
 *                Intertask messages are logged as MSG_HIGH when received and as 
 *                MSG_MED when sent.
 *
 *  Parameters:
 *  -----------
 *  IN:  message_p      pointer to the message
 *
 *  OUT: (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/

VOID_FUNC CNM_log_message(IMH_T *message_p)
{
   /*
   ** MESSAGE SET ?
   */
   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= MS:0x%X   MSG_ID:0x%X ", message_p->message_set, message_p->message_id);
   switch( message_p->message_set)
   {
	  
      case MS_CC_MM:
      case MS_CC_MM_PS:

         /*
          ** MESSAGE ID ?
          */
		 switch( message_p->message_id )
         {

            case MMCNM_DATA_REQ:
                {
                    byte   *msg_idx;

					/* Align pointer with start of message structure */
                    msg_idx = (byte*)message_p;

                    /* Skip the IMH_T */
                    msg_idx += sizeof(IMH_T);
 
                    /* log protocol discriminator and message type */
                    CNM_log_message_type(*msg_idx,      /* ti_pd */
                                         *(msg_idx + 1));/* msg_type */
                    break;
                }

            case MMCNM_EST_REQ:
                {
                     byte *msg_idx;

                    /* Align pointer with start of message structure */
                    msg_idx = (byte*)message_p;

                    /* Skip the IMH_T */
                    msg_idx += sizeof(IMH_T);

                    /* Log the CNM service type */
                    CNM_log_CNM_service_type(*msg_idx);
                    break;
                }

            case MMCC_REEST_REQ:
			   break;

            case MMCNM_REL_REQ:
                {

                  if(mm_rel_req_sent == FALSE)
                  {
                    MSG_MED_DS_0(MN_SUB,"=MNCNM= CNM -> MMCNM_REL_REQ -> MM");
                  }
                }
                break;

            case MMCNM_UNIT_DATA_REQ:
			   break;

            case MMCNM_DATA_IND:
                {
                    byte   *msg_idx;

					/* Align pointer with start of message structure */
                    msg_idx = (byte*)message_p;

                    /* Skip the IMH_T */
                    msg_idx += sizeof(IMH_T);

                    /* Log protocol discriminator and message type */
                    CNM_log_message_type(*msg_idx,      /* ti_pd */
                                         *(msg_idx + 1));/* msg_type */
                    break;
                }

            case MMCNM_EST_REJ:
                {
 
                    break;
                }

            case MMCNM_EST_CNF:
                {
                    break;
                }

            case MMCNM_REL_IND:
                {
                    byte   *msg_idx;

                    /* Align pointer with start of message structure */
                    msg_idx = (byte*)message_p;

                    /* Skip the IMH_T */
                    msg_idx += sizeof(IMH_T);

                    /* log reest flag and rel_cause */
                    MSG_HIGH_DS_2(MN_SUB,"=MNCNM= reest_possible=%d rel_cause=%d",*msg_idx, *(msg_idx + 1));

                    break;
                }

            case MMCNM_REL_CNF:
               break;

            case MMCC_REEST_CNF:
               break;

            case MMCC_REEST_REJ:
               break;

            case MMCC_SYNC_IND:
               break;

            case MMCNM_UNIT_DATA_IND:
               break;

#ifdef FEATURE_CCBS
            case MMCNM_PROMPT_IND:
               break;

            case MMCNM_PROMPT_RES:
               break;
#endif /* FEATURE_CCBS */

            case MMCC_SIG_CONN_REL_IND:
               break;

            case MMCNM_SRVCC_HANDOVER_COMPLETE_IND:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM received unknown message id %02X",
                   message_p->message_id);
               break;
         }
         break;

   case MS_MN_CC:
		 
         /*
          ** MESSAGE ID ?
          */
         switch( message_p->message_id )
         {
            case MNCC_ALERT_REQ:
               break;

            case MNCC_CALL_CONF_REQ:
               break;

            case MNCC_CALL_HOLD_REQ:
               break;

            case MNCC_CALL_RETRIEVE_REQ:
               break;

            case MNCC_DISC_REQ:
               break;

            case MNCC_FACILITY_REQ:
               break;

            case MNCC_REJ_REQ:
               break;

            case MNCC_REL_REQ:
               break;

            case MNCC_REL_COMPL_REQ:
               break;

            case MNCC_MODIFY_REJ:
               break;

            case MNCC_MODIFY_REQ:
               break;

            case MNCC_MODIFY_RES:
               break;

            case MNCC_NOTIFY_REQ:
               break;

            case MNCC_SETUP_COMPL_REQ:
               break;

            case MNCC_SETUP_COMPL_RES:
               break;

            case MNCC_SETUP_REQ:
               break;

            case MNCC_SETUP_RES:
               break;

            case MNCC_START_DTMF_REQ:
               break;

            case MNCC_STOP_DTMF_REQ:
               break;

#ifdef FEATURE_UUS
            case MNCC_USER_USER_DATA_REQ:
               break;
#endif

            case MNCC_MODIFY_ACCEPT:
               break;

		    case MNCC_SRVCC_HANDOVER_FAIL_REQ:
               break;

            case MNCC_ALERT_IND:
               break;

            case MNCC_CALL_HOLD_CNF:
               break;

            case MNCC_CALL_HOLD_REJ:
               break;

            case MNCC_CALL_PROC_IND:
               break;

            case MNCC_CALL_RETRIEVE_CNF:
               break;

            case MNCC_CALL_RETRIEVE_REJ:
               break;

            case MNCC_DISC_IND:
               break;

            case MNCC_ERR_IND:
               break;

            case MNCC_FACILITY_IND:
               break;

            case MNCC_MODIFY_CNF:
               break;

            case MNCC_MODIFY_IND:
               break;

            case MNCC_NOTIFY_IND:
               break;

            case MNCC_PROGRESS_IND:
               break;

            case MNCC_REEST_IND:
               break;

            case MNCC_REJ_IND:
               break;

            case MNCC_REL_CNF:
               break;

            case MNCC_REL_IND:
               break;

            case MNCC_SETUP_CNF:
               break;

            case MNCC_SETUP_COMPL_IND:
               break;

            case MNCC_SETUP_IND:
               break;

            case MNCC_START_DTMF_CNF:
               break;

            case MNCC_STOP_DTMF_CNF:
               break;

            case MNCC_SYNC_IND:
               break;

            case MNCC_RAB_REL_IND:
               break;

#ifdef FEATURE_UUS
            case MNCC_USER_USER_DATA_IND:
               break;

            case MNCC_CONGESTION_IND:
               break;
#endif

#ifdef FEATURE_CCBS
            case MNCC_RECALL_IND:
               break;
#endif /* FEATURE_CCBS */

            case MNCC_CS_SIG_CONN_REL_IND:
               break;

            case MNCC_SRVCC_HANDOVER_COMPLETE_IND:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM received/sent unknown msg id %02X from/to MN",
                   message_p->message_id);
            break;
         }
         break;

      case MS_MN_SS:

         /*
          ** MESSAGE ID ?

          */
         switch( message_p->message_id )
         {
            case MNSS_BEGIN_REQ:
               break;

            case MNSS_END_REQ:
               break;

            case MNSS_FACILITY_REQ:
               break;

            case MNSS_BEGIN_IND:
               break;

            case MNSS_END_IND:
               break;

            case MNSS_FACILITY_IND:
               break;

            case MNSS_REJ_IND:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM received/sent unknown message id %02X from/to MN",
                   message_p->message_id);
            break;
         }
         break;

      case MS_MN_SMS:

         /*
          ** MESSAGE ID ?
          */

         switch( message_p->message_id)
         {
            case MNSMS_EST_REQ:
               {
                   MNSMS_EST_REQ_T   mnsms_est_req;

                   /* Log RP events */
                   memscpy(&mnsms_est_req, sizeof(MNSMS_EST_REQ_T), message_p, sizeof(MNSMS_EST_REQ_T));
                   CNM_log_rp_event(mnsms_est_req.rpdu.data[0], 
                                    mnsms_est_req.rpdu.data[1],
                                    mnsms_est_req.rpdu.data[2]);
               }
               break;

            case MNSMS_DATA_REQ:
               {
                   MNSMS_DATA_REQ_T   mnsms_data_req;

                   /* Log RP events */
                   memscpy(&mnsms_data_req, sizeof(MNSMS_DATA_REQ_T), message_p, sizeof(MNSMS_DATA_REQ_T));
                   CNM_log_rp_event(mnsms_data_req.rpdu.data[0], 
                                    mnsms_data_req.rpdu.data[1],
                                    mnsms_data_req.rpdu.data[2]);
               }
               break;

            case MNSMS_ABORT_REQ:
               break;

            case MNSMS_REL_REQ:
               break;

            case MNSMS_EST_IND:
               break;

            case MNSMS_DATA_IND:
               {
                   MNSMS_DATA_IND_T   mnsms_data_ind;

                   /* Log the RP message type */
                   memscpy(&mnsms_data_ind, sizeof(MNSMS_DATA_IND_T) ,message_p, sizeof(MNSMS_DATA_IND_T));
                   CNM_log_rp_mti(mnsms_data_ind.rpdu.data[0]);

                   /* Log RP events */
                   CNM_log_rp_event(mnsms_data_ind.rpdu.data[0], 
                                    mnsms_data_ind.rpdu.data[1],
                                    mnsms_data_ind.rpdu.data[2]);

                   break;
               }

            case MNSMS_ERROR_IND:
               break;

			case MNSMS_CPACK_IND:
               break;


            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM received/sent unknown message id %02X from/to MN",
                   message_p->message_id);
            break;
         }
         break;


      case MS_TIMER:
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= CNM timer expiration");
         break;

      case MS_MN_TEST:
         MSG_MED_DS_1(MN_SUB,"=MNCNM= CNM received/sent MS_MN_TEST, msg_id = %02X",message_p->message_id);
         break;

      default:
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= CNM received/sent unknown message_set %02X, message_id %02X",
                 message_p->message_set,
                 message_p->message_id);

         break;
   }
          
}

/*******************************************************************************
 *
 *  Function name: CNM_log_message_type
 *  ------------------------------
 *  Description: This function logs the OTA message protocol discriminator
 *  ------------ and message type
 *  
 *
 *  Parameters: 
 *  -----------
 *   IN:  ti_pd   byte containing the transaction identifier and protocol discriminator
 *        m_ref   byte containing the messge type (see 24.008, section 10.4)
 *   
 *   OUT  (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/

VOID_FUNC CNM_log_message_type( byte ti_pd,byte m_ref)
{
   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= CNM msg infoPD=0x%X, OTA_msg_type=0x%X", (ti_pd & 0x0f),m_ref);
   switch (ti_pd & 0x0f)
   {
      
      case PD_CC:
         switch (m_ref)
         {

            case ALERTING:
               break;

            case CALL_CONFIRMED:
               break;

            case CALL_PROCEEDING:
               break;

            case CONNECT:
               break;

            case CONNECT_ACK:
               break;

            case EMERGENCY_SETUP:
               break;

            case PROGRESS:
               break;

            case SETUP:
               break;

            case MODIFY:
               break;

            case MODIFY_COMPLETE:
               break;

            case MODIFY_REJECT:
               break;

#ifdef FEATURE_UUS
            case USER_INFORMATION:
               break;

            case CONGESTION_CONTROL:
               break;
#endif

            case HOLD:
               break;

            case HOLD_ACK:
               break;

            case HOLD_REJECT:
               break;

            case RETRIEVE:
               break;

            case RETRIEVE_ACK:
               break;

            case RETRIEVE_REJ:
               break;

            case DISCONNECT:
               break;

            case RELEASE:
               break;

            case RELEASE_COMPLETE:
               break;

            case NOTIFY:
               break;

            case STATUS_MSG:
               break;

            case STATUS_ENQUIRY:
               break;

            case START_DTMF:
               break;

            case STOP_DTMF:
               break;

            case STOP_DTMF_ACK:
               break;

            case START_DTMF_ACK:
               break;

            case START_DTMF_REJ:
               break;

            case FACILITY:
               break;

#ifdef FEATURE_CCBS
            case CC_ESTABLISHMENT:
               break;

            case CC_ESTABLISHMENT_CONFIRMED:
               break;

            case RECALL:
               break;

            case START_CC:
               break;
#endif /* FEATURE_CCBS */

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM msg info:PD_CC, unknown msg_type=%02X",m_ref);
               break;
         }
         break;

      case PD_SMS:
         switch (m_ref)
         {

            case CP_DATA:
               break;

            case CP_ACK:
               break;

            case CP_ERROR:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM msg info:PD_SMS, unknown msg_type=%02X",m_ref);
               break;
         }
         break;

      case PD_SS:
         switch (m_ref)
         {

            case FACILITY:
               break;

            case REGISTER:
               break;

            case RELEASE_COMPLETE:
               break;

            default:
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM msg info:PD_SS, unknown msg_type=%02X",m_ref);
               break;
         }
         break;

      default:
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= CNM msg info:unknown PD=%02X, msg_type=%02X",
              ti_pd & 0x0f,m_ref);
         break;
   }
}

   /*******************************************************************************
 *
 *  Function name: CNM_log_CNM_service_type
 *  ------------------------------
 *  Description: This function logs the CNM service type of a MMCNM_EST_REQ or
 *  ------------ MMCNM_EST_CNF OTA message
 *  
 *
 *  Parameters:
 *  -----------
 *  IN:   CNM_service   byte containing the type of CNM service
 *  
 *  OUT:  (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/

VOID_FUNC CNM_log_CNM_service_type( byte CNM_service)
{
    /* CNM service type values are defined in sys_cnst.h*/
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM service type = 0x%X", CNM_service );
    switch(CNM_service)
    {
    case MO_CALL_ESTABLISHMENT:
       break;

    case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
       break;

    case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
       break;

    case MO_CALL_ESTABLISHMENT_DATA_9600:
       break;

    case MO_CALL_ESTABLISHMENT_DATA_4800:
       break;

    case MO_CALL_ESTABLISHMENT_DATA_2400:
       break;

    case EMERGENCY_CALL_ESTABLISHMENT:
       break;

    case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
       break;

    case SUPPLEMENTARY_SERVICE_ACTIVATION:
       break;

    case NO_CNM_SERVICE:
       break;

    default:
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Msg has unknown CNM service type = %02X", CNM_service);
       break;
    }
}

 /*******************************************************************************
 *
 *  Function name: CNM_log_rp_mti
 *  ------------------------------
 *  Description: This function logs the message type indicator of an RP message
 *  
 *
 *  Parameters:
 *  -----------
 *  IN:   mti   byte containing the type of RP message (RP_DATA, RP_ACK, RP_ERROR,
 *              RP_SMMA)
 *  
 *  OUT:  (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/

VOID_FUNC CNM_log_rp_mti(byte mti)
{
    MSG_MED_DS_1(MN_SUB,"=MNCNM= RP msg type  = 0x%X", mti );
    switch(mti)
    {
    case RP_DATA_MS:
        break;
    case RP_DATA_N:
        break;
    case RP_ACK_MS:
        break;
    case RP_ACK_N:
        break;
    case RP_ERROR_MS:
        break;
    case RP_ERROR_N:
        break;
    case RP_SMMA_MS:
        break;
    default:
        MSG_MED_DS_0(MN_SUB,"=MNCNM= RP msg type = RESERVED");
        break;
    }

}




 /*******************************************************************************
 *
 *  Function name: CNM_log_calls_events
 *  -----------------------------------
 *  Description: This function logs the calls statistics events indicator or
 *  state change event. 
 *
 *  Parameters:
 *  -----------
 *  IN:   call_indicator : calls statistic indicator as defined in naslog.h.
 *  
 *  OUT:  (none)
 *
 *  Returns:  void
 *  --------
 *
 ******************************************************************************/
VOID_FUNC CNM_log_event_calls_statistics(connection_id_T connection_id, byte curr_state, byte nextstate)
{
    byte events = FALSE;
/*lint -e578 */
    static byte prev_state[MAXNO_CC_TRANSACTIONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/*lint +e578 */

    if(connection_id < MAXNO_CC_TRANSACTIONS)
    {
        byte service_type;

        if(mn_call_information_p[connection_id] != NULL)
        {
            service_type = mn_call_information_p[connection_id]->CNM_service_type;
        }
        else
        {
            service_type = NO_CNM_SERVICE;
        }

        if(connection_id <= MAX_MO_CONNECTION_ID)  /* MO */
        {
            if(curr_state == nextstate)
            {
                events = FALSE;
            }
            else if((curr_state == CC_STATE_U0)&&(nextstate == CC_STATE_U0_1))   /* CC_STATE_U0 -> CC_STATE_U0_1 */
            {
                switch (service_type)
                {
                    case MO_CALL_ESTABLISHMENT:
                    case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
                    case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
                        events =  (byte)MO_NORMAL_CALL_ATTEMPTED;
                        break;
                    case MO_CALL_ESTABLISHMENT_DATA_9600:
                    case MO_CALL_ESTABLISHMENT_DATA_4800:
                    case MO_CALL_ESTABLISHMENT_DATA_2400:
                        events = (byte)MO_CS_DATA_CALL_ATTEMPTED;
                        break;

                    case EMERGENCY_CALL_ESTABLISHMENT:
                        events = (byte)MO_EMERGENCY_CALL_ATTEMPTED;
                        break;

                    case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
                    case SUPPLEMENTARY_SERVICE_ACTIVATION:
                    default:
                        events = FALSE;
                        break;
                } /* service type */
            }
#ifdef FEATURE_CCBS
            else if((curr_state == CC_STATE_U0_2)&&(nextstate == CC_STATE_U0_3)) /* CC_STATE_U0_2 -> CC_STATE_U0_3 */
            {
                events =  (byte)MO_NIMO_CALL_NOTIFIED;
                  /* Network initiated MO call request accepted by CC. Still
                   * need user action for call to go through */
            }
            else if((curr_state == CC_STATE_U0_6)&&(nextstate == CC_STATE_U1))   /* CC_STATE_U0_6 -> CC_STATE_U1 */
            {
                events =  (byte)MO_NORMAL_CALL_ATTEMPTED;  /* accepting recall */
            }
#endif /* FEATURE_CCBS */
            else if((curr_state == CC_STATE_U0_1)&&(nextstate == CC_STATE_U0))   /* CC_STATE_U0 -> CC_STATE_U0_1 */
            {
                events = (byte)MO_MS_CALL_ABORTED;  /* MM EST Rejected */
            }
            else if(((curr_state == CC_STATE_U3) || (curr_state == CC_STATE_U4) ||
                     (curr_state == CC_STATE_U7) || (curr_state == CC_STATE_U8) ||
#ifdef FEATURE_CCBS
                     (curr_state == CC_STATE_U0_6) ||
#endif /* FEATURE_CCBS */
                     (curr_state == CC_STATE_U9) || (curr_state == CC_STATE_U10)) && 
                     (nextstate == CC_STATE_U0))   /* CC_STATE_U0.6,CC_STATE_U3,CC_STATE_U4,CC_STATE_U7,CC_STATE_U8,CC_STATE_U9,CC_STATE_U10 -> CC_STATE_U0 */
            {
                events = (byte)MO_NORMAL_CALL_ENDED;  /* Normal Call Ended */
            }
            else if(((curr_state == CC_STATE_U1)&&(nextstate == CC_STATE_U10)) ||   /* CC_STATE_U1, CC_STATE_U3 or CC_STATE_U4 -> CC_STATE_U10 */
                    ((curr_state == CC_STATE_U3)&&(nextstate == CC_STATE_U10)) ||
                    ((curr_state == CC_STATE_U4)&&(nextstate == CC_STATE_U10)))
            {
                events = (byte)MO_NORMAL_CALL_CONNECTED;
            }
            else if((curr_state == CC_STATE_U10)&&(nextstate == CC_STATE_U11))
            {
                events = (byte)MO_MS_CALL_CLEARING_ATTEMPTED;
            }
            else if((curr_state == CC_STATE_U10)&&(nextstate == CC_STATE_U12))
            {
                events = (byte)MO_NW_CALL_CLEARING_ATTEMPTED;
            }
            else if((curr_state != CC_STATE_U10)&&(nextstate == CC_STATE_U11)) 
            {
                events = (byte)MO_MS_CALL_ABORTED;
            }
            else if((curr_state != CC_STATE_U10)&&(nextstate == CC_STATE_U12))
            {
                events = (byte)MO_NW_CALL_ABORTED;
            }
            else if(((curr_state == CC_STATE_U11)&&(nextstate == CC_STATE_U0)) ||
                    ((curr_state == CC_STATE_U19)&&(nextstate == CC_STATE_U0)))
            {
                if(prev_state[connection_id] != CC_STATE_U0)
                {
                    events = (byte)MO_NORMAL_CALL_ENDED;
                }
                else
                {
                    events = FALSE;  /* already reported */
                }
            }
            else if(((curr_state != CC_STATE_U11)&&(nextstate == CC_STATE_U0)) ||
                    ((curr_state != CC_STATE_U19)&&(nextstate == CC_STATE_U0)))
            {
                events = (byte)MO_ABNORMAL_CALL_ENDED;
            }
            else
            {
                events = FALSE;
            }
        }  /* MO Calls */
        else if(connection_id >= MT) 
        {
            if(curr_state == nextstate)
            {
                events = FALSE;
            }
            else if((curr_state == CC_STATE_U0)&&(nextstate == CC_STATE_U6))
            {
                events = (byte)MT_NORMAL_CALL_ATTEMPTED;
            }
            else if((curr_state == CC_STATE_U6)&&(nextstate == CC_STATE_U0))
            {
                events = (byte)MT_MS_CALL_ABORTED;
            }
            else if(((curr_state == CC_STATE_U3) || (curr_state == CC_STATE_U4) ||
                     (curr_state == CC_STATE_U7) || (curr_state == CC_STATE_U8) ||
                     (curr_state == CC_STATE_U9) || (curr_state == CC_STATE_U10)) && 
                     (nextstate == CC_STATE_U0))   /* CC_STATE_U0 -> CC_STATE_U0_1 */
            {
                events = (byte)MT_NORMAL_CALL_ENDED;  /* Normal Call Ended */
            }
            else if((curr_state == CC_STATE_U8)&&(nextstate == CC_STATE_U10))
            {
                events = (byte)MT_NORMAL_CALL_CONNECTED;
            }
            else if((curr_state == CC_STATE_U10)&&(nextstate == CC_STATE_U11))
            {
                events = (byte)MT_MS_CALL_CLEARING_ATTEMPTED;
            }
            else if((curr_state == CC_STATE_U10)&&(nextstate == CC_STATE_U12))
            {
                events = (byte)MT_NW_CALL_CLEARING_ATTEMPTED;
            }
            else if((curr_state != CC_STATE_U10)&&(nextstate == CC_STATE_U11)) 
            {
                events = (byte)MT_MS_CALL_ABORTED;
            }
            else if((curr_state != CC_STATE_U10)&&(nextstate == CC_STATE_U12))
            {
                events = (byte)MT_NW_CALL_ABORTED;
            }
            else if(((curr_state == CC_STATE_U11)&&(nextstate == CC_STATE_U0)) ||
                    ((curr_state == CC_STATE_U19)&&(nextstate == CC_STATE_U0)))
            {
                if(prev_state[connection_id] != CC_STATE_U0)
                {
                    events = (byte)MT_NORMAL_CALL_ENDED;
                }
                else
                {
                    events = FALSE;  /* already reported */
                }
            }
            else if(((curr_state != CC_STATE_U11)&&(nextstate == CC_STATE_U0)) ||
                    ((curr_state != CC_STATE_U19)&&(nextstate == CC_STATE_U0)))
            {
                events = (byte)MT_ABNORMAL_CALL_ENDED;
            }
            else
            {
                events = (byte)FALSE;
            }
        }  /* MT Calls */
        else
        {
            events = FALSE;
        }

        prev_state[connection_id] = nextstate;   /* save last state */

    } /* connection_id < MAX_CC_TRANSACTIONS */
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection=%d", connection_id);
    }

    if(events)
    {
        event_report_payload(EVENT_UMTS_CALLS_STATISTICS, 1, &events);
    }
}

