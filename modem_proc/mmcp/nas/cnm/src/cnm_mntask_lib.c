/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_mntask_lib.c_v   1.4   12 Mar 2002 13:11:26   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_mntask_lib.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

6/18/01    CD      Added mn_uasms_get_cmd_buf()

7/13/01    CD      Replaced MS_MN_CM by MS_MN_CM in mn_cm_get_cmd_buf

                   Replaced MS_MN_CM by MS_MN_UASMS in mn_uasms_get_cmd_buf

08/14/01   AB      Renamed connection manager references to CNM from CM.

03/12/02   VT      Removed Lint errors.

07/19/02   AB      In mn_cm_get_cmd_buf(), added CM_4_SEND_REQ, CM_4_X_SEND_REQ, and
                   CM_5_SEND_REQ so that CM can allocate an appropriate buffer.

10/12/02   AB      In mn_cm_get_cmd_buf(), added CM_MO_END_X_CALL_REQ for CM 
                   allocate the command buffer.

02/14/03   AB      Removed lint errors.

04/15/03   sbs     Added support for Location Services related SS messages

09/16/03   AB      Added support for closing the LCS session.

05/25/05   HS      Added support for FEATURE_CCBS

03/27/06   NR      Removing  FEATURE_CM_ABORT_X_CALL feature flag from mn_cm_get_cmd_buf()
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "err.h"

#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"

#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"

#include "nasutils.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "mnglobal.h"

extern rex_tcb_type* mn_tcb_ptr;

/*
 * static/global data definitions
 */


/*************** Begin of   mn_cnm_timer_expiry ****************************************
 * Function name :  mn_cnm_timer_expiry()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void mn_cnm_timer_expiry(unsigned long param)
{
   mn_cnm_cmd_type   *cmd_ptr;

   /* Get command buffer from mn_cnm task */
   cmd_ptr = mn_cnm_get_cmd_buf();

   /* Construct message */
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));
   
   cmd_ptr->cmd.timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= @Debug! #Timer_expiry_1, param = %lu, tid = %d, data = %d",param,cmd_ptr->cmd.timer_expiry.timer_id,cmd_ptr->cmd.timer_expiry.data);

   /* Put on MN_CNM command queue */
   mn_cnm_put_cmd(cmd_ptr);
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/

}

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
void mn_cnm_timer_expiry_subs_2(unsigned long param)
{
   mn_cnm_cmd_type   *cmd_ptr;
   timer_id_T                          local_timer_id = MAX_MNCNM_TIMER;

   /* Get command buffer from mn_cnm task */
   cmd_ptr = mn_cnm_get_cmd_buf();

   /* Construct message */
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));
   
   cmd_ptr->cmd.timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);
   local_timer_id = cmd_ptr->cmd.timer_expiry.timer_id;

   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= @Debug!  #MNCNM_Timer_expiry_2, param = %lu, tid = %d, data = %d",param,cmd_ptr->cmd.timer_expiry.timer_id,cmd_ptr->cmd.timer_expiry.data);

   if(cmd_ptr->cmd.timer_expiry.timer_id >= MAX_MNCNM_TIMER)
   {
     MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Value of timer (%d) exceeded max value = %d",cmd_ptr->cmd.timer_expiry.timer_id,MAX_MNCNM_TIMER,0);
   }
   
   if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= @Debug!  # MNCNM_Timer_expiry_2,timerid= %d should not expire in Single stack mode",cmd_ptr->cmd.timer_expiry.timer_id);
   }
   else
   {
     cmd_ptr->cmd.timer_expiry.timer_id = MNCNM_TIMER_TAG_AS_ID(cmd_ptr->cmd.timer_expiry.timer_id, 1);
   }
   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= @Debug! Timer ID = %d after encoding as_id, local_timer_id = %d and mn_dsda_as_id value = %d",cmd_ptr->cmd.timer_expiry.timer_id,local_timer_id,mn_dsda_as_id);
   
   /* Put on MN_CNM command queue */
   mn_cnm_put_cmd(cmd_ptr);
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/

}
#endif


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*************** End of   mn_cnm_timer_expiry **************************************/


