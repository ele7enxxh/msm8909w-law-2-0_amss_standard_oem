/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_sms_msg_util.c_v   1.10   17 Jun 2002 17:19:14   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_sms_msg_util.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/7/01      CD     Fixed compile warnings C4701 - local variable may be used 
                   without being initialized. Set diagnostics_present = FALSE,
                   diagnostics = 0, tpdu_length = 0

5/8/01      CD     Fixed several compile warnings C4244 conversion from int to byte

5/10/01     CD     Removed unnecessary header file includes

5/25/01     CD     Corrected calls to memset.  Deleted null_ptr
                   since it was not being accessed

                   Removed cause.h since it was not being used

                   Initialized tpdu in MN_get_cp_user_data to avoid lint error

6/7/01      CD     Removed MN_put_tpdu_deliver(), MN_put_tpdu_status_report()

6/8/01      CD     Changed call to MN_put_mmi_submit_sm_conf
                   by MN_put_uasms_sm_report_ind

6/14/01     CD     Removed ms.h since it is no loger used

7/17/01     CD     Replaced cid_index by connection id in for loop, 
                   since it was not being used

7/18/01     CD     Removed duplicate external declaration of sm_rl_transaction_information

8/1/01      CD     Fixed check for other active message transfers in RP_DATA_N case

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

09/27/01    CD     In MN_get_cp_user_data, corrected use of SM_RL_REPORT_IND_T due to
                   changes in structure definition.  Provided enum for error_status, 
                   and fixed use of cause_value

09/28/01    CD     Replaced 0x41 for RP_USER_DATA_IEI
                   Added comments to mn_get_cp_user_data()
                   
                   In case RP_ACK_N, 
                      - corrected cause value for a message not compatible 
                        with state; a value of 62 was being used instead of 98

                      - corrected 2 instances where a duplicate MNSMS_REL_REQ was
                        being sent

                      - added diagnostics field to the RP_ERROR sent in response to 
                        a invalid short message ref value

                   In case RP_ERROR_N,
                      - added more values to the list of rp_cause values to include in the 
                        report to UASMS 

                      - Added check for SMMA transaction active before rp_cause values are 
                        identified as permanent or temporary
                      
                      - corrected problems where the wrong msg reference, or msg received in the 
                        wrong state would results in the MS responding with another RP_ERROR.  
                        Requirements state that the RP_ERROR_N shall be ignored

                       - corrected problem where an optional IE (RP user data) would results in 
                         the MS responding with another RP_ERROR and a release req.  Requirements 
                         state that a syntax error is ignored and the msg is reated as if it 
                         did not have the IE, and an unknown IEI is ignored

                       - corrected problem where a mandatory IE (RP cause) w/ syntax error would
                         result in the code sending an RP_ERROR with the wrong cause value, and
                         a release request. Requirements state that only an RP_ERROR is sent

                   In case RP_DATA_N,                         
                       - corrected problem where a RP_DATA_N syntactically incorrect would not 
                         cause the state machine to transition the entity's state to IDLE

                   Corrected problem where a CP_USER_DATA with a reserved msg type id would
                   results in a release request.  Msg should be ignored instead

10/8/01            Included cause.h
                   Replaced hardcoded cause values with #defines

10/12/01           Added logging in MN_put_rp_error

1/29/02    RG      Initialized the SM_RL_DATA_IND to CM_NULL_INFO(=0). Changed the Initialization of 
                   the rp_cause to CM_NULL_INFO(=0) macro. This was the magic number 0 before.

2/04/02            Replaced CM_NULL_INFO by 0x00, since CM_NULL_INFO is not longer defined

3/27/02    RG      Fix for CR #21318(SMS: after sending the SMMA request the UI notifies of 
                   message delivery) When processing an RP-ACK with no tpdu (in which case the 
                   rpdu_lengh is 2), the MN_get_cp_user_data() routine fills the 
                   SM_RL_REPORT_IND.short_message_identifier with the value that is stored 
                   in the transaction. This was not done before which caused the UASMS to 
                   think of as an SM_RL_REPORT_IND for a regular MT SMS.

11/1/02    RG      Fix for CRs 25310  and 24539

02/14/03   AB      Removed lint errors.

05/12/03   RG      (Fix for CR30353) RP message type is indicated by the last 3 bits of the octet; 
                   bits 4 to 8 are spare bits that are ignored.

03/23/07   NR      Setting present field to true for RP_OA field for all successful cases
                   
04/09/07   RD      SMS optimiztion changes are incorporated

09/09/11   PM      Fix for deepsec read overflow CR 261584.
10/05/11   gps     Initiated some primitives with all zeros

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "cause.h"
#include "ms.h"
#include "cnm.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#include <stringl/stringl.h>
#include "nasutils.h"
#include "wms_modem.h"


extern  VOID_FUNC * CNM_alloc( unsigned int size );
extern  VOID_FUNC CNM_send_message( IMH_T * p_message, gs_queue_id_T queue);
extern  VOID_FUNC CNM_free( VOID_DATA * p_mem );

