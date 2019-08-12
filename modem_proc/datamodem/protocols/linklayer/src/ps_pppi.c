/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _ P P P I . C

GENERAL DESCRIPTION
  All of the internal PPP protocol suite functionality.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 1992-1994 William Allen Simpson
   Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_pppi.c_v   1.32   13 Feb 2003 20:13:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_pppi.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $


when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/18/14    skc    Removing usage of PS_RX_UW_FMC_SIGNAL 
10/17/14    sd     Fixed memory leak in pppi_start().
08/23/14    skc    Removing FEATURE_HDLC_HW_ACCEL, FEATURE_DATA_PS_HDLC_PP
06/06/14    vl     BSS optimization changes
05/26/14    vl     Added support for resync of exisitng VSNCPs in 
                   pppi_resync() function  
12/03/13    cx     Reset the snooped_bytes when process new DSM item
11/12/13    cx     Fixed memory leak in pppi_receive()
11/03/13    cx     Added support for second rm rx device
10/16/13    cx     Adding leaving critical section when slhc_decompress fails
06/20/13    ss     Adding critical section protection to SLHC API invocation.
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
04/29/13    cx     Remove debug msg 
01/21/13    ds     Fixing of KW protocols errors.
11/27/12    ash    Serialize VSNCP access in pppi_outgoing (in PS_RM Task) 
                   with VSNCP cleanup in PS task.
11/22/12    ssb    Fixed KW Error
10/25/12    fn     Remove usage of fatal error log messages
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
12/08/11    dvk    Merged PPP auth fail reason in DSSNET4 SM control block and 
                   overwrote PPP fail reason when handling PPP down event only 
                   if control block has PPP fail reason as NONE.
12/07/11    vpk    Add support for 5 PDNs
11/28/11    jz     ppp control packet on dos(FEATURE_DATA_SEND_PPP_CTL_ON_DOS) 
10/17/11    ssh    eHRPD Always On support
09/27/11    op     Compiler warning fixes
09/20/11    jee    Fix to handle new call when device manager is cleaning up
09/14/11    ash    Cancel PPP setup timer after AUTH success
08/11/11    sm     FEATURE_DATA_PS_TCP_ACK_PRIO, tagged tcp ack packet as 
                   DSM_HIGHEST.
06/24/11    vpk    initialize EAP_AKA, makes usage faster
06/06/11    ash    Added support for PPP Partial Context
05/16/11    ssh    Return failure from apply cached opts if device not inited.
                   Protect writes to device cache and inited flag.
03/04/11    jee    Fixed NCP cleanup issue 
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
12/08/10    rp     RAM optimization changes.
09/20/10    ssh    Increased PPP event buf counts
08/18/10    jee    Compiler warning fix merged from 9K 2.0
06/29/10    jee    Fixed compiler warnings
05/13/10    ssh    Added support for PDN Inactivity timer
4/19/2010   ash    Added changes to support Dual IP Feature.
10/30/09    jee    Fixed compiler warnings
10/01/09    mga    Changed pppi_abort to ensure device configuration is reset
09/29/09    jee    Fixed klocwork errors
09/25/09    mga    Fix in pppi_set_mode to use the pending_mode appropriately
09/21/09    mga    Merged from eHRPD branch
09/08/09    jee    Removed the FEATURE_DS_DEBUG_UI feature
07/27/09    sp     Fixed IP Fragmentation issues.
07/14/09    pp     ppp_cb_lock changes.
06/16/09    pp     Fixed a crash from PPP setup timeout callback.
06/09/09    mga    Merged Compiler warnings fixes
05/07/09    pp     CMI Phase-4: SU Level API Effort.
05/06/09    ss     SLHC De-coupling effort.
03/30/09    scb    Fixed high LINT errors for MoB builds
02/26/08    scb    Undid freeing the framed_data_ptr when in UNFRAMING state.
12/19/08    scb    Added fix to ensure that s/w to h/w HDLC mode change
                   happens only on demand.
12/04/08    scb    Added PPP configuration parameters logging capability
11/04/08    scb    Added fix to ensure that pseudonet_mode_hdlr handles
                   split packets in the framed byte stream correctly.
11/12/08    scb    Added protocol processor specific functionality.
09/12/08    pp     Metainfo optimizations.
09/02/08    rnp    Added STA related changes.
07/01/08    dm     Fixed issue of packets getting dropped and long data
                   sessions observed on PLTS due to VJ TCP compression
05/27/08    rnp    Added additional functionality reqd. for IPCP STA tests.
05/21/08    scb    High LINT error fixes
05/04/08    scb    PPP STA development
04/11/08    scb    Klocwork fixes
01/28/07    scb    Removed the Klocwork fix added in pppi_discard()
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/19/07    scb    Added protocol processor related functionality to
                   initialize HDLC config_ptr and also to add a new parameter to
                   the existing frame and unframe API.
05/12/07    scb    Added fix to perform DPL logging of PPP frames only if no
                   error occured during framing in pppi_outgoing().
09/27/07    ifk    Removed code for old PPP statistics.
07/16/07    scb    Removed RUIM support for CHAP authentication for 7500
07/11/07    scb    Added fix to drop a PPP packet with bad CRC.
05/20/07    scb    Added RUIM support for CHAP authentication in 7500
06/12/07    vp     Call to MIP_DATA_TRANSFER_ACTION moved to mode handler
05/22/07    scb    Fixed bug in event callback mechanism in pppi_abort()
04/27/07    scb    Removed LINT suppressions
04/20/07    scb    Removed NULL pointer check in FINDING_SOP state and added
                   NULL pointer check in SNOOPING state in
                   pppi_pseudonet_mode_hdlr()
04/19/07    scb    Added a KDDI PPP compliance requirement of sending
                   a TERM request if the PPP setup timer expires.
03/13/07    ssh    Frame *all* LCP packets using default accm (0xffffffff)
02/08/07    scb    Fixed Klocwork high errors
12/15/06    ssh    Fixed bug in event callback mechanism
11/03/06    msr    Sending meta info along with VJCH packets to iface_input
11/02/06    ifk    Added additional argument to rohc_decompress.
10/06/06    ssh    Eliminated redundant current_dsm ptr from
                   pseudonet_mode_hdlr; was causing DSM issues.
09/19/06    ssh    Set ACCM in pppi_dev_init() irrespective of config_ptr
08/18/06    mjp    Fixed IPHC and ROHC Logging
08/04/06    ssh    Now using featurized critical sections for event cbacks
08/31/06    mjp    Added IPHC and ROHC Logging
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link and added DPL PPP Partial Logging
06/07/06    ssh    Now allocating HDLC instance during pppi_powerup_init()
04/28/06    ifk    Added PPP critical sections.
04/20/06    ssh    Changes to make PPP use HDLC library exclusively
                   for software/hardware framing/unframing. Refactored the
                   PPP pseudonet mode state machine.
04/04/06    jd     Simplified pppi_outgoing header generation logic.
                   Removed PPP unframed logging from DPL and legacy PPP
02/06/06    ssh    Changed TASKLOCKs to critical sections (for L4 builds).
02/02/06    ifk    Fixed compile error for IPv6.
12/09/05    sv     Added support for new data path framework.
11/29/05    jd     Call PPP logging macro wrapper to reduce CPU loading
11/29/05    ssh    Fixed cleanup in pppi_abort(), moved  protocol mode
                   setting to PS context
11/29/05    ssh    Logging incoming packet before discarding in
                   pppi_get_data_cback()
11/28/05  ifk/ssh  Added support for ROHC and IPHC over PPP under
                   FEATURE_DATA_PS_PPP_ROHC and FEATURE_DATA_PS_PPP_IPHC.
11/23/05    ssh    Fixed HDLC hardware framer to use DSM items better
11/22/05    ssh    Now invoking ps_iface_input() via iface_config func ptr
11/05/05    rt     Fixed coverity errors.
10/11/05    ssh    Fixed compilation error due to multi-line expression
10/11/05    ssh    Fixed debug message in pppi_report_event_to_diag()
10/07/05    ssh    Changes due to HDLC driver redesign
09/03/05    ifk    Skip empty DSM items in HW hdlc unframing.
09/01/05    ssh    Fixed HDLC framer/unframer to use only small dsm items
08/16/05    ks     Merged Kavitha's netmodel changes from tip.
08/16/05    ssh    Fixed compilation error with FEATURE_HDLC_HW_ACCEL ON
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    kr     Removed FEATURE DS_PSTATS and added stats processing
08/11/05    ssh    Added additional checks in pppi_set_mode()
08/08/05    ssh    Fixed typo in pppi_call_event_callback()
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
06/29/05    ssh    Fixed PPP PFC and ACFC processing in pppi_incoming()
05/13/05    ks     merged irfan's fix from tip to dev04.
04/15/05    ifk    Free chap challenge dsm item in ppp_abort
04/11/05    ssh    Using session_handle from configbuf_ptr in pppi_start()
03/22/05    ssh    Added new function pppi_report_event_to_diag(), added
                   posting of resync event to diag, made some formatting
                   changes.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
03/14/05    ks     Corrected PPP logging in pseudo net mode
02/10/05    ifk    Took out HDR auth registered callback function.  Using PPP
                   auth events now.
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
01/26/05    ifk    For new HDLC HW added workaround to allow switching of
                   framing between SW and HW framer under
                   FEATURE_HDLC_SW_WORKAROUND
01/25/05    ifk    Aggregate writing of CRC and end flag in HW framer with
                   payload.
01/21/05    ks     In pppi_hdlc_unframe_init, removed incorrect ASSRETs and
                   now setting tail_ptr and orig_tail_ptr to NULL.
01/12/05    ks     Fixed indentation and removed link_instance parameter from
                   DPL_LOG_LINK_FULL_TX_PACKET macro during merge.
01/10/05    ks     Fixed a compilation error caused in merge.
01/10/05    ks     Added support for PPP logging using Data Protocol Logging.
01/09/04    ks     Fixed PPP logging in psuedo network mode.
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
12/27/04    vp     Pass meta_info to ps_iface_input() in case of VJUCH in
                   pppi_receive().
11/17/04    ifk    VJ compress only if in internal mode in pppi_outgoing()
10/30/04    ks     Changed ptr to ptr of ps_meta_info_type to single pointer
                   in pppi_iface_tx_function() and pppi_outgoing().
10/18/04    ifk    In pppi_set_mode() check for bridge device if in psuedo
                   net or full net mode.
10/14/04    ifk    Pass meta info ** to snoop functions.
08/23/04    mct    Fixed v6 featurization error.
08/03/04    jk/jd  Fixed pppi_abort bug where generating DOWN_EV past end of
                   fsm array
08/17/04    mvl    Linger bit no longer considered when trying to determine
                   if a resync should be inclue LCP or just NCP requested.
                   Also no longer clearing the NCP mask in dev_init().
08/04/04    ifk    If an NCP being resynced and it is only NCP, resync LCP
07/31/04    ifk    In pppi_start() use fsm_open() to handle SM transitions.
07/28/04    ifk    Initialize ncp_start in pppi_resync() before use.
                   Removed extra check for v4/v6 packets in pppi_receive()
07/16/04    ifk    Split pppi_start() into pppi_start() and pppi_resync().
                   Added pppi_failover_on_resync()
07/01/04    ifk    Removed setting of dsm kind field for sending out a packet
                   instead passing this in the protocol field.
06/15/04    ifk    Change PPP event notification to new architectrue.  Added
                   function pppi_call_event_callback()
06/11/04    vp     Removed all the dependency on iface.h & if_cb (stats info)
05/03/04    sy     Handled the new rx_sig_f_ptr appropriately.
04/30/04    usb    Do not cleanup IP reassembly queue  when PP dev goes down.
                   Done automaticaly through timers.
04/30/04    usb    Removed func callback to get peer info,
                   can be retrieved directly from iface_cb.
04/30/04    ifk    Post snoop ip events only when an IPCP event occurs
04/17/04    ifk    Added use of remote_initiate field in fsm struct.  If LCP
                   is already up in pppi_start() then don't bring it up again.
                   Clear rx_iface_ptr and fsm mode in ppp_stop().
04/09/04    ifk    Added check to pppi_set_mode() to verify that the PPP
                   control block is setup with a valid bridge device.
04/08/04    ifk    Fixed pppi_start() to start LCP in passive mode as well.
04/05/04    sv/rc  Featurized v6 specific code under FEATURE_DATA_PS_IPV6
03/15/04    ifk    Added PPP configuration buffers, moved code changing ppp_cb
                   from ppp_start() and ppp_resync() to pppi_start().
03/12/04    aku    Pushdown meta_info into PPP packet only if *meta_info_ptr
                   is not NULL.
01/19/03    usb    Allow passive wake up on receiving any PPP packet from
                   the network if specified through PPP confoguration.
01/28/04    mvl    Will now only perform CHAP when in Network or Auth phase.
11/11/03   aku/rc   Added support for IPv6.
10/24/03    ifk    Added auth configurable parameter initialization to
                   pppi_powerup_init().
10/20/03    usb    Removed Keyword LOCAL before iface_cb definition.
10/08/03    usb    Added event posting for IP packet snooping.
09/25/03    ifk    Added IPv6CP code. Added IPv6 handling in pppi_receive()
09/18/03    jd     When starting PPP on Um, put netmdl in RESYNC metastate if
                   previously UP to ensure options are checked after resync.
09/16/03    usb    Fixed pppi_start() to ensure LCP packets get sent out
                   for mobile initiated resync.
08/13/03    ifk    Seperated IPCP and LCP negotiation in preparation for
                   adding IP6CP.
07/22/03    sv     Removed all references to DSM_COOKIE as that went away in
                   the new DSM release.
07/15/03    usb    Starting PPP setup timer (if specified) even if LCP is in
                   Passive mode.
06/24/03    ifk    Fixed minor casting issue where & was used on a variable
                   that wasn't an lvalue.
06/11/03    usb    Fixed various bugs in HDLC sw and hw unframing.
                   Discard an unframed PPP packet if its less than 3 bytes.
06/10/03    ss     Changed the code to correctly fwd packets to laptop in a
                   laptop call during MIP re-registrations, etc.
05/28/05    usb    Set rx_iface_ptr to NULL in pppi_abort to avoid
                   inconsistency if somebody calls ppp_start followed
                   by ppp_stop without getting a DOWN event.
04/18/04    vsk    Initialize auth.mode to PPP_NO_AUTH at powerup
                   removed call to fsm_passive_wakeup in pppi_incoming.
                   passive mode is handled in ppp_fsm when a cfg req comes in
04/18/03    vsk    Handle PPP_INTERNAL_SNOOP mode also in pppi_hdlc_sw_unframe
                   check for fsm.mode rather than fsm.passive_mode for passive
                   mode operation
04/11/03    kvd    Moved logging of ppp frames to inside if( ppp_pkt_size >0)
                   in ppp_hdlc_hw_unframe as otherwise it was logging fatal error
                   messages unnecessarily.
04/11/03    ak     Updates for new RMSM interfaces.
04/08/03    mvl    Added support for not moving to the network phase if auth
                   info not received.
04/07/03    usb    Split second half of pppi_incoming() (protocol
                   processig part) into pppi_receive().
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added PPP events reporting.
02/13/03    jd     Removed calls to ppp_log_frames() - no longer used
                   Fixed featurization and parameters to ppp_log_frames_ext
02/13/03    usb    Handling ret code SNOOP_CB_FORWARD_PKT from snoop cbacks.
02/06/03    mvl    Freeing packet when slhc_remember() fails and freeing meta
                   info in default TX function.
02/02/03    usb    Removed the use of variable cumulative_pkt_size from
                   pppi_hdlc_hw_unframe() - using dsm_pullup_tail() instead
                   to remove CRC from the end of the packet, processing new
                   enum return values from snoop_proto_msg_detect().
01/27/03    usb    PPP mode is always set in PS context, added
                   PPP_SET_MODE_CMD, added hw framer cleanup to
                   ppp_hdlc_unframe_init()
01/27/03    jd     Moved chap processing into process_chap_pkt()
12/10/02    usb    Moved setting PPP mode to NONE from ppp_abort to
                   pppi_abort (PS context), checking if iface is NULL or not
                   before processing an incoming PPP packet.
12/02/02    vr     Fixed potential HW framer buffer overflow. Removed setting
                   of unframe_mode (7E, SOP, etc.)when using hardware
                   framer/deframer.
11/26/02    usb    Setting app field either to NULL or to a valid
                   meta_info_ptr before calling tx_f_ptr().
11/25/02    js     Nullify pkt_tail_ptr and orig_tail_ptr whenever their
                   corresponding pkt_head_ptr and orig_head_ptr are released.
11/19/02    jd     Fixed typos for FEATURE_DS_PPP_LOGGING
11/19/02    mvl    Fixed include for PPP logging.
11/14/02    jd     Fixed check for non-MIP packet forwarding during rereg
11/12/02    mvl    checking if setup timer exists before starting it.  Added
                   JCDMA support for auth failures.
11/08/02    usb    Reverted tx_callback() to pass meta info as a single
                   pointer because storage of the outer pointer is on stack
                   and hence cann't be passed through the context switch.
10/25/02    usb    Fixed tx_callback() to pass metainfo as double pointer
                   rather than single pointer. This was causing illegal
                   dereferences. Also added proper freeing up of memory
                   during errors in pppi)outgoing.
10/11/02    usb    Modified HDLC hw feature to FEATURE_HDLC_HW_ACCEL
10/08/02    vr     Add 7E at the beginning of the PPP packet in hw framing
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/19/02    mvl    Removed feature super_esc as it is not useful anymore.
09/18/02    mvl    Added user_data to ppp event callbacks.  Setting mode in
                   start() and abort() calls, not internally.
09/12/02    mvl    Added the timer handling for the setup timeout function.
                   Removed netmodel featurization.
09/10/02    jd     Set tail_full to true when hw_unframe runs out of memory
                   so it will get a new dsm item rather than trying to insert
                   data in null ptr.  Also fixed unframe_mode in hw_unframe
                   to correspond to flow of ppp_asy_in so switching deframing
                   modes will not cause data aborts.
09/06/02  igt/mvl  added calls to funciton ppp_log_frames_ext() under
                   FEATURE_DEBUG_PPP_LOG_EXT to support extended ppp logging
                   capabilities.
09/06/02    mvl    fixed assert()s.
09/05/02    rwh    Lowered the low memory threshold in ppp_hdlc_hw_unframe()
                   from LESS_THAN_FEW to DONT_EXCEED for both pools.
09/02/02    mvl    Setting the PPP rx signal handler here, defined the RX
                   signal handler, and the PS Iface TX function.
08/30/02    dwp    Fixed test for making sure buffer passed to hdlc_read_buf
                   is 4-byte aligned. 0x3 ensures optimal packing.
08/30/02    vr     Fixed initialization bug in tail_full flag for the ifaces
                   and ensured that buffer passed to hdlc_read_buf will be
                   4-byte aligned
08/20/02    vr     Free existing chap_challenge_ptr if we get a new challenge
08/13/02    mvl    Added session handle support.
08/06/02  mvl/dwp  Updates to ppp_hdlc_hw_unframe() to pack DSM pkt chains.
08/06/02    mvl    Reworked authentication.
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
                   Resetting the FSM state to fsmINITIAL when PPP done.
                   Removed invalid HW framer macro.
07/29/02    mvl    Moved the IP reassembly code to pppi_abort() and also
                   setting the bridge_dev to invalid there. Fixed TASKLOCK()s
07/25/02    mvl    Moved input_params into ppp control block. Updates for PPP
                   renaming.
08/01/02    vr     Fixed chap_challenge_length exceecding DSM item size
07/30/02    igt    Removed calls to macros HDLC_FRAMER_CLEAR_ERR and
                   HDLC_DEFRAMER_CLEAR_ERR.
07/24/02    mvl    Moved ppp mode check to the RX callback.  Variables now
                   called device rather than iface.
07/19/02    rc/dwp Fix typo in ppp_outgoing. outgoing_pkt_ptr to
                   outgoing_head_ptr.
07/16/02    aku    Fixed bug in pppi_stop when SOCKETS is enabled (wrong
                   variable name used)
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    using bridge device rather than SWAP_IFACE() macro.
05/22/02    mvl    Renamed file and moved all of the truely external code to
                   the new ppp.c file.
05/20/02    vr     Added support for hardware HDLC framer/deframer
05/03/02    mvl    File cleanup and added meta_info to ppp_outgoing()
03/22/02    ss/vr  Fixed ppp_asy_in() code to allocate multiple DSM items
                   when copying bytes of a new PPP frame.
03/18/02    vas    Added code to call registered callback func when CHAP
                   succeeds / fails on the HDR AN iface.
03/06/02    vsk    Fixed an invalid mem access problem in pppi_skipped()
03/01/02    vr     Added support for PPP passive mode in ppp_powerup_init().
03/01/02    vas    Modified to support authentication (PAP/CHAP) variables
                   per interface.
02/12/02    ss     Modified ppp_asy_in() to make allocation of DSM items
                   from the memory pools more intelligent.
02/12/02    vsk    Defined internal ppp_ip_pkt_hdlr_f_ptr_type to make it
                   build for all targets
02/11/02    ss     Modified pppi_outgoing() to make allocation of DSM items
                   from the memory pools more intelligent.
02/07/02    vsk    Added support to call IP pkt handler function if installed
                   to process IP packet instead of the default ip_route()
02/05/02    vr     Added code to not do IPCP if configured accordingly
01/24/02    mvl    Fixed an init problem for the case when ASSERT()s are
                   off.  Also did some cleanup.
01/17/02    vsk    Fixed a NULL ptr access in pppi_outgoing()
01/14/02    vsk    Fixed mem leak problem in pppi_outgoing()
12/04/01    vr     Initialize all interface to not require auth. by default
12/01/01    vas    Changed function prototypes to take the ppp iface as a
                   parameter instead of being hardcoded. Made ppp_do_auth
                   part of ppp ctl blk. lcp_chap_info maintained per iface.
11/26/01    vr     Check for laptop call before forwarding packets during MIP
                   re-registration.
11/21/01    ss     Added call to macro MIP_DATA_TRANSFER_ACTION when there is
                   some data transfer on Um.
11/17/01    aku    Changed ppp_initiate_resync() to enqueue a
                   PS_START_PPP_RESYNC_CMD to the PS task.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module. Use FEATURE_DATA
                   for old/new DS Mgr dependencies. Use of the hdlc_enabled
                   flag in ppp_incoming/pppi_outgoing/ppp_powerup_init for ETSI
                   mode PPP (no HDLC over Uu). Make ppp_incoming external.
11/07/01    vr     added FEATURE_DS_MOBILE_IP wrapper
11/05/01    vr     During MIP re-reg, forward non-reg pkts to laptop
10/08/01    na     Fixed potential NULL pointer reference in ppp_asy_in().
09/20/01    na     CR18411: Fixed merge problem by removing unecessary
                   ASSERT in ppp_asy_in().
09/15/01    na     CR18328: Low throughput problem for network model calls
                   fixed in ppp_asy_in. Removed case for optimizing of
                   copy before 7E in FINDING_EOP mode.
09/14/01    jd     Added iface mode parameter to ppp_initiate_resync to
                   optionally allow Um interface to be put in internal mode
                   during resync.
09/10/01    na     CR18256, 18270 - ASSERT on tracer values in ppp_asy_in_init
                   was being triggered by incorrectly maintained tail_ptr.
09/07/01    na     ppp_asy_in() was asserting when it received a malformed
                   packet. Now it handles such a packet properly
09/03/01    aku    Putting iface in internal mode before in ppp_start() and
                   ppp_close()
08/31/01    mvl    Now checking if internal mode rather than if we are
                   resyncing to determine if packet needs processing after
                   beening snooped.
08/29/01    mvl    Removed ppp_init() as it was empty.
08/19/01    sjy    Added local PPP ctrl block ptr to pppi_outgoing(),
                   ppp_asy_in(), ppp_initiate(), and ppp_close() for when
                   netmodel feature is not defined.
                   Featurized ppp_initiate_resync() under
                   netmodel feature.
                   Featurized reference to iface_cb_array in
                   ppp_powerup_init() under netmodel feature.
08/12/01    sjy    Featurized reference to do_mip variable in PPP control
                   block
08/02/01    mvl    Removed the dependency on ppp_cb_ptr, renamed
                   ppp_skipped() to pppi_skipped() as it is internal, added
                   iface as an argument to pppi_skipped() and ppp_discard() ,
                   ppp_incoming() always takes iface as an argument, and
                   iface is now the first argument.
07/31/01    na     Added ppp_initiate_resync() function
07/30/01    mvl    Modified ppp_start() so that it actually works with
                   options that are passed in.  Added get_iface_opts().
07/27/01    mvl    Modified to use PSTimers, some code cleanup.
07/26/01    na     Fixed PPP_ASY_IN, so that the MS doesn't add an extra 0x7e
                   at the beginning of packets
07/24/01    mvl    Added initialization code so both interfaces are setup
                   correctly, ppp_initiate() and pp_close() take iface as an
                   arg and general code cleanup
07/23/01    pjb    Change location/prefix of command names. dsm_pushdown
                   return type changes. Change free count api.
                   dsm_offset_new_buffer now called to reserver header space.
06/21/01    mvl    Changed resync check in ppp_incoming() to be explicit
                   (checks PPP meta state machine state rather than checking
                   if in internal mode) - reinserted ASSERT()s Nish removed
                   (see below) but fixed them.
06/20/01    na     Removed 2 ASSERTs from ppp_asy_in_init() which were not
                   needed.
06/14/01    mvl    Updated a comment.
05/18/01    mvl    Changed OUT_IFACE() macro call to SWAP_IFACE().
02/27/01    ak     Removed pppi_outgoing from IRAM.
10/10/00    jd     Merged PPG r1.15 changes - account for dsm small item data
                   offset
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
                   Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, if pool size is to be passed in as
                   parameter, a new macro DSM_DS_POOL_SIZE is used to
                   return pool id based on pool size. Added Macros that
                   check if there are enough items or if we are running out
                   of items.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
07/31/00    ak     Removed an extraneous #ifndef MSM5000_IRAM_FWD.
07/24/00    snn    Cancelled the timer if MS receives a NAK from IWF in PAP.
06/28/00    kjn    Added void argument to chap_proc and pap_proc.
06/20/00    ak     Added MSM5000_IRAM_FWD to ppp_incoming.
06/13/00    ak     Added #ifndefs for MSM5000_IRAM_FWD.
05/20/00    snn    Added support for the IP Reassembly.
05/04/00    mvl    changed interface to network model version of
                   pppi_outgoing() to fix bug where item was freed in this
                   function then freed again lower down.
04/24/00    snn    Updated a comment in PAP code for a FAQ.
03/10/00    snn    Added Support for CHAP. These have been added under the
                   DS_CHAP feature. Removed the USRX hack code.
01/25/00    ak     In ppp_asy_in, check to see if dsm_item ptr is null after
                   call to dsm_pull8.
11/16/99    rc     Added fix in pppi_outgoing() to dump packets if the
                   dsm item free count is less than the minimum required.
11/05/99    na     Fixed asserts in ppp_asy_in_init().
11/05/99    mvl    Fixed asserts in ppp_asy_in_init() so that only trip when
                   tail item is not properly freed.
11/04/99    hcg    Fixed incorrect packet chain insertion in ppp_asy_in().
10/28/99    mvl    Added code review changes
10/22/99    mvl    Added support for network model related statistics.
09/25/99    snn    Removed the parameter being passed to pap_proc(), because
                   it is not needed.
                   Added comments and fixed the commenting styles.
                   If Auth_Nak is recieved code will not terminate the link.
08/31/99    mvl    Fixed ppp_asy_in() so that we send items out in pseudo
                   network mode rather than buffering up an entire packet.
08/26/99    snn    Added code to terminate the PPP link if mobile recieves
                   an auth-nak from the base station.
08/25/99    mvl    Added ppp_asy_in_init() to reinitialize the ppp_asy_in()
                   variables to fix netmodel memory leak.  Also added tracer
                   values to help track down leak.
08/20/99    mvl    Changed LOG_MSG_INFO2()s in ppp_asy_in() to LOG_MSG_INFO3()s
08/17/99    smp    Added include files msg.h & err.h.
08/10/99    snn    Added support for PAP timeouts and cleaned up comments.
08/09/99    ak     Added test points for sockets testing, where we can
                   simulate dropping PPP packets.
08/09/99    mvl    Added network model changes.
07/08/99    snn    Added support for DS_PAP feature.
04/08/99    na     Changed PPP_LCP_PROTOCOL in pppi_outgoing() to
                   DSM_LCP_PROTOCOL.
10/27/98    ldg    ARM porting changes: ROM to ROM const.
07/26/98    jjn    Use more efficient DSM.
04/14/98    na     Increased RAMP_MAX_SIZE to avoid potential memory blowup
                   problems. Added buffer based flow control.
01/27/98    ldg    Removed the DS_SLIM switch.
01/19/98    na     Sped up incoming and outgoing PPP processing, by removing
                   the overhead of dsm_pullup in each while loop up.
12/12/97    ldg    if_cb.InErrors now excludes checksum errors.
10/02/97    na     Added support for escaping all PPP outgoing bytes with
                   upper nibble set to F. Under FEATURE_DS_SUPER_ESC.
07/31/97    ldg    Added accessor function for UI to see some PPP vars.
07/30/97    na     Changed ACCM processing for a USRX test build. PPP doesn't
                   drop characters below 20h at all. Under DS_PAP feature.
07/22/97    na     Added PAP support - under DS_PAP feature.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
06/12/97    ldg    Added new FEATURE_DS_PSTATS
06/05/97    na     Allowed a string of large items for incoming ppp data.
03/13/97    jjn    Included target.h and customer.h
02/19/97    na     UP fixes in ppp_asy_in.
02/13/97    na     Jim fixed a bug in ppp_asy_in that was causing memory
                   scribbles. CTIA show build.
01/23/97    na     Added a ramping procedure to guess whether next incoming
                   pkt. will need a small or a large item.
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
 *  Dec 92  Bill_Simpson@um.cc.umich.edu
 *    added IPXCP.
 *  Jun 92  Bill_Simpson@um.cc.umich.edu
 *    revised buffering and tracing.
 *    added CHAP and LQR.
 *  Mar 92  Bill_Simpson@um.cc.umich.edu
 *    combined send, output, and raw using new send and queue.
 *  Jan 92  Bill_Simpson@um.cc.umich.edu
 *    Conforms to newest draft RFCs.
 *  Jul 91  Glenn McGregor & Bill Simpson
 *    Improve PAP user interface and fix related bugs.
 *    Remove pwaits and "phase machine".
 *  May 91  Bill Simpson & Glenn McGregor
 *    Update to newest LCP and IPCP draft RFCs.
 *    Add quick installation features.
 *    Add support for echo and discard message sending.
 *  Feb 91  Glenn McGregor      (ghm@merit.edu)
 *    Testing and suggestions.
 *  Jan 91  Bill_Simpson@um.cc.umich.edu
 *    Computer Systems Consulting Services
 *  09-90  -- Katie Stevens    (dkstevens@ucdavis.edu)
 *       UC Davis, Computing Services
 *  Based (in part) upon previous implementations by:
 *  1989  -- Drew Daniel Perkins    (ddp@andrew.cmu.edu)
 *       Carnegie Mellon University
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"


#include "crc.h"
#include "dsm.h"
#include "ps_in.h"
#include "ps_mem.h"
#include "ps_ppp_config.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_pppi.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ps_ppp_task.h"
#include "ps_ppp_snoop.h"
#include "ps_pppi_snoop.h"
#include "ps_ppp_snpip.h"
#include "ps_ppp_netmodel.h"
#include "ps_meta_info.h"
#include "ps_tx_meta_info.h"
#include "ps_ip.h"
#include "ps_svc.h"
#include "pstimer.h"
#include "ps_byte.h"
#include "amssassert.h"
#include "rex.h"
#include "task.h"
#include "msg.h"
#include "err.h"
#include "ps_stat_ppp.h"
#include "ps_utils.h"
#include "ps_hdlc_libi.h"

#include "ps_ppp_logging.h"

#include "ps_ppp_events.h"
#include "ps_iface.h"
#include "ps_ifacei.h"

#include "ps_logging.h"
#include "ps_logging_defs.h"

#include "ps_iphc.h"

#ifdef FEATURE_DATA_PS_PPP_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_EAP
#include "ps_eap.h"
#include "ps_eap_aka.h"
#endif /* FEATURE_DATA_PS_PPP_EAP */

#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_mem.h"
#include "ps_ppp_ncpi.h"
#endif /* FEATURE_DATA_PS_EHRPD */


#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"

