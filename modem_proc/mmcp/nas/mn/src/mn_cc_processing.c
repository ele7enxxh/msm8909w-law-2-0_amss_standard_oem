/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cc_processing.c_v   1.21   10 Jul 2002 13:29:42   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_cc_processing.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

3/26/2001   VT     Added mmi_reserve_connection_id()

3/26/2001   VT     In MN_handle_mt_call_answer(), introduced a call to
                    MN_put_mmi_call_connected_ind() as per MN-MMI Interface 
                    requirement. Removed call to MN_put_mmi_mt_data_connected_ind()
                    if information_transfer_capability != 0.

4/20/2001   VT     Changed the implementation of critical section in 
                    mmi_reserve_connection_id() to use gs_attach_lock(), 
                    gs_lock() & gs_unlock(). Previously, INTLOCK() & 
                    INTFREE() were used.

5/3/01      CD     Fixed compile warnings C4244 - conversion from int to byte

5/3/01      CD    Initialised data_recorded = FALSE to avoid compile 
                   warning 4701 in MN_read_mutilcall_status()

5/3/01      CD     Initialized call_data in MN_request_mo_call_setup to 
                   avoid compile warning C4701 - local variable may be
                   used without being initialized

                    Initialized new_connection_id=0xff in 
                    mmi_reserve_connection_id to avoid compile warning C4701

5/4/01      CD     Fixed compile warning C4701 - local variable 'mmi_fca_data'
                   may be used without being initialized

5/4/01      CD     Fixed compile warning C4244 conversion from int to byte in
                   MN_execute_holdmpty_success
                   Fixed compile warning C4701 local variable 'sucess' may be
                   used without being initialized in MN_execute_holdmpty_error

5/4/01      CD     Fixed compile warning C4701 local variable 'sucess' may be
                   used without being initialized in MN_execute_retrievempty_error

5/4/01      CD     Fixed compile warning C4701 local variable 'sucess' may be
                   used without being initialized in MN_execute_splitmpty_sucess

5/9/2001    VT     Removed a bug introduced due to setting mo_setup_pending to
                    TRUE in mmi_reserve_connection_id() after reserving the
                    connection_id. Added MSG_HIGH in MN_request_mo_call_setup(),
                    if it receives a connection_id in the message, which has
                    not been reserved by Call Manager by calling 
                    mmi_reserve_connection_id().

5/10/01     CD     Removed unnecessary header file includes

5/21/01     CD     Removed redundant declaration of cc_transaction_information 
                   (already declared in mn_cc.h) to avoid lint error

                   Changed declaration of cc_auxiliary_state from int to unsigned
                   int in MN_manage_CC_auxiliary_state to avoid lint error 702

                   Removed declaration of null_ptr in MN_request_mo_call_setup
                   as it was not being used (lint 550)

                   Removed include of ms.h as it is not used

                   Removed 'return_ptr' in MN_clear_call_data as it was not accessed
                   (lint 550)

7/18/01     CD     Moved external declaration of Multicall_Status to mn_cc.h
                   Moved external declaration of mn_call_information to mn_cm.h
                   Moved external declaration of ss_invoke_data tp mn_cm.h

8/3/01      CD     Added info to log messages
                   Removed unnecessary queue.h

8/14/01     AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

09/05/01    CD     Added calls to QXDM logging functions in MN_handle_mt_call_answer()

09/30/01    AB     Return the request connection id instead of UNKNOWN connection ID, in case
                      the mobile origination failed.

21-Nov-2001  VT    Bug Fix: In cm_reserve_connection_id(), Reset the mo_setup_pending flag 
                     in mn_call_information[new_connection_id], if it is not already reset.

22-Jan-2002  AB    Added MN_whichMultiCallIndex() to find the Multicall_Status_Index for 
                     given connection ID without having to re-initializing the 
                     state of the Multicall_Status_index[] table.

23-Jan-2002  AB    Added CCE_CALL_HOLD_REQ event to split one call and put the 
                     other on hold in AUX_CALL_IN_MPTY state in 
                     MN_manage_CC_auxiliary_state().

23-Jan-2002  AB    Fixed crash due to invalid memory access with connection_id (255) in 
                     MN_manage_CC_auxiliary_state().

24-Jan-2002  AB    Won't wait for HOLD_ACK to sent out EST_REQ for 2nd MO calls in 
                     MN_request_mo_call_setup().

01/25/02    CD     Updated copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag
                   
02/18/02    AB     Fixed MN_manage_CC_auxiliary_state() to switch to AUX_CALL_HELD when 
             one of the party in MPTY_CALL_HELD got released and remaining party
             is no longer in MPTY.
 
02/20/02    AB     Removed the AUX_BUILD_REQ_FRM_MPTY_CALL_HELD state as this value 
                   and processing is the same as AUX_BUILD_REQUEST_FROM_CALL_HELD. 

25-Feb-2002  VT    In MN_request_mo_call_setup(), if the incoming message is
                     CM_MO_EMERGENCY_CALL_REQ, call_data->CNM_service_type is
                     now being set to EMERGENCY_CALL_ESTABLISHMENT.

02/27/02    AB     Fixed the aux state handling for BUILD_MPTY_REJ in 
                     MN_manage_CC_auxiliary_state(). Also moved this
                     function to mn_ss_cc_processing.c, as it is managed 
                     exclusively by SS call related functions.
             
05/31/02    AB     Added cm_unreserve_connection_id() for CM to 
                     unreserves a connection ID prior to initiating a
                     MO call.

06/12/02    AB     Rewrite the cm_reserve_connection_id() to return 
                     UNKNOWN_CONNECTION_ID when all of the MO connection
                     ID slot had already been reserved.

                   Added additional checking for validate MO call request 
                     in MN_request_mo_call_setup() to make sure that given
                     connection ID not only is in CC_STATE_U0, but that it not in
                     call clearing state.  Furthermore, MO call is not 
                     allow if previous MO call is in a call pending, 
                     mo_ending_call, state.                  

06/26/02    AB    In MN_request_mo_call_setup(), if 2nd MO call is possible,
                     don't sent SETUP request until receive HOLD_ACK from the
                     network.

                  In MN_call_over(), added a break when when detect a active
                     connection in the same call session.

07/01/02    CD    Added checking of return value of MN_get_cm_mo_call_req()
                  Corrected unusual pointer cast lint errors

08/20/02    AB    In MN_request_mo_call_setup(), reject any new call if 
                  there were call waiting.

11/24/02    AB    Fixed Logic of rejecting a new call if there 
                  were call waiting. 

02/12/02    AB    Removed lint errors.

04/16/03    AB    In MN_request_mo_call_setup, update to new 
                    interface function, MN_put_cm_mo_call_conf(), that
                    support silient redial reject tye and value received
                    from MM.

07/07/04    AB    Changed the silient redial CC cause to TEMPORARY_FAILURE.

08/24/05    AB    Change the cc cause value to internal cause value,
                    CNM_REJ_NO_RESOURCES, and CNM_MM_REL_PENDING. 
                     
11/02/04    AB    Added SS Call related User-to-User Signaling feature.

03/22/04    AB    Removed the MT call screening to support CC dual services 
                  selection (non-multicall enviroment) feature.

05/18/05    HS    Added support for FEATURE_CCBS

08/18/05    HS    MN_num_of_calls_in_group() added

08/31/05    HS    Detailed init of mn_call_information

09/09/05    HS    Reverted change 'Detailed init of mn_call_information'

12/14/05    NR    Adding disc_req_sent in  MN_request_mo_call_setup() for handling back to back call 

12/27/05    NR    Correcting the error made during the merging

05/02/06    NR    Passing correct RAB Id for VT call waiting scenario

09/20/06    NR    Adding check for allowing another voice call while there is AV call with active BCIE being voice

05/25/11    AM    Using macro for 0xff

07/06/11    PM    Guarding against NULL pointer access

08/23/11   PM     Changes to preserve preferences for codecs given by UI.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include <string.h>
#include "environ.h"
#include "cause.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "naslog_v.h"
#include "ds_cap_v.h"
#include "sys_cnst_v.h"

#include "cmlib.h"
#include "cnm.h"
#include "geran_grr_api.h"

extern boolean is_cs_wcdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_tdscdma_mode( sys_modem_as_id_e_type as_id );

extern void MN_process_bearer_capability_ie(IMH_T *message_ptr);
extern void MN_align_speech_preferences_with_AMR_NV_Item(cm_bearer_capability_T *bearer_cap_p);
extern void MN_set_gsm_speech_preference_and_update_bitmap(gsm_speech_ver_e_type  *speech_ver_p,byte *bitmap_p);
extern void MN_read_gsm_speech_preferences_into_bitmap(gsm_speech_ver_e_type  speech_ver,byte *bitmap_p);
extern void MN_configure_codec_preferences(cm_bearer_capability_T *bearer_cap_p);
extern byte gsm_wcdma_amr_codec_preference;
extern byte preserve_ui_speech_codec_preference; /* declared and set in mmglbl.c, Read purpose of variable there */

#ifdef FEATURE_ENHANCED_AMR
extern void ghdi_reset_sr_and_inform_AS(sys_radio_access_tech_e_type active_rat);
#endif

//extern boolean mm_rel_req_sent;

/***************************************************************
*                                                              *
*                   MN_handle_mt_call_answer                   *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_handle_mt_call_answer
 * -------------
 *
 * Description : Handles answering of MT call by phone user 
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MNCC_SETUP_COMPL_IND
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_mt_call_answer(IMH_T *message_header)
{
   mn_call_information_T  *data_ptr;
   MNCC_SETUP_COMPL_IND_T *message_ptr;
   cm_bearer_capability_T bearer_capability;
   ie_cm_ss_error_T       cm_ss_error;
   boolean                log_packet_sent = FALSE;
   byte                   call_type = VOICE_CALL;
   connection_id_T        connection_id;

   message_ptr = (MNCC_SETUP_COMPL_IND_T *)((void *)message_header);

   connection_id = message_ptr->connection_id;
   
   if(mn_call_information_p[connection_id] != NULL)
   {

     data_ptr = mn_call_information_p[message_ptr->connection_id];

#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
    if(MNCNM_IS_SINGLE_STACK)
    {
       geran_grr_dtx_override_off_per_subs(mn_as_id);
    }
#ifdef FEATURE_DUAL_ACTIVE
    else
    {
       geran_grr_dtx_override_off_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
    }
#endif
#else
    geran_grr_dtx_override_off();
#endif
#endif
     /*  TURN ON APPROPRIATE TCH  */
     if(data_ptr->active_bearer_capability == NULL)
     {
        bearer_capability.information_transfer_capability = 
            data_ptr->bearer_capability_1.information_transfer_capability;
     }
     else
     {
        bearer_capability.information_transfer_capability = 
            data_ptr->bearer_capability_2.information_transfer_capability;
     }

     /* If BC transfer capability is NOT speech */
     if( 
#ifdef FEATURE_ALS
        (bearer_capability.information_transfer_capability != ITC_AUX_SPEECH) &&
#endif
        (bearer_capability.information_transfer_capability != ITC_SPEECH)
       )
     {
        MN_connect_TCH(bearer_capability.information_transfer_capability);

        call_type = DATA_CALL;

        /* Send log packet to DIAG */

        log_packet_sent = mn_log_cs_connection_bc_info(connection_id, bearer_capability);

        if (log_packet_sent == FALSE)
        {
            MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send log packet to DIAG");
        }
     }

     /*  NB : Speech has already been connected 
               at receipt of cm_mt_call_res_message  */
 
     /* Send log packet to DIAG */

     log_packet_sent = mn_log_cs_call_setup_info(connection_id, call_type, CN_TO_UE_DIRECTION);

     if (log_packet_sent == FALSE)
     {
         MSG_LOW_DS_0(MN_SUB,"=MNCNM= Cannot send log packet to DIAG");
     }

     /* Tell cm that the connection has been made */

     MN_put_cm_call_connected_ind(message_ptr->connection_id);

     /* Tell cm if the setup was the result of a SS operation */

     if (data_ptr->mt_call_accept_cause != CM_MT_CALL_RES)
     {
         memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));

         MN_put_cm_manage_calls_conf(TRUE,
#if defined(FEATURE_DUAL_SIM )
                                              mn_call_information_p[message_ptr->connection_id]->as_id,
#endif
                                      cm_ss_error);
         data_ptr->mt_call_accept_cause = CM_MT_CALL_RES;
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_handle_mt_call_answer:Received mn_call_information_p[%d]=NULL",
                    connection_id);
   }
}


/*
 * Function name :  MN_handle_mt_call_setup
 * -------------
 *
 * Description :  Process MT call setup. This function gets compatibility
 * -----------    data from the CM to check if MT call can be set up 
 *
 *
 *
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_mt_call_setup(IMH_T *message_header)
{
   /* Locals */

   mn_call_information_T   *call_data_ptr;
   mn_call_information_T   *previous_call_ptr;
   boolean     reject_call;
   connection_id_T      mt_connection_id;
   MNCC_SETUP_IND_T  *message_ptr;
   ie_cm_cc_cause_T cm_cc_cause;

   /*  PROCESS THE MT CALL SETUP  */
   /*  GET THE MESSAGE_DATA  */

   message_ptr = (MNCC_SETUP_IND_T *)((void *)message_header);

   reject_call = FALSE;

   mt_connection_id = MN_get_MNCC_SETUP_IND(message_ptr);

   if(mn_call_information_p[message_ptr->connection_id] != NULL)
   {
     /*  INDEX THE STORED DATA  */
     call_data_ptr = mn_call_information_p[message_ptr->connection_id];
   

     /*  LOOK AT THE STORED DATA  */
     reject_call = MN_read_multicall_status();

     /* reject call if there are already too many calls */
     if (reject_call == TRUE)
     {
        cm_cc_cause.present = TRUE;
        cm_cc_cause.coding_standard = STANDARD_GSM;
        cm_cc_cause.location = LOCATION_USER;
        cm_cc_cause.recommendation_present = FALSE;
        cm_cc_cause.cause_value = USER_BUSY;        /*  User Busy  */
        cm_cc_cause.diagnostic_length = 0;

        MN_put_MNCC_REJ_REQ(mt_connection_id, &cm_cc_cause);
     }
     else
     {
        if(reject_call == TRUE)
        {

          /* Build the cause data */
          cm_cc_cause.coding_standard = STANDARD_GSM;
          cm_cc_cause.location = LOCATION_USER;
          cm_cc_cause.recommendation_present = FALSE;
          cm_cc_cause.cause_value = USER_BUSY ;       /*  TS24.008, Sec 5.2.2.3.1, Response to SETUP indication */
          cm_cc_cause.diagnostic_length = 0;

          /* Send Reject Request to CC */
          MN_put_MNCC_REJ_REQ(mt_connection_id, &cm_cc_cause);
        }
        else
        {
          if((Multicall_Status[0].call_activity == CALL_ACTIVE)||
             (Multicall_Status[0].call_activity == CALL_HELD))
          {
            previous_call_ptr = mn_call_information_p[Multicall_Status[0].connection_id[0]];

            if(((call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) || 
                (call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)) &&
               ((previous_call_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) || 
                (previous_call_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH))) 
            {
                /* copy channel configuration for voice only service */
                call_data_ptr->TCH_channel_type = previous_call_ptr->TCH_channel_type;
                call_data_ptr->subchannel_number = previous_call_ptr->subchannel_number;
                call_data_ptr->radio_access_bearer_id = previous_call_ptr->radio_access_bearer_id;
            }
            else if(call_data_ptr->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH &&
                    call_data_ptr->bearer_capability_1.information_transfer_capability != ITC_SPEECH &&
                    previous_call_ptr->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH &&
                    previous_call_ptr->bearer_capability_1.information_transfer_capability != ITC_SPEECH)
            {
                     call_data_ptr->radio_access_bearer_id = previous_call_ptr->radio_access_bearer_id;
            }
          } /* if CALl_ACTIVE || CALL_HELD */

          /* Override the use of UL DTX between call setup and ack */
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
          if(MNCNM_IS_SINGLE_STACK)
          {
            geran_grr_dtx_override_on_per_subs(mn_as_id);
          }
#ifdef FEATURE_DUAL_ACTIVE
          else
          {
            geran_grr_dtx_override_on_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
          }
#endif
#else
          geran_grr_dtx_override_on();
#endif
#endif
          MN_put_cm_mt_setup_ind(mt_connection_id);

        } /* !reject_call  */

     } /* accept SETUP */
   } 
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot handle MNCC_MT_CALL_SETUP as mn_call_information_p[%d]is NULL", 
                                            message_ptr->connection_id);
   }

}




/*
 * Function name :  MN_reserve_call_object
 * -------------
 *
 * Description : 
 * -----------     
 *
 *  Allocates following call objects for a validated connection id.
 *  mn_call_information_p  &  cc_transaction_information_p
 *
 *  In this operation, MN task will allocate memory for MN and CNM global data
 *  for a connection id 
 *    1. provided to MN by CM in MO call request
 *    2. Provided to CNM by NW in MT call request
 *    3. 
 *  
 *
 *
 * Parameters: new_connection_id --> Validated connection id for which call objects has to be allocated.
 * ----------  
 *
 *
 * Returns: success (boolean) --> TRUE, if the function is able to allocate call objects
 *                                                for given connection id.
 *
 */
boolean MN_reserve_call_object(

  connection_id_T       new_connection_id

)
{

    boolean success = FALSE;

      if( (new_connection_id < MAXNO_CC_TRANSACTIONS) && 
           (cc_transaction_information_p[new_connection_id] == NULL) && 
           (mn_call_information_p[new_connection_id] == NULL) )
      {

#ifdef FEATURE_MODEM_HEAP
            cc_transaction_information_p[new_connection_id] = (cc_transaction_information_T*)modem_mem_calloc(1, sizeof(cc_transaction_information_T),MODEM_MEM_CLIENT_NAS);
            mn_call_information_p[new_connection_id] = (mn_call_information_T*)modem_mem_calloc(1, sizeof(mn_call_information_T),MODEM_MEM_CLIENT_NAS);

            if(cc_transaction_information_p[new_connection_id] != NULL)
            {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALLOC - cc_transaction_information_p for connection id = %d\n", new_connection_id );
            }
            else
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Memory ALLOC failure for cc_transaction_information_p for connection id = %d", new_connection_id);
            }

            if(mn_call_information_p[new_connection_id] != NULL)
            {
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALLOC - mn_call_information_p for connection id = %d\n", new_connection_id);
            }
            else
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Memory ALLOC failure for mn_call_information_p for connection id = %d", new_connection_id);
            }

#else
            cc_transaction_information_p[new_connection_id] = &cc_transaction_information[new_connection_id];
            mn_call_information_p[new_connection_id] = &mn_call_information[new_connection_id];
#endif

            if((cc_transaction_information_p[new_connection_id] != NULL) && (mn_call_information_p[new_connection_id] != NULL))
            {
              cc_transaction_information_p[new_connection_id]->cc_state = CC_STATE_U0; 
              cc_transaction_information_p[new_connection_id]->reserved = TRUE; 
              mn_call_information_p[new_connection_id]->mo_setup_pending = FALSE;

#if defined(FEATURE_DUAL_SIM )
              mn_call_information_p[new_connection_id]->as_id = SYS_MODEM_AS_ID_NONE;
#endif

              /* cnm init*/
              cc_transaction_information_p[new_connection_id]->CNM_service_type = NO_CNM_SERVICE;
              cc_transaction_information_p[new_connection_id]->hold_auxiliary_state = HOLD_IDLE;
              cc_transaction_information_p[new_connection_id]->mpty_auxiliary_state = MPTY_IDLE;
              cc_transaction_information_p[new_connection_id]->cc_cause[0] = 0;
              cc_transaction_information_p[new_connection_id]->T308_expired = FALSE;
              cc_progress_information[new_connection_id] = 0;
              
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
              cc_transaction_information_p[new_connection_id]->repeat = FALSE;
#endif /* FALLBACK */

              CNM_clear_dtmf_queue((connection_id_T)new_connection_id);

              success = TRUE;

          }
          else
          {
              MN_clear_call_data(new_connection_id);
          }
      }
      else
      {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error!!! Invalid connection id or Call Object for connection id = %d is in use",new_connection_id);
      }

 
   return success;

}  /* End of function MN_reserve_call_object() */





/*
 * Function name :  MN_unreserve_call_object
 * -------------
 *
 * Description :   
 * -----------     
 *  Unreserves call objects.
 *  
 * Parameters: 
 * ----------  
 *  connection_id : The connection ID of a call object. 
 *
 * Returns:  
 * --------  
 *  TRUE : operation successful.
 *  FALSE: invalid connection ID.
 * 
 */
 
