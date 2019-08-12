/*===========================================================================
                       D S U M T S _ R M S M _ P P P . C

DESCRIPTION
  The Data Services UMTS RM State Machine for pdp-ppp calls source file. 
  This state machine manages the Um and Rm interfaces for pdp-ppp type 
  pkt data calls initiated from laptop or conneted terminals 
  to support multimode data architecture.
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/dsumts_rmsm_ppp.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/07/12    nd     SIO port not freed after DUN is ended in entitlement check.
06/19/12    nd     MT PDP pkt call.
05/09/12    nd     Memory reduction effort in 3gpp.
02/08/12    vs     Added support for 3GPP IPv6 over PPP
05/09/11    asn    Adding support for new active Iface event 
06/20/11    ttv    Merged silent redial support changes for UMTS tethered calls.
04/01/11    ttv    Support for Q6 free floating.
03/04/11    sa     Fixed PPP DUN call when Arbitration feature is defined.
07/29/10    sa     Fixed DUN data rate reporting based on device capability.
07/12/10    sch    Configuring HDLC mode HDLC_A2_AUTONOMOUS in ppp config.
03/04/09    sa      AU level CMI modifications.
02/14/08    vk     Graceful termination of PDP-PPP calls.
12/06/07    vrk    Changed extended CONNECT - default data rate to 7.2Mbps
11/15/07    asn    Added support for new feature MT-PDP terminating in TE
11/11/07    vrk    Added support for returning extended result codes 
                   with CONNECT.
02/11/07    vk     Fixed RVCT 2.1 compiler warnings.
08/01/06    rr     Fixed Lint errors/warnings.
07/28/05    rc     Removed calls to ps_flow_go_null_ind() and 
                   ps_flow_activate_ind().
05/11/05    rr     Fixed Lint errors/warnings.
11/21/04    rc     Added code to register and deregister phys link callbacks
                   for phys link flow enable and disable events.
10/14/04    ar     Ensure SIO port allocated for external data call.
07/15/04    ifk    Replaced ppp_reg_event() with new PPP event architecture
06/04/04    rc     Fixed Lint errors/warnings.
01/31/04    usb    No need to disable Um iface ACL, routing takes care of 
                   exclusive use of iface in laptop call.
01/27/04    vsk    M-PDP support 
01/05/03    rc     Enable flow by clearing all masks while transitioning to
                   DSUMTS_RMSM_PPP_NULL_STATE state in order to accept next
                   call.
08/15/03    aku    Access graceful_dormant_close variable via ps_iface macro
07/14/03    rc     Set the Phys Link State appropriately on the SIO iface.
04/22/03    vsk    code review cleanup: updated comments & cleanup.
04/18/03    vsk    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "memory.h"
#include "err.h"
#include "msg.h"
#include <stringl/stringl.h>

#include "dstask.h"
#include "ps_iface.h"
#include "ds3gsiolib.h"
#include "dserrno.h"
#include "ps_ppp_netmodel.h" 
#include "ps_ppp.h"
#include "ds_system_heap.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#include "dsumts_rmsm.h"
#include "dsumts_rmsmi.h"
#include "dsumts_rmsm_ppp.h"

#include "ds_3gpp_pdn_context.h"
#include "ds_umts_pdn_context.h"
#ifdef FEATURE_DATA_TE_MT_PDP
#include "dsumtspsmthdlr.h"
#include "dsat_v.h"
#include "dsatetsictab.h"
#endif /*FEATURE_DATA_TE_MT_PDP*/
#include "ds_3gppi_utils.h"

#include "ps_sys_event.h"
#include "ds_sys.h"
#include "ds_sys_event.h"
#include "ds3gsiolib_ex.h"
#include "dsati.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*---------------------------------------------------------------------------
  Variables for configuration of PPP over SIO.  
---------------------------------------------------------------------------*/

LOCAL ppp_dev_opts_type *dsumts_rmsm_ppp_rm_ppp_config_ptr;

/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*---------------------------------------------------------------------------
  UMTS_RMSM state management and event processing functions
---------------------------------------------------------------------------*/