/*---------------------------------------------------------------------------
  Lookup table to convert IPHC packet types to PPP protocols.  Indexed by
  enum from iphc_packet_type_e_type
---------------------------------------------------------------------------*/
int iphc_packet_type_to_ppp_protocol_tbl[IPHC_PACKET_TYPE_MAX] =
{
  PPP_IP_PROTOCOL, /* IPHC_PACKET_TYPE_NORMAL_IPV4 */
  PPP_IPV6_PROTOCOL, /* IPHC_PACKET_TYPE_NORMAL_IPV6 */
  PPP_IPHC_FULL_HEADER_PROTOCOL, /* IPHC_PACKET_TYPE_FULL_HEADER */
  PPP_IPHC_COMPRESSED_TCP_PROTOCOL, /* IPHC_PACKET_TYPE_COMPRESSED_TCP */
  PPP_IPHC_COMPRESSED_TCP_NODELTA_PROTOCOL,
                                /* IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA */
  PPP_IPHC_COMPRESSED_NON_TCP_PROTOCOL,
                                /* IPHC_PACKET_TYPE_COMPRESSED_NON_TCP */
  PPP_IPHC_CONTEXT_STATE_PROTOCOL, /* IPHC_PACKET_TYPE_CONTEXT_STATE */
  PPP_IPHC_COMPRESSED_UDP_8_PROTOCOL, /* IPHC_PACKET_TYPE_COMPRESSED_UDP_8 */
  PPP_IPHC_COMPRESSED_UDP_16_PROTOCOL, /* IPHC_PACKET_TYPE_COMPRESSED_UDP_16 */
  PPP_IPHC_COMPRESSED_RTP_8_PROTOCOL, /* IPHC_PACKET_TYPE_COMPRESSED_RTP_8 */
  PPP_IPHC_COMPRESSED_RTP_16_PROTOCOL  /* IPHC_PACKET_TYPE_COMPRESSED_RTP_16 */
};

#ifdef FEATURE_DATA_PS_PPP_IPHC
#define PPP_PROTOCOL_TO_IPHC_PACKET_TYPE(protocol, pkt_type) \
  for(pkt_type = 0; pkt_type < IPHC_PACKET_TYPE_MAX; pkt_type++) \
  { \
    if (protocol == iphc_packet_type_to_ppp_protocol_tbl[pkt_type]) \
      break; \
  }
#endif /* FEATURE_DATA_PS_PPP_IPHC */


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  Tuning the number of PPP command buffers needed by this module
---------------------------------------------------------------------------*/
#define PPP_CMD_BUF_SIZE  ((sizeof(ppp_cmd_buf_u_type) + 3) & ~3)

/*---------------------------------------------------------------------------
  Maximal use scenario calculation for eHRPD and non-eHRPD modes
  1) eHRPD mode (Max_PDNs = 8)
  = = = = = = = = = = = = = =
  a) Per-PDN commands: ncp_start, resync, stop, abort = 4
  b) Um commands: dev stop, ppp abort, vendor send, trigger lsd = 4
  Total = (Max * a) + b = 36
  
  2) non-eHRPD mode (Max_devices = 4 (Um, Rm, AN, FMC)
  = = = = = = = = = = = = = = = = = = = = = = = = = = 
  a) Per-Device (Um) commands: ppp_start, resync, stop, abort, set mode = 5
  b) Rm command (UMTS) = inject pkt

  Total = *Max_Devices * a) + b = 21
  Allocating per 75% of worst case requires ~27 buffers
  (Could go more conservative (say 60%) since simultaneous commands are 
  very unlikely and a total of 10 commands have been enough for quite a 
  while - 60% works to ~22 buffers)
  HIGH WM = 80%(28) = ~22
  LOW WM = 20%(28) = ~6
---------------------------------------------------------------------------*/
   /* Increase command buffer to accomdate for 8 VSNCP instances */
   /* Increase command buffer to accomdate for 1 RM instances */
#ifdef FEATURE_DATA_3GPP2_ULT   
   #define PPP_CMD_BUF_NUM       22
   #define PPP_CMD_BUF_HIGH_WM   17
   #define PPP_CMD_BUF_LOW_WM     5
#else
   #define PPP_CMD_BUF_NUM       28
   #define PPP_CMD_BUF_HIGH_WM   22
   #define PPP_CMD_BUF_LOW_WM     6
#endif /*FEATURE_DATA_3GPP2_ULT*/


/*----------------------------------------------------------------------------
  Allocate memory to hold ppp_cmd along with ps_mem header
----------------------------------------------------------------------------*/
static int ppp_cmd_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                           (
                            PPP_CMD_BUF_NUM,
                            PPP_CMD_BUF_SIZE
                           )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ppp_cmd_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ppp_cmd_buf_hdr[PPP_CMD_BUF_NUM];
static ppp_cmd_buf_u_type  * ppp_cmd_buf_ptr[PPP_CMD_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of PPP event info buffers needed by this module
---------------------------------------------------------------------------*/
#define PPP_EVT_INFO_BUF_SIZE  ((sizeof(ppp_event_info_s_type) + 3) & ~3)

/*---------------------------------------------------------------------------
  Event buffer counts. Account for the following client needs:
  a.	RMSM: 2
  b.	UMTS Data: 2
  c.	HDR Data: 1
  d.	8 Dual-IP PDNs need 16 total (1 each for v4 and v6)
  e.	UW FMC needs 1
  f.	3gpp2 vendor specific implementation needs 1
  g.	Async PPP needs 1  
  h.    The second RM device need 1
  Total: 18
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_3GPP2_ULT 
#define PPP_EVT_INFO_BUF_NUM      21
#define PPP_EVT_INFO_BUF_HIGH_WM  18
#else
#define PPP_EVT_INFO_BUF_NUM      25
#define PPP_EVT_INFO_BUF_HIGH_WM  22
#endif /*FEATURE_DATA_3GPP2_ULT*/
#define PPP_EVT_INFO_BUF_LOW_WM    4

/*----------------------------------------------------------------------------
  Allocate memory to hold ppp_evt_info along with ps_mem header
----------------------------------------------------------------------------*/
static int ppp_evt_info_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(
                                PPP_EVT_INFO_BUF_NUM,
                                PPP_EVT_INFO_BUF_SIZE
                              )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ppp_evt_info_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type   * ppp_evt_info_buf_hdr[PPP_EVT_INFO_BUF_NUM];
static ppp_event_info_s_type * ppp_evt_info_buf_ptr[PPP_EVT_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
boolean ppp_hdlc_a2_autonomous_enable = FALSE;
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */

/*===========================================================================
MACRO PPPI_PSEUDONET_APPEND_TO_CHAIN

DESCRIPTION
  This macro is used by the pseudonet state machine to maintain state for a
  packet whose end is being sought, for onwards transmission to the bridge
  device. It appends the passed DSM item to the "original" packet chain in the
  PPP pseudonet state machine. If there are no DSM items in the "original"
  chain, this item is set as the first in that chain, else it is appended to
  the tail of the existing DSM chain.

PARAMETERS
  param_ptr    : Pointer to pseudonet im the PPP control block
  this_dsm_ptr : pointer to the DSM item examined by PN state machine

===========================================================================*/
#define PPPI_PSEUDONET_APPEND_TO_CHAIN( param_ptr, this_dsm_ptr )           \
{                                                                           \
  if( param_ptr->framed_pkt_ptr == NULL )                                   \
  {                                                                         \
    param_ptr->framed_pkt_ptr      = this_dsm_ptr;                          \
    param_ptr->framed_pkt_tail_ptr = this_dsm_ptr;                          \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    ASSERT( (param_ptr->framed_pkt_tail_ptr) != NULL );                     \
    param_ptr->framed_pkt_tail_ptr->pkt_ptr  = this_dsm_ptr;                \
    param_ptr->framed_pkt_tail_ptr           = this_dsm_ptr;                \
    param_ptr->framed_pkt_tail_ptr->pkt_ptr  = NULL;                        \
  }                                                                         \
}

/*---------------------------------------------------------------------------
  PPP Control Block device constant definition

DESCRIPTION
  Definition of the cb_constant_s type structure common for LCP,
  IPCP and IPv6CP FSM structures.
  ---------------------------------------------------------------------------*/
struct cb_constant_s pppi_fsm_cb_default_constants =
{
  (call_event_callback_f_type)pppi_fsm_call_event_callback,
  pppi_outgoing,
  pppi_vendor_ext_hdlr
};

/*---------------------------------------------------------------------------
  PPP Info structure - used for debugging - contains pointers to all of the
  PPP data structures
---------------------------------------------------------------------------*/
static struct
{
  ppp_type        *ppp_cb;
  lcp_value_type  *lcp_local_want_vals;
  lcp_value_type  *lcp_local_work_vals;
  lcp_value_type  *lcp_remote_want_vals;
  lcp_value_type  *lcp_remote_work_vals;
  ipcp_value_type *ipcp_local_want_vals;
  ipcp_value_type *ipcp_local_work_vals;
  ipcp_value_type *ipcp_remote_want_vals;
  ipcp_value_type *ipcp_remote_work_vals;
#ifdef FEATURE_DATA_PS_IPV6
  ipv6cp_value_type *ipv6cp_local_want_vals;
  ipv6cp_value_type *ipv6cp_local_work_vals;
  ipv6cp_value_type *ipv6cp_remote_want_vals;
  ipv6cp_value_type *ipv6cp_remote_work_vals;
#endif /* FEATURE_DATA_PS_IPV6 */
} ppp_info[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  The value should be the maximum possible value of PPP_MAX_DEV (which is the
  value when all features are ON)
---------------------------------------------------------------------------*/
#define PPP_MAX_SIGNAL  4

/*---------------------------------------------------------------------------
  Array that maps the PPP devices to the PPP signals for each instance

  NOTE: It is CRITICAL that the signals here are in the order of the ppp
        devices that they correspond to.
---------------------------------------------------------------------------*/
const ppp_sig_type ppp_dev_to_default_sig_array[PPP_MAX_SIGNAL] =
{
  PPP_UM_1_RX_SIGNAL,
  PPP_RM_RX_SIGNAL,
  PPP_UM_2_RX_SIGNAL,
  PPP_RM_2_RX_SIGNAL
};


/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/
static boolean pppi_send_prot_rej
(
 ppp_dev_enum_type iface,           /* iface (Rm or Um) pkt arrived on   */
 uint16 protocol,                   /* Bad protocol for which to PROT_REJ*/
 dsm_item_type *item_ptr            /* Pointer to the packet             */
);

static void pppi_i_setup_timout_cb
(
  void *arg_ptr
);

static void pppi_pseudonet_reset
(
   ppp_dev_enum_type device
);

static void pppi_none_mode_hdlr
(
  ppp_dev_enum_type   device,
  void              * hdlc_inst,
  dsm_item_type    ** framed
);

static void pppi_relay_mode_hdlr
(
  ppp_dev_enum_type   device,
  void              * hdlc_inst,
  dsm_item_type    ** framed
);

static void pppi_pseudonet_mode_hdlr
(
  ppp_dev_enum_type   device,
  void              * hdlc_inst,
  dsm_item_type    ** framed
);

static void pppi_internal_mode_hdlr
(
  ppp_dev_enum_type   device,
  void              * hdlc_inst,
  dsm_item_type    ** framed
);



/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
/*===========================================================================
FUNCTION PPPI_A2_ENABLE_HDLC_AUTONOMOUS

DESCRIPTION
  This FUNCTION enables/disables autonomous mode HDLC framing/deframing on 
  a given SIO port.

PARAMETERS
  hdlc_accm
  enable

RETURN VALUE
  None

DEPENDENCIES
  
SIDE EFFECTS
  None
===========================================================================*/
void pppi_a2_enable_hdlc_autonomous
(
  uint32 hdlc_accm,
  boolean enable
)
{
  a2_hdlc_frame_config_s frm_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&frm_config, 0, sizeof(a2_hdlc_frame_config_s));

  /* Call A2 HDLC CONFIG API to enable autonomous HDLC framing/deframing */
  frm_config.insert_crc_flag = TRUE;
  frm_config.insert_end_flag = TRUE;
  frm_config.insert_start_flag = TRUE;
  LOG_MSG_INFO1_2("a2_hdlc_config_sio_mdm called with enable/disable: %d with accm: 0x%x",
                  enable, hdlc_accm);
  if(enable)
  {
    frm_config.action = A2_HDLC_ENABLE;
    a2_hdlc_config_sio_mdm(A2_HDLC_MODULE_PS, 
                            &frm_config, 
                            hdlc_accm);    
  }
  else
  {
    frm_config.action = A2_HDLC_DISABLE;
    a2_hdlc_config_sio_mdm(A2_HDLC_MODULE_PS, 
                            &frm_config, 
                            hdlc_accm);    

  }
  return;
}
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */

/*===========================================================================
FUNCTION PPPI_POWERUP_INIT()

DESCRIPTION
  This function does the power up initialization for PPP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_powerup_init
(
  void
)
{
  int                           init_check;
  uint32                        loop;
  hdlc_config_type              hdlc_config;
  ppp_type                     *ppp_cb_array = NULL;

  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PPP_CMD_BUF_TYPE,
                           ppp_cmd_buf_mem,
                           PPP_CMD_BUF_SIZE,
                           PPP_CMD_BUF_NUM,
                           PPP_CMD_BUF_HIGH_WM,
                           PPP_CMD_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ppp_cmd_buf_hdr,
                           (int *) ppp_cmd_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                           ) == -1)
  {
    DATA_ERR_FATAL("Can't init the module");
    return;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PPP_EVENT_INFO_ITEM_TYPE,
                           ppp_evt_info_buf_mem,
                           PPP_EVT_INFO_BUF_SIZE,
                           PPP_EVT_INFO_BUF_NUM,
                           PPP_EVT_INFO_BUF_HIGH_WM,
                           PPP_EVT_INFO_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ppp_evt_info_buf_hdr,
                           (int *) ppp_evt_info_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                           ) == -1)
  {
    DATA_ERR_FATAL("Can't init the module");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  for(loop = (uint8)PPP_MIN_DEV; loop < (uint8)PPP_MAX_DEV; loop += 1)
  {
    /*-----------------------------------------------------------------------
      memset() the ppp control blocks
    -----------------------------------------------------------------------*/
    memset(ppp_cb_array + loop, 0, sizeof(ppp_type));
    ppp_cb_array[loop].auth.ps_eap_handle = EAP_INVALID_HANDLE;

    /*-----------------------------------------------------------------------
      Initialize LCP and IPCP
    -----------------------------------------------------------------------*/
    init_check = lcp_powerup_init((ppp_dev_enum_type)loop);
    ASSERT(init_check == 0);
    init_check = ipcp_powerup_init((ppp_dev_enum_type)loop);
    ASSERT(init_check == 0);
#ifdef FEATURE_DATA_PS_IPV6
    init_check = ipv6cp_powerup_init((ppp_dev_enum_type)loop);
    ASSERT(init_check == 0);
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Allocate a timer for total setup time
    -----------------------------------------------------------------------*/
    ppp_cb_array[loop].setup_timer = ps_timer_alloc( pppi_i_setup_timout_cb,
                                                     (void*)( loop ) );
    ASSERT(ppp_cb_array[loop].setup_timer != PS_TIMER_INVALID_HANDLE);

    /*-----------------------------------------------------------------------
      set the PPP phase to dead and assign the default r/tx functions.
    -----------------------------------------------------------------------*/
    ppp_cb_array[loop].phase            = pppDEAD;
    ppp_cb_array[loop].rx_f_ptr         = pppi_default_rx;
    ppp_cb_array[loop].tx_f_ptr         = pppi_default_tx;
    ppp_cb_array[loop].rx_signal        = PPP_INVALID_SIGNAL;
    ppp_cb_array[loop].rx_sig_f_ptr     = pppi_get_data_cback;
    ppp_cb_array[loop].bridge_dev       = PPP_INVALID_DEV;
    ppp_cb_array[loop].mode             = (uint16)PPP_NONE_MODE;
    ppp_cb_array[loop].rx_pkt_hdlr_f_ptr = pppi_none_mode_hdlr;
    ppp_cb_array[loop].pending_mode     = PPP_INVALID_MODE;
    ppp_cb_array[loop].hdlc_mode        = HDLC_INVALID_MODE;
    ppp_cb_array[loop].auth.mode        = PPP_NO_AUTH;
    ppp_cb_array[loop].auth.retry_limit = PPP_AUTH_RETRY_COUNTER_VAL;
    ppp_cb_array[loop].auth.timeout     = PPP_AUTH_TIMER_VAL;
    ppp_cb_array[loop].failover         = PPP_FAILOVER_IGNORE;

    hdlc_init_config( &hdlc_config );
    hdlc_config.accm                    = LCP_ACCM_DEFAULT;
    hdlc_config.return_pkt_on_bad_crc   = FALSE;
    ppp_cb_array[loop].hdlc_handle = hdlc_alloc_instance( &hdlc_config );
    if( ppp_cb_array[loop].hdlc_handle  == NULL )
    {
      LOG_MSG_ERROR_1("Unable to allocate HDLC instance, dev (%d)", loop );
      ASSERT( 0 );
      return;
    } /* Unable to allocate HDLC instance */

    /*-----------------------------------------------------------------------
      Initialize the PPP lock
    -----------------------------------------------------------------------*/
    PS_INIT_CRIT_SECTION( pppi_get_crit_sect_lock( (ppp_dev_enum_type)loop) );

    /*-----------------------------------------------------------------------
      Initialize the PPP event queue
    -----------------------------------------------------------------------*/
    (void)q_init( &ppp_cb_array[loop].event_queue );

    /*-----------------------------------------------------------------------
      Initialize the ppp_info data structure
    -----------------------------------------------------------------------*/
    ppp_info[loop].ppp_cb                = &(ppp_cb_array[loop]);
    ppp_info[loop].lcp_local_want_vals   = lcp_get_local_want_cb_vals(loop);
    ppp_info[loop].lcp_local_work_vals   = lcp_get_local_work_cb_vals(loop);
    ppp_info[loop].lcp_remote_want_vals  = lcp_get_remote_want_cb_vals(loop);
    ppp_info[loop].lcp_remote_work_vals  = lcp_get_remote_work_cb_vals(loop);
    ppp_info[loop].ipcp_local_want_vals  = ipcp_get_local_want_cb_vals(loop);
    ppp_info[loop].ipcp_local_work_vals  = ipcp_get_local_work_cb_vals(loop);
    ppp_info[loop].ipcp_remote_want_vals = ipcp_get_remote_want_cb_vals(loop);
    ppp_info[loop].ipcp_remote_work_vals = ipcp_get_remote_work_cb_vals(loop);
#ifdef FEATURE_DATA_PS_IPV6
    ppp_info[loop].ipv6cp_local_want_vals  = ipv6cp_get_local_want_cb_vals(loop);
    ppp_info[loop].ipv6cp_local_work_vals  = ipv6cp_get_local_work_cb_vals(loop);
    ppp_info[loop].ipv6cp_remote_want_vals = ipv6cp_get_remote_want_cb_vals(loop);
    ppp_info[loop].ipv6cp_remote_work_vals = ipv6cp_get_remote_work_cb_vals(loop);
#endif /* FEATURE_DATA_PS_IPV6 */

  } /* for(all PPP devices) */

  (void)ppp_info;                 /* This provides the "access" lint needs */
  snoop_ip_init();

#ifdef FEATURE_DATA_PS_EHRPD
  ncpi_powerup_init();
#endif /* FEATURE_DATA_PS_EHRPD(); */

  return;
} /* pppi_powerup_init() */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                              CONTROL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_DEV_INIT()

DESCRIPTION
  Internal function to initialize a PPP device and copy any passed
  configuration

PARAMETERS
  ppp_cb_ptr   PPP device control block for which LCP needs to be
               initialized
  config_ptr   Any passed configuration

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes PPP device.
===========================================================================*/
void pppi_dev_init
(
  ppp_type          *ppp_cb_ptr,
  ppp_dev_opts_type *config_ptr
)
{
  pppi_mode_info_type   ppp_mode_info;
  hdlc_ioctl_u_type     ioctl_data;
  hdlc_errno_enum_type  ps_errno;
  ppp_dev_enum_type     device;
  ppp_type             *ppp_cb_array = NULL;
  ppp_dev_enum_type     fsm_device;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If configuration is passed then use it to set device parameters
    Parameters needing a task lock are rx_f_ptr, tx_f_ptr and user data,
    session_handle, mode, event callbacks and rx_iface_ptr

    Also set the world phone parameters for PPP passed in the config_ptr.
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  device = (ppp_dev_enum_type)(ppp_cb_ptr - ppp_cb_array);

  if(PPP_MAX_DEV <= device)
  {
    LOG_MSG_ERROR_1("Invalid device = %d", device);
    ASSERT(0);
    return;
  }

  if( NULL != config_ptr )
  {
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_ptr->tx_f_ptr               = config_ptr->tx_f_ptr;
    ppp_cb_ptr->rx_f_ptr               = config_ptr->rx_f_ptr;
    ppp_cb_ptr->tx_user_data           = config_ptr->tx_user_data;
    ppp_cb_ptr->rx_user_data           = config_ptr->rx_user_data;
    ppp_cb_ptr->fsm[LCP].mode          = config_ptr->lcp_info.mode;
    ppp_cb_ptr->bridge_dev             = config_ptr->bridge_dev;
    ppp_cb_ptr->serves_addrs           = config_ptr->serves_addrs;
    ppp_cb_ptr->passive_wakeup_on_data = config_ptr->passive_wakeup_on_data;
#ifdef FEATURE_DATA_PS_PPP_ROHC
    ppp_cb_ptr->rohc_handle = NULL;
    ppp_cb_ptr->rohc_ncp = 0;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
    ppp_cb_ptr->iphcp = NULL;
    ppp_cb_ptr->iphc_ncp = 0;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
    ppp_cb_ptr->ppp_failure_graceful_close =
      config_ptr->ppp_failure_graceful_close;
    ppp_cb_ptr->set_stabilize_accm_delay =
      config_ptr->set_stabilize_accm_delay;
    ppp_cb_ptr->resync_dns_opts = config_ptr->resync_dns_opts;

#ifdef FEATURE_DATA_PS_ENFORCE_AUTH
    ppp_cb_ptr->allow_no_auth = config_ptr->allow_no_auth;
#endif /* FEATURE_DATA_PS_ENFORCE_AUTH */

    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    lcp_init(ppp_cb_ptr->fsm[LCP].device, config_ptr);
	fsm_device = ppp_cb_ptr->fsm[LCP].device;

    /*---------------------------------------------------------------------
      if the auths peer variable is set then set the flags field in the auth
      info section of the control block to indicate that we are
      authenticating the peer - otherwise set it to indicate that we are
      being authenticated.

      NOTE: this implies that mutual authentication is NOT possible, i.e. we
            can only be a client OR a server not both.
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    if(FALSE != config_ptr->auths_peer)
    {
      ppp_cb_ptr->auth.flags = PPP_AP_REMOTE;
    }
    else
    {
      ppp_cb_ptr->auth.flags = PPP_AP_LOCAL;
    }

    /*-----------------------------------------------------------------------
      copy auth information
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->auth.mode = PPP_NO_AUTH;
    memscpy(&ppp_cb_ptr->auth.info,
            sizeof(ppp_auth_info_type),
            &config_ptr->auth_info,
            sizeof(ppp_auth_info_type));

    /*-----------------------------------------------------------------------
      if the value of the rx_signal is invalid then use the default 1x
      signal otherwise use the passed in value.  Update the signal handler
      appropriately.
    -----------------------------------------------------------------------*/
    if( config_ptr->rx_signal == PPP_INVALID_SIGNAL &&
        fsm_device < PPP_MAX_DEV )
    {
      ppp_cb_ptr->rx_signal =
      ppp_dev_to_default_sig_array[fsm_device];
    }
    else
    {
      ppp_cb_ptr->rx_signal = config_ptr->rx_signal;
    }
    ppp_cb_ptr->rx_sig_f_ptr = config_ptr->rx_sig_f_ptr;

    ppp_cb_ptr->setup_final_phase = config_ptr->setup_final_phase;

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
    ppp_cb_ptr->dos_is_enabled = config_ptr->dos_is_enabled;
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
    /*-----------------------------------------------------------------------
      If the setup timeout is set to a value less than the MIN_SETUP_TIME
      then set it to that value.  This enforces that the value is at least
      set to something reasonable.
    -----------------------------------------------------------------------*/
    if(config_ptr->setup_timeout > 0 &&
       config_ptr->setup_timeout < PPP_MIN_SETUP_TIME)
    {
      ppp_cb_ptr->setup_timeout = PPP_MIN_SETUP_TIME * 1000;
    }
    else
    {
      ppp_cb_ptr->setup_timeout  = (uint16)(config_ptr->setup_timeout*1000);
    }
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    if(ppp_cb_ptr->hdlc_mode == HDLC_INVALID_MODE)
    {
      ppp_cb_ptr->hdlc_mode = config_ptr->hdlc_mode;
    } /* HDLC is not yet initialized */

    ppp_mode_info.device = (uint16)device;
    ppp_mode_info.mode   = (uint16) PPP_INTERNAL_MODE;
    pppi_set_mode( ppp_mode_info );

  } /* config_ptr not NULL */

  ASSERT( ppp_cb_ptr->hdlc_handle != NULL );

#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
  if(ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS)
  {
    pppi_a2_enable_hdlc_autonomous(LCP_ACCM_DEFAULT, 
                                   TRUE);
    ppp_hdlc_a2_autonomous_enable = TRUE;
  }
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */
  ioctl_data.accm = LCP_ACCM_DEFAULT;
  LOG_MSG_INFO2_1("dev init, setting HDLC ACCM = 0x%x", ioctl_data.accm);
  if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                  HDLC_IOCTL_SET_ACCM,
                  &ioctl_data,
                  &ps_errno
                ) != 0 )
  {
    LOG_MSG_ERROR_2("Unable to set HDLC ACCM for dev %d, ps_errno %d",
                    ppp_cb_ptr - ppp_cb_array, ps_errno);
  }

  /*-------------------------------------------------------------------------
    Initialize the LCP state machine
  -------------------------------------------------------------------------*/
  fsm_init(ppp_cb_ptr->fsm + LCP);

  return;
} /* pppi_dev_init() */



/*===========================================================================
FUNCTION PPPI_DEV_START()

DESCRIPTION
  This function is called to start a PPP device

PARAMETERS
  ppp_cb_ptr   PPP control block pointer

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Enable PPP receive signal, start setup timer and if mode is ACTIVE call
  fsm_up on LCP
===========================================================================*/
void pppi_dev_start
(
  ppp_type *ppp_cb_ptr
)
{
  ppp_type         *ppp_cb_array = NULL;
  ppp_dev_enum_type device;
  boolean           bring_up_lcp = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ppp_cb_array = fsm_get_ppp_cb_array();
  device = (ppp_dev_enum_type)(ppp_cb_ptr - ppp_cb_array);

  /*-------------------------------------------------------------------------
    Set signal handler for PPP receive signal and enable the signal
  -------------------------------------------------------------------------*/
  (void)PPP_SET_SIG_HANDLER(ppp_cb_ptr->rx_signal,
                            ppp_cb_ptr->rx_sig_f_ptr,
                            (void*)device);
  PPP_ENABLE_SIG(ppp_cb_ptr->rx_signal);


  /*-------------------------------------------------------------------------
    Only start the setup timer if the setup time is non-zero
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->setup_timer != PS_TIMER_INVALID_HANDLE &&
     ppp_cb_ptr->setup_timeout > 0)
  {
    LOG_MSG_INFO2_1("Starting %dms setup timer", ppp_cb_ptr->setup_timeout);
    (void)ps_timer_start(ppp_cb_ptr->setup_timer,
                         (int64)ppp_cb_ptr->setup_timeout);
  }

  /*-------------------------------------------------------------------------
    Bring up LCP if in PPP_CP_MODE_ACTIVE state
  -------------------------------------------------------------------------*/
  if(PPP_CP_MODE_ACTIVE == ppp_cb_ptr->fsm[LCP].mode)
  {
    bring_up_lcp = TRUE;
  }

  if( TRUE == bring_up_lcp )
  {
    fsm_up(ppp_cb_ptr->fsm + LCP);
  }

  return;
} /* pppi_dev_start() */



/*===========================================================================
FUNCTION PPPI_NCP_MODE_CHANGE()

DESCRIPTION
  Figure out based on the passed current control protocol mode passed and
  new mode what mode should the protocol assume

  With old mode in the first row and new mode along the first column the
  mode returned by the function is given in the following table.  This
  table can also be implemented as a two dimensional array.

              INVALID    ACTIVE    PASSIVE    DISABLED   DISCARD

  INVALID     INVALID    ACTIVE    PASSIVE    DISABLED   DISCARD
  ACTIVE      ACTIVE     ACTIVE    ACTIVE     ACTIVE     ACTIVE
  PASSIVE     PASSIVE    ACTIVE    PASSIVE    PASSIVE    PASSIVE
  DISABLED    DISABLED   ACTIVE    PASSIVE    DISABLED   DISABLED
  DISCARD     DISCARD    ACTIVE    PASSIVE    DISCARD    DISCARD

PARAMETERS
  old_mode   Present protocol mode
  new_mode   New mode passed

RETURN VALUE
  the mode the control protocol should assume

DEPENDENCIES
  This should be called only for an NCP

SIDE EFFECTS
  None
===========================================================================*/
cp_mode_type pppi_ncp_mode_change
(
  cp_mode_type old_mode,
  cp_mode_type new_mode
)
{
  cp_mode_type ret_mode = new_mode;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( new_mode )
  {
  case PPP_CP_MODE_INVALID:
    ret_mode = old_mode;
    break;

  case PPP_CP_MODE_DISCARD:
  case PPP_CP_MODE_DISABLED:
  case PPP_CP_MODE_PASSIVE:
    if( PPP_CP_MODE_PASSIVE == old_mode ||
        PPP_CP_MODE_ACTIVE  == old_mode )
    {
      ret_mode = old_mode;
    }
    break;

  case PPP_CP_MODE_ACTIVE:
    break;

  default:
    LOG_MSG_ERROR_1( "Invalid mode passed %d", new_mode );
    ASSERT( 0 );
    break;
  }

  return ret_mode;
} /* pppi_ncp_mode_change() */



