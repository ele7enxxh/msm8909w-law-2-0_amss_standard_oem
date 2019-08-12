/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_sms_processing.c_v   1.14   17 Jun 2002 17:24:58   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_sms_processing.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes
                   Modified show_rl_state 

5/25/01     CD     Deleted redundant declaration of sms_transaction_information.
                   It is already declared in mn_sms.h

                   Removed ms.h since it was not being used

6/19/01     CD     Corrected usage of MSG_MED to avoid ADS110 compiler error
                   Deleted #include stdio.h

7/12/01     CD     Modified logging of state transitions

7/16/01     CD     In case WAIT_FOR_RETRANS_TIMER, event s_mo_abort_req,  changed
                   sm_rl_report to FALSE, and rel_req to FALSE

                   In case WAIT_FOR_RETRANS_TIMER event s_mo_smma_abort_req, added 
                   retrans_flag = FALSE

7/17/01     CD     Added log messages when retrans_flag is reset

7/18/01     CD     Removed duplicate external declaration of sm_rl_transaction_information
                   Removed duplicate external declaration of rl_mo_mr

8/02/01     CD     Changed state transition log message from MSG_MED to MSG_HIGH
                   Removed queue.h since it was not needed anymore

8/13/01     CD     Changed sm_rl_report flag to TRUE for the case where the SMR
                   entity is in the WAIT_FOR_RP_ACK state and the trigger is
                   s_mo_abort_req

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

9/10/01     CD     Corrected formatting of messages to avoid use of %s

9/15/01     CD     Improved information reported in MSG_HIGH

10/03/01    CD     Initialized return_data in NM_control_SM_RL
                   Set the flag rel_req = TRUE in case s_sms_ack
                   Set the rel_req flag accordingly in case s_mt_sms_request

                   Corrected implementation of s_mt_sms_request for the
                   WAIT_TO_SEND_RP_ACK state.  Changed success and report_req flags
                   to FALSE ( see 24.011 v 3.5.0, 6.3.1).  Deleted duplicate action to send 
                   RP_ERROR w/ cause set to wrong state;  it is now done once
                   in MN_get_cp_user_data()

                   Added logging information when reporting unexpected events, and
                   changed macro ERR to MSG_ERR

                   Added stimulus s_sms_to_idle to change state to idle

                   Corrected implementation of s_sms_per_error, s_sms_ack and 
                   s_cnm_error

                   Corrected implementation of s_sms_temp_error

1/29/02     RG     When the s_mo_smma_abort_req is received the retrans_flag should be 
                   set to TRUE. This is to indicate that there should NOT be any 
                   re-transmissions.

4/22/02     CD     Corrected connection_id assignment in MN_get_new_sms_connection_id

6/12/02     RG     Fix for CR 22313 (enable the sm_rl_report flag in line 258 to send the 
                   TR2M expiry report indication to WMS)

6/17/02     RG     In the WAIT_TO_SEND_RP_ACK state, enabled sending report (sm_rl_report flag) 
                   for the s_cnm_error case which captures any CP_ERROR or lower layer error.
                   Updated file history.

7/2/03      RG     Fix for CR 32105

12/15/04    AB     Added supports for AT+CMMS=1, ATCOP multiple MO SMS.

11/09/05    AB      Added supports for FEATURE_REL5 via NV_FORCE_UE_MSCR_R99_I

11/02/06    NR     Fixed lint errors

07/04/11    AM     Range check has been put for sms_transaction_information
                   and replacing oxff with MN_INVALID_CALL_SESSION 
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "cause.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "ms.h"
#include "cnm.h"
#include <stringl/stringl.h>
#include "nasutils.h"


extern VOID_FUNC MN_put_MNIMS_MO_RPDU(void);
extern VOID_FUNC CNM_send_CPACK_IND( connection_id_T  connection_id, byte error_status, byte cause_value);


/*
 * Function name :  MN_control_SMS_RL
 * -------------
 *
 * Description :  Controls the MN SMS RL layer in accordance
 * -----------    with GSM 04.11 v4.9.0 s6.3.1 - s6.3.3
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id_T
 * ----------  byte  (sms_rl_stimulus)
 *             
 *             
 *
 * Returns:    control_sm_rl_return_data_T
 * --------
 *
 */

