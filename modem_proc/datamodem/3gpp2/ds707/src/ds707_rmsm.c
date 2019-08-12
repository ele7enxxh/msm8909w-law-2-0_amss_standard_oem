/*===========================================================================

                          D S 7 0 7 _ R M S M . C
DESCRIPTION

  The Data Services IS707 Rm State Machine source file. This state machine
  manages the Um and Rm interfaces for certain IS707 packet calls.
  Specifically RMSM is used to support traditional Relay, SIP Network Model
  and Mobile IP type IS707 calls which are a special case of the
  generic multimode PS architecture.

EXTERNALIZED FUNCTIONS
  ds707_rmsm_init()
    Initialize the Rm State Machine.
  ds707_rmsm_post_event()
    Post an event to Rm State Machine.
  ds707_rmsm_process_cmd()  
    Process a cmd posted to Rm State Machine.
  ds707_rmsm_is_packet_call()
    Is RM in IS707 laptop pkt data call.
  ds707_rmsm_is_in_network_model_call()
    Is RM in IS707 laptop network model pkt data call.
  ds707_rmsm_dial_str_cb()
    Dial string handler for IS707 pkt data calls.
  ds707_rmsm_notify_ppp_up
    Notifies RMSM that end to end PPP is up for SIP network model call.

Copyright (c) 2001-2015 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_rmsm.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
12/15/15    sc     Added code to bring down traffic channel if call ended  
                   before phys link up
11/18/14    skc    Removing FEATURE_UW_FMC
07/02/14    sd     Interfacing with CFCM for CPU and Thermal based flow
                   control events.
12/23/13    vm     Stack memory optimization changes
10/28/13    sd     LLVM Compiler Warnings clean-up - 3GPP2
07/26/13    sc     Unbridge only non NULL sio and 707 ifaces when transitioning
                   to DS707_RMSM_NULL_STATE
07/09/13    sd     Handling WAITING_FOR_ENTITLEMENT_CHECK_STATE for RMSM
                   Rm Iface down event.
06/03/13    sd     ATCoP decoupling changes
03/01/13    ssb    Moving to IFACE_UP state before IFACE_ROUTEABLE State 
                   for Relay calls
11/08/12    ssb    Dun Call Entitlement support using PS_SYS Interface
11/02/12    ssb    Updating MIP to SIP Fallback Stats for DUN calls
05/25/12    msh    Remove write wmk values to NV
05/03/12    jee    To fix embedded/DUN failure
05/01/12    jee    Fix MIP to SIP fallback failure
04/30/12    ms     Cancel the aggregation timer while moving Rmsm to NULL state.
04/19/12    sb     Feature Cleanup: Always On Flag FEATURE_UIM_SUPPORT_3GPD
02/08/12    sk     Feature cleanup.
01/16/12    ack    Fix to post a cmd for iface bringup/teardown
01/02/12    msh    Coan: Feature cleanup 
09/01/11    jee    To fix Arbitration issue with v4/v6 calls
11/18/11    vpk    HDR SU API cleanup
09/01/11    msh    Modified feature to read wmk levels from NV 
09/26/11    msh    Fix compiler warnings
09/24/11    ms     Dont process tear down cmd if rmsm state is in NULL state. 
08/29/11    vpk    Fix for DUN call arbitration
08/26/11    ms     Fixed compiler warnings
08/22/11    ms     Added feature to read wmk levels from NV 
08/20/11    ttv    GSDI to MMGSDI_SESSION api migration changes.
08/15/11    ms     Fix graceful tear down in LPM
07/29/11    vpk    Add funtion to support memory flow control logging   
07/21/11    vpk    Fix for closing other interfaces during DUN call bringup
06/24/11    scb    Added support for EFS NV item is_fmc_enabled.
06/15/11    ack    Fix to check for RMSM state to be up while stopping PPP in
                   ds707_rmsm_pkt_tear_down_cmd
06/15/11    ack    Deregister phy link events with old iface when falling back 
                   from MIP to SIP
05/27/11    ls     Split packet to a small DSM chain if it is relay model call
                   and transferring the data
05/20/11    ls     When relay model call is up, it shall set the flag. When
                   next call comes, ds707_rt_acl() shall check the flag and
                   reject the call.
03/29/11    ack    Fix to gracefully tear down calls before phone 
                   goes to LPM mode
03/23/11    op     Data SU API cleanup
03/15/11    mg     Global variable cleanup
03/04/11    jee    Merged compiler warning fixes
03/04/11    ls     Fixed CW
02/26/11    ls     Global variable cleanup
02/25/11    ms     Fixed compiler error for JCDMA build.
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/11/11    ack    Global Variable Cleanup
01/06/11    scb    Changes for UW FMC Feature
01/18/11    ms     Send Term-Ack if a Term-Req is recieved from PC while cleaning PPP
                   on Rm.
12/14/10    ls     Fixed featurization issue
12/07/10    sn     Fix to read RM NAI (multiple NAI) only if phone is in NV 
                   mode.
12/07/10    ls     Make RMSM SIO Rev WM configurable
12/07/10    ms     For Relay model calls do the bringup first directly.
11/29/10    ms     Register for Phys link UP event for Relay model calls. 
11/04/10    mg     On phys link up, check if call is allowed in current config. 
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
10/12/10    vs     Remove q_init for the watermark queue as it is already 
                   initialized inside dsm_queue_init.
07/28/10   ttv     Merged the fix for the issue of not reading MIP active 
                   profile index from NV item NV_DS_MIP_ACTIVE_PROF_I during
                   call establishment.
06/25/10    vs     Moving the constants defining watermark levels
                   from ds707_rmsm.c to ds707_rmsm.h
06/09/10    ls     When UM/RM PPP option is mismatched, MH will bring down 
                   MIP instead of tearing down UM iface directly
05/17/10    ls     Modify CCM featurization
04/19/10    gc     klocwork fixes
04/16/10    ls     Fix the featurization issues
04/08/10    ls     DUN SIP call shall also post DS707_SYS_INFO_NO_SYSTEM when 
                   tearing down the call or DS707_SYS_INFO_NON_EPC_SYSTEM
                   when call is connected
03/10/10    ls     Use ps_iface_go_null_cmd() instead of ps_iface_tear_down_cmd()
                   to ensure that iface is torn down. Also, when iface ptr is NULL,
                   it shall clean up and send signal to PC side
02/05/10    ss     Remove q_init for the watermark queue as it is already 
                   initialized inside dsm_queue_init.
12/31/09    ls     Added Rev-B Rate support
12/21/09    spn    Remove Initialization of q_ptr within dsm_queue as it is
                   redundant. 
12/10/09    ps     Mainlining OMH feature based on new profile management
                   design.
11/10/09    as     Reduced RMSM timer timeout from 10 ms to 5 ms
10/23/09    ca     Fixed Lint Error.
10/22/09    ss     Initializing dsm queues sio_rev_wmk_list and sio_fwd_wmk_q.
                   Adding a utility function to get the RMSM state required for 
                   QTF test cases.
17/08/09    ps     Fixed the issue of INET call failure when current app
                   is in Dormant2 state.
07/29/09    ls     Update FEATURE_RMSM_DATA_AGG_TIMER provided by WM team
07/27/09    spn    Changed the function to get the hdr_active_prot
07/02/09    ps     Added changes to reset RMSM pdn instance for MIP to SIP
                   fallback scenario.
05/19/09    ss     Fixed Off Target Lint Medium warnings.
04/29/09    ms     Fixed Lint High Errors.
03/16/09    ls     In ds707_rmsmi_transition_state(), if it is JCDMA mode and 
                   the failed reason is not DS707_ORIG_FAIL_CALL_THROTTLE, 
                   it will send NO CARRIER to PC.
03/10/09    ls     Increased dont_exceed_cnt, so DNE-Hi WM has 35K difference
                   on SIO RX WM
02/15/09    ls     Initialize epzid in ds3g_pkt_call_info_type.
02/09/09    spn    Block RMSM Calls in EHRPD Mode of operation
02/09/09    spn    Moved RMSM PDN instance to the RMSM Module
02/09/09    ss     Off target Lint Fix
01/28/2009  ls     In ds707_rmsmi_process_um_iface_down_ev(), if current 
                   state is in DS707_RMSM_WAITING_FOR_UM_DOWN_STATE, it will
                   check if ppp(RM) is already down or not(or not started 
                   yet). If it is,
                   it shall change the state to DS707_RMSM_NULL_STATE state. 
                   Otherwise, change the state to 
                   DS707_RMSM_WAITING_FOR_RM_DOWN_STATE
01/20/2009  ls     Support FEATURE_RMSM_DATA_AGG_TIMER to starts a timer to 
                   collect (aggregate) data every time the watermark goes 
                   from empty to non-empty.  Whenever the timer expires or 
                   the high watermark is hit, the collected data is flushed 
                   from ARM9 to ARM11
12/12/08    ms     Lint fix
11/11/08    ms     WM/RLP trash is avoided on receiving 
                   IFACE_CONFIGIGURING_EV when
                   the AT is in WAITING_FOR_UM_UP_MIP_STATE
10/27/08   ua      Extending the "connect rate" for 1x data calls. Supported
                   rates for 1x RTT, HDR REV0, HDR REVA.
23/10/08    psng   Fixed compiler errors on JCDMA build.
10/13/08    cs     Klocwork Fixes.
09/27/08    pp     Metainfo optimizations.
09/03/08    sc     Fix to disable RM PPP auth on dual stack phone for a mip4
                   call
07/18/08    spn    Increased Watermark levels when the Enh Revb Rates are 
                   supported
06/27/08    rt     Added support for IWLAN_3GPP2 and 1x-v4 interface mutual
                   exclusion.
06/17/08    sn     Support for multiple app profiles for OMH.
06/17/08    psng   Added changes in "ds707_rmsmi_get_um_iface_ptr" to include
                   the case where iface is in IFACE_GOING_DOWN state 
                   whenever routing look up succeeds.
06/04/08    spn    Featurization of watermark levels
06/04/08    hs     Added the macro FEATURE_DSM_WM_CB needed to define the 
                   correct watermark function pointer signature.
05/29/08    sc     Changes to clean up the V4 and V6 ifaces properly
04/22/08   mga/as  Renamed DS707_RMSM_WAITING_FOR_RM_DOWN_MIP_STATE to 
                   DS707_RMSM_WAITING_FOR_RM_DOWN_STATE. Made changes 
                   to transition to DS707_RMSM_WAITING_FOR_RM_DOWN_STATE 
                   while bringing down the call.
03/07/08    ac     Added F3 messages for RM watermark status
02/26/08  mga/ack  Made changes to register for callbacks to bring up and 
                   teardown IS707_PKT_IFACE only if in a Network model call,
                   in ds707_rmsmi_transition_state. 
01/22/08    scb/sy Mainlined features under FEATURE_JCDMA.
01/10/08    da     Modified fwd link memory based flow control to not make
                   rpc call to modem side and instead flow control SMD.
01/03/08    gr     Changes to move Um iface state for Relay model calls to 
                   handle CHUM based DO session tear down upon Um iface down 
11/20/07    ms     Fixed compiler errors
11/06/07 mga/sq/as In ds707_rmsmi_process_rm_iface_down_ev if in 
                   DS707_RMSM_E2E_PPP_UP_SIP_STATE call ppp stop if phys link
                   on UM is up or iface is set to graceful close.
                   Restructured the code added ds707_cleanup_um function     
09/19/07    ms     Changed all ps_iface_up_ind to ps_iface_routeable_ind for 
                   RM link.For all laptop calls the state of the Rm iface
                   (SIO_IFACE) should be ROUTEABLE
09/17/07 as/sn/mga Changed snoop_reg_event to netmdl_snoop_reg in
                   ds707_rmsmi_transition_state 
09/13/07    sc     Changed ppp_update_ipcp_config to ppp_update_proto_config
                   for IPCP as well as LCP
07/03/07    squ    Increase SIO RX WMK lo, hi, dne values to support HS USB
07/02/07    sc     Added support for Application Profiles and Dormant2 (Iface
                   Lingering) features.
07/02/07   ak/az   Added code to reject data calls if card color is Gray
06/19/07    as     Added error check for MIP->SIP fallback to allow teardown
06/13/07   ak/az   Modified the tear down process for V4 and V6 ifaces
05/10/07    ac     Fixed Lint error
04/04/07   as/sq   Fixed Lint error and banned apis
03/29/07    as     transition to WAITING_FOR_E2E_PPP_UP when recieve 
                   RM_RESYNC_EV in RESYNCING_UM_SIP state
03/08/07    ara    Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
01/18/07    ar     Select automatic mode on ds3g_siolib_allocate_port().
12/01/06    rsj    Added code to view mode handler information global 
                   structure and if CM flags the call has been throttled,
                   then send out
		               DSAT_DELAYED response code for JCMDA JATE requirement.
11/13/06    ks     Moved ds707_rmsm_get_nv_rm_nai() to mip.
10/31/06    rsj    Added new function ds707_rmsm_is_relay_model_call() to 
                   indicate if a relay model call is active.
11/10/06    as     event_info.link_state is deprecated so changed to
                   event_info.phys_link_event_info.state
17/10/06    rsj    Added fix for JCDMA 2 dial string comparison.
10/13/06   as/gr   Added check to ds707_rmsmi_sio_tx_rlp_data to only send 
                   rlp data if connect has been sent to Apps processor.
10/13/05    az     Support for IPv6. No change was needed though. Only 
                   comments were added.
10/09/06    as     Added a transition to WAITING_FOR_UM_UP_RELAY_STATE when
                   rmsm wants packet call and some cleanup.
09/28/06    kvd    Added new function ds707_rmsm_get_sio_iface() to access RM
                   iface.
09/11/06    as     added function ds707_rmsm_is_rm_iface_up to be able to 
                   check rm iface state.
09/11/06    as     removed report success to DCTM while MIP->SIP fallback
08/30/06    as     Updated the fix for stripping off the meta info.
08/09/06    as     Changed ds707_rmsmi_sio_tx_ppp_data to strip off meta info
                   pushed down by ps task before passing data to SIO.
07/13/06    gr     changes to support simultaneous laptop and gps
28/06/06    sq/gr  added dctm 2.0 feature support.
13/06/06    sq/gr Fixed SIOLib errors that introduced in the
                   FEATURE_DATA_SERIALIZER
05/26/06    kvd    Set bring_up flag in acl policy to TRUE before route_get()
5/18/06     sq/gr  When RMSM state change to unll, replace ppp_stop to 
                   ppp_abort if dsat_amp_c_val = 0.
05/10/06    vp     Changes for FEATURE_DATA_SERIALIZER
2/27/06     gr     In function ds707_rmsmi_process_um_iface_down_ev, dereg 
                   IFACE_CONFIGURING_EV incase MIP fall back to SIP
2/15/06     sq     1. Added processing PS_FLOW_TX events. 
                   2. Change hi/lo WM level.
1/27/06     gr/sq  When RMSM state transfers to 
                   DS707_RMSM_RESYNCING_UM_MIP_STATE,
                   set PPP to internal mode.
1/16/06     sq     In ds707_rmsmi_pppum_event_handler_cb function, in case 
                   PPP_DOWN_EV, moved posting DS707_RMSM_UM_PPP_DOWN_EV code 
                   to the the end of case block. 
1/16/06     gr     Register for IFACE configuring event with the Um iface to 
                   handle PPP resync scnenarioregistered. 
01/13/06    ksu    EMPA: relay model works for Flow 0x00, Route A *only*
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
11/14/05    gr     Changes to throttle the um link when the rm link is slower
                   than the um link (specifically the BT scenario)
10/24/05    kvd/as  Post ppp_success event on PPP_UP.
10/03/05    kvd    fixed error with handling the PHY_LINK_GONE event
09/10/05    gr     Fixed the issue of ppp_resync_flag reset when the phys
                   link goes down. Now flag is reset after resync procedure
08/30/05    ks     Removed FEAUTRE_DATA_PS_DATA_LOGGING_FLOW feature around 
                   flow logging and mainlined in under the DPL feature, 
                   FEATURE_DATA_PS_DATA_LOGGING.
08/16/05    kvd    Moved reg_ppp_init_cbs() to WAITING_FOR_RM_UP state.
                   Fixed a bewk in previous checkin.
08/05/05   sk/vs   Merged the HDR Semi-Dynamic QOS changes from sandbox.
07/22/05    kvd    add fail_reason to ppp_ev callbacks, report call fail to
                   DCTM.
04/27/05    vrk    Registered for LCP down events from PPP engine (UM side)
06/14/05    kvd    Dereg for iface events before transitining to 
                   UM_UP_RELAY-SIP on Fallback.
04/29/05    vrk    Added more events to enable automated testing
04/26/05    vrk    Created events to enable automated testing
05/16/05    ks     made temp fixes for HDR_QOS to compile with PS.10.00.00 VU
05/10/05    kvd    Issue go_null_cmd() on 707_pkt_iface before iface_down_ind
04/20/05    mct    Flow changes. Take references to phys_link down out.
03/02/05    ks     Added call to ps_iface_dpl_set_iface_desc() to set the
                   iface description.
02/25/05    kvd    COpy ds_mip_rm_nai intop mip session info before sending
                   RRQ for DS_MULTIPLE_NAI feature.
02/05/05    rsl    Moved cta timer start and stop on tx side to dsrlp.c from 
                   ds707_rmsm.c
01/20/05    mct    Fixed RMSM to register for phys_link flow events. Added
                   new function rmsmi_707_pkt_phys_link_event_handler_cb to
                   process phys_link events.
01/12/05    ks     Added support for Data Protocol Logging.
12/03/04    kvd    Changed the check for is_in_network_model_call for BCMCS
                   to work for MIP calls also.
11/22/04    vas    Call phys link ioctl QOS Release instead of iface ioctl
11/16/04    rsl    Removed TASKLOCK and TASKFREE around route_get().
10/28/04    ks     Passing zeros as arguments to ppp_set_v4_server_opts() for
                   primary and secondary nbns addresses as WINS is not
                   supported for 1x.
10/25/04    vas    If phys link ptr is NULL, do not attempt to clean up QOS
                   Change to accept a filter with no Protocol specified 
                   (only IP)

10/14/04    ifk    Added meta info ** to snoop callback functions
09/24/04    jd     Use ppp dependent PPP_RM_RX_SIGNAL instead of PS task
                   dependent PS_PPP_RM_RX_SIGNAL
09/20/04    vas    Merge from Sept Dev including QOS changes and ppp 
                   interface changes
09/15/04    kvd    Featurized TRY_MIP label in process_rm_wants_pkt_call_ev.
09/09/04    kvd    Unbridge IS707_iface & sio_iface on going NULL.
08/02/04    kvd    Reset mip_config on going to NULL state.
07/21/04    kvd    Updated processing of RM_IFACE_DOWN, UM_PHYS_LINK/IFACE_UP
                   events while in SIP_WAITING_FOR_PHYS_LINK_STABLE state.
07/20/04    kvd    On fallback to SIP, wait till phys link comes down before
                   attempting the SIP call, if phys link is in GOING_DOWN 
                   state Added a new state SIP_WAITING_FOR_PHYS_LINK_STABLE
                   for this.
06/24/04    vr     Set Um and Rm to INTERNAL_SNOOP instead of INTERNAL if QoS
                   or BCMCS is supported. Enable flow on the Um iface when
                   Rm goes to NULL state.
06/22/04    kvd    Call mip_config_session in rmsm itself, instead of waiting
                   till dssnet ppp_open, and fallback/tear-down based on 
                   qcmip value, if config_session fails.
06/16/04    vr     Set Um and Rm to INTERNAL mode instead of FULL_NET if QoS
                   or BCMCS is supported. Also, did some cleanup to setup the
                   bridging between Rm and Um properly.
06/09/04    vr     Fixed compile error with FEATURE_BCMCS and FEATURE_HDR_QOS
                   undefined
06/08/04    vr     Setup bridging from Um to Rm iface if QOS or BCMCS is
                   supported
06/08/04    kvd    Do the route lookup and get ptr to iface again on fallback
                   for the iface to be in correct state to make the SIP call.
05/05/04    kvd    Give iface_configuring_ind on 707 iface while UM PPP is
                   resyncing.
05/25/04    vr     Fixed featurization issue with FEATURE_BCMCS
04/30/04    vr     Added ds707_rmsm_is_in_network_model_call().
                   Changed PSEUDO_NET to FULL_NET for BCMCS/QoS
05/03/04    gr     Added ppp_abort for Um interface (for NETMODEL scenario) 
                   This was initially done in pkt mgr. 
04/22/04    ak     Lint changes.
04/07/04    gr     Added functionality to process dial string to identify if
                   the service type has to be CDMA only. Also modified calls 
                   to bring up iface to pass the user data with the 
                   information regarding service type(CDMA only or Automatic)
03/23/03    mct    Passing NULL into route_get call in
                   ds707_rmsmi_get_um_iface_ptr, removed 
                   acl_ipfilter_info_type
03/02/04    kvd    Removed redundant MIP featurization in addr_changed_ev()
02/25/04    kvd    Teardown RM call on IP address change event for MIP calls
02/25/04    kvd    tear_down um before transitioning to WAITING_FOR_UM_DOWN
                   from WAITING_FOR_PHYS_LINK_UP state on RM_IFACE_DOWN
02/19/04    ak     Call macro to get dssnet4 SM.
02/13/04    ak     Updated to #59.
02/05/04    usb    Removed iface_ptr as an argument to init fn. Obtained
                   through routing. Removed unused variables/functions.
02/03/04    aku    Added support for logical ifaces
02/02/04    jd     Decoupled mipio commands.
                   Fixed MIP_PERF total call setup reporting.
01/07/04    kvd    Added a new state, WAITING_FOR_PHYS_LINK_UP and the
                   corresponding state transitions to rmsm state m/c for
                   correct SIP fallback.
12/29/03    usb    For Relay calls, change Um and RM iface states 
                   appropriately as soon as traffic channel is up.
12/26/03    usb    Handling for new ps_iface state CONFIGURING.
11/07/03    vas    For a HDR or 1X USB relay mode call optimize code so 
                   RLP/USB link bypasses the regular SIO insterfaces. Achived
                   by registering the USB queues/functions in place of the 
                   SIO queues/function while setting up the link
11/07/03    aku    Added abort callback to handle call aborts for SIP Laptop 
                   calls
11/06/03    jd     Trash RLP tx queue when PPP resync occurs to avoid sending
                   IP traffic on link before NCP is configured.
10/31/03    usb    Added definition for ds707_rmsm_notify_ppp_up().
10/31/03    usb    Removed disabling of 1X ACLs for SIP calls. Routing takes 
                   care of preventing sockets from originating 
                   simultaneously.
10/20/03    snn    Removed checks for FLOW Control before setting the 
                   watermark levels
10/13/03    vas    Changed watermarks size for AT to TE to be infinite.
09/18/03    vas    dsrlp_init moved to pkt_mgr. RLP init when traffic channel
                   is connected. Based on #44
08/18/03    jd     If forcing Um resync and call to ppp_resync() fails,
                   end call by going to WAITING_FOR_UM_DOWN state.
08/15/03    aku    Made changes to support phys_link separation from ps_iface
08/13/03    ifk    rx_iface_ptr moved to ipcp_info in ppp_dev_opts_type,
                   ppp_reg_event() protocol argument and returns it in cb.
08/08/03    rsl    Support for DSM 2.0
07/20/03    usb    Removed ACL post proc function (which should not reside 
                   here), modified ds707_rmsm_is_iface_in_use().
07/15/03    usb    Starting Rm PPP in passive mode for MIP calls.
07/14/03    vas    Temporarily commented out code to use the HDR optimization
                   (usbdc_pkt_dispatch_hdr_fl_data) based on if the device 
                   is UART or USB. 
07/04/03    usb    Resetting SIO iface IP address after Rm PPP comes up.
06/04/03    usb    Appropriately setting SIO iface and phys link state.
05/21/03    vas    Added support for setting up SIO/PPP <->RLPwatermarks for
                   HDR
05/05/03    usb    Added generic DSM memory mask to enable flow at the end
                   of a call. cleaned up process_rm_wants_pkt_call().
                   ppp_get_dev_setup() is only called if Um and Rm options
                   mismatch during MIP call setup.
05/05/03    usb    Incorporated Code review comments. Moved state enum and
                   RMSM info block from .h file. Moved all state 
                   machine specific variables into info_block. Improved
                   error handling.                   
05/02/03    usb    ps_iface_create() prototype change.
04/11/03    ak     Updated all vars/functions to include ds707_, as there
                   were naming collisions with GPRS.
04/07/03    usb    Renamed ppp_set_resync_opts() to ppp_update_ipcp_config()
03/10/03    jd/usb Renegotiate DNS addresses on Um PPP resync
02/02/03    usb    Updated snooop callback functions with new return values
01/30/03    usb    For relay calls, reset PPP mode at the end of the call,
                   moved rmsm state change call from tear_down_cb to process
                   event functions.
01/20/03    ar     Added dsat_dial_modifier_info_type to 
                   ds707_rmsm_dial_str_cb interface.
11/25/02    usb    Removed code to extract/free meta_info_ptr. Should be 
                   added later when RM sockets are suported.
11/19/02    usb    Added ACL processing and post processing functions to 
                   check if UM iface is in use, added flow event handling 
                   for relay calls
11/14/02    jd/usb Added mobile IP support
                   Optimize call failure scenarios to avoid extraneous events
                   If rm_wants_pkt_call_ev fails before state transition, 
                   dereg any reg'd callbacks.
11/14/02           Renamed RMSM_WAITING_FOR_RM_UM_DOWN_STATE to
                   RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE
11/14/02    jd     Removed um_mip_changed, added rmsm_is_packet_call()
11/13/02    usb    Miscellaneous fixes - Renamed RLP wms, revised flow 
                   control, fixed flow control masks being used, added 
                   enabling/disabling of ACLs to disallow sockets call.
09/30/02    atp    Added support for Release-A. (Changed wmk count settings).
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/18/02    mvl    Added user_data to ppp event callbacks.
09/18/02    usb    Miscellaneous fixes, fixed all TODO's, added bring_up_cmd
                   for 707 iface, better error handling, cleaned up PPP 
                   configuration sequence to fix some edge conditions,
                   removed FEATURE_DS_NET_MODEL, featurized the file
09/11/02    usb    Fixed bug so that orig_tear_down_cmd_cb for 1X is always 
                   restored properly at the end of a call.
09/10/02    ak     Updated for new dial string handler.
09/09/02    ak     Updated sio_func_table for at&D=0 dtr callback.
09/06/02    usb    Updated PPP event handlers prototype
09/06/02    ak     Always clean up rx_queue when going NULL.  Always set
                   RLP to NULL defaults when going NULL.
09/06/02    ak     on sio_change_mode, had watermarks backwards.
09/03/02    ak     Updated for ps_iface cback buf interface.  Added esc_seq
                   handler to sio func table, to handle dtr change when 
                   at&D=1.
08/28/02    usb    PPP down event handling on Um and Rm to handle failures 
                   during PPP resync
08/20/02    ak     Updated for multiple watermarks.
08/13/02    mvl    updated ps_iface state macros for name change. Changed
                   call to ppp_set_mode() as name changed. 
07/30/02    usb    Cleaned up call up termination code to make sure Rm
                   call cleanup happens only after Um
07/25/02    mvl    Updates for PPP renaming.
07/24/02    usb    Handling errors from PPP configuration
07/23/02    usb    Call termination edge case fixes
07/12/02    usb    Actually creating and deleting the SIO iface now
07/12/02    usb    Changes per the new PPP interface
07/11/02    usb    Renamed flow control constants
07/03/02    ak     Change errno's to ps_iface funcs from uint16 -> int16.
07/01/02    ak     Now first chng_mode to PKT and then send CONNECT.
05/14/02    usb    Re-created module (original filename - dsmip_rmsm.c)
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
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "queue.h"

#ifdef FEATURE_DATA_WMK_CONFIG
  #include "ds707_wmk_lvl.h"
#endif

#include "ds707_rmsm.h"
#include "ps_iface.h"
#include "dsat707extctab.h"
#include "ds3gsiolib.h"
#include "ds707_pdn_context.h"
#include "ds707_pkt_mgr.h"
#include "ds3gcfgmgr.h"
#include "ds707_pkt_mgri.h"
#include "ds707_cta_rlp.h"
#include "rdevmap.h"
#include "ds_flow_control.h"
#include "ps_phys_link.h"
#include "ds707.h"
#include "dserrno.h"
#include "dstaski.h"
#include "ps_route_iface.h"
#include "ps_route.h"
#include "ps_svc.h"
#include "ds707_netmodel.h"
#include "ps_ppp_netmodel.h"
#include "ps_ppp.h"
#include "ps_flowi_event.h"
#include "ps_iface_defs.h"
#include "ps_meta_info.h"
#include "ds707_data_session_profile.h"
#include "ps_flow.h"
#include "ps_crit_sect.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "ps_mip_task.h"
#include "dsat707mipctab.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp_defs.h"
#include "dsmip_v.h"
#include "dsmip_io.h"
#include "ds707_iface4_hdlr.h"
#include "ds707_event_defs.h"
#include "ds707_pkt_mgr_iface.h"
#include "dsmip_metasm.h"

#ifdef FEATURE_DS_MULTIPLE_NAI
#include "ps_utils.h"
#include "nv.h"
#endif /* FEATURE_DS_MULTIPLE_NAI */

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#endif /* FEATURE_DS_MOBILE_IP */

#include "dsat707util.h"
#include "mmgsdilib.h"
#ifdef FEATURE_HDR
#include "hdrds.h"
#include "hdrpac.h"
#include "hdrrlp.h"
#include "hdrcom_api.h"
#include "dsm.h"
#include "ds707_pkt_mgr.h" 
#endif /* FEATURE_HDR */

#include "ps_iface_logging.h"
#include "ps_logging_defs.h"
#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_EHRPD
#include "ds707_sec_pkt_mgr_util.h"
#endif /* FEATURE_EHRPD */

#include "ds3gtimer.h"
#include "ds_1x_profile.h"
#include "ds707_roaming.h"
#include "data_err_suppr.h"
#include "modem_mem.h"

#include "dsatctab.h"
#define DEFAULT_RMSM_AGG_TIMER_TIMEOUT 5
#define RMSM_AGG_TIMER_NV_INDEX 1
static timer_type ds707_smd_notify_delay_timer = { 0, };
static uint16 ds707_ds_smem_tx_delay = 0; //timeout for smd_notify_delay_timer, read from nv in ds707_rmsmi_setup_sio_watermarks

#include "ds707_sys_chg_hdlr.h"
#if defined(FEATURE_DATA_FL_FLOW_CONTROL)
#include "ds707_ccm.h"
#endif

#include "nvruimi.h"

#include "ds3gmgr.h"
#include "ds3gutil.h"
#include "ds707_tm_mgr.h"
#include "mc.h"

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
#include "iwlan_iface_ext.h"
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  RMSM SIO Reverse Watermark configurable
---------------------------------------------------------------------------*/
static uint32 ds707_rmsm_reva_rev_lo_wmk = DS707_RMSM_REVA_REV_LO_WATERMARK;
static uint32 ds707_rmsm_reva_rev_hi_wmk = DS707_RMSM_REVA_REV_HI_WATERMARK;
static uint32 ds707_rmsm_reva_rev_dont_exceed_cnt = DS707_RMSM_REVA_REV_DONT_EXCEED_CNT;

#ifdef FEATURE_HDR_REVB
static uint32 ds707_rmsm_revb_rev_lo_wmk = DS707_RMSM_REVB_REV_LO_WATERMARK;
static uint32 ds707_rmsm_revb_rev_hi_wmk = DS707_RMSM_REVB_REV_HI_WATERMARK;
static uint32 ds707_rmsm_revb_rev_dont_exceed_cnt = DS707_RMSM_REVB_REV_DONT_EXCEED_CNT;
#endif /* FEATURE_HDR_REVB */

/*---------------------------------------------------------------------------
  Declaration of the states of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_RMSM_MIN_STATE = -1,
  DS707_RMSM_NULL_STATE = 0,                    /* Initial state           */
  DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE = 1, /* Um coming up-Relay mode */
  DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE = 2,
                              /* Um coming up - SIP netmodel call */
  DS707_RMSM_UM_RM_UP_RELAY_STATE = 3,
                              /* phys link UP - hence Relay call UP  as far 
                                 as AT is concerned */
  DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE = 4,/* Waiting for e2e PPP-UP*/
  DS707_RMSM_E2E_PPP_UP_SIP_STATE = 5,         /* Um Rm Um up  -  SIP      */
  DS707_RMSM_RESYNCING_UM_SIP_STATE = 6,       /* Resyncind UM - SIP call  */
  DS707_RMSM_WAITING_FOR_UM_DOWN_STATE = 7,    /* Um going down            */

  DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE = 8,
#ifdef FEATURE_DS_MOBILE_IP
  DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE = 9,  /* Um coming up - MIP Call  */
  DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE = 10, /* PPP coming up on Rm      */
  DS707_RMSM_UM_RM_UP_MIP_STATE = 11,          /* Rm and Um up - MIP call  */
  DS707_RMSM_RESYNCING_UM_MIP_STATE = 12, /* Resyncing Um with Rm options  */
  DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE = 13,
                                               /* Um going down, Rm up     */
#endif /* FEATURE_DS_MOBILE_IP */
  DS707_RMSM_WAITING_FOR_RM_DOWN_STATE = 14, /* Rm going down          */

  /* waiting for phys link down */
  DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE = 15,
  /* Um waiting for Arbitration*/
  DS707_RMSM_WAITING_FOR_UM_ARBITRATION = 16,
  DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE = 17,
  DS707_RMSM_MAX_STATE
} ds707_rmsmi_state_type;

typedef enum
{
  DS707_RMSM_BRINGUP_UM_INVALID = -1,
  DS707_RMSM_BRINGUP_UM_FAIL,  /* goto NULL state */
  DS707_RMSM_BRINGUP_UM_SUCCESS,
  DS707_RMSM_BRINGUP_UM_SUCCESS_CLOSE_IN_PROGRESS,
  DS707_RMSM_BRINGUP_UM_WAITING_ARBITRATION,
  DS707_RMSM_BRINGUP_UM_MAX = DS707_RMSM_BRINGUP_UM_WAITING_ARBITRATION
}ds707_rmsmi_bringup_errno_type;

/*---------------------------------------------------------------------------
  The Rm state machine's information block

  field                      meaning
 -------                    ---------
  state                      State machine state
  inited                     State machine been initialized?
  IS707pkt_iface_ptr         IS707 iface used for data call
  sio_pkt_iface              SIO iface used for data call
  sio_pkt_phys_link          physlink for sio pkt iface
  orig_tear_down_f_ptr       fn ptr in IS707 iface
  orig_bring_up_f_ptr        fn ptr in IS707 iface
  rm_ppp_config              PPP configuration on RM Device
  um_iface_down_buf_ptr      Event buf for UM Iface DOWN
  um_iface_up_buf_ptr        Event buf for UM Iface UP
  um_phys_link_up_buf_ptr    Event buf - UM Phys Link UP
  um_phys_link_gone_buf_ptr  Event buf - UM Phys Link GONE (Down & NULL)
  flow_enable_buf_ptr        Event buf for UM Flow ON
  flow_disable_buf_ptr       Event buf for UM Flow OFF
  rlp_fwd_q;                 RLP FWD Link q, For Relay mode
  sio_fwd_wmk;               SIO FWD Link wm, SIO to laptop
  sio_fwd_wmk_q;             Corresponding SIO FWD LINK queue
  sio_rev_wmk;               SIO REV Link wm, laptop to SIO
  sio_rev_wmk_q;             Corresponding SIO REV LINK queue
  pkt_info                   Handles dial string based call setup

  rm_flow_enable_buf_ptr     Event buf for RM Flow ON
  rm_flow_disable_buf_ptr    Event buf for RM Flow OFF
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_rmsmi_state_type  state;
  boolean                 inited;
  ps_iface_type *         IS707pkt_iface_ptr;
  ps_iface_type           sio_pkt_iface;
  ps_phys_link_type       sio_pkt_phys_link;
  ps_iface_cmd_f_ptr_type orig_tear_down_f_ptr;
  ps_iface_cmd_f_ptr_type orig_bring_up_f_ptr;
  ppp_dev_opts_type       rm_ppp_config;
  void *                  um_iface_down_buf_ptr;
  void *                  um_iface_up_buf_ptr;
  void *                  um_iface_configuring_buf_ptr;
  void *                  um_phys_link_up_buf_ptr;
  void *                  um_phys_link_gone_buf_ptr;
  void *                  um_phys_link_flow_enable_buf_ptr;
  void *                  um_phys_link_flow_disable_buf_ptr;
  void *                  um_flow_tx_enable_buf_ptr;
  void *                  um_flow_tx_disable_buf_ptr;
  void *                  flow_enable_buf_ptr;
  void *                  flow_disable_buf_ptr;
  void *                  ip_addr_changed_buf_ptr;/* Event buf for ip addr 
                                                               change     */
  q_type                  rlp_fwd_wmk_q_list[DSRSP_NUM_ROUTES];
  dsm_watermark_type      rlp_fwd_wmk_list[DSRSP_NUM_ROUTES];
  dsm_watermark_type      sio_fwd_wmk;
  q_type                  sio_fwd_wmk_q;
  dsm_watermark_type      sio_rev_wmk_list[DSRSP_NUM_ROUTES];
  q_type                  sio_rev_wmk_q_list[DSRSP_NUM_ROUTES];
  ds3g_pkt_call_info_type pkt_info;   /* pkt info added to handle force 
                                                        cdma only scenario */
  void *                  ppp_rm_event_handle;      /* Rm PPP event handle */
  void *                  ppp_um_event_handle;      /* Um PPP event handle */
  void *                  rm_flow_enable_buf_ptr;
  void *                  rm_flow_disable_buf_ptr;
  boolean                 rmsm_sent_connect;
  void *                  client_data_ptr; /* client data used in arbitration */

  /* holds the state before entering arbitration */
  ds707_rmsmi_state_type  state_before_arbitration;
  boolean                 entitlement_check_req;
  /*-------------------------------------------------------------------------
    RMSM Supports v4 Only. If RMSM comes up, then it will be the only call 
    that is up

    Requirements:
      1. When RMSM wants to come up, no other iface should be in use.
          This condition is checked in rmsm wants pkt call
      2. While RMSM is coming up or up or going down, 
         no other call should be allowed
         This condition is checked in the ACLs
  -------------------------------------------------------------------------*/
  uint32                  subs_id;
} ds707_rmsmi_info_type;

LOCAL ds707_rmsmi_info_type ds707_rmsmi_info;

/*---------------------------------------------------------------------------
 Global structure to keep track of RM FWD/REV link watermark 
---------------------------------------------------------------------------*/
static ds707_rmsm_wmk_type ds707_rmsm_wmk_value;


/*---------------------------------------------------------------------------
  Function table registered with siolib to manage serial port
---------------------------------------------------------------------------*/
LOCAL ds3g_siolib_func_tbl_type ds707_rmsmi_sio_func_table;

/*---------------------------------------------------------------------------
  The SIO port allocated for external data call.
---------------------------------------------------------------------------*/
static ds3g_siolib_portid_e_type ds707_rmsmi_sio_port_allocated =
                          DS3G_SIOLIB_PORTID_NONE;


#ifndef T_ARM
/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
static const char *ds707_rmsmi_state_names[DS707_RMSM_MAX_STATE] =
{
  "Null State",
  "Waiting for Um to come up in SIP Relay mode",
  "Waiting for Um to come up in SIP Network mode",
  "Um/Rm up in SIP Relay mode",
  "Waiting for e2e PPP up in SIP Network mode",
  "e2e PPP up in SIP Network mode",
  "Resyncing Um in SIP Network mode",
  "Waiting for Um to go down",
  "Waiting for Phys Link UP state",
#ifdef FEATURE_DS_MOBILE_IP
  "Waiting for Um to come up in MIP mode",
  "Waiting for Rm to come up in MIP mode",
  "Um/Rm up in MIP mode",
  "Resyncing Um in MIP mode",
  "Rm up, waiting for Um to go down in MIP mode",
#endif /* FEATURE_DS_MOBILE_IP */
  "Waiting for Rm to go down",
  "Waiting for Phys Link to go to Stable state",
  "Waiting for Um Arbitation to complete"
};

static const char *ds707_rmsmi_event_names[DS707_RMSM_MAX_EV] =
{
  "Rm wants packet call",
  "Rm interface is down",
  "Um interface is down",
  "Um physical link is up",
  "Um physical link is down",
  "Um PPP is down",
  "Rm PPP is up",
  "Um PPP is up",
  "Um MIP IP Address changed",
  "e2e PPP is up",
  "Um resync",
  "Rm resync"
};
#endif /* T_ARM */

/*---------------------------------------------------------------------------
  RMSM state management and event processing functions
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Time to wait for Rm PPP setup to complete. This time should cover the 
  worst case scenario for PPP to complete (~30 seconds for LCP+~30 seconds
  for IPCP + some cushion). Time is in seconds.
 ---------------------------------------------------------------------------*/

#define RM_PPP_SETUP_TIME         120

/*---------------------------------------------------------------------------
   Maximum PKT Dial String length used only for making Dial String based
   decision of the service type (CDMA only or Automatic)
---------------------------------------------------------------------------*/
#define MAX_PKT_DIAL_STRING_LEN 32

/*---------------------------------------------------------------------------
   Maximum PKT SIZE will be used for splitting a large DSM chain to a 
   small DSM chain when relay model call is up. RLP layer is possible
   to pass a large DSM chain to A2/SMD in relay model call when air condition
   is poor. Without splitting, A2/SMD call will crash. In networkmodel call,
   PPP/HDLC module will traverse the DSM chain and split to the small chains.
---------------------------------------------------------------------------*/
#define MAX_PKT_SIZE              5000

/*---------------------------------------------------------------------------
  A boolean flag to convey if reestablishing the PPP is required or not. This
  is used when current PPP is established by some socket call and the iface
  is in Lingering state and that time a new laptop call comes in.
---------------------------------------------------------------------------*/
LOCAL boolean rmsmi_reestablish_um_link;

  /*-------------------------------------------------------------------------
  Now we do RM call bringup in 2 steps, a look_up first and then bring_up
  However, we store the iface returned by both the calls in:
  IS707pkt_iface_ptr
  The call to ds707_rmsm_get_um_iface returns value stored in IS707pkt_iface_ptr,
  however that value should only be returned if that iface is up,
  so a new boolean is needed
  -------------------------------------------------------------------------*/
LOCAL boolean rmsmi_IS707pkt_iface_brought_up;

LOCAL uint8   rmsmi_cnt_iface_down_ev_regd;

/*
Proc ID value to be used while making the acl policy
*/
#define DS707_RMSMI_PROC_ID 0x1
#define DS707_RMSMI_APP_ID  0x0000000100000000ULL

LOCAL void ds707_rmsmi_smd_notify_timer_expired(timer_cb_data_type data)
{
  sio_ioctl_param_type wait_param;

  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "RMSM_DATA_AGG_TIMER expired. Flushing data immediately");

/*---------------------------------------------------------------------------
  The timer expired, call the foll. ioctl in sio to call notifywrite 
  immediately
---------------------------------------------------------------------------*/
  wait_param.wm_wait = FALSE;
  ds3g_siolib_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param);
}

LOCAL void ds707_rmsmi_tx_wm_non_empty_func
(
  struct dsm_watermark_type_s *wm,
  void* callback_data
)
{
  sio_ioctl_param_type wait_param;

  if (ds707_ds_smem_tx_delay)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "RMSM_DATA_AGG_TIMER SET WAIT");

    /*------------------------------------------------------------------------
      Call the foll. ioctl in sio to set the wait which will delay the call to
      notifywrite (to ARM11).
    -------------------------------------------------------------------------*/
    wait_param.wm_wait = TRUE;
    ds3g_siolib_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param);
    timer_set(&ds707_smd_notify_delay_timer,
              ds707_ds_smem_tx_delay,  // this is the delay in MS
              0,
              T_MSEC);
  }
}


/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*===========================================================================
FUNCTION DS707_IS_BRINGUP_ALLOWED()

DESCRIPTION
  This function checks the current system state and determines if a
  3GPP2 call is allowed or not

RETURN VALUE
  TRUE: on success
  FALSE: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_is_bringup_allowed
(
  ps_iface_net_down_reason_type *end_reason,
  ds707_data_session_profile_info_type *data_sess_prof_ptr
);

/*---------------------------------------------------------------------------
  RMSM state management and event processing functions
---------------------------------------------------------------------------*/

LOCAL void ds707_rmsmi_transition_state
(
  ds707_rmsmi_state_type new_state               /* state to tranistion to */
);

LOCAL void ds707_rmsmi_process_rm_wants_pkt_call_ev( void );
LOCAL void ds707_rmsmi_process_rm_iface_down_ev( void );
LOCAL void ds707_rmsmi_process_um_iface_down_ev( void );
LOCAL void ds707_rmsmi_process_um_phys_link_up_ev( void );
LOCAL void ds707_rmsmi_process_um_phys_link_gone_ev( void );
LOCAL void ds707_rmsmi_process_um_ppp_down_ev( void );

LOCAL void ds707_rmsmi_process_e2e_ppp_up_ev( void );
LOCAL void ds707_rmsmi_process_um_resync_ev( void );
LOCAL void ds707_rmsmi_process_rm_resync_ev( void );

LOCAL boolean ds707_rmsmi_reg_ev_netmodel_after_bringup(ps_iface_type* iface_ptr);
LOCAL boolean ds707_rmsmi_dereg_ev(ps_iface_type* iface_ptr);

LOCAL ds707_rmsmi_bringup_errno_type ds707_rmsmi_bring_up_iface_by_policy
(
  void*                  client_data_ptr,
  ds707_rmsmi_state_type curr_state
);

LOCAL boolean ds707_rmsmi_bringup_iface
(
  void* client_data_ptr
);

#ifdef FEATURE_DS_MOBILE_IP
LOCAL void ds707_rmsmi_process_rm_ppp_up_ev( void );
LOCAL void ds707_rmsmi_process_um_iface_up_ev( void );
LOCAL void ds707_rmsmi_process_um_mip_ip_addr_changed_ev( void );

LOCAL boolean ds707_rmsmi_mip_config_after_bringup(void);

#ifdef FEATURE_DS_MULTIPLE_NAI
STATIC void ds707_rmsm_get_mip_session_info_cb
(
  byte                     active_mip_profile,
  mip_session_info_type *  session
);
#endif /* FEATURE_DS_MULTIPLE_NAI */
#endif /* FEATURE_DS_MOBILE_IP */

LOCAL void ds707_cleanup_um( int16* ps_iface_errno );

/*---------------------------------------------------------------------------
  RMSM callback functions registered with various external modules
---------------------------------------------------------------------------*/

LOCAL dsat_mode_enum_type ds707_rmsmi_process_esc_seq_cb(void);

LOCAL void ds707_rmsmi_process_dtr_sig_cb
(
  boolean is_asserted                /* Flag to indicate current DTR state */
);

LOCAL void ds707_rmsmi_call_abort_cb( void );

LOCAL void rmsmi_707_pkt_iface_event_handler_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

LOCAL void rmsmi_707_pkt_phys_link_event_handler_cb
(
  ps_phys_link_type *phys_link_ptr,        /* Pointer to the phys link     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

LOCAL void rmsmi_707_pkt_flow_event_handler_cb
(
  ps_flow_type *flow_ptr,             /* Pointer to the flow    */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

LOCAL void ds707_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);

LOCAL void ds707_rmsmi_pppum_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);

#ifdef FEATURE_DS_MOBILE_IP
LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_netmdl_term_req_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Ptr to ref to PS meta info    */
);

LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_rm_resync_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Ptr to ref to PS meta info    */
);
#endif /* FEATURE_DS_MOBILE_IP */

/*---------------------------------------------------------------------------
  SIO watermark configuration and event hanlding functions
---------------------------------------------------------------------------*/

LOCAL void ds707_rmsmi_setup_sio_watermarks
(
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr
);

LOCAL void ds707_rmsmi_sio_rx_gone_empty_func(
 struct dsm_watermark_type_s *wm_ptr, 
 void *user_data_ptr
);

LOCAL void ds707_rmsmi_sio_rx_non_empty_func(
 struct dsm_watermark_type_s *wm_ptr, 
 void *user_data_ptr
);

LOCAL void ds707_rmsmi_sio_rx_lowater_func(
 struct dsm_watermark_type_s *wm_ptr, 
 void *user_data_ptr
);

LOCAL void ds707_rmsmi_sio_rx_hiwater_func(
 struct dsm_watermark_type_s *wm_ptr, 
 void *user_data_ptr
);

LOCAL void ds707_rmsmi_sio_tx_lowater_func(
 struct dsm_watermark_type_s *wm_ptr, 
 void *user_data_ptr
);

LOCAL void ds707_rmsmi_sio_tx_hiwater_func(
 struct dsm_watermark_type_s *wm_ptr, 
 void *user_data_ptr
);

LOCAL void ds707_rmsmi_sio_tx_rlp_data(uint32 unused );

LOCAL void ds707_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void           *meta_info_ptr,
  void *user_data
);

LOCAL dsm_item_type* ds707_rmsmi_sio_rx_ppp_data( void *user_data );

LOCAL void ds707_rmsmi_trim_dial_str
(
  const byte *orig_dial_str,
  byte *dial_str_after_trim
);

/*---------------------------------------------------------------------------
  Helper utility functions
---------------------------------------------------------------------------*/
LOCAL boolean ds707_rmsmi_process_lingering_call( acl_policy_info_type* );

LOCAL void ds707_rmsmi_entitlement_check_complete
( 
  unsigned char            cb_data,  
  boolean                  allow_call
);

LOCAL void ds707_rmsmi_sio_cleanup ( void );

void ds707_timer_cb
(
  unsigned long    timer_id
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS707_RMSM_INIT()

  DESCRIPTION
    This function initializes the state machine.

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
boolean ds707_rmsm_init
(
  void
)
{
  int p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    RMSM should be initialized only once.  
    Note the "if" check below assumes that ds707_rmsmi_info is a defined on
    heap so that all its members get inited to 0 (FALSE).    
  -------------------------------------------------------------------------*/  
  if( ds707_rmsmi_info.inited == TRUE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "RMSM already initialized");
    return TRUE;
  }

  ds707_rmsmi_info.state = DS707_RMSM_NULL_STATE;
  ds707_rmsmi_info.entitlement_check_req = FALSE;

  /*-------------------------------------------------------------------------
    All packet data laptp calls require two interfaces. SIO iface is created
    and managed by RMSM. Note that iface is left DISABLED and bring up and 
    tear down are not registered since sockets origination over SIO is not 
    supported by this RMSM. IS707 iface which is managed by 707 pkt 
    controller is passed to this funtion and is stored for further usage.
    As SIO won't ever have more than a single physlink use '1' explicitly.
  -------------------------------------------------------------------------*/
  if (ps_iface_create( &ds707_rmsmi_info.sio_pkt_iface,
                       SIO_IFACE,
                       NULL,
                       NULL,
                       &ds707_rmsmi_info.sio_pkt_phys_link,
                       1
                     ) < 0 )
  {
    ERR_FATAL("Could not create iface", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    To support Data Protocol Logging on the SIO iface
  -------------------------------------------------------------------------*/
  
 (void) ps_iface_dpl_support_network_logging(&ds707_rmsmi_info.sio_pkt_iface,
                                              DPL_IID_IFNAME_SIO_IFACE_CDMA);
 (void) ps_iface_dpl_set_iface_desc(&ds707_rmsmi_info.sio_pkt_iface, 
                                    NULL);

 (void) ps_iface_dpl_support_link_logging(&ds707_rmsmi_info.sio_pkt_iface,
                                          DPL_IID_IFNAME_SIO_IFACE_CDMA);
  
 (void) ps_iface_dpl_support_flow_logging(&ds707_rmsmi_info.sio_pkt_iface,
                                          DPL_IID_IFNAME_SIO_IFACE_CDMA);

  
  /*-------------------------------------------------------------------------
    Clear RMSM's Um iface ptr.  Will be set to resolved interface after
    RM_WANTS_PKT is received.  Be sure not to reference until then.
  -------------------------------------------------------------------------*/
  ds707_rmsmi_info.IS707pkt_iface_ptr = NULL;

  /*-------------------------------------------------------------------------
    Initialize the sio callbacks.  Packet data always pays attention to
    DTR signal, regardless of AT&D setting.  Which means that if AT&D is
    de-asserted by the laptop, then the mobile always ends the data session
    (goes NULL), irrespective of the AT&D value.
  -------------------------------------------------------------------------*/
  memset(&ds707_rmsmi_sio_func_table, 0, sizeof(ds707_rmsmi_sio_func_table));

  ds707_rmsmi_sio_func_table.at_escape_sequence_handler = 
                                              ds707_rmsmi_process_esc_seq_cb;
  ds707_rmsmi_sio_func_table.dtr_changed_sig_handler    = 
                                              ds707_rmsmi_process_dtr_sig_cb;
  ds707_rmsmi_sio_func_table.dtr_changed_sig_handler_ampd0 = 
                                              ds707_rmsmi_process_dtr_sig_cb;
  
  /*-------------------------------------------------------------------------
    Initialize the watermarks and queues used to hook up to SIO in relay and
    network model.
  -------------------------------------------------------------------------*/
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    dsm_queue_init (&ds707_rmsmi_info.rlp_fwd_wmk_list[p],
                    (int)0xFFFFFFFF,
                    &ds707_rmsmi_info.rlp_fwd_wmk_q_list[p]);
    dsm_queue_init (&ds707_rmsmi_info.sio_rev_wmk_list[p],
                    (int)0xFFFFFFFF,
                    &ds707_rmsmi_info.sio_rev_wmk_q_list[p]);
    ds707_rmsmi_info.sio_rev_wmk_list[p].current_cnt = 0;
  }

  dsm_queue_init (&ds707_rmsmi_info.sio_fwd_wmk,
                  (int)0xFFFFFFFF,
                  &ds707_rmsmi_info.sio_fwd_wmk_q);
  ds707_rmsmi_info.sio_fwd_wmk.current_cnt = 0;

  /*-------------------------------------------------------------------------
    Initialize the ps_iface cback buf_ptrs for IS707 iface event handling
  -------------------------------------------------------------------------*/
  ds707_rmsmi_info.um_iface_up_buf_ptr = 
                    ps_iface_alloc_event_cback_buf(
                                        rmsmi_707_pkt_iface_event_handler_cb,
                                        NULL );

  ds707_rmsmi_info.um_iface_down_buf_ptr = 
                    ps_iface_alloc_event_cback_buf(
                                        rmsmi_707_pkt_iface_event_handler_cb,
                                        NULL);

  ds707_rmsmi_info.um_iface_configuring_buf_ptr = 
                    ps_iface_alloc_event_cback_buf(
                                        rmsmi_707_pkt_iface_event_handler_cb,
                                        NULL );


  ds707_rmsmi_info.um_phys_link_up_buf_ptr = 
                    ps_iface_alloc_event_cback_buf(
                                        rmsmi_707_pkt_iface_event_handler_cb,
                                        NULL);

  ds707_rmsmi_info.um_phys_link_gone_buf_ptr = 
                     ps_iface_alloc_event_cback_buf(
                                       rmsmi_707_pkt_iface_event_handler_cb,
                                       NULL );

  ds707_rmsmi_info.um_phys_link_flow_enable_buf_ptr = 
                     ps_phys_link_alloc_event_cback_buf(
                                    rmsmi_707_pkt_phys_link_event_handler_cb,
                                    NULL );

  ds707_rmsmi_info.um_phys_link_flow_disable_buf_ptr = 
                     ps_phys_link_alloc_event_cback_buf(
                                  rmsmi_707_pkt_phys_link_event_handler_cb,
                                  NULL );

  ds707_rmsmi_info.um_flow_tx_enable_buf_ptr = 
                     ps_flow_alloc_event_cback_buf(
                                   rmsmi_707_pkt_flow_event_handler_cb,
                                   NULL );

  ds707_rmsmi_info.um_flow_tx_disable_buf_ptr = 
                      ps_flow_alloc_event_cback_buf(
                                      rmsmi_707_pkt_flow_event_handler_cb,
                                      NULL );


  ds707_rmsmi_info.flow_enable_buf_ptr = 
                      ps_iface_alloc_event_cback_buf(
                                     rmsmi_707_pkt_iface_event_handler_cb,
                                     NULL);

  ds707_rmsmi_info.flow_disable_buf_ptr = 
                      ps_iface_alloc_event_cback_buf(
                                      rmsmi_707_pkt_iface_event_handler_cb,
                                      NULL);

  ds707_rmsmi_info.ip_addr_changed_buf_ptr = 
                      ps_iface_alloc_event_cback_buf(
                                      rmsmi_707_pkt_iface_event_handler_cb,
                                      NULL);

/**************************************************************************/
  if ( ( ds707_rmsmi_info.um_iface_down_buf_ptr == NULL )             ||
       ( ds707_rmsmi_info.um_iface_up_buf_ptr == NULL )               ||
       ( ds707_rmsmi_info.um_iface_configuring_buf_ptr == NULL )      ||
       ( ds707_rmsmi_info.um_phys_link_up_buf_ptr == NULL )           ||
       ( ds707_rmsmi_info.um_phys_link_gone_buf_ptr == NULL )         ||
       ( ds707_rmsmi_info.um_phys_link_flow_enable_buf_ptr == NULL )  ||
       ( ds707_rmsmi_info.um_phys_link_flow_disable_buf_ptr == NULL ) ||
       ( ds707_rmsmi_info.um_flow_tx_enable_buf_ptr == NULL )         ||
       ( ds707_rmsmi_info.um_flow_tx_disable_buf_ptr == NULL )        ||
       ( ds707_rmsmi_info.flow_enable_buf_ptr == NULL )               ||
       ( ds707_rmsmi_info.flow_disable_buf_ptr == NULL )              ||
       ( ds707_rmsmi_info.ip_addr_changed_buf_ptr == NULL ) 
     )
  {
    ERR_FATAL("Event buf allocation failed", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Populate the pkt info fields with the default values just as when a pkt 
    call is initiated by the TE. 
    drs = 1, 
    holddown_timer =1, 
    new network = NO SRV because we currently donot what system call will 
                  come up on. CM will inform if its HDR or 1x after call 
                 is connected
    force_cdma_only = FALSE 
  -------------------------------------------------------------------------*/ 
  ds707_rmsmi_info.pkt_info.drs_bit         = TRUE;
  ds707_rmsmi_info.pkt_info.use_hdown_timer = TRUE;
  ds707_rmsmi_info.pkt_info.new_network     = SYS_SYS_MODE_NO_SRV;
  ds707_rmsmi_info.pkt_info.force_cdma_only = FALSE;
  ds707_rmsmi_info.pkt_info.inc_call        = FALSE;
 /*-------------------------------------------------------------------
      Set the epzid flag to FALSE for all Origination attempts
  -------------------------------------------------------------------*/
  ds707_rmsmi_info.pkt_info.epzid           = FALSE;

  /*-------------------------------------------------------------------------
    Allocate the PPP event notification handles
  -------------------------------------------------------------------------*/ 
  ds707_rmsmi_info.ppp_rm_event_handle =
    ppp_event_alloc( PPP_RM_DEV,
                     ds707_rmsmi_ppprm_event_handler_cb,
                     NULL,
                     NULL,
                     NULL,
                     ds707_rmsmi_ppprm_event_handler_cb,
                     NULL,
                     NULL,
                     NULL );

  if( NULL == ds707_rmsmi_info.ppp_rm_event_handle )
  {
    ERR_FATAL( "Unable to acquire Rm PPP event handle", 0, 0, 0 );
  }

  ds707_rmsmi_info.ppp_um_event_handle =
    ppp_event_alloc( PPP_UM_SN_DEV,
                     ds707_rmsmi_pppum_event_handler_cb,
                     NULL,
                     NULL,
                     NULL,
                     ds707_rmsmi_pppum_event_handler_cb,
                     NULL,
                     NULL,
                     NULL );

  if( NULL == ds707_rmsmi_info.ppp_um_event_handle )
  {
    ERR_FATAL( "Unable to acquire Um PPP event handle", 0, 0, 0 );
  }

  ds707_rmsmi_info.rmsm_sent_connect = FALSE;
  ds707_rmsmi_info.inited = TRUE;
  
  /*-------------------------------------------------------------------------
    Set the rmsmi_reestablish_ppp flag to FALSE.
  -------------------------------------------------------------------------*/
  rmsmi_reestablish_um_link = FALSE;
  
  rmsmi_IS707pkt_iface_brought_up = FALSE;

#ifdef  FEATURE_DATA_WMK_CONFIG
  /*-------------------------------------------------------------------------
  Read wmk values from or write default values to NV. This function needs to be 
  called during initialization because the wmk lvl values will only be written 
  to the NV when the common function has both the um and rm wmk values 
  -------------------------------------------------------------------------*/
  ds707_rmsm_update_wmk_value();
#endif 
  return TRUE;
} /* ds707_rmsm_init() */


/*===========================================================================
  FUNCTION DS707_RMSM_IS_PACKET_CALL()

  DESCRIPTION
    Returns true if we're in a packet call

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_rmsm_is_packet_call
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    We are in a packet call if rmsm state is non-null 
  -------------------------------------------------------------------------*/
  return (ds707_rmsmi_info.state == DS707_RMSM_NULL_STATE) ? FALSE : TRUE;
} /* ds707_rmsm_is_packet_call() */


/*===========================================================================
FUNCTION DS707_RMSM_IS_IN_NETWORK_MODEL_CALL

DESCRIPTION
  This function returns whether we are currently in a simple IP network
  model laptop call

DEPENDENCIES
  None

RETURN VALUE
  TRUE if we are in a SIP network model call, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_rmsm_is_in_network_model_call( void )
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  if( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                         DS_CRM_VAL_NETMODEL_MODE )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* ds707_rmsm_is_in_network_model_call */


/*===========================================================================
  FUNCTION DS707_RMSM_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_post_event
(
  ds707_rmsm_event_type event
)
{
  ds_cmd_type *cmd_ptr;                          /* DS Task cmd buf        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds707_rmsmi_info.inited != TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Can't post ev %d, RMSM not inited", 
                     event);
    DS_FORCE_ASSERT();
    return;
  }

#ifdef T_ARM
  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "Event %d posted to RMSM in state %d",
                   event,
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "'%s' event posted to RMSM in state %d.",
                   ds707_rmsmi_event_names[event],
                   ds707_rmsmi_info.state);
#endif

  if( ( cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO) ) == NULL )
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DS task corresponding to each event
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case DS707_RMSM_RM_WANTS_PKT_CALL_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_RM_WANTS_PKT_CALL;
      break;

    case DS707_RMSM_RM_IFACE_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_RM_IFACE_DOWN;
      break;

    case DS707_RMSM_UM_IFACE_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_IFACE_DOWN;
      break;

    case DS707_RMSM_UM_PHYS_LINK_UP_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_PHYS_LINK_UP;
      break;

    case DS707_RMSM_UM_PHYS_LINK_GONE_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_PHYS_LINK_GONE;
      break;

    case DS707_RMSM_UM_PPP_DOWN_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_PPP_DOWN;
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_RM_PPP_UP_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_RM_PPP_UP;
      break;

    case DS707_RMSM_UM_IFACE_UP_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_IFACE_UP;
      break;

    case DS707_RMSM_UM_MIP_IP_ADDR_CHANGED_EV:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_MIP_IP_ADDR_CHANGED;
      break;
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_E2E_PPP_UP_EV:
    {
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_E2E_PPP_UP;
      break;
    }
    case DS707_RMSM_UM_RESYNC_EV:
    {
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_UM_RESYNCING;
      break;
    }
    case DS707_RMSM_RM_RESYNC_EV:
    {
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_RM_RESYNCING;
      break;
    }

    default:
      DS_FORCE_ASSERT();
      return;
  } /* switch(event) */

  ds_put_cmd( cmd_ptr );

} /* ds707_rmsm_post_event() */


/*===========================================================================
  FUNCTION DS707_RMSM_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to RMSM in DS task and calls
    an appropriate function to handle it.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds_cmd_enum_type cmd;
  ps_sys_ioctl_dun_ctrl_response_type *dun_ctrl_resp_ptr = NULL; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(NULL == ds_cmd_ptr)
  {
    DATA_ERR_FATAL("ds_cmd_ptr is NULL");
    return;
  }

  cmd  = ds_cmd_ptr->hdr.cmd_id;

  if(ds707_rmsmi_info.inited != TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Can't post cmd %d, RMSM not inited", 
                     cmd);
    DS_FORCE_ASSERT();
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "Recvd cmd %d in state %d",
                   cmd,
                   ds707_rmsmi_info.state);

  switch( cmd )
  {
    case DS_CMD_707_RMSM_RM_WANTS_PKT_CALL:
      /*---------------------------------------------------------------------
        Laptop wants to initiate a pkt data call
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_wants_pkt_call_ev();
      break;

    case DS_CMD_707_RMSM_RM_IFACE_DOWN:
      /*---------------------------------------------------------------------
        Laptop is disconnected or RM PPP terminated, RM iface is down.
        Terminate call on UM also.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_iface_down_ev();
      break;

    case DS_CMD_707_RMSM_UM_IFACE_DOWN:
      /*---------------------------------------------------------------------
        Call terminated on UM, cleanup RM and go NULL.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_iface_down_ev();
      break;

    case DS_CMD_707_RMSM_UM_PHYS_LINK_UP:
      /*---------------------------------------------------------------------
        Traffic channel established on Um, used for Relay/network model call        
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_phys_link_up_ev();
      break;

    case DS_CMD_707_RMSM_UM_PHYS_LINK_GONE:
      /*---------------------------------------------------------------------
        Traffic channel is down, terminate the call if this happens in the 
        middle of a call setup.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_phys_link_gone_ev();
      break;
    
    case DS_CMD_707_RMSM_UM_PPP_DOWN:
      /*---------------------------------------------------------------------
        UM PPP is down, this indicates the PPP resync failure on UM,
        cleanup RM and go NULL.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_ppp_down_ev();
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DS_CMD_707_RMSM_RM_PPP_UP:
      /*---------------------------------------------------------------------
        PPP is up on RM during a MIP call setup. Compare Um and Rm options,
        resync Um if they mismatch.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_ppp_up_ev();
      break;

    case DS_CMD_707_RMSM_UM_IFACE_UP:
      /*---------------------------------------------------------------------
        Um is up with MIP. Bring up Rm now.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_iface_up_ev();
      break;
    case DS_CMD_707_RMSM_UM_MIP_IP_ADDR_CHANGED:
      ds707_rmsmi_process_um_mip_ip_addr_changed_ev();
      break;
#endif /* FEATURE_DS_MOBILE_IP */

    case DS_CMD_707_RMSM_E2E_PPP_UP:
      /*---------------------------------------------------------------------
        E2E PPP complete. Call is UP now.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_e2e_ppp_up_ev();
      break;

    case DS_CMD_707_RMSM_UM_RESYNCING:
      /*---------------------------------------------------------------------
        UM is resyncing.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_um_resync_ev();
      break;

    case DS_CMD_707_RMSM_RM_RESYNCING:
      /*---------------------------------------------------------------------
        RM is resyncing.
      ---------------------------------------------------------------------*/
      ds707_rmsmi_process_rm_resync_ev();
      break;

    case DS_CMD_707_RMSM_ENTITLEMENT_CHECK_COMPLETE:
      if(NULL == ds_cmd_ptr->cmd_payload_ptr)
      {
        DATA_ERR_FATAL("cmd_payload_ptr is NULL");
        return;
      }

      /*---------------------------------------------------------------------
        Extract information from cb data
      ---------------------------------------------------------------------*/
      dun_ctrl_resp_ptr 
        = (ps_sys_ioctl_dun_ctrl_response_type *)(ds_cmd_ptr->cmd_payload_ptr);

      ds707_rmsmi_entitlement_check_complete(dun_ctrl_resp_ptr->cb_data,
                                             dun_ctrl_resp_ptr->allow_call);
      break;

    default:
      DS_FORCE_ASSERT();
      return;
  } /* switch( cmd ) */
} /* ds707_rmsm_process_cmd() */


/*===========================================================================
  FUNCTION DS707_RMSM_PROCESS_RM_IFACE_FLOW_CMD()

  DESCRIPTION
    This function processes the IFACE flow commands for the RM interface

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_process_rm_iface_flow_cmd
(
  ds_cmd_type  *ds_cmd_ptr
)
{
  ds_cmd_enum_type cmd_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  cmd_id  = ds_cmd_ptr->hdr.cmd_id;

  if(ds707_rmsmi_info.inited != TRUE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Can't post cmd %d, RMSM not inited", 
                     cmd_id);
    DS_FORCE_ASSERT();
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, "Recvd cmd %d in state %d",
                   cmd_id, ds707_rmsmi_info.state);

  switch( ds707_rmsmi_info.state )
  {
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
#endif
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    {
      switch( cmd_id)
      {
        case DS_CMD_707_RM_IFACE_FLOW_ENABLED:
#ifdef FEATURE_HDR
#if   defined(FEATURE_DATA_FL_FLOW_CONTROL)
       /*---------------------------------------------------------------------
         Send OFF command to CCM
       ---------------------------------------------------------------------*/
         DATA_IS707_MSG0(MSG_LEGACY_MED, 
                      "RM FWD low watermark reached, post DS707_CCM_FLOW_OFF");
         ds707_ccm_mem_send_flow_control_cmd(DS707_CCM_MEM_WMK_SIO_FWD, 
                                             DS707_CCM_FLOW_OFF);

#else
        /*-------------------------------------------------------------------
          Disable Flow Control by sending a message to the HDR Base Station. 
          Disable Flow Control means enable the flow and allow data to flow
          from the base station to the mobile
        -------------------------------------------------------------------*/
         DATA_IS707_MSG0(MSG_LEGACY_MED, "RM FWD disable MAC flow control");
          hdrl1_fmac_disable_flow_control ((uint32)HDRFMAC_DRC_REQ_PSQ);
#endif /* FEATURE_DATA_ON_APPS */
#endif /* FEATURE_HDR */
          break;

        case DS_CMD_707_RM_IFACE_FLOW_DISABLED:
#ifdef FEATURE_HDR
#if   defined(FEATURE_DATA_FL_FLOW_CONTROL)
        /*---------------------------------------------------------------------
          Send DOWN command to CCM
        ---------------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_MED, "RM FWD hi watermark reached, post DS707_CCM_FLOW_DOWN");
          ds707_ccm_mem_send_flow_control_cmd(DS707_CCM_MEM_WMK_SIO_FWD, DS707_CCM_FLOW_DOWN);
#else
        /*-------------------------------------------------------------------
          Enable Flow Control by sending a message to the HDR Base Station. 
          Enable Flow Control means disable the flow and thereby stop data 
          to flow from the base station to the mobile
        -------------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_MED, "RM FWD enable MAC flow control");
          hdrl1_fmac_enable_flow_control ((uint32)HDRFMAC_DRC_REQ_PSQ);
#endif /* FEATURE_DATA_ON_APPS */
#endif /* FEATURE_HDR */
          break;

        default:
          /* Invalid cmd */
          DS_FORCE_ASSERT();
          return;
      } /* switch( cmd ) */
      break;
    }
    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      break;
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsm_process_rm_iface_flow_cmd() */

/*===========================================================================
  FUNCTION DS707_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function processes the dial string it has registered for and
    initiates the appropriate data call over IS707_PKT_IFACE.

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Abort handler callback.  If there is an error, then kills the call
    via DCD and returns NULL.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
/* ARGSUSED */
dsat_call_abort_cb_type ds707_rmsm_dial_str_cb
(
  const byte *dial_string,
  boolean    digit_mode,
  dsat_dial_modifier_info_type  *modifiers,
  sys_modem_as_id_e_type        subs_id
)
{
  byte dial_str_after_trim[ MAX_PKT_DIAL_STRING_LEN];
  ds3g_siolib_port_alloc_info_s_type alloc_info; /* SIOLIB port allocation */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if subs_id passed is ps_subs_id */
  if (subs_id != ds3g_get_ps_subs_id() &&
      ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE)
  {
    MSG_ERROR("Non-PS subs_id: %d passed in dial_str_cb, failing call",
               subs_id, 0, 0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Register for desired events (dtr signal) with solib.  If the
    registration fails drop the call since siolib is already in use.
    Otherwise indicate to siolib that we are in call now.
  -------------------------------------------------------------------------*/
  memset( (void*)&alloc_info,
          0x0,
          sizeof(ds3g_siolib_port_alloc_info_s_type));
  alloc_info.mode = DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC;
  alloc_info.call_dir = DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED;
  
  if(DS3G_SIOLIB_PORTID_NONE !=
       (ds707_rmsmi_sio_port_allocated =
        ds3g_siolib_allocate_port( &alloc_info )) )
  {
  if( ds3g_siolib_register_callback_func( &ds707_rmsmi_sio_func_table ) == 
      FALSE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Call setup failed, port unavailable");
    (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
    dsat_send_result_code( DSAT_NO_CARRIER );
      ds3g_siolib_release_port ( ds707_rmsmi_sio_port_allocated );
      ds707_rmsmi_sio_port_allocated = DS3G_SIOLIB_PORTID_NONE;
    return(NULL);
  }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to allocate SIOLIB port");
    return(NULL);
  }
  
  ds3g_siolib_set_call_coming_up( TRUE );




  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {

    ds707_rmsmi_trim_dial_str(dial_string, dial_str_after_trim);
    /*-----------------------------------------------------------------------
      Get DATA_FORCE_CDMA_PKT_STR retrieved from NV and compare it with the
      dialstring. If they match, force the mode to be CDMA specific
    -----------------------------------------------------------------------*/
    if ( (ds707_pkt_mgr_force_cdma_str_len > 0 ) && 
         (ds707_pkt_mgr_force_cdma_str_len <= 16 ) &&
         (0 == memcmp((char *)dial_str_after_trim, 
                      (char *)ds707_pkt_mgr_force_cdma_dial_str,
                      ds707_pkt_mgr_force_cdma_str_len)) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "Pkt dial string forces CDMA Only Pkt Call" );
      ds707_rmsmi_info.pkt_info.force_cdma_only = TRUE;
    }
    else
    {
      ds707_rmsmi_info.pkt_info.force_cdma_only = FALSE;
    }
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "Pkt dial string recvd, posting "
                   "DS707_RMSM_RM_WANTS_PKT_CALL_EV");
  ds707_rmsmi_info.subs_id = subs_id;
  ds707_rmsm_post_event( DS707_RMSM_RM_WANTS_PKT_CALL_EV );

  return(ds707_rmsmi_call_abort_cb);

} /* ds707_rmsm_dial_str_cb() */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS707_RMSMI_BUILD_ACL_POLICY


  DESCRIPTION
    This function creates the ACL policy to send to routing layer

  PARAMETERS
    acl_policy_info_type*      acl_policy_info_ptr
      NON-NULL pointer to an acl policy structure

  RETURN VALUE
    Boolean TRUE: if successful
            FALSE: otherwise / or if NULL pointer to acl structre passed

  DEPENDENCIES
    NON-NULL pointer to an acl policy structure

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_rmsmi_build_acl_policy(
  acl_policy_info_type*  acl_policy_info_ptr,
  boolean                lookup_only_flag,
  boolean                bringup_flag
)
{
#ifdef FEATURE_DS_MOBILE_IP
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#endif 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(acl_policy_info_ptr == NULL)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     " Error: Policy ptr passed is NULL : 0x%x", acl_policy_info_ptr );
    return FALSE;
  }

  memset( acl_policy_info_ptr,
          0,
          sizeof(acl_policy_info_type) );

  acl_init_policy_info(acl_policy_info_ptr);

  acl_policy_info_ptr->iface.kind      = DSS_IFACE_NAME;
  acl_policy_info_ptr->iface.info.name = DSS_IFACE_CDMA_SN;
  acl_policy_info_ptr->policy_flag     = DSS_IFACE_POLICY_ANY;

  /* No change necessary since RMSM will handle only IPv4. RmNet will
     handle the IPv6 changes.
  */
  acl_policy_info_ptr->ip_family       = IFACE_IPV4_ADDR_FAMILY;

  acl_policy_info_ptr->lookup_only     = lookup_only_flag;
  acl_policy_info_ptr->bring_up        = bringup_flag;

  acl_policy_info_ptr->proc_id         = DS707_RMSMI_PROC_ID;
  acl_policy_info_ptr->app_identifier  = DS707_RMSMI_APP_ID;
  acl_policy_info_ptr->subs_id  =  (uint32) ds3gsubsmgr_subs_id_cm_to_ds(ds707_rmsmi_info.subs_id);

  /*------------------------------------------------------------------------
    set data session info in acl policy info struct. If FEATURE_JCDMA_1X
    is defined then set it to JCDMA INET profile. Else set it to DEFAULT
    profile.
  ------------------------------------------------------------------------*/
#ifdef FEATURE_DS_MOBILE_IP
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
#endif /*FEATURE_DS_MOBILE_IP*/
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    acl_policy_info_ptr->data_session_profile_id = 
                                            DATA_SESSION_JCDMA_INET_PROFILE;
  }
  else if( ds707_data_session_is_nvruim_profiles_available()
 #ifdef FEATURE_DS_MOBILE_IP
           &&( ds_1x_runtime_profile_ptr->mip.ds_qcmip_val ==  0 )
 #endif
         )
  {
    acl_policy_info_ptr->data_session_profile_id = 
      ds707_data_session_get_app_profile_index(TETHERED_MODE_APP_TYPE);
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "NVRUIM profile will be used for tethered call" );
  }
  else
  {
    acl_policy_info_ptr->data_session_profile_id =  DATA_SESSION_DEFAULT_PROFILE;
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
         "Legacy default profile id = %d will be used for tethered call", 
         acl_policy_info_ptr->data_session_profile_id );
  }

  acl_policy_info_ptr->ip_family       = IFACE_IPV4_ADDR_FAMILY;

  return TRUE;
}

/*===========================================================================
  FUNCTION DS707_RMSMI_GET_UM_IFACE_PTR


  DESCRIPTION
    This function retrieves the pointer to UM iface in the system which can
    be used to originate the 1x packet data call.  The iface pointer is 
    obtained by routing lookup.  

    An UM iface is returned only if there is one available and it is not
    in IFACE_DOWN state.

  PARAMETERS
    None

  RETURN VALUE
    Pointer to allocated interface, or NULL if none available

  DEPENDENCIES
    Note that context lock is required because sockets calls originate
    in PS task context.  So we need to make sure that if we decide to
    originate on a particular iface, IFACE_state should be set to
    something other than DOWN and DISABLED and the UM packet iface
    ACLs should be disabled to prevent sockets call before
    freeing up the locks. This ensures only one type of call originates
    at a time on each interface.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL ps_iface_type* ds707_rmsmi_get_um_iface_ptr
(
  void
)
{
  ps_iface_type *           um_iface_ptr;
  acl_policy_info_type      acl_policy_info;
  ps_iface_state_enum_type  um_state;
  boolean                   got_arb_candidate_iface = FALSE;
  uint32                    iface_priority_mask = 0; /* Not being used right now */
  ds707_mip_profile_type    *ds_1x_runtime_profile_ptr = NULL;
  ps_iface_type          *iface_ptr             = NULL;
  ds707_iface_table_type *iface_tbl_ptr         = NULL;
  uint8                    iface_index;
  ps_iface_state_enum_type iface_state;
  ps_iface_net_down_reason_type net_down_reason;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val != 
                             DS_CRM_VAL_NETMODEL_MODE )
  {
    if(FALSE == ds707_rmsmi_build_acl_policy(&acl_policy_info, FALSE, TRUE))
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Could NOT build ACL policy");
      return NULL;
    }
  }
  else
  {
    if(FALSE == ds707_rmsmi_build_acl_policy(&acl_policy_info, TRUE, FALSE))
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Could NOT build ACL policy");
      return NULL;
    }
  }
  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "ds707_rmsmi_get_um_iface_ptr - ACL policy built ");

#ifdef FEATURE_EHRPD
  if (ds707_pkt_is_ehrpd_mode_of_operation())
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "eHRPD mode of operation RMSM Calls not allowed");
    return NULL; 
  }

  /* A DUN call attempted in a EPC system should be trashed */

  if( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM )
  {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                         "NON-EPC call requested when system is EPC");
        return NULL;
  }
#endif /* FEATURE_EHRPD */

  for (iface_index = 0;
       iface_index < DS707_MAX_IFACES;
       iface_index++)
  {
    iface_tbl_ptr = ds707_get_iface_tbl_from_instance(iface_index);
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl(iface_tbl_ptr);

    iface_state = ps_iface_state(iface_ptr);
    if ((TRUE == ps_iface_addr_family_is_v6(iface_ptr))
       && (IFACE_DOWN != iface_state)
       && (IFACE_STATE_INVALID != iface_state)
       && (IFACE_DISABLED != iface_state))
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "V6 iface is in use, RMSM Calls not allowed");
      return NULL;       
    }
  }

  /*---------------------------------------------------------------------
    check if the default v4 iface is in lingering state
    check for the compatibility of the iface profiles and laptop profiles
    if both the above checks pass then 
        - register for iface down event
        - call ps_iface_go_null_cmd()
        - deregister for the event
        - in the down event call back call 
          ds707_rmsmi_process_rm_wants_pkt_call_ev
  ---------------------------------------------------------------------*/
  if ( ds707_rmsmi_process_lingering_call(&acl_policy_info) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "Current lingering V4 iface needs to be torn down");
    return NULL;
  }

  um_iface_ptr = NULL;

  um_iface_ptr = ps_route_look_up_iface_by_policy(&acl_policy_info,
                                                  &got_arb_candidate_iface,
                                                  &iface_priority_mask,
                                                  &net_down_reason);

  if(um_iface_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "ps_route_look_up_iface_by_policy returned NULL iface");
    return NULL;
  }

  DATA_IS707_MSG3(MSG_LEGACY_HIGH, 
          "ps_route_look_up_iface_by_policy returned iface: 0x%x:%d, arb: %c",
          um_iface_ptr->name,
          um_iface_ptr->instance,
          "FT"[(got_arb_candidate_iface==0)?0:1]);

  if(um_iface_ptr->name == EPC_IFACE) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Denying DUN call because EPC iface returned" );
    um_iface_ptr = NULL;
  }

  if( (got_arb_candidate_iface == TRUE) && 
      (ds707_rmsm_will_be_relay_model_call()) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "Not bringing up Relay call because iface is Arb candidate");
    um_iface_ptr = NULL;
  }

  if(um_iface_ptr != NULL)
  {
    if(got_arb_candidate_iface == FALSE)
    {
      /* If iface is Arb candidate, Arb mgr will handle state management, 
         so we will just retrurn the obtained iface */
      /* However, if we are here, this means that iface was not arb candidate,
         so lets do some state checks */
      um_state = ps_iface_state(um_iface_ptr);

      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "um_pkt_iface in state: %d", um_state);
  
      if( ( um_state != IFACE_DOWN) && 
          ( um_state != IFACE_GOING_DOWN) )
      {
        /* Invalid state */
        um_iface_ptr = NULL;
      }
    }
  } 

  if(um_iface_ptr != NULL)
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "ds707_rmsmi_get_um_iface_ptr: returning: 0x%x:%d ",
                     um_iface_ptr->name,
                     um_iface_ptr->instance);

  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "ds707_rmsmi_get_um_iface_ptr: returning: NULL");
  }

  return um_iface_ptr;

} /* ds707_rmsmi_get_um_iface_ptr() */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_WANTS_PKT_CALL_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_WANTS_PKT_CALL_EV event. Changes
    state to DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE or
    DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE or
    DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_wants_pkt_call_ev
(
  void
)
{ 
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
  ps_sys_event_dun_ctrl_entitlement_check_info_type dun_ctrl_ev_info;
  int16 rval, ps_error=0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "Recvd RM_WANTS_PKT_CALL_EV, state %d subs id = %d",
                   ds707_rmsmi_info.state,ds707_rmsmi_info.subs_id);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd RM_WANTS_PKT_CALL_EV, state '%s'",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
             SEND PS_SYS Event for  Entitlement check
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Entitlement Check.");
      dun_ctrl_ev_info.tech_type = (ps_sys_tech_enum_type)DS_SYS_TECH_3GPP2;
      dun_ctrl_ev_info.cb_data = 0;
      /* Generate DUN_CTRL_ENTITLEMENT events */
      rval = ps_sys_event_ind( PS_SYS_TECH_3GPP2, 
                               PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                               (void *)&dun_ctrl_ev_info,
                               &ps_error);
      if(rval==0)
      {
        ds707_rmsmi_transition_state( 
                              DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE );
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                        "Moving to WAITING_FOR_ENTITLEMENT_CHECK_STATE");
        ds707_rmsmi_info.entitlement_check_req = TRUE;
        break;
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ps_sys_event_ind failed %d", 
                        ps_error);
      }
    }

    case DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    {
      /*---------------------------------------------------------------------
        Bring up a data call if we have coverage (IFACE state != DISABLED)
        and if there is no call currently active over IS707 packet 
        (IFACE state == DOWN) or async interface (since all async calls are
        laptop calls, an earlier check on SIOLIB availability suffices 
        for now). Note that context lock is required because sockets calls 
        originate in PS task context.
        So we need to make sure that if we decide to originate,
        IFACE state should be set to something other than DOWN (and DISABLED)
        (to prevent async call) before freeingup the locks. This ensures 
        only one type of 1x call originates at a time.
      ---------------------------------------------------------------------*/

      ds_1x_profile_store();

#ifdef FEATURE_MMGSDI
      if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {
        if (ds707_get_jcdma_card_info( )== MMGSDI_JCDMA_GRAY_CARD)
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                           "UIM card color is MMGSDI_JCDMA_GRAY_CARD. "
                           "Rejecting Data Call.");
          if (ds707_rmsmi_info.entitlement_check_req == FALSE)
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Call Failed. Cleaning SIO");
            (void)ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
            dsat_send_result_code( DSAT_NO_CARRIER );
            if(ds707_rmsmi_sio_port_allocated != DS3G_SIOLIB_PORTID_NONE)
            {
              ds3g_siolib_set_call_coming_up( FALSE );
              (void)ds3g_siolib_deregister_callback_func( 
                                              &ds707_rmsmi_sio_func_table );
              ds3g_siolib_release_port ( ds707_rmsmi_sio_port_allocated );
              ds707_rmsmi_sio_port_allocated = DS3G_SIOLIB_PORTID_NONE;
            }
          }
          else 
          {
             DATA_IS707_MSG0( MSG_LEGACY_ERROR,"Moving to NULL State");
             ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          }
          return;
        }
      }
#endif /* FEATURE_MMGSDI */     

      ds707_rmsmi_info.IS707pkt_iface_ptr = ds707_rmsmi_get_um_iface_ptr();
     
      /*---------------------------------------------------------------------
        check if the rmsmi_reestablish_um_link flag is set (i.e. if current 
        lingering PPP needs to be brought down) and the um iface ptr returned
        is NULL. If both these checks are true then just return from this 
        function or else continue.
      ---------------------------------------------------------------------*/
      if ( (rmsmi_reestablish_um_link == TRUE) && 
           (ds707_rmsmi_info.IS707pkt_iface_ptr == NULL) )
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                         "Current lingering V4 iface needs to be torn down");
        return;
      }
     
      if (ds707_rmsmi_info.IS707pkt_iface_ptr == NULL)
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Iface ptr is NULL");
        if (ds707_rmsmi_info.entitlement_check_req == FALSE)
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,"Call Failed. Cleaning SIO");
          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
          dsat_send_result_code( DSAT_NO_CARRIER );
          if(ds707_rmsmi_sio_port_allocated != DS3G_SIOLIB_PORTID_NONE)
          {
            ds3g_siolib_set_call_coming_up( FALSE );
            (void) ds3g_siolib_deregister_callback_func( 
                                              &ds707_rmsmi_sio_func_table );
            ds3g_siolib_release_port ( ds707_rmsmi_sio_port_allocated );
            ds707_rmsmi_sio_port_allocated = DS3G_SIOLIB_PORTID_NONE;
          }
        }
        else 
        {
           DATA_IS707_MSG0( MSG_LEGACY_ERROR,"Moving to NULL State");
           ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        }
        return;
      }

       /*-------------------------------------------------------------------
         IS707 pkt iface is available to originate a data call.
       -------------------------------------------------------------------*/
          
       /*-------------------------------------------------------------------
         Register for flow disbale for relay model calls only. For netmodel
         calls, PS stops pulling data off the queue if flow is disabled.          
       -------------------------------------------------------------------*/
  
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val != 
                                 DS_CRM_VAL_NETMODEL_MODE )
      {
        if(ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_FLOW_DISABLED_EV,
                                     ds707_rmsmi_info.flow_disable_buf_ptr
                                    ) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                           "FLOW_DISABLED_EV reg failed" );
          ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          /* CALL FAILED */
          return;
        }
        /*-----------------------------------------------------------------
          Register for the flow disabled phys_link events.
        -----------------------------------------------------------------*/
        if(ps_phys_link_event_cback_reg( PS_IFACE_GET_PHYS_LINK(
                                         ds707_rmsmi_info.
                                         IS707pkt_iface_ptr  ),
                                         PHYS_LINK_FLOW_DISABLED_EV,
                                         ds707_rmsmi_info.
                                         um_phys_link_flow_disable_buf_ptr
                                       ) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                           "Phys_link FLOW_ENABLED_EV reg failed" );
          ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          /* CALL FAILED */
          return;
        }
  
        if(ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                     IFACE_FLOW_ENABLED_EV,
                                     ds707_rmsmi_info.flow_enable_buf_ptr
                                   ) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                           "IFACE FLOW_ENABLED_EV reg failed" );
          ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          /* CALL FAILED */
          return;
        }
          
        /*-------------------------------------------------------------------
          Register for the flow enabled phys_link events.
        -------------------------------------------------------------------*/
        if(ps_phys_link_event_cback_reg( PS_IFACE_GET_PHYS_LINK(
                                         ds707_rmsmi_info.
                                         IS707pkt_iface_ptr  ),
                                         PHYS_LINK_FLOW_ENABLED_EV,
                                         ds707_rmsmi_info.
                                         um_phys_link_flow_enable_buf_ptr
                                        ) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "Phys_link FLOW_ENABLED_EV reg failed" );
          ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          /* CALL FAILED */
          return;
        }

        if(ps_flow_event_cback_reg( 
            PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr  ),
            FLOW_TX_ENABLED_EV,
            ds707_rmsmi_info.um_flow_tx_enable_buf_ptr) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                           "FLOW_TX_ENABLED_EV reg failed" );
          ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          /* CALL FAILED */
          return;
        }
 
        /*-------------------------------------------------------------------
          Register for the flow_flow disabled events.
        -------------------------------------------------------------------*/
        if(ps_flow_event_cback_reg( 
            PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr  ),
            FLOW_TX_DISABLED_EV,
            ds707_rmsmi_info.um_flow_tx_disable_buf_ptr) < 0 )
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "FLOW_TX_DISABLED_EV reg failed");
          ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED );
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          /* CALL FAILED */
          return;
        }

      }

      if (ds707_rmsmi_info.entitlement_check_req != FALSE)
      {
        ds707_rmsmi_info.entitlement_check_req = FALSE;
      }

      ps_iface_enable_ind( &ds707_rmsmi_info.sio_pkt_iface );

      ds707_netmdl_reset_resync_callbacks();
      
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                  DS_CRM_VAL_NETMODEL_MODE )
      {
#ifdef FEATURE_DS_MOBILE_IP
        if ( ds_1x_runtime_profile_ptr->mip.ds_qcmip_val > 0 )
        {
            /*---------------------------------------------------------------
             Originate a MIP network model IS707 pkt call
           ---------------------------------------------------------------*/
          ds707_rmsmi_transition_state(
             DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE);
        }
        else
#endif /* FEATURE_DS_MOBILE_IP */
        {
          /*-----------------------------------------------------------------
            Originate a SIP network model IS707 pkt call
          -----------------------------------------------------------------*/
          ds707_rmsmi_transition_state(
                                    DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE);
        }
      }
      else
      {
         /*-----------------------------------------------------------------
           Originate a Relay network model IS707 pkt call
         -----------------------------------------------------------------*/
        ds707_rmsmi_transition_state(
                                 DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE);
      }
      
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    /*---------------------------------------------------------------------
      ignore the event in this state
    ---------------------------------------------------------------------*/
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    {
      /* Ignore */
      break; 
    }
#endif /* FEATURE_DS_MOBILE_IP */

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_rm_wants_pkt_call_ev() */


#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_MIP_IP_ADDR_CHANGED_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_MIP_IP_ADDR_CHANGED_EV event.
    Dependign on the state when this event is received it tearsdown the
    call on IP address change

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/

LOCAL void ds707_rmsmi_process_um_mip_ip_addr_changed_ev
(
  void
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_MIP_IP_ADDR_CHANGED_EV, state %d", 
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_MIP_IP_ADDR_CHANGED_EV, state '%s'",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  switch( ds707_rmsmi_info.state )
  {

    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "Drop call : IP address changed ");
      ds707_rmsmi_transition_state(
                            DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE);
      break;
    }
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:     
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:  
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_mip_ip_addr_changed_ev() */

#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_IFACE_DOWN_EV event. Changes state 
    to DS707_RMSM_WAITING_FOR_UM_DOWN_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_iface_down_ev
(
  void
)
{
  int16  ps_iface_errno;
  int16  ret_val;
  ps_phys_link_type   *phys_link_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd RM_IFACE_DOWN_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd RM_IFACE_DOWN_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    {
      
      /*---------------------------------------------------------------------
        We are tearing down the 707 pkt iface for Relay model
        call here. Since PPP connection is between TE2-IWF, all we need
        to do is bring down the traffic channel.
      ---------------------------------------------------------------------*/

      ps_flow_go_null_ind(
            PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr),
            PS_EIC_NOT_SPECIFIED);

      ret_val = (int16)ps_phys_link_go_null_cmd (
                PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr),
                &ps_iface_errno, NULL);
      /*----------------------------------------------------------------------
	     In the case where the flows are still bound to the phy link, we should 
	     send a ps_iface_down_ind
	     ----------------------------------------------------------------------*/
      if(ret_val == 0)
      {
        /*-------------------------------------------------------------------
          Phys link shared and still in use on another iface. No need to wait
          for phys_link_down - Send a ps_iface_down_ind.
         -------------------------------------------------------------------*/
        ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                 PS_NET_DOWN_REASON_NOT_SPECIFIED );
        ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        
      }
      break;
    }
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
#ifdef FEATURE_CALL_THROTTLE /*dctm 2.0*/
        DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                         "RM iface down in %d state, post dctm ev",
                         ds707_rmsmi_info.state);
        dctm_post_sip_fail();
#endif /* FEATURE_CALL_THROTTLE */
      //lint -fallthrough
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
      ds707_cleanup_um(&ps_iface_errno);
      break;    
    
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    /*-----------------------------------------------------------
      If phys link on UM is in PHYS_LINK_UP state or iface is set to 
      graceful close call ppp stop on the UM device.
      Else Fall through.
    -------------------------------------------------------------*/
      if (( ps_iface_phys_link_state(ds707_rmsmi_info.IS707pkt_iface_ptr) ==
                                                            PHYS_LINK_UP ) ||
           ( PS_IFACE_GET_GRACEFUL_CLOSE(ds707_rmsmi_info.IS707pkt_iface_ptr)
                                                                   == TRUE) )
      {
        phys_link_ptr = PS_IFACE_GET_PHYS_LINK(
                                  ds707_rmsmi_info.IS707pkt_iface_ptr);
        if (phys_link_ptr == NULL)
        {
           DATA_IS707_MSG0(MSG_LEGACY_ERROR,"PHYS LINK PTR IS NULL!");
        }
        else
        {
           if ((DS_FLOW_CTRL_IS_BIT_SET(
                ps_phys_link_get_flow_mask(phys_link_ptr),
                DS_FLOW_NO_SERVICE_MASK)) != TRUE)
           {
             DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Calling PPP stop on UM device");
             (void)ppp_stop(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP);
           }
           else
           {
             DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Calling PPP abort on UM device");
             (void)ppp_abort(PPP_UM_SN_DEV);
           }
        }
        break;
      }
    
      ds707_cleanup_um(&ps_iface_errno);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    /*----------------------------------------------------------------------
      For all the following states, PPP_CLOSE cmd will eventually result in
      transition to RMSM_NULL state.
    ----------------------------------------------------------------------*/
    /*----------------------------------------------------------------------
       when user cancel orig before phy_link up - Throttle,
       Tear down the phys link and this will make sure UM IFace goes down 
       and RMSM state moves to NULL state
      ---------------------------------------------------------------------*/
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:      
#ifdef FEATURE_CALL_THROTTLE
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "RM iface down in %d state, post dctm mip ev",
                       ds707_rmsmi_info.state);
      dctm_post_mip_call_status(MIP_FAILURE_EV,MIP_FAIL_REASON_NONE);
#endif /* FEATURE_CALL_THROTTLE */

      ps_flow_go_null_ind(
            PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr),
            PS_EIC_NOT_SPECIFIED);

      ps_phys_link_go_null_cmd(
            PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr),
            &ps_iface_errno, NULL);
      break;
    /*-----------------------------------------------------------------------
       DTR drop during  PPP negotiation / MIP registration - throttle
     ----------------------------------------------------------------------*/
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    {
#ifdef FEATURE_CALL_THROTTLE
        DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                         "RM iface down in %d state, post dctm mip ev",
                         ds707_rmsmi_info.state);
        dctm_post_mip_call_status(MIP_FAILURE_EV,MIP_FAIL_REASON_NONE);
#endif /* FEATURE_CALL_THROTTLE */
    /* intentional fall through*/
    }
    //lint -fallthrough
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
   {
      if(ps_iface_go_null_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                                &ps_iface_errno,
                                NULL ) == 0 )
      {
        /*-------------------------------------------------------------------
          If the IS707 iface is already down we should cleanup the call
          on Rm since we won't get any other notification from IS707
          controller
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
        break;
      }
    }
    //lint -fallthrough

    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_UM_DOWN_STATE );
      break;
    }

    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      break;
    }

    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_rm_iface_down_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_IFACE_DOWN_EV event. Changes state 
    to DS707_RMSM_NULL_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_iface_down_ev
(
  void
)
{
#ifdef FEATURE_DS_MOBILE_IP
  ds707_mip_profile_type  *ds_1x_runtime_profile_ptr = NULL;
  ps_phys_link_type *ps_phys_link_ptr;
  ds_fallback_enum_type ds_fallback;
  ds_fallback = DS_MIP_TO_SIP_FALLBACK;
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Recvd UM_IFACE_DOWN_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_IFACE_DOWN_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

#ifdef FEATURE_DS_MOBILE_IP
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
#endif /*FEATURE_DS_MOBILE_IP*/
  switch( ds707_rmsmi_info.state )
  {

    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                      "In UM_UP_SIP_STATE - Falling through");
    } /* case fall through */
#ifdef FEATURE_DS_MOBILE_IP
      /*-------------------------------------------------------------------
        We do not want to fall back to SIP here even if qcmip==1 since we
        never got the traffic channel.  Fail the call right away.
      -------------------------------------------------------------------*/
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    {
       DATA_IS707_MSG2(MSG_LEGACY_HIGH, "In UM iface down ev - "
                      "May try Bringup again, brought_up: %d, reestablish: %d",
                       rmsmi_IS707pkt_iface_brought_up,
                       rmsmi_reestablish_um_link);

       if(ds707_rmsmi_info.IS707pkt_iface_ptr != NULL)
       {
         DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                        "De-registering for IFACE_DOWN_EV for iface: 0x%x::%d",
                         ds707_rmsmi_info.IS707pkt_iface_ptr->name,
                         ds707_rmsmi_info.IS707pkt_iface_ptr->instance );

         ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                       IFACE_DOWN_EV,
                                       ds707_rmsmi_info.um_iface_down_buf_ptr);
       }
       else
       {
         DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not deregister for "
                         "UM IFACE DOWN Event, because iface is NULL");
       }
     
    } /* case: fall through */
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    {
      /*-------------------------------------------------------------------
        The 707 pkt iface is down now, we should also terminate the call
        on Rm. For Relay and Nemodel SIP calls there isn't any PPP running
        on SIO iface.  Only final cleanup needs to be performed.
      -------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "Final Fall through - Going to NULL state");
      ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      if(ppp_get_phase( PPP_RM_DEV )== pppDEAD )
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                         "PPP(RM) is down, Change state to NULL_STATE");
        /* PPP(RM) interface is already down */
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                   "PPP(RM) is up, Change state to WAITING_FOR_RM_DOWN_STATE");
        /* PPP(RM) interface is still up */
        ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_RM_DOWN_STATE );
      }

      break;
    }
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
#endif /*FEATURE_DS_MOBILE_IP*/
    {
      ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_RM_DOWN_STATE );
      break;
    }
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    {
      if (ds_1x_runtime_profile_ptr->mip.ds_qcmip_val == 2)
      {
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else /* ds_qcmip_val == 1 */
      {
        ps_phys_link_ptr = 
                 PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr);
        if( (PS_PHYS_LINK_GET_STATE(ps_phys_link_ptr)!= 
                                                    PHYS_LINK_GOING_DOWN ) &&
            (PS_PHYS_LINK_GET_STATE( ps_phys_link_ptr)!= 
                                                      PHYS_LINK_GOING_NULL ))
        {
          /*-----------------------------------------------------------------
            Restore netmdl meta state to OOC and fallback to SIP
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_MED, "Fall back to Simple IP. "
                "In DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE - dereging events");

          event_report_payload( EVENT_DS_FALLBACK,
                            sizeof(ds_fallback_enum_type), 
                            (void*)&ds_fallback);

         (void) ds707_rmsmi_dereg_ev(ds707_rmsmi_info.IS707pkt_iface_ptr);

          /* Clean up the iface flage here */
          ds707_rmsmi_info.IS707pkt_iface_ptr = NULL;

            ds707_netmdl_reset_resync_callbacks();

            ds707_rmsmi_transition_state(
                                   DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE );
          }
        else
        {
          /*-----------------------------------------------------------------
            Wait for phys link to come down before we can make SIP call.
            PHYS_LINK_GONE_EV is deregistred on transitionto NULL or
            WAITING_FOR_UM_UP_SIP_STATE state.
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0( MSG_LEGACY_MED,
                           "Fallback to SIP: waiting for PHYS_LINK_GONE");

          event_report_payload( EVENT_DS_FALLBACK,
                            sizeof(ds_fallback_enum_type), 
                            (void*)&ds_fallback);
      
          ds707_rmsmi_transition_state(
                          DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE);
        }
        ds707_pkt_mgr_incr_num_mip_to_sip_transitions();
      }
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }  

    case DS707_RMSM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        If the reestablish_ppp flag is set then call 
        ds707_rmsmi_process_rm_wants_pkt_call_ev from here
      ---------------------------------------------------------------------*/
      if (rmsmi_reestablish_um_link == TRUE)
      {
        /*-------------------------------------------------------------------
          Dereg for UM Iface DOWN EV
        -------------------------------------------------------------------*/
        ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_DOWN_EV,
                                  ds707_rmsmi_info.um_iface_down_buf_ptr );
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "Re-initiating laptop call after coming out of"
                         "IFACE Lingering state");
        rmsmi_reestablish_um_link = FALSE;
        ds707_rmsmi_process_rm_wants_pkt_call_ev();
      }
      break;
    }
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /* deregister events */    
      (void) ds707_rmsmi_dereg_ev(ds707_rmsmi_info.IS707pkt_iface_ptr);

      if (ds707_pkt_mgr_all_ifaces_down()) 
      {
        
        /* All ifaces are down, bringup iface after setting proper state */
        DATA_IS707_MSG0(MSG_LEGACY_MED, " Bringing up iface" );  
        
        ds707_rmsmi_transition_state(ds707_rmsmi_info.state_before_arbitration);
        
        /* Bringup iface only if its not brounght up & RMSM is not in NULL state.
           RMSM might transition to NULL state if there is an attempt to bringup 
           the call during the above rmsm transtion state call & it failed */
        if ((FALSE == rmsmi_IS707pkt_iface_brought_up) 
            && (DS707_RMSM_NULL_STATE != ds707_rmsm_get_state()))
        {
           ds707_rmsmi_bringup_iface(ds707_rmsmi_info.client_data_ptr);
        }
      }
      else 
      {
        /* Get active iface and tear it down*/ 
        ds707_rmsmi_info.IS707pkt_iface_ptr = ds707_pdncntx_get_active_iface();
        
        DATA_IS707_MSG0(MSG_LEGACY_MED, " Bringing down active iface" );  

        if ( (ps_iface_event_cback_reg(
              ds707_rmsmi_info.IS707pkt_iface_ptr,
              IFACE_DOWN_EV,
              ds707_rmsmi_info.um_iface_down_buf_ptr)) < 0 )
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Disallowing the laptop call. "
                          "IFACE_DOWN_EV reg failed - so returning FAIL");

          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
          // Transition to NULL state
        } 
        else  
        {
          int16 ps_iface_errno;
          int ret_val = 0;

          /*---------------------------------------------------------------------
            Tear down active iface 
          ---------------------------------------------------------------------*/
          ret_val = ps_iface_go_null_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   &ps_iface_errno,
                                   NULL);
          if (0 == ret_val )
          {
            ds707_rmsm_post_event(DS707_RMSM_UM_IFACE_DOWN_EV);
          }
          else
          {
            if ((ret_val < 0) && (ps_iface_errno != DS_EWOULDBLOCK))
            {

              /*-------------------------------------------------------------------
                Since it failed lets go NULL state
              -------------------------------------------------------------------*/
              ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
              DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                               "tearing down failed, errno: %d", ps_iface_errno);
            } 
            else 
            {
              DATA_IS707_MSG0(MSG_LEGACY_MED, "Waiting for iface to be down");
            }
          }
        }
      }
      break;
    }
    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_iface_down_ev() */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_PHYS_LINK_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_PHYS_LINK_UP_EV event.
    Changes state to DS707_RMSM_UM_RM_UP_RELAY_STATE OR
     WAITING_FOR_E2E_PPP_UP_STATE ( for Netmdl )..

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_phys_link_up_ev
(
  void
)
{
  dsat_connect_rate_s_type connect_rate; /* reporting connect rate */
  ds707_extif_bearer_tech_rate_type bearer_tech_query;
  sys_sys_mode_e_type      sys_mode = SYS_SYS_MODE_NO_SRV;
  int16  ps_iface_errno;
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
  ps_iface_net_down_reason_type call_end_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
  ds707_data_session_profile_info_type *requesting_prof_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_PHYS_LINK_UP_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_PHYS_LINK_UP_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  memset((void *)&connect_rate,0x0, sizeof(dsat_connect_rate_s_type));
  memset((void *)&bearer_tech_query, 0x0,
                           sizeof(ds707_extif_bearer_tech_rate_type));

  
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    {
      /*------------------------------------------------------------------
        Check if call is allowed with this profile in the current config   
      -------------------------------------------------------------------*/
      ds707_data_session_get_iface_requesting_profile( 
                                      ds707_rmsmi_info.IS707pkt_iface_ptr,
                                      &requesting_prof_ptr);

      if (ds707_data_session_call_allowed(requesting_prof_ptr, &call_end_reason) == FALSE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                        "Profile not allowed in current config");
        ds707_cleanup_um(&ps_iface_errno);
        break;
      }

      /*---------------------------------------------------------------------
        When system changes from 1x<->DO, if a non-EPC call is attempted 
        before DS receives IDM change from CM, we should not allow the
        call till the EPC PPP is torn down. Hence set system to non-EPC
        only if there is no other call on EPC.
      ---------------------------------------------------------------------*/
      if( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                         "NON-EPC call requested when system is EPC");
        ds707_cleanup_um(&ps_iface_errno);
        break;
      }
      ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NON_EPC_SYSTEM);

      /*---------------------------------------------------------------------
        Since Relay/network model calls are up at this point, phys link
        going up or down only means dormancy from this point onwards. RMSM
        does not care about dormancy, hence dereg those events.
      ---------------------------------------------------------------------*/
      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_UP_EV,
                                  ds707_rmsmi_info.um_phys_link_up_buf_ptr );


      if( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                   DS_CRM_VAL_NETMODEL_MODE )
      {
        /*-------------------------------------------------------------------
          Configure PPP on both the interfaces and return CONNECT to initiate
          end to end PPP setup.
        -------------------------------------------------------------------*/
        ppp_get_default_opts( &ds707_rmsmi_info.rm_ppp_config );
        ds707_rmsmi_info.rm_ppp_config.rx_f_ptr       = 
          ds707_rmsmi_sio_rx_ppp_data;
        ds707_rmsmi_info.rm_ppp_config.tx_f_ptr       = 
          ds707_rmsmi_sio_tx_ppp_data;
        ds707_rmsmi_info.rm_ppp_config.ipcp_info.rx_iface_ptr   = 
          &ds707_rmsmi_info.sio_pkt_iface;
        ds707_rmsmi_info.rm_ppp_config.lcp_info.mode  = PPP_CP_MODE_PASSIVE;
        ds707_rmsmi_info.rm_ppp_config.ipcp_info.mode = PPP_CP_MODE_PASSIVE;
        ds707_rmsmi_info.rm_ppp_config.hdlc_mode      = HDLC_SW;
        ds707_rmsmi_info.rm_ppp_config.bridge_dev     = PPP_UM_SN_DEV;
        
        if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
        {
          ds707_configure_jcdma_params_for_ppp(
                                     &ds707_rmsmi_info.rm_ppp_config);
           
        }
        
        if( ppp_start( PPP_RM_DEV, &ds707_rmsmi_info.rm_ppp_config ) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                           "PPP RM config failed, posting RM_IFACE_DOWN" );
          ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
          break;
        }

        /* Assumes that the UM IFACE is already selected */
        if( ds707_pkt_setup_SN_ppp_for_rmsm() < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                           "PPP UM config failed, posting UM_PPP_DOWN" );
          ds707_rmsm_post_event( DS707_RMSM_UM_PPP_DOWN_EV );
          break;
        }

        if( NULL == ds707_rmsmi_info.ppp_rm_event_handle ||
            NULL == ds707_rmsmi_info.ppp_um_event_handle )
        {
          ERR_FATAL( "NULL PPP event handles", 0, 0, 0 );
        }
        else
        {
          (void)ppp_event_register( ds707_rmsmi_info.ppp_rm_event_handle,
                                    PPP_PROTOCOL_LCP,
                                    PPP_DOWN_EV_MASK );
          (void)ppp_event_register( ds707_rmsmi_info.ppp_um_event_handle,
                                    PPP_PROTOCOL_IPCP,
            (ppp_event_mask_e_type)((uint32)PPP_DOWN_EV_MASK | 
                                    (uint32)PPP_UP_EV_MASK | 
                                    (uint32)PPP_RESYNC_EV_MASK));

          (void)ppp_event_register( ds707_rmsmi_info.ppp_um_event_handle,
                                    PPP_PROTOCOL_LCP,
                                    PPP_DOWN_EV_MASK );
        }
        ds707_rmsmi_transition_state(
                     DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE );
      }
      else
      {
        ds707_rmsmi_transition_state(
                    DS707_RMSM_UM_RM_UP_RELAY_STATE );
      }

      ds707_rmsmi_setup_sio_watermarks(&ds707_rmsmi_info.sio_fwd_wmk,
                          &ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A]);

      (void)ds3g_siolib_change_mode( SIO_DS_PKT_MODE,
                          &ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A],
                          &ds707_rmsmi_info.sio_fwd_wmk,
                          NULL );
      connect_rate.call_type = DSAT_CALL_TYPE_1X;
      sys_mode = ds707_pkt_mgr_get_current_sys_mode();

      /* Check the sys mode acquired and set up the following data rates */
      if (SYS_SYS_MODE_CDMA == sys_mode )
      {
        bearer_tech_query.bearer_tech = DS707_EXTIF_1X;
      }
#ifdef FEATURE_HDR
      else if ( SYS_SYS_MODE_HDR == sys_mode )
      {
        sys_active_prot_e_type  prot = 
                                ds707_sys_chg_hdlr_get_active_hdr_protocol();
        if (SYS_ACTIVE_PROT_HDR_REL0 == prot )
        {
          bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REV0;
        }
        else if ( SYS_ACTIVE_PROT_HDR_RELA == prot )
        {
          bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REVA;
        }
        else if ( SYS_ACTIVE_PROT_HDR_RELB == prot )
        {
          bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REVB;
        }
        else
        {
          DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Invalid HRD PROT %d", prot);
        }
      }
#endif /* FEATURE_HDR */
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid Operating mode %d",
                        sys_mode);
      }

      (void)ds707_extif_get_bearer_tech_max_rate( &bearer_tech_query );
      connect_rate.rate.cdma_call.dl_rate = 
                                   bearer_tech_query.max_rx_bearer_tech_rate;
      connect_rate.rate.cdma_call.ul_rate = 
                                   bearer_tech_query.max_tx_bearer_tech_rate;

      DATA_IS707_MSG3( MSG_LEGACY_MED,
                       " Updating connect UL rate as %d DL rate %d for"
                       " call type %d",
                       connect_rate.rate.cdma_call.ul_rate,
                       connect_rate.rate.cdma_call.dl_rate,
                       connect_rate.call_type);
      /* Currently, not updating the protocol field as it is of no use. */
      dsat_report_rate_on_connect(connect_rate);
      dsat_send_result_code( DSAT_CONNECT );
      ds707_rmsmi_info.rmsm_sent_connect = TRUE;
      (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON );
      break;
    }
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    {
      /*---------------------------------------------------------------------
        deregister for phys_link_up event inside the state transition
      ---------------------------------------------------------------------*/
      ds707_rmsmi_transition_state(DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE);
      break;
    }

    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    case DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }
    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_phys_link_up_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_PHYS_LINK_GONE_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_PHYS_LINK_GONE_EV event. Changes 
    state to DS707_RMSM_NULL_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_phys_link_gone_ev
(
  void
)
{

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_PHYS_LINK_GONE_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_PHYS_LINK_GONE_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    {
      /*---------------------------------------------------------------------
        Phys link gone
      ---------------------------------------------------------------------*/
      ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
      break;
    }

    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    {
      /*-------------------------------------------------------------------
        Restore netmdl meta state to OOC and fallback to SIP
      -------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "Fallback to Simple IP. "
                       "In DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE "
                       "- dereging events");

      (void) ds707_rmsmi_dereg_ev(ds707_rmsmi_info.IS707pkt_iface_ptr);

      ds707_rmsmi_info.IS707pkt_iface_ptr = NULL;

        ds707_netmdl_reset_resync_callbacks();

        ds707_rmsmi_transition_state(
          DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE );
      break;
    }

    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_phys_link_gone_ev() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_PPP_DOWN_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_PPP_DOWN_EV event. Changes state to
    DS707_RMSM_WAITING_FOR_UM_DOWN_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_ppp_down_ev
(
  void
)
{
  int16  ps_iface_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_PPP_DOWN_EV, state %d", 
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_PPP_DOWN_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    {
      /*---------------------------------------------------------------------
        Since laptop has terminated the call, we need to cleanup Um before
        completely terminating the call. Tear down the 707 pkt iface first.
        Checking for the return code as the iface could already be down.
      ---------------------------------------------------------------------*/
      if ( ps_iface_go_null_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  &ps_iface_errno,
                                  NULL) == 0 )
      {
        /*-------------------------------------------------------------------
          If the IS707 iface is already down we should cleanup the call
          on Rm since we won't get any other notification from IS707
          controller
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          Otherwsie wait for IS707 iface to cleanup
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_UM_DOWN_STATE );
      }

      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      ds707_rmsmi_transition_state( 
                           DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE );
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_ppp_down_ev() */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_PPP_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_PPP_UP_EV event. Changes state to
    DS707_RMSM_UM_RM_UP_MIP_STATE or DS707_RMSM_RESYNCING_UM_MIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_ppp_up_ev
(
  void
)
{
  ppp_dev_opts_type *dev_opts = NULL;
  ps_ip_addr_type   ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Recvd RM_PPP_UP_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Recvd RM_PPP_UP_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    case DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------- 
        Allocate memory for dev_opts
      ----------------------------------------------------------------------*/
      dev_opts = (ppp_dev_opts_type*)
                 modem_mem_alloc(sizeof(ppp_dev_opts_type),
                                 MODEM_MEM_CLIENT_DATA_CRIT);
      if(dev_opts == NULL)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_rmsmi_process_rm_ppp_up_ev():"
                        "memory allocation failed for dev_opts");
        ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        break;
      }
      memset((void *)dev_opts, 0, sizeof(ppp_dev_opts_type));

      /*---------------------------------------------------------------------
        PPP sets the PDSN IP address into SIO iface since that address is 
        advertized as the peer address to TE2. Reset SIO iface address to 0 
        so that any stack write to this address may not get loopbacked.
      ---------------------------------------------------------------------*/
      /* No change necessary for Ipv6 since RMSM will handle only IPv4. 
         RmNet will handle the IPv6 changes. */
      ip_addr.type    = IPV4_ADDR;
      ip_addr.addr.v4.ps_s_addr = 0;
      (void) ps_iface_set_addr( &ds707_rmsmi_info.sio_pkt_iface, 
                                &ip_addr);

      /*---------------------------------------------------------------------
        Set up options to be set to Um either for immediate or future resync.
        Copy the working options from Rm to Um will so any handoffs/resyncs
        etc. will use the current options.
      ---------------------------------------------------------------------*/
      (void) ppp_get_bridge_dev_prot_opts( PPP_RM_DEV, dev_opts);

      dev_opts->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
      dev_opts->ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
      dev_opts->ipcp_info.will_mask[FSM_LOCAL] |= IPCP_N_PRIMARY_DNS |
                                                 IPCP_N_SECONDARY_DNS;
      dev_opts->ipcp_info.want_mask[FSM_LOCAL] |= IPCP_N_PRIMARY_DNS |
                                                 IPCP_N_SECONDARY_DNS;
      /*---------------------------------------------------------------------
        Strip the IP Addr option from local - we get IP addr from MIP reg.
      ---------------------------------------------------------------------*/
      dev_opts->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
      dev_opts->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;

      dev_opts->ipcp_info.want_vals[FSM_LOCAL].primary_dns.ps_s_addr = 0;
      dev_opts->ipcp_info.want_vals[FSM_LOCAL].secondary_dns.ps_s_addr = 0;
      /*---------------------------------------------------------------------
        get_peer_data_cback() should look in the iface cb to get the peer
        IP address, the following line will break it otherwise
      ---------------------------------------------------------------------*/
      dev_opts->ipcp_info.want_vals[FSM_REMOTE].address.ps_s_addr = 0;
      
      /*---------------------------------------------------------------------
        Check if the PPP options negotiated on Rm match those negotiated
        previously on the Um.
      ---------------------------------------------------------------------*/
      if( ds707_netmdl_compare_lcp_opts() == TRUE &&
          ds707_netmdl_compare_ipcp_opts() == TRUE )
      {
        /*-------------------------------------------------------------------
          Rm came up with options compatible with Um.
          
          Reconfigure the Um IPCP options to prepare for a subsequent 
              resync.  Um should ask for new DNS and peer IP address values.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                         "RM/UM negotiated opts match. "
                         "Saving LCP & IPCP options for future resync");
        
        ppp_update_proto_config( PPP_UM_SN_DEV, 
                                 PPP_PROTOCOL_LCP,
                                 dev_opts);
        ppp_update_proto_config( PPP_UM_SN_DEV,
                                 PPP_PROTOCOL_IPCP,
                                 dev_opts);

        /*-------------------------------------------------------------------
          Transition to MIP up state
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_UM_RM_UP_MIP_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          Rm came up with options incompatible with Um.
          Modify the Um PPP options to use the negotiated Rm options and 
          force a Um PPP resync.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                         "RM/UM negotiated opts differ, resyncing Um");
        (void)ppp_get_dev_setup (PPP_UM_SN_DEV, dev_opts);
        dev_opts->lcp_info.mode = PPP_CP_MODE_ACTIVE;
        dev_opts->ipcp_info.mode = PPP_CP_MODE_ACTIVE;
        
        
#ifdef FEATURE_CALL_THROTTLE
        /*-------------------------------------------------------------------
          MIP call  fail due to option  mismatch - Post Fail event to DCTM.
        -------------------------------------------------------------------*/
        dctm_post_mip_call_status( MIP_FAILURE_EV,
                                   MIP_FAIL_REASON_PDSN_FAILURE);
#endif /* FEATURE_CALL_THROTTLE */
        
        if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
        {
          ds707_configure_jcdma_params_for_ppp(dev_opts); 
        }

        /*-------------------------------------------------------------------
          Start PPP on Um and transition to resyncing state
        -------------------------------------------------------------------*/
        if (ppp_resync( PPP_UM_SN_DEV, 
                        PPP_PROTOCOL_LCP,
                        dev_opts) == -1)
        {
          /*-----------------------------------------------------------------
            If Um resync fails for some reason, kill the call
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0( MSG_LEGACY_MED, "Couldn't resync Um, kill call");
          ds707_rmsmi_transition_state( 
                            DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE );
        }
        else
        {
          ds707_rmsmi_transition_state( DS707_RMSM_RESYNCING_UM_MIP_STATE );
        }
      }
      modem_mem_free(dev_opts,MODEM_MEM_CLIENT_DATA_CRIT);
      break;
    }

    /*---------------------------------------------------------------------
      ignore the event in this state
    ---------------------------------------------------------------------*/
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
      break; 

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_rm_ppp_up_ev() */
#endif /* FEATURE_DS_MOBILE_IP */    


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_IFACE_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_IFACE_UP_EV event (which is only
    registered for during a MIP call). 
    Changes state to DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_iface_up_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_IFACE_UP_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_IFACE_UP_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif

  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        Um PPP up and mobile IP registration complete.
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );

      /*---------------------------------------------------------------------
        Now bring up Rm PPP
      ---------------------------------------------------------------------*/
      ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE );
      break;
    }

    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    {
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      /*---------------------------------------------------------------------
        Check if options match after Um resync
      ---------------------------------------------------------------------*/
      if( ds707_netmdl_compare_lcp_opts() == TRUE &&
          ds707_netmdl_compare_ipcp_opts() == TRUE )
      {
        /*-------------------------------------------------------------------
          If so, all is well.  Go to MIP up state.
        -------------------------------------------------------------------*/
        ds707_rmsmi_transition_state( DS707_RMSM_UM_RM_UP_MIP_STATE );
      }
      else
      {
        /*-------------------------------------------------------------------
          If not, we give up.  Bring down the MIP first.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                         "Um & Rm PPP option mismatch, bring down MIP");
        /* 
         * We have to bring down MIP call first before tearing down
         * UM iface. Otherwise, MIP de-registration can not be sent
         * out because of iface is in going_down state 
         */
        mip_meta_sm_post_event( MSM_BRING_DOWN_MIP_EV, 
                                ds707_rmsmi_info.IS707pkt_iface_ptr );
      }
      break;
    }

    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        Put the Um back in routeable (dssnet had set it to UP)
        This will hapen everytime MIP rereg happens due to a PPP resync.
        Note the PPP resync after the call is setup is handled through 
        netmodel. 
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
      Ignore event in all other states.
    -----------------------------------------------------------------------*/
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    case DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      break;
    }

    default:
    {
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_FORCE_ASSERT();
    }
  } /* switch( ds707_rmsmi_info.state ) */
} /* ds707_rmsmi_process_um_iface_up_ev() */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_E2E_PPP_UP_EV()

  DESCRIPTION
    This function processes DS707_RMSM_E2E_PPP_UP_EV 
    Changes state to E2E_PPP_UP_SIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_e2e_ppp_up_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Recvd E2E_PPP_UP_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd E2E_PPP_UP_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif
  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    {
      /*--------------------------------------------------------------------
        PPP UP received while  phys link call is not yet UP.
      --------------------------------------------------------------------*/
      ASSERT(0);
      break;
    }

    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    {      
      /*--------------------------------------------------------------------
        Setup bridging Um <-> Rm ifaces.
      --------------------------------------------------------------------*/
      ppp_set_mode( PPP_RM_DEV, 
                    PPP_INTERNAL_SNOOP_MODE );
      ppp_set_mode( PPP_UM_SN_DEV, 
                    PPP_INTERNAL_SNOOP_MODE );

      ps_iface_set_bridge(&ds707_rmsmi_info.sio_pkt_iface,
                          ds707_rmsmi_info.IS707pkt_iface_ptr);
      ps_iface_set_bridge(ds707_rmsmi_info.IS707pkt_iface_ptr,
                          &ds707_rmsmi_info.sio_pkt_iface);
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_pkt_iface );
      ps_iface_routeable_ind( &ds707_rmsmi_info.sio_pkt_iface );
      ds707_rmsmi_transition_state(DS707_RMSM_E2E_PPP_UP_SIP_STATE);
      break;
    }

    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*--------------------------------------------------------------------
         Got PPP UP while call is already UP or call is going down - Ignore
      --------------------------------------------------------------------*/
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    default:
    {
      ASSERT(0);
    }
  }/* switch */
} /* ds707_rmsmi_process_e2e_ppp_up_ev() */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_UM_RESYNC_EV()

  DESCRIPTION
    This function processes DS707_RMSM_UM_RESYNC_EV
    Changes state to RESYNCING_UM_SIP_STATE

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_um_resync_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Recvd UM_RESYNC_EV, state %d", 
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd UM_RESYNC_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif
  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    {
      DATA_ERR_FATAL("PPP event received while in NULL state ");
      return;
    }
    //lint -fallthrough
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*--------------------------------------------------------------------
         Got RESYNC while waiting for PPP UP or call already down - Ignore
      --------------------------------------------------------------------*/
      break;
    }
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    {
      ds707_rmsmi_transition_state( DS707_RMSM_RESYNCING_UM_SIP_STATE );
      break;
    }
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    {
      /*--------------------------------------------------------------------
         Got RESYNC while waiting for MIP UP or call is already down -Ignore
      --------------------------------------------------------------------*/
      break;
    }
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      ds707_rmsmi_transition_state(
                               DS707_RMSM_RESYNCING_UM_MIP_STATE);
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */
  default:
    {
      break;
    }
  }/* switch */
} /* ds707_rmsmi_process_um_resync_ev() */

/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_RM_RESYNC_EV()

  DESCRIPTION
    This function processes DS707_RMSM_RM_RESYNC_EV
    Changes state to WAITING_FOR_E2E_PPP_UP_SIP_STATE.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_rm_resync_ev
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Recvd RM_RESYNC_EV, state %d",
                   ds707_rmsmi_info.state);
#else
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Recvd RM_RESYNC_EV, state '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state]);
#endif
  switch( ds707_rmsmi_info.state )
  {
    case DS707_RMSM_NULL_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /*--------------------------------------------------------------------
        Got RESYNC while waiting for PPP UP or call is already down -Ignore
      --------------------------------------------------------------------*/
      break;
    }
    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    {
      ds707_rmsmi_transition_state(
                                DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE);
      break;
    }
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    default:
    {
     ASSERT(0);
    }
  }/* switch */
} /* ds707_rmsmi_process_rm_resync_ev() */

/*===========================================================================
  FUNCTION DS707_RMSMI_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    Its the caller's responsibility that it initiates only valid state 
    transitions. This function does not check if a state transition is 
    valid or not.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_transition_state
(
  ds707_rmsmi_state_type new_state
)
{
#ifdef FEATURE_DS_MOBILE_IP
  ppp_dev_opts_type  *dev_opts = NULL;
  ps_ip_addr_type    ip_addr;
  sys_sys_mode_e_type      sys_mode = SYS_SYS_MODE_NO_SRV;
  ds707_iface_table_type*  ds707_iface_tbl_ptr;
  dssnet4_sm_cb_type*      dssnet4_sm_cb_ptr; 
#endif /* FEATURE_DS_MOBILE_IP */
  int16  ps_iface_errno;
  dsat_connect_rate_s_type connect_rate; /* report connecting rate */
  ds707_extif_bearer_tech_rate_type bearer_tech_query;
  ds707_rmsmi_bringup_errno_type bringup_err;
  qword                          ts_null;
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type* mip_perf_info_ptr = NULL;
#endif
  sio_ioctl_param_type wait_param;
  mc_queue_cmd_status_type       queue_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "RMSM state goes %d to %d",
                   ds707_rmsmi_info.state, 
                   new_state);
#else
  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "RMSM state goes '%s' to '%s'\n",
                   ds707_rmsmi_state_names[ds707_rmsmi_info.state],
                   ds707_rmsmi_state_names[new_state] );
#endif

  /*------------------------------------------------------------------------ 
    Initialize ts_null
  ------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

  memset( (void *)&connect_rate,
          0x0,
          sizeof(dsat_connect_rate_s_type));
  memset( (void *)&bearer_tech_query, 
          0x0,
          sizeof(ds707_extif_bearer_tech_rate_type));
  
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  ds707_rmsmi_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    case DS707_RMSM_NULL_STATE:
    {   
      /*------------------------------------------------------------------
        If we got to NULL state as a result of a failure from ent check,
        we need no further clean up here as ppp would not have been started and
        no event registration would have happened
      ------------------------------------------------------------------*/
      if(ds707_rmsmi_info.entitlement_check_req == TRUE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED,"Reset ent_chk_reg flag");
        ds707_rmsmi_info.entitlement_check_req = FALSE;
        ds707_rmsmi_sio_cleanup(); 
        return;
      }
      /* lets record right away that um iface has not been brought up */
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "In NULL state - "
                      "Setting brought up flag & reestablish flag to FALSE");
      rmsmi_IS707pkt_iface_brought_up = FALSE;
      rmsmi_reestablish_um_link = FALSE;
      rmsmi_cnt_iface_down_ev_regd = 0;

#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_PERF)
      /*-------------------------------------------------------------------
        Cancel MIP call setup timer
      -------------------------------------------------------------------*/
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
      mip_perf_delay( mip_perf_info_ptr,
                      PERF_TOTAL_CALL_SETUP_DELAY,
                      PERF_CANCEL_TS );
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_PERF */
      /*---------------------------------------------------------------------
        Aggregation might be running still.Cancel the timer.
        call the following ioctl in sio to clear the set wait variable.
      ---------------------------------------------------------------------*/
      if (ds707_ds_smem_tx_delay )
      {
        wait_param.wm_wait = FALSE;
        ds3g_siolib_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param);
        timer_undef(&ds707_smd_notify_delay_timer);
      }

      /*---------------------------------------------------------------------
        Pkt call is terminating, Perform all the necessary cleanup
      ---------------------------------------------------------------------*/
      ds3g_siolib_set_inbound_flow( DS_FLOW_707_RMSM_MASK, 
                                    DS_FLOW_DISABLE );

      (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
      (void) ds3g_siolib_change_mode( SIO_DS_AUTODETECT_MODE, 
                                      NULL,
                                      NULL,
                                      NULL );

      /*---------------------------------------------------------------------
        Pkt call is terminating. Check why it is terminating and if CM 
        throttled the call, then send a DSAT_DELAYED response.
      ---------------------------------------------------------------------*/
      if ((ds707_roaming_is_curr_mode_jcdma() == TRUE) &&
          (ds707_pkt_mgr_get_last_orig_fail_reason() ==  
                                            DS707_ORIG_FAIL_CALL_THROTTLE))
      {
          dsat_send_result_code( DSAT_DELAYED );
          DATA_IS707_MSG0( MSG_LEGACY_MED,
                           "Sending DSAT_DELAYED Response code from"
                           " RMSM_NULL_STATE_TRANSITION");
      }
      else
      {
          dsat_send_result_code( DSAT_NO_CARRIER );
          DATA_IS707_MSG0( MSG_LEGACY_MED,
                           "Setting DSAT_NO_CARRIER Response code from"
                           " RMSM_NULL_STATE_TRANSITION");
      }


      /*---------------------------------------------------------------------
        Deregister all the events with IS707_PKT_IFACE
      ---------------------------------------------------------------------*/

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_UP_EV,
                                  ds707_rmsmi_info.um_iface_up_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_DOWN_EV,
                                  ds707_rmsmi_info.um_iface_down_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                             IFACE_CONFIGURING_EV,
                             ds707_rmsmi_info.um_iface_configuring_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                              IFACE_PHYS_LINK_GONE_EV,
                              ds707_rmsmi_info.um_phys_link_gone_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_UP_EV,
                                  ds707_rmsmi_info.um_phys_link_up_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_FLOW_ENABLED_EV,
                                  ds707_rmsmi_info.flow_enable_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_FLOW_DISABLED_EV,
                                  ds707_rmsmi_info.flow_disable_buf_ptr );

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_ADDR_CHANGED_EV,
                                  ds707_rmsmi_info.ip_addr_changed_buf_ptr );

      /*---------------------------------------------------------------------
        Deregister all the events with the PHYS_LINK
      ---------------------------------------------------------------------*/

      ps_phys_link_event_cback_dereg( PS_IFACE_GET_PHYS_LINK(
                                        ds707_rmsmi_info.
                                        IS707pkt_iface_ptr  ),
                                      PHYS_LINK_FLOW_ENABLED_EV,
                                      ds707_rmsmi_info.
                                        um_phys_link_flow_enable_buf_ptr );
      
      ps_phys_link_event_cback_dereg( PS_IFACE_GET_PHYS_LINK(
                                        ds707_rmsmi_info.
                                        IS707pkt_iface_ptr  ),
                                      PHYS_LINK_FLOW_DISABLED_EV,
                                      ds707_rmsmi_info.
                                        um_phys_link_flow_disable_buf_ptr );

	  ps_flow_event_cback_dereg( PS_IFACE_GET_DEFAULT_FLOW(
                                        ds707_rmsmi_info.
                                        IS707pkt_iface_ptr  ),
                                      FLOW_TX_ENABLED_EV,
                                      ds707_rmsmi_info.
                                        um_flow_tx_enable_buf_ptr );

	  ps_flow_event_cback_dereg( PS_IFACE_GET_DEFAULT_FLOW(
                                        ds707_rmsmi_info.
                                        IS707pkt_iface_ptr  ),
                                      FLOW_TX_DISABLED_EV,
                                      ds707_rmsmi_info.
                                        um_flow_tx_disable_buf_ptr );

      /*---------------------------------------------------------------------
        For network mode, cleanup PPP on RM and deregister callbacks
      ---------------------------------------------------------------------*/
      if( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                   DS_CRM_VAL_NETMODEL_MODE )
      {
        (void) ppp_event_deregister( ds707_rmsmi_info.ppp_rm_event_handle,
                                     PPP_PROTOCOL_LCP,
                                     PPP_DOWN_EV_MASK );
        (void) ppp_event_deregister( ds707_rmsmi_info.ppp_rm_event_handle,
                                     PPP_PROTOCOL_IPCP,
                                     PPP_UP_EV_MASK );
        (void) ppp_event_deregister( ds707_rmsmi_info.ppp_um_event_handle,
          PPP_PROTOCOL_IPCP,
          (ppp_event_mask_e_type) ((int) PPP_DOWN_EV_MASK | 
                                   (int) PPP_UP_EV_MASK | 
                                   (int) PPP_RESYNC_EV_MASK) );

        (void) ppp_event_deregister( ds707_rmsmi_info.ppp_um_event_handle,
                                     PPP_PROTOCOL_LCP,
                                     PPP_DOWN_EV_MASK );
        (void)ppp_abort(PPP_RM_DEV);
        (void)ppp_abort(PPP_UM_SN_DEV);
      }
      /*---------------------------------------------------------------------
        For relay mode, set the PPP mode to NONE for both UM and RM, 
        there isn't anything else to cleanup on PPP
      ---------------------------------------------------------------------*/
      else
      {
        ppp_set_mode( PPP_UM_SN_DEV, PPP_NONE_MODE );
        ppp_set_mode( PPP_RM_DEV, PPP_NONE_MODE );
      }

      ds707_netmdl_reset_resync_callbacks();

      /* We need to update correct sys info. In case where eHRPD call is dormant
	     and DUN call is attempted, system should remain in EPC*/
	  
	    if( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_NON_EPC_SYSTEM )
				   ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
 
#ifdef FEATURE_DS_MOBILE_IP
      /*---------------------------------------------------------------------
        For cases when call is ended before/without PHYS_LINK coming up,
        we need to reset MIP info here, otherwise ds707_iface_hdlr wld
        take care of ressting MIP  config
      ---------------------------------------------------------------------*/
      mip_reset_config();
#endif /* FEATURE_DS_MOBILE_IP */


      /*---------------------------------------------------------------------
        Flush the Route for both Relay and network model calls
      ---------------------------------------------------------------------*/
      (void) route_flush( ds707_rmsmi_info.IS707pkt_iface_ptr ); 

      /*---------------------------------------------------------------------
        Cleanup RLP rx queue and restore original queues/wms on IS707 iface
        for all calls.  Always cleanup local watermarks.
      ---------------------------------------------------------------------*/
      ds707_pkt_setup_rlp( NULL, NULL, NULL);
        
      dsm_empty_queue(&ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A]);
      dsm_empty_queue(&ds707_rmsmi_info.sio_fwd_wmk);

      /*---------------------------------------------------------------------
        Set the resync behaviour back to default setting
      ---------------------------------------------------------------------*/
      ds707_netmdl_set_resync_behaviour (NETMDL_RESYNC_DFLT);

      ps_phys_link_gone_ind( PS_IFACE_GET_PHYS_LINK(
                             &ds707_rmsmi_info.sio_pkt_iface ));
      
      DATA_IS707_MSG0(MSG_LEGACY_MED, "PS_DATA_CALL_END_IND");
      ds3gflowmgr_notify_ps_call_state_per_sub(
         SYS_MODEM_AS_ID_1, DS3GFLOWMGR_PS_CALL_END_EV);
      
      ps_iface_disabled_ind( &ds707_rmsmi_info.sio_pkt_iface );

      /*---------------------------------------------------------------------
        Indicate that we are not in call anymore, deregister siolib
        event handlers
      ---------------------------------------------------------------------*/
      if(ds707_rmsmi_sio_port_allocated != DS3G_SIOLIB_PORTID_NONE)
      {
        ds3g_siolib_set_call_coming_up( FALSE );
        (void) ds3g_siolib_deregister_callback_func( 
                                              &ds707_rmsmi_sio_func_table );

        /*---------------------------------------------------------------------
          Enforce flow enable by passing the override mask to make sure next
          call can come up. Only reason to keep the flow disabed is if we are 
          out of memory.
        ---------------------------------------------------------------------*/
        ds3g_siolib_set_inbound_flow( (DS_FLOW_ALL_MASK & 
                                      ~DS_FLOW_DSM_LEVEL_MASKS),
                                      DS_FLOW_ENABLE );

        ds3g_siolib_release_port ( ds707_rmsmi_sio_port_allocated );
        ds707_rmsmi_sio_port_allocated = DS3G_SIOLIB_PORTID_NONE;
      }

      /*---------------------------------------------------------------------
        Enable flow on the Um interface
      ---------------------------------------------------------------------*/
      if(ds707_rmsmi_info.IS707pkt_iface_ptr != NULL)
      {
        ps_iface_enable_flow ( ds707_rmsmi_info.IS707pkt_iface_ptr,
                               DS_FLOW_707_RMSM_MASK);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED,
                         "Um iface ptr NULL. Cannot enable flow");
      }

      /*---------------------------------------------------------------------
        Cleanup the iface
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "IFACE cleanup for DUN calls");
      ds707_pdncntx_dun_cleanup(ds707_rmsmi_info.IS707pkt_iface_ptr);

      /*---------------------------------------------------------------------
        Unbridge sio & 707 ifaces as we are going NULL.
      ---------------------------------------------------------------------*/
      ps_iface_set_bridge(&ds707_rmsmi_info.sio_pkt_iface, NULL);

      if(ds707_rmsmi_info.IS707pkt_iface_ptr != NULL)
      {
        ps_iface_set_bridge(ds707_rmsmi_info.IS707pkt_iface_ptr, NULL);
       
        /*---------------------------------------------------------------------
          Reset the um interface pointer to NULL 
        ---------------------------------------------------------------------*/
        ds707_rmsmi_info.IS707pkt_iface_ptr = NULL;
      }

     /*---------------------------------------------------------------------
        Reset the force CDMA only flag to FALSE
      ---------------------------------------------------------------------*/
      ds707_rmsmi_info.pkt_info.force_cdma_only = FALSE;

      /*---------------------------------------------------------------------
        Reset the flag that determine if the connect was sent
      ---------------------------------------------------------------------*/
      ds707_rmsmi_info.rmsm_sent_connect = FALSE;

      break;
    }

    case DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    {
      #ifdef TEST_FRAMEWORK
        #error code not present
#else
        ds3gi_start_timer( DS3G_TIMER_DUN_CTRL_ENTL, 1000);
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "DS3G_TIMER_DUN_CTRL_ENTL timer started");
      #endif
      break;
    }
#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE:
    {
#ifdef FEATURE_DS_MOBILE_IP_PERF
      /*---------------------------------------------------------------------
        Start MIP call setup timer
      ---------------------------------------------------------------------*/
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
      mip_perf_delay( mip_perf_info_ptr,
                      PERF_TOTAL_CALL_SETUP_DELAY,
                      PERF_START_TS );
#endif /* FEATURE_DS_MOBILE_IP_PERF */

      /*---------------------------------------------------------------------
        Build Policy and Bring up the Um interface
      ---------------------------------------------------------------------*/

      DATA_IS707_MSG2( MSG_LEGACY_HIGH, "In WAITING_FOR_PHYS_LINK_UP - "
                       "Before Bringup, brought_up: %d, reestablish: %d", 
                        rmsmi_IS707pkt_iface_brought_up,
                        rmsmi_reestablish_um_link);

      bringup_err = ds707_rmsmi_bring_up_iface_by_policy(NULL,
                                                         ds707_rmsmi_info.state);

      DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                       "In WAITING_FOR_PHYS_LINK_UP_STATE - After Bringup, "
                       "brought_up: %d, reestablish: %d, bringup returned: %d", 
                       rmsmi_IS707pkt_iface_brought_up,
                       rmsmi_reestablish_um_link,
                       bringup_err);

      if(bringup_err == DS707_RMSM_BRINGUP_UM_FAIL)
      {
        /* Failed to bring_up_iface_by_policy */
        ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
      }
      else if(bringup_err == DS707_RMSM_BRINGUP_UM_SUCCESS)
      {
        /* Success bring_up_iface_by_policy */
        /* stay in same state - go to end of switch */
      }
      else if(bringup_err == DS707_RMSM_BRINGUP_UM_SUCCESS_CLOSE_IN_PROGRESS)
      {
        /* Success bring_up_iface_by_policy */
        /* stay in same state - go to end of switch */
      }
      else if(bringup_err == DS707_RMSM_BRINGUP_UM_WAITING_ARBITRATION)
      {
        /* Waiting for Arbitration */
        ds707_rmsmi_info.state_before_arbitration = ds707_rmsmi_info.state;
        ds707_rmsmi_transition_state(DS707_RMSM_WAITING_FOR_UM_ARBITRATION);
      }
      else
      {
        /* Unexpected response by bring_up_iface_by_policy */
        ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
      }
      
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        For laptop MIP calls fallback to SIP is handled by RMSM since it 
        executes a different state machine. Tell dssnet to fail call
        if MIP fails and not to fallback to SIP on ite own.
      ---------------------------------------------------------------------*/
      /*Get the iface table entry for IS707 iface and get dssnet4 pointer 
        using that iface table entry*/
      ds707_iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(
                              ds707_rmsmi_info.IS707pkt_iface_ptr);
      dssnet4_sm_cb_ptr = ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr);
      if (dssnet4_sm_cb_ptr != NULL)
      {
        dssnet4_override_qcmip(dssnet4_sm_cb_ptr);
      
      /* set a flag in the dssnet to determine if client is RMSM */
        dssnet4_set_te_client(dssnet4_sm_cb_ptr);
      }

      ps_iface_event_cback_dereg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                  IFACE_PHYS_LINK_UP_EV,
                                  ds707_rmsmi_info.um_phys_link_up_buf_ptr );
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE:
    {   
      /*---------------------------------------------------------------------
        Set netmdl meta state to INIT.
        Set resync state to drop call if options mismacth after a resync.
      ---------------------------------------------------------------------*/
      ds707_netmdl_reg_ppp_init_callbacks();
      ds707_netmdl_set_resync_behaviour(NETMDL_RESYNC_DROP_CALL);      

      /*---------------------------------------------------------------------
        Register cbacks for the IP_ADDR_CHANGED event with psiface.
      ---------------------------------------------------------------------*/
      if(ps_iface_event_cback_reg(ds707_rmsmi_info.IS707pkt_iface_ptr,
                             IFACE_ADDR_CHANGED_EV,
                             ds707_rmsmi_info.ip_addr_changed_buf_ptr ) < 0 )
      {
         DATA_IS707_MSG0( MSG_LEGACY_ERROR, "IP_ADDR_CHAGED ev reg failed" );

         if ( ps_iface_go_null_cmd( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                      &ps_iface_errno, NULL ) == 0 )
         {
           ds707_rmsm_post_event(DS707_RMSM_UM_IFACE_DOWN_EV);
         }
         break;
      }
       
      /*---------------------------------------------------------------------
        Change the sio mode to PKT. 
      ---------------------------------------------------------------------*/
      ds707_rmsmi_setup_sio_watermarks(&ds707_rmsmi_info.sio_fwd_wmk,
                        &ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A]);
      
      /*---------------------------------------------------------------------- 
        Allocate memory for dev_opts
      ----------------------------------------------------------------------*/
      dev_opts = (ppp_dev_opts_type*)
                 modem_mem_alloc(sizeof(ppp_dev_opts_type),
                                 MODEM_MEM_CLIENT_DATA_CRIT);      									 
      if(dev_opts == NULL)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_rmsmi_process_rm_ppp_up_ev():"
                        "memory allocation failed for dev_opts");
        ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        break;   
      } 
      memset((void *)dev_opts, 0, sizeof(ppp_dev_opts_type));
      
      /*---------------------------------------------------------------------
        Copy the IP address from the Um interface to the IPCP remote 
        working values as the IP address to be assigned to the TE2.
        Set will and want to negotiate IP address on the Rm.
      ---------------------------------------------------------------------*/
      ppp_get_default_opts (dev_opts);

      dev_opts->rx_f_ptr       = ds707_rmsmi_sio_rx_ppp_data;
      dev_opts->tx_f_ptr       = ds707_rmsmi_sio_tx_ppp_data;
      dev_opts->bridge_dev     = PPP_UM_SN_DEV;
      dev_opts->ipcp_info.rx_iface_ptr = &ds707_rmsmi_info.sio_pkt_iface;
      dev_opts->lcp_info.mode  = PPP_CP_MODE_PASSIVE;
      dev_opts->ipcp_info.mode = PPP_CP_MODE_ACTIVE;
      dev_opts->hdlc_mode      = HDLC_SW;

      /*---------------------------------------------------------------------
        If TE2 does not respond for a ceratin time period, terminate the 
        call. Otherwise MS will keep holding the call resources for ever 
        since PPP is in passive mode.
      ---------------------------------------------------------------------*/
      dev_opts->setup_timeout  = RM_PPP_SETUP_TIME;
      
      (void) ppp_get_bridge_dev_prot_opts( PPP_UM_SN_DEV,
                                           dev_opts);

      /*---------------------------------------------------------------------
        Assign the IP address obtained on the Um to the peer on the Rm
        interface and give a private address to the local side of Rm
        interface.Assign the DNS addresses, obtained over Um interface, to 
        the Rm interface.
      ---------------------------------------------------------------------*/
      /* No change necessary for Ipv6 since RMSM will handle only IPv4. 
         RmNet will handle the IPv6 changes. */
      ip_addr.type = IPV4_ADDR;
      ps_iface_get_peer_addr(ds707_rmsmi_info.IS707pkt_iface_ptr, 
                             &ip_addr);
      ppp_set_v4_server_opts( dev_opts,
       ip_addr.addr.v4.ps_s_addr,
       PS_IFACE_IP_V4_ADDR(ds707_rmsmi_info.IS707pkt_iface_ptr),
       ds707_rmsmi_info.IS707pkt_iface_ptr->v4_net_info.primary_dns.ps_s_addr,
       ds707_rmsmi_info.IS707pkt_iface_ptr->v4_net_info.secondary_dns.ps_s_addr,
       /*--------------------------------------------------------------------
          For WINS support - Passing 0 for primary and secondary nbns address
          as 1x doesn't support WINS.
        -------------------------------------------------------------------*/
        0,                     
        0
      );

      /*---------------------------------------------------------------------
        If IPV6 feature and V6 NV Item enabled and MIP4 call then disable 
        auth.
        In this case PPP auth is done on the UM side. But we dont need it on 
        RM side.
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
      if (ds3g_is_ipv6_enabled())  
      {

           dev_opts->lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
           dev_opts->lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
           dev_opts->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
           dev_opts->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
          
      }
#endif /*FEATURE_DATA_PS_IPV6*/

      /*---------------------------------------------------------------------
        Register for desired events with PPP on RM, don't bother with 
        resync on Rm, this should be handled by netmodel
      ---------------------------------------------------------------------*/
      if( NULL == ds707_rmsmi_info.ppp_rm_event_handle )
      {
        modem_mem_free(dev_opts, MODEM_MEM_CLIENT_DATA_CRIT);
        ERR_FATAL( "NULL Rm PPP event handle", 0, 0, 0 );
      }
      else
      {
        (void) ppp_event_register( ds707_rmsmi_info.ppp_rm_event_handle,
                                   PPP_PROTOCOL_LCP,
                                   PPP_DOWN_EV_MASK );
        (void) ppp_event_register( ds707_rmsmi_info.ppp_rm_event_handle,
                                   PPP_PROTOCOL_IPCP,
                                   PPP_UP_EV_MASK );
      }

      if( NULL == ds707_rmsmi_info.ppp_um_event_handle )
      {
        modem_mem_free(dev_opts, MODEM_MEM_CLIENT_DATA_CRIT);
        ERR_FATAL( "NULL Um PPP event handle", 0, 0, 0 );
      }
      else
      {
        (void) ppp_event_register( ds707_rmsmi_info.ppp_um_event_handle,
                                   PPP_PROTOCOL_IPCP,
                                   PPP_DOWN_EV_MASK );
        (void) ppp_event_register( ds707_rmsmi_info.ppp_um_event_handle,
                                   PPP_PROTOCOL_LCP,
                                   PPP_DOWN_EV_MASK );
      }

      /*---------------------------------------------------------------------
        Change the sio mode to PKT and Assert DCD and return CONNECT to 
        start PPP on Rm
      ---------------------------------------------------------------------*/
      (void) ds3g_siolib_change_mode( SIO_DS_PKT_MODE,
                          &ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A],
                          &ds707_rmsmi_info.sio_fwd_wmk,
                          NULL );
      (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON );

      connect_rate.call_type = DSAT_CALL_TYPE_1X;
      sys_mode = ds707_pkt_mgr_get_current_sys_mode();

      /* Check the sys mode acquired and set up the following data rates */
      if (SYS_SYS_MODE_CDMA == sys_mode )
      {
        bearer_tech_query.bearer_tech = DS707_EXTIF_1X;
      }
#ifdef FEATURE_HDR
      else if ( SYS_SYS_MODE_HDR == sys_mode )
      {
        sys_active_prot_e_type  prot = 
                                ds707_sys_chg_hdlr_get_active_hdr_protocol();
        if (SYS_ACTIVE_PROT_HDR_REL0 == prot )
        {
          bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REV0;
        }
        else if ( SYS_ACTIVE_PROT_HDR_RELA == prot )
        {
          bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REVA;
        }
        else if ( SYS_ACTIVE_PROT_HDR_RELB == prot )
        {
          bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REVB;
        }
        else
        {
          DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid HDR PROT %d", prot);
        }
      }
#endif /* FEATURE_HDR */
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR, "Invalid Operating mode %d",
                         sys_mode);
      }

      (void)ds707_extif_get_bearer_tech_max_rate( &bearer_tech_query );
      connect_rate.rate.cdma_call.dl_rate = 
                                   bearer_tech_query.max_rx_bearer_tech_rate;
      connect_rate.rate.cdma_call.ul_rate = 
                                   bearer_tech_query.max_tx_bearer_tech_rate;

      DATA_IS707_MSG3( MSG_LEGACY_MED,
                       " Updating connect UL rate as %d DL rate %d for "
                       "call type %d",
                       connect_rate.rate.cdma_call.ul_rate,
                       connect_rate.rate.cdma_call.dl_rate,
                       connect_rate.call_type);
      /* currently not updating protocol filed as it is of no use. */
      dsat_report_rate_on_connect(connect_rate);
      dsat_send_result_code( DSAT_CONNECT );

      if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {
        ds707_configure_jcdma_params_for_ppp(dev_opts);
      }
      
      if( ppp_start( PPP_RM_DEV, dev_opts ) < 0 )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "PPP RM config failed, posting RM_IFACE_DOWN" );
        ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
      }
      modem_mem_free(dev_opts, MODEM_MEM_CLIENT_DATA_CRIT);
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_RM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Stop PPP on Rm 
      ---------------------------------------------------------------------*/
      (void) ds707_netmdl_terminate_rm_ppp();
      break;
    }

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        If UM is already DOwn post the appropriate event. It would return
        EWOULDBLOCK otherwise. Assumption is that the call to tear 
        down never fails.              
      ---------------------------------------------------------------------*/
      if(ps_iface_go_null_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                               &ps_iface_errno,
                               NULL) == 0)
      {
        ds707_rmsm_post_event(DS707_RMSM_UM_IFACE_DOWN_EV);
      }

      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_RESYNCING_UM_MIP_STATE:
    {
      //since PPP is already set to PPP_INTERNAL_MODE, do nothing.
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP
    case DS707_RMSM_UM_RM_UP_MIP_STATE:
    {
      /*---------------------------------------------------------------------
        Put SIO iface in Routeable state and also put phys link state to UP
        so that PPP can cleanup gracefully during call termination.
      ---------------------------------------------------------------------*/
      ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_pkt_iface );
      ps_iface_routeable_ind( &ds707_rmsmi_info.sio_pkt_iface );

      /*---------------------------------------------------------------------
        Rm is up with same options as Um: set Um and Rm to pseudo network and
        put the PPP meta state machine in the up state.
      ---------------------------------------------------------------------*/
      ppp_set_mode( PPP_RM_DEV, PPP_INTERNAL_MODE );
      ppp_set_mode( PPP_UM_SN_DEV, PPP_INTERNAL_MODE );

      DATA_IS707_MSG2( MSG_LEGACY_MED,
                       "Setting up IP bridge Um:0x%x <-> Rm:0x%x",
                       &ds707_rmsmi_info.sio_pkt_iface,
                       ds707_rmsmi_info.IS707pkt_iface_ptr);
      ps_iface_set_bridge(&ds707_rmsmi_info.sio_pkt_iface,
                          ds707_rmsmi_info.IS707pkt_iface_ptr);
      ps_iface_set_bridge(ds707_rmsmi_info.IS707pkt_iface_ptr,
                          &ds707_rmsmi_info.sio_pkt_iface);
      ds707_netmdl_reg_ppp_up_callbacks();

#ifdef FEATURE_CALL_THROTTLE
        /*-------------------------------------------------------------------
          MIP call up successfully. Post SUCCESS event to DCTM.
        -------------------------------------------------------------------*/
        dctm_post_mip_call_status(MIP_SUCCESS_EV,MIP_FAIL_REASON_NONE);
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DS_MOBILE_IP_PERF
      mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
      if ( qw_cmp( mip_perf_info_ptr->ts_start,
                 ts_null ) != 0)
      {
        /*-------------------------------------------------------------------
          Notify M.IP call performance that M.IP call setup is complete for
          network model calls.
        -------------------------------------------------------------------*/
        mip_perf_delay( mip_perf_info_ptr,
                        PERF_TOTAL_CALL_SETUP_DELAY,
                        PERF_END_TS );
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

      /*---------------------------------------------------------------------
        Register theTerm-Req callback that causes the interface it is called
        on to get torn down for both interfaces and both LCP and IPCP.
        NOTE: this MUST be done AFTER the meta state has been set to UP-this
        guarantees that these callbacks are not over written by setting the
        meta state.
      ---------------------------------------------------------------------*/

      netmdl_snoop_reg( PPP_RM_DEV,
                        PPP_LCP_PROTOCOL,
                        TERM_REQ,
                        ds707_rmsmi_netmdl_term_req_cb );

      netmdl_snoop_reg( PPP_UM_SN_DEV,
                        PPP_LCP_PROTOCOL,
                        TERM_REQ,
                        ds707_rmsmi_netmdl_term_req_cb );

      netmdl_snoop_reg( PPP_RM_DEV,
                        PPP_IPCP_PROTOCOL,
                        TERM_REQ,
                        ds707_rmsmi_netmdl_term_req_cb );

      netmdl_snoop_reg( PPP_UM_SN_DEV,
                        PPP_IPCP_PROTOCOL,
                        TERM_REQ,
                        ds707_rmsmi_netmdl_term_req_cb );

      /*---------------------------------------------------------------------
        Register the ds707_rmsmi_rm_resync_cb. This would ensure that the
        call is released on detecting a PPP resync on the Rm link
      ---------------------------------------------------------------------*/
      netmdl_snoop_reg( PPP_RM_DEV,
                        PPP_LCP_PROTOCOL,
                        CONFIG_REQ,
                        ds707_rmsmi_rm_resync_cb );

      netmdl_snoop_reg( PPP_RM_DEV,
                        PPP_IPCP_PROTOCOL,
                        CONFIG_REQ,
                        ds707_rmsmi_rm_resync_cb );
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP */

    case DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE:
    case DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE:
    {

      /*---------------------------------------------------------------------
        Ask IS707_PKT_CTRL to initialize RLP.  For Relay mode calls, 
        DS707_RMSM provides config info to IS707_PKT_CTRL which is used to
        initialize RLP. For SIP Network Model calls IS707_PKT_CTRL uses its
        own configuration (1X generic pkt config) for RLP.
      ---------------------------------------------------------------------*/
      if( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                   DS_CRM_VAL_NETMODEL_MODE )
      {
        ds707_pkt_setup_rlp( NULL, 
                             NULL,
                             NULL);

#ifdef FEATURE_HDR
        /*-------------------------------------------------------------------
          Set the limit on the PS PPP Rx queue size at which to flow control
          the FL.
        -------------------------------------------------------------------*/
        *(get_hdrds_ps_q_lim_ptr()) = HDRDS_NETMDL_PS_Q_LIM;
#endif /* FEATURE_HDR */

      }
      else
      {
        if ( ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                       IFACE_PHYS_LINK_UP_EV,
                                       ds707_rmsmi_info.um_phys_link_up_buf_ptr
                                     ) < 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "PHYS_LINK_UP_EV reg failed" );
          ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );
          break;
        }
        /*------------------------------------------------------------------
           For relay calls SIO Rx wm is for RLP transmissions.
        -----------------------------------------------------------------*/
        ds707_pkt_setup_rlp( ds707_rmsmi_info.sio_rev_wmk_list,
                             ds707_rmsmi_sio_tx_rlp_data,
                             ds707_rmsmi_info.rlp_fwd_wmk_list);
      }

      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                   DS_CRM_VAL_NETMODEL_MODE )
      {

        /*-----------------------------------------------------------------
           Added the mechanism to pass force_cdma_only as a user data
           parameter while bringing up the phys link    
        -------------------------------------------------------------------*/
        /*---------------------------------------------------------------------
          Build Policy and Bring up the Um interface
        ---------------------------------------------------------------------*/

        DATA_IS707_MSG2(MSG_LEGACY_HIGH, "In UM_UP_SIP STATE - "
                        "Before Bringup, brought_up: %d, reestablish: %d",
                        rmsmi_IS707pkt_iface_brought_up,
                        rmsmi_reestablish_um_link);

        bringup_err = ds707_rmsmi_bring_up_iface_by_policy(&ds707_rmsmi_info.pkt_info,
                                                           ds707_rmsmi_info.state);

        DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                         "In UM_UP_SIP STATE - After Bringup, brought_up: %d, "
                         "reestablish: %d, bringup returned: %d", 
                         rmsmi_IS707pkt_iface_brought_up,
                         rmsmi_reestablish_um_link,
                         bringup_err);

        if(bringup_err == DS707_RMSM_BRINGUP_UM_FAIL)
        {
          /* Failed to bring_up_iface_by_policy */
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        }
        else if(bringup_err == DS707_RMSM_BRINGUP_UM_SUCCESS)
        {
          /* Success bring_up_iface_by_policy */
          /* stay in same state - go to end of switch */
        }
        else if(bringup_err == DS707_RMSM_BRINGUP_UM_SUCCESS_CLOSE_IN_PROGRESS)
        {
          /* Success bring_up_iface_by_policy */
          /* stay in same state - go to end of switch */
        }
        else if(bringup_err == DS707_RMSM_BRINGUP_UM_WAITING_ARBITRATION)
        {
          /* Waiting for arbitration */
          ds707_rmsmi_info.state_before_arbitration = ds707_rmsmi_info.state;
          ds707_rmsmi_transition_state(DS707_RMSM_WAITING_FOR_UM_ARBITRATION);
        }
        else
        {
          /* Unexpected response by bring_up_iface_by_policy */
          ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
        }
        
      }
      else /* not NET, so RELAY MODEL CALL */
      {

        ds707_rmsmi_info.pkt_info.iface_ptr = 
                                 ds707_rmsmi_info.IS707pkt_iface_ptr;
  
        if (ds707_data_session_get_oldest_active_iface_ptr() == NULL) 
        {
          ds707_data_session_set_oldest_active_iface_ptr(ds707_rmsmi_info.IS707pkt_iface_ptr);
        }

        /*---------------------------------------------------------------------
          Register the phys link gone cback.
        ---------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_MED, "RMSM setting abort cback");
        (void) ps_iface_event_cback_reg(ds707_rmsmi_info.IS707pkt_iface_ptr,
                                        IFACE_PHYS_LINK_GONE_EV,
                                        ds707_rmsmi_info.um_phys_link_gone_buf_ptr);

        /*-------------------------------------------------------------------
          We are bringing up the phys link for Relay Model
          call here. Since PPP connection is between TE2-IWF, all we need
          to do is bring up the traffic channel.
        -------------------------------------------------------------------*/

        (void) ps_flow_bind_phys_link (
              PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr),
              PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr) );

        ps_flow_activate_ind(
           PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr),
           PS_EIC_NOT_SPECIFIED);

        /* Reset bring up flag */
        rmsmi_IS707pkt_iface_brought_up = FALSE;
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "RMSM Resetting iface brought up flag");

        if( ( ps_phys_link_up_cmd( 
                 PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr),
                 &ps_iface_errno,
                 NULL ) < 0 ) && 
            ( ps_iface_errno != DS_EWOULDBLOCK ) )
        {
          /*-----------------------------------------------------------------
            Traffic channel could not be brought up, bring down iface and
            terminate the call
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "Phys link bring up failed: posting flow "
                           "go null and phys link gone ind");
          ps_flow_go_null_ind(
              PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr),
              PS_EIC_NOT_SPECIFIED);

          ps_phys_link_gone_ind( 
               PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr ));
        }
        else
        {
          /* Bring up traffic channel and set bring up flag. 
           * When relay model call is up, any other call
           * shall be rejected in ds707_rt_acl()
           */
          rmsmi_IS707pkt_iface_brought_up = TRUE;
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "RMSM Setting iface brought up flag to TRUE");

        }
      }

      break;
    }

    case DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE:
    {
      ppp_set_mode( PPP_UM_SN_DEV, PPP_PSEUDO_NET_MODE );
      ppp_set_mode( PPP_RM_DEV, PPP_PSEUDO_NET_MODE );
      ds707_netmdl_reg_ppp_init_callbacks();
      break;
    }

    case DS707_RMSM_E2E_PPP_UP_SIP_STATE:
    {
      ds707_netmdl_reg_ppp_up_callbacks();
      /*---------------------------------------------------------------------
        Check if ready for 1x tuneaway and update MMODE
      ---------------------------------------------------------------------*/
      if(ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
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
      }

#ifdef FEATURE_CALL_THROTTLE
      dctm_post_ppp_call_status( PPP_SUCCESS_EV, DSDCTM_PPP_PROTOCOL_IPCP, 
                                          PPP_FAIL_REASON_NONE);
#endif /* FEATURE_CALL_THROTTLE */
      break;
    }
    case DS707_RMSM_UM_RM_UP_RELAY_STATE:
    {
      /*---------------------------------------------------------------------
        Check if ready for 1x tuneaway and update MMODE
      ---------------------------------------------------------------------*/
      if(ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)
      {
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
      }
      /*-------------------------------------------------------------------
        For Relay calls, the call is pretty much up since PPP is handled in 
        TE2 only.  Change the iface and PPP states appropriately.  

       Modifed to make the Um iface routeable for Relay model calls to 
       handle DCM scenario of tearing down DO session upon Um iface down
      -------------------------------------------------------------------*/
      ps_iface_up_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_pkt_iface );
      ps_iface_routeable_ind( &ds707_rmsmi_info.sio_pkt_iface );

      ppp_set_mode( PPP_UM_SN_DEV, PPP_RELAY_MODE );
      ppp_set_mode( PPP_RM_DEV, PPP_RELAY_MODE );
#ifdef FEATURE_CALL_THROTTLE
      dctm_post_ppp_call_status( PPP_SUCCESS_EV, DSDCTM_PPP_PROTOCOL_IPCP, 
                                          PPP_FAIL_REASON_NONE);
#endif /* FEATURE_CALL_THROTTLE */
      break;
    }

    case DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE:
    {
      /*-------------------------------------------------------------------
        We just wait in this state for phys link to come down.
        Do nothing inside the state 
      -------------------------------------------------------------------*/
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_DOWN_STATE:
    {
      break;
    }

    case DS707_RMSM_RESYNCING_UM_SIP_STATE:
    {
     /*-------------------------------------------------------------------
       Register for callbacks to be notified once PPP is UP. 
     -------------------------------------------------------------------*/
      ds707_netmdl_reg_ppp_resync_callbacks();
      break;
    }

    case DS707_RMSM_WAITING_FOR_UM_ARBITRATION:
    {
      /* No special handling */
      break;
    }
    default:
    {
      /* Invalid state */
      DS_FORCE_ASSERT();
    }
  } /* switch(new state) */
} /* ds707_rmsmi_transition_state() */


/*===========================================================================
  FUNCTION DS707_RMSMI_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    appropriate event to ds707_rmsm which initiates the call termination.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_call_abort_cb
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "Call aborted, posting DS707_RMSM_RM_IFACE_DOWN_EV");
  ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
} /* ds707_rmsmi_call_abort_cb() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_ESC_SEQ_CB

  DESCRIPTION
    This function is called when DTR goes low and AT&D = 1.  This should do
    the same action as the dtr_sig_cb.

  PARAMETERS
    None

  RETURN VALUE
    Always return DSAT_ONLINE_DATA, which implies 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsat_mode_enum_type ds707_rmsmi_process_esc_seq_cb(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_rmsmi_process_dtr_sig_cb(FALSE);
  return(DSAT_ONLINE_DATA);
} /* ds707_rmsmi_process_esc_seq_cb() */


/*===========================================================================
  FUNCTION DS707_RMSMI_PROCESS_DTR_SIG_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    appropriate event is posted to ds707_rmsm which initiates the call 
    termination.
    DTR assert is ignored for now.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_process_dtr_sig_cb
(
  boolean is_asserted
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Currently only DTR deassert is handled, post appropriate event
  -------------------------------------------------------------------------*/
  if ( is_asserted == FALSE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,  
                     "DTR dropped, posting DS707_RMSM_RM_IFACE_DOWN_EV");
    ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
  }
} /* ds707_rmsmi_process_dtr_sig_cb() */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION   DS707_RMSMI_NETMDL_TERM_REQ_CB()

DESCRIPTION
  This is the Term-Req callback that is used to tear down whichever link the
  Term-Req is received on.  It places the recieving interface into internal
  mode.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item. For this information to be useful it is expected that
  the FF03 has been removed from the front and that any leading 00 s have
  been removed from the protocol ID.

RETURN VALUE
  SNOOP_CB_PROCESS_PKT: this callback requires that the packets it
                       processes be alwyas sent up the stack for further
                       processing.

SIDE EFFECTS
  This will cause the receiving link to be torn down.
===========================================================================*/
/* ARGSUSED */
LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_netmdl_term_req_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Ptr to ref to PS meta info    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((device == PPP_RM_DEV) || (device == PPP_UM_SN_DEV));
  ASSERT((protocol == PPP_IPCP_PROTOCOL) || (protocol == PPP_LCP_PROTOCOL));
  DATA_MDM_ARG_NOT_CONST(meta_info_ref_ptr);
  DATA_MDM_ARG_NOT_CONST(item_head_ptr);

  ppp_set_mode(device, PPP_INTERNAL_MODE);

  return SNOOP_CB_PROCESS_PKT;

} /* ds707_rmsmi_netmdl_term_req_cb() */
#endif /* FEATURE_DS_MOBILE_IP */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION   DS707_RMSMI_RM_RESYNC_CB()

DESCRIPTION
  This callback function is called when an LCP/IPCP C-Req is received on the
  Rm link i.e. when a PPP resync is detected on the Rm link once the call
  has been established. Drop the packet in such event.

DEPENDENCIES
  This callback should be registered only after PPP is up on both Um and
  Rm links

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL snoop_cb_ret_val_enum_type ds707_rmsmi_rm_resync_cb
(
  ppp_dev_enum_type device,            /* the arrival interface            */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr,       /* item containing data             */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Ptr to ref to PS meta info     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT(device == PPP_RM_DEV);
  ASSERT((protocol == PPP_IPCP_PROTOCOL) || (protocol == PPP_LCP_PROTOCOL));
  DATA_MDM_ARG_NOT_CONST(meta_info_ref_ptr);
  DATA_MDM_ARG_NOT_CONST(item_head_ptr);

  /*-------------------------------------------------------------------------
    Drop the dsm_item
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_head_ptr);

  /*-------------------------------------------------------------------------
    Close PPP on RM.
  -------------------------------------------------------------------------*/
  if(device == PPP_RM_DEV )
  {
    (void) ds707_netmdl_terminate_rm_ppp();
  }

  return SNOOP_CB_IGNORE_PKT;

} /* ds707_rmsmi_rm_resync_cb() */
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
FUNCTION   DS707_RMSM_PKT_BRING_UP_CMD()

DESCRIPTION 
  Function that processes DS_707_RMSM_BRING_UP_CMD. This is the cmd handler 
  for bringing up 707 iface during a Relay or Network Model call.  
  Since a special handling is required for bringing up Relay or Netmodel 
  SIP call, RMSM provides its own cmd handler rather than using the 
  one provided by IS707 controller
 
DEPENDENCIES
  None

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 
   
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_pkt_bring_up_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  int16                     ps_iface_errno;
  ps_ip_addr_type           def_addr;
  ps_ip_addr_type           zero_addr;
  ds707_mip_profile_type   *ds_1x_runtime_profile_ptr = NULL;
  ps_iface_type *this_iface_ptr = NULL;
  ps_flow_type  *default_flow_ptr = NULL;
  void *user_data = NULL;
  ps_iface_net_down_reason_type end_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
  ds707_data_session_profile_info_type *data_sess_profile_ptr = NULL;
  ds707_pdn_context_cb_type     *pdncntx_cb_ptr        = NULL;
  ds707_bring_up_cmd_info_type  *bring_up_cmd_info_ptr = NULL;
  ps_iface_down_mh_event_info_type      iface_down_ind_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(ds707_rmsmi_info.state != DS707_RMSM_NULL_STATE);

  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  bring_up_cmd_info_ptr 
                = (ds707_bring_up_cmd_info_type *)(cmd_ptr->cmd_payload_ptr);
  this_iface_ptr = bring_up_cmd_info_ptr->iface_ptr;
  if(bring_up_cmd_info_ptr->is_user_data_set)
  {
    user_data = (void *)(&(bring_up_cmd_info_ptr->user_data.pkt_call_info));
  }
 
  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "ds707_rmsm_pkt_bring_up_cmd: processing "
                   "DS_CMD_707_RMSM_BRING_UP for iface 0x%x:%d", 
                   this_iface_ptr->name,
                   this_iface_ptr->instance);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  default_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr);
  if ((NULL == default_flow_ptr) || 
      (NULL == PS_FLOW_GET_PHYS_LINK(default_flow_ptr)))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Either flow or physlink is null");
    ps_iface_down_ind_ex(this_iface_ptr, 
                         end_reason);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*--------------------------------------------------------------- 
    Reject call based on the current mitigation level.
  ---------------------------------------------------------------*/  
  memset(&iface_down_ind_info, 0, sizeof(iface_down_ind_info));

  ds707_pkt_mgr_get_bearer_tech_info_ex( &iface_down_ind_info.bearer_tech);

  if (FALSE == ds707_tm_mgr_is_call_allowed(
                            this_iface_ptr->iface_private.apn_type))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Thermal mitigation in progress."
                                     " call not allowed");

    iface_down_ind_info.down_reason = PS_NET_DOWN_REASON_THERMAL_MITIGATION; 
    
    ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr),
                        PS_EIC_NOT_SPECIFIED);
    ps_iface_down_ind(this_iface_ptr,&iface_down_ind_info);
    return;
  }

  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                             this_iface_ptr);
  ds707_pdncntx_get_profile(pdncntx_cb_ptr, &data_sess_profile_ptr);

  if(!ds707_is_bringup_allowed(&end_reason, data_sess_profile_ptr))
  {
    iface_down_ind_info.down_reason = end_reason;  
    ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr),
                        PS_EIC_NOT_SPECIFIED);
    ps_iface_down_ind(this_iface_ptr, &iface_down_ind_info);
    return; 
  }

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  /*-------------------------------------------------------------------------
    We are bringing up the 707 pkt iface for Relay or Network model SIP
    call here. Since PPP connection is between TE2-IWF, all we need
    to do is bring up the traffic channel.
  -------------------------------------------------------------------------*/
  if ( ds707_rmsmi_info.IS707pkt_iface_ptr != this_iface_ptr )
  {
    if ( ds707_rmsmi_info.IS707pkt_iface_ptr )
    {
      DATA_IS707_MSG2( MSG_LEGACY_MED,
                       "Saved iface 0x%x:%d is not matched with bringing up iface", 
                       (ds707_rmsmi_info.IS707pkt_iface_ptr)->name,
                       (ds707_rmsmi_info.IS707pkt_iface_ptr)->instance);
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "Saved iface Ptr is NULL");
    }

    ds707_rmsmi_info.IS707pkt_iface_ptr = this_iface_ptr;
    DATA_IS707_MSG2( MSG_LEGACY_MED, "Bringing up iface 0x%x:%d", 
                     this_iface_ptr->name, 
                     this_iface_ptr->instance);

  }
  else
  {
    DATA_IS707_MSG2( MSG_LEGACY_MED,
                     "Saved iface 0x%x:%d is matched with bringing up iface", 
                     (ds707_rmsmi_info.IS707pkt_iface_ptr)->name,
                     (ds707_rmsmi_info.IS707pkt_iface_ptr)->instance);
  }

  ps_flow_activate_ind(
     PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr),
     PS_EIC_NOT_SPECIFIED);

  if( ( ps_iface_phys_link_up_cmd( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                   &ps_iface_errno,
                                   user_data
                                 ) < 0 ) && 
      ( ps_iface_errno != DS_EWOULDBLOCK ) )
  {  
    /*---------------------------------------------------------------------
      Traffic channel could not be brought up, bring down iface and 
      terminate the call
    ---------------------------------------------------------------------*/
    ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                             PS_NET_DOWN_REASON_NOT_SPECIFIED );
  }
  
    
  if( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                 DS_CRM_VAL_NETMODEL_MODE )
  {
    /* Add a default route via this interface */

    /* No change necessary for Ipv6 since RMSM will handle only IPv4. 
       RmNet will handle the IPv6 changes. */
    def_addr.type            = IPV4_ADDR;
    def_addr.addr.v4.ps_s_addr  = ROUTE_DEFAULT_DST;

    /* No change necessary for Ipv6 since RMSM will handle only IPv4. 
       RmNet will handle the IPv6 changes. */
    zero_addr.type           = IPV4_ADDR;
    zero_addr.addr.v4.ps_s_addr = 0x0;

    if ( route_add(ROUTE_DEFAULT,
                   def_addr,
                   zero_addr,
                   zero_addr,
                   ds707_rmsmi_info.IS707pkt_iface_ptr,
                   1,
                   NULL)
         == -1 )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Default route addition failed");
    }
  }

} /* ds707_rmsm_pkt_bring_up_cmd */


/*===========================================================================
FUNCTION   DS707_RMSM_GRACEFUL_PPP_TEAR_DOWN_CMD()

DESCRIPTION 
 This callback function is the cmd handler for tearing down 707 iface
 in Low Power Mode. When the tear down is due to inactivity, we cant to handle
 the case differently because we want graceful close. In other cases, the
 ds707_rmsm_pkt_tear_down_cmd() is called.

DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr: iface pointer
  user_data: user data pointer
 
RETURN VALUE
  -1 for failure, 0 for success

SIDE EFFECTS
  None
===========================================================================*/
int ds707_rmsm_graceful_ppp_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void *user_data
)
{
  int ret_val = 0;
  ps_phys_link_type *phys_link_ptr = NULL;

  DATA_MDM_ARG_NOT_CONST(user_data);

  ASSERT(this_iface_ptr != NULL);

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,  
                   "iface 0x%x:%d graceful ppp tear down cmd", 
                   this_iface_ptr->name,
                   this_iface_ptr->instance);

  phys_link_ptr = PS_IFACE_GET_PHYS_LINK(this_iface_ptr);

  if((ds707_rmsmi_info.state == DS707_RMSM_NULL_STATE) || 
     (NULL == phys_link_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No DUN call active");
    return 0;
  }

  if(ds707_extif_is_ppp_up()== TRUE && 
     ds707_rmsmi_info.state == DS707_RMSM_E2E_PPP_UP_SIP_STATE)
  {
    if ((DS_FLOW_CTRL_IS_BIT_SET(
         ps_phys_link_get_flow_mask(phys_link_ptr),
         DS_FLOW_NO_SERVICE_MASK)) != TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "PPP(UM) is not down, calling ppp_stop on UM device");
      (void)ppp_stop(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Calling PPP abort on UM device");
      (void)ppp_abort(PPP_UM_SN_DEV);
    }
    ret_val = 0;
  }
  else
  {
    ret_val = ds707_rmsm_pkt_tear_down_cmd(this_iface_ptr, user_data);
  }

  return ret_val;
  
}/* ds707_rmsm_graceful_ppp_teardown_cmd */

/*===========================================================================
FUNCTION   DS707_RMSM_PKT_TEAR_DOWN_CMD()

DESCRIPTION
  This callback function is the cmd handler for tearing down 707 iface
  during a Relay or Network Model call.  Since a special handling is
  required for tearing down Relay or Netmodel SIP call, RMSM provides its
  own cmd handler rather than using the one provided by IS707 controller

DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr: iface pointer
  user_data: user data pointer
 
RETURN VALUE
  -1 for failure, 0 for success

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
int ds707_rmsm_pkt_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void *user_data
)
{
 int                  ret_val = 0;
 int16                err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(user_data);

  ASSERT(this_iface_ptr != NULL);

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "ds707_rmsm_pkt_tear_down_cmd: iface 0x%x:%d tear down cmd", 
                   this_iface_ptr->name,
                   this_iface_ptr->instance);

  if(ds707_rmsmi_info.state == DS707_RMSM_NULL_STATE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No DUN call active");
    return 0;
  }
  
  ASSERT( this_iface_ptr == ds707_rmsmi_info.IS707pkt_iface_ptr);


    ps_flow_go_null_ind(
              PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr),
              PS_EIC_NOT_SPECIFIED);

    /*-------------------------------------------------------------------------
      We are tearing down the 707 pkt iface for Relay or Network model SIP
      call here. Since PPP connection is between TE2-IWF, all we need
      to do is bring down the traffic channel.
    -------------------------------------------------------------------------*/

    ret_val = ps_phys_link_go_null_cmd (
              PS_IFACE_GET_PHYS_LINK(this_iface_ptr),
              &err_num, NULL);
    if(ret_val == 0)
    {
      /*---------------------------------------------------------------------
        Phys link shared and still in use on another iface. No need to wait
        for phys_link_down - Send a ps_iface_down_ind.
      ---------------------------------------------------------------------*/
      ps_iface_down_1x_v4_ind( this_iface_ptr,
                               PS_NET_DOWN_REASON_NOT_SPECIFIED );
    }

    DATA_IS707_MSG3( MSG_LEGACY_HIGH,  
                     "iface 0x%x:%d tear down cmd, returning: %d", 
                     this_iface_ptr->name,
                     this_iface_ptr->instance,
                     ret_val);
  

  return ret_val;

} /* ds707_rmsm_pkt_tear_down_cmd */

/*===========================================================================
FUNCTION   DS707_RMSM_TEAR_DOWN_CMD()

DESCRIPTION 
  Function that processes DS_707_RMSM_TEAR_DOWN_CMD. The lpm_mode is used to 
  determine whether Low Power mode is enabled. If it is enabled we
  need to check whether the tear_down is because of inactivity or normal
  teardown and take action accordingly. This is done by calling the function
  ds707_rmsm_graceful_ppp_tear_down_cmd(). In all other cases the normal
  ds707_rmsm_pkt_tear_down_cmd() is called.
 
DEPENDENCIES
  None

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 
   
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_tear_down_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ps_iface_type                 *this_iface_ptr         = NULL;
  void                          *client_data_ptr        = NULL;
  ds707_tear_down_cmd_info_type *tear_down_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  tear_down_cmd_info_ptr 
                = (ds707_tear_down_cmd_info_type *)(cmd_ptr->cmd_payload_ptr);

  this_iface_ptr = tear_down_cmd_info_ptr->iface_ptr;
  if(tear_down_cmd_info_ptr->is_user_data_set)
  {
    client_data_ptr
            = (void *)(&(tear_down_cmd_info_ptr->user_data.tear_down_info));
  }


  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
               "ds707_rmsm_tear_down_cmd: Processing DS_CMD_707_RMSM_TEAR_DOWN"
               "for iface 0x%x:%d",
               this_iface_ptr->name,
               this_iface_ptr->instance);

  if (tear_down_cmd_info_ptr->lpm_mode)
  {
    (void)ds707_rmsm_graceful_ppp_tear_down_cmd(this_iface_ptr, client_data_ptr);
  }
  else
  {
    (void)ds707_rmsm_pkt_tear_down_cmd(this_iface_ptr, client_data_ptr);
  }
  
}

/*===========================================================================
FUNCTION   DS707_RMSMI_IS_MIP_CALL()

DESCRIPTION 
  Function to check if the call is a MIP call
 
DEPENDENCIES
  None

PARAMETERS
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_rmsmi_is_mip_call
(
  void
)
{
#ifdef FEATURE_DS_MOBILE_IP
  ds707_rmsmi_state_type  rmsm_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rmsm_state = ds707_rmsm_get_state();
  if ((DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE == rmsm_state) 
      || (DS707_RMSM_WAITING_FOR_RM_UP_MIP_STATE == rmsm_state)
      || (DS707_RMSM_UM_RM_UP_MIP_STATE == rmsm_state)
      || (DS707_RMSM_RESYNCING_UM_MIP_STATE == rmsm_state)
      || (DS707_RMSM_RM_UP_WAITING_FOR_UM_DOWN_MIP_STATE == rmsm_state))
  {
    return TRUE;
  }
#endif /* FEATURE_DS_MOBILE_IP */

  return FALSE;
} /* ds707_rmsmi_is_mip_call() */

/*===========================================================================
FUNCTION   RMSMI_707_PKT_IFACE_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface 
  events occur on IS707_PKT_IFACE. Registraion is done for various events 
  during the call setup based upon the call type. As part of the event 
  handling appropriate event is posted to RMSM or an appropriate action is 
  taken.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void rmsmi_707_pkt_iface_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG2( MSG_LEGACY_MED, "Recvd iface ev %d from 1x i/f 0x%x",
                   event, this_iface_ptr);

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case IFACE_FLOW_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel call send signal to PS to resume pulling PPP packets 
        from SIO rx_wm, laptop flow will be enabled by rx_wm lo water func.
        For relay calls we do need to enable the flow to handle cases like 
        flow enable due to holddown timer etc. 
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val ==
                                                  DS_CRM_VAL_NETMODEL_MODE )
      {
        PS_SET_SIGNAL( PPP_RM_RX_SIGNAL );
      }
      else
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_ENABLE);
      }

      break;
    }

    case IFACE_FLOW_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel calls we don't need to do anything when UM flow is 
        disabled, rx function will stop pulling PPP packets from SIO rx_wm. 
        rx_wm hi water function will disable laptop flow.
        For relay calls we do need to disable flow to handle cases like 
        flow disable due to holddown timer etc.
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val !=
                                                  DS_CRM_VAL_NETMODEL_MODE )
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_DISABLE);
      }

      break;
    }

    case IFACE_UP_EV:
    {
      /*---------------------------------------------------------------------
        Enable flow (for MIP calls) which would have been disabled when iface 
        was set to configuring. Send signal to PS to resume pulling PPP pkts
        from SIO rx_wm, laptop flow will be enabled by rx_wm lo water func.
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val ==
                                                  DS_CRM_VAL_NETMODEL_MODE )
      {
        PS_SET_SIGNAL( PPP_RM_RX_SIGNAL );
      }

      /*---------------------------------------------------------------------
        UM iface is up. 
      ---------------------------------------------------------------------*/
      if( event_info.iface_up_info.state != IFACE_UP )
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                         "IS707 iface up, posting DS707_RMSM_UM_IFACE_UP_EV");
        ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_UP_EV );
      }
      break;
    }

    case IFACE_DOWN_EV:
    {
      /*---------------------------------------------------------------------
        UM iface is down. This event can also be generated when we register 
        for events since that happens during call setup ie UM is in DOWN 
        state. So check the previous state before taking any action.
        Also enable flow for MIP calls, which would have been disabled if
        MIP were configuring the iface and the action failed (hence the
        iface down event).
      ---------------------------------------------------------------------*/
      if( event_info.iface_down_info.state != IFACE_DOWN )
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, "IS707 iface down, posting "
                         "DS707_RMSM_UM_IFACE_DOWN_EV");
        ds707_rmsm_post_event( DS707_RMSM_UM_IFACE_DOWN_EV );

        if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val ==
                                                  DS_CRM_VAL_NETMODEL_MODE )
        {
          PS_SET_SIGNAL( PPP_RM_RX_SIGNAL );
        }
      }
      break;
    }

    case IFACE_CONFIGURING_EV:
    {
#ifdef FEATURE_DS_MOBILE_IP
      if(ds707_rmsmi_info.state != DS707_RMSM_WAITING_FOR_UM_UP_MIP_STATE)
#endif /* FEATURE_DS_MOBILE_IP */
      {
        ds707_pkt_trash_pri_rlp_tx_wmks( 
                                     ds707_rmsmi_info.IS707pkt_iface_ptr );
      }
      ds707_rmsm_post_event( DS707_RMSM_UM_RESYNC_EV );
      break;
    }

    case IFACE_PHYS_LINK_UP_EV:
    {
      /*---------------------------------------------------------------------
        Traffic channel on UM has come up. This event is used for Relay and
        network model call setup.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "IS707 phys link up, posting "
                       "DS707_RMSM_UM_PHYS_LINK_UP_EV");
      ds707_rmsm_post_event( DS707_RMSM_UM_PHYS_LINK_UP_EV );
      break;
    }

    case IFACE_ADDR_CHANGED_EV:
     /*----------------------------------------------------------------------
       ip address has changed during MIP call. terminate the call gracefully
     ----------------------------------------------------------------------*/
    {
      if(event_info.ip_addr.addr.v4.ps_s_addr != 0)
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                         "IP address changed during MIP call");
        ds707_rmsm_post_event( DS707_RMSM_UM_MIP_IP_ADDR_CHANGED_EV );
      }
      break;
    }

    case IFACE_PHYS_LINK_GONE_EV:
    {
      /*---------------------------------------------------------------------
        Check previous phys_link state to see if phys_link is indeed down.
      ---------------------------------------------------------------------*/
      if(event_info.phys_link_event_info.state != PHYS_LINK_NULL)
      {
        /*-------------------------------------------------------------------
          Don't post IFACE down if it is a MIP call. If it is a MIP call
          dssnet will be resposible for posting the Um iface down later
        -------------------------------------------------------------------*/
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "IS707 phys link gone. RMSM state %d",
                        ds707_rmsm_get_state());

        if(FALSE == ds707_rmsmi_is_mip_call())
        {
          /*-------------------------------------------------------------------
          Traffic channel on UM has gone NULL.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                         "IS707 phys link gone. Set ps_iface down ind" );

        ps_flow_go_null_ind(
           PS_IFACE_GET_DEFAULT_FLOW(this_iface_ptr),
           PS_EIC_NOT_SPECIFIED);

           ps_iface_down_1x_v4_ind( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                    ds707_pkt_mgr_get_last_call_end_reason() );
        }

        ds707_rmsm_post_event( DS707_RMSM_UM_PHYS_LINK_GONE_EV );
      }
      break;
    }

    default:
    {
      /* Ignore */
      break;
    }
  } /* switch( event ) */
} /* rmsmi_707_pkt_iface_event_handler_cb() */

/*===========================================================================
FUNCTION   RMSMI_707_PKT_PHYS_LINK_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_phys_link 
  events occur on the phys link. Registraion is done for various events 
  during the call setup based upon the call type. As part of the event 
  handling appropriate event is posted to RMSM or an appropriate action is 
  taken.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void rmsmi_707_pkt_phys_link_event_handler_cb
(
  ps_phys_link_type         *phys_link_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "Recvd phys_link ev %d from 1x phys link 0x%x", 
                   event, 
                   phys_link_ptr);

  
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case PHYS_LINK_FLOW_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel call send signal to PS to resume pulling PPP packets 
        from SIO rx_wm, laptop flow will be enabled by rx_wm lo water func.
        For relay calls we do need to enable the flow to handle cases like 
        flow enable due to holddown timer etc. 
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                   DS_CRM_VAL_NETMODEL_MODE )
      {
        PS_SET_SIGNAL( PPP_RM_RX_SIGNAL );
      }
      else
      {
        ds3g_siolib_set_inbound_flow( DS_FLOW_707_PKT_MASK, 
                                      DS_FLOW_ENABLE);
      }

      break;
    }
    
    case PHYS_LINK_FLOW_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel calls we don't need to do anything when UM flow is 
        disabled, rx function will stop pulling PPP packets from SIO rx_wm. 
        rx_wm hi water function will disable laptop flow.
        For relay calls we do need to disable flow to handle cases like 
        flow disable due to holddown timer etc.
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val !=
                                                  DS_CRM_VAL_NETMODEL_MODE )
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_DISABLE);
      }

      break;
    }

    default:
    {
      /* Ignore */
      break;
    }
  } /* switch( event ) */
} /* rmsmi_707_pkt_phys_link_event_handler_cb() */

/*===========================================================================
FUNCTION   RMSMI_707_PKT_FLOW_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_flow 
  events occur. Registraion is done for various events 
  during the call setup based upon the call type. As part of the event 
  handling appropriate event is posted to RMSM or an appropriate action is 
  taken.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void rmsmi_707_pkt_flow_event_handler_cb
(
  ps_flow_type              *flow_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "Recvd flow ev %d from 1x 0x%x",
                   event, flow_ptr);

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case FLOW_TX_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel call send signal to PS to resume pulling PPP packets 
        from SIO rx_wm, laptop flow will be enabled by rx_wm lo water func.
        For relay calls we do need to enable the flow to handle cases like 
        flow enable due to holddown timer etc. 
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                   DS_CRM_VAL_NETMODEL_MODE )
      {
        PS_SET_SIGNAL( PPP_RM_RX_SIGNAL );
      }
      else
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_ENABLE);
      }

      break;
    }
    
    case FLOW_TX_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For netmodel calls we don't need to do anything when UM flow is 
        disabled, rx function will stop pulling PPP packets from SIO rx_wm. 
        rx_wm hi water function will disable laptop flow.
        For relay calls we do need to disable flow to handle cases like 
        flow disable due to holddown timer etc.
      ---------------------------------------------------------------------*/
      if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val !=
                                                  DS_CRM_VAL_NETMODEL_MODE )
      {
        ds3g_siolib_set_inbound_flow(DS_FLOW_707_PKT_MASK, DS_FLOW_DISABLE);
      }

      break;
    }

    default:
    {
      /* ignoring */
      break;
    }
  } /* switch( event ) */
} /* rmsmi_707_pkt_flow_event_handler_cb() */

/*===========================================================================
FUNCTION   DS707_RMSMI_PPPRM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered PPP events
  occur on RM PPP device. As part of event handling appropriate event is 
  posted to RMSM.

DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(user_data);

  DATA_IS707_MSG1( MSG_LEGACY_MED, "Recd %d ppp event", ppp_event);

  ASSERT( ppp_dev == PPP_RM_DEV );
  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
      Currently we only handle the down event, we should not be getting 
      other events
    -----------------------------------------------------------------------*/
    case PPP_DOWN_EV:
      ds707_rmsm_post_event( DS707_RMSM_RM_IFACE_DOWN_EV );
      break;

    case PPP_UP_EV:
      ds707_rmsm_post_event( DS707_RMSM_RM_PPP_UP_EV );
      break;

    case PPP_RESYNC_EV:
      break;

    default:
      DS_FORCE_ASSERT();
      break;
  }
} /* ds707_rmsmi_ppprm_event_handler_cb */


/*===========================================================================
FUNCTION   DS707_RMSMI_PPPUM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered PPP events
  occur on UM PPP device. As part of event handling appropriate event is 
  posted to RMSM.

DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_pppum_event_handler_cb
(
  ppp_dev_enum_type       ppp_dev,
  ppp_protocol_e_type     protocol,
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
  boolean                   *ppp_resync_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(user_data);

  DATA_IS707_MSG1(MSG_LEGACY_MED,  "Recd %d ppp event", ppp_event);

  ASSERT( ppp_dev == PPP_UM_SN_DEV );
  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
      Currently we only hanlde the down event, we should not be getting 
      other events
    -----------------------------------------------------------------------*/
    case PPP_DOWN_EV:
      if(ds707_rmsmi_info.IS707pkt_iface_ptr != NULL)
      {
        ps_iface_enable_flow ( ds707_rmsmi_info.IS707pkt_iface_ptr,
                               DS_FLOW_707_RMSM_MASK);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                         "Um iface ptr NULL. Cannot enable flow");
      }

      if (PPP_PROTOCOL_LCP == protocol)
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Received LCP Down Event from PPP engine");
      }

    /*-----------------------------------------------------------------------
      Reset the PPP resync flag since the Um PPP went down. We do not
      need to attempt any more PPP resync for this packet data session
    -----------------------------------------------------------------------*/
      ppp_resync_ptr  = ds707_get_ppp_resync_flag_ptr();
      *ppp_resync_ptr = FALSE;
      ds707_rmsm_post_event( DS707_RMSM_UM_PPP_DOWN_EV );
      break;

    case PPP_RESYNC_EV:
      ps_iface_configuring_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      ps_iface_disable_flow( ds707_rmsmi_info.IS707pkt_iface_ptr,
                             DS_FLOW_707_RMSM_MASK);
      break;

    case PPP_UP_EV:
      ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
      ps_iface_enable_flow ( ds707_rmsmi_info.IS707pkt_iface_ptr,
                             DS_FLOW_707_RMSM_MASK);
    /*-----------------------------------------------------------------------
      Reset the PPP resync flag since the Um PPP went down. We do not
      need to attempt any more PPP resync for this packet data session
    -----------------------------------------------------------------------*/
      ppp_resync_ptr  = ds707_get_ppp_resync_flag_ptr();
      *ppp_resync_ptr = FALSE;
      break;

    default:
      DS_FORCE_ASSERT();
      break;
  }
} /* ds707_rmsmi_pppum_event_handler_cb */


/*===========================================================================
  FUNCTION DS707_RMSMI_SETUP_SIO_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the 1X calls handled by RMSM.

  PARAMETERS
    *tx_wm_ptr - ptr to sio transmit (to laptop) watermark
    *rx_wm_ptr - ptr to sio receive (from laptop) watermark

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause ERR_FATALs due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds707_rmsmi_setup_sio_watermarks
(
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr
)
{
  sio_ioctl_param_type ds707_rmsmi_sio_ioctl_param;
  nv_stat_enum_type   nv_status;
  nv_item_type         ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure that the watermarks passed in are empty
  -------------------------------------------------------------------------*/
  if(tx_wm_ptr->current_cnt != 0)
  {
    dsm_empty_queue(tx_wm_ptr);
  }
  if(rx_wm_ptr->current_cnt != 0)
  {
    dsm_empty_queue(rx_wm_ptr);
  }

  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This WaterMark Queue is to be used for the purposes of SIO to laptop 
    transmisssion. i.e. Fwd Link Data
    Watermark levels for Forward Link Transmission depend on whether
    REVB is supported or not
  -------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
    Changing the Watermark levels for the Reverse link transmission based
    on whether REVB is supported or not.
  -------------------------------------------------------------------------*/
  ds707_rmsm_update_wmk_value();
  
  
  tx_wm_ptr->gone_empty_func_ptr   = NULL;
  tx_wm_ptr->non_empty_func_ptr    = NULL;
  tx_wm_ptr->lowater_func_ptr      = ds707_rmsmi_sio_tx_lowater_func;
  tx_wm_ptr->hiwater_func_ptr      = ds707_rmsmi_sio_tx_hiwater_func;
  tx_wm_ptr->each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Configure the watermark used for receiving data from SIO.
    to SIO. 
    SIO watermark use should be synchonized with the ISR used by the 
    serial driver. Also for bluetooth, provide more cushion between hi and 
    don't exceed levels.
    Removed checking for the flow control before setting the watermark 
    levels. Observe that the watermark levels are same for software 
    Flowcontrol and hardware flowcontrol.
  -------------------------------------------------------------------------*/
  ds707_rmsmi_sio_ioctl_param.flow_ctl.tx_flow = SIO_FCTL_BEST;
  ds707_rmsmi_sio_ioctl_param.flow_ctl.rx_flow = SIO_FCTL_BEST;

  ds3g_siolib_ioctl( SIO_IOCTL_GET_FLOW_CTL, &ds707_rmsmi_sio_ioctl_param );

  rx_wm_ptr->lowater_func_ptr      = ds707_rmsmi_sio_rx_lowater_func;
  rx_wm_ptr->hiwater_func_ptr      = ds707_rmsmi_sio_rx_hiwater_func;
  rx_wm_ptr->gone_empty_func_ptr   = ds707_rmsmi_sio_rx_gone_empty_func;
  //rx_wm_ptr->non_empty_func_ptr    = NULL;
  rx_wm_ptr->each_enqueue_func_ptr = ds707_rmsmi_sio_rx_non_empty_func;

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  tx_wm_ptr->total_rcvd_cnt        = 0;
  rx_wm_ptr->total_rcvd_cnt        = 0;
#endif

  timer_def( &ds707_smd_notify_delay_timer, 
             NULL, 
             NULL, 
             0, 
             ds707_rmsmi_smd_notify_timer_expired, 
             0);

  ds707_ds_smem_tx_delay = 0;
  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DATA_AGG_TIMER_I, &ds707_nv_item, ds707_curr_subs_id());
  if( nv_status == NV_DONE_S)
  {
     ds707_ds_smem_tx_delay = 
              ds707_nv_item.data_agg_timer[RMSM_AGG_TIMER_NV_INDEX];

    //read the element at index 1, because it corresponds to smd data port 1.
     if(ds707_ds_smem_tx_delay == 0) {
         DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                          "NV read, smd_notify_delay_timer is not configured"
                          ": NV_DATA_AGG_TIMER_I, element at index 1: [%d]",
                          ds707_ds_smem_tx_delay );
     } 
     else 
     {
       DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                        "NV read, smd_notify_delay_timer : [%d]",
                        ds707_ds_smem_tx_delay);
     }
  }
  else 
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "Bad NV read status for smd_notify_delay_timer: %d, "
                     "defaulting to %d",
                     nv_status,
                     DEFAULT_RMSM_AGG_TIMER_TIMEOUT);
    // cannot read nv item for the timeout value for aggregation timer. 
    // Set the delay to default to 10 msec
    ds707_ds_smem_tx_delay = DEFAULT_RMSM_AGG_TIMER_TIMEOUT;
  }

  if (ds707_ds_smem_tx_delay)
  {
    tx_wm_ptr->non_empty_func_ptr    = ds707_rmsmi_tx_wm_non_empty_func;
    tx_wm_ptr->non_empty_func_data   = NULL ;
  }
  
} /* ds707_rmsmi_setup_sio_watermarks() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_GONE_EMPTY_FUNC()

  DESCRIPTION
    This function performs the 'Now Empty' event processing for the PACKET
    DATA SERVICES shared queue. This function is called when the SIO RX
    data stream watermark item goes empty. It clears its idle bit which
    allows IS707_PKT_CTRL to start cta dormancy if needed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_sio_rx_gone_empty_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "RM Rx ( SIO --> AT ) watermark gone empty for WM %x", 
                   wm_ptr);
} /* ds707_rmsmi_sio_rx_gone_empty_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_NON_EMPTY_FUNC()

  DESCRIPTION
    This function performs the 'Non Empty' event processing for the PACKET
    DATA SERVICES shared queue. This function is called when the SIO RX
    data stream watermark item gets its first item enqueued. It sets its
    idle bit which allows IS707_PKT_CTRL to end cta dormancy if needed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_sio_rx_non_empty_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  ds707_mip_profile_type  *ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  /*-------------------------------------------------------------------------
    Send signal to PS to start pulling stuff off the SIO for network model.
  -------------------------------------------------------------------------*/
  if ( ds_1x_runtime_profile_ptr->ds707.ds707_crm_val == 
                                                  DS_CRM_VAL_NETMODEL_MODE ) 
  {
    DATA_IS707_MSG2(  MSG_LEGACY_LOW, 
                      "RM Enqueue Data %d count = %d Setting Signal to PS", 
                      wm_ptr, wm_ptr->current_cnt);
    PS_SET_SIGNAL( PPP_RM_RX_SIGNAL );
  }
} /* ds707_rmsmi_sio_rx_non_empty_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_LOWATER_FUNC()

  DESCRIPTION
   This function enables inbound serial flow, when the lowater point in the
   packet watermark is reached.

   This function is called from RLP TX processing, which occurs in the decode
   ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_sio_rx_lowater_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "RM Rx ( SIO --> AT ) low watermark reached for WM %x "
                   "count = %d",
                   wm_ptr,
                   wm_ptr->current_cnt);
  ds3g_siolib_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_ENABLE );
} /* ds707_rmsmi_sio_rx_lowater_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_RX_HIWATER_FUNC()

  DESCRIPTION
   This function disables inbound serial flow, when the hiwater point in the
   packet watermark is reached.

   This is called from the serial RX ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_sio_rx_hiwater_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "RM Rx ( SIO --> AT ) high watermark reached for WM %x "
                   "count = %d",
                   wm_ptr,
                   wm_ptr->current_cnt);
  ds3g_siolib_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_DISABLE );
}  /* ds707_rmsmi_sio_rx_hiwater_func() */


/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_TX_LOWATER_FUNC()

  DESCRIPTION
   This function generates an event to enable flow on the forward link Um 
   interface, when the lowater point in the
   packet watermark is reached.


  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsmi_sio_tx_lowater_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  ds_cmd_type *cmd_ptr = NULL;                   /* DS Task cmd buf        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG3( MSG_LEGACY_HIGH,
                   "RMSM SIO Tx ( AT --> SIO ) low watermark reached, "
                   "post DS_707_RM_IFACE_FLOW_ENABLED_CMD to DS wmptr=0x%x "
                   "user_data=0x%x count:%d",
                   wm_ptr,
                   user_data_ptr,
                   wm_ptr->current_cnt);

  if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    ASSERT(0);
    return;
  }
  else
  {
    /* Post a cmd to DS task to process lo wmk and enable flow */
    cmd_ptr->hdr.cmd_id = DS_CMD_707_RM_IFACE_FLOW_ENABLED;
    ds_put_cmd( cmd_ptr );
  }

} /* ds707_rmsmi_sio_tx_lowater_func() */

/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_TX_HIWATER_FUNC()

  DESCRIPTION
   This function generates an event to disable flow on the forward link Um 
   interface, when the hiwater point in the
   packet watermark is reached.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsmi_sio_tx_hiwater_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  ds_cmd_type *cmd_ptr;                          /* DS Task cmd buf        */
  sio_ioctl_param_type wait_param;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                   "RMSM SIO Tx ( AT --> SIO ) hi watermark reached, post "
                   "DS_707_RM_IFACE_FLOW_DISABLED_CMD to DS wm_ptr:0x%x, "
                   "user_data:0x%x count:%d",
                   wm_ptr,
                   user_data_ptr,
                   wm_ptr->current_cnt);
  
  if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    ASSERT(0);
    return;
  }

  /* Post a command to DS task to process high wmk and disable flow */
  cmd_ptr->hdr.cmd_id = DS_CMD_707_RM_IFACE_FLOW_DISABLED;
  ds_put_cmd( cmd_ptr );

  // Since the high watermark has hit, call the foll. ioctl in sio to clear 
  // the wait 
  // This will call notifywrite and send data across immediately.
  if (ds707_ds_smem_tx_delay)
  {
    wait_param.wm_wait = FALSE;
    ds3g_siolib_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param);
    timer_clr(&ds707_smd_notify_delay_timer, T_MSEC);
  }
 
}  /* ds707_rmsmi_sio_rx_hiwater_func() */

/*===========================================================================
  FUNCTION DS707_RMSMI_SIO_TX_RLP_DATA()

  DESCRIPTION
    This function performs the processing of Received RLP Data frames for the
    Packet Data Service. All processed RX data frames will be queued for
    Serial TX.

    This function will be invoked in Packet mode or PMA active mode by RLP RX
    processing upon reception of error free Data frame(s).

  PARAMETERS
    None

  DEPENDENCIES
    1) The 'data_ptr' field in each RLP data frame must have previously been
       set to the correct location of the RLP payload data.
    2) The queued data frames contain valid length field.
    3) Must be in PKT srvc mode.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_sio_tx_rlp_data( uint32 unused )
{
  dsm_item_type     *item_ptr = NULL;             /* pointer to DSM item   */
  dsm_item_type     *item_tail_ptr = NULL;
  boolean           relay_model_flag =  FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  if ( ds707_rmsmi_info.rmsm_sent_connect == TRUE )
  {
    relay_model_flag = ds707_rmsm_is_relay_model_call();
    /*-----------------------------------------------------------------------
    Remove each item from Completed Data queue and pass to Serial Driver.
  -------------------------------------------------------------------------*/
    while( ( item_ptr = 
           dsm_dequeue (&ds707_rmsmi_info.rlp_fwd_wmk_list[DSRSP_ROUTE_A] ) 
         ) != NULL )
    {
      if ( relay_model_flag == TRUE )
      {
        /* If this is the relay model call, it needs to split
         * DSM items if DSM chain has more than 5000 bytes.
         */
        while ( item_ptr != NULL )
        {
          (void) dsm_split_packet
                 (
                 &item_ptr, 
                 &item_tail_ptr, 
                 MAX_PKT_SIZE, 
                 DSM_DUP_ITEM_POOL
                 );
          /* Send item to SIO */
          (void) ds3g_siolib_setup_tx( item_ptr, FALSE );
          item_ptr = item_tail_ptr;
          /* Reset tail pointer. It is needed when calling dsm_split_packet() */
          item_tail_ptr = NULL;
        }
      }
      else
      {
        /* 
         * PPP/HDLC will take care of network model call for passing
         * small dsm chains to A2/SMD. We do not need to worry this here.
         */
        (void) ds3g_siolib_setup_tx( item_ptr, FALSE );
      }
    }
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Emptying rlp fwd wmk.  No Connect sent yet");
    dsm_empty_queue(&ds707_rmsmi_info.rlp_fwd_wmk_list[DSRSP_ROUTE_A]);
  }
} /* ds707_rmsmi_sio_tx_rlp_data() */


/*===========================================================================
FUNCTION DS707_RMSMI_SIO_TX_PPP_DATA

DESCRIPTION
  This function is called to transmit a packet coming on Um PPP
  interface onto Rm interface.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
LOCAL void ds707_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void *meta_info_ptr,
  void *user_data
)
{
  void ** meta_info_ptr_ptr;
  /*------------------------------------------------------------------------
    Clear meta info!
  ------------------------------------------------------------------------*/
  if ((ps_meta_info_type *)meta_info_ptr != NULL)
  {
    meta_info_ptr_ptr = &meta_info_ptr;
    PS_META_INFO_FREE((ps_meta_info_type **)meta_info_ptr_ptr);
  }

  /* Do we have data to send? */
  if ((item_head_ptr == NULL) || (*item_head_ptr == NULL))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds707_rmsmi_sio_tx_ppp_data:Got a null pointer");
    /* Nothing to transmit, return */
    return;
  }

  DATA_IS707_MSG1( MSG_LEGACY_LOW, 
                   "ds707_rmsmi_sio_tx_ppp_data:Giving DSM Item length %d "
                   "to SIOLib",
                   dsm_length_packet(*item_head_ptr));
  (void) ds3g_siolib_setup_tx( *item_head_ptr, FALSE );
  *item_head_ptr = NULL;

} /* ds707_rmsmi_sio_tx_ppp_data() */


/*===========================================================================
FUNCTION DS707_RMSMI_SIO_RX_PPP_DATA

DESCRIPTION
  This function is called to receive the next PPP packet, coming on the Rm
  interface, for rx processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
LOCAL dsm_item_type* ds707_rmsmi_sio_rx_ppp_data( void *user_data )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_phys_link_type *ps_phys_link_ptr;
  ps_flow_type  * ps_flow_ptr;

  DATA_MDM_ARG_NOT_CONST(user_data);

  /*-------------------------------------------------------------------------
    Don't pull data out of the queue if Um iface is flow controlled or in
    CONFIGURING state (MIP reg in progress during call setup or a handoff).
    
    Note: For Um in COMING_UP state, we want to keep the packets flowing to 
    perform end to end PPP negotiation (for SIP network model calls).
  -------------------------------------------------------------------------*/

  ps_phys_link_ptr = 
               PS_IFACE_GET_PHYS_LINK(ds707_rmsmi_info.IS707pkt_iface_ptr );
  ps_flow_ptr = 
             PS_IFACE_GET_DEFAULT_FLOW(ds707_rmsmi_info.IS707pkt_iface_ptr );

  if ( ( PS_IFACE_FLOW_ENABLED( ds707_rmsmi_info.IS707pkt_iface_ptr ) ) &&
       ( PS_PHYS_LINK_FLOW_ENABLED(ps_phys_link_ptr) ) &&
       ( PS_FLOW_IS_TX_ENABLED(ps_flow_ptr) ) &&
       ( ps_iface_state(ds707_rmsmi_info.IS707pkt_iface_ptr) != 
           IFACE_CONFIGURING )
     )
  { 
    return dsm_dequeue( &ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A] );
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Flow disabled!");
    return NULL;
  }
} /* ds707_rmsmi_sio_rx_ppp_data() */


/*===========================================================================
FUNCTION DS707_RMSM_NOTIFY_PPP_UP

DESCRIPTION
  This function notifies RMSM that the end to end PPP is complete during a 
  SIP network model call.  This means that a SIP network model is 
  actually "UP" and hence RMSM sets the iface states appropriately.
    - Move SIO iface to UP state since nothing special needs to be done. 
    - Move IS707 to Routeable state since we are in a laptop call. 
    - Put the phys link state on SIO iface to UP so that PPP can 
      gracefully terminate during call  termination.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds707_rmsm_notify_ppp_up
(
  void
)
{  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (ds707_rmsmi_info.state == DS707_RMSM_E2E_PPP_UP_SIP_STATE) ||
      (ds707_rmsmi_info.state == DS707_RMSM_UM_RM_UP_RELAY_STATE))
  {  
    /*-----------------------------------------------------------------------
      Setup bridging Um <-> Rm ifaces. 
    -----------------------------------------------------------------------*/
    ppp_set_mode( PPP_RM_DEV, PPP_INTERNAL_SNOOP_MODE );
    ppp_set_mode( PPP_UM_SN_DEV, PPP_INTERNAL_SNOOP_MODE );

    ps_iface_set_bridge(&ds707_rmsmi_info.sio_pkt_iface,
                       ds707_rmsmi_info.IS707pkt_iface_ptr);
    ps_iface_set_bridge(ds707_rmsmi_info.IS707pkt_iface_ptr,
                        &ds707_rmsmi_info.sio_pkt_iface);
                            
    ps_iface_routeable_ind( ds707_rmsmi_info.IS707pkt_iface_ptr );
    ps_iface_phys_link_up_ind( &ds707_rmsmi_info.sio_pkt_iface );
    ps_iface_routeable_ind( &ds707_rmsmi_info.sio_pkt_iface );
  }
} /* ds707_rmsm_notify_ppp_up() */

/*===========================================================================
FUNCTION DS707_RMSMI_TRIM_DIAL_STR

DESCRIPTION
  This function trims the input dial string and eliminates all the
  unwanted characters. This is to avoid issues where characters 
  allowed in the spec are before or after the token searched for
  against the dial string
  
DEPENDENCIES
  dial_str_after_trim must 

RETURN VALUE
  allocation and cleanup of dial_str_after_trim handled by the caller

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds707_rmsmi_trim_dial_str
(
  const byte *orig_dial_str,
  byte *dial_str_after_trim
)
{  
  byte *s_ptr = NULL;
  const byte *p_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


 /* Perform aggressive strip on dial string to remove unwanted characters. */
 /* We do this assuming packet dial strings are limited to range [0-9#*].  */
 /* Avoids issue where characters allowed in the spec are before or        */
 /* after the token searched for against dial string                       */

  p_ptr = orig_dial_str;
  s_ptr = dial_str_after_trim;

  while (('\0' != *p_ptr) &&
     (MAX_PKT_DIAL_STRING_LEN > (s_ptr - dial_str_after_trim)))
  {
    /* Test range */
    if ( (('0' <= *p_ptr) && ('9' >= *p_ptr)) ||
     ('#' == *p_ptr) ||
     ('*' == *p_ptr) )
    {
      /* Save wanted character */
      *s_ptr++ = *p_ptr;
    }
    p_ptr++;
  }
  *s_ptr = '\0';

} /* ds707_rmsm_trim_dial_str() */

/*===========================================================================
FUNCTION      DS707_PKT_IS_RM_IFACE_UP

DESCRIPTION   Returns TRUE if the RM iface is up.

DEPENDENCIES  phys_link should be up or coming up.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_rmsm_is_rm_iface_up
(
  void
)
{
  /*--------------------------------------------------------------------
    The checks for IFACE_UP is for sockets, for IFACE_ROUTEABLE is for
    network model laptop calls
  --------------------------------------------------------------------*/
  if( (ps_iface_state (&ds707_rmsmi_info.sio_pkt_iface) == 
                                                         IFACE_ROUTEABLE) ||
	           (ps_iface_state (&ds707_rmsmi_info.sio_pkt_iface) == IFACE_UP) )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_rmsm_is_rm_iface_up */

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MULTIPLE_NAI

/*===========================================================================
FUNCTION DS707_RMSM_GET_MIP_SESSION_INFO_CB()

DESCRIPTION
  Function registered with MIP to be called at the end of MIP config session
  to update NAI information for laptop calls. 

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_get_mip_session_info_cb
(
  byte                     active_mip_profile,
  mip_session_info_type *  session
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mip_reg_ses_info_cb( NULL );
  if(mip_get_rm_nai(session,active_mip_profile)!=active_mip_profile)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Error reading tethered NAIs from NV");
    return;
  }
} /* ds707_rmsm_get_mip_session_info_cb() */
#endif /* FEATURE_DS_MULTIPLE_NAI */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DS707_RMSM_IS_NULL()

DESCRIPTION
  This function  returns the state of PPP on tethered device.
  Returns TRUE if RMSM is in NULL state, else returns FALSE.

PARAMETERS
  None

RETURN VALUE
   TRUE if RMSM is NULL state, else returns FALSE.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean ds707_rmsm_is_null
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ds707_rmsmi_info.state == DS707_RMSM_NULL_STATE ) 
  {
    return TRUE;
  }
  return FALSE;
} /* ds707_rmsm_null() */

/*===========================================================================
FUNCTION DS707_RMSM_GET_UM_IFACE()

DESCRIPTION
  This function  returns the UM iface associaetd with the current SN PPP cal

PARAMETERS
  None

RETURN VALUE
   return rmsmi-info.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

ps_iface_type *ds707_rmsm_get_um_iface
(
  void
)
{
  if(rmsmi_IS707pkt_iface_brought_up == TRUE)
  {
    return (ds707_rmsmi_info.IS707pkt_iface_ptr);
  }
  else
  {
    return NULL;
  }
}
/*===========================================================================
FUNCTION DS707_RMSM_GET_SIO_IFACE()

DESCRIPTION
  This function  returns the SIO iface associaetd with the current SN PPP call.
  Used by BCRM moduel to register for events on RMIface.

PARAMETERS
  None

RETURN VALUE
   return rmsmi-info.sio_pkt_iface.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

ps_iface_type *ds707_rmsm_get_sio_iface
(
  void
)
{
  return (&(ds707_rmsmi_info.sio_pkt_iface));
} /* ds707_rmsm_get_sio_iface  */

/*===========================================================================
FUNCTION DS707_RMSM_IS_RELAY_MODEL_CALL()

DESCRIPTION
  This function indicates if a relay model call is active.

PARAMETERS
  None

RETURN VALUE
   Returns True if Relay Call is active, False otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_rmsm_is_relay_model_call
(
  void
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  if( (ds_1x_runtime_profile_ptr->ds707.ds707_crm_val != 
                                                DS_CRM_VAL_NETMODEL_MODE) && 
      (ds707_rmsm_is_rm_iface_up() == TRUE))
  {
    return TRUE;
  }
  return FALSE;
}

boolean ds707_rmsm_will_be_relay_model_call
(
  void
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  if(ds_1x_runtime_profile_ptr->ds707.ds707_crm_val != 
                                                DS_CRM_VAL_NETMODEL_MODE)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION DS707_RMSMI_PROCESS_LINGERING_CALL()

DESCRIPTION
  This function tears down the current PPP session (brings down the IFACE) if
  the IFACE is in Lingering state and new call and the current call are not
  profile compatible i.e. they are foes.

PARAMETERS
  acl_policy_info_type* - pointer to the policy struct

RETURN VALUE
   boolean  - TRUE if lingering call processing is required i.e. if there is
              a need to bring down the current lingering PPP session and then
              reestablish the new PPP session

              FALSE if there is no need to torn down the current PPP session

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds707_rmsmi_process_lingering_call
( 
  acl_policy_info_type                    *policy_info_ptr
)
{
  ps_iface_type                           *if_ptr = NULL;
  /* profile id of the new call */
  ds707_data_session_profile_id pending_id; 
  /* iface tear down cmd err no */
  int16                                   ps_iface_errno;
  /* iface tear down cmd ret val */
  int                                     ret_val = 0;
  uint8                                   curr_iface_index;
  ds707_data_session_profile_info_type   *current_prof_ptr;
/*-------------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    pending_id = DATA_SESSION_JCDMA_INET_PROFILE;
  }
  else
  {
    pending_id =  DATA_SESSION_DEFAULT_PROFILE;
  }

  /*---------------------------------------------------------------------
    Tear down all ifaces that are in lingering state
    check if any iface is in lingering state
    check for the compatibility of the iface profiles and laptop profiles
    if both the above checks pass then 
        - register for iface down event
        - call ps_iface_go_null_cmd()
        - deregister for the event
        - in the down event call back call 
          ds707_rmsmi_process_rm_wants_pkt_call_ev
  ---------------------------------------------------------------------*/
  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {      
    if_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if  ( ps_iface_state(if_ptr) == IFACE_LINGERING ) 
    {
      ds707_data_session_get_iface_current_profile(if_ptr,
                                                   &current_prof_ptr);
      if (!(ds707_data_session_compatible_profiles(if_ptr,
                                                   policy_info_ptr,
                                                   pending_id,
                                                   current_prof_ptr)))
      {
      /*---------------------------------------------------------------------
          register for IFACE DOWN EV
      ---------------------------------------------------------------------*/
        if ( ps_iface_event_cback_reg( ds707_rmsmi_info.IS707pkt_iface_ptr,
                                       IFACE_DOWN_EV,
                                       ds707_rmsmi_info.um_iface_down_buf_ptr 
                                     ) < 0 )
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "IFACE_DOWN_EV reg failed. "
                          "Disallowing the laptop call");
        }
        else
        {
        /*-------------------------------------------------------------------
            Set the reestablish_ppp flag and then tear down the iface.
        -------------------------------------------------------------------*/
          rmsmi_reestablish_um_link = TRUE;
          ret_val = ps_iface_go_null_cmd ( if_ptr, &ps_iface_errno, NULL );
          if ((ret_val < 0) && (ps_iface_errno != DS_EWOULDBLOCK))
          {
            DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                             "Lingering iface tear down fail, errno: %d",
                             ps_iface_errno);
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_MED, 
                             "Lingering iface tear down success");
          }
        }
      }/* if (!(ds707_data_session_compatible_profiles(pending_id,current_id))) */
    }/* if (ps_iface_state(if_ptr) == IFACE_LINGERING) */
  } /* for each iface */  
  return (rmsmi_reestablish_um_link);
}

/*===========================================================================
FUNCTION DS707_CLEANUP_UM() 

DESCRIPTION
  Calls ps_iface_go_null_cmd, if iface already down transition to 
  DS707_RMSM_NULL_STATE else transition to DS707_RMSM_WAITING_FOR_UM_DOWN_STATE. 

PARAMETERS
  ps_iface_errno - out parameter updated by ps_iface_go_null_cmd

RETURN VALUE
   None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void ds707_cleanup_um(int16* ps_iface_errno)
{
  /*---------------------------------------------------------------------
    Since laptop has terminated the call, we need to cleanup Um before
    completely terminating the call. Tear down the 707 pkt iface first.
    Checking for the return code as the iface could already be down.
  ---------------------------------------------------------------------*/
  if ( ps_iface_go_null_cmd(ds707_rmsmi_info.IS707pkt_iface_ptr,
                              ps_iface_errno,
                              NULL ) == 0 )
  {
    /*-------------------------------------------------------------------
      If the IS707 iface is already down we should cleanup the call
      on Rm since we won't get any other notification from IS707
      controller
    -------------------------------------------------------------------*/
    ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
  }
  else
  {
    /*-------------------------------------------------------------------
      Otherwsie wait for IS707 iface to cleanup
    -------------------------------------------------------------------*/
    ds707_rmsmi_transition_state( DS707_RMSM_WAITING_FOR_UM_DOWN_STATE );
  }
}

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*===========================================================================
FUNCTION DS707_RMSM_IS_NETMDL_CALL_UP() 

DESCRIPTION
  This function returns whether a network model call is up or not.

PARAMETERS
  None

RETURN VALUE
   TRUE - if network model call is up
   FALSE - if no network model call

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean ds707_rmsm_is_netmdl_call_up(void)
{
  if (ds707_rmsmi_info.state == DS707_RMSM_E2E_PPP_UP_SIP_STATE)
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

/*===========================================================================
FUNCTION PS_IFACE_DOWN_1X_V4_IND()

DESCRIPTION
Posting a down indication on the IPv4 1x interface


PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  down_reason   : Call end reason to be given in IFACE DOWN indication 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_down_1x_v4_ind
(
  ps_iface_type                   *iface4_ptr,
  ps_iface_net_down_reason_type    down_reason
)
{
  ps_iface_down_mh_event_info_type      iface_down_ind_info;
#if defined(FEATURE_DATA_PS_IWLAN_3GPP2)
  /*---------------------------------------------------------------------
    Decrement the number of IWLAN+3GPP2 interface used count.
  ---------------------------------------------------------------------*/
  if ( num_used_3gpp2_v4_ifaces > 0 )
  {
    num_used_3gpp2_v4_ifaces--;
  }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 || FEATURE_UW_FMC */
  memset(&iface_down_ind_info, 0, sizeof(iface_down_ind_info));
  ds707_pkt_mgr_get_bearer_tech_info_ex( &iface_down_ind_info.bearer_tech);
  iface_down_ind_info.down_reason = down_reason;
  ps_iface_down_ind(iface4_ptr, &iface_down_ind_info);
  return;
} /* PS_IFACE_DOWN_1X_V4_IND */

/*===========================================================================
FUNCTION DS707_RMSM_GET_CURRENT_STATE()

DESCRIPTION
  This function  returns the current state of the RMSM. If RMSM is not 
  initialized, then default state is returned.

PARAMETERS
  None

RETURN VALUE
   Returns RMSM state (integer).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds707_rmsm_get_state(void)
{
  return (int32)ds707_rmsmi_info.state;
}

/*===========================================================================
FUNCTION DS707_IS_RMSM_IN_SIP_STATE()

DESCRIPTION
  This function returns TRUE if RMSM is in one of the SIP states

PARAMETERS
  None

RETURN VALUE
   Returns TRUE if RMSM is in one of the SIP states

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_is_rmsm_in_SIP_state(void)
{
  if(DS707_RMSM_WAITING_FOR_UM_UP_SIP_STATE == ds707_rmsmi_info.state || 
     DS707_RMSM_WAITING_FOR_E2E_PPP_UP_SIP_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_E2E_PPP_UP_SIP_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_RESYNCING_UM_SIP_STATE == ds707_rmsmi_info.state
    )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* DS707_IS_RMSM_IN_SIP_STATE */

/*===========================================================================
FUNCTION DS707_IS_RMSM_IN_NON_MIP_SIP_STATE()

DESCRIPTION
  This function returns TRUE if RMSM is in any state other than MIP/SIP states

PARAMETERS
  None

RETURN VALUE
   Returns TRUE if RMSM is in any state other than MIP/SIP states

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_is_rmsm_in_non_MIP_SIP_state(void)
{
  if(DS707_RMSM_NULL_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_WAITING_FOR_UM_UP_RELAY_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_UM_RM_UP_RELAY_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_WAITING_FOR_UM_DOWN_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_WAITING_FOR_RM_DOWN_STATE == ds707_rmsmi_info.state ||
     DS707_RMSM_SIP_WAITING_FOR_PHYS_LINK_STABLE_STATE == ds707_rmsmi_info.state
    )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* DS707_IS_RMSM_IN_NON_MIP_SIP_STATE */

/*===========================================================================
FUNCTION DS707_RMSM_GET_QCMIP_VAL

DESCRIPTION
  This function returns the qcmip val.

PARAMETERS
  None

RETURN VALUE
   UINT32 : qcmip value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds707_rmsm_get_qcmip_val(void)
{
#ifdef FEATURE_DS_MOBILE_IP
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  return (uint32)ds_1x_runtime_profile_ptr->mip.ds_qcmip_val;
#else
  return 0;
#endif /* FEATURE_DS_MOBILE_IP */
}

/*===========================================================================
FUNCTION DS707_RMSM_UPDATE_WMK_VAULE()

DESCRIPTION
  This function updates the RM FWD/REV link Watermark value based on 
  technology info getting from HDR 
  
PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_update_wmk_value
(
  void
)
{
#ifdef FEATURE_DATA_WMK_CONFIG
  int ret_val;
  /* Structure to hold the watermark values that are read from NV */
  ds707_wmk_lvl_nv_struct wmk_nv_data; 
  memset( &wmk_nv_data, 0, sizeof(wmk_nv_data) );
#endif

#ifdef FEATURE_HDR_REVB   
  if ( hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  { 
    DATA_IS707_MSG0( MSG_LEGACY_MED, "RevB WaterMark used" );
    /* REVB protocol */
    ds707_rmsm_wmk_value.ds707_rmsm_fwd_lo_wmk          = DS707_RMSM_REVB_FWD_LO_WATERMARK;
    ds707_rmsm_wmk_value.ds707_rmsm_fwd_hi_wmk          = DS707_RMSM_REVB_FWD_HI_WATERMARK;
    ds707_rmsm_wmk_value.ds707_rmsm_fwd_dont_exceed_cnt = 0xFFFFFFFF;
    ds707_rmsm_wmk_value.ds707_rmsm_rev_lo_wmk          = ds707_rmsm_revb_rev_lo_wmk;
    ds707_rmsm_wmk_value.ds707_rmsm_rev_hi_wmk          = ds707_rmsm_revb_rev_hi_wmk;
    ds707_rmsm_wmk_value.ds707_rmsm_rev_dont_exceed_cnt = ds707_rmsm_revb_rev_dont_exceed_cnt;  
  }
  else
  {
#endif
    DATA_IS707_MSG0( MSG_LEGACY_MED, "RevA WaterMark used");
    /* REVA/REL0 protocol */
    ds707_rmsm_wmk_value.ds707_rmsm_fwd_lo_wmk          = DS707_RMSM_REVA_FWD_LO_WATERMARK;
    ds707_rmsm_wmk_value.ds707_rmsm_fwd_hi_wmk          = DS707_RMSM_REVA_FWD_HI_WATERMARK;
    ds707_rmsm_wmk_value.ds707_rmsm_fwd_dont_exceed_cnt = DS707_RMSM_REVA_FWD_DONT_EXCEED_CNT;
    ds707_rmsm_wmk_value.ds707_rmsm_rev_lo_wmk          = ds707_rmsm_reva_rev_lo_wmk;
    ds707_rmsm_wmk_value.ds707_rmsm_rev_hi_wmk          = ds707_rmsm_reva_rev_hi_wmk;
    ds707_rmsm_wmk_value.ds707_rmsm_rev_dont_exceed_cnt = ds707_rmsm_reva_rev_dont_exceed_cnt;
#ifdef FEATURE_HDR_REVB
  }
#endif

#ifdef FEATURE_DATA_WMK_CONFIG
   /* Use wmk level values from NV if present*/
  ret_val = ds707_wmk_lvl_read_wmks_nv(  &wmk_nv_data, sizeof(wmk_nv_data) );
  if ( 0 == ret_val ) /* Wmk nv is written */
  {

    /*----------------------------------------------------------------------- 
      The user might enter some invalid watermark values in the NV. 
      In that case we will use the default values.
    -------------------------------------------------------------------------*/
    /* Check validity of fwd wmk lvls*/ 
    if( (wmk_nv_data.rmsm_fwd_lo_watermark_level > 0) &&
        (wmk_nv_data.rmsm_fwd_hi_watermark_level > wmk_nv_data.rmsm_fwd_lo_watermark_level) &&
        (wmk_nv_data.rmsm_fwd_dne_watermark_level > wmk_nv_data.rmsm_fwd_hi_watermark_level) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, "Using fwd rm wmk level values from NV");
      ds707_rmsm_wmk_value.ds707_rmsm_fwd_lo_wmk          = wmk_nv_data.rmsm_fwd_lo_watermark_level;
      ds707_rmsm_wmk_value.ds707_rmsm_fwd_hi_wmk          = wmk_nv_data.rmsm_fwd_hi_watermark_level;
      ds707_rmsm_wmk_value.ds707_rmsm_fwd_dont_exceed_cnt = wmk_nv_data.rmsm_fwd_dne_watermark_level;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "rm fwd wmk values from NV invalid, using defaults" );
    }
    
    /* Check validity of rev wmk lvls*/
    if( (wmk_nv_data.rmsm_rev_lo_watermark_level > 0) &&
        (wmk_nv_data.rmsm_rev_hi_watermark_level > wmk_nv_data.rmsm_rev_lo_watermark_level) &&
        (wmk_nv_data.rmsm_rev_dne_watermark_level > wmk_nv_data.rmsm_rev_hi_watermark_level) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, "Using rev rm wmk level values from NV");
      ds707_rmsm_wmk_value.ds707_rmsm_rev_lo_wmk          = wmk_nv_data.rmsm_rev_lo_watermark_level;
      ds707_rmsm_wmk_value.ds707_rmsm_rev_hi_wmk          = wmk_nv_data.rmsm_rev_hi_watermark_level;
      ds707_rmsm_wmk_value.ds707_rmsm_rev_dont_exceed_cnt = wmk_nv_data.rmsm_rev_dne_watermark_level;     
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "rm rev wmk values from NV invalid, using defaults" );
    }    
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "RM wmk values not present in NV, using defaults" );
  }
#endif /*FEATURE_DATA_WMK_CONFIG*/

  DATA_IS707_MSG6(MSG_LEGACY_HIGH, 
                  "rmsm fwd wmk lvl: lo %d, hi %d, dne %d. "
                  "rmsm rev wmk lvl: lo %d, hi %d, dne %d",
                  ds707_rmsm_wmk_value.ds707_rmsm_fwd_lo_wmk,
                  ds707_rmsm_wmk_value.ds707_rmsm_fwd_hi_wmk,
                  ds707_rmsm_wmk_value.ds707_rmsm_fwd_dont_exceed_cnt,
                  ds707_rmsm_wmk_value.ds707_rmsm_rev_lo_wmk,
                  ds707_rmsm_wmk_value.ds707_rmsm_rev_hi_wmk,
                  ds707_rmsm_wmk_value.ds707_rmsm_rev_dont_exceed_cnt);

  ds707_rmsmi_info.sio_fwd_wmk.lo_watermark    = ds707_rmsm_wmk_value.ds707_rmsm_fwd_lo_wmk;
  ds707_rmsmi_info.sio_fwd_wmk.hi_watermark    = ds707_rmsm_wmk_value.ds707_rmsm_fwd_hi_wmk;
  ds707_rmsmi_info.sio_fwd_wmk.dont_exceed_cnt = ds707_rmsm_wmk_value.ds707_rmsm_fwd_dont_exceed_cnt;
  ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A].lo_watermark    = ds707_rmsm_wmk_value.ds707_rmsm_rev_lo_wmk;
  ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A].hi_watermark    = ds707_rmsm_wmk_value.ds707_rmsm_rev_hi_wmk;
  ds707_rmsmi_info.sio_rev_wmk_list[DSRSP_ROUTE_A].dont_exceed_cnt = ds707_rmsm_wmk_value.ds707_rmsm_rev_dont_exceed_cnt;
}

#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
FUNCTION ds707_rmsmi_mip_config_after_bringup

DESCRIPTION
  Configures MIP call

PARAMETERS
  None
  

RETURN VALUE
  boolean: TRUE if configuration was succesful, else FALSE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds707_rmsmi_mip_config_after_bringup(void)
{
  int8    mip_index           = -1;
  ds707_mip_profile_type       *ds_1x_runtime_profile_ptr = NULL;
  ds707_data_session_profile_info_type *pdn_prof_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  if ( ds_1x_runtime_profile_ptr->mip.ds_qcmip_val > 0 )
  {
#ifdef FEATURE_DS_MULTIPLE_NAI
    /*-----------------------------------------------------------------------
      For laptop MIP calls NAI is different from sockets calls. Register 
      for mip_config_session callbacks to be called at the completion
      of MIP config session so that the NAIs may be updated. Multiple NAIs 
      are applicable only for NV mode and so check if phone is in NV mode 
      before registering callback.
    -----------------------------------------------------------------------*/
    if(uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM || 
       uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)
    {
      mip_reg_ses_info_cb( ds707_rmsm_get_mip_session_info_cb );
    }
#endif /* FEATURE_DS_MULTIPLE_NAI */

    /*-------------------------------------------------------------------
      From iface, get the requesting profile id. Get the mip index from 
      profile which should be used to read MIP parameters from NV
    -------------------------------------------------------------------*/
    ds707_data_session_get_iface_requesting_profile( 
                                    ds707_rmsmi_info.IS707pkt_iface_ptr,
                                    &pdn_prof_ptr);

    /*---------------------------------------------------------------------
      If profile id specified for call bring up is MIP profile, get the 
      index to be used to read MIP parameters from NV. If the profile id 
      specified is not a MIP profile, then read the MIP profile index from 
      NV item NV_DS_MIP_ACTIVE_PROF_I.
    ---------------------------------------------------------------------*/
    if( (pdn_prof_ptr != NULL) &&
        PROFILE_ID_IS_MIP(pdn_prof_ptr->data_session_profile_id) )
    {
      mip_index = ds707_get_mip_index_from_profile_id(
                    pdn_prof_ptr->data_session_profile_id);
    }
    else
    {
      mip_index = mip_get_active_profile_index_from_nvruim();
      ds707_set_mip_profile_index( mip_index );
    }

    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                     "In ds707_rmsmi_mip_config_after_bringup, mip_index = %d",
                     mip_index);

    if( ((mip_config_session(mip_index)) == FALSE) &&
        (ds_1x_runtime_profile_ptr->mip.ds_qcmip_val == 2) ) 
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
              "mip_config_session config Failed in MIP only mode, ret FALSE");
      return FALSE;
    }
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "mip session config passed - returning TRUE");

  return TRUE;

}
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
FUNCTION ds707_rmsmi_reg_ev_netmodel_after_bringup

DESCRIPTION
  Register for netmodel events

PARAMETERS
  ps_iface_type: ps iface pointer
  

RETURN VALUE
  boolean: TRUE if registration is succesful, else FALSE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds707_rmsmi_reg_ev_netmodel_after_bringup
(
  ps_iface_type* iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(iface_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "iface ptr is NULL" );
    return FALSE;
  }

  if( (ps_iface_event_cback_reg( iface_ptr,
                                 IFACE_DOWN_EV,
                                 ds707_rmsmi_info.um_iface_down_buf_ptr)) < 0 )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "IFACE_DOWN_EV reg failed - so returning FALSE" );
    return FALSE;
  }
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "Regd for IFACE_DOWN_EV for iface: 0x%x::%d. "
                  "RMSM setting abort cback",
                  iface_ptr->name, iface_ptr->instance);

  /*-------------------------------------------------------------------
    We Register for flow disbale for relay model calls only. For netmodel
    calls, PS stops pulling data off the queue if flow is disabled.          
    Register the phys link gone cback.
  ---------------------------------------------------------------------*/
  (void) ps_iface_event_cback_reg(iface_ptr,
                                  IFACE_PHYS_LINK_GONE_EV,
                                  ds707_rmsmi_info.um_phys_link_gone_buf_ptr);

  if(ps_iface_event_cback_reg( iface_ptr,
                               IFACE_FLOW_ENABLED_EV,
                               ds707_rmsmi_info.flow_enable_buf_ptr
                             ) < 0 )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "IFACE FLOW_ENABLED_EV reg failed" );
    /* CALL FAILED */
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Register for the flow enabled phys_link events.
  -------------------------------------------------------------------*/
  if(ps_phys_link_event_cback_reg(
                      PS_IFACE_GET_PHYS_LINK(iface_ptr),
                      PHYS_LINK_FLOW_ENABLED_EV,
                      ds707_rmsmi_info.um_phys_link_flow_enable_buf_ptr) < 0 )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Phys_link FLOW_ENABLED_EV reg failed");
    /* CALL FAILED */
    return FALSE;
  }


  /*-------------------------------------------------------------------
    Register for the flow_flow enabled events.
  -------------------------------------------------------------------*/
  if(ps_flow_event_cback_reg( 
      PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
      FLOW_TX_ENABLED_EV,
      ds707_rmsmi_info.um_flow_tx_enable_buf_ptr) < 0 )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "FLOW_TX_ENABLED_EV reg failed" );
    /* CALL FAILED */

    return FALSE;
  }

    /*-------------------------------------------------------------------
    Register for the flow_flow disabled events.
  -------------------------------------------------------------------*/
  if(ps_flow_event_cback_reg( 
      PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
      FLOW_TX_DISABLED_EV,
      ds707_rmsmi_info.um_flow_tx_disable_buf_ptr) < 0 )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "FLOW_TX_DISABLED_EV reg failed");
    /* CALL FAILED */
    return FALSE;
  }

  if ( ps_iface_event_cback_reg( iface_ptr,
                                 IFACE_UP_EV,
                                 ds707_rmsmi_info.um_iface_up_buf_ptr 
                               ) < 0 )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "IFACE_UP_EV reg failed" );

    return FALSE;
  }

    /*---------------------------------------------------------------------
    Register for IFACE configuring event with the Um iface to handle 
    PPP resync scnenario
  ---------------------------------------------------------------------*/
  if ( ps_iface_event_cback_reg( iface_ptr,
                                 IFACE_CONFIGURING_EV,
                                 ds707_rmsmi_info.um_iface_configuring_buf_ptr 
                                 ) < 0 )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "IFACE_CONFIGURING_EV reg failed" );

    return FALSE;
  }

  /*---------------------------------------------------------------------
    This is required for correct SIP falback - we donot want to 
    fallback to SIP if we are not able to bring the TC IP.
  ---------------------------------------------------------------------*/
  if ( ps_iface_event_cback_reg( iface_ptr,
                                 IFACE_PHYS_LINK_UP_EV,
                                 ds707_rmsmi_info.um_phys_link_up_buf_ptr
                                 ) < 0 )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "PHYS_LINK_UP_EV reg failed" );

    return FALSE;
  }

  ds707_rmsmi_info.pkt_info.iface_ptr = iface_ptr;

  if (ds707_data_session_get_oldest_active_iface_ptr() == NULL) 
  {
    ds707_data_session_set_oldest_active_iface_ptr(iface_ptr);
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_rmsmi_reg_ev_netmodel_after_bringup() retuning TRUE");

  return TRUE;
}


/*===========================================================================
FUNCTION DS707_RMSMI_BRINGUP_IFACE_BY_POLICY

DESCRIPTION
  Brings up iface by policy iface

PARAMETERS
  client_data_ptr: client data required for bringup
  curr_state: current state of rmsm

RETURN VALUE
  ds707_rmsmi_bringup_errno_type: errnumber for failure case

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds707_rmsmi_bringup_errno_type ds707_rmsmi_bring_up_iface_by_policy(
  void*                  client_data_ptr,
  ds707_rmsmi_state_type curr_state)
{

  ps_iface_type*        temp_iface_ptr = NULL;
  acl_policy_info_type  acl_policy_info;
  int16                 ps_iface_errno = DS_EINVAL;
  ds707_rmsmi_bringup_errno_type  rval = DS707_RMSM_BRINGUP_UM_INVALID;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP)
  ps_iface_type* v6_iface_ptr = NULL;
#endif /* FEATURE_EHRPD && FEATURE_DATA_EHRPD_DUAL_IP */
  ps_iface_net_down_reason_type net_down_reason;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                  "Bringup not done yet, so setting brought up flag to FALSE");
  rmsmi_IS707pkt_iface_brought_up = FALSE;

 /* Cache client data for use in arbitration */
  ds707_rmsmi_info.client_data_ptr = client_data_ptr;
  if( ( (ds707_rmsmi_build_acl_policy(&acl_policy_info, FALSE, TRUE)) == TRUE ) &&
      ( (ps_route_bring_up_iface_by_policy_ex(&acl_policy_info,
                                              &temp_iface_ptr,
                                              &ps_iface_errno,
                                              client_data_ptr,
                                              &net_down_reason)) < 0 )
    ) 
  {
    if(temp_iface_ptr == NULL)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "ps_route_bring_up_iface_by_policy returned NULL iface");

      return DS707_RMSM_BRINGUP_UM_FAIL;
    }

    DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                     "Tried Bringup got: iface: 0x%x:%d, err code: %d",
                     temp_iface_ptr->name,
                     temp_iface_ptr->instance,
                     ps_iface_errno);

    /*-------------------------------------------------------------------
    Set iface ptr in pkt call info. Also if oldest_active_iface_ptr is
    NULL then set it with this iface ptr.
    -------------------------------------------------------------------*/
    ds707_rmsmi_info.IS707pkt_iface_ptr = temp_iface_ptr;

    if(ps_iface_errno == DS_ENETCLOSEINPROGRESS)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Setting re-establish flag to TRUE - "
                       "PS Arb will teardown UM iface");

      /* Register for UM IFACE down event*/

      if( (ps_iface_event_cback_reg(temp_iface_ptr,
                                    IFACE_DOWN_EV,
                                    ds707_rmsmi_info.um_iface_down_buf_ptr)) < 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "IFACE_DOWN_EV reg failed. Disallowing the laptop call");
        rval = DS707_RMSM_BRINGUP_UM_FAIL;
      }
      else
      {
        rval = DS707_RMSM_BRINGUP_UM_WAITING_ARBITRATION;  
      }
      
    }
    else if(ps_iface_errno == DS_EWOULDBLOCK)
    {
      /* record that iface brought up */
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Bringup Success, so Setting "
                       "brought up flag to TRUE, reestablish to FALSE");

      rmsmi_IS707pkt_iface_brought_up = TRUE;

      rmsmi_reestablish_um_link = FALSE;

      if( ( ds707_rmsmi_reg_ev_netmodel_after_bringup(temp_iface_ptr) == FALSE)
#ifdef FEATURE_DS_MOBILE_IP
          ||
          ( (curr_state == DS707_RMSM_WAITING_FOR_PHYS_LINK_UP_STATE) &&
            ( ds707_rmsmi_mip_config_after_bringup() == FALSE) )
#endif /* ifdef FEATURE_DS_MOBILE_IP */
        )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Tearing down up iface - "
                         "because Event Reg (+ MIP) config failed");
        /* Note: temp iface and rmsm's um iface are same now... */
        
        if( (ps_iface_go_null_cmd(temp_iface_ptr, 
                                  &ps_iface_errno,
                                  client_data_ptr)) == 0)
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "Iface tear down failed, so returning FAIL ");

          rval = DS707_RMSM_BRINGUP_UM_FAIL;
        }
        else
        {
          rval = DS707_RMSM_BRINGUP_UM_SUCCESS_CLOSE_IN_PROGRESS;
        }

      }
      else /* config successful */
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                "After bringup: NetModel Event Reg (+ MIP) Config Successful");

        ds707_rmsmi_info.IS707pkt_iface_ptr = temp_iface_ptr;

        rval = DS707_RMSM_BRINGUP_UM_SUCCESS;

#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP)
        pdncntx_cb_ptr = 
               ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(temp_iface_ptr);
        v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

        if( (!PS_IFACE_IS_IN_USE(v6_iface_ptr)) &&
            (ps_iface_state(v6_iface_ptr) == IFACE_DOWN) )
        {
          /* Free V6 iface since DUN uses only V4 */
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, "DUN call, free V6 iface" );
          ds707_pdncntx_free_v6_iface(pdncntx_cb_ptr);
        }
#endif /* FEATURE_EHRPD && FEATURE_DATA_EHRPD_DUAL_IP */
      }
      
    }
    else /* (ps_iface_errno != DS_EWOULDBLOCK) */
    {
      /*-----------------------------------------------------------------
        Bringing up failed
      -----------------------------------------------------------------*/
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "UM iface bringup failed, with invalid Error code: %d",
                       ps_iface_errno);

      rval = DS707_RMSM_BRINGUP_UM_FAIL;
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Unhandled response by ps_route_bring_up_iface_by_policy");

    rval = DS707_RMSM_BRINGUP_UM_FAIL;
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707_rmsmi_bring_up_iface_by_policy returning: %d",
                   rval);

  return rval;
}

/*===========================================================================
FUNCTION DS707_RMSMI_BRINGUP_IFACE

DESCRIPTION
  Brings up the iface

PARAMETERS
  client_data_ptr: client data required for bringup

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds707_rmsmi_bringup_iface(void* client_data_ptr)
{
	boolean bRet = TRUE;
	ds707_rmsmi_bringup_errno_type bringup_err = ds707_rmsmi_bring_up_iface_by_policy(client_data_ptr,
                                                           ds707_rmsmi_info.state);

	DATA_IS707_MSG3( MSG_LEGACY_HIGH, "In UM_UP_SIP STATE - After Bringup, "
                   "brought_up: %d, reestablish: %d, bringup returned: %d", 
                   rmsmi_IS707pkt_iface_brought_up,
                   rmsmi_reestablish_um_link,
                   bringup_err);

	if(bringup_err == DS707_RMSM_BRINGUP_UM_FAIL)
	{
	  /* Failed to bring_up_iface_by_policy */
	  ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
	}
	else if(bringup_err == DS707_RMSM_BRINGUP_UM_SUCCESS)
	{
	  /* Success bring_up_iface_by_policy */
	  /* stay in same state - go to end of switch */
	}
	else if(bringup_err == DS707_RMSM_BRINGUP_UM_SUCCESS_CLOSE_IN_PROGRESS)
	{
	  /* Success bring_up_iface_by_policy */
	  /* stay in same state - go to end of switch */
	}
	else if(bringup_err == DS707_RMSM_BRINGUP_UM_WAITING_ARBITRATION)
	{
	  /* Waiting for arbitration */
    ds707_rmsmi_info.state_before_arbitration = ds707_rmsmi_info.state;
	  ds707_rmsmi_transition_state(DS707_RMSM_WAITING_FOR_UM_ARBITRATION);
	}
	else
	{
	  /* Unexpected response by bring_up_iface_by_policy */
	  ds707_rmsmi_transition_state(DS707_RMSM_NULL_STATE);
	}
	return bRet;
}


/*===========================================================================
FUNCTION DS707_RMSMI_DEREG_EV

DESCRIPTION
  This function deregisters events

PARAMETERS
  iface_ptr: ps_iface pointer

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds707_rmsmi_dereg_ev(ps_iface_type* iface_ptr)
{
  if(iface_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "iface_ptr is NULL");
  }

  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_DOWN_EV,
                              ds707_rmsmi_info.um_iface_down_buf_ptr );

  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_PHYS_LINK_UP_EV,
                              ds707_rmsmi_info.um_phys_link_up_buf_ptr );

  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_CONFIGURING_EV,
                              ds707_rmsmi_info.um_iface_configuring_buf_ptr );

  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_FLOW_ENABLED_EV,
                              ds707_rmsmi_info.flow_enable_buf_ptr);

  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_PHYS_LINK_GONE_EV,
                              ds707_rmsmi_info.um_phys_link_gone_buf_ptr );

  ps_phys_link_event_cback_dereg(PS_IFACE_GET_PHYS_LINK(iface_ptr ),
                                 PHYS_LINK_FLOW_ENABLED_EV,
                                 ds707_rmsmi_info.um_phys_link_flow_enable_buf_ptr);

  ps_flow_event_cback_dereg(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                            FLOW_TX_ENABLED_EV,
                            ds707_rmsmi_info.um_flow_tx_enable_buf_ptr);

  ps_flow_event_cback_dereg(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                            FLOW_TX_DISABLED_EV,
                            ds707_rmsmi_info.um_flow_tx_disable_buf_ptr);

  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_UP_EV,
                              ds707_rmsmi_info.um_iface_up_buf_ptr);

  return TRUE;
}

/*===========================================================================
FUNCTION DS707_RMSM_GET_WMK_VAL

DESCRIPTION
  This function returns watermarks low and high levele value

PARAMETERS
  None

RETURN VALUE
   None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_get_wmk_val
(
  ds707_rmsm_wmk_type *rmsm_wmk_type_ptr
)
{
  if ( rmsm_wmk_type_ptr )
  {
    *rmsm_wmk_type_ptr = ds707_rmsm_wmk_value;
  }
}

/*====================================================================
  FUNCTION DS707_RMSMI_SIO_CLEANUP

  DESCRIPTION
    This is used to perform SIO port related cleanup before call tear down

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
LOCAL void ds707_rmsmi_sio_cleanup ( void )
{
  (void)ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
  dsat_send_result_code( DSAT_NO_CARRIER );
  ds3g_siolib_set_call_coming_up( FALSE );
  (void)ds3g_siolib_deregister_callback_func(&ds707_rmsmi_sio_func_table );
  ds3g_siolib_release_port ( ds707_rmsmi_sio_port_allocated );
  ds707_rmsmi_sio_port_allocated = DS3G_SIOLIB_PORTID_NONE;
}

/*====================================================================
  FUNCTION DS707_RMSMI_ENTITLEMENT_CHECK_COMPLETE

  DESCRIPTION
    This API is called by client once the entitlement check is done. 
    The result of this check would determine whether to bring up the call 
    or not.

  PARAMETERS
    cb_data:     (IN) call back data, relevant for 3GPP
    allow_call:  (IN) TRUE if entitlement check was successful
                      FALSE otherwise
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
LOCAL void ds707_rmsmi_entitlement_check_complete
( 
  unsigned char            cb_data,  
  boolean                  allow_call
)
{
  
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                  "ds707_rmsmi_entitlement_check_complete  allow call = %d",
                  allow_call);
  /*---------------------------------------------------------------
   Only if we are waiting on entitlement check result, proceed. 
   Return Otherwise
  -------------------------------------------------------------*/
  if( ds707_rmsm_get_state ()!= 
                    DS707_RMSM_WAITING_FOR_ENTITLEMENT_CHECK_STATE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Invalid state to receive Entitlement Check Complete");
    return;
  }
  
  /*------------------------------------------------------------
    If Entitlement check resulted in Failure, terminate call.
  -------------------------------------------------------------*/
  if( allow_call == FALSE )
  {
    ds707_rmsmi_transition_state( DS707_RMSM_NULL_STATE );
    return;
  }

  /*-------------------------------------------------------------------
    Here we can safely assume Entitlement check resulted in Success
  ------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "Entitlement check success. Proceed with call origination");
  ds707_rmsm_post_event( DS707_RMSM_RM_WANTS_PKT_CALL_EV );
  return;
}

#endif /* FEATURE_DATA_IS707 */