LOCAL void dsumts_rmsm_ppp_process_rm_wants_pkt_call_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void dsumts_rmsm_ppp_process_rm_ppp_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void dsumts_rmsm_ppp_process_um_iface_up_ev
(
   dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void dsumts_rmsm_ppp_process_um_iface_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void ds_3gpp_rmsm_ppp_process_entitlement_check_failed_cmd
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void dsumts_rmsm_ppp_transition_state
(
  dsumts_rmsmi_sm_info_type      *sm_ptr,
  dsumts_rmsm_pppi_state_type   new_state
);

#ifdef FEATURE_DATA_TE_MT_PDP
LOCAL void dsumts_rmsm_ppp_process_um_wants_mt_pkt_call_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void dsumts_rmsm_ppp_process_rm_reject_mt_pkt_call_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL void dsumts_rmsm_ppp_process_rm_um_locked_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

LOCAL ds3g_siolib_func_tbl_type dsumts_rmsm_ppp_siolib_func_tbl =
{
   // at_escape_sequence_handler
   NULL,
   
   // dtr_changed_sig_handler 
   NULL,
   
   // dtr_changed_sig_handler_ampd0 
   NULL,
   
   // at_return_to_online_data_handler 
   NULL,
   
   // at_return_to_online_data_complete_handler 
   NULL,
   
   // return_to_online_cmd_mode_complete_handler                                        
   NULL,
   
   // auto_answer_handler
   dsumts_rmsm_mt_auto_ans_cb
};

#endif /*FEATURE_DATA_TE_MT_PDP*/

LOCAL void dsumts_rmsm_ppp_process_um_ppp_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_UM_PPP_EVENT_HANDLER_CB()

  DESCRIPTION
    Callback function registered with UM PPP to receive desired state change
    notifications.

  PARAMETERS
    ppp_dev: PPP device (i.e. PPP_UM_SN_DEV)
    protocol: PPP protocol (i.e. LCP)
    ppp_event: PPP event that has occurred
    user_data: RMSM instance 
    session_handle: unused
    fail_reason: reason for PPP failure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_um_ppp_event_handler_cb
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,
  void*               user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
  dsumts_rmsmi_sm_info_type *sm_ptr;
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  dsumts_rmsm_instance_type inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_MED( "Recd %d ppp event", ppp_event);
    
  ASSERT( ppp_dev == PPP_UM_SN_DEV );

  sm_ptr = (dsumts_rmsmi_sm_info_type*)user_data;
  ASSERT(sm_ptr);

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  ASSERT(rmsmi_info_ptr);

  inst.sm_instance = sm_ptr->instance;
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  
  switch(ppp_event)
  {
    case PPP_DOWN_EV:
      dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_PPP_DOWN_EV );
      break;

    default:
      DS_3GPP_MSG1_ERROR( "Unexpected PPP event %d", ppp_event);
      ASSERT(0);
  }
} /* dsumts_rmsm_ppp_um_ppp_event_handler_cb() */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_SM_INIT

  DESCRIPTION
    This function initializes the PDP-PPP specific state machine information
    pointed by sm_specific_info

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
boolean dsumts_rmsm_ppp_sm_init( dsumts_rmsmi_sm_info_type *sm_ptr)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sm_ptr->pdp_specific_info.pdp_ppp.state = DSUMTS_RMSM_PPP_NULL_STATE;
  sm_ptr->pdp_specific_info.pdp_ppp.um_ppp_ev_handle = 
      ppp_event_alloc(
                       PPP_UM_SN_DEV,
                       dsumts_rmsm_ppp_um_ppp_event_handler_cb,
                       (void *)sm_ptr,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL
                     );

  if ( NULL == sm_ptr->pdp_specific_info.pdp_ppp.um_ppp_ev_handle ) 
  {
    DATA_ERR_FATAL("Couldn't get UM PPP event handle");
    return FALSE;
  }

  return TRUE;
} /* dsumts_rmsm_ppp_sm_init() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to PDP-PPP type RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    sm_ptr: The pointer to the UMTS_RMSM instance control block
    event: The event that is being posted to the state machine

  RETURN VALUE
    None

  DEPENDENCIES
    dsumts_rmsm_ppp_sm_init() should have been called first.
    The top level state machine post function checks if we the state machine
    is initialized in the first place

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ppp_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  dsumts_rmsm_event_type    event
 /*lint -esym(818,sm_ptr) */
)
{
  ds_cmd_type                *cmd_ptr = NULL;      /* DS Task cmd buf        */
  ds_cmd_enum_type            cmd_id;              /* Identifies the command */
  dsumts_rmsm_instance_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently
    in.
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case UMTS_RMSM_RM_WANTS_PKT_CALL_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL;
      break;

    case UMTS_RMSM_RM_PPP_UP_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_PPP_UP;
      break;

    case UMTS_RMSM_RM_PPP_DOWN_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_PPP_DOWN;
      break;

    case UMTS_RMSM_UM_IFACE_UP_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_IFACE_UP;
      break;

    case UMTS_RMSM_UM_IFACE_DOWN_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_IFACE_DOWN;
      break;

#ifdef FEATURE_DATA_TE_MT_PDP
    case UMTS_RMSM_UM_WANTS_MT_PKT_CALL_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_WANTS_MT_PKT_CALL;
      break;

    case UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_REJECT_MT_PKT_CALL;
      break;

      case UMTS_RMSM_UM_LOCKED_EV :
        cmd_id = DS_CMD_3GPP_RMSM_UM_LOCKED;
        break;

#endif /*FEATURE_DATA_TE_MT_PDP*/

    case UMTS_RMSM_UM_PPP_DOWN_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_PPP_DOWN;
      break;

    case UMTS_RMSM_ENTITLEMENT_CHECK_FAILED_EV:
      cmd_id = DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_FAIL;
      break;

    default:
      DS_3GPP_MSG3_ERROR( "Invalid UMTS RMSM SM instance:%d, Rm Dev instance:%d,"
                          "event %d, ignoring", 
                          inst_ptr->sm_instance,
                          inst_ptr->rm_dev_instance,
                          event);
      /*lint -e718 -e746 */
      ASSERT(0);
      /*lint +e718 +e746 */
      return;
  } /* switch(event) */

  cmd_ptr = ds_allocate_cmd_buf(sizeof(dsumts_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = cmd_id;
  data_ptr = (dsumts_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->rm_dev_instance = (int)inst_ptr->rm_dev_instance;
  data_ptr->sm_instance = (int)inst_ptr->sm_instance;
  ds_put_cmd( cmd_ptr );

} /* dsumts_rmsm_ppp_post_event() */
/*lint +esym(818,sm_ptr) */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM (for pdp-ppp calls)
    in DS task and calls an appropriate function to handle it

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ppp_process_cmd
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_cmd_enum_type  cmd
)
{
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (inst_ptr == NULL)
  {
    DATA_ERR_FATAL("Inst ptr is NULL");
    return;
  }

  sm_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_ptr == NULL)
  {
    DATA_ERR_FATAL("sm_ptr is NULL");
    return;
  }
  
  DS_3GPP_MSG2_HIGH( "Recd cmd %d in state %d", 
             cmd, sm_ptr->pdp_specific_info.pdp_ppp.state);

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently
    in.
  -------------------------------------------------------------------------*/
  switch( cmd )
  {
    case DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL:
      dsumts_rmsm_ppp_process_rm_wants_pkt_call_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_DOWN:
      dsumts_rmsm_ppp_process_rm_ppp_down_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_IFACE_UP:
      dsumts_rmsm_ppp_process_um_iface_up_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_IFACE_DOWN:
      dsumts_rmsm_ppp_process_um_iface_down_ev(sm_ptr);
      break;

#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_CMD_3GPP_RMSM_UM_WANTS_MT_PKT_CALL:
      dsumts_rmsm_ppp_process_um_wants_mt_pkt_call_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_REJECT_MT_PKT_CALL:
      dsumts_rmsm_ppp_process_rm_reject_mt_pkt_call_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_LOCKED:
      dsumts_rmsm_ppp_process_rm_um_locked_ev(sm_ptr);
      break;

#endif /*FEATURE_DATA_TE_MT_PDP*/

    case DS_CMD_3GPP_RMSM_UM_PPP_DOWN:
      dsumts_rmsm_ppp_process_um_ppp_down_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_FAIL:
      ds_3gpp_rmsm_ppp_process_entitlement_check_failed_cmd(sm_ptr);
      break;

    default:
      DS_3GPP_MSG1_ERROR( "Invalid RMSM cmd %d, ignoring", cmd);
      ASSERT(0);
      return;
  } /* switch( cmd ) */
} /* dsumts_rmsm_ppp_process_cmd() */
/*lint +esym(818,ds_cmd_ptr) */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_RM_WANTS_PKT_CALL_EV

  DESCRIPTION
    This function processes UMTS_RMSM_RM_WANTS_PKT_CALL_EV event. 
    Changes state to DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_rm_wants_pkt_call_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  ps_iface_type* sio_iface_ptr;
  ds_sys_event_dun_ctrl_entitlement_check_info_type ent_chk_info;
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  int16 ps_errno;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: sm_ptr = NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_3GPP_MSG1_HIGH( "Recd RM_WANTS_PKT_CALL_EV, state %d",
           sm_ptr->pdp_specific_info.pdp_ppp.state);

  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
  {
    case DSUMTS_RMSM_PPP_NULL_STATE:
#ifdef FEATURE_DATA_TE_MT_PDP
      rmsmi_info_ptr->is_locked = TRUE;
      /*------------------------------------------------------------------------- 
        Bug trap: 
        (1) check that RMSM is NOT marked for MT as this event is not allowed
            in NULL state for MT
      -------------------------------------------------------------------------*/
      ASSERT( !rmsmi_info_ptr->is_mt );
#endif /*FEATURE_DATA_TE_MT_PDP*/

      /*---------------------------------------------------------------------
        Send indication to DS_SYS_IF for ent_chk
      ---------------------------------------------------------------------*/
      memset(&ent_chk_info, 0, 
             sizeof(ds_sys_event_dun_ctrl_entitlement_check_info_type));

      ent_chk_info.tech_type = DS_SYS_TECH_3GPP;
      ent_chk_info.cb_data   = (unsigned char)(inst.rm_dev_instance);

      rmsmi_info_ptr->entitlement_check_req = TRUE;
      dsumts_rmsm_ppp_transition_state( sm_ptr, 
                          DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE );

      if (ps_sys_event_ind_ex(PS_SYS_TECH_3GPP,
                              PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                              (ps_sys_subscription_enum_type)
                              rmsmi_info_ptr->subs_id,
                              &ent_chk_info,
                              &ps_errno) == -1)
      {
        DS_3GPP_MSG1_ERROR("Error in sending sys_event ent_chk. ps_errno: %d",
                            ps_errno);

        dsumts_rmsm_post_event( &inst, UMTS_RMSM_ENTITLEMENT_CHECK_FAILED_EV );
      }
      break;

   case DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    {
      rmsmi_info_ptr->entitlement_check_req = FALSE;
   
    /*---------------------------------------------------------------------
     Bring up a data call on one of the available packet interfaces.
     Grab a Um interface : If a UM iface is allocated, it is automatically
     locked (acl) and net model state machine initied. 
     If the iface allocation fails or iface is in any state other than
     IFACE_DOWN, the fn sends a NO carrier result
     code & deregisters callbacks from SIO
        
     ---------------------------------------------------------------------*/
     sm_ptr->um_iface_ptr = dsumts_rmsmi_get_um_iface_ptr(sm_ptr);

     if ( sm_ptr->um_iface_ptr == NULL )
     {
       /*-----------------------------------------------------------------
         No interface available, We will send NO carrier result code & 
         deregister all callbacks from SIO when we transiotion to NULL state.
       -----------------------------------------------------------------*/
       DS_3GPP_MSG0_HIGH("um_iface is not available: Shutdown call");
       dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );
     }
     else
     {
       /*-------------------------------------------------------------------
        Since we have an UM iface, setup the SIO interface and 
        transition state to originate a UM pkt call. 
       -------------------------------------------------------------------*/
       sio_iface_ptr = &(sm_ptr->sio_iface);

       netmdl_set_meta_state( PPP_OOC_META_STATE ); 
       ps_iface_enable_ind( sio_iface_ptr );
       ps_iface_disable_flow( sio_iface_ptr, DS_FLOW_UMTS_RMSM_MASK );
       /*-----------------------------------------------------------------
       Transition the state to waiting for Um up state which would 
       bring up the um packet call
       -----------------------------------------------------------------*/
       dsumts_rmsm_ppp_transition_state( sm_ptr,
                           DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE );
     }
      break;
    }
        
