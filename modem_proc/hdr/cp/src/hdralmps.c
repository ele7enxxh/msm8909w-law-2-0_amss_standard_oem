
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     A I R   L I N K   M A N A G E M E N T   P R O T O C O L   S T A T E

GENERAL DESCRIPTION
  This module contains code related to the state machine of ALMP.  Its
  primary function is to encapsulate the state variable and require code
  external to this module to reference the state through access functions.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdralmps_get_state() - Returns the ALMP state
  hdralmps_change_state() - Changes the ALMP state
  hdralmps_get_pending_state() - Returns the pending ALMP state
  hdralmps_change_pending_state() - Changes the pending ALMP state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdralmps.c_v   1.28   07 Feb 2003 16:45:16   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdralmps.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
01/13/15   sjj	   WLAN coexistence improvements to Type 7 connected Using RRC states
04/07/14   vko     Fixed klocwork issues
03/14/14   vko/dsp FR19338:Reduce call setup time by skipping RF Tx shutdown.
10/31/13   ukl     Keep NSET updated for system avoidance 
10/04/12   cnx     Initialize local variable num_avoided_channels.
09/14/12   cnx     Handle reselection when AT is in TUNNEL_NO_SRV state.
08/22/12   cnx     Set deactivation_cmd_is_pending before entering SUSPEND.
06/20/12   cnx     Don't skip activating OVHD when ALMP enters IDLE in native HDR.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
03/23/11   wsh     Fixed not deactivating HDRSRCH when time transfer fail
10/28/11   pba     Optimized handoff phase 2 support
07/27/11   cnx     Merged Optimized Handoff changes.
03/24/11   pba     Deactivate RUP, CMAC, OVHD before entering Suspend state.
03/22/11   cnx     Set hdralmp.deactivate.deactivation_cmd_is_pending before enter 
                   INACTIVE state to make sure MMoC is informed of deactivation.
03/02/10   pba     Added support for LTE to HDR reselection
12/04/09   sju     Addd support for back to back TCAs. 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
12/04/08   pba     Lint and Compiler warning fixes
11/10/08   smd     Removed off-target build errors. 
05/09/08   sju     Added support to remove avoided channels from NSET
04/21/08   mpa     Fixed compiler error caused by 04/01/08 check in.
04/17/08   wsh     Fixed compiler error caused by 04/01/08 check in.
04/01/08   wsh     Send N-Set update after switching from idle to traffic
09/10/07   wsh     Report session or user traffic for KDDI system reselection timer
10/02/07   wsh     Report to SD whether a traffic is for session or user data
10/16/06   etv     Added support for LOG_HDR_SUBTYPE_N_STREAM_CONFIG_C log pkt.
05/18/05   mpa     Moved sd_ss_ind_hdr_opr_stop_activity() to hdralmp.c
05/09/06   yll     Added support for Minimal UATI Mode.
11/08/05   dna     SD API change to fix 1x acq throttling during BCMCS idle
08/28/05   mpa     Control BCCP activation/deactivation.
11/29/04   grl     Added support for connected state stat logging.
08/05/04   mpa     Notify searcher if any flows are monitored upon entering 
                   idle state
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
04/14/04   dna     HICPS3.0 support, to tell SD when we are connected.
01/08/04   mpa     Only change the pending state if it is "less than" the
                   new pending state being proposed.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSIDs
05/07/02   dna     Force state change to pending state if it is "less than"
                   the new state being proposed.
09/05/01   dna     Deactivate ACMAC after RUP (and LMAC), to ensure ACMAC
                   isn't active when HDR Searcher tries to release the RF.
09/05/01   dna     Added separate handling for in-use vs. in-config messages.
08/18/01   sh      Added protocol logging support
09/07/01   dna     Deactivate LMAC when entering inactive, activate LMAC when
                   exiting inactive
05/08/01   dna     Re-naming for consistency,
                   added HDRTRACE_ALMP_CONNECTION_OPENED