control_sm_rl_return_data_T MN_control_SMS_RL(connection_id_T connection_id,
                                              byte            sms_rl_stimulus)
{
   byte                             rl_state = 0;
   sm_rl_transaction_information_T  *sm_data_ptr;
   control_sm_rl_return_data_T      return_data;
   byte                             rel_cause = 0;

   /* Initialize the return data */
   return_data.rel_req = FALSE;
   return_data.sm_rl_report = FALSE;
   return_data.success = FALSE;

   if (connection_id < MAXNO_SMS_TRANSACTIONS)
   { 
      sm_data_ptr = &(sm_rl_transaction_information[connection_id]);
   
      /* Get state variables from SM RL global data */

      rl_state = sm_data_ptr->rl_state;

      /* The control state machine */

      switch (rl_state)
      {

      case IDLE:
         {
            switch (sms_rl_stimulus)
            {

            case s_mt_sms_request:
               {
                   /*
                   * TS 24.011, 6.3.1
                   *    - start TR2M
                   *    - transition state to WAIT_TO_SEND_RP_ACK
                   *    - set report flag to FALSE
                   *    - set rel_req flag to FALSE
                   */
                  MN_start_sms_timer(connection_id,
                                  TR2M);    /* SMS timer id */

                  sm_data_ptr->rl_state = WAIT_TO_SEND_RP_ACK;
                  sm_data_ptr->smma_transaction = FALSE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            case s_mo_sms_req:
               {
                  if(mo_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS)
                  {
                    MN_put_MNIMS_MO_RPDU();
                    /* Start TR1M Timer here only in case of WMS-IMS interface */
                    /* Start the MN timer just before sending the RP-DATA */
                    MN_start_sms_timer(connection_id,TR1M); /* SMS timer id */
                  }
                  else
                  {
                    MN_put_MNSMS_EST_REQ();
                  }
                  /* TR1M is started at the time of sending the CP-DATA(RP-DATA) to
                  to the network in the CNM */

                  sm_data_ptr->rl_state = WAIT_FOR_RP_ACK;
                  sm_data_ptr->smma_transaction = FALSE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  /*
                   * GCF issue. If the link control value is ... and timer is running (which would arise 
                      only if it were started before sending RP ACK), restart the timer
                   */
                  if(sms_link_control.mode)
                  {
                     sms_link_control.status = SMS_LINK_CONTROL_INACTIVE;
                     MN_stop_sms_timer(connection_id, TLC1M);/*Stop timer*/
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Stopping the TLC1M timer");
                  }
                  break;
               }

            case s_mo_smma_req:
               {
                  /* TR1M is started at the time of sending the CP-DATA(RP-DATA) to
                  to the network in the CNM */

                  sm_data_ptr->rl_state = WAIT_FOR_RP_ACK;
                  sm_data_ptr->smma_transaction = TRUE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  break;
               }

            case s_mo_abort_req:
               {
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  break;
               }

            case s_cnm_error:
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  break;

            case sms_TR2M_timer_expiry:      /* ignore */
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  break;

            case s_sms_to_idle: /* no need to change states here */
               {
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            default:
               {
                  return_data.success = FALSE;
                  return_data.sm_rl_report = FALSE;

                  MSG_ERROR_DS_2(MN_SUB,"=MNCNM= SMR-%d in IDLE recd unexpected evt %d",
                        connection_id,sms_rl_stimulus);


                  break;
               }
            }

            break;
         }


      case WAIT_TO_SEND_RP_ACK:
         {
            switch (sms_rl_stimulus)
            {

            case s_mo_abort_req:
               {
                  MN_stop_sms_timer( connection_id,
                                     TR2M);     /* SMS timer id */

                  MN_put_MNSMS_ABORT_REQ(connection_id, 
                                         PROTOCOL_ERROR_UNSPECIFIED);

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  break;
               }

            case sms_TR2M_timer_expiry:
               {
                  MN_put_MNSMS_ABORT_REQ(connection_id, 
                                         PROTOCOL_ERROR_UNSPECIFIED);

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = TRUE;
                  break;
               }

            case s_report_req:
               {
                  MN_stop_sms_timer(connection_id, TR2M);

                  if (sm_data_ptr->mtf_flag == TRUE)
                  {
                     return_data.rel_req = FALSE;
                  }
                  else
                  {
                     return_data.rel_req = TRUE;
                  }

                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  sm_data_ptr->rl_state = WAIT_FOR_CP_ACK;

                  /*  Do post a message to MN here "MNSMS_CPACK_IND"  in case of SMSoIMSoLTE 
                   *  So that from the MN point of view the Processing of the MT SMS transaction gets complete.
                   */
                  
                  /* Check if WMS callback is registered for SMS over IMS
                   *  If yes, then check if CNM state for any MT transaction is non IDLE
                   *  If yes, then MT SMS path is over MM connection and not IMS
                   *  */

                  if(CNM_is_GW_MT_SMS_in_progress() == FALSE)
                  {
                    CNM_send_CPACK_IND(connection_id, 0, 0);
                  }

                  break;
               }

            case s_mt_sms_request:
               {
                  /*  (TS 24.011, 6.3.1)
                  *    - stop TR2M
                  *    - transition state to IDLE
                  *    - set report flag to FALSE
                  *    - set rel_req flag to FALSE
                  *    - set sucess flag to FALSE (this causes the calling function
                  *      to send an RP_ERROR and a release request)
                  */
                  MN_stop_sms_timer(connection_id, TR2M);   /* SMS timer id */

                  return_data.success = FALSE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  sm_data_ptr->rl_state = IDLE;
                  break;
               }

            case s_cnm_error:
               {
                  MN_stop_sms_timer(connection_id,
                                     TR2M);   /* SMS timer id */

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  /* 
                   * Send report to higher layers when there is any lower layer
                   * error.
                   */
                  return_data.sm_rl_report = TRUE;
                  break;
               }

            case s_sms_to_idle: 
               {
                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            default:
               {
                  MN_stop_sms_timer(connection_id,
                                     TR2M);

                  return_data.success = FALSE;
                  return_data.rel_req = FALSE;
                  return_data.sm_rl_report = FALSE;
                  sm_data_ptr->rl_state = IDLE;

                  MSG_ERROR_DS_2(MN_SUB,"=MNCNM= SMR-%d-W_TO_S_RP_ACK recd unexpct evt %d",
                          connection_id,sms_rl_stimulus);
                  break;
               }
            }

            break;

         /* case WAIT_TO_SEND_RP_ACK */
         }

      case WAIT_FOR_RP_ACK:
         {
            switch (sms_rl_stimulus)
            {

            case s_mo_abort_req:
               {
                  MN_stop_sms_timer(connection_id, TR1M);   /* SMS timer id */

                  MN_put_MNSMS_ABORT_REQ(connection_id, 
                                         PROTOCOL_ERROR_UNSPECIFIED); 
                                                            /* Cause value */

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = TRUE;
                  break;
               }


            case s_mo_smma_abort_req:
               {
                  /* Setting the retrans_flag to TRUE (previous value was
                  FALSE). This means that there should NOT be any re-transmissions*/
                  sm_data_ptr->retrans_flag = TRUE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to FALSE",connection_id);
                  break;
               }

            case s_sms_ack:
            case s_sms_per_error:
            case s_cnm_error:
               {
                   /*
                  * (TS 24.011, 6.3.1) 
                  *  If state == WAIT_FOR_RP_ACK
                  *      - Stop TR1M
                  *      - Send release request
                  *      - Transition to idle
                  *      - (TS 24.011, 6.3.3.1.2)-- permanent error 
                  *        if this is a SMMA transaction
                  *          - reset TRAM and RETRANS flag
                  */
                  MN_stop_sms_timer(connection_id, TR1M);   

                  if (sms_rl_stimulus == s_sms_ack)
                  {
                     rel_cause = 0;
                  }
                  else
                  {
                     rel_cause = PROTOCOL_ERROR_UNSPECIFIED;
                  }

                  MN_put_MNSMS_REL_REQ(connection_id, rel_cause);
                  return_data.rel_req = TRUE;

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = TRUE;

                  if (sm_data_ptr->smma_transaction == TRUE)
                  {
                     MN_stop_sms_timer(connection_id, TRAM);
                     sm_data_ptr->retrans_flag = FALSE;
                     MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to FALSE",connection_id);
                  }
                  break;
               }

            case sms_TR1M_timer_expiry:
               {
                  if (sm_data_ptr->smma_transaction == FALSE)
                  {
                     sm_data_ptr->retrans_flag = FALSE;
                     MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to FALSE",connection_id);

                     sm_data_ptr->rl_state = IDLE;

                     MN_put_MNSMS_ABORT_REQ(connection_id, 
                                    PROTOCOL_ERROR_UNSPECIFIED);

                     return_data.success = TRUE;
                     return_data.sm_rl_report = TRUE;
                     return_data.rel_req = FALSE;
                  } 
                  else if (sm_data_ptr->retrans_flag == TRUE )
                  {
                     /* SMMA Tx'n  N no more re-Tx'ns */
                     sm_data_ptr->retrans_flag = FALSE;
                     MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to FALSE",connection_id);

                     sm_data_ptr->rl_state = IDLE;
                     MN_put_MNSMS_REL_REQ(connection_id, 
                                    PROTOCOL_ERROR_UNSPECIFIED);
                     
                     return_data.success = TRUE;
                     return_data.sm_rl_report = TRUE;
                     return_data.rel_req = FALSE;                                          
                  }
                  else
                  {
                     /* SMMA Tx'n i */                  
                     MN_start_sms_timer(connection_id, TRAM);    /* SMS timer id */

                     sm_data_ptr->retrans_flag = TRUE;
                     MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to TRUE",connection_id);

                     sm_data_ptr->rl_state = WAIT_FOR_RETRANS_TIMER;
                     MN_put_MNSMS_REL_REQ(connection_id, 
                                    PROTOCOL_ERROR_UNSPECIFIED);

                     return_data.success = TRUE;
                     return_data.sm_rl_report = FALSE;
                     return_data.rel_req = FALSE;
                  }

                  break;
               }

            case s_sms_temp_error:
               {
                  /*
                  * (TS 24.011, 6.3.3.1.2) -- temporary error 
                  *  If state == WAIT_FOR_RP_ACK and SMMA transaction is active
                  *      - If RETRANS flag is is set
                  *          - send report to UASMS
                  *          - send a release request
                  *          - reset the RETRANS flag
                  *          - reset TR1M
                  *          - transition to idle
                  *      - else
                  *          - send a release request
                  *          - reset the RETRANS flag
                  *          - reset TR1M
                  *          - start TRAM
                  *          - transition to the Wait_for_retrans_timer state
                  */
                  if (sm_data_ptr->retrans_flag == TRUE )
                  {
                     MN_stop_sms_timer(connection_id, TR1M);    /* SMS timer id */

                     sm_data_ptr->retrans_flag = FALSE;
                     MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to FALSE",connection_id);

                     sm_data_ptr->rl_state = IDLE;

                     MN_put_MNSMS_REL_REQ(connection_id, 
                                    PROTOCOL_ERROR_UNSPECIFIED);

                     return_data.rel_req = TRUE;
                     return_data.success = TRUE;
                     return_data.sm_rl_report = TRUE;
                  }

                  else
                  {
                     MN_start_sms_timer(connection_id, TRAM);    /* SMS timer id */

                     MN_stop_sms_timer(connection_id, TR1M);

                     sm_data_ptr->retrans_flag = TRUE;
                     MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to TRUE",connection_id);

                     sm_data_ptr->rl_state = WAIT_FOR_RETRANS_TIMER;

                     MN_put_MNSMS_REL_REQ(connection_id, 
                                    PROTOCOL_ERROR_UNSPECIFIED);
                     return_data.rel_req = TRUE;

                     return_data.success = TRUE;
                     return_data.sm_rl_report = FALSE;
                  }

                  break;
               }

            case s_sms_to_idle: 
               {
                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            default:
               {
                  MN_stop_sms_timer(connection_id, TR1M);

                  return_data.success = FALSE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  sm_data_ptr->rl_state = IDLE;

                  MSG_ERROR_DS_2(MN_SUB,"=MNCNM= SMR-%d-W_FOR_RP_ACK recd unexpct evt %d",
                          connection_id,sms_rl_stimulus);
                  break;
               }
            }

            break;
         }

      case WAIT_FOR_RETRANS_TIMER:
         {
            switch (sms_rl_stimulus)
            {

            case s_mo_abort_req:
               {
                  MN_stop_sms_timer(connection_id, TRAM);

                  MN_put_MNSMS_ABORT_REQ(connection_id, 
                                 PROTOCOL_ERROR_UNSPECIFIED);    /* cp cause */

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            case s_mo_smma_abort_req:
               {
                  MN_stop_sms_timer(connection_id, TRAM);
                  sm_data_ptr->retrans_flag = FALSE;
                  MSG_MED_DS_1(MN_SUB,"=MNCNM= Retrans Flag for SMR-%d set to FALSE",connection_id);

                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            case sms_TRAM_timer_expiry:
               {
                  /* TR1M is started at the time of sending the CP-DATA(RP-DATA) to
                  to the network in the CNM */

                  
                  /* copy from memory available rpdu to mo rpdu */
                  mo_rpdu_store.connection_id = mo_memory_available_rpdu_store.connection_id;
                  mo_rpdu_store.rpdu_length = mo_memory_available_rpdu_store.rpdu_length;
                  memscpy(mo_rpdu_store.rpdu, sizeof(mo_rpdu_store.rpdu) ,mo_memory_available_rpdu_store.rpdu, (unsigned int)mo_rpdu_store.rpdu_length);  

#ifdef FEATURE_3GPP_SMS_OVER_IMS
                  if(NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_sms_transport_type)
                  {
                    MN_put_MNIMS_MO_RPDU();
                    /* Start TR1M Timer here only in case of WMS-IMS interface */
                    /* Start the MN timer just before sending the RP-DATA */
                    MN_start_sms_timer(connection_id,TR1M); /* SMS timer id */
                  }
                  else
                  {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN WMS-IMS has not registered the CallBack Func");
                    MN_put_MNSMS_EST_REQ();
                  }
#else
                  /* Send message to SMS */
                  MN_put_MNSMS_EST_REQ();
#endif

                  sm_data_ptr->rl_state = WAIT_FOR_RP_ACK;
                  sm_data_ptr->smma_transaction = TRUE; /* This statement is redundant, this should ideally be already TRUE */
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }

            case s_sms_to_idle: 
               {
                  sm_data_ptr->rl_state = IDLE;
                  return_data.success = TRUE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  break;
               }
            default:

               {
                  MN_stop_sms_timer(connection_id, TRAM);

                  return_data.success = FALSE;
                  return_data.sm_rl_report = FALSE;
                  return_data.rel_req = FALSE;
                  sm_data_ptr->rl_state = IDLE;

                  MSG_ERROR_DS_2(MN_SUB,"=MNCNM= SMR-%d-W_FOR_RETRANS recd unexpct evt %d",
                          connection_id,sms_rl_stimulus);
                  break;
               }
            }

            break;
         }

      case WAIT_FOR_CP_ACK:
         {
            switch (sms_rl_stimulus)
            {
         
            case s_cnm_error:
            case s_cnm_ack:
               {
                  return_data.success = TRUE;
                  return_data.sm_rl_report = TRUE;
                  sm_data_ptr->rl_state = IDLE;
                  break;
               }
            default:
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= CP_ACK for MT SMS received in wrong state");
                  break;
               }
            }
            break;
         }
   
      default:
         {
            MN_stop_sms_timer(connection_id, TR1M);

            MN_stop_sms_timer(connection_id, TR2M);

            MN_stop_sms_timer(connection_id, TRAM);

            return_data.success = FALSE;
            return_data.sm_rl_report = FALSE;
            return_data.rel_req = FALSE;
            sm_data_ptr->rl_state = IDLE;

            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= SMR-%d in unknown state %d",connection_id,rl_state);

            break;
         }
      }
   

#ifdef FEATURE_MN_UNIT_TEST
      MSG_HIGH_DS_3(MN_SUB,"=MNCNM= SMR-%d %s-->%s",
               connection_id,
               show_rl_state(rl_state),
               show_rl_state(sm_data_ptr->rl_state));
#else
      MSG_HIGH_DS_3(MN_SUB,"=MNCNM= SMR-%d transitioned from state %d to %d",
               connection_id, rl_state, sm_data_ptr->rl_state);
#endif
   }
   else
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Connection_id exceeds maximum permissible value %d",connection_id,rl_state);
   }

   return(return_data);
}


char *  show_rl_state( byte rl_state )
{
   char *s;

   switch ( rl_state )
   {
      case IDLE:
         s = "IDLE";
         break;
      case WAIT_FOR_RETRANS_TIMER:
         s = "WAIT_FOR_RETRANS_TIMER";
         break;
      case WAIT_FOR_RP_ACK:
         s = "WAIT_FOR_RP_ACK";
         break;
      case WAIT_TO_SEND_RP_ACK:
         s = "WAIT_TO_SEND_RP_ACK";
         break;
      case WAIT_FOR_CP_ACK:
         s = "WAIT_FOR_CP_ACK";
         break;         
      default:
         s = "UNKNOWN_STATE";
         break;

   }
    return s;
}


/*
 * Function name :  MN_get_new_sms_connection_id
 * -------------
 *
 * Description :  Initialises a new connection_id for SMS
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: None
 * ----------  
 *             
 *             
 *
 * Returns:    connection_id
 * --------
 *
 */
connection_id_T
MN_get_new_sms_connection_id(void)
{
   boolean         success;
   connection_id_T connection_id, counter = 1;

   success = FALSE;

   for (counter = 1; 
       (counter <= UNKNOWN_CONNECTION_ID) && (success == FALSE);
       counter++)
   {
      connection_id = (last_allocated_mo_sms_connection_id + counter) % UNKNOWN_CONNECTION_ID;

      if( (sms_transaction_information[connection_id].sms_state == IDLE) &&
          (sm_rl_transaction_information[connection_id].rl_state == IDLE) &&
          (connection_id != UNKNOWN_CONNECTION_ID))
      {

         /* ATCOP: AT +CMMS=1: skip CP_ACK deferred connection_id/TI  */
         if( (sms_link_control.mode != SMS_LINK_CONTROL_DISABLED) &&
             (sms_link_control.cp_ack_deferred) &&
             (sms_link_control.connection_id == connection_id))
         {
            /* Do Nothing. Continue with next iteration */
         }
         else
         { 
             success = TRUE;
             break;
         }
      } /* For */
   }

   if (success == TRUE)
   {
      last_allocated_mo_sms_connection_id = connection_id;
      return connection_id;
   }
   else
   {
      return MN_INVALID_CALL_SESSION;     /* error value */
   }

}




/*
 * Function name :  MN_get_rl_mo_mr
 * -------------
 *
 * Description :  Process decodes the cp_user_data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: None
 * ----------  
 *             
 *             
 *
 * Returns:    connection_id
 * --------
 *
 */
byte 
MN_get_rl_mo_mr(void)
{
   byte rl_message_ref;

   rl_message_ref = rl_mo_mr;
   rl_message_ref++;
   rl_mo_mr = rl_message_ref;

   return rl_message_ref;

}



/*
 * Function name :  MN_get_rp_report
 * -------------
 *
 * Description :  Send an RP-ERROR or RP-ACK message in response
 * -----------    to a MT short message
 *
 *
 *
 *
 *
 *
 * Parameters: SM_RL_REPORT_REQ
 * ----------  
 *             
 *             
 *
 * Returns:    
 * --------
 *
 */
VOID_FUNC
MN_get_rp_report(SM_RL_REPORT_REQ_T *SM_RL_REPORT_REQ)
{
   /* Locals  */
   rp_cause_T                  rp_cause;
   control_sm_rl_return_data_T return_data;


   return_data = MN_control_SMS_RL(SM_RL_REPORT_REQ->connection_id,
       s_report_req);

   if (return_data.success == FALSE)
   {
       ERR("ERROR: SMS_RL in wrong state for an RP_ACK message",0,0,0);

   }

   else
   {
      if (SM_RL_REPORT_REQ->sm_rl_success == TRUE)
      {
         /* The message was received OK */

         MN_put_rp_ack(return_data.rel_req,
             SM_RL_REPORT_REQ->connection_id,
             SM_RL_REPORT_REQ);
      }

      else
      {
         rp_cause.cause_value = SM_RL_REPORT_REQ->cause_value;

         if (SM_RL_REPORT_REQ->diagnostics_present == TRUE)
         {
            rp_cause.diagnostics = SM_RL_REPORT_REQ->diagnostics;
            rp_cause.Length = 2;
         }

         else
         {
            rp_cause.Length = 1;
         }

         MN_put_rp_error(rp_cause,
             SM_RL_REPORT_REQ->connection_id,
             return_data.rel_req, 
             SM_RL_REPORT_REQ);
      }
   }

}


/*
 * Function name :  MN_process_link_control_req()
 * -------------
 *
 * Description :  
 * -----------    
 *   Process the SMS link control request. 
 *
 *
 * Parameters: SM_RL_REPORT_REQ
 * ----------  
 *             
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC
MN_process_SM_RL_LINK_CONTRL_REQ(SM_RL_LINK_CONTROL_REQ_T *sms_link_control_req)
{

    switch (sms_link_control_req->mode)
    {
        case SMS_LINK_CONTROL_ENABLED_ONE:
        case SMS_LINK_CONTROL_ENABLED_ALL:
            {   
                if(sms_link_control_req->timeout_in_seconds != 0)
                {
                    sms_link_control.duration = MIN (sms_link_control_req->timeout_in_seconds, SM_LINK_CONTROL_MAX_DURATION); 

                    /* AT+CMMS=1: support for Multiple SMS, GCF 16.9.1,2 */
                    sms_link_control.mode = sms_link_control_req->mode; 
                }
                else
                {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= NAS discards link control, if link control duation is zero");
                }                  
            }
            break;

        case SMS_LINK_CONTROL_DISABLED:
            { 
                sms_link_control.mode = sms_link_control_req->mode;
            }
            break;

        default:
            {
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid UASMS_SM_LINK_CONTROL_REQ: mode = %d", sms_link_control_req->mode); 

                MN_put_uasms_link_control_report_ind(
                                    sms_link_control.mode, 
#if defined(FEATURE_DUAL_SIM )
                                    sms_link_control_req->as_id,
#endif
                                    sms_link_control.status);
            }
            break;
    }
}

