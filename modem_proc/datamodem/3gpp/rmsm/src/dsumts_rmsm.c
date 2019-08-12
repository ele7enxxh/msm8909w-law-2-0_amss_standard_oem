/*===========================================================================
                        D S U M T S _ R M S M . C 

DESCRIPTION
  The Data Services RM State Machine used for pdp-ip/pdp-ppp calls dispatcher 
  source file. This handles diapatching the events to the right UMTS RMSM 
  state machine (pdp-ip or pdp-ppp ) for pkt data calls initiated from laptop 
  or conneted terminals to support multimode data architecture. It contains
  the modules that are also common to both  PDP-IP and PDP-PPP sub state 
  machines.

  Note: There is only ONE RM state machine per interface. It is set to 
  operate for PDP-IP or PDP-PPP calls and kick started when the call is
  originated with ATD command. 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/dsumts_rmsm.c#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/12    vs     Added handle for RMSM PPP state 
11/21/12    ss     Fixed initialization of ATI interface at powerup for LTE
                   off configuration.
06/19/12    nd     MT PDP pkt call.
02/08/12    vs     Added support for 3GPP IPv6 over PPP
01/19/12    ss     Fixed issue in initializing entitlement cb function.
06/20/11    ttv    Merged silent redial support changes for UMTS tethered calls.
12/12/09    sa     Added support for ATCoP - Mode handler interface.
10/14/09    ss     CMI AU level cleanup.
11/15/07    asn    Added support for new feature MT-PDP terminating in TE
08/01/06    rr     Fixed Lint errors/warnings.
06/04/04    rc     Fixed Lint errors/warnings.
04/22/03    vsk    code review cleanup
04/18/03    vsk    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "sys.h"

#include "dstask.h"
#include "dstask_v.h"
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
#include "dsumts_rmsm.h"
#include "dsumts_rmsmi.h"
#include "dsumts_rmsm_ppp.h"
#include "dsumts_rmsm_ip.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_rmsm_ati.h"

#ifdef FEATURE_DATA_TE_MT_PDP
#include "dsumtspsmthdlr.h"
#endif /*FEATURE_DATA_TE_MT_PDP*/

#include "ds3gmgr.h"

#include "ps_sys_ioctl.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
typedef struct
{
  boolean result;
  uint32  cb_data;
} dsumts_rmsm_dun_ctrl_params_type;