/*
 * Function name :  MN_get_cp_user_data
 * -------------
 *
 * Description :  Process decodes the CP_user_data(RP_ACK_N, RP_ERROR_N, RP_DATA_N)
 * -----------    This function is called when processing a MNSMS_DATA_IND
 *                or a MNSMS_EST_IND.  These two primitives have the same structure
 *                (IMH_T header , byte connection id, rpdu_T rpdu)
 *
 *                This function checks for syntactic and semantic errors in the 
 *                CP_user_data, and calls the SMS state machine to execute the SM-RL
 *                procedures (TS 24.011 V3.5.0 section 9.3 and 6.3,respectively).
 *
 *
 * Parameters: connection_id
 * ----------  message_length   - from the IMH_T header 
 *             cp_user_data     - rpdu union
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_get_cp_user_data(connection_id_T connection_id, 
                    int message_length, 
                    rpdu_T cp_user_data,
					nas_msg_transport_type_e_type    sms_transport_type)
{
   sm_rl_transaction_information_T *sm_data_ptr;
   control_sm_rl_return_data_T      return_data;
   SM_RL_REPORT_IND_T               *SM_RL_REPORT_IND;
   SM_RL_DATA_IND_T                 *SM_RL_DATA_IND;
   int                              index = 0;
   byte                             rp_cause_index = 0;
   byte                             rpda_length = 0;
   boolean                          valid_message = TRUE;
   boolean                          diagnostics_present = FALSE;
   byte                             diagnostics = 0;
   byte                             rp_cause_value=0;
   byte                             tpdu_length= 0;
   int                              bcd_index,tpdu_index;
   rp_cause_T                       rp_cause;
   int                              rpdu_length = 0;
   boolean                          send_rel_req = FALSE;
   byte                             smr_event;

   
#ifdef FEATURE_MODEM_HEAP
            SM_RL_REPORT_IND = (SM_RL_REPORT_IND_T *) modem_mem_calloc(1, sizeof(SM_RL_REPORT_IND_T),MODEM_MEM_CLIENT_NAS);
            SM_RL_DATA_IND   = (SM_RL_DATA_IND_T *) modem_mem_calloc(1, sizeof(SM_RL_DATA_IND_T),MODEM_MEM_CLIENT_NAS);
#else
            SM_RL_REPORT_IND = (SM_RL_REPORT_IND_T *) gs_alloc(sizeof(SM_RL_REPORT_IND_T));
            SM_RL_DATA_IND =   (SM_RL_DATA_IND_T *) gs_alloc(sizeof(SM_RL_DATA_IND_T));

#endif
   if((SM_RL_REPORT_IND != NULL) && (SM_RL_DATA_IND != NULL))
   {
   SM_RL_REPORT_IND->short_message_identifier = 0;
   SM_RL_REPORT_IND->error_status = NO_ERROR_RP_ACK;
   SM_RL_REPORT_IND->cause_value = 0;
   SM_RL_REPORT_IND->diagnostics_present = diagnostics_present;
   SM_RL_REPORT_IND->diagnostics = diagnostics;
   SM_RL_REPORT_IND->tpdu_length = tpdu_length;
   

   /* initialize the rp_cause */
   memset(&rp_cause,0x00, sizeof(rp_cause_T));

   
   /* Initialize the state machine return data */
   return_data.rel_req = FALSE;
   return_data.sm_rl_report = FALSE;
   return_data.success = FALSE;

   /* Get the SMR transaction information for this connection id */
   sm_data_ptr = &(sm_rl_transaction_information[connection_id]);

   if (connection_id >= MAXNO_SMS_TRANSACTIONS)
   {
       valid_message = FALSE;
       MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMR red CPUserData for invalid connId %d", connection_id);
   }

   if (valid_message) 
   {
       /*
       * If message_length indicates that there is a RPDU, continue to decode the message 
       * else, message is ignored 
       */
       if(message_length > (sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type)))
       {

           rpdu_length = message_length - (int)sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type);

           /* 
            * (TS24.011, section 9.3.1)
            * The RPDU message must be at least 2 bytes (mesage_type and message reference)
            * If not, message is ignored
            */
            if (rpdu_length < 2)
            {
                valid_message = FALSE;
                MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMR-%d recd CP_USER_DATA too short", connection_id);
            }
       }
       else
       {
           valid_message = FALSE;
           MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMR-%d recd CP_USER_DATA w/o RPDU", connection_id);
       }
   }
   if (valid_message)
   {
      /* switch by message type */
      /* RP message type is indicated by the last 3 bits of the octet; bits 4 to 8 are spare bits 
         to be ignored */
      switch (cp_user_data.data[index]&0x07)   
      {

      case RP_ACK_N:    
         {

            index++; 
            /*
            * (TS 24.011, 9.3.2)
            * If the message reference is invalid, discard the msg and send RP_ERROR  
            * (with the received msg ref)
            */
            if (cp_user_data.data[index] != sm_data_ptr->rl_message_ref)
            {
                valid_message = FALSE;
                rp_cause.Length = 2;
                rp_cause.cause_value = INVALID_SHORT_MESSAGE_REF_VALUE;
                rp_cause.diagnostics = cp_user_data.data[index];
                send_rel_req = FALSE;
                MN_put_rp_error(rp_cause, connection_id, send_rel_req, NULL);
            }

            index++;

            /*
            * Process the IEs in this message
            */

            /*
            * The RP_user_data IE is optional in an RP_ACK. 
            * Check the length to see if it is included in the message.
            * If the RP_user_data IE is included,  the IEI must be present
            */

            while ((valid_message) && ((index + 1)< rpdu_length))
            {
                switch (cp_user_data.data[index])
                {
                    case RP_USER_DATA_IEI:
                    {
                        index++;
                        /* 
                        * Build the UASMS report 
                        */
                        SM_RL_REPORT_IND->short_message_identifier = 
                            sm_data_ptr->short_message_identifier;

                        /* get the length of the TPDU */
                        SM_RL_REPORT_IND->tpdu_length = cp_user_data.data[index];

                        if ((SM_RL_REPORT_IND->tpdu_length > 0) && 
                            (SM_RL_REPORT_IND->tpdu_length <= RP_USER_DATA_TPDU_LENGTH) /* prevents write overflow*/&&
                            (SM_RL_REPORT_IND->tpdu_length <= (sizeof(cp_user_data.data) - index -1) ) /* prevents read overflow*/ ) 
                        {
                            index ++;
                            /* copy the TPDU to the report */
                            memscpy(SM_RL_REPORT_IND->tpdu, 
                                   sizeof(SM_RL_REPORT_IND->tpdu),
                                   &cp_user_data.data[index], 
                                   SM_RL_REPORT_IND->tpdu_length);
                            index = index + SM_RL_REPORT_IND->tpdu_length;
                        }
                        else if (SM_RL_REPORT_IND->tpdu_length != 0)
                        {
                            /*
                            * (TS 24.008, 8.7.1)tpdu length syntactically incorrect. 
                            * Treat the message as if the IE was not present
                            */
                            MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMR-%d rec RP_ACK w/usrDataIE syntax err",connection_id);

                            /* set the index = rpdu_length to stop parsing */
                            index = rpdu_length;
                        }

                        break;
                    }

                    default:
                    {
                        /* (TS 24.008, 8.6.1)
                        *  RP_user_data IEI is unknown, ignore the IE.
                        *  Note:  because the IE is optional, there is no need to check
                        *  for syntax errors in the IEI
                        */
                        MSG_LOW_DS_2(MN_SUB,"=MNCNM= SMR-%d rec RP_ACK w/unknown usrDataIEI %d",
                                 connection_id,cp_user_data.data[index]);
                        /*
                        * Try to keep on parsing the message and position the 'index'
                        * correctly
                        */
                        if ((cp_user_data.data[index] & 0x80) == 0x80)
                        {
                            /* 
                            * The IEI is of type 1 or 2, and has bit 8 set to 1.
                            * The whole IE is one octet long
                            */
                            index = index +1;
                        }
                        else
                        {
                            /*
                            * The IEI is of type 4, and has bit bit 8 set to 0
                            * The following octet is the length octet
                            */
                            /* get the length */
                            index = index + 1;
                            index = index + cp_user_data.data[index] +1;
                        }

                        break;
                    }
                }
            }
            
            if(valid_message)
            {
               /*
               * Send an sms_ack stimulus to the state machine (TS 24.011, 6.3.1)
               */
               return_data = MN_control_SMS_RL(connection_id, s_sms_ack);

               if (((return_data.success == TRUE) && (return_data.sm_rl_report == TRUE)) && (connection_id < MAXNO_SMS_TRANSACTIONS))
               {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                   mmcc_domain_name_T domain;
#endif
                   /*
                   * Send the report to UASMS 
                   */
                   /* Fix for CR #21318 */
                   SM_RL_REPORT_IND->short_message_identifier =
                            sm_data_ptr->short_message_identifier;
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                     SM_RL_REPORT_IND->as_id = sm_data_ptr->as_id;
                   }
#ifdef FEATURE_DUAL_ACTIVE
                   else
                   {
                     SM_RL_REPORT_IND->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                   }
#endif
#endif
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                   domain = (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM)? MMCC_SMS_CS : MMCC_SMS_PS;
                   SM_RL_REPORT_IND->current_domain = domain;
                   SM_RL_REPORT_IND->permanent_error = 0;
#endif
                   SM_RL_REPORT_IND->ims_cause_value = (word)0;
                   MN_put_uasms_sm_report_ind(SM_RL_REPORT_IND);
               }
               
               /* (TS 24.011, 9.3.3) If the state machine detected an invalid state */ 
               if (return_data.success == FALSE)
               {
                  /* Set up cause */
                  rp_cause.Length = 1;
                  rp_cause.cause_value = MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;

                  /* send release request unless it has 
                  *  already been done by the state machine
                  */
                  send_rel_req = TRUE;
                  if (return_data.rel_req == TRUE)
                  {
                      send_rel_req = FALSE;
                  }
                  MN_put_rp_error(rp_cause, connection_id, send_rel_req, NULL);
               }
            }

            break;
         }

      case RP_ERROR_N:
         {

#ifdef FEATURE_MODEM_HEAP
            byte        *tpdu = (byte *) modem_mem_calloc(1, TPDU_SIZE,MODEM_MEM_CLIENT_NAS);
#else
            byte        *tpdu = (byte *) gs_alloc(TPDU_SIZE);
#endif
            if(tpdu != NULL)
            {
            /* Determine the validity of the RP_ERROR */
            index++;

            /* 
            * (TS 24.011, 9.3.2)            
            *  If the message reference is invalid, discard the message
            */
            if (cp_user_data.data[index] != sm_data_ptr->rl_message_ref)
            {
                valid_message = FALSE;
                MSG_LOW_DS_2(MN_SUB,"=MNCNM= SMR-%d rec RP_ERROR w/invalid msf ref %d",
                         connection_id, cp_user_data.data[index]);

                MN_put_MNSMS_REL_REQ(connection_id,0);     /* cause_value */
            }

            if (valid_message)
            {
                /* 
                * Decode the RP_Cause IE, whis is mandatory for an RP_ERROR.
                * The RP_Cause IEI is not present in an RP_ERROR, so go directly to the
                * length indicator 
                * The length indicator is 
                *      2 bytes -- if cause value and diagnostic field are included
                *      1 byte -- if cause value is included only
                */
               index++;
               if ((index + 1) > rpdu_length)
               {
                 valid_message = FALSE;
               }
               else
               {
                 rp_cause_index = cp_user_data.data[index++];     /* length indicator (2 or 1)*/
                 rp_cause_value = cp_user_data.data[index++];     /* cause value              */
                 rp_cause_index--;                                /* length indicator (1 or 0)*/

                 switch(rp_cause_index)
                 {
                   case 0:
                   {
                       /* msg does not contain diagnostics */
                       break;
                   }
                   case 1:
                   {
                       if(index > rpdu_length)
                       {
                         valid_message = FALSE;
                       }
                       else
                       {
                         diagnostics_present = TRUE;
                         diagnostics = cp_user_data.data[index++];
                       }
                       break;
                   }
                   default:
                   {
                       /* 
                       * (TS 24.011, 9.3.4) 
                       * Length indicator syntactically incorrect.
                       * Ignore the message and send a RP_ERROR
                       */
                       valid_message = FALSE;
                       break;
                   }
                 }
               }
   
               if(valid_message == FALSE)
               {
                 rp_cause.Length = 1;
                 rp_cause.cause_value = INVALID_MANDATORY_INFORMATION;
                 send_rel_req = FALSE;
                 MN_put_rp_error(rp_cause, connection_id, send_rel_req, NULL);
               }
            }
            /* 
            *   Process optional IEs
            */

            /* 
            *   The RP_User_data IE is optional on an RP_ERROR.
            *   Check the message length to see if the element is included or not.
            */
            while ( (valid_message) && ((index + 1) < rpdu_length) )
            {
               /* An RP_User data iei must be present */

                switch (cp_user_data.data[index])
                {
                    case RP_USER_DATA_IEI:
                    {
                        index++;
                        tpdu_length = cp_user_data.data[index++];

                        if (((rpdu_length - index) == tpdu_length) && (tpdu_length <= TPDU_SIZE))
                        {
                           /* TPDU must fit in message */

                           for (tpdu_index = 0; index < rpdu_length; index++, tpdu_index++)
                           {
                               tpdu[tpdu_index] = cp_user_data.data[index];
                           }
                        }
                        /*
                        * (TS 24.008, 8.7.1)
                        * RP_user_data length is syntactically incorrect.
                        * Treat the msg as if the IE was not present
                        */
                        else  
                        {
                            MSG_LOW_DS_2(MN_SUB,"=MNCNM= SMR-%d rec RP_ERR w/UsrDataIE syntax err tpdu_length = %d",
                                    connection_id,tpdu_length);
                            tpdu_length = 0;
                            /* set the index = rpdu_length to stop parsing */
                            index = rpdu_length;
                        }

                        break;
                    }

                    default:
                    {
                        /* (TS 24.008, 8.6.1)
                        *  RP_user_data IEI is unknown, ignore the IE.
                        *  Note:  because the IE is optional, there is no need to check
                        *  for syntax errors in the IEI
                        */
                        MSG_LOW_DS_2(MN_SUB,"=MNCNM= SMR-%d rec RP_ERR w/unknown usrDataIEI %d",
                                 connection_id,cp_user_data.data[index]);
                        /*
                        * Try to keep on parsing the message and position the 'index'
                        * correctly
                        */
                        if ((cp_user_data.data[index] & 0x80) == 0x80)
                        {
                            /* 
                            * The IEI is of type 1 or 2, and has bit 8 set to 1.
                            * The whole IE is one octet long
                            */
                            index = index +1;
                        }
                        else
                        {
                            /*
                            * The IEI is of type 4, and has bit bit 8 set to 0
                            * The following octet is the length octet
                            */
                            /* get the length */
                            index = index + 1;
                            index = index + cp_user_data.data[index] +1;
                        }

                        break;
                    }
                }
            }

            if (valid_message)
            {
               /*
               * Table 8.4/3GPP TS 24.011 (Part 1 and Part 3)
               * Note that all possible rp_cause values are valid.
               * If a SMMA transaction is active, flag some rp cause values 
               * as temporary.  Otherwise, rp_cause values are treated as
               * permanent errors 
               */
               smr_event = s_sms_per_error;
               if (sm_data_ptr->smma_transaction == TRUE)
               {
                  switch (rp_cause_value)
                  {
                     case UNKNOWN_SUBSCRIBER:
                     case REQUESTED_FACILITY_NOT_IMPLEMENTED:
                     case SEMANTICALLY_INCORRECT_MESSAGE:
                     case INVALID_MANDATORY_INFORMATION:
                     case MESSAGE_TYPE_NON_EXISTENT:
                     case MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE:
                     case IE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
                     case PROTOCOL_ERROR_UNSPECIFIED:
                     case INTERWORKING_UNSPECIFIED:
                         /* do nothing-- keep stimulus as permanent error */
                         break;

                     case NETWORK_OUT_OF_ORDER:
                     case TEMPORARY_FAILURE:
                     case CP_CONGESTION:
                     case RESOURCES_UNAVAILABLE_UNSPECIFIED:
                         smr_event = s_sms_temp_error;
                         break;
                     default:
                         /* set as cause number 41 (TEMPORARY_FAILURE) */
                         smr_event = s_sms_temp_error;
                         break;
                  }
               }

               /*
               * Send stimulus (permanent or temporary error) to the state machine
               */
               return_data = MN_control_SMS_RL(connection_id, smr_event);

               /* 
               *(TS 24.011, 9.3.3)If the state machine detected the wrong state, 
               * ignore the message
               */
               if (return_data.success == FALSE)
               {
                  MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMR-%d recd RP_ERROR_N in wrong state", connection_id);
                  MN_put_MNSMS_REL_REQ(connection_id,0);     /* cause_value */
               }

               else if ((return_data.sm_rl_report == TRUE) && (connection_id < MAXNO_SMS_TRANSACTIONS))
               {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  mmcc_domain_name_T domain;
#endif
                  SM_RL_REPORT_IND->short_message_identifier = 
                      sm_data_ptr->short_message_identifier;
#if defined(FEATURE_DUAL_SIM )
                  if(MNCNM_IS_SINGLE_STACK)
                  {
                    SM_RL_REPORT_IND->as_id = sm_data_ptr->as_id;
                  }
#ifdef FEATURE_DUAL_ACTIVE
                  else
                  {
                     SM_RL_REPORT_IND->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                  }
#endif
#endif
                  SM_RL_REPORT_IND->error_status = SMS_RP_ERROR;
                  SM_RL_REPORT_IND->cause_value = rp_cause_value;
                  SM_RL_REPORT_IND->diagnostics_present = 
                      diagnostics_present;
                  SM_RL_REPORT_IND->diagnostics = diagnostics;
                  SM_RL_REPORT_IND->tpdu_length = tpdu_length;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  SM_RL_REPORT_IND->permanent_error = FALSE;
                  domain = (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM)? MMCC_SMS_CS : MMCC_SMS_PS;
                  SM_RL_REPORT_IND->current_domain = domain;
#endif
                  /*Add a check to see if message_set is same as that stored in sms_trans_info array*/
                  memscpy(SM_RL_REPORT_IND->tpdu,sizeof(SM_RL_REPORT_IND->tpdu),tpdu,tpdu_length);

                  SM_RL_REPORT_IND->ims_cause_value = (word)0;

                  MN_put_uasms_sm_report_ind(SM_RL_REPORT_IND);
               }
            }
#ifndef FEATURE_MODEM_HEAP
            gs_free( tpdu );
#else
            modem_mem_free(tpdu, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Mem alloc failed for tpdu");
            }

            break;
         }

      case RP_DATA_N:
         {
            index++;

            /*
            * (TS 24.011, 9.3.2) Invalid message reference
            *  Send RP_ERROR w/ INVALID_SHORT_MESSAGE_REF_VALUE
            */
            /* Fix for CR25310 :The code was sending an RP-ERROR if the message references(of 
                   the stored and the received) mismatch for the first active transaction in the SMR 
                   transaction table without checking the connection-id on which the RP-message was 
                   received.This has to be modified as follows: Send RP-ERROR with cause 
                   INVALID_SHORT_MESSAGE_REF_VALUE only if the message reference in the SMR 
                   transaction for the connection-id for which the RP-message received does not 
                   match with the message reference received in the RP-message.
            */
            if(connection_id < MAXNO_SMS_TRANSACTIONS)
            {
                if( (sm_rl_transaction_information[connection_id].rl_state  != IDLE) &&
                    (sm_rl_transaction_information[connection_id].smma_transaction == FALSE) &&
                    (sm_rl_transaction_information[connection_id].rl_message_ref != cp_user_data.data[index]))
                {
                     valid_message = FALSE;
                      rp_cause.cause_value = INVALID_SHORT_MESSAGE_REF_VALUE;    
                }
            }

            /* Note: If the UE receives an MT SMS with a message reference
            that is already active for an MO SMS, the MT SMS is still allowed to go through, since
            the key to the RP layer transaction is the connection-id and not the message reference.
            */

            if (valid_message == TRUE)
            {
               /* 
                * Set up SM_RL_DATA_IND message 
                */

               /* Store the message reference */
               sm_data_ptr->rl_message_ref = cp_user_data.data[index++];

               SM_RL_DATA_IND->connection_id = connection_id;
			   SM_RL_DATA_IND->sms_transport_type=sms_transport_type;
#if defined(FEATURE_DUAL_SIM )
               if(MNCNM_IS_SINGLE_STACK)
               {
                 SM_RL_DATA_IND->as_id = sm_rl_transaction_information[connection_id].as_id;
               }
#ifdef FEATURE_DUAL_ACTIVE
               else
               {
                 SM_RL_DATA_IND->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
               }
#endif
#endif
               /* 
               * (TS 24.011, 8.2.5.1)
               * Decode the RP_originator_address IE which is mandatory in a RP_DATA_N.
               * The The RP_originator address IEI is not present in a RP_DATA_N, so
               * proceed to get the length.
               * The length must be in the range  2 -- 11
               */

               SM_RL_DATA_IND->RP_OA.length = cp_user_data.data[index++];

               if (( (rpdu_length - index) < SM_RL_DATA_IND->RP_OA.length)||
                   (SM_RL_DATA_IND->RP_OA.length > sizeof(SM_RL_DATA_IND->RP_OA.data)) ||
                   (SM_RL_DATA_IND->RP_OA.length < 2) || /* Fix for CR 24539 */
                   (SM_RL_DATA_IND->RP_OA.length > 11))
               {
                  /* 
                  * (TS 24.011, 9.3.4) 
                  * Return a RP_ERROR w/ INVALID MANDATORY INFORMATION
                  */
                  valid_message = FALSE;         /* length is syntactically incorrect */
                  rp_cause.cause_value = INVALID_MANDATORY_INFORMATION;
               }

               else
               {
                  SM_RL_DATA_IND->RP_OA.present = TRUE;

                  /* Initialise SM_RL_DATA_IND */
                  memset(SM_RL_DATA_IND->RP_OA.data,'\0', sizeof(SM_RL_DATA_IND->RP_OA.data));

                  /* Load the type of number and the numbering plan ID */
                  SM_RL_DATA_IND->RP_OA.data[0] = cp_user_data.data[index++];

                  /* Load the BCD number */
                  for (bcd_index = 1; bcd_index < SM_RL_DATA_IND->RP_OA.length;  index++)
                  {
                     SM_RL_DATA_IND->RP_OA.data[bcd_index++] =  cp_user_data.data[index];
                  }
               }
            }

            if (valid_message)
            {
                  /* 
                  * (TS 24.011, 8.2.5.2)
                  * Decode the RP Destination addressIE which is mandatory in a RP_DATA_N.  
                  * The RP_Destination_adderess IEI is not present, so move onto the length. 
                  * For RP_DATA_N, the value of the length must be set to 0.  
                  */
                  rpda_length = cp_user_data.data[index++];

                  if (( (rpdu_length - index) < (int)rpda_length)||
                      (rpda_length != 0))
                  {
                     /* 
                     * (TS 24.011, 9.3.4) 
                     * Return a RP_ERROR w/ INVALID MANDATORY INFORMATION
                     */
                     valid_message = FALSE;      /* RP_DA length syntactically incorrect */
                     rp_cause.cause_value = INVALID_MANDATORY_INFORMATION;
                  }
            }
            if (valid_message)
            {
                index += rpda_length;
                /*
                * Decode the rp_user_data , which is mandatory for RP_DATA_N
                * The RP_User_data IEI is not included, so move on to the length
                */
                if(index >= 254) /*size of rpdu data 255(-1 as index incremented twice)*/
                {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
                }
                else
                {
                  tpdu_length = cp_user_data.data[index++];

                  SM_RL_DATA_IND->tpdu_length = tpdu_length;

                  if (((rpdu_length - index) < SM_RL_DATA_IND->tpdu_length)||
                      ( tpdu_length > RP_USER_DATA_TPDU_LENGTH))
                  {
                      valid_message = FALSE;      /* TPDU length syntactically incorrect */
                      rp_cause.cause_value = INVALID_MANDATORY_INFORMATION;
                  }
                  else
                  {
                      /* The tpdu must fit in the message */

                      for (tpdu_index = 0; (tpdu_index < tpdu_length && index < sizeof(cp_user_data.data)); tpdu_index++) /* added extra check to prevent the read overflow */
                      {
                          SM_RL_DATA_IND->tpdu[tpdu_index] = cp_user_data.data[index++];
                      }
                  }
                }
            }

            if (valid_message)
            {
                /*
                * (TS 24.011, 6.3.1) Send a stimulus to the state machine.
                */
                return_data = MN_control_SMS_RL(connection_id,s_mt_sms_request);

                if (return_data.success == TRUE)
                {
                    /* Forward data to UASMS */
                    MN_put_uasms_sm_data_ind(SM_RL_DATA_IND);
                }
                /* (TS24.011, 6.3.1, 9.3.3) msg received in the wrong state */
                else
                {
                    rp_cause.Length = 1;
                    rp_cause.cause_value = MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
                    send_rel_req  = TRUE;
                    MN_put_rp_error(rp_cause, connection_id,send_rel_req, NULL);
                }
            }   

            /*
            *  (TS 24.011, 9.3.2) invalid message reference
            *       - send RP_ERROR w/ cause_value == 81
            *  (TS 24.011, 9.3.4) syntactically incorrect message
            *       - send_RP_ERROR w/ cause_value == 96
            *  (TS 24.011, 6.3.4) abnormal case 
            *       - send RP_ERROR
            *       - send release request
            *       - transition to Idle
            */
            else /*valid_message == FALSE */
            {
               rp_cause.Length = 1;
               send_rel_req = TRUE;
               MN_put_rp_error(rp_cause, connection_id, send_rel_req, NULL);
               /* 
               * Send stimulus to state machine to transition to idle.
               * No need to look for return code
               */
               (void)MN_control_SMS_RL(connection_id, s_sms_to_idle);
            }

            break;
         }

      default:
         {
            /* (TS 24.011, 9.3.3 Reserved message type.
             *  ignore the message and send a RP_ERROR*/
            rp_cause.Length = 1;
            rp_cause.cause_value = MESSAGE_TYPE_NON_EXISTENT;
            /* Release request to be sent by MN eventhough
               MN is in IDLE state.
               (See SDL in 24.011 R99 V3.5.0 page 86)
             */
            send_rel_req = TRUE;
            MN_put_rp_error(rp_cause, connection_id, send_rel_req, NULL);
         }
      } /* end switch  on message type */
   }  /* endif valid message length */
