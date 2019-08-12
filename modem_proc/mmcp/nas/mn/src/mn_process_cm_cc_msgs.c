/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_process_cm_cc_msgs.c_v   1.17   10 Jul 2002 15:44:16   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_process_cm_cc_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/2/01      CD     Removed compile warning C4244

5/2/01      CD     Fixed compilation warning C4244 (conversion from int to byte)
                   Fixed compilation warning C4701 (initialized error=FALSE)
                   Fixed compilation warning C4701 (moved up emergency call decoding)
             
5/3/01      CD     Added initialization of MNCC_MODIFY_data in 
                   MN_handle_mmi_mo_modify_req() to avoid compile warning
                   C4701 - local variable may be used without being initialized

                   Changed function definition for MN_handle_mmi_abort_cc_req
                   to avoid warning C4100 - unreferenced formal param

5/3/01      CD     Changed function definition of MN_process_mmi_0_send

5/3/01      CD     Changed function definition of MN_process_mmi_1_send
                   and MN_process_mmi_2_send to avoid compilation warning 
                   C4100 - unreferenced formal parameter

                   Initialized Multicall_Status_index to 0 in 
                   MN_process_mmi_2_send() to avoid compile warning C4701

5/3/01      CD     Changed MN_process_mmi_3_send function declaration to
                   avoid compile warning C4100 - unreferenced formal parameter

5/3/01      CD     Changed MN_process_mmi_mo_end_call_req function declaration
                   to avoid compile warning C4100 - unreferenced formal parameter

5/9/01      CD     Removed unnecesary header file includes

5/23/01     CD     Removed #includes for ghdi_exp.h, memory.h and ms.h since they are
                   not used

                   Corrected initialization of null_mmi_connected_subaddress in
                   MN_process_mmi_2_x_send to avoid lint error

                   Deleted 'ptr' from MN_process_mmi_mo_end_call_req since it was
                   not accessed

7/18/01     CD     Removed duplicate external declaration of Multicall_Status to mn_cc.h
                   Removed duplicate external declaration of mn_call_information
                   Removed external declaration of tch_user_connections_store

8/10/01     CD     Removed unnecessary queue.h.

8/10/01     CD     Removed unnecessary queue.h

9/30/01     AB     Moved SS call related request from CM to 
                      mn_process_cm_ss_cc_msgs.c module.

01/25/02    CD     Replaced IEI instances by boolean flag
                   Replaced calls to ERR() by MSG_ERROROR()
                   Updated copyright

02/10/02    AB     Send CM_MT_CALL_IND when there is incoming call and 
                   current active call in MultiCall Status table can either 
                   be in CALL_ACTIVE, or CALL_HELD state.
                
02/12/02    AB     Updated the MN_read_multicall_status() to handle release 
                   of multicall party release correctly.  Otherwise, other 
                   asynchroous subsequence MO or MT call related action 
                   based on the MultiCalStatus[] table shall not 
                   be able to process correctly.

25-Feb-2002  VT    In MN_get_cm_mo_call_req(), corrected the MSG_ERROR if 
                   emerg_call_message_ptr->bearer_capability.present is
                   neither TRUE nor FALSE.
                    
16-March-2002 AB   Updated the call to MN_put_MNCC_DISC_REQ() per 
                   interface change to support SS CD feature.

06-May-2002   AB   Updated the MN_read_multicall_status() to passed 
                   Lint, and replaced constant value with newly defined
                   call SESSION literals.

01-June-2002  AB   Corrected previous Lint changes in MN_read_multicall_status()
                   to update the MO_CALL_PENDING flag for given connection ID 
                   only when it is an MO call and currently in CC_STATE_U0 state.

06/3/02       CD   Merged in changes from revision 1.10.1.0
                   Updated calls to MN_disconnect_TCH()
                   
06/23/02      AB   Updated MN_read_multicall_status() to handle special 
                    case of split and disconect of a 2 parties-MPTY. 
 
06/25/02      AB   Changed ERROR() to MSG_ERROR as Cheetah build can't
                   reference the ERROR macros.

07/09/02      AB   In MN_process_cm_end_call_req(), clear the mo_setup_pending 
                   flag in the mn_call_information[] of given connection ID 
                   when user request a call to be ended, so that a new call 
                   can be process.

07/29/02      AB   Fixed the MSVC++ warning line 1301. Added the 
                   MN_read_multicall_status(), function description.

08/20/02      AB   In MN_read_multicall_status(), do not update AUX call state 
                   in the MPTY_REQUEST call session.  Also, do not 
                   perform special process for 1 party in MPTY.

09/02/02      AB   In MN_handle_cm_mt_setup_res(), Added a hack to
                   set the default cause value to passed GSM 11.10 conformance
                   test spec 26.8.1.3.1.1, while waiting to UI/CM to fix it.

10/08/02      AB   Added MN_process_cm_mo_end_x_call_req() to abort or 
                   disconnect a specific call.

02/12/03      AB   Removed lint errors.

04/16/03      AB   Added provisioned to abort a MO call that is still 
                   in IDLE state or connection established pending state.

11/18/04      AB   Fixed CR53051, problem in aborting MM connection
                   establish pending call in MN_process_cm_mo_end_x_call_req().

03/22/05      AB   Validate the MT setup response, MN_handle_cm_mt_setup_res(),
                   for dual service selection.

05/10/05      HS   Added support for FEATURE_CCBS

06/06/02      AB   Sending ALERTING if TCH already been assigned in 
                   MN_handle_cm_mt_setup_res().

11/09/05      AB   Added supports for FEATURE_REL5 and 
                   FEATURE_EMERGENCY_CATEGORY

01/18/06      AB   Added supportes for immediate modify indication.

04/04/06      NR   Properly checking the condition for including emergency category IE

07/18/06      NR   Stopping HOLD timer on receipt of disconnect message

02/11/06      NR   Fixed lint errors

12/22/06      RD   Fixed lint errors

07/06/11    PM    Guarding against NULL pointer access 
09/12/11      HC   Replacing the #include "timers.h" with #include "timers_v.h"
                   as a part of CMI activity
                   Replacing the #include "ds_cap.h" with #include "ds_cap_v.h"
                   as a part of CMI activity
 
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "cause.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "ds_cap_v.h"
#include "msg.h"
#include "err.h"
#include "ms.h"
#include "cause.h"
#include "cmlib.h"
#include "cnm.h"
#include "mm.h"
#include "nasutils.h"


extern boolean is_cs_wcdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_tdscdma_mode( sys_modem_as_id_e_type as_id );

extern void MN_process_bearer_capability_ie(IMH_T *message_ptr);

#ifdef FEATURE_ENHANCED_AMR
extern void    ghdi_reset_sr_and_inform_AS(sys_radio_access_tech_e_type active_rat);
#endif

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 * Function name :  MN_handle_cm_mt_modify_res
 * -------------
 *
 * Description :  Handle MMI's response to a status modify requset
 * -----------     
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
VOID_FUNC MN_handle_cm_mt_modify_res(IMH_T *message)
{
   cm_mt_modify_res_T     *message_ptr;
   mn_call_information_T   *data_ptr;
   MNCC_MODIFY_data_T      modify_data;



   /* Align the pointers */
   message_ptr = (cm_mt_modify_res_T *)message;

#if defined(FEATURE_DUAL_SIM )
   if( (MNCNM_IS_SINGLE_STACK) && (message_ptr->as_id == mn_as_id))
#endif  
   {
     /* KlockWork Fix */
     if(message_ptr->connection_id < MAXNO_CC_TRANSACTIONS) 
     {
      if ((cc_transaction_information_p[message_ptr->connection_id] != NULL) && 
         (CC_STATE_U27 == cc_transaction_information_p[message_ptr->connection_id]->cc_state ) &&
         (mn_call_information_p[message_ptr->connection_id] != NULL))
      {
        /* GET THE CALL INFO */
        data_ptr = mn_call_information_p[message_ptr->connection_id];

        /* IF CM HAS ACCEPTED MODIFY RESPONSE */
        if(((message_ptr->cm_accept == TRUE) &&
           (data_ptr->active_bearer_capability == 1)) ||
           ((message_ptr->cm_accept == FALSE ) &&
           (data_ptr->active_bearer_capability == 0)))

       {
          /* LOAD DETAILS IN TO MODIFY DATA STORE */
          modify_data.cm_bearer_capability = data_ptr->bearer_capability_1;

          /* IF NO LOW LAYER COMP PRESENT */
          if( (data_ptr->repeat_indication_llc == 0) ||
              (data_ptr->low_layer_compatibility_1.present == FALSE) )
          {
             modify_data.repeat_indication_llc = 0;
          }
          /* IF HIGH LAYER COMP IS PRESENT */
          else
          {
             modify_data.repeat_indication_llc = 1;
             modify_data.cm_low_layer_compatibility = 
                 data_ptr->low_layer_compatibility_1;
          }

          /* IF NO LOW LAYER COMP PRESENT */
          if( (data_ptr->repeat_indication_hlc == 0) ||
              (data_ptr->high_layer_compatibility_1.present == FALSE) )
          {
             modify_data.repeat_indication_hlc = 0;
          }

          /* IF HIGH LAYER COMP IS PRESENT */
          else
          {
             modify_data.repeat_indication_hlc = 1;
             modify_data.cm_high_layer_compatibility = 
                 data_ptr->high_layer_compatibility_1;
          }
       }
       else
       {
          /* USE THE SECOND BC/HLC/LLC */
          modify_data.cm_bearer_capability = data_ptr->bearer_capability_2;

          if( (data_ptr->repeat_indication_llc < REPEAT_SUPPORTS_FALLBACK) ||
              (data_ptr->low_layer_compatibility_2.present == FALSE) )
          {
             modify_data.repeat_indication_llc = 0;
          }
          else
          {
             modify_data.repeat_indication_llc = 1;
             modify_data.cm_low_layer_compatibility = 
                 data_ptr->low_layer_compatibility_2;
          }

          if( (data_ptr->repeat_indication_hlc < REPEAT_SUPPORTS_FALLBACK) ||
              (data_ptr->high_layer_compatibility_2.present == FALSE) )
          {
             modify_data.repeat_indication_hlc = 0;
          }
          else
          {
             modify_data.repeat_indication_hlc = 1;
             modify_data.cm_high_layer_compatibility = 
             data_ptr->high_layer_compatibility_2;
          }
       }

       modify_data.connection_id = message_ptr->connection_id;

       /* LOAD CAUSE INFORMATION */
       if(message_ptr->cm_accept == FALSE)
       {
          modify_data.cause_present = TRUE;
          modify_data.cm_cc_cause.coding_standard = STD_CODESTANDARD;
          modify_data.cm_cc_cause.location = STD_LOCATION;
          modify_data.cm_cc_cause.cause_value = 
              BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE;
          modify_data.cm_cc_cause.recommendation_present = FALSE;
          modify_data.cm_cc_cause.diagnostic_length = 0;
          modify_data.reverse_call_direction = FALSE;
       }

       else
       {
          modify_data.cause_present = FALSE;
          modify_data.reverse_call_direction = data_ptr->reverse_call_direction;
       }

       MN_put_MNCC_MODIFY_RES(message_ptr->connection_id, &modify_data);

       /*  CHANGE THE CHANNEL BC  */

       if(message_ptr->cm_accept == TRUE)
       {
          data_ptr->no_of_changes++;

          /* replaced statement to avoid compile warning C4244 */
          data_ptr->active_bearer_capability = 
              (byte)((data_ptr->active_bearer_capability + 1) % 2);  

          data_ptr->modifiy_in_progress = TRUE;
          mn_store_rab_modify_status();
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= modifiy_in_progress(conn:%d) set to True ", message_ptr->connection_id);

          if(data_ptr->immediate_modify_ind == TRUE)
          {
              boolean is_G_RAT = FALSE;
#ifdef FEATURE_DUAL_SIM
              if(MNCNM_IS_SINGLE_STACK)
              {
                is_G_RAT = is_gsm_mode_per_subs(mn_as_id);
              }
#ifdef FEATURE_DUAL_ACTIVE
              else
              {
                is_G_RAT = is_gsm_mode_per_subs(mn_dsda_as_id);
              }
#endif
#else
              is_G_RAT = is_gsm_mode();
#endif
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= No change in Radio Bearer: immediate_modify = %d", data_ptr->immediate_modify_ind);

              /* Reset vocoder w/o release the RAB/TCH */
              if(is_G_RAT == TRUE)
              {    
                  MN_disconnect_TCH(MN_VOCODER_SWITCH_TO_WCDMA);  /* turn off GSM vocoder */  
              }
              else
              {
                  MN_disconnect_TCH(MN_VOCODER_SWITCH_TO_GSM);   /* turn off WCDMA vocoder */
              }
          
              /* Reconnect the new active service w/ same TCH */
              if ( data_ptr->active_bearer_capability == 0)
              {
                 MN_connect_TCH( data_ptr->bearer_capability_1.information_transfer_capability);
              }
              else
              {
                 MN_connect_TCH(
                     data_ptr->bearer_capability_2.information_transfer_capability);
              }

              /* reset the immediate flag */
              data_ptr->immediate_modify_ind = FALSE;
          }
          else
          {
#if defined(FEATURE_DUAL_SIM )          
              if(MNCNM_IS_SINGLE_STACK)
              {
                MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,message_ptr->as_id); /* turn off GSM vocoder */
              }
              else
#endif
              {
                MN_disconnect_TCH(MN_VOCODER_OFF); /* turn off GSM vocoder */
              }
          }

       } /* cm_accept */
    }
   }
   else
   {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection ID = %d", message_ptr->connection_id);

   }
}
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", message_ptr->as_id,mn_as_id); 
   }