/*===========================================================================
                            Forward Declarations
===========================================================================*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_INIT()

  DESCRIPTION
    This function initializes the UMTS RM state machine. Once initialized, the 
    state machine manages SIO iface for packet data calls on an appropriate 
    Um iface. 
   
   It also calls the pdp-ip and pdp-ppp specific sub state machine
   initializations.
   
   We just call the internal initialization function
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsm_init( void )
{
  boolean result = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  result = dsumts_rmsmi_init_all_instances();
  if (FALSE == result)
  {
    DS_3GPP_MSG0_ERROR("RMSM initialization failed ");
  }

  /* Initialize RMSM - ATI interface */
  ds_3gpp_rmsm_ati_init_all_instances();

  return result;
} /* dsumts_rmsm_init() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to DSUMTS_RMSM and enqueues the
    corresponding command to be processed in the DS task by the appropriate
    sub state machine (pdp-ip or pdp-ppp)

  PARAMETERS
    inst_ptr: RmSm instance to which the event is being posted.
    event:    Event that is being posted to the state machine

  RETURN VALUE
    None

  DEPENDENCIES
    dsumts_rmsm_init() should have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  dsumts_rmsm_event_type    event
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (inst_ptr == NULL)
  { 
    DATA_ERR_FATAL("Inst ptr is NULL");
    return;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return;
  }

  switch(rmsmi_info_ptr->sm_type )
  {
  case DSUMTS_RMSM_SM_TYPE_PDP_IP:
      ds_3gpp_rmsm_ip_common_post_event(inst_ptr,event);
      break;
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  case DSUMTS_RMSM_SM_TYPE_PDP_PPP:
      dsumts_rmsm_ppp_post_event(inst_ptr,event);
      break;
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
  default:
    DS_3GPP_MSG1_ERROR("invalid SM type :%d",rmsmi_info_ptr->sm_type);
  }

} /* dsumts_rmsm_post_event() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM in DS task and passes it 
    to the appropriate sub state machine to handle it (pdp-ip or pdp-ppp).

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    dsumts_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds_cmd_enum_type                    cmd;
  dsumts_rmsm_instance_type           inst;
  dsumts_rmsm_instance_type          *rmsm_inst_ptr = NULL;
  dsumts_rmsmi_info_type             *rmsmi_info_ptr = NULL;
  dsumts_rmsm_dun_ctrl_params_type   *rmsm_dun_ctrl_param_ptr = NULL;
  boolean                             result = FALSE;
  ds_3gpp_rmsm_ip_common_state_type   rmsm_ip_state;
  dsumts_rmsm_pppi_state_type         rmsm_ppp_state;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("NULL cmd_ptr or null payload ptr");
    return;
  }

  cmd  = ds_cmd_ptr->hdr.cmd_id;

  /*-------------------------------------------------------------------------
  Special Handling for ENTITLEMENT CHECK CMD as the process is the same for 
  both PPP and IP state machines
  ---------------------------------------------------------------------------*/
  if ( cmd == DS_CMD_3GPP_RMSM_DUN_CTRL_RSP)
  {
   rmsm_dun_ctrl_param_ptr
     = (dsumts_rmsm_dun_ctrl_params_type*)ds_cmd_ptr->cmd_payload_ptr;
    /*--------------------------------------------------------
     Extract information from cb data
     ---------------------------------------------------------*/
    inst.rm_dev_instance = (dsumts_rmsm_rm_dev_instance_type)
                             ((uint32)rmsm_dun_ctrl_param_ptr->cb_data);

    rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);
  
    if (rmsmi_info_ptr == NULL)
    {
      DATA_ERR_FATAL("RmSmi Info pointer is NULL");
      return;
    }

    if(rmsmi_info_ptr->sm_type == DSUMTS_RMSM_SM_TYPE_PDP_IP )
    {
      rmsm_ip_state = ds_3gpp_rmsm_ip_common_get_state(inst.rm_dev_instance);
      switch (rmsm_ip_state)
      {
        case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
          cmd = DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_COMPLETE;
          break;

        case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
          cmd = DS_CMD_3GPP_RMSM_REDIAL_CHECK_COMPLETE;
          break;

        default:
          DS_3GPP_MSG1_ERROR("dun_ctrl_resp called in invalid RMSM state: %d",
                                rmsm_ip_state);
          return ;
      }
    }
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
    else if (rmsmi_info_ptr->sm_type == DSUMTS_RMSM_SM_TYPE_PDP_PPP)
    {
      /* PPP is a v4 only so SM instance value hard code to 0 */
       rmsm_ppp_state = rmsmi_info_ptr->sm_info_ptr[0]->pdp_specific_info.pdp_ppp.state;
     
       switch (rmsm_ppp_state)
       {    
          case DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
            cmd = DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_COMPLETE;
            break;
  
          default:
            DS_3GPP_MSG1_ERROR("dun_ctrl_resp called in invalid RMSM state: %d",
                                rmsm_ppp_state);
        return;
       }
    }
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
    else
    {
      DS_3GPP_MSG0_ERROR("Invalid SM type");
      return;
    }
  }

  if( cmd == DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_COMPLETE )
  {
    rmsm_dun_ctrl_param_ptr
     = (dsumts_rmsm_dun_ctrl_params_type*)ds_cmd_ptr->cmd_payload_ptr;

    result = rmsm_dun_ctrl_param_ptr->result;

    /*----------------------------------------------------------
     Extract information from cb data
    ---------------------------------------------------------*/
    inst.rm_dev_instance = (dsumts_rmsm_rm_dev_instance_type)
                             ((uint32)rmsm_dun_ctrl_param_ptr->cb_data);

    dsumts_rmsmi_entitlement_check_complete
    (
      result, 
      inst.rm_dev_instance
    );
    return;
  }

  /*---------------------------------------------------------------------------
     REDIAL CHECK COMPLETE CMD is only currently supported for the IP RMSM.
   -----------------------------------------------------------------------------*/
  else if( cmd == DS_CMD_3GPP_RMSM_REDIAL_CHECK_COMPLETE )
  {
    rmsm_dun_ctrl_param_ptr
     = (dsumts_rmsm_dun_ctrl_params_type*)ds_cmd_ptr->cmd_payload_ptr;

    /*--------------------------------------------------------
     Extract information from cb data
     ---------------------------------------------------------*/
    inst.rm_dev_instance = (dsumts_rmsm_rm_dev_instance_type)
                             ((uint32)rmsm_dun_ctrl_param_ptr->cb_data);
    
    dsumts_rmsmi_redial_check_complete
    (
      rmsm_dun_ctrl_param_ptr->result,
      inst.rm_dev_instance
    );
    return;
  }

  else
  {
    rmsm_inst_ptr = (dsumts_rmsm_instance_type*)ds_cmd_ptr->cmd_payload_ptr;
    rmsmi_info_ptr
      = dsumts_rmsmi_get_rmsmi_info_ptr(rmsm_inst_ptr->rm_dev_instance);
  
    if (rmsmi_info_ptr == NULL)
    {
      DATA_ERR_FATAL("RmSmi Info pointer is NULL");
      return;
    }

    switch(rmsmi_info_ptr->sm_type )
    {
      case  DSUMTS_RMSM_SM_TYPE_PDP_IP:
        ds_3gpp_rmsm_ip_common_process_cmd(rmsm_inst_ptr,
                                           ds_cmd_ptr->hdr.cmd_id);
        break;

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
      case  DSUMTS_RMSM_SM_TYPE_PDP_PPP:
        dsumts_rmsm_ppp_process_cmd(rmsm_inst_ptr,
                                    ds_cmd_ptr->hdr.cmd_id);
        break;
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */

      default:
        DS_3GPP_MSG1_ERROR("Invalid SM Type :%d",rmsmi_info_ptr->sm_type);
        ASSERT(0);
     } 
  }
} /* dsumts_rmsm_process_cmd() */
#ifdef FEATURE_DATA_WCDMA_PS
/*===========================================================================
  FUNCTION dsumts_rmsm_process_sig()

  DESCRIPTION
    This function dequeues the next FC command in RMSM FC Q.

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    dsumts_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_process_sig ( void )
{
  dsumts_rmsmi_process_fc_cmd_q();
} /*dsumts_rmsm_process_sig*/
#endif /* FEATURE_DATA_WCDMA_PS */
/*===========================================================================
  FUNCTION DSUMTS_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function handles the dial stings for pkt data calls and initiates 
    the data call over the appropriate pkt iface.  This function is 
    registered with ATCOP ATD handler table.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
/*lint -esym(818, modifiers)*/
dsat_call_abort_cb_type dsumts_rmsm_dial_str_cb
(
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  dsat_dial_modifier_info_type  *modifiers, /* Dial sting modifiers  */
  sys_modem_as_id_e_type        subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Received dial string for Pkt data call");
  
  return (dsumts_rmsmi_handle_atd_on_rm( dsumts_rmsmi_get_next_rm_dev_instance(),
                                         dial_string, digit_mode,
                                         modifiers,
                                         subs_id));

} /* dsumts_rmsm_dial_str_cb() */
/*lint +esym(818, modifiers)*/

