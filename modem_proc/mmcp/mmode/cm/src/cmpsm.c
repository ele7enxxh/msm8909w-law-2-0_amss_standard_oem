/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E
                        Task related functions

GENERAL DESCRIPTION
  This module is created for handling the PSM related functionality.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmpsm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/15   SKK      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "cm.h"       /* Interface to CM client service.                   */
#include "cmph.h"
#include "cmidbg.h"
#include "cmtaski.h"
#include "cmmmgsdi.h"

#include "cmpsm.h"

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/
static cm_psm_info_s_type cm_psm;


/*===========================================================================
FUNCTION cmpsm_ptr

DESCRIPTION
  To get the pointer to psm object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_psm_info_s_type *cmpsm_ptr(void)
{
	return &cm_psm;
}
/*===========================================================================
FUNCTION cmpsm_nv_read

DESCRIPTION
  Readin PSM related NV items.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_nv_read(void)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();
  /* get the pointer to the psm object to be initialized */

  int32 read_size = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                       READ NV ITEMS*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Read the PSM config */
  read_size = sizeof (cm_psm_config_s_type);
  if ( cmefs_read ( CMEFS_PSM_CONFIG,
	               (byte* )&psm_ptr->psm_cfg_info, &read_size) )
  {
    CM_MSG_MED_2 ("cmpsm: psm conf(enabled = %d, threshold = %d. ",
		           psm_ptr->psm_cfg_info.is_enabled, psm_ptr->psm_cfg_info.threshold );
    if((psm_ptr->psm_cfg_info.is_enabled) &&(psm_ptr->psm_cfg_info.threshold < CM_PSM_MIN_THRESHOLD ))
    {
      psm_ptr->psm_cfg_info.threshold = CM_PSM_MIN_THRESHOLD;
    }

    //Reading PSM_CONFIG_EXT
    read_size = sizeof(cm_psm_config_ext_s_type);
    if ( cmefs_read ( CMEFS_PSM_CONFIG_EXT,
               (byte* )&psm_ptr->psm_cfg_ext_info, &read_size) )
    {
       CM_MSG_MED_2 ("cmpsm: psm conf ext(version = %d, psm_opt_mask = %d ) ",
           psm_ptr->psm_cfg_ext_info.version, psm_ptr->psm_cfg_ext_info.psm_opt_mask );
       CM_MSG_MED_3 ("cmpsm: psm conf ext(max_oos_full_scans = %d, psm_duration_due_to_oos = %d, randomization_window = %d) ",
           psm_ptr->psm_cfg_ext_info.max_oos_full_scans, 
           psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos, 
           psm_ptr->psm_cfg_ext_info.psm_randomization_window );

    }
    else
    {
      /* set default value to FALSE */
      psm_ptr->psm_cfg_ext_info.psm_opt_mask = CM_PSM_OPT_MASK_NONE;
      psm_ptr->psm_cfg_ext_info.max_oos_full_scans       = CM_PSM_MAX_OOS_SCANS;
      psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos  = (uint16)(2 * psm_ptr->psm_cfg_info.threshold); //(2*Tm). 
      psm_ptr->psm_cfg_ext_info.psm_randomization_window = CM_PSM_DEFAULT_RANDOMIZATION_WINDOW;

      CM_MSG_MED_0 ("cmpsm: set default psm conf ext ");
    }

    //Reading PSM_UIM_OPT
    read_size = sizeof(uint8);
    if ( cmefs_read ( CMEFS_PSM_UIM_OPT,
               (byte* )&psm_ptr->psm_uim_opt_enabled, &read_size) )
    {
       CM_MSG_MED_1 ("cmpsm: : psm_uim_opt_enabled-%d ", psm_ptr->psm_uim_opt_enabled );
    }
    else
    {
      /* set default value to FALSE */
      psm_ptr->psm_uim_opt_enabled = 0;
      CM_MSG_MED_0 ("cmpsm: set default uim_opt_enabled as FALSE");
    }
  }
  else
  {
    /* set default value to FALSE */
    psm_ptr->psm_cfg_info.is_enabled = FALSE;
    psm_ptr->psm_cfg_info.threshold = 0;
    psm_ptr->psm_cfg_info.version = 0;
    psm_ptr->psm_cfg_ext_info.psm_opt_mask = CM_PSM_OPT_MASK_NONE;
    psm_ptr->psm_uim_opt_enabled = 0;

    CM_MSG_MED_0 ("cmpsm: set default psm enabled to FALSE");
  }

}
/*===========================================================================
FUNCTION cmpsm_reset

DESCRIPTION
  Initialize the PSM related data structures.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_reset(void)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();
  /* get the pointer to the psm object to be initialized */

  psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_NOT_SENT;
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_NAS] = SYS_PSM_STATUS_NONE;
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_MC]  = SYS_PSM_STATUS_NONE;
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_HDR] = SYS_PSM_STATUS_NONE;
}
/*===========================================================================
FUNCTION cmpsm_init

DESCRIPTION
  PSM initialization. Read the NV items related to PSM and reset internal 
  data structures.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_init(void)
{
  //Read the NV items related to PSM
  cmpsm_nv_read();
  //Reset the internal data structures.
   cmpsm_reset();

  cmpsm_ptr()->full_service_lost_count     = CM_PSM_MAX_OOS_SCANS;
  cmpsm_ptr()->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
}
/*===========================================================================
FUNCTION cmpsm_send_psm_ind

DESCRIPTION
 Send PSM indication to clients through msgr.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_psm_ind( cm_psm_status_e_type psm_status, cm_psm_reject_reason_e_type reason )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  errno_enum_type            msgr_error;
  cm_psm_status_ind_s_type   psm_status_ind;


  msgr_init_hdr( (msgr_hdr_struct_type*)&psm_status_ind, MSGR_MM_CM, MM_CM_PSM_STATUS_IND );

  psm_status_ind.status = psm_status;
  psm_status_ind.reject_reason = reason;

  msgr_error = msgr_send( (msgr_hdr_struct_type*)&psm_status_ind, sizeof(psm_status_ind) );
  CM_MSG_HIGH_3 ( "MM_CM_PSM_STATUS_IND - status:%d - reason:%d, msgr_error=%d",psm_status, reason, msgr_error );
}
/*===========================================================================
FUNCTION cmpsm_process_lpm

DESCRIPTION
 Send PSM_STATUS_COMPLETE Indication to clients.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_lpm( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if( psm_ptr->psm_enter_req_status == CM_PSM_ENTER_REQ_STATUS_LPM_IN_PROGRESS )
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_COMPLETED, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_NOT_SENT;
  }
  else
  {
    CM_MSG_HIGH_1("LPM, psm_enter_req_status:%d", psm_ptr->psm_enter_req_status);
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_NOT_SENT;
  }

}
/*===========================================================================
FUNCTION cmpsm_check_all_prot_status_and_send_ready_ind_or_lpm

DESCRIPTION
 1. In case if any one of the Module rejected before and ready now then 
     send READY_IND to clients.
 2. If all the protocals responded with READY then post LPM, once LPM is 
    complete then COMPLETE_IND to clients.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_check_all_prot_status_and_send_ready_ind_or_lpm( )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

//#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
//#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


  if( psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_NAS] == SYS_PSM_STATUS_READY 
    && psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_MC]  == SYS_PSM_STATUS_READY 
    && psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_HDR] == SYS_PSM_STATUS_READY )
  {
    //In case if any one of the Module rejected before and ready now then send READY_IND to clients.
    if(psm_ptr->psm_enter_req_status == CM_PSM_ENTER_REQ_STATUS_REJECTED)
    {
      cmpsm_send_psm_ind(CM_PSM_STATUS_READY, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_NOT_SENT;
    }
    else if(psm_ptr->psm_enter_req_status != CM_PSM_ENTER_REQ_STATUS_LPM_IN_PROGRESS)
    {
      dword  clk_time  = time_get_uptime_secs();

      psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_NAS] = SYS_PSM_STATUS_NONE;
      psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_MC]  = SYS_PSM_STATUS_NONE;
      psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_HDR] = SYS_PSM_STATUS_NONE;
      if(( psm_ptr->psm_enter_uptime - clk_time ) >= psm_ptr->psm_cfg_info.threshold)
      {
        //Sending LPM
        (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_LPM);
        //Mark psm enter request state to lpm in progress
        psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_LPM_IN_PROGRESS;
      }
      else
      {
        cmpsm_send_psm_ind(CM_PSM_STATUS_REJECTED, CM_PSM_REJECT_REASON_DURATION_TOO_SHORT);
        CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - REJECTED - TOO_SHORT" );

        //uptime is not meeting the threshold limit so discarding request.
        psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_NOT_SENT;
      }
    }
    else
    {
    }
  }
}
/*===========================================================================
FUNCTION cmpsm_process_ready_rsp

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_ready_rsp( cm_psm_module_e_type module, sys_psm_status_e_type psm_status )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if(psm_ptr->psm_enter_req_status == CM_PSM_ENTER_REQ_STATUS_NOT_SENT)
  {
    CM_MSG_HIGH_1("Ignoring READY_RSP from Module:%d", module);
    return;
  }

  switch( psm_status)
  {
    case SYS_PSM_STATUS_NOT_READY_NOT_IDLE:
    case SYS_PSM_STATUS_NOT_READY_EMC_CALL:
    {
      if ( psm_ptr->psm_enter_req_status != CM_PSM_ENTER_REQ_STATUS_REJECTED )
      {
         cmpsm_send_psm_ind(CM_PSM_STATUS_REJECTED, CM_PSM_REJECT_REASON_MODEM_NOT_READY);
      }

      psm_ptr->psm_enter_prot_status[module] = psm_status;
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_REJECTED; 
    }
    break;
    case SYS_PSM_STATUS_READY:
    {
      psm_ptr->psm_enter_prot_status[module] = SYS_PSM_STATUS_READY;
      cmpsm_check_all_prot_status_and_send_ready_ind_or_lpm();
    }
    break;
  default:
    break;
  }
}

/*===========================================================================
FUNCTION cmpsm_process_ready_ind

DESCRIPTION
  Process READY_IND from NAS/MC/HDR. Move to LPM if all the modules are 
  ready to enter PSM.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_ready_ind( cm_psm_module_e_type module )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if(psm_ptr->psm_enter_req_status == CM_PSM_ENTER_REQ_STATUS_NOT_SENT)
  {
    CM_MSG_HIGH_1("Ignoring READY_IND from REG, module:%d",module);
    return;
  }
  psm_ptr->psm_enter_prot_status[module] = SYS_PSM_STATUS_READY;
  cmpsm_check_all_prot_status_and_send_ready_ind_or_lpm();

}
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION cmpsm_send_psm_enter_req_to_reg

DESCRIPTION
  Send PSM enter request to the NAS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_psm_enter_req_to_reg(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  reg_cmd_type               *reg_ptr;

  /* Get reg cmd. buffer and populate params.*/
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PSM_READY_REQ );

  /* Fill in parameters. */
  reg_ptr->cmd.cm_psm_ready_req.as_id = SYS_MODEM_AS_ID_1;
  CM_MSG_HIGH_1("as_id=%d, Send CM_PSM_READY_REQ", reg_ptr->cmd.cm_psm_ready_req.as_id );

  /* Send the request to NAS */
  cm_reg_send_cmd( reg_ptr );
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_NAS] = SYS_PSM_STATUS_NONE;

}
#endif
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================
FUNCTION cmpsm_send_psm_enter_req_to_mc

