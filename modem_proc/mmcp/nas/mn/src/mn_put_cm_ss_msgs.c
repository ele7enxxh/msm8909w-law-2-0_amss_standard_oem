/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_put_cm_ss_msgs.c_v   1.10   10 Jul 2002 14:10:22   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_put_cm_ss_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01    CD      Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA
04/25/01   TS      Missing ; at end of line.  Arm compiler found it.

5/10/01     CD     Removed unnecessary header file includes

5/25/01     CD     Removed cause.h since it was not being used

                   Corrected calls to PUT_IMH_LEN to avoid many lint errors.
                   Removed use of temporary pointers

                   Fixed a few negative indentation lint errors

7/13/01     CD     Replaced all instances of GS_QUEUE_MMI by GS_QUEUE_CM
                   Replaced all instances of MS_MN_CM by MS_MN_CM

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/22/01    TS     Added include cmll.h.

01/22/01    AB     Added MN_put_cm_manage_calls_conf() from mn_put_cc_msgs.c.  

                   Also, Don't not update the Multicall_Status_index[] table 
                   via MN_read_multicall_status(), for on an error or failure
                   case.

01/25/02    CD     Removed IEI and added boolean flag
                   Updated copyright
                   Replaced calls to ERR() by MSG_ERROR()

02/18/02    AB     Set the new success flag for SS Call Independent
                   confirmation function, to prevent UI from 
                   locking up in case UE unable to process the result
                   result parameters received from the network.

04/08/02    AB     Added MN_put_notify_deferred_ind() to deferred the
                   notifySS indication so that the SETUP indication can
                   be sent to CM/UI first, which allows it to
                   create a call object prior to receiving the notification
                   message for given connection id.

04/30/02    AB     Merged the fix in setting of the success flag in SS call 
                   independent message between MN and CM that was made 
                   on 2/18/02 into mainline.  

07/10/02    CD     Replaced include cmll.h by cmwll.h

09/04/2     AB     Added MN_put_cm_manage_calls_x_conf() function, to 
                   to sent CM_MANAGE_CALLS_CNF, with a specific 
                   connection ID, instead of list of ACTIVE call.

04/15/03    sbs    Added support for Location Services related SS messages

04/19/05    HS     Added support for FEATURE_CCBS

06/02/05    AB     Replaced structure assignment w/ memcpy().

06/23/05    HS     Replaced cm_forward_check_ss_ind_T with
                   cm_release_unstructured_ss_ind_T for UTF

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS

11/02/06    NR     Fixed lint errors

12/06/06    NR     Passing byte stream for interrogate SS 
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "ms.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "nasutils.h"

/* ----------------------------------------------------------------------- 
** REMOVED compiler warning from inclusion of memheap.h 
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MEM_DEBUG
#undef FEATURE_MEM_DEBUG
#endif
#include "cmwll.h"

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include <stringl/stringl.h>


/*
 * Function name :  MN_put_interrogate_ss_conf
 * -------------
 *
 * Description :  Builds an cm_interrogate_ss_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  InterrogateSS_Res
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_interrogate_ss_conf(boolean ss_success, byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                        sys_modem_as_id_e_type as_id,
#endif

                                        cm_ss_conf_e_type ss_conf_type,
                                        InterrogateSS_Res_T *InterrogateSS_Res
                                        )
{
   cm_interrogate_ss_conf_T *message;
   int message_length;
   gs_status_T success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_interrogate_ss_conf_T *)gs_alloc(sizeof(cm_interrogate_ss_conf_T));
#else
   message = (cm_interrogate_ss_conf_T *)modem_mem_calloc(1, sizeof(cm_interrogate_ss_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_INTERROGATE_SS_CONF;

      message_length = (sizeof(cm_interrogate_ss_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      message->res_type = ss_conf_type;
      if(InterrogateSS_Res != NULL)
      {
          memscpy(&message->InterrogateSS_Res, sizeof(message->InterrogateSS_Res) ,InterrogateSS_Res, sizeof(InterrogateSS_Res_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = interrogateSS;
          suppsvc_l3_data_length = 0;
      }

      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_interrogate_ss_conf message");
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
/*
 * Function name :  MN_put_lcs_molr_conf
 * -------------
 *
 * Description :  Builds an cm_lcs_molr_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  lcs_molr_res
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_lcs_molr_conf(boolean ss_success, 
                                  byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                  sys_modem_as_id_e_type as_id,
#endif
                                  lcs_molr_res_T *lcs_molr_res)
{
   cm_lcs_molr_conf_T *message;
   int message_length;
   gs_status_T success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_lcs_molr_conf_T *)gs_alloc(sizeof(cm_lcs_molr_conf_T));
#else
   message = (cm_lcs_molr_conf_T *)modem_mem_calloc(1, sizeof(cm_lcs_molr_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (byte)CM_LCS_MOLR_CONF;

      message_length = (sizeof(cm_lcs_molr_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message-> message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      if(lcs_molr_res != NULL)
      {
          memscpy(&message->lcs_molr_res, sizeof(message->lcs_molr_res) ,lcs_molr_res, sizeof(lcs_molr_res_T));
      } 
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = lcs_MOLR;
      }

      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_lcs_molr_conf message");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}

#endif

/*
 * Function name :  MN_put_register_ss_conf
 * -------------
 *
 * Description :  Builds an cm_register_ss_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_return_data
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_register_ss_conf(boolean ss_success, 
                                     byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    sys_modem_as_id_e_type as_id,
#endif
                                     ss_info_T *ss_info)
{
   cm_register_ss_conf_T *message;
   int message_length;
   gs_status_T success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_register_ss_conf_T *)gs_alloc(sizeof(cm_register_ss_conf_T));
#else
   message = (cm_register_ss_conf_T *)modem_mem_calloc(1, sizeof(cm_register_ss_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_REGISTER_SS_CONF;

      message_length = (sizeof(cm_register_ss_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      if(ss_info != NULL)
      {
          memscpy(&message->ss_info, sizeof(message->ss_info) ,ss_info, sizeof(ss_info_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = registerSS;
      }
      
      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_register_ss_conf messag");
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_erase_ss_conf
 * -------------
 *
 * Description :  Builds an cm_erase_ss_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_info
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_erase_ss_conf (boolean ss_success, 
                                   byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                   sys_modem_as_id_e_type as_id,
#endif
                                   ss_info_T *ss_info)
{
   cm_erase_ss_conf_T *message;
   int message_length;
   gs_status_T success;

#ifndef FEATURE_MODEM_HEAP
   message = (cm_erase_ss_conf_T *)gs_alloc(sizeof(cm_erase_ss_conf_T));
#else
   message = (cm_erase_ss_conf_T *)modem_mem_calloc(1, sizeof(cm_erase_ss_conf_T), MODEM_MEM_CLIENT_NAS);
#endif


   if (message != NULL )
   {

      /* Set up message header data */

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_ERASE_SS_CONF;
      message_length = (sizeof(cm_erase_ss_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      if(ss_info != NULL)
      {
          memscpy(&message->ss_info, sizeof(message->ss_info) ,ss_info, sizeof(ss_info_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
         message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = eraseSS;
          suppsvc_l3_data_length = 0;
      }

      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_erase_ss_conf message");
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_activate_ss_conf
 * -------------
 *
 * Description :  Builds an cm_activate_ss_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_return_data
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_activate_ss_conf(boolean ss_success, 
                                     byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                   sys_modem_as_id_e_type as_id,
#endif
                                     ss_info_T *ss_info)
{
   cm_activate_ss_conf_T *message;
   int message_length;
   gs_status_T success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_activate_ss_conf_T *)gs_alloc(sizeof(cm_activate_ss_conf_T));
#else
   message = (cm_activate_ss_conf_T *)modem_mem_calloc(1, sizeof(cm_activate_ss_conf_T), MODEM_MEM_CLIENT_NAS);
#endif
   
   if ( message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_ACTIVATE_SS_CONF;
      message_length = (sizeof(cm_activate_ss_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      if(ss_info != NULL)
      {
          memscpy(&message->ss_info, sizeof(message->ss_info) ,ss_info, sizeof(ss_info_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = activateSS;
          suppsvc_l3_data_length = 0;
      }

      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_activate_ss_conf message");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_deactivate_ss_conf
 * -------------
 *
 * Description :  Builds an cm_deactivate_ss_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_info
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_deactivate_ss_conf(boolean ss_success, 
                                       byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      sys_modem_as_id_e_type as_id,
#endif
                                       ss_info_T *ss_info)
{
   cm_deactivate_ss_conf_T *message;
   int message_length;
   gs_status_T success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_deactivate_ss_conf_T *)gs_alloc(sizeof(cm_deactivate_ss_conf_T));
#else
   message = (cm_deactivate_ss_conf_T *)modem_mem_calloc(1, sizeof(cm_deactivate_ss_conf_T), MODEM_MEM_CLIENT_NAS);
#endif


   if ( message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_DEACTIVATE_SS_CONF;
      message_length = (sizeof(cm_deactivate_ss_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      if(ss_info != NULL)
      {
          memscpy(&message->ss_info, sizeof(message->ss_info) ,ss_info, sizeof(ss_info_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = deactivateSS;
          suppsvc_l3_data_length = 0;
      }

      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_deactivate_ss_conf message");
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_register_password_conf
 * -------------
 *
 * Description :  Builds an cm_register_password_ss_conf message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_password_info
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_register_password_conf(boolean ss_success, 
                                           byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                           sys_modem_as_id_e_type as_id,
#endif
                                           ss_password_info_T *ss_password_info)
{
   cm_register_password_conf_T *message;
   int message_length;
   gs_status_T success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_register_password_conf_T *)gs_alloc(sizeof(cm_register_password_conf_T));
#else
   message = (cm_register_password_conf_T *)modem_mem_calloc(1, sizeof(cm_register_password_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_REGISTER_PASSWORD_CONF;
      message_length = (sizeof(cm_register_password_conf_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      message->success = ss_success;
      message->cm_ss_ref = cm_ss_ref;
      if(ss_password_info != NULL)
      {
          memscpy(&message->info, sizeof(message->info) ,ss_password_info, sizeof(ss_password_info_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }

#endif

#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = registerPassword;
          suppsvc_l3_data_length = 0;
      }

      success = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (success != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_register_password_ss_conf message");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_unstructured_ss_ind
 * -------------
 *
 * Description :  Builds an cm_unstructured_ss_ind message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_return_data
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_unstructured_ss_ind(byte  invoke_id, 
#if defined(FEATURE_DUAL_SIM )
                                     sys_modem_as_id_e_type as_id,
#endif
                                     uss_data_T *uss_data)
{
   cm_unstructured_ss_ind_T   *message;
   gs_status_T                 gs_status;
   int                        message_length;

   /* Build the message for sending to CM */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_unstructured_ss_ind_T *)gs_alloc(sizeof(cm_unstructured_ss_ind_T));
#else
   message = (cm_unstructured_ss_ind_T *)modem_mem_calloc(1, sizeof(cm_unstructured_ss_ind_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_UNSTRUCTURED_SS_IND;

      message_length = (sizeof(cm_unstructured_ss_ind_T) - sizeof(IMH_T));

      PUT_IMH_LEN(message_length, &message->message_header);

      /* Initialise message elements */

      memset(&(message->uss_data),
       NULL, 
      sizeof(uss_data_T));

      /* Load data */

      message->invoke_id = invoke_id;
      memscpy(&message->uss_data, sizeof(message->uss_data) ,uss_data, sizeof(uss_data_T));
      message->uss_data.present = TRUE;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      gs_status = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send message cm_unstructured_ss_ind"); 
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_unstructured_ss_notify_ind
 * -------------
 *
 * Description :  Builds an cm_unstructured_ss_notify_ind message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_return_data
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_unstructured_ss_notify_ind(byte  invoke_id, 
#if defined(FEATURE_DUAL_SIM )
                                               sys_modem_as_id_e_type as_id,
#endif
                                               uss_data_T *uss_data)
{
   cm_unstructured_ss_notify_ind_T  *message;
   gs_status_T                 gs_status;
   int                        message_length;

   /* Build the message for sending to CM */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_unstructured_ss_notify_ind_T *)gs_alloc(sizeof(cm_unstructured_ss_notify_ind_T));
#else
   message = (cm_unstructured_ss_notify_ind_T *)modem_mem_calloc(1, sizeof(cm_unstructured_ss_notify_ind_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_UNSTRUCTURED_SS_NOTIFY_IND;

      message_length = sizeof(cm_unstructured_ss_notify_ind_T) - sizeof(IMH_T);
      PUT_IMH_LEN(message_length, &message->message_header);

      /* Load data */

      message->invoke_id = invoke_id;
      memscpy(&message->uss_data, sizeof(message->uss_data) ,uss_data, sizeof(uss_data_T));
      message->uss_data.present = TRUE;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      gs_status = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send message cm_unstructured_ss_ind");
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
/*
 * Function name :  MN_put_cm_lcs_location_notification_ind
 * -------------
 *
 * Description :  Builds an cm_lcs_location_notification_ind message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_return_data
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_lcs_location_notification_ind(byte  invoke_id, 
#if defined(FEATURE_DUAL_SIM )
                                                  sys_modem_as_id_e_type as_id,
#endif
                                                  location_notification_T *location_notification)
{
   cm_lcs_location_notification_ind_T  *message;
   gs_status_T                 gs_status;
   int                        message_length;

   /* Build the message for sending to CM */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_lcs_location_notification_ind_T *)gs_alloc(sizeof(cm_lcs_location_notification_ind_T));
#else
   message = (cm_lcs_location_notification_ind_T *)modem_mem_calloc(1, sizeof(cm_lcs_location_notification_ind_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (byte)CM_LCS_LOCATION_NOTIFICATION_IND;

      message_length = sizeof(cm_lcs_location_notification_ind_T) - sizeof(IMH_T);
      PUT_IMH_LEN(message_length, &message->message_header);

      /* Load data */

      message->invoke_id = invoke_id;
      memscpy(&message->location_notification, sizeof(message->location_notification) ,location_notification, sizeof(location_notification_T));
      message->location_notification.present = TRUE;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      gs_status = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send message cm_lcs_location_notification_ind");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}

#endif



/*
 * Function name :  MN_put_cm_connection_state_ind
 * -------------
 *
 * Description :    Sends message to CM indicating whether
 * -----------    the connection has been made or not
 *
 *
 *
 *
 *
 *
 * Parameters:    connection_state (boolean)
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_cm_connection_state_ind(
#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type       as_id,
#endif
   boolean connection_state)
{
   cm_connection_state_ind_T *message;
   gs_status_T                return_value;

   /* Build message header */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_connection_state_ind_T *)gs_alloc(sizeof(cm_connection_state_ind_T));
#else
   message = (cm_connection_state_ind_T *)modem_mem_calloc(1, sizeof(cm_connection_state_ind_T), MODEM_MEM_CLIENT_NAS);
#endif

   if (message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_CONNECTION_STATE_IND;
      PUT_IMH_LEN(sizeof(cm_connection_state_ind_T) - sizeof(IMH_T), &message->message_header);

      /* Set the connection_state */

      message->channel_connected = connection_state;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif

      /* Send message */

      return_value = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (return_value != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_connection_state_ind send failed");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_process_unstructured_ss_conf
 * -------------
 *
 * Description :  Sends message to CM confirming
 * -----------    SS transaction
 *
 *
 *
 *
 *
 *
 * Parameters:    connection_state (boolean)
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_cm_process_unstructured_ss_conf(boolean success, 
                                                 byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                sys_modem_as_id_e_type as_id,
#endif
                                                 ss_data_info_T *ss_data_info)
{
   cm_process_unstructured_ss_conf_T *message;
   gs_status_T                        gs_status;

   /* Build the message */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_process_unstructured_ss_conf_T *)gs_alloc(sizeof(cm_process_unstructured_ss_conf_T));
#else
   message = (cm_process_unstructured_ss_conf_T *)modem_mem_calloc(1, sizeof(cm_process_unstructured_ss_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {
      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_PROCESS_UNSTRUCTURED_SS_CONF;

      PUT_IMH_LEN(sizeof(cm_process_unstructured_ss_conf_T) - sizeof(IMH_T), &message->message_header);

      message->success = success;
      message->cm_ss_ref = cm_ss_ref;
      if(ss_data_info != NULL)
      {
          memscpy(&message->ss_data_info, sizeof(message->ss_data_info) ,ss_data_info, sizeof(ss_data_info_T));
      }
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = processUnstructuredSS_Request;
          suppsvc_l3_data_length = 0;
      }

      gs_status = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_process_unstructured_ss_conf"); 
      }
      
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif

   }
   else
   { 
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_process_unstructured_ss_data_conf
 * -------------
 *
 * Description :  Sends message to CM confirming
 * -----------    SS transaction - phase 1
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_UserData
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_cm_process_unstructured_ss_data_conf(boolean success, 
                                                      byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                     sys_modem_as_id_e_type as_id,
#endif
                                                      ss_UserData_info_T *ss_data_info)
{
   cm_process_unstructured_ss_data_conf_T *message;
   gs_status_T                             gs_status;

   /* Build the message */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_process_unstructured_ss_data_conf_T *)gs_alloc(sizeof(cm_process_unstructured_ss_data_conf_T));
#else
   message = (cm_process_unstructured_ss_data_conf_T *)modem_mem_calloc(1, sizeof(cm_process_unstructured_ss_data_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word) CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF;

      PUT_IMH_LEN(sizeof(cm_process_unstructured_ss_data_conf_T) - sizeof(IMH_T), &(message->message_header));
      if(ss_data_info != NULL)
      {
          memscpy(&message->ss_UserData_info, sizeof(message->ss_UserData_info) ,ss_data_info, sizeof(ss_UserData_info_T));
      }
      message->cm_ss_ref = cm_ss_ref;
      message->success = success;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      if(suppsvc_l3_data_length != 0)
      {
          message->suppsvc_result.present = TRUE;
          message->suppsvc_result.size = suppsvc_l3_data_length + 1;
          memscpy(&(message->suppsvc_result.data[1]), sizeof(message->suppsvc_result.data)-1 ,suppsvc_l3_data, suppsvc_l3_data_length );
          message->suppsvc_result.data[0] = processUnstructuredSS_Data;
          suppsvc_l3_data_length = 0;
      }

      gs_status = MN_send_message(GS_QUEUE_CM,
                  message,
                  TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_process_unstructured_ss_data_conf"); 
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {  
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_get_password_ind
 * -------------
 *
 * Description :  Builds an cm_get_password_ind message
 * -----------    and sends it to the CM requesting a
 *                password from the user
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  guidance / ss_operation
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_put_cm_get_password_ind( byte invoke_id,
#if defined(FEATURE_DUAL_SIM )
                                     sys_modem_as_id_e_type as_id,
#endif
                                     byte guidance)
{
   cm_get_password_ind_T *message;
   gs_status_T            gs_status;

#ifndef FEATURE_MODEM_HEAP
   message = (cm_get_password_ind_T *)gs_alloc(sizeof(cm_get_password_ind_T));
#else
   message = (cm_get_password_ind_T *)modem_mem_calloc(1, sizeof(cm_get_password_ind_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( message != NULL )
   {

      /* Build the message */

      message->message_header.message_set = MS_MN_CM;
      message->message_header.message_id = (word)CM_GET_PASSWORD_IND;
      PUT_IMH_LEN(sizeof(cm_get_password_ind_T) - sizeof(IMH_T), &(message->message_header));

      message->invoke_id = invoke_id;
      message->guidance_info = guidance;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        message->as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

#endif
      /* Send message */

      gs_status = MN_send_message(GS_QUEUE_CM, message, TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_get_password_ind");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {   
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_cm_forward_check_ss_ind
 * -------------
 *
 * Description :  Wrapper to build a cm_forward_check_ss_ind message
 * -----------    to the MMI
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
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_cm_forward_check_ss_ind(
#if defined(FEATURE_DUAL_SIM )
            sys_modem_as_id_e_type as_id
#else
            void
#endif
)
{
    MN_put_cm_forward_check_or_release_USSD_ss_ind(
                      CM_FORWARD_CHECK_SS_INDICATION_IND
#if defined(FEATURE_DUAL_SIM )
                      ,as_id
#endif
     );

}




/*
 * Function name :  MN_put_cm_release_unstructured_ss_ind
 * -------------
 *
 * Description :  Wrapper to build a cm_forward_check_ss_ind message
 * -----------    to the MMI
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
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
           sys_modem_as_id_e_type as_id
#else
           void
#endif
)
{
    MN_put_cm_forward_check_or_release_USSD_ss_ind(
                      CM_RELEASE_UNSTRUCTURED_SS_IND
#if defined(FEATURE_DUAL_SIM )
                      ,as_id
#endif
     );
}


/*
 * Function name :  MN_put_cm_release_unstructured_ss_ind
 * -------------
 *
 * Description :  Builds an cm_forward_check_ss_ind 
 *                or cm_forward_check_ss_ind message
 * -----------    and sends it to the MMI
 *
 *
 *
 *
 *
 * Parameters: message_id
 * ----------  
 *
 *
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_cm_forward_check_or_release_USSD_ss_ind(
           byte                  message_id
#if defined(FEATURE_DUAL_SIM )
           ,sys_modem_as_id_e_type as_id
#endif
)  
{
   IMH_T                                 *message = NULL;
   gs_status_T                            gs_status;

   /* Build the message */
#ifndef FEATURE_MODEM_HEAP
   if(message_id == CM_RELEASE_UNSTRUCTURED_SS_IND)
   {
     message = (IMH_T *)gs_alloc(sizeof(cm_release_unstructured_ss_ind_T));
   }
   else if(message_id == CM_FORWARD_CHECK_SS_INDICATION_IND)
   {
     message = (IMH_T *)gs_alloc(sizeof(cm_forward_check_ss_ind_T));
   }
#else
   if(message_id == CM_RELEASE_UNSTRUCTURED_SS_IND)
   {
     message = (IMH_T *)modem_mem_calloc(1, sizeof(cm_release_unstructured_ss_ind_T), MODEM_MEM_CLIENT_NAS);
   }
   else if(message_id == CM_FORWARD_CHECK_SS_INDICATION_IND)
   {
     message = (IMH_T *)modem_mem_calloc(1, sizeof(cm_forward_check_ss_ind_T), MODEM_MEM_CLIENT_NAS);
   }
#endif

   if ( message != NULL )
   {

      message->message_set = MS_MN_CM;
      message->message_id =  message_id;

      if(message_id == CM_RELEASE_UNSTRUCTURED_SS_IND)
      {
        /* Reset the variable as CM_RELEASE_UNSTRUCTURED_SS_IND is now being sent */
        waiting_for_next_network_ussd_trigger = FALSE;

#ifdef FEATURE_DUAL_SIM
        if(MNCNM_IS_SINGLE_STACK)
        {
           ((cm_release_unstructured_ss_ind_T *)message)->as_id = as_id;
           PUT_IMH_LEN(sizeof(((cm_release_unstructured_ss_ind_T *)message)->as_id), message);
        }
#ifdef FEATURE_DUAL_ACTIVE
        else
        {
           ((cm_release_unstructured_ss_ind_T *)message)->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
           PUT_IMH_LEN(sizeof(((cm_release_unstructured_ss_ind_T *)message)->as_id), message);
        }
#endif
#else
        {
           PUT_IMH_LEN(0, message);
        }
#endif

      }
      else
      {
#ifdef FEATURE_DUAL_SIM
        if(MNCNM_IS_SINGLE_STACK)
        {
           ((cm_forward_check_ss_ind_T *)message)->as_id = as_id;
           PUT_IMH_LEN(sizeof(((cm_forward_check_ss_ind_T *)message)->as_id), message);
        }
#ifdef FEATURE_DUAL_ACTIVE
        else
        {
           ((cm_forward_check_ss_ind_T *)message)->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
           PUT_IMH_LEN(sizeof(((cm_forward_check_ss_ind_T *)message)->as_id), message);
        }
#endif
#else
        {
           PUT_IMH_LEN(0, message);
        }
#endif
      }


      /* Send the message */
      gs_status = MN_send_message(GS_QUEUE_CM,
                  message,
                  TRUE);

      if (gs_status != GS_SUCCESS)
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_forward_check_ss_ind");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(message);
#else
      modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif

   }
   else
   {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}



/*
 * Function name :  MN_put_notify_deferred_ind
 * -------------
 *
 * Description :  
 * -----------   
 *   Re-queue the message to allow the SETUP indication message
 *      sent to CM prior to the notifySS message. 
 *
 * Uses :
 * ----
 *
 *
 * Parameters:  
 * ----------   
 *   connection_id
 *   cm_notify_ss_data
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_notify_deferred_ind(
connection_id_T connection_id, 
ie_cm_notify_ss_T  *cm_notify_ss_data)
{

   /* Locals */
   MNCC_NOTIFY_DEFERRED_IND_T      message;
   int                             message_length;
   gs_status_T                     GSstatus;

   /* Set up the message header */
   message.message_header.message_set = MS_MN_CC;
   message.message_header.message_id = MNCC_NOTIFY_DEFERRED_IND;
   message_length = sizeof(MNCC_NOTIFY_DEFERRED_IND_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length, &message.message_header)

   /* Set up the connection id */
   message.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
   message.notification_indicator = cm_notify_ss_data;

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_MN_CM, &message, TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: notify deferred message id = %d", message.connection_id);
   }
}

/*
 * Function name :  MN_process_MNCC_NOTIFY_DEFERRED_IND
 * -------------
 *
 * Description :  
 * -----------  
 *     Now pass the deferred notify SS message to CM.
 *
 * Uses :
 * ----
 *
 *
 * Parameters:  
 * ----------  
 *    IMH_T *message_ptr - Inter-Task message header.
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_process_MNCC_NOTIFY_DEFERRED_IND(IMH_T  *message_ptr)
{
   MNCC_NOTIFY_DEFERRED_IND_T             *message;

   message = (MNCC_NOTIFY_DEFERRED_IND_T *)message_ptr;

   if(message != NULL)
   {
       MN_put_cm_mt_call_ss_ind(message->connection_id,
                                (byte)CM_NOTIFY_SS_IND,
                                message->notification_indicator,
                                NULL);

#ifndef FEATURE_MODEM_HEAP
       gs_free(message->notification_indicator);
#else
       modem_mem_free(message->notification_indicator, MODEM_MEM_CLIENT_NAS);
#endif   
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid deferred notify SS indicator");
   }
}


/*
 * Function name :  MN_put_cm_mt_call_ss_ind
 * -------------
 *
 * Description :  Creates the above named message
 * -----------    message and sends to the GS_QUEUE_CM
 *                queue.
 *
 *
 * Uses :
 * ----
 *
 *
 * Parameters:  connection_id,msg_type,
 * ----------   cm_forward_charge_advice_data,
 *              cm_notify_ss_data
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_mt_call_ss_ind(
connection_id_T                     connection_id,
byte                                msg_type,
ie_cm_notify_ss_T                  *cm_notify_ss_data,
ie_cm_forward_charge_advice_req_T  *cm_forward_charge_advice_data
)
{

   /* Locals */
   cm_mt_call_ss_ind_T             message={{0}};
   int                             message_length;
   gs_status_T                     GSstatus;

   /* Set up the message header */
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MT_CALL_SS_IND;
   message_length = sizeof(cm_mt_call_ss_ind_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header)

   /* Set up the connection id */
   message.connection_id = connection_id;
   message.ind_type = (cm_mt_call_ss_ind_e_type)msg_type;
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     if (mn_call_information_p[connection_id] != NULL)
     {
       message.as_id = mn_call_information_p[connection_id]->as_id;
     }
     else
     {
       message.as_id = mn_as_id;
     }
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }

#endif

#endif
   /* Set up the forward charge advice data */
   if (msg_type == (byte)CM_FWD_AOC_IND)
   {
      memscpy(&message.data.cm_forward_charge_advice, sizeof(message.data.cm_forward_charge_advice) ,cm_forward_charge_advice_data, sizeof(ie_cm_forward_charge_advice_req_T));
      message.data.cm_forward_charge_advice.present = TRUE;
   }

   if (msg_type == (byte)CM_NOTIFY_SS_IND)
   {
      memscpy(&message.data.cm_notify_ss, sizeof(message.data.cm_notify_ss) ,cm_notify_ss_data, sizeof(ie_cm_notify_ss_T));
      message.data.cm_notify_ss.present = TRUE;
   }

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_mt_call_ss_ind message");
   }
}



/*
 * Function name :  MN_put_cm_manage_calls_conf
 * -------------
 *
 * Description :   Creates the cm_manage_calls_conf message
 * -----------     and sends it to the mmi.
 *
 *
 *
 *
 *
 *
 * Parameters:  call_ss_success,
 * ----------   connection_id
 *              cm_ss_error
 *
 *Comments: call_ss_success will be TRUE whenever CM has to report active call list to its clients such as UI.
 *                  It has nothing to do with success or failure of procedure. Active Call List actually contains state of calls.
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_manage_calls_conf(
        boolean call_ss_success, 
#if defined(FEATURE_DUAL_SIM )
        sys_modem_as_id_e_type as_id,
#endif
        ie_cm_ss_error_T cm_ss_error
        )
{
   cm_manage_calls_conf_T   message;
   gs_status_T               GSstatus;
   int                       message_length,index;

   /* Set up the header */
   memset(&message, 0x00, sizeof(cm_manage_calls_conf_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MANAGE_CALLS_CONF;
   message_length = sizeof(cm_manage_calls_conf_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header);
#ifdef FEATURE_DUAL_SIM 
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
   if(call_ss_success == TRUE) 
   {
       /* Set new the active calls list */
       MN_MULTICALL_LOG(MN_read_multicall_status());
   }

   message.active_calls_list.size = 0;

   for (index = 0 ; index< 2; index++)
   {
      if (Multicall_Status[index].call_activity == CALL_ACTIVE )
      {
          memscpy(message.active_calls_list.table, sizeof(message.active_calls_list.table) ,Multicall_Status[index].connection_id,ACTIVE_CALLS_LIST_SIZE);
          message.active_calls_list.size = Multicall_Status[index].id_index;
      }
   }

   /* Set up the success indicator */
   message.call_ss_success = call_ss_success;

   /* Set up the cm ss error */
   if (call_ss_success == TRUE)
   {
      memset(&message.cm_ss_error, NULL, sizeof(ie_cm_ss_error_T));
      message.cm_ss_error.present = FALSE;
   }
   else
   {  /* If SS transaction failed */
      memscpy(&message.cm_ss_error, sizeof(message.cm_ss_error) ,&cm_ss_error,sizeof(ie_cm_ss_error_T));
      message.cm_ss_error.present = TRUE;

   }

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      ERR("ERROR: Failed to send cm_manage_calls_conf message",0,0,0);
   }
}

/*
 * Function name :  MN_put_cm_manage_calls_conf
 * -------------
 *
 * Description :   Creates the cm_manage_calls_conf message
 * -----------     for a specific (x) call and sends it to the CM/UI.
 *
 *
 * Parameters:  call_ss_success,
 * ----------   connection_id
 *              cm_ss_error
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_put_cm_manage_calls_x_conf(connection_id_T connection_id, boolean call_ss_success, ie_cm_ss_error_T cm_ss_error)
{
   cm_manage_calls_conf_T   message;
   gs_status_T               GSstatus;
   int                       message_length;

   /* Set up the header */
   memset(&message, 0x00, sizeof(cm_manage_calls_conf_T)); 
   message.message_header.message_set = MS_MN_CM;
   message.message_header.message_id = (word)CM_MANAGE_CALLS_CONF;
   message_length = sizeof(cm_manage_calls_conf_T) - sizeof(IMH_T);
   PUT_IMH_LEN(message_length,&message.message_header);

#if defined(FEATURE_DUAL_SIM)
   if(MNCNM_IS_SINGLE_STACK)
   {
     /* Check how to fill as_id 
      * Right now this function is not used.
      */
   }
#ifdef FEATURE_DUAL_ACTIVE   
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif   
#endif


   if(call_ss_success == TRUE) 
   {
       /* Set new the active calls list */
       MN_MULTICALL_LOG(MN_read_multicall_status());
   }

   /* fill in connection ID */
   message.active_calls_list.table[0] = connection_id;
   message.active_calls_list.size = 1;

   /* Set up the success indicator */
   message.call_ss_success = call_ss_success;

   /* Set up the cm ss error */
   if (call_ss_success == TRUE)
   {
      memset(&message.cm_ss_error, NULL, sizeof(ie_cm_ss_error_T));
      message.cm_ss_error.present = FALSE;
   }
   else
   {  /* If SS transaction failed */
      memscpy(&message.cm_ss_error, sizeof(message.cm_ss_error) ,&cm_ss_error,sizeof(ie_cm_ss_error_T));
      message.cm_ss_error.present = TRUE;

   }

   /* Send the message */
   GSstatus = MN_send_message(GS_QUEUE_CM,&message,TRUE);

   /* log any failures */
   if(GSstatus != GS_SUCCESS)
   {
      ERR("ERROR: Failed to send cm_manage_calls_conf message",0,0,0);
   }
}

#ifdef FEATURE_CCBS
/*
 * Function name :  MN_put_cm_erase_cc_entry_conf
 * -------------
 *
 * Description :  Builds an cm_erase_cc_entry_conf message
 * -----------    and send it to CM
 *
 * Parameters:
 *
 * Returns: none
 * --------
 */

VOID_FUNC MN_put_cm_erase_cc_entry_conf ( boolean     ss_success,
                                          byte        cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                                       sys_modem_as_id_e_type as_id,
#endif
                                          ss_info_T  *ss_info)
{
  cm_erase_cc_entry_conf_T  *message;
  int                        message_length;
  gs_status_T                success;

  /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   message = (cm_erase_cc_entry_conf_T *)gs_alloc(sizeof(cm_erase_cc_entry_conf_T));
#else
   message = (cm_erase_cc_entry_conf_T *)modem_mem_calloc(1, sizeof(cm_erase_cc_entry_conf_T), MODEM_MEM_CLIENT_NAS);
#endif

  if (message != NULL)
  {
    message->message_header.message_set = MS_MN_CM;
    message->message_header.message_id  = (word)CM_ERASE_CC_ENTRY_CONF;

    message_length = sizeof(cm_erase_cc_entry_conf_T) - sizeof(IMH_T);

    PUT_IMH_LEN(message_length, &message->message_header);

    message->success   = ss_success;
    message->cm_ss_ref = cm_ss_ref;
    if(ss_info != NULL)
    {
      message->ss_info   = *ss_info;
    }
    else
    {
      /* do nothing */
    }
#ifdef FEATURE_DUAL_SIM
    if(MNCNM_IS_SINGLE_STACK)
    {
      message->as_id = as_id;
    }
#ifdef FEATURE_DUAL_ACTIVE
    else
    {
      message->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif

#endif
    success = MN_send_message(GS_QUEUE_CM, message, TRUE);

    if (success != GS_SUCCESS)
    {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_erase_cc_entry_conf message");
    }
#ifndef FEATURE_MODEM_HEAP
    gs_free(message);
#else
    modem_mem_free(message, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
  }
}


/*
 * Function name :  MN_put_cm_5_send_conf
 * -------------
 *
 * Description :  Builds an cm_5_send_conf message
 * -----------    and send it to CM
 *
 * Parameters:
 *
 * Returns: none
 * --------
 */

VOID_FUNC MN_put_cm_5_send_conf (
      connection_id_T           cid,
      cm_5_send_conf_e_type     type,
      ie_ccbs_indicator_T       *res,  /* for CCBS_RET_RES */
#if defined(FEATURE_DUAL_SIM )
      sys_modem_as_id_e_type    as_id,
#endif
      ie_cm_ss_error_T          *err   /* for CCBS_RET_ERR or CCBS_REJ */
)
{
   cm_5_send_conf_T   *msg;
   int                 msg_len;
   gs_status_T         success;

   /* Set up message header data */
#ifndef FEATURE_MODEM_HEAP
   msg = (cm_5_send_conf_T *)gs_alloc(sizeof(cm_5_send_conf_T));
#else
   msg = (cm_5_send_conf_T *)modem_mem_calloc(1, sizeof(cm_5_send_conf_T), MODEM_MEM_CLIENT_NAS);
#endif
   
   if (msg != NULL)
   {
      msg->message_header.message_set = MS_MN_CM;
      msg->message_header.message_id  = (word)CM_5_SEND_CONF;

      msg_len = sizeof(cm_5_send_conf_T) - sizeof(IMH_T);

      PUT_IMH_LEN(msg_len, &msg->message_header);
      
      msg->connection_id   =  cid;
      msg->conf_type       =  type;
#ifdef FEATURE_DUAL_SIM 
      if(MNCNM_IS_SINGLE_STACK)
      {
        msg->as_id       =  as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        msg->as_id     =  (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
      if (res != NULL)
      {
         memscpy (&msg->rsp.ret_res, sizeof(msg->rsp.ret_res) ,res, sizeof(ie_ccbs_indicator_T));
      }
      else if (err != NULL)
      {
         memscpy (&msg->rsp.cm_ss_error, sizeof(msg->rsp.cm_ss_error) ,err, sizeof(ie_cm_ss_error_T));
      }
      else
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= cm_5_send_conf without res or err");
      }

      success = MN_send_message(GS_QUEUE_CM, msg, TRUE);

      if (success != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to send cm_5_send_conf message");
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free(msg);
#else
      modem_mem_free(msg, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   }
}
#endif /* FEATURE_CCBS */