#endif
}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */


/*
 * Function name :  MN_get_cm_mo_call_req
 * -------------
 *
 * Description :  Handle initial screening and setup the mobile originate call request from MMI, 
 * -----------     
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

boolean 
MN_get_cm_mo_call_req(connection_id_T new_connection_id, IMH_T *message_ptr)
{
   boolean                       error = FALSE;
   cm_mo_normal_call_req_T      *normal_call_message_ptr;
   cm_mo_emergency_call_req_T   *emerg_call_message_ptr;
   byte                          rpt_ind;

   if(message_ptr->message_id == CM_MO_EMERGENCY_CALL_REQ)
   {
      emerg_call_message_ptr = (cm_mo_emergency_call_req_T *)message_ptr;

      mn_call_information_p[new_connection_id]->CNM_service_type = EMERGENCY_CALL_ESTABLISHMENT;

      /* DECODE EMERGENCY CALLS */
      /*  SET UP BEARER CAPABILITY  */

      if(emerg_call_message_ptr->bearer_capability.present == FALSE)
      {
         /*  MESSAGE IS FROM SPEECH ONLY CM  */

         mn_call_information_p[new_connection_id]->repeat_indicator_bc = 0;
      }
      else
      {
         /* OTHERWISE THERE MUST BE A BEARER CAPABILITY - BUT ONLY ONE
          * FOR EMERGENCY CALL  */

         if(emerg_call_message_ptr->bearer_capability.present == TRUE)
         {
            mn_call_information_p[new_connection_id]->repeat_indicator_bc = 1;

            mn_call_information_p[new_connection_id]->bearer_capability_1 =
                emerg_call_message_ptr->bearer_capability;

         }
         else
         {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid BC IE flag in CM_MO_EMERGENCY_CALL_REQ!!");
             error = TRUE;
         }
      }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      /* STREAM IDENTIFIER */
      if(emerg_call_message_ptr->stream_id.present == TRUE)
      {
         mn_call_information_p[new_connection_id]->stream_id =
                                                  emerg_call_message_ptr->stream_id;

      }
      
      /* SUPPORTED CODEC LIST */
      if(emerg_call_message_ptr->codecs_list.present == TRUE)
      {
         /* Get data from message and put in to mn_call_information */

         mn_call_information_p[new_connection_id]->supported_codecs_list =
                                                  emerg_call_message_ptr->codecs_list;
      }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      /* EMERGENCY CATEGORY */
      if(emerg_call_message_ptr->service_category.present == TRUE)
      {
         mn_call_information_p[new_connection_id]->emergency_category =
                                                  emerg_call_message_ptr->service_category;
      } 

   }
   else
   {
      normal_call_message_ptr = (cm_mo_normal_call_req_T *)message_ptr;


      /*  SET UP A NORMAL MESSAGE  */

      error = FALSE;

      /*  READ BEARER CAPABILITY  */

      if( (normal_call_message_ptr->bc_repeat_indicator.present == FALSE) &&
          (normal_call_message_ptr->bearer_capability_1.present == FALSE) )
      {
         mn_call_information_p[new_connection_id]->
             repeat_indicator_bc = 0;
      }
      else
      {
         /*  OTHERWISE, THERE MUST BE A BEARER CAPABILITY  */
         if(normal_call_message_ptr->bearer_capability_1.present == FALSE)
         {
            /*  UNEXPECTED IEI  */
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= normal call req bearer cap IE not present");
            error = TRUE;
         }

         /*  COPY THE FIRST BC  */

         mn_call_information_p[new_connection_id]->bearer_capability_1 =
             normal_call_message_ptr->bearer_capability_1;


         if(normal_call_message_ptr->bc_repeat_indicator.present == FALSE)
         {

            /*  THERE MUST BE ONLY ONE BEARER CAPABILITY  */
            mn_call_information_p[new_connection_id]->repeat_indicator_bc = 1;
         }
         else
         {
            if(normal_call_message_ptr->bc_repeat_indicator.present == TRUE)
            {
               if(normal_call_message_ptr->bearer_capability_2.present != TRUE)
               {
                  /*  UNEXPECTED IEI  */
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= normal call req bearer cap 2 IEI is not present");
                  
                  error = TRUE;
               }
               /* replace statement to avoid compilation warning C4244 */
               rpt_ind = (byte)(normal_call_message_ptr->bc_repeat_indicator.repeat_indication & 0x0F);

               if(rpt_ind == REPEAT_CIRCULAR)
               {
                   rpt_ind++;
               }
               
               mn_call_information_p[new_connection_id]->repeat_indicator_bc = rpt_ind;

               mn_call_information_p[new_connection_id]->bearer_capability_2 = 
                   normal_call_message_ptr->bearer_capability_2;
            }
            else
            {

               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req bc repeat indicator IE not present");

               error = TRUE;
            }
         }
      }

      /*  SET UP LOW LAYER COMPATIBILIES  */
      if( (normal_call_message_ptr->llc_repeat_indicator.present == FALSE) &&
          (normal_call_message_ptr->low_layer_compatibility_1.present == FALSE) )
      {

         /*  THERE ARE NO LLC IEs  */

         mn_call_information_p[new_connection_id]->repeat_indication_llc = 0;
      }
      else
      {
         /*  OTHERWISE THERE MUST BE A HIGH_LAYER_COMPATIBILITY_1  */

         if(normal_call_message_ptr->low_layer_compatibility_1.present != TRUE)

         /*  UNEXPECTED IEI  */

         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req LL compat IE not present");
            
            error = TRUE;
         }
         else

         /*  OTHERWISE COPY THE FIRST LLC DATA  */

         {
            mn_call_information_p[new_connection_id]->low_layer_compatibility_1 =
                normal_call_message_ptr->low_layer_compatibility_1;
         }

         if(normal_call_message_ptr->llc_repeat_indicator.present == FALSE)

         /*  THERE MUST BE ONLY ONE LOW_LAYER_COMPATIBILITY  */

         {
            mn_call_information_p[new_connection_id]->
                repeat_indication_llc = 1;
         }
         else
         {
            if(normal_call_message_ptr->llc_repeat_indicator.present == TRUE)
            {

               /*  WITH A VALID REPEAT INDICATOR, THERE MUST BE TWO 
                *  LOW_LAYER_COMPATIBILITIES */

               if(normal_call_message_ptr->low_layer_compatibility_2.present != TRUE)
               {
                  /*  UNEXPECTED IEI  */
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req LL compat 2 IE not present");

                  error = TRUE;
               }
               else
               {
                  /* replace statement to avoid compilation warning C4244 */
                  rpt_ind =  (byte)(normal_call_message_ptr->llc_repeat_indicator.
                      repeat_indication & 0x0F);

                  if ( rpt_ind == 1 )
                  {
                      rpt_ind++;
                  }

                  mn_call_information_p[new_connection_id]->
                      repeat_indication_llc = rpt_ind;

                  mn_call_information_p[new_connection_id]->
                      low_layer_compatibility_2 = 
                      normal_call_message_ptr->low_layer_compatibility_2;

               }
            }
            else
            {
               /*  UNEXPECTED IEI  */
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req LLC repeat indicator IE not present");

               error = TRUE;
            }
         }
      }

      /*  SET UP HIGH LAYER COMPATIBILIES  */

      if( (normal_call_message_ptr->hlc_repeat_indicator.present == FALSE) &&
          (normal_call_message_ptr->high_layer_compatibility_1.present == FALSE) )
      {

         /*  THERE ARE NO HLC IEs  */

         mn_call_information_p[new_connection_id]->repeat_indication_hlc = 0;
      }
      else
      {
         /*  OTHERWISE THERE MUST BE A HIGH_LAYER_COMPATIBILITY_1  */

         if(normal_call_message_ptr->high_layer_compatibility_1.present != TRUE)

         /*  UNEXPECTED IEI  */

         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req HL compat IE not present");
            error = TRUE;
         }
         else

         /*  OTHERWISE COPY THE FIRST HLC DATA  */

         {
            mn_call_information_p[new_connection_id]->high_layer_compatibility_1 =
                normal_call_message_ptr->high_layer_compatibility_1;
         }

         if(normal_call_message_ptr->hlc_repeat_indicator.present == FALSE)

         /*  THERE MUST BE ONLY ONE HIGH_LAYER_COMPATIBILITY  */

         {
            mn_call_information_p[new_connection_id]->
                repeat_indication_hlc = 1;
         }
         else
         {
            if(normal_call_message_ptr->hlc_repeat_indicator.present == TRUE)
            {

               /*  WITH A VALID REPEAT INDICATOR, THERE MUST BE TWO 
                *  HIGH_LAYER_COMPATIBILITIES  */

               if(normal_call_message_ptr->high_layer_compatibility_2.present != TRUE)
               {
                  /*  UNEXPECTED IEI  */
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req HL compat 2 IE not present");

                  error = TRUE;
               }
               else
               {
                  /* replace statement to avoid compilation warning C4244 */
                  rpt_ind =  (byte)(normal_call_message_ptr->llc_repeat_indicator.
                      repeat_indication & 0x03);

                  if ( rpt_ind == 1 )
                  {
                     rpt_ind++;
                  }
                  mn_call_information_p[new_connection_id]->
                      repeat_indication_hlc = rpt_ind;

                  mn_call_information_p[new_connection_id]->
                      high_layer_compatibility_2 = 
                      normal_call_message_ptr->high_layer_compatibility_2;

               }
            }
            else
            {
               /*  UNEXPECTED IEI  */
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req HLC repeat ind IE is present");
               error = TRUE;
            }
         }
      }

      /*  READ AND STORE CALLED PARTY BCD NUMBER  */

      if(normal_call_message_ptr->cm_called_party_bcd_number.present == 
          FALSE)
      {
         mn_call_information_p[new_connection_id]->
             called_party_bcd_number_present = FALSE;
      }
      else
      {
         if(normal_call_message_ptr->cm_called_party_bcd_number.present == TRUE)
         {
            mn_call_information_p[new_connection_id]->called_party_bcd_number =
                normal_call_message_ptr->cm_called_party_bcd_number;

            mn_call_information_p[new_connection_id]->
                called_party_bcd_number_present = TRUE;
         }
         else
         {
            /*  UNEXPECTED IEI  */
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req called partry bcd IE not present");  
            error = TRUE;
         }
      }


      /*  READ AND STORE CALLED PARTY SUBADDRESS  */

      if(normal_call_message_ptr->cm_called_party_subaddress.present == 
          FALSE)
      {
         mn_call_information_p[new_connection_id]->
             called_party_subaddress_present = FALSE;
      }

      else
      {
         if(normal_call_message_ptr->cm_called_party_subaddress.present == TRUE)
         {
            mn_call_information_p[new_connection_id]->called_party_subaddress =
                normal_call_message_ptr->cm_called_party_subaddress;
            mn_call_information_p[new_connection_id]->
                called_party_subaddress_present = TRUE;
         }

         else
         {
            /*  UNEXPECTED IEI  */

            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req called party subadd IE not present");

            error = TRUE;
         }
      }

      /*  READ AND STORE CALLING PARTY SUBADDRESS  */

      if(normal_call_message_ptr->cm_calling_party_subaddress.present == 
          FALSE)
      {
         mn_call_information_p[new_connection_id]->
             calling_party_subaddress_present = FALSE;
      }
      else
      {
         if(normal_call_message_ptr->cm_calling_party_subaddress.present == TRUE)
         {
            mn_call_information_p[new_connection_id]->calling_party_subaddress =
                normal_call_message_ptr->cm_calling_party_subaddress;
            mn_call_information_p[new_connection_id]->
                calling_party_subaddress_present = TRUE;
         }
         else
         {
            /*  UNEXPECTED IEI  */
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Normal call req calling party subadd IE not present");
            error = TRUE;
         }
      }

      /*  SET UP CLIR  */
      mn_call_information_p[new_connection_id]->clir_suppression = FALSE;
      mn_call_information_p[new_connection_id]->clir_invocation = FALSE;

      if(normal_call_message_ptr->cm_clir == CM_CLIR_INVOCATION)
      {
         mn_call_information_p[new_connection_id]->clir_invocation = TRUE;
      }

      else
      {
         if(normal_call_message_ptr->cm_clir == CM_CLIR_SUPPRESSION)
         {
            mn_call_information_p[new_connection_id]->clir_suppression = TRUE;
         }

         else
         {
            /*  UNEXPECTED IEI  */
            if(normal_call_message_ptr->cm_clir != FALSE)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid IEI in cm_mo_normal_call_req message");
               error = TRUE;
            }
         }
      }

      /*  READ FORWARD CUG INFO  */

      memset(&(mn_call_information_p[new_connection_id]->cm_forward_cug_info),
          0,
          sizeof(cm_forward_cug_info_T) );

      if(normal_call_message_ptr->forward_cug_info.present == TRUE)
      {
         /* Get data from message and put in to mn_call_information_p */

         mn_call_information_p[new_connection_id]->cm_forward_cug_info =
             normal_call_message_ptr->forward_cug_info;
      }

   /* Set up CNM service request type */
      /* Set up CNM service request type */
      if (mn_call_information_p[new_connection_id]->repeat_indicator_bc == 1 )
      {
         /* Only 1 BC */
         if (mn_call_information_p[new_connection_id]->bearer_capability_1.
               radio_channel_requirement == DUAL_SUPPORT_HALF_PREFERRED)
         {
            if (
#ifdef FEATURE_ALS
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH ) ||
#endif
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_SPEECH )
               )
            {

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_HALF_SPEECH;
            }
            else
            { 

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_DATA_4800;
            }
         }
         else
         {
            if (
#ifdef FEATURE_ALS
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
               )
            {

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_FULL_SPEECH;
            }
            else
            { 

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_DATA_9600;
            }
         }
      }
      else
      {
        /* More than 1 Bearer Capability  */
         if ((mn_call_information_p[new_connection_id]->bearer_capability_1.
               radio_channel_requirement == DUAL_SUPPORT_HALF_PREFERRED)&&
            (mn_call_information_p[new_connection_id]->bearer_capability_2.
               radio_channel_requirement == DUAL_SUPPORT_HALF_PREFERRED))
         {
            
           if (
#ifdef FEATURE_ALS
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH ) ||
#endif
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_SPEECH )
               )
            {

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_HALF_SPEECH;
            }
            else
            { 

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_DATA_4800;
            }

         }
         else
         {
            if (
#ifdef FEATURE_ALS
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                (mn_call_information_p[new_connection_id]->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
               )
            {

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_FULL_SPEECH;
            }
            else
            { 

               mn_call_information_p[new_connection_id]->
                            CNM_service_type = MO_CALL_ESTABLISHMENT_DATA_9600;
            }
         }
      }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      /* STREAM IDENTIFIER */
      if(normal_call_message_ptr->stream_id.present == TRUE)
      {

         mn_call_information_p[new_connection_id]->stream_id =
                       normal_call_message_ptr->stream_id;

      }

      /* SUPPORTED CODEC LIST */
      if(normal_call_message_ptr->codecs_list.present == TRUE)
      {
         /* Get data from message and put in to mn_call_information */

         mn_call_information_p[new_connection_id]->supported_codecs_list =
                       normal_call_message_ptr->codecs_list;
      }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

   }

   return error;
}