#ifdef FEATURE_DATA_TE_MT_PDP
    case DSUMTS_RMSM_PPP_RINGING_TE:
      /*------------------------------------------------------------------------- 
        Bug trap: 
        (1) check that RMSM is locked
        (2) check that RMSM is marked for MT
      -------------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("RMSM_PPP_PKT_CALL_EV: check RMSM is locked, is_locked [%d]", 
              rmsmi_info_ptr->is_locked);
      ASSERT( rmsmi_info_ptr->is_locked );

      DS_3GPP_MSG1_HIGH("RMSM_PPP_PKT_CALL_EV: check RMSM is for MT, is_mt [%d]", 
              rmsmi_info_ptr->is_mt);
      ASSERT( rmsmi_info_ptr->is_mt );

      /*---------------------------------------------------------------------
        Bring up a data call on one of the available packet interfaces.
        Grab a Um interface : If a UM iface is allocated, it is automatically
        locked (acl) and net model state machine initied. 
        If the iface allocation fails or iface is in any state other than
        IFACE_DOWN, the fn sends a NO carrier result
        code & deregisters callbacks from SIO
      ---------------------------------------------------------------------*/
      sm_ptr->um_iface_ptr = dsumts_rmsmi_get_um_iface_ptr(sm_ptr);

      if ( sm_ptr->um_iface_ptr == NULL )
      {
        /*-----------------------------------------------------------------
          No interface available, Send NO carrier result code & 
          deregister all callbacks from SIO . Remain in NULL state
        -----------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("RMSM_PPP_PKT_CALL_EV: um_iface is not available: Shutdown call");
        (void)ds3g_siolib_ex_set_cd_state
               (DS3G_SIOLIB_REMOTE_CARRIER_OFF,rmsmi_info_ptr->port_id);
        dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
        ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,FALSE );
        dsumts_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);

        ds3g_siolib_ex_release_port (rmsmi_info_ptr->port_id);
        dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, DS3G_SIOLIB_PORT_NONE);      }
      else
      {
        /*-------------------------------------------------------------------
         Since we have an UM iface, setup the SIO interface and 
         transition state to originate a UM pkt call. 
        --------------------------------------------------------------------*/
        sio_iface_ptr = &(sm_ptr->sio_iface);
        
        netmdl_set_meta_state( PPP_OOC_META_STATE ); 
        ps_iface_enable_ind( sio_iface_ptr );
        ps_iface_disable_flow( sio_iface_ptr, DS_FLOW_UMTS_RMSM_MASK );
        /*-----------------------------------------------------------------
          Transition the state to waiting for Um up state which would 
          bring up the um packet call
        -----------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("RMSM_PPP_PKT_CALL_EV: transition to WAITING_FOR_UM_UP_STATE");
        dsumts_rmsm_ppp_transition_state( sm_ptr,
                              DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE );
      }

      break;

#endif /* FEATURE_DATA_TE_MT_PDP */

    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE:

      /*---------------------------------------------------------------------
        It's error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR("Unexpected RM_WANTS_PKT_CALL_EV in %d state",
                         sm_ptr->pdp_specific_info.pdp_ppp.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR("Invalid RMSM state %d",
                          sm_ptr->pdp_specific_info.pdp_ppp.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ppp.state ) */
} /* dsumts_rmsm_ppp_process_rm_wants_pkt_call_ev() */



/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_RM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes UMTS_RMSM_RM_IFACE_DOWN_EV event. Changes state
    to DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_rm_ppp_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  int16  ps_iface_errno;
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_HIGH( "Recd RM_IFACE_DOWN_EV, state %d", 
           sm_ptr->pdp_specific_info.pdp_ppp.state);

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;


  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
  {
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE:
      /*---------------------------------------------------------------------
        User Pressed cancel before the call came up. Sends DSAT_OK result
        code to TE.
      ---------------------------------------------------------------------*/      
      rmsmi_info_ptr->user_cancel = TRUE;

      /* fall through */
    //lint -fallthrough
    case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
      /*---------------------------------------------------------------------
        Since laptop has terminated the call, we need to cleanup Um before
        completely terminating the call. Tear down the UM pkt iface first.
        Checking for the return code as the iface could already be down.
      ---------------------------------------------------------------------*/
      if ( ps_iface_tear_down_cmd
             (
               sm_ptr->um_iface_ptr, 
               &ps_iface_errno,
               NULL 
             ) == 0 )
      {
        /*-------------------------------------------------------------------
          If the UM packet iface is already down, we should cleanup the
          call on Rm since we won't get any other notification from UM
          controller
        -------------------------------------------------------------------*/
        dsumts_rmsm_ppp_transition_state( sm_ptr,
                                          DSUMTS_RMSM_PPP_NULL_STATE );
      }
      else
      {
        dsumts_rmsm_ppp_transition_state( sm_ptr, 
                                DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE );
      }

      break;

    case DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
      dsumts_rmsm_ppp_transition_state( sm_ptr,
                                          DSUMTS_RMSM_PPP_NULL_STATE );
      break;

    case DSUMTS_RMSM_PPP_NULL_STATE:
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE:

      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH( "Ignoring RM_IFACE_DOWN_EV in %d state",
                sm_ptr->pdp_specific_info.pdp_ppp.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR("Invalid RMSM state %d",
                          sm_ptr->pdp_specific_info.pdp_ppp.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ppp.state ) */
} /* dsumts_rmsm_ppp_process_rm_iface_down_ev() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_UM_IFACE_UP_EV()

  DESCRIPTION
    This function processes UMTS_RMSM_UM_IFACE_UP_EV event. Changes 
    state to DSUMTS_RMSM_PPP_UM_RM_UP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_um_iface_up_ev
(
   dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsat_connect_rate_s_type connect_rate; /* Report with next CONNECT    */
#ifdef FEATURE_DATA_A2
  sint15              ps_errno;         /* PS err number - needed for IOCTL */
  sio_port_id_type    sio_port_id;
#endif /*FEATURE_DATA_A2*/
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  dsumts_rmsm_instance_type inst;
#ifdef FEATURE_DATA_TE_MT_PDP
  ds_umts_pdp_auth_type       pdata;
#endif
  ppp_dev_enum_type ppp_dev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: sm_ptr = NULL");
    return;
  }

  DS_3GPP_MSG1_HIGH( "Recd UM_IFACE_UP_EV, state %d", 
           sm_ptr->pdp_specific_info.pdp_ppp.state);

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: rmsmi_info_ptr = NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
  {
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE:
      /*---------------------------------------------------------------------
        Return the extended result code with CONNECT.
        V0 and &E0(CONNECT?<radio zone speed>)
        V1 and &E0(CONNECT<radio zone speed><communication protocol>[<text>])
        V1 and &E1(CONNECT?<TAF-TE speed> <communication protocol> [<text>])
        <text>:APN/MaxBitRateUplink/MaxBitRateDownlink
      ---------------------------------------------------------------------*/
      memset((void*)&connect_rate, 0, sizeof(dsat_connect_rate_s_type));

      /* Always return PACKET for PS */
      connect_rate.call_type = DSAT_CALL_TYPE_PS;
      connect_rate.comm_prot = DSAT_COMM_PROT_PACKET;

      /* Return APN from the sm_ptr profile details */
      memscpy( (void*)connect_rate.rate.ps_call.apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1),
               (void*)rmsmi_info_ptr->profile_context_info.apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1) );

      /* return default value */
      connect_rate.rate.ps_call.dl_rate = MAX_BIT_RATE_DEFAULT;
      connect_rate.rate.ps_call.ul_rate = MAX_BIT_RATE_DEFAULT;

      dsat_report_rate_on_connect( connect_rate );

      dsat_send_result_code_ex( DSAT_CONNECT,rmsmi_info_ptr->port_id );

      (void)ds3g_siolib_ex_set_cd_state
             (DS3G_SIOLIB_REMOTE_CARRIER_ON,rmsmi_info_ptr->port_id);

      ps_iface_routeable_ind(sm_ptr->um_iface_ptr);

      dsumts_rmsmi_setup_sio_watermarks(inst.rm_dev_instance,
                                        &rmsmi_info_ptr->sio_tx_wmk,
                                        &rmsmi_info_ptr->sio_rx_wmk,
                                        &rmsmi_info_ptr->sio_tx_wmk_q,
                                        &rmsmi_info_ptr->sio_rx_wmk_q);

      /*---------------------------------------------------------------------
        Allocate config block.
      ---------------------------------------------------------------------*/
      dsumts_rmsm_ppp_rm_ppp_config_ptr = 
                  (ppp_dev_opts_type *)ds_system_heap_mem_alloc(
                                                    sizeof(ppp_dev_opts_type));
      if(dsumts_rmsm_ppp_rm_ppp_config_ptr == NULL)
      {
        DS_3GPP_MSG2_ERROR("Memory allocation failure for client:%d, size:%d",
                    MODEM_MEM_CLIENT_DATA, sizeof(ppp_dev_opts_type));
        ASSERT(0);
      }

      ppp_get_default_opts( dsumts_rmsm_ppp_rm_ppp_config_ptr );
      dsumts_rmsm_ppp_rm_ppp_config_ptr->rx_f_ptr       = dsumts_rmsmi_sio_rx_ppp_data;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->rx_user_data   = (void *)rmsmi_info_ptr;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->tx_f_ptr       = dsumts_rmsmi_sio_tx_ppp_data;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->tx_user_data   = (void *)rmsmi_info_ptr;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->ipcp_info.rx_iface_ptr =
        &sm_ptr->sio_iface;

      dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.mode  = PPP_CP_PASSIVE;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->ipcp_info.mode = PPP_CP_PASSIVE;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->hdlc_mode      = HDLC_HW_PREF;

#ifdef FEATURE_DATA_TE_MT_PDP
      /* Make LCP and IPCP active for MT call */
      dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.mode   = PPP_CP_ACTIVE;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->ipcp_info.mode  = PPP_CP_ACTIVE;
      /*---------------------------------------------------------------------
        Turn on auth bits for remote.want and remote.will. Turn others off.
      ---------------------------------------------------------------------*/
      dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.will_mask[FSM_REMOTE] |= LCP_N_AP;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.want_mask[FSM_REMOTE] |= LCP_N_AP;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
      dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;      

      dsumts_rmsm_ppp_rm_ppp_config_ptr->auths_peer = FALSE;
      memset( (void*)&pdata, 0, sizeof(ds_umts_pdp_auth_type) );
      /* Read authentication info from profile. This info will be used for 
         authentication negotiation with PC */
      if (ds_umts_get_pdp_profile_auth_info_per_subs((uint16)rmsmi_info_ptr->profile_id,
                                dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
                                            &pdata) != DS_UMTS_PDP_SUCCESS )
  
      {
        DS_3GPP_MSG1_HIGH ("Error PDP Auth profile read: %d,using CHAP/PAP"
                           " with default settings",
                            rmsmi_info_ptr->profile_id);
        dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
        (ppp_auth_proto_e_type) ( (uint32)PPP_AUTH_PAP | (uint32)PPP_AUTH_CHAP );
      }
      else
      {
        DS_3GPP_MSG0_HIGH("Setting CHAP/PAP based on profile auth");
        dsumts_rmsm_ppp_rm_ppp_config_ptr->auth_info.user_id_len = 
                          strlen((char*)pdata.username);
        dsumts_rmsm_ppp_rm_ppp_config_ptr->auth_info.passwd_len = 
                          strlen((char*)pdata.password);
        strlcpy((char*)dsumts_rmsm_ppp_rm_ppp_config_ptr->auth_info.user_id_info,
                (char*)pdata.username,
                PPP_MAX_USER_ID_LEN);
        strlcpy((char*)dsumts_rmsm_ppp_rm_ppp_config_ptr->auth_info.passwd_info,
                (char*)pdata.password,
                PPP_MAX_PASSWD_LEN);
        if (pdata.auth_type == DS_UMTS_PDP_AUTH_PAP)
        {
          DS_3GPP_MSG0_HIGH("Using PAP protocol");
          dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].authentication
                 = PPP_PAP_PROTOCOL;
        }
        else
        {
          DS_3GPP_MSG0_HIGH("Using CHAP protocol");
          dsumts_rmsm_ppp_rm_ppp_config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
                          (uint16)PPP_AUTH_CHAP;
        }
      }
#endif /*FEATURE_DATA_TE_MT_PDP*/

#ifdef FEATURE_DATA_A2
      if(dsumts_rmsm_iface_ioctl_hdlr(&sm_ptr->sio_iface,
                                      PS_IFACE_IOCTL_RM_GET_SIO_PORT,
                                      &sio_port_id,
                                      &ps_errno) == 0)
      {
        if(sio_port_id == SIO_PORT_A2_MDM)
        {
          dsumts_rmsm_ppp_rm_ppp_config_ptr->hdlc_mode    = HDLC_A2_AUTONOMOUS;
        }
      }
#endif /* FEATURE_DATA_A2 */
      dsumts_rmsm_ppp_rm_ppp_config_ptr->bridge_dev     = PPP_UM_SN_DEV;

      if ( ppp_start( ppp_dev, dsumts_rmsm_ppp_rm_ppp_config_ptr ) < 0 )
      {
        DS_3GPP_MSG0_ERROR("RM PPP config failed, posting RM_IFACE_DOWN");
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );
        break;
      }

      /*---------------------------------------------------------------------
        Now configure PPP for UM device to initiate end to end PPP 
        negotiation. If it fails we still wanna post RM_IFACE_DOWN ev since 
        the side effect is we are unable to bring up Rm.
      ---------------------------------------------------------------------*/
      if ( dsUmtsPdnCntx_configurePdpPpp(sm_ptr->um_iface_ptr, 
                                         ppp_dev) < 0 )
      {
        DS_3GPP_MSG0_ERROR("UM PPP config failed, posting RM_IFACE_DOWN");
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );
        break;
      }
      
      PS_IFACE_SET_GRACEFUL_CLOSE(sm_ptr->um_iface_ptr,TRUE);

      ppp_set_mode( ppp_dev, PPP_FULL_NET_MODE );
      ppp_set_mode( PPP_UM_SN_DEV, PPP_FULL_NET_MODE );     
      netmdl_set_meta_state( PPP_INIT_META_STATE );
      (void)ppp_event_register(
                                rmsmi_info_ptr->ppp_handle,
                                PPP_PROTOCOL_LCP,
                                PPP_DOWN_EV_MASK
                         );

      (void)ppp_event_register(
                                sm_ptr->pdp_specific_info.pdp_ppp.um_ppp_ev_handle,
                                PPP_PROTOCOL_LCP,
                                PPP_DOWN_EV_MASK
                         );

      (void)ds3g_siolib_ex_change_mode(
                                     SIO_DS_PKT_MODE,
                                     &rmsmi_info_ptr->sio_rx_wmk,
                                     &rmsmi_info_ptr->sio_tx_wmk,
                                     NULL,
                                     rmsmi_info_ptr->port_id
                                   );
#ifdef FEATURE_QXDM_WM_TRACING
      //Register RMSM client with FC
      dsumts_rmsmi_register_with_fc(sm_ptr->instance);
#endif /*FEATURE_QXDM_WM_TRACING*/

      dsumts_rmsm_ppp_transition_state( sm_ptr, 
                                DSUMTS_RMSM_PPP_UM_RM_UP_STATE );
      break;

    case DSUMTS_RMSM_PPP_NULL_STATE:
    case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE:

      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH( "Ignoring UM_IFACE_UP_EV in %d state",
                sm_ptr->pdp_specific_info.pdp_ppp.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR("Invalid RMSM state %d", 
                          sm_ptr->pdp_specific_info.pdp_ppp.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ppp.state) */
} /* dsumts_rmsm_ppp_process_um_iface_pdp_ev() */



/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_UM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes UMTS_RMSM_UM_IFACE_DOWN_EV event. Changes state
    to DSUMTS_RMSM_PPP_NULL_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_um_iface_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: sm_ptr = NULL");
    return;
  }
  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
 
  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: rmsmi_info_ptr = NULL");
    return;
  }

  DS_3GPP_MSG1_HIGH( "Recd UM_IFACE_DOWN_EV, state %d", 
          sm_ptr->pdp_specific_info.pdp_ppp.state);

  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
  {
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE:
#ifdef FEATURE_DATA_TE_MT_PDP
      DSUMTS_CONDITIONAL_BLOCK_START( rmsmi_info_ptr->is_mt, "RMSM_PPP_UM_IFACE_DOWN: is_mt set!" );

      if (sm_ptr->pdp_specific_info.pdp_ppp.state 
              == DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE)
      {
        /* Call is not UP yet, send REJECT */
        dsumtsps_mt_umsm_post_ev( rmsmi_info_ptr->mt_index, 
                                  MT_UMSM_CALL_REJECT_EV , NULL );
      }
      else if ( (sm_ptr->pdp_specific_info.pdp_ppp.state 
                                          == DSUMTS_RMSM_PPP_UM_RM_UP_STATE) ||
                (sm_ptr->pdp_specific_info.pdp_ppp.state
                               == DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE) 
              )
      {
        DS_3GPP_MSG0_HIGH( "MT-UMSM is cleared in UM up no need to do anything");
      }
      else
      {
        DS_3GPP_MSG1_ERROR("PPP_PROCESS_RM_IFACE_DOWN: wrong state [%d]", sm_ptr->pdp_specific_info.pdp_ppp.state);
        ASSERT(0);
        return;
      }

      DSUMTS_CONDITIONAL_BLOCK_END();
#endif /*FEATURE_DATA_TE_MT_PDP*/
      /*-------------------------------------------------------------------
        The UM pkt iface is down now, we should also terminate the call
        on Rm. There isn't any PPP running on SIO iface, only final
        cleanup needs to be done.
      -------------------------------------------------------------------*/
      dsumts_rmsm_ppp_transition_state( sm_ptr, 
                                        DSUMTS_RMSM_PPP_NULL_STATE );
      break;

    case DSUMTS_RMSM_PPP_NULL_STATE:

      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH( "Ignoring UM_IFACE_DOWN_EV in %d state",
                sm_ptr->pdp_specific_info.pdp_ppp.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR("Invalid RMSM state %d", 
                          sm_ptr->pdp_specific_info.pdp_ppp.state);
      ASSERT( 0 );
      break;

  } /* switch( m_info_ptr->sm_info.pdp_ppp.stat ) */
} /* dsumts_rmsm_ppp_process_um_iface_down_ev() */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_PPP_PROCESS_ENTITLEMENT_CHECK_FAILED_CMD

 DESCRIPTION
   
   This function will process the entitlement check failed cmd.
 
   This function is called when there is an error in posting entitlement check
   event to PS.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ppp_process_entitlement_check_failed_cmd
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type  *rmsmi_info_ptr = NULL;
  ps_iface_type*           sio_iface_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    return;
  }

  DS_3GPP_MSG1_MED("Entered Process Entitlement Check Failed CMd handling, "
                   "Rm Dev instance: %d", rmsmi_info_ptr->instance);

  switch (sm_ptr->pdp_specific_info.pdp_ppp.state)
  {
    case DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    {
      rmsmi_info_ptr->entitlement_check_req = FALSE;
      /*---------------------------------------------------------------------
        Bring up a data call on one of the available packet interfaces.
        Grab a Um interface : If a UM iface is allocated, it is automatically
        locked (acl) and net model state machine initied. 
        If the iface allocation fails or iface is in any state other than
        IFACE_DOWN, the fn sends a NO carrier result
        code & deregisters callbacks from SIO
      ---------------------------------------------------------------------*/
      sm_ptr->um_iface_ptr = dsumts_rmsmi_get_um_iface_ptr(sm_ptr);

      if ( sm_ptr->um_iface_ptr == NULL )
      {
        /*-----------------------------------------------------------------
          No interface available, Send NO carrier result code & 
          deregister all callbacks from SIO . Remain in NULL state
        -----------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("um_iface is not available: Shutdown call");
        (void)ds3g_siolib_ex_set_cd_state
               (DS3G_SIOLIB_REMOTE_CARRIER_OFF,rmsmi_info_ptr->port_id);
        dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
        ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,FALSE );
        dsumts_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);
        ds3g_siolib_ex_release_port (rmsmi_info_ptr->port_id);
        dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, 
                                     DS3G_SIOLIB_PORT_NONE);
      }
      else
      {
        /*-------------------------------------------------------------------
          Since we have an UM iface, setup the SIO interface and 
          transition state to originate a UM pkt call. 
        -------------------------------------------------------------------*/
        sio_iface_ptr = &(sm_ptr->sio_iface);
      
        netmdl_set_meta_state( PPP_OOC_META_STATE ); 
        ps_iface_enable_ind( sio_iface_ptr );
        ps_iface_disable_flow( sio_iface_ptr, DS_FLOW_UMTS_RMSM_MASK );
        /*-----------------------------------------------------------------
          Transition the state to waiting for Um up state which would 
          bring up the um packet call
        -----------------------------------------------------------------*/
        dsumts_rmsm_ppp_transition_state( sm_ptr,
                              DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE );
      }

      break;
    }

    default:
      DS_3GPP_MSG1_ERROR("Ignore entitlement failed Ev in %d state ", 
  		          sm_ptr->pdp_specific_info.pdp_ppp.state);  
      break;
  }

} /* ds_3gpp_rmsm_ppp_process_entitlement_check_failed_cmd */


#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_UM_WANTS_MT_PKT_CALL_EV()

  DESCRIPTION
   

  PARAMETERS
    sm_ptr: State Machine info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_um_wants_mt_pkt_call_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{

  dsumts_rmsmi_info_type* rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  DS_3GPP_MSG0_MED("RMSM_PPP_UM_MT_PKT_CALL: UM wants MT PPP pkt call");

  /*-------------------------------------------------------------------------
    This command is valid only in DSUMTS_RMSM_PPP_NULL_STATE
    Perform the following Exit Actions:
    (1) Lock RMSM
    (2) Mark RMSM for MT PDP
    then, Transition over to new state DSUMTS_RMSM_RINGING_TE
  -------------------------------------------------------------------------*/
  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
  {
    case DSUMTS_RMSM_PPP_NULL_STATE:
      rmsmi_info_ptr->is_locked = TRUE;
      if ( !dsumts_rmsm_set_mt( rmsmi_info_ptr->mt_index ) )
      {
        DS_3GPP_MSG0_ERROR("RMSM_PPP_MT_PKT_CALL: unable to set MT flag");
      }
      dsumts_rmsm_ppp_transition_state( sm_ptr,
                              DSUMTS_RMSM_PPP_RINGING_TE );
    break;

    default:
      DS_3GPP_MSG1_ERROR("RMSM_PPP_UM_MT_PKT_CALL: UM_WANTS_MT_PKT_CALL_EV is invalid in state [%d]",
                sm_ptr->pdp_specific_info.pdp_ppp.state);
  }/* switch(state) */

}/*dsumts_rmsm_ppp_process_um_wants_mt_pkt_call_ev*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_RM_REJECT_MT_PKT_CALL_EV()

  DESCRIPTION
   

  PARAMETERS
    sm_ptr: State Machine info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_rm_reject_mt_pkt_call_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  DS_3GPP_MSG0_MED("PPP_PROCESS: RM rejects MT PPP pkt call");

  /*------------------------------------------------------------------------- 
    Bug trap: 
    (1) check that RMSM is marked for MT
  -------------------------------------------------------------------------*/
  ASSERT( rmsmi_info_ptr || rmsmi_info_ptr->is_mt );

  /*-------------------------------------------------------------------------
    Exit actions:
    (1) Deregister ANS+HANGUP callbacks
    (2) Deregister SIOLIB auto-ans callbacks
    (3) Post REJECT_EV to MT-UMSM
    (then, Transition over to new state DSUMTS_RMSM_IP_NULL_STATE
  -------------------------------------------------------------------------*/
  ds3g_siolib_stop_alerting();

  dsat_deregister_pdp_handlers();

  ds3g_siolib_set_answer_callbacks( NULL );

  dsumtsps_mt_umsm_post_ev( rmsmi_info_ptr->mt_index, 
                            MT_UMSM_CALL_REJECT_EV, NULL );

  dsumts_rmsm_ppp_transition_state( sm_ptr, DSUMTS_RMSM_PPP_NULL_STATE );
  
}/* dsumts_rmsm_ppp_process_rm_reject_mt_pkt_call_ev() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_RM_UM_LOCKED_EV()

  DESCRIPTION
   

  PARAMETERS
    sm_ptr: State Machine info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_rm_um_locked_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  DS_3GPP_MSG0_MED("PPP_PROCESS: RM Um locked ");

  /*------------------------------------------------------------------------- 
    Bug trap: 
    (1) check that RMSM is marked for MT
  -------------------------------------------------------------------------*/
  ASSERT( rmsmi_info_ptr || rmsmi_info_ptr->is_mt );

  DS_3GPP_MSG1_HIGH( "Recd RM_UM_LOCKED, state %d", 
                      sm_ptr->pdp_specific_info.pdp_ppp.state);

  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
  {
    case DSUMTS_RMSM_PPP_RINGING_TE:
    
      /*-------------------------------------------------------------------------
      Exit actions:
      (1) Deregister ANS+HANGUP callbacks
      (2) Deregister SIOLIB auto-ans callbacks
      (then, Transition over to new state DSUMTS_RMSM_IP_NULL_STATE
    -------------------------------------------------------------------------*/
      ds3g_siolib_stop_alerting();

      dsat_deregister_pdp_handlers();

      ds3g_siolib_set_answer_callbacks( NULL );

      dsumts_rmsm_ppp_transition_state( sm_ptr, DSUMTS_RMSM_PPP_NULL_STATE );
      break;

     case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
     case DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE:
      dsumts_rmsm_ppp_transition_state( sm_ptr, DSUMTS_RMSM_PPP_NULL_STATE );
      break;
    default:
      DS_3GPP_MSG0_ERROR("Um down received in incorrect state: %d ");
      break;
  }
}/* dsumts_rmsm_ppp_process_rm_um_locked_ev */

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_UM_PPP_DOWN_EV()

  DESCRIPTION
    Processes UM_PPP_DOWN_EV. Sends STOP command to RM PPP. 

  PARAMETERS
    sm_ptr: State Machine info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    Triggers call disconnection. 
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_process_um_ppp_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  ppp_dev_enum_type ppp_dev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  DS_3GPP_MSG3_HIGH( "Recd UM_PPP_DOWN_EV, state %d", 
                      sm_ptr->pdp_specific_info.pdp_ppp.state, 0, 0 );

  switch( sm_ptr->pdp_specific_info.pdp_ppp.state )
 {
   case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
     /*---------------------------------------------------------------------
       Bring down RM PPP. No need to change state at this time. 
     ---------------------------------------------------------------------*/
     if (ppp_stop(ppp_dev,PPP_PROTOCOL_LCP) != 0)
     {
       DS_3GPP_MSG0_HIGH("ppp_stop failed!");
     }
     break;

   default:
     /*---------------------------------------------------------------------
       Ignore event in all other states.
     ---------------------------------------------------------------------*/
     DS_3GPP_MSG1_HIGH( "Ignoring UM_PPP_DOWN_EV in %d state",
               sm_ptr->pdp_specific_info.pdp_ppp.state);
     break;
 }
} /* dsumts_rmsm_ppp_process_um_ppp_down_ev() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ppp_transition_state
(
  dsumts_rmsmi_sm_info_type       *sm_ptr,
  dsumts_rmsm_pppi_state_type  new_state
)
{
  int16  ps_iface_errno;
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_pdp_answer_data_type ans_cb_data;
#endif /*FEATURE_DATA_TE_MT_PDP*/
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  dsumts_rmsm_instance_type inst;
  ppp_dev_enum_type ppp_dev;
  ps_iface_down_mh_event_info_type   down_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  down_info.down_reason = PS_NET_DOWN_REASON_CLIENT_END;

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.so_mask = DS_SYS_SO_EX_UNSPECIFIED;
  down_info.bearer_tech.rat_value = DS_SYS_RAT_UNKNOWN;

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: sm_ptr = NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: rmsmi_info_ptr = NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (!dsumts_rmsmi_validate_ppp_dev(ppp_dev))
  {
    DS_3GPP_MSG3_ERROR("Invalid PPP Dev %d", ppp_dev, 0, 0); 
    ASSERT(0);
    return;
  }
