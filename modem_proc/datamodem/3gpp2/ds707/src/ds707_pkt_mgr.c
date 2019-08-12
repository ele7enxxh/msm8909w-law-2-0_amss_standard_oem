/*===========================================================================
                        D S 7 0 7 _ P K T _ M G R . C
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls.  It does not have any
  async/fax functionality.

  This module mainly controls the traffic channel portion of the PS_IFACE.
  This module works in conjunction with ds707_rmsm.c and ds707_dssnet.c

  This file mainly concerns itself with interacting with CM/MC, in order to
  bring up and tear down the traffic channel, as well as going into and
  coming out of dormancy.

  This module does not know if the packet call is laptop or sockets, or even
  if it is relay or network.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_pkt_mgr_init() and ds707_pkt_cm_init() must be called at startup.

Copyright (c) 2002-2016 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/04/15    sd     Fix to enable/disable flow control based on DSM memory 
                   events for data call on 1x.
12/26/14    vm     Prevent traffic channel bringup if retry delay timer is running
11/18/14    skc    Removing FEATURE_UW_FMC
10/21/14    sc     Added code to perform graceful PPP termination during hard
                   failure and then update eHRPD availability  
09/11/14    sd     Fix to disable the flow and block reorigination when 1X 
                   is not in service.
08/05/14    ms     DSDA Phase 2 changes.
07/23/14    sd     Fix to use credentials from OMH profile in case of card mode
                   during MIP to SIP fallback.
07/04/14    sc     Optimize the code to reduce the static memory
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
06/04/14    sd     Fix to utilize DSM memory event callbacks to enable and
                   disable flow control.
04/19/14    sc     Added code to handle v6 cleanup in socket tear down when
                   ipv6 is disabled  
04/18/14    ms     KW fixes.
03/04/14    sd     Changes made to notify DSD to indicate 1x service 
                   availability only after ESPM is received.
12/17/13    hr     New DIAG event to log dormancy & reorigination of data calls 
10/09/13    sc     Removed featurization of FEATURE_DUAL_SIM
07/04/13    sc     Added null check before dereferencing iface_ptr in eHRPD mode
                   when incoming data is processed by UM SN sig hdlr
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
06/10/13    sc     Changes made to check for eHRPD mode only when SO mask is 
                   RevA/RevB EMPA and RevB MMPA
06/03/13    sd     ATCoP decoupling changes
05/27/12    sc     Send packets on default flow if sec flows are not activated
                   instead of dropping them.
11/02/12    ssb    Adding utility funtion to increment and get number of 
                   MIP to SIP Fallback attempts
09/25/12    ss     Moving EFS item path string variable to global memory.
09/20/12    dvk    Made changes for accessing tcp_ack_prio_enabled global 
                   variable from a accessor function.
09/03/12    dvk    Accessor function to set BP paramaters value to the 
                   specified values. 
08/23/12    jz     Gracefully tear down PPP upon receiving PDN ID in use
08/13/12    ss     Removed ds_create_efs_item_file usage. Listed the file in 
                   ds_init_efs_config_file instead.
07/13/12    jz     Support for emergency only mode when mandatory APN is disable
07/19/12    jee    LPM mode change to send VSNCP term before LCP term
07/17/12    ms     Dynamic back pressure removal support for TCP throughput
                   optimization.
06/19/12    jee    To do graceful teardown when card REFRESH event happens
07/11/12    sb     Fixed KW Errors
05/02/12    jz     Set ip_ver_capability according to profile specifies 
05/03/12    ash    Indicate NULL bearer when HDR session is not open.
05/04/12    jee    Mainlined FEATURE_UIM_SUPPORT_HOTSWAP
04/20/12    op     Added support to read the dns_v4_addr_req_type NV item
04/18/12    jz     Seperate old JCDMA and Simplified JCDMA
04/16/12    ash    Use new HDR API to inform LTE indication.
04/12/12    jee    To propagate correct call end reason from MH
03/21/12    msh    TCB changes due to Core changes  
02/22/12    jz     Disable active HDR->1X handdown for JCDMA  
02/10/12    jz     Support for ignoring the last PDN inactivity timer 
02/06/12    jz     Add additional dial string for DUN call 
01/26/12    msh    Read internal auth nv item 
01/18/12    jz     PDN failure fallback list 
01/16/12    ack    Fix to post a cmd for iface bringup/teardown
01/02/12    msh    Coan: Feature cleanup 
04/01/12    jee    Fix to set PCMT value to iRAT when LTE attach happens
01/04/12    ack    Fixed KW Warnings
12/23/11    sb     Fixed compiler warnings 
12/21/11    msh    Added debug messages for get and set current sys mode 
11/11/11    jee    To fix Arbitration issue with v4/v6 calls
12/07/11    jz     To close HDR session upon MIP authentication failure(err 67)
12/07/11    vpk     Add support for 5 PDNs
11/29/11    vpk    HDR SU API cleanup
11/28/11    jz     support ppp control packet on dos 
11/18/11    jee    Simplify devmgr_stop() implementation
11/16/11    msh    Fixed graceful teardown in LPM mode 
10/31/11    ssh    eHRPD Always On support
10/21/11    jee    Abort devmgr in offline mode
10/15/11    jee    Added support to abort devmgr
10/04/11    jee    To handle device manager when hot swap happens
10/03/11    msh    Added wmk_lvl_init function for rm and um wmk lvls 
09/26/11    msh    Fixed Compiler Warning 
09/15/11    vpk    Tear down all the ifaces when an UIM refresh happens.
09/19/11    msh    Fixed compiler warnings 
09/08/11    ash    Added support for eHRPD to HRPD fallback
08/16/11    vpk    Support memory flow control logging
08/15/11    ms     Fixed graceful tear down in LPM
08/11/11    var	   Resolve Compiler warnings & KW errors.
08/09/11    sha    Global variable clean up.
07/22/11    dvk    Global Variable Cleanup
07/08/11    ls     Check reverse link ptr before accessing it and enqueing to 
                   WM  
06/24/11    scb    Added support for EFS NV item is_fmc_enabled.
06/23/11    ls     Tearing down MIP call when PPP_CLOSE_CMD is received and
                   it is in EPC system
06/14/11    op     Decremented "num_used_3gpp2_v4_ifaces" when dssnet_start()
                   fails.
06/13/11    dvk    Merged support to get pre-dormant data call information.
06/06/11    ash    Added support for PPP Partial Context
06/02/11    vpk    Do not clear watermarks during reorigination as its done 
                   during tear down.
05/22/11    bq     Add ds3g_is_pref_pkt_mode in ds707_bring_up_cmd
05/04/11    ttv    Added changes to compute mip statistics.
04/15/11    ls     During bringup, it needs to check with EPC if the call 
                   is allowed or not
04/11/11    ack    Fix to gracefully tear down data call on receiving 
                   DS707_PKT_MGR_MMGSDI_RESET
04/11/11    ttv    Added support for Hot Swap feature.
04/08/11    ls     Clean up the API call for ds707_ccm
03/29/11    ack    Fix to gracefully tear down calls before phone 
                   goes to LPM mode
03/21/11    ack    Init Crit Sect Variable before usage
03/15/11    mg     Global variable cleanup
03/14/11    mga    Merged changes to set the MTU accordingly
03/02/11    op     Added global to store hdr hybrid status
02/25/11    ms     Support for KDDI special NAI and Password for AN HDR
                   authentication.
02/26/11    ls     Global Variable cleanup
02/25/11    op     Klocwork fix
02/23/11    ash    Initialize sm_config regardless of whether nv item is 
                   active or not.
02/16/11    op     Klocwork fix
02/11/11    ack    Global Variable Cleanup
01/28/11    jee    JCDMA feature cleanup & PPP re-sync handling
02/04/11    mg     Process pwroff/lpm mode notification cmd
02/03/11    ms     Enter PS critcal section before posting phys link gone ind.
01/20/11    op     Fix for static analysis warning
01/17/11    ss     Added support for Iface linger.
01/03/11    ls     Check phy link state before aborting the data session
11/22/10    sn     Removed REX premption macros as it is not required and 
                   added new critical sections for CHS and SCRM.
11/03/10    mg     Added ds707 critical section global variable
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
09/13/10    op     Changed DS707_CBACK_MSG macro to use MSG 2.0 format
09/13/10    ls     Correct the mistake for updating CTA timer value
09/08/10    jy     Call EPC API for bearer tech change
09/07/10    op     IPv6 enable/disable feature for HRPD
07/19/10    ttv    Merged fix for the issue of wrong bearer tech info being 
                   informed to APP.
07/18/10    mg     PDN throttling feature for eHRPD
07/12/10    gc     Use default ps flow if meta info or ps flow don't exist 
                   or ps flow is invalid
07/08/10    ssh    Posting command to DS on PDN Inactivity timer expiry
04/27/10    ls     Add themal monitor PA support
05/21/10    gc     IPv6 call setup fix
05/20/10    gc     Use default ps flow if meta info or ps flow don't exist, 
                   or ps flow is invalid.
05/17/10    ls     Ensure that HDR/CDMA timer starts only when it is in HDR/CDMA mode
                   and data call is active
05/13/10    ssh    Added support for PDN Inactivity timer
05/04/10    mg     Dual IP feature
04/21/10    gc     Moved some of the event processing to ds707_wmk.c
04/16/10    ls     Fixed featurization issue 
04/02/10    spn    Support eHRPD + MMPA
03/25/10   spn     Give bearer tech indications when we are not in call
03/26/10    gc     Dont allow CAM to set CTA timeout to 1sec in 1x only mode
03/15/10    ls     Silent Redial Feature
03/05/10    spn    Updated the Data Bearer Event 
01/12/10    ls     Adding CSIM change back. It lost during eHRPD merge
12/10/09    ps     Mainlining OMH feature based on new profile management
                   design.
08/11/09    ca     Fixed compiler warnings.
07/29/09    spn    Set IFACE Names for DPL Logging.
07/27/09    spn    (1) Tear down all IFACES if AMSS moves from EPC to Non-
                    EPC. (2) IFACE Descriptions added. (3) Removed functions 
                    to get/set active_hdr_prot.
07/24/09    vs/ssh Setting address management f_ptrs to NULL
07/17/09    ls     Support FEATURE_CSIM
07/24/09    ss     Fixed issue of AT not closing HDR session when call 
                   origination on 1x fails during HDR to 1x handdown.
06/18/09    ls     Support operation mode change
05/29/09    sn     Fixed off target lint medium warnings.
05/26/09    ls     Support operation mode change
05/06/09    ss     Added support to write the default values of IPV6 parameters
                   to NV memory if the NV for the same is not provisioned.
05/04/09    ss     Added check for IPV6 NV Items.
04/30/09    ls     Support LPM mode change
04/29/09    ms     Fixed Lint High Errors.
04/13/09    psng   Added support to write the default values of IPV6 parameters
                   to NV memory if the NV for the same is not provisioned.
03/30/09    ls     Created ds707_pkt_mgr_data_call_support() to know if 
                   data call is supported on mode preference.
                   In ds707_pkt_mgr_process_ph_mode_pref_chg_cmd(), it 
                   shall call ds707_pkt_mgr_data_call_support() and
                   PS_PHYS_LINK_IS_VALID to abort data session if
                   it is available.
04/04/09    ms     Added code to handle the pref_mode_change_event from CM
                   and to tear down the call (dormant and active both)
03/20/09    spn    changed ds707_pkt_state to ds707_pdn_state
03/20/09    spn    Increased number of PDNs to 3 for eHRPD 
03/12/09  ssh/spn  PPP VSNCP interface changes
02/15/09    ls     Initialize epzid in ds3g_pkt_call_info_type
02/09/09    spn    Moved RMSM PDN instance to the RMSM Module
01/30/09    ls     Remove uncessary print in ds707_pkti_cta_timer_expired()
12/18/08    ss     Lint Fixes
12/07/08    ms     Fixed Offtarget Lint errors and Compiler warnings
11/20/08    sn     Fixed the issue of GO NULL timer not getting initialized  
                   for JCDMA.
11/20/08    yz     Fixed to set rereg if traffic since last rrq flag 
                   correctly
11/10/08    yz     Fixed TX/RX byte count issue with wmk approach, 
                   reverted previous changes in rlp
10/06/08    sn     Fix to make sure HDR SMP deactivate is not sent when 1x 
                   data call ends.
09/27/08    pp     Metainfo optimizations.
08/07/08    as     Lint error fixes. 
07/25/08    hs     Fixed compilation warnings
07/07/08    ms     Klocwork Fixes
06/17/08    psng   Added support for multiple app profiles for OMH.
06/17/08    hs     Fixed the SPD crash
06/12/08    sn     Fixed featurization issue.
05/14/08    hs     Removed dsatcmif.h. There was no need for the header.
04/03/08    sn     Fixed featurization issue.
04/02/08    hs     Fixed CR 142039. Introduced function 
                   ds707_pkt_mgr_update_cdsm
                   Made changes in places where CDSN was updated directly.
03/13/08    sc     Fix to enable phys link flow only after re-orig hdlr reg
02/12/08    sc     Added support to propagate CM call end reason to IFACE.
01/22/08    scb/sy Mainlined features under FEATURE_JCDMA
01/10/08    da     Added processing of DS_707_UM_RX_HI_WM_CMD and 
                   DS_707_UM_RX_LO_WM_CMD for memory based flow control 
                   on fwd link. 
11/15/07    sk     Release all the secondary flows before posting 
                   ps_flow_go_null_ind() to primary flow
10/11/07    ac     Propagate the default CTA timer value to RLP to take care 
                   of multi-mode  targets
08/30/07    sk     Enable/Disable secondary physlink bound flows when primay 
                   physlink bound flows are enabled/disabled
08/16/07    as     Enable flow if DS gets DS_707_PKT_IN_SERVICE_MODE_CMD and 
                   disable flow if DS gets DS_707_PKT_NO_SERVICE_MODE_CMD
08/16/07    spn    Lint changes.
07/02/07    sc     Added support for Application Profiles in
                   ds707_pkt_mgr_init function. 
                   Also added ds707_linger_cmd for Dormant 2 (iface
                   lingering) feature.
05/29/07  squ/mga  Added case DS_707_HDR_LN_STATUS_CMD in 
                   ds707_pkt_process_cmd
05/23/07    vp     MIP CCoA fixes.
03/20/07    ac     Fix compiling error for when HDR feature is turned off.
03/08/07   ara     Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
03/05/07    ac     Merge the change in EMPA SandBox 5209 to the tip.
02/12/07    az     Support to request VJ compression during PPP negotiation
01/29/07    ac     Fix the AT crash in ds707_pkti_reorig_sig_hdlr()
01/10/07    sq     Added function ds707_pkt_mgr_process_retry_delay_orig_cmd
12/21/06    spn    Handling the timestamp in the ds meta info as part of SPD.
12/01/06    rsj    Added ds707_pkt_mgr_set_last_orig_fail_reason and 
                   ds707_pkt_mgr_get_last_orig_fail_reason,
                   ds707_pkt_mgr_clear_last_orig_fail_reason functions to 
                   operate on global mode handler information structure.
12/18/06    spn    Introduced DS_META_INFO in place of PS_META_INFO
11/16/06    sq     Added a function to process cm_ph_sys_sel_pref event.
11/12/06    sq     1. When received a phys_link_down_ev, if the reason 
                      is phone in 911 call, disable flow.
                   2. When received a phys_link_up_ev, enable flow.
11/09/06    vrk    Post command to DS to process tx high and low wmk
10/13/06    az     Support for IPv6 
09/11/06    as     Added posting of DCTM success for a call coming out of 
                   dormancy.
09/05/06    ks     Updated ds707_bring_up_cmd() to use new struct
                   network_params_info_type and save it in ps_iface.
07/20/06    kvd    modified hdrmrlpsched_unbind/bind wm_rlp calls 
                   (multi-proc re-arch ).- resides on apps side.
07/19/06    aw     Port Sc2x featurization changes to 6800
07/18/06    az     IPV6 changes to populate the IpV6 watermark.
06/07/06    ssh    In ds707_pkt_mgr_init(), now calling ppp_get_hdlc_handle() 
                   instead of allocating an HDLC instance 
05/30/06    ksu    EMPA: RLP init code review comments
05/18/06    gr     Fix to send DRS=0 orig for IDM change after PHYS_LINK
                   goes down (if IDM Change was received before CALL_END)
05/04/06    ksu    EMPA: RSP part 2
04/16/06    gr/sq  Disable phys_link flow if call orig fails because a voice
                   call is up. When voice call is ended (EXIT_TC), the flow 
                   is enabled.
03/30/06    spn    Fixed Featurization for FEATURE_HDR_QOS
03/27/06    sy     Updated the signal_hdlr callback behaviour from PS.
03/23/06    ksu    convert RX PS command back to signal
03/16/06    sk     Updated watermark behavior
03/02/06    az     Removed the call to silently cleanup secondary flows
03/01/06    spn    Changed Debug Message as it was misleading   
02/21/06    az     Added method to silently cleanup all secondary flows
02/22/06    ksu    don't float more than one RX command at a time
01/05/06    ksu    EMPA: rx queues per tc state
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/29/05    ksu    EMPA: framework for route protocol (RP)
12/29/05    ksu    EMPA: trivial route selection protocol (RSP)
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
08/29/05    ksu/sy Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                   parameter to dsrlp_reg_srvc().
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
09/09/05    vrk/gr  EPZID enhancements releated to L2/L3 ack
09/07/05    gr/vrk enabled hdlc_mode to HDLC_HW_PREF
09/09/05    jd     Setup auth parameters in ps_iface from user_data provided
                   to bringup cmd.
08/30/05    ks     Removed FEAUTRE_DATA_PS_DATA_LOGGING_FLOW feature around 
                   flow logging and mainlined in under the DPL feature, 
                   FEATURE_DATA_PS_DATA_LOGGING.
08/08/05   sk/sy   Merged semi-dynamic QOS changes from sandbox.
05/31/05    gr     Set the last_data_sess_network to NO_SRV when the pkt 
                   call goes NULL upon receiving an ABORT_SESSION event
05/27/05    gr     modified to make sockets work temp fix 
                   before HDR DATA QOS merge
05/23/05    spn    Added a check in the function ds707_pkt_tx_um_SN_data() 
                   so that if SDB is needed then we check whether we can do 
                   SDB. 
05/16/05    ks     made temp fixes for HDR_QOS to compile with PS.10.00.00 VU
04/26/05    gr     Added inc_call flag to ds3g_pkt_call_info_type. Also
                   differentiated the inc_call with the flag set to TRUE 
04/26/05    vrk    Created events to enable automated testing
04/07/05    atp    Added support for DOS.                   
04/05/05    gr     Fixed the issue of populating hdlc_inst_ptr for MDRRLP
04/05/05    gr     Integrated EPZID functionality onto the main line
03/07/05    vas    Removed inclusion of ds707_sec_pkt_mgri.h
03/02/05    ks     Added call to ps_iface_dpl_set_iface_desc() to set the
                   iface description.
03/01/05    sk     Fixed CTA TIMER issue
03/01/05    kvd    Added 2 new functions for RDUD module to set/reset 
                   dsat707_cta_val.
01/31/05    sy     Fixed the flow control issue on the Um interface in 
                   HDR mode.
01/26/05    gr     Fixed the issue of LocNot not being sent during subnet 
                   handoffs in HICPS mode
01/12/05    ks     Added support for Data Protocol Logging.
10/14/04    msr    Calling PS_IFACE_SET_GRACEFUL_CLOSE() when an interface is
11/30/04    kvd    Added new proto-types for CAM module.
11/29/04    gr     Merged PZID hysteresis changes into main line
11/22/04    vas    Minor changes in transmit function for QOS
09/28/04    kvd    Initialized dssnet instance in bring_up_cmd/tear_down_cmd.
09/24/04   vas/sy  Fixed null tx_wm_ptr issue when the QOS feature is turned 
                   off.
09/10/04    vas    QOS related changes
08/23/04    mct    Removed duplicate include.
08/20/04    mvl    Removed IPv6 NV reads outside of initialization loops.
08/18/04    kvd    Moved failover iface settings to dssnetAny_sm_create()
                   from ds707_pkt_mgr_init().
08/10/04    mvl    Using the default #defines for IPv6 parameters.
08/09/04    mct    Support for ANY iface hdlr and v4/v6 failover support.
08/06/04    ak     Added handling for PZID changed event (do a reconnect).
07/30/04    kvd    Added new fns, ds707_tear_down_cmd, ds707_bring_up_cmd.
07/30/04    mct    Fixed potential issue with the ip6 sm_config struct.
06/03/04    mct    Now passing in V6 state machine parameters from NV into 
                   ip6_sm_create. Also checking NV item ipv6_enabled, and if
                   false don't create v6 iface, dssnet6, or ip6_sm.
05/03/04    sy     Added rx_sig_f_ptr handler for ds707_pkt_setup_SN_ppp(),
                   which helps in using the hdlc deframer library.
04/23/04    rc     Fixed the solication parameters passed into ip6_sm_create
04/24/04    ak     Lint fixes.
04/21/04    mct	   Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
04/11/04    gr     Set the value of force_cdma_only to FALSE for all
                   dormancy re-origination scenarios
04/06/04 vas/ak    For non SDB, extract meta info from dsm item before 
                   enquing  it on the transmit watermark
03/22/04    ak     See if okay to orig from dorm on re-origs.
03/15/04    ak     Merge to 3/15 IPV6 build.
03/09/04    ak     Make event callback take a const.
02/19/04    ak     Changes to support IPV4 and IPV6.
11/06/03    ak     Added ds707_pkt_trash_pri_rlp_tx_wmks api to flush the 
                   outgoing RLP queue.  Used by higher layers when 
                   reconfiguring link.
11/04/03    rsl    Added code under FEATURE_HDR to get correct data in 
                   get_data_count_hdlr and get_call_info_hdlr.
10/30/03    vr     Inform hdrpac if there is a handdown from HDR to 1X under
                   FEATURE_HDR_PPP_RESYNC_ON_HANDDOWN
10/19/03    vas    End Key causes PPP abort if 
                   FEATURE_HDR_SESSION_TIED_TO_PPP IS DEFINED.
10/28/03    ak     Fixes for JCDMA service options.  On incoming call, see
                   if packet allowed.  When transitioning in and out of
10/13/03    vr/kvd Added support for ALN and PPP resync on handdown to 1X
10/13/03    vas    Changed hdr rlp initialization/cleanup. Added code that
                   will register functions for RLP initialization when Data 
                   Session initiated
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
  #define FEATURE_DSM_WM_CB

#ifdef FEATURE_EPC_HANDOFF
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#endif /* FEATURE_EPC_HANDOFF */

  #include "amssassert.h"
  #include "cm.h"
  #include "mc.h"
  #include "ds3gcmif.h"
  #include "ds3gmgr.h"
  #include "ds3gcfgmgr.h"
  #include "ds3gutil.h"
  #include "ds3gmshif.h"
  #include "ds3gtimer.h"
  #include "ds3geventmgr.h"
  #include "ds707.h"
  #include "ds707_cback.h"
  #include "ds707_cta_rlp.h"
  #include "ds707_diag.h"
  #include "ds707_dorm_timer.h"
  #include "ds707_drs.h"
  #include "ds707_gonulltmri.h"
  #include "ds707_hi_spd_clk.h"
  #include "ds707_pkt_mgr.h"
  #include "ds707_pkt_mgr_hdlr.h"
  #include "ds707_pkt_mgr_hdr.h"
  #include "ds707_pkt_mgr_iface.h"
  #include "ds707_pkt_mgr_phys_link.h"
  #include "ds707_pkt_mgri.h"
  #include "ds707_epzid_mgr.h"
  #include "ds707_rmsm.h"
  #include "ds707_rrm.h"
  #include "ps_aclrules.h"  /* needed because ds707_rt_acl missing include */
  #include "ds707_rt_acl.h"
  #include "ds707_so33.h"
  #include "ds707_so_async.h"
  #include "ds707_so_pkt.h"
  #include "ds707_so_retrydel.h"
  #include "ds707_timer.h"
  #include "ds707_wmk.h"
  #include "ds707_iface4_hdlr.h"
  #include "ds707_p_rev.h"
  #include "ds707_sec_pkt_mgr_handoff.h"
  #include "ds707_roaming.h"
  #include "ds707_cmd_hdlr.h"
  #include "dsgen_iface_hdlr.h"
  #include "ds_flow_control.h"
  #include "dsat707util.h"
  #include "dsat707vendctab.h"
  #include "dsm.h"
  #include "dsrlp_v.h"
  #include "dstaski.h"
  #include "dsutil.h"
  #include "err.h"
  #include "event.h"
  #include "fs_errno.h"
  #include "mccsrid.h"
  #include "msg.h"
  #include "data_msg.h"
  #include "ps_acl.h"
  #include "ps_iface.h"
  #include "ps_phys_link.h"
  #include "ps_ppp.h"
  #include "ps_ppp_ipcp.h"
  #include "ps_meta_info.h"
  #include "ds1x_meta_info.h"
  #include "ds707_event_defs.h"
  #include "ds707_pkt_mgr_flow.h"
  #include "dstask.h"
  #include "dstask_v.h"
  #include "ds707_nv_util.h"
  #include "ds707_async_defs.h"
  #include "ds3gmgrint.h"
  #include "hdrcom_api.h"
  #include "dsmip_regsm.h"
  #include "ds707_sys_chg_hdlr.h"
  #include "ps_crit_sect.h"
  #include "dsgen_iface6_hdlr.h"
  #include "event_defs.h"
  #include "ps_wmk_logging.h"
  #include "ds707_tm_mgr.h"
  #include "ds3gdsdif.h"

  #ifdef FEATURE_DATA_WMK_CONFIG
    #include "ds707_wmk_lvl.h"
  #endif /*FEATURE_DATA_WMK_CONFIG*/

  #ifdef FEATURE_DATA_PS_IPV6
    #include "dsgen_iface6_hdlr.h"
    #include "ps_ip6_sm.h"
    #include "ds707_ifaceAny_hdlr.h"
  #endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_3GPP2_VS
#include "ds_3gpp2_vendor.h"
#endif /* FEATURE_DATA_3GPP2_VS */

  #include "dshdr_dhcp_mgr.h"
  #include "ds707_jcdma_m51.h"
  #include "ds707_jcdma_apprate.h"
  #include "ds707_so_jcdma.h"

  #ifdef FEATURE_IS95B_MDR
    #include "mdrrlp.h"
  #else
    #include "rlp.h"
  #endif

  #if defined(FEATURE_DATA_IS2000_SDB) || defined(FEATURE_HDR_DOS)
    #include "ds707_sdb.h"
    #include "mccdma.h"
  #endif /* FEATURE_DATA_IS2000_SDB || FEATURE_HDR_DOS */


  #ifdef FEATURE_HDR
    #include "hdrrlp.h"
    #include "hdrpacdsif.h"
    #include "hdrpac.h"
    #include "hdrds.h"
    #include "hdrrlplog.h"
  #endif /* FEATURE_HDR */

  #include "hdrpacdsif.h"
  #include "hdrpacrlpctl.h"

  #include "ps_iface_defs.h"

  #ifdef FEATURE_HDR_QOS
    #include "dsrlpi.h" 
    #include "ds707_sec_pkt_mgr.h"
    #include "ds707_sec_pkt_mgr_util.h"
    #include "ds707_sec_pkt_mgr_handoff.h"
    #ifdef FEATURE_NIQ_EHRPD
      #include "ds707_nw_init_qos_mgr.h"
    #endif /*FEATURE_NIQ_EHRPD*/
    #ifdef FEATURE_HDR_EMPA
      #include "hdrrsp.h"
    #endif /* FEATURE_HDR_EMPA */
  #endif /* FEATURE_HDR_QOS */

  #ifdef FEATURE_DS_CAM
    #include "dscam.h"
  #endif /* FEATURE_DS_CAM */

    #include "ps_iface_logging.h"
    #include "ps_logging_defs.h"

  #if defined(FEATURE_HDR_QOS) || defined(FEATURE_DS_RDUD)
    #include "dsat707extctab.h"
  #endif /* FEATURE_HDR_QOS || FEATURE_DS_RDUD */

  #include "ds_1x_profile.h"
  #include "ds707_sec_pkt_mgr.h"

  #ifdef FEATURE_CALL_THROTTLE
    #include "dsdctm.h"
  #endif /* FEATURE_CALL_THROTTLE */

  #include "ds707_data_session_profile.h"
  #if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DATA_PS_MIP_CCOA)
    #include "ps_mipccoa_iface_hdlr.h"
  #endif

#ifdef FEATURE_DS_MOBILE_IP
  #include "dsmip_v.h"
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  #include "ds707_pkt_auto_redial.h"
#endif

#include "ds707_roaming.h"
#include "ds707_wmk.h"
#include "ds707_extif.h"
#include "data_err_suppr.h"
#include "ds707_sys_chg_hdlr.h"
#include "ds707_pdn_context.h"
#include "mcfg_fs.h"

#ifdef FEATURE_EHRPD
  #include "ds_ehrpd_pdn_mgr.h"
  #include "ds707_devmgr.h"
  #include "ps_ppp_vsncp_3gpp2.h"
#endif

#ifdef FEATURE_EPC_HANDOFF
  #include "ds_epc_hdlr.h"
#endif /* FEATURE_EPC_HANDOFF */

#if defined(FEATURE_CCM_FLOW_CONTROL)
  #include "ds707_ccmi.h"
#endif

#include "dsat707mip.h"
#include "ds707_ccm_log.h"

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
  #include "hdrcom_api.h"
  #include "ds707_s101_vctm.h"
  #include "ds707_s101_dctm.h"
#endif /* FEATURE_DATA_OPTHO */

#include "ds707_pkt_mgr_ioctl.h"

#include "ps_pkt_info_utils.h"
#include "pstimer.h"
#include "ds707_wmk.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
  #include "ds_sys.h"
#endif

#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
#include "dsmip_parse.h"
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
#include "iwlan_iface_ext.h"
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  #include "ds707_pkt_classifier.h"
#endif

#define DS707_DATA_EFS_CONF_FILE_PATH \
         "/nv/item_files/conf/data_config_info.conf"

#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
#define MIP_FAIL_DO_SESS_CLOSE_NV_PATH \
     "/nv/item_files/data/3gpp2/mip_fail_do_sess_close"
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/

#define DS707_ADDITIONAL_DIAL_STR_NUM  2  
#define DS707_ADDITIONAL_DIAL_STR_LEN  20
#define DS707_ADDITIONAL_DIAL_STR_EFS_PATH \
       "/nv/item_files/data/3gpp2/additional_dial_string"

#define DS707_IGNORE_LAST_PDN_IA_EFS_PATH \
          "/nv/item_files/data/3gpp2/ignore_last_pdn_ia_timer"

#define DS707_DSM_SMALL_ITEM_FEW_MARK  50
#define DS707_DSM_SMALL_ITEM_MANY_MARK 100

/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/
/*-------------------------------------------------------------------------
  Global variable for 707 critical sections.
-------------------------------------------------------------------------*/
rex_crit_sect_type ds707_global_crit_sect;
rex_crit_sect_type ds707_chs_global_crit_sect;
rex_crit_sect_type ds707_scrm_global_crit_sect;

/*-------------------------------------------------------------------------
  Global variable for current subs id.
-------------------------------------------------------------------------*/
static sys_modem_as_id_e_type curr_subs_id=SYS_MODEM_AS_ID_1;

/*-------------------------------------------------------------------------
  The last bearer tech. This tells whether the last bearer was 1x / HDR, 
  and if HDR< it tells, if it was Rev 0 or Rev A, and if it was Rev A, it 
  tells if it was DPA / MFPA / EMPA.  
-------------------------------------------------------------------------*/
static ps_iface_bearer_technology_type ds707_last_bearer_tech;

/*-------------------------------------------------------------------------
  Used to determine the current bearer. This stores the current system 
  mode information as passed by CM
-------------------------------------------------------------------------*/
static sys_sys_mode_e_type ds707_current_sys_mode = SYS_SYS_MODE_NO_SRV;

#ifdef FEATURE_DATA_PS_IPV6
/*---------------------------------------------------------------------------
  Global variable which is updated from the NV during powerup if v6 is 
  enabled or not for HRPD. Default value is FALSE.
---------------------------------------------------------------------------*/
static boolean ds707_hrpd_ipv6_enabled;

#define HRPD_IPV6_DEFAULT_VAL FALSE

/*--------------------------------------------------------------------------
  Structure used to read the EFS item from the item file in EFS
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  boolean data_hrpd_ipv6_enabled;
} hrpd_ipv6_nv_from_efs;
#endif /* FEATURE_DATA_PS_IPV6 */

/*---------------------------------------------------------------------------
  Global variable which indicates whether DSM based flow control is enabled or 
  disabled depending on the DSM memory events.
---------------------------------------------------------------------------*/
 boolean ds707_dsm_fc_flag = FALSE;

/*-------------------------------------------------------------------------
  To complete the PPP resync without interruption from GPS
  ppp_resync_lock_release_cb will be called upon successfully releasing
  the ppp_resync_lock, if a valid callback is registered
-------------------------------------------------------------------------*/
void (*ds707_pkt_mgr_ppp_resync_lock_release_cb)(void);

#ifdef FEATURE_DS_RDUD
/*---------------------------------------------------------------------------
  Variable to save the original CTA value during RDUD 
---------------------------------------------------------------------------*/
static dword ds707_pkti_rdud_save_cta_val =0;
#endif /* FEATURE_DS_RDUD */


#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM
/*---------------------------------------------------------------------------
 Global variables maintained by DS707 for use by CAM module.
---------------------------------------------------------------------------*/
static dword ds707_pkti_save_cta_val=0;
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */

/*---------------------------------------------------------------------------
 Global variable to store information about the last call that ended.
 Currently uses CM end call reason to satisfy JCDMA JATE requirement. Also
 maps this CM end call reason to PS NET down reason.
 Need to propogate end call reason to applications.
---------------------------------------------------------------------------*/
static ds707_mode_handler_information_type ds707_mode_handler_info;

/* Decided wheher the MIP call works in CCoA mode */
static boolean dshdr_mobile_ip_uses_ccoa = FALSE;

#define DS707_ROAM_CUST_HOME_BYTE_LEN  32
/* Max allowed roam indicators are 255, hence we declary 32 byte array , so that
   each bit corresponds to a roam indicator value up to 255 */
/* Bit positions 64, 65 and 76 through 83 (inclusive) have been turned on in
** the default case to accomodate the custom home values used by Verizon.
*/

static byte ds707_roam_ind_custom_home[DS707_ROAM_CUST_HOME_BYTE_LEN] =  
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*---------------------------------------------------------------------------
  Global variable to store prefered mode of AT (Hybrid ON or OFF)
---------------------------------------------------------------------------*/
static boolean ds707_cm_hybrid_pref;
#endif

/*---------------------------------------------------------------------------
  Global variable to store NV_IPV6_FAILOVER_CONFIG_I read at powerup.
  This is failover mode to set addr family in case of dual ip when policy 
  specifies UNSPEC
---------------------------------------------------------------------------*/
static ip_addr_enum_type ds707_ip_addr_failover_mode = IFACE_IPV4_ADDR_FAMILY;

/*---------------------------------------------------------------------------
  Global boolean to determine hdr hybrid status
---------------------------------------------------------------------------*/
//static boolean ds707_hdr_hybrid_status = FALSE;
#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
static boolean ds707_mip_auth_fail_close_sess = FALSE;
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*---------------------------------------------------------------------------
  eHRPD to HRPD fallback config variable, holds NV config.
---------------------------------------------------------------------------*/
static ds707_pkt_mgr_ehrpd_to_hrpd_fb_config_type 
                                       ds707_pkt_mgr_ehrpd_to_hrpd_fb_config;

/*---------------------------------------------------------------------------
  Control block for eHRPD to HRPD fallback.
---------------------------------------------------------------------------*/
static ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb_type 
                                        ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

/*---------------------------------------------------------------------------
  Variable to store the dns_v4_addr_req_type NV item
---------------------------------------------------------------------------*/
static vsncp_3gpp2i_dns_v4_addr_req_type
             ds707_pkt_mgr_dns_v4_addr_req = VSNCP_3GPP2_DNS_V4_REQ_DEFAULT;

#endif /* FEATURE_EHRPD */

#ifdef FEATURE_MMGSDI_SESSION_LIB
static mmgsdi_jcdma_card_info_enum_type ds707_jcdma_card_info;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifdef FEATURE_DATA_FALLBACK_LIST
/*---------------------------------------------------------------------------
  The failure count for the PDN connections listed in fallback list
---------------------------------------------------------------------------*/
static uint8 ds707_fb_pdn_failure_count = 0;
/*---------------------------------------------------------------------------
  The max number of  failures configured in NV for the PDNs that are listed
  in the fallback list before falling back to HRPD. If set to 0, then PDN
  failures shall not trigger HRPD fallback
---------------------------------------------------------------------------*/
static uint8 ds707_max_fb_pdn_failure_count = 0;
#endif /*FEATURE_DATA_FALLBACK_LIST*/

/*-----------------------------------------------------------------------------
  Counter to store number of mip to sip transitions happened from powerup to
  powerdown. Note that not per call basis.
-----------------------------------------------------------------------------*/
static uint32 num_mip_to_sip_transitions;

/*---------------------------------------------------------------------------
  each string might not end up with '\0' if that string length is the maximun length
---------------------------------------------------------------------------*/
static byte ds707_pkt_mgr_additional_dial_str[DS707_ADDITIONAL_DIAL_STR_NUM][DS707_ADDITIONAL_DIAL_STR_LEN];

/*---------------------------------------------------------------------------
  Whether to ignore the inactivity timer for the last pdn connection, if set 
  to TRUE, the inactivity timer for the last pdn will be restarted and this pdn 
  keeps in connection when the timer expires. 
---------------------------------------------------------------------------*/
static boolean ds707_ignore_last_pdn_ia_timer = FALSE;

/*---------------------------------------------------------------------------
  Global variable to store if any termination is in progress.
  If it is TRUE, we shall not allow a new call to be originated.
---------------------------------------------------------------------------*/
static boolean ds707_term_in_progress = FALSE;

/*---------------------------------------------------------------------------
  Global variable to store the iface pointer which triggered UE re-originate
  out of dormancy.
---------------------------------------------------------------------------*/
static ps_iface_type* reorig_iface_ptr = NULL;

static ds707_back_pressure_params_type ds707_back_pressure_params;

/*---------------------------------------------------------------------------
  Global variable to hold the efs file path for 
  NV_TRAFFIC_CHANNEL_TEARDOWN_IN_LINGERING
---------------------------------------------------------------------------*/
static char ds_nv_path_traffic_channel_teardown_in_lingering[]
		  = "/nv/item_files/data/common/traffic_channel_teardown_in_lingering";
	

static char nv_item_file_path[] = 
"/nv/item_files/data/common/dsmgr/a2_mdm_config_info\n\
/nv/item_files/data/3gpp2/disable_cs_data_service\n\
/nv/item_files/data/3gpp2/minapnlist_disallow_call\n\
/nv/item_files/data/3gpp2/ehrpd_partial_context\n\
/nv/item_files/data/3gpp2/ds_ppp_ctl_packet_on_dos\n\
/nv/item_files/data/3gpp2/ignore_last_pdn_ia_timer\n\
/nv/item_files/data/3gpp2/additional_dial_string\n\
/nv/item_files/data/3gpp2/mip_fail_do_sess_close\n\
/nv/item_files/data/3gpp2/ehrpd_to_hrpd_fallback\n\
/nv/item_files/data/3gpp2/max_fb_pdn_failure_count\n\
/nv/item_files/data/3gpp2/hrpd_1x_ipv6_config_info\n\
/nv/item_files/data/1x/707/ds707_ehrpd_mtu\n\
/nv/item_files/data/1x/707/ds707_hrpd_mtu\n\
/nv/item_files/data/3gpp2/prereg_backoff_timer_lcp\n\
/nv/item_files/data/3gpp2/prereg_backoff_timer_virtual_conn\n\
/nv/item_files/data/3gpp2/um_rm_watermark_parameters\n\
/nv/item_files/data/3gpp2/internal_auth\n\
/nv/item_files/data/3gpp2/mpit_enabled\n\
/nv/item_files/data/3gpp2/always_on_config_info\n\
/nv/item_files/data/3gpp2/epc_data_context_duration\n\
/nv/item_files/data/3gpp2/disable_eHRPD_PDN_type_override\n\
/nv/item_files/data/3gpp2/dns_v4_addr_req_type\n\
/nv/item_files/jcdma/jcdma_mode\n\
/nv/item_files/data/3gpp2/3gpp2_back_pressure_removal\n\
/nv/item_files/data/3gpp2/NonEPC_data_session_cleanup\n\
/nv/item_files/data/3gpp2/request_mtu_pco_in_vsncp\n\
/nv/item_files/data/3gpp2/epc_handoff_retry_count\n\
/nv/item_files/data/3gpp2/enable_nw_init_qos_on_ehrpd\n\
/nv/item_files/data/3gpp2/resv_on_req_throttle_timer\n\
/nv/item_files/data/3gpp2/qos_data_flow_inactivity_timer\n\
/nv/item_files/data/3gpp2/nw_init_ppp_resync_wait_timer\n\
/nv/item_files/data/3gpp2/nw_init_qos_fwd_resv_on_from_ue\n\
/nv/item_files/data/3gpp2/1x_to_LTE_handup_enabled\n\
/nv/item_files/data/3gpp2/epc_ho_thrtl_timer\n";

/*===========================================================================
                       FORWARD DECLARATION
===========================================================================*/
static ps_extended_info_code_enum_type ds707_pkt_mgr_get_eic_end_reason
(
  void
);

void ds707_dsm_mem_event_cb
(
  dsm_mempool_id_enum_type pool_id,
  dsm_mem_level_enum_type  level,
  dsm_mem_op_enum_type     mem_op
);

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT_IGNORE_LAST_PDN_IA_TIMER_NV

DESCRIPTION   This function reads the ignore_last_pdn_ia_timer NV. 

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_init_ignore_last_pdn_ia_timer_nv(void)
{
  nv_stat_enum_type  status;

  status = ds3gcfgmgr_read_efs_nv_ex(DS707_IGNORE_LAST_PDN_IA_EFS_PATH, 
                          &ds707_ignore_last_pdn_ia_timer, 
                          sizeof(ds707_ignore_last_pdn_ia_timer), ds707_curr_subs_id()); 
  if (0 != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "EFS item not configured for ignore_last_pdn_ia_timer "
        "Initializing defaults.");
    ds707_ignore_last_pdn_ia_timer = FALSE;
  }
} /* ds707_pkt_mgr_init_ignore_last_pdn_ia_timer_nv */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_ADDITIONAL_DIAL_STR_INIT

DESCRIPTION   This function reads nv for DS707_ADDITIONAL_DIAL_STR_EFS_PATH 
              for addional dial string provisioning 

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_additional_dial_str_init(void)
{
  nv_stat_enum_type    read_status = NV_DONE_S;
  
  memset(ds707_pkt_mgr_additional_dial_str, 0, 
         DS707_ADDITIONAL_DIAL_STR_NUM*DS707_ADDITIONAL_DIAL_STR_LEN);

  read_status = ds3gcfgmgr_read_efs_nv_ex(DS707_ADDITIONAL_DIAL_STR_EFS_PATH, 
                ds707_pkt_mgr_additional_dial_str, 
                DS707_ADDITIONAL_DIAL_STR_NUM*DS707_ADDITIONAL_DIAL_STR_LEN, ds707_curr_subs_id());
  
  if(read_status != NV_DONE_S)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
        "ds707_pkt_mgr_additional_dial_str not configured" );
    memset(ds707_pkt_mgr_additional_dial_str, 0, 
           DS707_ADDITIONAL_DIAL_STR_NUM*DS707_ADDITIONAL_DIAL_STR_LEN);
  }
}

#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT_MIP_DO_SESS_CLOSE_NV

DESCRIPTION   This function reads nv on whether to close session 
              upon mip failure with cause err 67 (authentication error),
              and records it in static variable

DEPENDENCIES  None

RETURN VALUE  TRUE: to close session 
              FALSE: do NOT close session 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_init_mip_do_sess_close_nv(void)
{
  nv_stat_enum_type    read_status = NV_DONE_S;

  read_status = ds3gcfgmgr_read_efs_nv_ex(MIP_FAIL_DO_SESS_CLOSE_NV_PATH, 
                               &ds707_mip_auth_fail_close_sess, 
                               sizeof(boolean), ds707_curr_subs_id());
  if(read_status != NV_DONE_S)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
        "mip_do_sess_close_nv not configured set to default value." );
    ds707_mip_auth_fail_close_sess = FALSE;
  }
}
/*===========================================================================
FUNCTION      DS707_PKT_MGR_MIP_CAN_DO_SESS_CLOSE

DESCRIPTION   This function returns whether to close session upon mip failure 
              with cause err 67 (authentication error) 

DEPENDENCIES  None

RETURN VALUE  TRUE: to close session 
              FALSE: do NOT close session 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_mgr_mip_can_do_sess_close(byte mip_rrp_code)
{
  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                    "mip_do_sess_close_nv: %d, RRP code:%d",  
                   ds707_mip_auth_fail_close_sess, mip_rrp_code);
  return (ds707_mip_auth_fail_close_sess &&
    (mip_rrp_code==MIP_RRP_CODE_FAILURE_FA_MOBILE_NODE_FAILED_AUTH));
}
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/
#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION      DS707_PKT_MGR_READ_EHRPD_TO_HRPD_FB_NV

DESCRIPTION   This function reads the ehrpd_to_hrpd_fb NV. This NV specifies 
              ehrpd to hrpd fallback configuration.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_read_ehrpd_to_hrpd_fb_nv(
  void 
)
{
  char item_file_path[] = "/nv/item_files/data/3gpp2/ehrpd_to_hrpd_fallback";

  nv_stat_enum_type  status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Read the eHRPD to HRPD fallback config from the EFS item
   -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_efs_nv_ex(item_file_path, 
                          &ds707_pkt_mgr_ehrpd_to_hrpd_fb_config, 
                          sizeof(ds707_pkt_mgr_ehrpd_to_hrpd_fb_config), ds707_curr_subs_id()); 
  if (0 != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "EFS item not configured for ehrpd to hrpd fallback, "
        "Initializing defaults.");
    ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.enabled = TRUE;
    ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.max_hard_fallback_count = 
                                                 DEFAULT_MAX_HARD_FAILURE_COUNT;
    ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.max_soft_fallback_count = 
                                                 DEFAULT_MAX_SOFT_FAILURE_COUNT;
    ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.propose_ehrpd_after_irat = TRUE;
    ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.is_ehrpd_roaming_disabled = FALSE;
  }

  /*-------------------------------------------------------------------------
    Initialize ehrpd to hrpd control block.
   -------------------------------------------------------------------------*/
  memset( &ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb, 
          0, 
          sizeof(ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb) );
  return;
} /* ds707_pkt_mgr_read_ehrpd_to_hrpd_fb_nv */
#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_PKT_MGR_FB_LIST_INIT

DESCRIPTION   This function initialize the parameters related to fallback pdn 
              list 

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_fb_list_init(void)
{
  char item_file_path[] = "/nv/item_files/data/3gpp2/max_fb_pdn_failure_count";

  nv_stat_enum_type  status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Read the max_fb_pdn_failure_count from the EFS item
   -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_efs_nv_ex(item_file_path, 
                          &ds707_max_fb_pdn_failure_count, 
                          sizeof(uint8), ds707_curr_subs_id()); 
  if (0 != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "EFS item not configured for ds707_max_fb_pdn_failure_count "
        "Initializing defaults.");
    ds707_max_fb_pdn_failure_count = 0;
  }
}
#endif /*FEATURE_DATA_FALLBACK_LIST*/
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_READ_DNS_V4_ADDR_REQ_NV

DESCRIPTION   This function reads the dns_v4_addr_req_type NV. This NV
              specifies the method in which to obtain the DNS IPv4 address
              during VSNCP negotiation.

              NV#67335 - /nv/item_files/data/3gpp2/dns_v4_addr_req_type

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_read_dns_v4_addr_req_nv(void)
{
  char item_file_path[] = "/nv/item_files/data/3gpp2/dns_v4_addr_req_type";

  nv_stat_enum_type  status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Read the ds707_pkt_mgr_dns_v4_addr_req from the EFS item.
    If the item is not present or it is invalid, the default value
    will be used.
   -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
                          &ds707_pkt_mgr_dns_v4_addr_req,
                          sizeof(uint8), ds707_curr_subs_id());
  if ( 0 != status ||
       ds707_pkt_mgr_dns_v4_addr_req >= VSNCP_3GPP2_DNS_V4_REQ_MAX )
  {
    ds707_pkt_mgr_dns_v4_addr_req = VSNCP_3GPP2_DNS_V4_REQ_DEFAULT;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "eHRPD DNS v4 Address request type set, "
                  "ds707_pkt_mgr_dns_v4_addr_req = %d",
                  ds707_pkt_mgr_dns_v4_addr_req);
}
#endif /* FEATURE_EHRPD */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_FAILOVER_IP_MODE

DESCRIPTION   This function reads the failover ip type. Uses the failover mode 
              to set addr family of iface incase of dual IP when ip family in
              policy is UNSPEC

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_set_failover_ip_mode(
  void 
)
{
  ps_iface_ip_ver_failover_e_type failover_mode;   
  
  failover_mode = ds3g_get_failover_mode();
   
  if ( (failover_mode == IPV6_PREFERRED) ||
       (failover_mode == IPV6_DESIRED) )
  {
    ds707_ip_addr_failover_mode = IFACE_IPV6_ADDR_FAMILY;
  }
  else if ( (failover_mode == IPV4_PREFERRED) || 
            (failover_mode == IPV4_DESIRED))
  {
    ds707_ip_addr_failover_mode = IFACE_IPV4_ADDR_FAMILY;
  }

  DATA_IS707_MSG1( MSG_LEGACY_MED, "IP failover type set to %d", 
                   ds707_ip_addr_failover_mode );
} /* ds707_pkt_mgr_set_failover_ip_mode() */

/*===========================================================================
FUNCTION      DS707_PKT_MGRI_GET_IP_FAILOVER_MODE

DESCRIPTION   This function returns the value of IP failover mode. This is 
              called to resolve address family when policy says UNSPEC
              and profile says V4_V6 for a call.

DEPENDENCIES  None.

RETURN VALUE  ip_addr_enum_type.

SIDE EFFECTS  None.
===========================================================================*/
ip_addr_enum_type ds707_pkt_mgri_get_ip_failover_mode(void)
{
  return ds707_ip_addr_failover_mode;
}

/*===========================================================================
FUNCTION      DS707_IS_HRPD_IPV6_ENABLED

DESCRIPTION   Returns whether IPv6 is enabled through EFS item or not for HRPD.

DEPENDENCIES  None.

RETURN VALUE  Boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_hrpd_ipv6_enabled(void)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED, "hrpd_1x_ipv6_enabled = %d",
                  ds707_hrpd_ipv6_enabled);
  return ds707_hrpd_ipv6_enabled;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS707_PKTI_REORIG

DESCRIPTION   Called when the RLP TX watermark goes non-empty when
              dormant.  Sets a signal so that the DS task goes ahead
              and processes this function.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715
 *           - unused params 
 */
void ds707_pkti_reorig
(
struct dsm_watermark_type_s *wm_ptr,
void                        *user_data_ptr
)
{
  DATA_MDM_ARG_NOT_CONST(wm_ptr);
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "reorig signal for 707 pkt");
  (void)rex_set_sigs(dsi_get_ds_tcb(), DS_PKT_REORIG_SIG);
} /* ds707_pkti_reorig() */
/*lint -restore */

/*===========================================================================
FUNCTION      DS707_PKTI_DEREG_PRI_WMK_REORIG_CBACKS

DESCRIPTION   Called when need to de-register the RLP watermark callbacks
              which can trigger us out of dormancy.  Called when either
              we originate, get an incoming call, or connect, or go NULL.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_dereg_pri_wmk_reorig_cbacks
(
ds707_tc_state_type   *tc_state_ptr
)
{
  ds707_flow_type *flow_ptr;
  int p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(tc_state_ptr);

  /*-------------------------------------------------------------------------
    Set the each_enqueue wmk cbacks to NULL, which will prevent dormant
    reoriginations when data goes to RLP.  Clear signal in-case incoming
    data is showing up at the same time...

    Note that this can happen only for the default tc. 
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
  -------------------------------------------------------------------------*/
  flow_ptr = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    flow_ptr->tx_wm_list[p].each_enqueue_func_ptr = NULL;
  }
  /*lint -e539 adjusted indentation*/
  (void)rex_clr_sigs(dsi_get_ds_tcb(), DS_PKT_REORIG_SIG);
  /*lint -restore*/

} /* ds707_pkti_dereg_pri_wmk_reorig_cbacks() */

/*===========================================================================
FUNCTION      DS707_PKTI_REG_PRI_WMK_REORIG_CBACKS

DESCRIPTION   Called when need to de-register the RLP watermark callbacks
              which can trigger us out of dormancy.  Called when either
              we originate, get an incoming call, or connect, or go NULL.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_reg_pri_wmk_reorig_cbacks
(
ds707_tc_state_type   *tc_state_ptr
)
{
  ds707_flow_type *flow_ptr;
  int p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(tc_state_ptr);

  /*-------------------------------------------------------------------------
    Set the each_enqueue wmk cbacks to NULL, which will prevent dormant
    reoriginations when data goes to RLP.  Clear signal in-case incoming
    data is showing up at the same time...
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
  -------------------------------------------------------------------------*/
  flow_ptr = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    flow_ptr->tx_wm_list[p].each_enqueue_func_ptr = ds707_pkti_reorig;
  }
  (void)rex_clr_sigs(dsi_get_ds_tcb(), DS_PKT_REORIG_SIG);

} /* ds707_pkti_reg_pri_wmk_reorig_cbacks() */

/*===========================================================================
FUNCTION      DS707_PKTI_REG_1X_RLP_SRVC

DESCRIPTION   Registers parameters with RLP.

DEPENDENCIES  The watermarks need to be setup before this function is called.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_reg_1x_rlp_srvc
(
ds707_tc_state_type   *tc_state_ptr
)
{

  ds707_flow_type *flow_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
  -------------------------------------------------------------------------*/
  flow_ptr = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);
  ASSERT( flow_ptr != NULL);

  /*-------------------------------------------------------------------------
    Register with both types of RLP.  This is okay, as MUX will only
    use one.
  -------------------------------------------------------------------------*/
  rlp_reg_srvc(  &flow_ptr->tx_wm_list[DSRSP_ROUTE_A],
                 tc_state_ptr->fwd_rlp.post_rx_func_ptr,
                 &tc_state_ptr->fwd_rlp.post_rx_wm_list[DSRSP_ROUTE_A],
                 (uint32) NULL );

  dsrlp_reg_srvc(DSRLP_FIXED_SR_ID_INDEX,
                 DSRLP_VER_1X_3,
                 &flow_ptr->tx_wm_list[DSRSP_ROUTE_A],
                 tc_state_ptr->fwd_rlp.post_rx_func_ptr,
                 &tc_state_ptr->fwd_rlp.post_rx_wm_list[DSRSP_ROUTE_A],
                 DSRSP_ROUTE_A,
                 (uint32)tc_state_ptr);


  rlp_reg_hdlc( tc_state_ptr->fwd_rlp.hdlc_inst_ptr);

} /* ds707_pkti_reg_1x_rlp_srvc() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_DATA_CALL_SUPPORT

DESCRIPTION    This function checks if the data call is supported based on the 
               mode preference

DEPENDENCIES   None.

RETURN VALUE   TRUE: data call is supported
               FALSE: data call is not supported

SIDE EFFECTS   None

===========================================================================*/
LOCAL boolean ds707_pkt_mgr_data_call_support
(
  cm_mode_pref_e_type mode_pref
)
{
  boolean supported_flag = FALSE;

  switch ( mode_pref )
  {
    /* the following mode proferences are supported to make a data call*/
    case CM_MODE_PREF_AUTOMATIC:
    case CM_MODE_PREF_CDMA_ONLY:
    case CM_MODE_PREF_CDMA_HDR_ONLY:
    case CM_MODE_PREF_HDR_ONLY:
    case CM_MODE_PREF_DIGITAL_ONLY:
    case CM_MODE_PREF_CDMA_WLAN:
    case CM_MODE_PREF_HDR_WLAN:
    case CM_MODE_PREF_CDMA_HDR_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY:
    case CM_MODE_PREF_ANY_BUT_HDR_WLAN:
    case CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY:
    case CM_MODE_PREF_HDR_LTE_ONLY:
    case CM_MODE_PREF_CDMA_HDR_LTE_ONLY:
    case CM_MODE_PREF_CDMA_HDR_GW:
    case CM_MODE_PREF_CDMA_GW:
    case CM_MODE_PREF_ANY_BUT_WLAN:
    case CM_MODE_PREF_CDMA_LTE_ONLY:
         supported_flag = TRUE;
         break;
    default:
         break;
  }
  return (supported_flag);
}


#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS707_PKTI_REG_HDR_RLP_SRVC

DESCRIPTION   Registers parameters with the specified Default HDR RLP

DEPENDENCIES  The watermarks need to be setup before this function is called.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_reg_hdr_rlp_srvc( void )
{
  ds707_tc_state_type   *pri_tc_state_ptr;
  ds707_flow_type       *pri_flow_ptr;
#ifdef FEATURE_HDR_QOS
  static dsm_watermark_type *last_registered_pri_tx_wm_list = NULL; 
  //Unbinding should happen when PPP disconnected
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pri_tc_state_ptr = ds707_get_pri_tc_ptr();
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
  -------------------------------------------------------------------------*/
  pri_flow_ptr = DS707_PRI_DS_FLOW_PTR(pri_tc_state_ptr);
  ASSERT( pri_flow_ptr != NULL);

  /*-------------------------------------------------------------------------
   IMPORTANT:
   Note that in this function we register the transmit watermark and the same
   HDLC deframer for BOTH Default Packet App and MultiFlow Packet App. This
   is not a valid configuration in case both Packet Apps are defined,
   because it will result in both RLPs incorrectly feeding the same HDLC
   deframer. Also on the transmit side, both DPA and MPA will be reading
   bytes from the same watermark, resulting in some packets being transmitted
   on DPA and some on MPA.
   
   Revisit: Move this function to when call orig instead of phys link up
   Revisit: Reg tx wm/HDLC for Default & MRLP. Invalid to have both assigned
  
   Change this if we want to support both DPA and MPA at the same time!
  -------------------------------------------------------------------------*/

  dsrlp_reg_srvc(DSRLP_HDR_SN_RLP_SR_ID_INDEX,
                 DSRLP_VER_HDR,
                 &pri_flow_ptr->tx_wm_list[DSRSP_ROUTE_A],
                 pri_tc_state_ptr->fwd_rlp.post_rx_func_ptr,
                 &pri_tc_state_ptr->fwd_rlp.post_rx_wm_list[DSRSP_ROUTE_A],
                 DSRSP_ROUTE_A,
                 (uint32)pri_tc_state_ptr);

#ifdef FEATURE_HDR_QOS
  /*-------------------------------------------------------------------------
    Register with the HDR RLP also. This is okay as only one RLP (1X or HDR)
    will be active at a time and receiving/sending data.
  -------------------------------------------------------------------------*/
  /* Register the default forward and reverse link MRLPs */
  dsrlp_reg_srvc(DSRLP_HDR_MRLP_FWD_BASE_INDEX,
                 DSRLP_VER_HDR_MULTIFLOW_FORWARD,
                 &pri_flow_ptr->tx_wm_list[DSRSP_ROUTE_A],
                 pri_tc_state_ptr->fwd_rlp.post_rx_func_ptr,
                 &pri_tc_state_ptr->fwd_rlp.post_rx_wm_list[DSRSP_ROUTE_A],
                 DSRSP_ROUTE_A,
                 (uint32)pri_tc_state_ptr);

  if ( last_registered_pri_tx_wm_list != NULL )
  {
    hdrmrlpsched_unbind_wm_rlp_ex( DSRLP_HDR_MRLP_REV_BASE_INDEX,
                                   last_registered_pri_tx_wm_list ); 
  }

  hdrmrlpsched_bind_wm_rlp_ex( DSRLP_HDR_MRLP_REV_BASE_INDEX,
                               pri_flow_ptr->tx_wm_list );

  last_registered_pri_tx_wm_list = pri_flow_ptr->tx_wm_list;
#endif /* FEATURE_HDR_QOS */

} /* ds707_pkti_reg_hdr_rlp_srvc() */
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION       DS707_SET_TERM_IN_PROGRESS

DESCRIPTION    This function gets called when termination starts or
               finishes, and sets the flag accordingly.

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_set_term_in_progress
(
  boolean val
)
{
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "ds707_set_term_in_progress: %d", val);

  rex_enter_crit_sect(&ds707_global_crit_sect);
  ds707_term_in_progress = val;
  rex_leave_crit_sect(&ds707_global_crit_sect);

} /* ds707_set_term_in_progress() */

/*===========================================================================
FUNCTION       DS707_IS_TERM_IN_PROGRESS

DESCRIPTION    This function returns if termination is in progress.

DEPENDENCIES   None

RETURN VALUE   TRUE : termination in progress 
               FALSE: not in progress
 
SIDE EFFECTS   None

===========================================================================*/
boolean ds707_is_term_in_progress
(
  void
)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED, "ds707_is_term_in_progress: %d",
                   ds707_term_in_progress);

  return ds707_term_in_progress;
} /* ds707_is_term_in_progress */

#ifdef FEATURE_DATA_FALLBACK_LIST
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_DATA_FALLBACK_LIST */

/*===========================================================================
                 FUNCTIONS FOR PPP/RLP INTERACTION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_UM_RLP_FWD_FUNC

DESCRIPTION   Called (via function pointer) by RLP when data is received on
              the forward link.  Used by both SN and AN RLP sessions.  This
              will signal PS task that there is forward link RLP/PPP data
              to process.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_um_rlp_fwd_func(uint32 unused)
{
  DATA_MDM_ARG_NOT_USED(unused);

  /*lint -save -e641 */
  PS_SET_SIGNAL(PS_PPP_UM_SN_RX_SIGNAL);
  /*lint -restore */
} /* ds707_pkti_um_rlp_fwd_func() */

/*===========================================================================
FUNCTION      DS707_PKTI_SET_RLP_DEFAULTS

DESCRIPTION   Sets up the pkt_state_info with the default RLP TX and RX
              settings.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_set_rlp_defaults
(
  void
)
{
  ds707_tc_state_type   *tc_state_ptr;

  ds707_flow_type *curr_flow_ptr;
  uint8  curr_iface_index = 0;
  ps_iface_type* curr_iface_ptr;
  dsm_watermark_type *ds707_wmk_um_SN_fwd_wm_list = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tc_state_ptr = ds707_get_pri_tc_ptr();

  ds707_wmk_um_SN_fwd_wm_list = ds707_get_wmk_um_SN_fwd_wm_list();
  tc_state_ptr->fwd_rlp.post_rx_wm_list   = ds707_wmk_um_SN_fwd_wm_list;
  tc_state_ptr->fwd_rlp.post_rx_func_ptr = ds707_pkti_um_rlp_fwd_func;

  /* For each default ps_flow on an iface we have a ds_flow,
     set the tx_wm_list on each of these default ds_flow's to the 
     pri watermark. 
  */   
  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index ++ )
  {        
    curr_flow_ptr = ds707_get_flow_pool_ptr(curr_iface_index);
    curr_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(
                                                       curr_iface_index);
    /* Make sure that the iface ptrs match */
    ASSERT ( curr_iface_ptr == curr_flow_ptr->ps_iface_ptr );
    /* Make sure that the tx_wm_list of a default ds flow is being set */
    if ( !PS_IFACE_IS_FLOW_DEFAULT( curr_iface_ptr, 
                                   curr_flow_ptr->ps_flow_ptr ) )
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Flow 0x%x is not default flow on iface 0x%x",
                       curr_flow_ptr->ps_flow_ptr, curr_iface_ptr);
      ASSERT(0);
      return;
    }
    curr_flow_ptr->tx_wm_list = ds707_wmk_get_pri_wm_list(); 
  }  
} /* ds707_pkti_set_rlp_defaults() */

/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_CTA_TIMER_EXPIRED

DESCRIPTION   Called when AT+CTA timer expires.  Two options - either
              go dormant or end the session completely.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_cta_timer_expired
(
  void
)
{
#ifdef FEATURE_DATA_IS707_ASYNC
  ds707_tc_state_type   *tc_state_ptr;
#endif /* FEATURE_DATA_IS707_ASYNC */
  ps_phys_link_type     *phys_link_ptr;
  int16                  err_num;
  int                    ret_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();

#ifdef FEATURE_DATA_IS707_ASYNC
  tc_state_ptr   = ds707_get_pri_tc_ptr();
  if (ds707_so_async_so_is_async(tc_state_ptr->so) == TRUE)
  {
    /*-----------------------------------------------------------------------
      QNC calls cannot go dormant.  Set CTA timer so that it never expires
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "CTA timer expired. QNC call cannot go dormant");
    ds707_cta_reset_timer( DS707_CTA_STOP_TIMER, 
                           tc_state_ptr->so);
    return;
  }
#endif /* FEATURE_DATA_IS707_ASYNC */

  /*-------------------------------------------------------------------------
    First makes sure that there is an active data session to work on.
  -------------------------------------------------------------------------*/
  if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CTA timer expired");
    ret_val = ps_phys_link_down_cmd( phys_link_ptr,
                                     &err_num,
                                     NULL );

    if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Cant tear down phys link - error %d",
                      err_num);
    }
  }
  /* If phy link is down, it is cleaning up the interface or CTA+timer expired */

} /* ds707_pkti_cta_timer_expired() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_BRING_DOWN_PHYS_LINK

DESCRIPTION   Bring down phys link (Currently used when in partial context)

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_bring_down_phys_link
(
  void
)
{
  ps_phys_link_type     *phys_link_ptr = NULL;
  int16                  err_num;
  int                    ret_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();

  /*-------------------------------------------------------------------------
    First makes sure that there is an active data session to work on.
  -------------------------------------------------------------------------*/
  if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP)
  {
    ret_val = ps_phys_link_down_cmd( phys_link_ptr,
                                     &err_num,
                                     NULL );

    if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Cant tear down phys link - error %d",
                      err_num);
    }
  }
} /* ds707_pkt_bring_down_phys_link() */

/*===========================================================================
FUNCTION      DS707_PKTI_REORIG_SIG_HDLR

DESCRIPTION   Called when the signal to re-originate is set.  Usually set
              when the RLP Tx watermark goes non-empty in a dormant mode.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_reorig_sig_hdlr(void)
{
  ps_phys_link_type          *pri_phys_link_ptr;
  ps_iface_type              *iface_ptr;
  ds3g_pkt_call_info_type   pkt_info;
  int16                      err_num;
  ds707_flow_type           *ds_flow_ptr_arr[DS707_NUM_FWD_PS_FLOWS + 
                                             DS707_NUM_REV_PS_FLOWS];
  int                        ret_val;
  int                        i, j;
  int                        p;
  uint8                      num_ds_flows = 0;
  dsm_item_type             *log_pkt_ptr = NULL; /* Packet to be logged */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Clear the signal */
  (void)rex_clr_sigs( dsi_get_ds_tcb(), 
                      DS_PKT_REORIG_SIG);

  /* Make sure PRI phys link is down */
  pri_phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  if (ds707_pkt_is_dormant(pri_phys_link_ptr) != TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "PRI phys link not dormant - Returning. State :%d", 
                     pri_phys_link_ptr->phys_private.state);

    /*
     *  Do not clear watermarks in reorigination. Clearing of watermarks 
     *  is part of tear down.
     */
    return;
  }

  for (i = 0; i < DS707_MAX_IFACES; i++)
  {
    /* Poll the default watermark to see if it has a packet for TX
     *   
     * Variable j is used later for reorigination or SDB / DoS. So make it 
     * zero to correspond to the index of ds_flow_ptr_arr that contains the 
     * flow that has the packet for TX
     */
    j = 0;
    /*-----------------------------------------------------------------------
      The Macro returns first of the multiple Default DS flows. Using it is 
      fine here as all the tx_wm_list of such DS flows points to the same 
      Watermark. 
    -----------------------------------------------------------------------*/
    ds_flow_ptr_arr[0]= DS707_PRI_DS_FLOW_PTR(ds707_get_pri_tc_ptr());    
    if (ds_flow_ptr_arr[0]->rev_tc_ptr != NULL &&            
        ds_flow_ptr_arr[0]->tx_wm_list != NULL)
    {
      if (dsrsp_wm_current_cnt(&ds_flow_ptr_arr[0]->rev_tc_ptr->rev_rlp.flow,
                               ds_flow_ptr_arr[0]->tx_wm_list) > 0)
      {
        /*lint -e40 variable 'phys_link_ptr' is assigned a value before use*/
        DATA_IS707_MSG2( MSG_LEGACY_MED, 
                         "PRI DS flow has packet to TX - physlink_ptr:%0x"
                         "TX WM list:%0x",
                         pri_phys_link_ptr, 
                         ds_flow_ptr_arr[0]->tx_wm_list);
        /*lint -restore*/
        break;
      }
    }

#ifdef FEATURE_HDR_QOS
    if (ds707_sec_pkt_is_qos_available_on_system() == FALSE)
    {
      continue;
    }

    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index((uint8)i);    
    if (!PS_IFACE_IS_VALID(iface_ptr))
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "iface_ptr is invalid. No data on to reoriginate");
      continue;
    }

    ds707_sec_pkt_get_all_active_secondary_flows(iface_ptr, 
                                                 ds_flow_ptr_arr,
                                                 &num_ds_flows);
    /* Poll all the watermarks and see which one has data */
    for (j = 0; j < num_ds_flows; ++j)
    {
      if (ds_flow_ptr_arr[j]->dirn == DS707_DIRECTION_FWD)
      {
        continue;
      }
      if (ds_flow_ptr_arr[j]->state.air_link_qos_state == 
                                           DS707_FLOW_AIR_LINK_QOS_GRANTED)
      {
        if (ds707_pkt_is_dormant(
                 ds_flow_ptr_arr[j]->rev_tc_ptr->ps_phys_link_ptr) == FALSE)
        {
          DATA_IS707_MSG1( MSG_LEGACY_MED, 
                           "Physlink not dormant. Not sending DoS - j:%d",j);
          break;
        }
        if (dsrsp_wm_current_cnt(
                          &ds_flow_ptr_arr[j]->rev_tc_ptr->rev_rlp.flow,
                          ds_flow_ptr_arr[j]->tx_wm_list) > 0)
        {
          DATA_IS707_MSG3( MSG_LEGACY_MED, 
                           "SEC DS flow has pkt to TX - physlink_ptr:%0x "
                           "TX WM list:%0x j:%d",
                           ds_flow_ptr_arr[j]->rev_tc_ptr->ps_phys_link_ptr, 
                           ds_flow_ptr_arr[j]->tx_wm_list, 
                           j);
          break;
        }
      }
    }  /* for (j = 0; j < num_ds_flows; ++j)  */

    if (j != num_ds_flows)
    {
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "Watermarks for Iface:0x%x contain data", 
                       iface_ptr);
      break;
    }
#endif /* FEATURE_HDR_QOS */    
  } /* for (i = 0; i < DS707_MAX_IFACES; i++) */

  if (i == DS707_MAX_IFACES)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No dormant call needing re-orig");
  }
  else
  {
#if defined(FEATURE_DATA_IS2000_SDB) || defined(FEATURE_HDR_DOS)
    if (
#ifdef FEATURE_HDR
       (ds707_sec_pkt_is_qos_available_on_system() == TRUE) || 
#endif /* FEATURE_HDR */
       (ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_CDMA))
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "SDB/DoS Packet processed = %d for tx for WM List:%0x",
                       ds707_sdb_process_txq(ds_flow_ptr_arr[j]),
                       ds_flow_ptr_arr[j]->tx_wm_list);

      if (ds707_sdb_process_txq(ds_flow_ptr_arr[j]) == TRUE)
      {
        /* SDB Packet processed for tx WM List */
        return;
      }
      else
      {
        /* SDB Packet NOT processed for tx WM List */
      }
    }
#endif /* FEATURE_DATA_IS2000_SDB || FEATURE_HDR_DOS */

    if (DS_FLOW_CTRL_IS_BIT_SET( ps_phys_link_get_flow_mask(
                                                       pri_phys_link_ptr),
                                 DS_HOLDDOWN_TIMER_MASK) == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "Cant reorig with holddown timer");
      for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
      {
        dsm_empty_queue(&ds_flow_ptr_arr[j]->tx_wm_list[p]);
      }
    }
    else if (DS_FLOW_CTRL_IS_BIT_SET( ps_phys_link_get_flow_mask(
                                                       pri_phys_link_ptr),
                                      DS_FLOW_NO_SERVICE_MASK)  == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "Cant reorig with No service MASK set");
      for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
      {
        dsm_empty_queue(&ds_flow_ptr_arr[j]->tx_wm_list[p]);
      }
    }
    else if (ds707_pkti_ok_orig_from_dorm(pri_phys_link_ptr,
                                          DS707_PKT_DORM_REASON_DATA) 
                                                                   == FALSE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "Cant reorig since phys link say not OK");
      for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
      {
        dsm_empty_queue(&ds_flow_ptr_arr[j]->tx_wm_list[p]);
      }
    }
    else if (ds707_so_retrydel_is_delayed() == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "Cant reorig since retry delay timer is running");
      for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
      {
        dsm_empty_queue(&ds_flow_ptr_arr[j]->tx_wm_list[p]);
      }
    }
#ifdef FEATURE_DATA_OPTHO
      /*------------------------------------------------------------ 
          Based on S101 SM state and iface handoff policy determine
          whether to allow/reject tunnel call bringup
      ------------------------------------------------------------*/
     else if ( ds707_s101_query_sm_state() ==
                  DS707_S101_SM_TUNNEL_CALL_DISALLOWED )
     {
       DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                       "S101 in Tunnel Disallowed State. Reject Tunnel Call");
       for ( p = 0; p < DSRSP_NUM_ROUTES; ++p )
       {
         dsm_empty_queue(&ds_flow_ptr_arr[j]->tx_wm_list[p]);
       }
     }
 #endif /* FEATURE_DATA_OPTHO*/
    else
    {
      /*-------------------------------------------------------------------
        Retrieve the packet without dequeuing it for logging
      -------------------------------------------------------------------*/
      for (p = 0; p < DSRSP_NUM_ROUTES; p++)
      {
        if ((log_pkt_ptr = (dsm_item_type *)q_check(
               ds_flow_ptr_arr[j]->tx_wm_list[p].q_ptr)) != NULL)
        {
          break;
        }
      }

      /*---------------------------------------------------------------------
        Log the packet to know which packet caused the phys link to move
        out of dormancy
      ---------------------------------------------------------------------*/
      if ((NULL != log_pkt_ptr) && 
          (0 != ps_wmk_logging_reorig_pkt(DS_SYS_TECH_3GPP2,
                                          PS_WMK_UPLINK_PKT,
                                          log_pkt_ptr)))
      {
        /*-------------------------------------------------------------------
          Ignore error & continue. No need to print message since it would be
          printed in the above function call 
        -------------------------------------------------------------------*/
      }

      pkt_info.drs_bit         = TRUE;
      pkt_info.use_hdown_timer = TRUE;
      /*-------------------------------------------------------------------
        Need to Populate the new_network to be the current data sess 
        network. Otherwise the Orig handler will always set the current
        data session network to zero which is NO_SRVC
      -------------------------------------------------------------------*/
      pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();
      /*-------------------------------------------------------------------
        For Non TE Based origination force_cdma_only should always be FALSE
      -------------------------------------------------------------------*/
      pkt_info.force_cdma_only = FALSE; 
      /*-------------------------------------------------------------------
        Set the inc_call flag to FALSE for all Origination attempts
      -------------------------------------------------------------------*/
      pkt_info.inc_call        = FALSE; 


      /*-------------------------------------------------------------------
        Set the epzid flag to FALSE for all Origination attempts
       -------------------------------------------------------------------*/
      pkt_info.epzid           = FALSE;

      /* Originating pri_phys_link as against the corresponding phys_link
         as they are all tied. DS code tries to tie all events to 
         pri_phys_link and do all transactions on it. Any resultant 
         phys_link indications are propagated to all secondary phys_links. 
         PS need not behave the same way (and probably cannot as 
         multi-phys_link is transparent to it)*/

      ret_val = ps_phys_link_up_cmd( pri_phys_link_ptr, 
                                     &err_num, 
                                     &pkt_info);
      if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "PS_IFACE reorig failed %d",
                        err_num);
        for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
        {
          dsm_empty_queue(&ds_flow_ptr_arr[j]->tx_wm_list[p]);
        }
      }
    }
  }
} /* ds707_pkti_reorig_sig_hdlr() */

/*===========================================================================
FUNCTION       DS707_PKT_REL_SEC_FLOWS_AND_POST_GO_NULL_IND

DESCRIPTION    Util function to release all secondary flows and post 
               go null indication on the primary flow for all ifaces. 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
STATIC void ds707_pkt_rel_sec_flows_and_post_go_null_ind
(
  void
)
{
  uint8                      curr_iface_index = 0;
  ps_iface_type             *iface_ptr;
  uint8                      num_ps_flows = 0;
  ps_flow_type              *ps_flow_ptr_arr[DS707_MAX_DS_FLOWS];
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_MED, 
                  "Release all the secondary flows");

  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES; 
        curr_iface_index++ )
  {      
    iface_ptr =  NULL;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    
    if(iface_ptr != NULL)
    {
    #ifdef FEATURE_HDR_QOS
      num_ps_flows = 0;
      
      if(ds707_sec_pkt_mgr_get_all_secondary_flows(iface_ptr,
                                                ps_flow_ptr_arr,
                                                DS707_MAX_DS_FLOWS,
                                                &num_ps_flows)==TRUE)
      {
        ds707_sec_pkt_mgr_release_qos_resources(
                                    ps_flow_ptr_arr,
                                    num_ps_flows,
                                    (uint8)DS707_SEC_PKT_MGR_AQOS_S_DQOS_S,
                                    DS707_SEC_PKT_FORCEFULLY_CLEAN_DS_FLOW,
                                    PS_EIC_NOT_SPECIFIED );
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Couldn't get sec flows");
      }
    #endif
    
      ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                        PS_EIC_NOT_SPECIFIED);
    }
  } /* For all ifaces */                    
} /* ds707_pkt_rel_sec_flows_and_post_go_null_ind */

#ifdef FEATURE_HDR_QOS
/*===========================================================================
FUNCTION       DS707_PKT_POST_SEC_PHYS_LINK_DOWN_IND

DESCRIPTION    Util function to post PS phys link down indication for 
               Secondary Phys links 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
STATIC void ds707_pkt_post_sec_phys_link_down_ind
(
  void
)
{
  ds707_tc_state_type       *tc_state_ptr = NULL;
  ps_phys_link_type         *sec_phys_link_ptr = NULL;
  int                        i;

  /*Giving down indication for secondary phys link*/
  if ((hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) ||
      (hdrcp_stream_stream_is_assigned(
                           HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)) ||
      (hdrcp_stream_stream_is_assigned(
                            HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
      )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Giving down indication for Secondary phys links");
    for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
    {
      tc_state_ptr = ds707_get_sec_tc_ptr(i);
      if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE)
      {
        continue;
      }
      sec_phys_link_ptr = tc_state_ptr->ps_phys_link_ptr;
      ASSERT (sec_phys_link_ptr != NULL);
      ps_phys_link_down_ind_ex(sec_phys_link_ptr, 
                               ds707_pkt_mgr_get_eic_end_reason()); 
    }
  }
} /* ds707_pkt_post_sec_phys_link_down_ind*/

/*===========================================================================
FUNCTION       DS707_PKT_CLEANUP_SEC_RX_WMKS_AND_POST_GONE_IND

DESCRIPTION    Util function to clean up secondary RX wmks and post 
               PS phys link gone indication for Secondary Phys links 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
STATIC void ds707_pkt_cleanup_sec_rx_wmks_and_post_gone_ind
(
  void
)
{
  ds707_tc_state_type       *tc_state_ptr = NULL;
  ps_phys_link_type         *sec_phys_link_ptr = NULL;
  int                        i, p;

  if ((hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) ||
      (hdrcp_stream_stream_is_assigned(
                            HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))||
      (hdrcp_stream_stream_is_assigned(
                            HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
     )
  {
    for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
    {
      tc_state_ptr = ds707_get_sec_tc_ptr(i);
      if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE)
      {
        continue;
      }
      if (tc_state_ptr->fwd_rlp.flow.ver == 
                                      DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        /* Cleanup the Secondary RX watermarks
         */
        for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
        {
          dsm_empty_queue(&tc_state_ptr->fwd_rlp.post_rx_wm_list[p]);
        }              
      }
    }
  }

  /* This seems to be a redundant run of the above 'for' loop. Can't we
     merge these two ?*/
  if ((hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) ||
      (hdrcp_stream_stream_is_assigned(
                            HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))||
      (hdrcp_stream_stream_is_assigned(
                            HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
     )
  {
    for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
    {
      tc_state_ptr = ds707_get_sec_tc_ptr(i);
      if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE)
      {
        continue;
      }
      sec_phys_link_ptr = tc_state_ptr->ps_phys_link_ptr;
      if (sec_phys_link_ptr != NULL)
      {
        /*-----------------------------------------------------------
         Reset bearer_id 
        -----------------------------------------------------------*/
        sec_phys_link_ptr->bearer_id = DSRLP_INVALID_HDR_MRLP_FLOW;
      }
      else
      {
        ASSERT (0);
      }
      ps_phys_link_gone_ind_ex(sec_phys_link_ptr,
                               ds707_pkt_mgr_get_eic_end_reason());
    }
  }
} /* ds707_pkt_cleanup_sec_rx_wmks_and_post_gone_ind*/

/*===========================================================================
FUNCTION       DS707_PKT_POST_SEC_PHYS_LINK_UP_IND

DESCRIPTION    Util function to post PS phys link up ind for secondary 
               Phys links 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
STATIC void ds707_pkt_post_sec_phys_link_up_ind
(
  void
)
{
  ds707_tc_state_type       *tc_state_ptr = NULL;
  ps_phys_link_type         *sec_phys_link_ptr = NULL;
  int                        i;
  dsrlp_rlp_identity_type    rlp;

  if ((hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) ||
      (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))||
      (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
     )
  {
    for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
    {
      tc_state_ptr = ds707_get_sec_tc_ptr(i);
      if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE)
      {
        continue;
      }

      DATA_IS707_MSG4(MSG_LEGACY_MED,
                    "Processing tc_state_ptr : direction for RLP fwd:%d rev:%d, "
                    "MRLP fwd %d MRLP rev %d",
                    tc_state_ptr->fwd_rlp.flow.ver, 
                    tc_state_ptr->rev_rlp.flow.ver,
                    tc_state_ptr->fwd_rlp.flow.flow,
                    tc_state_ptr->rev_rlp.flow.flow);

      if (tc_state_ptr->fwd_rlp.flow.ver == 
                                         DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        rlp = tc_state_ptr->fwd_rlp.flow;
      }
      else if (tc_state_ptr->rev_rlp.flow.ver == 
                                         DSRLP_VER_HDR_MULTIFLOW_REVERSE)
      {
        rlp = tc_state_ptr->rev_rlp.flow;
      }
      else
      {
        /* Invalid direction for RLP */
        continue;
      }

      if (hdrpacrlpctl_is_rlp_activated(rlp))
      {
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "Giving phys link up ind for RLP Id:%d", 
                         rlp.flow);
        sec_phys_link_ptr = tc_state_ptr->ps_phys_link_ptr;
        if (sec_phys_link_ptr != NULL)
        {
          /*--------------------------------------------------------------
           Populate bearer_id with rlp flow_id 
          --------------------------------------------------------------*/
          sec_phys_link_ptr->bearer_id = tc_state_ptr->fwd_rlp.flow.flow;
        }
        else
        {
          ASSERT(0);
          return;
        }
        ps_phys_link_up_ind(sec_phys_link_ptr);
      }
    }        
  }
} /* ds707_pkt_post_sec_phys_link_up_ind  */

/*===========================================================================
FUNCTION       DS707_PKT_POST_SEC_PHYS_LINK_GO_NULL_CMD

DESCRIPTION    Util function to post PS phys link go null cmd for secondary 
               Phys links. 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
STATIC void ds707_pkt_post_sec_phys_link_go_null_cmd
(
  void
)
{
  ds707_tc_state_type       *tc_state_ptr = NULL;
  ps_phys_link_type         *sec_phys_link_ptr = NULL;
  int                        i;
  int                        rval = -1;
  int16                      err_num = DS_EINVAL;

  if ((hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) ||
      (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))||
      (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ))
  )
  {
    for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
    {
      tc_state_ptr = ds707_get_sec_tc_ptr(i);

      if(tc_state_ptr == NULL)
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "TC state ptr is NULL");
        continue;
      }
      
      if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE)
      {
        continue;
      }
      
      sec_phys_link_ptr = tc_state_ptr->ps_phys_link_ptr;
      if(sec_phys_link_ptr == NULL)
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "sec_phys_link_ptr is NULL");
        continue;
      }
      
      rval = ps_phys_link_go_null_cmd(sec_phys_link_ptr,
                                      &err_num,
                                      NULL);
      DATA_IS707_MSG3( MSG_LEGACY_MED, "Called sec ps_phys_link_go_null_cmd "
                       "on phys_link : 0x%x: ret: %d, err code: %d",
                       sec_phys_link_ptr,
                       rval,
                       err_num);
      
      if( rval == 0 )
      {
        DATA_IS707_MSG1( MSG_LEGACY_MED,
                         "Calling sec ps_phys_link_gone_ind on phys_link : 0x%x",
                         sec_phys_link_ptr);
        ps_phys_link_gone_ind(sec_phys_link_ptr);                       
      }
    }
  }
} /* ds707_pkt_post_sec_phys_link_go_null_cmd */

#endif /* FEATURE_HDR_QOS */

/*===========================================================================
FUNCTION ds707_pkt_mgr_back_pressure_timer_exp_hdlr

DESCRIPTION
  This is the timer exp hdlr for dynamic back pressure disable. This would be 
  called in PS task context.

PARAMETERS
  user_data_ptr - user data.

DEPENDENCIES
  None.

RETURN VALUE
  None 

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_pkt_mgr_back_pressure_timer_exp_hdlr
(
  void                              *user_data_ptr
)
{
  ds707_back_pressure_params_type   *back_pressure_data_p;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Timer Dynamic BP exp hdlr");
  back_pressure_data_p = (ds707_back_pressure_params_type *)user_data_ptr;

  if (back_pressure_data_p == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Back pressure data ptr is NULL");
    return;
  }
  else
  {
    if ( back_pressure_data_p->tcp_rev_link_ack_counter == 
        back_pressure_data_p->last_tcp_rev_link_ack_count )
    {
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,"Curr_ack_cnt %d, last_ack_cnt: %d"
                                      "Enabling Back pressure",
                                       back_pressure_data_p->tcp_rev_link_ack_counter,
                                       back_pressure_data_p->last_tcp_rev_link_ack_count);

      /*----------------------------------------------------------------------
        Reset the Ack counters and enable back pressure
      ----------------------------------------------------------------------*/
      back_pressure_data_p->tcp_rev_link_ack_counter    = 0;
      back_pressure_data_p->last_tcp_rev_link_ack_count = 0;
      ds707_wmk_set_um_tx_high_wm_func(FALSE);
    }
    else
    {
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,"Curr_ack_cnt %d, last_ack_cnt: %d"
                          "restart BP timer",
                          back_pressure_data_p->tcp_rev_link_ack_counter,
                          back_pressure_data_p->last_tcp_rev_link_ack_count);

      /*----------------------------------------------------------------------
        Update the last_tcp_ack_count and start the timer
      ----------------------------------------------------------------------*/
      back_pressure_data_p->last_tcp_rev_link_ack_count = 
              back_pressure_data_p->tcp_rev_link_ack_counter;
      if (PS_TIMER_SUCCESS != 
          ps_timer_start(back_pressure_data_p->tmr_hndl_dyn_back_pressure, 
                        (back_pressure_data_p->data.timer_val)))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Failed to start timer");
      }
    }
  }
} /* ds707_timer_back_pressure_disable_exp_hdlr */

/*===========================================================================
FUNCTION 	ds707_pkt_mgr_disable_back_pressure

DESCRIPTION   	This function disables the back pressure.

PARAMETERS  	None
    
DEPENDENCIES    None.
  
RETURN VALUE    None.
  
SIDE EFFECTS    Restores the high WMK func ptr and high WMK values.
  
===========================================================================*/
static void ds707_pkt_mgr_disable_back_pressure
(
  ds707_back_pressure_params_type   *back_pressure_data_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0(MSG_LEGACY_HIGH," Disable back pressure");
  /*-------------------------------------------------------------------------
    Disable back pressure as rev links Acks are being sent
  -------------------------------------------------------------------------*/
  ds707_wmk_set_um_tx_high_wm_func(TRUE);
  /*-------------------------------------------------------------------------
    Update the last_tcp_ack_count and start the timer
  -------------------------------------------------------------------------*/
  back_pressure_data_p->last_tcp_rev_link_ack_count = 
              back_pressure_data_p->tcp_rev_link_ack_counter;

  if(PS_TIMER_SUCCESS != 
     ps_timer_start(back_pressure_data_p->tmr_hndl_dyn_back_pressure, 
                    ( back_pressure_data_p->data.timer_val)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Failed to start timer ");
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "Started BP timer for %d  milliseconds, BP disabled", 
                    back_pressure_data_p->data.timer_val);
  }
} /* ds707_pkt_mgr_disable_back_pressure */

/*===========================================================================
FUNCTION ds707_pkt_mgr_read_bp_enabled_nv

DESCRIPTION
   This function Initializes the back pressure related params.

PARAMETERS  

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds707_pkt_mgr_read_bp_enabled_nv
(
  void
)
{
  ds707_back_pressure_data_s     data_bp;
  char      ds707_back_pressure_nv_path[] = 
                   "/nv/item_files/data/3gpp2/3gpp2_back_pressure_removal";
/*-------------------------------------------------------------------------*/

   memset(&data_bp,0,sizeof(ds707_back_pressure_data_s));
  /*------------------------------------------------------------------------
   Set the global variable ds707_back_pressure_params
  -------------------------------------------------------------------------*/
  if(MCFG_FS_STATUS_OK == mcfg_fs_read(ds707_back_pressure_nv_path,
                  &data_bp,
                 sizeof(ds707_back_pressure_data_s),
                 MCFG_FS_TYPE_EFS,
                 (mcfg_fs_sub_id_e_type)ds707_curr_subs_id()))
  {
    ds707_back_pressure_params.data.enabled   = data_bp.enabled;
    ds707_back_pressure_params.data.timer_val = data_bp.timer_val;
  }
  else
  {
  /* by default it is disabled */
    ds707_back_pressure_params.data.enabled   = 0;  
    ds707_back_pressure_params.data.timer_val = 0;     
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH,"EFS items read are: "
                  "timer_val: %d in millisecs, back_pressure_removal: %d",
                  ds707_back_pressure_params.data.timer_val,
                  ds707_back_pressure_params.data.enabled);

  /*---------------------------------------------------------------------
    Allocate timer handle for dynamic back pressure disable
  ---------------------------------------------------------------------*/
  ds707_back_pressure_params.tmr_hndl_dyn_back_pressure
        = ps_timer_alloc(ds707_pkt_mgr_back_pressure_timer_exp_hdlr,
                         (void *)&ds707_back_pressure_params);

  if ( PS_TIMER_INVALID_HANDLE == 
           ds707_back_pressure_params.tmr_hndl_dyn_back_pressure )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid PS timer handle ");
  }

  ds707_back_pressure_params.tcp_rev_link_ack_counter = 
            ds707_back_pressure_params.last_tcp_rev_link_ack_count = 0;

}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_RESET_BACK_PRESSURE_PARAMS

DESCRIPTION   Called to reset back pressure related params.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
static void ds707_pkt_mgr_reset_back_pressure_params
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  ps_timer_cancel(ds707_back_pressure_params.tmr_hndl_dyn_back_pressure);
  ds707_back_pressure_params.tcp_rev_link_ack_counter = 0;
  ds707_back_pressure_params.last_tcp_rev_link_ack_count = 0;
  ds707_wmk_set_um_tx_high_wm_func(FALSE);
}

/*===========================================================================
FUNCTION      DS707_PKTI_DORMANCY_TRANSFER_EVENT_REPORT

DESCRIPTION   Called during dormancy transfer to report wmk level and 
              PDN level info to framework 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkti_dormancy_transfer_event_report
(
  ps_phys_link_type                  *phys_link_ptr,
  ps_iface_type                      *iface_ptr,
  ps_wmk_logging_dorm_state_type      dormancy_state
)
{
  uint32 rx_wmk_cnt = 0; //current rx watermark count
  uint32 tx_wmk_cnt = 0; //current tx watermark count
  uint8  curr_iface_index = 0;
  int    rval = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------- 
    Sanity check
   -------------------------------------------------*/
  if( phys_link_ptr == NULL)
  {
     DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                      "Failure to indicate WMK LEVELS to PS: "
                      "Invalid physlink pointer 0x%x", phys_link_ptr);

     /*------------------------------------------------------------ 
      Clean up the reorig iface in case it's later needed
      ------------------------------------------------------------*/
     reorig_iface_ptr = NULL;
     return;
  }

  /*----------------------------------------------------------------------- 
    iface level info may not always be valid from MH for going into
    dormancy and network triggered re-orig. Pick one of the valid iface
    for framework to figure out the needed info.
   ----------------------------------------------------------------------*/
  if ( iface_ptr == NULL )
  {
    for ( curr_iface_index = 0; curr_iface_index < DS707_MAX_IFACES; 
          curr_iface_index++ )
    {      
      iface_ptr = 
        ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

      if ( DS707_PKT_MGR_IS_PS_IFACE_ACTIVE(iface_ptr) )
      {
        break;
      }
    }/* for each iface */ 

    if ( curr_iface_index == DS707_MAX_IFACES )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Failure to indicate WMK LEVELS to PS: "
                       "Fail to find valid iface info");
      /*------------------------------------------------------------ 
       Clean up the reorig iface in case it's later needed
       ------------------------------------------------------------*/
      reorig_iface_ptr = NULL;
      return;
    }
  }

  rx_wmk_cnt = ds707_get_current_rx_wmk_cnt();
  tx_wmk_cnt = ds707_get_current_tx_wmk_cnt();

  rval = ps_wmk_logging_event_report_ex( phys_link_ptr,
                                         EVENT_DS_UM_QUEUE_STATS_EX,
                                         dormancy_state,
                                         rx_wmk_cnt,
                                         tx_wmk_cnt,
                                         iface_ptr );

  DATA_IS707_MSG6( MSG_LEGACY_HIGH, 
                   "Dormancy transfer ind to PS on phys_link: 0x%x, "
                   "iface:0x%x, dormancy status:%d, "
                   "Tx WM count:%d, Rx WM count:%d, ret_val:%d",
                   phys_link_ptr, iface_ptr, dormancy_state,
                   tx_wmk_cnt, rx_wmk_cnt, rval );

  /*------------------------------------------------------------ 
   Clean up the reorig iface in case it's later needed
   ------------------------------------------------------------*/
  reorig_iface_ptr = NULL;

  return;
}

/*===========================================================================
FUNCTION       DS707_PKTI_EVENT_CBACK

DESCRIPTION    Callback when various phone events occur.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_pkti_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
  ds707_tc_state_type       *tc_state_ptr;
  ps_phys_link_type         *phys_link_ptr;
  ps_iface_type             *iface_ptr;         /* ptr to IPV4 1x iface  */
  ds3g_pkt_call_info_type   pkt_info;
  int16                      err_num = DS_EINVAL;
  int                        p;
  boolean                    enable_flow_flag;    /*ps phys link enable flag*/
  uint8                      curr_iface_index = 0;
  ds707_iface_table_type    *ds707_iface_tbl_ptr = NULL;
  int                       rval = -1;
  uint32                    ds707_nest_level = 0;
#ifdef FEATURE_EHRPD
  boolean                   handoff_notify_to_devmgr = TRUE;
#endif /*FEATURE_EHRPD*/
  ds707_devmgr_ppp_failure_counts_type *ppp_failure_counts_ptr = NULL;
  mc_queue_cmd_status_type     queue_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  VERIFY_PKT_INSTANCE(event_info_ptr->pkt_inst);

  tc_state_ptr   = ds707_get_pri_tc_ptr();
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();


  enable_flow_flag = TRUE;
  ds707_nest_level = ds707_cback_get_nest_level();

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "%d ds707_pkti_event_cback: event %d", 
                  ds707_nest_level, event);

  switch (event)
  {
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      if (event_info_ptr->going_dormant == TRUE)
      {
        
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link DOWN dorm - "
                      "cm pkt state dorm, reg reorig wmks, phys link down ind",
                      ds707_nest_level);

        ds3g_msh_set_pkt_state(CM_PACKET_STATE_DORMANT);

        /*-------------------------------------------------------------- 
        Deregister for DSM memory events.
      --------------------------------------------------------------*/
        if( ps_pkt_info_get_tcp_ack_prio_enabled() &&
            ds707_back_pressure_params.data.enabled )
        {
          ds707_reg_unreg_dsm_mem_event(FALSE);
        }

        /*-------------------------------------------------------------- 
          provide PS framework with um wmk info when going dormant
         --------------------------------------------------------------*/
        ds707_pkti_dormancy_transfer_event_report( phys_link_ptr,
                                                   NULL,
                                                   PS_WMK_LOGGING_DORMANT );

        ds707_pkti_reg_pri_wmk_reorig_cbacks(tc_state_ptr);
        /*-------------------------------------------------------------------
          Empty the watermarks to discard any data that was sitting before
          reorig callback was registered.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Clearing wmks after reorig callback registration");
 
        ds707_pkti_cleanup_wmks();
 
        /* if PHYS_LINK_DOWN_EV is caused by orig failure,we need to disable 
           flow to provent apps keeping orig data calls */
        if (event_info_ptr->call_end_info.end_status == 
                                      CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS)
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CM_CALL_END_CCS_NOT_SUPPORTED"
                          "flow is disabled");
          ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK);
          enable_flow_flag = FALSE;
        }
        if ( (event_info_ptr->failed_orig == TRUE) && 
             ( (event_info_ptr->call_cmd_err == CM_CALL_CMD_ERR_IN_USE_S) ||
               (event_info_ptr->call_cmd_err == 
                                        CM_CALL_CMD_ERR_EMERGENCY_MODE_S)
             )
           )
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "call orig fail, phone IN_USE, "
                          "flow is disabled");
          ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK);
          enable_flow_flag = FALSE;
        }
        if (enable_flow_flag == TRUE)
        {
          /*---------------------------------------------------------------
            Enable the phys link flow
          ---------------------------------------------------------------*/
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Enabling phys link flow after reorig callback"
                           "registration");
          ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);
        }

#ifdef FEATURE_HDR_QOS
        /* Registering the reorig function for all the secondary watermarks
         */
        ds707_sec_pkt_reg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();     

#endif /* FEATURE_HDR_QOS */        

        /*-----------------------------------------------------------
         Reset bearer_id 
        -----------------------------------------------------------*/
        phys_link_ptr->bearer_id = DSRLP_INVALID_HDR_MRLP_FLOW;

        /*Giving down indication for Primary phys link*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Giving down indication for Primary phys link");
        ps_phys_link_down_ind_ex(phys_link_ptr, 
                                 ds707_pkt_mgr_get_eic_end_reason());

        DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_INACTIVE_IND");
        ds3gflowmgr_notify_ps_call_state_per_sub(SYS_MODEM_AS_ID_1,
                                                 DS3GFLOWMGR_PS_CALL_DORMANT_EV);

#ifdef FEATURE_HDR_QOS
        /*Post down indication for Secondary phys link*/
        ds707_pkt_post_sec_phys_link_down_ind();
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR
        ds707_pkt_idm_reorig_after_phys_link_down();
#endif /* FEATURE_HDR */

        /* Cleanup all watermarks
         */
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Flushing all watermarks when going dormant");
        ds707_pkti_cleanup_wmks();
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, " %d.  phy link DOWN null - "
            "cm pkt state NULL, dereg reorig wmks, cleanup wmks, "
            "reset PPP flags, curr net/last net = NO_SRV, phys link gone ind",
            ds707_nest_level);

        if( ps_pkt_info_get_tcp_ack_prio_enabled() && 
            ds707_back_pressure_params.data.enabled )
        {
          ds707_pkt_mgr_reset_back_pressure_params();
          ds707_reg_unreg_dsm_mem_event(FALSE);
        }

#ifdef FEATURE_EHRPD
        /*-----------------------------------------------------------------------
          if phys link state is COMING_UP or RESUMING then this is a failure
          during call origination. At this point silent redial for ehrpd needs
          to be processed.
          We don't want to perform any action regarding eHRPD silent redial if
          call ends for an already up call (traffic channel was already assigned)
        -----------------------------------------------------------------------*/
        if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP ||
            PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING)
        {
          ds707_sys_chg_sr_process_call_end();
        }
#endif
        /*-----------------------------------------------------------
          Stop the RETRY DELAY timer and also enable to phys link 
          flow when the packet data session is aborted
          -----------------------------------------------------------*/
        ds3gi_stop_timer(DS3G_TIMER_RETRY_DELAY_ORIG);
        ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

        ds3g_msh_set_pkt_state(CM_PACKET_STATE_NULL);
        /*------------------------------------------------------------- 
          As a part of processing SUB_NOT_AVAIL/PWROFF/LPM mode notification
          from CM, DS will tear down all ifaces. This function is to notify 
          CM when the data session is torn down
        -------------------------------------------------------------*/
        /*--------------------------------------------------------------------- 
          In case of CARD SUB_NOT_AVAIL/LPM/PWR OFF mode, Notify DSMGR which in
          turn will notify CM after all the other internal DS modules are done
          processing
        ---------------------------------------------------------------------*/
        if (ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress() == TRUE)
        {
          ds3geventmgr_clr_and_notify_lpm_pwr_off_status(
            DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK);
        }
        else if (ds3geventmgr_is_sub_not_avail_processing_in_progress(
                                                   SYS_MODEM_AS_ID_1) == TRUE)
        {
          ds3geventmgr_clr_and_notify_sub_not_avail_status(
            DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK,
            SYS_MODEM_AS_ID_1,
            ds3gi_sub_not_avail_subs_id_mask);
        }

        if((0 != ds707_max_fb_pdn_failure_count) 
           && (ds707_fb_pdn_failure_count >= ds707_max_fb_pdn_failure_count))
        {
          ds707_set_ppp_failure_flag(TRUE);
          /*----------------------------------------- 
          Inform HDR to switch personality to HRPD.
          -----------------------------------------*/
          hdrpac_set_data_ehrpd_capability(FALSE);
          ds707_pkt_mgr_clear_pdn_fb_count();
        }

        ppp_failure_counts_ptr = ds707_devmgr_get_ppp_failure_counts();

        DATA_IS707_MSG1(MSG_LEGACY_HIGH, " ppp_failure_counts_ptr->hard_failure_count - %d ",
                        ppp_failure_counts_ptr->hard_failure_count);
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, " ds707_get_max_ppp_hard_failures() - %d ",
                        ds707_get_max_ppp_hard_failures());
        if( ds707_get_max_ppp_hard_failures() > 0 && 
            ( ppp_failure_counts_ptr->hard_failure_count == 
               ds707_get_max_ppp_hard_failures() ) ) 
        {
          ds707_set_ppp_failure_flag(TRUE);
          /*----------------------------------------- 
          Inform HDR to switch personality to HRPD.
          -----------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, " Inform HDR to switch personality to HRPD ");
          hdrpac_set_data_ehrpd_capability(FALSE);
          ppp_failure_counts_ptr->hard_failure_count = 0;
          ppp_failure_counts_ptr->soft_failure_count = 0;
          ppp_failure_counts_ptr->max_ppp_failure_reached = TRUE;
        }

        ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);
        ds707_extif_inform_hdr_sess_status();

#ifdef FEATURE_HDR_QOS
        /* De-register all the watermarks */
        ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();
#endif /* FEATURE_HDR_QOS */

        /*-----------------------------------------------------------
          There might be routing take place in the middle of below
          cleanup. In such cases we need to block new originations.
        -----------------------------------------------------------*/
        ds707_set_term_in_progress(TRUE);
        
        /* This may need to be Revisited since we are trying to enter
           PS critical section from DS task context */
        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

        /*-----------------------------------------------------------
          Release all secondary flows and post 
          go null indication on the primary flow for all ifaces
        -----------------------------------------------------------*/
        ds707_pkt_rel_sec_flows_and_post_go_null_ind();                         

#ifdef FEATURE_EHRPD
        /*-----------------------------------------------------------
          Abort device manager
        -----------------------------------------------------------*/
        if (0 != ds707_devmgr_abort())
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_abort() failed");
        }
#endif /* FEATURE_EHRPD */

        ps_phys_link_gone_ind_ex(phys_link_ptr,
                                 ds707_pkt_mgr_get_eic_end_reason());

        ds3gflowmgr_notify_ps_call_state_per_sub ( SYS_MODEM_AS_ID_1, 
                                                   DS3GFLOWMGR_PS_CALL_END_EV );         

        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

       /*------------------------------------------------------------- 
          Clear termination in progress flag.
        -------------------------------------------------------------*/
        ds707_set_term_in_progress(FALSE);

        ds707_drs_reset_ppp_resync_flags();
        ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_NO_SRV);
        ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_NO_SRV);
        ds707_pkt_set_prev_call_so((uint16) CAI_SO_NULL);

        /* Cleanup all watermarks
        */
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Flushing all watermarks when going NULL");
        ds707_pkti_cleanup_wmks();


        tc_state_ptr   = ds707_get_pri_tc_ptr();

        /* Cleanup the Primary RX watermark
         */
        for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
        {
          dsm_empty_queue(&tc_state_ptr->fwd_rlp.post_rx_wm_list[p]);
        }

#ifdef FEATURE_HDR_QOS        
        /* Cleanup the Sec RX watermark           */
        ds707_pkt_cleanup_sec_rx_wmks_and_post_gone_ind();
#endif /* FEATURE_HDR_QOS */
      }
#if (defined (FEATURE_HDR) && defined (FEATURE_HDRRLP_LOGGING) && !defined(TEST_FRAMEWORK))
      hdrrlplog_stop_log_pkt();
#endif /* FEATURE_HDR && !TEST_FRAMEWORK */

      break;

    case DS707_CBACK_PHYS_LINK_UP_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link UP - "
                      "cm pkt state connect, tell dssnet4 if qnc, "
                      "pzid hyst active event, phys link up ind", 
                      ds707_nest_level);

#ifdef FEATURE_EHRPD
      /*------------------------------------------------------------------------ 
        Process call connect for silent redial on ehrpd
      ------------------------------------------------------------------------*/
      ds707_sys_chg_sr_process_call_connect();
#endif
      ds3g_msh_set_pkt_state(CM_PACKET_STATE_CONNECT);

      for ( curr_iface_index = 0; 
            curr_iface_index < DS707_MAX_IFACES; 
            curr_iface_index++ )
      {      
        iface_ptr = 
          ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
        ds707_iface_tbl_ptr = ds707_get_iface_tbl_from_instance(curr_iface_index);

        if ( ps_iface_addr_family_is_v4(iface_ptr) &&
             PS_IFACE_IS_IN_USE(iface_ptr) )
        {
          dssnet4_set_in_qnc_call(
            ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr),
#ifdef FEATURE_DATA_IS707_ASYNC            
            ds707_so_async_so_is_async(event_info_ptr->so)
#else
            0
#endif /* FEATURE_DATA_IS707_ASYNC */
          );
                                 
        }
      } /* for each iface */ 
      /* enable flow incase flow was disabled when orig failed */
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "recv phys_link_up ev, "
                      "flow is enabled");
      ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

#ifdef FEATURE_CALL_THROTTLE
      /*---------------------------------------------------------------------
        If the call is coming out of dormancy we  post call_success to DCTM
        as PPP is already UP hence the call is successful.  For the first 
        call we wait till PPP comes up before we report SUCCESS to DCTM 
        module. Without this fix if the previous re-orig failed at system 
        level for some reason, the call remains throttled.
      ---------------------------------------------------------------------*/
      for ( curr_iface_index = 0; 
            curr_iface_index < DS707_MAX_IFACES; 
            curr_iface_index++ )
      {      
        iface_ptr = NULL ;
        iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

        if ( PS_IFACE_IS_VALID ( iface_ptr ) )
        {
            /*---------------------------------------------------------------
              Upon Call origination check if the returning from a DORMANT 
              call IFACE_UP is for sockets calls and IFACE_ROUTEABLE is for
              relay and network model calls
            ---------------------------------------------------------------*/
            if ( (ds707_pkt_is_um_iface_up(iface_ptr) == TRUE) ||
                 (ds707_rmsm_is_rm_iface_up() == TRUE) )
            {
#ifdef FEATURE_DS_MOBILE_IP
              if (ds707_drs_is_in_mip_call() == TRUE)
              {
                dctm_post_mip_call_status( MIP_SUCCESS_EV,
                                           MIP_FAIL_REASON_NONE);
              }
              else
#endif /* FEATURE_DS_MOBILE_IP */
              {
                dctm_post_ppp_call_status( PPP_SUCCESS_EV,
                                           DSDCTM_PPP_PROTOCOL_IPCP,
                                           PPP_FAIL_REASON_NONE);
              }

              /* For 1x2GTA */
              DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                              "Posting MC_TUNE_AWAY_CNTL_F - Ready for TuneAway");
              queue_status = mc_queue_cmd(MC_TUNE_AWAY_CNTL_F); 
              if (queue_status == MC_FREE_QUEUE_EMPTY) 
              {
                ERR_FATAL( "No MC command links available", 0, 0, 0);
              }
              else if (queue_status != MC_CMD_QUEUED) 
              {
                ERR_FATAL( "Unknown status return from mc_queue_cmd()", 0, 0, 0);
              }
              
              /* Fall out of the loop */
              break;
            }          
        } /* is valid iface */
      
      } /* for each iface */
#endif /* FEATURE_CALL_THROTTLE */

      /*---------------------------------------------------------------
       Populate bearer_id with rlp flow_id 
      ---------------------------------------------------------------*/
      phys_link_ptr->bearer_id = 
        tc_state_ptr->fwd_rlp.flow.flow;

      ps_phys_link_up_ind(phys_link_ptr);

      DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_ACTIVE_IND");
      ds3gflowmgr_notify_ps_call_state_per_sub(SYS_MODEM_AS_ID_1,
                                               DS3GFLOWMGR_PS_CALL_ACTIVE_EV);

#ifdef FEATURE_HDR_QOS
      ds707_pkt_post_sec_phys_link_up_ind();
#endif /* FEATURE_HDR_QOS */

#if (defined(FEATURE_HDR) && !defined(TEST_FRAMEWORK) && defined (FEATURE_HDRRLP_LOGGING) )
      hdrrlplog_start_log_pkt();
#endif /* FEATURE_HDR && !TEST_FRAMEWORK */
      break;

case DS707_CBACK_PHYS_LINK_RESUMING_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link ORIG - "
                      "dereg reorig wmks, reg RLP", ds707_nest_level);

      ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
      /* De-register all the watermarks */
      ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();   
#endif /* FEATURE_HDR_QOS */

      /*------------------------------------------------------------------ 
        provide PS framework with um wmk info when coming out of dormancy
       ------------------------------------------------------------------*/
      ds707_pkti_dormancy_transfer_event_report( phys_link_ptr,
                                                 reorig_iface_ptr,
                                                 PS_WMK_LOGGING_REORIG_FROM_DORMANCY );

      break;

#ifdef FEATURE_DATA_OPTHO
    case DS707_CBACK_TUN_PHYS_LINK_COMING_UP_EV:
    case DS707_CBACK_TUN_PHYS_LINK_RESUMING_EV:
      /* Fallthrough */
#endif /* FEATURE_DATA_OPTHO */
    
    case DS707_CBACK_PHYS_LINK_COMING_UP_EV:
      //lint -fallthrough
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link ORIG - "
                      "dereg reorig wmks, reg RLP", ds707_nest_level);

      ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
      /* De-register all the watermarks */
      ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();   
#endif /* FEATURE_HDR_QOS */
      break;

    case DS707_CBACK_INC_CALL_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link INC - "
                      "dereg reorig wmks, reg RLP", ds707_nest_level);

      ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
      /* De-register all the watermarks */
      ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();
#endif
      /*-------------------------------------------------------------------
         Set the inc_call flag to TRUE for all Mobile terminated calls
         The ps_phys_link_up_cmd just transitions the PHYS_LINK state to 
         RESUMING. It does not do any other mode handler processing
      -------------------------------------------------------------------*/
      pkt_info.inc_call        = TRUE; 

      /*-------------------------------------------------------------------
        Set the epzid flag to FALSE for all Origination attempts
       -------------------------------------------------------------------*/
      pkt_info.epzid           = FALSE;

      /*lint -e534 return value not required*/
      (void)ps_phys_link_up_cmd( phys_link_ptr, 
                                 &err_num, 
                                 &pkt_info);

      /*------------------------------------------------------------------ 
        provide PS framework with um wmk info when coming out of dormancy
       ------------------------------------------------------------------*/
      ds707_pkti_dormancy_transfer_event_report( phys_link_ptr,
                                                 NULL,
                                                 PS_WMK_LOGGING_REORIG_FROM_DORMANCY );

      /*lint -restore*/

      break;

    case DS707_CBACK_ABORT_SESSION_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link ABORT - "
                      "cm pkt state NULL, dereg reorig wmks, cleanup wmks " 
                      "reset PPP flags, curr net = NO_SRV, phys link gone ind",
                      ds707_nest_level);

      /*-----------------------------------------------------------
        Stop the RETRY DELAY timer and also enable to phys link 
        flow when the packet data session is aborted
      -----------------------------------------------------------*/
      ds3gi_stop_timer(DS3G_TIMER_RETRY_DELAY_ORIG);
      ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

      ds3g_msh_set_pkt_state(CM_PACKET_STATE_NULL);

      ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
      /* De-register all the watermarks */
      ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();
#endif /* FEATURE_HDR_QOS */


      ds707_drs_reset_ppp_resync_flags();
      ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_NO_SRV);
      /*-----------------------------------------------------------
        Set the last_data_sess_network to NO_SRV when the pkt 
        call goes NULL upon receiving an ABORT_SESSION event
      -----------------------------------------------------------*/
      ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_NO_SRV);
      ds707_pkt_set_prev_call_so((uint16) CAI_SO_NULL);

      ds707_extif_inform_hdr_sess_status();

      /*-----------------------------------------------------------
        There might be routing take place in the middle of below
        cleanup. In such cases we need to block new originations.
      -----------------------------------------------------------*/
      ds707_set_term_in_progress(TRUE);

      /*-----------------------------------------------------------
        Release all secondary flows and post 
        go null indication on the primary flow for all ifaces
      -----------------------------------------------------------*/
      ds707_pkt_rel_sec_flows_and_post_go_null_ind();                   

      /*-----------------------------------------------------------
        Set call end reason for each valid iface
      -----------------------------------------------------------*/
      if(
          (
            (event_info_ptr->tear_down_info.validity_mask)
            & 
            (PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK)
           ) 
          == PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK
        ) 
      {
        ds707_pkt_mgr_set_end_reason_for_all_ifaces(
               event_info_ptr->tear_down_info.data_info.call_end_reason);
      }
      else
      {
      ds707_pkt_mgr_set_end_reason_for_all_ifaces(
               PS_NET_DOWN_REASON_NOT_SPECIFIED);
      } 
#ifdef FEATURE_EHRPD
      /*-----------------------------------------------------------
        Abort device manager
      -----------------------------------------------------------*/
      if (0 != ds707_devmgr_abort())
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_abort() failed");
      }
#endif /* FEATURE_EHRPD */

      if(phys_link_ptr != NULL)
      {
        rval = ps_phys_link_go_null_cmd(phys_link_ptr,
                                        &err_num,
                                        NULL);
        DATA_IS707_MSG3( MSG_LEGACY_MED,
                         "Called pri ps_phys_link_go_null_cmd on phys_link : 0x%x: "
                         "ret: %d, err code: %d",
                         phys_link_ptr, rval, err_num);
        
        if( rval == 0 )
        {
          DATA_IS707_MSG1( MSG_LEGACY_MED,
                         "Calling pri ps_phys_link_gone_ind on phys_link : 0x%x",
                         phys_link_ptr);
          ps_phys_link_gone_ind(phys_link_ptr);
          
          DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_END_IND");
          ds3gflowmgr_notify_ps_call_state_per_sub(
             SYS_MODEM_AS_ID_1, DS3GFLOWMGR_PS_CALL_END_EV);
        }
  
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "Pri phys link is NULL");
      }

#ifdef FEATURE_HDR_QOS
      /* Post secondary phys link go null cmd */
      ds707_pkt_post_sec_phys_link_go_null_cmd();
#endif /* FEATURE_HDR_QOS */

      /*------------------------------------------------------------- 
        Clear termination in progress flag.
      -------------------------------------------------------------*/
      ds707_set_term_in_progress(FALSE);
      break;

    case DS707_CBACK_HOLDDOWN_TIMER_STARTED_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  hdown timer start - "
                      "disable flow", ds707_nest_level);

      ds707_phys_link_disable_flow(DS_HOLDDOWN_TIMER_MASK);
      break;

    case DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  hdown timer stop - "
                      "enable flow", ds707_nest_level);

      ds707_phys_link_enable_flow(DS_HOLDDOWN_TIMER_MASK);
      break;

#ifdef FEATURE_EHRPD
    case DS707_CBACK_HANDOFF_NONEPC_TO_EPC:
      /*--------------------------------------------------------------------- 
        No need to notify device manager if handoff is from NONEPC to EPC
      ---------------------------------------------------------------------*/
      handoff_notify_to_devmgr = FALSE;

      /*--------------------------------------------------------------------- 
        Notify Sys Chg Sm so it can transition its state to nonepc_to_epc
      ---------------------------------------------------------------------*/
      ds707_sys_chg_nonepc_to_epc_ind();

      /*--------------------------------------------------------------------- 
        Immaterial of whether special cleanup is performed or not, go 
        ahead and start tearing down the ifaces. This will cause the 
        DSSNET SMs to go to closed state. If special cleanup is performed,
        DSSNET will not issues phys link go null cmd and will wait for phy
        link gone ind after special cleanup is completed to post iface down
        ind. Else, DSSNET will issue phys link go null cmd and will post iface
        down ind after it receives phys link gone ind
      ---------------------------------------------------------------------*/
      if (ds707_sys_chg_is_nonepc_sp_call_cleanup_needed
                               (DS707_SYS_CHG_NONEPC_TO_EPC, NULL))
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "Special cleanup needed on nonepc_to_epc");
      }
      /*--------------------------------------------------------------------- 
        Fall through
      ---------------------------------------------------------------------*/
    case DS707_CBACK_HANDOFF_EPC_TO_NONEPC:
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, " HANDOFF  EVENT %d", event);

        /*--------------------------------------------------------------------- 
          Notify Sys Chg Sm so it can transition its state to epc_to_nonepc.
          If chk needed since the NONEPC_TO_EPC case statement falls through
        ---------------------------------------------------------------------*/
        if (event == DS707_CBACK_HANDOFF_EPC_TO_NONEPC)
        {
#ifdef FEATURE_DATA_WLAN_MAPCON
          if( DS707_SYS_CHG_SM_NULL == ds707_query_sys_chg_sm_state())
          {
            DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                           "Moved to NON_EPC system: Disable Phys link flow - Mask: "
                           "DS_FLOW_707_LOCK_MASK");
            ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK); //Do we still 
            // need to disable the phys link even when there is no IFACEs in UP 
            // state? who will enable the flow if we disable during no call 
            // scenario
          }
#else
          ds707_sys_chg_epc_to_nonepc_ind();
#endif  /* FEATURE_DATA_WLAN_MAPCON*/         
        }

        /*--------------------------------------------------------------------- 
          transition sr_state correctly depending on the type of handoff
        ---------------------------------------------------------------------*/
        ds707_sys_chg_sr_process_epc_handoff();
        /*--------------------------------------------------------------------- 
          tear down all the UP ifaces
        ---------------------------------------------------------------------*/
        for ( curr_iface_index = 0; 
              curr_iface_index < DS707_MAX_IFACES; 
              curr_iface_index++ )
        {      
          iface_ptr = NULL ;
          iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

#ifdef FEATURE_DATA_WLAN_MAPCON 
#ifdef FEATURE_EPC_HANDOFF
          /* If some ifaces are up, EPC will decide whether to handoff the PDNs are
             tear down. MH need not take any action for UP ifaces on EPC_TO_NONEPC
             transition */
          if (ps_iface_state(iface_ptr) == IFACE_UP &&
              DS707_CBACK_HANDOFF_EPC_TO_NONEPC == event)
          {
            DATA_IS707_MSG3( MSG_LEGACY_MED, 
                             "Handoff:%d, Iface:0x%x:%d is UP. wait for EPC to "
                             "Handoff / tear down the iface",
                             event,
                             iface_ptr->name,
                             iface_ptr->instance);

            /*-----------------------------------------------------------------
              No need to notify device manager since EPC might handoff this
              iface to iWLAN
            -----------------------------------------------------------------*/
            handoff_notify_to_devmgr = FALSE;

            continue;
          }
#endif /* FEATURE_EPC_HANDOFF */
#endif /* FEATURE_DATA_WLAN_MAPCON */ 
        
          /* Tear down coming up ifaces and if EPC is not present - tear down
             Up ifaces as well (EPC module present or not is taken care
             with the feature def check above. If transition is nonepc_to_epc
             tear down all coming up and up ifaces (EPC doesn't handle nonepc_to_epc
             transition */
          if ( (ps_iface_state(iface_ptr) != IFACE_DOWN) && 
               (ps_iface_state(iface_ptr) != IFACE_STATE_INVALID) &&
               (ps_iface_state(iface_ptr) != IFACE_DISABLED) )
          {
            DATA_IS707_MSG3( MSG_LEGACY_MED, 
                             "Handoff:%d, Tearing down iface:0x%x:%d",
                             event,
                             iface_ptr->name,
                             iface_ptr->instance);

            /*-----------------------------------------------------------------
              Set call end reason to indicate teardown due to handoff
           -----------------------------------------------------------------*/
            ds707_pdncntx_teardown_iface(iface_ptr, 
                                         PS_NET_DOWN_REASON_REDIR_OR_HANDOFF);
          }
        }                                                  
      }

      /*-----------------------------------------------------------------------
        Notify device manager of handoff if required
      -----------------------------------------------------------------------*/
      if (TRUE == handoff_notify_to_devmgr)
      {
        ds707_devmgr_epc_to_nonepc_handoff_ind();
      }
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "Handoff:%d, Not notifying devmgr", event);
                       
      }

      /*--------------------------------------------------------------
      System info should be moved to NO_SYSTEM. This state change is 
      called in ppp_ev_callback. Next time call is brought up it will 
      be changed to EPC or NON-EPC depending on system. 
      ---------------------------------------------------------------*/
      break;
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_OPTHO
    case DS707_CBACK_TUN_PHYS_LINK_DOWN_EV:
    {
      if (event_info_ptr->going_dormant == TRUE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Tunnel phys link Down ind");

        /*---------------------------------------------------------------
          Move the tun phys link state to DOWN
        ---------------------------------------------------------------*/
        ds707_transition_tun_phys_link_state(tc_state_ptr, DS707_TUN_PHYS_LINK_DOWN);

        ds707_pkti_reg_pri_wmk_reorig_cbacks(tc_state_ptr);

        /*-------------------------------------------------------------------
          Empty the watermarks to discard any data that was sitting before
          reorig callback was registered.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Clearing wmks & enabling phys link flow after "
                         "reorig callback registration");
 
        ds707_pkti_cleanup_wmks();

        /*---------------------------------------------------------------
          Enable the phys link flow
        ---------------------------------------------------------------*/
        ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

#ifdef FEATURE_HDR_QOS
        /* Registering the reorig function for all the secondary watermarks
         */
        ds707_sec_pkt_reg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();     

#endif /* FEATURE_HDR_QOS */        

        /*Giving down indication for Primary phys link*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Giving down indication for Primary phys link");

        /*--------------------------------------------------------------
         Reset bearer_id 
         --------------------------------------------------------------*/
        phys_link_ptr->bearer_id = DSRLP_INVALID_HDR_MRLP_FLOW;

        ps_phys_link_down_ind(phys_link_ptr);

#ifdef FEATURE_HDR_QOS
        /*---------------------------------------------------------------
          Post down indication for Secondary phys link
        ---------------------------------------------------------------*/
        ds707_pkt_post_sec_phys_link_down_ind();       
#endif /* FEATURE_HDR_QOS */  
      
        /*-----------------------------------------------------------
          Set LDSN to CDSN when the pkt call goes dormant upon
          receiving Phys link down event
        -----------------------------------------------------------*/
        ds707_pkt_set_last_data_sess_network(ds707_pkt_get_current_data_sess_network());
        DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Tun Phys Link Down: CDSN:%d LDSN:%d ",
                        ds707_pkt_get_current_data_sess_network(),
                        ds707_pkt_get_last_data_sess_network());
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Tunnel phys link Gone ind");

        /*---------------------------------------------------------------
          Move the tun phys link state to NULL
        ---------------------------------------------------------------*/
        ds707_transition_tun_phys_link_state(tc_state_ptr, DS707_TUN_PHYS_LINK_NULL);

        /*---------------------------------------------------------------
          Enable the phys link flow
        ---------------------------------------------------------------*/
        ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

        /*------------------------------------------------------------- 
          As a part of processing CARD REFRESH/PWROFF/LPM mode notification
          from CM, DS will tear down all ifaces. This function is to notify 
          CM when the data session is torn down
        -------------------------------------------------------------*/
        /*--------------------------------------------------------------------- 
          In case of CARD REFRESH/LPM/PWR OFF mode, Notify DSMGR which in turn
          will notify CM after all the other internal DS modules are done
          processing
        ---------------------------------------------------------------------*/
        if (ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress() == TRUE)
        {
          ds3geventmgr_clr_and_notify_lpm_pwr_off_status(
            DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK);
        }
        else if (ds3geventmgr_is_sub_not_avail_processing_in_progress(
                                                   SYS_MODEM_AS_ID_1) == TRUE)
        {
          ds3geventmgr_clr_and_notify_sub_not_avail_status(
            DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK,
            SYS_MODEM_AS_ID_1,
            ds3gi_sub_not_avail_subs_id_mask);
        }

        ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
        /* De-register all the watermarks */
        ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();
#endif /* FEATURE_HDR_QOS */

        /*-----------------------------------------------------------
          There might be routing take place in the middle of below
          cleanup. In such cases we need to block new originations.
        -----------------------------------------------------------*/
        ds707_set_term_in_progress(TRUE);
        
        /* This may need to be Revisited since we are trying to enter
           PS critical section from DS task context
           -- Comment and logic copied from DS707_CBACK_PHYS_LINK_DOWN_EV
           above */
        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

        /*-----------------------------------------------------------
          Release all secondary flows and post 
          go null indication on the primary flow for all ifaces
        -----------------------------------------------------------*/
        ds707_pkt_rel_sec_flows_and_post_go_null_ind();  

#ifdef FEATURE_EHRPD
        /*-----------------------------------------------------------
          Abort device manager
        -----------------------------------------------------------*/
        if (0 != ds707_devmgr_abort())
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_abort() failed");
        }
#endif /* FEATURE_EHRPD */
                        
        ps_phys_link_gone_ind(phys_link_ptr);

        DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_END_IND");
        ds3gflowmgr_notify_ps_call_state_per_sub(
           SYS_MODEM_AS_ID_1, DS3GFLOWMGR_PS_CALL_END_EV);

        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

        /*------------------------------------------------------------- 
          Clear termination in progress flag.
        -------------------------------------------------------------*/
        ds707_set_term_in_progress(FALSE);

        /*-----------------------------------------------------------
          Set LDSN and CDSN to NO_SRV when the pkt 
          call goes NULL upon receiving Phys link gone event
        -----------------------------------------------------------*/
        ds707_pkt_set_current_data_sess_network(SYS_SYS_MODE_NO_SRV);
        ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_NO_SRV);

         DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Tun Phys Link Gone: CDSN:%d LDSN:%d ",
                        ds707_pkt_get_current_data_sess_network(),
                        ds707_pkt_get_last_data_sess_network());  

        /*-----------------------------------------------------------
          Cleanup all watermarks
        -----------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Flushing all watermarks when going NULL");
        ds707_pkti_cleanup_wmks();

        /*-----------------------------------------------------------
          Cleanup the Primary RX watermark
        -----------------------------------------------------------*/
        for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
        {
          dsm_empty_queue(&tc_state_ptr->fwd_rlp.post_rx_wm_list[p]);
        }

#ifdef FEATURE_HDR_QOS 
        /*-----------------------------------------------------------
          Cleanup the Sec RX watermark and post gone Ind
        -----------------------------------------------------------*/
        ds707_pkt_cleanup_sec_rx_wmks_and_post_gone_ind();
#endif /* FEATURE_HDR_QOS */
      }

#if (defined(FEATURE_HDR) && !defined(TEST_FRAMEWORK) )
      hdrrlplog_stop_log_pkt();
#endif /* FEATURE_HDR && !TEST_FRAMEWORK */
  
      break;
    }

    case DS707_CBACK_TUN_PHYS_LINK_UP_EV:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "Tunnel phys link Up ind, flow is enabled");

      /*---------------------------------------------------------------
          Move the tun phys link state to UP
      ---------------------------------------------------------------*/
      ds707_transition_tun_phys_link_state(tc_state_ptr, DS707_TUN_PHYS_LINK_UP);

      /* enable flow incase flow was disabled when orig failed */
      ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

#ifdef FEATURE_CALL_THROTTLE
      /*---------------------------------------------------------------------
        If the call is coming out of dormancy we  post call_success to DCTM
        as PPP is already UP hence the call is successful.  For the first 
        call we wait till PPP comes up before we report SUCCESS to DCTM 
        module. Without this fix if the previous re-orig failed at system 
        level for some reason, the call remains throttled.
        -- Comment and logic copied from DS707_CBACK_PHYS_LINK_UP_EV above
      ---------------------------------------------------------------------*/
      for ( curr_iface_index = 0; 
            curr_iface_index < DS707_MAX_IFACES; 
            curr_iface_index++ )
      {      
        iface_ptr = NULL ;
        iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

        if ( PS_IFACE_IS_VALID ( iface_ptr ) )
        {
          /*---------------------------------------------------------------
            Upon Call origination check if returning from a DORMANT call
          ---------------------------------------------------------------*/
          if ( (ds707_pkt_is_um_iface_up(iface_ptr) == TRUE))
          {
            /* Though this is the same function called from
               DS707_CBACK_PHYS_LINK_UP_EV as well, since we are in tunnel
               mode, instead of informing CM, the proxy DCTM will service
               this */
            dctm_post_ppp_call_status( PPP_SUCCESS_EV,
                                       DSDCTM_PPP_PROTOCOL_IPCP,
                                       PPP_FAIL_REASON_NONE);
            
            /* Fall out of the loop */
            break;
          }          
        } /* is valid iface */
      } /* for each iface */
#endif /* FEATURE_CALL_THROTTLE */

     /*-----------------------------------------------------------------
      Populate bearer_id with rlp flow_id 
      -----------------------------------------------------------------*/
     phys_link_ptr->bearer_id = 
        tc_state_ptr->fwd_rlp.flow.flow;

      ps_phys_link_up_ind(phys_link_ptr);

      DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_ACTIVE_IND");
      ds3gflowmgr_notify_ps_call_state_per_sub(SYS_MODEM_AS_ID_1,
                                               DS3GFLOWMGR_PS_CALL_ACTIVE_EV);
      
#ifdef FEATURE_HDR_QOS
      ds707_pkt_post_sec_phys_link_up_ind();
#endif /* FEATURE_HDR_QOS */

      ds707_pkt_set_current_data_sess_network(SYS_SYS_MODE_HDR);
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Tun Phys Link Up: CDSN:%d LDSN:%d ",
                        ds707_pkt_get_current_data_sess_network(),
                        ds707_pkt_get_last_data_sess_network());

#if (defined(FEATURE_HDR) && !defined(TEST_FRAMEWORK) )
      hdrrlplog_start_log_pkt();
#endif /* FEATURE_HDR && !TEST_FRAMEWORK */
      break;
    }

    case DS707_CBACK_TUN_INC_CALL_EV:
    {
      ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
      /* De-register all the watermarks */
      ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();
#endif
      /*-------------------------------------------------------------------
         Set the inc_call flag to TRUE (though it may not be used in
         tunnel mode)
      -------------------------------------------------------------------*/
      pkt_info.inc_call        = TRUE; 

      (void)ps_phys_link_up_cmd( phys_link_ptr, 
                                 &err_num, 
                                 &pkt_info);
    
      break;
    }

    case DS707_CBACK_TUN_ABORT_SESSION_EV:
    {
      ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

      ds707_pkti_dereg_pri_wmk_reorig_cbacks(tc_state_ptr);

#ifdef FEATURE_HDR_QOS
      /* De-register all the watermarks */
      ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces();
#endif /* FEATURE_HDR_QOS */

      /*-----------------------------------------------------------
        There might be routing take place in the middle of below
        cleanup. In such cases we need to block new originations.
      -----------------------------------------------------------*/
      ds707_set_term_in_progress(TRUE);

      /*-----------------------------------------------------------
        Release all secondary flows and post 
        go null indication on the primary flow for all ifaces
      -----------------------------------------------------------*/
      ds707_pkt_rel_sec_flows_and_post_go_null_ind();
      
      /*-----------------------------------------------------------
        Set LDSN and CDSN to NO_SRV when the pkt 
        call goes NULL upon receiving an ABORT_SESSION event
      -----------------------------------------------------------*/
      ds707_pkt_set_current_data_sess_network(SYS_SYS_MODE_NO_SRV);
      ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_NO_SRV);
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Tun Phys Link Abort: CDSN:%d LDSN:%d ",
                        ds707_pkt_get_current_data_sess_network(),
                        ds707_pkt_get_last_data_sess_network());                   

      /*-----------------------------------------------------------
        Set call end reason for each valid iface
      -----------------------------------------------------------*/
      if(
          (
            (event_info_ptr->tear_down_info.validity_mask)
            & 
            (PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK)
           ) 
          == PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK ) 
      {
        ds707_pkt_mgr_set_end_reason_for_all_ifaces(
               event_info_ptr->tear_down_info.data_info.call_end_reason);
      }
      else
      {
      ds707_pkt_mgr_set_end_reason_for_all_ifaces(
               PS_NET_DOWN_REASON_NOT_SPECIFIED);
      } 

#ifdef FEATURE_EHRPD
        /*-----------------------------------------------------------
          Abort device manager
        -----------------------------------------------------------*/
        if (0 != ds707_devmgr_abort())
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_abort() failed");
        }
#endif /* FEATURE_EHRPD */
      if(phys_link_ptr != NULL)
      {

        DATA_IS707_MSG1( MSG_LEGACY_MED,
                         "Calling pri ps_phys_link_go_null_cmd on phys_link : 0x%x",
                         phys_link_ptr);
        rval = ps_phys_link_go_null_cmd(phys_link_ptr,
                                        &err_num,
                                        NULL);
        DATA_IS707_MSG2( MSG_LEGACY_MED,
                         "Called pri ps_phys_link_go_null_cmd: ret: %d, err code: %d",
                         rval,
                         err_num);
        
        if( rval == 0 )
        {
          DATA_IS707_MSG1( MSG_LEGACY_MED,
                         "Calling pri ps_phys_link_gone_ind on phys_link : 0x%x",
                         phys_link_ptr);
          ps_phys_link_gone_ind(phys_link_ptr);
          
          DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_END_IND");
          ds3gflowmgr_notify_ps_call_state_per_sub(
             SYS_MODEM_AS_ID_1, DS3GFLOWMGR_PS_CALL_END_EV);
        }
  
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "Pri phys link is NULL");
      }

#ifdef FEATURE_HDR_QOS
      /* Post secondary phys link go null cmd */
      ds707_pkt_post_sec_phys_link_go_null_cmd();
#endif /* FEATURE_HDR_QOS */

      /* Cleanup complete */
      ds707_set_term_in_progress(FALSE);

      break;
    }
#endif /* FEATURE_DATA_OPTHO */

    default:
      DS_FORCE_ASSERT();
      break;
  }
} /* ds707_pkti_event_cback() */

/*===========================================================================
FUNCTION       DS707_PKTI_REGISTER_FOR_EVENTS

DESCRIPTION    Register for particular phone events

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkti_register_for_events(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_cback_register( DS707_CBACK_PHYS_LINK_DOWN_EV, 
                            ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_DOWN_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_PHYS_LINK_UP_EV,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_PHYS_LINK_COMING_UP_EV,
                          ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_COMING_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_PHYS_LINK_RESUMING_EV,
                          ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_RESUMING_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_INC_CALL_EV,
                           ds707_pkti_event_cback) == FALSE)
                          
  {
    ERR_FATAL("Could not reg INC CALL cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_ABORT_SESSION_EV,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg ABORT SESSION cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_HOLDDOWN_TIMER_STARTED_EV,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg HDOWN TIMER START cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg HDOWN TIMER STOP cback",0,0,0);
  }

  #ifdef FEATURE_EHRPD
  if (ds707_cback_register(DS707_CBACK_HANDOFF_EPC_TO_NONEPC,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg DS707_CBACK_HANDOFF_EPC_TO_NONEPC ",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_HANDOFF_NONEPC_TO_EPC,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg DS707_CBACK_HANDOFF_NONEPC_TO_EPC ",0,0,0);
  }
  #endif /*FEATURE_EHRPD */

#ifdef FEATURE_DATA_OPTHO
  if (ds707_cback_register( DS707_CBACK_TUN_PHYS_LINK_DOWN_EV, 
                            ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_DOWN_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_TUN_PHYS_LINK_UP_EV,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_TUN_PHYS_LINK_COMING_UP_EV,
                          ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_COMING_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_TUN_PHYS_LINK_RESUMING_EV,
                          ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_RESUMING_EV cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_TUN_INC_CALL_EV,
                           ds707_pkti_event_cback) == FALSE)

  {
    ERR_FATAL("Could not reg INC CALL cback",0,0,0);
  }

  if (ds707_cback_register(DS707_CBACK_TUN_ABORT_SESSION_EV,
                           ds707_pkti_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg ABORT SESSION cback",0,0,0);
  } 
#endif /* FEATURE_DATA_OPTHO */

} /* ds707_pkti_register_for_events() */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKT_MGR_UPDATE_CDSN

DESCRIPTION   Called everytime the CDSN is updated.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_update_cdsn(sys_sys_mode_e_type current_data_sess_network)
{
  ds707_pkt_set_current_data_sess_network(current_data_sess_network);
  DATA_IS707_MSG1( MSG_LEGACY_MED, "Updated CDSN to %d",
                   current_data_sess_network);

  ds707_extif_inform_hdr_sess_status();

#ifdef FEATURE_HDR_QOS
  /* If the current data session network happens to be HDR, call 
     the method to post an indication that the mobile has transitioned 
     into a DO network*/
  if(SYS_SYS_MODE_HDR == current_data_sess_network)
  {
    ds707_sec_pkt_handoff_to_qos_aware_sys(TRUE);
  }
#endif /* FEATURE_HDR_QOS */
}

/*===========================================================================
FUNCTION      DS707_PKT_SETUP_RLP

DESCRIPTION   Sets up RLP with a TX watermark and RLP RX func and RLP RX
              queue.  If the inputs are NULL, then use defaults as
              defined by this module.  Else use the ones passed into this
              function.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_setup_rlp
(
dsm_watermark_type  tx_wm_list[],          /* Tx wms ptr for txmit frs     */
void                (*post_rx_func_ptr)(uint32), /* rx cback for rx frames */
dsm_watermark_type  post_rx_wm_list[]      /* watermarks for rx'ed frames  */
)
{
  ds707_tc_state_type *tc_state_ptr;
  ps_phys_link_type   *pri_phys_link_ptr;    
  ps_phys_link_link_protocol_handle_type protocol_handle;
  ds707_flow_type *curr_flow_ptr;
  uint8  curr_iface_index = 0;
  ps_iface_type* curr_iface_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  tc_state_ptr = ds707_get_pri_tc_ptr();

  if ((tx_wm_list == NULL) &&
      (post_rx_func_ptr == NULL) &&
      (post_rx_wm_list    == NULL))
  {
    ds707_pkti_set_rlp_defaults();
  }
  else
  {
    /* For all the default DS Flows, point its tx_wm_list to the one that 
       is passed in, this will get reset when setup_rlp is called with 
       NULL values.
       This makes sure that no matter which IFACE gets selected all the 
       flows point to the same watermark
    */
    for ( curr_iface_index = 0;
          curr_iface_index < DS707_MAX_IFACES;
          curr_iface_index ++ )
    {      
      curr_flow_ptr = ds707_get_flow_pool_ptr(curr_iface_index);
      curr_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(
                                                          curr_iface_index);
      ASSERT ( curr_iface_ptr == curr_flow_ptr->ps_iface_ptr );
      ASSERT ( PS_IFACE_IS_FLOW_DEFAULT(curr_iface_ptr, 
                                        curr_flow_ptr->ps_flow_ptr ) );
      curr_flow_ptr->tx_wm_list = tx_wm_list;
    }  
    tc_state_ptr->fwd_rlp.post_rx_func_ptr = post_rx_func_ptr;
    tc_state_ptr->fwd_rlp.post_rx_wm_list   = post_rx_wm_list;
  }

  /*----------------------------------------------------------------------
    Set PPP to be the link protocol for the default RLP.
    ----------------------------------------------------------------------*/
  pri_phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  protocol_handle.ppp_device = PPP_UM_SN_DEV;    
  (void)ps_phys_link_set_link_protocol(pri_phys_link_ptr,
                                       PS_PHYS_LINK_LINK_PROTOCOL_PPP,
                                       protocol_handle);

  (void)ps_phys_link_set_framing_handle(pri_phys_link_ptr,
                                        PS_PHYS_LINK_FRAMING_PROTOCOL_HDLC,
                                        tc_state_ptr->fwd_rlp.hdlc_inst_ptr);

  ds707_pkti_reg_1x_rlp_srvc(tc_state_ptr); 
#ifdef FEATURE_HDR
  ds707_pkti_reg_hdr_rlp_srvc(); 
#endif

} /* ds707_pkt_setup_rlp() */

/*===========================================================================
                 FUNCTIONS FOR PPP/RLP INTERACTION
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_RX_UM_SN_SIG_HDLR

DESCRIPTION   Called by PPP to get forward-link data from RLP.  Specific to
              HDR SN/1x-Data.

DEPENDENCIES  None.

RETURN VALUE  TRUE : all done emptying all rx queues
              FALSE: otherwise
             
SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 unused variables*/
boolean ds707_rx_UM_SN_sig_hdlr
(
  ps_sig_enum_type sig,
  void *dummy
)
{
  int                  i;
  ds707_tc_state_type *tc_state_ptr;
  dsm_item_type       *item_data_ptr;
  boolean              done_handling_signal = TRUE;
#ifdef FEATURE_EHRPD
  ds707_pdn_context_cb_type * pdncntx_cb_ptr;
  ps_iface_type * iface_ptr;
#endif /* FEATURE_EHRPD */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(dummy);

  /* Process at most one packet from each rx watermark.  As long as at */
  /* least one packet is processed, return FALSE to the PS main loop, */
  /* which will cause it to call us again after other PS signals get a */
  /* chance to be handled (prevents watchdog timeouts) */
  for (i = 0; i < DS707_NUM_PHYS_LINKS; ++i)
  {
    tc_state_ptr = GET_TC_FROM_PHYS_LINK_PTR(ds707_get_phys_link_ptr(i));
    if (tc_state_ptr == NULL)
    {
      continue;
    }

    /* Phys link zero is always the default flow (bi-directional), */
    /* else it needs to be forward MRLP. */
    if ( i == 0 || 
         tc_state_ptr->fwd_rlp.flow.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
    {
      item_data_ptr = dsrsp_q_get(&tc_state_ptr->fwd_rlp.flow,
                                  tc_state_ptr->fwd_rlp.post_rx_wm_list);
      if (item_data_ptr != NULL)
      {
#ifdef FEATURE_DATA_3GPP2_VS
        /*------------------------------------------------------------------------ 
          Restart Max PPP inactivity timer for RX path.
          ds_3gpp2_mpit_timer_restart() function will restart the timer only 
          if the timer is running.
        ------------------------------------------------------------------------*/
        (void)ds_3gpp2_mpit_timer_restart();
#endif /* FEATURE_DATA_3GPP2_VS */

        (void)ps_phys_link_input(tc_state_ptr->ps_phys_link_ptr, 
                                 &item_data_ptr, 
                                 NULL);
        done_handling_signal = FALSE;
#ifdef FEATURE_EHRPD
        /* If this is eHRPD non-PPP QoS data, start the PDN inactivity timer. */
        /* The assumption here is that one RLP serves one PDN for non-PPP flows */
        if( ( ds707_pkt_is_ehrpd_mode_of_operation() ) &&
            ( tc_state_ptr->ps_phys_link_ptr->link_protocol_info.link_rx_f_ptr == NULL ) 
          )
          {
            if(tc_state_ptr->fwd_rlp.flow_ptr[0] != NULL)
            {
            iface_ptr = tc_state_ptr->fwd_rlp.flow_ptr[0]->ps_iface_ptr;
            if( iface_ptr != NULL )
            {
              pdncntx_cb_ptr = 
                ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
              if( pdncntx_cb_ptr != NULL )
              {
                DATA_IS707_MSG0( MSG_LEGACY_MED, 
                                 "non-PPP QoS RX: Start Inactivity Timer");
                ds707_pdncntx_inactivity_timer_start(pdncntx_cb_ptr); 
              }
            }
            }
        } /* eHRPD non-PPP data */
#endif /* FEATURE_EHRPD */
      }
    }
  }

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    If we have exhausted the receive queue on a HDR system, re-enable flow
    from the base station if it was disabled.

    The flag hdrds_ps_q_lim_exceeded below will only be set to TRUE if we
    are in an HDR system in a network model/sockets call and one queue is 
    too large.

    If we are in a 1X system or in a relay model call, this flag will
    always be false.

    Since this function is in the critical path for all data received on the
    UM iface, hdrds_ps_q_lim_exceeded flag should be checked first to 
    optimize processing.
  -------------------------------------------------------------------------*/
  if ((done_handling_signal == TRUE) && 
      (*(get_hdrds_ps_q_lim_exceeded_ptr()) == TRUE))
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, "Remove NULL COVER");
    /* This function is always being called from PPP. 'hdrds_ps_q_lim_exceeded' 
       is always FALSE. This is a DEAD code. Discuss to see if we can remove 
       this whole block of code. */
    *(get_hdrds_ps_q_lim_exceeded_ptr()) = FALSE;
    hdrl1_fmac_disable_flow_control ((uint32)HDRFMAC_DRC_REQ_PSQ);
  }
#endif
#ifdef FEATURE_DS_MOBILE_IP
  /*-----------------------------------------------------------------------
   Let MIP code know that data is being transferred over Um interface
  -----------------------------------------------------------------------*/
  mip_data_transfer_action();
#endif
  return done_handling_signal;
}
/*lint -restore*/

/*===========================================================================
FUNCTION      DS707_PKT_TRASH_PRI_RLP_TX_WMKS

DESCRIPTION   This function is registered with PS_IFACE.  When user calls
              this (via PS_IFACE), this will go ahead an empty the RLP
              TX watermark of all items (i.e, rev link).  By "empty" we
              mean that all data will be trashed.

              This trashes the tx watermarks of all RLP instances.

              THIS EXECUTES IN CALLER's CONTEXT.  May not be in DS TASK.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_trash_pri_rlp_tx_wmks
(
ps_iface_type    *iface_ptr
)
{  
  ds707_tc_state_type *tc_state_ptr;
  ds707_flow_type     *flow_ptr;
  int                  p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Trashing 1x RLP trasmit queue");

  tc_state_ptr = ds707_get_pri_tc_ptr();
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
  -------------------------------------------------------------------------*/
  flow_ptr     = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);
 
  if (flow_ptr->tx_wm_list != NULL)
  {
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      DATA_IS707_MSG2( MSG_LEGACY_MED, 
                       " dumped %d buffers, %d bytes",
                       q_cnt( flow_ptr->tx_wm_list[p].q_ptr ),
                       flow_ptr->tx_wm_list[p].current_cnt);

      dsm_empty_queue(&flow_ptr->tx_wm_list[p]);
    }
  }
  /*lint -e550 variable i is assigned a value*/
  /*lint -restore*/
} /* ds707_pkt_trash_pri_rlp_tx_wmks() */


/*===========================================================================
FUNCTION      DS707_CONFIGURE_VJ_COMPRESSION_FOR_PPP

DESCRIPTION   Called to request VJ compression during PPP negotiation.
              VJ is not defined for IPv6.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_configure_VJ_compression_for_PPP
(
  /* pointer to PPP config info structure. Used during PPP setup */
  ppp_dev_opts_type   * ppp_config_info
)
{
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Configuring VJ compression for PPP");
  /* set the compress bit */ 
  ppp_config_info->ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_COMPRESS; 

  /* set VJ compression bit */
  ppp_config_info->ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_COMPRESS_VJ;  

  /* VJ protocol */ 
  ppp_config_info->ipcp_info.want_vals[FSM_REMOTE].compression = 
                                                          PPP_VJCH_PROTOCOL;
  ppp_config_info->ipcp_info.want_vals[FSM_REMOTE].vj_config.num_slots = 
                                                          IPCP_SLOT_DEFAULT; 
  ppp_config_info->ipcp_info.want_vals[FSM_REMOTE].vj_config.slot_comp = 
                                                      IPCP_NO_SLOT_COMPRESS;

} /* ds707_configure_VJ_compression_for_PPP */

/*===========================================================================
FUNCTION      DS707_CONFIGURE_JCDMA_PARAMS_FOR_PPP

DESCRIPTION   Called to request JCDMA phone parameters to be configured prior 
              to PPP negotiation.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_configure_jcdma_params_for_ppp
(
  ppp_dev_opts_type   * ppp_config_info /* pointer to PPP config info 
                                           structure. Used during PPP setup*/
)
{
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Configuring JCDMA params for PPP");
  
  /*------------------------------------------------------------------ 
     Set PPP graceful close to be true, to indicate that an LCP term
     request is to be sent to terminate PPP incase PPP fails to 
     establish 
  ------------------------------------------------------------------*/ 
  ppp_config_info->ppp_failure_graceful_close = TRUE; 

  /*------------------------------------------------------------------ 
     Set stabilize accm delay to be false, to indicate that a discard
     request should not be send at the end of LCP negotiation for JCDMA
     networks 
  ------------------------------------------------------------------*/ 
  ppp_config_info->set_stabilize_accm_delay = FALSE;  

  /*------------------------------------------------------------------ 
     Set negotiate dns opts to be FALSE to ensure that the primary and 
     secondary DNS options are not renegotiated when a resync happens
     for JCDMA networks
  ------------------------------------------------------------------*/ 
  ppp_config_info->resync_dns_opts = FALSE;

} /* ds707_configure_jcdma_params_for_ppp */


/*===========================================================================
FUNCTION      DS707_CONFIGURE_JCDMA_SPECIAL_AN_AUTH_PARAMS

DESCRIPTION   Called to fill in JCDMA auth info for HDR AN PPP authentication.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_configure_jcdma_special_an_auth_params
(
  ppp_auth_info_type   *ppp_auth_info   /* pointer to PPP auth info structure
                                           used during PPP AN auth phase */
)
{
  nv_item_type         ds707_nv_item;
  nv_stat_enum_type    nv_status;             /* status from NV call	   */
  uint8                imsi_string[DS707_IMSI_STR_MAX_LEN];
  uint8                a_key[A_KEY_AUTH_DIGITS+1];
  byte                 *imsi_string_ptr = NULL;
  uint8                i,j;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
     Construct the special NAI. This should be constructed from IMSI which is in encoded form
     in the follwoing NV items: 
     NV_IMSI_MCC_I, 
     NV_IMSI_11_12_I,
     NV_MIN2_I and 
     NV_MIN1_I
     We have to read the above four NV items and decode the IMSI.
   ---------------------------------------------------------------------------*/
 
  memset( &ppp_auth_info->user_id_info[0], 0, PPP_MAX_USER_ID_LEN );
  memset( &imsi_string[0], 0, DS707_IMSI_STR_MAX_LEN );
  memset( &a_key[0], 0, A_KEY_AUTH_DIGITS+1 );
  
  /*-------------------------------------------------------------------------
    The NV Item MCC is a 10-bit (16-bit in storage) bit-string that contains
    3 decimal digits encoded in this fashion.
    |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
    |   6-bits  |     10-bits         |
    |<--------->|<------------------->|
    |  padding  |     3-digits        |
    |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
    Read the MCC from the NV first.
  -------------------------------------------------------------------------*/	  
  imsi_string_ptr = &imsi_string[DS707_IMSI_MCC_OFFSET];
  if (FALSE == ds707_pkt_mgr_read_imsi_mcc(imsi_string_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  "Error retrieving IMSI MCC from NV" );
    return FALSE;
  }

  /*-----------------------------------------------------------------------------
    Read IMSI_11_12. This is MNC
    The NV Item MNC is a 8-bit integer in the range of 0 to 99.

    Append the MNC to the existing string.
   ----------------------------------------------------------------------------*/
  imsi_string_ptr = &imsi_string[DS707_IMSI_MNC_OFFSET];
  if (FALSE == ds707_pkt_mgr_read_imsi_mnc(imsi_string_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  "Error retrieving IMSI MNC from NV" );
    return FALSE;
  }

  /*-----------------------------------------------------------------------------
    Read MIN_2. convert in to 3 digits

    Append the MIN_2 to the existing string.
   ----------------------------------------------------------------------------*/
  imsi_string_ptr = &imsi_string[DS707_IMSI_MIN2_OFFSET];
  if (FALSE == ds707_pkt_mgr_read_min2(imsi_string_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  "Error retrieving MIN2 from NV");
    return FALSE;
  }

  /*-----------------------------------------------------------------------------
    Read MIN_1
    Now read the NV item NV_MIN1_1
    The NV Item MIN1 is a 24-bit bit-string that contains 7 decimal digits
    encoded in this fashion.
    |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
    |   10-bits                  | 4-bits   |  10-bits                     |
    |<----------------->|<----->|<------------------->|
    |   3-digits                 |1-digit    |    3-digits                   |
    |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
 
    Append the MIN_1 to the existing string.
   ---------------------------------------------------------------*/
  imsi_string_ptr = &imsi_string[DS707_IMSI_MIN1_OFFSET];
  if (FALSE == ds707_pkt_mgr_read_min1(imsi_string_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  "Error retrieving MIN1 from NV" );
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Read the A-Key and construct the password 
  -------------------------------------------------------------------------*/
  memset(&ds707_nv_item, 0, sizeof(nv_item_type));
  ds707_nv_item.a_key.nam = (byte) CM_NAM_1;

  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_A_KEY_I, &ds707_nv_item, ds707_curr_subs_id() );
  if( nv_status != NV_DONE_S )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR," A_key is not active ");
    return FALSE;
  }

  ppp_auth_info->user_id_len = 15;
  for( i = 0; i < ppp_auth_info->user_id_len; i++ )
  {
    ppp_auth_info->user_id_info[i] =  imsi_string[i];
  }
    
  ppp_auth_info->user_id_info[ppp_auth_info->user_id_len] = '\0';

  for( i = 0 ; i <= ppp_auth_info->user_id_len; i++ )
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,"User ID[%d]: 0x%x",
                    i, ppp_auth_info->user_id_info[i]);
  }

  memscpy((void *)a_key, (A_KEY_AUTH_DIGITS + 1), 
          (void *)ds707_nv_item.a_key.key, sizeof(qword));

  for ( i = 0,j = 0; j < A_KEY_AUTH_DIGITS; j++, i+=2 )
  {
    ppp_auth_info->passwd_info[i+1] = 
                   (a_key[A_KEY_AUTH_DIGITS - j - 1] & 0x0F) + 0x40;
    ppp_auth_info->passwd_info[i] = 
                   ((a_key[A_KEY_AUTH_DIGITS - j - 1] & 0xF0) >> 4) + 0x40;
  }

  ppp_auth_info->passwd_len = i;
  ppp_auth_info->passwd_info[i+1]='\0';

  for( i = 0 ; i <= ppp_auth_info->passwd_len ; i++ )
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,"Password[%d]: 0x%x",
                    i , ppp_auth_info->passwd_info[i]);
  }

  return TRUE;
}

/*===========================================================================
FUNCTION      DS707_PKT_SETUP_SN_PPP

DESCRIPTION   Called when IFACE is coming up, to intialize PPP for the
              SN PPP.  This the interface used for 1X.

DEPENDENCIES  None

RETURN VALUE  TRUE for successful PPP config, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_setup_SN_ppp_for_rmsm
(
  void
)
{
  ppp_dev_opts_type*            ppp_config_info = NULL;
  ps_iface_type*                iface_ptr = NULL;
  ds707_data_session_profile_id requesting_profile_id = 
                                  DATA_SESSION_PROFILE_ID_INVALID;
  int                           ret_val = -1;
  ds707_data_session_profile_info_type *pdn_prof_ptr = NULL;
  uint8                         sip_profile_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_config_info = 
            modem_mem_alloc(sizeof(ppp_dev_opts_type), MODEM_MEM_CLIENT_DATA);
  if (ppp_config_info == NULL)
  {
    DATA_DOQOS_MSG1(MSG_LEGACY_ERROR, 
                    "Unable to allocate memory for ppp_config_info %d",
                    ppp_config_info);
    return -1;
  }

  memset((void*)ppp_config_info, 0, sizeof(ppp_dev_opts_type));

  ppp_get_default_opts( ppp_config_info );

  iface_ptr = ds707_rmsm_get_um_iface();

  if ( ds_qcmip_get_runtime_val() == 1 &&
       ds707_data_session_get_num_valid_profiles_in_nvruim() > 0 )
  {
    sip_profile_index = ds707_data_session_get_sip_active_profile_index();
    requesting_profile_id = sip_profile_index + 
                            DATA_SESSION_NVRUIM_PROFILE_MIN;
    ds707_data_session_set_iface_requesting_profile(iface_ptr,
                                                    requesting_profile_id,
                                                    ACTIVE_SUBSCRIPTION_1);
  }

  if (ds707_data_session_get_iface_requesting_profile(iface_ptr, 
                                                      &pdn_prof_ptr))
  {
    requesting_profile_id = pdn_prof_ptr->data_session_profile_id;
  }
  
  if( ds707_data_session_is_nvruim_profile(requesting_profile_id) )
  {
    /*-------------------------------------------------------------------
      Retrieve auth info from NVRUIM profile
    -------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Retrieving auth info from NVRUIM for "
                    "Non-DEFAULT profile");

    ds707_data_session_get_ppp_auth_info_for_nvruim_profile(
                                 &(ppp_config_info->auth_info) );
  }
  else
  {
    is707_get_ppp_auth_info_from_nv( &(ppp_config_info->auth_info), 
                                     ds3gcfgmgr_read_legacy_nv_ex );
  }

  /* As of PS12, rx_f_ptr is no longer used, as it is PPP specific. */
  /* Instead, we always use rx_sig_f_ptr, even for PPP flows. */
  ppp_config_info->rx_f_ptr       = NULL;
  ppp_config_info->rx_sig_f_ptr   = ds707_rx_UM_SN_sig_hdlr;
  ppp_config_info->tx_f_ptr       = ds707_pkt_tx_um_SN_data;

  /* Nothing needs to be done here for IPv6. The reason being that RMSM 
     does not and will never support IPv6. RmNet and QMI will be used to
     support IPv6.

     Hence the inherent assumption of IPv4 in the assignment is OK.
  */

  ppp_config_info->ipcp_info.rx_iface_ptr = ds707_rmsm_get_um_iface();
  
  ppp_config_info->lcp_info.mode  = PPP_CP_PASSIVE;
  ppp_config_info->ipcp_info.mode = PPP_CP_ACTIVE;
  ppp_config_info->hdlc_mode      = HDLC_SW;
  ppp_config_info->bridge_dev     = PPP_RM_DEV;

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ppp_config_info->lcp_info.want_vals[FSM_REMOTE].use_same_auth = TRUE;
    ds707_configure_VJ_compression_for_PPP(ppp_config_info);
    ds707_configure_jcdma_params_for_ppp(ppp_config_info);
  }
  /*Simplified JCDMA:
    3.2.1 Use the same PPP Auth type in the reconnection [D1-No.63]
    5.1.26 Enable VJ compression   [D1-No.78]*/
  else if(ds707_roaming_is_simplified_jcdma_mode())
  {
    ppp_config_info->lcp_info.want_vals[FSM_REMOTE].use_same_auth = TRUE;
    ds707_configure_VJ_compression_for_PPP(ppp_config_info);
  }
  
  ret_val = ppp_start( PPP_UM_SN_DEV, ppp_config_info );
  
  modem_mem_free(ppp_config_info, MODEM_MEM_CLIENT_DATA);
  
  return ret_val;
} /* ds707_pkt_setup_SN_ppp() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_EXIT_TC_CMD

DESCRIPTION   Process the exit traffic channel command obtained from CM

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

LOCAL void ds707_pkt_mgr_process_exit_tc_cmd(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "get EXIT_TC from CM, enable flow");
  ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);
} /* ds707_pkt_mgr_process_exit_tc_cmd */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_ROAM_CHG_CMD

DESCRIPTION   Process roam change command.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_pkt_mgr_process_roam_chg_cmd 
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  uint8                   array_pos = 0, bit_pos = 0;
  uint8                   roam_value = 0;
  boolean                 roam_flag = FALSE;
  ds707_roaming_ind_type *ds707_roaming_ind_ptr = NULL;
  
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if ((ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ERR_FATAL("CMD PTR NULL",0,0,0);
    return;
  }
  /*---------------------------------------------------------------------- 
    Set roam flag based on roam_status sent by CM.
    0-63 values are defind in standard.
    Value zero is considered Home and any positive value is considered Roam.
    64 - 255 values are extended values and the interpretation of the values
    are left to the network provider.
    Note: Every customer implments the logic based on roaming tables,
    and this might require change per customer bases. 
  -----------------------------------------------------------------------*/
  ds707_roaming_ind_ptr = (ds707_roaming_ind_type*)(ds_cmd_ptr->cmd_payload_ptr);
  roam_value = (uint8)(ds707_roaming_ind_ptr->roam_status);
  array_pos = roam_value / 8;
  bit_pos = roam_value % 8;
  
  if( ( roam_value >= 64 ) && 
      ( ds707_roam_ind_custom_home[array_pos] & ((uint8)(1 << bit_pos) ) ) )
  {
    roam_flag = FALSE;
    DATA_IS707_MSG3(MSG_LEGACY_HIGH,"Roam set to FALSE for value %d array pos%d, bit pos%d", 
                                   roam_value,array_pos,bit_pos);
  }
  else
  {
    if (roam_value == 0) 
    {
      roam_flag = FALSE;
    }
    else
    {
      roam_flag = TRUE;
    }
    DATA_IS707_MSG4(MSG_LEGACY_HIGH,"Roam set to %d for value %d array pos%d, bit pos%d", 
                                     roam_flag, roam_value, array_pos, bit_pos);
  }

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
  if (ds707_roaming_ind_ptr->sys_mode == SYS_SYS_MODE_HDR)
  {
    ds707_set_roam_flag(roam_flag);
  }
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#endif /* FEATURE_EHRPD */

  /* Notify DSD of roaming status change */
  ds3gdsdif_notify_dsd_roam_status_chg_event(ds707_roaming_ind_ptr->sys_mode, 
                                             ds707_roaming_ind_ptr->subscription_id,
                                             roam_flag);

} /* ds707_pkt_mgr_process_roam_chg_cmd */

LOCAL void ds707_pkt_mgr_process_sys_chg_cmd 
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("ds_cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

#ifdef FEATURE_DATA_OPTHO
  /*---------------------------------------------------------------------- 
    While HDR Actv HO is in progress, we ignore this command generated
    due to CM SS event.
    
    When S101 is in Tunnel call disallowed state, UE is in LTE, so we don't
    enable or disable the phys link flow. However, as part of S101 transition 
    to TUNNEL Allowed to NON-TUNNEL state, we again enable the phys link
    from s101 transition state function and S101 sys chg info function.
  -----------------------------------------------------------------------*/ 
  if ( ( ds707_s101_is_hdr_actv_ho_in_prog() ) || 
       ( ds707_s101_query_sm_state () == DS707_S101_SM_TUNNEL_CALL_DISALLOWED )
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "HDR actv ho already in prog.or S101 in TUN DISALLOWED state"
                    "Rcvd process_sys_chg_cmd with ds_707_not_in_service=%d"        
                    "- Ignore Enabling/Disabling of phys link flow",
                    *((boolean *)(ds_cmd_ptr->cmd_payload_ptr)));
    return;
  }
#endif /* FEATURE_DATA_OPTHO */

  if (*((boolean *)(ds_cmd_ptr->cmd_payload_ptr)))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "No service from CM, disable flow");
    ds707_phys_link_disable_flow(DS_FLOW_NO_SERVICE_MASK );
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "In Service now from CM, enable flow");
    ds707_phys_link_enable_flow(DS_FLOW_NO_SERVICE_MASK);
  }
} /* ds707_pkt_mgr_process_sys_chg_cmd */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_PH_SYS_SEL_PREF_NORMAL_CMD

DESCRIPTION   Process the ph_sys_sel_pref command obtained from CM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_ph_sys_sel_pref_normal_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  sys_sys_mode_e_type       curr_mode            = SYS_SYS_MODE_NONE;
  ds3g_cm_ph_ev_info_type  *cm_ph_event_info_ptr = NULL;
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  boolean                   hybrid_pref = TRUE;
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("ds_cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  cm_ph_event_info_ptr 
                = (ds3g_cm_ph_ev_info_type  *)ds_cmd_ptr->cmd_payload_ptr;

  curr_mode = ds3g_get_current_network_mode_ex (SYS_MODEM_AS_ID_1);
  if(ds707_data_sess_is_ds_centric_emerg_only_mode())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "#EMERG_ONLY: process_ph_sys_sel_pref_normal_cmd:"
                    "In emergency only mode");
    ds707_data_sess_handle_cmd_in_emerg_only(ds_cmd_ptr);
  }
  else if(
    cm_ph_event_info_ptr->as_id == SYS_MODEM_AS_ID_1 &&
    (curr_mode == SYS_SYS_MODE_CDMA || curr_mode == SYS_SYS_MODE_HDR)
   )
  {
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    if(ph_info_ptr->hybr_pref == CM_HYBR_PREF_OFF)
    {
      hybrid_pref = FALSE;
    }

    DATA_IS707_MSG1( MSG_LEGACY_HIGH, "hybrid_pref = %d", hybrid_pref);

    ds707_pkt_mgr_set_current_hybrid_pref(hybrid_pref);
#endif

    if (cm_ph_event_info_ptr->emerg_to_normal_mode == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "get ph_sys_sel_pref_normal, enable flow");
      ps_phys_link_enable_flow(ds707_pkt_get_pri_phys_link_ptr(),
                               DS_FLOW_707_LOCK_MASK);
    }
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_PH_EVENT_OPRT_MODE_CMD

DESCRIPTION   Process the CM_PH_EVENT_OPRT_MODE with LPM/OFFLINE/RESET/POWEROFF
              command obtained from CM

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_ph_event_oprt_mode_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds707_cback_event_info_type     event_info = {0};
  ps_phys_link_type              *phys_link_ptr   = NULL;
  phys_link_state_type            phys_link_state;
  sys_sys_mode_e_type             curr_mode = SYS_SYS_MODE_NONE;
  ds3g_cm_ph_ev_info_type        *cm_ph_event_ptr = NULL;
  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds_cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  cm_ph_event_ptr = (ds3g_cm_ph_ev_info_type *)ds_cmd_ptr->cmd_payload_ptr;

  curr_mode = ds3g_get_current_network_mode_ex (SYS_MODEM_AS_ID_1);
  if(ds707_data_sess_is_ds_centric_emerg_only_mode())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                   "#EMERG_ONLY: ds707_pkt_mgr_process_ph_event_oprt_mode_cmd:"
                   "In emergency only mode");
    ds707_data_sess_handle_cmd_in_emerg_only(ds_cmd_ptr);
  }

  else if(
    cm_ph_event_ptr->as_id == SYS_MODEM_AS_ID_1 &&
    (
#ifdef FEATURE_DATA_OPTHO
    (curr_mode == SYS_SYS_MODE_LTE) ||
#endif
    (curr_mode == SYS_SYS_MODE_CDMA) || (curr_mode == SYS_SYS_MODE_HDR)) &&
    cm_ph_event_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
    cm_ph_event_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE)
  {

    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Phone Operation Mode - LPM/POWROFF/RESET/OFFLINE");

    phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

    /* Get the phys link state */
    phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);
   
    DATA_IS707_MSG3(MSG_LEGACY_HIGH,
                    "Current Phys link state:%d CDSN:%d LDSN:%d ",
                    phys_link_state,
                    ds707_pkt_get_current_data_sess_network(),
                    ds707_pkt_get_last_data_sess_network());

    /* Abort data session, if there is a data activity */
    if ( phys_link_state != PHYS_LINK_NULL )
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      " Abort packet data call in oprt mode  ");
      //event_info.pkt_inst = DS707_DEF_PKT_INSTANCE;
      event_info.curr_network = ds707_pkt_get_current_data_sess_network();

      /*-----------------------------------------------------------------------
       Update last data sess network to what the network was when aborting 
       the call. It will be updated to NO_SRV while the call goes NULL. The 
       update to NO_SRV happens while processing the ABORT_SESSION Event.
      -----------------------------------------------------------------------*/
      ds707_pkt_set_last_data_sess_network(event_info.curr_network);
      event_info.tear_down_info.validity_mask |= 
  	                        PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      event_info.tear_down_info.data_info.call_end_reason = 
                                  PS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN;

  #ifdef FEATURE_DATA_OPTHO
      if ( ds707_s101_query_sm_state() != DS707_S101_SM_NON_TUNNEL )
      {
        ds707_cback_post_event(DS707_CBACK_TUN_ABORT_SESSION_EV, &event_info);
      }
      else
  #endif /* FEATURE_DATA_OPTHO */
      {
        ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);
      }
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                 "No data call active, skip abort event, phys link state: %d",
                 phys_link_state );
    }
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_GRACEFUL_PPP_TERMINATION

DESCRIPTION   Handles graceful PPP termination. Currently used in 
              LPM/PWROFF/Card reset events

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_graceful_ppp_termination
(
  ps_iface_net_down_reason_type end_reason
)
{
  uint8                   curr_iface_index = 0;
  ps_iface_type          *iface_ptr = NULL; 
  boolean                 iface_in_use = FALSE;
#ifdef FEATURE_EHRPD
  ds707_devmgr_stop_reason  devmgr_reason = DS707_DEVMGR_STOP_COMPLETE;
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_EPC_HANDOFF
  ps_iface_bearer_technology_type last_bearer_tech;
#endif
  ps_iface_tear_down_client_data_info_type tear_down_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&tear_down_info,0,sizeof(tear_down_info));
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Graceful PPP termination");
        
  if (PS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK != end_reason)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Reset last bearer tech");
    ds707_pkt_bearer_tech_reset();

#ifdef FEATURE_EPC_HANDOFF
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Report bearer reset to EPC");
    last_bearer_tech = ds707_get_last_bearer_tech();
    ds_epc_ehrpd_system(last_bearer_tech.data.cdma_type.so_mask,
                        last_bearer_tech.data.cdma_type.rat_mask);
#endif
  }
        
#ifdef FEATURE_EHRPD        
  /*-------------------------------------------------------------------------- 
    Disable partial context, so that devmgr just tears down the call instead 
    of maintaining partial context state (if enabled).
  ---------------------------------------------------------------------------*/
  ds707_devmgr_disable_partial_context();
#endif /* FEATURE_EHRPD */
  
  /*--------------------------------------------------------------------- 
    tear down all the UP ifaces
  ---------------------------------------------------------------------*/
  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES; 
        curr_iface_index++ )
  {      
    iface_ptr = NULL;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if ( PS_IFACE_IS_IN_USE(iface_ptr) )
    {
      DATA_IS707_MSG3( MSG_LEGACY_MED,
                       "processing gracefull PPP termination, Tearing down iface:0x%x::%d,"
                       "end reason: %d",
                       iface_ptr->name, iface_ptr->instance, end_reason);
      tear_down_info.validity_mask |= PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      tear_down_info.data_info.call_end_reason = end_reason;
      tear_down_info.validity_mask |= PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK;
      tear_down_info.data_info.local_abort = FALSE;
      (void)ds707_tear_down_cmd_processor(iface_ptr, (void *)(&tear_down_info), TRUE);
      iface_in_use = TRUE;
    }
  }
  
#ifdef FEATURE_EHRPD
  /*--------------------------------------------------------------------- 
    If no iface is UP, then stop devmgr.
  ---------------------------------------------------------------------*/
  if (FALSE == iface_in_use)
  {
    devmgr_reason = ds707_devmgr_stop();
    if (DS707_DEVMGR_STOP_FAIL == devmgr_reason)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_stop() failed");
    }
  }
#endif /* FEATURE_EHRPD */

  /*---------------------------------------------------------------
    If no ifaces are in use & devmgr already cleaned up, notify CM. 
  ---------------------------------------------------------------*/
  if ((iface_in_use == FALSE) 
#ifdef FEATURE_EHRPD
        && (DS707_DEVMGR_STOP_COMPLETE == devmgr_reason)
#endif /* FEATURE_EHRPD */
     )
  {
    if (PS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK == end_reason)
    {
      ds707_set_ppp_failure_flag(TRUE);
      /*----------------------------------------- 
      Inform HDR to switch personality to HRPD.
      -----------------------------------------*/
      hdrpac_set_data_ehrpd_capability(FALSE);
      ds707_pkt_mgr_clear_pdn_fb_count();
    }

    else
    {
      /*--------------------------------------------------------------------- 
        In case of CARD REFRESH/LPM/PWR OFF mode, Notify DSMGR which in turn
        will notify CM after all the other internal DS modules are done
        processing
      ---------------------------------------------------------------------*/
      if (ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress() == TRUE)
      {
        ds3geventmgr_clr_and_notify_lpm_pwr_off_status(
          DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK);
      }
      else if (ds3geventmgr_is_sub_not_avail_processing_in_progress(
                                                 SYS_MODEM_AS_ID_1) == TRUE)
      {
        ds3geventmgr_clr_and_notify_sub_not_avail_status(
          DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK,
          SYS_MODEM_AS_ID_1,
          ds3gi_sub_not_avail_subs_id_mask);
      }
    }
  }
  else
  {
    /*------------------------------------------------------------- 
       Set termination in progress flag.
     -------------------------------------------------------------*/
    ds707_set_term_in_progress(TRUE);
  }
} /* ds707_pkt_mgr_graceful_ppp_termination() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_POST_GRACEFUL_PPP_TERMINATION_CMD

DESCRIPTION   Handles graceful PPP termination asynchronously by posting msg 
              to DS task. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_post_graceful_ppp_termination_cmd
(
  ps_iface_net_down_reason_type end_reason
)
{
  ds_cmd_type *cmd_ptr = NULL;
  ps_iface_net_down_reason_type *net_down_reason_ptr = NULL;

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ps_iface_net_down_reason_type));
  /** Get command buffer to do task switch */
  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }
 
  net_down_reason_ptr = (ps_iface_net_down_reason_type* ) cmd_ptr->cmd_payload_ptr;


 /** Copy the values into the command buffer */
  *net_down_reason_ptr = end_reason;
  cmd_ptr->hdr.cmd_id = DS_CMD_707_POST_GRACEFUL_PPP_TERM;
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Posting DS_707_POST_TERM_PPP_CMD");

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_PWROFF_LPM_MODE_CMD

DESCRIPTION   Process the PWROFF / LPM mode notification from CM. Tear down 
              all active ifaces. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_pkt_mgr_process_pwroff_lpm_mode_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Process PWROFF/LPM cmd");
  ds707_pkt_mgr_graceful_ppp_termination(PS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN);

#ifdef FEATURE_EHRPD
  ds707_sys_chg_sr_process_lpm_mode();
#endif /*FEATURE_EHRPD*/

} /* ds707_pkt_mgr_process_pwroff_lpm_mode_cmd() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_CM_CARD_REFRESH_CMD

DESCRIPTION   Process the card refresh notification from CM. Tear down 
              all active ifaces. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_cm_card_refresh_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Process CM card refresh/Removed notification");
  /*-------------------------------------------------------------------------
   With the existing interface between CM and DS, we can't distinguish whether 
   the event is for Card removed or Card refresh. In future, if needed CM  team
   agreed to pass the enum type in the DS callback while notifying to DS.

   TODO Check if subs id in cmd payload needs to be used for DSDA 

  ---------------------------------------------------------------------------*/
  ds707_pkt_mgr_mmgsdi_reset();
} /* ds707_pkt_mgr_process_cm_card_refresh_cmd */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_NV_REFRESH_CMD

DESCRIPTION   Process the NV refresh notification from CM. Read all the NV/EFS 
              items as part of HotSwap

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_nv_refresh_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds707_pkt_mcfg_notify_cmd_type *notify_info_ptr=NULL;

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Process NV refresh notification");

  notify_info_ptr = 
  ((ds707_pkt_mcfg_notify_cmd_type*)(ds_cmd_ptr->cmd_payload_ptr));

  if( notify_info_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Invalid input. Skip NV refresh notification");
    return;
  }

  ds707_pkt_mgr_nv_refresh(notify_info_ptr->nv_refresh_info.subs_id);
} /* ds707_pkt_mgr_process_cm_card_refresh_cmd */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_CM_NOTIFICATION_CMD

DESCRIPTION   Process the notification commands from CM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_cm_notification_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds707_pkt_mgr_cm_notify_cmd_info_type* sub_cmd = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds_cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  sub_cmd = 
    (ds707_pkt_mgr_cm_notify_cmd_info_type*)(ds_cmd_ptr->cmd_payload_ptr);
  DATA_IS707_MSG1(MSG_LEGACY_MED, "CM notification sub_cmd %d", 
                  sub_cmd->cm_notify_type);

  switch (sub_cmd->cm_notify_type)
  {
    case DS707_PWROFF_LPM_MODE:
      ds707_pkt_mgr_process_pwroff_lpm_mode_cmd(ds_cmd_ptr);
      break;

    case DS707_SUB_NOT_AVAIL_NOTIFY:
      ds707_pkt_mgr_process_cm_card_refresh_cmd(ds_cmd_ptr);
      break;

    default:
      break;
  }
} /* ds707_pkt_mgr_process_cm_notification_cmd() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_DS3G_EVENT_CB

DESCRIPTION   Called by clients to register to get DS3G event.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_ds3g_event_cb
(
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type      *filter_info_ptr,
  void                          *event_info_ptr,
  void                          *data_ptr
)
{
  ds_cmd_type  *cmd_ptr = NULL;
  sys_sys_mode_e_type                    current_nw_mode = SYS_SYS_MODE_NONE; 
  ds707_pkt_mgr_cm_notify_cmd_info_type *cmd_info_ptr = NULL;
  ds707_TM_mgr_therm_mit_notify_cmd_info_type *cmd_info_ptr_tm = NULL;
  sys_modem_as_id_e_type                 subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( filter_info_ptr == NULL )
  { 
    DATA_IS707_MSG0(MSG_LEGACY_HIGH ,"Invalid input data: No Filter Info");
    return;
  }

  if(event_info_ptr == NULL && event_id == DS3GEVENTMGR_THERMAL_MITIGATION_EV)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH ,"Invalid input data: Event Info");
    return;
  }

  if((filter_info_ptr->ps_subs_id < PS_SYS_PRIMARY_SUBS || 
     filter_info_ptr->ps_subs_id > PS_SYS_TERTIARY_SUBS))
  { 
    DATA_IS707_MSG1(MSG_LEGACY_HIGH ,"Invalid input data, Subs ID: %d",
                    filter_info_ptr->ps_subs_id);
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm((ds_sys_subscription_enum_type)
                                         filter_info_ptr->ps_subs_id);

  DATA_IS707_MSG2(MSG_LEGACY_HIGH ,"Processing DS3G event %d on subs %d", 
                  event_id, subs_id);

  current_nw_mode = ds3g_get_current_network_mode_ex(subs_id);
  
  switch (event_id)
  {
    case DS3GEVENTMGR_MODE_LPM_EV:
      /*----------------------------------------------------------------------- 
        Set the Bit mask corresponding to 3GPP2 MH for LPM/Processing event.
        If the bit mask cannot be set, then there is a possibility that CM
        may shutdown the stack, before DS 3GPP2 MH completes its processing
        (in which case, graceful shutdown may not have completed)
      -----------------------------------------------------------------------*/
      if(ds3geventmgr_set_lpm_pwr_off_status(DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK) 
         == FALSE)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Cannot set lpm pwr off %d Bit msk",
                        DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK);
      }

      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_pkt_mgr_cm_notify_cmd_info_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_707_CM_NOTIFY;
      cmd_info_ptr = 
        (ds707_pkt_mgr_cm_notify_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
      cmd_info_ptr->cm_notify_type = DS707_PWROFF_LPM_MODE;
      cmd_info_ptr->subs_id = subs_id;

      ds_put_cmd( cmd_ptr );
      break;

    case DS3GEVENTMGR_SUB_NOT_AVAIL_EV:
      /*----------------------------------------------------------------------- 
        Set the Bit mask corresponding to 3GPP2 MH for SUB_NOT_AVAIL Processing
        event. If the bit mask cannot be set, then there is a possibility that
        CM may shutdown the stack, before DS 3GPP2 MH completes its processing
        (in which case, graceful shutdown may not have completed)
      -----------------------------------------------------------------------*/
      if(ds3geventmgr_set_sub_not_avail_status
                         (DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK,
                          subs_id) == FALSE)
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                        "Cannot set card refresh %d Bit msk on subs %d",
                        DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK,
                        subs_id);
      }

      current_nw_mode = ds3g_get_current_network_mode_ex(subs_id);

      if (current_nw_mode == SYS_SYS_MODE_CDMA || 
          current_nw_mode == SYS_SYS_MODE_HDR)
      {
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_pkt_mgr_cm_notify_cmd_info_type));
        if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
        {
          ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_707_CM_NOTIFY;
        cmd_info_ptr = 
          (ds707_pkt_mgr_cm_notify_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
        cmd_info_ptr->cm_notify_type = DS707_SUB_NOT_AVAIL_NOTIFY;
        cmd_info_ptr->subs_id = subs_id;

        ds_put_cmd( cmd_ptr );
      }
      else
      {
        ds3geventmgr_clr_and_notify_sub_not_avail_status(
           DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK,
           subs_id,
           ds3gi_sub_not_avail_subs_id_mask);

        DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Network mode %d is not a 3GPP2 mode.Not processing "
                        "SUB_NOT_AVAIL event for 3GPP2 MH on subs %d",
                        current_nw_mode,
                        subs_id);
      }
      break;

     case DS3GEVENTMGR_NV_REFRESH_EV:
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_pkt_mcfg_notify_cmd_type));
     
        if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
        {
          ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_707_MCFG_NOTIFY; 
      
        ((ds707_pkt_mcfg_notify_cmd_type *)(cmd_ptr->cmd_payload_ptr))->nv_refresh_info.subs_id
                                      = subs_id;
        ds_put_cmd( cmd_ptr );
      break;
   
    case DS3GEVENTMGR_THERMAL_MITIGATION_EV:
     /*----------------------------------------------------------------------- 
        Posts Thermal Mitigation command internally to 3GPP2 with the action 
        received.
        Processing Thermal Mitigation event for primary subs ID and ignoring 
        other Subs ID's. If other Subs Id is to be handled in future, subs id
        info should be added to payload.     

      -----------------------------------------------------------------------*/
      if (PS_SYS_PRIMARY_SUBS == filter_info_ptr->ps_subs_id)
      {
        cmd_ptr = ds_allocate_cmd_buf(
                  sizeof(ds707_TM_mgr_therm_mit_notify_cmd_info_type));
        if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
        {
          ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP2_TM_NOTIFY;
        cmd_info_ptr_tm = 
          (ds707_TM_mgr_therm_mit_notify_cmd_info_type*)
                                  cmd_ptr->cmd_payload_ptr;
        cmd_info_ptr_tm->action 
          = ((ds3geventmgr_event_info_type*)event_info_ptr)->
                                               thermal_mitigation_info.action;
  
        ds_put_cmd( cmd_ptr );  
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Subs ID: %d is not supported by "
                                         "3gpp2. Ignore call back and return.",
                                        filter_info_ptr->ps_subs_id);
      }
      break;
    default:
      break;
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_RETRY_DELAY_ORIG_CMD

DESCRIPTION   Process the retry delay orig command obtained from MC

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_pkt_mgr_process_retry_delay_orig_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds707_retry_delay_orig_type  *retry_delay_orig_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if ((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "process_retry_delay_orig_cmd: ds_cmd_ptr is NULL");
    return;
  }

  retry_delay_orig_ptr = (ds707_retry_delay_orig_type *)ds_cmd_ptr->cmd_payload_ptr;

  if (retry_delay_orig_ptr->delay_infinite == TRUE)
  {
    /* need to disable flow until MC sends clear cmd*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "retry delay infinite, flow is disabled");
    ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK);
  }
  else if (retry_delay_orig_ptr->delay != 0)
  {
    /*disable flow and start timer, after delay timer expires, enable flow*/
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "retry delay %d, flow disabled",
                     retry_delay_orig_ptr->delay);
    ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK );
    ds3gi_start_timer( DS3G_TIMER_RETRY_DELAY_ORIG,
                       retry_delay_orig_ptr->delay);
  }
  else
  {
    /* delay timer = 0 means MC cancel the retry delay, stop delay timer,
       enable flow*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "retry delay cleared, flow enabled");
    ds3gi_stop_timer(DS3G_TIMER_RETRY_DELAY_ORIG);
    ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_OUTAGE_NOTIFICATION

DESCRIPTION   Passes on the outage notification indication received from
              lower layer to PS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_outage_notification
(
uint16 time_to_outage,
uint16 duration_of_outage
)
{
  ps_iface_outage_notification_event_info_type outage_notification_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Fill the structure with outage info to be passed to PS
   */
  outage_notification_info.time_to_outage = time_to_outage;
  outage_notification_info.duration       = duration_of_outage;

  /* Send the notification to IPv4 and/or IPv6 if it exists
   */
  ds707_pkt_iface_generic_ind(IFACE_OUTAGE_NOTIFICATION_EV,
                              &outage_notification_info); //todo:msankar deprecate it?

  ds707_pkt_sys_event_ind(PS_SYS_EVENT_3GPP2_OUTAGE_NOTIFICATION,
                           &outage_notification_info,
                           PS_SYS_PRIMARY_SUBS);
}

/*===========================================================================
FUNCTION: DS707_PKT_MGR_GET_BEARER_TECH_INFO

DESCRIPTION:
  Send bearer techonology in the struct passed.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None
===========================================================================*/
boolean ds707_pkt_mgr_get_bearer_tech_info
(
  ps_iface_bearer_technology_type * bearer_tech
)
{

  byte                p_rev = 0;
  boolean             ret_val = FALSE;
  uint16              phys_layer_subtype;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* Fill in an invalid service option as default 
   */    
  bearer_tech->current_network = PS_IFACE_NETWORK_CDMA;
  bearer_tech->data.cdma_type.rat_mask = PS_IFACE_CDMA_NULL_BEARER;
  bearer_tech->data.cdma_type.so_mask = 0; // force to zero as invalid SO MASK

  /* If phone did not acquire signal or has lost signal send no service 
   * indication to the application
   */
  if (ds707_get_has_phone_acquired_signal() == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "Bearer Tech Info: Phone has not yet acquired signal");

    /* Returning True as we want to return that we are on a NULL Bearer */
    ret_val = TRUE;                     
    return ret_val;
  }

  /*  Depending on whether the current bearer is CDMA or HDR, fill the
   *  bearer_tech rat_mask and so_mask appropriately
   */
  if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)
  {
    bearer_tech->data.cdma_type.rat_mask = PS_IFACE_CDMA_1X;    

    /* Get the primary tc_state to obtain the service option from it */
    p_rev = mc_get_p_rev_in_use();

    if (p_rev <= 4)
    {
      bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_1X_IS95;
    }
    else if (p_rev >= 7)
    {
      bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_1X_IS2000_REL_A;
    }
    else
    {
      /* p_rev 5 is undefined and 6 is IS2000. 
         Default to IS2000 for both 5 and 6 */
      bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_1X_IS2000;
    }

    ret_val = TRUE;
  } /* The current bearer is 1x CDMA network */

  /* OPTHO_ToDo: For Optho case, we allow to query HDR for the details only if
     S101 is in tunnel allowed state. This is a conservative approach since this
     info is used by ds707_pkt_is_ehrpd_mode_of_operation() mostly to bring up a
     data call. It's possible that HDR might have a eHRPD tunnel session but S101
     is in tunnel disallowed state due to other reasons (tunnel mode !=1, lcp/virt
     conn throttled etc. Revisit this if necessary */
#ifdef FEATURE_HDR
  else if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR
#ifdef FEATURE_DATA_OPTHO
           || ds707_s101_query_sm_state() == DS707_S101_SM_TUNNEL_CALL_ALLOWED
#endif /* FEATURE_DATA_OPTHO */
          )
  {
if ( FALSE == hdrcp_smp_session_is_open(HDRHAI_PACKET_APPLICATION_CONTROLLER) ||
         FALSE == hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY)
       )
    {
      /*----------------------------------------------------------------------- 
       If HDR session is not open notify NULL bearer technology.
       By default bearer tech is set to NULL bearer.
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "HDR session not open.Notify Null bearer");
      ret_val = TRUE;
    }
    else
    {
      /* Is current protocol Rev 0, Rev A or Rev B */
      phys_layer_subtype = 
                 hdrcp_scp_get_current_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
  
      if ( phys_layer_subtype == HDRCP_SCP_DEFAULT_PHYS_LAYER )
      {
        bearer_tech->data.cdma_type.rat_mask = PS_IFACE_CDMA_EVDO_REV0;
      }
      #ifdef FEATURE_HDR_QOS
      else if ( phys_layer_subtype == HDRCP_SCP_SUB2_PHYS_LAYER )
      {
        bearer_tech->data.cdma_type.rat_mask = PS_IFACE_CDMA_EVDO_REVA;
      }
      #ifdef FEATURE_HDR_REVB
      else if ( phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
      {
        bearer_tech->data.cdma_type.rat_mask = PS_IFACE_CDMA_EVDO_REVB;
      }
      #endif /* FEATURE_HDR_REVB */
      #endif /* FEATURE_HDR_QOS */
  
      /* Now get the packet app information */
      if (hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
      {
        bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_EVDO_DPA;
      }
      #ifdef FEATURE_HDR_QOS
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN))
      {
        bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_EVDO_MFPA;
      }
      #ifdef FEATURE_HDR_EMPA
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
      {
        #ifdef FEATURE_EHRPD
        if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD )
        {
          bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_EVDO_EMPA_EHRPD;
        }
        /*----------------------------------------------------------------------- 
          the correct flow proto id is checked only in case of eHRPD. This is
          done because today extracting the flow proto id from DO CP is done
          only in case of eHRPD feature is enabled. Ideally even when eHRPD
          feature is not enabled this strict flow prot id validity check needs to
          happen. TODO.
        -----------------------------------------------------------------------*/
        else if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_UNKNOWN )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "flow prot id not valid, setting so_mask as 0" );
          bearer_tech->data.cdma_type.so_mask = 0;
        }
        else
        #endif /* FEATURE_EHRPD */
        {
          bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_EVDO_EMPA;
        }
      }
      #endif /* FEATURE_HDR_EMPA */
      #ifdef FEATURE_HDR_REVB
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {       
        /* Support for MMPA */
        #ifdef FEATURE_EHRPD
        if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD )
        {
          bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_EVDO_MMPA_EHRPD;
        }
        else if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_UNKNOWN )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "flow prot id not valid, setting so_mask as 0" );
          bearer_tech->data.cdma_type.so_mask = 0;
        }
        else
        #endif /* FEATURE_EHRPD */
        {
          bearer_tech->data.cdma_type.so_mask = PS_IFACE_CDMA_EVDO_MMPA;
        }
      }
      #endif /* FEATURE_HDR_REVB */
      #endif /* FEATURE_HDR_QOS */
  
      if ( ( bearer_tech->data.cdma_type.rat_mask != 0 ) || 
           ( bearer_tech->data.cdma_type.so_mask != 0 ) )
      {
        ret_val = TRUE;
      }
    }
  } /* else if (ds707_current_sys_mode == SYS_SYS_MODE_HDR) */
#endif /* FEATURE_HDR */

  /*------------------------------------------------------------------------- 
    If so_mask = 0, then SO mask was not. If there is no SO MASK, the bearer
    should be NULL. This covers the case where DO session is in transient
    period and HDRSCP query results default value, which is mapped as
    DO REV0.
  -------------------------------------------------------------------------*/
  if(bearer_tech->data.cdma_type.so_mask == 0)
  {
    DATA_IS707_MSG3( MSG_LEGACY_MED, 
                     "Resetting Old Rat Mask :%d to New Rat Mask :%d  so:%d", 
                     bearer_tech->data.cdma_type.rat_mask,
                     PS_IFACE_CDMA_NULL_BEARER,
                     bearer_tech->data.cdma_type.so_mask);
    bearer_tech->data.cdma_type.rat_mask = PS_IFACE_CDMA_NULL_BEARER;
  }

  return ret_val;                   

} /* ds707_pkt_mgr_get_bearer_tech_info() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_GET_HDR_SESS_INFO

DESCRIPTION   Util function to return HDR RAT and SO Mask based on lower 
              layer query info 

DEPENDENCIES  None

RETURN VALUE  TRUE  - On Success 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_mgr_get_hdr_sess_info
( 
  uint32* rat_mask, 
  uint32* so_mask
)
{
  uint16   phys_layer_subtype;
  boolean  ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    DSD will call this API everytime HDR is acquired. There are
    certain scenarios (e.g eHRPD->HRPD->LTE->eHRPD fallback scenario)
    where HDR negotiates a new session after it goes online. In those
    cases, DSD shouldn't proceed with the cached HDR session information
    which might not be the correct session that HDR is negotiating. So
    every time DSD gets HDR ACQ indication, it'll call mode handler and
    mode handler will query HDR directly and give the exact session state
    to DSD
  -------------------------------------------------------------------------*/
  
  if (NULL == rat_mask || NULL == so_mask)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pkt_mgr_get_hdr_sess_info: "
                     "rat_mask or so_mask passed in is NULL");
    return ret_val;
  }

  /* Fill in an invalid service option as default */
  *rat_mask = PS_IFACE_CDMA_NULL_BEARER;
  *so_mask  = 0;

  /*-------------------------------------------------------------------------
    Here we check for session validity with HDR. Even if the session is open;
    if HDR is going to close it since there is a change is ehrpd capability,
    it'll return FALSE here. Expectation is that if we get false here due to
    that reason, HDR will follow it up with a session close and session open
    (once new session gets negotiated)
  -------------------------------------------------------------------------*/
    if (FALSE == hdrcp_smp_session_is_valid(HDRHAI_PACKET_APPLICATION_CONTROLLER))
    {
      /*----------------------------------------------------------------------- 
     If HDR session is not open notify NULL bearer technology. We have
     already set the rat and so to null bearer initially. Just return TRUE
     from here
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "ds707_pkt_mgr_get_hdr_sess_info: "
                      "HDR session not open.Notify Null bearer");
      ret_val = TRUE;
    }
    else
    {
    /*----------------------------------------------------------------------- 
                                    RAT MASK
    -----------------------------------------------------------------------*/
      phys_layer_subtype = 
                 hdrcp_scp_get_current_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
  
      if ( phys_layer_subtype == HDRCP_SCP_DEFAULT_PHYS_LAYER )
      {
        *rat_mask = PS_IFACE_CDMA_EVDO_REV0;
      }
      #ifdef FEATURE_HDR_QOS
      else if ( phys_layer_subtype == HDRCP_SCP_SUB2_PHYS_LAYER )
      {
        *rat_mask = PS_IFACE_CDMA_EVDO_REVA;
      }
      #ifdef FEATURE_HDR_REVB
      else if ( phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
      {
        *rat_mask = PS_IFACE_CDMA_EVDO_REVB;
      }
      #endif /* FEATURE_HDR_REVB */
      #endif /* FEATURE_HDR_QOS */
  
    /*----------------------------------------------------------------------- 
                                    SO MASK
    -----------------------------------------------------------------------*/
      if (hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
      {
        *so_mask = PS_IFACE_CDMA_EVDO_DPA;
      }
      #ifdef FEATURE_HDR_QOS
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN))
      {
        *so_mask = PS_IFACE_CDMA_EVDO_MFPA;
      }
      #ifdef FEATURE_HDR_EMPA
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
      {
        #ifdef FEATURE_EHRPD
        if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD )
        {
          *so_mask = PS_IFACE_CDMA_EVDO_EMPA_EHRPD;
        }
        /*----------------------------------------------------------------------- 
          the correct flow proto id is checked only in case of eHRPD. This is
          done because today extracting the flow proto id from DO CP is done
          only in case of eHRPD feature is enabled. Ideally even when eHRPD
          feature is not enabled this strict flow prot id validity check needs to
          happen. TODO.
        -----------------------------------------------------------------------*/
        else if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_UNKNOWN )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "flow prot id not valid, setting so_mask as 0" );
          *so_mask = 0;
        }
        else
        #endif /* FEATURE_EHRPD */
        {
          *so_mask = PS_IFACE_CDMA_EVDO_EMPA;
        }
      }
      #endif /* FEATURE_HDR_EMPA */
      #ifdef FEATURE_HDR_REVB
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {       
        /* Support for MMPA */
        #ifdef FEATURE_EHRPD
        if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD )
        {
          *so_mask = PS_IFACE_CDMA_EVDO_MMPA_EHRPD;
        }
        else if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_UNKNOWN )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "flow prot id not valid, setting so_mask as 0" );
          *so_mask = 0;
        }
        else
        #endif /* FEATURE_EHRPD */
        {
          *so_mask = PS_IFACE_CDMA_EVDO_MMPA;
        }
      }
      #endif /* FEATURE_HDR_REVB */
      #endif /* FEATURE_HDR_QOS */
  
      if (( *rat_mask != 0 ) || ( *so_mask != 0 ))
      {
        ret_val = TRUE;
      }
    }

  return ret_val;                   

} /* ds707_pkt_mgr_get_hdr_sess_info */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION: DS707_PKT_MGR_GET_BEARER_TECH_INFO_EX

DESCRIPTION:
  Send bearer techonology in the struct passed.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None
===========================================================================*/
boolean ds707_pkt_mgr_get_bearer_tech_info_ex
(
  ds_sys_system_status_info_type * bearer_tech
)
{
  byte                p_rev = 0;
  boolean             ret_val = FALSE;
  uint16              phys_layer_subtype;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* Fill in an invalid service option as default 
   */    

  if( bearer_tech == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "bearer_tech ptr invalid");
    return FALSE;
  }

  memset(bearer_tech, 0, sizeof(ds_sys_system_status_info_type));
  bearer_tech->technology = DS_SYS_NETWORK_3GPP2;
  bearer_tech->rat_value = DS_SYS_RAT_EX_NULL_BEARER;
  bearer_tech->so_mask = 0;

  /* If phone did not acquire signal or has lost signal send no service 
   * indication to the application
   */
  if (ds707_get_has_phone_acquired_signal() == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "Bearer Tech Info: Phone has not yet acquired signal");

    /* Returning True as we want to return that we are on a NULL Bearer */
    ret_val = TRUE;                     
    return ret_val;
  }

  /*  Depending on whether the current bearer is CDMA or HDR, fill the
   *  bearer_tech rat_mask and so_mask appropriately
   */
  if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)
  {
    bearer_tech->rat_value = DS_SYS_RAT_EX_3GPP2_1X;    

    /* Get the primary tc_state to obtain the service option from it */
    p_rev = mc_get_p_rev_in_use();
    DATA_IS707_MSG1( MSG_LEGACY_MED, "in CDMA mode: p_rev in use: %d", p_rev);

    if (p_rev <= 4)
    {
      bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_1X_IS95;
    }
    else if (p_rev >= 7)
    {
      bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_1X_IS2000_REL_A;
    }
    else
    {
      /* p_rev 5 is undefined and 6 is IS2000. 
         Default to IS2000 for both 5 and 6 */
      bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_1X_IS2000;
    }

    ret_val = TRUE;
  } /* The current bearer is 1x CDMA network */

  /* OPTHO_ToDo: For Optho case, we allow to query HDR for the details only if
     S101 is in tunnel allowed state. This is a conservative approach since this
     info is used by ds707_pkt_is_ehrpd_mode_of_operation() mostly to bring up a
     data call. It's possible that HDR might have a eHRPD tunnel session but S101
     is in tunnel disallowed state due to other reasons (tunnel mode !=1, lcp/virt
     conn throttled etc. Revisit this if necessary */
#ifdef FEATURE_HDR
  else if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR
#ifdef FEATURE_DATA_OPTHO
           || ds707_s101_query_sm_state() == DS707_S101_SM_TUNNEL_CALL_ALLOWED
#endif /* FEATURE_DATA_OPTHO */
          )
  {
    if ( FALSE == hdrcp_smp_session_is_open(HDRHAI_PACKET_APPLICATION_CONTROLLER) ||
         FALSE == hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY)
       )
    {
      /*----------------------------------------------------------------------- 
       If HDR session is not open notify NULL bearer technology.
       By default bearer tech is set to NULL bearer.
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "HDR session not open.Notify Null bearer");
      ret_val = TRUE;
    }
    else
    {
      /* Is current protocol Rev 0, Rev A or Rev B */
      phys_layer_subtype = 
                 hdrcp_scp_get_current_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
  
      DATA_IS707_MSG1(MSG_LEGACY_MED, "in HDR mode: Phys layer subtype is %x", 
                      phys_layer_subtype);
  
      /* Now get the packet app information */
      if (hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
      {
        if ( phys_layer_subtype == HDRCP_SCP_DEFAULT_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REV0_DPA;
        }
        #ifdef FEATURE_HDR_QOS
        else if ( phys_layer_subtype == HDRCP_SCP_SUB2_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVA_DPA;
        }
        #ifdef FEATURE_HDR_REVB
        else if ( phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVB_DPA;
        }
        #endif /* FEATURE_HDR_REVB */
        #endif /* FEATURE_HDR_QOS */
      }

      #ifdef FEATURE_HDR_QOS
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN))
      {
        if ( phys_layer_subtype == HDRCP_SCP_SUB2_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVA_MPA;
        }
        #ifdef FEATURE_HDR_REVB
        else if ( phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVB_MPA;
        }
        #endif /* FEATURE_HDR_REVB */
      }

      #ifdef FEATURE_HDR_EMPA
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
      {
        if ( phys_layer_subtype == HDRCP_SCP_SUB2_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA;
        }
        #ifdef FEATURE_HDR_REVB
        else if ( phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA;
        }
        #endif /* FEATURE_HDR_REVB */
      }
      #endif /* FEATURE_HDR_EMPA */

      #ifdef FEATURE_HDR_REVB
      else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      { 
        if ( phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
        {
          bearer_tech->so_mask = DS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA;
        }      
      }
      #endif /* FEATURE_HDR_REVB */
      #endif /* FEATURE_HDR_QOS */

      #ifdef FEATURE_EHRPD
      /*------------------------------------------------------------------------- 
        Check for eHRPD mode, if the stream assigned is either EMPA or MMPA
      -------------------------------------------------------------------------*/
      if ( bearer_tech->so_mask == DS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA ||
           bearer_tech->so_mask == DS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA ||
           bearer_tech->so_mask == DS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA 
         )
      {
        if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD )
        {
          bearer_tech->rat_value = DS_SYS_RAT_EX_3GPP2_EHRPD;
        }
        else if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_UNKNOWN )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "ehrpd mode unknown, return null bearer" );
  	 	           bearer_tech->rat_value = DS_SYS_RAT_EX_NULL_BEARER;
          bearer_tech->so_mask = 0;
        }
        else
        {
          bearer_tech->rat_value = DS_SYS_RAT_EX_3GPP2_HRPD;
        }
      }
      else
      #endif /* FEATURE_EHRPD */
      {
        bearer_tech->rat_value = DS_SYS_RAT_EX_3GPP2_HRPD;
      }
  
      if ( ( bearer_tech->rat_value != 0 ) || 
           ( bearer_tech->so_mask != 0 ) )
      {
        ret_val = TRUE;
      }
    }
  } /* else if (ds707_current_sys_mode == SYS_SYS_MODE_HDR) */
#endif /* FEATURE_HDR */

  /*------------------------------------------------------------------------- 
    If so_mask = 0, then SO mask was not. If there is no SO MASK, the bearer
    should be NULL. This covers the case where DO session is in transient
    period and HDRSCP query results default value, which is mapped as
    DO REV0.
  -------------------------------------------------------------------------*/
  if(bearer_tech->so_mask == 0)
  {
    DATA_IS707_MSG3( MSG_LEGACY_MED, "bearer_tech_info_ex: "
                     "Resetting Old Rat Mask :%d to New Rat Mask :%d  so:%d",
                     bearer_tech->rat_value,
                     DS_SYS_RAT_EX_NULL_BEARER,
                     bearer_tech->so_mask);
    bearer_tech->rat_value = DS_SYS_RAT_EX_NULL_BEARER;
  }

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "Bearer Tech info Ex: tech:%d rat:%d so:%d", 
                   bearer_tech->technology,
                   bearer_tech->rat_value,
                   bearer_tech->so_mask );

  return ret_val;                   

} /* ds707_pkt_mgr_get_bearer_tech_info_ex() */
#endif

/*===========================================================================
FUNCTION      DS707_PKT_IS_EHRPD_MODE_OF_OPERATION

DESCRIPTION   Utility function to see if the current serving system is eRHPD

DEPENDENCIES  None

RETURN VALUE  TRUE  - if serving system is eHRPD
              FALSE - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_is_ehrpd_mode_of_operation(void)
{
  boolean ret_val;
  ps_iface_bearer_technology_type   bearer_tech;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  if (ds707_is_pkt_classifier_enabled()) 
  {
    return ds707_pkt_classifier_is_ehrpd_mode();
  }
#endif

  memset( &bearer_tech, 
          0, 
          sizeof(ps_iface_bearer_technology_type));

  (void) ds707_pkt_mgr_get_bearer_tech_info(&bearer_tech);
  if(( (bearer_tech.data.cdma_type.rat_mask == PS_IFACE_CDMA_EVDO_REVA) && 
       (bearer_tech.data.cdma_type.so_mask == PS_IFACE_CDMA_EVDO_EMPA_EHRPD) ) || 
     ( (bearer_tech.data.cdma_type.rat_mask == PS_IFACE_CDMA_EVDO_REVB) && 
       (bearer_tech.data.cdma_type.so_mask == PS_IFACE_CDMA_EVDO_MMPA_EHRPD) )  )
  {
    ret_val =  TRUE;
  }
  else
  {
    ret_val = FALSE;
  }
  return ret_val;
}/* ds707_pkt_is_ehrpd_mode_of_operation() */


/*===========================================================================
FUNCTION      DS707_PKT_BEARER_TECH_CHANGED_NOTIFICATION

DESCRIPTION   Gives a notification to APPS (through PS) of the bearer tech 
              changed event

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_bearer_tech_changed_notification()
{
  ps_iface_ioctl_bearer_tech_changed_type  bearer_tech_change;
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_sys_system_status_info_type bearer_tech_ex;
#endif

  /*-----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_OPTHO
  if (  ds707_s101_query_sm_state() != DS707_S101_SM_NON_TUNNEL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Bearer Tech Chngd Notif: S101 not in Non Tunnel State. "
                     "Ignoring bearer tech change");
    return;
  }
#endif /* FEATURE_DATA_OPTHO */


   memset( &bearer_tech_change,
           0,
           sizeof(ps_iface_ioctl_bearer_tech_changed_type));

#ifdef FEATURE_DATA_WLAN_MAPCON
   memset(&bearer_tech_ex, 0, sizeof(ds_sys_system_status_info_type));
#endif

  /* Fill in old bearer tech details from previously stored value
   */
  bearer_tech_change.old_bearer_tech = ds707_get_last_bearer_tech(); 

  /* Only if we are successful in getting new bearer tech information */
  if (  ds707_pkt_mgr_get_bearer_tech_info(&(bearer_tech_change.new_bearer_tech)) == TRUE ) 
  {

    /* Generate the event only if something has changed */
    if(0 != memcmp( &bearer_tech_change.old_bearer_tech,
                    &bearer_tech_change.new_bearer_tech,
                    sizeof(ps_iface_bearer_technology_type)))
    {
      /* post the bearer change event */
      ds707_pkt_iface_generic_ind(IFACE_BEARER_TECH_CHANGED_EV,
                                  (void *)&bearer_tech_change);
    
      /* Update the last bearer tech with the current bearer tech */
      ds707_set_last_bearer_tech(bearer_tech_change.new_bearer_tech);

#ifdef FEATURE_DATA_WLAN_MAPCON
      if( ds707_pkt_mgr_get_bearer_tech_info_ex(&bearer_tech_ex) == TRUE )
      {
        /* post the bearer change event in the new format */
        ds707_pkt_iface_generic_ind( IFACE_BEARER_TECH_EX_CHANGED_EV,
                                     (void *)&bearer_tech_ex);
      }
#endif
    }
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_BEARER_TECH_CHANGED_NOTIFICATION

DESCRIPTION   Gives a notification to APPS (through PS) of the bearer tech 
              changed event

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_bearer_tech_reset()
{
  memset( &ds707_last_bearer_tech,
           0,
           sizeof(ps_iface_bearer_technology_type) );
  /* Fill in default values */
  ds707_last_bearer_tech.current_network = PS_IFACE_NETWORK_CDMA;
  ds707_last_bearer_tech.data.cdma_type.rat_mask = PS_IFACE_CDMA_NULL_BEARER;
}


/*===========================================================================
FUNCTION      DS707_PKT_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this
              mode handler.

              There is no featurization of commands here.  Functionality
              should be FEATURE'ized elsewhere, so that these commands are
              generated appopriately.  For example, the SID module only
              generates its command if the SID FEATURE's is defined.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_pkt_process_cmd
(
ds_cmd_type   *ds_cmd_ptr
)
{
  ds707_mip_profile_type     *ds_static_profile_ptr = NULL;
  uint8                       temp_val = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ds_cmd_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Serious error ds_cmd_ptr is NULL:"
                     "ds707_pkt_process_cmd");
    return;
  }

  switch (ds_cmd_ptr->hdr.cmd_id)
  {
#ifdef FEATURE_HDR
    case DS_CMD_707_PKT_IDM_CHANGE:
#ifdef FEATURE_DS_LN_UPDATE
    case DS_CMD_707_HDR_LN_STATUS: 
#endif /*FEATURE_DS_LN_UPDATE*/
    case DS_CMD_707_INITIATE_ALN:
#ifdef FEATURE_DS_CAM
    case DS_CMD_707_COLOC_INFO_CHANGED:
#endif /* FEATURE_DS_CAM */
      ds707_pkt_hdr_process_cmd(ds_cmd_ptr);
      break;
#endif /* FEATURE_HDR */

    case DS_CMD_707_PKT_CTA_TIMER_EXPIRED:
      event_report(EVENT_INACTIVITY_TIMER_EXPIRED);
      ds707_pkti_cta_timer_expired();
      break;

    case DS_CMD_707_PKT_HOLDDOWN_TIMER_EXPIRED:
      ds707_dorm_timer_process_cmd(ds_cmd_ptr);
      break;

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    case DS_CMD_707_PKT_AUTO_REDIAL_TIMER_EXPIRED:
      ds707_pkt_auto_redial_process_cmd(ds_cmd_ptr);
      break;
#endif
    
    case DS_CMD_707_PKT_PHYS_LINK_UP:
    case DS_CMD_707_PKT_PHYS_LINK_DOWN:
      ds707_pkt_phys_link_process_cmd(ds_cmd_ptr);
      break;

    case DS_CMD_707_TOGGLE_QNC_ENABLE:
      ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
      if( ds_static_profile_ptr == NULL )
      {
        DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
        return;
      }

      temp_val = !(ds_static_profile_ptr->ds707.ds707_qcqnc_val);
      (void) ds707_pkt_mgr_set_1x_profile_val(DS707_QCQNC_VAL,
                                              (void *)&temp_val,
	  	            0);
      ds707_pkt_mgr_nv_sync();
      ds707_so_pkt_recal();
      break;

    case DS_CMD_707_ABORT_DORMANT_PPP_TIMER_EXPIRED:
      ds707_gonulltmri_process_cmd(ds_cmd_ptr);
      break;

    case DS_CMD_707_EXIT_TC:
      /* Call the cmd handler in EPZID Module*/
      ds707_epzid_mgr_process_cmd(ds_cmd_ptr);
      ds707_pkt_mgr_process_exit_tc_cmd();
      break;

    case DS_CMD_707_UM_TX_HI_WM:
      /* call the high watermark handler for tx */
      ds707_wmk_process_um_tx_hi_wm(ds_cmd_ptr);
      break;

    case DS_CMD_707_UM_TX_LO_WM:
      /* call the low watermark handler for tx */
      ds707_wmk_process_um_tx_lo_wm(ds_cmd_ptr);
      break;

    case DS_CMD_707_RETRY_DELAY_ORIG:
      ds707_pkt_mgr_process_retry_delay_orig_cmd(ds_cmd_ptr);
      break;

    case DS_CMD_707_PKT_SYS_CHG:
      ds707_pkt_mgr_process_sys_chg_cmd(ds_cmd_ptr);
      break;

    case DS_CMD_707_UM_RX_HI_WM:
      /* call the high watermark handler for tx */
      ds707_wmk_process_um_rx_hi_wm(ds_cmd_ptr);
      break;

    case DS_CMD_707_UM_RX_LO_WM:
      /* call the low watermark handler for tx */
      ds707_wmk_process_um_rx_lo_wm(ds_cmd_ptr);
      break;

    case DS_CMD_707_NEW_CALL_ATTEMPT_TIMER:
      ds707_sys_chg_process_cmd(ds_cmd_ptr);
      break;  
    
#ifdef FEATURE_EHRPD
    case DS_CMD_707_PDN_INACTIVITY_TIMER_CMD:
      ds707_pdncntx_inactivity_timer_process_cmd(ds_cmd_ptr);
      break;  

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
    case DS_CMD_707_LSD_TIMEOUT:
      ds_3gpp2_lsd_timer_process_cmd(ds_cmd_ptr);
      break;  
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

#ifdef FEATURE_DATA_3GPP2_VS
    case DS_CMD_707_MPIT_TIMEOUT:
      ds_3gpp2_mpit_timer_process_cmd(ds_cmd_ptr);
      break;  
#endif /* FEATURE_DATA_3GPP2_VS */
#endif /* FEATURE_EHRPD */

    case DS_CMD_707_CM_NOTIFY:
      ds707_pkt_mgr_process_cm_notification_cmd(ds_cmd_ptr);
      break;

    case DS_CMD_3GPP2_TM_NOTIFY:
      ds707_tm_mgr_process_therm_mit_cmd(ds_cmd_ptr);
      break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   case DS_CMD_707_MCFG_NOTIFY:
    ds707_pkt_mgr_process_nv_refresh_cmd(ds_cmd_ptr);
    break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   case DS_CMD_707_MANDATORY_APN_UPDATE:
      ds707_data_session_process_update_min_apn_hdlr(ds_cmd_ptr);
      break;

    case DS_CMD_707_ROAM_CHG:
      ds707_pkt_mgr_process_roam_chg_cmd(ds_cmd_ptr);
      break;

#ifdef FEATURE_EHRPD
     case DS_CMD_707_POST_GRACEFUL_PPP_TERM:
       if (ds_cmd_ptr->cmd_payload_ptr != NULL)
       {
         ds707_pkt_mgr_graceful_ppp_termination
        ( *((ps_iface_net_down_reason_type* ) ds_cmd_ptr->cmd_payload_ptr) );
       }
 
      break;
#endif /* FEATURE_EHRPD */

    case DS_CMD_707_THROTTLE_INFO_UPDATE:
      ds707_update_call_throttle_info(ds_cmd_ptr);
      break;
    case DS_CMD_707_WAIT_FOR_EMERG_CALL_TIMEOUT:
      ds707_data_sess_handle_cmd_in_emerg_only(ds_cmd_ptr);
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown cmd to ds707_pkt %d",
                      ds_cmd_ptr->hdr.cmd_id);
      break;
  }
} /* ds707_pkt_process_cmd() */

/*===========================================================================
FUNCTION      DS707_PKT_PROCESS_SIGNALS

DESCRIPTION   Processes sigals set by other parts of the system.  Currently,
              no signals should be set for this mode handler.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_process_signals
(
rex_sigs_type   sigs
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (sigs & DS_PKT_REORIG_SIG)
  {
    ds707_pkti_reorig_sig_hdlr();
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"unknown signal to ds707_pkt 0x%x", sigs);
    DS_FORCE_ASSERT();
  }
} /* ds707_pkt_process_signals() */


/*===========================================================================
FUNCTION      DS707_PKT_CM_INIT

DESCRIPTION   Called once at startup.  Registers with CM for things specific
              to IS-707.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_cm_init
(
cm_client_id_type        cm_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_USED(cm_id);

#if defined(FEATURE_DATA_IS2000_SDB) || defined(FEATURE_HDR_DOS)
  /*-------------------------------------------------------------------------
    Register with CM for getting following SDB information:
    Mobile Terminated messages,
    Status of Mobile Originated Messages (to convey to application).

       client:           Requesting client i.e. DS
      *dbm_event_func:   Pointer to a callback function to notify the client
                         of dbm events
       event_reg_type:   Is this a registration or de-reg. DS registers once
                         and never deregisters.cm_dbm_event_e_type
       from_dbm_event:   register from this event (including). So any events
                         in the enum type from CM_DBM_EVENT_MO_STATUS to
                         to_dbm_event will be informed to DS.
       to_dbm_event:     To this event (including). CM_DBM_EVENT_MT_MSG
      *dbm_cmd_err_func: Pointer to a callback function to notify the
                         client of dbm command errors of OTHER clients.
                         DS currently does not care. So, choose NULL.

  -------------------------------------------------------------------------*/
  if (cm_client_dbm_event_reg(cm_id,
                              ds707_sdb_DBM_event_handler,
                              CM_CLIENT_EVENT_REG,
                              CM_DBM_EVENT_MO_STATUS,
                              CM_DBM_EVENT_MT_MSG,
                              NULL) != CM_CLIENT_OK)
  {
    ERR_FATAL("Could not reg with CM",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Register with CM for getting following SDB information:
  -------------------------------------------------------------------------*/
  if (cm_client_dbm_class_reg(cm_id,
                              CM_CLIENT_EVENT_REG,
                              CM_DBM_CLASS_SDB,
                              CM_DBM_CLASS_SDB
                             ) != CM_CLIENT_OK)
  {
    ERR_FATAL("Could not reg with CM",0,0,0);
  }

#endif /* FEATURE_DATA_IS2000_SDB || FEATURE_HDR_DOS */

}/* ds707_pkt_cm_init() */

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*===========================================================================
FUNCTION      DS707_PKT_MGR_HYBR_PREF_INIT

DESCRIPTION   This function initializes Hybr pref mode to ON.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_pkt_mgr_hybr_pref_init
(
  void
)
{
  ds707_cm_hybrid_pref = TRUE;
}
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

#ifdef FEATURE_HDRRLP_LOGGING
/*===========================================================================
FUNCTION      DS707_PKT_MGR_LOG

DESCRIPTION   Logs memory information.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_pkt_mgr_log
(
  void
)
{
  ds707_ccm_log_memory(1);
}
#endif /*FEATURE_HDRRLP_LOGGING*/
/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT_CRIT_SECT

DESCRIPTION   Called once at mobile power-up.  Initializes ds707 global crit 
              section variables
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_init_crit_sect
(
  void
)
{
  DS_INIT_CRIT_SECTION( &ds707_global_crit_sect );
  DS_INIT_CRIT_SECTION( &ds707_chs_global_crit_sect );
  DS_INIT_CRIT_SECTION( &ds707_scrm_global_crit_sect );

  /*-------------------------------------------------------------------------
     Initialize Data Session Profile Critical Section
  -------------------------------------------------------------------------*/
  ds707_data_session_init_crit_sect();

} /* ds707_pkt_mgr_init_crit_sect() */

/*===========================================================================
FUNCTION      DS707_JCDMA_MODE_GONULLTMR_INIT

DESCRIPTION   Called once at mobile power-up to init JCDMA mode and go null 
              timer. 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_mode_gonulltmr_init
(
  void
)
{
  /*------------------------------------------------------------------------- 
    Read the NV item for full JCDMA mode
  -------------------------------------------------------------------------*/
  ds707_roaming_read_jcdma_nv();
  /*-------------------------------------------------------------------------
    Read the NV item for simplified JCDMA mode
  -------------------------------------------------------------------------*/
  ds707_roaming_read_simplified_jcdma_nv();

  if( (ds707_roaming_is_curr_mode_jcdma() == TRUE) ||
      (ds707_roaming_is_simplified_jcdma_mode() == TRUE) )
  {
    ds707_gonulltmri_init();
  }
}/* ds707_jcdma_mode_gonulltmr_init */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_POWERUP_INIT

DESCRIPTION   Called once at mobile power-up to init necessary data 
              structures 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_powerup_init
(
  void
)
{
  boolean                       *ppp_resync_ptr;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Initialize 3GPP2 MH cmd hdlr
  -------------------------------------------------------------------------*/
  ds707_cmd_hdlr_init();

  /*-------------------------------------------------------------------------
    Initialize the 707 event callbacks.  This must be the first module
    initialized, as the following init's will register callbacks with this
    module.
  -------------------------------------------------------------------------*/
  ds707_cback_init();

  ds707_pkti_register_for_events();

  /*-------------------------------------------------------------------------
    Initialize dormant timer
  -------------------------------------------------------------------------*/
  ds707_dorm_timer_init();

  /*-------------------------------------------------------------------------
    Initialize the CTA timer module.
  -------------------------------------------------------------------------*/
  ds707_cta_init();

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  /*-------------------------------------------------------------------------
    Initialize auto reidal timer
  -------------------------------------------------------------------------*/
  ds707_pkt_auto_redial_init();
#endif

  /*-------------------------------------------------------------------------
    Initialize RLP's.
  -------------------------------------------------------------------------*/
  rlp_init();

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  ds707_pkt_mgr_hybr_pref_init();
#endif

  ds707_so_retrydel_init();

  /*-------------------------------------------------------------------------
     Initialize Timers and Watermarks
  -------------------------------------------------------------------------*/
  ds707_wmk_init();

  ds707_timer_init();

  /*-------------------------------------------------------------------------
     Initialize PPP_resync flag and resync_lock
  -------------------------------------------------------------------------*/
  ppp_resync_ptr    = ds707_get_ppp_resync_flag_ptr();
  *ppp_resync_ptr   = FALSE;

  ds707_pkt_mgr_ppp_resync_lock_release_cb = NULL;

#ifdef FEATURE_DATA_3GPP2_VS
  /* Power up the DS 3GPP2 module for handling the vendor specific extensions
   * to PPP packets. 
   */
  ds_3gpp2_vendor_powerup_init();  
#endif /* FEATURE_DATA_3GPP2_VS */

  /*-------------------------------------------------------------------------
    Initialize last bearer tech to indicate NO SRV.
  -------------------------------------------------------------------------*/
  ds707_pkt_bearer_tech_reset();

#ifdef FEATURE_EHRPD
  ds_ehrpd_pdn_mgr_powerup_init();
#endif

#ifdef FEATURE_MMGSDI_SESSION_LIB
  ds707_jcdma_card_info = MMGSDI_JCDMA_NON_JCDMA_CARD;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifdef FEATURE_DATA_OPTHO
  /*------------------------------------------------------------------------- 
    Initialize S101 State Machine
  -------------------------------------------------------------------------*/
  ds707_s101_sm_init();
#endif /* FEATURE_DATA_OPTHO */

} /* ds707_pkt_mgr_powerup_init */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              functionality with 3G DSMGR.

DEPENDENCIES  None.

RETURN VALUE  Rex signals this mode specific handler is interested in.

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type ds707_pkt_mgr_init(void)
{
  uint8                      i = 0;
  ps_iface_type             *iface_ptr;      /* ptr to IPV4 1x iface     */
  dssnet4_sm_cb_type        *dssnet4_sm_ptr;
                              /* ptr to IPV4 data session info structure */
  rex_timer_type  *conn_delay_timer; 
                              /* ptr to connection release delay timer */

#ifdef FEATURE_DATA_PS_IPV6
  ip6_sm_type               *ip6_sm_ptr     = NULL;
  dssnet6_sm_cb_type        *dssnet6_sm_ptr = NULL;
  ip6_sm_config_type        *sm_config_ptr  = NULL;


#if 0
/*lint -save -e64 access of failover_mode wrapped under e64 so declaration 
   too*/
  ps_iface_ip_ver_failover_e_type failover_mode = IPV4_ONLY;
/*lint -restore */
  dssnetAny_sm_cb_type      *dssnetAny_sm_ptr;
#endif
#endif /* FEATURE_DATA_PS_IPV6 */
  ds707_tc_state_type       *tc_state_ptr;
  acl_type                  *my_acl_ptr;       /* ptr to acl               */

#ifdef FEATURE_HDR_QOS  
  boolean                    dqos_supported = FALSE; /* Is D-QoS Supported */
#endif

  ds707_iface_table_type*       ds707_iface_tbl_ptr = NULL;
  ds707_mip_profile_type*    ds_1x_runtime_profile_ptr = NULL;

  nv_item_type               ds707_nv_item;
  nv_stat_enum_type			 status;
  ds3geventmgr_filter_type   filter_info;

/*-------------------------------------------------------------------------
  Array of IFACE IDs to uniquely identify logging of a particular IFACE
  This array size should match the number of PDN Instances
-------------------------------------------------------------------------*/
dpl_iid_ifname_enum_type iface_name[DS707_MAX_IFACES] = {
                                        DPL_IID_IFNAME_CDMA_SN_IFACE_1,
                                        DPL_IID_IFNAME_CDMA_SN_IFACE_2
#ifdef FEATURE_EHRPD
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_3
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_4
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_5
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_6
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_7
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_8
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_9
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_10
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_11
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_12
#ifndef FEATURE_DATA_3GPP2_ULT
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_13
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_14
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_15
                                        ,DPL_IID_IFNAME_CDMA_SN_IFACE_16
#endif /*FEATURE_DATA_3GPP2_ULT*/
#endif
                                      };

char *iface_desc_name[DS707_MAX_IFACES] = {
                                        "CDMA SN IFACE:1",
                                        "CDMA SN IFACE:2"
#ifdef FEATURE_EHRPD
                                        ,"CDMA SN IFACE:3"
                                        ,"CDMA SN IFACE:4"
                                        ,"CDMA SN IFACE:5"
                                        ,"CDMA SN IFACE:6"
                                        ,"CDMA SN IFACE:7"
                                        ,"CDMA SN IFACE:8"
                                        ,"CDMA SN IFACE:9"
                                        ,"CDMA SN IFACE:10"
                                        ,"CDMA SN IFACE:11"
                                        ,"CDMA SN IFACE:12"
#ifndef FEATURE_DATA_3GPP2_ULT
                                        ,"CDMA SN IFACE:13"
                                        ,"CDMA SN IFACE:14"
                                        ,"CDMA SN IFACE:15"
                                        ,"CDMA SN IFACE:16"
#endif /*FEATURE_DATA_3GPP2_ULT*/
#endif
                                      }; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------- 
    Register handlers with 3G DSMGR
  -------------------------------------------------------------------------*/
  ds707_pkt_mgr_hdlr_init();

  /*-------------------------------------------------------------------------
    Initialize EPZID (Inherently initializes for SID/NID/PZID)
    Registers cbacks based on JCDMA value read above.
  -------------------------------------------------------------------------*/
  ds707_epzid_mgr_init();

  /*-------------------------------------------------------------------------
    Initialize hi speed clock.
  -------------------------------------------------------------------------*/
  ds707_hi_spd_clk_init();

#ifdef FEATURE_HDR_EMPA
  /*-------------------------------------------------------------------------
    Initialize RSP
  -------------------------------------------------------------------------*/
  hdrrsp_startup();
#endif /* FEATURE_HDR_EMPA */

  /*-------------------------------------------------------------------------
    Start RLP's.
  -------------------------------------------------------------------------*/
  dsrlp_startup();

  /*-------------------------------------------------------------------------
     Register resource request modules (SCRM & Control/Hold) with MC/CM.
  -------------------------------------------------------------------------*/
  ds707_rrm_reg_funcs();
  
  ds707_tm_mgr_powerup_init();

  ds707_nv_util_qcmip_set_crm_val();
  ds707_nv_util_qcmip_set_mdr_val();
  /*-------------------------------------------------------------------------
     Set up the various service options with MC/CM.  These implicitly do the
     JCDMA registrations (if that is enabled).
  -------------------------------------------------------------------------*/
  ds707_nv_util_so_pkt_init();

  /*-------------------------------------------------------------------------
    Initialize M51 and apprate JCDMA modules.  Must be done after SO's have
    been initialized.
  -------------------------------------------------------------------------*/
   ds707_nv_util_ds707_so_jcdma_init();
   ds707_nv_util_read_jcdma_m51_init();
   ds707_nv_util_jcdma_apprate_init();

    /*-------------------------------------------------------------------------
    Disable DO->1x handdown if  JCDMA  or simplified JCDMA NV is enabled
    -------------------------------------------------------------------------*/
  ds707_nv_util_hdrpac_disable_1x_handdown_mode();

  /*-------------------------------------------------------------------------
     Register SO 33 initializers with MC/CM.  Must happen after the
     SO's are initialized.
  -------------------------------------------------------------------------*/
  ds707_so33_reg_initializers();

  /*------------------------------------------------------------------------
    Initialize dsgen iface hdlr   
   ------------------------------------------------------------------------*/
  ds707_nv_util_read_internal_auth_nv();

  /*-------------------------------------------------------------------------
     Set CTA timer to not be active.
  -------------------------------------------------------------------------*/
  ds707_cta_reset_timer(DS707_CTA_STOP_TIMER, DS707_INVALID_SO);

  ds707_nv_util_read_drs_nv();

  /*-------------------------------------------------------------------------
    Register query functions with DIAG.
  -------------------------------------------------------------------------*/
  ds707_diag_init();
#ifdef FEATURE_DS_MOBILE_IP
  dsmip_stats_init();
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Set the current 707 network (1X or HDR) the data session is on to NONE
  -------------------------------------------------------------------------*/
  ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_NO_SRV);
  ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_NO_SRV);

  ds707_extif_inform_hdr_sess_status();

#ifdef FEATURE_HDR_QOS
  /*-------------------------------------------------------------------------
  Initialize the sec pkt mgr. This should happen before the rest of the
  pkt mgr initialization
  -------------------------------------------------------------------------*/
  ds707_sec_pkt_init();
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
  ds707_nv_util_read_ehrpd_to_hrpd_fb_nv();
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

  ds707_nv_util_read_dns_v4_addr_req_nv();
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Read failover NV settings for FAILOVER behavior.
  -------------------------------------------------------------------------*/
  ds707_nv_util_set_failover_ip_mode();
  
  ds707_nv_util_hrpd_v6_enabled_nv();


#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
  Make sure watermarks are initialized before setting up RLP/wm bindings.
  -------------------------------------------------------------------------*/
  ds707_wmk_setup_wmks(); 

  tc_state_ptr      = ds707_get_pri_tc_ptr();
  tc_state_ptr->call_id = CM_CALL_ID_INVALID;
  tc_state_ptr->so      = DS707_INVALID_SO;

#ifdef FEATURE_IS2000_REL_A_SVD
  /*-----------------------------------------------------------------------
     Allocate sr_id for DS during the power up time.
  -----------------------------------------------------------------------*/
  tc_state_ptr->alloc_sr_id = mccsrid_allocate_srid(SRID_OWNER_DS);

  if (tc_state_ptr->alloc_sr_id == MCCSRID_INVALID_SRID)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DS could not allocate SR_ID");
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "SR_ID %d allocated for DS", 
                     tc_state_ptr->alloc_sr_id);
  }
#endif /* FEATURE_IS2000_REL_A_SVD */
  /*-----------------------------------------------------------------------
  Do the static initialization of the tc state variables
  -----------------------------------------------------------------------*/    
  tc_state_ptr->fwd_rlp.hdlc_inst_ptr = ppp_get_hdlc_handle( PPP_UM_SN_DEV ); 

  tc_state_ptr->fwd_rlp.flow.flow = DSRLP_INVALID_HDR_MRLP_FLOW;
  tc_state_ptr->rev_rlp.flow.flow = DSRLP_INVALID_HDR_MRLP_FLOW;
  tc_state_ptr->fwd_rlp.flow.ver = DSRLP_VER_INVALID;
  tc_state_ptr->rev_rlp.flow.ver = DSRLP_VER_INVALID;

  /*-----------------------------------------------------------------------
    Initialize the phys links
  -----------------------------------------------------------------------*/
  ds707_pkt_phys_link_init();
  ds707_nv_util_aims_nv();

  /*---------------------------------------------------------------------
    Register the Sys Ioctl with PS
  ---------------------------------------------------------------------*/
  ds707_pkt_mgr_sys_ioctl_init();

  /*-------------------------------------------------------------------------
    Initialize each ds707_iface_tbl entry
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS707_MAX_IFACES; i++)
  {
    /*-------------------------------------------------------------------------
      Get the ith iface table entry and Mark it as valid. Marking it as valid
      here, since all below ds707_pdncntx_get* functions require this table
      entry to be valid 
    -------------------------------------------------------------------------*/
    ds707_iface_tbl_ptr = ds707_get_iface_tbl_from_instance(i);
    ds707_iface_tbl_ptr->this_iface_tbl_entry = ds707_iface_tbl_ptr;

    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(i);
    if (iface_ptr == NULL)
    {
      DATA_ERR_FATAL("Iface ptr is NULL");
    }

    /*---------------------------------------------------------------------
      Initialize the ACL info
    ---------------------------------------------------------------------*/
    my_acl_ptr = ds707_pdncntx_get_acl_ptr(ds707_iface_tbl_ptr);
    if (my_acl_ptr == NULL)
    {
      DATA_ERR_FATAL("ACL ptr is NULL");
    }
    my_acl_ptr->acl_fptr       = ds707_rt_acl;
    my_acl_ptr->post_proc_fptr = ds707_rt_acl_post_proc;

    /*---------------------------------------------------------------------
      Initialize the PS_IFACE
    ---------------------------------------------------------------------*/
    ds707_pkt_mgr_iface_init( iface_ptr, 
                              my_acl_ptr,
                              ds707_pkt_get_pri_phys_link_ptr(), 
                              DS707_NUM_PHYS_LINKS );

    /*---------------------------------------------------------------------
      Set PPP termination policy.
    ---------------------------------------------------------------------*/
    if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, FALSE);
    }
    else
    {
      PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, TRUE);
    }

    /*-------------------------------------------------------------------------
      Set up the bring up, tear down and lingering functions for DSSNET6 to 
      bring_up, tear down or cause lingering of the traffic channel.
    -------------------------------------------------------------------------*/
    iface_ptr->bring_up_cmd_f_ptr = ds707_bring_up_cmd;
    iface_ptr->tear_down_cmd_f_ptr = ds707_tear_down_cmd;
    iface_ptr->linger_cmd_f_ptr = ds707_linger_cmd;

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
    /*-----------------------------------------------------------------------
      Active iface in_use/out_use cmd callback.
    -----------------------------------------------------------------------*/
    iface_ptr->in_use_cmd_f_ptr   = ds707_pdncntx_iface_in_use_cmd;
    iface_ptr->out_use_cmd_f_ptr  = ds707_pdncntx_iface_out_use_cmd;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */

    /*-------------------------------------------------------------------------
      PS Iface Addr Mgmt layer requires all Um ifaces to either provide a 
      function for the iface addr_cb_f_ptr and dad_f_ptr or set them to NULL.
    -------------------------------------------------------------------------*/
    iface_ptr->addr_cb_f_ptr = NULL;
    iface_ptr->dad_f_ptr = NULL;
    
    /*-----------------------------------------------------------------------
        To support Data Protocol Logging on the iface
    -----------------------------------------------------------------------*/
/*lint -save -e831*/
/*lint -save -e661*/
   (void) ps_iface_dpl_support_network_logging(iface_ptr,iface_name[i]);

   (void) ps_iface_dpl_set_iface_desc(iface_ptr, iface_desc_name[i]);

   (void) ps_iface_dpl_support_link_logging(iface_ptr,iface_name[i]);

   (void) ps_iface_dpl_support_flow_logging(iface_ptr,iface_name[i]);
/*lint -restore */    
/*lint -restore */    

    dssnet4_sm_ptr = ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr);
    if (dssnet4_sm_create(dssnet4_sm_ptr,iface_ptr) < 0)
    {
      ERR_FATAL("Could not create dssnet4",0,0,0);
    }
    /*---------------------------------------------------------------------
      Register callback for V4 iface down Ind from dssnet4
    -------------------------------------------------------------------- */
    if (-1 == dssnet4_iface_down_ind_callback_reg(
                dssnet4_sm_ptr,
               (dssnet4_iface_down_ind_cb_type) ds707_pdncntx_post_iface_down))
    {
      DATA_ERR_FATAL("Could not register iface_down_ind cb with DSSNET4");
      return (rex_sigs_type)0;
    }
    
#ifdef FEATURE_DATA_PS_IPV6
    if  (ds3g_is_ipv6_enabled()) 
      {
      /*---------------------------------------------------------------------
        create the IPv6 state machine.
      ---------------------------------------------------------------------*/
         sm_config_ptr = ds3g_get_ipv6_sm_config_info(SYS_MODEM_AS_ID_1);
      ip6_sm_ptr         = ds707_pdncntx_get_ip6_sm_ptr(ds707_iface_tbl_ptr);
      if ((NULL == ip6_sm_ptr) || (NULL == sm_config_ptr))
      {
        DATA_ERR_FATAL("ip6_sm_ptr or sm_config is NULL");
        return (rex_sigs_type)0;
      }
      (void)ip6_sm_create( ip6_sm_ptr,
                           iface_ptr,
                           sm_config_ptr);
  
      /*---------------------------------------------------------------------
        Initialize the DSSNET6 State Machine for this pkt instance.
      ---------------------------------------------------------------------*/
      dssnet6_sm_ptr = ds707_pdncntx_get_dssnet6_sm_ptr(ds707_iface_tbl_ptr);
      if (dssnet6_sm_ptr == NULL)
      {
        DATA_ERR_FATAL("dssnet6_sm_ptr is NULL");
        return (rex_sigs_type)0;
      }
      if (dssnet6_sm_create(dssnet6_sm_ptr,
                            iface_ptr,
                            ip6_sm_ptr,
                               TRUE,
                               SYS_MODEM_AS_ID_1) < 0)
      {
        ERR_FATAL("Could not create dssnet6",0,0,0);
      }

      /*---------------------------------------------------------------------
        Register callback for V6 iface down Ind from dssnet6
      -------------------------------------------------------------------- */
      if (-1 == dssnet6_iface_down_ind_callback_reg(
                 dssnet6_sm_ptr,
                (dssnet6_iface_down_ind_cb_type)ds707_pdncntx_post_iface_down))
      {
        DATA_ERR_FATAL("Could not register iface_down_ind cb with DSSNET6");
        return (rex_sigs_type)0;
      }
    }
#endif /* FEATURE_DATA_PS_IPV6 */
  
    /*-----------------------------------------------------------------------
      Setup current iface for IPv4 by default. 
      The actual address family will be set after the iface is selected for 
      an incoming call.
    -----------------------------------------------------------------------*/
    if (ps_iface_set_addr_family(iface_ptr, IFACE_IPV4_ADDR_FAMILY) == -1)
    {
      DATA_ERR_FATAL("ps_iface_set_addr_family returned error");
      return (rex_sigs_type)0;
    }

    /*--------------------------------------------------------------------
     This is rex timer defined to start connection release delay timer
    --------------------------------------------------------------------*/
    conn_delay_timer = 
      ds707_pdncntx_get_conn_delay_timer_ptr(ds707_iface_tbl_ptr);
    rex_def_timer_ex( conn_delay_timer, 
                      ds707_pkt_mgr_iface_conn_delay_timer_cback, 
                      (unsigned long) (ds707_iface_tbl_ptr)); 

    /*----------------------------------------------------------------------- 
      Register for IFACE events (up, down and routable) for this
      iface
    -----------------------------------------------------------------------*/
    ds707_pdncntx_register_iface_ev(ds707_iface_tbl_ptr);

    /*-------------------------------------------------------------------------
      Make pdncntx_cb_ptr ptr point to NULL
    -------------------------------------------------------------------------*/
    ds707_iface_tbl_ptr->pdn_cntx_ptr = NULL;

    /*-------------------------------------------------------------------------
      Make iface client data ptr point to the iface table entry for that iface
    -------------------------------------------------------------------------*/
    iface_ptr->client_data_ptr = (void *)ds707_iface_tbl_ptr;
          
  } /*for DS707_MAX_IFACES*/

  /*-----------------------------------------------------------------------
    If we ever support multiple RM PPP interfaces, then RMSM will need to
    be modified.
  -----------------------------------------------------------------------*/
  if (ds707_rmsm_init() == FALSE)
  {
    ERR_FATAL("could not init 707 RMSM",0,0,0);
  }

#ifdef FEATURE_HDR_QOS
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  ds_1x_runtime_profile_ptr->ds707.ds707_crm_val = 2;
  DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                   "setting CRM Val to 2; Default value override");
#endif /* FEATURE_HDR_QOS */

#if 0
  /*-------------------------------------------------------------------------
    create the Any IP version interfaces only if IPv6 is enabled.
  -------------------------------------------------------------------------*/
  if (DS707_IS_PS_IFACE_IPV6_ENABLED(DS707_DEF_V6_PDN_INSTANCE))
  {
    for (i = 0; i < DS707_MAX_ANY_INSTS; i++)
    {
      dssnetAny_sm_ptr  = GET_ANY_SM_PTR(i);

      /*-----------------------------------------------------------------------
        Initialize the DssnetAny State Machine for this pkt instance - but
        only do so if the failover mode erquires failover (i.e. it is not
        IPv4 or IPv6 only) as these settings don't require the any interface.
      -----------------------------------------------------------------------*/
      if ((failover_mode != IPV4_ONLY) && (failover_mode != IPV6_ONLY))
      {
        if (dssnetAny_sm_create(dssnetAny_sm_ptr,
                                ((void*)DS707_DEF_V6_PDN_INSTANCE),
                                ds707_any_acl,
                                GET_PS_IFACE_IPV4_PTR(DS707_DEF_V4_PDN_INSTANCE),
                                GET_PS_IFACE_IPV6_PTR(DS707_DEF_V6_PDN_INSTANCE),
                                failover_mode) < 0)
        {
          ERR_FATAL("Could not create dssnetAny SM",0,0,0);
        }
      }
    } /* for(all any instances) */
  } /* if(ipv6 is enabled) */
#endif /* 0 */

  /*-------------------------------------------------------------------------
    Initialize SDB
  -------------------------------------------------------------------------*/
#if defined(FEATURE_DATA_IS2000_SDB) || defined(FEATURE_HDR_DOS)
  ds707_sdb_init( 0, 
                  NULL, 
                  NULL, 
                  NULL);
#endif

  ds707_pkt_flow_init();

  /*-----------------------------------------------------------------------
    Now Initialize the watermarks and receive queues for this pkt_state.
  -----------------------------------------------------------------------*/
  ds707_pkt_setup_rlp( NULL, 
                       NULL, 
                       NULL);

  /* ds707_data_session_init moved to dstask power up */

#ifdef FEATURE_HDR_QOS
  /*-------------------------------------------------------------------------
    Initialization of D-QoS should happen after all the ifaces have been 
    created on which QoS is supported. Before initializing, we need to 
    determine whether D_QoS is enabled or not.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_HDR_QOS_DQOS_SUPPORTED_ALWAYS
  /*-------------------------------------------------------------------------
    If FEATURE_HDR_QOS_ALWAYS_ENABLE_DQOS is defined, then we enable D-QoS,
    we don't read an NV item to determine whether DQoS is supported or not
  -------------------------------------------------------------------------*/
  dqos_supported = TRUE;
#else /* FEATURE_HDR_QOS_DQOS_SUPPORTED_ALWAYS */
  /*-------------------------------------------------------------------------
    Since FEATURE_HDR_QOS_ALWAYS_ENABLE_DQOS is not enabled, Check if DQOS 
    needs to be supported by reading corresponding NV item.

    Requirement from AST is to completely get rid of this NV item. Hence
    DATA is removing code to support reading from NV. If the feature is 
    not defined, we do not support DQoS.
  -------------------------------------------------------------------------*/
  dqos_supported = FALSE;

#endif /* FEATURE_HDR_QOS_DQOS_SUPPORTED_ALWAYS */

  ds707_sec_pkt_dqos_init( dqos_supported );
#endif /* FEATURE_HDR_QOS */

     /* Initialize DHCP both v4 and v6*/
  ds707_nv_util_read_dshdr_dhcp_nv();

#if defined(FEATURE_CCM_FLOW_CONTROL)
  ds707_ccm_powerup_init();
#endif

#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDRRLP_LOGGING
  /* Log memory information along with RLP statistics */
  hdrrlplog_notify_onlog(ds707_pkt_mgr_log);
#endif /* FEATURE_HDRRLP_LOGGING */  
#endif /* FEATURE_HDR_QOS */

  ds707_sys_chg_hdlr_init();

  ds707_pkt_mgr_util_init();
  
#ifdef FEATURE_DATA_OPTHO
  /*------------------------------------------------------------------------- 
    Initialize S101 VCTM and DCTM
  -------------------------------------------------------------------------*/
   ds707_nv_util_s101_vctm_init();
   ds707_nv_util_s101_dctm_init();
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
     ds707_nv_util_mip_do_sess_close_nv();
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/  
#ifdef FEATURE_DATA_FALLBACK_LIST
     ds707_nv_util_fb_list_init();
#endif /*FEATURE_DATA_FALLBACK_LIST*/
     ds707_nv_util_additional_dial_str_init();
     ds707_nv_util_ignore_last_pdn_ia_timer_nv();
     ds707_nv_util_read_pdn_override_nv();
  
  if( ps_pkt_info_get_tcp_ack_prio_enabled() )
  {
       ds707_nv_util_read_bp_enabled_nv();
  }

  ds707_register_sync_espm_ind();

   /*-----------------------------------------------------------------------
    Read NV_ROAM_IND_CUSTOMER_HOME_I  to check custome home values for operator
  -----------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex(NV_ROAM_IND_CUSTOM_HOME_I, &ds707_nv_item, ds707_curr_subs_id());

  if( status == NV_DONE_S )
  {
    memscpy(&ds707_roam_ind_custom_home,DS707_ROAM_CUST_HOME_BYTE_LEN,
            &ds707_nv_item.roam_ind_custom_home,DS707_ROAM_CUST_HOME_BYTE_LEN) ;
  }
  DATA_IS707_MSG3(MSG_LEGACY_MED,"NV ROAM IND CUSTOMER HOME "
                                     "Values after NV read custom home are 0x%x 0x%x 0x%x ",
                                     ds707_roam_ind_custom_home[8],
                                     ds707_roam_ind_custom_home[9],
                                     ds707_roam_ind_custom_home[10]);
  /*-------------------------------------------------------------------------
       Register for ds3g events
  -------------------------------------------------------------------------*/
   memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

   filter_info.tech = PS_SYS_TECH_ALL;

   for(i = SYS_MODEM_AS_ID_1; i < DS3GSUBSMGR_SUBS_ID_COUNT; i++)
   {
     filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
              ds3gsubsmgr_subs_id_cm_to_ds((sys_modem_as_id_e_type)i);
     ds3geventmgr_event_reg(DS3GEVENTMGR_MODE_LPM_EV,
                            DS3GEVENTMGR_CLIENT_ID_3GPP2,
                            &filter_info,
                            ds707_pkt_mgr_ds3g_event_cb,
                            NULL);

     ds3geventmgr_event_reg(DS3GEVENTMGR_SUB_NOT_AVAIL_EV,
                            DS3GEVENTMGR_CLIENT_ID_3GPP2,
                            &filter_info,
                            ds707_pkt_mgr_ds3g_event_cb,
                            NULL);
         
   }

   /*-------------------------------------------------------------------------
       Registering Thermal mitigation for primary sus ID as 3GPP2 do not .   
       support multiple SUBS ID.                                                                  .   
  -------------------------------------------------------------------------*/
   memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
   filter_info.tech = PS_SYS_TECH_3GPP2;
   filter_info.ps_subs_id = PS_SYS_PRIMARY_SUBS;
   ds3geventmgr_event_reg(DS3GEVENTMGR_THERMAL_MITIGATION_EV,
                            DS3GEVENTMGR_CLIENT_ID_3GPP2,
                            &filter_info,
                            ds707_pkt_mgr_ds3g_event_cb,
                            NULL);

  /*-------------------------------------------------------------------------
    Currently only interested in reorig signal.
  -------------------------------------------------------------------------*/
  return((rex_sigs_type)DS_PKT_REORIG_SIG);

} /* ds707_pkt_mgr_init() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_READ_HRPD_V6_ENABLED

DESCRIPTION   Read EFS item and set the config file for IPv6 
              enabled for HRPD 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_read_hrpd_v6_enabled(void)
{
   int                        retval = 0;
   hrpd_ipv6_nv_from_efs      config_info;

   char item_file_path[] = "/nv/item_files/data/3gpp2/hrpd_1x_ipv6_config_info";
/*-------------------------------------------------------------------------
       Determine if IPv6 is enabled for HRPD.
-------------------------------------------------------------------------*/
  ds707_hrpd_ipv6_enabled = HRPD_IPV6_DEFAULT_VAL; 

  memset(&config_info,0,sizeof(hrpd_ipv6_nv_from_efs));
  if(ds3g_is_ipv6_enabled())
  {
    /*-----------------------------------------------------------------------
    Read item file into config_info                           .                 
    If item file does not exist write default value to it     .                 
    -----------------------------------------------------------------------*/ 
    retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path, &config_info, sizeof(config_info), ds707_curr_subs_id());  
  
    if ( 0 != retval )
    {
      config_info.data_hrpd_ipv6_enabled = HRPD_IPV6_DEFAULT_VAL;
      DATA_IS707_MSG0(MSG_LEGACY_MED,
            "HRPD_1X_IPV6_ENABLED EFS item not present, using defaults");
    }

    ds707_hrpd_ipv6_enabled = config_info.data_hrpd_ipv6_enabled;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "EFS item hrpd_1x_ipv6_enabled=%d",
                    ds707_hrpd_ipv6_enabled);
  return;
}/* ds707_pkt_mgr_read_hrpd_v6_enabled */



/*===========================================================================
FUNCTION      DS707_PKT_MGR_AIMS_NV

DESCRIPTION   Read the NV item to check if AIMS mode is on

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_aims_nv(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DATA_PS_MIP_CCOA) 
   nv_stat_enum_type  status;

   /*-----------------------------------------------------------------------
  Read NV to check if AIMS mode is ON
  -----------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex(NV_AIMS_ON_I, &ds707_nv_item,ds707_curr_subs_id());  

    if( status == NV_NOTACTIVE_S )
    {
      dshdr_mobile_ip_uses_ccoa = FALSE;
    }
    else if( status != NV_DONE_S)
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad NV read status %d", status);
      dshdr_mobile_ip_uses_ccoa = FALSE;
    }
    else
    {
      dshdr_mobile_ip_uses_ccoa = ds707_nv_item.aims_on;
    }

    if (dshdr_get_mobile_ip_uses_ccoa() && (ps_mipccoa_sm_create() < 0))
    {
      ERR_FATAL("Could not create PS MIP CCoA SM",0,0,0);
    }
   #endif/*defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DATA_PS_MIP_CCOA) */
    return;
}/* ds707_pkt_mgr_aims_nv */

/*===========================================================================
FUNCTION      DS707_PKT_TX_data

DESCRIPTION   Called by PhysLink directly in the IP Flow path and by 
              ds707_pkt_tx_um_SN_data in the PPP Flow path to transmit 
              rev-link data over RLP to the base station.  This is for the 
              SN link/1X-data.
              

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_tx_data
(
  ps_phys_link_type *phys_link_ptr,
  dsm_item_type   **dsm_item_ptr_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *tx_cmd_info
)
{
  ds707_tc_state_type       *default_tc_state_ptr;
  ds707_flow_type           *default_flow_ptr;
  ds1x_meta_info_type        ds1x_meta_info = {0};
  ds1x_meta_info_type       *ds1x_meta_info_ptr = NULL;
  boolean                    push_meta_info = FALSE;
  ps_flow_type              *ps_flow_ptr = NULL;
  ds707_flow_type           *sec_ds_flow_ptr;
#ifdef FEATURE_EHRPD
  ds707_pdn_context_cb_type * pdncntx_cb_ptr;
  ps_iface_type * iface_ptr;
#endif /* FEATURE_EHRPD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(phys_link_ptr);
  DATA_MDM_ARG_NOT_CONST(tx_cmd_info);

  default_tc_state_ptr = ds707_get_pri_tc_ptr();
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
    In this function it is used only for Debug messages.
  -------------------------------------------------------------------------*/
  default_flow_ptr     = DS707_PRI_DS_FLOW_PTR(default_tc_state_ptr);

  /*------------------------------------------------------------------------
    Find the ps_flow_ptr from meta_info is not NULL. If ps_flow thus obtained
    is NULL or invalid, use default ps_flow.
  ------------------------------------------------------------------------*/
  /*lint -save -e641 */
  if ( PS_TX_AND_RT_META_INFO_IS_NULL(meta_info_ptr) == TRUE )
  {
    ps_flow_ptr = NULL;
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "ds707_pkt_tx_data: meta_info_ptr is "
                     "NULL. Setting ps_flow to NULL");
             
  }
  else
  {
    ps_flow_ptr = PS_FLOW_GET_FLOW_FROM_META_INFO(meta_info_ptr);
    if(ps_flow_ptr == NULL)
    {
      ps_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(
                  (ps_iface_type*)PS_META_GET_ROUTING_CACHE(meta_info_ptr));
    }
    /*lint -save -e731 Reason for suppressing error 731*/
    if ( PS_FLOW_IS_VALID( ps_flow_ptr ) == FALSE )
    {
      ps_flow_ptr = NULL;
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "ds707_pkt_tx_data: ps_flow is invalid.Setting to NULL");
    }   
    /*lint -restore Restore lint error 731*/
  }
  /*lint -restore*/

  if( ps_pkt_info_get_tcp_ack_prio_enabled() && 
      TRUE == ds707_back_pressure_params.data.enabled )
  {
    /*-------------------------------------------------------------------------
      Check for Rev link Acks and increment the ack counter.
      If flow control flag is set, reset the ack counter and
      don't disable the back pressure.
    -------------------------------------------------------------------------*/
    if ( DSM_HIGHEST == ((*dsm_item_ptr_ptr)->priority) ) 
    {
      ds707_back_pressure_params.tcp_rev_link_ack_counter++;
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,"Incremented rev link ack counter to %d "
      	              "and ds707_dsm_fc_flag = %d",
                      ds707_back_pressure_params.tcp_rev_link_ack_counter,
                      ds707_dsm_fc_flag);

      if (ds707_back_pressure_params.tcp_rev_link_ack_counter == 1)
      {
        if ( ds707_dsm_fc_flag == TRUE )
        {
          ds707_back_pressure_params.tcp_rev_link_ack_counter = 0;
        }
        else
        {
          ds707_pkt_mgr_disable_back_pressure(&ds707_back_pressure_params);
        }
      }	 
    }
  }

  /*------------------------------------------------------------------------
   If Packet does not need any special treatment then send on Default
   Physical layer transport
  ------------------------------------------------------------------------*/
  if (ps_flow_ptr == NULL)
  {
    DATA_IS707_MSG3( MSG_LEGACY_LOW, 
                     "Enquing data to default WM %x rlpId %d size = %d."
                     "No meta info added", 
                     &default_flow_ptr->tx_wm_list[DSRSP_ROUTE_A], 
                     default_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow,
                     dsm_length_packet(*dsm_item_ptr_ptr));  

#ifdef FEATURE_DATA_3GPP2_VS
    /*------------------------------------------------------------------------ 
      Restart Max PPP inactivity timer for TX path.
      ds_3gpp2_mpit_timer_restart() will restart the timer only if the
      timer is running.
      For LCP Echo-Requests (triggered due to PPP Link Status Determination
      - Always On), MPIT is NOT restarted.
    ------------------------------------------------------------------------*/
    if( ( meta_info_ptr == NULL ) ||
        ( PS_TX_META_GET_PKT_META_INFO_PTR( meta_info_ptr ) == NULL ) ||
        !( PS_TX_META_GET_TX_FLAGS( meta_info_ptr ) & MSG_LCP_ECHO_REQ ) )
    {
      ds_3gpp2_mpit_timer_restart();
    }
#endif /* FEATURE_DATA_3GPP2_VS */

    /*--------------------------------------------------------------------- 
      If it's dormant, record the first iface that triggers re-orig
     ---------------------------------------------------------------------*/
    if( (ds707_pkt_is_dormant(phys_link_ptr)) && (reorig_iface_ptr == NULL) )
    {
      if ( PS_TX_AND_RT_META_INFO_IS_NULL(meta_info_ptr) == TRUE )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                         "ds707_pkt_tx_data: meta_info_ptr is NULL, "
                         "not recording reorig iface ptr");
      }
      else
      {
        reorig_iface_ptr = 
          (ps_iface_type*)PS_META_GET_ROUTING_CACHE(meta_info_ptr);
      }
    }

    if(meta_info_ptr!=NULL &&
       PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr)!=NULL)
    {
      ds1x_meta_info.info.sdb.tx_flags =  PS_META_GET_TX_FLAGS(meta_info_ptr);
      ds1x_meta_info.info.sdb.sockfd   =  PS_META_GET_DOS_ACK_HANDLE(meta_info_ptr);
    }
    if ( ds707_sdb_curr_pkt_needs_SDB(&ds1x_meta_info.info.sdb) &&
         ds707_sdb_supports_SDB_now(ds1x_meta_info.info.sdb.tx_flags, 
                                    ps_flow_ptr) )
    {
      ds1x_meta_info.ds1x_meta_kind |= DS1X_META_SDB_KIND;
      (*dsm_item_ptr_ptr)->kind = DS1X_META_INFO_KIND;
      ds1x_meta_info_ptr = &ds1x_meta_info;
    }
    dsrsp_dsm_enqueue(&default_flow_ptr->rev_tc_ptr->rev_rlp.flow, 
                      default_flow_ptr->tx_wm_list, 
                      dsm_item_ptr_ptr,
                      ds1x_meta_info_ptr);
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                     "WM count after enquing = %d",
                     default_flow_ptr->tx_wm_list[0].current_cnt);

    if (meta_info_ptr)
    {
      PS_META_INFO_FREE(&meta_info_ptr);
    }
#ifdef FEATURE_DS_MOBILE_IP
    /*-----------------------------------------------------------------------
      Let MIP code know that data is being transferred over Um interface
    -----------------------------------------------------------------------*/
    mip_data_transfer_action();
#endif
    return;
  }

  /* Check to see if the packet needs any special treatment.  
   */

  /*lint -e64 variables are of the same size*/
  /* Initialize the meta_kind 
   */
  ds1x_meta_info.ds1x_meta_kind = (ds1x_meta_kind_enum_type) 0;
  /*lint -restore*/

#if defined(FEATURE_DATA_IS2000_SDB) || defined(FEATURE_HDR_DOS)
  if(meta_info_ptr != NULL && PS_META_GET_PKT_META_INFO_PTR(meta_info_ptr) != 0)
  {
    if ( PS_META_GET_TX_FLAGS(meta_info_ptr) != 0)
    {
      ds1x_meta_info.info.sdb.tx_flags =  PS_META_GET_TX_FLAGS(meta_info_ptr);
      ds1x_meta_info.info.sdb.sockfd   =  PS_META_GET_DOS_ACK_HANDLE(meta_info_ptr);
      /*----------------------------------------------------------------------
        If the current packet needs SDB/DOS and we can do SDB/DOS now, we 
        populate the DS1X META INFO, and push it down. 
        The reason we do it here, is because if it is not a reorigination of 
        the data call and the traffic channel is up, then there is no need
        of the SDB/DOS meta info to be present, and can be stripped off. 
        
        In case where the AT is not dormant, there will be only one check for 
        DOS/SDB. Copy the PS Meta Info to DS Meta Info.
      ----------------------------------------------------------------------*/
      /*lint -save -e655 */
      if ( ds707_sdb_curr_pkt_needs_SDB(&ds1x_meta_info.info.sdb) &&
           ds707_sdb_supports_SDB_now(ds1x_meta_info.info.sdb.tx_flags, 
                                      ps_flow_ptr) )
      {
        push_meta_info = TRUE;
        ds1x_meta_info.ds1x_meta_kind |= DS1X_META_SDB_KIND;
        (*dsm_item_ptr_ptr)->kind = DS1X_META_INFO_KIND;
      }
      /*lint -restore 655*/
    }
  }
#endif /* FEATURE_DATA_IS2000_SDB || FEATURE_HDR_DOS */

#ifdef FEATURE_HDR_QOS
  if(meta_info_ptr != NULL && PS_META_GET_PKT_META_INFO_PTR(meta_info_ptr) != 0)
  {
    /*------------------------------------------------------------------------
     If the CAPABILITY is set on the PS flow for 
     PS_FLOW_CAPABILITY_DELAY_SENSITIVE, then we need to add a timestamp to
     meta info. Copy the PS Meta Info to DS Meta Info.
    ------------------------------------------------------------------------*/
    /*lint -e506 macro being used*/
    /*lint -save -e641 */
    if ( PS_FLOW_GET_CAPABILITY(ps_flow_ptr, 
                                PS_FLOW_CAPABILITY_DELAY_SENSITIVE) )
    /*lint -restore 641*/
    /*lint -restore*/
    {
      push_meta_info = TRUE;
      /*lint -save -e655 */
      ds1x_meta_info.ds1x_meta_kind |= DS1X_META_TIMESTAMP_KIND;
      /*lint -restore */
      qw_equ(ds1x_meta_info.info.timestamp.value, 
             (PS_META_GET_TIMESTAMP(meta_info_ptr))); 
      (*dsm_item_ptr_ptr)->kind = DS1X_META_INFO_KIND;
    }
  }
#endif /* FEATURE_HDR_QOS */

  if (push_meta_info == TRUE)
  {
    ds1x_meta_info_ptr = &ds1x_meta_info;    
  }

#ifdef FEATURE_HDR_QOS
  /*------------------------------------------------------------------------
    Determine the watermark corresponding to the PS Flow and enqueue the 
    data onto that watermark.
   ------------------------------------------------------------------------*/
  sec_ds_flow_ptr = ds707_pkt_mgr_get_ds_flow(ps_flow_ptr);

  /* Check if NULL, or if this is the default flow for 
     the iface to which it belongs to
   */
  if ( (sec_ds_flow_ptr == NULL) ||
       (PS_IFACE_IS_FLOW_DEFAULT( sec_ds_flow_ptr->ps_iface_ptr, 
                                  ps_flow_ptr )) )
  {
    if ( sec_ds_flow_ptr == NULL )
    {
      sec_ds_flow_ptr = default_flow_ptr;
    }
    DATA_IS707_MSG3( MSG_LEGACY_LOW, 
                     "Enquing data to Primary WM %x rlpId %d size = %d", 
                     &sec_ds_flow_ptr->tx_wm_list[DSRSP_ROUTE_A], 
                     sec_ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow,
                     dsm_length_packet(*dsm_item_ptr_ptr));
  }
  else
  {
    /* There is transient time, where the flow was suspeded in ds task 
       context and PS layer puts data in watermark. There is a possibility 
       that these packets might be IP only or PPP frames, it is advised to 
       drop those packet on the floor and let the upper layer re-cover.
       */
    if(PS_FLOW_GET_STATE( sec_ds_flow_ptr->ps_flow_ptr) != FLOW_ACTIVATED)
    {
      DATA_IS707_MSG4(MSG_LEGACY_LOW, 
                    "Flow 0x%x suspended. Sending packet to the default flow. "
                    "Enquing data to default WM %x rlpId %d size = %d.",
                    sec_ds_flow_ptr->ps_flow_ptr,
                    &default_flow_ptr->tx_wm_list[DSRSP_ROUTE_A],
                    default_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow,
                    dsm_length_packet(*dsm_item_ptr_ptr));

      /*--------------------------------------------------------------------- 
        If it's dormant, record the first iface that triggers re-orig
       ---------------------------------------------------------------------*/
      if ( (ds707_pkt_is_dormant(phys_link_ptr)) && (reorig_iface_ptr == NULL) )
      {

        if ( PS_TX_AND_RT_META_INFO_IS_NULL(meta_info_ptr) == TRUE )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "ds707_pkt_tx_data: meta_info_ptr is NULL, "
                           "not recording reorig iface ptr");
        }
        else
        {
          reorig_iface_ptr = 
            (ps_iface_type*)PS_META_GET_ROUTING_CACHE(meta_info_ptr);
        }
      }

      dsrsp_dsm_enqueue(&default_flow_ptr->rev_tc_ptr->rev_rlp.flow,
                      default_flow_ptr->tx_wm_list,
                      dsm_item_ptr_ptr,
                      ds1x_meta_info_ptr);
      DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "WM count after enquing = %d",
                     default_flow_ptr->tx_wm_list[0].current_cnt);
      PS_META_INFO_FREE(&meta_info_ptr);

      #ifdef FEATURE_NIQ_EHRPD
     /*------------------------------------------------------------------------
      Send the flow activity indication to the secondary packet manager.
     ------------------------------------------------------------------------*/
      ds707_nw_init_qos_rl_data_activity_start(sec_ds_flow_ptr->ps_flow_ptr);
      #endif /*FEATURE_NIQ_EHRPD*/
      return;
    }
    /* There is transient time, when the ps flow is acitve but air link QOS
    is not grated, thus flow to RLP binding is removed. In this case drop
    the packet as there is no RLP to write to. Let the upper layer recover */
    else if ( sec_ds_flow_ptr->state.air_link_qos_state != 
               DS707_FLOW_AIR_LINK_QOS_GRANTED )
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Sec DS flow 0x%x in air link qos state = %d."
                       "Dropping transient packet", 
                       sec_ds_flow_ptr,
                       sec_ds_flow_ptr->state.air_link_qos_state);
      dsm_free_packet(dsm_item_ptr_ptr);
      PS_META_INFO_FREE(&meta_info_ptr);
      return;
    }
    /* There is transient time when PS flow is active and air link QoS 
    is granted but filter are not installed on the PDSN. In this case 
    drop the packet and let the upper layers re-cover */
    else if (sec_ds_flow_ptr->state.fltr_state != DS707_FLOW_FLTR_INSTALLED)
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Sec DS flow 0x%x in filter state = %d."
                       "Dropping transient packet", 
                       sec_ds_flow_ptr,
                       sec_ds_flow_ptr->state.fltr_state);
      dsm_free_packet(dsm_item_ptr_ptr);
      PS_META_INFO_FREE(&meta_info_ptr);
      return;
    }
    /* This function is called under PS task when enqueing the data
     * to WM. There is possibility that DS has unbounded reverse RLP
     * at this moment. To avoid crash, it needs to return if reverse
     * RLP is unbounded.
     */
    else if ( sec_ds_flow_ptr->rev_tc_ptr == NULL )
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "Sec DS flow 0x%x Reverse link is NULL."
                       "Dropping transient packet",
                       sec_ds_flow_ptr );
      dsm_free_packet(dsm_item_ptr_ptr);
      PS_META_INFO_FREE(&meta_info_ptr);
      return;
    }
    else
    {
      DATA_IS707_MSG3( MSG_LEGACY_LOW, 
                       "Enquing data to SEC WM %x rlpId %d size = %d", 
                       &sec_ds_flow_ptr->tx_wm_list[DSRSP_ROUTE_A], 
                       sec_ds_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow,
                       dsm_length_packet(*dsm_item_ptr_ptr));
#ifdef FEATURE_EHRPD
      /* If this is eHRPD non-PPP QoS data, start the PDN inactivity timer. */
      /* The assumption here is that one RLP serves one PDN for non-PPP flows */
      if( ( ds707_pkt_is_ehrpd_mode_of_operation() ) &&
          ( sec_ds_flow_ptr->rev_tc_ptr->ps_phys_link_ptr->link_protocol_info.link_rx_f_ptr == NULL ) 
        )
        {
         iface_ptr = sec_ds_flow_ptr->ps_iface_ptr;
          
          if( iface_ptr != NULL )
          {
            pdncntx_cb_ptr = 
              ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
            if( pdncntx_cb_ptr != NULL )
            {
              DATA_IS707_MSG0( MSG_LEGACY_MED, 
                             "non-PPP QoS TX: Start Inactivity Timer");
              ds707_pdncntx_inactivity_timer_start(pdncntx_cb_ptr); 
            }
          }
      } /* eHRPD non-PPP data */

      #ifdef FEATURE_NIQ_EHRPD
     /*------------------------------------------------------------------------
      Send the flow activity indication to the secondary packet manager.
     ------------------------------------------------------------------------*/
      ds707_nw_init_qos_rl_data_activity_start(sec_ds_flow_ptr->ps_flow_ptr);
      #endif /*FEATURE_NIQ_EHRPD*/
#endif /* FEATURE_EHRPD */
    }
  }

#ifdef FEATURE_DATA_3GPP2_VS
  /*------------------------------------------------------------------------ 
    Restart Max PPP inactivity timer for TX path.
    ds_3gpp2_mpit_timer_restart() function will restart the timer only 
    if the timer is running.
    For LCP Echo-Requests (triggered due to PPP Link Status Determination
    - Always On), MPIT is NOT restarted.
  ------------------------------------------------------------------------*/
  if( ( meta_info_ptr == NULL ) ||
      ( PS_TX_META_GET_PKT_META_INFO_PTR( meta_info_ptr ) == NULL ) ||
      !( PS_TX_META_GET_TX_FLAGS( meta_info_ptr ) & MSG_LCP_ECHO_REQ ) )
  {
    ds_3gpp2_mpit_timer_restart();
  }
#endif /* FEATURE_DATA_3GPP2_VS */

  dsrsp_dsm_enqueue(&sec_ds_flow_ptr->rev_tc_ptr->rev_rlp.flow, 
                    sec_ds_flow_ptr->tx_wm_list,
                    dsm_item_ptr_ptr,
                    ds1x_meta_info_ptr);    
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "WM count after enquing = %d",
                   sec_ds_flow_ptr->tx_wm_list[DSRSP_ROUTE_A].current_cnt);

#else

    DATA_IS707_MSG3( MSG_LEGACY_LOW, 
                     "Enquing data to PRI WM %x rlpId %d size = %d", 
                     &default_flow_ptr->tx_wm_list[DSRSP_ROUTE_A], 
                     default_flow_ptr->rev_tc_ptr->rev_rlp.flow.flow,
                     dsm_length_packet(*dsm_item_ptr_ptr));

#ifdef FEATURE_DATA_3GPP2_VS
  /*------------------------------------------------------------------------ 
    Restart Max PPP inactivity timer for TX path.
    ds_3gpp2_mpit_timer_restart() function will restart the timer only 
    if the timer is running.
    For LCP Echo-Requests (triggered due to PPP Link Status Determination
    - Always On), MPIT is NOT restarted.
  ------------------------------------------------------------------------*/
  if( ( meta_info_ptr == NULL ) ||
      ( PS_TX_META_GET_PKT_META_INFO_PTR( meta_info_ptr ) == NULL ) ||
      !( PS_TX_META_GET_TX_FLAGS( meta_info_ptr ) & MSG_LCP_ECHO_REQ ) )
  {
    ds_3gpp2_mpit_timer_restart();
  }
#endif /* FEATURE_DATA_3GPP2_VS */

  /*--------------------------------------------------------------------- 
    If it's dormant, record the first iface that triggers re-orig
   ---------------------------------------------------------------------*/
  if ( (ds707_pkt_is_dormant(phys_link_ptr)) && (reorig_iface_ptr == NULL) )
  {
    if ( PS_TX_AND_RT_META_INFO_IS_NULL(meta_info_ptr) == TRUE )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_pkt_tx_data: meta_info_ptr is NULL, "
                       "not recording reorig iface ptr");
    }
    else
    {
      reorig_iface_ptr = 
        ps_iface_type*)PS_META_GET_ROUTING_CACHE(meta_info_ptr);
    }
  }

  /* Initialize the wm_ptr before it is being used. */  
  dsrsp_dsm_enqueue(&default_flow_ptr->rev_tc_ptr->rev_rlp.flow, 
                    default_flow_ptr->tx_wm_list,
                    dsm_item_ptr_ptr,
                    ds1x_meta_info_ptr);  
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "WM count after enquing = %d",
                   default_flow_ptr->tx_wm_list[DSRSP_ROUTE_A].current_cnt);
#endif /*  FEATURE_HDR_QOS */
  /* Free the meta_info if allocated */
  PS_META_INFO_FREE(&meta_info_ptr);
}

/*===========================================================================
FUNCTION      DS707_PKT_TX_UM_SN_DATA

DESCRIPTION   Called by PPP to transmit rev-link data over RLP to the base
              station.  This is for the SN link/1X-data.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 unused variables*/
void ds707_pkt_tx_um_SN_data
(
  dsm_item_type   **dsm_item_ptr_ptr,
  void             *meta_info_ptr,
  void *user_data
)
{
  ps_phys_link_type         *phys_link_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(user_data);

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  /* Call generic function ds707_pkt_tx_data to send data over RLP */
  ds707_pkt_tx_data( phys_link_ptr, 
                     dsm_item_ptr_ptr, 
                     meta_info_ptr, NULL);

} /* ds707_pkt_tx_um_SN_data */
/*lint -restore*/

/*===========================================================================
FUNCTION DS707_IS_BRINGUP_ALLOWED()

DESCRIPTION
  This function checks the current system state and determines if a
  3GPP2 call is allowed or not

PARAMETERS
  end_reason - End reason, to be passed if call bringup is not allowed
  profile_id - Profile ID, used to get the apn pref packet mode

RETURN VALUE
  TRUE: on success
  FALSE: on failure

DEPENDENCIES
  None
===========================================================================*/
boolean ds707_is_bringup_allowed
(
  ps_iface_net_down_reason_type *end_reason,
  ds707_data_session_profile_info_type *data_sess_prof_ptr
)
{
  sys_modem_as_id_e_type resolved_subs_id = SYS_MODEM_AS_ID_NONE;
  
  if ( data_sess_prof_ptr ==NULL )
  {
	DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
					"ds707_is_bringup_allowed: data_sess_profile_ptr NULL, "
					"bringup failed");
	return FALSE;
  }
 
#ifdef FEATURE_EHRPD
  /*--------------------------------------------------------------- 
    Sys Chg SM is in a transition state due to some IRAT. Don't
    allow any new calls until the sys chg SM moves back to NULL
  ---------------------------------------------------------------*/
  if (DS707_SYS_CHG_SM_NULL != ds707_query_sys_chg_sm_state())
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                   "ds707_is_bringup_allowed: Sys Chg SM not in NULL state, "
                   "bring up failed");
    *end_reason = PS_NET_DOWN_REASON_PPP_ERR_CLOSE_IN_PROGRESS;
    return FALSE;
  }
#endif /* FEATURE_EHRPD */

   if ( TRUE == ds707_is_term_in_progress() )
   {
     DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
          "ds707_is_bringup_allowed: PPP term in progress, bring up failed");
     *end_reason = PS_NET_DOWN_REASON_PPP_ERR_CLOSE_IN_PROGRESS;
     return FALSE;
   }
#ifdef FEATURE_MODEM_CONFIG_REFRESH
   else if( ds707_nv_util_is_refresh_in_progress(data_sess_prof_ptr->subs_id) ==  TRUE )
   {
     DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                      "ds707_is_bringup_allowed: HotSwap in progress, bring up failed");
     *end_reason = PS_NET_DOWN_REASON_NV_REFRESH_IN_PROGRESS;
     return FALSE;
   }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   
  if( ds3g_resolve_subs_id((ds_sys_subscription_enum_type)data_sess_prof_ptr->subs_id,
                           &resolved_subs_id ) == FALSE) 
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,"Error in obataining resolved sub id %d "
                       "Policy Sub ID %d",data_sess_prof_ptr->subs_id,resolved_subs_id);
    *end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
    return FALSE;
  }

  if (!ds707_is_apn_pref_pkt_mode_ex(data_sess_prof_ptr,SYS_SYS_MODE_CDMA,resolved_subs_id)
#ifdef FEATURE_DATA_OPTHO
      && ds707_s101_query_sm_state() != DS707_S101_SM_TUNNEL_CALL_ALLOWED
#endif /* FEATURE_DATA_OPTHO */
      )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Pref Mode is not CDMA or HDR, bring up failed");
#ifdef FEATURE_DATA_OPTHO
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
             "or S101 SM in Tunnel call not allowed state - Bring Up failed");

#endif /* FEATURE_DATA_OPTHO */
    *end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
    return FALSE;
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_is_bringup_allowed: Bringup allowed in current mode");
    return TRUE;
  }

}

/*===========================================================================
FUNCTION DS707_POST_FLOW_GONULL_IFACE_DOWN_IND

DESCRIPTION
  This function post flow go null ind (so the phys link is unbinded
  and ref count decreased) and iface down ind

PARAMETERS


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_post_flow_gonull_iface_down_ind
(
  ps_iface_type *this_iface_ptr,
  ps_iface_net_down_reason_type reason

)
{
  void *dssnet_instance = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "Posting flow go null and iface down ind on 0x%x, "
                  "reason: 0x%x",
                  this_iface_ptr, reason);

  ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr),
                      PS_EIC_NOT_SPECIFIED);

  /*---------------------------------------------------------------------- 
    Set correct call end reason in associated dssnet sm
    ---------------------------------------------------------------------*/
  if (ps_iface_addr_family_is_v4(this_iface_ptr))
  {
    dssnet_instance = ds707_pkt_mgr_get_dssnet4_sm_cb(this_iface_ptr);
    dssnet4_set_ps_call_end_reason( dssnet_instance, 
                                    reason);
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if (ps_iface_addr_family_is_v6(this_iface_ptr))
  {
    dssnet_instance = ds707_pkt_mgr_get_dssnet6_sm_cb(this_iface_ptr);
    dssnet6_set_ps_call_end_reason( dssnet_instance, 
                                    reason);
  }
#endif

  ds707_pdncntx_post_iface_down(this_iface_ptr, reason);

} /* ds707_post_flow_gonull_iface_down_ind  */

/*===========================================================================
FUNCTION DS707_SOCKET_BRING_UP_CMD()

DESCRIPTION
  This function is used to bring up an iface.  Will call the registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it
  return DS_EWOULDBLOCK.  Finally if it is in the going down state, it
  returns DS_ECLOSEINPROGRESS.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ps_errno:       value to return error codes
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715 unused variables*/
void ds707_socket_bring_up_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  void* dssnet_instance = NULL;
  int              ret_val = -1;
#ifdef FEATURE_DATA_PS_IPV6
  word   p_rev_index;          /* holds current sys's p_rev index */
#endif
  network_params_info_type * net_cfg_info_ptr = NULL;
#ifdef FEATURE_EPC_HANDOFF
  ds707_call_allowed_hdlr_f_type call_allow_hdlr = NULL;
#endif /* FEATURE_EPC_HANDOFF */
  int16 p_errno;
  ps_iface_type *this_iface_ptr  = NULL;
  uint8          qcmdr_val = 0;
  ps_iface_net_down_reason_type end_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr = NULL;
  ds707_pdn_context_cb_type     *pdncntx_cb_ptr        = NULL;
  ps_flow_type                  *default_ps_flow_ptr=NULL;
  ds707_bring_up_cmd_info_type  *bring_up_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
                                          DS707_NON_ARRAY);

  bring_up_cmd_info_ptr 
                = (ds707_bring_up_cmd_info_type *)(cmd_ptr->cmd_payload_ptr);
  this_iface_ptr  = bring_up_cmd_info_ptr->iface_ptr;
  if(bring_up_cmd_info_ptr->is_user_data_set)
  {
    net_cfg_info_ptr = &(bring_up_cmd_info_ptr->user_data.nw_params_info);
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "ds707_socket_bring_up_cmd: processing "
                   "DS_CMD_707_SOCKETS_BRING_UP for iface 0x%x:%d", 
                   this_iface_ptr->name,
                   this_iface_ptr->instance);


  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  default_ps_flow_ptr =  PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr);

  if (NULL == PS_FLOW_GET_PHYS_LINK(default_ps_flow_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Flow not bound to phys link. Binding it now");
    ps_flow_bind_phys_link(default_ps_flow_ptr,
                           PS_IFACE_GET_PHYS_LINK(this_iface_ptr));
  }
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

   /*--------------------------------------------------------------- 
    Reject call based on the current mitigation level.
  ---------------------------------------------------------------*/  
  if (FALSE == ds707_tm_mgr_is_call_allowed(
                              this_iface_ptr->iface_private.apn_type))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Thermal mitigation in progress."
                                     " call not allowed");
    ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                                       PS_NET_DOWN_REASON_THERMAL_MITIGATION);
    return;
  }

  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                             this_iface_ptr);
  ds707_pdncntx_get_profile(pdncntx_cb_ptr, &data_sess_profile_info_ptr);

  if(!ds707_is_bringup_allowed(&end_reason, data_sess_profile_info_ptr))
  {
    ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                                          end_reason);
    return; 
  }

#ifdef FEATURE_EPC_HANDOFF
  /*-------------------------------------------------------------------------
    Invoke the function that EPC mode handler registered with 3GPP2 MH
    to query EPC to see if iface bring up is allowed, if not we do not
    bring up the call
  -------------------------------------------------------------------------*/
  call_allow_hdlr = ds707_get_call_allowed_hdlr();
  if ( call_allow_hdlr != NULL && data_sess_profile_info_ptr!= NULL )
  {
        DATA_IS707_MSG_SPRINTF_3(MSG_LEGACY_HIGH,
                          "In socket_bringup_cmd: Profile: %d, APN: %s, "
                          "Profile IP type: %d (0->V4,1->V6,2->V4_V6)",
                          data_sess_profile_info_ptr->data_session_profile_id,
                          data_sess_profile_info_ptr->apn_string,
                          data_sess_profile_info_ptr->pdn_type);

    if (( data_sess_profile_info_ptr->data_session_profile_id
            == DATA_SESSION_PROFILE_ID_INVALID ) ||
        ( !call_allow_hdlr( data_sess_profile_info_ptr->data_session_profile_id,
                            (ps_sys_subscription_enum_type)PS_IFACE_GET_SUBS_ID(this_iface_ptr) ) ) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "EPC does not allow call bring up with profile id %d",
                       data_sess_profile_info_ptr->data_session_profile_id );
      ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                           PS_NET_DOWN_REASON_APN_PENDING_HANDOVER);
      return;
    }
  }
#endif /* FEATURE_EPC_HANDOFF */

  ds_1x_profile_store();

  if (ps_iface_addr_family_is_v4(this_iface_ptr))
  {
    dssnet_instance = ds707_pkt_mgr_get_dssnet4_sm_cb(this_iface_ptr);
    #ifdef FEATURE_DS_MOBILE_IP_PERF
    if (!dshdr_get_mobile_ip_uses_ccoa())
    {
      dssnet4_start_mip_perf_stats((dssnet4_sm_cb_type *)dssnet_instance);
    }
    #endif /* ifdef FEATURE_DS_MOBILE_IP_PERF */
  }
  #ifdef FEATURE_DATA_PS_IPV6
  else if ( ps_iface_addr_family_is_v6(this_iface_ptr) )
  {
    if ( ds3g_is_ipv6_enabled() ) 
  {
    if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)
    { 
  /*-----------------------------------------------------------------------
        Retrieve the currently value of the P_REV being used
    -----------------------------------------------------------------------*/
      p_rev_index = ds707_p_rev_get_index();
  
      if (p_rev_index < (word) DS707_P_REV_6)
      {
        /* if the P_rev is < 6 then it will not be possible to make an SO33
           call */
        ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                             PS_NET_DOWN_REASON_NOT_SPECIFIED);
        return;
      }
        else if (qcmdr_val != DS_MDR_MODE_SO33_PREF)
      {
        /* if the P_REV is high enough but we are not set to do SO33 fail */
        ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                             PS_NET_DOWN_REASON_NOT_SPECIFIED);
        return;
      }
    
    }


    dssnet_instance = ds707_pkt_mgr_get_dssnet6_sm_cb(this_iface_ptr);
  }
    else
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "Invalid Iface bringup 0x%x: IPV6 disabled",
                       this_iface_ptr);
      ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                           PS_NET_DOWN_REASON_IPV6_DISABLED);
      return;
    }
  }
  #endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid Iface bringup 0x%x",
                     this_iface_ptr);
    ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                         PS_NET_DOWN_REASON_NOT_SPECIFIED);
    return;
  }


  if ( dssnet_instance == NULL )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "could not find dssnet_instance for iface 0x%x",
                     this_iface_ptr);
    ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                         PS_NET_DOWN_REASON_NOT_SPECIFIED);
    return;
  }


  if (net_cfg_info_ptr != NULL)
  {
    if (-1 == ps_iface_set_net_params(this_iface_ptr, net_cfg_info_ptr))
    {
      ERR ("Couldn't set NET info config parameters!",0,0,0);
    }
  }

  ret_val = dssnet_start(this_iface_ptr,dssnet_instance, &p_errno);
  if(ret_val == -1)
  {
#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
    /*---------------------------------------------------------------------
      Decrement the number of IWLAN+3GPP2 interface used count.
    ---------------------------------------------------------------------*/
    if( num_used_3gpp2_v4_ifaces > 0 )
    {
      num_used_3gpp2_v4_ifaces--;
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Decremented num 3GPP2 v4 IFACEs is %d",
                       num_used_3gpp2_v4_ifaces );
    }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

    if (p_errno == DS_ENETGOINGDORMANT ||
        p_errno == DS_ENETCLOSEINPROGRESS
       )
    {
      ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                           PS_NET_DOWN_REASON_PHYS_LINK_CLOSE_IN_PROGRESS);
    }
    else
    {
      ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                           PS_NET_DOWN_REASON_NOT_SPECIFIED);
    }
    return;
  }

} /* ds707_socket_bring_up_cmd() */
/*lint -restore*/

int ds707_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr
)
{
  ds_cmd_type                   *cmd_ptr               = NULL;
  ds707_bring_up_cmd_info_type  *bring_up_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL == this_iface_ptr)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_bring_up_cmd: iface ptr null. Cannot bring up");
    return -1;
  }

  DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                   "ds707_bring_up_cmd: Proc ID: %d, QCMIP: %d, RMSM state: %d",
                   PS_IFACE_GET_PROC_ID( this_iface_ptr ),
                   ds707_rmsm_get_qcmip_val(),
                   ds707_rmsm_get_state() );

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_bring_up_cmd_info_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return -1;
  }

  /* Populate the generic cmd data */
  bring_up_cmd_info_ptr 
                = (ds707_bring_up_cmd_info_type *)(cmd_ptr->cmd_payload_ptr);
  bring_up_cmd_info_ptr->iface_ptr = this_iface_ptr;

  /* if DUN call */
  if(1 == PS_IFACE_GET_PROC_ID( this_iface_ptr ))
  {
    /* if Netmodel-SIP or RELAY call*/
    if( (0 == ds707_rmsm_get_qcmip_val() ) ||
        /* OR if SIP fallback AND we are in WAITING_FOR_UM_UP_SIP_STATE */
        ( (1 == ds707_rmsm_get_qcmip_val() ) && (2 == ds707_rmsm_get_state()) ) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_bring_up_cmd: Posting DS_707_RMSM_BRING_UP_CMD");

      
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_BRING_UP;
    
      if (NULL == client_data_ptr)
      {
        bring_up_cmd_info_ptr->is_user_data_set = FALSE;
      }
      else
      {
        bring_up_cmd_info_ptr->is_user_data_set = TRUE;
        bring_up_cmd_info_ptr->user_data.pkt_call_info 
                             = *((ds3g_pkt_call_info_type *)client_data_ptr);
      }
      
      ds_put_cmd_ext(cmd_ptr);
      return 0;
    }
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_bring_up_cmd: Posting DS_707_SOCKETS_BRING_UP_CMD");
  
  cmd_ptr->hdr.cmd_id = DS_CMD_707_SOCKETS_BRING_UP;
  if (NULL == client_data_ptr)
  {
    bring_up_cmd_info_ptr->is_user_data_set = FALSE;
  }
  else
  {
    bring_up_cmd_info_ptr->is_user_data_set = TRUE;
    bring_up_cmd_info_ptr->user_data.nw_params_info 
                             = *((network_params_info_type *)client_data_ptr);
  }
  
  ds_put_cmd_ext(cmd_ptr);
  return 0;
} /* ds707_bring_up_cmd() */

/*===========================================================================
FUNCTION DS707_SOCKET_TEAR_DOWN_CMD()

DESCRIPTION
  This function is used to stop an iface.

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715 unused variables*/
void ds707_socket_tear_down_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ps_iface_type *this_iface_ptr = NULL;
  void          *dssnet_instance = NULL;
  boolean       local_abort = FALSE;
  ps_iface_net_down_reason_type end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds707_tear_down_cmd_info_type *tear_down_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_socket_tear_down_cmd: "
                   "Processing DS_CMD_707_SOCKETS_TEAR_DOWN" );
    
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  tear_down_cmd_info_ptr 
                = (ds707_tear_down_cmd_info_type *)(cmd_ptr->cmd_payload_ptr);
  if(tear_down_cmd_info_ptr->is_user_data_set)
  {
    if(((tear_down_cmd_info_ptr->user_data.tear_down_info.validity_mask)
         & (PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK))  
       == PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK)
    {
      end_reason = tear_down_cmd_info_ptr->user_data.tear_down_info
                                                  .data_info.call_end_reason;
    }

    if(((tear_down_cmd_info_ptr->user_data.tear_down_info.validity_mask)
         & (PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK)) 
       == PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK)
    {    
      local_abort 
       = tear_down_cmd_info_ptr->user_data.tear_down_info.data_info.local_abort;
    }
  }

  this_iface_ptr = tear_down_cmd_info_ptr->iface_ptr;
  if (ps_iface_addr_family_is_v4(this_iface_ptr))
  {
    dssnet_instance = ds707_pkt_mgr_get_dssnet4_sm_cb(this_iface_ptr);
    #ifdef FEATURE_DS_MOBILE_IP_PERF
    if (!dshdr_get_mobile_ip_uses_ccoa())
    {
      dssnet4_start_mip_perf_stats((dssnet4_sm_cb_type *)dssnet_instance);
    }
    #endif /* ifdef FEATURE_DS_MOBILE_IP_PERF */
    dssnet4_set_ps_call_end_reason(dssnet_instance, end_reason);
    dssnet4_set_ps_local_abort(dssnet_instance, local_abort);

  }
  #ifdef FEATURE_DATA_PS_IPV6
  else if (ps_iface_addr_family_is_v6(this_iface_ptr)) 
  {
    if(!ds3g_is_ipv6_enabled())
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                       "V6 Iface cleanup 0x%x: %d IPV6 disabled",
                       this_iface_ptr->name,this_iface_ptr->instance);
      ds707_post_flow_gonull_iface_down_ind(this_iface_ptr, 
                                            PS_NET_DOWN_REASON_IPV6_DISABLED);
      return;
    }
    else
    {
      dssnet_instance = ds707_pkt_mgr_get_dssnet6_sm_cb(this_iface_ptr);
      dssnet6_set_ps_call_end_reason(dssnet_instance, end_reason);
      dssnet6_set_ps_local_abort(dssnet_instance, local_abort);
    }
  }
  #endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid Iface tear down 0x%x",
                     this_iface_ptr);
    ASSERT(0)
    return;
  }


  if ( dssnet_instance == NULL )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "could not find dssnet_instance for iface 0x%x",
                     this_iface_ptr);
    ASSERT(0);
    return;
  }

  if (-1 == dssnet_stop(this_iface_ptr, dssnet_instance))
  {
    DATA_ERR_FATAL("dssnet_stop failed");
    return;
  }
} /* ds707_socket_tear_down_cmd() */
/*lint -restore*/

/*===========================================================================
FUNCTION DS707_TEAR_DOWN_CMD()

DESCRIPTION
 This will call the ds707_tear_down_cmd_processor() function with last option
 as FALSE.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_tear_down_cmd
(
ps_iface_type *this_iface_ptr,
void          *client_data_ptr
)
{
  if (NULL == this_iface_ptr)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_tear_down_cmd: iface ptr null. Cannot tear down");
    return -1;
  }

  return ds707_tear_down_cmd_processor(this_iface_ptr, client_data_ptr, FALSE);
}/* ds707_tear_down_cmd() */

/*===========================================================================
FUNCTION DS707_TEAR_DOWN_CMD_PROCESSOR()

DESCRIPTION
  This function is used to stop an iface
 
PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.
  is_lpm_mode: indicator of whether the tear down is in Low Power mode or not.
 
RETURN VALUE
   0 for success
  -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_tear_down_cmd_processor
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr,
  boolean is_lpm_mode
)
{
  ds707_mip_profile_type        *ds_1x_runtime_profile_ptr = NULL;
  ds_cmd_type                   *cmd_ptr                   = NULL;
  ds707_tear_down_cmd_info_type *tear_down_cmd_info_ptr    = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                   "ds707_tear_down_cmd: Proc ID: %d, QCMIP: %d, RMSM state: %d",
                   PS_IFACE_GET_PROC_ID( this_iface_ptr ),
                   ds707_rmsm_get_qcmip_val(),
                   ds707_rmsm_get_state() );

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_tear_down_cmd_info_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return -1;
  }

  tear_down_cmd_info_ptr 
              = (ds707_tear_down_cmd_info_type *)(cmd_ptr->cmd_payload_ptr);

  /* Populate the generic cmd data*/
  tear_down_cmd_info_ptr->iface_ptr = this_iface_ptr;
  tear_down_cmd_info_ptr->lpm_mode = is_lpm_mode;
  if (NULL == client_data_ptr)
  {
    tear_down_cmd_info_ptr->is_user_data_set = FALSE;
  }
  else
  {
    tear_down_cmd_info_ptr->is_user_data_set = TRUE;
    memscpy(&(tear_down_cmd_info_ptr->user_data.tear_down_info),
            sizeof(ps_iface_tear_down_client_data_info_type),		
            (ps_iface_tear_down_client_data_info_type *)client_data_ptr,
            sizeof(ps_iface_tear_down_client_data_info_type));
  }
        
  /* if DUN call */
  if(1 == PS_IFACE_GET_PROC_ID( this_iface_ptr ))
  {
    /* if Netmodel-SIP or RELAY call*/
    if( (0 == ds707_rmsm_get_qcmip_val() ) || 
        (DS_CRM_VAL_RELAY_MODE == 
                        ds_1x_runtime_profile_ptr->ds707.ds707_crm_val) ||
        /* OR if SIP fallback AND we are in any one of the SIP states */
        ( (1 == ds707_rmsm_get_qcmip_val() ) && ds707_is_rmsm_in_SIP_state()) 
      )
    {

      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "ds707_tear_down_cmd: Posting DS_707_RMSM_TEAR_DOWN_CMD");
      
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_TEAR_DOWN;
        
      ds_put_cmd_ext(cmd_ptr);
      return 0;
    }
    else if ((1 == ds707_rmsm_get_qcmip_val()) && ds707_is_rmsm_in_non_MIP_SIP_state())
    {
      /* if SIP fallback AND we are not in any one of the MIP/SIP states*/
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "ds707_tear_down_cmd: Illegal tear down in RMSM State: %d",
                       ds707_rmsm_get_state());

      ds_release_cmd_buf(&cmd_ptr);
      return -1;
    }
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_tear_down_cmd: Posting DS_707_SOCKETS_TEAR_DOWN_CMD");

  cmd_ptr->hdr.cmd_id = DS_CMD_707_SOCKETS_TEAR_DOWN;
  
  ds_put_cmd_ext(cmd_ptr);
  return 0;
 
}/* ds707_tear_down_cmd_processor() */

/*===========================================================================
FUNCTION DS707_LINGER_CMD()

DESCRIPTION
  This function is used to make an iface go in LINGERING.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_linger_cmd
(
ps_iface_type       *this_iface_ptr,
void                *client_data_ptr
)
{
  ps_phys_link_type   *phys_link_ptr = NULL;
  int                 ret_val;
  int16               err_num;
  boolean             traffic_channel_teardown;
  
  if (!(PS_IFACE_IS_VALID(this_iface_ptr))) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Invalid IFACE ptr passed to data sess ev cb");
    return -1;
  }

  phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(this_iface_ptr);
  traffic_channel_teardown = 
                  ds_get_traffic_channel_teardown_in_lingering_val();
  /*--------------------------------------------------------------------------
    Teardown traffic channel only if NV_TRAFFIC_CHANNEL_TEARDOWN_IN_LINGERING
    is set to TRUE
  --------------------------------------------------------------------------*/
  if (traffic_channel_teardown)
  {
    /*-------------------------------------------------------------------------
      Call the underlying phys link's "down" command.
    -------------------------------------------------------------------------*/
    
    ret_val = ps_phys_link_down_cmd( phys_link_ptr,
                                     &err_num,
                                     client_data_ptr );
    if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "Cant tear down phys link - error %d",
                       err_num);
    }
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "Do not tear down phys link %x in lingering",
                     phys_link_ptr);
  }

  /*---------------------------------------------------------------------
    Set the Linger timer value according to the data session profile.
    Here we use DYNAMIC_LINGER_CONFIG so that the Linger timer value is
    set according to the current bearer technology.
  ---------------------------------------------------------------------*/
  ds707_data_session_set_linger_tmr_val( this_iface_ptr, 
                                         DYNAMIC_LINGER_CONFIG);

  /*-------------------------------------------------------------------
    Set the iface linger status to TRUE
  -------------------------------------------------------------------*/
  ds707_data_session_set_iface_linger_status( this_iface_ptr, 
                                              TRUE);

  return 0;

}/* ds707_linger_cmd */

  #if defined(FEATURE_DS_CAM) && defined(FEATURE_HDR)

/*===========================================================================
FUNCTION DS707_PKT_SET_CAM_CTA_TIMEOUT

DESCRIPTION    This function sets cam timeout ( RLP idle timeout, after which
               TC is brought down ) to the value specified by the CAM module.
               Current value of cta_timeout is saved so that it is restored
               after TC goes down. Note that this value is specified in sec.

DEPENDENCIES   None

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now cam_cta_timeout.

===========================================================================*/

void ds707_pkt_set_cam_cta_timeout
(
dword         duration                 /* time interval, in sec          */
)
{
  ds_cta_timer_struct cta_timer_change;
  ps_phys_link_type     *phys_link_ptr;
  uint32                 cta_val = 0; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  cta_timer_change.ds_old_cta_val = cta_val;
  cta_timer_change.ds_new_cta_val = duration;

  event_report_payload (EVENT_DS_CTA_TIMER, 
                        sizeof(ds_cta_timer_struct), 
                        (void*)&cta_timer_change);

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "EVENT_DS_CTA_TIMER change from %d to %d",
                   cta_timer_change.ds_old_cta_val,
                   cta_timer_change.ds_new_cta_val);

  /*-------------------------------------------------------------------------
   Save current cta_timeout and set it to CAM CTA timeout
 -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "Changing cta timeout from %d to %d",
                   ds707_pkti_save_cta_val,duration);

  ds707_pkti_save_cta_val = cta_val;
#endif /*FEATURE_DS_CAM*/
#endif /* FEATURE_HDR*/
  cta_val = duration;
  (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,(void*)&cta_val,0);
#ifdef FEATURE_HDR
  hdrrlp_copy_default_cta_val(cta_val);
#endif /* FEATURE_HDR */

  if ( PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP )
  {
#ifdef FEATURE_HDR
    if (ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR) 
    { 
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                 "Enhanced_cta CAM resets idle timeout - starting CTA timer");
      hdrrlp_phy_link_up_inactivity_timer_hdlr();
    }
    else
#endif
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "CAM resets idle timeout - starting CTA timer");
      ds707_cta_reset_timer(DS707_CTA_START_TIMER,
                        (ds707_get_pri_tc_ptr())->so);
    }
  }
} /* ds_set_cam_cta_timeout() */

/*===========================================================================
FUNCTION DS707_PKT_RESTORE_CAM_CTA_TIMEOUT

DESCRIPTION    This function restores cta timeout to what it was before CAM
               module set it to CAM cta timeout.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now what is set by atcop.

===========================================================================*/

void ds707_pkt_restore_cam_cta_timeout
(
void
)
{
  ds_cta_timer_struct cta_timer_change;
  uint32              cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  cta_timer_change.ds_old_cta_val = cta_val;
#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM
  cta_timer_change.ds_new_cta_val = ds707_pkti_save_cta_val;
#endif
#endif
  event_report_payload (EVENT_DS_CTA_TIMER, 
                        sizeof(ds_cta_timer_struct), 
                        (void*)&cta_timer_change);

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "EVENT_DS_CTA_TIMER change from %d to %d",
                   cta_timer_change.ds_old_cta_val,
                   cta_timer_change.ds_new_cta_val);
  /*-------------------------------------------------------------------------
    Restore cta_timeout to previous value;
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "restoring CTA timeout to %d",ds707_pkti_save_cta_val);

  cta_val = ds707_pkti_save_cta_val;
  (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,0);
  hdrrlp_copy_default_cta_val(cta_val);
#endif
#endif
} /* ds707_pkt_restore_cam_cta_timeout() */

  #endif /* FEATURE_DS_CAM && FEATURE_HDR */

  #ifdef FEATURE_DS_RDUD

/*===========================================================================
FUNCTION DS707_PKT_SET_RDUD_CTA_TIMEOUT

DESCRIPTION    This function sets cam timeout ( RLP idle timeout, after which
               TC is brought down ) to the value specified by the RDUD module.
               Current value of cta_timeout is saved so that it is restored
               after TC goes down. Note that this value is specified in sec.

DEPENDENCIES   None

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now cam_cta_timeout.

===========================================================================*/

void ds707_pkt_set_rdud_cta_timeout
(
dword         duration                 /* time interval, in sec          */
)
{
  ds_cta_timer_struct cta_timer_change;
  ps_phys_link_type     *phys_link_ptr;
  uint32              cta_val = 0; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  cta_timer_change.ds_old_cta_val = cta_val;
  cta_timer_change.ds_new_cta_val = duration;

  event_report_payload (EVENT_DS_CTA_TIMER, 
                        sizeof(ds_cta_timer_struct), 
                        (void*)&cta_timer_change);

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "EVENT_DS_CTA_TIMER change from %d to %d. "
                   "Setting  707 cta timeout to %d secs",
                   cta_timer_change.ds_old_cta_val,
                   cta_timer_change.ds_new_cta_val,
                   duration);

  /*-------------------------------------------------------------------------
    Save current cta_timeout and set it to RDUD CTA timeout
  -------------------------------------------------------------------------*/
  ds707_pkti_rdud_save_cta_val = cta_val;

  cta_val = duration;
  (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,(void*)&cta_val,0);
  cta_timer_change.ds_new_cta_val = cta_val;

#ifdef FEATURE_HDR
  hdrrlp_copy_default_cta_val(cta_val);
#endif

  if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP)
  {
#ifdef FEATURE_HDR
    if (ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR)
    { 
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "Enhanced_cta RDUD resets idle timeout - "
                     "starting CTA timer");
      hdrrlp_phy_link_up_inactivity_timer_hdlr();
    }
    else
#endif
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "RDUD resets idle timeout - starting CTA timer");
      ds707_cta_reset_timer(DS707_CTA_START_TIMER,
                        (ds707_get_pri_tc_ptr())->so);
    }
  } 
} /* ds707_pkt_set_rdud_cta_timeout() */

/*===========================================================================
FUNCTION DS707_PKT_RESTORE_RDUD_CTA_TIMEOUT

DESCRIPTION    This function restores cta timeout to what it was before RDUD
               module set it to RDUD cta timeout.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now what is set by atcop.

===========================================================================*/

void ds707_pkt_restore_rdud_cta_timeout
(
void
)
{
  ds_cta_timer_struct cta_timer_change;
  ps_phys_link_type     *phys_link_ptr;
  uint32              cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  cta_timer_change.ds_old_cta_val = cta_val;
  cta_timer_change.ds_new_cta_val = ds707_pkti_rdud_save_cta_val;

  event_report_payload (EVENT_DS_CTA_TIMER, 
                        sizeof(ds_cta_timer_struct), 
                        (void*)&cta_timer_change);

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "EVENT_DS_CTA_TIMER change from %d to %d. "
                   "Restoring ds707_cta_val to %d ",
                   cta_timer_change.ds_old_cta_val,
                   cta_timer_change.ds_new_cta_val,
                   ds707_pkti_rdud_save_cta_val);
  /*-------------------------------------------------------------------------
  Restore cta_timeout to previous value;
  -------------------------------------------------------------------------*/
  cta_timer_change.ds_old_cta_val = cta_val;
  cta_val = ds707_pkti_rdud_save_cta_val;
  (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,(void*)&cta_val,0);

#ifdef FEATURE_HDR
  hdrrlp_copy_default_cta_val(cta_val);
  if ((ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR) &&
      (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP))
  {
    hdrrlp_phy_link_up_inactivity_timer_hdlr();
  }
#endif
} /* ds707_pkt_restore_rdud_cta_timeout() */

/*===========================================================================
FUNCTION DS707_PKT_SET_RDUD_CTA_DEFAULT_VALUE

DESCRIPTION    This function changes the default value of cta timer. (Called 
               from ioctl to change default value of cta timer). 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Default value of CTA timer is changed.

===========================================================================*/

void ds707_pkt_set_rdud_cta_default_value
(
  dword         duration                 /* time interval, in sec          */
)
{
  ds707_pkti_rdud_save_cta_val = duration;  
}

  #endif  /* FEATURE_DS_RDUD */


/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_LAST_ORIG_FAIL_REASON

DESCRIPTION    This function returns the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   A value defined in ds707_orig_fail_reason_enum_type indicating
               the reason the call was ended.

SIDE EFFECTS   None

===========================================================================*/

ds707_orig_fail_reason_enum_type ds707_pkt_mgr_get_last_orig_fail_reason
(
void
)
{
  return ds707_mode_handler_info.call_orig_fail_reason;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_LAST_ORIG_FAIL_REASON

DESCRIPTION    This function sets the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   0 upon sucess, -1 on failure

SIDE EFFECTS   None

===========================================================================*/

int ds707_pkt_mgr_set_last_orig_fail_reason
(
/* The reason the call has ended from CM*/
cm_call_end_e_type end_reason
)
{
  int ret_val = 0;
  switch (end_reason)
  {
    case CM_CALL_END_ORIG_THR:
      {
        ds707_mode_handler_info.call_orig_fail_reason = 
                                              DS707_ORIG_FAIL_CALL_THROTTLE;
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "Setting last call_orig_fail_reason to %d", 
                         DS707_ORIG_FAIL_CALL_THROTTLE);

        break;
      }
    default:
      {
        ds707_mode_handler_info.call_orig_fail_reason =  
                                                DS707_ORIG_FAIL_REASON_NONE;
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "Setting last call_orig_fail_reason to %d",  
                         DS707_ORIG_FAIL_REASON_NONE);

        ret_val = -1;
        break;
      }
  }
  return ret_val;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_CLEAR_LAST_ORIG_FAIL_REASON

DESCRIPTION    This function clears the reason the last data call's
               origination failed (if applicable).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_clear_last_orig_fail_reason
(
void
)
{
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Setting last call_orig_fail_reason to None (Reset)");
  ds707_mode_handler_info.call_orig_fail_reason = 
                                                DS707_ORIG_FAIL_REASON_NONE;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_LAST_CALL_END_REASON

DESCRIPTION    This function returns the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   A value defined in ps_iface_net_down_reason_type indicating
               the reason the call was ended.

SIDE EFFECTS   None

===========================================================================*/

ps_iface_net_down_reason_type ds707_pkt_mgr_get_last_call_end_reason
(
  void
)
{
  return ds707_mode_handler_info.call_end_reason;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_LAST_CALL_END_REASON

DESCRIPTION    This function sets the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_last_call_end_reason
(
  /* The reason the call has ended from CM*/
  cm_call_end_e_type end_reason,
  /* The call cmd error code from CM */
  cm_call_cmd_err_e_type   call_cmd_err
)
{
  switch(end_reason)
  {
    case CM_CALL_END_CLIENT_END:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CLIENT_END;
 
      break;
    }

    case CM_CALL_END_NO_SRV:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_NO_SRV;
 
      break;
    }

    case CM_CALL_END_CDMA_LOCK:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CDMA_LOCK;
      
      break;
    }

    case CM_CALL_END_FADE:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_FADE;
      
      break;
    }

    case CM_CALL_END_INTERCEPT:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_INTERCEPT;
      
      break;
    }

    case CM_CALL_END_REORDER:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_REORDER;
      
      break;
    }

    case CM_CALL_END_REL_NORMAL:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_REL_NORMAL;
      
      break;
    }

    case CM_CALL_END_REL_SO_REJ:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_REL_SO_REJ;
      
      break;
    }

    case CM_CALL_END_INCOM_CALL:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_INCOM_CALL;
      
      break;
    }

    case CM_CALL_END_ALERT_STOP:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ALERT_STOP;
      
      break;
    }

    case CM_CALL_END_ACTIVATION:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ACTIVATION;
      
      break;
    }

    case CM_CALL_END_MC_ABORT:
    {
       ds707_mode_handler_info.call_end_reason = 
         PS_NET_DOWN_REASON_MC_ABORT;

       break;
    }

    case CM_CALL_END_MAX_ACCESS_PROBE:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_MAX_ACCESS_PROBE;
      
      break;
    }

    case CM_CALL_END_PSIST_NG:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_PSIST_NG;

       break;
    }

    case CM_CALL_END_UIM_NOT_PRESENT:
    {
       ds707_mode_handler_info.call_end_reason = 
         PS_NET_DOWN_REASON_UIM_NOT_PRESENT;

       break;
    }

    case CM_CALL_END_ACC_IN_PROG:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ACC_IN_PROG;
      
      break;
    }

    case CM_CALL_END_ACC_FAIL:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ACC_FAIL;
      
      break;
    }

    case CM_CALL_END_RETRY_ORDER:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_RETRY_ORDER;

      break;
    }

    case CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CCS_NOT_SUPPORTED_BY_BS;
      
      break;
    }

    case CM_CALL_END_NO_RESPONSE_FROM_BS:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_NO_RESPONSE_FROM_BS;
      
      break;
    }

    case CM_CALL_END_REJECTED_BY_BS:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_REJECTED_BY_BS;
      
      break;
    }

    case CM_CALL_END_INCOMPATIBLE:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_INCOMPATIBLE;
      
      break;
    }

    case CM_CALL_END_ACCESS_BLOCK:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ACCESS_BLOCK;
      
      break;
    }

    case CM_CALL_END_ALREADY_IN_TC:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ALREADY_IN_TC;
      
      break;
    }

    case CM_CALL_END_USER_CALL_ORIG_DURING_GPS:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_GPS;
      
      break;
    }

    case CM_CALL_END_USER_CALL_ORIG_DURING_SMS:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_SMS;
      
      break;
    }

    case CM_CALL_END_USER_CALL_ORIG_DURING_DATA:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_VOICE_CALL;
      break;
    }

    case CM_CALL_END_REDIR_OR_HANDOFF:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_REDIR_OR_HANDOFF;
      
      break;
    }

    case CM_CALL_END_ACCESS_BLOCK_ALL:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_ACCESS_BLOCK_ALL;

      break;
    }

    case CM_CALL_END_IS707B_MAX_ACC:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_IS707B_MAX_ACC;

      break;
    }

    case CM_CALL_END_NO_CDMA_SRV:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_NO_CDMA_SRV;
      
      break;
    }

    /* HDR specific call-end reasons from CM */
    case CM_CALL_END_CD_GEN_OR_BUSY:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CD_GEN_OR_BUSY;
      
      break;
    }

    case CM_CALL_END_CD_BILL_OR_AUTH:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CD_BILL_OR_AUTH;
      
      break;
    }

    case CM_CALL_END_CHG_HDR:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CHG_HDR;
      
      break;
    }

    case CM_CALL_END_EXIT_HDR:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_EXIT_HDR;
      
      break;
    }

    case CM_CALL_END_HDR_NO_SESSION:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_NO_SESSION;
      
      break;
    }

    case CM_CALL_END_CM_COLLOC_ACQ_FAIL:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_COLLOC_ACQ_FAIL;

      break;
    }

    case CM_CALL_END_HDR_ORIG_DURING_GPS_FIX:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_ORIG_DURING_GPS_FIX;
      
      break;
    }

    case CM_CALL_END_HDR_CS_TIMEOUT:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_CS_TIMEOUT;
      
      break;
    }

    case CM_CALL_END_HDR_RELEASED_BY_CM:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_RELEASED_BY_CM;
      
      break;
    }

    case CM_CALL_END_OTASP_COMMIT_IN_PROG:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_OTASP_COMMIT_IN_PROG;
      
      break;
    }

    case CM_CALL_END_NO_HYBR_HDR_SRV:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_NO_HYBR_HDR_SRV;

      break;
    }

    case CM_CALL_END_HDR_NO_LOCK_GRANTED:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_NO_LOCK_GRANTED;

      break;
    }

    case CM_CALL_END_HOLD_OTHER_IN_PROG:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HOLD_OTHER_IN_PROG;

      break;
    }

    case CM_CALL_END_HDR_FADE:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_FADE;

      break;
    }

    case CM_CALL_END_HDR_ACC_FAIL:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_HDR_ACC_FAIL;

      break;
    }

    case CM_CALL_END_INTERNAL_ERROR:
    {
       ds707_mode_handler_info.call_end_reason = 
         ds707_pkt_mgr_set_last_call_end_reason_from_call_cmd_err(
           call_cmd_err );
       break;
    }

    case CM_CALL_END_THERMAL_EMERGENCY:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_THERMAL_EMERGENCY;

      break;
    }

    case CM_CALL_END_ORIG_THR:
    {
      ds707_mode_handler_info.call_end_reason = 
        PS_NET_DOWN_REASON_CALL_ORIG_THROTTLED;

      break;
    }

    default:
    {
      ds707_mode_handler_info.call_end_reason =  
        PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
      
      break;
    }
  } /* switch */

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "Received CM call end reason as %d"
                   "Setting PS net down reason to %d",
                   end_reason,  
                   ds707_mode_handler_info.call_end_reason);

}

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION       DS707_PKT_MGR_SET_LAST_TUN_CALL_END_REASON

DESCRIPTION    This function sets the reason the last tunnel call
               ended (either failed during origination/normal teardown)

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_last_tun_call_end_reason
(
  uint32 end_reason
)
{  
  ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_NOT_SPECIFIED;
  switch (end_reason)
  {
    /* Connection Denied */
    case HDRHAI_CONN_FAIL_IDLE_CONN_DENY:
      ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_CONN_DENIED;
      break;

    /* Connection Request Reservation On Request msg failure */
    case HDRHAI_CONN_FAIL_IDLE_CR_ROR:
      ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_CONN_CR_ROR_FAILED;
      break;

    /* Signaling Adaptation Protocol L2 ACK failure */
    case HDRHAI_CONN_FAIL_SAP_L2_FAILED:
      ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_CONN_SAP_L2_FAILED;
      break;

    /* SAP L3 Response failure */
    case HDRHAI_CONN_FAIL_SAP_L3_FAILED:
      ds707_mode_handler_info.call_end_reason =  
          PS_NET_DOWN_REASON_TUN_CONN_SAP_L3_FAILED;
      break;

    /* SAP time out */
    case HDRHAI_CONN_FAIL_SAP_TIMEOUT:
      ds707_mode_handler_info.call_end_reason =  
        PS_NET_DOWN_REASON_TUN_CONN_SAP_TIMEOUT;
      break;

    /* No HDR session */
    case HDRHAI_CONN_FAIL_SAP_NO_SESSION:
      ds707_mode_handler_info.call_end_reason =  
        PS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SESSION;
      break;

    /* PreReg is not allowed  */
    case HDRHAI_CONN_FAIL_SAP_PREREG_NOT_ALLOW:
      ds707_mode_handler_info.call_end_reason =  
        PS_NET_DOWN_REASON_TUN_CONN_SAP_PREREG_NOT_ALLOW;
      break;

    /* Active Handoff in progess  */
    case HDRHAI_CONN_FAIL_SAP_ACTIVE_HO:
      ds707_mode_handler_info.call_end_reason =  
        PS_NET_DOWN_REASON_TUN_CONN_SAP_ACTIVE_HO;
      break;

    /* Tunnel mode, LTE is OOS   */
    case HDRHAI_CONN_FAIL_SAP_NO_SRV:
      ds707_mode_handler_info.call_end_reason =  
        PS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SRV;
      break;

    /* Invalid Tunnel call configuration */
    case DS707_S101_DOWN_REASON_CALL_INVALID_CONFIG:
      ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_INTERNAL_ERROR;
      break;

    /* Tunnel call disallowed */
    case DS707_S101_DOWN_REASON_CALL_DISALLOWED:
    {
      if ( ds707_s101_is_sys_mode_lte() == FALSE)
      {
        ds707_mode_handler_info.call_end_reason =
            PS_NET_DOWN_REASON_TUN_NO_LTE_SRV;
      }
      else if ( ds707_s101_is_lcp_throttled() == TRUE )
      {
        ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_LCP_THROTTLED;
      }
      else if (ds707_s101_is_virt_conn_throttled() == TRUE )
      {
        ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_VIRTUAL_CONN_THROTTLED;
      }
      else if ( ds707_s101_is_ehrpd_mode() == FALSE )
      {
        ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_NON_EHPRD_MODE;
      }
      else if (ds707_s101_is_hdr_session_open() == FALSE)
      {
        ds707_mode_handler_info.call_end_reason =
            PS_NET_DOWN_REASON_TUN_HDR_SESSION_CLOSED;
      }
      else if ( ds707_s101_is_tunnel_mode() == FALSE )
      {
        ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_NON_TUNNEL_MODE;
      }
      else if ( ds707_s101_is_prereg_allowed() == FALSE )
      {
        ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_TUN_PREREG_DISALLOWED;
      }
      break;
    }
    /* 
     * Normal Tunnel internal call ended using 
     * PS_NET_DOWN_REASON_CLIENT_END 
     */
    case DS707_S101_DOWN_REASON_CLIENT_END:
      ds707_mode_handler_info.call_end_reason =  
            PS_NET_DOWN_REASON_CLIENT_END;
      break;

    case DS707_S101_DOWN_REASON_UNSPECIFIED:
      break;

    default: 
      break;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "Set last tun_call_end_reason: HDR/Tunnel Err Code=0x%x, "
                   "PS Err Code=0x%x",
                   end_reason, ds707_mode_handler_info.call_end_reason);

} /* ds707_pkt_mgr_set_last_tun_call_end_reason */
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_LAST_CALL_END_REASON_FROM_CALL_CMD_ERR

DESCRIPTION    This function sets the reason the last data call
               end reason based on cm call cmd err info

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
ps_iface_net_down_reason_type 
ds707_pkt_mgr_set_last_call_end_reason_from_call_cmd_err
(
  /* The call cmd error code from CM */
  cm_call_cmd_err_e_type   call_cmd_err
)
{
  ps_iface_net_down_reason_type call_end_reason = 
    PS_NET_DOWN_REASON_CLIENT_END;

  switch ( call_cmd_err )
  {
    case CM_CALL_CMD_ERR_OFFLINE_S:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_OFFLINE;
 
      break;
    }

    case CM_CALL_CMD_ERR_EMERGENCY_MODE_S:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_EMERGENCY_MODE;

      break;
    }

    case CM_CALL_CMD_ERR_IN_USE_S:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_PHONE_IN_USE;

      break;
    }

    case CM_CALL_CMD_ERR_INVALID_MODE_S:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_INVALID_MODE;

      break;
    }

    case CM_CALL_CMD_ERR_INVALID_SIM_STATE_S:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_INVALID_SIM_STATE;

      break;
    }

    case CM_CALL_CMD_ERR_NO_COLLOC_HDR:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_NO_COLLOC_HDR;

      break;
    }

    case CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED:
    {
      call_end_reason = 
        PS_NET_DOWN_REASON_CALL_CONTROL_REJECTED;

      break;
    }

    default:
    {
      call_end_reason =  
        PS_NET_DOWN_REASON_INTERNAL_ERROR;

      break;
    }
  } /* switch */

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "Received CM call cmd err as %d"
                   "Setting PS net down reason to %d",
                   call_cmd_err, call_end_reason );

  return call_end_reason;
  
}
/*===========================================================================
FUNCTION DS707_PKT_MGR_CLEAR_LAST_CALL_END_REASON

DESCRIPTION    This function clears the reason the last data call's
               origination failed (if applicable).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_clear_last_call_end_reason
(
  void
)
{
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Setting last call_end_reason to Client End (Reset)");
  ds707_mode_handler_info.call_end_reason = PS_NET_DOWN_REASON_CLIENT_END;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_EIC_END_REASON

DESCRIPTION    This function returns the ps_extended_info_code_enum_type 
               reason for phys link to go down/null

DEPENDENCIES   None.

RETURN VALUE   A value defined in ps_extended_info_code_enum_type indicating
               the reason the call was ended.

SIDE EFFECTS   None

===========================================================================*/
ps_extended_info_code_enum_type ds707_pkt_mgr_get_eic_end_reason
(
  void
)
{
  ps_extended_info_code_enum_type ps_eic_reason = PS_EIC_NOT_SPECIFIED;
  ps_iface_net_down_reason_type   call_end_reason = 
                                      PS_NET_DOWN_REASON_NOT_SPECIFIED;
  /*-----------------------------------------------------------------------*/

  call_end_reason = ds707_pkt_mgr_get_last_call_end_reason();

  switch (call_end_reason)
  {
    case PS_NET_DOWN_REASON_CLIENT_END:
      ps_eic_reason = PS_EIC_CLIENT_END;
      break;
    case PS_NET_DOWN_REASON_NO_SRV:
      ps_eic_reason = PS_EIC_NO_SRV;
      break;
    case PS_NET_DOWN_REASON_CDMA_LOCK:
      ps_eic_reason = PS_EIC_CDMA_LOCK;
      break;
    case PS_NET_DOWN_REASON_FADE:
      ps_eic_reason = PS_EIC_FADE;
      break;
    case PS_NET_DOWN_REASON_INTERCEPT:
      ps_eic_reason = PS_EIC_INTERCEPT;
      break;
    case PS_NET_DOWN_REASON_REORDER:
      ps_eic_reason = PS_EIC_REORDER;
      break;
    case PS_NET_DOWN_REASON_REL_NORMAL:
      ps_eic_reason = PS_EIC_REL_NORMAL;
      break;
    case PS_NET_DOWN_REASON_REL_SO_REJ:
      ps_eic_reason = PS_EIC_REL_SO_REJ;
      break;
    case PS_NET_DOWN_REASON_INCOM_CALL:
      ps_eic_reason = PS_EIC_INCOM_CALL;
      break;
    case PS_NET_DOWN_REASON_ALERT_STOP:
      ps_eic_reason = PS_EIC_ALERT_STOP;
      break;
    case PS_NET_DOWN_REASON_ACTIVATION:
      ps_eic_reason = PS_EIC_ACTIVATION;
      break;
    case PS_NET_DOWN_REASON_MC_ABORT:
      ps_eic_reason = PS_EIC_MC_ABORT;
      break;
    case PS_NET_DOWN_REASON_MAX_ACCESS_PROBE:
      ps_eic_reason = PS_EIC_MAX_ACCESS_PROBE;
      break;
    case PS_NET_DOWN_REASON_PSIST_NG:
      ps_eic_reason = PS_EIC_PSIST_NG;
      break;
    case PS_NET_DOWN_REASON_UIM_NOT_PRESENT:
      ps_eic_reason = PS_EIC_UIM_NOT_PRESENT;
      break;
    case PS_NET_DOWN_REASON_ACC_IN_PROG:
      ps_eic_reason = PS_EIC_ACC_IN_PROG;
      break;
    case PS_NET_DOWN_REASON_ACC_FAIL:
      ps_eic_reason = PS_EIC_ACC_FAIL;
      break;
    case PS_NET_DOWN_REASON_RETRY_ORDER:
      ps_eic_reason = PS_EIC_RETRY_ORDER;
      break;
    case PS_NET_DOWN_REASON_CCS_NOT_SUPPORTED_BY_BS:
      ps_eic_reason = PS_EIC_CCS_NOT_SUPPORTED_BY_BS;
      break;
    case PS_NET_DOWN_REASON_NO_RESPONSE_FROM_BS:
      ps_eic_reason = PS_EIC_NO_RESPONSE_FROM_BS;
      break;
    case PS_NET_DOWN_REASON_REJECTED_BY_BS:
      ps_eic_reason = PS_EIC_REJECTED_BY_BS;
      break;
    case PS_NET_DOWN_REASON_INCOMPATIBLE:
      ps_eic_reason = PS_EIC_INCOMPATIBLE;
      break;
    case PS_NET_DOWN_REASON_ACCESS_BLOCK:
      ps_eic_reason = PS_EIC_ACCESS_BLOCK;
      break;
    case PS_NET_DOWN_REASON_ALREADY_IN_TC:
      ps_eic_reason = PS_EIC_ALREADY_IN_TC;
      break;
    case PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_GPS:
      ps_eic_reason = PS_EIC_USER_CALL_ORIG_DURING_GPS;
      break;
    case PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_SMS:
      ps_eic_reason = PS_EIC_USER_CALL_ORIG_DURING_SMS;
      break;
    case PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_VOICE_CALL:
      ps_eic_reason = PS_EIC_USER_CALL_ORIG_DURING_VOICE_CALL;
      break;
    case PS_NET_DOWN_REASON_REDIR_OR_HANDOFF:
      ps_eic_reason = PS_EIC_REDIR_OR_HANDOFF;
      break;
    case PS_NET_DOWN_REASON_ACCESS_BLOCK_ALL:
      ps_eic_reason = PS_EIC_ACCESS_BLOCK_ALL;
      break;
    case PS_NET_DOWN_REASON_IS707B_MAX_ACC:
      ps_eic_reason = PS_EIC_IS707B_MAX_ACC;
      break;
    case PS_NET_DOWN_REASON_NO_CDMA_SRV:
      ps_eic_reason = PS_EIC_NO_CDMA_SRV;
      break;
    case PS_NET_DOWN_REASON_CD_GEN_OR_BUSY:
      ps_eic_reason = PS_EIC_CD_GEN_OR_BUSY;
      break;
    case PS_NET_DOWN_REASON_CD_BILL_OR_AUTH:
      ps_eic_reason = PS_EIC_CD_BILL_OR_AUTH;
      break;
    case PS_NET_DOWN_REASON_CHG_HDR:
      ps_eic_reason = PS_EIC_CHG_HDR;
      break;
    case PS_NET_DOWN_REASON_EXIT_HDR:
      ps_eic_reason = PS_EIC_EXIT_HDR;
      break;
    case PS_NET_DOWN_REASON_HDR_NO_SESSION:
      ps_eic_reason = PS_EIC_HDR_NO_SESSION;
      break;
    case PS_NET_DOWN_REASON_COLLOC_ACQ_FAIL:
      ps_eic_reason = PS_EIC_COLLOC_ACQ_FAIL;
      break;
    case PS_NET_DOWN_REASON_HDR_ORIG_DURING_GPS_FIX:
      ps_eic_reason = PS_EIC_HDR_ORIG_DURING_GPS_FIX;
      break;
    case PS_NET_DOWN_REASON_HDR_CS_TIMEOUT:
      ps_eic_reason = PS_EIC_HDR_CS_TIMEOUT;
      break;
    case PS_NET_DOWN_REASON_HDR_RELEASED_BY_CM:
      ps_eic_reason = PS_EIC_HDR_RELEASED_BY_CM;
      break;
    case PS_NET_DOWN_REASON_OTASP_COMMIT_IN_PROG:
      ps_eic_reason = PS_EIC_OTASP_COMMIT_IN_PROG;
      break;
    case PS_NET_DOWN_REASON_NO_HYBR_HDR_SRV:
      ps_eic_reason = PS_EIC_NO_HYBR_HDR_SRV;
      break;
    case PS_NET_DOWN_REASON_HDR_NO_LOCK_GRANTED:
      ps_eic_reason = PS_EIC_HDR_NO_LOCK_GRANTED;
      break;
    case PS_NET_DOWN_REASON_HOLD_OTHER_IN_PROG:
      ps_eic_reason = PS_EIC_HOLD_OTHER_IN_PROG;
      break;
    case PS_NET_DOWN_REASON_HDR_FADE:
      ps_eic_reason = PS_EIC_HDR_FADE;
      break;
    case PS_NET_DOWN_REASON_HDR_ACC_FAIL:
      ps_eic_reason = PS_EIC_HDR_ACC_FAIL;
      break;
    case PS_NET_DOWN_REASON_THERMAL_EMERGENCY:
      ps_eic_reason = PS_EIC_THERMAL_EMERGENCY;
      break;
    case PS_NET_DOWN_REASON_CALL_ORIG_THROTTLED:
      ps_eic_reason = PS_EIC_CALL_ORIG_THROTTLED;
      break;
    case PS_NET_DOWN_REASON_OFFLINE:
      ps_eic_reason = PS_EIC_OFFLINE;
      break;
    case PS_NET_DOWN_REASON_EMERGENCY_MODE:
      ps_eic_reason = PS_EIC_EMERGENCY_MODE;
      break;
    case PS_NET_DOWN_REASON_PHONE_IN_USE:
      ps_eic_reason = PS_EIC_PHONE_IN_USE;
      break;
    case PS_NET_DOWN_REASON_INVALID_MODE:
      ps_eic_reason = PS_EIC_INVALID_MODE;
      break;
    case PS_NET_DOWN_REASON_INVALID_SIM_STATE:
      ps_eic_reason = PS_EIC_INVALID_SIM_STATE;
      break;
    case PS_NET_DOWN_REASON_NO_COLLOC_HDR:
      ps_eic_reason = PS_EIC_NO_COLLOC_HDR;
      break;
    case PS_NET_DOWN_REASON_CALL_CONTROL_REJECTED:
      ps_eic_reason = PS_EIC_CALL_CONTROL_REJECTED;
      break;
     default:
      break; 
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "call_end_reason: %d mapped to ps_eic reason:%d", 
                   call_end_reason, ps_eic_reason );
  return ps_eic_reason;
}

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_CURRENT_HYBRID_PREF

DESCRIPTION    This function returns the current hybrid pref for AT.

DEPENDENCIES   None.

RETURN VALUE   TRUE - If AT is in Hybrid ON mode
               

SIDE EFFECTS   None

===========================================================================*/
boolean ds707_pkt_mgr_get_current_hybrid_pref
(
  void
)
{
  return ds707_cm_hybrid_pref;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_CURRENT_HYBRID_PREF

DESCRIPTION    This function sets the hybrid pref mode 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_current_hybrid_pref
(
  boolean hybrid_pref
)
{
  ds707_cm_hybrid_pref = hybrid_pref;
}
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

/*===========================================================================
FUNCTION DS707_GET_CURR_MODE_PREF

DESCRIPTION    This function returns the current phone's mode preference.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None
===========================================================================*/
nv_mode_enum_type ds707_get_curr_mode_pref
(
void
)
{
  nv_stat_enum_type status;
  nv_item_type          data_ptr;
    /*-------------------------------------------------------------------------
    Read the NV item PREF_MODE. We have to read everytime, bcos user can change the pref_mode
    at any time
  -------------------------------------------------------------------------*/
  memset((void*)&data_ptr, 0 , sizeof(nv_item_type));
  data_ptr.pref_mode.nam = (byte)0;
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_PREF_MODE_I, &data_ptr, ds707_curr_subs_id());
  if( status == NV_NOTACTIVE_S )
  {
    /*-----------------------------------------------------------------------
       If the NV item is not configured return default value
    -----------------------------------------------------------------------*/
     /* If this target supports ACP, default the mode preference
       * to automatic. Else, default mode preference to digital only.
       */
    #ifdef FEATURE_ACP
    data_ptr.pref_mode.mode = NV_MODE_AUTOMATIC;
    #else
    data_ptr.pref_mode.mode = NV_MODE_DIGITAL_ONLY;
    #endif
  }
  else if( status != NV_DONE_S)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad NV read status %d", status);
    return NV_MODE_AUTOMATIC;
  }
  return(data_ptr.pref_mode.mode);
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_CHECK_DCM_JCDMA_AVAILABILITY

DESCRIPTION    This function determines whether to allow or deny 
               further processing.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
boolean ds707_pkt_mgr_check_dcm_jcdma_allowed
(
  void
)
{
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  return TRUE;
#endif /* #ifndef FEATURE_DEDICATED_C2K_MODE_DATA */

  if (!ds707_roaming_is_curr_mode_jcdma())
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_CURRENT_SYS_MODE

DESCRIPTION    This function sets the current mode.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_current_sys_mode
(
  sys_sys_mode_e_type sys_mode
)
{
  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "ds707_pkt_mgr_set_current_sys_mode() "
                   "old mode: %d, new mode: %d", 
                   ds707_current_sys_mode, sys_mode );

  ds707_current_sys_mode = sys_mode;
/* commenting here, called from idm change hdlr */
//  ds707_sys_chg_hdlr_sys_mode_updated(sys_mode);
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_CURRENT_SYS_MODE

DESCRIPTION    This function returns the current mode.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
sys_sys_mode_e_type ds707_pkt_mgr_get_current_sys_mode
(
  void
)
{

  return ds707_current_sys_mode;
}

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS707_PKT_MGR_MMGSDI_UPDATE

DESCRIPTION    Update corresponding 1x CDMA services

DEPENDENCIES   None.

RETURN VALUE   TURE: corresponding 1x update is successful
               FALSE: corresponding 1x udpate fails

SIDE EFFECTS   None

===========================================================================*/
boolean ds707_pkt_mgr_mmgsdi_update
(
  ds707_pkt_mgr_mmgsdi_update_enum_type mmgsdi_update_type
)
{
  boolean pass_fail = FALSE;

  switch ( mmgsdi_update_type )
  {
    case DS707_MMGSDI_CDMA_NV_UPDATE:
    
#ifdef FEATURE_DATA_IS707
      /* Initialize NV items for CDMA */
      ds707_pkt_mgr_nv_sync();
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DS_MOBILE_IP
      /* Read MIP related NV items present in EF MMGSDI_CDMA_3GPD_MIPUPP */
      dsmip_read_mip_nv_items();
#endif /* FEATURE_DS_MOBILE_IP */ 
      pass_fail = TRUE;
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_MMGSDI_CDMA_3GPD_3GPDOPM_UPDATE:
      /*-------------------------------------------------------------------------
        Get Mobile IP preference setting from NV.
      -------------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_pkt_mgr_mmgsdi_update() getting NV_DS_QCMIP_I" );

#ifdef FEATURE_DS_AT_TEST_ONLY
      /* if ds_qcmip_update is called with TRUE
       * it will read the QCMIP value from NV and update 
       * the global variable ds_qcmip_val. 
       */
      pass_fail = ds_qcmip_update( TRUE );
      DATA_IS707_MSG1( MSG_LEGACY_MED, "ds_qcmip_update() returned %d",
                       (int)pass_fail );
#endif /* FEATURE_DS_AT_TEST_ONLY */
      break;

    case DS707_MMGSDI_CDMA_3GPD_MIPSP_UPDATE:
      /*-------------------------------------------------------------------------
        Get Mobile IP active profile from NV.
        -------------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "ds707_pkt_mgr_mmgsdi_update getting NV_DS_MIP_ACTIVE_PROF_I");
      /* if ds_qcmipp_update is called with FALSE
       * it will read the QCMIPP value from NV and update 
       * the global variable ds_qcmipp_val. 
       */
      pass_fail = ds_qcmipp_update( FALSE );
      DATA_IS707_MSG1( MSG_LEGACY_MED, "ds_qcmipp_update() returned %d",
                       (int)pass_fail );
      break;
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_MMGSDI_CDMA_PROFILE_UPDATE:
      /*-------------------------------------------------------------------------
        Read Profile parameters from RUIM when it gets intialized
      -------------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "ds707_pkt_mgr_mmgsdi_update() reading profile from RUIM" );
      ds707_read_data_session_profile_from_nvruim();
      pass_fail = TRUE;
      break;

    case DS707_MMGSDI_CDMA_AT_CMD_INIT:
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_pkt_mgr_mmgsdi_update() initializes AT cmds" );
      ds707_pkt_mgr_nv_sync();
      ds707_pkt_mgr_qcmip_set_crm_val();
      ds707_pkt_mgr_qcmip_set_mdr_val();
      ds707_so_pkt_recal();
      dsatcmdp_init_config();
      pass_fail = TRUE;
      break;

  default:
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "ds707_pkt_mgr_mmgsdi_update() Invalid Parameter %d", 
                       mmgsdi_update_type );
    break;
  }
  return pass_fail;

} /* ds707_pkt_mgr_mmgsdi_update */

 
/*===========================================================================
FUNCTION DS707_PKT_MGR_NV_REFRESH

DESCRIPTION    NV REFRESH Event occurred so we need to read all the stored 
               NVs and EFS paths 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_nv_refresh(sys_modem_as_id_e_type subs_id)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "NV Refresh Event occurred as part of hotswap. Read all NVs for subs_id %d",subs_id);

  if( subs_id != SYS_MODEM_AS_ID_1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NV Refresh received on Sub 2");
  }
  else
  {
    curr_subs_id = subs_id;
    ds707_nv_util_nvrefresh(curr_subs_id); 

    /*Debug Mesg*/ 
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                    "Nv refresh done for subs_id %d",
                    subs_id);
  }

  ds3geventmgr_set_and_notify_nv_refresh_status(DS3GEVENTMGR_CLIENT_ID_3GPP2);

  return;
} /* ds707_pkt_mgr_nv_refresh */


/*==================================================================================
FUNCTION       DS707_CURR_SUBS_ID

DESCRIPTION    Return the current subs id in a global variable
 
DEPENDENCIES   None.

RETURN VALUE   Return the current subs id 
              (sys_modem_as_id_e_type) 

SIDE EFFECTS   None

=====================================================================================*/
sys_modem_as_id_e_type ds707_curr_subs_id()
{
   /*-------------------------------------------------------------------------
   Return the current subs id  
   -------------------------------------------------------------------------*/
  return curr_subs_id;
}/* ds707_curr_subs_id */


/*===========================================================================
FUNCTION DS707_PKT_MGR_MMGSDI_RESET

DESCRIPTION    Card removed so resetting corresponding 1x CDMA data

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_mmgsdi_reset(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "Process Card refreshed/ removed ev. Tear down all calls");
        
  ds707_pkt_mgr_graceful_ppp_termination(PS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT);

  ds707_reset_data_session_profile_from_nvruim();

} /* ds707_pkt_mgr_mmgsdi_reset */

/*===========================================================================
  FUNCTION DS707_MMGSDI_RESPONSE_CB()

  DESCRIPTION
    Callback function called by mmgsdi to return response

  PARAMETERS
    status  : return status
    cnf     : type of cnf response
    cnf_ptr : response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_mmgsdi_response_cb
(
  mmgsdi_return_enum_type  status, 
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type *  cnf_ptr
)
{
  if( status != MMGSDI_SUCCESS )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "MMGSDI ERROR");
    return;
  }

  ASSERT( cnf_ptr != NULL );

  if( cnf == MMGSDI_JCDMA_GET_CARD_INFO_CNF )
  {
    ds707_jcdma_card_info = cnf_ptr->get_jcdma_card_info_cnf.card_info;
  }
} /* ds_ucsd_mmgsdi_response_cb */

/*===========================================================================
FUNCTION DS707_GET_JCDMA_CARD_INFO()

DESCRIPTION
  Returns the JCDMA crad info.

PARAMETERS
  None

RETURN VALUE
  MMGSDI_JCDMA_GRAY_CARD      : If card is a JCDMA card.
  MMGSDI_JCDMA_NON_JCDMA_CARD : If not a JCDMA crad.


DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_jcdma_card_info_enum_type
ds707_get_jcdma_card_info(void)
{
  return ds707_jcdma_card_info;
} /* ds707_get_jcdma_card_info */

/*===========================================================================
FUNCTION DS707_READ_JCDMA_CARD_INFO()

DESCRIPTION
  Reads the JCDMA crad info.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds707_read_jcdma_card_info
(
  mmgsdi_session_id_type session_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_MAX_RETURN_ENUM;

  if( (mmgsdi_status = ds3g_mmgsdi_session_jcdma_get_card_info(
                            session_id,
                            ds707_mmgsdi_response_cb,
                            (mmgsdi_client_data_type)0)) != MMGSDI_SUCCESS )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "mmgsdi_session_jcdma_get_card_info returned error. "
                    "Status = %d", mmgsdi_status);
  }
} /* ds707_read_jcdma_card_info */

/*===========================================================================
FUNCTION DS707_INVALIDATE_JCDMA_INFO()

DESCRIPTION
  Invalidates JCDMA card info because some card error occured.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds707_invalidate_jcdma_card_info( void )
{
  ds707_jcdma_card_info = MMGSDI_JCDMA_NON_JCDMA_CARD;
} /* ds707_invalidate_jcdma_card_info */

#endif /* FEATURE_MMGSDI_SESSION_LIB */


/*===========================================================================

FUNCTION       DS707_PKT_MGR_GET_MTU_SIZE_FROM_NV

DESCRIPTION    This function reads the NV items and provides the MTU size. 
               If the NV is inactive, the default values will be provided.
        
DEPENDENCIES   

RETURN VALUE    NONE

SIDE EFFECTS   None.
===========================================================================*/
uint16 ds707_pkt_mgr_get_mtu_size_from_nv(void)
{
  uint16 mtu = 0;
  boolean is_ehrpd_call = FALSE;
  char item_file_path[] = "/nv/item_files/data/1x/707/ds707_ehrpd_mtu";
  char item_file_path_hrpd[] = "/nv/item_files/data/1x/707/ds707_hrpd_mtu";
  nv_stat_enum_type               retval;

  if ( ds707_pkt_is_ehrpd_mode_of_operation() )
  {
    is_ehrpd_call = TRUE;
  }
  if( TRUE == is_ehrpd_call )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "eHRPD call, read eHRPD NV");
    mtu = EHRPD_MTU_SIZE;

    /*-----------------------------------------------------------------------
      Read item file into mtu. Return of 0 from ds3gcfgmgr_read_efs_nv_ex implies
      success. If item file does not exist write default value to it.
    -----------------------------------------------------------------------*/ 
    retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path, &mtu, sizeof(mtu), ds707_curr_subs_id()); 
    if ( 0 != retval )
    {
      mtu = EHRPD_MTU_SIZE;
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_ehrpd_mtu EFS item not present, using defaults");
    }

    DATA_IS707_MSG1( MSG_LEGACY_HIGH, "eHRPD call MTU = %d", mtu);
  }/*is_ehrpd_call*/
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "not eHRPD call, read HRPD NV");
    mtu = HRPD_MTU_SIZE;  

    /*-----------------------------------------------------------------------
      Read item file into mtu. Return of 0 from ds3gcfgmgr_read_efs_nv_ex implies
      success. If item file does not exist write default value to it.
    -----------------------------------------------------------------------*/ 
    retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path_hrpd, &mtu, sizeof(mtu), ds707_curr_subs_id()); 
    if ( 0 != retval )
    {
      mtu = HRPD_MTU_SIZE;
        DATA_IS707_MSG0(MSG_LEGACY_MED,
                      "ds707_hrpd_mtu EFS item not present, using defaults");
    }
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, "HRPD call MTU = %d", mtu);
  }/* NOT an ehrpd call*/
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_pkt_mgr_get_mtu_size_from_nv, MTU = %d", mtu );
  return mtu;
}

/*===========================================================================
FUNCTION      DS707_GET_LAST_BEARER_TECH

DESCRIPTION   Returns the struct containing the last bearer tech

DEPENDENCIES  None.

RETURN VALUE  ps_iface_bearer_technology_type

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_bearer_technology_type ds707_get_last_bearer_tech(void)
{
  return ds707_last_bearer_tech;
}

/*===========================================================================
FUNCTION      DS707_SET_LAST_BEARER_TECH

DESCRIPTION   Sets the struct containing the last bearer tech

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_last_bearer_tech(
  ps_iface_bearer_technology_type bearer_tech
)
{
  ds707_last_bearer_tech = bearer_tech;
}

/*===========================================================================
FUNCTION      DSHDR_GET_MOBILE_IP_USES_CCOA

DESCRIPTION   Returns boolean dshdr_mobile_ip_uses_ccoa

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/
boolean dshdr_get_mobile_ip_uses_ccoa(void)
{
  return dshdr_mobile_ip_uses_ccoa;
}

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION      DS707_GET_MAX_PPP_HARD_FAILURES

DESCRIPTION   Returns value of ehrpd_to_hrpd_fb_config.max_hard_fallback_count

DEPENDENCIES  None.

RETURN VALUE  uint8

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds707_get_max_ppp_hard_failures(void)
{
  return ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.max_hard_fallback_count;
}

/*===========================================================================
FUNCTION      DS707_GET_MAX_PPP_SOFT_FAILURES

DESCRIPTION   Returns value of ehrpd_to_hrpd_fb_config.max_soft_fallback_count

DEPENDENCIES  None.

RETURN VALUE  uint8

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds707_get_max_ppp_soft_failures(void)
{
  return ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.max_soft_fallback_count;
}

/*===========================================================================
FUNCTION      DS707_SET_PPP_FAILURE_FLAG

DESCRIPTION   Sets ehrpd_to_hrpd_fb_cb.ppp_failed flag. 
              This function should be called when
              max of hard or soft ppp failures reached.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_ppp_failure_flag(boolean ppp_failed)
{
  ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb.ppp_failed = ppp_failed;
}

/*===========================================================================
FUNCTION      DS707_SET_ROAM_FLAG

DESCRIPTION   Sets ehrpd_to_hrpd_fb_cb.is_roam. 
              This function should be called when
              device moves into roaming network.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_roam_flag(boolean is_roam)
{
  ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb.is_roam = is_roam;
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Roam set to %d",is_roam);
}

/*===========================================================================
FUNCTION      DS707_IS_EHRPD_TO_HRPD_FB_ENABLED

DESCRIPTION   Returns boolean ehrpd_to_hrpd_fb_config.enabled

DEPENDENCIES  None.

RETURN VALUE  booolean

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_ehrpd_to_hrpd_fb_enabled(void)
{
  return ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.enabled;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_LTE_ATTACH_SUCESS_IND

DESCRIPTION   This function indicates DS that LTE attach is successful and 
              DS can inform HDR layer to propose eHRPD next time when the
              device acquires DO signal, required for eHRPD to HRPD
              fallback feature.
 
PARAMETERS    None
 
DEPENDENCIES  None.
 
RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_lte_attach_success_ind(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------- 
   For eHRPD to HRPD fallback: When UE moves to LTE, indicate HDR to propose
   eHRPD when UE moves back to DO network, only if the list of mandatory APNs 
   are enabled.
  -----------------------------------------------------------------------*/ 
  if ( TRUE == ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.propose_ehrpd_after_irat )
  {
    if ( TRUE == ds707_data_session_get_ehrpd_attach_allowed() )
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "LTE attach success, inform HDR & to propose eHRPD");
      hdrpac_lte_attach_success_ind();
      hdrpac_set_data_ehrpd_capability( TRUE );
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "LTE attach success: "
                                       "Inform HDR to propose HRPD");
      hdrpac_set_data_ehrpd_capability( FALSE );
    }
  }
} /* ds707_pkt_mgr_lte_attach_success_ind */

#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

/*===========================================================================
FUNCTION ds707_pkt_mgr_get_dns_v4_addr_req_type

DESCRIPTION
  Returns the request method in which to obtain the DNS IPv4 address during
  VSNCP negotiation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  0 : None
  1 : IPCP packet in PCO
  2 : DNS v4 Request Container in PCO
  3 : Both methods

SIDE EFFECTS
  None

===========================================================================*/
vsncp_3gpp2i_dns_v4_addr_req_type ds707_pkt_mgr_get_dns_v4_addr_req_type(void)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "ds707_pkt_mgr_dns_v4_addr_req: %d",
                   ds707_pkt_mgr_dns_v4_addr_req);
  return ds707_pkt_mgr_dns_v4_addr_req;
}
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_IS_EHRPD_CAPABILITY_ALLOWED

DESCRIPTION   This function checks following flags and decide whether 
              eHRPD capability is allowed or not. This function should be called
              by hdrpacdsif.c

DEPENDENCIES  None.

RETURN VALUE  boolean 
  TRUE  - eHRPD allowed. 
  FALSE - eHRPD not allowed. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_ehrpd_capability_allowed(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
  if (ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.enabled)
  {
/* 
 * In LTE full service, when HDR query eHRPD capability, we shall not pass 
 * info regading LCP/EAP failure and roaming info. Only APN allowed
 * info shall be checked and passed.
 */
    if ( ( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_LTE ) &&
         ( ds3g_get_current_srv_status_ex(SYS_MODEM_AS_ID_1)
                                                == SYS_SRV_STATUS_SRV ) )
    {
      return ds707_data_session_get_ehrpd_attach_allowed();
    }
    else if ( ds707_pkt_mgr_ehrpd_to_hrpd_fb_config.is_ehrpd_roaming_disabled == TRUE)
    {
      return ( (ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb.is_roam == FALSE) &&
               (ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb.ppp_failed == FALSE) &&
               (ds707_data_session_get_ehrpd_attach_allowed()) );

    }
    else
    {
      return ( ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb.ppp_failed == FALSE && 
               ds707_data_session_get_ehrpd_attach_allowed() );
    }
  }
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
  return ds707_data_session_get_ehrpd_attach_allowed();
#else
  return FALSE;
#endif /* FEATURE_EHRPD */
} /* ds707_is_ehrpd_capability_allowed */

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION DS707_IS_POLICY_HANDOFF_CLASS_EPC

DESCRIPTION
  Util function in to check if the passed in policy's handoff_class
  is of EPC class

DEPENDENCIES
  None.

PARAMETERS
  policy_info_ptr  pointer to policy info data structure.

RETURN VALUE
  TRUE - If Handoff Class is EPC
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_is_policy_handoff_class_epc
(
  acl_policy_info_type *policy_info_ptr
)
{
  if (policy_info_ptr != NULL &&
      (policy_info_ptr->handoff_ctxt.handoff_class ==
                                          PS_IFACE_HANDOFF_CLASS_EPC
#ifdef FEATURE_DATA_OPTHO
       || policy_info_ptr->handoff_ctxt.handoff_class ==
                                          PS_IFACE_HANDOFF_CLASS_EPC_PREREG
#endif /* FEATURE_DATA_OPTHO */
      )
     )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "ds707_is_policy_handoff_class_epc: "
                     "Policy handoff class Set to EPC");
    return TRUE;
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "ds707_is_policy_handoff_class_epc: "
                     "Policy handoff class not set to EPC");
    return FALSE;
  }

} /* ds707_is_policy_handoff_class_epc */
#endif /* FEATURE_EPC_HANDOFF */
#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_PKT_MGR_CLEAR_PDN_FB_COUNT

DESCRIPTION
  This function clears the PDN failure count
PARAMETERS    

DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS None
===========================================================================*/
void ds707_pkt_mgr_clear_pdn_fb_count(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED, "ds707_pkt_mgr_clear_pdn_fb_count"); 
  ds707_fb_pdn_failure_count = 0;
}
/*===========================================================================
FUNCTION      DS707_PKT_MGR_FB_LIST_FALLBACK

DESCRIPTION
  This function let HDR fallback to HRPD when detects the input apn is in
  the fallback list and the failure count has reached the max count configured
  in NV.

PARAMETERS    

DEPENDENCIES  

RETURN VALUE  TRUE: DID fallback 
              FALSE: did NOT fallback 

SIDE EFFECTS None
===========================================================================*/
boolean ds707_pkt_mgr_fb_list_fallback(
                                   ps_iface_type *ps_iface_ptr,
                                   const char  *apn_string,
                                   const uint8 apn_len
                                )
{
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;

  pdncntx_cb_ptr = 
    ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( (ps_iface_type*)ps_iface_ptr );
  if(NULL == pdncntx_cb_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, 
       "ds707_pkt_mgr_fb_list_fallback: invalid pdncntx_cb_ptr"); 
    return FALSE;
  }

  if(ds707_pdncntx_get_pdn_fb_cnt_added(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, 
       "ds707_pkt_mgr_fb_list_fallback: pdn_fb_cnt_added flag set,"
       "do NOT increase fb cnt."); 
    return FALSE;
  }

  if(ds707_is_ehrpd_to_hrpd_fb_enabled())
  {
    if(
       ds707_max_fb_pdn_failure_count!=0 
#ifndef TEST_FRAMEWORK
       && ds707_data_session_is_in_fallback_list(apn_string,apn_len)
#endif /* TEST_FRAMEWORK */
       )
    {
      ds707_fb_pdn_failure_count++;
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
         "ds707_pkt_mgr_fb_list_fallback: count increased to %d",
         ds707_fb_pdn_failure_count); 
      ds707_pdncntx_set_pdn_fb_cnt_added(pdncntx_cb_ptr,TRUE);
      if(ds707_fb_pdn_failure_count == 
              ds707_max_fb_pdn_failure_count)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, "PDN in fallback pdn list fails, " 
                        "Cleanup all the calls & fallback to HRPD");
        ds707_pkt_mgr_graceful_ppp_termination(
                                  PS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK);
        return TRUE;
      }
    }
  }
  return FALSE;
} /*ds707_pkt_mgr_fb_list_fallback*/
#endif /*FEATURE_DATA_FALLBACK_LIST*/

/*===========================================================================

FUNCTION       DS707_PKT_MGR_IS_ALLOW_FALLBACK_TO_SIP

DESCRIPTION    This function is used to judge whether fallback to sip when 
               mip failure happens 
        
DEPENDENCIES   

RETURN VALUE    TRUE: can fallback 
                FALSE: cannot fallback 

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_is_allow_fallback_to_sip(
                                  mip_fail_reason_e_type  mip_status,
                                  byte               mip_rrp_code
                                  )
{
  if(mip_status == MIP_FAIL_REASON_HA_ADMIN_PROHIBIT)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
     "MIP_FAIL_REASON_HA_ADMIN_PROHIBIT, not allowed to fallback to sip");
    return FALSE;
  }
#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
  if(ds707_pkt_mgr_mip_can_do_sess_close(mip_rrp_code))
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "mip rrp code 67, not allowed to fallback to sip");
    return FALSE;
  }
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/
  return TRUE;
}
/*===========================================================================

FUNCTION       DS707_PKT_MGR_ALL_DIAL_STR_DO_PKT_CALL

DESCRIPTION    This function is used to judge whether all dial string should 
               do pkt call when the dial string is passed to ds707_async_mgr.c 
        
DEPENDENCIES   

RETURN VALUE    TRUE: do packet call anyway
                FALSE: otherwise

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_all_dial_str_do_pkt_call(
                                              const byte *dial_str
                                              )
{
  boolean    do_pkt_call = FALSE;
  const byte *s_ptr = NULL;
  uint8      dial_string_len = 0;
  uint8      efs_str_len = 0;
  uint8      i = 0;

  if(NULL == dial_str)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
     "ds707_pkt_mgr_all_dial_str_do_pkt_call, null string pointer input");
    return FALSE;
  }
  /*Simplified JCDMA:
  5.1.10 ATD(T)*99**24# invokes the hybrid packet call  [D2-1-No.57]*/
  if(ds707_roaming_is_curr_mode_jcdma()||
     ds707_roaming_is_simplified_jcdma_mode())
  {

    /*------------------------------------------------------------------------- 
    If dial string equals to the provisioned additional dial string in EFS or
    M511 indicates PKT in NV, do packet call for all dial string
    -------------------------------------------------------------------------*/
    if(ds707_jcdma_m51_make_pkt_call())
    {
      do_pkt_call = TRUE;
    }
    else
    {   
      /*---------------------------------------------------------------------
          Count the length of the input dial string.
      ---------------------------------------------------------------------*/
      s_ptr = dial_str;

      while( *s_ptr++ != '\0' )
      {
        dial_string_len++;
      }
      if(dial_string_len>DS707_ADDITIONAL_DIAL_STR_LEN)
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
           "ds707_pkt_mgr_all_dial_str_do_pkt_call, input string too long");
        return FALSE;
      }

      for(i=0; i<DS707_ADDITIONAL_DIAL_STR_NUM; i++)
      {
        /*---------------------------------------------------------------------
          Count the length of the dial string provisioned in EFS.
        ---------------------------------------------------------------------*/
	      efs_str_len = 0;
        s_ptr = ds707_pkt_mgr_additional_dial_str[i];
        while( *s_ptr++ != '\0' &&
               efs_str_len<DS707_ADDITIONAL_DIAL_STR_LEN )
        {
          efs_str_len++;
        }

        if(dial_string_len == efs_str_len)
        {
          if(0 ==
             memcmp(ds707_pkt_mgr_additional_dial_str[i], 
                    dial_str, efs_str_len))
          {
            do_pkt_call = TRUE;
            break;
          }
        }
      }
    }
  }
  return do_pkt_call;
}
/*===========================================================================
FUNCTION DS707_PKT_MGR_IGNORE_LAST_PDN_IA_TIMER

DESCRIPTION
  returns whether to ignore the last PDN inactivity timer

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - to ignore
  FALSE - to proceed

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_pkt_mgr_ignore_last_pdn_ia_timer(void)
{
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707_pkt_mgr_ignore_last_pdn_ia_timer: %d",
                   ds707_ignore_last_pdn_ia_timer);
  return ds707_ignore_last_pdn_ia_timer;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_TEAR_DOWN_ALL

DESCRIPTION   To tear down all active call 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_tear_down_all
(
  void *         tear_down_info_ptr
)
{
  uint8 iface_index = 0;
  ps_iface_type  *iface_ptr = NULL;

  for ( iface_index = 0; 
        iface_index < DS707_MAX_IFACES; 
        iface_index++ )
  {      
    iface_ptr = NULL ;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(iface_index);
    if ( (ps_iface_state(iface_ptr) != IFACE_DOWN) && 
         (ps_iface_state(iface_ptr) != IFACE_STATE_INVALID) &&
         (ps_iface_state(iface_ptr) != IFACE_DISABLED) )
    {
      DATA_IS707_MSG2( MSG_LEGACY_MED, 
                       "ds707_pkt_mgr_tear_down_all, Tearing down iface:%x:%d",
                       iface_ptr,
                       iface_ptr->instance);
      (void)ds707_tear_down_cmd(iface_ptr, tear_down_info_ptr);
    }
  }                                                  
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_END_REASON_FOR_ALL_IFACES

DESCRIPTION   To set call end reason for all valid ifaces

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_set_end_reason_for_all_ifaces
(
  ps_iface_net_down_reason_type call_end_reason
)
{
  uint8  curr_iface_index = 0;
  ps_iface_type                *iface_ptr           = NULL;
  ds707_iface_table_type       *ds707_iface_tbl_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------------
    Set call end reason for all the valid ifaces
  ---------------------------------------------------------------------------*/
  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES; 
        curr_iface_index++ )
  {      
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    ds707_iface_tbl_ptr = ds707_get_iface_tbl_from_instance(curr_iface_index);

    if (DS707_PKT_MGR_IS_PS_IFACE_ACTIVE(iface_ptr))
    {
      if (ps_iface_addr_family_is_v4(iface_ptr))
      {
        dssnet4_set_ps_call_end_reason(
                      ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr), 
                      call_end_reason);
                               
      }

      else if (ps_iface_addr_family_is_v6(iface_ptr))
      {
        dssnet6_set_ps_call_end_reason(
                      ds707_pdncntx_get_dssnet6_sm_ptr(ds707_iface_tbl_ptr), 
                      call_end_reason);
                               
      }
    }
  } /* for each iface */ 

  return;
} /* ds707_pkt_mgr_set_end_reason_for_all_ifaces() */

/*===========================================================================
  FUNCTION DS_3GPP2_UPDATE_DATA_CONFIG_INFO
  
  DESCRIPTION
    Puts all the required nv item file paths in the data_config_info.conf 
    file. Would be called from dsutils during powerup.
    
  PARAMETERS  
    file_paths_buf: To hold the efs item file paths
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    0 for success.
    -1 for failure.
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
int32 ds_3gpp2_update_data_config_info
(
  char  *file_paths_buf
)
{
/*-------------------------------------------------------------------------*/
  if (NULL == file_paths_buf)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_ERROR, "file_paths_buf is NULL");
    return -1;
  }

  /* Copy current path to buffer */
  memscpy(file_paths_buf, strlen(nv_item_file_path), 
          nv_item_file_path, strlen(nv_item_file_path));
  return 0;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_BP_PARAMS

DESCRIPTION   This function sets the BP paramaters value to the specified 
              values.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_set_bp_params(boolean is_enabled, uint32 timerVal)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_back_pressure_params.data.enabled   = is_enabled;  
  ds707_back_pressure_params.data.timer_val = timerVal; 
     
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "ds707_pkt_mgr_enabled_bp = %d, "
                  "ds707_pkt_mgr_bp_timer_val = %d",
                  ds707_back_pressure_params.data.enabled,
                  ds707_back_pressure_params.data.timer_val);
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_3GPP2_EFS_ITEM_FILE_PATH_SIZE

DESCRIPTION   Returns the total 3gpp2 EFS item file paths size

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_pkt_mgr_3gpp2_efs_item_file_path_size
(
  void
)
{
  return sizeof(nv_item_file_path);
} /* ds707_pkt_mgr_3gpp2_efs_item_file_path_size() */

/*===========================================================================
FUNCTION       DS707_PKT_MGR_INCR_NUM_MIP_TO_SIP_TRANSITIONS

DESCRIPTION    This function increments the number of MIP to SIP transitions that 
               happened from power on till another power cycle is initiated. 
               This is used mainly for statistics purpose

DEPENDENCIES   None.

RETURN VALUE   none
 
SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_mgr_incr_num_mip_to_sip_transitions(void)
{
  num_mip_to_sip_transitions++ ;
} /* ds707_pkt_mgr_incr_num_mip_to_sip_transitions() */

/*===========================================================================
FUNCTION       DS707_PKT_MGR_GET_NUM_MIP_TO_SIP_TRANSITIONS

DESCRIPTION    This function gives the number of MIP to SIP transitions that 
               happened from power on till another power cycle is initiated. 
               This is used mainly for statistics purpose

DEPENDENCIES   None.

RETURN VALUE   uint32 - No. of MIP to SIP transitions till a 
                        reset/powercycle is initiated.
 
SIDE EFFECTS   None

===========================================================================*/
uint32 ds707_pkt_mgr_get_num_mip_to_sip_transitions(void)
{
  return num_mip_to_sip_transitions;
} /* ds707_pkt_mgr_get_num_mip_to_sip_transitions() */

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_CDMA_RAT_INFO

DESCRIPTION
  This function returns the RAT and SO masks for 1x.
  
PARAMETERS  
  rat_mask         - Pointer to rat_mask field
  so_mask          - Pointer to so_mask field
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - TRUE if RAT is valid and ESPM indiaction is received.
            FALSE otherwise.
==========================================================================*/
boolean ds707_pkt_mgr_get_cdma_rat_info
(
  uint32  *rat_mask,
  uint32  *so_mask
)
{
  boolean  ret_val = FALSE;
  boolean  espm_ind;
  byte     p_rev = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rat_mask == NULL || so_mask == NULL)
  { 
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Null params");
    return ret_val;
  }

  espm_ind = ds707_get_espm_ind_recvd();

  if ( espm_ind == TRUE )
  {
    *rat_mask = PS_IFACE_CDMA_1X;    

    /* Get the p_rev in use from 1x CP. */
    p_rev = mc_get_p_rev_in_use();

    *so_mask = ds707_get_so_mask_from_p_rev(p_rev);
    ret_val = TRUE;
  }
  else
  {
    *rat_mask = PS_IFACE_CDMA_NULL_BEARER;
    *so_mask = 0;
    ret_val  = FALSE;
  }  

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "In ds707_pkt_mgr_get_cdma_rat_info(), p_rev in use: %d, "
                   "so mask : %d, return %d", 
                   p_rev, *so_mask, ret_val );
  return ret_val;
}

/*===========================================================================
FUNCTION DS707_REG_UNREG_DSM_MEM_EVENT

DESCRIPTION
  This function registers/de-registers DS707's callback function for DSM memory
  events for DSM_MEM_LEVEL_LINK_LAYER_MANY and 
  DSM_MEM_LEVEL_LINK_LAYER_FEW.
  
PARAMETERS  
  boolean - flag to indicate whether the function call is for 
  registration or deregistration.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_reg_unreg_dsm_mem_event
(
  boolean reg_flag
)
{
  static boolean already_reg = FALSE;

  if( reg_flag == TRUE && already_reg == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Register for DSM memory events.");
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_LINK_LAYER_MANY,
                            DS707_DSM_SMALL_ITEM_MANY_MARK);

    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_LINK_LAYER_FEW,
                            DS707_DSM_SMALL_ITEM_FEW_MARK);
  
    dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_LINK_LAYER_MANY,
                         DSM_MEM_OP_FREE,
                         ds707_dsm_mem_event_cb);

    dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_LINK_LAYER_FEW,
                         DSM_MEM_OP_NEW,
                         ds707_dsm_mem_event_cb);
    already_reg = TRUE;
  }
  else if( reg_flag == FALSE && already_reg == TRUE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Deregister DSM memory events.");
    dsm_unreg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_LINK_LAYER_MANY,
                           DSM_MEM_OP_FREE);

    dsm_unreg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_LINK_LAYER_FEW,
                           DSM_MEM_OP_NEW);
    ds707_dsm_fc_flag = FALSE; 
    already_reg = FALSE;
  }

  return;
}

/*===========================================================================
FUNCTION DS707_DSM_MEM_EVENT_CB

DESCRIPTION
  This function is DS707's callback function for DSM memory events for
  DSM_MEM_LEVEL_LINK_LAYER_MANY and DSM_MEM_LEVEL_LINK_LAYER_FEW. This will 
  enable and disable flow control according to different DSM memory levels.
  
PARAMETERS  
  dsm_mem_pool_id_enum type   pool_id     -  Memory pool ID
  dsm_mempool_level_enum_type level       -  Memory Level
  dsm_mem_op_enum_type        mem_op      -  Type of memory operation
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_dsm_mem_event_cb
(
  dsm_mempool_id_enum_type pool_id,
  dsm_mem_level_enum_type  level,
  dsm_mem_op_enum_type     mem_op
)
{
  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "ds707_dsm_mem_event_cb called for level = %d "
                  "and ds707_dsm_fc_flag = %d",
                  level, ds707_dsm_fc_flag);
  if (level == DSM_MEM_LEVEL_LINK_LAYER_MANY)
  {
    if ( ds707_dsm_fc_flag == TRUE )
    {
      ds707_dsm_fc_disable();
      ds707_dsm_fc_flag = FALSE;
    }
  }
  else if (level == DSM_MEM_LEVEL_LINK_LAYER_FEW)
  {
    if ( ds707_dsm_fc_flag == FALSE )
    {
      ds707_dsm_fc_flag = TRUE;

      ps_timer_cancel(ds707_back_pressure_params.tmr_hndl_dyn_back_pressure);
      ds707_back_pressure_params.tcp_rev_link_ack_counter = 0;
      ds707_back_pressure_params.last_tcp_rev_link_ack_count = 0;

      ds707_dsm_fc_enable();
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Sanity check.  We should not have received notification for anything
      but the Many and Few Items Left memory events.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Callback called for unregistered memory level %d", level);
  }

  return;
}

/*===========================================================================
FUNCTION DS707_GET_DSM_FC_FLAG

DESCRIPTION
  This function returns the value for ds707_dsm_fc_flag.
  ds707_dsm_fc_flag is a global variable which indicates whether flow control
  is enabled or disabled depending on the DSM memory events.
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Boolean - Current value of ds707_dsm_fc_flag.
==========================================================================*/
boolean ds707_get_dsm_fc_flag(void)
{
  return ds707_dsm_fc_flag;
}

/*===========================================================================
FUNCTION DS707_IS_BP_REMOVAL_ENABLED

DESCRIPTION
  This function returns the flag value which indicates whether back pressure
  removal is enabled or disabled.
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Boolean - TRUE if back pressure removal is enabled
            FALSE otherwise
==========================================================================*/
boolean ds707_is_bp_removal_enabled(void)
{
  return ds707_back_pressure_params.data.enabled;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_LOCAL_ABORT

DESCRIPTION   Handles Local abort for NV refresh. Currently used in 
                        nv refresh event case.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_local_abort
(
  uint32 subs_id,
  ps_iface_net_down_reason_type end_reason
)
{
  uint8                   curr_iface_index = 0;
  ps_iface_type          *iface_ptr = NULL; 
  boolean                 iface_in_use = FALSE;
#ifdef FEATURE_EHRPD
  ds707_devmgr_stop_reason  devmgr_reason = DS707_DEVMGR_STOP_COMPLETE;
#endif /* FEATURE_EHRPD */
  ps_iface_tear_down_client_data_info_type tear_down_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&tear_down_info,0,sizeof(tear_down_info));
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Local abort PPP termination");
        
     
#ifdef FEATURE_EHRPD        
  /*-------------------------------------------------------------------------- 
    Disable partial context, so that devmgr just tears down the call instead 
    of maintaining partial context state (if enabled).
  ---------------------------------------------------------------------------*/
  ds707_devmgr_disable_partial_context();
#endif /* FEATURE_EHRPD */
  
  /*--------------------------------------------------------------------- 
    tear down all the UP ifaces
  ---------------------------------------------------------------------*/
  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES; 
        curr_iface_index++ )
  {      
    iface_ptr = NULL;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if ( PS_IFACE_IS_IN_USE(iface_ptr) 
	#if 0 
		&& (PS_IFACE_GET_SUBSID(iface_ptr)== subs_id)
	#endif 
	   )
    {
      DATA_IS707_MSG3( MSG_LEGACY_MED,
                       "processing local abort PPP termination, Tearing down iface:0x%x::%d,"
                       "end reason: %d",
                       iface_ptr->name, iface_ptr->instance, end_reason);
      tear_down_info.validity_mask |= PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      tear_down_info.data_info.call_end_reason = end_reason;
      tear_down_info.validity_mask |= PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK;
      tear_down_info.data_info.local_abort = TRUE;
      (void)ds707_tear_down_cmd_processor(iface_ptr, (void *)(&tear_down_info), TRUE);
      iface_in_use = TRUE;
    }
  }
  
#ifdef FEATURE_EHRPD
  /*--------------------------------------------------------------------- 
    If no iface is in USE, then stop devmgr.
  ---------------------------------------------------------------------*/
  if (FALSE == iface_in_use)
  {
    devmgr_reason = ds707_devmgr_stop();
    if (DS707_DEVMGR_STOP_FAIL == devmgr_reason)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_stop() failed");
    }
  }
#endif /* FEATURE_EHRPD */
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_TRAFFIC_CH_TEARDOWN_EFS

DESCRIPTION   This function initializes the Data Services Mode Specific
              Handler interface.  It initializes the critical section
              used for call operations. 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_traffic_ch_teardown_efs( void )
{
  nv_stat_enum_type   status;
  boolean             ds_traffic_channel_teardown = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Read traffic channel behaviour NV setting when iface is in LINGERING.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_efs_nv_ex(
                          ds_nv_path_traffic_channel_teardown_in_lingering,
                          (void *)&ds_traffic_channel_teardown,
                          sizeof(boolean), ds707_curr_subs_id()
                         );

  if (status != NV_DONE_S)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "NV item: NV_TRAFFIC_CHANNEL_ACTION_IN_LINGERING was not set"
                    "Reverting to default behaviour");
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "NV item: NV_TRAFFIC_CHANNEL_ACTION_IN_LINGERING was set: %d",
                    ds_traffic_channel_teardown);
  }

  ds_set_traffic_channel_teardown_in_lingering_val(ds_traffic_channel_teardown);
} /* ds707_pkt_mgr_traffic_ch_teardown_efs() */


#endif /* FEATURE_DATA_IS707 */
