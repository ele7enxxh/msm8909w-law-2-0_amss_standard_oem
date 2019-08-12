#ifndef DSTASK_V_H
#define DSTASK_V_H
/*===========================================================================

                     D A T A   S E R V I C E S   T A S K

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the Data Services (DS) Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the Data Services Task.

  Signal and command definitions for the Data Services Task, and functions
  that should be used by other tasks to send commands to the Data Services
  Task are defined here.


  Copyright (c) 2001-2016 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dstask.h_v   1.17   28 Feb 2003 18:56:06   rchar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/dstask_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
06/03/15   vls     Added a new s2b command to handle wp addr change
05/04/15   skc     Added new s2b commands to handle wp flow enabled/disabled   
12/09/14   sd      Removal of BCMCS commands.
11/25/14   ab      Added support for PLMN change event cmd  
09/23/14   ab      Added support for wlan proxy bearer tech change event cmd   
07/14/14   scb     Add support for DS_CMD_MPPM_NULL_APN_RESOLVED_INFO
07/07/14   ab      Added a new API to put cmd in the internal DS queue     
06/27/14   tk      Added support for Dynamic ATCoP.
04/15/14   sc      Fixed +COPS DSDS issue.
03/15/14   scb     Added support for MPPM MMGSDI SUBSCRIPTION INFO.   
02/25/14   scb     Added support for MPPM EPC timers. 
12/17/13   sc      Fixed +CSIM APDU response size issue.
11/21/13   scb     Add support for WLAN_MAX_CONN reached indication in MPPM
11/12/13   sc      Added support for $QCCSGCOPS command.
11/06/13   scb     Add support for PDN Database updates in MPPM
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
09/23/2013 scb     Add support for throttling in MPPM.
09/23/13   ssb     Changing variable name from ds_3gpp_pdn_bearer_wm_type
                   to ds_3gpp_pdn_bearer_wm_info
09/16/13   scb     Add support for LTE_ATTACH_INIT status indication in MPPM
09/15/13   rs      Common build changes for dual sim and triple sim
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/24/13   sd      TSTS changes for DS3G.
05/02/13   vs      NV item to check call origination is allowed before PS attach
03/17/13   tk      Fixed stack overflow issue with emergency number list.
02/22/13   op      Added support for the IWLAN_S2B module
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
02/07/13   tk      ATCoP changes for DSDx Co-existence with TDS.
01/25/13   sd      Mainlining Modem Statistics Feature
01/23/13   sk      DS command buffer memory reduction.
11/21/12   sk      Fixed CSQ issue due to invalid updation of RSSI,BER values.
11/08/12   ssb     Dun Call Entitlement support using PS_SYS Interface
10/23/12   scb     Added support for MPPM in DS task. 
09/24/12   tk      Migrated to CM APIs for MRU table access.
06/19/12   nd      MT PDP pkt call.
08/06/12   tk      Added support for $QCDRX command.
07/31/12   sk      C-EONS Feature Support.
07/16/12   nc      New interface changes for Manual PLMN selection.
06/12/12   tk      Extended $QCSYSMODE command for 3GPP2 modes.
07/29/11   ua      Updated ds_at_mmgsdi_sim_info_type to have accessed rec num
                   and accessed offset to be used in mmgsdi read handlers.
04/19/12   sb      Feature Cleanup: Always On Flag FEATURE_UIM_SUPPORT_3GPD
03/07/12   sk      Made CLCC common for all modes.
03/12/12   ss      RPM: Fixed issue of oprt_mode and regd_serv_domain not 
                   being set after UE power-up.
03/09/12   msh     Removed FEATURE_DATA_NIKEL     
02/23/12   kv      Replaced FEATURE_DATA_TEST_LOOPBACK_HDLR with
                   FEATURE_DATA_TEST_LOOPBACK.
02/17/11   ua      Added support for $QCRMCALL. 
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/08/12   vs      Added support for 3GPP IPv6 over PPP
02/08/12   sk      Feature cleanup.
02/01/12   ss      RPM: Fixed of sending ATTACH request again due to PS session
                   initiation after PS ATTACH is ignored by network.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/31/12   sk      Removed featurization for call_direction.
01/19/12   sk      Feature cleanup.
01/16/12   ack     Fix to post a cmd for iface bringup/teardown
01/02/12   msh     Coan: Feature cleanup
12/26/11   ss      Added support for new command to process IPCP delayed NAK.
11/17/11   sa      Added support for V6 Loopback Mode B.
11/02/11   msh     Added DS_707_PARSE_DORM_TIMER_SOCM_CMD     
10/31/11   ssh     eHRPD Always On support
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
10/25/11   ua      Adding EFHPLMN to DSAT_MAX_NUM_REFRESH_FILES. 
10/24/11   ss      Fixed RPM mmgsdi refresh registration issue.   
10/17/11   ad      Added +CECALL support.
09/06/10   nc      Fixed the RSSI and RSCP display when 
                   FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY is enabled
09/15/11   vpk     Tear down all the ifaces when an UIM refresh happens.
09/08/11    ash    Added support for eHRPD to HRPD fallback.
09/05/11   ss      Changes to support RPM.
08/20/11   ttv     GSDI to MMGSDI_SESSION api migration changes.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/06/11   ash     Added support for PPP Partial Context
06/20/11   ttv     Merged silent redial support changes for UMTS tethered calls.
04/11/11   ttv     Added support for Hot Swap feature.
03/24/11   sa      Added MT PDP Auto answer support.
03/23/11   sn      Globals cleanup for free floating of tasks.
03/14/11   sch     Added DS3GPP_DSM_MEM_CTRL_EV_CMD and related event type
03/02/11   ss      Added support for configurable RAB_REESTAB RETRY.
03/01/11   nc      Modified +CEMODE to be inline with CM Design.
02/11/11   vs      DUN changes for CSFB Fusion
02/02/11   ack     Added DS_RLP_INIT_CMD
02/04/11   mg      Added DS_707_PWROFF_LPM_MODE_CMD to process pwroff/lpm 
                   mode notification from CM
01/21/11   sa      Add support for CSFB call in service change SS evt from LTE to 1x.
01/17/11   ss      Added support for Iface linger.
01/14/11   ack     Added PHY_LINK_DOWN_EV reg/dereg to dssnet4/6 SM
01/04/11   op      Added MMGSDI event for ACL enable/disable check
12/15/10   ms      Handle Iface related events by posting command to DS task.
11/22/10   ttv     Added support for DSDS.
11/19/10   ars     Changed proc of setupBridgeInterfaceEvent to command based
10/25/10   ad      Init Changes for DSDS. 
10/23/10   pp      ds_send_cmd[_ext]APIs are introduced for sending commands 
                   to DS task. Cmd payloads are de-coupled from Task buffer.
10/12/10   mg      Changes for error code propogation to ps_iface  
09/08/10   kk      Including pbmlib.h for CMI compliance.
08/22/10   vk      Use a single signal for all forwarding ports
07/27/10   bs      Fixed MMGSDI commands queuing in RPC context.
07/09/10   asn     Support for Arbitration
07/08/10   ad      Added support for $ECALL command.
07/08/10   ssh     New command for PDN Inactivity timer expiry
07/07/10   ms      Data statistics support for Field Test Display.
06/25/10   vs      Support for Carrier Detect Signals for Fusion.
06/17/10   as      Fusion additions
06/04/10   bs      MMGSDI SESSION Migration.
05/17/10   ad      Added Support for +CEREG for LTE.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
05/10/10   kk      Added support for $QCVOLT, $QCHWREV, $QCBOOTVER, $QCTEMP,
                   $QCAGC, $QCALLUP.
05/10/10   kk      Added support for ^DSDORMANT command.
05/10/10   kk      Added support for ^HDRCSQ command.
04/21/10   sa      Added support for Loopback Mode B.
04/14/10   sn      Mainlined GOBI changes.
03/15/10   ls      Silent Redial Feature
02/04/10   asn     Process DSM Mem events in DS Task context
01/16/10   ls      Add Message id when registering client id
01/15/10   ua      Added support for ^CPIN, CPIN, CLCK, CLCC for CDMA.
01/06/10   bs      Added +CGACT command support for LTE.
11/16/09   ca      Added support for the MMGSDI authentication
                   interface commands.
12/15/09   nc      Featurisation changes for LTE.
12/12/09   sa      Added support for ATCoP - Mode handler interface.
11/25/09   ls      Add support for retrieving slot info
11/22/09   vk      Added support for internal DS command queue
11/09/09   ls      Add support for retrieving IMSI info from USIM card
10/21/09   ls      Add app_type field into ds3g_mmgsdi_event_info
10/20/09   ss      Feature wrapping BCMCS header file inclusion and definitions.
09/22/09   vrk     Merged LTE changes.
08/22/08   ua      Added support for N/w initated Qos.
09/09/09   ua      Added support for EONS (spec 22.101).
08/10/09   ms      Changes related to CM AU level CMI changes.
08/10/09   ms      Remove private header file.
08/07/09   ls      Add union structure for CSIM feature
07/23/09   ls      Support FEATURE_CSIM
07/16/09   sch     Adding item srv_status to structure ds_ss_info_type
07/02/09   ss      WLAN decoupling from DS task.
06/19/09   ls      Support operation mode change
06/12/09   pp      DS Task de-coupling: Removed PS logical mode handler
                   references.
04/30/09   ls      Support LPM mode change.
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
03/13/09   ls      Merge the changes from MFLO
03/09/09   sa      AU level CMI modifications.
03/06/09   ms      CMI Integration fixes.
02/15/09   ls      Added DS_CALL_TYPE_PKT_IS707B and epzid in ds3g_pkt_call_info_type
1/13/09    nc      Fixing Featurisation issue.
12/29/08   nc      Added support for +VTS Command
12/17/08   ms      Added code to handle the pref_mode_change_event from CM
                   and to tear down the call (dormant and active both).
11/21/08   sa      Added synchronization of +CRSM command with MMGSDI.
10/23/08   bs      Added support for 1X AT Phonebook commands.
10/18/08   pp      Added support for SLIP interface.
10/17/08   bs      Fixed Klocwork issues.
09/30/09   yz      Increased DS command buffer size to 32 for UMTS/GSM, added
                   timestamp info in DS command.
06/23/08   ar      Added QMI proxy iface mode handler support.
09/03/08   ms      Feature enhancements for BCMCS 2.0 Phase2
08/05/08   ms      IS707B addendum Feature support.
07/21/08   bs      Added support for CDMA NAARefresh.
07/21/08   bs      Added support for +CREG=2 command.
07/16/08   ua      Added support for external client support.
                   Optimized received SIO signals usage. This is feature wrapped in
                   FEATURE_DSAT_EXT_CLIENT_SUPPORT.
07/08/08   ua      Added support for DSUMTSPS_PDP_IP_IRAT_TX_UM_CMD.
06/30/08   ssh     UICC support.
06/16/08   sn      Removed "snd.h" include as it is not required.
06/13/08   ua      Correcting reading of data from RUIM/NV items
                   for OMH 2.
04/15/08   ms      Feature BCMCS2.0 Enhancements
02/20/08   vd      MBMS Support.
02/14/08   vk      Graceful termination of PDP-PPP calls.
02/04/08   ssh     Changes to MIPv6 commands
01/29/08   asn     Serialize processing of FLOW IOCTL - QOS_RELEASE
                   by moving it to DS Task context
01/11/08   vk      Changes to propagate HSPA call status indication
01/10/08   spn     Added Commands for Memory Based Flow Control
12/12/07   as      Added support hybrid mode hdr serving system
11/30/07   asn     Added support for new feature MT-PDP terminating in TE
11/20/07   vp      Added support for MIPv6
11/15/07   sc      Added support to send PPP auth starting event to IFACE
10/08/07   ar      Added DS_SMD_DATA11_PORT_1ST_SIO_RX_SIG
09/21/07   ss      Added rtre_control in ds_at_ph_info_u_type.
09/11/07 sq/mga    Merged changes for FEATURE_RUIM_FUNC_INTERFACE
09/07/07   rt      Added support for IWLAN.
08/16/07   as      Added support for DS_707_PKT_SYS_CHG_CMD
06/19/07  ak/squ   Added a new member named 'call_inst' to the
                   structure: ds_call_info_type
06/11/07   as      Added DS_HDR_AN_PPP_START_CMD
06/06/07   ar      Add CM call ID to ds_ucsd_uus_data_cmd_type.
05/29/07  squ/mga  Added new field ln_status to union cmd
                   Added   DS_707_HDR_LN_STATUS_CMD.
05/01/07   sk      QoS Modify support
04/20/07   vk      Processing GSNDCP reg cnf cbs in DS context
03/28/07   sa      Added active_calls_list_s_type to ds_at_call_info_s_type.
03/08/07   ua      Added sim state in ds_at_ss_info_s_type
02/04/06   sr      Added support for STA UMTS mode handler commands
02/14/07   vd      Lint error fixes.
01/10/07   sq      Added support of DS_707_RETRY_DELAY_ORIG_CMD.
01/18/06   rr      Added dormant_reorig in dsumtsps_call_info.
12/07/06   as      Added DEREGED cmd used for MIP deregistration
11/16/06   sq      Add DS_707_PH_SYS_SEL_PREF_NORMAL_CMD.
11/09/06   vrk     Post command to DS to process tx high and low wmk
10/31/06   spn     Changes to consolidate a burst of commands in flow mgr
09/26/06   sa      Inter RAT handover support for $QCDGEN.
09/06/06   rsl     Added 2 new commands PHYS_LINK_FLOW_ENABLE and IFACE_FLOW_ENABLE.
09/06/06   ks      Featurized under FEATURE_MMGSDI.
09/05/06   ks      Added members to ds_qmi_cmd_type for QMI sim support.
08/23/06   ks      Removed RMNET_SIO_SIG.
07/14/06   ks      Added structs to hold CM SS and PH info in ds_qmi_cmd_type
07/11/06   ua      CACM/CAMM modifications according to the new CM API's
                   added cm_ph_aoc_info_s_type member in ds_at_ph_info_u_type
07/06/06   sk      QoS 2.0 Support
05/12/06   ua      Added mmgsdi_sim_info_type to provide MMGSDI support for CPOL command.
                   It is wrapped in FEATURE_MMGSDI.
05/01/06   vp      Defining sio_rx signals for shared memory data ports (for
                   multiple processor builds)
04/16/06   squ/gr  Added failed_orig and call_cmd_err fields to the ds_call_info_type.
03/06/06   sq      Added support to ds_hdr_an_ppp_ev_cmd
02/21/06   lyr     Added support for WLAN QoS-related cmds and types
01/19/06   sl      Serializer related changes.
11/14/05   gr      Changes to throttle the um link when the rm link is slower
                   than the um link (specifically the BT scenario)
10/12/05   mct     Added DS_DSSNET4_FLOW_DELETED_EV_CMD.
10/10/05   ks      Removed DS_RMNET_SIO_RDM_[OPEN/CLOSE]_CMD.
09/13/05   ks      Featurized cm_call_orig_fail_layer_e_type temporarily.
09/13/05  jd/ks    Added DS_RMNET_SM_CMD. Added ds_rmnet_sm_cmd_type to
                   ds_cmd_type.
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
09/09/05   ks      Corrected featurization for nas member union
                   in ds_qmi_cmd_type.
09/08/05   ks      Added member for qmi_nas in ds_qmi_cmd_type.
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
08/26/05   ar      Added support for CS Data in-call modification & fallback.
08/05/05   sk/vs   Merged the HDR Semi-Dynamic QOS changes from sandbox.
08/05/05   kvd     Featurized #include dsdctm.h nder DS707.
08/03/05   hap     Added csd_rate_adaption to ds_at_call_info_s_type
08/03/05   kvd     Featurized mip_ppp_status under DS707.
07/29/05   ar      Remove ds_at_cm_ss_event_type. Added DS_AT_CM_MSG_SIG.
07/22/05   kvd     Added ds_mip_ppp_status to cmd union in ds_cmd_type.
07/18/05   ar      Add support for asynchronous call setup response.
06/27/05   ks      Added QMI_CHARGER cmd
06/27/05   snb     Add field to ds_at_ps_data_gen_type supporting $QCDGEN
                   data send over multiple contexts simulataneously.
05/31/05   jd      Added QMI command.
06/09/05   aku     Added support for WLAN WPA
05/31/05   jd      Added QMUX receive signal and RDM cmd user data field.
05/25/05   atp     Added support for semi-dynamic QOS on DO (flow_mgr cmds).
05/20/05   ar      Added support for DS_CM_CALL_PROGRESS_CMD event.
05/11/05   ks      merged the change for ds_at_cm_ph_pref_type for CM mode
                   preference event from tip, as UMTS needs it.
04/21/05   mct     Removed include ps_bcmcs.h.
04/26/05   gr      Added the inc_call flag in the ds3g_pkt_call_info_type
04/20/05   mct     Removed phys_link down references for dssnet/rmsm.
04/20/05   ar      Add ds_at_cm_ph_pref_type for CM mode preference event.
04/14/05   tkk     Added new values to ds_at_ui_cmd_e_type and removed
                   FEATURE_DSAT_SOUND portion.
04/08/05   ar      Add ds_at_cm_ph_pref_type for CM mode preference event.
04/06/05   gr      Moved the ds707_epzid.h include under 707 data feature
04/05/05   gr      Integrated EPZID functionality onto the main line
04/01/05   snb     Add channel_id field to ds_at_gsdi_apdu_resp_type for +CSIM
                   and +CRSM commands with USIM card.
03/15/05   ks      Removed Featurization of RMNET and QMI.
03/11/05   snb     Added DS_AT_UI_CMD event.
03/08/05   ks      Changed names of RM NET signals, cmds, functions.
03/02/05   snb     Add value to cmd for ds_cmd_type, associated data type and
                   nsapi info to ds_at_call_info_s_type in support of AT
                   command for generating PS data, merged ss_info and
                   call_direction additions to ds_at_call_info_s_type from
                   Sirius branch and removed timezone feature definition.
03/01/05   kvd     Added RDUD cmds.
02/28/05   rr      Added end_type to ds_call_info_type to distinguish between
                   CM's and Internal call end commands
02/21/05   sl      Serializer related changes.
02/15/05   jd      Added Rm Network interface rmsm event commands
02/10/05   sb      Accessories related changes.
01/19/05   ar      Merge in changes from ATCOP development branch.
01/12/05   aku     Added support for WLAN,
12/07/04   ar      Add available and preferred networks to PH info event.
                   Add sys_id to SS info event.
12/02/04   vas     Numbered all the DS commands
11/20/04   kvd     Added CAM module related DS commands.
11/30/04   dwp     Add FEATURE_UUS around relative types.
11/29/04   vr      Merged PZID hysteresis changes into main line
11/19/04   tkk     Updated "ds_at_ss_info_s_type" structure with
                   "sys_mm_information_s_type" element for supporting
                   +CTZR
11/18/04   dwp     Added DS_UCSD_USER_USER*_CMD, ds_ucsd_uus_data_cmd_type, &
                   ds_ucsd_uus_info_type to ds_ucsd_app_orig_call_cmd_type.
11/17/04   mv      Added DSUMTSPS_SEND_MT_REJECT_CMD.
11/14/04   mv      Added mt_flag to dsumtsps_call_info_type structure and
                   dsumtsps_mt_index to the ds_cmd_type structure.
11/09/04   ar      Generalize ATCOP GSDI SIM read to support write.
11/04/04   gr      Added EPZID_HYS_SDB_DATA_CMD to notify the hysteresis
                   engine whenever SDB packet data is exchanged
11/03/04   dwp     Added reject to ds_ucsd_app_answer_call_cmd_type.
10/12/04   vr      Added EPZID_HYS_DATA_READY_CMD to notify the hysteresis
                   engine whenever PPP data is exchanged
09/20/04   vas     QOS related changes
09/17/04   rc      Added DS_CM_SS_PS_DATA_FAIL_CMD.
09/10/04   sb      S-PDP changes
09/03/04   snb     Add support for AT command +CSIM under FEATURE_MMGSDI_
                   ATCSIM and structure additions for command +CHLD.
09/02/04   vas/jkomenda Added command for CM subscription changed event.
                   Added signal for RUIM command processing
07/16/04   ar      Add dial modifiers to ds_ucsd_app_orig_call_cmd_type.
07/06/04   snb     Add support for personalization PINs for ATCOP.
06/01/04   ar      Added DS_UCSD_GSDI_MSG_SIG definition.
05/25/04   vr      Fixed featurization bug for FEATURE_BCMCS
05/21/04   jd      Renamed FEATURE_AT_707_REINIT to FEATURE_UIM_SUPPORT_3GPD
05/17/04   vr/kvd  Added new BCMCS commands and command data types.
04/07/04   gr      Added force_cdma_only parameter into
                   ds3g_pkt_call_info_type
03/22/04   gr      Added command to process the expiry of Go Null timer.
                   Go Null timer is the same as Abort Dormant PPP timer.
02/27/04   kvd     Added DS_707_RMSM_UM_MIP_IP_ADDR_CHANGED_CMD.
02/24/04   snb     Added support for handling SMSes via signals.
02/02/04   jd      Removed RMSM MIP_DOWN cmd, renamed MIP_UP as IFACE_UP
01/06/03   snb     Move registration for pin event with GSDI and handling
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.
12/02/03   sb      Open ATCOP SMS events to both ETSI and CDMA
11/11/03   ar      Adjust ETSI wrapper for ATCOP events.
10/29/03   snb     Added support for ATCOP initiated asynchronous SIM read
                   and relocation of CM SS and PH cmd/events to common files.
10/14/03   ar      Add support for ATCOP network registration reporting.
10/13/03   vas     Removed Call Connected/End events for HDR AN module
10/13/03   kvd/vr  Added DS_707_INITIATE_ALN_CMD
10/01/03   ak      Added EPZID Delay timer command.
10/01/03   ar      Restructure ATCOP event types to minimize storage.
                   Remove FEATURE_DATA_ETSI_SUPSERV wrappers
10/01/03   sb      SMS Cell broadcast additions
09/10/03   ar      Added support for CSD handover event.
09/10/03   ak      Removed extern of ds_esn.  never used.
09/02/03   sy      Added epzid hysteresis timer expired command.
08/27/03   vr      Added new cmd for Idle Digital Mode changed and added
                   new_network field to ds3g_pkt_call_info_type
08/24/03   ak      New cmds for IFACE_UP, _DOWN and _ROUTEABLE indications
                   for 1X packet.
07/25/03   ak      Added call_mode field to call_info struct.
06/20/03   dgy     Added UMTS CS Data API commands.
05/21/03   vas     Added HDR commands.
04/18/03   sb      Partition CM supserv event info to conserve storage.
04/18/03   vsk     updated to use new UMTS RMSM cmd and names.
                   added info_ptr to dsumtsps_call_info_type
04/11/03   ak      Updated to use new 707 RMSM CMD names.
02/26/03   rc      Added field ps_data_suspend to ds_ss_info_type.Renamed
                   DS_CM_SS_SYS_MODE_CHG_CMD as DS_CM_SS_SRV_CHG_CMD.
01/10/03   sb      Event handler declarations for +CGATT command
11/14/02   jd      Removed DS_RMSM_UM_PPP_UP_CMD (not used)
11/13/02   rc      Added multimode support for GPRS.
11/12/02   ar      Add support for supplementary services events
11/11/02   sb      Call Related CM events now apply in all modes
10/30/02   rsl     Made call_cmd and call_event common.
10/14/02   ak      Added client_id to call_info_type.  This helps support
                   JCDMA.
10/14/02   ak      Removed JCDMA commands - no longer needed.
10/14/02   sb      Merge from ETSI. PBM event declarations and state machine.
10/14/02   vsk     Added support to get statistics at end of call
10/10/02   ak      Updated for JCDMA 1X.
10/04/02   usb     Added Generic RM State Machine specific commands
10/03/02   rc      Added UMTS specific command handling.
08/28/02   usb     Added/renamed RMSM cmd names
08/14/02   atp     Added DS_707_ASYNC_ATZ_CMD.
08/13/02   dgy     Added return value to ds_get_call_information() and
                   ds_get_current_session_data_counters().
08/02/02   dgy     Added support for UI data call screen.
08/01/02   rc      Removed FEATURE_DS_SOCKETS wrap around socket commands.
                   Added DS_FLOW_CTRL_DSMGR_MASK to sockets flow control mask.
07/29/02   ak      Added use_hdown_timer field to ds3g_pkt_call_info_type.
07/24/02   atp     Added DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD.
07/22/02   ak      Added DS_707_PKT_IFACE_DOWN_CBACK_CMD.
07/19/02   aku     Moved flow control masks to ds_flow_control.h
07/16/02   aku     Removed SOCKET featurization for dssnet flow mask.
07/13/02   atp     Added commands for 707 async.
07/01/02   ak      Added signal for re-origing a 707 pkt call.
06/18/02   tmr     Added DS_COMPLETE_LL_DISCONNECT_CMD, DS_GCSD_RX_DCD_ON_CMD and
                   DS_GCSD_TX_DATA_FLUSHED_CMD
05/21/02   smp     Added DS_CM_SS_SYS_MODE_CHG_CMD.
05/21/02   ak      #ifdef'd some RAB stuff since CM does it.  Should soon
                   go away.  But this helps compile.
05/20/02   usb     Added new command ids for RMSM (IS707 pkt call Rm handler)
05/16/02   smp     Updates for Multimode CM.
05/08/02   ak      Added end_status to call_info type.
05/08/02    wx     Remove FEATURE_DSAT_TIMER
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Added DS_INITIATE_CALL_CMD, DS_ENTER_SOCKET_MODE_CMD,
                   DS_EXIT_SOCKET_MODE_CMD, DS_ORIG_SOCKET_NET_CMD,
                   DS_RELEASE_SOCKET_NET_CMD. Added ds_call_type_e_type and
                   ds_call_stack_e_type and ds_orig_call_type.
04/04/02   tmr     Modified definitions of DS_TASK_START_SIG,
                   DS_TASK_STOP_SIG and DS_TASK_OFFLINE_SIG
02/26/02   tmr     Added DS_COMPLETE_LL_CONNECT_CMD, DS_RELEASE_CALL_CMD
                   and DS_GCSD_XID_NEG_COMPL_CMD.
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. Added call_type parameter
                   for commands received from CM.
11/21/01   tmr     Added DS_GCSD_DISCONNECT_CMD command ID
10/31/01   smp     Removed extra comma from ds_cmd_enum_type.
08/31/01   smp     Updates due to CM name changes.
05/24/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dstask.h"
#include "queue.h"
#include "cm.h"

#include "err.h"
#include "amssassert.h"
#include "rdevmap.h"
#include "fs_public.h"
#include "sys.h"
#include "nv.h"
#include "task.h"
#include "cause.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Data Services Task Signal Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Task top-level signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_TASK_START_SIG       TASK_START_SIG         /* 0x8000           */
#define DS_TASK_STOP_SIG        TASK_STOP_SIG          /* 0x4000           */
#define DS_TASK_OFFLINE_SIG     TASK_OFFLINE_SIG       /* 0x2000           */
#define DS_CMD_Q_SIG            0x1000   /* Command Queue signal           */
#define DS_DOG_HB_REPORT_SIG    0x0800   /* Dog HB Report signal           */
#define DS_NV_CMD_SIG           0x0400   /* NV item retrieval signal       */
#define DS_TIMER_SIG            0x0200   /* Timer signal                   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  IS-707 Sub-task signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_PKT_REORIG_SIG       0x00000100  /* called when want to reorig  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  QMI NAS client signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_NOTIFY_NAS_SVC_SIG      0x00200000
#define DS_SEND_MSG_WAIT_SIG       0x00400000

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Serial Port signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_DATA_PORT_1ST_SIO_RX_SIG      0x00100000
#define DS_DTR_EVENT_SIG                 0x00800000
#define DS3G_SIOLIB_BLOCK_SIG          0x80000000

#define DS_SIO_RX_PKT_SIG       0x0004   /* Protocol packet rx'ed          */
#define DS_SIO_ESCAPE_SIG       0x0002   /* AT escape sequence detected    */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ATCOP Sub-task signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_AT_MT_MSG_SIG        0x10000  /* MT message received            */
#define DS_AT_SMS_SIG           0x20000  /* Other, non-"MT msg" SMS event  */
#define DS_AT_CM_MSG_SIG        0x40000  /* CM message received            */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  UCSD Sub-task signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_UCSD_GSDI_MSG_SIG    0x00010  /* GSDI message received          */
#define DS_UIM_CMD_SIG          0x00020  /* UIM Command Signal             */
#define DS_RMSM_FC_SIG          0x80000  /* RMSM FC Command Q signal */

#ifdef FEATURE_DATA_ATP_IMS_HTTP
#define DS_IMS_MSG_SIG          0x0080   /* signal sent by IMS libary*/
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  DPM signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_DPM_WAIT_SIGNAL      0x01000000  /* DPM Wait signal  */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  MAX command in a module
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_MAX_CMDS_PER_MODULE  1000

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ZERO command payload size
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_CMD_PAYLOAD_SIZE_ZERO  (size_t)0

/*---------------------------------------------------------------------------
  Data call types: circuit-switched or packet. In each mode, circuit-switched
  and packet data calls may be handled by different call control modules.
---------------------------------------------------------------------------*/
typedef enum
{
  /*-----------------------------------------------------------------------
  Main call type is used by different call control modules
  -------------------------------------------------------------------------*/
  DS_CALL_TYPE_UNSPECIFIED = -1,
  DS_CALL_TYPE_CKT = 0,                      /* Circuit-switched data call */
  DS_CALL_TYPE_PKT,                                    /* Packet data call */
  DS_NUM_CALL_TYPES,            /* !! For internal use of 3G Dsmgr only !! */
  /*-----------------------------------------------------------------------
  Sub call type is the subset of main call type
  -------------------------------------------------------------------------*/
  DS_CALL_TYPE_PKT_IS707B                      /* IS707B data call type    */

} ds_call_type_e_type;


/*---------------------------------------------------------------------------
  Data Services Task Command Definitions
---------------------------------------------------------------------------*/

typedef enum
{
  DS_SUBS_NONE = -1,
  DS_FIRST_SUBS = 0,             /* Indicates the ds_cmd is for First Subs */
  DS_SECOND_SUBS = 1,            /* Indicates the ds_cmd is for Second Subs */
#ifdef FEATURE_TRIPLE_SIM
  DS_THIRD_SUBS,
#endif /* FEATURE_TRIPLE_SIM */
  DS_ALL_SUBS,
  DS_SUBS_MAX
}ds_subs_e_type;

#define CSIM_MAX_CMD_APDU_SIZE 260

#ifdef FEATURE_DUAL_SIM

typedef sys_modem_as_id_e_type ds_sys_modem_as_id_e_type;

#else

typedef uint8 ds_sys_modem_as_id_e_type;

#endif /* FEATURE_DUAL_SIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Commands that may be sent to the Data Services Task.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum
{
  DS_MIN_CMD = 0,

  /*-------------------------------------------------------------------------
    DSMGR Commands (1 - 999)
  -------------------------------------------------------------------------*/
  DS_CMD_DSMGR_INIT_COMPLETE                   = 1,
  /* DSMG Initialization Completed */

  DS_CMD_STATS_INIT                            = 2,
  /* Cmd to call hdrrlpstats initialization routines */

  DS_CMD_MSGR_RECV                             = 3,
  /* Message received from MSGR */

  DS_CMD_UT_SYNC                               = 4,
  /* Sync up request from UT framework */

  DS_CMD_TIMER_EXPIRED                         = 5,
  /* Notification of timer expiry */

  DS_CMD_FLOW_MGR_TIMER_EXPIRED                = 6,
  /* Flow manager timer expired */

  DS_CMD_MCFG_REFRESH_IND                      = 7,
  /* command to process MCFG refresh ind */

  DS_CMD_CFCM_THERMAL_RUNAWAY_IND              = 8,
  /* command to process CFCM thermal runaway ind */

  DS_CMD_RDM_OPEN                              = 20,
  /* Cmd from RDM to open a serial port */

  DS_CMD_RDM_CLOSE                             = 21,
  /* Cmd from RDM to close a serial port */

  DS_CMD_SIO_CLOSE_COMPLETE                    = 22,
  /* Cmd from SIOLIB when sio_close is done */

  DS_CMD_SIO_DSM_MEM_CTRL                      = 23,
  /* Cmd from SIOLIB */

  DS_CMD_SIO_DTR_CHANGED                       = 24,
  /* DTR changed event notification */

  DS_CMD_SIO_CD_CHANGED                        = 25,
  /* CD changed event notification */

  DS_CMD_ENTER_ONLINE_CMD_TX_FLUSH             = 26,
  /* Goto online flush TX */

  DS_CMD_ENTER_ONLINE_DATA_TX_FLUSH            = 27,
  /* Goto online-data, flush TX */

  DS_CMD_ENTER_ONLINE_DATA_NO_TX_FLUSH         = 28,
  /* Goto online-data, don't flush TX */

  DS_CMD_COMPLETE_ONLINE_CMD_SWITCH            = 29,
  /* OK to finish online-cmd switch   */

  DS_CMD_COMPLETE_ONLINE_DATA_SWITCH           = 30,
  /* OK to finish online-data switch  */

  DS_CMD_CM_CALL_EV_ORIG                       = 50,
  /* Cmd for CM_CALL_EVENT_ORIG */

  DS_CMD_CM_CALL_EV_END                        = 51,
  /* Cmd for CM_CALL_EVENT_END */

  DS_CMD_CM_CALL_EV_INCOM                      = 52,
  /* Cmd for CM_CALL_EVENT_INCOM */

  DS_CMD_CM_CALL_EV_CONNECT                    = 53,
  /* Cmd for CM_CALL_EVENT_CONNECT */

  DS_CMD_CM_CALL_EV_SETUP_IND                  = 54,
  /* Cmd for CM_CALL_EVENT_SETUP_IND */

  DS_CMD_CM_CALL_EV_PROGRESS_INFO_IND          = 55,
  /* Cmd for CM_CALL_EVENT_PROGRESS_INFO_IND */

  DS_CMD_CM_CALL_EV_CALL_CONF                  = 56,
  /* Cmd for CM_CALL_EVENT_MODIFY_IND,
     CM_CALL_EVENT_MODIFY_RES,
     CM_CALL_EVENT_MODIFY_COMPLETE_CONF */

  DS_CMD_CM_CALL_EV_MODIFY_IND                 = 57,
  /* Cmd for CM_CALL_EVENT_CALL_CONF */

  DS_CMD_CM_CALL_EV_PDP_NW_MODIFY_REQ          = 58,
  /* Cmd for CM_CALL_EVENT_PDP_NW_MODIFY_REQ */

  DS_CMD_CM_CALL_EV_PS_SIG_REL_CNF             = 59,
  /* Cmd for CM_CALL_EVENT_PS_SIG_REL_CNF */

  DS_CMD_CM_CALL_EV_INCOMING_PAGE              = 60,
  /* Cmd for CM_CALL_EVENT_INCOMING_PAGE */

  DS_CMD_CM_CALL_EV_GET_PDN_CONN_IND           = 61,
  /* Cmd for CM_CALL_EVENT_GET_PDN_CONN_IND */

  DS_CMD_CM_CALL_EV_PDN_CONN_REJ_IND           = 62,
  /* Cmd for CM_CALL_EVENT_PDN_CONN_REJ_IND */

  DS_CMD_CM_CALL_EV_PDN_CONN_FAIL_IND          = 63,
  /* Cmd for CM_CALL_EVENT_PDN_CONN_FAIL_IND */

  DS_CMD_CM_CALL_EV_ACT_BEARER_IND             = 64,
  /* Cmd for CM_CALL_EVENT_ACT_BEARER_IND */

  DS_CMD_CM_CALL_EV_BEARER_ALLOC_REJ_IND       = 65,
  /* Cmd for CM_CALL_EVENT_RES_ALLOC_REJ_IND */

  DS_CMD_CM_CALL_EV_BEARER_ALLOC_FAIL_IND      = 66,
  /* Cmd for CM_CALL_EVENT_RES_ALLOC_FAIL_IND */

  DS_CMD_CM_CALL_EV_BEARER_MOD_REJ_IND         = 67,
  /* Cmd for CM_CALL_EVENT_BEARER_MOD_REJ_IND */

  DS_CMD_CM_CALL_EV_MOD_BEARER_IND             = 68,
  /* Cmd for CM_CALL_EVENT_MOD_BEARER_IND */

  DS_CMD_CM_CALL_EV_RAB_REL_IND                = 69,
  /* Cmd for CM_CALL_EVENT_RAB_REL_IND */

  DS_CMD_CM_CALL_EV_RAB_REESTAB_IND            = 70,
  /* Cmd for CM_CALL_EVENT_RAB_REESTAB_IND */

  DS_CMD_CM_CALL_EV_RAB_REESTAB_REJ            = 71,
  /* Cmd for CM_CALL_EVENT_RAB_REESTAB_REJ */

  DS_CMD_CM_CALL_EV_RAB_REESTAB_FAIL           = 72,
  /* Cmd for CM_CALL_EVENT_RAB_REESTAB_FAIL */

  DS_CMD_CM_CALL_EV_PDP_MODIFY_IND             = 73,
  /* Cmd for CM_CALL_EVENT_PDP_MODIFY_IND */

  DS_CMD_CM_CALL_EV_PDP_PROMOTE_IND            = 74,
  /* Cmd for CM_CALL_EVENT_PDP_PROMOTE_IND */

  DS_CMD_CM_CALL_EV_PDP_MODIFY_CONF            = 75,
  /* Cmd for CM_CALL_EVENT_PDP_MODIFY_CONF */

  DS_CMD_CM_CALL_EV_PDP_MODIFY_REJ             = 76,
  /* Cmd for CM_CALL_EVENT_PDP_MODIFY_REJ */

  DS_CMD_CM_CALL_EV_USER_DATA_IND              = 77,
  /* Cmd for CM_CALL_EVENT_USER_DATA_IND */

  DS_CMD_CM_CALL_EV_USER_DATA_CONG_IND         = 78,
  /* Cmd for CM_CALL_EVENT_USER_DATA_CONG_IND */

  DS_CMD_CM_SS_EV_SRV_CHG                      = 100,
  /* Cmd for CM_SS_EVENT_SRV_CHANGED */

  DS_CMD_CM_SS_EV_PS_DATA_FAIL                 = 101,
  /* Cmd for CM_SS_EVENT_PS_DATA_FAIL */

  DS_CMD_CM_SS_EV_CNTXT_TRANSFER               = 102,
  /* Cmd for  CM_SS_EVENT_IRAT_CTXT_TRANSFER */

  DS_CMD_CM_PH_EV_INFO                         = 110,
  /* Cmd to notify CM PH events */

  DS_CMD_CM_PH_EV_SUB_INFO                     = 111,
  /* Cmd for CM SUB info */

  DS_CMD_CM_AC_STATE                           = 120,
  /* Cmd for CM AC state */

  DS_CMD_CM_AC_EVENT                           = 121,
  /* Cmd for CM AC state */

  DS_CMD_CM_PLMN_ID_CHG                        = 122,
  /* Cmd for PLMN ID change */

  DS_CMD_CM_PDN_CONNECTIVITY_REQ_ERR           = 123,
  /*  Cmd when CM returns error for PDN Req */

  DS_CMD_MMGSDI_SIM_INFO                       = 130,
  /* MMGSDI SIM event cmds  */

  DS_CMD_PROCESS_DEVICE_SETTINGS               = 131,

  DS_CMD_DSD_TIMER_EXPIRED                     = 150,
  /* Cmd for DSD timer expired */

  DS_CMD_DSD_EVENT_NOTIFY                      = 151,
  /* Cmd for DSD event notify */

  DS_CMD_DSD_RECOMPUTE                         = 152,
  /* Cmd for DSD recompute */

  DS_CMD_DSD_EXT_NOTIFY                        = 153,
  /* Cmd for DSD external notify */

  DS_CMD_DSD_MODE_PREF_CHG                     = 154,
  /* Cmd for DSD mode pref change */

  DS_CMD_DSD_RESET                             = 155,
  /* Cmd for DSD reset */

  DS_CMD_DSD_APM_APN_CHG                       = 156,
  /* Cmd for APM APN change */

  DS_CMD_DSD_APM_REFRESH_ATTACH_PDN_LIST       = 157,
  /* Cmd for APM Refresh attach pdn list */

  DS_CMD_DSD_APM_SET_APN_SWITCH_STATUS         = 158,
  /* Cmd for APM set APN switch status */

  DS_CMD_DSD_APM_APN_DISABLE                   = 159,
  /* Cmd for APM APN disable */

  DS_CMD_DSD_DSC_LTE_ENABLE                    = 160,
  /* Cmd for DSC LTE enable/disable */

  DS_CMD_DS3GDSDIF_PREF_SYS_CHANGED            = 161,
  /* Cmd for DS3GDSDIF Pref sys changed */

  DS_CMD_DS3GDSDIF_SS_EVENT_PROC_DONE          = 162,
  /* Cmd for DS3GDSDIF SS event processing done */

  DS_CMD_DS3GDSDIF_WLAN_SYS_CHG                = 163,
  /* Cmd for DS3GDSDIF WLAN sys change */

  DS_CMD_DS3GDSDIF_VOICE_CALL_EVENT            = 164,
  /* Cmd for DS3GDSDIF voice call event */

  DS_CMD_DSD_LTE_EMERG_CALL_EVENT              = 165,
  /*Cmd for LTE PS Emergency call is in progress*/

  DS_CMD_DS3GDSDIF_MT_CSFB_HYST_TIMER_EXP      = 166,
  /* Cmd for DS3GDSDIF MT CSFB estab hysteresis timer expired */

  DS_CMD_PROFILE_DB_EXT                        = 170,
  /* Cmd for Interfaces Porfile DB cmd */

  DS_CMD_PROFILE_DB_NV_REFRESH_EXT             = 171,
  /*Cmd for Interfaces Profile DB NV Refresh*/

  DS_CMD_PROFILE_DB_DEVICE_MODE_EXT            = 172,
  /*Cmd for Interfaces Profile Device Mode*/
  DS_CMD_DSMGR_LOW_LATENCY_CMD                 = 173,
  /*-------------------------------------------------------------------------
    DSSNET6 commands
  -------------------------------------------------------------------------*/
  DS_CMD_DSSNET6_PHY_LINK_UP_EV                = 201,
  DS_CMD_DSSNET6_PHY_LINK_GONE_EV              = 202,
  DS_CMD_DSSNET6_PHY_LINK_DOWN_EV              = 203,
  DS_CMD_DSSNET6_LINK_UP_EV                    = 204,
  DS_CMD_DSSNET6_LINK_DOWN_EV                  = 205,
  DS_CMD_DSSNET6_LINK_RESYNC_EV                = 206,
  DS_CMD_DSSNET6_NET_UP_EV                     = 207,
  DS_CMD_DSSNET6_NET_DOWN_EV                   = 208,
  DS_CMD_DSSNET6_NET_ABORTED_EV                = 209,
  DS_CMD_DSSNET6_PPP_CLOSE_EV                  = 210,
  DS_CMD_DSSNET6_FLOW_DELETED_EV               = 211,
  /* End of DSMGR Commands */

  /*-------------------------------------------------------------------------
    3GPP MH Commands (1000 - 1999)
  -------------------------------------------------------------------------*/
  DS_CMD_3GPP_BRIDGE_INTERFACE_SETUP              = 1000,
  /* Cmd for bridging/unbridging UM and RM */

  DS_CMD_3GPP_CALL_PENDING_CLR                    = 1010,
  /* Cmd for clearing pending calls */

  DS_CMD_3GPP_CALL_STOP_DATA_AVAIL_IND            = 1011,
  /* Cmd for stopping sending data avil ind to CM */

  DS_CMD_3GPP_DORM_REORIG                         = 1020,
  /* Cmd for re-originating call from dormancy */

  DS_CMD_3GPP_DORM_REORIG_TMR_EXP                 = 1021,
  /* Cmd for notifying dormancy re-orig timer expired */

  DS_CMD_3GPP_DPM_DEREG                           = 1030,
  /* Cmd for deregistering with DPM */

  DS_CMD_3GPP_DSM_MEM_CTRL_EV                     = 1040,
  /* Cmd for notifying DSM memory control event */

  DS_CMD_3GPP_INIT_AFTER_POWERUP                  = 1050,
  /* Cmd for reading efs files after UE init */

  DS_CMD_3GPP_EMBMS_IFACE_BRING_UP                = 1060,
  /* Cmd for bringing up EMBMS iface */

  DS_CMD_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND       = 1061,
  /* Cmd for sending EMBMS TMGI activated ind */

  DS_CMD_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND     = 1062,
  /* Cmd for sending EMBMS TMGI deactived ind */

  DS_CMD_3GPP_FLOW_FLTR_ID_CONFLICT               = 1070,
  /* Cmd for notifying flow filter id conflict */

  DS_CMD_3GPP_FLOW_FLTR_PRECED_CONFLICT           = 1071,
  /* Cmd for notifying flow filter precedence conflict */

  DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER             = 1072,
  /* Cmd for invoking lower layer flow function */

  DS_CMD_3GPP_GCSD_DISCONNECT                     = 1080,
  /* Cmd for GCSD call disconnect */

  DS_CMD_3GPP_GCSD_RX_DCD_ON                      = 1081,
  /* Cmd for GCSD call connected */

  DS_CMD_3GPP_GCSD_TX_DATA_FLUSHED                = 1082,
  /* Cmd for GCSD call disconnect complete */

  DS_CMD_3GPP_GSNDCP_REG_CNF                      = 1090,
  /* Cmd setting up transmit function with GSNDCP */

  DS_CMD_3GPP_GSNDCP_RESUME_CNF                   = 1091,
  /* Cmd for confirmation of GSNDCP resume */

  DS_CMD_3GPP_IFACE_BRING_UP_RSP                  = 1100,
  /* Cmd for processing incoming call event */

  DS_CMD_3GPP_IFACE_DOWN                          = 1101,
  /* Cmd for iface down */

  DS_CMD_3GPP_IFACE_FLOW_DISABLE                  = 1102,
  /* Cmd for iface flow disable */

  DS_CMD_3GPP_IFACE_FLOW_ENABLE                   = 1103,
  /* Cmd for iface flow enable */

  DS_CMD_3GPP_IFACE_IN_USE                        = 1104,
  /* Cmd for iface in use */

  DS_CMD_3GPP_IFACE_OUT_OF_USE                    = 1105,
  /* Cmd for iface out of use */

  DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF        = 1106,
  /* Cmd for tearing down invalid PCSCF */

  DS_CMD_3GPP_IFACE_UP                            = 1107,
  /* Cmd for iface up */

  DS_CMD_3GPP_IFACE_UP_EMERGENCY                  = 1108,
  /* Cmd for emergency iface up */

  DS_CMD_3GPP_EPS_KAMGR_IFACE_DOWN_EV_CB          = 1121,
  /* Cmd for KAM iface down event callback */

  DS_CMD_3GPP_EPS_KAMGR_IFACE_UP_EV_CB            = 1122,
  /* Cmd for KAM iface up event callback */

  DS_CMD_3GPP_EPS_KAMGR_IFACE_ALL_UP_EV_CB        = 1123,
  /* Cmd for KAM all ifaces up event callback */

  DS_CMD_3GPP_EPS_KAMGR_SYS_INFO                  = 1124,
  /* Cmd for KAM to process sys info */

  DS_CMD_3GPP_EPS_KAMGR_TIMER_EXPIRY_CB           = 1125,
  /* Cmd for KAM timer expiry */

  DS_CMD_3GPP_LOOPBACK_HDLR                       = 1130,
  /* Cmd for 3GPP loopback */

  DS_CMD_3GPP_MT_SEND_REJECT                      = 1140,
  /* Cmd for sending MT call reject */

  DS_CMD_3GPP_MT_TE_ANS_TIMEOUT                   = 1141,
  /* Cmd for handling MT answer timeout */

  DS_CMD_3GPP_PDN_CONN_REQ_RESEND                 = 1150,
  /* Cmd for resending PDN connectivity request */

  DS_CMD_3GPP_PDN_INACTIVITY_TMR_EXP              = 1151,
  /* Cmd for PDN inactivity timer expiring */

  DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP          = 1152,
  /* Cmd for PDN connectivity timer expiring */

  DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP     = 1153,
  /* Cmd for PDN request throttle timer expiring */

  DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP          = 1154,
  /* Cmd for PDN wait timer expiring */

  DS_CMD_3GPP_PDN_POLLING_TMR_EXP                 = 1155,
  /* Cmd for PDN polling timer expiring */

  DS_CMD_3GPP_PDN_REMOVE_UNUSED                   = 1156,
  /* Cmd for removing unused PDN */

  DS_CMD_3GPP_PDN_THROT_CLEAR_APN_REJ_T3402_TMR   = 1157,
  /* Cmd for clearing T3402 timer */

  DS_CMD_3GPP_PDN_THROT_IMSI_INFO_AVAIL           = 1159,
  /* Cmd for processing IMSI info in throttle case */

  DS_CMD_3GPP_PDN_THROT_SAMPLING_TIMER_EXP        = 1160,
  /* Cmd for PDN throttle sampling timer expiring */

  DS_CMD_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR   = 1161,
  /* Cmd for timestamping APN throttle timer events */

  DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN           = 1162,
  /* Cmd for unblockng all throttled APN */

  DS_CMD_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP     = 1163,
  /* Cmd for unblock APN timer expiring */

  DS_CMD_3GPP_PDN_THROT_RESET                     = 1164,
  /* Cmd for PDN throttle reset */

  DS_CMD_3GPP_PLMN_BLOCK_TMR_EXP_CMD              = 1165,
  /* Cmd for plmn block timer expiry*/

  DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD               = 1166,
  /* Cmd for efs pdn  throttle timer expiry*/

  DS_CMD_3GPP_EPS_THROT_T3402_TMR_EXP_CMD         = 1167,
  /* Cmd for t3402 timer expiry*/

  DS_CMD_3GPP_RAU_COMPLTE_CMD                     = 1168,
  /* Cmd for rau complete ind from NAS*/

  DS_CMD_3GPP_APN_NAME_CHG_CMD                    = 1169,
  /* Cmd for processing apn name change */

  DS_CMD_3GPP_PHYS_LINK_DOWN                      = 1180,
  /* Cmd ofr phys link down */

  DS_CMD_3GPP_PHYS_LINK_FLOW_ENABLE               = 1181,
  /* Cmd for phys link flow enable */

  DS_CMD_3GPP_PHYS_LINK_UP                        = 1182,
  /* Cmd for phys link up */

  DS_CMD_3GPP_PROFILE_DB_PARAM_CHG            = 1190,
  /* Cmd for profile db APN param change */

  DS_CMD_3GPP_PROFILE_DB_APN_DISABLE              = 1191,
  /* Cmd for profile db APN disable */

  DS_CMD_3GPP_PROFILE_DB_PDP_TYPE_CHANGE          = 1192,
  /* Cmd for to notify profile pdp type change */

  DS_CMD_3GPP_PROFILE_DB_APN_NAME_CHANGE          = 1193,
  /*Cmd for apn name chg*/

  DS_CMD_3GPP_RLC_PURGE_CNF                       = 1200,
  /* Cmd for notifying RLC purge */

  DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED               = 1210,
  /* Cmd for UMTS_RMSM_ADDR_CFG_CHANGED_EV */

  DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE              = 1211,
  /* Cmd for UMTS_RMSM_ADDR_CFG_COMPLETE_EV */

  DS_CMD_3GPP_RMSM_ADDR_CFG_END                   = 1213,
  /* Cmd for UMTS_RMSM_ADDR_CFG_END_EV */

  DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL                  = 1214,
  /* Cmd for UMTS_RMSM_ADDR_CFG_FAILURE_EV */

  DS_CMD_3GPP_RMSM_AT_FLOW                        = 1215,
  /* Cmd for RMSM ATCOP flow events */

  DS_CMD_3GPP_RMSM_AT_SEND_DATA                   = 1216,
  /* Cmd for RMSM forwarding pkts received for Um interface from ATCoP */

  DS_CMD_3GPP_RMSM_AT_UM_IFACE_EV                 = 1217,
  /* Cmd for RMSM um iface event notification */

  DS_CMD_3GPP_RMSM_AUTOCONFIG_SM                  = 1218,
  /* Cmd for autoconfiguring RMSM SM */

  DS_CMD_3GPP_RMSM_DUN_CTRL_RSP                   = 1219,
  /* Cmd for RMSM DUN control response  */

  DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_COMPLETE     = 1220,
  /* Cmd for RMSM entitlement check complete */

  DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_FAIL         = 1221,
  /* Cmd for UMTS_RMSM_ENTITLEMENT_CHECK_FAILED_EV */

  DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER                 = 1222,
  /* Cmd for RMSM IPCP time expiring */

  DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER               = 1223,
  /* Cmd for RMSM IPV6CP timer expiring */

  DS_CMD_3GPP_RMSM_NO_REDIALS_REMAINING           = 1224,
  /* Cmd for UMTS_RMSM_NO_REDIALS_REMAINING_EV */

  DS_CMD_3GPP_RMSM_REDIAL_CHECK_COMPLETE          = 1225,
  /* Cmd for RMSM redial check complete */

  DS_CMD_3GPP_RMSM_RM_BRING_UP_UM_IFACE           = 1226,
  /* Cmd for UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV */

  DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED              = 1227,
  /* Cmd for UMTS_RMSM_RM_PPP_CONFIGURED_EV */

  DS_CMD_3GPP_RMSM_RM_PPP_DOWN                    = 1228,
  /* Cmd for UMTS_RMSM_RM_PPP_DOWN_EV */

  DS_CMD_3GPP_RMSM_RM_PPP_UP                      = 1229,
  /* Cmd for UMTS_RMSM_RM_PPP_UP_EV */

  DS_CMD_3GPP_RMSM_RM_REJECT_MT_PKT_CALL          = 1230,
  /* Cmd for UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV */

  DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL              = 1231,
  /* Cmd for UMTS_RMSM_RM_WANTS_PKT_CALL_EV */

  DS_CMD_3GPP_RMSM_UM_IFACE_DOWN                  = 1232,
  /* Cmd for UMTS_RMSM_UM_IFACE_DOWN_EV */

  DS_CMD_3GPP_RMSM_UM_IFACE_DOWN_ARB              = 1233,
  /* Cmd for UMTS_RMSM_UM_IFACE_DOWN_ARB_EV */

  DS_CMD_3GPP_RMSM_UM_IFACE_UP                    = 1234,
  /* Cmd for UMTS_RMSM_UM_IFACE_UP_EV */

  DS_CMD_3GPP_RMSM_UM_LOCKED                      = 1235,
  /* Cmd for UMTS_RMSM_UM_LOCKED_EV */

  DS_CMD_3GPP_RMSM_UM_PPP_DOWN                    = 1236,
  /* Cmd for UMTS_RMSM_UM_PPP_DOWN_EV */

  DS_CMD_3GPP_RMSM_UM_WANTS_MT_PKT_CALL           = 1237,
  /* Cmd for UMTS_RMSM_UM_WANTS_MT_PKT_CALL_EV */

  DS_CMD_3GPP_RMSM_RM_IPCP_CREQ_RECD = 1238,
  DS_CMD_3GPP_RMSM_RM_IPV6CP_CREQ_RECD = 1239,
  DS_CMD_3GPP_RMSM_PPPRM_EVENT = 1240,
  DS_CMD_3GPP_RMSM_UM_EVENT = 1241,
  DS_CMD_3GPP_RMSM_UM_PHYS_LINK_EVENT = 1242,

  DS_CMD_3GPP_ROAMING_DISALLOWED_FLAG_CHG         = 1250,
  /* Cmd for changing roaming disallowed flag */

  DS_CMD_3GPP_ROAMING_STATUS_CHG                  = 1251,
  /* Cmd for roaming status change */

  DS_CMD_3GPP_RPM_HOURLY_TIMER_EXP                = 1260,
  /* Cmd for RPM hourly timer expiring */

  DS_CMD_3GPP_RPM_LR3_TIMER_EXP                   = 1261,
  /* Cmd for RPM LR3 timer expiring */

  DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_FAIL          = 1262,
  /* Cmd for RPM MMGSDI read param fail */

  DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_CNF           = 1263,
  /* Cmd for RPM MMGSDI read param success */

  DS_CMD_3GPP_RRC_PCH_IND_PROC                    = 1280,
  /* Cmd for RRC PCH indication processing */

  DS_CMD_3GPP_SYS_CHG_SS_INFO                     = 1290,
  /* Cmd for CM SS info system change */

  DS_CMD_3GPP_SYS_SEL_PREF_ERR                    = 1291,
  /* Cmd for notifying CM system selection failed */

  DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR                 = 1292,
  /* Cmd for notifying CM system selection success */

  DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP             = 1293,
  /* Cmd for system selection retry timer expiring */

  DS_CMD_3GPP_TD_CONFIG_UL_DATA_RATE              = 1300,
  /* Cmd for configuring TD UL data rate */

  DS_CMD_3GPP_THROT_TMR_EXP                       = 1310,
  /* Cmd for throttle timer expiring */

  DS_CMD_3GPP_TLB_MODE_B_HANDLER                  = 1320,
  /* Cmd for handling TLB mode B */

  DS_CMD_3GPP_UCSD_APP_ANSWER_CALL                = 1330,
  /* Cmd for UCSD app call answering */

  DS_CMD_3GPP_UCSD_APP_CONFIRM_CALL               = 1331,
  /* Cmd for UCSD app call confirmation */

  DS_CMD_3GPP_UCSD_APP_END_CALL                   = 1332,
  /* Cmd for UCSD app call end */

  DS_CMD_3GPP_UCSD_APP_MODIFY_CALL                = 1333,
  /* Cmd for UCSD app call modify */

  DS_CMD_3GPP_UCSD_APP_ORIG_CALL                  = 1334,
  /* Cmd for originating a UCSD application call */

  DS_CMD_3GPP_UCSD_HANDOVER                       = 1335,
  /* Cmd for UCDS ETSI Inter-RAT handover */

  DS_CMD_3GPP_UCSD_MMGSDI_PROC                    = 1336,
  /* Cmd for UCSD MMGSDI response processing */

  DS_CMD_3GPP_CM_NOTIFY                           = 1340,
  /* Cmd for 3GPP LPM/card refresh processing */

  DS_CMD_3GPP_CALL_END_MODE_CHG_LTE               = 1341,
  /* Cmd for resending call end in LTE */

  DS_CMD_3GPP_MMGSDI_CARD_ERROR                   = 1350,
  /* Cmd for to clear infinite throttle */

  DS_CMD_3GPP_NV_REFRESH                          = 1351,

  DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START         = 1352,
  /* Cmd for PDN Limit throttle start */

  DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP        = 1353,
  /* Cmd to notify PDN limit sampling timer expired */

  DS_CMD_3GPP_MMGSDI_UICC_3G_RESET                = 1355,
  /* Cmd to notify 3GPP MH of UICC/3G Reset  */

  DS_CMD_3GPP_THERMAL_MITIGATION                  = 1356,
  /* Cmd for 3GPP handling of thermal mitigation */

  DS_CMD_3GPP_PDN_APN_PARAM_CHG_TMR_EXP           = 1359,

  DS_CMD_3GPP_DO_SILENT_REDIAL                    = 1360,

  DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_DNS_ADDR     = 1361,
  /* Cmd for PDN teardown in case of INVALID DNS */

  DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF_DNS_ADDR = 1362,
  /* Cmd for PDN teardown in case of INVALID DNS */
 
  DS_CMD_3GPP_PLMN_LIST_CHG                         = 1367,
  /* Cmd for PLMN List change processing */

  /*-------------------------------------------------------------------------
    3GPP2 MH Commands (2000 - 2999)
  -------------------------------------------------------------------------*/
  DS_CMD_707_PKT_EPZID_CHANGE                    = 2000,
  /* EPZID has changed */

  DS_CMD_707_PKT_EPZID_SOCM                      = 2001,
  /* EPZID has new SOCM */

  DS_CMD_707_PKT_EPZID_DELAY_TIMER_EXPIRED       = 2002,
  /* pzid delay timer expired */

  DS_CMD_707_PKT_EPZID_HYSTERESIS_TIMER_EXPIRED  = 2003,
  /* pzid hysteresis timer expired */

  DS_CMD_707_PKT_EPZID_HYS_DATA_READY            = 2004,
  /* to notify epzid hyst engine  */

  DS_CMD_707_PKT_EPZID_HYS_SDB_DATA              = 2005,
  /* about packet data exchange   */

  DS_CMD_707_PKT_MGR_SET_HAT_TIMER               = 2006,
  /* Command to Set Hysteresis Activation Timer */

  /*-------------------------------------------------------------------------
    IS707 Packet Data sub-task Commands
  -------------------------------------------------------------------------*/
  DS_CMD_707_PKT_CTA_TIMER_EXPIRED               = 2007,
  /* AT+CTA (idle timeout) timer */

  DS_CMD_707_PKT_HOLDDOWN_TIMER_EXPIRED          = 2008,
  /* holddown timer expired   */

  DS_CMD_707_PKT_PHYS_LINK_UP                    = 2009,
  /* bring up phys link            */

  DS_CMD_707_PKT_PHYS_LINK_DOWN                  = 2010,
  /* tear down phys link           */

  DS_CMD_707_PKT_IDM_CHANGE                      = 2011,
  /* current network(1X/HDR) change*/

  DS_CMD_707_PKT_SYS_CHG                         = 2012,
  /* to signify change is serving system */

  DS_CMD_707_PKT_AUTO_REDIAL_TIMER_EXPIRED       = 2013,
  /*Auto Redial timer expired*/

  DS_CMD_707_PKT_DELAY_CONNECTION_CLOSE_TIMER    = 2014,
  /* connection release delay timer */

  DS_CMD_707_TOGGLE_QNC_ENABLE                   = 2015,
  /* toggle QNC setting - UI */

  DS_CMD_707_INITIATE_ALN                        = 2016,
  /* Initiate PPP resync due to ALN*/

  DS_CMD_707_ABORT_DORMANT_PPP_TIMER_EXPIRED     = 2017,
  /*close dormant PPP timer expired*/

  DS_CMD_707_EXIT_TC                             = 2018,
  /* cmd when CDMA traffic channel torn down */

  DS_CMD_707_UM_TX_HI_WM                         = 2019,
  /* cmd to process tx high watermark */

  DS_CMD_707_UM_TX_LO_WM                         = 2020,
  /* cmd to process tx low watermark */

  DS_CMD_707_UM_RX_HI_WM                         = 2021,
  /* cmd to process rx high watermark */

  DS_CMD_707_UM_RX_LO_WM                         = 2022,
  /* cmd to process rx low watermark */

  DS_CMD_707_RETRY_DELAY_ORIG                    = 2023,
  /* recvd retry delay from MC */

  DS_CMD_707_HDR_LN_STATUS                       = 2024,
  /* Command to update LN status */

  DS_CMD_707_CM_NOTIFY                           = 2025,
  /* Cmd to process notification from CM */

  DS_CMD_707_NEW_CALL_ATTEMPT_TIMER              = 2026,
  /*new call attempt timer expiration */

  DS_CMD_707_PDN_INACTIVITY_TIMER_CMD            = 2027,
  /*PDN Inactivity timer expiration */

  DS_CMD_RLP_INIT                                = 2028,

  /*-------------------------------------------------------------------------
    New cmds introduced for CAM module.
  -------------------------------------------------------------------------*/
  DS_CMD_707_COLOC_INFO_CHANGED                   = 2029,
  DS_CMD_707_IFACE_IN_USE                         = 2030,
  DS_CMD_707_IFACE_OUT_USE                        = 2031,
  DS_CMD_707_PARSE_DORM_TIMER_SOCM                = 2032,

  /*-------------------------------------------------------------------------
    Cmds used for 3GPP2 iface bringup and teardown
  -------------------------------------------------------------------------*/
  DS_CMD_707_THROTTLE_INFO_UPDATE                 = 2033,
  DS_CMD_707_RMSM_BRING_UP                        = 2034,
  DS_CMD_707_SOCKETS_BRING_UP                     = 2035,
  DS_CMD_707_RMSM_TEAR_DOWN                       = 2036,
  DS_CMD_707_SOCKETS_TEAR_DOWN                    = 2037,

  /*-------------------------------------------------------------------------
    IS707 Packet Data Rm Iface Flow Control Specific commands
  -------------------------------------------------------------------------*/
  DS_CMD_707_RM_IFACE_FLOW_ENABLED                = 2038,
  DS_CMD_707_RM_IFACE_FLOW_DISABLED               = 2039,

  /*-------------------------------------------------------------------------
    RMSM IS707-PKT Commands
  -------------------------------------------------------------------------*/
  DS_CMD_707_RMSM_RM_WANTS_PKT_CALL               = 2040,
  DS_CMD_707_RMSM_RM_IFACE_DOWN                   = 2041,
  DS_CMD_707_RMSM_UM_IFACE_DOWN                   = 2042,
  DS_CMD_707_RMSM_UM_PHYS_LINK_UP                 = 2043,
  DS_CMD_707_RMSM_UM_PPP_DOWN                     = 2044,
  DS_CMD_707_RMSM_RM_PPP_UP                       = 2045,
  DS_CMD_707_RMSM_UM_IFACE_UP                     = 2046,
  DS_CMD_707_RMSM_UM_MIP_IP_ADDR_CHANGED          = 2047,
  DS_CMD_707_RMSM_UM_PHYS_LINK_GONE               = 2048,
  DS_CMD_707_RMSM_E2E_PPP_UP                      = 2049,
  DS_CMD_707_RMSM_UM_RESYNCING                    = 2050,
  DS_CMD_707_RMSM_RM_RESYNCING                    = 2051,
  DS_CMD_707_RMSM_ENTITLEMENT_CHECK_COMPLETE      = 2052,

  DS_CMD_707_MANDATORY_APN_CHECK                  = 2053,
  /* command to request for mandatory list check */

  /*-------------------------------------------------------------------------
    HDR Commands
  -------------------------------------------------------------------------*/
  DS_CMD_HDR_EVENT                                = 2054,
  DS_CMD_HDR_CDMA_SUBS_AVAIL_EVENT                = 2055,
  DS_CMD_HDR_AN_PPP_EV                            = 2056,
  DS_CMD_HDR_AN_PPP_START                         = 2057,

  /*-------------------------------------------------------------------------
    DSSNET4 commands
  -------------------------------------------------------------------------*/
  DS_CMD_DSSNET4_PHY_LINK_UP_EV                   = 2058,
  DS_CMD_DSSNET4_PHY_LINK_GONE_EV                 = 2059,
  DS_CMD_DSSNET4_PHY_LINK_DOWN_EV                 = 2060,
  DS_CMD_DSSNET4_LINK_UP_WITH_SIP_EV              = 2061,
  DS_CMD_DSSNET4_LINK_DOWN_EV                     = 2062,
  DS_CMD_DSSNET4_LINK_RESYNC_EV                   = 2063,
  DS_CMD_DSSNET4_LINK_UP_WITH_MIP_EV              = 2064,
  DS_CMD_DSSNET4_MIP_UP_SUCCESS_EV                = 2065,
  DS_CMD_DSSNET4_MIP_FAILURE_EV                   = 2066,
  DS_CMD_DSSNET4_MIP_DEREGED_EV                   = 2067,
  DS_CMD_DSSNET4_PPP_CLOSE_EV                     = 2068,
  DS_CMD_DSSNET4_FLOW_DELETED_EV                  = 2069,
  DS_CMD_DSSNET4_AUTH_STARTED_EV                  = 2070,

/*-------------------------------------------------------------------------
    DS707 ANY iface related Commands
  -------------------------------------------------------------------------*/
  DS_CMD_DSSNETANY_IFACE_DOWN_EV               = 2082,
  DS_CMD_DSSNETANY_IFACE_UP_EV                 = 2083,
  DS_CMD_DSSNETANY_CONFIGURING_EV              = 2084,
  DS_CMD_DSSNETANY_OPEN_EV                     = 2085,
  DS_CMD_DSSNETANY_TEARDOWN_EV                 = 2086,
  DS_CMD_DSSNETANY_CLOSE_EV                    = 2087,

  /*-------------------------------------------------------------------------
    QOS/NW init commands
  -------------------------------------------------------------------------*/
  DS_CMD_707_QOS_FLOW_MGR_RESV_ON_NOTIFY_CMD        = 2088,
  DS_CMD_707_QOS_FLOW_MGR_RESV_OFF_NOTIFY_CMD       = 2089,
  DS_CMD_707_QOS_FLOW_MGR_RESV_ON_SENT_NOTIFY_CMD   = 2090,
  DS_CMD_707_QOS_FLOW_MGR_CONSOLIDATED_CMD          = 2091,
  DS_CMD_707_SEC_PKT_FLOW_CONFIGURE_CMD             = 2092,
  DS_CMD_707_SEC_PKT_FLOW_ACTIVATE_CMD              = 2093,
  DS_CMD_707_SEC_PKT_FLOW_RESUME_CMD                = 2094,
  DS_CMD_707_SEC_PKT_FLOW_SUSPEND_CMD               = 2095,
  DS_CMD_707_SEC_PKT_FLOW_GO_NULL_CMD               = 2096,
  DS_CMD_707_SEC_PKT_FLOW_MODIFY_CMD                = 2097,
  DS_CMD_DQOS_MGR_PROC_RSVP_MSG_CMD                 = 2098,
  DS_CMD_DQOS_MGR_REXMIT_TIMER_CMD                  = 2099, /* Rexmit timer is expired */
  DS_CMD_707_SEC_NETWORK_INITIATED_PKT_PPP_DOWN_CMD = 2100, /* PPP is released by network */
  DS_CMD_707_SEC_PKT_GRANT_TIMER_EXPIRE_CMD         = 2101, /* QoS Grant timer expire*/
  DS_CMD_707_SEC_PKT_MGR_IFACE_CMD                  = 2102,
  DS_CMD_707_NW_INIT_QOS_PPP_RESYNC_TIMER_EXP_CMD   = 2103,
  DS_CMD_707_NW_INIT_QOS_INACTIVITY_TIMER_EXP_CMD   = 2104,

  /*-------------------------------------------------------------------------
   Following cmds added for CAM module that runs in DS context. Receives
   cmds from DS & CM tasks.  The purpose of this module is to force dormancy
   on 1X to move to EVDO if available, if the call ahs been UP on 1X for a
   while, by reducing  RLP IDLE timeout to 1 sec afetr 5 mnts into the call
  -------------------------------------------------------------------------*/
  DS_CMD_CAM_SO33_CALL_CONNECTED                  = 2105,
  DS_CMD_CAM_TIMER_EXPIRED                        = 2106,
  DS_CMD_CAM_CALL_GONE_NULL                       = 2107,
  DS_CMD_CAM_CALL_DORMANT                         = 2108,
  DS_CMD_CAM_EVDO_COLOCATED                       = 2109,
  DS_CMD_CAM_NO_EVDO_COLOCATED                    = 2110,
  DS_CMD_CAM_FLOW_CTRL_TIMER_EXPIRED              = 2111,
  DS_CMD_CAM_NON_SO33_CALL_CONNECTED              = 2112,
  DS_CMD_707_MPIT_TIMEOUT                         = 2113,   /* Max PPP Inactivity timer expiration */ 
  DS_CMD_707_LSD_TIMEOUT                          = 2114,   /* Link Status Determination (LSD) expiry */
  DS_CMD_707_PS_EVT_PROC                          = 2115,   /* To process all PS events */
  DS_CMD_707_ROAM_CHG                             = 2116,   /* to signify change is roaming status */
  DS_CMD_707_POST_GRACEFUL_PPP_TERM               = 2117,  /*Terminate all active PDNs and LCP*/
  DS_CMD_707_S101_COMPLETE_PREREG_HANDOFF         = 2118,    /* Prereg Handoff handling*/
  DS_CMD_707_DEVMGR_PCMT_TIMEOUT                  = 2119,
  DS_CMD_707_DEVMGR_PPP_EV                        = 2120,
  DS_CMD_707_WAIT_FOR_EMERG_CALL_TIMEOUT          = 2121,


  /*-------------------------------------------------------------------------
   Following cmds added for RDUD module that runs in DS context.
   The purpose of this module is to enforce quick idle_timeout dormancy
   if TC came up due to an incoming call, to avoid keepign TC alive
   unncessarily long.
  -------------------------------------------------------------------------*/
  DS_CMD_RDUD_INC_CALL_ACCEPTED                   = 2122,
  DS_CMD_RDUD_PHYS_LINK_DOWN                      = 2123,
  DS_CMD_RDUD_TIMER_EXPIRED                       = 2124,
 
#ifdef FEATURE_DATA_IS707_ASYNC
  /*-------------------------------------------------------------------------
    IS707 Async Data sub-task Commands
  -------------------------------------------------------------------------*/
  DS_CMD_707_ASYNC_IFACE_BRING_UP                 = 2125,
  DS_CMD_707_ASYNC_IFACE_TEAR_DOWN                = 2126,
  DS_CMD_707_ASYNC_PHYS_LINK_TEAR_DOWN            = 2127,
  DS_CMD_707_ASYNC_PTCL_OPENING_TIMER_EXPIRED     = 2128,
  DS_CMD_707_ASYNC_PTCL_OPENED                    = 2129,
  DS_CMD_707_ASYNC_PTCL_CLOSED                    = 2130,
  DS_CMD_707_ASYNC_ATZ                            = 2131,
#endif /* FEATURE_DATA_IS707_ASYNC */

  /*-------------------------------------------------------------------------
    RVSP for Qos in 3GPP2
  -------------------------------------------------------------------------*/
  DS_CMD_RSVP_SOCKET_EVENT                        = 2132,

  /*-------------------------------------------------------------------------
    Cmd to process notification from CM 
  -------------------------------------------------------------------------*/
  DS_CMD_707_MCFG_NOTIFY                          = 2133,

  /*-------------------------------------------------------------------------
    Cmd to process notification from DS3G for Thermal mitigation 
  -------------------------------------------------------------------------*/
  DS_CMD_3GPP2_TM_NOTIFY                          = 2134,

  /*-------------------------------------------------------------------------
    SRLTE Enabled . 
  -------------------------------------------------------------------------*/
  DS_CMD_CAM_SRLTE_ENABLED                         = 2135,

  /*-------------------------------------------------------------------------
    Cmd to process notification from DS profile  for MIN APN updation 
  -------------------------------------------------------------------------*/
  DS_CMD_707_MANDATORY_APN_UPDATE                  = 2136,

  /*-------------------------------------------------------------------------
    WLAN MH Commands (3000 - 3499)
  -------------------------------------------------------------------------*/
  DS_CMD_WLAN_PROXY_IFACE_BRING_UP                   = 3000,
  DS_CMD_WLAN_PROXY_IFACE_TEAR_DOWN                  = 3001,
  DS_CMD_WLAN_PROXY_PROC_DSD_IND                     = 3002,
  DS_CMD_WLAN_PROXY_REV_IP_XPORT_SIO_CONFIG_COMPLETE = 3003,
  DS_CMD_WLAN_PROXY_REV_IP_XPORT_WDS_CONFIG_COMPLETE = 3004,
  DS_WLAN_PROXY_PROC_BRINGUP_RETRY_TIMER_EXPIRY_CMD  = 3005,
  DS_CMD_WLAN_PROXY_RM_DOWN                          = 3006,
  DS_CMD_WLAN_PROXY_WLAN_AVAILABLE                   = 3007,

  /*-------------------------------------------------------------------------
    WLAN Utils Commands (3500 - 3999)
  -------------------------------------------------------------------------*/
  DS_CMD_WLAN_UTILS_DS3G_EVENT_MGR                   = 3500,
  DS_CMD_WLAN_THERMAL_MITIGATION_EV                  = 3501,

  /*-------------------------------------------------------------------------
    EPC Commands (4000 - 4999)
  -------------------------------------------------------------------------*/
  DS_CMD_EPC_LTE_ATTACH_INIT_PROC              = 4000,
  DS_CMD_EPC_SS_BEARER_TECH_CHG_PROC           = 4001,
  DS_CMD_EPC_PREF_SYS_CHG                      = 4002,
  DS_CMD_EPC_eHRPD_S101_STATUS_CHG_PROC        = 4003,
  DS_CMD_EPC_eHRPD_PREREG_COMPLETE_RSP         = 4004,
  DS_CMD_EPC_IFACE_EV_PROC                     = 4005,
  DS_CMD_EPC_IFACE_TEARDOWN                    = 4006,
  DS_CMD_EPC_THRTL_PDN_LIST_PROC               = 4007,
  DS_CMD_EPC_REPOST_SYS                        = 4008,
  DS_CMD_EPC_CB_INIT                           = 4009,
  DS_CMD_EPC_SRAT_TIMER_EXP_PROC               = 4010,
  DS_CMD_EPC_RETRY_TIMER_EXP_PROC              = 4011,
  DS_CMD_EPC_HO_GIVEUP_TIMER_EXP_PROC          = 4012,
  DS_CMD_EPC_HO_THROTTLE_TIMER_EXP_PROC        = 4013,

  /*-------------------------------------------------------------------------
    MPPM Commands (5000 - 5999)
  -------------------------------------------------------------------------*/
  DS_CMD_MPPM_DSD_EVENT                        = 5000,
  /*DSD event processing command*/

  DS_CMD_MPPM_FLUSH_EXT_EVENT_Q                = 5001,
  /*CMD to flush external events queue*/

  DS_CMD_MPPM_WLAN_LB_PREF_CHANGE              = 5002,
  /*Handle WLAN LB preference change*/

  DS_CMD_MPPM_APN_PREF_SYS_CHANGE              = 5003,
  /*Handle APN system preference change*/

  DS_CMD_MPPM_PDN_THROTTLE_INFO                = 5004,
  /*Handle PDN throttle info cmd*/

  DS_CMD_MPPM_DSDIF_LTE_ATTACH_INIT_STATUS     = 5005,
  /*Handle LTE init status cmd*/

  DS_CMD_MPPM_WLAN_MAX_CONN_INFO               = 5006,
  /*Handle WLAN MAX_CONN info cmd*/

  DS_CMD_MPPM_PDN_DB_UPDATE                    = 5007,
  /*Handle MPPM PDN DB updates cmd*/

  DS_CMD_MPPM_PDN_STATUS_INFO                  = 5008,
  /*Handle MPPM PDN status info cmd*/

  DS_CMD_MPPM_EPC_TIMER_CB                     = 5009,
  /* EPC RAT timer callback cmd*/

  DS_CMD_MPPM_MMGSDI_SUBSCRIPTION_EVENT        = 5010,
  /* MMGSDI SUBS process cmd*/

  DS_CMD_MPPM_T_MINONWWAN_TIMER_CB             = 5011,
  /* MINONWWAN timer callback cmd*/

  DS_CMD_MPPM_NULL_APN_RESOLVED_INFO           = 5012,
 /* DS_CMD_MPPM_NULL_APN_RESOLVED_INFO callback cmd */

  DS_CMD_MPPM_NV_REFRESH_EVENT_CB              = 5013,
  /* DS3GEVENTMGR_NV_REFRESH_EV Event callback cmd*/

  DS_CMD_MPPM_EMERGENCY_PROFILE_UPDATE_EVENT_CB = 5014,
  /* DS Profile update Event callback cmd*/

  DS_CMD_MPPM_ANDSF_EVENT_CB                   = 5015,
  /* Andsf event cmd*/

  DS_CMD_MPPM_WIFI_SETTINGS_CHANGE               = 5016,
 /* DS_CMD_MPPM_RAT_PREFERENCE callback cmd */
  
  DS_CMD_MPPM_WLAN_IDI_TYPE_EVENT                = 5017,
  /* DS3GEVENTMGR_WLAN_IDI_TYPE_EV Event callback cmd*/

  DS_CMD_MPPM_IMSI_INFO_EVENT                    = 5018,
  /* DS3GEVENTMGR_MMGSDI_IMSI_INFO_EV Event callback cmd*/

  DS_CMD_MPPM_IMPI_INFO_EVENT                    = 5019,
  /* DS3GEVENTMGR_MMGSDI_IMPI_INFO_EV Event callback cmd*/

  DS_CMD_MPPM_ROAMING_STATUS_EVENT              = 5020,
  /* DS3GEVENTMGR_ROAMING_STATUS_EV Event callback cmd*/

  DS_CMD_MPPM_PROFILE_UPDATE_EVENT_CB           = 5021,
  /* DS Profile update Event callback cmd*/

 /*================ AT COMMAND PROCESSOR (values 6000 - 6999) ================*/

   /*-------------------------------------------------------------------------
     ATCoP asynchronous events
   -------------------------------------------------------------------------*/
   DS_CMD_ATCOP_ASYNC_EVENT_START               = 6000,
   /* Start of ATCoP asynchronous events */

 /*-------------------------------------------------------------------------
     CDMA and ETSI SMS AT asynchronous events
   -------------------------------------------------------------------------*/

   DS_CMD_ATCOP_SMS_ERR_CMD                     = 6001,
   /* Error Event from UASMS */

   DS_CMD_ATCOP_SMS_ABT_CMD                     = 6002,
   /* Event to tell ATCOP to abort cmd */

   DS_CMD_ATCOP_TIMER_EXPIRED_CMD               = 6003,
   /* a ATCOP timer expire  */

   DS_CMD_ATCOP_CM_CALL_CMD                     = 6004,
   /* Call feedback from CM  */

   DS_CMD_ATCOP_CM_CALL_INFO_CMD                = 6005,
   /* Call information from CM  */

   DS_CMD_ATCOP_CM_SS_CMD                       = 6006,
   /* Serving System feedback from CM  */

   DS_CMD_ATCOP_CM_SS_INFO_CMD                  = 6007,
    /* Serving System information from CM  */

   DS_CMD_ATCOP_CM_PH_CMD                       = 6008,
   /* Phone command feedback from CM */

   DS_CMD_ATCOP_CM_PH_INFO_CMD                  = 6009,
   /* Phone information from CM   */

   DS_CMD_ATCOP_CM_INBAND_CMD                   = 6010,
   /* Inband cmd from CM */

   DS_CMD_ATCOP_CM_INBAND_INFO_CMD              = 6011,
   /* Inband event from CM  */
   
   DS_CMD_ATCOP_GSDI_SIM_INFO                   = 6012,
    /* SIM read/write response from GSDI */

   /*-------------------------------------------------------------------------
     WCDMA and GSM asynchronous AT events/commands
   -------------------------------------------------------------------------*/
    DS_CMD_ATCOP_STATUS_CMD                      = 6013,
    /* used to report the excution status of an */
    /* async func call.  The callback function of  */
   /* the  async command submit this cmd. */

    DS_CMD_ATCOP_CM_SUPS_CMD                     = 6014,
    /* Supplemental Services status from CM   */

    DS_CMD_ATCOP_CM_SUPS_INFO_CMD                = 6015,
   /* Supplemental Services response from CM */

    DS_CMD_ATCOP_MMGSDI_APDU_RESP                = 6016,
   /* Response APDU sent from GSDI */

   /*-------------------------------------------------------------------------
     GSM FAX asynchronous AT events/commands
   -------------------------------------------------------------------------*/
   DS_CMD_ATCOP_FPS_T31_CMD_STATUS               = 6017,
   /* GSM Fax command status  */

   DS_CMD_ATCOP_PBM_CB_CMD                       = 6018,
    /* Pbm async function's call back
       function submits this command to send
       the results to ATCOP */

    DS_CMD_ATCOP_TO_SIO_LOW_CMD                  = 6019,
   /* ds-to-sio watermark hits low */

   DS_CMD_ATCOP_SEND_AT_CMD                      = 6020,

    DS_CMD_ATCOP_FWD_CLIENT_REG                  = 6021,
    DS_CMD_ATCOP_FWD_CLIENT_DEREG                = 6022,
    DS_CMD_ATCOP_FWD_AT_CMD_REG                  = 6023,
    DS_CMD_ATCOP_EXT_CMD_RESP                    = 6024,
    DS_CMD_ATCOP_RESET_REQ_CMD                   = 6025,
    DS_CMD_ATCOP_SHUTDOWN_REQ_CMD                = 6026,
    DS_CMD_ATCOP_EXT_CMD_URC                     = 6027,

   DS_CMD_ATCOP_MMGSDI_INFO_CMD                  = 6028,
   /* Response info sent from MMGSDI */

   DS_CMD_ATCOP_MMGSDI_SIM_INFO                  = 6029,
   /* SIM read/write response from MMGSDI */

   DS_CMD_ATCOP_COPS_AT_CMD                      = 6030,
   DS_CMD_ATCOP_CPOL_AT_CMD                      = 6031,

   DS_CMD_ATCOP_MMGSDI_AUTH_CMD                  = 6032,
   /* MMGSDI authentication commands result */

   DS_CMD_ATCOP_MD5_ABT_CMD                      = 6033,
   /* Event to tell ATCoP to abort MD5 command */

    DS_CMD_ATCOP_PDP_CMD                         = 6034,
   /* AT PDP cmd */

   DS_CMD_ATCOP_FTM_DIAGPKT_RSP_CMD              = 6035,
   /* Response sent from FTM diag request. */

   DS_CMD_ATCOP_ECALL_CMD                        = 6036,
   DS_CMD_ATCOP_PD_INFO_CMD                      = 6037,
   /* GPS information from PDSM */

   DS_CMD_ATCOP_MMGSDI_INIT_INFO                 = 6038,
   /* Response info sent from MMGSDI */

   DS_CMD_ATCOP_MMGSDI_OPER_NAME_INFO            = 6039,

    DS_CMD_ATCOP_RMNET_EV_CMD                    = 6040,
   /* Rmnet related Events */

   DS_CMD_ATCOP_EAP_SIM_AKA_TASK_SWITCH_CMD      = 6041,
   DS_CMD_ATCOP_EAP_SIM_AKA_RESULT_IND_CMD       = 6042,
   DS_CMD_ATCOP_EAP_SIM_AKA_SUPP_RESULT_IND_CMD  = 6043,
   DS_CMD_ATCOP_EAP_SIM_AKA_TRP_TX_CMD           = 6044,
   DS_CMD_ATCOP_MUX_NOTIFY_CMD                   = 6045,
   DS_CMD_ATCOP_RDM_NOTIFY_CMD                   = 6046,
   DS_CMD_ATCOP_VOIPM_AT_CMD                     = 6047,
   DS_CMD_ATCOP_MMGSDI_CRSM_RESP                 = 6048,
   DS_CMD_ATCOP_FWD_AT_CMD_DEREG                 = 6049,
   DS_CMD_ATCOP_ASYNC_EVENT_END                  = 6050,
   /* End of ATCoP asynchronous events */

  /*-------------------------------------------------------------------------
    DS IWLAN S2B commands (7000-7999)
  -------------------------------------------------------------------------*/
  DS_CMD_IWLAN_S2B_PDN_BRING_UP                    = 7000,
  DS_CMD_IWLAN_S2B_PDN_TEARDOWN                    = 7001,
  DS_CMD_IWLAN_S2B_WP_IFACE_UP                     = 7002,
  DS_CMD_IWLAN_S2B_WP_IFACE_DOWN                   = 7003,
  DS_CMD_IWLAN_S2B_REV_IP_XPORT_UP                 = 7004,
  DS_CMD_IWLAN_S2B_REV_RM_CONFIG_COMPLETE          = 7005,
  DS_CMD_IWLAN_S2B_RM_DOWN                         = 7006,
  DS_CMD_IWLAN_S2B_IKEV2_EPDG_ADDR_CHANGED         = 7007,
  DS_CMD_IWLAN_S2B_IKEV2_DNS_CALLBACK              = 7008,
  DS_CMD_IWLAN_S2B_IKEV2_INITIATE_ESP_REKEY        = 7009,
  DS_CMD_IWLAN_S2B_IKEV2_PROCESS_IKEV2_CB          = 7010,
  DS_CMD_IWLAN_S2B_SUB_NOT_AVAIL_EV                = 7011,
  DS_CMD_IWLAN_S2B_EPDG_ADDR_RESLVR_DNS_CALLBACK   = 7012,
  DS_CMD_IWLAN_S2B_EPDG_ADDR_RESLVR_IFACE_UP       = 7013,
  DS_CMD_IWLAN_S2B_EPDG_ADDR_RESLVR_IFACE_DOWN     = 7014,
  DS_CMD_IWLAN_S2B_EPDG_ADDR_RESLVR_TIMER_CALLBACK = 7015,
  DS_CMD_IWLAN_S2B_IFACE_IPSEC_IPFLTR_CHANGED      = 7017,
  DS_CMD_IWLAN_S2B_WP_FLOW_CHANGED                 = 7018,
  DS_CMD_IWLAN_S2B_PLMN_CHANGED                    = 7019,
  DS_CMD_IWLAN_S2B_WP_INFO_CHANGED                 = 7020,
  DS_CMD_IWLAN_S2B_LPM_MODE                        = 7021,
  DS_CMD_IWLAN_S2B_WIFI_OFF                        = 7022,
  DS_CMD_IWLAN_S2B_PDN_KEEP_ALIVE_GUARD_TIMER_CB   = 7023,
  DS_CMD_IWLAN_S2B_IKEV2_PROCESS_NAT_KA_STATUS     = 7024,
  DS_CMD_IWLAN_S2B_NAT_KA_OFFLOAD_GUARD_TIMER_CB   = 7025,
  DS_CMD_IWLAN_S2B_EPDG_ADDR_RESLVR_TTL_TIMER_CALLBACK = 7026,
  DS_CMD_IWLAN_S2B_PDN_HO_GUARD_TIMER_CB           = 7027,
  DS_CMD_IWLAN_S2B_SUBS_NA_PDN_KA_GUARD_TIMER_CB   = 7028,
  DS_CMD_IWLAN_S2B_PDN_STATUS_CHANGED              = 7029,
  DS_CMD_IWLAN_S2B_IMPI_INFO_CB                    = 7030,

  /*-------------------------------------------------------------------------
    DS Application Service commands (8000-8999)
  -------------------------------------------------------------------------*/
  DS_APPSRV_CMD = 8000,


  DS_MAX_CMD

} ds_cmd_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  DS Modules enum type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum 
{
  DS_MODULE_INVALID      = -1,
  DS_MODULE_DSMGR        = 0,
  DS_MODULE_3GPP_MH      = 1,
  DS_MODULE_3GPP2_MH     = 2,
  DS_MODULE_WLAN_MH      = 3,
  DS_MODULE_EPC          = 4,
  DS_MODULE_MPPM         = 5,
  DS_MODULE_ATCOP        = 6,
  DS_MODULE_IWLAN_S2B_MH = 7,
  DS_MODULE_APPSRV       = 8,
  DS_MODULE_MAX
} ds_module_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Header type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  q_link_type       link;         /* Queue link type */
  ds_cmd_enum_type  cmd_id;       /* Identifies the command */
#ifdef FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG
  time_type         timestamp;    /* Timestamp of the command
                                     measured in ms after phone is powered on*/
#endif /*FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG*/
} ds_cmd_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command type: The command header identifies the command. If the command has
  any parameters, they are specified in the union.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  ds_cmd_hdr_type   hdr;               /* Command header */
  void             *cmd_payload_ptr;   /* Command Payload */
} ds_cmd_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command processing callback function type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef void (*ds_cmd_process_func_type)( ds_cmd_type *cmd_ptr );

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_CMD_PROCESS_REGISTER

DESCRIPTION   This function registers command processing function for each 
              module.

DEPENDENCIES  None

RETURN VALUE  TRUE: callback registration successful
              TRUE: callback registration Un-successful

SIDE EFFECTS  None
===========================================================================*/
boolean ds_cmd_process_register
(
  ds_module_enum_type       module_id,
  ds_cmd_process_func_type  cb_func_ptr
);

/*===========================================================================
FUNCTION      DS_ALLOCATE_CMD_BUF

DESCRIPTION   This function allocates a command buffer from the Data Services 
              Task. The caller of this function must check for a NULL return
              value, and either perform some error recovery or ERR_FATAL,
              as appropriate.

DEPENDENCIES  None

RETURN VALUE  Returns a pointer to a command buffer, if available, or
              NULL if no command buffers are available.

SIDE EFFECTS  None
===========================================================================*/
ds_cmd_type* ds_allocate_cmd_buf
(
  size_t payload_size  /* number of bytes to allocate for cmd payload */
);

/*===========================================================================
FUNCTION      DS_RELEASE_CMD_BUF

DESCRIPTION   This function returns a command buffer to the Data Services 
              Task's free queue.

DEPENDENCIES  The caller of this function should have previously allocated 
              a command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds_release_cmd_buf
(
  ds_cmd_type  **cmd_pptr           /* Pointer to command to be released */
);

/*===========================================================================
FUNCTION      DS_GET_CMD_QUEUE_CNT

DESCRIPTION   This function returns the count of outstanding commands that 
              are in DS command queue.

DEPENDENCIES  None

RETURN VALUE  Value of type uint8 - Indicates number of outstanding commands 
              in DS command queue.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_cmd_queue_cnt( void );

/*===========================================================================
FUNCTION      DS_GET_CUR_FREE_CMD_CNT

DESCRIPTION   This function returns the current count of free command
              buffers that are available in the DS task.

DEPENDENCIES  None

RETURN VALUE  Returns a uint8 with the number of outstanding commands 
              in the dstask.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_cur_free_cmd_cnt( void );

/*===========================================================================
FUNCTION      DS_PUT_CMD_EXT2

DESCRIPTION   This function puts a command on the Data Services Task's 
              command queue, and sets the DS_CMD_Q_SIG. The command is put
              on the regular external command queue regardless of the calling
              task context, to preserve legacy behavior.

DEPENDENCIES  The caller of this function should have previously allocated 
              a command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_put_cmd_ext2
(
  ds_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
);

/*===========================================================================
FUNCTION      DS_PUT_CMD

DESCRIPTION   This function puts a command on the Data Services Task's 
              command queue, and sets the DS_CMD_Q_SIG. The command is put
              on the regular external command queue regardless of the calling
              task context, to preserve legacy behavior.

DEPENDENCIES  The caller of this function should have previously allocated a
              command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_put_cmd
(
  ds_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
);

/*===========================================================================
FUNCTION      DS_PUT_CMD_EXT

DESCRIPTION   This function puts a command on the Data Services Task's 
              command queue, and sets the DS_CMD_Q_SIG. If the calling task
              is DS task itself, then command is put on the internal
              (i.e. high priority) command queue, otherwise the command is
              put on the regular external command queue.

DEPENDENCIES  The caller of this function should have previously allocated 
              a command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_put_cmd_ext
(
  ds_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
);

/*===========================================================================

FUNCTION DS_PUT_CMD_INT

DESCRIPTION
  This function puts a command on the Data Services Task's internal command queue,
  and sets the DS_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling ds_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds_put_cmd_int
(
  ds_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
);

#endif /* DSTASK_V_H */