#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================
  FUNCTION DSUMTS_RMSM_LOCK_RM()

  DESCRIPTION
    Checks whether Rm is available to terminate a MT-PDP call. This function
    currenly assumes that only 1 RM Device can be active.

  PARAMETERS
    void

  RETURN VALUE
    TRUE:   successful in locking Rm
    FALSE:  unsuccessful

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RM Device can be active.
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsm_lock_rm
(
  void
)
{
  dsumts_rmsm_rm_dev_instance_type rm_dev_inst = 0; /* default Rm instance */
  dsumts_rmsmi_info_type *rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr( rm_dev_inst );

  ASSERT(rmsmi_info_ptr !=NULL);
  
  if ( rmsmi_info_ptr->is_locked )
  {
    DS_3GPP_MSG0_HIGH("RMSM_IS_RM_AVAIL: Rm not available!");
    return FALSE;
  }

  rmsmi_info_ptr->is_locked = TRUE;

  DS_3GPP_MSG0_HIGH("RMSM_IS_RM_AVAIL: Rm available!");

  return TRUE;
}/* dsumts_rmsm_lock_rm() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_UNLOCK_RM()

  DESCRIPTION
    Checks whether Rm is available to terminate a MT-PDP call. This function
    currenly assumes that only 1 RM Device can be active.

  PARAMETERS
    void

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_unlock_rm
(
  void
)
{
  dsumts_rmsm_rm_dev_instance_type rm_dev_inst = 0; /* default Rm instance */
  dsumts_rmsmi_info_type *rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr( rm_dev_inst );
  
  ASSERT(rmsmi_info_ptr !=NULL);

  if ( !rmsmi_info_ptr->is_locked )
  {
    DS_3GPP_MSG0_ERROR("RMSM_RELEASE_RM: Rm already released!");
    return;
  }

  rmsmi_info_ptr->is_locked = FALSE;

  DS_3GPP_MSG0_MED("RMSM_RELEASE_RM: Rm released!");

}/* dsumts_rmsm_unlock_rm() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_SET_MT()

  DESCRIPTION
    Used to set the is_mt flag in RMSM state table

  PARAMETERS
    int mt_index: The index of call in MT TIMER TBL    

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsm_set_mt
(
  uint8 mt_index
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst; /* default Rm instance */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if DSUMTS_RMSM_MAX_SM_INSTANCES > 1
  #error "This function is valid for DSUMTS_RMSM_MAX_INSTANCE = 1"
