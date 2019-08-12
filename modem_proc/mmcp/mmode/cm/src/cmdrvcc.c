/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object changes for
  setting up DRVCC call from WiFi to CS technologies.


EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdrvcc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/15   pk      DRVCC setup initial version

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "sys_gw.h"
#include "sys_gw_v.h"
#include "cmi.h"       /* Internal interface to cm.c */

#include <string.h>    /* Interface to string library */
#include "rex.h"       /* Interface to rex services */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmaccessctrl.h"
#include "cmlog.h"     /* Interface to diag services */
#include "cmlog_v.h"
#include "cmreply.h"
#include "cmtaski.h"
#include "cmcall.h"    /* Interface to CM call object */
#include "sd_v.h"      /* Interface to system Determination */
#include "cmpmprx.h"
#include "cmregprxi.h"

#ifdef FEATURE_CM_LTE
#include "cmltecall.h" /* Interface to CM LTE call interfaces */
#endif
#include "cmsds.h"      /* Interface to Domain selection object*/
#if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined (FEATURE_DOMAIN_SELECTION)
#include "cmcsfbcall.h" /* Interface to CM CSFB call object */
#endif

#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxcall.h"   /* Interface to CM CDMA call object */
#include"cmxll.h"
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#ifdef CM_GW_SUPPORTED
#include "smtask.h"
#include "smtask_v.h"
#include "cmwcall.h"
#include "cmsups.h"
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

#include "cmutil.h"
#include "cmss.h"
#include "cmmsc.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"

#ifdef FEATURE_IP_CALL
#include "cmipcall.h"    /* CM IP call interface */
#include "cmipappi.h"    /* Interface to query registered apps */
#endif

#include "cmmmgsdi.h"
#include "cmtaski.h"
#include "cmph.h"
#ifdef CM_DEBUG
#error code not present
#endif

#include "cmcc.h"
#include "ran.h"        /* for ran_dist */
#include "secapi.h"     /* for secure random */
#include "mm.h"
#include "cmemgext.h"
#include "subs_prio.h"
#include "cmpmprx.h"
#include "cmclnup.h"
#include "cmsimcoord.h"
#include "cmdrvcc.h"

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)


/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

static cmcall_drvcc_cache_s_type cmcall_drvcc_cache;  




/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/




