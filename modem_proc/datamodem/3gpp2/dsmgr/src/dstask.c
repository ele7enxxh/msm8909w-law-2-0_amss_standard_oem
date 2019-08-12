/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D A T A   S E R V I C E S   T A S K

GENERAL DESCRIPTION
  This software unit contains the task entry point and the top-level dispatch
  routines for the Data Services (DS) Task. Functions to manipulate the Data
  Services Task's command queue are also included here.

EXTERNALIZED FUNCTIONS
  ds_task()
    Data Services Task entry point

  ds_get_cmd_buf()
    Returns a command buffer

  ds_put_cmd()
    Puts a command on the Data Services Task command queue

EXTERNALIZED FUNCTIONS INTERNAL TO DS SUBSYSTEM

  ds_release_cmd_buf()
    Releases a command buffer

  ds_get_cur_free_cmd_cnt()
    Returns the number of free command buffers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The DS Task should have been created before any calls to ds_get_cmd_buf()
  or ds_put_cmd().


  Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/dstask.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/14   ab      Added an API to put a cmd in the internal DS queue
03/15/14   scb     Added support for MPPM MMGSDI SUBSCRIPTION INFO
02/25/14   scb     Added support for MPPM EPC timers.
11/21/13   scb     Add support for WLAN_MAX_CONN reached indication in MPPM
11/06/13   scb     Add support for PDN Database updates in MPPM.
09/23/13   op      Added a command to support ePDG address changes
09/23/13   scb     Added support for throttling in MPPM.
09/16/13   scb     Add support for LTE_ATTACH_INIT status indication in MPPM
07/21/13   tk      Fixed DS_AT_SHUTDOWN_REQ_CMD processing.
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/25/13   sd      Boundary check for 707-mip profile variables.
05/24/13   sd      TSTS changes for DS3G.
05/02/13   vs      NV item to check call origination is allowed before PS attach
03/02/13   pkp     Dog heart beat implementation.
02/22/13   op      Added support for the IWLAN_S2B module in DS task
01/25/13   sd      Mainlining Modem Statistics Feature
11/27/12   ss      Fixed the issue of RMSM AT commands not handled in LTE
                   off configuration.
11/08/12   ssb     Dun Call Entitlement support using PS_SYS Interface
10/29/12   sd      Fixed compiler warnings.
10/23/12   scb     Added support for the MPPM module in DS task.
08/23/12   jz      Gracefully tear down PPP upon receiving PDN ID in use
08/13/12   ss      Adding items to data config EFS file.
06/19/12   nd      MT PDP pkt call.
07/13/12   jz      Support for emergency only mode when mandatory APN is disabled
07/31/12   sk      C-EONS Feature Support.
07/12/12   jee     To handle LPM/PWROFF & card refresh notifications from CM
05/17/12   jee     Mainlined feature FEATURE_DATA_FUSION_PS_ARBITRATION_MGR
05/18/12   msh     Move RCINIT handshake out of rcinit phase 2 feature
04/19/12   sb      Feature Cleanup: Always On Flag FEATURE_UIM_SUPPORT_3GPD
03/21/12   msh     TCB changes due to Core changes 
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
01/19/12   ss      Fixed issue in initializing entitlement cb function.
01/16/12   ack     Fix to post a cmd for iface bringup/teardown
01/02/12   msh     Coan: Feature cleanup
01/03/12   jee     Fix to set PCMT value to iRAT when LTE attach happens
12/26/11   ss      Added support for new command to process IPCP delayed NAK.
10/01/11   bvd     RCINIT : Merged all Task befor tmc_start_task into one 
                   function
11/02/11   msh     Added DS_707_PARSE_DORM_TIMER_SOCM_CMD
10/31/11   ssh     eHRPD Always On support
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
09/05/11   ss      Changes to support RPM.
09/08/11   ash     Added support for eHRPD to HRPD fallback.
07/08/11   dvk     Fixed compiler warning
06/06/11   ash     Added support for PPP Partial Context
06/20/11   ttv     Merged silent redial support changes for UMTS tethered calls.
06/16/11   ars     Moved dsgen crit sect init to powerup init
06/15/11   asn     Allow UMTS PDP Registry init for testframework 
05/09/11   ars     SU API compliance changes
05/09/11   asn     Adding support for remoting UMTS PDP Registry
04/08/11   dvk     Global Variable Cleanup
03/24/11   sa      Added MT PDP Auto answer support.
03/23/11   sn      Globals cleanup for free floating of tasks.
03/21/11   ack     Init Crit Sect Variable before usage
03/02/11   ss      Added support for configurable RAB_REESTAB RETRY.
02/25/11   ad      Fixed Crash in CDMA subscripton while NV read. 
02/02/11   ack     Added DS_RLP_INIT_CMD
02/04/11   mg      Added DS_707_PWROFF_LPM_MODE_CMD to process pwroff/lpm 
                   mode notification from CM
01/17/11   ss      Added support for Iface linger.
01/14/11   ack     Added PHY_LINK_DOWN_EV reg/dereg to dssnet4/6 SM
01/11/11   ms      Handle RSVP Socket Write/Read event commands.
12/15/10   ms      Handle Iface related events by posting command to DS task.
12/07/10   ad      Added Dual IMEI support for DSDS.
11/25/10   ss      Fixed memory leak of unhandled DS cmds. 
11/22/10   ttv     Added support for DSDS.
11/19/10   ars     Changed proc of setupBridgeInterfaceEvent to command based
10/14/10   sa      Adding debug messages before ASSERT.
10/23/10   pp      ds_send_cmd[_ext]APIs are introduced for sending commands 
                   to DS task. Cmd payloads are de-coupled from Task buffer.
10/12/10   ss      DYNAMIC WDOG support for DATA tasks.
09/30/10   ttv     Merged the fixes of compiler warnings.
09/09/10   op      Migrated to MSG 2.0 macros
09/07/10   op      IPv6 enable/disable feature for HRPD
08/16/10   rp      Made use of ds_task_ready variable for TEST_FRAMEWORK
07/09/10   asn     Support for Arbitration - round II
07/18/10   mg      Function call to read EFS config file for PDN
                   throttling feature for eHRPD
07/14/10   asn     Address initialization of DS Profile and UMTS Profile Reg
07/09/10   asn     Support for Arbitration
07/08/10   ad      Added support for $ECALL command.
07/08/10   ssh     Posting command to DS on PDN Inactivity timer expiry
07/07/10   ms      Data statistics support for Field Test Display.
06/25/10   vs      Support for Carrier Detect Signals for Fusion. Also
                   initialization of Sm Manager and RmSm proxy.
06/17/10   as      Fusion Additions
06/04/10   bs      MMGSDI SESSION Migration.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
05/10/10   kk      Mainlining pending GOBI changes.
04/21/10   sa      Added support for Loopback Mode B.
04/14/10   sn      Mainlined GOBI changes.
03/16/10   sy      Changed task_* api to tmc_task* api.
03/15/10   ls      Silent Redial Feature
02/04/10   asn     Process DSM Mem events in DS Task context
01/06/10   bs      Added +CGACT command support for LTE.
01/05/10   sn      Moved debug message which prints processed command info to
                   print before command buffer is freed.
11/16/09   ca      Added support for MMGSDI Auth commands.
12/15/09   nc      Featurisation changes for LTE.
12/12/09   sa      Added support for ATCoP - Mode handler interface.
11/22/09   vk      Added support for internal DS command queue
09/30/09   sn      Fixed klocwork errors.
09/22/09   vrk     Merged LTE changes.
08/22/08   ua      Added support for N/w initated Qos.
06/04/09   ls      Support CSIM feature
07/02/09   ss      WLAN decoupling from DS task.
06/19/09   ls      Support operation mode change
06/12/09   pp      DS Task de-coupling: Removed PS logical mode handler
                   references.
05/18/09   ls      Remove included header file for MFLO team
05/05/09   ss      Fixed Compiler Warnings.
04/30/09   ls      Support LPM mode change
04/29/09   ms      Fixed Lint High Errors.
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
04/04/09   ms      Added code to handle the pref_mode_change_event from CM
                   and to tear down the call (dormant and active both)
12/29/08   psng    Merged support for +VTS Command.
03/13/09   ls      Merge the changes from MFLO
03/04/09   sa      AU level CMI modifications.
02/25/09   sn      Fixed off-targer lint errors.
12/29/08   nc      Added support for +VTS Command
12/17/08   ms      Added code to handle the pref_mode_change_event from CM
                   and to tear down the call (dormant and active both)
12/10/08   sk      Start the ds task before calling rmnet_sm_init().
11/21/08   sa      Added synchronization of +CRSM command with MMGSDI.
10/23/08   bs      Added support for 1X AT Phonebook commands.
10/18/08   pp      Added support for SLIP interface.
09/30/08   yz      Increased DS command buffer size to 32 for UMTS/GSM,
                   added timestamp info in DS command.