#endif

  inst.rm_dev_instance = 0;
  inst.sm_instance = 0;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr( inst.rm_dev_instance );

  if ( !rmsmi_info_ptr->is_locked )
  {
    DS_3GPP_MSG0_HIGH("RMSM_SET_MT: Rm not locked! cannot set is_mt flag");
    return FALSE;
  }

  if ( rmsmi_info_ptr->is_mt )
  {
    DS_3GPP_MSG0_HIGH("RMSM_SET_MT: is_mt already set!");
  }
  else
  {
    rmsmi_info_ptr->is_mt = TRUE;
  }

  rmsmi_info_ptr->mt_index = mt_index;
  DS_3GPP_MSG1_HIGH("RMSM_SET_MT: mt_index [%d]",mt_index);

  return TRUE;
}/* dsumts_rmsm_set_mt() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_RESET_MT()

  DESCRIPTION
    Used to reset the is_mt flag in RMSM state table

  PARAMETERS
    int mt_index: The index of call in MT TIMER TBL    

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsm_reset_mt
(
  uint8 mt_index
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst; /* default Rm instance */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
#if DSUMTS_RMSM_MAX_SM_INSTANCES > 1
  #error "This function is valid for DSUMTS_RMSM_MAX_INSTANCE = 1"
#endif

  inst.rm_dev_instance = 0;
  inst.sm_instance = 0;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr( inst.rm_dev_instance );

  if( NULL == rmsmi_info_ptr )
  {
    DS_3GPP_MSG1_HIGH("RMSM_RESET_MT: mt_index [%d]",
                       mt_index);
    return TRUE;
  }
  if ( !rmsmi_info_ptr->is_locked )
  {
    DS_3GPP_MSG0_HIGH("RMSM_RESET_MT: Rm not locked! cannot reset is_mt flag");
    return FALSE;
  }

  if ( !rmsmi_info_ptr->is_mt )
  {
    DS_3GPP_MSG0_HIGH("RMSM_RESET_MT: is_mt already reset!");
  }
  else
  {
    rmsmi_info_ptr->is_mt = FALSE;
  }

  rmsmi_info_ptr->mt_index = DSUMTSPS_MT_INVALID_TBL_INDEX;

  DS_3GPP_MSG1_HIGH("RMSM_RESET_MT: mt_index [%d]",
           mt_index);

  return TRUE;
}/* dsumts_rmsm_reset_mt() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_MT_RING_TE()

  DESCRIPTION
    This routine calls the RMSM INT routine to handle RING IND to TE.

  PARAMETERS
    int mt_index: The index of call in MT TIMER TBL    

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_mt_ring_te
(
  const dsumts_rmsm_mt_info_type *mt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_MED("RMSM_MT_RING_HDLR: Rcvd MT-PDP request");

  if ( !dsumts_rmsmi_mt_ring_te( mt_info_ptr ) ) 
  {
    DS_3GPP_MSG0_HIGH("RMSM_MT_RING_HDLR: cannot RING TE!");
    dsumts_rmsm_unlock_rm();
    dsumtsps_mt_umsm_post_ev( mt_info_ptr->mt_index, 
                              MT_UMSM_CALL_REJECT_EV, NULL );
  }
     
  return;

} /* dsumts_rmsm_mt_ring_hdlr() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_MT_AUTO_ANS_CB()

  DESCRIPTION
    This routine serves as the callback to auto-answer from TE

  PARAMETERS
    none    

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_mt_auto_ans_cb
(
  void
)
{
  dsat_pdp_answer_data_type ans_cb_data;

  DS_3GPP_MSG0_MED("MT_AUTO_ANS_CB: use default ATCOP profile to bringup call");

  /* Set up for default profile */
  memset( &ans_cb_data, 0, sizeof( dsat_pdp_answer_data_type ) );
  ans_cb_data.number_of_profiles = 0;

  dsumts_rmsm_mt_ans_cb( &ans_cb_data );
}/* dsumts_rmsm_mt_auto_ans_cb() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_MT_HANGUP_CB()

  DESCRIPTION
    This routine serves as the callback to ATH from TE

  PARAMETERS
    none    

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_mt_hangup_cb 
(
  void
)
{
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inst.rm_dev_instance = 0;
  inst.sm_instance = 0;

  DS_3GPP_MSG0_MED("RMSM_MT_HANGUP_CB: TE hung-up");
  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);

  ASSERT( rmsmi_info_ptr->is_locked );

  /* overloaded use of user_cancel to indicate ATH by TE */
  rmsmi_info_ptr->user_cancel = TRUE;

  DS_3GPP_MSG0_MED("RMSM_MT_HANGUP_CB: post RM_REJECT_MT_PKT_CALL_EV to RMSM");
  dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV );

  return;
}


/*===========================================================================
  FUNCTION dsumts_rmsm_mt_ans_cb()

  DESCRIPTION
    Called by DSAT in DS task context. In this function we need to find the profile 
    to be used to bringup the call (if specified by user +CGANS or ATA) then compare
    PDP address and type of MT call with profile and either perform MT REJECT or
    call dsumts_rmsmi_mt_handle_ans_on_rm() to bringup call

  PARAMETERS
    dsat_pdp_answer_data_type

  RETURN VALUE
    dsat_call_abort_cb_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsm_mt_ans_cb
(
  dsat_pdp_answer_data_type * ans_cb_data
)
{

  /*---------------------------------------------------------------------
    Validate input parameters
  ---------------------------------------------------------------------*/
  if ( ( ans_cb_data == NULL ) || 
       ( ans_cb_data->number_of_profiles > DS_UMTS_MAX_PDP_PROFILE_NUM ) 
     ) 
  {
    DATA_ERR_FATAL("RMSM_MT_ANS_CB: ans_cb_data validation failed!");
    return NULL;
  }
  
  DS_3GPP_MSG0_MED("RMSM_MT_ANS_CB: TE answered, num of profiles [%d], stop RING",
          ans_cb_data->number_of_profiles);

  ds3g_siolib_stop_alerting();

  DS_3GPP_MSG0_MED("RMSM_MT_ANS_CB: dereg dsat PDP and SIOLIB auto-ans hdlrs");
  
  dsat_deregister_pdp_handlers();

  ds3g_siolib_set_answer_callbacks( NULL );

  return ( dsumts_rmsmi_mt_handle_ans_on_rm( DSUMTS_RMSM_RM_DEV_INSTANCE_0, ans_cb_data ) );
}/* dsumts_rmsm_mt_ans_cb() */

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_DUN_CTRL_RESP()

  DESCRIPTION
    This function processes the result for entitlement check and silent
    redial.

  PARAMETERS
    arg_val_ptr - cb_data 
    ps_errno    - errno    

  RETURN VALUE
    int16

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int16 dsumts_rmsm_dun_ctrl_resp
(
  void                    *arg_val_ptr,
  int16                   *ps_errno
)
{
  ds_sys_ioctl_dun_ctrl_response_type  *resp_ptr = NULL;
  ds_cmd_type                          *cmd_ptr = NULL;
  dsumts_rmsm_dun_ctrl_params_type     *data_ptr = NULL;


  if ( (NULL == arg_val_ptr) || (NULL == ps_errno) )
  {
    DS_3GPP_MSG3_ERROR("Invaild ptr in dun_ctrl_cb: arg_val_ptr: %x, ps_errno: %x",
                        arg_val_ptr, ps_errno, 0);
    return -1;
  }
  
  resp_ptr = (ds_sys_ioctl_dun_ctrl_response_type *)arg_val_ptr;
#if 0
  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr((uint32)(resp_ptr->cb_data)); 
  if (rmsmi_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rmsmi info ptr is NULL");
    return -1;
  }
  else if(rmsmi_info_ptr->sm_type == DSUMTS_RMSM_SM_TYPE_PDP_IP )
  {
    rmsm_ip_state = ds_3gpp_rmsm_ip_common_get_state((uint32)(resp_ptr->cb_data));
    switch (rmsm_ip_state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
      cmd_id = DSUMTS_RMSM_ENTITLEMENT_CHECK_COMPLETE_CMD;
      break;
 
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      cmd_id = DSUMTS_RMSM_REDIAL_CHECK_COMPLETE_CMD;
      break;
 
    default:
      DS_3GPP_MSG1_ERROR("dun_ctrl_resp called in invalid RMSM state: %d",
                            rmsm_ip_state);
      *ps_errno = DS_EFAULT;
      return -1;
  }
  }
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  else if (rmsmi_info_ptr->sm_type == DSUMTS_RMSM_SM_TYPE_PDP_PPP)
  {

     rmsm_ppp_state = rmsmi_info_ptr->sm_info_ptr[sm_instance]->pdp_specific_info.pdp_ppp.state;
	 
     switch (rmsm_ppp_state)
     {	  
        case DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
          cmd_id = DSUMTS_RMSM_ENTITLEMENT_CHECK_COMPLETE_CMD;
          break;

        default:
          DS_3GPP_MSG1_ERROR("dun_ctrl_resp called in invalid RMSM state: %d",
                              rmsm_ppp_state);
      *ps_errno = DS_EFAULT;
      return -1;
  }
  }
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid SM type");
    return -1;
  }  
#endif

  cmd_ptr = ds_allocate_cmd_buf(sizeof(dsumts_rmsm_dun_ctrl_params_type));
  if( (cmd_ptr == NULL) ||(cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return -1;
  }
  
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_DUN_CTRL_RSP;
  data_ptr = (dsumts_rmsm_dun_ctrl_params_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->result = resp_ptr->allow_call;
  data_ptr->cb_data = (uint32)(resp_ptr->cb_data);
  ds_put_cmd(cmd_ptr);
  return 0;
} /* dsumts_rmsm_dun_ctrl_cb */

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS || FEATURE_DATA_LTE */