/*===========================================================================

FUNCTION cmcall_cmd_drvcc_setup_req_proc

DESCRIPTION

  This function handles the DRVCC setup command.
  This function sends the HO_START to clients.
  Fills the preferences for DRVCC call origination
  set the context in switch in call id.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_cmd_drvcc_setup_req_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
)
{
  cm_call_cmd_info_s_type      *cmd_info_ptr   = NULL;
  cmcall_s_type                *switch_in_call_ptr       = NULL;
  cm_drvcc_setup_req_s_type    *drvcc_setup_info_ptr    = NULL;
  cmcall_s_type                *switch_out_call_ptr       = NULL;
  cm_call_id_type              active_switch_out_call_id = CM_CALL_ID_INVALID;
    
  cmss_s_type                  *ss_ptr         = cmss_ptr();

  cm_call_cmd_err_e_type        cmd_err        = CM_CALL_CMD_ERR_NOERR;
  cm_cc_mode_pref_e_type        mode_usage     = CM_CC_MODE_PREF_NONE;
  cmph_s_type                  *ph_ptr         = cmph_ptr();

  
  CM_MSG_LOW_0("START cmcall_cmd_drvcc_setup_req_proc()" );

  cmd_info_ptr         = CMD_INFO_PTR( call_cmd_ptr );
  drvcc_setup_info_ptr = DRVCC_SETUP_INFO_PTR( call_cmd_ptr );

  /*
  ** Get a pointer to switch in call Object.
  */
  switch_in_call_ptr = cmcall_ptr(cmd_info_ptr->call_id);
  if (switch_in_call_ptr == NULL)
  {
    CM_MSG_HIGH_1("DRVCC: Can not retrieve call pointer for call id in drvcc setup cmd = %d",
       cmd_info_ptr->call_id);
    cmcall_drvcc_trigger_ho_fail_ipapp(FALSE, drvcc_setup_info_ptr->as_id);
    return;
  }

  /* get active wifi call that needs to be drvcc'ed 
  VT call is to be downgraded before drvcc, so only find Voice call*/
  active_switch_out_call_id = cmcall_get_active_drvcc_vowlan_call(CM_CALL_TYPE_VOICE_MASK|CM_CALL_TYPE_VT_MASK);
  if(cmcall_drvcc_is_3gpp2_drvcc_type(drvcc_setup_info_ptr->drvcc_type) &&
    !cmcall_drvcc_validate_active_call_in_context(active_switch_out_call_id, drvcc_setup_info_ptr))
  {
    CM_MSG_HIGH_1("DRVCC: switch_out call id cannot be validated in context = %d",
       active_switch_out_call_id);
    cmcall_drvcc_trigger_ho_fail_ipapp(FALSE, drvcc_setup_info_ptr->as_id);
    return;
  }
  
  switch_out_call_ptr = cmcall_ptr(active_switch_out_call_id);
  if (switch_out_call_ptr == NULL)
  {
    CM_MSG_HIGH_1("DRVCC: Can not retrieve call pointer for active call that can be drvcc'ed = %d",
       active_switch_out_call_id);
    cmcall_drvcc_trigger_ho_fail_ipapp(FALSE, drvcc_setup_info_ptr->as_id);
    return;
  }  

  cmcall_drvcc_init_switch_in_call_object(switch_in_call_ptr, call_cmd_ptr);
  /* As only Voice call type can be setup for 1XDRVCC as of now*/
  cmcall_set_type(switch_in_call_ptr,CM_CALL_TYPE_VOICE); 
  /* Copy the current serving system status  */
  cmcall_copy_ss_info( switch_in_call_ptr );

  CM_MSG_HIGH_2("DRVCC: switch in call ss %d, subs_id %d",switch_in_call_ptr->ss, switch_in_call_ptr->asubs_id);  

  /* This function will write CM_MODE_PREF_NONE into favored_mode_pref
     if it was not able to compute it */
  if (!cmcall_drvcc_set_favored_mode_pref_for_setup ( switch_in_call_ptr->ss,
                                                      drvcc_setup_info_ptr->drvcc_type,
                                                      &cmd_info_ptr->favored_mode_pref,
                                                      &cmd_info_ptr->favored_hybr_pref ))
  {
    CM_MSG_HIGH_0("DRVCC: Cannot set valid mode pref for DRVCC setup call");
    cmcall_drvcc_trigger_ho_fail_ipapp(switch_in_call_ptr->orm_sent_status, drvcc_setup_info_ptr->as_id);
    return;
  }

  CM_MSG_HIGH_0("DRVCC: Fav mode pref as computed:");
  cmdbg_print_fav_mode_pref_for_origination( switch_in_call_ptr->asubs_id,
                                             switch_in_call_ptr->call_type,
                                             CM_SRV_TYPE_AUTOMATIC,
                                             CMPH_SS_MODE_PREF(switch_in_call_ptr->asubs_id, ph_ptr),
                                             cmd_info_ptr->favored_mode_pref,
                                             cmd_info_ptr->favored_hybr_pref );

  if( !cmph_is_valid_call_mode_pref_on_ss(switch_in_call_ptr->ss, cmd_info_ptr->favored_mode_pref, (cm_cmd_type*)call_cmd_ptr) )
  {
    /* param: need to deallocate call id and connection id as well when doing drvcc ho fail trigger even for IPAPP only */
    CM_MSG_HIGH_0("DRVCC: not valid favored mode pref for drvcc");
    cmcall_drvcc_trigger_ho_fail_ipapp(switch_in_call_ptr->orm_sent_status, drvcc_setup_info_ptr->as_id);
    return;
  }
  
  switch_in_call_ptr->favored_mode_pref = cmd_info_ptr->favored_mode_pref;
  switch_in_call_ptr->favored_hybr_pref = cmd_info_ptr->favored_hybr_pref;

  /* Run domain selection */
  #if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)  || \
      defined (FEATURE_DOMAIN_SELECTION)
  if(switch_in_call_ptr->csfb_type == CM_CALL_CSFB_TYPE_NONE)
  {
    switch_in_call_ptr->csfb_type = 
      cmcall_process_dom_sel_params(switch_in_call_ptr);
    /*what should be done if there is mismatch in the csfb_type sent by IMS
    and what is calculated here */
    /* Say if IMS sends drvcc type as 1XCSFB, but current LTE does not support 1XCSFB 
    or if UE is OOS, then mark drvcc type as native 1X*/
    if(drvcc_setup_info_ptr->drvcc_type == SYS_DRVCC_ONEXCSFB)
    {
      if(switch_in_call_ptr->csfb_type != CM_CALL_CSFB_TYPE_1XCSFB)
      {
        drvcc_setup_info_ptr->drvcc_type = SYS_DRVCC_NATIVE_ONEX;
        switch_in_call_ptr->favored_mode_pref = CM_MODE_PREF_CDMA_ONLY;
        CM_MSG_HIGH_0("DRVCC: updating DRVCC type to native 1x");
      }
    }
    else if(drvcc_setup_info_ptr->drvcc_type == SYS_DRVCC_PPCSFB)
    {
      if(switch_in_call_ptr->csfb_type != CM_CALL_CSFB_TYPE_PPCSFB)
      {
        drvcc_setup_info_ptr->drvcc_type = SYS_DRVCC_3GPP;
        CM_MSG_HIGH_0("DRVCC: updating DRVCC type to 3gpp");
      }
    }
  }
  #endif

  /* drvcc determine mode usage will consider the drvcc type, and also check if target supports
  modes or not SD functions, refer determine mode usage */
  mode_usage = cmcall_drvcc_determine_mode_usage(drvcc_setup_info_ptr->drvcc_type);
  if (mode_usage == CM_CC_MODE_PREF_NONE)
  {
    CM_MSG_HIGH_0("DRVCC: Mode usage is calculated as NONE reject DRVCC");
    cmcall_drvcc_trigger_ho_fail_ipapp(switch_in_call_ptr->orm_sent_status, drvcc_setup_info_ptr->as_id);
    return;
  }

  /*Param: need to check if need to allocate connection id for this intermediate setup call */
  if ((mode_usage == CM_CC_MODE_PREF_GW_ONLY) ||
        (mode_usage == CM_CC_MODE_PREF_ANY))
  {
    if (!BETWEEN((int8)switch_in_call_ptr->gw_connection_id, -1, (int8)UNKNOWN_CONNECTION_ID))
    {
      if (!cmcall_generate_connection_id(switch_in_call_ptr))
      {
        CM_ERR_1("DRVCC: Can not set connection Id Info",switch_in_call_ptr->call_id);
        cmcall_drvcc_trigger_ho_fail_ipapp(switch_in_call_ptr->orm_sent_status, drvcc_setup_info_ptr->as_id);
        return;
      }
    }
  }  

  CM_MSG_HIGH_1( "DRVCC: mode_usage=%d",mode_usage );

  cmd_err = cmcall_process_orig_mode(mode_usage,switch_in_call_ptr);

  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    CM_MSG_HIGH_1("DRVCC: Process Orig Mode failed err %d",cmd_err);
    cmcall_drvcc_trigger_ho_fail_ipapp(switch_in_call_ptr->orm_sent_status, drvcc_setup_info_ptr->as_id);
    return;
  }

  /* param: 
  1. switch in call ptr cc_object_ptr here to NULL 
  2. do the mapping and store context of switch out call id in switch in call id
  3. Store the DRVCC call context sent by IMS, maybe reuse SRVCC cache
  will be required for 3gpp and not 3gpp2, mapping would also have to be done to store context in CM.
  */
  cmcall_drvcc_copy_fields_from_switch_out_to_switch_in_call( switch_in_call_ptr, switch_out_call_ptr );
  if( cmcall_drvcc_is_3gpp2_drvcc_type(drvcc_setup_info_ptr->drvcc_type))
  {
    /* taking from cache context rather than switch out call ptr, as IMS may have changed that */
    switch_in_call_ptr->num = drvcc_setup_info_ptr->call_context[0].call_info.ip_call.call_num;
  }
  

  switch_in_call_ptr->voice_ho_type = switch_out_call_ptr->voice_ho_type =
      cmcall_drvcc_get_voice_ho_type_from_drvcc_type(drvcc_setup_info_ptr->drvcc_type);
  /* send call event HO start */
  cmcall_event( switch_out_call_ptr, CM_CALL_EVENT_HO_START );

  /* set drvcc cache */
  cmcall_drvcc_cache_init();
  cmcall_drvcc_fill_cache_from_setup_info(drvcc_setup_info_ptr);
  cmcall_drvcc_cache_update_state(CM_DRVCC_STATE_SETUP_IN_PROG);

  /*-----------------------------------------------------------------------*/
  cmcall_orig_start (switch_in_call_ptr);
}