06/23/08   ar      Added QMI proxy iface mode handler support.
09/03/08   ms      Feature enhancement for BCMCS 2.0 Phase2
08/05/08   ms      IS707B addendum Feature support.
07/09/08   bs      Added support for NAARefresh.
07/16/08   ua      Added support for external client support.
07/08/08   ua      Added support for DSUMTSPS_PDP_IP_IRAT_TX_UM_CMD
07/10/08   ssh     UICC support.
07/08/08   sn      Fixed compiler warnings.
06/13/08   ua      Correcting reading of data from RUIM/NV items
                   for OMH 2.
04/15/08   ms      Feature BCMCS2.0 Enhancements
02/20/08   vd      MBMS Support.
02/14/08   vk      RDM Open command aggregation.
02/14/08   vk      Graceful termination of PDP-PPP calls
02/04/08   ssh     Changes to MIPv6 commands
01/29/08   asn     Serialize processing of FLOW IOCTL - QOS_RELEASE
                   by moving it to DS Task context
01/11/08   vk      Changes to propagate HSPA call status
01/10/08   spn     Added Commands for Memory Based Flow Control
11/30/07   asn     Added support for new feature MT-PDP terminating in TE
11/20/07   vp      Added support for MIPv6
11/15/07   sc      Added support to send PPP auth starting event to IFACE
09/07/07   rt      Added support for IWLAN.
08/16/07   as      Added support for DS_707_PKT_SYS_CHG_CMD
08/14/07   vk      Increased number of command buffers for UMTS Data on Apps
06/11/07   as      Added support for DS_HDR_AN_PPP_START_CMD
05/25/07 squ/mga  Added case DS_707_HDR_LN_STATUS_CMD in dsi_process_cmds
05/23/07   vp      MIP CCoA related Fixes.
05/01/07   sk      QoS Modify support
04/20/07   vk      Processing GSNDCP reg cnf cbs in DS context
02/04/06   sr      Added support for STA UMTS mode handler commands
02/14/07   vd      Lint error fixes.
01/10/07   sq      Added support of DS_707_RETRY_DELAY_ORIG_CMD
12/07/06   as      Added support for DEREGED cmd used for MIP deregistration
11/16/06   sq      Add support of DS_CM_PH_SYS_SEL_PREF_NORMAL_CMD
11/06/06   vk      Changes for UMTS data on apps support.
11/09/06   vrk     Post command to DS to process tx high and low wmk
10/31/06   spn     Changes to consolidate a burst of commands in flow mgr
09/06/06   rsl     Added 2 new commands PHYS_LINK_FLOW_ENABLE and IFACE_FLOW_ENABLE.
08/23/06   rsl     Masking out signals that were not requested.
08/23/06   ks      Changes in qmi/rmnet initialization.
08/10/06   vas     Fixed merge error. Re-added WLAN QoS command processing
08/04/06   TMR     Fixed ds3g_siolib_init sequencing problem introduced with
                   FEATURE_DATA_ON_APPS builds
07/06/06   sk      QoS 2.0 Support
03/13/06   spn     Removed unused code.
01/08/06   TMR     Moved ds3g_silib_int() call to task startup.
11/22/05   aku     Added call to wlan_sm_powerup() to fix powerup
                   initialization  issue between WLAN CP and WLAN iface hdlr.
11/14/05    gr     Changes to throttle the um link when the rm link is slower
                   than the um link (specifically the BT scenario)
10/28/05   sk      Increased DSI_CMD_BUF_COUNT to 36 in HDR
10/10/05   ks      Removed DS_RMNET_SIO_RDM_[OPEN/CLOSE]_CMD.
09/12/05   gr      Added debug messages to track leak in cmd buffer items
09/09/05  vrk/gr   Added support for L2 L3 Ack releated EPZID enhancements
09/13/05  jd/ks    Added DS_RMNET_SM_CMD.
09/09/05   jd      Moved rmnet_sm_init after task startup.  Renamed dsi_init
                   as dsi_powerup_init to reflect true behaviour.
09/08/05   ks      Changed qmux_process_cmd to qmi_process_cmd.
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
08/26/05   ar      Added support for CS Data in-call modification & fallback.
08/09/05   kvd     Added new cmds for RMSM Netmdl.
08/05/05   sk/vs   Merged the HDR Semi-Dynamic QOS changes from sandbox.
07/21/05   sr      Check if TIMER is active if FEATURE_USE_TIME_VU is defined
06/27/05   ks      Added support for QMI_CHARGER cmd.
06/27/05   jd      Added support for QMI commands
06/16/05   ar      Add support for asynchronous call setup response.
06/09/05   aku     Added support for WLAN WPA
06/02/05   sk      Moved hdrmrlpctl_startup() after ds task starting.
05/25/05   atp     Added support for semi-dynamic QOS on DO (flow_mgr cmds).
05/20/05   ar      Added support for DS_CM_CALL_PROGRESS_CMD event.
05/12/05   rr      Featurized Any events under FEATURE_DATA_IS_707.
04/21/05   mct     Removed include ps_bcmcs.h.
04/20/05   mct     Removed phys_link_down references for rmsm.
04/14/05   tkk     Removed featurized DS_AT_SND_CMD event handling.
04/05/05   gr      Integrated EPZID functionality onto the main line
03/22/03   snb     DS_AT_UI_CMD command merged from branch.
03/08/05   ks      Changed names of RM NET signals, cmds, functions.
03/02/05   snb     Add command supporting PS data generation AT command.
03/01/05   kvd     Added cmd processing for RDUD cmds.
02/24/05   sb      Accessories related changes.
02/15/05   jd      Added Rm NET control rx and wwan_rmsm command handling
                   for network interface.  Added Rm NET data signal to be
                   used when modem & net supported simultaneously.
01/19/05   ar      Merge in changes from ATCOP development branch.
01/12/05   aku     Added support for WLAN,
12/30/04   tkk     Added support for handling DS_AT_SRS_CMD async event.
11/20/04   kvd     Added cmd processing for CAM cmds.
11/29/04   vr      Merged PZID hysteresis changes into main line
11/18/04   dwp     Added DS_UCSD_USER_USER_* commands.
11/17/04   mv      Added DSUMTSPS_SEND_MT_REJECT_CMD.
11/09/04   ar      Generalize ATCOP GSDI SIM read to support write.
11/04/04   gr      Added PZID_HYS_SDB_DATA_CMD to notify the hysteresis
                   engine whenever SDB packet data is exchanged
10/12/04   vr      Added PZID_HYS_DATA_READY_CMD to notify the hysteresis
09/10/04   vas     QOS related changes
09/17/04   rc      Added DS_CM_SS_PS_DATA_FAIL_CMD.
09/10/04   sb      S-PDP changes
09/03/04   snb     Add support for AT command +CSIM under FEATURE_MMGSDI_ATCSIM
09/02/04   vas     Added command processing for CDMA subscription changed
07/06/04   snb     Add support for personalization PINs for ATCOP.
05/25/04   vr      Moved FEATURE_BCMCS above the comment
05/24/04   snb     Add low water event for DSAT to SIO queue.
05/20/04   snb     Add dog restart and error fatal under feature defines.
05/21/04   jd      Renamed FEATURE_AT_707_REINIT to FEATURE_UIM_SUPPORT_3GPD
05/12/04   kvd/vr  Added command processing for BCMCS/BCRM commands.
                   Added dsbc_1xhdr_init(), dsbc_init(), dsbcrm_1xhdr_init()
                   and dsbc_1xhdr_tx_init() to the init module.
04/21/04   mct     Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
04/09/04   rc      Fixed featurization problem with IS707 includes.
03/22/04   gr      Added command to process the Go Null timer. Go Null timer
                   is the same as Abort Dormant PPP timer.
03/11/04   vas     Moved hdrds_init before the task_start
02/27/04   kvd     Added DS_707_RMSM_UM_MIP_IP_ADDR_CHANGED_CMD.
02/24/04   snb     ATCoP SMS configuration and message commands now handled
                   via signals.
02/02/04   jd      Removed RMSM MIP_DOWN cmd, renamed MIP_UP as IFACE_UP
01/06/03   snb     Move registration for pin event with GSDI and handling
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.
01/12/04   snn     Cleared the DOG_TIMER_SIG in the set_signals variable
                   after processing the watch dog. This was done in dsi_wait
12/02/03   sb      Open ATCOP SMS events to both ETSI and CDMA
11/11/03   ar      Adjust ETSI wrapper for ATCOP events.
10/29/03   snb     Added support for ATCOP initiated asynchronous SIM read
                   and relocation of CM SS and PH cmd/events to common files.
10/14/03   ar      Add support for processing CM PH events.
10/14/03   vr      Added case DS_707_INITIATE_ALN_CMD
10/13/03   vas     Removed processing the HDR AN Call Connected/End events
10/01/03   ak      Added PZID delay timer processing commands.
10/01/03   ar      Removed obsolete FEATURE_DATA_ETSI_SUPSERV wrapper.
10/01/03   sb      SMS Cell broadcast additions
09/10/03   ar      Added support for CSD handover event.
09/10/03   ak      Removed retrieval of ESN from NV at startup.  never used.
09/02/03   sy      Added new command for pzid hysteresis timer expired.
08/27/03   vr      Added new command for Idle Digital Mode changed.
08/25/03   ak      Added new commands for 1x pkt IFACE indications.
06/18/03   vas     Increased number of bufs for DS cmd queue for HDR.
                   Init HDR modules after ds707_pkt_mgr
