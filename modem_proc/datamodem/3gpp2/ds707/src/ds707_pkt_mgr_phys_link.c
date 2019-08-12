/*===========================================================================

               D S 7 0 7 _ P K T _ M G R _ P H Y S _ L I N K . C
               
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls and the PS phys link.
  
EXTERNALIZED FUNCTIONS
  ds707_pkt_phsy_link_init() - Creates the 1x pkt ps_phys_link struct.
  
  ds707_pkt_phys_link_process_cmd() - Process cmd's specifc to the 1x
    ps_phys_link handling.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_pkt_phys_link_init() at startup.

  Copyright (c) 2002-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_phys_link.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/23/14    sd     Fixed compialtion errors when FEATURE_EPC_HANDOFF is 
                   disabled.
08/05/14    ms     DSDA Phase 2 changes.
06/03/13    sd     ATCoP decoupling changes
01/08/13    sd     Disable the physical link flow when RETRY DELAY is triggered
11/21/12    sd     Feature Dual SIM related changes.
01/18112    jz     Previously for ds707_pkti_phys_link_bring_up_cmd_hdlr(), when 
                   it's in JCDMA, dial string was set to empty. Remove this for
                   new KDDI requirement.
12/23/11    sb     Fixed compiler warnings 
11/18/11    vpk    HDR SU API cleanup
03/15/11    mg     Global variable cleanup
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
03/04/09    sa     AU level CMI modifications.
02/15/09    ls     If epzid is changed, we need to set IS707B data call type
                   before calling ds3g_msh_initate_call(). Initialize epzid
                   in ds3g_pkt_call_info_type.
10/06/08    sn     Fix to make sure HDR SMP deactivate is not sent when 1x 
                   data call ends.
07/02/07    sc     Set the Iface ptr to cmd_ptr->cmd.ds707_data.iface_ptr in
                   ds707_pkti_phys_link_bring_up_cmd
05/10/07    ac     Lint fix
01/10/07    gr     Changes to populate the call end reason related parameters
                   in the event_info correctly while publishing 
                   PHYS_LINK_DOWN_EV 
01/05/06    ksu    EMPA: rx queues per tc state
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
10/27/05    ksu    EMPA: octet stream, octet sequencing
06/28/05    gr     Fixed the issue of going NULL from Dormancy. Teardown
                   handler issues a PHYS_LINK_DOWN_EV to handle this scenario
06/23/06    squ    In ds707_pkti_phys_link_bring_up_cmd_hdlr function, added
                   call_end_info to the event_info when origination fails.
05/16/05    gr     Moved the populating event_info in bring_up_hdlr within
                   the check for phys link state
04/26/05    gr     Added inc_call flag to ds3g_pkt_call_info_type
04/11/05    vrk    fix Digit Mode used during Data call origination
04/05/05    gr     Moved the phys link ioctl handler into ds707_pkt_mgr_ioctl
04/05/05    gr     Integrated EPZID functionality onto the main line
03/14/05    vas    Removed inclusion of ds707_pkt_mgri.h. Not needed anymore
11/22/04    vas    Added function ds707_pkt_phys_link_ioctl
10/25/04    sk     Fixed delayed retry order problem - CR 49333
10/14/04    msr    Removed calls to PS_PHYS_LINK_SET_GRACEFUL_CLOSE().
                   Instead ds707_pkt_mgr.c calls
                   PS_IFACE_SET_GRACEFUL_CLOSE() for each iface.
09/08/04    vas    Added code for QOS (secondary pkt mgr)
04/24/04    ak     Lint fixes.
04/07/04    gr     Modified ds707_pkt_phys_link_bring_up_cmd_hdlr. 
                   to pass the pointer to the entire ds3g_pkt_call_info_type 
                   instead of individual fields of the structure
03/17/04    ak     Fixed F3 typo.
03/10/04    ak     Code review comments.
12/30/03    ak     Code created from original ds707_pkt_mgr.c.
11/13/03    vas    Ensure XOFF is sent on RTC when PPP ended.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#define FEATURE_DSM_WM_CB
#include "amssassert.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "ds707_cback.h"
#include "ds707_pkt_mgr_phys_link.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr_ioctl.h"
#include "dsat707util.h"
#include "dstask.h"
#include "dstaski.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "nv.h"
#include "ps_phys_link.h"
#include "ps_phys_link.h"

#ifdef FEATURE_HDR
  #include "hdrpac.h"
  #include "hdrpac_api.h"
  #include "hdrcom_api.h"
#endif /* FEATURE_HDR */

#ifdef FEATURE_HDR_QOS
#include "ds707_sec_pkt_mgr.h"
#include "ds707_sec_pkt_mgr_util.h"
#endif /* FEATURE_HDR_QOS */

#include "ds707_roaming.h"
#include "data_err_suppr.h"
#include "ds707_so_retrydel.h"

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_pdn_context.h"
  #include "ds707_s101.h"
  #include "ds707_pkt_mgr_hdlr.h"
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_EHRPD
 #include "ds707_sys_chg_hdlr.h"
#endif /* FEATURE_EHRPD */
/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                                 TYPEDEFS
===========================================================================*/

/*===========================================================================
                                VARIABLES
===========================================================================*/

/*===========================================================================
                        INTERNAL MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                             PHYS_LINK_DOWN
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_TEAR_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context when traffic channel needs to be
              torn down.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_phys_link_tear_down_cmd_hdlr
(
  ds3g_pkt_call_info_type *mode_spec_tear_info
)
{
  ds3g_pkt_call_info_type     mode_spec_info;
  ds707_tc_state_type         *tc_state_ptr, 
                              *temp_tc_state_ptr;
  ps_phys_link_type           *phys_link_ptr, 
                              *this_phys_link_ptr,
                              *temp_phys_link_ptr;
  ds707_cback_event_info_type  event_info;
  uint32             pkt_instance = mode_spec_tear_info->pkt_instance;
  int16                        ps_errno;
  uint8                        i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(mode_spec_tear_info);
  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "DS707 PKT Phys link down cmd hdlr");

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(pkt_instance);
  mode_spec_info.pkt_instance = pkt_instance;

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  tc_state_ptr   = ds707_get_pri_tc_ptr();

  this_phys_link_ptr = mode_spec_tear_info->phys_link_ptr;  

  /* Only primary phys link should reach this point. SEC phys link won't 
     post the DS_707_PKT_PHYS_LINK_DOWN_CMD
   */
  ASSERT(phys_link_ptr == this_phys_link_ptr);
  
  if ((PS_PHYS_LINK_GET_STATE(this_phys_link_ptr) != PHYS_LINK_GOING_NULL)&&
      (PS_PHYS_LINK_GET_STATE(this_phys_link_ptr) != PHYS_LINK_GOING_DOWN))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                    "Phys link is not in PHYS_LINK_GOING_NULL or "
                    "PHYS_LINK_GOING_DOWN, do nothing");
	 return;
  }

#ifdef FEATURE_HDR_QOS
  /* This is PRI physlink put all the SEC physlinks in transient state */
  for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
  {
    temp_tc_state_ptr = ds707_get_sec_tc_ptr(i);
    temp_phys_link_ptr = ds707_get_phys_link_ptr(i);
    if (ds707_sec_pkt_is_tc_state_allocated(temp_tc_state_ptr) == FALSE) 
    {
      continue;
    }
    if ((PS_PHYS_LINK_GET_STATE(temp_phys_link_ptr) != 
                                              PHYS_LINK_GOING_DOWN)&&
        (PS_PHYS_LINK_GET_STATE(temp_phys_link_ptr) != 
                                               PHYS_LINK_GOING_NULL))
    {
      (void) ps_phys_link_down_cmd( temp_phys_link_ptr, 
                                    &ps_errno, 
                                    NULL);
    }
  }

#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR
  /*------------------------------------------------------------------------
    To ensure that the Xoff message is sent on the RTC, Queue up the Xoff
    message before giving the End command. This has to be called before the 
    End comamnd is given because since the CM/HDRMC are higher priority, the
    End command will cause a context switch to HDRMC. HDRMC will then 
    initiate a call end before the Xoff message can be queued up.
  ------------------------------------------------------------------------*/
  if( DS_ISDATA_HDR((ds707_get_pri_tc_ptr())->so)  &&
      (ds707_pkti_can_go_dormant( tc_state_ptr, 
                                  phys_link_ptr) == FALSE))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Call rel-disable SN flow");
    if ((hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN)) &&
         (hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_SN) == TRUE))
    {
      hdrpac_flow_disable( HDRHAI_DEF_PACKET_APP_SN );
    }
#ifdef FEATURE_HDR_QOS
    else if((hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) &&
            (hdrpac_app_flow_is_enabled(HDRHAI_MULTIFLOW_PACKET_APP_SN) ==
                                                                       TRUE))
    {
      hdrpac_flow_disable( HDRHAI_MULTIFLOW_PACKET_APP_SN );
    }
    else if ((hdrcp_stream_stream_is_assigned(
                                HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)) &&
         (hdrpac_app_flow_is_enabled(
                          HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) == TRUE))
    {
      hdrpac_flow_disable( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN );
    }
#ifdef FEATURE_HDR_REVB
    else if ((hdrcp_stream_stream_is_assigned(
                                HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) &&
             (hdrpac_app_flow_is_enabled(
                          HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) == TRUE))
    {
      hdrpac_flow_disable( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN );
    }
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */
  }
#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_OPTHO
  if(DS707_TUN_PHYS_LINK_NULL != ds707_get_tun_phys_link_state(tc_state_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Tearing down Tunnel call via HDR");
    hdrpac_close_tunnel_connection();

    /*-----------------------------------------------------------------------
      We do not wait for a response (call end) from HDR and
      continue cleaning up our physlink resources.
      
      Reason: HDR might be using the virtual connection and
      may not bring it down until it's done. So DS shouldn't
      wait for a response after issuing hangup tunnel call. When
      the call ended indication is eventually recvd, tunnel
      call ended hdlr will ignore it since phys link will already
      be in Down/Null state
    -----------------------------------------------------------------------*/

    /* OPTHO_ToDo: Set the correct call end reason here */
    ds707_pkti_tunnel_phys_link_cleanup( DS707_S101_DOWN_REASON_CLIENT_END,
                                         tc_state_ptr, 
                                         phys_link_ptr );
  }
  else
#endif /* FEATURE_DATA_OPTHO */
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Tearing down data call via CM");
    if (ds3g_msh_hangup_call_ex((ds707_get_pri_tc_ptr())->call_id,
                             &mode_spec_info,
                             SYS_MODEM_AS_ID_1) == DS3G_FAILURE)
    {
      /*-----------------------------------------------------------------------
        The call has already ended.  Do nothing here, as the call_ended handler
        has been/will be called.  That will do most of the work.
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Call already ended");
  
      /*-----------------------------------------------------------------------
        If the PPP goes down while the Packet Call is dormant, the tear down 
        handler has to give a phys_link_gone indication to do the cleanup 
        of the Phys link and all the associated resources
      -----------------------------------------------------------------------*/
      if ( ds707_pkti_can_go_dormant( tc_state_ptr,
                                      phys_link_ptr) == FALSE) 
      {
        event_info.so           = tc_state_ptr->so;
        event_info.pkt_inst     = pkt_instance;
        event_info.curr_network = ds707_pkt_get_current_data_sess_network();
  
        event_info.failed_orig  = FALSE;
        event_info.call_cmd_err = CM_CALL_CMD_ERR_NOERR;
        event_info.going_dormant = FALSE;
  
        /*---------------------------------------------------------------------
         Update the last data sess network to what the network was when hanging 
         up the call. It will be updated to NO_SRV while the call goes NULL. 
         The update to NO_SRV happens while processing the PHYS_LINK_DOWN 
         Event.
        ---------------------------------------------------------------------*/
        ds707_pkt_set_last_data_sess_network(event_info.curr_network);
  
      /*-----------------------------------------------------------------------
        Perform a little housekeeping by issuing a PHYS_LINK_DOWN_EV to tear
        down the PPP completely in case of a PPP down event
      -----------------------------------------------------------------------*/
        ds707_cback_post_event(
                                DS707_CBACK_PHYS_LINK_DOWN_EV,
                                &event_info
                              );
      }
    }
  }

  /*-------------------------------------------------------------------------
    This function is done.  Wait for the CALL_RELEASED command from
    3G DSMGR before saying phys link is completely down.  
  -------------------------------------------------------------------------*/
} /* ds707_pkti_phys_link_tear_down_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_TEAR_DOWN_CMD

DESCRIPTION   Called via the 1x IFACE.  Tears down the traffic channel.
              Sends a message to the DS task which will cause the
              channel to be torn down.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715  */
LOCAL int ds707_pkti_phys_link_tear_down_cmd
(
  ps_phys_link_type   *phys_link_ptr,
  void                *not_used
)
{
  ds_cmd_type     *cmd_ptr;                      /* DS Task msg            */
  ps_phys_link_type         *pri_phys_link_ptr;
  phys_link_state_type       pri_phys_link_state;
  int16                      ps_errno;
  ds3g_pkt_call_info_type  *pkt_call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_ARG_NOT_CONST(not_used);
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "DS707 PHYS LINK DOWN CMD");
  
  pri_phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  pri_phys_link_state = PS_PHYS_LINK_GET_STATE(pri_phys_link_ptr); 

  /* If phys link tear down was requested on a secondary physlink, 
   * resend the request on the primary physlink. Design is such 
   * that actual tear down through CM happens only for the
   * primary physlink
   */
  if (phys_link_ptr != pri_phys_link_ptr) 
  {
    /* No need to tear down a secondary physlink if the primary 
     * physlink is already in the process of being torn down
     */
    if ((pri_phys_link_state == PHYS_LINK_GOING_NULL) ||
          (pri_phys_link_state == PHYS_LINK_GOING_DOWN))
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                   "Primary phys link is already being torn down. Do nothing");
      return 0;
    }
    /* If primary is physlink is not already in the process of being 
     * torn down, tear it down
     */
    (void) ps_phys_link_down_cmd( pri_phys_link_ptr, 
                                  &ps_errno, 
                                  NULL);
    return 0;
  }

  /* If the request is for the primary physlink, tear it down through CM
   */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_pkt_call_info_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return -1;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_PHYS_LINK_DOWN;
  pkt_call_info_ptr = (ds3g_pkt_call_info_type *)cmd_ptr->cmd_payload_ptr;

  /*lint -save -e507  */
  /* Setting the Pkt Instance to 0, as we have only one */
  pkt_call_info_ptr->pkt_instance = 0;

  /* Passing physlink info as well */
  pkt_call_info_ptr->phys_link_ptr = phys_link_ptr;

  /*lint -restore */

  ds_put_cmd(cmd_ptr);

  return(0);
} /* ds707_pkti_phys_link_tear_down_cmd() */
/*lint -restore */


/*===========================================================================
FUNCTION      DS707_PKTI_POST_PHYS_LINK_COMING_UP_OR_RESUMING_EV

DESCRIPTION   Utility Function to post ds707 phys link coming up or resuming 
              depending on the PS Phys link state and the previous call orig
              medium (HDR/CM)
              
DEPENDENCIES  None

RETURN VALUE  TRUE  - If Phys link is in coming up or resuming state 
              FALSE - Otherwise 

SIDE EFFECTS  None.
===========================================================================*/
static boolean ds707_pkti_post_phys_link_coming_up_or_resuming_ev
(
  ps_phys_link_type           *phys_link_ptr,
  ds707_cback_event_info_type  *event_info               
)
{
#ifdef FEATURE_DATA_OPTHO
  ds707_tc_state_type *tc_state_ptr = ds707_get_pri_tc_ptr();
#endif /* FEATURE_DATA_OPTHO */

  /*-------------------------------------------------------------------------
   Populate the event info to handle dormancy with TRUE only when resuming
   from dormancy. For coming out of NULL, the mobile cannot go dormant if
   the newly initiated call fails. 
  -------------------------------------------------------------------------*/
  if(PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP) 
  {
    event_info->going_dormant = FALSE;
    ds707_cback_post_event( DS707_CBACK_PHYS_LINK_COMING_UP_EV, 
                            event_info);
    return TRUE;
  }
  else if(PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING) 
  {
    event_info->going_dormant = TRUE;

#ifdef FEATURE_DATA_OPTHO
    /*---------------------------------------------------------------------- 
      When a context is brought up in tunnel mode, the transition from
      phys link NULL to Phy link coming up is notified only to certain
      clients that listen to tunnel phys link events. When handing over the
      preregistered contexts in eHRPD direct mode, all modules will receive
      phys link resuming event via DS707_CBACK_PHYS_LINK_RESUMING_EV.
      The modules that are unaware of tunnel mode will be getting only this
      resuming event. If they distinguish DS707_CBACK_PHYS_LINK_RESUMING_EV
      and DS707_CBACK_PHYS_LINK_COMING_UP_EV (e.g registering some call
      backs only in coming up ev), then they need to be modified to handle
      the case of receiving a resuming event without receiving a coming up
      event earlier. 
    
      Instead, one solution is to fake the phys link to be in coming up
      state as shown here. However, this logic just reverses the problem
      (if tunnel mode clients differentiate between coming up and resuming
      events)
    
      Currently there are only two tunnel mode clients(pkt mgr and cta) and
      they don’t distinguish coming up/resuming. So the logic of posting
      coming up instead of resuming is implemented here.
    
      In future if a new tunnel mode client is added and it differentiates
      these two events, then it should also be able to handle receiving
      two coming_up events due to the change implemented here
    ----------------------------------------------------------------------*/
    if(DS707_TUN_PHYS_LINK_NULL != ds707_get_tun_phys_link_state(tc_state_ptr))
    {
     /* The if check implies that the earlier call bring up was 
        for a tunnel call */
     ds707_cback_post_event( DS707_CBACK_PHYS_LINK_COMING_UP_EV, 
                             event_info);
    }
    else
#endif /* FEATURE_DATA_OPTHO */
    {
     ds707_cback_post_event( DS707_CBACK_PHYS_LINK_RESUMING_EV, 
                             event_info);
    }
    return TRUE;
  }
  else
  {
    /* PS Phys link neither in Coming up/resuming state*/
    return FALSE;
  }
} /* ds707_pkti_post_phys_link_coming_up_or_resuming_ev */

/*===========================================================================
FUNCTION      DS707_PKTI_POST_PHYS_LINK_DOWN_EV_DURING_BRINGUP

DESCRIPTION   Utility Function to post ds707 phys link down event when the 
              phys link bring up couldn't be completed 
              
DEPENDENCIES  None

RETURN VALUE  None 
 
SIDE EFFECTS  None.
===========================================================================*/
static void ds707_pkti_post_phys_link_down_ev_during_bringup
(
  ds707_cback_event_info_type  *event_info               
)
{
  ds707_cm_call_end_info_type  call_end_info;

  /* OPTHO_ToDo: do we need to set ds707_pkt_mgr_set_last_call_end_reason here?
     This reason is what DSSNET relies on to send to upper layers */
  call_end_info.highest_call_fail_reason = CM_CALL_ORIG_ERR_LAYER_NONE;
  call_end_info.end_status = CM_CALL_END_INTERNAL_ERROR;
  event_info->call_end_info = call_end_info;
  event_info->failed_orig = TRUE;
  event_info->call_cmd_err = CM_CALL_CMD_ERR_NOERR;
  
  ds707_cback_post_event(DS707_CBACK_PHYS_LINK_DOWN_EV, event_info);

} /* ds707_pkti_post_phys_link_down_ev */

/*===========================================================================
                             PHYS_LINK_BRING_UP
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_BRING_UP_CMD_HDLR

DESCRIPTION   Called in DS TASK context to bring up traffic channel.

              All traffic channel originations must come through this
              function.

DEPENDENCIES  Called in DS TASK context.  Assumes that PS_IFACE has already
              set phys_link state to COMING_UP.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_phys_link_bring_up_cmd_hdlr
(
  ds3g_pkt_call_info_type *mode_spec_orig_info
)
{
  dsat_dial_val_e_type         dial_str_digits_type;
  boolean 	                   dial_str_digits_mode;
  byte                         dial_str[NV_MAX_PKT_ORIG_DIGITS];
  ds707_cback_event_info_type  event_info;
  ds707_tc_state_type         *temp_tc_state_ptr;
  ps_phys_link_type           *temp_phys_link_ptr;
  ps_phys_link_type           *phys_link_ptr;
  ps_phys_link_type           *this_phys_link_ptr;
  boolean                      inc_call;
  uint8                        i;
  int16                        ps_errno;
  ds_call_type_e_type          call_type;
  ds_sys_subscription_enum_type iface_subs_id = DS_SYS_PRIMARY_SUBS;  
  sys_modem_as_id_e_type        resolved_subs_id=SYS_MODEM_AS_ID_1;

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
  ds707_pdn_throttle_info_type          * throttle_info_ptr = NULL;
  ds707_data_session_profile_info_type  * data_sess_profile_info_ptr = NULL;
    ds707_iface_table_type              * iface_tbl_ptr = NULL;
#endif /*FEATURE_DATA_EHRPD_PDN_THROTTLE*/
#endif /*FEATURE_EHRPD*/

  ps_iface_type               *iface_ptr = NULL;
  ps_iface_type               *ds707_iface_ptr = NULL;
  uint8                       iface_index;

#ifdef FEATURE_DATA_OPTHO
  ds707_tc_state_type         *tc_state_ptr = NULL;
  ds707_pdn_context_cb_type   *ds707_pdn_cb_ptr = NULL;
  acl_handoff_ctxt_type       *handover_ip_config_handle = NULL;
  ds707_s101_sm_e             s101_sm_state = DS707_S101_SM_NON_TUNNEL;
  boolean                     call_bring_up_via_hdr = FALSE;
  boolean                     reject_tunnel_call_bringup = FALSE;
  uint32                      end_reason = DS707_S101_DOWN_REASON_UNSPECIFIED;
#endif /* FEATURE_DATA_OPTHO */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "DS707 PKT PHYS LINK UP CMD HDLR");

  ASSERT(IS_IN_DS_TASK());

  VERIFY_PKT_INSTANCE(mode_spec_orig_info->pkt_instance);

  inc_call = mode_spec_orig_info->inc_call;
  this_phys_link_ptr = mode_spec_orig_info->phys_link_ptr;

  /*-------------------------------------------------------------------------
     For an incoming call PHYS_LINK has to be brought up to RESUMING state.
     ps_phys_link_up_cmd called in response to the incoming call just needs
     to transition the PHYS_LINK_STATE to RESUMING. Rest of the processing
     in the bring_up_cmd_hdlr are applicable only for Origination attempts
  -------------------------------------------------------------------------*/
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();

  /* Only primary phys link should reach this point. SEC phys link won't post the
     DS_707_PKT_PHYS_LINK_UP_CMD
   */
  ASSERT(phys_link_ptr == this_phys_link_ptr);
  
  if ((PS_PHYS_LINK_GET_STATE(this_phys_link_ptr) != PHYS_LINK_COMING_UP)&&
      (PS_PHYS_LINK_GET_STATE(this_phys_link_ptr) != PHYS_LINK_RESUMING))
  { 
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Phys link is not in PHYS_LINK_COMING_UP or"
                     "PHYS_LINK_RESUMING, do nothing ");
	 return;
  }

#ifdef FEATURE_HDR_QOS
 /* This is PRI physlink, put all the SEC physlinks in transient state */
  for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
  {
    temp_tc_state_ptr = ds707_get_sec_tc_ptr(i);
    temp_phys_link_ptr = ds707_get_phys_link_ptr(i);
      if (ds707_sec_pkt_is_tc_state_allocated(temp_tc_state_ptr) == FALSE) 
      {
        continue;
      }
      if ((PS_PHYS_LINK_GET_STATE(temp_phys_link_ptr) != 
                                                      PHYS_LINK_COMING_UP)&&
          (PS_PHYS_LINK_GET_STATE(temp_phys_link_ptr) != PHYS_LINK_RESUMING))
      {
        /* Not necessary to call for FWD phys)links but does not hurt. 
           Revisit later
         */
        (void) ps_phys_link_up_cmd( temp_phys_link_ptr, 
                                    &ps_errno, 
                                    (void*)mode_spec_orig_info);
      }
    }
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_DATA_OPTHO
   tc_state_ptr   = ds707_get_pri_tc_ptr();
#endif  /* FEATURE_DATA_OPTHO */

   /* Note: iface_ptr is only filled in when a call is brought up by
      DSSNET. For other cases (e.g reorig) iface_ptr is not filled in */
   iface_ptr = mode_spec_orig_info->iface_ptr;
   
   if (NULL != iface_ptr)
   {
     /* Run through the stored 707 ifaces to ensure that the passed in
        iface is actually valid. PS_IFACE_IS_VALID just checks if the
        passed in iface_ptr is !NULL and tries to dereference which
        might trigger a memory exception if clients pass in some random
        ptr */

     for (iface_index = 0; iface_index < DS707_MAX_IFACES; iface_index++)
     {
       ds707_iface_ptr = NULL ;
       ds707_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index((uint8)iface_index);

       if (ds707_iface_ptr == iface_ptr)
       {
         DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                       "Valid iface ptr passed to bringup cmd hdlr");
         iface_subs_id = PS_IFACE_GET_SUBS_ID (iface_ptr);
         
         if( ds3g_resolve_subs_id ( iface_subs_id,&resolved_subs_id ) == FALSE )
         {
           DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                   "Policy Sub ID %d",iface_subs_id,resolved_subs_id);
           return;
         }
         break;
       }
     }

     if (iface_index != DS707_MAX_IFACES)
     {
#ifdef FEATURE_DATA_OPTHO
       ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
       if (DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr))
       {
#ifdef FEATURE_EPC_HANDOFF       
         handover_ip_config_handle = 
         (acl_handoff_ctxt_type*) ds707_pdn_cb_ptr->handover_ip_config_handle;
#endif /* FEATURE_EPC_HANDOFF */
       }
#endif  /* FEATURE_DATA_OPTHO */
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
       if ( ds707_data_session_get_iface_requesting_profile(iface_ptr, 
                                         &data_sess_profile_info_ptr) == TRUE)
       {
         if ( ds707_pdncntx_is_iface_throttled(data_sess_profile_info_ptr, 
                                        ps_iface_get_addr_family( iface_ptr )) )
         {
           DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                           "PDN is throttled. NO phyiscal link bring up.");
           /*------------------------------------------------------------- 
             Set ps net down reason 
          ---------------------------------------------------------------*/

           iface_tbl_ptr = 
             ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);

           if ( ps_iface_addr_family_is_v4(iface_ptr) ) 
           {
             ds707_pdncntx_v4_throttling_set_ps_call_end_reason(
               throttle_info_ptr,
               iface_tbl_ptr);
           }
           else if( ps_iface_addr_family_is_v6(iface_ptr) )
           {
             ds707_pdncntx_v6_throttling_set_ps_call_end_reason(
               throttle_info_ptr,
               iface_tbl_ptr);
           }
           else
           {
             ASSERT(0);
             return;
           }

           event_info.failed_orig = TRUE;
           event_info.going_dormant = FALSE;

          /*------------------------------------------------------------- 
             Post Physical link down event 
          ---------------------------------------------------------------*/
#ifdef FEATURE_DATA_OPTHO
           if ((DS707_S101_SM_TUNNEL_CALL_ALLOWED == s101_sm_state) 
               || (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == s101_sm_state))
           {
             DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                           "Posting phyiscal link down event to DS task for "
                           "s101.");
             ds707_cback_post_event(DS707_CBACK_TUN_PHYS_LINK_DOWN_EV, &event_info);
             return;
           }
#endif /* FEATURE_DATA_OPTHO */

          /*------------------------------------------------------------- 
            Fill in event info for s101 non-tunnel and non-s101 case
          ---------------------------------------------------------------*/

           DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                           "Posting phyiscal link down event to DS task");
           ds707_cback_post_event(DS707_CBACK_PHYS_LINK_DOWN_EV, &event_info); 
           return;

         }
       } 
#endif /*FEATURE_DATA_EHRPD_PDN_THROTTLE*/
#endif /*FEATURE_EHRPD*/
     }
     else
     {
       DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                       "Invalid iface ptr passed to bringup cmd hdlr");
     }
   }
   else
   {
     DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                       "NULL iface ptr passed to bringup cmd hdlr");
   }

#ifdef FEATURE_DATA_OPTHO
   /*----------------------------------------------------------------------- 
      Query S101 SM State. Based on the state and the policy info determine
      the course of action
      Logic:
      S101 = None or sys chg sm in some transition state(req for 1x sp
                                                         call bringup)
        if iface policy set and = prereg
            post phys link down (i.e reject call)
        else call via CM

      S101 = TUN allowed
        if iface policy set and != prereg
            post tun phys link down (i.e reject call)
        else call via hdr
      
      S101 = Tun Disallowed
        post tun phys link down (i.e reject call)
  -----------------------------------------------------------------------*/
  s101_sm_state = ds707_s101_query_sm_state();

  /* Even if pref sys is LTE (S101 SM will be in tunnel allowed/disallowed
     sate), we need to bring up the sp srv call via CM to 1XCP if
     sys chg sm is in 1X_SP_CALL_IN_PROG state. So skip the Optho
     logic below */
  if (DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG == ds707_query_sys_chg_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Sys chg Sm in DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG state. "
                    "Bring up call via CM");
    call_bring_up_via_hdr = FALSE;
  }
  else if (DS707_S101_SM_NON_TUNNEL == s101_sm_state)
  {
    if ((NULL != handover_ip_config_handle) && 
        (PS_IFACE_HANDOFF_CLASS_EPC_PREREG == 
                handover_ip_config_handle->handoff_class)
       )
    {
      /* First post phys link coming_up/resuming before posting down. This
         is to ensure that we transition ds707 Phys Link states in sync with
         PS phys link states*/
      event_info.pkt_inst = mode_spec_orig_info->pkt_instance;
      event_info.failed_orig  = FALSE; /* Default */
      event_info.curr_network = ds707_pkt_get_current_data_sess_network();

      if (!ds707_pkti_post_phys_link_coming_up_or_resuming_ev
                                              (phys_link_ptr, &event_info)
         )
      {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                       "Could not originate pkt call, phys_link state %d",
                       PS_PHYS_LINK_GET_STATE(phys_link_ptr));
       return;
      }

      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "S101 SM in Non Tunnel state "
                     "but handoff policy is EPC_PREREG - Reject Call Bringup");
      ds707_pkti_post_phys_link_down_ev_during_bringup(&event_info); 
    }
    else
    {
      call_bring_up_via_hdr = FALSE;
    }
  
  }
  else /* S101 SM in Tunnel Allowed/Disallowed State*/
  {
    /*------------------------------------------------------------------------- 
       Note: Immaterial of whether we can bring up tunnel phys link or not
       move the ds707 phys link state to coming up/resuming to be in
       sync with PS Phys link state
    -------------------------------------------------------------------------*/

    /* Fill in event_info with default values */
    event_info.failed_orig  = FALSE;
    
    /* For calls brought up via CM, clearing happens as part of orig hdlr.
      Since for Tunnel calls, we dont have a orig hdlr, clear it here*/
    ds707_pkt_mgr_clear_last_call_end_reason();
    
    /*-------------------------------------------------------------------------
     Populate the event info to handle dormancy with TRUE only when resuming
     from dormancy. For coming out of NULL, the mobile cannot go dormant if
     the newly initiated call fails. 
    -------------------------------------------------------------------------*/
    if(PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP) 
    {
      event_info.going_dormant = FALSE;
      ds707_cback_post_event( DS707_CBACK_TUN_PHYS_LINK_COMING_UP_EV, 
                             &event_info);
    }
    else if(PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING) 
    {
      event_info.going_dormant = TRUE;
      ds707_cback_post_event( DS707_CBACK_TUN_PHYS_LINK_RESUMING_EV, 
                             &event_info);
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                     "Could not originate pkt call, phys_link state %d",
                     PS_PHYS_LINK_GET_STATE(phys_link_ptr));
      return;
    }


    /*------------------------------------------------------------ 
      Based on S101 SM state and iface handoff policy determine
      whether to allow/reject tunnel call bringup
    ------------------------------------------------------------*/
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == s101_sm_state)
    {
      /* If there is a specific handoff class mentioned that is not prereg
         reject the call*/
      if ((NULL != handover_ip_config_handle) && 
          (PS_IFACE_HANDOFF_CLASS_EPC_PREREG != 
                handover_ip_config_handle->handoff_class)
          )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                       "Handoff Policy not EPC_PREREG. Reject Tunnel Call");
        reject_tunnel_call_bringup = TRUE;
        end_reason = DS707_S101_DOWN_REASON_CALL_INVALID_CONFIG;
      }
      else
      {
        call_bring_up_via_hdr = TRUE;
      }
    }

    if (reject_tunnel_call_bringup)
    {
      event_info.failed_orig  = TRUE;
      
      /*OPTHO_ToDo: If failed due to sys throttled, Set 
       this to the new PS call end reason */
      ds707_pkt_mgr_set_last_tun_call_end_reason( end_reason );
      
      ds707_cback_post_event(DS707_CBACK_TUN_PHYS_LINK_DOWN_EV, &event_info);
      return;
    }
  }

  if (call_bring_up_via_hdr)
  {
   /*------------------------------------------------------------------------
                           CALL BRINGUP VIA HDR
   ------------------------------------------------------------------------*/
   DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "Handoff Policy set to PREREG and/or S101 in Tunnel Mode."
                   "Bringing up Tunnel call via HDR");
  
   /* Any failure to initiate call will be notified async via HDR ind */
   hdrpac_open_tunnel_connection();
   
   /*Move the tun phys link state to Coming Up 
     Note: We do not distinguish coming up or resuming. We treat them as same
     Revisit in future if necessary */
   ds707_transition_tun_phys_link_state(tc_state_ptr, DS707_TUN_PHYS_LINK_COMINGUP);
  }
  else
#endif /* FEATURE_DATA_OPTHO */
  {
    /*------------------------------------------------------------------------
                             CALL BRINGUP VIA CM
    ------------------------------------------------------------------------*/
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                    "Bringing up call via CM on subs_id %d",resolved_subs_id);

    ds3gflowmgr_notify_ps_call_state_per_sub ( SYS_MODEM_AS_ID_1, 
                                               DS3GFLOWMGR_PS_CALL_ORIG_EV );
     
     if (inc_call == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "Phys link up cmd during incoming call");
#ifdef FEATURE_DATA_OPTHO
      /* Transition tun phys link to NULL. This essentially means that
         the phys link has been taken over in direct mode */
      ds707_transition_tun_phys_link_state(tc_state_ptr, DS707_TUN_PHYS_LINK_NULL);
#endif /* FEATURE_DATA_OPTHO */
      return;
    }
   
    dial_str_digits_type = dsat_proc_dial_str(ds707_pkt_mgr_orig_str,
                                                dial_str);
 
    /*-------------------------------------------------------------------------
      Post event so any required set up happens.  Note that the call to 
      initiate_call() below will most likely result in a task switch to MC
      and CM before returning.
    -------------------------------------------------------------------------*/
    event_info.pkt_inst = mode_spec_orig_info->pkt_instance;
    
    /*-------------------------------------------------------------------------
      If call is going dormant, set watermark callback to initiate leaving
      dormancy.  See if holddown timer is required.
    -------------------------------------------------------------------------*/
    /* populate members of structure */ 
    event_info.failed_orig  = FALSE; /* Default */
    event_info.curr_network = ds707_pkt_get_current_data_sess_network();
    /* populate ends */
  
    /* post phys link coming up/resuming event */
    if (!ds707_pkti_post_phys_link_coming_up_or_resuming_ev(phys_link_ptr, &event_info))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Could not originate pkt call, phys_link state %d",
                      PS_PHYS_LINK_GET_STATE(phys_link_ptr));
      return;
    }
    /*-------------------------------------------------------------------
      Determine the digit mode 
      TRUE = dial str has non-digit chars
      FALSE = dial str has only digits
    -------------------------------------------------------------------*/
  
    if ( (dial_str_digits_type == DSAT_DIAL_DIGIT) || 
         (dial_str_digits_type == DSAT_DIAL_EMPTY))
    {
        dial_str_digits_mode = FALSE; 
    }
    else
    {
        dial_str_digits_mode = TRUE; 
    }
    /*-------------------------------------------------------------------------
    If epzid is changed and drs_bit is FALSE, we need to set up IS707B data call type. 
    -------------------------------------------------------------------------*/
    if ((mode_spec_orig_info->epzid == TRUE)&&
        (mode_spec_orig_info->drs_bit == FALSE))
    {
      call_type = DS_CALL_TYPE_PKT_IS707B;
    }
    else 
    {
      call_type = DS_CALL_TYPE_PKT;
    }
   /* ----------------------------------------------------------------------------
     For now, call is originated on default subs id SYS_MODEM_AS_ID_1, if no iface_ptr 
     is passed but in future, this might need to be changed such that we pass the 
     subs_id in orig_pkt_call_params from  phys link orig func.
   ----------------------------------------------------------------------------*/
    if (ds3g_msh_initiate_call_ex(call_type,
                               SYS_SYS_MODE_CDMA,
                               (const byte *)(dial_str),
                               dial_str_digits_mode,
                               mode_spec_orig_info,
                               resolved_subs_id
                              ) == DS3G_FAILURE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not originate pkt call");
      ds707_pkti_post_phys_link_down_ev_during_bringup(&event_info);
      
    }
#ifdef FEATURE_DATA_OPTHO
    else
    {
      /* Transition tun phys link to NULL. This essentially means that
         the phys link has been taken over in direct mode */
      ds707_transition_tun_phys_link_state(tc_state_ptr, DS707_TUN_PHYS_LINK_NULL);
    }
#endif /* FEATURE_DATA_OPTHO */
  }
} /* ds707_pkti_phys_link_bring_up_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_BRING_UP_CMD

DESCRIPTION   Called via the 1x ps_phys link.  Brings up the 1X traffic
              channel.  Specific to the zero'th 1X pkt interface.

              Since this is called by some module external to 1X, DRS_BIT
              is always set to TRUE (always data to be sent).

DEPENDENCIES  Watermarks to RLP already set up correctly.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 unused variables*/
LOCAL int ds707_pkti_phys_link_bring_up_cmd
(
  ps_phys_link_type    *phys_link_ptr,
  void                 *info_ptr
)
{
  ds_cmd_type               *cmd_ptr;                     /* DS Task msg            */
  ds3g_pkt_call_info_type  *pkt_info_ptr;
  int16                      ps_errno;
  ps_phys_link_type         *pri_phys_link_ptr;
  phys_link_state_type       pri_phys_link_state;
  ds3g_pkt_call_info_type  *cmd_pkt_call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(phys_link_ptr);
  
    /*------------------------------------------------------------------------- 
    For RATs other than LTE, we would have torn down 1x/HDR context on sys
    change. But for LTE, we keep eHRPD context to support handoff. This may
    lead to some unintentional data coming on eHRPD IFACEs and will end up
    bringing up 1x/HDR CM call. This will abort the ongoing LTE data call
    in CM if LTE attach is in progress. To avoid this race condition we will
    not originate in 1x/HDR. 
     
    Note on Sys Chg SM: For 1X Sp call cleanup, Sys Chg SM will bring up the 
    phys link even when the pref pkt mode is LTE 
     
    Note on OptHo: For OptHo, we should allow phys link bring up when S101
    is in tunnel call allowed state
  -------------------------------------------------------------------------*/
#ifdef FEATURE_EPC_HANDOFF 
  if (ds3g_is_pref_pkt_mode(SYS_SYS_MODE_LTE)  
#ifdef FEATURE_EHRPD
      && DS707_SYS_CHG_SM_NULL == ds707_query_sys_chg_sm_state()
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_DATA_OPTHO
      && DS707_S101_SM_TUNNEL_CALL_ALLOWED != ds707_s101_query_sm_state()
#endif /* FEATURE_DATA_OPTHO */
     )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "DS3G system is LTE, Can't originate 1x/HDR call");

#ifdef FEATURE_DATA_OPTHO
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "And S101 not in tunnel call allowed state.");
      
#endif /* FEATURE_DATA_OPTHO */
    return -1;
  }