02/07/01   dna     Added HDRALMP_RETURNING_TO_IDLE indication
01/09/01   kss     Added hdramac_activate().
09/07/00   dna     Activate OVHD when entering idle state instead of having
                   idle and connected protocols do it.
02/02/00   dna     Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrhai.h"
#include "hdrmci.h"
#include "hdralmp.h"
#include "hdralmpi.h"
#include "hdralmps.h"
#include "hdrinit.h"
#include "hdrovhd.h"
#include "hdramac.h"
#include "hdrcmac.h"
#include "hdrlmac.h"
#include "hdrrup.h"
#include "hdrind.h"
#include "hdrplog.h"
#include "sd.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "emm_irat_if_msg.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "hdrsrchtypes.h"
#ifdef FEATURE_HDR_BCMCS
#include "hdrbccp.h"
#endif /* FEATURE_HDR_BCMCS */

#include "hdrscmdb.h"
#include "hdrutil.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
#include "wcn_coex_mgr.h"
#endif


/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

typedef void ( hdralmps_exit_func_type ) ( hdralmps_state_enum_type );
typedef void ( hdralmps_enter_func_type ) ( hdralmps_state_enum_type );

LOCAL boolean const
  hdralmps_is_valid_trans[HDRALMPS_NUM_STATES][HDRALMPS_NUM_STATES] =
{
/* To INACTIVE  INIT   IDLE   CONNECTED  SUSPEND */
    { FALSE,    TRUE,  FALSE, FALSE,     FALSE     }, /* From HDRALMPS_INACTIVE  */
    { TRUE,     FALSE, TRUE,  FALSE,     FALSE     }, /* From HDRALMPS_INIT      */
    { TRUE,     TRUE,  FALSE, TRUE,      TRUE      }, /* From HDRALMPS_IDLE      */
    { TRUE,     TRUE,  TRUE,  FALSE,     FALSE     }, /* From HDRALMPS_CONNECTED */
    { TRUE,     TRUE,  TRUE,  FALSE,     FALSE     }  /* From HDRALMPS_SUSPEND   */
};

LOCAL void hdralmps_exit_inactive ( hdralmps_state_enum_type next_state);
LOCAL void hdralmps_exit_init ( hdralmps_state_enum_type next_state);
LOCAL void hdralmps_exit_idle ( hdralmps_state_enum_type next_state);
LOCAL void hdralmps_exit_connected ( hdralmps_state_enum_type next_state);
LOCAL void hdralmps_exit_suspend ( hdralmps_state_enum_type next_state);

LOCAL hdralmps_exit_func_type * const hdralmps_exit[HDRALMPS_NUM_STATES] =
{
  hdralmps_exit_inactive,
  hdralmps_exit_init,
  hdralmps_exit_idle,
  hdralmps_exit_connected,
  hdralmps_exit_suspend
};

LOCAL void hdralmps_enter_inactive ( hdralmps_state_enum_type prev_state);
LOCAL void hdralmps_enter_init ( hdralmps_state_enum_type prev_state);
LOCAL void hdralmps_enter_idle ( hdralmps_state_enum_type prev_state);
LOCAL void hdralmps_enter_connected ( hdralmps_state_enum_type prev_state);
LOCAL void hdralmps_enter_suspend ( hdralmps_state_enum_type next_state);

LOCAL hdralmps_enter_func_type * const hdralmps_enter[HDRALMPS_NUM_STATES] =
{
  hdralmps_enter_inactive,
  hdralmps_enter_init,
  hdralmps_enter_idle,
  hdralmps_enter_connected,
  hdralmps_enter_suspend,
};

/* All local data (except constant, statically initialized data) must be
 * contained within this structure.
 */
struct
{
  hdralmps_state_enum_type state;         /* Current state */
  hdralmps_state_enum_type pending_state; /* Pending future state */
} hdralmps;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_ENTER_INACTIVE

DESCRIPTION
  Called when ALMP enters the inactive state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_enter_inactive( hdralmps_state_enum_type prev_state )
{
#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
	/* setting RAT's layer3 state and reporting it */
	cxm_tech_l3_state_s l3_state_s;
	l3_state_s.mode = CXM_TECH_INACTIVE;
	l3_state_s.state = CXM_TECH_L3_INACTIVE;
  	cxm_set_tech_l3_state(CXM_TECH_HDR, &l3_state_s);
#endif
  
  if (( prev_state == HDRALMPS_SUSPEND ) 
#ifdef FEATURE_LTE_TO_HDR_OH
      || 
      ( hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL) && 
        !hdrmc_lte_to_hdr_ho_is_in_progress() &&
        !hdrmc_hdr_to_lte_time_transfer_is_in_progress() &&
        !hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    /* SRCH is not running so need not disable SRCH */
  }
  else
  {
#ifdef FEATURE_LTE_TO_HDR
    hdralmp_stop_get_cgi_timer();
#endif /* FEATURE_LTE_TO_HDR */
    /* Make sure we don't leave the HDR searcher running when not in HDR mode */
    hdrlmac_deactivate( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
  }

} /* hdralmps_enter_inactive() */ /*lint !e715*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_EXIT_INACTIVE

DESCRIPTION
  Called when ALMP exits the inactive state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_exit_inactive( hdralmps_state_enum_type next_state )
{
  /* next_state currently not used */

#ifdef FEATURE_LTE_TO_HDR_OH
  if (!hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL))
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /* Although HDR searcher doesn't need to do anything until told to acquire,
     * the design is to have LMAC always 'active' while HDR protocol is active.
     */
    hdrlmac_activate( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
  }

} /* hdralmps_exit_inactive() */ /*lint !e715*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_ENTER_INIT

DESCRIPTION
  Called when ALMP enters the initialization state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_enter_init( hdralmps_state_enum_type prev_state )
{
#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
  /* setting RAT's layer3 state and reporting it */
  cxm_tech_l3_state_s l3_state_s;
  l3_state_s.mode = CXM_TECH_ACTIVE;
  l3_state_s.state = CXM_TECH_L3_ACQUISITION;
  cxm_set_tech_l3_state(CXM_TECH_HDR, &l3_state_s);
#endif
  
  if (prev_state != HDRALMPS_INACTIVE)
  {
    hdrind_give_ind( HDRIND_ALMP_REACQUIRING , NULL );
  }

#ifdef FEATURE_LTE_TO_HDR
  hdralmp_start_get_cgi_timer();