DESCRIPTION
  Send PSM enter request to MC.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_psm_enter_req_to_mc(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  mc_msg_type                *mc_ptr;
      /* MC command pointer */

  /* Get MC command buffer and indicate the
  ** appropriate MC command.
  */
  mc_ptr = cm_mc_get_buf_else_err_fatal();
  mc_ptr->hdr.cmd = MC_CHECK_PSM_READY_REQ_F;

  /* Send command to MC.
  */
  cm_mc_send_cmd( mc_ptr );
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_MC] = SYS_PSM_STATUS_NONE;
}
/*===========================================================================
FUNCTION cmpsm_send_psm_enter_req_to_hdr

DESCRIPTION
  Send PSM enter request to HDR.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_psm_enter_req_to_hdr(void)
{
  #ifdef FEATURE_HDR_HYBRID
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  hdrcp_msg_req_u       *hdrcp_req;

  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));
  if(hdrcp_req == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  memset(hdrcp_req, 0, sizeof(hdrcp_msg_req_u));

  CM_MSG_HIGH_0("send HDR_CP_CM_CHECK_PSM_READY_REQ");
  (void) cm_msgr_send( HDR_CP_CM_CHECK_PSM_READY_REQ, MSGR_MM_CM, &(hdrcp_req->hdr), sizeof(hdrcp_msg_req_u) );

  cm_mem_free(hdrcp_req);
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_HDR] = SYS_PSM_STATUS_NONE;

#else
  CM_MSG_HIGH_0("HDR is disabled");
#endif // FEATURE_HDR_HYBRID

}
#endif
/*===========================================================================
FUNCTION cmpsm_process_psm_enter_req

DESCRIPTION
  processing PSM_ENTER_REQ and send psm enter request to all the protocals/modules.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpsm_process_psm_enter_req(uint32 apps_time)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  dword                      clk_time  = time_get_uptime_secs();
  cmph_s_type                *ph_ptr   = cmph_ptr();


  //Ignore if already in process of PSM LPM
  if( psm_ptr->psm_enter_req_status == CM_PSM_ENTER_REQ_STATUS_LPM_IN_PROGRESS )
  {
    CM_MSG_HIGH_1 ( "Ignoring PSM_ENTER_REQ in LPM, psm_status:%d",psm_ptr->psm_enter_req_status );
    return;
  }

  CM_MSG_MED_3 ("cmpsm: psm conf(version = %d, enabled = %d, threshold = %d. ",
                 psm_ptr->psm_cfg_info.version, psm_ptr->psm_cfg_info.is_enabled, psm_ptr->psm_cfg_info.threshold );
  CM_MSG_MED_1 ("cmpsm: psm conf_ext(opt_mask = %d)", psm_ptr->psm_cfg_ext_info.psm_opt_mask );

  //Rejct if PSM feature is diabled at NV
  if( !psm_ptr->psm_cfg_info.is_enabled )
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_REJECTED, CM_PSM_REJECT_REASON_PSM_NOT_ENABLED);
    CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - REJECTED - PSM_NOT_ENABLED" );
    return;
  }

  //Report error if apps time is less than threshold
  if(apps_time < psm_ptr->psm_cfg_info.threshold)
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_REJECTED, CM_PSM_REJECT_REASON_DURATION_TOO_SHORT);
    CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - REJECTED - TOO_SHORT" );
    if(psm_ptr->psm_enter_req_status != CM_PSM_ENTER_REQ_STATUS_NOT_SENT)
    {
      //latest apps_time is not meeting the threshold limit so discarding request. 
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_NOT_SENT;
      //clear prot status info is not necessary after marking psm_enter_req_status with NOT_SENT  
    }
    return;
  }

  //Send completed indication if already in PSM/LPM
  if( ph_ptr->oprt_mode == SYS_OPRT_MODE_LPM )
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_COMPLETED, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    CM_MSG_HIGH_0 ( "CM_PSM_STATUS_COMPLETED - Already in LPM" );
    return;
  }

  if(( psm_ptr->psm_cfg_info.is_enabled == TRUE ) && ( psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_QUICK ))
  {
      CM_MSG_HIGH_0 ( "Quik PSM is enabled so sending LPM rightaway, no need to check with NAS/MC/HDR" );
     //Sending LPM
     (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_LPM);
     //Mark psm enter request state to lpm in progress
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_LPM_IN_PROGRESS;
     return;
  }
    
  //update the uptime with apps_time
  psm_ptr->psm_enter_uptime = clk_time + apps_time;

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
  //PSM ready request to NAS
  cmpsm_send_psm_enter_req_to_reg();
#else
  //Mark NAS as READY 
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_NAS] = SYS_PSM_STATUS_READY;
#endif
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  //PSM ready request to MC
  cmpsm_send_psm_enter_req_to_mc();
  //PSM ready request to HDR
  cmpsm_send_psm_enter_req_to_hdr();
#else
  //Mark MC & HDR as READY 
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_MC]  = SYS_PSM_STATUS_READY;
  psm_ptr->psm_enter_prot_status[CM_PSM_MODULE_HDR] = SYS_PSM_STATUS_READY;
#endif
  psm_ptr->psm_enter_req_status = CM_PSM_ENTER_REQ_STATUS_SENT;
}
/*===========================================================================
FUNCTION cmpsm_process_get_cfg_params_req

DESCRIPTION
  processing PSM_ENABLE_REQ and send PSM_ENABLE_RSP.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpsm_process_get_cfg_params_req(void)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  errno_enum_type msgr_error;
  cm_psm_get_cfg_params_rsp_s_type resp_msg;

  msgr_init_hdr( (msgr_hdr_struct_type*)&resp_msg, MSGR_MM_CM, MM_CM_PSM_GET_CFG_PARAMS_RSP );

  resp_msg.enabled    = psm_ptr->psm_cfg_info.is_enabled;
  resp_msg.threshold = psm_ptr->psm_cfg_info.threshold;

  resp_msg.psm_duration_due_to_oos = psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos;
  resp_msg.psm_randomization_window = psm_ptr->psm_cfg_ext_info.psm_randomization_window;

  msgr_error = msgr_send( (msgr_hdr_struct_type*)&resp_msg, sizeof(resp_msg) );
  CM_MSG_HIGH_1 ( "MM_CM_PSM_GET_CFG_PARAMS_RSP, msgr_error=%d", msgr_error );
}

/*===========================================================================
FUNCTION cmpsm_msgr_cmd_proc

DESCRIPTION
  Process PSM msgr reports.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_msgr_cmd_proc(

  const void   *cmd_ptr
    /* Pointer to the report sent by Message Router */
)
{
  cm_psm_u_type            *cm_cmd_ptr    = (cm_psm_u_type*)cmd_ptr;

  CM_ASSERT( cm_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_1("cmpsm_msgr_cmd_proc, id %d",cm_cmd_ptr->msg_hdr.id);
  switch( cm_cmd_ptr->msg_hdr.id )
  {
  case MM_CM_PSM_GET_CFG_PARAMS_REQ:
	  cmpsm_process_get_cfg_params_req();
    break;
  case MM_CM_PSM_ENTER_REQ:
    cmpsm_process_psm_enter_req(cm_cmd_ptr->cm_psm_enter_req.apps_time);
    break;

  default:
    break;
  }
}
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION cm_psm_wrpt_handle

DESCRIPTION
  This function should be called to process the PSM reports from NAS like
  psm_ready_rsp, psm_ready_ind.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                        cm_psm_wrpt_handle
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
{
  /* Pointer to LL report message.
  */
  cm_rpt_type           *cm_rpt_ptr        = NULL;

  /* Flag to indicate if the report was processed.
  */
  boolean is_rpt_processed                 = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpt_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;

  /* Invoke the appropriate event report handler.
  */
  switch ( cm_rpt_ptr->hdr.cmd )
  {
    case CM_PSM_READY_RSP:
    {
      cmpsm_process_ready_rsp(CM_PSM_MODULE_NAS, cm_rpt_ptr->cmd.psm_ready_rsp.status );
      break;
    }
    case CM_PSM_READY_IND:
    {
      cmpsm_process_ready_ind(CM_PSM_MODULE_NAS);
      break;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      is_rpt_processed = FALSE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch ( ) */
  return is_rpt_processed;
}
#endif

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================
FUNCTION cm_psm_xrpt_handle

DESCRIPTION
  This function should be called to process the PSM reports from MC & HDR like
  psm_ready_rsp, psm_ready_ind.

DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                        cm_psm_xrpt_handle
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
{
  /* Pointer to LL report message.
  */
  cm_mc_rpt_type          *mc_rpt_ptr;

  /* Flag to indicate if the report was processed.
  */
  boolean is_rpt_processed                 = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpt_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;

  /* Invoke the appropriate event report handler.
  */
  switch ( mc_rpt_ptr->hdr.cmd )
  {
    case CM_MC_PSM_READY_RSP_F:
    {
      cmpsm_process_ready_rsp(CM_PSM_MODULE_MC, mc_rpt_ptr->psm_resp_info.status );
      break;
    }
    case CM_MC_PSM_READY_IND_F:
    {
      cmpsm_process_ready_ind(CM_PSM_MODULE_MC);
      break;
    }
    case CM_HDR_PSM_READY_RSP_F:
    {
      cmpsm_process_ready_rsp(CM_PSM_MODULE_HDR, mc_rpt_ptr->psm_resp_info.status );
      break;
    }
    case CM_HDR_PSM_READY_IND_F:
    {
      cmpsm_process_ready_ind(CM_PSM_MODULE_HDR);
      break;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      is_rpt_processed = FALSE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch ( ) */
  return is_rpt_processed;
}
#endif

/*===========================================================================

FUNCTION cmpsm_proc_full_srv_lost

DESCRIPTION
   Process the full service lost indication from SD.


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmpsm_proc_full_srv_lost 
(  
  sd_ss_e_type ss  
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if(( psm_ptr->psm_cfg_info.is_enabled != TRUE ) || !( psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS ))
  {
    return;
  }

  if( psm_ptr->full_service_lost_count == CM_PSM_MAX_OOS_SCANS )
  {
    psm_ptr->full_service_lost_count = 0;
  }

  psm_ptr->full_service_lost_count ++;

  CM_MSG_HIGH_2 ( "cmpsm_proc_full_srv_lost - (max:%d,  count:%d)" , 
               psm_ptr->psm_cfg_ext_info.max_oos_full_scans, psm_ptr->full_service_lost_count);

  if(psm_ptr->full_service_lost_count  >= psm_ptr->psm_cfg_ext_info.max_oos_full_scans )
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_OOS, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - OOS" );
    psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
  }

}

/*===========================================================================
FUNCTION cmpsm_proc_srv_ind

DESCRIPTION
   Process service indication from SD. Reset the psm full service lost counter.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void  cmpsm_proc_srv_ind 
( 
  const sd_si_info_s_type   *si_info_ptr 
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if(( psm_ptr->psm_cfg_info.is_enabled != TRUE ) || 
     !( psm_ptr->psm_cfg_ext_info.psm_opt_mask & (CM_PSM_OPT_MASK_OOS | CM_PSM_OPT_MASK_LMTD_SRV) )
    )
  {
    return;
  }

  // Reset full_service_lost_count on reciving full service
  if( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS) &&
      (psm_ptr->full_service_lost_count != CM_PSM_MAX_OOS_SCANS) && 
      (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
    )
  {
    CM_MSG_HIGH_1 ( "cmpsm_proc_srv_ind - resetting oos counter(%d)", psm_ptr->full_service_lost_count );
    psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
  }

  if(psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV)
  {
    // Reset lmtd srv uptimer if already running and got full service
    if( (psm_ptr->psm_lmtd_service_ind_uptime != CM_PSM_MAX_UPTIME) && 
        (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
      )
    {
      CM_MSG_HIGH_1 ( "cmpsm_proc_srv_ind - resetting psm lmtd srv uptime(%d)", psm_ptr->psm_lmtd_service_ind_uptime );
      psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
    }
    //Send indication on limted service, reset the psm lmtd srv uptimer
    if( (sys_intl_srv_status_to_actl_srv_status(si_info_ptr->srv_status) == SYS_SRV_STATUS_LIMITED )&&
        /* To make sure limited service is not due to CAMPED_IND*/
        (si_info_ptr->srv_domain != SYS_SRV_DOMAIN_CAMPED)
      )
    {
      cmpsm_send_psm_ind(CM_PSM_STATUS_LIMITED_SERVICE, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
      CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - LIMITERE SRV" );
      psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
    }
  }
}

/*===========================================================================

FUNCTION cmpsm_timer_proc

DESCRIPTION
  Process timer for PSM.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_timer_proc
(  
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();         /* get the pointer to the psm object */
  dword                    uptime = time_get_uptime_secs();  /* Get the current uptime */
  cmss_s_type *ss_ptr = cmss_ptr();

  //PSM is not enabled
  if(psm_ptr->psm_cfg_info.is_enabled != TRUE )
  {
    return;
  }
  /* PSM Limited service indication timer is expired 
  */
  if( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV) &&
      (psm_ptr->psm_lmtd_service_ind_uptime != CM_PSM_MAX_UPTIME) &&
      (uptime >= psm_ptr->psm_lmtd_service_ind_uptime)  
   )
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_LIMITED_SERVICE, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - LIMITERE SRV" );
    psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
  }

} /* cmpsm_timer_proc */