06/20/03   dgy     Added support for UMTS CS Data API.
05/21/03   vas     Added support for HDR. Processig of HDR auth commands.
05/15/03   rsl     Moved rex_clr_sigs to one common point just after dsi_wait.
05/05/03   usb     Added comment.
04/23/03   dgy     Merged GSM and WCDMA CS Data mode-specific handlers.
04/18/03   sb      Reduce DSI_CMD_BUF_COUNT to 12 to conserve storage.
04/18/03   vsk     updated to use umts_rmsm instead of gen_rmsm
04/11/03   ak      Updated to use new 707 RMSM CMD names.
03/27/03   ar      Reduce DSI_CMD_BUF_COUNT to 17 to conserve storage.
03/14/03   ar      Added DS_AT_CM_PH_CMD to command processing.
02/26/03   rc      Renamed DS_CM_SS_SYS_MODE_CHG_CMD as DS_CM_SS_SRV_CHG_CMD.
02/25/03   sb      Increased DSI_CMD_BUF_COUNT to 20. WMS is sending us 3 more
                   events. Hence the increase.
01/16/03   vsk     Initialize PDP registry during startup for wcdma / gprs
11/14/02   jd      Removed DS_RMSM_UM_PPP_UP_CMD (not used)
11/13/02   rc      Added Multimode support for GPRS.
11/12/02   ar      Add support for supplementary services events
11/11/02   sb      Call Related CM events now apply in all modes
10/14/02   ak      Removed JCDMA command processing - no longer needed.
10/14/02   sb      Merge from ETSI. PBM event declarations and state machine.
10/12/02   ak      Re-did FEATUREs around DS_GEN... commands
10/12/02   ak      Updated for JCDMA 1X.
10/04/02   usb     Added Generic RMSM specific command handling
10/03/02   rc      Added UMTS specific Commands to support multimode.
08/28/02   usb     Added/renamed RMSM cmd names
08/23/02   ak      Added OFFLINE & STOP sigs to requested sig mask.
08/14/02   atp     Added DS_707_ASYNC_ATZ_CMD.
08/06/02   ak      Updated for new task interface.
08/01/02   rc      Removed FEATURE_DS_SOCKETS wrap around socket commands.
                   Renamed dsumtsps_process_socket_cmd() as
                   dsumtsps_process_ps_cmds().
                   Fixed bug in dsi_wait() where the check for which signal was
                   received was incorrect.
07/27/02   vsk     Removed FEATURE_DATA_WCDMA_PS_PDP_IP and adhere to new
                   wcdma/gprs mode specific handler interface and file/fn naming
07/24/02   atp     Added DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD.
07/22/02   ak      Added DS_707_PKT_IFACE_DOWN_CBACK_CMD.
07/13/02   atp     Added commands for 707 async.
07/13/02   atp     Added init of 707 async mgr when DS task comes up.
07/03/02   ak      In nv_get/put, check to make sure in DS task.
06/18/02   tmr     Added DS_COMPLETE_LL_DISCONNECT_CMD, DS_GCSD_RX_DCD_ON_CMD and
                   DS_GCSD_TX_DATA_FLUSHED_CMD
05/21/01   smp     Added DS_CM_SS_SYS_MODE_CHG_CMD to dsi_process_cmds().
05/20/02   usb     Added command support for RMSM (IS707 pkt call Rm handler)
05/10/02   ak      Various changes to support 707 and new PS structure.
05/07/02    wx     Remove FEATURE_DSAT_TIMER
04/19/02   vsk     Added PDP context preservation support
04/18/02   rc      Commented out tmc_task_ack() and added line
                   (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG) instead.
04/17/02   rc      Added DS_INITIATE_CALL_CMD, DS_ENTER_SOCKET_MODE_CMD,
                   DS_EXIT_SOCKET_MODE_CMD, DS_ORIG_SOCKET_NET_CMD and
                   DS_RELEASE_SOCKET_NET_CMD to to dsi_process_cmds() for
                   processing commands related to socket calls.
04/04/02   tmr     Added handling of DS_TASK_STOP_SIG and DS_TASK_OFFLINE_SIG
                   signals
02/26/02   tmr     Added DS_COMPLETE_LL_CONNECT_CMD, DS_RELEASE_CALL_CMD to
                   DSMGR processed commands, and DS_GCSD_XID_NEG_COMPL_CMD to
                   GCSD processed commands.
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. Added WCDMA PS
                   mode-specific handler sub-task.
11/21/01   tmr     Added support for GSM CS data (GCSD) sub-task.
10/31/01   smp     Added support for watchdog kicking. Invoke WCDMA CS signal
                   handler function.
10/01/01   ar      Added dsi_get_nv_item() and dsi_put_nv_item()
08/29/01   smp     Renamed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
08/28/01   smp     Added WCDMA CS mode-specific handler sub-task, and moved
                   all sub-task initialization out of dsi_powerup_init() into
                   ds_task().
08/17/01   smp     Renamed ATCOP files/functions/data types.
05/24/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ttl_map.h"

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#include "lte_tlb.h"
#include "dslteps_ut_msg.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "queue.h"
#include "data_msg.h"

#include "modem_mem.h"

#include "task.h"
#include "dsutil.h"
#include "ds_3gpp_nv_manager.h"

#include "rcinit_rex.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dsgen_iface6_hdlr.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_ifaceAny_hdlr.h"
#endif
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#include "dsucsdhdlr.h"
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_PS)) || \
     (defined(FEATURE_GSM  ) && defined(FEATURE_GSM_GPRS)) || \
     (defined(FEATURE_DATA_LTE)))
#include "dsumts_rmsm.h"
#endif

#include "ds_3gpp_hdlr.h"

#ifdef FEATURE_DATA_EMBMS
#include "ds_3gpp_embms_hdlr.h"
#endif /* FEATURE_DATA_EMBMS */

#include "dstask_v.h"
#include "dstaski.h"
#include "ds3gmgr.h"
#include "ds3gutil.h"
#include "ds3gcfgmgr.h"
#include "ds3gcfcmif.h"
#include "dsat_v.h"
#include "dsati.h"
#include "dog_hb_rex.h"
#include "ds_profile_db_internal.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "ds_profile_3gpp2_mgr.h"
#include "ds707_async_mgr.h"
#include "ds_1x_profile.h"
#include "ds707_devmgr.h"
#ifdef FEATURE_DS_RDUD
#include "dsrdud.h"
#endif /* FEATURE_DS_RDUD */
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_HDR_QOS
#include "hdrmrlpctl.h"
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR
#include "hdrds.h"
#include "hdrpac.h"
#include "dshdr_an_mgr.h"
#ifdef FEATURE_DS_CAM
#include "dscam.h"
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "ds3gpp_ext_msg.h"
#endif
#ifdef FEATURE_DATA_TEST_LOOPBACK
#include "ds_loopback_hdlr.h"
#endif /* FEATURE_DATA_TEST_LOOPBACK */

#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_hdlr.h"
#endif /*FEATURE_EPC_HANDOFF*/

#include "ds_dsd_ext_i.h"
#include "ds_dsd_apm_ext_i.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_mppm_ext_i.h"
#include "ds_eps_kamgr_ext.h"
#include "ds_wlan_proxy_mode_hdlr.h"
#endif /* FEATURE_DATA_WLAN_MAPCON */

#ifdef FEATURE_DATA_IWLAN_S2B
#include "ds_iwlan_s2b_ext.h"
#endif /* FEATURE_DATA_IWLAN_S2B */

#include "ds3gstkproc.h"
#include "ds3gdsdif.h"
#include "ds3gcsdhdlr.h"
#include "ds3gtimer.h"
#include "ds3gflowmgr.h"

#include "sm_mgr.h"

#ifdef FEATURE_EHRPD
#include "ds707_ps_evt_hdlr.h"
#endif /* FEATURE_EHRPD */

#include "ds707_data_session_profile.h"
#include "ds_3gpp_profile_api.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif /* FEATURE_DATA_PKT_CLASSIFIER */

#include "ds_appsrv_task.h"
#include "ds_appsrv_utils.h"

#ifdef FEATURE_DATA_ATP_IMS_HTTP
#include "ds_atp_http.h"
#endif /* FEATURE_DATA_ATP_IMS_HTTP */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/
/*-------------------------------------------------------------------------
  DS command process data block structure used for queues
-------------------------------------------------------------------------*/
typedef struct
{
  q_link_type               link;
  ds_module_enum_type       ds_module_id;
  ds_cmd_process_func_type  cb_func_ptr;
}ds_cmd_process_block;

/*===========================================================================

                          EXTERNAL ROUTINES

===========================================================================*/
#if (defined(FEATURE_DATA_WCDMA_PS)||defined(FEATURE_GSM_GPRS)|| \
     defined(FEATURE_DATA_LTE))

  #if (defined(TEST_FRAMEWORK) || (!defined(FEATURE_DATA_FUSION_MSM_TYPE_2)))
    extern void ds_umts_pdp_registry_power_up_init(void);
  #endif

#endif /* (FEATURE_DATA_WCDMA_PS)||defined(FEATURE_GSM_GPRS)||(DATA_LTE) */