#endif /* FEATURE_LTE_TO_HDR */

  hdrinit_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
} /* hdralmps_enter_init() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_EXIT_INIT

DESCRIPTION
  Called when ALMP exits the initialization state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_exit_init( hdralmps_state_enum_type next_state )
{
  /* next_state currently not used */

  hdrinit_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

} /* hdralmps_exit_init() */ /*lint !e715*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_ENTER_IDLE

DESCRIPTION
  Called when ALMP enters the idle state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_enter_idle( hdralmps_state_enum_type prev_state )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type curr_tunnel_mode = 
      hdrsap_get_current_tunnel_mode_value (HDRHAI_ROUTE_UPDATE_PROTOCOL); 
  hdrmc_lte_to_hdr_active_ho_info_type *active_handoff_msg_ptr;
#endif  /* FEATURE_LTE_TO_HDR_OH */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ENABLED ||
       curr_tunnel_mode == HDRHAI_TUNNEL_MODE_NO_SRV )
  {
    /* Move protocols to correct state to get CGI */
    if ( hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
    {
      hdrsap_cgi_ready();
    }      
    else
    {
      hdrsap_idle_handoff( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
    }
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  hdridle_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
  hdrrup_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL ) &&
       hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
  {
    /* OVHD has been activated in hdrsap_cgi_ready already. */
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {  
    hdrovhd_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
  }

  hdramac_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

#ifdef FEATURE_LTE_TO_HDR_OH
  if (curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO)
  {
    active_handoff_msg_ptr = hdrmc_get_active_handoff_payload();

    hdrsap_continue_active_handoff( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                    active_handoff_msg_ptr );
  }
#endif  /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_BCMCS
  if ( (prev_state == HDRALMPS_INACTIVE) || (prev_state == HDRALMPS_INIT) )
  {
    hdrbccp_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
  }
#endif /* FEATURE_HDR_BCMCS */

} /* hdralmps_enter_idle() */ /*lint !e715*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_EXIT_IDLE

DESCRIPTION
  Called when ALMP exits the idle state.

DEPENDENCIES
  hdridle_deactivate() must be called before changing out of idle state.

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_exit_idle( hdralmps_state_enum_type next_state )
{
  /* hdridle_deactivate() has already been called */
  hdramac_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, FALSE);

  if ( next_state == HDRALMPS_INIT ||
       next_state == HDRALMPS_INACTIVE ||
       next_state == HDRALMPS_SUSPEND )
  {
    /* When going to initialization or inactive state, we also need
     * to deactivate the RouteUpdate, OverheadMessages, and
     * ControlChannelMAC protocols
     */
    hdrrup_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
    hdrovhd_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
    hdrcmac_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
#ifdef FEATURE_HDR_BCMCS
    hdrbccp_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
#endif /* FEATURE_HDR_BCMCS */
  }

  /* Call IDLE routine for leaving Idle state cleanup */
  hdridle_almp_leaving_idle();

} /* hdralmps_exit_idle() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_ENTER_CONNECTED

DESCRIPTION
  Called when ALMP enters the connected state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_enter_connected( hdralmps_state_enum_type prev_state )
{
#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
  /* setting RAT's layer3 state and reporting it */
  cxm_tech_l3_state_s l3_state_s;
  l3_state_s.mode = CXM_TECH_ACTIVE;
  l3_state_s.state = CXM_TECH_L3_CONNECTED;
  cxm_set_tech_l3_state(CXM_TECH_HDR, &l3_state_s);
#endif
  
  /* Nset info retrieved from the most recent SP message */
  hdrovhd_sp_msg_nset_info_type nset_info;

  /* Pilot GroupID retrieved from the most recent SP message */
  uint8 serving_pilot_gid = 0;

  hdrerrno_enum_type errnum = E_SUCCESS;

  uint8 num_avoided_channels = 0;

  /* Current avoided channels returned by SD*/
  sys_avoided_channel_record_type avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];       

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Opening a connection");
  HDRTRACE_ALMP_CONNECTION_OPENED();

  /* Initialize local variable */
  memset( &nset_info, 0, sizeof(hdrovhd_sp_msg_nset_info_type) );
  memset( avoided_chan_rec, 0, sizeof(avoided_chan_rec) );

  /* Start logging connection statistics when starting a call */
  hdrplog_conn_start_event();

  /* Tell SD that we are opening a connection so it can throttle acquisition
   * attempts for 1x, if no 1x system is acquired or if 1x is lost during
   * the connection. Report if the traffic chanel is for session or user data
   */
  if (hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
  {
    /* if stream has already been configured, this is user traffic */
    (void) sd_ss_ind_hdr_opr_start_activity( NULL, SD_SS_HDR_ACT_MASK_TRAFFIC );
  }
  else
  {
    /* otherwise, it is session traffic */
    (void) sd_ss_ind_hdr_opr_start_activity( NULL, SD_SS_HDR_ACT_MASK_SESSION );
  }

  errnum = hdrovhd_get_sp_msg_pilot_gid_and_nset_info( 
      &serving_pilot_gid,
      &nset_info, 
      HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL );

  if (errnum == E_SUCCESS)
  {
    /*-------------------------------------------------------------------------
    Prune invalid neighbors from nset (including unpaired channels).
    -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
    num_avoided_channels = hdrovhd_update_sys_avoid_list(
        HDRSRCH_AVOIDED_CHAN_MAX_SIZE,
        avoided_chan_rec);
#else /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
    sd_misc_sys_avoid_list(SYS_SYS_MODE_HDR, avoided_chan_rec, 
                           &num_avoided_channels, HDRSRCH_AVOIDED_CHAN_MAX_SIZE);
#endif /* #else FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

	if(num_avoided_channels > HDRSRCH_AVOIDED_CHAN_MAX_SIZE)
	{
	    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "num_avoided_channels is %d greater than max allowed. limiting to max %d.",num_avoided_channels, HDRSRCH_AVOIDED_CHAN_MAX_SIZE);  
		num_avoided_channels = HDRSRCH_AVOIDED_CHAN_MAX_SIZE;
	}
	 
    (void) hdrutil_remove_invalid_neighbors( nset_info.nset, &nset_info.num_neighbors,
                                             avoided_chan_rec, num_avoided_channels );

    hdrlmac_nset_update( nset_info.num_neighbors, nset_info.nset, 
                         nset_info.channel_count, nset_info.channel_list, 
                         0, NULL,
                         num_avoided_channels, avoided_chan_rec,
                         HDRSRCH_NSET_UPDATE_BROADCAST, 
                         HDRHAI_ROUTE_UPDATE_PROTOCOL,
                         serving_pilot_gid );
  }

  hdrcon_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

  /* Log the personality subtype pkt to record the subtypes of the protocol 
     in effect for the current connection */
  hdrscmdb_log_pers_subtype_stream_config();

} /* hdralmps_enter_connected() */ /*lint !e715*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_EXIT_CONNECTED

DESCRIPTION
  Called when ALMP exits the connected state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_exit_connected( hdralmps_state_enum_type next_state )
{
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Closing the connection");
  HDRTRACE_ALMP_CONNECTION_CLOSED();
  if (next_state == HDRALMPS_INIT || next_state == HDRALMPS_INACTIVE)
  {
    /* When going to initialization or inactive state, we need
     * to deactivate the RouteUpdate protocol
     */
    hdrrup_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
    hdrovhd_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
#ifdef FEATURE_HDR_BCMCS
    hdrbccp_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
#endif /* FEATURE_HDR_BCMCS*/
  }
  else
  {
    /* otherwise we just need to close it */
    hdrrup_close(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
    hdrind_give_ind( HDRIND_ALMP_RETURNING_TO_IDLE, NULL );
    hdrovhd_return_to_idle(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
  }
  hdralmp_send_psm_ready_ind_to_cm();
  hdrcmac_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

  hdrlmac_clear_pending_tca_queue();

} /* hdralmps_exit_connected() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in ALMP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmps_change_state
(
  hdralmps_state_enum_type new_state
)
{
  hdralmps_state_enum_type prev_state; /* Previous state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_STATE( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
    hdralmps.state, new_state);

  if (hdralmps_is_valid_trans[ hdralmps.state ][ new_state ])
  {
    if (hdralmps.pending_state != hdralmps.state)
    {
      /* If the pending state is "less than" the suggested new state,
       * transition to the pending state instead.  For example, if
       * we have "init" pending, and someone is trying to change to
       * "idle", go to "init" instead
       */
      if (hdralmps.pending_state < new_state)
      {
        new_state = hdralmps.pending_state;
        if (new_state == HDRALMPS_INACTIVE)
        {
          hdralmp_set_deactivate_notifier();
        }
      }
    }

    /* Do exit processing from current state */
    (*hdralmps_exit[ hdralmps.state ])( new_state );

    /* Change the state so any processing done upon entry into the new
     * state will have hdralmps.state set properly
     */
    prev_state = hdralmps.state;
    hdralmps.state = new_state;
    hdralmps.pending_state = new_state;

    /* Do entry processing for the new state */
    (*hdralmps_enter[ new_state ])( prev_state );

    HDRTRACE_STATE_ENTRY( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, new_state);
    hdrplog_log_state_info();
  }
  else
  {
    ERR_FATAL( "Bad transition %d to %d", hdralmps.state, new_state, 0 );
  }

} /* hdralmps_change_state() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the ALMP protocol.  This funciton
  is only to be used by other code in the ALMP protocol.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The current state of ALMP

SIDE EFFECTS
  None

===========================================================================*/