/*===========================================================================

FUNCTION cmcall_drvcc_init_switch_in_call_object

DESCRIPTION

  initializes switch in call object 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_init_switch_in_call_object(cmcall_s_type *switch_in_call_ptr,
                                                  cm_call_cmd_s_type *call_cmd_ptr)
{
  cm_ss_e_type                  ss;
  /* ss on which this origination cmd is issued */
  sys_modem_as_id_e_type   asubs_id;
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cdma_cmcall_type *call_mode_info_ptr = NULL;
  #endif

  /* call control is not required for this STN setup call, so mark CC allowed with no modifications */
#ifdef CM_GW_SUPPORTED
  switch_in_call_ptr->gw_cc_modified_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
#endif
  switch_in_call_ptr->cdma_cc_modified_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
  switch_in_call_ptr->call_state      = CM_CALL_STATE_IDLE;
  switch_in_call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_NONE;
  switch_in_call_ptr->line = cmph_ptr()->line;
  switch_in_call_ptr->cc_object_ptr = NULL;
  switch_in_call_ptr->srv_type = CM_SRV_TYPE_AUTOMATIC;

  switch_in_call_ptr->client_categ = call_cmd_ptr->client_categ;
  switch_in_call_ptr->direction = call_cmd_ptr->direction; 
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  /* set SO to wildcard*/
  call_mode_info_ptr = CALL_MODE_INFO_PTR( switch_in_call_ptr );
  call_mode_info_ptr->srv_opt = CAI_SO_VOICE_WILDCARD;  
  #endif
  
  if (cmph_is_msim())
  {
    /* Get the SS, based on the subscription-id, on which this call is
    ** originated.
    */
    ss = cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs);
    asubs_id = switch_in_call_ptr->asubs_id = call_cmd_ptr->cmd_subs;
    CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);
    CM_ASSERT_ENUM_IS_INRANGE(ss, CM_SS_MAX);

    /* Set client asubs_id to the subs id as sent by client orig cmd */
    switch_in_call_ptr->client_asubs_id = call_cmd_ptr->cmd_subs;
    switch_in_call_ptr->ss = ss;
  }
  else
  {
    switch_in_call_ptr->ss = CM_SS_MAIN;
    asubs_id = switch_in_call_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }
}


/*===========================================================================

FUNCTION cmcall_get_active_drvcc_volte_call

DESCRIPTION
  Find VOWIFI  call which is in active CONV state for DRVCC

  3GPP2 : Active Voice call is the one that is to be DRVCC'ed

DEPENDENCIES
  None

RETURN VALUE
  valid call id of call that is to be DRVCC'ed, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_active_drvcc_vowlan_call
(
   cm_call_type_mask_type         call_type_mask
        /* Call type to search */
)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
   if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
         call_ptr->is_vowlan == TRUE &&
         ((SYS_BM_64BIT(call_ptr->call_type)) & call_type_mask) &&
         call_ptr->call_state == CM_CALL_STATE_CONV &&
         call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE)
   {
      return call_ptr->call_id;
   }
   call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */
  
  #else
  SYS_ARG_NOT_USED(call_type_mask);
  #endif
  return CM_CALL_ID_INVALID;

} /* cmcall_get_active_srvcc_volte_call() */