#ifdef IMAGE_MODEM_PROC
extern void ds_profile_power_up_init(void);
#endif /*IMAGE_MODEM_PROC*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Data services command queues
---------------------------------------------------------------------------*/
static q_type dsi_cmd_free_q = {{NULL}};    /* Queue for storing free command buffers */
static q_type dsi_cmd_q = {{NULL}};         /* DS Task command queue */
static q_type dsi_icmd_q = {{NULL}};        /* DS Task internal command queue */

/*---------------------------------------------------------------------------
  Dog data structures for DS task
---------------------------------------------------------------------------*/
static uint32  ds_dog_rpt_id = 0;         /*! dog report ID */

static rex_sigs_type   siolib_sigs = 0;   /*! SIOLIB signals to suspend on */ 

/*---------------------------------------------------------------------------
  TCB pointer for DS task
---------------------------------------------------------------------------*/
static rex_tcb_type* ds_tcb_ptr = NULL;

/*-------------------------------------------------------------------------
  DS queues to store module command processing callbacks
-------------------------------------------------------------------------*/
static q_type dsi_cmd_process_q = {{NULL}};
static q_type dsi_cmd_process_free_q = {{NULL}};

/*-------------------------------------------------------------------------
  Array of DS command processing block
-------------------------------------------------------------------------*/
ds_cmd_process_block ds_cmd_process_list[DS_MODULE_MAX] = {{{NULL}}};

/*===========================================================================

              EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST //#ifdef TEST_FRAMEWORK
/*===========================================================================
FUNCTION      DSI_UT_SEND_SYNC_CNF

DESCRIPTION

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static void dsi_ut_send_sync_cnf(void)
{
  boolean               rval;
  msgr_hdr_struct_type  msg; /* Message with only the header and no body */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rval = dsmsgrsnd_msg_send(DS_UT_DSTASK_SYNC_CNF, &msg, sizeof(msg));
  ASSERT(rval == TRUE);
  return;
} /* dsi_ut_send_sync_cnf() */

/*===========================================================================
FUNCTION      DSI_UT_SYNC_REQ_HDLR

DESCRIPTION

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static boolean dsi_ut_sync_req_hdlr
(
  msgr_umid_type        msgtype,
  const msgr_hdr_struct_type *msg_p
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (msgtype == DS_UT_DSTASK_SYNC_REQ)
  {
    ds_ut_sync_cmd_hdlr();
  }
  else
  {
    /* We don't expect to receive any other message type
    */
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "MSG type %d!= DS_UT_DSTASK_SYNC_REQ",msgtype);
    ASSERT(0);
  }

  return TRUE;
} /* dsi_ut_sync_req_hdlr() */

/*===========================================================================
FUNCTION      DSI_UT_DS_CLOSE_READY_HDLR

DESCRIPTION

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static boolean dsi_ut_ds_close_ready_hdlr
(
  msgr_umid_type               msgtype,
  const msgr_hdr_struct_type  *msg_p
)
{
  boolean               rval = FALSE;
  msgr_hdr_struct_type  msg; /* Message with only the header and no body */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (msgtype == LTE_TLB_DS_CTL_READY_IND)
  {
    rval = dsmsgrsnd_msg_send(DS_UT_LTE_TLB_DS_CTL_READY_CNF,
                              &msg,
                              sizeof(msg));
    ASSERT(rval == TRUE);
    return TRUE;
  }
  else
  {
    /* We don't expect to receive any other message type
    */
    ASSERT(0);
  }

  return rval;
} /* dsi_ut_ds_close_ready_hdlr() */

/*===========================================================================
FUNCTION      DSI_UT_DS_BEARER_QOS_INFO_IND_HDLR

DESCRIPTION   

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static boolean dsi_ut_ds_bearer_qos_info_ind_hdlr
(
  msgr_umid_type               msgtype,
  const msgr_hdr_struct_type  *msg_p
)
{
  boolean rval = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (msgtype == DS_3GPP_BEARER_QOS_INFO_IND)
  {
    (void)dsmsgrsnd_msg_send(DS_UT_3GPP_BEARER_QOS_INFO_IND,
                              (msgr_hdr_struct_type *)msg_p,
                              sizeof(ds_3gpp_bearer_qos_info_msg_s));
    return TRUE;
  }
  else
  {
    /* We don't expect to receive any other message type
    */
    ASSERT(0);
  }

  return rval;
}/* dsi_ut_ds_bearer_qos_info_ind_hdlr() */
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

/*===========================================================================
FUNCTION      DSI_POWERUP_INIT

DESCRIPTION   This function performs all the power-up initialization for the 
              Data Services Task.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  dsi_powerup_init( void )
{
  uint8  i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
  -------------------------------------------------------------------------*/
  (void)q_init( &dsi_cmd_q );
  (void)q_init( &dsi_icmd_q );

  /*-------------------------------------------------------------------------
   Under FEATURE_DATA_MODEM_HEAP, we do not use the free queue anymore. The
   only    reason it was still initialized is that the QOS code makes some
   decisions based on free_q_cnt. The QOS code will be cleaned up as part of
   refactoring.
  -------------------------------------------------------------------------*/

  (void)q_init( &dsi_cmd_free_q );

  /*-----------------------------------------------------------------------
   Initialize DS cmd process queues for module registration
  -----------------------------------------------------------------------*/
  q_init(&dsi_cmd_process_q);
  q_init(&dsi_cmd_process_free_q);
  for(i = 0; i < DS_MODULE_MAX; i++)
  {
    memset(&ds_cmd_process_list[i], 0, sizeof(ds_cmd_process_block));
    ds_cmd_process_list[i].ds_module_id = DS_MODULE_INVALID;
    ds_cmd_process_list[i].cb_func_ptr  = NULL;

    q_link((void *)&ds_cmd_process_list[i], &(ds_cmd_process_list[i].link));
    q_put(&dsi_cmd_process_free_q, &ds_cmd_process_list[i].link);
  }

  /*-------------------------------------------------------------------------
    Initialize DSSNET6 module.
  -------------------------------------------------------------------------*/
  dssnet6_init();

/* Init 3gpp critical sections */
  ds3gpp_init_all_crit_sect();

/*-------------------------------------------------------------------------
  Initialize timers
-------------------------------------------------------------------------*/
  ds3gi_timer_init();

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
   Initialize the HDRDS modules.
  -------------------------------------------------------------------------*/
  hdrds_init();
#endif /* FEATURE_HDR */

  /*-------------------------------------------------------------------------
    Initialize SIOLIB -- done at task creation time so that the RDM
    registrations are performed before the rdm_issue_opens() call is made.
   -------------------------------------------------------------------------*/
  siolib_sigs = ds3g_siolib_init();

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) \
    || defined(FEATURE_DATA_LTE)
  /*-------------------------------------------------------------------------
    Powerup init for UMTS PDP REG API in DS Task context
   -------------------------------------------------------------------------*/
  ds_umts_pdp_registry_power_up_init();
#endif /*(FEATURE_DATA_WCDMA_PS)||defined(FEATURE_GSM_GPRS)||(DATA_LTE) */

#ifdef IMAGE_MODEM_PROC
  /*------------------------------------------------------------------------
    DS Profile Profile powerup initialization
  -------------------------------------------------------------------------*/
  ds_profile_power_up_init();
#endif

#ifdef FEATURE_HDR_QOS
  /*-------------------------------------------------------------------------
    This startup depends on reading on NV item and should happen after the
    task has started.
  -------------------------------------------------------------------------*/
  hdrmrlpctl_startup();
#endif

#ifdef FEATURE_DATA_IS707
  ds707_nv_util_1x_profile_init(); 

  /*------------------------------------------------------------------------- 
    Initialize the ds707 global crit section. Since the crit sections are
    accessed even before ds707_pkt_mgr_init is called (e.g dsat_init), we
    need to init the crit sections here
  -------------------------------------------------------------------------*/
  ds707_pkt_mgr_init_crit_sect();
#endif /* FEATURE_DATA_IS707 */

  ds3g_init_crit_sect();  
#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_IS707)
  /*------------------------------------------------------------------------- 
    Initialize the ds707 device manager
  -------------------------------------------------------------------------*/
  ds707_devmgr_powerup_init();
#endif /* FEATURE_EHRPD && FEATURE_DATA_IS707 */

  sm_powerup_init();

#ifdef FEATURE_DATA_IS707
  /*------------------------------------------------------------------------- 
    3GPP2 MH Power up Init
  -------------------------------------------------------------------------*/
  ds707_pkt_mgr_powerup_init();
#endif

#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM
  /*-----------------------------------------------------------------------
    Initialize the CAM module - register CAM timers and register for
    phys link events.
  -----------------------------------------------------------------------*/
  dscam_init();
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */

  /*------------------------------------------------------------------------- 
    ATCoP Power up Init
  -------------------------------------------------------------------------*/
  dsat_powerup_init();

} /* dsi_powerup_init() */