static VOID_FUNC MN_unreserve_call_object(connection_id_T connection_id)
{

     /* Deallocate Call Data of given connection ID */
     if(connection_id < MAXNO_CC_TRANSACTIONS)
     {
       if(cc_transaction_information_p[connection_id] == NULL)
       {
#ifdef FEATURE_MODEM_HEAP
         if(mn_call_information_p[connection_id] != NULL)
         {
           modem_mem_free(mn_call_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - mn_call_information_p for connection id = %d\n", connection_id);
         }
#endif
         mn_call_information_p[connection_id] = NULL; 
       }
       else
       {
           if(cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0)
           {
              MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Freeing cc_transaction_information_p where cc_state is %d for connection_id = %d",cc_transaction_information_p[connection_id]->cc_state,connection_id);
           }
#ifdef FEATURE_MODEM_HEAP
           modem_mem_free(cc_transaction_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - cc_transaction_information_p for connection_id = %d\n", connection_id);
           if(mn_call_information_p[connection_id] != NULL)
           {
             modem_mem_free(mn_call_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - mn_call_information_p for connection id = %d\n", connection_id);
           }
#endif
           cc_transaction_information_p[connection_id] = NULL;  
           mn_call_information_p[connection_id] = NULL; 
       }
   }
   else
   {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection_id = %d\n", connection_id );
   }

}  /* End of function MN_unreserve_call_object() */




/*
 * Function name :  MN_request_mo_call_setup
 * -------------
 *
 * Description :  sets up an mo call
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: pointer to cm_mo_normal_call_req
 * ----------  or to cm_mo_emergency_call_req
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_request_mo_call_setup(IMH_T *message_ptr)
{
   connection_id_T           new_connection_id;
   int                       index, call_session = 0, other_session = 0;
   boolean                   call_possible = FALSE;
   boolean                   call_held = FALSE;
   mn_call_information_T     *call_data = NULL;
   cm_bearer_capability_T    null_bearer_capability;
   ie_cm_cc_cause_T          cm_cc_cause;
   boolean                   setup_req_syntax_err = FALSE;
   boolean                   disc_req_sent= FALSE;
   unsigned int              CC_auxiliary_state;
   boolean                   success = FALSE;
 

   /*  GET THE RECEIVED (& ALREADY RESERVED) TRANSACTION ID   */
   /*  The Transaction Id aka connection_id is in the same position
    *   irrespective of whether this is an emergency call or a 
    *   normal call  */

   new_connection_id 
       = ((cm_mo_emergency_call_req_T *)((void *)message_ptr))->connection_id;

   if(new_connection_id <= MAX_MO_CONNECTION_ID)
   {
      success = MN_reserve_call_object(new_connection_id); /* Reserve call objects for connection id given by CM  */
   }
   else
   {
      success = FALSE;
   }

#ifdef FEATURE_DUAL_SIM
   if((success == TRUE) && (MNCNM_IS_SINGLE_STACK))
   {
     if( (((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id != SYS_MODEM_AS_ID_NONE) && 
        ( (mn_as_id == SYS_MODEM_AS_ID_NONE) ||(((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id == mn_as_id)))
     {
         /* Set subscription id */ 
         mn_as_id = ((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id;
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting mn_as_id to %d",mn_as_id);
         mn_call_information_p[new_connection_id]->as_id = ((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id;
     }
     else
     {
       success = FALSE;
     }
   }
#endif
   
   if(success == TRUE) /* Check if call objects are reserved */
   {

#ifdef FEATURE_ENHANCED_AMR
     /* Reset eAMR sampling rate at call establishment if no other ACTIVE call exists */
      
     if((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
        (Multicall_Status[1].call_activity == CALL_INACTIVE))
     {
       sys_radio_access_tech_e_type active_rat = SYS_RAT_NONE;
       sys_modem_as_id_e_type           local_as_id = 0;
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_DUAL_ACTIVE
       local_as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;       
#else       
       local_as_id = mn_as_id;
#endif
#endif       
       if(is_cs_wcdma_mode(local_as_id))
       {
         active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
       }
       else if(is_cs_gsm_mode(local_as_id))
       {
         active_rat = SYS_RAT_GSM_RADIO_ACCESS;
       }
#ifndef FEATURE_DUAL_SIM       
       else if(is_cs_tdscdma_mode(0))
       {
         active_rat = SYS_RAT_TDS_RADIO_ACCESS;
       }
#endif       
       ghdi_reset_sr_and_inform_AS(active_rat);
     }
#endif

     /*  process the MESSAGE and set the AMR Speech Preferences (GSM) in Bearer Capability according to the NV Item 
          NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I ( gsm_wcdma_amr_codec_preference ) Setting */

     MN_process_bearer_capability_ie(message_ptr);
   
     if (new_connection_id != UNKNOWN_CONNECTION_ID)
     {   
       if ((cc_transaction_information_p[new_connection_id] != NULL) &&
            (mn_call_information_p[new_connection_id] != NULL))
       {
         /* Make sure it a new call */
         if (
             (cc_transaction_information_p[new_connection_id]->cc_state == CC_STATE_U0
#ifdef FEATURE_CCBS
              || cc_transaction_information_p[new_connection_id]->cc_state == CC_STATE_U0_6
              /* user responding to a recall */
#endif /* FEATURE_CCBS */
             ) &&
             (mn_call_information_p[new_connection_id]->mo_call_clear_pending == FALSE)) /* call cleared, not waiting to be clear */
         {
             if (cc_transaction_information_p[new_connection_id]->reserved == TRUE)
             {
                 call_possible = TRUE;

                 cc_transaction_information_p[new_connection_id]->reserved = FALSE; /* reset the flag */

                 mn_call_information_p[new_connection_id]->mo_setup_pending = TRUE;
             }
             else
             {
                 call_possible = FALSE;
                 MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Received a connection_id in MO Setup, which has not been reserved before\n" );

             }  /* end of else... */
         } /* end of if cc_transaction_information...cc_state... */
       }
       else
       {
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_call_information_p[%d] is NULL\n",new_connection_id);
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cc_transaction_information_p[%d] is NULL\n",new_connection_id);
       }
     }
     else
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Received UNKNOWN_CONNECTION_ID\n"); 
     }  /* end of if new_connection_id... */


   if(call_possible == TRUE)
   {
      /* See if there already too many calls */
      if (MN_read_multicall_status() == TRUE)
      {
         call_possible = FALSE;
      }
      else
      {
         /* Current Multicall Session */

         call_session = MN_whichMultiCallIndex(new_connection_id);

         /*  DEAL WITH THE OTHER CALL WHICH SHOULD BE HELD  */
     
         other_session = (byte)((call_session + 1) % 2);

         /*  See if the ACTIVE CALL In the Active Call Session (Multicall_Status)
             involves DATA OR FAX - Only Speech allows. Also check if previous call 
             in CALL ACTIVE state - Only Active can by put on HOLD. */

         if(Multicall_Status[other_session].call_activity > CALL_INACTIVE)
         {
            index = Multicall_Status[other_session].connection_id[0];  /* Any Previous Call */

            /* Speech Check */
            if(((mn_call_information_p[index]->active_bearer_capability== 0 ) 
#ifdef FEATURE_ALS
                  && ( mn_call_information_p[index]->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH)
#endif
                  && ( mn_call_information_p[index]->bearer_capability_1.information_transfer_capability != ITC_SPEECH)) ||
               ((mn_call_information_p[index]->active_bearer_capability == 1 )  
#ifdef FEATURE_ALS
                && ( mn_call_information_p[index]->bearer_capability_2.information_transfer_capability != ITC_AUX_SPEECH) 
#endif
                && ( mn_call_information_p[index]->bearer_capability_2.information_transfer_capability != ITC_SPEECH)) )
            {
               call_possible = FALSE;
               
               /* 1st Call is NON SPEECH 2nd Call is an E Call. Disc the 1st Call */
               if( message_ptr->message_id == CM_MO_EMERGENCY_CALL_REQ )
               {
                 /* If DISC has already been sent and this is the E-Call is because of Silent Redial */
                 if( mn_call_information_p[index]->mo_call_clear_pending != TRUE)
                 {
                    memset((VOID_DATA *)&cm_cc_cause,'\0',sizeof(ie_cm_cc_cause_T));

                    cm_cc_cause.location = STD_LOCATION;
                    cm_cc_cause.coding_standard = STD_CODESTANDARD;
                    cm_cc_cause.recommendation_present = FALSE;
                    cm_cc_cause.cause_value = NORMAL_CALL_CLEARING;
                    cm_cc_cause.diagnostic_length = 0;
               
                    MN_put_MNCC_DISC_REQ((connection_id_T)index, &(cm_cc_cause),NULL);
                  
                    mn_call_information_p[index]->mo_call_clear_pending = TRUE;
                    mn_call_information_p[index]->mo_setup_pending = FALSE;
                 }
                  /* The below Setting is required for triggering the SR, even if DISC was already sent for this call
                       and was omitted this time */
                 disc_req_sent=TRUE;                  
                  
               }               
               
            }

            /* Active Call Check */
            else if ((cc_transaction_information_p[index] != NULL) && (cc_transaction_information_p[index]->cc_state != CC_STATE_U10))  /* 1st Call Not in Active State */
            {
               call_possible = FALSE;
               /* 1st Call is being Setup AND 2nd Call is an E Call. Disc the 1st Call that is being Set up */
               if(( cc_transaction_information_p[index]->cc_state == CC_STATE_U0_1||
                   cc_transaction_information_p[index]->cc_state == CC_STATE_U1 ||
                   cc_transaction_information_p[index]->cc_state == CC_STATE_U3||
                   cc_transaction_information_p[index]->cc_state == CC_STATE_U4 ) &&
                   (message_ptr->message_id == CM_MO_EMERGENCY_CALL_REQ))
               {
                  memset((VOID_DATA *)&cm_cc_cause,'\0',sizeof(ie_cm_cc_cause_T));

                  cm_cc_cause.location = STD_LOCATION;
                  cm_cc_cause.coding_standard = STD_CODESTANDARD;
                  cm_cc_cause.recommendation_present = FALSE;
                  cm_cc_cause.cause_value = NORMAL_CALL_CLEARING;
                  cm_cc_cause.diagnostic_length = 0;

                  /* stop any SS releated timers if SS was active */
                  CC_auxiliary_state = (byte)(cc_transaction_information_p[index]->mpty_auxiliary_state +
                      (4* cc_transaction_information_p[index]->hold_auxiliary_state));
               
                  if(CC_auxiliary_state==AUX_HOLD_REQUEST)
                  {
                     MN_stop_cc_timer((connection_id_T)index);
                  }                    
               
                  MN_put_MNCC_DISC_REQ((connection_id_T)index, &(cm_cc_cause),NULL);
                  
                  mn_call_information_p[index]->mo_call_clear_pending = TRUE;
                  mn_call_information_p[index]->mo_setup_pending = FALSE;                  

               }
               
               if(mn_call_information_p[index]->mo_call_clear_pending==TRUE)
               {
                  disc_req_sent=TRUE;
               } 
            }  
            else if ((cc_transaction_information_p[index] != NULL) && (cc_transaction_information_p[index]->cc_state == CC_STATE_U10) && /* 1st Call in Active State */
                     (cc_transaction_information_p[new_connection_id]->cc_state != CC_STATE_U0 /* 2nd Call in IDLE State */
#ifdef FEATURE_CCBS
                      && cc_transaction_information_p[new_connection_id]->cc_state != CC_STATE_U0_6  /* 2nd Call in recall */
#endif /* FEATURE_CCBS */
                       ))
            {
               call_possible = FALSE;  /* Some Call Related SS transaction pending */
            }    
            else if (Multicall_Status[other_session].call_activity == MO_CALL_PENDING) 
            {
               call_possible = FALSE;
            }
            /*  IF THE ONE ACTIVE CALL IS MPTY & BEING SPLIT - REFUSE REQUEST */
            else if (Multicall_Status[other_session].split_mpty == TRUE)
            {
               call_possible = FALSE;
            }
            else
            {
              /* If call is active, attempt to put it on hold 
               */
               if(Multicall_Status[other_session].call_activity != CALL_HELD) 
               {
                   call_possible = MN_put_call_on_hold(Multicall_Status[other_session].connection_id[0]);

                   if(call_possible == TRUE)
                   {
                       call_held = TRUE;
                   }
                   else
                   {
                       call_held = FALSE;
                   }  
               }
               else
               {
                   call_held = FALSE;
               } /* ( != CALL HELD */

            } /* call is possible */ 

         } /* (Multicall_Status[other_session].call_activity > CALL_INACTIVE) */
         else if (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity > CALL_INACTIVE)
         {
            /* In coming call has high priority */
            call_possible = FALSE;
         }
         else
         {
            call_possible = TRUE;
         }
 
      } /* (MN_read_multicall_status() == TRUE) */

   } /* (call_possible == TRUE) */

   if(call_possible == TRUE)
   {
      setup_req_syntax_err = MN_get_cm_mo_call_req(new_connection_id, message_ptr);

      if (setup_req_syntax_err != FALSE)
      {
          call_possible = FALSE;
      }
   }

   /*  READ THE INCOMING MESSAGES AND STORE IN CC_TRANSACTION_INFORMATION  */
   if (call_possible == TRUE)
   {

      call_data = mn_call_information_p[new_connection_id];

      /*  SET UP A DEFAULT BEARER CAPABILITY FOR CALLS WITHOUT A BC GIVEN BY CM  */

      if(call_data->repeat_indicator_bc == 0)
      {
         call_data->repeat_indicator_bc = 1;
         call_data->bearer_capability_1.radio_channel_requirement = DEFAULT_SPEECH_RC_REQUIREMENT;
         call_data->bearer_capability_1.information_transfer_capability = SPEECH_ITC;
         call_data->CNM_service_type = MO_CALL_ESTABLISHMENT_DEFAULT;
         call_data->repeat_indication_hlc = 0;
         call_data->repeat_indication_llc = 0;
         call_data->calling_party_subaddress_present = FALSE;
      }

      if (message_ptr->message_id == CM_MO_EMERGENCY_CALL_REQ)
      {
          call_data->CNM_service_type = EMERGENCY_CALL_ESTABLISHMENT;
          
          /*  SET ALL OTHER DATA ELEMENTS TO NULL  */
          
          call_data->repeat_indication_hlc = 0;
          call_data->repeat_indication_llc = 0;
          call_data->called_party_bcd_number_present = FALSE;
          call_data->called_party_subaddress_present = FALSE;
          call_data->calling_party_bcd_number_present = FALSE;
          call_data->calling_party_subaddress_present = FALSE;
          call_data->signal_present = FALSE;
          call_data->clir_suppression = FALSE;
          call_data->clir_invocation = FALSE;
      }
   }

   /*  SEND MESSAGES  */
   if(call_possible == TRUE)
   {
      if(mm_rel_req_sent == TRUE)
      {
          /* Lower Layer/MM is connection release pending state = 19 */
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Call = %d rejected 'til MM REL_CNF/REL_IND", new_connection_id);

          /* Resetting of mn_as_id is required because call is rejected here*/
#if defined(FEATURE_DUAL_SIM )
          if(MNCNM_IS_SINGLE_STACK)
          {
            if( (CNM_check_if_any_service_object_exists() == FALSE) &&
                (mn_as_id != SYS_MODEM_AS_ID_NONE) )
            {
              MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
              mn_as_id = SYS_MODEM_AS_ID_NONE;
            }
          }
#endif

          /* Reset the setup pending flag */
          mn_call_information_p[new_connection_id]->mo_setup_pending = FALSE;

          /* create a cause */
          memset(&cm_cc_cause,'\0',sizeof(ie_cm_cc_cause_T));
          memset(&null_bearer_capability,0,sizeof(null_bearer_capability));

          cm_cc_cause.present = TRUE;
          cm_cc_cause.cause_value = CNM_MM_REL_PENDING;
          cm_cc_cause.coding_standard = STD_CODESTANDARD;
          cm_cc_cause.location = STD_LOCATION;

          MN_put_cm_mo_call_conf(new_connection_id,
                                 FALSE,  /* CNM service success */
                                 null_bearer_capability,
                                 null_bearer_capability,
                                 cm_cc_cause, /* cc_cause */
#if defined(FEATURE_DUAL_SIM )
                                 mn_call_information_p[new_connection_id]->as_id,
#endif
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                                 NULL,
                                 NULL,
                                 NULL
#else
                                 NULL
#endif
                                 );  /* no est. reject cause */    

          MN_clear_call_data(new_connection_id);

/*
#ifdef FEATURE_DUAL_SIM
            if((CNM_check_if_any_service_object_exists() == FALSE ))
            {
              MSG_HIGH_MN_DS(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n", 0, 0, 0);
              mn_as_id = SYS_MODEM_AS_ID_NONE;
            }
#endif
*/
      }
      else if(call_held == FALSE) /* Call already in HOLD REQ */
      {
          /* Override the use of UL DTX between call setup and ack */
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
          if(MNCNM_IS_SINGLE_STACK)
          {
            geran_grr_dtx_override_on_per_subs(mn_as_id);
          }
#ifdef FEATURE_DUAL_ACTIVE
          else
          {
             geran_grr_dtx_override_on_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
          }
#endif
#else
          geran_grr_dtx_override_on();
#endif
#endif
          MN_put_MNCC_SETUP_REQ(new_connection_id);
      }
      else
      {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Call = %d is deferred 'til HOLD_ACK %d", new_connection_id);
      }
   }
   else
   {
      
      /* Reset the setup pending flag */
      if(mn_call_information_p[new_connection_id] != NULL)
      {
         mn_call_information_p[new_connection_id]->mo_setup_pending = FALSE;
      }
      else
      {
         /* do nothing*/
      }

      /* create a cause */
      memset(&cm_cc_cause,'\0',sizeof(ie_cm_cc_cause_T));
      memset(&null_bearer_capability,0,sizeof(null_bearer_capability));
      if(disc_req_sent)
         cm_cc_cause.cause_value = CNM_MM_REL_PENDING  ;      
      else
         cm_cc_cause.cause_value = CNM_REJ_NO_RESOURCES;

      cm_cc_cause.present = TRUE;
      cm_cc_cause.coding_standard = STD_CODESTANDARD;
      cm_cc_cause.location = STD_LOCATION;

      MN_put_cm_mo_call_conf(new_connection_id,
          FALSE,  /* CNM service success */
          null_bearer_capability,
          null_bearer_capability,
          cm_cc_cause, /* cc_cause */
#if defined(FEATURE_DUAL_SIM )
            ((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id,
#endif
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
          NULL,
          NULL,
          NULL
#else
          NULL
#endif
          );  /* no est. reject cause */

      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process MOcallReq for conn id %d",new_connection_id);
      MN_clear_call_data(new_connection_id);
#if defined(FEATURE_DUAL_SIM )
        if(MNCNM_IS_SINGLE_STACK)
        {
          if( (CNM_check_if_any_service_object_exists() == FALSE) &&
              (mn_as_id != SYS_MODEM_AS_ID_NONE) )
          {
            MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
            mn_as_id = SYS_MODEM_AS_ID_NONE;
          }
        }
#endif
     }
   }
  else if(new_connection_id <= MAX_MO_CONNECTION_ID)
  {
    ie_cm_cc_reject_T cm_cc_reject;

    /* Reset the setup pending flag */
  
    if(mn_call_information_p[new_connection_id] != NULL)
    {
      mn_call_information_p[new_connection_id]->mo_setup_pending = FALSE;
    }
    else
    {
      /* do nothing*/
    }

    if(success == FALSE)
    {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Could not allocate call object for connection_id = %d \n", new_connection_id); 
    }
    else
    {
#if defined(FEATURE_DUAL_SIM )
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", ((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id,mn_as_id); 
#endif    
    }

    /* create a cause */
    memset(&cm_cc_reject,'\0',sizeof(ie_cm_cc_reject_T));
    memset(&cm_cc_cause,'\0',sizeof(ie_cm_cc_cause_T));
    memset(&null_bearer_capability,0,sizeof(null_bearer_capability));

    cm_cc_reject.present = TRUE;
    cm_cc_reject.rej_type = AS_REJECT_CAUSE;
    cm_cc_reject.rej_value = AS_REJ_ABORT_RADIO_UNAVAILABLE;

    cm_cc_cause.present = FALSE;
     
    MN_put_cm_mo_call_conf(new_connection_id,
                           FALSE,  /* CNM service success */
                           null_bearer_capability,
                           null_bearer_capability,
                           cm_cc_cause, /* cc_cause */
#if defined(FEATURE_DUAL_SIM )
                           ((cm_mo_emergency_call_req_T *)((void *)message_ptr))->as_id,
#endif
#ifndef  FEATURE_MULTIMEDIA_FALLBACK
                           &cm_cc_reject
#else
                           #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                           );  /* no est. reject cause */    
    MN_clear_call_data(new_connection_id);  
  }
else
  {
    MSG_ERROR_DS_1(MN_SUB, "Received UNKNOWN_CONNECTION_ID %d\n", new_connection_id);
  }  /* end of if new_connection_id... */
}

/***************************************************************
*                                                              *
*                     MN_process_bearer_capability_ie                              *
*                                                              *
****************************************************************/
/*
 * Function name :  
 * -------------
 *
 * Description :  Processes the BC IE of the Messages from CM.
 *
 *Messages from CM which include BC IE are 
 *  1. Emergency Call Req
 *  2. MO Normal Call Req
 *  3. Modify Req
 *  4. Call Confirm  (cm_mt_setup_res_T)
 *  5. cm_mt_modify_ind_T
 *  6. cm_modify_complete_conf_T
 *  7. cm_modify_complete_data_T
 *
 *
 * Parameters:  Message_Ptr
 * ----------  
 *
 *
 *
 * Returns:    Void
 * --------
 *
 */
void MN_process_bearer_capability_ie(IMH_T *message_ptr)
{
  if( message_ptr->message_id == CM_MO_EMERGENCY_CALL_REQ )
  {
    cm_mo_emergency_call_req_T   *emer_call_p;
   
    emer_call_p = (cm_mo_emergency_call_req_T *)((void *)message_ptr);
    if (emer_call_p->bearer_capability.present == TRUE)
    {
      /* Emergency Calls are only Speech Calls */
      if(preserve_ui_speech_codec_preference == 0)
      {
        MN_align_speech_preferences_with_AMR_NV_Item(&emer_call_p->bearer_capability);
      }
      else
      {
        MN_configure_codec_preferences(&emer_call_p->bearer_capability);
      }
    }
  }
  else if (message_ptr->message_id == CM_MO_NORMAL_CALL_REQ)
  {
    cm_mo_normal_call_req_T   *cm_normal_mo_call_p;
  
    cm_normal_mo_call_p = (cm_mo_normal_call_req_T *)((void *)message_ptr);
    if ((cm_normal_mo_call_p->bearer_capability_1.present == TRUE) && 
        ((cm_normal_mo_call_p->bearer_capability_1.information_transfer_capability == ITC_SPEECH) ||
         (cm_normal_mo_call_p->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH )))
    {
      if(preserve_ui_speech_codec_preference == 0)
      {
        MN_align_speech_preferences_with_AMR_NV_Item(&cm_normal_mo_call_p->bearer_capability_1);
      }
      else
      {
        MN_configure_codec_preferences(&cm_normal_mo_call_p->bearer_capability_1);
      }
    }
     
    if ((cm_normal_mo_call_p->bearer_capability_2.present == TRUE) && 
        ((cm_normal_mo_call_p->bearer_capability_2.information_transfer_capability == ITC_SPEECH) ||
         (cm_normal_mo_call_p->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH )))
    {
      if(preserve_ui_speech_codec_preference == 0)
      {
        MN_align_speech_preferences_with_AMR_NV_Item(&cm_normal_mo_call_p->bearer_capability_2);
      }
      else
      {
        MN_configure_codec_preferences(&cm_normal_mo_call_p->bearer_capability_2);
      }
    }
  }
  else if (message_ptr->message_id == CM_MO_MODIFY_REQ)
  {
    cm_mo_modify_req_T   *cm_mo_mod_req_p;
  
    cm_mo_mod_req_p = (cm_mo_modify_req_T *)((void *)message_ptr);
    if ((cm_mo_mod_req_p->bearer_capability.present == TRUE) && 
        ((cm_mo_mod_req_p->bearer_capability.information_transfer_capability == ITC_SPEECH) ||
         (cm_mo_mod_req_p->bearer_capability.information_transfer_capability == ITC_AUX_SPEECH )))
    {
      if(preserve_ui_speech_codec_preference == 0)
      {
        MN_align_speech_preferences_with_AMR_NV_Item(&cm_mo_mod_req_p->bearer_capability);
      }
      else
      {
        MN_configure_codec_preferences(&cm_mo_mod_req_p->bearer_capability);
      }
    }
  }
  else if (message_ptr->message_id == CM_MT_SETUP_RES)
  {
    cm_mt_setup_res_T   *cm_mt_setup_response_p;
  
    cm_mt_setup_response_p = (cm_mt_setup_res_T *)((void *)message_ptr);
    if ((cm_mt_setup_response_p->bearer_capability_1.present == TRUE) && 
        ((cm_mt_setup_response_p->bearer_capability_1.information_transfer_capability == ITC_SPEECH) ||
         (cm_mt_setup_response_p->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH )))
    {
      if(preserve_ui_speech_codec_preference == 0)
      {
        MN_align_speech_preferences_with_AMR_NV_Item(&cm_mt_setup_response_p->bearer_capability_1);
      }
      else
      {
        MN_configure_codec_preferences(&cm_mt_setup_response_p->bearer_capability_1);
      }
    }
     
    if ((cm_mt_setup_response_p->bearer_capability_2.present == TRUE) && 
        ((cm_mt_setup_response_p->bearer_capability_2.information_transfer_capability == ITC_SPEECH) ||
         (cm_mt_setup_response_p->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH )))
    {
      if(preserve_ui_speech_codec_preference == 0)
      {
        MN_align_speech_preferences_with_AMR_NV_Item(&cm_mt_setup_response_p->bearer_capability_2);
      }
      else
      {
        MN_configure_codec_preferences(&cm_mt_setup_response_p->bearer_capability_2);
      }
    }
  }
  
}




/***************************************************************
*                                                              *
*                     MN_align_speech_preferences_with_AMR_NV_Item                              *
*                                                              *
****************************************************************/
/*
 * Function name :  
 * -------------
 *
 * Description :  Aligns the Speech Preferences in the Bearer Capability IE of the Message from CM with the NV Item
 *                     NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I (gsm_wcdma_amr_codec_preference) Settings.
 *
 *Messages from CM which include BC IE are 
 *  1. Emergency Call Req
 *  2. MO Normal Call Req
 *  3. Modify Req
 *  4. Call Confirm  (cm_mt_setup_res_T)
 *  5. cm_mt_modify_ind_T
 *  6. cm_modify_complete_conf_T
 *  7. cm_modify_complete_data_T
 *
 *
 * Parameters:  Message_Ptr
 * ----------  
 *
 *
 *
 * Returns:    belongs  byte - (Multicall_Status_index)
 * --------
 *
 */
void MN_align_speech_preferences_with_AMR_NV_Item(cm_bearer_capability_T *bearer_cap_p)
{
  byte    gsm_codec_bitmap =0;
  
  /*-----------------------BITMAP----------------------------- */
  /* ----------------------------------------------------------*/
  /* |    08    |   07   |   06      |   05   |  04   | 03    |   02   |   01    | */ 
  /* ----------------------------------------------------------*/
  /* | XXXX  | XXXX  | FR AMR |   HR   |  FR   | GSM  | GSM | GSM | */  
  /* | XXXX  | XXXX  |   WB     | AMR  | AMR  | EFR   |  HR   |  FR   | */   
  /* -----------------------------------------------------------*/

/* Read the Speech Versions Set into the Bitmap */
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SPEECH PREF Initial gsm_codec_bitmap = %d",gsm_codec_bitmap);
#if defined(FEATURE_GSM_AMR_WB) 
  MN_read_gsm_speech_preferences_into_bitmap(bearer_cap_p->gsm_speech_ver_pref_0,&gsm_codec_bitmap);
#endif
  MN_read_gsm_speech_preferences_into_bitmap(bearer_cap_p->gsm_speech_ver_pref_1,&gsm_codec_bitmap);
  MN_read_gsm_speech_preferences_into_bitmap(bearer_cap_p->gsm_speech_ver_pref_2,&gsm_codec_bitmap);
  MN_read_gsm_speech_preferences_into_bitmap(bearer_cap_p->gsm_speech_ver_pref_3,&gsm_codec_bitmap);
#if defined(FEATURE_GSM_EXT_SPEECH_PREF_LIST) 
  MN_read_gsm_speech_preferences_into_bitmap(bearer_cap_p->gsm_speech_ver_pref_4,&gsm_codec_bitmap);
  MN_read_gsm_speech_preferences_into_bitmap(bearer_cap_p->gsm_speech_ver_pref_5,&gsm_codec_bitmap);
#endif
  
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SPEECH PREF After Read'n frm CM sent BCIE gsm_codec_bitmap = %d",gsm_codec_bitmap);
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= NV Settings gsm_wcdma_amr_codec_preference = %d",gsm_wcdma_amr_codec_preference);
/* Update the Same Bitmap with the values set for the GSM AMR Codecs in the NV Item */
  if ( gsm_wcdma_amr_codec_preference & 0x01)
  { /* GSM AMR NB (FR AMR) */
    gsm_codec_bitmap |= 0x08;
  }
  else
  {
    gsm_codec_bitmap &= ~(0x08);
  }
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= gsm_codec_bitmap 111 = %d",gsm_codec_bitmap);

  if ( gsm_wcdma_amr_codec_preference & 0x02)
  { /* GSM AMR WB */
    gsm_codec_bitmap |= 0x20;
  }
  else
  {
    gsm_codec_bitmap &= ~(0x20);
  }

  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= gsm_codec_bitmap 222 = %d",gsm_codec_bitmap);

  if ( gsm_wcdma_amr_codec_preference & 0x04)
  { /* GSM HR AMR */
    gsm_codec_bitmap |= 0x10;
  }
  else
  {
    gsm_codec_bitmap &= ~(0x10);
  }
  
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SPEECH PREF After applying NV settings gsm_codec_bitmap = %d",gsm_codec_bitmap);  
/* Now Write back the Speech Codec Versions in the BCIE based on this Bitmap Settings */

#if defined(FEATURE_GSM_AMR_WB) 
  MN_set_gsm_speech_preference_and_update_bitmap(&(bearer_cap_p->gsm_speech_ver_pref_0),&gsm_codec_bitmap);
#endif
  MN_set_gsm_speech_preference_and_update_bitmap(&(bearer_cap_p->gsm_speech_ver_pref_1),&gsm_codec_bitmap);
  MN_set_gsm_speech_preference_and_update_bitmap(&(bearer_cap_p->gsm_speech_ver_pref_2),&gsm_codec_bitmap);
  MN_set_gsm_speech_preference_and_update_bitmap(&(bearer_cap_p->gsm_speech_ver_pref_3),&gsm_codec_bitmap);
#if defined(FEATURE_GSM_EXT_SPEECH_PREF_LIST) 
  MN_set_gsm_speech_preference_and_update_bitmap(&(bearer_cap_p->gsm_speech_ver_pref_4),&gsm_codec_bitmap);
  MN_set_gsm_speech_preference_and_update_bitmap(&(bearer_cap_p->gsm_speech_ver_pref_5),&gsm_codec_bitmap);
#endif
  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SPEECH PREF Final one gsm_codec_bitmap = %d",gsm_codec_bitmap);  
  
}


/***************************************************************
*                                                              *
*                     MN_configure_codec_preferences                              *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_configure_codec_preferences
 * -------------
 *
 * Description :  This function turns off a few GSM codecs depending on 
 * the feature flags and bits in global variable gsm_wcdma_amr_codec_preference.
 *
 *
 * Parameters:  
 *            bearer_cap_p: Bearer capability structure which will be modified.
 * ----------  
 *
 *
 * Returns:    none
 * --------
 *
 */

void MN_configure_codec_preferences(cm_bearer_capability_T *bearer_cap_p)
{
  byte index = 0, write_index = 0; /* write_index will be set to either 0 or 1 in the later code depending upon FEATURE_GSM_AMR_WB */
  gsm_speech_ver_e_type copy_speech_codec[MAX_NUMBER_GSM_SPEECH_CODECS]; /* size_bearer_cap = array size, 6  */

  /* Copy codecs in local array */
  /* Initialize the entry if some variable is not available */

#if defined(FEATURE_GSM_AMR_WB)
  copy_speech_codec[0] = bearer_cap_p->gsm_speech_ver_pref_0;
  write_index = 0;
#else
  copy_speech_codec[0] = GSM_INVALID_SPEECH_VER;
  write_index = 1;
#endif /* FEATURE_GSM_AMR_WB */
  copy_speech_codec[1] = bearer_cap_p->gsm_speech_ver_pref_1;
  copy_speech_codec[2] = bearer_cap_p->gsm_speech_ver_pref_2;
  copy_speech_codec[3] = bearer_cap_p->gsm_speech_ver_pref_3;
#if defined(FEATURE_GSM_EXT_SPEECH_PREF_LIST)   
  copy_speech_codec[4] = bearer_cap_p->gsm_speech_ver_pref_4;
  copy_speech_codec[5] = bearer_cap_p->gsm_speech_ver_pref_5;
#else
  copy_speech_codec[4] = GSM_INVALID_SPEECH_VER;
  copy_speech_codec[5] = GSM_INVALID_SPEECH_VER;
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */

  
#ifndef FEATURE_GSM_AMR_WB
/* If not defined, switch off codec with value 8 in input */
  for(index = 0; index < MAX_NUMBER_GSM_SPEECH_CODECS; index++)
  {
    if(copy_speech_codec[index] == GSM_FR_SPEECH_VER_5) /* codec with value 8 should not be published*/
    {
      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= FEATURE_GSM_AMR_WB is OFF, Turning off codec with value = %d at index = %d",copy_speech_codec[index],index);  
      copy_speech_codec[index] = GSM_INVALID_SPEECH_VER;
      break;
    }
  }
#endif /* FEATURE_GSM_AMR_WB */


/*------------------------ BitMap in NV ---------------------------*/

/* NV Variable which is User Configured through UI, about his AMR preference */
/* -------------AMR CODEC  USER PREFERENCES-- ------------------*/                    
/*                                  -----  BITMAP  ------                                         */          
/* --------------------------------------------------------------*/                    
/* |  08   |  07  |   06  |  05  |       04     |      03      |     02      |     01     |  */   
/* --------------------------------------------------------------*/          
/* |  XX..|  XX..|  XX..|  XX  | WCDMA   |     GSM    |    GSM    |   GSM    |  */          
/* |  XX..|  XX..|  XX..|  XX  | AMR  WB |  HR AMR    |   AMR WB  |  AMR NB  |  */
/* |  XX  | XX   |  XX  |   XX |  NA     |     5      |     8     |    4     |  */
/* ---------------------------------------------------------------*/

  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= gsm_wcdma_amr_codec_preference is %d",gsm_wcdma_amr_codec_preference);  

  if((gsm_wcdma_amr_codec_preference & 0x01) == 0) 
  {
      for(index = 0; index < MAX_NUMBER_GSM_SPEECH_CODECS; index++)
      {
        if(copy_speech_codec[index] == GSM_FR_SPEECH_VER_3)
        {
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Turning off codec with value = %d at index = %d",copy_speech_codec[index],index);  
          copy_speech_codec[index] = GSM_INVALID_SPEECH_VER;
          break;
        }
      }

  }

  if((gsm_wcdma_amr_codec_preference & 0x02) == 0)
  {
      for(index = 0; index < MAX_NUMBER_GSM_SPEECH_CODECS; index++)
      {
        if(copy_speech_codec[index] == GSM_FR_SPEECH_VER_5) /* codec with value 8 should not be published*/
        {
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Turning off codec with value = %d at index = %d",copy_speech_codec[index],index);  
          copy_speech_codec[index] = GSM_INVALID_SPEECH_VER;
          break;
        }
      }
  }

  if((gsm_wcdma_amr_codec_preference & 0x04) == 0)
  {
      for(index = 0; index < MAX_NUMBER_GSM_SPEECH_CODECS; index++)
      {
        if(copy_speech_codec[index] == GSM_HR_SPEECH_VER_3) /* codec with value 5 should not be published*/
        {
          MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Turning off codec with value = %d at index = %d",copy_speech_codec[index],index);  
          copy_speech_codec[index] = GSM_INVALID_SPEECH_VER;
          break;
        }
      }
  }

 /***** Write back the values in the bearer_cap input structure ******/

 /* First Copy GSM_INVALID_SPEECH_VER in each of input codec variables */

#if defined(FEATURE_GSM_AMR_WB)
  bearer_cap_p->gsm_speech_ver_pref_0 = GSM_INVALID_SPEECH_VER;
#endif
  bearer_cap_p->gsm_speech_ver_pref_1 = GSM_INVALID_SPEECH_VER;
  bearer_cap_p->gsm_speech_ver_pref_2 = GSM_INVALID_SPEECH_VER;
  bearer_cap_p->gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;
#if defined(FEATURE_GSM_EXT_SPEECH_PREF_LIST)   
  bearer_cap_p->gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;
  bearer_cap_p->gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
#endif

 /* Now Overwrite valid values till they get finished, rest of the values will remain invalid*/
 
 for(index = 0; index < MAX_NUMBER_GSM_SPEECH_CODECS; index++)
 {
   if(copy_speech_codec[index] != GSM_INVALID_SPEECH_VER)
   {
     switch(write_index)
     {
#if defined(FEATURE_GSM_AMR_WB) /* This featurization is redundant, provided for clarity */  
       case 0:
        {
                     bearer_cap_p->gsm_speech_ver_pref_0 = copy_speech_codec[index];
                     ++write_index;
                     break;
        }
#endif /* FEATURE_GSM_AMR_WB */
       case 1:
        {
                     bearer_cap_p->gsm_speech_ver_pref_1 = copy_speech_codec[index];
                     ++write_index;
                     break;
        }
       case 2:
        {
                     bearer_cap_p->gsm_speech_ver_pref_2 = copy_speech_codec[index];
                     ++write_index;
                     break;
        }
       case 3:
        {
                     bearer_cap_p->gsm_speech_ver_pref_3 = copy_speech_codec[index];
                     ++write_index;
                     break;
        }
#if defined(FEATURE_GSM_EXT_SPEECH_PREF_LIST)   
       case 4:
        {
                     bearer_cap_p->gsm_speech_ver_pref_4 = copy_speech_codec[index];
                     ++write_index;
                     break;
        }
       case 5:
        {
                     bearer_cap_p->gsm_speech_ver_pref_5 = copy_speech_codec[index];
                     ++write_index;
                     break;
        }
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
    default:
        {
                     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invaid Index = %d\n", index);
                     ++write_index;
                     break;
        }

     }
   }
 }

 }/* MN_configure_codec_preferences */


/***************************************************************
*                                                                                                        *
*          MN_set_gsm_speech_preference_and_update_bitmap                    *
*                                                                                                        *
****************************************************************/
/*
 * Function name :  
 * -------------
 *
 * Description :   Sets the speech preference with the highest priority Codec set in the bitmap 
 * -----------   and resets the bit in the Bitmap.
 *
 *
 * Parameters: speech version preference , gsm codec bitmap
 * ----------  
 *
 *
 * Returns:    void
 * --------
 *
 */

/*-----------------------BITMAP----------------------------- */
/* ----------------------------------------------------------*/
/* |   08    |   07    |    06     |   05   |  04   | 03    | 02       |  01  | */ 
/* ----------------------------------------------------------*/
/* | XXXX  | XXXX  | FR AMR |   HR   |  FR   | GSM  | GSM | GSM | */  
/* | XXXX  | XXXX  |   WB    |   AMR | AMR  | EFR   |  HR  |  FR  | */   
/* -----------------------------------------------------------*/


void MN_set_gsm_speech_preference_and_update_bitmap(gsm_speech_ver_e_type  *speech_ver_p,byte *bitmap_p)
{

  if (((*bitmap_p)& 0x20))
  { /* FR AMR WB */
    (*speech_ver_p) = GSM_FR_SPEECH_VER_5;
    (*bitmap_p) &= ~(0x20);
  }
  else if (((*bitmap_p)& 0x08))
  { /* FR AMR  */
    (*speech_ver_p) = GSM_FR_SPEECH_VER_3;
    (*bitmap_p) &= ~(0x08);
  }
  else if (((*bitmap_p)& 0x04))
  { /* EFR */
    (*speech_ver_p) = GSM_FR_SPEECH_VER_2;
    (*bitmap_p) &= ~(0x04);
  }
  else if (((*bitmap_p)& 0x01))
  { /* FR */
    (*speech_ver_p) = GSM_FR_SPEECH_VER_1;
    (*bitmap_p) &= ~(0x01);
  }
  else if (((*bitmap_p)& 0x10))
  { /* HR  AMR*/
    (*speech_ver_p) = GSM_HR_SPEECH_VER_3;
    (*bitmap_p) &= ~(0x10);
  }
  else if (((*bitmap_p)& 0x02))
  { /* HR */
    (*speech_ver_p) = GSM_HR_SPEECH_VER_1;
    (*bitmap_p) &= ~(0x02);
  }
  else if ((*bitmap_p)==0x0)
  {
    (*speech_ver_p) = GSM_INVALID_SPEECH_VER;
  }
  else
  {
    (*speech_ver_p) = GSM_INVALID_SPEECH_VER;
  }
  return;
  
}

/***************************************************************
*                                                              *
*                     MN_read_gsm_speech_preferences_into_bitmap                              *
*                                                              *
****************************************************************/
/*
 * Function name :  
 * -------------
 *
 * Description :  Sets the gsm codec bitmap based on the speech version.
 *
 *
 * Parameters:  speech version ; bitmap
 * ----------  
 *
 *
 *
 * Returns:     void
 * --------
 *
 */

void MN_read_gsm_speech_preferences_into_bitmap(gsm_speech_ver_e_type  speech_ver,byte *bitmap_p)
{
/*-----------------------BITMAP----------------------------- */
/* ----------------------------------------------------------*/
/* |   08    |    07   |   06     |   05   |  04    | 03    |  02   |   01  | */ 
/* ----------------------------------------------------------*/
/* | XXXX  | XXXX  | FR AMR  | HR   |  FR    | GSM  | GSM | GSM  | */  
/* | XXXX  | XXXX  |   WB    | AMR  | AMR    | EFR  |  HR |  FR  | */ 
/* -----------------------------------------------------------*/
  switch (speech_ver)
  {
   /*  GSM FR  */
   case GSM_FR_SPEECH_VER_1 :
         *bitmap_p |= 0x01;
   break;   

   /*  GSM HR  */
   case GSM_HR_SPEECH_VER_1 :
         *bitmap_p |= 0x02;
   break;  

   /*  GSM EFR  */
   case GSM_FR_SPEECH_VER_2 :
         *bitmap_p |= 0x04;
   break;  

   /*  FR AMR  */
   case GSM_FR_SPEECH_VER_3 :
         *bitmap_p |= 0x08;
   break;  

   /*  HR AMR  */
   case GSM_HR_SPEECH_VER_3 :
         *bitmap_p |= 0x10;
   break;  

   /*  FR AMR WB  */
   case GSM_FR_SPEECH_VER_5 :
         *bitmap_p |= 0x20;
   break;  

   default :
   break;
  }
}

/***************************************************************
*                                                              *
*                     MN_whereami                              *
*                                                              *
****************************************************************/
/*
 * Function name :  
 * -------------
 *
 * Description :  Sets the Multicall_Status_index for any 
 * -----------    calling function
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  
 *
 *
 *
 * Returns:    belongs  byte - (Multicall_Status_index)
 * --------
 *
 */

byte
MN_whereami (connection_id_T connection_id)
{

   byte Multicall_Status_index;
   int index;
   byte belongs;

   MN_MULTICALL_LOG(MN_read_multicall_status());

   belongs = MN_INVALID_CALL_SESSION;

   for (Multicall_Status_index = 0; Multicall_Status_index < 3; Multicall_Status_index++)
   {
      for (index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
      {
         if(connection_id == Multicall_Status[Multicall_Status_index].
             connection_id[index])
         {
            belongs = Multicall_Status_index;
         }
      }
   }

   return belongs;

}

/***************************************************************
*                                                              *
*                     MN_whichMultiCallIndex                   *
*                                                              *
****************************************************************/
/*
 * Function name :  
 * -------------
 *
 * Description :  Sets the Multicall_Status_index for any 
 * -----------    calling function with out having to reinitializing
 *                the multicall status table with MN_read_multicall_status()
 *                function.  
 *
 * Parameters: connection_id
 * ----------  
 *
 *
 *
 * Returns:    belongs  byte - (Multicall_Status_index)
 * --------
 *
 */

byte MN_whichMultiCallIndex (connection_id_T connection_id)
{

   byte Multicall_Status_index;
   int index;
   byte belongs;

   belongs = MN_INVALID_CALL_SESSION;

   for (Multicall_Status_index = 0; Multicall_Status_index < 3; Multicall_Status_index++)
   {
      for (index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
      {
         if(connection_id == Multicall_Status[Multicall_Status_index].
             connection_id[index])
         {
            belongs = Multicall_Status_index;
         }
      }
   }

   return belongs;
}


/***************************************************************
*                           *
*                     MN_call_over               *
*                           *
****************************************************************/
/*
 * Function name :  MN_call_over
 * -------------
 *
 * Description :  Determine whether the connection given is
 * -----------    being closed - even if part of a MPTY call
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  
 *
 *
 *
 * Returns:  call_over (boolean)
 * --------
 *
 */

boolean
MN_call_over(connection_id_T connection_id)

{

   int index,max_index;
   connection_id_T  test_connection_id;
   byte Multicall_Status_index,cc_state = CC_STATE_U0;
   boolean call_over = TRUE;

   Multicall_Status_index = MN_whereami(connection_id);

   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid connection id");
   
   }
   else
   {
      call_over = TRUE;
      max_index = Multicall_Status[Multicall_Status_index].id_index;
      for(index = 0; index < max_index; index++)
      {

         test_connection_id = (connection_id_T)Multicall_Status[Multicall_Status_index].connection_id[index];

         if(test_connection_id < MAXNO_CC_TRANSACTIONS)
         {
            if(cc_transaction_information_p[test_connection_id] != NULL)
            {
              cc_state = cc_transaction_information_p[test_connection_id]->cc_state;
            }
            else
            {
              cc_state = CC_STATE_U0;
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= mn_call_information_p is NULL, cc_state is U0\n" );
            }
         }
         else
         {
            cc_state = CC_STATE_U0;
            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection id = %d\n", test_connection_id );
         }

         if( (cc_state != CC_STATE_U11) && (cc_state != CC_STATE_U12) && (cc_state != CC_STATE_U19) )

         {
            call_over = FALSE;

            break;
         }
      }
   }

   return (call_over);
}



/*
 * Function name :  MN_clear_call_data
 * -------------
 *
 * Description :  Zeros all the mn_call_information for a released call
 * -----------    
 *
 *
 *
 * Parameters: connection_id
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_clear_call_data(connection_id_T connection_id)
{

   /* Locals */

#ifdef FEATURE_UUS
   void *user_data;
#endif

   if (connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for mn_call_information_p");

   }
   else
   {
     
#ifdef FEATURE_UUS
      /* Clear any pending UserUser Service & Buffer */
     while((user_data = MN_get_user_data(connection_id)) != NULL)
     {
          MN_free_user_data((cm_mo_user_data_req_T*) user_data);
     }
#endif

MN_unreserve_call_object(connection_id);

   }
}

/*
 * Function name :  MN_check_for_inband_message
 * -------------
 *
 * Description :  Checks 
 * -----------     
 *
 *
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

boolean MN_check_for_inband_message(connection_id_T connection_id)
{

/* Locals */

   mn_call_information_T   *mn_info_ptr;
   byte                    progress_description;
   mn_info_ptr = mn_call_information_p[connection_id];

   if(mn_info_ptr != NULL)
   {
     progress_description = mn_info_ptr->progress_indicator.progress_description;

     /* Progress description value are extracted from TS24.008, in-band signaling
        section 5.5.1 */
     if (( mn_info_ptr->progress_indicator_present == TRUE) &&
         ((progress_description == 1) ||
          (progress_description == 2) ||
          (progress_description == 3) ||
          ((progress_description >= 6) && (progress_description <=20))))
     {
        return (TRUE);
     }
     else
     {
        return (FALSE);
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot check for inband msgs as mn_call_information_p[%d] is NULL",
                              connection_id);
     return (FALSE);
   }
}


boolean MN_check_for_inband_message_subs(connection_id_T connection_id, sys_modem_as_id_e_type as_id)
{

/* Locals */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   mn_call_information_T   *mn_info_ptr;
   byte                    progress_description;

   if(MNCNM_IS_SINGLE_STACK)
   {
     mn_info_ptr = mn_call_information_p_sim[0][connection_id];
   }
   else
   { 
     mn_info_ptr = mn_call_information_p_sim[as_id][connection_id];
   }

   if(mn_info_ptr != NULL)
   {
     progress_description = mn_info_ptr->progress_indicator.progress_description;

     /* Progress description value are extracted from TS24.008, in-band signaling
        section 5.5.1 */
     if (( mn_info_ptr->progress_indicator_present == TRUE) &&
         ((progress_description == 1) ||
          (progress_description == 2) ||
          (progress_description == 3) ||
          ((progress_description >= 6) && (progress_description <=20))))
     {
        return (TRUE);
     }
     else
     {
        return (FALSE);
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot check for inband msgs as mn_call_information_p[%d] is NULL",
               connection_id);
     return (FALSE);
   }
#else
    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: DSDA not defined! Incorrect API usage");
   return MN_check_for_inband_message(connection_id);
#endif   
}






/*
 * Function name :  MN_num_of_calls_in_group
 * -------------
 *
 * Description :  Get the number of calls in active or held call group
 * -----------
 *
 * Prerequisite :  Multicall_Status[] should have been updated
 * -----------
 *
 * Parameters: int group (either CALL_ACTIVE or CALL_HELD)
 * ----------  
 *
 * Returns: number of calls in active call or active MPTY call
 * --------
 *
 */

byte MN_num_of_calls_in_group(int group)
{
  if ( group != CALL_ACTIVE && group != CALL_HELD )
  {
     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid param: %d", group);
    return 0xff;
  }

  /* select which group is the one you're looking for */
  if ( Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == group )
  {
    return Multicall_Status[MN_ACTIVE_HOLD_SESSION].id_index;
  }
  else if ( Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == group )
  {
    return Multicall_Status[MN_HOLD_ACTIVE_SESSION].id_index;
  }
  else  /* group couldn't be found : return 0 */
  {
    return 0;
  }
}


/*
 * Function name :  MN_handle_srvcc_ho_complete
 * -------------
 *
 * Description :  Process MNCC_SRVCC_HANDOVER_COMPLETE_IND Message from CNM. 
 *
 *Based on the trigger from CNM , MN will get context from CM or MN will do the transient procedures.
 *
 * Parameters: message_header
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_handle_srvcc_ho_complete(IMH_T *message_header)
{


    MNCC_SRVCC_HANDOVER_COMPLETE_IND_T   *message_ptr;
    message_ptr = (MNCC_SRVCC_HANDOVER_COMPLETE_IND_T *)((void *)message_header);
    if(message_ptr->first_trigger == TRUE)
    {
      MN_is_lte_deact_in_progress = TRUE;
      MN_put_cm_get_srvcc_context_req(message_ptr->active_rat);
    }
    else
    {
       byte counter;
       for(counter = 0;counter < MAX_TRANSIENT_TRIGGERS; counter++)
       {
          switch (buffer_transient_triggers[counter].trigger)
          {
             case T_2_SEND:
             {
                cm_n_send_req_T *message;
                message = (cm_n_send_req_T *)CNM_alloc(sizeof(cm_n_send_req_T));
			
#ifdef FEATURE_DUAL_SIM
                if(MNCNM_IS_SINGLE_STACK)
                {
                   if(mn_as_id != SYS_MODEM_AS_ID_NONE)
                   {
                     message->as_id = mn_as_id;
                   }
                   else
                   {
                      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error! mn_as_id is NULL ");
                    }
                 }
      			 
#if defined(FEATURE_DUAL_ACTIVE)
                  else
                  {
                    message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                  }
#endif
#endif
                  MN_process_cm_2_send(message);
                  CNM_free(message);
              }
              break;

              case T_MT_CALL_RES:
              {
                cm_mt_call_res_T  *message;
                message = (cm_mt_call_res_T *)CNM_alloc(sizeof(cm_mt_call_res_T));
              
                message->cm_accept = TRUE;
                message->connection_id = buffer_transient_triggers[counter].connection_id;
#ifdef FEATURE_DUAL_SIM
                if(MNCNM_IS_SINGLE_STACK)
                {
                   if(mn_as_id != SYS_MODEM_AS_ID_NONE)
                   {
                     message->as_id = mn_as_id;
                   }
                   else
                   {
                      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Error! mn_as_id is NULL ");
                    }
                 }
      			 
#if defined(FEATURE_DUAL_ACTIVE)
                  else
                  {
                    message->as_id = mn_dsda_as_id;
                  }
#endif
#endif
                MN_handle_cm_mt_call_res((IMH_T *)message);
              
                CNM_free(message);
              }
              break;

              case T_START_T310:
              {
                 CNM_cc_start_timer(TIMER_T310,buffer_transient_triggers[counter].connection_id);  /* Start Timer T310 if Early media is not present in U3 call */
              }
              break;

              default:
              break;

            }

            buffer_transient_triggers[counter].connection_id = UNKNOWN_CONNECTION_ID;
            buffer_transient_triggers[counter].trigger = T_NULL;

          }
		
          MN_is_lte_deact_in_progress = FALSE;
      }
   
}