/***************************************************************
*                                                              *
*                   MN_handle_cm_abort_cc_req                 *
*                                                              *
****************************************************************/
/*
 * Function name : MN_handle_cm_abort_cc_req 
 * -------------
 *
 * Description :  CM wishes to have all calls cleared for 
 * -----------     a power down
 *
 *
 *
 *
 *
 *
 * Parameters: none
 * ----------  
 *
 *
 *
 * Returns: void
 * --------
 *
 */
VOID_FUNC MN_handle_cm_abort_cc_req(cm_abort_cc_req_T *abort_req)
{
   ie_cm_cc_cause_T cm_cc_cause;
   int mc_index,
   index;
   connection_id_T   connection_id;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || 
       (abort_req->as_id != SYS_MODEM_AS_ID_NONE && 
       (mn_as_id == SYS_MODEM_AS_ID_NONE || abort_req->as_id == mn_as_id)))
#endif  
   {
   
     /*  CM WISHES TO HAVE ALL CALLS CLEARED FOR A POWER DOWN  */
     /*  REQUEST AN ABORT FOR A WAITING CALL  */
     /*  REQUEST A DISCONNECT CALL FOR EACH ACTIVE OR HELD CALL  */

     cm_cc_cause.location = STD_LOCATION;
     cm_cc_cause.coding_standard = STD_CODESTANDARD;
     cm_cc_cause.recommendation_present = FALSE;
     cm_cc_cause.cause_value = 16;          /* Normal Clearing */
     cm_cc_cause.diagnostic_length = 0;

     /* Refresh the Multicall_status [], call session */

     MN_MULTICALL_LOG(MN_read_multicall_status());


   if ((Multicall_Status[0].call_activity == CALL_INACTIVE)&&
   (Multicall_Status[1].call_activity == CALL_INACTIVE)&&
   (Multicall_Status[2].call_activity == CALL_INACTIVE))
   {

      /* No active calls so return the _conf */
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
        MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,abort_req->as_id);
      }
      else
#endif
      {
        MN_disconnect_TCH(MN_VOCODER_OFF);
      }

      MN_put_cm_abort_cc_conf(
#if defined(FEATURE_DUAL_SIM )
                      abort_req->as_id
#endif
                                );

      /* Assuming GSM context, use DEFAULT_RAB_ID below  */
      tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
   }
   else
   {
      if(Multicall_Status[2].call_activity == CALL_WAITING)

      /* reject waiting call first */
      {
         connection_id = Multicall_Status[2].connection_id[0];

         MN_put_MNCC_DISC_REQ(connection_id, &cm_cc_cause, NULL);

         mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
         mn_call_information_p[connection_id]->mo_release_indicator = CM_ABORT_REL;
      }

      for(mc_index = 0; mc_index < 2; mc_index++)
      {
         for(index = 0; index < (Multicall_Status[mc_index].id_index); index++)
         {
            connection_id = Multicall_Status[mc_index].connection_id[index];

            MN_stop_cc_timer(connection_id);

            MN_put_MNCC_DISC_REQ(connection_id, &cm_cc_cause, NULL);

            mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
            mn_call_information_p[connection_id]->mo_release_indicator = CM_ABORT_REL;

            /* Inhibit the RELEASE_COMPLETE if POWER OFF or OFFLINE */
            if(abort_req != NULL)
            {
                if( (abort_req->cause == (byte)SYS_STOP_MODE_REASON_POWER_OFF) ||
                    (abort_req->cause == (byte)SYS_STOP_MODE_REASON_OFFLINE) )
                {
                    mn_call_information_p[connection_id]->offline = TRUE;
                }
            }
         }
      }
     }
   }
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", abort_req->as_id,mn_as_id);
     
	 
     /* CM wants to send CM_ABORT_CC_CONF from NAS on an inactive sub where there is no call in progress*/
     MN_put_cm_abort_cc_conf(
#if defined(FEATURE_DUAL_SIM )
                      abort_req->as_id
#endif
                            );
   }
#endif
}
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 *   Function name : MN_handle_cm_mo_modify_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the cm_mo_modify_req message from the 
 *  cm to MN.
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_handle_cm_mo_modify_req(IMH_T    *message)
{

   cm_mo_modify_req_T      *pointer;
   boolean                  success;
   MNCC_MODIFY_data_T       MNCC_MODIFY_data;
   mn_call_information_T    *call_data;

   memset(&MNCC_MODIFY_data,0,sizeof(MNCC_MODIFY_data_T));

   /* Requests that the bearer capability of the call be modified
            send message MNCC_MODIFY_REQ  */

   MN_process_bearer_capability_ie(message);

   /* set the pointers */

   pointer = (cm_mo_modify_req_T *)message;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (pointer->as_id == mn_as_id))
#endif  
   {
     /* Determine the active connection id */

     /* Which refreshes multicall status */

     MN_MULTICALL_LOG(MN_read_multicall_status());

     success = TRUE;

     /* If call 1 is active */
     if (Multicall_Status[0].call_activity == CALL_ACTIVE)
     {
        MNCC_MODIFY_data.connection_id = Multicall_Status[0].connection_id[0];
     }
     /* If call 2 is active */
     else
     {
       if  (Multicall_Status[1].call_activity == CALL_ACTIVE)
       {
         MNCC_MODIFY_data.connection_id = Multicall_Status[1].connection_id[0];
       }
       else
       {
         success = FALSE;

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_mo_modify_req received while no call active"); 
      }
     }

     if (success == TRUE)
     {
       /* Set up bearer capability */

       if (pointer->bearer_capability.present != TRUE)
       {
         /* If not bearer capability, return error message */
         success = FALSE;

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Bearer cap IE not present");
       }

       /* Check the bearer_capability */

       call_data = mn_call_information_p[MNCC_MODIFY_data.connection_id];

       if (call_data->active_bearer_capability == 0)
       {
         if (MN_compare_bearer_capability(&(call_data->bearer_capability_2), 
                                          &(pointer->bearer_capability), MO) == FALSE)
         {
            success = FALSE;
         }
      }

      if (call_data->active_bearer_capability == 1)
      {
         if (MN_compare_bearer_capability(&(call_data->bearer_capability_1),
                                          &(pointer->bearer_capability), MO) == FALSE)
         {
            success = FALSE;
         }
      }

      if ( success == FALSE)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: wrong bearer capability"); 
      }


      if (success == TRUE)
      {
         /* Set up the bearer capability */

         MNCC_MODIFY_data.cm_bearer_capability = pointer->bearer_capability;

         /* Set up low layer compatability */

         if (pointer->low_layer_compatibility.present == FALSE)
         {
            MNCC_MODIFY_data.repeat_indication_llc = 0;
         }

         if (pointer->low_layer_compatibility.present == TRUE)
         {
            MNCC_MODIFY_data.repeat_indication_llc = 1;

            MNCC_MODIFY_data.cm_low_layer_compatibility = pointer->low_layer_compatibility;
         }

         /* Set up high layer compatibility */

         if (pointer->high_layer_compatibility.present == FALSE)
         {
            MNCC_MODIFY_data.repeat_indication_hlc = 0;
         }

         if (pointer->high_layer_compatibility.present ==TRUE)
         {
            MNCC_MODIFY_data.repeat_indication_hlc = 1;

            MNCC_MODIFY_data.cm_high_layer_compatibility = pointer->high_layer_compatibility;
         }

         MNCC_MODIFY_data.reverse_call_direction = pointer->reverse_call_setup_direction;

         MN_put_MNCC_MODIFY_REQ(&(MNCC_MODIFY_data));

      }
      else
      {
          cm_modify_complete_data_T  cm_modify_data;


          /* clear the buffer -> KlockWork Fix */
          memset(&cm_modify_data, NULL, sizeof(cm_modify_complete_data_T));
          
          /* modify request complete = false */
          cm_modify_data.complete = FALSE;
          cm_modify_data.cm_cc_cause.present = TRUE;
          cm_modify_data.cm_cc_cause.cause_value = BEARER_CAPABILITY_NOT_AUTHORISED;

          MN_put_cm_mo_modify_complete_conf(&cm_modify_data);
      }
     }
   }
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     cm_modify_complete_data_T  cm_modify_data;

     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 

     /* clear the buffer -> KlockWork Fix */
     memset(&cm_modify_data, NULL, sizeof(cm_modify_complete_data_T));
            
     /* modify request complete = false */
     cm_modify_data.complete = FALSE;
     cm_modify_data.cm_cc_cause.present = TRUE;
     cm_modify_data.cm_cc_cause.cause_value = AS_REJ_ABORT_RADIO_UNAVAILABLE;
     cm_modify_data.as_id = pointer->as_id;
  
     MN_put_cm_mo_modify_complete_conf(&cm_modify_data);
   }
#endif

}

#endif /* FEATURE_MULTIMEDIA_FALLBACK */