/*===========================================================================

FUNCTION cmcall_trigger_drvcc_ho_fail_ipapp

DESCRIPTION
  Send DRVCC_HO_FAIL_NOTIFY to IMS APP
  orm_sent_status will be for switch_in_call_id
  rather than switch_out IMS call id

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_trigger_ho_fail_ipapp
(
   boolean orm_sent_status, 
   sys_modem_as_id_e_type as_id
)
{
  cmipapp_send_drvcc_notification(CMIPAPP_DRVCC_HO_FAIL_NOTIFY, orm_sent_status, as_id);
}

/*===========================================================================

FUNCTION cmcall_drvcc_validate_active_call_in_context

DESCRIPTION
  To validate if switched out call id is part of context sent by IMS 

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_validate_active_call_in_context(cm_call_id_type call_id, cm_drvcc_setup_req_s_type *drvcc_setup_info_ptr)
{
  boolean ret_val = FALSE;
  int i = 0;
  
  if(call_id == CM_CALL_ID_INVALID)
  {
    ret_val = FALSE;
  }

  for(i = 0; i < drvcc_setup_info_ptr->num_of_calls; i++)
  {
    if(drvcc_setup_info_ptr->call_context[i].is_conf_call)
    {
      if(drvcc_setup_info_ptr->call_context[i].call_info.ip_conf_call.call_id == call_id)
      {
        ret_val = TRUE;
        break;
      }
    }
    else
    {
      if(drvcc_setup_info_ptr->call_context[i].call_info.ip_call.call_id == call_id)
      {
        ret_val = TRUE;
        break;
      }
    }
  }

  return ret_val;
}


/*===========================================================================

FUNCTION cmcall_drvcc_skip_call_event

DESCRIPTION
  This function tells if call events are to be suppressed for switch in and switch out call id
  when DRVCC is in progress
  1. Suppress Call events for switch in call, when DRVCC is in progress
  2. Suppress Call end event for switch out call id of WLAN, i.e. call that is being drvcc'ed

DEPENDENCIES
  None

RETURN VALUE
  True : Skip sending call event
  False : Do not skip sending call event.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_skip_call_event(cm_call_event_e_type call_event, cmcall_s_type *call_ptr)
{
  boolean ret_val = FALSE;

  if(call_ptr == NULL)
  {
    ret_val = FALSE;
  }
  else
  {
    if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
    {
      /* for switch in call id all events are to be skipped till DRVCC is in progress */
      if( cmcall_drvcc_is_3gpp2_drvcc_type(cmcall_drvcc_get_setup_call_drvcc_type(call_ptr)) )
      {
        CM_MSG_HIGH_1("DRVCC: switch in call id %d for 3gpp2 drvcc, skip event", call_ptr->call_id);
        ret_val = TRUE;
      }
    }
    /* for switch out call id that is DRVCC'ed and ended by IMS
    it is end_status based so special care is to be taken */
    if(call_ptr->end_status == CM_CALL_END_DRVCC_WIFI_TO_CS)
    {
      ret_val = TRUE;
    }
  }
  return ret_val;
}


/*===========================================================================

FUNCTION cmcall_drvcc_copy_fields_from_switch_out_to_switch_in_call

DESCRIPTION

  Copies the required fields from switch out call to switch in call 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_copy_fields_from_switch_out_to_switch_in_call(cmcall_s_type *switch_in_call_ptr,
                                                                        cmcall_s_type *switch_out_call_ptr )
{
  CM_ASSERT(switch_in_call_ptr);
  CM_ASSERT(switch_out_call_ptr);
  
  switch_in_call_ptr->calling_num = switch_out_call_ptr->calling_num;
#ifdef FEATURE_HDR_HANDOFF
  switch_in_call_ptr->last_act_data_net = switch_out_call_ptr->last_act_data_net;
#endif
  /* set drvcc parent call id in switch in call, as switch out call's call id */
  switch_in_call_ptr->drvcc_parent_call_id = switch_out_call_ptr->call_id;
}



/*===========================================================================

FUNCTION cmcall_drvcc_is_valid_drvcc_type

DESCRIPTION
  To check if drvcc type is valid

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_is_valid_drvcc_type(sys_drvcc_type_e_type drvcc_type)
{
  switch(drvcc_type)
  {
  case SYS_DRVCC_NATIVE_ONEX:
  case SYS_DRVCC_ONEXCSFB:
  case SYS_DRVCC_3GPP:
  case SYS_DRVCC_PPCSFB:
    return TRUE;

  default:
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmcall_drvcc_is_3gpp2_drvcc_type

DESCRIPTION
  To check if drvcc type is 3gpp2 

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_is_3gpp2_drvcc_type(sys_drvcc_type_e_type drvcc_type)
{
   switch(drvcc_type)
  {
  case SYS_DRVCC_NATIVE_ONEX:
  case SYS_DRVCC_ONEXCSFB:
    return TRUE;

  default:
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmcall_drvcc_set_favored_mode_pref_for_setup

DESCRIPTION
  sets favored mode preference for DRVCC setup call 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if favored mode and hybrid preference were computed successfully,
  in which case they are written into fav_mode_pref_ptr and
  fav_hybr_pref_ptr, FALSE otherwise.

  If the function retured FALSE then CM wrote the values of CM_HYBR_PREF_NONE
  and CM_MODE_PREF_NONE into fav_hybr_pref_ptr and fav_mode_pref_ptr

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_set_favored_mode_pref_for_setup (cm_ss_e_type ss,
                                                      sys_drvcc_type_e_type drvcc_type,
                                                      cm_mode_pref_e_type *fav_mode_pref_ptr,
                                                      cm_hybr_pref_e_type *fav_hybr_pref_ptr )
{


  const cmph_s_type   *ph_ptr = cmph_ptr();
    /* point at phone object */
  cm_mode_pref_e_type ph_mode_pref, intersect_mode_pref;
  cm_mode_pref_e_type calc_mode_pref = CM_MODE_PREF_NONE;
  cm_hybr_pref_e_type ph_hybr_pref;
  
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /*-------------------------------------------------------------------------*/

  CM_ASSERT(fav_mode_pref_ptr != NULL);
  CM_ASSERT(fav_hybr_pref_ptr != NULL);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  ph_mode_pref = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);
  ph_hybr_pref = *fav_hybr_pref_ptr = CMPH_SS_HYBR_PREF(asubs_id, ph_ptr);


  /*-------------------------------------------------------------------------*/
  if(drvcc_type == SYS_DRVCC_NATIVE_ONEX)
  {
    calc_mode_pref = CM_MODE_PREF_CDMA_ONLY;
  }
  else if(drvcc_type == SYS_DRVCC_ONEXCSFB)
  {
    calc_mode_pref = CM_MODE_PREF_CDMA_LTE_ONLY;
  }
  else if(drvcc_type == SYS_DRVCC_3GPP || 
    drvcc_type == SYS_DRVCC_PPCSFB)
  {
    calc_mode_pref = CM_MODE_PREF_GWL;
  }

  /* if calculated mode pref is not part of mode preference set currently in PH
  then return FALSE and set favored modes and bands to NONE*/
  intersect_mode_pref = cmcall_misc_get_common_mode_pref(calc_mode_pref, 1, ph_mode_pref);
  if(intersect_mode_pref == CM_MODE_PREF_NONE ||
      intersect_mode_pref >= CM_MODE_PREF_MAX )
  {
    *fav_hybr_pref_ptr = CM_HYBR_PREF_NONE;
    *fav_mode_pref_ptr = CM_MODE_PREF_NONE;

    CM_MSG_LOW_0("DRVCC: mode pref not part of currently set Phone mode prefs !!!");
    return FALSE;
  }
  
  *fav_mode_pref_ptr = intersect_mode_pref;