/*===========================================================================
FUNCTION PPPI_START()

DESCRIPTION
  This function will use configuration information passed in to initialize
  any PPP protocol that isn't started yet and start any active PPP protocol
  in sequence (LCP followed by NCPs once LCP is open)

PARAMETERS
  configbuf_ptr   Pointer to PPP configuration buffer (allocated from PS mem)

RETURN VALUE
  None

DEPENDENCIES
  The device must be properly initialized

SIDE EFFECTS
  Initializes and starts any relevant PPP protocols.  Frees configuration
  buffer at the end
===========================================================================*/
void pppi_start
(
  ppp_config_buffer_s_type *configbuf_ptr
)
{
  ppp_type          *ppp_cb_ptr;
  ppp_type          *ppp_cb_array = NULL;
  ppp_dev_opts_type *config_ptr;
  uint16             session_handle;
  ppp_dev_enum_type  device;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify passed in arguments.  For ppp_start() configuration pointer can't
    be NULL
  -------------------------------------------------------------------------*/
  if( NULL        == configbuf_ptr                ||
      TRUE        == configbuf_ptr->dev_opts_null ||
      PPP_MAX_DEV <= configbuf_ptr->device )
  {
    if( NULL == configbuf_ptr )
    {
      LOG_MSG_ERROR_0( "NULL configbuf_ptr passed" );
    }
    else
    {
      LOG_MSG_INFO1_2( "Invalid arguments: config_ptr %d, device %d",
                       configbuf_ptr->dev_opts_null, configbuf_ptr->device );
    }
    ASSERT( 0 );
    return;
  }

  /*-----------------------------------------------------------------------
      Set the appropriate parameters in the configuration log packet
      and call the API to log to the packet.
  -----------------------------------------------------------------------*/
  ps_ppp_log_dev_config_param(configbuf_ptr);

  /*-------------------------------------------------------------------------
    Initialize configuration pointer and PPP control block pointer.
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)configbuf_ptr->device;
  config_ptr = &configbuf_ptr->dev_opts;
  device = configbuf_ptr->device;

  /*-------------------------------------------------------------------------
    Get the device session handle.  This is set in application context
  -------------------------------------------------------------------------*/
  session_handle = configbuf_ptr->session_handle;

  /*-------------------------------------------------------------------------
    Set the PPP control block session handle for this device
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  ppp_cb_ptr->session_handle = session_handle;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Verify that device has been started.
  -------------------------------------------------------------------------*/
  if(session_handle == 0)
  {
    LOG_MSG_INFO2_1("starti() for free dev %d", configbuf_ptr->device);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Copy passed common device and LCP configuration parameters if
      PPP device is in pppDEAD phase
    -----------------------------------------------------------------------*/
    if(pppDEAD == ppp_cb_ptr->phase)
    {
      pppi_dev_init( ppp_cb_ptr, config_ptr );
    }

    /*-----------------------------------------------------------------------
      Call fsm_open on LCP FSM.  fsm_open() handles administrative opens and
      handles the SM transitions
    -----------------------------------------------------------------------*/
    if(PPP_CP_MODE_ACTIVE  == config_ptr->lcp_info.mode ||
       PPP_CP_MODE_PASSIVE == config_ptr->lcp_info.mode)
    {
      fsm_open(ppp_cb_ptr->fsm + LCP);
    }

    /*-----------------------------------------------------------------------
      Copy passed IPCP configuration parameters if mode of IPCP configuration
      is not INVALID.
      If passed mode is ACTIVE or PASSIVE fsm_open() is called to handle
      SM transitions corresponding to an IPCP administrative open
    -----------------------------------------------------------------------*/
    if( PPP_CP_MODE_INVALID  != config_ptr->ipcp_info.mode )
    {
      /*-----------------------------------------------------------------------
        Stopping IPCP if FSM is in fsmSTOPPED state, this cleans ncp_mask & 
        other data structures if needed
      -----------------------------------------------------------------------*/
      if(fsmSTOPPED == ppp_cb_ptr->fsm[IPCP].state)
      {
        LOG_MSG_ERROR_0("FSM in fsmSTOPPED state, stopping IPCP");
        ipcp_stopping (ppp_cb_ptr->fsm + IPCP);
        PS_MEM_FREE( configbuf_ptr );
        return;
      }

      /*---------------------------------------------------------------------
        Set mode and PS iface pointer
      ---------------------------------------------------------------------*/
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      ppp_cb_ptr->fsm[IPCP].mode         =
        pppi_ncp_mode_change( ppp_cb_ptr->fsm[IPCP].mode,
                              config_ptr->ipcp_info.mode );
      ppp_cb_ptr->fsm[IPCP].rx_iface_ptr =
        config_ptr->ipcp_info.rx_iface_ptr;
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

      /*---------------------------------------------------------------------
        Initialize IPCP if current SM state is INITIAL
      ---------------------------------------------------------------------*/
      if( fsmINITIAL == ppp_cb_ptr->fsm[IPCP].state )
      {
        ipcp_init(ppp_cb_ptr->fsm[IPCP].device, config_ptr);
        fsm_init(ppp_cb_ptr->fsm + IPCP);
      }

      /*---------------------------------------------------------------------
        Call fsm_open() to handle FSM transitions.  Set ncp_mask again to
        avoid race condition
      ---------------------------------------------------------------------*/
      if( PPP_CP_MODE_ACTIVE  == config_ptr->ipcp_info.mode ||
          PPP_CP_MODE_PASSIVE == config_ptr->ipcp_info.mode )
      {
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        ppp_cb_ptr->ncp_mask |= PPP_PROTOCOL_IPCP_MASK;
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        fsm_open(ppp_cb_ptr->fsm + IPCP);
      }
    }

#ifdef FEATURE_DATA_PS_IPV6
    /*-----------------------------------------------------------------------
      Copy passed IPV6CP configuration parameters if mode of IPV6CP
      configuration is not INVALID.
      If passed mode is ACTIVE or PASSIVE fsm_open() is called to handle
      SM transitions corresponding to an IPV6CP administrative open
    -----------------------------------------------------------------------*/
    if( PPP_CP_MODE_INVALID  != config_ptr->ipv6cp_info.mode )
    {
      /*-----------------------------------------------------------------------
        Stopping IPv6CP if FSM is in fsmSTOPPED state, this cleans ncp_mask & 
        other data structures if needed
      -----------------------------------------------------------------------*/
      if(fsmSTOPPED == ppp_cb_ptr->fsm[IPV6CP].state)
      {
        LOG_MSG_ERROR_0("FSM in fsmSTOPPED state, stopping IPv6CP");
        ipv6cp_stopping (ppp_cb_ptr->fsm + IPV6CP);
        PS_MEM_FREE( configbuf_ptr );
        return;
      }
      
      /*---------------------------------------------------------------------
        Set mode and PS iface pointer
      ---------------------------------------------------------------------*/
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      ppp_cb_ptr->fsm[IPV6CP].mode         =
        pppi_ncp_mode_change( ppp_cb_ptr->fsm[IPV6CP].mode,
                              config_ptr->ipv6cp_info.mode );
      ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr =
        config_ptr->ipv6cp_info.rx_iface_ptr;
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

      /*---------------------------------------------------------------------
        Initialize IPV6CP if current SM state is INITIAL
      ---------------------------------------------------------------------*/
      if( fsmINITIAL == ppp_cb_ptr->fsm[IPV6CP].state )
      {
        ipv6cp_init(ppp_cb_ptr->fsm[IPV6CP].device, config_ptr);
        fsm_init(ppp_cb_ptr->fsm + IPV6CP);
      }

      /*---------------------------------------------------------------------
        Call fsm_open() to handle FSM transitions.  Set ncp_mask again to
        avoid race condition
      ---------------------------------------------------------------------*/
      if( PPP_CP_MODE_ACTIVE  == config_ptr->ipv6cp_info.mode ||
          PPP_CP_MODE_PASSIVE == config_ptr->ipv6cp_info.mode )
      {
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        ppp_cb_ptr->ncp_mask |= PPP_PROTOCOL_IPV6CP_MASK;
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        fsm_open(ppp_cb_ptr->fsm + IPV6CP);
      }
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Start PPP device including LCP if:
      *  LCP state is fsmSTARTING (i.e. it is initialized above)
    -----------------------------------------------------------------------*/
    if( fsmSTARTING == ppp_cb_ptr->fsm[LCP].state )
    {
      pppi_dev_start( ppp_cb_ptr );
    }

    /*-----------------------------------------------------------------------
      Start NCPs if:
      *  LCP state is fsmOPENED (i.e. LCP is already up)
      *  NCP is in fsmSTARTING state
      This is needed to transition from fsmSTARTING to fsmREQ_Sent state
    -----------------------------------------------------------------------*/
    if( fsmOPENED == ppp_cb_ptr->fsm[LCP].state )
    {
      /*---------------------------------------------------------------------
        Start IPCP if:
        *  IPCP state is fsmSTARTING (i.e. it is initialized above)
      ---------------------------------------------------------------------*/
      if( fsmSTARTING == ppp_cb_ptr->fsm[IPCP].state )
      {
        ipcp_start( ppp_cb_ptr );
      }

#ifdef FEATURE_DATA_PS_IPV6
      /*---------------------------------------------------------------------
        Start IPV6CP if:
        *  IPV6CP state is fsmSTARTING (i.e. it is initialized above)
      ---------------------------------------------------------------------*/
      if( fsmSTARTING == ppp_cb_ptr->fsm[IPV6CP].state )
      {
        ipv6cp_start( ppp_cb_ptr );
      }
#endif /* FEATURE_DATA_PS_IPV6 */
    } /* if( fsmOPENED == ppp_cb_ptr->fsm[LCP].state ) */
  }

  /*-------------------------------------------------------------------------
    Free configuration memory
  -------------------------------------------------------------------------*/
  PS_MEM_FREE( configbuf_ptr );

  return;
} /* pppi_start() */



/*===========================================================================
FUNCTION PPPI_RESYNC()

DESCRIPTION
  This function will resync any PPP protocols asked for that are already
  started.

PARAMETERS
  configbuf_ptr   Pointer to PPP configuration buffer (allocated from PS mem)

RETURN VALUE
  None

DEPENDENCIES
  The protocol requested to be resynced must have already been started

SIDE EFFECTS
  Initializes, if configuration passed, and resyncs any relevant PPP
  protocols.  Frees configuration buffer at the end
===========================================================================*/
void pppi_resync
(
  ppp_config_buffer_s_type *configbuf_ptr
)
{
  ppp_type            *ppp_cb_ptr;
  ppp_type            *ppp_cb_array = NULL;
  ppp_dev_opts_type   *config_ptr;
  ppp_protocol_e_type  protocol;
  uint32               session_handle;
  boolean              start_ncp;
  int                  i;
  pppi_mode_info_type  ppp_mode_info;
  ppp_dev_enum_type    device;
  ncpi_cb_type        *ncp_handle_ptr = NULL;
  uint8                index = 0;
  ppp_fsm_type        *ncp_fsm_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify passed in arguments.
  -------------------------------------------------------------------------*/
  if( NULL == configbuf_ptr )
  {
    DATA_ERR_FATAL("NULL configbuf_ptr passed");
    return;
  }

  if( PPP_MAX_DEV <= configbuf_ptr->device )
  {
    LOG_MSG_ERROR_1( "Invalid device %d", configbuf_ptr->device );
    ASSERT( 0 );
    return;
  }

  /*-----------------------------------------------------------------------
      Set the appropriate parameters in the configuration log packet
      and call the API to log to the packet.
  -----------------------------------------------------------------------*/
  ps_ppp_log_dev_config_param(configbuf_ptr);

  /*-------------------------------------------------------------------------
    Initialize configuration pointer, PPP control block pointer and protocol
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)configbuf_ptr->device;
  protocol   = configbuf_ptr->resync_protocol;
  config_ptr = (FALSE == configbuf_ptr->dev_opts_null) ?
                 &configbuf_ptr->dev_opts : NULL;
  device     = configbuf_ptr->device;

  /*-------------------------------------------------------------------------
    Get the device session handle.  This is set in application context
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  session_handle = ppp_cb_ptr->session_handle;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Verify that device has been started.
  -------------------------------------------------------------------------*/
  if(session_handle == 0)
  {
    LOG_MSG_INFO2_1("resynci() for free dev %d", configbuf_ptr->device);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If any protocols to be resynced were previously passive, set them
      to active now since the caller actually wants a resync to occur NOW.
    -----------------------------------------------------------------------*/
    for(i = LCP; i < fsmi_Size; i++)
    {
      if(ppp_cb_ptr->fsm[i].mode == PPP_CP_MODE_PASSIVE)
      {
        ppp_cb_ptr->fsm[i].mode = PPP_CP_MODE_ACTIVE;
      }
    }
    /*-----------------------------------------------------------------------
      If an NCP is being resynced and it is the only NCP also resync LCP
      Note: we need to ignore the linger bit as well as the bit for the NCP
            being resynced to ensure that it is not counted as an NCP.
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    if( PPP_PROTOCOL_LCP != protocol &&
        0 == (ppp_cb_ptr->ncp_mask &
               ~(uint16)( ( 1 << (uint8)protocol ) | PPP_PROTOCOL_LCP_LINGER_MASK ) ) )
    {
      protocol = PPP_PROTOCOL_LCP;
    }
    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    /*-----------------------------------------------------------------------
      Initialize device including LCP if passed protocol is LCP.
      *  set device phase to pppDEAD
      *  set device mode to PPP_INTERNAL_MODE
      *  Initialize LCP parameters

      After initialization LCP state transitions from fsmINITIAL to
      fsmSTARTING
    -----------------------------------------------------------------------*/
    if( PPP_PROTOCOL_LCP == protocol )
    {
      ppp_cb_ptr->phase = pppDEAD;

      ppp_mode_info.device = (uint16)configbuf_ptr->device;
      ppp_mode_info.mode   = (uint16)PPP_INTERNAL_MODE;
      pppi_set_mode( ppp_mode_info );

      pppi_dev_init( ppp_cb_ptr, config_ptr );

#ifdef FEATURE_DATA_PS_IPV6
      /*---------------------------------------------------------------------
        Call pppi_failover_on_resync() to perform failover logic
      ---------------------------------------------------------------------*/
      pppi_failover_on_resync( configbuf_ptr->device, PPP_PROTOCOL_LCP );
#endif /*FEATURE_DATA_PS_IPV6*/

      /*---------------------------------------------------------------------
        Call fsm_open on LCP FSM.  fsm_open() handles the SM transitions
      ---------------------------------------------------------------------*/
      fsm_open(ppp_cb_ptr->fsm + LCP);

      /*---------------------------------------------------------------------
        Generate a PS_PPP_EVENT_RESYNC event
      ---------------------------------------------------------------------*/
      pppi_report_event_to_diag( EVENT_PPP,
                                 configbuf_ptr->device,
                                 PPP_PROTOCOL_LCP,
                                 PS_PPP_EVENT_UNKNOWN,
                                 PS_PPP_EVENT_RESYNC );
    }

    /*-----------------------------------------------------------------------
      Initialize IPCP if either:
      *  Passed protocol is IPCP or LCP
      *  IPCP state is not fsmINITIAL i.e. it has at least been already
         initialized

      As a result of FSM opening IPCP transitions to fsmSTARTING
    -----------------------------------------------------------------------*/
    if( (PPP_PROTOCOL_IPCP == protocol || PPP_PROTOCOL_LCP == protocol) &&
        fsmINITIAL != ppp_cb_ptr->fsm[IPCP].state )
    {
      if( NULL != config_ptr )
      {
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        ppp_cb_ptr->fsm[IPCP].mode         = config_ptr->ipcp_info.mode;
        ppp_cb_ptr->fsm[IPCP].rx_iface_ptr =
          config_ptr->ipcp_info.rx_iface_ptr;
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      }

      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      start_ncp = (PPP_CP_MODE_ACTIVE  == ppp_cb_ptr->fsm[IPCP].mode ||
                   PPP_CP_MODE_PASSIVE == ppp_cb_ptr->fsm[IPCP].mode ) ?
                   TRUE : FALSE;
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

      /*---------------------------------------------------------------------
        Check mode again in case the passed configuration changes it
      ---------------------------------------------------------------------*/
      if( TRUE == start_ncp )
      {
        if( NULL != config_ptr )
        {
          /*-----------------------------------------------------------------
            JCDMA requires that DNS options should not be negotiated during
            a resync, so if a config is passed for IPCP and DNS options have
            already been pre-negotiated, then clear the DNS options
          -----------------------------------------------------------------*/
          if( ( config_ptr->resync_dns_opts == FALSE ) &&
            !( ppp_cb_ptr->fsm[IPCP].local.want & IPCP_N_PRIMARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].local.want & IPCP_N_SECONDARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].local.will & IPCP_N_PRIMARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].local.will & IPCP_N_SECONDARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].remote.want & IPCP_N_PRIMARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].remote.want & IPCP_N_SECONDARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].remote.will & IPCP_N_PRIMARY_DNS ) &&
            !( ppp_cb_ptr->fsm[IPCP].remote.will & IPCP_N_SECONDARY_DNS )
          )
          {
            LOG_MSG_INFO1_0("pppi_resync: Disabling DNS option in IPCP");
            /*---------------------------------------------------------------
              Disable primary DNS option.
            ---------------------------------------------------------------*/
            config_ptr->ipcp_info.will_mask[FSM_REMOTE] &=
                                                         ~IPCP_N_PRIMARY_DNS;
            config_ptr->ipcp_info.want_mask[FSM_REMOTE] &=
                                                         ~IPCP_N_PRIMARY_DNS;
            config_ptr->ipcp_info.will_mask[FSM_LOCAL]  &=
                                                         ~IPCP_N_PRIMARY_DNS;
            config_ptr->ipcp_info.want_mask[FSM_LOCAL]  &=
                                                         ~IPCP_N_PRIMARY_DNS;

            /*---------------------------------------------------------------
              Disable secondary DNS option.
            ---------------------------------------------------------------*/
            config_ptr->ipcp_info.will_mask[FSM_REMOTE] &=
                                                       ~IPCP_N_SECONDARY_DNS;
            config_ptr->ipcp_info.want_mask[FSM_REMOTE] &=
                                                       ~IPCP_N_SECONDARY_DNS;
            config_ptr->ipcp_info.will_mask[FSM_LOCAL]  &=
                                                       ~IPCP_N_SECONDARY_DNS;
            config_ptr->ipcp_info.want_mask[FSM_LOCAL]  &=
                                                       ~IPCP_N_SECONDARY_DNS;
          }
          ipcp_init(ppp_cb_ptr->fsm[IPCP].device, config_ptr);
        }
        fsm_init(ppp_cb_ptr->fsm + IPCP);
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        ppp_cb_ptr->ncp_mask |= PPP_PROTOCOL_IPCP_MASK;
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        fsm_open(ppp_cb_ptr->fsm + IPCP);

        /*--------------------------------------------------------------------
          Generate a PS_PPP_EVENT_RESYNC event
        --------------------------------------------------------------------*/
        pppi_report_event_to_diag( EVENT_PPP,
                                   configbuf_ptr->device,
                                   PPP_PROTOCOL_IPCP,
                                   PS_PPP_EVENT_UNKNOWN,
                                   PS_PPP_EVENT_RESYNC );
      }
    }

#ifdef FEATURE_DATA_PS_IPV6
    /*-----------------------------------------------------------------------
      Initialize IPV6CP if either:
      *  Passed protocol is IPV6CP or LCP
      *  IPV6CP mode is ACTIVE.  Currently any PASSIVE NCP is being set to
         ACTIVE in application context.

      As a result of FSM opening IPV6CP transitions to fsmSTARTING
    -----------------------------------------------------------------------*/
    if( (PPP_PROTOCOL_IPV6CP == protocol || PPP_PROTOCOL_LCP == protocol) &&
        PPP_CP_MODE_ACTIVE == ppp_cb_ptr->fsm[IPV6CP].mode )
    {
      if( NULL != config_ptr )
      {
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        ppp_cb_ptr->fsm[IPV6CP].mode         = config_ptr->ipv6cp_info.mode;
        ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr =
          config_ptr->ipv6cp_info.rx_iface_ptr;
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      }

      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      start_ncp = (PPP_CP_MODE_ACTIVE  == ppp_cb_ptr->fsm[IPV6CP].mode ||
                   PPP_CP_MODE_PASSIVE == ppp_cb_ptr->fsm[IPV6CP].mode ) ?
                   TRUE : FALSE;
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

      /*---------------------------------------------------------------------
        Check mode again in case the passed configuration changes it
      ---------------------------------------------------------------------*/
      if( TRUE == start_ncp )
      {
        if( NULL != config_ptr )
        {
          ipv6cp_init(ppp_cb_ptr->fsm[IPV6CP].device, config_ptr);
        }
        fsm_init(ppp_cb_ptr->fsm + IPV6CP);
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        ppp_cb_ptr->ncp_mask |= PPP_PROTOCOL_IPCP_MASK;
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        fsm_open(ppp_cb_ptr->fsm + IPV6CP);

        /*--------------------------------------------------------------------
          Generate a PS_PPP_EVENT_RESYNC event
        --------------------------------------------------------------------*/
        pppi_report_event_to_diag( EVENT_PPP,
                                   configbuf_ptr->device,
                                   PPP_PROTOCOL_IPV6CP,
                                   PS_PPP_EVENT_UNKNOWN,
                                   PS_PPP_EVENT_RESYNC );
      }
    }
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_EHRPD
    /*-----------------------------------------------------------------------
      For eHRPD case, just tear down all the existing NCPs 
      The NCPs can be started once the EAP authentication succeeds 
      which is mandatory for eHRPD
      The configuration paramteres will be NULL for eHRPD 
    ----------------------------------------------------------------------*/
    if((TRUE == configbuf_ptr->dev_opts_null) &&
       (PPP_PROTOCOL_LCP == protocol ))
    {
      /*---------------------------------------------------------------
        Loop into ps_mem to find the corresponding NCP control block.
        Call the corresponding down function of each ncp.
      -----------------------------------------------------------------*/
      for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
      {
        ncp_handle_ptr = 
          (ncpi_cb_type *)( ps_mem_index_to_buf( index,
                                             PS_MEM_PPP_NCP_CB_TYPE ) );
        if(( NCPI_CB_IS_INITED( ncp_handle_ptr ) == TRUE ) &&
           ( ncp_handle_ptr->device == device ))
        {
          switch (ncp_handle_ptr->ncp)
          {
            case PPP_NCP_VSNCP_3GPP2:
              ncp_fsm_ptr = &ncp_handle_ptr->cb_u.vsncp_3gpp2.fsm;
              if((TRUE == VSNCP_3GPP2I_FSM_IS_VALID(ncp_fsm_ptr)) && 
                 (fsmINITIAL != ncp_fsm_ptr->state))
              {
                fsm_init(ncp_fsm_ptr);
                fsm_open(ncp_fsm_ptr);
                /*-----------------------------------------------------
                  Generate a PS_PPP_EVENT_RESYNC event
                -----------------------------------------------------*/
                pppi_report_event_to_diag( EVENT_PPP,
                                           device,
                                           PPP_PROTOCOL_VSNCP,
                                           PS_PPP_EVENT_UNKNOWN,
                                           PS_PPP_EVENT_RESYNC );               
              }
              break;

            case PPP_NCP_IPCP:
            case PPP_NCP_IPV6CP:
            default:
              /* unexpected! */
              LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_handle_ptr->ncp);
             break;
           }/* switch (ncp) */      
         }/* if (NCPI_CB_IS_INITED) */
       }/* for(PS_PPP_NCP_CB_NUM) */
    }
#endif

    /*-----------------------------------------------------------------------
      Start PPP device including LCP if:
      *  LCP being resynced
    -----------------------------------------------------------------------*/
    if( PPP_PROTOCOL_LCP == protocol )
    {
      pppi_dev_start( ppp_cb_ptr );
    }

    /*-----------------------------------------------------------------------
      Start NCPs if:
      *  LCP already up
      *  NCP is in fsmSTARTING state
      This is needed to transition from fsmSTARTING to fsmREQ_Sent state
    -----------------------------------------------------------------------*/
    if( fsmOPENED == ppp_cb_ptr->fsm[LCP].state )
    {
      /*---------------------------------------------------------------------
        Start IPCP if:
        *  IPCP state is fsmSTARTING due to the initialization above
      ---------------------------------------------------------------------*/
      if( fsmSTARTING == ppp_cb_ptr->fsm[IPCP].state )
      {
        ipcp_start( ppp_cb_ptr );
      }

#ifdef FEATURE_DATA_PS_IPV6
      /*---------------------------------------------------------------------
        Start IPV6CP if:
        *  IPV6CP state is fsmSTARTING due to the initialization above
      ---------------------------------------------------------------------*/
      if( fsmSTARTING == ppp_cb_ptr->fsm[IPV6CP].state )
      {
        ipv6cp_start( ppp_cb_ptr );
      }
#endif /* FEATURE_DATA_PS_IPV6 */               
    } /* if(fsmOPENED == ppp_cb_ptr->fsm[LCP].state) */
  }

  /*-------------------------------------------------------------------------
    Free configuration memory
  -------------------------------------------------------------------------*/
  PS_MEM_FREE( configbuf_ptr );

  return;
} /* pppi_resync() */



/*===========================================================================
FUNCTION PPPI_STOP()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_stop
(
  pppi_stop_info_type stop_info
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  boolean   stop = TRUE;
  pppi_mode_info_type  ppp_mode_info;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(stop_info.device >= (uint16)PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("Invalid device %d!", stop_info.device);
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + stop_info.device;

  if(fsmi_Size <= stop_info.protocol)
  {
    LOG_MSG_ERROR_1 ("Invalid stop_info.protocol = %d", stop_info.protocol);
    return;
  }

  /*-------------------------------------------------------------------------
    If the device is invalid then don't do anything
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("stopi() for free dev %d", stop_info.device);
    stop = FALSE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Put device in internal mode before initiating ppp closure.
    -----------------------------------------------------------------------*/
    ppp_mode_info.device = stop_info.device;
    ppp_mode_info.mode   = (uint16)PPP_INTERNAL_MODE;
    pppi_set_mode( ppp_mode_info );
    ppp_cb_ptr->fsm[stop_info.protocol].rx_iface_ptr = NULL;
    if( IPCP ==  stop_info.protocol &&
        PPP_CP_MODE_DISABLED != ppp_cb_ptr->fsm[IPCP].mode &&
        PPP_CP_MODE_INVALID != ppp_cb_ptr->fsm[IPCP].mode )
    {
      snoop_ip_post_event( (ppp_dev_enum_type)(stop_info.device),
                           SNOOP_IP_PPP_DEV_DOWN_EV );
    }
  }

  /*-------------------------------------------------------------------------
    If we are to stop - close the FSM
  -------------------------------------------------------------------------*/
  if(stop == TRUE)
  {
    fsm_close(&ppp_cb_ptr->fsm[stop_info.protocol]);

    PS_ENTER_CRIT_SECTION(
	   pppi_get_crit_sect_lock( (ppp_dev_enum_type)stop_info.device) );
    if( fsmCLOSING != ppp_cb_ptr->fsm[stop_info.protocol].state )
    {
      ppp_cb_ptr->fsm[stop_info.protocol].mode = PPP_CP_MODE_INVALID;
    }
    PS_LEAVE_CRIT_SECTION(
	   pppi_get_crit_sect_lock( (ppp_dev_enum_type)stop_info.device) );
  }

} /* pppi_stop() */



/*===========================================================================
FUNCTION PPPI_ABORT()

DESCRIPTION
  This function will clean up after PPP is finished.

DEPENDENCIES
  It should only ever be called from the command (i.e. via task command
  interface) and not directly by any other piece of code).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_abort
(
  pppi_abort_info_type abort_info
)
{
  ppp_type               *ppp_cb_ptr;
  ppp_type               *ppp_cb_array = NULL;
  pppi_mode_info_type     ppp_mode_info;
  ppp_dev_enum_type       device;
  boolean    call_ipcp_cback   = FALSE; /* should IPCP cbacks be called ?  */
#ifdef FEATURE_DATA_PS_IPV6
  boolean    call_ipv6cp_cback = FALSE; /* should IPV6CP cbacks be called ?*/
#endif /* FEATURE_DATA_PS_IPV6 */
  uint8                   index = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(abort_info.device >= (uint16)PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid device %d!", abort_info.device);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Ignore abort request if called on already free device.
  -------------------------------------------------------------------------*/
  if(abort_info.session_handle == 0)
  {
    LOG_MSG_INFO2_1("Ignoring, pppi_abort(%d) on free device!",
                    abort_info.device);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + abort_info.device;
  device     = (ppp_dev_enum_type)abort_info.device;

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    Abort all NCPs
  -------------------------------------------------------------------------*/
  ncpi_abort_all_ncps ( (ppp_dev_enum_type)abort_info.device );
#endif /* FEATURE_DATA_PS_EHRPD */

  /*-------------------------------------------------------------------------
    NCP callbacks should be called if the NCP is active
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->fsm[IPCP].state >= fsmREQ_Sent &&
      ppp_cb_ptr->phase == pppNETWORK )
  {
    call_ipcp_cback = TRUE;
  }

#ifdef FEATURE_DATA_PS_IPV6
  if( ppp_cb_ptr->fsm[IPV6CP].state >= fsmREQ_Sent &&
      ppp_cb_ptr->phase == pppNETWORK )
  {
    call_ipv6cp_cback = TRUE;
  }
#endif /* FEATURE_DATA_PS_IPV6 */


#ifdef FEATURE_DATA_PS_PPP_EAP
  if(ppp_cb_ptr->auth.ps_eap_handle != EAP_INVALID_HANDLE)
  {
    eap_delete_instance(ppp_cb_ptr->auth.ps_eap_handle);
    ppp_cb_ptr->auth.ps_eap_handle = EAP_INVALID_HANDLE;
  }
#endif /*FEATURE_DATA_PS_PPP_EAP*/

  /*-------------------------------------------------------------------------
    Disable the RX signal, make sure all of the timers are cancelled - and
    reinitialize the sw deframer
  -------------------------------------------------------------------------*/
  PPP_DISABLE_SIG(ppp_cb_ptr->rx_signal);
  (void)ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);
  (void)ps_timer_cancel(ppp_cb_ptr->fsm[IPCP].timer);
#ifdef FEATURE_DATA_PS_IPV6
  (void)ps_timer_cancel(ppp_cb_ptr->fsm[IPV6CP].timer);
#endif
  (void)ps_timer_cancel(ppp_cb_ptr->setup_timer);
  pppi_pseudonet_reset(device);

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  ppp_cb_ptr->fsm[LCP].state  = fsmINITIAL;
  ppp_cb_ptr->fsm[IPCP].state = fsmINITIAL;
#ifdef FEATURE_DATA_PS_IPV6
  ppp_cb_ptr->fsm[IPV6CP].state = fsmINITIAL;
#endif /* FEATURE_DATA_PS_IPV6 */
  ppp_cb_ptr->ncp_mask        = 0;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  if( PPP_CP_MODE_DISABLED != ppp_cb_ptr->fsm[IPCP].mode &&
      PPP_CP_MODE_INVALID != ppp_cb_ptr->fsm[IPCP].mode )
  {
    snoop_ip_post_event( device,
                         SNOOP_IP_PPP_DEV_DOWN_EV );
  }

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device));
  ppp_cb_ptr->phase                   = pppDEAD;
  ppp_mode_info.device                = abort_info.device;
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
  if(ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS)
  {
    pppi_a2_enable_hdlc_autonomous(LCP_ACCM_DEFAULT, 
                                   FALSE);
    ppp_hdlc_a2_autonomous_enable = FALSE;
  }
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */

  ppp_mode_info.mode                  = (uint16)PPP_NONE_MODE;
  pppi_set_mode( ppp_mode_info );
  ppp_cb_ptr->pending_mode            = PPP_INVALID_MODE;
  ppp_cb_ptr->hdlc_mode               = HDLC_INVALID_MODE;
  ppp_cb_ptr->bridge_dev              = PPP_INVALID_DEV;
  ppp_cb_ptr->session_handle          = 0;
  ppp_cb_ptr->fsm[IPCP].rx_iface_ptr  = NULL;
 
  for( index = 0 ; index < PPP_OUI_MAX ; index++ )
  {
    ppp_cb_ptr->vendor_cb_array[index].oui         = 0;
    ppp_cb_ptr->vendor_cb_array[index].input_f_ptr = NULL;
  } /* for */

#ifdef FEATURE_DATA_PS_IPV6
  ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr = NULL;
#endif /* FEATURE_DATA_PS_IPV6 */
  ppp_cb_ptr->fsm[LCP].mode           = PPP_CP_MODE_INVALID;
  ppp_cb_ptr->fsm[IPCP].mode          = PPP_CP_MODE_INVALID;
#ifdef FEATURE_DATA_PS_IPV6
  ppp_cb_ptr->fsm[IPV6CP].mode        = PPP_CP_MODE_INVALID;
#endif /* FEATURE_DATA_PS_IPV6 */
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  ASSERT( ppp_cb_ptr->hdlc_handle != NULL );
  if( hdlc_reset( ppp_cb_ptr->hdlc_handle ) == -1 )
  {
    LOG_MSG_ERROR_1("Error in resetting HDLC instance 0x%x",
                    ppp_cb_ptr->hdlc_handle);
  }

  /*-------------------------------------------------------------------------
    Free DSM item containing chap respone
  -------------------------------------------------------------------------*/
  if (NULL != ppp_cb_ptr->auth.chap_challenge_ptr)
  {
    dsm_free_packet( &ppp_cb_ptr->auth.chap_challenge_ptr );
  }

  /*-------------------------------------------------------------------------
    Call event callbacks for PPP_DOWN_EV, for any active NCPs and for LCP
  -------------------------------------------------------------------------*/
  if( call_ipcp_cback == TRUE )
  {
    pppi_call_event_callback( &ppp_cb_ptr->fsm[IPCP],
                              PPP_PROTOCOL_IPCP,
                              PPP_DOWN_EV );
  }
#ifdef FEATURE_DATA_PS_IPV6
  if( call_ipv6cp_cback == TRUE )
  {
    pppi_call_event_callback( &ppp_cb_ptr->fsm[IPV6CP],
                              PPP_PROTOCOL_IPV6CP,
                              PPP_DOWN_EV );
  }
#endif /* FEATURE_DATA_PS_IPV6 */
  pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                            PPP_PROTOCOL_LCP,
                            PPP_DOWN_EV );

  /*-------------------------------------------------------------------------
    Report PS_PPP_EVENT_CLOSED event for both IPCP and LCP
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             (ppp_dev_enum_type)(abort_info.device),
                             PPP_PROTOCOL_LCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );
  pppi_report_event_to_diag( EVENT_PPP,
                             (ppp_dev_enum_type)(abort_info.device),
                             PPP_PROTOCOL_IPCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );
#ifdef FEATURE_DATA_PS_IPV6
  pppi_report_event_to_diag( EVENT_PPP,
                             (ppp_dev_enum_type)(abort_info.device),
                             PPP_PROTOCOL_IPV6CP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_CLOSED );
#endif
} /* pppi_abort() */



/*===========================================================================
FUNCTION PPPI_SET_MODE()

DESCRIPTION
  This function is used by PS task to set the PPP mode.
  The table below describes the allowed transitions between PPP modes.
  Note (1) and (2) below for the mechanism to effect a change of mode
  between "snooping" (early-protocol-detection) and "non-snooping" modes.

  PPP mode switch combinations
  ============================

  Old modes are vertical, new modes horizontal.
  "Internal" means any one of PPP_INTERNAL_MODE, PPP_INTERNAL_SNOOP_MODE or
  PPP_FULL_NET_MODE modes. These are the "non-snooping" modes.

  +------------+--------+--------+-----------+------------+
  | PPP Mode   |  NONE  | RELAY  | PSEUDONET | "Internal" |
  +------------+--------+--------+-----------+------------+
  | NONE       |  YES   |  YES   |  YES      |  YES       |
  +------------+--------+--------+-----------+------------+
  | RELAY      |  YES   |  YES   |  NO       |  NO        |
  +------------+--------+--------+-----------+------------+
  | PSEUDONET  |  YES   |  NO    |  YES      |  YES(1)    |
  +------------+--------+--------+-----------+------------+
  | "Internal" |  YES   |  NO    |  YES(2)   |  YES       |
  +------------+--------+--------+-----------+------------+

  (1) Switch from pseudonet mode to an "internal" mode
      ------------------------------------------------
    - Uses pending_mode variable in PPP control block.
    - In pppi_set_mode, pending_mode is set to the mode to switch to.
    - In FINDING_SOP state of pseudonet state machine, this variable is
      examined per-packet.
    - HDLC library state machine is put into the FINDING_SOP state.
    - HDLC mode is switched to hardware.
    - The mode is switched to the pending one, rx_pkt_hdlr_f_ptr is set to
      pppi_internal_mode_hdlr and it is invoked with the data received.
    - The pending mode is set to invalid.

  (2) Switch from an "internal" mode to pseudonet mode
      ------------------------------------------------
    - Happens in the control path
    - HDLC mode is switched to software.
    - HDLC state machine is queried to find current state
      (FINDING_7E, FINDING_SOP or UNFRAMING)
    - Pseudonet state machine is put into the same state.
    - mode is set to pseudonet and rx_pkt_hdlr_f_ptr is set to
      pppi_pseudonet_mode_hdlr

PARAMETERS
  mode_info: parameters passed through the cmd required to set mode

RETURN VALUE
  None

DEPENDENCIES
  This function should always be executed in PS task context

SIDE EFFECTS
  None
===========================================================================*/
void pppi_set_mode
(
  pppi_mode_info_type mode_info
)
{
  ppp_type             * ppp_cb_ptr;
  ppp_type             * ppp_cb_array = NULL;
  ppp_mode_enum_type     old_mode;
  hdlc_ioctl_u_type      ioctl_data;
  hdlc_errno_enum_type   ps_errno;
  hdlc_instance_type     *instance = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mode_info.device >= (uint16)PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("Invalid device %d!", mode_info.device);
    ASSERT(0);
    return;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + mode_info.device;
  old_mode   = ppp_cb_ptr->mode;
  instance   = (hdlc_instance_type*)(ppp_cb_ptr->hdlc_handle);


  LOG_MSG_INFO2_3("Entering pppi_set_mode(dev %d), old_mode = 0x%x, new_mode = 0x%x",
           mode_info.device, old_mode, mode_info.mode );

  if( ( (uint16)PPP_PSEUDO_NET_MODE == mode_info.mode ||
        (uint16)PPP_FULL_NET_MODE == mode_info.mode ) &&
      ppp_cb_ptr->bridge_dev >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_2("PPP %d could not set mode %d, no bridge dev",
                    mode_info.device, mode_info.mode );
    ASSERT(0);
  }

  ASSERT( PPP_IS_CURR_TASK_OWNER() );
  switch( mode_info.mode )
  {
    case PPP_RELAY_MODE:
      switch( old_mode )
      {
        case PPP_NONE_MODE:
        case PPP_RELAY_MODE:
          PS_ENTER_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          ppp_cb_ptr->mode              = PPP_RELAY_MODE;
          ppp_cb_ptr->rx_pkt_hdlr_f_ptr = pppi_relay_mode_hdlr;
          PS_LEAVE_CRIT_SECTION( 
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          break;

        case PPP_PSEUDO_NET_MODE:
        case PPP_INTERNAL_SNOOP_MODE:
        case PPP_INTERNAL_MODE:
        case PPP_FULL_NET_MODE:
        default:
          LOG_MSG_ERROR_3("Invalid PPP mode transition, dev (%d), mode (%d) to "
                    " (%d)", mode_info.device, old_mode, mode_info.mode);
          ASSERT(0);
          return;
      } /* switch( old_mode ) */
      break; /* PPP_RELAY_MODE */

    case PPP_PSEUDO_NET_MODE:
      /*---------------------------------------------------------------------
        Set HDLC mode to SW. If the hdlc_ioctl() call returns an error,
        ASSERT because pseudonet REQUIRES software.
      ---------------------------------------------------------------------*/
      ioctl_data.hdlc_mode = HDLC_SW;
      instance->sw_mode_required = TRUE;
      if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                      HDLC_IOCTL_SET_HDLC_MODE,
                      &ioctl_data,
                      &ps_errno
                    ) != 0 )
      {
        LOG_MSG_ERROR_1("PPP dev %d, unable to set HDLC mode to software",
                        mode_info.device);
        ASSERT( 0 );
        return;
      }

      switch( old_mode )
      {
        case PPP_NONE_MODE:
        case PPP_PSEUDO_NET_MODE:
          PS_ENTER_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          ppp_cb_ptr->mode              = PPP_PSEUDO_NET_MODE;
          ppp_cb_ptr->pending_mode      = PPP_INVALID_MODE;
          ppp_cb_ptr->rx_pkt_hdlr_f_ptr = pppi_pseudonet_mode_hdlr;
          PS_LEAVE_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          break;

        case PPP_RELAY_MODE:
          LOG_MSG_ERROR_3("Invalid PPP mode transition, dev (%d), mode (%d) to "
                    " (%d)", mode_info.device, old_mode, mode_info.mode);
          ASSERT(0);
          return;

        case PPP_INTERNAL_SNOOP_MODE:
        case PPP_INTERNAL_MODE:
        case PPP_FULL_NET_MODE:
          if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                          HDLC_IOCTL_GET_UNFRAMER_STATE,
                          &ioctl_data,
                          &ps_errno
                        ) != 0 )
          {
            LOG_MSG_ERROR_1("PPP dev %d, unable to get HDLC unframer state",
                            mode_info.device);
            return;
          }

          ASSERT( ioctl_data.unframe_state == FINDING_7E  ||
                  ioctl_data.unframe_state == FINDING_SOP ||
                  ioctl_data.unframe_state == UNFRAMING );
          pppi_pseudonet_reset( (ppp_dev_enum_type)(mode_info.device) );
          PS_ENTER_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          ppp_cb_ptr->pseudonet.state   = ioctl_data.unframe_state;
          ppp_cb_ptr->mode              = PPP_PSEUDO_NET_MODE;
          ppp_cb_ptr->pending_mode      = PPP_INVALID_MODE;
          ppp_cb_ptr->rx_pkt_hdlr_f_ptr = pppi_pseudonet_mode_hdlr;
          PS_LEAVE_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          LOG_MSG_INFO2_2("PPP dev %d switched to pseudonet, state 0x%x",
                          mode_info.device, ioctl_data.unframe_state);
          break;

        default:
          LOG_MSG_ERROR_3("Invalid PPP mode transition, dev (%d), mode (%d) to (%d)",
                    mode_info.device, old_mode, mode_info.mode);
          ASSERT(0);
          return;
      } /* switch( old_mode ) */
      break; /* PPP_PSEUDO_NET_MODE */

    case PPP_FULL_NET_MODE:
    case PPP_INTERNAL_SNOOP_MODE:
    case PPP_INTERNAL_MODE:
      switch( old_mode )
      {
        case PPP_NONE_MODE:
        case PPP_FULL_NET_MODE:
        case PPP_INTERNAL_SNOOP_MODE:
        case PPP_INTERNAL_MODE:
          /*-----------------------------------------------------------------
            Set HDLC mode to HW. If the hdlc_ioctl() call returns an error,
            don't fail as hardware is "desirable", not necessary.
          -----------------------------------------------------------------*/
          instance->sw_mode_required = FALSE;
          if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                          HDLC_IOCTL_SET_HDLC_MODE,
                          &ioctl_data,
                          &ps_errno
                        ) != 0 )
          {
            LOG_MSG_INFO2_1("PPP dev %d, unable to set HDLC mode to hardware",
                            mode_info.device);
          }

          PS_ENTER_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          ppp_cb_ptr->mode = (ppp_mode_enum_type)(mode_info.mode);
          ppp_cb_ptr->rx_pkt_hdlr_f_ptr = pppi_internal_mode_hdlr;
          PS_LEAVE_CRIT_SECTION(
             pppi_get_crit_sect_lock( (ppp_dev_enum_type)mode_info.device) );
          break;

        case PPP_RELAY_MODE:
          LOG_MSG_ERROR_3("Invalid PPP mode transition, dev (%d), mode (%d) to "
                    " (%d)", mode_info.device, old_mode, mode_info.mode);
          ASSERT(0);
          return;

        case PPP_PSEUDO_NET_MODE:
          /*-----------------------------------------------------------------
            Just set the pending mode. The actual mode switch will happen
            in pppi_pseudonet_mode_hdlr() once the current packet has been
            processed completely.
          -----------------------------------------------------------------*/
          LOG_MSG_INFO2_3("PPP dev (%d) mode change pending, (0x%x) --> (0x%x)",
                   mode_info.device, old_mode, mode_info.mode);
          ppp_cb_ptr->pending_mode = (ppp_mode_enum_type)(mode_info.mode);
          break;

        default:
          LOG_MSG_ERROR_3("Invalid PPP mode transition, dev (%d), mode (%d) to (%d)",
              mode_info.device, old_mode, mode_info.mode);
          ASSERT(0);
          return;
      } /* switch( old_mode ) */
      break; /* An "internal" mode */

    case PPP_NONE_MODE:
      /*---------------------------------------------------------------------
        Can go from any mode to PPP_NONE_MODE
      ---------------------------------------------------------------------*/
      ppp_cb_ptr->mode              = PPP_NONE_MODE;
      ppp_cb_ptr->rx_pkt_hdlr_f_ptr = pppi_none_mode_hdlr;
      break;

    default:
      LOG_MSG_ERROR_3("Invalid PPP mode requested, dev(%d), old_mode(%d) "
                      "new_mode(%d)",
                      mode_info.device, old_mode, mode_info.mode);
      break;
  } /* switch( mode_info.mode ) */

  LOG_MSG_INFO2_3("Leaving pppi_set_mode(dev %d), old_mode = 0x%x, new_mode = 0x%x",
           mode_info.device, old_mode, ppp_cb_ptr->mode );
} /* pppi_set_mode() */



/*===========================================================================
                                OUTBOUND DATA
===========================================================================*/
/*===========================================================================
FUNCTION PPPI_OUTGOING()

DESCRIPTION
  This function will process the passed datagram into a PPP packet for TX.

  Input Conditions:

  1. device specifies the device( Rm or Um) where the PPP pkt. is to be sent.

  2. The protocol argument specifies the protocol of this packet.

     This field can be:

     PPP_IP_PROTOCOL:    For IP datagrams. An attempt will be made to
                         compress the protocol header if compression enabled
     PPP_LCP_PROTOCOL: For LCP proto.
     PPP_IPCP_PROTOCOL: For IPCP
     PPP_PAP_PROTOCOL: For PAP.
     PPP_VJCH_PROTOCOL: For VJ Compressed pkts.
     PPP_VJUCH_PROTOCOL: For VJ Uncompressed pkts.

  3. For all packets the address and control
     fields(FF03) and the protocol fields should be stripped.

   Output Conditions:
     This function takes care to free the passed packet chain even if there
     is a error. If there is no error, it passes the packet chain to the
     lower layer.

     Depending on the hdlc_mode for the interface, it would either use the
     hardware HDLC framer or do the framing in software.

DEPENDENCIES
  None

RETURN VALUE
  -1 on error.
   0 on success.

SIDE EFFECTS
  None
===========================================================================*/
int pppi_outgoing
(
  ppp_dev_enum_type     device,                 /* device to be sent on    */
  uint16                protocol,               /* protocol of packet      */
  dsm_item_type       **incoming_pkt_ptr,       /* ptr to PPP packet chain */
  ps_tx_meta_info_type *meta_info_ptr           /* ptr to meta info        */
)
{
  ppp_type       *ppp_cb_ptr = NULL;            /* PPP ctrl block ptr      */
  ppp_type       *ppp_cb_array = NULL;
  dsm_item_type  *tx_item_ptr = NULL;
  register uint8 *cp;
  int             len;
  byte            ppp_hdr[PPP_HDR_LEN];
  negotiate_t     negotiated;           /* negotiated remote device opts  */
  boolean         doing_lcp;            /* Indication that LCP is happening*/
  boolean         insert_acfc;          /* address & control field added   */
  boolean         no_pfc;               /* no protocol field compression   */
  boolean         restore_accm = FALSE;
  uint32          accm_val = 0;
  hdlc_ioctl_u_type     ioctl_data;
  hdlc_errno_enum_type  ps_errno;
  ps_phys_link_type  * ps_phys_link_ptr;
#ifdef FEATURE_DATA_PS_PPP_ROHC
  errno_enum_type err;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  iphc_packet_type_e_type iphc_pkt;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
#ifdef FEATURE_DATA_PS_EHRPD
  ps_iface_type * vsncp_iface_ptr = NULL;
  uint8 index;
  uint8 pdn_id;
  ncpi_cb_type * instance_ptr;
  vsncp_3gpp2i_type * vsncp_cb_ptr;
#endif /* FEATURE_DATA_PS_EHRPD */

  slhc_packet_type_e_type slhc_pkt_type; /* Packet type returned by slhc_compress */

  boolean is_tcp_ack = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( incoming_pkt_ptr == NULL || *incoming_pkt_ptr == NULL )
  {
    DATA_ERR_FATAL("Invalid dsm passed to pppi_outgoing");
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  if(device >= PPP_MAX_DEV ||
     (*incoming_pkt_ptr)->used > (*incoming_pkt_ptr)->size)
  {
    ASSERT(0);
    if( device < PPP_MAX_DEV )
    {
      PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx, 1);
    }
    dsm_free_packet(incoming_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /* Check it has a PRIORITY tagged */
  if ((*incoming_pkt_ptr)->priority == DSM_HIGHEST)
  {
    is_tcp_ack = TRUE;
  }
  
  PPP_INC_INSTANCE_STATS(device,pkts_tx, 1);
  PPP_INC_INSTANCE_STATS(device,unframed_bytes_tx,(*incoming_pkt_ptr)->used);
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  if( ppp_cb_ptr->session_handle == 0 ||
      ppp_cb_ptr->mode == PPP_NONE_MODE
    )
  {
    PPP_INC_INSTANCE_STATS(device, pkts_dropped_tx, 1);
    LOG_MSG_INFO1_1("PPP %d not configured discading pkt", device);
    dsm_free_packet(incoming_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Assign interface specific variables.
  -------------------------------------------------------------------------*/
  negotiated  = ppp_cb_ptr->fsm[LCP].remote.work;

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    For IPv4/IPv6, see if they need to be encapsulated within VSNP
  -------------------------------------------------------------------------*/
  if( ( NULL != meta_info_ptr ) &&
      ( ( PPP_IP_PROTOCOL == protocol )
#ifdef FEATURE_DATA_PS_IPV6
    || ( PPP_IPV6_PROTOCOL == protocol )
#endif /* FEATURE_DATA_PS_IPV6 */
    ) )
  {
    /*-------------------------------------------------------------------------
      Serialize VSNCP control block access between PS_RM_task & PS_task.
    -------------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    vsncp_iface_ptr = PS_META_GET_ROUTING_CACHE( meta_info_ptr );
    if( vsncp_iface_ptr != NULL )
    {
      for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
      {
        instance_ptr =
          (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                               PS_MEM_PPP_NCP_CB_TYPE );
        if( ( NCPI_CB_IS_INITED( instance_ptr ) == TRUE )  && 
            ( instance_ptr->ncp == PPP_NCP_VSNCP_3GPP2 ) &&
            ( ( ( PS_IFACE_IS_VALID(
                  instance_ptr->cb_u.vsncp_3gpp2.fsm.IPV4_IFACE_PTR))&& 
                ( instance_ptr->cb_u.vsncp_3gpp2.fsm.IPV4_IFACE_PTR == 
                  vsncp_iface_ptr) ) 
#ifdef FEATURE_DATA_PS_IPV6

           || ( ( PS_IFACE_IS_VALID(
                  instance_ptr->cb_u.vsncp_3gpp2.fsm.IPV6_IFACE_PTR))&&
                (instance_ptr->cb_u.vsncp_3gpp2.fsm.IPV6_IFACE_PTR == 
                 vsncp_iface_ptr) )
#endif
             ) )
        {
          vsncp_cb_ptr = &(instance_ptr->cb_u.vsncp_3gpp2);
          pdn_id = vsncp_cb_ptr->config.option_vals.pdn_id;
          
          if( vsncp_cb_ptr->fsm.state != fsmOPENED )
          {
            LOG_MSG_INFO1_2( "dev %d, pdn %d not open for VSNP traffic",
                             device, pdn_id);
            PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx, 1);
            dsm_free_packet(incoming_pkt_ptr);
            PS_TX_META_INFO_FREE(&meta_info_ptr);
            PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
            return -1;
          }

          if (dsm_pushdown_packed( incoming_pkt_ptr,
                                   &pdn_id,
                                   sizeof(uint8),
                                   DSM_DS_SMALL_ITEM_POOL) < sizeof(uint8))
          {
            PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx,1);
            LOG_MSG_ERROR_0(" Running out of memory: dumping PPP frames");
            dsm_free_packet(incoming_pkt_ptr);
            PS_TX_META_INFO_FREE(&meta_info_ptr);
            PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
            return -1;
          }
          protocol = PPP_VSNP_PROTOCOL;        
          
          /*-----------------------------------------------------------------
            PDN activity seen, call the callback if registered
          -----------------------------------------------------------------*/
          if( vsncp_cb_ptr->config.activity_cb.func != NULL )
          {
            vsncp_cb_ptr->config.activity_cb.func
              ( vsncp_cb_ptr->config.activity_cb.data );
          }
          break;
        } /* this is a vsncp iface */
      } /* for */
    } /* iface in meta-info */

    PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  } /* ipv4/v6 pkts */
#endif /* FEATURE_DATA_PS_EHRPD */

  /*-------------------------------------------------------------------------
    For IP packets try compressing them
  -------------------------------------------------------------------------*/
  if( PPP_IP_PROTOCOL == protocol )
  {
    PPP_INC_INSTANCE_STATS(device,ipv4_pkts_tx, 1);
    if(ppp_cb_ptr->fsm[IPCP].state != fsmOPENED)
    {
      LOG_MSG_INFO1_1( "dev %d Not Open for IP Traffic", device);
      PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx, 1);
      dsm_free_packet(incoming_pkt_ptr);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
      return -1;
    }

    /*-----------------------------------------------------------------------
      If compression is negotiated and we are in internal mode only then
      call vj compress.  If lhcp in IPCP is NULL then skip as VJ is not
      initialized
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS)
    {
      register ipcp_pdv_type *ipcp_p =
        (ipcp_pdv_type*)ppp_cb_ptr->fsm[IPCP].pdv;
      register ipcp_value_type *vp   =
        (ipcp_value_type*)ppp_cb_ptr->fsm[IPCP].remote.work_pdv;\

      if((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS_VJ) &&
          NULL != ((ipcp_pdv_type*)ppp_cb_ptr->fsm[IPCP].pdv)->slhcp)\
      {
        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        if( slhc_compress( ipcp_p->slhcp,
                              incoming_pkt_ptr,
                           vp->vj_config.slot_comp,
                           &slhc_pkt_type ) == SLHC_SUCCESS )
        {
          switch(slhc_pkt_type)
          {
            case SLHC_PACKET_TYPE_IP: /* IP packet */
            break;

            case SLHC_PACKET_TYPE_COMPRESSED_TCP: /* compressed packet */
            PPP_INC_INSTANCE_STATS(device,vjch_pkts_tx, 1);
            PPP_DEC_INSTANCE_STATS(device,ipv4_pkts_tx, 1);
            protocol = PPP_VJCH_PROTOCOL;
            break;

            case SLHC_PACKET_TYPE_UNCOMPRESSED_TCP: /* incompressed packet */
            PPP_INC_INSTANCE_STATS(device,vjuch_pkts_tx, 1);
            PPP_DEC_INSTANCE_STATS(device,ipv4_pkts_tx, 1);
            protocol = PPP_VJUCH_PROTOCOL;
            break;

          default:
              LOG_MSG_ERROR_1("pppi_outgoing: Invalid slhc packet type: %d.",
                              slhc_pkt_type);
              break;
          }
        }
        else
        {
            /*---------------------------------------------------------------
              slhc_compress releases the incoming_pkt_ptr in case of an error
              Update counters and return here.
            ---------------------------------------------------------------*/
            PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx, 1);
            LOG_MSG_INFO1_0( "Bad IP Packet");
            dsm_free_packet(incoming_pkt_ptr);
            PS_TX_META_INFO_FREE(&meta_info_ptr);
            PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
            return -1;
        }
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      }
#ifdef FEATURE_DATA_PS_PPP_ROHC
       /*--------------------------------------------------------------------
         If ROHC is set up, attempt ROHC compression
       --------------------------------------------------------------------*/
      else if((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS_ROHC) &&
          NULL != ppp_cb_ptr->rohc_handle)
      {
        if(rohc_compress( ppp_cb_ptr->rohc_handle,
                          NULL,
                          NULL,
                          incoming_pkt_ptr,
                          &err) == -1 )
        {
          LOG_MSG_ERROR_1("Couldn't compress with ROHC: err %d. Dropping packet",
                          err);
          dsm_free_packet(incoming_pkt_ptr);
          PS_TX_META_INFO_FREE(&meta_info_ptr);
          return -1;
        }
        else
        {
          protocol = (((ipcp_value_type *)ppp_cb_ptr->fsm[IPCP].local.
                         work_pdv)->rohc_config.max_cid > 15) ?
                         PPP_ROHC_LCID_PROTOCOL :
                         PPP_ROHC_SCID_PROTOCOL;
        }
      }
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
      /*---------------------------------------------------------------------
          If IPHC is set up, attempt IPHC compression
      ---------------------------------------------------------------------*/
      else if((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS_IPHC) &&
          NULL != ppp_cb_ptr->iphcp)
      {
        if(iphc_compress(ppp_cb_ptr->iphcp, incoming_pkt_ptr, &iphc_pkt)
            == -1)
        {
          LOG_MSG_ERROR_0("Couldn't compress with IPHC. Dropping packet");
          dsm_free_packet(incoming_pkt_ptr);
          PS_TX_META_INFO_FREE(&meta_info_ptr);
          return -1;
        }
        else
        {
          if (iphc_pkt < IPHC_PACKET_TYPE_MAX)
          {
            protocol = iphc_packet_type_to_ppp_protocol_tbl[iphc_pkt];
          }
          else
          {
            LOG_MSG_ERROR_1("Invalid IPHC packet type %d. Dropping packet",
                            (iphc_pkt));
            dsm_free_packet(incoming_pkt_ptr);
            PS_TX_META_INFO_FREE(&meta_info_ptr);
            return -1;
          }
        }
      }
#endif /* FEATURE_DATA_PS_PPP_IPHC */
    }
  }

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    For IPv6 packets try compressing them
  -------------------------------------------------------------------------*/
  if( PPP_IPV6_PROTOCOL == protocol )
  {
    PPP_INC_INSTANCE_STATS(device,ipv6_pkts_tx, 1);
    if(ppp_cb_ptr->fsm[IPV6CP].state != fsmOPENED)
    {
      LOG_MSG_INFO1_1( "dev %d Not Open for IPv6 Traffic", device);
      PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx, 1);
      dsm_free_packet(incoming_pkt_ptr);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
      return -1;
    }

#ifdef FEATURE_DATA_PS_PPP_ROHC
    /*-------------------------------------------------------------------
      If ROHC is set up, attempt ROHC compression
    -------------------------------------------------------------------*/
    if((ppp_cb_ptr->fsm[IPV6CP].remote.work & IPV6CP_N_COMPRESS_ROHC) &&
        NULL != ppp_cb_ptr->rohc_handle)
    {
      if(rohc_compress(ppp_cb_ptr->rohc_handle,
                       NULL,
                       NULL,
                       incoming_pkt_ptr,
                       &err) == -1)
      {
        LOG_MSG_ERROR_1("Couldn't compress with ROHC: err %d. Dropping packet",
                        err);
        dsm_free_packet(incoming_pkt_ptr);
        PS_TX_META_INFO_FREE(&meta_info_ptr);
        return -1;
      }
      else
      {
        protocol = (((ipcp_value_type *)ppp_cb_ptr->fsm[IPV6CP].local.
                     work_pdv)->rohc_config.max_cid > 15) ?
                       PPP_ROHC_LCID_PROTOCOL :
                       PPP_ROHC_SCID_PROTOCOL;
      }
    }
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
    /*-------------------------------------------------------------------
      If IPHC is set up, attempt IPHC compression
    -------------------------------------------------------------------*/
    else if((ppp_cb_ptr->fsm[IPV6CP].remote.work & IPV6CP_N_COMPRESS_IPHC) &&
            NULL != ppp_cb_ptr->iphcp)
    {
      if(iphc_compress(ppp_cb_ptr->iphcp, incoming_pkt_ptr, &iphc_pkt) == -1)
      {
        LOG_MSG_ERROR_0("Couldn't compress with IPHC. Dropping packet");
        dsm_free_packet(incoming_pkt_ptr);
        PS_TX_META_INFO_FREE(&meta_info_ptr);
        return -1;
      }
      else
      {
        if (iphc_pkt < IPHC_PACKET_TYPE_MAX)
        {
          protocol = iphc_packet_type_to_ppp_protocol_tbl[iphc_pkt];
        }
        else
        {
          LOG_MSG_ERROR_1("Invalid IPHC packet type %d. Dropping packet",
                          (iphc_pkt));
          dsm_free_packet(incoming_pkt_ptr);
          PS_TX_META_INFO_FREE(&meta_info_ptr);
          return -1;
        }
      }
    }
#endif /* FEATURE_DATA_PS_PPP_IPHC */
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  doing_lcp = (protocol == PPP_LCP_PROTOCOL);

  /*-------------------------------------------------------------------------
    Check PPP stage and negotiated header compression settings to determine
    what all to prepend.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Compress protocol field if allowed:
    - can't compress until LCP is complete
    - can only compress if PFC is negotiated during LCP.
    - Can only compress protocols whose MSB is 0
      (protocol value must be less or equal to 255).
  -------------------------------------------------------------------------*/
  no_pfc = doing_lcp || !(negotiated & LCP_N_PFC) || (protocol >> 8);

  /*-------------------------------------------------------------------------
    Insert HDLC address and control fields if needed
    - NOT needed if HDLC mode is off
    - otherwise,
      - always included during LCP
      - not required after LCP, if ACFC is negotiated

    The logic representing above is summarized below:

    hdlc off  doing lcp  acfc neg. | insert ac
    --------  ---------  --------- | ---------
        0         0          0     |     1
        0         1          X     |     1
        0         0          1     |     0
        1         X          X     |     0

  -------------------------------------------------------------------------*/
  insert_acfc = !( (ppp_cb_ptr->hdlc_mode == HDLC_OFF)
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
                || (ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS)
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */
                || (!doing_lcp && (negotiated & LCP_N_ACFC)) );

  /*-------------------------------------------------------------------------
    Build the PPP header in the local buffer
  -------------------------------------------------------------------------*/
  cp = ppp_hdr;

  /*-------------------------------------------------------------------------
    Do we do full address and control fields? Add HDLC framing if needed.
  -------------------------------------------------------------------------*/
  if(insert_acfc)
  {
    *cp++ = HDLC_ALL_ADDR;
    *cp++ = HDLC_UI;
  }

  /*-------------------------------------------------------------------------
    Do we do put in full protocol field.  If so place MSB in packet,
    otherwise only put LSB in packet.
  -------------------------------------------------------------------------*/
  if(no_pfc)
  {
    *cp++ = (protocol >> 8);
  }
  *cp++ = (protocol & 0x00ff);

  /*-------------------------------------------------------------------------
    Load header with proper values
  -------------------------------------------------------------------------*/
  len = (cp - ppp_hdr);
  if (dsm_pushdown_packed( incoming_pkt_ptr,
                           ppp_hdr,
                           (uint16)len,
                           DSM_DS_SMALL_ITEM_POOL) < len)
  {
     PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx,1);
     dsm_free_packet(incoming_pkt_ptr);
     PS_TX_META_INFO_FREE(&meta_info_ptr);
     DATA_ERR_FATAL(" Running out of memory: dumping PPP frames");
     return -1;
  }

  if( (ppp_cb_ptr->hdlc_mode != HDLC_OFF)
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
   && (ppp_cb_ptr->hdlc_mode != HDLC_A2_AUTONOMOUS)  
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */
    )
  {
    /*-----------------------------------------------------------------------
      LCP packets need to be framed using the default accm (0xffffffff).
      Get the current ACCM in use. If it's not the default value, save the
      value and set it to default. Restore after hdlc_frame.
    -----------------------------------------------------------------------*/
    if( doing_lcp )
    {
      if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                      HDLC_IOCTL_GET_ACCM,
                      &ioctl_data,
                      &ps_errno  ) == 0 )
      {
        if( ioctl_data.accm != LCP_ACCM_DEFAULT )
        {
          accm_val = ioctl_data.accm;
          ioctl_data.accm = LCP_ACCM_DEFAULT;
          if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                          HDLC_IOCTL_SET_ACCM,
                          &ioctl_data,
                          &ps_errno ) == 0 )
          {
            restore_accm = TRUE;
          }
          else
          {
            LOG_MSG_ERROR_2("Unable to set HDLC ACCM for dev %d, ps_errno %d",
                            device, ps_errno);
          }
        }
      }
      else
      {
        LOG_MSG_ERROR_2("Unable to get HDLC ACCM for dev %d, ps_errno %d",
                        device, ps_errno);
      }
    } /* doing LCP */

    /*-----------------------------------------------------------------------
      Call the HDLC library function to frame the packet
    -----------------------------------------------------------------------*/
    if( hdlc_frame( ppp_cb_ptr->hdlc_handle,
                    incoming_pkt_ptr,
                    &tx_item_ptr, (void*)meta_info_ptr ) == 0)
    {
      if( TRUE == restore_accm )
      {
        ioctl_data.accm = accm_val;
        if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                        HDLC_IOCTL_SET_ACCM,
                        &ioctl_data,
                        &ps_errno ) != 0 )
        {
          LOG_MSG_ERROR_2("Unable to set HDLC ACCM for dev %d, ps_errno %d",
                          device, ps_errno);
        }
      } /* restore accm */
    }
    else
    {
      LOG_MSG_ERROR_2("Framing error, dev (%d), ps_errno = %d",
                      device, hdlc_errno( ppp_cb_ptr->hdlc_handle ));
    }
  } /* hdlc_mode != HDLC_OFF */
  else
  {
    tx_item_ptr = *incoming_pkt_ptr;
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
    if(ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS)
    {
      if( doing_lcp )
      {      
        tx_item_ptr->app_field = LCP_ACCM_DEFAULT;
      }
      else
      {
        tx_item_ptr->app_field = ((lcp_value_type*)
                         (ppp_cb_array[device].fsm[LCP].remote.work_pdv))->accm;
      }
    }
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */ 
  }

  /*-------------------------------------------------------------------------
    If the meta_info item is non-NULL we need to push it down onto the front
    of the packet chain.
  -------------------------------------------------------------------------*/
  if(tx_item_ptr == NULL)
  {
    PPP_INC_INSTANCE_STATS(device,pkts_dropped_tx,1);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Log framed data if HDLC mode is not HDLC_OFF
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->hdlc_mode != HDLC_OFF)
    {
#ifdef FEATURE_DATA_PS_PPP_LOGGING
      PPP_LOG_FRAMES_EXT (tx_item_ptr, PPP_LOG_FRAMED_TX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

      ps_phys_link_ptr =
        PS_IFACE_GET_PHYS_LINK(ppp_cb_ptr->fsm[IPCP].rx_iface_ptr);
      if(ps_phys_link_ptr != NULL)
      {
        DPL_LOG_LINK_FULL_TX_PACKET(ps_phys_link_ptr,
                                    tx_item_ptr,
                                    (uint8)DPL_IID_LINKPROT_PPP_IN_HDLC);
      }
    } /* if(ppp_cb_ptr->hdlc_mode != HDLC_OFF) */
    *incoming_pkt_ptr = NULL;

  /* Tag back the prioroty if it was tagged earlier */
  if (is_tcp_ack)
  {
    tx_item_ptr->priority = DSM_HIGHEST;
  }
	
    /* meta_info_ptr should be cleared by mode handlers! */
    ppp_cb_ptr->tx_f_ptr(&tx_item_ptr,
                         meta_info_ptr,
                         ppp_cb_ptr->tx_user_data);
    return 0;
  } /* else(valid TX data) */

} /* pppi_outgoing() */


/*===========================================================================

                                INBOUND PATH

===========================================================================*/
/*===========================================================================
FUNCTION PPPI_INCOMING()

DESCRIPTION
  This function will process the PPP packet passed by the underlying device.
  Makes sure that the PFC and ACFC compression are not on when they shouldn't
  be. For modes other than INTERNAL, calls the snooping callback if one is
  registered and passes pkt to the correct destination based upon the return
  value from the callback. When in INTERNAL mode, it passes pkt up the stack
  for protocol procesing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_incoming
(
   ppp_dev_enum_type device,           /* ppp device pkt arrived on        */
   dsm_item_type **item_ref_ptr        /* Pointer to ref to pkt            */
)
{
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;
  uint16      protocol;               /* protocol of packet                */
  negotiate_t negotiated = FALSE;     /* what's been negotiated            */
  snoop_cb_ret_val_enum_type snoop_cb_res;  /* return value from snoop cb  */
  int pullvar;
  ps_rx_meta_info_type *meta_info_ptr = NULL;
  uint8 first_byte;
  uint8 second_byte;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= PPP_MAX_DEV ||
     item_ref_ptr == NULL  ||
     (*item_ref_ptr) == NULL)
  {
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  if((*item_ref_ptr)->used == 0)
  {
    PPP_INC_INSTANCE_STATS(device,pkts_dropped_rx,1);
    dsm_free_packet(item_ref_ptr);
    ASSERT(0);
    return;
  }

  ppp_cb_ptr = &(ppp_cb_array[device]);
  /*-------------------------------------------------------------------------
    All packets here are unframed; Increment statistics
  -------------------------------------------------------------------------*/
  PPP_INC_INSTANCE_STATS(device,unframed_bytes_rx,(*item_ref_ptr)->used);
  /*-------------------------------------------------------------------------
    Use negotiated values if LCP finished
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->fsm[LCP].state == fsmOPENED)
  {
    negotiated = ppp_cb_ptr->fsm[LCP].local.work;
  }
  /*-------------------------------------------------------------------------
    If we have the LCP in passive mode and are in starting phase, it
    probably means we have re-started our PPP for some reason (maybe powerup)
    and we need to resync PPP. Allow passive wakeup on receiving any PPP
    packet in this case if enabled through PPP config. Alternately we will
    wakeup on reciving an LCP config packet when it hits the FSM.
  -------------------------------------------------------------------------*/
  else if(ppp_cb_ptr->fsm[LCP].state == fsmSTARTING &&
          ppp_cb_ptr->fsm[LCP].mode == PPP_CP_MODE_PASSIVE &&
          ppp_cb_ptr->passive_wakeup_on_data == TRUE)

  {
    fsm_passive_wakeup(&(ppp_cb_ptr->fsm[LCP]));
    /* fall thru and process the incoming packet as usual */
  }

  /*-------------------------------------------------------------------------
    If HDLC was enabled, then the incoming pkt was HDLC framed, we only
    care about this if we are in internal mode.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->hdlc_mode != HDLC_OFF)
  {
    /*-----------------------------------------------------------------------
      HDLC address and control fields may be compressed out.  IFF the first
      byte is not the HDLC_ALL_ADDR byte (FF) then it is compressed out.
      Check if compression is enabled.  Error checking should only be done
      when PPP is in INTERNAL mode.
    -----------------------------------------------------------------------*/
    first_byte = (uint8)(*item_ref_ptr)->data_ptr[0];
    if ((*item_ref_ptr)->used == 1)
    {
      if( FALSE == dsm_peek_byte(*item_ref_ptr,1,&second_byte) )
      {
        LOG_MSG_INFO2_0("Unable to extract second hdr byte, discarding pkt");
        pppi_skipped(device,  item_ref_ptr);
        return;
      }
    }
    else
    {
      second_byte = (uint8)(*item_ref_ptr)->data_ptr[1];
    }

    if(first_byte != HDLC_ALL_ADDR)
    {
      if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE &&
         !(negotiated & LCP_N_ACFC))
      {
        PPP_INC_INSTANCE_STATS(device,missing_addr,1);
        LOG_MSG_INFO1_0( "Missing ALL Address");
        pppi_skipped(device,  item_ref_ptr);
        return;
      }
    } /* if(first byte != FF) */

    /*-----------------------------------------------------------------------
      The first byte was FF: if the second byte is HDLC_UI (03) - if not then
      Check if compression is enabled.  Error checking should only be done
      when PPP is in INTERNAL mode.
    -----------------------------------------------------------------------*/
    else if(second_byte != HDLC_UI)
    {
      if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE &&
         (!(negotiated & LCP_N_ACFC) ||
          !(negotiated & LCP_N_PFC)))
      {
        PPP_INC_INSTANCE_STATS(device,missing_ctrl,1);
        LOG_MSG_INFO1_0( "Missing UI");
        pppi_skipped(device,  item_ref_ptr);
        return;
      }
    }
    /*-----------------------------------------------------------------------
      the first two bytes are FF03, strip them out
    -----------------------------------------------------------------------*/
    else
    {
      (void)dsm_pull16(item_ref_ptr);     /* skip address/control fields */
    }
  } /* if(ppp_cb_ptr->hdlc_mode != HDLC_OFF) */

  /*-------------------------------------------------------------------------
    Initialize the expected header
  -------------------------------------------------------------------------*/
  if((pullvar = dsm_pull8(item_ref_ptr)) == -1 )
  {
    LOG_MSG_INFO1_0( "Truncated packet");
    pppi_skipped(device,  item_ref_ptr);
    return;
  }
  protocol = (uint16)pullvar;

  /*-------------------------------------------------------------------------
    First byte of PPP protocol field may be compressed out.  If the LS bit is
    1 then this indicates that the upper protocol byte is compressed out, as
    only the lower byte should be odd (i.e. upper should be even).  Check if
    compression is enabled.  In the case of network model builds the error
    checking should only be done when PPP is in INTERNAL mode.
  -------------------------------------------------------------------------*/
  if( protocol & 0x01)
  {
    if(ppp_cb_ptr->mode == PPP_INTERNAL_MODE &&
      !(negotiated & LCP_N_PFC))
    {
      PPP_INC_INSTANCE_STATS(device,bad_proto,1);
      LOG_MSG_INFO1_0( "Missing Upper Protocol Byte");
      pppi_skipped(device,  item_ref_ptr);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Upper protocol byte is NOT compressed out check that lower is correct
  -------------------------------------------------------------------------*/
  else
  {
    if((pullvar = dsm_pull8(item_ref_ptr)) == -1 )
    {
      LOG_MSG_INFO1_0( "Truncated packet");
      pppi_skipped(device,  item_ref_ptr);
      return;
    }

    protocol = (uint16)((protocol << 8) | pullvar);

    /*-----------------------------------------------------------------------
      Second byte of PPP protocol field must be odd
    -----------------------------------------------------------------------*/
    if( !(protocol & 0x01))
    {
      PPP_INC_INSTANCE_STATS(device,bad_proto,1);
      LOG_MSG_INFO1_0( "Missing Lower Protocol Byte");
      pppi_skipped(device,  item_ref_ptr);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    If the input interface is not in PPP_INTERNAL_MODE, call the function
    to detect the PPP protocol and message and call the callbacks.
    In PPP_INTERNAL_MODE, the PPP packets are processed internally.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->mode != PPP_INTERNAL_MODE)
  {
    do
    {
      snoop_cb_res = snoop_proto_msg_detect(device,
                                            protocol,
                                            item_ref_ptr,
                                            &meta_info_ptr);
    } while( snoop_cb_res == SNOOP_CB_SNOOP_AGAIN);

    /*-----------------------------------------------------------------------
      If we are asked to forward the packet, send it to the bridged PPP dev.
      If we are asked to ignore the pakcket, it means the packet has been
      completely processed by the callback and we are done in this function.
      Otherwise we will pass the packet up the stack for further processing.
    -----------------------------------------------------------------------*/
    if(snoop_cb_res == SNOOP_CB_FORWARD_PKT)
    {
      if(ppp_cb_ptr->bridge_dev >= PPP_MAX_DEV)
      {
        PPP_INC_INSTANCE_STATS(device, pkts_dropped_rx, 1);
        LOG_MSG_ERROR_2("Dev %d cann't forward pkt to invalid dev %d",
                        device, ppp_cb_ptr->bridge_dev);
        dsm_free_packet(item_ref_ptr);
      }
      else
      {
        DSM_TOUCH_ITEM(*item_ref_ptr);
        /*-------------------------------------------------------------
          No metainfo should get generated when we reach here, in case 
          if we do just clear RX metainfo. Expectation is SNOOP module 
          clear the metainfo if it generates one. 
          TODO: Need to revisit if we get some use-case(eHRPD?).
        ------------------------------------------------------------*/
        PS_RX_META_INFO_FREE(&meta_info_ptr);
        (void)pppi_outgoing(ppp_cb_ptr->bridge_dev, protocol,
                            item_ref_ptr, 
                            NULL); /* Null Metainfo pointer */
      }
      return;
    }
    else if(snoop_cb_res == SNOOP_CB_IGNORE_PKT)
    {
      return;
    }
  }

  /*-------------------------------------------------------------------------
    If we reach this far, then the packet belongs to the stack in the
    phone.  Pass the packet up to perform protocol specific processing.
  -------------------------------------------------------------------------*/
  pppi_receive(device, protocol, item_ref_ptr, meta_info_ptr);
} /* pppi_incoming() */


/*===========================================================================
FUNCTION PPPI_NCP_RECEIVE()

DESCRIPTION
  This processes incoming NCP packets

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_ncp_receive
(
  ppp_dev_enum_type device,           /* ppp device pkt arrived on         */
  uint16            protocol,         /* protocol of packet                */
  dsm_item_type   **item_ref_ptr      /* Pointer to ref to pkt             */
)
{

  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;
  ppp_fsm_type * fsm_ptr = NULL;
#ifdef FEATURE_DATA_PS_EHRPD
  uint32 oui = 0;
#endif /* FEATURE_DATA_PS_EHRPD */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  if( ( protocol != PPP_IPCP_PROTOCOL ) 
#ifdef FEATURE_DATA_PS_IPV6
      && ( protocol != PPP_IPV6CP_PROTOCOL ) 
#endif /* FEATURE_DATA_PS_IPV6 */
#ifdef FEATURE_DATA_PS_EHRPD
      && ( protocol != PPP_VSNCP_PROTOCOL ) 
#endif /* FEATURE_DATA_PS_EHRPD */
    ) 
  {
    LOG_MSG_ERROR_1("Invalid NCP 0x%x passed!", protocol);
    ASSERT(0);
    return;
  }
  
  ppp_cb_array = fsm_get_ppp_cb_array();  
  ppp_cb_ptr = &(ppp_cb_array[device]);

  /*-------------------------------------------------------------------------
    If we are still in authenticate phase, check if it is OK to move on to 
    the network phase.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->phase == pppAUTHENTICATE)
  {
    /*-----------------------------------------------------------------------
      If we were authenticating, and the success flag is NOT set then we
      can't move onto the next stage, so free the packet and break out of
      this processing.  NOTE: This will only happen if we don't get the
      authentication information from the peer.
    -----------------------------------------------------------------------*/
    if((ppp_cb_ptr->auth.flags & PPP_AP_REMOTE) &&
        !(ppp_cb_ptr->auth.flags & PPP_AP_SUCCESS))
    {
      PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
      pppi_skipped(device, item_ref_ptr);
      return;
    }

    /*-------------------------------------------------------------------
      It is OK to Move on, so complete auth processing for appropriate
      protocol.
    -------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_PPP_EAP
    else if(ppp_cb_ptr->auth.mode == PPP_DO_EAP)
    {
      ppp_auth_complete(ppp_cb_ptr, PPP_EAP_PROTOCOL);
    }
#endif /*FEATURE_DATA_PS_PPP_EAP*/

    else if(ppp_cb_ptr->auth.mode == PPP_DO_PAP)
    {
      ppp_auth_complete(ppp_cb_ptr, PPP_PAP_PROTOCOL);
    }

    else if(ppp_cb_ptr->auth.mode == PPP_DO_CHAP)
    {
      ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);
    }
  } /* if(in auth phase) */
    
  /*---------------------------------------------------------------------
    Silently discard packet if we are not in network phase yet
  ---------------------------------------------------------------------*/
  if(ppp_cb_ptr->phase != pppNETWORK)
  {
    PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
    LOG_MSG_INFO1_1("dev %d not Ready for NCP traffic ", device);
    pppi_skipped(device, item_ref_ptr);
    return;
  }

  switch( protocol )
  {
    case PPP_IPCP_PROTOCOL:
      fsm_ptr = &(ppp_cb_ptr->fsm[IPCP]);
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case PPP_IPV6CP_PROTOCOL:
      fsm_ptr = &(ppp_cb_ptr->fsm[IPV6CP]);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_EHRPD
    case PPP_VSNCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Is this an OUI we support ?
      ---------------------------------------------------------------------*/
      if( dsm_extract( *item_ref_ptr, 
                       sizeof(ppp_config_hdr_type), 
                       &oui, 
                       PPP_OUI_LEN ) != PPP_OUI_LEN )
      {
        LOG_MSG_ERROR_1("dsm_extract() failed, pkt = 0x%x", item_ref_ptr);
        dsm_free_packet( item_ref_ptr );
        return;
      }
      
      oui <<= 8;            /* lshift needed since OUI is only 3-byte wide */
      oui = ps_ntohl( oui );
      switch( oui )
      {
        case PPP_VS_3GPP2_OUI:
          vsncp_3gpp2_receive( device, *item_ref_ptr );
          break;
        
        default:
          LOG_MSG_ERROR_1("Unsupported OUI 0x%x rxed in VSNCP pkt", oui);
          PPP_INC_INSTANCE_STATS(device, not_enabled_proto, 1);
          (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
          break;
      } /* oui */
      return;
#endif /* FEATURE_DATA_PS_EHRPD */

    default:
      /* can't happen! */
      LOG_MSG_ERROR_1("Invalid protocol 0x%x", protocol);
      ASSERT(0);
      return;
  } /* protocol */

  /*---------------------------------------------------------------------
    If device is not configured for this NCP, send a protocol reject,
    else process the packet. This section is relevant only for ipxcp.
  ---------------------------------------------------------------------*/
  if( fsm_ptr->mode == PPP_CP_MODE_DISABLED)
  {
    (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
  }
  else
  {
    /*-------------------------------------------------------------------
      If this NCP is configured to ignore unsolicited NCP configs then
      silently discard the packet.
    -------------------------------------------------------------------*/
    if( ( fsm_ptr->mode == PPP_CP_MODE_DISCARD ) ||
        ( fsm_ptr->mode == PPP_CP_MODE_INVALID ) )
    {
      LOG_MSG_INFO1_1("dev %d set to ignore uninitiated NCP Traffic", device);
      pppi_skipped(device, item_ref_ptr);
      return;
    }
    
    fsm_proc( fsm_ptr, *item_ref_ptr);
  } /* else(IPxCP is available) */
} /* pppi_ncp_receive() */


/*===========================================================================
FUNCTION PPPI_RECEIVE()

DESCRIPTION
  This is the PPP packet receiving function in the stack. It processes each
  PPP packet based upon the protocol type and pass it up to the appropriate
  protocol engine in the stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_receive
(
  ppp_dev_enum_type device,           /* ppp device pkt arrived on         */
  uint16            protocol,         /* protocol of packet                */
  dsm_item_type   **item_ref_ptr,     /* Pointer to ref to pkt             */
  ps_rx_meta_info_type *meta_info_ptr /* Pointer to PS meta info           */
)
{
  ppp_type *ppp_cb_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;
  ps_meta_info_type_ex  *meta_info_ex_ptr = NULL;
  struct    ipcp_pdv_s *ipcp_p;       /* Pointer to IPCP control block     */
#ifdef FEATURE_DATA_PS_PPP_ROHC
  errno_enum_type err;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
  iphc_packet_type_e_type pkt_type;
#endif /* FEATURE_DATA_PS_PPP_IPHC */
#ifdef FEATURE_DATA_PS_EHRPD
  int pdn_id;
  uint8 ip_version;
  uint8 index;
  ncpi_cb_type * instance_ptr;
  ppp_fsm_type * fsm_ptr;
  vsncp_3gpp2i_type * vsncp_cb_ptr;
#endif /* FEATURE_DATA_PS_EHRPD */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( item_ref_ptr == NULL || *item_ref_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid args in pppi_receive");
    return;
  }

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid device in pppi_receive");
    dsm_free_packet(item_ref_ptr);
    PS_RX_META_INFO_FREE( &meta_info_ptr );
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[device]);
  if( ppp_cb_ptr->session_handle == 0 ||
      ppp_cb_ptr->mode == PPP_NONE_MODE)
  {
    PPP_INC_INSTANCE_STATS(device, pkts_dropped_rx, 1);
    LOG_MSG_INFO1_1("PPP %d not configured discading pkt", device);
    dsm_free_packet(item_ref_ptr);
    PS_RX_META_INFO_FREE( &meta_info_ptr );
    return;
  }

  switch(protocol)
  {
    case PPP_LCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got LCP packet, update stats and process it
      ---------------------------------------------------------------------*/
      fsm_proc(&(ppp_cb_ptr->fsm[LCP]),*item_ref_ptr);
      break;

    case PPP_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Make sure we are in the auth phase, as that is the only place for PAP
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->phase != pppAUTHENTICATE)
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        LOG_MSG_INFO1_3("dev %d in phase %d not auth(%d)!",
                        device, ppp_cb_ptr->phase, pppAUTHENTICATE);
        pppi_skipped(device, item_ref_ptr);
        break;
      }

      pap_proc(device, item_ref_ptr);
      break;

    case PPP_CHAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Make sure we are in the auth phase or network phase as CHAP can
        happen in either
      ---------------------------------------------------------------------*/
      if( ( (uint8)ppp_cb_ptr->phase & 
            ((uint8)pppAUTHENTICATE | (uint8)pppNETWORK ) ) == 0 )
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        LOG_MSG_INFO1_2("dev %d in phase %d not auth or network phases!",
                        device, ppp_cb_ptr->phase);
        pppi_skipped(device, item_ref_ptr);
        break;
      }

      chap_proc(device, item_ref_ptr);
    break;

#ifdef FEATURE_DATA_PS_PPP_EAP
  case PPP_EAP_PROTOCOL:
      /* Process the EAP packet only if in Authentication or Network Phase */
      if(((uint8)ppp_cb_ptr->phase & ((uint8)pppAUTHENTICATE | (uint8)pppNETWORK)) == 0)
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        LOG_MSG_INFO1_3("dev %d in phase %d not auth(%d)!",
                        device, ppp_cb_ptr->phase, pppAUTHENTICATE);
          pppi_skipped(device, item_ref_ptr);
          break;
        }

      ppp_eap_proc(device, item_ref_ptr);
          break;

#endif/*FEATURE_DATA_PS_PPP_EAP*/

    case PPP_IPCP_PROTOCOL:
#ifdef FEATURE_DATA_PS_IPV6
    case PPP_IPV6CP_PROTOCOL:
#endif /* FEATURE_DATA_PS_IPV6 */
#ifdef FEATURE_DATA_PS_EHRPD
    case PPP_VSNCP_PROTOCOL:
#endif /* FEATURE_DATA_PS_EHRPD */
      pppi_ncp_receive( device, protocol, item_ref_ptr );
          break;

    case PPP_IP_PROTOCOL:
      /*---------------------------------------------------------------------
        Got Regular IP, make sure we are ready for it, then update stats and
        process it.
        If device is configured to not do IPCP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_DISABLED)
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPCP].state != fsmOPENED )
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Not Open For IP Traffic");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }
        PPP_INC_INSTANCE_STATS(device,ipv4_pkts_rx, 1);

        /* Allocate Metainfo_ex buffer to pass RX metainfo_ptr */
        if (meta_info_ptr != NULL)
        {
          PS_META_INFO_GET_EX(meta_info_ex_ptr);
          if (meta_info_ex_ptr == NULL)
          {
            LOG_MSG_ERROR_0("pppi_receive: Metainfo_ex pool out of memory"  );
            dsm_free_packet(item_ref_ptr);
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            return;
          }
          meta_info_ex_ptr->rx_meta_info_ptr = meta_info_ptr;
        }

        (void)ppp_cb_ptr->fsm[IPCP].iface_input
              (
                ppp_cb_ptr->fsm[IPCP].rx_iface_ptr,
                item_ref_ptr,
                meta_info_ex_ptr
              );
      }
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case PPP_IPV6_PROTOCOL:
      /*---------------------------------------------------------------------
        Got Regular IPv6, make sure we are ready for it, then update stats
        and process it.
        If device is configured to not do IPV6CP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPV6CP].mode == PPP_CP_MODE_DISABLED)
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPV6CP].state != fsmOPENED )
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Not Open For IPv6 Traffic");
          pppi_skipped(device, item_ref_ptr);
          break;
        }
        PPP_INC_INSTANCE_STATS(device,ipv6_pkts_rx, 1);

        /* Allocate Metainfo_ex buffer to pass RX metainfo_ptr */
        if (meta_info_ptr != NULL)
        {
          PS_META_INFO_GET_EX(meta_info_ex_ptr);
          if (meta_info_ex_ptr == NULL)
          {
            LOG_MSG_ERROR_0("pppi_receive: Metainfo_ex pool out of memory"  );
            dsm_free_packet(item_ref_ptr);
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            return;
          }
          meta_info_ex_ptr->rx_meta_info_ptr = meta_info_ptr;
        }

        (void)ppp_cb_ptr->fsm[IPV6CP].iface_input
              (
                ppp_cb_ptr->fsm[IPV6CP].rx_iface_ptr,
                item_ref_ptr,
                meta_info_ex_ptr
              );
      }
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_EHRPD
    case PPP_VSNP_PROTOCOL:
      /*---------------------------------------------------------------------
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |            Protocol           |    pdn-id      |     data     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                        Data ...
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
        The protocol field has already been grabbed. Remove the pdn_id to 
        locate the corresponding fsm.
      ---------------------------------------------------------------------*/
      pdn_id = dsm_pull8( item_ref_ptr );
      if( pdn_id == -1 )
      {
        LOG_MSG_ERROR_1("dsm_pull8() failed, pkt = 0x%x", item_ref_ptr);
        dsm_free_packet( item_ref_ptr );
        break;
      }
      
      /*---------------------------------------------------------------------
        See if a matching fsm exists
      ---------------------------------------------------------------------*/
      for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
      {
        instance_ptr =
          (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                               PS_MEM_PPP_NCP_CB_TYPE );
        if( ( NCPI_CB_IS_INITED( instance_ptr ) == TRUE )  && 
            ( instance_ptr->ncp == PPP_NCP_VSNCP_3GPP2 ) &&
            ( instance_ptr->cb_u.vsncp_3gpp2.config.option_vals.pdn_id 
              == pdn_id ) )
        {
          fsm_ptr = &(instance_ptr->cb_u.vsncp_3gpp2.fsm);
          if( fsm_ptr->state != fsmOPENED )
          {
            PPP_INC_INSTANCE_STATS(device, not_enabled_proto, 1);
            LOG_MSG_INFO1_1("pdn id 0x%x not open for VSNP traffic!", pdn_id);
            pppi_skipped(device,  item_ref_ptr);
            return;
          }

          /* Allocate Metainfo_ex buffer to pass RX metainfo_ptr */
          if (meta_info_ptr != NULL)
          {
            PS_META_INFO_GET_EX(meta_info_ex_ptr);
            if (meta_info_ex_ptr == NULL)
            {
              LOG_MSG_ERROR_0("pppi_receive: Metainfo_ex pool out of memory"  );
              dsm_free_packet(item_ref_ptr);
              PS_RX_META_INFO_FREE(&meta_info_ptr);
              return;
            }
            meta_info_ex_ptr->rx_meta_info_ptr = meta_info_ptr;
          }

          /*---------------------------------------------------------------------
            Extract Version field from IP header.
          ---------------------------------------------------------------------*/
          if ( FALSE == dsm_peek_byte( *item_ref_ptr, 0, &ip_version ) )
          {
            LOG_MSG_ERROR_1("dsm_peek_byte() failed, pkt = 0x%x",
                            item_ref_ptr);
            dsm_free_packet( item_ref_ptr );
            return;
          }

          /*---------------------------------------------------------------------
            Most significant 4 bits of first data byte in VSNP packet represents
            IP Version.
          ---------------------------------------------------------------------*/
          ip_version >>= 4;

          /*---------------------------------------------------------------------
            Call iface_input with appropriate iface_ptr based on IP version.
          ---------------------------------------------------------------------*/
          if ( ip_version == (uint8)IPV4_ADDR )
          {
            (void)fsm_ptr->iface_input( fsm_ptr->IPV4_IFACE_PTR,
                                        item_ref_ptr,
                                        meta_info_ex_ptr );
          }
#ifdef FEATURE_DATA_PS_IPV6
          else if ( ip_version == (uint8)IPV6_ADDR )
          {
            (void)fsm_ptr->iface_input( fsm_ptr->IPV6_IFACE_PTR,
                                        item_ref_ptr,
                                        meta_info_ex_ptr );
          }
#endif
          else
          {
            LOG_MSG_ERROR_1("Invalid IP packet found in pkt = 0x%x",
                            item_ref_ptr);
            dsm_free_packet( item_ref_ptr );
            return;
          }

          /*-----------------------------------------------------------------
            PDN activity seen, call the callback if registered
          -----------------------------------------------------------------*/
          vsncp_cb_ptr = &(instance_ptr->cb_u.vsncp_3gpp2);
          if( vsncp_cb_ptr->config.activity_cb.func != NULL )
          {
            vsncp_cb_ptr->config.activity_cb.func
              ( vsncp_cb_ptr->config.activity_cb.data );
          }    
          return;
        } /* match found */
      } /* for */

      /*---------------------------------------------------------------------
        No matching fsm found, drop packet
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO1_1("Dropping data for invalid pdn id 0x%x", pdn_id);
      pppi_skipped(device,  item_ref_ptr);
      break;
#endif /* FEATURE_DATA_PS_EHRPD */

    case PPP_VJCH_PROTOCOL:
      /*---------------------------------------------------------------------
        Got Van Jacobson Compressed TCP/IP, make sure that we are ready for
        it, support it, and have state for it.  Then update stats and process.

        If device is configured to not do IPCP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_DISABLED)
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPCP].state != fsmOPENED)
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Not Open for Compressed TCP/IP");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        if(!(ppp_cb_ptr->fsm[IPCP].local.work & IPCP_N_COMPRESS) ||
           !(ppp_cb_ptr->fsm[IPCP].local.work & IPCP_N_COMPRESS_VJ))
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Compressed TCP/IP Not Enabled");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        ipcp_p = (ipcp_pdv_type*)(ppp_cb_ptr->fsm[IPCP].pdv);

        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        if( slhc_decompress(ipcp_p->slhcp,
                            item_ref_ptr,
                            SLHC_PACKET_TYPE_COMPRESSED_TCP) == SLHC_FAILED )
        {
          LOG_MSG_INFO1_0( "Compressed TCP/IP Packet Err");
          pppi_skipped(device,  item_ref_ptr);
          PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
          break;
        }
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

        PPP_INC_INSTANCE_STATS(device,vjch_pkts_rx, 1);
        (void) ppp_cb_ptr->fsm[IPCP].iface_input
               (
                 ppp_cb_ptr->fsm[IPCP].rx_iface_ptr,
                 item_ref_ptr,
                 NULL
               );
      }
      break;

    case PPP_VJUCH_PROTOCOL:
      /*---------------------------------------------------------------------
        Get Van Jacobson Uncompressed TCP/IP, make sure that we are ready for
        it, support it, and can store state for it.  Then update stats and
        process.

        If device is configured to not do IPCP, send a protocol reject,
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_DISABLED)
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        if( ppp_cb_ptr->fsm[IPCP].state != fsmOPENED)
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Not Open for Uncompressed TCP/IP");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        if(!(ppp_cb_ptr->fsm[IPCP].local.work & IPCP_N_COMPRESS) ||
           !(ppp_cb_ptr->fsm[IPCP].local.work & IPCP_N_COMPRESS_VJ))
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Uncompressed TCP/IP Not Enabled");
          pppi_skipped(device, item_ref_ptr);
          break;
        }

        ipcp_p = (ipcp_pdv_type*)(ppp_cb_ptr->fsm[IPCP].pdv);

        PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        if( slhc_decompress(ipcp_p->slhcp,
                            item_ref_ptr,
                            SLHC_PACKET_TYPE_UNCOMPRESSED_TCP) == SLHC_FAILED )
        {
          LOG_MSG_INFO1_0( "Uncompressed TCP/IP Packet Err");
          dsm_free_packet(item_ref_ptr);
          PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
          break;
        }
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

        PPP_INC_INSTANCE_STATS(device,vjuch_pkts_rx,1);

        /* Allocate Metainfo_ex buffer to pass RX metainfo_ptr */
        if (meta_info_ptr != NULL)
        {
          PS_META_INFO_GET_EX(meta_info_ex_ptr);
          if (meta_info_ex_ptr == NULL)
          {
            LOG_MSG_ERROR_0("pppi_receive: Metainfo_ex pool out of memory"  );
            dsm_free_packet(item_ref_ptr);
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            return;
          }
          meta_info_ex_ptr->rx_meta_info_ptr = meta_info_ptr;
        }

        (void)ppp_cb_ptr->fsm[IPCP].iface_input
              (
                ppp_cb_ptr->fsm[IPCP].rx_iface_ptr,
                item_ref_ptr,
                meta_info_ex_ptr
              );
      }
      break;
#ifdef FEATURE_DATA_PS_PPP_ROHC
    case PPP_ROHC_SCID_PROTOCOL:
    case PPP_ROHC_LCID_PROTOCOL:
      /*---------------------------------------------------------------------
        Got ROHC small-CID packet, make sure that we are ready for
        it, support it, and have state for it.  Then update stats and process.
        If device is configured to not do IPCP, send a protocol reject.
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_DISABLED)
      {
        PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
        (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        dsm_item_type *fb_pkt = NULL;
        ip_version_enum_type pkt_ver;
        uint32 ncp_ver;

        if( ppp_cb_ptr->fsm[IPCP].state != fsmOPENED)
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "Not Open for ROHC small-CID");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        ipcp_p = (ipcp_pdv_type*)(ppp_cb_ptr->fsm[IPCP].pdv);

        if (((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS) == 0) ||
            ((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS_ROHC) == 0)||
            ppp_cb_ptr->rohc_handle == NULL)
        {
          PPP_INC_INSTANCE_STATS(device,not_enabled_proto, 1);
          LOG_MSG_INFO1_0( "ROHC Not Enabled");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        if( rohc_decompress(ppp_cb_ptr->rohc_handle,
                            NULL,
                            NULL,
                            item_ref_ptr,
                            &pkt_ver,
                            &fb_pkt,
                            0,
                            &err) == -1)
        {
          LOG_MSG_INFO1_1( "ROHC decompress error 0x%x", err);
          /*-----------------------------------------------------------------
            If the decompressor returned a feedback packet, send it with the
            correct protocol
          -----------------------------------------------------------------*/
          if(fb_pkt != NULL)
          {
            ps_tx_meta_info_type * tx_meta_info_ptr;
            LOG_MSG_INFO2_1("ROHC Feedback packet cid dev (%d)", device);
            /* Get TX meta info pointer */
            PS_TX_META_INFO_AND_RT_META_INFO_GET(tx_meta_info_ptr);

            if((tx_meta_info_ptr == NULL) ||
               (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) == NULL))
            {
              PPP_INC_INSTANCE_STATS(device, pkts_dropped_rx, 1);
              LOG_MSG_ERROR_0( "Meta info - No memory");
              pppi_skipped(device,  item_ref_ptr);
              break;
            }
            /* Copy fields from RX meta info into TX */
            PS_TX_META_INFO_COPY_FROM_RX_META_INFO(meta_info_ptr,
                                                   tx_meta_info_ptr);
            /* Clear RX meta info */
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            pppi_outgoing(device,
                          (((ipcp_value_type *)ppp_cb_ptr->fsm[IPCP].local.
                            work_pdv)->rohc_config.max_cid > 15) ?
                              PPP_ROHC_LCID_PROTOCOL :
                              PPP_ROHC_SCID_PROTOCOL,
                          &fb_pkt,
                          tx_meta_info_ptr);
          }
          pppi_skipped(device,  item_ref_ptr);
          break;
        }
        /*-----------------------------------------------------------------
          If the decompressor returned a feedback packet, send it with the
          correct protocol
        -----------------------------------------------------------------*/
        if(fb_pkt != NULL)
        {
          ps_tx_meta_info_type * tx_meta_info_ptr;
          LOG_MSG_INFO2_1("ROHC Feedback packet cid dev (%d)", device);
          /* Get TX meta info pointer */
          PS_TX_META_INFO_AND_RT_META_INFO_GET(tx_meta_info_ptr);

          if((tx_meta_info_ptr == NULL) ||
             (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) == NULL))
          {
            PPP_INC_INSTANCE_STATS(device, pkts_dropped_rx, 1);
            LOG_MSG_ERROR_0( "Meta info - No memory");
            pppi_skipped(device,  item_ref_ptr);
            break;
          }

          /* Copy fields from RX meta info into TX */
          PS_TX_META_INFO_COPY_FROM_RX_META_INFO(meta_info_ptr,
                                                 tx_meta_info_ptr);
          /* Clear RX meta info */
          PS_RX_META_INFO_FREE(&meta_info_ptr);
          pppi_outgoing(device,
                        (((ipcp_value_type *)ppp_cb_ptr->fsm[IPCP].local.
                          work_pdv)->rohc_config.max_cid > 15) ?
                            PPP_ROHC_LCID_PROTOCOL :
                            PPP_ROHC_SCID_PROTOCOL,
                        &fb_pkt,
                        tx_meta_info_ptr);
        }

#ifdef FEATURE_DATA_PS_IPV6
        ncp_ver = (pkt_ver == IP_V4) ? IPCP : IPV6CP;
#else
        ncp_ver = IPCP;
#endif
        (void)ps_iface_input( ppp_cb_ptr->fsm[ncp_ver].rx_iface_ptr,
                              item_ref_ptr, NULL);
      }
      break;
#endif /* FEATURE_DATA_PS_PPP_ROHC */
#ifdef FEATURE_DATA_PS_PPP_IPHC
    case PPP_IPHC_FULL_HEADER_PROTOCOL:
    case PPP_IPHC_COMPRESSED_TCP_PROTOCOL:
    case PPP_IPHC_COMPRESSED_TCP_NODELTA_PROTOCOL:
    case PPP_IPHC_COMPRESSED_NON_TCP_PROTOCOL:
    case PPP_IPHC_CONTEXT_STATE_PROTOCOL:
    case PPP_IPHC_COMPRESSED_UDP_8_PROTOCOL:
    case PPP_IPHC_COMPRESSED_UDP_16_PROTOCOL:
    case PPP_IPHC_COMPRESSED_RTP_8_PROTOCOL:
    case PPP_IPHC_COMPRESSED_RTP_16_PROTOCOL:
      if(ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_DISABLED)
      {
        (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      }
      else
      {
        dsm_item_type *fb_pkt = NULL;
        uint32 ncp_ver;

        if( ppp_cb_ptr->fsm[IPCP].state != fsmOPENED)
        {
          LOG_MSG_INFO1_0( "Not Open for IPHC");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        ipcp_p = (ipcp_pdv_type*)(ppp_cb_ptr->fsm[IPCP].pdv);

        if (((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS) == 0) ||
            ((ppp_cb_ptr->fsm[IPCP].remote.work & IPCP_N_COMPRESS_IPHC) == 0)||
            ppp_cb_ptr->iphcp == NULL)
        {
          LOG_MSG_INFO1_0( "IPHC not enabled");
          pppi_skipped(device,  item_ref_ptr);
          break;
        }

        PPP_PROTOCOL_TO_IPHC_PACKET_TYPE(protocol, pkt_type);

        if (iphc_decompress(ppp_cb_ptr->iphcp,
                            item_ref_ptr,
                            &pkt_type,
                            &fb_pkt) == -1)
        {
          LOG_MSG_INFO1_0( "IPHC decompress error");
          /*-------------------------------------------------------------------
            If the decompressor returned a feedback packet, send it with the
            correct protocol
          -------------------------------------------------------------------*/
          if (fb_pkt != NULL)
          {
            ps_tx_meta_info_type * tx_meta_info_ptr;
            LOG_MSG_INFO2_1("IPHC Feedback packet dev (%d)", device);
            /* Get TX meta info pointer */
            PS_TX_META_INFO_AND_RT_META_INFO_GET(tx_meta_info_ptr);

            if((tx_meta_info_ptr == NULL) ||
               (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) == NULL))
            {
              PPP_INC_INSTANCE_STATS(device, pkts_dropped_rx, 1);
              LOG_MSG_ERROR_0( "Meta info - No memory");
              pppi_skipped(device,  item_ref_ptr);
              break;
            }

            /* Copy fields from RX meta info into TX */
            PS_TX_META_INFO_COPY_FROM_RX_META_INFO(meta_info_ptr,
                                                   tx_meta_info_ptr);
            /* Clear RX meta info */
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            pppi_outgoing(device,
                          PPP_IPHC_CONTEXT_STATE_PROTOCOL,
                          &fb_pkt,
                          tx_meta_info_ptr);
          }

          pppi_skipped(device,  item_ref_ptr);
          break;
        }
        /*-------------------------------------------------------------------
          If the decompressor returned a feedback packet, send it with the
          correct protocol
        -------------------------------------------------------------------*/
        if (fb_pkt != NULL)
        {
          ps_tx_meta_info_type * tx_meta_info_ptr;
          LOG_MSG_INFO2_1("IPHC Feedback packet dev (%d)", device);
          /* Get TX meta info pointer */
          PS_TX_META_INFO_AND_RT_META_INFO_GET(tx_meta_info_ptr);

          if((tx_meta_info_ptr == NULL) ||
             (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) == NULL))
          {
            PPP_INC_INSTANCE_STATS(device, pkts_dropped_rx, 1);
            LOG_MSG_ERROR_0( "Meta info - No memory");
            pppi_skipped(device,  item_ref_ptr);
            break;
          }

          /* Copy fields from RX meta info into TX */
          PS_TX_META_INFO_COPY_FROM_RX_META_INFO(meta_info_ptr,
                                                 tx_meta_info_ptr);
          /* Clear RX meta info */
          PS_RX_META_INFO_FREE(&meta_info_ptr);
          pppi_outgoing(device,
                        PPP_IPHC_CONTEXT_STATE_PROTOCOL,
                        &fb_pkt,
                        tx_meta_info_ptr);
        }

#ifdef FEATURE_DATA_PS_IPV6
        ncp_ver = (pkt_type == IPHC_PACKET_TYPE_NORMAL_IPV4) ? IPCP : IPV6CP;
#else
        ncp_ver = IPCP;
#endif
        (void)ps_iface_input( ppp_cb_ptr->fsm[ncp_ver].rx_iface_ptr,
                              item_ref_ptr, NULL);
      }
      break;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

    default:
      PPP_INC_INSTANCE_STATS(device, unsupported_proto, 1);
      /*---------------------------------------------------------------------
        Unknown protocol, update stats and generate protocol reject.
      ---------------------------------------------------------------------*/
      (void)pppi_send_prot_rej(device, protocol, *item_ref_ptr);
      break;

  } /* switch(protocol) */

} /* pppi_receive() */



/*===========================================================================
                           PPP PROTOCOL PROCESSING
===========================================================================*/
/*===========================================================================
FUNCTION PPPI_DISCARD()

DESCRIPTION
  This function will queue a PPP Discard packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_discard
(
  ppp_dev_enum_type device,
  dsm_item_type    *item_ptr
)
{
  ppp_type *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid dev 0x%x in pppi_discard", device);
    dsm_free_packet(&item_ptr);
    return -1;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  return
    fsm_send(&(ppp_cb_array[device].fsm[LCP]), DISCARD_REQ, 0, item_ptr);

} /* pppi_discard() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                MISCELLANEOUS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PPPI_DEFAULT_RX()

DESCRIPTION
  This function will simply return NULL indicating to the caller that there
  was no data to receive.

PARAMETERS
  None

RETURN VALUE
  NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_default_rx
(
  void *user_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("pppi_default_rx()!");
  return NULL;

} /* pppi_default_rx() */


/*===========================================================================
FUNCTION PPPI_DEFAULT_TX()

DESCRIPTION
  This function will free any packet that is passed in.

PARAMETERS
  item_ref_ptr: pointer to a reference to a dsm item chain.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_default_tx
(
  dsm_item_type **item_ref_ptr,
  void           *meta_info_ptr,
  void *user_data
)
{
  ps_tx_meta_info_type *temp_ptr = (ps_tx_meta_info_type*)meta_info_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("pppi_default_tx() freeing pkt!");

  PS_TX_META_INFO_FREE(&temp_ptr);
  dsm_free_packet(item_ref_ptr);

} /* pppi_default_tx() */


/*===========================================================================
FUNCTION PPPI_GET_DATA_CBACK()

DESCRIPTION
  This function is intended to be registered with the task that is providing
  the execution context for PPP.  It passes the incoming data to ppp_input
  for processing as per PPP mode.

PARAMETERS
  ppp_intance: the value of the PPP instance is expected to be stopred in the
  void*, that is this should be used as memory NOT as a pointer.

RETURN VALUE
  true:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pppi_get_data_cback
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *dev_instance                       /* user data, indicates the ppp */
)
{
  ppp_dev_enum_type device;
  ppp_type  *ppp_cb_ptr;
  ppp_type  *ppp_cb_array = NULL;
  dsm_item_type *ppp_data_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get the PPP instance we are supposed to be working with (the dev_instance
    is the offset into the ppp_cb_array.
  -------------------------------------------------------------------------*/
  device = ((ppp_dev_enum_type)((uint32)dev_instance));
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return FALSE;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Get the next data item by calling the registered rx function: if there is
    no more data available then return TRUE to indicate the bit can be
    removed from the processing signal mask.
  -------------------------------------------------------------------------*/
  ppp_data_ptr = ppp_cb_ptr->rx_f_ptr(ppp_cb_ptr->rx_user_data);
  if(ppp_data_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      There is no data - so we can have the bit removed from the mask
    -----------------------------------------------------------------------*/
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Send packet to ppp_input()
  -------------------------------------------------------------------------*/
  if( ppp_input( device, ppp_cb_ptr->hdlc_handle, &ppp_data_ptr ) == -1)
  {
    LOG_MSG_INFO2_2("Dev %d, hdlc_handle = 0x%x, ppp_input returned failure",
                    device, ppp_cb_ptr->hdlc_handle);
  }

  /*-------------------------------------------------------------------------
    return FALSE indicating there may still be processing required - don't
    check if there is anything required, as that will introduce overhead in
    the loaded case.
  -------------------------------------------------------------------------*/
  return FALSE;

} /* pppi_get_data_cback() */



/*===========================================================================
FUNCTION PPPI_I_SETUP_TIMEOUT_CB()

DESCRIPTION
  This function is called when the setup timer expires

PARAMETERS
  arg_ptr: void pointer that was registered: the device instance

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  will kill the PPP instance it is called for.
===========================================================================*/
static void pppi_i_setup_timout_cb
(
  void *arg_ptr
)
{
  ppp_dev_enum_type  device;
  ppp_type          *ppp_cb_ptr;
  ppp_type          *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  device = (ppp_dev_enum_type)(uint32)arg_ptr;
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];
  LOG_MSG_INFO1_1("PPP setup timer expired, stopping PPP %d", device);

  if( 0 != ppp_cb_ptr->session_handle )
  {
      ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_TIMEOUT;
      (void)ppp_stop(device, PPP_PROTOCOL_LCP);
  }

} /* pppi_i_setup_timout_cb() */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         LOCAL FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_SEND_PROT_REJ()

DESCRIPTION
  This function will send a protocol reject for an unsupported protocol.

  NOTE: this function is only used internally to this file.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if PROT_REJ successfully sent, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean pppi_send_prot_rej
(
 ppp_dev_enum_type device,             /* device (Rm or Um) pkt arrived on   */
 uint16 protocol,                     /* Bad protocol for which to PROT_REJ*/
 dsm_item_type *item_ptr              /* Pointer to the packet             */
)
{
  ppp_type *ppp_cb_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( item_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid DSM");
    return FALSE;
  }

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in pppi_send_prot_rej");
    dsm_free_packet(&item_ptr);
    return FALSE;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[device]);
  LOG_MSG_INFO1_1( "Unknown PPP protocol: %x", protocol);
  if (dsm_pushdown(&item_ptr,
                   NULL,
                   sizeof(protocol),
                   DSM_DS_SMALL_ITEM_POOL) < sizeof(protocol))
  {
    dsm_free_packet(&item_ptr);
    DATA_ERR_FATAL(" Running out of memory: dumping PPP frames");
    return FALSE;
  }
  (void)put16(item_ptr->data_ptr,protocol);
  (void)fsm_send( &(ppp_cb_ptr->fsm[LCP]), PROT_REJ, 0, item_ptr);

  return TRUE;

} /* pppi_send_prot_rej() */



/*===========================================================================
FUNCTION PPPI_SKIPPED()

DESCRIPTION
  This function will Skip Incoming Packet routine

  NOTE: this funtion is only used internally to this file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_skipped
(
  ppp_dev_enum_type device,
  dsm_item_type **item_head_ptr
)
{
  ipcp_pdv_type *ipcp_ptr;
  ppp_type      *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid args in pppi_skipped", device);
    dsm_free_packet(item_head_ptr);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  PPP_INC_INSTANCE_STATS(device,pkts_dropped_rx,1);
  /* IPv6 TODO: Does this need to be modified for IP6CP interface as well */
  ipcp_ptr = (ipcp_pdv_type*) (ppp_cb_array[device].fsm[IPCP].pdv);
  if( ( ipcp_ptr != NULL ) && ( ipcp_ptr->slhcp != NULL ) )
  {
    /* Make a call to slhc_ioctl to set the toss flag */
    (void)slhc_ioctl(ipcp_ptr->slhcp, SLHC_IOCTL_SET_TOSS, NULL);
  }
  dsm_free_packet(item_head_ptr);

} /* pppi_skipped() */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION PPPI_FAILOVER_ON_RESYNC()

DESCRIPTION
  This function is called to handle failover on resyncs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Performs the failover logic
===========================================================================*/
void pppi_failover_on_resync
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol
)
{
  ppp_type            *ppp_cb_ptr;
  ppp_type            *ppp_cb_array = NULL;
  ppp_protocol_e_type  pref;
  ppp_protocol_e_type  nonpref;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device )
  {
    ASSERT( 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    failover enum being manipulated in application context: TASKLOCK()
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Perform failover logic only if protocol being resyned is LCP and the
    failover enum is set
  -------------------------------------------------------------------------*/
  if( ( PPP_PROTOCOL_LCP == protocol ) &&
      ( PPP_FAILOVER_IGNORE != ppp_cb_ptr->failover ) )
  {
    /*-----------------------------------------------------------------------
      Determine preferred and non-preferred NCP
    -----------------------------------------------------------------------*/
    switch( ppp_cb_ptr->failover )
    {
      case PPP_FAILOVER_V4_THEN_V6:
        pref    = PPP_PROTOCOL_IPCP;
        nonpref = PPP_PROTOCOL_IPV6CP;
        break;

      case PPP_FAILOVER_V6_THEN_V4:
        pref    = PPP_PROTOCOL_IPV6CP;
        nonpref = PPP_PROTOCOL_IPCP;
        break;

      default:
        PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
        LOG_MSG_ERROR_1("Invalid value of failover %d", ppp_cb_ptr->failover);
        ASSERT(0);
        return;
    } /* switch( ppp_cb_ptr->failover ) */

    /*-----------------------------------------------------------------------
      The logic here is that an NCP that is non-preferred is brought up only
      if:
        *  Preferred NCP is also up
        *  Preferred NCP is down but ref count of associated PS iface is > 1

      This is achieved by the following implementation:

      Set non-preferred NCP to PPP_CP_MODE_DISCARD if all of these hold:
      *  non-Preferred NCP is set to ACTIVE
      *  Preferred NCP is down (mode is not ACTIVE)
      *  Reference count of non-preferred NCP is 1
    -----------------------------------------------------------------------*/
    if( PPP_CP_MODE_ACTIVE == ppp_cb_ptr->fsm[nonpref].mode &&
        PPP_CP_MODE_ACTIVE != ppp_cb_ptr->fsm[pref].mode &&
        1                  == PS_IFACE_GET_REF_CNT(
                                ppp_cb_ptr->fsm[nonpref].rx_iface_ptr) )
    {
      ppp_cb_ptr->fsm[nonpref].mode = PPP_CP_MODE_DISCARD;
    }

    /*-----------------------------------------------------------------------
      Set preferred NCP's mode to ACTIVE
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->fsm[pref].mode = PPP_CP_MODE_ACTIVE;
  }

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  return;
} /* pppi_failover_on_resync() */
#endif /*FEATURE_DATA_PS_IPV6*/



/*===========================================================================
FUNCTION PPPI_CALL_EVENT_CALLBACK()

DESCRIPTION
  Traverses the PPP event queue and calls the passed event on the
  passed protocol.  If the protocol is LCP and the event is RESYNC
  then it also calls the callbacks on the NCPs.

  NOTE: This function TASKLOCKs inside and need not be called with TASKLOCK

  NOTE: The callback function should not dealloc the event notification
        structure

  NOTE: For the DOWN event, the caller is expected to call this function
        for all the appropriate protocols (e.g. separately for LCP, IPCP
        and IPv6CP). Note that this function does not call the
        callbacks for the NCPs for DOWN event, unlike the RESYNC event
        in which the appropriate callbacks are called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Generates PPP event callbacks
===========================================================================*/
void pppi_call_event_callback
(
  ppp_fsm_type        *fsm_ptr,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    event
)
{
  ppp_event_info_s_type *event_ptr;
  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
#ifdef FEATURE_DATA_PS_EHRPD
  ncpi_cb_type *ncp_handle_ptr = NULL;  
#endif /* FEATURE_DATA_PS__EHRPD */
  uint8                  i;
  ppp_event_mask_e_type  event_mask = 
    (ppp_event_mask_e_type)(1 << (uint8)event);
  ppp_fail_reason_e_type fail_reason = PPP_FAIL_REASON_NONE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if(NULL == fsm_ptr)
  {
    DATA_ERR_FATAL("pppi_call_event_callback: fsm_ptr is NULL");
    return;
  }

  if (PPP_MAX_DEV <= fsm_ptr->device 
      || PPP_PROTOCOL_MAX <= protocol 
      || PPP_MAX_EV <= event)
  {
    LOG_MSG_ERROR_3( "Invalid args passed. device: %d, protocol: %d, event: %d",
                     fsm_ptr->device, protocol, event );
    ASSERT( 0 );
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[fsm_ptr->device];

  LOG_MSG_INFO1_3("Protocol = %d , Event called = %d Fail Reason = %d",
                protocol, event, fsm_ptr->fail_reason );

  /*-------------------------------------------------------------------------
    Evaluate the fail reason if event to post is DOWN event
  -------------------------------------------------------------------------*/
  if( PPP_DOWN_EV == event )
  {
    fail_reason = fsm_ptr->fail_reason;
    fsm_ptr->fail_reason = PPP_FAIL_REASON_NONE;
  }

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );

  /*-------------------------------------------------------------------------
    Traverse queue of PPP event info structures
  -------------------------------------------------------------------------*/
  event_ptr = q_check( &ppp_cb_ptr->event_queue );

  while( NULL != event_ptr )
  {
    switch(protocol)
    {
      case PPP_PROTOCOL_LCP:
      case PPP_PROTOCOL_AUTH:  
      case PPP_PROTOCOL_IPCP:
      case PPP_PROTOCOL_IPV6CP:
      /*----------------------------------------------------------------------
        If the protocol is LCP and the event is RESYNC we need to also inform
        any active NCPs (though the fail reason for RESYNC case is always
        PPP_FAIL_REASON_NONE)
        The for loop posts the event for IPCP, IPV6CP and ncpi_all_resync 
        posts the event to all active VSNCPs.
      ----------------------------------------------------------------------*/
        if( ( PPP_PROTOCOL_LCP == protocol ) &&  ( PPP_RESYNC_EV == event ) )
        {
          for(i = fsmi_Size - 1; i > LCP; i--)
          {
            /*---------------------------------------------------------------
              If the NCP is registered for this protocol and is not in the
              fsmINITIAL state then call the callback.
            ---------------------------------------------------------------*/
            if( 0 != ((uint8)event_mask & (uint8)event_ptr->event_mask[i]) &&
                fsmINITIAL != ppp_cb_ptr->fsm[i].state )
            {
              event_ptr->cback[i]( event_ptr->device,
                                   (ppp_protocol_e_type)i,
                                   event,
                                   event_ptr->user_data[i],
                                   ppp_cb_ptr->session_handle,
                                   PPP_FAIL_REASON_NONE
                                 );
            }
          } /* for (fsm_size) */
    
  #ifdef FEATURE_DATA_PS_EHRPD
          ncpi_resync_all_ncps ( fsm_ptr->device, event_ptr );            
  #endif /* FEATURE_DATA_PS_EHRPD*/
    
        } /* ( protocol == LCP, event == resync ) */
  
        /*-------------------------------------------------------------------
          Call the callback on the passed protocol if it is registered for 
          the event
        -------------------------------------------------------------------*/
        if(0 != ((uint8)event_mask & (uint8)event_ptr->event_mask[protocol]))
        {
          event_ptr->cback[protocol]( event_ptr->device,
                                      protocol,
                                      event,
                                      event_ptr->user_data[protocol],
                                      ppp_cb_ptr->session_handle,
                                      fail_reason
                                    );
        }
        break;

#ifdef FEATURE_DATA_PS_EHRPD
      case PPP_PROTOCOL_VSNCP:
        ncp_handle_ptr = (ncpi_cb_type *)fsm_ptr->pdv;
        if( NCPI_CB_IS_INITED( ncp_handle_ptr ) == FALSE )
        {
          DATA_ERR_FATAL("Invalid ncp_handle");
          return;
        }
        /*-----------------------------------------------------------------
          Call the callback on the VSNCP instance  if it is registered for 
          the event.
       ------------------------------------------------------------------*/
        if( ( 0 != ( (uint8)event_mask & (uint8)event_ptr->event_mask[
                    protocol] ) ) &&
            ( ncp_handle_ptr == 
                ( (ncpi_cb_type*)event_ptr->ncp_handle_ptr ) ) )
        {
          LOG_MSG_INFO1_1("Calling cback for NCP instance 0x%x",
                          ncp_handle_ptr);
          event_ptr->cback[protocol]( event_ptr->device,
                                      protocol,
                                      event,
                                      event_ptr->user_data[protocol],
                                      ppp_cb_ptr->session_handle,
                                      fail_reason );
          /*-----------------------------------------------------------------
            Post the VSNCP diag event for resync
          -----------------------------------------------------------------*/
          if( PPP_RESYNC_EV == event )
          {
            NCPI_REPORT_EVENT_TO_DIAG(ncp_handle_ptr,PS_PPP_EVENT_RESYNC);
          }
        }
      break;
#endif /* FEATURE_DATA_PS_EHRPD */

      default:
        LOG_MSG_ERROR_1("Invalid protocol %d", protocol);
        PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );
        ASSERT( 0 );
        return;
    } /*End of switch(protocol)*/

    /*-----------------------------------------------------------------------
      Get the next node
    -----------------------------------------------------------------------*/
    event_ptr = q_next( &ppp_cb_ptr->event_queue, &event_ptr->link );
  }

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(fsm_ptr->device) );

  return;
} /* pppi_call_event_callback() */


/*===========================================================================
FUNCTION PPPI_REPORT_EVENT_TO_DIAG()

DESCRIPTION
  Creates the ppp event payload per the information passed in, and sends the
  event to DIAG via event_report_payload()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_report_event_to_diag
(
  event_id_enum_type event_id,
  ppp_dev_enum_type device,
  ppp_protocol_e_type protocol,
  ppp_event_state_type prev_state,
  ppp_event_state_type state
)
{
  ppp_event_payload_type ppp_event_payload;
  ppp_event_protocol_type event_protocol;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device ||
      PPP_PROTOCOL_MAX <= protocol )
  {
    LOG_MSG_ERROR_2( "Invalid args passed. device: %d, protocol: %d",
                     device, protocol );
    ASSERT( 0 );
    return;
  }

  /*-----------------------------------------------------------------------
    Obtain equivalent ppp_event_protocol_type value for the passed protocol
  -----------------------------------------------------------------------*/
  switch(protocol)
  {
  case PPP_PROTOCOL_LCP:
    event_protocol = PPP_EV_LCP;
    break;

  case PPP_PROTOCOL_IPCP:
    event_protocol = PPP_EV_IPCP;
    break;

  case PPP_PROTOCOL_IPV6CP:
    event_protocol = PPP_EV_IPv6CP;
    break;

#ifdef FEATURE_DATA_PS_EHRPD
  case PPP_PROTOCOL_VSNCP:
    event_protocol = PPP_EV_VSNCP;
    break;
#endif /* FEATURE_DATA_PS_EHRPD */

  case PPP_PROTOCOL_AUTH:
  case PPP_PROTOCOL_MAX:
  default:
    event_protocol = PPP_EV_INVALID_PROTO;
    return;
  } /* switch(protocol) */

  /*-----------------------------------------------------------------------
    Fill in the event payload fields
  -----------------------------------------------------------------------*/
  ppp_event_payload.ppp_event_device     = (int)device;
  ppp_event_payload.ppp_event_protocol   = event_protocol ;
  ppp_event_payload.ppp_event_prev_state = prev_state;
  ppp_event_payload.ppp_event_state      = state ;

  LOG_MSG_INFO2_3("pppi_report_event_to_diag: dev (%d), prot (%d), state (%d)",
          device, event_protocol, state);
  /*-----------------------------------------------------------------------
    Report event to DIAG
  -----------------------------------------------------------------------*/
  event_report_payload( event_id,
                        sizeof(ppp_event_payload),
                    &ppp_event_payload);
}


/*===========================================================================
FUNCTION PPPI_NONE_MODE_HDLR()

DESCRIPTION
   Frees up the packet passed in

PARAMETERS
  device    : PPP device to pass the packet to
  hdlc inst : instance of HDLC for which the received packet belongs to
  packet    : pointer to the incoming packet.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_none_mode_hdlr
(
  ppp_dev_enum_type   device,               /* PPP device to pass pkt to   */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** packet                /* Pointer to pkt              */
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in pppi_none_mode_hdlr");
    return;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  LOG_MSG_INFO2_2("PPP discarding received packet in NONE mode(dev %d, mode %d)",
                  device, ppp_cb_array[device].mode);
  dsm_free_packet( packet );
  return;
} /* pppi_none_mode_hdlr() */


/*===========================================================================
FUNCTION PPPI_RELAY_MODE_HDLR()

DESCRIPTION
   Invalid condition, PPP should not be receiving input packets in RELAY
   mode. This function will ASSERT if ever called. Frees the packet passed in
   and returns.

PARAMETERS
  device    : PPP device to pass the packet to
  hdlc inst : instance of HDLC for which the received packet belongs to
  packet    : pointer to the incoming packet.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_relay_mode_hdlr
(
  ppp_dev_enum_type   device,               /* PPP device to pass pkt to   */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** packet                /* Pointer to pkt              */
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in pppi_relay_mode_hdlr");
    return;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  LOG_MSG_INFO2_2("PPP discarding received packet in RELAY mode(dev %d, mode %d)",
                  device, ppp_cb_array[device].mode);
  ASSERT( 0 );
  dsm_free_packet( packet );
  return;
} /* pppi_relay_mode_hdlr() */


/*===========================================================================
FUNCTION PPPI_INTERNAL_MODE_HDLR()

DESCRIPTION
   This function handles the common processing for the "internal" PPP modes,
   i.e. modes requiring complete HDLC unframing if HDLC is enabled and
   dispatch upwards to the stack (pppi_incoming). These modes are
   PPP_INTERNAL_MODE, PPP_INTERNAL_SNOOP_MODE and PPP_FULL_NET_MODE.
   This handler function is invoked from ppp_input and assumes that the
   PPP device is configured in one of the above modes.

PARAMETERS
  device    : PPP device to pass the packet to
  hdlc inst : instance of HDLC to which the received packet belongs to
  framed    : pointer to the PPP packet which needs to get unframed.

RETURN VALUE
  None

DEPENDENCIES
  This function gets executed in PS task.

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_internal_mode_hdlr
(
  ppp_dev_enum_type   device,               /* PPP device to pass pkt to   */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** framed                /* Pointer to PPP pkt          */
)
{
  dsm_item_type        * unframed = NULL;
  hdlc_errno_enum_type   ps_errno    = HDLC_OK;
  int                    retval   = 0;
  ppp_type             * ppp_cb_ptr = NULL;
  ppp_type             * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( framed == NULL || *framed == NULL )
  {
    LOG_MSG_ERROR_0("Invalid DSM");
    return;
  }

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid device in internal mode hdlr");
    dsm_free_packet( framed );
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[device]);
  ASSERT( ppp_cb_ptr->mode == PPP_INTERNAL_MODE ||
          ppp_cb_ptr->mode == PPP_INTERNAL_SNOOP_MODE ||
          ppp_cb_ptr->mode == PPP_FULL_NET_MODE );

  /*-------------------------------------------------------------------------
    Skip unframing operation and send packet up the stack if HDLC mode is OFF
    Possible optimization: If this is only specific to a particular mode
    (UMTS et. al.), can look into having a separate mode and rx_pkt_hdlr
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->hdlc_mode == HDLC_OFF
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
  || ppp_cb_ptr->hdlc_mode == HDLC_A2_AUTONOMOUS
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */
    )
  {
    pppi_incoming(device, framed);
    return;
  }

  /*-------------------------------------------------------------------------
    Per hdlc_unframe() API semantic, need to call hdlc_unframe() until
    *framed goes NULL and ps_errno returned is HDLC_OK.
    If HDLC_BAD_HANDLE is returned, need to quit immediately.
  -------------------------------------------------------------------------*/
  while( ( *framed != NULL ) || ( ps_errno != HDLC_OK ) )
  {
    unframed = NULL;
    retval   = hdlc_unframe( hdlc_inst, framed, &unframed, NULL );
    ps_errno    = hdlc_errno( hdlc_inst );
    if( retval < 0 )
    {
      switch( ps_errno )
      {
        case HDLC_MORE_PKTS_AVAIL:
          /*----------------------------------------------------------------
            The unframer has more packets available, pass the current
            packet returned upwards to the stack, and invoke the unframer
            again.
          ----------------------------------------------------------------*/
          LOG_MSG_INFO3_0("HDLC Unframer: More packets available");
          if( unframed != NULL )
          {
            pppi_incoming( device, &unframed );
          }
          continue;

        case HDLC_BAD_HANDLE:
          LOG_MSG_ERROR_3("PPP unframe error %d, framed = (%p), unframed = "
                    "(%p) discarding pkt", ps_errno, *framed, unframed);
          dsm_free_packet( framed );
          dsm_free_packet( &unframed );
          return;

        default:
          LOG_MSG_ERROR_3("PPP unframe error %d, framed = (%p), unframed = "
                    "(%p)", ps_errno, *framed, unframed);
          break;
      } /* switch (ps_errno) */

      if( unframed == NULL )
      {
        continue;
      }
    } /* if(retval < 0) */

    /*-----------------------------------------------------------------------
      NOTE: It is possible to get unframed data even when hdlc_unframe()
      returns an error. Dispatch the complete packet for PPP receive
      processing. If unframing is not complete, we are waiting for remaining
      pkt, the current pkt must have been exhausted, hence the Assert.
    -----------------------------------------------------------------------*/

    if( unframed != NULL )
    {
      pppi_incoming( device, &unframed );
    }
    else
    {
      ASSERT( *framed == NULL );
    }
  } /* while( ( *framed != NULL ) || ( ps_errno != HDLC_OK ) ) */

} /* pppi_internal_mode_hdlr() */


/*===========================================================================

                PPP PSEUDONET STATE MACHINE IMPLEMENTATION

  This section contains the internal functions used to implement the
  PPP pseudonet state machine.

===========================================================================*/
/*===========================================================================
FUNCTION PPPI_PSEUDONET_RESET()

DESCRIPTION
  This function will reset the pseudonet state machine

PARAMETERS
  device    : PPP device to reset the pseudonet state machine for

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_pseudonet_reset
(
  ppp_dev_enum_type device
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_1("Resetting Pseudonet state machine, moving to FINDING_7E",
                  device);
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Free any items that are hanging around, memset the control block
    set state to FINDING_7E
  -------------------------------------------------------------------------*/
  dsm_free_packet( &( ppp_cb_ptr->pseudonet.snoop_pkt_ptr ) );
  dsm_free_packet( &( ppp_cb_ptr->pseudonet.framed_pkt_ptr ) );
  ppp_cb_ptr->pseudonet.framed_pkt_tail_ptr = NULL;
  memset( &( ppp_cb_ptr->pseudonet ), 0, sizeof(ppp_pseudonet_type) );
  ppp_cb_ptr->pseudonet.state = FINDING_7E;
} /* pppi_pseudonet_reset() */


/*===========================================================================
FUNCTION PPPI_PSEUDONET_FORWARD_DATA

DESCRIPTION
  This function is used to forward data to the bridge device.
  It also logs the outgoing bytes by invoking the legacy PPP and DPL logging
  macros.

PARAMETERS
  device          : (IN) PPP device
  pkt_to_forward  : (IN) packet to forward

RETURN VALUE
  None

DEPENDENCIES
  Args passed in to the function are assumed to be valid (too many if checks
  in data path et. al.)

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_pseudonet_forward_data
(
  ppp_dev_enum_type   device,
  dsm_item_type     * pkt_to_forward
)
{
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_bridge_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;

  ps_phys_link_type  * phys_link_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid dev in pseudonet fwd data");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[device]);
  if(ppp_cb_ptr->bridge_dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_0("Invalid bridge dev in pseudonet fwd data");
    return;
  }
  ppp_bridge_cb_ptr = &ppp_cb_array[ppp_cb_ptr->bridge_dev];
  if( pkt_to_forward != NULL )
  {
#ifdef FEATURE_DATA_PS_PPP_LOGGING
    PPP_LOG_FRAMES_EXT ( pkt_to_forward,
                         PPP_LOG_FRAMED_TX,
                         ppp_cb_ptr->bridge_dev );
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

    phys_link_ptr =
      PS_IFACE_GET_PHYS_LINK(ppp_bridge_cb_ptr->fsm[IPCP].rx_iface_ptr);
    if(phys_link_ptr != NULL)
    {
      DPL_LOG_PPP_PARTIAL_TX_PACKET(phys_link_ptr, pkt_to_forward);
    }

    ppp_bridge_cb_ptr->tx_f_ptr( &pkt_to_forward,
                                 NULL,
                                 ppp_bridge_cb_ptr->tx_user_data );
  }
} /* pppi_pseudonet_forward_data() */


/*===========================================================================
FUNCTION PPPI_PSEUDONET_FINDING_EOP_PROC

DESCRIPTION
  This function is used by the PPP pseudonet state machine for FINDING_EOP
  processing. It forwards any data hanging around from SNOOPING in
  framed_pkt_ptr and then examines the incoming DSM chain to find the
  end-of-packet indicator (0x7e).

  If a 7e is found, it forwards the packet to the bridge device,
  frees the snoop_pkt_ptr and moves to FINDING_SOP state (since a 7e was
  just seen, we know we have a the beginning of a new packet).

  If a 7e is not seen, pseudonet state machine stays in FINDING_EOP state.

  Error scenario: If a 7e is found and the DUP operation fails, the trailing
  data will be lost, and function will return a failure.

  On return from this function, *in_pkt_ptr will point to the data needing
  further processing by the calling function (note that this can be
  NULL in which case no further processing will be needed).

PARAMETERS
  param_ptr   : (IN) Pointer to pseudonet im the PPP control block
  device      : (IN) PPP device
  in_pkt_ptr  : (IN)  Points to the DSM chain being looked at by PN SM
                (OUT) Points to the DSM chain needing further processing
                      by PN SM. This can be NULL.
  offset_in   : (IN)  Offset into the first DSM in the passed DSM chain,
                      to start looking for 0x7e


RETURN VALUE
   0: success
  -1: failure (this will be returned if the DUP'ing failed)

DEPENDENCIES
  Args passed in to the function are assumed to be valid (too many if checks
  in data path et. al.)

SIDE EFFECTS
  *in_pkt_ptr will be modified to point to the data that needs further
  processing by pppi_pseudonet_mode_hdlr(). Note that this can be NULL.
===========================================================================*/
static int pppi_pseudonet_finding_eop_proc
(
  ppp_pseudonet_type * param_ptr,
  ppp_dev_enum_type    device,
  dsm_item_type     ** in_pkt_ptr,
  uint16               offset_in
)
{
  dsm_item_type * rest_of_chain       = NULL;  /* ptr to next DSM in chain */
  dsm_item_type * left_over_data      = NULL;  /* ptr to DUP'ed item       */
  uint8         * read_ptr            = NULL;  /* examine bytes in DSM     */
  uint8         * end_ptr             = NULL;  /* bounds                   */
  boolean         found_end_of_packet = FALSE;
  int             retval              = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( ( in_pkt_ptr != NULL ) && ( (*in_pkt_ptr) != NULL ) );

  /*-------------------------------------------------------------------------
    Forward any data hanging around in framed_pkt_ptr from before (SNOOPING)
  -------------------------------------------------------------------------*/
  pppi_pseudonet_forward_data( device, param_ptr->framed_pkt_ptr );
  param_ptr->framed_pkt_ptr = NULL;
  param_ptr->framed_pkt_tail_ptr = NULL;

  /*-------------------------------------------------------------------------
    For the first DSM in the chain, start looking from offset_in bytes
  -------------------------------------------------------------------------*/
  rest_of_chain = (*in_pkt_ptr)->pkt_ptr;
  (*in_pkt_ptr)->pkt_ptr = NULL;
  read_ptr      = (*in_pkt_ptr)->data_ptr + offset_in;
  end_ptr       = (*in_pkt_ptr)->data_ptr + (*in_pkt_ptr)->used;

  ASSERT( read_ptr <= end_ptr );
  while( *in_pkt_ptr != NULL )
  {
    for( ; ( *read_ptr != HDLC_FLAG && read_ptr < end_ptr ) ; read_ptr++ )
    {
      /* NOOP */
    }
    if( read_ptr == end_ptr )
    {
      if( *read_ptr == HDLC_FLAG ) /* the last byte of the DSM is a 0x7e   */
      {
        found_end_of_packet = TRUE;
        break;
      }

      pppi_pseudonet_forward_data( device, *in_pkt_ptr );
      *in_pkt_ptr = rest_of_chain;
      if( *in_pkt_ptr != NULL )
      {
        (*in_pkt_ptr)->pkt_ptr = NULL;
        read_ptr      = (*in_pkt_ptr)->data_ptr;
        end_ptr       = (*in_pkt_ptr)->data_ptr + (*in_pkt_ptr)->used;
        rest_of_chain = rest_of_chain->pkt_ptr;
      }
      continue;
    } /* reached end of this DSM item */

    ASSERT( *read_ptr == HDLC_FLAG );
    found_end_of_packet = TRUE;
    break;
  } /* while( *in_pkt_ptr != NULL ) */

  if( found_end_of_packet == TRUE )
  {
    /*-----------------------------------------------------------------------
      Found a 0x7e flag. If there are more bytes in this item, DUP them.
      +--+--+--+--+--+--+--+--+--+   The bytes that need to be dup'ed
      | 1st pkt      |~ | 2nd pkt|   are between end and read.
      +--+--+--+--+--+--+--+--+--+
                     ^           ^   Note that (read_ptr + 1 == end_ptr)
                     |           |   means that read_ptr is currently
      read ----------+           |   pointing to the last byte in the DSM,
      end  ----------------------+   so DUP only if end_ptr > read_ptr + 1
    -----------------------------------------------------------------------*/
    if( end_ptr > read_ptr + 1 )
    {
      if(dsm_dup_packet(
          /* dup_ptr */  &left_over_data,
          /* src_ptr */  ( *in_pkt_ptr ),
          /* offset  */  ( read_ptr - ( (*in_pkt_ptr)->data_ptr ) ),
          /* cnt     */  ( end_ptr - read_ptr)
                       ) != ( end_ptr - read_ptr ) )
      {
        LOG_MSG_ERROR_1("PPP pseudonet, failed to DUP after ~ in %p",
                        *in_pkt_ptr);
        retval = -1;
      }

      /*---------------------------------------------------------------------
        Trim passed DSM to item to the offset where 7e was found
      ---------------------------------------------------------------------*/
      dsm_trim_packet( in_pkt_ptr,
                       read_ptr - ( (*in_pkt_ptr)->data_ptr ) );
    }

    /*----------------------------------------------------------------------
      We have a 0x7e, forward packet to bridge device, free the snoop pkt
      and move to FINDING_SOP. *in_pkt_ptr on the way out points to
      the data needing further processing by pseudonet state machine.
    ----------------------------------------------------------------------*/
    pppi_pseudonet_forward_data( device, *in_pkt_ptr );
    dsm_free_packet( &(param_ptr->snoop_pkt_ptr) );
    dsm_append( &left_over_data, &rest_of_chain );
    *in_pkt_ptr = left_over_data;
    param_ptr->state = FINDING_SOP;
  } /* if( found_end_of_packet == TRUE ) */

  return retval;
} /* pppi_pseudonet_finding_eop_proc() */


/*===========================================================================
FUNCTION PPPI_PSEUDONET_NEXT_STATE

DESCRIPTION
  This function is used by the pseudonet state machine to figure out the
  next state while snooping. If 2 or 4 bytes of the incoming PPP packet
  have been unframed, this function invokes snoop_early_proto_detect() to
  see if we are interested in this packet.

PARAMETERS
  param_ptr : Pointer to pseudonet im the PPP control block
  device    : PPP device

RETURN VALUE
  hdlc_unframe_state_type : The next state PPP pseudonet state machine
                            should move to

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static hdlc_unframe_state_type pppi_pseudonet_next_state
(
  ppp_pseudonet_type * param_ptr,
  ppp_dev_enum_type    device
)
{
  snoop_proto_ret_val_enum_type snoop_res;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ( ( param_ptr->snoop_pkt_ptr )->used == PPP_PROTO_SIZ ) ||
      ( ( param_ptr->snoop_pkt_ptr )->used == PPP_ACF_PROTO_SIZ )
    )
  {
    snoop_res = snoop_early_proto_detect( device,
                                          param_ptr->snoop_pkt_ptr );
    switch( snoop_res )
    {
      case SNOOP_IGNORE_PKT:
        LOG_MSG_INFO3_1(" %d bytes snooped - not interested in pkt",
                        (param_ptr->snoop_pkt_ptr)->used);
        dsm_free_packet( &( param_ptr->snoop_pkt_ptr ) );
        return FINDING_EOP;

      case SNOOP_DO_UNFRAMING:
        LOG_MSG_INFO3_1(" %d bytes snooped - might have protocol",
                        (param_ptr->snoop_pkt_ptr)->used);
        return UNFRAMING;

      case SNOOP_NEXT_TWO_BYTES:
        LOG_MSG_INFO3_1(" %d bytes snooped - need to unframe more",
                        (param_ptr->snoop_pkt_ptr)->used);
        return SNOOPING;

      default:
        ASSERT(0);
    } /* switch( snoop_res ) */
  } /* if 2 or 4 bytes unframed */

  return SNOOPING;
} /* pppi_pseudonet_next_state() */


/*===========================================================================
FUNCTION PPPI_PSEUDONET_HDLC_UNFRAME

DESCRIPTION
  This function is used to unframe the framed input PPP packet.
  On returning from the hdlc_unframe() call, the following actions are
  performed. Note that if the hdlc_unframe() API invokation results in
  HDLC_BAD_HANDLE error code, an error is returned.

  1. If a complete packet is unframed, it sends the packet up the stack
     (pppi_incoming), frees the snoop_pkt_ptr and moves to FINDING_SOP state
     (since a 7e was just seen (inside HDLC library), we know we have a the
     beginning of a new packet).
  2. If a complete packet is not yet unframed and the whole input chain is
     exhausted, pseudonet state machine stays in UNFRAMING
  3. Error scenarios:
     3.1 HDLC lib returns HDLC_BAD_HANDLE:
         Function returns -1. pseudonet state machine should cleanup and
         return.
     3.2 HDLC lib returns an error other than HDLC_BAD_HANDLE:
         - If there is some unframed data, processing is same as 1. above.
         - If there is no unframed data, but there is still more data in
           incoming chain, the function resets the pseudonet state machine.

  On return from this function, *framed will point to the data needing
  further processing by the calling function (note that his can be
  NULL in which case no further processing will be needed).

PARAMETERS
  param_ptr       : (IN) Pointer to pseudonet im the PPP control block
  device          : (IN) PPP device
  hdlc_handle     : (IN) Handle to the HDLC instance
  framed          : (IN)  Points to the original framed DSM chain passed in
                          to PN state machine.
                    (OUT) Points to the DSM chain needing further processing
                          by PN state machine. This can be NULL.

RETURN VALUE
   0: success
  -1: failure (this will be returned if the HDLC handle was bad)

DEPENDENCIES
  Args passed in to the function are assumed to be valid (too many if checks
  in data path et. al.)

SIDE EFFECTS
  *framed will will be modified to point to the data that needs further
  processing by pppi_pseudonet_mode_hdlr(), unless HDLC_BAD_HANDLE
  is returned by HDLC lib, in which case pppi_pseudonet_mode_hdlr() should
  cleanup and return immediately.
===========================================================================*/
static int pppi_pseudonet_hdlc_unframe
(
  ppp_pseudonet_type * param_ptr,
  ppp_dev_enum_type    device,
  void               * hdlc_handle,
  dsm_item_type     ** framed
)
{
  dsm_item_type        * unframed = NULL;
  hdlc_errno_enum_type   ps_errno;
  int                    retval;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  retval = hdlc_unframe( hdlc_handle, framed, &unframed, NULL );
  if( retval < 0 )
  {
    /*-----------------------------------------------------------------------
      Get specific error. Return immediately for BAD handle. Else carry on.
    -----------------------------------------------------------------------*/
    ps_errno = hdlc_errno( hdlc_handle );
    switch( ps_errno )
    {
      case HDLC_BAD_HANDLE:
        LOG_MSG_ERROR_1("PPP dev %d, Bad hdlc handle (0x%x) error",
                        hdlc_handle );
        return -1;

      default:
        LOG_MSG_ERROR_3("HDLC inst (0x%x) unframing error %d, unframed = (%p)",
                   hdlc_handle, ps_errno, unframed);
        break;
    } /* switch( ps_errno ) */
  } /* if( retval < 0 ) */

  if( unframed != NULL )
  {
    /*-----------------------------------------------------------------------
      Send unframed pkt up. Free snoop_pkt_ptr. Move to FINDING_SOP.
    -----------------------------------------------------------------------*/
    pppi_incoming( device, &unframed );
    dsm_free_packet( &(param_ptr->snoop_pkt_ptr) );
    param_ptr->state = FINDING_SOP;
  }
  else
  {
    if( *framed != NULL )
    {
      /*---------------------------------------------------------------------
        No unframed packet, but still more data in input packet. Prolly
        an error from hdlc_unframe(). Reset and continue.
      ---------------------------------------------------------------------*/
      pppi_pseudonet_reset( device );
    }
  } /* if( unframed == NULL ) */

  return 0;
} /* pppi_pseudonet_hdlc_unframe() */


/*===========================================================================
FUNCTION PPPI_PSEUDONET_SWITCH_MODE

DESCRIPTION
  This function is invoked from with pppi_pseudonet_mode_hdlr() to switch
  from pseudonet mode to one of the internal modes. Invokes HDLC library
  ioctls to set HDLC lib unframing state to FINDING_SOP (since we are now
  moving to a new packet), and move to hardware mode (desirable, since
  moving to an "internal" mode).

PARAMETERS
  device     : (IN) PPP device to switch the mode for
  ppp_cb_ptr : Pointer to control block for device
  in_pkt_ptr : pointer to the PPP packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_pseudonet_switch_mode
(
  ppp_dev_enum_type   device,
  ppp_type          * ppp_cb_ptr,
  dsm_item_type    ** in_pkt_ptr
)
{
  hdlc_ioctl_u_type        ioctl_data;
  hdlc_errno_enum_type     ps_errno;
  hdlc_instance_type       *instance = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( ppp_cb_ptr->pending_mode == PPP_INTERNAL_MODE ||
          ppp_cb_ptr->pending_mode == PPP_INTERNAL_SNOOP_MODE ||
          ppp_cb_ptr->pending_mode == PPP_FULL_NET_MODE );
  instance = (hdlc_instance_type*)(ppp_cb_ptr->hdlc_handle);
  /*-------------------------------------------------------------------------
    Move the HDLC unframing state machine to FINDING_SOP, since we are about
    to supply it with a new packet which most likely won't have a 0x7e in
    front. This will work since we have already seen one 0x7e during EOP
    processing, which is sufficient to indicate that we have the start of a
    new packet, so SOP is the way to go.
    If the ioctl fails, continue. One packet might get dropped, that's all.
  -------------------------------------------------------------------------*/
  ioctl_data.unframe_state = FINDING_SOP;
  if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                  HDLC_IOCTL_SET_UNFRAMER_STATE,
                  &ioctl_data,
                  &ps_errno
                ) != 0 )
  {
    LOG_MSG_ERROR_1("PPP dev %d, unable to set HDLC state to FINDING_SOP",
                    device);
  }

  /*------------------------------------------------------------------
    Since now moving out of pseudonet, call hdlc_ioctl() to switch
    HDLC mode to hardware. Continue with switching out of pseudonet,
    even if this fails (doing HW is "desired", not "required").
  ------------------------------------------------------------------*/
  instance->sw_mode_required = FALSE;
  if( hdlc_ioctl( ppp_cb_ptr->hdlc_handle,
                  HDLC_IOCTL_SET_HDLC_MODE,
                  &ioctl_data,
                  &ps_errno
                ) != 0 )
  {
    LOG_MSG_INFO2_1("PPP dev %d, unable to set HDLC mode to hardware", device);
  }

  /*-----------------------------------------------------------------
    Set the rx_pkt_hdlr_f_ptr to the internal mode handler and
    invoke it with the data to process (the new packet)
  -----------------------------------------------------------------*/
  ppp_cb_ptr->mode              = ppp_cb_ptr->pending_mode;
  ppp_cb_ptr->pending_mode      = PPP_INVALID_MODE;
  ppp_cb_ptr->rx_pkt_hdlr_f_ptr = pppi_internal_mode_hdlr;
  ppp_cb_ptr->rx_pkt_hdlr_f_ptr( device,
                                 ppp_cb_ptr->hdlc_handle,
                                 in_pkt_ptr );
} /* pppi_pseudonet_switch_mode() */


/*===========================================================================
FUNCTION PPPI_PSEUDONET_MODE_HDLR

DESCRIPTION
  This function processes incoming PPP packets for PPP_PSEUDO_NET_MODE.
  It implements the PPP pseudonet state machine.
  1. FINDING_7E: Looking for a 0x7e flag, all non-0x7e bytes are ignored
     in this state.
  2. FINDING_SOP: Looking for the first non-0x7e flag having seen a 0x7e.
     In this state, the start-of-packet is being sought.
  3. SNOOPING: The state machine examines the first 2 or 4 unframed bytes
     in the incoming packet to invoke early-protocol-detection. A transition
     to UNFRAMING or FINDING_EOP states is made depending upon if a packet
     of interest is found or nor.
  4. UNFRAMING: A single packet is unframed using hdlc_unframe
  5. FINDING_EOP: The end of the current packet is sought, to be forwarded
     to the bridge device.

  This function currently ignores the passed hdlc_inst, and uses the hdlc
  handle associated with the PPP device, as maintained in the PPP control
  block.

PARAMETERS
  device     : (IN) PPP device to pass the packet to
  hdlc inst  : (IN) instance of HDLC to which the received packet belongs to
  in_pkt_ptr : (IN) pointer to the incoming PPP packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pppi_pseudonet_mode_hdlr
(
  ppp_dev_enum_type   device,         /* PPP device pkt arrived on         */
  void              * hdlc_inst,      /* Handle to HDLC instance           */
  dsm_item_type    ** in_pkt_ptr      /* address of ptr to a dsm_item_type */
)
{
  ppp_pseudonet_type * param_ptr   = NULL; /* Ptr to static pseudonet vars */
  ppp_type           * ppp_cb_ptr  = NULL;
  ppp_type           * ppp_cb_array = NULL;
  uint8                current_byte;
  dsm_item_type      * rest_of_chain = NULL; /* ptr to remaining chain     */
  uint8              * read_ptr = NULL;
  uint8              * end_ptr = NULL;
  /* Number of bytes in current DSM item examined during SNOOPING          */
  uint16               snooped_bytes = 0;
  int                  offset = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( in_pkt_ptr == NULL || *in_pkt_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid DSM");
    return;
  }

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid dev in pseudonet mode hdlr");
    dsm_free_packet(in_pkt_ptr);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[device]);
  param_ptr = &(ppp_cb_array[device].pseudonet);

  /*-------------------------------------------------------------------------
    Navigate the DSM chain passed in, processing byte-wise for snooping etc.
  -------------------------------------------------------------------------*/
  while( *in_pkt_ptr != NULL )
  {
    read_ptr = (*in_pkt_ptr)->data_ptr;
    end_ptr  = (*in_pkt_ptr)->data_ptr + (*in_pkt_ptr)->used;

    LOG_MSG_INFO3_2("Dev %d Pseudonet in %d state", device, param_ptr->state);
    switch( param_ptr->state )
    {
      case FINDING_7E:
        for( ; *read_ptr != HDLC_FLAG && read_ptr < end_ptr ; read_ptr++ )
        {
          /* NOOP */
        }
        if( read_ptr == end_ptr )
        {
          break;
        }
        ASSERT( *read_ptr == HDLC_FLAG );

        /*-------------------------------------------------------------------
          Found 0x7E, move to FINDING_SOP
        -------------------------------------------------------------------*/
        param_ptr->state = FINDING_SOP;
        /* fall through */

      case FINDING_SOP:

        /*--------------------------------------------------------------------
          If a mode switch is pending, switch to the pending "internal" mode.
        --------------------------------------------------------------------*/
        if( ppp_cb_ptr->pending_mode != PPP_INVALID_MODE )
        {
          LOG_MSG_INFO1_2("Dev %d Pseudonet :: Switching to PPP mode 0x%x",
                          device, ppp_cb_ptr->pending_mode);
          pppi_pseudonet_switch_mode( device, ppp_cb_ptr, in_pkt_ptr );
          return;
        } /* if a mode switch is pending */

        /*-------------------------------------------------------------------
          Skip forward to the first non-7e byte (start-of-packet)
        -------------------------------------------------------------------*/
        for( ; *read_ptr == HDLC_FLAG && read_ptr < end_ptr ; read_ptr++ )
        {
          /* NOOP */
        }
        if( read_ptr == end_ptr )
        {
          break;
        }
        ASSERT( *read_ptr != HDLC_FLAG );

        /*-------------------------------------------------------------------
          Found start-of-packet, move to SNOOPING
        -------------------------------------------------------------------*/
        param_ptr->state = SNOOPING;

        /*-------------------------------------------------------------------
          Get memory to hold snooped bytes
        -------------------------------------------------------------------*/
        ASSERT( param_ptr->snoop_pkt_ptr == NULL );
        /* fall through */

      case SNOOPING:

        /*-------------------------------------------------------------------
          If the snoop_pkt_ptr is NULL, go ahead and allocate a dsm buffer
        -------------------------------------------------------------------*/
        if(NULL == param_ptr->snoop_pkt_ptr)
        {
          param_ptr->snoop_pkt_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
          if( NULL == param_ptr->snoop_pkt_ptr )
          {
            LOG_MSG_ERROR_0( "Memory allocation error" );
            dsm_free_packet( in_pkt_ptr );
            return;
          }
        }

        /*-------------------------------------------------------------------
          Unframe the first 2 or 4 bytes; is this a packet to be unframed
          completely (UNFRAMING) or forwarded to bridge dev (FINDING_EOP) ?
        -------------------------------------------------------------------*/
        while( ( param_ptr->state == SNOOPING ) && ( read_ptr < end_ptr ) )
        {
          current_byte = *read_ptr;
          if( current_byte == HDLC_FLAG )
          {
            /*---------------------------------------------------------------
              Rxed 0x7e while SNOOPING. Likely malformed packet.
              Throw it away. Move to FINDING_SOP.
            ---------------------------------------------------------------*/
            offset = read_ptr - (*in_pkt_ptr)->data_ptr;
            LOG_MSG_INFO1_2("PPP dev %d found 7E in SNOOPING loc=%d",
                            device, offset );
            if( dsm_pullup( in_pkt_ptr, NULL, offset ) != offset )
            {
              LOG_MSG_ERROR_1("Dev %d Pseudonet : error in SNOOPING, "
                              " discarding packet", device);
              dsm_free_packet( in_pkt_ptr );
              pppi_pseudonet_reset( device );
              return;
            }
            dsm_free_packet( &(param_ptr->snoop_pkt_ptr) );
            param_ptr->escape = FALSE;
            param_ptr->state = FINDING_SOP;
            continue;
          } /* if( current_byte == HDLC_FLAG ) */

          /*-----------------------------------------------------------------
            We have a valid byte. Unescape as needed, and store in snoop pkt
          -----------------------------------------------------------------*/
          if( param_ptr->escape == TRUE )
          {
            current_byte ^= HDLC_ESC_COMPL;
            param_ptr->escape = FALSE;
          }
          else if( current_byte == HDLC_ESC_ASYNC )
          {
            read_ptr++;
            current_byte = *read_ptr;
            ASSERT( read_ptr <= end_ptr );
            if( read_ptr == end_ptr )
            {
              param_ptr->escape = TRUE;
              break; /* move to next DSM */
            }
            else
            {
              current_byte ^= HDLC_ESC_COMPL;
            }
          } /* if current_byte == 0x7d */

          param_ptr->snoop_pkt_ptr->
            data_ptr[(param_ptr->snoop_pkt_ptr)->used++] = current_byte;
          snooped_bytes = read_ptr - (*in_pkt_ptr)->data_ptr;

          /*-----------------------------------------------------------------
            Figure out next state based on current state of snooped packet.
            This will be SNOOPING, FINDING_EOP or UNFRAMING
          -----------------------------------------------------------------*/
          param_ptr->state = pppi_pseudonet_next_state( param_ptr, device );
          read_ptr++;
        } /* while SNOOPING and current dsm item hasn't been exhausted */
        break; /* move to next DSM */

      case FINDING_EOP:
        if( pppi_pseudonet_finding_eop_proc( param_ptr,
                                             device,
                                             in_pkt_ptr,
                                             snooped_bytes ) == -1 )
        {
          LOG_MSG_ERROR_1("Dev %d Pseudonet : error in FINDING_EOP, "
                          " discarding packet", device);
          dsm_free_packet( in_pkt_ptr );
          pppi_pseudonet_reset( device );
          return;
        } /* error during FINDING_EOP */

        ASSERT( param_ptr->state == FINDING_SOP ||
                param_ptr->state == FINDING_EOP );
        continue;

      case UNFRAMING:
        /*-----------------------------------------------------------------
          Unframe a single packet using HDLC library. *in_pkt_ptr will be
          modified to point to what needs further processing.
        -----------------------------------------------------------------*/
        if(param_ptr->framed_pkt_ptr != NULL)
        {
          LOG_MSG_INFO3_2("Dev %d Pseudonet in %s state - Appending stored "
                          "partial packet and incoming packet",
                          device, param_ptr->state);
          dsm_append( &(param_ptr->framed_pkt_ptr), in_pkt_ptr);
          *in_pkt_ptr = param_ptr->framed_pkt_ptr;
          param_ptr->framed_pkt_ptr      = NULL;
          param_ptr->framed_pkt_tail_ptr = NULL;
        }

        if( pppi_pseudonet_hdlc_unframe( param_ptr,
                                         device,
                                         ppp_cb_ptr->hdlc_handle,
                                         in_pkt_ptr ) == -1 )
        {
          LOG_MSG_ERROR_1("Dev %d Pseudonet : error in UNFRAMING,"
                          " discarding packet", device);
          dsm_free_packet( in_pkt_ptr );
          pppi_pseudonet_reset( device );
          return;
        } /* error during UNFRAMING */

        ASSERT( param_ptr->state == FINDING_7E  ||
                param_ptr->state == FINDING_SOP ||
                param_ptr->state == UNFRAMING );
        continue;

      default:
        ASSERT(0);  /* Sanity check for param_ptr->state */
        break;

    } /* switch( param_ptr->state ) */

    if( *in_pkt_ptr != NULL)
    {
      ASSERT( read_ptr <= end_ptr );
      if( read_ptr == end_ptr ) /* exhausted current dsm item */
      {
        /*-------------------------------------------------------------------
          Finished processing current DSM. If SNOOPING right now, a decision
          has not yet been made about this packet, so add the current DSM to
          the framed packet chain. Else, forward data to the bridge device
          unless the state is UNFRAMING.
        -------------------------------------------------------------------*/
        rest_of_chain = (*in_pkt_ptr)->pkt_ptr;
        (*in_pkt_ptr)->pkt_ptr = NULL;

        if( param_ptr->state == SNOOPING ||
           ( param_ptr->state == FINDING_SOP && rest_of_chain == NULL ) ||
           ( param_ptr->state == UNFRAMING   && rest_of_chain == NULL ))
        {
          LOG_MSG_INFO3_2("Dev %d Pseudonet in %s state - APPENDING",
                          device, param_ptr->state);
          PPPI_PSEUDONET_APPEND_TO_CHAIN( param_ptr, *in_pkt_ptr );
        }
        else if( param_ptr->state != UNFRAMING )
        {
          pppi_pseudonet_forward_data( device, *in_pkt_ptr );
        }

        /*-----------------------------------------------------------------
          Move to the next DSM in the chain, read, end ptrs get set next
		  Reset the snooped_bytes
        -----------------------------------------------------------------*/
        *in_pkt_ptr = rest_of_chain;
		snooped_bytes = 0;
      } /* if( examined all bytes in current DSM item ) */
    } /* if( *in_pkt_ptr != NULL ) */
  } /* while( *in_pkt_ptr != NULL ) */
} /* pppi_pseudonet_mode_hdlr() */


#ifdef FEATURE_DATA_PS_PPP_ROHC
/*===========================================================================
FUNCTION PPPI_ROHC_ALLOC()

DESCRIPTION
  This function is called by an NCP to allocate ROHC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Allocate / reconfigure the PPP ROHC instance
===========================================================================*/
void pppi_rohc_alloc
(
  ppp_dev_enum_type     device,
  int                   ncp,
  ppp_rohc_config_type *local_rohc_config,
  ppp_rohc_config_type *remote_rohc_config
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  rohc_params_comp_channel_s_type rohc_comp_channel;
  rohc_params_decomp_channel_s_type rohc_decomp_channel;
  rohc_params_comp_flow_s_type rohc_comp_flow;
  rohc_params_decomp_flow_s_type rohc_decomp_flow;
  errno_enum_type err;
  int i;
  rohc_ioctl_e_type ioctl_type;
  rohc_ioctl_u_type ioctl_data;
  int ret = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_type *ppp_cb_ptr = &ppp_cb_array[device];

  memset( &rohc_comp_channel, 0, sizeof(rohc_comp_channel));
  memset( &rohc_decomp_channel, 0, sizeof(rohc_decomp_channel));
  memset( &rohc_comp_flow, 0, sizeof(rohc_comp_flow));
  memset( &rohc_decomp_flow, 0, sizeof(rohc_decomp_flow));

  rohc_comp_channel.max_cid        = remote_rohc_config->max_cid;
  rohc_comp_channel.use_large_cids = (rohc_comp_channel.max_cid > 15) ?
                                       TRUE : FALSE;
  rohc_comp_channel.mrru           = remote_rohc_config->mrru;
  rohc_comp_channel.profiles       = 0;
  for( i=0; i < remote_rohc_config->no_of_profiles; i++)
  {
    rohc_comp_channel.profiles |= (1<<remote_rohc_config->profiles[i]);
  }
  rohc_comp_channel.max_jitter_cd = 150;



  rohc_decomp_channel.max_cid        = local_rohc_config->max_cid;
  rohc_decomp_channel.use_large_cids = (rohc_decomp_channel.max_cid > 15) ?
                                         TRUE : FALSE;
  rohc_decomp_channel.mrru           = local_rohc_config->mrru;
  rohc_decomp_channel.profiles       = 0;
  for( i=0; i < local_rohc_config->no_of_profiles; i++)
  {
    rohc_decomp_channel.profiles |= (1<<local_rohc_config->profiles[i]);
  }
  rohc_decomp_channel.k1_out_n1 = ROHC_POLICY_1_OUT_1;
  rohc_decomp_channel.k2_out_n2 = ROHC_POLICY_1_OUT_1;

  rohc_comp_flow.ww_num_update        = 4;
  rohc_comp_flow.num_update_IR        = 3;
  rohc_comp_flow.num_nonupdates       = 3;
  rohc_comp_flow.SO_IRt               = 512;
  rohc_comp_flow.SO_FOt               = 255;
  rohc_comp_flow.timer_based_comp     = FALSE;
  rohc_comp_flow.profile_hint         = 0;
  rohc_comp_flow.f_max_time           = 5000;
  rohc_comp_flow.exp_backoff          = 2;
  rohc_comp_flow.time_stride          = 20;
  rohc_comp_flow.ts_stride_recalc     = TRUE;
  rohc_comp_flow.loss_option          = FALSE;
  rohc_comp_flow.p_sn                 = 6;
  rohc_comp_flow.const_ip_id_handling = TRUE;

  rohc_decomp_flow.preferred_mode       = 1;
  rohc_decomp_flow.optional_ack_enabled = FALSE;
  rohc_decomp_flow.rd_depth             = 0;
  rohc_decomp_flow.timer_based_decomp   = FALSE;
  rohc_decomp_flow.loss_option_thres    = 0;
  rohc_decomp_flow.p_sn                 = 6;

  /*-------------------------------------------------------------------------
    ROHC Logging is not enabled here because PPP logging is already enabled
  -------------------------------------------------------------------------*/

  if (ppp_cb_ptr->rohc_handle == NULL)
  {
    ppp_cb_ptr->rohc_handle = rohc_alloc(&rohc_comp_channel,
                                         &rohc_comp_flow,
                                         &rohc_decomp_channel,
                                         &rohc_decomp_flow,
                                         &err);
  }
  else
  {
    ioctl_type = ROHC_IOCTL_REINIT_COMP_CHANNEL;
    ioctl_data.comp_channel_params = &rohc_comp_channel;
    ret = rohc_ioctl(ppp_cb_ptr->rohc_handle,
                     ioctl_type,
                     &ioctl_data,
                     &err);
    if (ret == -1)
    {
      LOG_MSG_INFO1_2("ROHC ioctl %d failed.  ps_errno %d", ioctl_type, err);
    }
    else
    {
      ioctl_type = ROHC_IOCTL_REINIT_DECOMP_CHANNEL;
      ioctl_data.decomp_channel_params = &rohc_decomp_channel;
      ret = rohc_ioctl(ppp_cb_ptr->rohc_handle, ioctl_type,
                       &ioctl_data, &err);
      if (ret == -1)
      {
        LOG_MSG_INFO1_2("ROHC ioctl %d failed.  ps_errno %d", ioctl_type, err);
      }
    }
  }

  if (ppp_cb_ptr->rohc_handle == NULL || ret == -1)
  {
    LOG_MSG_INFO1_1("Unable to allocate ROHC instance, dev %d", device);
  }
  else
  {
    ppp_cb_ptr->rohc_ncp |= (1<<ncp);
  }

  return;
} /* pppi_rohc_alloc() */


/*===========================================================================
FUNCTION PPPI_ROHC_FREE()

DESCRIPTION
  This function is called by an NCP to free it's use of ROHC.  It checks to
  see if ROHC is in use by another NCP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Can free the PPP ROHC instance if not in use
===========================================================================*/
void pppi_rohc_free
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type ncp
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];
  LOG_MSG_INFO2_2("pppi_rohc_free(): called for device %d, ncp %d",
                  device, ncp);

  if (ppp_cb_ptr->rohc_handle == NULL)
  {
    return;
  }

  ppp_cb_ptr->rohc_ncp &= ~(1<<ncp);

  if (ppp_cb_ptr->rohc_ncp == 0)
  {
    rohc_free( ppp_cb_ptr->rohc_handle );
    ppp_cb_ptr->rohc_handle = NULL;
  }
} /* pppi_rohc_free() */
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
/*===========================================================================
FUNCTION PPPI_IPHC_ALLOC()

DESCRIPTION
  This function is called by an NCP to allocate IPHC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Allocate / reconfigure the PPP IPHC instance
===========================================================================*/
void pppi_iphc_alloc
(
  ppp_dev_enum_type     device,
  int                   ncp,
  ppp_iphc_config_type *local_config,
  ppp_iphc_config_type *remote_config
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  iphc_ioctl_e_type ioctl_type;
  iphc_ioctl_u_type ioctl_data;
  iphc_config_options_s_type comp_opt;
  iphc_config_options_s_type decomp_opt;
  int ret = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  comp_opt.tcp_space         = remote_config->tcp_space;
  comp_opt.non_tcp_space     = remote_config->non_tcp_space;
  comp_opt.f_max_period      = remote_config->f_max_period;
  comp_opt.f_max_time        = remote_config->f_max_time;
  comp_opt.max_header        = remote_config->max_header;
  comp_opt.rtp_enabled       = remote_config->rtp_enabled;
  comp_opt.expect_reordering = FALSE;

  decomp_opt.tcp_space         = local_config->tcp_space;
  decomp_opt.non_tcp_space     = local_config->non_tcp_space;
  decomp_opt.f_max_period      = local_config->f_max_period;
  decomp_opt.f_max_time        = local_config->f_max_time;
  decomp_opt.max_header        = local_config->max_header;
  decomp_opt.rtp_enabled       = local_config->rtp_enabled;
  decomp_opt.expect_reordering = FALSE;

  /*-------------------------------------------------------------------------
    IPHC Logging is not enabled here because PPP logging is already enabled
  -------------------------------------------------------------------------*/

  if (ppp_cb_ptr->iphcp == NULL)
  {
    if (ncp == IPCP)
    {
      ppp_cb_ptr->iphcp = iphc_alloc( &comp_opt, &decomp_opt, NULL, NULL);
    }
#ifdef FEATURE_DATA_PS_IPV6
    else
    {
      ppp_cb_ptr->iphcp = iphc_alloc(NULL, NULL, &comp_opt, &decomp_opt);
    }
#endif /* FEATURE_DATA_PS_IPV6 */

  }
  else
  {
    ioctl_type = (ncp == IPCP) ? IPHC_IOCTL_COMPRESSOR_V4_SET_OPTIONS :
                   IPHC_IOCTL_COMPRESSOR_V6_SET_OPTIONS;
    memscpy( &ioctl_data.options,
              sizeof(iphc_config_options_s_type), 
              &comp_opt, 
              sizeof(iphc_config_options_s_type) );
    ret = iphc_ioctl(ppp_cb_ptr->iphcp,
                     ioctl_type,
                     &ioctl_data);
    if (ret == -1)
    {
      LOG_MSG_INFO1_1("IPHC ioctl %d failed.", ioctl_type);
    }
    else
    {
      ioctl_type = (ncp == IPCP) ? IPHC_IOCTL_DECOMPRESSOR_V4_SET_OPTIONS :
                     IPHC_IOCTL_DECOMPRESSOR_V6_SET_OPTIONS;
      memscpy( &ioctl_data.options, 
                sizeof(iphc_config_options_s_type), 
                &decomp_opt, 
                sizeof(iphc_config_options_s_type) );
      ret = iphc_ioctl(ppp_cb_ptr->iphcp, ioctl_type, &ioctl_data);
      if (ret == -1)
      {
        LOG_MSG_INFO1_1("IPHC ioctl %d failed.", ioctl_type);
      }
    }
  }

  if(ppp_cb_ptr->iphcp == NULL || ret == -1)
  {
    LOG_MSG_INFO1_1("Unable to allocate IPHC instance, dev %d", device);
  }
  else
  {
    ppp_cb_ptr->iphc_ncp |= (1<<ncp);
  }

  return;
} /* pppi_iphc_alloc() */


/*===========================================================================
FUNCTION PPPI_IPHC_FREE()

DESCRIPTION
  This function is called by an NCP to free it's use of IPHC.  It checks to
  see if IPHC is in use by another NCP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Can free the PPP IPHC instance if not in use
===========================================================================*/
void pppi_iphc_free
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type ncp
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];
  LOG_MSG_INFO2_2("pppi_iphc_free(): called for device %d, ncp %d",
                  device, ncp);

  if (ppp_cb_ptr->iphcp == NULL)
  {
    return;
  }

  ppp_cb_ptr->iphc_ncp &= ~(1<<ncp);

  if (ppp_cb_ptr->iphc_ncp == 0)
  {
    iphc_free( ppp_cb_ptr->iphcp );
    ppp_cb_ptr->iphcp = NULL;
  }
} /* pppi_iphc_free() */
#endif /* FEATURE_DATA_PS_PPP_IPHC */

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION PPPI_GETCONST()

DESCRIPTION
 To get constants from the phone

PARAMETERS
 device : PPP device to get the constant from
 param  : the constant or enum to fetch

DEPENDENCIES
 None

RETURN VALUE
 Integer value of the constant, -1 in case of an error.

SIDE EFFECTS
 None

===========================================================================*/
int pppi_getconst
(
  ppp_dev_enum_type device,
  pppi_param_const_enum_type param
)
{
  ppp_fsm_type *fsm_ptr;
#ifdef FEATURE_DATA_PS_EHRPD
  uint32 value;
  ppp_init_consts_enum_type vsncp_3gpp2_enum;
#endif /* FEATURE_DATA_PS_EHRPD */
  ppp_fsm_type_enum_type ppp_fsm_enum;
  ppp_auth_type_enum_type ppp_auth_enum;
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= PPP_MAX_DEV || param >= PPPI_MAX_PARAM)
  {
    LOG_MSG_ERROR_2( "Invalid arguments: device %d, constant parameter %d",
                     device, param );
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  switch ( param )
  {
    case PPPI_LCP_FSM_STATE:
      ppp_fsm_enum = PPP_FSM_STATE;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_RX_IFACE_PTR:
      ppp_fsm_enum = PPP_FSM_RX_IFACE_PTR;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_CONFIGURE_TRIES:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_CONFIGURE_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_TERMINATE_TRIES:
      ppp_fsm_enum = PPP_FSM_TERMINATE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_NAK_TRIES:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_NAK_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_ACK_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_ACK_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_LCP_TERM_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_TERM_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_FSM_STATE:
      ppp_fsm_enum = PPP_FSM_STATE;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_RX_IFACE_PTR:
      ppp_fsm_enum = PPP_FSM_RX_IFACE_PTR;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_CONFIGURE_TRIES:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_CONFIGURE_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_TERMINATE_TRIES:
      ppp_fsm_enum = PPP_FSM_TERMINATE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_NAK_TRIES:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_NAK_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_ACK_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_ACK_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPCP_TERM_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_TERM_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

#ifdef FEATURE_DATA_PS_IPV6
    case PPPI_IPV6CP_FSM_STATE:
      ppp_fsm_enum = PPP_FSM_STATE;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_RX_IFACE_PTR:
      ppp_fsm_enum = PPP_FSM_RX_IFACE_PTR;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_CONFIGURE_TRIES:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_CONFIGURE_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_TERMINATE_TRIES:
      ppp_fsm_enum = PPP_FSM_TERMINATE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_NAK_TRIES:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_NAK_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_ACK_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_ACK_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );

    case PPPI_IPV6CP_TERM_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_TERM_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_getconst( fsm_ptr, ppp_fsm_enum );
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_EHRPD
    case PPPI_VSNCP_3GPP2_CONFIGURE_TRIES:
      vsncp_3gpp2_enum = VSNCP_3GPP2_REQ_TRY;
      vsncp_3gpp2_get_constant( vsncp_3gpp2_enum, &value );
      return (int)value;

    case PPPI_VSNCP_3GPP2_TERMINATE_TRIES:
      vsncp_3gpp2_enum = VSNCP_3GPP2_TERM_TRY;
      vsncp_3gpp2_get_constant( vsncp_3gpp2_enum, &value );
      return (int)value;

    case PPPI_VSNCP_3GPP2_ACK_TIMEOUT:
      vsncp_3gpp2_enum = VSNCP_3GPP2_VSNCP_ACK_TIMEOUT;
      vsncp_3gpp2_get_constant( vsncp_3gpp2_enum, &value );
      return (int)value;

    case PPPI_VSNCP_3GPP2_TERM_TIMEOUT:
      vsncp_3gpp2_enum = VSNCP_3GPP2_TERM_TIMEOUT;
      vsncp_3gpp2_get_constant( vsncp_3gpp2_enum, &value );
      return (int)value;

#endif /* FEATURE_DATA_PS_EHRPD */

    case PPPI_AUTH_PASSWD_ENCRYPT:
      ppp_auth_enum = PPP_AUTH_PASSWD_ENCRYPT;
      return auth_getconst( &ppp_cb_array[device], ppp_auth_enum );

    case PPPI_AUTH_TIMEOUT:
      ppp_auth_enum = PPP_AUTH_TIMEOUT;
      return auth_getconst( &ppp_cb_array[device], ppp_auth_enum );

    case PPPI_AUTH_RETRY_COUNT:
      ppp_auth_enum = PPP_AUTH_RETRY_COUNT;
      return auth_getconst( &ppp_cb_array[device], ppp_auth_enum );

    case PPPI_AUTH_RETRY_LIMIT:
      ppp_auth_enum = PPP_AUTH_RETRY_LIMIT;
      return auth_getconst( &ppp_cb_array[device], ppp_auth_enum );

   default:
      LOG_MSG_ERROR_1( "Unknown constant parameter %d", param );
      return -1;
  }
} /* pppi_getconst */
#endif /* FEATURE_STA_PPP */
#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION PPPI_SETCONST()

DESCRIPTION
 To set constants on the phone

PARAMETERS
 device : PPP device to set the constant for
 param  : the constant or enum to set
 value  : the value to set the constant to
 len    : the length of the value in bytes

DEPENDENCIES
 none

RETURN VALUE
 0 for success, -1 in case of an error.

SIDE EFFECTS
 none
===========================================================================*/
int pppi_setconst
(
  ppp_dev_enum_type device,
  pppi_param_const_enum_type param,
  void * value,
  uint32 len
)
{
  ppp_fsm_type *fsm_ptr;
  ppp_fsm_type_enum_type ppp_fsm_enum;
  ppp_auth_type_enum_type ppp_auth_enum;
#ifdef FEATURE_DATA_PS_EHRPD
  ppp_init_consts_enum_type vsncp_3gpp2_enum;  
#endif /* FEATURE_DATA_PS_EHRPD */
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( device >= PPP_MAX_DEV || param >= PPPI_MAX_PARAM ||
      value == NULL || len == 0                        )
  {
    LOG_MSG_ERROR_2( "Invalid arguments: device %d, constant parameter %d",
                     device, param );
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  switch ( param )
  {
    case PPPI_LCP_RX_IFACE_PTR:
      ppp_fsm_enum = PPP_FSM_RX_IFACE_PTR;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_CONFIGURE_TRIES:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_CONFIGURE_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_TERMINATE_TRIES:
      ppp_fsm_enum = PPP_FSM_TERMINATE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_NAK_TRIES:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_NAK_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_ACK_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_ACK_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_LCP_TERM_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_TERM_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_RX_IFACE_PTR:
      ppp_fsm_enum = PPP_FSM_RX_IFACE_PTR;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_CONFIGURE_TRIES:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_CONFIGURE_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_TERMINATE_TRIES:
      ppp_fsm_enum = PPP_FSM_TERMINATE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_NAK_TRIES:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_NAK_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_ACK_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_ACK_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPCP_TERM_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_TERM_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPCP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

#ifdef FEATURE_DATA_PS_IPV6
    case PPPI_IPV6CP_RX_IFACE_PTR:
      ppp_fsm_enum = PPP_FSM_RX_IFACE_PTR;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_CONFIGURE_TRIES:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_CONFIGURE_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_CONFIGURE_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_TERMINATE_TRIES:
      ppp_fsm_enum = PPP_FSM_TERMINATE_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_NAK_TRIES:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_NAK_TRIES_REMAINING:
      ppp_fsm_enum = PPP_FSM_NAK_TRIES_REMAINING;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_ACK_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_ACK_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );

    case PPPI_IPV6CP_TERM_TIMEOUT:
      ppp_fsm_enum = PPP_FSM_TERM_TIMEOUT;
      fsm_ptr = &(ppp_cb_array[device].fsm[IPV6CP]);
      return fsm_setconst( fsm_ptr, ppp_fsm_enum, *(uint32*)value );
#endif /* FEATURE_DATA_PS_IPV6 */

    case PPPI_AUTH_PASSWD_ENCRYPT:
      ppp_update_auth_setting(device, *(boolean*)value);
      return 0;

#ifdef FEATURE_DATA_PS_EHRPD
    case PPPI_VSNCP_3GPP2_CONFIGURE_TRIES:
      vsncp_3gpp2_enum = VSNCP_3GPP2_REQ_TRY;
      vsncp_3gpp2_set_constant( vsncp_3gpp2_enum, *(uint32*)value );
      return 0;

    case PPPI_VSNCP_3GPP2_TERMINATE_TRIES:
      vsncp_3gpp2_enum = VSNCP_3GPP2_TERM_TRY;
      vsncp_3gpp2_set_constant( vsncp_3gpp2_enum, *(uint32*)value );
      return 0;

    case PPPI_VSNCP_3GPP2_ACK_TIMEOUT:
      vsncp_3gpp2_enum = VSNCP_3GPP2_VSNCP_ACK_TIMEOUT;
      vsncp_3gpp2_set_constant( vsncp_3gpp2_enum, *(uint32*)value );
      return 0;

    case PPPI_VSNCP_3GPP2_TERM_TIMEOUT:
      vsncp_3gpp2_enum = VSNCP_3GPP2_TERM_TIMEOUT;
      vsncp_3gpp2_set_constant( vsncp_3gpp2_enum, *(uint32*)value );
      return 0;

#endif /* FEATURE_DATA_PS_EHRPD */

    case PPPI_AUTH_TIMEOUT:
      ppp_auth_enum = PPP_AUTH_TIMEOUT;
      return auth_setconst( &ppp_cb_array[device],
                           ppp_auth_enum,
                           *(uint32*)value );

    case PPPI_AUTH_RETRY_COUNT:
      ppp_auth_enum = PPP_AUTH_RETRY_COUNT;
      return auth_setconst( &ppp_cb_array[device],
                           ppp_auth_enum,
                           *(uint32*)value );

    case PPPI_AUTH_RETRY_LIMIT:
      ppp_auth_enum = PPP_AUTH_RETRY_LIMIT;
      return auth_setconst( &ppp_cb_array[device],
                           ppp_auth_enum,
                           *(uint32*)value );

    default:
      LOG_MSG_ERROR_1( "Unknown constant parameter %d", param );
      return -1;
    }
} /* pppi_setconst */
#endif /* FEATURE_STA_PPP */

#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPPI_APPLY_CACHED_OPTS()

DESCRIPTION
  None

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_apply_cached_opts
(
  ppp_dev_enum_type   device,
  ppp_dev_opts_type * config_ptr
)
{
  ppp_dev_config_type          *cached_config_ptr = NULL ;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( ( device >= PPP_MAX_DEV ) || (config_ptr == NULL ) )
  {
    DATA_ERR_FATAL("Invalid params passed");
    return -1;
  }

  /*-----------------------------------------------------------------------
    Get ppp device cache config.
  -----------------------------------------------------------------------*/
  cached_config_ptr = ppp_get_device_config_ptr(device);
  if ( NULL == cached_config_ptr )
  {
    LOG_MSG_ERROR_1("PPP device %d not initialized, failing...", device);
    return -1;
  }

  config_ptr->lcp_info.mode = cached_config_ptr->lcp_mode;
  config_ptr->hdlc_mode = cached_config_ptr->hdlc_mode;
  
  switch( cached_config_ptr->auth_role )
  {
    case PPP_AUTH_ROLE_NONE:
      /*---------------------------------------------------------------------
        Turn all auth bits off
      ---------------------------------------------------------------------*/
      config_ptr->lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
      config_ptr->lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
      config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
      config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;      
      break;

    case PPP_AUTH_ROLE_AUTHENTICATEE:
      /*---------------------------------------------------------------------
        Turn on auth bits for remote.want and remote.will. Turn others off.
      ---------------------------------------------------------------------*/
      config_ptr->lcp_info.will_mask[FSM_REMOTE] |= LCP_N_AP;
      config_ptr->lcp_info.want_mask[FSM_REMOTE] |= LCP_N_AP;
      config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
      config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;      

      config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
        cached_config_ptr->auth_proto_mask;
#ifdef FEATURE_DATA_PS_PPP_EAP
      config_ptr->lcp_info.want_vals[FSM_REMOTE].authentication = 
        PPP_EAP_PROTOCOL;
#endif /* FEATURE_DATA_PS_PPP_EAP */
      config_ptr->auths_peer = FALSE;
      break;

    case PPP_AUTH_ROLE_AUTHENTICATOR:
      /*---------------------------------------------------------------------
        Turn on auth bits for local.want and local.will. Turn others off.
      ---------------------------------------------------------------------*/
      config_ptr->lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
      config_ptr->lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP; 
      config_ptr->lcp_info.will_mask[FSM_LOCAL] |= LCP_N_AP;
      config_ptr->lcp_info.want_mask[FSM_LOCAL] |= LCP_N_AP;      

      config_ptr->lcp_info.want_vals[FSM_LOCAL].auth_proto_mask = 
        cached_config_ptr->auth_proto_mask;
      config_ptr->auths_peer = TRUE;
      break;

    default:
      /* unexpected */
      ASSERT(0);
      return -1;
  } /* switch( auth_role ) */

  memscpy( &(config_ptr->auth_info),
           sizeof(ppp_auth_info_type),
           &(cached_config_ptr->auth_info),
           sizeof(ppp_auth_info_type) );
  
  config_ptr->setup_final_phase = cached_config_ptr->setup_final_phase;  
  config_ptr->setup_timeout = cached_config_ptr->setup_timeout;

  config_ptr->tx_f_ptr = cached_config_ptr->tx_f_ptr;
  config_ptr->tx_user_data = cached_config_ptr->tx_user_data;
  config_ptr->rx_signal = cached_config_ptr->rx_signal;
  config_ptr->rx_sig_f_ptr = cached_config_ptr->rx_sig_f_ptr;
  
  config_ptr->bridge_dev = cached_config_ptr->bridge_dev;
#ifdef FEATURE_DATA_PS_ENFORCE_AUTH
  config_ptr->allow_no_auth = cached_config_ptr->allow_no_auth;
#endif /* FEATURE_DATA_PS_ENFORCE_AUTH */

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  config_ptr->dos_is_enabled = cached_config_ptr->dos_is_enabled;
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
  return 0;

} /* pppi_apply_cached_opts() */
#endif /* FEATURE_DATA_PS_EHRPD */


/*===========================================================================
FUNCTION CALL_EVENT_CALLBACK_F_TYPE()

DESCRIPTION
  Function pointer for the function handler in the FSM to handle specific
  events that have been triggered because of FSM activity.

PARAMETERS
  device:           PPP device pkt arrived on
  protocol:         Protocol assc. with the packet
  event:            Event associated with cback function
  prev_state:       Previous state of the control block event
  state:            Current state of the control block event

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Probably.
===========================================================================*/
void pppi_fsm_call_event_callback
(
  ppp_fsm_type         *fsm_ptr,          /* fsm ptr                     */
  uint16                protocol,         /* protocol of packet      */
  ppp_event_e_type      event,            /* event associated with cback */
  ppp_event_state_type  prev_state,        /* previous state of cb */
  ppp_event_state_type  state              /* current state of cb */
)
{

  ppp_protocol_e_type proto_val;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(NULL == fsm_ptr)
  {
    DATA_ERR_FATAL("pppi_fsm_call_event_callback: fsm_ptr is NULL");
    return;
  }

  if( PPP_MAX_DEV <= fsm_ptr->device || PPP_MAX_EV <= event )
  {
    LOG_MSG_ERROR_2( "Invalid args passed. device: %d, event: %d",
                     fsm_ptr->device, event );
    ASSERT(0);
    return;
  }
  /*-----------------------------------------------------------------------
    Obtain equivalent ppp_event_protocol_type value for the passed protocol
  -----------------------------------------------------------------------*/
  switch(protocol)
  {
    case PPP_LCP_PROTOCOL:
      proto_val = PPP_PROTOCOL_LCP;
      break;

    case PPP_IPCP_PROTOCOL:
      proto_val = PPP_PROTOCOL_IPCP;
      break;

    case PPP_IPV6CP_PROTOCOL:
      proto_val = PPP_PROTOCOL_IPV6CP;
      break;

    case PPP_VSNCP_PROTOCOL:
      proto_val = PPP_PROTOCOL_VSNCP;
      break;

    case PPP_PAP_PROTOCOL:
    case PPP_CHAP_PROTOCOL:
    case PPP_NO_PROTOCOL:
    default:
      proto_val = PPP_PROTOCOL_MAX;
      break;
  } /* switch(protocol) */

  if(event == PPP_RESYNC_EV)
  {
#ifdef FEATURE_DATA_PS_IPV6
    pppi_failover_on_resync( fsm_ptr->device, proto_val );
#endif /*FEATURE_DATA_PS_IPV6*/
  }

  pppi_call_event_callback( fsm_ptr, proto_val, event );

  if(event == PPP_RESYNC_EV)
  {
    /*------------------------------------------------------------------
      Generate a PS_PPP_EVENT_RESYNC event
    ------------------------------------------------------------------*/
    pppi_report_event_to_diag( EVENT_PPP,
                               fsm_ptr->device,
                               proto_val,
                               prev_state,
                               state
                             );
  }
}

/*===========================================================================
FUNCTION PS_PPPI_FRAG_TX_CMD()

DESCRIPTION
  This function transmits out the passed IP fragment, by calling
  pppi_outgoing

PARAMETERS
  device_ptr:    PPP device
  dsm_item_ptr:  Data Packet
  meta_info_ptr: Meta Info associated with the Packet

RETURN VALUE
 0 for success, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_pppi_frag_tx_cmd
(
  void                  * device_ptr,
  dsm_item_type        ** dsm_item_ptr,
  ps_tx_meta_info_type  * meta_info_ptr
)
{
  ppp_dev_enum_type dev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (device_ptr == NULL) || (dsm_item_ptr == NULL) )
  {
    LOG_MSG_ERROR_0("PPP device/dsm item is NULL");
    return -1;
  }

  dev = (ppp_dev_enum_type)((uint32)device_ptr);

  (void) pppi_outgoing(dev,
                       PPP_IP_PROTOCOL,
                       dsm_item_ptr,
                       meta_info_ptr);
  return 0;
} /* ps_pppi_frag_tx_cmd() */

/*===========================================================================
FUNCTION: PPP_REPORT_EVENT_IID

DESCRIPTION
  This reports the IPV6 IID event to the Diag.

PARAMETERS
  device: PPP device
  iid: The IPV6 iid in network byte order.

RETURN VALUE
  none
===========================================================================*/
void ppp_report_event_iid
(
  ppp_dev_enum_type device,
  uint64 iid 
)
{
  ppp_event_iid_payload_type ppp_event_iid; 
  ppp_event_iid.ppp_event_device = (uint8)device; 
  (void)snprintf( ppp_event_iid.iid,sizeof(ppp_event_iid.iid), 
                        "%x:%x:%x:%x",  
                        (uint16)(ps_ntohs(iid)),  
                        (uint16)(ps_ntohs(iid >> 16)),  
                        (uint16)(ps_ntohs(iid >> 32)) ,  
                        (uint16)(ps_ntohs(iid >> 48)) ); 
  event_report_payload( EVENT_IID, 
                        sizeof(ppp_event_iid), 
                        &ppp_event_iid );   
}

#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPPI_DEVICE_STOP()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_device_stop
(
  ppp_dev_enum_type device
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  pppi_mode_info_type  ppp_mode_info;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(device >= (uint16)PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("Invalid device %d!", device);
    ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    If the device is invalid then don't do anything
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("pppi_device_stop() for free dev %d", device);
    return;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Put device in internal mode before initiating ppp closure.
    -----------------------------------------------------------------------*/
    ppp_mode_info.device = device;
    ppp_mode_info.mode   = (uint16)PPP_INTERNAL_MODE;
    pppi_set_mode( ppp_mode_info );
  }

  fsm_close(&ppp_cb_ptr->fsm[LCP]);

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if( fsmCLOSING != ppp_cb_ptr->fsm[LCP].state )
  {
    ppp_cb_ptr->fsm[LCP].mode = PPP_CP_MODE_INVALID;
  }
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

} /* pppi_device_stop() */


/*===========================================================================
FUNCTION PPPI_ECHO_REQ_TIMER_CB()

DESCRIPTION
  This function processes the LSD Echo-Req timer

PARAMETERS
  lsd_info_ptr   Pointer to PPP LSD config (allocated from PS mem)

RETURN VALUE
  None

DEPENDENCIES
  PPP device must already have been started

SIDE EFFECTS
===========================================================================*/
void pppi_echo_req_timer_cb
(
  void * arg_ptr
)
{
  ppp_dev_enum_type  device;
  boolean            lsd_failed = TRUE;
  ppp_type         * ppp_cb_array = NULL;
  ppp_type         * ppp_cb_ptr = (ppp_type *)arg_ptr;
  dsm_item_type    * echo_req_pkt = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ppp_cb_ptr == NULL )
  {
    DATA_ERR_FATAL("Invalid PPP control block!");
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  device = (ppp_dev_enum_type)(ppp_cb_ptr - ppp_cb_array);
  
  /*-------------------------------------------------------------------------
    If tries remain, send another LCP Echo-Req, else LSD has failed, go to 
    end.
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->lsd.config.echo_req_attempts > 0 &&
      ppp_cb_ptr->lsd.config.echo_req_timeout > 0 )
  {
    do
    {
      /*---------------------------------------------------------------------
        Send LCP Echo-req and start waiting
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Dev %d, sending LCP Echo-Req", device );
      if( fsm_send( &ppp_cb_ptr->fsm[LCP], 
                    ECHO_REQ, 
                    0, 
                    echo_req_pkt ) != 0 )
      {
        /*-------------------------------------------------------------------
          Unable to send packet. LSD has failed. Go to end.
        -------------------------------------------------------------------*/
        LOG_MSG_ERROR_1("Dev %d, Unable to send LCP Echo-Req!", device);
        break;
      }
      ppp_cb_ptr->lsd.config.echo_req_attempts--;
      ps_timer_start_secs( ppp_cb_ptr->lsd.timer,
                           ppp_cb_ptr->lsd.config.echo_req_timeout );

      /*---------------------------------------------------------------------
        Echo-Req packet sent. LSD is in progress. Return here.
      ---------------------------------------------------------------------*/
      return;
    } while(0);
  }

  if( lsd_failed == TRUE )
  {
    /*-------------------------------------------------------------------------
      Declare LSD failure, free the timer and reset the control block
    -------------------------------------------------------------------------*/
    LOG_MSG_ERROR_1("Dev %d, Link Status Determination Failed!", device);
    ppp_cb_ptr->lsd.config.result_cback
    ( 
      device,
      FALSE,
      ppp_cb_ptr->lsd.config.user_data 
    );
    ps_timer_free( ppp_cb_ptr->lsd.timer );
    memset( &ppp_cb_ptr->lsd.config, 0, sizeof( ppp_lsd_config_type ) );
  }
} /* pppi_echo_req_timer_cb() */


/*===========================================================================
FUNCTION PPPI_TRIGGER_LSD()

DESCRIPTION
  This function processes the PPP Trigger Link Status Determination command

PARAMETERS
  lsd_info_ptr   Pointer to PPP LSD config (allocated from PS mem)

RETURN VALUE
  None

DEPENDENCIES
  PPP device must already have been started

SIDE EFFECTS
===========================================================================*/
void pppi_trigger_lsd
(
  ppp_trigger_lsd_info_s_type * lsd_info_ptr
)
{
  ppp_type            * ppp_cb_ptr;
  ppp_type            * ppp_cb_array = NULL;
  uint32                session_handle;
  ppp_dev_enum_type     device;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify passed in arguments.
  -------------------------------------------------------------------------*/
  if( NULL == lsd_info_ptr )
  {
    DATA_ERR_FATAL("NULL lsd_info_ptr passed");
    return;
  }

  if( PPP_MAX_DEV <= lsd_info_ptr->device )
  {
    LOG_MSG_ERROR_1( "Invalid device %d", lsd_info_ptr->device );
    ASSERT( 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize configuration pointer and PPP control block pointer
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)lsd_info_ptr->device;
  device     = lsd_info_ptr->device;

  /*-------------------------------------------------------------------------
    Verify that PPP device has been started and LCP is Open
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  session_handle = ppp_cb_ptr->session_handle;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if( ( 0 == session_handle ) || 
      ( fsmOPENED != ppp_cb_ptr->fsm[LCP].state ) )
  {
    /*-----------------------------------------------------------------------
      Declare LSD failure, free config memory and return
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_1("Dev %d Cannot trigger LSD", lsd_info_ptr->device);
    lsd_info_ptr->config.result_cback( device,
                                       FALSE,
                                       lsd_info_ptr->config.user_data );
    PS_MEM_FREE( lsd_info_ptr );
    return;
  }
  
  /*-------------------------------------------------------------------------
    Verify LSD timer can be allocated
  -------------------------------------------------------------------------*/
  ppp_cb_ptr->lsd.timer = ps_timer_alloc( pppi_echo_req_timer_cb, 
                                          ppp_cb_array + (uint8)device );
  if( ppp_cb_ptr->lsd.timer == PS_TIMER_INVALID_HANDLE )
  {
    /*-----------------------------------------------------------------------
      Declare LSD failure, free config memory and return
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_1("Dev %d, unable to allocate LSD timer!", device);
    lsd_info_ptr->config.result_cback( device,
                                       FALSE,
                                       lsd_info_ptr->config.user_data );
    PS_MEM_FREE( lsd_info_ptr );    
    return;
  }
  
  /*-------------------------------------------------------------------------
    Things look good - copy LSD configuration to PPP device control block
  -------------------------------------------------------------------------*/
  memscpy( &ppp_cb_ptr->lsd.config,
           sizeof( ppp_lsd_config_type ),
           &lsd_info_ptr->config,
           sizeof( ppp_lsd_config_type ) );

  /*-------------------------------------------------------------------------
    Send the first LCP Echo-Req to start Link Status Determination
  -------------------------------------------------------------------------*/
  pppi_echo_req_timer_cb( ppp_cb_ptr );
  
  /*-------------------------------------------------------------------------
    Free configuration memory
  -------------------------------------------------------------------------*/
  PS_MEM_FREE( lsd_info_ptr );
  return;
} /* pppi_trigger_lsd() */
#endif /* FEATURE_DATA_PS_EHRPD */