/*
 * Function name :  MN_handle_cm_mt_setup_res
 * -------------
 *
 * Description :  handles the cm_mt_setup_res 
 * -----------     message from the mmi
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

VOID_FUNC MN_handle_cm_mt_setup_res(IMH_T *message_header)
{
   boolean                 negotiation;
   mn_call_information_T   *call_data_ptr;
   cm_mt_setup_res_T       *message_ptr;
   

   MN_process_bearer_capability_ie(message_header);

   message_ptr = (cm_mt_setup_res_T *)message_header;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (message_ptr->as_id == mn_as_id))
#endif
   {
     if(message_ptr->cm_accept == FALSE)
     {
       if( (message_ptr->cm_cc_cause.present == FALSE) ||
           (message_ptr->cm_cc_cause.cause_value == 0) || 
           (message_ptr->cm_cc_cause.cause_value == BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE) )
       {
          

          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CAUSE is mandatory for SETUP REJECT %d", message_ptr->connection_id);

          /* patch it up with default value */
          message_ptr->cm_cc_cause.present = TRUE;
          message_ptr->cm_cc_cause.coding_standard = 0x60;
          message_ptr->cm_cc_cause.location = 0x00;
          message_ptr->cm_cc_cause.recommendation_present = FALSE;
          message_ptr->cm_cc_cause.cause_value = 0x58;  /* #88 */
          message_ptr->cm_cc_cause.diagnostic_length = 0;
        }

        MN_put_MNCC_REJ_REQ(message_ptr->connection_id, &(message_ptr->cm_cc_cause));
     }
     else
     {
      /*  CALL HAS BEEN ACCEPTED OR NEGOTIATED             *
                   *  COPY NEGOTIATION DATA LOCALLY        * 
                   *  SET UP POINTER  */

       negotiation = FALSE;
       if(mn_call_information_p[message_ptr->connection_id] != NULL)
       {

         call_data_ptr = mn_call_information_p[message_ptr->connection_id];
  
         if(message_ptr->bc_repeat_indicator.present == TRUE)  
           /* cm has returned a repeat indicator and 2 BCs */
         {
  
           negotiation = TRUE;
  
           if (message_ptr->bc_repeat_indicator.repeat_indication == REPEAT_SUPPORTS_FALLBACK)
           {
             call_data_ptr->repeat_indicator_bc = REPEAT_SUPPORTS_FALLBACK;
           }
           else 
           {
             call_data_ptr->repeat_indicator_bc = REPEAT_SERVICE_CHANGE_FALLBACK;
           }
  
           if((message_ptr->bearer_capability_1.present == TRUE) &&
              (call_data_ptr->bearer_capability_1.present == TRUE) &&
              (message_ptr->bearer_capability_1.information_transfer_capability !=
              call_data_ptr->bearer_capability_1.information_transfer_capability))
           {
             /* Bearers have been swapped. Swap the LLC and HLC*/   
             cm_high_layer_compatibility_T   high_layer_compatibility_BC1;     
             cm_low_layer_compatibility_T    low_layer_compatibility_BC1;
  
             high_layer_compatibility_BC1 = call_data_ptr->high_layer_compatibility_1;
             low_layer_compatibility_BC1 = call_data_ptr->low_layer_compatibility_1;
             call_data_ptr->high_layer_compatibility_1 = call_data_ptr->high_layer_compatibility_2;
             call_data_ptr->low_layer_compatibility_1 = call_data_ptr->low_layer_compatibility_2;
  
             call_data_ptr->high_layer_compatibility_2 = high_layer_compatibility_BC1;
             call_data_ptr->low_layer_compatibility_2 = low_layer_compatibility_BC1;
           }
           call_data_ptr->bearer_capability_1 =
           message_ptr->bearer_capability_1;
           call_data_ptr->bearer_capability_2 =
           message_ptr->bearer_capability_2;  /* new BCIE 2 */
         } /* cm has returned possibly 1 BC */
         else if (message_ptr->bearer_capability_1.present == TRUE)
         {
           negotiation = TRUE;
  
           call_data_ptr->bearer_capability_1 = message_ptr->bearer_capability_1;  
           call_data_ptr->bearer_capability_2 = message_ptr->bearer_capability_2;  /* new BCIE 2 */
           call_data_ptr->repeat_indicator_bc = 1;
  
         } /* cm has returned no BC */
         else
         {
           call_data_ptr->repeat_indicator_bc = 0;
         }
       
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
         /* stream ID */
         if(message_ptr->stream_id.present == TRUE)
         {
            call_data_ptr->stream_id = message_ptr->stream_id; 
         }
         else
         {
            call_data_ptr->stream_id.present = FALSE; 
         }
  
         /* supported codec list IE */
         if(message_ptr->codecs_list.present == TRUE)
         {
            call_data_ptr->supported_codecs_list = message_ptr->codecs_list; 
         }
         else
         {
            call_data_ptr->supported_codecs_list.present = FALSE;
         } 
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
        
         MN_put_MNCC_CALL_CONF_REQ(negotiation,
                                   message_ptr->connection_id,
                                   &(message_ptr->cm_cc_cause));
  
         /*  IF A SIGNAL IEI HAS BEEN RECEIVED FOR THIS CONNECTION, OR
             THERE IS AN EXISTING CALL, THE USER
             MAY BE IMMEDIATELY ALERTED  */
  
         MN_MULTICALL_LOG(MN_read_multicall_status());
  
         if( (call_data_ptr->signal_present == TRUE) ||
             ( (Multicall_Status[0].call_activity == CALL_ACTIVE) ||     /* Active call */
             (Multicall_Status[1].call_activity == CALL_ACTIVE) ||
             (Multicall_Status[0].call_activity == CALL_HELD) ||
             (Multicall_Status[1].call_activity == CALL_HELD) ) ||
             (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) ||  /* TCH already been assigned */
             (CNM_mmcc_sync_ind.reason != ((byte)WCDMA_RAB_RELEASED)) )  /* Active RR/RRC connection */
         {
           MN_put_cm_mt_call_ind(
#if defined(FEATURE_DUAL_SIM )
           mn_call_information_p[message_ptr->connection_id]->as_id,
#endif    
           message_ptr->connection_id
                                );
  
           MN_put_MNCC_ALERT_REQ(message_ptr->connection_id);
         }
       }
       else
       {
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process CM_MT_SETUP_RES as mn_call_information_p[%d] is NULL",
                     message_ptr->connection_id);
       }
    }  /* CALL ACCEPTED */
  }
#if defined(FEATURE_DUAL_SIM )
  else if(MNCNM_IS_SINGLE_STACK)
  {
    MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", message_ptr->as_id,mn_as_id); 
  }
#endif
}


/*
 * Function name :  MN_handle_cm_mt_call_res
 * -------------
 *
 * Description :  Process MT call response from MMI
 * -----------     
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
VOID_FUNC MN_handle_cm_mt_call_res(IMH_T *message_header)
{

   cm_mt_call_res_T *message_ptr;
   ie_cm_cc_cause_T cm_cc_cause;
   mn_call_information_T *data_ptr;
   volatile connection_id_T  connection_id = 8;
   byte reject_cause =  mn_get_mt_call_reject_cause();

   if(message_header != NULL)
   {
       message_ptr = (cm_mt_call_res_T *)message_header;
#if defined(FEATURE_DUAL_SIM )
       if((MNCNM_IS_SINGLE_STACK == FALSE) || (message_ptr->as_id == mn_as_id))
#endif
       {
         connection_id = message_ptr->connection_id;

         if(message_ptr->cm_accept == FALSE)
         {
           cm_cc_cause.coding_standard = STD_CODESTANDARD;
           cm_cc_cause.location = STD_LOCATION;
           cm_cc_cause.recommendation_present = FALSE;
           MSG_HIGH_DS_2(MN_SUB,"=MNCNM= MN_handle_cm_mt_call_res, EFS = %d, cause_present = %d", nas_is_qmss_enabled, message_ptr->cm_cc_cause.present);
           if((nas_is_qmss_enabled == TRUE) && (message_ptr->cm_cc_cause.present == TRUE))
           {
               cm_cc_cause.cause_value = message_ptr->cm_cc_cause.cause_value;
               cm_cc_cause.diagnostic_length = 0;
           }
           else
           {
               cm_cc_cause.cause_value = reject_cause;    
               cm_cc_cause.diagnostic_length = 1;
               cm_cc_cause.diagnostics[0] = 0x80; /* Unknown */
           }

           MN_put_MNCC_DISC_REQ(message_ptr->connection_id, &cm_cc_cause, NULL);
         }
         else
         {
           if(connection_id < MAXNO_CC_TRANSACTIONS)
           {
              mn_call_information_T   *call_data_ptr, *active_call_data_ptr;
              connection_id_T active_connection_id = UNKNOWN_CONNECTION_ID;
              call_data_ptr =  mn_call_information_p[message_ptr->connection_id];

              /* any active calls */
              if(Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_ACTIVE)
              {
                  active_connection_id = Multicall_Status[MN_ACTIVE_HOLD_SESSION].connection_id[0];

                  active_call_data_ptr =  mn_call_information_p[active_connection_id];
              }
              else if(Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_ACTIVE)
              {
                  active_connection_id = Multicall_Status[MN_HOLD_ACTIVE_SESSION].connection_id[0];

                  active_call_data_ptr =  mn_call_information_p[active_connection_id];
              }
              else if ((Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING) &&
                       (Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0] != message_ptr->connection_id))
              {
                  active_connection_id = Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0];

                  active_call_data_ptr =  mn_call_information_p[active_connection_id];
              }
              else
              {
                  active_call_data_ptr = NULL;
              }

              /* either active or incoming call is not speech, which is not multi-party speech call */
              if((active_call_data_ptr != NULL ) &&
                 (cc_transaction_information_p[active_connection_id] != NULL) &&
                 (cc_transaction_information_p[active_connection_id]->cc_state == CC_STATE_U10) &&
                 (((call_data_ptr->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH) && 
                   (call_data_ptr->bearer_capability_1.information_transfer_capability != ITC_SPEECH)) ||
                  ((active_call_data_ptr->bearer_capability_1.information_transfer_capability != ITC_AUX_SPEECH) &&
                   (active_call_data_ptr->bearer_capability_1.information_transfer_capability != ITC_SPEECH))))
              {
                  /* clear all active calls */
                  MN_process_cm_mo_end_call_req();

                  /* defer CALL CONFIRM until active calls had been cleared */
                  if(((active_call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
                      (active_call_data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)))
                  {
                      call_data_ptr->dual_services = DUAL_SERVICE_IN_VOICE;  /* call waiting */
                  }
                  else
                  {
                      call_data_ptr->dual_services = DUAL_SERVICE_IN_DATA;   
                  }
              }
              else if( (active_call_data_ptr != NULL) &&
                       (cc_transaction_information_p[active_connection_id] == NULL ||
                        cc_transaction_information_p[active_connection_id]->cc_state != CC_STATE_U10) )
              {
                  /* Reject the MT call */
                  //MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Can't accept incoming call while active call= %d is not connect", message_ptr->connection_id, 0, 0);
                  /* Presently above F3 is a misnomer. Below F3 applies.
                   * Need to check this behavior when this scenario is struck. */
                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Disconnecting previously active call, connection id = %d ",active_connection_id);

                  /* patch it up with default value */
                  cm_cc_cause.present = TRUE;
                  cm_cc_cause.coding_standard = 0x60;
                  cm_cc_cause.location = 0x00;
                  cm_cc_cause.recommendation_present = FALSE;
                  cm_cc_cause.cause_value = 0x58;  /* #88 */
                  cm_cc_cause.diagnostic_length = 0;

                  MN_put_MNCC_DISC_REQ(active_connection_id, &(cm_cc_cause), NULL);  /* cause MM connection ABORT */

                  mn_call_information_p[active_connection_id]->mo_call_clear_pending = TRUE;
              }
              else 
              {
                  if(mn_call_information_p[connection_id] != NULL)
                  {
                    mn_call_information_p[connection_id]->mt_call_accept_cause = CM_MT_CALL_RES;

                    MN_put_MNCC_SETUP_RES(connection_id, &(message_ptr->cm_connected_subaddress));

#ifdef FEATURE_ENHANCED_AMR
                     /* Reset eAMR sampling rate when user answers MT call and no other ACTIVE call exists */
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
                    data_ptr =  mn_call_information_p[connection_id];

                    /*  IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
                    if(
#ifdef FEATURE_ALS
                       (data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                       (data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                      )
                    {
                        MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                    }
                  }
              }
          }  /* connection ID */
          else
          {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid Connection ID = %d", connection_id);
          }
       }
    }
#if defined(FEATURE_DUAL_SIM )
    else if(MNCNM_IS_SINGLE_STACK)
    {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n",message_ptr->as_id,mn_as_id); 
    }
#endif

   } /* message_header */
}

#ifdef FEATURE_CCBS

/*
 * Function name :  MN_handle_cm_recall_rsp
 * -------------
 *
 * Description :  Process MT recall response from MMI
 * -----------     
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
VOID_FUNC MN_handle_cm_recall_rsp(IMH_T *message_header)
{
  cm_recall_rsp_T      *msg=(cm_recall_rsp_T*)message_header;
  connection_id_T       cid;
  cm_recall_rsp_e_type  resp;
  boolean               msg_ok=TRUE, send_error=TRUE;

  cc_transaction_information_T  *cc_info;
#if defined(FEATURE_DUAL_SIM )
  if((MNCNM_IS_SINGLE_STACK == FALSE) || (msg->as_id == mn_as_id))
#endif  
  {
    cid     =  msg->connection_id;
    resp    =  msg->response;
    cc_info = cc_transaction_information_p[cid];

    if(cc_info != NULL)
    {
      MSG_HIGH_DS_3(MN_SUB,"=MNCNM= [ccbs]Recall res %d,%d,%d",cid,cc_info->cc_state,resp);
    }
    else
    {
      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= [ccbs]Recall res %d,%d",cid,resp);
    }

    /* ******************************************************* */
    /*  Check error first                                      */
    /* ******************************************************* */
    if ( cid               >=  UNKNOWN_CONNECTION_ID &&  /* recall is only for MO calls */
       (cc_info == NULL ||
        cc_info->cc_state != CC_STATE_U0_6) )
    {
      msg_ok = FALSE;
    }

    if ( resp <= CCBS_LOW || resp >= CCBS_MAX )
    {
      msg_ok = FALSE;
    }

    /* ******************************************************* */
    /*  Send messages accordingly                              */
    /* ******************************************************* */
    if (msg_ok)
    {
      switch (resp)
      {
        case CCBS_REL_ACCEPT:
            {
               ie_cm_cc_cause_T    cause;
               int                 Multicall_Status_index;
               int                 index;
               boolean             active_calls_exist = FALSE;
               connection_id_T     disc_cid;

               MN_MULTICALL_LOG(MN_read_multicall_status()); /* refresh Multicall_Status */

               /* set up the cause data */
               cause.location               = STD_LOCATION;
               cause.coding_standard        = STD_CODESTANDARD;
               cause.recommendation_present = FALSE;
               cause.cause_value            = NORMAL_CALL_CLEARING ;
               cause.diagnostic_length      = 0;

               for (Multicall_Status_index = 0; Multicall_Status_index < 2; Multicall_Status_index++)
               {
                  /* close down these calls */
                  if (Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE)
                  {
                     for (index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
                     {
                        disc_cid = Multicall_Status[Multicall_Status_index].connection_id[index];

                        if (disc_cid < MAXNO_CC_TRANSACTIONS)
                        {
                           active_calls_exist = TRUE;

                           MN_stop_cc_timer(disc_cid);

                           MN_put_MNCC_DISC_REQ(disc_cid,&(cause), NULL);

                           mn_call_information_p[disc_cid]->mo_call_clear_pending = TRUE;
                           mn_call_information_p[disc_cid]->mo_release_indicator  = CM_RECALL_REL;
                        }
                     }
                  }
               }

               if (active_calls_exist)
               {
                  send_error = FALSE;
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= [ccbs]Release_Accepting recall");
                  break;
               }
               else
               {
                  /* fall through on purpose */
                  /* - user wanted to release and accept, but since there is no call to */
                  /*   release, we will just accept the recall */
                  MSG_HIGH_DS_0(MN_SUB,"=MNCNM= [ccbs]moving to CCBS_ACCEPT");
               }
            }

         /* create a fake setup message and send it to myself (MN) */
         case CCBS_ACCEPT:
         case CCBS_HOLD_ACCEPT:
            {
               cm_mo_normal_call_req_T  setup;
               gs_status_T              GSstatus;

               memset (&setup, 0x00, sizeof(cm_mo_normal_call_req_T));

               /* Set up the message header */
               setup.message_header.message_set = MS_MN_CM;
               setup.message_header.message_id  = CM_MO_NORMAL_CALL_REQ;
               PUT_IMH_LEN(sizeof(cm_mo_normal_call_req_T)-sizeof(IMH_T), &setup.message_header);

               MN_fill_setup_from_mn (&setup, cid);
#if defined(FEATURE_DUAL_SIM)
               if(MNCNM_IS_SINGLE_STACK)
               {
                 setup.as_id  =  msg->as_id;
               }
#if defined(FEATURE_DUAL_ACTIVE)
               else
               {
                 setup.as_id  =  (sys_modem_as_id_e_type)mn_dsda_as_id;
               }
#endif
#endif

               /* Send the message */
               GSstatus = MN_send_message(GS_QUEUE_MN_CM, &setup, TRUE);

               /* log any failures */
               if(GSstatus == GS_SUCCESS)
               {
                  send_error = FALSE;
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= [ccbs]Recall response failed");
               }
            }
            break;

         case CCBS_REJECT:
            MN_put_MNCC_REL_COMPL_REQ(cid, 0x03/*PD_CC*/, 21/*call rejected*/, NULL);

            /* Reset all information relating the this state */
            MN_clear_call_data(cid);

            /* Refresh the Multicall Session per above state changes */
            MN_MULTICALL_LOG(MN_read_multicall_status());

            /* If this is the last connection, remove the vocoder connection */
            if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                  (Multicall_Status[1].call_activity == CALL_INACTIVE) &&
                  (Multicall_Status[2].call_activity == CALL_INACTIVE))
            {
#if defined(FEATURE_DUAL_SIM )
               if(MNCNM_IS_SINGLE_STACK)
               {
                 MN_disconnect_TCH_with_AS_ID(MN_VOCODER_OFF,msg->as_id);
               }
               else
#endif
               {
                 MN_disconnect_TCH(MN_VOCODER_OFF);
               }

               /* For both GSM & UMTS, Use DEFAULT_RAB_ID below. Will change when we 
                            * implement the Multicall Feature (Not the Multicall SS)  */
               tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
            }

            send_error = FALSE;
            break;
         default:
            break;
      }
    }

    if (send_error)
    {
      /* do nothing */
      if(cc_info != NULL)
      {
        MSG_ERROR_DS_3(MN_SUB,"=MNCNM= [ccbs]Recall res err %d,%d,%d",cid,cc_info->cc_state,resp);
      }
      else
      {
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= [ccbs]Recall res err %d,%d\n",cid,resp);
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= [ccbs]cc_info is NULL\n");
      }
    }
#if defined(FEATURE_DUAL_SIM )
    else if(MNCNM_IS_SINGLE_STACK)
    {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", msg->as_id,mn_as_id); 
    }
#endif
  }
}
#endif /* FEATURE_CCBS */


/*
 *   Function name : MN_handle_cm_asid_2_vsid_mapping_ind
 *   ----------------------------------------------
 *
 *   Description:
 *   ------------
 *  Message from CM to inform NAS about asid to vsid mapping *
 *
 *
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

#if defined (FEATURE_DUAL_SIM) 
VOID_FUNC MN_handle_cm_asid_2_vsid_mapping_ind(IMH_T *message)
{

  cm_asid_2_vsid_mapping_ind_T *msg = (cm_asid_2_vsid_mapping_ind_T *)message;
  sys_modem_as_id_e_type local_as_id;

  /* Validate the data from CM 
     * Explore how to do this.
     */
  
  for(local_as_id = SYS_MODEM_AS_ID_1; local_as_id < MAX_AS_IDS; local_as_id++)
  {
    MSG_HIGH_DS_2(MN_SUB,"=MNCNM= VS Value = %d for subs = %d",msg->asid_to_vsid_map[local_as_id], local_as_id);
    mn_asid_2_vsid_map[local_as_id] = msg->asid_to_vsid_map[local_as_id];
  }
  
}
#endif



/*
 *   Function name : MN_handle_cm_transfer_srvcc_call_context_res
 *   ----------------------------------------------
 *
 *   Description:
 *   ------------
 *  Message from CM to inform NAS about call contexts transferred in SRVCC
 *
 *
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_handle_cm_transfer_srvcc_call_context_res(IMH_T *message_header)
{

  cm_srvcc_call_context_T *message = (cm_srvcc_call_context_T *)message_header;
  byte local_call_cntr;
  byte local_conn_id;

  /* Following Variables are used for error checking only*/
  /* Please note that only one of these variables can be TRUE */
  /* For following variables, 255 mean variable not initialized, valid are 0,1*/
  byte is_active_call_MPTY   =  MN_INVALID_CALL_SESSION; 
  byte is_held_call_MPTY     =  MN_INVALID_CALL_SESSION; 
  byte is_ret_req_for_MPTY   =  MN_INVALID_CALL_SESSION; 
  byte is_hold_req_for_MPTY  =  MN_INVALID_CALL_SESSION; 

  /* It is a Boolean variable with only two states 0 and 1*/
  byte mt_alerting_call_state = MN_INVALID_CALL_SESSION;
  byte mt_alerting_call_tid   = MN_INVALID_CALL_SESSION;
  byte mo_alerting_call_present = FALSE; 
  byte mo_alerting_call_tid   = MN_INVALID_CALL_SESSION;
  byte mo_pre_alerting_call_present = FALSE;  /* This variable is used to check whether the call in U3 state or not */
  byte mo_pre_alerting_call_tid   = MN_INVALID_CALL_SESSION;
  
  byte call_transfer_success =  TRUE; 

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (message->as_id == mn_as_id))
#endif
   {
      if((message->num_calls > 0) && (message->num_calls <= MAX_NUM_OF_SRVCC_CALLS)) /* Validate the # of calls being transferred in SRVCC HO */
      {
         /* Iterate over the info given by CM and store it in NAS globals */
         for(local_call_cntr = 0; ( (local_call_cntr < message->num_calls) && (call_transfer_success == TRUE)); local_call_cntr++) 
         {
             local_conn_id =  message->individual_call_context[local_call_cntr].srvcc_call_connection_id; /* Store this in local to increase readability */
     
             if( MN_reserve_call_object(local_conn_id) == TRUE)
             {
      
                /* Store the info given by CM in NAS call object */
                cc_transaction_information_p[local_conn_id]->cc_state = message->individual_call_context[local_call_cntr].srvcc_call_cc_state;
                cc_transaction_information_p[local_conn_id]->hold_auxiliary_state = message->individual_call_context[local_call_cntr].srvcc_call_hold_aux_state;
                cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state = message->individual_call_context[local_call_cntr].srvcc_call_mpty_aux_state;

#ifdef FEATURE_DUAL_SIM
                if(MNCNM_IS_SINGLE_STACK == TRUE)
                {
                  mn_call_information_p[local_conn_id]->as_id = mn_as_id;
                }
#endif

                /* To check if the early_media is already present or not */
                if((cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U4 ||
                    cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U3 )&& 
                    message->individual_call_context[local_call_cntr].srvcc_ps_early_media_type == CM_NAS_SRVCC_ALERT_MEDIA_NW)
                {
                  mn_call_information_p[local_conn_id]->progress_indicator_present = TRUE;
                  mn_call_information_p[local_conn_id]->progress_indicator.present = TRUE;
                  mn_call_information_p[local_conn_id]->progress_indicator.progress_description = 1; /* Assign a value for which NAS aquires early vocoder */
                }
    	    
    
                MSG_HIGH_DS_3(MN_SUB,"=MNCNM= Call state given by CM, cc_state = %d, hold_state = %d, mpty_state = %d",
                           cc_transaction_information_p[local_conn_id]->cc_state,
                           cc_transaction_information_p[local_conn_id]->hold_auxiliary_state,
                           cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state);
                
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Early Media Type given by CM, early_media = %d",
                           message->individual_call_context[local_call_cntr].srvcc_ps_early_media_type);
    
                if((local_conn_id > UNKNOWN_CONNECTION_ID) && (local_conn_id < MAXNO_CC_TRANSACTIONS)) /* Process calls with MT TIDs */
                {
                    /* Validate the data given by CM */
    
                    if(cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U10)
                    {
    
                       /*  If current call is in HELD state */
                       /*  AND, this is first HELD call encountered */
                       /*  THEN, Mark HELD call's MPTY status - whether HELD call is in MPTY, MPTY_REQ or MPTY_IDLE state*/
                       if((cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_CALL_HELD)&&
                          (is_held_call_MPTY == MN_INVALID_CALL_SESSION))
                       {
                         if(cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_CALL_IN_MPTY)
                         {
                           is_held_call_MPTY = TRUE;
                         }
                         else
                         {
                           is_held_call_MPTY = FALSE;
                         }
                       }
    
                       /* If current call is in HELD state */
                       /* THEN check that MPTY status of current call matches with previous stored MPTY status of HELD call */
                       /* All HELD calls should have same MPTY status */
                       else if(cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_CALL_HELD)
                       {
                         if( ((is_held_call_MPTY == TRUE) && (cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_IDLE)) ||
                             (is_held_call_MPTY == FALSE) )
                         {
                           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error1 in CM data: 2 Held Calls");
                           call_transfer_success = FALSE;
                         }
                       }
    
                       /*  If current call is in ACTIVE state */
                       /*  AND, this is first ACTIVE call encountered*/
                       /*  THEN, Mark ACTIVE call's MPTY status - whether ACTIVE call is in MPTY, MPTY_REQ or MPTY_IDLE state*/
                       else if((cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_IDLE)&&
                          (is_active_call_MPTY == MN_INVALID_CALL_SESSION))
                       {
                          if(cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_CALL_IN_MPTY)
                          {
                            is_active_call_MPTY = TRUE;
                          }
                          else
                          {
                            is_active_call_MPTY = FALSE;
                          }
                       }
    
                       /* If current call is in ACTIVE state */
                       /* THEN check that MPTY status of current call matches with previous stored MPTY status of ACTIVE call */
                       /* All ACTIVE calls should have same MPTY status */
                       else if(cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_IDLE)
                       {
                         if( ((is_active_call_MPTY == TRUE) && (cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_IDLE)) ||
                             (is_active_call_MPTY == FALSE) )
                         {
                           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error2 in CM data: 2 Active Calls");
                           call_transfer_success = FALSE;
                         }
                       }
    
                       /*  If current call is in HOLD_REQ state */
                       /*  AND, this is first call encountered in this state */
                       /*  THEN, Mark MPTY status for call of type HOLD_REQ_SENT - whether HELD call is in MPTY, MPTY_REQ or MPTY_IDLE state*/
                       else if((cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_REQUEST)&&
                          (is_hold_req_for_MPTY == MN_INVALID_CALL_SESSION))
                       {
                       
                         cc_transaction_information_p[local_conn_id]->hold_auxiliary_state = HOLD_IDLE;
      
                         if(cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_CALL_IN_MPTY)
                         {
                            is_hold_req_for_MPTY = TRUE;
                         }
                         else
                         {
                            is_hold_req_for_MPTY = FALSE;
                         }
                       }
                       
                       /* If current call is in HOLD_REQ state */
                       /* AND, this is not first call in this state */
                       /* THEN check that MPTY status of current call matches with previous stored MPTY status of HOLD_REQ call */
                       /* All HOLD_REQ calls should have same MPTY status */
                       else if(cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_REQUEST)
                       {
                         if( ((is_hold_req_for_MPTY == TRUE) && (cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_IDLE)) ||
                             (is_hold_req_for_MPTY == FALSE) )
                         {
                            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error3 in CM data: 2 Hold REQs for calls not in MPTY");
                            call_transfer_success = FALSE;
                         }
                         else
                         {
                           cc_transaction_information_p[local_conn_id]->hold_auxiliary_state = HOLD_IDLE;
                         }
                       }
                       
                       /*  If current call is in RETRIEVE_REQ state */
                       /*  AND, this is first call encountered in this state */
                       /*  THEN, Mark MPTY status for call of type RETRIEVE_REQ_SENT - whether HELD call is in MPTY, MPTY_REQ or MPTY_IDLE state*/
                       else if((cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_RETRIEVE_REQUEST)&&
                               (is_ret_req_for_MPTY == MN_INVALID_CALL_SESSION))
                       {
                          cc_transaction_information_p[local_conn_id]->hold_auxiliary_state = HOLD_CALL_HELD;
      
                          if(cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_CALL_IN_MPTY)
                          {
                             is_ret_req_for_MPTY = TRUE;
                          }
                          else
                          {
                             is_ret_req_for_MPTY = FALSE;
                          }
                       }
                      
                       /* If current call is in RETRIEVE_REQ state */
                       /* AND, this is not first call in this state */
                       /* THEN check that MPTY status of current call matches with previous stored MPTY status of RETRIEVE_REQ call */
                       /* All RETRIEVE_REQ calls should have same MPTY status */
                       else if(cc_transaction_information_p[local_conn_id]->hold_auxiliary_state == HOLD_RETRIEVE_REQUEST)
                       {
                         if( ((is_ret_req_for_MPTY == TRUE) && (cc_transaction_information_p[local_conn_id]->mpty_auxiliary_state == MPTY_IDLE)) ||
                         (is_ret_req_for_MPTY == FALSE) )
                         {
                            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error4 in CM data: 2 RET REQs for calls not in MPTY");
                            call_transfer_success = FALSE;
                         }
                         else
                         {
                           cc_transaction_information_p[local_conn_id]->hold_auxiliary_state = HOLD_CALL_HELD;
                         }
                       }
      
                    }
         
                    /* Check if this is MT call in Call received state */
                    else if(cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U7)
                    {
                      mt_alerting_call_state = CC_STATE_U7;
                      mt_alerting_call_tid   = local_conn_id;
                    }
    
                    else if(cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U8)
                    {
                      mt_alerting_call_state = CC_STATE_U8;
                      mt_alerting_call_tid   = local_conn_id;
    
                      /* Store the state as though user hasn't answered MT call */
                      /* because user has to send CONNECT again */
                      cc_transaction_information_p[local_conn_id]->cc_state = CC_STATE_U7;
                    }
    
                    /* Check that current call is MO call whose alerting has started */
                    /* Also check that there is no MT call in alerting state */
                    else if( cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U4 ) 
                    {
                      mo_alerting_call_present = TRUE;
                      mo_alerting_call_tid = local_conn_id;
                    }
    
                    /* check that the current MO call is in U3 state */
                    else if( cc_transaction_information_p[local_conn_id]->cc_state == CC_STATE_U3 ) 
                    {
                      mo_pre_alerting_call_present = TRUE;
                      mo_pre_alerting_call_tid = local_conn_id;
                    }     
                    else
                    {
                       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error5 in CM data: Inappropriate state for call");
                       call_transfer_success = FALSE;
                    }
      
                }
      
                else
                {
                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error7: Invalid connection id = %d recd from CM",local_conn_id); /* !tocheck: Actually this should be treated as failure of SRVCC*/
                  call_transfer_success = FALSE;
                }
      
                if(call_transfer_success == TRUE)
                {
                   /* Check that both ACTIVE and HELD calls can't be in MPTY or MPTY_REQ state or MPTY split REQ state */
                   if((is_held_call_MPTY == TRUE) && (is_active_call_MPTY == TRUE))
                   {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error8 in CM data: Both Held and Active calls in MPTY");
                      call_transfer_success = FALSE;
                   }
                   
                   /* Both type of calls - HOLD_REQ and RETRIEVE_REQ can't be in MPTY */
                   if( (is_hold_req_for_MPTY == TRUE) && (is_ret_req_for_MPTY == TRUE))
                   {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error9 in CM data: RET_REQ and HOLD_REQ both for MPTY call not possible");
                     call_transfer_success = FALSE;
                   }
      
                   if( (mo_alerting_call_present == TRUE) && (mt_alerting_call_state != MN_INVALID_CALL_SESSION) )
                   {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error10 MO and MT calls both alerting");
                      call_transfer_success = FALSE;
                   }
                   
                   /* Either all TIDs should be in HOLD_REQ/RETRIEVE_REQ sent status */
                   /* OR, all TIDs should be in HELD or ACTIVE state */
                   if( ((is_hold_req_for_MPTY != MN_INVALID_CALL_SESSION) || (is_ret_req_for_MPTY != MN_INVALID_CALL_SESSION)) && 
                       ((is_held_call_MPTY != MN_INVALID_CALL_SESSION) || (is_active_call_MPTY != MN_INVALID_CALL_SESSION)))
                   {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error11 in CM data: HOLD/RET REQ should be sent together ");
                     call_transfer_success = FALSE;
                   }
      
                   if( (mo_alerting_call_present == TRUE) || (mo_pre_alerting_call_present == TRUE) || (mt_alerting_call_state != MN_INVALID_CALL_SESSION))
                   {
                     if((is_held_call_MPTY != MN_INVALID_CALL_SESSION) && (is_active_call_MPTY != MN_INVALID_CALL_SESSION))
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error12 Active, HELD and Alerting call present ");
                        call_transfer_success = FALSE;
                     }
                     else if( (is_active_call_MPTY != MN_INVALID_CALL_SESSION) &&
                              ( (mo_alerting_call_present == TRUE) || 
                                (mo_pre_alerting_call_present == TRUE) || 
                                (mt_alerting_call_state == CC_STATE_U8) ) )
                     {
                       
                       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error13 Active call can't be present alongwith MO Alerting call or MT alerting in U8 or MO call in U3 ");
                       call_transfer_success = FALSE;
                     }
                     else if( ( is_hold_req_for_MPTY != MN_INVALID_CALL_SESSION )  &&
                              ( (mo_alerting_call_present == TRUE) || 
                                (mo_pre_alerting_call_present == TRUE) || 
                                (mt_alerting_call_state == CC_STATE_U8) ) )
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error14 MT /MO alerting/MO pre-alerting call  cannot be in U8 when HOLD_CNF is pending ");
                        call_transfer_success = FALSE;
                     }
                     else if(is_ret_req_for_MPTY != MN_INVALID_CALL_SESSION) /* Alerting call can be present along with a call for which HOLD_REQ is sent */
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error15 MO alerting/ MO pre-alerting/ MT alerting call can't be present along with RET_REQ call ");
                        call_transfer_success = FALSE;
                     }
                     else if((mo_pre_alerting_call_present == TRUE) && ((mo_alerting_call_present == TRUE) || (mt_alerting_call_state != MN_INVALID_CALL_SESSION)))  /* Call in U3 state can not be present with MO?MT alerting call*/
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error16 call in U3 state can't be present with MO/MT alerting call ");
                        call_transfer_success = FALSE;
                     }
                   }
      
                }
      
             }
     
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error14; Call object creation failed");
               call_transfer_success = FALSE;
            }
     
         }
    
         if(call_transfer_success == FALSE) /* De-allocate all call objects which were created just now */
         {
            for(local_call_cntr = 0; (local_call_cntr < message->num_calls); local_call_cntr++) 
            {
              local_conn_id =  message->individual_call_context[local_call_cntr].srvcc_call_connection_id; /* Store this in local to increase readability */
              if((local_conn_id > UNKNOWN_CONNECTION_ID) && (local_conn_id < MAXNO_CC_TRANSACTIONS)) /* conn id validity check*/
              {
                 MN_clear_call_data(local_conn_id);
              }
            }
    
         }
         else
         {
#ifdef FEATURE_WCDMA
            mn_store_rab_status();
#endif			
            MN_read_multicall_status(); /* Fill Multicall_Status array*/
    
            if((is_ret_req_for_MPTY != MN_INVALID_CALL_SESSION) || (is_hold_req_for_MPTY != MN_INVALID_CALL_SESSION))
            {
              if(MN_is_lte_deact_in_progress == TRUE)
              {
                CNM_set_buffer_transient_triggers(T_2_SEND, UNKNOWN_CONNECTION_ID);
              }
              else
              {
                cm_n_send_req_T *message_ptr;

                message_ptr = (cm_n_send_req_T *)CNM_alloc(sizeof(cm_n_send_req_T));
			
#ifdef FEATURE_DUAL_SIM
                if(MNCNM_IS_SINGLE_STACK)
                {
                  if(mn_as_id != SYS_MODEM_AS_ID_NONE)
                  {
                    message_ptr->as_id = mn_as_id;
                  }
                  else
                  {
                    MSG_HIGH_MN_DS(MN_SUB,"=MNCNM= Error! mn_as_id is NULL ",0,0,0);                    
                  }
                }
			 
#if defined(FEATURE_DUAL_ACTIVE)
                else
                {
                   message_ptr->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                }
#endif
#endif
                MN_process_cm_2_send(message_ptr);
                CNM_free(message_ptr);
              }
            }
    
            else if(mt_alerting_call_state == CC_STATE_U8) /* Send CONNECT for MT ALERTING Call */
            {
              if(MN_is_lte_deact_in_progress == TRUE)
              {
                CNM_set_buffer_transient_triggers(T_MT_CALL_RES, mt_alerting_call_tid);
              }
              else
              {
              cm_mt_call_res_T  *message_ptr;
              message_ptr = (cm_mt_call_res_T *)CNM_alloc(sizeof(cm_mt_call_res_T));
    
              message_ptr->cm_accept = TRUE;
              message_ptr->connection_id = mt_alerting_call_tid;
#ifdef FEATURE_DUAL_SIM
                if(MNCNM_IS_SINGLE_STACK)
                {
                  if(mn_as_id != SYS_MODEM_AS_ID_NONE)
                  {
                    message_ptr->as_id = mn_as_id;
                  }
                  else
                  {
                    MSG_HIGH_MN_DS(MN_SUB,"=MNCNM= Error! mn_as_id is NULL ",0,0,0);                    
                  }
                }
			 
#if defined(FEATURE_DUAL_ACTIVE)
                else
                {
                   message_ptr->as_id = mn_dsda_as_id;
                }
#endif
#endif
              MN_handle_cm_mt_call_res((IMH_T *)message_ptr);
    
              CNM_free(message_ptr);
              }
            }
            else if(mo_pre_alerting_call_present == TRUE)
            {
              if((mn_call_information_p[mo_pre_alerting_call_tid] != NULL) &&
                 (mn_call_information_p[mo_pre_alerting_call_tid]->progress_indicator_present != TRUE))
              {		
                if(MN_is_lte_deact_in_progress == TRUE)
                {
                  CNM_set_buffer_transient_triggers(T_START_T310, mo_pre_alerting_call_tid);
                }
                else
                {
                CNM_cc_start_timer(TIMER_T310,mo_pre_alerting_call_tid);  /* Start Timer T310 if Early media is not present in U3 call */
                }
              }
            }
    
            /* Acquire vocoder if Access stratum has already informed about CS RAB establishment */
            if((tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE) || /* TCH already been assigned */
               (CNM_mmcc_sync_ind.reason != ((byte)WCDMA_RAB_RELEASED))) /* Active RR/RRC connection */
            {
              if( (is_active_call_MPTY != MN_INVALID_CALL_SESSION) || (is_held_call_MPTY != MN_INVALID_CALL_SESSION) || 
                   (is_ret_req_for_MPTY != MN_INVALID_CALL_SESSION) || (is_hold_req_for_MPTY != MN_INVALID_CALL_SESSION) || 
                   (mo_alerting_call_present && MN_check_for_inband_message(mo_alerting_call_tid)) || 
                   (mo_pre_alerting_call_present && MN_check_for_inband_message(mo_pre_alerting_call_tid)))
              {
                MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Early Media Present in U4 or U3 or call in U10 received ");
                MN_connect_TCH(mn_call_information_p[message->individual_call_context[0].srvcc_call_connection_id]->bearer_capability_1.information_transfer_capability);
              }
            }
            else
            {
               /* This indicates that OTA channel is not connected when SRVCC call context transfer indication is received
                * This variable will be used in CNM to trigger only one SYNC indication  from CNM to MN in multiple calls scenario */
               mn_waiting_for_tch_in_srvcc = TRUE;
            }
         }
      }
      else
      {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Recd invalid num_calls = %d recd in CM_TRANSFER_SRVCC_CALL_CONTEXT_RES",message->num_calls); /* !tocheck: Actually this should be treated as failure of SRVCC*/
        call_transfer_success = FALSE;
      }
    
       MN_put_cm_srvcc_ho_complete_ind(call_transfer_success);
    
       /* invalid num of calls are tranfered during srvcc handover*/
       if(call_transfer_success == FALSE)
       {
    
         MN_put_MNCC_SRVCC_HANDOVER_FAIL_REQ();
       }
   }
#ifdef FEATURE_DUAL_SIM
   else
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", message->as_id,mn_as_id); 
   }
#endif
}
 /* End of MN_handle_cm_transfer_srvcc_call_context_res */




/*
 *   Function name : MN_process_cm_mo_end_call_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *  Handle mobile originated call cleardown *
 *
 *  active calls i.e. active or held calls.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_cm_mo_end_call_req(void)
{
   /* Locals */

   int         Multicall_index,
   index;
   ie_cm_cc_cause_T  cm_cc_cause;
   connection_id_T  connection_id = 0;

   /* refresh  Multicall_Status */
   MN_MULTICALL_LOG(MN_read_multicall_status());

   /* request a disconnect call for each active or held call */
   /* set up the cause data */

   memset((VOID_DATA *)&cm_cc_cause,'\0',sizeof(ie_cm_cc_cause_T));

   cm_cc_cause.location = STD_LOCATION;
   cm_cc_cause.coding_standard = STD_CODESTANDARD;
   cm_cc_cause.recommendation_present = FALSE;
   cm_cc_cause.cause_value = NORMAL_CALL_CLEARING;
   cm_cc_cause.diagnostic_length = 0;


   if ((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
       (Multicall_Status[1].call_activity == CALL_INACTIVE))
   { /* If there are no active calls, release any waiting call */

      if (Multicall_Status[2].call_activity == CALL_WAITING)  /* end a waiting call */
      {

         cm_cc_cause.cause_value = CALL_REJECTED;

         connection_id = Multicall_Status[2].connection_id[0];

         MN_put_MNCC_DISC_REQ(connection_id,
                              &(cm_cc_cause),
                              NULL);

      }
      else
      {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Ended an empty call", connection_id);
      }

      if(mn_call_information_p[connection_id] != NULL)
      {
          mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;

          mn_call_information_p[connection_id]->mo_setup_pending = FALSE;
      }

   }
   else
   {
      /* Release the all active calls in call call sessions */

      for (Multicall_index = 0; Multicall_index < 2; Multicall_index++)
      {
         for ( index = 0; index <  Multicall_Status[Multicall_index].id_index; index++)
         {
            connection_id = 
                Multicall_Status[Multicall_index].connection_id[index];

            if (connection_id < MAXNO_CC_TRANSACTIONS)

            {

               if ((cc_transaction_information_p[connection_id] != NULL) && 
                    (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U12))
               {
#ifdef FEATURE_CCBS
                  MN_put_MNCC_REL_REQ(connection_id, &cm_cc_cause, &cm_cc_cause, 1, NULL);   /* number of causes */
#else
                  MN_put_MNCC_REL_REQ(connection_id, &cm_cc_cause, &cm_cc_cause, 1);   /* number of causes */
#endif /* FEATURE_CCBS */
               }
               else if(mn_call_information_p[connection_id] != NULL)
               {
                 /* stop any SS releated timers */
               
                  MN_stop_cc_timer(connection_id);

                  MN_put_MNCC_DISC_REQ(
                      Multicall_Status[Multicall_index].connection_id[index],
                      &(cm_cc_cause),
                      NULL);

                  mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;

                  mn_call_information_p[connection_id]->mo_setup_pending = FALSE;
               }

            } /* MAXNO_CC_TRANSACTIONS */

         } /* Connection ID in a given Multicall index */

      } /* Multicall index */

   } /* non CALL WAITING */

}


/*
 *   Function name : MN_process_cm_mo_end_x_call_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *     Handle non-standard clearing of a specific call 
 *     that is use to abort 2nd or subsequence MO call.
 *
 *   Parameters:
 *   -----------
 *     In message
 *
 *   Returns:
 *   -------
 *     None.
 *
 */

VOID_FUNC   MN_process_cm_mo_end_x_call_req(cm_n_x_send_req_T *message)
{
    MNCC_REL_CNF_T    cc_call_release;
    ie_cm_cc_cause_T  cm_cc_cause;
    connection_id_T   connection_id;
#if defined(FEATURE_DUAL_SIM )
    if((MNCNM_IS_SINGLE_STACK == FALSE) || ((message != NULL) && (message->as_id == mn_as_id) ))
#endif  
    {
    /* Deference the connection ID */
    if(message)
    {
       connection_id = message->connection_id;
    }
    else
    {
       connection_id = MAXNO_CC_TRANSACTIONS;
    }
   
    /* Release a specific call */
    if ((message != NULL) && 
        (connection_id < MAXNO_CC_TRANSACTIONS) && 
        (cc_transaction_information_p[connection_id] != NULL ) &&
        (mn_call_information_p[connection_id] != NULL))
    {
        byte cause_value =  ((message->cause_value) ? 
                                    message->cause_value : NORMAL_CALL_CLEARING); 
        
        /* request a disconnect call for specific call */
        memset((VOID_DATA *)&cm_cc_cause, NULL, sizeof(ie_cm_cc_cause_T));

        /* Build cause IE for DISCONNECT */
        cm_cc_cause.location = STD_LOCATION;
        cm_cc_cause.coding_standard = STD_CODESTANDARD;
        cm_cc_cause.recommendation_present = FALSE;
        cm_cc_cause.cause_value = cause_value; 
        cm_cc_cause.diagnostic_length = 0;
        
        /* Build Abort & Release message for RELEASE */
        cc_call_release.message_header.message_set = MS_MN_CC;
        cc_call_release.message_header.message_id = MNCC_REL_CNF;
        cc_call_release.message_header.message_len_lsb = 6;
        cc_call_release.message_header.message_len_msb = 0;
        cc_call_release.connection_id = connection_id;
        cc_call_release.cause = cause_value;
        cc_call_release.data[0] = IEI_CAUSE;
        cc_call_release.data[1] = 2;
        cc_call_release.data[2] = 0x80 | STD_CODESTANDARD | STD_LOCATION;
        cc_call_release.data[3] = 0x80 | cause_value;

        if(cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U0)
        {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Aborting an IDLE call %d", connection_id);

            MN_handle_MNCC_REL_CNF(&cc_call_release.message_header);
        }
        else if (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U0_1)
        {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Aborting a MM Connection Est. pending call = %d", connection_id);

            MN_put_MNCC_DISC_REQ(connection_id, &(cm_cc_cause), NULL);  /* cause MM connection ABORT */

            mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
        }
        else if (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U12)
        {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Aborting a clearing pending call = %d", connection_id);

#ifdef FEATURE_CCBS
            MN_put_MNCC_REL_REQ(connection_id, &cm_cc_cause, &cm_cc_cause, 1, NULL);
#else
            MN_put_MNCC_REL_REQ(connection_id, &cm_cc_cause, &cm_cc_cause, 1);  
#endif /* FEATURE_CCBS */
        }
        else
        {

           /* stop any SS releated timers */

            MN_stop_cc_timer(connection_id);

            /* Normal clearing */
            MN_put_MNCC_DISC_REQ(connection_id, &(cm_cc_cause), NULL);
    
            mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
        }
    } /* valid connection Id, MAXNO_CC_TRANSACTIONS */
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Not Processing cm_mo_end_x_call_req due to error in connection id or call object, connection_id = %d", connection_id);
    }
  }  

#if defined(FEATURE_DUAL_SIM )
  else if((MNCNM_IS_SINGLE_STACK)&&(message != NULL))
  {
    MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", message->as_id, mn_as_id); 
  }
#endif
}


/*
 *   Function name : MN_process_cm_start_dtmf_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the cm_start_dtmf_req message from the 
 *  cm to MN.
 *
 * Relay DTMF information to/from MMI
 *
 * DTMF queueing and timing is provided as a service by CC
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_process_cm_start_dtmf_req(IMH_T    *message)

{

   /* Locals */

   cm_start_dtmf_req_T      *pointer;
   boolean                     speech;
   mn_call_information_T       *call_data;
#if defined(FEATURE_DUAL_SIM )
   ie_cm_cc_reject_T        cm_cc_reject;
   cm_cc_reject.present = FALSE;
#endif
   /* align the pointers  */

   pointer = (cm_start_dtmf_req_T *)message;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (pointer->as_id == mn_as_id))