#ifdef CM_DEBUG
  #error code not present
#endif
  return TRUE;

}


/*===========================================================================

FUNCTION cmcall_drvcc_determine_mode_usage

DESCRIPTION
  To determine mode usage for the drvcc setup call

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_mode_pref_e_type cmcall_drvcc_determine_mode_usage(sys_drvcc_type_e_type drvcc_type)
{
  cm_cc_mode_pref_e_type ret_cc_mode_pref = CM_CC_MODE_PREF_ANY;
  
  switch(drvcc_type)
  {
    case SYS_DRVCC_NATIVE_ONEX:
      ret_cc_mode_pref = CM_CC_MODE_PREF_CDMA_ONLY;
      break;
    case SYS_DRVCC_ONEXCSFB:
      ret_cc_mode_pref = CM_CC_MODE_PREF_ANY;
      break;
    case SYS_DRVCC_3GPP:
    case SYS_DRVCC_PPCSFB:
      ret_cc_mode_pref = CM_CC_MODE_PREF_GW_ONLY;
      break;
    default:
      CM_ASSERT(0);
      break;
  }

  return ret_cc_mode_pref;
}

/*===========================================================================

FUNCTION cmcall_drvcc_get_voice_ho_type_from_drvcc_type

DESCRIPTION
  To determine voice_ho_type from drvcc_type

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
sys_voice_handover_e_type cmcall_drvcc_get_voice_ho_type_from_drvcc_type(sys_drvcc_type_e_type drvcc_type)
{
  sys_voice_handover_e_type voice_ho_type = SYS_VOICE_HO_NONE;
  
  switch(drvcc_type)
  {
    case SYS_DRVCC_NATIVE_ONEX:
    case SYS_DRVCC_ONEXCSFB:
      voice_ho_type = SYS_VOICE_HO_DRVCC_WIFI_2_C;
      break;
    case SYS_DRVCC_3GPP:
    case SYS_DRVCC_PPCSFB:
      voice_ho_type = SYS_VOICE_HO_DRVCC_WIFI_2_GW;
      break;
    default:
      CM_ASSERT(0);
      break;
  }

  return voice_ho_type;
}

/*===========================================================================

FUNCTION cmcall_drvcc_get_setup_call_drvcc_type

DESCRIPTION
  To determine if call in concern is drvcc setup call
  1. drvcc_parent call id is active and existing
  2. cache has valid drvcc_type

DEPENDENCIES
  None

RETURN VALUE
  sys_drvcc_type_e_type what kind of drvcc call type is in progress, from drvcc cache.

SIDE EFFECTS
  none

===========================================================================*/
sys_drvcc_type_e_type cmcall_drvcc_get_setup_call_drvcc_type(cmcall_s_type *call_ptr)
{
  sys_drvcc_type_e_type ret_drvcc_type = cmcall_drvcc_cache_get_drvcc_type();
  cm_call_id_type drvcc_parent_call_id = CM_CALL_ID_INVALID;
  cmcall_s_type *drvcc_parent_call_ptr = NULL;

  CM_ASSERT( call_ptr != NULL );
  
  drvcc_parent_call_id = call_ptr->drvcc_parent_call_id;
  
  if( (drvcc_parent_call_id != CM_CALL_ID_INVALID)
    )
  {
    drvcc_parent_call_ptr = cmcall_ptr(drvcc_parent_call_id);
    /* for 3gpp2 drvcc only active call can be drvcc'ed */
    if( (drvcc_parent_call_ptr != NULL) &&
        (drvcc_parent_call_ptr->call_state == CM_CALL_STATE_CONV) &&
        (drvcc_parent_call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE) &&
        (cmcall_drvcc_is_3gpp2_drvcc_type(ret_drvcc_type))
    )
    {
      CM_MSG_HIGH_2("DRVCC: Call id %d is 3gpp2 DRVCC type %d setup call",call_ptr->call_id, ret_drvcc_type);
    }
    else
    {
      /* drvcc parent call pointer is NULL */
      ret_drvcc_type = SYS_DRVCC_NONE;
    }
  }
  else
  {
    /* drvcc parent call id is invalid */
    ret_drvcc_type = SYS_DRVCC_NONE;
  }

  return ret_drvcc_type;
}

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 