/*************** Begin of   mn_cnm_get_cmd_buf **************************************
 * Function name :  mn_cnm_get_cmd_buf()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
mn_cnm_cmd_type *mn_cnm_get_cmd_buf(void)
{
    mn_cnm_cmd_type *cmd;

#ifndef FEATURE_MODEM_HEAP
    cmd = (mn_cnm_cmd_type*)gs_alloc(sizeof(mn_cnm_cmd_type));
#else
    cmd = (mn_cnm_cmd_type*)modem_mem_calloc(1,sizeof(mn_cnm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif

    if (cmd == NULL)
    {
        MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Memory allocation failed",0,0,0);
    }
   /* Allocate command buffer */
    return cmd;

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*************** End of   mn_cnm_get_cmd_buf **************************************/


/*************** Begin of   mn_cm_get_cmd_buf ************************************
 * Function name :  mn_cm_get_cmd_buf()
 * -------------
 *
 * Description : This function gets a mn_cnm_cmd_type for CNM REQ 
 * -----------   commands with the header information filled in
 *
 * Uses :
 * ----
 *
 * Parameters: byte cmd_id
 * ----------
 *
 * Returns: mn_cnm_cmd_type
 * --------
 *
 */

mn_cnm_cmd_type *mn_cm_get_cmd_buf(byte cmd_id)
{
    mn_cnm_cmd_type *cmd;

    /* allocate command buffer */
#ifndef FEATURE_MODEM_HEAP
    cmd = (mn_cnm_cmd_type*)gs_alloc(sizeof(mn_cnm_cmd_type));
#else
    cmd = (mn_cnm_cmd_type*)modem_mem_calloc(1, sizeof(mn_cnm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
    if (cmd == NULL)
    {
        MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Memory allocation failed",0,0,0);
    }

    /* Fill in message_set, message_id, and message length */

    cmd->cmd.hdr.message_set = MS_MN_CM;
    cmd->cmd.hdr.message_id = cmd_id;

    switch(cmd_id)
    {
/*lint -e778 */
    case CM_MO_NORMAL_CALL_REQ:
        PUT_IMH_LEN(sizeof(cm_mo_normal_call_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_MO_EMERGENCY_CALL_REQ:
        PUT_IMH_LEN(sizeof(cm_mo_emergency_call_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_START_DTMF_REQ:
        PUT_IMH_LEN(sizeof(cm_start_dtmf_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_STOP_DTMF_REQ:
        PUT_IMH_LEN(sizeof(cm_stop_dtmf_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_0_SEND_REQ:
    case CM_1_SEND_REQ:
    case CM_2_SEND_REQ:
    case CM_3_SEND_REQ:
    case CM_4_SEND_REQ:   /* ECT */
#ifdef FEATURE_CCBS
        PUT_IMH_LEN(sizeof(cm_n_send_req_T) - sizeof(IMH_T), &cmd->cmd.hdr); 
        break;

    case CM_5_SEND_REQ: /* CCBS */
        PUT_IMH_LEN(sizeof(cm_5_send_req_T) - sizeof(IMH_T), &cmd->cmd.hdr); 
        break;
#else
    case CM_5_SEND_REQ: /* CCBS */
        PUT_IMH_LEN(sizeof(cm_n_send_req_T) - sizeof(IMH_T), &cmd->cmd.hdr); 
        break;
#endif /* FEATURE_CCBS */

    case CM_1_X_SEND_REQ:
    case CM_2_X_SEND_REQ:
        PUT_IMH_LEN(sizeof(cm_n_x_send_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_4_X_SEND_REQ: /* CD */
        PUT_IMH_LEN(sizeof(cm_4_x_send_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_MO_END_CALL_REQ:
        PUT_IMH_LEN(sizeof(cm_mo_end_call_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;


    case CM_MO_END_X_CALL_REQ:
        PUT_IMH_LEN(sizeof(cm_n_x_send_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_ASID_2_VSID_MAPPING_IND:
        PUT_IMH_LEN(sizeof(cm_asid_2_vsid_mapping_ind_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_TRANSFER_SRVCC_CALL_CONTEXT_RES:
        PUT_IMH_LEN(sizeof(cm_srvcc_call_context_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

#ifdef FEATURE_UUS
    case CM_MO_USER_DATA_REQ:
        PUT_IMH_LEN(sizeof(cm_mo_user_data_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;
#endif

    case CM_MO_MODIFY_REQ:
        PUT_IMH_LEN(sizeof(cm_mo_modify_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_ABORT_CC_REQ:
        PUT_IMH_LEN(sizeof(cm_abort_cc_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_RELEASE_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_release_ss_req_T)- sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_ACTIVATE_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_activate_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_DEACTIVATE_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_deactivate_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_ERASE_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_erase_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_INTERROGATE_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_interrogate_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_REGISTER_PASSWORD_REQ:
        PUT_IMH_LEN(sizeof(cm_register_password_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_REGISTER_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_register_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_PROCESS_UNSTRUCTURED_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_process_unstructured_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_ABORT_SS_REQ:
        PUT_IMH_LEN(sizeof(cm_abort_ss_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_PROCESS_UNSTRUCTURED_SS_DATA:
        PUT_IMH_LEN(sizeof(cm_process_unstructured_ss_data_T) - sizeof(IMH_T), 
            &cmd->cmd.hdr);
        break;

    case CM_MT_CALL_RES:
        PUT_IMH_LEN(sizeof(cm_mt_call_res_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_MT_SETUP_RES:
        PUT_IMH_LEN(sizeof(cm_mt_setup_res_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_MT_MODIFY_RES:
        PUT_IMH_LEN(sizeof(cm_mt_modify_res_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_GET_PASSWORD_RES:
        PUT_IMH_LEN(sizeof(cm_get_password_res_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case CM_UNSTRUCTURED_SS_NOTIFY_RES:
        PUT_IMH_LEN(sizeof(cm_unstructured_ss_notify_res_T)- sizeof(IMH_T), 
            &cmd->cmd.hdr);
        break;

    case CM_UNSTRUCTURED_SS_RES:
        PUT_IMH_LEN(sizeof(cm_unstructured_ss_res_T) - sizeof(IMH_T), 
            &cmd->cmd.hdr);
        break;

    case CM_MT_CALL_SS_RES:
        PUT_IMH_LEN(sizeof(cm_mt_call_ss_res_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
    case CM_LCS_MOLR_REQ:
        PUT_IMH_LEN(sizeof(cm_lcs_molr_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
    break;

    case CM_LCS_LOCATION_NOTIFICATION_RES:
        PUT_IMH_LEN(sizeof(cm_lcs_location_notification_res_T) - sizeof(IMH_T), &cmd->cmd.hdr);
    break;

    case CM_LCS_MOLR_COMPLETED:
        PUT_IMH_LEN(sizeof(cm_lcs_molr_compl_T) - sizeof(IMH_T), &cmd->cmd.hdr);
    break;
#endif

#ifdef FEATURE_CCBS
    case CM_ERASE_CC_ENTRY_REQ:
       PUT_IMH_LEN(sizeof(cm_erase_cc_entry_req_T) - sizeof(IMH_T), &cmd->cmd.hdr);
       break;

    case CM_RECALL_RSP:
       PUT_IMH_LEN(sizeof(cm_recall_rsp_T) - sizeof(IMH_T), &cmd->cmd.hdr);
       break;
#endif /* FEATURE_CCBS */

    default:
        ERR ("Unknown CNM command id",0,0,0);
#ifndef FEATURE_MODEM_HEAP
        gs_free( cmd );
#else
        modem_mem_free(cmd, MODEM_MEM_CLIENT_NAS);
#endif
        cmd = NULL;
        break;

/*lint +e778 */

    }
   
    return cmd;

}
/*************** End of   mn_cm_get_cmd_buf **************************************/

/*************** Begin of   mn_uasms_get_cmd_buf ************************************
 * Function name :  mn_uasms_get_cmd_buf()
 * -------------
 *
 * Description : This function gets a mn_cnm_cmd_type for UASMS REQ 
 * -----------   commands with the header information filled in
 *
 * Uses :
 * ----
 *
 * Parameters: byte cmd_id
 * ----------
 *
 * Returns: mn_cnm_cmd_type
 * --------
 *
 */

mn_cnm_cmd_type *mn_uasms_get_cmd_buf(byte cmd_id)
{
    mn_cnm_cmd_type *cmd;

    /* allocate command buffer */
#ifndef FEATURE_MODEM_HEAP
    cmd = (mn_cnm_cmd_type*)gs_alloc(sizeof(mn_cnm_cmd_type));
#else
    cmd = (mn_cnm_cmd_type*)modem_mem_calloc(1, sizeof(mn_cnm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
    if (cmd == NULL)
    {
        MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Memory allocation failed",0,0,0);
    }

    /* Fill in message_set, message_id, and message length */

    cmd->cmd.hdr.message_set = MS_MN_UASMS;
    cmd->cmd.hdr.message_id = cmd_id;

    switch(cmd_id)
    {
/*lint -e778 */
    case UASMS_SM_DATA_REQ:
        PUT_IMH_LEN(sizeof(SM_RL_DATA_REQ_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case UASMS_MEMORY_AVAIL_NOTIFICATION_REQ:
        PUT_IMH_LEN(sizeof(SM_RL_MEMORY_AVAILABLE_REQ_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case UASMS_ABORT_SM_REQ:
        PUT_IMH_LEN(sizeof(SM_RL_ABORT_SM_REQ_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case UASMS_SM_REPORT_REQ:
        PUT_IMH_LEN(sizeof(SM_RL_REPORT_REQ_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case UASMS_SM_LINK_CONTROL_REQ:
        PUT_IMH_LEN(sizeof(SM_RL_LINK_CONTROL_REQ_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    case UASMS_SM_UPDATE_TIMERF:
        PUT_IMH_LEN(sizeof(SM_RL_UPDATE_TIMERF_T) - sizeof(IMH_T), &cmd->cmd.hdr);
        break;

    default:
        ERR ("Unknown UASMS command id",0,0,0);
#ifndef FEATURE_MODEM_HEAP
        gs_free( cmd );
#else
        modem_mem_free(cmd, MODEM_MEM_CLIENT_NAS);
#endif
        cmd = NULL;
        break;
/*lint +e778 */
    }
   
    return cmd;

}
/*************** End of   mn_uasms_get_cmd_buf **************************************/

/*************** Begin of   mn_cnm_put_cmd ***************************************
 * Function name :  mn_cnm_put_cmd()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void mn_cnm_put_cmd(mn_cnm_cmd_type *cmd_ptr)
{

   /* Check for Null pointer */
   if(cmd_ptr != NULL)
   {

      /* First initialize the queue link. This is always done
      before putting an item in a queue. */
      (void) q_link(cmd_ptr, &(cmd_ptr->link));

      /* Put the message on the queue */
      q_put(&mn_cnm_cmd_q, &cmd_ptr->link);

      /* Set the command queue signal for */
      /* receive task                     */
      (void)rex_set_sigs(mn_tcb_ptr, MN_CNM_CMD_Q_SIG);

   }
   else
   {
       /* Raise error condition */
       MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= NULL Command pointer passed!!\n",0,0,0);

   }
}

/*************** Begin of   mn_cnm_put_internal_cmd ***************************************
 * Function name :  mn_cnm_put_internal_cmd()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */


void mn_cnm_put_internal_cmd(mn_cnm_cmd_type *cmd_ptr)
{

   /* Check for Null pointer */
   if(cmd_ptr != NULL)
   {

      /* First initialize the queue link. This is always done
      before putting an item in a queue. */
      (void) q_link(cmd_ptr, &(cmd_ptr->link));

      /* Put the message on the queue */
      q_put(&mn_cnm_internal_cmd_q, &cmd_ptr->link);

      /* Set the command queue signal for */
      /* receive task                     */
      (void)rex_set_sigs(mn_tcb_ptr, MN_CNM_CMD_INTERNAL_Q_SIG);

   }
   else
   {
       /* Raise error condition */
       ERR_FATAL("NULL Command pointer passed!!\n",0,0,0);

   }
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*************** End of   mn_cnm_put_cmd *****************************************/
