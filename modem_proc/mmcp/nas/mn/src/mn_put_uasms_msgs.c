/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_put_uasms_msgs.c_v   1.3   25 Oct 2001 09:27:52   cdealy  $   
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_put_uasms_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

5/25/01     CD     Removed cause.h since it was not being used

6/6/01      CD     Corrected typo in error message

6/7/01      CD     Deleted MN_put_mmi_sm_status_report_ind(), MN_put_mmi_mt_sms_ind(),
                   MN_put_mmi_mt_sm_available_ind(), MN_put_mmi_deliver_sm_conf(),
                   MN_put_mmi_submit_sm_conf(), MN_put_mmi_check_for_smma_conf()

                   Added MN_put_uasms_sm_data_ind(), MN_put_uasms_sm_report_ind()

                   Included gs.h and deleted string.h

7/13/01     CD     Replaced all instances of MS_MN_MMI by MS_MN_CM

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

10/8/01     CD     Added printing the report structure for unit test

02/16/07    NR     Renaming the FEATRUE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "gs_v.h"
#include "ms.h"
#include "timers_v.h"
#include "mn.h"
#include "err.h"
#include "msg.h"
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif
#include "mnglobal.h"
#include "nasutils.h"
#include "cnm.h"



/*
 * Function name :  MN_put_uasms_sm_data_ind
 * -------------
 *
 * Description :    Sends a SM data indication to UASMS . 
 * -----------       
 *
 *
 * Parameters:    SM_RL_DATA_IND
 * ----------            It may contain a SMS_DELIVER, or a SMS_STATUS_REPORT TPDU
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_put_uasms_sm_data_ind(SM_RL_DATA_IND_T *SM_RL_DATA_IND)
{
   gs_status_T      gs_return;

   /* Fill in the message header information */
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
   #error code not present
#else
   SM_RL_DATA_IND->message_header.message_set = MS_MN_UASMS;
   SM_RL_DATA_IND->message_header.message_id = UASMS_SM_DATA_IND;
#endif



   PUT_IMH_LEN((sizeof(SM_RL_DATA_IND_T) - sizeof(IMH_T)) , 
       &(SM_RL_DATA_IND->message_header));

   /* Send the message */

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
   #error code not present
#else
   gs_return = MN_send_message(GS_QUEUE_UASMS, SM_RL_DATA_IND, TRUE);
#endif

   if (gs_return != GS_SUCCESS)
   {
       ERR("ERROR: Failed to send SMS data indication to UASMS",0,0,0);

   }

}



/*
 * Function name :  MN_put_uasms_sm_report_ind
 * -------------
 *
 * Description :    Sends SM report indication to UASMS
 * -----------      
 *
 *
 *
 *
 *
 *
 * Parameters:    SM_RL_REPORT_IND
 * ----------          It contains a SMS submit report, which is used 
 *                     for both MO SMS and SMMA transactions
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_put_uasms_sm_report_ind(SM_RL_REPORT_IND_T *SM_RL_REPORT_IND)
{
   gs_status_T          gs_success;

   /* Fill in the message header information*/

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
   #error code not present
#else
   SM_RL_REPORT_IND->message_header.message_set = MS_MN_UASMS;
   SM_RL_REPORT_IND->message_header.message_id  = UASMS_SM_REPORT_IND;
#endif


   PUT_IMH_LEN((sizeof(SM_RL_REPORT_IND_T) - sizeof(IMH_T)) , 
       &(SM_RL_REPORT_IND->message_header));