#ifdef FEATURE_DATA_TE_MT_PDP
  /*------------------------------------------------------------------------- 
    Bug trap: check whether RMSM is locked
    RMSM gets locked when it exists NULL state
  -------------------------------------------------------------------------*/
    ASSERT( rmsmi_info_ptr->is_locked );
#endif /*FEATURE_DATA_TE_MT_PDP*/

  DS_3GPP_MSG2_HIGH( "UMTS RMSM state goes from %d to %d", 
          sm_ptr->pdp_specific_info.pdp_ppp.state, new_state);


  sm_ptr->pdp_specific_info.pdp_ppp.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    case DSUMTS_RMSM_PPP_NULL_STATE:
    {
      rmsmi_info_ptr->subs_id = SYS_MODEM_AS_ID_1;

      /*------------------------------------------------------------------
      If we got to NULL state as a result of a failure from ent check,
      we need no further clean up here as pp would not have been started and
      no event registration would have happened
      ------------------------------------------------------------------*/
      if(rmsmi_info_ptr->entitlement_check_req == TRUE)
      {
        DS_3GPP_MSG0_MED("No further cleanup action needed. Reset ent_chk_reg flag");
        rmsmi_info_ptr->entitlement_check_req = FALSE;
        /* Client can also end the call while entitlement check is in progress */
        dsumts_rmsmi_sio_cleanup(rmsmi_info_ptr->instance);
        return;
      }
#ifdef FEATURE_DATA_TE_MT_PDP
      DSUMTS_CONDITIONAL_BLOCK_START( ( rmsmi_info_ptr->mt_te_ring ), 
                                      "RMSM_IP_TRANS_STATE: failed at ring, minimal cleanup" );
      if (rmsmi_info_ptr->user_cancel == TRUE)
      {
        /*-------------------------------------------------------------------
          TE said ATH
        -------------------------------------------------------------------*/
        dsat_send_result_code_ex( DSAT_OK,rmsmi_info_ptr->port_id );
        rmsmi_info_ptr->user_cancel = FALSE;
      }
      else
      {
        dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
      }

      rmsmi_info_ptr->mt_te_ring = FALSE;

      if ( !dsumts_rmsm_reset_mt( rmsmi_info_ptr->mt_index ) )
      {
        DS_3GPP_MSG0_ERROR("RMSM_IP_TRANS_STATE: unable to reset MT flag");
      }

      rmsmi_info_ptr->is_locked = FALSE;

      DSUMTS_CONDITIONAL_BLOCK_END();
#endif /*FEATURE_DATA_TE_MT_PDP*/

      /*---------------------------------------------------------------------
        Pkt call is terminating, Perform all the necessary cleanup
      ---------------------------------------------------------------------*/
      ds3g_siolib_ex_set_inbound_flow( DS_FLOW_UMTS_RMSM_MASK, DS_FLOW_DISABLE, 
                                       rmsmi_info_ptr->port_id );

#ifdef FEATURE_QXDM_WM_TRACING
      dsumts_rmsmi_deregister_with_fc(sm_ptr->instance);
#endif
#ifdef FEATURE_DATA_WCDMA_PS

      dsumts_rmsmi_dl_fc_deregister();
#endif /* FEATURE_DATA_WCDMA_PS */

      if (ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF,
                                       rmsmi_info_ptr->port_id ) ==
          DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_3GPP_MSG0_ERROR("Error setting CD State");
      }
      else if(ds3g_siolib_ex_change_mode( SIO_DS_AUTODETECT_MODE, NULL, NULL, NULL,
                                          rmsmi_info_ptr->port_id)
        == DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_3GPP_MSG0_ERROR("Error changing mode to autodetect");
      }
      
      if (rmsmi_info_ptr->user_cancel == TRUE)
      {
        /*-------------------------------------------------------------------
          User pressed cancel before the call came up. Send DSAT_OK result
          code to TE.
        -------------------------------------------------------------------*/
        dsat_send_result_code_ex( DSAT_OK,rmsmi_info_ptr->port_id );
        rmsmi_info_ptr->user_cancel = FALSE;
      }
      else
      {
        dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
      }


      dsumts_rmsmi_dereg_um_events(sm_ptr);
      /*---------------------------------------------------------------------
        Reset the PPP state and deregister the callbacks
      ---------------------------------------------------------------------*/
      /*lint -e655 */
      (void)ppp_event_deregister(
                                  rmsmi_info_ptr->ppp_handle,
                                  PPP_PROTOCOL_IPCP,
                                  (ppp_event_mask_e_type)
                                    (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK)
                                );
      /*lint +e655 */
      (void)ppp_event_deregister(
                                  rmsmi_info_ptr->ppp_handle,
                                  PPP_PROTOCOL_LCP,
                                  PPP_DOWN_EV_MASK
                                );


      (void)ppp_event_deregister(
                                  sm_ptr->pdp_specific_info.pdp_ppp.um_ppp_ev_handle,
                                  PPP_PROTOCOL_LCP,
                                  PPP_DOWN_EV_MASK
                                );

      (void)ppp_abort(ppp_dev);

      netmdl_set_meta_state( PPP_OOC_META_STATE );

      /*---------------------------------------------------------------------
        Cleanup local watermarks.
      ---------------------------------------------------------------------*/

      dsm_empty_queue(&rmsmi_info_ptr->sio_tx_wmk);
      dsm_empty_queue(&rmsmi_info_ptr->sio_rx_wmk);

      /*---------------------------------------------------------------------
        Restore SIO and Um interfaces to correct state
      ---------------------------------------------------------------------*/
      ps_iface_set_bridge( &sm_ptr->sio_iface, NULL);
      ps_iface_phys_link_down_ind(&sm_ptr->sio_iface);
      ps_iface_down_ind( &sm_ptr->sio_iface,&down_info);

      sm_ptr->um_iface_ptr = NULL;
      sm_ptr->um_phys_lnk_ptr = NULL;
      
      /*---------------------------------------------------------------------
        Indicate to siolib that we are not in call anymore, deregister
        all the event handlers and enable the flow to accept the next call
      ---------------------------------------------------------------------*/
      ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,FALSE );
      dsumts_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);

      ds3g_siolib_ex_release_port ( rmsmi_info_ptr->port_id );
      ds3g_siolib_ex_set_inbound_flow( DS_FLOW_UMTS_RMSM_MASK|DS_FLOW_PKT_WM_MASK,
                                       DS_FLOW_ENABLE,
                                       rmsmi_info_ptr->port_id );
      /*-----------------------------------------------------------------------
        De-allocate ppp config block.
      ---------------------------------------------------------------------*/
      ds_system_heap_mem_free(dsumts_rmsm_ppp_rm_ppp_config_ptr);
      dsumts_rmsm_ppp_rm_ppp_config_ptr = NULL;
      dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, DS3G_SIOLIB_PORT_NONE);
      break;
    }

    case DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
      break;

    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE:

      if ( !dsumts_rmsmi_reg_um_events( sm_ptr ) )
      {
          /*-------------------------------------------------------------------
            Um Iface event registration failed.
          -------------------------------------------------------------------*/
          DS_3GPP_MSG0_ERROR( "Um Iface event reg failed, "
                              "post UMTS_RMSM_UM_IFACE_DOWN_EV");
          dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      /*---------------------------------------------------------------------
        Bring up the interface. If there is an error, bring iface to down 
        state which will post appropriate event to bring down the laptop 
        call.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "route lookup ret Um Iface [0x%x:%x], try to bring-up by policy", 
                sm_ptr->um_iface_ptr->name, sm_ptr->um_iface_ptr->instance);
      if ( dsumts_rmsmi_bring_up_um_iface_by_policy( sm_ptr, 
                                                     &sm_ptr->um_iface_ptr, 
                                                     &ps_iface_errno ) == 0 )
      {
        DS_3GPP_MSG1_ERROR("UM i/f 0x%x already up", 
                  sm_ptr->um_iface_ptr);
        ASSERT(0);
      }

      if ( ps_iface_errno != DS_EWOULDBLOCK )
      {
        DS_3GPP_MSG2_ERROR("UM i/f 0x%x error %d in bring up", 
                  sm_ptr->um_iface_ptr, ps_iface_errno);
        /*-------------------------------------------------------------------
          Um iface could not be brought up, post the appropriate event so
          that the call on Rm can be cleaned up
        -------------------------------------------------------------------*/
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
      }

      break;

    case DSUMTS_RMSM_PPP_UM_RM_UP_STATE:
      ps_iface_up_ind( &sm_ptr->sio_iface );
      ps_iface_phys_link_up_ind(&sm_ptr->sio_iface);
      ps_iface_enable_flow( &sm_ptr->sio_iface, 
                            DS_FLOW_UMTS_RMSM_MASK );

      break;

    case DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE:
      break;