hdralmps_state_enum_type hdralmps_get_state( void )
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "hdralmps_get_state %d",
                  hdralmps.state);

  return ( hdralmps.state );
} /* hdralmps_get_state() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_GET_PENDING_STATE

DESCRIPTION
  Called to retrieve the pending state of the ALMP protocol.
  This funciton is only to be used by other code in the ALMP protocol.
  The pending state is the same as the current state unless it was
  explicitly set differently since the last state transition.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The pending state of ALMP

SIDE EFFECTS
  None

===========================================================================*/

hdralmps_state_enum_type hdralmps_get_pending_state( void )
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "hdralmps_get_pending_state %d",
                  hdralmps.pending_state);

  return ( hdralmps.pending_state );
} /* hdralmps_get_pending_state() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_CHANGE_PENDING_STATE

DESCRIPTION
  Called to set the pending state of the ALMP protocol.
  This funciton is only to be used by other code in the ALMP protocol.
  The pending state is a way for ALMP to specify a future state but delay
  the transition until some other action occurs.
  The pending state is the same as the current state unless it was
  explicitly set differently since the last state transition.

DEPENDENCIES
  None

PARAMETERS
  pending_state - The state that ALMP is to transition to at some future
  time.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdralmps_change_pending_state( hdralmps_state_enum_type pending_state )
{
  HDR_MSG_PROT_3 (MSG_LEGACY_MED,  "hdralmps_chg_pending_st (%d, %d)(cs %d)",
    hdralmps.pending_state, pending_state, hdralmps.state );

  if ((pending_state <= hdralmps.state) &&
     (hdralmps_is_valid_trans[ hdralmps.state ][ pending_state ]))
  {

    /* Only accept the new pending state if it "less than" the current pending
       state.  For example, if MMOC has instructed HDR to go into power save
       and ALMP is waiting to transition to INACTIVE state, then receiving say
       a new ConnectionClose command shouldn't force ALMP back to IDLE.
       ALMP should still deactivate when it is finished closing the connection.
    */
    if (hdralmps.pending_state >= pending_state)
    {
      hdralmps.pending_state = pending_state;
    }
    else
    {
      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "hdralmps_keep_pending_st %d (cs %d)",
        hdralmps.pending_state,hdralmps.state);
    }
  }
  else
  {
    ERR_FATAL( "Bad transition %d to %d", hdralmps.state, pending_state, 0 );
  }
} /* hdralmps_get_pending_state() */

/*===========================================================================

FUNCTION HDRALMPS_ENTER_SUSPEND

DESCRIPTION
  Called when ALMP enters the suspend state.

DEPENDENCIES
  None

PARAMETERS
  prev_state - The state transitioned from

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_enter_suspend( hdralmps_state_enum_type prev_state )
{
  /* SRCH goes to INACTIVE when HDR CP enters SUSPEND.
   * Set deactivation_cmd_is_pending so that we can handle deactivation
   * command when still waiting for SRCH to finish deactivation. */
  hdralmp_set_deactivate_notifier();
  hdrlmac_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

} /* hdralmps_enter_suspend() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRALMPS_EXIT_SUSPEND

DESCRIPTION
  Called when ALMP exits the suspend state.

DEPENDENCIES
  None

PARAMETERS
  next_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmps_exit_suspend( hdralmps_state_enum_type next_state )
{

  if (next_state == HDRALMPS_INIT)
  {
    /* Although HDR searcher doesn't need to do anything until told to acquire,
     * the design is to have LMAC always 'active' while HDR protocol is active.
     */
    hdrlmac_activate( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
  }

} /* hdralmps_exit_suspend() */ /*lint !e715*/