#ifndef FEATURE_MODEM_HEAP 
     gs_free( SM_RL_REPORT_IND );
     gs_free( SM_RL_DATA_IND );
#else
     modem_mem_free(SM_RL_REPORT_IND, MODEM_MEM_CLIENT_NAS);
     modem_mem_free(SM_RL_DATA_IND, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Mem alloc failed for SM_RL_REPORT_IND or SM_RL_DATA_IND");
   }

}



/*
 * Function name :  MN_put_rp_ack
 * -------------
 *
 * Description :    Sends an RP-ACK message in response to a successfully
 * -----------      delivered MT short message
 *
 *
 *
 *
 *
 *
 * Parameters: rel_req
 * ----------  connection_id
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_put_rp_ack(byte rel_req,
connection_id_T connection_id,
SM_RL_REPORT_REQ_T *SM_RL_REPORT_REQ)
{
   byte  *rpdu;
   byte  rpdu_length = 2;
   byte tpdu_length;

   if (SM_RL_REPORT_REQ == NULL) 
   {
    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Received Null SM_RL_REPORT_REQ ptr");
    return;
   }   
   
   tpdu_length = SM_RL_REPORT_REQ->tpdu_length;

   if (tpdu_length > 232)
   {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= tpdu length invalid %d", SM_RL_REPORT_REQ->tpdu_length);
    tpdu_length = 0;
   }   
   else if (tpdu_length > 0) 
   {
    tpdu_length += 2;
   }

#ifndef FEATURE_MODEM_HEAP
   rpdu = gs_alloc(rpdu_length + tpdu_length);
#else
   rpdu = modem_mem_calloc(1, (rpdu_length + tpdu_length), MODEM_MEM_CLIENT_NAS);
#endif

   if (rpdu != NULL)
   {   
       rpdu[0] = RP_ACK_MS;
       rpdu[1] = sm_rl_transaction_information[connection_id].rl_message_ref;

       tpdu_length = SM_RL_REPORT_REQ->tpdu_length;

       if (tpdu_length > 0)
       {
         byte i;
         byte *tpdu_ptr = SM_RL_REPORT_REQ->tpdu;          

         /* Added TP UD IEI 0x41 and length indicator */
         rpdu[2] = 0x41;
         rpdu[3] = tpdu_length;

         for (i = 0; i < tpdu_length; ++i)
         {
            rpdu[i+4] = tpdu_ptr[i];
         }
         rpdu_length += tpdu_length;
         rpdu_length += 2;
       }
       {
         SM_RL_RPDU_T  mn_rpdu;
         /* Check if CNM state for any MT transaction is non IDLE
          *  If yes, then MT SMS path is over MM connection and not IMS*/
         if(CNM_is_GW_MT_SMS_in_progress() == FALSE)
         {
           memscpy((void *)&mn_rpdu.rpdu[0],
                   sizeof(mn_rpdu.rpdu),
                  (void *)rpdu,
                  rpdu_length);
           mn_rpdu.rpdu_length = rpdu_length;

           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN Invoking the WMS-IMS regd CallBack Func ");
           mn_rpdu.RP_DA.present = FALSE;
           mn_rpdu.as_id = sm_rl_transaction_information[connection_id].as_id;

           wms_mn_mo_sms_callback((void *)&mn_rpdu);
         }
         else
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN sending ACK over CS ");
           MN_put_MNSMS_DATA_REQ(connection_id,
               rpdu,
               rpdu_length);
         }
       }


       if (rel_req == TRUE)
       {
         MN_put_MNSMS_REL_REQ(connection_id, 0);    /* cause_value */
       }   

#ifndef FEATURE_MODEM_HEAP
       gs_free(rpdu);
#else
       modem_mem_free(rpdu, MODEM_MEM_CLIENT_NAS);
#endif
   }   
   else
   {
      ERR("ERROR: Unable to allocate memory",0,0,0);

   }
}



/*
 * Function name :  MN_put_rp_error
 * -------------
 *
 * Description :    Sends an RP-ERROR message in response to a 
 * -----------      unsuccessfully delivered MT short message
 *
 *
 *
 *
 *
 *
 * Parameters: rp_cause
 * ----------  connection_id
 *             rel_req
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_put_rp_error(rp_cause_T rp_cause, connection_id_T connection_id,byte rel_req, SM_RL_REPORT_REQ_T *SM_RL_REPORT_REQ)
{
   int   index;
   byte *rpdu;
   byte  rpdu_length;
   byte  tpdu_length = 0;

   if (SM_RL_REPORT_REQ != NULL)
   {
     tpdu_length = SM_RL_REPORT_REQ->tpdu_length;     

     if (tpdu_length > 232)
     {
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= tpdu length invalid %d", SM_RL_REPORT_REQ->tpdu_length);
        tpdu_length = 0;
     }   
     else if (tpdu_length > 0) 
     {
       tpdu_length += 2;
     }
   }

   
   if (connection_id < MAXNO_SMS_TRANSACTIONS)
   {
#ifndef FEATURE_MODEM_HEAP
      if ((rpdu = (byte *)gs_alloc(3 + rp_cause.Length + tpdu_length)) != NULL)
#else
      if ((rpdu = (byte *)modem_mem_calloc(1, (3 + rp_cause.Length + tpdu_length), MODEM_MEM_CLIENT_NAS)) != NULL)
#endif
      {
         index = 0;
         rpdu[index++] = RP_ERROR_MS;
         rpdu[index++] = sm_rl_transaction_information[connection_id].
       rl_message_ref;
         rpdu[index++] = rp_cause.Length;

         if (rp_cause.Length == 1)
         {
            rpdu[index++] = rp_cause.cause_value;
         }
         else if(rp_cause.Length == 2)
         {
            rpdu[index++] = rp_cause.cause_value;
            rpdu[index++] = rp_cause.diagnostics;  /* single byte */
         }

         MSG_LOW_DS_3(MN_SUB,"=MNCNM= RP_ERROR msg_ref %d cause %d, diag %d",
               sm_rl_transaction_information[connection_id].rl_message_ref,
               rp_cause.cause_value,
               rp_cause.diagnostics);

         rpdu_length = (byte)(3 + rp_cause.Length);

         if (SM_RL_REPORT_REQ != NULL)
         {
            tpdu_length = SM_RL_REPORT_REQ->tpdu_length; 

            if (tpdu_length > 0)
            {
               byte i;
               byte *tpdu_ptr = SM_RL_REPORT_REQ->tpdu;               

               /* Added TP UD IEI 0x41 and length indicator */
               rpdu[index++] = 0x41;
               rpdu[index++] = tpdu_length;

               for (i = 0; i < tpdu_length; ++i)
               {
                  rpdu[i+index] = tpdu_ptr[i];
               }
               rpdu_length += tpdu_length;
               rpdu_length += 2;
            }
         }

         {
           SM_RL_RPDU_T       mn_rpdu;

         /* Check if CNM state for any MT transaction is non IDLE and SMS is not over IMS
          *  If yes, then MT SMS path is over MM connection and not IMS*/

           if((CNM_is_GW_MT_SMS_in_progress() == FALSE) &&
              ((mt_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS) || (mo_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS)))
           {
             memscpy((void *)&mn_rpdu.rpdu[0],
                    sizeof(mn_rpdu.rpdu),
                    (void *)rpdu,
                    rpdu_length);

             mn_rpdu.rpdu_length = rpdu_length;
             mn_rpdu.RP_DA.present = FALSE;
             mn_rpdu.as_id = sm_rl_transaction_information[connection_id].as_id;

             MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN Invoking the WMS-IMS regd CallBack Func ");

             wms_mn_mo_sms_callback((void*)&mn_rpdu);

             /* Reset the mt transport to NAS_MSG_TRANSPORT_TYPE_MAX here */
             if(mt_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS)
             {
               mt_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_MAX;
             }

           }
           else
           {
             MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN sending ACK over CS ");
             MN_put_MNSMS_DATA_REQ(connection_id,
              rpdu,
              rpdu_length);
           }
         }

         if (rel_req == TRUE)
         {
            MN_put_MNSMS_REL_REQ(connection_id,0);     /* cause_value */
         }
         