#ifdef FEATURE_DATA_TE_MT_PDP
    case DSUMTS_RMSM_PPP_RINGING_TE:
      /*-------------------------------------------------------------------------
        Perform following Entry actions:
         Set mt_te_ring
         Perform auto-answer if set
         Register handlers for ANS and HANGUP
         Register handlers for ATSO Auto-answer with SIOLIB 
         RING TE
      -------------------------------------------------------------------------*/

      /* This flag is set to indicate a different cleanup mech on reaching NULL state */
      rmsmi_info_ptr->mt_te_ring = TRUE;

      if ( dsat_cgauto_val == 1 ) 
      {
        /* This will make sure that default profile is used for auto-answer */
        ans_cb_data.number_of_profiles = 0;

        DS_3GPP_MSG0_HIGH("RMSM_PPP_TRANS_STATE: Rm ready for auto-ans! RING TE and ANS immediately");
        /*-------------------------------------------------------------------------
          After _alert_user() has been called, _stop_alerting() should be called when 
          user answers the call or if the call is released in the RINGING call state.
          Send RING and do not perform auto-answer, next call ANS routine
        -------------------------------------------------------------------------*/
        ds3g_siolib_alert_user( DSAT_RING, 
                                FALSE );
        dsumts_rmsm_mt_ans_cb( &ans_cb_data );
        break;
      }
      else
      {
        DS_3GPP_MSG0_HIGH("RMSM_PPP_TRANS_STATE: Rm is not set for auto-ans! reg ANS+HUP cb");
      }

      /* Register handler with ATCOP and SIOLIB. ATCOP callback functions 
       * must be registered before SIOLIB callback functions are registered 
       * to avoid a race condition in ATCOP.
       *
       * DSAT_OK: if registered successfully
       * DSAT_ERROR: if trying to register when +CGAUTO=1
       * DSAT_BUSY: if registered unsuccessfully
       */

      result = dsat_register_pdp_handlers( 
                 dsumts_rmsm_mt_ans_cb,
                 dsumts_rmsm_mt_hangup_cb );

      if ( result == DSAT_BUSY ) 
      {
        DS_3GPP_MSG0_HIGH("RMSM_PPP_TRANS_STATE: cannot reg ANS+HUP CBs, reject MT call!");

        /* Post UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV to itself */
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV);
        return;
      }
      else if ( result == DSAT_ERROR ) 
      {
        /* dsat_cgauto_val has been checked and found not set, trap this */
        DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: err! trying to reg hdlr when +CGAUTO=1");
        return;
      }
      else if ( result == DSAT_OK ) 
      {
        DS_3GPP_MSG0_HIGH("RMSM_PPP_TRANS_STATE: ANS and HUP registered successfully!");
      }
      else
      {
        DATA_ERR_FATAL("RMSM_PPP_TRANS_STATE: err! unknown ret code");
        return;
      }

      ds3g_siolib_set_answer_callbacks( &dsumts_rmsm_ppp_siolib_func_tbl );

      DS_3GPP_MSG0_HIGH("RMSM_PPP_TRANS_STATE: RING TE! with SIOLIB auto-ans set");
      ds3g_siolib_alert_user( DSAT_RING, 
                              TRUE );     /*Perform auto-answer*/
      break; 
#endif /*FEATURE_DATA_TE_MT_PDP*/
  
    default:
      DS_3GPP_MSG1_ERROR ( "Invalid state %d transition", new_state);
      ASSERT(0);
      break;

  } /* switch(new state) */
} /* dsumts_rmsm_ppp_transition_state() */

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */
