/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_route_ss_msgs.c_v   1.6   13 May 2002 15:19:14   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_route_ss_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

5/25/01     CD     Removed cause.h and ms.h since it was not being used

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

10/12/01    AB     Fixed Lint errors.

05/06/02    AB     Added handler fos CM_RELEASE_SS_REQ event from CM/UI.

04/15/03    sbs    Added support for Location Services related SS messages

05/10/05    HS     Added support FEATURE_CCBS

===========================================================================*/


#include "mmcp_variation.h"
#include "rex.h"
#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "cnm.h"
#include "nasutils.h"

/*
 *   Function name : MN_route_SS_CM_to_MN_primitives
 *   ---------------
 *
 *   Description:   Route SS primitives from CM to correct function
 *   ------------
 *  
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



VOID_FUNC   MN_route_SS_CM_to_MN_primitives(IMH_T          *message)
{

   void *cm_mo_ss_req = message;


   /* Send incoming primitives to their handling functions */

   switch (message->message_id)
   {

   case CM_REGISTER_SS_REQ:
      {
         MN_handle_cm_register_ss_req((cm_register_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_ERASE_SS_REQ:
      {
         MN_handle_cm_erase_ss_req((cm_erase_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_ACTIVATE_SS_REQ:
      {
         MN_handle_cm_activate_ss_req((cm_activate_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_DEACTIVATE_SS_REQ:
      {
         MN_handle_cm_deactivate_ss_req((cm_deactivate_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_INTERROGATE_SS_REQ:
      {
         MN_handle_cm_interrogate_ss_req((cm_interrogate_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_REGISTER_PASSWORD_REQ:
      {
         MN_handle_cm_register_password_req((cm_register_password_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_PROCESS_UNSTRUCTURED_SS_DATA:
      {
         MN_handle_cm_process_unstructured_ss_data((cm_process_unstructured_ss_data_T *)cm_mo_ss_req);
         break;
      }

   case CM_PROCESS_UNSTRUCTURED_SS_REQ:
      {
         MN_handle_cm_process_unstructured_ss_req((cm_process_unstructured_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_RELEASE_SS_REQ:
      {
         MN_handle_cm_release_ss_req((cm_release_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_ABORT_SS_REQ:
      {
         MN_handle_cm_abort_ss_req((cm_abort_ss_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_UNSTRUCTURED_SS_NOTIFY_RES:
      {
         MN_handle_cm_unstructured_ss_notify_res((cm_unstructured_ss_notify_res_T *)cm_mo_ss_req);
         break;
      }

   case CM_UNSTRUCTURED_SS_RES:
      {
         MN_handle_cm_unstructured_ss_res((cm_unstructured_ss_res_T *)cm_mo_ss_req);
         break;
      }

   case CM_GET_PASSWORD_RES:
      {
         MN_handle_cm_get_password_res((cm_get_password_res_T *)cm_mo_ss_req);
         break;
      }

   case CM_MT_CALL_SS_RES:
      {
         MN_handle_cm_mt_call_ss_res((cm_mt_call_ss_res_T *)cm_mo_ss_req);
         break;
      }

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
   case CM_LCS_MOLR_REQ:
      {
         MN_handle_cm_lcs_molr_req((cm_lcs_molr_req_T *)cm_mo_ss_req);
         break;
      }

   case CM_LCS_MOLR_COMPLETED:
      {
         MN_handle_cm_lcs_molr_completed((cm_lcs_molr_compl_T *)cm_mo_ss_req);
         break;
      }

   case CM_LCS_LOCATION_NOTIFICATION_RES:
      {
         MN_handle_cm_lcs_location_notification_res((cm_lcs_location_notification_res_T *)cm_mo_ss_req);
         break;
      }
#endif

#ifdef FEATURE_CCBS
   case CM_ERASE_CC_ENTRY_REQ:
      {
         MN_handle_cm_erase_cc_entry_req((cm_erase_cc_entry_req_T *)cm_mo_ss_req);
         break;
      }
#endif /* FEATURE_CCBS */

   default:
      {
        ERR("ERROR: Invalid MMI->MN SS message received",0,0,0);

      }
   }
}


/*
 *   Function name : MN_route_SS_to_MN_primitives
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the cm_mo_call_conf message to the 
 *  mmi.
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


VOID_FUNC   MN_route_SS_to_MN_primitives(IMH_T          *message)

{
   void *cm_mo_ss_req = message;

   switch(message->message_id)
   {
   case MNSS_BEGIN_IND:
      {
         MN_get_MNSS_BEGIN_IND((MNSS_BEGIN_IND_T *)cm_mo_ss_req);
         break;
      }

   case MNSS_END_IND:
      {
         MN_get_MNSS_END_IND((MNSS_END_IND_T *)cm_mo_ss_req);
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
         break;
      }

   case MNSS_FACILITY_IND:
      {
         MN_get_MNSS_FACILITY_IND((MNSS_FACILITY_IND_T *)cm_mo_ss_req);
         break;
      }

   case MNSS_REJ_IND:
      {
         MN_get_MNSS_REJ_IND((MNSS_REJ_IND_T *)cm_mo_ss_req);
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

         break;
      }

   default:
      {
          ERR("ERROR: Invalid SS->MN message received",0,0,0);
      }
   }
}