#if defined(FEATURE_DUAL_SIM )
boolean MN_validate_sms_as_id(sys_modem_as_id_e_type as_id, word short_message_identifier, boolean send_rsp)
{
  boolean success = FALSE;
  SM_RL_REPORT_IND_T   SM_RL_REPORT_IND;
  if(as_id != SYS_MODEM_AS_ID_NONE && (mn_as_id == SYS_MODEM_AS_ID_NONE || as_id == mn_as_id))
  {
    success = TRUE;
  }
  else
  {
    success = FALSE;
    MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", as_id,mn_as_id); 
    /* Set up the SM_RL_REPORT_IND message & send to RL */
    if(send_rsp) 
    {
      SM_RL_REPORT_IND.short_message_identifier = short_message_identifier;
      SM_RL_REPORT_IND.error_status = SMS_RPT_RADIO_UNAVAILABLE;
      SM_RL_REPORT_IND.cause_value = 0;
      SM_RL_REPORT_IND.diagnostics_present = FALSE;
      SM_RL_REPORT_IND.diagnostics = 0;
      SM_RL_REPORT_IND.as_id = as_id;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      SM_RL_REPORT_IND.permanent_error = 0;
      SM_RL_REPORT_IND.current_domain = MMCC_SMS_CS_PS;
#endif
       SM_RL_REPORT_IND.ims_cause_value = (word)0;
      /*
       * This function is called during MO SMS. Here, message set doesnt correspond to 
       * domain. So, set the domain as some default one
       */
    
      MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);
    }
  }
  return success;
  
}
#endif