#ifdef FEATURE_MN_UNIT_TEST
   /* Print out report contents */
   MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMRL_RPT SMI = %d", SM_RL_REPORT_IND->short_message_identifier);
   switch(SM_RL_REPORT_IND->error_status)
   {
   case NO_ERROR_RP_ACK:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = NO_ERROR_RP_ACK");
       break;

   case NO_ERROR_ABORT_SM_COMPLETE:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = NO_ERROR_ABORT_SM_COMPLETE");
       break;

   case NO_ERROR_SMMA_ABORT_SENT:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = NO_ERROR_SMMA_ABORT_SENT");
       break;

   case SMR_ERROR_NO_SM_RESOURCES:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = SMR_ERROR_NO_SM_RESOURCES");
       break;

   case SMR_ERROR_TR1M_TIMEOUT:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = SMR_ERROR_TR1M_TIMEOUT");
       break;

   case SMR_ERROR_TR2M_TIMEOUT:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = SMR_ERROR_TR2M_TIMEOUT");
       break;

   case SMS_RP_ERROR:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = SMS_RP_ERROR");
       break;

   case SMS_CP_ERROR:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = SMS_CP_ERROR");
       break;

   case SMS_LL_ERROR:
       MSG_LOW_DS_0(MN_SUB,"=MNCNM= SMRL_RPT err = SMS_LL_ERROR");
       break;

   default:
       MSG_LOW_DS_1(MN_SUB,"=MNCNM= SMRL_RPT unknown err%d", SM_RL_REPORT_IND->error_status);
   }

   MSG_LOW_DS_3(MN_SUB,"=MNCNM= SMRL_RPT cause=%d diag_pres=%d TPDUlen=%d",
            SM_RL_REPORT_IND->cause_value, 
            SM_RL_REPORT_IND->diagnostics_present,
            SM_RL_REPORT_IND->tpdu_length);

#endif

   /* If no other MO SMS is in progress,
     * then reset transport type, so that next 
     * MO SMS can be handled over any transport type
     * */
   if(MN_is_MO_SMS_in_progress() == FALSE)
   {
   #if defined(FEATURE_DUAL_SIM )
     if(MNCNM_IS_SINGLE_STACK)
     {
       /* Check if no other transaction is going on */
       if(((NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_sms_transport_type) ||
           (NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mt_sms_transport_type))&&
          CNM_check_if_any_service_object_exists() == FALSE) 
       {
         /* store only when FEATURE_3GPP_SMS_OVER_IMS is not defined in Mainline */
         mn_as_id = SYS_MODEM_AS_ID_NONE;
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
       }
     }
   #endif   
     mo_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_MAX;
     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= mo_sms_transport_type is reset to NAS_MSG_TRANSPORT_TYPE_MAX");
   }

   /* Send the message */
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
   #error code not present
#else
   gs_success = MN_send_message(GS_QUEUE_UASMS, SM_RL_REPORT_IND, TRUE);
#endif

   if (gs_success != GS_SUCCESS)
   {
      ERR("ERROR: UASMS SM_REPORT_IND send failed",0,0,0);

   }

}


/*
 * Function name :  MN_put_sms_link_control_report_ind()
 * -------------
 *
 * Description :    Sends SM report indication to UASMS
 * -----------      
 *
 *
 *
 *
 *
 *
 * Parameters:    SM_RL_REPORT_IND
 * ----------          It contains a SMS submit report, which is used 
 *                     for both MO SMS and SMMA transactions
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_put_uasms_link_control_report_ind(
                sms_link_control_mode_T mode, 
#if defined(FEATURE_DUAL_SIM )
                sys_modem_as_id_e_type as_id,
#endif
                sms_link_control_status_T status)
{
    gs_status_T          gs_success;

    SM_RL_LINK_CONTROL_REPORT_T  SM_RL_LINK_CONTROL_REPORT_IND;
    memset(&SM_RL_LINK_CONTROL_REPORT_IND, 0, sizeof(SM_RL_LINK_CONTROL_REPORT_T));

    /* Fill in the message header information*/

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
    #error code not present
#else
    SM_RL_LINK_CONTROL_REPORT_IND.message_header.message_set = MS_MN_UASMS;
    SM_RL_LINK_CONTROL_REPORT_IND.message_header.message_id  = UASMS_SM_LINK_CONTROL_IND;
#endif
    PUT_IMH_LEN((sizeof(SM_RL_LINK_CONTROL_REPORT_T) - sizeof(IMH_T)), 
                &(SM_RL_LINK_CONTROL_REPORT_IND.message_header));

    SM_RL_LINK_CONTROL_REPORT_IND.mode = mode;
    SM_RL_LINK_CONTROL_REPORT_IND.status = status;

#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
     SM_RL_LINK_CONTROL_REPORT_IND.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     SM_RL_LINK_CONTROL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
#endif
    /* Send the message */
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
    #error code not present
#else
    gs_success = MN_send_message(GS_QUEUE_UASMS, &SM_RL_LINK_CONTROL_REPORT_IND, TRUE);
#endif

    if (gs_success != GS_SUCCESS)
    {
       ERR("ERROR: UASMS SM_LINK_CONTROL_REPORT_IND send failed",0,0,0);

    }
}