#ifndef FEATURE_MODEM_HEAP
         gs_free(rpdu);
#else
         modem_mem_free(rpdu, MODEM_MEM_CLIENT_NAS);
#endif
      }
      else
      {
         ERR("ERROR: Unable to allocate memory",0,0,0);
      }
   }
   else
   {
      ERR("ERROR: Connection_id exceeds maximum value permissible",0,0,0);

   }


}


/*
 * Function name :  UASMS_put_mn_ims_msg
 * -------------
 *
 * Description :     This function is invoked by UASMS and it stores the callback() function 
.*                        registered by UASMS (WMS).
 *
 *
 *
 *
 * Parameters:  ims_msg of type SM_RL_IMS_MSG_T
 *
 * Returns:    None
 * --------
 *
 *
 */
sms_ims_state_e_type UASMS_put_mn_ims_msg(SM_RL_IMS_MSG_T  ims_msg)
{
  sms_ims_state_e_type sms_status = SMS_OK;
  
  if(ims_msg.Ims_msg_type == SMS_IMS_ERROR_IND)
  { /* Report ERROR INDICATION to MN on all the ACTIVE MO/MT SMSs. In LTE system which is always connected, 
            only one connection should serves all the SMSs MO and MT  */
    /* For Every Active MO/MT SMS  Do the below block applies */
    byte connection_id;
    boolean GW_MT_SMS_in_progress_on_CNM = CNM_is_GW_MT_SMS_in_progress();
 
    for(connection_id = 0; connection_id < MAXNO_SMS_TRANSACTIONS; connection_id++)
    { 
      boolean send_error = FALSE;
      
      if(sm_rl_transaction_information[connection_id].rl_state != IDLE)
      {
        if (connection_id <= MAXNO_SMS_MO_TRANSACTIONS)
        {
          if (NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_sms_transport_type) send_error = TRUE;
        }
        else
        {
          if (FALSE == GW_MT_SMS_in_progress_on_CNM) send_error = TRUE;
        }
      }
      if(TRUE == send_error)
      {
        MNSMS_ERROR_IND_T       *p_mnsms_error_ind;
 
        p_mnsms_error_ind = (MNSMS_ERROR_IND_T*) CNM_alloc( sizeof(MNSMS_ERROR_IND_T) );
        p_mnsms_error_ind->message_header.message_set = MS_MN_SMS;
        p_mnsms_error_ind->message_header.message_id  = MNSMS_ERROR_IND;
        PUT_IMH_LEN( sizeof( MNSMS_ERROR_IND_T ) - sizeof(IMH_T),
                     &p_mnsms_error_ind->message_header);
      
        p_mnsms_error_ind->connection_id = connection_id;
        p_mnsms_error_ind->as_id = ims_msg.as_id;
           
        p_mnsms_error_ind->cp_cause.Iei  = 0;
        p_mnsms_error_ind->cp_cause.cause_value  = 0;
        p_mnsms_error_ind->lower_layer_cause = ims_msg.cause;
        p_mnsms_error_ind->lower_layer_cause_value = ims_msg.ims_cause_value;
        p_mnsms_error_ind->permanent_error = (ims_msg.cause == SMS_IMS_PERM_ERROR)? 1: 0;
        MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Retry Interval for SMS over IMS, present = %d, value = %d",p_mnsms_error_ind->retry_interval.present, p_mnsms_error_ind->retry_interval.value);
        p_mnsms_error_ind->retry_interval.present = (ims_msg.retry_interval.present == TRUE)? TRUE:FALSE;
        p_mnsms_error_ind->retry_interval.value = ims_msg.retry_interval.value;
   
        CNM_send_message( (IMH_T *)p_mnsms_error_ind, GS_QUEUE_MN_CM );
        CNM_free( p_mnsms_error_ind );
      }
    }
       
  }
  else if (ims_msg.Ims_msg_type == SMS_IMS_RPDU_IND)
  {
    switch ( ims_msg.rpdu[0]&0x07 )
    {
      case RP_DATA_N :
      {
        byte connection_id;
        /* Check if there is already any MT SMS going on. There should be other MT SMS Transaction 
                   ongoing for accepting a fresh MT SMS */
        for (connection_id = MAXNO_SMS_MO_TRANSACTIONS +1; connection_id < MAXNO_SMS_TRANSACTIONS; connection_id++)
        {
          if(sm_rl_transaction_information[connection_id].rl_state != IDLE)
          {
            sms_status = SMS_TRANSACTION_ONGOING;
            MSG_HIGH_DS_2(MN_SUB,"=MNCNM= MN Already a MT SMS Transaction ongoing, connection_id = %d and sms status = %d ",connection_id ,sms_status );
            break;
          }
        }

        if (connection_id == MAXNO_SMS_TRANSACTIONS)
        {
          byte conn_id;
          MNSMS_EST_IND_T    mnsms_est_ind;
   
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN New MT SMS Transaction ");
          for (conn_id = MAXNO_SMS_MO_TRANSACTIONS +1; conn_id < MAXNO_SMS_TRANSACTIONS; conn_id++)
          {
            if(sm_rl_transaction_information[conn_id].rl_state == IDLE)
            {
              break;
            }
          }
            
          /* Get any connection id of the SMR Entity for which is in IDLE state (MT SMS) */
             
          mnsms_est_ind.message_header.message_set = MS_MN_SMS;
          mnsms_est_ind.message_header.message_id = MNSMS_EST_IND;
          mnsms_est_ind.connection_id  = conn_id;
          mnsms_est_ind.sms_transport_type = NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;
   
          mnsms_est_ind.as_id = ims_msg.as_id;

          (void)memscpy( (void *) &mnsms_est_ind.rpdu.data[0],
                         sizeof(mnsms_est_ind.rpdu.data),
                         (void *) &ims_msg.rpdu[0],
                         ims_msg.rpdu_length);
         
      /*
       * fill in the length field in the header
       */
          PUT_IMH_LEN(((sizeof(MNSMS_EST_IND_T) -
                      sizeof(IMH_T)) -
                      sizeof(mnsms_est_ind.rpdu)) +
                      ims_msg.rpdu_length,
                      &mnsms_est_ind.message_header);
      /*
       * send message to MN
       */
          CNM_send_message((IMH_T *) &mnsms_est_ind, GS_QUEUE_MN_CM );
        }
         
      }
      break;
   
      case RP_ACK_N :
      case RP_ERROR_N :
      {
        byte connection_id;
        MNSMS_DATA_IND_T      mnsms_data_ind;
        /* Get the MO connection id of the SMR Entity for which there is a Transaction pending (MO SMS) */
   
        for (connection_id = 0; connection_id < MAXNO_SMS_MO_TRANSACTIONS; connection_id++)
        {
          if (NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_sms_transport_type &&
              sm_rl_transaction_information[connection_id].rl_state == WAIT_FOR_RP_ACK)
          {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN MO SMS SMR - %d",connection_id);
            break;
          }
        }
        if(connection_id != MAXNO_SMS_MO_TRANSACTIONS)
        {
          mnsms_data_ind.message_header.message_set = MS_MN_SMS;
          mnsms_data_ind.message_header.message_id  = MNSMS_DATA_IND;
  
          mnsms_data_ind.as_id = ims_msg.as_id;
         
          mnsms_data_ind.connection_id  = connection_id;
 
          sms_status = SMS_ACK_OK;
   
          (void)memscpy ((void *) &mnsms_data_ind.rpdu.data[0],
                          sizeof(mnsms_data_ind.rpdu.data),
                          (void *) &ims_msg.rpdu[0],
                          ims_msg.rpdu_length);
          /*
           * fill in the length field in the header
           */
          PUT_IMH_LEN(((sizeof(MNSMS_DATA_IND_T) -
                      sizeof(IMH_T)) -
                      sizeof(mnsms_data_ind.rpdu)) +
                      ims_msg.rpdu_length,
                      &mnsms_data_ind.message_header);
          /*
           * send message to MN
           */      
          CNM_send_message( (IMH_T *) &mnsms_data_ind, GS_QUEUE_MN_CM );
        }
        else
        {
          sms_status = SMS_TRANSACTION_INVALID;
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= MN NO SMR connection_id = %d , sms status= %d",connection_id,sms_status);
        }
         
      }
    }
       
  }
  else
  {
    sms_status = SMS_TRANSACTION_UNKNOWN;
    ERR("ERROR: MN received wrong message type received from WMS Glue/IMS layer",0, 0, 0);
  }

  return sms_status;
}


VOID_FUNC
MN_put_MNIMS_MO_RPDU(void)
{
  
  SM_RL_RPDU_T  mn_rpdu;
  
  /* Get a new rl_mr to use with this message */
  rl_mo_mr = MN_get_rl_mo_mr();

  /* Store it for comparing with return ACK's/ERROR's */
  sm_rl_transaction_information[mo_rpdu_store.connection_id].rl_message_ref = rl_mo_mr;

  mo_rpdu_store.rpdu[1] = rl_mo_mr; /* Always the 2nd octet */

  mn_rpdu.rpdu_length = mo_rpdu_store.rpdu_length;
  memscpy(&mn_rpdu.rpdu[0], sizeof(mn_rpdu.rpdu) ,&mo_rpdu_store.rpdu[0],mo_rpdu_store.rpdu_length);
  mn_rpdu.RP_DA = mo_sms_destination_addr;
  mn_rpdu.as_id = sm_rl_transaction_information[mo_rpdu_store.connection_id].as_id;
  
  /* Invoking the CallBack function to post a Message to WMS-GLUE  / IMS */
  wms_mn_mo_sms_callback((void*)&mn_rpdu);

}