/*===========================================================================

FUNCTION cmpsm_set_psm_lmtd_srv_ind_timer

DESCRIPTION
  Set limited service indication timer.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_set_psm_lmtd_srv_ind_timer( )
{

  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if ( (psm_ptr->psm_cfg_info.is_enabled != TRUE) || 
       !( psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV )
     )
  {
    return;
  }
  //Set psm lmtd srv uptime if timer is not running
  if ( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV) && 
       (psm_ptr->psm_lmtd_service_ind_uptime == CM_PSM_MAX_UPTIME) 
     )
  {
    dword                    uptime = time_get_uptime_secs();  /* Get the current uptime */

    psm_ptr->psm_lmtd_service_ind_uptime =  time_get_uptime_secs() + CM_PSM_LMTD_SRV_IND_WAIT_TIME;
    CM_MSG_HIGH_2 ( "cmpsm_set_psm_lmtd_srv_ind_timer - (uptime:%d,  ind_uptime:%d)" , 
                     uptime, psm_ptr->psm_lmtd_service_ind_uptime);

  }
}

/*===========================================================================
FUNCTION cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail

DESCRIPTION
  To check whether PSM UIM Optimization is enabled and no subscription available.

DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail()
{

  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if( psm_ptr->psm_cfg_info.is_enabled &&  psm_ptr->psm_uim_opt_enabled &&
      !cmmmgsdi_count_given_state(CMMMGSDI_STATE_SUBSCRIPTION_READY)
     )
  {
    CM_MSG_HIGH_0 ( "uim_opt_enabled_and_no_subs_avail" );
    return TRUE;
  }

  return FALSE;


} /* cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail */
