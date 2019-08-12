/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          I D L E  S T A T E  P R O T O C O L

GENERAL DESCRIPTION

   This file contains routines for the Idle Protocol

EXTERNALIZED FUNCTIONS (Global)

  hdridle_abort_open_connection() - Abort open connection request
  hdridle_activate()              - Command function to activate IDLE
  hdridle_deactivate()            - Command function to deactivate IDLE
  hdridle_ind_cb()                - Callback registered for delivery of
                                    indications
  hdridle_msg_cb()                - Callback registered for delivery of
                                    messages
  hdridle_open_connection()       - Open connection request
  hdridle_reselect_is_allowed()   - Check whether re-selection is allowed
                                    or not

  hdridle_powerup_init()          - Function called by hdrmc task to
                                    initialize IDLE once after powerup.
  hdridle_init()                  - Function called by hdrmc task to
                                    initialize IDLE each HDR mode is
                                    entered

EXTERNALIZED FUNCTIONS (Regional)

  hdridle_process_cmd()           - Function called by hdrmc task to
                                    process a command for IDLE.
  hdridle_process_ind()           - Function called by hdrmc task to
                                    process an indication for IDLE
  hdridle_process_msg()           - Function called by hdrmc task to
                                    process a message for IDLE.
  hdridle_process_sig()           - Function called by hdrmc task to
                                    process a signal for IDLE.
  hdridle_start_suspend_timer()   - starts the suspend timer
  hdridle_stop_suspend_timer()    - stops the suspend timer
  hdridle_start_conn_setup_timer()- starts the connection setup time
  hdridle_stop_conn_setup_timer() - stops the connection setup timer
  hdridle_send_conn_req_msg()     - ends the connection request message
  hdridle_set_rcv_end_of_sync_cap_ind() - set the flag for the end of sync
                                          capsule indication received
  hdridle_set_connect_reason      - set the connection setup reason
  hdridle_set_first_sleep_flag    - Flag first sleep after monitoring

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdridle.c_v   1.84   13 Mar 2003 13:48:58   vramaswa  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdridle.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/16   vko     FR 33944: Fix to Race condition between CC pkts ignore and Page
05/09/16   vko     Reduce excessive F3 logging
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
01/19/15   sjj     WLAN coexistence improvements to Type 7 connected Using RRC states
11/14/14   vko     Handle Conn Req tx failure during active HO
07/09/14   cnx     Support NV refresh without reset.
04/14/14   vko     Reset idle_reacq_suspend_to_reacq if LMAC AWAKE is recieved when IDLE is not in MONITOR
04/07/14   vko     Fixed IDLE giving SLEEP command to LMAC, when SRCH moves from 3a->3f
03/12/14   vko     Fixed HDR not doing system loss properly when FR2399 enabled
02/21/14   vko     FR2399 - improved cc handling
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
12/02/13   mbs     Enable sleep for BOLT bringup
11/27/13  dsp/cnx  FR17482 - Page blocking management.
11/21/13   mbs     Disable sleep for BOLT bringup
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
04/15/13   ukl     Discard Quick Page if received in Tunnel mode.
04/03/13   wsh     Fixed not releasing RF during ACT HO.
12/18/12   vko     Added new event EVENT_HDR_MSG_RX_PAGE
03/20/13   cnx     Return to tunnel if sys loss happens during active handoff. 
03/18/13   smd     Enable sleep on TRITON.
03/15/13   smd     Disable sleep on TRITON temporarily.
02/14/13   vko     Fixed AT not able to decode Quick page
12/20/12   vko     Fixed compiler warnings
11/24/12   ukl     Fix Klocwork issues
08/24/12   sju     Fix to Re-try open conn cmd after resume if hdrhmp_pending_queue
                   is having pending RTC MSG from HDRHAI_IDLE_STATE_PROTOCOL
11/05/12   cnx     Don't resend ConnectionRequest during active handoff.
10/03/12   ljl     Enabled sleep by default.
09/11/12   cnx     Handle TX and RX messages during active handoff
08/28/12   ljl     Added a flag to enable sleep.
08/17/12   ljl     Disable sleep on Dime.
08/07/12   cnx     Abort Active Handoff if redirection is received.
07/16/12   cnx     Handle connection request when SAP subtype is 0.
06/27/12   cnx     Handle the situation that ConnectionDeny recieved in 
                   active handoff while ALMP is still in INIT state.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
06/18/12   cnx     Deregister call ID if connection setup is aborted because of
                   HDRHAI_ABORT_PREF_SYS_CHANGE. 
04/25/12   cnx     Go back to tunnel mode if ConnectionDeny is rx'ed during
                   Active Handoff.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
09/24/11   cnx     Changed subtype checking for Optimized Handoff.
08/01/11   cnx     Merged Optimized Handoff changes.
07/28/11   cnx     Supported force long sleep.
05/31/11   cnx     Added HDRTRACE_IDLE_CONNECTION_DENIED_REASON,
                   HDRTRACE_IDLE_CONNECTION_SETUP_FAIL_REASON,
                   and HDRTRACE_IDLE_CONNECT_REASON.
07/21/10   cnx     Added support for GAUP LoadInformationSupported attribute
11/09/10   pba     Handled abort reason HDRHAI_ABORT_UNSPECIFIED.
11/09/10   pba     Fixed issue of DO CP not reporting call failure to CM during
                   BSR attempt.
10/06/10   sju     Fixed not reporting call failure to CM.
09/02/10   pba     Added support for eHRPD to LTE reselection phase 1
06/10/10   pba     IRAT Measurement feature.
05/28/10   pba     Added eHRPD VOIP Silent redial feature.
05/19/10   pba     Declare system loss immediately when AT losses the pilot 
                   upon rude wake up
05/04/10   ljl     Removed sleep disable. 
04/30/10   ljl     Disabled sleep for Voyager 2.0 bringup. 
04/12/10   sju     Removed FEATURE_HDR_CP_9K_BRINGUP.
03/24/10   pxu     Removed FEATURE_HDR_QSM features.
02/21/10   kss     Enable sleep on 9K.
01/12/10   sju     Added FEATURE_HDR_CP_9K_BRINGUP
01/05/10   pba     Fixed AT prematurely going to full sleep when there is no
                   session.
12/01/09   suren   Added QChat optimization changes.
12/03/09   grl     Force slotted sleep off.
12/01/09   grl     Temporarily removed access failure reporting.
11/12/09   sju     Added support for SVDO.
09/30/09   wsh     Fixed KW warnings
09/18/09   wsh     CMI: support changes from modem DATA
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
09/09/09   wsh     CMI: to support changes in clk and ts APIs
05/22/09   wsh     AT stuck in ConSetup state, unable to send AC message
04/10/09   wsh     Fixed not aborting connection when rcvd HDRMC_END_F.
03/27/09   sju     Fixed missing BCMCS overhead message 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/27/09   wsh     Added run-time check for KDDI feature flags
02/10/09   sju     Added support for intra scc sleep.
01/28/09   pba     Always unpack and keep track of the CC change fields in QP 
                   message.
12/23/08   pba     Support for Quick Idle Protocol
12/18/09   sju     Updated f3 messages.
12/10/08   smd     Replaced assert.h with amssassert.h
10/08/08   pba     Fixed critical and high lint errors.
07/11/08   wsh     Compiler warning FEATURE_HDR_DISABLE_SUSPEND_TIMER defined
06/12/08   wsh     Fixed not reporting to CM when receiving IDLE deact.
                   in ConSetupSuspend state.
06/08/08   wsh     Fixed DCM can't turn on when in ConSetup
05/30/08   wsh     Added EVENT trace when reacq fails and DDARF kicks in
05/22/08   sju     Removed sleep hack for 7800 bringup
04/30/08   wsh     Moved code to check if data session is on HDR to ARM9
03/12/08   sju     Temporarily Disalbed sleep for 7800
02/01/08   wsh     Set IDLE to proper state after existing ConSetup from 
                   ConSetupSuspend state.
01/30/08   mpa     Removed sleep hack for 7800 + fixed compiler warnings.
12/12/07   sju     Renamed hdr channel record type structure based on sys.h
11/29/07   sju     Temp disable sleep for 7800 revB
10/17/07   pba     Parsing support for new fields in page and connection request 
                   messages.
11/07/07   wsh     Updated DDARF max react limit
09/11/07   wsh     DoS droppped during AHO is fixed
09/11/07   wsh     Removed lint high warnings
08/23/07   pba     Do not do DDARF for rude wakeups.
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
7/13/07    sju     Changed channel record type to hdr channel record type 
05/16/07   yll     Don't check maintain EVDO session status before going to 
                   sleep if AT just woke up from it.
05/02/07   pba     Print HDRTRACE_LMAC_NETWORK_LOST event when Idle declares 
                   network lost due to DDARF.
03/27/07   pba     Fixed compile issue.
03/20/07   pba     DDARF update.
02/28/07   hal     Added real sleep values to sleep F3 messages
11/27/06   pba     Added hdridle_is_suspended()
11/16/06   sju     Obsoleted HDRIND_FCP_ALL_STREAMS_CLOSED indication.
11/15/06   sju     Add ABORT_MSG as a new reason to abort connection.
11/14/06   wsh     Disallow message on access probe when connection attempt is in
                   progress. This should not affect bundling with ConReq.
10/23/06   mpa     Allow a ConnectionRequest to be re-queued in conn_suspend state
10/09/06   mpa     Fixed typo when aborting connection setup.
10/05/06   wsh     hdridle_is_connecting() TRUE while sending ConnectionReq
10/10/06   hal     Added support to throttle 1x acquisitions if AT negotiates low SCI
09/29/06   yll     Treat SCI < 5 as non slotted mode operation
09/26/06   sju     Changed msg priority of ConnectionRequest to support 
                   bundling with other priority 40 messages.
09/22/06   pba     Added FEATURE_HDR_SSPI.
09/19/06   pba     Added support for DDARF.
09/05/06   mpa     Added support for FEATURE_HDR_QSM.
09/05/06   mpa     Always pack PreferredChannelCount for ConnRequest in EIDLE.
08/29/06   mpa     Added BCMCS flow monitoring as a condition for reselection.
08/03/06   etv     (for Sarika, QCHAT). Exported hdridle_get_current_sci.
08/01/06   sju     Added EVENT_HDR_IDLE_SET_SLEEP_DURATION event. 
07/31/06   mpa     Added support for connection setup suspend state.
06/14/06   etv     Added support for AT init GAUP of SlottedMode
06/20/06   pba     Removed redundant AN auth status maintained by SCP.
05/25/06   dna     Report CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED if suspended
05/10/06   yll/dna Don't allow new AC messages to be sent while waiting for TCA
05/09/06   yll     Added support for Minimal UATI Mode.
04/26/06   hal     Renamed hdridle_con_is_for_signaling() to 
                   hdridle_con_is_for_session_config()
                   Fixed permissible tune-away conditions
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/04/06   kss     Add support for sleep during BCMCS.
04/03/06   yll     Before AT goes to sleep, if it has made an access attempt 
                   since last time entering Monitor state, then call SD's
                   reselection function.
03/29/06   dna     1.  Fail origination while in suspended state
                   2.  Ensure distance based registration occurs consistently
                   3.  Some F3 message cleanup
01/26/06   mpa     Rely on eidle negotiation for enh ConnectionRequest w/ BCMCS
01/16/06   yll     When AN sends the connection deny message due to Auth or
                   Billing failure, avoid the system immediately for 10 min. 
12/13/05   pba     Removed obsoleted SCP_RECONFIGURED indication
12/05/05   yll     For FMAC Supervision failure, don't call SD actions in
                   HDRIDLE, but instead call it in ALMP to avoid reentrant
                   of hdralmp_process_ind() and a subsequent ERR_FATAL.
12/01/05   dna     Allow SD to return ACQ_GW or any other new reasons
11/30/05   yll     Don't report connection timeout to SD if AT is in the
                   session negotiation.
11/22/05   pba     Fixed merge error, call hdrbccp_get_preferred_bc_channel 
                   with flow_id parameter set to NULL.
11/18/05   hal     Uses CM API instead of directly accessing cm_mc_rpt_free_q
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/21/05   mpa     If BCMCS flows requested, only sleep if whole CCsync rxed.
09/14/05   mpa     Added support for enh ConnectionRequest msg for BCMCS.
07/07/05   dna     Added support for Enhanced Idle State protocol
07/02/05   dna     Mainlined FEATURE_HDR_426MS_SLEEP
05/31/05   grl     Changed code to use HDRSRCHRF/TRM rather than SAM.
05/24/05   pba     Look at RUP state instead of RMAC to find if RTC is assigned
04/04/05   pba     Removed Lint errors
01/24/05   dna     Keep redials on HDR if 1x is hogging the RF (new CM reason)
12/13/04   hal     Changed dsm_offset_new_buffer to use hdrutil_new_buffer.
11/29/04   dna     Send all commands to CM even if there is no session.
                   Command LMAC back to idle if we abort connection setup
                   before we start probing.
11/09/04   dna     Allow for ConnectionRequest to be aborted.
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
08/05/04   mpa     Check for active broadcast before going to sleep.
04/13/04   dna     Wait to determine one way or the other if we need to enter
                   the suspend state before deciding to sleep.
01/08/04   mpa     Treat a ConnectionDeny msg with an Invalid reason as a
                   ConnectionDeny given for a general reason
10/23/03    km     Send response to CM only if we have valid Call Id
10/22/03    km     Added detail to F3 messaging and fixed callid dealloc
10/13/03   vas     Added comments.
10/03/03   mpa     Use new ds707 interface.
09/29/03   mpa     Added hdridle_is_sleeping()
09/12/03   mpa     Converted F3 messages to use new MSG2.0 HDR PROT SSID
09/11/03    km     Added support for new CM interface for array of callids
09/03/03    km     Enabled HDR sleep
08/08/03   vas     Deallocate Call IDs when call ends/fails
07/30/03    sq     Temporily disabled HDR sleep
07/21/03   mpa     Merges in 6500 baseline
06/26/03   mpa     Pass in boolean indicating whether conn setup failed because
                   of a ConnectionDeny from the AN in the connection failed ind
05/07/03   mpa     Updated fn name to cm_ds_hdr_has_data_session.
03/12/03    vr     hdridle_rpt_conn_setup_success will send a page to CM
                   if conn req due to HMP needing to send RTC messages
                   happens at the same time as conn setup due to AN page
02/28/03   mpa     Enable 426ms sleep for missed sync CC
02/14/03   mpa     Call hdridle_ready_for_long_sleep throughout.
01/17/03   mpa     Moved logging of successful connection attempt to hdrrup
12/10/02   dna     Added support for HDRIDLES_SUSPEND_STATE
12/02/02   vas     Added hdridle_ready_for_long_sleep()
11/19/02   mpa     Moved sleep dip switch to new interface
11/13/02   mpa     Added dip switch for 40s sleep
11/12/02   mpa     Do not call sd resel fn if we were previously sleeping
09/24/02   mpa     Moved 426MS_SLEEP featurization of CCMAC_SLEEP_CAPSULE_DONE
                   ind handling to only encompass the body
08/21/02   sh      Added cm_hdr_has_data_session
08/09/02   dna     Don't process link acquired if rmac is not active.
                   Race condition between RTCACK message and RUP connection
                   timeout would cause IDLE to send ALMP to connected while
                   RUP/HDRSRCH return to idle mode.
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
06/10/02   dna     Featurized FEATURE_HDR_426MS_SLEEP and
                   FEATURE_HDR_LONG_SLEEP separately from
                   FEATURE_HDR_PARKING.  Also renamed some variables.
05/06/02   dna     Tell others whether connection is opening to send signaling
04/22/02   sh      Added HDR parking support
03/29/02   dna     Changed timers to use REX callback instead of indications
                   Added support to sleep on 426ms or sleep until handoff
04/09/02   sh      Added change HDR and exit HDR for redirect and PRL not
                   preferred abort reason
03/29/02   dna     Only give SDSS_EVT_HDR_OPR_RESEL_OK on first sleep
03/18/02   dna     Add "sleep mask" for debugging
03/10/02   sh      Use CM_HDR prefix for CM reports from HDR
01/21/02   sh      Added update PCCC cmd and removed mode changed cmd and
                   IMSI changed ind
01/04/02   sh      Added SD_SS_ACT_PWR_SAVE in the sd action processing
12/11/01   sh      Discarded reports to CM if SN stream is not assigned
11/30/01   sh      Added access network authentication support
11/17/01   sh      Added 1xhdr data session handoff support
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/18/01   sh      Added protocol logging support
07/11/01   sh      Added more connect reasons and processing
06/19/01   sh      Added HMP and SLP queue checkings for reselect is allowed
06/06/01   dna     new param for hmp_send_msg()
05/31/01   SH      Added empty configuration checking in hdridle_configure
05/17/01   sh      Added preferred session seed generation
04/17/01   sh      Consolidated negotiation related event function calls
03/27/01   sh      Removed trans_id array in the control block
03/23/01   sh      Added the cnt argument for start conn setup timer
03/14/01   sh      Added Process Config request and Send Config response
03/10/01   sh      Removed the open conn state. Added support for PCCC
                   configuration negotiation
02/21/01   vas     Set the field "hdridle_cmd.sender" before processing the
                   open_connection command
02/14/01   dna     Hack to avoid sleep altogether until it is implemented.
                   Otherwise we stop demodulating the control channel.
02/09/01   dna     Added call to hdrhmp_tx_queue_is_empty() to tell if HMP
                   has something to send before going to sleep.
02/05/01   dna     Commented out code to set CC hash until sleep is supported
                   by the network.
01/13/01   dna     Merged back hdrmcdebug code that was lost in rev. 1.14.
01/09/01   kss     Changed hdramac_is_active() call to hdramac_is_in_attempt()
                   due to ACMAC interface changes.
09/22/00   SH      Used SD's connection deny enum
09/21/00   SH      Added slotted operation
05/26/00   SH      Created module.

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "rex.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "dsm.h"
#include "hdrmci.h"
#include "hdrcallid.h"
#include "hdridle.h"
#include "hdridlei.h"
#include "hdridles.h"
#include "hdridlec.h"
#include "hdrind.h"
#include "comdef.h"
#include "hdrrup.h"
#include "hdrhmp.h"
#include "hdrhai.h"
#include "cm.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "cmxll.h"
#include "hdrcon.h"
#include "hdrscp.h"
#include "hdrsmp.h"
#include "hdramp.h"
#include "hdrslp.h"
#include "hdrlmac.h"
#include "hdrutil.h"
#include "time_svc.h"
#include "sd.h"

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "hdrovhd.h"
#include "hdramac.h"
#include "hdrtrace.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif
#include "hdrrmac.h"
#include "hdrplog.h"
#include "hdrlog.h"
#ifdef FEATURE_HDR_HANDOFF
#include "hdrstream.h"
#endif
#ifdef FEATURE_HDR_BCMCS
#include "hdrbccp.h"
#include "hdrbcmac.h"
#endif /* FEATURE_HDR_BCMCS */

#include "ran.h"
#include "hdrsrchrf.h" 
  /* Keep redials on HDR if 1x is hogging the RF. This is temporary since
     only HDR search should call hdrsrchrf functions. */

#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk_opts.h"
#include "hdrqchatupk.h"
#endif /*FEATURE_HDR_QCHAT*/

#include "hdrsrchtypes.h" 
#include "nv_items.h"
  /* Added to access the HDR L1 debug NV item for override SCI */

#include "hdrscmdb.h"
#include "hdralmps.h"
#ifdef FEATURE_HDR_REVC
#include "hdrampati.h"
#include "hdrfmac.h"
#include "hdrscmfmac.h"
#include "hdrscmidle.h"
#include "hdrmc_v.h"
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
#include "wcn_coex_mgr.h"
#endif

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global defidleions for constants, macros, types,
variables

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Right shift timestamp 22 bits to convert it into an 80ms value */
#define HDRIDLE_TS_TO_80MS_CONVERSION  -22

#define HDRIDLE_PAGE_ID       0x00  /* message id for the page msg */
#define HDRIDLE_CONN_REQ_ID   0x01  /* message id for connection req msg */
#define HDRIDLE_CONN_DENY_ID  0x02  /* message id for connection deny msg */
#define HDRIDLE_QUICK_PAGE_ID 0x03

#define HDRIDLEC_QPI_NUM_GROUPS 4

/* priority of ConnectionRequest msg. the default value is 40 
 * To handle ConnectionRequest bundled with other priority 40 messages. 
 * we need to increase its priority to make sure it always goes out in the 
 * first Probe.
 * even this is not compliant with standard, it should be a saft change because
 * there is no AC_OR_RTC message with priority between 30 ~ 40. */
#define HDRIDLE_CONN_REQ_PRIORITY  35  

/* Deny Reason for the connection deny message */
#define HDRIDLE_CD_GENERAL                 0x00
#define HDRIDLE_CD_NETWORK_BUSY            0x01
#define HDRIDLE_CD_AUTH_OR_BILLING_FAILURE 0x02
#define HDRIDLE_CD_PREF_CHAN_NOT_AVAILABLE 0x03

#define HDRIDLE_HDR_SO                      CAI_SO_HDR_PKT_DATA  /* temporary */

/* Long sleep delay timer duration - 1 min */
#define HDRIDLE_LONG_SLEEP_DELAY_TIMEOUT  (60*1000)

/* Bit mask for deciding whether or not to go to sleep */

/* basic sleep conditions */
#define HDRIDLE_SLEEP_AMAC_IN_ATTEMPT   0
#define HDRIDLE_SLEEP_RMAC_ACTIVE       1
#define HDRIDLE_SLEEP_SLP_TX_QUEUE      2
#define HDRIDLE_SLEEP_HMP_TX_QUEUE      3
#define HDRIDLE_SLEEP_SUSPEND_TIMER     4
#ifdef FEATURE_IRAT_PILOT_MEAS
#define HDRIDLE_SLEEP_MEAS_IN_PROGRESS  4
#endif /* FEATURE_IRAT_PILOT_MEAS */
#define HDRIDLE_SLEEP_BIO_SWITCH        5

/* additional conditions for 426 ms */
#define HDRIDLE_SLEEP_SYNC_CAPSULE_RXED 6

/* additional conditions for full sleep */
#define HDRIDLE_SLEEP_SESSION_OPEN      7
#define HDRIDLE_SLEEP_OVHD_UPDATED      8
#define HDRIDLE_SLEEP_CAPSULE_DONE      9


/* additional condition for basic sleep */

/* This addresses a race condition when we enter IDLE while 1x has the RF.  
 * We must not sleep if we are about to receive HDRIND_LMAC_IDLE_SUSPEND.
 * LMAC will send "resume" immediately upon entering idle if HDR has the RF.
 */
#define HDRIDLE_SLEEP_LMAC_SUSPEND_OR_RESUME_RECEIVED     10

/* Do not sleep if broadcast is active */
#define HDRIDLE_BROADCAST_ACTIVE       11

/* If flows are requested (we are in dual slotted sleep), wait for the
   entire CCsync capsule to be received before we can go to sleep */
#define HDRIDLE_SLEEP_ENTIRE_CAPSULE_RXED_FOR_BCMCS 12

#ifdef FEATURE_HDR_TO_LTE
/* If OtherRATNeighborList is being transmitted in current SyncCC then
   entire CCsync capsule need to be received before we can go to sleep */
#define HDRIDLE_SLEEP_ENTIRE_CAPSULE_RXED_FOR_ORNL 12
#endif /* FEATURE_HDR_TO_LTE */

/* AST doesn't want to support and test SCI < 5, treat all cases of 
   SCI < 5 as non-slotted mode, except the Preferred Slotted Mode NV
   item is set */
#define HDRIDLE_SLEEP_SCI_SUPPORTED    13

/* additional condition for intra control channel packet sleep */
/* We can not assume that HDRIDLE_SLEEP_SYNC_CAPSULE_RXED == FALSE means 
 * next cc pkt is expected, becaue in the case of idle enters monitor state from 
 * sleep, we can't go to intra cc sleep (not ok to sleep in his case)*/
#define HDRIDLE_SLEEP_CONTINUED_SCC_PKT_EXPECTED  14

/* additional condition for intra control channel packet sleep 
   If QC is received even we haven't received sleep capsule we may still 
   go to FULL sleep in case of issue occurs in hdrsrch during intra scc sleep 
   operation 
*/
#define HDRIDLE_SLEEP_QC_RXED  15

/* -------------------------------------------------------------
   Basic sleep conditions:  0,1,2,3,4,5,10,11 and 13 '0010 1100 0011 1111'
   1.  We are not doing access probes
   2.  We are not attempting to open a connection
   3.  We are not waiting to send anything
   4.  We are not in the suspend period where we can't sleep
   5.  Sleep switch is disabled
   6.  We have determined LMAC does not want to suspend us
   7.  Broadcast is not active  
   8.  SCI is >= 5 or Preferred slotted mode NV item is set
   ------------------------------------------------------------- */
#define HDRIDLE_SLEEP_BASIC_CONDITIONS 0x00002C3F
#define HDRIDLE_SLEEP_BASIC_CONDITIONS_MET(sleep_mask) \
        (((sleep_mask) & HDRIDLE_SLEEP_BASIC_CONDITIONS) == \
         HDRIDLE_SLEEP_BASIC_CONDITIONS)

/* -------------------------------------------------------------
   Conditions for 426ms sleep: basic plus 6 '0010 1100 0111 1111'
   8.  We have received the entire synchronous capsule on the CC
       or we missed it (either nopreamble detected when it was
       expected or a bad CRC was received)
   ------------------------------------------------------------- */
#define HDRIDLE_SLEEP_426MS_CONDITIONS 0x00002C7F
#define HDRIDLE_SLEEP_426MS_CONDITIONS_MET(sleep_mask) \
        (((sleep_mask) & HDRIDLE_SLEEP_426MS_CONDITIONS) == \
          HDRIDLE_SLEEP_426MS_CONDITIONS)

/* -------------------------------------------------------------
   Full sleep conditions: basic plus 7, 8, 9 and 12 '0011 1111 1011 1111'
   9.  We have an HDR session
   10. We have current overhead information
   11. We have received the SleepStateCapsule on the CC
   12. Either there are no BCMCS flows requested or else there are
        and we have received the entire SynCC Capsule 
        (ensuring we get the BroadcastOverhead message)
   ------------------------------------------------------------- */
#define HDRIDLE_SLEEP_FULL_CONDITIONS  0x00003FBF
#define HDRIDLE_SLEEP_FULL_CONDITIONS_MET(sleep_mask) \
        (((sleep_mask) & HDRIDLE_SLEEP_FULL_CONDITIONS) == \
          HDRIDLE_SLEEP_FULL_CONDITIONS)

/* -------------------------------------------------------------
   Basic conditions for intra cc sleep: basic sleep plus 14 '0110 1100 0011 1111'
   14.  We have not received the entire synchronous capsule on the CC
        but we can go to sleep(sci 3 or 4) before the next cc packet come 
   ------------------------------------------------------------- */
#define HDRIDLE_SLEEP_BASIC_INTRA_SCC_CONDITIONS 0x00006C3F
#define HDRIDLE_SLEEP_BASIC_INTRA_SCC_CONDITIONS_MET(sleep_mask) \
        (((sleep_mask) & HDRIDLE_SLEEP_BASIC_INTRA_SCC_CONDITIONS) == \
          HDRIDLE_SLEEP_BASIC_INTRA_SCC_CONDITIONS)

/* -------------------------------------------------------------
   AT can go to 426ms sleep even it hasn't received entire SCC
   in the case of overhead is upated and received cc packet with 
   sleep_done set to 1 and received BOM
   basic sleep pluc 8, 9,12, 14
   '0111 1111 0011 1111'
   ------------------------------------------------------------- */
#define HDRIDLE_SLEEP_426MS_PARTIAL_SCC_RXED_CONDITIONS 0x00007F3F
#define HDRIDLE_SLEEP_426MS_PARTIAL_SCC_RXED_CONDITIONS_MET(sleep_mask) \
        (((sleep_mask) & HDRIDLE_SLEEP_426MS_PARTIAL_SCC_RXED_CONDITIONS) == \
          HDRIDLE_SLEEP_426MS_PARTIAL_SCC_RXED_CONDITIONS)

/* -------------------------------------------------------------
   AT may go to FULL sleep even it hasn't received entire sleep capsule
   This is only used as fallback sleep cycle in case of intra scc operation fails.
   Full sleep conditions minus 10 plus 15,16 '1111 1101 1011 1111'
   10. We have received the SleepStateCapsule on the CC
   15. Next CC packet is expected
   16. QC is received 
   ------------------------------------------------------------- */
#define HDRIDLE_SLEEP_FULL_PARTIAL_SCC_RXED_CONDITIONS 0x0000FDBF
#define HDRIDLE_SLEEP_FULL_PARTIAL_SCC_RXED_CONDITIONS_MET(sleep_mask) \
        (((sleep_mask) & HDRIDLE_SLEEP_FULL_PARTIAL_SCC_RXED_CONDITIONS) == \
          HDRIDLE_SLEEP_FULL_PARTIAL_SCC_RXED_CONDITIONS)

/* Internal message structure*/

#define HDRIDLE_QUICKPAGE_QPI_BIT_MASK 0x0000000000000001

#ifdef FEATURE_HDR_REVC
#define HDRIDLE_MULTI_PAGE_SUPPORTED_ATTRIB_ID 0xFB

/* Constants for unpacking MultiATIPage message */
#define HDRIDLE_MULTI_PAGE_ID                  4

#define HDRIDLE_MSG_ID_SIZE                    8

#define HDRIDLE_MULTI_PAGE_NUM_UATI_MSB_SIZE   5

#define HDRIDLE_MULTI_PAGE_UATI_MSB_LEN_SIZE   5

#define HDRIDLE_MULTI_PAGE_NUM_PAGES_SIZE      6

#define HDRIDLE_MULTI_PAGE_RESP_APERSIST_INCLUDED_SIZE  1

#define HDRIDLE_MULTI_PAGE_RESP_APERSIST_SIZE  6

/* Marco to unpack bits and advance position */
#define HDRIDLE_UNPACK32_ADV(ptr, val, offset, len)  HDRBIT_UNPACK32(ptr, val, offset, len), offset += len
#endif /* FEATURE_HDR_REVC */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* CONNECTION DENY MESSAGE(see HAI Sec 6.4.6.3)*/
typedef PACK( struct )
{
  byte MessageID[8];         /* Message ID */
  byte TransactionID[8]; /* Transaction ID */
  byte DenyReason[4];     /* Deny Reason */
  byte Reserved[4];       /* Reserved */
} hdridle_connection_deny_type;

/* SCI value at which 1x acquisition throttling begins */
#define HDRIDLE_LOW_SCI 6

/* Number of SCI values supported by the AT */
#define HDRIDLE_NUM_SCIS 13

/* Max SCI Value supported by the AT */
#define HDRIDLE_MAX_SCI_VALUE 12

#ifdef FEATURE_HDR_DDARF

/* This static table specifies the maximum times IDP would allow
   the searcher to go back to sleep after losing acquisition on 
   wakeup before declaring network lost. Table is index based on  
   SCI values */
static const uint8 hdridle_reacq_retry_attempts[HDRIDLE_NUM_SCIS] = 
                     {
                       3, /* SCI 0 */ 
                       3, /* SCI 1 */ 
                       3, /* SCI 2 */ 
                       3, /* SCI 3 */ 
                       3, /* SCI 4 */ 
                       3, /* SCI 5 */ 
                       3, /* SCI 6 */ 
                       3, /* SCI 7 */ 
                       3, /* SCI 8 */
                       2, /* SCI 9 */
                       1, /* SCI 10 */ 
                       1, /* SCI 11 */
                       1, /* SCI 12 */
                     };
#endif /* FEATURE_HDR_DDARF */

/* Static table of actual sleep values */
static const char *hdridle_sleep_time_strings[HDRIDLE_NUM_SCIS] =
{
  "6.67ms", /* SCI 0 */
  "13.33ms", /* SCI 1 */
  "26.67ms", /* SCI 2 */
  "53.33ms", /* SCI 3 */
  "106.67ms", /* SCI 4 */
  "213.33ms", /* SCI 5 */
  "426.67ms", /* SCI 6 */
  "853.33ms", /* SCI 7 */
  "2.56s", /* SCI 8 */
  "5.12s", /* SCI 9 */
  "10.24s", /* SCI 10 */
  "20.48s", /* SCI 11 */
  "40.96s", /* SCI 12 */
};

/*===========================================================================

          MACROS FOR EXTRACTING CONN DENY MESSAGE FIELDS

===========================================================================*/


#define HDRIDLE_GET_CD_TRANS_ID(src_ptr, des_ptr) \
               HDRBIT_UNPACK8(src_ptr, des_ptr,    \
               (uint16) FPOS(hdridle_connection_deny_type, TransactionID), \
               (uint16) FSIZ(hdridle_connection_deny_type, TransactionID))

#define HDRIDLE_GET_CD_DENY_REASON(src_ptr, des_ptr) \
                HDRBIT_UNPACK8(src_ptr, des_ptr,    \
                (uint16) FPOS(hdridle_connection_deny_type, DenyReason), \
                (uint16) FSIZ(hdridle_connection_deny_type, DenyReason))


#define HDRIDLE_GET_PAGE_RESP_APERSISTENCE_INCLUDED( msg_ptr, attrib_val ) \
                HDRBIT_UNPACK8( item_ptr, attrib_val, 8, 1 )

#define HDRIDLE_GET_PAGE_RESP_APERSISTENCE( msg_ptr, attrib_val ) \
                HDRBIT_UNPACK8( item_ptr, attrib_val, 9, 6 );

#define HDRIDLE_MSG_ID_SIZE 8
#define HDRIDLE_QUICKPAGE_CONFIG_CHANGE_SIZE 2
#define HDRIDLE_QUICKPAGE_QPI_PER_GRP_COUNT_SIZE 6

#define HDRIDLE_GET_QP_CONFIG_CHANGE( src_ptr, des_ptr, offset ) \
               HDRBIT_UNPACK8(src_ptr, des_ptr, offset, \
                              HDRIDLE_QUICKPAGE_CONFIG_CHANGE_SIZE)

#define HDRIDLE_GET_QP_QPI_PER_GROUP_COUNT( src_ptr, des_ptr, offset ) \
               HDRBIT_UNPACK8(src_ptr, des_ptr, offset,  \
                              HDRIDLE_QUICKPAGE_QPI_PER_GRP_COUNT_SIZE)

#define HDRIDLE_GET_QP_QPI( src_ptr, des_ptr, offset, len ) \
               HDRBIT_UNPACK64(src_ptr, des_ptr, offset, len)

/*===========================================================================

          MACROS FOR PACKING CONNECTION REQUEST MESSAGE FIELDS

===========================================================================*/


#define HDRIDLE_PACK_CON_REQ_MSG_ID( msg_ptr, conn_req ) \
          HDRBIT_PACK8(msg_ptr, conn_req.msg_id, 0, 8)

#define HDRIDLE_PACK_CON_REQ_TRANS_ID( msg_ptr, conn_req ) \
          HDRBIT_PACK8(msg_ptr, conn_req.trans_id, 8, 8)

#define HDRIDLE_PACK_CON_REQ_REQ_REASON( msg_ptr, conn_req ) \
          HDRBIT_PACK8(msg_ptr, conn_req.req_reason, 16, 4)

#define HDRIDLE_PACK_CON_REQ_PREF_CHAN_COUNT( msg_ptr, conn_req ) \
          HDRBIT_PACK8(msg_ptr, conn_req.preferred_channel_cnt, 20, 5)

#define HDRIDLE_PACK_CON_REQ_EMERGENCY_INDICATION( msg_ptr, val, offset, len ) \
          HDRBIT_PACK8(msg_ptr, val, (uint16)offset, len)


#define HDRIDLE_CON_REQ_PREF_CHANNEL_OFFSET 25
#define HDRIDLE_CON_REQ_PREF_CHANNEL_SIZE 24
#define HDRIDLE_CON_REQ_EMERGENCY_INDICATION_SIZE 1

/* Channel Record */
#define HDRIDLE_PACK_CON_REQ_SYSTEM_TYPE( msg_ptr, conn_req, i, offset )  \
          HDRBIT_PACK8( msg_ptr, (uint8)conn_req.preferred_channel[i].mode, \
                          (uint16)(offset),  8 )

#define HDRIDLE_PACK_CON_REQ_BAND_CLASS( msg_ptr, conn_req, i, offset ) \
          HDRBIT_PACK8( msg_ptr, \
                         (uint8)(conn_req.preferred_channel[i].channel.band), \
                         (uint16)(offset + 8), 5 )

#define HDRIDLE_PACK_CON_REQ_CHAN_NUMBER( msg_ptr, conn_req, i, offset )  \
          HDRBIT_PACK16( msg_ptr, \
            (uint16)(conn_req.preferred_channel[i].channel.chan_num), \
            (uint16)(offset + 13), 11 )

/* This command function is really only for the 
 * hdridle_conn_req_msg_tx_outcome_cb(), so it is kept local.
 */
LOCAL void hdridle_finish_abort_con_setup
(
  hdrhai_protocol_name_enum_type sender
);

LOCAL void hdridle_process_conn_setup_tca_failed(void);

LOCAL void hdridle_connect_setup_fail_cleanup
(
  const boolean is_conn_deny_failure
);

hdridle_type hdridle;


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_GET_OVERRIDE_SCI

DESCRIPTION
  This function reads the HDR L1 debug mask NV item and extracts the 
  FORCED SCI 4 bit value.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint32 - The forced SCI value read from the HDR L1 debug NV item

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 hdridle_get_override_sci( void )
{
  nv_hdrl1_dbg_msk_type debug_mask; /* HDR L1 debug mask field */
  uint32 sci = 0x00;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read HDR L1 debug mask */
  if( hdrutil_read_nv_item( NV_HDRL1_DBG_MSK_I, 
      (nv_item_type *)&debug_mask ) != NV_DONE_S )
  {
    ERR("Read NV_HDRL1_DBG_MSK_I failed ...", 0, 0, 0);
  }
  else
  {
    /* Bits [28:31] from HDR L1 debug mask to indicate what the
       SCI is forced to. */
    sci = (( ((uint32) debug_mask.hdrsrch_dbg_msk) >> 28 ) & 0xF);
  }

  return sci;
} /* hdridle_get_override_sci() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_CONN_REQ_TRANS_ID_MATCHED

DESCRIPTION
  This function processes the transmission outcome of the connection request
  message.

DEPENDENCIES
  None

PARAMETERS
  data    - user data which contains trans ID

RETURN VALUE
  Ture - transaction ID matched for the Connection Request message

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdridle_is_conn_req_trans_id_matched
(
  void  *data
)
{
  if( hdridle.conn_req_msg.trans_id ==  (byte)((dword) data) )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }

} /* hdridle_is_conn_req_trans_id_matched */

 /* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CONN_REQ_MSG_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_conn_req_msg_tx_outcome_cb
(
  void                 *data,
  hdrerrno_enum_type    status
)
{
  hdridles_state_enum_type cur_state= hdridles_get_state();
                           /* current idle state */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (!hdridle.conn_req_msg.awaiting_slp_callback)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "Ignore creq tx outcome cb s:%d, d:%d",
                  status, (uint32)data);
    return;
  }


  if(!hdridle_is_conn_req_trans_id_matched(data))
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                   "creq tx outcome cb trans_id mismatch s:%d, d:%d", 
                   status, (uint32)data);
    return;
  }

  /* This flag is set to TRUE when the message is sent, and because it is
   * guaranteed that this callback is called regardless of the outcome of
   * the message, the flag will not get out of sync.
   */
  hdridle.conn_req_msg.awaiting_slp_callback = FALSE;

  switch(status)
  {
    case E_SUCCESS:
      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLE_CONN_REQ_ID, cur_state); /*lint !e506 */

      if(cur_state == HDRIDLES_CONN_SETUP_STATE )
      {
#ifdef FEATURE_LTE_TO_HDR_OH
        if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
        {
          /* In active handoff connection set up timer (2sec) is started after 
             CR is sent */
        }
        else
#endif /* FEATURE_LTE_TO_HDR_OH */
        {
          hdridle_start_conn_setup_timer (HDRIDLE_CONN_SETUP_TIMER1_MSECS);
        }
      }
      else
      {
        /* ignore the callback if it is not received in the right state */     
        HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                       "Ignore creq tx outcome cb (wrong state: %d)", 
                       cur_state);
      }
      break;

    case E_ABORTED:
      HDRTRACE_TXMSG_ABORTED(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLE_CONN_REQ_ID, cur_state); /*lint !e506 */

#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
      {
        /* In active handoff, we receive E_ABORTED only because TCA arrives
         * earlier than L2 ACK. */
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      /* If we still want to open a connection, re-queue the message.
       * Otherwise we need to clean up and return to monitor state.
       */
      if( (cur_state == HDRIDLES_CONN_SETUP_STATE) ||
          (cur_state == HDRIDLES_CONN_SETUP_SUSPEND_STATE)
        )
      {
        if (hdridle.connect_reason == HDRIDLE_CR_ABORT)
        {
          /* We still start the connection setup timer here just in case 
           * the abort doesn't work for any reason.
           */
          hdridle_start_conn_setup_timer (HDRIDLE_CONN_SETUP_TIMER1_MSECS);
          hdridle_finish_abort_con_setup( HDRHAI_IDLE_STATE_PROTOCOL );
        }
        else
        {
          hdrplog_end_conn_attempt(HDRPLOG_CONN_ATT_ACCESS_HO);
          /* Send the ConnectionRequest again. */
          hdridles_change_csetup_substate(HDRIDLES_CSETUP_INVALID);
          hdridle_send_conn_req_msg();
        }
      }
      break;

    case E_FAILURE:
      HDRTRACE_TXMSG_FAILED(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLE_CONN_REQ_ID, cur_state);

#ifdef FEATURE_LTE_TO_HDR_OH
  
      if( hdrsap_lte_to_hdr_active_ho_is_in_progress() ||
          (hdrsap_get_current_tunnel_mode_value(HDRHAI_IDLE_STATE_PROTOCOL) == HDRHAI_TUNNEL_MODE_NO_SRV))
      {
          /* Connection Request tranmission failed during active ho. However, if LTE OOS
                      the tunnel mode value becomes HDRHAI_TUNNEL_MODE_NO_SRV. In that case also cleanup is needed */

         HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Tunnel mode %d during conn req failure",
                                        hdrsap_get_current_tunnel_mode_value(HDRHAI_IDLE_STATE_PROTOCOL));
         if ( rex_get_timer( &hdridle.tunnel_conn_timer ) != 0 )
         {
             HDR_MSG_PROT( MSG_LEGACY_HIGH, "Stop Tunnel Connection timer" );
             ( void ) rex_clr_timer( &hdridle.tunnel_conn_timer );
         }

         hdridle_connect_setup_fail_cleanup(FALSE);

         if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
         {
             /* In the case of HDRHAI_TUNNEL_MODE_NO_SRV (LTE OOS), setting tunnel mode ENABLED explicitly is NOT needed 
                           because, it is going to be set when LTE again comes back to service */
               hdrsap_set_tunnel_mode(HDRHAI_TUNNEL_MODE_ENABLED);
         }
         hdridles_change_state(HDRIDLES_INACTIVE_STATE);
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
      break;

    default:
      HDRTRACE_TXMSG_FAILED(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLE_CONN_REQ_ID, cur_state); /*lint !e506 */
      /* if transmission failed then ALMP will deactivate IDLE so do
         nothing here except to log the failure
      */
      break;
  }
} /* hdridle_conn_req_msg_tx_outcome_cb() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SUSPEND_TIMER_IS_INACTIVE

DESCRIPTION
  This function return TRUE if the suspend timer is inactive

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - Timer is inactive

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdridle_suspend_timer_is_inactive ( void )
{
  if( rex_get_timer( &hdridle.suspend_timer) !=0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_LOW, "suspend timer is active" );
    return FALSE;
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_LOW, "suspend timer is not active" );
    return TRUE;
  }
} /* hdridle_suspend_timer_is_inactive() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_SD_ACT

DESCRIPTION
     This function process the SD's returned action

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void  hdridle_process_sd_act
(
  sd_ss_act_e_type act
)
{
  switch(act)
  {
    case SD_SS_ACT_ACQ_HDR:
      /* change state then give indication
         Do not change this sequence since it is tied to the ALMP
         implementation
      */
      hdridles_change_state(HDRIDLES_INACTIVE_STATE);
      hdrind_give_ind(HDRIND_IDLE_CHANGE_HDR_SYSTEMS, NULL);
      break;

    case SD_SS_ACT_CONTINUE:
      break;

    default:
      /* Any other reason, exit HDR mode */
      hdridles_change_state(HDRIDLES_INACTIVE_STATE);
      hdrind_give_ind(HDRIND_IDLE_EXIT_HDR_MODE, NULL);
      break;
  }

} /* hdridle_process_sd_act() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_RPT_TO_CM

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

PARAMETERS

cmd_ptr - pointer to command to send to cm

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdridle_send_rpt_to_cm
(
  cm_mc_rpt_type *cmd_ptr
    /* pointer to command to send to cm */
)
{
  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Send CM rpt %d",cmd_ptr->hdr.cmd );
  cm_mc_rpt(cmd_ptr);

} /* mcc_cm_mc_rpt() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_REDIRECT_TO_CM

DESCRIPTION
  This function sends a redirect report to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_send_redirect_to_cm
(
  hdridle_conn_fail_enum_type reason
)
{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to mc buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrcallid_get_num_active_call_ids() > 0)
  {

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->redirect.hdr.cmd = CM_HDR_REDIRECT_F;
      cmd_ptr->redirect.num_call_ids =
      hdrcallid_get_all_call_ids ( cmd_ptr->redirect.call_ids );

      switch (reason)
      {
      case HDRIDLE_CF_REDIRECT_CHG_HDR:
        cmd_ptr->redirect.code = CM_REDIRECT_CHG_HDR;
        break;

      case HDRIDLE_CF_REDIRECT_EXIT_HDR:
      default:
        cmd_ptr->redirect.code = CM_REDIRECT_EXIT_HDR;
        break;
      }
      HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "Sent CM_HDR_REDIRECT_F for %d CallIds,code=%d",
                      cmd_ptr->redirect.num_call_ids, cmd_ptr->redirect.code);
      hdridle_send_rpt_to_cm( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

    hdrcallid_deallocate_all_call_ids ();
  }

}/* hdridle_send_redirect_to_cm() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_CALL_FADE_TO_CM

DESCRIPTION
  This function sends a call fade report to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_send_call_fade_to_cm(void)

{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (hdrcallid_get_num_active_call_ids() > 0)
  {

    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->fade.hdr.cmd = CM_HDR_CALL_FADE_F;
      cmd_ptr->fade.num_call_ids = hdrcallid_get_all_call_ids ( cmd_ptr->fade.call_ids );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "Sent CM_HDR_CALL_FADE_F for %d callids",
                      cmd_ptr->fade.num_call_ids);
      hdridle_send_rpt_to_cm( cmd_ptr );

    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

    hdrcallid_deallocate_all_call_ids();
  }


}/* hdridle_send_call_fade_to_cm() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_DEACTIVATE_ACK_TO_CM

DESCRIPTION
  This function sends a deactivate ack to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_send_deactivate_ack_to_cm(void)

{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrcallid_get_num_active_call_ids() > 0)
  {

    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->fade.hdr.cmd = CM_HDR_DEACTIVATE_ACK_F;
      cmd_ptr->fade.num_call_ids = hdrcallid_get_all_call_ids ( cmd_ptr->fade.call_ids );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "Sent CM_HDR_DEACTIVATE_ACK_F for %d callids",
                      cmd_ptr->fade.num_call_ids);
      hdridle_send_rpt_to_cm( cmd_ptr );
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

    hdrcallid_deallocate_all_call_ids();
  }

}/* hdridle_send_deactivate_ack_to_cm() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_CONN_DENY_TO_CM

DESCRIPTION
  This function sends a connection deny report to CM.

DEPENDENCIES
  None.

PARAMETERS
  reason - reason for denying the connection

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_send_conn_deny_to_cm
(
  hdridle_conn_fail_enum_type reason
)

{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to mc buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrcallid_get_num_active_call_ids() > 0)
  {

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->conn_deny.hdr.cmd = CM_HDR_CONNECTION_DENY_F;
      cmd_ptr->conn_deny.num_call_ids =
      hdrcallid_get_all_call_ids ( cmd_ptr->conn_deny.call_ids );

      switch (reason)
      {
      case HDRIDLE_CF_CD_GENERAL:
        cmd_ptr->conn_deny.code = CM_CD_GENERAL;
        break;

      case HDRIDLE_CF_CD_NET_BUSY:
        cmd_ptr->conn_deny.code = CM_CD_NETWORK_BUSY;
        break;

      case HDRIDLE_CF_CD_PREF_CHAN_NOT_AVAIL:
#ifdef FEATURE_EHRPD_VOIP_SR
        cmd_ptr->conn_deny.code = CM_CD_PREF_CH_NOT_AVAIL;
#else
        cmd_ptr->conn_deny.code = CM_CD_GENERAL;
#endif /* FAETURE_EHRPD_VOIP_SR */
        break;

      case HDRIDLE_CF_CD_AUTH_BILL_FAIL:
      default:
        cmd_ptr->conn_deny.code = CM_CD_AUTH_BILLING_FAIL;
        break;
      }

      HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "Sent CM_HDR_CONNECTION_DENY_F for %d CallIds,code=%d",
                      cmd_ptr->conn_deny.num_call_ids, cmd_ptr->conn_deny.code );
      hdridle_send_rpt_to_cm( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

    hdrcallid_deallocate_all_call_ids ();
  }


} /* hdridle_send_conn_deny_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_CALL_ORIG_FAIL_TO_CM

DESCRIPTION
  This function sends a orig fail report to CM.

DEPENDENCIES
  None.

PARAMETERS
 reason - reason for the orig failure


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_send_call_orig_fail_to_cm
(
  hdridle_conn_fail_enum_type reason
)

{

  cm_mc_rpt_type *cmd_ptr;    /* Pointer to mc buffer */

  hdrcallid_all_callids   call_ids;
  byte                    num_call_ids;
  int                     i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get Call ids */
  num_call_ids = hdrcallid_get_all_call_ids ( call_ids );

  /* Send CM report for all call ids */
  for (i = 0; i < num_call_ids; i++)
  {

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->orig_fail.hdr.cmd = CM_HDR_CALL_ORIG_FAIL_F;
      cmd_ptr->orig_fail.call_id = call_ids[i];

      switch (reason)
      {
        case HDRIDLE_CF_TCA_TIMEOUT:
#ifdef FEATURE_EHRPD_VOIP_SR
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_TCA_TIMEOUT;
#else
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_TIMEOUT;
#endif /* FAETURE_EHRPD_VOIP_SR */
          break;

        case HDRIDLE_CF_RTCACK_TIMEOUT:
#ifdef FEATURE_EHRPD_VOIP_SR
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_RTCACK_TIMEOUT;
#else
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_TIMEOUT;
#endif /* FAETURE_EHRPD_VOIP_SR */
          break;

        case  HDRIDLE_CF_MAX_ACC_PROBE:
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_ACC_FAIL;
          break;

        case  HDRIDLE_CF_SYS_LOST:
          /* If we have the RF lock when we lose the system we report
           * CM_HDR_CALL_FADE_F.  If we are reporting CM_HDR_CALL_ORIG_FAIL_F
           * it is because we didn't get the RF lock when we needed it.
           */
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED;
          break;

        case  HDRIDLE_CF_ORIG_ERR_ABORT:
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_ABORT;
          break;

#ifdef FEATURE_SVDO
        case  HDRIDLE_CF_TCA_FAILED:
          /* Silent redial shouldn't happen with this error code*/
          cmd_ptr->orig_fail.code = CM_CALL_ORIG_ERR_NO_SVDO;
          break;
#endif  /* FEATURE_SVDO */

        default:
          break;
      }

      HDR_MSG_PROT_2 (MSG_LEGACY_MED,
                      "Sent CM_HDR_CALL_ORIG_FAIL_F for callid=%d,code=%d",
                      call_ids[i], cmd_ptr->orig_fail.code);
      hdridle_send_rpt_to_cm( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }

  hdrcallid_deallocate_all_call_ids();

} /* hdridle_send_call_orig_fail_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_INCOMING_CALL_TO_CM

DESCRIPTION
  This function sends an Incoming Call notification to the Call Manager

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The alert ans bypass field is always set to TURE

===========================================================================*/

LOCAL void hdridle_send_incoming_call_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to CM command buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->incoming_call.hdr.cmd = CM_HDR_INCOMING_CALL_F;
    cmd_ptr->incoming_call.so =  HDRIDLE_HDR_SO;
    cmd_ptr->incoming_call.call_id = cm_call_id_allocate();

    if (cmd_ptr->incoming_call.call_id == CM_CALL_ID_INVALID)
    {
      /* There is no call_id available. Something must
      ** be wrong, and we cannot do anything about it.
      ** Just log the error fatal
      **
      */
      ERR_FATAL("No Call ID", 0, 0, 0);
    }
    else
    {
      hdrcallid_set_cur_call_id(cmd_ptr->incoming_call.call_id);
    }

#ifdef FEATURE_IS95B_ALERT_ANS_BYPASS
    cmd_ptr->incoming_call.alert_ans_bypass = TRUE ;
#endif /* FEATURE_IS95B_ALERT_ANS_BYPASS */

    hdridle_send_rpt_to_cm( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdridle_send_incoming_call_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_CALL_CONNECTED_TO_CM

DESCRIPTION
  This function sends an connected notification to the Call Manager

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The alert ans bypass field is always set to TURE

===========================================================================*/
void hdridle_send_call_connected_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;
  /* Pointer to CM command buffer */

  hdrcallid_all_callids   call_ids;
  byte                    num_call_ids;
  int                     i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get Call ids */
  num_call_ids = hdrcallid_get_all_call_ids ( call_ids );

  /* Send CM report for all call ids */
  for (i = 0; i < num_call_ids; i++)
  {

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->connect.hdr.cmd = CM_HDR_CALL_CONNECTED_F;
      cmd_ptr->connect.so      = HDRIDLE_HDR_SO;
      cmd_ptr->connect.call_id = call_ids [i];

      HDR_MSG_PROT_1 (MSG_LEGACY_MED,
                      "Sent CM_HDR_CALL_CONNECTED_F for callid=%d",
                      call_ids[i] );
      hdridle_send_rpt_to_cm( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }

} /* hdridle_send_call_connected_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SEND_PRL_NOT_PREFERRED_TO_CM

DESCRIPTION
  This function sends an PRL not preferred report to the Call Manager

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdridle_send_prl_not_preferred_to_cm
(
  hdridle_conn_fail_enum_type reason
)
{
  cm_mc_rpt_type *cmd_ptr;
  /* Pointer to CM command buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrcallid_get_num_active_call_ids() > 0)
  {

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->prl_not_preferred.hdr.cmd = CM_HDR_PRL_NOT_PREFERRED_F;
      cmd_ptr->prl_not_preferred.num_call_ids =
      hdrcallid_get_all_call_ids ( cmd_ptr->prl_not_preferred.call_ids );

      switch (reason)
      {
      case HDRIDLE_CF_PRL_NOT_PREF_CHG_HDR:
        cmd_ptr->prl_not_preferred.code = CM_PRL_NOT_PREF_CHG_HDR;
        break;

      case HDRIDLE_CF_PRL_NOT_PREF_EXIT_HDR:
      default:
        cmd_ptr->prl_not_preferred.code = CM_PRL_NOT_PREF_EXIT_HDR;
        break;
      }

      HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "Sent CM_HDR_PRL_NOT_PREFERRED_F for %d CallIds,code=%d",
                      cmd_ptr->prl_not_preferred.num_call_ids, cmd_ptr->prl_not_preferred.code);
      hdridle_send_rpt_to_cm( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }


    hdrcallid_deallocate_all_call_ids ();
  }


} /* hdridle_send_prl_not_preferred_to_cm() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RPT_CONN_SETUP_FAIL_TO_CM

DESCRIPTION
  This function reports a connectrion setup failure to CM based
  on the specified failed reason.

DEPENDENCIES
  None.

PARAMETERS
 reason - reason for the connection setup failure

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_rpt_conn_setup_fail_to_cm
(
  hdridle_conn_fail_enum_type reason
)
{

  switch(reason)
  {
    case  HDRIDLE_CF_SYS_LOST:

      /* TODO: This should be changed so that the protocol layer doesn't
         call a hdrsrchrf function.  This coupling existed prior to SHDR. */
      if ( hdrsrchrf_has_lock( HDRSRCHRF_PRI_CHAIN ) == TRUE )
      {
        hdridle_send_call_fade_to_cm();
      }
      else
      {
        /* If the system was lost while we do not have the RF lock, then
         * 1x was probably holding the RF lock for too long.  Report this
         * as a separate failure reason to avoid originating on 1x when
         * HDR is probably still our best option.
         */
        hdridle_send_call_orig_fail_to_cm(reason);
      }
      break;

    case HDRIDLE_CF_TCA_TIMEOUT:
    case HDRIDLE_CF_RTCACK_TIMEOUT:
    case HDRIDLE_CF_MAX_ACC_PROBE:

      hdridle_send_call_orig_fail_to_cm(reason);
      break;

    case HDRIDLE_CF_CD_GENERAL:
    case HDRIDLE_CF_CD_NET_BUSY:
    case HDRIDLE_CF_CD_AUTH_BILL_FAIL:
    case HDRIDLE_CF_CD_PREF_CHAN_NOT_AVAIL:

      hdridle_send_conn_deny_to_cm(reason);
      break;

    case  HDRIDLE_CF_REDIRECT_CHG_HDR:
    case  HDRIDLE_CF_REDIRECT_EXIT_HDR:

      hdridle_send_redirect_to_cm(reason);
      break;

    case HDRIDLE_CF_PRL_NOT_PREF_CHG_HDR:
    case HDRIDLE_CF_PRL_NOT_PREF_EXIT_HDR:

      hdridle_send_prl_not_preferred_to_cm(reason);
      break;

    default:

      /* do nothing */
      break;
  }

} /* hdridle_rpt_conn_setup_fail_to_cm() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RPT_CONN_SETUP_FAIL_TO_SD

DESCRIPTION
  This function reports a connection setup failure to SD based
  on the specified failed reason.

DEPENDENCIES
  None.

PARAMETERS
 reason - reason for the connection setup failure

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL sd_ss_act_e_type hdridle_rpt_conn_setup_fail_to_sd
(
  hdridle_conn_fail_enum_type reason
)
{

  sd_ss_act_e_type  ret_val = SD_SS_ACT_CONTINUE;
                              /* action returned by SD */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
  {
    return ret_val;
  }

  switch(reason)
  {

   case HDRIDLE_CF_TCA_TIMEOUT:
   case HDRIDLE_CF_RTCACK_TIMEOUT:
      ret_val = sd_ss_ind_hdr_opr_acc_term(SD_SS_ACC_TERM_TIMEOUT, NULL);
      break;

    case HDRIDLE_CF_CD_GENERAL:
    case HDRIDLE_CF_CD_PREF_CHAN_NOT_AVAIL:
      ret_val = sd_ss_ind_hdr_opr_acc_term(SD_SS_ACC_TERM_CON_DEN, NULL);
      break;

    case HDRIDLE_CF_CD_NET_BUSY:
      ret_val = sd_ss_ind_hdr_opr_acc_term(SD_SS_ACC_TERM_NET_BUSY, NULL);
      break;

    case HDRIDLE_CF_CD_AUTH_BILL_FAIL:
      ret_val = sd_ss_ind_hdr_opr_acc_term(SD_SS_ACC_TERM_AUTH_FAIL, NULL);
      break;

    case HDRIDLE_CF_SYS_LOST:
      /* For FMAC Supervision, ALMP calls SD action. For others,
       * we shouldn't call SD action here otherwise it will end
       * up with an ERR_FATAL. */
      break;

    default:
       /* do nothing */
       break;
  }

  if(ret_val != SD_SS_ACT_CONTINUE)
  {
    hdridle_process_sd_act(ret_val);
  }

  return ret_val;
} /* hdridle_rpt_conn_setup_fail_to_sd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RPT_CONN_SETUP_FAIL

DESCRIPTION
  This function reports a conntion setup failure to CM and SD based
  on the specified failed reason.

DEPENDENCIES
  None.

PARAMETERS
 reason - reason for the connection setup failure

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL sd_ss_act_e_type hdridle_rpt_conn_setup_fail
(
  hdridle_conn_fail_enum_type reason
)
{
  HDRTRACE_IDLE_CONNECTION_SETUP_FAIL_REASON( reason );

  if( hdridle.connect_reason & ~HDRIDLE_CR_AN )
  {
    hdridle_rpt_conn_setup_fail_to_cm(reason);
  }

  return hdridle_rpt_conn_setup_fail_to_sd(reason);
} /* hdridle_rpt_conn_setup_fail() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RPT_CONN_SETUP_SUCCESS

DESCRIPTION
  This function reports a successful connection setup

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdridle_rpt_conn_setup_success (void)
{

  if( (hdridle.connect_reason & HDRIDLE_CR_AN) &&
      ((hdridle.connect_reason & ~(HDRIDLE_CR_AN|HDRIDLE_CR_MSG)) == 0))
  {
    hdridle_send_incoming_call_to_cm();
  }

  hdridle_send_call_connected_to_cm();

} /* hdridle_rpt_conn_setup_success() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CONNECT_SETUP_FAIL_CLEANUP

DESCRIPTION
  This function performs the cleanup operation for connection setup
  failure

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  TRUE  - connection setup failed because we received a ConnectionDeny msg
          from the AN
  FALSE - connection failed for any other reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_connect_setup_fail_cleanup(const boolean is_conn_deny_failure)
{
  hdrind_idle_conn_fail_info_type conn_fail_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  conn_fail_info.failure_reason = 0;
  /*KW fix:Initiialize the variable so that Oring can happen successfully later*/
  
  /* close route update */
  hdrrup_close(HDRHAI_IDLE_STATE_PROTOCOL);

  if (is_conn_deny_failure)
  {
    conn_fail_info.failure_reason = HDRIDLE_CONN_FAIL_CONN_DENY_MASK;
  }

  if (hdridle_get_cr_ror_progress())
  {
    conn_fail_info.failure_reason |= HDRIDLE_CONN_FAIL_CR_ROR_MASK;
  }

  conn_fail_info.backoff_time = 0;

  /* Indication whether the connection failure reason was a ConnectionDeny from
     the AN.  If we close the session because we fail to open the connection
     for configuration for max retries in SCP, and we received at least one
     ConnectionDeny in our last round of retries, we will count the connection
     setup failure towards avoiding the current system. */
  hdrind_give_ind(HDRIND_IDLE_CONNECTION_FAILED, (void*)&conn_fail_info);
} /* hdridle_connect_setup_fail_cleanup() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CONN_SETUP_TIMEOUT

DESCRIPTION
  This function processes the connection setup state timer timeout.

DEPENDENCIES
  None

PARAMETERS
  Reason - Connection set up time out reason - no TCA or RTCAck.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_conn_setup_timeout 
( 
  hdridle_conn_fail_enum_type reason 
)
{

  if ( hdridles_get_state() == HDRIDLES_CONN_SETUP_STATE )   
  {
    hdrplog_end_conn_attempt(HDRPLOG_CONN_ATT_SETUP_TIMEOUT);
    hdridle_connect_setup_fail_cleanup(FALSE);
#ifdef FEATURE_LTE_TO_HDR_OH
    if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Should not reach here" );
      hdridles_change_state( HDRIDLES_INACTIVE_STATE );
      hdrind_give_ind( HDRIND_IDLE_EXIT_HDR_MODE, NULL );
    }
    else
#endif  /* FEATURE_LTE_TO_HDR_OH */
    {
      if(hdridle_rpt_conn_setup_fail(reason) == SD_SS_ACT_CONTINUE)
      {
        hdridles_change_state(HDRIDLES_MONITOR_STATE);
      }
    }
  }
  else
  {

    /*-----------------------------------------------------------------------
      Wrong state or timer is not on, do nothing..
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT( MSG_LEGACY_LOW, "Connection setup timer timout ignored" );
  }
}/* hdridle_process_conn_setup_timeout() */

/* <EJECT> */
#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRIDLE_STOP_TUNNEL_CONN_TIMER

DESCRIPTION
  This function stops the tunnel connection timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_stop_tunnel_conn_timer ( void )
{
  if( rex_get_timer( &hdridle.tunnel_conn_timer) !=0 )
  {
    /* stop the timer */
    HDR_MSG_PROT( MSG_LEGACY_LOW, "Stop Tunnel Connection timer" );
    ( void ) rex_clr_timer( &hdridle.tunnel_conn_timer);
  }
} /* hdridle_stop_tunnel_conn_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_START_TUNNEL_CONN_TIMER

DESCRIPTION
  This function starts the tunnel connection timer

DEPENDENCIES
  None

PARAMETERS
  cnt - timer count in millisecond

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_start_tunnel_conn_timer
(
  rex_timer_cnt_type cnt
)
{
  /* stop connection setup timer if it is active */
  hdridle_stop_tunnel_conn_timer();
  HDR_MSG_PROT( MSG_LEGACY_LOW, "Start Tunnel Connection timer" );
  ( void ) rex_set_timer( &hdridle.tunnel_conn_timer, cnt);
} /* hdridle_start_tunnel_conn_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_TUNNEL_CONN_TIMEOUT

DESCRIPTION
  This function processes the tunnel connection timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_tunnel_conn_timeout ( void )
{

  hdridles_state_enum_type curr_idle_state = hdridles_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (( curr_idle_state != HDRIDLES_CONN_SETUP_STATE ) &&   
      ( curr_idle_state != HDRIDLES_TUNNEL_STATE ))
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Tunnel connection timer timout ignored" );
    return;
  }

  if ( rex_get_timer( &hdridle.tunnel_conn_timer ) != 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "Stop Tunnel Connection timer" );
    ( void ) rex_clr_timer( &hdridle.tunnel_conn_timer );
  }

  hdridle_connect_setup_fail_cleanup(FALSE);

  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    hdrsap_set_tunnel_mode(HDRHAI_TUNNEL_MODE_ENABLED);
    hdridles_change_state(HDRIDLES_INACTIVE_STATE);
  }

}/* hdridle_process_conn_setup_timeout() */
#endif /* FEATURE_LTE_TO_HDR_OH */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_LINK_ACQUIRED_IND

DESCRIPTION
  This function processes the link acquired indication

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdridle_process_link_acquired_ind) */
LOCAL void hdridle_process_link_acquired_ind
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{

  hdridles_state_enum_type state=hdridles_get_state();
                          /* current idle state */
  hdrind_ind_data_union_type ind_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( state )
  {

    case HDRIDLES_MONITOR_STATE:

      hdridle.connect_reason = HDRIDLE_CR_AN; /*lint !e616 */
      /* fall through; no break here */

    case HDRIDLES_CONN_SETUP_STATE:

      if (!hdrrup_is_tc_assigned(HDRHAI_IDLE_STATE_PROTOCOL))
      {
        /* This is a race condition.  We got a RUP connection setup timeout,
         * but the RTCACK was decoded before we deactivated the RMAC.
         * In this case IDLE should not open the connection. */
         HDR_MSG_PROT( MSG_LEGACY_HIGH, "RTCACK after RUP Timeout!" );
         break;
      }

#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, 
         "Active ho wait till MMOC is activated to tell call connected to CM" );
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        hdridle_rpt_conn_setup_success();
      }

      /* change state then give indication
         Do not change this sequence since it is tied to the ALMP
         implementation
      */
      hdridles_change_state(HDRIDLES_INACTIVE_STATE);

      /* fill the indication data */
      if(hdridle.connect_reason & HDRIDLE_CR_USER)
      {
        ind_data.open_reason =   HDRHAI_OPEN_USER_REQ;
      }
      else if (hdridle.connect_reason & HDRIDLE_CR_AN)
      {
        ind_data.open_reason =   HDRHAI_OPEN_AN;
      }
#ifdef FEATURE_HDR_HANDOFF
      else if (hdridle.connect_reason & HDRIDLE_CR_NO_DATA)
      {
        ind_data.open_reason =   HDRHAI_OPEN_ABORT;
      }
#endif /* FEATURE_HDR_HANDOFF */
      else if (hdridle.connect_reason & HDRIDLE_CR_MSG)
      {
        ind_data.open_reason =   HDRHAI_OPEN_SEND_RTC_MSG;
      }
      else if (hdridle.connect_reason & HDRIDLE_CR_ABORT)
      {
        ind_data.open_reason =   HDRHAI_OPEN_ABORT;
      }
#ifdef FEATURE_LTE_TO_HDR_OH
      else if (hdridle.connect_reason & HDRIDLE_CR_ACTIVE_HO)
      {
        ind_data.open_reason =   HDRHAI_OPEN_ACTIVE_HO;
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
      else
      {
        ERR("Error in connect reason %d",hdridle.connect_reason,0,0);
        ind_data.open_reason =   HDRHAI_OPEN_UNSPECIFIED;
      }

      /* Give the Connection opened indication. Note that this will result in
         RLP geting initialized. The RLP initialization is done in the HDRMC
         task context (from HDRPAC) as part of the Indication callback */
      HDRTRACE_IDLE_CONNECT_REASON( hdridle.connect_reason ); 
      hdrind_give_ind(HDRIND_IDLE_CONNECTION_OPENED, (void *)&ind_data);
      break;

    default:

      HDRTRACE_IND_IGNORED_STATE( HDRHAI_IDLE_STATE_PROTOCOL,
                                   ind_name, hdridles_get_state() );
      break;
  }

} /* hdridle_process_link_acquired_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_SLEEP_RELATED_IND

DESCRIPTION
  This function processes the end of the sync capsule indication and
  sleep capsule done indication, and the indication that all the data
  streams have been closed.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_sleep_related_ind
(
  hdrind_ind_name_enum_type         ind_name
)
{

  if(hdridles_get_state() != HDRIDLES_MONITOR_STATE)
  {
     /* ignore the indication if it is not in the monitor state */
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_IDLE_STATE_PROTOCOL,
                                 ind_name, hdridles_get_state() );
    return;
  }

  hdridle_check_ok_to_sleep();

} /* hdridle_process_sleep_related_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SET_REC_END_OF_SYNC_CAP_IND

DESCRIPTION
  This function set the flag for the
  end of sync capsule indication received

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  flag   - TRUE or FALSE

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridle_set_rcv_end_of_sync_cap_ind(boolean flag)
{
  hdridle.rcv_end_of_sync_cap_ind = flag;
  hdridle.sleep_capsule_done = flag;

  /* Alwasys set continued_scc_pkt_expected to FALSE:
   * when flag argument is set to TRUE, we definitely need to set 
   * continued_scc_pkt_expected to FALSE, but even when flag is set to FALSE
   * it means the phone just enters monitor state from sleep
   */
  hdridle.intra_scc.continued_scc_pkt_expected = FALSE;
} /* hdridle_set_rcv_end_of_sync_cap_ind() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_WAKEUP_IND

DESCRIPTION
  This function processes the wakeup indication

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_wakeup_ind
(
  hdrind_ind_name_enum_type         ind_name,
  hdrind_ind_data_union_type        *ind_data_ptr
)
{
  hdridles_state_enum_type curr_state = hdridles_get_state();
/* ----------------------------------------------------------------------- */

  if ( (curr_state == HDRIDLES_SLEEP_STATE) ||
       (curr_state == HDRIDLES_CONN_SETUP_STATE) )
  {
    /* Process the indication if idle protocol is in the sleep or connection 
       setup state. Idle state will be connection set up if the wake up is a 
       rude wake up. 
     */
  }
  else
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_IDLE_STATE_PROTOCOL,
                                ind_name, hdridles_get_state() );
    return;
  }

#ifdef FEATURE_HDR_TO_LTE   
  ASSERT (ind_data_ptr != NULL);

  /* Set the last wakeup reason so that it can be checked during next 
     sleep command to SRCH */ 
  hdridle.last_wakeup_reason = ind_data_ptr->wakeup_status.wakeup_reason;
#endif /* FEATURE_HDR_TO_LTE*/

  if (curr_state == HDRIDLES_SLEEP_STATE)
  {
    hdridles_change_state(HDRIDLES_MONITOR_STATE);
  }
  else
  {
    /* If idle is in connection set up state we still need to process the 
       wake up indication because in DDARF case as SRCH is not going to 
       declare system loss, CP should declare system loss immediately for
       rude wake up. Otherwise connection attempt will progress when HDR 
       system is not acquired.
     */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "Recvd wake up ind in connection setup state");
  }

#ifdef FEATURE_HDR_DDARF
  ASSERT (ind_data_ptr != NULL);

  if (ind_data_ptr->wakeup_status.reacq_succeded)
  {
    /* AT acquired successfully on wakeup before reaching the max allowed
       retry attempts, so reset the reacq failure count */
    hdridle.num_reacq_failed = 0;
  }
  else
  {
#ifdef FEATURE_HDR_CC_DECODE_FAIL
	/* Increment the common counter of consecutive reacquistion failure/cc failure on wakeup */
	hdridle.num_cc_reacq_failed++;
        HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                 "CC/Re-Acq failed for %d times so far",hdridle.num_cc_reacq_failed);
        /* The following 2 conditions enable minimum of SCI sleep and CC sleep */
        hdridle.rcv_end_of_sync_cap_ind = TRUE;
        hdridle.intra_scc.continued_scc_pkt_expected = FALSE;
	  
	if ((hdridle.num_cc_reacq_failed == 1) && /* reacq failed for the first time */
        hdridle.last_sci <= 9)/* less than equal 5.12s sleep */
	{	
      	     hdridle.sleep_capsule_done = TRUE; /* enables SCI sleep */
             HDR_MSG_PROT(MSG_LEGACY_HIGH, "Enabled condition for SCI sleep");
	}
#else /* only  FEATURE_HDR_DDARF is defined */ 
    /* Increment the number of consecutive reacquistion failure on wakeup */
    hdridle.num_reacq_failed++;
  
    if ( (hdridle.last_sci > HDRIDLE_MAX_SCI_VALUE) || 
         (hdridle.num_reacq_failed >= 
          hdridle_reacq_retry_attempts[hdridle.last_sci]) ||
         (ind_data_ptr->wakeup_status.wakeup_reason == HDRSRCH_WAKEUP_FAST) )
    {
      /* If AT is doing longer sleeps or AT did not reacquire within the max 
         allowed retry attempts or current wakeup was a rude wakeup declare 
         network lost */
      HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
           "System Lost: reason=%d. Reacquisition failed after max retry= %d",
           HDRSRCH_SYSLOST_LOST_ACQ_AFTER_SLEEP,
           hdridle.num_reacq_failed);
    
      HDRTRACE_LMAC_NETWORK_LOST(HDRSRCH_SYSLOST_LOST_ACQ_AFTER_SLEEP);

      /* Reset the reacq failure counter */
      hdridle.num_reacq_failed = 0;

      hdrind_give_ind(HDRIND_IDLE_NETWORK_LOST, NULL);
    }
    else
    {
      hdridle_set_rcv_end_of_sync_cap_ind(TRUE);

      HDRTRACE_IDLE_REACQ_FAIL_DDRAF(hdridle.num_reacq_failed,
                                     hdridle_reacq_retry_attempts[hdridle.last_sci],
                                     hdridle.last_sci);

      HDR_MSG_PROT(MSG_LEGACY_HIGH, "DDARF: Forcing sleep");
    }
#endif /* FEATURE_HDR_CC_DECODE_FAIL */ 
  }/* else */
#endif /* FEATURE_HDR_DDARF */

} /* hdridle_process_wakeup_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CONN_SETUP_FAILED_IND

DESCRIPTION
  This function processes the connection setup failed indication from
  the route update

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdridle_process_conn_setup_failed_ind) */
LOCAL void hdridle_process_conn_setup_failed_ind
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{

  switch(hdridles_get_state())
  {

    case HDRIDLES_CONN_SETUP_STATE:
      if (ind_data_ptr->rup_conn_fail_reason == HDRRUP_CONN_FAIL_TIMEOUT)
      {
        /* treat this same as the connection setup timeout */
        hdridle_process_conn_setup_timeout(HDRIDLE_CF_RTCACK_TIMEOUT);
      }
#ifdef FEATURE_SVDO
      else if (ind_data_ptr->rup_conn_fail_reason == HDRRUP_CONN_FAIL_TCA_FAILED)
      {
        hdridle_process_conn_setup_tca_failed();
      }
#endif
      else
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "undef conn_fail_reason %d", 
                     ind_data_ptr->rup_conn_fail_reason);
      }
      break;

    default:
      /* ignore the indication if it is not in the right state */
      HDRTRACE_IND_IGNORED_STATE( HDRHAI_IDLE_STATE_PROTOCOL,
                                   ind_name, hdridles_get_state() );
      break;
  }
} /* hdridle_process_conn_setup_failed_ind() */

/* <EJECT> */
#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CONN_DENY_MSG

DESCRIPTION
  This function processes the connection deny message

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_conn_deny_msg
(
  dsm_item_type * item_ptr
)
{
  hdrerrno_enum_type        status = E_SUCCESS;
                                     /* msg unpacking status */
  sd_ss_act_e_type          sd_act = SD_SS_ACT_CONTINUE;
                                     /* action returned by SD */
  hdridles_state_enum_type  cur_state = hdridles_get_state();
                                     /* current idle state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cur_state != HDRIDLES_CONN_SETUP_STATE)
  {
    /*-----------------------------------------------------------------------
      If the protocol is in the inactive state, do nothing.
    -----------------------------------------------------------------------*/
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_IDLE_STATE_PROTOCOL,
    HDRIDLE_CONN_DENY_ID, cur_state); /*lint !e506 */
    return;
  }


  status |= HDRIDLE_GET_CD_TRANS_ID(item_ptr,
                            &(hdridle.conn_deny_msg.trans_id));

  status |= HDRIDLE_GET_CD_DENY_REASON(item_ptr,
                               &(hdridle.conn_deny_msg.deny_reason));

  if (status != E_SUCCESS)
  {
    /* unable to unpack the message fields discard the message*/
    HDR_MSG_PROT( MSG_LEGACY_LOW, "Discard Conn deny msg -invald fields" );
    return;
  }

  if( hdridle.conn_deny_msg.trans_id !=
      hdridle.conn_req_msg.trans_id  )
  {
    /* rev number is not in the valid range */
    HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_IDLE_STATE_PROTOCOL,
    HDRIDLE_CONN_DENY_ID, cur_state); /*lint !e506 */
    return;
  }

  if ( hdridle.conn_deny_msg.deny_reason == 
       HDRIDLE_CD_AUTH_OR_BILLING_FAILURE )
  {
    /* Treat the Auth / Billing failure differently
     * from the other failures. Mark Auth/Billing
     * failure as a connection deny, then HMP will 
     * report to SD to immediately avoid the system
     * for 10 minute. Other failures are marked as
     * connection failure, then HMP will report to
     * SD to perform the telescoping avoidance.
     */
    hdridle_connect_setup_fail_cleanup(TRUE);
  }
  else
  {
    hdridle_connect_setup_fail_cleanup( FALSE );
  }

  HDRTRACE_IDLE_CONNECTION_DENIED_REASON( hdridle.conn_deny_msg.deny_reason );
  HDRTRACE_HDRMC_IDLE_CONNECTION_DENIED();
  hdrplog_conn_deny_rcvd((hdrplog_conn_attempt_result_enum_type)
                         hdridle.conn_deny_msg.deny_reason,
                         item_ptr);

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    if ( hdralmps_get_state() == HDRALMPS_IDLE )
    {
      /* Ideally in the ActiveHandoff, ConnectionDeny will be received before
       * ALMP enters IDLE state (AT does fast acq). However, we do observe that
       * a ConnecitonDeny received after TCA is processed (ALMP has entered 
       * IDLE state and waiting for RTCAck). To handle the later case, we
       * have to send HDRIND_IDLE_EXIT_HDR_MODE to go back to tunnel mode. */
      hdridles_change_state( HDRIDLES_INACTIVE_STATE );
      hdrind_give_ind( HDRIND_IDLE_EXIT_HDR_MODE, NULL );
    }
    else
    {
      hdrsap_set_tunnel_mode( HDRHAI_TUNNEL_MODE_ENABLED );
      hdridles_change_state( HDRIDLES_INACTIVE_STATE );
    }
  }
  else
#endif  /* FEATURE_LTE_TO_HDR_OH */
  {
    switch(hdridle.conn_deny_msg.deny_reason)
    {
      case HDRIDLE_CD_GENERAL:
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "ConnectionDeny (General)" );
        sd_act = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_CD_GENERAL);
        break;

      case HDRIDLE_CD_NETWORK_BUSY:
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "ConnectionDeny (Network Busy)" );
        sd_act = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_CD_NET_BUSY);
        break;

      case HDRIDLE_CD_AUTH_OR_BILLING_FAILURE:
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "ConnectionDeny (Auth/Billing Failure)" );
        sd_act = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_CD_AUTH_BILL_FAIL);
        break;

      case HDRIDLE_CD_PREF_CHAN_NOT_AVAILABLE:
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
           "ConnectionDeny (Pref. Chan. Not Avail.) [Treated same as General]" );
        sd_act = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_CD_PREF_CHAN_NOT_AVAIL);
        break;

      default:
        ERR( "Invalid deny reason code %d", hdridle.conn_deny_msg.deny_reason,0,0);
        sd_act = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_CD_GENERAL);
        break;  /*lint !e527 */
    }

    if(sd_act == SD_SS_ACT_CONTINUE)
    {
      hdridles_change_state(HDRIDLES_MONITOR_STATE);
    }
  }
} /* hdridle_process_conn_deny_msg() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_UNPACKED_PAGE_MSG

DESCRIPTION
  This function processes the page message

  Following table lists the AT behavior when SSPI and QSM are used in 
  combination.
  SSPI   QSM   Expected Behavior 
  ----   ---   -----------------
  ON     ON    Accept pages while in QSM or Suspended state, but ignore pages
               on wakeup after slotted sleep.
               
  ON     OFF   Accept pages while in Suspended state and ignore pages on wakeup
               after slotted sleep.
  
  OFF    ON    Accept pages while in QSM or Suspended state and also accept
               pages on wakeup after slotted sleep.
               
  OFF    OFF   Legacy behavior. (Accept pages while in Suspended state and also
               accept pages on wakeup after slotted sleep.)

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_unpacked_page_msg
(
  boolean persist_included,
  uint8   persist
)
{
  hdridles_state_enum_type state = hdridles_get_state();
                                   /* current idle state */

#ifdef FEATURE_HDR_SSPI
  #error code not present
#endif /* FEATURE_HDR_SSPI */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* currently we don't make use of persistence value */
  HDR_MSG_PROT_2(MSG_LEGACY_MED,
                 "Page Persist %d/%d",
                 persist_included, persist);

#ifdef FEATURE_HDR_SSPI
  #error code not present
#endif /* FEATURE_HDR_SSPI */

  switch( state )
  {

    case HDRIDLES_CONN_SETUP_STATE:
    case HDRIDLES_CONN_SETUP_SUSPEND_STATE:

      /* set user abort bit to 0 */
      hdridle.connect_reason &= ~HDRIDLE_CR_ABORT;
      /* connection setup is in progress. update the connect_reason
         to include CR_AN
      */
      hdridle.connect_reason |= HDRIDLE_CR_AN;
      break;

    case HDRIDLES_MONITOR_STATE:
    case HDRIDLES_SLEEP_STATE:
    case HDRIDLES_SUSPEND_STATE:

      hdridle.connect_reason          = HDRIDLE_CR_AN;
      hdridle.conn_req_msg.req_reason = HDRIDLE_AN_INITIATED;
      /* change to the connection setup state */
      hdridles_change_state(HDRIDLES_CONN_SETUP_STATE);
      break;

    default:
      /* ignore the page */
      HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLE_PAGE_ID,state); /*lint !e506 */

      break;
  }
} /* hdridle_process_unpacked_page_msg */

/*===========================================================================

FUNCTION HDRIDLE_PROCESS_PAGE_MSG

DESCRIPTION
  This function processes the page message

  Following table lists the AT behavior when SSPI and QSM are used in 
  combination.
  SSPI   QSM   Expected Behavior 
  ----   ---   -----------------
  ON     ON    Accept pages while in QSM or Suspended state, but ignore pages
               on wakeup after slotted sleep.
               
  ON     OFF   Accept pages while in Suspended state and ignore pages on wakeup
               after slotted sleep.
  
  OFF    ON    Accept pages while in QSM or Suspended state and also accept
               pages on wakeup after slotted sleep.
               
  OFF    OFF   Legacy behavior. (Accept pages while in Suspended state and also
               accept pages on wakeup after slotted sleep.)

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_page_msg 
(
  dsm_item_type *item_ptr
)
{
  uint8 page_resp_apersistence_included = 0;
  uint8 page_resp_apersistence  = 0;

#ifdef FEATURE_HDR_SSPI
  #error code not present
#endif /* FEATURE_HDR_SSPI */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
  if (hdralmp_hdr_to_lte_redir_meas_in_progress() == TRUE )
  { 
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "ORNL msg ignored: D2L redirection is in progress");
    return;
  }
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

#ifdef FEATURE_HDR_SSPI
  #error code not present
#endif

  hdridle_process_unpacked_page_msg((boolean)page_resp_apersistence_included,
                                    page_resp_apersistence);

} /* hdridle_process_page_msg() */

#ifdef FEATURE_HDR_REVC
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_MULTI_PAGE_MSG

DESCRIPTION
  This function processes the page message

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_multi_page_msg 
(
  dsm_item_type *msg_ptr
)
{
  uint16 offset = HDRIDLE_MSG_ID_SIZE;

  /* fields in MultiATPage */
  uint32 num_msb = 0;
  uint32 num_lsb = 0;
  uint32 msb_len = 0;
  uint32 lsb_len = 0;
  uint32 msb = 0;
  uint32 lsb = 0;

  hdrhai_ati_record_type uati;

  uint32 page_resp_apersistence_included = 0;
  uint32 page_resp_apersistence  = 0;

  uint8 i, j;

  /* msg unpacking status */
  hdrerrno_enum_type status = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  status = HDRIDLE_UNPACK32_ADV(msg_ptr, &num_msb, 
                                offset, HDRIDLE_MULTI_PAGE_NUM_UATI_MSB_SIZE);

  if (status != E_SUCCESS)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                   "Unpacking MultiATPage:NumUATIMSB err:%d", status);
    return;
  }

  uati.ati_type = HDRHAI_UATI;

  HDR_MSG_PROT_1(MSG_LEGACY_MED, "num msb %d", ++num_msb);                 

  for (i=0; i<num_msb; i++)
  {
    status = HDRIDLE_UNPACK32_ADV(msg_ptr, &msb_len, 
                                  offset, 
                                  HDRIDLE_MULTI_PAGE_UATI_MSB_LEN_SIZE);

    msb_len++;

    if (status != E_SUCCESS)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Unpacking MultiATPage:UATIMsbLen err:%d", status);
      return;
    }    

    status = HDRIDLE_UNPACK32_ADV(msg_ptr, &msb, offset, (uint16) msb_len);

    status |= HDRIDLE_UNPACK32_ADV(msg_ptr, &num_lsb, 
                                   offset, HDRIDLE_MULTI_PAGE_NUM_PAGES_SIZE);

    HDR_MSG_PROT_1(MSG_LEGACY_MED, "num lsb %d", ++num_lsb);

    if (status != E_SUCCESS)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Unpacking MultiATPage:UATIMsb/NumPage err:%d", status);
      return;
    }
    
    lsb_len = 32 - msb_len;

    for (j=0; j<num_lsb; j++)
    {
      status = HDRIDLE_UNPACK32_ADV(msg_ptr, &lsb, offset, (uint16) lsb_len);

      status |= HDRIDLE_UNPACK32_ADV(msg_ptr, &page_resp_apersistence_included, 
                                     offset,
                                     HDRIDLE_MULTI_PAGE_RESP_APERSIST_INCLUDED_SIZE);

      if (page_resp_apersistence_included)
      {
        status |= HDRIDLE_UNPACK32_ADV(msg_ptr, &page_resp_apersistence,
                                      offset, 
                                      HDRIDLE_MULTI_PAGE_RESP_APERSIST_SIZE);
      }

      if (status != E_SUCCESS)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                       "Unpacking MultiATPage:UATILsb/APersist err:%d", status);
        return;
      }

      uati.ati_value = msb << lsb_len | lsb;

      HDR_MSG_PROT_7(MSG_LEGACY_MED, "msb %x/%d lsb %x/%d uati %x AP inc %d AP %d",
                     msb, msb_len, lsb, lsb_len, uati.ati_value, 
                     page_resp_apersistence_included, page_resp_apersistence);      

      if (hdrampati_match_receive_ati_address(&uati, 
                                              HDRHAI_IDLE_STATE_PROTOCOL))
      {
	HDRTRACE_IDLE_MSG_RX_PAGE();/* Page msg received, log event */
	  
        /* both UATI matched, page is for us */
        hdridle_process_unpacked_page_msg(
          (boolean)page_resp_apersistence_included, 
          (uint8)page_resp_apersistence);

        /* assuming only one page for this AT */
        return;
      }
    }

    /* possible optimizations:
       1) if MSB/LSB in MultiATPage are sorted
       -- In this case, early return is possible
       2) If AN will not include two MSB record matching AT, for example:
          Suppose AT's UATI = 0x1234567890123456, AN will not include a
          UATIMsb record of 0x1234 which does not contain page for this
          AT, AND includes another UATIMsb record of 0x12345 which
          contains a page for this AT.
       -- In this case, early return is possible if MSB matched and no
          LSB has matched
    */
  }
} /* hdridle_process_multi_page_msg() */
#endif /* FEATURE_HDR_REVC */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_GET_CURRENT_CONFIG_CHANGE

DESCRIPTION
  This function returns the configuration change values associated with the 
  current sector.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Value of the configuration change field for current sector.

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdridle_get_current_config_change ( void )
{

  return hdridle.config_change;

}  /* hdridle_get_current_config_change() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_AT_HAS_A_PAGE_MSG

DESCRIPTION
  This function checks the QuickPage message to find whether AT has a Page in 
  the sync capsule.

DEPENDENCIES
  None

PARAMETERS
  qpi_per_grp_cnt - QuickPIPerGroupCountMinusOne field value from QuickPage 
                    message.
  qpi[] - QuickPageIndicator field from QuickPage message.

RETURN VALUE
  True if the QuickPageIndicator indicates that AT has a Page and FALSE 
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdridle_at_has_a_page_msg
(
  uint8 qpi_per_grp_cnt,
  uint64 qpi[]
)
{
  uint32 session_seed = 0; /* Session seed value */
  uint16 w, x, y, z;      /* Index QuickPageIndicator */
  uint64 grp0, grp1, grp2, grp3;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get current session seed value */
  session_seed = hdramp_get_session_seed(HDRHAI_IDLE_STATE_PROTOCOL);

  /* Compute W, X, Y, Z QuickPageIndicator. The Wth, Xth, Yth, and Zth 
     indicator are computed as detailed in section 7.6.6.1.6.1.2 of Quick 
     Idle state protocol 
  */
  w = x = y = z = 0;

  w = hash( session_seed, 
            (qpi_per_grp_cnt + 1), 
            (uint16) (2*(session_seed & 0x0FFF))) + 0*(qpi_per_grp_cnt + 1);

  x = hash( session_seed, 
            (qpi_per_grp_cnt + 1), 
            (uint16) (6*(session_seed & 0x0FFF))) + 1*(qpi_per_grp_cnt + 1);

  y = hash( session_seed, 
            (qpi_per_grp_cnt + 1), 
            (uint16) (14*(session_seed & 0x0FFF))) + 2*(qpi_per_grp_cnt + 1);

  z = hash( session_seed, 
            (qpi_per_grp_cnt + 1), 
            (uint16) (30*(session_seed & 0x0FFF))) + 3*(qpi_per_grp_cnt + 1);
  
  HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
                  "QPIs in QuickPage message %x, %x, %x, %x", 
                  qpi[0], qpi[1], qpi[2], qpi[3] );

  HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
                  "Indices to look for in QPIs W=%d, X=%d, Y=%d, Z=%d", 
                  w, x, y, z );

  grp0 = grp1 = grp2 = grp3 = 0;

  grp0 = ((qpi[0] >> ((qpi_per_grp_cnt) - (w - (qpi_per_grp_cnt + 1)*0))) & 
          HDRIDLE_QUICKPAGE_QPI_BIT_MASK);
  grp1 = ((qpi[1] >> ((qpi_per_grp_cnt) - (x - (qpi_per_grp_cnt + 1)*1))) & 
          HDRIDLE_QUICKPAGE_QPI_BIT_MASK);
  grp2 = ((qpi[2] >> ((qpi_per_grp_cnt) - (y - (qpi_per_grp_cnt + 1)*2))) &
          HDRIDLE_QUICKPAGE_QPI_BIT_MASK );
  grp3 = ((qpi[3] >> ((qpi_per_grp_cnt) - (z - (qpi_per_grp_cnt + 1)*3))) & 
          HDRIDLE_QUICKPAGE_QPI_BIT_MASK);

  HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
            "AT's bit value in QPI groups grp1=%d, grp2=%d, grp3=%d, grp4=%d",
             grp0, grp1, grp2, grp3 );

  /* If Wth, Xth, Yth and Zth bit are set then AT has a page */
  if ( grp0 && grp1 && grp2 && grp3 )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "QP indicates AT has a page" );
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* hdridle_at_has_a_page_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_QUICK_PAGE_MSG

DESCRIPTION
  This function processes the quick page message

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_quick_page_msg
(
  dsm_item_type * item_ptr
)
{
  hdrerrno_enum_type status = E_SUCCESS;  /* MSG unpacking status */
  uint16 cur_subtype = hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
                                          /* Current idle subtype */
  uint8 config_change = 0; /* Configuration change */
  uint8 qpi_per_grp_cnt = 0; /* QuickPIPerGroupCountMinusOne */
  uint64 qpi[HDRIDLEC_QPI_NUM_GROUPS]; /* QuickPageIndicator Groups */
  qword qpi_temp_var = {0};
  uint8 i = 0; /* Loop index */ 
  uint16 offset = 0;  /* Bit offset in the message */
  sys_link_type qp_link; /* Link message was received on */
  hdrsrch_link_id_type  link_id;  /* Link ID of received message */
  hdridles_state_enum_type cur_state = hdridles_get_state();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_LTE_TO_HDR_OH
if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_IDLE_STATE_PROTOCOL ))
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_IDLE_STATE_PROTOCOL,
    HDRIDLE_QUICK_PAGE_ID, cur_state);
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
	      "Discard quick page msg - rcvd in Tunnel mode idlst:%d idlsubtyp=%d",
              cur_state, cur_subtype);
    return;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Get the link ID on which QP was received */
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  /* Get the PN, Channel on which Quick Page message was received */
  hdrlmac_get_link_info( link_id , &qp_link );

  /* Quick Page is valid only when Quick idle is negotiated and in 
     idle state */
  if ( !( cur_subtype == HDRSCP_QUICK_IDLE
#ifdef FEATURE_LTE_TO_HDR_OH
          ||
          cur_subtype == HDRSCP_IRAT_QUICK_IDLE
#endif /* FEATURE_LTE_TO_HDR_OH */ 
       ) ||
       ( hdralmp_connection_is_open( HDRHAI_IDLE_STATE_PROTOCOL ) ) || 
       ( !hdrovhd_qp_is_ok_to_process( &qp_link ) )
     )
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_IDLE_STATE_PROTOCOL,
    HDRIDLE_QUICK_PAGE_ID, cur_state); /*lint !e506 */
    return;
  }

  offset += HDRIDLE_MSG_ID_SIZE;

  /* Unpack Configuration Change field */
  status |= HDRIDLE_GET_QP_CONFIG_CHANGE(item_ptr, &config_change, offset);

  offset += HDRIDLE_QUICKPAGE_CONFIG_CHANGE_SIZE;

  /* Unpack QuickPIPerGroupCountMinusOne */
  status |= HDRIDLE_GET_QP_QPI_PER_GROUP_COUNT(item_ptr, &qpi_per_grp_cnt, offset);

  offset += HDRIDLE_QUICKPAGE_QPI_PER_GRP_COUNT_SIZE;

  for ( i = 0; i < HDRIDLEC_QPI_NUM_GROUPS; i++ )
  {
    /* Unpack QuickPageIndicator */
    status |= HDRIDLE_GET_QP_QPI(item_ptr, qpi_temp_var, offset, 
                                 (qpi_per_grp_cnt + 1));
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy ( &qpi[i], sizeof(qpi[0]), qpi_temp_var, sizeof(qword) );
#else
    memcpy ( &qpi[i], qpi_temp_var, sizeof(qword) );
#endif /* FEATURE_MEMCPY_REMOVAL */

    offset += (qpi_per_grp_cnt + 1);
  }

  if ( status != E_SUCCESS )
  {
    /* unable to unpack the message fields discard the message*/
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Discard quick page msg -invalid fields" );
    return;
  }

  /* Update ConfigurationChange to the value in QuickPage message */
  hdridle.config_change = config_change;

  if ( config_change == 0x03 )
  {
    /* When configuration change is set to '11' generate 
       OverheadMessages.ANRedirect command */
    hdrovhd_an_redirected( HDRHAI_IDLE_STATE_PROTOCOL );
  }
  else
  {
    /* Overhead checkconfiguration */
    hdrovhd_check_configuration( config_change, link_id, qp_link, 
                                 HDRHAI_IDLE_STATE_PROTOCOL );

    if ( hdridle_at_has_a_page_msg(qpi_per_grp_cnt, qpi) )
    {
      hdridle_sleep_based_on_quickpage( FALSE );
    }
    else
    { 
      /* If AT has no page in the sync capsule then do not go to sleep after 
         decoding the quick sync capsule stay awake to decode the sync capsule 
       */
    }

    if (cur_state == HDRIDLES_MONITOR_STATE)
    {
      /* Check whether AT is ready to go to sleep */
      hdridle_check_ok_to_sleep();
    }
  }

} /* hdridle_process_quick_page_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_SUSPEND_TIMEOUT

DESCRIPTION
  This function processes the suspend timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_suspend_timeout ( void )
{

  if(hdridles_get_state() == HDRIDLES_MONITOR_STATE)
  {
        hdridle_check_ok_to_sleep();
  }
  else
  {
        /*------------------------------------------------------------------
           Wrong state or timer is not on, do nothing..
        ------------------------------------------------------------------*/
        HDR_MSG_PROT( MSG_LEGACY_LOW, "Suspend timer timout ignored" );
  }
} /* hdridle_process_suspend_timeout() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_STOP_CONN_SETUP

DESCRIPTION
  This function cancels the connection setup if it is in progress


DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
   reason - reason for aborting the connection setup

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL sd_ss_act_e_type hdridle_stop_conn_setup
(
  hdrhai_abort_reason_enum_type reason
)
{

  hdridles_state_enum_type              cur_state = hdridles_get_state();
                                           /* current idle state */
  sd_ss_act_e_type                      ret_val = SD_SS_ACT_CONTINUE;
                                          /* action returned by SD */
  hdrplog_conn_attempt_result_enum_type result = HDRPLOG_CONN_ATT_SYS_LOST;
                                        /* connection attempt result */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cur_state == HDRIDLES_CONN_SETUP_STATE) ||
      (cur_state == HDRIDLES_CONN_SETUP_SUSPEND_STATE))
  {
      /* stop connection setup */
      hdridle_connect_setup_fail_cleanup(FALSE);

      switch(reason)
      {
        case HDRHAI_ABORT_MAX_ACC_PROBE:
          ret_val = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_MAX_ACC_PROBE);
          result  = HDRPLOG_CONN_ATT_MAX_PROBES;
          break;

        case HDRHAI_ABORT_SYS_LOST:
          ret_val = hdridle_rpt_conn_setup_fail(HDRIDLE_CF_SYS_LOST);
          result  = HDRPLOG_CONN_ATT_SYS_LOST;
          break;

        case HDRHAI_ABORT_REDIRECT_CHG_HDR:
          ret_val = hdridle_rpt_conn_setup_fail(
                      HDRIDLE_CF_REDIRECT_CHG_HDR);
          result  = HDRPLOG_CONN_ATT_REDIRECT;
          break;

        case HDRHAI_ABORT_REDIRECT_EXIT_HDR:
          ret_val = hdridle_rpt_conn_setup_fail(
                      HDRIDLE_CF_REDIRECT_EXIT_HDR);
          result  = HDRPLOG_CONN_ATT_REDIRECT;
          break;

        case HDRHAI_ABORT_NOT_PREF_CHG_HDR:
          ret_val = hdridle_rpt_conn_setup_fail(
                      HDRIDLE_CF_PRL_NOT_PREF_CHG_HDR);
          result  = HDRPLOG_CONN_ATT_NOT_PREFERRED;
          break;

        case HDRHAI_ABORT_NOT_PREF_EXIT_HDR:
          ret_val = hdridle_rpt_conn_setup_fail(
                      HDRIDLE_CF_PRL_NOT_PREF_EXIT_HDR);
          result  = HDRPLOG_CONN_ATT_NOT_PREFERRED;
          break;

        case HDRHAI_ABORT_OFFLINE:
          hdridle_send_call_fade_to_cm();
          result  = HDRPLOG_CONN_ATT_OFFLINE;
          break;

        case HDRHAI_ABORT_POWERDOWN:
          result  = HDRPLOG_CONN_ATT_POWERDOWN;
          break;

        case HDRHAI_ABORT_NAM_CHANGE:
          hdridle_send_call_fade_to_cm();
          result  = HDRPLOG_CONN_ATT_NAM_CHANGE;
          break;

        case HDRHAI_ABORT_ACQ_OTHERS:
          hdridle_send_call_fade_to_cm();
          result  = HDRPLOG_CONN_ATT_ACQ_OTHERS;
          break;

        case HDRHAI_ABORT_PREF_SYS_CHANGE:
          hdridle_send_deactivate_ack_to_cm();
          result  = HDRPLOG_CONN_ATT_ACQ_OTHERS;
          break;

        case HDRHAI_ABORT_UNSPECIFIED:
          hdridle_send_call_fade_to_cm();
          result  = HDRPLOG_CONN_ATT_UNSPECIFIED;
          break;

        case HDRHAI_ABORT_EXIT_TUNNEL:
          HDR_MSG_PROT(MSG_LEGACY_MED, "ConSetup clean-up in ACT HO");
          break;

        default:
          break;
      }

      /* log the end status of conenction attempt */
      hdrplog_end_conn_attempt(result);

  }

  return ret_val;
} /* hdridle_stop_conn_setup() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_ABORT_OPEN_CONN_CMD

DESCRIPTION
  This function processes an abort open connection command given to the
  the idle state protocol

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
   abort_reason - reason for aborting the open connection request

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_abort_open_conn_cmd
(
  hdrhai_abort_reason_enum_type abort_reason
)
{
  boolean abort_the_connection_now = FALSE; /* Flag to make code readable */
  cm_call_id_type call_id; /* Call IDs of pending user end */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (hdridles_get_state() != HDRIDLES_CONN_SETUP_STATE) &&
       (hdridles_get_state() != HDRIDLES_CONN_SETUP_SUSPEND_STATE) )
  {
    /* ignore the command */
    HDR_MSG_PROT( MSG_LEGACY_MED, "IDLE Abort Open Conn CMD ignored" );
    return;
  }

  /* -----------------------------------------------
   * First, adjust the connect_reason and flag if we 
   * need to abort the connection.
   * ----------------------------------------------- */

  if (abort_reason == HDRHAI_ABORT_USER)
  {
    if(hdridle.connect_reason == HDRIDLE_CR_USER)
    {
      /* The user was the only reason for the connection.  Abort. */
      hdridle.connect_reason = HDRIDLE_CR_ABORT;

      /* We would like to abort now, if possible */
      abort_the_connection_now = TRUE;
    }
    else if (hdridle.connect_reason & HDRIDLE_CR_USER)
    {
      /* remove the user bit from the connect reason and continue
         to setup the connection
      */
      hdridle.connect_reason &= ~HDRIDLE_CR_USER;

      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "connect reason: cleared: %d, current value: %d",
                      HDRIDLE_CR_USER, hdridle.connect_reason );
    }
    else
    {
      /* ignore the command and log an error message*/
      ERR("IDLE- Invalid abort open conn reason %d ",
            abort_reason,0,0); /*lint !e516 */
    }
  }
  else if (abort_reason == HDRHAI_ABORT_MSG)
  {
    if(hdridle.connect_reason == HDRIDLE_CR_MSG)
    {
      /* The MSG was the only reason for the connection and there is no more
       * msg in the pending_q, Abort. */
      hdridle.connect_reason = HDRIDLE_CR_ABORT;

      /* We would like to abort now, if possible */
      abort_the_connection_now = TRUE;
    }
    else if (hdridle.connect_reason & HDRIDLE_CR_MSG)
    {
      /* remove the MSG bit from the connect reason and continue
         to setup the connection
      */
      hdridle.connect_reason &= ~HDRIDLE_CR_MSG;
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "connect reason: cleared: %d, current value: %d",
                      HDRIDLE_CR_MSG, hdridle.connect_reason );
    }
    else
    {
      /* ignore the command and log an error message*/
      ERR("IDLE- Invalid abort open conn reason %d ",
            abort_reason,0,0); /*lint !e516 */
    }
  }
  else
  {
    /* ignore the command and log an error message*/
    ERR("IDLE- Inavlid abort open conn reason %d ",
    abort_reason,0,0); /*lint !e516 */
  }

  /* ------------------------------------------------
   * Second, if the connect_reason was just changed 
   * to HDRIDLE_CR_ABORT, try to abort the connection
   * ------------------------------------------------ */

  if (hdrrup_is_tc_assigned(HDRHAI_IDLE_STATE_PROTOCOL))
  {
    /* If we have got the TCA, let the connection setup proceed to its
     * logical conclusion.  If the connection opens successfully, ALMP
     * will close it.  Otherwise it will fail in at most 
     * HDRRUP_CONN_SETUP_TIMER_MSECS
     */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Connection Setup abort rxed after TCA" );
    abort_the_connection_now = FALSE;
  }
  else if (!hdridle.conn_req_msg.awaiting_slp_callback)
  {
    /* If we already got an ACK to our ConnectionRequest message 
     * then HDRIDLE_CONN_SETUP_TIMER1_MSECS applies, so again we 
     * just wait for the connection setup to complete then let
     * ALMP close it if necessary.  Forcing it closed would be foolish
     * as the TCA could come later and would cause a fast-connect.
     */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Connection Setup abort rxed after slp callback" );
    abort_the_connection_now = FALSE;
  }

  if (abort_the_connection_now)
  {
    if (hdramac_is_in_attempt(HDRHAI_IDLE_STATE_PROTOCOL))
    {
      /* We need to cancel this access attempt in order to stop the 
       * ConnectionRequest message from being sent.  When SLP calls
       * hdridle_conn_req_msg_tx_outcome_cb() with E_ABORTED we don't
       * re-queue the ConnectionRequest if we are aborting, and will 
       * instead give HDRIDLE_FINISH_ABORT_CON_SETUP_CMD which will put us 
       * back to MONITOR state.
       * If the ConnectionRequest gets through on the last probe, we again 
       * just let the connection setup finish and ALMP will close it if it 
       * succeeds.
       */
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Aborting Access for Connection Setup. Reason 0x%x",
                      abort_reason );
      hdramac_stop(HDRHAI_IDLE_STATE_PROTOCOL);
    }
    else
    {
      /* We have not yet started the access attempt, so just indicate 
         that we want to abort the connection and let SLP flush its queues.
      */   
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Aborting Connection Setup" );
      hdrplog_end_conn_attempt(HDRPLOG_CONN_ATT_USER_ABORT);

      /* Note that we have already verified that hdrrmac is not active,
       * so there is no risk of putting SRCH back to idle from connected.
       */
      hdrlmac_idle_mode( HDRHAI_IDLE_STATE_PROTOCOL );
      hdrind_give_ind(HDRIND_IDLE_CONNECTION_ABORTED, NULL);
      if (hdridles_get_state() == HDRIDLES_CONN_SETUP_SUSPEND_STATE)
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "OpenCon aborted while in ConSuspend State" );
        hdridles_change_state(HDRIDLES_SUSPEND_STATE);
      }
      else
      {
        hdridles_change_state(HDRIDLES_MONITOR_STATE);
      }
    }

    /* if there is a pending CallEndCnf, send it here */
    if (hdrcallid_deallocate_user_ended_call_id( &call_id ))
    {
      hdrmc_send_end_cnf( call_id );
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "CallEndCnf while ConSetup, callid %d",
                     call_id);
    }
  }
} /* hdridle_process_abort_open_conn_cmd() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_OPEN_CONN_CMD

DESCRIPTION
  This function processes an open connection command given to the
  the idle state protocol

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_open_conn_cmd
(
  hdrhai_open_reason_enum_type reason
)
{
#ifdef FEATURE_HDR_REVC

  uint8 fmac_sub_type = hdrscp_get_current_subtype(HDRHAI_FTC_MAC_PROTOCOL);
    /* FMAC subtype */

  boolean load_info_supported = FALSE;  
    /* AT supports LoadInformationSupported attribute or not */

  uint8 sub_type = hdrscp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
    /* IDLE subtype */

  boolean multi_at_page_supported = FALSE;  
    /* Whether AT supports MultiATPageMsgSupported attribute */

#endif /* FEATURE_HDR_REVC */

  hdridles_state_enum_type state = hdridles_get_state() ;
                                   /* current idle state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( state )
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIDLES_TUNNEL_STATE:

      if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
      {
        hdridle.conn_req_msg.req_reason = HDRIDLE_AT_INITIATED;
        hdridle.connect_reason = HDRIDLE_CR_ACTIVE_HO;
        hdridles_change_state(HDRIDLES_CONN_SETUP_STATE);
      }
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRIDLES_INACTIVE_STATE:

      /* ignore the request and log an error */
      ERR("IDLE-ignore open conn cmd",0,0,0);
      break;

    case HDRIDLES_CONN_SETUP_STATE:
    case HDRIDLES_CONN_SETUP_SUSPEND_STATE:

      /* connection setup is in progress; update the connect_reason */
      switch(reason)
      {
        case HDRHAI_OPEN_USER_REQ:
          /* set user abort bit to 0 */
          hdridle.connect_reason &= ~HDRIDLE_CR_ABORT;
          /* Or the current connect reason bit */
          hdridle.connect_reason |= HDRIDLE_CR_USER;
          break;

        case HDRHAI_OPEN_SEND_RTC_MSG:
          /* set user abort bit to 0 */
          hdridle.connect_reason &= ~HDRIDLE_CR_ABORT;
          /* Or the current connect reason bit */
          hdridle.connect_reason |= HDRIDLE_CR_MSG;
          break;

          default:
            break;
       }
       break;

    case HDRIDLES_MONITOR_STATE:
    case HDRIDLES_SLEEP_STATE:

      hdridle.conn_req_msg.req_reason = HDRIDLE_AT_INITIATED;

      switch(reason)
      {
        case HDRHAI_OPEN_USER_REQ:
          hdridle.connect_reason = HDRIDLE_CR_USER;
          break;

        case HDRHAI_OPEN_SEND_RTC_MSG:
          hdridle.connect_reason = HDRIDLE_CR_MSG;
          break;

#ifdef FEATURE_HDR_HANDOFF
        case HDRHAI_OPEN_NO_DATA:

          hdridle.connect_reason = HDRIDLE_CR_NO_DATA;
          break;
#endif /* FEATURE_HDR_HANDOFF */

          default:
            break;
       }

#ifdef FEATURE_HDR_REVC

      if (fmac_sub_type != HDRSCP_DEFAULT_FMAC)
      {
        (void) hdrscmdb_get_inuse_attrib(HDRHAI_FTC_MAC_PROTOCOL, 
                                         HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED,
                                         &load_info_supported,
                                         fmac_sub_type);

      /* Update LoadInformationSupported attribute if the following conditions
         are satisfied:
         1. LoadInformationSupported attribute is not supported by AT at
            present;
         2. AT is going to bring up a traffic channel due to data activity       
         3. AT has received a Load Information message from the current serving
            sector in either CONNECTED or IDLE state;
         4. after entering current subnet, the AN hasn't rejected the
            LoadInformationSupported attribute proposed (either by
            Config or GAUP) by the AT;
      */
        if ( !load_info_supported &&
             (hdridle.connect_reason & (HDRIDLE_CR_MSG|HDRIDLE_CR_NO_DATA)) == 0 &&
             hdrfmac_is_load_info_msg_rcvd() &&
             !hdrscmfmac_is_load_info_attrib_rejected() )
        {
          if ( hdrscmfmac_gaup_load_info_supported() != E_SUCCESS )
          {
            HDR_MSG_PROT( MSG_LEGACY_ERROR,
                          "LoadInfoSupported update request failed" );
          }
        }
      }

#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_REVC

      if (fmac_sub_type != HDRSCP_DEFAULT_FMAC)
      {
        (void) hdrscmdb_get_inuse_attrib(HDRHAI_FTC_MAC_PROTOCOL, 
                                         HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED,
                                         &load_info_supported,
                                         fmac_sub_type);

      /* Update LoadInformationSupported attribute if the following conditions
         are satisfied:
         1. LoadInformationSupported attribute is not supported by AT at
            present;
         2. AT is going to bring up a traffic channel due to data activity       
         3. AT has received a Load Information message from the current serving
            sector in either CONNECTED or IDLE state;
         4. after entering current subnet, the AN hasn't rejected the
            LoadInformationSupported attribute proposed (either by
            Config or GAUP) by the AT;
      */
        if ( !load_info_supported &&
             (hdridle.connect_reason & (HDRIDLE_CR_MSG|HDRIDLE_CR_NO_DATA)) == 0 &&
             hdrfmac_is_load_info_msg_rcvd() &&
             !hdrscmfmac_is_load_info_attrib_rejected() )
        {
          if ( hdrscmfmac_gaup_load_info_supported() != E_SUCCESS )
          {
            HDR_MSG_PROT( MSG_LEGACY_ERROR,
                          "LoadInfoSupported update request failed" );
          }
        }
      }

      if (sub_type != HDRSCP_DEFAULT_IDLE)
      {
        (void) hdrscmdb_get_inuse_attrib( HDRHAI_IDLE_STATE_PROTOCOL,                             
                                          HDRIDLE_MULTI_PAGE_SUPPORTED_ATTRIB_ID,
                                          &multi_at_page_supported,
                                          sub_type );

        /* Update MultiATPageMsgSupported attribute if the following conditions
           are satisfied:
         1. AT is going to bring up a traffic channel (already satisfied);         
         2. MultiATPageMsgSupported attribute is not supported by AT at
            present;      
         3. AT has received a MultiATPage message from the current serving
            sector in either CONNECTED or IDLE state;
         4. after entering current subnet, the AN hasn't rejected the
            MultiATPageMsgSupported attribute proposed (either by
            Config or GAUP) by the AT;
        */
        if ( hdrmc_feature_is_enabled(HDRMC_FEATURE_MULTI_AT_PAGE) && 
             !multi_at_page_supported &&
             hdridle_is_multi_at_page_msg_rcvd() &&
             !hdridle_is_multi_at_page_attrib_rejected() )
        {
          if ( hdrscmidle_gaup_multi_at_page_supported() != E_SUCCESS )
          {
            HDR_MSG_PROT( MSG_LEGACY_ERROR,
                          "MultiATPageMsgSupported update request failed" );
          }
        }
      }
#endif /* FEATURE_HDR_REVC */

      /* change to the connection setup state */
      hdridles_change_state(HDRIDLES_CONN_SETUP_STATE);
      break;

    case HDRIDLES_SUSPEND_STATE:
      switch(reason)
      {
        case HDRHAI_OPEN_USER_REQ:
          /* HDRIDLE_CF_SYS_LOST maps to CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED
           * The failure reason given to CM here must not cause immediate failure 
           * to DS.  Otherwise DS may retry immediately and the loop will overflow 
           * CM's queue. */
          hdridle_send_call_orig_fail_to_cm(HDRIDLE_CF_SYS_LOST);
          HDR_MSG_PROT( MSG_LEGACY_HIGH, "Ignoring Connection Setup while suspended");
          break;

        case HDRHAI_OPEN_SEND_RTC_MSG:
          hdridle.retry_open_conn_cmd = TRUE;
          HDR_MSG_PROT( MSG_LEGACY_HIGH, "Re-try open conn cmd after resume");
          break;

        default:
              HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Rxed conn Setup(%d) while Suspended",
                               reason);
                break;
      }
      break;
	  
    default:
      break;
  }
} /* hdridle_process_open_conn_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_DEACTIVATE_CMD

DESCRIPTION
  This function processes an deactivate command given to the
  the idle state protocol

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
   reason - reason for deactivation

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_deactivate_cmd
(
  hdrhai_abort_reason_enum_type reason
)
{

  if( hdridles_get_state() == HDRIDLES_INACTIVE_STATE )
  {
    /*-----------------------------------------------------------------------
      If the protocol is in the inactive state, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT( MSG_LEGACY_LOW, "IDLE Inactivate CMD ignored" );
    return;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    hdridles_change_state( HDRIDLES_INACTIVE_STATE );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  if ( hdridle_stop_conn_setup(reason) == SD_SS_ACT_CONTINUE )
  {
    hdridles_change_state( HDRIDLES_INACTIVE_STATE );
  }

} /* hdridle_process_deactivate_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_FINISH_ABORT_CON_SETUP_CMD

DESCRIPTION
  This function processes a FINISH_ABORT_CON_SETUP_CMD command given to the
  the idle state protocol.  This command is given after the ACMAC stops
  the access attempt to send the ConnectionRequest message.

DEPENDENCIES
  hdridle_init should have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_finish_abort_con_setup_cmd ( void )
{

  hdridles_state_enum_type cur_state = hdridles_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cur_state != HDRIDLES_CONN_SETUP_STATE) &&
       (cur_state != HDRIDLES_CONN_SETUP_SUSPEND_STATE) )
  {
    /*-----------------------------------------------------------------------
      If the protocol is not in connection setup state, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT( MSG_LEGACY_LOW, "finish_abort_con_setup_cmd ignored" );
    return;
  }

  if (hdridle.connect_reason != HDRIDLE_CR_ABORT)
  {
    ERR("connect_reason != HDRIDLE_CR_ABORT",0,0,0);
  }

  /* We were waiting for the ACMAC to stop to return to MONITOR state */
  HDR_MSG_PROT( MSG_LEGACY_MED, "Finish aborting after hdramac_stop()" );
  hdrplog_end_conn_attempt(HDRPLOG_CONN_ATT_USER_ABORT);


  if (cur_state == HDRIDLES_CONN_SETUP_STATE)
  {
    hdridles_change_state(HDRIDLES_MONITOR_STATE);
  }
  else /* if (cur_state == HDRIDLES_CONN_SETUP_SUSPEND) */
  {
    hdridles_change_state(HDRIDLES_SUSPEND_STATE);
  }

}  /* hdridle_process_finish_abort_con_setup_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_ACTIVATE_CMD

DESCRIPTION
  This function processes an activate command given to the
  the idle state protocol

DEPENDENCIES
  hdridle_init should have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_activate_cmd ( void )
{

  if( hdridles_get_state() != HDRIDLES_INACTIVE_STATE )
  {
    /*-----------------------------------------------------------------------
      If the protocol has been activated, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT( MSG_LEGACY_LOW, "IDLE Activate CMD ignored" );
    return;
  }

  /* We moved the resetting of lmac_suspend_or_resume_received 
   * flag from here to when ALMP leaves idle. Because Idle activate
   * will be called every time after session close, and when MUM 
   * mode is enabled, AT doesn't open a session again, so this flag 
   * will never be set to TRUE, preventing AT from going to sleep.
   */

  #ifdef FEATURE_HDR_LONG_SLEEP
  /* On activation start the long sleep timer early because RUP checks this
     to determine if it should perform distance based registration.  Otherwise,
     if RUP checks this timer before we try to sleep for the first time
     after acquisition, it will see the timer isn't running even though
     it really should be. */
  (void) rex_set_timer(&hdridle.long_sleep_delay_timer,
                HDRIDLE_LONG_SLEEP_DELAY_TIMEOUT);
  HDR_MSG_PROT( MSG_LEGACY_HIGH, "activate:reset 40s sleep delay 1 min" );
  #endif /* FEATURE_HDR_LONG_SLEEP */

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_IDLE_STATE_PROTOCOL ) &&
       !hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
  {
    hdridles_change_state( HDRIDLES_TUNNEL_STATE );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /*-----------------------------------------------------------------------
      Set the protocol state to monitor state.
    -----------------------------------------------------------------------*/
    hdridles_change_state( HDRIDLES_MONITOR_STATE);
  }

}  /* hdridle_process_activate_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CMD

DESCRIPTION
  This function processes commands given to the idleialization state protocol.

DEPENDENCIES
  None

PARAMETERS
  idle_cmd_ptr - Pointer to the command for the idleialization state protocol
            to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_cmd
(
  hdridle_cmd_type *idle_cmd_ptr
)
{


  HDRTRACE_CMD( HDRHAI_IDLE_STATE_PROTOCOL, idle_cmd_ptr->cmd,
    idle_cmd_ptr->sender, hdridles_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( idle_cmd_ptr->cmd )
  {
    case HDRIDLE_ACTIVATE_CMD:

      /* process the activate cmd */
      hdridle_process_activate_cmd();
      break;

    case HDRIDLE_DEACTIVATE_CMD:

      /* process the deactivate cmd */
      hdridle_process_deactivate_cmd(idle_cmd_ptr->data.abort_reason);
      break;

    case HDRIDLE_OPEN_CONN_CMD:

      /* process the open connection cmd */
      hdridle_process_open_conn_cmd(idle_cmd_ptr->data.open_reason);
      break;

    case HDRIDLE_ABORT_OPEN_CONN_CMD:

      /* process the abort open connection cmd */
      hdridle_process_abort_open_conn_cmd(idle_cmd_ptr->data.abort_reason);
      break;


    case HDRIDLE_CONFIG_CMD:

      /* start configuration */
      hdridlec_start_config();
      break;

    case HDRIDLE_UPDATE_PCCC_CMD:

      /* update pccc config  */
      hdridlec_update_config();
      break;

    case HDRIDLE_FINISH_ABORT_CON_SETUP_CMD:

      /* Finish aborting the connection setup */
      hdridle_process_finish_abort_con_setup_cmd();
      break;

    default:

      ERR ("Invalid commands for the IDLE protocol",0,0,0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
}  /* hdridle_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_MSG

DESCRIPTION
  This function processes messages for IDLE.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */

  boolean free_the_dsm_item = TRUE;
    /* Do we have to free the DSM item? */

#ifdef FEATURE_HDR_REVC
  boolean multi_at_page_supported = FALSE;
    /* Is MultiATPage msg supported? */

  uint16 sub_type = hdrscp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
    /* Subtype of IDLE state protocol */
#endif /* FEATURE_HDR_REVC */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /*-------------------------------------------------------------------------
   Extract the first byte of the received message. This is the message ID.
  -------------------------------------------------------------------------*/
  if ( dsm_extract( item_ptr, 0, &msg_id, 1 ) == 0 )
  {
    ERR( "Could not extract msg_id", 0, 0, 0 );
    dsm_free_packet(&item_ptr);
    return;
  }

  HDRTRACE_MSG( HDRHAI_IDLE_STATE_PROTOCOL, msg_id, hdridles_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  if (protocol_instance == HDRHAI_IN_USE)
  {
    switch ( msg_id )
    {
      case HDRIDLE_PAGE_ID:

        /*-------------------------------------------------------------------
           Page message
        -------------------------------------------------------------------*/
	HDRTRACE_IDLE_MSG_RX_PAGE(); /* Page msg received, log event */
		
        HDR_MSG_PROT( MSG_LEGACY_LOW,  "Page message received" );
#ifdef FEATURE_HDR_QCHAT
        if (hdrqchatupk_imo_enabled()) 
        {
          hdrqchatupk_amac_set_ani_conn(TRUE);
        }
#endif /*FEATURE_HDR_QCHAT*/        
        hdridle_process_page_msg( item_ptr );
        hdrplog_log_page_msg(item_ptr);
        break;

#ifdef FEATURE_HDR_REVC
      case HDRIDLE_MULTI_PAGE_ID:

        /* MultiATPage message received */
        HDR_MSG_PROT( MSG_LEGACY_MED, "MultiATPage Msg received" );
        hdridle_set_multi_at_page_msg_rcvd( TRUE );

        if ( sub_type != HDRSCP_DEFAULT_IDLE )
        {
          hdrscmdb_get_inuse_attrib( HDRHAI_IDLE_STATE_PROTOCOL,                             
                                     HDRIDLE_MULTI_PAGE_SUPPORTED_ATTRIB_ID,
                                     &multi_at_page_supported,
                                     sub_type );

          if ( multi_at_page_supported )
          {
            hdridle_process_multi_page_msg( item_ptr );
          }
          else
          {
            HDR_MSG_PROT( MSG_LEGACY_ERROR,
                          "Rcvd MultiATPage msg when not supported" );    

            /* When IDLE state protocol is not subtype-0 and
               MultiATPageMessageSupported attribute is not supported,
               attribute update request for MultiATPageMessageSupported will
               be sent out if the following conditions are met:
               1. AT has received a MultiATPage msg from the current serving
                  sector in either CONNECTED or IDLE state (always true here);
               2. after entering the current subnet, the AN hasn't rejected the
                  MultiATPageMessageSupported attribute proposed (either by
                  Config or GAUP) by the AT;
               3. AT is in CONNECTED state. 
            */
            if ( hdrmc_feature_is_enabled(HDRMC_FEATURE_MULTI_AT_PAGE) &&  
                 !hdridle_is_multi_at_page_attrib_rejected() &&
                 hdralmps_get_state() == HDRALMPS_CONNECTED )
            {
              if ( hdrscmidle_gaup_multi_at_page_supported() != E_SUCCESS)
              {
                HDR_MSG_PROT( MSG_LEGACY_ERROR,
                              "MultiPageSupported update request failed" );
              }
            }
          } /* !multi_at_page_supported */
        } /* sub_type != HDRSCP_DEFAULT_IDLE */
        else 
        {
          HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                          "Unexpected MultiATPage msg in subtype %d",
                          sub_type );
        } /* sub_type == HDRSCP_DEFAULT_IDLE */
        break;
#endif /* FEATURE_HDR_REVC */

      case HDRIDLE_CONN_DENY_ID:

        /*-------------------------------------------------------------------
           ConnectionDeny message
        -------------------------------------------------------------------*/
        HDR_MSG_PROT( MSG_LEGACY_LOW,  "ConnectionDeny message received" );
        hdridle_process_conn_deny_msg( item_ptr);
        break;

      case HDRIDLEC_ATTRIB_UPDATE_REQ_ID:
        hdridlec_process_msg( item_ptr, msg_id );
        break;

      case HDRIDLEC_ATTRIB_UPDATE_ACCEPT_ID:
      case HDRIDLEC_ATTRIB_UPDATE_REJECT_ID:

        hdrscm_process_msg(item_ptr, HDRHAI_IN_USE);

        /* SCM frees the DSM item after it is done. Don't free again. */
        free_the_dsm_item = FALSE;
        break;

      case HDRIDLE_QUICK_PAGE_ID:
        HDR_MSG_PROT( MSG_LEGACY_LOW,  "Quick Page message received" );
        hdridle_process_quick_page_msg( item_ptr );
        break;

      default:
        /*-------------------------------------------------------------------
          Default: message not recognized.
        -------------------------------------------------------------------*/
        ERR("Invalid IDLE in use msg %d",msg_id,0,0);
        break;
    }
  }
  else /* (protocol_instance == HDRHAI_IN_CONFIG) */
  {
    switch ( msg_id )
    {
      case HDRIDLEC_CONFIG_REQ_ID:
      case HDRIDLEC_CONFIG_RES_ID:

        hdridlec_process_msg( item_ptr, msg_id );
        break;

      default:
        /*-------------------------------------------------------------------
          Default: message not recognized.
        -------------------------------------------------------------------*/
        ERR("Invalid IDLE in config msg %d",msg_id,0,0);
        break;
    }
  }

  /* free the msg buffer */
  if (free_the_dsm_item)
  {
    dsm_free_packet(&item_ptr);
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
}  /* hdridle_process_msg() */


/* <EJECT> */
/*===========================================================================
FUNCTION HDRIDLE_PROCESS_SIG

DESCRIPTION
  This function processes signals for IDLE.

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdridle_process_sig
(
  rex_sigs_type sig
)
{
  hdridles_state_enum_type cur_state = hdridles_get_state();
                                       /* current idle state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( sig )
  {

    case HDRMC_IDLE_MONITOR_ENTERED_SIG:

      if((cur_state == HDRIDLES_MONITOR_STATE)&&(hdridle.idle_suspend_to_reacq == FALSE))
      {
        hdridle_check_ok_to_sleep();
      }
      break;

    default :
      ERR("Invalid IDLE protocol signal!",0,0,0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
}  /* hdridle_process_sig() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRIDLE_TIMER_CB

DESCRIPTION
  This function is the callback for all idle state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdridle_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_IDLE_STATE_PROTOCOL, timer_id);
} /* hdridle_timer_cb() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRIDLE_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the idle state
  protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdridle_process_timer
(
  uint32 timer_id
)
{
  hdridles_state_enum_type cur_state = hdridles_get_state();
                                       /* current idle state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( timer_id )
  {
    case HDRIDLE_STATE_TIMER:

      switch (cur_state)
      {
        case HDRIDLES_CONN_SETUP_STATE:
          HDRTRACE_HDRMC_IDLE_CON_SETUP_TIMEOUT();
          hdridle_process_conn_setup_timeout(HDRIDLE_CF_TCA_TIMEOUT);
          break;

        default:
          break;
      }
      break;

    case HDRIDLE_SUSPEND_TIMER:

      HDRTRACE_HDRMC_IDLE_SUSPEND_TIMER_SIG( cur_state );
      hdridle_process_suspend_timeout();
      break;

    case HDRIDLE_CONFIG_RES_TIMER:

      HDRTRACE_HDRMC_IDLE_NOT_RCV_CONFIG_RES_IN_2_SEC();
      hdridlec_process_config_res_timeout();
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
   case HDRIDLE_TUNNEL_CONN_TIMER:

      hdridle_process_tunnel_conn_timeout();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_LONG_SLEEP
    case HDRIDLE_LONG_SLEEP_DELAY_TIMER:
      /*do nothing */
      break;
#endif /* FEATURE_HDR_LONG_SLEEP */

#ifdef FEATURE_HDR_QCHAT
  case HDRIDLE_QCHAT_TIMER:
      if (hdrqchatupk_imo_enabled()) 
      {   
        hdrqchatupk_idle_process_qchat_timer();
      }
      break;
#endif /*FEATURE_HDR_QCHAT*/

    default :
      ERR("Invalid IDLE protocol signal!",0,0,0);
      break;
  }
}  /* hdridle_process_timer() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RESET_SLEEP_SCHEDULE

DESCRIPTION
  This function resets the Enhanced Idle State Protocol slotted sleep 
  schedule to begin at the current system time.  After this, the AT will
  start using SlotCycle1 for WakeCount1 period, then switch to SlotCycle2,
  etc., unless this routine is called again.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridle_reset_sleep_schedule ( void )
{
  qword current_system_time_qw; /* current system time in milliseconds */
  hdridlec_slotted_mode_type slotted_mode_attrib; /* in-use SlottedMode */
  uint64 period1; /* Duration of SlotCycle1 in nanoseconds */
  uint64 period2; /* Duration of SlotCycle2 in nanoseconds */
  uint64 pccc_ns; /* Offset of PCCC from slot 0, in nanoseconds */
  uint64 t_c;  /* current system time in nanoseconds */
  uint64 t_12; /* system time when to change from SlotCycl1 to SlotCycle2 */
  uint64 t_23; /* system time when to change from SlotCycl2 to SlotCycle3 */
  uint8 wake_count1; /* Shorthand to keep the equations readable */
  uint8 wake_count2; /* Shorthand to keep the equations readable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  
      t_12 = t_c  + period1 - ((t_c  + 256 * R) % period1) + 
             period1 * (24 * (wake_count1 + 1) - 1)
      t_23 = t_12 + period2 - ((t_12 + 256 * R) % period2) + 
             period2 * (24 * (wake_count2 + 1) - 1)
    
      256 * R is our control channel cycle in slots
  */

  if (hdridlec_get_slotted_mode_attrib( &slotted_mode_attrib ))
  {
    wake_count1 = slotted_mode_attrib.wake_count1;
    wake_count2 = slotted_mode_attrib.wake_count2;

    /* The calculation is done in units of nanoseconds to keep percision */

    period1 = hdrutil_sci_to_ns( slotted_mode_attrib.slot_cycle1 );
    period2 = hdrutil_sci_to_ns( slotted_mode_attrib.slot_cycle2 );
  
    pccc_ns = (uint64) hdridlec_get_current_pccc() * (uint64) (256 * 1666);

    time_get_ms( current_system_time_qw );
    t_c = ((uint64) qw_hi(current_system_time_qw) << 32) | 
           qw_lo(current_system_time_qw);
    t_c *= 1000; /* convert to nanoseconds */
  
    t_12 = t_c  + period1 - ((t_c  + pccc_ns) % period1) +
           period1 * (24 * (wake_count1 + 1) - 1);
  
    t_23 = t_12 + period2 - ((t_12 + pccc_ns) % period2) +
           period2 * (24 * (wake_count2 + 1) - 1);

    hdridle.slot_cycle1_timeout = t_12 / 1000; /* convert back to ms */
    hdridle.slot_cycle2_timeout = t_23 / 1000; /* convert back to ms */

    hdridlec_set_idle_slot_cycle_timeout( hdridle.slot_cycle1_timeout,
                                          hdridle.slot_cycle2_timeout );
    HDR_MSG_PROT_6( MSG_LEGACY_HIGH, "SCI Reset. Tc=%x:%x T1=%x:%x, T2=%x:%x",
                    qw_hi(current_system_time_qw), qw_lo(current_system_time_qw),
                    (uint32) (hdridle.slot_cycle1_timeout >> 32),
                    (uint32) hdridle.slot_cycle1_timeout,
                    (uint32) (hdridle.slot_cycle2_timeout >> 32),
                    (uint32) hdridle.slot_cycle2_timeout );
  }
  else
  {
    /* No attribute! */
    hdridle.slot_cycle1_timeout = 0;
    hdridle.slot_cycle2_timeout = 0;
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "SCI Reset w/ wrong subtype %d",
                    hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL) );
  }
} /* hdridle_reset_sleep_schedule() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRIDLE_GET_CURRENT_SCI

DESCRIPTION
  This function computes the current slot cycle based on the Enhanced Idle 
  State Protocol slotted sleep schedule.

DEPENDENCIES
  None

PARAMETERS
  gear_ptr - Return which SlotCycle value is chosen, for debug

RETURN VALUE
  The proper slot cycle to use

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdridle_get_current_sci ( int *gear_ptr )
{
  qword current_system_time_qw; /* current system time */
  uint64 current_system_time; /* converted to a uint64 */
  hdridlec_slotted_mode_type slotted_mode_attrib; /* in-use attrib val */
  uint8 slot_cycle = 0x09; /* Default, 5.12s */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  time_get_ms( current_system_time_qw );
  current_system_time = ((uint64) qw_hi(current_system_time_qw) << 32) | 
                         qw_lo(current_system_time_qw);

  if ( hdridlec_get_slotted_mode_attrib( &slotted_mode_attrib ) &&
       (hdridle.slot_cycle1_timeout != 0)&&(hdridle.slot_cycle2_timeout != 0))
  {
    if (current_system_time < hdridle.slot_cycle1_timeout)
    {
      slot_cycle = slotted_mode_attrib.slot_cycle1;
      *gear_ptr = 1;
    }
    else if (current_system_time < hdridle.slot_cycle2_timeout)
    {
      slot_cycle = slotted_mode_attrib.slot_cycle2;
      *gear_ptr = 2;
    }
    else
    {
      slot_cycle = slotted_mode_attrib.slot_cycle3;
      *gear_ptr = 3;
    }
  }

  return slot_cycle;
} /* hdridle_get_current_sci() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CC_PKT_RXED_IND

DESCRIPTION
  This function processes Control Channel header information 

DEPENDENCIES
  Messages will be queued to event queue before indication. 
  For example, we will know if QC msg has been received or not 
  before receiving this indication.

PARAMETERS
  ind_name     - Name of the indication for IDLE to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_cc_pkt_rxed_ind
(
  hdrind_ind_name_enum_type         ind_name,
  hdrind_ind_data_union_type        *ind_data_ptr
)
{
  /* indication data must be pointing to a valid address */
  ASSERT(ind_data_ptr != NULL);

#ifdef FEATURE_HDR_CC_DECODE_FAIL
  /* Reset the reacq failure/cc decode fail counter, when CC decode success */
  hdridle.num_cc_reacq_failed = 0;
#endif

  hdridle.intra_scc.continued_scc_pkt_expected = !(ind_data_ptr->intra_scc_info.last_pkt);
  hdridle.intra_scc.cc_start = ind_data_ptr->intra_scc_info.cc_start;
  hdridle.sleep_capsule_done = ind_data_ptr->intra_scc_info.sleep_caps_done;
  hdridle.rcv_end_of_sync_cap_ind = ind_data_ptr->intra_scc_info.last_pkt;

  /* We need to remember if we have received QC in the Sync Capsule
   * it will be used later to determine if we can go to normal sleep
   * in case of receiving CC_MISSED indication.
   */
  if (hdrovhd_qc_is_received())
  {
    /* If we already received QC message, we can set sleep capsule done 
     * to TRUE in the CC MISSED case.
     * Note: above statement is true only in CC MISSED case, in other cases,
     *       even we received QC, we still need to wait for Page message
     */
    hdridle.qc_is_rxed_in_this_scc = TRUE;
  }

  HDR_MSG_PROT_5( MSG_LEGACY_HIGH, "expected %d, done %d, cc_start %d, end scc %d, qc %d",
                  hdridle.intra_scc.continued_scc_pkt_expected,
                  hdridle.sleep_capsule_done,
                  hdridle.intra_scc.cc_start,
                  hdridle.rcv_end_of_sync_cap_ind, 
                  hdridle.qc_is_rxed_in_this_scc);

} /* hdridle_process_cc_pkt_rxed_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_LMAC_CC_MISSED_IND

DESCRIPTION
  This function processes Control Channel header information 

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for IDLE to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_lmac_cc_missed_ind(void)
{
  /* We reuse the end of sync CC rcved flag for a missed sync
     CC since we should go to 426ms sleep too in this case.  However,
     we won't mark the sleep_capsule done flag as updated because we want
     to make sure that we don't go to long sleep in this case unless QC 
     message was received. */
  hdridle.rcv_end_of_sync_cap_ind = TRUE;
  hdridle.intra_scc.continued_scc_pkt_expected = FALSE;

#ifdef FEATURE_HDR_CC_DECODE_FAIL
    hdridle.num_cc_reacq_failed++;
     HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                 "CC/Re-Acq failed for %d times so far",hdridle.num_cc_reacq_failed);
    if(hdridle.num_cc_reacq_failed == 1 && /* cc decode failed for the first time */
       hdridle.last_sci <= 9)/* less than equal 5.12s sleep */
    {
	hdridle.sleep_capsule_done = TRUE;
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Enabled condition for SCI sleep");
    }
#else
  if (hdridle.qc_is_rxed_in_this_scc)
  {
    /* If we already received QC message, we can set sleep capsule done to TRUE
     * Note: above statement is true only in CC MISSED case, in other cases,
     *       even we received QC, we still need to wait for Page message
     */
    hdridle.sleep_capsule_done = TRUE;
  }
#endif /* FEATURE_HDR_CC_DECODE_FAIL */
} /* hdridle_process_lmac_cc_missed_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_IND

DESCRIPTION
  This function processes indications given to IDLE.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for IDLE to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_ind
(
  hdrind_ind_name_enum_type         ind_name,
  hdrind_ind_data_union_type        *ind_data_ptr
)
{
#ifdef FEATURE_COEX_RPT_RAT_L3_STATE
  cxm_tech_l3_state_s l3_state_s;
  /* Struct to hold RAT's L3 state */
#endif
  
  hdridles_state_enum_type cur_state = hdridles_get_state();

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_open_reason_enum_type open_reason;
  hdrind_idle_conn_fail_info_type conn_fail_info;
  conn_fail_info.failure_reason = 0;
  /*KW fix:Initiialize the variable so that Oring can happen successfully later*/
#endif /* FEATURE_LTE_TO_HDR_OH */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ind_name != HDRIND_CCMAC_SCC_PKT_RXED) &&
    (ind_name != HDRIND_LMAC_AWAKE))
  {
    HDRTRACE_IND( HDRHAI_IDLE_STATE_PROTOCOL, ind_name, cur_state );
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
    case HDRIND_RTCMAC_LINK_ACQUIRED:

      hdridle_process_link_acquired_ind(ind_name, ind_data_ptr);
      break;

    case HDRIND_RUP_CONN_SETUP_FAILED:

      hdridle_process_conn_setup_failed_ind(ind_name, ind_data_ptr);
      break;

    case HDRIND_OVHD_INFO_UPDATED:
    case HDRIND_OVHD_AP_UPDATED:
      hdridle_process_sleep_related_ind(ind_name);
      break;

    case HDRIND_CONNECTED_CONNECTION_CLOSED:
    case HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL:
      /* After negotiation we get ConnectionClosed before the new 
       * SlottedMode attribute is stored, so we do this again later,
       * when the new attribute is stored.
       */
      hdridle_reset_sleep_schedule();
      break;

	case HDRIND_ACMAC_TX_STARTED:
	  #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
	  	/* This indication is given when the access channel is 
	   	starting the access attempts. Here, we report RAT's
	   	L3 state as ACCESS to CXM.
	   	*/	  
	 	l3_state_s.mode = CXM_TECH_ACTIVE;
        	l3_state_s.state = CXM_TECH_L3_ACCESS;
          	cxm_set_tech_l3_state(CXM_TECH_HDR, &l3_state_s);
	  #endif
	  break;

    case HDRIND_ACMAC_TX_ENDED:
	  #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
	  	/* 
	 	This indication indicates that the access channel is inactive.
	 	set RAT's layer3 state as IDLE and report it. Ignore sending IDLE to CXM
	 	if connection setup is proceeding.	   
	 	*/  	  
	 
	 	if( hdridles_get_state() != HDRIDLES_CONN_SETUP_STATE )
	 	{
	   	   l3_state_s.mode = CXM_TECH_ACTIVE;
     	    	   l3_state_s.state = CXM_TECH_L3_IDLE;
      	     	   cxm_set_tech_l3_state(CXM_TECH_HDR, &l3_state_s);
	 	}
	  #endif

#ifdef FEATURE_HDR_LONG_SLEEP
      /* After an access extend or restart the long sleep timer
        so we monitor all our slots,
         in case the AT decides to re-CHAP us */
      (void) rex_set_timer(&hdridle.long_sleep_delay_timer,
                    HDRIDLE_LONG_SLEEP_DELAY_TIMEOUT);
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "tx_ended:reset 40s sleep delay 1 min" );
#endif /* FEATURE_HDR_LONG_SLEEP */
 
      /* Idle doesn't know when AT starts an access attempt.
       * This indication is actually when AT finishes the access. */ 
      hdridle.access_since_entering_monitor = TRUE;

      hdridle_process_sleep_related_ind(ind_name);
      break;

    case HDRIND_LMAC_CC_MISSED:
      hdridle_process_lmac_cc_missed_ind();
      hdridle_process_sleep_related_ind(ind_name);
      break;

#ifdef FEATURE_HDR_BCMCS
    case HDRIND_BCMCS_SLEEP_OK:
      hdridle_process_sleep_related_ind(ind_name);
      break;
#endif /* FEATURE_HDR_BCMCS */

    case HDRIND_CCMAC_SCC_PKT_RXED:
      hdridle_process_cc_pkt_rxed_ind(ind_name, ind_data_ptr);
      hdridle_process_sleep_related_ind(ind_name);
      break;

    case HDRIND_LMAC_AWAKE:
	  if((hdridle.idle_suspend_to_reacq == TRUE)&&
         (cur_state == HDRIDLES_MONITOR_STATE)) /* this can happen only when IDLE in MONITOR */
      {
	     /* while SRCH comes from 3f -> 3 and earlier 3a -> 3f must have happened */
	     hdridle.idle_suspend_to_reacq = FALSE;
		 HDR_MSG_PROT (MSG_LEGACY_HIGH, "idle_suspend_to_reacq is reset to FALSE. HDRIND_LMAC_AWAKE is recieved.");
         hdridle_check_ok_to_sleep(); /* Earlier check ok to sleep is not done. So check now */
      }	   
	  else
      {         
         hdridle_process_wakeup_ind(ind_name, ind_data_ptr);
      }	
      break;

    case HDRIND_LMAC_IDLE_SUSPEND:

      hdridle.lmac_suspend_or_resume_received = TRUE;

      if (cur_state == HDRIDLES_MONITOR_STATE)
      {
        hdridles_change_state( HDRIDLES_SUSPEND_STATE );
      }
#ifdef FEATURE_HDR_HYBRID_ACCESS
      /* If we are in the process of sending a ConnectionRequest message but
         we haven't received an ACK for it yet, then we transition to 
         conn_setup suspended state.  If we've already received an ACK for it,
         we stay in CONN_SETUP state (state timer is running) */
      else if ( (cur_state == HDRIDLES_CONN_SETUP_STATE) &&
                (hdridle.conn_req_msg.awaiting_slp_callback) )
      {
        hdridles_change_state( HDRIDLES_CONN_SETUP_SUSPEND_STATE );
      }
#endif /* FEATURE_HDR_HYBRID_ACCESS */
      else
      {
        ERR("SUSPEND received in %d state",cur_state,0,0);
      }
      break;

	case HDRIND_LMAC_IDLE_SUSPEND_TO_REACQ:
      if (cur_state == HDRIDLES_SUSPEND_STATE) /* this indication can come only during 3a->3f. Hence
                                                  IDLE current state has to be IDLE SUSPEND */
      {
        hdridle.idle_suspend_to_reacq = TRUE; /* set this flag and continue processing IDLE RESUME */
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Setting idle_suspend_to_reacq when idle is in %d state",hdridles_get_state());
      }
      else
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "HDRIND_LMAC_IDLE_SUSPEND_TO_REACQ rcvd when idle is in %d state",hdridles_get_state());
      }
	  
    case HDRIND_LMAC_IDLE_RESUME:

      /* LMAC also sends "resume" if we enter idle and aren't suspending */
      hdridle.lmac_suspend_or_resume_received = TRUE;

      if (cur_state == HDRIDLES_SUSPEND_STATE)
      {
        hdridles_change_state( HDRIDLES_MONITOR_STATE );
        if (hdridle.retry_open_conn_cmd)
        {
          if (!hdrhmp_pending_queue_is_empty(HDRHAI_IDLE_STATE_PROTOCOL))
          {
            hdridle_open_connection(HDRHAI_OPEN_SEND_RTC_MSG,
                                     HDRHAI_IDLE_STATE_PROTOCOL);
          }
          hdridle.retry_open_conn_cmd = FALSE;
        }
      }
#ifdef FEATURE_HDR_HYBRID_ACCESS
      else if (cur_state == HDRIDLES_CONN_SETUP_SUSPEND_STATE)
      {
        hdridles_change_state( HDRIDLES_CONN_SETUP_STATE );
      }
#endif /* FEATURE_HDR_HYBRID_ACCESS */
      break;

    case HDRIND_SMP_SESSION_CLOSED:
    case HDRIND_SMP_SESSION_OPENED:
      hdridlec_process_ind(ind_name,ind_data_ptr );
      break;

    case HDRIND_SCM_SESSION_CHANGED:
    case HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED:
      hdridlec_process_ind(ind_name,ind_data_ptr );
      hdridle_reset_sleep_schedule();
      break;

#ifdef FEATURE_IRAT_PILOT_MEAS
    case HDRIND_LMAC_IRAT_PLT_MEAS_DONE_IND:
      hdridle_process_sleep_related_ind(ind_name);
      break;
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAP_CONNECTION_OPENED:
    case HDRIND_SAP_CONNECTION_ALREADY_OPENED:

      if( ind_data_ptr->sap_open_reason & HDRSAP_CR_USER )
      {
        open_reason = HDRHAI_OPEN_USER_REQ;
      }
      else if ( ind_data_ptr->sap_open_reason & HDRSAP_CR_AN )
      {
        open_reason = HDRHAI_OPEN_AN;
      }
      else if ( ind_data_ptr->sap_open_reason & HDRSAP_CR_MSG )
      {
        open_reason = HDRHAI_OPEN_SEND_RTC_MSG;
      }
      else if ( ind_data_ptr->sap_open_reason & HDRSAP_CR_ABORT )
      {
        open_reason = HDRHAI_OPEN_ABORT;
      }
      else
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "Error in connect reason %d",
                        ind_data_ptr->sap_open_reason );
        open_reason = HDRHAI_OPEN_UNSPECIFIED;
      }

      if ( ind_name == HDRIND_SAP_CONNECTION_OPENED )
      {
        /* The SAP connection reason macros is the same as IDLE */
        HDRTRACE_IDLE_CONNECT_REASON( ind_data_ptr->sap_open_reason ); 
        hdrind_give_ind( HDRIND_IDLE_CONNECTION_OPENED, (void *)&open_reason );
      }
      else if ( ind_name == HDRIND_SAP_CONNECTION_ALREADY_OPENED )
      {
        hdrind_give_ind( HDRIND_IDLE_CONNECTION_ALREADY_OPENED, 
                         (void *)&open_reason );
      }
      break;

    case HDRIND_SAP_CONNECTION_FAILED:
      if ( ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRHAI_CONN_FAIL_SAP_ACTIVE_HO ) || 
           ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRHAI_CONN_FAIL_SAP_NO_SRV ) || 
           ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRHAI_CONN_FAIL_SAP_PREREG_NOT_ALLOW ) || 
           ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRHAI_CONN_FAIL_SAP_NO_SESSION ) ||
           ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRHAI_CONN_FAIL_SAP_NOT_ACTIVE ) )
      {
        /* When the above failure happens, HDR is not in the state to open a
         * connetion. Do nothing but notify DS connection open fails. */
        conn_fail_info.failure_reason = ind_data_ptr->conn_fail_info.failure_reason;
        conn_fail_info.backoff_time = 0;
        hdrind_give_ind( HDRIND_IDLE_CONNECTION_NOT_ALLOWED, (void*)&conn_fail_info );
      }
      else
      {
        if ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRHAI_CONN_FAIL_SAP_L2_FAILED )
        {
          conn_fail_info.failure_reason |= HDRIDLE_CONN_FAIL_SAP_L2_FAILED_MASK;
          conn_fail_info.backoff_time = 0;
        }
        else
        {
          /* In tunnel mode, L3 failure is simialr to "TCA" or "RTCAck"
           * timedout. After 5 failures, we will run "session backoff"
           * algorithm (similar to "avoidance" in native mode). 
           */
          conn_fail_info.failure_reason |= HDRIDLE_CONN_FAIL_SAP_L3_FAILED_MASK;
          conn_fail_info.backoff_time = 0;
        }
        hdrind_give_ind( HDRIND_IDLE_CONNECTION_FAILED, (void*)&conn_fail_info );
      }
      break;

#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_REVC
    case HDRIND_SCM_IDLE_AT_GAUP_REJECTED:
      if ( hdrscmidle_is_multi_at_page_gaup(ind_data_ptr->gaup_trans_id) ) 
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH,
                      "MultiATPageMsgSupported update req rejected by AN" );
        hdridle_set_multi_at_page_attrib_rejected( TRUE );
      }
      break;

    case HDRIND_SCM_IDLE_AT_GAUP_FAILED:
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "update req failed" );
      break;
#endif /* FEATURE_HDR_REVC */

    default:
      ERR("Invalid IDLE protocol ind!",0,0,0);
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
}/* hdridle_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRIDLE.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdridle_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void                      *ind_data_ptr
)
{
  /* This protocol runs in the HDRMC task.*/
  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* Indication is from another protocol in MC task, so consider it
     * an internal indication
     */
    hdridle_process_ind( ind_name,
      (hdrind_ind_data_union_type *)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_IDLE_STATE_PROTOCOL, ind_name,
      ind_data_ptr );
  }
} /* hdridle_ind_cb() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRIDLE.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_msg_cb
(
  dsm_item_type *item_ptr
)
{
  /* This protocol runs in the HDRMC task.*/
  hdrmc_queue_msg( HDRHAI_IDLE_STATE_PROTOCOL, HDRHAI_IN_USE, item_ptr );
} /* hdridle_msg_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRIDLE in-config instance.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_config_msg_cb
(
  dsm_item_type *item_ptr
)
{
  /* This protocol runs in the HDRMC task.*/
  hdrmc_queue_msg( HDRHAI_IDLE_STATE_PROTOCOL, HDRHAI_IN_CONFIG, item_ptr );
} /* hdridle_config_msg_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_OPEN_CONNECTION

DESCRIPTION
  This function commands the idle state protocol to open a connections.

DEPENDENCIES
  None.

PARAMETERS
  reason - Reason to open a connection
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_open_connection
(
  hdrhai_open_reason_enum_type   reason,
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1( MSG_LEGACY_LOW, "IDLE Open Conn reason=%d", reason );

  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdridle_cmd.cmd              = HDRIDLE_OPEN_CONN_CMD;
  hdridle_cmd.sender           = sender;
  hdridle_cmd.data.open_reason = reason;


  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    /* Currently no other task ever activates IDLE, so we should never
     * get here
     */
    ERR("IDLE activated from outside MC!", 0, 0, 0);

    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }

} /* hdridle_open_connection() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_ACTIVATE

DESCRIPTION
  This function commands the idleialization state protocol to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_activate
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW, "Activate IDLE" );

  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdridle_cmd.cmd = HDRIDLE_ACTIVATE_CMD;
  hdridle_cmd.sender = sender;

  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    /* Currently no other task ever activates IDLE, so we should never
     * get here
     */
    ERR("IDLE activated from outside MC!", 0, 0, 0);

    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }

} /* hdridle_activate() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_ABORT_OPEN_CONNECTION

DESCRIPTION
  This function commands the idle state protocol to abort
  open connection request.

DEPENDENCIES
  None.

PARAMETERS
  reason  - reason for aborting the open connection
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_abort_open_connection
(
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1( MSG_LEGACY_LOW, "IDLE Abort Open Conn %d", reason );

  hdridle_cmd.cmd               = HDRIDLE_ABORT_OPEN_CONN_CMD;
  hdridle_cmd.sender            = sender;
  hdridle_cmd.data.abort_reason = reason;

  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    /* Currently no other task ever deactivates IDLE, so we should never
     * get here
     */
    ERR("IDLE deactivated from outside MC!", 0, 0, 0);

    /*-------------------------------------------------------------------------
        Fill in the command ID
    -------------------------------------------------------------------------*/

    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }
} /* hdridle_abort_open_connection() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_DEACTIVATE

DESCRIPTION
  This function commands the idle state protocol to deactivate.

DEPENDENCIES
  None.

PARAMETERS
  reason  - reason for the deactivation

  sender - The protocol giving this command (only for software trace)
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_deactivate
(
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW, "Deactivate IDLE" );

  hdridle_cmd.cmd               = HDRIDLE_DEACTIVATE_CMD;
  hdridle_cmd.sender            = sender;
  hdridle_cmd.data.abort_reason = reason;

  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    /* Currently no other task ever deactivates IDLE, so we should never
     * get here
     */
    ERR("IDLE deactivated from outside MC!", 0, 0, 0);

    /*-------------------------------------------------------------------------
        Fill in the command ID
    -------------------------------------------------------------------------*/

    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }
}/* hdridle_deactivate() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_FINISH_ABORT_CON_SETUP

DESCRIPTION
  This function commands the idle state protocol to finish aborting the
  connectino setup.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_finish_abort_con_setup
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW, "Finish aborting con setup" );

  hdridle_cmd.cmd               = HDRIDLE_FINISH_ABORT_CON_SETUP_CMD;
  hdridle_cmd.sender            = sender;

  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    ERR("HDRIDLE_FINISH_ABORT_CON_SETUP_CMD given from HDRMC task",0,0,0);
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }
}/* hdridle_finish_abort_con_setup() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CONFIGURE

DESCRIPTION
  This function queries the Idle State Protocol whether configuration
  negotiation is needed or not.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_configure
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdridlec_config_is_needed())
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "IDLE Config is not required" );
    return FALSE;
  }

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "IDLE Config is required" );

  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdridle_cmd.cmd     = HDRIDLE_CONFIG_CMD;
  hdridle_cmd.sender  = sender;

  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    /* Currently no other task ever call Idle command fucntion,
       so we should never get here
     */
    ERR("IDLE config from outside MC!", 0, 0, 0);


    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }

  return TRUE;
} /* hdridle_configure() */


/*===========================================================================

FUNCTION HDRIDLE_UPDATE_PREF_CNTL_CHAN_CYCLE

DESCRIPTION
  This function commands the idle state protocol tto update
  the preferred control channel cycle.


DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_update_pref_cntl_chan_cycle
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdridle_cmd_type hdridle_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW, "IDLE update PCCC" );
  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdridle_cmd.cmd               = HDRIDLE_UPDATE_PCCC_CMD;
  hdridle_cmd.sender            = sender;

  if (rex_self() == HDRIDLE_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdridle_process_cmd( &hdridle_cmd );
  }
  else
  {
    /* Currently no other task ever deactivates IDLE, so we should never
     * get here
     */
    ERR("IDLE pgslot change is called from outside MC!", 0, 0, 0);

    hdrmc_queue_cmd( HDRHAI_IDLE_STATE_PROTOCOL,
                     &hdridle_cmd,
                     sizeof(hdridle_cmd_type) );
  }
}/* hdridle_update_pref_cntl_chan_cycle() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization of the idle state protocol.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_powerup_init ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW, "IDLE powerup init" );

  if ( hdrmc_is_first_init() )
  {
    /* Initialize timers. */
    rex_def_timer_ex( &hdridle.suspend_timer,
                      hdridle_timer_cb,
                      HDRIDLE_SUSPEND_TIMER );

    rex_def_timer_ex( &hdridle.state_timer,
                      hdridle_timer_cb,
                      HDRIDLE_STATE_TIMER );

#ifdef FEATURE_LTE_TO_HDR_OH
    rex_def_timer_ex( &hdridle.tunnel_conn_timer,
                      hdridle_timer_cb,
                      HDRIDLE_TUNNEL_CONN_TIMER );
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_LONG_SLEEP
    rex_def_timer_ex( &hdridle.long_sleep_delay_timer,
                      hdridle_timer_cb,
                      HDRIDLE_LONG_SLEEP_DELAY_TIMER );
#endif /* FEATURE_HDR_LONG_SLEEP */
  }

#ifdef FEATURE_HDR_QCHAT
  hdrqchatupk_idle_powerup_init();
#endif /*FEATURE_HDR_QCHAT*/

  /* initialize the control block */
  hdridle.conn_req_msg.msg_id = HDRIDLE_CONN_REQ_ID; 
  hdridle.conn_req_msg.trans_id = 0 ;
  hdridle.conn_req_msg.awaiting_slp_callback = FALSE;

  hdridlec_powerup_init();

  hdridlec_get_idle_slot_cycle_timeout( &hdridle.slot_cycle1_timeout,
                                        &hdridle.slot_cycle2_timeout );

  hdridle.sci_override = hdridle_get_override_sci();
  hdridle.data_session_is_on_hdr = FALSE;

  /* Init ConfigurationChange to a default value */
  hdridle.config_change = 0xFF;

  /* Init force_long_sleep flag */
  hdridle.force_long_sleep = FALSE;

  /* Init retry_open_conn_cmd falg */
  hdridle.retry_open_conn_cmd = FALSE;

} /* hdridle_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_INIT

DESCRIPTION
  This function initializes the idle state protocol.

DEPENDENCIES
  This function must called every time when the acess terminal enters the HDR
  mode and before the idle state protocol can be activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_init ( void )
{
#ifdef FEATURE_HDR_DDARF
  hdridle.num_reacq_failed = 0;
  hdridle.last_sci = 6;
#endif
#ifdef FEATURE_HDR_CC_DECODE_FAIL
  hdridle.num_cc_reacq_failed = 0;
#endif
  hdridle.idle_suspend_to_reacq = FALSE;
  hdridles_init();
} /* hdridle_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RESELECT_IS_ALLOWED

DESCRIPTION
  This function checks whether re-selection is allowed or not

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE        - Relection is allowed.
  FALSE       - Relection is not allowed.
SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_reselect_is_allowed ( hdrhai_protocol_name_enum_type caller )
{

  boolean                  ret_val = TRUE; /* boolean flag */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(hdramac_is_in_attempt(HDRHAI_IDLE_STATE_PROTOCOL)              ||
      hdrrup_is_tc_assigned(HDRHAI_IDLE_STATE_PROTOCOL)             ||
     (hdrslp_tx_queue_is_empty( HDRHAI_IDLE_STATE_PROTOCOL)==FALSE) ||
     (hdrhmp_tx_queue_is_empty( HDRHAI_IDLE_STATE_PROTOCOL)==FALSE) ||
     (hdridles_get_state() == HDRIDLES_CONN_SETUP_STATE)            
#ifdef FEATURE_HDR_BCMCS
                                                                    ||
     (hdrbccp_are_any_flows_monitored()) 
#endif /* FEATURE_HDR_BCMCS */
     )
  {
     /* re-selection is not allowed */
     ret_val = FALSE;
  }

  HDRTRACE_IDLE_RESELECT_IS_ALLOWED( caller, hdridles_get_state(), ret_val );
  return ret_val;

} /* hdridle_reselect_is_allowed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SET_CONNECT_REASON

DESCRIPTION
  This function is used to set the connection setup reason

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  reason   - connection setup reason

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_set_connect_reason
(
  uint8 reason
)
{
  hdridle.connect_reason |= reason;
}/* hdridle_set_connect_reason () */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_ENTER_MONITOR

DESCRIPTION
  This function processes the entering of Monitor state. 

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  come_from_sleep - Whether the state transition is from Sleep state.

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_enter_monitor
(
  boolean come_from_sleep
)
{
  /* If Monitor state was entered from Sleep state, set the flag
   * for the end of sync capsule indication to be FALSE. Otherwise,
   * set it to be TRUE */
  if ( come_from_sleep )
  {
    hdridle_set_rcv_end_of_sync_cap_ind( FALSE );
  }
  else
  {
    hdridle_set_rcv_end_of_sync_cap_ind( TRUE );
  }

  /* Idle enters Monitor state. Clear the access flag since 
   * entering Monitor state. */
  hdridle.access_since_entering_monitor = FALSE;

} /* hdridle_enter_monitor() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_SET_FIRST_SLEEP_FLAG

DESCRIPTION
  This function set the flag that indicates if the idle state protocol has
  entered sleep since it last entered monitor state, either from some state
  other than sleep.

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  flag   - TRUE or FALSE

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_set_first_sleep_flag(boolean flag)
{
  hdridle.first_sleep = flag;
} /* hdridle_set_first_sleep_flag() */

/* <EJECT> */
#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRIDLE_SEND_CONN_REQ_MSG

DESCRIPTION
  This function sends the connection request message to the access
  network

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_send_conn_req_msg ( void )
{

  dsm_item_type  *dsm_ptr; /* msg buffer pointer */
  uint8 offset = 0; /* for packing channel record */
  uint8 num_pref_chan;  /* number of preferred channels */
  hdrhai_protocol_name_enum_type  curr_subtype = 
      (hdrhai_protocol_name_enum_type)
      hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_ptr = hdrutil_new_msg_buffer();

  /* hdrutil_new_buffer will always get a buffer
     if it can not get buffer it will call ERR_FATAL
  */

  HDRIDLE_PACK_CON_REQ_MSG_ID(dsm_ptr,hdridle.conn_req_msg);
  hdridle.conn_req_msg.trans_id++;
  hdridle.conn_req_msg.awaiting_slp_callback = TRUE;
  HDRIDLE_PACK_CON_REQ_TRANS_ID(dsm_ptr,hdridle.conn_req_msg);
  HDRIDLE_PACK_CON_REQ_REQ_REASON(dsm_ptr,hdridle.conn_req_msg);
                                  
  /* If Searcher supports the subtype 2 physical layer, then the AT should 
     advertise its preferred bc idle channel if it is currently monitoring 
     a flow. */
  if ( curr_subtype != HDRSCP_DEFAULT_IDLE )
  {

#ifdef FEATURE_HDR_BCMCS_MFRQ
    if ( hdrbccp_get_preferred_bc_channel(
          &hdridle.conn_req_msg.preferred_channel[0].channel, NULL) == 
          E_SUCCESS )
    {
      hdridle.conn_req_msg.preferred_channel_cnt = 1;
      HDRIDLE_PACK_CON_REQ_PREF_CHAN_COUNT(dsm_ptr, hdridle.conn_req_msg);
      offset = HDRIDLE_CON_REQ_PREF_CHANNEL_OFFSET;

      /* TODO RevB:
       * Need to initialize mode, for example
       * hdridle.conn_req_msg.preferred_channel[0].mode = SYS_HDR_SYSTYPE_IS856; 
       */
      HDRIDLE_PACK_CON_REQ_SYSTEM_TYPE(dsm_ptr,hdridle.conn_req_msg,0,offset); 
      HDRIDLE_PACK_CON_REQ_BAND_CLASS(dsm_ptr,hdridle.conn_req_msg,0,offset);
      HDRIDLE_PACK_CON_REQ_CHAN_NUMBER(dsm_ptr,hdridle.conn_req_msg,0,offset);
      offset += HDRIDLE_CON_REQ_PREF_CHANNEL_SIZE;
    }
    else
#endif /* FEATURE_HDR_BCMCS_MFRQ */
    {
      hdrovhd_get_preferred_channel(&hdridle.conn_req_msg.preferred_channel_cnt, 
                                    hdridle.conn_req_msg.preferred_channel, 
                                    HDRIDLE_MAX_PREF_CHANNEL);

      HDRIDLE_PACK_CON_REQ_PREF_CHAN_COUNT(dsm_ptr, hdridle.conn_req_msg);
      offset = HDRIDLE_CON_REQ_PREF_CHANNEL_OFFSET;

      for(num_pref_chan = 0; 
           num_pref_chan < hdridle.conn_req_msg.preferred_channel_cnt; num_pref_chan++)
      {
        HDRIDLE_PACK_CON_REQ_SYSTEM_TYPE(dsm_ptr,hdridle.conn_req_msg,num_pref_chan,offset); 
        HDRIDLE_PACK_CON_REQ_BAND_CLASS(dsm_ptr,hdridle.conn_req_msg,num_pref_chan,offset);
        HDRIDLE_PACK_CON_REQ_CHAN_NUMBER(dsm_ptr,hdridle.conn_req_msg,num_pref_chan,offset);
        offset += HDRIDLE_CON_REQ_PREF_CHANNEL_SIZE;
      }
    }

    HDRIDLE_PACK_CON_REQ_EMERGENCY_INDICATION(dsm_ptr, 0, offset, 
            HDRIDLE_CON_REQ_EMERGENCY_INDICATION_SIZE);
    offset += HDRIDLE_CON_REQ_EMERGENCY_INDICATION_SIZE;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrsap_tunnel_mode_is_enabled(HDRHAI_IDLE_STATE_PROTOCOL))
  {
    hdridle_start_tunnel_conn_timer(HDRIDLE_CONN_SETUP_TIMER3_MSECS);
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  hdrhmp_send_msg
  (
    HDRHAI_IDLE_STATE_PROTOCOL,
    HDRHAI_AC,
    FALSE,
    HDRIDLE_CONN_REQ_PRIORITY,
    HDRHAI_IN_USE, /* Instance of protocol sending message */
    dsm_ptr,
    hdridle_conn_req_msg_tx_outcome_cb,
    (void *) ((dword) hdridle.conn_req_msg.trans_id)
  );

  hdrplog_start_conn_attempt(hdridle.conn_req_msg.trans_id);
} /* hdridle_send_conn_req_msg() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_START_SUSPEND_TIMER

DESCRIPTION
  This function starts the suspend timer if the advertised suspend time
  is greater than the current time.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_start_suspend_timer ( void )
{
  qword cur_time;          /* current time */
  qword suspend_time;      /* suspend time */
  qword time_diff;         /* time difference between
                              suspend time and current time */
  dword suspend_duration;  /* remaining suspend time duration */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrmc_feature_is_enabled(
        HDRMC_FEATURE_HDR_DISABLE_SUSPEND_TIMER))
  {
    HDR_MSG_PROT(MSG_LEGACY_FATAL, 
                 "hdridle_start_suspend_timer called when feature disabled!");
  }
  else if(hdrcon_get_advertised_suspend_time( suspend_time,
      HDRHAI_IDLE_STATE_PROTOCOL) == E_SUCCESS)
  {

    /* Get current time in units of 80ms */
    (void) time_get(cur_time);
    qw_shift(cur_time, HDRIDLE_TS_TO_80MS_CONVERSION);

    if(qw_cmp(suspend_time, cur_time) > 0)
    {
      qw_sub(time_diff, suspend_time, cur_time);

      /* note: The suspend time should not be more than 5 seconds
         greater than the current time. See hdrcon.c suspend_time
         for more details.
         With this contraint, time_diff[0] alone should be sufficient
         to hold the suspend duration (the diff between suspend_time and
         cur_time).
      */

      /* convert the suspend time duration from 80 ms unit to 1 ms unit */
      suspend_duration = qw_lo(time_diff) * 80;
      HDR_MSG_PROT( MSG_LEGACY_LOW, "Start suspend timer" );
      ( void ) rex_set_timer( &hdridle.suspend_timer,
                              suspend_duration );
    }
  }
} /* hdridle_start_suspend_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_STOP_SUSPEND_TIMER

DESCRIPTION
  This function stops the suspend timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_stop_suspend_timer ( void )
{

  if( rex_get_timer( &hdridle.suspend_timer) !=0 )
  {
     /* stop the timer */
     HDR_MSG_PROT( MSG_LEGACY_LOW, "Stop Suspend timer" );
     ( void ) rex_clr_timer( &hdridle.suspend_timer);
  }

} /* hdridle_stop_suspend_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_START_CONN_SETUP_TIMER

DESCRIPTION
  This function starts the connection setup timer

DEPENDENCIES
  None

PARAMETERS
  cnt - timer count in millisecond

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_start_conn_setup_timer
(
  rex_timer_cnt_type cnt
)
{
  /* stop connection setup timer if it is active */
  hdridle_stop_conn_setup_timer();
  HDR_MSG_PROT( MSG_LEGACY_LOW, "Start Connection Setup timer" );
  ( void ) rex_set_timer( &hdridle.state_timer, cnt);
} /* hdridle_start_conn_setup_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_STOP_CONN_SETUP_TIMER

DESCRIPTION
  This function stops the connection setup timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_stop_conn_setup_timer ( void )
{
   if( rex_get_timer( &hdridle.state_timer) !=0 )
   {
     /* stop the timer */
     HDR_MSG_PROT( MSG_LEGACY_LOW, "Stop Connection Setup timer" );
     ( void ) rex_clr_timer( &hdridle.state_timer);
   }
} /* hdridle_stop_conn_setup_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CHECK_OK_TO_SLEEP

DESCRIPTION
     This function checks whether all requirements for transition to
         the Sleep State are met or not. If they are met, then change the state
         to the Sleep State.

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_check_ok_to_sleep(void)
{

  sd_ss_act_e_type    act;                /* action returned by SD */
  boolean             status  = TRUE; /* boolean flag */
  int                 sleep_mask = 0; /* Mask to track sleep conditions */
  boolean             sleep_ok   = FALSE; /* Is it ok to sleep at all? */
  boolean             ovhd_is_rxed = TRUE; /* If we have overhead info */
#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  boolean             session_is_open = FALSE; /* If EVDO is opened */
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

  /* How long to sleep for */
  uint8 slot_cycle = 0x09; /* Default, 5.12s */
  int gear = 0; /* For debug, which slot_cycle value is being used */

  uint8 negotiated_sci = 0x09; /* Used to throttle 1x if necessary */
  uint8 sci_to_print = 0x09; /* Used to print the actual sleep this period */
  hdrhai_protocol_name_enum_type  curr_subtype = 
      (hdrhai_protocol_name_enum_type)
      hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
  boolean qpch_enabled = FALSE; /* whether QPCH is enabled or not */

  /* Following two variables will be passed to hdrlmac/hdrsrch to indicate if
   * intra scc sleep is needed or not*/
  boolean intra_scc_valid = FALSE; 
  uint16 cc_start = 0; 
#ifdef FEATURE_HDR_TO_LTE
  uint8 other_rat_tx_cycle = HDRSRCH_ORNL_CYCLE_DISABLED;
#endif /* FEATURE_HDR_TO_LTE*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  status = (hdrrup_is_tc_assigned( HDRHAI_IDLE_STATE_PROTOCOL )? FALSE: TRUE);
  sleep_mask |= (status << HDRIDLE_SLEEP_RMAC_ACTIVE);

  /* This flag is used for SRCH to request TRM. If there is no session, no 
   * page is expected, SRCH wakes up with lower priority. 
   * Only EVDO session is checked while PPP session is not considered.
   * This is because in AN stream, AT do not expect page when there is no PPP
   * session, but in AN stream, page could come as long as there is 
   * EVDO session.
   */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  if ( hdrsmp_is_to_maintain_evdo_session() )
  {
    session_is_open = hdrsmp_session_is_open( HDRHAI_IDLE_STATE_PROTOCOL );
  }
  else
  {
    /* With Minimal UATI Mode, if AT doesn't maintain the EVDO
     * session, we fake the session open status to be TRUE so
     * AT can meet the Full Sleep conditions.
     */
    session_is_open = TRUE;
  }
  sleep_mask |= (session_is_open << HDRIDLE_SLEEP_SESSION_OPEN);
#else
  if ( hdrsmp_is_to_maintain_evdo_session() )
  {
    status = hdrsmp_session_is_open( HDRHAI_IDLE_STATE_PROTOCOL );
  }
  else
  {
    /* With Minimal UATI Mode, if AT doesn't maintain the EVDO
     * session, we fake the session open status to be TRUE so
     * AT can meet the Full Sleep conditions.
     */
    status = TRUE;
  }
  sleep_mask |= (status << HDRIDLE_SLEEP_SESSION_OPEN);
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

  status = ( hdrovhd_info_is_updated( HDRHAI_IDLE_STATE_PROTOCOL )  && 
             !hdrovhd_is_waiting_to_update_ap( HDRHAI_IDLE_STATE_PROTOCOL ) );
  sleep_mask |= (status << HDRIDLE_SLEEP_OVHD_UPDATED);

  status = hdridle_suspend_timer_is_inactive();
#ifdef FEATURE_IRAT_PILOT_MEAS
  /* Suspend timer flag is overloaded to also indicate whether 
     IRAT measurement is in progress, in which case AT should 
     not got to sleep. */
  status = !hdrrup_is_irat_meas_in_progress();
  sleep_mask |= (status << HDRIDLE_SLEEP_MEAS_IN_PROGRESS);
#else
  sleep_mask |= (status << HDRIDLE_SLEEP_SUSPEND_TIMER);
#endif /* FEATURE_IRAT_PILOT_MEAS */

  status = (hdramac_is_in_attempt(HDRHAI_IDLE_STATE_PROTOCOL)
                  ? FALSE : TRUE);
  sleep_mask |= (status << HDRIDLE_SLEEP_AMAC_IN_ATTEMPT);

  status = hdrslp_tx_queue_is_empty( HDRHAI_IDLE_STATE_PROTOCOL );
  sleep_mask |= (status << HDRIDLE_SLEEP_SLP_TX_QUEUE);

  status = hdrhmp_tx_queue_is_empty( HDRHAI_IDLE_STATE_PROTOCOL );
  sleep_mask |= (status << HDRIDLE_SLEEP_HMP_TX_QUEUE);

  status = hdridle.rcv_end_of_sync_cap_ind;
  sleep_mask |= (status << HDRIDLE_SLEEP_SYNC_CAPSULE_RXED);

    /* BIO switch is TRUE if sleep is disabled */
  status = (!hdrlog_is_dipsw_set(HDRLOG_DIPSW_SLEEP_OFF));
  sleep_mask |= (status << HDRIDLE_SLEEP_BIO_SWITCH);

  status = hdridle.sleep_capsule_done;
  sleep_mask |= (status << HDRIDLE_SLEEP_CAPSULE_DONE);

  status = hdridle.lmac_suspend_or_resume_received;
  sleep_mask |= (status << HDRIDLE_SLEEP_LMAC_SUSPEND_OR_RESUME_RECEIVED);

#ifdef FEATURE_HDR_BCMCS
  /* Check whether BCMCS is ok to sleep. */
  status = hdrbcmac_is_ok_to_sleep();
#else
  status = TRUE;
#endif /* FEATURE_HDR_BCMCS*/
  sleep_mask |= (status << HDRIDLE_BROADCAST_ACTIVE);

#ifdef FEATURE_HDR_BCMCS
  /* If we have BCMCS flows requested, we need to monitor the BroadcastOverhead
     message which may be sent in the SyncCC capsule after the 
     SleepStateCapsuleDone bit is set. */
  if ( hdrbccp_are_any_flows_requested() )
  {
    status = hdridle.rcv_end_of_sync_cap_ind;
  }
  else
  {
    status = TRUE;
  }
#else
  status = TRUE;
#endif /* FEATURE_HDR_BCMCS */
  sleep_mask |= (status << HDRIDLE_SLEEP_ENTIRE_CAPSULE_RXED_FOR_BCMCS);

#ifdef FEATURE_HDR_TO_LTE
  /* If OtherRATNeighborList is being transmitted in current SyncCC then
     entire CCsync capsule need to be received before we can go to sleep */
  if ((hdridle.last_wakeup_reason == HDRSRCH_WAKEUP_OTHER_RAT_NL) &&
       !hdrovhd_ornl_is_updated(HDRHAI_IDLE_STATE_PROTOCOL))
  {
    status = hdridle.rcv_end_of_sync_cap_ind;
  }
  else
  {
    status = TRUE;
  }

  sleep_mask |= (status << HDRIDLE_SLEEP_ENTIRE_CAPSULE_RXED_FOR_BCMCS);
#endif /* FEATURE_HDR_TO_LTE*/

  /* AST doesn't want to support the configuration and testing of SCI < 5.
   * Treat them as non-slotted mode, except if the Prefered Slotted Mode
   * NV item is set to allow this. */
  status = hdridlec_slotted_mode_is_ok_to_sleep(); 
  sleep_mask |= (status << HDRIDLE_SLEEP_SCI_SUPPORTED);
  
  status = hdridle.intra_scc.continued_scc_pkt_expected;
  sleep_mask |= (status << HDRIDLE_SLEEP_CONTINUED_SCC_PKT_EXPECTED);
  
  status = hdridle.qc_is_rxed_in_this_scc;
  sleep_mask |= (status << HDRIDLE_SLEEP_QC_RXED);

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "sleep mask 0x%x", sleep_mask );
  if ( HDRIDLE_SLEEP_BASIC_CONDITIONS_MET( sleep_mask ))
  {
#ifdef FEATURE_HDR_LONG_SLEEP
    /* If this our first entry into sleep since we acquired, closed a
       connection, or failed a connection setup, then we should reset the
       long sleep timer */
    if (hdridle.first_sleep)
    {
      (void) rex_set_timer(&hdridle.long_sleep_delay_timer,
                    HDRIDLE_LONG_SLEEP_DELAY_TIMEOUT);
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "first_sleep:reset 40s sleep delay 1 min" );
      hdridle.first_sleep = FALSE;
    }
#endif /* FEATURE_HDR_LONG_SLEEP */

    /*---------------------------------------------------------------------
                          Set duration for sleep
     ---------------------------------------------------------------------*/

    /* //DA If we have a stream for data, sleep on our normal slot */
    if ( curr_subtype != HDRSCP_DEFAULT_IDLE )
    {
      slot_cycle = hdridle_get_current_sci( &gear );
      negotiated_sci = slot_cycle;
    }

    if( hdridle.sci_override != 0 )
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Test SCI Override Enabled: %d->%d", 
        slot_cycle, hdridle.sci_override );

      negotiated_sci = hdridle.sci_override;
      slot_cycle = hdridle.sci_override;
    }

    /* AT never sleeps more than 40s so if the negotiated SCI is greater
       than 12, print the F3 using the value of 12 (40s sleep) */
    sci_to_print = negotiated_sci > 12 ? 12 : negotiated_sci;

    if (HDRIDLE_SLEEP_FULL_CONDITIONS_MET( sleep_mask ))
    {
#ifdef FEATURE_HDR_LONG_SLEEP
      if ( hdridle_ready_for_long_sleep() )
      {
        /* Force to 40s sleep */
        if( hdridle.sci_override == 0 )
        {
          slot_cycle = 0x0c;
        }

        HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, 
          "40s sleep (%d) = %s sleep this period", 
          slot_cycle, hdridle_sleep_time_strings[sci_to_print] );

        sleep_ok = TRUE;
      }
      else
#endif /* FEATURE_HDR_LONG_SLEEP */
      {
        /* If we have a stream for data, sleep on our normal slot */
        if ( curr_subtype != HDRSCP_DEFAULT_IDLE )
        {
          HDRTRACE_IDLE_SET_SLEEP_DURATION( slot_cycle, gear);
          HDR_MSG_SPRINTF_3( MSG_LEGACY_HIGH, 
            "SCI %d sleep (using SlotCycle%d) = %s sleep this period",
            slot_cycle, gear, hdridle_sleep_time_strings[sci_to_print] );
        }
        else
        {
          HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, 
            "5.12s sleep (%d) = %s sleep this period", slot_cycle,
            hdridle_sleep_time_strings[sci_to_print] );
        }

        sleep_ok = TRUE;
      }
    }
    else
    {
      if (HDRIDLE_SLEEP_426MS_CONDITIONS_MET( sleep_mask ))
      {
        /* We woke up and received sync capsule. 
         * At a minimum we can sleep until the next synchronous capsule */

        slot_cycle = MIN( 0x06, slot_cycle );
        HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, 
          "426ms Sleep (%d) = %s sleep this period", slot_cycle,
          hdridle_sleep_time_strings[slot_cycle] );

        sleep_ok = TRUE;

        /* session not open or ovhd not receive, either way ovhd not recivd? how about BCMCS*/
        ovhd_is_rxed = FALSE; 
      }
      else if (HDRIDLE_SLEEP_BASIC_INTRA_SCC_CONDITIONS_MET(sleep_mask))
      {
        /* we didn't receive entire sync causule yet, but we received a CC pkt */

        if (HDRIDLE_SLEEP_426MS_PARTIAL_SCC_RXED_CONDITIONS_MET(sleep_mask))
        {
          /* We can at least go to 426ms if following two conditions are TRUE:
           * 1) Received CC pkt with sleep capsule done bit set to 1 
           * 2) overhead is up to date 
           * 3) received BCMCS OVHD
           */ 

          slot_cycle = MIN( 0x06, slot_cycle );
          HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
            "426ms Sleep partial scc rxed(%d) = %s sleep this period", slot_cycle,
          hdridle_sleep_time_strings[sci_to_print] );

          sleep_ok = TRUE;
        }
        else
        {
          /* We received CC pkt(s) but haven't received entire SleepCapsule yet.
           * Do intra scc sleep now... 
           */
          cc_start = hdridle.intra_scc.cc_start;
          intra_scc_valid = TRUE;

          /* - For Intra SCC sleep, slot_cycle is the sleep duration that we will like 
           *   hdrsrch to use in case of intra scc sleep/wakeup fails 
           * - The actual intra scc sleep duration will be caculated by hdrsrch based on 
           *   cc_start value and intra_scc_valid flag.
           */
          if (HDRIDLE_SLEEP_FULL_PARTIAL_SCC_RXED_CONDITIONS_MET(sleep_mask))
          {
            /* If the srch is not able to do intra scc sleep, it can go back to normal 
             * sleep cycle(i.e. slot_cycle variable) if all following condictions are true:
             * 1) rx QC 2) Session is open 3)ovhd is updated 4) rx BOM
             * Note: slot_cycle is not the actual intra scc sleep duration, 
             * it's only used by hdrsrch in case of intra scc sleep/wakeup fails
             */
#ifdef FEATURE_HDR_LONG_SLEEP
            if ( hdridle_ready_for_long_sleep() )
            {
              /* Force to 40s sleep */
              if( hdridle.sci_override == 0 )
              {
                slot_cycle = 0x0c;
              }
            }
#endif /* FEATURE_HDR_LONG_SLEEP */

          }
          else
          { 
            slot_cycle = MIN( 0x06, slot_cycle );
          }

          HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Intra SCC Sleep cc_start: %d, fallback sci: %d", 
                          cc_start, slot_cycle);

          sleep_ok = TRUE;
          /* We don't want to deactivate CMAC during intra SCC sleep*/
          ovhd_is_rxed = FALSE;
        }
      }
      else
      {
        /* Can't go to sleep yet, because: 
         *  1)We are still waiting to finish receiving this synchronous
         *   control channel capsule, and
         *  2) we haven't received any CC pkt yet
         */
      }
    }
  }

  if ( sleep_ok &&
      (HDRIDLE_SLEEP_FULL_CONDITIONS_MET( sleep_mask )))
  {
    /* We don't need to call the SD reselection function if we were previously
     * sleeping. However if any access attempts have made since last time 
     * entering Monitor state, we still need to call the SD reselection.
     */
    if (!hdridles_is_previous_state_sleep() ||
        hdridle.access_since_entering_monitor  )
    {
      act = sd_ss_ind_hdr_opr_resel_ok(NULL);

      if(act != SD_SS_ACT_CONTINUE)
      {
        hdridle_process_sd_act(act);
        sleep_ok = FALSE;
      }
    }
  }

#ifdef FEATURE_HDR_QCHAT
  if (hdrqchatupk_imo_enabled()) 
  {
    hdrqchatupk_idle_check_sleep(&sleep_ok);
  }
#endif /*FEATURE_HDR_QCHAT*/
  if (sleep_ok)
  {
    if (ovhd_is_rxed)
    {
      hdrovhd_sleep(HDRHAI_IDLE_STATE_PROTOCOL);
      hdrcmac_deactivate(HDRHAI_IDLE_STATE_PROTOCOL);
    }
    else
    {
      /* If we are waiting for overhaed info, don't disable 
         CCMAC or OVHD supervision.  Otherwise we could stay 
         forever on a weak system */
    }

    if( negotiated_sci > HDRIDLE_LOW_SCI )
    {
      hdridle_stop_1x_acq_throttling();
    }
    else
    {
      if( !hdridle.throttling_1x_acq )
      {
        hdridle.throttling_1x_acq = TRUE;
        (void) sd_ss_ind_hdr_opr_start_activity( 
                 NULL, SD_SS_HDR_ACT_MASK_IDLE_LOW_SCI );
      }
    }

    hdridles_change_state(HDRIDLES_SLEEP_STATE);
    
    if (hdridlec_qpch_is_enabled())
    {
      qpch_enabled = TRUE;
    }

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Redir is in progress %d ovhd_updated %d",
                    hdralmp_hdr_to_lte_redir_meas_in_progress(), ovhd_is_rxed);
    if((hdralmp_hdr_to_lte_redir_meas_in_progress() == TRUE)&&
        (ovhd_is_rxed == TRUE))
    {
      if(hdridle.first_sleep_for_meas == TRUE)
      {
        /* Disable CC pkts reading  */
        hdrhmp_set_ignore_cc_msgs(TRUE);
      }
      /* Prepare sleep command for D2L redir measurement */
      negotiated_sci = slot_cycle = 0x06; /* override with 426ms sleep */
      intra_scc_valid = FALSE;
      other_rat_tx_cycle = (hdridle.first_sleep_for_meas == TRUE) ? HDRSRCH_ORNL_CYCLE_DISABLED : HDRSRCH_ORNL_CYCLE_NO_UPDATE;
      hdridle_set_first_hdr_to_lte_redir_meas(FALSE);
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "OtherRATTxCycle %d sleep_cnt %d",other_rat_tx_cycle, hdridle.first_sleep_for_meas );
    }
    else
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
    {
#ifdef FEATURE_HDR_TO_LTE
    other_rat_tx_cycle = hdrovhd_get_other_rat_tx_cycle();
#endif /* FEATURE_HDR_TO_LTE*/
    }

    hdrlmac_sleep( slot_cycle, negotiated_sci, HDRHAI_IDLE_STATE_PROTOCOL,
                   intra_scc_valid, cc_start, qpch_enabled
#ifdef FEATURE_HDR_TO_LTE
                   , other_rat_tx_cycle 
#endif /* FEATURE_HDR_TO_LTE*/
#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
                   ,session_is_open
                   ,ovhd_is_rxed
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */
                 );
#ifdef FEATURE_HDR_DDARF
    hdridle.last_sci = slot_cycle;
#endif
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Not Ok to Sleep (0x%x)",sleep_mask );
  }

  HDR_MSG_PROT_6( MSG_LEGACY_MED, "sleep %d slot %d sci %d intra %d cc %d qpch %d",sleep_ok, slot_cycle, negotiated_sci, intra_scc_valid, cc_start, qpch_enabled);

} /* hdridle_check_ok_to_sleep() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_CON_IS_FOR_SESSION_CONFIG

DESCRIPTION
  Return whether the current connection is being opened in order to send
  signaling information for session config.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the current connection is being opened in order to send
  signaling information for session config
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_con_is_for_session_config (void)
{
  hdridles_state_enum_type cur_state = hdridles_get_state();
                                       /* current idle state */
  boolean is_for_session_config = FALSE; /* Return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cur_state != HDRIDLES_INACTIVE_STATE )
  {
    if( !hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) )
    {
      is_for_session_config = TRUE;
    }
  }
  else
  {
    is_for_session_config = FALSE;
  }

  return is_for_session_config;

} /* hdridle_con_is_for_session_config() */

/*===========================================================================

FUNCTION HDRIDLE_SET_FORCE_LONG_SLEEP

DESCRIPTION
  This function sets force_long_sleep flag.
 
  When force_long_sleep flag is set, AT uses 40 seconds as sleep period when
  it is ready to go to sleep state.

DEPENDENCIES
  None

PARAMETERS
  force_long_sleep - turn on/off force_long_sleep flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_set_force_long_sleep
(
  boolean force_long_sleep
)
{
  hdridle.force_long_sleep = force_long_sleep;
} /* hdridle_set_force_long_sleep */

/*===========================================================================

FUNCTION HDRIDLE_GET_FORCE_LONG_SLEEP

DESCRIPTION
  This function gets the current value of force_long_sleep flag.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - force_long_sleep flag is set. AT uses 40 seconds as sleep period in
         SLEEP state. 
  FALSE - force_long_sleep flag is not set.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_get_force_long_sleep( void )
{
  return hdridle.force_long_sleep;
} /* hdridle_get_force_long_sleep */

/* <EJECT> */
#ifdef FEATURE_HDR_LONG_SLEEP
/*===========================================================================

FUNCTION HDRIDLE_READY_FOR_LONG_SLEEP

DESCRIPTION
  This function checks for if AT were to go to sleep, would it do long sleep.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If AT were to do long sleep
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_ready_for_long_sleep( void )
{
  /* Go for long sleep if force_long_sleep flag is set, or there is no packet
     data session and long_sleep_delay_timer is not set. */
  if ( ((hdridle_get_force_long_sleep()) ||
        (!hdridle_is_pkt_data_sess_on_hdr() &&
          rex_get_timer(&hdridle.long_sleep_delay_timer) == 0 )) &&
       (!hdrlog_is_dipsw_set(HDRLOG_DIPSW_40S_SLEEP_OFF))
     )
  {
    return TRUE;
  }

  return FALSE;
} /* hdridle_ready_for_long_sleep() */
#endif /* FEATURE_HDR_LONG_SLEEP */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_SLEEPING

DESCRIPTION
  This function checks if AT is sleeping

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If AT is sleeping
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_sleeping( void )
{

  return ( (hdridles_get_state() == HDRIDLES_SLEEP_STATE) ? TRUE : FALSE );

} /* hdridle_is_sleeping() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_SUSPENDED

DESCRIPTION
  This function checks if IDLE state protocol is in suspended state

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If IDLE state protocol is in suspended state
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_suspended( void )
{

  return ( (hdridles_get_state() == HDRIDLES_SUSPEND_STATE) ? TRUE : FALSE );

} /* hdridle_is_suspended() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_CONNECTION_NEEDED

DESCRIPTION
  This function tells if a connection is needed by the protocol


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if a connection is needed, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_connection_needed ( void )
{
  hdridles_state_enum_type  cur_state = hdridles_get_state();
                                     /* current idle state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  return ( ( (cur_state == HDRIDLES_CONN_SETUP_STATE) ||
             (cur_state == HDRIDLES_CONN_SETUP_SUSPEND_STATE) ) &&
           (hdridle.connect_reason != HDRIDLE_CR_ABORT) );
} /* hdridle_is_connection_needed() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_ALMP_LEAVING_IDLE

DESCRIPTION
  This function processes when ALMP leaves the Idle, specifically it resets
  the lmac_suspend_or_resume_received flag. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None  

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_almp_leaving_idle ( void )
{
  /* We moved the resetting of lmac_suspend_or_resume_received 
   * flag from IDLE activate to here when ALMP leaves Idle. It
   * doesn't matter whether there is a session open or close, 
   * as long as ALMP stays in Idle state, this flag won't be 
   * reset, so AT can go to sleep.
   */
  hdridle.lmac_suspend_or_resume_received = FALSE;

} /* hdridle_almp_leaving_idle() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_CONNECTING

DESCRIPTION
  This function tells if idle is attempting to open a connection.  This 
  includes the period from when the AT queues the ConnectionRequest 
  to when the RTCACK is received or the connection setup fails.

  The requirement in section 8.7.6.1.5.4 says, "The access terminal shall not 
  send a RouteUpdate message if the state timer of the Connection Setup State 
  in the Idle State Protocol is active."

  This needs to include the period while the AT is sending the 
  ConnectionRequest (before receiving the ACACK) because other messages
  queued to SLP druing this time would also still be transmitted in a new
  access probe after the ACACK is received.  hdrhmp_msg_is_sendable() needs
  to make a special exception for the ConnectionRequest message so it can
  still be sent.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  if performing connection setup and an ACACK has already been received
        to the ConnectionRequest message
  FALSE if not performing connection setup, or just sending the 
        ConnectionRequest (no ACK yet)

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_connecting ( void )
{
  hdridles_state_enum_type idle_state;
  hdridles_csetup_substate_enum_type csetup_substate;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  idle_state = hdridles_get_state();
  csetup_substate = hdridles_get_csetup_substate();

  if ((csetup_substate == HDRIDLES_CSETUP_CREQ_ACMAC) ||
      (csetup_substate == HDRIDLES_CSETUP_TCA_RCVD)) 
  {
    if ((idle_state == HDRIDLES_CONN_SETUP_STATE) ||
        (idle_state == HDRIDLES_CONN_SETUP_SUSPEND_STATE))
    {
      return TRUE;
    }
    else
    {
      HDR_MSG_PROT_2(MSG_LEGACY_FATAL, "Idle state/substate outof sync %d/%d",
                     idle_state, csetup_substate);
      hdridles_change_csetup_substate(HDRIDLES_CSETUP_INVALID);
    }
  }

  return FALSE;
} /* hdridle_is_connecting() */


/*===========================================================================

FUNCTION HDRIDLE_STOP_1X_ACQ_THROTTLING

DESCRIPTION
  This function stops 1x acquisition throttling if needed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_stop_1x_acq_throttling( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( hdridle.throttling_1x_acq == TRUE )
  {
    hdridle.throttling_1x_acq = FALSE;
    (void) sd_ss_ind_hdr_opr_stop_activity( 
            NULL, SD_SS_HDR_ACT_MASK_IDLE_LOW_SCI );
  }
} /* hdridle_stop_1x_acq_throttling() */

/*===========================================================================

FUNCTION HDRIDLE_GET_CR_ROR_PROGRESS

DESCRIPTION
  This function gets ConReq + RoR progress.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  True - if CR+RoR is in progress
  False - if CR+RoR has ended.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdridle_get_cr_ror_progress()
{
  return hdridle.cr_ror_in_progress;
} /* hdridle_set_cr_ror_status() */

/*===========================================================================

FUNCTION HDRIDLE_SET_CR_ROR_PROGRESS

DESCRIPTION
  This function sets ConReq + RoR status.

DEPENDENCIES
  None.

PARAMETERS
  progress - indicates if CR+RoR is in progress

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_set_cr_ror_progress
(
  boolean progress
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdridle.cr_ror_in_progress = progress;
} /* hdridle_set_cr_ror_status() */

/*===========================================================================

FUNCTION HDRIDLE_IS_PKT_DATA_SESS_ON_HDR

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - Whether the data session is on hdr

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdridle_is_pkt_data_sess_on_hdr(void)
{
 return hdridle.data_session_is_on_hdr;
} /* hdridle_is_pkt_data_sess_on_hdr */

/*===========================================================================

FUNCTION HDRIDLE_SET_PKT_DATA_SESS_STATUS

DESCRIPTION
  Sets the current packet data session status - whether it is on HDR
  or not. This function is called everytime the session status changes.

DEPENDENCIES
  None.

PARAMETERS
  is_session_on_hdr - TRUE if whether data session is on HDR or 1x
                    - FALSE otherwise

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_set_pkt_data_sess_status(boolean is_session_on_hdr)
{
  hdridle.data_session_is_on_hdr = is_session_on_hdr;
} /* hdridle_set_pkt_data_sess_status */

/*===========================================================================

FUNCTION HDRIDLE_IS_CONNECTING_BEFORE_ACK

DESCRIPTION
  Checks if IDLE is in connection set up, and the ConRequest
  has not been ACKed

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  True - If IDLE is connecting AND ConReq is ACKed
  False - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdridle_is_connecting_before_ack( void )
{
  return hdridle_is_connecting() && hdridle.conn_req_msg.awaiting_slp_callback;
} /* hdridle_is_connecting_before_ack */

/*===========================================================================

FUNCTION HDRIDLE_SLEEP_BASED_ON_QUICKPAGE

DESCRIPTION
  This function sets the sleep flag to indicate whether AT can go 
  to sleep based on quick page message.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_sleep_based_on_quickpage( boolean sleep_flag )
{
  hdridle_set_rcv_end_of_sync_cap_ind(sleep_flag);
} /* hdridle_sleep_based_on_quickpage */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_RESET_QC_IS_RECEIVED_FLAG

DESCRIPTION
  This function set the flag that indicates if the QC is received in current 
  Sync Capsule 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_reset_qc_is_received_flag(void)
{
  if (hdridle.rcv_end_of_sync_cap_ind)
  {
     /* qc_is_rxed_in_this_scc flag is used only in intra scc and CC_MISSED
      * cases, if we are not expecting more CC packets, reset flag */
    hdridle.qc_is_rxed_in_this_scc = FALSE;
  }
} /* hdridle_reset_qc_is_received_flag() */

#ifdef FEATURE_SVDO
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CONN_SETUP_TCA_FAILED

DESCRIPTION
  This function processes the connection setup state tca failed due to
  - channel conflict in SVDO case 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdridle_process_conn_setup_tca_failed ( void )
{

  if ( hdridles_get_state() == HDRIDLES_CONN_SETUP_STATE )   
  {
    hdrplog_end_conn_attempt(HDRPLOG_CONN_ATT_SETUP_TIMEOUT);
    hdridle_connect_setup_fail_cleanup(FALSE);
    if(hdridle_rpt_conn_setup_fail(HDRIDLE_CF_TCA_FAILED) ==
                                   SD_SS_ACT_CONTINUE)
    {
      hdridles_change_state(HDRIDLES_MONITOR_STATE);
    }
  }
  else
  {

    /*-----------------------------------------------------------------------
      Wrong state or timer is not on, do nothing..
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT( MSG_LEGACY_LOW, "Connection setup TCA FAILED ignored" );
  }
}/* hdridle_process_conn_setup_tca_failed() */
#endif /* FEATURE_SVDO */

#ifdef FEATURE_HDR_REVC
/*===========================================================================

FUNCTION HDRIDLE_IS_MULTI_AT_PAGE_MSG_RCVD

DESCRIPTION
  Check whether any MultiATPage message is received from current
  serving sector.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE  - A MultiATPage message has been received from current
          serving sector in the CONNECTED or IDLE state
  FALSE - None of loading information message has been received.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdridle_is_multi_at_page_msg_rcvd ( void )
{
  return hdridle.multi_at_page_msg_rcvd;
} /* hdridle_is_multi_at_page_msg_rcvd */


/*===========================================================================
 
FUNCTION HDRIDLE_SET_MULTI_AT_PAGE_MSG_RCVD

DESCRIPTION
  This function resets the indicator which shows whether MultiATPage
  message is received in the CONNECTED OR IDLE state from the current serving
  sector.
 
DEPENDENCIES
  None.

PARAMETERS
  multi_at_page_msg_rcvd - TRUE, if MultiATPage message has been received
                           in the CONNECTED OR IDLE state from the current 
                           serving sector. FALSE, if not.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 
===========================================================================*/
void hdridle_set_multi_at_page_msg_rcvd
(
  boolean multi_at_page_msg_rcvd 
)
{
  hdridle.multi_at_page_msg_rcvd = multi_at_page_msg_rcvd;
} /* hdridle_set_multi_at_page_msg_rcvd */


/*===========================================================================
 
FUNCTION HDRIDLE_IS_MULTI_AT_PAGE_ATTRIB_REJECTED

DESCRIPTION
  This function tells whether MultiATPageMessageSupported attribute has been
  rejected by the current serving sector or not. 

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  TRUE  - MultiATPageMessageSupported attribute request has been
          rejected by the current serving sector.
  FALSE - MultiATPageMessageSupported attribute request has not
          been rejected by the current serving sector yet.

SIDE EFFECTS
  None.
 
===========================================================================*/
boolean hdridle_is_multi_at_page_attrib_rejected( void )
{
  return hdridle.multi_at_page_attrib_rejected;
} /* hdridle_is_multi_at_page_attrib_rejected */


/*===========================================================================
 
FUNCTION HDRIDLE_SET_MULTI_AT_PAGE_ATTRIB_REJECTED

DESCRIPTION
  This function resets the indicator which shows whether
  MultiATPageMessageSupported attribute proposed by AT is rejected by the 
  current serving sector. 

DEPENDENCIES
  None.

PARAMETERS
  multi_at_page_attrib_rejected - TRUE, if MultiATPageMessageSupported 
                                  attribute proposed or GAUPed has been 
                                  rejected by the current serving sector. 
                                  FALSE, if not.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_set_multi_at_page_attrib_rejected
(
  boolean multi_at_page_attrib_rejected 
)
{
  hdridle.multi_at_page_attrib_rejected = multi_at_page_attrib_rejected;
} /* hdridle_set_multi_at_page_attrib_rejected */
#endif /* FEATURE_HDR_REVC */


#ifdef FEATURE_HDR_CC_DECODE_FAIL
/*===========================================================================
 
FUNCTION HDRIDLE_RESET_CC_REACQ_FAIL_COUNTER

DESCRIPTION
  This function resets the cc/re-acq fail counter.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_reset_cc_reacq_fail_counter()
{
  	hdridle.num_cc_reacq_failed = 0;
} /* hdridle_reset_cc_reacq_fail_counter */
#endif /* FEATURE_HDR_CC_DECODE_FAIL */


/*===========================================================================

FUNCTION HDRIDLE_RESET_IDLE_SUSPEND_TO_REACQ_FLAG

DESCRIPTION
  This function set the flag that indicates if idle suspend to reacq occured

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_reset_idle_suspend_to_reacq_flag()
{
    if(hdridle.idle_suspend_to_reacq == TRUE)
    {
     /* This is unexpected and should not be happening. The flag must have 
        already RESET, when LMAC_AWAKE is recieved */
	   hdridle.idle_suspend_to_reacq = FALSE;
       HDR_MSG_PROT (MSG_LEGACY_HIGH, "idle_suspend_to_reacq was TRUE, when IDLE is exiting MONITOR. Resetting it.");
    }
}/*=============================================================================

FUNCTION HDRIDLE_SET_FIRST_HDR_TO_LTE_REDIR_MEAS

DESCRIPTION
  This function sets if LTE measurements for redirection are in progress (or) not

DEPENDENCIES
  None.

PARAMETERS
  value - value to be set TRUE (or) FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdridle_set_first_hdr_to_lte_redir_meas(boolean value)
{
   hdridle.first_sleep_for_meas = value;
}