/*===========================================================================

FUNCTION cmcall_drvcc_fill_stn_nr

DESCRIPTION

  Fills STN Number in orig drvcc request.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_fill_stn_nr(
  mc_origination_type    *orig_param
)
{
  int dig_len = 0;
  CM_ASSERT(orig_param != NULL);  
  dig_len = strlen((char *) cmcall_drvcc_cache.stn_nr);  
  orig_param->cnt  = memscpy( orig_param->called_address,ARR_SIZE(orig_param->called_address), 
                            cmcall_drvcc_cache.stn_nr, dig_len );
  CM_MSG_HIGH_2("DRVCC: dig_len %d orig_param->cnt %d",dig_len, orig_param->cnt);
}


/*===========================================================================

FUNCTION cmcall_drvcc_send_drvcc_orig

DESCRIPTION
  To send call orig for drvcc

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_send_xdrvcc_orig(cmcall_s_type *call_ptr)
{
  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();
#if defined (FEATURE_LTE_TO_1X)
  cmsds_s_type                            *sds_ptr               = cmsds_ptr();
#endif

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( mc_ptr != NULL );

#if defined (FEATURE_LTE_TO_1X)
  if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB && 
      cmcall_drvcc_cache_get_drvcc_type() == SYS_DRVCC_ONEXCSFB && 
      sds_ptr->is_1xcsfb_activated)
  {
    /* Call type is 1XCSFB and 1XCP is activated in CSFB mode
    ** and drvcc type is also 1xcsfb
    */
    CM_MSG_HIGH_0("DRVCC: CSFB DRVCC Orig");
    mc_ptr->hdr.cmd = MC_DRVCC_CSFB_ORIGINATION_F;
  } else
#endif
  {
    CM_MSG_HIGH_0("DRVCC: NATIVE ONEX DRVCC Orig");
    mc_ptr->hdr.cmd = MC_DRVCC_ORIGINATION_F;
  }

  cmxcall_fill_srvcc_orig_param(call_ptr, &mc_ptr->origination, TRUE);  
  mc_ptr->origination.number_of_calls = 1;  
  /* set STN in called_address field of orig_param */
  cmcall_drvcc_fill_stn_nr(&mc_ptr->origination);
  cm_mc_send_cmd( mc_ptr );
  call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_CDMA;
  
  return TRUE;
}



/*===========================================================================

FUNCTION cmcall_drvcc_send_x_audio_rel_ind

DESCRIPTION
  To send audio release ind to 1XCP

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_send_x_audio_rel_ind(void)
{
  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();

  CM_ASSERT( mc_ptr != NULL );
  mc_ptr->hdr.cmd = MC_AUDIO_REL_IND_F;
  mc_ptr->audio_rel_ind.rel_status = TRUE;
  cm_mc_send_cmd( mc_ptr );  
  return TRUE;
}


/*===========================================================================

FUNCTION cmcall_drvcc_send_xdrvcc_end

DESCRIPTION
  To send DRVCC_END_F for ending DRVCC setup

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_send_xdrvcc_end(cmcall_s_type *call_ptr)
{
  mc_msg_type    *mc_ptr = cm_mc_get_buf_else_err_fatal();

  mc_ptr->hdr.cmd = MC_DRVCC_END_F;
  mc_ptr->drvcc_call_end.call_id = call_ptr->call_id;
  mc_ptr->drvcc_call_end.end_reason = (byte)CAI_REL_NORMAL;
  cm_mc_send_cmd( mc_ptr );
}




#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


/*===========================================================================

FUNCTION cmcall_drvcc_send_drvcc_end

DESCRIPTION
  To send DRVCC End 

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_send_drvcc_end(cmcall_s_type *call_ptr)
{
  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 

  if(cmcall_drvcc_is_3gpp2_drvcc_type(cmcall_drvcc_cache_get_drvcc_type()))
  {
    cmcall_drvcc_send_xdrvcc_end(call_ptr);
  }
  
  #else
  /* for 3gpp DRVCC will have to add code here for sending End 
  */
  SYS_ARG_NOT_USED(call_ptr);
  CM_ERR_0("DRVCC: 1X is not supported on this target");
  #endif 
  
  return;
}



/*===========================================================================

FUNCTION cmcall_drvcc_send_drvcc_orig

DESCRIPTION
  To send call orig for drvcc

DEPENDENCIES
  None

RETURN VALUE
  TRUE : If DRVCC ORIG is sent succesfully
  False : If DRVCC ORIG is not sent

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_send_drvcc_orig(cmcall_s_type *call_ptr, sys_drvcc_type_e_type drvcc_type)
{
  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 

  if(cmcall_drvcc_is_3gpp2_drvcc_type(drvcc_type))
  {
    cmcall_drvcc_send_xdrvcc_orig(call_ptr);
  }
  return TRUE;
  #else
  /* for 3gpp DRVCC will have to add code here for origination 
  */
  SYS_ARG_NOT_USED(call_ptr);
  SYS_ARG_NOT_USED(drvcc_type);
  CM_ERR_0("DRVCC: 1X is not supported on this target");
  return FALSE;
  #endif 
}


