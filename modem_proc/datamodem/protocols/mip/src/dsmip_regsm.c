/*===========================================================================

                          D S M I P _ R E G S M . C

DESCRIPTION

 The Data Services Mobile IP Agent Solicitation State Machine source file.

EXTERNALIZED FUNCTIONS
   mip_reg_sm_init()
     Initialize the Registration State Machine.
   mip_reg_post_event()
     Post an event to a Registration State Machine.
   mip_reg_sm_post_event_all()
     Pose an event to ALL of the Registrations State Machines. 
     Initialize the Meta State Machine.
   mip_reg_sm_session_active
     Check if MIP client currently has a valid registration 

Copyright (c) 2000-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_regsm.c_v   1.39   24 Jan 2003 19:42:24   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_regsm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/15/15    skc    Fixed immdeiate retry event logic for DeRRQ.
12/05/14    sd     Fixed immdeiate retry event logic for RRQ and DeRRQ.
07/02/14    ms     Allow MIP if HDR is in service even for P_rev < 6.
10/22/12    sd     Posting PS command for MIP registration when PS task is not running
07/20/12    ssb    Cancel life timer before transitioning to  
                   INIT RRQ State from REGISTERED State.
02/06/12    sn     Fix to increment MIP stats RRQ count for MIP Re-registration 
                   scenarios.
05/20/11    op     Migrated to MSG 2.0 macros
05/04/11    ttv    Added changes to compute mip statistics.
03/23/11    op     Data SU API cleanup
03/15/11    mg     Global variable cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
10/07/10    ttv    Added messages which explain the reason for asserting.
04/23/10    gc     Lint fixes
12/17/09    ms     Compiler Warning fix.
09/04/09    ss     CMI SU level modifications.
09/19/08    mga    Made changes to transition to RSMI_INIT_RRQ_STATE from 
                   RSMI_REGISTERED_STATE for RSM_MOVE_EV case.
08/07/08    as     Lint error fixes.
05/23/07    vp     MIP CCoA related changes.
05/04/07    as     Converted macro mip_data_transfer_action to function
12/07/06    as     Updated the REGSM to handle mip deregistration rrq / rrps.
07/06/06    kvd    Change ASSERTs on RRQ & Life timer expiry in NO_SESSION 
                   state to MSG_ERR.
08/17/05    ks     Calling ps_phys_link_ioctl() instead of ps_iface_ioctl for
                   GO_DORMANT.
08/15/05    mct    Updated for ps_iface_ioctl changes.
07/22/05    kvd    Set mip_fail_reason in SOL & RRQ timeouts.
06/21/05    kvd    Dereg UDP filters on transition to NO_SESSION_STATE rather
                   than inside the transition, as there is no need to dereg
                   on transition from REGISTERED_STATE.
03/17/05    jd     Remove timer drift adjustment since timers no longer drift
03/01/05    jd     When deferring re-reg due to no traffic, stay in 
                   registered state.
10/18/04    vp     Removed the network order conversion of ip addr before call
                   to ps_iface_set_addr().
06/15/04    mvl    Changed the type of reg_timer_left in
                   mip_reg_sm_set_rtt_estimation() to an int64 (from int32)
                   to support the new timer interface. 
06/11/04    vp     Changes due to changes in PS data structures.
05/06/04    kvd    Before sending RRQ check for p_rev_index  EXIT MIP if
                   doesn't support MIP.
04/28/04    jd     Set Um IP address on transition to registered before 
                   notifying dssnet (needed since dssnet is in DS task now)
04/16/04    jd     Use ps_iface_ioctl directly instead of dss_iface_ioctl.
03/20/04    ak     Compiler fixes for IPV6/June Dev merge (has new dssnets).
02/06/04    jd     Adjusted 6500 MIP timer scale to reflect characterisation
02/02/04    jd     Removed references to app_id, flow control.  
                   Pass iface_id from session info in meta_sm_post_event.
                   Remove MIP perf dependency on RMSM.  
                   Use iface ID from session info (Removes dependency on PPP)
                   Use ioctl instead of deprecated dss_force_dormancy()
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
11/26/03    jd     Updated 6500 timer scale factor reflecting characterisation
11/25/03    jd     Start snooping RRP's when immediate_retry_all occurs
11/21/03    jd     Added adjustment for clock drift when starting MIP timers
11/13/03    jd     Clear RRQ ID at end of call
11/12/03    igt    Add a Start for the MIP Re-reg Delay upon RSM_IMMED_RETRY_EV
                   in case has not been started yet and we are in RE-Reg state.
                   Also other changes to fix various delay calculations for MIP.
10/28/03    sv     Replaced dss_get_iface_id call with mipio_get_iface_id call.
10/09/03    jd     Start RRP snooping on entry to init-reg and re-reg states,
                   Stop RRP snooping on entry to reg'd and null states.
09/23/03    jd     In mip_reg_sm_all_sessions_regd(), check if SSM is active
                   to close small window where we resolicit during reg.
09/23/03    jd     Set maximum ASM retries to initial value (1) on closure, 
                   and set to handoff value (19) when registered.
09/23/03    jd     Mainlined Irfan's changes (!FEATURE_DATA_CURRENT_PPP_CB)
09/22/03    jd     Fixed bug where RRQ timeout was divided by 4 instead of 2
                   when sending RRQ upon WRITE_EV after having blocked on 
                   previous attempt.
08/28/03    ss     Updated to use modified ioctl() call
08/18/03    jd     Featurized Irfan's changes (FEATURE_DATA_CURRENT_PPP_CB)
08/13/03    ifk    rx_iface_ptr moved to IPCP fsm in ps_iface_set_addr().
06/13/03    ss     Featurized call to ds707_rmsm_is_packet_call().
06/11/03    jd     Add RRQ->RRP RTT estimate to registration re-tx timer
06/10/03    ss     Added code to change the PPP mode to PSEUDO_NET upon
                   transitioning to registered state in a laptop call.
05/05/03    jd     Added mip_reg_sm_session_active() external API to tell if 
                   MIP client is registered
04/11/03    ak     Updated to use new RMSM interfaces.
01/24/03    usb    Updated call to dss_iface_ioctl to use new prototype
12/10/02    jd     Remain in re-registration state while awaiting AAM to keep 
                   Um in internal mode so AAM can be received.  Also, do not
                   increase re-RRQ counter and interval during this time.
11/25/02    usb    Wrong arg type used (dss_iface_ioctl_phys_link_state_type
                   instead of dss_iface_ioctl_state_type in IOCTL_GET_STATE.
11/19/02    jd     For MM, check for dormancy via dss_iface_ioctl's
11/14/02    jd     For MM, call rmsm_is_packet_call instead of checking RMSM
                   state directly, dssnet api changes, .
10/23/02    jay    Fixed a Message
10/09/02    jay    Renamed #define
07/10/02    jd     Cleaned up #include's
06/07/02    jd     Mod. MSG to report actual re-registration timer value
05/07/02    jd     Moved session info block to dsmip.c
03/21/02   jd/sjy  Moved much of processing done when entering registered
                   state to after the state variable is set, in order for
                   mip_reg_sm_all_sessions_regd to return the correct state 
03/21/02   jd/sjy  Added mip_reg_sm_all_sessions_regd() to check on status 
                   of re-registrations for all sessions
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
12/11/01    ss     Modified rsmi_transition_state() to cancel reg timer if
                   no data is transferred since previous registration.
11/26/01    vr     Removed flag mip_is_rereg_in_progress. Do not flow control
                   during MIP re-registration.
11/21/01    aku    dssnet_sm_post_event now takes an additional acb_ptr arg
11/21/01    ss     Added code to support sending RRQ only if there has been 
                   some data transfer since last registration.
11/14/01    ss     Added a random offset to the base rrq interval for 
                   NEED_NEW_FA_INFO event to fix the problem caused by
                   stale challenges.
11/05/01    vr     Add a flag mip_is_rereg_in_progress for ppp to determine if
                   re-registration is going on
10/17/01    aku    The force_dormancy variable needs to set to FALSE at the 
                   beginning of a call
10/16/01    jd     Initialize force_domancy flag to avoid forcing dormancy
                   after initial registration.
09/17/01    js     Added header comment block for externalized functions. 
09/07/01    mvl    Modified the valule for Unset HA to be 255.255.255.255
                   rather than 0 - this synchs up with new PST (2.7.26+).
09/06/01    jd     Fixed bug in dsmip_regsm_init where we scribbled on the
                   rxc_dec_free_q when writing app_id into session info. 
08/23/01    mvl    Modified the setting of the Re-RRQ timer so that it cannot
                   be set to less than 50% of the registration lifetime.
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/14/01    aku    Added support for forcing call dormant after re-reg if 
                   call was dormant prior to re-registration.
08/08/01    aku    Added check for 0.0.0.0 home agent as fallback to SIP was
                   taking very long
08/07/01    jd     Allow 0.0.0.0 home agent
08/05/01    jd     Added RRQ holdoff timer support
08/01/01    mvl    Added fix to insure infinite registrations are supported.
                   Added fix to reset retry count and initial timeout when
                   doing re-registrations.
07/27/01    aku    Post event to DSSNET State machine on successful M. IP
                   registration
06/28/01    mvl    Checking if the Pre RRQ is >= lifetime (bug fix)
06/26/01    mvl    Now if we get a solicitation failure in no session state
                   we send Init-RRQ failure rather than Re-RRQ failure
06/12/01    mvl    Updated to reflect latest design
11/29/00    mvl    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "memory.h"
#include "amssassert.h"
#include "pstimer.h"

#include "ps_iface.h"
#include "ps_ip_addr.h"
#include "ds707_iface4_hdlr.h"
#include "ps_phys_link_ioctl.h"
#include "dssocket_defs.h"
#include "ds707_pkt_mgr.h"
#include "ds707_pkt_mgri.h"

#include "dsmip_v.h"
#include "dsmip_regsm.h"
#include "dsmip_solsm.h"
#include "dsmip_metasm.h"
#include "dsmip_io.h"
#include "dsmip_outmsg.h"
#include "dsmip_cfg.h"
#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */
#include "ps.h"
#include "ps_mip_task.h"

#ifdef FEATURE_QCHAT
#include "data1x_qchatupk.h"
#include "data1x_qchatupk_opts.h"
#endif /* FEATURE_QCHAT */
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
#define RSMI_INIT_RRQ_TIMEOUT       750   /* millisecs -                   */
#define RSMI_MAX_RETRY_BACKOFF_TIME 16000 /* millisecs -                   */
#define RSMI_MAX_DERRQ_TIMEOUT      6000  /* millisecs -                   */

#ifdef ASSERT
static boolean rsmi_inited = FALSE;
#endif

/*---------------------------------------------------------------------------
  Indicates if re-registration is required, as determined by data transfer 
  happening since the last registration and the corresponding user 
  preference retrieved from NV indicating if this optimization is enabled or
  not.
---------------------------------------------------------------------------*/
static boolean mip_data_sent_since_reg = TRUE;
static boolean mip_rereg_pending       = FALSE;

/*---------------------------------------------------------------------------
  These flags are used to indicate if immediate retry for RRQ and DeRRQ is 
  really needed or has been processed already.
---------------------------------------------------------------------------*/
static boolean immed_rrq_retry_reqd    = TRUE;
static boolean immed_derrq_retry_reqd  = TRUE;
static boolean io_failure = FALSE;

#ifndef T_ARM
/*---------------------------------------------------------------------------
  These need to be kept in sync with the enums which they describe!
---------------------------------------------------------------------------*/
static char *rsmi_state_names[RSMI_MAX_STATE] =
{
  "No Session",
  "Initial RRQ",
  "Registered",
  "Re-RRQ"
};

static char *rsmi_event_names[RSM_MAX_EV] =
{
  "Exit M.IP",
  "Move",
  "Sol Failed",
  "Immediate Retry",
  "Need new FA info",
  "RRQ Successful",
  "RRQ Failed with Retry",
  "RRQ Failed no Retry",
  "RRQ Failed HA Unavailable",
  "RRQ Timer",
  "Lifetime Timer",
};
#endif /* ! T_ARM */

#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type * rex_ps_tcb;
#endif

/*===========================================================================
                            Forward Declarations
===========================================================================*/
void rsmi_rrq_timer_cb
(
  void *param
);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
void rsmi_derrq_timer_cb
(
  void *param
);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

void rsmi_life_timer_cb
(
  void *param
);

void rsmi_rrq_holdoff_timer_cb
(
  void *param
);

void rsmi_transition_state
(
  mip_session_info_type *mip_info,
  rsmi_state_type new_state  
);

void rsmi_set_registered_state_timers
(
  mip_session_info_type *session
);



/*lint -save -e641 Reason for suppressing error 641*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION MIP_DATA_TRANSFER_ACTION

DESCRIPTION
  Function to be called whenever data transfer takes place on Um. If data is 
  being transferred for the first time since previous registration, we 
  want to register immediately (only if we are in re-rrq state).

PARAMETERS
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_data_transfer_action (void)
{
  if (mip_data_sent_since_reg == FALSE) 
  {                                     
    mip_data_sent_since_reg = TRUE;     
    if (mip_rereg_pending) 
    {                                      
      if(
#ifdef FEATURE_MODEM_RCINIT_PHASE2
	 (rex_self() == rex_ps_tcb)
#else
	 (rex_self() == &ps_tcb)
#endif
      )
      {
        /* In PS task */
        mip_reg_sm_immed_registration_all(); 
      }  
      else
      {
        ps_send_cmd(PS_MIP_SEND_IMMED_REG_CMD,NULL);
      }
    }  
  }
}

/*===========================================================================
FUNCTION PS_MIP_SEND_IMMED_REG_CMD_HANDLER()

DESCRIPTION
  This function processes PS_MIP_SEND_IMMED_REG_CMD.

PARAMETERS
  ps_cmd          : Must be PS_MIP_SEND_IMMED_REG_CMD.
  ps_cmd_data_ptr : cmd data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mip_send_immed_reg_cmd_handler
(
  ps_cmd_enum_type    ps_cmd,
  void              * ps_cmd_data_ptr
)
{
  
  if(ps_cmd == PS_MIP_SEND_IMMED_REG_CMD)
  { 
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Posting command to PS task for immediate MIP registration!\n"); 
    mip_reg_sm_immed_registration_all();
  }
}

/*===========================================================================
FUNCTION MIP_REG_SM_INIT()

DESCRIPTION
  This function initializes the agent solicitation state machine.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_reg_sm_init
(
  void
)
{
  int loop;
  mip_session_info_type* session;
  mip_session_info_type* mip_ses_info_array;

  (void) ps_set_cmd_handler(MIP_SEND_IMMED_REG_CMD,ps_mip_send_immed_reg_cmd_handler);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------ 
    Get MIP session Information Array.
  ------------------------------------------------------------------------*/
  mip_ses_info_array = mip_get_ses_info_array();

  /*-------------------------------------------------------------------------
    Zero out the entire mip_ses_info_array[]
  -------------------------------------------------------------------------*/
  memset(mip_ses_info_array,
         0,
         MIP_MAX_SESSIONS * sizeof(mip_session_info_type));

  /*-------------------------------------------------------------------------
    Loop over all of the control blocks that need the rsm_info rsmi_inited -
    note that the pointer to the control block is being incremented in the
    for statement.
  -------------------------------------------------------------------------*/
  session = mip_ses_info_array;
  for (loop = 0; loop < MIP_MAX_SESSIONS; loop++, session++)
  {
    /*-----------------------------------------------------------------------
      Get PS timers to support retries and lifetime expiration
    -----------------------------------------------------------------------*/
    session->rsm_info.reg_timer = ps_timer_alloc(rsmi_rrq_timer_cb, session);
    if(session->rsm_info.reg_timer == PS_TIMER_FAILURE)
    {
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Failed to allocate reg timer for RSM!\n");
      return -1;
    }
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "reg timer handle is %d\n", session->rsm_info.reg_timer);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    session->rsm_info.dereg_timer = ps_timer_alloc(rsmi_derrq_timer_cb, session);
    if(session->rsm_info.dereg_timer == PS_TIMER_FAILURE)
    {
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Failed to allocate dereg timer for RSM!\n");
      return -1;
    }
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "dereg timer handle is %d\n", session->rsm_info.dereg_timer);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    session->rsm_info.life_timer = ps_timer_alloc(rsmi_life_timer_cb,
                                                  session);
    if(session->rsm_info.life_timer == PS_TIMER_FAILURE)
    {
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Failed to allocate reg timer for RSM!\n");
      return -1;
    }
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "lifetime timer handle is %d\n",
            session->rsm_info.life_timer);

    session->rsm_info.rrq_holdoff_timer = 
      ps_timer_alloc (rsmi_rrq_holdoff_timer_cb, session);
    if(session->rsm_info.rrq_holdoff_timer == PS_TIMER_FAILURE)
    {
      DATA_MIP_MSG1(MSG_LEGACY_HIGH, "Failed to allocate RRQ holdoff timer for RSM%d", loop);
      return -1;
    }
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "lifetime timer handle is %d",
            session->rsm_info.rrq_holdoff_timer);

    /*-----------------------------------------------------------------------
      Set the retry count, holdoff and registration states to their 
      initial values
    -----------------------------------------------------------------------*/
    session->rsm_info.rrq_cnt = 0;
    session->rsm_info.one_second_since_last_rrq = TRUE;
    session->rsm_info.state = RSMI_NO_SESSION_STATE;
    session->retry_interval_offset = 0;
    session->rsm_info.rtt_estimate = 0;

    /*-----------------------------------------------------------------------
      Set initial value of force_dormancy to false
    -----------------------------------------------------------------------*/
    session->rsm_info.force_dormancy = FALSE;
    session->iface_ptr = NULL;
  } /* for(all MIP control blocks) */

#ifdef ASSERT
  rsmi_inited = TRUE;
#endif
  return 0;

} /* mip_reg_sm_init() */



/*===========================================================================
FUNCTION MIP_REG_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the registration state machine.

PARAMETERS
  mip_info: pointer to the M.IP info block array
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  mip_reg_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_post_event
(
  mip_session_info_type *session,
  mip_reg_sm_event_type event
)
{
  ps_timer_error_type    timer_result;
  mip_session_info_type *mip_ses_info_array;
  qword                  ts_null;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------ 
    Get MIP session Information Array.
  ------------------------------------------------------------------------*/
  mip_ses_info_array = mip_get_ses_info_array();

  /*------------------------------------------------------------------------ 
    Initialize ts_null
  ------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

#ifdef ASSERT
  ASSERT(rsmi_inited == TRUE);
#endif /* ASSERT */

#ifndef T_ARM
  if (!(event == RSM_MIN_EV || event == RSM_MAX_EV))
  {
    DATA_MIP_MSG2(MSG_LEGACY_MED, "'%s' event posted to Reg SM %d.\n",
          rsmi_event_names[event],
          abs(session - mip_ses_info_array)
          );
  }
#else
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Event %d posted to Reg SM %d.\n",
          event,
          abs(session - mip_ses_info_array));
#endif

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently in
  -------------------------------------------------------------------------*/
#ifndef T_ARM
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Got %d event in %s state",
                event, rsmi_state_names[session->rsm_info.state]);
#else
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Got %d event in %d state",
                event, session->rsm_info.state);
#endif

  switch(event)
  {
    /*-----------------------------------------------------------------------
                               RSM_EXIT_MIP_EV

      Exit M.IP: Return to the No Session state
    -----------------------------------------------------------------------*/
  case RSM_EXIT_MIP_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_NO_SESSION_STATE:
      /* ignore */
      
      break;

    case RSMI_RE_RRQ_STATE:
    case RSMI_INIT_RRQ_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      /* fall through */
    case RSMI_REGISTERED_STATE:
      /* fall through */
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);
	  ASSERT(0);

    } /* switch(rsm state) */
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                               RSMI_DEREG_MIP_EV

      DSSNET asked to deregister the MIP state machine.
        No Session/De RRQ/Deregistered/Init RRQ: ignore
        Re RRQ/Registered: proceed to De RRQ state
    -----------------------------------------------------------------------*/
  case RSMI_DEREG_MIP_EV:
    switch(session->rsm_info.state)
    {
    
    case RSMI_RE_RRQ_STATE:
      /* cancel the re-registration time */
      timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      /* fall through intentional */

    case RSMI_REGISTERED_STATE:
      session->rsm_info.rrq_cnt = 0;
      /*------------------------------------------------------------------- 
        Install IP filters 
      -------------------------------------------------------------------*/
      if (!mipio_start_udp_snooping(MIP_SES_PTR_2_INDEX(session)))
      {
        /*----------------------------------------------------------------- 
          FUTURE: mip_meta_sm_post_event(MSM_RE_RRQ_FAILED_EV);
                  and make sure to start timer if call fails
        -----------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg UDP filters for MIP failed");
      }
      rsmi_transition_state(session, RSMI_DE_RRQ_STATE);
      break;

    case RSMI_NO_SESSION_STATE:
    case RSMI_DE_RRQ_STATE:
    case RSMI_DEREGISTERED_STATE:
    case RSMI_INIT_RRQ_STATE:
      /* ignore event and flag an error */
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Got unexpected Deregister M.IP Event!");
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);	  	
      ASSERT(0);

    } /* switch(rsm state) */
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                           RSM_NEED_NEW_FA_INFO_EV

      Our current FA info is invalid, need to get new info.
        No Session/Registered state: ignore
        Re RRQ: return to registered
        Init RRQ: return to no session
    -----------------------------------------------------------------------*/
  case RSM_NEED_NEW_FA_INFO_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_RE_RRQ_STATE:
      /*---------------------------------------------------------------------
        mip_sol_sm_get_random_byte() returns a random number between 0..255. 
        We multiply the return value by 20 to get a random offset in the
        range of 0..~5000 ms. Note that we assign a random offset to 
        retry_interval_offset in RSMI_INIT_RRQ_STATE also.
      ---------------------------------------------------------------------*/
      session->retry_interval_offset = mip_sol_sm_get_random_byte() * 20;
      /* Remain in re-reg state while awaiting AAM */
      break;

    case RSMI_INIT_RRQ_STATE:
      session->retry_interval_offset = mip_sol_sm_get_random_byte() * 20;
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignore */
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);		
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                               E5 - RSM_RRQ_SUCC_EV

      The Registration Request Succeeded:
        Initial RRQ/Re-RRQ: Go to to the Registered state
        De-RRQ: Go to the Deregistered state
        No Session/Registered/Deregistered: ignore 
    -----------------------------------------------------------------------*/
  case RSM_RRQ_SUCC_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_RE_RRQ_STATE:
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rtt_estimate = 0;
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Set RRP latency estimate to %d", 
               session->rsm_info.rtt_estimate);
      rsmi_transition_state(session, RSMI_REGISTERED_STATE);
      break;

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignore */
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      DATA_MIP_MSG0(MSG_LEGACY_MED, "MIP deregistered successfully");
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);    
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                          E7 - RSM_RRQ_FAIL_NO_RETRY_EV

      Registration Request failed no retry is to happen:
        Initial RRQ: Post the Init-RRQ failed event to the meta state machine
                     and go to the No session state.
        Re-RRQ: Post the Re-RRQ failed event to the meta state machine and
                go to the No Session state.
        De-RRQ: go to the Deregistered state.
        No Session/Registered/Deregistered: ignore 
    -----------------------------------------------------------------------*/
  case RSM_RRQ_FAIL_NO_RETRY_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      /*---------------------------------------------------------------------
          The state transition is happening before the event is posted to
          avoid entering the No Session state twice (and doing all that
          work).  (This is an optimization because we know how the
          interaction with the meta state machine works.)
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      mip_meta_sm_post_event(MSM_INIT_RRQ_FAILED_EV, session->iface_ptr);
      break;

    case RSMI_RE_RRQ_STATE:
      goto Re_RRQ_2_No_Session;

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignore */
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      DATA_MIP_MSG0(MSG_LEGACY_MED, "MIP deregistration timed out.");
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */


    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);    
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                           E6 - RSM_RRQ_FAIL_W_RETRY_EV

      Registration Request failed a retry should happen:  ensure that this
        Initial RRQ: reset count to 0 - fall through
        Re-RRQ/De-RRQ: reset retry timeout, return to current state
        Registered/Deregistered/No Session: ignore
    -----------------------------------------------------------------------*/
  case RSM_RRQ_FAIL_W_RETRY_EV:
    switch(session->rsm_info.state)
    {
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rrq_cnt = 0;
      /* fall through intentional */

    case RSMI_RE_RRQ_STATE:
      session->rsm_info.re_rrq_timeout =  session->base_retry_interval;
      rsmi_transition_state(session, session->rsm_info.state);
      break;
      
    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
      /* ignoree */
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);		
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                                 E1 - RSM_MOVE_EV

      Move event
        Initial RRQ: reset count to 0 - fall through
        Re-RRQ: reset retry timeout, return to current state
        Registered State: reset the retry count to 0, reset the timeout
        value, cancel the lifetime timer (we have moved) and go to 
        INIT RRQ state.
        De-RRQ: stop udp snooping and move to Deregistered state
        Deregistered state: move to No Session state
        No Session State: go to RRQ in Progress state.
    -----------------------------------------------------------------------*/
  case RSM_MOVE_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rrq_cnt = 0;
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      rsmi_transition_state(session, session->rsm_info.state);
      break;
      
    case RSMI_REGISTERED_STATE:
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      /*------------------------------------------------------------------- 
        Install IP filters 
      -------------------------------------------------------------------*/
      if (!mipio_start_udp_snooping(MIP_SES_PTR_2_INDEX(session)))
      {
         /*----------------------------------------------------------------- 
           FUTURE: mip_meta_sm_post_event(MSM_RE_RRQ_FAILED_EV);
                  and make sure to start timer if call fails
         -----------------------------------------------------------------*/
         DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg UDP filters for MIP failed");
      }
      /*------------------------------------------------------------------- 
       Since we moved to different subnet, 
       the previous registration is no longer valid. 
       Hence Cancelling the life timer associated with it. 
      -------------------------------------------------------------------*/
      timer_result = ps_timer_cancel(session->rsm_info.life_timer);
      if(timer_result == PS_TIMER_FAILURE)
      {
         DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Life Timer Cancellation Failed! ");
      }
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;

    case RSMI_NO_SESSION_STATE:
      /*---------------------------------------------------------------------        
        Install IP filters 
      ---------------------------------------------------------------------*/
      if (!mipio_start_udp_snooping(MIP_SES_PTR_2_INDEX(session)))
      {
        /*-------------------------------------------------------------------        
          FUTURE: mip_meta_sm_post_event(MSM_RE_RRQ_FAILED_EV);
                  and make sure to start timer if call fails
        -------------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg UDP filters for MIP failed");
      }
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      DATA_MIP_MSG0(MSG_LEGACY_MED, "MIP deregistration timeout.");
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Ignore RSM_MOVE in DEREGISTERED state");
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
	  	            session->rsm_info.state);    
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                             RSM_IMMED_RETRY_EV

      We can send a RRQ immediately - DSM buffers freed up for writes
        Initial RRQ: decrement retry count (make sure this is valid) and fall
                     through 
        Re-RRQ: reduce the rrq timeout (will be re-increased on state 
                transition), stop RRQ timer and return to current state
        De-RRQ: reduce the rrq timeout (will be re-increased on state 
                transition), stop RRQ timer and return to current state
        No Session/Registered/Deregistered: ignore

      Immediate retry is not considered in the count for RRQ retries and hence
      rrq_cnt is decremented by 1 in that case, and we try to send out RRQ 
      immediately. There is a scenario where while trying to send out RRQ, we 
      try to bring up the traffic channel but CM sends call end and hence it
      fails. So, in that case, we increment the rrq_cnt while sending RRQ and
      decrement it while doing immediate retry. So, rrq_cnt is not reaching the
      maximum limit and we are exhausting resources while executing this in 
      loop leading to crash. To fix this, we have added logic so that only one
      immediate retry happens for each RRQ. The same thing applies for DeRRQ
      as well.
    -----------------------------------------------------------------------*/ 
  case RSM_IMMED_RETRY_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      if(session->rsm_info.rrq_cnt == 0)	
      {
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, 
                      "RRQ cnt is equal to zero %d",
                      session->rsm_info.rrq_cnt);
        ASSERT(0);
      }
      
      if(immed_rrq_retry_reqd == TRUE)
      {
        session->rsm_info.rrq_cnt -= 1;
        immed_rrq_retry_reqd = FALSE;
      }
      else
      {
        DATA_MIP_MSG0(MSG_LEGACY_MED, 
                      "Wait for RRQ timer expiry to send RRQ.");
        break;
      }
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      session->rsm_info.re_rrq_timeout /= MIP_RRQ_RETRY_BACKOFF_MULT;
      timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      rsmi_transition_state(session, session->rsm_info.state);

#ifdef FEATURE_DS_MOBILE_IP_PERF
      /*---------------------------------------------------------------------
        This is the immediate retry event while we are in the RE_RRQ state.
        In this case we want to start the re-reg Delay.
      ---------------------------------------------------------------------*/
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);
      if( (qw_cmp(mip_perf_info_ptr->ts_start, ts_null)==0)
          && ( session->rsm_info.state == RSMI_RE_RRQ_STATE) )
      {    
        /*-------------------------------------------------------------------
          Notify M.IP call performance that M.IP re-registration is starting
        -------------------------------------------------------------------*/
        mip_perf_delay(mip_perf_info_ptr,
                       PERF_MIP_REREG_DELAY,
                       PERF_START_TS);
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      if(session->rsm_info.rrq_cnt == 0)
      {
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, 
                      "RRQ cnt is equal to zero %d",
                      session->rsm_info.rrq_cnt);
        ASSERT(0);
      }
      
      if(immed_derrq_retry_reqd == TRUE)
      {
        session->rsm_info.rrq_cnt -= 1;
        immed_derrq_retry_reqd = FALSE;
      }
      /* First attempt for immediate retry. If this again fails with IO
         failure, register for the write event and wait till traffic channel
         is brought up and have an extended timer running */
      else if(io_failure == TRUE)
      {
        session->rsm_info.rrq_cnt -= 1;
      }
      else
      {
        DATA_MIP_MSG0(MSG_LEGACY_MED, 
                      "Wait for De-RRQ timer expiry to send De-RRQ.");
        break;
      }

      session->rsm_info.re_rrq_timeout /= MIP_RRQ_RETRY_BACKOFF_MULT;
      timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      rsmi_transition_state(session, RSMI_DE_RRQ_STATE);

#ifdef FEATURE_DS_MOBILE_IP_PERF
      /*---------------------------------------------------------------------
        This is the immediate retry event while we are in the RE_RRQ state.
        In this case we want to start the re-reg Delay.
      ---------------------------------------------------------------------*/
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_DEREG_DELAY);
      if( (qw_cmp(mip_perf_info_ptr->ts_start, ts_null)==0)
          && ( session->rsm_info.state == RSMI_DE_RRQ_STATE) )
      {    
        /*-------------------------------------------------------------------
          Notify M.IP call performance that M.IP de-registration is starting
        -------------------------------------------------------------------*/
        mip_perf_delay(mip_perf_info_ptr,
                       PERF_MIP_DEREG_DELAY,
                       PERF_START_TS);
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
      break;

    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
      /* ignore */
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
                    session->rsm_info.state);		
      ASSERT(0);
    } /* switch(rsm state) */
    break;
    
    /*-----------------------------------------------------------------------
                           E8 - RSM_RRQ_FAIL_HA_UNAVAIL_EV

      The registration failed because the HA was unavailable
        Re-RRQ: post Re-RRQ failed event to MetaSM, goto No Session
        de-RRQ: go to deregistered
        Init-RRQ: reset retry count, reset timer and increment ha index
        No Session/Registered/Deregistered: ignore
    -----------------------------------------------------------------------*/
  case RSM_RRQ_FAIL_HA_UNAVAIL_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      session->rsm_info.rrq_cnt = 0;
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      session->curr_ha_index += 1;
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;

    case RSMI_RE_RRQ_STATE:
      goto Re_RRQ_2_No_Session;
      
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      DATA_MIP_MSG0(MSG_LEGACY_MED, "MIP HA unavailable. Continue tear-down");
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    case RSMI_NO_SESSION_STATE:
    case RSMI_REGISTERED_STATE:
      /* ignore */
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
                    session->rsm_info.state);		
      ASSERT(0);
    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                            RSMI_RRQ_TIMER_EXP_EV

      The registration timer expired - time to send a new registration
        Registered State/Re-RRQ: go to the Re-RRQ state
        De-RRQ: go to De-RRQ state
        Initial RRQ: return to the Initial RRQ State
        Deregistered/No Session State: assert()
    -----------------------------------------------------------------------*/
  case RSMI_RRQ_TIMER_EXP_EV:
    switch(session->rsm_info.state)
    {
    case RSMI_REGISTERED_STATE:
      /*---------------------------------------------------------------------
        If the "reregister only if data transferred since last registration"
        setting is enabled and no data has been transferred since previous 
        registration, remain in registered state until data is transferred.
      ---------------------------------------------------------------------*/
      if( mip_data_sent_since_reg == FALSE )
      {
        DATA_MIP_MSG0(MSG_LEGACY_MED, "No IP traffic since last reg, delay rereg");
        mip_rereg_pending = TRUE;
        break;
      }

      DATA_MIP_MSG0(MSG_LEGACY_MED, "Begin MIP re-registration");
      session->rsm_info.rrq_cnt = 0;
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
      /*------------------------------------------------------------------- 
        Install IP filters 
      -------------------------------------------------------------------*/
      if (!mipio_start_udp_snooping(MIP_SES_PTR_2_INDEX(session)))
      {
        /*----------------------------------------------------------------- 
          FUTURE: mip_meta_sm_post_event(MSM_RE_RRQ_FAILED_EV);
                 and make sure to start timer if call fails
        -----------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg UDP filters for MIP failed");
      }
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      rsmi_transition_state(session, RSMI_RE_RRQ_STATE);
      break;

    case RSMI_INIT_RRQ_STATE:
      DATA_MIP_MSG0(MSG_LEGACY_MED, 
                    "RRQ Timer expired.  Resend registration req");
      immed_rrq_retry_reqd = TRUE;
      rsmi_transition_state(session, RSMI_INIT_RRQ_STATE);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      DATA_MIP_MSG0(MSG_LEGACY_MED, 
                    "RRQ Timer expired.  Resend deregistration req");
      immed_derrq_retry_reqd = TRUE;
      io_failure = FALSE;
      rsmi_transition_state(session, RSMI_DE_RRQ_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      /*---------------------------------------------------------------------
        RRQ_TIMER_EXP_EV received while in DEREGISTERED state. - Ignore.
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "RRQ Timer expired while in DEREGISTERED state");
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    case RSMI_NO_SESSION_STATE:
      /*---------------------------------------------------------------------
        RRQ_TIMER_EXP_EV received while in NO_SESSION state. - Ignore.
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "RRQ Timer expired while in NO_SESSION state");
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
                    session->rsm_info.state);		
      ASSERT(0);

    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                          E10 - RSMI_LIFE_TIMER_EXP_EV

      Lifetime timer expired
        Init-RRQ/No Session: assert()
        Re-RRQ: transition to No Session 
        Registered: throw error and go to No Session
    -----------------------------------------------------------------------*/
  case RSMI_LIFE_TIMER_EXP_EV:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Got Life Timer Expired event in %d state!",session->rsm_info.state );
    switch(session->rsm_info.state)
    {
    case RSMI_REGISTERED_STATE:
      /* fall through */

    case RSMI_RE_RRQ_STATE:
      goto Re_RRQ_2_No_Session;
      
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      /*---------------------------------------------------------------------
        LIFE_TIMER_EXP_EV received while in DEREGISTERED state. - Ignore.
      ---------------------------------------------------------------------*/
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    case RSMI_NO_SESSION_STATE:
      /*---------------------------------------------------------------------
        LIFE_TIMER_EXP_EV received while in NO_SESSION state. - Ignore.
      ---------------------------------------------------------------------*/
      break;

    case RSMI_INIT_RRQ_STATE:
      /*---------------------------------------------------------------------
        LIFE_TIMER_EXP_EV received while in INIT_RRQ  state. - Ignore.
      ---------------------------------------------------------------------*/
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
                    session->rsm_info.state);		
      ASSERT(0);

    } /* switch(rsm state) */
    break;


    /*-----------------------------------------------------------------------
                              E2 - RSM_SOL_FAILED_EV

      Solicitation failed
        No Session/Re-RRQ/Registered: post Re-RRQ failure to Meta SM go to no
                                      session
        Init-RRQ: post Init-RRQ failure to Meta SM go to no session
    -----------------------------------------------------------------------*/
  case RSM_SOL_FAILED_EV:
    session->mip_fail_reason = MIP_FAIL_REASON_SOL_TIMEOUT;
    switch(session->rsm_info.state)
    {
    case RSMI_INIT_RRQ_STATE:
      /* fall through */
      /*---------------------------------------------------------------------
        The state transition is happening before the event is posted to avoid
        entering the No Session state twice (and doing all that work).  (This
        is an optimization because we know how the interaction with the meta
        state machine works.)
      ---------------------------------------------------------------------*/
    case RSMI_NO_SESSION_STATE:
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      mip_meta_sm_post_event(MSM_INIT_RRQ_FAILED_EV, session->iface_ptr);
      break;

    Re_RRQ_2_No_Session:
      /* fall through */
    case RSMI_RE_RRQ_STATE:
      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      /* fall through */
    case RSMI_REGISTERED_STATE:
      /*---------------------------------------------------------------------
        The state transition is happening before the event is posted to avoid
        entering the No Session state twice (and doing all that work).  (This
        is an optimization because we know how the interaction with the meta
        state machine works.)
      ---------------------------------------------------------------------*/
      rsmi_transition_state(session, RSMI_NO_SESSION_STATE);
      mip_meta_sm_post_event(MSM_RE_RRQ_FAILED_EV, session->iface_ptr);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case RSMI_DE_RRQ_STATE:

      /*---------------------------------------------------------------------
        Deinstall IP filters 
      ---------------------------------------------------------------------*/
      mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));
      rsmi_transition_state(session, RSMI_DEREGISTERED_STATE);
      break;

    case RSMI_DEREGISTERED_STATE:
      /*---------------------------------------------------------------------
        SOL_FAILED_EV received while in DEREGISTERED state. - Ignore.
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Ignore Sol Failed event while in DEREGISTERED state");
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid rsm state %d ",
                    session->rsm_info.state);    
      ASSERT(0);

    } /* switch(rsm state) */
    break;
    

    /*-----------------------------------------------------------------------
                                   DEFAULT
    -----------------------------------------------------------------------*/
  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid event posted to reg SM %d ",
                  event);
    ASSERT(0);

  } /* switch(event) */

} /* mip_reg_sm_post_event() */



/*===========================================================================
FUNCTION MIP_REG_SM_POST_EVENT_ALL()

DESCRIPTION
  This function posts an event to ALL of the registration state machines.

PARAMETERS
  event: The event to be posted to the state machines.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_post_event_all
(
  mip_reg_sm_event_type event
)
{
  int loop;
  mip_session_info_type *mip_ses_info_array;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------ 
    Get MIP session Information Array.
  ------------------------------------------------------------------------*/
  mip_ses_info_array = mip_get_ses_info_array();

  for(loop = 0; loop < MIP_MAX_SESSIONS; loop += 1)
  {
    mip_reg_sm_post_event(mip_ses_info_array + loop, event);
  }
} /* mip_reg_sm_post_event_all() */



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION RSMI_RRQ_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the registration
  request timer.

PARAMETERS
  param: this is passed in as a void* put it should point to the relevant
  Mobile IP session info block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_rrq_timer_cb
(
  void *param
)
{
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If we are starting re-registration, notify the MIP performance module
  -------------------------------------------------------------------------*/
  if ( ((mip_session_info_type *) param)->rsm_info.state == 
       RSMI_REGISTERED_STATE )
  { 
    /*-----------------------------------------------------------------------
      Only start the REREG Delay timer if there is data traffic since
      last registration. Otherwise Re-registration will not happen right 
      away and we do not want to be counting time.
    -----------------------------------------------------------------------*/
    if (mip_data_sent_since_reg == TRUE) 
    {
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_REREG_DELAY,
                     PERF_START_TS);
    }
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
  
  mip_reg_sm_post_event((mip_session_info_type*)param,RSMI_RRQ_TIMER_EXP_EV);

} /* rsmi_rrq_timer_cb() */

#ifdef FEATURE_DS_MOBILE_IP_DEREG
/*===========================================================================
FUNCTION RSMI_DERRQ_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the deregistration
  request timer.

PARAMETERS
  param: this is passed in as a void* put it should point to the relevant
  Mobile IP session info block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_derrq_timer_cb
(
  void *param
)
{
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If we are starting re-registration, notify the MIP performance module
  -------------------------------------------------------------------------*/
  if ( ( ( (mip_session_info_type *) param)->rsm_info.state == 
               RSMI_REGISTERED_STATE ) ||
       ( ( (mip_session_info_type *) param)->rsm_info.state == 
               RSMI_RE_RRQ_STATE ) ) 
  { 
    /*-----------------------------------------------------------------------
      Only start the DEREG Delay timer if there is data traffic since
      last registration. Otherwise Re-registration will not happen right 
      away and we do not want to be counting time.
    -----------------------------------------------------------------------*/
    if (mip_data_sent_since_reg == TRUE) 
    {
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_DEREG_DELAY,
                     PERF_START_TS);
    }
  }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
  
  mip_reg_sm_post_event((mip_session_info_type*)param,RSMI_RRQ_TIMER_EXP_EV);

} /* rsmi_derrq_timer_cb() */
#endif /* FEATURE_DS_MOBILE_IP_DEREG */


/*===========================================================================
FUNCTION RSMI_LIFE_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the registration
  request timer.

PARAMETERS
  param: this is passed in as a void* put it should point to the relevant
  Mobile IP session info block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_life_timer_cb
(
  void *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mip_reg_sm_post_event((mip_session_info_type*)param,
                        RSMI_LIFE_TIMER_EXP_EV);

} /* rsmi_life_timer_cb() */


/*===========================================================================
FUNCTION RSMI_RRQ_HOLDOFF_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the registration
  holdoff timer.  It sets the flag that indicates it has been at least
  one second since the last RRQ.

PARAMETERS
  param: pstimer passes a pointer to the session info block corresponding  
         to the timer that expires, which was provided when the timer was 
         allocated.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  This session's one_second_since_last_rrq flag is set asynchronously.  
  This must be the only place where this value is written.
===========================================================================*/
void rsmi_rrq_holdoff_timer_cb
(
  void *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ((mip_session_info_type*)param)->rsm_info.one_second_since_last_rrq = TRUE;

} /* rsmi_rrq_holdoff_timer_cb() */


/*===========================================================================
FUNCTION RSMI_TRANSITION_STATE()

DESCRIPTION
  This function changes the state of the registration state machine and
  performs the actions associated with entry into the state.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_transition_state
(
  mip_session_info_type *session,
  rsmi_state_type new_state  
)
{
  ps_ip_addr_type                       ip_addr;
  sint15                                ps_errno;
  phys_link_state_type                  phys_state;
  ps_iface_state_enum_type              iface_state;
  ps_timer_error_type                   timer_result;
  qword                                 ts_null;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type                *mip_perf_info_ptr = NULL;
#endif
  int64                                 temp_timer = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef T_ARM
  DATA_MIP_MSG3(MSG_LEGACY_MED, "RegSM %d from '%s' to '%s'\n",
          MIP_SES_PTR_2_INDEX(session),
          rsmi_state_names[session->rsm_info.state],
          rsmi_state_names[new_state]);
#else
  DATA_MIP_MSG3(MSG_LEGACY_MED, "RegSM %d from %d to %d\n",
          MIP_SES_PTR_2_INDEX(session),
          session->rsm_info.state,
          new_state);
#endif
 
  /*-------------------------------------------------------------------------
    Initialize ts_null
  -------------------------------------------------------------------------*/
   qw_set(ts_null, 0, 0);
   
  /*-------------------------------------------------------------------------
    Perform action associated with entering into this state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                               RSMI_NO_SESSION_STATE

      Reset the FA information
      Cancel reg timer
      Set current home addr to provisioned value
      Set current rev tun pref to provisioned value
    -----------------------------------------------------------------------*/
  case RSMI_NO_SESSION_STATE:


    /*-----------------------------------------------------------------------
      Since the MIP session has ended, if this is the last active session,
      restore the max solicitation retries to the initial value for the next
      call setup.
    -----------------------------------------------------------------------*/
    /* if (mip_in_session()) - add this when multiple sessions supported */
    MIP_SOL_SM_SET_MAX_RETRIES( SSM_INITIAL_SOL_RETRIES );

    /*-----------------------------------------------------------------------
      Re-initialize the session info to be ready for a subsequent call.
    -----------------------------------------------------------------------*/
    timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
    timer_result = ps_timer_cancel(session->rsm_info.life_timer);
    session->curr_ha_index = 0;
    session->reg_id_msw = 0;
    session->reg_id_lsw = 0;
    session->reg_lifetime = 0;
    session->rsm_info.rrq_cnt = 0;
    session->rsm_info.re_rrq_timeout = session->base_retry_interval;
    session->rsm_info.rtt_estimate = 0;
    DATA_MIP_MSG1(MSG_LEGACY_MED, "Set RRP latency estimate to %d", 
             session->rsm_info.rtt_estimate);
    break;

    /*-----------------------------------------------------------------------
                             RSMI_INIT_RRQ_STATE

      Set the force_dormancy variable to FALSE at the begining of a call. We
      want to force the mobile to go dormant only on re-regsistrations and  
      not during intial call setup
    -----------------------------------------------------------------------*/
  case RSMI_INIT_RRQ_STATE:
    session->rsm_info.force_dormancy = FALSE;
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "Set force_dormancy val to %d",
               session->rsm_info.force_dormancy);

    break;

    /*-----------------------------------------------------------------------
                             RSMI_RE_RRQ_STATE
                                   
      If the previous state was not RSMI_RE_RRQ_STATE, determine if the call 
      was dormant prior to sending the RRQ
    -----------------------------------------------------------------------*/
  case RSMI_RE_RRQ_STATE:

#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      For PZID change initiated handoff, start re-registration timer here.
      This should only happen if there was data traffic since last re-reg so
      we check that mip_data_sent_since_reg is true. 
    -----------------------------------------------------------------------*/
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);
    if ( session->rsm_info.state == RSMI_REGISTERED_STATE &&
        qw_cmp(mip_perf_info_ptr->ts_start, ts_null)==0 )
    {    
      /*---------------------------------------------------------------------
        Notify M.IP call performance that M.IP re-registration is starting.
      ---------------------------------------------------------------------*/
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_REREG_DELAY,
                     PERF_START_TS);
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

    /*-----------------------------------------------------------------------
      Do special start of re-reg processing
      (i.e. the initial transition into Re-reg state)
    -----------------------------------------------------------------------*/
    if(session->rsm_info.state != RSMI_RE_RRQ_STATE)
    {
      /*---------------------------------------------------------------------
        If the mobile is dormant, take note and force dormancy after 
        re-reg is complete.
      ---------------------------------------------------------------------*/
      ASSERT( session->iface_ptr );
      if( ((phys_state  = ps_iface_phys_link_state(session->iface_ptr)) == 
           PHYS_LINK_STATE_INVALID) ||
          ((iface_state = ps_iface_state(session->iface_ptr)) ==
           IFACE_STATE_INVALID))
      {
        /*-------------------------------------------------------------------
          To be safe don't go dormant   
        -------------------------------------------------------------------*/
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Unable to get iface 0x%x state", session->iface_ptr); 
        session->rsm_info.force_dormancy = FALSE;
      }
      else if((iface_state == IFACE_UP || 
               iface_state == IFACE_ROUTEABLE) &&
              (phys_state == PHYS_LINK_DOWN || 
               phys_state == PHYS_LINK_NULL ||
               phys_state == PHYS_LINK_GOING_NULL ||
               phys_state == PHYS_LINK_GOING_DOWN))
      {
        session->rsm_info.force_dormancy = TRUE;
      }
      else
      {
        session->rsm_info.force_dormancy = FALSE;
      }
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Dormancy mode is %d",session->rsm_info.force_dormancy);
    }
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                             RSMI_DE_RRQ_STATE
                                   
    -----------------------------------------------------------------------*/
  case RSMI_DE_RRQ_STATE:

#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*---------------------------------------------------------------------
      Notify M.IP call performance that M.IP re-registration has stopped.
    ---------------------------------------------------------------------*/
    if (session->rsm_info.state == RSMI_RE_RRQ_STATE)
    {
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);  
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_REREG_DELAY,
                     PERF_END_TS);
    }

    /*-----------------------------------------------------------------------
      This should only happen if there was data traffic since last de-reg so
      we check to see if we are transitioning from REGISTERED_STATE or 
      RE_RRQ_STATE.
    -----------------------------------------------------------------------*/
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_DEREG_DELAY);
    if ( ( (session->rsm_info.state == RSMI_REGISTERED_STATE) || 
           (session->rsm_info.state == RSMI_RE_RRQ_STATE) ) &&
        qw_cmp(mip_perf_info_ptr->ts_start, ts_null)==0 )
    {    
      /*---------------------------------------------------------------------
        Notify M.IP call performance that M.IP de-registration is starting.
      ---------------------------------------------------------------------*/
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_DEREG_DELAY,
                     PERF_START_TS);
    }

#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                            RSMI_REGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_REGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      Notify M.IP call performance that M.IP re-registration is complete
      only if we are in the re-rrq state.  If we are in the initial-rrq
      state, notify M.IP call performance that M.IP registration is 
      complete.
    -----------------------------------------------------------------------*/
    if (session->rsm_info.state == RSMI_RE_RRQ_STATE)
    {
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);  
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_REREG_DELAY,
                     PERF_END_TS);  

      /*---------------------------------------------------------------------
        If M.IP registration was due to a handoff, notify M.IP call 
        performance that M.IP handoff is complete.
      ---------------------------------------------------------------------*/      
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_HANDOFF_DELAY);
      if (qw_cmp(mip_perf_info_ptr->ts_start, 
                 ts_null) != 0)       
      {      
        mip_perf_delay(mip_perf_info_ptr,
                       PERF_TOTAL_HANDOFF_DELAY,
                       PERF_END_TS);        
      }
    }
    else if (session->rsm_info.state == RSMI_INIT_RRQ_STATE) 
    {
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_INIT_REG_DELAY);
      mip_perf_delay(mip_perf_info_ptr,
                     PERF_MIP_INIT_REG_DELAY,
                     PERF_END_TS);
    }
#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                            RSMI_DEREGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_DEREGISTERED_STATE:
#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      Notify M.IP call performance that M.IP de-registration is complete
    -----------------------------------------------------------------------*/
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_DEREG_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_MIP_DEREG_DELAY,
                   PERF_END_TS);  

#endif /* FEATURE_DS_MOBILE_IP_PERF */
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid new state for RegSM %d ",
                  new_state);
    ASSERT(0);
  } /* switch( new state ) */

  /*-------------------------------------------------------------------------
    Transition into state
  -------------------------------------------------------------------------*/
  session->rsm_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform actions that occur after state transition
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                            RSMI_NO_SESSION_STATE

      All you have to do is set some Delay counters back to zero so they can
      be restarted in the right state.
    -----------------------------------------------------------------------*/
  case RSMI_NO_SESSION_STATE:

#ifdef FEATURE_DS_MOBILE_IP_PERF
    
    /*-----------------------------------------------------------------------
      Notify M.IP call performance that the Delay timer needs to be reset.
      At this point we can cancel all but the PERF_UM_PPP_DELAY and 
      PERF_RM_PPP_DELAY.
    -----------------------------------------------------------------------*/
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_INIT_REG_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_MIP_INIT_REG_DELAY,
                   PERF_CANCEL_TS);
    
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_REREG_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_MIP_REREG_DELAY,
                   PERF_CANCEL_TS);
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_DEREG_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_MIP_DEREG_DELAY,
                   PERF_CANCEL_TS);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_TOTAL_CALL_SETUP_DELAY,
                   PERF_CANCEL_TS);

    mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_HANDOFF_DELAY);
    mip_perf_delay(mip_perf_info_ptr,
                   PERF_TOTAL_HANDOFF_DELAY,
                   PERF_CANCEL_TS);
#endif /* FEATURE_DS_MOBILE_IP_PERF */     

    break;

    /*-----------------------------------------------------------------------
                            RSMI_REGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_REGISTERED_STATE:

    /*-----------------------------------------------------------------------
      Now that we are registered, increase the max solicitation retries 
      so we are more tolerant of interruptions (e.g. registration/SMS/etc.
      in hybrid devices) during a handoff once we already have a 
      registration.
    -----------------------------------------------------------------------*/
    MIP_SOL_SM_SET_MAX_RETRIES( SSM_HANDOFF_SOL_RETRIES );

    /*-----------------------------------------------------------------------
      Deinstall IP filters 
    -----------------------------------------------------------------------*/
    mipio_stop_udp_snooping(MIP_SES_PTR_2_INDEX(session));

    /*-----------------------------------------------------------------------
      Cancel the registration and lifetime timers
    -----------------------------------------------------------------------*/
    timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);
    timer_result = ps_timer_cancel(session->rsm_info.life_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      Set the timers
    -----------------------------------------------------------------------*/
    rsmi_set_registered_state_timers(session);

    /*-----------------------------------------------------------------------
      Set the IP address of the mobile to that in the RRP
      (could change from requested static IP or be assigned dynamically)
    -----------------------------------------------------------------------*/
    ip_addr.type    = IPV4_ADDR;
    ip_addr.addr.v4.ps_s_addr = session->home_addr;
    DATA_MIP_MSG1(MSG_LEGACY_MED, 
                  "Set IP addr to %x", ip_addr.addr.v4.ps_s_addr);
    
    if (ps_iface_set_addr( session->iface_ptr, &ip_addr) < 0)
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Could not set iface address");
    }
    

    session->rsm_info.rrq_cnt = 0;
    session->rsm_info.re_rrq_timeout = session->base_retry_interval;
    immed_rrq_retry_reqd = TRUE;

    /*-----------------------------------------------------------------------
      Reset rereg_reqd flag since we want to know if any data transfer takes 
      place after the registration. When any data is transferred, PPP code 
      would set the flag. Note that this behavior is configurable in NV and 
      rrq_if_tfrk stores this preference.
    -----------------------------------------------------------------------*/
    if (session->rrq_if_tfrk == TRUE)
    {
      mip_data_sent_since_reg = FALSE;
    }
    mip_rereg_pending = FALSE;

    /*-----------------------------------------------------------------------
      Post an event to the DSSNET State machine that M. IP registration has
      completed successfully
    -----------------------------------------------------------------------*/
    
    mip_call_event_callback(MIP_SUCCESS_EV);

    /*-----------------------------------------------------------------------
      Release the call if the call was dormant prior to start of 
      re-registration
    -----------------------------------------------------------------------*/
    if(session->rsm_info.force_dormancy)
    {
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Registration done. Forcing dormancy");

      /*---------------------------------------------------------------------
        Issue an ioctl to the MIP interface to go dormant
      ---------------------------------------------------------------------*/
      
      if(ps_phys_link_ioctl(PS_IFACE_GET_PHYS_LINK(session->iface_ptr),
                            PS_PHYS_LINK_IOCTL_GO_DORMANT,
                            NULL,
                            &ps_errno) < 0)
      {
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Force dorm failed, err=%d", ps_errno);
      }
    }
    else
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Call was not dormant prior to reg");
    }
    break;
  
    /*-----------------------------------------------------------------------
                              RSMI_INIT_RRQ_STATE

      If another RRQ is to be sent, send it and start timer.
      Otherwise, post the RRQ Failed w/o Retry event.
    -----------------------------------------------------------------------*/
  case RSMI_INIT_RRQ_STATE:

    if(session->rsm_info.rrq_cnt >= session->rsm_info.max_rrqs)
    {
      DATA_MIP_MSG1(MSG_LEGACY_MED, "HA %d not responding trying next",session->curr_ha_index);
      session->curr_ha_index += 1;
      session->rsm_info.rrq_cnt = 0;
      session->rsm_info.re_rrq_timeout = session->base_retry_interval;
    }

    /*----------------------------------------------------------------------
      Check if p_rev_index has changed - if the new p_rev doesn't support
       post MIP_EXIT - and we will fallback to SIP, if allowed
    -----------------------------------------------------------------------*/

    if ((session->curr_ha_index < MAX_HA_ADDRS)  &&
        (session->ha_addrs[session->curr_ha_index] != UNSET_HA_ADDR) && 
        (
          ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA) && 
               dssnet4_1x_mip_support())
          ||   (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR))
          ||   (dshdr_get_mobile_ip_uses_ccoa())
        )
       )
    {
      if (!mip_sol_sm_active())
      {
        mip_outmsg_send_rrq(session);
      }
      /*---------------------------------------------------------------------
        We add an offset to the rrq_timeout to avoid the stale challenge 
        problem caused by the request going out at fixed intervals
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(session->rsm_info.reg_timer,
                                    session->rsm_info.re_rrq_timeout + 
                                    session->retry_interval_offset + 
                                    session->rsm_info.rtt_estimate);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      DATA_MIP_MSG1(MSG_LEGACY_MED, "Set registration timer for %dms\n", 
               session->rsm_info.re_rrq_timeout +
               session->retry_interval_offset +
               session->rsm_info.rtt_estimate);
      session->retry_interval_offset = 0;

      if (!mip_sol_sm_active())
      {
        session->rsm_info.rrq_cnt += 1;
        session->rsm_info.re_rrq_timeout *= MIP_RRQ_RETRY_BACKOFF_MULT;
      }
    }
    else
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Init RRQ failed - giving up");
      mip_reg_sm_post_event(session, RSM_RRQ_FAIL_NO_RETRY_EV);
    }
    break;


    /*-----------------------------------------------------------------------
                              RSMI_RE_RRQ_STATE

      Change Um stack to internal mode, send RRQ, start RRQ timer, 
      and increment backoff if it hasn't hit the ceiling.
    -----------------------------------------------------------------------*/
  case RSMI_RE_RRQ_STATE:

    timer_result = ps_timer_cancel(session->rsm_info.reg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      Check if p_rev_index has changed - if the new p_rev doesn't support
      post MIP_EXIT - and we will fallback to SIP, if allowed
    -----------------------------------------------------------------------*/
    if( ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA) && 
             dssnet4_1x_mip_support())
        ||   (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR))
        ||    dshdr_get_mobile_ip_uses_ccoa()
      )
    {
      if (!mip_sol_sm_active())
      {
        mip_outmsg_send_rrq(session);
      }

      /*---------------------------------------------------------------------
        We add an offset to the rrq_timeout to avoid the stale challenge 
        problem caused by the request going out at fixed intervals
      ---------------------------------------------------------------------*/
      timer_result = ps_timer_start(session->rsm_info.reg_timer,
                                    session->rsm_info.re_rrq_timeout +
                                    session->retry_interval_offset +
                                    session->rsm_info.rtt_estimate);
      ASSERT(timer_result != PS_TIMER_FAILURE);
 
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Set registration timer for %dms\n", 
               session->rsm_info.re_rrq_timeout +
               session->retry_interval_offset +
               session->rsm_info.rtt_estimate);
      session->retry_interval_offset = 0;

      if (!mip_sol_sm_active())
      {
        /*-------------------------------------------------------------------
          Calculate the RRQ timeout for the next RRQ sent (next time 
          through this state transition, not the current RRQ timeout).
        -------------------------------------------------------------------*/
        if(session->rsm_info.re_rrq_timeout <= RSMI_MAX_RETRY_BACKOFF_TIME)
        {
          session->rsm_info.re_rrq_timeout *= MIP_RRQ_RETRY_BACKOFF_MULT;
        }
      }
    }
    else
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "prev doesnt support MIP - exit MIP" );
      mip_reg_sm_post_event(session, RSM_RRQ_FAIL_NO_RETRY_EV);
    }
    break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    /*-----------------------------------------------------------------------
                              RSMI_DE_RRQ_STATE

      Change Um stack to internal mode, send De-RRQ, start De-RRQ timer, 
      and increment backoff if it hasn't hit the ceiling.
    -----------------------------------------------------------------------*/
  case RSMI_DE_RRQ_STATE:

    timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    if(session->rsm_info.rrq_cnt < session->rsm_info.max_derrqs)
    {

      if (!mip_sol_sm_active())
      {
        mip_outmsg_send_dereg_rrq(session);
      }

      /*---------------------------------------------------------------------
        We add an offset to the rrq_timeout to avoid the stale challenge 
        problem caused by the request going out at fixed intervals
      ---------------------------------------------------------------------*/
      if( (session->rsm_info.max_derrqs == 1) &&
          io_failure == TRUE && 
          immed_derrq_retry_reqd == FALSE )
      {
        temp_timer = RSMI_MAX_DERRQ_TIMEOUT;        
      }
      else
      {
        temp_timer = session->rsm_info.re_rrq_timeout;
      }

      timer_result = ps_timer_start(session->rsm_info.dereg_timer,
                                    temp_timer +
                                    session->retry_interval_offset +
                                    session->rsm_info.rtt_estimate);
      ASSERT(timer_result != PS_TIMER_FAILURE);

      DATA_MIP_MSG1(MSG_LEGACY_MED, "Set registration timer for %dms\n", 
               temp_timer +
               session->retry_interval_offset +
               session->rsm_info.rtt_estimate);
      session->retry_interval_offset = 0;

      if (!mip_sol_sm_active())
      {
        session->rsm_info.rrq_cnt += 1;
        session->rsm_info.re_rrq_timeout *= MIP_RRQ_RETRY_BACKOFF_MULT;
      }
    }
    else
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Deregistration RRQ failed - giving up");
      mip_reg_sm_post_event(session, RSM_RRQ_FAIL_NO_RETRY_EV);
    }
    break;

    /*-----------------------------------------------------------------------
                            RSMI_DEREGISTERED_STATE

      comments inlined below
    -----------------------------------------------------------------------*/
  case RSMI_DEREGISTERED_STATE:

    /*-----------------------------------------------------------------------
      Cancel the registration and life timer. 
    -----------------------------------------------------------------------*/
    timer_result = ps_timer_cancel(session->rsm_info.dereg_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    timer_result = ps_timer_cancel(session->rsm_info.life_timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      Post an event to the DSSNET State machine that M. IP deregistration has
      completed successfully
    -----------------------------------------------------------------------*/
    mip_call_event_callback(MIP_DEREGED_EV);

    immed_derrq_retry_reqd = TRUE;
    io_failure = FALSE;
    break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid new state for RegSM %d ",
                  new_state);
    ASSERT(0);
  } /* switch( new state ) */

} /* rsmi_transition_state() */



/*===========================================================================
FUNCTION RSMI_SET_REGISTERED_STATE_TIMERS()

DESCRIPTION
  This function sets both the lifetime and re-rrq timers for the registered
  state.

PARAMETERS
  session: pointer for the session info block in question.

RETURN VALUE
  None

DEPENDENCIES
  PS timers has to have been initialized, as does the Registration State
  Machine.

SIDE EFFECTS
  None
===========================================================================*/
void rsmi_set_registered_state_timers
(
  mip_session_info_type *session
)
{
  ps_timer_error_type  timer_result;
  uint32  s_lifetime;                       /* lifetime provided by the HA */
  uint32  s_re_rrq;                         /* time when re-reg begins     */
  uint32  s_b4_lifetime_expiry;             /* pre-lifetime expiry period  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
      Only set the lifetime and the RRQ timers if the lifetime is not
      infinite (0xFFFF) per RFC2002 S2.1.1
  -------------------------------------------------------------------------*/
  if(session->reg_lifetime < 0xFFFF)
  {
    s_lifetime = session->reg_lifetime;
    timer_result = ps_timer_start(session->rsm_info.life_timer,
                                  ( ((int64)s_lifetime) * 1000) );
    ASSERT(timer_result != PS_TIMER_FAILURE);
    DATA_MIP_MSG2(MSG_LEGACY_MED, "Set Lifetime timer (%d) for %ds", 
            session->rsm_info.life_timer, s_lifetime);

    /*-----------------------------------------------------------------------
      if the pre-re-registration time or the provisioned max time to 
      register is more than half of the registration lifetime, then set the 
      RRQ timer to half of the lifetime
    -----------------------------------------------------------------------*/
    s_b4_lifetime_expiry = session->rsm_info.pre_re_rrq_time / 1000;

    if(s_b4_lifetime_expiry > (s_lifetime/2) ||
       (uint32) (session->max_reg_time/1000) > (s_lifetime/2))
    {
      s_re_rrq = (s_lifetime/2);
      DATA_MIP_MSG1(MSG_LEGACY_MED, "RRQ timer = lifetime/2 = %ds", s_re_rrq);
    }

    /*-----------------------------------------------------------------------
      else if the pre-re-registration time is 0 then set the RRQ timer to 
      the lifetime minus the provisioned max time to register
    -----------------------------------------------------------------------*/
    else if (s_b4_lifetime_expiry == 0)
    {
      s_re_rrq = s_lifetime - (session->max_reg_time / 1000);
      DATA_MIP_MSG1(MSG_LEGACY_MED, "RRQ timer = lifetime - max reg time = %ds", s_re_rrq);
    }

    /*-----------------------------------------------------------------------
      else (the pre-rrq time is good) then set the rrq timer to the 
      lifetime minus the pre-rrq time.
    -----------------------------------------------------------------------*/
    else 
    {
      s_re_rrq = s_lifetime - s_b4_lifetime_expiry;
      DATA_MIP_MSG1(MSG_LEGACY_MED, "(re-RRQ timer -= s_b4_lifetime_expiry) = %ds", s_re_rrq);
    }

    timer_result = ps_timer_start(session->rsm_info.reg_timer, 
                                  ( ((int64)s_re_rrq) * 1000) );
    ASSERT(timer_result != PS_TIMER_FAILURE);
    DATA_MIP_MSG2(MSG_LEGACY_MED, "Set RRQ timer (%d) for %ds", 
            session->rsm_info.reg_timer, s_re_rrq);
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_MED, "Infinite reg - no RRQ timer");
  }

} /* rsmi_set_registered_state_timers() */



/*===========================================================================
FUNCTION MIP_REG_SM_IMMED_REGISTRATION_ALL

DESCRIPTION
  This function posts IMMED_RETRY_EV to every registration state machine
  which is in RE_RRQ_STATE.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_immed_registration_all
(
  void
)
{
  int index;
  mip_session_info_type *mip_ses_info_array;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------ 
    Get MIP session Information Array.
  ------------------------------------------------------------------------*/
  mip_ses_info_array = mip_get_ses_info_array();

  DATA_MIP_MSG0(MSG_LEGACY_MED, "Reregister all MIP sessions!");
  for(index = 0; index < MIP_MAX_SESSIONS; index++)
  {
    /*-----------------------------------------------------------------------
      Install IP filters if coming out of registered state (always true,
      but check anyway in case someone decides to call this API elsewhere)
    -----------------------------------------------------------------------*/
    if( mip_ses_info_array[index].rsm_info.state == RSMI_REGISTERED_STATE &&
        !mipio_start_udp_snooping(index) )
    {
      /*---------------------------------------------------------------------
        If UDP filter registration fails, leave call connected until end of 
        current registration, but don't bother trying to re-register since
        the RRP can never be received.
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg UDP filters for MIP failed - "
                    "call will drop at end of current registration");
    }
    else if( mip_ses_info_array[index].rsm_info.state != 
             RSMI_NO_SESSION_STATE )
    {
      mip_reg_sm_post_event( mip_ses_info_array + index, 
                             RSMI_RRQ_TIMER_EXP_EV );
    }
  }
} /* mip_reg_sm_immed_registration_all() */

/*===========================================================================
FUNCTION MIP_REG_SM_ALL_SESSIONS_REGD

DESCRIPTION
  Checks if all sessions are registered

PARAMETERS
  None

RETURN VALUE
  TRUE if all sessions are registered
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_reg_sm_all_sessions_regd
(
  void
)
{
  int     index;
  mip_session_info_type *mip_ses_info_array;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------ 
    Get MIP session Information Array.
  ------------------------------------------------------------------------*/
  mip_ses_info_array = mip_get_ses_info_array();

  /*-------------------------------------------------------------------------
    If Solicitation state machine is active, we are in a registration.
    This may be true for a very small window during a resolicitation.
  -------------------------------------------------------------------------*/
  if (mip_sol_sm_active())
  {
    return FALSE;
  }

  for(index = 0; index < MIP_MAX_SESSIONS; index++)
  {
    if ( mip_ses_info_array[index].rsm_info.state != RSMI_REGISTERED_STATE )
    {
      return FALSE; 
    }
  }

  return TRUE;
} /* mip_reg_sm_all_sessions_regd */


/*===========================================================================
FUNCTION MIP_REG_SM_SESSION_ACTIVE()

DESCRIPTION

  This function checks if Mobile IP session currently has a valid 
  registration (registered or reregistering) on the Um interface.

  This function executes in the context of the application task.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE

  TRUE:  If MIP is active
  FALSE: If MIP is not active

SIDE EFFECTS
  None.
===========================================================================*/
boolean mip_reg_sm_session_active
(
  mip_session_info_type *  session
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Hard coded to check MIP session #0 for now.  
    Change if multiple MIP sessions are supported.
  -------------------------------------------------------------------------*/
  session = mip_get_ses_info_array();

  if( session->rsm_info.state == RSMI_REGISTERED_STATE ||
      session->rsm_info.state == RSMI_RE_RRQ_STATE )
  {
    return( TRUE );
  }
  else
  {
    return( FALSE );
  }

} /* mip_reg_sm_session_active() */


/*===========================================================================
FUNCTION MIP_REG_SM_SET_RTT_ESTIMATION

DESCRIPTION
  Sets the estimated RTT between sending RRQ and RRP based on the current 
  rrq_timeout timer value.

PARAMETERS
  None

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_set_rtt_estimation
(
  mip_session_info_type * session
)
{
  int64 reg_timer_left;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_timer_is_running(session->rsm_info.reg_timer))
  {
    /*-----------------------------------------------------------------------
      Set the RTT estimation based on the time left in the re-RRQ timer
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG3(MSG_LEGACY_MED, "Re_rrq_to %d  offset %d  rtt_est %d", 
             session->rsm_info.re_rrq_timeout,
             session->retry_interval_offset,
             session->rsm_info.rtt_estimate);

    reg_timer_left = ps_timer_remaining(session->rsm_info.reg_timer);
    if (reg_timer_left == -1)
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg timer doesn't exist! reset RTT estimate");
      session->rsm_info.rtt_estimate = 0;
      return;
    }

    session->rsm_info.rtt_estimate = (uint16)((session->rsm_info.re_rrq_timeout /
                                      MIP_RRQ_RETRY_BACKOFF_MULT) +
                                     session->retry_interval_offset +
                                     session->rsm_info.rtt_estimate -
                                     reg_timer_left);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Registration timer isn't running, so reset the RTT estimate to 0
    -----------------------------------------------------------------------*/
    session->rsm_info.rtt_estimate = 0;
  }

  DATA_MIP_MSG1(MSG_LEGACY_MED, "Set RRP latency estimate to %d", 
           session->rsm_info.rtt_estimate);

} /* mip_reg_sm_set_rtt_estimation() */

/*lint -restore Restore lint error 641*/

/*===========================================================================
FUNCTION MIP_REG_SM_SET_IO_FAILURE

DESCRIPTION
  Sets the IO failure flag.

PARAMETERS
  boolean - flag

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_set_io_failure
(
  boolean flag
)
{
  io_failure = flag;
}

#if 0
/*============================================================================
FUNCTION MIP_GET_DATA_SENT_SINCE_REG()

DESCRIPTION
  Access function for mip_data_sent_since_reg flag.

PARAMETERS
  None

RETURN VALUE
  mip_data_sent_since_reg flag value.

DEPENDENCIES
  None

============================================================================*/
boolean mip_get_data_sent_since_reg(void)
{
  return mip_data_sent_since_reg;
}

/*============================================================================
FUNCTION MIP_GET_REREG_PENDING()

DESCRIPTION
  Access function for mip_rereg_pending flag.

PARAMETERS
  None

RETURN VALUE
  mip_rereg_pending flag value.

DEPENDENCIES
  None

============================================================================*/
boolean mip_get_rereg_pending(void)
{
  return mip_rereg_pending;
}
#endif /* IF 0 */
#endif /* FEATURE_DS_MOBILE_IP */
