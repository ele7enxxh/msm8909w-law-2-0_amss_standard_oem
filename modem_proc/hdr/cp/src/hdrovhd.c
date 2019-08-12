/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

          O V E R H E A D  M E S S A G E S   P R O T O C O L

GENERAL DESCRIPTION
   The Overhead Messages Protocol monitors & processes the Control Channel
   QuickConfig & SectorParameters messages. The Protocol also monitors and
   processes AccessParameters messages which are re-routed to it by the
   Access Channel MAC.

EXTERNALIZED FUNCTIONS (Global)

  hdrovhd_msg_cb()
    Callback registered with HMP to deliver messages

  hdrovhd_ind_cb()
    Callback registered for delivery of indications

  hdrovhd_activate()
    Command function to activate OVHD

  hdrovhd_deactivate()
    Command function to deactivate OVHD

  hdrovhd_get_acc_sig()
    Returns the value of the AccessSignature.

  hdrovhd_get_sp_msg()
    This function is used to access the current SectorParameters message.

  hdrovhd_get_ap_msg()
    This function is used to access the current AccessParameters message.

  hdrovhd_info_is_updated()
    This function is used to query if the Overhead Info has been updated.

  hdrovhd_get_link_id()
    This function is used to retrieve the link info of the current QC message.

  hdrovhd_get_sector_is_mc_capable()
    This function is used to query whether the current sector is MC capable.

EXTERNALIZED FUNCTIONS (Regional)
  hdrovhd_powerup_init()
    Function called to initialize OVHD on powerup.

  hdrovhd_init()
    Function called by hdrmc task to initialize OVHD upon entry into HDR.

  hdrovhd_process_cmd()
    Function called by hdrmc task to process a command for OVHD

  hdrovhd_process_ind()
    Function called by hdrmc task to process an indication for OVHD

  hdrovhd_process_msg()
    Function called by hdrmc task to process a message for OVHD
              
  hdrovhd_process_timer()
    Function called by hdrmc task to process a timer for OVHD

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.  The procotol must be activated before Control Channel
  Overhead messages can be processed.

Copyright (c) 2000 - 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrovhd.c_v   1.84   12 Mar 2003 10:29:08   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrovhd.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/05/16   sjj     FR 26262 : LTE-U band support
06/21/16   vko     FR 33944: Fix to Race condition between CC pkts ignore and ORNL msg
05/09/16   vko     Reduce excessive F3 logging
05/09/16   vko     Fixed redefining avoided earfcn timer
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
07/24/15   vko     Skip earfcn for D2L reselection, if reselection failed with forbidden TA
                   reason
04/13/15   vko     Recover from invalid protocol state, during initialization
03/25/15   sjj     Mainline the feature FEATURE_LTE_RESEL_SPLMN_SUPPORT
02/06/15   sjj     Return E_NOT_SUPPORTED when ornl message doesn't include 
                   EUTRAN RATType.
12/10/14   vko     Check for subnet mask NULL, when only sector id info is needed
12/18/14   sjj     Cleaned up the usage of the feature "FEATURE_GSTK_EHRPD_LOC_INFO".
11/30/14   vko     Clear sys avoid timer, when inactive
11/12/14   vko     Do not inform ehrpd personality if session is closing
11/08/14   vko     DO CP to double check if D2L Reselection CH is supported before sending reselect
10/12/14   vko     Reset avoided channel list before retrieveing the list from SD
09/10/14   vko     SIM Toolkit eHRPD location information
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
07/09/14   cnx     Support NV refresh without reset.
04/07/14   vko     Fixed klocwork issues
02/26/14   vko     FR 17879: Extended EARFCN
05/05/14   cnx     Update NSET and IDLE channel change in one command.
08/04/14   vko     Ignore processing SP msg, when ALMP pending state is inactive
01/15/14   vko     Fixed compiler warnings
12/16/13   vko     Ignore AHO request, when HDRIDLE is in SLEEP state
03/14/14   cnx     Notify SPLMN to SD whenever it changes.
11/14/13   cnx     Ignore OVHD rx'ed on old channel until TCAssign is complete.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/31/13   ukl     Keep NSET updated for system avoidance.
09/06/13   cnx     Do not start AP supervision timer if Access Attempt is started.
08/27/13   cnx     Remove unsupported channel in hash before session negotiation.
08/01/13   smd     Used new secapi to get random number.
08/01/13   ukl     Process ORNL If OtherRATAvailable in the latest QC msg is 1.
07/18/13   vko     Inform pre-reg allowed FALSE to DS, when HDR pre-reg info is reset 
08/07/13   cnx     Do not report USABLE status when AT is in traffic.
07/12/12   cnx     Force D2L based on SD's decision.
05/24/13   ven     When shortterm cache is valid force sleep flags to TRUE so 
                   that AT can immediately go to sleep.
05/06/13   mbs     Fixed compiler errors for BOLT build
03/31/13   cnx     Support LTE deprioritization.
03/20/13   wsh     Moved NLB code out of SVDO feature and fixed SP parsing
03/13/13   ukl     Avoid current system if band or channel to hash to is invalid
02/11/13   vko     Fixed compiler warnings.
14/02/13   vko     Fixed AT not going to sleep, even if the longterm cache is valid when QP is recieved.
02/15/13   vko     Fix klockwork warnings.
02/08/13   ukl     Fix the log buffer size when BCMCS is disabled.
02/04/13   cnx     Replace PriorSession with UpdateUATI in some network.
02/04/13   cnx     Don't reset tunnel info if AT comes back to the same cell
                   after OOS. 
11/22/12   vko     HDR CP should not process ORAT when HDR is in traffic state
01/14/13   ukl     Fixed the issue of not waking up for ONRL by default
12/14/12   ukl     Fix Klocwork issues
10/10/12   cnx     Report session status after session restore or personality change.
10/05/12   ukl     Resolved compilation errors after removing FEATURE_BCMCS.
10/09/12   cnx     Added Qchat support
10/04/12   cnx     Initialize local variable num_avoided_channels.
08/14/12   wsh     Reparse LoadInfo cache when ASET is updated
08/13/12   cnx     Use hdrstream_stream_is_assigned() to check session status.
08/08/12   cnx     Support getting CGI from OVHD cache.
08/06/12   cnx     Make OVHD not updated in processing CGI command.
07/16/12   cnx     Give HDRIND_OVHD_PREREG_INFO_UPDATED when AT leaves tunnel
07/11/12   cnx     Reset tunnel information after entering HDR network.
06/28/12   cnx     Ignore IDLE handoff indication when OVHD is in tunnel.
05/25/12   ukl     Added Support for ThreshXQ when perEARFCNParmsIncluded is set to 0.
05/17/12   sju     Fixed issue of unpacking ORNL when S_PLMN is not included. 
05/03/12   ukl     Added processing for new parameters in ornl message. 
05/02/12   rmg     Switched the order of propagating ovhd updated indication 
                   and silence parameter update in hdrovhd_give_ind().
04/30/12   cnx     Unpack QC properly when RPCCount127To64Included is 0.
04/27/12   cnx     Avoid Reselection from eHRPD to LTE forbidden PLMNs.
04/26/12   cnx     Unpack PLMN ID in ORNL msg in BCD order.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
03/23/12   cnx     Support getting Pilot strength for CGI response.
03/23/12   cnx     Reverted change on 03/21/12 because it has side affect. 
03/21/12   cnx     Don't update OVHD cache if can't save SP to cache.
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/14/12   wsh     Reset AccesscHashingChannelMask if not included in SP 
03/08/12   wsh     Merged NLB support
02/29/12   cnx     Fixed the issue of not getting CGI when OVHD cache is hit.
02/28/12   cnx     Don't wake up for ORNL msg if D2L reselection is disabled.
02/02/12   cnx     Fixed incorrect timestamp when calculating the out-of-date 
                   RUTC entries.
01/06/12   cnx     Ignore preReg change in the same cell.
12/05/11   cnx     Support hdrcp_config_info.conf.
11/28/11   cnx     Support HDRIND_OVHD_COLOR_CODE_CHANGED.
11/03/11   cnx     Support getting subnet in the tunnel mode.
10/28/11   pba     Optimized handoff phase 2 support
10/18/11   cnx     Treat as no secondary ColorCode list if secondary 
                   preregZoneId list is not included in SIB8.
10/10/11   cnx     Support HDRMC_LTE_RRC_SERVICE_IND.
10/03/11   cnx     Don't send preReg info update indication in 
                   HDRSAP_TUNNEL_MODE_NO_SRV.
09/24/11   cnx     Session maintaince for Optimized Handoff.
08/04/11   cnx     Supported  pre-regisration info from LTE.
07/27/11   cnx     Merged Optimized Handoff changes.
07/18/11   cnx     Merged from 11.01.00.
07/07/11   wsh     Fixed not clearing dup. cache entry in corner cases
05/31/11   cnx     Added hdrovhd_get_subnet_mask();
                   Added hdrovhd_get_sp_msg_country_code();
                   Added hdrovhd_get_rutc_list_count().
05/18/11   wsh     Added network load balancing support
04/29/11   pba     EV-DO Rev C overhead caching support.
04/15/11   cnx     Removed hdrrup_band_is_supported() if RF_HAS_RFM_BAND_CHAN_V2_API defined.  
02/16/11   pba     Added check to make sure LTE is available before processing ORNL.
02/01/11   pba     Do not process idle handoff indication in inactive, acc ho 
                   and freq change states.
12/17/10   sju     Check link info before using AP buffer.
11/18/10   sju     Reported SYS_ACTIVE_PROT_NONE to SD when there is no Session.
11/10/10   cnx     Added sanity check on SubnetMask when processing SectorParameter msg.
09/16/10   cnx     Supported eHRPD indication.
09/02/10   pba     Added support for eHRPD to LTE reselection phase 1
06/21/10   cnx     Fixed compiler warnings.
06/10/10   pba     IRAT Measurement feature.
04/14/10   pba     Updated Ovhd Cache Packet log summary with color code info.
03/18/10   sju     Invalidated AP state when connection is ended.
03/08/10   sju     Always wait for AP if cache a new SP is needed.
03/04/10   wsh     Fixed double LMAC CMD queued crash
03/02/10   pba     Added support for LTE to HDR reselection
01/21/10   sju     Fixed using wrong index and qc valid flag to search cache.
11/12/09   sju     Added support for SVDO.
11/09/09   pba     Enhanced overhead cache.
09/30/09   sju     Fixed KW warnings.
09/04/09   wsh     Added support to report RelB to SD.
09/09/09   wsh     CMI: to support changes in clk and ts APIs
08/28/09   wsh     CMI: Removing conditional defines from API headers
06/01/09   sju     Notify SD of LocalTimeOffset and LeapSeconds 
04/10/09   pba     If the current cache ptr is same as the available node ptr 
                   clear the overhead cache and stop processing the SP message.
3/20/09    sju     Fixed pilot gid value if it is not included in revB SPM
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI--
02/27/09   wsh     Added run-time check for KDDI feature flags
02/24/09   wsh     CMI: updated included headers due to header split
02/18/09   wsh     Updated file to include private header hdrmc_v.h
02/10/09   sju     Added support for intra scc sleep.
02/03/09   pba     Fixed condition in validating SCC in SP message.
01/30/09   wsh     Merged BCMCS 2.0 on to mainbranch
01/28/09   pba     Log Overhead Cache when long timer cache timer expires.
01/23/09   wsh     Updated OvhdCacheLog to include 128b SectID & QPCH fields
12/23/08   pba     Support for Quick Idle Protocol
12/10/08   smd     Replaced assert.h with amssassert.h
11/21/08   pba     Added events for Overhead Cache Enhancement feature.
11/14/08   sju     Fixed NULL pointer reference in RouteUpdateTriggerCode 
11/12/08   wsh     Fixed NULL pointer reference in OVHD log packet
10/10/08   wsh     Fixed SectorID24 error in overhead log packet 
09/30/08   wsh     Not checking if channel is within band while hashing
09/18/08   smd     Added fix for off-target build.
09/17/08   pba     Added overhead cache enhancement.
07/08/08   wsh     Log ovhd cache when it is updated.
06/24/08   wsh     Always populate hash_channel with a valid value
07/03/08   pba     Do not ask SRCH/LMAC for link info when AT is attempting to 
                   reacquire.
05/28/08   sju     Notify SD of new overhead info whenever channel changed
05/22/08   pba     Klockwork fixes.
05/09/08   sju     Added support to remove avoided channels from NSET
05/08/08   sju     Fixed an issue where passing incorrect nset PNs to srch 
03/03/08   wsh     DCM optimization: not closing session unnecessarily when
                   moving across subnet
04/18/08   mpa     Cleaned up mode from hdrsrch_nset_pilot_type struct
04/15/08   mpa     Added support for relB pilot gid/nset update + parsing.
12/14/07   mpa     Added stub for hdrovhd_get_sector_is_mc_capable().
12/12/07   sju     Renamed SystemType enum based on sys.h
11/20/07   wsh     KDDI passport mode phase 1
11/08/07   sju     Added support to allow AHO before the first access attempt
                   when there is no DO session
09/11/07   wsh     Removed lint high warnings
08/15/07   sju     Temporary change for revB nset update
07/13/07   sju     Changed channel record type to hdr channel record type 
06/19/07   yll     Fixed a problem where reselection does not start after IHO 
                   to Less Pref Sys with same subnetmask
06/12/07   kss     Include support for max PN delay field in AP msg.
06/04/07   yll     Added support for AT Operational Mode
05/16/07   mpa     Mainlined FEATURE_HDR_LRCH
03/27/07   wsh     Added support for new prior session handling
03/06/07   wsh     Removal of RVCT compiler warnings
01/08/07   yll     Added support for CSNA
10/30/06   mpa     Notify SD of active protocol mode (rel0 or relA)
11/03/06   wsh     Cleaning up timer processing function in ovhd protocol
10/23/06   mpa     Make sure that ovhd is updated before starting BCMCS.
10/18/06   pba     Defined maximum RUTC list size in hdrhai.h 
10/10/06   pba     Fixed an issue where deleting an entry from RUTC list was 
                   causing resets.
10/10/06   hal     Added log packet support for RouteUpdateTriggerCode list
                   Added F3 messages to expedite AST testing of RUTC list
09/28/06   hal     Added restrictions to prevent AHO and CHAN_CHANGE from colliding.
09/26/06   pba     Fixed an issue where after subnet handoff if the SecColorCode
                   is not included, AT was not sending UATIRequest.
09/15/06   pba     Fixed memcpy in hdrovhd_get_secondary_color_codes().
09/05/06   mpa     Fixed bc hash override when bc ovhd is in not_parsed state.
08/23/06   hal     Added support to reset the current access attempt during 
                   personality switches.
08/15/06   pba     Initialize fields in extended SP message to default values 
                   before unpacking.
07/31/06   pba     Added SecondaryColorCode Support
07/24/06   hal     Added channel hashing routine to hdrovhd_process_cache_entry()
07/18/06   dna     Support for RouteUpdateTriggerCode, to send RUP msg and 
                   also retry following max access probes, if necessary.
07/18/06   etv     Added hdrscmovhd module to support OMP config attribs.
07/06/06   mpa     Suspend QC/SP supervision timers if IDLE is suspending. 
06/26/06   dna     Still hash ext. chans if channel list is empty.
06/15/06   hal     Mark all channels as hashable if default IDLE subtype is negotiated
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
05/31/06   hal     Changed hdrovhd_get_sp_msg_location() to use a less stringent
                   check for OVHD validity
                   Changed parameter to hdrrup_band_is_supported()
05/31/06   dna     Don't clear ovhd cache on personality switch
05/19/06   hal     Ignore extended SP channels if default IDLE state is negotiated
05/15/06   mpa     Added support for LRCH (less restrictive channel hashing).
04/27/06   mpa     Initialize cache entry before unpacking message.
04/19/06   mpa     Ignore bc msg if bc link does not match curr cache entry link
04/17/06   pba     Do not enter wait for link state if TC is already assigned.
04/11/06   mpa     Ignore bc hash override if bc msg in cache not processed yet
03/29/06   mpa     Clear bc list when ovhd inactive but don't process reparse
03/15/06   dna     Support for SD multi-channel avoid feature.
02/28/06   mpa     Clear the bc supervision timer on handback to previous sector
02/27/06   mpa     Ignore BOM if SP signature doesn't match that of SP msg.
02/23/06   hal     Added accessor functions hdrovhd_get_sp_msg_location() and
                   hdrovhd_get_sp_msg_nset_info()
                   Added helper function hdrovhd_sp_msg_is_readable()
02/13/06   mpa     Added parameter to hdrbccp_disable_all_monitored_flows().
01/12/06   etv     Fixed bug with unpacking QC msg and minor cleanup
01/06/06   hal     Minor updates to F3 message output of hashable channels
                   Changed hdrovhd_chan_is_in_cl() to search through combined 
                   channel list
                   Renamed hdrovhd_extract_extended_info_from_sp_msg() to
                   hdrovhd_unpack_sp_msg_ext_info()
                   Moved invalid neighbor pruning from hdrovhd_unpack_sp_msg() to
                   hdrrup_process_ovhd_info_updated_ind()
12/16/05   hal     Added F3 messages to expedite AST testing of AccessHashingChannelMask
12/15/05   mpa     Handle channel changes in sleep state.
12/13/05   pba     Added support for handle TCA + ConnectionClose bundling
12/07/05   mpa     Pass in is_first_after_acq in hdrbccp_process_bc_ovhd_updated
12/01/05   dna     Ensure BC SectorSignature matches SP message
                   Re-design how hdrovhd_bc_info_updated() works
                   Implement bc_supervision_timer
                   Tune back out of BC0 SC2 after BCMCS ends if necessary
12/01/05   dna     Allow SD to return ACQ_GW or any other new reasons
11/21/05   hal     Added support for the AccessHashingChannelMask attribute
11/13/05   mpa     Added HDRTRACE_OVHD_BCMCS_CHAN_CHANGE
11/10/05   mpa     Ignore a BCOvhd message with MultiplexCnt=3.
10/28/05   dna     Add pilot pn as parameter to hdrovhd_bc_info_updated.
10/22/05   dna     Don't call hdrovhd_bc_info_updated() if we changed channels
10/06/05   dna     Inform SD if PSIST value makes HDR unusable
10/06/05   dna     Parse new Release A fields in AccessParameters message
10/05/05   mpa     Fixed tune back to SP hash when reference flow becomes NULL
10/04/05   dna     Fixed SP update for BCMCS.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/30/05   mpa/dna Replaced HDRIND_OVHD_BC_UPDATED with CMD to conserve 
                   processing order between bcmac and bccp.
                   Fixed hdrovhd_process_channel_change().
09/27/05   dna     Handle race condition where CMAC supervision timer expires
                   at the same time as QC and SP timers while connected.
09/19/05   mpa     Added bc multifrequency support
09/12/05   hal     Added use of hdrrup_band_is_supported() during SP message 
                   unpack (SupportedCDMAChannels attrib)
09/06/05   mpa     Added events for BC ovhd msg and supervision timer.
08/28/05   dna     (mpa for) Added BCOvhd message handling and caching.
07/11/05   dna     Keep entire channel list in cache.  Report cached sectors.
07/09/05   dna     Add support for Rel. A QuickConfig message fields
07/04/05   dna     Change hdrovhd_get_subnet to not get subnet from SP message.
06/22/05   pba     Added hdrovhd_get_last_capsule_length_max()
05/24/05   pba     Look at RUP state instead of RMAC to find if RTC is assigned
04/04/05   pba     Removed Lint errors
01/26/05   hal     Converted all HDROVHD timers to use callbacks.
                   Added timers to avoid if invalid overhead is being sent.
12/20/04   pba     Fixed compiler warnings.
11/29/04   grl     Added support for connected state stat logging.
11/29/04   pba     Initialize available_node_ptr to NULL in hdrovhd_process_ap_msg
11/18/04   dna     Fixed merge error. AP info not updated in process_sp_msg().
11/08/04   pba     Check for invalid hash_channel band class and remove neighbors
                   with invalid band class in SP msg. Check capsule_length_max
                   is not zero in AP msg.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
11/01/04   mpa     Added HDRIND_BCMCS_FREQ_CHANGE_INITIATED.
10/20/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
10/20/04   dna     Added hdrovhd_return_to_idle() to allow overhead to be valid
                   immediately if a current cache entry is available.
07/22/04   mpa     Temporary multifrequency support for broadcast.
04/13/04   mpa     Ignore messages decoded in traffic that would be processed
                   in idle.
02/27/04   mpa     Added hdrovhd_get_qc_link().
09/26/03   mpa     AP marked as not updated when rxing QC msg and ASig differs.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDR PROT SSID
06/06/03   mpa     Flag giving the first Ovhd.Updated ind after acq instead of
                   flagging receiving the first ovhd messages after acq
03/12/03   vas     Do no assert, but rather ignore the SP msg if invalid band/
                   system type received.
02/14/03   mpa     Only give AP updated indication at the end of processing
12/10/02   dna     Added support for HDRIDLES_SUSPEND_STATE
12/06/02   vas     Give ovhd updated indication even when sys det func not
                   called (sleep optimization).
11/21/02   mpa     (for dna) Sleep optimization in process_cache_update()
09/16/02   mpa     Added fix for channel hashing problem while connected
09/12/02   mpa     Process unpacked qc msg if ovhd info is not updated.
                   This makes sure that we look for SP/AP msgs in the
                   cache if we do an AHO but stay on the same system.
09/04/02   mpa     Cache AP messages to speed up AHO.
06/24/02   mpa     Clear ap sup. timer before enter sleep (process_sleep_cmd)
06/07/02   dna     Put values in DB to enable correct time-of-day display
06/05/02   dna     Prevent Access Handoff during initial UATIRequest
06/03/02   mpa     Mainlined FEATURE_HDR_7_4_DDR.
04/16/02   dna     Don't enter sleep for 426ms sleep (if OVHD not current)
03/07/02   dna     Allow for case where RPCCount is 0 in quick config
01/28/02   dna     Send sector ID, not subnet to SD (don't mask the lower bits)
01/12/02   vas     Changed to process AHO_request while in WAIT_FOR_LINK state
12/21/01   dna     Return to proc. all msgs. state on con. setup and don't start
                   AHO when RMAC is active, to avoid invalid ovhd during con.
11/20/01   dna     Sign extend probe_init_adjust from AP msg.
10/05/01   vas     Fixed to store default WindowSize in SP msg.
10/01/01   vas     Fix to store Neighborlist PNs received in SP msg correctly
09/27/01   kss     Left justify forward traffic valid field in QC.
09/09/01   vas     DDR 62. Removed DRCLock from QC msg.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/21/01   vas     Updated sector info logging. Changed hash function to use
                   session seed instead of transmit ati. Changes as per DDRs:
                   Added SP msg silence params processing. Added Country Code
                   to SP & PowerStep to AP msg. Removed FEATURE_IS856_NO_DDR
08/18/01   sh      Added protocol logging support
06/20/01   vas     On QC redirection, corrected event reported to Sys Det from
                   SD_SS_REDIR_HDR_DIRECTED to SD_SS_REDIR_HDR_QC.
06/11/01   vas     Reset AP & SP parameters state when ovhd activated.
05/09/01   vas     Re-naming for consistency. Code cleanup. Some re-organizing.
04/27/01   vas     Shortened all MSG_MEDs to correct display problem on QXDM
04/19/01   vas     DDR Changes. Added capsule_length_max to AP msg. Restart
                   SP supervision timer if QC is rcvd whose SectorSig matches
                   the stored value for SectorSig
04/15/01   vas     Corrected event reporting when AP msg ignored. SP & AP msg
                   reception only traced if these are not already stored. Added
                   a trace for when the ovhd_info_updated indication is given
03/30/01   vas     Added hdrovhd_get_sector_id()
03/28/01   vas     Added hdrovhd_get_subnet()
03/22/01   vas     Added ASSERT to verify a valid cached msg does not hash to a
                   diff channel than it was received on.
03/21/01   dna     Fixes for QC and SP message unpacking.
03/19/01   dna     Added support for IS-856 compliant session layer
03/19/01   vas     Removed using band to enum conversion function.
03/19/01   vas     Changed to use hash in ran.c instead of hdrutil_hash
03/08/01   vas     Not changing state autonomously to inactive after giving
                   change HDR / Exit HDR indication
03/01/01   vas     Corrected state check in hdrovhd_conn_end_reset_info_state
02/27/01   vas     Restructured QC & SP msg processing to give any indications
                   at the end of processing. Added flag to the ovhd updated ind
                   to indicate if it is the first one after network acquisition
02/16/01   vas     Code cleanup. Added processing of session indications.
                   Renamed functions. Processing of conn fail & close ind
                   changed to not return an error in case OVHD is in inactive
                   state. Correction in hdrovhd_process_channel_change() to
                   tune to the right channel.
02/14/01   vas     Removed compiler warnings when compiled for X4
02/09/01   dna     Ignore AHO complete indication if not in AHO state.
01/26/01   dna     Prevent IGNORED_AP_MSG event from being logged incorrectly.
01/17/01   vas     Changed so that on receiving this indication during sleep
                   OVHD stores the expected link & transitions to WaitForLink"
           instead of "ProcessAllMsg" state when wakeup command is received.
03/27/00   vas     Created module.

=============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "memory.h"
#include "amssassert.h"
#include "hdrdebug.h"
#include "err.h"
#include "nv.h"
#include "qw.h"
#include "bit.h"
#include "time_svc.h"
#include "rex.h"
#include "dsm.h"
#include "db.h"
#include "fs_lib.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "db_v.h"
#endif

#include "sd.h"
#include "sd_v.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "ran.h"
#include "hdrcp_msg.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"
#include "hdrmci.h"
#include "hdrrup.h"
#include "hdrfmac.h"
#include "hdrutil.h"
#include "hdrbit.h"
#include "hdrdsm.h"
#include "hdramp.h"
#include "hdrsmp.h"
#include "hdrovhd.h"
#include "hdrovhdi.h"
#include "hdrovhds.h"
#ifdef FEATURE_EHRPD
#include "hdrmrlpc.h"
#endif
#include "hdrlmac.h"
#include "hdrrmac.h"
#include "hdramac.h"
#include "hdraddr.h"
#include "hdraddr_v.h"


#include "hdralmps.h"  /* hdralmps_get_state() */


#include "hdrtrace.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif /* FEATURE_HDRMC_DEBUG */

#include "hdrplog.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbccpi.h"
//DA hdrbccpc.h is in the library, so we can't include it here.
// #include "hdrbccpc.h"
extern uint8 hdrbccpc_get_curr_bc_ovhd_period(void);
#include "hdrbcovhd.h"
#include "hdrbcovhdi.h"
#include "hdrbcmac.h"
#include "hdrts.h"
#include "time_svc.h"
#include "hdrbccp.h"
#endif /* FEATURE_HDR_BCMCS */

#include "hdridlec.h" /* Included for the AccessChannelHashingMask attrib */

#include "hdrscmovhd.h"

#ifdef FEATURE_QCHAT
#include "data1x_qchatupk.h"
#include "data1x_qchatupk_opts.h"
#endif /* FEATURE_QCHAT */
#include "hdrsrchtypes.h"
#ifdef FEATURE_HDR_TO_LTE
#include "hdrsrchcmd.h"
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_HDR_TO_LTE*/

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/*---------------------------------------------------------------------------
                            OVHD MESSAGES

  The overhead protocol can receive the following messages.
---------------------------------------------------------------------------*/

/* Message ID values */

#define HDROVHD_QC_MSG_ID   0x00 /* QuickConfig Msg ID */
#define HDROVHD_SP_MSG_ID   0x01 /* SectorParameters Msg ID */
#define HDROVHD_AP_MSG_ID   0x01 /* AccessParameters Msg ID */
#ifdef FEATURE_LTE_TO_HDR_OH
#define HDROVHD_HSP_MSG_ID  0x02 /* HRPDSilenceParameters Msg ID */
#endif /* FEATURE_LTE_TO_HDR_OH */
#define HDROVHD_BC_MSG_ID   0x03 /* BroadcastOverhead Msg ID */
#ifdef FEATURE_HDR_TO_LTE
#define HDROVHD_ORNL_MSG_ID 0x03 /* OtherRATNeighborList Msg ID */
#endif /* FEATURE_HDR_TO_LTE*/

/*---------------------------------------------------------------------------
                              OVHD TIMERS

  The QuickConfig, SectorParameters & AccessParameters supervision timers are
  set for 12  Control Channel cycles.
---------------------------------------------------------------------------*/

#define HDROVHD_QC_TIMER_MSECS 5120 /* 12 Ctrl Chnl cycles */
#define HDROVHD_SP_TIMER_MSECS 5120 /* 12 Ctrl Chnl cycles */
#define HDROVHD_AP_TIMER_MSECS 5120 /* 12 Ctrl Chnl cycles */

#define HDROVHD_INSANITY_TIMER_MSECS 60000 /* 60s for insanity timer check */

#define HDROVHD_AP_UPDATE_WAIT_TIMER_MSECS 3000

#ifdef FEATURE_HDR_BCMCS
#define HDROVHD_SCC_IN_MS ( (HDRTS_SLOTS_PER_SCC * 5) / 3)
  /* SCC in ms = 426.6667ms */

#ifdef FEATURE_HDR_BCMCS_2p0 
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
#endif /* FEATURE_HDR_BCMCS */

/* LongTermOverheadCachePeriod and ShortTermOverheadCachePeriod - 
   implemantation specific default */
#define HDROVHD_QP_PREF_DEFAULT_LONGTERM_OVHD_CACHE_PERIOD   0x02BF
  /* 1 hour in units of 5.12 secs */
#define HDROVHD_QP_PREF_DEFAULT_SHORTTERM_OVHD_CACHE_PERIOD  0x000C
  /* 1 minute in units of 5.12 secs */
#define HDROVHD_QC_PREF_DEFAULT_LONGTERM_OVHD_CACHE_PERIOD   0x02BF
  /* 1 hour in units of 5.12 secs */
#define HDROVHD_QC_PREF_DEFAULT_SHORTTERM_OVHD_CACHE_PERIOD  0x00B0
  /* 15 minutes in units of 5.12 secs */

#ifdef FEATURE_LTE_TO_HDR_OH 
#define HDROVHD_MCC_LIST_MAX 10
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
#define SYS_AVOID_TIMER_MARGIN  500   /* Margin over the system avoidance timer */
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

typedef struct
{
  uint16 qp_lt_cache_period;
  uint16 qp_st_cache_period;

  uint16 qc_lt_cache_period;
  uint16 qc_st_cache_period;

} hdrovhd_pref_default_cache_period_type;

/* Address of the TCB for the task in which this protocol runs */
#define HDROVHD_TASK_PTR hdrmc_tcb_ptr

/* Max value of RouteUpdateTriggerCodeListSize attribute plus 1 */
#define HDROVHD_MAX_RUTC_ENTRIES HDRHAI_RUTC_LIST_SIZE_MAX + 1
#define HDROVHD_RUTC_UNSPECIFIED 0xFFFF

#define HDROVHD_CACHE_PERIOD_IN_MS 5.12 * 1000

#define HDROVHD_SCC_UNSPECIFIED 0xFF

#ifdef FEATURE_HDR_REVC
/* based on # of ASP HDRSRCH supports, 3 carrier * 6 pn each */
#define HDROVHD_NLB_CACHE_SIZE               18

/* Time NLB SP cache is considered valid, 
   1 min based on discussion with system */
#define HDROVHD_NLB_CACHE_VALID_DURATION_MS  60000
#endif /* FEATURE_HDR_REVC */

#define HDROVHD_MAX_FPLMN_LIST_SIZE HDRCP_MAX_FPLMN_LIST_SIZE

#define HDROVHD_MAX_EARFCN_AVOID 20
/* Maximum number of EARFCN that can be avoided at any point of time */
/* Control Channel messages state */

typedef enum
{
  HDROVHD_INFO_NOT_UPDATED,        /* Overhead info has not been updated  */
  HDROVHD_INFO_UPDATED             /* Overhead info has been updated */
} hdrovhd_info_state_enum_type;

/* AccessParameters messages state */

typedef enum
{
  HDROVHD_AP_NOT_UPDATED,          /* AccessParameters have not been updated */
  HDROVHD_AP_UPDATED               /* AccessParameters have been updated     */
} hdrovhd_ap_state_enum_type;


#ifdef FEATURE_HDR_TO_LTE
/* OtherRATNeighborList messages state */

typedef enum
{
  HDROVHD_ORNL_NOT_UPDATED,        /* OtherRATNeighborList have not been updated */
  HDROVHD_ORNL_UPDATED             /* OtherRATNeighborList have been updated     */
} hdrovhd_ornl_state_enum_type;
#endif /* FEATURE_HDR_TO_LTE*/

/* BroadcastOverhead messages state */

typedef enum
{
  HDROVHD_BC_INVALID,        /* Cache entry does not contain a valid msg   */
  HDROVHD_BC_CURRENT,        /* Cache entry current and not in use         */
  HDROVHD_BC_NOT_PARSED      /* Cache entry valid but needs to be reparsed */
} hdrovhd_bc_entry_state_enum_type;

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/* Enumeration of the various states a buffer (msg) can be in within a
   cache entry */
typedef enum
{
  HDROVHD_BUF_NOT_IN_USE, /* Buffer has valid values & is not in use */
  HDROVHD_BUF_READ_IN_PROG, /* Buffer has valid values & it is being
                                  ** read from */
  HDROVHD_BUF_WRITE_IN_PROG /* Buffer has valid values & it is being
                                  **  written to */
} hdrovhd_buffer_state_enum_type;

/* Various states of a cache entry.  A cache entry may only be valid when the
   SP msg associated with the entry is itself valid */
typedef enum
{
  HDROVHD_ENTRY_INVALID,
  HDROVHD_ENTRY_VALID,
  HDROVHD_ENTRY_VALID_WHEN_CONN  /* Buffer is valid only during connection  */
} hdrovhd_cache_entry_state_enum_type;

/* When AT receives QP, QC or SP message it uses all the available information
   with that message to uniquely identify an entry in the cache */
typedef enum
{
  HDROVHD_CACHE_KEY_QP_INFO,
    /* QP tells us PN and Channel */
  HDROVHD_CACHE_KEY_QC_INFO,
    /* QC tells us PN, Channel, ColorCode, SectorID24, SectorSig, AccessSig */
  HDROVHD_CACHE_KEY_SP_INFO
    /* SP tells us SectorID128 and Channel */

}hdrovhd_cache_key_type;

/* When QP is received AT has PN and channel to look up the ovhd cache list */
typedef struct
{
  sys_link_type link_info;

} hdrovhd_cache_key_qp_info_type;

/* When QC is received AT in addition to link it also has color code, 
   sectorID24 and sector signature to look up the ovhd cache list */
typedef struct
{
  sys_link_type link_info;
  uint8  color_code;
  uint32 sector_id_24;
  uint16 sector_signature;

} hdrovhd_cache_key_qc_info_type;

/* When SP is recieved AT has sectorID128 (which itself is enough as 
   it is unique across all sectors in given network) to look up the 
   ovhd cache list */
typedef struct
{
  hdraddr_type sp_sector_id_128;
  uint16 sector_signature;
  sys_channel_type channel;

} hdrovhd_cache_key_sp_info_type;

typedef union
{
  hdrovhd_cache_key_qp_info_type qp_info;
  hdrovhd_cache_key_qc_info_type qc_info;
  hdrovhd_cache_key_sp_info_type sp_info;

} hdrovhd_cache_key_info_type;

/*---------------------------------------------------------------------------
                         ACCESSPARAMETERS TEMP BUFFER

  The most recently received AccessParameters message are stored in a
  temporary buffer if a corresponding sectorParameter msg hasn't been stored
  in the cache yet for that sector.
  Elements of the cache are defined by the following type.
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                OVERHEAD & ACCESSPARAMETERS INFORMATION CACHE

  The most recently received SectorParameters and AccessParameters messages
  are stored in a cache.
  Elements of the cache are defined by the following types.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                        QUICKCONFIG 
  The following data structure is used to store the QuickConfig message and 
  the link on which it was received.
---------------------------------------------------------------------------*/

typedef struct
{
  boolean initialized;          /* Keeps track if struct is initialized    */
  sys_link_type link;           /* Link Info on which QC msg is received   */
  uint8  color_code;            /* Color Code                              */
  uint32 sector_id_24;          /* Least Significant 24 bit of SectorID    */
  uint16 sector_signature;      /* Sector Signature                        */
  uint16 access_signature;      /* Access Signature                        */
  uint8  redirect;              /* Redirect field rcvd in QC msg */
  uint8  rpc_count63to0;        /* RPC count rcvd in QC msg (bits 63 to 0) */
  qword  fwd_traffic_valid63to0;/* ForwardTrafficValid rcvd in QC msg */
  boolean rpc_count127to64_included; /* Are the upper 64 RPC bits included? */
  uint8  rpc_count127to64;        /* Number of upper 64 RPC bits included */
  qword  fwd_traffic_valid127to64;/* ForwardTrafficValid rcvd in QC msg */
  boolean rpc_count130to383_included; /* Are the upper 64 RPC bits included? */
  uint8  rpc_count130to383;     /* Number of upper 64 RPC bits includ*/
  /* ForwardTrafficValid130to383 - not supported at this time */
#ifdef FEATURE_HDR_TO_LTE
  boolean other_rat_available;
  uint8 other_rat_txcycle;
  uint8 other_rat_signature;
#endif /* FEATURE_HDR_TO_LTE*/

} hdrovhd_qc_info_type;

/*--------------------------------------------------------------
Sector Parameters
--------------------------------------------------------------*/
typedef struct
{
  hdrovhd_buffer_state_enum_type state;   /* State of sp entry in cache    */
  hdrovhd_sp_msg_type sp_msg;   /* Actual Stored sectorParameters message  */
} hdrovhd_sp_entry_type;

/*--------------------------------------------------------------
Access Parameters
--------------------------------------------------------------*/
typedef struct
{
  boolean is_valid;                     /* Is AP msg valid                 */
  hdrovhd_buffer_state_enum_type state; /* State of entry in cache         */
  hdrovhd_ap_msg_type ap_msg;           /* AccessParameters message        */
  sys_link_type link;                   /* Link Info on which AP msg is received   */
} hdrovhd_ap_entry_type;

#ifdef FEATURE_HDR_BCMCS
/*--------------------------------------------------------------
Broadcast Overhead
--------------------------------------------------------------*/
typedef struct
{
  hdrovhd_bc_entry_state_enum_type state; /* State of entry in cache       */
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
  dsm_item_type *bc_item_ptr;           /* Pointer to packed message       */
  hdrbcovhd_bc_msg_type bc_msg;         /* BroadcastOverhead message       */
} hdrovhd_bc_entry_type;
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_TO_LTE
/*--------------------------------------------------------------
Other RAT Neighbor List message
--------------------------------------------------------------*/
#define HDROVHD_NUM_OTHER_RAT_MAX 1
#define HDROVHD_MAX_PLMNIDS_PER_FREQ 8

typedef struct
{
  uint8 num_plmn_ids;
    /* Number of PLMN IDs that are associated with this EARFCN */

  boolean plmn_same_as_prev_channel;
    /* PLMN IDs of this channel is same as the previous EARFCN */
  
  sys_plmn_id_s_type plmnid[HDROVHD_MAX_PLMNIDS_PER_FREQ];
    /* PLMNID of the neighboring EUTRA system */

} hdrovhd_lte_plmnid_list_type;

typedef struct
{
  hdrsrch_lte_nset_update_params_type lte_nset;
    /* LTE neighbor list */

  boolean plmnid_included;
    /* Whether PLMNID is included */

  hdrovhd_lte_plmnid_list_type lte_plmnid_list[HDRSRCH_LTE_FREQ_ARR_SIZE];
    /* List of PLMNIDs associated with EUTRA freq */

  boolean serving_nw_plmn_included;
    /* Whether Serving Network PLMN is included */

  hdrovhd_lte_plmnid_list_type lte_serving_plmnid_list;
	
} hdrovhd_lte_rat_rec_type;

typedef struct
{
  uint8 rat_type;
    /* Radio Access Technology Type */

  union
  {
    hdrovhd_lte_rat_rec_type lte_nbr;
    /* LTE neighbor list and plmn information */
  }other_rat_rec;

} hdrovhd_neighbor_rat_record_type;

typedef struct
{
  uint8 other_rat_signature; 
    /* OtherRATNeighborList message signature */

  uint8 num_other_rat; 
    /* Number of other RAT type records included in this message */

  hdrovhd_neighbor_rat_record_type other_rat[HDROVHD_NUM_OTHER_RAT_MAX];
    /* Other RAT type records*/

} hdrovhd_ornl_entry_type;
#endif /* FEATURE_HDR_TO_LTE*/

typedef struct
{
  uint16 num_fplmn;
    /* Number of Forbidden PLMN IDs */

  sys_plmn_id_s_type fplmnid[HDROVHD_MAX_FPLMN_LIST_SIZE];
    /* Forbidden PLMN list */

} hdrovhd_lte_fplmn_list_type;

#ifdef FEATURE_LTE_DEPRIORITIZATION
typedef struct
{
  boolean depri_all_freq;
    /* Indicate if all EUTRA neighbor freqs should be deprioritized */

  uint8 num_depri_freq;
    /* Number of freqs being deprioritized; Will be set to a valid value 
       only if the depri_all_freq boolean is set to FALSE */

  lte_earfcn_t depri_freq[HDRCP_MAX_LTE_DEPRI_FREQ];
    /* Set of deprioritized EUTRA frequencies. This will be valid only 
       if the depri_all_freq boolean is set to FALSE */

} hdrovhd_lte_depri_freq;
#endif /* FEATURE_LTE_DEPRIORITIZATION */

/*--------------------------------------------------------------
Cache entry
--------------------------------------------------------------*/
typedef struct
{
  hdrovhd_cache_entry_state_enum_type cache_entry_state;
    /* State of the whole cache entry */

  qword  ts_explicit;
    /* System time at instant that SP msg is received. The timestamp is tied
       to the SP msg since the cache entry is only valid when SP is valid     */

  qword  ts_implicit;
     /* System time at instant that QC msg is received and SectorSignature is 
        current   */

  sys_link_type rx_link;
  /* Channel on which the msg was rcvd. 
     The pilot PN is ONLY used to report later to 
     SRCH what pilots are in our cache in order to allow SRCH to distinguish 
     cheap vs. expensive idle handoff.
     */

  uint8 config_change;
  /* ConfigurationChange bits associated with this sector */

  boolean qc_is_valid;
   /* Flag indicating whether QC message is valid or not (alternatively 
      this could be thought as whether AT can go to sleep based on decoding
      QP message) */

  hdrovhd_qc_info_type  qc_entry;
    /* QC msg */

  hdrovhd_sp_entry_type sp_entry;
    /* SP msg */

  hdrovhd_ap_entry_type ap_entry;
    /* AP msg */

#ifdef FEATURE_HDR_BCMCS
  hdrovhd_bc_entry_type bc_entry;
    /* BroadcastOverhead msg */
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_TO_LTE
   hdrovhd_ornl_entry_type ornl_entry;
    /* OtherRAT NL msg */
#endif /* FEATURE_HDR_TO_LTE*/

} hdrovhd_cache_entry_type;

/* This structure stores the current subnet id & subnet mask if it is valid */
typedef struct
{
  boolean is_valid;
  hdraddr_type subnet_id;
  uint8   subnet_mask;
  sys_channel_type	ch;
} hdrovhd_subnet_type;

/* As the QC & SP messages are processed, the following data structure is
   filled. Based on the contents of this structure, at the end of
   processing, the appropriate indication is given. The advantage of this is
   that multiple redundant indications are not given while processing the msgs,
   but rather a single appropriate indication is given at the end */
typedef struct
{
  boolean need_to_give_ind;           /* Does any ind. need to be given */
  hdrind_ind_name_enum_type ind_name; /* Name of indication to be given */
  boolean need_to_give_ap_ind;        /* May need to give both AP and OVHD INFO
                                         updated indications */
} hdrovhd_ind_to_give_type;

typedef enum
{
  HDROVHD_QC_SUPERVISION_TIMER,  /* QuickConfig supervision timer */
  HDROVHD_SP_SUPERVISION_TIMER,  /* Sector Parameters supervision timer */
  HDROVHD_AP_SUPERVISION_TIMER,  /* Access Parameters supervision timer */
  
  HDROVHD_QC_INSANITY_TIMER,     /* QuickConfig insanity timer */
  HDROVHD_SP_INSANITY_TIMER,     /* Sector Parameters insanity timer */

  HDROVHD_BC_SUPERVISION_TIMER   /* Broadcast Overhead supervision timer */
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
 ,HDROVHD_AP_UPDATE_WAIT_TIMER  /* Access Parameters update wait timer */
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
 ,
  HDROVHD_SYS_AVOID_TIMER       /* Avoidance timer for SD Avoided channels in NSET */  
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
#ifdef FEATURE_HDR_TO_LTE
 ,
  HDROVHD_EARFCN_AVOID_TIMER       /* Avoidance timer for EARFCN for D2L reselection */  
#endif /* FEATURE_HDR_TO_LTE */

} hdrovhd_timer_enum_type;

typedef struct
{
  q_link_type link;          /* Queue link */
  hdraddr_type subnet_id;
  uint8        subnet_mask;
  uint16       route_update_trigger_code;
  uint32       timestamp; /* When entry was last updated, units of 1.28s */
  boolean      rup_was_acked;
} hdrovhd_rutc_entry_type;

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */


#ifdef FEATURE_LTE_TO_HDR_OH
typedef struct
{
  /* Flag indicating whether PreReg_Params is presented or not. */
  boolean sib8_is_updated;

  /* Flag indicating whether PreReg_Params is presented or not. */
  boolean prereg_is_presented;

  /* Flag indicating whether Previous PreReg_Params is presented or not. */
  boolean prev_prereg_is_presented;

  /* Flag indicating whether pre-registration is allowed or not. */
  boolean prereg_is_allowed;

  /* The previous value of pre-registration is allowed flag. */
  boolean prev_prereg_is_allowed;

  /* The previous value of number of secondary pre_reg_zone_id. */
  uint8 prev_sec_cc_count;

  /* The previous value of number of neighbor cells */
  uint8 prev_neighbor_count;

  /* Current LTE global cell ID */
  lte_rrc_global_cell_id_t cell_id; 

  /* Whether it is the first time rcvd service indication */
  boolean cell_id_initialized;

  /* First SIB8 or preReg after LTE OOS */
  boolean waiting_prereg_after_oos;

} hdrovhd_tunnel_ovhd_info_type;
 
typedef PACK( struct ) {
  /* Total number of valid entries in the items array */
   uint8  count;

  /* Carrier configured mcc list where Prior Session is not supported */
   uint32 mcc_list[HDROVHD_MCC_LIST_MAX];
} hdrovhd_mcc_list_type;

#endif /* FEATURE_LTE_TO_HDR_OH */

/* The following structure contains all the global variables used by OVHD */
typedef struct
{
  /* Timers to ensure AN is sending valid overhead information */
  rex_timer_type qc_insanity_timer;
  rex_timer_type sp_insanity_timer;
  
  /* The QuickConfig, SectorParameters & AccessParameters supervision timers */
  rex_timer_type qc_supervision_timer;
  rex_timer_cnt_type saved_qc_superv_time_ms;

  rex_timer_type sp_supervision_timer;
  rex_timer_cnt_type saved_sp_superv_time_ms;

  rex_timer_type ap_supervision_timer;

  rex_timer_type ap_update_wait_timer;

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  rex_timer_type sys_avoid_timer;
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

#ifdef FEATURE_HDR_BCMCS

  rex_timer_type bc_supervision_timer;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS_MFRQ
  boolean ignore_bc_hash_override; 
#endif /* FEATURE_HDR_BCMCS_MFRQ */

#endif /* FEATURE_HDR_BCMCS */

  /* Variable that keeps track of overhead info state */
  hdrovhd_info_state_enum_type info_state;

  /* This boolean is set while an Idle HandOff occurs while overhead is in
  sleep state. It is used to determine the state that overhead should
  transition to on waking up */
  boolean idle_ho_while_sleep;

  /* Boolean that records if a SID has been received after initial acquisition
   of the system. It is used in informing system determination of the SID */
  boolean acq_sid_rxed;

  /* This boolean is set after acquiring the system.  It is used in the
  OVHD_UPDATED ind to track the first update after acquisition */
  boolean is_first_after_acquisition;

  /* Expected link ( ie. Channel & PN ) on which messages will be rcvd */
  sys_link_type expected_link;

  /* Temporary buffer for storing recvd AccessParameters messages if a
  current entry doesn't exist yet in the cache for that sector. */
  hdrovhd_ap_entry_type ap_buffer;

  /* Variable that keeps track of access parameters state */
  hdrovhd_ap_state_enum_type ap_state;

#ifdef FEATURE_HDR_TO_LTE
  /* Variable that keeps track of OtherRATNeighborList state */
  hdrovhd_ornl_state_enum_type ornl_state;
#endif /* FEATURE_HDR_TO_LTE*/

  /* Cache for storing recvd SectorParameters messages */
  hdrovhd_cache_entry_type cache[HDROVHD_MAX_CACHE_RECORDS];

  /* A pointer to the current cache entry node */
  hdrovhd_cache_entry_type *curr_cache_entry_ptr;

  /* Struct that keeps track of the current subnet & if it is valid. The
  subnet becomes valid when we get a Sector Parameters matching a previous
  QuickConfig msg. It becomes invalid when when the QC color code changes */
  hdrovhd_subnet_type curr_subnet;
  hdrovhd_subnet_type last_sd_subnet;

  /* Stores info about the link on which the most recent QC msg was recvd */
  hdrovhd_qc_info_type qc_info;

  uint8  last_capsule_length_max; /* Max number of frames in access capsule */

  /* RouteUpdateTriggerCode related fields */
  hdrovhd_rutc_entry_type rutc_buf_pool[HDROVHD_MAX_RUTC_ENTRIES];
  q_type rutc_list;
  q_type rutc_free_q;

  /* HDR Search operational mode based on 1x neighbors in the 
   * SectorParameters message */
  hdrsrch_opmode_config_enum_type op_mode;

  /* boolean indicating if cache_entry has changed */
  boolean cache_changed;

  boolean cgi_info_is_valid;

  /* Pilot report to be reported in CGI response */
  hdrsrch_pilot_rpt_type cgi_info_pilot_rpt;

  /* Temporary buffer for unpacking SP message before storing into the cache */
  hdrovhd_sp_msg_type sp_buffer;

  /* Flag indicating whether the QP message received is the first 
     after idle ho */
  boolean is_first_qp_after_idle_ho;

  /* Flag indicating whether the QC message is received after entering 
     Monitor State. It's mainly used to check the sleep conditions */ 
  boolean qc_is_received;

  hdrovhd_pref_default_cache_period_type pref_default_cache_period;

#ifdef FEATURE_HDR_REVC
  hdrovhd_nlb_cache_type nlb_cache[HDROVHD_NLB_CACHE_SIZE];
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrovhd_tunnel_ovhd_info_type tunnel_info;
  
  /* Whether MCC is received */
  boolean mcc_is_rxed;
      
  /* MCC list that where Prior Session is not supported */
  hdrovhd_mcc_list_type no_ps_mcc;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Flag indicating whether HDR to LTE reselection is enabled */
  boolean hdr_to_lte_resel_enabled;

#ifdef FEATURE_IRAT_FORCE_BSR_D2L_SETTING 
  /* Whether to turn on force D2L feature */
  boolean enable_dynamic_d2l;

  /* Force Do to LTE reselection regardless of NV 66162. It should be TRUE
   * when enable_dynamic_d2l is TRUE and AT is in KDDI's network. */
  boolean force_d2l;
#endif /* FEATURE_IRAT_FORCE_BSR_D2L_SETTING */

  hdrovhd_lte_fplmn_list_type fplmn_list; 

#ifdef FEATURE_LTE_DEPRIORITIZATION
  hdrovhd_lte_depri_freq depri_freq;
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_HDR_TO_LTE
  /* Use this flag to make HDR send SPLMN to SD at least once after powerup 
   * or reacquisition */
  boolean first_splmn_after_acq;

  /* Serving network PLMN IDs information */
  sd_splmn_info_list_s splmn_info;

lte_earfcn_t avoided_earfcn[HDROVHD_MAX_EARFCN_AVOID];
uint8        num_avoided_earfcn;
rex_timer_type avoid_earfcn_timer;

#endif /* FEATURE_HDR_TO_LTE */

} hdrovhd_static_data_type;

static hdrovhd_static_data_type hdrovhd;

/*---------------------------------------------------------------------------
                     MESSAGE FIELD EXTRACTION MACROS

  Following macros are used for unpacking and extracting the individual
  fields of the received message
---------------------------------------------------------------------------*/

/* Mesage ID */
#define HDROVHD_MSG_ID( ptr, val_ptr )  \
           HDRBIT_UNPACK8( ptr, val_ptr, 0,  8 )

/*---------------------------------------------------------------------------
                     QUICKCONFIG FIELD EXTRACTION MACROS

  Following macros are used for unpacking and extracting the individual
  fields of the QuickConfig message
---------------------------------------------------------------------------*/

/* Color Code */
#define  HDROVHD_QC_COLOR_CODE( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 8,  8 )

/* Sector Id */
#define HDROVHD_QC_SECTOR_ID_24( ptr, val_ptr ) \
          HDRBIT_UNPACK32( ptr, val_ptr, 16,  24 )

/* Sector Signature */
#define HDROVHD_QC_SECTOR_SIGNATURE( ptr, val_ptr ) \
          HDRBIT_UNPACK16( ptr, val_ptr, 40,  16 )

/* Access Signature */
#define HDROVHD_QC_ACCESS_SIGNATURE( ptr, val_ptr ) \
          HDRBIT_UNPACK16( ptr, val_ptr, 56,  16 )

/* Redirect */
#define HDROVHD_QC_REDIRECT( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 72,  1 )

/* RPC Count bits 63 to 0 */
#define HDROVHD_QC_RPC_COUNT63TO0( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 73,  6 )

/* Forward Traffic Valid bits 63 to 0 */
#define HDROVHD_QC_FWD_TRAFFIC_VALID63TO0( ptr, val_ptr, size ) \
          HDRBIT_UNPACK64( ptr, val_ptr, 79,  size )

#define HDROVHD_QC_RPC_COUNT63TO0_OFFSET 79

#define HDROVHD_QC_RPC_COUNT_INCLUDED_SIZE 1
#define HDROVHD_QC_RPC_COUNT127TO64_SIZE 6
#define HDROVHD_QC_RPC_COUNT130TO383_SIZE 8
#ifdef FEATURE_HDR_TO_LTE
#define HDROVHD_QC_OTHER_RAT_AVAILABLE_SIZE 1
#define HDROVHD_QC_OTHER_RAT_TX_CYCLE_SIZE 3
#define HDROVHD_QC_OTHER_RAT_SIGNATURE_SIZE 6
#endif /* FEATURE_HDR_TO_LTE*/

/*---------------------------------------------------------------------------
                    SECTORPARAMETERS FIELD EXTRACTION MACROS

  Following macros are used for unpacking and extracting the individual
  fields of the SectorParameters message
---------------------------------------------------------------------------*/
#define HDROVHD_BIT22   0x200000
#define HDROVHD_XTEND22 0xFFE00000

/* sign extend 22 bit signed integer to signed long */
#define HDROVHD_EXTEND22(num22) \
  (int32) (((num22) & HDROVHD_BIT22) ? ((num22) | HDROVHD_XTEND22) : (num22))

#define HDROVHD_BIT23   0x400000
#define HDROVHD_XTEND23 0xFFC00000

/* sign extend 23 bit signed integer to signed long */
#define HDROVHD_EXTEND23(num23) \
  (int32) (((num23) & HDROVHD_BIT23) ? ((num23) | HDROVHD_XTEND23) : (num23))

#define HDROVHD_BIT11   0x400
#define HDROVHD_XTEND11 0xFC00

/* sign extend 11 bit signed integer to signed word */
#define HDROVHD_EXTEND11(num11) \
  (int16) (((num11) & HDROVHD_BIT11) ? ((num11) | HDROVHD_XTEND11) : (num11))

#define HDROVHD_BIT5   0x10
#define HDROVHD_XTEND5 0xFFE0

/* sign extend 5 bit signed integer to signed word */
#define HDROVHD_EXTEND5(num5) \
  (int16) (((num5) & HDROVHD_BIT5) ? ((num5) | HDROVHD_XTEND5) : (num5))

/* Size of various fields in the SP msg */
#define HDROVHD_SP_CHANNEL_COUNT_SIZE                 5
#define HDROVHD_SP_BAND_CLASS_SIZE                    5
#define HDROVHD_SP_CHANNEL_NUMBER_SIZE               11
#define HDROVHD_SP_SYSTEM_TYPE_SIZE                   8
#define HDROVHD_SP_NEIGHBOR_COUNT_SIZE                5
#define HDROVHD_SP_NEIGHBOR_PN_SIZE                   9
#define HDROVHD_SP_NEIGHBOR_CHAN_INCL_SIZE            1
#define HDROVHD_SP_NEIGHBOR_SRCH_WIN_SIZE_INCL_SIZE   1
#define HDROVHD_SP_NEIGHBOR_SRCH_WINSIZE_SIZE         4
#define HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET_INCL_SIZE 1
#define HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET_SIZE      3

/* Offset in bits of the first Channel field from the start of SP msg */
#define HDROVHD_SP_CHANNELLIST_OFFSET 256

#ifdef FEATURE_HDR_REVC
#define HDROVHD_SP_CHANNEL_RECORD_SIZE                24
#define HDROVHD_SP_CHANNEL_COUNT_OFFSET               251
#endif /* FEATURE_HDR_REVC */

/* Country Code */
#define HDROVHD_SP_COUNTRY_CODE( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, val_ptr, 8, 12 )

/* Sector ID */
#define HDROVHD_SP_SECTOR_ID( ptr, val_ptr )  \
               hdraddr_unpack_addr( ptr, val_ptr, 20 )

/* Subnet Mask */
#define HDROVHD_SP_SUBNET_MASK( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 148, 8 )

/* Sector Signature */
#define HDROVHD_SP_SECTOR_SIGNATURE( ptr, val_ptr )  \
            HDRBIT_UNPACK16( ptr, val_ptr, 156,  16 )

/* Latitude */
#define HDROVHD_SP_LATITUDE( ptr, val_ptr )  \
          HDRBIT_UNPACK32( ptr, (uint32 *) val_ptr, 172,  22 )

/* Longitude */
#define HDROVHD_SP_LONGITUDE( ptr, val_ptr )  \
          HDRBIT_UNPACK32( ptr, (uint32 *) val_ptr, 194,  23 )

/* Route Update Radius */
#define HDROVHD_SP_ROUTE_UPDATE_RADIUS( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, val_ptr, 217,  11 )

/* Leap Seconds */
#define HDROVHD_SP_LEAP_SECONDS( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 228,  8 )

/* Local Time Offset */
#define HDROVHD_SP_LOCAL_TIME_OFFSET( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, (uint16 *) val_ptr, 236,  11 )

/* Reverse Link Silence Duration */
#define HDROVHD_SP_REV_LINK_SILENCE_DURATION( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 247,  2 );

/* Reverse Link Silence Period */
#define HDROVHD_SP_REV_LINK_SILENCE_PERIOD( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 249,  2 );

/* Number of channels */
#define HDROVHD_SP_CHANNEL_COUNT( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 251,  5 );

/* Channel from Channel Record */
#define HDROVHD_SP_BAND_CLASS( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  5 )

/* Channel Record. Channel Number */
#define HDROVHD_SP_CHANNEL_NUMBER( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  11 )

/* Channel Record System Type */
#define HDROVHD_SP_SYSTEM_TYPE( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  8 )

/* Number of Neighbors */
#define HDROVHD_SP_NEIGHBOR_COUNT( ptr, val_ptr , offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  5 )

/* Neighbor PN */
#define HDROVHD_SP_NEIGHBOR_PN( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  9 )

/* Neighbor Channel Included */
#define HDROVHD_SP_NEIGHBOR_CHAN_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* Neighbor Search Window Size Included */
#define HDROVHD_SP_NEIGHBOR_SRCH_WIN_SIZE_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* Neighbor Search Window Size */
#define HDROVHD_SP_NEIGHBOR_SRCH_WINSIZE( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, 4 )

/* Neighbor Search Window Offset Included */
#define HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* Neighbor Search Window Offset */
#define HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  3 )

/*---------------------------------------------------------------------------
                    ACCESSPARAMETERS FIELD EXTRACTION MACROS

  Following macros are used for unpacking and extracting the individual
  fields of the AccessParameters message
---------------------------------------------------------------------------*/
#define HDROVHD_AP_APERSISTENCE_OFFSET 60 /* Offset of first Apersistence field
                                          from start of AP msg */

#define HDROVHD_AP_APERSISTENCE_SIZE   6  /* Size of Apersistence field */

/* Access Signature */
#define HDROVHD_AP_ACCESS_CYCLE_DURATION( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 8,  8 )

/* Access Signature */
#define HDROVHD_AP_ACCESS_SIGNATURE( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, val_ptr, 16,  16 )

/* Nominal Power */
#define HDROVHD_AP_NOMINAL_POWER( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 32,  8 )

/* Probe Initial Adjust */
#define HDROVHD_AP_PROBE_INITIAL_ADJUST( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 40,  5 )

/* Probe Num Step */
#define HDROVHD_AP_PROBE_NUM_STEP( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 45,  4 )

/* Power Step */
#define HDROVHD_AP_POWER_STEP( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 49, 4 )

/* Preamble Length */
#define HDROVHD_AP_PREAMBLE_LENGTH( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 53,  3 )

/* Capsule Length Max */
#define HDROVHD_AP_CAPSULE_LENGTH_MAX( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 56,  4 )

/* APersistence */
#define HDROVHD_AP_A_PERSISTENCE( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  6 )

#define HDROVHD_AP_ENHANCED_ACCESS_PARAMETERS_INCLUDED_SIZE   1
#define HDROVHD_AP_PREAMBLE_LENGTH_SLOTS_SIZE                 1
#define HDROVHD_AP_ACCESS_OFFSET_SIZE                         2
#define HDROVHD_AP_SECTOR_ACCESS_MAX_RATE_SIZE                2
#define HDROVHD_AP_PROBE_TIME_OUT_ADJUST_SIZE                 3
#define HDROVHD_AP_PILOT_STRENGTH_NOMINAL_SIZE                3
#define HDROVHD_AP_PILOT_STRENGTH_CORRECTION_MIN_SIZE         3
#define HDROVHD_AP_PILOT_STRENGTH_CORRECTION_MAX_SIZE         3
#define HDROVHD_AP_MAX_PN_RANDOMIZATION_DELAY_INCLUDED_SIZE   1
#define HDROVHD_AP_MAX_PN_RANDOMIZATION_DELAY_SIZE            4


/* <EJECT> */

#ifdef FEATURE_HDR_TO_LTE
/*---------------------------------------------------------------------------
           OTHERRAT NEIGHBORLIST FIELD EXTRACTION MACROS

  Following macros are used for unpacking and extracting the individual
  fields of the OtherRATNeighborList message
---------------------------------------------------------------------------*/
#define HDROVHD_ORNL_EUTRA_RAT_TYPE 0x00

#define HDROVHD_ORNL_FIXED_FIELDS_SIZE 18
#define HDROVHD_ORNL_EUTRAN_OTHER_RAT_TYPE_SIZE 4
#define HDROVHD_ORNL_EUTRAN_NGBR_RAT_REC_LEN_SIZE 10
#define HDROVHD_ORNL_EUTRAN_PRIORITY_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_SERVING_PRIORITY_SIZE 3
#define HDROVHD_ORNL_EUTRAN_THRESH_SERVING_SIZE 6
#define HDROVHD_ORNL_EUTRAN_PER_EARFCN_PARAMS_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_COMMON_SIZE 7
#define HDROVHD_ORNL_EUTRAN_PE_MAX_COMMON_SIZE 6
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON_SIZE 3
#define HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER_SIZE 4
#define HDROVHD_ORNL_EUTRAN_SRCH_BACKOFF_TIMER_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_MIN_MESAUREMENT_BACKOFF_SIZE 4
#define HDROVHD_ORNL_EUTRAN_MAX_MESAUREMENT_BACKOFF_SIZE 4
#define HDROVHD_ORNL_EUTRAN_PLMNID_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_NUM_FREQ_SIZE 3
#define HDROVHD_ORNL_EUTRAN_EARFCN_SIZE 16
#define HDROVHD_ORNL_EUTRAN_EARFCN_PRIORITY_SIZE 3
#define HDROVHD_ORNL_EUTRAN_EARFCN_THRESHX_SIZE 5
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_SIZE 7
#define HDROVHD_ORNL_EUTRAN_PE_MAX_SIZE 6
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_SIZE 3
#define HDROVHD_ORNL_EUTRAN_MEAS_BW_SIZE 3
#define HDROVHD_ORNL_EUTRAN_PLMN_SAME_AS_PREV_CH_SIZE 1
#define HDROVHD_ORNL_EUTRAN_NUM_PLMNIDS_SIZE 3
#define HDROVHD_ORNL_EUTRAN_PLMNID_SIZE 4
#define HDROVHD_ORNL_EUTRAN_SERVING_NW_PLMNID_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_NUM_SERVING_PLMNIDS_SIZE 3
#define HDROVHD_ORNL_EUTRAN_S_PLMNID_SIZE 4
#define HDROVHD_ORNL_EUTRAN_RSRQ_PARAMS_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_COMMON_SIZE 5
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON_INC_SIZE 1
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON_SIZE 3
#define HDROVHD_ORNL_EUTRAN_EARFCN_THRESHXQ_SIZE 5
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_SIZE 5
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_INCL_SIZE 1
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_SIZE 3

/* OtherRATSignature */
#define HDROVHD_ORNL_OTHER_RAT_SIGNATURE( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 8, 6 )
/* NumOtherRAT */
#define HDROVHD_ORNL_NUM_OTHER_RAT( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 14, 4 )

/* RATType */
#define HDROVHD_ORNL_OTHER_RAT_TYPE( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 4 )

/* NeighborRATRecordLength */
#define HDROVHD_ORNL_NGBR_RAT_REC_LEN( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, offset, 10 )

/* PriorityIncluded */
#define HDROVHD_ORNL_EUTRAN_PRIORITY_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* ServingPriority */
#define HDROVHD_ORNL_EUTRAN_SERVING_PRIORITY( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* ThreshServing */
#define HDROVHD_ORNL_EUTRAN_THRESH_SERVING( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 6 )

/* PerEARFCNParamsIncluded */
#define HDROVHD_ORNL_EUTRAN_PER_EARFCN_PARAMS_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* RxLevMinEUTRACommon */
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_COMMON( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 7 )

/* PEMaxCommon */
#define HDROVHD_ORNL_EUTRAN_PE_MAX_COMMON( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 6 )

/* RxLevMinEUTRAOffsetCommonIncl */
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* RxLevMinEUTRAOffsetCommon */
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* MaxReselectionTimerIncluded */
#define HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* MaxReselectionTimer */
#define HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 4 )

/* SearchBackOffTimerIncluded */
#define HDROVHD_ORNL_EUTRAN_SRCH_BACKOFF_TIMER_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* MinMeasurementBackoff */
#define HDROVHD_ORNL_EUTRAN_MIN_MESAUREMENT_BACKOFF( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 4 )

/* MaxMeasurementBackoff */
#define HDROVHD_ORNL_EUTRAN_MAX_MESAUREMENT_BACKOFF( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 4 )

/* PLMNIDIncluded */
#define HDROVHD_ORNL_EUTRAN_PLMNID_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* NumEUTRAFrequencies */
#define HDROVHD_ORNL_EUTRAN_NUM_FREQ( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* EARFCN */
#define HDROVHD_ORNL_EUTRAN_EARFCN( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, offset, 16 )

/* EARFCNPriority */
#define HDROVHD_ORNL_EUTRAN_EARFCN_PRIORITY( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* ThreshX */
#define HDROVHD_ORNL_EUTRAN_EARFCN_THRESHX( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 5 )

/* RxLevMinEUTRA */
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 7 )

/* PEMax */
#define HDROVHD_ORNL_EUTRAN_PE_MAX( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 6 )

/* RxLevMinEUTRAOffsetIncl */
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* RxLevMinEUTRAOffset */
#define HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* MeasurementBandwidth */
#define HDROVHD_ORNL_EUTRAN_MEAS_BW( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* PLMNSameAsPreviousChannel */
#define HDROVHD_ORNL_EUTRAN_PLMN_SAME_AS_PREV_CH( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* NumPLMNIDs */
#define HDROVHD_ORNL_EUTRAN_NUM_PLMNIDS( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* PLMNID */
#define HDROVHD_ORNL_EUTRAN_PLMNID( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 4 )

/* ServingNetworkPLMNIDIncluded */
#define HDROVHD_ORNL_EUTRAN_SERVING_NW_PLMNID_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* NumServingPLMNIDs */
#define HDROVHD_ORNL_EUTRAN_NUM_SERVING_PLMNIDS( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* S_PLMNID */
#define HDROVHD_ORNL_EUTRAN_S_PLMNID( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 4 )

/* RSRQParametersIncluded */
#define HDROVHD_ORNL_EUTRAN_RSRQ_PARAMS_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* QualMinEUTRACommon */
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_COMMON( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 5 )

/* QualMinEUTRAOffsetCommonInc */
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON_INC( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* QualMinEUTRAOffsetCommon */
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

/* ThreshXQ */
#define HDROVHD_ORNL_EUTRAN_EARFCN_THRESHXQ( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 5 )

/* QualMinEUTRA */
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 5 )

/* QualMinEUTRAOffsetInc */
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_INCL( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 1 )

/* QualMinEUTRAOffset */
#define HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, 3 )

#endif /* FEATURE_HDR_TO_LTE*/

#ifdef FEATURE_LTE_TO_HDR_OH
/* ReverseLinkSilenceDuration */
#define HDROVHD_HSP_RL_SILENCEDURATION( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 8, 2 )
/* ReverseLinkSilencePeriod */
#define HDROVHD_HSP_RL_SILENCEPERIOD( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 10, 2 )
#endif /* FEATURE_LTE_TO_HDR_OH */

  /*---------------------------------------------------------------------------
  BroadcastOverhead Message extraction macro
  ---------------------------------------------------------------------------*/
/* Broadcast Overhead Signature */
#define HDROVHD_BC_OVERHEAD_SIGNATURE( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, val_ptr, 8, 16 )

/* Broadcast Sector Signature */
#define HDROVHD_BC_SECTOR_SIGNATURE( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, val_ptr, 24, 16 )

/* MultiplexCnt field in the BCOvhd message */
#define HDROVHD_BC_MULTIPLEX_CNT( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 45, 2 )

/* Each logical channel record can have max ~ 368 bits
   ( 368 * 8 )/8 ~= 368  bytes + Up to 20 more bytes for remaining
   fields + 16 bytes for sector_id */
#define HDROVHD_MAX_BC_MSG_LENGTH  416

/*---------------------------------------------------------------------------
  Overhead Cache Logging macro
---------------------------------------------------------------------------*/
#define HDROVHD_ADVANCE_PTR(ptr_type, ptr, offset) \
          (ptr_type)((uint32)(ptr) + (uint32)(offset))

#define HDROVHD_ROLLBACK_PTR(ptr_type, ptr, offset) \
          (ptr_type)((uint32)(ptr) - (uint32)(offset))

/*-----------------------------------------------------------------------------
  Extended Channel field layout
-----------------------------------------------------------------------------*/
#define HDROVHD_EXTENDED_CHANNEL_INC_SIZE 1
#define HDROVHD_EXTENDED_CHANNEL_COUNT_SIZE 5
#define HDROVHD_ACCESS_HASHING_CHANNEL_MASK_INC_SIZE 1
#define HDROVHD_ACCESS_HASHING_MASK_LEN_SIZE 4
#define HDROVHD_ACCESS_HASHING_CHANNEL_MASK_SAME_AS_PREV_SIZE 1
#define HDROVHD_RUP_TRIGGER_CODE_INC_SIZE 1
#define HDROVHD_RUP_TRIGGER_CODE_SIZE 12
#define HDROVHD_RUP_TRIGGER_MAX_AGE_SIZE 4
#define HDROVHD_PRIOR_SESSION_GAUP_SIZE 1
#define HDROVHD_FPDCH_SUPPORTED_INC_SIZE 1
#define HDROVHD_FPDCH_SUPPORTED_SIZE 1
#define HDROVHD_SECONDARY_COLORCODE_INC_SIZE 1
#define HDROVHD_SECONDARY_COLORCODE_COUNT_SIZE 3
#define HDROVHD_SECONDARY_COLORCODE_SIZE 8
#define HDROVHD_PILOT_GROUP_ID_LOOP_INC_SIZE 1
#define HDROVHD_PILOT_GROUP_ID_INC_SIZE 1
#define HDROVHD_PILOT_GROUP_ID_SIZE 3
#define HDROVHD_IS_SECTOR_MC_CAPABLE_SIZE 1
/*=============================================================================

                      LOCAL FUNCTION DECLARATIONS

=============================================================================*/

LOCAL void hdrovhd_process_activate_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
);

LOCAL void hdrovhd_process_deactivate_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
);

LOCAL void hdrovhd_process_wakeup_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
);

LOCAL void hdrovhd_process_sleep_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
);

LOCAL void hdrovhd_process_idle_ho_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
);

LOCAL void hdrovhd_process_aho_request_ind( void );

LOCAL void hdrovhd_process_aho_complete_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
);

LOCAL void hdrovhd_process_freq_change_initiated_ind( void ); 

LOCAL void hdrovhd_process_freq_change_complete_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
);

LOCAL void hdrovhd_clear_cache( void );

LOCAL void hdrovhd_clear_cache_entry
(
  hdrovhd_cache_entry_type *cache_entry_ptr
);

LOCAL void hdrovhd_clear_cache_entry_and_state
(
  hdrovhd_cache_entry_type *cache_entry_ptr
);

LOCAL hdrerrno_enum_type hdrovhd_duplicate_cache_entry
(
  hdrovhd_cache_entry_type *from_cache_ptr,
  hdrovhd_cache_entry_type *to_cache_ptr
);

LOCAL void hdrovhd_copy_temp_msg_to_cache
(
  hdrovhd_cache_entry_type *cache_entry_ptr
);

LOCAL void hdrovhd_set_ap_not_updated(void);

LOCAL void hdrovhd_conn_end_reset_info_state( void );

LOCAL void hdrovhd_process_ap_msg
(
  dsm_item_type *item_ptr
);

LOCAL void hdrovhd_ap_info_updated
(
  hdrovhd_ind_to_give_type *ind_to_give_ptr
);

LOCAL hdrerrno_enum_type hdrovhd_unpack_ap_msg
(
  dsm_item_type *item_ptr,
  hdrovhd_ap_entry_type *ap_msg_ptr
);

LOCAL void hdrovhd_process_qc_msg
(
  dsm_item_type *item_ptr
);

LOCAL boolean hdrovhd_qc_is_ok_to_process
(
  sys_link_type *qc_link
);

LOCAL hdrerrno_enum_type hdrovhd_unpack_qc_msg
(
  dsm_item_type        *item_ptr,
  hdrovhd_qc_info_type *qc_info_ptr
);

LOCAL boolean hdrovhd_process_unpacked_qc_msg
(
  const hdrovhd_qc_info_type *qc_info_ptr,
  hdrovhd_ind_to_give_type *ind_to_give_ptr
);

LOCAL void hdrovhd_update_curr_qc_info
(
  hdrovhd_qc_info_type *qc_info_ptr ,
  hdrsrch_link_id_type link_id

);

LOCAL void hdrovhd_process_sp_msg
(
  dsm_item_type *item_ptr
);

LOCAL hdrovhd_cache_entry_type *hdrovhd_find_available_cache_node
( 
  hdrovhd_sp_msg_type *sp_ptr 
);

LOCAL hdrerrno_enum_type hdrovhd_unpack_sp_msg
(
  dsm_item_type *item_ptr ,
  hdrovhd_sp_msg_type *sp_ptr
);

static boolean hdrovhd_process_cache_entry
(
  hdrovhd_cache_entry_type *cache_entry_ptr,
  hdrovhd_ind_to_give_type *ind_to_give_ptr,
  const sys_link_type      *current_link_ptr,
  boolean                  *chan_was_changed_ptr
);

LOCAL boolean hdrovhd_process_channel_change
(
  hdrovhd_cache_entry_type *cache_entry_ptr
);

LOCAL boolean hdrovhd_process_cache_update
(
  hdrovhd_cache_entry_type *cache_entry_ptr,
  hdrovhd_ind_to_give_type *ind_to_give_ptr
);

LOCAL void hdrovhd_give_ind
(
  hdrovhd_ind_to_give_type *ind_to_give_ptr
);

LOCAL boolean hdrovhd_sector_id_24_128_is_equal
(
  uint32 sector_id_24,
  hdraddr_type  sector_id_128
);

LOCAL void hdrovhd_concatenate_sector_id
(
  hdraddr_type sector_id_128,
  uint32 sector_id_24,
  hdrovhd_subnet_type subnet
);

LOCAL void hdrovhd_process_return_to_idle_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
);

LOCAL void hdrovhd_timer_cb
(
  uint32 timer_id
);

static int hdrovhd_compute_hashable_channels
(
  hdrovhd_sp_msg_type *const sp_ptr,
  sys_channel_type    *const hashable_channel_list
);

static hdrerrno_enum_type hdrovhd_sp_msg_is_readable
(
  hdrovhd_cache_entry_type *curr_cache_entry_ptr,
  hdrhai_protocol_name_enum_type caller
);

static errno_enum_type hdrovhd_compute_sp_hash_channel
(
  hdrovhd_sp_msg_type *sp_ptr,
  const sys_link_type *current_link_ptr,
  boolean show_trace_output
);

#ifdef FEATURE_LTE_DEPRIORITIZATION 
#ifdef FEATURE_HDR_TO_LTE
static void hdrovhd_update_earfcn_priority
( 
  hdrsrch_lte_freq_params_type *earfcn
);
#endif /* FEATURE_HDR_TO_LTE */
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_HDR_TO_LTE
static void hdrovhd_update_splmn_info
( 
  hdrovhd_qc_info_type *qc_info_ptr,
  hdrovhd_ornl_entry_type *ornl_msg_ptr
);
#endif /* FEATURE_HDR_TO_LTE */

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

#ifdef FEATURE_HDR_TO_LTE
/*=============================================================================

FUNCTION HDROVHD_SET_ORNL_NOT_UPDATED

DESCRIPTION
  Sets the ORNL msg status to not updated.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_set_ornl_not_updated(void)
{
  hdrovhd.ornl_state = HDROVHD_ORNL_NOT_UPDATED;
} /* hdrovhd_set_ornl_not_updated() */
/* <EJECT> */
#endif /* FEATURE_HDR_TO_LTE*/

/*===========================================================================

FUNCTION HDROVHD_GET_CACHE_PERIOD_ATTRIB

DESCRIPTION
  This function returns the value of overhead cache period.

DEPENDENCIES
  None

PARAMETERS 
  attrib_id: Attribute ID.
  subtype:   Subtype of the protocol

RETURN VALUE
  Negotiated value for the attribute.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrovhd_get_cache_period_attrib
(
  hdrscmovhd_attrib_id_enum_type attrib_id,
  uint16 subtype
)
{
  uint16 cache_period_value = 0;
    /* Negotiated value of cache period */
  uint16 pref_cache_period_value = 0;
    /* Preferred value of cache period */
  uint16 idle_subtype = hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL); 
/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the current value for long/short term overhead cache period */
  (void) hdrscmdb_get_inuse_attrib( 
      HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 
      attrib_id,
      &cache_period_value,
      subtype );

  /* When the negotiated value is 0x00 we use preferred implementation 
     values for Short and Long TermCachePeriod */
  if (cache_period_value == 0x00)
  {
    /* LT */
    if (attrib_id == HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID)
    {
      /* QP */
      if ( idle_subtype == HDRSCP_QUICK_IDLE
#ifdef FEATURE_LTE_TO_HDR_OH
           || idle_subtype == HDRSCP_IRAT_QUICK_IDLE
#endif /* FEATURE_LTE_TO_HDR_OH */
         )
      {
        pref_cache_period_value = 
            hdrovhd.pref_default_cache_period.qp_lt_cache_period;
      }
      else
      {
        pref_cache_period_value = 
            hdrovhd.pref_default_cache_period.qc_lt_cache_period;
      }
    }
    /* ST */
    else if (attrib_id == HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID)
    {
      /* QP */
      if ( idle_subtype == HDRSCP_QUICK_IDLE
#ifdef FEATURE_LTE_TO_HDR_OH
           || idle_subtype == HDRSCP_IRAT_QUICK_IDLE
#endif /* FEATURE_LTE_TO_HDR_OH */
         )
      {
        pref_cache_period_value = 
            hdrovhd.pref_default_cache_period.qp_st_cache_period;
      }
      else
      {
        pref_cache_period_value = 
            hdrovhd.pref_default_cache_period.qc_st_cache_period;
      }
    }
  }
  else
  {
    pref_cache_period_value = cache_period_value; 
  }

  return pref_cache_period_value;

} /* hdrovhd_get_cache_period_attrib */

/*=============================================================================
FUNCTION HDROVHD_LOG_CACHE_SUMMARY

DESCRIPTION
  This function logs overhead cache entry

DEPENDENCIES
  None.
  
PARAMETERS
  Reason - Reason cache is updated.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
static void hdrovhd_log_cache_summary( void )
{
  uint16 log_size;
  uint8 entry_num;
  uint8 total_entries = 0;
  LOG_HDR_OVHD_CACHE_SUMMARY_C_type* log_ptr;
  log_hdr_ovhd_cache_entry_type* log_entry_ptr;
  hdrovhd_cache_entry_type* entry_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!log_status( LOG_HDR_OVHD_CACHE_SUMMARY_C ))
  {
    return;
  }

  /* calculate log packet size */
  log_size = sizeof(LOG_HDR_OVHD_CACHE_SUMMARY_C_type) - 
             sizeof(log_hdr_ovhd_cache_entry_type);

  for (entry_num = 0;entry_num < HDROVHD_MAX_CACHE_RECORDS;entry_num++)
  {
    entry_ptr = &hdrovhd.cache[entry_num];

    if (entry_ptr->cache_entry_state == HDROVHD_ENTRY_INVALID)
    {
      continue;
    }

    total_entries++;
    log_size += sizeof(log_hdr_ovhd_cache_entry_type);

    if (!entry_ptr->ap_entry.is_valid)
    {
      log_size -= sizeof(log_hdr_ovhd_cache_ap_entry_type);
    }

#ifdef FEATURE_HDR_BCMCS	
    if (entry_ptr->bc_entry.state != HDROVHD_BC_CURRENT)
#endif /* FEATURE_HDR_BCMCS */	
    {
      log_size -= sizeof(log_hdr_ovhd_cache_bc_entry_type);
    }
  }

  /* allocate memory for logging. */
  log_ptr = (LOG_HDR_OVHD_CACHE_SUMMARY_C_type*)
            log_alloc_ex((log_code_type) LOG_HDR_OVHD_CACHE_SUMMARY_C,
                          log_size);

  if (log_ptr == NULL)
  {
    HDR_MSG_PROT(MSG_LEGACY_FATAL, "OVHD Cache Summary cannot alloc memory, skip logging");
    return;
  }

  log_ptr->version = HDRLOG_OVHD_CACHE_SUMMARY_LOG_VERSION;

  log_ptr->num_cache_entry = total_entries;
  log_entry_ptr = &log_ptr->cache[0];
    
  HDR_MSG_PROT_1(MSG_LEGACY_LOW, "Valid cache entries:%d", total_entries);

  /* for each log entry */
  for (entry_num = 0;entry_num < HDROVHD_MAX_CACHE_RECORDS;entry_num++)
  {
    entry_ptr = &hdrovhd.cache[entry_num];

    if (entry_ptr->cache_entry_state == HDROVHD_ENTRY_INVALID)
    {
      continue;
    }

    if (hdrovhd.curr_cache_entry_ptr == entry_ptr)
    {
      log_ptr->cur_cache_idx = entry_num;
    }

    log_size = sizeof(log_hdr_ovhd_cache_entry_type);

    log_entry_ptr->index = entry_num;

    log_entry_ptr->config_change =
        entry_ptr->config_change;

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy((void*)log_entry_ptr->timestamp,
             sizeof(log_entry_ptr->timestamp), 
             entry_ptr->ts_explicit,
             sizeof(log_entry_ptr->timestamp));
#else
    memcpy((void*)log_entry_ptr->timestamp,
           entry_ptr->ts_explicit,
           sizeof(log_entry_ptr->timestamp));
#endif /* FEATURE_MEMCPY_REMOVAL */

    log_entry_ptr->band = (uint8) entry_ptr->rx_link.channel.band;
    log_entry_ptr->channel = (uint16) entry_ptr->rx_link.channel.chan_num;
    log_entry_ptr->pilot_pn = (uint16) entry_ptr->rx_link.pilot_pn;

    log_entry_ptr->color_code = entry_ptr->qc_entry.color_code;

    /* if entry is valid, the SP msg must be valid */
    log_entry_ptr->sector_sig = entry_ptr->sp_entry.sp_msg.sector_sig;

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy((void*)log_entry_ptr->sector_id, 
             sizeof(log_entry_ptr->sector_id), 
             entry_ptr->sp_entry.sp_msg.sector_id,
             sizeof(entry_ptr->sp_entry.sp_msg.sector_id));
#else
    memcpy((void*)log_entry_ptr->sector_id,
           entry_ptr->sp_entry.sp_msg.sector_id,
           sizeof(entry_ptr->sp_entry.sp_msg.sector_id));
#endif /* FEATURE_MEMCPY_REMOVAL */

    log_entry_ptr->subnet_mask = entry_ptr->sp_entry.sp_msg.subnet_mask;

    log_entry_ptr->qc_valid = 
        entry_ptr->qc_is_valid;

    log_entry_ptr->ap_included = 
        entry_ptr->ap_entry.is_valid;

#ifdef FEATURE_HDR_BCMCS
    HDR_MSG_PROT_9(MSG_LEGACY_LOW, 
                   "cache %d,band %d,chnl %d,pn %x,sig %d,ap_include %d,"
                   "ap_sig %d,bc_state %d,bc_sig %d",
                   entry_num, log_entry_ptr->band, log_entry_ptr->channel,
                   log_entry_ptr->pilot_pn, log_entry_ptr->sector_sig,
                   log_entry_ptr->ap_included, 
                   entry_ptr->ap_entry.ap_msg.access_sig, 
                   entry_ptr->bc_entry.state,
                   entry_ptr->bc_entry.bc_msg.head.sector_signature
                   );
#else
    HDR_MSG_PROT_7(MSG_LEGACY_LOW, 
                   "cache %d,band %d,chnl %d,pn %x,sig %d,ap_include %d,"
                   "ap_sig %d",
                   entry_num, log_entry_ptr->band, log_entry_ptr->channel,
                   log_entry_ptr->pilot_pn, log_entry_ptr->sector_sig,
                   log_entry_ptr->ap_included, 
                   entry_ptr->ap_entry.ap_msg.access_sig 
                   );
#endif

    if (log_entry_ptr->ap_included)
    {
      log_entry_ptr->ap[0].sig =
          entry_ptr->ap_entry.ap_msg.access_sig;
    }
    else
    {
      /* roll back log_entry_ptr so fields after AP msg are
         aligned correctly. */
      log_entry_ptr = 
          HDROVHD_ROLLBACK_PTR(log_hdr_ovhd_cache_entry_type*, log_entry_ptr, sizeof(log_hdr_ovhd_cache_ap_entry_type));
    }

    #ifdef FEATURE_HDR_BCMCS
    log_entry_ptr->bc_state = entry_ptr->bc_entry.state;
    #else
    log_entry_ptr->bc_state = HDROVHD_BC_INVALID;
    #endif /* FEATURE_HDR_BCMCS */

    #ifdef FEATURE_HDR_BCMCS
    if (log_entry_ptr->bc_state == HDROVHD_BC_INVALID)
    #endif /* FEATURE_HDR_BCMCS */
    {
      log_entry_ptr->bc_packed_msg = (hdrlog_ovhd_cache_pointer_type) NULL;
      log_size -= sizeof(log_hdr_ovhd_cache_bc_entry_type);
    }
    #ifdef FEATURE_HDR_BCMCS
    else if (log_entry_ptr->bc_state == HDROVHD_BC_NOT_PARSED)
    {
      log_entry_ptr->bc_packed_msg = 
          (hdrlog_ovhd_cache_pointer_type) entry_ptr->bc_entry.bc_item_ptr;
      log_size -= sizeof(log_hdr_ovhd_cache_bc_entry_type);
    }
    else
    {
      log_entry_ptr->bc_packed_msg = 
          (hdrlog_ovhd_cache_pointer_type) entry_ptr->bc_entry.bc_item_ptr;
      log_entry_ptr->bc[0].sig = 
          entry_ptr->bc_entry.bc_msg.head.sector_signature;
    }    
    #endif /* FEATURE_HDR_BCMCS */

    log_entry_ptr = 
         HDROVHD_ADVANCE_PTR(log_hdr_ovhd_cache_entry_type*, log_entry_ptr, log_size);
  } /* for each entry */

  log_commit(log_ptr);

  hdrovhd.cache_changed = FALSE;
}

#ifdef FEATURE_LTE_TO_HDR_OH
/*=============================================================================
FUNCTION HDROVHD_RESET_TUNNEL_INFO

DESCRIPTION
  This function processes reset hdrovhd.tunnel_info 

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_reset_tunnel_info( void )
{
  boolean give_prereg_indication = FALSE;
  
  HDR_MSG_PROT( MSG_LEGACY_HIGH, "ovhd tunnel info is reset" );
  
  /* If reset tunnel info is called when pre-reg allowed is TRUE */
  if(hdrovhd.tunnel_info.prereg_is_allowed)
     give_prereg_indication = TRUE;

  hdrovhd.tunnel_info.prereg_is_presented = FALSE;
  hdrovhd.tunnel_info.prev_prereg_is_presented = FALSE;
  hdrovhd.tunnel_info.prereg_is_allowed = FALSE;
  hdrovhd.tunnel_info.prev_prereg_is_allowed = FALSE;
  hdrovhd.tunnel_info.prev_neighbor_count = 0;
  hdrovhd.tunnel_info.prev_sec_cc_count = 0;
  hdrovhd.tunnel_info.sib8_is_updated = FALSE;
  hdrovhd.tunnel_info.cell_id_initialized = FALSE;
  hdrovhd.tunnel_info.cell_id = 0;
  hdrovhd.tunnel_info.waiting_prereg_after_oos = FALSE;
  
  if(give_prereg_indication)
  {
    hdrind_give_ind( HDRIND_OVHD_PREREG_INFO_UPDATED,
                     (void *)&hdrovhd.tunnel_info.prereg_is_allowed );
  }

} /* hdrovhd_reset_tunnel_info() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*=============================================================================
FUNCTION HDROVHD_POWERUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  The QuickConfig & SectorParameters supervision
  timers are initialized, the protocol state is set to Inactive and various
  variables set to their initial values.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_powerup_init( void )
{
  int cnt;  /* Index Counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Initializing Overhead Protocol");

  if ( hdrmc_is_first_init() )
  {
    /*---------------------------------------------------------------------------
    Initialize supervision and insanity timers for QuickConfig, Sector Parameters
    and Access Parameters.
    ---------------------------------------------------------------------------*/
    rex_def_timer_ex( &hdrovhd.qc_supervision_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_QC_SUPERVISION_TIMER );

    rex_def_timer_ex( &hdrovhd.qc_insanity_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_QC_INSANITY_TIMER );

    rex_def_timer_ex( &hdrovhd.sp_supervision_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_SP_SUPERVISION_TIMER );
    
    rex_def_timer_ex( &hdrovhd.sp_insanity_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_SP_INSANITY_TIMER );
    
    rex_def_timer_ex( &hdrovhd.ap_supervision_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_AP_SUPERVISION_TIMER );

    rex_def_timer_ex( &hdrovhd.ap_update_wait_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_AP_UPDATE_WAIT_TIMER );

#ifdef FEATURE_HDR_BCMCS
    rex_def_timer_ex( &hdrovhd.bc_supervision_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_BC_SUPERVISION_TIMER );
#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_TO_LTE
    rex_def_timer_ex(&(hdrovhd.avoid_earfcn_timer),
                     hdrovhd_timer_cb,
                     HDROVHD_EARFCN_AVOID_TIMER);
#endif

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
    rex_def_timer_ex( &hdrovhd.sys_avoid_timer,
                      hdrovhd_timer_cb,
                      HDROVHD_SYS_AVOID_TIMER );
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

    ( void ) q_init( &hdrovhd.rutc_list );
    ( void ) q_init( &hdrovhd.rutc_free_q );
    for ( cnt = 0; cnt < HDROVHD_MAX_RUTC_ENTRIES; cnt++ )
    {
      q_put( &hdrovhd.rutc_free_q, 
             q_link( &hdrovhd.rutc_buf_pool[cnt],
                     &hdrovhd.rutc_buf_pool[cnt].link ) );
    }
  }

  hdrovhd.saved_qc_superv_time_ms = 0;
  hdrovhd.saved_sp_superv_time_ms = 0;
  hdrovhd.cache_changed = FALSE;
  hdrovhd.cgi_info_is_valid = FALSE;

#ifdef FEATURE_LTE_TO_HDR_OH
  memset ( &hdrovhd.tunnel_info, 0, sizeof(hdrovhd.tunnel_info) ); 
  hdrovhd.mcc_is_rxed = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /*---------------------------------------------------------------------------
  Initialize various paramters
  ---------------------------------------------------------------------------*/
  hdrovhd.curr_cache_entry_ptr = NULL;

  /*---------------------------------------------------------------------------
  Initialize cache entry and msg states.
  ---------------------------------------------------------------------------*/
  for( cnt=0; cnt < HDROVHD_MAX_CACHE_RECORDS; cnt++ )
  {
    hdrovhd.cache[cnt].cache_entry_state = HDROVHD_ENTRY_INVALID;
    hdrovhd.cache[cnt].ap_entry.is_valid = FALSE;
#ifdef FEATURE_HDR_BCMCS
    hdrovhd.cache[cnt].bc_entry.state    = HDROVHD_BC_INVALID;
#endif /* FEATURE_HDR_BCMCS*/
  }

  /*---------------------------------------------------------------------------
  Initialize that no AT operational algorithm for Search to run.
  ---------------------------------------------------------------------------*/
  hdrovhd.op_mode = HDRSRCH_OPMODE_DISABLED;

  /*---------------------------------------------------------------------------
  Initialize overhead state module
  ---------------------------------------------------------------------------*/
  hdrovhds_powerup_init();

  /*---------------------------------------------------------------------------
  Initialize preferred defaults for overhead cache period
  ---------------------------------------------------------------------------*/
  memset ( &hdrovhd.pref_default_cache_period, 0,
           sizeof(hdrovhd_pref_default_cache_period_type) );

  if ( efs_get( HDROVHD_PREF_DEFAULT_CACHE_PERIOD_FILENAME,
                &hdrovhd.pref_default_cache_period, 
                sizeof(hdrovhd_pref_default_cache_period_type) ) == 
                (int)sizeof(hdrovhd_pref_default_cache_period_type) )
  {
    HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
             "CachePeriod. Pref Default QP LT=%d QP ST=%d QC LT=%d QC ST=%d ",
             hdrovhd.pref_default_cache_period.qp_lt_cache_period,
             hdrovhd.pref_default_cache_period.qp_st_cache_period,
             hdrovhd.pref_default_cache_period.qc_lt_cache_period,
             hdrovhd.pref_default_cache_period.qc_st_cache_period );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
            "Pref default cache period value not available in NV");

    hdrovhd.pref_default_cache_period.qp_lt_cache_period = 
        HDROVHD_QP_PREF_DEFAULT_LONGTERM_OVHD_CACHE_PERIOD;
    hdrovhd.pref_default_cache_period.qp_st_cache_period = 
        HDROVHD_QP_PREF_DEFAULT_SHORTTERM_OVHD_CACHE_PERIOD;
    hdrovhd.pref_default_cache_period.qc_lt_cache_period = 
        HDROVHD_QC_PREF_DEFAULT_LONGTERM_OVHD_CACHE_PERIOD;
    hdrovhd.pref_default_cache_period.qc_st_cache_period = 
        HDROVHD_QC_PREF_DEFAULT_SHORTTERM_OVHD_CACHE_PERIOD;
  }

  /*---------------------------------------------------------------------------
  Initialize HDR to LTE reselection enabled flag
  ---------------------------------------------------------------------------*/
  if ( efs_get( HDROVHD_HDR_TO_LTE_RESEL_ENABLED_FILENAME,
                &hdrovhd.hdr_to_lte_resel_enabled, 
                sizeof(hdrovhd.hdr_to_lte_resel_enabled) ) == 
                (int)sizeof(hdrovhd.hdr_to_lte_resel_enabled) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "HDR to LTE resel enabled flag=%d",
      hdrovhd.hdr_to_lte_resel_enabled );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "HDR to LTE resel enabled flag not available in NV, enabled by default");
    hdrovhd.hdr_to_lte_resel_enabled = TRUE;
  }

#ifdef FEATURE_IRAT_FORCE_BSR_D2L_SETTING
  hdrovhd.force_d2l = FALSE;

  if ( sd_efs_is_loc_bsr_enabled() )
  {
    hdrovhd.enable_dynamic_d2l = TRUE;
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Force to enable HDR to LTE resel" );
  }
  else
  {
    hdrovhd.enable_dynamic_d2l = FALSE;
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "use default HDR to LTE resel setting" );
  }
#endif /* FEATURE_IRAT_FORCE_BSR_D2L_SETTING */

#ifdef FEATURE_LTE_TO_HDR_OH
  memset ( &hdrovhd.no_ps_mcc, 0, sizeof(hdrovhd_mcc_list_type) );

  if ( efs_get( HDROVHD_MCC_NO_PRIOR_SESSION_FILENAME,
                &hdrovhd.no_ps_mcc, 
                sizeof(hdrovhd_mcc_list_type) ) == 
         (int)sizeof(hdrovhd_mcc_list_type) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                    "%d MCC do not support prior session",
                    hdrovhd.no_ps_mcc.count );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH,
                  "Reading no Prior Session MCC list fails" );
    hdrovhd.no_ps_mcc.count = 0;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_BCMCS
  hdrbcovhd_powerup_init();
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_REVC
  for ( cnt=0; cnt<HDROVHD_NLB_CACHE_SIZE; cnt++)
  {
    hdrovhd.nlb_cache[cnt].state = HDROVHD_NLB_CACHE_INVALID;
  }
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_TO_LTE
  hdrovhd.num_avoided_earfcn = 0;
#endif

} /* hdrovhd_powerup_init() */

/* <EJECT> */

/*=============================================================================
FUNCTION HDROVHD_INIT

DESCRIPTION
  This function initializes the protocol each time HDR mode is entered.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_init( void )
{

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Re-initializing Overhead Protocol");

  /*---------------------------------------------------------------------------
  re-set flags describing the state of HDROVHD
  ---------------------------------------------------------------------------*/
  hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
  hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
  hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */

  hdrovhd.is_first_qp_after_idle_ho = FALSE;
  hdrovhd.acq_sid_rxed = FALSE;
  hdrovhd.is_first_after_acquisition = FALSE;
  hdrovhd.idle_ho_while_sleep = FALSE;
  hdrovhd.last_capsule_length_max = 0;
#ifdef FEATURE_HDR_BCMCS_MFRQ
  hdrovhd.ignore_bc_hash_override = FALSE;
#endif /* FEATURE_HDR_BCMCS_MFRQ */

  if(hdrovhds_get_state() != HDROVHDS_INACTIVE)  
  {	 
	  HDR_MSG_PROT (MSG_LEGACY_HIGH, " Overhead Protocol:Moving to INACTIVE state"); 
	  hdrovhds_change_state( HDROVHDS_INACTIVE );   
	  /* This is to make sure, OVHD is in INACTIVE state and no timers are running during initilization, to recover from a crash. 	   
		 There will not be any side-effects, as it is expected OVHD to be INACTIVE*/  
  }	

} /* hdrovhd_init() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to OVHD.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the overhead protocol.
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
    HDRHAI_IN_USE, msg_ptr );
} /* hdrovhd_msg_cb() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to OVHD.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{
  /*-------------------------------------------------------------------------
  If the ind callback called from within the same task, process it
  immediately, else queue it & tag it for the overhead protocol.
  -------------------------------------------------------------------------*/
  if (rex_self() == HDROVHD_TASK_PTR)
  {
    hdrovhd_process_ind( ind_name, (hdrind_ind_data_union_type*)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, ind_name,
                     ind_data_ptr );
  }
} /* hdrovhd_ind_cb() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_ACTIVATE

DESCRIPTION
  This function commands OVHD to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_activate( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_ACTIVATE_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    Currently no other task ever activates OVHD, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("OVHD activated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof(hdrovhd_cmd_type) );
  }

} /* hdrovhd_activate() */

/* <EJECT> */

#ifdef FEATURE_LTE_TO_HDR_OH
/*=============================================================================

FUNCTION HDROVHD_TUNNEL_DEACTIVATE

DESCRIPTION
  This function commands OVHD to exit tunnel state.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_tunnel_deactivate( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the tunnel deactivate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_TUNNEL_DEACTIVATE_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    Currently no other task ever activates OVHD, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("OVHD activated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof(hdrovhd_cmd_type) );
  }

} /* hdrovhd_tunnel_deactivate() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*=============================================================================

FUNCTION HDROVHD_RETURN_TO_IDLE

DESCRIPTION
  This function commands OVHD to return to PROCESS_ALL_MSGS state and mark
  the overhead as invalid to ensure we re-hash to the correct channel, among
  other things.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_return_to_idle( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_RETURN_TO_IDLE_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    Currently no other task gives this command, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("OVHD return_to_idle from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof(hdrovhd_cmd_type) );
  }

} /* hdrovhd_return_to_idle() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_DEACTIVATE

DESCRIPTION
  This function commands OVHD to deactivate and stop processing overhead
  messages.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_deactivate( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_DEACTIVATE_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in deactivating OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    Currently no other task ever deactivates OVHD, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("OVHD deactivated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof( hdrovhd_cmd_type) );
  }

} /* hdrovhd_deactivate() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_WAKEUP

DESCRIPTION
  This function commands OVHD to Wakeup from sleep & start processing the
  overhead messages.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_wakeup( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_WAKEUP_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in waking up OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    -------------------------------------------------------------------------*/
    ERR("OVHD commanded to wakeup from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof( hdrovhd_cmd_type) );
  }

} /* hdrovhd_wakeup() */
/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_AN_REDIRECTED

DESCRIPTION
  This function commands OVHD to return AN redirected indication.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_an_redirected( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_AN_REDIRECTED_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    Currently no other task ever activates OVHD, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("OVHD activated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof(hdrovhd_cmd_type) );
  }

} /* hdrovhd_an_redirected() */
/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_CHECK_CONFIGURATION

DESCRIPTION
  This function commands OVHD to check whether the overhead information 
  is still valid.

DEPENDENCIES
  None.

PARAMETERS
  config_change - ConfigurationChange field values from QP message.
  link_info - Link on which the QP message was received.
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_check_configuration
( 
  uint8 config_change,
  hdrsrch_link_id_type  link_id,
  sys_link_type link_info,
  hdrhai_protocol_name_enum_type sender 
)
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_CHECK_CONFIG_CMD;
  hdrovhd_cmd.sender = sender;

  hdrovhd_cmd.params.config_change.link_id = link_id;
  hdrovhd_cmd.params.config_change.config_change = config_change;
  hdrovhd_cmd.params.config_change.link = link_info;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating OVHD, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    Currently no other task ever activates OVHD, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("OVHD activated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof(hdrovhd_cmd_type) );
  }

} /* hdrovhd_check_configuration() */
/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_SUSPEND_AND_SLEEP

DESCRIPTION
  This function commands OVHD to suspend its timers then go to sleep.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_suspend_and_sleep( hdrhai_protocol_name_enum_type sender )
{
  /* If Idle is suspending, OVHD will go into SLEEP state but saves how much
     time it has left on its qc and sp supervision timers.  When transitioning
     out of SLEEP, OVHD will resume the QC and SP supervision timer rather than 
     restarting them for their full duration (5.12s).  This allows the QC/SP 
     supervision timers to expire in weak HDR coverage, when HDR keeps 
     suspending to do short tune aways to 1X.  If OVHD is deactivated before
     the saved timers are restored, OVHD will reset its saved timers.
  */
  if (( hdrovhds_get_state() == HDROVHDS_PROCESS_ALL_MSGS ) ||
      ( hdrovhds_get_state() == HDROVHDS_WAIT_FOR_LINK ))
  {
    hdrovhd.saved_qc_superv_time_ms = rex_get_timer(&hdrovhd.qc_supervision_timer);
    hdrovhd.saved_sp_superv_time_ms = rex_get_timer(&hdrovhd.sp_supervision_timer);
  }

  hdrovhd_sleep(sender);

} /* hdrovhd_suspend_and_sleep() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_SLEEP

DESCRIPTION
  This function commands OVHD to sleep.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_sleep( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the Sleep command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_SLEEP_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in sleep command to OVHD, then this is
    an internal command.
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    -------------------------------------------------------------------------*/
    ERR("OVHD commanded to sleep from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof( hdrovhd_cmd_type) );
  }

} /* hdrovhd_sleep() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_REPARSE_BC_MSG

DESCRIPTION
  This function commands OVHD to re-parse the BroadcastOverhead message.
  This is necessary when the set of desired flows changes.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_reparse_bc_msg( hdrhai_protocol_name_enum_type sender )
{
  hdrovhd_cmd_type hdrovhd_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the ovhd_cmd_type for the reparse_bc_msg command
  ---------------------------------------------------------------------------*/
  hdrovhd_cmd.name = HDROVHD_REPARSE_BC_MSG_CMD;
  hdrovhd_cmd.sender = sender;

  if (rex_self() == HDROVHD_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    Currently only HDRBCCP should issue this command
    -------------------------------------------------------------------------*/
    hdrovhd_process_cmd( &hdrovhd_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the overhead protocol
    -------------------------------------------------------------------------*/
    hdrmc_queue_cmd( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, &hdrovhd_cmd ,
                     sizeof( hdrovhd_cmd_type) );
  }

} /* hdrovhd_reparse_bc_msg() */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/* <EJECT> */ 


/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_RESET_IND_TO_GIVE

DESCRIPTION
  This function resets the ind_to_give structure

DEPENDENCIES
  None.

PARAMETERS
 ind_to_give_ptr - Pointer to the ind_to_give structure to be reset
                                                
RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
static void hdrovhd_reset_ind_to_give
( 
  hdrovhd_ind_to_give_type *ind_to_give_ptr 
)
{
  ind_to_give_ptr->need_to_give_ap_ind = FALSE;
  ind_to_give_ptr->need_to_give_ind = FALSE;
}   

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_BC_INFO_UPDATED

DESCRIPTION
  This function pushes the most current BroadcastOverhead message information
  to the BCMAC and BCOVHD modules, then calls BCCP to update its flow table
  and determine if we are on the best BC frequency.

  This function is called when
  a) The current BroadcastOverhead message changes
  b) The QuickConfig message causes a change of the current cache entry (e.g. 
     idle handoff) and an up-to-date SectorParameters message is in the cache.
  c) A new SectorParameters message is received that causes an update to the
     cache.
  d) The set of requested flows changes.

  Note that if any of these events causes a channel change, then nothing is 
  done until after we change channels at which time one of these events will
  occur again.
     

DEPENDENCIES

PARAMETERS
  bc_entry_ptr - Pointer to updated BC info
  sp_msg_ptr   - Pointer to current SectorParameters message
  current_link_ptr - Current channel and pilot

RETURN VALUE
  TRUE if a channel change is necessary as a result of the new BC info
  FALSE otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean hdrovhd_bc_info_updated
(
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
  hdrovhd_bc_entry_type     *bc_entry_ptr,
  hdrovhd_sp_msg_type       *sp_msg_ptr,
  const sys_link_type       *current_link_ptr
)
{
  hdrerrno_enum_type err_number; /* Was the BC message unpacked successfully? */
  hdrbcovhd_bc_msg_head_type *msg_head_ptr; /* Convenience pointer */
  int dfi; /* desired_flow_index - Index of Flow Record in bc_msg */
  boolean need_to_tune = FALSE; /* If we need to change channels */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  ASSERT( sp_msg_ptr != NULL );
  ASSERT( bc_entry_ptr != NULL );

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /*  FEATURE_HDR_BCMCS_2p0*/

  if (bc_entry_ptr->state == HDROVHD_BC_NOT_PARSED)
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_bc_info_updated");
    hdrovhd.cache_changed = TRUE;

    err_number = (hdrerrno_enum_type)
            hdrbcovhd_unpack_ovhd_msg( bc_entry_ptr->bc_item_ptr,
                                       &bc_entry_ptr->bc_msg );

    if (err_number == E_SUCCESS)
    {
      /* Ensure the SectorSignature in the BroadcastOverhead message
       * matches the one in the current SP message.  Otherwise the 
       * soft combine info is not valid.
       */
      if (bc_entry_ptr->bc_msg.head.sector_signature != sp_msg_ptr->sector_sig)
      {
        err_number = E_NOT_ALLOWED;
      }
    }

    if (err_number == E_SUCCESS)
    {
      /* Unpack desired flow information for each cache entry to avoid
       * excessive unpacking on each active pilot change.
       */
      for (dfi = 0; dfi < bc_entry_ptr->bc_msg.num_desired_flows; dfi++)
      { 
        err_number = hdrbcovhd_unpack_soft_combine_info( 
                  bc_entry_ptr->bc_item_ptr, 
                  sp_msg_ptr,
                  bc_entry_ptr->bc_msg.head.neighbor_count,
                  &bc_entry_ptr->bc_msg.desired_flow[dfi].soft_comb_pn_info,
                  bc_entry_ptr->bc_msg.desired_flow[dfi].soft_combine_offset );

        if (err_number != E_SUCCESS)
        {
          break;
        }
      }
    }

    if (err_number == E_SUCCESS)
    {
      bc_entry_ptr->state = HDROVHD_BC_CURRENT;
    }
    else
    {
      bc_entry_ptr->state = HDROVHD_BC_INVALID;
      dsm_free_packet( &bc_entry_ptr->bc_item_ptr );
      bc_entry_ptr->bc_item_ptr = NULL;

      if (err_number == E_DATA_INVALID)
      {
        /* Some values in the BroadcastOverhead message were not within their
         * specified range.
         */
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "BC msg field outside valid range" );
      }
      else if (err_number == E_NOT_SUPPORTED)
      {
        /* Some field value required functionality not currently supported. */
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "BC msg field not supported" );
      }
      else if (err_number == E_NOT_ALLOWED)
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "BC msg sector signature mismatch" );
      }
      else
      {
        HDR_MSG_PROT_1 ( MSG_LEGACY_ERROR, "BC msg field extraction error %d",
                         err_number );
      }
    }
  } /* end if HDROVHD_BC_NOT_PARSED */

  if (bc_entry_ptr->state == HDROVHD_BC_CURRENT)
  {
    msg_head_ptr = &bc_entry_ptr->bc_msg.head;
    HDR_MSG_PROT_4( MSG_LEGACY_MED, 
                    "BC. BSig:%d SSig:%d Framing:%d FlowCount:%d",
                    msg_head_ptr->broadcast_overhead_signature,
                    msg_head_ptr->sector_signature,
                    msg_head_ptr->framing_type,
                    msg_head_ptr->flow_count );

#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/

#ifdef FEATURE_HDR_BCMCS
    /* If we handoff to a new sector, don't get bcovhd, but handback before
       the end of supervision we need to clear the bc supervision timer
       (bc will already be current in this case) */
    if (rex_get_timer( &hdrovhd.bc_supervision_timer ) != 0)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,"Clear BC supervision timer");
      /* We have a valid message, so stop the supervision timer */
      (void) rex_clr_timer( &hdrovhd.bc_supervision_timer );
    }
#endif /* FEATURE_HDR_BCMCS */

    if ((bc_entry_ptr->bc_item_ptr != NULL) && (bc_entry_ptr->bc_item_ptr->dup_ptr != NULL))
    {
      HDR_MSG_PROT_3 (MSG_LEGACY_ERROR, "DSM3862 BC info updated; ref:%d item:%x dup:%x", bc_entry_ptr->bc_item_ptr->dup_ptr->references, bc_entry_ptr->bc_item_ptr, bc_entry_ptr->bc_item_ptr->dup_ptr);
    }

    if ((bc_entry_ptr->bc_item_ptr == NULL) || 
        (bc_entry_ptr->bc_item_ptr->dup_ptr == NULL) || 
        (bc_entry_ptr->bc_item_ptr->dup_ptr->references == 0))
    {
      bc_entry_ptr->state = HDROVHD_BC_INVALID;
      ERR_FATAL("DSM3862 reproduced!",0,0,0);
    }
  } /* end if HDROVHD_BC_CURRENT */

  if (bc_entry_ptr->state == HDROVHD_BC_CURRENT)
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_bc_info_updated");
    hdrovhd.cache_changed = TRUE;

    hdrbcovhd_update_ovhd_info( bc_entry_ptr->bc_item_ptr,
                                &bc_entry_ptr->bc_msg );

    /* Update the Broadcast MAC overhead.  It must be updated before
       BCCP processes its new ovhd info because BCCP may be ready to 
       enable a new flow through bc mac when the ovhd is updated.
       By updating the bcmac first, we make sure that the correct ovhd info
       is used when enabling the flow. */
    hdrbcmac_update_ovhd_info( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 
                               current_link_ptr->pilot_pn );

    /* Command BCMCS to update its requested flow list based on the latest
       bc ovhd.        
       BCCP returns true if: 
       - the AT finds a better bc channel to monitor after updating its 
         requested flow list (channel change needed)
       - the AT's reference flow becomes NULL.  The AT may or may not need
         to tune depending on the SP hash info.  This will be determined
         in hdrovhd_process_channel_change().  

     */
    if ( hdrbccp_process_bc_ovhd_updated( current_link_ptr,
                                          hdrovhd.is_first_after_acquisition ) )
    {
      need_to_tune = TRUE;
    }
  } /* endif HDROVHD_BC_CURRENT */

  return (need_to_tune);
} /* hdrovhd_bc_info_updated() */
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_BCMCS
/* <EJECT> */ 
/*==============================================================================
FUNCTION HDROVHD_PROCESS_REPARSE_BC_MSG_CMD

DESCRIPTION
  Updates the currently stored version of the BroadcstOverhead message.
  
DEPENDENCIES
  
PARAMETERS

RETURN VALUE
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_process_reparse_bc_msg_cmd( void )
{
  int i; /* Loop counter */
  boolean bc_info_updated = FALSE; /* Flag if current bc_info changes */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; i < HDROVHD_MAX_CACHE_RECORDS; i++ )
  {
    if ((hdrovhd.cache[i].cache_entry_state == HDROVHD_ENTRY_VALID) &&
        (hdrovhd.cache[i].bc_entry.state == HDROVHD_BC_CURRENT))
    {
      hdrovhd.cache[i].bc_entry.state = HDROVHD_BC_NOT_PARSED;
      if ((&(hdrovhd.cache[i])) == hdrovhd.curr_cache_entry_ptr)
      {
        /* If we changed the current cache entry from CURRENT to NOT_PARSED
         * then treat this the same as switching to a new cache entry
         */
        bc_info_updated = TRUE;
      }
    }
  }

  if (bc_info_updated)
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_bc_info_updated");
    hdrovhd.cache_changed = TRUE;
  }

  /* Make sure that ovhd is active and updated before starting bcmcs. */
  if ( (hdrovhd.info_state == HDROVHD_INFO_UPDATED) &&
       (bc_info_updated) && 
       (hdrovhds_get_state() != HDROVHDS_INACTIVE) )
  {
    if ( hdrovhd_bc_info_updated(
#ifdef FEATURE_HDR_BCMCS_2p0
                                 #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
                                 &hdrovhd.curr_cache_entry_ptr->bc_entry,
                                 &hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg,
                                 &hdrovhd.qc_info.link )
       )
    {
      (void) hdrovhd_process_channel_change(hdrovhd.curr_cache_entry_ptr);
    }
  }
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

} /* hdrovhd_process_reparse_bc_msg_cmd() */
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_BC_MSG

DESCRIPTION
  This function processes a received BroadcastOverhead message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr -  Pointer to the DSM item chain holding the message

RETURN VALUE
  TRUE if it is OK to free the DSM item chain holding the message
  FALSE if we have cached this message and are holding the item for a while.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL boolean hdrovhd_process_bc_msg
(
  dsm_item_type *item_ptr
)
{
  sys_link_type bc_link;                     /* Link message was received on */
  hdrsrch_link_id_type  link_id;              /* Link ID of received message */
  hdrerrno_enum_type err_number = E_SUCCESS;        /* Result of unpacking bc msg */
  uint16 bc_overhead_sig;          /* BroadcastOverheadSignature from bc msg */
  uint16 bc_sector_sig;                  /* SectorSignature from bc / sp msg */
  hdrovhd_cache_entry_type *curr_cache_entry_ptr;         /* local cache ptr */
  hdrovhd_bc_entry_type *bc_entry_ptr;  /* Where to unpacke the message into */
  boolean free_dsm_item = TRUE;        /* Should we free the message buffer? */
  uint16 bc_msg_length;       /* Number of bits in the received overhead msg */
  uint8 multiplex_cnt = 0;        /* Ignore a msg with multiplex_cnt idx = 3 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*-------------------------------------------------------------------------
  If Overhead protocol is not in ProcessAllMsgs state, do not process message
  -------------------------------------------------------------------------*/
  if( hdrovhds_get_state() != HDROVHDS_PROCESS_ALL_MSGS )
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "BroadcastOverhead Message Ignored (state %d)", hdrovhds_get_state() );
    return free_dsm_item;
  }

  /*-------------------------------------------------------------------------
  Only process overhead messages received on the same link as the most recent
  QuickConfig message.  This avoids race conditions during idle handoff.
  -------------------------------------------------------------------------*/
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  hdrlmac_get_link_info( link_id , &bc_link );

  if( ( !hdrovhd.qc_info.initialized ) ||
      ( bc_link.pilot_pn != hdrovhd.qc_info.link.pilot_pn ) ||
      !SYS_CHAN_CMP_EQUAL(bc_link.channel,hdrovhd.qc_info.link.channel) )
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
    HDR_MSG_PROT_6( MSG_LEGACY_HIGH, 
      "BroadcastOverhead Message Ignored (link %d:%d:%d, not %d:%d:%d)",
      bc_link.channel.band, bc_link.channel.chan_num, bc_link.pilot_pn, 
      hdrovhd.qc_info.link.channel.band, hdrovhd.qc_info.link.channel.chan_num,
      hdrovhd.qc_info.link.pilot_pn );
    return free_dsm_item;
  }

  bc_msg_length = dsm_length_packet( item_ptr );
  if ( bc_msg_length > HDROVHD_MAX_BC_MSG_LENGTH )
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                    "BcastOvhd Msg Exceeds allowed bytes =%d",
                    bc_msg_length );
    return free_dsm_item;
  }

  /*---------------------------------------------------------------------------
  Take a peek at the MultiplexCount field. 
  MultipleCnt Idx = 3 is not supported, ignore the message.
  ---------------------------------------------------------------------------*/
  if ( HDROVHD_BC_MULTIPLEX_CNT( item_ptr, &multiplex_cnt ) != E_SUCCESS )
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Could not extract MultiplexCount field - Ignore BOM");
    return free_dsm_item;
  }
  else if (multiplex_cnt > 2)
  {     
    HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                   "BcastOvhd Message Ignored, MultiplexCnt %d not supported.",
                   multiplex_cnt);
    return free_dsm_item;
  }

  if ((curr_cache_entry_ptr != NULL) &&
      (curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID))
  {
    /*-------------------------------------------------------------------------
    Take a peek at the SectorSignature field. 
    If it doesn't match the sector signature of the current SP message, ignore.
    -------------------------------------------------------------------------*/
    if ( HDROVHD_BC_SECTOR_SIGNATURE( item_ptr, &bc_sector_sig ) != E_SUCCESS )
    {
      HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
                   "Could not extract SectorSignature field - Ignore BOM");
      return free_dsm_item;
    }
    else if (bc_sector_sig != curr_cache_entry_ptr->sp_entry.sp_msg.sector_sig)
    {     
      HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Ignore BOM: bc sector sig %d != SP sector sig %d",
                     bc_sector_sig, 
                     curr_cache_entry_ptr->sp_entry.sp_msg.sector_sig);
      return free_dsm_item;
    }

    if ( !SYS_CHAN_CMP_EQUAL(bc_link.channel,curr_cache_entry_ptr->rx_link.channel) ||
         (bc_link.pilot_pn != curr_cache_entry_ptr->rx_link.pilot_pn) )
    {
      HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
      HDR_MSG_PROT_6( MSG_LEGACY_HIGH, 
        "BroadcastOverhead Message Ignored (link %d:%d:%d, cache entry %d:%d:%d)",
        bc_link.channel.band, bc_link.channel.chan_num, bc_link.pilot_pn, 
        curr_cache_entry_ptr->rx_link.channel.band, 
        curr_cache_entry_ptr->rx_link.channel.chan_num,
        curr_cache_entry_ptr->rx_link.pilot_pn );
      return free_dsm_item;
    }

    /*-------------------------------------------------------------------------
    If the BroadcastOverhead message is already current and the BroadcastOvhd
    Signature hasn't changed,  there is no need to process the message.
    -------------------------------------------------------------------------*/
    if (curr_cache_entry_ptr->bc_entry.state == HDROVHD_BC_CURRENT)
    {
      /*-----------------------------------------------------------------------
      If received BroadcastOverheadSignature does not match the one in the
      cache.  We need to parse and store the new one.
      -----------------------------------------------------------------------*/
      err_number = HDROVHD_BC_OVERHEAD_SIGNATURE( item_ptr, &bc_overhead_sig );
  
      if (err_number != E_SUCCESS)
      {
        HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
        ERR("Discarding BC msg. Error unpacking overhead sig (%d)",err_number ,0 ,0);
        return free_dsm_item;
      }
  
      if ( bc_overhead_sig == curr_cache_entry_ptr->
             bc_entry.bc_msg.head.broadcast_overhead_signature )
      {
#ifdef FEATURE_HDR_BCMCS_2p0
        #error code not present
#endif /* FEATURE_HDR_BCMCS */
        HDRTRACE_OVHD_MSG_IGNORED_STATE(HDROVHD_BC_MSG_ID, hdrovhds_get_state());
        HDR_MSG_PROT( MSG_LEGACY_LOW, "BC msg ignored: already stored" );
        return free_dsm_item;
      }

      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "BroadcastOverhead message received (%d) %d bits", 
                      bc_overhead_sig, bc_msg_length );
    }
  }

  /*---------------------------------------------------------------------------
  Store BC entry in the cache.
  ---------------------------------------------------------------------------*/
  if ((curr_cache_entry_ptr != NULL) &&
      (curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID))
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_bc_info_updated");
    hdrovhd.cache_changed = TRUE;
    /*-------------------------------------------------------------------------
     Unlike the other messages in the cache, the BC message is only copied
     from the cache by one module, hdrbcovhd, and this module runs in the
     same thread as hdrovhd, so there is no chance for the BC message entry
     to be in-use at the time we are processing a newly received BC message.
    -------------------------------------------------------------------------*/
    bc_entry_ptr = &curr_cache_entry_ptr->bc_entry;

    dsm_free_packet( &bc_entry_ptr->bc_item_ptr );
    bc_entry_ptr->bc_item_ptr = item_ptr;
    bc_entry_ptr->state = HDROVHD_BC_NOT_PARSED;
    free_dsm_item = FALSE;

    if (hdrovhd_bc_info_updated( 
#ifdef FEATURE_HDR_BCMCS_2p0
                                 #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
                                 bc_entry_ptr,
                                 &curr_cache_entry_ptr->sp_entry.sp_msg,
                                 &bc_link ))
    {
      (void) hdrovhd_process_channel_change( curr_cache_entry_ptr );
    }
  }

  return free_dsm_item;

} /* hdrovhd_process_bc_msg() */
#endif /* FEATURE_HDR_BCMCS */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_CHANGE_TO_WAIT_FOR_LINK_STATE

DESCRIPTION
  This function is a wrapper to hdrovhds call to change OVHD state to wait for
  link state. 

DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
LOCAL void hdrovhd_change_to_wait_for_link_state(void)
{
  if (hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL))
  {
    /* Wait for link state is only used during idle handoff and idle frequency
     * change to make sure messages still coming out of the decoder from the 
     * old link (pilot PN, channel) are not processed as if they came from the
     * new link.  The overhead info state isn't important during the connection
     * setup, and will be updated correctly after the connection setup succeeds
     * or fails.  So there is no need to enter wait for link when a TC is 
     * already assigned. */
  }
  else
  {
    hdrovhds_change_state( HDROVHDS_WAIT_FOR_LINK );
  }
} /* hdrovhd_change_to_wait_for_link_state() */

/* <EJECT> */

/*=============================================================================
FUNCTION HDROVHD_PROCESS_AN_REDIRECTED

DESCRIPTION
  This function processes AN redirected command. 

DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
LOCAL void hdrovhd_process_an_redirected(void)
{

  switch( sd_ss_ind_hdr_opr_redir( NULL,
                                   NULL,
                                   SD_SS_REDIR_HDR_QC,
                                   NULL ) )
  {
    case SD_SS_ACT_CONTINUE:
      break;

    case SD_SS_ACT_ACQ_HDR:
      /*-------------------------------------------------------------------
      Change HDR system. Update parameters & give indication.
      Note that this will overwrite a previously filled OVHD_UPDATED
      indication in the data structure
      -------------------------------------------------------------------*/
      hdrovhd.is_first_after_acquisition = FALSE;

      hdrind_give_ind( HDRIND_OVHD_CHANGE_HDR_SYSTEMS, NULL );
      break;

    default :
      /*-------------------------------------------------------------------
      Exit HDR system. Update parameters & give indication.
      Note that this will overwrite a previously filled OVHD_UPDATED
      indication in the data structure
      -------------------------------------------------------------------*/
      hdrovhd.is_first_after_acquisition = FALSE;

      hdrind_give_ind( HDRIND_OVHD_EXIT_HDR_MODE, NULL );
      break;
  }

} /* hdrovhd_process_an_redirected() */

/* <EJECT> */

/*=============================================================================
FUNCTION HDROVHD_QP_IS_OK_TO_PROCESS

DESCRIPTION
  This function checks whether the received QP message can be processed. 
  In non-idle handoff case if the command is ignored AT will stay awake 
  to process the QC message. In idle handoff case if the command is ignored
  overhead updated flag will be set to not updated which will force same
  behavior as the case when QP message is not received.

DEPENDENCIES
  None

PARAMETERS
  qp_link_ptr  - Link on which the QP msg was received.

RETURN VALUE
  TRUE  - If it is okay to continue processing configuration change cmd.
  FALSE - Otherwise

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrovhd_qp_is_ok_to_process
(
  sys_link_type *qp_link_ptr
)
{
  boolean ret_val; /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret_val = FALSE;

  /* If overhead is not updated neither short or longterm cache is valid,
     so just return. AT waits for QC message. */
  if (hdrovhd.info_state == HDROVHD_INFO_NOT_UPDATED)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Overhead protocol not in valid state to process QP" );
    return ret_val;
  }

  /*---------------------------------------------------------------------------
  Msgs are processed in the Process All msg state. If protocol is inactive or
  sleep or if freq change or access HO is in progress, discard the rcvd msg.
  If in wait for link state, check if the QP msg is rcvd on the expected link.
  If so change state to process all msg & continue processing.
  ---------------------------------------------------------------------------*/
  switch( hdrovhds_get_state() )
  {
    case HDROVHDS_INACTIVE:
    case HDROVHDS_SLEEP:
    case HDROVHDS_FREQ_CHANGE_IN_PROG:
    case HDROVHDS_ACC_HO_IN_PROG:
      /*-----------------------------------------------------------------------
      Discard message. Stop processing it.
      -----------------------------------------------------------------------*/
      HDRTRACE_OVHD_MSG_IGNORED_STATE( HDROVHD_QC_MSG_ID,
                                        hdrovhds_get_state() );
      ret_val = FALSE;
      break;

    case HDROVHDS_PROCESS_ALL_MSGS:
      /*-----------------------------------------------------------------------
      Continue with processing of message.
      -----------------------------------------------------------------------*/
      ret_val = TRUE;
      break;

    case HDROVHDS_WAIT_FOR_LINK:
      /*-----------------------------------------------------------------------
      Check if expected QP msg rcvd on the expected link
      -----------------------------------------------------------------------*/
      if((qp_link_ptr->pilot_pn != hdrovhd.expected_link.pilot_pn)||
         !SYS_CHAN_CMP_EQUAL( qp_link_ptr->channel,
                              hdrovhd.expected_link.channel))
      {
        /*---------------------------------------------------------------------
        Discard message. Stop processing it.
        ---------------------------------------------------------------------*/
        HDRTRACE_OVHD_IGNORED_MSG_UNEXPECTED_LINK(HDROVHD_QC_MSG_ID,
        hdrovhd.expected_link.channel.chan_num,
        hdrovhd.expected_link.pilot_pn,
        qp_link_ptr->channel.chan_num,
        qp_link_ptr->pilot_pn );

        ret_val = FALSE;

        if ( hdrovhd.is_first_qp_after_idle_ho )
        {
          hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
          hdrovhd_set_ap_not_updated();

          hdrovhd.is_first_qp_after_idle_ho = FALSE;
        }
      }
      else
      {
        /*---------------------------------------------------------------------
        Change state to process all msg & Continue with processing of message.
        ---------------------------------------------------------------------*/
        hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
        ret_val = TRUE;
      }
      break;

    default:
      /*-----------------------------------------------------------------------
      Discard message. Stop processing it.
      -----------------------------------------------------------------------*/
      ERR_FATAL("hdrovhds_get_state returned unknown value", 0, 0, 0 );
  }

  return ret_val;
} /* hdrovhd_qp_is_ok_to_process() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_LONGTERM_CACHE_ENTRY_IS_VALID

DESCRIPTION
  This function checks whether long term cache entry is still valid.

DEPENDENCIES
  None.

PARAMETERS
  cache_entry_ptr - cache entry that needs to be checked to find whether it 
                    is still valid.
  cache_key_type - Key that needs used to search the cache.

RETURN VALUE
  True - if the entry is valid, False otherwise.

SIDE EFFECTS
  None

=============================================================================*/
static boolean hdrovhd_longterm_cache_entry_is_valid 
( 
  hdrovhd_cache_entry_type *cache_entry_ptr, 
  hdrovhd_cache_key_type cache_key_type
)
{
  uint16 lt_cache_period = 0;
  uint64 lt_cache_period_in_ms = 0;
  qword current_timestamp = {0};
  uint64 time_elapsed = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cache_entry_ptr == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "CACHE: Ovhd info not found in short term cache");
    return FALSE;
  }

  if ( ( !cache_entry_ptr->qc_is_valid ) && 
       ( cache_key_type == HDROVHD_CACHE_KEY_QP_INFO ) )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "CACHE: QC entry in the ovhd cache is not valid");
    return FALSE;
  }

  /* Get the current timestamp value */
  (void) time_get (current_timestamp);

  /* Calculate the time elapsed since the overhead message was last updated */
  time_elapsed = hdrutil_calc_duration ( cache_entry_ptr->ts_explicit, 
                                         current_timestamp ); 

  /* Get the current value for long term overhead cache period */
  lt_cache_period = hdrovhd_get_cache_period_attrib(
                        HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID,
                        HDRSCP_DEFAULT_OVHD );

  /* Convert the long term overhead cache period in to ms */
  lt_cache_period_in_ms = (uint64) (lt_cache_period * HDROVHD_CACHE_PERIOD_IN_MS);

  /* Check whether long term cache timer has expired */
  if (time_elapsed <= lt_cache_period_in_ms)
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  
      "CACHE: Ovhd info still valid time_elapsed=%d, lt_cache_period=%d",
                  time_elapsed, lt_cache_period_in_ms );
    return TRUE;
  }
  else
  {
    /* long term cache timer has expired, clear cache entry */
    hdrovhd_clear_cache_entry(cache_entry_ptr );
    hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
    hdrovhd_set_ap_not_updated();

    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  
      "CACHE: Ovhd info not valid time_elapsed=%d, lt_cache_period=%d",
                  time_elapsed, lt_cache_period_in_ms );
    return FALSE;
  }
} /* hdrovhd_longterm_cache_entry_is_valid() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_SHORTTERM_CACHE_ENTRY_IS_VALID

DESCRIPTION
  This function checks whether short term cache entry is still valid.

DEPENDENCIES
  None.

PARAMETERS
  cache_entry_ptr - cache entry that needs to be checked to find whether it 
                    is still valid.
  cache_key_type - Key that needs used to search the cache.

RETURN VALUE
  True - if the entry is valid, False otherwise.

SIDE EFFECTS
  None

=============================================================================*/
static boolean hdrovhd_shortterm_cache_entry_is_valid 
( 
  hdrovhd_cache_entry_type *cache_entry_ptr, 
  hdrovhd_cache_key_type cache_key_type
)
{
  uint16 st_cache_period = 0;
  uint64 st_cache_period_in_ms = 0;
  qword current_timestamp = {0};
  uint64 time_elapsed = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cache_entry_ptr == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "CACHE: Ovhd info not found in short term cache");
    return FALSE;
  }

  /* Always ensure long term is valid before checking short term timer */
  if ( !hdrovhd_longterm_cache_entry_is_valid(cache_entry_ptr, cache_key_type) )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "CACHE: Long Term timer expired");
    return FALSE;
  }

  /* qc_is_valid bit is set to FALSE when the phone is in Connected state. 
   * It is used as a flag to indicate that the ShortTermOverheadCache is invalid
   * and the phone can not use QP to search for cached QC message. 
   * The phone have to wait for QC in this scenario. However, the phone can still
   * use QC to search cached SP message.
   */
  if ( ( !cache_entry_ptr->qc_is_valid ) && 
       ( cache_key_type == HDROVHD_CACHE_KEY_QP_INFO ) )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "CACHE: QC entry in the ovhd cache is not valid");
    return FALSE;
  }

  /* Get the current timestamp value */
  (void) time_get (current_timestamp);

  /* Calculate the time elapsed since the overhead message was last updated */
  time_elapsed = hdrutil_calc_duration ( cache_entry_ptr->ts_implicit, 
                                         current_timestamp ); 

  /* Get the current value for short term overhead cache period */
  st_cache_period = hdrovhd_get_cache_period_attrib(
                        HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID,
                        HDRSCP_DEFAULT_OVHD );

  /* Convert the short term overhead cache period in to ms */
  st_cache_period_in_ms = (uint64) (st_cache_period * HDROVHD_CACHE_PERIOD_IN_MS);

  /* Check whether short term cache timer has expired */
  if (time_elapsed <= st_cache_period_in_ms)
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  
      "CACHE: Ovhd info still valid time_elapsed=%d, st_cache_period=%d",
                  time_elapsed, st_cache_period_in_ms );
    return TRUE;
  }
  else
  {
    /* short term cache timer has expired, clear cache entry */
    hdrovhd_clear_cache_entry(cache_entry_ptr );

    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,  
      "CACHE: Ovhd info not valid time_elapsed=%d, st_cache_period=%d",
                  time_elapsed, st_cache_period_in_ms );
    return FALSE;
  }
} /* hdrovhd_shortterm_cache_entry_is_valid() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_FIND_CACHED_MSG

DESCRIPTION
  This function is used to find a cached overhead information. Searches the OVHD 
  cache to see whether the overhead information  (QC/SP/AP messages) from a 
  particular sector has been previously stored. 

  This function can use either the PN and Channel combination or Sector ID and 
  Channel combination as the key to search the cache. Former is used when a 
  QuickPage message is received and latter is used when QuickConfig message is
  received.

DEPENDENCIES
  None

PARAMETERS
  cache_key_info - Key values to use to search the cache entries.
  cache_key_type - Key that needs used to search the cache.

RETURN VALUE
  NULL if valid matching Sector Parameters message is not found.
  Otherwise returns pointer to node containing the SP msg.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL hdrovhd_cache_entry_type *hdrovhd_find_cached_msg
(
  hdrovhd_cache_key_info_type cache_key_info,
  hdrovhd_cache_key_type cache_key_type
)
{
  int cnt;                                           /* Index counter */
  hdrovhd_cache_entry_type *found_cached_msg_ptr = NULL;
                                                     /* Cache entry Pointer */
  boolean cache_entry_matched = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(cnt=0; cnt < HDROVHD_MAX_CACHE_RECORDS; cnt++)
  {
    if (hdrovhd.cache[cnt].cache_entry_state == HDROVHD_ENTRY_INVALID)
    {
      continue;
    }

    switch (cache_key_type)
    {
      case HDROVHD_CACHE_KEY_QP_INFO:
        if ((cache_key_info.qp_info.link_info.pilot_pn == 
             hdrovhd.cache[cnt].rx_link.pilot_pn) &&
            (SYS_CHAN_CMP_EQUAL( cache_key_info.qp_info.link_info.channel , 
                                 hdrovhd.cache[cnt].rx_link.channel )))
        {
          cache_entry_matched = TRUE;
        }
        break;

      case HDROVHD_CACHE_KEY_QC_INFO:
        if ((cache_key_info.qc_info.link_info.pilot_pn == 
             hdrovhd.cache[cnt].rx_link.pilot_pn) &&
            (SYS_CHAN_CMP_EQUAL( cache_key_info.qc_info.link_info.channel , 
                                 hdrovhd.cache[cnt].rx_link.channel )) &&
            (cache_key_info.qc_info.color_code == 
             hdrovhd.cache[cnt].qc_entry.color_code) &&
            (cache_key_info.qc_info.sector_id_24 == 
             hdrovhd.cache[cnt].qc_entry.sector_id_24) &&
            (cache_key_info.qc_info.sector_signature == 
             hdrovhd.cache[cnt].qc_entry.sector_signature))
        {
          cache_entry_matched = TRUE;
        }
        break;

      case HDROVHD_CACHE_KEY_SP_INFO:
        if ((SYS_CHAN_CMP_EQUAL( cache_key_info.sp_info.channel , 
                                 hdrovhd.cache[cnt].rx_link.channel )) &&
            hdraddr_is_equal( hdrovhd.cache[cnt].sp_entry.sp_msg.sector_id, 
                                   cache_key_info.sp_info.sp_sector_id_128 )&&
            (cache_key_info.sp_info.sector_signature == 
             hdrovhd.cache[cnt].sp_entry.sp_msg.sector_sig))
        {
          cache_entry_matched = TRUE;
        }
        break;

      default:
        cache_entry_matched = FALSE;
        break;
    }

    if (cache_entry_matched)
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "CACHE: match with key %d", cache_key_type);
      if(hdrovhd.cache[cnt].cache_entry_state == HDROVHD_ENTRY_VALID_WHEN_CONN)
      {
        /*---------------------------------------------------------------------
        If a cache entry is marked as valid when only connected, then check
        the state of the protocols. If it is connected, then return back this
        entry. If not, then mark this entry as invalid and continue searching
        through the cache.
        ---------------------------------------------------------------------*/
        if( hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) )
        {
          found_cached_msg_ptr = &hdrovhd.cache[cnt] ;
          break;
        }  
        else
        {
#ifdef FEATURE_HDR_BCMCS
          /* If a BroadcastOverhead message was stored in this cache entry
           * while in traffic on a non-hash and non-broadcast frequency,
           * then it is no longer needed now that we are not connected.
           */
          dsm_free_packet( &(hdrovhd.cache[cnt].bc_entry.bc_item_ptr) );
#endif /* FEATURE_HDR_BCMCS */
          hdrovhd_clear_cache_entry(&hdrovhd.cache[cnt]);
          cache_entry_matched = FALSE;
        }
      }
      else
      {
        /*---------------------------------------------------------------------
        A SP msg will not be cached if it causes a hash to a different channel
        than it was received on. Thus, for a cached msg these should match.
        ---------------------------------------------------------------------*/
        /* This was changed because tdtgen didn't like the syntax */
        if (SYS_CHAN_CMP_EQUAL(
                hdrovhd.cache[cnt].rx_link.channel,
                hdrovhd.cache[cnt].sp_entry.sp_msg.hash_channel ))
        {
           ASSERT(TRUE);
        }
        found_cached_msg_ptr = &hdrovhd.cache[cnt] ;
        break;
      }
    }
  }

  if ((found_cached_msg_ptr != NULL) && 
      (cache_key_type == HDROVHD_CACHE_KEY_QP_INFO ||
        cache_key_type == HDROVHD_CACHE_KEY_QC_INFO))
  {
    /* Found entry, make sure it's still valid */
    if ( !hdrovhd_shortterm_cache_entry_is_valid(found_cached_msg_ptr, cache_key_type) )
    {
      found_cached_msg_ptr = NULL;
    }
  }

  if (found_cached_msg_ptr != NULL)
  {
    HDRTRACE_OVHD_FIND_CACHED_MSG( HDROVHD_CACHE_HIT );
  }
  else
  {
    HDRTRACE_OVHD_FIND_CACHED_MSG( HDROVHD_CACHE_MISS );
  }

  return found_cached_msg_ptr;
} /* hdrovhd_find_cached_msg() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_FIND_CACHED_MSG_USING_QP_INFO

DESCRIPTION
  This function is used to find a cached overhead information when QuickPage
  message is received. PN and Channel are used to find the cached 
  information.

DEPENDENCIES
  None

PARAMETERS
  link_info - Link info of the handoff destination sector

RETURN VALUE
  NULL if valid matching Sector Parameters message is not found.
  Otherwise returns pointer to node containing the SP msg.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL hdrovhd_cache_entry_type *hdrovhd_find_cached_msg_using_qp_info
(
  sys_link_type link_info
)
{
  hdrovhd_cache_entry_type *found_cached_msg_ptr = NULL;
    /* Cache entry Pointer */
  hdrovhd_cache_key_info_type cache_key_info;
    /* Info to find the cached entry */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &cache_key_info, 0, sizeof(hdrovhd_cache_key_info_type) );

  cache_key_info.qp_info.link_info = link_info;

  found_cached_msg_ptr = hdrovhd_find_cached_msg(
                           cache_key_info,
                           HDROVHD_CACHE_KEY_QP_INFO);

  return found_cached_msg_ptr;
} /* hdrovhd_find_cached_msg_using_qp_info() */

/* <EJECT> */

/*=============================================================================
FUNCTION HDROVHD_PROCESS_CHECK_CONFIG

DESCRIPTION
  This function processes check configuration command. 

DEPENDENCIES
  None.
  
PARAMETERS
  config_change - ConfigurationChange field from QP message, the PN and channel 
                  on which QP was received.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
LOCAL void hdrovhd_process_check_config
( 
  hdrovhd_config_change_cmd_type config_change 
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;
    /* Pointer to Cache entry */
  uint64 lt_cache_period_in_ms = 0;
  uint64 time_elapsed = 0;
  hdrovhd_cache_entry_type *cache_entry_ptr = NULL;
  hdrovhd_ind_to_give_type ind_to_give;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first QP message that is received after idle handoff to 
     a new sector then check for short term cache validity. */
  if (hdrovhd.is_first_qp_after_idle_ho)
  {
    /* Find if there is a entry in the cache (using PN and channel) for the 
       handoff destination */
    cache_entry_ptr = hdrovhd_find_cached_msg_using_qp_info( 
                          config_change.link );

    /* If the configchange in QP message & stored overhead information is 
       same and shortterm cache is valid then process the cached overhead 
       information. Note that we check only for the config change field 
       and not for the longterm cache timer expiry because longterm cache
       timer is always less than shortterm cache timer */
    if ( cache_entry_ptr != NULL &&
         (cache_entry_ptr->config_change == config_change.config_change) )
    {
      /* Initialize the indication data structure. */
      hdrovhd_reset_ind_to_give( &ind_to_give );

      /* Process the cached QC message */
      if ( hdrovhd_process_unpacked_qc_msg( &cache_entry_ptr->qc_entry, 
                                            &ind_to_give ) )
      {
        /*-------------------------------------------------------------------------
         If system determination does not cause to change or exit the current HDR
         system, then store Received fields & link information
        -------------------------------------------------------------------------*/
        hdrovhd_update_curr_qc_info( &cache_entry_ptr->qc_entry, 
                                      config_change.link_id );

        /* qc data valid; reset qc insanity timer */
        (void) rex_set_timer( &hdrovhd.qc_insanity_timer, 
                              HDROVHD_INSANITY_TIMER_MSECS );
      }

      /* Give necessary indications */
      hdrovhd_give_ind( &ind_to_give );

	  /* Force the sleep flags to TRUE so that AT can go to sleep */
      hdridle_sleep_based_on_quickpage( TRUE );
	  
      /* Log the overhead information */
      if ( hdrovhd.cache_changed )
      {
        HDR_MSG_PROT(MSG_LEGACY_LOW, 
                     "Idle handoff and shortterm cache is valid");
        hdrovhd_log_cache_summary();
      }
    }
    else
    {
      /* The above conditions failed then mark overhead as not updated and wait
         for QC message */
      hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
      hdrovhd_set_ap_not_updated();
    }

    hdrovhd.is_first_qp_after_idle_ho = FALSE;
  }
  /* If this is the not the first QP message after idle handoff then only check
     for long term cache validity. For checking LT cache validity current cache 
     entry should be valid and stored QC message should be valid. */
  else if( curr_cache_entry_ptr &&
           (curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID) &&
           curr_cache_entry_ptr->qc_is_valid )
  {
    /* If the ConfiguationChange value has not changed since the last time 
       the overhead message was received from this sector and the LongTerm
       Cache has not expired then AT can go to sleep by just reading the 
       QuickPage message, it need wait for QuickConfig message */
    if ( (curr_cache_entry_ptr->config_change == config_change.config_change) &&
         hdrovhd_longterm_cache_entry_is_valid(curr_cache_entry_ptr, 
                                                HDROVHD_CACHE_KEY_QP_INFO) )
    {
      hdridle_sleep_based_on_quickpage( TRUE );
    }
    else
    {
      HDR_MSG_PROT_4( MSG_LEGACY_HIGH,  
        "Ovhd info not valid stored CC=%d, QP CC=%d, time_elapsed=%d, lt_cache_period=%d",
        curr_cache_entry_ptr->config_change, config_change.config_change, time_elapsed, lt_cache_period_in_ms );
    }
  }

} /* hdrovhd_process_check_config() */

/* <EJECT> */

#ifdef FEATURE_LTE_TO_HDR_OH
/*=============================================================================
FUNCTION HDROVHD_PROCESS_TUNNEL_DEACTIVATE

DESCRIPTION
  This function processes tunnel deactivate command

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
LOCAL void hdrovhd_process_tunnel_deactivate
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{
  if( hdrovhds_get_state() != HDROVHDS_TUNNEL_ACTIVE )
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
        ovhd_cmd_ptr->name , ovhd_cmd_ptr->sender, hdrovhds_get_state() );
    return;
  }

  hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
  hdrovhd.qc_info.initialized = FALSE;
  hdrovhd_set_ap_not_updated();
  hdrovhd.last_sd_subnet.is_valid = FALSE;

}
#endif /* FEATURE_LTE_TO_HDR_OH */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_CMD

DESCRIPTION
  This function processes commands given to OVHD.

DEPENDENCIES
  None

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_process_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{

  HDRTRACE_CMD( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, ovhd_cmd_ptr->name,
                 ovhd_cmd_ptr->sender, hdrovhds_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Determine which command has been received.
  ---------------------------------------------------------------------------*/
  switch ( ovhd_cmd_ptr->name )
  {
    case HDROVHD_ACTIVATE_CMD:
      /*-----------------------------------------------------------------------
      Activate overhead protocol command
      -----------------------------------------------------------------------*/
      hdrovhd_process_activate_cmd( ovhd_cmd_ptr );
      break;

    case HDROVHD_RETURN_TO_IDLE_CMD:
      /*-----------------------------------------------------------------------
      Return to idle operation from connected state.
      -----------------------------------------------------------------------*/
      hdrovhd_process_return_to_idle_cmd( ovhd_cmd_ptr );
      break;

    case HDROVHD_DEACTIVATE_CMD:
      /*-----------------------------------------------------------------------
      Deactivate overhead protocol command
      -----------------------------------------------------------------------*/
      hdrovhd_process_deactivate_cmd( ovhd_cmd_ptr );
      break;

    case HDROVHD_WAKEUP_CMD:
      /*-----------------------------------------------------------------------
      WakeUp overhead protocol
      -----------------------------------------------------------------------*/
      hdrovhd_process_wakeup_cmd( ovhd_cmd_ptr );
      break;

    case HDROVHD_SLEEP_CMD:
      /*-----------------------------------------------------------------------
      Put overhead protocol in Sleep state
      -----------------------------------------------------------------------*/
      hdrovhd_process_sleep_cmd( ovhd_cmd_ptr );
      break;

    case HDROVHD_REPARSE_BC_MSG_CMD:
#ifdef FEATURE_HDR_BCMCS
      /*-----------------------------------------------------------------------
      Process Reparse Broadcast Overhead Message 
      -----------------------------------------------------------------------*/
      hdrovhd_process_reparse_bc_msg_cmd();
      /* Per Marie-Pierre's comment, reparse BC does change the cache record,
         however, it doesn't change the fields we log (BC sig) right now.
         so no need to log */
#endif /* FEATURE_HDR_BCMCS */
      break;

    case HDROVHD_AN_REDIRECTED_CMD:
      /*-----------------------------------------------------------------------
      Process AN redirected command
      -----------------------------------------------------------------------*/
      hdrovhd_process_an_redirected();
      break;

    case HDROVHD_CHECK_CONFIG_CMD:
      /*-----------------------------------------------------------------------
      Process check configuration command
      -----------------------------------------------------------------------*/
      hdrovhd_process_check_config( ovhd_cmd_ptr->params.config_change );
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDROVHD_TUNNEL_DEACTIVATE_CMD:
      /*-----------------------------------------------------------------------
      Process tunnel deactivate command
      -----------------------------------------------------------------------*/
      hdrovhd_process_tunnel_deactivate( ovhd_cmd_ptr );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      /*-----------------------------------------------------------------------
      Unrecognized command.
      -----------------------------------------------------------------------*/
      ERR("OVHD: Received Unrecognized command:%d", ovhd_cmd_ptr->name, 0, 0 );
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrovhd_process_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_ACTIVATE_CMD

DESCRIPTION
  This function processes an activate command given to the overhead protocol
  This function is called by ALMP on transition from initialization to idle
  state.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_activate_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{
  if( hdrovhds_get_state() != HDROVHDS_INACTIVE )
  {
    /*-------------------------------------------------------------------------
    If the Protocol has already been activated, do nothing.
    -------------------------------------------------------------------------*/
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
        ovhd_cmd_ptr->name , ovhd_cmd_ptr->sender, hdrovhds_get_state() );
    return;
  }

  /*---------------------------------------------------------------------------
  Change state to Process All Messages. Reset QC info, AP & SP parameters state
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_LTE_TO_HDR_OH  
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) )
  {
    hdrovhds_change_state( HDROVHDS_TUNNEL_ACTIVE );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
  }

  hdrovhd.qc_info.initialized = FALSE;
  hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
  hdrovhd_set_ap_not_updated();
  hdrovhd.last_sd_subnet.is_valid = FALSE;
#ifdef FEATURE_HDR_TO_LTE
  hdrovhd_set_ornl_not_updated();
  hdrovhd.first_splmn_after_acq = TRUE;
  memset( &hdrovhd.splmn_info, 0, sizeof(sd_splmn_info_list_s) );
#endif /* FEATURE_HDR_TO_LTE */

} /* hdrovhd_process_activate_cmd() */


/*=============================================================================

FUNCTION HDROVHD_PROCESS_RETURN_TO_IDLE_CMD

DESCRIPTION
  This function processes a return to idle command given to the overhead 
  protocol.  This function is called by ALMP on transition from connected
  to idle state.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_return_to_idle_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{

  SYS_ARG_NOT_USED(ovhd_cmd_ptr); /* remove this line when ovhd_cmd_ptr 
                                   * is used */

  if( hdrovhds_get_state() != HDROVHDS_PROCESS_ALL_MSGS )
  {
    /*-------------------------------------------------------------------------
    All transitions into HDROVHDS_PROCESS_ALL_MSGS are valid.  Make sure this
    is our initial state when returning to idle.
    -------------------------------------------------------------------------*/
    hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
  }
  else
  {
    /* We still need to reset the supervision timers even if we 
     * are remaining in HDROVHDS_PROCESS_ALL_MSGS state.
     */
    hdrovhd_restart_timers();
  }

  /* We don't want to invalidate the qc_info because then we will not know
   * the color_code of the sector we were on when on the TC and will need
   * to assume that our current subnet is invalid.  This, then, would require
   * us to get a brand new SP message before we would have valid overhead again
   * which in turn could delay our ability to send another ConnectionRequest
   * message.  For the TC->IDLE transition this is OK since we have been
   * receiving the control channel continuously.  Also note that that if we
   * change pilots on this transition, HDRSRCH will indicate an idle handoff
   * to prevent us from accidently starting the access on the wrong pilot.
   */
  hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
  hdrovhd_set_ap_not_updated();
  hdrovhd.last_sd_subnet.is_valid = FALSE;
 #ifdef FEATURE_HDR_TO_LTE
  hdrovhd_set_ornl_not_updated();
 #endif /* FEATURE_HDR_TO_LTE */
} /* hdrovhd_process_return_to_idle_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_DEACTIVATE_CMD

DESCRIPTION
  This function processes a deactivate command given to the overhead protocol

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_deactivate_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{

  if( hdrovhds_get_state() == HDROVHDS_INACTIVE )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is already in active state, do nothing.
    -------------------------------------------------------------------------*/
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
      ovhd_cmd_ptr->name, ovhd_cmd_ptr->sender, hdrovhds_get_state() );
    return;
  }

#ifdef FEATURE_HDR_BCMCS
  /* Once we deactivate OVHD we are sure to re-process the overhead upon
     activation so the supervision timer will get re-started then, if 
     necessary. */
  if (rex_get_timer( &hdrovhd.bc_supervision_timer ) != 0)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Clear BC supervision timer");
    (void) rex_clr_timer( &hdrovhd.bc_supervision_timer );
  }
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
#endif /* FEATURE_HDR_BCMCS */

  hdrovhds_change_state( HDROVHDS_INACTIVE );

} /* hdrovhd_process_deactivate_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_WAKEUP_CMD

DESCRIPTION
  This function processes a wakeup command given to the overhead protocol

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_wakeup_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{

  if( hdrovhds_get_state() != HDROVHDS_SLEEP )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is not in sleep state, this command should not have been
    given. log error.
    -------------------------------------------------------------------------*/
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
      ovhd_cmd_ptr->name , ovhd_cmd_ptr->sender, hdrovhds_get_state() );
    return;
  }

  if( hdrovhd.idle_ho_while_sleep )
  {
    /*-------------------------------------------------------------------------
    Transition to wait for Link state and then reset the boolean flag
    indicating that an idle handoff occured while in sleep. The expected link
    is already set when the idle handoff indication is received.
    -------------------------------------------------------------------------*/
    hdrovhd_change_to_wait_for_link_state();
    hdrovhd.idle_ho_while_sleep = FALSE;
  }
  else
  {
    /*-------------------------------------------------------------------------
    No idle handoff occured while in Sleep state. Thus, transition to
    Process All Msgs state.
    -------------------------------------------------------------------------*/
    hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
  }

} /* hdrovhd_process_wakeup_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_SLEEP_CMD

DESCRIPTION
  This function processes a sleep command given to the overhead protocol

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_sleep_cmd
(
  hdrovhd_cmd_type *ovhd_cmd_ptr
)
{
  hdrovhds_state_enum_type cur_state = hdrovhds_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(ovhd_cmd_ptr); /* remove this line when ovhd_cmd_ptr 
                                   * is used */

  if (( cur_state == HDROVHDS_PROCESS_ALL_MSGS ) ||
      ( cur_state == HDROVHDS_WAIT_FOR_LINK ))
  {
    /*-------------------------------------------------------------------------
    The purpose of WAIT_FOR_LINK state is to prevent overhead info from being
    flagged as current based on overhead messages from the old link.
    It is safe to allow WAIT_FOR_LINK -> SLEEP -> PROCESS_ALL_MSGS because
    we ignore overhead messages in sleep state, and when we wake up from sleep
    we expect to be on the new link.  This transition is needed for
    HDRIDLE_SUSPEND_STATE operation.
    -------------------------------------------------------------------------*/
  }
  else
  {
    ERR("OVHD Sleep Cmd in wrong state", 0, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
  In some valid cases, the AccessParameters supervision timer could still be
  running when returning to sleep state.  Clear it before entering sleep.
  -------------------------------------------------------------------------*/
  (void) rex_clr_timer( &hdrovhd.ap_supervision_timer );

  hdrovhds_change_state( HDROVHDS_SLEEP );

} /* hdrovhd_process_sleep_cmd() */
/*lint +esym(715, ovhd_cmd_ptr) ovhd_cmd_ptr not referenced */

#ifdef FEATURE_HDR_REVC
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_REPARSE_ASET_NEIGHBORS

DESCRIPTION
  This function refills cache_ptr->aset_neighbors array in case
  ASET changes

DEPENDENCIES
  Caller shoud ensure serving sector is part of ASET. 

PARAMETERS
  hdrovhd_nlb_cache_type  - Ptr to NLB cache entry to reparse

RETURN VALUE
  None.

SIDE EFFECTS

=============================================================================*/
LOCAL boolean hdrovhd_reparse_aset_neighbors
(
  hdrovhd_nlb_cache_type* cache_ptr
)
{
  uint8 i;
  boolean updated = FALSE;
  uint8 prev_aset_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cache_ptr != NULL);
  prev_aset_cnt = cache_ptr->aset_cnt;

  cache_ptr->aset_cnt = 0;

  for (i=0; i<cache_ptr->neighbor_cnt &&
            cache_ptr->aset_cnt < HDROVHD_SUBASET_PER_CHAN; i++)
  {
    /* check if neigbhor is on serving chan and in ASET */
    if (cache_ptr->serving_sector.channel.band == 
        cache_ptr->neighbors[i].channel.band &&
        cache_ptr->serving_sector.channel.chan_num ==
        cache_ptr->neighbors[i].channel.chan_num && 
        hdrlmac_pilot_is_in_aset(&cache_ptr->neighbors[i].channel,
                                 cache_ptr->neighbors[i].pilot_pn))
    {
      if (cache_ptr->aset_neighbors[cache_ptr->aset_cnt] != i)
      {
        /* ASET order changed */
        updated = TRUE;
        cache_ptr->aset_neighbors[cache_ptr->aset_cnt] = i;
      }

      HDR_MSG_PROT_4(MSG_LEGACY_MED, "reparse_aset_nbr ch %d pn %d added @ %d/%d",
                     cache_ptr->neighbors[i].channel.chan_num,
                     cache_ptr->neighbors[i].pilot_pn, i, 
                     cache_ptr->aset_cnt);

      cache_ptr->aset_cnt++;
    }
    else
    {
      HDR_MSG_PROT_3(MSG_LEGACY_LOW, "reparse_aset_nbr ch %d pn %d @ %d not valid",
                     cache_ptr->neighbors[i].channel.chan_num,
                     cache_ptr->neighbors[i].pilot_pn, i);
    }
  }

  /*  updated if 1) any entry changed 2) # of entries changed */
  updated |= (prev_aset_cnt != cache_ptr->aset_cnt);

  return updated;
}

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_LMAC_ASET_CHANGED

DESCRIPTION
  This function process the LMAC_ASET_CHANGED indication. Receiving
  this indication means ASET has just been updated. Go thru all cache
  entry and reparse.

DEPENDENCIES
  None

PARAMETERS
  hdrovhd_nlb_cache_type  - Ptr to NLB cache entry to reparse

RETURN VALUE
  None.

SIDE EFFECTS

=============================================================================*/
LOCAL void hdrovhd_process_lmac_aset_changed( void )
{
  uint8 i;
  sys_link_type serving_sector;
  boolean updated = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "proc_lmac_aset_changed");

  for (i=0; i<HDROVHD_NLB_CACHE_SIZE; i++)
  {
    if (hdrovhd.nlb_cache[i].state == HDROVHD_NLB_CACHE_SP_RCVD &&
        hdrlmac_pilot_is_in_aset(&hdrovhd.nlb_cache[i].serving_sector.channel,
                                 hdrovhd.nlb_cache[i].serving_sector.pilot_pn))
    {
      updated |= hdrovhd_reparse_aset_neighbors(&hdrovhd.nlb_cache[i]);
    }
  }

  /* setting chan/pn to 0 to indicate cache update for all sectors */
  serving_sector.channel.chan_num = 0;
  serving_sector.pilot_pn = 0;

  if (updated)
  {
    hdrind_give_ind(HDRIND_OVHD_NLB_CACHE_UPDATED, &serving_sector);
  }
}
#endif /* FEATURE_HDR_REVC */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_IND

DESCRIPTION
  This function processes indications given to OVHD.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication for OVHD to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_process_ind
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  hdrovhds_state_enum_type cur_state = hdrovhds_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, ind_name,
                 cur_state );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Determine which indication has been received.
  ---------------------------------------------------------------------------*/
  switch ( ind_name )
  {
    case HDRIND_LMAC_IDLE_HANDOFF:
      /*-----------------------------------------------------------------------
      Idle Handoff indication from LMAC
      -----------------------------------------------------------------------*/
      hdrovhd_process_idle_ho_ind( ind_data_ptr );
      break;

    case HDRIND_IDLE_CONNECTION_FAILED:
      /*-----------------------------------------------------------------------
      ConnectionFailed Indication
      -----------------------------------------------------------------------*/
      hdrovhd_conn_end_reset_info_state();
      break;

    case HDRIND_CONNECTED_CONNECTION_CLOSED:
      /*-----------------------------------------------------------------------
      ConnectionClosed Indications
      -----------------------------------------------------------------------*/
      hdrovhd_conn_end_reset_info_state();
      break;

    case HDRIND_LMAC_AHO_REQUEST:
      /*-----------------------------------------------------------------------
      AccessHandOff Indication
      -----------------------------------------------------------------------*/
      hdrovhd_process_aho_request_ind();
      break;

    case HDRIND_LMAC_AHO_COMPLETE:
      /*-----------------------------------------------------------------------
      AccessHandOff Complete Indication
      -----------------------------------------------------------------------*/
      hdrovhd_process_aho_complete_ind( ind_data_ptr );
      break;

    case HDRIND_RUP_FREQ_CHANGE_INITIATED:
      /*-----------------------------------------------------------------------
      Frequency Change Initiated Indication
      -----------------------------------------------------------------------*/
      hdrovhd_process_freq_change_initiated_ind();
      break;

    case HDRIND_LMAC_FREQ_CHANGE_COMPLETE:
      /*-----------------------------------------------------------------------
      Frequency Change Complete Indication
      -----------------------------------------------------------------------*/
      hdrovhd_process_freq_change_complete_ind( ind_data_ptr );
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      /*-----------------------------------------------------------------------
      Do the processing needed in case the current session is changed, ie if
      a session configuration is complete, a session is closed, or a previous
      session is opened.
      -----------------------------------------------------------------------*/
      if( ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_NORMAL ||
          ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR )
      {
        hdrovhd_clear_cache();
        if (hdrovhd.cache_changed)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, "HDRIND_SCM_SESSION_CHANGED");
          hdrovhd_log_cache_summary();
        }
      }
      else if( ind_data_ptr->change_reason == HDRHAI_SCHG_PERSONALITY_SWITCH )
      {
        hdramac_reset_attempt( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
        hdrovhd.ap_state = HDROVHD_AP_NOT_UPDATED;
        /* Set last subnet to not valid so that we will report session
         * status to SD after session change even there is no OVHD change
         */
        hdrovhd.last_sd_subnet.is_valid = FALSE;
#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
      }
      else
      {
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
        hdrovhd.ap_state = HDROVHD_AP_NOT_UPDATED;
        /* Set last subnet to not valid so that we will report session
         * status to SD after session change even there is no OVHD change
         */
        hdrovhd.last_sd_subnet.is_valid = FALSE;
#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
      }
      break;

    case HDRIND_ALMP_REACQUIRING:
      /*-----------------------------------------------------------------------
      ALMP is re-acquiring a new system (or the same system)
      -----------------------------------------------------------------------*/
      /* Flag that we have not yet acquired a system. */
      hdrovhd.acq_sid_rxed = FALSE;
      break;

    case HDRIND_RUP_CONNECTION_INITIATED:
      /*-----------------------------------------------------------------------
      Once we get a TCA, we no longer care about what link we are on
      -----------------------------------------------------------------------*/
      if ((cur_state == HDROVHDS_ACC_HO_IN_PROG) ||
          (cur_state == HDROVHDS_FREQ_CHANGE_IN_PROG) ||
          (cur_state == HDROVHDS_WAIT_FOR_LINK))
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Abandon %d state upon con. setup",
                        cur_state);
        hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
      }
      else if (cur_state != HDROVHDS_PROCESS_ALL_MSGS)
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "OVHD state (%d) inconsistentJ", cur_state );
      }

      break;

#ifdef FEATURE_HDR_REVC
    case HDRIND_LMAC_ASET_CHANGED:
      hdrovhd_process_lmac_aset_changed();
      break;
#endif /* FEATURE_HDR_REVC */

    default:
      /*-----------------------------------------------------------------------
      Default: Indication not recognized.
      -----------------------------------------------------------------------*/
      ERR("OVHD: Unrecognized Indication: %d", ind_name, 0, 0 );
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
}  /* hdrovhd_process_ind() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_IDLE_HO_IND

DESCRIPTION
  This function processes Idle HandOff Indication. It sets state of
  the SectorParameters & AccessParameters to not current and sets the value of
  the expected link.

DEPENDENCIES
  None.

PARAMETERS
  ind_data_ptr - This contains the link_id for the expected link.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_idle_ho_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{

  hdrsrch_link_id_type link_id; /* Link ID of the link handed off to */
  sys_link_type link_info; /* Link info of the handoff destination */
  hdrovhd_cache_entry_type *cache_entry_ptr;
  boolean shortterm_cache_is_valid = FALSE;
  hdrovhds_state_enum_type curr_state = hdrovhds_get_state();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (( curr_state == HDROVHDS_INACTIVE ) ||
      ( curr_state == HDROVHDS_ACC_HO_IN_PROG ) ||
      ( curr_state == HDROVHDS_FREQ_CHANGE_IN_PROG )
#ifdef FEATURE_LTE_TO_HDR_OH
      || ( curr_state == HDROVHDS_TUNNEL_ACTIVE )
#endif /* FEATURE_LTE_TO_HDR_OH */      
     )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
      HDRIND_LMAC_IDLE_HANDOFF, curr_state );
    return;
  }

  link_id = ind_data_ptr->link_id;

  /* Get the link info of the handoff destination */
  hdrlmac_get_link_info( link_id , &link_info );

  if ( hdrscp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL ) == 
         HDRSCP_QUICK_IDLE
#ifdef FEATURE_LTE_TO_HDR_OH
       || 
       hdrscp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL ) ==
         HDRSCP_IRAT_QUICK_IDLE
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    /* Find if there is an entry in the cache (using PN and channel) for the 
       handoff destination */
    cache_entry_ptr = hdrovhd_find_cached_msg_using_qp_info( link_info );

    if ( cache_entry_ptr != NULL )
    {
      shortterm_cache_is_valid = TRUE;
    }

    /* Set flag to indicate that the next QP message that AT will receive will 
     be the first one after idle handoff */
    hdrovhd.is_first_qp_after_idle_ho = TRUE;
  }

  switch( hdrovhds_get_state() )
  {
    case HDROVHDS_PROCESS_ALL_MSGS:
      /*-----------------------------------------------------------------------
      In ProcessAllMsgs state, Change to WaitForLink & update expected link. If
      shortterm cache is valid do not mark overhead as not updated.
      -----------------------------------------------------------------------*/
      if (!shortterm_cache_is_valid)
      {
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
        hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
      }
      hdrlmac_get_link_info( link_id , &hdrovhd.expected_link );
      hdrovhd_change_to_wait_for_link_state();
      break;

    case HDROVHDS_WAIT_FOR_LINK:
      /*-----------------------------------------------------------------------
      In WaitForLink state. Update expected link
      -----------------------------------------------------------------------*/
      hdrlmac_get_link_info( link_id , &hdrovhd.expected_link );
      break;

    case HDROVHDS_SLEEP:
      /*-----------------------------------------------------------------------
      In Sleep State. Set the expected link & set a flag to remember that idle
      handoff has occured. This flag will cause ovhd to transition to
      "Wait For Link" state instead of "Process All Msgs" state on wakeup. If
      shortterm cache is valid do not mark overhead as not updated.
      -----------------------------------------------------------------------*/
      if (!shortterm_cache_is_valid)
      {
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
        hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
      }
      hdrlmac_get_link_info( link_id , &hdrovhd.expected_link );
      hdrovhd.idle_ho_while_sleep = TRUE;
      break;

    case HDROVHDS_INACTIVE:
    case HDROVHDS_ACC_HO_IN_PROG:
    case HDROVHDS_FREQ_CHANGE_IN_PROG:
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDROVHDS_TUNNEL_ACTIVE:
#endif /* FEATURE_LTE_TO_HDR_OH */
      /*-----------------------------------------------------------------------
      In any other state, it is an error
      -----------------------------------------------------------------------*/
      ERR("Idle HO ind rcvd in invalid state:%d", hdrovhds_get_state(), 0, 0);
      break;

    default:
      ERR_FATAL("hdrovhds_get_state() returned unrecognized value!", 0, 0, 0 );
  }

} /* hdrovhd_process_idle_ho_ind() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_CLEAR_CACHE

DESCRIPTION
  This function processes any Indication that indicates that the session has
  changed. This includes the following indications:
  1. Boot completed (X4 only)
  2. Session Reconfigured
  3. Session closed
  4. Previous session opened
  It sets state of the SectorParameters & AccessParameters to not current &
  clears the SP cache. The cache is cleared because all the channels stored for
  each entry have been hashed to using an ATI value which is no longer valid
  after the session has changed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_clear_cache( void )
{
  int i;  /* Index Counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
  hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
  hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */

  /*---------------------------------------------------------------------------
  Clear all SP cache values. This will force overhead to process received
  SectorParameters messages & hash to the correct frequency based on the
  mobile UATI (valid after sesion opened).
  ---------------------------------------------------------------------------*/
  for( i=0; i < HDROVHD_MAX_CACHE_RECORDS; i++ )
  {
    if (hdrovhd.cache[i].cache_entry_state != HDROVHD_ENTRY_INVALID)
    {
      HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_clear_cache");
      hdrovhd.cache_changed = TRUE;
    }
    hdrovhd.cache[i].cache_entry_state = HDROVHD_ENTRY_INVALID;
    hdrovhd.cache[i].ap_entry.is_valid = FALSE;
    hdrovhd.cache[i].qc_is_valid = FALSE;
#ifdef FEATURE_HDR_BCMCS
    hdrovhd.cache[i].bc_entry.state    = HDROVHD_BC_INVALID;
    dsm_free_packet( &(hdrovhd.cache[i].bc_entry.bc_item_ptr) );
#endif /* FEATURE_HDR_BCMCS */
    /* For BroadcastOverhead messages, they are cleared from the cache but
     * hdrbcovhd keeps the latest one for use until we open another session.
     */
  }

} /* hdrovhd_clear_cache() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_CLEAR_CACHE_ENTRY

DESCRIPTION
  This function invalidates a cache node

DEPENDENCIES
  None

PARAMETERS
  cache_entry_ptr - pointer to the cache node to be invalidated

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_clear_cache_entry
(
  hdrovhd_cache_entry_type *cache_entry_ptr
)
{
  if(cache_entry_ptr == NULL)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "cache entry pointer NULL. Ignore: hdrovhd_clear_cache_entry");
    return;
  }
  
  if (cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID)
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_clear_cache_entry");
    hdrovhd.cache_changed = TRUE;
  }

  cache_entry_ptr->qc_is_valid = FALSE;
  cache_entry_ptr->cache_entry_state = HDROVHD_ENTRY_INVALID;
  cache_entry_ptr->ap_entry.is_valid = FALSE;
#ifdef FEATURE_HDR_BCMCS
  cache_entry_ptr->bc_entry.state    = HDROVHD_BC_INVALID;
  cache_entry_ptr->bc_entry.bc_item_ptr = NULL;

  /* Note that we don't free the DSM item pointer here.  This is because in
   * most cases the item was just moved to another cache entry.  In cases
   * where we are deleting entries from the cache we explicitly free the item
   */
#endif /* FEATURE_HDR_BCMCS */

} /* hdrovhd_clear_cache_entry() */
/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_CLEAR_CACHE_ENTRY_AND_STATE

DESCRIPTION
  This function clears a cache entry's state and invalidates the cache node

DEPENDENCIES
  None

PARAMETERS
  cache_entry_ptr - pointer to the cache node to be invalidated

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_clear_cache_entry_and_state
(
  hdrovhd_cache_entry_type *cache_entry_ptr
)
{
    HDR_MSG_PROT_2(MSG_LEGACY_MED,"Clear cache entry + state (chan %d PN %d)",
                   cache_entry_ptr->rx_link.channel.chan_num, 
                   cache_entry_ptr->rx_link.pilot_pn);

    hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
    hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
    hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
    hdrovhd_clear_cache_entry(cache_entry_ptr);
} /* hdrovhd_clear_cache_entry_and_state() */
/*=============================================================================

FUNCTION HDROVHD_DUPLICATE_CACHE_ENTRY

DESCRIPTION
  This function copies a cache node to a different location in the cache.
  The from_cache_ptr is never modified at all.  If an error is returned, 
  the to_cache_ptr is not modified.

DEPENDENCIES
  None

PARAMETERS
  from_cache_ptr - cache node ptr to copy the entry from
  dest_cache_ptr - cache node ptr to copy the entry to

RETURN VALUE
  E_SUCCESS       - cache entry was successfully copied
  E_NOT_AVAILABLE - couldn't copy cache entry because one of the parameters was
                    being written to.  to_cache_ptr is unchanged.
  E_NO_DATA       - couldn't copy cache entry (invalid entry). to_cache_ptr is
                    unchanged.

SIDE EFFECTS
  If E_SUCCESS is returned,the to_cache_ptr is changed to contain a copy of 
  the from_cache_ptr.  If any other value is returned, the to_cache_ptr is 
  unchanged.
  

=============================================================================*/
LOCAL hdrerrno_enum_type hdrovhd_duplicate_cache_entry
(
  hdrovhd_cache_entry_type *from_cache_ptr,
  hdrovhd_cache_entry_type *to_cache_ptr
)
{
  if ( from_cache_ptr->cache_entry_state == HDROVHD_ENTRY_INVALID )
  {
    return E_NO_DATA;
  }

  /*---------------------------------------------------------------------------
  Verify that the node we are copying from is not being written to at this
  moment.
  ---------------------------------------------------------------------------*/
  if ((from_cache_ptr->sp_entry.state == HDROVHD_BUF_WRITE_IN_PROG) || 
      ((from_cache_ptr->ap_entry.is_valid) &&
       (from_cache_ptr->ap_entry.state == HDROVHD_BUF_WRITE_IN_PROG)))
  {
    return E_NOT_AVAILABLE;
  }

  /* technically, if to_cache_ptr is not pointing to an actual
     cache entry (ie, temp variable), this is not a cache update.
     However, for now it is always pointing to a cache entry */
  HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_duplicate_cache_entry");
  hdrovhd.cache_changed = TRUE;

  /*---------------------------------------------------------------------------
  Copy the SP entry.  This is the cache node header, hence if the entry is
  valid, then SP is valid as well.
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_MEMCPY_REMOVAL
  to_cache_ptr->sp_entry = from_cache_ptr->sp_entry; 
#else
  memcpy( (void*) &to_cache_ptr->sp_entry,
          (void*) &from_cache_ptr->sp_entry,
          sizeof(hdrovhd_sp_entry_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* The new cache entry is not in use, even if the old one is. */
  to_cache_ptr->sp_entry.state    = HDROVHD_BUF_NOT_IN_USE;
  to_cache_ptr->cache_entry_state = HDROVHD_ENTRY_VALID;

  //DA what about timestamp and channel fields of the to_cache_ptr?

  /*---------------------------------------------------------------------------
  Copy the AP entry if it is valid.
  ---------------------------------------------------------------------------*/
  if ( from_cache_ptr->ap_entry.is_valid )
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    to_cache_ptr->ap_entry = from_cache_ptr->ap_entry;
#else
    memcpy( (void*) &to_cache_ptr->ap_entry,
            (void*) &from_cache_ptr->ap_entry,
            sizeof(hdrovhd_ap_entry_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

    /* The new cache entry is not in use, even if the old one is. */
    to_cache_ptr->ap_entry.state = HDROVHD_BUF_NOT_IN_USE;
  }
  else
  {
    /* No need to copy an invalid entry, although it should not have hurt. */
    to_cache_ptr->ap_entry.is_valid = FALSE;
  }

#ifdef FEATURE_HDR_BCMCS
  /*---------------------------------------------------------------------------
  Copy the BC entry if it is valid.
  ---------------------------------------------------------------------------*/
  if ( from_cache_ptr->bc_entry.state != HDROVHD_BC_INVALID)
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    to_cache_ptr->bc_entry = from_cache_ptr->bc_entry; 
#else    
    memcpy( (void*) &to_cache_ptr->bc_entry,
            (void*) &from_cache_ptr->bc_entry,
            sizeof(hdrovhd_bc_entry_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
    /* The state of the BC entry is copied along with the entry */
  }
  else
  {
    to_cache_ptr->bc_entry.state = HDROVHD_BC_INVALID;
    to_cache_ptr->bc_entry.bc_item_ptr = NULL;
  }
#endif /* FEATURE_HDR_BCMCS */

  return E_SUCCESS;

} /* hdrovhd_duplicate_cache_entry() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_COPY_TEMP_MSG_TO_CACHE

DESCRIPTION
  Copies all the valid temp msgs to a cache location

DEPENDENCIES
  None

PARAMETERS
  cache_entry_ptr - pointer to where we should copy the temp msg to

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_copy_temp_msg_to_cache
(
  hdrovhd_cache_entry_type *cache_entry_ptr
)
{
  if ( hdrovhd.ap_buffer.is_valid )
  {
    hdrovhd.ap_buffer.is_valid = FALSE;

    if ( (hdrovhd.ap_buffer.link.pilot_pn != cache_entry_ptr->qc_entry.link.pilot_pn) ||
        !SYS_CHAN_CMP_EQUAL(hdrovhd.ap_buffer.link.channel, cache_entry_ptr->qc_entry.link.channel) )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                   "AP in the ap buffer is not from the same sector which QC is received ");
      return;
    }

    /*---------------------------------------------------------------------------
    Copy temp AP msg to cache if it exists and if the destination ap entry is
    valid and not currently in use
    ---------------------------------------------------------------------------*/
    if ( (cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID) &&
            ((!cache_entry_ptr->ap_entry.is_valid) ||
           (cache_entry_ptr->ap_entry.state == HDROVHD_BUF_NOT_IN_USE) )
       )
    {
    /* technically, if cache_entry_ptr is not pointing to an actual
       cache entry (ie, temp variable), this is not a cache update.
       However, for now it is always pointing to a cache entry */
      HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_copy_temp_msg_to_cache");
      hdrovhd.cache_changed = TRUE;

#ifdef FEATURE_MEMCPY_REMOVAL
      cache_entry_ptr->ap_entry = hdrovhd.ap_buffer;
#else
      memcpy( (void *) &cache_entry_ptr->ap_entry,
              (void *) &hdrovhd.ap_buffer,
              sizeof(hdrovhd_ap_entry_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
  
      /*-----------------------------------------------------------------------
        The AP msg in cache is now valid
      -----------------------------------------------------------------------*/
      cache_entry_ptr->ap_entry.is_valid = TRUE;
      cache_entry_ptr->ap_entry.state = HDROVHD_BUF_NOT_IN_USE;
    }
    else
    {
      ERR("Could not copy temp AP msg to cache",0 ,0 ,0);
    }
  }
} /* hdrovhd_copy_temp_msg_to_cache() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_SET_AP_NOT_UPDATED

DESCRIPTION
  Sets the AP msg status to not updated and clears the temp AP buffer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_set_ap_not_updated(void)
{
  hdrovhd.ap_state = HDROVHD_AP_NOT_UPDATED;
  hdrovhd.ap_buffer.is_valid = FALSE;
} /* hdrovhd_set_ap_not_updated() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_CONN_END_RESET_INFO_STATE

DESCRIPTION
  This function is called when a connection is shutdown. Thus, it processes
  ConnectionFailed & ConnectionClosed Indications. It resets the overhead info
  (that is makes it not updated). This is because while connected, overhead
  does not change it's channel according to the hashed value in the SP msg,
  whereas it needs to do so when AT is in Idle. Making the info not updated
  forces the next Sector Parameters to be processed, thus forcing the AT to
  hash to the correct channel.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_conn_end_reset_info_state( void )
{

  hdrovhds_state_enum_type ovhd_state; /* Local copy of state of OVHD */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ovhd_state = hdrovhds_get_state();

  /*---------------------------------------------------------------------------
  While the reverse traffic channel is active the OVHD protocol can only be in
  PROCESS_ALL_MSGS state. In the call flow, the connection closed/ ended
  indication may be received after the deactivate command has been given to
  OVHD in which case it will be in inactive state
  ---------------------------------------------------------------------------*/
  if(( ovhd_state != HDROVHDS_PROCESS_ALL_MSGS ) &&
     ( ovhd_state != HDROVHDS_INACTIVE ))
  {
    ERR("OVHD state not PROCESS_ALL_MSGS or INACTIVE when Ind rcvd!",0,0,0);
    return;
  }

} /* hdrovhd_conn_end_reset_info_state() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_AHO_REQUEST_IND

DESCRIPTION
  This function processes Access HandOff Request Indication.
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_aho_request_ind( void )
{
  if(hdridle_is_sleeping() == TRUE)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "AHO req. while IDLE in sleep state, ignored. OVHD state %d",hdrovhds_get_state());
    return;	
  } 
  switch( hdrovhds_get_state() )
  {
    case HDROVHDS_PROCESS_ALL_MSGS:
    case HDROVHDS_WAIT_FOR_LINK:
      if(!hdrrup_is_tc_assigned( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
         (hdrsmp_session_is_open(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) ||
         (!hdrsmp_session_is_open(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
         !hdrslp_first_access_attempted())))
      {
        /* In the case of DO session is not open, AHO is allowed before the AT starts
         * the first access attempt
         */
        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "AHO. session %d,slp first access attempted %d",
                        hdrsmp_session_is_open(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL),
                        hdrslp_first_access_attempted());
        /*----------------------------------------------------------------------
        Set the ovhd & AP info to be not current & change to AHO_in_prog state
        ----------------------------------------------------------------------*/
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
        hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
        hdrovhds_change_state( HDROVHDS_ACC_HO_IN_PROG );
      }
      else
      {
        /*----------------------------------------------------------------------
        If we are setting up a connection or operating on the traffic channel
        we should stay in HDROVHDS_PROCESS_ALL_MSGS state.  The only reason for
        HDROVHDS_ACC_HO_IN_PROG state is to ensure we don't advertise current
        overhead based on an "old" pilot, but when connected all pilots are
        valid, because the network is assigning us the active set.
        ----------------------------------------------------------------------*/
        /*----------------------------------------------------------------------
        If we are in the process of opening a session, changing pilots across a
        subnet boundary can prevent the session negotiation from succeeding.
        ----------------------------------------------------------------------*/
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "AHO req. while connected, ignored");
      }
      break;

    case HDROVHDS_ACC_HO_IN_PROG:
      /*-----------------------------------------------------------------------
      do nothing.
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "AHO already in progress");
      break;

    case HDROVHDS_INACTIVE:
    case HDROVHDS_SLEEP:
      /*-----------------------------------------------------------------------
      Print Error. Should not have received indication in any of these states
      -----------------------------------------------------------------------*/
      ERR("Acc HO ind rcvd in invalid state:%d", hdrovhds_get_state(), 0, 0);
      break;

    case HDROVHDS_FREQ_CHANGE_IN_PROG:
      /*-----------------------------------------------------------------------
      Print ignore message. Discard an AHO req while changing channel.
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
        "AHO req. while changing IDLE channel (BCMCS), ignored");
      break;

    default:
      ERR_FATAL("hdrovhds_get_state() returned unrecognized value!", 0, 0, 0 );
  }

} /* hdrovhd_process_aho_ind() */
/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_AHO_COMPLETE_IND

DESCRIPTION
  This function processes Access HandOff Completed Indication.
DEPENDENCIES
  None.

PARAMETERS
  ind_data_ptr - Contains the link id of the expected link

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_aho_complete_ind
(
    hdrind_ind_data_union_type *ind_data_ptr
)
{
  hdrsrch_link_id_type link_id; /* Link ID of the link handed off to */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -esym(613, ind_data_ptr) ind_data_ptr is not NULL */
  ASSERT( ind_data_ptr != NULL );
  link_id = ind_data_ptr->link_id;

  if( hdrovhds_get_state() == HDROVHDS_ACC_HO_IN_PROG )
  {
    hdrlmac_get_link_info( link_id , &hdrovhd.expected_link );
    hdrovhd_change_to_wait_for_link_state();
  }
  else
  {
    /* We must have lost acquisition or have received a TCA during the handoff */
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
        "AHO_COMPLETE_IND ignored, state %d",hdrovhds_get_state());

    /* If the phone is not in ACC_HO_IN_PROG state, it may have received and 
       processed AP before access handoff is complete. Invalidate it now */
    hdrovhd.ap_buffer.is_valid = FALSE;
  }
  /*lint +esym(613, ind_data_ptr) ind_data_ptr is not NULL */
} /* hdrovhd_process_aho_complete_ind */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_FREQ_CHANGE_INITIATED_IND

DESCRIPTION
  This function processes Frequency Change Initiated Indication.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_process_freq_change_initiated_ind ( void )
{
  if( hdrovhds_get_state() != HDROVHDS_PROCESS_ALL_MSGS )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
      HDRIND_RUP_FREQ_CHANGE_INITIATED , hdrovhds_get_state() );
    return;
  }

  /*---------------------------------------------------------------------------
  Set the ovhd & AP info = not updated & change to Freq change_in_prog state
  ---------------------------------------------------------------------------*/
  hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
  hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
  hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
  hdrovhds_change_state( HDROVHDS_FREQ_CHANGE_IN_PROG );
} /* hdrovhd_process_freq_change_initiated_ind() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_FREQ_CHANGE_COMPLETE_IND

DESCRIPTION
  This function processes Frequency Change Completed Indication.

DEPENDENCIES
  None.

PARAMETERS
  ind_data_ptr - Contains the link id of the expected link

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_freq_change_complete_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  hdrsrch_link_id_type link_id; /* Link ID for the new channel */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  link_id = ind_data_ptr->link_id;

  if( hdrovhds_get_state() != HDROVHDS_FREQ_CHANGE_IN_PROG )
  {
    ERR("FREQ_CHANGE_COMPLETE Ind rcvd. OVHD state != FREQ_CHANGE_IN_PROG",
         0, 0, 0);
    return;
  }

  hdrlmac_get_link_info( link_id , &hdrovhd.expected_link );
  hdrovhd_change_to_wait_for_link_state();

} /* hdrovhd_process_freq_change_complete_ind */

#ifdef FEATURE_HDR_TO_LTE
/*=============================================================================

FUNCTION HDROVHD_UNPACK_PLMNID

DESCRIPTION
  This function unpacks the plmnid fields from OtherRATNeighborList message 
  into the sys_plmn_id_s_type structure passed in.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  plmnid_ptr - Pointer to the location where plmnid should be unpacked
  offset ptr - Pointer to the offset in the main message to be incremented

RETURN VALUE
  err -  result of error in unpacking

SIDE EFFECTS
  None

=============================================================================*/
LOCAL int hdrovhd_unpack_plmnid
(
  dsm_item_type *item_ptr, 
  sys_plmn_id_s_type *plmnid_ptr,
  uint32 *offset_ptr
)
{
  /* Public Land Mobile Network (PLMN) ID. Format of sys_plmn_id_s_type
   A PLMN ID is defined within a GSM network to be the combination of
   a Mobile Country Code (MCC) and Mobile Network Code (MNC). A PLMN 
   ID is stored in three octets, as specified in 3G TS 24.008, 
   Section 10.5.1.3 and shown below:
  
                                        Bit
                  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                  |===============================================|
   byte 0/octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                  |-----------------------------------------------|
   byte 1/octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                  |-----------------------------------------------|
   byte 2/octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                  |===============================================|
  
   OTA format of MCC and MNC in OtherRATNeighborList. 
  
   When 3 digit MCC and 3 digit MNC is used it is
   MCC Digit 1, MCC Digit 2, MCC Digit 3, MNC Digit 1, MNC Digit 2 and MNC Digit 3
  
   When 3 digit MCC and 2 digit MNC is used it is
   MCC Digit 1, MCC Digit 2, MCC Digit 3, MNC Digit 1 and MNC Digit 2 and "0xF"
  */
  
  /* Extract MCC Digit 1 */
  byte temp = 0;
  int err = 0;					   

  /* Result of unpacking ORNL msg.0=E_SUCCESS */
  err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID( item_ptr, &temp, *offset_ptr );
  *offset_ptr += HDROVHD_ORNL_EUTRAN_PLMNID_SIZE;
  
  plmnid_ptr->identity[0] = (temp & 0x0f);
  
  /* Extract MCC Digit 2 */
  temp = 0;
  err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID( item_ptr, &temp, *offset_ptr );
  *offset_ptr += HDROVHD_ORNL_EUTRAN_PLMNID_SIZE;
  
  plmnid_ptr->identity[0] |= ((temp << 4) & 0xf0);
  
  /* Extract MCC Digit 3 */
  temp = 0;
  err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID( item_ptr, &temp, *offset_ptr );
  *offset_ptr += HDROVHD_ORNL_EUTRAN_PLMNID_SIZE;
  
  plmnid_ptr->identity[1] = (temp & 0x0f);
  
  /* Extract MNC Digit 1 */
  temp = 0;
  err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID( item_ptr, &temp, *offset_ptr );
  *offset_ptr += HDROVHD_ORNL_EUTRAN_PLMNID_SIZE;
  
  plmnid_ptr->identity[2] = (temp & 0x0f);
  
  /* Extract MNC Digit 2 */
  temp = 0;
  err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID( item_ptr, &temp, *offset_ptr );
  *offset_ptr += HDROVHD_ORNL_EUTRAN_PLMNID_SIZE;
  
  plmnid_ptr->identity[2] |= ((temp << 4) & 0xf0);
  
  /* Extract MNC Digit 3 */
  temp = 0;
  err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID( item_ptr, &temp, *offset_ptr );
  *offset_ptr += HDROVHD_ORNL_EUTRAN_PLMNID_SIZE;
  
  plmnid_ptr->identity[1] |= ((temp << 4) & 0xf0);
  
  return err;
}/* hdrovhd_unpack_plmnid() */

/*=============================================================================

FUNCTION HDROVHD_UNPACK_ORNL_MSG

DESCRIPTION
  This function unpacks the received OtherRATNeighborList message into the 
  structure passed in.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  ornl_msg_ptr - Pointer to the location where ORNL msg should be unpacked

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL int hdrovhd_unpack_ornl_msg
(
  dsm_item_type *item_ptr,
  hdrovhd_ornl_entry_type *ornl_msg_ptr
)
{
  boolean rat_type_supported = FALSE;
    /* flag to check if the ORNL message include EUTRAN RATType */
  uint16 other_rat_rec_len;
    /* Length of other RAT NL record */
  hdrsrch_lte_nset_update_params_type *lte_nset_ptr; 
    /* Struct for EUTRA NL record */
  hdrsrch_lte_freq_params_type *lte_freq_ptr;
    /* Struct for EUTRA frequency list */
  uint16 i, j, k; 
    /* Loop couhnter */ 
  uint32 offset = 0;
    /* Offset within ORNL message to extract the field */
  int err = 0;                     
    /* Result of unpacking ORNL msg.0=E_SUCCESS */
  errno_enum_type err_number = E_SUCCESS;
    /* return err_number from unpacking */
  hdrovhd_lte_plmnid_list_type *lte_plmnid_ptr = 0, *prev_lte_plmnid_ptr = 0;
  hdrovhd_lte_plmnid_list_type *lte_s_plmnid_ptr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  err |= (int) HDROVHD_ORNL_OTHER_RAT_SIGNATURE( item_ptr, 
                   &ornl_msg_ptr->other_rat_signature );
  err |= (int) HDROVHD_ORNL_NUM_OTHER_RAT( item_ptr, 
                   &ornl_msg_ptr->num_other_rat );
  offset += HDROVHD_ORNL_FIXED_FIELDS_SIZE;

  /* Extract NumOtherRAT occurrences of the record */
  for ( i = 0; ((i < ornl_msg_ptr->num_other_rat) && 
                (i < HDROVHD_NUM_OTHER_RAT_MAX)); 
        i++ )
  {
    err |= (int) HDROVHD_ORNL_OTHER_RAT_TYPE( item_ptr, 
                   &ornl_msg_ptr->other_rat[i].rat_type, offset );
    offset += HDROVHD_ORNL_EUTRAN_OTHER_RAT_TYPE_SIZE;

    err |= (int) HDROVHD_ORNL_NGBR_RAT_REC_LEN( item_ptr, &other_rat_rec_len, 
                                                offset );
    offset += HDROVHD_ORNL_EUTRAN_NGBR_RAT_REC_LEN_SIZE;
	/*  This field is not used because HDROVHD_NUM_OTHER_RAT_MAX is '1' */

    /* Parse the EUTRA RAT type record */
    if (ornl_msg_ptr->other_rat[i].rat_type == HDROVHD_ORNL_EUTRA_RAT_TYPE)
    {
      /* Set the flag indicating that the ORNL message has got a EUTRAN RATType record */
      rat_type_supported = TRUE;
	  
      lte_nset_ptr = &ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.lte_nset;

      /* PriorityIncluded */
      err |= (int) HDROVHD_ORNL_EUTRAN_PRIORITY_INCL( item_ptr, 
                       &lte_nset_ptr->priority_incl, offset );
      offset += HDROVHD_ORNL_EUTRAN_PRIORITY_INCL_SIZE;

      if (lte_nset_ptr->priority_incl)
      {
        /* ServingPriority */
        err |= (int) HDROVHD_ORNL_EUTRAN_SERVING_PRIORITY( item_ptr, 
                         &lte_nset_ptr->serving_priority, offset );
        offset += HDROVHD_ORNL_EUTRAN_SERVING_PRIORITY_SIZE;
      }

      /* ThreshServing */
      err |= (int) HDROVHD_ORNL_EUTRAN_THRESH_SERVING( item_ptr, 
                       &lte_nset_ptr->thresh_serving, offset );
      offset += HDROVHD_ORNL_EUTRAN_THRESH_SERVING_SIZE;


      /* PerEARFCNParamsIncluded */
      err |= (int) HDROVHD_ORNL_EUTRAN_PER_EARFCN_PARAMS_INCL( item_ptr, 
                       &lte_nset_ptr->per_earfcn_params_incl, offset );
      offset += HDROVHD_ORNL_EUTRAN_PER_EARFCN_PARAMS_INCL_SIZE;

      if (!lte_nset_ptr->per_earfcn_params_incl)
      {
        /* RxLevMinEUTRACommon */
        err |= (int) HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_COMMON( item_ptr, 
                         &lte_nset_ptr->rx_level_min_eutra_common, offset );
        offset += HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_COMMON_SIZE;

        /* PEMaxCommon */
        err |= (int) HDROVHD_ORNL_EUTRAN_PE_MAX_COMMON( item_ptr, 
                         &lte_nset_ptr->pe_max_common, offset );
        offset += HDROVHD_ORNL_EUTRAN_PE_MAX_COMMON_SIZE;

        /* RxLevMinEUTRAOffsetCommonIncl */
        err |= (int) HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON_INCL( 
                         item_ptr, 
                         &lte_nset_ptr->rx_lev_min_eutra_offset_common_incl, 
                         offset );
        offset += HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON_INCL_SIZE;

        if (lte_nset_ptr->rx_lev_min_eutra_offset_common_incl)
        {
            /* RxLevMinEUTRAOffsetCommon */
          err |= (int) HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON( 
                           item_ptr, 
                           &lte_nset_ptr->rx_lev_min_eutra_offset_common,
                           offset );
          offset += HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_COMMON_SIZE;
        }
      }

      /* MaxReselectionTimerIncluded */
      err |= (int) HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER_INCL( item_ptr, 
                           &lte_nset_ptr->max_reselect_timer_incl, offset );
      offset += HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER_INCL_SIZE;

      if (lte_nset_ptr->max_reselect_timer_incl)
      {
        /* MaxReselectionTimer */
        err |= (int) HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER( item_ptr, 
                           &lte_nset_ptr->max_reselect_timer, offset );
        offset += HDROVHD_ORNL_EUTRAN_MAX_RESEL_TIMER_SIZE;
      }

      /* SearchBackOffTimerIncluded */
      err |= (int) HDROVHD_ORNL_EUTRAN_SRCH_BACKOFF_TIMER_INCL( item_ptr, 
                            &lte_nset_ptr->backoff_timer_incl, offset );
      offset += HDROVHD_ORNL_EUTRAN_SRCH_BACKOFF_TIMER_INCL_SIZE;

      if (lte_nset_ptr->backoff_timer_incl)
      {
        /* MinMeasurementBackoff */
        err |= (int) HDROVHD_ORNL_EUTRAN_MIN_MESAUREMENT_BACKOFF( item_ptr, 
                            &lte_nset_ptr->min_backoff, offset );
        offset += HDROVHD_ORNL_EUTRAN_MIN_MESAUREMENT_BACKOFF_SIZE;

        /* MaxMeasurementBackoff */
        err |= (int) HDROVHD_ORNL_EUTRAN_MAX_MESAUREMENT_BACKOFF( item_ptr, 
                            &lte_nset_ptr->max_backoff, offset );
        offset += HDROVHD_ORNL_EUTRAN_MAX_MESAUREMENT_BACKOFF_SIZE;
      }

      /* PLMNIDIncluded */
      err |= (int) HDROVHD_ORNL_EUTRAN_PLMNID_INCL( item_ptr, 
               &ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.plmnid_included, 
               offset );
      offset += HDROVHD_ORNL_EUTRAN_PLMNID_INCL_SIZE;

      /* NumEUTRAFrequencies */
      err |= (int) HDROVHD_ORNL_EUTRAN_NUM_FREQ( item_ptr, 
                            &lte_nset_ptr->num_eutra_freqs, offset );
      offset += HDROVHD_ORNL_EUTRAN_NUM_FREQ_SIZE;

      for ( j = 0; j < lte_nset_ptr->num_eutra_freqs; j++ )
      {
        lte_freq_ptr = &lte_nset_ptr->earfcn_params_arr[j];

        /* EARFCN */
        err |= (int) HDROVHD_ORNL_EUTRAN_EARFCN( item_ptr, 
                         (uint16 *)&lte_freq_ptr->earfcn, offset );
        offset += HDROVHD_ORNL_EUTRAN_EARFCN_SIZE;

        if (lte_nset_ptr->priority_incl)
        {
          /* EARFCNPriority */
          err |= (int) HDROVHD_ORNL_EUTRAN_EARFCN_PRIORITY( item_ptr, 
                           &lte_freq_ptr->earfcn_priority, offset );
          offset += HDROVHD_ORNL_EUTRAN_EARFCN_PRIORITY_SIZE;
        }

        /* ThreshX */
        err |= (int) HDROVHD_ORNL_EUTRAN_EARFCN_THRESHX( item_ptr, 
                         &lte_freq_ptr->thresh_x, offset );
        offset += HDROVHD_ORNL_EUTRAN_EARFCN_THRESHX_SIZE;

        if (lte_nset_ptr->per_earfcn_params_incl)
        {
          /* RxLevMinEUTRA */
          err |= (int) HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA( item_ptr, 
                           &lte_freq_ptr->rx_lev_min_eutra, offset );
          offset += HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_SIZE;

          /* PEMax */
          err |= (int) HDROVHD_ORNL_EUTRAN_PE_MAX( item_ptr, 
                           &lte_freq_ptr->pe_max, offset );
          offset += HDROVHD_ORNL_EUTRAN_PE_MAX_SIZE;

          /* RxLevMinEUTRAOffset Incl */
          err |= (int) HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_INCL( item_ptr, 
                           &lte_freq_ptr->rx_lev_min_eutra_offset_incl, offset );
          offset += HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_INCL_SIZE;

		  if (lte_freq_ptr->rx_lev_min_eutra_offset_incl)
		  {
          /* RxLevMinEUTRAOffset  */
          err |= (int) HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET( item_ptr, 
                           &lte_freq_ptr->rx_lev_min_eutra_offset, offset );
          offset += HDROVHD_ORNL_EUTRAN_RX_LEV_MIN_EUTRA_OFFSET_SIZE;
          }
        }

        /* MeasurementBandwidth */
        err |= (int) HDROVHD_ORNL_EUTRAN_MEAS_BW( item_ptr, 
                         &lte_freq_ptr->bandwidth, offset );
        offset += HDROVHD_ORNL_EUTRAN_MEAS_BW_SIZE;

        if (ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.plmnid_included)
        {
          lte_plmnid_ptr = 
		  	&ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.lte_plmnid_list[j];
		  /* local pointer variable to fit the long structure elements in 90 chars */
		  
          /* PLMNSameAsPreviousChannel */
          err |= (int) HDROVHD_ORNL_EUTRAN_PLMN_SAME_AS_PREV_CH( item_ptr, 
                           &(lte_plmnid_ptr->plmn_same_as_prev_channel), offset );
          offset += HDROVHD_ORNL_EUTRAN_PLMN_SAME_AS_PREV_CH_SIZE;

          if (!lte_plmnid_ptr->plmn_same_as_prev_channel)
          {
            /* NumPLMNIDs */
            err |= (int) HDROVHD_ORNL_EUTRAN_NUM_PLMNIDS( item_ptr, 
                             &(lte_plmnid_ptr->num_plmn_ids), offset );
            offset += HDROVHD_ORNL_EUTRAN_NUM_PLMNIDS_SIZE;

            /* If NumPLMNIDs field is included, the NumPLMNIDs+1 occurrences of the 
               following record; Otherwise, 0 occurances of the following record. 
             */
            lte_plmnid_ptr->num_plmn_ids++;

            for (k = 0; k < lte_plmnid_ptr->num_plmn_ids; k++)
            {
			  err |= hdrovhd_unpack_plmnid(item_ptr, &(lte_plmnid_ptr->plmnid[k]), &offset );
            }
          }
          else
          {
            if (j > 0)
            {
			  prev_lte_plmnid_ptr = 
			    &ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.lte_plmnid_list[j-1];
			  /* local pointer variable to fit the long structure elements in 90 chars */

              lte_plmnid_ptr->num_plmn_ids = prev_lte_plmnid_ptr->num_plmn_ids;
#ifdef FEATURE_MEMCPY_REMOVAL
              memscpy ( lte_plmnid_ptr->plmnid, sizeof(lte_plmnid_ptr->plmnid), 
                        prev_lte_plmnid_ptr->plmnid, 
                        sizeof(prev_lte_plmnid_ptr->plmnid) );
#else
              memcpy ( lte_plmnid_ptr->plmnid, prev_lte_plmnid_ptr->plmnid,
                     (sizeof (sys_plmn_id_s_type) * HDROVHD_MAX_PLMNIDS_PER_FREQ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
            }
            else
            {
              HDR_MSG_PROT (MSG_LEGACY_ERROR, 
                       "plmn_same_as_prev_channel set to 1 for first entry");
            }
          }
        }
      } /* Per NumEUTRAFrequencies */

      /* ServingNetworkPLMNIDIncluded */
      err_number |= (int) HDROVHD_ORNL_EUTRAN_SERVING_NW_PLMNID_INCL( item_ptr, 
         &ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.serving_nw_plmn_included,
         offset );
      offset += HDROVHD_ORNL_EUTRAN_SERVING_NW_PLMNID_INCL_SIZE;

      if (err_number == E_NO_DATA)
      {
        ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.serving_nw_plmn_included = 0;
      }
      else if (ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.serving_nw_plmn_included)
      {
        lte_s_plmnid_ptr = 
          &ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.lte_serving_plmnid_list;

        /* This field is not applicable for S PLMNID */
        lte_s_plmnid_ptr->plmn_same_as_prev_channel  = 0;
		
        /* NumServingPLMNIDs */
        err |= (int) HDROVHD_ORNL_EUTRAN_NUM_SERVING_PLMNIDS( item_ptr, 
                                   &(lte_s_plmnid_ptr->num_plmn_ids), offset );
        offset += HDROVHD_ORNL_EUTRAN_NUM_SERVING_PLMNIDS_SIZE;

        /* If the ServingNetworkPLMNIncl is not included, or is set to „0., then the 
                   access network shall omit this field. Otherwise the access network shall 
                   set this field to one less than the number of PLMN IDs that are 
                   associated with this Serving Network.
              */
        lte_s_plmnid_ptr->num_plmn_ids++;

        for (k = 0; k < lte_s_plmnid_ptr->num_plmn_ids; k++)
        {
		  err |= hdrovhd_unpack_plmnid(item_ptr, &(lte_s_plmnid_ptr->plmnid[k]), &offset );

		  HDR_MSG_PROT_7(MSG_LEGACY_MED, 
					 "ornl orat#=%x.lte_nbr.splmn_inc=%x,nplmn=%x,spmlmnid#=%x,\
					  plmdid[0]=%x,plmdid[1]=%x,plmdid[2]=%x", i,
					  ornl_msg_ptr->other_rat[i].other_rat_rec.lte_nbr.serving_nw_plmn_included,
					  lte_s_plmnid_ptr->num_plmn_ids, k, 
					  lte_s_plmnid_ptr->plmnid[k].identity[0],
					  lte_s_plmnid_ptr->plmnid[k].identity[1], 
					  lte_s_plmnid_ptr->plmnid[k].identity[2]);
		  /*Unit Test code */
        }
      }	

      /* RSRQParametersIncluded */
	  /* The access network shall include this field if any of the fields other than 
	   the Reserved field that follow this field are to be included in the message. 
	   If this field is included, the access network shall set it as follows: The 
	   access network shall set this field to „1. if the EUTRA RSRQ associated 
	   fields are included in this message. Otherwise, the access network shall 
	   set this field to „0.. If this field is not included in the message, the access 
	   terminal shall assume a value of „0. for this field. */
	  lte_nset_ptr->rsrq_parameters_incl = 0;
      err_number |= (int) HDROVHD_ORNL_EUTRAN_RSRQ_PARAMS_INCL( item_ptr, 
                       &lte_nset_ptr->rsrq_parameters_incl, offset );
      offset += HDROVHD_ORNL_EUTRAN_RSRQ_PARAMS_INCL_SIZE;

      if (err_number == E_NO_DATA)
      {
        lte_nset_ptr->rsrq_parameters_incl = 0; 
      }

      if (lte_nset_ptr->rsrq_parameters_incl && !lte_nset_ptr->per_earfcn_params_incl)
      {
        /* QualMinEUTRACommon */
		/* If the RSRQParametersIncluded is set to „1. and the 
		PerEARFCNParamsIncluded is set to „0., the access network shall set this 
		field in the range 0 to 31, where -3 – QualMinEUTRACommon in dB is 
		equal to the minimum reference signal received quality (RSRQ) level of a 
		EUTRA cell as specified in in [16]. Otherwise, the access network shall omit 
		this field. */
        err |= (int) HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_COMMON( item_ptr, 
                         &lte_nset_ptr->qual_min_eutra_common, offset );
        offset += HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_COMMON_SIZE;

        /* QualMinEUTRAOffsetCommonInc */
	    /* If the RSRQParametersIncluded is set to „1. and the 
	       PerEARFCNParamsIncluded is set to „0., then if the access network 
	       includes QualMinEUTRAOffsetCommon in this record, the access 
	       network shall set this field to „1.; else the access network shall set this 
	       field to „0.. Otherwise, the access network shall omit this field. */
        err |= (int) HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON_INC( item_ptr, 
                         &lte_nset_ptr->qual_min_eutra_offset_common_incl, offset );
        offset += HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON_INC_SIZE;

        if (lte_nset_ptr->qual_min_eutra_offset_common_incl)
        {
          /* QualMinEUTRAOffsetCommon */
		  /* If the QualMinEUTRAOffsetCommonIncl is set to „1., then the access 
		   network shall set this field to one less than the value specified in [16]. 
		   Otherwise, the access network shall omit this field. The range of the 
		   common offset values allowed is 1 through 8. */
          err |= (int) HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON( item_ptr, 
                         &lte_nset_ptr->qual_min_eutra_offset_common, offset );
          offset += HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_COMMON_SIZE;
        }

		HDR_MSG_PROT_5 (MSG_LEGACY_MED, 
			"ornl orat#=%x rsrqp_inc=%x,qmineutracom=%x,qmineutraoffscom_inc=%x,\
			qmineutraoffscom=%x", i, lte_nset_ptr->rsrq_parameters_incl, 
            lte_nset_ptr->qual_min_eutra_common,
			lte_nset_ptr->qual_min_eutra_offset_common_incl, 
			lte_nset_ptr->qual_min_eutra_offset_common);
		/*Unit Test code */
	  }

      if (lte_nset_ptr->rsrq_parameters_incl)
      {
        for ( j = 0; j < lte_nset_ptr->num_eutra_freqs; j++ )
        {
          lte_freq_ptr = &lte_nset_ptr->earfcn_params_arr[j];

	      /* ThreshXQ */
		  /* If the RSRQParametersIncluded is set to „1., access network shall set this 
		   field in the range 0 to 31. Otherwise, the access network shall omit this 
		   field. */
          err |= (int) HDROVHD_ORNL_EUTRAN_EARFCN_THRESHXQ( item_ptr, 
                           &lte_freq_ptr->thresh_xq, offset );
          offset += HDROVHD_ORNL_EUTRAN_EARFCN_THRESHXQ_SIZE;

		  HDR_MSG_PROT_3 (MSG_LEGACY_MED, 
			  "ornl orat#=%x eutrafreq#=%x threshxq=%x", i, j,
			  lte_freq_ptr->thresh_xq);
		      /*Unit Test code */
          
	   if (lte_nset_ptr->per_earfcn_params_incl)
           {  
            /* QualMinEUTRA */
		    /* If the RSRQParametersIncluded is set to „1. and the 
		     PerEARFCNParamsIncluded is set to „1., the access network shall set this 
		     field in the range 0 to 31, where -3 – QualMinEUTRA in dB is equal to the 
		     minimum reference signal received quality (RSRQ) level of a EUTRA cell 
		     as specified in [16]. Otherwise, the access network shall omit this field. */
            err |= (int) HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA( item_ptr, 
                             &lte_freq_ptr->qual_min_eutra, offset );
            offset += HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_SIZE;
            
            /* QualMinEUTRAOffsetInc */
		    /* If the RSRQParametersIncluded is set to „1. and the 
		     PerEARFCNParamsIncluded is set to „1., then if the access network 
		     includes QualMinEUTRAOffset in this record, the access network shall 
		     set this field to „1.; else the access network shall set this field to 
		     „0.. Otherwise, the access network shall omit this field. */
            err |= (int) HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_INCL( item_ptr, 
                             &lte_freq_ptr->qual_min_eutra_offset_incl, offset );
            offset += HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_INCL_SIZE;
            
            if (lte_freq_ptr->qual_min_eutra_offset_incl)
            {
              /* QualMinEUTRAOffset */
		      /* If the QualMinEUTRAOffsetIncl is set to „1., then the access network 
		         shall set this field to one less than the QualMinEUTRAOffset.  
		         Otherwise, the access network shall omit this field. The range of  
		         the offset values allowed is 1 through 8. */
              err |= (int) HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET( item_ptr, 
                             &lte_freq_ptr->qual_min_eutra_offset, offset );
              offset += HDROVHD_ORNL_EUTRAN_QUAL_MIN_EUTRA_OFFSET_SIZE;
              }
		    
		      HDR_MSG_PROT_5 (MSG_LEGACY_MED, 
			  "ornl orat#=%x eutrafreq#=%x qmineutra=%x,\
			  qmineutraoffs_inc=%x,qmineutraoffs=%x", i, j,
			  lte_freq_ptr->qual_min_eutra,
			  lte_freq_ptr->qual_min_eutra_offset_incl, 
			  lte_freq_ptr->qual_min_eutra_offset);
		      /*Unit Test code */
          }
	}
      }
    }
    else /* Per RATType */
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Unknown RATType %d in OtherRATNeighborList",
	  									ornl_msg_ptr->other_rat[i].rat_type );
    }
  }
  if(rat_type_supported == FALSE)
  {
  	/*RATType does not include EUTRAN in the entire ORNL message*/
	HDR_MSG_PROT( MSG_LEGACY_ERROR, "RATType not supported");
  	err = E_NOT_SUPPORTED;
  }
  return err;

} /* hdrovhd_unpack_ornl_msg() */

/*===========================================================================

FUNCTION HDROVHD_LTE_NSET_UPDATE

DESCRIPTION
  This function is called to update the searcher with LTE nset.
  
DEPENDENCIES
  None

PARAMETERS
  lte_nset_update_params - LTE frequency information.
  lte_band_pref - LTE band pref info from SD.
  is_new_nset - this function is called because of receiving ORNL or
                FPLMN list update.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrovhd_lte_nset_update
(   
#if (LTE_BAND_NUM == 256)
  sys_lte_band_mask_e_type lte_band_pref,
#else
  sys_band_mask_type lte_band_pref,
#endif /* (LTE_BAND_NUM == 256) */
  boolean is_new_nset
)
{
  hdrsrch_lte_nset_update_params_type *lte_nset_update_params;
  hdrsrch_lte_nset_update_params_type pruned_lte_nset_params;
  uint8 num_eutra_freqs = 0;
  uint8 i = 0;
  uint8 k = 0;
  boolean keep_earfcn = TRUE;
  boolean plmn_forbidden = FALSE;
    /* Whether a freq is pruned because of its associated PLMNs are forbidden */
  boolean tdd_prune_required = lte_rrc_get_tdd_prune_required();
  sys_plmn_id_s_type pruned_plmn[HDROVHD_MAX_PLMNIDS_PER_FREQ];
  hdrovhd_ornl_entry_type *ornl_msg_ptr; 
    /* Pointer to cache entry for current ORNL message */
  hdrovhd_lte_rat_rec_type *lte_nbr_ptr;
    /* local pointer used to keep code line length below 80 char */
#if (LTE_BAND_NUM == 256)
  static sys_lte_band_mask_e_type lte_band_pref_static;
#else
  static sys_band_mask_type lte_band_pref_static = 0;
#endif /* (LTE_BAND_NUM == 256) */
    /* static variable stores the lastest LTE band preference info */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "LTE nset updated with searcher (new nset %d)", 
                  is_new_nset );

  /* Get the entry at which the ORNL message need to be extracted */  
  ASSERT( hdrovhd.curr_cache_entry_ptr != NULL );
  ornl_msg_ptr = &hdrovhd.curr_cache_entry_ptr->ornl_entry;
  ASSERT(ornl_msg_ptr->other_rat[0].rat_type == HDROVHD_ORNL_EUTRA_RAT_TYPE);
  lte_nset_update_params = &ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr.lte_nset;

  /* Reuse last LTE band preference info if this function is called
   * because FPLMN update. */
  if ( is_new_nset )
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    lte_band_pref_static = lte_band_pref;
#else
    memcpy( &lte_band_pref_static, &lte_band_pref, 
#if (LTE_BAND_NUM == 256)
            sizeof(sys_lte_band_mask_e_type) );
#else
            sizeof(sys_band_mask_type) );
#endif /* (LTE_BAND_NUM == 256) */
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

  /* Copy the entire Other RAT info first */
#ifdef FEATURE_MEMCPY_REMOVAL
  pruned_lte_nset_params = *lte_nset_update_params;
#else
  memcpy ( &pruned_lte_nset_params, lte_nset_update_params, 
           sizeof ( hdrsrch_lte_nset_update_params_type )); 
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Current design supports only 1 other rat */
  lte_nbr_ptr = &(ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr);

  /* Loop through and remove the unsupported LTE freq and LTE TDD freq */
  for ( i = 0; i < lte_nset_update_params->num_eutra_freqs; i++)
  {
    /* Reset flag whether the given EARFCN need to pruned or not */
    keep_earfcn = TRUE;

	
    /* Check whether freq's PLMN are forbidden */
    if ( lte_nbr_ptr->lte_plmnid_list[i].plmn_same_as_prev_channel )
    {
      /* Do not calculate PLMNs for a second time if PLMN is the same
       * as previous freq */
      if ( plmn_forbidden )
      {
        keep_earfcn = FALSE;
      }
      else
      {
        keep_earfcn = TRUE;
      }
    }
    else
    {
      plmn_forbidden = FALSE;
      if ( !hdrovhd_get_plmnids_associated_with_earfcn( 
              lte_nset_update_params->earfcn_params_arr[i].earfcn, pruned_plmn ) )
      {
        /* There are two cases if no PLMNs are associated with EARFCN.
         * 1. PLMNs are not included
         * 2. PLMNs are included but all of them are forbidden 
         */
        if ( lte_nbr_ptr->plmnid_included )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "0 PLMN ID is allowed for LTE freq = %d from ORNL message",
                          lte_nset_update_params->earfcn_params_arr[i].earfcn );
          keep_earfcn = FALSE;
          plmn_forbidden = TRUE;
        }
      }
    }

    if ( keep_earfcn )
    {
      /* Remove any unsupported frequencies from ORNL message */
      if ( lte_rrc_earfcn_is_supported ( 
                         lte_nset_update_params->earfcn_params_arr[i].earfcn,
                         lte_band_pref_static ))
      {
        /* Remove LTE TDD frequency if required */
        if ( tdd_prune_required && lte_rrc_cell_is_tdd( 
                   lte_nset_update_params->earfcn_params_arr[i].earfcn ))
        {
          keep_earfcn = FALSE;
          HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
                          "Removed LTE TDD freq = %d from ORNL message",
                          lte_nset_update_params->earfcn_params_arr[i].earfcn);
        }
        else
        {
          keep_earfcn = TRUE;
        }
      }
      else
      {
        keep_earfcn = FALSE;
        HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
                        "Removed unsupported LTE freq = %d from ORNL message",
                        lte_nset_update_params->earfcn_params_arr[i].earfcn);
      }
    }

    if (keep_earfcn)
    {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Number of barred earfcn %d", hdrovhd.num_avoided_earfcn);							     
        for(k = 0; k < hdrovhd.num_avoided_earfcn; k++)
        {
          if(lte_nset_update_params->earfcn_params_arr[i].earfcn == 
		  	             hdrovhd.avoided_earfcn[k])
          {
			  HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
							  "Removed LTE freq = %d, which is barred", hdrovhd.avoided_earfcn[k]);
			  keep_earfcn = FALSE;
			  break;
          }
        }
    }
	
    if (keep_earfcn)
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      pruned_lte_nset_params.earfcn_params_arr[num_eutra_freqs] = 
                              lte_nset_update_params->earfcn_params_arr[i];
#else
      memcpy ( &pruned_lte_nset_params.earfcn_params_arr[num_eutra_freqs],
               &lte_nset_update_params->earfcn_params_arr[i],
               sizeof(hdrsrch_lte_freq_params_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_LTE_DEPRIORITIZATION
      /* If there is frequency deprioritization, it is deprioritized on the
       * local variable which is passed to SRCH. So when deprioritization is
       * cancelled, we still have the original priority stored in global 
       * variable. 
       */ 
      hdrovhd_update_earfcn_priority(&pruned_lte_nset_params.earfcn_params_arr[num_eutra_freqs]);
#endif /* FEATURE_LTE_DEPRIORITIZATION */

      num_eutra_freqs++;
    }
  }

  pruned_lte_nset_params.num_eutra_freqs = num_eutra_freqs;

  hdrlmac_lte_nset_update( &pruned_lte_nset_params, lte_band_pref_static );

} /* hdrovhd_lte_nset_update */


/*=============================================================================

FUNCTION HDROVHD_D2L_IS_ENABLED

DESCRIPTION
  This function checks whether HDR to LTE reselection is enabled or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if HDR to LTE reselection is enabled; FALSE otherwise.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL boolean hdrovhd_d2l_is_enabled( void )
{
  if ( 
#ifdef FEATURE_IRAT_FORCE_BSR_D2L_SETTING
       hdrovhd.force_d2l ||
#endif /* FEATURE_IRAT_FORCE_BSR_D2L_SETTING */      
       hdrovhd.hdr_to_lte_resel_enabled
     )  
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrovhd_d2l_is_enabled() */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_ORNL_MSG

DESCRIPTION
  This function processes a received OtherRATNeighborList message.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_ornl_msg
(
  dsm_item_type *item_ptr
)
{
  sys_link_type ornl_link;           /* Link on which ORNL msg is received */
  hdrsrch_link_id_type link_id;    /* LinkID of rcvd msg */
  hdrovhd_ornl_entry_type *ornl_msg_ptr; 
    /* Pointer to cache entry at which the ORNL message need to be extracted*/
  hdrovhds_state_enum_type cur_state = hdrovhds_get_state();
    /* Current overhead state */
  int err = 0;                     
    /* Result of unpacking ORNL msg.0=E_SUCCESS */
  sys_rat_pri_list_info_s_type rat_pri_list;
    /* RAT priority list */
  boolean lte_is_supported = FALSE;
   /* Whether LTE is supported or not */
  uint8 i = 0;
   /* Loop index */
#if (LTE_BAND_NUM == 256)
  sys_lte_band_mask_e_type lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE; 
#else
  sys_band_mask_type lte_band_pref = 0;
#endif /* (LTE_BAND_NUM == 256) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
  if (hdralmp_hdr_to_lte_redir_meas_in_progress() == TRUE )
  { 
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "ORNL msg ignored: D2L redirection is in progress");
    return;
  }
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

#ifdef FEATURE_LTE_TO_HDR
  /* Do not process ORNL msg if any L2D HO, get_CGI or D2L Time Tfr in progress */
  if ( hdrmc_lte_to_hdr_ho_is_in_progress() ||
       hdrmc_lte_to_hdr_get_cgi_is_in_progress() ||
       hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
  {
    HDR_MSG_PROT_3( MSG_LEGACY_MED,
                    "ORNL msg ignored: lte to hdr ho/resel/redir/cgi/time tfr in progress\
	             ho=%d, cgi=%d, rtt=%d",
                    hdrmc_lte_to_hdr_ho_is_in_progress(),
                    hdrmc_lte_to_hdr_get_cgi_is_in_progress(),
                    hdrmc_hdr_to_lte_time_transfer_is_in_progress() );
    return;
  }
#endif /*FEATURE_LTE_TO_HDR*/

  if(hdralmps_get_state() == HDRALMPS_CONNECTED)
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE( HDROVHD_ORNL_MSG_ID, cur_state );
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "ORNL msg ignored: message received in connected state");
    return;	
  }
  if ((cur_state != HDROVHDS_PROCESS_ALL_MSGS) &&
      (cur_state != HDROVHDS_WAIT_FOR_LINK))
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE( HDROVHD_ORNL_MSG_ID, cur_state );
    HDR_MSG_PROT_1 (MSG_LEGACY_MED, "ORNL msg ignored: message received in state %d", cur_state);
    return;
  }
    
  /* If OtherRATAvailable field of the latest received QuickConfig message is set to 1, 
     only then AT shall process the OtherRATNeighborList message, otherwise ignore */
  if (!hdrovhd.qc_info.other_rat_available)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
       "ORNL msg ignored: No OtherRATTxCycle included in QC message");
    return;
  }  
    
 /*---------------------------------------------------------------------------
  Check if DO to LTE reselection is supported in NV
  ---------------------------------------------------------------------------*/
  if ( !hdrovhd_d2l_is_enabled() )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "ORNL msg ignored: D2L reselection disabled in NV" );
    return;
  }

  /*---------------------------------------------------------------------------
  Check with SD whether full LTE service is supported
  ---------------------------------------------------------------------------*/
  if ( !sd_misc_is_lte_full_srv_allowed() )
  {
    HDR_MSG_PROT (MSG_LEGACY_MED, "ORNL msg ignored: GWL service not available");
    return;
  }

  /*---------------------------------------------------------------------------
  Check whether LTE is supported in RAT priority list
  ---------------------------------------------------------------------------*/
  rat_pri_list = sd_misc_get_rat_priority_list();

  for (i = 0; i < rat_pri_list.num_items; i++)
  {
    if (rat_pri_list.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      lte_is_supported = TRUE;
#if (LTE_BAND_NUM == 256)
      lte_band_pref = rat_pri_list.priority_list_info[i].band_cap.lte_band_cap;
#else
      lte_band_pref = rat_pri_list.priority_list_info[i].band_cap;
#endif /* (LTE_BAND_NUM == 256) */
      break;
    }
  }

  if (!lte_is_supported)
  {
    HDR_MSG_PROT (MSG_LEGACY_MED, 
                  "ORNL msg ignored: LTE not in RAT priority list");
    return;
  }

  /*---------------------------------------------------------------------------
  If the OtherRATNeighborList are already current, there is no need to further
  process the message.
  ---------------------------------------------------------------------------*/
  if ( hdrovhd.ornl_state == HDROVHD_ORNL_UPDATED )
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "ORNL msg ignored: already stored");
    return;
  }

  /*---------------------------------------------------------------------------
  Get the link_id for received message. Compare the link_id fields
  with those received for previous QuickConfig message. If the fields
  do not match, then do not process message
  ---------------------------------------------------------------------------*/
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  /*  Call LMAC function to get the HDR channel & PN offset associated with
  this link_id.*/
  hdrlmac_get_link_info( link_id, &ornl_link );

  if( (!hdrovhd.qc_info.initialized) ||
      (!SYS_CHAN_CMP_EQUAL( ornl_link.channel, hdrovhd.qc_info.link.channel )) ||
      (ornl_link.pilot_pn != hdrovhd.qc_info.link.pilot_pn ) ||
      (hdrovhd.curr_cache_entry_ptr == NULL) )
  {
    HDRTRACE_OVHD_IGNORED_MSG_UNEXPECTED_LINK( HDROVHD_ORNL_MSG_ID,
                        hdrovhd.qc_info.link.channel.chan_num,
                        hdrovhd.qc_info.link.pilot_pn,
                        ornl_link.channel.chan_num,
                        ornl_link.pilot_pn);
    HDR_MSG_PROT_4 (MSG_LEGACY_MED, 
           "ORNL msg ignored: Unexpected Link qc=%d, qp=%d, oc=%d, op=%d",
		   hdrovhd.qc_info.link.channel.chan_num,
                   hdrovhd.qc_info.link.pilot_pn,
                   ornl_link.channel.chan_num,
                   ornl_link.pilot_pn);
    return;
  }

  /* The current design assumes that only one ornl_msg_ptr->other_rat record is supported */
  /* Get the entry at which the ORNL message need to be extracted */
  ornl_msg_ptr = &hdrovhd.curr_cache_entry_ptr->ornl_entry;

  memset(ornl_msg_ptr, 0 , sizeof(hdrovhd_ornl_entry_type));
  err = hdrovhd_unpack_ornl_msg(item_ptr, ornl_msg_ptr);
  hdrovhd_update_splmn_info(NULL, ornl_msg_ptr);

  if (err == E_SUCCESS)
  {
    hdrovhd.ornl_state = HDROVHD_ORNL_UPDATED;

    hdrovhd_lte_nset_update ( lte_band_pref, TRUE );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Error unpacking ORNL message" );
  }

} /* hdrovhd_process_ornl_msg() */
#endif /* FEATURE_HDR_TO_LTE*/

#ifdef FEATURE_LTE_TO_HDR_OH
/*=============================================================================

FUNCTION HDROVHD_PROCESS_HSP_MSG

DESCRIPTION
  This function processes a received HRPDSilenceParameters message.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_hsp_msg
(
  dsm_item_type *item_ptr
)
{
  int err = 0;                     
    /* Result of unpacking HSP msg.0=E_SUCCESS */
  uint8 rl_silence_duration = 0;
    /* ReverseLinkSilenceDuration in HSP message */
  uint8 rl_silence_period = 0;
    /* ReverseLinkSilenceDuration in HSP message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !hdrsap_tunnel_mode_is_enabled(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) )
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "HSP msg ignored: tunnel mode not enabled");
    return;
  }

  err |= (int) HDROVHD_HSP_RL_SILENCEDURATION( item_ptr, 
                   &rl_silence_duration );
  err |= (int) HDROVHD_HSP_RL_SILENCEPERIOD( item_ptr, 
                   &rl_silence_period );

  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "HSP RSD=%d, RSP=%d", 
                  rl_silence_duration, rl_silence_period);

  /*---------------------------------------------------------------------
  Update the Reverse Link SilenceParameters in the RMAC & AMAC
  ---------------------------------------------------------------------*/
  hdrrmac_set_silence_params( rl_silence_duration, rl_silence_period );
  hdramac_set_silence_params( rl_silence_duration, rl_silence_period );

}
#endif /* FEATURE_LTE_TO_HDR_OH */
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_MSG

DESCRIPTION
  This function processes messages for OVHD.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */
  uint8 protocol_id;      /* Protocol Identifier */
  boolean free_dsm_item = TRUE; /* Should we free the message buffer? */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /*---------------------------------------------------------------------------
  Get the protocol ID for received message.
  ---------------------------------------------------------------------------*/
  protocol_id = (uint8) HDRDSM_GET_PROT_ID( item_ptr );

  /*---------------------------------------------------------------------------
  Extract the first byte of the received message. This is the message ID.
  ---------------------------------------------------------------------------*/
  if( HDROVHD_MSG_ID( item_ptr, &msg_id ) != E_SUCCESS )
  {
    ERR( "Could not extract msg_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* Currently only the in use instance receives any messages */
  if (protocol_instance != HDRHAI_IN_USE)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* If awaiting_tc_assign_complete is TRUE, meaning HDR SRCH is still
   * processing TC Assign command. Should ignore the OHVD msg since they
   * are received on old channel. 
   */
  if ( hdrfmac_is_active(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) &&
       hdrfmac_get_awaiting_tc_assign_complete() ) 
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Waiting TrafficAssignment to be completed, ignore ovhd msgs");
    HDRTRACE_OVHD_MSG_IGNORED_STATE( msg_id, hdrovhds_get_state() );
    dsm_free_packet( &item_ptr );
    return;
  }

  /*---------------------------------------------------------------------------
  Ignore packets that were decoded while in connected state, but would be
  processed in idle.
  ---------------------------------------------------------------------------*/
  if ( HDRDSM_GET_TRAFFIC_BIT(item_ptr) &&
       !hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL))
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE( msg_id, hdrovhds_get_state() );
    dsm_free_packet( &item_ptr );
    return;
  }

#ifdef FEATURE_LTE_TO_HDR
  /* Until a confirmation is rcvd from SD that LTE stack has been shutdown 
     after a LTE to HDR redirection/reselection success indication is given 
     to SD by CP do not process any ovhd messages. The indication from CP to
     SD tells that DO camping is successful, hence LTE stack can be shutdown 
     and HDR can be set as active RAT. So until this action is completed HDR
     should not update SD with any HDR information. */
  if (hdrmc_lte_to_hdr_ho_is_in_progress() 
#ifdef FEATURE_LTE_TO_HDR_OH
      && (msg_id != HDROVHD_HSP_MSG_ID) 
#endif /* FEATURE_LTE_TO_HDR_OH */
      )
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE( msg_id, hdrovhds_get_state() );
    dsm_free_packet( &item_ptr );
    HDR_MSG_PROT (MSG_LEGACY_MED, 
                 "IRAT: Waiting for CNF from SD, ignoring ovhd msgs ");
    return;
  }
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  /*---------------------------------------------------------------------------
  Determine which message has been received.
  The AccessParameters message is an ACMAC protocol message,but is processed
  by the overhead protocol. To distinguish this message from other overhead
  protocol messages, we check the protocol id field (carried in dsm header)
  along with the Message ID.
  ---------------------------------------------------------------------------*/
  if ( (hdrhai_protocol_type_enum_type) protocol_id == HDRHAI_N_OMP_TYPE )
  {
    switch ( msg_id )
    {
      case HDROVHD_QC_MSG_ID:
        /*---------------------------------------------------------------------
        QuickConfig message
        ---------------------------------------------------------------------*/
        HDRTRACE_OVHD_QC_MSG_RX();

        hdrovhd_process_qc_msg( item_ptr );
        if (hdrovhd.cache_changed)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, "HDRTRACE_OVHD_QC_MSG_RX");
          hdrovhd_log_cache_summary();
        }
        break;

      case HDROVHD_SP_MSG_ID:
        /*---------------------------------------------------------------------
        SectorParameters message.
        ---------------------------------------------------------------------*/
        if( hdrovhd.info_state != HDROVHD_INFO_UPDATED )
        {
          HDRTRACE_OVHD_SP_MSG_RX();
        }

        hdrovhd_process_sp_msg( item_ptr );
        if (hdrovhd.cache_changed)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, "HDROVHD_SP_MSG_ID");
          hdrovhd_log_cache_summary();
        }
        break;

#ifdef FEATURE_HDR_TO_LTE
      case HDROVHD_ORNL_MSG_ID:
        /*---------------------------------------------------------------------
        OtherRAT NeighborList message.
        ---------------------------------------------------------------------*/
        hdrovhd_process_ornl_msg( item_ptr );
        if (hdrovhd.cache_changed)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, "HDRTRACE_OVHD_ORNL_MSG_RX");
          hdrovhd_log_cache_summary();
        }
        break;
#endif /* FEATURE_HDR_TO_LTE*/

#ifdef FEATURE_LTE_TO_HDR_OH
      case HDROVHD_HSP_MSG_ID:
        /*---------------------------------------------------------------------
        HRPDSilenceParameters message.
        ---------------------------------------------------------------------*/
        hdrovhd_process_hsp_msg( item_ptr );
        break;
#endif /* FEATURE_LTE_TO_HDR_OH */

      default:
        /*---------------------------------------------------------------------
        Default: message not recognized.
        ---------------------------------------------------------------------*/
        HDR_MSG_PROT (MSG_LEGACY_LOW, "OVHD: Unrecognized message received");
        break;
    }
  }

  if ( (hdrhai_protocol_type_enum_type) protocol_id == HDRHAI_N_ACMP_TYPE )
  {

    switch( msg_id )
    {
      case HDROVHD_AP_MSG_ID:
        /*---------------------------------------------------------------------
        AccessParameters message
        ---------------------------------------------------------------------*/
        if ( hdrovhd.ap_state != HDROVHD_AP_UPDATED )
        {
          HDRTRACE_OVHD_AP_MSG_RX();
        }

        hdrovhd_process_ap_msg( item_ptr );
        if (hdrovhd.cache_changed)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, "HDROVHD_AP_MSG_ID");
          hdrovhd_log_cache_summary();
        }
        break;

      default :
        /*---------------------------------------------------------------------
        Default: message not recognized. The Overhead Protocol ony processes
        the AccessParameters message. All other Access Channel messages are
        ignored by this protocol.
        ---------------------------------------------------------------------*/
        HDR_MSG_PROT (MSG_LEGACY_LOW, "OVHD: Unrecognized message received");
        break;
    }
  }

  if ( (hdrhai_protocol_type_enum_type) protocol_id == HDRHAI_N_BCMCS_TYPE )
  {

    switch( msg_id )
    {
      case HDROVHD_BC_MSG_ID:
#ifdef FEATURE_HDR_BCMCS
        /*---------------------------------------------------------------------
        BroadcastOverhead message
        ---------------------------------------------------------------------*/
        HDRTRACE_OVHD_BC_MSG_RX();

        free_dsm_item = hdrovhd_process_bc_msg( item_ptr );
        if (hdrovhd.cache_changed)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, "HDROVHD_BC_MSG_ID");
          hdrovhd_log_cache_summary();
        }
#endif /* FEATURE_HDR_BCMCS */
        break;

      default :
        /*---------------------------------------------------------------------
        Default: message not recognized. The Overhead Protocol ony processes
        the AccessParameters message. All other Access Channel messages are
        ignored by this protocol.
        ---------------------------------------------------------------------*/
        HDR_MSG_PROT (MSG_LEGACY_LOW, "OVHD: Unrecognized message received");
        break;
    }
  }

  if (free_dsm_item)
  {
    /*-------------------------------------------------------------------------
    Finished processing message. Free the dsm item containing the message.
    -------------------------------------------------------------------------*/
    dsm_free_packet( &item_ptr );
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
}  /* hdrovhd_process_msg() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_AP_MSG

DESCRIPTION
  This function processes a received AccessParameters message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr -  Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_ap_msg
(
  dsm_item_type *item_ptr
)
{
  sys_link_type         ap_link;
    /* Link on which AP msg rcvd                   */
  hdrsrch_link_id_type  link_id;
    /* Link ID of rcvd msg                         */
  hdrerrno_enum_type    err_number = E_SUCCESS;
    /* Result of unpacking ap msg                  */
  uint16 ap_access_sig;
    /* Holds access sig from ap msg                */
  hdrovhd_cache_entry_type *available_node_ptr = NULL;
    /* used in finding available cache node        */
  hdrovhd_cache_entry_type *curr_cache_entry_ptr;
    /* local copy of current cache entry pointer   */
  hdrovhd_ind_to_give_type ind_to_give;
    /* Indication to be given at end of processing AP msg */
  sd_ss_act_e_type sd_action;      
     /* Continue, acquire HDR, enter power save etc. */
  boolean using_duplicate_node = FALSE;
     /* Flag if we are a second cache entry for double-buffering */
  hdrovhd_ap_entry_type *ap_entry_ptr;
     /* Pointer to where we want to unpack the AP message into */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*-------------------------------------------------------------------------
  If Overhead protocol is not in ProcessAllMsgs state, do not process message
  -------------------------------------------------------------------------*/
  if( hdrovhds_get_state() != HDROVHDS_PROCESS_ALL_MSGS )
  {
    HDRTRACE_OVHD_AP_MSG_IGNORED_STATE(HDROVHD_AP_MSG_ID, hdrovhds_get_state());
    return;
  }

  /*-------------------------------------------------------------------------
  Get the link_id for received message & call the RUP to get the channel &
  pn offset for this link id. Compare the link_id fields with those received
  for previous QuickConfig message. If the fields do not match, then do not
  process message.
  -------------------------------------------------------------------------*/
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  hdrlmac_get_link_info( link_id , &ap_link );

  if( ( !hdrovhd.qc_info.initialized ) ||
      ( ap_link.pilot_pn != hdrovhd.qc_info.link.pilot_pn ) ||
      !SYS_CHAN_CMP_EQUAL(ap_link.channel,hdrovhd.qc_info.link.channel) )
  {
    /* Discard AP msg. Msg rcvd on diff link than QC msg */
    HDRTRACE_OVHD_IGNORED_AP_MSG_UNEXPECTED_LINK(hdrovhd.qc_info.link, 
                                                 ap_link);
    return;
  }

  /*---------------------------------------------------------------------------
  If received Access Signature does not match expected Access Signature
  from previous Quick Config message, do not process the message.
  ---------------------------------------------------------------------------*/
  err_number = HDROVHD_AP_ACCESS_SIGNATURE( item_ptr, &ap_access_sig );

  if (err_number != E_SUCCESS)
  {
    ERR("Discarding AP msg. Error unpacking access sig field (%d)",err_number ,0 ,0);
    return;
  }


  if ( ap_access_sig != hdrovhd.qc_info.access_signature )
  {
    HDRTRACE_OVHD_IGNORED_AP_MSG_DIFF_ACC_SIG(
    hdrovhd.qc_info.access_signature, ap_access_sig );
    return;
  }

  /*---------------------------------------------------------------------------
  If the AccessParameters are already current, there is no need to further
  process the message.
  ---------------------------------------------------------------------------*/
  if ( hdrovhd.ap_state == HDROVHD_AP_UPDATED )
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "AP msg ignored: already stored");
    return;
  }

  /*-------------------------------------------------------------------------
  Initialize ind_to_give parameters
  -------------------------------------------------------------------------*/
  hdrovhd_reset_ind_to_give( &ind_to_give );

  /*---------------------------------------------------------------------------
  Extract & unpack the contents of received AccessParameters message.

  If the ovhd info is updated (i.e. sp message is current in our cache), then
  we copy the incoming AP msg directly into the current ap cache entry if the
  node is not currently being read from.
  ---------------------------------------------------------------------------*/
  if ( hdrovhd.info_state == HDROVHD_INFO_UPDATED )
  {
    ASSERT( curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID );

    /* We shouldn't have any msg in tmp if ovhd is updated */
    if (hdrovhd.ap_buffer.is_valid)
    {
      ERR("Found a valid AP msg in tmp",0,0,0);
      hdrovhd.ap_buffer.is_valid = FALSE;
    }

    /*-------------------------------------------------------------------------
    Check if node is in use, that is, either being read from or written to.
    -> If it is, then the current entry cannot be overwritten since it may
    being read from & overwritting may cause a race condition. Instead, an
    unused node or the oldest entry is overwritten, the current node
    invalidated, and then the current cache msg pointer updated to this node.
    -> If it isn't, then copy the ap msg in the current node directly.
    -------------------------------------------------------------------------*/
    if ( curr_cache_entry_ptr->ap_entry.is_valid &&
         curr_cache_entry_ptr->ap_entry.state != HDROVHD_BUF_NOT_IN_USE )
    {
      available_node_ptr = hdrovhd_find_available_cache_node(
          &curr_cache_entry_ptr->sp_entry.sp_msg);

      /*-----------------------------------------------------------------------
      Make sure that our destination pointer is valid and does not point to
      the current cache entry ptr.
      -----------------------------------------------------------------------*/
      ASSERT(available_node_ptr != NULL &&
             available_node_ptr != curr_cache_entry_ptr );

      /*-----------------------------------------------------------------------
      Make a copy of the current cache node
      -----------------------------------------------------------------------*/
      err_number = hdrovhd_duplicate_cache_entry( curr_cache_entry_ptr,
                                             available_node_ptr );
      if ( err_number != E_SUCCESS )
      {
        HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_process_ap_msg");
        hdrovhd.cache_changed = TRUE;

        ERR("Could not copy cache entry: ap msg not updated (%d)",err_number ,0 ,0);
        curr_cache_entry_ptr->ap_entry.is_valid = FALSE;
        hdrovhd_set_ap_not_updated();
        return;
      }

      /*-----------------------------------------------------------------------
      We have successfully made a duplicate of the cache entry, so we can use
      the new entry as the current entry no matter what happens after this.
      If we fail to unpack the AP message, then the AP message in this new
      node will be invalid, but everything else is the same as before.
      -----------------------------------------------------------------------*/
      hdrovhd.curr_cache_entry_ptr = available_node_ptr;
      hdrovhd_clear_cache_entry( curr_cache_entry_ptr );
      curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;
      ap_entry_ptr = &curr_cache_entry_ptr->ap_entry;
      using_duplicate_node = TRUE;
    }
    else
    /*-------------------------------------------------------------------------
    The node isn't in use, unpack directly in the current node
    -------------------------------------------------------------------------*/
    {
      ap_entry_ptr = &curr_cache_entry_ptr->ap_entry;
    }
    /* will unpack AP msg into current cache entry */
    HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_process_ap_msg");
    hdrovhd.cache_changed = TRUE;
  }
  else
  /*---------------------------------------------------------------------------
  If the ovhd info is not updated, store the AP message in a temp buffer.
  ---------------------------------------------------------------------------*/
  {
    /* unpacking into temp buffer, not a cache update. */
    ap_entry_ptr = &hdrovhd.ap_buffer;
  }

  ap_entry_ptr->link.pilot_pn = ap_link.pilot_pn;
  ap_entry_ptr->link.channel = ap_link.channel;

  err_number = hdrovhd_unpack_ap_msg( item_ptr, ap_entry_ptr );

  if ( err_number == E_SUCCESS )
  {
    if ( hdrovhd.info_state == HDROVHD_INFO_UPDATED )
    {
      /*-----------------------------------------------------------------------
      The AccessParameters are only flagged as "updated" when they reside in
      the cache (ie. when the OVHD info is updated too).  If we store the AP 
      msg temp buffer while we await the corresponding SP msg, the AP msg will
      not be valid yet.
      -----------------------------------------------------------------------*/
      hdrovhd_ap_info_updated( &ind_to_give );
    }

    /*-------------------------------------------------------------------------
    Give necessary indications
    -------------------------------------------------------------------------*/
    hdrovhd_give_ind( &ind_to_give );
  }
  else
  {
    if (using_duplicate_node)
    {
      HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_process_ap_msg");
      hdrovhd.cache_changed = TRUE;
      /*-----------------------------------------------------------------------
      The new AP message was not unpacked correctly, so the AP message in it
      may be in an inconsistent state.
      -----------------------------------------------------------------------*/
      curr_cache_entry_ptr->ap_entry.is_valid = FALSE;
    }

    if (err_number == E_DATA_INVALID)
    {
      sd_action = sd_ss_ind_hdr_opr_bad_sys(SD_SS_BAD_SYS_HDR_INV_AP_CAPSULE_LEN, 
                                            NULL );

      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "SD Action %d", sd_action);

      ASSERT (sd_action != SD_SS_ACT_CONTINUE);
      ind_to_give.need_to_give_ind = TRUE;

      if (sd_action == SD_SS_ACT_ACQ_HDR)
      {
         ind_to_give.ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
      }
      else
      {
         ind_to_give.ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
      }

     /*-----------------------------------------------------------------------
      Give necessary indications
      -----------------------------------------------------------------------*/
      hdrovhd_give_ind( &ind_to_give );
    }
    else
    {
      ERR("Could not extract fields from rxcd AP msg (%d)",err_number ,0 ,0);
    }
  }
} /* hdrovhd_process_ap_msg() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_UPDATE_AP_INFO

DESCRIPTION
  This function marks the AccessParameters as updated and does the processing
  needed along with this.

DEPENDENCIES
  AP should only be updated if the SP message is already updated (or about to be)

PARAMETERS
  ind_to_give_ptr - Pointer to the structure containing information about
                which indication to give at the end of processing the msg.


RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_ap_info_updated
(
  hdrovhd_ind_to_give_type *ind_to_give_ptr
)
{

  uint8   persist_index;      /* Persistance index */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* AP can never be updated unless the SP (OVHD) message already is */
  if ( hdrovhd.ap_state != HDROVHD_AP_UPDATED )
  {
    /*-------------------------------------------------------------------------
    Stop the AP timer
    -------------------------------------------------------------------------*/
    (void) rex_clr_timer( &hdrovhd.ap_supervision_timer );

    /*-------------------------------------------------------------------------
    Stop the AP update wait timer
    -------------------------------------------------------------------------*/

    (void) rex_clr_timer( &hdrovhd.ap_update_wait_timer );

    /*-------------------------------------------------------------------------
    Update current pointer & state
    -------------------------------------------------------------------------*/
    hdrovhd.ap_state = HDROVHD_AP_UPDATED;
    HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Updated AP. ASig:%d",
                    hdrovhd.curr_cache_entry_ptr->ap_entry.ap_msg.access_sig);

    /*-------------------------------------------------------------------------
    Remember the most recent capsule length max to allow upper layers to make 
    a "best estimate" of the largest message size that can be sent on the 
    current access channel.
    -------------------------------------------------------------------------*/
    hdrovhd.last_capsule_length_max =  
      hdrovhd.curr_cache_entry_ptr->ap_entry.ap_msg.capsule_length_max;

    if ( !hdralmp_connection_is_open(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) )
    {
      /*-------------------------------------------------------------------------
      Inform SD if the system is usable based on the latest Access Parameters
      -------------------------------------------------------------------------*/
      persist_index = hdramac_get_persist_index();
      ASSERT(persist_index < NUM_APERSIST);

      if (hdrovhd.curr_cache_entry_ptr->
            ap_entry.ap_msg.apersistence[persist_index] ==
              HDRAMAC_PSIST_NO_ACCESS)
      {
        (void) sd_ss_ind_hdr_opr_sys_usable( SD_SYS_USABLE_NOT_OK, NULL );
      }
      else
      {
        (void) sd_ss_ind_hdr_opr_sys_usable( SD_SYS_USABLE_OK, NULL );
      }
    }

    /*-------------------------------------------------------------------------
    Need to give AP updated indication.  Wait until we are done processing
    messages (qc or sp) to give indication.  May be sent along with the
    OVHD INFO updated indication.
    -------------------------------------------------------------------------*/
    ind_to_give_ptr->need_to_give_ap_ind = TRUE;
  }

} /* hdrovhd_ap_info_updated() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_GET_LAST_CAPSULE_LENGTH_MAX

DESCRIPTION
  This function returns the most recent capsule length max to allow upper
  layers to make a "best estimate" of the largest message size that can be
  sent on the current access channel.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  last_capsule_length_max - most recent capsule_length_max field value

SIDE EFFECTS
  None

=============================================================================*/
uint8 hdrovhd_get_last_capsule_length_max
(
  void
)
{
  return hdrovhd.last_capsule_length_max;
} /* hdrovhd_get_last_capsule_length_max() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_UNPACK_AP_MSG

DESCRIPTION
  This function unpacks and extracts the received AccessParameters message
  into a buffer.

DEPENDENCIES
  Memory should have been allocated for the buffer into which AccessParameters
  are extracted.

PARAMETERS
  item_ptr   - Pointer to the DSM item chain holding the message
  ap_msg_ptr - Pointer to memory into which AccessParameters are stored.

RETURN VALUE
  E_SUCCESS  - If extraction, unpacking of the message succeeds & field values 
             are valid
  E_NOT_AVAILABLE - If the node we are writing to is already in use.
  E_NO_DATA  - If there was an error extracting the message.
  E_DATA_INVALID - If capsule_length_max field value is invalid.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL hdrerrno_enum_type hdrovhd_unpack_ap_msg
(
  dsm_item_type *item_ptr,
  hdrovhd_ap_entry_type *ap_entry_ptr
)
{
  int cnt;        /* Index Counter */
  int err = 0;    /* 0 = Value for E_SUCCESS */
  hdrerrno_enum_type unpack_result;    /* Return value of unpack ap msg */
  uint8 raw_probe_init_adjust; 
                  /* Temporary Buffer for AP msg probe_init_adjust*/
  errno_enum_type err_number; /* return err_number from unpacking */
  uint16 offset; /* offset in packed message to extract from */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Check again to make sure that the destination node isn't in use (this should
  not happen)
  ---------------------------------------------------------------------------*/
  if ( (ap_entry_ptr->is_valid) &&
       ap_entry_ptr->state != HDROVHD_BUF_NOT_IN_USE )
  {
    ERR("Cannot write to cache: AP node is in use",0 ,0 ,0);
    return E_NOT_AVAILABLE;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents. (i.e. Mutex)
  ---------------------------------------------------------------------------*/
  ap_entry_ptr->is_valid = TRUE;
  ap_entry_ptr->state = HDROVHD_BUF_WRITE_IN_PROG;

  /*---------------------------------------------------------------------------
  E_SUCCESS is assumed to have a value of 0.
  The return values from macros are cast to int & OR-ed with the previous error
  If any of the macros return an error, the final value of err will be non-zero
  err is an int type & not hdrerrno_enum_type, because OR-ing two
  hdrerrno_enum_type values can result in an invalid hdrerrno_enum_type value.
  ---------------------------------------------------------------------------*/
  err |= (int) HDROVHD_AP_ACCESS_CYCLE_DURATION( item_ptr,
                                 &ap_entry_ptr->ap_msg.access_cycle_duration );
  err |= (int) HDROVHD_AP_ACCESS_SIGNATURE( item_ptr,
                                 &ap_entry_ptr->ap_msg.access_sig );
  err |= (int) HDROVHD_AP_NOMINAL_POWER( item_ptr,
                                 &ap_entry_ptr->ap_msg.nominal_pwr );
  err |= (int) HDROVHD_AP_PROBE_INITIAL_ADJUST( item_ptr,
                                 &raw_probe_init_adjust );
  ap_entry_ptr->ap_msg.probe_init_adjust =
                 HDROVHD_EXTEND5( raw_probe_init_adjust );

  err |= (int) HDROVHD_AP_PROBE_NUM_STEP( item_ptr,
                                 &ap_entry_ptr->ap_msg.probe_num_step );

  err |= (int) HDROVHD_AP_POWER_STEP( item_ptr,
                                 &ap_entry_ptr->ap_msg.power_step);

  err |= (int) HDROVHD_AP_PREAMBLE_LENGTH( item_ptr,
                                 &ap_entry_ptr->ap_msg.preamble_len);
  err |= (int) HDROVHD_AP_CAPSULE_LENGTH_MAX( item_ptr,
                                 &ap_entry_ptr->ap_msg.capsule_length_max );
  for( cnt=0; cnt < NUM_APERSIST; cnt++ )
  {
      err |= (int) HDROVHD_AP_A_PERSISTENCE( item_ptr,
                                        &ap_entry_ptr->ap_msg.apersistence[cnt],
                                        (HDROVHD_AP_APERSISTENCE_OFFSET  +
                                        (HDROVHD_AP_APERSISTENCE_SIZE * cnt)));
  }

  /* The idea of hiding the offset inside the macros is hard to maintain,
   * so for new fields we won't do that any more.
   */

  offset = HDROVHD_AP_APERSISTENCE_OFFSET + 
           (HDROVHD_AP_APERSISTENCE_SIZE * NUM_APERSIST);

  /* EnhancedAccessParametersIncluded */
  err_number = HDRBIT_UNPACK8( 
    item_ptr, &ap_entry_ptr->ap_msg.enhanced_access_parameters_included, 
    offset, HDROVHD_AP_ENHANCED_ACCESS_PARAMETERS_INCLUDED_SIZE );
  offset += HDROVHD_AP_ENHANCED_ACCESS_PARAMETERS_INCLUDED_SIZE;

  if (err_number == E_NO_DATA)
  {
    ap_entry_ptr->ap_msg.enhanced_access_parameters_included = FALSE;
  }
  else if ( ap_entry_ptr->ap_msg.enhanced_access_parameters_included )
  {
    /* PreambleLengthSlots */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.preamble_length_slots,
      offset, HDROVHD_AP_PREAMBLE_LENGTH_SLOTS_SIZE );
    offset += HDROVHD_AP_PREAMBLE_LENGTH_SLOTS_SIZE;

    /* AccessOffset */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.access_offset,
      offset, HDROVHD_AP_ACCESS_OFFSET_SIZE );
    offset += HDROVHD_AP_ACCESS_OFFSET_SIZE;

    /* SectorAccessMaxRate */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.sector_access_max_rate,
      offset, HDROVHD_AP_SECTOR_ACCESS_MAX_RATE_SIZE );
    offset += HDROVHD_AP_SECTOR_ACCESS_MAX_RATE_SIZE;

    /* ProbeTimeOutAdjust */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.probe_time_out_adjust,
      offset, HDROVHD_AP_PROBE_TIME_OUT_ADJUST_SIZE );
    offset += HDROVHD_AP_PROBE_TIME_OUT_ADJUST_SIZE;

    /* PilotStrengthNominal */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.pilot_strength_nominal,
      offset, HDROVHD_AP_PILOT_STRENGTH_NOMINAL_SIZE );
    offset += HDROVHD_AP_PILOT_STRENGTH_NOMINAL_SIZE;

    /* PilotStrengthCorrectionMin */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.pilot_strength_correction_min,
      offset, HDROVHD_AP_PILOT_STRENGTH_CORRECTION_MIN_SIZE );
    offset += HDROVHD_AP_PILOT_STRENGTH_CORRECTION_MIN_SIZE;

    /* PilotStrengthCorrectionMax */
    err |= HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.pilot_strength_correction_max,
      offset, HDROVHD_AP_PILOT_STRENGTH_CORRECTION_MAX_SIZE );
    offset += HDROVHD_AP_PILOT_STRENGTH_CORRECTION_MAX_SIZE;

    /* MaxPNRandomizationDelayIncluded */
    err_number = HDRBIT_UNPACK8( 
      item_ptr, &ap_entry_ptr->ap_msg.max_pn_randomization_delay_included, 
      offset, HDROVHD_AP_MAX_PN_RANDOMIZATION_DELAY_INCLUDED_SIZE );
    offset += HDROVHD_AP_MAX_PN_RANDOMIZATION_DELAY_INCLUDED_SIZE;

    if (err_number == E_NO_DATA)
    {
      ap_entry_ptr->ap_msg.max_pn_randomization_delay_included = 0;
    }
    else if ( ap_entry_ptr->ap_msg.max_pn_randomization_delay_included )
    {
      /* MaxPNRandomizationDelay */
      err |= HDRBIT_UNPACK8( 
        item_ptr, &ap_entry_ptr->ap_msg.max_pn_randomization_delay,
        offset, HDROVHD_AP_MAX_PN_RANDOMIZATION_DELAY_SIZE );
      offset += HDROVHD_AP_MAX_PN_RANDOMIZATION_DELAY_SIZE;
    }
  }


  /*---------------------------------------------------------------------------
  Check that all field extractions succeeded
  ---------------------------------------------------------------------------*/
  if ( err != 0 )
  {
    ap_entry_ptr->is_valid = FALSE;
    unpack_result = E_NO_DATA;
  }
  else if (ap_entry_ptr->ap_msg.capsule_length_max == 0)
  {
    /*-------------------------------------------------------------------------
    Check capsule_length_max field is valid
    -------------------------------------------------------------------------*/
    ap_entry_ptr->is_valid = FALSE;
    unpack_result = E_DATA_INVALID;
  }
  else
  {
    ap_entry_ptr->is_valid = TRUE;
    ap_entry_ptr->state = HDROVHD_BUF_NOT_IN_USE;
    unpack_result = E_SUCCESS;
  }

  return unpack_result;
} /* hdrovhd_unpack_ap_msg() */

#ifdef FEATURE_HDR_REVC
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_FIND_NLB_CACHE

DESCRIPTION
  This function searchs network load balancing neighbors cache based on
  serving sector. The function can possibily return an invalid/expired
  cache. Caller need to call hdrovhd_nlb_cache_is_valid() to check.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  serving_sector - ptr to serving sector the SP is received

RETURN VALUE
  None.

SIDE EFFECTS

=============================================================================*/
LOCAL hdrovhd_nlb_cache_type* hdrovhd_find_nlb_cache
(
  const sys_link_type* serving_sector
)
{
  uint8 i;
  hdrovhd_nlb_cache_type *cache_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(serving_sector != NULL);
  
  for (i=0;i<HDROVHD_NLB_CACHE_SIZE;i++)
  {
    if (hdrovhd.nlb_cache[i].serving_sector.channel.band ==
        serving_sector->channel.band &&
        hdrovhd.nlb_cache[i].serving_sector.channel.chan_num ==
        serving_sector->channel.chan_num &&
        hdrovhd.nlb_cache[i].serving_sector.pilot_pn ==
        serving_sector->pilot_pn)
    {
      /* exit from loop if found matching neighbor */
      cache_ptr = &hdrovhd.nlb_cache[i];
      break;
    }
  }

  HDR_MSG_PROT_3(MSG_LEGACY_LOW, "find_nlb_cache ch %d pn %d @ %d",
                 serving_sector->channel.chan_num,
				         serving_sector->pilot_pn,
				         i);
  
  return cache_ptr;
}

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_NLB_CACHE_HAS_EXPIRED

DESCRIPTION
  This function checks if the given NLB cache has expired.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  cache_ptr - Pointer to the NLB cache entry to check
  cur_time  - Current time, used to check if cache expired

RETURN VALUE
  True   - if cache_ptr has expired or otherwise invalid
  False  - cache is valid

SIDE EFFECTS
  Sets an expired entry to invalid state

=============================================================================*/
LOCAL boolean hdrovhd_nlb_cache_has_expired
(
  hdrovhd_nlb_cache_type* cache_ptr,
  time_type cur_time
)
{

  ASSERT(cache_ptr != NULL);

  if (cache_ptr->state != HDROVHD_NLB_CACHE_INVALID &&
      hdrutil_calc_duration(cache_ptr->timestamp,
                            cur_time) > 
      HDROVHD_NLB_CACHE_VALID_DURATION_MS)
  {
    cache_ptr->state = HDROVHD_NLB_CACHE_INVALID;
    HDR_MSG_PROT_2(MSG_LEGACY_MED, "nlb ch %d pn %d expired",
                   cache_ptr->serving_sector.channel.chan_num,
                   cache_ptr->serving_sector.pilot_pn);
  }
  
  return (cache_ptr->state == HDROVHD_NLB_CACHE_INVALID);
}

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_FIND_FREE_NLB_CACHE

DESCRIPTION
  This function returns the first invalid cache entry. If there is no
  invalid cache entry, the oldes cache is returned.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Sets found entry to invalid state

=============================================================================*/
LOCAL hdrovhd_nlb_cache_type* hdrovhd_find_free_nlb_cache( void )
{
  uint8 i;
  hdrovhd_nlb_cache_type* cache_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cache_ptr = &hdrovhd.nlb_cache[0];

  for (i=0;i<HDROVHD_NLB_CACHE_SIZE;i++)
  {
    if (hdrovhd.nlb_cache[i].state == HDROVHD_NLB_CACHE_INVALID)
    {
      cache_ptr = &hdrovhd.nlb_cache[i];
      break;
    }

    if (qw_cmp(cache_ptr->timestamp, 
               hdrovhd.nlb_cache[i].timestamp) > 0)
    {
      cache_ptr = &hdrovhd.nlb_cache[i];
    }
  }

  cache_ptr->state = HDROVHD_NLB_CACHE_INVALID;

  HDR_MSG_PROT_1(MSG_LEGACY_MED, "find_free_nlb found %d", i);
  return cache_ptr;
}

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_GET_NLB_ASET_NEIGHBORS

DESCRIPTION
  This function populates pn[] and pos[] array with ASET members
  from NLB cache

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  serving_sector - sector cache is received on
  neighbor_cnt   - number of ASET neighbors returned
  pn             - array to be filled with neighbor PNs
  pos            - ASET's position in SP neighborlist, needed to match
                   LoadInfo msg

RETURN VALUE
  E_SUCCESS      - If success
  E_NO_DATA      - If there is no NLB cache for on given serving sector

SIDE EFFECTS
  None

=============================================================================*/
errno_enum_type hdrovhd_get_nlb_aset_neighbors
( 
  const sys_link_type* serving_sector,
  uint8 *neighbor_cnt,
  sys_pilot_pn_type pn[HDROVHD_SUBASET_PER_CHAN],
  uint8 pos[HDROVHD_SUBASET_PER_CHAN]
)
{
  uint8 i;
  time_type cur_time;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrovhd_nlb_cache_type* cache_ptr;

  ASSERT(serving_sector != NULL);

  cache_ptr = hdrovhd_find_nlb_cache(serving_sector);

  time_get(cur_time);

  if (cache_ptr == NULL ||
      cache_ptr->state != HDROVHD_NLB_CACHE_SP_RCVD ||
      hdrovhd_nlb_cache_has_expired(cache_ptr, cur_time))
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, "get_nlb_aset ch %d pn %d expired",
                   serving_sector->channel.chan_num, 
                   serving_sector->pilot_pn);

    return E_NO_DATA;
  }

  *neighbor_cnt = cache_ptr->aset_cnt;

  HDR_MSG_PROT_1(MSG_LEGACY_MED, "loadadj cnt: %d", 
                 cache_ptr->aset_cnt);

  for (i=0;i<cache_ptr->aset_cnt;i++)
  {
    pos[i] = cache_ptr->aset_neighbors[i];
    pn[i] = cache_ptr->neighbors[pos[i]].pilot_pn;

    HDR_MSG_PROT_2(MSG_LEGACY_MED, "get_nlb_aset %d pn %d",
                   pos[i], pn[i]);
  }

  return E_SUCCESS;
}

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_QC_FOR_NLB

DESCRIPTION
  This function searches NLB cache based on received QC. If no cache exist,
  one is created.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  qc_info_ptr pointer to QC info structure

RETURN VALUE
  None.

SIDE EFFECTS
  Updates cache entry and may delete older caches

=============================================================================*/
LOCAL void hdrovhd_process_qc_for_nlb
(
  const hdrovhd_qc_info_type* qc_info_ptr
)
{
  hdrovhd_nlb_cache_type* cache_ptr;
  time_type cur_time;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(qc_info_ptr != NULL);
  
  cache_ptr = hdrovhd_find_nlb_cache(&qc_info_ptr->link);

  if (cache_ptr == NULL)
  {
    cache_ptr = hdrovhd_find_free_nlb_cache();
    ASSERT(cache_ptr != NULL);

#ifdef FEATURE_MEMCPY_REMOVAL
    cache_ptr->serving_sector = qc_info_ptr->link;
#else
    memcpy(&cache_ptr->serving_sector,
           &qc_info_ptr->link,
           sizeof(sys_link_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
  }
  
  time_get(cur_time);

  if (cache_ptr->color_code != qc_info_ptr->color_code ||
      cache_ptr->sector24 != qc_info_ptr->sector_id_24 ||
      cache_ptr->sector_sig != qc_info_ptr->sector_signature ||
      hdrovhd_nlb_cache_has_expired(cache_ptr, cur_time))
  {
    cache_ptr->color_code = qc_info_ptr->color_code;
    cache_ptr->sector24 = qc_info_ptr->sector_id_24;
    cache_ptr->sector_sig = qc_info_ptr->sector_signature;
    cache_ptr->state = HDROVHD_NLB_CACHE_QC_RCVD;

    cache_ptr->neighbor_cnt = 0;
    cache_ptr->aset_cnt = 0;
  }

  HDR_MSG_PROT_5(MSG_LEGACY_MED, "proc_qc_nbr ch %d pn %d st %d nbr# %d aset# %d",
                 qc_info_ptr->link.channel.chan_num,
                 qc_info_ptr->link.pilot_pn,
                 cache_ptr->state,
                 cache_ptr->neighbor_cnt, 
                 cache_ptr->aset_cnt);

  qw_equ(cache_ptr->timestamp, cur_time);
}
#endif /* FEATURE_HDR_REVC */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_QC_MSG

DESCRIPTION
  This function processes a received QuickConfig message.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_process_qc_msg
(
  dsm_item_type *item_ptr
)
{

  hdrsrch_link_id_type link_id;     /* Link ID for received message */
  hdrovhd_qc_info_type qc_info;     /* Stores unpacked QC msg fields */
  hdrovhd_ind_to_give_type ind_to_give;
              /* Indication to be given at end of processing QC msg */
  hdrovhds_state_enum_type cur_state = hdrovhds_get_state();

#ifdef FEATURE_HDR_REVC
  hdrerrno_enum_type err;
  sys_channel_type *pri_cc_chan;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Msgs are processed in the Process All msg state. If protocol is inactive or
  sleep or if freq change or access HO is in progress, discard the rcvd msg.
  If in wait for link state, check if the QC msg is rcvd on the expected link.
  If so change state to process all msg & continue processing.
  ---------------------------------------------------------------------------*/
  if ((cur_state != HDROVHDS_PROCESS_ALL_MSGS) &&
      (cur_state != HDROVHDS_WAIT_FOR_LINK))
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE( HDROVHD_QC_MSG_ID,
                                     hdrovhds_get_state() );
    return;
  }

  /* Initialize the local variable */
  memset( &qc_info, 0, sizeof(qc_info) );

  /*---------------------------------------------------------------------------
  Get link_id from dsm item
  ---------------------------------------------------------------------------*/
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  /*---------------------------------------------------------------------------
  Call LMAC to get link information (ie channel & pn) on which the msg is rcvd
  ---------------------------------------------------------------------------*/
  hdrlmac_get_link_info( link_id, &qc_info.link );

  /*---------------------------------------------------------------------------
  Check if Okay to process received QC message
  ---------------------------------------------------------------------------*/
  if(!hdrovhd_qc_is_ok_to_process( &qc_info.link ))
  {
    /* Discard QC msg. Note: Trace is printed by hdrovhd_qc_is_ok_to_process */
    return;
  }

#ifdef FEATURE_HDR_REVC
  pri_cc_chan = hdrlmac_get_cc_monitoring_chan();
  ASSERT(pri_cc_chan != NULL);
  /* if QC is received from non primary control channel */
  if ((pri_cc_chan->band != qc_info.link.channel.band ||
       pri_cc_chan->chan_num != qc_info.link.channel.chan_num))
  {
    if (hdralmps_get_state() != HDRALMPS_CONNECTED)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_MED, "proc_qc QC on non pri CC chan %d, in wrong state %d",
                     qc_info.link.channel.chan_num, hdralmps_get_state());
      return;
    }

    err = E_SUCCESS;
    err |= HDROVHD_QC_COLOR_CODE( item_ptr, &qc_info.color_code );
    err |= HDROVHD_QC_SECTOR_ID_24( item_ptr, &qc_info.sector_id_24 );
    err |= HDROVHD_QC_SECTOR_SIGNATURE( item_ptr, 
                                        &qc_info.sector_signature);

    if (err == E_SUCCESS)
    {
      hdrovhd_process_qc_for_nlb(&qc_info);
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Error %d extracting QC for NLB", err);
    }

    return;
  }
#endif /* FEATURE_HDR_REVC */

  /*---------------------------------------------------------------------------
    If is_first_qp_after_idle_ho is still true when we receive the QC message 
    it means that we did not recieve QP message at all (possibly because of 
    erasure) in this case we just process QC message as in non QP idle hand 
    off case.
    -------------------------------------------------------------------------*/
  if ( hdrovhd.is_first_qp_after_idle_ho )
  {
    hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
    hdrovhd_set_ap_not_updated();

    hdrovhd.is_first_qp_after_idle_ho = FALSE;
  }

  /*---------------------------------------------------------------------------
  Restart the QuickConfig timer
  ---------------------------------------------------------------------------*/
  (void) rex_set_timer(&hdrovhd.qc_supervision_timer , HDROVHD_QC_TIMER_MSECS);

  /* This flag is used to check the sleep condition */
  hdrovhd_set_qc_is_received(TRUE);

  /*---------------------------------------------------------------------------
  Unpack received message
  ---------------------------------------------------------------------------*/
  if( hdrovhd_unpack_qc_msg( item_ptr , &qc_info ) != E_SUCCESS )
  {
    ERR("Error Extracting fields from rxcd QC msg", 0, 0, 0 );
    return;
  }

  /*---------------------------------------------------------------------------
  Initialize the indication data structure.
  ---------------------------------------------------------------------------*/
  hdrovhd_reset_ind_to_give( &ind_to_give );

  /*---------------------------------------------------------------------------
  Process the unpacked msg
  ---------------------------------------------------------------------------*/
  if( hdrovhd_process_unpacked_qc_msg( &qc_info , &ind_to_give ) )
  {
    /*-------------------------------------------------------------------------
    If system determination does not cause to change or exit the current HDR
    system, then store Received fields & link information
    -------------------------------------------------------------------------*/
    hdrovhd_update_curr_qc_info( &qc_info , link_id );

    /* qc data valid; reset qc insanity timer */
    (void) rex_set_timer( &hdrovhd.qc_insanity_timer, 
                          HDROVHD_INSANITY_TIMER_MSECS );
  }

#ifdef FEATURE_HDR_REVC
  hdrovhd_process_qc_for_nlb(&qc_info);
#endif /* FEATURE_HDR_REVC */

  /*---------------------------------------------------------------------------
  Give necessary indications
  ---------------------------------------------------------------------------*/
  hdrovhd_give_ind( &ind_to_give );

} /* hdrovhd_process_qc_msg() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_QC_IS_OKAY_TO_PROCESS

DESCRIPTION
  This function checks various conditions on receiving a QC msg & determines
  whether it is okay to unpack & process the QC msg further or whether it
  should just be ignored.
  If OVHD state = ProcessAllMsgs, then the msg is okay to be processed further.
  If OVHD state = WaitForExpectedLink & the link on which QC msg is received is
  the expected one, then it is okay to process further
  For all other states the QC msg should be ignored.

DEPENDENCIES
  This function should be called only when OMP is in PROCESS_ALL_MSG or
  WAIT_FOR_LINK state.

PARAMETERS
  qc_link  - Link on which the QC msg was received.

RETURN VALUE
  TRUE  - If Alright to unpack and process the received QC msg
  FALSE - Otherwise

SIDE EFFECTS
  None

=============================================================================*/
LOCAL boolean hdrovhd_qc_is_ok_to_process
(
  sys_link_type *qc_link_ptr
)
{
  boolean ret_val; /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret_val = FALSE;

  /*---------------------------------------------------------------------------
  Msgs are processed in the Process All msg state. If protocol is inactive or
  sleep or if freq change or access HO is in progress, discard the rcvd msg.
  If in wait for link state, check if the QC msg is rcvd on the expected link.
  If so change state to process all msg & continue processing.
  ---------------------------------------------------------------------------*/
  switch( hdrovhds_get_state() )
  {
    case HDROVHDS_PROCESS_ALL_MSGS:
      /*-----------------------------------------------------------------------
      Continue with processing of message.
      -----------------------------------------------------------------------*/
      ret_val = TRUE;
      break;

    case HDROVHDS_WAIT_FOR_LINK:
      /*-----------------------------------------------------------------------
      Check if expected QC msg rcvd on the expected link
      -----------------------------------------------------------------------*/
      if((qc_link_ptr->pilot_pn != hdrovhd.expected_link.pilot_pn)||
         !SYS_CHAN_CMP_EQUAL( qc_link_ptr->channel,
                              hdrovhd.expected_link.channel))
      {
        /*---------------------------------------------------------------------
        Discard message. Stop processing it.
        ---------------------------------------------------------------------*/
        HDRTRACE_OVHD_IGNORED_MSG_UNEXPECTED_LINK(HDROVHD_QC_MSG_ID,
        hdrovhd.expected_link.channel.chan_num,
        hdrovhd.expected_link.pilot_pn,
        qc_link_ptr->channel.chan_num,
        qc_link_ptr->pilot_pn );

        ret_val = FALSE;
      }
      else
      {
        /*---------------------------------------------------------------------
        Change state to process all msg & Continue with processing of message.
        ---------------------------------------------------------------------*/
        hdrovhds_change_state( HDROVHDS_PROCESS_ALL_MSGS );
        ret_val = TRUE;
      }
      break;

    case HDROVHDS_INACTIVE:
    case HDROVHDS_SLEEP:
    case HDROVHDS_FREQ_CHANGE_IN_PROG:
    case HDROVHDS_ACC_HO_IN_PROG:
      /*-----------------------------------------------------------------------
      Message should have been already discarded.
      -----------------------------------------------------------------------*/
      ret_val = FALSE;
      ERR("Message should have been already discarded.", 0, 0, 0 );
      break;

    default:
      /*-----------------------------------------------------------------------
      Discard message. Stop processing it.
      -----------------------------------------------------------------------*/
      ERR_FATAL("hdrovhds_get_state returned unknown value", 0, 0, 0 );
  }

  return ret_val;
} /* hdrovhd_qc_is_ok_to_process() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_UNPACK_QC_MSG

DESCRIPTION
  This function unpacks and extracts the received QuickConfig message
  into a buffer.

DEPENDENCIES
  Memory should have been allocated for the buffer into which QuickConfig msg
  is extracted.

PARAMETERS
  item_ptr    - Pointer to the DSM item chain holding the message
  qc_info_ptr - Pointer to memory into which QuickConfig msg is stored.

RETURN VALUE
  E_SUCCESS  - If extraction & unpacking of message succeeds.
  E_NO_DATA  - Otherwise.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL hdrerrno_enum_type hdrovhd_unpack_qc_msg
(
  dsm_item_type        *item_ptr,
  hdrovhd_qc_info_type *qc_info_ptr
)
{

  int err;
    /* Error value. 0 = E_SUCCESS */

  uint8 user_count = 0;
    /* Number of users based on FTValid=1 */
  uint32 fwd_traffic_valid_half = 0;
    /* to break down the fwd_traffic_valid qword into 32-bit fields */
  uint8 i;
    /* iterator */
  errno_enum_type err_number; /* return err_number from unpacking */
  int offset; /* offset in packed message to extract from */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------------
  Extract QC message fields into local variables.
  E_SUCCESS is assumed to have a value of 0.
  The return values from macros are cast to int & OR-ed with the previous error
  If any of the macros return an error, the final value of err will be non-zero
  err is an int type & not hdrerrno_enum_type, because OR-ing two
  hdrerrno_enum_type values can result in an invalid hdrerrno_enum_type value.
  ---------------------------------------------------------------------------*/
  err = 0;           /* 0 is the value of E_SUCCESS */
  err |= (int) HDROVHD_QC_COLOR_CODE( item_ptr, &qc_info_ptr->color_code );
  err |= (int) HDROVHD_QC_SECTOR_ID_24( item_ptr, &qc_info_ptr->sector_id_24 );
  err |= (int) HDROVHD_QC_SECTOR_SIGNATURE( item_ptr,
                   &qc_info_ptr->sector_signature);
  err |= (int) HDROVHD_QC_ACCESS_SIGNATURE( item_ptr,
                   &qc_info_ptr->access_signature );
  err |= (int) HDROVHD_QC_REDIRECT( item_ptr, &qc_info_ptr->redirect );
  err |= (int) HDROVHD_QC_RPC_COUNT63TO0( item_ptr, 
                                          &qc_info_ptr->rpc_count63to0 );

  if (qc_info_ptr->rpc_count63to0 == 0)
  {
    qw_set( qc_info_ptr->fwd_traffic_valid63to0, 0, 0 );
  }
  else
  {
    err |= (int) HDROVHD_QC_FWD_TRAFFIC_VALID63TO0( item_ptr,
                                          qc_info_ptr->fwd_traffic_valid63to0,
                                          qc_info_ptr->rpc_count63to0 );

    /* Left justify Forward Traffic Valid field. */
    qw_shift( qc_info_ptr->fwd_traffic_valid63to0, 
              (64-qc_info_ptr->rpc_count63to0) );
  }

  offset = HDROVHD_QC_RPC_COUNT63TO0_OFFSET + qc_info_ptr->rpc_count63to0;

  /*---------------------------------------------------------------------------
  RelA Fields
  ---------------------------------------------------------------------------*/

  err_number = HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->rpc_count127to64_included, 
                          offset,  HDROVHD_QC_RPC_COUNT_INCLUDED_SIZE );

  offset += HDROVHD_QC_RPC_COUNT_INCLUDED_SIZE;

  if (err_number == E_NO_DATA)
  {
    qc_info_ptr->rpc_count127to64_included = FALSE;
    qc_info_ptr->rpc_count127to64 = 0;
    qw_set( qc_info_ptr->fwd_traffic_valid127to64, 0, 0 );
  }
  else if ( qc_info_ptr->rpc_count127to64_included )
  {
    err |= (int) HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->rpc_count127to64, 
                                 offset, HDROVHD_QC_RPC_COUNT127TO64_SIZE );

    offset += HDROVHD_QC_RPC_COUNT127TO64_SIZE;

    if (qc_info_ptr->rpc_count127to64 == 0)
    {
      qw_set( qc_info_ptr->fwd_traffic_valid127to64, 0, 0 );
    }
    else
    {

      err |= (int) HDRBIT_UNPACK64( item_ptr, 
                                    qc_info_ptr->fwd_traffic_valid127to64, 
                                    offset, qc_info_ptr->rpc_count127to64 );

      offset += qc_info_ptr->rpc_count127to64;

      /* Left justify Forward Traffic Valid field. */
      qw_shift( qc_info_ptr->fwd_traffic_valid127to64, 
                (64-qc_info_ptr->rpc_count127to64) );
    }

    HDR_MSG_PROT( MSG_LEGACY_MED, "QC. Includes Rel. A Fields" );
  }
  else
  {
    qc_info_ptr->rpc_count127to64 = 0;
    qw_set( qc_info_ptr->fwd_traffic_valid127to64, 0, 0 );
    HDR_MSG_PROT( MSG_LEGACY_MED, "QC. Includes Rel. A Fields" );
  }


  /*---------------------------------------------------------------------------
  RelB Fields
  ---------------------------------------------------------------------------*/
  err_number = HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->rpc_count130to383_included, 
                          offset,  HDROVHD_QC_RPC_COUNT_INCLUDED_SIZE );

  offset += HDROVHD_QC_RPC_COUNT_INCLUDED_SIZE;

  if (err_number == E_NO_DATA)
  {
    qc_info_ptr->rpc_count130to383_included = FALSE;
    qc_info_ptr->rpc_count130to383 = 0;
    /* fwd_traffic_valid130to383 */
  }
  else if ( qc_info_ptr->rpc_count130to383_included )
  {                             
    err |= (int) HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->rpc_count130to383, 
                                 offset, HDROVHD_QC_RPC_COUNT130TO383_SIZE );

    offset += HDROVHD_QC_RPC_COUNT130TO383_SIZE;

    if (qc_info_ptr->rpc_count130to383 == 0)
    {
      /* fwd_traffic_valid130to383 */
    }
    else
    {
      /* Unpack fwd_traffic_valid130to383 */
      offset += qc_info_ptr->rpc_count130to383;
    }

    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "QC. Includes RelB Fields - ignore RPCCnt130to383, not supported" );
    /* Don't bother unpacking right now - ignore field */

  }
  else
  {
    qc_info_ptr->rpc_count130to383 = 0;
  }

  if( err == 0 )
  {
    /* Tally the number of users based on the number of FTValid=1 fields */

    /* tally the upper 32 'on' bits of the fwd_traffic_valid bit field */
    fwd_traffic_valid_half = qw_hi(qc_info_ptr->fwd_traffic_valid63to0);
    for (i=0; i<32; i++)
    {
      if (( fwd_traffic_valid_half & 1) == 1) user_count++;
      fwd_traffic_valid_half >>= 1;
    }

    /* tally the lower 32 'on' bits of the fwd_traffic_valid bit field */
    fwd_traffic_valid_half = qw_lo(qc_info_ptr->fwd_traffic_valid63to0);
    for (i=0; i<32; i++)
    {
      if (( fwd_traffic_valid_half & 1) == 1) user_count++;
      fwd_traffic_valid_half >>= 1;
    }

    if ( qc_info_ptr->rpc_count127to64 )
    {
      /* tally the upper 32 'on' bits of the fwd_traffic_valid bit field */
      fwd_traffic_valid_half = qw_hi(qc_info_ptr->fwd_traffic_valid127to64);
      for (i=0; i<32; i++)
      {
        if (( fwd_traffic_valid_half & 1) == 1) user_count++;
        fwd_traffic_valid_half >>= 1;
      }

      /* tally the lower 32 'on' bits of the fwd_traffic_valid bit field */
      fwd_traffic_valid_half = qw_lo(qc_info_ptr->fwd_traffic_valid127to64);
      for (i=0; i<32; i++)
      {
        if (( fwd_traffic_valid_half & 1) == 1) user_count++;
        fwd_traffic_valid_half >>= 1;
      }
    }

    /* TODO:
       - Tally up FTValid130to383 when supported
       - Both RMAC and OVHD do the same tally - look into consolidating this.
         Ovhd only does it to print the user count.  Have the MAC print that
         info or add an interface.
     */
    if ( qc_info_ptr->rpc_count130to383 > 0 )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"Not tallying relB FTValid info");
    }

#ifdef FEATURE_HDR_TO_LTE
    /*---------------------------------------------------------------------------
    OtherRATNeighborList fields
    ---------------------------------------------------------------------------*/
    err_number = (int) HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->other_rat_available, 
                                 offset, HDROVHD_QC_OTHER_RAT_AVAILABLE_SIZE );
    offset += HDROVHD_QC_OTHER_RAT_AVAILABLE_SIZE;

    if (err_number == E_NO_DATA)
    {
      qc_info_ptr->other_rat_txcycle = 0xFF;
      qc_info_ptr->other_rat_signature = 0;
    }
    else if (qc_info_ptr->other_rat_available)
    {
      err |= (int) HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->other_rat_txcycle, 
                                   offset, HDROVHD_QC_OTHER_RAT_TX_CYCLE_SIZE );
      offset += HDROVHD_QC_OTHER_RAT_TX_CYCLE_SIZE;

      err |= (int) HDRBIT_UNPACK8( item_ptr, &qc_info_ptr->other_rat_signature, 
                                   offset, HDROVHD_QC_OTHER_RAT_SIGNATURE_SIZE );
      offset += HDROVHD_QC_OTHER_RAT_SIGNATURE_SIZE;
    }
    else
    {
      qc_info_ptr->other_rat_txcycle = 0xFF;
      qc_info_ptr->other_rat_signature = 0;
    }
#endif /* FEATURE_HDR_TO_LTE*/

    HDR_MSG_PROT_5 (MSG_LEGACY_MED,
                    "QC. SecID24:%d CCode:%d SSig:%d ASig:%d Users:%d",
                    qc_info_ptr->sector_id_24,
                    qc_info_ptr->color_code,
                    qc_info_ptr->sector_signature,
                    qc_info_ptr->access_signature,
                    user_count);

    /*-------------------------------------------------------------------------
    Log the number of users information received in QuickConfig messages.
    We track the accumulated user count and the number of messages received
    for averaging purposes.
    -------------------------------------------------------------------------*/
    hdrplog_inc_conn_state_stats_counter(HDRPLOG_TOTAL_USERS_COUNT, 
                                         user_count);
    hdrplog_inc_conn_state_stats_counter(HDRPLOG_USERS_MESSAGES_COUNT, 1);
  }

  return (hdrerrno_enum_type) ((err != 0) ? E_NO_DATA : E_SUCCESS) ;

} /* hdrovhd_unpack_qc_msg() */

/* <EJECT> */


/*=============================================================================

FUNCTION HDROVHD_FIND_CACHED_MSG_USING_QC_INFO

DESCRIPTION
  This function is used to find the cached overhead information when 
  QuickConfig message is received.

DEPENDENCIES
  None

PARAMETERS
  link_info - Link on which the message was received.
  color_code - Color code in received QC Message.
  sector_id_24 - SectorID24 in received QC Message.
  sector_signature - Sector Sig in received QC Message.

RETURN VALUE
  NULL if valid matching Sector Parameters message is not found.
  Otherwise returns pointer to node containing the SP msg.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL hdrovhd_cache_entry_type *hdrovhd_find_cached_msg_using_qc_info
(
  sys_link_type link_info,
  uint8  color_code,
  uint32 sector_id_24,
  uint16 sector_signature
)
{
  hdrovhd_cache_entry_type *found_cached_msg_ptr = NULL;
    /* Cache entry Pointer */
  hdrovhd_cache_key_info_type cache_key_info;
    /* Info to find the cached entry */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &cache_key_info, 0, sizeof (hdrovhd_cache_key_info_type) );

  cache_key_info.qc_info.link_info = link_info;
  cache_key_info.qc_info.color_code = color_code;
  cache_key_info.qc_info.sector_id_24 = sector_id_24;
  cache_key_info.qc_info.sector_signature = sector_signature; 

  found_cached_msg_ptr = hdrovhd_find_cached_msg(
                           cache_key_info,
                           HDROVHD_CACHE_KEY_QC_INFO);

  return found_cached_msg_ptr;

} /* hdrovhd_find_cached_msg_using_qc_info() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_FIND_CACHED_MSG_USING_SP_INFO

DESCRIPTION
  This function is used to find the cached overhead information when 
  SectorParameters message is received. Sectord128 and channel are 
  used to find the cached information.

DEPENDENCIES
  None

PARAMETERS
  sector_id_128 - Sector ID from received QC msg
  channel - channel on which message was received.

RETURN VALUE
  NULL if valid matching Sector Parameters message is not found.
  Otherwise returns pointer to node containing the SP msg.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL hdrovhd_cache_entry_type *hdrovhd_find_cached_msg_using_sp_info
(
  hdraddr_type sp_sector_id_128,
  uint16 sector_signature,
  sys_channel_type channel
)
{
  hdrovhd_cache_entry_type *found_cached_msg_ptr = NULL;
    /* Cache entry Pointer */
  hdrovhd_cache_key_info_type cache_key_info;
    /* Info to find the cached entry */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset ( &cache_key_info, 0, sizeof(hdrovhd_cache_key_info_type) );

  cache_key_info.sp_info.channel = channel;
  cache_key_info.sp_info.sector_signature = sector_signature;

#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy (cache_key_info.sp_info.sp_sector_id_128,
            sizeof(cache_key_info.sp_info.sp_sector_id_128), 
            sp_sector_id_128,
            HDRADDR_LENGTH_IN_BYTES);
#else
  memcpy (cache_key_info.sp_info.sp_sector_id_128,
          sp_sector_id_128,
          HDRADDR_LENGTH_IN_BYTES);
#endif /* FEATURE_MEMCPY_REMOVAL */

  found_cached_msg_ptr = hdrovhd_find_cached_msg(
                           cache_key_info,
                           HDROVHD_CACHE_KEY_SP_INFO);

  return found_cached_msg_ptr;

} /* hdrovhd_find_cached_msg_using_sp_info() */

#ifdef FEATURE_HDR_REVC
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_UNPACK_SP_MSG_NEIGHBORS

DESCRIPTION
  This function unpacks Neighbors inside a SectorParameters message.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  E_SUCCESS - If unpacking succeded
  E_NO_DATA - If unpacking failed

SIDE EFFECTS 
  neighbors structure is modified regardless even when unpack failed 

=============================================================================*/
LOCAL hdrerrno_enum_type hdrovhd_unpack_sp_msg_neighbors
(
  dsm_item_type *item_ptr,
  sys_link_type *serving_sector,
  hdrovhd_sp_msg_type *sp_ptr
)
{
  uint8 i;
  uint8 temp;
  uint16 pos;
  hdrerrno_enum_type err;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* skip to channel count */
  pos = HDROVHD_SP_CHANNEL_COUNT_OFFSET;

  /* unpack channel count and skip channel list */

  sp_ptr->channel_count = 0;
  err = HDRBIT_UNPACK8(item_ptr, &sp_ptr->channel_count, pos, 
                       HDROVHD_SP_CHANNEL_COUNT_SIZE);
  
  pos += HDROVHD_SP_CHANNEL_COUNT_SIZE + 
         sp_ptr->channel_count * HDROVHD_SP_CHANNEL_RECORD_SIZE;

  /* unpack neighbor list */
  sp_ptr->num_neighbors = 0;
  err |= HDRBIT_UNPACK8(item_ptr, &sp_ptr->num_neighbors, pos, 
                        HDROVHD_SP_NEIGHBOR_COUNT_SIZE);
  pos += HDROVHD_SP_NEIGHBOR_COUNT_SIZE;
  
  /* unpack PN info for neighbor list */
  for (i=0;i<sp_ptr->num_neighbors;i++)
  {
    HDRBIT_UNPACK16(item_ptr, &sp_ptr->nset[i].link.pilot_pn, pos, 
                    HDROVHD_SP_NEIGHBOR_PN_SIZE);
    pos += HDROVHD_SP_NEIGHBOR_PN_SIZE;
  }

  /* unpack channel info for neighbor list */
  for (i=0;i<sp_ptr->num_neighbors;i++)
  {
    temp = 0;
    err |= HDRBIT_UNPACK8(item_ptr, &temp, pos, 
                          HDROVHD_SP_NEIGHBOR_CHAN_INCL_SIZE);
    pos += HDROVHD_SP_NEIGHBOR_CHAN_INCL_SIZE;

    if (temp)
    {
      err |= HDRBIT_UNPACK8(item_ptr, &temp, pos, 
                             HDROVHD_SP_SYSTEM_TYPE_SIZE);
      sp_ptr->nset[i].link.mode = (sys_hdr_chan_rec_mode_e_type) temp;
      pos += HDROVHD_SP_SYSTEM_TYPE_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &temp, pos, 
                             HDROVHD_SP_BAND_CLASS_SIZE);
      sp_ptr->nset[i].link.channel.band = (sys_band_class_e_type) temp;
      pos += HDROVHD_SP_BAND_CLASS_SIZE;

      err |= HDRBIT_UNPACK16(item_ptr, 
                             &sp_ptr->nset[i].link.channel.chan_num, pos, 
                            HDROVHD_SP_CHANNEL_NUMBER_SIZE);
      pos += HDROVHD_SP_CHANNEL_NUMBER_SIZE;
    }
    else
    {
      sp_ptr->nset[i].link.mode = serving_sector->mode;
      sp_ptr->nset[i].link.channel = serving_sector->channel;
    }
	
	  HDR_MSG_PROT_4(MSG_LEGACY_MED, "unpack_sp_nbr chan %d pn %d @ %d err %d",
	                 sp_ptr->nset[i].link.channel.chan_num,
			        	   sp_ptr->nset[i].link.pilot_pn,
				           pos, err);
  }
  
  return err;
}

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_SP_NEIGHBORS

DESCRIPTION
  This function copies unpacked SP neighbors into NLB cache

DEPENDENCIES
  None

PARAMETERS
  serving_sect_ptr        - Ptr to sector the SP msg is received
  hdrovhd_nlb_cache_type  - Ptr to NLB cache entry to copy into
  sp_ptr                  - Ptr to unpacked SP structure

RETURN VALUE
  None.

SIDE EFFECTS

=============================================================================*/
LOCAL void hdrovhd_process_sp_neighbors
(
  const sys_link_type* serving_sect_ptr,
  hdrovhd_nlb_cache_type* cache_ptr,
  const hdrovhd_sp_msg_type* sp_ptr
)
{
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  ASSERT(serving_sect_ptr != NULL &&
         cache_ptr != NULL &&
         sp_ptr != NULL);

  cache_ptr->neighbor_cnt = sp_ptr->num_neighbors;

  for (i=0; i<sp_ptr->num_neighbors; i++)
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    cache_ptr->neighbors[i] = sp_ptr->nset[i].link;
#else
    memcpy(&cache_ptr->neighbors[i], 
           &sp_ptr->nset[i].link,
           sizeof(sys_link_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    HDR_MSG_PROT_3(MSG_LEGACY_LOW,
                   "proc_sp_nlb ch %d pn %d @ %d",
                   cache_ptr->neighbors[i].channel.chan_num,
                   cache_ptr->neighbors[i].pilot_pn, i);
  }

  (void) hdrovhd_reparse_aset_neighbors(cache_ptr);

  cache_ptr->state = HDROVHD_NLB_CACHE_SP_RCVD;

  hdrind_give_ind(HDRIND_OVHD_NLB_CACHE_UPDATED, 
                  (void*) serving_sect_ptr);
}
#endif /* FEATURE_HDR_REVC */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_PROCESS_UNPACKED_QC_MSG

DESCRIPTION
  This function does the processing for the unpacked QC msg. It updates the
  cache if needed, updates the expected SP & AP msgs & the SP & AP info
  states. It also informs system determination if the QC msg received is the
  first one received after activation.

DEPENDENCIES
  Memory should have been allocated for the buffer into which QuickConfig msg
  is extracted.

PARAMETERS
  qc_info_ptr - Pointer to memory into which QuickConfig msg is stored.
  ind_to_give_ptr - Pointer to the structure containing information about
                which indication to give at the end of processing the msg.

RETURN VALUE
  TRUE  - If Access Terminal is allowed to continue on current HDR system
  FALSE - If AT has to change to another HDR system or exit HDR system

SIDE EFFECTS
  AT may have to leave current HDR system

=============================================================================*/
LOCAL boolean hdrovhd_process_unpacked_qc_msg
(
  const hdrovhd_qc_info_type *qc_info_ptr,
  hdrovhd_ind_to_give_type *ind_to_give_ptr
)
{
  hdrovhd_cache_entry_type *cache_entry_ptr = NULL; /* Pointer to cache */
  boolean continue_curr_hdr_system; /* Keeps track of if system determination
                                       causes to change or exit the current HDR
                                       system */
  hdraddr_type sector_id_128;       /* SectorID128 = SubnetID|SectorID24 */
  boolean chan_was_changed;
  uint16 st_cache_period = 0;

#ifdef FEATURE_HDR_REVC
  hdrovhd_nlb_cache_type* nlb_cache_ptr;
#endif /* FEATURE_HDR_REVC */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  continue_curr_hdr_system = TRUE;

#ifdef FEATURE_HDR_REVC
    hdrovhd_process_qc_for_nlb(qc_info_ptr);
#endif /* FEATURE_HDR_REVC */

  /*---------------------------------------------------------------------------
  Update the state of the stored SectorParameters in the following case:
  - if this is the first QuickConfig message
  - if either the SectorID, ColorCode or SectorSignature has changed
  - if the message is received on a channel different from the previous one
  - if the ovhd info is not updated (for instance, if we do an AHO but stay on
    the same sector, we will want to retrieve the SP/AP msgs right away and not
    have to wait for an extra sync capsule for the SP to be updated again)
  If matching SectorParameters are found stored in the
  SP cache, then update their timestamp & give an indication.
  ---------------------------------------------------------------------------*/
  if( ( !hdrovhd.qc_info.initialized ) ||
      ( hdrovhd.info_state != HDROVHD_INFO_UPDATED) ||
      ( hdrovhd.qc_info.sector_id_24 != qc_info_ptr->sector_id_24 ) ||
      ( hdrovhd.qc_info.color_code != qc_info_ptr->color_code ) ||
      ( hdrovhd.qc_info.sector_signature != qc_info_ptr->sector_signature) ||
      !SYS_CHAN_CMP_EQUAL( hdrovhd.qc_info.link.channel,
                           qc_info_ptr->link.channel ) ||
      !hdrovhd_longterm_cache_entry_is_valid(hdrovhd.curr_cache_entry_ptr, 
                                             HDROVHD_CACHE_KEY_QC_INFO)
      )
  {
    /*---------------------------------------------------------------------
    Look for a matching Sector in the cache
    ----------------------------------------------------------------------*/

    /* Get the current value for short term overhead cache period */
    st_cache_period = hdrovhd_get_cache_period_attrib(
                          HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID,
                          HDRSCP_DEFAULT_OVHD );

    /* Use timer based caching mechanism only when timer value is greater than 
       zero. Otherwise fall back to legacy caching based on sector id 128  */
    if (st_cache_period > 0)
    {
      cache_entry_ptr = hdrovhd_find_cached_msg_using_qc_info( 
                            qc_info_ptr->link,
                            qc_info_ptr->color_code,
                            qc_info_ptr->sector_id_24,
                            qc_info_ptr->sector_signature);
    }
    else
    {
      /* Check if color code changed */
      if( ( !hdrovhd.qc_info.initialized ) ||
          ( hdrovhd.qc_info.color_code != qc_info_ptr->color_code ) )
      {
        /*-----------------------------------------------------------------------
        Invalidate the current subnet, current overhead and current access
        parameters information
        -----------------------------------------------------------------------*/
        hdrovhd.curr_subnet.is_valid = FALSE;
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
        hdrovhd_set_ap_not_updated();
#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
      }

      /*-----------------------------------------------------------------------
      If the current subnet is still valid, then the cache can be searched for
      an entry from the sector specified in the QC msg.
      -----------------------------------------------------------------------*/
      if( hdrovhd.curr_subnet.is_valid )
      {
        /* A value has been stored for the subnet & since color code has not
        changed, the value is still valid. This subnet value can be used to
        search the cache for a previous stored entry from the same sector.
        Determine the sector Id by concatenating the stored subnet value with
        the Sector ID[24] from the received QC msg */
        hdrovhd_concatenate_sector_id( sector_id_128 ,
                                       qc_info_ptr->sector_id_24,
                                       hdrovhd.curr_subnet );
        /*---------------------------------------------------------------------
        Look for a matching Sector in the cache
        ----------------------------------------------------------------------*/
        cache_entry_ptr = hdrovhd_find_cached_msg_using_sp_info( sector_id_128,
                                                   qc_info_ptr->sector_signature,
                                                   qc_info_ptr->link.channel );
      }
    }

    if ( cache_entry_ptr != NULL )
    {
#ifdef FEATURE_LTE_TO_HDR

      if ( hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "Get CGI from cache!" );

        hdrovhd.cgi_info_is_valid = TRUE;

        /* Copy sector id info to sp buffer from which we read CGI */
        hdraddr_set( hdrovhd.sp_buffer.sector_id, cache_entry_ptr->sp_entry.sp_msg.sector_id );
        hdrovhd.sp_buffer.subnet_mask = cache_entry_ptr->sp_entry.sp_msg.subnet_mask;

        /* Save pilot information since we can't get pilot information once
         * SRCH is deactivated. */
        if ( hdrlmac_get_pilots( 1, &hdrovhd.cgi_info_pilot_rpt, 
                                 HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) )
        {
          HDR_MSG_PROT_2( MSG_LEGACY_MED, "CGI pilot rpt: PN %d, strength %d",
                          hdrovhd.cgi_info_pilot_rpt.link.pilot_pn,
                          hdrovhd.cgi_info_pilot_rpt.strength );
        }
        else
        {
          HDR_MSG_PROT( MSG_LEGACY_ERROR, "Can't get pilot report!" );   
        }
       
        ind_to_give_ptr->need_to_give_ind = TRUE;
        ind_to_give_ptr->need_to_give_ap_ind = FALSE;
        ind_to_give_ptr->ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
        return FALSE;
     }

#endif /* FEATURE_LTE_TO_HDR */

      /*-------------------------------------------------------------------
      We have found a SectorParameters msg in the cache, make sure that the
      associated AccessParameters msg in the same cache entry is valid.
      -------------------------------------------------------------------*/
      if ( (!cache_entry_ptr->ap_entry.is_valid) ||
           ( cache_entry_ptr->ap_entry.ap_msg.access_sig !=
             qc_info_ptr->access_signature) )
      {
        if (cache_entry_ptr->ap_entry.is_valid)
        {
          HDR_MSG_PROT(MSG_LEGACY_LOW, 
                       "cache changed: hdrovhd_process_unpacked_qc_msg");
          hdrovhd.cache_changed = TRUE;
        }
        hdrovhd_set_ap_not_updated();
        cache_entry_ptr->ap_entry.is_valid = FALSE;
      }
#ifdef FEATURE_HDR_TO_LTE
      if (cache_entry_ptr->ornl_entry.other_rat_signature != 
          qc_info_ptr->other_rat_signature)
      {
        hdrovhd_set_ornl_not_updated();
      }
#endif /* FEATURE_HDR_TO_LTE */
      /* Update the current Subnet.( ie, combination of SectorID & Subnet Mask ) */
      hdraddr_mask( hdrovhd.curr_subnet.subnet_id ,
                    cache_entry_ptr->sp_entry.sp_msg.sector_id,
                    cache_entry_ptr->sp_entry.sp_msg.subnet_mask );
      hdrovhd.curr_subnet.subnet_mask = cache_entry_ptr->sp_entry.sp_msg.subnet_mask;
      hdrovhd.curr_subnet.is_valid = TRUE;

      /*-------------------------------------------------------------------
      Do the processing necessary for new entry to be current entry &
      restart the SP supervision timer.
      NOTE: Processing the entry found in the cache may cause a channel
      change because the desired channel can change dynamically with BCMCS.
      -------------------------------------------------------------------*/
      continue_curr_hdr_system =
         hdrovhd_process_cache_entry( cache_entry_ptr, ind_to_give_ptr, 
                                      &qc_info_ptr->link, 
                                      &chan_was_changed );

      (void) rex_set_timer( &hdrovhd.sp_supervision_timer,
                            HDROVHD_SP_TIMER_MSECS );

      if ((continue_curr_hdr_system) && (!chan_was_changed))
      {
        /* If we found the SP message in the cache then store QC also so that
           next time we can use QP and go to sleep. If we did n't find the SP
           message, we wait till we get the SP message and then store the QC 
         */
#ifdef FEATURE_MEMCPY_REMOVAL
        cache_entry_ptr->qc_entry = *qc_info_ptr;
#else
        memcpy( &cache_entry_ptr->qc_entry, qc_info_ptr,
                sizeof(hdrovhd_qc_info_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

        /* Cache the QC message only in idle state */
        if ( !hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) )
        {
          cache_entry_ptr->qc_is_valid = TRUE;
        }

        if (cache_entry_ptr->ap_entry.is_valid)
        {
          hdrovhd_ap_info_updated( ind_to_give_ptr );
        }

#ifdef FEATURE_HDR_REVC
        nlb_cache_ptr = hdrovhd_find_nlb_cache(&qc_info_ptr->link);

        if (nlb_cache_ptr)            
        {
          if (nlb_cache_ptr->state == HDROVHD_NLB_CACHE_QC_RCVD)
          {
            hdrovhd_process_sp_neighbors(&qc_info_ptr->link, 
                                         nlb_cache_ptr, 
                                         &cache_entry_ptr->sp_entry.sp_msg);
          }

          time_get(nlb_cache_ptr->timestamp);
        }        
#endif /* FEATURE_HDR_REVC */
      }
    }
    else /* SP msg was not found in cache */
    {
      /*-------------------------------------------------------------------
      SP msg is not found in cache, or the sector signature did not match.
      Set control channel state to not current
      --------------------------------------------------------------------*/
      hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
      hdrovhd.ap_state = HDROVHD_AP_NOT_UPDATED;
#ifdef FEATURE_HDR_TO_LTE
      hdrovhd_set_ornl_not_updated();
#endif /* FEATURE_HDR_TO_LTE */
    }
  }
  else
  {
    /* If ColorCode, SectorID & SectorSignature have not changed, and the
       Overhead messages are current, then restart the SectorParameters
       supervision timer */
    (void) rex_set_timer( &hdrovhd.sp_supervision_timer,
                          HDROVHD_SP_TIMER_MSECS);

    hdrovhd.curr_cache_entry_ptr->config_change = 
        hdridle_get_current_config_change();

    /* Whenever we process the QC message, with QP enabled and connection 
       not open log the overhead cache. This means that long term cache 
       timer has expired */
    if (hdridlec_qpch_is_enabled() && 
        !hdralmp_connection_is_open(HDRHAI_IDLE_STATE_PROTOCOL))
    {
      hdrovhd.cache_changed = TRUE;
    }

    /* If only the Access Parameters changed and ovhd is current, then mark
       Access Parameters as not updated */
    if ( hdrovhd.qc_info.access_signature != qc_info_ptr->access_signature )
    {
      hdrovhd_set_ap_not_updated();
    }
#ifdef FEATURE_HDR_TO_LTE
    if ( hdrovhd.qc_info.other_rat_signature != 
         qc_info_ptr->other_rat_signature )
    {
      hdrovhd_set_ornl_not_updated();
    }
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_REVC
    nlb_cache_ptr = hdrovhd_find_nlb_cache(&qc_info_ptr->link);

    /* no need to check if cache expired, since we just processed QC */
    if (nlb_cache_ptr)
    {
      if (nlb_cache_ptr->state == HDROVHD_NLB_CACHE_QC_RCVD)
      {
        hdrovhd_process_sp_neighbors(&qc_info_ptr->link, 
            nlb_cache_ptr, 
            &hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg);
      }

      time_get(nlb_cache_ptr->timestamp);
    }    
#endif /* FEATURE_HDR_REVC */
  }

  /*---------------------------------------------------------------------------
  Check for redirection, unless have to leave the current HDR system
  ---------------------------------------------------------------------------*/
  if( continue_curr_hdr_system )
  {
    /*-------------------------------------------------------------------------
    Check if redirect bit is set
    -------------------------------------------------------------------------*/
    if( qc_info_ptr->redirect == (uint8)1 )
    {
      /*-----------------------------------------------------------------------
      Inform System Determination of received Redirection & perform action
      based on returned value
      -----------------------------------------------------------------------*/
      switch( sd_ss_ind_hdr_opr_redir( NULL,
                                       NULL,
                                       SD_SS_REDIR_HDR_QC,
                                       NULL ) )
      {
        case SD_SS_ACT_CONTINUE:
          break;

        case SD_SS_ACT_ACQ_HDR:
          /*-------------------------------------------------------------------
          Change HDR system. Update parameters & give indication.
          Note that this will overwrite a previously filled OVHD_UPDATED
          indication in the data structure
          -------------------------------------------------------------------*/
          ind_to_give_ptr->need_to_give_ind = TRUE;
          ind_to_give_ptr->ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
          continue_curr_hdr_system = FALSE;
          break;

        default :
          /*-------------------------------------------------------------------
          Exit HDR system. Update parameters & give indication.
          Note that this will overwrite a previously filled OVHD_UPDATED
          indication in the data structure
          -------------------------------------------------------------------*/
          ind_to_give_ptr->need_to_give_ind = TRUE;
          ind_to_give_ptr->ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
          continue_curr_hdr_system = FALSE;
          break;
      }
    }
  }

  return continue_curr_hdr_system;
} /* hdrovhd_process_unpacked_qc_msg() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_UPDATE_CURR_QC_INFO

DESCRIPTION
  This function updates the current stored QC info & informs FTCMAC of the
  received DRCLock & ForwardTrafficValid fields.

DEPENDENCIES
  None

PARAMETERS
  qc_info_ptr - Pointer to the unpacked QC msg
  link_id     - LinkID of received message

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_update_curr_qc_info
(
  hdrovhd_qc_info_type *qc_info_ptr,
  hdrsrch_link_id_type link_id

)
{
  hdrovhd.qc_info.initialized = TRUE;
  hdrovhd.qc_info.color_code = qc_info_ptr->color_code;
  hdrovhd.qc_info.sector_id_24 = qc_info_ptr->sector_id_24;
  hdrovhd.qc_info.sector_signature = qc_info_ptr->sector_signature;
  hdrovhd.qc_info.access_signature = qc_info_ptr->access_signature;
  hdrovhd.qc_info.link.channel.chan_num = qc_info_ptr->link.channel.chan_num;
  hdrovhd.qc_info.link.channel.band = qc_info_ptr->link.channel.band;
  hdrovhd.qc_info.link.pilot_pn = qc_info_ptr->link.pilot_pn;
#ifdef FEATURE_HDR_TO_LTE
  hdrovhd.qc_info.other_rat_available = qc_info_ptr->other_rat_available;
  hdrovhd.qc_info.other_rat_signature = qc_info_ptr->other_rat_signature;
  hdrovhd.qc_info.other_rat_txcycle = qc_info_ptr->other_rat_txcycle;
#endif /* FEATURE_HDR_TO_LTE */

  /*---------------------------------------------------------------------------
  Inform the FMAC protocol of the Forward traffic valid & drc lock fields
  received on this link
  ---------------------------------------------------------------------------*/
  hdrfmac_update_link_status( qc_info_ptr->fwd_traffic_valid63to0,
                              qc_info_ptr->fwd_traffic_valid127to64,
                              link_id,
                              HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );
} /* hdrovhd_update_curr_qc_info() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_CACHE_ENTRY

DESCRIPTION
  This function does the processing required when a new entry is written into
  the cache. The processing involves:
  1. Informing System Determination of the Subnet if this is the first time
     after activation that overhead information has been received.
  2. Determine if tuning to a new channel is required.

DEPENDENCIES
  None

PARAMETERS
  cache_entry_ptr -  Pointer to the cache entry.
  ind_to_give_ptr - Pointer to the structure containing information about
                which indication to give at the end of processing the msg.
  current_link_ptr - Current channel and pilot
  chan_was_changed_ptr - An output parameter to tell if the channel was changed
                         as a result of processing the cache entry.              

RETURN VALUE
  FALSE - If it is necessary to change or exit the current HDR system.
  TRUE  - Otherwise.

  chan_was_changed_ptr - set to TRUE if the channel was changed, 
                         FALSE otherwise                        
SIDE EFFECTS
  None

=============================================================================*/
static boolean hdrovhd_process_cache_entry
(
  hdrovhd_cache_entry_type *cache_entry_ptr,
  hdrovhd_ind_to_give_type *ind_to_give_ptr,
  const sys_link_type      *current_link_ptr,
  boolean                  *chan_was_changed_ptr
)
{
  boolean continue_curr_hdr_system; /* Keeps track of if system determination
                                       causes to change or exit the current HDR
                                       system */
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  sd_ss_act_e_type sd_action; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -esym(613, cache_entry_ptr) cache_entry_ptr is not NULL */
  ASSERT( cache_entry_ptr != NULL );


  continue_curr_hdr_system = TRUE;
  *chan_was_changed_ptr    = FALSE;
  /*---------------------------------------------------------------------------
  If this is the first time that overhead information has been obtained
  after activation, inform system determination
  ---------------------------------------------------------------------------*/
  if( !hdrovhd.acq_sid_rxed )
  {
    /*-------------------------------------------------------------------------
    Set flags to indicate that SID is received & that the indication given
    will be the first after acquistion
    -------------------------------------------------------------------------*/
    hdrovhd.acq_sid_rxed = TRUE;
    hdrovhd.is_first_after_acquisition = TRUE;

#ifdef FEATURE_LTE_TO_HDR_OH
    /* Ideally tunnel information is reset after AT leaves tunnel mode.
     * However, AT may receive some LTE info, e.g. cell ID, without entering 
     * tunnel. Reset as AT is in HDR network now.
     * If AT leaves tunnel and enters HDR in order to get CGI or 
     * perform ReverseTimeTransfer, do not reset as AT will go back 
     * to tunnel after completing CGI/ReverseTimeTransfer. 
     */
    if ( !( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
            ( hdrmc_lte_to_hdr_get_cgi_is_in_progress() ||
              hdrmc_hdr_to_lte_time_transfer_is_in_progress() ) ) )
    {
      hdrovhd_reset_tunnel_info();
    }
#endif /* FEATURE_LTE_TO_HDR_OH */

    sd_action = sd_ss_ind_hdr_acq_sid2(
                  cache_entry_ptr->sp_entry.sp_msg.sector_id,
                  cache_entry_ptr->sp_entry.sp_msg.subnet_mask, 
                  cache_entry_ptr->sp_entry.sp_msg.channel_list.chan,
                  cache_entry_ptr->sp_entry.sp_msg.channel_count,
                  NULL );

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                    "sd_ss_ind_hdr_acq_sid2, SD Action %d", sd_action );

    switch( sd_action )
    {
      case SD_SS_ACT_CONTINUE:
        continue_curr_hdr_system = TRUE;
#ifdef FEATURE_IRAT_FORCE_BSR_D2L_SETTING
        if ( hdrovhd.enable_dynamic_d2l && 
             sdss_get_loc_base_bsr_flag() )
        {
          HDR_MSG_PROT( MSG_LEGACY_MED, 
                        "D2L is enabled because of force D2L feature" );
          hdrovhd.force_d2l = TRUE;
        }
        else
        {
          HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                          "force D2L is not enabled, D2L is %d",
                          hdrovhd.hdr_to_lte_resel_enabled );
          hdrovhd.force_d2l = FALSE;
        }
#endif /* FEATURE_IRAT_FORCE_BSR_D2L_SETTING */
        break;

      case SD_SS_ACT_ACQ_HDR:
        /*-------------------------------------------------------------------
        Change HDR system. Update parameters, give indication & free buffers
        -------------------------------------------------------------------*/
        ind_to_give_ptr->need_to_give_ind = TRUE;
        ind_to_give_ptr->ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
        continue_curr_hdr_system = FALSE;
        break;

      default:
        /*-------------------------------------------------------------------
        Exit HDR system. Update parameters, give indication & free buffers.
        -------------------------------------------------------------------*/
        ind_to_give_ptr->need_to_give_ind = TRUE;
        ind_to_give_ptr->ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
        continue_curr_hdr_system = FALSE;
        break;
    }
  }

  if( continue_curr_hdr_system )
  {
#ifdef FEATURE_HDR_BCMCS
    if (hdrovhd_bc_info_updated( 
#ifdef FEATURE_HDR_BCMCS_2p0
                                 #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
                                 &cache_entry_ptr->bc_entry, 
                                 &cache_entry_ptr->sp_entry.sp_msg,
                                 current_link_ptr ))
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, 
        "New cache entry; BOM update would require a channel change");
    }
    else if (cache_entry_ptr->bc_entry.state == HDROVHD_BC_CURRENT)
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, 
        "New cache entry; BOM update would require no channel change");
    }
#endif /* FEATURE_HDR_BCMCS */

    /*-------------------------------------------------------------------------
    Run hashing algorithm here
    -------------------------------------------------------------------------*/
    (void) hdrovhd_compute_sp_hash_channel( 
      &(cache_entry_ptr->sp_entry.sp_msg), current_link_ptr, TRUE );

    /* In order to make AT has better chance to acquire and negotiate session,
     * AT is allowed to remove unsupported channel in hashing before session 
     * negotiation. 
     * However, if AT negotiates rev0 personality, AT needs to rerun hash 
     * algorithm based on all the channels listed in SP message. So there is a 
     * chance that AT will hash to unsupported channel after session 
     * negotiation, which is dangerous (device may crash). 
     * To avoid the above situation, we validates hashed channel here. 
     */
    if ( !hdrutil_is_band_supported(cache_entry_ptr->sp_entry.sp_msg.hash_channel.band) ||
         !hdrrup_is_band_chan_supported(&cache_entry_ptr->sp_entry.sp_msg.hash_channel) )
    {
      /* If the hash channel in SP msg is of invalid band class, call SD and setup
       * appropriate indications */
      sd_action = sd_ss_ind_hdr_opr_bad_sys( SD_SS_BAD_SYS_HASH_INV_BAND, NULL );

      HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                      "Unsupported/invalid Band Class, SD Action %d", sd_action );

      ind_to_give_ptr->need_to_give_ind = TRUE;
      continue_curr_hdr_system = FALSE;
      if (sd_action == SD_SS_ACT_ACQ_HDR)
      {
        ind_to_give_ptr->ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
      }
      else
      {
        ind_to_give_ptr->ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
      }
    }
    else if (hdrovhd_process_channel_change(cache_entry_ptr))
    {
      *chan_was_changed_ptr = TRUE;
    }
    else
    {
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif

      /* If we are not tunning to a new channel, make the cache entry current */
      continue_curr_hdr_system =
        hdrovhd_process_cache_update( cache_entry_ptr, ind_to_give_ptr );

      /* Whenever SP message is updated and AP is not updated wait for 
         3 secs to go to slotted sleep, this gives a chance to update 
         AP before going to sleep */
      if ( (continue_curr_hdr_system) && 
           (hdrovhd.info_state == HDROVHD_INFO_UPDATED) && 
           (hdrovhd.ap_state != HDROVHD_AP_UPDATED) )
      {
        (void) rex_set_timer( &hdrovhd.ap_update_wait_timer, 
                              HDROVHD_AP_UPDATE_WAIT_TIMER_MSECS );
      }

#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
    }
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Not continuing on curr HDR system.");
  }

  /*lint +esym(613, cache_entry_ptr) cache_entry_ptr is not NULL */
  return continue_curr_hdr_system;

} /* hdrovhd_process_cache_entry() */

#ifdef FEATURE_HDR_BCMCS_MFRQ
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_CHAN_IS_IN_CL

DESCRIPTION
  This function determines if the given channel is in the channel list of 
  the given SectorParameters message. Assumes FDD_paired channels.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  TRUE  - Channel change in progress
  FALSE - Channel change was not needed (stay on the current channel )

SIDE EFFECTS
  None

=============================================================================*/
static boolean hdrovhd_chan_is_in_cl
(
  hdrovhd_sp_msg_type *sp_msg_ptr,
  const sys_channel_type *channel_ptr
)
{
  int i; /* loop variable */

  /* Combined count of both standard and extended channels */
  int channel_count = sp_msg_ptr->channel_count + 
                      sp_msg_ptr->extended_channel_count;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < channel_count; i++)
  {
    if (SYS_CHAN_CMP_EQUAL(sp_msg_ptr->channel_list.chan[i], *channel_ptr))
    {
      return TRUE;
    }
  }
  return FALSE;
} /* hdrovhd_chan_is_in_cl() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_BCMCS_IS_OVERRIDING_HASHING

DESCRIPTION
  This function determines if BCMCS is going to override the channel
  hashing logic to keep us on the best bc_channel.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  TRUE  - BCMCS will be choosing the current channel
  FALSE - The normal hash function will be choosing the current channel

SIDE EFFECTS
  None

=============================================================================*/
static boolean hdrovhd_bcmcs_is_overriding_hashing
(
  hdrovhd_cache_entry_type *cache_entry_ptr,
  const sys_channel_type *bc_channel_ptr,
  boolean processing_sp_msg
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the AT acquires on a sector/channel it has already visited before, it
     already has a cache entry stored for that sector/channel.  Upon processing
     the SP message, the AT checks the best bc channel against the current SP
     channel to see if the BC hash rules should override the SP ones.  Since
     this is the first time we are parsing a new SP message for this sector,
     the best bc hash channel is still set to the bc hash channel from the sector
     we just came from (bc ovhd updated won't be called until we are done 
     processing the SP message.)  

     Until bcovhd has updated its best bc channel, the bc override rules should
     be ignored. 

     process_sp_msg

     |__ unpack_sp_msg

         |__ checks bcmcs_is_overriding_hashing
             If we know that the best bc channel is not up to date, the expected
             channel will now be the SP hash channel (ignore bc hash)

             Before that, if the expected channel was set to an old bc hash 
             channel, the current SP entry would stay in WRITE_IN_PROG state 
             and the current cache entry in INVALID state.  This was still 
             considered a success because the assumption was that AT would 
             immediately tune to the bc hash channel when going through
             process_cache_entry (same thread). See (***)
             
     |__ if SUCCESS process_cache_entry

         |__ bc_info_updated -> updates bc msg in bcovhd

             |__  bccp_process_bc_info_updated 

                  |__ find_best_bc_freq -> updates best bc channel

         |__ process_channel_change

             |__ checks bcmcs_is_overriding_hashing
                 (***)
                 Now that the best bc channel is updated, if we had decided to 
                 tune in unpack_sp_msg based on an old best bc channel, it is
                 possible that now we would decide not to tune and the current
                 cache entry would be in a bad (INVALID) state. 

                 We prevented this by ignoring the bc hash when unpacking the
                 SP message, but now we still want to ignore the bc hash based
                 on the updated best bc channel so we have to remember that
                 we were ignoring the bc hash when we unpacking SP.

                 The flag to ignore bc hash is unconditionally reset at the end 
                 of process_sp_msg to make sure that we can never get out of 
                 sync.
  */

  if ( hdrovhd.ignore_bc_hash_override )
  {
    if (processing_sp_msg)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
                   "Shouldn't already be ignoring BC hash override rules!");
    }

    HDR_MSG_PROT(MSG_LEGACY_HIGH,"Ignore BC hash override (0)");
    hdrovhd.ignore_bc_hash_override = FALSE;
    return FALSE;
  }

  if ( ( processing_sp_msg ) &&
       ( cache_entry_ptr->bc_entry.state != HDROVHD_BC_INVALID ) &&
       ( !hdrbcovhd_msg_is_up_to_date(cache_entry_ptr->bc_entry.bc_item_ptr) ) 
     )
  {
      /* BC channel ptr has not been updated and may change. 
         The AT won't override hashing until it knows for sure. */
      HDR_MSG_PROT(MSG_LEGACY_HIGH,
        "Ignore BC hash override (1) - bc msg in cache not processed yet");
      hdrovhd.ignore_bc_hash_override = TRUE;
      return FALSE;
  }
  /* If bc_channel isn't known to be a valid channel on the current
     sector, continue as if BCMCS wasn't involved. */
  else if (cache_entry_ptr->bc_entry.state == HDROVHD_BC_CURRENT)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"BC override hashing (bc is current)");
    return TRUE;
  }
  else if (SYS_CHAN_CMP_EQUAL(cache_entry_ptr->rx_link.channel, *bc_channel_ptr))
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED,"BC override hashing (cache chan=ref bc chan %d)",
                   cache_entry_ptr->rx_link.channel.chan_num);
    return TRUE;
  }
  else if (hdrovhd_chan_is_in_cl( &cache_entry_ptr->sp_entry.sp_msg, 
                             bc_channel_ptr ))
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED,
                   "BC override hashing (ref bc chan (%d) is in chan list)",
                   bc_channel_ptr->chan_num);
    return TRUE;
  }

  return FALSE;
} /* hdrovhd_bcmcs_is_overriding_hashing */
#endif /* FEATURE_HDR_BCMCS_MFRQ */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_CHANNEL_CHANGE

DESCRIPTION
  This function is called to perform a channel change if needed.

  If a received SP msg message or BC flow hashes to a channel different from 
  the current one, then this function sets the ovhd info &
  AP state to be not updated, invalidates the cache where SP message is stored
  entry & commands LMAC to change channels

DEPENDENCIES
  None

PARAMETERS
  cache_entry_ptr - Cache entry where either:
                    * the SP msg that may hash to a diff channel is stored
                    * the BC msg that may hash to a diff channel is stored 

RETURN VALUE
  TRUE  - Channel change in progress
  FALSE - Channel change was not needed (stay on the current channel )

SIDE EFFECTS
  None

=============================================================================*/
LOCAL boolean hdrovhd_process_channel_change
(
  hdrovhd_cache_entry_type *cache_entry_ptr
)
{
  sys_link_type link; /* The link we are changing to */
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience pointer */

  #ifdef FEATURE_HDR_BCMCS_MFRQ
  sys_channel_type bc_channel;
  sys_bcmcs_flow_id_type flow_id;
  hdrerrno_enum_type err_number; /* result from retrieving bc_channel */
  #endif /* FEATURE_HDR_BCMCS_MFRQ */

  #ifdef FEATURE_HDR_BCMCS
  boolean bcmcs_is_overriding_hashing = FALSE;
  boolean bc_ovhd_needs_reprocessing = FALSE;
  #endif
  
  boolean need_to_tune = FALSE;

  /* number of avoided channels returned by SD*/
  uint8 num_avoided_channels = 0;

  /* Current avoided channels returned by SD*/
  sys_avoided_channel_record_type avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];       
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -esym(613, cache_entry_ptr) cache_entry_ptr is not NULL */
  ASSERT( cache_entry_ptr != NULL );

  /* Initialize local variable */
  memset( avoided_chan_rec, 0, sizeof(avoided_chan_rec) );

  #ifdef FEATURE_HDR_BCMCS_MFRQ
  /* This is just a convenient place to implement this timer... */

  err_number = hdrbccp_get_preferred_bc_channel( &bc_channel, &flow_id );
  if (err_number == E_SUCCESS)
  {
    if (cache_entry_ptr->bc_entry.state == HDROVHD_BC_INVALID)
    {
      /* We have a reference flow but no overhead info.  Set a timer for
         3 times the BroadcastOverhead period to make sure we get the 
         message in a reasonable amount of time. */
      HDR_MSG_PROT_1(MSG_LEGACY_MED,"Start BC supervision timer (%dms)",
                     3 * HDROVHD_SCC_IN_MS * hdrbccpc_get_curr_bc_ovhd_period());   
      (void) rex_set_timer( &hdrovhd.bc_supervision_timer, 
                            3 * HDROVHD_SCC_IN_MS * hdrbccpc_get_curr_bc_ovhd_period() );
    }
    else
    {
      HDR_MSG_PROT_2(MSG_LEGACY_MED,
                     "No BC supervision - BC msg already stored (%d:%d)",
                     cache_entry_ptr->rx_link.channel.chan_num, 
                     cache_entry_ptr->rx_link.pilot_pn);
    }
  }
  /* We don't have a reference flow, so no need for supervision */
  else if (rex_get_timer( &hdrovhd.bc_supervision_timer ) != 0)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Clear BC supervision timer");
    /* We have a valid message, so stop the supervision timer */
    (void) rex_clr_timer( &hdrovhd.bc_supervision_timer );
  }
  #endif /* FEATURE_HDR_BCMCS_MFRQ */

  /*-------------------------------------------------------------------------
  If we are in connected state we don't tune channel.
  -------------------------------------------------------------------------*/
  if( hdrrup_is_tc_assigned( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
  If we are in the Access Handoff state we don't tune channel.
  -------------------------------------------------------------------------*/
  if( hdrovhds_get_state() == HDROVHDS_ACC_HO_IN_PROG )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "Delaying channel change due to AHO in prog" );
    return FALSE;
  }

  #ifdef FEATURE_HDR_BCMCS_MFRQ
  /* If broadcast has requested flows that are available on a bc_channel
     different than the SP hash channel, then the AT will disregard the
     SP hashing rules and tune to the preferred bc_channel.
     One exception is if the bc_channel is being pulled from a stale Broadcast
     Overhead message and isn't in the ChannelList. In this case the bc_channel
     is possibly not a valid channel on this sector.  In this case the AT follows
     normal hashing rules until it gets a new BroadcastOverhead message, then
     tries again to tune to the correct bc_channel.
   */

  if (err_number == E_SUCCESS)
  {
    /* If bc_channel is the same as the hash channel, continue as if 
       BCMCS wasn't involved. */
    if (!SYS_CHAN_CMP_EQUAL( cache_entry_ptr->sp_entry.sp_msg.hash_channel, 
                             bc_channel))
    {
      /* If bc_channel isn't known to be a valid channel on the current
         sector, continue as if BCMCS wasn't involved. */
      if (hdrovhd_bcmcs_is_overriding_hashing( 
              cache_entry_ptr, &bc_channel, 
              FALSE )) /* Not calling from unpack_sp_msg() */
      {
        bcmcs_is_overriding_hashing = TRUE;

        HDR_MSG_PROT_6( MSG_LEGACY_HIGH,
                        "current (%d:%d) sp hash (%d:%d) bc_channel (%d:%d)",
                        cache_entry_ptr->rx_link.channel.band,
                        cache_entry_ptr->rx_link.channel.chan_num,
                        cache_entry_ptr->sp_entry.sp_msg.hash_channel.band,
                        cache_entry_ptr->sp_entry.sp_msg.hash_channel.chan_num,
                        bc_channel.band,
                        bc_channel.chan_num );
    
        /* If the preferred bc channel is the channel we are currently on,
           then there is no need to tune. */
        if (!SYS_CHAN_CMP_EQUAL(cache_entry_ptr->rx_link.channel, bc_channel))
        {
          link.channel = bc_channel;
          need_to_tune = TRUE;
    
          HDRTRACE_OVHD_BCMCS_CHAN_CHANGE(bc_channel.chan_num, flow_id);
        }
        else
        {
          HDR_MSG_PROT_1(MSG_LEGACY_MED,"No need to tune (bc chan=curr chan %d)",
                         bc_channel.chan_num);
        }
      } /* end if (bc_channel is valid on current sector) */
      else if (cache_entry_ptr->bc_entry.state != HDROVHD_BC_INVALID)
      {
        /* BCMCS is currently not overriding hashing, but our up-to-date bc
           ref channel is different from the sp hash channel.  If we don't end up
           tuning, then we should re-process the bc ovhd message next time so
           we try to tune to the correct bc_channel. */
        bc_ovhd_needs_reprocessing = TRUE;
      }
    } /* end if (bc_channel is not same as hash channel) */
    else
    {
      if (hdrovhd.ignore_bc_hash_override)
      {
        /* Our sp hash channel is the same as our up to date bc channel,
           no need to continue ignoring the bc hash rules */
        hdrovhd.ignore_bc_hash_override = FALSE;
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignore bc hash override (0)");
      }
    }
  } /* end if (bc_channel is valid) */

  if (hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA) &&
      (err_number == E_NOT_AVAILABLE))
  {
    if (hdrbccp_get_original_channel( &cache_entry_ptr->rx_link.channel,  
                                      &bc_channel ) == E_SUCCESS)
    {
      bcmcs_is_overriding_hashing = TRUE;

      HDR_MSG_PROT_4(MSG_LEGACY_HIGH,
        "Leaving BC%d SC2 (%d) after BCMCS -> BC%d SC3 (%d) [1024-1323]",
        cache_entry_ptr->rx_link.channel.band, 
        cache_entry_ptr->rx_link.channel.chan_num,
        bc_channel.band, bc_channel.chan_num );

      link.channel = bc_channel;
      need_to_tune = TRUE;

      HDRTRACE_OVHD_BCMCS_CHAN_CHANGE(bc_channel.chan_num, 0xbc);
    }
  }

  #endif /* FEATURE_HDR_BCMCS_MFRQ */

  #ifdef FEATURE_HDR_BCMCS
  if (!bcmcs_is_overriding_hashing)
  #endif /* FEATURE_HDR_BCMCS */
  {
    if (!SYS_CHAN_CMP_EQUAL( cache_entry_ptr->rx_link.channel,
                             cache_entry_ptr->sp_entry.sp_msg.hash_channel))
    {
      /* Channel and band are what are specified by the SP message */
      link.channel = cache_entry_ptr->sp_entry.sp_msg.hash_channel;
      need_to_tune = TRUE;

      HDR_MSG_PROT_2(MSG_LEGACY_MED,
                     "Not overriding bc hash but need to tune (cache chan %d != sp hash chan %d)",
                     cache_entry_ptr->rx_link.channel.chan_num, 
                     cache_entry_ptr->sp_entry.sp_msg.hash_channel.chan_num);
    }
    /* If BCMCS is not overriding hashing but yet we didn't need to tune because of SP hash, mark
       the entry as not parsed if the bc ref channel would send the AT to a new channel.  */
    #ifdef FEATURE_HDR_BCMCS
    else if ( bc_ovhd_needs_reprocessing )
    {
      if (cache_entry_ptr->bc_entry.state == HDROVHD_BC_CURRENT)
      {
        HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_process_channel_change");
        hdrovhd.cache_changed = TRUE;

        HDR_MSG_PROT_1(MSG_LEGACY_MED,"Need to reprocess bc ovhd for cache entry ref -> %d",
                       cache_entry_ptr->bc_entry.bc_item_ptr);
        cache_entry_ptr->bc_entry.state = HDROVHD_BC_NOT_PARSED;
      }
    }
    #endif /* FEATURE_HDR_BCMCS */
  }

  if (need_to_tune)
  {
    /*------------------------------------------------------------------------- 
       LRCH (Less Restrictive Channel Hashing) 

       Provide searcher with the SP neighbor list from the channel we are 
       currently on. 

       Ovhd is not currently updated and this cache entry is about
       to be deleted.  If the coverage is weak on the new channel, the AT will
       attempt to idle ho to a new sector when it gets to the hash channel.
       The candidate is provided by the neighbor list the AT gave searcher 
       before the channel hash because there is a good chance that this  
       neighbor list would be similar to the neighbor list we will get after 
       successfully acquiring on the new channel.    
    -------------------------------------------------------------------------*/
    if (hdrovhd.info_state != HDROVHD_INFO_UPDATED)
    {
      sp_msg_ptr = &cache_entry_ptr->sp_entry.sp_msg;

      /*-------------------------------------------------------------------------
      Remove invalid neighbors from nset (including IS856_FWD_CH system type).  
      -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
      num_avoided_channels = hdrovhd_update_sys_avoid_list(
          HDRSRCH_AVOIDED_CHAN_MAX_SIZE,
          avoided_chan_rec);
#else 
      sd_misc_sys_avoid_list(SYS_SYS_MODE_HDR, avoided_chan_rec, 
                             &num_avoided_channels, HDRSRCH_AVOIDED_CHAN_MAX_SIZE);
#endif /* #else FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

	if(num_avoided_channels > HDRSRCH_AVOIDED_CHAN_MAX_SIZE)
	{
	    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "num_avoided_channels is %d greater than max allowed. limiting to max %d.",num_avoided_channels, HDRSRCH_AVOIDED_CHAN_MAX_SIZE);  
		num_avoided_channels = HDRSRCH_AVOIDED_CHAN_MAX_SIZE;
	}
	
      (void) hdrutil_remove_invalid_neighbors( sp_msg_ptr->nset, 
                                               &(sp_msg_ptr->num_neighbors),
                                               avoided_chan_rec, num_avoided_channels
                                             );

      /*-------------------------------------------------------------------------
      Update the searcher neighbor list and perform channel change in one shot
      -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_BCMCS
      dsm_free_packet( &cache_entry_ptr->bc_entry.bc_item_ptr );
#endif /* FEATURE_HDR_BCMCS */
      hdrovhd_clear_cache_entry_and_state( cache_entry_ptr );

      /* We are changing to the current pilot, only on a different channel */
      link.pilot_pn = hdrovhd.qc_info.link.pilot_pn;
      link.mode = SYS_HDR_CHAN_REC_MODE_IS856;

      hdrovhds_change_state( HDROVHDS_FREQ_CHANGE_IN_PROG );

      hdrlmac_idle_channel_change_and_nset_update(
                           sp_msg_ptr->num_neighbors, sp_msg_ptr->nset, 
                           0, NULL, /* Don't update channel list */ 
                           0, NULL, /* Don't update cached sectors */
                           num_avoided_channels, 
                           avoided_chan_rec, /* avoided chan */
                           HDRSRCH_NSET_UPDATE_BROADCAST, 
                           sp_msg_ptr->pilot_gid, /* serving sector's pilot gr id */
                           &link, 
                           (hdrovhds_get_state() == HDROVHDS_SLEEP ? TRUE : FALSE),
                           HDRHAI_OVERHEAD_MESSAGES_PROTOCOL
                         );
    }
    else
    {
#ifdef FEATURE_HDR_BCMCS
      dsm_free_packet( &cache_entry_ptr->bc_entry.bc_item_ptr );
#endif /* FEATURE_HDR_BCMCS */
      hdrovhd_clear_cache_entry_and_state( cache_entry_ptr );

      /* We are changing to the current pilot, only on a different channel */
      link.pilot_pn = hdrovhd.qc_info.link.pilot_pn;
      link.mode = SYS_HDR_CHAN_REC_MODE_IS856;

      hdrovhds_change_state( HDROVHDS_FREQ_CHANGE_IN_PROG );

     /* A frequency change may occur in sleep state when a BCMCS flow is 
       requested while sleeping. OVHD still need to transition to 
       FrequencyChangeInProgress state in this case because LMAC might
       not be able to complete IDLE_CHAN_CHANGE before OVHD returns
       to PROCESS_ALL_MSG state. */
      hdrlmac_idle_chan_change( &link, 
                                (hdrovhds_get_state() == HDROVHDS_SLEEP ? TRUE : FALSE),
                                HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );
    }

    HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
                    "Tuning (%d:%d) -> (%d:%d)",
                    cache_entry_ptr->rx_link.channel.band,
                    cache_entry_ptr->rx_link.channel.chan_num,
                    link.channel.band,
                    link.channel.chan_num );
  }

  return need_to_tune;

/*lint +esym(613, cache_entry_ptr) cache_entry_ptr is not NULL */
} /* hdrovhd_process_channel_change() */

/*=============================================================================

FUNCTION HDROVHD_REGISTRATION_SUCCESS

DESCRIPTION
  This function updates the RouteUpdateTriggerCodeList entry to reflect
  that the RouteUpdate message was sent successfully.

DEPENDENCIES

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

=============================================================================*/
void hdrovhd_registration_success( void )
{
  uint16 route_update_trigger_code;
  hdrovhd_subnet_type *cur_subnet_ptr;
  hdrovhd_rutc_entry_type *entry_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( hdrovhd.curr_cache_entry_ptr != NULL );

  route_update_trigger_code = 
    hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg.route_update_trigger_code,
  cur_subnet_ptr = &hdrovhd.curr_subnet;

  entry_ptr = (hdrovhd_rutc_entry_type*) q_check( &hdrovhd.rutc_list );

  /* See if this code is already in the list */
  while (entry_ptr != NULL)
  {        
    if ((cur_subnet_ptr->subnet_mask == entry_ptr->subnet_mask) &&
        (hdraddr_is_equal( cur_subnet_ptr->subnet_id, entry_ptr->subnet_id )) &&
        (route_update_trigger_code == entry_ptr->route_update_trigger_code))
    {
      entry_ptr->rup_was_acked = TRUE;
      break;
    }
    entry_ptr = (hdrovhd_rutc_entry_type *)
                 q_next( &hdrovhd.rutc_list, &entry_ptr->link );
  }
} /* hdrovhd_registration_success */

/*=============================================================================

FUNCTION HDROVHD_UPDATE_ROUTE_UPDATE_TRIGGER_CODE_LIST

DESCRIPTION
  This function updates the RouteUpdateTriggerCodeList.

DEPENDENCIES
  The RouteUpdateTriggerCode field must have been included in the most 
  recently received SectorParameters message.

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

=============================================================================*/
static void hdrovhd_update_route_update_trigger_code_list
(
  uint16 route_update_trigger_code,
  uint8  route_update_trigger_max_age,
  qword  ts_val,
  hdrovhd_subnet_type *cur_subnet_ptr
)
{
  uint8 route_update_trigger_code_list_size; /* Attribute value */
  uint32 adjust; /* Value to subtract from timestamp to get expiration time */
  uint32 timestamp; /* current system time in units if 1.28s */
  uint32 oldest_valid_timestamp; /* Anything older than this has expired */
  uint32 oldest_timestamp; /* Timestamp of oldest entry in the list */
  hdrovhd_rutc_entry_type *entry_ptr = NULL;
  hdrovhd_rutc_entry_type *next_entry_ptr = NULL;
  hdrovhd_rutc_entry_type *cur_entry_ptr = NULL;
  hdrovhd_rutc_entry_type *oldest_entry_ptr = NULL;

  int entry; /* Used to index the current entry while displaying RUTC
                list via F3 messaging */
  boolean is_current; /* Used to determine if the RUTC entry being 
                         displayed is the current entry */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate input parameters */
  ASSERT( cur_subnet_ptr != NULL );
  if (!cur_subnet_ptr->is_valid) { return; }
  if (route_update_trigger_code == HDROVHD_RUTC_UNSPECIFIED) { return; }

  if (hdrovhd.rutc_free_q.cnt == 0)
  {
    /* This should never happen because the free_q is sized to have one
     * more entry than the max value of RouteUpdateTriggerCodeListSize.
     */
    HDR_MSG_PROT (MSG_LEGACY_ERROR, "RouteUpdateTriggerCode list overflow");
    return;
  }

  /* Retrieve the InUse value of the RouteUpdateTriggerCodeListSize attrib */
  (void) hdrscmdb_get_inuse_attrib( 
                      HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 
                      HDRSCMOVHD_RUT_CODE_LIST_SIZE_ATTRIB_ID,
                      &route_update_trigger_code_list_size,
                      HDRSCP_DEFAULT_OVHD );

  /* -----------------------------------------------------------------------
     If RouteUpdateTriggerCodeListSize is set to 0x00, the access terminal 
     shall delete all entries in the RouteUpdateTriggerCodeList.
     ----------------------------------------------------------------------- */

  if (route_update_trigger_code_list_size == 0)
  {
    /* Delete all entries from the list */
    while ((entry_ptr = q_get( &hdrovhd.rutc_list )) != NULL)
    {
      q_put( &hdrovhd.rutc_free_q, &entry_ptr->link );
    }
    return;
  }

  /* -----------------------------------------------------------------------
     The access terminal shall delete any entries in the 
     RouteUpdateTriggerCodeList other than the current (Subnet, 
     RouteUpdateTriggerCode) received in the most recent SectorParameters 
     message if the entries have an expiration timer that has been running 
     for at least 2^(RouteUpdateTriggerMaxAge + 3) * 1.28 seconds.     
     ----------------------------------------------------------------------- */

  /* Convert timestamp to a 32 bit quantity in units of 1.28 seconds */
  timestamp = ( qw_lo( ts_val ) >> 26 );
  timestamp |= ( qw_hi( ts_val ) << 6 );

  /* Now subtract 2^(RouteUpdateTriggerMaxAge + 3).  The result is the oldest
   * timestamp that is still valid.  Any timestamp less than this has expired.
   */
  adjust = (1 << (route_update_trigger_max_age + 3));
  if (adjust > timestamp)
  {
    /* Pathological case to avoid making an unsigned value negative */
    oldest_valid_timestamp = 0;
  }
  else
  {
    oldest_valid_timestamp = timestamp - adjust;
  }

  entry_ptr = (hdrovhd_rutc_entry_type*) q_check( &hdrovhd.rutc_list );

  /* See if this code is already in the list */
  while (entry_ptr != NULL)
  {        
    next_entry_ptr = (hdrovhd_rutc_entry_type *)
                     q_next( &hdrovhd.rutc_list, &entry_ptr->link );

    if ((cur_entry_ptr == NULL) &&
        (cur_subnet_ptr->subnet_mask == entry_ptr->subnet_mask) &&
        (hdraddr_is_equal( cur_subnet_ptr->subnet_id, entry_ptr->subnet_id )) &&
        (route_update_trigger_code == entry_ptr->route_update_trigger_code))
    {
      cur_entry_ptr = entry_ptr;
    }
    else if (entry_ptr->timestamp < oldest_valid_timestamp)
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "RUTC %d: deleted; exceeded max age",
                      entry_ptr->route_update_trigger_code);

      /* Delete the expired entries */
      q_delete( &hdrovhd.rutc_list, &entry_ptr->link );
      q_put( &hdrovhd.rutc_free_q, &entry_ptr->link );
    }
    entry_ptr = next_entry_ptr;
  }


  if (cur_entry_ptr == NULL)
  {
    /* ---------------------------------------------------------------------
       If the (Subnet, RouteUpdateTriggerCode) pair from the most recently 
       received SectorParameters message is not included in the 
       RouteUpdateTriggerCodeList, then the access terminal shall add the 
       entry to the RouteUpdateTriggerCodeList and shall reset, initialize 
       to zero, and start the expiration timer for that entry.
       --------------------------------------------------------------------- */
    entry_ptr = q_get( &hdrovhd.rutc_free_q );
    ASSERT( entry_ptr != NULL );

    entry_ptr->route_update_trigger_code = route_update_trigger_code;
    hdraddr_set( entry_ptr->subnet_id, cur_subnet_ptr->subnet_id );
    entry_ptr->subnet_mask = cur_subnet_ptr->subnet_mask;
    entry_ptr->timestamp = timestamp;
    entry_ptr->rup_was_acked = FALSE;
    q_put( &hdrovhd.rutc_list, &entry_ptr->link );
    cur_entry_ptr = entry_ptr;
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "RUTC %d: added to list; sending RUP",
                    entry_ptr->route_update_trigger_code);

    /* ---------------------------------------------------------------------
       The access terminal shall generate a RouteUpdate.SendRouteUpdate 
       command when it adds an entry to the RouteUpdateTriggerCodeList.
       --------------------------------------------------------------------- */
    hdrrup_send_route_update( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );
  }
  else 
  {
    if (!cur_entry_ptr->rup_was_acked)
    {
      /* We haven't successfully transmitted the RUP message since we added
       * this entry to the list.  Try again now.
       */
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
                      "RUTC %d: already in list but RUP not acked; resend",
                      route_update_trigger_code);
      hdrrup_send_route_update( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );
    }
    else
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_MED, "RUTC %d: already in list", 
                      route_update_trigger_code);
    }

    if (cur_entry_ptr->timestamp < oldest_valid_timestamp)
    {
      /* -------------------------------------------------------------------
         If the expiration timer for the RouteUpdateTriggerCodeList entry 
         corresponding to the current (Subnet, RouteUpdateTriggerCode) 
         received in the most recent SectorParameters message has been running 
         for at least 2^(RouteUpdateTriggerMaxAge + 3) * 1.28 seconds, the 
         access terminal shall reset, initialize to zero, and restart the 
         expiration timer for that entry.
         ------------------------------------------------------------------- */
      cur_entry_ptr->timestamp = timestamp;

      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "RUTC %d: updating timestamp",
                      cur_entry_ptr->route_update_trigger_code);
    }
  }

  /* -----------------------------------------------------------------------
     If there are more than the number of entries specified by the 
     RouteUpdateTriggerCodeListSize attribute in the 
     RouteUpdateTriggerCodeList, then the access terminal shall delete 
     entries from the list until there are exactly 
     RouteUpdateTriggerCodeListSize entries in the list according to the 
     following rules:
       o The access terminal shall delete the oldest entries in the list 
         first, and
       o the access terminal shall not delete the entry in the list that 
         corresponds to the (Subnet, RouteUpdateTriggerCode) received in the 
         most recent SectorParameters message.
     ----------------------------------------------------------------------- */

  while (hdrovhd.rutc_list.cnt > route_update_trigger_code_list_size)
  {
    /* Initialize to higher value than all entries */
    oldest_timestamp = timestamp + 1;
    oldest_entry_ptr = NULL;

    /* Find the oldest item and delete it */
    entry_ptr = (hdrovhd_rutc_entry_type*) q_check( &hdrovhd.rutc_list );
    while (entry_ptr != NULL)
    {        
      if (entry_ptr == cur_entry_ptr)
      {
        /* Never remove the current entry */
      }
      else if (entry_ptr->timestamp < oldest_timestamp)
      {
        oldest_timestamp = entry_ptr->timestamp;
        oldest_entry_ptr = entry_ptr;
      }
      entry_ptr = (hdrovhd_rutc_entry_type *)
                   q_next( &hdrovhd.rutc_list, &entry_ptr->link );
    }

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "RUTC %d: Deleted; too many entries (%d > %d)",
                    cur_entry_ptr->route_update_trigger_code,
                    hdrovhd.rutc_list.cnt,
                    route_update_trigger_code_list_size);

    ASSERT( oldest_entry_ptr != NULL );
    q_delete( &hdrovhd.rutc_list, &oldest_entry_ptr->link );
    q_put( &hdrovhd.rutc_free_q, &oldest_entry_ptr->link );
  }

  /*-----------------------------------------------------
    Print and log the current RUTC List for AST testing
  -----------------------------------------------------*/
  if( q_cnt( &hdrovhd.rutc_list ) > 0 )
  {
    entry = 0;
    hdrplog_rutc_clear_list( route_update_trigger_code_list_size );

    entry_ptr = (hdrovhd_rutc_entry_type *) q_check( &hdrovhd.rutc_list );
    while( entry_ptr != NULL )
    {
      hdrplog_rutc_add_entry( 
        entry_ptr->subnet_id,
        entry_ptr->subnet_mask,
        entry_ptr->route_update_trigger_code,
        entry_ptr->timestamp,
        entry_ptr->rup_was_acked );

      is_current = ( entry_ptr == cur_entry_ptr ) ? TRUE : FALSE;

      if( entry_ptr->rup_was_acked )
      {
        if( is_current )
        {
          HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
            "*RUTC[%d] = %d; Was Acked (Timestamp 0x%x)",
            entry, entry_ptr->route_update_trigger_code, entry_ptr->timestamp );
        }
        else
        {
          HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
            " RUTC[%d] = %d; Was Acked (Timestamp 0x%x)",
            entry, entry_ptr->route_update_trigger_code, entry_ptr->timestamp );
        }
      }
      else
      {
        if( is_current )
        {
          HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
            "*RUTC[%d] = %d; Not Acked (Timestamp 0x%x)",
            entry, entry_ptr->route_update_trigger_code, entry_ptr->timestamp );
        }
        else
        {
          HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
            " RUTC[%d] = %d; Not Acked (Timestamp 0x%x)",
            entry, entry_ptr->route_update_trigger_code, entry_ptr->timestamp );
        }
      }

      entry_ptr = (hdrovhd_rutc_entry_type *)
                   q_next( &hdrovhd.rutc_list, &entry_ptr->link );
      entry++;
    }

    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "RUTC[%d-%d] Not used",
      entry, route_update_trigger_code_list_size - 1);

    hdrplog_rutc_commit();
  }
} /* hdrovhd_update_route_update_trigger_code_list() */



/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_CONFIGURE_AT_OPERATIONAL_MODE

DESCRIPTION
  This function configures Search to run different AT operational mode
  algorithms based on whether SectorParameters message contains 1x
  neighbors.

DEPENDENCIES
  None

PARAMETERS
  sp_entry_ptr - pointer to the SectorParametersMessage of which 1x neighbors
                 are evaluated.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

=============================================================================*/
LOCAL void hdrovhd_configure_at_operational_mode
(
   hdrovhd_sp_entry_type *sp_entry_ptr
)
{
  #ifdef FEATURE_ATOM
  hdrsrch_opmode_config_enum_type opmode;
  int   i;
/*-------------------------------------------------------------------------*/

  for (i = 0; i < sp_entry_ptr->sp_msg.num_neighbors; i++)
  {
    if ( sp_entry_ptr->sp_msg.nset[i].link.mode == SYS_HDR_CHAN_REC_MODE_IS2000 )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "ATOM: Found 1x neighbors in SPM");

      opmode = HDRSRCH_OPMODE_POOR_COVERAGE_DETECTION_ENABLED;
      break;
    }
  }

  if ( i == sp_entry_ptr->sp_msg.num_neighbors )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH,
                  "ATOM: No 1x neighbors in SPM");
    opmode = HDRSRCH_OPMODE_GOOD_COVERAGE_DETECTION_ENABLED;
  }

  if ( hdrovhd.op_mode != opmode )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "ATOM: Config the AT operational mode %d", opmode);

    hdrovhd.op_mode = opmode;
    hdrlmac_configure_at_operational_mode(opmode);
  }
  #endif /* FEATURE_ATOM */
  
} /* hdrovhd_configure_at_operational_mode */




/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_CACHE_UPDATE

DESCRIPTION
  This function updates the current cache pointer to point to the entry
  specified in input & does the processing needed along with this.

DEPENDENCIES
  None

PARAMETERS
  cache_entry_ptr - Pointer to the cache entry to be made current
  ind_to_give_ptr - Pointer to the structure containing information about
                which indication to give at the end of processing the msg.

RETURN VALUE
  TRUE - If the current cache entry is updated & ovhd info updated indication
         given
  FALSE - If it is necessary to change or exit the current HDR system.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL boolean hdrovhd_process_cache_update
(
  hdrovhd_cache_entry_type *cache_entry_ptr,
  hdrovhd_ind_to_give_type *ind_to_give_ptr
)
{
  boolean continue_curr_hdr_system; /* Keeps track of if system determination
                                       causes to change or exit the current HDR
                                       system */

  sys_active_prot_e_type hdr_active_prot;
#ifdef FEATURE_EHRPD
  sys_personality_e_type hdr_personality;   
#endif
  uint16 phys_subtype = HDRSCP_DEFAULT_PHYS_LAYER;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -esym(613, cache_entry_ptr) cache_entry_ptr is not NULL */
  /*lint -esym(613, ind_to_give_ptr) ind_to_give_ptr is not NULL */
  ASSERT(( cache_entry_ptr != NULL ) && (ind_to_give_ptr != NULL ));

  continue_curr_hdr_system = TRUE;

  /*---------------------------------------------------------------------
  Point current cache pointer to the inputted entry. Update timestamp on
  this entry & ovhd info state.
  ---------------------------------------------------------------------*/
  HDR_MSG_PROT(MSG_LEGACY_LOW, "cache changed: hdrovhd_process_cache_update");
  hdrovhd.cache_changed = TRUE;

  hdrovhd.curr_cache_entry_ptr = cache_entry_ptr;
 
  (void) time_get( hdrovhd.curr_cache_entry_ptr->ts_implicit );

  hdrovhd.info_state = HDROVHD_INFO_UPDATED;

  (void) rex_set_timer( &hdrovhd.sp_insanity_timer, 
                        HDROVHD_INSANITY_TIMER_MSECS );
  
  /* Update RouteUpdateTriggerCodeList */
  hdrovhd_update_route_update_trigger_code_list( 
    hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg.route_update_trigger_code,
    hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg.route_update_trigger_max_age,
    hdrovhd.curr_cache_entry_ptr->ts_implicit,
    &hdrovhd.curr_subnet );

    /*---------------------------------------------------------------------
    Configure the AT operational mode based on 1x neighbors in SPM.
    ---------------------------------------------------------------------*/
    hdrovhd_configure_at_operational_mode(
          &hdrovhd.curr_cache_entry_ptr->sp_entry);

  if ( !hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY )|| 
        hdrsmp_session_is_closing( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL)  )/* If the session is closing,  
                                                                                                                                   report null personality */
  {
    hdr_active_prot = SYS_ACTIVE_PROT_NONE;
#ifdef FEATURE_EHRPD
    hdr_personality = SYS_PERSONALITY_NONE;
#endif
  }
  else
  {
    /* To notify SD if the phone is currently in rel0 or relA mode. */
    phys_subtype = hdrscp_get_current_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

    switch(phys_subtype)
    {
      case HDRSCP_SUB3_PHYS_LAYER:
        hdr_active_prot = SYS_ACTIVE_PROT_HDR_RELB;
        break;
      case HDRSCP_SUB2_PHYS_LAYER:
        hdr_active_prot = SYS_ACTIVE_PROT_HDR_RELA;
        break;
      case HDRSCP_DEFAULT_PHYS_LAYER:
        hdr_active_prot = SYS_ACTIVE_PROT_HDR_REL0;
        break;
      default:
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                       "Invalid phys layer subtype %d", 
                       phys_subtype);
        hdr_active_prot = SYS_ACTIVE_PROT_NONE;
    }

    /* To notify SD if the phone is in HRPD or eHRPD personality */
#ifdef FEATURE_EHRPD
    if ( hdrmrlpc_current_personality_is_ehrpd() )
    {
      hdr_personality = SYS_PERSONALITY_EHRPD;
    }
    else 
    {
      hdr_personality = SYS_PERSONALITY_HRPD;
    }
#endif
  }

  /* Remember current channel record*/
  hdrovhd.curr_subnet.ch.band = hdrovhd.qc_info.link.channel.band;
  hdrovhd.curr_subnet.ch.chan_num = hdrovhd.qc_info.link.channel.chan_num;
  HDR_MSG_PROT_4(MSG_LEGACY_MED, "(curr band=%d chan=%d, last band=%d chan=%d)",
                 hdrovhd.curr_subnet.ch.band, 
                 hdrovhd.curr_subnet.ch.chan_num, 
                 hdrovhd.last_sd_subnet.ch.band,
                 hdrovhd.last_sd_subnet.ch.chan_num);
  
  /*---------------------------------------------------------------------
  Inform System Determination of the Channel & Sector ID for which the
  ovhd info has been updated. Note that we inform SD of the channel on
  which msg is rcvd & not of channel to which it hashes. This is because
  1. When RMAC is active, the AT remains on the current channel even though
  the SP msg may hash to a different one. 2. When RMAC is inactive, as a
  condition of the cache being updated, the current channel and hashed channel
  are the same.
  ---------------------------------------------------------------------*/
  if ( hdrovhd.last_sd_subnet.is_valid         &&
      (hdrovhd.last_sd_subnet.subnet_mask ==
       hdrovhd.curr_subnet.subnet_mask)        &&
       hdraddr_is_equal(hdrovhd.last_sd_subnet.subnet_id,
                        hdrovhd.curr_subnet.subnet_id) &&
       SYS_CHAN_CMP_EQUAL(hdrovhd.last_sd_subnet.ch, hdrovhd.curr_subnet.ch))
  {

    /*----------------------------------------------------------------------
    The current subnet matches the one used when sd_ss_ind_hdr_opr_oh_info()
    was called last, so there is no need to call it again.  This is an
    optimization to improve sleep time.  Just give the overhead updated
    indication. Note that it would previously have been recorded if this
    is the first time network has been acquired.  See the processing in
    hdrovhd_process_cache_entry()
    -----------------------------------------------------------------------*/
    ind_to_give_ptr->need_to_give_ind = TRUE;
    ind_to_give_ptr->ind_name = HDRIND_OVHD_INFO_UPDATED;
    continue_curr_hdr_system = TRUE;    
  }
  else
  {
#ifdef FEATURE_QCHAT
    if(qchatupk_lu_enabled())
    {
      if(!hdraddr_is_equal(hdrovhd.last_sd_subnet.subnet_id,
                         hdrovhd.curr_subnet.subnet_id))
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Inform QChat about new sector location");
        qchatupk_update_location(SYS_SYS_MODE_HDR);
      }
    }
#endif /* FEATURE_QCHAT */
#ifdef FEATURE_MEMCPY_REMOVAL
    hdrovhd.last_sd_subnet = hdrovhd.curr_subnet; /* curr_subnet is always valid here */
#else
    memcpy ((void*)&hdrovhd.last_sd_subnet, /* curr_subnet is always valid here */
            (void*)&hdrovhd.curr_subnet,
            sizeof(hdrovhd_subnet_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
#ifdef FEATURE_EHRPD
    switch ( sd_ss_ind_hdr_opr_oh_info6( cache_entry_ptr->rx_link.channel.band,
                    cache_entry_ptr->rx_link.channel.chan_num,
                    cache_entry_ptr->sp_entry.sp_msg.sector_id,
                    cache_entry_ptr->sp_entry.sp_msg.subnet_mask,
                    TRUE,
                    cache_entry_ptr->sp_entry.sp_msg.channel_list.chan,
                    cache_entry_ptr->sp_entry.sp_msg.channel_count,
                    hdr_active_prot,
                    hdr_personality,
                    cache_entry_ptr->sp_entry.sp_msg.local_time_offset,
                    cache_entry_ptr->sp_entry.sp_msg.leap_secs,
					cache_entry_ptr->sp_entry.sp_msg.country_code,
					cache_entry_ptr->sp_entry.sp_msg.latitude,
					cache_entry_ptr->sp_entry.sp_msg.longitude,
					cache_entry_ptr->rx_link.mode,
                    NULL ))
#else
    switch ( sd_ss_ind_hdr_opr_oh_info4( cache_entry_ptr->rx_link.channel.band,
                    cache_entry_ptr->rx_link.channel.chan_num,
                    cache_entry_ptr->sp_entry.sp_msg.sector_id,
                    cache_entry_ptr->sp_entry.sp_msg.subnet_mask,
                    TRUE,
                    cache_entry_ptr->sp_entry.sp_msg.channel_list.chan,
                    cache_entry_ptr->sp_entry.sp_msg.channel_count,
                    hdr_active_prot,
                    cache_entry_ptr->sp_entry.sp_msg.local_time_offset,
                    cache_entry_ptr->sp_entry.sp_msg.leap_secs,
                    NULL ))
#endif /* FEATURE_EHRPD */
    {
        case SD_SS_ACT_CONTINUE:
          /*-------------------------------------------------------------------
          Give the overhead updated indication. Note that it would previously
          have been recorded if this is the first time network has been
          acquired.  See the processing in hdrovhd_process_cache_entry()
          -------------------------------------------------------------------*/
          ind_to_give_ptr->need_to_give_ind = TRUE;
          ind_to_give_ptr->ind_name = HDRIND_OVHD_INFO_UPDATED;
          continue_curr_hdr_system = TRUE;
          break;

        case SD_SS_ACT_ACQ_HDR:
          /*-------------------------------------------------------------------
          Change HDR system. Update parameters, give indication & free buffers
          -------------------------------------------------------------------*/
          ind_to_give_ptr->need_to_give_ind = TRUE;
          ind_to_give_ptr->ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
          continue_curr_hdr_system = FALSE;
          break;

        default:
          /*-------------------------------------------------------------------
          Exit HDR system. Update parameters, give indication & free buffers.
          -------------------------------------------------------------------*/
          ind_to_give_ptr->need_to_give_ind = TRUE;
          ind_to_give_ptr->ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
          continue_curr_hdr_system = FALSE;
          break;
    }
  }

  /*lint +esym(613, cache_entry_ptr) cache_entry_ptr is not NULL */
  /*lint +esym(613, ind_to_give_ptr) ind_to_give_ptr is not NULL */
  return continue_curr_hdr_system;

} /* hdrovhd_process_cache_update() */


/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_GIVE_IND

DESCRIPTION
  This function gives the appropriate indication according to the input
  parameter ind_to_give_ptr.

DEPENDENCIES
  ind_to_give_ptr should not be NULL

PARAMETERS
  ind_to_give_ptr - Pointer to the structure describing the indication to be
  given.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL void hdrovhd_give_ind
(
  hdrovhd_ind_to_give_type *ind_to_give_ptr
)
{
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience sp msg pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sp_msg_ptr = &hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg;

  /*lint -esym(613, ind_to_give_ptr) ind_to_give_ptr is not NULL */
  ASSERT( ind_to_give_ptr != NULL );

  if( ind_to_give_ptr->need_to_give_ind )
  {
    switch( ind_to_give_ptr->ind_name )
    {
      case HDRIND_OVHD_INFO_UPDATED:
        /*---------------------------------------------------------------------
        Give the overhead info updated indication & flag if it is the
        first one after acquiring the network. Log the sector info and
        Trace this event when Idle. Update Reverse link Silence Parameters
        ---------------------------------------------------------------------*/
        if( !hdrrup_is_tc_assigned( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) )
        {
          HDRTRACE_OVHD_INFO_UPDATED();
        }

        /*---------------------------------------------------------------------
        Update the Reverse Link SilenceParameters in the RMAC & AMAC
        ---------------------------------------------------------------------*/
        hdrrmac_set_silence_params( sp_msg_ptr->silence_duration,
                                    sp_msg_ptr->silence_period );
        hdramac_set_silence_params( sp_msg_ptr->silence_duration,
                                    sp_msg_ptr->silence_period );

        hdrind_give_ind( ind_to_give_ptr->ind_name ,
                         &hdrovhd.is_first_after_acquisition );

#ifdef FEATURE_HDR_TO_LTE
        hdrovhd_update_splmn_info(&hdrovhd.curr_cache_entry_ptr->qc_entry, NULL);
#endif /* FEATURE_HDR_TO_LTE */

        hdrovhd.is_first_after_acquisition = FALSE;

        hdrplog_log_sector_info( sp_msg_ptr->sector_id,
                                 sp_msg_ptr->subnet_mask,
                                 hdrovhd.qc_info.color_code,
                                 hdrovhd.qc_info.link.pilot_pn);
        break;

      case HDRIND_OVHD_CHANGE_HDR_SYSTEMS:
      case HDRIND_OVHD_EXIT_HDR_MODE :

        hdrovhd.is_first_after_acquisition = FALSE;

        /*---------------------------------------------------------------------
        Give Change HDR or Exit HDR system ind & change state to Inactive
        ---------------------------------------------------------------------*/
        hdrind_give_ind( ind_to_give_ptr->ind_name , NULL );
        break;

      default :
        /*---------------------------------------------------------------------
        Unrecognized indication.
        ---------------------------------------------------------------------*/
        ERR("Unrecognized Indication", 0, 0, 0 );
        break;
    }
  }
  /*lint +esym(613, ind_to_give_ptr) ind_to_give_ptr is not NULL */

  /*---------------------------------------------------------------------
  Give AP updated indication, if necessary
  ---------------------------------------------------------------------*/
  if (ind_to_give_ptr->need_to_give_ap_ind)
  {
    hdrind_give_ind( HDRIND_OVHD_AP_UPDATED , NULL );
  }

} /* hdrovhd_give_ind() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_COPY_UNPACKED_SP_MSG_TO_CACHE

DESCRIPTION
  This function copies the unpacked SP message to the cache entry.  

DEPENDENCIES
  None

PARAMETERS
  node_ptr - Pointer to cache entry into which the unpacked SP message 
             has to be copied
  sp_ptr - Pointer to the unpacked SP message.

RETURN VALUE
  err - the result of copying SP message to the cache entry.

SIDE EFFECTS
  None

=============================================================================*/
static hdrerrno_enum_type hdrovhd_copy_unpacked_sp_msg_to_cache
(
  hdrovhd_cache_entry_type *node_ptr,
  hdrovhd_sp_msg_type *sp_ptr
)
{
#ifdef FEATURE_HDR_BCMCS_MFRQ
  sys_channel_type bc_channel;
#endif /* FEATURE_HDR_BCMCS_MFRQ */

  sys_channel_type expected_channel;
  hdrerrno_enum_type err = E_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_LOW, "Ovhd cache changed: hdrovhd_unpack_sp_msg");
  hdrovhd.cache_changed = TRUE;

  /*---------------------------------------------------------------------------
  Check if node is in use, that is, either being read from or written to.
  This should never be the case. It can be being read from only if it
  is the current node, which is impossible. It cannot be being written to,
  since this would imply we are still processing some previous SP message,
  which implies something terrible is wrong.
  ---------------------------------------------------------------------------*/
  if( node_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID &&
      node_ptr->sp_entry.state != HDROVHD_BUF_NOT_IN_USE )
  {
    ERR("Cannot write to cache - SP node is in use!", 0, 0, 0);
    return E_NOT_AVAILABLE;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents.
  ---------------------------------------------------------------------------*/
  node_ptr->cache_entry_state = HDROVHD_ENTRY_VALID;
  node_ptr->sp_entry.state = HDROVHD_BUF_WRITE_IN_PROG;

  /*---------------------------------------------------------------------------
  Copy the SectorID, Access Network ID & channel for Sector which sent the
  msg. These parameters for the msg are same as the previous received
  QuckConfig message. This is assured, since we check that the link id fields
  match when we receive the SectorParamters message.
  ---------------------------------------------------------------------------*/
  ASSERT( hdrovhd.qc_info.initialized );
  node_ptr->rx_link = hdrovhd.qc_info.link;
  /* We only keep the whole link info (i.e. w/ pilot_pn) so we can tell SRCH 
   * what pilots are in the cache, so it can optimize cheap vs. expensive idle 
   * handoffs.
   */

#ifdef FEATURE_MEMCPY_REMOVAL
  node_ptr->sp_entry.sp_msg = *sp_ptr;
#else
  memcpy (&node_ptr->sp_entry.sp_msg, sp_ptr, sizeof(hdrovhd_sp_msg_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /*-----------------------------------------------------------------------
  Check if the channel that OVHD hashed to is different from the channel
  that msg was received on. If not then mark the node as valid. Otherwise
  if AT is Idle mark the node as invalid. If connected, mark it as being
  valid only in connected - That way this entry will not be used while Idle,
  but will actually cause a channel hash
  -----------------------------------------------------------------------*/
  expected_channel = sp_ptr->hash_channel;

  #ifdef FEATURE_HDR_BCMCS_MFRQ
  /* If BCMCS is going to override channel hashing we need to change our
   * expected channel to be the BCMCS channel instead.  The best 
   * implementation here is to go ahead and mark the entry as valid even 
   * if we are going to change channels.  That would require more risky
   * changes that we don't have time to work out now.
   */
  if ((hdrbccp_get_preferred_bc_channel( &bc_channel, NULL ) == E_SUCCESS) &&
      (hdrovhd_bcmcs_is_overriding_hashing( node_ptr, &bc_channel, TRUE )))
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED,
                   "override hashing: expected_channel update (sp hash %d-> bc hash%d)",
                   expected_channel.chan_num, bc_channel.chan_num);
    expected_channel = bc_channel;
  }
  /* If the AT does not need to override channel hashing for BCMCS, make sure
     that it will not try to override SP hashing later, when processing 
     bcovhd_info_updated() */
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Do not override SP channel hashing for BCMCS");
    hdrovhd.ignore_bc_hash_override = TRUE;
  }
  #endif /* FEATURE_HDR_BCMCS_MFRQ */
  
  /* For now don't let BCMCS interfere with the HDROVHD_ENTRY_VALID_WHEN_CONN
   * logic even though this logic doesn't necesarily make sense for BCMCS
   * capable devices.  This is to minimize the risk of BCMCS related changes 
   * in this software branch.
   */
  if((SYS_CHAN_CMP_EQUAL(node_ptr->rx_link.channel,sp_ptr->hash_channel)) &&
     (hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL)))
  {
    /* In traffic on our hash channel... */
    node_ptr->cache_entry_state = HDROVHD_ENTRY_VALID;
    node_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;
  }
  else if(hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL))
  {
    /* In traffic not on our hash channel... */
    node_ptr->cache_entry_state = HDROVHD_ENTRY_VALID_WHEN_CONN;
    node_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;
  }
  else if(SYS_CHAN_CMP_EQUAL(node_ptr->rx_link.channel, expected_channel))
  {
    /* Not in traffic.
     * If BCMCS overriding hashing - not on our hash channel, but still on
     * our preferred BCMCS channel.
     * If BCMCS not overriding hashing - we already are on our SP hash
     * channel.
     */
    node_ptr->cache_entry_state = HDROVHD_ENTRY_VALID;
    node_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;
    HDR_MSG_PROT_1(MSG_LEGACY_MED,
                   "Already on expected channel (=curr channel=%d)",
                   expected_channel.chan_num);
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
      "Can't save SP msg to cache (curr channel=%d expected channel=%d)",
      node_ptr->rx_link.channel.chan_num,expected_channel.chan_num);
    node_ptr->cache_entry_state = HDROVHD_ENTRY_INVALID;
  }

  return err;

} /* hdrovhd_copy_unpacked_sp_msg_to_cache() */

#ifdef FEATURE_HDR_REVC
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_UNPACK_SP_MSG_NEIGBHORS

DESCRIPTION
  This function unpacks Neighbors inside a SectorParameters message.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  E_SUCCESS - If unpacking succeded
  E_NO_DATA - If unpacking failed

SIDE EFFECTS 
  neighbors structure is modified regardless even when unpack failed 

=============================================================================*/
LOCAL hdrerrno_enum_type hdrovhd_unpack_sp_msg_neigbhors
(
  dsm_item_type *item_ptr,
  sys_link_type *serving_sector,
  hdrovhd_sp_msg_type *sp_ptr
)
{
  uint8 i;
  uint8 temp;
  uint16 pos;
  hdrerrno_enum_type err;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* skip to channel count */
  pos = HDROVHD_SP_CHANNEL_COUNT_OFFSET;

  /* unpack channel count and skip channel list */

  sp_ptr->channel_count = 0;
  err = HDRBIT_UNPACK8(item_ptr, &sp_ptr->channel_count, pos, 
                       HDROVHD_SP_CHANNEL_COUNT_SIZE);
  
  pos += HDROVHD_SP_CHANNEL_COUNT_SIZE + 
         sp_ptr->channel_count * HDROVHD_SP_CHANNEL_RECORD_SIZE;

  /* unpack neighbor list */
  sp_ptr->num_neighbors = 0;
  err |= HDRBIT_UNPACK8(item_ptr, &sp_ptr->num_neighbors, pos, 
                        HDROVHD_SP_NEIGHBOR_COUNT_SIZE);
  pos += HDROVHD_SP_NEIGHBOR_COUNT_SIZE;
  
  /* unpack PN info for neighbor list */
  for (i=0;i<sp_ptr->num_neighbors;i++)
  {
    HDRBIT_UNPACK16(item_ptr, &sp_ptr->nset[i].link.pilot_pn, pos, 
                    HDROVHD_SP_NEIGHBOR_PN_SIZE);
    pos += HDROVHD_SP_NEIGHBOR_PN_SIZE;
  }

  /* unpack channel info for neighbor list */
  for (i=0;i<sp_ptr->num_neighbors;i++)
  {
    temp = 0;
    err |= HDRBIT_UNPACK8(item_ptr, &temp, pos, 
                          HDROVHD_SP_NEIGHBOR_CHAN_INCL_SIZE);
    pos += HDROVHD_SP_NEIGHBOR_CHAN_INCL_SIZE;

    if (temp)
    {
      err |= HDRBIT_UNPACK8(item_ptr, &temp, pos, 
                             HDROVHD_SP_SYSTEM_TYPE_SIZE);
      sp_ptr->nset[i].link.mode = (sys_hdr_chan_rec_mode_e_type) temp;
      pos += HDROVHD_SP_SYSTEM_TYPE_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &temp, pos, 
                             HDROVHD_SP_BAND_CLASS_SIZE);
      sp_ptr->nset[i].link.channel.band = (sys_band_class_e_type) temp;
      pos += HDROVHD_SP_BAND_CLASS_SIZE;

      err |= HDRBIT_UNPACK16(item_ptr, 
                             &sp_ptr->nset[i].link.channel.chan_num, pos, 
                            HDROVHD_SP_CHANNEL_NUMBER_SIZE);
      pos += HDROVHD_SP_CHANNEL_NUMBER_SIZE;
    }
    else
    {
      sp_ptr->nset[i].link.mode = serving_sector->mode;
      sp_ptr->nset[i].link.channel = serving_sector->channel;
    }
	
	  HDR_MSG_PROT_4(MSG_LEGACY_MED, "unpack_sp_nbr chan %d pn %d @ %d err %d",
	                 sp_ptr->nset[i].link.channel.chan_num,
			        	   sp_ptr->nset[i].link.pilot_pn,
				           pos, err);
  }
  
  return err;
}

#endif /* FEATURE_HDR_REVC */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_PROCESS_SP_MSG

DESCRIPTION
  This function processes a received SectorParameters message.

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS

=============================================================================*/
LOCAL void hdrovhd_process_sp_msg
(
  dsm_item_type *item_ptr
)
{
  sys_link_type sp_link;           /* Link on which SP msg is received */
  uint16 sp_sector_signature;      /* SectorSig rcvd in SP msg */
  hdrsrch_link_id_type link_id;    /* LinkID of rcvd msg */
  int err = 0;                     /* Result of unpacking SP msg.0=E_SUCCESS */
  uint8  sp_subnet_mask;           /* Subnet Mask rcvd in SP msg */
  hdraddr_type sp_sector_id_128;   /* SectorID rcvd in SP msg */
  hdrovhd_cache_entry_type *stored_node_ptr; /* Used in searching cache */
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Local pointer to current
                                   ** cache entry */
  hdrovhd_cache_entry_type *available_node_ptr = NULL; /* Used in finding the oldest
                                   ** entry or unused entry in cache */
  hdrovhd_ind_to_give_type ind_to_give;  /* Indication to be given at
                                   **  end of processing SP msg */
  db_items_value_type db_item;     /* To put time-of-day values in DB for UI */
  sd_ss_act_e_type sd_action;      
                             /* Continue, acquire HDR, enter power save etc. */
  boolean using_duplicate_node = FALSE;
           /* Flag if we are using a second cache entry for double-buffering */
  hdrovhd_cache_entry_type *new_cache_entry_ptr;
                   /* Pointer to location where we store SP and QC message */
  boolean chan_was_changed;
  hdrerrno_enum_type err_copy_cache = E_SUCCESS; /* Result of copying SP msg */
  uint16 st_cache_period = 0;

#ifdef FEATURE_HDR_REVC
  hdrovhd_nlb_cache_type *cache_ptr;
  time_type cur_time;
  sys_channel_type *pri_cc_chan;
#endif /* FEATURE_HDR_REVC */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*---------------------------------------------------------------------------
  If Overhead protocol is not active, do not process message
  ---------------------------------------------------------------------------*/
  if( hdrovhds_get_state() != HDROVHDS_PROCESS_ALL_MSGS )
  {
    HDRTRACE_OVHD_MSG_IGNORED_STATE( HDROVHD_SP_MSG_ID,
                                      hdrovhds_get_state() );
    return;
  }

  /*---------------------------------------------------------------------------
  Get the link_id for received message. Compare the link_id fields
  with those received for previous QuickConfig message. If the fields
  do not match, then do not process message
  ---------------------------------------------------------------------------*/
  link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );

  /*  Call LMAC function to get the HDR channel & PN offset associated with
  this link_id.*/
  hdrlmac_get_link_info( link_id, &sp_link );

  /*---------------------------------------------------------------------------
  If received Sector Signature does not match expected sector signature
  from previous Quick Config message, then do not process the message
  ---------------------------------------------------------------------------*/
  err |= (int) HDROVHD_SP_SECTOR_ID(item_ptr, sp_sector_id_128) ;
  err |= (int) HDROVHD_SP_SUBNET_MASK(item_ptr, &sp_subnet_mask) ;
  err |= (int) HDROVHD_SP_SECTOR_SIGNATURE(item_ptr, &sp_sector_signature) ;

  if( err != 0 )
  {
    ERR("Discarding SP msg. Error extracting fields", 0, 0, 0 );
    return;
  }

#ifdef FEATURE_HDR_REVC
  pri_cc_chan = hdrlmac_get_cc_monitoring_chan();
  ASSERT(pri_cc_chan != NULL);
  /* SP not received on pri CC */
  if ((pri_cc_chan->band != sp_link.channel.band ||
       pri_cc_chan->chan_num != sp_link.channel.chan_num))
  {
    if (hdralmps_get_state() != HDRALMPS_CONNECTED)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_MED, "proc_sp SP on non pri chan %d in state %d",
                     sp_link.channel.chan_num, hdralmps_get_state());
    }

    cache_ptr = hdrovhd_find_nlb_cache(&sp_link);

    time_get(cur_time);

    if (cache_ptr != NULL &&
        !hdrovhd_nlb_cache_has_expired(cache_ptr, cur_time) &&
        hdrovhd_sector_id_24_128_is_equal( cache_ptr->sector24,
                                           sp_sector_id_128 ) &&
        cache_ptr->sector_sig == sp_sector_signature)
    {
      if (cache_ptr->state == HDROVHD_NLB_CACHE_QC_RCVD)
      {
        /*---------------------------------------------------------------------------
        Clear the temporary sp message buffer
        ---------------------------------------------------------------------------*/
        memset( &hdrovhd.sp_buffer, 0, sizeof(hdrovhd_sp_msg_type) );

        /*---------------------------------------------------------------------------
        Unpack the received message in to the temp buffer
        ---------------------------------------------------------------------------*/
        err = hdrovhd_unpack_sp_msg( item_ptr, &hdrovhd.sp_buffer );

        if (err == E_SUCCESS)
        {
          hdrovhd_unpack_sp_msg_neighbors(item_ptr, &sp_link,
                                          &hdrovhd.sp_buffer);

          hdrovhd_process_sp_neighbors(&sp_link, 
                                       cache_ptr, &hdrovhd.sp_buffer);
        }
      }

      qw_equ(cache_ptr->timestamp, cur_time);
    }

    return;
  }
#endif /* FEATURE_HDR_REVC */

  if( (!hdrovhd.qc_info.initialized) ||
      (!SYS_CHAN_CMP_EQUAL( sp_link.channel, hdrovhd.qc_info.link.channel )) ||
      (sp_link.pilot_pn != hdrovhd.qc_info.link.pilot_pn ) )
  {
    HDRTRACE_OVHD_IGNORED_MSG_UNEXPECTED_LINK( HDROVHD_SP_MSG_ID,
                        hdrovhd.qc_info.link.channel.chan_num,
                        hdrovhd.qc_info.link.pilot_pn,
                        sp_link.channel.chan_num,
                        sp_link.pilot_pn);
    return;
 }

  /*---------------------------------------------------------------------------
  Ignore msg if the SectorSignature does not match expected from QC msg
  ---------------------------------------------------------------------------*/
  if( ( sp_sector_signature != hdrovhd.qc_info.sector_signature ) )
  {
    HDRTRACE_OVHD_IGNORED_SP_MSG_DIFF_SEC_SIG(
                 hdrovhd.qc_info.sector_signature , sp_sector_signature );
    return;
  }

  /*---------------------------------------------------------------------------
  Ignore msg if subnet mask is more than 128 bits.
  ---------------------------------------------------------------------------*/
  if ( sp_subnet_mask > HDRADDR_LENGTH_IN_BITS )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Subnet mask is more than 128 bits");
    return;
  }

  /*---------------------------------------------------------------------------
  Ignore msg if the SectorID does not match the expected from QC msg. This
  should generally never happen, since we check the link to ensure that the
  SP msg is received on the same link as the QC msg.
  ---------------------------------------------------------------------------*/
  if( !hdrovhd_sector_id_24_128_is_equal( hdrovhd.qc_info.sector_id_24,
                                          sp_sector_id_128 ) )
  {
    HDRTRACE_OVHD_IGNORED_SP_MSG_DIFF_SEC_ID( );
    return;
  }

  /*---------------------------------------------------------------------------
  Restart SP timer
  ---------------------------------------------------------------------------*/
  (void) rex_set_timer(&hdrovhd.sp_supervision_timer , HDROVHD_SP_TIMER_MSECS);

  /*---------------------------------------------------------------------------
  If control channel is current, then no need for further processing of
  message. Also ensure SectorID is same.
  ---------------------------------------------------------------------------*/
  if ( ( hdrovhd.info_state == HDROVHD_INFO_UPDATED ) &&
         hdraddr_is_equal( curr_cache_entry_ptr->sp_entry.sp_msg.sector_id,
                           sp_sector_id_128) )
  {
    HDRTRACE_OVHD_MSG_IGNORED_REDUNDANT( HDROVHD_SP_MSG_ID );

    /* AN is still working correctly; resetting insanity timer. */
    (void) rex_set_timer( &hdrovhd.sp_insanity_timer,
                          HDROVHD_INSANITY_TIMER_MSECS );

    return;
  }

  /*---------------------------------------------------------------------------
   When ALMP is going to be deactivated next, ignore processing the SP msg.   
  ---------------------------------------------------------------------------*/
  if((hdralmps_get_pending_state() == HDRALMPS_INACTIVE) &&
     (hdralmps_get_state() == HDRALMPS_CONNECTED))
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Not processing SP msg, because ALMP will be deactivated shortly.");
	return;  
  }

  
  /*---------------------------------------------------------------------------
  Store the current Subnet.( ie, combination of SectorID & Subnet Mask )
  ---------------------------------------------------------------------------*/
  hdraddr_mask( hdrovhd.curr_subnet.subnet_id ,
                sp_sector_id_128,
                sp_subnet_mask );
  hdrovhd.curr_subnet.subnet_mask = sp_subnet_mask;
  hdrovhd.curr_subnet.is_valid = TRUE;

  /*---------------------------------------------------------------------------
  Initialize indication data structure
  ---------------------------------------------------------------------------*/
  hdrovhd_reset_ind_to_give( &ind_to_give );

  /*---------------------------------------------------------------------------
  Clear the temporary sp message buffer
  ---------------------------------------------------------------------------*/
  memset( &hdrovhd.sp_buffer, 0, sizeof(hdrovhd_sp_msg_type) );

  /*---------------------------------------------------------------------------
  Unpack the received message in to the temp buffer
  ---------------------------------------------------------------------------*/
  err = hdrovhd_unpack_sp_msg( item_ptr, &hdrovhd.sp_buffer );

#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_lte_to_hdr_get_cgi_is_in_progress())
  {
    if (err == E_SUCCESS)
    {
      hdrovhd.cgi_info_is_valid = TRUE;
    }
    else
    {
      hdrovhd.cgi_info_is_valid = FALSE;
    }

    /* Save pilot information since we can't get pilot information once
     * SRCH is deactivated. */
    if ( hdrlmac_get_pilots( 1, &hdrovhd.cgi_info_pilot_rpt, 
                             HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) )
    {
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "CGI pilot rpt: PN %d, strength %d",
                      hdrovhd.cgi_info_pilot_rpt.link.pilot_pn,
                      hdrovhd.cgi_info_pilot_rpt.strength );
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Can't get pilot report!" );   
    }

    ind_to_give.need_to_give_ind = TRUE;
    ind_to_give.ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
    hdrovhd_give_ind( &ind_to_give );
    return;
  }
#endif /* FEATURE_LTE_TO_HDR */

  if (err == E_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    Look for a matching SP msg in the cache
    ---------------------------------------------------------------------------*/
    /* Get the current value for short term overhead cache period */
    st_cache_period = hdrovhd_get_cache_period_attrib(
                          HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID,
                          HDRSCP_DEFAULT_OVHD );

    /* Use timer based caching mechanism only when timer value is greater than 
       zero. Otherwise fall back to legacy caching based on sector id 128  */
    if (st_cache_period > 0)
    {
      stored_node_ptr = hdrovhd_find_cached_msg_using_qc_info( 
                            hdrovhd.qc_info.link,
                            hdrovhd.qc_info.color_code,
                            hdrovhd.qc_info.sector_id_24,
                            hdrovhd.qc_info.sector_signature);
    }
    else
    {
      stored_node_ptr = hdrovhd_find_cached_msg_using_sp_info( 
                            sp_sector_id_128,
                            sp_sector_signature,
                            hdrovhd.qc_info.link.channel );
    }

    if( stored_node_ptr == NULL )
    {
      HDR_MSG_PROT (MSG_LEGACY_LOW, "CACHE: (SP) didn't find cache ");
      /*-------------------------------------------------------------------------
      Entry is not found in cache. Replace an invalid entry, if one exists,
      or the oldest entry with new SP msg & make this the current entry.
      -------------------------------------------------------------------------*/
      available_node_ptr = hdrovhd_find_available_cache_node(&hdrovhd.sp_buffer);

      new_cache_entry_ptr = available_node_ptr;

    }
    else     /* Entry is found in cache */
    {
      /*-------------------------------------------------------------------------
      Check if the current sp msg pointer points to the found entry
      -------------------------------------------------------------------------*/
      if( stored_node_ptr  == curr_cache_entry_ptr )
      {
        /*-----------------------------------------------------------------------
        The current pointer points to the found entry. The current
        entry cannot be overwritten since it may being read from & overwritting
        may cause a race condition. Instead, an unused node or the oldest
        entry is overwritten, the current node invalidated, & then the current
        sp msg pointer updated to this node
        -----------------------------------------------------------------------*/
        available_node_ptr = hdrovhd_find_available_cache_node(&hdrovhd.sp_buffer);

        /*-----------------------------------------------------------------------
        If the current pointer is same as the available node pointer. Something 
        has gone wrong with the overhead cache timestamp so clear the overhead 
        cache and return.
        -----------------------------------------------------------------------*/
        if ( available_node_ptr == curr_cache_entry_ptr )
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR,
            "Current cache ptr same as available node ptr, clearing cache");
          hdrovhd_clear_cache();
          return;
        }
        else 
        {
          /*-----------------------------------------------------------------------
          Make a copy of the current cache node.
          -----------------------------------------------------------------------*/
          if ( hdrovhd_duplicate_cache_entry( curr_cache_entry_ptr,
                                              available_node_ptr ) != E_SUCCESS )
          {
            /* If we fail to copy the cache entry, neither the curr_cache_entry_ptr
             * nor the available_node_ptr are modified at this point.  
             */
            ERR("Could not copy cache entry: sp msg not updated",0 ,0 ,0);
            hdrovhd_clear_cache_entry_and_state( curr_cache_entry_ptr );
            return;
          }
        }

        /*-----------------------------------------------------------------------
        Unpack the SP msg in the newly copied entry.
        -----------------------------------------------------------------------*/

        new_cache_entry_ptr = available_node_ptr;
        using_duplicate_node = TRUE;
      }
      else
      {
        /*-----------------------------------------------------------------------
        Found node is not current node. Overwrite the cache entry with the
        latest version of the SP for this sector.
        -----------------------------------------------------------------------*/

        new_cache_entry_ptr = stored_node_ptr;
      }

    if ( hdrovhd.ap_state == HDROVHD_AP_UPDATED)
    {
      /* If we actually process SP message, it means that hdrovhd is not updated. 
       * AP should not'be updated.
       * Something is wrong, but we can continue by making ap_state not updated*/
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"AP State Shouldn't be updated"); 
      hdrovhd.ap_state = HDROVHD_AP_NOT_UPDATED;
    }
      /* Always wait for new AP if can't find cached SP by using QC */
      HDR_MSG_PROT(MSG_LEGACY_MED,"after receiving new SP, always wait for AP"); 
      new_cache_entry_ptr->ap_entry.is_valid = FALSE;
    }

    err_copy_cache = hdrovhd_copy_unpacked_sp_msg_to_cache(new_cache_entry_ptr, 
                                                           &hdrovhd.sp_buffer);

    if (err_copy_cache == E_SUCCESS)
    {
      /* Once the AT has received the SP message, cache the QC message along
         with it. This is needed because when QuickPaging is enabled AT can 
         go to sleep without waiting for QC message */
#ifdef FEATURE_MEMCPY_REMOVAL
      new_cache_entry_ptr->qc_entry = hdrovhd.qc_info;
#else
      memcpy( &new_cache_entry_ptr->qc_entry, &hdrovhd.qc_info, 
              sizeof(hdrovhd_qc_info_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* Cache the QC message only in idle state */
      if ( !hdrrup_is_tc_assigned(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL) )
      {
        new_cache_entry_ptr->qc_is_valid = TRUE;
      }

      if (using_duplicate_node)
      {
        HDR_MSG_PROT (MSG_LEGACY_LOW, "CACHE, clear cache entry");
         hdrovhd_clear_cache_entry( curr_cache_entry_ptr );
      }
    
      curr_cache_entry_ptr = new_cache_entry_ptr;

      /*-------------------------------------------------------------------------
       Update the cache entry with the current ConfigurationChange values. 
       ------------------------------------------------------------------------*/
      curr_cache_entry_ptr->config_change = hdridle_get_current_config_change();

      /*-------------------------------------------------------------------------
      Put necessary values in DB to enable correct time-of-day display for UI
      -------------------------------------------------------------------------*/
      db_item.ltm_off = curr_cache_entry_ptr->sp_entry.sp_msg.local_time_offset /
                        30;
      db_put( DB_LTM_OFF, &db_item );
  
      db_item.lp_sec = curr_cache_entry_ptr->sp_entry.sp_msg.leap_secs;
      db_put( DB_LP_SEC, &db_item );
  
      /*-------------------------------------------------------------------------
      Do the processing necessary for new entry to be current entry.
      -------------------------------------------------------------------------*/
      (void) hdrovhd_process_cache_entry( curr_cache_entry_ptr, &ind_to_give, 
                                          &sp_link, &chan_was_changed );
  
      /*-------------------------------------------------------------------------
      If SP is updated, then copy temp msg to cache if it exists
      -------------------------------------------------------------------------*/
      if (hdrovhd.info_state == HDROVHD_INFO_UPDATED)
      {
        hdrovhd_copy_temp_msg_to_cache(curr_cache_entry_ptr);
        
        /* Update both the explicit and implicit timestamp when SP message 
           received OTA is stored in the cache */
        (void) time_get( hdrovhd.curr_cache_entry_ptr->ts_explicit );
        (void) time_get( hdrovhd.curr_cache_entry_ptr->ts_implicit );
      }
  
      if (!chan_was_changed)
      {
        /*-----------------------------------------------------------------------
        Update AP status
        -----------------------------------------------------------------------*/
        if ( curr_cache_entry_ptr->ap_entry.is_valid &&
             ( curr_cache_entry_ptr->ap_entry.ap_msg.access_sig ==
               hdrovhd.qc_info.access_signature) )
        {
          hdrovhd_ap_info_updated( &ind_to_give );
        }
      }

      /*-------------------------------------------------------------------------
      Give necessary indications
      -------------------------------------------------------------------------*/
      hdrovhd_give_ind( &ind_to_give );
    }
    else
    {
      ERR("Could not copy rxcd SP msg to cache", 0, 0, 0 );
    }
  }
  else if ((err == E_NOT_SUPPORTED) || (err == E_NOT_ALLOWED))
  {
    if (using_duplicate_node)
    {
       hdrovhd_clear_cache_entry( available_node_ptr );
    }

    /*-------------------------------------------------------------------------
    If the hash channel in SP msg is of invalid band class, call SD and setup
    appropriate indications
    -------------------------------------------------------------------------*/
    sd_action = sd_ss_ind_hdr_opr_bad_sys( SD_SS_BAD_SYS_HASH_INV_BAND, NULL );

    if (err == E_NOT_SUPPORTED)
    {
       HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  
                       "Unsupported Band Class SD Action %d", sd_action);
    }
    else /*(err == E_NOT_ALLOWED) */
    {
       HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  
                       "Invalid Hash channel, SD Action %d", sd_action);
    }
    ASSERT (sd_action != SD_SS_ACT_CONTINUE);
    ind_to_give.need_to_give_ind = TRUE;

    if (sd_action == SD_SS_ACT_ACQ_HDR)
    {
       ind_to_give.ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
    }
    else
    {
       ind_to_give.ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
    }
  
    /*-------------------------------------------------------------------------
    Give necessary indications
      -------------------------------------------------------------------------*/
    hdrovhd_give_ind( &ind_to_give );
  }
  else 
  {
    if (using_duplicate_node)
    {
       hdrovhd_clear_cache_entry( available_node_ptr );
    }

    ERR("Could not copy rxcd SP msg to cache", 0, 0, 0 );
  }

#ifdef FEATURE_HDR_BCMCS_MFRQ
  if ( hdrovhd.ignore_bc_hash_override )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignore bc hash override (0)");
    hdrovhd.ignore_bc_hash_override = FALSE;
  }
#endif /* FEATURE_HDR_BCMCS_MFRQ */

#ifdef FEATURE_HDR_REVC
  cache_ptr = hdrovhd_find_nlb_cache(&sp_link);

  time_get(cur_time);

  if (cache_ptr != NULL &&
      !hdrovhd_nlb_cache_has_expired(cache_ptr, cur_time) &&
      hdrovhd_sector_id_24_128_is_equal( cache_ptr->sector24,
                                         sp_sector_id_128 ) &&
      cache_ptr->sector_sig == sp_sector_signature)
  {
    if (cache_ptr->state == HDROVHD_NLB_CACHE_QC_RCVD)
    {
      hdrovhd_process_sp_neighbors(&sp_link, 
                                   cache_ptr, &hdrovhd.sp_buffer);
    }

    qw_equ(cache_ptr->timestamp, cur_time);
  }
#endif /* FEATURE_HDR_REVC */

} /* hdrovhd_process_sp_msg() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_CACHE_ENTRY_IN_CURRENT_NSET

DESCRIPTION
  This function finds whether a given sector is in current sector's neighbor
  set or not. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the sector is in neighbor set of the current sector, FALSE otherwise.

SIDE EFFECTS
  None.

=============================================================================*/
static __inline boolean hdrovhd_cache_entry_in_current_nset
( 
  hdrovhd_sp_msg_type *curr_sp_msg_ptr,
  sys_link_type link 
)
{
  int i;                                          
    /* Loop variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < curr_sp_msg_ptr->num_neighbors; i++)
  {
    if ( (curr_sp_msg_ptr->nset[i].link.pilot_pn == link.pilot_pn)  &&
         (SYS_CHAN_CMP_EQUAL(curr_sp_msg_ptr->nset[i].link.channel, 
                             link.channel)) )
    {
      return TRUE;
    }
  }

  return FALSE;
} /* hdrovhd_cache_entry_in_current_nset() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_FIND_AVAILABLE_CACHE_NODE

DESCRIPTION
  The function searches through the SectorParameters cache to see if an
  unused ( or invalid ) entry exists. If such an entry exists, it returns
  the index of the first occurence of such an entry. If no such entry
  exists, it returns the index of the oldest entry that is not in the 
  neighbor list of current sector's SP message or if all the entries are 
  in neighbor list of current sector's SP message then it returns the 
  oldest entry.

DEPENDENCIES
  None

PARAMETERS
  sp_ptr - Pointer to current sector's unpacked SP message.

RETURN VALUE
  If an invalid entry exists in the cache, its index is returned,
  else the index of the oldest entry is returned.

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL hdrovhd_cache_entry_type *hdrovhd_find_available_cache_node
( 
  hdrovhd_sp_msg_type *sp_ptr
)
{
  int cnt;                /* Index counter */
  int non_nset_index = 0;          /* Index in array to oldest or unused entry */
  int nset_index = 0;     /* Index in array to oldest entry that is in curr 
                             sector's nset */
  boolean found_non_nset_cache_entry = FALSE;
  boolean first_non_nset_cache_entry = TRUE;
  boolean first_nset_cache_entry = TRUE;
  hdrovhd_cache_entry_type *available_node_ptr = NULL;
                          /* Pointer to oldest or unused cache entry */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(cnt=0; cnt < HDROVHD_MAX_CACHE_RECORDS; cnt++)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, "CACHE: cache_entry %d, state %d", 
                   cnt, hdrovhd.cache[cnt].cache_entry_state);

    /* Skip the current cache entry */
    if ((&(hdrovhd.cache[cnt])) == hdrovhd.curr_cache_entry_ptr)
    {
      continue;
    }

    /*-------------------------------------------------------------------------
    Return the index of the first entry that has INVALID state
    -------------------------------------------------------------------------*/
    if((hdrovhd.cache[cnt].cache_entry_state == HDROVHD_ENTRY_INVALID))
    {
      non_nset_index = cnt;
      found_non_nset_cache_entry = TRUE;
      break;
    }

    if (hdrovhd_cache_entry_in_current_nset(sp_ptr, hdrovhd.cache[cnt].rx_link))
    {
      if (first_nset_cache_entry)
      {
        nset_index = cnt;
        first_nset_cache_entry = FALSE;
      }

    /*-------------------------------------------------------------------------
      Find oldest entry among the sectors in the nset of the current sector. 
      nset_index is updated to point to the older (ie with smaller timestamp 
      value) of 2 entries being compared.
    -------------------------------------------------------------------------*/
      if( qw_cmp( hdrovhd.cache[nset_index].ts_explicit ,
                hdrovhd.cache[cnt].ts_explicit ) == (short) 1 )
      {
        nset_index = cnt;
      }
    }
    else 
    {
      found_non_nset_cache_entry = TRUE;

      if (first_non_nset_cache_entry)
      {
        non_nset_index = cnt;
        first_non_nset_cache_entry = FALSE;
      }

      /*-------------------------------------------------------------------------
      Find oldest entry among the sectors that are not in the nset of the current 
      sector. index is updated to point to the older ( ie, with smaller timestamp
      value ) of 2 entries being compared.
      -------------------------------------------------------------------------*/
      if( qw_cmp( hdrovhd.cache[non_nset_index].ts_explicit ,
                  hdrovhd.cache[cnt].ts_explicit ) == (short) 1 )
      {
        non_nset_index = cnt;
      }
    }
  }

  if (found_non_nset_cache_entry)
  {
    available_node_ptr = &hdrovhd.cache[non_nset_index];
  }
  else
  {
    available_node_ptr = &hdrovhd.cache[nset_index];
  }

#ifdef FEATURE_HDR_BCMCS
  dsm_free_packet( &available_node_ptr->bc_entry.bc_item_ptr );
#endif /* FEATURE_HDR_BCMCS */
  hdrovhd_clear_cache_entry(available_node_ptr);

  return available_node_ptr;
} /* hdrovhd_find_available_cache_node() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDROVHD_UNPACK_SP_MSG_EXT_INFO

DESCRIPTION
  This function extracts any Extended Channel Information fields from the
  Sector Parameters message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr            - The DSM item containing the SP message.
  field_offset_ptr    - The offset into the SP message to begin extraction.
  sp_ptr              - A pointer to the stored SP message values.

RETURN VALUE
  An E_SUCCESS to indicate successful parsing of the Extended Channel fields
  or E_DATA_INVALID if an error was encountered during the parse.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrovhd_unpack_sp_msg_ext_info
(
  dsm_item_type       *const item_ptr,           /* IN */
  uint32              *const field_offset_ptr,   /* IN */
  hdrovhd_sp_msg_type *const sp_ptr              /* IN/OUT */
)
{
  int err; /* Will hold any error codes from the bit extraction routines */
  int chan_cnt = sp_ptr->channel_count; /* Get current channel count */
  uint16 offset = *field_offset_ptr; /* Set the current offset */
  
  uint8 ext_chan_inc = 0; /* Stores the ExtendedChannelIncluded field */
  uint8 ext_chan_cnt = 0; /* Stores the ExtendedChannelCount field */
  uint8 sys_type; /* Stores the System Type of the ExtendedChannel field */

  uint8 hash_mask_inc = 0; /* Stores the hash mask included field */
  uint8 hash_mask_len = 0; /* Stores the length of the hash mask field */
  uint16 cur_hash_mask = 0x0000; /* Stores the current hash mask field */
  uint8 hash_mask_same_as_prev = 0; /* Stores the same as prev field */
  uint16 prev_hash_mask = 0; /* Used with the hash mask same as prev field */

  /* Non-zero implies that RouteUpdateTriggerCode fields are included */
  uint8 rup_trigger_code_inc = 0;

  /* Discarded temp storage for the PriorSessionGAUP field */
  uint8 prior_session_gaup;

  /* Non-zero implies that FPDCHSupported fields are included */
  uint8 fpdch_supported_inc = 0;

  /* Discarded temp storage for the FPDCHSupported field */
  uint8 fpdch_supported = 0;

  uint8 secondary_colorcode_inc = 0;

  /*----------------
     Rel B Fields
  ----------------*/        
  /* Non-zero implies that some PilotGroupID fields are included*/
  uint8 pilot_group_id_loop_inc = 0;

  /* Non-zero implies that the PilotGroupID info is included for a given neighbor */
  uint8 pilot_group_id_inc = 0;

  /* Discarded temp storage for the IsSectorMulticarrierCapable field */
  uint8 is_sector_mc_capable;

  /* Discarded temp storage for the reverse band class */
  uint8 rev_band;                         

  int i; /* Loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize extended SP message fields to default values before trying to
     unpack so that if these fields are not included AT will use default values
     otherwise AT will be using random values for this fields. 
   */
  sp_ptr->extended_channel_count = 0;
  sp_ptr->sec_color_code_count = HDROVHD_SCC_UNSPECIFIED;
  sp_ptr->route_update_trigger_code = HDROVHD_RUTC_UNSPECIFIED;
  sp_ptr->ps_gaup_is_allowed = FALSE;
  sp_ptr->is_sector_mc_capable = FALSE;

  do
  {
    /*--------------------------------------------------------------------
                     E X T E N D E D    C H A N N E L
      --------------------------------------------------------------------*/
    /* Extract the 1 bit ExtendedChannelIncluded field */
    err = HDRBIT_UNPACK8( item_ptr, &ext_chan_inc, 
      offset, HDROVHD_EXTENDED_CHANNEL_INC_SIZE );
    offset += HDROVHD_EXTENDED_CHANNEL_INC_SIZE;

    /* An error here implies no further fields left in the message */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    /* If ExtendedChannelIncluded is non-zero */
    if( ext_chan_inc )
    {
      /* Extract the 5 bit ExtendedChannelCount field */
      err = HDRBIT_UNPACK8( item_ptr, &ext_chan_cnt,
        offset, HDROVHD_EXTENDED_CHANNEL_COUNT_SIZE );
      offset += HDROVHD_EXTENDED_CHANNEL_COUNT_SIZE;

      /* Loop over all the listed ExtendedChannels */
      for( i = 0; i < ext_chan_cnt; i++ )
      {
        err |= hdrutil_unpack_channel_record( 
          item_ptr, &offset, &sys_type, &sp_ptr->channel_list.chan[chan_cnt] );

        /* If the System Type does not match up */
        if( sys_type != SYS_HDR_CHAN_REC_MODE_IS856 )
        {
          /* TODO: Revisit with the other fwd compatibility issues caused by
                   the new sys_type */
          if ( sys_type == SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH )
          {
            HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
              "(Ext Ch:%d, band:%d) is unpaired - not supported", 
              sp_ptr->channel_list.chan[i].chan_num,
              sp_ptr->channel_list.chan[i].band);
            ++(sp_ptr->unpaired_channel_count);
          }
          else
          {
            /* Flag an error and exit */
            err |= E_DATA_INVALID;
            break;
          }
        }

        /* Assign the same channel to rev_chan.  
           - If the channel is unpaired, the RCH info will be overwritten/unpacked
             at the end of the message parsing. 
           - If the channel is FDD-paired, then FCH and RCH are the same 
        */
        sp_ptr->channel_list.mode[chan_cnt] = (sys_hdr_chan_rec_mode_e_type)sys_type;
        sp_ptr->channel_list.rev_chan[chan_cnt] = sp_ptr->channel_list.chan[chan_cnt];

        /* Increment the existing channel count; this effectively
           combines the standard and extended channel lists */
        chan_cnt++;

      } /* Loop over ExtendedChannelCount */

      /* Store the count of Extended Channels */
      sp_ptr->extended_channel_count = ext_chan_cnt;

      /* An error here implies a problem extracting fields that should exist */
      if( err )
      {
        *field_offset_ptr = offset;
        err =  E_DATA_INVALID;
        break;
      }
    }

    /*--------------------------------------------------------------------
            A C C E S S   H A S H I N G   C H A N N E L   M A S K
      -------------------------------------------------------------------*/
    /* Extract the 1 bit AccessHashingChannelMaskIncluded field */
    err = HDRBIT_UNPACK8( item_ptr, &hash_mask_inc,
      offset, HDROVHD_ACCESS_HASHING_CHANNEL_MASK_INC_SIZE );
    offset += HDROVHD_ACCESS_HASHING_CHANNEL_MASK_INC_SIZE;

    /* An error here implies no further fields */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    /* If the AccessHashingChannelMask fields are included */
    if( hash_mask_inc )
    {
      /* Unpack the 4 bit AccessHashingChannelMask length */
      err = HDRBIT_UNPACK8( item_ptr, &hash_mask_len,
        offset, HDROVHD_ACCESS_HASHING_MASK_LEN_SIZE );
      offset += HDROVHD_ACCESS_HASHING_MASK_LEN_SIZE;

      /* Loop over the combined channel list */
      for( i = 0; i < chan_cnt; i ++ )
      {
        /* Extract the 1 bit same as prev field */
        err |= HDRBIT_UNPACK8( item_ptr, &hash_mask_same_as_prev,
          offset, HDROVHD_ACCESS_HASHING_CHANNEL_MASK_SAME_AS_PREV_SIZE );
        offset += HDROVHD_ACCESS_HASHING_CHANNEL_MASK_SAME_AS_PREV_SIZE;

        if( hash_mask_same_as_prev )
        {
          sp_ptr->channel_hash_mask_list[i] = prev_hash_mask;
        }
        else /* if the hash mask is not the same */
        {
          /* Extract the hash mask of the specified length + 1 
             according to 3GPP2.C.S0024-A v2.0 Section 8-156 */
          err |= HDRBIT_UNPACK16( item_ptr, &cur_hash_mask,
            offset, hash_mask_len + 1 );
          offset += hash_mask_len + 1;

          /* Store the number of matched bits in the hash match list */
          sp_ptr->channel_hash_mask_list[i] = cur_hash_mask;

          /* Remember this hash match for use with the same as prev field */
          prev_hash_mask = sp_ptr->channel_hash_mask_list[i];
        }
      } /* end for( i < chan_cnt ) */

      /* An error here implies a problem extracting fields that should exist */
      if( err )
      {
        *field_offset_ptr = offset;
        err =  E_DATA_INVALID;
        break;
      }
    }
    else
    {
      /* reset to 0 if not included */
      memset(sp_ptr->channel_hash_mask_list, 0, 
             chan_cnt * sizeof(sp_ptr->channel_hash_mask_list[0]));
    }

    /*--------------------------------------------------------------------
               R O U T E   U P D A T E   T R I G G E R   C O D E
      -------------------------------------------------------------------*/
    /* Extract the 1 bit RUP Trigger Code Included field */
    err = HDRBIT_UNPACK8( item_ptr, &rup_trigger_code_inc,
      offset, HDROVHD_RUP_TRIGGER_CODE_INC_SIZE );
    offset += HDROVHD_RUP_TRIGGER_CODE_INC_SIZE;

    /* An error here implies no further fields */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    /* If the RUP Trigger Code is included */
    if( rup_trigger_code_inc )
    {
      /* Extract the 12 bit RUP Trigger Code field */
      err = HDRBIT_UNPACK16( item_ptr, &sp_ptr->route_update_trigger_code,
        offset, HDROVHD_RUP_TRIGGER_CODE_SIZE );
      offset += HDROVHD_RUP_TRIGGER_CODE_SIZE;

      /* Extract the 4 bit RUP Trigger Max Age field */
      err |= HDRBIT_UNPACK8( item_ptr, &sp_ptr->route_update_trigger_max_age,
        offset, HDROVHD_RUP_TRIGGER_MAX_AGE_SIZE );
      offset += HDROVHD_RUP_TRIGGER_MAX_AGE_SIZE;

      /* An error here implies a problem extracting fields that should exist */
      if( err )
      {
        *field_offset_ptr = offset;
        err = E_DATA_INVALID;
        break;
      }
    }
    else
    {
      sp_ptr->route_update_trigger_code = HDROVHD_RUTC_UNSPECIFIED;
    }

    /*--------------------------------------------------------------------
                  P R I O R   S E S S I O N   G A U P 
      -------------------------------------------------------------------*/
    /* Extract the 1 bit Prior Session GAUP field */
    err = HDRBIT_UNPACK8( item_ptr, &prior_session_gaup,
      offset, HDROVHD_PRIOR_SESSION_GAUP_SIZE );
    offset += HDROVHD_PRIOR_SESSION_GAUP_SIZE;

    /* An error here implies no further fields */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    /* ps_gaup_is_allowed is used to determine if AN support ps gaup */
    sp_ptr->ps_gaup_is_allowed = (boolean) prior_session_gaup;

    /*--------------------------------------------------------------------
                        FPDCH   S U P P O R T E D 
      -------------------------------------------------------------------*/
    /* Extract the 1 bit FPDCH Supported Included field */
    err = HDRBIT_UNPACK8( item_ptr, &fpdch_supported_inc,
      offset, HDROVHD_FPDCH_SUPPORTED_INC_SIZE );
    offset += HDROVHD_FPDCH_SUPPORTED_INC_SIZE;

    /* An error here implies no further fields */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    /* If the FPDCHSupportedIncluded field is non-zero */
    if( fpdch_supported_inc )
    {
      /* Loop through the previously extracted neighbors */
      for( i = 0; i < sp_ptr->num_neighbors; i++ )
      {
        /* There will be one FPDCHSupported entry for each neighbor with a
           SystemType of SYS_HDR_SYS_TYPE_IS2000 (0x01); */
        if( sp_ptr->nset[i].link.mode == SYS_HDR_CHAN_REC_MODE_IS2000 )
        {
          err |= HDRBIT_UNPACK8( item_ptr, &fpdch_supported,
            offset, HDROVHD_FPDCH_SUPPORTED_SIZE );
          offset += HDROVHD_FPDCH_SUPPORTED_SIZE;
        }
      }

      if( err )
      {
        err = E_DATA_INVALID;
        break;
      }
    }

    /*--------------------------------------------------------------------
                    S E C O N D A R Y   C O L O R   C O D E
      -------------------------------------------------------------------*/
    /* Extract the 1 bit SecondaryColorCode Included field */
    err = HDRBIT_UNPACK8( item_ptr, &secondary_colorcode_inc,
      offset, HDROVHD_SECONDARY_COLORCODE_INC_SIZE );
    offset += HDROVHD_SECONDARY_COLORCODE_INC_SIZE;

    /* An error here implies no further fields */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    if ( secondary_colorcode_inc )
    {
      /* The value of this field should be less than or equal to 104 if 
         SecondaryColorCodeIncluded field is included and is set to ‘1’. 
       */
      if ( sp_ptr->subnet_mask > 104 )
      {
        err = E_DATA_INVALID;
        break;
      }

      /* Extract the 3 bit SecondaryColorCodeCount field */
      err = HDRBIT_UNPACK8( item_ptr, &sp_ptr->sec_color_code_count,
        offset, HDROVHD_SECONDARY_COLORCODE_COUNT_SIZE );
      offset += HDROVHD_SECONDARY_COLORCODE_COUNT_SIZE;

      for ( i = 0; i < sp_ptr->sec_color_code_count; i++ )
      {
        if ( err )
        {
          err = E_DATA_INVALID;
          break;
        }

        /* Extract the 8 bit SecondaryColorCode field */
        err = HDRBIT_UNPACK8( item_ptr, &sp_ptr->sec_color_code_list[i],
          offset, HDROVHD_SECONDARY_COLORCODE_SIZE );
        offset += HDROVHD_SECONDARY_COLORCODE_SIZE;
      }

      if( err )
      {
        err = E_DATA_INVALID;
        break;
      }
    }
    else
    {
      sp_ptr->sec_color_code_count = HDROVHD_SCC_UNSPECIFIED;
    }

    /*--------------------------------------------------------------------
                    P I L O T    G R O U P    I D
      -------------------------------------------------------------------*/
    /* Extract the 1 bit PilotGroupIDLoop Included field */
    err = HDRBIT_UNPACK8( item_ptr, &pilot_group_id_loop_inc,
      offset, HDROVHD_PILOT_GROUP_ID_LOOP_INC_SIZE );
    offset += HDROVHD_PILOT_GROUP_ID_LOOP_INC_SIZE;

    /* An error here implies no further fields */
    if( err )
        {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
        }

    if ( pilot_group_id_loop_inc )
        {
      /* - The AN shall set the first occurence of this field to '1' if the 
         PilotGroupID field corresponding to the pilot transmitting this 
         message is included in this message.
         - The AN shall set the n+1 th occurrence of this field to '1' if 
         PilotGroupID field corresponding to the n th occurence of 
         NeighborPilotPN field, with corresponding SystemType equal to 0x00 
         or 0x02, is included in this message.
         - Otherwise, the AN shall set this field to '0'

         i.e. there is one PilotGroupIDIncluded entry for the serving sector and 
         one additional PilotGroupIDIncluded entry for each neighbor with DO as
         SystemType */


      /*-------------------------------------------------------------
         Extract the first 1 bit PilotGroupID Included field 
         (i.e. the one for the serving sector) 
      -------------------------------------------------------------*/
      err = HDRBIT_UNPACK8( item_ptr, &pilot_group_id_inc,
        offset, HDROVHD_PILOT_GROUP_ID_INC_SIZE );
      offset += HDROVHD_PILOT_GROUP_ID_INC_SIZE;

      if( err )
      {
        err = E_DATA_INVALID;
        break;
        }

      /* If the pilot group ID is explicitly specified, extract the PilotGroup.  */
      if ( pilot_group_id_inc )
      {
        err = HDRBIT_UNPACK8( item_ptr, &sp_ptr->pilot_gid,
          offset, HDROVHD_PILOT_GROUP_ID_SIZE );
        offset += HDROVHD_PILOT_GROUP_ID_SIZE;

        if( err )
        {
          err = E_DATA_INVALID;
          break;
        }
      }
      else
      {
        /* The implicit specification of the PilotGroupID value is equivalent to
         * explicit specification iwth a value of zero*/
        sp_ptr->pilot_gid = HDRSRCH_PILOT_GID0;
      }

      /*-------------------------------------------------------------
         Loop through the previously extracted neighbors and extract the
         PilotGroupID for the DO neighbors 
      -------------------------------------------------------------*/
      for( i = 0; i < sp_ptr->num_neighbors; i++ )
      {
        err |= HDRBIT_UNPACK8( item_ptr, &pilot_group_id_inc,
          offset, HDROVHD_PILOT_GROUP_ID_INC_SIZE );
        offset += HDROVHD_PILOT_GROUP_ID_INC_SIZE;

        if( err )
        {
          err = E_DATA_INVALID;
          break;
        }

        /* If the pilot group ID is included, extract the PilotGroup. */
        if ( pilot_group_id_inc )
        {
          err = HDRBIT_UNPACK8( item_ptr, &sp_ptr->nset[i].pilot_gid,
            offset, HDROVHD_PILOT_GROUP_ID_SIZE );
          offset += HDROVHD_PILOT_GROUP_ID_SIZE;

          if( err )
          {
            err = E_DATA_INVALID;
            break;
          }
        }
        else
        {
          /* The implicit specification of the PilotGroupID value is equivalent to
           * explicit specification iwth a value of zero*/
          sp_ptr->nset[i].pilot_gid = HDRSRCH_PILOT_GID0;
        }
      }
    } /* (end-if) PilotGroupIDLoopIncluded*/
    else
    {
      /* The implicit specification of the PilotGroupID value is equivalent to
       * explicit specification iwth a value of zero*/
      sp_ptr->pilot_gid = HDRSRCH_PILOT_GID0;
     
      for( i = 0; i < sp_ptr->num_neighbors; i++ )
      {
        sp_ptr->nset[i].pilot_gid = HDRSRCH_PILOT_GID0;
      }
    }

    /*-------------------------------------------------------------------
       I S    S E C T O R    M U L T I C A R R I E R    C A P A B L E
    -------------------------------------------------------------------*/
    /* Extract the 1 bit IsSectorMulticarrierCapable field */
    err = HDRBIT_UNPACK8( item_ptr, &is_sector_mc_capable,
      offset, HDROVHD_IS_SECTOR_MC_CAPABLE_SIZE );
    offset += HDROVHD_IS_SECTOR_MC_CAPABLE_SIZE;

    /* An error here implies no further fields */
    if( err )
    {
      *field_offset_ptr = offset;
      err = E_SUCCESS;
      break;
    }

    /* To determine if this sector is multicarrier capable */
    sp_ptr->is_sector_mc_capable = (boolean) is_sector_mc_capable;

    /*-------------------------------------------------------------------
    R E V E R S E   B A N D   C L A S S   &   C H A N N E L   N U M B E R
    -------------------------------------------------------------------*/

    /* If we don't have any non FDD-paired channels, we're done. */
    if (sp_ptr->unpaired_channel_count == 0)
    {
      break;
    }

    /* Loop over the full channel list*/
    for ( i=0; i < chan_cnt; i++)
    {
      /* Find all the channels that have FDD-unpaired type and unpack the
         RCH info for each one of those. */
      if (sp_ptr->channel_list.mode[i] == SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH)
      {
        err |= (int) HDROVHD_SP_BAND_CLASS( item_ptr, &rev_band, offset );
        offset += HDROVHD_SP_BAND_CLASS_SIZE;

        if( err )
        {
          err = E_DATA_INVALID;
          break;
        }

        /* Keep going anyway since we don't support non FDD-paired channels */
        if( (sys_band_class_e_type) rev_band >= SYS_BAND_CLASS_MAX )
        {
          ERR("SP Msg. Invalid RCH band:%d for FCH [band:%d, chan:%d]", 
              rev_band, sp_ptr->channel_list.chan[i].band, 
              sp_ptr->channel_list.chan[i].chan_num);
        }

        sp_ptr->channel_list.rev_chan[i].band = (sys_band_class_e_type) rev_band;

        err |= (int) HDROVHD_SP_CHANNEL_NUMBER( item_ptr,
                 &sp_ptr->channel_list.rev_chan[i].chan_num,
                 offset );
        offset += HDROVHD_SP_CHANNEL_NUMBER_SIZE;

        if( err )
        {
          err = E_DATA_INVALID;
          break;
        }

      }
    }

  } while ( 0 );

  *field_offset_ptr = offset;
  return (hdrerrno_enum_type) err;
} /* hdrovhd_unpack_sp_msg_ext_info() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDROVHD_COMPUTE_HASHABLE_CHANNELS

DESCRIPTION
  This function computes and returns a list of hashable channels.

DEPENDENCIES
  The hashable_channel_list being passed by reference must be allocated
  HDR_SP_MAX_CHANNELS in size to accomodate the scenario where all possible 
  channels are included and hashable.

PARAMETERS
  sp_ptr                    - Pointer to the stored SP message.
  hashable_channel_list_ptr - Pointer to a list that will be updated with
                              the computed hashable channels.

RETURN VALUE
  The number of hashable channels.

SIDE EFFECTS
  None.

===========================================================================*/
static int hdrovhd_compute_hashable_channels
(
  hdrovhd_sp_msg_type *const sp_ptr,
  sys_channel_type    *const hashable_channel_list
)
{
  int channel_count = sp_ptr->channel_count;
  int hashable_channel_count = 0;
  int best_hash_match = 0;
  int hash_match = 0;
  int hash_match_list[HDR_MAX_SP_CHANNELS];

  uint16 const hash_mask = hdridlec_get_access_hashing_channel_mask();
  uint16 const idle_subtype = 
    hdrscp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL );

  int i; /* Loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Default Idle State Protocol is used, after running hash algorithm,
   * it is possible that AT selects a channel which it does not support. 
   * When this happens, AT may not be able to receive EVDO service even 
   * supported channels are available in that area. To prevent this happening, 
   * if AT does not have a session yet, filter channels first so that AT won't
   * be hashed to unsupported channels. 
   * The side effact is that AN may page the device on other channels since AN 
   * runs the hash algorithm without filtering. This is ok because ususally AN 
   * does not page AT if there is no session. */ 
  if( idle_subtype == HDRSCP_DEFAULT_IDLE )
  {
    ASSERT(channel_count <= HDR_MAX_SP_CHANNELS);

    /* The hashable list is just the raw channel list */
    for( i = 0; i < channel_count; i++ )
    {
      if( hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) )
      {
        hashable_channel_list[hashable_channel_count] = sp_ptr->channel_list.chan[i];
        hashable_channel_count++;
      }
      else 
      {
        /* Session is not negotiated, AT is in default IDLE. Take only the
         * supported channels */
        if ( hdrutil_is_band_supported( sp_ptr->channel_list.chan[i].band ) &&
             hdrrup_is_band_chan_supported( &sp_ptr->channel_list.chan[i] ) )
        {
          hashable_channel_list[hashable_channel_count] = sp_ptr->channel_list.chan[i];
          hashable_channel_count++;
        }
      }
    }
    return hashable_channel_count;
  }

  memset( hash_match_list, 0, sizeof( int ) * HDR_MAX_SP_CHANNELS );

  channel_count += sp_ptr->extended_channel_count;

  ASSERT(channel_count <= HDR_MAX_SP_CHANNELS);

  /* Loop through the entire channel list */
  for( i = 0; i < channel_count; i++ )
  {
    /* TODO: remove unsupported channels based on InterFlexDuplexTag 
             and IntraFlexDuplexFlag */
    if ( hdrutil_is_band_supported( sp_ptr->channel_list.chan[i].band ) &&
         hdrrup_is_band_chan_supported( &sp_ptr->channel_list.chan[i] ) )
    {
      /* Calculate the hash match */
      hash_match = 
        hdrutil_bit_count16( sp_ptr->channel_hash_mask_list[i] & hash_mask );

      /* If the new hash match is better than the existing "best" match,
        replace the "best" with the hash match */
      if( hash_match > best_hash_match )
      {
        best_hash_match = hash_match;
      }

      /* Store the hash match in hash match list */
      hash_match_list[i] = hash_match;
    }
    else /* if the band is unsupported or we have a non-FDD paired channel */
    {
      /* Mark channel as bad */
      hash_match_list[i] = -1; /* HDROVHD_UNSUPPORTED_BAND_HASH_MATCH_VALUE */
    }
  }

  if( hash_mask == HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_DEFAULT )
  {
    best_hash_match = 0;
  }

  /* Loop through the hash match list */
  for( i = 0; i < channel_count; i++ )
  {
    /* If the hash match at this index is a "best" match */
    if( hash_match_list[i] >= best_hash_match )
    {
      /* Store this channel entry in the hashable channel list */
      hashable_channel_list[hashable_channel_count] = 
       sp_ptr->channel_list.chan[i];

      /* Increment the number of hashable channels */
      hashable_channel_count++;
    }
  }

  return hashable_channel_count;

} /* hdrovhd_compute_hashable_channels() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_UNPACK_SP_MSG

DESCRIPTION
  This function extracts & copies received Sector Parameters message to cache.

DEPENDENCIES

PARAMETERS
  item_ptr - A pointer to the DSM item chain holding the message.
  node_ptr - Pointer to the node to which to copy the received
                SectorParameters message to.

RETURN VALUE
  E_SUCCESS  - If extraction, unpacking of the message succeeds & field values 
             are valid.
  E_NOT_AVAILABLE - If the entry we want to extract the msg into is in use.
  E_NO_DATA  - If there was an error extracting the message.
  E_NOT_SUPPORTED - If band class of hash_channel is invalid.

SIDE EFFECTS
  If the extraction fails, then the cache node into which fields were being
  extracted is marked as INVALID since it does not contain proper values.

=============================================================================*/
LOCAL hdrerrno_enum_type hdrovhd_unpack_sp_msg
(
  dsm_item_type *item_ptr ,
  hdrovhd_sp_msg_type *sp_ptr
)
{
  int    cnt;                          /* Index Counter */
  int    err = 0;                      /* Result of unpacking 0 = E_SUCCESS */
  uint32 field_offset;                 /* Offset of a field in a SP msg */
  
  uint8  neighbor_channel_incl;        /* Is a channel specified */
  uint8  num_channels;                 /* Num channels in SP msg */
  hdrsrch_link_id_type sp_link_id;     /* LinkID for rcvd SP msg */
  sys_link_type sp_link;               /* Link for rcvd SP msg */
  uint8 system_type;                   /* Unpacked SystemType from SP msg */
  uint8 band;                          /* Unpacked Band from SP msg */
  uint8 neighbor_srch_win_size_incl;   /* Is search window size included */
  uint8 neighbor_srch_win_offset_incl; /* Is search window offset included */
  hdrerrno_enum_type unpack_result;    /* Return value of unpack sp msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Unpack the SectorParameters message fields & copy to node
  E_SUCCESS is assumed to have a value of 0.
  The return values from macros are cast to int & OR-ed with the previous error
  If any of the macros return an error, the final value of err will be non-zero
  err is an int type & not hdrerrno_enum_type, because OR-ing two
  hdrerrno_enum_type values can result in an invalid hdrerrno_enum_type value.
 ----------------------------------------------------------------------------*/
  err |= (int) HDROVHD_SP_COUNTRY_CODE( item_ptr, &sp_ptr->country_code );
  err |= (int) HDROVHD_SP_SECTOR_ID( item_ptr, sp_ptr->sector_id );
  err |= (int) HDROVHD_SP_SUBNET_MASK( item_ptr, &sp_ptr->subnet_mask );
  err |= (int) HDROVHD_SP_SECTOR_SIGNATURE( item_ptr,
                                            &sp_ptr->sector_sig );
  err |= (int) HDROVHD_SP_LATITUDE( item_ptr, &sp_ptr->latitude );
  sp_ptr->latitude = HDROVHD_EXTEND22( (uint32)sp_ptr->latitude );

  err |= (int) HDROVHD_SP_LONGITUDE( item_ptr, &sp_ptr->longitude  );
  sp_ptr->longitude = HDROVHD_EXTEND23( (uint32)sp_ptr->longitude );

  err |= (int) HDROVHD_SP_ROUTE_UPDATE_RADIUS( item_ptr, &sp_ptr->rup_radius );
  err |= (int) HDROVHD_SP_LEAP_SECONDS( item_ptr, &sp_ptr->leap_secs );
  err |= (int) HDROVHD_SP_LOCAL_TIME_OFFSET( item_ptr,
                                       &sp_ptr->local_time_offset);
  sp_ptr->local_time_offset = HDROVHD_EXTEND11( sp_ptr->local_time_offset );

  err |= (int) HDROVHD_SP_REV_LINK_SILENCE_DURATION( item_ptr,
                                       &sp_ptr->silence_duration);

  err |= (int) HDROVHD_SP_REV_LINK_SILENCE_PERIOD( item_ptr,
                                       &sp_ptr->silence_period);

  /*---------------------------------------------------------------------------
  SP message contains channel_count channel records. extract this field
  ---------------------------------------------------------------------------*/
  err |= (int) HDROVHD_SP_CHANNEL_COUNT( item_ptr, &num_channels ) ;

  sp_ptr->channel_count = num_channels;
  sp_ptr->extended_channel_count = 0;
  sp_ptr->unpaired_channel_count = 0;

  /*---------------------------------------------------------------------------
     Initialize the serving sector's pilot gid to "NOT_INCLUDED_PILOT_GID" for now. 
     It will be overwritten if the pilot group info is provided in the message.
  ---------------------------------------------------------------------------*/
  sp_ptr->pilot_gid = HDRSRCH_NOT_INCLUDED_PILOT_GID;

  field_offset = HDROVHD_SP_CHANNELLIST_OFFSET;

  if( num_channels == 0 )
  {
    /*-------------------------------------------------------------------------
    If num of channels specified in SectorParameters message is 0. then we
    cannot hash to any channel specified in the SP message.  Thus, store the
    current channel itself.
    -------------------------------------------------------------------------*/
    sp_ptr->hash_channel= hdrovhd.qc_info.link.channel;
  }
  else
  {
    /* For the purpose of doing a sanity check that the neighbor list does
     * not contain o-freq neighbors from vertical channels, we need to 
     * keep the entire list around.
     */
    for( cnt=0; cnt < num_channels; cnt++ )
    {
      err |= (int) HDROVHD_SP_SYSTEM_TYPE( item_ptr, &system_type,
                                           field_offset );
      field_offset += HDROVHD_SP_SYSTEM_TYPE_SIZE;

      /* Revisit forward compatibility issues */
      if ( ( system_type != SYS_HDR_CHAN_REC_MODE_IS856 ) &&
           ( system_type != SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH ) )
      {
        ERR("SP Msg. Invalid system_type:%d", system_type, 0, 0);
        err |= E_NO_DATA;
         break;
      }

      err |= (int) HDROVHD_SP_BAND_CLASS( item_ptr, &band, field_offset );
      field_offset += HDROVHD_SP_BAND_CLASS_SIZE;

      if( (sys_band_class_e_type) band >= SYS_BAND_CLASS_MAX )
      {
        ERR("SP Msg. Invalid band:%d", band, 0, 0);
        err |= E_NO_DATA;
      }

      sp_ptr->channel_list.chan[cnt].band = (sys_band_class_e_type) band;

      err |= (int) HDROVHD_SP_CHANNEL_NUMBER( item_ptr,
               &sp_ptr->channel_list.chan[cnt].chan_num,
               field_offset );
      field_offset += HDROVHD_SP_CHANNEL_NUMBER_SIZE;

      if ( system_type == SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH )
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"(Ch:%d, band:%d) is unpaired - not supported", 
                       sp_ptr->channel_list.chan[cnt].chan_num,
                       sp_ptr->channel_list.chan[cnt].band);
        ++(sp_ptr->unpaired_channel_count);
      }                    

      /* Assign the same channel to rev_chan.  
         - If the channel is unpaired, the RCH info will be overwritten/unpacked
           at the end of the message parsing. 
         - If the channel is FDD-paired, then FCH and RCH are the same 
      */
      sp_ptr->channel_list.mode[cnt] = (sys_hdr_chan_rec_mode_e_type)system_type;
      sp_ptr->channel_list.rev_chan[cnt] = sp_ptr->channel_list.chan[cnt];

    } /* end for( cnt < num_channels ) */

  } /* end if( num_channels != 0 ) */

  err |= (int) HDROVHD_SP_NEIGHBOR_COUNT( item_ptr,
                                       &sp_ptr->num_neighbors ,
                                       field_offset );

  field_offset = field_offset + HDROVHD_SP_NEIGHBOR_COUNT_SIZE;

  /* Extract the neighbor list */
  for( cnt=0; cnt < sp_ptr->num_neighbors ; cnt++ )
  {
    err |= (int) HDROVHD_SP_NEIGHBOR_PN( item_ptr ,
                         &sp_ptr->nset[cnt].link.pilot_pn,
                         field_offset );

    field_offset = field_offset + HDROVHD_SP_NEIGHBOR_PN_SIZE;
  }

  for( cnt=0; cnt < sp_ptr->num_neighbors ; cnt++ )
  {
    err |= (int) HDROVHD_SP_NEIGHBOR_CHAN_INCL(item_ptr,
                                            &neighbor_channel_incl,
                                            field_offset);
    field_offset = field_offset + HDROVHD_SP_NEIGHBOR_CHAN_INCL_SIZE;

    if( neighbor_channel_incl == (uint8)1 )
    {
      err |= (int) HDROVHD_SP_SYSTEM_TYPE( item_ptr ,
                                           &system_type ,
                                           field_offset );
      field_offset += HDROVHD_SP_SYSTEM_TYPE_SIZE;

      /* _MPA_ featurized in srcher code */
      sp_ptr->nset[cnt].link.mode = (sys_hdr_chan_rec_mode_e_type)system_type;
      if( sp_ptr->nset[cnt].link.mode >= SYS_HDR_CHAN_REC_MODE_MAX )
      {
        err |= E_NO_DATA;
        ERR("Invalid system_type %d rcvd in SP", system_type, 0, 0);
      }

      /* Extract the channel from the rcvd message */
      err |= (int) HDROVHD_SP_BAND_CLASS( item_ptr ,
             &band,
             field_offset );
      if( (sys_band_class_e_type) band >= SYS_BAND_CLASS_MAX )
      {
        ERR("SP Msg. Invalid band:%d", band, 0, 0);
        err |= E_NO_DATA;
      }

      /* Since there is a direct mapping between the number & enum value no
      conversion function is needed */
      sp_ptr->nset[cnt].link.channel.band  =
                               (sys_band_class_e_type) band;
      field_offset += HDROVHD_SP_BAND_CLASS_SIZE;

      err |= (int) HDROVHD_SP_CHANNEL_NUMBER( item_ptr ,
             &sp_ptr->nset[cnt].link.channel.chan_num ,
             field_offset );
      field_offset += HDROVHD_SP_CHANNEL_NUMBER_SIZE;
    }
    else
    {
      /* Update the channel to the current channel */
      sp_link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );
      hdrlmac_get_link_info( sp_link_id, &sp_link );
      sp_ptr->nset[cnt].link.channel.band = sp_link.channel.band;
      sp_ptr->nset[cnt].link.channel.chan_num = sp_link.channel.chan_num;

      /* We need to set mode because currently hdrlmac_get_link_info 
       * doesn't return correct mode */
      sp_ptr->nset[cnt].link.mode = SYS_HDR_CHAN_REC_MODE_IS856;
    }

    /* Initialize the pilot gid - it will be overwritten if provided when 
       extracting the info from the extended SP info */
    sp_ptr->nset[cnt].pilot_gid = HDRSRCH_NOT_INCLUDED_PILOT_GID;

  }

  err |= (int) HDROVHD_SP_NEIGHBOR_SRCH_WIN_SIZE_INCL(item_ptr,
                                  &neighbor_srch_win_size_incl,
                                  field_offset);
  field_offset += HDROVHD_SP_NEIGHBOR_SRCH_WIN_SIZE_INCL_SIZE;

  for( cnt=0; cnt < sp_ptr->num_neighbors ; cnt++ )
  {
    if( neighbor_srch_win_size_incl == (uint8)1 )
    {
      err |= (int) HDROVHD_SP_NEIGHBOR_SRCH_WINSIZE(item_ptr,
                            (uint8 *) &sp_ptr->nset[cnt].window_size,
                            field_offset);
      field_offset += HDROVHD_SP_NEIGHBOR_SRCH_WINSIZE_SIZE;
    }
    else
    {
      sp_ptr->nset[cnt].window_size = HDRSRCH_WINSIZE_DEFAULT;
    }
  }

  err |= (int) HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET_INCL(item_ptr,
                                    &neighbor_srch_win_offset_incl,
                                    field_offset);
  field_offset += HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET_INCL_SIZE;

  for( cnt=0; cnt < sp_ptr->num_neighbors ; cnt++ )
  {
    if( neighbor_srch_win_offset_incl == (uint8)1 )
    {
      err |= (int) HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET(item_ptr,
                     (uint8 *) &sp_ptr->nset[cnt].window_offset,
                     field_offset);
      field_offset += HDROVHD_SP_NEIGHBOR_SRCH_WIN_OFFSET_SIZE;
    }
    else
    {
      sp_ptr->nset[cnt].window_offset = HDRSRCH_WINOFFSET_DEFAULT;
    }
  }

  if( err == 0 )
  {
    /*-------------------------------------------------------------------------
    Extract the Extended Information segment of the Sector Parameters Message
    -------------------------------------------------------------------------*/
    err = hdrovhd_unpack_sp_msg_ext_info( item_ptr, &field_offset, sp_ptr );
    
  }

  if( err == 0 )
  {
    /*-------------------------------------------------------------------------
    Run the channel hashing algorithm if all fields have extracted successfully
    -------------------------------------------------------------------------*/
    err = hdrovhd_compute_sp_hash_channel( sp_ptr, &hdrovhd.qc_info.link, FALSE ); 
  }

  /*---------------------------------------------------------------------------
  Check if all field extractions succeeded
  ---------------------------------------------------------------------------*/
  if( err != 0 )
  {
    /*-------------------------------------------------------------------------
    If Extracting any fields has failed, then mark the cache node as Invalid
    & return FALSE.
    -------------------------------------------------------------------------*/
    unpack_result = E_NO_DATA;
  }
  else if ( !hdrutil_is_band_supported(sp_ptr->hash_channel.band) )
  {
    /*-------------------------------------------------------------------------
    If hash_channel band class is invalid then mark the cache node as Invalid
    -------------------------------------------------------------------------*/
    unpack_result = E_NOT_SUPPORTED;
  }
  else if (!hdrrup_is_band_chan_supported( &sp_ptr->hash_channel))
  {
    /*-------------------------------------------------------------------------
    If hash_channel is invalid then mark the cache node as Invalid
    -------------------------------------------------------------------------*/
      unpack_result = E_NOT_ALLOWED;
  }
  else
  {
    unpack_result = E_SUCCESS;
  }

  return unpack_result;
} /* hdrovhd_unpack_sp_msg */

/* <EJECT> */

/*===========================================================================

FUNCTION HDROVHD_SP_MSG_IS_READABLE

DESCRIPTION
  This helper function determines if the cached Sector Parameters message is 
  valid and available for read access.

DEPENDENCIES
  None.

PARAMETERS
  curr_cache_entry_ptr - A pointer to the current cached Sector Parameters
    entry.
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrovhd_sp_msg_is_readable
(
  hdrovhd_cache_entry_type *curr_cache_entry_ptr,
  hdrhai_protocol_name_enum_type caller
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_OVHD_GET_SP_MSG( caller, hdrovhds_get_state() );

  /*---------------------------------------------------------------------------
  Check if Control channel is current. If not log an error
  ---------------------------------------------------------------------------*/
  if( hdrovhd.info_state == HDROVHD_INFO_NOT_UPDATED )
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "SectorParameters not updated");
    return E_NOT_ALLOWED;
  }

  /*---------------------------------------------------------------------------
  Check for Invalid value. This should never occur, since current SP
  pointer is never updated unless valid value are assigned
  ---------------------------------------------------------------------------*/
  ASSERT( curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID );

  /*---------------------------------------------------------------------------
  Check if the current SectorParameters node is in use, that is, either
  being read from or written to. If so, then cause an error.
  ---------------------------------------------------------------------------*/
  if( curr_cache_entry_ptr->sp_entry.state == HDROVHD_BUF_WRITE_IN_PROG )
  {
    ERR("Cannot read curr SP. Cache node in use!", 0, 0, 0);
    return E_NOT_AVAILABLE;
  }

  /*---------------------------------------------------------------------------
  Check if the current SectorParameters state is valid only when in connected.
  If that is the case, then check whether AT is connected. If not, return
  error, else store the current stae & retore it after reading.
  ---------------------------------------------------------------------------*/
  if((curr_cache_entry_ptr->cache_entry_state == HDROVHD_ENTRY_VALID_WHEN_CONN)
     && ( !hdrrup_is_tc_assigned( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) ) )
  {
    ERR("SP not valid in Idle.", 0, 0, 0);
    return E_NOT_AVAILABLE;
  }

  return E_SUCCESS;
} /* hdrovhd_sp_msg_is_readable() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDROVHD_GET_SP_MSG_PILOT_GID_AND_NSET_INFO

DESCRIPTION
  This function returns the values of nset, num_neighbors, channel_list
  and num_channels of the most recently received Sector Parameters message.

DEPENDENCIES
  The input parameter nset_info_ptr should point to allocated memory.

PARAMETERS
  pilot_gid_ptr - returns the pilot group ID for the current sector
  nset_info_ptr - A pointer to the struct that will hold the returned values.
  caller - The name of the calling protocol (used in software trace).
  
RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrovhd_get_sp_msg_pilot_gid_and_nset_info
(
  uint8 *pilot_gid_ptr,
  hdrovhd_sp_msg_nset_info_type *nset_info_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrerrno_enum_type err_number; /* Traps and returns any error codes */
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience pointer to SP message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*---------------------------------------------------------------------------
  Check the status of the SP message.
  ---------------------------------------------------------------------------*/
  if( ( err_number = hdrovhd_sp_msg_is_readable( curr_cache_entry_ptr, caller ) ) !=
        E_SUCCESS )
  {
    return err_number;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_READ_IN_PROG;

  /*---------------------------------------------------------------------------
  Set the SP message convenience pointer.
  ---------------------------------------------------------------------------*/
  sp_msg_ptr = &curr_cache_entry_ptr->sp_entry.sp_msg;

  /*---------------------------------------------------------------------------
  Copy the Pilot Group ID info for the serving sector
  ---------------------------------------------------------------------------*/
  *pilot_gid_ptr = sp_msg_ptr->pilot_gid;

  /*---------------------------------------------------------------------------
  Copy neighborset information.
  ---------------------------------------------------------------------------*/
  nset_info_ptr->channel_count = sp_msg_ptr->channel_count;
  nset_info_ptr->num_neighbors = sp_msg_ptr->num_neighbors;

  /* A memcpy is needed for the following two fields due to allocation as
     arrays of structs */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( (void *) nset_info_ptr->nset, 
           sizeof(nset_info_ptr->nset), 
           (void *) sp_msg_ptr->nset, 
           sizeof( sp_msg_ptr->nset ));
#else
  memcpy( (void *) nset_info_ptr->nset, 
          (void *) sp_msg_ptr->nset, 
          sizeof( hdrsrch_nset_pilot_type ) * HDRHAI_MAX_SP_NEIGHBORS );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* The channel list is only used for SMPF so searcher doesn't care whether
     or not it knows about which channels are FDD-paired or not. */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( (void *) nset_info_ptr->channel_list,
            sizeof(nset_info_ptr->channel_list), 
           (void *) sp_msg_ptr->channel_list.chan,
            sizeof( sp_msg_ptr->channel_list.chan ));
#else
  memcpy( (void *) nset_info_ptr->channel_list, 
          (void *) sp_msg_ptr->channel_list.chan,
          sizeof( sys_channel_type ) * HDR_MAX_SP_CHANNELS );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /*---------------------------------------------------------------------------
  Restore state of buffer.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;

  return E_SUCCESS; 
} /* hdrovhd_get_sp_msg_pilot_gid_and_nset_info() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDROVHD_GET_SP_MSG_LOCATION

DESCRIPTION
  This function returns the latitude and longitude of the most recently
  received Sector Parameters message.

DEPENDENCIES
  The input parameter location_ptr should point to allocated memory.

PARAMETERS
  location_ptr - A pointer to the struct that will hold the returned values.
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrovhd_get_sp_msg_location
(
  hdrovhd_sp_msg_location_type *location_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience pointer to SP message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*---------------------------------------------------------------------------
  Check the status of the SP message.
  ---------------------------------------------------------------------------*/
  if( !( curr_cache_entry_ptr && 
         curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID ) )
  {
    return E_NOT_AVAILABLE;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_READ_IN_PROG;

  /*---------------------------------------------------------------------------
  Set the SP message convenience pointer.
  ---------------------------------------------------------------------------*/
  sp_msg_ptr = &curr_cache_entry_ptr->sp_entry.sp_msg;

  /*---------------------------------------------------------------------------
  Copy latitude and longitude.
  ---------------------------------------------------------------------------*/
  location_ptr->latitude = sp_msg_ptr->latitude;
  location_ptr->longitude = sp_msg_ptr->longitude;
  location_ptr->rup_radius = sp_msg_ptr->rup_radius;

  /*---------------------------------------------------------------------------
  Restore state of buffer.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;

  return E_SUCCESS;
} /* hdrovhd_get_sp_msg_location() */

/*===========================================================================

FUNCTION HDROVHD_GET_SP_MSG_COUNTRY_CODE

DESCRIPTION
  This function returns the country code of the most recently
  received Sector Parameters message.

DEPENDENCIES
  The input parameter location_ptr should point to allocated memory.

PARAMETERS
  country_code_ptr - A pointer to the memory that will hold the returned values.
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrovhd_get_sp_msg_country_code
(
  uint16 *country_code_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience pointer to SP message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*---------------------------------------------------------------------------
  Check the status of the SP message.
  ---------------------------------------------------------------------------*/
  if( !( curr_cache_entry_ptr && 
         curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID ) )
  {
    return E_NOT_AVAILABLE;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_READ_IN_PROG;

  /*---------------------------------------------------------------------------
  Set the SP message convenience pointer.
  ---------------------------------------------------------------------------*/
  sp_msg_ptr = &curr_cache_entry_ptr->sp_entry.sp_msg;

  *country_code_ptr = sp_msg_ptr->country_code;

  /*---------------------------------------------------------------------------
  Restore state of buffer.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;

  return E_SUCCESS;
} /* hdrovhd_get_sp_msg_country_code() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_GET_AP_MSG

DESCRIPTION
  This function is used to access the current AccessParameters message.
  If the AccessParameters are not current & the AccessParameters supervision
  timer is not already running, it starts the timer.

DEPENDENCIES
  Access Parameters should be current when calling this function.
  The input parameter ap_msg_ptr should point to valid memory, ie.,
  memory for storing the AccessParameters message should be allocated before
  calling this function.

PARAMETERS
  ap_msg_ptr - Pointer to allocated memory where the current AccessParameters
  message will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NOT_ALLOWED - If AccessParameters are not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_ap_msg
(
  hdrovhd_ap_msg_type *ap_msg_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_ap_entry_type *curr_ap_ptr;  /* Pointer to Current AP msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  HDRTRACE_OVHD_GET_AP_MSG( caller, hdrovhds_get_state() );

  /*---------------------------------------------------------------------------
  Make a local copy of the pointer to the current AP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_ap_ptr = &hdrovhd.curr_cache_entry_ptr->ap_entry;

  /*---------------------------------------------------------------------------
  Check if AccessParameters are current. If not log an error
  ---------------------------------------------------------------------------*/
  if( hdrovhd.ap_state != HDROVHD_AP_UPDATED )
  {
    ERR("AccessParameters are not current!", 0, 0, 0);
#ifdef FEATURE_HDRMC_DEBUG
    #error code not present
#endif
    return E_NOT_ALLOWED;
  }

  /*---------------------------------------------------------------------------
  Check for Invalid value. This should never occur, since current AP
  pointer is never updated unless valid value are assigned
  ---------------------------------------------------------------------------*/
  ASSERT( curr_ap_ptr->is_valid );

  /*---------------------------------------------------------------------------
  Check if the current AccessParameters node is in use, that is, either
  being read from or written to. If so, then cause an error. this is to
  avoid race conditions
  ---------------------------------------------------------------------------*/
  if( curr_ap_ptr->state == HDROVHD_BUF_WRITE_IN_PROG )
  {
    ERR("Cannot read current AP msg. Buffer is in use!", 0, 0, 0);
#ifdef FEATURE_HDRMC_DEBUG
    #error code not present
#endif
    return E_NOT_AVAILABLE;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents to the buffer provided by
  the calling routine.
  ---------------------------------------------------------------------------*/
  curr_ap_ptr->state = HDROVHD_BUF_READ_IN_PROG;

  /*---------------------------------------------------------------------------
  Copy contents of node to the buffer provided by the calling routine.
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_MEMCPY_REMOVAL
  *ap_msg_ptr = curr_ap_ptr->ap_msg;
#else
  memcpy(ap_msg_ptr, &curr_ap_ptr->ap_msg , sizeof(hdrovhd_ap_msg_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /*---------------------------------------------------------------------------
  Mark that node is not use
  ---------------------------------------------------------------------------*/
  curr_ap_ptr->state = HDROVHD_BUF_NOT_IN_USE;

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return E_SUCCESS;

} /* hdrovhd_get_ap_msg() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_GET_QC_LINK

DESCRIPTION
  This function is used to retrieve the link info of the current qc message.

DEPENDENCIES
  None

PARAMETERS
  link_id_ptr - Pointer to allocated memory where the link info
  will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the no qc message has been received yet

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_qc_link
(
  sys_link_type *link_info_ptr,
  hdrhai_protocol_name_enum_type caller
)
{

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Check if at least 1 QC msg (thus color code) has been rcvd since activation
  ---------------------------------------------------------------------------*/
  if( !hdrovhd.qc_info.initialized )
  {
    ERR("No Link Info. No QC msg rcvd since activation", 0, 0, 0);
#ifdef FEATURE_HDRMC_DEBUG
    #error code not present
#endif
    return E_NO_DATA;
  }

  *link_info_ptr = hdrovhd.qc_info.link ;
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return E_SUCCESS;
} /* hdrovhd_get_qc_link() */


/*=============================================================================
FUNCTION HDROVHD_GET_COLOR_CODE

DESCRIPTION
  This function is used to retrieve the color code of the current sector.

DEPENDENCIES
  None

PARAMETERS
  color_code_ptr - Pointer to allocated memory where the color code
                   will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has not been received yet

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_color_code
(
  uint8 *color_code_ptr,
  hdrhai_protocol_name_enum_type caller
)
{

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  HDRTRACE_OVHD_GET_COLOR_CODE( caller, hdrovhds_get_state() );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
       hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
         == HDRSCP_IRAT_SAP )
  {
    /* If tunnel mode is enabled, color code is received from sib8 or
     * pre-reg info. Don't have to check whether QC is received or not. */
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /*---------------------------------------------------------------------------
    Check if at least 1 QC msg (thus color code) has been rcvd since activation
    ---------------------------------------------------------------------------*/
    if( !hdrovhd.qc_info.initialized )
    {
      ERR("No Color Code. No QC msg rcvd since activation", 0, 0, 0);
#ifdef FEATURE_HDRMC_DEBUG
      #error code not present
#endif
      return E_NO_DATA;
    }
  }

  *color_code_ptr = hdrovhd.qc_info.color_code;

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return E_SUCCESS;

} /* hdrovhd_get_color_code() */

/* <EJECT> */

/*============================================================================

FUNCTION HDROVHD_GET_SUBNET

DESCRIPTION
  This function is used to retrieve the Subnet of the current sector.

DEPENDENCIES
  None

PARAMETERS
  subnet_ptr - Pointer to allocated memory where the subnet will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_subnet
(
  hdraddr_type subnet,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrerrno_enum_type result = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
       hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
         == HDRSCP_IRAT_SAP )
  {
    /* Get subnet ID from UATI when HDR is in tunnel mode.
     * SubnetIncluded is always presented in the a fresh UATI assignment in
     * tunnel mode. */
    result = hdramp_get_uati_subnet( subnet );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {  
    /*---------------------------------------------------------------------------
    Check if a valid subnet has been stored
    ---------------------------------------------------------------------------*/
    if( !hdrovhd.curr_subnet.is_valid )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Current subnet unknown" );
      result = E_NO_DATA;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( subnet, HDRADDR_LENGTH_IN_BYTES, hdrovhd.curr_subnet.subnet_id, 
                       HDRADDR_LENGTH_IN_BYTES );
#else
      memcpy( subnet, hdrovhd.curr_subnet.subnet_id, HDRADDR_LENGTH_IN_BYTES );
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return result;

} /* hdrovhd_get_subnet() */

/*============================================================================

FUNCTION HDROVHD_GET_SUBNET_MASK

DESCRIPTION
  This function is used to retrieve the Subnet mask of the current sector.

DEPENDENCIES
  None

PARAMETERS
  subnet_mask - Pointer to allocated memory where the subnet mask will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_subnet_mask
(
  uint8 *subnet_mask,
  hdrhai_protocol_name_enum_type caller
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  //HDRTRACE_OVHD_GET_SUBNET( caller, hdrovhds_get_state() );

  /*---------------------------------------------------------------------------
  Check if a valid subnet has been stored
  ---------------------------------------------------------------------------*/
  if( !hdrovhd.curr_subnet.is_valid )
  {
    ERR("Current subnet unknown", 0, 0, 0);
#ifdef FEATURE_HDRMC_DEBUG
    #error code not present
#endif
    return E_NO_DATA;
  }

  *subnet_mask = hdrovhd.curr_subnet.subnet_mask;

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return E_SUCCESS;

} /* hdrovhd_get_subnet_mask() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_ID

DESCRIPTION
  This function is used to retrieve the 128 bit SectorID of the current sector

DEPENDENCIES
  None

PARAMETERS
  sector_id - Pointer to allocated memory where the sector id will get copied.
  subnet_mask_ptr - Pointer to where the subnet_mask will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - if overhead parameters are not current, so the current
               128 bit sector id is unknown

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_sector_id
(
  hdraddr_type sector_id,
  uint8 * subnet_mask_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_lte_to_hdr_get_cgi_is_in_progress())
  {
    if (hdrovhd.cgi_info_is_valid)
    {
      hdraddr_set( sector_id, hdrovhd.sp_buffer.sector_id );
	  if(subnet_mask_ptr != NULL)
	  {
        *subnet_mask_ptr = hdrovhd.sp_buffer.subnet_mask;
	  }
      hdrovhd.cgi_info_is_valid = FALSE;
      return E_SUCCESS;
    }
    else
    { 
      return E_NO_DATA;
    }
  }
#endif /* FEATURE_LTE_TO_HDR */

  //HDRTRACE_OVHD_GET_SECTOR_ID( caller, hdrovhds_get_state() );

  /*--------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*--------------------------------------------------------------------------
  Check if the current sector id is known
  ---------------------------------------------------------------------------*/
  if (( hdrovhd.info_state != HDROVHD_INFO_UPDATED ) ||
      (curr_cache_entry_ptr == NULL))
  {
    ERR("Current SectorID unknown", 0, 0, 0);
#ifdef FEATURE_HDRMC_DEBUG
    #error code not present
#endif
    return E_NO_DATA;
  }

  hdraddr_set( sector_id, curr_cache_entry_ptr->sp_entry.sp_msg.sector_id );
  if(subnet_mask_ptr != NULL)
  {
     *subnet_mask_ptr = curr_cache_entry_ptr->sp_entry.sp_msg.subnet_mask;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return E_SUCCESS;

} /* hdrovhd_get_sector_id() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_ID2

DESCRIPTION
  This function is used to retrieve the 128 bit SectorID and subnet mask
  from the overhead information, even if it is not marked as being current.
  If E_SUCCESS is returned, the SectorID given is guaranteed to be from  the
  same subnet as the link (pilot PN and channel) passed in, but is not
  necessarily from the same exact sector.

DEPENDENCIES
  None.

PARAMETERS
  sector_id - Pointer to allocated memory where the sector id will get copied.
  subnet_mask_ptr - Pointer to where the subnet_mask will get copied.
  link_ptr - Pointer to information on the current link
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - if the given link info is not current, so the current
               128 bit sector id is unknown

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_sector_id2
(
  hdraddr_type sector_id,
  uint8 * subnet_mask_ptr,
  sys_link_type *link_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */
  boolean sector_id_is_valid = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

  /*--------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  if (curr_cache_entry_ptr == NULL)
  {
    return E_NO_DATA;
  }

  if( hdrovhd.info_state == HDROVHD_INFO_UPDATED )
  {
    sector_id_is_valid = TRUE;
  }
  else
  {
    if( (hdrovhd.qc_info.initialized) &&
        ( curr_cache_entry_ptr->cache_entry_state == HDROVHD_ENTRY_VALID ) &&
        (SYS_CHAN_CMP_EQUAL( link_ptr->channel, hdrovhd.qc_info.link.channel )) &&
        (link_ptr->pilot_pn == hdrovhd.qc_info.link.pilot_pn ) )
    {
      /*--------------------------------------------------------------------------
      The check here for hdrovhd.curr_subnet.is_valid ensures that the SP
      message in the cache is from the same subnet as the QC message
      ---------------------------------------------------------------------------*/
      if ( hdrovhd.curr_subnet.is_valid )
      {
        sector_id_is_valid = TRUE;
      }
    }
  }

  if (sector_id_is_valid)
  {
    hdraddr_set( sector_id, curr_cache_entry_ptr->sp_entry.sp_msg.sector_id );
    *subnet_mask_ptr = curr_cache_entry_ptr->sp_entry.sp_msg.subnet_mask;
    return E_SUCCESS;
  }

  return E_NO_DATA;

} /* hdrovhd_get_sector_id2() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_ID_24

DESCRIPTION
  This function is used to retrieve the lowest 24 bits of the sector ID in
  the last received quick config message.

DEPENDENCIES
  None

PARAMETERS
  sector_id_24_ptr - Pointer to allocated memory where the lowest 24 bits of
  the current sector's sector ID will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the sector ID has not been received yet

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_sector_id_24
(
  uint32 *sector_id_24_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  HDRTRACE_OVHD_GET_SECTOR_ID_24( caller, hdrovhds_get_state() );

  /*---------------------------------------------------------------------------
  Check if at least 1 QC msg (thus sectorid24) has been rcvd since activation
  ---------------------------------------------------------------------------*/
  if( !hdrovhd.qc_info.initialized )
  {
    ERR("No SectorID24. No QC msg rcvd since activation", 0, 0, 0);
    return E_NO_DATA;
  }

  *sector_id_24_ptr= hdrovhd.qc_info.sector_id_24 ;

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return E_SUCCESS;
} /* hdrovhd_get_sector_id_24() */


/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_GET_CACHED_SECTORS

DESCRIPTION
  This function returns a list of what sectors are in the cache.

DEPENDENCIES
  None

PARAMETERS
  sector_list_ptr - Where to store the sector list
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  Number of sectors currently in the cache

SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_get_cached_sectors
(
  sys_link_type *sector_list_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  uint8 num_sectors = 0; /* number of sectors in the cache */
  int i; /* index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < HDROVHD_MAX_CACHE_RECORDS; i++)
  {
    /*-------------------------------------------------------------------------
    Return a list of all pilots stored in the cache
    -------------------------------------------------------------------------*/
    if (hdrovhd.cache[i].cache_entry_state == HDROVHD_ENTRY_VALID)
    {
      sector_list_ptr[num_sectors] = hdrovhd.cache[i].rx_link;
      num_sectors++;
    }
  }

  return num_sectors;

} /* hdrovhd_get_cached_sectors() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_INFO_IS_UPDATED

DESCRIPTION
   This function returns TRUE if the overhead info has been updated, otherwise
   it returns FALSE.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - If overhead info has been updated
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_info_is_updated( hdrhai_protocol_name_enum_type caller )
{
  boolean info_is_updated; /* is the overhead info updated */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  info_is_updated = (hdrovhd.info_state == HDROVHD_INFO_UPDATED) ?
                    TRUE : FALSE ;

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
       !hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
  {
    /* If we have received SIB8 or PreREg info, mark ovhd is updated */
    if ( hdrovhd.tunnel_info.sib8_is_updated )
    {
      info_is_updated = TRUE;
    }
  }
#endif /*FEATURE_LTE_TO_HDR_OH*/

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  HDRTRACE_OVHD_INFO_IS_UPDATED( caller, hdrovhds_get_state(), 
                                 info_is_updated );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  return info_is_updated;

} /* hdrovhd_info_is_updated() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_AP_IS_UPDATED

DESCRIPTION
  This function returns TRUE if the AccessParameters have been updated,
  otherwise it returns FALSE. If the AccessParameters have not been updated
  & the  AccessParameters supervision timer is not already running, it starts a
  the timer.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - If AccessParameters have been updated
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_ap_is_updated( hdrhai_protocol_name_enum_type caller )
{
  boolean ap_is_updated;  /* Are AccessParams updated */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  if( hdrovhd.ap_state == HDROVHD_AP_UPDATED )
  {
    ap_is_updated = TRUE;
  }
  else
  {
    /*-------------------------------------------------------------------------
    If AP timer is not already running, start the timer
    -------------------------------------------------------------------------*/
    if( rex_get_timer( &hdrovhd.ap_supervision_timer ) == 0 )
    {
      (void) rex_set_timer( &hdrovhd.ap_supervision_timer,
                            HDROVHD_AP_TIMER_MSECS );
    }
    ap_is_updated = FALSE;
  }
  HDRTRACE_OVHD_AP_IS_UPDATED( caller, hdrovhds_get_state(), ap_is_updated );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  return ap_is_updated;

} /* hdrovhd_ap_is_updated() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDROVHD_IS_WAITING_TO_UPDATE_AP

DESCRIPTION
  This function returns whether AT needs to force 426 ms sleep to update AP.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - AP is not updated and AP update timer has not expired.
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_is_waiting_to_update_ap( hdrhai_protocol_name_enum_type caller )
{
  boolean waiting_to_update_ap;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( hdrovhd.ap_state != HDROVHD_AP_UPDATED ) &&
       ( rex_get_timer( &hdrovhd.ap_update_wait_timer ) != 0 ) )
  {
    waiting_to_update_ap = TRUE;
  }
  else
  {
    waiting_to_update_ap = FALSE;
  }

  return waiting_to_update_ap;

} /* hdrovhd_is_waiting_to_update_ap() */

/* <EJECT> */


/*=============================================================================

FUNCTION HDROVHD_SECTOR_ID_24_128_ID_EQUAL

DESCRIPTION
  This function compares the least significant 24 bits of a 24 bit SectorID
  (received in the QuickConfig message) and the 128 bit SectorId ( received
  in the SectorParameters message)

DEPENDENCIES
  None

PARAMETERS
  sector_id_24  - 24 bit Sector ID
  sector_id_128 - 128 bit Sectod ID

RETURN VALUE
  TRUE  - If the 24 LSBs match
  FALSE - Otherwise

SIDE EFFECTS
  None.

=============================================================================*/
LOCAL boolean hdrovhd_sector_id_24_128_is_equal
(
  uint32 sector_id_24,
  hdraddr_type sector_id_128
)
{
  uint32 lsb24_sector_id; /* Least significant 24 bits of sector id */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  lsb24_sector_id =
    ((sector_id_128[HDRADDR_LENGTH_IN_BYTES-3] << 16) & 0x00ff0000) |
    ((sector_id_128[HDRADDR_LENGTH_IN_BYTES-2] << 8) & 0x0000ff00) |
    ( sector_id_128[HDRADDR_LENGTH_IN_BYTES-1] & 0x000000ff );

  return ( sector_id_24 == lsb24_sector_id );
} /* hdrovhd_sector_id_24_128_is_equal() */

/* <EJECT> */


/*=============================================================================

FUNCTION HDROVHD_CONCATENATE_SECTOR_ID

DESCRIPTION
  This function concatenates the least significant 24 bits of SectorID to the
  Subnet value to form the complete 128 bit SectorID.

DEPENDENCIES

PARAMETERS
  sector_id_128 - Resultant concatenated 128 bit SectorID
  sector_id_24  - Least significant 24 bits of SectorID
  subnet        - Subnet to which sector belongs

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrovhd_concatenate_sector_id
(
  hdraddr_type sector_id_128,
  uint32 sector_id_24,
  hdrovhd_subnet_type subnet
)
{
  /* Set the upper bits of the SectorID equal to the subnet value */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( sector_id_128 , HDRADDR_LENGTH_IN_BYTES, 
           subnet.subnet_id , HDRADDR_LENGTH_IN_BYTES-3 );
#else
  memcpy( sector_id_128 , subnet.subnet_id , HDRADDR_LENGTH_IN_BYTES-3 );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Set the least significant 24 bits equal to the SectorId specified */
  sector_id_128[HDRADDR_LENGTH_IN_BYTES-3] =
                    (uint8)((sector_id_24 & 0x00ff0000) >> 16 );
  sector_id_128[HDRADDR_LENGTH_IN_BYTES-2] =
                    (uint8)((sector_id_24 & 0x0000ff00) >> 8 );
  sector_id_128[HDRADDR_LENGTH_IN_BYTES-1] =
                    (uint8)(sector_id_24 & 0x000000ff);
} /* hdrovhd_concatenate_sector_id() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDROVHD_RESTART_TIMERS

DESCRIPTION
  This function resets & starts the QuickConfig & SectorParameters
  supervision timers. It resets & restarts the AccessParameters timer only
  if it were already running

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrovhd_restart_timers( void )
{
  /*---------------------------------------------------------------------------
  Restart the QuickConfig & SectorParameters Supervision timer
  ---------------------------------------------------------------------------*/
  if (hdrovhd.saved_qc_superv_time_ms != 0)
  {
    (void) rex_set_timer(&hdrovhd.qc_supervision_timer , hdrovhd.saved_qc_superv_time_ms);
    hdrovhd.saved_qc_superv_time_ms = 0;
  }
  else
  {
  (void) rex_set_timer(&hdrovhd.qc_supervision_timer , HDROVHD_QC_TIMER_MSECS);
  }

  if (hdrovhd.saved_sp_superv_time_ms != 0)
  {
    (void) rex_set_timer(&hdrovhd.sp_supervision_timer , hdrovhd.saved_sp_superv_time_ms);
    hdrovhd.saved_sp_superv_time_ms = 0;
  }
  else
  {
  (void) rex_set_timer(&hdrovhd.sp_supervision_timer , HDROVHD_SP_TIMER_MSECS);
  }

  (void) rex_set_timer( &hdrovhd.qc_insanity_timer, HDROVHD_INSANITY_TIMER_MSECS );
  (void) rex_set_timer( &hdrovhd.sp_insanity_timer, HDROVHD_INSANITY_TIMER_MSECS );
  
  /*---------------------------------------------------------------------------
  The AccessParameters timer is only started if another protocol requests
  for the AccessParameters and they are not current. So we do not reset &
  start the AccessParameters everytime we change state. We do so only if the
  AccessParameters Supervision timer is already running because of a prior
  event.
  ---------------------------------------------------------------------------*/
  if( rex_get_timer( &hdrovhd.ap_supervision_timer ) != 0 )
  {
    (void) rex_set_timer( &hdrovhd.ap_supervision_timer ,
                          HDROVHD_AP_TIMER_MSECS );
  }
} /* hdrovhd_restart_timers() */

 /* <EJECT> */

/*===========================================================================
FUNCTION HDROVHD_STOP_TIMERS

DESCRIPTION
  This function stops all OVHD timers ( QC, SP & AP ) and clears the
  associated signals.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrovhd_stop_timers( void )
{
  /*-------------------------------------------------------------------------
  Stop QuickConfig, SectorParameters & AccessParameters Supervision timers.
  -------------------------------------------------------------------------*/
  ( void ) rex_clr_timer( &hdrovhd.qc_supervision_timer );
  ( void ) rex_clr_sigs( rex_self(), HDRMC_OVHD_QC_TIMER_SIG );
  ( void ) rex_clr_timer( &hdrovhd.sp_supervision_timer );
  ( void ) rex_clr_sigs( rex_self(), HDRMC_OVHD_SP_TIMER_SIG );
  ( void ) rex_clr_timer( &hdrovhd.ap_supervision_timer );
  ( void ) rex_clr_sigs( rex_self(), HDRMC_OVHD_AP_TIMER_SIG );

  /* Stop QuickConfig, SectorParameters insanity timers */
  (void) rex_clr_timer( &hdrovhd.qc_insanity_timer );
  (void) rex_clr_timer( &hdrovhd.sp_insanity_timer );
  
} /* hdrovhd_stop_timers() */
 /* <EJECT> */

/*===========================================================================
FUNCTION HDROVHD_CLEAR_TIMERS

DESCRIPTION
  This function resets all saved supervision times ( QC, SP ) and stops all
  the supervision timers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void hdrovhd_clear_timers( void )
{
  hdrovhd.saved_qc_superv_time_ms = 0;
  hdrovhd.saved_sp_superv_time_ms = 0;
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS  
  rex_clr_timer( &hdrovhd.sys_avoid_timer );
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
  hdrovhd_stop_timers();

} /* hdrovhd_clear_timers() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_TIMER_CB

DESCRIPTION
  This function acts as the callback for when a timer expires; it 
  will queue up the proper messages for hdrmc to handle.

DEPENDENCIES
  None.
  
PARAMETERS
  timer_id - The id of the expired timer triggering the function.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_timer_cb
( 
  uint32 timer_id 
)
{
  hdrmc_queue_timer( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, timer_id );
} /* hdrovhd_timer_cb() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_PROCESS_TIMER

DESCRIPTION
  This function is an entry point for hdrmc to process the timer message
  that has been queued up.

DEPENDENCIES
  None.
  
PARAMETERS
  timer_id - The id of the expired timer triggering the function.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_process_timer
( 
  uint32 timer_id 
) 
{
  hdrovhd_ind_to_give_type ind_to_give;
  sd_ss_act_e_type sd_action;

  boolean is_connected = FALSE;

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS  
  uint8 num_avoided_channels;
  sys_avoided_channel_record_type 
      avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Initialize the ind_to_give */
  hdrovhd_reset_ind_to_give(&ind_to_give);
  
  /* Determine if we are in a connection */
  is_connected = 
      hdralmp_connection_is_open( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );

  switch( timer_id )
  {
    case HDROVHD_QC_SUPERVISION_TIMER:
      /*-----------------------------------------------------------------------
        QuickConfig Supervision timer expired:
        Set control channel to not current & give SupervisionFailed indication
        -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_OVHD_QC_TIMER_SIG( hdrovhds_get_state() );

      if( rex_get_timer( &hdrovhd.qc_supervision_timer ) == 0 )
      {
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;

        /* Increment the supervision count depending on whether we are connected
          or not. */
        if ( is_connected )
        {
          hdrplog_inc_conn_term_stats_counter(
            HDRPLOG_QC_TRAFFIC_SUPERVISION_FAILURE_COUNT, 1 );
        }
        else
        {
          hdrplog_inc_conn_term_stats_counter(
            HDRPLOG_QC_IDLE_SUPERVISION_FAILURE_COUNT, 1 );
        }

        hdrind_give_ind( HDRIND_OVHD_SUPERVISION_FAILED , NULL );
        /* Giving SUPERVISION_FAILED indication causes ALMP to deactivate OVHD,
        * So OVHD state has now changed to OVHD_INACTIVE.
        */
      }
      else
      {
        HDR_MSG_PROT (MSG_LEGACY_MED, "Stale QC supervision timer event");
      }
      break;

    case HDROVHD_SP_SUPERVISION_TIMER:
      /*-----------------------------------------------------------------------
        SectorParameters Supervision timer expired:
        Set control channel to not current & give SupervisionFailed indication
        -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_OVHD_SP_TIMER_SIG( hdrovhds_get_state() );

      if( rex_get_timer( &hdrovhd.sp_supervision_timer ) == 0 )
      {
        hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;
  
        /* Increment the supervision count depending on whether we are connected
          or not. */
        if ( is_connected )
        {
          hdrplog_inc_conn_term_stats_counter(
            HDRPLOG_SP_TRAFFIC_SUPERVISION_FAILURE_COUNT, 1 );
        }
        else
        {
          hdrplog_inc_conn_term_stats_counter(
            HDRPLOG_SP_IDLE_SUPERVISION_FAILURE_COUNT, 1 );
        }
  
        hdrind_give_ind( HDRIND_OVHD_SUPERVISION_FAILED , NULL );
        /* Giving SUPERVISION_FAILED indication causes ALMP to deactivate OVHD,
        * So OVHD state has now changed to OVHD_INACTIVE.
        */
      }
      else
      {
        HDR_MSG_PROT (MSG_LEGACY_MED, "Stale SP supervision timer event");
      }
      break;

    case HDROVHD_AP_SUPERVISION_TIMER:
      /*-----------------------------------------------------------------------
        AccessParameters Supervision timer expired:
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_OVHD_AP_TIMER_SIG( hdrovhds_get_state() );

      hdrovhd_set_ap_not_updated();

      /* Increment the supervision count depending on whether we are connected
        or not. */
      if ( is_connected )
      {
        hdrplog_inc_conn_term_stats_counter(
          HDRPLOG_AP_TRAFFIC_SUPERVISION_FAILURE_COUNT, 1 );
      }
      else
      {
        hdrplog_inc_conn_term_stats_counter(
          HDRPLOG_AP_IDLE_SUPERVISION_FAILURE_COUNT, 1 );
      }

      hdrind_give_ind( HDRIND_OVHD_SUPERVISION_FAILED , NULL );
      /* Giving SUPERVISION_FAILED indication causes ALMP to deactivate OVHD,
      * So OVHD state has now changed to OVHD_INACTIVE.
      */
      break;

    case HDROVHD_QC_INSANITY_TIMER:
    case HDROVHD_SP_INSANITY_TIMER:
      /*----------------------------------------------------------------------- 
        QC or SP Insanity Timer expired:
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Insanity timer %d expired", timer_id );

    
      /* We are receiving overhead messages but the content is invalid. */
      if( (hdrovhd.info_state == HDROVHD_INFO_NOT_UPDATED) && 
          (hdrovhds_get_state() != HDROVHDS_INACTIVE) )
      {
        sd_action = sd_ss_ind_hdr_opr_bad_sys( 
                      SD_SS_BAD_SYS_HDR_INV_AP_CAPSULE_LEN, 
                      NULL );
        HDR_MSG_PROT_1( MSG_LEGACY_MED, "SD Action %d", sd_action );
        ASSERT( sd_action != SD_SS_ACT_CONTINUE );
        
        ind_to_give.need_to_give_ind = TRUE;
        if( sd_action == SD_SS_ACT_CONTINUE ) 
        {
          ind_to_give.ind_name = HDRIND_OVHD_CHANGE_HDR_SYSTEMS;
        }
        else
        {
          ind_to_give.ind_name = HDRIND_OVHD_EXIT_HDR_MODE;
        }
      
        hdrovhd_give_ind( &ind_to_give );
      }

      else
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "Insanity timer %d expired unexpectedly", timer_id );
      }
      break;

    case HDROVHD_BC_SUPERVISION_TIMER:
#ifdef FEATURE_HDR_BCMCS
      /*----------------------------------------------------------------------- 
        Broadcast Supervision Timer expired:
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_OVHD_BC_SUP_TIMER( hdrovhds_get_state() );
      hdrbccp_disable_all_monitored_flows( SYS_BCMCS_FLOW_STATUS_TIMEOUT );
#endif /* FEATURE_HDR_BCMCS */
      break;
    
#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
    case HDROVHD_AP_UPDATE_WAIT_TIMER:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "AP update wait timer expired");
      break;

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
    case HDROVHD_SYS_AVOID_TIMER:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Reset avoided channel list");
	  num_avoided_channels = 0;
	  memset( avoided_chan_rec, 0, sizeof(avoided_chan_rec) );
      num_avoided_channels = hdrovhd_update_sys_avoid_list(
          HDRSRCH_AVOIDED_CHAN_MAX_SIZE,
          avoided_chan_rec);
	  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "num_avoided_channels returned %d",num_avoided_channels);
      /*-------------------------------------------------------------------------
      Update the searcher neighbor list.
      -------------------------------------------------------------------------*/

      hdrlmac_nset_update( 0, NULL, /* Don't update neighbor list */ 
                           0, NULL, /* Don't update channel list */ 
                           0, NULL, /* Don't update cached sectors */
                           num_avoided_channels, 
                           avoided_chan_rec, /* avoided chan */
                           HDRSRCH_NSET_AVOID_LIST_UPDATED, 
                           HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 
                           HDRSRCH_NOT_INCLUDED_PILOT_GID 
                             /* serving sector's pilot gr id */
                         );
      break;
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

#ifdef FEATURE_HDR_TO_LTE

    case HDROVHD_EARFCN_AVOID_TIMER:
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "hdrovhd : EARFCN avoid timer expired");
        memset(hdrovhd.avoided_earfcn, 0, sizeof(hdrovhd.avoided_earfcn));
		hdrovhd.num_avoided_earfcn = 0;
		break;

#endif /* FEATURE_HDR_TO_LTE */

    default:
      break;
  }
} /* hdrovhd_process_timer() */

/*===========================================================================

FUNCTION HDROVHD_COMPUTE_SP_HASH_CHANNEL

DESCRIPTION
  This function will process the SP message's channel and extended channel
  lists and remove any unsupported channels from the hashable channel list.
  After the removal of invalid channels this function will run the hashing
  function over the hashable channels and set the proper fields in the SP
  message pointed to by the sp_ptr parameter.

DEPENDENCIES
  None.

PARAMETERS
  sp_ptr - A pointer to the SectorParameters message
  current_link_ptr - Pointer to current link
  show_trace_output - If this flag is set to TRUE then the function will
    display a list of the hashable channels via F3 messages

RETURN VALUE
  E_SUCCESS if the channel is hashed; E_FAILURE in all other cases.

SIDE EFFECTS
  The SP message pointed to by the sp_ptr will have its hash_channel field
  updated to whichever channel has been chosen as the hash channel by this
  function.

===========================================================================*/
static errno_enum_type hdrovhd_compute_sp_hash_channel
(
  hdrovhd_sp_msg_type *sp_ptr,
  const sys_link_type *current_link_ptr,
  boolean show_trace_output
)
{
  errno_enum_type err = E_SUCCESS;

  uint32 session_seed;
  uint32 channel_index;
  uint16 hashable_chan_cnt;

  int cnt;

  /* Hashable channel list */
  sys_channel_type hashable_channel_list[HDR_MAX_SP_CHANNELS];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Prune the combined channel and extended channel lists and compute the
    list of hashable channels.
    -------------------------------------------------------------------------*/
  hashable_chan_cnt = 
    hdrovhd_compute_hashable_channels( sp_ptr, hashable_channel_list );

  /*-------------------------------------------------------------------------
    Run the hashing algorithm if there are hashable channels
    -------------------------------------------------------------------------*/
  if( hashable_chan_cnt > 0 )
  {
    /*-------------------------------------------------------------------------
      Get the sesssion seed and call the hash function to determine the index of
      the channel record that pertains to the Access Terminal. Hash over only the
      supported (valid) channels. Store this channel record in the hash_channel 
      structure.
      -------------------------------------------------------------------------*/
    session_seed = hdramp_get_session_seed( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL );
    channel_index = hash( session_seed, hashable_chan_cnt, 0 );
    ASSERT(channel_index < HDR_MAX_SP_CHANNELS);

    /*-------------------------------------------------------------------------
    The following F3 messages are used to expedite AST testing
    -------------------------------------------------------------------------*/
    if( show_trace_output )
    {
      if( (uint8) hashable_chan_cnt < 
          (sp_ptr->channel_count + sp_ptr->extended_channel_count) )
      {
        /* Print session seed information */
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
          "Session seed: 0x%x, num_chans: %d", session_seed, hashable_chan_cnt );
    
        /* Loop across all hashable channels and print band/channel number info */
        for( cnt = 0; cnt < hashable_chan_cnt; cnt++ )
        {
          HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "chan_list[%d]: band %d, chan_num %d",
            cnt, hashable_channel_list[cnt].band, hashable_channel_list[cnt].chan_num );
        }
      }
      else
      {
        if( sp_ptr->extended_channel_count > 0 )
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH, "Hashing over all standard & extended channels" );
        }
        else
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH, "Hashing over all standard channels only" );
        }
      }
    }

    if ((sp_ptr->hash_channel.band != 
         hashable_channel_list[channel_index].band) ||
        (sp_ptr->hash_channel.chan_num != 
         hashable_channel_list[channel_index].chan_num))
    {
      sp_ptr->hash_channel = 
          hashable_channel_list[channel_index];

      HDR_MSG_PROT(MSG_LEGACY_LOW, 
                   "cache changed: hdrovhd_compute_sp_hash_channel");
      hdrovhd.cache_changed = TRUE;
    }
  }
  else
  {
    /* if no hashable channel, assign hash_channel to current channel */
    sp_ptr->hash_channel = current_link_ptr->channel;
  }

  HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "hash to band:%d, channel:%d",
                  sp_ptr->hash_channel.band,
                  sp_ptr->hash_channel.chan_num );

  return err;

} /* hdrovhd_compute_sp_hash_channel() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_GET_SECONDARY_COLOR_CODES

DESCRIPTION
  This function returns the count and current list of SecondaryColorCodes.

DEPENDENCIES
  None.
  
PARAMETERS
 sec_cc_count_ptr - Count of SecondaryColorCode returned in the "sec_cc_list".
 sec_cc_list  - List of SecondaryColorCodes

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_secondary_color_codes
(
  uint8 *sec_cc_count_ptr,
  uint8 *sec_cc_list
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr = NULL; /* Pointer to Cache entry */
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience pointer to SP message */
  hdrerrno_enum_type err_no;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) &&
       hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
         == HDRSCP_IRAT_SAP )
  {
    sp_msg_ptr = &hdrovhd.sp_buffer;
  
    if ( sp_msg_ptr->sec_color_code_count != HDROVHD_SCC_UNSPECIFIED )
    {
      /*---------------------------------------------------------------------------
      Copy the SecondaryColorCode information.
      ---------------------------------------------------------------------------*/
      *sec_cc_count_ptr = sp_msg_ptr->sec_color_code_count;
  
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( (void *) sec_cc_list, 
               HDROVHD_MAX_SEC_COLORCODES, 
              (void *) sp_msg_ptr->sec_color_code_list,
              HDROVHD_MAX_SEC_COLORCODES );
#else
      memcpy( (void *) sec_cc_list,
              (void *) sp_msg_ptr->sec_color_code_list,
              HDROVHD_MAX_SEC_COLORCODES );
#endif /* FEATURE_MEMCPY_REMOVAL */
  
      err_no = E_SUCCESS;
    }
    else
    {
      err_no = E_NOT_AVAILABLE;
    }
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /*---------------------------------------------------------------------------
    Make a local copy of the pointer to the current SP msg node in the cache,
    in case the global pointer changes in the midst of read operation
    ---------------------------------------------------------------------------*/
    curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;
  
    /*---------------------------------------------------------------------------
    Check the status of the SP message.
    ---------------------------------------------------------------------------*/
    if( !( curr_cache_entry_ptr && 
           curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID ) )
    {
      return E_NOT_AVAILABLE;
    }
  
    /*---------------------------------------------------------------------------
    Mark node to be in use while copying contents.
    ---------------------------------------------------------------------------*/
    curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_READ_IN_PROG;
  
    /*---------------------------------------------------------------------------
    Set the SP message convenience pointer.
    ---------------------------------------------------------------------------*/
    sp_msg_ptr = &curr_cache_entry_ptr->sp_entry.sp_msg;
  
    if (sp_msg_ptr->sec_color_code_count != HDROVHD_SCC_UNSPECIFIED)
    {
      /*---------------------------------------------------------------------------
      Copy the SecondaryColorCode information.
      ---------------------------------------------------------------------------*/
      *sec_cc_count_ptr = sp_msg_ptr->sec_color_code_count;
  
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( (void *) sec_cc_list, 
                HDROVHD_MAX_SEC_COLORCODES, 
                (void *) sp_msg_ptr->sec_color_code_list,
                HDROVHD_MAX_SEC_COLORCODES );
#else
      memcpy( (void *) sec_cc_list,
              (void *) sp_msg_ptr->sec_color_code_list,
              HDROVHD_MAX_SEC_COLORCODES );
#endif /* FEATURE_MEMCPY_REMOVAL */
  
      err_no = E_SUCCESS;
    }
    else
    {
      err_no = E_NOT_AVAILABLE;
    }
  
    /*---------------------------------------------------------------------------
    Restore state of buffer.
    ---------------------------------------------------------------------------*/
    curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;
  }

  return err_no; 
} /* hdrovhd_get_secondary_color_codes() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_GET_CURR_RUTC

DESCRIPTION
  This function returns the current RouteUpdateTriggerCode.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  This function returns the current RouteUpdateTriggerCode.
  
SIDE EFFECTS
  None.
=============================================================================*/
uint16 hdrovhd_get_curr_rutc ( void )
{
  uint16  curr_rutc = hdrovhd.curr_cache_entry_ptr->
                        sp_entry.sp_msg.route_update_trigger_code;

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Current RUTC = 0x%x", curr_rutc);

  return curr_rutc;

} /* hdrovhd_get_curr_rutc */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_GET_RUTC_LIST_COUNT

DESCRIPTION
  This function returns number of RouteUpdateTriggerCode in the list.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  The number of RouteUpdateTriggerCode in the list.
  
SIDE EFFECTS
  None.
=============================================================================*/
int hdrovhd_get_rutc_list_count ( void )
{
  return q_cnt(&hdrovhd.rutc_list);
} /* hdrovhd_get_rutc_list_count */

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_GET_SP_PS_GAUP_IS_ALLOWED

DESCRIPTION
  This function returns if AN supports prior session GAUP.

DEPENDENCIES
  None.
  
PARAMETERS
 caller  - Calling protocol

RETURN VALUE
  TRUE - if priorsession GAUP flag is set in SectorParam
  FALSE - Otherwise
  
SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_get_sp_ps_gaup_is_allowed
(
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */
  hdrovhd_sp_msg_type *sp_msg_ptr; /* Convenience pointer to SP message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*---------------------------------------------------------------------------
  Check the status of the SP message.
  ---------------------------------------------------------------------------*/
  if( !( curr_cache_entry_ptr && 
         curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID ) )
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_READ_IN_PROG;

  /*---------------------------------------------------------------------------
  Set the SP message convenience pointer.
  ---------------------------------------------------------------------------*/
  sp_msg_ptr = &curr_cache_entry_ptr->sp_entry.sp_msg;

  /*---------------------------------------------------------------------------
  Restore state of buffer.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;

  /*---------------------------------------------------------------------------
  Copy Prior Session GAUP Is Allowed
  ---------------------------------------------------------------------------*/
  return sp_msg_ptr->ps_gaup_is_allowed;

} /* hdrovhd_get_sp_ps_gaup_is_allowed() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_IS_MC_CAPABLE

DESCRIPTION
  This function is used to query from the SP message whether the current
  sector is MC capable.

DEPENDENCIES
  None
  
PARAMETERS
  sector_is_mc_capable_ptr - OUT whether the sector is multicarrier capable
  caller - The protocol calling this externalized function (for sw trace)
  
RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the information is not available at this time
  
SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_sector_is_mc_capable
( 
  boolean *sector_is_mc_capable_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrovhd_cache_entry_type *curr_cache_entry_ptr; /* Pointer to Cache entry */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Make a local copy of the pointer to the current SP msg node in the cache,
  in case the global pointer changes in the midst of read operation
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr = hdrovhd.curr_cache_entry_ptr;

  /*---------------------------------------------------------------------------
  Check the status of the SP message.
  ---------------------------------------------------------------------------*/
  if( !( curr_cache_entry_ptr && 
         curr_cache_entry_ptr->cache_entry_state != HDROVHD_ENTRY_INVALID ) )
  {
    return E_NOT_AVAILABLE;
  }
  
  /*---------------------------------------------------------------------------
  Mark node to be in use while copying contents.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_READ_IN_PROG;
  
  /*---------------------------------------------------------------------------
    Copy the IsSectorMCCapable information.
  ---------------------------------------------------------------------------*/
  *sector_is_mc_capable_ptr = 
    curr_cache_entry_ptr->sp_entry.sp_msg.is_sector_mc_capable;
  
  /*---------------------------------------------------------------------------
  Restore state of buffer.
  ---------------------------------------------------------------------------*/
  curr_cache_entry_ptr->sp_entry.state = HDROVHD_BUF_NOT_IN_USE;

  return E_SUCCESS; 

} /* hdrovhd_get_sector_is_mc_capable() */

/*=============================================================================
FUNCTION HDROVHD_SET_QC_IS_RECEIVED

DESCRIPTION
   This function set if the QuickConfig message has been received or not 

DEPENDENCIES
  None.
  
PARAMETERS
  qc_is_received - if QC message has been received or not
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_set_qc_is_received
( 
  boolean qc_is_received
)
{
  hdrovhd.qc_is_received = qc_is_received;
} /* hdrovhd_set_qc_is_received() */

/*=============================================================================
FUNCTION HDROVHD_QC_IS_RECEIVED

DESCRIPTION
   This function returns TRUE if the QuickConfig message has been received after 
   the phone enters Monitor State, otherwise, it returns FALSE.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  TRUE - QuickConfig message has been received
  FALSE -   otherwise
  
SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_qc_is_received( void )
{
  return hdrovhd.qc_is_received;
} /* hdrovhd_qc_is_received() */

/*=============================================================================
FUNCTION HDROVHD_GET_PREFERRED_CHANNEL 

DESCRIPTION
   This function is used to get the list of channles on which the AT prefers 
   to be assigned a Traffic Channel  
                                     
DEPENDENCIES
  None.

PARAMETERS
  pref_chan_count_ptr - returns the number of preferred channels 
  pref_chan_list_ptr - returns the preferred bc idle channel if it exists
  max_pref_chan_count - max number of preferred channels 
                
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_get_preferred_channel
(
  uint8 *pref_chan_list_cnt,
  sys_hdr_channel_record_s_type *pref_chan_list_ptr,
  uint8 max_pref_chan_count
)
{

#ifdef FEATURE_SVDO
  uint8 chan_idx;
  uint8 num_channels = 0;  /* Total number of channels in the channel list*/
  uint8 num_pref_channels = 0;  /* Total number of preferred channels*/
  sys_channel_type chan_array[2]; /* Array of channels to be evaluated for
  simultaneous operation. */
  sys_serving_channel_record_type serving_chan; /* 1x serving chan info */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrovhd.curr_cache_entry_ptr->cache_entry_state == HDROVHD_ENTRY_INVALID )
  {
    *pref_chan_list_cnt = 0;
    HDR_MSG_PROT(MSG_LEGACY_MED,"No preferred channel ( OVHD_ENTRY_INVALID )");
    return;
  }

  num_channels = hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg.channel_count;

  if (num_channels == 0)
  {
    *pref_chan_list_cnt = 0;
    HDR_MSG_PROT(MSG_LEGACY_MED,"No preferred channel ( channel count == 0 )");
    return;
  }

  /* Get 1x serving channel */
  sd_misc_serving_chan_info (SD_SS_MAIN, &serving_chan);
  chan_array[0] = serving_chan.chan_rec.channel;

  if (serving_chan.srv_status != SYS_SRV_STATUS_SRV )
  {
    *pref_chan_list_cnt = 0;
    HDR_MSG_PROT(MSG_LEGACY_MED,"No preferred channel ( 1x is not in service )");
    return;
  }

  for (chan_idx = 0; chan_idx < num_channels; chan_idx++)
  {
    /* Store channel listed in SP to chan_array */
    chan_array[1] = 
      hdrovhd.curr_cache_entry_ptr->sp_entry.sp_msg.channel_list.chan[chan_idx];

    if (hdrlmac_rf_channel_config_is_valid(2, chan_array))
    {
      pref_chan_list_ptr[num_pref_channels].mode = SYS_HDR_CHAN_REC_MODE_IS856;          
      pref_chan_list_ptr[num_pref_channels++].channel =  chan_array[1];          
      HDR_MSG_PROT_3(MSG_LEGACY_MED,"preferred channel %d, band %d, chan_num %d",
                     num_pref_channels -1, 
                     pref_chan_list_ptr[num_pref_channels - 1].channel.band,
                     pref_chan_list_ptr[num_pref_channels - 1].channel.chan_num);
    }
  }

  if (num_pref_channels == num_channels)
  {
    /* If all channels listed in SP message are within MaxSupportedBW of the i
       serving 1x channel, exclude PreferredChannel field
     */
    num_pref_channels = 0;
    HDR_MSG_PROT(MSG_LEGACY_MED,"All channels listed in SP are preferredchannel");
  }

  if (num_pref_channels > max_pref_chan_count)
  {
    /* If number of preferred channels is greater than the 
     * max_pref_chan_count, pick the first max_pref_chan_count channels 
     * as preferred channels
     */
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 " Number of preferred channels is greate than max_pref_chan_count");
    num_pref_channels = max_pref_chan_count;
  }

  *pref_chan_list_cnt = num_pref_channels;

#else 

  *pref_chan_list_cnt = 0;
  HDR_MSG_PROT(MSG_LEGACY_MED,"No preferred channel");
  return;

#endif /* FEATURE_SVDO */

} /* hdrovhd_get_preferred_channel() */

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_DCM_REACQ_DIFF_SUBNET

DESCRIPTION
  This function informs OVHD that AT has reacq-ed on a different
  subnet during DCM mode.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_dcm_reacq_diff_subnet( void )
{
  hdrovhd.is_first_after_acquisition = TRUE;
}
#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */


/*=============================================================================
FUNCTION HDROVHD_RESET_INFO_STATE

DESCRIPTION
  This function resets OVHD INFO state to NOT_UPDATED.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_reset_info_state( void )
{
  hdrovhds_state_enum_type ovhd_state; /* Local copy of state of OVHD */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ovhd_state = hdrovhds_get_state();

  HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH, "DCM: resetting OVHD info state from %d",
                   ovhd_state );

  hdrovhd.info_state = HDROVHD_INFO_NOT_UPDATED;

} /* hdrovhd_reset_info_state() */

/*=============================================================================

FUNCTION HDROVHD_UPDATE_LTE_FPLMN

DESCRIPTION
  This function updates LTE forbidden PLMN list. 

DEPENDENCIES
  None.

PARAMETERS
  num_fplmn - Number of FPLMNs in the list.
  fplmn - pointer to the FPLMN list.
  is_online - whether HDR is online.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_lte_fplmn 
( 
  uint32 num_fplmn,
  sys_plmn_id_s_type *fplmn,
  boolean is_online
)
{
  if ( num_fplmn <= HDROVHD_MAX_FPLMN_LIST_SIZE )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "%d FPLMN updated", num_fplmn );
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Too many FPLMN (%d)", num_fplmn );
    num_fplmn = HDROVHD_MAX_FPLMN_LIST_SIZE;
  }

  hdrovhd.fplmn_list.num_fplmn = num_fplmn;
  memset( hdrovhd.fplmn_list.fplmnid, 0, sizeof(hdrovhd.fplmn_list.fplmnid) );
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( hdrovhd.fplmn_list.fplmnid, sizeof(hdrovhd.fplmn_list.fplmnid), 
           fplmn, num_fplmn*sizeof(sys_plmn_id_s_type) );
#else
  memcpy( hdrovhd.fplmn_list.fplmnid, fplmn, num_fplmn*sizeof(sys_plmn_id_s_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_HDR_TO_LTE

  if ( !is_online )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED,
                  "Receive FPLMN when HDR is offline." );
    return;
  }

  /* If ORNL is in UPDATED state, all other conditions to update nset is
   * considered to be satisfied. */
  if( hdrovhd.ornl_state == HDROVHD_ORNL_NOT_UPDATED )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Don't update nset because ornl is not updated" );
    return;
  }
#if (LTE_BAND_NUM == 256)
  hdrovhd_lte_nset_update( SYS_LTE_BAND_MASK_CONST_NONE, FALSE );
#else
  hdrovhd_lte_nset_update( 0, FALSE );
#endif /* (LTE_BAND_NUM == 256)   */
#endif /* FEATURE_HDR_TO_LTE*/

} /* hdrovhd_update_lte_fplmn() */

#ifdef FEATURE_HDR_TO_LTE
/*=============================================================================
FUNCTION HDROVHD_GET_OTHER_RAT_TX_CYCLE

DESCRIPTION
  This function rteurns the other RAT TX cycle.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  Other RAT TX Cycle value included in the QC message
  
SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_get_other_rat_tx_cycle( void )
{
  uint8 other_rat_tx_cycle = HDRSRCH_ORNL_CYCLE_DISABLED; 
    /* Other RAT TX Cycle value included in the QC message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_LTE_TO_HDR
  /* Do not process ORNL msg if any L2D HO, get_CGI or D2L Time Tfr in progress */
  if ( hdrmc_lte_to_hdr_ho_is_in_progress() ||
       hdrmc_lte_to_hdr_get_cgi_is_in_progress() ||
       hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
  {
    HDR_MSG_PROT_3( MSG_LEGACY_MED,
                    "No OtherRATTxCycle: L2D in progress, ho=%d, cgi=%d, rtt=%d",
                    hdrmc_lte_to_hdr_ho_is_in_progress(),
                    hdrmc_lte_to_hdr_get_cgi_is_in_progress(),
                    hdrmc_hdr_to_lte_time_transfer_is_in_progress() );
    return other_rat_tx_cycle;
  }
#endif /*FEATURE_LTE_TO_HDR*/

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
	/* Do not process ORNL msg if any L2D HO, get_CGI or D2L Time Tfr in progress */
	if ( hdralmp_hdr_to_lte_redir_meas_in_progress() )
	{
      HDR_MSG_PROT( MSG_LEGACY_MED, "OtherRATTxCycle must have already updated, if D2L redirection meas is in progress!!");
      other_rat_tx_cycle = HDRSRCH_ORNL_CYCLE_NO_UPDATE;
      return other_rat_tx_cycle;
	}
#endif

  /*---------------------------------------------------------------------------
  Check if DO to LTE reselection is supported in NV
  ---------------------------------------------------------------------------*/
  if ( !hdrovhd_d2l_is_enabled() )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "No OtherRATTxCycle: D2L reselection disabled in NV" );
    return other_rat_tx_cycle;
  }

  /*---------------------------------------------------------------------------
  Check whether LTE is supported in RAT priority list and LTE is supported 
  ---------------------------------------------------------------------------*/
  if ( !hdrovhd_check_ok_to_resel_to_lte( (sys_rat_pri_list_info_s_type *)NULL, 0) )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "No OtherRATTxCycle: LTE is not supported" );
    return other_rat_tx_cycle;
  }

  if( !hdrovhd.qc_info.initialized )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
        "No OtherRATTxCycle. No QC msg rcvd since activation");
    return other_rat_tx_cycle;
  }

  if (hdrovhd_ornl_is_updated(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL))
  {
    /* If OtherRATNeighborList is updated then pass 0xFF in which case SRCH 
       will not wakeup for OtherRATNeighborList message decoding. */
    HDR_MSG_PROT( MSG_LEGACY_MED, "OtherRATTxCycle already updated");
    other_rat_tx_cycle = HDRSRCH_ORNL_CYCLE_NO_UPDATE;
    return other_rat_tx_cycle;
  }

  if (!hdrovhd.qc_info.other_rat_available)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
       "No OtherRATTxCycle included in QC message");
    other_rat_tx_cycle = HDRSRCH_ORNL_CYCLE_DISABLED;
  }
  else
  {
    other_rat_tx_cycle = hdrovhd.qc_info.other_rat_txcycle;
  }
  
  return other_rat_tx_cycle;

} /* hdrovhd_get_other_rat_tx_cycle() */

/*===========================================================================

FUNCTION:  hdrovhd_lte_rrc_plmn_to_sys_plmn_id

===========================================================================*/
/*!
    @brief
    This function converts from the RRC PLMN ID format (1 BCD digit per byte)
    to the Sys PLMN_id_T format (2 BCD digits per byte).

    @return
    PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

 */
/*=========================================================================*/
sys_plmn_id_s_type hdrovhd_lte_rrc_plmn_to_sys_plmn_id
( 
  lte_rrc_plmn_s plmn_id  /*! RRC PLMN format */
)
{
  sys_plmn_id_s_type nas_plmn_id = { {0x00, 0x00, 0x00} };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*
** Converting CSP PLMN format to sys PLMN format.  SYS PLMN type has the below format:
**                                        Bit
**                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
**                 |===============================================|
**         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
**                 |-----------------------------------------------|
**         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
**                 |-----------------------------------------------|
**         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
**                 |===============================================|
*/

  if (plmn_id.num_mnc_digits == 2)
  {
    plmn_id.mnc[2] = 0x0F;
  }

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  nas_plmn_id.identity[0] = (byte) ((plmn_id.mcc[1] << 4) + plmn_id.mcc[0]);

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
   nas_plmn_id.identity[1] = (byte) ((plmn_id.mnc[2] << 4) + plmn_id.mcc[2]);

  /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   nas_plmn_id.identity[2] = (byte) ((plmn_id.mnc[1] << 4) + plmn_id.mnc[0]);

   return nas_plmn_id;

} /* end of hdrovhd_lte_rrc_plmn_to_sys_plmn_id() */

/*=============================================================================

FUNCTION HDROVHD_ORNL_IS_UPDATED

DESCRIPTION
  This function returns TRUE if the OtherRATNeighborList have been updated,
  otherwise it returns FALSE. 

DEPENDENCIES
  None.

PARAMETERS
  Caller - protocol calling this externalized function

RETURN VALUE
  TRUE  - If ORNL have been updated
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_ornl_is_updated( hdrhai_protocol_name_enum_type caller )
{
  boolean ornl_is_updated;  /* Are ORNL updated */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( hdrovhd.ornl_state == HDROVHD_ORNL_UPDATED )
  {
    ornl_is_updated = TRUE;
  }
  else
  {
    ornl_is_updated = FALSE;
  }

  HDRTRACE_OVHD_ORNL_IS_UPDATED( caller,
      hdrovhds_get_state(), ornl_is_updated );

  return ornl_is_updated;

} /* hdrovhd_ornl_is_updated() */

/*=============================================================================

FUNCTION HDROVHD_GET_PLMNIDS_ASSOCIATED_WITH_EARFCN

DESCRIPTION
  This function returns the PLMNIDs associated with EARFCN. 

DEPENDENCIES
  None.

PARAMETERS
  earfcn - Frequency of the LTE cell.
  req_plmnid - Matching PLMNID to be returned.

RETURN VALUE
  Number of PLMNIDs that are associated with the given EARFCN.

SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_get_plmnids_associated_with_earfcn
(   
  lte_earfcn_t earfcn,
  sys_plmn_id_s_type *req_plmnid
)
{
  hdrovhd_ornl_entry_type *ornl_msg_ptr; 
    /* Pointer to cache entry for current ORNL message */
  uint8 i = 0, j = 0, k = 0;
    /* Loop index */
  uint8 num_plmn_ids = 0;
    /* Number of PLMNIDs associated with EARFCN */
  boolean plmn_is_undefined;
    /* Indicate that the plmn contains a defined MCC and MNC (FALSE)
       or was set to 0xFFFFFF (TRUE) */
  boolean mnc_includes_pcs_digit;
    /* Indicate whether the MNC includes the third PCS digit or not */
  sys_mcc_type fplmn_mcc;
    /* MCC in an integer */
  sys_mnc_type fplmn_mnc;
    /* MNC in an integer */
  boolean is_in_fplmn = FALSE;
    /* Whether a PLMN is in the FPLMN list */
  hdrovhd_lte_rat_rec_type *lte_nbr_ptr;
    /* local pointer used to keep code line length below 80 char */
	
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The current design assumes that only one ornl_msg_ptr->other_rat record is supported */
  /* Get the entry at which the ORNL message need to be extracted */
  ornl_msg_ptr = &hdrovhd.curr_cache_entry_ptr->ornl_entry;

  ASSERT ( ornl_msg_ptr != NULL );

  lte_nbr_ptr = &(ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr);

  if (!lte_nbr_ptr->plmnid_included)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"ORNL message does not include PLMNID");
    return num_plmn_ids;
  }

  for ( i = 0; i < lte_nbr_ptr->lte_nset.num_eutra_freqs; i++ )
  {
    /* Loop till the matching reselection frequency is found. */
    if ( lte_nbr_ptr->lte_nset.earfcn_params_arr[i].earfcn == 
         earfcn )
    {
      for ( j = 0; j < lte_nbr_ptr->lte_plmnid_list[i].num_plmn_ids; j++ )
      {
        is_in_fplmn = FALSE;

        /* Check whether PLMN is in the FPLMN list */
        for ( k = 0; k < hdrovhd.fplmn_list.num_fplmn; k++ )
        {
          if ( sys_plmn_match( lte_nbr_ptr->lte_plmnid_list[i].plmnid[j], 
                               hdrovhd.fplmn_list.fplmnid[k] ) )
          {
            sys_plmn_get_mcc_mnc2(
              lte_nbr_ptr->lte_plmnid_list[i].plmnid[j],
              &plmn_is_undefined, &mnc_includes_pcs_digit,
              &fplmn_mcc, &fplmn_mnc );

            HDR_MSG_PROT_4( MSG_LEGACY_MED, 
                            "PLMN (defined: %d, pcs: %d, mcc: %d, mnc: %d) is in the FPLMN list",
                            plmn_is_undefined, mnc_includes_pcs_digit, fplmn_mcc, fplmn_mnc );
            is_in_fplmn = TRUE;
            break;
          }
        }

        if ( !is_in_fplmn )
        {
          /* Copy the PLMN if it is not in the forbidden PLMN list */
#ifdef FEATURE_MEMCPY_REMOVAL
          req_plmnid[num_plmn_ids] = lte_nbr_ptr->lte_plmnid_list[i].plmnid[j];
#else
          memcpy ( &req_plmnid[num_plmn_ids], 
                   &lte_nbr_ptr->lte_plmnid_list[i].plmnid[j], 
                   sizeof(sys_plmn_id_s_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
          num_plmn_ids++;
        }
      }
      break;
    }
  }

  return num_plmn_ids;

} /* hdrovhd_get_plmnids_associated_with_earfcn() */

/*=============================================================================

FUNCTION HDROVHD_CEHCK_OK_TO_RESEL_TO_LTE

DESCRIPTION
  This function checks if it is ok to resel to LTE. 

DEPENDENCIES
  None.

PARAMETERS
  d2l_rat_pri_list - the RAT priority list to be given in reselecton command
                     if D2L reselection is allowed.

RETURN VALUE
  TRUE is ok to resel to LTE. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_check_ok_to_resel_to_lte
(
  sys_rat_pri_list_info_s_type *d2l_rat_pri_list,
  lte_earfcn_t earfcn
)
{
  sys_rat_pri_list_info_s_type rat_pri_list;
    /* RAT priority list */
  boolean lte_is_supported = FALSE;
   /* Whether LTE is supported or not */
  boolean lte_full_srv_allowed = FALSE;
   /* Whether LTE full srv allowed in SD or not */
  boolean is_reselected_earfcn = FALSE;
   /* Whether reselected earfcn is still supported in the LTE band */
  uint8 i = 0;
   /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rat_pri_list = sd_misc_get_rat_priority_list();
  for (i = 0; i < rat_pri_list.num_items; i++)
  {
    if (rat_pri_list.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      lte_is_supported = TRUE;
	  	  
      if(d2l_rat_pri_list)/* to indicate this function is called during reselection */
      {
         if(lte_rrc_earfcn_is_supported(earfcn, 
#if (LTE_BAND_NUM == 256)
	  	                 rat_pri_list.priority_list_info[i].band_cap.lte_band_cap) == TRUE)
#else
	  	                 rat_pri_list.priority_list_info[i].band_cap) == TRUE)
#endif /* (LTE_BAND_NUM == 256)  */  	                 
         {
#if (LTE_BAND_NUM == 256)					
         		HDR_MSG_PROT_5( MSG_LEGACY_MED, 
                          "Reselected EARFCN (%d) is supported in LTE band %x_%x_%x_%x", earfcn, rat_pri_list.priority_list_info[i].band_cap.lte_band_cap.bits_1_64, \
								rat_pri_list.priority_list_info[i].band_cap.lte_band_cap.bits_65_128, rat_pri_list.priority_list_info[i].band_cap.lte_band_cap.bits_129_192, \
					   rat_pri_list.priority_list_info[i].band_cap.lte_band_cap.bits_193_256);
					
#else				  
           	HDR_MSG_PROT_2( MSG_LEGACY_MED, 
						  "Reselected EARFCN (%d) is supported in LTE band %d", earfcn, rat_pri_list.priority_list_info[i].band_cap );
#endif /* (LTE_BAND_NUM == 256)  */  
           is_reselected_earfcn = TRUE;
           break;
         }
      }
	  else
	  {
         break;
	  }
    }/* if (rat_pri_list.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE) */ 
  }

  lte_full_srv_allowed = sd_misc_is_lte_full_srv_allowed();

  if ( !lte_is_supported ||
       !lte_full_srv_allowed )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                    "GWL service not available (%d) or LTE not in RAT pri list (%d)",
                    lte_full_srv_allowed, lte_is_supported );
    hdrovhd_set_ornl_not_updated();
    return FALSE;
  }

  if(d2l_rat_pri_list && !is_reselected_earfcn)
  {
		HDR_MSG_PROT_1( MSG_LEGACY_MED, 
						  "Reselected EARFCN (%d) is not supported in LTE RAT pri list", earfcn );
		  hdrovhd_set_ornl_not_updated();
		  return FALSE;
  }
  

  if ( d2l_rat_pri_list )
  {
    *d2l_rat_pri_list = rat_pri_list;
  }

  return TRUE;
} /* hdrovhd_check_ok_to_resel_to_lte() */


/*=============================================================================

FUNCTION HDROVHD_UPDATE_LTE_FEARFCN

DESCRIPTION
  This function updates LTE forbidden EARFCN list. 

DEPENDENCIES
  None.

PARAMETERS
  earfcn - EARFCN to be added to forbidden list
  bar_time - time for which this EARFCN has to be barred

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_lte_fearfcn(lte_earfcn_t earfcn, uint16 bar_time_s)
{

    if(hdrovhd.num_avoided_earfcn == 0)
    {
       HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Starting timer for %d",(bar_time_s)* 1000);
      (void) rex_set_timer( &hdrovhd.avoid_earfcn_timer, bar_time_s*1000 );	   
    }
     hdrovhd.avoided_earfcn[hdrovhd.num_avoided_earfcn] = earfcn;
     hdrovhd.num_avoided_earfcn++;

   HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "EARFCN %d added to list of %d. bar time %d",
                earfcn, hdrovhd.num_avoided_earfcn, bar_time_s*1000);

}

#endif /* FEATURE_HDR_TO_LTE*/

#ifdef FEATURE_LTE_TO_HDR_OH
/*=============================================================================
FUNCTION HDROVHD_UPDATE_MCC_INFO

DESCRIPTION
  This function checks whether prior session is supported in current
  network based on PLMN MCC info. 

DEPENDENCIES
  None.

PARAMETERS
  plmn - PLMN iformation in SIB8 or RRCReconfig.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_mcc_info
( 
  lte_rrc_plmn_s plmn
)
{
  /* Loop index */
  uint8 i = 0;

  /* MCC */
  uint16 mcc;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Only need to check once after power up.*/
  if ( hdrovhd.mcc_is_rxed )
  {
    return;
  }

  hdrovhd.mcc_is_rxed = TRUE;
  mcc = (plmn.mcc[0]*100) + (plmn.mcc[1]*10) + (plmn.mcc[2]);
  for (i = 0; i < hdrovhd.no_ps_mcc.count; i++)
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                    "MCC in NV %d, MCC from LTE %d",
                    hdrovhd.no_ps_mcc.mcc_list[i], mcc );

    if (hdrovhd.no_ps_mcc.mcc_list[i] == mcc)
    {
      hdramp_set_ps_is_not_supported(TRUE);
      return;
    }
  }

  hdramp_set_ps_is_not_supported(FALSE);
} /* hdrovhd_update_mcc_info() */

/*=============================================================================
FUNCTION HDROVHD_UPDATE_LTE_CELL_INFO

DESCRIPTION
  This function updates the lte cell info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  cell_id - Current serving LTE cell ID.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_lte_cell_info 
( 
  lte_rrc_global_cell_id_t cell_id
)
{
  /* Whether the first cell changed indication */
  boolean first_cell_changed_ind = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                  "Update LTE cell, current cell ID %d, new cell ID %d",
                  hdrovhd.tunnel_info.cell_id, cell_id );

  if ( !hdrovhd.tunnel_info.cell_id_initialized )
  {
    first_cell_changed_ind = TRUE;
  }

  if ( cell_id != hdrovhd.tunnel_info.cell_id &&
       hdrovhd.tunnel_info.waiting_prereg_after_oos )
  {
    hdrovhd_reset_tunnel_info();
  }

  hdrovhd.tunnel_info.cell_id_initialized = TRUE;
  hdrovhd.tunnel_info.waiting_prereg_after_oos = FALSE;

  /* Give LTE cell changed indication to registered modules. */
  if ( cell_id != hdrovhd.tunnel_info.cell_id )
  {
    hdrovhd.tunnel_info.cell_id = cell_id;
    hdrind_give_ind( HDRIND_OVHD_LTE_CELL_CHANGED,
                     (void *)&first_cell_changed_ind );
  }

} /* hdrovhd_update_lte_cell_info() */

/*=============================================================================
FUNCTION HDROVHD_UNPACK_PREREG_INFO

DESCRIPTION
  This function unpack and store PreReg Parameters from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  prereg_params_ptr - Pointer to latest prereg info.
  cc_changed - Whether color code is changed.
 
RETURN VALUE
  TRUE  - If PreReg Params has changed 
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_unpack_prereg_info 
(      
  lte_rrc_hdr_sib8_prereg_params_s *prereg_params_ptr,
  boolean *cc_changed
)
{
  hdrovhd_tunnel_ovhd_info_type *tunnel_info_ptr;

  /* Whether overhead parameter is up to date */
  boolean ovhd_param_diff = FALSE;

  uint8 i = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tunnel_info_ptr = &hdrovhd.tunnel_info;
  *cc_changed = FALSE;

  /* Populate pre-registration allowed flag */
  tunnel_info_ptr->prev_prereg_is_allowed = tunnel_info_ptr->prereg_is_allowed;
  tunnel_info_ptr->prereg_is_allowed = prereg_params_ptr->prereg_allowed;

  HDR_MSG_PROT_2( MSG_LEGACY_MED, "preReg, previous: %d, current: %d", 
                  tunnel_info_ptr->prev_prereg_is_allowed, 
                  tunnel_info_ptr->prereg_is_allowed );

  /* PreRegParams is considered different than the previous one if any of the following 
   * conditions are ture:
   * 1. PreReg_allowed field changed
   * 2. PreRegZoneID field changed
   * 3. NumOfSecPreRegZoneID changed
   * 4. NumOfSecPreRegZoneID doesn't change but value is greater than 
   *    MAX_SEC_COLORCODES. (this is becasue we can only store MAX_SEC_COLORCODES
   *    SecPreRegZoneID) 
   */
  if ( hdrovhd.qc_info.color_code != prereg_params_ptr->preregZoneId )
  {
    ovhd_param_diff = TRUE;
    *cc_changed = TRUE;
  }
  else if ( ( tunnel_info_ptr->prev_prereg_is_allowed != tunnel_info_ptr->prereg_is_allowed ) ||
       ( prereg_params_ptr->numOfSecPreRegZoneID > HDROVHD_MAX_SEC_COLORCODES ) ||
       ( tunnel_info_ptr->prev_sec_cc_count != prereg_params_ptr->numOfSecPreRegZoneID ) )
  {
    ovhd_param_diff = TRUE;
  }
  else if ( ( prereg_params_ptr->numOfSecPreRegZoneID <= HDROVHD_MAX_SEC_COLORCODES ) && 
            ( tunnel_info_ptr->prev_sec_cc_count == prereg_params_ptr->numOfSecPreRegZoneID ) &&
            ( hdrovhd.sp_buffer.sec_color_code_count != HDROVHD_SCC_UNSPECIFIED ) )
  {
    /*  5. If NumOfSecPreRegZoneID doesn't change and value is less or equal to  
     *     MAX_SEC_COLORCODES. Need to compare each entry... 
     */
    for ( i = 0; i < hdrovhd.sp_buffer.sec_color_code_count; i++ )
    {
      if ( hdrovhd.sp_buffer.sec_color_code_list[i] != 
           prereg_params_ptr->secondary_preregZoneId_list[i] )
      {
        ovhd_param_diff = TRUE;
        break;
      }
    }
  }

  /* Populate color code field */
  hdrovhd.qc_info.color_code = prereg_params_ptr->preregZoneId;

  if ( prereg_params_ptr->numOfSecPreRegZoneID == 0 )
  {
    /* SecondaryColorCode is not included in the SIB8 */
    tunnel_info_ptr->prev_sec_cc_count = prereg_params_ptr->numOfSecPreRegZoneID;
    hdrovhd.sp_buffer.sec_color_code_count = HDROVHD_SCC_UNSPECIFIED;
  }
  else
  {
    /* Populate second color code field. */
    tunnel_info_ptr->prev_sec_cc_count = prereg_params_ptr->numOfSecPreRegZoneID;

    hdrovhd.sp_buffer.sec_color_code_count = 
      ( prereg_params_ptr->numOfSecPreRegZoneID <= LTE_RRC_MAX_HDR_PREREG_ZONES ) ? 
      prereg_params_ptr->numOfSecPreRegZoneID : LTE_RRC_MAX_HDR_PREREG_ZONES;

    /* Populate secondary preregzonid to sp_buffer */
    for ( i = 0; i < hdrovhd.sp_buffer.sec_color_code_count; i++ )
    {
        hdrovhd.sp_buffer.sec_color_code_list[i] = 
        prereg_params_ptr->secondary_preregZoneId_list[i];
    }
  }

  return ovhd_param_diff;

} /* hdrovhd_unpack_prereg_info() */


/*=============================================================================

FUNCTION HDROVHD_UPDATE_SIB8_INFO

DESCRIPTION
  This function updates the SIB8 info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  sib8_info - Pointer to latest SIB8 info.
  give_sib8_ind - Whether to give pre-registration update indication. FALSE if
                  HDR is inactive. TRUE when HDR is online.
 
RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/

void hdrovhd_update_sib8_info 
( 
  lte_rrc_hdr_sib8_info_ind_s *sib8_info, 
  boolean give_sib8_ind 
)
{
  hdrovhd_tunnel_ovhd_info_type *tunnel_info_ptr;

  /* Whether overhead parameter is up to date */
  boolean ovhd_param_diff = FALSE;

  /* Whether color code is changed */
  boolean cc_changed = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tunnel_info_ptr = &hdrovhd.tunnel_info;

  /* Check if LTE cell is changed. */
  hdrovhd_update_lte_cell_info( sib8_info->global_cell_id );
  hdrovhd_update_mcc_info( sib8_info->camped_plmn );

  /* Populate pre-registration present flag */
  tunnel_info_ptr->prev_prereg_is_presented = tunnel_info_ptr->prereg_is_presented;
  tunnel_info_ptr->prereg_is_presented = sib8_info->prereg_params_present;

  if ( sib8_info->prereg_params_present )
  {
    ovhd_param_diff = hdrovhd_unpack_prereg_info(
      &sib8_info->prereg_params, &cc_changed );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "PreReg info not included" ); 

    /* If prereg_info is not presented, change pre_reg_is_allowed to FALSE */
    tunnel_info_ptr->prev_prereg_is_allowed = tunnel_info_ptr->prereg_is_allowed;
    tunnel_info_ptr->prereg_is_allowed = 0;

    HDR_MSG_PROT_2( MSG_LEGACY_MED, "pre-reg, previous: %d, current: %d", 
                    hdrovhd.tunnel_info.prev_prereg_is_allowed, 
                    hdrovhd.tunnel_info.prereg_is_allowed );
  }

  /*  SIB8 is considered different then previous one if any of the following
   *  conditions are true: 
   *  1. PreRegParams is different. This is checked in unpack_prereg_info
   *  2. If PreReg_Params_Presented flag changed
   *  3. Number of Neighbor flag changed (we may not need to check this since
   *     NeighborCellList info is also passed from LTE to HDR SRCH)
   */
  if ( tunnel_info_ptr->prev_prereg_is_presented != 
         tunnel_info_ptr->prereg_is_presented )
  {
    ovhd_param_diff = TRUE;
  }

  /* Populate number of neighbor */ 
  tunnel_info_ptr->prev_neighbor_count = sib8_info->neigh_cell_info_list_size;

  /*TODO: we can store neighbor info to sp_buffer, similar thing we do for Secondary CC */

  tunnel_info_ptr->sib8_is_updated = TRUE;

  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) && 
       give_sib8_ind &&
       ( hdrsap_get_current_tunnel_mode_value( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) 
           != HDRHAI_TUNNEL_MODE_NO_SRV ) )
  {
    if ( ovhd_param_diff )
    {
      hdrind_give_ind( HDRIND_OVHD_PREREG_INFO_UPDATED, 
                       (void *)&hdrovhd.tunnel_info.prereg_is_allowed );
    }

    /* HDR will either update UATI or restore prior session when color code
     * is changed.
     * Data Service will try to open connection when it receives color code 
     * changed indication. If HDR updates UATI, the AlternateLinkOpenReq will 
     * be queued in the slp until UATIAssignment is received. If HDR restores 
     * prior session, by the time DS receives the indication, HDR session has 
     * been closed and CONNECTION_NOT_ALLOWED indication will be returned to 
     * DS if DS tries to open a connection. (DS will backoff 15s and retry)
     */ 
    if ( cc_changed )
    {
      hdrind_give_ind( HDRIND_OVHD_COLOR_CODE_CHANGED, 
                       (void *)&hdrovhd.qc_info.color_code );
    }
  }
} /* hdrovhd_update_sib8_info() */

/*=============================================================================

FUNCTION HDROVHD_UPDATE_PREREG_INFO

DESCRIPTION
  This function updates the pre-regisration info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  prereg_info - Pointer to latest pre-registration info.
  give_prereg_ind - Whether to give pre-registration update indication. FALSE if
                    HDR is inactive. TRUE when HDR is online.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/

void hdrovhd_update_prereg_info 
( 
  lte_rrc_hdr_pre_reg_info_ind_s *prereg_info, 
  boolean give_prereg_ind 
)
{
  /* Whether overhead parameter is up to date */
  boolean ovhd_param_diff = FALSE;

  /* Whether color code is changed */
  boolean cc_changed = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if LTE cell is changed. */
  hdrovhd_update_lte_cell_info( prereg_info->global_cell_id );
  hdrovhd_update_mcc_info( prereg_info->camped_plmn );

  /* Set pre-registration present flag to 1 */
  hdrovhd.tunnel_info.prev_prereg_is_presented = hdrovhd.tunnel_info.prereg_is_presented;
  hdrovhd.tunnel_info.prereg_is_presented = TRUE;

  ovhd_param_diff = hdrovhd_unpack_prereg_info(
    &prereg_info->prereg_params, &cc_changed );

  hdrovhd.tunnel_info.sib8_is_updated = TRUE;

  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) && 
       give_prereg_ind &&
       ( hdrsap_get_current_tunnel_mode_value( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) 
           != HDRHAI_TUNNEL_MODE_NO_SRV ) )
  {
    if ( ovhd_param_diff )
    {
      hdrind_give_ind( HDRIND_OVHD_PREREG_INFO_UPDATED, 
                       (void *)&hdrovhd.tunnel_info.prereg_is_allowed );
    }

     /* HDR will either update UATI or restore prior session when color code
     * is changed.
     * Data Service will try to open connection when it receives color code 
     * changed indication. If HDR updates UATI, the AlternateLinkOpenReq will 
     * be queued in the slp until UATIAssignment is received. If HDR restores 
     * prior session, by the time DS receives the indication, HDR session has 
     * been closed and CONNECTION_NOT_ALLOWED indication will be returned to 
     * DS if DS tries to open a connection (DS will backoff 15s and retry). 
     */ 
    if ( cc_changed )
    {
      hdrind_give_ind( HDRIND_OVHD_COLOR_CODE_CHANGED, 
                       (void *)&hdrovhd.qc_info.color_code );
    }
  }
} /* hdrovhd_update_prereg_info() */

/*=============================================================================
FUNCTION HDROVHD_UPDATE_LTE_CELL_INFO

DESCRIPTION
  This function updates the lte cell info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  cell_id_ptr - Pointer to allocated memory where the lte cell id will
                get copied.

RETURN VALUE
  E_SUCCESS - cell id is initialized
  E_FAILURE - otherwise

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_lte_cell_id 
( 
  lte_rrc_global_cell_id_t *cell_id_ptr 
)
{
  if ( hdrovhd.tunnel_info.cell_id_initialized )
  {
    *cell_id_ptr = hdrovhd.tunnel_info.cell_id;
    return E_SUCCESS;
  }
  else
  {
    return E_FAILURE;
  }

} /* hdrovhd_get_lte_cell_id() */


/*=============================================================================
FUNCTION HDROVHD_PREREG_IS_ALLOWED

DESCRIPTION
  This function tells whether pre-registration is allowed or not in tunnel mode.

DEPENDENCIES
  None.

PARAMETERS
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  TRUE - pre-registration is allowed 
  FALSE - pre-registration is not allowed

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_prereg_is_allowed 
( 
  hdrhai_protocol_name_enum_type caller
)
{

  HDRTRACE_OVHD_PRE_REG_IS_ALLOWED( caller, 
                                    hdrovhds_get_state(),
                                    hdrovhd.tunnel_info.prereg_is_allowed );

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "HDR pre-reg allowed=%d", 
              hdrovhd.tunnel_info.prereg_is_allowed);

  return hdrovhd.tunnel_info.prereg_is_allowed;
} /* hdrovhd_prereg_is_allowed() */


/*=============================================================================
FUNCTION HDROVHD_PREV_PREREG_IS_ALLOWED

DESCRIPTION
  This function tells whether the previous pre-registration is allowed or not
  in tunnel mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - previous pre-registration is allowed 
  FALSE - previous pre-registration is not allowed

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_prev_prereg_is_allowed( void )
{
  return hdrovhd.tunnel_info.prev_prereg_is_allowed;

} /* hdrovhd_prev_prereg_is_allowed() */


/*=============================================================================
FUNCTION HDROVHD_SET_PRE_REG_IS_ALLOWED

DESCRIPTION
  This function tells sets pre-registration is allowed or not in tunnel mode.

DEPENDENCIES
  None.

PARAMETERS
  pre_reg_is_allowed - whether pre-reg is allowed or not.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_set_pre_reg_is_allowed 
( 
  boolean pre_reg_is_allowed
)
{
  hdrovhd.tunnel_info.prev_prereg_is_allowed = 
    hdrovhd.tunnel_info.prereg_is_allowed;
  hdrovhd.tunnel_info.prereg_is_allowed = pre_reg_is_allowed;

  if ( hdrovhd.tunnel_info.prev_prereg_is_allowed && 
       !hdrovhd.tunnel_info.prereg_is_allowed )
  {
    hdrind_give_ind( HDRIND_OVHD_PREREG_INFO_UPDATED, 
                     (void *)&hdrovhd.tunnel_info.prereg_is_allowed );
  }

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR pre-reg allowed flag updated=%d", 
                  hdrovhd.tunnel_info.prereg_is_allowed );

} /* hdrovhd_set_pre_reg_is_allowed() */


/*===========================================================================
FUNCTION HDROVHD_IS_SIB8_UPDATED

DESCRIPTION
  This function tells whether sib8 or preregistration info has been received.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrovhd_is_sib8_updated( void )
{
  return hdrovhd.tunnel_info.sib8_is_updated;
}

/*===========================================================================
FUNCTION HDROVHD_SET_WAITING_PREREG_AFTER_OOS

DESCRIPTION
  This function sets the flag of waiting_prereg_after_oos.
  
DEPENDENCIES
  None.

PARAMETERS
  value - the boolean value to be set.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrovhd_set_waiting_prereg_after_oos
( 
  boolean value
)
{
  hdrovhd.tunnel_info.waiting_prereg_after_oos = value;
} /* hdrovhd_set_waiting_prereg_after_oos() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */
/*===========================================================================

FUNCTION HDROVHD_GET_CGI_PILOT_STRENGTH

DESCRIPTION
  This function finds out pilot strength for CGI response.

DEPENDENCIES
  None

PARAMETERS
  pilot_pn - pilot PN

RETURN VALUE
  the strength of the PN

SIDE EFFECTS
  None

===========================================================================*/
int16 hdrovhd_get_cgi_pilot_strength
( 
  uint16 pilot_pn
)
{
  if ( pilot_pn == hdrovhd.cgi_info_pilot_rpt.link.pilot_pn )
  {
    return hdrovhd.cgi_info_pilot_rpt.strength;
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "CGI: Can't find PN %d", pilot_pn );
    return 0;
  }

} /* hdrovhd_get_cgi_pilot_strength() */

#ifdef FEATURE_LTE_DEPRIORITIZATION
/*=============================================================================

FUNCTION HDROVHD_UPDATE_LTE_DEPRI_FREQ

DESCRIPTION
  This function updates LTE deprioritized frequencies. 

DEPENDENCIES
  None.

PARAMETERS
  depri_req - pointer to deprioritize request structure
  is_online - whether HDR is online.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_lte_depri_freq 
( 
  hdrcp_lte_depri_freq_req_t *depri_req,
  boolean is_online
)
{
  uint8 num_depri_freq = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( depri_req->depri_all_freq )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "All frequencies are deprioritized" );
    hdrovhd.depri_freq.depri_all_freq = TRUE;
  }
  else
  {
    /* If depri_all_freq is not set and 0 frequency is deprioritized,
     * deprioritized frequencies will be restored with previous priorities.
     */
    if ( depri_req->num_depri_freq <= HDRCP_MAX_LTE_DEPRI_FREQ )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "%d deprioritized frequencies updated", 
                      depri_req->num_depri_freq );
      num_depri_freq = depri_req->num_depri_freq;
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Too many deprioritized frequencies (%d)", 
                      depri_req->num_depri_freq );
      num_depri_freq = HDRCP_MAX_LTE_DEPRI_FREQ;
    }

    hdrovhd.depri_freq.num_depri_freq = num_depri_freq;
    hdrovhd.depri_freq.depri_all_freq = FALSE;
    memset( hdrovhd.depri_freq.depri_freq, 0, 
            sizeof(hdrovhd.depri_freq.depri_freq) );
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( hdrovhd.depri_freq.depri_freq, sizeof(hdrovhd.depri_freq.depri_freq), 
                depri_req->depri_freqs, num_depri_freq*sizeof(uint16) );
#else
    memcpy( hdrovhd.depri_freq.depri_freq, depri_req->depri_freqs, 
            num_depri_freq*sizeof(uint16) );
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

#ifdef FEATURE_HDR_TO_LTE

  if ( !is_online )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH,
                  "Receive deprioritized freq request when HDR is offline." );
    return;
  }

  /* If ORNL is in UPDATED state, all other conditions to update nset is
   * considered to be satisfied. */
  if( hdrovhd.ornl_state == HDROVHD_ORNL_NOT_UPDATED )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Don't update nset because ornl is not updated" );
    return;
  }
#if (LTE_BAND_NUM == 256)  
  hdrovhd_lte_nset_update( SYS_LTE_BAND_MASK_CONST_NONE, FALSE );
#else
  hdrovhd_lte_nset_update( 0, FALSE );
#endif /*(LTE_BAND_NUM == 256)*/
#endif /* FEATURE_HDR_TO_LTE*/

} /* hdrovhd_update_lte_depri_freq() */

#ifdef FEATURE_HDR_TO_LTE
/*=============================================================================

FUNCTION HDROVHD_UPDATE_EARFCN_PRIORITY

DESCRIPTION
  This function updates the priority of the EARFCN of the
  neighboring EUTRA network. 

DEPENDENCIES
  None.

PARAMETERS 
  earfcn - pointer to the LTE frequency structure

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
static void hdrovhd_update_earfcn_priority
( 
  hdrsrch_lte_freq_params_type *earfcn
)
{
  uint8 i = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrovhd.depri_freq.depri_all_freq )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "freq %d is deprioritized", earfcn->earfcn );
    earfcn->earfcn_priority = HDRSRCH_LTE_FREQ_LOWEST_PRIORITY;
  }
  else
  {
    for ( i = 0; i < hdrovhd.depri_freq.num_depri_freq; i++ )
    {
      if ( hdrovhd.depri_freq.depri_freq[i] == earfcn->earfcn )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "freq %d is deprioritized", earfcn->earfcn );
        earfcn->earfcn_priority = HDRSRCH_LTE_FREQ_LOWEST_PRIORITY;
        break;
      }
    }
  }
} /* hdrovhd_update_earfcn_priority() */

#endif /* FEATURE_HDR_TO_LTE */
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_HDR_TO_LTE
/*=============================================================================

FUNCTION HDROVHD_UPDATE_SPLMN_INFO

DESCRIPTION
  This function tells whether Serving Network PLMNIDs in the OtherRATNeighborList
  message have been changed and update SD accordingly.

DEPENDENCIES
  None.

PARAMETERS
  qc_info_ptr - pointer to the QuickConfig message
  ornl_msg_ptr - pointer to the ORNL message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_splmn_info
( 
  hdrovhd_qc_info_type *qc_info_ptr,
  hdrovhd_ornl_entry_type *ornl_msg_ptr
)
{
  sd_splmn_info_list_s splmn_info;
  hdrovhd_lte_plmnid_list_type *curr_splmn = NULL;
  uint8 i = 0, j = 0;
  boolean splmn_changed = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( qc_info_ptr != NULL )
  {
    /* Usually we should check if SPLMN is changed when ORNL is received. In 
     * the scenario where ORNL is disabled, we check QuickConfig message's
     * OtherRatAvailable field. 
     * If ORNL is always disabled, we don't have to tell SD. BSR will be used 
     * if SPLMN is not provided. */
    if ( qc_info_ptr->other_rat_available == FALSE &&
         ( hdrovhd.splmn_info.num_splmn != 0 || 
           hdrovhd.first_splmn_after_acq ) )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Update SPLMN info, null" );
      memset(&hdrovhd.splmn_info, 0, sizeof(sd_splmn_info_list_s));
      sd_ss_ind_hdr_update_splmn_list( &hdrovhd.splmn_info );
    }
  }
  else if ( ornl_msg_ptr != NULL )
  {
    /* ORNL msg is received. Check if SPLMN is changed. */
    curr_splmn = &ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr.lte_serving_plmnid_list;
    memset(&splmn_info, 0, sizeof(sd_splmn_info_list_s));

    for ( i = 0; i < curr_splmn->num_plmn_ids; i++ )
    {
      for ( j = 0; splmn_changed == FALSE, j < hdrovhd.splmn_info.num_splmn; j++ )
      {
        if ( sys_plmn_match(curr_splmn->plmnid[i], hdrovhd.splmn_info.splmns_list[j]) )
        {
          break;
        }
      }
      if ( j == hdrovhd.splmn_info.num_splmn )
      {
        splmn_changed = TRUE;
      }
      splmn_info.splmns_list[i] = curr_splmn->plmnid[i];
    }

    if ( hdrovhd.first_splmn_after_acq ||
         splmn_changed ||
         ( (hdrovhd.splmn_info.num_eutra_freq != 
              ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr.lte_nset.num_eutra_freqs) &&
           ((hdrovhd.splmn_info.num_eutra_freq == 0) ||
            (ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr.lte_nset.num_eutra_freqs == 0)) ) )
    {
      splmn_info.num_splmn = curr_splmn->num_plmn_ids;
      memscpy( &hdrovhd.splmn_info, sizeof(sd_splmn_info_list_s),
               &splmn_info, sizeof(sd_splmn_info_list_s) );
      hdrovhd.splmn_info.num_eutra_freq = 
        ornl_msg_ptr->other_rat[0].other_rat_rec.lte_nbr.lte_nset.num_eutra_freqs;
      sd_ss_ind_hdr_update_splmn_list( &hdrovhd.splmn_info );
    }
  }

  hdrovhd.first_splmn_after_acq = FALSE;

} /* hdrovhd_update_splmn_info() */
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
/*=============================================================================
FUNCTION HDROVHD_UPDATE_SYS_AVOID_LIST

DESCRIPTION
  This function reads system avoidance list from SD and updates
  system avoidance timer

DEPENDENCIES
  This function must be called from the same task as HDROVHD.
  
PARAMETERS
  num_channels - Max number of entries in avoided_chan_rec
  avoided_chan_rec - num_channels entries of avoided channels
  
RETURN VALUE
  Number of avoided channels read from SD.
  
SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_update_sys_avoid_list
(
  uint8 max_channels,
  sys_avoided_channel_record_type* avoided_chan_rec
)
{
  uint8 min_avoid_timer_idx;
  uint8 avoided_chan_idx; /* debug purpose only to print avoided channels*/
  uint8 num_channels;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(rex_self() != HDROVHD_TASK_PTR)
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR, "Function hdrovhd_update_sys_avoid_list() \
                        called from outside hdrmc task: returning");
    return 0;
  };

  sd_misc_sys_avoid_list(SYS_SYS_MODE_HDR, avoided_chan_rec, 
                         &num_channels, max_channels);

  if(num_channels > max_channels)
  {
    /* If more channels are returned than max channel. Then limit to max channels */
    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "SD returned %d channels, more than max size %d. Limiting to max",
                                         num_channels,max_channels);
	num_channels = MIN( num_channels, max_channels ); 
  }
  /* Print if avoided ch = 1 as F3 for 1 avoided ch is skipped in for loop */
  if(num_channels == 1)
  {
    HDR_MSG_PROT_3 (MSG_LEGACY_MED, "avoided channel: mode: %d, band: %d, chan: %d",
                    avoided_chan_rec[0].mode,
                    avoided_chan_rec[0].channel.band,
                    avoided_chan_rec[0].channel.chan_num
                    );
  }  	
  min_avoid_timer_idx = 0;
  /* start the loop from 1 as min_avoid_timer_idx = 0 so it saves the loop 
  in case of 1 avoided channel */
  for (avoided_chan_idx = 1; avoided_chan_idx < num_channels; avoided_chan_idx++ )
  {
    HDR_MSG_PROT_3 (MSG_LEGACY_MED, "avoided channel: mode: %d, band: %d, chan: %d",
                    avoided_chan_rec[avoided_chan_idx].mode,
                    avoided_chan_rec[avoided_chan_idx].channel.band,
                    avoided_chan_rec[avoided_chan_idx].channel.chan_num
                    );
    if (avoided_chan_rec[avoided_chan_idx].avoided_time < 
        avoided_chan_rec[min_avoid_timer_idx].avoided_time)
    {
      min_avoid_timer_idx = avoided_chan_idx;
    }
  }

  rex_clr_timer( &hdrovhd.sys_avoid_timer );
  if (num_channels > 0) 
  {
    rex_set_timer( &hdrovhd.sys_avoid_timer,
                   avoided_chan_rec[min_avoid_timer_idx].avoided_time - 
                   avoided_chan_rec[min_avoid_timer_idx].uptime + 
                   SYS_AVOID_TIMER_MARGIN );
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "avoid timer set for %d ms",
                    avoided_chan_rec[min_avoid_timer_idx].avoided_time - 
                    avoided_chan_rec[min_avoid_timer_idx].uptime + 
                    SYS_AVOID_TIMER_MARGIN );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "No avoided system" );
  }
  return num_channels;
}
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