/*===========================================================================

FUNCTION cmcall_drvcc_is_switch_out_call_id_ret_switch_in_call_id

DESCRIPTION
  Determine if this is switch out call id by checking drvcc parent call id 
  of all existing calls, if any call matched then this is switch out call id being ended 

DEPENDENCIES
  None

RETURN VALUE
  CM_CALL_ID_INVALID : If call is not switch out call id
  Valid call id : If call id passed is switch out call id, then this function returns switch in call id corresponding to it
  
SIDE EFFECTS
  none

===========================================================================*/
static cm_call_id_type cmcall_drvcc_is_switch_out_call_id_ret_switch_in_call_id(cm_call_id_type call_id)
{
  cm_call_id_type switch_in_call_id = CM_CALL_ID_INVALID;
  
  if(call_id != CM_CALL_ID_INVALID)
  {
    cmcall_s_type   *call_ptr;                        /* Call object pointer */
    cm_iterator_type call_itr;                        /* call Object Iterator */

    cmcall_obj_iterator_init(&call_itr);
    call_ptr = cmcall_obj_get_next(&call_itr);
    while(call_ptr != NULL)
    {
      if(call_ptr->drvcc_parent_call_id == call_id)
      {
        switch_in_call_id = call_ptr->call_id;
        break;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
    }
  }
  return switch_in_call_id;
}


/*===========================================================================

FUNCTION cmcall_drvcc_check_and_abort_drvcc_setup

DESCRIPTION
  To check if the call being ended shall abort the DRVCC setup
  If call being ended is switch out call ID, and DRVCC_SETUP_MSG has already been sent to 1X
  then End drvcc setup call over 1X.
  Send DRVCC FAIL to IMS.
  and HO_FAIL call event to CM clients for switch out call id.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_check_and_abort_drvcc_setup(cmcall_s_type *call_ptr)
{
  boolean abort_drvcc_setup = FALSE;
  boolean skip_switch_in_call_end = FALSE;
  cmcall_s_type *switch_in_call_ptr = NULL;
  cmcall_s_type *switch_out_call_ptr = NULL;
  cm_call_id_type switch_in_call_id = cmcall_drvcc_is_switch_out_call_id_ret_switch_in_call_id(call_ptr->call_id);
  
  if(switch_in_call_id != CM_CALL_ID_INVALID)
  {
    /* Call being passed is switch out call id */
    abort_drvcc_setup = TRUE;    
    switch_out_call_ptr = call_ptr;
    switch_in_call_ptr = cmcall_ptr(switch_in_call_id);
    if(switch_in_call_ptr == NULL)
    {
      CM_ERR_1("DRVCC:switch in call pointer is NULL while abort for id %d",switch_in_call_id);
      return;
    }
    CM_MSG_HIGH_2("DRVCC: Abort drvcc due to switch out call %d end, switch in id %d",call_ptr->call_id,switch_in_call_id);
  }
  /* currently checking for 3gpp2 only */
  if(cmcall_drvcc_is_3gpp2_drvcc_type(cmcall_drvcc_get_setup_call_drvcc_type(call_ptr)))
  {
    switch_in_call_ptr = call_ptr;
    switch_out_call_ptr = cmcall_ptr(switch_in_call_ptr->drvcc_parent_call_id);
    if(switch_out_call_ptr == NULL)
    {
      CM_ERR_1("DRVCC:switch out call pointer is NULL while abort for id %d",switch_in_call_ptr->drvcc_parent_call_id);
      return;
    }
    abort_drvcc_setup = TRUE;
    /* this part is for when switch in call is getting ended so no need to call cmcall_end for switch in call here */
    skip_switch_in_call_end = TRUE;
    CM_MSG_HIGH_1("DRVCC: Abort drvcc due to switch in call id %d end",call_ptr->call_id);
  }

  if(abort_drvcc_setup == TRUE)
  {
    /* send MC_DRVCC_END_F to MC to abort the DRVCC setup process 
    CM does not expect any response from 1X for the same. */
    cmcall_drvcc_send_drvcc_end(switch_in_call_ptr);
    
    /* DRVCC_FAIL_NOTIFY to IMS with correct ORM status */
    cmcall_drvcc_trigger_ho_fail_ipapp(switch_in_call_ptr->orm_sent_status, switch_in_call_ptr->asubs_id);

    if(skip_switch_in_call_end == FALSE)
    {
      /* end the switch in call*/
      cmcall_end(switch_in_call_ptr);
    }

    /* reset the drvcc cache this should be done after ending switch in call */
    cmcall_drvcc_cache_init();
    
    /* Send HO_FAIL call event to CM call clients with switch out call id */
    cmcall_event( switch_out_call_ptr, CM_CALL_EVENT_HO_FAIL );
  }
}


/*===========================================================================

FUNCTION cmcall_drvcc_abort_drvcc_setup_due_to_emerg_orig

DESCRIPTION
  Due to E911 call origination need to abort DRVCC process

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_abort_drvcc_setup_due_to_emerg_orig(void)
{
  cmcall_s_type *switch_in_call_ptr = NULL;
  cmcall_s_type *switch_out_call_ptr = NULL;
  cm_call_id_type switch_in_call_id = CM_CALL_ID_INVALID;
  cmcall_s_type     *call_ptr; /* Call object pointer */
  cm_iterator_type   call_itr; /* call Object Iterator */
  sys_drvcc_type_e_type drvcc_type = SYS_DRVCC_NONE;

  CM_MSG_HIGH_0("DRVCC: E911 originated, abort DRVCC");
  
  /* Loop through all calls to find out DRVCC switch in call */  
  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    drvcc_type = cmcall_drvcc_get_setup_call_drvcc_type(call_ptr);
    if(cmcall_drvcc_is_3gpp2_drvcc_type(drvcc_type))
    {
      switch_in_call_id = call_ptr->call_id;
      switch_in_call_ptr = call_ptr;
      switch_out_call_ptr = cmcall_ptr(switch_in_call_ptr->drvcc_parent_call_id);      
      break;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  CM_MSG_HIGH_1("DRVCC: Abort setup for switch in call %d",switch_in_call_id);
  
  if(switch_in_call_id != CM_CALL_ID_INVALID)
  {
    /* send MC_DRVCC_END_F to MC to abort the DRVCC setup process 
    CM does not expect any response from 1X for the same. */
    cmcall_drvcc_send_drvcc_end(switch_in_call_ptr);
    cmcall_end(switch_in_call_ptr);
     /* reset the drvcc cache this should be done after ending switch in call */
    cmcall_drvcc_cache_init();    
    if(switch_out_call_ptr != NULL)
    {
        /* Send HO_FAIL call event to CM call clients with switch out call id */
        cmcall_event( switch_out_call_ptr, CM_CALL_EVENT_HO_FAIL );
    }
  }
}



/*===========================================================================

FUNCTION cmcall_drvcc_cache_init

DESCRIPTION

  initialize drvcc cache, and will be used to reset as well once process completes.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_cache_init(void)
{
  cmcall_drvcc_cache.drvcc_type = SYS_DRVCC_NONE;
  cmcall_drvcc_cache.state = CM_DRVCC_STATE_NONE;
  memset(cmcall_drvcc_cache.stn_nr,0,CM_MAX_STN_DIGITS);
  CM_MSG_HIGH_0("DRVCC: cache init");
}

/*===========================================================================

FUNCTION cmcall_drvcc_fill_cache_from_setup_info

DESCRIPTION

  Fills DRVCC cache from drvcc setup info coming from IMS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_fill_cache_from_setup_info(cm_drvcc_setup_req_s_type *drvcc_setup_info_ptr)
{
  CM_ASSERT(drvcc_setup_info_ptr != NULL);
  cmcall_drvcc_cache_update_drvcc_type(drvcc_setup_info_ptr->drvcc_type);
  memscpy(cmcall_drvcc_cache.stn_nr,sizeof(cmcall_drvcc_cache.stn_nr),
          drvcc_setup_info_ptr->stn_nr, sizeof(drvcc_setup_info_ptr->stn_nr));
  cmcall_drvcc_cache.drvcc_allowed_in_roaming = drvcc_setup_info_ptr->drvcc_allowed_in_roaming;
}


/*===========================================================================

FUNCTION cmcall_drvcc_cache_update_drvcc_type

DESCRIPTION

  updaets drvcc type in drvcc cache.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_cache_update_drvcc_type(sys_drvcc_type_e_type drvcc_type)
{
  cmcall_drvcc_cache.drvcc_type = drvcc_type;
  CM_MSG_HIGH_1("DRVCC: drvcc_type in cache set to %d",drvcc_type);
}

/*===========================================================================

FUNCTION cmcall_drvcc_cache_get_drvcc_type

DESCRIPTION

  returns drvcc type in drvcc cache

DEPENDENCIES
  none

RETURN VALUE
  drvcc type in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
sys_drvcc_type_e_type cmcall_drvcc_cache_get_drvcc_type(void)
{
  return cmcall_drvcc_cache.drvcc_type;
}

/*===========================================================================

FUNCTION cmcall_drvcc_cache_update_state

DESCRIPTION

  Updates state of drvcc cache.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_cache_update_state(cm_drvcc_prog_state drvcc_state)
{
  cmcall_drvcc_cache.state = drvcc_state;
  CM_MSG_HIGH_1("DRVCC: state in cache set to %d",drvcc_state);
}


/*===========================================================================

FUNCTION cmcall_drvcc_cache_get_drvcc_state

DESCRIPTION

  returns state in drvcc cache

DEPENDENCIES
  none

RETURN VALUE
  drvcc state in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
cm_drvcc_prog_state cmcall_drvcc_cache_get_drvcc_state(void)
{
  CM_MSG_HIGH_1("DRVCC: cache state is %d",cmcall_drvcc_cache.state);
  return cmcall_drvcc_cache.state;
}

/*===========================================================================

FUNCTION cmcall_drvcc_cache_get_drvcc_allowed_in_roaming

DESCRIPTION

  returns drvcc allowed in roaming state from drvcc cache

DEPENDENCIES
  none

RETURN VALUE
  drvcc state in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_cache_get_drvcc_allowed_in_roaming(void)
{
  return cmcall_drvcc_cache.drvcc_allowed_in_roaming;
}


/*===========================================================================

FUNCTION cmcall_drvcc_is_drvcc_allowed_per_roam_status

DESCRIPTION

  returns TRUE if DRVCC is allowed or not per Roam status and custom home status

DEPENDENCIES
  none

RETURN VALUE
  drvcc state in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_is_drvcc_allowed_per_roam_status(sys_srv_status_e_type srv_status,
                        sys_roam_status_e_type roam_status,
                        boolean is_cdma_custom_home)
{  
  boolean ret_val = TRUE;
  if( sys_srv_status_is_srv(srv_status) )
  {
    if(roam_status > SYS_ROAM_STATUS_BANNER_OFF && roam_status < SYS_ROAM_STATUS_UNKNOWN)
    { 
      /* need to consider cdma custom home setting */
      if(!is_cdma_custom_home)
      {
        ret_val = FALSE;
      }
    }
    else if((roam_status != SYS_ROAM_STATUS_OFF) && (roam_status != SYS_ROAM_STATUS_UNKNOWN))
    {
      ret_val = FALSE;
    }
    else
    {
      ret_val = TRUE;
    }
  }
  CM_MSG_MED_1("DRVCC: allowed per roam status returned %d",ret_val);
  return ret_val;
}


#endif