#endif     
   {  /* check that the connection given in the message is for a speech call */

   speech = FALSE;

   call_data =  mn_call_information_p[pointer->connection_id];
   if(call_data != NULL)
   {
     if (call_data->active_bearer_capability == 0)
     {

       if (
#ifdef FEATURE_ALS
           (call_data->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
           (call_data->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
          )
       {
         speech = TRUE;
       }
     }
     else
     {
       if (
#ifdef FEATURE_ALS
           (call_data->bearer_capability_2.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
           (call_data->bearer_capability_2.information_transfer_capability == ITC_SPEECH)
          )
       {
         speech = TRUE;
       }
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process CM_START_DTMF_REQ as mn_call_information_p[%d] is NULL",
                               pointer->connection_id);
   }
   if ((speech == TRUE) && (cc_transaction_information_p[pointer->connection_id] != NULL)
        && (cc_transaction_information_p[pointer->connection_id]->hold_auxiliary_state != HOLD_CALL_HELD)
        && (cc_transaction_information_p[pointer->connection_id]->hold_auxiliary_state != HOLD_RETRIEVE_REQUEST))
   {
      mn_call_information_p[pointer->connection_id]->received_start_dtmf = TRUE;
      MN_put_MNCC_START_DTMF_REQ(pointer->connection_id,
          pointer->dtmf_value);
   }
   else
   {
      MN_put_cm_start_dtmf_conf(pointer->connection_id,
#if defined(FEATURE_DUAL_SIM )
                              cm_cc_reject,
                              pointer->as_id,
#endif
                              FALSE
                                );
    }
  }
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     cm_cc_reject.present = TRUE;
     cm_cc_reject.rej_type = AS_REJECT_CAUSE;
     cm_cc_reject.rej_value = AS_REJ_ABORT_RADIO_UNAVAILABLE;

     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 
     MN_put_cm_start_dtmf_conf(pointer->connection_id, cm_cc_reject, pointer->as_id, FALSE);  
   }
#endif
}


/*
 *   Function name : MN_process_cm_stop_dtmf_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the cm_stop_dtmf_req message from the 
 *  cm to MN.
 *
 * Relay DTMF information to/from MMI
 *
 * DTMF queueing and timing is provided as a service by CC
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_process_cm_stop_dtmf_req(IMH_T    *message)

{

   /* Locals */

   cm_stop_dtmf_req_T      *pointer;
#if defined(FEATURE_DUAL_SIM )
   ie_cm_cc_reject_T       cm_cc_reject;
   cm_cc_reject.present = FALSE;
#endif
   /* align the pointers  */

   pointer = (cm_stop_dtmf_req_T *)message;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (pointer->as_id == mn_as_id))
#endif
   {
   /* Accept STOP_DTMF_REQ only if the corresponding START_DTMF has been accepted */ 
   if(mn_call_information_p[pointer->connection_id] != NULL)
   {
     if(TRUE == mn_call_information_p[pointer->connection_id]->received_start_dtmf)
     {
        MN_put_MNCC_STOP_DTMF_REQ(pointer->connection_id);
        mn_call_information_p[pointer->connection_id]->received_start_dtmf = FALSE;
        if((cc_transaction_information_p[pointer->connection_id] != NULL) 
            && ((cc_transaction_information_p[pointer->connection_id]->hold_auxiliary_state == HOLD_CALL_HELD)
            || (cc_transaction_information_p[pointer->connection_id]->hold_auxiliary_state == HOLD_RETRIEVE_REQUEST)))
        {
            MN_put_cm_stop_dtmf_conf(pointer->connection_id
#if defined(FEATURE_DUAL_SIM )
                              ,cm_cc_reject 
                              ,mn_call_information_p[pointer->connection_id]->as_id
#endif
                                   );      

        }
     }
     else
     {
         MN_put_cm_stop_dtmf_conf(pointer->connection_id
#if defined(FEATURE_DUAL_SIM )
                                  ,cm_cc_reject 
                                  ,mn_call_information_p[pointer->connection_id]->as_id
#endif
                                   ); 
     }
   }
   else
   {
     MN_put_cm_stop_dtmf_conf(pointer->connection_id
#if defined(FEATURE_DUAL_SIM ) 
                                ,cm_cc_reject 
                                ,pointer->as_id
#endif
                                 );
   }
   }
#if defined(FEATURE_DUAL_SIM ) 
   else if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 
     cm_cc_reject.present = TRUE;
     cm_cc_reject.rej_type = AS_REJECT_CAUSE;
     cm_cc_reject.rej_value = AS_REJ_ABORT_RADIO_UNAVAILABLE;

     MN_put_cm_stop_dtmf_conf(pointer->connection_id, cm_cc_reject, pointer->as_id);
   }
#endif
}


/*
 *  Function name : MN_read_multicall_status()
 *  ---------------
 *
 *  Description:
 *  ------------
 *  This function update the Multicall_Status[3] global structure, 
 *  whose index contains information of the following three possible 
 *  CALL SESSION:
 *
 *      0 - Call ACTIVE or HELD,
 *      1 - Call HELD or ACTIVE, and
 *      2 - Call WAITING
 *   
 *  The above call session are updated according to each transation/
 *  connection identifier, whose call states and call information are 
 *  stored in both the cc_transaction_information[], and 
 *  mn_call_information[] global data structure.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  -------
 *  boolean too_many_calls: TRUE, in valid call session.
 *
 */
boolean MN_read_multicall_status(void)
{

   /* Locals */

   int      index, session;
   boolean  data_recorded = FALSE;
   boolean  toomany_calls = FALSE;
   byte     state;
   byte     hold_state;
   byte     mpty_state;

   /* Initialise Multicall_Status, which contains MN_MAXNO_CALL_SESSION */

   for (index =0; index < MN_MAXNO_CALL_SESSION; index++)
   {
      Multicall_Status[index].call_activity = CALL_INACTIVE;
      Multicall_Status[index].call_is_mpty = FALSE;
      Multicall_Status[index].split_mpty = FALSE;
      Multicall_Status[index].id_index = 0;

      for (session = 0; session < MN_MAXNO_CONNECTION_IN_SESSION; session++)
      {
         Multicall_Status[index].connection_id[session] =  MN_INVALID_CALL_SESSION;
      }
   }

   /* Goes through the call data - excepting emergency calls */

   for(index = 0; index < MAXNO_CC_TRANSACTIONS; index++)
   {

      if (index != UNKNOWN_CONNECTION_ID && cc_transaction_information_p[index] != NULL && mn_call_information_p[index] != NULL)  /* reserved connection ID = emergency call */ 
      {
         state = cc_transaction_information_p[index]->cc_state; /* CC call state of given connection ID */
         /* 1. CC State = CC_STATE_U0 : Due to new MO_CALL_REQ or MNCC_REL_IND (cleared a call) */
         /* 2. Call inactive, but a mo call setup could be initiated. */
         if(state == CC_STATE_U0
#ifdef FEATURE_CCBS
             || state == CC_STATE_U0_6 /* user responded to recall */
#endif /* FEATURE_CCBS */
             ) 
         {
            /*  Record an MO call */
            if(mn_call_information_p[index]->mo_setup_pending == TRUE)  /* MO SETUP had been sent */
            {  
               data_recorded = FALSE;
               session = 0;
               
               do  /* find appropriate call session */
               {
                  if (Multicall_Status[session].call_activity == CALL_INACTIVE)  /* new session */
                  {
                     Multicall_Status[session].call_activity = MO_CALL_PENDING;  
                     Multicall_Status[session].
                         connection_id[Multicall_Status[session].id_index++]
                         = (byte)index;
                     data_recorded = TRUE;
                  }

                  session++;

               } while ((data_recorded == FALSE) && (session < 2 ));

               if (data_recorded == FALSE )
               {
                   MSG_ERROR_DS_2(MN_SUB,"=MNCNM= too many MO call pending %d, %d",index,session); 

                   toomany_calls = TRUE;
               }

            } /* MO call = Otherwise is a RELEASED call */

         } /* new call, state == CC_STATE_U0 */
         else
#ifdef FEATURE_CCBS
         if (state != CC_STATE_U0_2 && state != CC_STATE_U0_3 && state != CC_STATE_U0_4 && state != CC_STATE_U0_5)
           /* interim states during recall setup */
#endif /* FEATURE_CCBS */
         {
            /* Determine other  MulticallStatus[].call_activity state:
             *
             *    CALL_WAITING,
             *    CALL_IN_PROGRESS,
             *    CALL_ACTIVE,
             *    CALL_HELD,
             *   
             * and wheter MulticallStatus[].call_is_mpty and 
             * MulticallStatus[].split_mpty flag should be set */
            
            if ((state == CC_STATE_U7)||(state == CC_STATE_U9)||(state==CC_STATE_U6)) /* Record a call WAITING session */
            {

               if (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_INACTIVE)
               {
                  Multicall_Status[MN_CALL_WAITING_SESSION].call_activity = CALL_WAITING;
                  Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[
                     Multicall_Status[MN_CALL_WAITING_SESSION].id_index++] = (byte)index;
               }
               else
               {
                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Too many waiting calls %d", index);

                  toomany_calls = TRUE;

               }
            } /* Record waiting call */
            else
            {
               /* Record a mpty call */

               mpty_state = cc_transaction_information_p[index]->mpty_auxiliary_state;
               hold_state = cc_transaction_information_p[index]->hold_auxiliary_state;

               if ((mpty_state == MPTY_CALL_IN_MPTY) || 
                   (mpty_state == MPTY_SPLIT_REQUEST))  /* is MPTY */
               {

                  /* Check for ATIVE or ACTIVE HELD (CC_STATE_U10, CC_STATE_U26, CC_STATE_U27) or 
                   * DISCONNECT (CC_STATE_U11, CC_STATE_U12) or RELEASE (CC_STATE_U19) state */

                  if ((state == CC_STATE_U10) || (state == CC_STATE_U26) || (state == CC_STATE_U27) ||  
                      (state == CC_STATE_U11) || (state == CC_STATE_U12) || (state == CC_STATE_U19))
                  {
                     data_recorded = FALSE;
                     session = 0;
                     do   
                     {  /* Update this connection ID to an appropriate call SESSION */

                        if (Multicall_Status[session].call_activity == CALL_INACTIVE)  /* new call SESSION */
                        {
                           Multicall_Status[session].call_is_mpty = TRUE;

                           /* check subset of MPTY split request */

                           if (mpty_state == MPTY_SPLIT_REQUEST)
                           {
                               Multicall_Status[session].split_mpty = TRUE;
                           }

                           /* Check for MPTY but also in Held Call */

                           if ((hold_state == HOLD_IDLE)||
                               (hold_state == HOLD_REQUEST))
                           {  /* call not in held state */

                              Multicall_Status[session].call_activity = CALL_ACTIVE;

                              if (mpty_state == MPTY_SPLIT_REQUEST)  /* TBD: should be REMOVE this redudant check */
                              {
                                 Multicall_Status[session].split_mpty = TRUE;
                              }
                           }
                           else 
                           {
                              Multicall_Status[session].call_activity = CALL_HELD;
                           }

                           Multicall_Status[session].connection_id[Multicall_Status[session].id_index++] = (byte)index;

                           data_recorded = TRUE;
                        } 
                        else
                        {  /* not a new call SESSION */ 
                            
                           if (Multicall_Status[session].call_is_mpty == TRUE)
                           {
                              /* Add another call to the party */
                              if ((hold_state == HOLD_RETRIEVE_REQUEST) || 
                                  (hold_state == HOLD_CALL_HELD))
                              {
                                 Multicall_Status[session].call_activity = CALL_HELD;
                              }

                              /* add connection id to list */
                              Multicall_Status[session].connection_id[Multicall_Status[session].id_index++] = (byte)index;

                              data_recorded = TRUE;
                           }
                        }

                        session++;  /* next call SESSION */

                     } while ((data_recorded == FALSE) && (session < 2));

                  } /* call ACTIVE State : CC_STATE_U10, CC_STATE_U11, 12, CC_STATE_U19, CC_STATE_U26, CC_STATE_U27 */
                  else
                  {
                      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid call state in MPTY %d, %d",index, session);
                  }

                  /* Check for non recording of a mpty call */
                  if (data_recorded ==FALSE)
                  {
                      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Too many calls active %d, %d",index, session);

                      toomany_calls = TRUE;
                  }

               } /* mpty_state */
               else
               /* Record a non-mpty call */
               {
                  data_recorded = FALSE;
                  session=0;
                  do
                  {
                     if (Multicall_Status[session].call_activity == CALL_INACTIVE)
                     {

                        if ((hold_state == HOLD_IDLE) ||
                            (hold_state == HOLD_REQUEST)) /* not call held active state */
                        {
                           
                           if ((state == CC_STATE_U10) || (state == CC_STATE_U26) || (state == CC_STATE_U27)) /* call active state */
                           {
                              Multicall_Status[session].call_activity = CALL_ACTIVE;
                           }
                           else
                           {
                              Multicall_Status[session].call_activity = CALL_IN_PROGRESS;
                           }
                        }
                        else
                        {
                           Multicall_Status[session].call_activity = CALL_HELD;
                        }

                        Multicall_Status[session].connection_id[Multicall_Status[session].id_index++] = (byte)index;

                        data_recorded = TRUE;

                     } /* new entry in a call SESSION */

                     session++;

                  } while ((data_recorded == FALSE) && (session < 2));

                  /* Check for non recording of a non-mpty call */

                  if (data_recorded ==FALSE)
                  {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Too many calls active %d session %d",index,session);

                     toomany_calls = TRUE;
                  }

               } /* non-mpty call */

            } /* Record Waiting Call */

         } /* New call = MO_CALL_PENDING */

      } /* index = unknown/invalid connection id */

   } /* MAX_CC_TRANSACTION */

   return(toomany_calls);
}