/*===========================================================================
FUNCTION      ARB_POLICY_INIT

DESCRIPTION   Add records to policy mgr for arb

DEPENDENCIES  

RETURN VALUE  None

SIDE EFFECTS  none
===========================================================================*/
static void arb_policy_init( void )
{
  /*------------------------------------------------------------------------
    Add record to policy manager for arbitration. The following records are
    added:
    { APP_ID_TE_RMNET,       PS_POLICY_MGR_ANY_PROFILE, PS_POLICY_MGR_PRIORITY_MAX - PRIO_TE_RMNET }
    { APP_ID_TE_DUN,         PS_POLICY_MGR_ANY_PROFILE, PS_POLICY_MGR_PRIORITY_MAX - APP_ID_TE_DUN }
    { PS_POLICY_MGR_ANY_APP, PS_POLICY_MGR_ANY_PROFILE, PS_POLICY_MGR_PRIORITY_MAX - PS_POLICY_MGR_PRIORITY_MAX }
  ------------------------------------------------------------------------*/

#define PROC_ID_TE_DUN   0x1
#define APP_ID_TE_DUN    0x0000000100000000ULL
#define PRIO_TE_DUN      0x0 /*MAX*/

#define PROC_ID_APPS     0x2
#define APP_ID_APPS      0x0000000200000000ULL
#define PRIO_APPS        0x1

#define PROC_ID_TE_RMNET 0x3
#define APP_ID_TE_RMNET  0x0000000300000000ULL
#define PRIO_TE_RMNET    0x0 /*MAX*/


  if( ( ps_policy_mgr_add_record( APP_ID_TE_DUN,
                                   PS_POLICY_MGR_ANY_PROFILE,
                                   PS_POLICY_MGR_PRIORITY_MAX - PRIO_TE_DUN ) ) != 0 )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "unable to add TE-DUN record to policy mgr");
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "added TE-DUN record, prf [0x%x], prio [0x%x]",
                    PS_POLICY_MGR_ANY_PROFILE,
                    PS_POLICY_MGR_PRIORITY_MAX - PRIO_TE_DUN);
  }

  if ( ( ps_policy_mgr_add_record( APP_ID_APPS,
                                    PS_POLICY_MGR_ANY_PROFILE,
                                    PS_POLICY_MGR_PRIORITY_MAX - PRIO_APPS ) ) != 0 )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                     "unable to add APPS-RmNET record to policy mgr");
  }
  else
  {
    DATA_3GMGR_MSG2( MSG_LEGACY_LOW,
                     "added APPS record, prf [0x%x], prio [0x%x]",
                     PS_POLICY_MGR_ANY_PROFILE, 
                     PS_POLICY_MGR_PRIORITY_MAX - PRIO_APPS);
  }

  if ( ( ps_policy_mgr_add_record( APP_ID_TE_RMNET,
                                    PS_POLICY_MGR_ANY_PROFILE,
                                    PS_POLICY_MGR_PRIORITY_MAX - PRIO_TE_RMNET ) ) != 0 )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "unable to add TE-RMNET record to policy mgr");
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "added TE-RMNET record, prf [0x%x], prio [0x%x]",
                    PS_POLICY_MGR_ANY_PROFILE,
                    PS_POLICY_MGR_PRIORITY_MAX - PRIO_TE_RMNET);
  }

} /* arb_policy_init() */

/*===========================================================================
FUNCTION      DSI_WAIT

DESCRIPTION   This function suspends execution of the Data Services task
              until one (or more) signals from the requested signal mask is
              set.
              Watchdog kicking is performed in this function.

DEPENDENCIES  None

RETURN VALUE  The signal mask returned by rex_wait().

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type  dsi_wait
(
  rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
)
{
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/
  do
  {
    set_signals = rex_wait( requested_signal_mask | DS_DOG_HB_REPORT_SIG );

    if( (set_signals & DS_DOG_HB_REPORT_SIG) != 0 )
    {
      /*---------------------------------------------------------------------
        Report back to Dog heart beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      dog_hb_report( ds_dog_rpt_id);
      /*---------------------------------------------------------------------
        Processed the DS_DOG_HB_REPORT_SIG so clear it off in set_signals.
      ---------------------------------------------------------------------*/
	  (void) rex_clr_sigs( dsi_get_ds_tcb(), DS_DOG_HB_REPORT_SIG );
      set_signals &= (~DS_DOG_HB_REPORT_SIG);
    }
  } while( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* dsi_wait() */

/*===========================================================================
FUNCTION      DSI_GET_NEXT_CMD

DESCRIPTION   This function de-queues and returns a command from the 
              Data Services Task's command queues. Command is dequeued from
              the high priority internal command queue, if available,
              otherwise from the regular command queue.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static ds_cmd_type* dsi_get_next_cmd( void )
{
  ds_cmd_type *cmd_ptr;

  if ((cmd_ptr = (ds_cmd_type *)q_get( &dsi_icmd_q )) == NULL)
    cmd_ptr = (ds_cmd_type *)q_get( &dsi_cmd_q );

  return cmd_ptr;
} /* dsi_get_next_cmd() */

/*===========================================================================
FUNCTION      DSI_PROCESS_CMDS

DESCRIPTION   This function de-queues commands from the Data Services Task's 
              command queue, and dispataches commands to the appropriate
              entity for further processing, if necessary. Commands are
              de-queued until the command queue is empty.

DEPENDENCIES  This function should be called when the DS_CMD_Q_SIG is set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void dsi_process_cmds( void )
{
  ds_cmd_type           *cmd_ptr = NULL;
  boolean                processed_cmd = FALSE;
  ds_module_enum_type    module_id = DS_MODULE_INVALID;
  uint8                  q_count = 0;
  ds_cmd_process_block  *data_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, dispatch the command to the appropriate sub-task.
  -------------------------------------------------------------------------*/
  while( (cmd_ptr = dsi_get_next_cmd()) != NULL )
  {
    processed_cmd = FALSE;

#ifdef DS_TASK_DEBUG
    /*-----------------------------------------------------------------------
      Do not Strip.  Here for debugging purposes only.
    -----------------------------------------------------------------------*/
    if ( (cmd_ptr->hdr.cmd_id <= DS_MIN_CMD) ||
         (cmd_ptr->hdr.cmd_id >= DS_MAX_CMD)
       )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cmd_id %d is not within DS_MIN_CMD and DS_MAX_CMDS",
                      (int)cmd_ptr->hdr.cmd_id);
      ASSERT(0);
    }
#endif /* DS_TASK_DEBUG */

    /*-----------------------------------------------------------------------
      New command processing logic
    -----------------------------------------------------------------------*/
    /* Get module id from command id for new commands */
    module_id 
      = ((uint32)(cmd_ptr->hdr.cmd_id)) / DS_MAX_CMDS_PER_MODULE;
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "New DS task command:%d module_id:%d",
                    (uint32)cmd_ptr->hdr.cmd_id, module_id);

    /*---------------------------------------------------------------------
      Call module specific command handler
    ---------------------------------------------------------------------*/
    q_count  = q_cnt(&dsi_cmd_process_q);
    while(q_count--)
    {
      data_block = (ds_cmd_process_block*)q_get(&dsi_cmd_process_q);
      if(data_block)
      {
        if((data_block->ds_module_id == module_id) &&
           (data_block->cb_func_ptr  != NULL))
        {
          processed_cmd = TRUE;
          data_block->cb_func_ptr(cmd_ptr);
        }
        q_put(&dsi_cmd_process_q, &(data_block->link));
      }
    } /* end while */

    if(processed_cmd == FALSE)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_FATAL, "cmd_id:%d not processed",
                      cmd_ptr->hdr.cmd_id);
      ASSERT(0);
    }

    DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                    "Processed cmd %d from DS Cmd buffer - "
                    "cmd_q cnt %d free_q cnt %d",
                    cmd_ptr->hdr.cmd_id,
                    q_cnt(&dsi_cmd_q) + q_cnt(&dsi_icmd_q),
                    q_cnt(&dsi_cmd_free_q) );

    ds_release_cmd_buf(&cmd_ptr);

  } /* while */
  return;
} /* dsi_process_cmds() */

/*===========================================================================
FUNCTION      DSI_PUT_CMD

DESCRIPTION   This function puts a command on the Data Services Task's
              command queue, and sets the DS_CMD_Q_SIG. The command queue to
              use is passed as an argument.

DEPENDENCIES  The caller of this function should have previously allocated a
              command buffer by calling ds_get_cmd_buf().

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void dsi_put_cmd
(
  ds_cmd_type    *cmd_ptr,                /* Pointer to command to be sent */
  q_type         *cmd_q_ptr                    /* Pointer to command queue */
)
{
#ifdef DS_TASK_DEBUG
  uint32 i;
#endif /* DS_TASK_DEBUG */
#ifdef FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG
  time_type up_time;
#endif /*FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG
  // Get time since phone power up and set timestamp field in command header
  memset( up_time, 0, sizeof(time_type) );
  time_get_uptime_ms(up_time);

  cmd_ptr->hdr.timestamp[0] = up_time[0];
  cmd_ptr->hdr.timestamp[1] = up_time[1];
#endif /*FEATURE_DATA_DS_CMD_TIMESTAMP_DEBUG*/
  /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
  -------------------------------------------------------------------------*/
  q_put( cmd_q_ptr, &cmd_ptr->hdr.link );
  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_CMD_Q_SIG );

  DATA_3GMGR_MSG3(MSG_LEGACY_MED, "Putting cmd %d into DS Cmd buffer - "
                  "cmd_q cnt %d free_q cnt %d", cmd_ptr->hdr.cmd_id,
                  q_cnt(&dsi_cmd_q) + q_cnt(&dsi_icmd_q),
                  q_cnt(&dsi_cmd_free_q));

} /* dsi_put_cmd() */

/*===========================================================================
FUNCTION      DSI_ARBITRATION_INIT

DESCRIPTION   Init arbitration if allowed

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
static void dsi_arbitration_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( TRUE == ds3g_get_enable_arbitration() )
  {
    /*-------------------------------------------------------------------------
      Arbitration support
    -------------------------------------------------------------------------*/
    arb_policy_init();
  }
  return;
} /* dsi_arbitration_init() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION      DS_TASK

DESCRIPTION   This is the entry point for the Data Services Task. This
              function contains the main processing loop that waits for
              events (signals or commands) and dispatches each event to the
              appropriate entity for further processing.

DEPENDENCIES  None

RETURN VALUE  This function does not return.

SIDE EFFECTS  None
===========================================================================*/
void  ds_task
(
  dword ignored
)
{
  rex_sigs_type   ret_val=0;        /* return value for rex_clr_sigs       */
  rex_sigs_type   requested_sigs;   /* Signal mask to suspend on           */
  rex_sigs_type   set_sigs;         /* Signals set upon return from wait   */
  rex_sigs_type   atcop_sigs  = 0;  /* ATCoP signals to suspend on         */
  rex_sigs_type   ucsd_sigs   = 0;  /* UMTS CS Hdlr signals to suspend on */
  rex_sigs_type   wpsd_sigs   = 0;  /* WCDMA PS Hdlr signals to suspend on */
  rex_sigs_type   cdma_sigs   = 0;  /* CDMA sub-task signals to suspend on */
  rex_sigs_type   async707_sigs= 0; /* 707 async signals to suspend on     */
  rex_sigs_type   lte_sigs    = 0;   /* LTE Hdlr signal to suspend on */

#ifdef FEATURE_DATA_LTE_ESPRESSO_TEST
  boolean         b_first_time_use = TRUE;
#endif /*FEATURE_DATA_LTE_ESPRESSO_TEST */

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  RCEVT_SIGEX_SIGREX term_sig; /* Termination Signal to wait on*/
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   TTLBOOTUP(STARTUP_DS_TASK_MS);


#ifdef FEATURE_MODEM_RCINIT_PHASE2
  term_sig.signal = ds_tcb_ptr = rex_self();
  term_sig.mask = DS_TASK_STOP_SIG; 

  /* Register for the STOP Signal */
  rcinit_register_term_group(RCINIT_GROUP_5, 
                             RCEVT_SIGEX_TYPE_SIGREX, 
                             &term_sig);
  //rcinit_handshake_startup(); moved to end!
#else
  /*-------------------------------------------------------------------------
    Do task initialization. The init function performs all the task-level
    initialization.
  -------------------------------------------------------------------------*/
  ds_tcb_ptr = &ds_tcb;
  dsi_powerup_init();
#endif /*FEATURE_MODEM_RCINIT_PHASE2*/

  /*-------------------------------------------------------------------------
   Dog heart beat registration.
  -------------------------------------------------------------------------*/
  ds_dog_rpt_id = dog_hb_register_rex((rex_sigs_type)DS_DOG_HB_REPORT_SIG);



  /*-------------------------------------------------------------------------
    Initialize DS3G Cfg Manager
  -------------------------------------------------------------------------*/
  ds3gcfgmgr_init();

  /*-------------------------------------------------------------------------
    Read DS3G NV items
  -------------------------------------------------------------------------*/
  ds3g_util_read_nv();

  /*-------------------------------------------------------------------------
    Initialize DS3G Event Manager
  -------------------------------------------------------------------------*/
  ds3geventmgr_init();

#if defined(FEATURE_MODEM_CONFIG_REFRESH) && defined(FEATURE_DATA_IS707)
  /*-------------------------------------------------------------------------------------------
  Initialize the list for first use for NV Refresh functions   
  ---------------------------------------------------------------------------------------------*/	 
  DATA_IS707_MSG0(MSG_LEGACY_LOW, "Init the list first");
  ds707_nv_util_init();
#endif /* FEATURE_MODEM_CONFIG_REFRESH && FEATURE_DATA_IS707 */


  /*-------------------------------------------------------------------------
    Read/store WLAN EFS NV items
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_wlan_powerup_init();
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*-------------------------------------------------------------------------
    Wrapper function to read/store DS707 EFS NV items
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
  ds707_nv_util_read_pkt_mgr_nv_sync();
#endif /* FEATURE_DATA_IS707 */

  /*-------------------------------------------------------------------------
    Init Arbitration 
  -------------------------------------------------------------------------*/
  dsi_arbitration_init();

  /*-------------------------------------------------------------------------
    Perform sub-task initialization.
  -------------------------------------------------------------------------*/
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) \
    || defined(FEATURE_DATA_LTE)

  #if (defined(TEST_FRAMEWORK) || (!defined(FEATURE_DATA_FUSION_MSM_TYPE_2)))
    /*-------------------------------------------------------------------------
      Initialize the PDP profiles registry for wcdma / gprs
      This should be done before ATCOP as ATCOP is one of the users of the
      profiles.
    -------------------------------------------------------------------------*/
    if( ds_umts_pdp_registry_init() != DS_UMTS_PDP_SUCCESS )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "UMTS PDP Profile Registry initialization failed");
    }
  #endif

#endif/*(FEATURE_DATA_WCDMA_PS)||defined(FEATURE_GSM_GPRS)||(DATA_LTE) */

#ifdef FEATURE_DATA_IS707
  /*------------------------------------------------------------------------
    Init go null timer values. This is needed for JCDMA profile init in
    3GPP2 profile cache init. Will be removed if they move away from
    hardcoded profile values. 
  ------------------------------------------------------------------------*/
  ds707_nv_util_init_jcdma_read_nv();

  /*------------------------------------------------------------------------
    Initialize 3GPP2 profile cache structure.
  ------------------------------------------------------------------------*/
  ds707_data_session_profile_cache_init();

#endif

  /*-------------------------------------------------------------------------
    Initialize DS Profile library
  -------------------------------------------------------------------------*/
  if( ds_profile_init_lib() != DS_PROFILE_REG_RESULT_SUCCESS )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "DS Profile library initialization failed");
  }


  /*-------------------------------------------------------------------------
    Each sub-task returns a signal mask containing the signals it wishes
    to suspend on.
    Note that ATCoP initialization should always be performed first,
    since other sub-tasks may use AT parameter values during initialization.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
   Initialize DSD module
  -------------------------------------------------------------------------*/
  ds_dsd_init();

  /*-------------------------------------------------------------------------
   Initialize DS3G modules
  -------------------------------------------------------------------------*/
  ds3gsubsmgr_init();
  ds3gstkproc_init();
  ds3gdsdif_init();
  ds3gcsdhdlr_init();
  ds3g_init();
  ds3gflowmgr_init();
  ds3gcfcmif_init();
  ds_3gpp_profile_function_table_init();

    /*------------------------------------------------------------------------
    Profile DB init.
  ------------------------------------------------------------------------*/
  if (DB_RESULT_SUCCESS != ds_profile_db_init()) 
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
      "DB Profile library initialization failed");

    /* TODO remove */
    ASSERT(0);
  }
  /*------------------------------------------------------------------------ 
    Register with Profile DB
    ------------------------------------------------------------------------*/
  ds_3gpp_profile_api_init();

#ifdef FEATURE_DATA_IS707
  ds_profile_3gpp2_mgr_register_cb_event();
#endif

  ds_profile_db_init_power_up_finished();
  /*-------------------------------------------------------------------------
    Each sub-task returns a signal mask containing the signals it wishes
    to suspend on.
    Note that ATCoP initialization should always be performed first,
    since other sub-tasks may use AT parameter values during initialization.
  -------------------------------------------------------------------------*/

  ds_3gpp_nv_manager_init(); 

  /*-------------------------------------------------------------------------
    Register Appsrv command to dstask
  -------------------------------------------------------------------------*/
  ds_appsrv_cmd_init();

  /*--------------------------------------------------------------------------
    Register APPSRV NV refresh event (andsf), should before mppm_init
  ----------------------------------------------------------------------------*/
  ds_appsrv_utils_init();

  /*-------------------------------------------------------------------------
    Initialize ANDSF module
  -------------------------------------------------------------------------*/
  ds_andsf_rule_init();

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
   Initialize MPPM module
  -------------------------------------------------------------------------*/
  ds_mppm_init();
#endif /*FEATURE_DATA_WLAN_MAPCON*/

 /*-------------------------------------------------------------------------
   Initialize APM module. It has to be called after MPPM module initialization.
  -------------------------------------------------------------------------*/
  ds_dsd_apm_init();

#ifdef FEATURE_EHRPD
  /*------------------------------------------------------------------------- 
    Initialize ds707 ps evt handler.Calls ps_sys_event_reg() –
    So this init should be done in task init. 
  -------------------------------------------------------------------------*/
  ds707_ps_evt_hdlr_init();
#endif /* FEATURE_EHRPD */

  atcop_sigs = dsat_init();

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
  ucsd_sigs = ds_ucsd_hdlr_init();
#endif

#ifdef FEATURE_DSAT_MDM_FUSION
  ds707_rmsm_proxy_init();
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_PS)) || \
     (defined(FEATURE_GSM  ) && defined(FEATURE_GSM_GPRS))      || \
     defined(FEATURE_DATA_LTE))
  (void)dsumts_rmsm_init();
#endif

  lte_sigs = ds3gpp_init();
  ds3g_util_init();
#if defined (FEATURE_DATA_WLAN_MAPCON) && defined (FEATURE_DATA_LTE) 
  ds_eps_kamgr_init();
#endif
#ifdef FEATURE_DATA_TEST_LOOPBACK
  /* Initialize data loopback mode handler */
  ds_loopback_hdlr_init();
#endif /* FEATURE_DATA_TEST_LOOPBACK */

#ifdef FEATURE_DATA_EMBMS
  /* Initialize the eMBMS mode handler */
  ds_3gpp_embms_init();
#endif


#if defined(FEATURE_DATA_IS707)
  /*-------------------------------------------------------------------------
    Make sure that Pkt iface is always initialized before Async iface. This
    is because ps_iface assigns instance numbers to iface in sequential
    order. We want pkt iface to get instance 0...max_pkt_ifaces since Apps
    call ioctls on pkt iface using those instance numbers.
  -------------------------------------------------------------------------*/
  cdma_sigs = ds707_pkt_mgr_init();

#ifdef FEATURE_DATA_IS707_ASYNC
  async707_sigs= ds707_async_mgr_powerup_init();
#endif /* FEATURE_DATA_IS707_ASYNC */

#ifdef FEATURE_DS_RDUD
  /*-----------------------------------------------------------------------
    Initialize the RDUD module - register RDUD timers and register for
    phys link events.
  -----------------------------------------------------------------------*/
  dsrdud_init();
#endif /* FEATURE_DS_RDUD */
#endif

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
   Initialize the HDRPAC modules. The PAC initializes FCP. Make sure
   that this initialization happens after  ds707_pkt_mgr_init()
  -------------------------------------------------------------------------*/
  hdrpac_powerup_init();

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  ds707_pkt_classifier_init();
#endif /* FEATURE_DATA_PKT_CLASSIFIER */

  /*-------------------------------------------------------------------------
   Initialize the HDR AN authentication module
  -------------------------------------------------------------------------*/
  dshdr_an_mgr_powerup_init();

#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_wlan_proxy_mode_hdlr_init();
#endif /*FEATURE_DATA_WLAN_MAPCON*/

#ifdef FEATURE_DATA_IWLAN_S2B
  /*-------------------------------------------------------------------------
    Initialize the iwlan s2b module
  -------------------------------------------------------------------------*/
    ds_iwlan_s2b_task_init();
#endif /* FEATURE_DATA_IWLAN_S2B */

  /*-------------------------------------------------------------------------
   Move EPC initialize function out of FEATURE_HDR, since LTE and iwlan 
   handoff needs EPC module as well.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_EPC_HANDOFF
  if( ds_epc_disable_flag == 0 )
  {
    ds_epc_hdlr_init();
  }
#endif

#ifdef FEATURE_MBMS
  #error code not present
#endif /* FEATURE_MBMS */

#ifdef FEATURE_DATA_IS707
  ds707_nv_util_init_done();
#endif /*FEATURE_DATA_IS707 */

#ifdef FEATURE_MBMS
  #error code not present
#endif /* FEATURE_MBMS */

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
  dsmsgrrcv_reg_hdlr(DS_UT_DSTASK_SYNC_REQ, dsi_ut_sync_req_hdlr);
  dsmsgrrcv_reg_hdlr(LTE_TLB_DS_CTL_READY_IND, 
                     dsi_ut_ds_close_ready_hdlr);
  dsmsgrrcv_reg_hdlr(DS_3GPP_BEARER_QOS_INFO_IND, 
                     dsi_ut_ds_bearer_qos_info_ind_hdlr);
#endif

  /*=========================================================================
                !!!!!! REGISTRATION WITH EXTERNAL MODULES !!!!!!
   
    Any function that registers itself as a client with external modules
    should be called at the very end of the task init.
    If your function doesn't register with external modules add your
    function above this. Else add it below
   
    Reason: As soon as your module registers as a client with the external
    module, the ext module might start interacting with DS immediately
    while DS task is still in task init. This will lead to unnecessary
    crashes.
  =========================================================================*/

 /*-------------------------------------------------------------------------
    Registers ds3gmgr as client with external modules like CM
  -------------------------------------------------------------------------*/
  ds3g_client_reg();

  /*-------------------------------------------------------------------------
    Initialize Appsrv modules
  -------------------------------------------------------------------------*/
  ds_appsrv_init();

  //Task Ready now...!
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  rcinit_handshake_startup();
  if(RCEVT_NULL != rcevt_signal_name("DS:READY"))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_MED," DS task is ready");
  }
#else
  /*-------------------------------------------------------------------------
    Wait for the task start signal from task controller.
   -------------------------------------------------------------------------*/
  tmc_task_start();
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  
  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  requested_sigs = DS_CMD_Q_SIG        |
                   DS_RMSM_FC_SIG      |
                   siolib_sigs         |
                   atcop_sigs          |
                   ucsd_sigs           |
                   lte_sigs            |
                   cdma_sigs           |
                   async707_sigs       |
                   DS_TASK_STOP_SIG    |
                   DS_TASK_OFFLINE_SIG |
                   DS_NOTIFY_NAS_SVC_SIG
#ifdef FEATURE_DATA_ATP_IMS_HTTP
                   | DS_IMS_MSG_SIG
#endif /* FEATURE_DATA_ATP_IMS_HTTP */
                   ;
   TTLBOOTUP(DS_TASK_READY);
  /*-------------------------------------------------------------------------
    Main task loop, never exits.
  -------------------------------------------------------------------------*/
  for( ;; )
  {
    /*----------------------------------------------------------------------
      Wait for one of the specified signals to be set. Note that watchdog
      kicking is performed in the wait.
    -----------------------------------------------------------------------*/
    set_sigs = dsi_wait( requested_sigs );

    /*----------------------------------------------------------------------
    We used to individually clear the wrong set of signals and some signals
    were getting lost.  Here, we clear ds_tcb with set_sigs.  set_sigs is
    not altered.
    ----------------------------------------------------------------------*/
    ret_val = rex_clr_sigs( dsi_get_ds_tcb(), set_sigs );

    if( !(ret_val & set_sigs) )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW," The previously set signal was not set");
    }

  /*----------------------------------------------------------------------
    Sometimes we see signals that were not requested in wait being set.
    So we mask returned sigs with requested sigs
    ----------------------------------------------------------------------*/
    set_sigs &= requested_sigs;

    /*----------------------------------------------------------------------
      If any of the task signals were received, invoke the function to ACK
      task conroller.
    -----------------------------------------------------------------------*/
    if( (set_sigs & DS_TASK_STOP_SIG) != 0 )
    {
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */

#ifdef FEATURE_MODEM_RCINIT_PHASE2
      dog_hb_deregister(ds_dog_rpt_id);
      rcinit_unregister_term_group(RCINIT_GROUP_5, RCEVT_SIGEX_TYPE_SIGREX, &term_sig);
      rcinit_handshake_term();
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
#else
      tmc_task_stop();
#endif 
    }

    if( (set_sigs & DS_TASK_OFFLINE_SIG) != 0 )
    {
      //No action!
    }
    /*----------------------------------------------------------------------
      If the command queue signal was set, clear the signal and invoke the
      function that dispatches commands to the appropriate sub-task.
    -----------------------------------------------------------------------*/
    if( (set_sigs & DS_CMD_Q_SIG) != 0 )
    {
      dsi_process_cmds();
    }
    /*----------------------------------------------------------------------
      If the RMSM FC signal was set, clear the signal and invoke the
      function that dispatches commands to the appropriate sub-task.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_WCDMA_PS
    if( (set_sigs & DS_RMSM_FC_SIG) != 0 )
    {
      dsumts_rmsm_process_sig();
    }
#endif /* FEATURE_DATA_WCDMA_PS */
    /*----------------------------------------------------------------------
      If any of the 3G SIOLIB signals were set, clear the signals and invoke
      a function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & siolib_sigs) != 0 )
    {
      ds3g_siolib_process_signals( set_sigs );
    }

    /*----------------------------------------------------------------------
      If any of the ATCoP signals were set, clear the signals and invoke a
      function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & atcop_sigs) != 0 )
    {
      dsat_process_async_signal( set_sigs );
    }

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
    /*----------------------------------------------------------------------
      If any of the UMTS CS Hdlr signals were set, clear the signals and
      invoke a function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & ucsd_sigs) != 0 )
    {
      ds_ucsd_process_signals( set_sigs );
    }
#endif

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_PS)) ||\
     (defined(FEATURE_GSM  ) && defined(FEATURE_GSM_GPRS)))
    /*----------------------------------------------------------------------
      If any of the WCDMA PS Hdlr signals were set, clear the signals and
      invoke a function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & wpsd_sigs) != 0 )
    {
      //Need a WPSD function to process the set signals
    }
#endif
    if( (set_sigs & lte_sigs) != 0 )
    {
      //Need a LTE function to process the set signals
    }
#if defined(FEATURE_DATA_IS707)
    /*----------------------------------------------------------------------
      If any of the WCDMA CS Hdlr signals were set, clear the signals and
      invoke a function to process the signals.
    -----------------------------------------------------------------------*/
    if( (set_sigs & cdma_sigs) != 0 )
    {
      ds707_pkt_process_signals( set_sigs );
    }

#ifdef FEATURE_DATA_IS707_ASYNC    
    if( (set_sigs & async707_sigs) != 0 )
    {
      ds707_async_process_signals( set_sigs );
    }
#endif /* FEATURE_DATA_IS707_ASYNC */    
#endif

#ifdef FEATURE_8960_SGLTE_FUSION
    /*----------------------------------------------------------------------
      If the QCCI Notify signal was set, clear the signal and invoke the
      function to process the signal.
    -----------------------------------------------------------------------*/
    if( (set_sigs & DS_NOTIFY_NAS_SVC_SIG) != 0 )
    {
      /* Check if DSD Sync is enabled */
      if(ds3g_qmi_nas_client_is_dsd_sync_enabled() == TRUE)
      {    
        ds3g_qmi_nas_client_process_signal(set_sigs);
      }
    }
#endif /* FEATURE_8960_SGLTE_FUSION */

#ifdef FEATURE_DATA_ATP_IMS_HTTP
    if ( 0 != (set_sigs & DS_IMS_MSG_SIG) )
    {
      ds_atp_http_process_cmd_q();
    }
#endif /* FEATURE_DATA_ATP_IMS_HTTP */

#ifdef FEATURE_DATA_LTE_ESPRESSO_TEST
    if ( b_first_time_use == TRUE )
    {
      espresso_bring_up_um_iface();
      b_first_time_use = FALSE;
    }
#endif /* FEATURE_DATA_LTE_ESPRESSO_TEST */
  } /* forever */
} /* ds_task() */

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
)
{
  boolean                ret_val = FALSE;
  ds_cmd_process_block  *cmd_process_block_to_insert = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds_cmd_process_register module_id:%d ", module_id);

  cmd_process_block_to_insert = (ds_cmd_process_block*)
                                  q_get(&dsi_cmd_process_free_q);

  if(cmd_process_block_to_insert !=  NULL)
  {
    cmd_process_block_to_insert->ds_module_id = module_id;
    cmd_process_block_to_insert->cb_func_ptr = cb_func_ptr;
    q_put(&dsi_cmd_process_q, &(cmd_process_block_to_insert->link));
    ret_val = TRUE;
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "No Free  buffer to register module_id:%d ", module_id);
    ret_val = FALSE;
  }

  return ret_val;
}/* ds_cmd_process_register() */

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
)
{
  char         *cmd_buf_ptr = NULL;
  ds_cmd_type  *ds_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate memory for DS command type + command payload
  -------------------------------------------------------------------------*/
  cmd_buf_ptr = (char *)modem_mem_alloc((sizeof(ds_cmd_type) + payload_size),
                                        MODEM_MEM_CLIENT_DATA_CRIT);
  if(cmd_buf_ptr == NULL)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_FATAL, 
                    "Can't get cmd buf for DS task. "
                    "Heap Memory Allocation failed for client: %d size: %d" ,
                    MODEM_MEM_CLIENT_DATA_CRIT, (sizeof(ds_cmd_type)));
  }
  else
  {
    ds_cmd_ptr = (ds_cmd_type*)cmd_buf_ptr;
    memset(ds_cmd_ptr , 0, sizeof(ds_cmd_type));
    /*-----------------------------------------------------------------------
      Ensure command payload pointer points to the payload allocated
    -----------------------------------------------------------------------*/
    if(payload_size > 0)
    {
      ds_cmd_ptr->cmd_payload_ptr = (void*)(cmd_buf_ptr + sizeof(ds_cmd_type));
    }
  }

  /*-------------------------------------------------------------------------
    Note that the cmd_ptr may be NULL if there were no free command buffers
    available. The calling task must either handle the NULL return value
    or ERR_FATAL.
  -------------------------------------------------------------------------*/
  return( ds_cmd_ptr );
}/* ds_allocate_cmd_buf() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 modem_mem_free(*cmd_pptr, MODEM_MEM_CLIENT_DATA_CRIT);
 return;
} /* ds_release_cmd_buf() */

/*===========================================================================
FUNCTION      DS_GET_CMD_QUEUE_CNT

DESCRIPTION   This function returns the count of outstanding commands that 
              are in DS command queue.

DEPENDENCIES  None

RETURN VALUE  Value of type uint8 - Indicates number of outstanding commands 
              in DS command queue.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_cmd_queue_cnt( void )
{
  uint8 cmd_q_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_q_cnt = (uint8) q_cnt(&dsi_cmd_q);

  return (cmd_q_cnt);
} /* ds_get_cmd_queue_cnt() */

/*===========================================================================
FUNCTION      DS_GET_CUR_FREE_CMD_CNT

DESCRIPTION   This function returns the current count of free command
              buffers that are available in the DS task.

DEPENDENCIES  None

RETURN VALUE  Returns a uint8 with the number of outstanding commands 
              in the dstask.

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_get_cur_free_cmd_cnt( void )
{
  uint8 free_q_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  free_q_cnt = (uint8) q_cnt(&dsi_cmd_free_q);

  return (free_q_cnt);
} /* ds_get_cur_cmd_cnt() */

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
)
{
  /*-----------------------------------------------------------------------
  Calling task is not DS. Post command in the regular command queue so
  that it gets processed with normal priority.
  -----------------------------------------------------------------------*/
  dsi_put_cmd(cmd_ptr, &dsi_cmd_q);

}/* ds_put_cmd_ext2 () */

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
)
{
  /* All DS internal commands should be sent to internal queue */
#ifdef TEST_FRAMEWORK
  #error code not present
#else
  ds_put_cmd_ext(cmd_ptr);
#endif
} /* ds_put_cmd() */

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
)
{
  if (IS_IN_DS_TASK()) {
    /*-----------------------------------------------------------------------
      Calling task is DS. Post command in the internal command queue so that
      it gets processed with higher priority than external commands.
    -----------------------------------------------------------------------*/
    dsi_put_cmd(cmd_ptr, &dsi_icmd_q);
  } else {
    /*-----------------------------------------------------------------------
      Calling task is not DS. Post command in the regular command queue so
      that it gets processed with normal priority.
    -----------------------------------------------------------------------*/
    dsi_put_cmd(cmd_ptr, &dsi_cmd_q);
  }
} /* ds_put_cmd_ext() */

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
void ds_put_cmd_int
(
  ds_cmd_type  *cmd_ptr                 /* Pointer to command to be sent */
)
{
  if( NULL != cmd_ptr )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "ds_put_cmd_int: Putting cmd %d into DS Internal Cmd "
                    "buffer - i_cmd_q cnt %d",
                    cmd_ptr->hdr.cmd_id,
                    q_cnt(&dsi_icmd_q));
    dsi_put_cmd(cmd_ptr, &dsi_icmd_q);
  }
} /* ds_put_cmd_int() */

/*===========================================================================
FUNCTION      DSI_IS_IN_DS_TASK

DESCRIPTION   Returns TRUE if executing in DS task.

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
boolean dsi_is_in_ds_task( void )
{
  return ( ((rex_self() == ds_tcb_ptr) ? TRUE : FALSE));
} /* dsi_is_in_ds_task() */

/*===========================================================================
FUNCTION  DSI_GET_DS_TCB

DESCRIPTION   Returns DS TCB pointer

RETURN VALUE  ds_tcb_ptr

DEPENDENCIES  Should be called after ds_tcb_ptr has been inited .

SIDE EFFECTS  None
===========================================================================*/
rex_tcb_type* dsi_get_ds_tcb( void )
{
  return ds_tcb_ptr;
} /* dsi_get_ds_tcb() */

/*===========================================================================
FUNCTION      DS_UT_SYNC_CMD_HDLR

DESCRIPTION   UT Sync command

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
void ds_ut_sync_cmd_hdlr( void )
{
  ds_cmd_type  *cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If there is another CMD pending processing, post DS SYNC CMD
    Else send SYNC CNF MSG
  -------------------------------------------------------------------------*/
  if ((q_cnt(&dsi_cmd_q) != 0) || (q_cnt(&dsi_icmd_q) != 0))
  {
    cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    ASSERT(cmd_ptr != NULL);

    cmd_ptr->hdr.cmd_id = DS_CMD_UT_SYNC;
    ds_put_cmd(cmd_ptr);
  }
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST //#ifdef TEST_FRAMEWORK
  else
  {
    dsi_ut_send_sync_cnf();
  }
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
} /* ds_ut_sync_cmd_hdlr() */