#endif
  
  /*------------------------------------------------------------------------- 
    If the current system is CDMA, the retry delay shall take effect 
  -------------------------------------------------------------------------*/
  if ((ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA) &&
      (ds707_so_retrydel_is_delayed() == TRUE))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
		     "Pkt data calls are delayed in CDMA mode");
    return -1;
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "not in CDMA, current mode: %d.",
                     ds707_pkt_mgr_get_current_sys_mode() );
  }
  
  pri_phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  pri_phys_link_state = PS_PHYS_LINK_GET_STATE(pri_phys_link_ptr); 
  pkt_info_ptr = (ds3g_pkt_call_info_type *)info_ptr;

  /* If phys link orig was requested on a secondary physlink, 
   * resend the request on the primary physlink. Design is such 
   * that actual orgination through CM happens only through the
   * primary physlink
   */
  if (phys_link_ptr != pri_phys_link_ptr) 
  {
    /* No need to orig on a secondary physlink if the primary 
     * physlink is already trying to come up
     */
    if ((pri_phys_link_state == PHYS_LINK_COMING_UP) ||
          (pri_phys_link_state == PHYS_LINK_RESUMING))
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                 "Primary phys link is already trying to come up. Do nothing");
      return 0;
    }
    /* If primary is physlink is not already in the process of bring up,
     * bring it up
     */
    (void) ps_phys_link_up_cmd( pri_phys_link_ptr, 
                                &ps_errno, 
                                pkt_info_ptr);
    return 0;
  }

  /* If the request is for the primary physlink, bring it up through CM
   */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_pkt_call_info_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return -1;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_PHYS_LINK_UP;
  cmd_pkt_call_info_ptr = (ds3g_pkt_call_info_type *)cmd_ptr->cmd_payload_ptr;

  /*lint -save -e507
   *           client data ptr (6 bytes?) cast to 4 bytes
   */
  /* Setting the Pkt Instance to 0, as we have only one */
  cmd_pkt_call_info_ptr->pkt_instance = 0;
  /*lint -restore */

  if (pkt_info_ptr == NULL)
  {
    /*---------------------------------------------------------------------
      In the default case, we have data to send, and we should pay
      attention to the holddown timer. Also ignore the dial string based
      decision if pkt_info_ptr was never passed
    ---------------------------------------------------------------------*/
    cmd_pkt_call_info_ptr->drs_bit         = TRUE;
    cmd_pkt_call_info_ptr->use_hdown_timer = TRUE;
    cmd_pkt_call_info_ptr->new_network  = ds707_pkt_get_current_data_sess_network();
    cmd_pkt_call_info_ptr->force_cdma_only = FALSE;
    cmd_pkt_call_info_ptr->inc_call        = FALSE;
    /* Passing Iface ptr as well */
    cmd_pkt_call_info_ptr->iface_ptr       = NULL;
    cmd_pkt_call_info_ptr->epzid           = FALSE;
  }
  else
  {
    cmd_pkt_call_info_ptr->drs_bit         = pkt_info_ptr->drs_bit;
    cmd_pkt_call_info_ptr->use_hdown_timer = pkt_info_ptr->use_hdown_timer;
    cmd_pkt_call_info_ptr->new_network     = pkt_info_ptr->new_network;
    cmd_pkt_call_info_ptr->force_cdma_only = pkt_info_ptr->force_cdma_only;
    cmd_pkt_call_info_ptr->inc_call        = pkt_info_ptr->inc_call;
    /* Passing Iface ptr as well */
    cmd_pkt_call_info_ptr->iface_ptr       = pkt_info_ptr->iface_ptr;
    cmd_pkt_call_info_ptr->epzid           = pkt_info_ptr->epzid;
  }
  /* Orignations are always done on primary physlink 
   */
  cmd_pkt_call_info_ptr->phys_link_ptr   = pri_phys_link_ptr;

  DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                   "Phys link up cmd DRS %d Use Hdown %d new IDM %d",
                   cmd_pkt_call_info_ptr->drs_bit,
                   cmd_pkt_call_info_ptr->use_hdown_timer,
                   cmd_pkt_call_info_ptr->new_network );

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                     "Force CDMA Only Flag %d",
                     cmd_pkt_call_info_ptr->force_cdma_only);
  }

  ds_put_cmd(cmd_ptr);

  return(0);
} /* ds707_pkti_phys_link_bring_up_cmd() */
/*lint -restore*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_PROCESS_CMD

DESCRIPTION   Processes the phys link commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_phys_link_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ds3g_pkt_call_info_type *pkt_call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("ds_cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  pkt_call_info_ptr = (ds3g_pkt_call_info_type *)ds_cmd_ptr->cmd_payload_ptr;

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_PHYS_LINK_UP:
      VERIFY_PKT_INSTANCE(pkt_call_info_ptr->pkt_instance);
      ds707_pkti_phys_link_bring_up_cmd_hdlr(pkt_call_info_ptr);
      break;
    
    case DS_CMD_707_PKT_PHYS_LINK_DOWN:
      VERIFY_PKT_INSTANCE(pkt_call_info_ptr->pkt_instance);
      ds707_pkti_phys_link_tear_down_cmd_hdlr(pkt_call_info_ptr);
      break;

    default:
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Unknown cmd %d", 
                       ds_cmd_ptr->hdr.cmd_id);
      DS_FORCE_ASSERT();
      break;
  }
} /* ds707_pkt_phys_link_process_cmd() */


/*===========================================================================
FUNCTION      DS707_SETUP_PHYS_LINK_F_PTRS

DESCRIPTION   This funtion with set up the primary and secondary 
              physical link pointers.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_setup_phys_link_f_ptrs 
(
    ps_phys_link_type *phys_link_ptr,
    int index
)
{
  ASSERT(phys_link_ptr != 0);

  phys_link_ptr->client_data_ptr = ds707_get_sec_tc_ptr(index);

  /*-------------------------------------------------------------------------
    Setup the phys link ptrs to map to the corresponding mode handler 
    specific functionality. for e.g. phys_link_up_cmd maps to the Mode 
    handler's bring up command.
  -------------------------------------------------------------------------*/
  phys_link_ptr->phys_link_go_null_cmd_f_ptr = 
                                          ds707_pkti_phys_link_tear_down_cmd;

  phys_link_ptr->phys_link_down_cmd_f_ptr = 
                                          ds707_pkti_phys_link_tear_down_cmd;

  phys_link_ptr->phys_link_up_cmd_f_ptr  = 
                                           ds707_pkti_phys_link_bring_up_cmd;

  phys_link_ptr->ioctl_f_ptr = ds707_pkt_phys_link_ioctl;

  /* Assign the callback function defined in DS707 Pkt Mgr which will be */
  /* used to transmit data from higher layers into RSP */
  ps_phys_link_set_tx_function(phys_link_ptr, ds707_pkt_tx_data, NULL);
}

/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              iface unctionality with PS IFACE.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_phys_link_init
(
  void
 )
{
  ds707_tc_state_type *tc_state_ptr;
#ifdef FEATURE_HDR_QOS
  int i;     /* Index counter for TCs */
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Create and initialize the phys links */
  if (ps_phys_link_create(ds707_pkt_get_pri_phys_link_ptr(), DS707_NUM_PHYS_LINKS)  < 0)
  {
    ERR_FATAL("Could not create phys links",0,0,0);
  }

  /*-------------------------------------------------------------------------
  Set up the primary phys link. Get a phys link from the free pool of
  phys links. Associate this phys link with the Reservation label 0xFF
  which indicates default phys link. Associate it with the default TC.
  -------------------------------------------------------------------------*/
  tc_state_ptr = ds707_get_pri_tc_ptr();
  tc_state_ptr->ps_phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  /*-------------------------------------------------------------------------
    Initialize the primary phys link related function pointers
  -------------------------------------------------------------------------*/
  ds707_setup_phys_link_f_ptrs(tc_state_ptr->ps_phys_link_ptr, 0);

#ifdef FEATURE_HDR_QOS
  
  /*-------------------------------------------------------------------------
  Set up the secondary phys links and associate it with the secondary TCs
  -------------------------------------------------------------------------*/
  for(i=1;i<DS707_NUM_PHYS_LINKS;i++)
  {    
    tc_state_ptr = ds707_get_sec_tc_ptr(i);
    tc_state_ptr->ps_phys_link_ptr = ds707_get_phys_link_ptr(i);     

    /*----------------------------------------------------------------------
    Initialize the secondary phys link related function pointers
    -----------------------------------------------------------------------*/
    ds707_setup_phys_link_f_ptrs(tc_state_ptr->ps_phys_link_ptr, i);
  }

#endif /* FEATURE_HDR_QOS */
} /* ds707_pkt_phys_link_init() */
#endif /* FEATURE_DATA_IS707 */

