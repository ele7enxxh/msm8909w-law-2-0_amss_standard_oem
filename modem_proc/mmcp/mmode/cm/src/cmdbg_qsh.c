/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   QSH D E B U G   M O D U L E

GENERAL DESCRIPTION
  This module contains the debug related functionality of call manager.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1991 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdbg_qsh.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/15   KC      Initial release.

===========================================================================*/

/*lint -save -e656 -e641
** Turn off enum to int / operationuses compatible enums
*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "cm_qsh_ext.h"
#include "cmcall.h"
#include "cmregprxi.h"
#include "cmidbg.h"
#include "cmph.h"
#include "cmdbg.h"
#include "qsh_ext.h"
#include "cmtaski.h"
#include "cmdbg_qsh.h"
#include "cause.h"



#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
/* holds all the information that is associated with QSH */
typedef struct cmdbg_qsh_s  {
  qsh_client_metric_cfg_s metric_cfg[SYS_MODEM_AS_ID_MAX][CM_QSH_METRIC_MAX];
  boolean event_action[CM_QSH_EVENT_MAX];
}cmdbg_qsh_s_type;

cmdbg_qsh_s_type  *cmdbg_qsh_ptr
( void 
)CM_API_IN_UNCOMPRESSED_IN_ELF;

void cmdbg_qsh_dump_dbg_buffer
( cm_dbg_cmregprx_buffer_s_type* dump_ptr 
)CM_API_IN_UNCOMPRESSED_IN_ELF;


void cmdbg_copy_cmreg_stack_info
(
cm_dbg_cmregprx_stack_info_s_type* stack_info_ptr
)CM_API_IN_UNCOMPRESSED_IN_ELF;


void cmdbg_qsh_cmregprx_dump_state_info
(
cm_dbg_cmregprx_info_s_type* dump_ptr
)CM_API_IN_UNCOMPRESSED_IN_ELF;


void cmdbg_qsh_copy_dump
(
qsh_client_cb_params_s *cb_params_ptr
)CM_API_IN_UNCOMPRESSED_IN_ELF;


void cmdbg_qsh_cb
(
qsh_client_cb_params_s *cb_params_ptr
)CM_API_IN_UNCOMPRESSED_IN_ELF;


/*===========================================================================
FUNCTION cmdbg_qsh_ptr

DESCRIPTION
  Return a pointer to the QSH object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmdbg_qsh_s_type  *cmdbg_qsh_ptr( void )
{

  static cmdbg_qsh_s_type cmdbg_qsh_local;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmdbg_qsh_local;

} /* cmdbg_qsh_ptr() */


/*===========================================================================

FUNCTION cmdbg_qsh_helper_determine_call_start_event_from_sys_mode

DESCRIPTION
  helper function to determine the specific call start event to send to QSH based on 
  sys_mode passed in

DEPENDENCIES
  none

RETURN VALUE
  the call start event to send to QSH

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_helper_determine_call_start_event_from_sys_mode(

    sys_sys_mode_e_type    sys_mode,
        /* the sys mode used to determine the QSH event */

    boolean                is_cs_call
        /* flag to check if call is CS or PS */

)
{
  cm_qsh_event_e call_qsh_event = CM_QSH_EVENT_NONE;
  
  switch(sys_mode)
  {
    case SYS_SYS_MODE_GSM:
      if(is_cs_call)
      {
        call_qsh_event = CM_QSH_EVENT_GSM_CS_CALL_START;
      }
      else /* call is a PS call */
      {
        call_qsh_event = CM_QSH_EVENT_GSM_PS_CALL_START;
      }
      
      break;
    
    case SYS_SYS_MODE_WCDMA:
      if(is_cs_call)
      {
        call_qsh_event = CM_QSH_EVENT_WCDMA_CS_CALL_START;
      }
      else /* call is a PS call */
      {
        call_qsh_event = CM_QSH_EVENT_WCDMA_PS_CALL_START;
      }
      break;
    
    case SYS_SYS_MODE_TDS:
      if(is_cs_call)
      {
        call_qsh_event = CM_QSH_EVENT_TDS_CS_CALL_START;
      }
      else /* call is a PS call */
      {
        call_qsh_event = CM_QSH_EVENT_TDS_PS_CALL_START;
      }
      break;

    case SYS_SYS_MODE_CDMA:
      if(is_cs_call)
      {
      call_qsh_event = CM_QSH_EVENT_CDMA_CS_CALL_START;
      }
      else /* call is a PS call */
      {
      call_qsh_event = CM_QSH_EVENT_CDMA_PS_CALL_START;
      }
      break;
    
    case SYS_SYS_MODE_HDR:
      call_qsh_event = CM_QSH_EVENT_HDR_PS_CALL_START;
      break;

    case SYS_SYS_MODE_LTE:
      /* only set to lte ps call start if is_cs_call is FALSE */
      if(!is_cs_call)
      {
        CM_MSG_HIGH_0("call start: last_sys is LTE and is_cs_call is FALSE");
        call_qsh_event = CM_QSH_EVENT_LTE_PS_CALL_START;
      }
      break;

    default:
      /* if the sys_mode is none of the above (i.e, no last sys reported), drop the event */
      call_qsh_event = CM_QSH_EVENT_NONE;
      break;
  }

  return call_qsh_event;
}


/*===========================================================================

FUNCTION cmdbg_qsh_helper_determine_call_end_event_from_sys_mode

DESCRIPTION
  helper function to determine the specific call event to send to QSH based on 
  sys_mode passed in. LTE is not checked because LTE is determined by mode info alone

DEPENDENCIES
  none

RETURN VALUE
  the call event to send to QSH

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_helper_determine_call_end_event_from_sys_mode(

    sys_sys_mode_e_type    sys_mode,
        /* the sys mode used to determine the QSH event */

    boolean                is_cs_call
        /* flag to check if call is CS or PS */

)
{
  cm_qsh_event_e call_qsh_event = CM_QSH_EVENT_NONE;
  
  switch(sys_mode)
  {
    case SYS_SYS_MODE_GSM:
      if(is_cs_call)
      {
        call_qsh_event = CM_QSH_EVENT_GSM_CS_CALL_END;
      }
      else /* call is a PS call */
      {
        call_qsh_event = CM_QSH_EVENT_GSM_PS_CALL_END;
      }
      
      break;
    
    case SYS_SYS_MODE_WCDMA:
      if(is_cs_call)
      {
        call_qsh_event = CM_QSH_EVENT_WCDMA_CS_CALL_END;
      }
      else /* call is a PS call */
      {
        call_qsh_event = CM_QSH_EVENT_WCDMA_PS_CALL_END;
      }
      break;
    
    case SYS_SYS_MODE_TDS:
      if(is_cs_call)
      {
        call_qsh_event = CM_QSH_EVENT_TDS_CS_CALL_END;
      }
      else /* call is a PS call */
      {
        call_qsh_event = CM_QSH_EVENT_TDS_PS_CALL_END;
      }
      break;

    case SYS_SYS_MODE_CDMA:
      if(is_cs_call)
      {
      call_qsh_event = CM_QSH_EVENT_CDMA_CS_CALL_END;
      }
      else /* call is a PS call */
      {
      call_qsh_event = CM_QSH_EVENT_CDMA_PS_CALL_END;
      }
      break;

    case SYS_SYS_MODE_HDR:
      call_qsh_event = CM_QSH_EVENT_HDR_PS_CALL_END;
      break;

    default:
      call_qsh_event = CM_QSH_EVENT_NONE;
      break;
  }

  return call_qsh_event;
}

/*===========================================================================

FUNCTION cmdbg_qsh_helper_get_last_sys_reported

DESCRIPTION
  helper function to get the cmss last sys reported field, depending on the call_ptr ss

DEPENDENCIES
  none

RETURN VALUE
  the last sys reported, depending on call_ptr ss

SIDE EFFECTS
  none

===========================================================================*/
static sys_sys_mode_e_type cmdbg_qsh_helper_get_last_sys_reported(

    cm_ss_e_type    ss
        /* the ss used for the call */

)
{
  cmss_s_type         *ss_ptr       = cmss_ptr();
  sys_sys_mode_e_type last_sys_mode = SYS_SYS_MODE_NONE;

  if(ss == CM_SS_NONE)
  {
    //CM_MSG_LOW_0("ss NONE is invalid; return SYS_SYS_MODE_NONE");
    return last_sys_mode;
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(ss == CM_SS_HYBR_3)
  {
    last_sys_mode = ss_ptr->hybr_3_gw_last_system_reported;
  }
  else
  #endif
  if(ss == CM_SS_HYBR_2)
  {
    last_sys_mode = ss_ptr->hybr_gw_last_system_reported;
  }
  else
  {
    last_sys_mode = ss_ptr->last_system_reported;
  }

  return last_sys_mode;
}


/*===========================================================================
FUNCTION cmdbg_qsh_is_metric_enabled

DESCRIPTION
  Checks if metric_id is enabled or not

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static boolean cmdbg_qsh_is_metric_enabled(sys_modem_as_id_e_type as_id,
                       cm_qsh_metric_e metric_id)
{
  cmdbg_qsh_s_type  *cm_qsh_ptr = cmdbg_qsh_ptr();
  boolean ret = FALSE;

  if(cm_qsh_ptr->metric_cfg[as_id][metric_id].action == QSH_METRIC_ACTION_START)
  {
    ret = TRUE;
  }
  return ret;
}


static boolean cmdbg_qsh_lost_3gpp_srv(sys_modem_as_id_e_type as_id,
                   uint8 stack_cnt)

{
  cmss_s_type          *ss_ptr        =  cmss_ptr();
  cm_mm_msim_ss_info_s_type *ss_info_ptr = &ss_ptr->new_srv_avl_info[as_id];
  boolean          ue_lost_3gpp_srv = FALSE;
  sys_sys_mode_e_type last_sys =  SYS_SYS_MODE_NONE;
  cm_ss_e_type ss = CM_SS_NONE;


  /* when current srv status moved to no-srv
   ** then we need to do special handling ,
   ** since curr sys will be updated none
   ** Hence use last_system_reported , which stores the last sys value */
  if (as_id == SYS_MODEM_AS_ID_1)
  {
    if (stack_cnt == 0)
    {
      ss = CM_SS_MAIN;
    }
    else if (stack_cnt == 1 &&
             ss_ptr->info.hybr_gw_changed_fields & CM_SS_GW_SRV_STATUS_MASK)
    {
      ss = CM_SS_HYBR_2;
    }
  }
  else if (as_id == SYS_MODEM_AS_ID_2
           && stack_cnt == 0)
  {
    ss = CM_SS_HYBR_3;
  }

  if (ss_info_ptr->stack_info[stack_cnt].changed_fields & CM_SS_EVT_SRV_STATUS_MASK
      && ss_info_ptr->stack_info[stack_cnt].srv_status == SYS_SRV_STATUS_NO_SRV)
  {
    switch(ss)
    {
      case CM_SS_MAIN:
        last_sys = ss_ptr->last_system_reported;
        break;

      case CM_SS_HYBR_2:
        last_sys = ss_ptr->hybr_gw_last_system_reported;
        break;

      case CM_SS_HYBR_3:
        last_sys = ss_ptr->hybr_3_gw_last_system_reported;
        break;

      default:
        break;
    }
  }
  if (CM_IS_MODE_3GPP(last_sys))
  {
    ue_lost_3gpp_srv = TRUE;
  }
  return ue_lost_3gpp_srv;
}


/*===========================================================================

FUNCTION cmdbg_is_srv_metrics_changed

DESCRIPTION
  Checks if any service related metrics changed
  Currently we are checking if any of the following changed only for a 3GPP system
  1) cell_info
  2) srv_status
  3) sys_mode
  4) sim_state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

boolean   cmdbg_is_srv_metrics_changed
                (sys_modem_as_id_e_type as_id)
{
  uint8 stack_cnt = 0;
  cmss_s_type          *ss_ptr        =  cmss_ptr();


  for(stack_cnt = 0; stack_cnt < ARR_SIZE(ss_ptr->new_srv_avl_info[as_id].stack_info); stack_cnt++)
  {

    cm_mm_msim_ss_stack_info_s_type *ss_stack_info_ptr = &ss_ptr->new_srv_avl_info[as_id].stack_info[stack_cnt];
    if ((CM_IS_MODE_3GPP(ss_stack_info_ptr->sys_mode)
         &&
         (ss_stack_info_ptr->changed_fields & CM_SS_EVT_SYS_MODE_MASK
          || ss_stack_info_ptr->changed_fields & CM_SS_EVT_CELL_INFO_MASK
          || ss_stack_info_ptr->changed_fields & CM_SS_EVT_SIM_STATE_MASK
          || ss_stack_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK))
        ||
        cmdbg_qsh_lost_3gpp_srv(as_id,stack_cnt))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_call_end_normal

DESCRIPTION
  helper API to determine if call end is normal.
  criteria for normal call end:
  1. call end reason is CLIENT_END ( for all calls)
  Note: 2 is not applicable to 3GPP calls
  2. call end reason is REL_NORMAL ( network release reason for 3GPP2 / IP calls but not for 3GPP calls)
  #ifdef FEATURE_CUST_1
  Note: 3 is only applicable for GWT CS Calls
  3. 3. call end reason is NETWORK_END (meaning CM got MT_DISC_IND. only for 3GPP calls), with cause code between 16 (NORMAL CALL CLEARING)
  #else
  Note: 3, 4,5 are only applicable for 3GPP CS calls
  3. call end reason is NETWORK_END (meaning CM got MT_DISC_IND. only for 3GPP calls), with cause code between 1 - 30
  4. no MT_DISC_IND and only MT_CALL_END_IND (call end reason is not CLIENT_END or NETWORK_END) with cause 17
  5. MT_DISC_IND, followed by MT_CALL_END_IND with cause 17
  #endif
  6. If call is of type GWT PS and released by the network and conditions 8 and 9
     dont apply
  7. If call mode info is LTE and end status is NONE 
     And neither conditions a or b are met.
     a) lte_info->esm_cause.valid is TRUE and 
         esm_cause is not NAS_ESM_REGULAR_DEACTIVATION 
     b) esm_cause.valid is FALSE and
        esm_local_cause.valid is TRUE and 
        esm_local_cause.local_cause is DRB_RELEASED_AT_RRC,  
        ESM_SYNC_UP_WITH_NW ,EMM_ATTACH_STARTED or ESM_DS_REJECTED_THE_CALL 
  8. if abnormal failure flag is set , treat as abnormal failure.
     This flag is set for GW PS calls when :
     a) PDP ACTIVATE REJ happens
     b) CM_PDP_DEACTIVATE_IND with cause NETWORK and reason not REGULAR_DEACTIVATION

  9. check if it has been marked as a normal call end due to
     a)CM_PDP_DEACTIVATE_REJ
     b) CM_PDP_DEACTIVATE_IND with cause type INTERNAL
     c) CM_PDP_DEACTIVATE_IND with cause NETWORK and reason REGULAR_DEACTIVATION

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if call end is normal call end. FALSE otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_call_end_normal(

  cmcall_s_type    *call_ptr
      /* pointer to call object */

)
{ 
  /* check for criteria 1 and 2 */
  if(call_ptr->end_status == CM_CALL_END_CLIENT_END ||
     call_ptr->end_status == CM_CALL_END_REL_NORMAL)
  { 
    return TRUE;
  }

  /* criteria 8: check for abnormal failure ( see desc for details)
  */
  if( call_ptr->call_debug_info & BM(CMCALL_DEBUG_INFO_ABNORMAL_FAILURE) )
  {
    CM_MSG_HIGH_0("dbg:abnormal failure");
    return  FALSE;
  }
  /* criteria 9: check if call has specifically been marked as normal
   call end 
  */
  if( call_ptr->call_debug_info & BM(CMCALL_DEBUG_INFO_NORMAL_CALL_END) )
  {
    CM_MSG_HIGH_0("dbg:normal call end");
    return  TRUE;
  }

  /* criteria 7 */
  #if defined (FEATURE_CM_LTE)
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE &&
      call_ptr->end_status == CM_CALL_END_NONE  )
  { 
    cm_lte_call_info_s_type *lte_info = &call_ptr->cmcall_mode_info.info.lte_info;
    if ( lte_info->esm_cause.valid &&
         lte_info->esm_cause.esm_cause !=NAS_ESM_REGULAR_DEACTIVATION ) 
    {
      CM_MSG_HIGH_1("deact bearer :esmcause abnormal %d",
                  lte_info->esm_cause.esm_cause
                  );
      return FALSE;
    }
    else if (!lte_info->esm_cause.valid &&
             lte_info->esm_local_cause.valid &&
             ( lte_info->esm_local_cause.local_cause == DRB_RELEASED_AT_RRC ||
               lte_info->esm_local_cause.local_cause == ESM_SYNC_UP_WITH_NW ||
               lte_info->esm_local_cause.local_cause == EMM_ATTACH_STARTED ||
	       lte_info->esm_local_cause.local_cause == ESM_DS_REJECTED_THE_CALL  )
             )
    {
      CM_MSG_HIGH_1("deact bearer :esm local cause abnormal %d",
                  lte_info->esm_local_cause.local_cause
                  );
      return FALSE;
    }
    return TRUE;
  }
  #endif //defined (FEATURE_CM_LTE)

  #ifdef CM_GW_SUPPORTED
  /* if the call is of type GW CS  and cc cause is present, check cause value along with call end status */
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS &&
      call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present)
  {

    #ifndef FEATURE_CUST_1
    /* check for criteria 3 */
    if(call_ptr->end_status == CM_CALL_END_NETWORK_END && 
       INRANGE(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value, 
               UNASSIGNED_CAUSE, RESPONSE_TO_STATUS_ENQUIRY))
    {
      return TRUE;
    }
    /* check for criteria 4 & 5 */
    if(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value == USER_BUSY)
    {
      return TRUE;
    }
    #else
    if(call_ptr->end_status == CM_CALL_END_NETWORK_END && 
       call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value == NORMAL_CALL_CLEARING)
    {
      return TRUE;
    }
    #endif
  }// GW call with cc cause present 

  /* criteria 6: check for GW PS calls ended by NW */
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS &&
      call_ptr->end_status == CM_CALL_END_NETWORK_END)
  {
      return TRUE;
  }// GW call with cc cause present 

  #endif
  
  return FALSE;
}
/*===========================================================================

FUNCTION cmdbg_qsh_is_3gpp_cs_call_end_normal

DESCRIPTION
  helper to determine if 3gpp call end is normal.
  criteria for normal call end:
  -call end reason is CLIENT_END
  -call end reason is NETWORK_END (meaning CM got MT_DISC_IND), with cause code between 1 - 31
  -no MT_DISC_IND and only MT_CALL_END_IND (call end reason is not CLIENT_END or NETWORK_END) with cause 17
  -MT_DISC_IND, followed by MT_CALL_END_IND with cause 17

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if call end is normal call end. FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_3gpp_cs_call_end_normal(

  cmcall_s_type    *call_ptr
      /* pointer to call object */

)
{ 
  if(call_ptr->end_status == CM_CALL_END_CLIENT_END)
  { 
    return TRUE;
  }

  #ifdef CM_GW_SUPPORTED
  /* if cause is present, check cause value along wtih call end status */
  if(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present)
    {
    if(call_ptr->end_status == CM_CALL_END_NETWORK_END && 
       INRANGE(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value, 
               UNASSIGNED_CAUSE, RESPONSE_TO_STATUS_ENQUIRY))
      {
      return TRUE;
      }

    if(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value == USER_BUSY)
      {
      return TRUE;
    }
  }
  #endif
  
  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_is_3gpp_cs_call_end_rx_disconnect

DESCRIPTION
  helper to determine if QSH event for 3gpp call end should be RX_DISCONNECT.
  criteria for RX_DISCONNECT:
  -CM got MT disc ind. This means call end reason is NETWORK_END
  -cause code is in range 34 - 127

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if event should be RX_DISCONNECT. FALSE otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_is_3gpp_cs_call_end_rx_disconnect(

  cmcall_s_type    *call_ptr
      /* pointer to call object */

)
{
  #ifdef CM_GW_SUPPORTED
  if(call_ptr->end_status == CM_CALL_END_NETWORK_END &&
     call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present &&
     INRANGE(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value,
             NORMAL_UNSPECIFIED, INTERWORKING_UNSPECIFIED))
      {
    return TRUE;
      }
  #endif

  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_is_3gpp_cs_call_end_tx_disconnect

DESCRIPTION
  helper to determine if QSH event for 3gpp call end should be TX_DISCONNECT.
  criteria for TX_DISCONNECT:
  -CM does not get MT disc ind (directly gets MT call end). This means call end reason is neither CLIENT_END nor NETWORK_END
  -cause code is 16

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if event should be TX_DISCONNECT. FALSE otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_is_3gpp_cs_call_end_tx_disconnect(

  cmcall_s_type    *call_ptr
      /* pointer to call object */

)
{
  #ifdef CM_GW_SUPPORTED
  if(call_ptr->end_status != CM_CALL_END_NETWORK_END && 
     call_ptr->end_status != CM_CALL_END_CLIENT_END && 
     call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present &&
     call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value == NORMAL_CALL_CLEARING)
      {
    return TRUE;
    }
  #endif

  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_is_3gpp_cs_call_end_rx_release

DESCRIPTION
  helper to determine if QSH event for 3gpp call end should be RX_RELEASE.
  criteria for RX_RELEASE:
  -CM does not get MT disc ind (directly gets MT call end). This means call end reason is neither CLIENT_END nor NETWORK_END
  -the cause code is not 16 or 17 (16 maps to TX_DISCONNECT, and 17 maps to NORMAL)

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if event should be RX_RELEASE. FALSE otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_is_3gpp_cs_call_end_rx_release(

  cmcall_s_type    *call_ptr
      /* pointer to call object */

)
{
  #ifdef CM_GW_SUPPORTED
  if(call_ptr->end_status != CM_CALL_END_NETWORK_END && 
     call_ptr->end_status != CM_CALL_END_CLIENT_END && 
     call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present &&
     INRANGE(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value, 
             UNASSIGNED_CAUSE, INTERWORKING_UNSPECIFIED) &&
     call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value != NORMAL_CALL_CLEARING &&
     call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value != USER_BUSY)
  {
    return TRUE;
  }
  #endif
  
  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_is_3gpp_cs_call_end_rx_rrc_release

DESCRIPTION
  helper to determine if QSH event for 3gpp call end should be RX_RRC_RELEASE.
  criteria for RX_RRC_RELEASE:
  -reject is present and
  a)reject type is AS_REJECT_CAUSE or
  b) reject type is MM_REJECT_CAUSE and rej value is 175 ( OTHER_CAUSE)
  
DEPENDENCIES
  none

RETURN VALUE
  TRUE, if event should be RX_RRC_RELEASE. FALSE otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_is_3gpp_cs_call_end_rx_rrc_release(

  cmcall_s_type    *call_ptr
      /* pointer to call object */

)
{
  #ifdef CM_GW_SUPPORTED
  ie_cm_cc_reject_s_type  *cc_reject_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject;
  if(cc_reject_ptr->present &&
      ( cc_reject_ptr->rej_type == AS_REJECT_CAUSE ||
        (cc_reject_ptr->rej_type == MM_REJECT_CAUSE  && cc_reject_ptr->rej_value == OTHER_CAUSE)
       )
    )
  {
    return TRUE;
  }
  #endif
  
  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_determine_call_end_evt_type

DESCRIPTION
  determine the call end event from either cause or rej type. Only applicable for 3GPP calls.
  To determine the event:
    call helper functions to determine if event is NORMAL, RX_DISC, TX_DISC, RX_RELEASE
    if reject type is present and value is AS_REJECT:
  	map it to event RX_RRC_RELEASE
    if neither cause or rej type can be mapped, map to event OTHERS

DEPENDENCIES
  none

RETURN VALUE
  the call end event, determined from either cause or rej type

SIDE EFFECTS
  none

===========================================================================*/
 sys_call_end_event_e_type cmdbg_qsh_determine_3gpp_call_end_evt_type(

    cmcall_s_type    *call_ptr

)
{
  sys_call_end_event_e_type event = SYS_CALL_END_EVENT_NONE;

  #ifdef CM_GW_SUPPORTED
  if( call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_GW_CS )
  {
    return SYS_CALL_END_EVENT_NONE;
  }

  /* Cause values are from GSM 04.08 Table 10.86 in cause.h */
  CM_MSG_HIGH_3("3gpp CS call end: call end status %d, cause present %d, cause %d", 
                call_ptr->end_status,
                call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present,
              call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value );

  CM_MSG_HIGH_2("3gpp CS call end:  rej present %d, rej_type %d", 
                 call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.present,
                call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_type);

  if(cmdbg_qsh_is_3gpp_cs_call_end_normal(call_ptr))
  {
    event = SYS_CALL_END_EVENT_NORMAL_CALL_END;
  }
  else if(cmdbg_qsh_is_is_3gpp_cs_call_end_rx_rrc_release(call_ptr))
  {
    event = SYS_CALL_END_EVENT_RX_RRC_RELEASE;
  }
  else if(cmdbg_qsh_is_is_3gpp_cs_call_end_rx_disconnect(call_ptr))
  {
    event = SYS_CALL_END_EVENT_RX_DISCONNECT;
  }
  else if(cmdbg_qsh_is_is_3gpp_cs_call_end_tx_disconnect(call_ptr))
  {
    event = SYS_CALL_END_EVENT_TX_DISCONNECT;
  }
  else if(cmdbg_qsh_is_is_3gpp_cs_call_end_rx_release(call_ptr)) 
  {
    event = SYS_CALL_END_EVENT_RX_RELEASE;
  }
  else
  {
    event = SYS_CALL_END_EVENT_OTHERS;
  }

  #endif
   
  CM_MSG_HIGH_1("3gpp CS call end: event is %d", event);
 
  return event;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_call_start_event_cdma_cs

DESCRIPTION
  helper to determine if the call start is CDMA CS.
  A call start is CDMA CS if the following conditions are met:
  -call type is not PS_DATA

DEPENDENCIES
  none

RETURN VALUE
  TRUE if call start is CDMA CS, FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_call_start_event_cdma_cs(

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

)
{
  if(call_ptr->call_type != CM_CALL_TYPE_PS_DATA)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION cmdbg_qsh_determine_3gpp2_call_end_evt_type

DESCRIPTION
  Determine the call end event for CDMA CS calls. ( this API does not support HDR )
  The orig failures reported by 1xcp are converted to call end status in CM.
  This call end status can be classified into 3 categories using this API
  a) RLF_OR_WEAK_SIGNAL
  b) NORMAL
  c) OTHERS 

DEPENDENCIES
  none

RETURN VALUE
  the call end event type, determined from call_ptr->end_status
 
SIDE EFFECTS
  none

===========================================================================*/
sys_call_end_event_e_type cmdbg_qsh_determine_3gpp2_call_end_evt_type(

    cmcall_s_type    *call_ptr
    /* pointer to call object */

)
{
  sys_call_end_event_e_type  event = SYS_CALL_END_EVENT_NONE;


  if( call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_CDMA )
  {
    return SYS_CALL_END_EVENT_NONE;
  }
  
  /* Map call end status to event type
  */
  switch (call_ptr->end_status)
  {
    case CM_CALL_END_NO_CDMA_SRV:
    case CM_CALL_END_ACC_FAIL:
    case CM_CALL_END_FADE:
    case CM_CALL_END_NO_RESPONSE_FROM_BS:
    case CM_CALL_END_MAX_ACCESS_PROBE:
    case CM_CALL_END_TIMEOUT_T40:
    case CM_CALL_END_TIMEOUT_T41:
    case CM_CALL_END_TIMEOUT_T42:
    case CM_CALL_END_FADE_T50_EXP:
    case CM_CALL_END_FADE_T51_EXP:
    case CM_CALL_END_FADE_RL_ACK_TIMEOUT:
    case CM_CALL_END_FADE_BAD_FL:
      event = SYS_CALL_END_EVENT_RLF_OR_WEAK_SIGNAL;
      break;

    case CM_CALL_END_CLIENT_END:
    case CM_CALL_END_NETWORK_END:
    case CM_CALL_END_REL_NORMAL:
      event = SYS_CALL_END_EVENT_NORMAL_CALL_END;
      break;

    default:
      event = SYS_CALL_END_EVENT_OTHERS;
      break;
  }

  CM_MSG_HIGH_2("1x call end: call_ptr->end_status=%d event=%d", call_ptr->end_status,event);
  
  return event;
} //cmdbg_qsh_determine_3gpp2_call_end_evt_type


/*===========================================================================

FUNCTION cmdbg_qsh_is_call_start_event_cdma_ps

DESCRIPTION
  helper to determine if the call start is CDMA PS.
  A call start is CDMA PS if the following conditions are met:
  -call type is PS_DATA
  -favored mode pref contains CDMA
  -favored mode pref does not contain HDR
  OR
  -favored mode pref has both CDMA and HDR, and there is CDMA service, but no HDR service

DEPENDENCIES
  none

RETURN VALUE
  TRUE if call start is CDMA PS, FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_call_start_event_cdma_ps(
   
    cmcall_s_type    *call_ptr
        /* pointer to a call object. */ 

)
  {
  cm_service_status_s_type srv_info;

  if( call_ptr->call_type != CM_CALL_TYPE_PS_DATA )
  {
    return FALSE;
  }

  /* check favored mode pref */
  if(cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref, CM_MODE_PREF_CDMA_ONLY))
  {
    if(!cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref, CM_MODE_PREF_HDR_ONLY))
    {
      return TRUE;
    }
    else
    {
      /* check the service -- first, check for CDMA srv, then HDR srv */
      cmss_get_service_status_per_stack(CM_SS_MAIN, &srv_info);
      if(srv_info.sys_mode == SYS_SYS_MODE_CDMA && sys_srv_status_is_srv(srv_info.srv_status))
      {
        /* check for HDR service - if there is no HDR service at this point, then return TRUE */
        cmss_get_service_status_per_stack(CM_SS_HYBR_1, &srv_info);
        if(srv_info.sys_mode == SYS_SYS_MODE_HDR && !sys_srv_status_is_srv(srv_info.srv_status))
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_call_start_event_hdr_ps

DESCRIPTION
  helper to determine if the call start is HDR PS.
  A call start is HDR PS if the following conditions are met:
  -call type is PS_DATA
  -favored mode pref does not contain CDMA
  OR
  -favored mode pref contains both CDMA and HDR
  -there is no CDMA service
 
DEPENDENCIES
  none

RETURN VALUE
  TRUE if call start is HDR PS, FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_call_start_event_hdr_ps(
   
    cmcall_s_type    *call_ptr
        /* pointer to a call object. */ 

)
{
  cm_service_status_s_type srv_info;

  if( call_ptr->call_type != CM_CALL_TYPE_PS_DATA )
  {
    return FALSE;
  }

  if(!cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref, CM_MODE_PREF_CDMA_ONLY))
  {
    return TRUE;
  }
  else if(cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref, CM_MODE_PREF_HDR_ONLY))
  { 
    /* favored mode pref contains both CDMA and HDR, so check the service */
    cmss_get_service_status_per_stack(CM_SS_MAIN, &srv_info);
    if(srv_info.sys_mode == SYS_SYS_MODE_CDMA && sys_srv_status_is_srv(srv_info.srv_status))
    {
      /* check for HDR service -- if there is HDR service, then return TRUE. else return FALSE */
      cmss_get_service_status_per_stack(CM_SS_HYBR_1, &srv_info);
      if(srv_info.sys_mode == SYS_SYS_MODE_HDR && sys_srv_status_is_srv(srv_info.srv_status))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      /* there is no CDMA service, so return TRUE */
      return TRUE;
    }
  }

  /* if favored mode pref doesn't contain cdma or hdr, return FALSE */
  return FALSE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_helper_determine_call_start_event_cdma_mode_info

DESCRIPTION
  helper to determine call start event for calls with mode info CDMA

DEPENDENCIES
  none

RETURN VALUE
  the call start event to send to QSH

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_helper_determine_call_start_event_cdma_mode_info(

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */ 

)
{
  if(cmdbg_qsh_is_call_start_event_cdma_cs(call_ptr))
  {
    return CM_QSH_EVENT_CDMA_CS_CALL_START;
  }
  if(cmdbg_qsh_is_call_start_event_hdr_ps(call_ptr))
  {
    return CM_QSH_EVENT_HDR_PS_CALL_START;
  }
  if(cmdbg_qsh_is_call_start_event_cdma_ps(call_ptr))
  {
    return CM_QSH_EVENT_CDMA_PS_CALL_START;
  }
  
  return CM_QSH_EVENT_NONE;
}


/*===========================================================================

FUNCTION cmdbg_qsh_determine_csfb_call_start

DESCRIPTION
  determine the CSFB call start, if applicable

DEPENDENCIES
  none

RETURN VALUE
  the CSFB call start event to send to QSH. if call is not CSFB, then event returned is NONE.

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_determine_csfb_call_start(

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

)
{
  switch(call_ptr->csfb_type)
  {
    case CM_CALL_CSFB_TYPE_1XCSFB:
      return CM_QSH_EVENT_1XCSFB_CALL_START;

    case CM_CALL_CSFB_TYPE_PPCSFB:
      return CM_QSH_EVENT_PPCSFB_CALL_START;

    default:
      return CM_QSH_EVENT_NONE;
  }
}


/*===========================================================================

FUNCTION cmdbg_qsh_is_cs_call

DESCRIPTION
  determine if call is a CS call

DEPENDENCIES
  none

RETURN VALUE
  TRUE if CS call
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmdbg_qsh_is_cs_call(

    cmcall_s_type    *call_ptr
    /* pointer to a call object. */

)
    {
      boolean is_cs_call = FALSE;
      
  if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS)
      {
        is_cs_call = TRUE;
      }
  else if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS)
      {
        is_cs_call = FALSE;
      }
  else /* mode info is NONE or CDMA; check call type to set is_cs_call flag */
  {
    if(call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
    {
      is_cs_call = FALSE;
    }
    else
    {
      is_cs_call = TRUE;
    }
  }

  return is_cs_call;
}
	  
/*===========================================================================

FUNCTION cmdbg_qsh_determine_call_start_event_from_mode_info

DESCRIPTION
  determine the specific call start event to send to QSH, based on mode info

DEPENDENCIES
  none

RETURN VALUE
  the call start event to send to QSH

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_determine_call_start_event_from_mode_info(

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

)
      {
  cm_qsh_event_e call_qsh_event = CM_QSH_EVENT_NONE;
  
  switch(call_ptr->cmcall_mode_info.info_type)
  {
    case CM_CALL_MODE_INFO_CDMA:
      call_qsh_event = cmdbg_qsh_helper_determine_call_start_event_cdma_mode_info(call_ptr);
      break;

    case CM_CALL_MODE_INFO_IP:
    {
      if(call_ptr->is_volte)
      {
        call_qsh_event = CM_QSH_EVENT_VOLTE_CALL_START;
      }
      break;
    }
    case CM_CALL_MODE_INFO_LTE:
      call_qsh_event = CM_QSH_EVENT_LTE_PS_CALL_START;
      break;

    /* for these 3 cases, determine event from last system reported */
    case CM_CALL_MODE_INFO_GW_CS:
    case CM_CALL_MODE_INFO_GW_PS:
    case CM_CALL_MODE_INFO_NONE:
    {
      call_qsh_event = cmdbg_qsh_helper_determine_call_start_event_from_sys_mode(
                         cmdbg_qsh_helper_get_last_sys_reported(call_ptr->ss),
                         cmdbg_qsh_is_cs_call(call_ptr));
      break;
    }

    default:
      call_qsh_event = CM_QSH_EVENT_NONE;
      break;
  }
  
  CM_MSG_HIGH_1("QSH call start event: %d", call_qsh_event);
  return call_qsh_event;

      }


/*===========================================================================

FUNCTION cmdbg_qsh_determine_call_end_event_from_mode_info

DESCRIPTION
  determine the specific call end event to send to QSH, based on mode info and RAT the call was made on

DEPENDENCIES
  none

RETURN VALUE
  the call end event to send to QSH

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_determine_call_end_event_from_mode_info(

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

)
{
  cm_qsh_event_e           call_qsh_event    = CM_QSH_EVENT_NONE;
  cm_call_mode_info_e_type info_type         = call_ptr->cmcall_mode_info.info_type;
  sys_sys_mode_e_type      call_sys_mode     = call_ptr->sys_mode;
  sys_sys_mode_e_type      orig_sent_on_mode = call_ptr->sr.orig_sent_on_mode;
  boolean                  is_cs_call        = FALSE;

  switch(info_type)
  {
    case CM_CALL_MODE_INFO_CDMA:
    case CM_CALL_MODE_INFO_GW_CS:
    case CM_CALL_MODE_INFO_GW_PS:
    case CM_CALL_MODE_INFO_NONE: /* for E911 call cases */
    { 
      is_cs_call = cmdbg_qsh_is_cs_call(call_ptr);
	  
      call_qsh_event = cmdbg_qsh_helper_determine_call_end_event_from_sys_mode(orig_sent_on_mode, is_cs_call);

      CM_MSG_HIGH_3("determine_call_end_event: determined event %d from orig_sent_on_mode %d, is_cs_call %d", 
                    call_qsh_event, orig_sent_on_mode, is_cs_call);

      if(call_qsh_event == CM_QSH_EVENT_NONE)
      {
        CM_MSG_HIGH_1("determine_call_end_event: unable to use orig_sent_on_mode; check call sys_mode %d",
                      call_sys_mode);

        call_qsh_event = cmdbg_qsh_helper_determine_call_end_event_from_sys_mode(call_sys_mode, is_cs_call);
      }

      if(call_qsh_event == CM_QSH_EVENT_NONE)
      {
        CM_MSG_HIGH_0("determine_call_end_event: unable to use call sys_mode; check cmss_ptr");

        call_qsh_event = cmdbg_qsh_helper_determine_call_end_event_from_sys_mode(
                           cmdbg_qsh_helper_get_last_sys_reported(call_ptr->ss), is_cs_call);
      }

      break;
    }

    case CM_CALL_MODE_INFO_LTE:
      call_qsh_event = CM_QSH_EVENT_LTE_PS_CALL_END;
      break;

    default:
      call_qsh_event = CM_QSH_EVENT_NONE;
      break;
  }

  CM_MSG_HIGH_1("QSH call end event: %d", call_qsh_event);
  return call_qsh_event;
}


/*===========================================================================

FUNCTION cmdbg_qsh_map_call_end_event_to_call_setup_event

DESCRIPTION
  map call end event to call setup event

DEPENDENCIES
  none

RETURN VALUE
  the call setup event mapped from call end event

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_map_call_end_event_to_call_setup_event(

    cm_qsh_event_e    call_end_event
        /* call end event to map to a call setup event */

)
{
  cm_qsh_event_e call_setup_event = CM_QSH_EVENT_NONE;
  
  switch(call_end_event)
  {
    case CM_QSH_EVENT_CDMA_CS_CALL_END:
      call_setup_event = CM_QSH_EVENT_CDMA_CS_CALL_SETUP_FAILED;
      break;

    case CM_QSH_EVENT_CDMA_PS_CALL_END:
      call_setup_event = CM_QSH_EVENT_CDMA_PS_CALL_SETUP_FAILED;
      break;

    case CM_QSH_EVENT_HDR_PS_CALL_END:
      call_setup_event = CM_QSH_EVENT_HDR_PS_CALL_SETUP_FAILED;
      break;

    case CM_QSH_EVENT_GSM_CS_CALL_END:
      call_setup_event = CM_QSH_EVENT_GSM_CS_CALL_SETUP_FAILED;
      break;
    
    case CM_QSH_EVENT_GSM_PS_CALL_END:
      call_setup_event = CM_QSH_EVENT_GSM_PS_CALL_SETUP_FAILED;
      break;
    
    case CM_QSH_EVENT_WCDMA_CS_CALL_END:
      call_setup_event = CM_QSH_EVENT_WCDMA_CS_CALL_SETUP_FAILED;
      break;
    
    case CM_QSH_EVENT_WCDMA_PS_CALL_END:
      call_setup_event = CM_QSH_EVENT_WCDMA_PS_CALL_SETUP_FAILED;
      break;
    
    case CM_QSH_EVENT_TDS_CS_CALL_END:
      call_setup_event = CM_QSH_EVENT_TDS_CS_CALL_SETUP_FAILED;
      break;
    
    case CM_QSH_EVENT_TDS_PS_CALL_END:
      call_setup_event = CM_QSH_EVENT_TDS_PS_CALL_SETUP_FAILED;
      break;
    
    case CM_QSH_EVENT_LTE_PS_CALL_END:
      call_setup_event = CM_QSH_EVENT_LTE_PS_CALL_SETUP_FAILED;
      break;
    
    default:
      /* if no mapping exists, return the call end event passed in */
      call_setup_event = call_end_event;
      break;
  }

  return call_setup_event;
}


/*===========================================================================

FUNCTION cmdbg_qsh_map_call_end_event_to_call_drop_event

DESCRIPTION
  map call end event to call drop event

DEPENDENCIES
  none

RETURN VALUE
  the call drop event mapped from call end event

SIDE EFFECTS
  none

===========================================================================*/
static cm_qsh_event_e cmdbg_qsh_map_call_end_event_to_call_drop_event(

    cm_qsh_event_e    call_end_event
        /* call end event to map to a call setup event */

)
{
  cm_qsh_event_e call_drop_event = CM_QSH_EVENT_NONE;
  
  switch(call_end_event)
  {
    case CM_QSH_EVENT_CDMA_CS_CALL_END:
      call_drop_event = CM_QSH_EVENT_CDMA_CS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_CDMA_PS_CALL_END:
      call_drop_event = CM_QSH_EVENT_CDMA_PS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_HDR_PS_CALL_END:
      call_drop_event = CM_QSH_EVENT_HDR_PS_CALL_DROP;
      break;

    case CM_QSH_EVENT_GSM_CS_CALL_END:
      call_drop_event = CM_QSH_EVENT_GSM_CS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_GSM_PS_CALL_END:
      call_drop_event = CM_QSH_EVENT_GSM_PS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_WCDMA_CS_CALL_END:
      call_drop_event = CM_QSH_EVENT_WCDMA_CS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_WCDMA_PS_CALL_END:
      call_drop_event = CM_QSH_EVENT_WCDMA_PS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_TDS_CS_CALL_END:
      call_drop_event = CM_QSH_EVENT_TDS_CS_CALL_DROP;
      break;
    
    case CM_QSH_EVENT_TDS_PS_CALL_END:
      call_drop_event = CM_QSH_EVENT_TDS_PS_CALL_DROP;
      break;

    case CM_QSH_EVENT_LTE_PS_CALL_END:
      call_drop_event = CM_QSH_EVENT_LTE_PS_CALL_DROP;
      break;
	  
    default:
      /* if no mapping exists, return the call end event passed in */
      call_drop_event = call_end_event;
      break;
  }

  return call_drop_event;
}


static void cmdbg_qsh_send_call_start_evts(
	
    cm_call_event_e_type    call_event
        /* notify client list of this call event */,
    
    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

)
{
  qsh_client_event_notify_params_s event_notify_params;

  /* initialize event struct */
  qsh_client_event_notify_init(&event_notify_params);
  
  event_notify_params.client = QSH_CLT_CM;

  event_notify_params.id = CM_QSH_EVENT_NONE;

  if( call_event == CM_CALL_EVENT_GET_PDN_CONN_IND )
  {
    /* map get PDN conn ind to LTE PS call start */
    event_notify_params.id = CM_QSH_EVENT_LTE_PS_CALL_START;
    
    event_notify_params.subs_id = SYS_MODEM_AS_ID_1;

    CM_MSG_HIGH_1("call_event GET_PDN_CONN_IND maps to LTE PS call start event %d", event_notify_params.id);
	
    qsh_client_event_notify(&event_notify_params);
  }
  else if( call_event == CM_CALL_EVENT_ACT_BEARER_IND && 
           call_ptr != NULL && 
           !(call_ptr->call_debug_info & (BM(CMCALL_DEBUG_INFO_ACT_BEARER_CALL_ID_FOUND))) )
  {
    /* map act bearer ind to LTE PS call start, if it is for a new call */
    event_notify_params.id = CM_QSH_EVENT_LTE_PS_CALL_START;
    
    event_notify_params.subs_id = call_ptr->asubs_id;

    CM_MSG_HIGH_1("call_event ACT_BEARER_IND maps to LTE PS call start event %d", event_notify_params.id);
	
    qsh_client_event_notify(&event_notify_params);
  }
  else if( call_ptr != NULL  &&
	   ( call_event == CM_CALL_EVENT_ORIG || call_event == CM_CALL_EVENT_INCOM )
         )
  {
    /* determine if call is a CSFB call */
    if(event_notify_params.id == CM_QSH_EVENT_NONE)
    {
      event_notify_params.id = cmdbg_qsh_determine_csfb_call_start(call_ptr);
    }
    
    /* for non-CSFB calls, use mode info to determine the call start event */
    if(event_notify_params.id == CM_QSH_EVENT_NONE)
    {
      event_notify_params.id = cmdbg_qsh_determine_call_start_event_from_mode_info(call_ptr);
    }
    
    if(event_notify_params.id != CM_QSH_EVENT_NONE)
    {
      CM_MSG_HIGH_1("call start event: %d", event_notify_params.id);
      
      event_notify_params.subs_id = call_ptr->asubs_id;
      
      qsh_client_event_notify(&event_notify_params);
    }
  }
}
/*===========================================================================

FUNCTION cmdbg_qsh_send_call_end_drop_setup_evts(

DESCRIPTION
  API which determines whether to send call end events and if required, do so.
 
  1. For LTE PS calls, send call setup failure events if event is
     PDN_CONN_REJ_IND or PDN_CONN_FAIL_IND
  2a. If call_ptr is NULL , bail out at this point ( only LTE setup failures can have NUll ptrs)
  2b. If the call event is END REQ and end status is NETWORK END , proceed to step 3.
      If the call event is END and debug info indicates that QSH has not been updated regarding 
      this call end before, proceed to step 3.
      in all other cases, bail out.
  3.  Check for normal call end event for GWLTC based on call's sys mode
  4.  if the call is normal, send out the event id computed at step 3
  5. If the call is not normal, check the call's connect time to 
     determine if it should be CALL DROP ( non zero connect time) or 
     CALL SETUP FAILURE ( zero connect time)

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/


static void cmdbg_qsh_send_call_end_drop_setup_evts(
	
    cm_call_event_e_type    call_event
        /* notify client list of this call event */,
    
    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

)
{
  qsh_client_event_notify_params_s event_notify_params;
  
  /* special case to map PDN conn rej/fail indications to LTE call setup failure
     ** For these events it is possible for call_ptr to be NULL
     */
  if( call_event == CM_CALL_EVENT_PDN_CONN_REJ_IND
      || call_event == CM_CALL_EVENT_PDN_CONN_FAIL_IND )
  {
    CM_MSG_HIGH_1("send call evt: call_event %d maps to LTE call setup failure",
    call_event);
    
    /* initialize event struct */
    qsh_client_event_notify_init(&event_notify_params);
    
    event_notify_params.client = QSH_CLT_CM;
    
    /* map the call event to LTE call setup failure */ 
    event_notify_params.id = CM_QSH_EVENT_LTE_PS_CALL_SETUP_FAILED;
    
    event_notify_params.subs_id = SYS_MODEM_AS_ID_1;
    
    qsh_client_event_notify(&event_notify_params);
    return;
  }

  /* For all other event types, call ptr needs to be valid
  */
  if (call_ptr == NULL) 
  {
    return;
  }

  /* only send call end event for 
    1. CM_CALL_EVENT_END ( provided the debug event was not sent already) or
    2.  call end req due to a disconnect ind from NAS (END_REQ with status NETWORK_END)
    For all other events, bail out.
  */
  if(   ( call_event == CM_CALL_EVENT_END && 
         !(call_ptr->call_debug_info & BM(CMCALL_DEBUG_INFO_CALL_END_EVENT_SENT))
        ) || 
        ( call_event == CM_CALL_EVENT_END_REQ && 
          ( call_ptr->end_status == CM_CALL_END_NETWORK_END ||
		    call_ptr->end_status == CM_CALL_END_CLIENT_END
          )
        )
    )
  {
    /*CM_MSG_HIGH_2("send call evt: call_event %d, end_status %d", 
    call_event, call_ptr->end_status);
    
    CM_MSG_HIGH_3("send call evt: connect_time %d, asubs_id %d, call_type %d",
    call_ptr->call_connect_time,call_ptr->asubs_id, call_ptr->call_type);
    
    CM_MSG_HIGH_3("send call evt: info_type %d, sys_mode %d, orig_sent_on_mode %d",
    call_ptr->cmcall_mode_info.info_type, call_ptr->sys_mode, call_ptr->sr.orig_sent_on_mode);*/
    
    /* update this for MT DISC IND scenarios where call end info such as CC cause is updated prior to
    ** call end
    */
    call_ptr->call_debug_info |= BM(CMCALL_DEBUG_INFO_CALL_END_EVENT_SENT);

    /* initialize event struct */
    qsh_client_event_notify_init(&event_notify_params);
    
    event_notify_params.client = QSH_CLT_CM;
    
    /* determine the specific call end event, based on mode info and RAT the call was made on */ 
    event_notify_params.id = cmdbg_qsh_determine_call_end_event_from_mode_info(call_ptr);
    
    /*CM_MSG_HIGH_1("send call evt: call end evt is %d", event_notify_params.id);*/

    /* map call end event to call drop or call setup failed event if
       end status is not a normal end status 
    */
    if( !cmdbg_qsh_is_call_end_normal(call_ptr))
    {
      if(call_ptr->call_connect_time > 0)
      {
        /* map the call end event to call drop event */
        event_notify_params.id = cmdbg_qsh_map_call_end_event_to_call_drop_event(event_notify_params.id);
        /*CM_MSG_HIGH_1("send call evt: call drop evt is %d", event_notify_params.id);*/
      }
      else if(call_ptr->call_connect_time == 0 && event_notify_params.id != CM_QSH_EVENT_NONE)
      {
        /* map the call end event to call setup event*/
        event_notify_params.id = cmdbg_qsh_map_call_end_event_to_call_setup_event(event_notify_params.id);
        /*CM_MSG_HIGH_1("send call evt: call setup evt is %d", event_notify_params.id);*/
      }
    } // not normal call end
    
    if(event_notify_params.id != CM_QSH_EVENT_NONE)
    {
      CM_MSG_HIGH_1("call end/drop/setup fail event: %d", event_notify_params.id);

      event_notify_params.subs_id = call_ptr->asubs_id;
	  
      qsh_client_event_notify(&event_notify_params);
    }
  } // call event == end
}


/*===========================================================================

FUNCTION cmdbg_qsh_send_volte_call_start_end_evts

DESCRIPTION
  Sends VOLTE call start/end events to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_qsh_send_volte_call_start_end_evts
(

    const cm_ext_volte_call_state_ind_s_type    *volte_state
        /* VOLTE state indication */

)
{
  qsh_client_event_notify_params_s event_notify_params;

  CM_MSG_HIGH_3("send QSH VOLTE call start/end: asubs_id %d, call_type %d, call_state %d",
                volte_state->asubs_id, volte_state->call_type, volte_state->call_state);

  /* initialize event struct */
  qsh_client_event_notify_init(&event_notify_params);
  
  event_notify_params.client = QSH_CLT_CM;

  event_notify_params.id = CM_QSH_EVENT_NONE;

  /* check the call type for voice, VT, or emerg */
  if(volte_state->call_type & (CM_CALL_TYPE_VOICE_MASK | 
                               CM_CALL_TYPE_VT_MASK |
                               CM_CALL_TYPE_EMERG_MASK))
  {
    /* check the call state */
    if(volte_state->call_state == CM_CALL_STATUS_START)
    {
      event_notify_params.id = CM_QSH_EVENT_VOLTE_CALL_START;
    }
    else if(volte_state->call_state == CM_CALL_STATUS_END)
    {
      event_notify_params.id = CM_QSH_EVENT_VOLTE_CALL_END;
    }
  }

  if(event_notify_params.id != CM_QSH_EVENT_NONE)
  {
    CM_MSG_HIGH_1("volte call event: %d", event_notify_params.id);
    
    event_notify_params.subs_id = volte_state->asubs_id;
    
    qsh_client_event_notify(&event_notify_params);
  }
}


/*===========================================================================

FUNCTION cmdbg_qsh_last_sent_srv_metric_ptr

DESCRIPTION
  Returns a pointer to the last sent srv info metric.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_qsh_metric_srv_info_s_type *cmdbg_qsh_last_sent_srv_metric_ptr(void)
{
  static cm_qsh_metric_srv_info_s_type cmdbg_qsh_last_sent_srv_metric_local;

  return &cmdbg_qsh_last_sent_srv_metric_local;
} /* cmdbg_qsh_last_sent_srv_metric_ptr */


/*===========================================================================

FUNCTION cmdbg_qsh_last_sent_serving_cell_metric_ptr

DESCRIPTION
  Returns a pointer to the last sent serving cell metric.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_qsh_metric_serving_cell_info_s_type  *cmdbg_qsh_last_sent_serving_cell_metric_ptr()
{
  static cm_qsh_metric_serving_cell_info_s_type cmdbg_qsh_last_sent_serving_cell_metric_local;
  
  return &cmdbg_qsh_last_sent_serving_cell_metric_local;
} /* cmdbg_qsh_last_sent_serving_cell_metric_ptr */


/*===========================================================================

FUNCTION cmdbg_qsh_dump_dbg_buffer

DESCRIPTION
  Dumps the cmregprx_debug_buffer

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmdbg_qsh_dump_dbg_buffer
(cm_dbg_cmregprx_buffer_s_type* dump_ptr)
{
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
  cmregprx_debug_buffer_s_type *cmreg_debug_buffer_ptr = cmregprx_get_dbg_buff_ptr();
  uint8 cnt = 0;

  dump_ptr->dbg_buf_idx = cmreg_debug_buffer_ptr->dbg_buf_idx;
  dump_ptr->stack_id_for_current_report = cmreg_debug_buffer_ptr->stack_id_for_current_report;
  dump_ptr->asubs_id_for_current_report = cmreg_debug_buffer_ptr->asubs_id_for_current_report;

  for(cnt=0;cnt<CM_DBG_MAX_DEBUG_BUFFER_SIZE;cnt++)
  {
    dump_ptr->dbg_buf[cnt].message_name = cmreg_debug_buffer_ptr->dbg_buf[cnt].message_name;
    dump_ptr->dbg_buf[cnt].ss = cmreg_debug_buffer_ptr->dbg_buf[cnt].ss;
    dump_ptr->dbg_buf[cnt].is_cmd = cmreg_debug_buffer_ptr->dbg_buf[cnt].is_cmd;
    dump_ptr->dbg_buf[cnt].cnt = cmreg_debug_buffer_ptr->dbg_buf[cnt].cnt;

    dump_ptr->dbg_buf[cnt].message_name = cmreg_debug_buffer_ptr->dbg_buf[cnt].message_name;
    dump_ptr->dbg_buf[cnt].ss = cmreg_debug_buffer_ptr->dbg_buf[cnt].ss;

    memscpy(dump_ptr->dbg_buf[cnt].cmregprx_state,
            sizeof(dump_ptr->dbg_buf[cnt].cmregprx_state),
            cmreg_debug_buffer_ptr->dbg_buf[cnt].cmregprx_state,
            sizeof(cmreg_debug_buffer_ptr->dbg_buf[cnt].cmregprx_state));

    memscpy(dump_ptr->dbg_buf[cnt].cmregprx_substate,
            sizeof(dump_ptr->dbg_buf[cnt].cmregprx_substate),
            cmreg_debug_buffer_ptr->dbg_buf[cnt].cmregprx_substate,
            sizeof(cmreg_debug_buffer_ptr->dbg_buf[cnt].cmregprx_substate));
  }
#endif
}


/*===========================================================================

FUNCTION cmdbg_copy_cmreg_stack_info

DESCRIPTION
  Dumps the CMREGPRX state

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmdbg_copy_cmreg_stack_info
(cm_dbg_cmregprx_stack_info_s_type* stack_info_ptr)
{
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
  uint8 stack_cnt;

  cmregprx_info_s_type* cmregprx_info_ptr = cmregprx_get_info_ptr();

  for(stack_cnt=0;stack_cnt < CM_DBG_NUM_OF_GW_STACKS;stack_cnt++)
  {
    stack_info_ptr[stack_cnt].state = cmregprx_info_ptr->stack_info[stack_cnt].state;
    stack_info_ptr[stack_cnt].substate = cmregprx_info_ptr->stack_info[stack_cnt].substate;
    stack_info_ptr[stack_cnt].reg_trans_id = cmregprx_info_ptr->stack_info[stack_cnt].reg_trans_id;
    stack_info_ptr[stack_cnt].mmoc_trans_id = cmregprx_info_ptr->stack_info[stack_cnt].mmoc_trans_id;
    stack_info_ptr[stack_cnt].is_gwl_subs_avail = cmregprx_info_ptr->stack_info[stack_cnt].is_gwl_subs_avail;
    stack_info_ptr[stack_cnt].is_resel_allowed = cmregprx_info_ptr->stack_info[stack_cnt].is_resel_allowed;
    stack_info_ptr[stack_cnt].cnt_cm_srv_req = cmregprx_info_ptr->stack_info[stack_cnt].cnt_cm_srv_req;
    stack_info_ptr[stack_cnt].stop_mode_reason = cmregprx_info_ptr->stack_info[stack_cnt].stop_mode_reason;
    stack_info_ptr[stack_cnt].ss = cmregprx_info_ptr->stack_info[stack_cnt].ss;
    stack_info_ptr[stack_cnt].as_id = cmregprx_info_ptr->stack_info[stack_cnt].as_id;
    stack_info_ptr[stack_cnt].nas_stack_id = cmregprx_info_ptr->stack_info[stack_cnt].nas_stack_id;
#ifdef FEATURE_CM_LTE
    stack_info_ptr[stack_cnt].lte_connected_mode = cmregprx_info_ptr->stack_info[stack_cnt].lte_connected_mode;
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
    stack_info_ptr[stack_cnt].wcdma_connected_mode = cmregprx_info_ptr->stack_info[stack_cnt].wcdma_connected_mode;
#endif
    stack_info_ptr[stack_cnt].is_rlf_reset_delayed = cmregprx_info_ptr->stack_info[stack_cnt].is_rlf_reset_delayed;
    stack_info_ptr[stack_cnt].sys_sel_pref_req_id = cmregprx_info_ptr->stack_info[stack_cnt].sys_sel_pref_req_id;
    stack_info_ptr[stack_cnt].pending_ue_mode = cmregprx_info_ptr->stack_info[stack_cnt].pending_ue_mode;
    stack_info_ptr[stack_cnt].pending_substate_srlte = cmregprx_info_ptr->stack_info[stack_cnt].pending_substate_srlte;
    stack_info_ptr[stack_cnt].ue_mode = cmregprx_info_ptr->stack_info[stack_cnt].ue_mode;
    stack_info_ptr[stack_cnt].is_ue_mode_substate_srlte = cmregprx_info_ptr->stack_info[stack_cnt].is_ue_mode_substate_srlte;
  }
#endif
}

/*===========================================================================

FUNCTION cmdbg_qsh_cmregprx_dump_state_info

DESCRIPTION
  Dumps the CMREGPRX state

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmdbg_qsh_cmregprx_dump_state_info
(cm_dbg_cmregprx_info_s_type* dump_ptr)
{
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
   cmregprx_info_s_type* cmregprx_info_ptr = cmregprx_get_info_ptr();

   dump_ptr->ds_pref = cmregprx_info_ptr->ds_pref;
   dump_ptr->device_mode = cmregprx_info_ptr->device_mode;
   dump_ptr->ds_stat_chgd_trans_id = cmregprx_info_ptr->ds_stat_chgd_trans_id;
   dump_ptr->mmoc_trans_id = cmregprx_info_ptr->mmoc_trans_id;
   dump_ptr->subs_chgd_trans_id = cmregprx_info_ptr->subs_chgd_trans_id;
   dump_ptr->unique_trans_id = cmregprx_info_ptr->unique_trans_id;
   dump_ptr->is_waiting_for_plmn_block_resp =
            cmregprx_info_ptr->is_waiting_for_plmn_block_resp;
   dump_ptr->is_send_unblock_on_next_flpmn =
            cmregprx_info_ptr->is_send_unblock_on_next_flpmn;


   cmdbg_copy_cmreg_stack_info(dump_ptr->stack_info);
#endif
} /* mmocdbg_dump_state_info */


/*-----------------------------------------------------------------------------------------------
** QSH event-related functions
** -----------------------------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmdbg_qsh_send_call_evts

DESCRIPTION
  Sends call start/end/drop/setup failed events to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_qsh_send_call_evts
(

    cm_call_event_e_type    call_event
        /* notify client list of this call event */,

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */


)
{
  if(call_event == CM_CALL_EVENT_ORIG || 
     call_event == CM_CALL_EVENT_INCOM || 
     call_event == CM_CALL_EVENT_GET_PDN_CONN_IND || 
     call_event == CM_CALL_EVENT_ACT_BEARER_IND)
    {
    /* send call start event */
    cmdbg_qsh_send_call_start_evts(call_event, call_ptr);
    }
  else
    {
    /* send call end/drop/setup failed event */
    cmdbg_qsh_send_call_end_drop_setup_evts(call_event, call_ptr);
  }
}


/*===========================================================================

FUNCTION cmdbg_qsh_send_call_end_evts

DESCRIPTION
  Send call end event to QSH (the event is CM_QSH_EVENT_CALL_DROP).
  Include the start address of the corresponding metric in the payload of the event.
  Only send this event for CS call end.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_qsh_send_call_end_evts
(

    cmcall_s_type          *call_ptr,
        /* pointer to a call object. */

    uint8                  *start_addr
        /* start address of metric */
)
    {
  qsh_client_event_notify_params_s event_notify_params;

  /* only notify QSH for voice/emergency/VT/ext calls */
  if( call_ptr == NULL || 
     (call_ptr->call_type != CM_CALL_TYPE_VOICE && 
      call_ptr->call_type != CM_CALL_TYPE_EMERGENCY && 
      call_ptr->call_type != CM_CALL_TYPE_VT && 
      call_ptr->call_type != CM_CALL_TYPE_EXT) )
  {
    return;
}

  /* initialize event struct */
  qsh_client_event_notify_init(&event_notify_params);

  event_notify_params.client = QSH_CLT_CM;

  event_notify_params.id = CM_QSH_EVENT_CALL_DROP;

  event_notify_params.event_data = (void *)start_addr;

  event_notify_params.subs_id = call_ptr->asubs_id;

  QSH_LOG(QSH_CLT_CM, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH, 
          "CALL_DROP: as_id %d, start_addr 0x%x",
          call_ptr->asubs_id, start_addr);
  qsh_client_event_notify(&event_notify_params);
}


/*===========================================================================

FUNCTION cmdbg_qsh_chk_and_send_srv_evts

DESCRIPTION
  Sends srv change events to QSH
  Currently we are checking if any of the following changed only for a 3GPP system
  1) Full service
  2) No Service
  3) Limited service

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_qsh_chk_and_send_srv_evts(sys_modem_as_id_e_type as_id)
{
  uint8 stack_cnt = 0;
  cm_qsh_event_e evt = CM_QSH_EVENT_NONE;
  cmss_s_type          *ss_ptr        =  cmss_ptr();
  cm_mm_msim_ss_info_s_type *ss_info_ptr = &ss_ptr->new_srv_avl_info[as_id];
  qsh_client_event_notify_params_s event_notify_params;

  for(stack_cnt = 0; stack_cnt < ARR_SIZE(ss_info_ptr->stack_info); stack_cnt++)
  {
    /* Retrieve the last sys , in case on no-service */

    if (ss_info_ptr->stack_info[stack_cnt].changed_fields & CM_SS_EVT_SRV_STATUS_MASK)
    {
      if (CM_IS_MODE_3GPP(ss_info_ptr->stack_info[stack_cnt].sys_mode) ||
          cmdbg_qsh_lost_3gpp_srv(as_id,stack_cnt) == TRUE)
      {
        switch(ss_info_ptr->stack_info[stack_cnt].srv_status)
        {
          case SYS_SRV_STATUS_NO_SRV:
            evt = CM_QSH_EVENT_NO_SRV;
            QSH_LOG(QSH_CLT_CM, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH, 
                    "NO_SRV: as_id %d", ss_info_ptr->asubs_id);
            break;

          case SYS_SRV_STATUS_LIMITED:
            evt = CM_QSH_EVENT_LTD_SRV;
            QSH_LOG(QSH_CLT_CM, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH, 
                    "LTD_SRV: as_id %d", ss_info_ptr->asubs_id);
            break;

          case SYS_SRV_STATUS_SRV:
            evt = CM_QSH_EVENT_FULL_SRV;
            QSH_LOG(QSH_CLT_CM, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH, 
                    "FULL_SRV: as_id %d", ss_info_ptr->asubs_id);
            break;

          default:
            evt = CM_QSH_EVENT_NONE;
            break;
        }
        if(evt != CM_QSH_EVENT_NONE)
        {
          /*CM_MSG_HIGH_2("send srv evt to qsh %d %d",evt,ss_info_ptr->asubs_id);*/

          /* initialize event struct */
          qsh_client_event_notify_init(&event_notify_params);

          event_notify_params.client = QSH_CLT_CM;

          event_notify_params.id = evt;

          event_notify_params.subs_id = as_id;

          qsh_client_event_notify(&event_notify_params);
        }
      }
    }
  }
}


/*-----------------------------------------------------------------------------------------------
** QSH metric-related functions
** -----------------------------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmdbg_update_srv_metrics

DESCRIPTION
  Update srv change Metrics in QSH buffer. Related metric is CM_QSH_METRIC_SRV_INFO

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmdbg_update_srv_metrics
                (sys_modem_as_id_e_type as_id,
                 cm_qsh_metric_srv_info_s_type *metrics_ptr)
{
  uint8 stack_cnt = 0;
  cmss_s_type          *ss_ptr        =  cmss_ptr();

  for(stack_cnt = 0; stack_cnt < ARR_SIZE(ss_ptr->new_srv_avl_info[as_id].stack_info); stack_cnt++)
  {
    cm_mm_msim_ss_stack_info_s_type *ss_stack_info_ptr = &ss_ptr->new_srv_avl_info[as_id].stack_info[stack_cnt];

    if (CM_IS_MODE_3GPP(ss_stack_info_ptr->sys_mode)
        ||
        cmdbg_qsh_lost_3gpp_srv(as_id,stack_cnt))
    {
      memscpy(metrics_ptr->plmn,sizeof(metrics_ptr->plmn),
              ss_stack_info_ptr->cell_info.plmn_id.identity,
              sizeof(ss_stack_info_ptr->cell_info.plmn_id.identity));

      metrics_ptr->srv_status = ss_stack_info_ptr->srv_status;
      metrics_ptr->sys_mode = ss_stack_info_ptr->sys_mode;
      metrics_ptr->sim_state = ss_stack_info_ptr->mode_info.gw_info.sim_state;

      if(metrics_ptr->sim_state == SYS_SIM_STATE_AVAILABLE)
      {
        /* SIM is available -- copy HPLMN from CMREGPRX */
        #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
        cmregprx_info_s_type* cmregprx_info_ptr = cmregprx_get_info_ptr();
		
        memscpy(&(metrics_ptr->hplmn),
                sizeof(metrics_ptr->hplmn),
                &(cmregprx_info_ptr->hplmn[as_id]),
                sizeof(cmregprx_info_ptr->hplmn[as_id]));
        #endif
      }
      else
      {
        /* SIM is not available -- set HPLMN to invalid value */
        metrics_ptr->hplmn[0] = 0xFF;
        metrics_ptr->hplmn[1] = 0xFF;
        metrics_ptr->hplmn[2] = 0xFF;
      }
    }
  }
}


/*===========================================================================

FUNCTION cmdbg_update_serving_cell_metrics

DESCRIPTION
  Update serving cell metrics in QSH buffer. Related metric is CM_QSH_METRIC_SERVING_CELL_INFO

DEPENDENCIES
  none

RETURN VALUE
  FALSE if there is no need to update metric
  TRUE if there is a valid 3GPP mode and metric values. This is not a 
  guarantee of update (since CM will still need to compare with the previous
  sent metric)

SIDE EFFECTS
  none

===========================================================================*/

static boolean cmdbg_update_serving_cell_metrics(

  sys_modem_as_id_e_type as_id,
      /* AS ID */

  cm_ss_e_type           cm_ss,
      /* ss where the info changed */

  cm_qsh_metric_serving_cell_info_s_type *metrics_ptr
      /* pointer to the serving cell metrics to update */
)
{
  uint8        stack_cnt = cmss_map_ss_to_stack_info(cm_ss);
  cmss_s_type          *ss_ptr        =  cmss_ptr();
  cm_mm_msim_ss_stack_info_s_type *ss_stack_info_ptr   =  NULL;


  ss_stack_info_ptr = &ss_ptr->new_srv_avl_info[as_id].stack_info[stack_cnt];


  if (!CM_IS_MODE_3GPP(ss_stack_info_ptr->sys_mode) || !sys_srv_status_is_srv(ss_stack_info_ptr->srv_status) )
    {
    return FALSE ;
  }

  /* update fields only for 3GPP RATs */
  metrics_ptr->sys_mode = ss_stack_info_ptr->sys_mode;
      memscpy(metrics_ptr->plmn,sizeof(metrics_ptr->plmn),
              ss_stack_info_ptr->cell_info.plmn_id.identity,
              sizeof(ss_stack_info_ptr->cell_info.plmn_id.identity));

      /* update LAC/TAC, RAC info */
      switch(ss_stack_info_ptr->sys_mode)
      {
        case SYS_SYS_MODE_GSM:
          metrics_ptr->lac_tac = ss_stack_info_ptr->cell_info.lac_id;
          metrics_ptr->rac = ss_stack_info_ptr->rat_mode_info.gsm_mode_info.rac_or_mme_code;
          break;
		
        case SYS_SYS_MODE_WCDMA:
          metrics_ptr->lac_tac = ss_stack_info_ptr->cell_info.lac_id;
          metrics_ptr->rac = ss_stack_info_ptr->rat_mode_info.wcdma_mode_info.rac_or_mme_code;
          break;
        
        case SYS_SYS_MODE_LTE:
          metrics_ptr->lac_tac = ss_stack_info_ptr->cell_info.tac;
          metrics_ptr->rac = ss_stack_info_ptr->rat_mode_info.lte_mode_info.rac_or_mme_code;
          break;
        
        case SYS_SYS_MODE_TDS:
          metrics_ptr->lac_tac = ss_stack_info_ptr->cell_info.lac_id;
          metrics_ptr->rac = ss_stack_info_ptr->rat_mode_info.tds_mode_info.rac_or_mme_code;
          break;
        
        default:
          //CM_MSG_LOW_1("lac/tac/rac not valid for sys mode %d", ss_stack_info_ptr->sys_mode);
          break;
      }

  return TRUE;

  /*
  CM_MSG_LOW_3("SERVING_CELL_INFO: plmn 0x%x 0x%x 0x%x", 
               metrics_ptr->plmn[0],metrics_ptr->plmn[1],metrics_ptr->plmn[2]);
  CM_MSG_LOW_3("SERVING_CELL_INFO: sys_mode %d, lac/tac %d, rac %d", 
               metrics_ptr->sys_mode, metrics_ptr->lac_tac, metrics_ptr->rac);
  */
}


/*===========================================================================

FUNCTION cmdbg_update_call_end_info_metrics

DESCRIPTION
  Update call end info metrics in QSH buffer.
  Related metric is CM_QSH_METRIC_CALL_END_INFO

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmdbg_update_call_end_info_metrics(

  cmcall_s_type                       *call_ptr,
	/* pointer to a call object. */

  cm_qsh_metric_call_end_info_s_type *metrics_ptr
    /* pointer to the metrics to update */
)
{
  cmss_s_type *ss_ptr = cmss_ptr();
  cm_mm_msim_ss_stack_info_s_type *ss_stack_info_ptr;

  /* check range of asubs_id */
  if( !INRANGE(call_ptr->asubs_id, SYS_MODEM_AS_ID_1, MAX_AS_IDS-1) )
  {
    return;
  }
  
  ss_stack_info_ptr = &ss_ptr->new_srv_avl_info[call_ptr->asubs_id].stack_info[0];

  /* update asubs id */
  metrics_ptr->asubs_id = call_ptr->asubs_id;

  /* update last system reported (from ss_ptr) */
  metrics_ptr->last_system_reported = cmdbg_qsh_helper_get_last_sys_reported( 
                                        cmph_map_subs_to_ss(call_ptr->asubs_id) );

  /* update srv_status, srv_domain, roam_status */
  metrics_ptr->srv_status = ss_stack_info_ptr->srv_status;
  metrics_ptr->srv_domain = ss_stack_info_ptr->srv_domain;
  metrics_ptr->roam_status = ss_stack_info_ptr->roam_status;
  
  /* update call type, call state, and call end reason (from call_ptr) */
  metrics_ptr->call_type = call_ptr->call_type;
  metrics_ptr->call_state = call_ptr->prev_call_state;
  metrics_ptr->call_end_reason = call_ptr->end_status;

  /*
  CM_MSG_LOW_4("CALL_END_INFO: as_id %d, last_sys %d, srv_status %d, srv_domain %d",
                metrics_ptr->asubs_id, metrics_ptr->last_system_reported, 
                metrics_ptr->srv_status, metrics_ptr->srv_domain);
  CM_MSG_LOW_4("CALL_END_INFO: roam_status %d, call_type %d, call_state %d, call end reas %d",
                metrics_ptr->roam_status, metrics_ptr->call_type, 
                metrics_ptr->call_state, metrics_ptr->call_end_reason);
  */

  #ifdef CM_GW_SUPPORTED
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS )
  {
    /* for 3gpp calls, update event and cause (also from call_ptr) */
    metrics_ptr->event_type = cmdbg_qsh_determine_3gpp_call_end_evt_type(call_ptr);
    metrics_ptr->cause = call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value;

    //CM_MSG_LOW_2("CALL_END_INFO: event_type %d, cause %d", metrics_ptr->event_type, metrics_ptr->cause);
  } else
  #endif
  if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA ) 
  {
    /* for 1x calls, update event type from call_ptr->end_status*/
    metrics_ptr->event_type = cmdbg_qsh_determine_3gpp2_call_end_evt_type(call_ptr);
  }
}


/*===========================================================================

FUNCTION cmdbg_chk_and_update_srv_metrics

DESCRIPTION
  Updates the metrics buffer and sends to QSH. Related metric is CM_QSH_METRIC_SRV_INFO

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_chk_and_update_srv_metrics
                (sys_modem_as_id_e_type as_id,
                 /* AS id */
                 cm_qsh_metrics_chg_type chg_type
                 /* densotes which metric changed */
                 )
{
  cm_qsh_metric_srv_info_s_type *metrics_ptr;
  static boolean dual_sim_state_sent = FALSE;
  cmph_s_type         *ph_ptr = cmph_ptr();
  qsh_client_metric_log_done_s cb_done;
  boolean ph_dual_sim_state = FALSE;
  cmdbg_qsh_s_type  *cm_qsh_ptr = cmdbg_qsh_ptr();
  qsh_client_metric_cfg_s *metric_cfg = &cm_qsh_ptr->metric_cfg[as_id][CM_QSH_METRIC_SRV_INFO];
  cm_qsh_metric_srv_info_s_type *last_sent_metric_ptr = cmdbg_qsh_last_sent_srv_metric_ptr();

  if(cmdbg_qsh_is_metric_enabled(as_id,CM_QSH_METRIC_SRV_INFO) == FALSE)
  {
    return;
  }

  metrics_ptr = (cm_qsh_metric_srv_info_s_type *)metric_cfg->start_addr;

  if (ph_ptr->active_subs & SYS_MODEM_AS_ID_1_MASK
                          & SYS_MODEM_AS_ID_2_MASK)
  {
    ph_dual_sim_state = TRUE;;
  }

  /* Dual SIM state dint change ,
   ** but the trigger is from PH-Event,
   ** we can ignore that , since nothing changed */
  if (chg_type == CM_QSH_METRICS_CHG_TYPE_PH_EVENT
      &&
      dual_sim_state_sent == ph_dual_sim_state)
  {
    return;
  }

  /* This API is called from SS event ,
   ** but None of the ineterested metrics change,
   ** we can ignore that */
  if (chg_type == CM_QSH_METRICS_CHG_TYPE_SS_EVENT &&
      cmdbg_is_srv_metrics_changed(as_id) == FALSE)
  {
    return;
  }

  if (metrics_ptr != NULL
      && metric_cfg->size_bytes >= sizeof(cm_qsh_metric_srv_info_s_type))
  {
    metrics_ptr->hplmn[0] = 0xFF;
    metrics_ptr->hplmn[1] = 0xFF;
    metrics_ptr->hplmn[2] = 0xFF;

    /* srv related metrics should be sent in below 2 cases
     ** 1) Dual SIM state changed
     **  2) SS related metrics changed */

    cmdbg_update_srv_metrics(as_id,metrics_ptr);

    metrics_ptr->dual_sim_state = ph_ptr->active_subs
                                  & SYS_MODEM_AS_ID_1_MASK
                                  & SYS_MODEM_AS_ID_2_MASK;

    /* set hdr of last metric to be same as current metric */
	memscpy(&(last_sent_metric_ptr->hdr), sizeof(qsh_metric_hdr_s),
			&(metrics_ptr->hdr), sizeof(qsh_metric_hdr_s));

    if( memcmp(last_sent_metric_ptr, metrics_ptr, sizeof(cm_qsh_metric_srv_info_s_type)) == 0 )
    {
      return;
    }
    else
    {
      /* if the info is different, then update last_sent_metric_ptr with the current metrics_ptr info */
      memscpy(last_sent_metric_ptr, sizeof(cm_qsh_metric_srv_info_s_type),
              metrics_ptr, sizeof(cm_qsh_metric_srv_info_s_type));
    }

    qsh_client_metric_log_done_init(&cb_done);

    cb_done.client = QSH_CLT_CM;
    cb_done.metric_id = CM_QSH_METRIC_SRV_INFO;
    cb_done.metric_context_id = metric_cfg->metric_context_id;
    cb_done.log_reason = QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL;

    metric_cfg->start_addr = qsh_client_metric_log_done(&cb_done);
  }

}


/*===========================================================================

FUNCTION cmdbg_chk_and_update_serving_cell_metrics

DESCRIPTION
  Updates the serving cell metrics buffer and sends to QSH. Related metric is CM_QSH_METRIC_SERVING_CELL_INFO

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_chk_and_update_serving_cell_metrics(

  sys_modem_as_id_e_type as_id,
      /* AS id */

  cm_ss_e_type           cm_ss
      /* ss where the info changed */

)
{
  cm_qsh_metric_serving_cell_info_s_type *metrics_ptr;
  qsh_client_metric_log_done_s            cb_done;
  cmdbg_qsh_s_type                       *cm_qsh_ptr = cmdbg_qsh_ptr();
  qsh_client_metric_cfg_s                *metric_cfg = 
                                           &cm_qsh_ptr->metric_cfg[as_id][CM_QSH_METRIC_SERVING_CELL_INFO];
  cm_qsh_metric_serving_cell_info_s_type *last_sent_metric_ptr = 
                                           cmdbg_qsh_last_sent_serving_cell_metric_ptr();
  
  if(cmdbg_qsh_is_metric_enabled(as_id,CM_QSH_METRIC_SERVING_CELL_INFO) == FALSE)
  {
    return;
  }
  
  metrics_ptr = (cm_qsh_metric_serving_cell_info_s_type *)metric_cfg->start_addr;
  
  if (metrics_ptr != NULL
      && metric_cfg->size_bytes >= sizeof(cm_qsh_metric_serving_cell_info_s_type))
  {
    memset(metrics_ptr,0,sizeof(cm_qsh_metric_serving_cell_info_s_type));
  
    /* Update the metric based on SS info. if there is no 3GPP system acquired,
    ** there is no need to update cell related metrics */
    if(!cmdbg_update_serving_cell_metrics(as_id, cm_ss, metrics_ptr))
    {
      /* No valid 3GPP mode. Hence no need to update cell metrics */
      return;
    }

    /* set hdr of last metric to be same as current metric */
    memscpy(&(last_sent_metric_ptr->hdr), sizeof(qsh_metric_hdr_s),
            &(metrics_ptr->hdr), sizeof(qsh_metric_hdr_s));
    
    if( memcmp(last_sent_metric_ptr, metrics_ptr, sizeof(cm_qsh_metric_serving_cell_info_s_type)) == 0 )
    {
      return;
    }
    else
    {
      /* if the info is different, then update last_sent_metric_ptr with the current metrics_ptr info */
      memscpy(last_sent_metric_ptr, sizeof(cm_qsh_metric_serving_cell_info_s_type),
              metrics_ptr, sizeof(cm_qsh_metric_serving_cell_info_s_type));
    }

    qsh_client_metric_log_done_init(&cb_done);
	
    cb_done.client = QSH_CLT_CM;
    cb_done.metric_id = CM_QSH_METRIC_SERVING_CELL_INFO;
    cb_done.metric_context_id = metric_cfg->metric_context_id;
    cb_done.log_reason = QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL;
    
    metric_cfg->start_addr = qsh_client_metric_log_done(&cb_done);
  }

}


/*===========================================================================

FUNCTION cmdbg_chk_and_update_call_end_info_metrics

DESCRIPTION
  Updates the call end info metrics buffer and sends to QSH.
  Related metric is CM_QSH_METRIC_CALL_END_INFO

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_chk_and_update_call_end_info_metrics(

  cm_call_event_e_type	  call_event,
      /* the call event */
  
  cmcall_s_type          *call_ptr
      /* pointer to a call object. */

)
{
  cm_qsh_metric_call_end_info_s_type  *metrics_ptr;
  qsh_client_metric_log_done_s         cb_done;
  cmdbg_qsh_s_type                    *cm_qsh_ptr = cmdbg_qsh_ptr();
  qsh_client_metric_cfg_s             *metric_cfg;
  uint8                               *call_end_start_addr = NULL;

  /* only notify QSH for voice/emergency/VT/ext calls */
  if( call_ptr == NULL || 
      (call_ptr->call_type != CM_CALL_TYPE_VOICE && 
      call_ptr->call_type != CM_CALL_TYPE_EMERGENCY && 
      call_ptr->call_type != CM_CALL_TYPE_VT &&
      call_ptr->call_type != CM_CALL_TYPE_EXT) )
  {
    return;
  }

  /* check range of asubs_id */
  if( !INRANGE(call_ptr->asubs_id, SYS_MODEM_AS_ID_1, MAX_AS_IDS-1) )
  {
    return;
  }

  metric_cfg = &cm_qsh_ptr->metric_cfg[call_ptr->asubs_id][CM_QSH_METRIC_CALL_END_INFO];

  metrics_ptr = (cm_qsh_metric_call_end_info_s_type *)metric_cfg->start_addr;

  /* only send metrics/event for call end or call end req due to a disconnect ind from NAS */
  if( ((call_event == CM_CALL_EVENT_END && !call_ptr->is_call_end_info_sent) || 
       (call_event == CM_CALL_EVENT_END_REQ && call_ptr->end_status == CM_CALL_END_NETWORK_END)) )
  {
    call_ptr->is_call_end_info_sent = TRUE;

    if( cmdbg_qsh_is_metric_enabled(call_ptr->asubs_id,CM_QSH_METRIC_CALL_END_INFO) && 
        metrics_ptr != NULL && 
        metric_cfg->size_bytes >= sizeof(cm_qsh_metric_call_end_info_s_type) )
    {
      //CM_MSG_LOW_1("CALL_END_INFO: updating metric, call event %d", call_event);
	  
      memset(metrics_ptr,0,sizeof(cm_qsh_metric_call_end_info_s_type));
      
      /* update call end metrics */
      cmdbg_update_call_end_info_metrics(call_ptr, metrics_ptr);
      
      qsh_client_metric_log_done_init(&cb_done);
      
      cb_done.client = QSH_CLT_CM;
      cb_done.metric_id = CM_QSH_METRIC_CALL_END_INFO;
      cb_done.metric_context_id = metric_cfg->metric_context_id;
      cb_done.log_reason = QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL;
      
      /* save the start addr of the metric */
      call_end_start_addr = metric_cfg->start_addr;
      
      metric_cfg->start_addr = qsh_client_metric_log_done(&cb_done);
    }

    /* send call end event to QSH */
    cmdbg_qsh_send_call_end_evts(call_ptr, call_end_start_addr);
  }
}


/*===========================================================================

FUNCTION cmdbg_qsh_copy_dump

DESCRIPTION
  Copies the DUMP to the pointer sent by QSH .

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_qsh_copy_dump(qsh_client_cb_params_s *cb_params_ptr)
{
  qsh_client_dump_collect_s *dump_params_ptr = &cb_params_ptr->action_params.dump_collect;


  /* check if necessary memory is available or not */
  if(dump_params_ptr->dump_iovec.size_bytes >=
       sizeof(cm_qsh_dump_tag_mini_s_type))
  {
    if(QSH_DUMP_TAG_ENABLED(dump_params_ptr->dump_tag_mask,CM_QSH_DUMP_TAG_MINI))
    {
      qsh_client_action_done_s action_done;
      cm_qsh_dump_tag_mini_s_type *mini_dump = (cm_qsh_dump_tag_mini_s_type*)dump_params_ptr->dump_iovec.addr;

      qsh_client_dump_tag_hdr_init(&mini_dump->hdr,
                                   CM_QSH_DUMP_TAG_MINI,
                                   sizeof(cm_qsh_dump_tag_mini_s_type));
      cmdbg_qsh_cmregprx_dump_state_info(&mini_dump->cmreg_info);

      cmdbg_qsh_dump_dbg_buffer(&mini_dump->dbg_buffer);

      qsh_client_action_done_init(&action_done);
      action_done.cb_params_ptr = cb_params_ptr;
      action_done.params.dump_collect.size_written_bytes
                = sizeof(cm_qsh_dump_tag_mini_s_type);
      action_done.action_mode_done = QSH_ACTION_MODE_DONE_SYNC;
      qsh_client_action_done(&action_done);
    }
  }
}


/*===========================================================================

FUNCTION cmdbg_qsh_generic_rpt_proc

DESCRIPTION
  Process Generic reports for QSH


DEPENDENCIES
  cm qsh object must have already been initialized with
  cmdbg_qsh_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_qsh_generic_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to generic reports */
)
{
  cm_generic_rpt_s_type *gen_rpt_ptr = (cm_generic_rpt_s_type *)rpt_ptr;
  cmdbg_qsh_s_type  *cm_qsh_ptr = cmdbg_qsh_ptr();
  qsh_client_metric_cfg_s *metric_cfg = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( gen_rpt_ptr != NULL );
  if(gen_rpt_ptr->hdr.cmd == CM_QSH_CFG_RPT)
  {
    qsh_client_cb_params_s *cb_params_ptr;
    qsh_client_action_done_s client_action_done;

    cb_params_ptr = gen_rpt_ptr->generic_rpt.qsh_cb_params;


    qsh_client_action_done_init(&client_action_done);
    client_action_done.cb_params_ptr = cb_params_ptr;
    client_action_done.action_mode_done = QSH_ACTION_MODE_DONE_ASYNC;
    qsh_client_action_done(&client_action_done);


    if(cb_params_ptr->action & QSH_ACTION_METRIC_CFG)
    {
      /* store the metric_cfg ,
       **  since QSH expects this to be sent ,
       **  when we update the metrics */
      metric_cfg = &cb_params_ptr->action_params.metric_cfg;
      cm_qsh_ptr->metric_cfg[metric_cfg->subs_id][metric_cfg->id] = *metric_cfg;
      /*CM_MSG_HIGH_3("QSH_ACTION_METRIC_CFG: type %d  %d  %d ",metric_cfg->action,
          metric_cfg->id,metric_cfg->subs_id);*/

      if(metric_cfg->action == QSH_METRIC_ACTION_START && metric_cfg->id == CM_QSH_METRIC_SRV_INFO)
      {
        /* push the metrics though there is no change for the first time */
        cmdbg_chk_and_update_srv_metrics(metric_cfg->subs_id,
                     /* AS id */
                     CM_QSH_METRICS_CHG_TYPE_SEND_INITIAL_METRIC
                     /* densotes which metric changed */
                     );
      }
      if(metric_cfg->action == QSH_METRIC_ACTION_START && metric_cfg->id == CM_QSH_METRIC_SERVING_CELL_INFO)
      {
        if(!cmph_is_subs_feature_mode_1x_sxlte(metric_cfg->subs_id))
        {
          /* push the metrics though there is no change for the first time */
          cmdbg_chk_and_update_serving_cell_metrics (metric_cfg->subs_id , cmph_map_subs_to_ss(metric_cfg->subs_id)  );

        }
        else
        {
          /* push the metrics though there is no change for the first time */
          cmdbg_chk_and_update_serving_cell_metrics (metric_cfg->subs_id , CM_SS_MAIN );
          cmdbg_chk_and_update_serving_cell_metrics (metric_cfg->subs_id , CM_SS_HYBR_2 );
        }
      }
    }
    if(cb_params_ptr->action & QSH_ACTION_EVENT_CFG)
    {
      cm_qsh_event_e event_id = cb_params_ptr->action_params.event_cfg.id;
      if(cb_params_ptr->action_params.event_cfg.action 
           == QSH_EVENT_ACTION_ENABLE)
      {
        /* this action controls to enable or disable the event */
        cm_qsh_ptr->event_action[event_id] = TRUE;
      }  
      else
      {
        cm_qsh_ptr->event_action[event_id] = FALSE;      
      }
      /*CM_MSG_HIGH_2("QSH_ACTION_EVENT_CFG: type %d  %d ",event_id,
          cm_qsh_ptr->event_action[event_id]);*/
    }

    /* Free allocated client message buffer */
    cm_mem_free (cb_params_ptr);

  }
}



/*===========================================================================

FUNCTION cmdbg_qsh_cb

DESCRIPTION
  Processes the QSH request .

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_qsh_cb(qsh_client_cb_params_s *cb_params_ptr)
{
  if(cb_params_ptr->action & QSH_ACTION_DUMP_COLLECT)
  {
    cmdbg_qsh_copy_dump(cb_params_ptr);
  }
  if(cb_params_ptr->action & QSH_ACTION_METRIC_CFG ||
     cb_params_ptr->action & QSH_ACTION_EVENT_CFG)
  {
    cm_generic_rpt_s_type *cm_generic_rpt_ptr;

    if ((cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal())!=NULL)
    {
      cm_generic_rpt_ptr->hdr.cmd = CM_QSH_CFG_RPT;
      cm_generic_rpt_ptr->generic_rpt.qsh_cb_params =
                      (void *)cm_mem_malloc(sizeof(qsh_client_cb_params_s));

      memscpy(cm_generic_rpt_ptr->generic_rpt.qsh_cb_params,
              sizeof(qsh_client_cb_params_s),
              cb_params_ptr,
              sizeof(qsh_client_cb_params_s));

      cm_generic_rpt(cm_generic_rpt_ptr);
    }
  }
}

#ifdef FEATURE_QSH_EVENT_METRIC
#define CMDBG_QSH_METRIC_DEFAULT_TEST_CONFIG_COUNT 6
#define CMDBG_QSH_METRIC_CALL_END_INFO_ELEMENT_COUNT 5
#define CMDBG_QSH_METRIC_SRV_INFO_ELEMENT_COUNT 50
#define CMDBG_QSH_METRIC_SERVING_CELL_INFO_ELEMENT_COUNT 50
qsh_ext_metric_cfg_s  cmdbg_qsh_metric_default_cfg_arr[CMDBG_QSH_METRIC_DEFAULT_TEST_CONFIG_COUNT];
#endif

/*===========================================================================

FUNCTION cmdbg_qsh_init

DESCRIPTION
  Initilize CM QSH interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_qsh_init()
{
  qsh_client_reg_s qsh_client;
  int as_id = 0;
  int metric_idx = 0;

  memset(cmdbg_qsh_ptr(),0,sizeof(cmdbg_qsh_s_type));

  /* disable all metrics during initialization by setting actions to STOP */
  for(as_id = 0; as_id < SYS_MODEM_AS_ID_MAX; as_id++)
  {
    for(metric_idx = 0; metric_idx < CM_QSH_METRIC_MAX; metric_idx++)
    {
      cmdbg_qsh_ptr()->metric_cfg[as_id][metric_idx].action = QSH_METRIC_ACTION_STOP;
    }
  }

  memset(cmdbg_qsh_last_sent_srv_metric_ptr(),0,sizeof(cm_qsh_metric_srv_info_s_type));

  memset(cmdbg_qsh_last_sent_serving_cell_metric_ptr(),0,sizeof(cm_qsh_metric_serving_cell_info_s_type));


  memset(cmdbg_qsh_last_sent_serving_cell_metric_ptr(),0,sizeof(cm_qsh_metric_serving_cell_info_s_type));

  qsh_client_reg_init(&qsh_client);

  qsh_client.client = QSH_CLT_CM;
  qsh_client.major_ver = CM_QSH_MAJOR_VER;
  qsh_client.minor_ver = CM_QSH_MINOR_VER;
  qsh_client.cb_action_support_mask = QSH_ACTION_DUMP_COLLECT
            | QSH_ACTION_METRIC_CFG | QSH_ACTION_EVENT_CFG;
  qsh_client.client_cb_ptr = cmdbg_qsh_cb;

#ifdef FEATURE_QSH_EVENT_METRIC
      /* Below test code enables 
       **  to test the QSH functionality .
       **  Ideally this code wont have any impact 
       **  in commercial builds */    
      qsh_client_metric_cfg_init(cmdbg_qsh_metric_default_cfg_arr,
                              CMDBG_QSH_METRIC_DEFAULT_TEST_CONFIG_COUNT);
    
      //service metric enable
      cmdbg_qsh_metric_default_cfg_arr[0].id = CM_QSH_METRIC_SRV_INFO;
      cmdbg_qsh_metric_default_cfg_arr[0].subs_id = SYS_MODEM_AS_ID_1;
      cmdbg_qsh_metric_default_cfg_arr[0].action = QSH_METRIC_ACTION_START;
      cmdbg_qsh_metric_default_cfg_arr[0].fifo.element_size_bytes = 
                                        sizeof(cm_qsh_metric_srv_info_s_type);
      cmdbg_qsh_metric_default_cfg_arr[0].fifo.element_count_total = 
                                    CMDBG_QSH_METRIC_SRV_INFO_ELEMENT_COUNT;
    
      cmdbg_qsh_metric_default_cfg_arr[1].id = CM_QSH_METRIC_SRV_INFO;
      cmdbg_qsh_metric_default_cfg_arr[1].subs_id = SYS_MODEM_AS_ID_2;
      cmdbg_qsh_metric_default_cfg_arr[1].action = QSH_METRIC_ACTION_START;
      cmdbg_qsh_metric_default_cfg_arr[1].fifo.element_size_bytes = 
                                        sizeof(cm_qsh_metric_srv_info_s_type);
      cmdbg_qsh_metric_default_cfg_arr[1].fifo.element_count_total = 
                                    CMDBG_QSH_METRIC_SRV_INFO_ELEMENT_COUNT;

      /* enable call end info metrics */
      cmdbg_qsh_metric_default_cfg_arr[2].id = CM_QSH_METRIC_CALL_END_INFO;
      cmdbg_qsh_metric_default_cfg_arr[2].subs_id = SYS_MODEM_AS_ID_1;
      cmdbg_qsh_metric_default_cfg_arr[2].action = QSH_METRIC_ACTION_START;
      cmdbg_qsh_metric_default_cfg_arr[2].fifo.element_size_bytes = 
                                        sizeof(cm_qsh_metric_call_end_info_s_type);
      cmdbg_qsh_metric_default_cfg_arr[2].fifo.element_count_total = 
                                    CMDBG_QSH_METRIC_CALL_END_INFO_ELEMENT_COUNT;

      cmdbg_qsh_metric_default_cfg_arr[3].id = CM_QSH_METRIC_CALL_END_INFO;
      cmdbg_qsh_metric_default_cfg_arr[3].subs_id = SYS_MODEM_AS_ID_2;
      cmdbg_qsh_metric_default_cfg_arr[3].action = QSH_METRIC_ACTION_START;
      cmdbg_qsh_metric_default_cfg_arr[3].fifo.element_size_bytes = 
                                        sizeof(cm_qsh_metric_call_end_info_s_type);
      cmdbg_qsh_metric_default_cfg_arr[3].fifo.element_count_total = 
                                    CMDBG_QSH_METRIC_CALL_END_INFO_ELEMENT_COUNT;

      /* enable serving cell info metrics */
      cmdbg_qsh_metric_default_cfg_arr[4].id = CM_QSH_METRIC_SERVING_CELL_INFO;
      cmdbg_qsh_metric_default_cfg_arr[4].subs_id = SYS_MODEM_AS_ID_1;
      cmdbg_qsh_metric_default_cfg_arr[4].action = QSH_METRIC_ACTION_START;
      cmdbg_qsh_metric_default_cfg_arr[4].fifo.element_size_bytes = 
                                        sizeof(cm_qsh_metric_serving_cell_info_s_type);
      cmdbg_qsh_metric_default_cfg_arr[4].fifo.element_count_total = 
                                    CMDBG_QSH_METRIC_SERVING_CELL_INFO_ELEMENT_COUNT;
    
      cmdbg_qsh_metric_default_cfg_arr[5].id = CM_QSH_METRIC_SERVING_CELL_INFO;
      cmdbg_qsh_metric_default_cfg_arr[5].subs_id = SYS_MODEM_AS_ID_2;
      cmdbg_qsh_metric_default_cfg_arr[5].action = QSH_METRIC_ACTION_START;
      cmdbg_qsh_metric_default_cfg_arr[5].fifo.element_size_bytes = 
                                        sizeof(cm_qsh_metric_serving_cell_info_s_type);
      cmdbg_qsh_metric_default_cfg_arr[5].fifo.element_count_total = 
                                    CMDBG_QSH_METRIC_SERVING_CELL_INFO_ELEMENT_COUNT;
    
      qsh_client.metric_info.metric_cfg_count = 
                        CMDBG_QSH_METRIC_DEFAULT_TEST_CONFIG_COUNT;
      qsh_client.metric_info.metric_cfg_arr_ptr = 
                                 &cmdbg_qsh_metric_default_cfg_arr[0];
  
      qsh_client.event_info.support_mask =
        1 << CM_QSH_EVENT_CALL_DROP |
        1 << CM_QSH_EVENT_FULL_SRV |
        1 << CM_QSH_EVENT_NO_SRV |
        1 << CM_QSH_EVENT_LTD_SRV |
        1 << CM_QSH_EVENT_WCDMA_CS_CALL_DROP |
        1 << CM_QSH_EVENT_WCDMA_CS_CALL_SETUP_FAILED |
        1 << CM_QSH_EVENT_WCDMA_PS_CALL_DROP |
        1 << CM_QSH_EVENT_WCDMA_PS_CALL_SETUP_FAILED |
        1 << CM_QSH_EVENT_TDS_CS_CALL_DROP |
        1 << CM_QSH_EVENT_TDS_CS_CALL_SETUP_FAILED |
        1 << CM_QSH_EVENT_TDS_PS_CALL_DROP |
        1 << CM_QSH_EVENT_TDS_PS_CALL_SETUP_FAILED |
        1 << CM_QSH_EVENT_LTE_PS_CALL_SETUP_FAILED |
        1 << CM_QSH_EVENT_GSM_CS_CALL_DROP |
        1 << CM_QSH_EVENT_GSM_CS_CALL_SETUP_FAILED |
        1 << CM_QSH_EVENT_GSM_PS_CALL_DROP |
        1 << CM_QSH_EVENT_GSM_PS_CALL_SETUP_FAILED;
#endif
  
#ifdef FEATURE_QSH_DUMP
      qsh_client.dump_info.max_size_bytes = sizeof(cm_qsh_dump_tag_mini_s_type);
#endif

  qsh_client_reg(&qsh_client);
}



#endif
