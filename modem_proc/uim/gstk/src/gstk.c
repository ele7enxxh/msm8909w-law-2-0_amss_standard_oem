/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      G S T K     M A I N      T A S K


GENERAL DESCRIPTION

  This source file contains the main GSTK task along with supporting functions.

EXTERNALIZED FUNCTIONS
  gstk_task_put_cmd_buf
    This function put the command onto gstk task command queue

  gstk_cmd
    This function allows clients to put the gstk_cmd onto the GSTK command
    queue.

  gstk_power_down
    This function flushes GSTK queue, vote UIM off and Ack TMC

  gstk_cm_ss_event_cb
    This function listens to CM SS events and will put a GSTK mode change
    command when appropriate

  gstk_test_automation_restart
    Called by test automation script via DIAG to soft restart GSTK

  gstk_switch_to_instance
    This function switches current context data (private data) to a specific
    instance assiciated with specified SIM slot id

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/16   vr      Set gstk_bip_env_in_progress before de-queue of bip env cmd
05/31/16   shr     F3 reduction: Dumping Arrays
05/31/16   gs      Duplicate registration shouldn't return error
05/30/16   gs      F3 improvements
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
05/06/16   gs      F3 logging improvements
12/07/15   lm      Fixed compilation warnings
11/19/15   vr      Update client userdata before sending the sw1_sw2
11/12/15   vr      F3 Logging inprovements
11/05/15   ks      Rel12 polling enhancement
10/28/15   vr      Don't send SW1 report for the clients which didn't send TR
09/25/15   vr      Check pending proactive cmd after CM/QMI ready
08/27/15   vdc     Do not retry service registration if it fails once
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
05/25/15   lm      Updating slot id in ut_gstk_notify
05/25/15   vr      Don't skip CC cmd when CC retry is diabled and when
                   evt dl is getting retried
04/27/15   vr      Non UI and non NW proactive command parallel processing
02/13/15   hh      Split qmi_cat_init into pre and post startup
01/27/15   gm      Retry call control after recovery
12/10/14   vr      Disable CC env during SEND/RECEIVE DATA based on NV config
11/26/14   vr      Handle MMGSDI_CARD_ERROR_EVT thread safe
11/13/14   shr     Fixed compilation warnings
11/12/14   shr     Fixed compilation warnings
11/12/14   vr      Fix for RRC notify is not going to test case
10/29/14   am      Move gstk_cleanup_globals to gstk_task loop?
10/28/14   gs      Removed gstk_check_for_pending_send_sms_term_resp()
10/08/14   bd      Changes to control eHRPD loc info feature with NV
10/08/14   sw      Corrected gstk instance in gstk_send_cell_change_ind()
10/08/14   shr     Memory optmization to the GSTK NV logic
10/08/14   gm      Remove GSTK registration with WMS if FEATURE_ESTK is enabled
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
10/06/14   gm      Added support to send failure indication for OTASP
09/30/14   gs      Use card_protocol to determine length of cell id
09/25/14   bd      Fixed compiler errors for CDMA feature compiled out variant
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/19/14   gm      Added support for SMS_PP data download retry
09/15/14   sw      Defining gstk_env_retry_timer during gstk initialization
09/09/14   vr      Reduce stack usage
09/04/14   sw      Donot reset NAS and CM related variables for REFRESH_NAA_INIT
09/02/14   dy      Adding handling for session changed in non-idle states
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/26/14   gs      Handle the Proactive command registration
08/14/14   vr      Read NV values after gstk instances pointers initialization
08/14/14   vr      Use mmgsdi card slot session to read EF-IMG
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
08/06/14   gm      Removed unused variable
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/29/14   vr      Offtarget support for mmgsdi fetch_allowed
07/23/14   vr      Cleanup GSTK_CLIENT_INIT_EXT function
07/11/14   shr     Add Support for NV Refresh
07/09/14   bd      LTE NMR interface changes
07/08/14   dy      Fixing compiler warnings
07/01/14   kr      Removing tmc from UIM codebase
06/30/14   gm      change name from tp_rpt_time to tp_rpt_timer
06/20/14   gm      Dynamically disable TP related to LTE
06/22/14   gs      Feature flags removed
06/12/14   vr      Initalize LTE message queue in case of TDSCDMA RRC
06/11/14   hn      Remove unused variable
06/09/14   sw      Updating active data sub with NO_TUNEAWAY standby_pref in CM_PH event.
05/28/14   gm      Support of CSG Rel-11 feature
05/28/14   dy      Add GSTK_SUBS_FEATURE_MODE_SRLTE
05/23/14   gm      Verifying TR response report
05/26/14   vr      Memory cleanup in error cases of CM SUBS and CM SS events
05/21/14   kr      Fixed compilation errors when WCDMA and TDS disabled
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
05/19/14   dy      Fix mapping of ds sub id to slot id
05/06/14   gs      GSTK Featurization enhancements and clean up
04/17/14   shr     Fixed logic to update 3GPP2 bmask
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/28/14   kb      Update function assert_on_crit_sect_owner
03/24/14   vr      Split ESTK init handling
03/20/14   sw      Enable GSTK_CFG_FEATURE_ESTK_QMI and GSTK_CFG_FEATURE_REPORT_SW1_SW2
                   in feature_enabled flag if GSTK_CFG_QMI_CAT_CFG is enabled.
03/17/14   gm      Added support for IMS communication control
03/18/14   hn      Start 1x timer only in SVLTE sub feature mode and LTE is
                   reported on CM stack 1 (hybrid stack)
03/11/14   kb      Added TASK_EXIT_SIG to synchronize exiting of all tasks
02/26/14   sw      Memory leak fixes
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/04/14   shr     Fix Subscription Ready caching logic
03/03/14   shr     ESTK BIP clean-up and enhancements
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
02/18/14   gm      Not allow copy if offset is more than max buffer size
02/11/14   gm      Fix KW critical warning
01/31/14   vr      Allow the fetch command based on the fetch status from MMGSDI
01/28/14   kb      Disable function assert_on_crit_sect_owner
01/20/14   sw      Slot to active data subscription mapping
01/21/14   kb      Replace modem_mem api with gstk wrapper
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated gstk_power_down and added gstk_cleanup_globals function
01/09/14   hn      Call wrapper API for segment loading
01/08/14   hn      Block SMS-PP DL when proactive cmd in progress
01/06/14   bd      Fixed a log message and LINT warnings
01/03/14   sw      Update active slot during BIP open channel
12/16/13   bd      Added support for new CM SS event handling
12/12/13   vr      Split GSTK Init pre and post task startup
12/11/13   df      Remove unused memory paging feature
12/06/13   shr     Added support for preferred activation over 1x
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
12/05/13   hn      Support DNS query and Channel reuse
11/01/13   sw      BIP for Multi-slot changes
11/29/13   vr      Enable feature GSTK_CFG_FEATURE_ENH_RAT_AND_LOC_EVT in
                   recovery
11/15/13   vr      Fix TR DELAY timer callback handling for multislot
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
11/12/13   sw      Deregister with watchdog timer while GSTK task power down
11/15/13   sw      Reading cat_version properly from the EFS
11/13/13   shr     Added support for BIP over WLAN
11/01/13   vr      Compilation errors when removing WCDMA feature
10/31/13   vr      Fix for CRM_QTF MOB break
10/31/13   kb      Add offtarget support to check mutex is owned or not
10/29/13   hn      Support segment loading for TDSCDMA/WCDMA
10/31/13   gs      Fixed LAC byte copying issue
10/22/13   jd      Do not modify gstk_curr_inst_ptr within callback, use
                   gstk_instances_ptr[slot - 1] instead
10/21/13   jd      Added slot parameter to gstkutil_update_plmn_lac_cellId_table()
10/18/13   jd      Add missing code when merged CR524417/MPSS2.2 at gstk.c,gstkutil.c
10/04/13   sw      Reduced F3 messages
10/01/13   shr     Fixed KW error
09/23/13   vr      qtf-crm MOB migration
09/20/13   shr     LTE multi-SIM support
09/13/13   gm      Support for Recovery and hot-swap
09/05/13   jd      Track starting position of nmr_data
08/29/13   jd      Featurize LTE dependent functions under FEATURE_LTE
08/27/13   jd      Fix of data card TP issue
08/22/13   vr      Klocwork fixes
08/19/13   hn      Move lte_nmr_ref_id out of FEATURE_LTE and rename it
08/09/13   yt      Deprecate VIRTUIM/VIRTSAT module
08/06/13   vr      Centralized Call Control Changes
06/20/13   gm      Passing proactive_cmd_pending through TAL
08/01/13   bd      Enabled CCP2 TP bit for SIM
07/26/13   gs      Fix to check for HDR personality change
07/26/13   hh      Support for E-UTRAN Inter-frequency measurements
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
07/24/13   gm      Processing cm_ss event when change field is CM_SS_SRV_IND_MASK
07/24/13   sw      Executing location env rsp rcvd confirmation in gstk context
07/23/13   sw      Support for DS callback to obtain RAT information.
07/22/13   bd      Fix for delay in SIM initialization when incorrect value for
                   tp_clients efs file is configured
07/12/13   gs      Removed the gstk_curr_inst_ptr->is_tp_rsp_received=FALSE during the recovery.
07/02/13   kb      Added support for memory leak detection
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
07/01/13   hn      Support SGLTE dual MMs
06/27/13   vr      Handle CM_SS_EVENT for multi SIM hardware
06/27/13   vr      Featurized GSTK num of instances for offtarget
06/25/13   bd      Fix to not set GSTK_ESTK_CMD_SIG unnecessarily during bootup
06/24/13   xz      Support IMS Registration event download
06/24/13   sw      Removed an unnecessary ERROR statement in gstk_cm_stats_event_cb
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Send error response to clients when env ref table is full
06/19/13   hh      Correctly handle GSTK_LOCATION_INFORMATION_IND in all GSTK states
06/19/13   gs      Removed Unused Variables
06/12/13   vr      Fix for Task starvation due to GSTK infinite loop based on
                   queue race condition
06/12/13   gm      Setting GSTK_ESTK_CMD_SIG everytime when command is posted
                   to GSTK task
06/17/13   vr      Don't check as_id to slot_id mapping in single SIM build
06/15/13   bd      Consider SYS_SRV_STATUS_LIMITED_REGIONAL for CM SS event
06/11/13   hh      Fix Toolkit Compiling Errors
05/30/13   akv     Heart beat mechanism for Watchdog
05/29/13   hn      Disable loc_info and its timer at power-up if device is set to CDMA only mode
05/23/13   gm      Fix to compare location info in rrc callback with
                   GSTK loci table
05/24/13   hn      Fix power-up race condition by doing RCINIT handshake later
05/23/13   hh      Fix KW errors
05/21/13   gm      Copy cell_info_type field-by-field
05/20/13   vr      Fix for MOB compilation fail because of memscpy changes
05/17/13   abd     Added support for reading default lang from NV
05/13/13   gm      Changes to make gstk shared variable "gstk_requested_mask"
                   thread safe
05/01/13   vr      Fix command processing logic in GSTK
05/03/13   av      Init gstk tal only once TMC starts creating group6 tasks
04/29/13   gm      Removed unused GSTK function.
04/26/13   gm      Not allowing fetch before TP download
04/12/13   av      Move UIM tasks to group4 of RCINIT
04/11/13   hh      Log envelope command id instead of "BIP Envelope command"
04/11/13   bd      Avoid notifying NAS to ATTACH before IMSI switch
04/09/13   vr      Use safer versions of memcpy() and memmove()
04/02/13   vr      GSTK self client is created from the client table index 0
04/01/13   vr      Clear the terminal response delay timer in recovery handling
03/18/13   bd      Support for client registration in TP wait state
03/05/13   bd      Support for dynamic reg and dereg
03/04/13   bd      Prioritize RRC queue over GSTK queue
02/27/13   gm      Do not overwrite Attach control timer value
02/27/13   gm      Fix compiler warning
02/25/13   xz      Process attach ctrl req when envelope is in retry
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/22/13   kb      Fixed to update MM information
02/19/13   tl      Revert incorrect merge of task signals
02/19/13   av      Merge mmgsdi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13   tl      Move QMI CAT to GSTK context from QMI modem context
02/11/13   bd      Support for (e)HRPD access tech
02/11/13   bd      Task stop and RCINIT registration changes for Dime
02/02/13   bd      Fix to enable FETCH after recovery
01/25/13   hn      Deprecation of FEATURE_UI_NO_TP and FEATURE_THIN_UI
01/22/13   hn      Support 1xRTT CSIM programming retries
01/17/13   hn      Support multiple sessions per subscription
01/08/13   hn      Fix critical Klocwork warning
12/12/12   av      Replace FEATURE_UIM_DEPRECATE_TMC with FEATURE_MODEM_RCINIT
11/29/12   bd      Clean up of redundant members from client table
11/27/12   bd      Fixed incorrect slot reading
11/13/12   bd      Support for Closed Subscriber Group feature
11/06/12   vv      Do not initialize QMI_CAT in FTM mode
10/04/12   hn      Re-merge 'Do not send VPE with all 0s'
10/25/12   hn      Disable proprietary behavior when NV_GPRS_ANITE_GCF_I is set
10/25/12   hn      Update internal access_tech when there is no service on main
                   stack but there is service on hybrid stack
10/19/12   hn      Remove unnecessary write of attach_ctrl_timer to file
10/05/12   yt      Register with RCInit framework for termination signal
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
09/30/12   vs      Update CM serving system info for TAL
09/20/12   yt      Manage concurrency between BIP and OTASP
09/17/12   hk      Register TDSCDMA serving cell change event
09/14/12   hk      Remove Write Protection of gstk_client_table in gstk_process_client_init_req
09/10/12   sg      Read QMI CAT mode when QMI related bits are not set
09/06/12   hk      Update default TP to include USSD string DO support in call control
                   for data card
09/04/12   abd     Added support for additional TR info byte in off-target env
08/18/12   bd      Fixed incorrect cell ID copying
06/21/12   sg      Selectively delete commands on queue during hotswap
08/08/12   sg      Optimization to expedite command processing
08/08/12   sg      Roll back Change default loc state to limited service
08/06/12   sg      Fix starvation when reading IMSI_M failed
08/07/12   tl      Perform TP download using protocol info
07/31/12   bd      Clean up the hack to calculate correct direction for call end event
                   download envelopes
07/30/12   yt      Removed the wait for MM_GSDI: READY event
06/26/12   dd      Add support for SYS_SYS_MODE_TDS
06/12/12   dd      Change default loc state to limited service
06/05/12   sg      Use Hybrid Stack values for LTE
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
05/17/12   nb      Added gstk_get_and_store_curr_app_type and migrated it to
                   MMGSDISESSION
05/17/12   tkl     Fix TDSCDMA RAT in get_curr_cell_id_info
04/23/12   nb      Fix for CSIM refresh
04/16/12   dd      Remove the check for NID being zeros
03/09/12   bd      Enhancements for location information in CC
04/11/12   bd      Fix to update the location status when LTE on main stack is
                   in power save mode and hybrid stack is in full service
04/10/12   dd      Remove dependency on AEEstd.h
04/10/12   dd      Do not send VPE with all 0s
04/05/12   xz      Enable all pro cmd for MOB
04/03/12   av      Replace feature flags with NVITEM
04/03/12   xz      Update default TP to include USSD string DO support in
                   Call Control By USIM
03/30/12   yt      Defeaturized declaration of gstk_get_curr_cell_id_info()
03/21/12   av      Use offtarget nvitem to skip qmi_cat_init for gstk tests
03/21/12   hn      Enable gstk_cm_init() off-target
03/20/12   dd      Delay VPE if sent before TP
03/16/12   yt      Modify reference to gstk_tcb
03/12/12   hn      Fix Q6 compilation error
03/07/12   hn      Add UT GSTK notification for off-target
03/07/12   yt      Move RCInit init function to GSTK context and wait event
                   from MMGSDI before registering
02/28/12   xz      Fix the issue of sending VPE in RRC CB
02/23/12   nmb     QMI QMUX Transition
02/18/12   av      Cleaned up remaining ALWAYS ON/OFF features
02/08/12   nb      ThreadSafe - Remove blocking calls out of critical section
02/07/12   sg      Initialize EFS config read to FALSE
02/06/12   xz      Fix macro define error
01/19/12   bd      Remove unused function gstk_cm_access_tech_cb()
12/20/11   nb      GSDI cleanup
12/05/11   xz      Fix the issue of loc info in CC envelope cmd
11/29/11   dd      Add support for TDSCDMA RAT
11/21/11   sg      Add RRC rat for tracking cell change from LTE vs WCDMA
11/09/11   sg      1)Fix TP bit for Network Reject
                   2)Move TP bit for EUTRAN byte 17 but 7 to estk
11/04/11   dd      Fix data card TP
10/31/11   sg      Class L and Class M support
10/31/11   xz      Fix issue of client reg failure when no SIM
10/20/11   sg      Propagate Code changes for single GSTK support multiple RATs
10/20/11   yt      Added support for RCInit
10/17/11   vv      Support for GSTK UT and fix for memory leak
10/13/11   nb      Full recovery support
10/13/11   nb      Hot Swap Support
10/04/11   xz      Fix the issue of memcmp()
10/03/11   dd      Fix Get Input TP item for data cards
09/30/11   ssr     Removed uim_notify_sys_mode api
09/28/11   dd      Enable E-UTRAN TP field if LTE is enabled
09/28/11   xz      Fix compile error
09/28/11   xz      Featurized declaration of gstk_get_curr_cell_id_info()
09/28/11   xz      Send AcT and Loc envelope when receive SETUP EVT LIST cmd
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
09/14/11   nb      Reverting changes to block polling
09/01/11   xz      Fixed an issue of infinite loop when ENS is enabled
08/22/11   bd      Fixed incorrect message direction in envelope
07/26/11   js      Change to support AcT event processing in TAL
07/25/11   sg      Fix compiler warning
07/13/11   dd      Process SCWS commands immediately
06/27/11   yt      Removed featurization from UIM public headers
06/16/11   sg      Change command id to GSTK_OTASP_CALL_CMD_ERROR in cm_cmd_cb
06/08/11   js      Update GPRS Anite NV after subscription ready.
06/08/11   nb      Read envelope retry NV item upfront ot avoid probable
                   deadlock between NV, GSTK and GSDI
06/03/11   bd      Fixed longitude copying
06/01/11   yt      Reverted defeaturization of gstk_get_curr_cell_id_info()
05/18/11   xz      Use MMGSDI evt to track apps enabled
05/14/11   nb      Thread Safe - Fixed Crash while camping
05/14/11   nb      Thread Safe - Leave critical section
05/14/11   bd      Thread safe changes - protect client table
05/14/11   bd      Thread safe changes - Removed usage of
                   gstk_curr_inst_ptr from gstk_cm_ss_event_cb()
05/14/11   bd      Thread safe changes - Removed the global is_gstk_rrc_registered
05/12/11   xz      Fix issue of infinite loop in when send envelope rsp
05/12/11   xz      Fix an issue of infinite loop while process envelope cmd
05/11/11   nb      Fix for Async TP
05/04/11   xz      Don't send new VPE if envelope retry is in progress
05/03/11   xz      Provide LTE only support
05/02/11   yt      Defeaturized declaration of gstk_get_curr_cell_id_info()
04/29/11   nk      Fixed KW Error
04/27/11   yt      Included private GSDI header
04/26/11   yt      Replaced references to TMC heap with Modem heap
04/25/11   dd      Fix compilation error
04/20/11   xz      Support sending VPE for GSM RAT
04/20/11   ea      Changes for Enable/Disable support
04/13/11   xz      Fix issue of unexpected NO SERVICE envelope cmd
04/11/11   nb      Enable Steering of Roaming support
04/11/11   nb      Added Slot check before accessing globals in event callback
04/08/11   bd      Removed class "K" support in default TP
04/05/11   xz      Changes to support VPE ATTACH CTRL
04/06/11   bd      Removed class "K" support in default TP
03/31/11   js      Fixed merge error
03/23/11   sg      Add GSTK_NW_REJ_EVT_IND for gstk_compare_specific_cmds
03/15/11   sg      Move free of memory from cdma timer cb to gstk context
03/16/11   ssr     Added q_init for gstk_uim_free_q to fix crash
03/11/11   xz      Remove 2nd CCP support in default TP
02/23/11   sg      Call gstk_tal_free_cmd_buf to clean up to fix crash
02/22/11   js      Fix for async processing of TAL commands
02/16/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
02/02/11   dd      Process LTE AcT correctly
02/02/11   sg      Fix overwrite of callback with 0
01/26/11   xz      Fix the issue of eHRPD access tech event in hybrid mode
01/26/10   xz      Add support of HRPD access tech
01/26/11   xz      Fix compiler warning
01/26/11   xz      Set command_id when initiate OTASP call
01/26/11   xz      Reset OTASP call ID if call orig cmd fails
01/25/11   bd      Added a log message
01/24/11   js      Fix GSTK command buffer queue leak
01/20/11   dd      Enable FEATURE_CAT_REL7
01/13/11   xz      Fix the issue of CM triggering (E)UTRAN NO SERVICE event
01/11/11   xz      Fix link error of gstk_task_free_cmd_buf()
01/07/11   nb      While starting DIAG test case, set to default data for
                   instance 1 as DIAG command go onto slot1
01/03/11   sg      Merge error missing definition for cdma location status timer
01/03/11   xz      Fix the issue of wrong ECI
12/30/10   ssr     Dynamically allocate command buffer
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   ps      UIM memory reduction changes
12/17/10   xz      Decouple feature bitmask
12/17/10   ssr     Fixed compilation warnings
12/10/10   sg      Port CDMA location status timer
12/07/10   ssr     Fix for compilation error
11/30/10   yt      Including private UIMDRV and GSTK headers
11/26/10   adp     Fix to clean up event registration table correctly
11/22/10   sg      Fix bug for checking file name by path for IMSI_M
11/18/10   bd      Fixed compilation errors
11/16/10   yt      Migrated to new CM public DS API header file inclusion
11/10/10   nb      Added definition of gsdi_get_nv_ext
11/01/10   js      Fixed GSTK Memory Leak
10/28/10   dd      Fix LTE only build
10/28/10   ssr     Fixed compilation error
10/25/10   mib     Removed free from gstk_power_down()
10/22/10   js      Make GSTK do malloc on modem_heap
10/21/10   dd      Set default time zone to 0xFF until it is received from NW
10/20/10   xz      Fix the issue of multiple location status events
10/20/10   xz      Fix the issue of not send TR in eHRPD only mode
10/18/10   xz      Add support of QCN backup
10/11/10   sg      Port IMSI_M enhancement, multiple OTASP
10/07/10   ms      Fixed Lint errors
10/05/10   bd      Fixed issue of TP DL
09/17/10   yk      Added support dynamic registration on WDOG.
09/13/10   nb      Compilation Fix
09/10/10   ps      Merge UIM Driver DSDS changes
09/08/10   nb      Delay Card Polling till Terminal Profile download
09/02/10   dd      Changes for compilation for LTE only build
09/01/10   xz      Fix compile error
08/30/10   xz      1) Fix compile warning
                   2) Fix issue of handling CARD INSERTED event
08/26/10   dd      Fix TP for M6610
08/24/10   vs      Change in gstk_tal_client_registration() signature
08/19/10   xz      Block OPEN CH until MMGSDI_SUBSCRIPTION_READY_EVT
08/18/10   dd      Correct default TP for ICC
08/13/10   dd      Set correct TP for data cards
08/11/10   js      Fixed issue with TAL remote registration
08/06/10   js      Fixed GSTK TAL command processing bugs
07/30/10   xz      Switch to the instanace that is in a state of processing
                   more command when slot is AUTO
07/28/10   sg      TAL support for envelope commands
07/28/10   xz      Read ENS NV during initialization
07/27/10   sg      Add support for OTASP call for IMSI_M
07/27/10   js      TAL fixes for sending TR
07/26/10   xz      Add support of MM and EMM idle events simultaneously
07/24/10   js      TAL command processing fixes
07/17/10   js      GSTK should not process any other event from GSDI except
                   OK/NOT_OK TO TP DL for fusion remote target
07/16/10   xz      Fix the issue of queuing too many cmd in MMGSDI to read
                   service table
07/15/10   sg      Change RPC callback for TAL argument to pointer
07/14/10   dd      Remove display related events from TP for targets with no UI
07/13/10   dd      Fix compilation error
07/13/10   sg      Feature TAL
07/09/10   dd      Check for CM Sups events
07/09/10   xz      Fix issue of setting LTE RAT
07/07/10   xz      New Call Control API support
07/07/10   mib     Removed mmgsdi_client_id_and_evt_reg from gstk_init
06/29/10   nb      Merged changes for DS DS
06/21/10   xz      Use correct feature mask
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
06/16/10   js      Removed work around for fusion power up.
05/24/10   sg      Send Location information envelope only when it has changed
05/27/10   bd      Leave critical section in case of error
05/25/10   bd      Fixed gstk_get_and_store_curr_app_type to read card protocol
05/23/10   xz      Add support of LTE NMR
05/21/10   xz      1) Add support of network rejection event
                   2) Only use LTE_RRC_SERVICE_IND when service is available
05/20/10   js      Temporary work around for fusion power up
05/20/10   js      Defer mmgsdi client reg till we do tmc_task_init()
04/27/10   xz      Add support of EMM-IDLE event
04/21/10   xz      Fix the issue of copying MNC from RRC event
04/20/10   xz      Add support of LTE-RRC LTE_RRC_SERVICE_IND
04/19/10   sg      Send Loc Info Envelope only on PLMN change
04/09/10   ssr     Merged gobi changes
04/07/10   xz      Fix the issue of delay in FETCH after envelope rsp
03/30/10   xz      Use gstk_get_nv to read IMEI and IMEI_SV
03/18/10   bd      Fixed gstk_process_client_init_req to add GSTK_WAKEUP_TYPE
03/15/10   xz      Add support of saving LTE mode as access tech
03/08/10   sg      Location information envelope command
02/26/10   yt      Fixed KW errors
02/22/10   bd      Support for caching proactive commands
02/19/10   xz      Fix compilation error
02/15/10   nb      Moving Dual Slot Changes
02/10/10   dd      Provide local time and time zone in PLI
02/09/10   xz      IMSI_M provision enhancement
02/04/10   sg      Add global for open channel timeout TR extra parameters
11/23/09   dd      Use GMT time zone in PLI
11/18/09   bd      Support for delaying sending of CC envelope to card
11/10/09   ps      Fix compile issue on 9K
10/29/09   xz      Initialize gstk_curr_location_status to GSTK_NO_SERVICE
10/28/09   nb      Release 7 Enhanced Network Selection
10/12/09   kp      Moved some of the functions under NON_DEAMD_PAGED wrapper
09/26/09   kp      ZI memory reduction changes
09/09/09   sg      Allow client reg and event reg in IDLE state
                   Fix issue of unable to clean up client id
09/07/09   bd      Fix KW issue
07/21/09   dd      Adding Time Zone information
08/11/09   bd      Make changes to include ghdi_nvmem.h
08/07/09   adp     API's being moved from ghdi_nvmem.h to ghdi_nvmem_v.h
07/07/09   sg      Assigned gstk_mmgsdi_client_id inside gstk_mmgsdi_cmd_cb
06/01/09   dd      Added NVMEM registration
05/18/09   sg      Added as argument in gstk_update_default_app_specific_tp
                   void keyword to remove warning
05/08/09   yb      Fixed Lint error
04/20/09   yb      Fixed compilation errors
04/15/09   xz      Support client id clean up of modem restart
04/03/09   xz      Fix dead lock issue in main task loop
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
03/11/09   yb      Fixed Warnings for using 'free' as variable name
02/26/09   gg      Introduced Class B event registration
02/17/09   nb      Fixed Compilation Warning
02/03/09   gg      Fixed lint error
01/23/09   xz      Add gstk_priv_data
01/23/09   yb      Defined terminal response wait timers before clearing them
                   in gstk_util_cmd_details_ref_table_free_slot()
01/19/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
12/17/08   xz      Fix debug message
12/17/08   nb      Common Modem Interface
11/11/08   xz      Fix issue of unable to dereg via DIAG
11/06/08   sk      Fixed error message
10/22/08   sk      Intiialize client tp dl cache
                   Return error to client for client id reg or proactive
                   command reg when not in the right gstk state
                   Removed gstk_terminal_profile_data_from_client
                   Fixed lint errors
10/13/08   yb      Removed Lint Errors
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/29/08   xz      Added support of ESTK for test automation
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/05/08   xz      Change param type of gstk_test_automation_restart() to int
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/29/08   yb      Fixed warning
08/28/08   sk      Fixed enhanced registration with class c and e cmds
08/28/08   yb      Added support for longer text string in terminal response
08/22/08   xz      Added gstk_test_automation_restart() to support test automation
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
08/21/08   sk      Added support for enhanced GSTK registration that includes
                   support for multiple clients to perform terminal profile
                   download and support for clients to register for a subset of
                   proactive command in GSTK or RAW format.
08/08/08   xz      Added support for raw proactive command, raw terminal response
                   and raw envelope command.
                   Added support for processing proactive command timer expiration
05/15/06   sk      Fixed warning
05/06/08   sk      Added support for event download envelope retry
02/06/08   sk      Support for PLI UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/07/07   sk      Added support for NAA Refresh
09/25/07   sk      Changed tp dl bytes to allow alpha and display of extn text
                   support indication to come from the app
08/07/07   sk      Fixed compilation problem and warning
08/01/07   sk      Fixed lint error
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
07/03/07   st      Merged changes from UNDP-1, add interface to DCTSTK
06/27/07   sk      Fixed cell id len value
05/11/07   sp      Featurize for LTK
04/19/07   nk      Added GSTK memcpy function
04/12/07   jk      Added GSTK MALLOC and FREE functions
03/22/07   sk      Featurized header file specific to 1X
11/29/06   sk      Downgraded message from error to high
10/23/06   sk      Fixed lint errors.
08/28/06   jar     Convert a detailed GSDI Error into a generic error.
08/25/06   sk      Featurized uim recovery
08/17/06   sk      Updated Terminal Profile bits
07/31/06   tml     Terminal Profile bit for GPRS Call Control
06/24/06   sk      Lint fixes
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM in LTK.
06/13/06   sk      UIM recovery support
06/06/06   sk      Added support for network search mode
05/11/06   tml     Lint fix
05/03/06   sk      Removed checks for 2G cell id
05/03/06   tml     lint
04/06/06   sk      Updated Terminal Profile
04/04/06   sk      Added gstk_get_nv() to read NV items.
                   Added support for 1X related location information.
11/21/05   sp      Fixed lint errors
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Added protocol info
10/10/05   tml     Added terminal response error support for Access Tech
10/08/05   jar     Add Get Cell ID info function
10/07/05   sk      PLI/LS fixes
10/04/05   sk      Added extra messages in gstk_rrc_cell_change_ind_cb().
                   Also added check for card app type to add appropriate
                   support to extended cell id.
09/30/05   sk      Changed prototype for gstk_rrc_cell_change_ind_cb()
                   Added two new tables gstk_loci_table and rrc_loci_table
                   for maintaining plmn/lac/cell_id info from mm and rrc
                   respectively.
09/26/05   tml     Updated RRC cell id received variable to ensure location
                   status for normal service is only sent when both MM and RRC
                   info are received
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
09/20/05   tml     Terminal profile support update
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
                   Added two new functions gstk_cm_access_tech_cb()
                   and gstk_check_for_pending_pro_cmd()
06/09/05   sun     Check for flag before processing commands
05/11/05   sst     Lint fixes
04/06/05   tml     Removed unnecessary timers for terminal response
11/23/04  sk/tml   fixed compilation warning
11/23/04   jar     Added Features around code to fix compile errors in dual slot
09/02/03   PMD     Added SMS CB Data Download supported in Terminal Profile
08/30/04   tml     Added gstk_profile_received
08/10/04   tml     Added GSTK_SEND_POSTPONE_ENVELOPE signal handling
06/29/04   tml     include ROAM status check for ss cb
06/28/04   tml     added uim notification during cm ss cb
05/11/04   tml     additional SAT/USAT support (timer management, language
                   notification etc)
04/16/04   tml     linted
04/02/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
03/01/04   tml     Merged from branch, fixes include:
                   location status envelope command fix, simplify gstk_wait
                   and initialize terminal profile cache to certain default value
09/13/03   tml     Changed slot_id_type to slot_id_enum_type
09/04/03   tml     Dual Slot support
06/23/03   tml     fixed compiler wanring for 1x/gsm
06/23/03   tml     Used CM_CLIENT_TYPE_GSTK and added default support for
                   command_response
05/16/03   tml     lint
04/17/03   tml     lint and UTK support
03/06/03   tml     Updated enum names
02/22/03   tml     Changed gstk registration with CM to another client type
                   memset the task cmd type before returning to calling functs
02/13/03   tml     Added/Changed DM Messages
02/11/03   tml     Initialize GSTK_CLIENT_ID in the gstk_client_table after
                   other client table initialization
02/07/03 jar/tml   Initial Version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* OS Headers */
#include "uim_variation.h"
#include "rex.h"
#include "customer.h"
#include <stringl/stringl.h>

/* Task Definition Headers */
#include "task.h"

/* Command/Msg Headers/Memory */
#include "err.h"
#include "queue.h"
#include "memheap.h"
#include "uim_msg.h"

/* Task Reporting Headers */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
#include "dog_hb_rex.h"
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* Other Headers */
#include "dsm.h"
#include "customer.h"
#include "string.h"
#include "cm.h"
#include "sys.h"
#include "cm_dualsim.h"

/* GSTK Specific Headers */
#include "gstk_nv_p.h"
#include "gstk.h"
#include "gstki.h"
#include "gstkutil.h"
#include "gstk_state.h"
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "gstk_term_profile.h"
#include "gstk_proactive_cmd.h"
#include "gstk_nv.h"
#ifdef FEATURE_GSTK_TAL
#include "gstk_tal.h"
#endif /* FEATURE_GSTK_TAL */
#ifdef FEATURE_GSM
#include "rr_usat_if.h"
#endif /* FEATURE_GSM */
/* GSTK implementation headers */

/* GSTK - GSDI Interface */
#include "mmgsdisessionlib_v.h"

#ifdef FEATURE_ESTK
#include "estk.h"
#endif /* FEATURE_ESTK */

/* For UNIT Testing Only */
#include "uim_p.h"

#include "err.h"

#ifdef FEATURE_LTE
#include "msgr_rex.h"
#endif /*FEATURE_LTE*/

#include "modem_mem.h"

#ifdef FEATURE_WCDMA
#include "rrcmmif.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "tdsrrcmmif.h"
#endif /*FEATURE_TDSCDMA*/

#include "rcevt_rex.h"
#include "rcinit.h"
#include "qmi_cat.h"

#include "fs_lib.h"
#include "fs_public.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* num_report(1 byte), size (2 bytes), freq bytes (4 bytes) */
#define GSTK_LTE_NMR_INTRA_FREQ_MIN_BUF_SIZE 7

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_SEGMENT_LOADING
/* Segment Loading headers */
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#endif /* FEATURE_SEGMENT_LOADING */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

gstk_shared_data_type        gstk_shared_data;

#if defined FEATURE_UIM_SUPPORT_TRIPLE_SLOTS
gstk_priv_data_type          gstk_instances[3];
#elif defined FEATURE_UIM_SUPPORT_DUAL_SLOTS
gstk_priv_data_type          gstk_instances[2];
#else
gstk_priv_data_type          gstk_instances[1];
#endif /* FEATURE_UIM_SUPPORT_TRIPLE_SLOTS */
gstk_priv_data_type         *gstk_instances_ptr[GSTK_MAX_NUM_OF_INSTANCES];
gstk_priv_data_type         *gstk_curr_inst_ptr;

/* command details reference table */
gstk_cmd_details_ref_table_type   command_details_ref_table[GSTK_MAX_PROACTIVE_COMMAND];
gstk_cmd_details_ref_table_type*  command_details_ref_table_ptr = command_details_ref_table;

/* Queue to hold commands for gstk */
q_type gstk_task_cmd_q;
/* Queue to hold commands for gstk */
q_type gstk_task_priority_cmd_q;

/*  GSTK Dog variables */
dog_report_type gstk_dog_rpt_var = 0; /* Initial Safety */

#ifdef FEATURE_SEGMENT_LOADING
/* Interface pointers for dynamically-loaded modules TDSCDMA/WCDMA */
interface_t *gstk_pi_tdscdma = NULL;
interface_t *gstk_pi_wcdma = NULL;
#endif /* FEATURE_SEGMENT_LOADING */

#if defined(FEATURE_WCDMA)
void gstk_rrc_cell_change_ind_cb (cell_info_type cell_info);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
void gstk_tdscdma_rrc_cell_change_ind_cb(tdscell_info_type cell_info);
#endif /*FEATURE_TDSCDMA*/

#if (defined(FEATURE_WCDMA) || defined(FEATURE_LTE))
void gstk_send_cell_change_ind(
  gstk_cell_info_type              cell_info,
  gstk_access_technology_type      rat
);
#endif /* (FEATURE_WCDMA) || (FEATURE_LTE) */

/* Specifically used for Event download envelope retry and BIP envelop check*/
static int gstk_compare_specific_cmds(void *item_ptr,void *param);

#if defined(FEATURE_ESTK)
extern void estk_gstk_init_cb (
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
);
#endif /* FEATURE_ESTK */

void gstk_mmgsdi_events_cb(const mmgsdi_event_data_type *evt_ptr);

static gstk_status_enum_type gstk_copy_access_tech (
  gstk_access_technology_type*           access_tech_ptr,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr
);

static gstk_status_enum_type gstk_copy_location_status (
  gstk_location_status_enum_type* loc_status_ptr,
  const sys_srv_status_e_type     srv_status
);

static void gstk_copy_csg_info(
  gstk_task_cmd_type*                     task_cmd_ptr,
  const cm_mm_msim_ss_stack_info_s_type*  cm_stack_info_ptr
);

static gstk_status_enum_type gstk_ds_event_reg_sys_status_change(void);

static gstk_status_enum_type gstk_ds_event_reg_wlan_mac_addr_change(void);

static gstk_status_enum_type gstk_ds_event_reg_wlan_ssid_change(void);

static gstk_status_enum_type gstk_ds_sys_init(void);

static void gstk_process_ds_system_status_ex_ind_wlan_status(
  gstk_cmd_type *cmd_ptr
);

static void gstk_cache_ds_system_status_ex_ind_process_cellular_status(
  gstk_cmd_type *cmd_ptr
);

static void gstk_process_ds_system_status_ex_ind(
  gstk_cmd_type *cmd_ptr
);

static void gstk_process_ds_wlan_mac_addr_change_ind(
  gstk_cmd_type *cmd_ptr
);

static void gstk_process_ds_wlan_ssid_change_ind(
  gstk_cmd_type *cmd_ptr
);

static gstk_status_enum_type gstk_copy_subs_feature_mode(
  gstk_subs_feature_mode_enum_type *subs_feature_mode_ptr,
  const sys_subs_feature_t          cm_subs_feature_mode
);

static void gstk_build_cm_subs_mode_cmd(
  const cm_subs_info_s_type *subs_info_ptr
);

static void gstk_tp_config_init( void );

static void gstk_msgr_init(void);

#if defined(FEATURE_GSTK_FUSION_REMOTE)
#define GSTK_MCC_LIST_SIZE    0x0C
#endif /* FEATURE_GSTK_FUSION_REMOTE */

#ifdef FEATURE_LTE
#define STD_OFFSETOF(type,member)     (((char*)(&((type*)1)->member))-((char*)1))
#endif /* FEATURE_LTE */

#ifdef FEATURE_LTE
/*===========================================================================
FUNCTION   gstk_nmr_status_lte_to_gstk

DESCRIPTION
  The function convert lte_rrc_nmr_info_statue_e to gstk_utran_status_enum_type

PARAMETERS
  status:       result status from RRC

DEPENDENCIES
  None

RETURN VALUE
  gstk_utran_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_utran_status_enum_type gstk_nmr_status_lte_to_gstk(
   lte_rrc_nmr_info_statue_e status)
{
  switch (status)
  {
  case LTE_RRC_NMR_INFO_SUCCESS:
    return GSTK_UTRAN_NMR_RESULT_SUCCESS;
  case LTE_RRC_NMR_INFO_FAIL_LTE_INACTIVE:
    return GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN;
  case LTE_RRC_NMR_INFO_FAIL_UE_NOT_CAMPED:
    return GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS;
  case LTE_RRC_NMR_INFO_FAIL_OTHER:
    return GSTK_UTRAN_NMR_RESULT_FAIL_OTHER;
  default:
    return GSTK_UTRAN_NMR_RESULT_INVALID;
  }
}/*gstk_nmr_status_lte_to_gstk*/

/*===========================================================================
FUNCTION   gstk_nmr_type_lte_to_gstk

DESCRIPTION
  The function convert lte_rrc_nmr_info_statue_e to gstk_utran_status_enum_type

PARAMETERS
  reportType:       report type from RRC

DEPENDENCIES
  None

RETURN VALUE
  gstk_nmr_utran_qualifier_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_nmr_utran_qualifier_enum_type gstk_nmr_type_lte_to_gstk(
   lte_rrc_nmr_report_type_e report_type)
{
  switch (report_type)
  {
  case LTE_RRC_NMR_REPORT_NONE:
    return GSTK_NMR_NONE_MEAS;
  case LTE_RRC_NMR_REPORT_INTRA_FREQ:
    return GSTK_NMR_LTE_INTRA_FREQ_MEAS;
  case LTE_RRC_NMR_REPORT_INTER_FREQ:
    return GSTK_NMR_LTE_INTER_FREQ_MEAS;
  case LTE_RRC_NMR_REPORT_IRAT_WCDMA:
    return GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS;
  case LTE_RRC_NMR_REPORT_IRAT_GERAN:
    return GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS;
  case LTE_RRC_NMR_REPORT_IRAT_CDMA2K:
    /* Need to change once LTE support for LTE_RRC_NMR_REPORT_IRAT_CDMA2K is available */
  default:
    return GSTK_NMR_NONE_MEAS;
  }
}/*gstk_nmr_type_lte_to_gstk*/

/*===========================================================================
FUNCTION   gstk_process_rrc_msgr_cmd

DESCRIPTION
  The function processes the RRC messages coming from message queue

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_rrc_msgr_cmd()
{
  gstk_rrc_msg_type           *msg_ptr      = NULL;
  boolean                     first_rrc_msg = TRUE;
  uint32                      i             = 0;
  uint32                      cell_info_s   = sizeof(gstk_cell_info_type);
  gstk_cell_info_type         cell_info;
  gstk_cell_info_type         prev_cell_info;
  gstk_utran_status_enum_type nmr_status    = GSTK_UTRAN_NMR_RESULT_INVALID;
  /* LTE RRC Instance ID, by default set to 1 (i.e. AS_ID_1) */
  uint8                       inst_id       = 1;
  dsm_item_type               *dsm_ptr      = NULL;
  uint8                       *nmr_data     = NULL;
  uint8                       *tmp_ptr      = NULL;
  gstk_nmr_utran_qualifier_enum_type nmr_utran_qualifier = GSTK_NMR_NONE_MEAS;

  memset(&prev_cell_info, 0x00, sizeof(prev_cell_info));
  memset(&cell_info, 0x00, cell_info_s);

  while((msg_ptr = (gstk_rrc_msg_type *)q_get(
                     &gstk_shared_data.rrc_msg_q)) != NULL)
  {
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif

    switch (msg_ptr->rrc_msgr_cmd.msg_hdr.id)
    {
    case LTE_RRC_SERVICE_IND:
      UIM_MSG_HIGH_1("LTE_RRC_SERVICE_IND received. svc_status = 0x%x",
                     msg_ptr->rrc_msgr_cmd.rrc_service_ind.svc_status);

      if (msg_ptr->rrc_msgr_cmd.rrc_service_ind.svc_status ==
          LTE_RRC_SVC_STATUS_AVAILABLE)
      {
        cell_info.cell_id =
          msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.cell_identity;
        cell_info.plmn_id.num_mnc_digits =
          uint32touint8(msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.selected_plmn.num_mnc_digits);
        for (i = 0; i < GSTK_RRC_MAX_MNC_DIGITS; ++i)
        {
          cell_info.plmn_id.mnc[i] =
            uint32touint8(
              msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.selected_plmn.mnc[i]);
        }
        for (i = 0; i < GSTK_RRC_MAX_MCC_DIGITS; ++i)
        {
          cell_info.plmn_id.mcc[i] =
            uint32touint8(
              msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.selected_plmn.mcc[i]);
        }
        (void)gstk_memcpy(cell_info.lac,
                          &msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.tac,
                          sizeof(msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.tac),
                          GSTK_RRC_MAX_GSMMAP_LAC_LENGTH,
                          sizeof(msg_ptr->rrc_msgr_cmd.rrc_service_ind.camped_svc_info.tac));

        /* Retrieve AS_ID by converting the Instance ID provided the LTE RRC */
        inst_id = MSGR_GET_INST(&msg_ptr->rrc_msgr_cmd.msg_hdr);
        cell_info.as_id = INST_ID_TO_SYS_AS_ID(inst_id);

        if (first_rrc_msg ||
            memcmp(&prev_cell_info, &cell_info, cell_info_s) != 0)
        {
          gstk_send_cell_change_ind(cell_info, GSTK_ACCESS_TECH_LTE);
          first_rrc_msg = FALSE;
          (void)gstk_memcpy(&prev_cell_info,
                            &cell_info,
                            cell_info_s,
                            cell_info_s,
                            cell_info_s);
        }
      }
      break;
    case LTE_RRC_NMR_INFO_CNF:
      UIM_MSG_HIGH_2("LTE_RRC_NMR_INFO_CNF: status = 0x%x report_type = 0x%x",
                     msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.status,
                     msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.report_type);
      nmr_status = gstk_nmr_status_lte_to_gstk(msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.status);
      nmr_utran_qualifier = gstk_nmr_type_lte_to_gstk(msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.report_type);
      i = 0;
      switch (nmr_status)
      {
      case GSTK_UTRAN_NMR_RESULT_SUCCESS:
        msgr_get_dsm_attach(&msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.dsm_attach,
                            &dsm_ptr);
        if(dsm_ptr)
        {
          if((i = dsm_length_packet(dsm_ptr)) > 0)
          {
            tmp_ptr = nmr_data = (uint8*)GSTK_CALLOC(i);
            if ((nmr_data != NULL) && (i == dsm_pullup(&dsm_ptr, nmr_data, (uint16)i)))
            {
              gstk_util_dump_byte_array("nmr_data", nmr_data, i);

              if (LTE_RRC_NMR_REPORT_INTRA_FREQ == msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.report_type)
              {
                /* Skip num_report(1 byte), size (2 bytes), freq bytes (2 bytes)
                   and just to pick up the measurement result */
                i -= GSTK_LTE_NMR_INTRA_FREQ_MIN_BUF_SIZE;
                nmr_data += GSTK_LTE_NMR_INTRA_FREQ_MIN_BUF_SIZE;
              }
            } /* if (nmr_data != NULL) && (i == dsm_pullup ... */
            else
            {
              UIM_MSG_ERR_2("Unable to retrieve full nmr_data, len: 0x%x, nmr_data: 0x%x",
                            i, nmr_data);
              i = 0;
              if (nmr_data)
              {
                gstk_free((void *)nmr_data);
                tmp_ptr = nmr_data = NULL;
              }
            }
          } /* if((i = dsm_length_packet(dsm_ptr)) > 0) */
          else
          {
            UIM_MSG_ERR_1("Invalid DSM Packet length %d", i);
          }
        }/*dsm_ptr != NULL*/
        msgr_clear_attach(&msg_ptr->rrc_msgr_cmd.rrc_nmr_cnf.dsm_attach);
      case GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN:
      case GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS:
      case GSTK_UTRAN_NMR_RESULT_FAIL_ASID_MISMATCH:
      case GSTK_UTRAN_NMR_RESULT_FAIL_OTHER:
        gstk_queue_rrc_utran_nmr_rsp_ext(nmr_status,
                                         i,
                                         nmr_data,
                                         &gstk_shared_data.nmr_ref_id,
                                         nmr_utran_qualifier);

        break;
      default:/*GSTK_UTRAN_NMR_RESULT_INVALID*/
        break;
      }
      gstk_free((void *)tmp_ptr);
      tmp_ptr = nmr_data = NULL;
      break;
    default:
      UIM_MSG_ERR_1("RRC msg 0x%x is unhandled!",
                    msg_ptr->rrc_msgr_cmd.msg_hdr.id);
    } /* switch (msg_ptr->rrc_msgr_cmd.msg_hdr.id) */
    (void)q_link(msg_ptr, &msg_ptr->link);
    q_put(&gstk_shared_data.rrc_msg_q_free, &msg_ptr->link);
  }
} /*gstk_process_rrc_msgr_cmd*/
#endif /*FEATURE_LTE*/

/*===========================================================================
FUNCTION   gstk_cm_ss_event_cb

DESCRIPTION
  Serving system (SS) event callback function, called when serving system
  information is changed. This function copies the required information
  and puts the command in the GSTK command queue.

PARAMETERS
  ss_event: [Input] CM serving system event
  ss_info_ptr: [Input] Pointer to the ss info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_ss_event_cb (
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type*  ss_info_ptr
)
{
  int                     i                    = 0;
  gstk_task_cmd_type*     task_cmd_ptr         = NULL;
  gstk_slot_id_enum_type  slot                 = GSTK_SLOT_1;
  uint64                  changed_fields_mask  = 0;
  uint64                  changed_fields_mask2 = 0;
  gstk_status_enum_type   gstk_status          = GSTK_SUCCESS;
  gstk_nv_items_data_type nv_data;

  if (ss_info_ptr == NULL || ss_event != CM_SS_EVENT_SRV_CHANGED)
  {
    UIM_MSG_ERR_2("gstk_cm_ss_event_cb(): 0x%x 0x%d ", ss_info_ptr, ss_event);
    return;
  }

  if (gstk_io_ctrl (GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                    &ss_info_ptr->asubs_id,
                    &slot) != GSTK_SUCCESS)
  {
    return;
  }
  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            slot,
                            &nv_data);

  GSTK_SET_CM_SS_MODE_CHANGE_MASK(changed_fields_mask, nv_data.cat_version);
  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, slot) == TRUE)
  {
    GSTK_SET_CM_SS_MODE_CHANGE_MASK2(changed_fields_mask2, nv_data.cat_version);
  }

  for(i = 0; i < GSTK_CM_NUM_STACKS; ++i)
  {
    if (!(ss_info_ptr->stack_info[i].changed_fields & changed_fields_mask) &&
        !(ss_info_ptr->stack_info[i].changed_fields2 & changed_fields_mask2))
    {
      continue;
    }
    task_cmd_ptr = gstk_task_get_cmd_buf();

    if (NULL == task_cmd_ptr)
    {
      break;
    }
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_group =
      GSTK_MODE_CHANGE_IND;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_id =
      GSTK_MODE_CHANGE_FROM_CM;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.sim_slot_id = slot;
    break;
  } /* for(i = 0 ... */

  for (i = 0; NULL != task_cmd_ptr && i < GSTK_CM_NUM_STACKS; ++i)
  {
    if (GSTK_SUCCESS != (gstk_status = gstk_copy_access_tech(
                           &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].access_tech,
                           &ss_info_ptr->stack_info[i])))
    {
      break;
    }
    if (GSTK_SUCCESS != (gstk_status = gstk_copy_location_status(
                           &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].loc_status,
                           ss_info_ptr->stack_info[i].srv_status)))
    {
      break;
    }
#ifdef FEATURE_CDMA
    if (SYS_SYS_MODE_CDMA == ss_info_ptr->stack_info[i].sys_mode)
    {
      if(GSTK_SUCCESS != (gstk_status = gstk_copy_1x_info_from_cm(
                          &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].loc_info.cdma_ext,
                          &ss_info_ptr->stack_info[i])))
      {
        UIM_MSG_ERR_0("Copy 1x info from CM is failed!");
        break;
      }
      if (ss_info_ptr->stack_info[i].changed_fields2 & CM_SS_EVT_POSITION_INFO_MASK)
      {
        task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].advance_info_changed = TRUE;
      }
    }
    if (SYS_SYS_MODE_HDR == ss_info_ptr->stack_info[i].sys_mode)
    {
      if(GSTK_SUCCESS != (gstk_status = gstk_copy_hdr_info_from_cm(
                          &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].loc_info.cdma_ext,
                          &ss_info_ptr->stack_info[i])))
      {
        UIM_MSG_ERR_0("Copy HDR info from CM is failed!");
        break;
      }
    }
#endif /* FEATURE_CDMA */
  } /* for (i = 0 ... */
  if (NULL != task_cmd_ptr)
  {
    if (GSTK_SUCCESS == gstk_status)
    {
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      task_cmd_ptr = NULL;
    }
  }

  /* Time zone */
  for(i = 0, task_cmd_ptr = NULL; i < GSTK_CM_NUM_STACKS; ++i)
  {
    if (!(ss_info_ptr->stack_info[i].changed_fields & CM_SS_MOBILITY_MGMT_MASK ||
          ss_info_ptr->stack_info[i].changed_fields & CM_SS_CDMA_TIME_CHGD_MASK))
    {
      /* Just continue as neither of the interested fields are set */
      continue;
    }
    task_cmd_ptr = gstk_task_get_cmd_buf();

    if (NULL == task_cmd_ptr)
    {
      break;
    }
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_group =
      GSTK_TIME_ZONE_CHANGE_IND;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_id = 0; /* not used */
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.sim_slot_id = slot;
    break;
  } /* for(i = 0 ... */

  for (i = 0; NULL != task_cmd_ptr && i < GSTK_CM_NUM_STACKS; ++i)
  {
    gstk_access_technology_type access_tech = GSTK_ACCESS_NONE;

    if (GSTK_SUCCESS != (gstk_status = gstk_copy_access_tech(
                           &access_tech,
                           &ss_info_ptr->stack_info[i])))
    {
      break;
    }
    task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].access_tech = access_tech;
    switch(access_tech)
    {
      case GSTK_ACCESS_TECH_GSM:
      case GSTK_ACCESS_TECH_UTRAN:
      case GSTK_ACCESS_TECH_LTE:
        if (ss_info_ptr->stack_info[i].changed_fields & CM_SS_MOBILITY_MGMT_MASK)
        {
          UIM_MSG_HIGH_3("Access Tech: 0x%x, time_zone_avail: 0x%x, tz: %d",
                         access_tech,
                         ss_info_ptr->stack_info[i].mode_info.gw_info.mm_information.time_zone_avail,
                         ss_info_ptr->stack_info[i].mode_info.gw_info.mm_information.time_zone);
          if (ss_info_ptr->stack_info[i].mode_info.gw_info.mm_information.time_zone_avail)
          {
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_AVAILABLE;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz =
              ss_info_ptr->stack_info[i].mode_info.gw_info.mm_information.time_zone;
          }
          else
          {
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_NOT_AVAILABLE;
          }
        }
        break;
      case GSTK_ACCESS_TECH_CDMA:
      case GSTK_ACCESS_TECH_HRPD:
      case GSTK_ACCESS_TECH_EHRPD:
        if (ss_info_ptr->stack_info[i].changed_fields & CM_SS_CDMA_TIME_CHGD_MASK)
        {
          UIM_MSG_HIGH_1("Access Tech: 0x%x - Marking TZ as available",
                         access_tech);
          task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_AVAILABLE;
          /* Assign tz to 0xFF, as GSTK doesnt use it in 3GPP2 */
          task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz = 0xFF;
        }
        break;
      default:
        UIM_MSG_HIGH_0("TZ info not available");
        task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_NOT_AVAILABLE;
        task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz = 0xFF;
        break;
    }
  }
  if (NULL != task_cmd_ptr)
  {
    if (GSTK_SUCCESS == gstk_status)
    {
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      task_cmd_ptr = NULL;
    }
  }

  /* Check for CSG information change. CSG cell can be camped only on main stack */
  if (ss_info_ptr->stack_info[0].changed_fields & (CM_SS_CSG_INFO_CHANGED_MASK))
  {
    if (nv_data.cat_version >= GSTK_CFG_CAT_VER9)
    {
       gstk_access_technology_type access_tech = GSTK_ACCESS_NONE;

       UIM_MSG_HIGH_1("CM CSG INFO CHANGE: sys_mode = %d",
                      ss_info_ptr->stack_info[0].sys_mode);
      /* Queue the changed CSG information for further processing */
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL)
      {
        task_cmd_ptr->cmd.csg_change_ind.message_header.command_group =
          GSTK_CM_CSG_CHANGE_IND;
        task_cmd_ptr->cmd.csg_change_ind.message_header.sim_slot_id = slot;
        /* At the moment CSG is maintained only on main stack. It's
           currently not allowed on any of the hybrid stacks */
        if(ss_info_ptr->stack_info[0].srv_status == SYS_SRV_STATUS_SRV)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped = TRUE;
          if (GSTK_SUCCESS == gstk_copy_access_tech(
                                               &access_tech,
                                               &ss_info_ptr->stack_info[0]))
          {
            task_cmd_ptr->cmd.csg_change_ind.cmd.access_tech = access_tech;
            gstk_copy_csg_info(task_cmd_ptr, &ss_info_ptr->stack_info[0]);
            if(task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id == SYS_CSG_ID_INVALID)
            {
              task_cmd_ptr->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped = FALSE;
            }
            if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.plmn_info,
                                  ss_info_ptr->stack_info[0].sys_id.id.plmn.identity,
                                  3,
                                  3,
                                  3) < 3)
            {
              /* Proper PLMN id is not copied */
            }
            gstk_task_put_cmd_buf(task_cmd_ptr);
          }
          else
          {
           gstk_task_free_cmd_buf(task_cmd_ptr);
           task_cmd_ptr = NULL;
          }
        }
        else
        {
           task_cmd_ptr->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped = FALSE;
           gstk_task_put_cmd_buf(task_cmd_ptr);
        }
      }
    }
  }
} /* gstk_cm_ss_event_cb */

/*===========================================================================
FUNCTION GSTK_WAIT

DESCRIPTION
  Main wait routine for GSTK task.

PARAMETERS
  requested_mask [Input]: input signal to be waited on

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals).  The calling unit can then check for the reason for
  exiting the wait.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
LOCAL rex_sigs_type gstk_wait
(
  rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */
  boolean       gstk_is_specific_signal = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------
  ** Check message queue:  If the request mask contains a check for
  ** GSTK_TASK_CMD_Q_SIG then we should check the queue, and set in
  ** the queue_mask an indication that something is waiting to be
  ** serviced on that queue.  The rex signal is also cleared
  ** regardless of the queue count.  This is to cover the case
  ** where there is a signal set but nothing on the queue.
  ** ------------------------------------------------------------- */
  if(requested_mask == gstk_shared_data.gstk_requested_mask)
  {
    gstk_is_specific_signal = FALSE;
  }

  if (requested_mask & GSTK_TASK_CMD_Q_SIG)
  {
    (void) rex_clr_sigs( UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG );

    if ((q_cnt( &gstk_task_cmd_q )
        && gstk_shared_data.gstk_continue_task
       ) || (q_cnt( &gstk_task_priority_cmd_q )) )
    {
      (void) rex_set_sigs(UIM_GSTK_TCB,GSTK_TASK_CMD_Q_SIG);
    }
  }

  /* --------------------------------------------------------------------
  ** Loop until the current signals from REX (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  ** -------------------------------------------------------------------- */
  do
  {
    if(gstk_is_specific_signal == FALSE)
    {
      requested_mask = gstk_shared_data.gstk_requested_mask;
    }

    rex_signals_mask = rex_wait( requested_mask | GSTK_RPT_TIMER_SIG );

      /* DOG HeartBeat
         1. A single shared timer resource among all clients is used to register for dog.
         2. Dog will send the signals to the registered tasks (“ping instead of going through timer,
            through dog’s own heartbeat, and the tasks will respond to wdog (“pong
            Voluntary dog reporting is supported (pongs without pings).
         3. Hence we use dog_hb_report() to report to the dog and in turn pet it.
      */
    (void) rex_clr_sigs( rex_self(), GSTK_RPT_TIMER_SIG );
    dog_hb_report(gstk_dog_rpt_var);

    /* ---------------------------------------------------------------
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue indicated
    ** --------------------------------------------------------------- */
    if (rex_signals_mask & GSTK_TASK_CMD_Q_SIG)
    {
      if ((q_cnt(&gstk_task_cmd_q) == 0) &&   /* No elements in queue ? */
          (q_cnt(&gstk_task_priority_cmd_q) == 0))
      {
        (void) rex_clr_sigs( UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG );
        rex_signals_mask &= ~GSTK_TASK_CMD_Q_SIG;  /* Turn off bit */
      }
    }

  /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);

  return (rex_signals_mask);

} /* end gstk_wait() */

/*=============================================================================
FUNCTION: gstk_cm_cmd_cb

DESCRIPTION:
  Command callback from CM to report the status of queuing a request to CM

PARAMETERS:
  *data_block_ptr : [Input]  estk data block pointer
  call_cmd        : [Input]  cm command type
  call_cmd_err    : [Input]  cm command error type
  call_cmd_err_cause : [Input]  cm command error cause
  alpha              : [Input]  cm modified alpha

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void gstk_cm_cmd_cb (
  void                   *data_block_ptr,
  cm_call_cmd_e_type     call_cmd,
  cm_call_cmd_err_e_type        call_cmd_err,
  cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
  cm_alpha_s_type               alpha
)
{
  gstk_priv_data_type *gstk_inst_ptr = NULL;
  uint32              i              = 0;
  gstk_task_cmd_type  *task_cmd_ptr  = NULL;

  UIM_MSG_HIGH_2("gstk_cm_cmd_cb(): 0x%x, 0x%x", call_cmd, call_cmd_err);

  gstk_inst_ptr = (gstk_priv_data_type *)data_block_ptr;
  for (i = 0; i < gstk_shared_data.gstk_num_of_instances ; ++ i)
  {
    if (gstk_inst_ptr == gstk_instances_ptr[i])
      break;
  }
  if (i == gstk_shared_data.gstk_num_of_instances)
  {
    UIM_MSG_ERR_0("invalid gstk instance ptr!");
    return;
  }

  if (call_cmd == CM_CALL_CMD_ORIG
      &&
      call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL)
      {
        task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
        task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_CMD_ERROR;
        task_cmd_ptr->cmd.terminal_profile_cmd.message_header.user_data = call_cmd_err;
        gstk_task_put_cmd_buf(task_cmd_ptr);
      }
  }
} /*gstk_cm_cmd_cb*/

#if defined(FEATURE_GSTK_FUSION_REMOTE)
/*===========================================================================
FUNCTION GSTK_MMGSDI_IMSI_VERIFICATION_CB

DESCRIPTION
  GSTK Callback for IMSI READ Operation with MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_imsi_verification_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  uint8                   imsi_len       = 0xFF;
  uint8                  *imsi_data_ptr  = NULL;
  gstk_mcc_data_type      imsi_mcc;
  boolean                 mcc_match      = FALSE;
  uint8                   i              = 0;
  gstk_nv_items_data_type nv_data;
  gstk_mcc_data_type      gstk_reserved_mcc_tbl[GSTK_MCC_LIST_SIZE] =
  {
    {0x00, 0x00, 0x01}, {0x00, 0x00, 0x02}, {0x00, 0x00, 0x03},
    {0x00, 0x00, 0x04}, {0x00, 0x00, 0x05}, {0x00, 0x00, 0x06},
    {0x00, 0x00, 0x07}, {0x00, 0x00, 0x08}, {0x00, 0x00, 0x09},
    {0x00, 0x01, 0x00}, {0x00, 0x01, 0x01}, {0x00, 0x01, 0x02}
  };



  UIM_MSG_HIGH_3("gstk_mmgsdi_imsi_verification_cb status 0x%x, cnf 0x%x, cnf_ptr 0x%x",
                 status, cnf, cnf_ptr);

  if((status != MMGSDI_SUCCESS) || (cnf_ptr == NULL) || (cnf != MMGSDI_READ_CNF))
  {
    UIM_MSG_ERR_0("Invalid result");
    return;
  }

  memset(&imsi_mcc, 0x00, sizeof(gstk_mcc_data_type));
  /* Validate IMSI Data */
  imsi_data_ptr = cnf_ptr->read_cnf.read_data.data_ptr;
  if((imsi_data_ptr == NULL) || (cnf_ptr->read_cnf.read_data.data_len == 0))
  {
    UIM_MSG_ERR_2("Invalid data returned imsi_data_ptr 0x%x, len 0x%x",
                  imsi_data_ptr, cnf_ptr->read_cnf.read_data.data_len);
    return;
  }

  imsi_len = imsi_data_ptr[0];

  /* Is IMSI Provisioned */
  if ( imsi_len == GSDI_IMSI_NOT_PROVISIONED )
  {
    UIM_MSG_ERR_1("IMSI Not Provisionsed, 0x%x", imsi_len);
    return;
  }
  else if (( imsi_len < GSDI_MIN_VALID_IMSI_LEN ) ||
           ( imsi_len > GSDI_MAX_VALID_IMSI_LEN - 1))
  {
    UIM_MSG_ERR_3("IMSI Len 0x%x less or greater than ME MIN 0x%x, MAX 0x%x",
                  imsi_len,
                  GSDI_MIN_VALID_IMSI_LEN,
                  GSDI_MAX_VALID_IMSI_LEN - 1);
    return;
  }

  /* -----------------------------------------------------------
    Parse MCC from the IMSI:
    B0     B1       B2       B3       B4     B5   B6   B7    B8
    LEN  MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3  D3D2 D5D4 D7D6  D9D8
  -------------------------------------------------------------*/
  imsi_mcc.mcc1 = imsi_data_ptr[1] & 0xF0;
  imsi_mcc.mcc1 >>= 4;
  imsi_mcc.mcc2 = imsi_data_ptr[2] & 0x0F;
  imsi_mcc.mcc3 = imsi_data_ptr[2] & 0xF0;
  imsi_mcc.mcc3 >>= 4;

  UIM_MSG_HIGH_3("MCC value in the IMSI: %x%x%x",
                 imsi_mcc.mcc1, imsi_mcc.mcc2, imsi_mcc.mcc3);

  /* Compare MCC against the reserved MCC values */
  for(i = 0; i < GSTK_MCC_LIST_SIZE; i++)
  {
    if(memcmp(&imsi_mcc,
              &gstk_reserved_mcc_tbl[i],
              sizeof(gstk_mcc_data_type)) == 0)
    {
      mcc_match = TRUE;
      break;
    }
  }

  UIM_MSG_HIGH_1("MCC match with reserved MCCs mcc_match=0x%x",
                 mcc_match);

  if(mcc_match)
  {
    nv_data.gprs_anite_gcf = TRUE;

    (void)gstk_nv_access_write(GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I,
                               GSTK_SLOT_ID_MAX,
                               &nv_data);
  }
}
#endif /* FEATURE_GSTK_FUSION_REMOTE */
/*===========================================================================
FUNCTION GSTK_MMGSDI_CMD_CB

DESCRIPTION
  GSTK Callback for Client Id Reg and Refresh from MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_cmd_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  gstk_status_enum_type gstk_status   = GSTK_SUCCESS;
  gstk_task_cmd_type    *task_cmd_ptr = NULL;
  boolean               open_ch_requeue_flag = FALSE;
  uint32                         i              = 0;
  mmgsdi_return_enum_type        mmgsdi_status  = MMGSDI_SUCCESS;
  uint32                    session_type_mask    = 0;
  gstk_slot_id_enum_type    slot = GSTK_SLOT_ID_MAX;

  if(cnf_ptr == NULL)
  {
    return;
  }

  switch(cnf)
  {
  case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
    gstk_shared_data.gstk_mmgsdi_client_id =
      cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;

    /* Include masks of all the required types */
    session_type_mask = MMGSDI_GW_PROV_PRI_SESSION_MASK |
                        MMGSDI_GW_PROV_SEC_SESSION_MASK |
                        MMGSDI_GW_PROV_TER_SESSION_MASK |
                        MMGSDI_1X_PROV_PRI_SESSION_MASK |
                        MMGSDI_1X_PROV_SEC_SESSION_MASK |
                        MMGSDI_1X_PROV_TER_SESSION_MASK |
                        MMGSDI_CARD_SESSION_SLOT_1_MASK |
                        MMGSDI_CARD_SESSION_SLOT_2_MASK |
                        MMGSDI_CARD_SESSION_SLOT_3_MASK;

    for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
    {
      gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_ID_MAX;
      gstk_shared_data.mmgsdi_sess_info[i].sess_type =
        MMGSDI_MAX_SESSION_TYPE_ENUM;
    }
    mmgsdi_status = mmgsdi_session_open_ext(
                        gstk_shared_data.gstk_mmgsdi_client_id,
                        session_type_mask,
                        gstk_mmgsdi_events_cb,
                        0,
                        gstk_mmgsdi_cmd_cb,
                        0);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("failed to open sessions, mmgsdi_status: 0x%x",
                    mmgsdi_status);
    }
    break;
  case MMGSDI_SESSION_OPEN_EXT_CNF:
    if ((status == MMGSDI_SUCCESS) &&
        (cnf_ptr->session_open_ext_cnf.session_info_ptr != NULL))
    {
      /* Store the session id that were opened */
      for (i = 0; i < cnf_ptr->session_open_ext_cnf.num_sessions; i++)
      {
        mmgsdi_session_type_enum_type session_type =
          cnf_ptr->session_open_ext_cnf.session_info_ptr[i].session_type;
        mmgsdi_session_id_type session_id =
          cnf_ptr->session_open_ext_cnf.session_info_ptr[i].session_id;

        switch (session_type)
        {
          case MMGSDI_GW_PROV_PRI_SESSION:
          case MMGSDI_1X_PROV_PRI_SESSION:
          case MMGSDI_GW_PROV_SEC_SESSION:
          case MMGSDI_1X_PROV_SEC_SESSION:
          case MMGSDI_GW_PROV_TER_SESSION:
          case MMGSDI_1X_PROV_TER_SESSION:
            gstk_shared_data.mmgsdi_sess_info[i].sess_id   = session_id;
            gstk_shared_data.mmgsdi_sess_info[i].sess_type = session_type;
            break;
          case MMGSDI_CARD_SESSION_SLOT_1:
            gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_1;
            gstk_shared_data.mmgsdi_sess_info[i].sess_id   = session_id;
            gstk_shared_data.mmgsdi_sess_info[i].sess_type = session_type;
            break;
          case MMGSDI_CARD_SESSION_SLOT_2:
            gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_2;
            gstk_shared_data.mmgsdi_sess_info[i].sess_id   = session_id;
            gstk_shared_data.mmgsdi_sess_info[i].sess_type = session_type;
            break;
          case MMGSDI_CARD_SESSION_SLOT_3:
            gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_3;
            gstk_shared_data.mmgsdi_sess_info[i].sess_id   = session_id;
            gstk_shared_data.mmgsdi_sess_info[i].sess_type = session_type;
            break;
          default:
          break;
        }
      }
    }
    break;
  case MMGSDI_REFRESH_CNF:
    gstk_status = gstk_util_compare_ref_id(cnf_ptr->refresh_cnf.response_header.client_data);
    if(gstk_status != GSTK_SUCCESS)
    {
      break;
    }
    gstk_status = gstk_handle_mmgsdi_refresh_cnf(status, &(cnf_ptr->refresh_cnf));
    break;
  case MMGSDI_READ_CNF:
    UIM_MSG_HIGH_2("mmgsdi read cnf: 0x%x, 0x%x",
                   status, cnf_ptr->read_cnf.access.file.file_enum);
    slot = (gstk_slot_id_enum_type)cnf_ptr->response_header.slot_id;

    /* Validate Slot ID */
    if(!GSTK_IS_VALID_SLOT_ID(slot))
    {
      gstk_status = GSTK_BAD_PARAM;
      break;
    }

    if(cnf_ptr->response_header.client_data != (uint32)gstk_instances_ptr[(uint8)slot - 1]->open_ch_imsi_m_ptr)
    {
      gstk_status = GSTK_BAD_PARAM;
      break;
    }

    if (status == MMGSDI_SUCCESS &&
        cnf_ptr->read_cnf.access.access_method == MMGSDI_BY_PATH_ACCESS &&
        cnf_ptr->read_cnf.access.file.path_type.path_len == 0x3 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[0] == GSTK_CSIM_IMSI_M_PATH_BUF_1 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[1] == GSTK_CSIM_IMSI_M_PATH_BUF_2 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[2] == GSTK_CSIM_IMSI_M_PATH_BUF_3)
    {
      if (cnf_ptr->read_cnf.read_data.data_len > 8 &&
          !(cnf_ptr->read_cnf.read_data.data_ptr[7] & 0x80))
      {
        /* based on 3GPP2 C.S0065, bit 8 of byte 8 of IMIS_M is not set,
           IMSI_M is not programmed */
        task_cmd_ptr = gstk_task_get_cmd_buf();
        if (task_cmd_ptr != NULL)
        {
          task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
          task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_ORIG;
          task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = slot;
          task_cmd_ptr->cmd.general_cmd.message_header.user_data =
            cnf_ptr->response_header.client_data;
          gstk_task_put_cmd_buf(task_cmd_ptr);
        }
        else
        {
          break;          
        }
      }
      else
      {
        UIM_MSG_HIGH_0("IMSI_M provisioned, requeue Open Ch cmd from mgmsdi context");
        gstk_shared_data.gstk_is_imsi_m_provisioned = TRUE;
        open_ch_requeue_flag = TRUE;
      }
    }
    else if(status != MMGSDI_SUCCESS &&
        cnf_ptr->read_cnf.access.access_method == MMGSDI_BY_PATH_ACCESS &&
        cnf_ptr->read_cnf.access.file.path_type.path_len == 0x3 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[0] == GSTK_CSIM_IMSI_M_PATH_BUF_1 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[1] == GSTK_CSIM_IMSI_M_PATH_BUF_2 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[2] == GSTK_CSIM_IMSI_M_PATH_BUF_3)
    {
      UIM_MSG_ERR_0("MMSGDI read for CSIM IMSI_M failed");
      open_ch_requeue_flag = TRUE;
    }

    if(open_ch_requeue_flag == TRUE)
    {
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL)
      {
        /* Copy over the open channel command to requeue*/
        (void)gstk_memcpy((void *)&(task_cmd_ptr->cmd.general_cmd),
                          (void *)cnf_ptr->response_header.client_data,
                          sizeof(gstk_proactive_cmd_type),
                          sizeof(gstk_proactive_cmd_type),
                          sizeof(gstk_proactive_cmd_type));

        if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_PROACTIVE_CMD
           && task_cmd_ptr->cmd.general_cmd.message_header.command_id == 0)
        {
          task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_PRO_CMD_ID_OPEN_CH_IMSI_M_READ_FAIL;
          task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = slot;
          gstk_task_put_cmd_buf(task_cmd_ptr);
        }
        else
        {
          gstk_task_free_cmd_buf(task_cmd_ptr);
        }
      }
      gstk_free((void *)cnf_ptr->response_header.client_data);
    }
    break;

  case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_CNF:
    /* If IMSI is switched: Send NOT allowed to to NAS */
    /* If IMSI in not switched - Allow NAS */
    if(gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
         (gstk_slot_id_enum_type)cnf_ptr->gstk_loc_env_rsp_received_cnf.response_header.slot_id) == TRUE)
    {
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL)
      {
        task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.message_header.command_group = GSTK_MMGSDI_CMD;
        task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.message_header.command_id = GSTK_MMGSDI_LOC_ENV_RSP_RCVD_CNF_EVT;
        task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.message_header.sim_slot_id =
          (gstk_slot_id_enum_type)cnf_ptr->gstk_loc_env_rsp_received_cnf.response_header.slot_id;
        task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.imsi_switched =
          cnf_ptr->gstk_loc_env_rsp_received_cnf.imsi_switched;
        gstk_task_put_cmd_buf(task_cmd_ptr);
      }
    }
    break;

  default:
    break;
  }
}

/*===========================================================================
FUNCTION GSTK_FIND_MMGSDI_CMD

DESCRIPTION
  This function is called by q_linear_delete_new. This function will return TRUE only when the
  command id is that of a GSTK_MMGSDI_CMD.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  ptr to seq num

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int gstk_find_mmgsdi_cmd(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type     *task_cmd_ptr	 = NULL;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_ID_MAX;

  if(item_ptr == NULL || param_ptr == NULL)
  {
    return 0;
  }

  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  slot = *((gstk_slot_id_enum_type *)param_ptr);

  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_MMGSDI_CMD &&
     task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id == slot )
  {
    UIM_MSG_HIGH_1("GSTK_MMGSDI_CMD  of cmd id 0x%x found for deletion",
                   task_cmd_ptr->cmd.general_cmd.message_header.command_id);
    return 1;
  }
  else
  {
    return 0;
  }
} /* gstk_find_mmgsdi_cmd */

/*===========================================================================
FUNCTION GSTK_MMGSDI_EVENTS_CB

DESCRIPTION
  GSTK Callback for events from MMGSDI

PARAMETER
  evt_ptr [Input]: the MMGSDI event received

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_events_cb (
  const mmgsdi_event_data_type *evt_ptr
)
{
  gstk_task_cmd_type        *task_cmd_ptr    = NULL;
  gstk_mmgsdi_evt_enum_type  gstk_mmgsdi_evt = GSTK_MMGSDI_EVT_MAX;
  gstk_slot_id_enum_type     slot            = GSTK_SLOT_AUTO;

  if(evt_ptr == NULL) 
  {
    return;
  }

  switch (evt_ptr->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_CARD_INSERTED_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.card_inserted.slot;
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_SESSION_CHANGED_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.session_changed.app_info.slot;
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_SUBSCRIPTION_READY_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.subscription_ready.app_info.slot;
      break;

    case MMGSDI_REFRESH_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_REFRESH_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.refresh.slot;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_CARD_ERROR_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.card_error.slot;
      /* Entering gstk critical section */
      rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
      /* Find and delete pending MMGSDI commands */
      while( (task_cmd_ptr = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                     gstk_find_mmgsdi_cmd,
                                                     &slot,
                                                     NULL,
                                                     NULL)) != NULL)
      {
        gstk_task_free_cmd_buf(task_cmd_ptr);
      }
      rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
      task_cmd_ptr = NULL;
      break;

    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.terminal_profile.slot;
      break;

     default:
       break;
    }

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.command_group = GSTK_MMGSDI_CMD;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.command_id    = gstk_mmgsdi_evt;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.sim_slot_id   = slot;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.user_data     = 0;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.client_id     = 0;
    task_cmd_ptr->cmd.mmgsdi_evt.session_id                   = evt_ptr->session_id;
    (void)gstk_memcpy(&task_cmd_ptr->cmd.mmgsdi_evt.evt,
                      &evt_ptr->data,
                      sizeof(task_cmd_ptr->cmd.mmgsdi_evt.evt),
                      sizeof(task_cmd_ptr->cmd.mmgsdi_evt.evt),
                      sizeof(evt_ptr->data));

    if(evt_ptr->evt == MMGSDI_CARD_ERROR_EVT)
    {
      gstk_task_put_cmd_priority_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
  }
} /*gstk_mmgsdi_events_cb*/

#if defined(FEATURE_GSTK_FUSION_REMOTE) || defined (FEATURE_GSTK_TAL)
/*===========================================================================
FUNCTION gstk_tal_client_cb

DESCRIPTION
  GSTK callback registered with TAL for command noifications

PARAMETERS
  cmd_details: [Input] Details of command

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tal_client_cb(gstk_tal_cmd_details_type *tal_cmd)
{
  uim_rpt_type                      uim_rpt;
  gstk_status_enum_type             gstk_status  = GSTK_ERROR;

  GSTK_RETURN_IF_NULL_PTR(tal_cmd);
  memset(&uim_rpt, 0x00, sizeof(uim_rpt_type));

  if((tal_cmd->cmd_group == GSTK_TERMINAL_PROFILE_RSP) ||
       (tal_cmd->cmd_group == GSTK_TERMINAL_RSP) ||
       (tal_cmd->cmd_group == GSTK_ENVELOPE_RSP))
  {
    uim_rpt.user_data = tal_cmd->cmd_details.tal_uim_rpt.user_data;
    uim_rpt.sw1 = (uim_sw1_type)tal_cmd->cmd_details.tal_uim_rpt.sw1;
    uim_rpt.sw2 = (uim_sw2_type)tal_cmd->cmd_details.tal_uim_rpt.sw2;
    uim_rpt.rpt_status = (uim_rpt_status)tal_cmd->cmd_details.tal_uim_rpt.rpt_status;
  }

  /* Temporarily set UIM SLOT to 1 always */
  uim_rpt.slot = UIM_SLOT_1;

  UIM_MSG_HIGH_3("TAL::cmd_grp 0x%x, sw1 0x%x, sw2 0x%x",
                 tal_cmd->cmd_group, uim_rpt.sw1, uim_rpt.sw2 );
  switch(tal_cmd->cmd_group)
  {
  case GSTK_TERMINAL_PROFILE_RSP:
    uim_rpt.rpt_type = UIM_TERMINAL_PROFILE_R;
    gstk_uim_terminal_profile_report(&uim_rpt);
    break;
  case GSTK_TERMINAL_RSP:
    uim_rpt.rpt_type = UIM_TERMINAL_RESPONSE_R;
    uim_rpt.rpt.terminal_response.proactive_cmd_pending =
      tal_cmd->cmd_details.tal_uim_rpt.proactive_cmd_pending;
    gstk_uim_terminal_response_report(&uim_rpt);
    break;
  case GSTK_PROACTIVE_CMD:
    gstk_status = gstk_send_proactive_cmd(
                    (uim_slot_type)tal_cmd->cmd_details.tal_pro_cmd.slot,
                    tal_cmd->cmd_details.tal_pro_cmd.data_length,
                    tal_cmd->cmd_details.tal_pro_cmd.data_ptr);
    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Sending proactive command failed");
    }
    break;
  case GSTK_ENVELOPE_RSP:
    uim_rpt.rpt_type = UIM_ENVELOPE_R;
    if(tal_cmd->cmd_details.tal_uim_rpt.data_length > 0)
    {
      uim_rpt.rpt.envelope.data_length = tal_cmd->cmd_details.tal_uim_rpt.data_length;
      if(gstk_memcpy((void *)uim_rpt.rpt.envelope.data,
                     (void *)tal_cmd->cmd_details.tal_uim_rpt.data_ptr,
                     tal_cmd->cmd_details.tal_uim_rpt.data_length,
                     sizeof(uim_rpt.rpt.envelope.data),
                     tal_cmd->cmd_details.tal_uim_rpt.data_length) <
         tal_cmd->cmd_details.tal_uim_rpt.data_length)
      {
        return;
      }
    }
    gstk_uim_envelope_report(&uim_rpt);
    break;
  default:
    break;
  }
} /* gstk_tal_client_cb */
#endif /* (FEATURE_GSTK_FUSION_REMOTE) || (FEATURE_GSTK_TAL) */

/*===========================================================================
FUNCTION gstk_cm_subs_evt_cb

DESCRIPTION
  CMPH Subscription Event callback. Currently handled only
  CM_PH_EVENT_SUBSCRIPTION_PREF_INFO.

PARAMETERS
  subs_evt                  Subscription Event Enum
  subs_info_ptr             Subscription Event Data

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_subs_evt_cb(
  cm_ph_event_e_type        subs_evt,
  const cm_subs_info_s_type *subs_info_ptr
)
{
  uint32 i   = 0;
  uint32 idx = GSTK_MODEM_AS_INFO_SIZE;

  GSTK_RETURN_IF_NULL_PTR(subs_info_ptr);

  switch (subs_evt)
  {
    case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
      UIM_MSG_HIGH_3("CM subs evt: subs_id=0x%x, sess_t_1x=0x%x, sess_t_gw=0x%x",
                     subs_info_ptr->subscription_id,
                     subs_info_ptr->session_type_1x,
                     subs_info_ptr->session_type_gw);
      for (i = 0; i < GSTK_MODEM_AS_INFO_SIZE; ++i)
      {
        if (gstk_shared_data.modem_as_info[i].is_valid)
        {
          if (gstk_shared_data.modem_as_info[i].as_id ==
              subs_info_ptr->subscription_id)
          {
            gstk_shared_data.modem_as_info[i].sess_types[GSTK_AS_1X_SESSION] =
              subs_info_ptr->session_type_1x;
            gstk_shared_data.modem_as_info[i].sess_types[GSTK_AS_GW_SESSION] =
              subs_info_ptr->session_type_gw;
            idx = GSTK_MODEM_AS_INFO_SIZE;
            break;
          }
        }
        else if (idx == GSTK_MODEM_AS_INFO_SIZE)
        {
          idx = i;
        }
      }
      if (idx < GSTK_MODEM_AS_INFO_SIZE)
      {
        gstk_shared_data.modem_as_info[idx].is_valid = TRUE;
        gstk_shared_data.modem_as_info[idx].as_id =
          subs_info_ptr->subscription_id;
        gstk_shared_data.modem_as_info[idx].sess_types[GSTK_AS_1X_SESSION] =
          subs_info_ptr->session_type_1x;
        gstk_shared_data.modem_as_info[idx].sess_types[GSTK_AS_GW_SESSION] =
          subs_info_ptr->session_type_gw;
      }
      else
      {
        UIM_MSG_ERR_0("modem_as_info table is too small!");
      }
      /* Queue Subs mode info to GSTK as a command */
      gstk_build_cm_subs_mode_cmd(subs_info_ptr);
      break;
    default:
      break;
  }
} /* gstk_cm_subs_evt_cb */

/*===========================================================================
FUNCTION gstk_mm_idle_no_service_timer_cb

DESCRIPTION
  This function queues the saved location status command to gstk

PARAMETERS
  slot_index - [Input] In DS/DS the timer corresponding to the slot

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mm_idle_no_service_timer_cb(
  unsigned long  slot_index
)
{
  gstk_task_cmd_type     *task_cmd_ptr = NULL;

  if (!GSTK_IS_VALID_SLOT_ID(slot_index + 1))
  {
    return;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.mm_idle_state.message_header.command_group =
      GSTK_MM_IDLE_CMD;
    task_cmd_ptr->cmd.mm_idle_state.message_header.command_id =
      GSTK_MM_IDLE_FROM_TIMER;
    task_cmd_ptr->cmd.mm_idle_state.message_header.sim_slot_id =
      (gstk_slot_id_enum_type)slot_index;
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
} /* gstk_mm_idle_no_service_timer_cb */

/*===========================================================================
FUNCTION gstk_cdma_loc_status_timer_cb

DESCRIPTION
  This function queues the saved location status command to gstk

PARAMETERS
  slot_index - [Input] In DS/DS the timer corresponding to the slot

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cdma_loc_status_timer_cb(
  unsigned long  slot_index
)
{
  gstk_task_cmd_type     *task_cmd_ptr = NULL;

  if (!GSTK_IS_VALID_SLOT_ID(slot_index + 1))
  {
    return;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_group =
      GSTK_MODE_CHANGE_IND;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_id =
      GSTK_MODE_CHANGE_FROM_TIMER;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.sim_slot_id =
      slot_index + 1;
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
} /* gstk_cdma_loc_status_timer_cb */

/*===========================================================================
FUNCTION  gstk_process_rr_scell_info

DESCRIPTION
  This function process RR serving cell information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gstk_process_rr_scell_info(
  const sys_eng_mode_scell_info_s_type *cell_info_ptr
)
{
  gstk_status_enum_type gstk_status = GSTK_ERROR;
  uint8  invalid_plmn[GSTK_MAX_PLMN_LEN];

  if (cell_info_ptr == NULL)
  {
    return;
  }

  memset(invalid_plmn, 0x00, GSTK_MAX_PLMN_LEN);

  UIM_MSG_HIGH_5("RR MCC/MNC: 0x%x 0x%x 0x%x,"
  	         "RR LAC/CELL ID: 0x%x, 0x%x",
                 cell_info_ptr->plmn.identity[0],
                 cell_info_ptr->plmn.identity[1],
                 cell_info_ptr->plmn.identity[2],
                 cell_info_ptr->lac,
                 cell_info_ptr->cell_id);
  if(gstk_memcpy(gstk_shared_data.rr_mcc_mnc,
                 cell_info_ptr->plmn.identity,
                 sizeof(gstk_shared_data.rr_mcc_mnc),
                 sizeof(gstk_shared_data.rr_mcc_mnc),
                 sizeof(cell_info_ptr->plmn.identity)) <
     sizeof(gstk_shared_data.rr_mcc_mnc))
  {
    return;
  }
  gstk_shared_data.rr_lac[1] = (uint8) (cell_info_ptr->lac & 0x00FF);
  gstk_shared_data.rr_lac[0] = (uint8) ((cell_info_ptr->lac >> 8) & 0x00FF);
  gstk_shared_data.rr_cell_id[1] = (uint8) (cell_info_ptr->cell_id & 0x000000FF);
  gstk_shared_data.rr_cell_id[0] = (uint8) ((cell_info_ptr->cell_id >> 8) & 0x000000FF);

  if(gstk_shared_data.gstk_num_of_instances == 1)
  {
    UIM_MSG_HIGH_2("New PLMN from RR - send update loc info: 0x%x 0x%x",
                   gstk_instances_ptr[0]->is_tp_rsp_received,
                   gstk_instances_ptr[0]->is_first_loc_info_envelope_sent);
    if ((gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
           GSTK_SLOT_1) == TRUE)
        &&
        gstk_instances_ptr[0]->is_first_loc_info_envelope_sent != TRUE)
    {
      if((memcmp(gstk_instances_ptr[0]->gstk_last_loc_info_sent,
                cell_info_ptr->plmn.identity,
                GSTK_MAX_PLMN_LEN) != 0) &&
         (memcmp(cell_info_ptr->plmn.identity,
                invalid_plmn,
                GSTK_MAX_PLMN_LEN) != 0))
      {
        if (gstk_instances_ptr[0]->is_tp_rsp_received == TRUE)
        {
          gstk_status = gstk_send_envelope_loc_info_command(
                          gstk_instances_ptr[0]->client_id,
                          cell_info_ptr->plmn.identity,
                          0);
        }
        else
        {
          /* Copy the PLMN so that VPE can be sent after TP DL reponse is
                        received from card */
          (void)gstk_memcpy(gstk_instances_ptr[0]->gstk_last_loc_info_sent,
                            cell_info_ptr->plmn.identity,
                            GSTK_MAX_PLMN_LEN,
                            GSTK_MAX_PLMN_LEN,
                            GSTK_MAX_PLMN_LEN);
        }
      }
    }
  }
} /*gstk_process_rr_scell_info*/

/*===========================================================================
FUNCTION gstk_cm_stats_event_cb

DESCRIPTION
  This callback function receives registered statistic data from CM

PARAMETERS
  stats_event - [Input] The type of statistic event data
  stats_info_ptr - [Input] Statistic event data

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_stats_event_cb(
  cm_stats_event_e_type                  stats_event,
  const sys_modem_stats_info_s_type      *stats_info_ptr
)
{
  switch (stats_event)
  {
  case CM_STATS_EVENT_MODEM_INFO:
    if (stats_info_ptr == NULL)
    {
      UIM_MSG_ERR_0("Null stats data ptr!");
      return;
    }
    switch (stats_info_ptr->type)
    {
    case SYS_MODEM_MODULE_GSM:
      UIM_MSG_HIGH_1("stats data change bmsk 0x%x",
                     stats_info_ptr->prm.gsm_info.changed_bitmask);
      if (stats_info_ptr->prm.gsm_info.changed_bitmask &
          SYS_GERAN_ENG_MODE_SCELL_INFO)
      {
        gstk_process_rr_scell_info(
          &stats_info_ptr->prm.gsm_info.scell_info);
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
} /*gstk_cm_stats_event_cb*/


/*===========================================================================
FUNCTION gstk_ds_sys_evt_cb

DESCRIPTION
  Callback registered with DS which will be invoked when DS serving system
  changes

PARAMETERS
  Technology for which the event is being sent
  Event name
  DS Subscription ID
  Event information
  User data being returned to client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void  gstk_ds_sys_evt_cb
(
  ds_sys_tech_enum_type                          tech_type,
  ds_sys_event_enum_type                         event_name,
  ds_sys_subscription_enum_type                  subscription_id,
  void*                                          event_info_ptr,
  void*                                          user_data_ptr
)
{
  gstk_task_cmd_type*    task_cmd = NULL;

  (void)user_data_ptr;

  if(event_info_ptr == NULL)
  {
    return;
  }

  /* Validate DS event */
  if((event_name != DS_SYS_EVENT_SYSTEM_STATUS_EX) &&
     (event_name != DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE) &&
     (event_name != DS_SYS_EVENT_WLAN_SSID_CHANGE))
  {
     return;
  }

  UIM_MSG_HIGH_2("Received DS event: 0x%x for DS SUB ID: 0x%x",
                 event_name, subscription_id);

  /* Allocate memory for the command */
  task_cmd = gstk_task_get_cmd_buf();

  if(task_cmd == NULL)
  {
    return;
  }

  /* Build the command */
  task_cmd->cmd.ds_sys_status_ex_ind.message_header.command_group =
    GSTK_DS_EVT_CMD;
  task_cmd->cmd.ds_sys_status_ex_ind.message_header.sim_slot_id = GSTK_SLOT_AUTO;

  if(event_name == DS_SYS_EVENT_SYSTEM_STATUS_EX)
  {
    task_cmd->cmd.ds_sys_status_ex_ind.subscription_id = subscription_id;
    task_cmd->cmd.ds_sys_status_ex_ind.message_header.command_id =
      GSTK_DS_SYSTEM_STATUS_EX_IND;
    task_cmd->cmd.ds_sys_status_ex_ind.ds_sys_status_info =
      *((ds_sys_system_status_ex_type*)event_info_ptr);

    UIM_MSG_HIGH_1("Number of available systems 0x%x",
                   task_cmd->cmd.ds_sys_status_ex_ind.ds_sys_status_info.num_avail_sys);
  }
  else if(event_name == DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE)
  {
    task_cmd->cmd.ds_wlan_mac_addr_change_ind.message_header.command_id =
      GSTK_DS_WLAN_MAC_ADDR_CHANGE_IND;
    task_cmd->cmd.ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info =
      *((ds_sys_wlan_mac_addr_type*)event_info_ptr);
  }
  else if(event_name == DS_SYS_EVENT_WLAN_SSID_CHANGE)
  {
    task_cmd->cmd.ds_wlan_ssid_change_ind.message_header.command_id =
      GSTK_DS_WLAN_SSID_CHANGE_IND;
    task_cmd->cmd.ds_wlan_ssid_change_ind.ds_wlan_ssid_info =
      *((ds_sys_wlan_ssid_type*)event_info_ptr);
  }

  /* Put command on to the GSTK command queue */
  gstk_task_put_cmd_buf(task_cmd);
}


/*===========================================================================
FUNCTION gstk_init

DESCRIPTION
  GSTK Task Intialization Procedures before GSTK TASK is Started.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_init( void )
{
   uint8  i                       = 0;

   UIM_MSG_MED_0(" gstk_init");

   /* Intialize GSTK Queues */
  (void) q_init(&gstk_task_cmd_q);
  (void) q_init(&gstk_task_priority_cmd_q);

  gstk_curr_inst_ptr = NULL;

  gstk_shared_data.gstk_num_of_instances = 1;

  /* Initialize the Critical Section for GSTK Task */
  rex_init_crit_sect(&gstk_shared_data.gstk_crit_sect);

  gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
  gstk_shared_data.active_data_subs= SYS_MODEM_AS_ID_MAX;
  memset(gstk_shared_data.rr_mcc_mnc,
         0xFF,
         sizeof(gstk_shared_data.rr_mcc_mnc));
  memset(gstk_shared_data.rr_lac,
         0x00,
         sizeof(gstk_shared_data.rr_lac));
  memset(gstk_shared_data.rr_cell_id,
         0xFF,
         sizeof(gstk_shared_data.rr_cell_id));
  gstk_shared_data.mm_ind_cache = GSTK_IO_MM_CMD_MAX;

  gstk_shared_data.gstk_otasp_status_cb = NULL;
  /* initialize other slots in GSTK client table */
  for (i = 0; i < GSTK_MAX_CLIENT; i++) {
    gstk_shared_data.gstk_client_table[i].client_cb    = NULL;
    gstk_shared_data.gstk_client_table[i].client_type  = GSTK_NO_TYPE;
    gstk_shared_data.gstk_client_table[i].is_free      = TRUE;
    gstk_shared_data.gstk_client_table[i].user_data    = 0;
  }

  gstk_shared_data.gstk_is_imsi_m_provisioned      = FALSE;
  gstk_shared_data.is_open_ch_ok                   = TRUE;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  /* Mark that none of the tasks required for proper GSTK functioning
       are ready yet */
  gstk_shared_data.ready_tasks = 0;
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  gstk_shared_data.gstk_continue_task = TRUE;

  /* initilaize GSTK command details table */
  memset(command_details_ref_table, 0x00, sizeof(command_details_ref_table));
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
    command_details_ref_table_ptr[i].free_slot = TRUE;
  }

  /* initialize GSTK env command ref table */
  for (i = 0; i <= GSTK_MAX_PROACTIVE_COMMAND; i++) {
    gstk_shared_data.envelope_cmd_ref_table[i].is_free = TRUE;
  }

#ifdef FEATURE_GSTK_TAL
  memset(gstk_tal_client_cb_list, 0x00 , sizeof(gstk_tal_client_cb_list));
#endif /* FEATURE_GSTK_TAL */

  for(i=0; i<GSTK_MAX_NUM_OF_INSTANCES; i++)
  {
    gstk_instances_ptr[i] = NULL;
  }

/* Initialize estk queue and estk global variables */
#ifdef FEATURE_ESTK
  estk_init();
#endif /* FEATURE_ESTK */

} /* gstk_init */

/*===========================================================================
FUNCTION gstk_init_post_task_startup

DESCRIPTION
  GSTK Task Intialization Procedures after GSTK TASK is Started.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_init_post_task_startup( void )
{
  uint8                            i                       = 0;
  uint8                            j                       = 0;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif
  uim_hw_slot_info_type            slots_available;
  uint8                            slot_count              = 0;

  UIM_MSG_HIGH_0(" gstk_init_post_task_startup");

  slots_available = uim_get_hw_slot_info();
  for(i=0; i<UIM_MAX_NUMBER_INSTANCES; i++)
  {
    if(slots_available.slot_status[i].slot_enabled)
    {
      slot_count++;
    }
  }

  if(slot_count > GSTK_MAX_NUM_OF_INSTANCES)
  {
    ERR_FATAL("No of slots enabled: 0x%x is greater that GSTK MAX instances 0x%x",
              slot_count, GSTK_MAX_NUM_OF_INSTANCES, 0);
  }

  if(slot_count != 0)
  {
    gstk_shared_data.gstk_num_of_instances = slot_count;
  }

/* To avoid crashes on Multi SIM NV setting with lesser SIM build variant */
#if defined FEATURE_UIM_SUPPORT_TRIPLE_SLOTS
  if(gstk_shared_data.gstk_num_of_instances > 3)
  {
    gstk_shared_data.gstk_num_of_instances = 3;
  }
#elif defined FEATURE_UIM_SUPPORT_DUAL_SLOTS
  if(gstk_shared_data.gstk_num_of_instances > 2)
  {
    gstk_shared_data.gstk_num_of_instances = 2;
  }
#else
  if(gstk_shared_data.gstk_num_of_instances > 1)
  {
    gstk_shared_data.gstk_num_of_instances = 1;
  }
#endif /* FEATURE_UIM_SUPPORT_TRIPLE_SLOTS */

  for (i = 0; i < gstk_shared_data.gstk_num_of_instances; ++ i)
  {
    gstk_instances_ptr[i] = &gstk_instances[i];

    /* set instance state to START */
    gstk_instances_ptr[i]->gstk_state = GSTK_START_S;
    gstk_instances_ptr[i]->gstk_sub_state = GSTK_INVALID_SUB_S;
    gstk_instances_ptr[i]->nw_search_mode = GSTK_NW_SEARCH_MODE_MAX;
    gstk_instances_ptr[i]->csg_info.access_tech = GSTK_ACCESS_NONE;
    gstk_instances_ptr[i]->slot_id = (gstk_slot_id_enum_type)(i + 1);
  }

  /* initialize default client ID for each instance */
  for (i = 0; i < gstk_shared_data.gstk_num_of_instances; ++ i)
  {
    for (j = 0; j < GSTK_MAX_CLIENT;j++)
    {
      if (gstk_shared_data.gstk_client_table[j].is_free)
      {
        gstk_shared_data.gstk_client_table[j].is_free = FALSE;
        gstk_shared_data.gstk_client_table[j].client_type = GSTK_GSTK_TYPE;
        gstk_shared_data.gstk_client_table[j].client_cb = NULL;
        gstk_shared_data.gstk_client_table[j].user_data = 0;
        gstk_shared_data.gstk_client_table[j].sim_slot_id = GSTK_SLOT_AUTO;
        gstk_instances_ptr[i]->client_id = (gstk_client_id_type)(j+1);
        break;
      }
    }
    if (j == GSTK_MAX_CLIENT)
    { /* no more space available */
      UIM_MSG_ERR_0("GSTK Client Table Full");
    }
    gstk_shared_data.gstk_client_table[
      gstk_instances_ptr[i]->client_id - 1].sim_slot_id = (gstk_slot_id_enum_type)(i + 1);
  }

  /* terminal response wait timer */
  /* assign timer signal to each timer in the table */
  rex_def_timer_ex(&(command_details_ref_table_ptr[0].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER1);
  rex_def_timer_ex(&(command_details_ref_table_ptr[1].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER2);
  rex_def_timer_ex(&(command_details_ref_table_ptr[2].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER3);
  rex_def_timer_ex(&(command_details_ref_table_ptr[3].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER4);
  rex_def_timer_ex(&(command_details_ref_table_ptr[4].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER5);
  rex_def_timer_ex(&(command_details_ref_table_ptr[5].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER6);
  rex_def_timer_ex(&(command_details_ref_table_ptr[6].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER7);
  rex_def_timer_ex(&(command_details_ref_table_ptr[7].tp_rpt_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TR_TIMER8);
  rex_def_timer_ex(&(gstk_shared_data.gstk_clients_reg_timer),
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_CLIENTS_REG_TIMER);

  /* terminal profile command wait timer */
  rex_def_timer_ex(&gstk_shared_data.gstk_tp_wait_timer,
                   gstk_build_timer_expired_cmd,
                   (unsigned long)GSTK_TP_TIMER);

  for (i = 0; i < gstk_shared_data.gstk_num_of_instances; ++ i)
  {

    /* Initialize timer */
    rex_def_timer_ex( &(gstk_instances_ptr[i]->gstk_loc_info_envelope_timer),
                       gstk_build_timer_expired_cmd,
                     (unsigned long)GSTK_LOC_INFO_TIMER);
    gstk_instances_ptr[i]->is_first_loc_info_envelope_sent = FALSE;
    memset(gstk_instances_ptr[i]->gstk_last_loc_info_sent, 0xFF, 3);

    /* set it to TRUE because FALSE means TP DL is in progress */
    gstk_instances_ptr[i]->is_tp_dl_done = TRUE;
    gstk_instances_ptr[i]->is_tp_rsp_received = FALSE;

    /* gstk_mm_idle_no_service_timer */
    rex_def_timer_ex(&(gstk_instances_ptr[i]->gstk_mm_idle_no_service_timer),
                       gstk_mm_idle_no_service_timer_cb,
                       (unsigned long)i);
    gstk_instances_ptr[i]->gstk_mm_idle_no_srv_from_timer = NULL;

    /* 1x location status base ID wait timer */
    rex_def_timer_ex(&(gstk_instances_ptr[i]->gstk_cdma_loc_status_timer),
                       gstk_cdma_loc_status_timer_cb,
                       (unsigned long)i);
    gstk_instances_ptr[i]->gstk_cdma_loc_status_cmd_ptr = NULL;
    rex_def_timer_ex(&gstk_instances_ptr[i]->attach_ctrl_timer,
                     gstk_attach_ctrl_timer_exp_cb,
                     (i + 1));
    gstk_instances_ptr[i]->in_prog_cmd_ref_id = GSTK_MAX_PROACTIVE_COMMAND;

    /* define timer management timer */
    for (j = 0; j < GSTK_MAX_TIMER_SUPPORTED; j++) {
      rex_def_timer_ex(&(gstk_instances_ptr[i]->gstk_timer_management[j].timer),
                       gstk_rex_timer_expire_cb,
                       (i << 8) | j );
      rex_def_timer_ex(&(gstk_instances_ptr[i]->gstk_timer_management[j].timer_elapsed),
                       gstk_rex_timer_elapsed_expire_cb,
                       (i << 8) | j);
    }

    /* Initialize structure that stores current envelope info */
    memset(&gstk_instances_ptr[i]->gstk_curr_envelope_info,
           0x00,
           sizeof(gstk_instances_ptr[i]->gstk_curr_envelope_info));

    /* initialize the mm_param received to FALSE */
    /* this is set to true when received the first mm idle event in the idle state */
    gstk_instances_ptr[i]->gstk_mm_param_received = FALSE;

    gstk_instances_ptr[i]->gstk_profile_received           = FALSE;
    gstk_instances_ptr[i]->gstk_is_curr_session_ui_related = FALSE;
    gstk_instances_ptr[i]->gstk_cm_call_is_connected = FALSE;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
    for (j = 0; j < GSTK_RAT_MAX_IDX; j++)
    {
      gstk_instances_ptr[i]->is_rrc_cell_id_received[j] = FALSE;
      /* Memset RRC location information table */
      memset(gstk_instances_ptr[i]->rrc_loci_table[j].plmn_id, 0xFF, GSTK_MAX_PLMN_LEN);
      memset(gstk_instances_ptr[i]->rrc_loci_table[j].lac, 0xFF, GSTK_MAX_LAC_LEN);
      gstk_instances_ptr[i]->rrc_loci_table[j].cell_info.cell_len = GSTK_MAX_CELL_ID_LEN;
      memset(gstk_instances_ptr[i]->rrc_loci_table[j].cell_info.cell_id,
             0xFF,
             GSTK_MAX_CELL_ID_LEN);
    }
#endif /* FEATURE_WCDMA || FEATURE_LTE*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  /* Memset MM-GSTK location information table */
    memset(gstk_instances_ptr[i]->gstk_loci_table.plmn_id,
           0xFF,
           GSTK_MAX_PLMN_LEN);
    memset(gstk_instances_ptr[i]->gstk_loci_table.lac,0xFF,GSTK_MAX_LAC_LEN);
    gstk_instances_ptr[i]->gstk_loci_table.cell_info.cell_len =
      GSTK_MAX_CELL_ID_LEN;
    memset(gstk_instances_ptr[i]->gstk_loci_table.cell_info.cell_id,
           0xFF,
           GSTK_MAX_CELL_ID_LEN);
    gstk_instances_ptr[i]->gstk_loci_table.loc_status = GSTK_NO_SERVICE;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/

    /* Initialize the class - evt table */
    for (j = 0; j < GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY; j++)
    {
      /* Modem functionality client */
      gstk_instances_ptr[i]->gstk_class_evt_reg_table[j].gstk_modem_func_client.client_id =
        GSTK_MAX_CLIENT + 1;
      gstk_instances_ptr[i]->gstk_class_evt_reg_table[j].gstk_modem_func_client.data_format_type =
        GSTK_FORMAT_NONE;
      gstk_instances_ptr[i]->gstk_class_evt_reg_table[j].gstk_modem_func_client.is_reg = FALSE;
      /* UI functionality client */
      gstk_instances_ptr[i]->gstk_class_evt_reg_table[j].gstk_ui_func_client.client_id =
        GSTK_MAX_CLIENT + 1;
      gstk_instances_ptr[i]->gstk_class_evt_reg_table[j].gstk_ui_func_client.data_format_type =
        GSTK_FORMAT_NONE;
      gstk_instances_ptr[i]->gstk_class_evt_reg_table[j].gstk_ui_func_client.is_reg = FALSE;
    }

    gstk_instances_ptr[i]->gstk_mm_state_is_idle     = FALSE;
    gstk_instances_ptr[i]->gstk_pending_location_evt_envelope = FALSE;
    memset(gstk_instances_ptr[i]->gstk_curr_mcc_mnc, 0xFF, 3);
    memset(gstk_instances_ptr[i]->gstk_curr_lac, 0xFF, 2);
    memset(gstk_instances_ptr[i]->gstk_curr_cell_id.cell_id,
           0xFF,
           GSTK_MAX_CELL_ID_LEN);
    gstk_instances_ptr[i]->gstk_curr_cell_id.cell_len = 0;
    gstk_instances_ptr[i]->gstk_curr_location_status = GSTK_NO_SERVICE;
    gstk_instances_ptr[i]->gstk_curr_rat = GSTK_ACCESS_NONE;
    memset(gstk_instances_ptr[i]->gstk_next_curr_mcc_mnc, 0xFF, 3);
    memset(gstk_instances_ptr[i]->gstk_next_curr_lac, 0xFF, 2);
    memset(gstk_instances_ptr[i]->gstk_next_curr_cell_id.cell_id,
           0xFF,
           GSTK_MAX_CELL_ID_LEN);
    gstk_instances_ptr[i]->gstk_next_curr_cell_id.cell_len = 0;
    gstk_instances_ptr[i]->gstk_next_curr_location_status = GSTK_NO_SERVICE;
    gstk_instances_ptr[i]->gstk_next_curr_rat = GSTK_ACCESS_NONE;
    gstk_instances_ptr[i]->nas_rat = GSTK_ACCESS_NONE;
	gstk_instances_ptr[i]->gstk_curr_tech_group= GSTK_ACCESS_TECH_NONE;

    GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
      gstk_instances_ptr[i]->gstk_proactive_cmd_in_progress, FALSE);
    gstk_instances_ptr[i]->card_protocol                   = MMGSDI_NO_PROTOCOL;
    gstk_instances_ptr[i]->gstk_num_of_tp_dl_received      = 0;
    gstk_instances_ptr[i]->apps_enabled                    = GSTK_APP_BMSK_NONE;
    gstk_instances_ptr[i]->recovery_enable_ls_act          = FALSE;

    gstk_instances_ptr[i]->gstk_is_msg_cached = FALSE;
    gstk_instances_ptr[i]->otasp_call_id = -1;
    gstk_instances_ptr[i]->otasp_failure_cnt = 0;
    gstk_instances_ptr[i]->recv_otasp_commit = FALSE;
    gstk_instances_ptr[i]->open_ch_imsi_m_ptr = NULL;
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
    gstk_instances_ptr[i]->otasp_requeued = FALSE;
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */
    gstk_instances_ptr[i]->subs_feature_mode =
      GSTK_SUBS_FEATURE_MODE_NONE;

    memset (&(gstk_instances_ptr[i]->gstk_cmd_cache),
              0x00,
              sizeof (gstk_cmd_cache_type));
#ifdef FEATURE_CDMA
    memset(&gstk_instances_ptr[i]->cdma_ext_loci_cache,
           0xFF,
           sizeof(gstk_instances_ptr[i]->cdma_ext_loci_cache));
    gstk_instances_ptr[i]->cdma_ext_loci_cache.is_cdma_info_valid = FALSE;
    gstk_instances_ptr[i]->cdma_ext_loci_cache.is_advance_info_valid = FALSE;
    gstk_instances_ptr[i]->cdma_ext_loci_cache.is_hdr_info_valid = FALSE;
#endif /* FEATURE_CDMA */
    /* Timer to delay Terminal Response */
    rex_def_timer_ex(&(gstk_instances_ptr[i]->gstk_terminal_rsp_delay_timer),
                       gstk_terminal_rsp_delay_cb,
                       (unsigned long)i);
    gstk_instances_ptr[i]->setup_evt_list_cnt  = 0;
    gstk_instances_ptr[i]->mm_idle_pri_ptr     = NULL;
    gstk_instances_ptr[i]->mode_change_cmd_ptr = NULL;
    for (j = 0; j < GSTK_CM_NUM_STACKS; ++j)
    {
      gstk_instances_ptr[i]->tz_info[j].access_tech = GSTK_ACCESS_NONE;
      gstk_instances_ptr[i]->tz_info[j].tz_avail = GSTK_TZ_NOT_AVAILABLE;
      gstk_instances_ptr[i]->tz_info[j].tz = 0xFF;
    }

    gstk_instances_ptr[i]->nw_rej.access_tech = GSTK_ACCESS_NONE;
    memset(gstk_instances_ptr[i]->nw_rej.mcc_mnc, 0xFF, GSTK_MAX_PLMN_LEN);
    memset(gstk_instances_ptr[i]->nw_rej.lac, 0xFF, GSTK_MAX_LAC_LEN);
  }

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* Register for NV refresh notifications with MCFG */
  gstk_nv_mcfg_refresh_register();
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  /* Read and cache ME and slot specific NV items */
  gstk_nv_init_task();

  for (i = 0; i < gstk_shared_data.gstk_num_of_instances; ++ i)
  {
    if((gstk_instances_ptr[i]->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) ||
       (gstk_nv_get_feature_status(
          GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY, (gstk_slot_id_enum_type)(i+1)) == FALSE))
    {
      UIM_MSG_HIGH_0("Env retry flag is set to TRUE or call ctrl envelope retry is not disabled");
      /* clear timer for envelope retry */
      (void)rex_clr_timer(&gstk_instances_ptr[i]->gstk_curr_envelope_info.gstk_env_retry_timer);

      /* define timer for envelope retry */
      rex_def_timer_ex(
        &(gstk_instances_ptr[i]->gstk_curr_envelope_info.gstk_env_retry_timer),
        gstk_env_retry_timer_exp_cb,
        (gstk_slot_id_enum_type)(i+1));
    }
  }

  /* Initalize number of expected TP info */
  gstk_tp_config_init();

/* Call ESTK initializtion to register with GSTK as one of the GSTK client */
#ifdef FEATURE_ESTK
  estk_gstk_init(gstk_shared_data.gstk_num_of_instances);
#endif /* FEATURE_ESTK */

  for (i = 0; i < gstk_shared_data.gstk_num_of_instances; ++ i)
  {
    if (gstk_update_default_app_specific_tp(i) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Failed to update GSTK default TP!");
      /* increment counter even if it fails to queue TP cmd */
      ++ gstk_instances_ptr[i]->gstk_num_of_tp_dl_received;
    }
  }

#ifdef FEATURE_LTE
  /* Register with MSGR */
  gstk_msgr_init();
#endif

  (void)gstk_switch_to_instance(GSTK_SLOT_1);

/* For the gstk offtarget UT, skip qmi cat init. NOTE that the efs item below
   exists only offtarget. For on-target, qmi cat init will be done when QMI
   MODEM task signals us that it is ready */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  UIM_MSG_LOW_0(" == GSTK _ INIT _ POST _ TASK _ STARTUP COMPLETE ");
} /* gstk_init_post_task_startup */

/*===========================================================================
FUNCTION gstk_tp_config_init

DESCRIPTION
  Initialize number of expected TP info

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_tp_config_init( void )
{
  uint32                  bitmask = 1;
  gstk_nv_items_data_type nv_data;


#ifdef TEST_FRAMEWORK
  #error code not present
#else
  /* GSTK itself will do TP DL */
  gstk_shared_data.num_of_expected_tp = 1;
#endif /* TEST_FRAMEWORK */

  if(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_ME_DYNAMIC_TP_CLIENTS,
       GSTK_SLOT_ID_MAX) == TRUE)
  {
    /* read all clients that will do TP DL */
    (void)gstk_nv_access_read(
            GSTK_NV_ME_TP_CLIENTS,
            GSTK_SLOT_ID_MAX,
            &nv_data);

    /* count the num of clients that do TP DL */
    while (bitmask && nv_data.tp_clients)
    {
      if (bitmask & nv_data.tp_clients)
      {
        ++ gstk_shared_data.num_of_expected_tp;
      }
      bitmask <<= 1;
    }
  }
} /*gstk_tp_config_init*/

/*===========================================================================
FUNCTION gstk_cm_init

DESCRIPTION
  Register GSTK as client of Call Manager.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
LOCAL static void gstk_cm_init( void )
{
  cm_client_status_e_type cm_init_return_status = CM_CLIENT_OK;
  cm_client_status_e_type cm_reg_return_status  = CM_CLIENT_OK;

  do
  {
    /* client init with CM */
    cm_init_return_status = cm_client_init(CM_CLIENT_TYPE_GSTK,
                                           &gstk_shared_data.gstk_client_id_from_cm);
  
    if (cm_init_return_status != CM_CLIENT_OK)
    {
       break;
    }
  
    (void)cm_client_act(gstk_shared_data.gstk_client_id_from_cm);
  
    /* register for CM serving system events */
    cm_reg_return_status = cm_mm_client_ss_reg_msim(
        gstk_shared_data.gstk_client_id_from_cm, /* Client id from CM */
        gstk_cm_ss_event_cb, /* CB function */
        (cm_client_event_reg_e_type)CM_CLIENT_EVENT_REG, /* Register */
        (cm_ss_event_e_type)CM_SS_EVENT_SRV_CHANGED, /* From ss_event */
        (cm_ss_event_e_type)CM_SS_EVENT_SRV_CHANGED, /* To ss_event */
        NULL, /* cb function for errors of other clients */
        SYS_MODEM_AS_ID_MASK_ANY); /* asubs_mask */
  
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      break;
    }
  
    cm_reg_return_status = cm_client_subs_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_subs_evt_cb);
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_0("Failed to register subs evt from CM!");
    }
  
    cm_reg_return_status = cm_client_ph_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_ph_event_cb,
        CM_CLIENT_EVENT_REG,
        CM_PH_EVENT_DUAL_STANDBY_PREF,
        CM_PH_EVENT_DUAL_STANDBY_PREF,
        NULL );
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      break;
    }
  
    cm_reg_return_status = cm_client_ph_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_ph_event_cb,
        CM_CLIENT_EVENT_REG,
        CM_PH_EVENT_SYS_SEL_PREF,
        CM_PH_EVENT_SYS_SEL_PREF,
        NULL );
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      break;
    }
  
    cm_reg_return_status = cm_client_ph_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_ph_event_cb,
        CM_CLIENT_EVENT_REG,
        CM_PH_EVENT_INFO,
        CM_PH_EVENT_INFO,
        NULL );
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      break;
    }
  
    cm_reg_return_status = cm_client_ph_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_ph_event_cb,
        CM_CLIENT_EVENT_REG,
        CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,
        CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,
        NULL );
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
       break;
    }
  
    /* register for call connected event */
    cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_REG,        /* event registration */
                          CM_CALL_EVENT_CONNECT,      /* from this call event */
                          CM_CALL_EVENT_CONNECT,      /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
       break;
    }
  
    /* register for call disconnected event */
  
    cm_reg_return_status = cm_mm_client_call_reg(
                      gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                      gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                      CM_CLIENT_EVENT_REG,        /* event registration */
                      CM_CALL_EVENT_END,          /* from this call event */
                      CM_CALL_EVENT_END,          /* to this call event */
                      NULL );                     /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
       break;
    }
  
    /* register for call OTASP event */
    cm_reg_return_status = cm_mm_client_call_reg(
                      gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                      gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                      CM_CLIENT_EVENT_REG,        /* event registration */
                      CM_CALL_EVENT_OTASP_STATUS, /* from this call event */
                      CM_CALL_EVENT_OTASP_STATUS, /* to this call event */
                      NULL );                     /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
       break;
    }
  
#ifdef FEATURE_GSM
    cm_reg_return_status = cm_client_stats_reg(
                             gstk_shared_data.gstk_client_id_from_cm,
                             gstk_cm_stats_event_cb,
                             CM_CLIENT_EVENT_REG,
                             CM_STATS_EVENT_MODEM_INFO,
                             CM_STATS_EVENT_MODEM_INFO,
                             NULL);
    if (cm_reg_return_status == CM_CLIENT_OK)
    {
      if (!cm_stats_set_modem_info_mask(NULL,
                                        NULL,
                                        gstk_shared_data.gstk_client_id_from_cm,
                                        SYS_MODEM_MODULE_GSM,
                                        SYS_GERAN_ENG_MODE_SCELL_INFO))
      {
        break;
      }
    }
    else
    {
      break;
    }
#endif /* FEATURE_GSM */
  }while(0);

  if((cm_reg_return_status != CM_CLIENT_OK) ||
     (cm_init_return_status != CM_CLIENT_OK))
  {
    UIM_MSG_ERR_2("Reg Status 0x%x, Init Status 0x%x",
		cm_reg_return_status, cm_reg_return_status);
    return;
  }
/* Call ESTK CM initialization function to allow ESTK to resgister with CM */
#ifdef FEATURE_ESTK
  estk_cm_init();
#endif /* FEATURE_ESTK */
} /* gstk_cm_init */

#ifndef FEATURE_ESTK
/*===========================================================================
FUNCTION: gstk_wms_init

DESCRIPTION:
  GSTK registration of WMS client

PARAMETERS:
  None

DEPENDENCIES:
  This is only called if FEATURE_ESTK is off. If feature is turned on
  then ESTK will register to WMS instead.

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void gstk_wms_init( void )
{
  wms_client_err_e_type wms_client_err = WMS_CLIENT_ERR_MAX;

  do
  {
    wms_client_err = wms_client_init(WMS_CLIENT_TYPE_CAT,
                                     &gstk_shared_data.wms_client_id );
    if (wms_client_err != WMS_CLIENT_ERR_NONE)
    {
      UIM_MSG_ERR_1("wms_client_init returned (0x%x)", wms_client_err);
      return;
    }
  
    wms_client_err = wms_client_activate( gstk_shared_data.wms_client_id );
  
    if (wms_client_err != WMS_CLIENT_ERR_NONE)
    {
      break;
    }
  
    wms_client_err = wms_client_reg_msg_cb(
      gstk_shared_data.wms_client_id,
      (wms_msg_event_cb_type)gstk_wms_event_cb);
  
    if (wms_client_err != WMS_CLIENT_ERR_NONE)
    {
        break;
    }
  }while(0);

  if (wms_client_err != WMS_CLIENT_ERR_NONE)
  {
    (void)wms_client_release( gstk_shared_data.wms_client_id );
    UIM_MSG_ERR_1("Failed to reg GSTK WMS evt cb. (0x%x)", wms_client_err);
  }
}
#endif /* FEATURE_ESTK */

#ifdef FEATURE_LTE
/*===========================================================================
FUNCTION gstk_msgr_init

DESCRIPTION
  Register GSTK as client of Message Router.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
LOCAL static void gstk_msgr_init( void )
{
  uint8                            i                       = 0;
  errno_enum_type                  e_status                = E_SUCCESS;
  q_type                           *q_ptr                  = NULL;

  do {
    /* initialize msg queue */
    q_ptr = q_init(&gstk_shared_data.rrc_msg_q);
    if (q_ptr == NULL)
    {
      e_status = E_FAILURE;
      break;
    }

    q_ptr = q_init(&gstk_shared_data.rrc_msg_q_free);
    if (q_ptr == NULL)
    {
      e_status = E_FAILURE;
      break;
    }
    for(i = 0; i < GSTK_NUM_CMD_BUFS; ++i)
    {
      q_put(&gstk_shared_data.rrc_msg_q_free,
            q_link((void *)&gstk_shared_data.rrc_msg_buff[i],
                   &gstk_shared_data.rrc_msg_buff[i].link));
    }

    e_status = msgr_client_create(
                 &gstk_shared_data.msgr_rrc_client);
    if (e_status != E_SUCCESS)
    {
      break;
    }

    e_status = msgr_client_set_rex_q(
                 &gstk_shared_data.msgr_rrc_client,
                 UIM_GSTK_TCB,
                 GSTK_RRC_MSG_Q_SIG,
                 &gstk_shared_data.rrc_msg_q,
                 &gstk_shared_data.rrc_msg_q_free,
                 STD_OFFSETOF( gstk_rrc_msg_type, rrc_msgr_cmd ),
                 MSGR_NO_OFFSET,
                 (uint16)sizeof(gstk_rrc_msg_type));
    if (e_status != E_SUCCESS)
    {
      break;
    }

    e_status = msgr_register(MSGR_UIM_STK,
                             &gstk_shared_data.msgr_rrc_client,
                             MSGR_ID_NONE,
                             LTE_RRC_SERVICE_IND) ;
    if (e_status != E_SUCCESS)
    {
      break;
    }

    e_status = msgr_register(MSGR_UIM_STK,
                             &gstk_shared_data.msgr_rrc_client,
                             MSGR_ID_NONE,
                             LTE_RRC_NMR_INFO_CNF) ;
  } while(0);

  if (e_status != E_SUCCESS)
  {
    UIM_MSG_ERR_0("gstk_msgr_init failed");
  }
}/* gstk_msgr_init */
#endif /*FEATURE_LTE*/

/*===========================================================================
FUNCTION gstk_ds_event_reg_sys_status_change

DESCRIPTION
  Function for having GSTK register with DS for System Status changes.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_ds_event_reg_sys_status_change(void)
{
  int16  dss_ret_val = 0;
  int16  dss_errno   = 0;

  dss_ret_val =  ds_sys_event_reg_ex(DS_SYS_TECH_ALL,
                                     DS_SYS_EVENT_SYSTEM_STATUS_EX,
                                     DS_SYS_PRIMARY_SUBS,
                                     gstk_ds_sys_evt_cb,
                                     NULL,
                                     &dss_errno);

  if(gstk_shared_data.gstk_num_of_instances > 1)
  {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_ALL,
                                      DS_SYS_EVENT_SYSTEM_STATUS_EX,
                                      DS_SYS_SECONDARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);
  }

  if(gstk_shared_data.gstk_num_of_instances > 2)
  {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_ALL,
                                      DS_SYS_EVENT_SYSTEM_STATUS_EX,
                                      DS_SYS_TERTIARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);
  }

  if(0 > dss_ret_val)
  {
    UIM_MSG_ERR_2("DS SYS Status event reg failed ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
  }

  return GSTK_SUCCESS;
} /* gstk_ds_event_reg_sys_status_change */

/*===========================================================================
FUNCTION gstk_ds_event_reg_wlan_mac_addr_change

DESCRIPTION
  Function for having GSTK register with DS for WLAN MAC address changes.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_ds_event_reg_wlan_mac_addr_change(void)
{
  int16  dss_ret_val = 0;
  int16  dss_errno   = 0;

  UIM_MSG_HIGH_0("gstk_ds_event_reg_wlan_mac_addr_change");

  dss_ret_val =  ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                     DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
                                     DS_SYS_PRIMARY_SUBS,
                                     gstk_ds_sys_evt_cb,
                                     NULL,
                                     &dss_errno);
  if(0 > dss_ret_val)
  {
    UIM_MSG_ERR_2("DS WLAN MAC address change event reg for Primary Subscription failed ret_val=0x%x, err 0x%x",
                  dss_ret_val, dss_errno);
  }

  if(gstk_shared_data.gstk_num_of_instances > 1)
  {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
                                      DS_SYS_SECONDARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS WLAN MAC address change event reg for Secondary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
  }

 if(gstk_shared_data.gstk_num_of_instances>2)
 {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
                                      DS_SYS_TERTIARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS MAC Address change event reg for Tertiary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
 }
  return GSTK_SUCCESS;
} /* gstk_ds_event_reg_wlan_mac_addr_change */

/*===========================================================================
FUNCTION gstk_ds_event_reg_wlan_ssid_change

DESCRIPTION
  Function for having GSTK register with DS for WLAN SSID changes.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_ds_event_reg_wlan_ssid_change(void)
{
  int16  dss_ret_val = 0;
  int16  dss_errno   = 0;

  UIM_MSG_HIGH_0("gstk_ds_event_reg_wlan_ssid_change");

  dss_ret_val =  ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                     DS_SYS_EVENT_WLAN_SSID_CHANGE,
                                     DS_SYS_PRIMARY_SUBS,
                                     gstk_ds_sys_evt_cb,
                                     NULL,
                                     &dss_errno);
  if(0 > dss_ret_val)
  {
    UIM_MSG_ERR_2("DS WLAN SSID change event reg for Primary Subscription failed ret_val=0x%x, err 0x%x",
                  dss_ret_val, dss_errno);
  }

  if(gstk_shared_data.gstk_num_of_instances > 1)
  {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_SSID_CHANGE,
                                      DS_SYS_SECONDARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS WLAN SSID change event reg for Secondary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
  }

  if(gstk_shared_data.gstk_num_of_instances > 2)
  {
   dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_SSID_CHANGE,
                                      DS_SYS_TERTIARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS SSID change event reg for Tertiary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
  }

  return GSTK_SUCCESS;
} /* gstk_ds_event_reg_wlan_ssid_change */

/*===========================================================================
FUNCTION gstk_ds_sys_init

DESCRIPTION
  Function for having GSTK register with DS SYS.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_ds_sys_init(void)
{
  (void)gstk_ds_event_reg_sys_status_change();

  /* Register for WLAN status changes */
  (void)gstk_ds_event_reg_wlan_mac_addr_change();
  (void)gstk_ds_event_reg_wlan_ssid_change();

  return GSTK_SUCCESS;
} /* gstk_ds_sys_init */

#if defined FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*  FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================
FUNCTION GSTK_TASK

DESCRIPTION
  GSTK Main Task

PARAMETER
  dummy [Input]:  no use!

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_task( dword dummy )
{
  RCEVT_SIGEX_SIGREX              rcevt_siginfo_qmi_modem;
  RCEVT_SIGEX_SIGREX              rcevt_siginfo_cm;
  RCEVT_SIGEX_SIGREX              rcevt_siginfo_rrc;
#ifdef FEATURE_TDSCDMA
  RCEVT_SIGEX_SIGREX              rcevt_siginfo_tds_rrc;
#endif /*FEATURE_TDSCDMA*/
  gstk_status_enum_type           gstk_status               = GSTK_SUCCESS;
  gstk_task_cmd_type              *task_cmd_ptr             = NULL;
  /* Define sigs variable */
  rex_sigs_type                   gstk_sigs;
  mmgsdi_return_enum_type         mmgsdi_status             = MMGSDI_SUCCESS;
  rex_tcb_type*                   gstk_tcb_ptr              = rex_self();
  RCEVT_SIGEX_SIGREX              stop_sig;
  RCINIT_GROUP                    gstk_group;


  UIM_MSG_LOW_0("STARTING gstk_task");

#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_TDSCDMA
  gstk_pi_tdscdma = NULL;
  if(create_instance(CLSID_TDSCDMA, &gstk_pi_tdscdma) != E_SUCCESS)
  {
    UIM_MSG_HIGH_0("create_instance() for CLSID_TDSCDMA failed!");
    gstk_pi_tdscdma = NULL;
  }
#endif /*FEATURE_TDSCDMA*/

  gstk_pi_wcdma = NULL;
  if(create_instance(CLSID_WCDMA, &gstk_pi_wcdma) != E_SUCCESS)
  {
    UIM_MSG_HIGH_0("create_instance() for CLSID_WCDMA failed!");
    gstk_pi_wcdma = NULL;
  }
#endif /* FEATURE_SEGMENT_LOADING */

  stop_sig.signal = gstk_tcb_ptr;
  stop_sig.mask = GSTK_TASK_STOP_SIG;

  gstk_group = rcinit_lookup_group_rextask(gstk_tcb_ptr);
  rcinit_register_term_group(gstk_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);

  rcevt_siginfo_qmi_modem.signal = gstk_tcb_ptr;
  rcevt_siginfo_qmi_modem.mask   = GSTK_RCEVT_QMI_MODEM_TASK_READY_SIG;
  (void)rcevt_register_sigex_name("QMI_MODEM: READY", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_qmi_modem);

  rcevt_siginfo_cm.signal = gstk_tcb_ptr;
  rcevt_siginfo_cm.mask   = GSTK_RCEVT_CM_TASK_READY_SIG;
  (void)rcevt_register_sigex_name("cm:ready", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_cm);

#ifdef FEATURE_SEGMENT_LOADING
  if(gstk_pi_wcdma != NULL)
#endif /* FEATURE_SEGMENT_LOADING */
  {
    rcevt_siginfo_rrc.signal = gstk_tcb_ptr;
    rcevt_siginfo_rrc.mask   = GSTK_RCEVT_RRC_TASK_READY_SIG;
    (void)rcevt_register_sigex_name("rrc:ready", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_rrc);
  }

#ifdef FEATURE_SEGMENT_LOADING
  if(gstk_pi_tdscdma != NULL)
#endif /* FEATURE_SEGMENT_LOADING */
  {
#ifdef FEATURE_TDSCDMA
    rcevt_siginfo_tds_rrc.signal = gstk_tcb_ptr;
    rcevt_siginfo_tds_rrc.mask   = GSTK_RCEVT_TDS_RRC_TASK_READY_SIG;
    (void)rcevt_register_sigex_name("TDSRRC:ready", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_tds_rrc);
#endif /*FEATURE_TDSCDMA*/
  }

  /* Clear All Signals */
  (void) rex_clr_sigs(UIM_GSTK_TCB, (rex_sigs_type)~0);

  /* Init QMI CAT globals and queues */
  qmi_cat_init_pre_startup();

  /* Handshake with RCInit framework */
  rcinit_handshake_startup();

  /* Init gstk task */
  gstk_init_post_task_startup();

  if(gstk_curr_inst_ptr == NULL)
  {
    ERR_FATAL("gstk_curr_inst_ptr is NULL", 0, 0, 0);
  }

/* Perform various registrations only for off-target. For on target the
   registrations are performed when the corresponding tasks are up */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  /* Support Dynamic WDOG registration */

  gstk_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) GSTK_RPT_TIMER_SIG);

  /* Set timer for clients to register */
  (void) rex_set_timer(&gstk_shared_data.gstk_clients_reg_timer, GSTK_CLIENTS_REG_WAIT_TIME);

  /* Get client id from mmgsdi */
  mmgsdi_status = mmgsdi_client_id_and_evt_reg(
                    gstk_mmgsdi_events_cb,
                    (mmgsdi_callback_type)gstk_mmgsdi_cmd_cb,
                    0);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_client_id_reg() result 0x%x", mmgsdi_status);
  }

#ifdef FEATURE_GSTK_TAL
 (void)gstk_tal_init();
  if(gstk_tal_client_registration(GSTK_TAL_LOCAL_CLIENT,
                                  gstk_tal_client_cb,
                                  0) == GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("TAL registration successful for LOCAL");
  }

  memset(gstk_tal_client_req_table, 0x00, sizeof(gstk_tal_client_req_table));

#endif /* FEATURE_GSTK_TAL */
#ifdef FEATURE_GSTK_FUSION_REMOTE
  if(gstk_tal_client_registration(GSTK_TAL_REMOTE_CLIENT,
                                  gstk_tal_client_cb,
                                  0) == GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("TAL registration successful for REMOTE");
  }
#endif /*FEATURE_GSTK_FUSION_REMOTE*/

  /* Register with RRC to receive cell change indications for
   * Provide Local Info - Location Information proactive command
   * Location Status - envelope cmd
     only for off-target. For on target this is performed when the
     RRC and TDSCDMA tasks notify that they are up and running */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/

  gstk_shared_data.gstk_requested_mask = GSTK_TASK_CMD_Q_SIG |
                                              GSTK_TASK_STOP_SIG      |
#if defined FEATURE_UIM_TEST_FRAMEWORK
                                              #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
                                              GSTK_TASK_OFFLINE_SIG   |
                                              GSTK_ESTK_CMD_SIG       |
#ifdef FEATURE_LTE
                                              GSTK_RRC_MSG_Q_SIG      |
#endif /*FEATURE_LTE*/
                                              GSTK_QMI_CAT_SIG|
                                             GSTK_RCEVT_CM_TASK_READY_SIG |
                                              GSTK_RCEVT_RRC_TASK_READY_SIG |
                                              GSTK_RCEVT_QMI_MODEM_TASK_READY_SIG
#ifdef FEATURE_TDSCDMA
                                            | GSTK_RCEVT_TDS_RRC_TASK_READY_SIG
#endif /*FEATURE_TDSCDMA*/
  ;

  (void)rcevt_signal_name("GSTK:READY");

  for ( ; ; )
  {
    /* Add Wait Code and Wait Function Here */
    gstk_sigs = gstk_wait(gstk_shared_data.gstk_requested_mask);

#if defined FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /*defined FEATURE_UIM_TEST_FRAMEWORK */
    if ( gstk_sigs & GSTK_TASK_STOP_SIG ) {
      UIM_MSG_HIGH_0("Got STOP Sig");
      (void) rex_clr_sigs(UIM_GSTK_TCB, GSTK_TASK_STOP_SIG);
      gstk_status = gstk_power_down();
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_0("Err in proc stop Sig");
      }
      rcinit_unregister_term_group(gstk_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
      rcinit_handshake_term();
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
    }
    else if (gstk_sigs & GSTK_TASK_OFFLINE_SIG ) {
      UIM_MSG_HIGH_0("Got Offline Sig");
      (void) rex_clr_sigs(UIM_GSTK_TCB, TASK_OFFLINE_SIG);
    }
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    else if (gstk_sigs & GSTK_RCEVT_QMI_MODEM_TASK_READY_SIG)
    {
      UIM_MSG_HIGH_0("GSTK - QMI modem task is ready");
      (void)rcevt_unregister_sigex_name("QMI_MODEM: READY", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_qmi_modem);
      rex_clr_sigs(rex_self(), GSTK_RCEVT_QMI_MODEM_TASK_READY_SIG);
      /* Make sure we don't process the signal multiple times incase the signal is
         erroneously received more than once */
      if(!(gstk_shared_data.ready_tasks & (00000001 << (uint8)GSTK_QMI_MODEM_TASK)))
      {
        /* Perform qmi_cat_init irrespective of whether FTM NV is set or not */
        qmi_cat_init_post_startup();
        /* Mark that the task QMI MODEM is ready */
        gstk_shared_data.ready_tasks |= 0x00000001 << (uint8)GSTK_QMI_MODEM_TASK;
#ifdef FEATURE_UIM_MULTI_PROFILE
        if ((gstk_shared_data.ready_tasks & (00000001 << (uint8) GSTK_CM_TASK)))
        {
          gstk_util_check_slot_with_fetch_rejected();
        }
#endif /* FEATURE_UIM_MULTI_PROFILE */
      }
    }
    else if (gstk_sigs & GSTK_RCEVT_CM_TASK_READY_SIG)
    {
      UIM_MSG_HIGH_0("GSTK - CM task is ready");
      (void)rcevt_unregister_sigex_name("cm:ready", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_cm);
      rex_clr_sigs(rex_self(), GSTK_RCEVT_CM_TASK_READY_SIG);
      /* Make sure we don't process the signal multiple times incase the signal is
         erroneously received more than once */
      if(!(gstk_shared_data.ready_tasks & (00000001 << (uint8)GSTK_CM_TASK)))
      {
        /* Register with CM */
        gstk_cm_init();

#ifndef FEATURE_ESTK
        /*
          Since FEATURE_ESTK not enabled, we need to register gstk_wms_event_cb to still receive
          WMS_MSG_EVENT_MT_MESSAGE_ERROR
        */
        gstk_wms_init();
#endif
        /* Perform gstk tal init for multimodem targets and gstk_ds_sys_init once we get CM task
           ready signal. The assumption is that PS, WMS and DS init would also
           already have been completed by TMC by the time we received task
           ready signal for CM */
#ifdef FEATURE_GSTK_TAL
         (void)gstk_tal_external_init();
#endif /* FEATURE_GSTK_TAL */
        /* Register with DS */
        gstk_ds_sys_init();

          /* Mark that the task CM is ready */
        gstk_shared_data.ready_tasks |= 0x00000001 << (uint8)GSTK_CM_TASK;
#ifdef FEATURE_UIM_MULTI_PROFILE
        if ((gstk_shared_data.ready_tasks & (00000001 << (uint8) GSTK_QMI_MODEM_TASK)))
        {
          gstk_util_check_slot_with_fetch_rejected();
        }
#endif /* FEATURE_UIM_MULTI_PROFILE */
      }
    }
    else if (gstk_sigs & GSTK_RCEVT_RRC_TASK_READY_SIG)
    {
      UIM_MSG_HIGH_0("GSTK - RRC task is ready");
      (void)rcevt_unregister_sigex_name("rrc:ready", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_rrc);
      rex_clr_sigs(rex_self(), GSTK_RCEVT_RRC_TASK_READY_SIG);
      /* Make sure we don't process the signal multiple times incase the signal is
         erroneously received more than once */
      if(!(gstk_shared_data.ready_tasks & (00000001 << (uint8)GSTK_RRC_TASK)))
      {
        /* Register with RRC to receive cell change indications for
         * Provide Local Info - Location Information proactive command
         * Location Status - envelope cmd
         */
#if defined(FEATURE_WCDMA)
#ifdef FEATURE_SEGMENT_LOADING
        if(gstk_pi_wcdma != NULL)
        {
          UIM_MSG_HIGH_0("GSTK - Register cell change ind callback with RRC task");
          (void)IWCDMA_rrc_register_serving_cell_change_ind_cb(gstk_pi_wcdma, gstk_rrc_cell_change_ind_cb);
        }
        else
        {
          UIM_MSG_ERR_0("GSTK - WCDMA interface pointer is NULL!");
        }
#else
        (void)rrc_register_serving_cell_change_ind_cb(gstk_rrc_cell_change_ind_cb);
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA && FEATURE_GSTK_FOR_NON_LTK_BUILD */
        /* Mark that the RRC task is ready */
        gstk_shared_data.ready_tasks |= 0x00000001 << (uint8)GSTK_RRC_TASK;
      }
    }
#ifdef FEATURE_TDSCDMA
    else if (gstk_sigs & GSTK_RCEVT_TDS_RRC_TASK_READY_SIG)
    {
      UIM_MSG_HIGH_0("GSTK - TDSRRC task is ready");
      (void)rcevt_unregister_sigex_name("TDSRRC:ready", RCEVT_SIGEX_TYPE_SIGREX, &rcevt_siginfo_tds_rrc);
      rex_clr_sigs(rex_self(), GSTK_RCEVT_TDS_RRC_TASK_READY_SIG);
      /* Make sure we don't process the signal multiple times incase the signal is
         erroneously received more than once */
      if(!(gstk_shared_data.ready_tasks & (00000001 << (uint8)GSTK_TDS_RRC_TASK)))
      {
#ifdef FEATURE_SEGMENT_LOADING
        if(gstk_pi_tdscdma != NULL)
        {
          (void)ITDSCDMA_tdsrrc_register_serving_cell_change_ind_cb(gstk_pi_tdscdma, gstk_tdscdma_rrc_cell_change_ind_cb);
        }
        else
        {
          UIM_MSG_ERR_0("GSTK - TDSCDMA interface pointer is NULL!");
        }
#else
        (void)tdsrrc_register_serving_cell_change_ind_cb(gstk_tdscdma_rrc_cell_change_ind_cb);
#endif /* FEATURE_SEGMENT_LOADING */
        /* Mark that the TDSRRC task is ready */
        gstk_shared_data.ready_tasks |= 0x00000001 << (uint8)GSTK_TDS_RRC_TASK;
      }
    }
#endif /*FEATURE_TDSCDMA*/
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
#ifdef FEATURE_LTE
    else if ( gstk_sigs & GSTK_RRC_MSG_Q_SIG)
    {
      /* clear the signal */
      (void) rex_clr_sigs(UIM_GSTK_TCB, GSTK_RRC_MSG_Q_SIG);
      gstk_process_rrc_msgr_cmd();
    }
#endif /*FEATURE_LTE*/
    else if ( gstk_sigs & GSTK_TASK_CMD_Q_SIG )
    {
      /* other non timer sig */
      /* clear the signal */
      (void) rex_clr_sigs(UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG);
      while((q_cnt( &gstk_task_cmd_q ) != 0) || (q_cnt(&gstk_task_priority_cmd_q) != 0))
      {
        if(!gstk_shared_data.gstk_continue_task)
        {
          (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG);
          break;
        }
        if(q_cnt(&gstk_task_priority_cmd_q) != 0)
        {
          task_cmd_ptr = (gstk_task_cmd_type*) q_get(&gstk_task_priority_cmd_q);
        }
        else
        {
          /* Entering gstk critical section */
          rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
          /* get the command from gstk_cmd_q to be processed */
          task_cmd_ptr = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                                    gstk_compare_specific_cmds,
                                                                    NULL,
                                                                    NULL,
                                                                    NULL);
          rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
          /* Leaving gstk critical section */

          if (NULL == task_cmd_ptr)
          {
              /* No cmd was dequeued possibly because all are event downloads or bip envelops
                 with a pending TR*/
            gstk_shared_data.gstk_requested_mask &= ~GSTK_TASK_CMD_Q_SIG;
            break; /* break out of the while loop */
          }
        }

        if(task_cmd_ptr != NULL)
        {
#ifdef FEATURE_UIM_TEST_FRAMEWORK
          #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
          /* GSTK instance context switching */
          if (!gstk_switch_to_instance(
                task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id))
          {
            UIM_MSG_ERR_2("ignoring cmd 0x%x 0x%x!",
                          task_cmd_ptr->cmd.general_cmd.message_header.command_group,
                          task_cmd_ptr->cmd.general_cmd.message_header.command_id);
            gstk_task_free_cmd_buf(task_cmd_ptr);
            task_cmd_ptr = NULL;
            continue;
          }
          gstk_status = gstk_process_state(&(task_cmd_ptr->cmd));
          gstk_task_free_cmd_buf(task_cmd_ptr);
          task_cmd_ptr = NULL;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
          #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
        }

      } /* while(q_cnt( &gstk_task_cmd_q ) != 0) */
    } /* GSTK_TASK_CMD_Q_SIG */
    else if ( gstk_sigs & GSTK_ESTK_CMD_SIG )
    {
      UIM_MSG_LOW_0("GOT GSTK_ESTK_CMD_SIG");

      if(!(gstk_shared_data.gstk_requested_mask & GSTK_TASK_CMD_Q_SIG))
      {
        gstk_shared_data.gstk_requested_mask |= GSTK_TASK_CMD_Q_SIG;
      }

      /* clear the signal */
      (void) rex_clr_sigs(UIM_GSTK_TCB, GSTK_ESTK_CMD_SIG);
#ifdef FEATURE_ESTK
      gstk_status = estk_process_cmd();
#endif /*FEATURE_ESTK */
    }
    else if(gstk_sigs & GSTK_QMI_CAT_SIG)
    {
      (void)rex_clr_sigs(UIM_GSTK_TCB, GSTK_QMI_CAT_SIG);
      if(qmi_cat_sig_handler() == FALSE)
      {
        /* If QMI CAT command queue is not empty, reset signal to
           continue processing commands */
        (void)rex_set_sigs(UIM_GSTK_TCB, GSTK_QMI_CAT_SIG);
      }
    }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
#if defined FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /*  FEATURE_UIM_TEST_FRAMEWORK */
} /* gstk_task */

/*lint -e715 "param_ptr not referenced " */
/*===========================================================================
FUNCTION GSTK_COMPARE_SPECIFIC_CMDS

DESCRIPTION
  GSTK Compare function callback that is called by q_linear_delete_new for
  cmd in the gstk_task_cmd_q. This function will return TRUE only when the
  command id is that of a Terminal Response, Proactive Command, an Envelope
  response or Envelope for a non event download and non BIP envelop commands.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete_new()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int gstk_compare_specific_cmds(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type     *task_cmd_ptr     = NULL;
  gstk_slot_id_enum_type  sim_slot_id      = GSTK_SLOT_ID_MAX;

  (void)param_ptr;

  if(item_ptr == NULL)
  {
    return 0;
  }

  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  sim_slot_id  = task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id;

  /* If sim_slot_id is not a valid, then get the command and delete the command */
  if(!GSTK_IS_VALID_SLOT_ID(sim_slot_id))
  {
    return 1;
  }

  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD)
  {
    UIM_MSG_HIGH_4("GSTK env state= %d, gstk_proactive_cmd_in_progress = 0x%x, gstk_bip_env_in_progress = 0x%x, cmd_id = 0x%x",
                   gstk_instances_ptr[sim_slot_id-1]->gstk_curr_envelope_info.gstk_env_state,
                   gstk_instances_ptr[sim_slot_id-1]->gstk_proactive_cmd_in_progress,
                   gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress,
                   task_cmd_ptr->cmd.general_cmd.message_header.command_id);

    if(gstk_util_has_pending_setup_call() &&
       gstk_instances_ptr[sim_slot_id - 1]->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_NONE)
    {
      /* Process call connect and call disconnect event downloads after setup call TR */
      switch((gstk_cmd_enum_type)task_cmd_ptr->cmd.general_cmd.message_header.command_id)
      {
        case GSTK_CALL_CONNECTED_EVT_IND:
        case GSTK_CALL_DISCONNECTED_EVT_IND:
          return 0;
        default:
          break;
      }
    }

    if(gstk_instances_ptr[sim_slot_id - 1]->gstk_sub_state == GSTK_RECOVERY_SUB_S)
    {
      switch((gstk_cmd_enum_type)task_cmd_ptr->cmd.general_cmd.message_header.command_id)
      {
        case GSTK_CC_IND:
          return 0;
        default:
          break;
      }
    }

    /* Check if Event Download Envelope commands for the same slot have to be skipped */
    if (gstk_instances_ptr[sim_slot_id - 1]->gstk_curr_envelope_info.gstk_env_state != GSTK_SUB_ST_PENDING_ENV_NONE)
    {
      /* Since an evt dl is still being retried, process only proactive commands, TRs and
         non evt dl envelope commands.*/
      switch((gstk_cmd_enum_type)task_cmd_ptr->cmd.general_cmd.message_header.command_id)
      {
        case GSTK_MT_CALL_EVT_IND:
        case GSTK_CALL_CONNECTED_EVT_IND:
        case GSTK_CALL_DISCONNECTED_EVT_IND:
        case GSTK_IDLE_SCRN_EVT_IND:
        case GSTK_LOCATION_STATUS_EVT_IND:
        case GSTK_USER_ACTIVITY_EVT_IND:
        case GSTK_LANG_SELECTION_EVT_IND:
        case GSTK_BROWSER_TERMINATION_EVT_IND:
        case GSTK_DATA_AVAIL_EVT_IND:
        case GSTK_CH_STATUS_EVT_IND:
        case GSTK_ACCESS_TECH_EVT_IND:
        case GSTK_NW_SEARCH_MODE_EVT_IND:
        case GSTK_NW_REJ_EVT_IND:
        case GSTK_IMS_REG_EVT_IND:
          return 0;
        case GSTK_CC_IND:
        case GSTK_MO_SMS_CTRL_IND:
          if (!gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY,
                                           sim_slot_id))
          {
            return 0;
          }
          break;
        case GSTK_LOCATION_INFORMATION_IND:
          /* we shall process ATTACH CTRL cmd but not VPE */
          return (task_cmd_ptr->cmd.general_cmd.message_header.user_data ==
                  GSTK_MAX_PROACTIVE_COMMAND);
        default:
          return 1;
      }/* switch */
    }/*if (... != GSTK_SUB_ST_PENDING_ENV_NONE)*/
    /* Check if BIP envelope commands have to be skipped */
    else if(gstk_instances_ptr[sim_slot_id - 1]->gstk_proactive_cmd_in_progress == TRUE ||
            gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress == TRUE)
    {
      /* BIP Envelope command should not be sent to the card in either of following two cases:
             1> if gstk has allowed UIM to do fetch and a TR for that has not been sent yet
                (i.e. gstk_instances_ptr[sim_slot_id - 1]->gstk_proactive_cmd_in_progress is TRUE)
             2> or if gstk has already sent a BIP Envelope command and has not received a response
                for it yet(i.e.gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress is TRUE)
             As UIM can not keep track of which toolkit command it is processing and what toolkit command
             it has processed, we are keeping this logic in gstk and thus preventing things from getting
             mixed up in UIM.
             We perform these checks only when gstk is going to process the BIP ENV cmd (i.e. when it is
             in the front of the queue and it is going to be removed from the queue). BIP Env cmd is of
             no harm in any of the above two cases if its just sitting in the queue - but if its going to
             be processed then we need to check the above two conditions and accordingly skip the bip env cmd. */
      switch(task_cmd_ptr->cmd.general_cmd.message_header.command_id)
      {
        case GSTK_DATA_AVAIL_EVT_IND:
          if(task_cmd_ptr->cmd.data_avail_evt_envelope_ind.ch_status_info.is_scws_ch)
          {
            gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress = TRUE;
            return 1;
          }
          else
          {
            return 0;
          }
        case GSTK_CH_STATUS_EVT_IND:
          if(task_cmd_ptr->cmd.ch_status_evt_envelope_ind.ch_status_info.is_scws_ch)
          {
            gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress = TRUE;
            return 1;
          }
          else
          {
            return 0;
          }
        case GSTK_CC_IND:
          if (gstk_nv_get_feature_2_status(GSTK_CFG_FEATURE_2_SLOT_BLOCK_CALL_CTRL_DURING_SEND_RECEIVE_DATA,
                                           sim_slot_id) &&
              gstk_instances_ptr[sim_slot_id - 1]->gstk_block_cc_env)
          {
            return 0;
          }
          else
          {
            gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress = TRUE;
            return 1;
          }
        default:
          return 1;
      }/* switch */
    }/*else if(...)*/
  }/*  if(cmd_p->general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD) */

  /* set BIP envelope progress flag just before de-queue */
  switch(task_cmd_ptr->cmd.general_cmd.message_header.command_id)
  {
    case GSTK_DATA_AVAIL_EVT_IND:
    case GSTK_CH_STATUS_EVT_IND:
      gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress = TRUE;
      break;
    case GSTK_CC_IND:
      if (gstk_nv_get_feature_2_status(GSTK_CFG_FEATURE_2_SLOT_BLOCK_CALL_CTRL_DURING_SEND_RECEIVE_DATA,
                                       sim_slot_id) &&
          gstk_instances_ptr[sim_slot_id - 1]->gstk_block_cc_env)
      {
        gstk_instances_ptr[sim_slot_id - 1]->gstk_bip_env_in_progress = TRUE;
      }
      break;
    default:
      break;
  }

  return 1;
} /* gstk_compare_specific_cmds */
/*lint +e715 "param_ptr not referenced " */

/*===========================================================================
FUNCTION gstk_task_get_cmd_buf

DESCRIPTION
  This function get the gstk_task_cmd_type from the GSTK free queue.
  Calling client SHOULD NOT change information on the gstk_task_cmd_type's
  header.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_task_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_task_cmd_type *gstk_task_get_cmd_buf ( void  )
{
  gstk_task_cmd_type * cmd_p = NULL;
  /* Allocate command buffer */

  cmd_p = (gstk_task_cmd_type *)GSTK_CALLOC(sizeof(gstk_task_cmd_type));

  return cmd_p;
} /* gstk_task_cmd_type */

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF

DESCRIPTION
  This function put the command onto gstk task command queue

PARAMETER
  cmd_ptr [Input]: Command pointer to be put on the gstk queue

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_task_put_cmd_buf (
  gstk_task_cmd_type *cmd_ptr
)
{
  /* Check for Null pointer */
  if (cmd_ptr != NULL) {
    UIM_MSG_HIGH_2("Queuing cmd: 0x%x 0x%x",
                   cmd_ptr->cmd.general_cmd.message_header.command_group,
                   cmd_ptr->cmd.general_cmd.message_header.command_id);

    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);

    (void)q_link(cmd_ptr, &cmd_ptr->cmd_hdr.link);

    /* Put the message on the queue */
    q_put(&gstk_task_cmd_q, &cmd_ptr->cmd_hdr.link);

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG);
    if((gstk_shared_data.gstk_requested_mask != 0) &&
       !(gstk_shared_data.gstk_requested_mask & GSTK_TASK_CMD_Q_SIG))
    {
      (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_ESTK_CMD_SIG);
    }
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  }
} /* gstk_task_put_cmd_buf */

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF_TO_HEAD

DESCRIPTION
  This function puts the command at the front of the gstk task command queue

PARAMETER
  cmd_ptr [Input]: Command pointer to be put on the gstk queue

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_task_put_cmd_buf_to_head (
  gstk_task_cmd_type *cmd_ptr
)
{
  /* Check for Null pointer */
  if (cmd_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_2("Queuing cmd to head: 0x%x 0x%x",
                 cmd_ptr->cmd.general_cmd.message_header.command_group,
                 cmd_ptr->cmd.general_cmd.message_header.command_id);
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->cmd_hdr.link );

  if ( q_cnt(&gstk_task_cmd_q) == 0 )
  {
    q_put( &gstk_task_cmd_q, & cmd_ptr->cmd_hdr.link );
  }
  else
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert(&gstk_task_cmd_q,
             &cmd_ptr->cmd_hdr.link,
             &((gstk_task_cmd_type*) q_check(&gstk_task_cmd_q))->cmd_hdr.link );
#else
    q_insert(&cmd_ptr->hdr.cmd_hdr.link,
             &((gstk_task_cmd_type*) q_check(&gstk_task_cmd_q))->cmd_hdr.link );
#endif /* FEATURE_Q_NO_SELF_QPTR */
  }

  /* Set the command queue signal for */
  /* receive task                     */
  (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG);
  if(!(gstk_shared_data.gstk_requested_mask & GSTK_TASK_CMD_Q_SIG))
  {
    (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_ESTK_CMD_SIG);
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
} /* gstk_task_put_cmd_buf_to_front_of_q */

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_PRIORITY_BUF

DESCRIPTION
  This function put the command onto gstk task command queue

PARAMETER
  cmd_ptr [Input]: Command pointer to be put on the gstk queue

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_task_put_cmd_priority_buf (
  gstk_task_cmd_type *cmd_ptr
)
{
  /* Check for Null pointer */
  if (cmd_ptr != NULL) {
    UIM_MSG_HIGH_2("Queuing cmd to priority Queue: 0x%x 0x%x",
                   cmd_ptr->cmd.general_cmd.message_header.command_group,
                   cmd_ptr->cmd.general_cmd.message_header.command_id);

    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);

    (void)q_link(cmd_ptr, &cmd_ptr->cmd_hdr.link);

    /* Put the message on the queue */
    q_put(&gstk_task_priority_cmd_q, &cmd_ptr->cmd_hdr.link);

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_TASK_CMD_Q_SIG);
    if((gstk_shared_data.gstk_requested_mask != 0) &&
       !(gstk_shared_data.gstk_requested_mask & GSTK_TASK_CMD_Q_SIG))
    {
      (void) rex_set_sigs(UIM_GSTK_TCB, GSTK_ESTK_CMD_SIG);
    }
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  }
} /* gstk_task_put_cmd_priority_buf */

/*===========================================================================
FUNCTION GSTK_CMD

DESCRIPTION
  This function allows clients to put the gstk_cmd onto the GSTK command
  queue.

PARAMETER
  gstk_cmd [Input]:  Pointer of the task_cmd_type

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cmd (
  gstk_task_cmd_type *gstk_task_cmd
)
{
  if (gstk_task_cmd != NULL) 
  {
    /* put command back to command queue */
    gstk_task_put_cmd_buf(gstk_task_cmd);
  }
} /* gstk_cmd */

/*===========================================================================
FUNCTION gstk_attach_ctrl_timer_exp_cb

DESCRIPTION

  This function build the timer expiration command if the attach query timer
  expires

PARAMETERS
  slot - [Input] slot of which GSTK instance's timer expires

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_attach_ctrl_timer_exp_cb(
  unsigned long  slot
)
{
  gstk_task_cmd_type    *task_cmd_ptr = NULL;

  UIM_MSG_HIGH_1("gstk_attach_timer_exp_cb(): 0x%x", slot);

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->cmd.timer_msg.message_header.sim_slot_id =
    (gstk_slot_id_enum_type)slot;
  task_cmd_ptr->cmd.timer_msg.message_header.command_group = GSTK_TIMER_EXP_CMD;
  task_cmd_ptr->cmd.timer_msg.message_header.command_id = GSTK_ATTACH_CTRL_TIMER;
  gstk_task_put_cmd_buf(task_cmd_ptr);
} /*gstk_attach_ctrl_timer_exp_cb*/

/*===========================================================================
FUNCTION gstk_build_timer_expired_cmd

DESCRIPTION

  This function build the timer expiration command based on the passed in
  timer type and put onto the command queue

PARAMETERS
  gstk_timer_type - [Input] gstk_timer_enum_type, use to build the command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_build_timer_expired_cmd(
  unsigned long  gstk_timer_type
)
{
  gstk_task_cmd_type    *task_cmd_ptr = NULL;
  uint32                ref_id        = 0;

  UIM_MSG_HIGH_1("Timer expired! 0x%x", gstk_timer_type);

  if ((gstk_timer_enum_type)gstk_timer_type >= GSTK_TR_TIMER_INVALID)
  {
    return;
  }

  switch (gstk_timer_type)
  {
  case GSTK_TP_TIMER:
  case GSTK_LOC_INFO_TIMER:
    for (ref_id = 0; ref_id < gstk_shared_data.gstk_num_of_instances; ++ ref_id)
    {
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL)
      {
        task_cmd_ptr->cmd.timer_msg.message_header.sim_slot_id =
          (gstk_slot_id_enum_type)(ref_id + 1);
        task_cmd_ptr->cmd.timer_msg.message_header.command_group =
          GSTK_TIMER_EXP_CMD;
        task_cmd_ptr->cmd.timer_msg.message_header.command_id =
          (uint32)gstk_timer_type;
        /* put command back to command queue */
        gstk_task_put_cmd_buf(task_cmd_ptr);
      }
    }
    break;

  case GSTK_CLIENTS_REG_TIMER:
    task_cmd_ptr = gstk_task_get_cmd_buf();
    if (task_cmd_ptr != NULL)
    {
      task_cmd_ptr->cmd.timer_msg.message_header.sim_slot_id = GSTK_SLOT_AUTO;
      task_cmd_ptr->cmd.timer_msg.message_header.command_group = GSTK_TIMER_EXP_CMD;
      task_cmd_ptr->cmd.timer_msg.message_header.command_id = (uint32)gstk_timer_type;
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
    else
    {
      UIM_MSG_ERR_0("Failed to allocate memory - GSTK_CLIENTS_REG_TIMER");
    }
    break;

  default:
    if (gstk_timer_type < GSTK_MAX_TIMER_SUPPORTED)
    {
      /* get the command from buffer */
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL) {
        /* find corresponding entry in command_details_ref_table_ptr */
        ref_id = uint64touint32(gstk_timer_type) - (uint32)GSTK_TR_TIMER1;

        /* Build the gstk_cmd */
        task_cmd_ptr->cmd.timer_msg.message_header.sim_slot_id =
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].sim_slot_id;
        task_cmd_ptr->cmd.timer_msg.message_header.command_group =
          GSTK_TIMER_EXP_CMD;
        task_cmd_ptr->cmd.timer_msg.message_header.command_id = gstk_timer_type;
        task_cmd_ptr->cmd.timer_msg.message_header.user_data =
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].unique_ref_id;

        /* put command back to command queue */
        gstk_task_put_cmd_buf(task_cmd_ptr);
      }
    }
    else
    {
      UIM_MSG_ERR_1("invalid timer type 0x%x", gstk_timer_type);
    }
  }
} /* gstk_build_timer_expired_cmd */

/*===========================================================================
FUNCTION gstk_terminal_rsp_delay_cb

DESCRIPTION

  This function build the timer expiration command for Terminal Response Delay Timer

PARAMETERS
  gstk_instance - [Input] Used to identify the GSTK instance

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_terminal_rsp_delay_cb(
  unsigned long  gstk_instance
)
{
  gstk_task_cmd_type    *task_cmd_ptr = NULL;

  if (!GSTK_IS_VALID_SLOT_ID(gstk_instance + 1))
  {
    return;
  }

  GSTK_RETURN_IF_NULL_PTR(gstk_instances_ptr[(uint8)gstk_instance]);

  if(gstk_instances_ptr[(uint8)gstk_instance]->gstk_cmd_cache.term_resp_length > 0)
  {
    task_cmd_ptr = gstk_task_get_cmd_buf();
    if (task_cmd_ptr != NULL)
    {
      task_cmd_ptr->cmd.timer_msg.message_header.sim_slot_id =
         (gstk_slot_id_enum_type)(gstk_instance + 1);
      task_cmd_ptr->cmd.timer_msg.message_header.command_group =
          GSTK_TIMER_EXP_CMD;
      task_cmd_ptr->cmd.timer_msg.message_header.command_id =
         (uint32)GSTK_TR_DELAY_TIMER;
          /* put command back to command queue */
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("No TR in cache");
  }
} /* gstk_terminal_rsp_delay_cb */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
FUNCTION gstk_cleanup_globals

DESCRIPTION

  This function
  - Deletes global variables
  - Deletes GSTK queue
  - Deletes GSTK timers

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cleanup_globals ()
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  return;
} /* gstk_cleanup_globals */


/*===========================================================================
FUNCTION gstk_power_down

DESCRIPTION

  This function
  - flushes GSTK queue
  - vote UIM off
  - Ack TMC

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_power_down ()
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  uint8 i = 0;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  /* vote UIM off */
  for(i=0; i < gstk_shared_data.gstk_num_of_instances; i++)
  {
    if(gstk_instances_ptr[i] == NULL)
    {
      continue;
    }
    gstk_instances_ptr[i]->gstk_state = GSTK_POWER_DOWN_S;
    (void)uim_power_control_ds((uim_slot_type)(UIM_SLOT_1 + i),UIM_USIM_SESSION, FALSE,
                                               UIM_CALL_TYPE_NOT_APPLICABLE);
  }
  /* Deregister with the watch dog timer */
  dog_hb_deregister(gstk_dog_rpt_var);

  return gstk_status;
} /* gstk_power_down */

/*===========================================================================
FUNCTION gstk_set_additional_info

DESCRIPTION
  Sets gstk_additional_info_type->additional_info to field.

PARAMETERS
  rec: The record to set the field in.
  out: The field used to set the record with.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_set_additional_info
(
  gstk_additional_info_type*          rec,
  gstk_additional_info_code_enum_type field
)
{
  if(rec != NULL)
  {
    rec->length = 1;
    rec->additional_info[0] = (uint8)field;
  }
} /* gstk_set_additional_info */

/*===========================================================================
FUNCTION gstk_set_command_id

DESCRIPTION
  Sets the command id.

PARAMETER
  header: The header to set the command_id for.
  command_id: The value to set the command_id to.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_set_command_id(gstk_hdr_type* header,
                         gstk_cmd_enum_type command_id)
{
  if(header != NULL)
  {
    header->command_id = (int)command_id;
  }
} /* gstk_set_command_id */

/*===========================================================================
FUNCTION gstk_gsdi_continue_task

DESCRIPTION
  Sets the flags

PARAMETER
  val indeicates whether the task should continue processing commands

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  If the flag is set to FALSE, GSTK will not process any commands.

SEE ALSO
  None
===========================================================================*/
void gstk_gsdi_continue_task(boolean val)
{
  gstk_shared_data.gstk_continue_task = val;
  (void) rex_set_sigs(UIM_GSTK_TCB,GSTK_TASK_CMD_Q_SIG);
} /* gstk_gsdi_continue_task */

/*===========================================================================
FUNCTION   gstk_check_for_pending_pro_cmd

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  none

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_for_pending_pro_cmd(void)
{
  int i = 0;

  for(i=0;i<GSTK_MAX_PROACTIVE_COMMAND;i++)
  {
    if (command_details_ref_table_ptr[i].free_slot == FALSE)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* gstk_check_for_pending_pro_cmd */

#if defined(FEATURE_WCDMA)
/*===========================================================================
FUNCTION   gstk_rrc_cell_change_ind_cb

DESCRIPTION
  Callback function invoked by RRC to indicate cell change.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_rrc_cell_change_ind_cb (
  cell_info_type              cell_info)
{
  gstk_cell_info_type gstk_cell_info;

  /* Copy cell_info field-by-field */
  gstk_cell_info.as_id = cell_info.as_id;
  gstk_cell_info.cell_id = cell_info.cell_id;
  if(gstk_memcpy(gstk_cell_info.lac,
                 cell_info.lac,
                 sizeof(gstk_cell_info.lac),                 
                 sizeof(gstk_cell_info.lac),
                 sizeof(cell_info.lac)) <
     sizeof(gstk_cell_info.lac))
  {
    return;
  }
  if(gstk_memcpy(&gstk_cell_info.plmn_id,
                 &cell_info.plmn_id,
                 sizeof(gstk_cell_info.plmn_id),                 
                 sizeof(gstk_cell_info.plmn_id),
                 sizeof(cell_info.plmn_id)) <
     sizeof(gstk_cell_info.plmn_id))
  {
    return;
  }
  gstk_cell_info.psc = cell_info.psc;
  gstk_cell_info.uarfcn_dl = cell_info.uarfcn_dl;
  gstk_cell_info.uarfcn_ul = cell_info.uarfcn_ul;

  gstk_send_cell_change_ind(gstk_cell_info, GSTK_ACCESS_TECH_UTRAN);
} /* gstk_rrc_cell_change_ind_cb */

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION   gstk_tdscdma_rrc_cell_change_ind_cb

DESCRIPTION
  Callback function invoked by TDSCDMA RRC to indicate cell change.

PARAMETERS
  cell_info  TDSCDMA serving cell information

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_tdscdma_rrc_cell_change_ind_cb (
  tdscell_info_type              cell_info)
{
  cell_info_type wcdma_cell;

  wcdma_cell.as_id = cell_info.as_id;
  wcdma_cell.cell_id = cell_info.cell_id;
  if(gstk_memcpy(wcdma_cell.lac,
                 cell_info.lac,
                 sizeof(wcdma_cell.lac),                 
                 sizeof(wcdma_cell.lac),
                 sizeof(cell_info.lac)) <
     sizeof(wcdma_cell.lac))
  {
    return;
  }
  if(gstk_memcpy(&wcdma_cell.plmn_id,
                 &cell_info.plmn_id,
                 sizeof(wcdma_cell.plmn_id),                 
                 sizeof(wcdma_cell.plmn_id),
                 sizeof(cell_info.plmn_id)) <
     sizeof(wcdma_cell.plmn_id))
  {
    return;
  }
  wcdma_cell.psc = cell_info.psc;
  wcdma_cell.uarfcn_dl = cell_info.uarfcn_dl;
  wcdma_cell.uarfcn_ul = cell_info.uarfcn_ul;
#if defined(FEATURE_WCDMA)
  gstk_rrc_cell_change_ind_cb(wcdma_cell);
#endif /* FEATURE_WCDMA */
} /*gstk_tdscdma_rrc_cell_change_ind_cb*/
#endif /*FEATURE_TDSCDMA*/

#if (defined(FEATURE_WCDMA) || defined(FEATURE_LTE))
/*===========================================================================
FUNCTION   gstk_send_cell_change_ind

DESCRIPTION
  Send cell change indication

PARAMETERS
  cell_info  the cell information
  rat        the access tech
DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_send_cell_change_ind(
  gstk_cell_info_type         cell_info,
  gstk_access_technology_type rat
)
{
  gstk_task_cmd_type*         task_cmd                       = NULL;
  byte                        nas_plmn_id[GSTK_MAX_PLMN_LEN] = {0};
  gstk_status_enum_type       gstk_status                    = GSTK_SUCCESS;
  gstk_slot_id_enum_type      slot                           = GSTK_SLOT_1;
  uint32                      eci                            = 0;
  uint8                       lac_byte                       = 0;
  gstk_cell_id_type           temp_cell_info                 = {0};
  uint8                       i                              = GSTK_RAT_MAX_IDX;

  /* convert modem AS ID to slot ID */
  if (gstk_io_ctrl(
          GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
          &cell_info.as_id,
          &slot) != GSTK_SUCCESS)
   {
      return;
  }

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  GSTK_RETURN_IF_NULL_PTR(gstk_instances_ptr[slot - 1]);

  UIM_MSG_HIGH_4("In gstk_send_cell_change_ind(), rat 0x%x apps enabled: 0x%x Card Protocol: 0x%x, is_first_loc_info_envelope_sent: 0x%x",
                 rat,
                 gstk_instances_ptr[slot - 1]->apps_enabled,
                 gstk_instances_ptr[slot - 1]->card_protocol,
                 gstk_instances_ptr[slot - 1]->is_first_loc_info_envelope_sent);

  /* Cache the cell id from RRC */
  switch (rat)
  {
    case GSTK_ACCESS_TECH_UTRAN:
      temp_cell_info.cell_id[1] = (uint8)(cell_info.cell_id & 0x000000FF);
      temp_cell_info.cell_id[0] = (uint8)((cell_info.cell_id >> 8) & 0x000000FF);
      temp_cell_info.cell_id[3] = (uint8)((cell_info.cell_id >> 16) & 0x000000FF);
      temp_cell_info.cell_id[2] = (uint8)((cell_info.cell_id >> 24) & 0x000000FF);
      i = GSTK_RAT_UMTS_IDX;
      break;
    case GSTK_ACCESS_TECH_LTE:
      /* refer to 3GPP 31.111 section 8.19 */
      eci = (cell_info.cell_id << 4) | 0x0000000F;
      temp_cell_info.cell_id[0] = (uint8)((eci >> 24) & 0x000000FF);
      temp_cell_info.cell_id[1] = (uint8)((eci >> 16) & 0x000000FF);
      temp_cell_info.cell_id[2] = (uint8)((eci >> 8) & 0x000000FF);
      temp_cell_info.cell_id[3] = (uint8)(eci & 0x000000FF);
      lac_byte = cell_info.lac[1];
      cell_info.lac[1] = cell_info.lac[0];
      cell_info.lac[0] = lac_byte;
      i = GSTK_RAT_LTE_IDX;
      break;
    default:
      UIM_MSG_ERR_0("invalid rat");
      return;
  }

  if (gstk_instances_ptr[slot - 1]->card_protocol == MMGSDI_UICC)
  {
    temp_cell_info.cell_len =  GSTK_MAX_CELL_ID_LEN;
  }
  else
  {
    temp_cell_info.cell_len =  GSTK_MIN_CELL_ID_LEN;
  }

  /* Set this to TRUE so that when MM ENTER IDle is received, an envelope
     can be sent down */
  gstk_instances_ptr[slot - 1]->is_rrc_cell_id_received[i] = TRUE;

  UIM_MSG_HIGH_9("RRC Plmn MCC: 0x%x 0x%x 0x%x Plmn MNC: 0x%x 0x%x 0x%x LAC: 0x%x 0x%x, Cell ID: 0x%x",
                 cell_info.plmn_id.mcc[0],
                 cell_info.plmn_id.mcc[1],
                 cell_info.plmn_id.mcc[2],
                 cell_info.plmn_id.mnc[0],
                 cell_info.plmn_id.mnc[1],
                 cell_info.plmn_id.mnc[2],
                 cell_info.lac[0],
                 cell_info.lac[1],
                 cell_info.cell_id);

  gstk_status =  gstkutil_convert_rrc_plmn_id_to_nas_plmn_id(cell_info.plmn_id,
                                                             nas_plmn_id);

  if(gstk_status == GSTK_ERROR)
  {
    UIM_MSG_ERR_0("Conversion from RRC PLMN format to MM PLMN format is unsuccessful");
    return;
  }

  if ((gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
         slot) == TRUE)
      &&
      gstk_instances_ptr[slot - 1]->is_first_loc_info_envelope_sent != TRUE)
  {
    if(memcmp(gstk_instances_ptr[slot - 1]->gstk_last_loc_info_sent,
              nas_plmn_id,
              GSTK_MAX_PLMN_LEN) != 0)
    {
      UIM_MSG_HIGH_1("New PLMN from RRC. Send update loc info if TP DL RSP is received; is_tp_rsp_received=0x%x",
                     gstk_instances_ptr[slot - 1]->is_tp_rsp_received);
      if(gstk_instances_ptr[slot - 1]->is_tp_rsp_received == TRUE)
      {
        gstk_status = gstk_send_envelope_loc_info_command(
                        gstk_instances_ptr[0]->client_id,
                        nas_plmn_id,
                        0);
      }
      else
      {
        (void)gstk_memcpy(gstk_instances_ptr[slot - 1]->gstk_last_loc_info_sent,
                          nas_plmn_id,
                          GSTK_MAX_PLMN_LEN,
                          GSTK_MAX_PLMN_LEN,
                          GSTK_MAX_PLMN_LEN);
      }
    }
  }

  /* Update RRC loci table everytime RRC resports a cell change */
  gstk_status = gstkutil_update_plmn_lac_cellId_table(RRC_LOCI_UPDATE,
                                                      nas_plmn_id,
                                                      (uint8*)(cell_info.lac),
                                                      temp_cell_info,
                                                      GSTK_NO_SERVICE,
                                                      rat,
                                                      slot);
  /* Update cell information in the MM-GSTK LOCI table only when RRC reports a
     cell change within the same PLMN that MM_IDLE event reported to GSTK earlier */
  if((memcmp(gstk_instances_ptr[slot - 1]->gstk_loci_table.plmn_id,
             nas_plmn_id,
             GSTK_MAX_PLMN_LEN) == 0)
     &&
     (memcmp(gstk_instances_ptr[slot - 1]->gstk_loci_table.lac,
             (uint8*)(cell_info.lac),
             GSTK_MAX_LAC_LEN) == 0)
    )
  {
    UIM_MSG_HIGH_0("Match between RRC reported PLMN/LAC and MM plmn/LAC info");
    gstk_status = gstkutil_update_plmn_lac_cellId_table(MM_GSTK_LOCI_UPDATE,
                                                        nas_plmn_id,
                                                        (uint8*)(cell_info.lac),
                                                        temp_cell_info,
                                                        gstk_instances_ptr[slot - 1]->gstk_loci_table.loc_status,
                                                        GSTK_ACCESS_NONE,
                                                        slot);
  }

  if(gstk_instances_ptr[slot - 1]->gstk_mm_state_is_idle == FALSE)
  {
    UIM_MSG_HIGH_0("Queue RRC msgs to GSTK only after the first MM Idle event is received");
    return;
  }

  /* Check if RRC PLMN ID and LAC Info matches the current PLMN and LAC Info that GSTK has.
     Do not queue RRC Cell Change Indication if PLMN infor from RRC does not match
     current PLMN Info that GSTK has
  */
  if((memcmp(nas_plmn_id,
             gstk_instances_ptr[slot - 1]->gstk_loci_table.plmn_id,
             GSTK_MAX_PLMN_LEN) != 0)
     ||
     (memcmp((uint8*)(cell_info.lac),
             gstk_instances_ptr[slot - 1]->gstk_loci_table.lac,
             GSTK_MAX_LAC_LEN) != 0))
  {
    UIM_MSG_HIGH_0("Seems to be a plmn/lac change...do not cache cmd to GSTK");
    return;
  }

  /* Check the CM Call Connected Flag
     If Flag is set to TRUE  - do not build cmd to put in GSTK queue.
     If Flag is set to FALSE - build cmd to put in GSTK queue.
  */
  if(gstk_instances_ptr[slot - 1]->gstk_cm_call_is_connected == TRUE)
  {
    UIM_MSG_HIGH_0("CS call in progress...Do not cache cmd to GSTK");
    return;
  }

  /* Queue the cmd to GSTK Only if the current service state is either normal
     service or limited service */
  if((gstk_instances_ptr[slot - 1]->gstk_next_curr_location_status != GSTK_NORMAL_SERVICE) &&
     (gstk_instances_ptr[slot - 1]->gstk_next_curr_location_status != GSTK_LIMITED_SERVICE))
  {
    UIM_MSG_HIGH_0("Current Service state is neither limited nor normal service");
    return;
  }

  /* Build a cmd and put it in the gstk queue . This cmd will be processed
     by gstk to decide if any envelope needs to be sent down to the card
  */

  /* get the command from buffer */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    return;
  }

  /* Build the gstk_cmd */
  task_cmd->cmd.rrc_cell_change_ind.message_header.command_group = GSTK_RRC_CELL_CHANGE_IND;
  task_cmd->cmd.rrc_cell_change_ind.message_header.command_id = 0;
  task_cmd->cmd.rrc_cell_change_ind.message_header.user_data = i;
  task_cmd->cmd.rrc_cell_change_ind.message_header.sim_slot_id = slot;
  task_cmd->cmd.rrc_cell_change_ind.rrc_rat = rat;
  (void)memscpy(task_cmd->cmd.rrc_cell_change_ind.cell_info.cell_id,
                sizeof(task_cmd->cmd.rrc_cell_change_ind.cell_info.cell_id),
                temp_cell_info.cell_id,
                (uint32)temp_cell_info.cell_len);
  task_cmd->cmd.rrc_cell_change_ind.cell_info.cell_len = temp_cell_info.cell_len;
  (void)memscpy(task_cmd->cmd.rrc_cell_change_ind.mcc_mnc,
  	        sizeof(task_cmd->cmd.rrc_cell_change_ind.mcc_mnc),
                gstk_instances_ptr[slot - 1]->gstk_loci_table.plmn_id,
                GSTK_MAX_PLMN_LEN);
  (void)memscpy(task_cmd->cmd.rrc_cell_change_ind.lac,
  	        sizeof(task_cmd->cmd.rrc_cell_change_ind.lac),
                gstk_instances_ptr[slot - 1]->gstk_loci_table.lac,
                GSTK_MAX_LAC_LEN);
  task_cmd->cmd.rrc_cell_change_ind.location_state =
     (uint8)gstk_instances_ptr[slot - 1]->gstk_loci_table.loc_status;

  /* put command in to command queue */
  gstk_task_put_cmd_buf(task_cmd);
} /* gstk_send_cell_change_ind */
#endif /* (FEATURE_WCDMA) || (FEATURE_LTE) */

/*lint -e715 "cell_id_info_p not referenced when FEATURE_GSM is
  not defined" */
/*===========================================================================
FUNCTION gstk_get_rr_cell_id_info

DESCRIPTION
  Function used to query RR for the Cell Id.

PARAMETER
  Input:   const uint8       * mcc_mnc_p      : Current PLMN Information
  Input:   const uint8       *lac_p           : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  RR has to be able to provide a valid Cell ID:

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_rr_cell_id_info(
  const uint8             * mcc_mnc_p,
  const uint8             * lac_p,
  gstk_cell_id_type       * cell_id_info_p,
  gstk_slot_id_enum_type    slot
)
{
#ifdef FEATURE_GSM
  sys_modem_as_id_e_type as_id       = SYS_MODEM_AS_ID_1;

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return FALSE;
  }

  if ( gstk_io_ctrl(GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
                   &slot,
                   &as_id) != GSTK_SUCCESS)
  {
     return FALSE;
  }

  /* ---------------------------------------------------------------------------
     Validate that Pointer
     -------------------------------------------------------------------------*/
  if ( cell_id_info_p == NULL )
  {
    return FALSE;
  }

  /* ---------------------------------------------------------------------------
     Initailize the cell id information
     -------------------------------------------------------------------------*/
  memset(cell_id_info_p->cell_id,0xFF,GSTK_MAX_CELL_ID_LEN);
  cell_id_info_p->cell_len = GSTK_MAX_CELL_ID_LEN;

  /* ---------------------------------------------------------------------------
     Retrieve the Cell ID Information from RR
     -------------------------------------------------------------------------*/
  if ( rr_ds_get_cell_id(cell_id_info_p->cell_id, as_id) == TRUE )
  {
    cell_id_info_p->cell_len = 2;
    UIM_MSG_HIGH_2("VALID RR CELL ID:  0x%x 0x%x",
                   cell_id_info_p->cell_id[0], cell_id_info_p->cell_id[1]);
  }
  else
  {
    /* Update the cell id information from gstk_instances_ptr[slot - 1]->gstk_loci_table only if the current
      cell information(mcc,mnc,lac) is same as in gstk_instances_ptr[slot - 1]->gstk_loci_table */
    if((memcmp(gstk_instances_ptr[slot - 1]->gstk_loci_table.plmn_id,
               mcc_mnc_p,
               GSTK_MAX_PLMN_LEN) == 0) &&
       (memcmp(gstk_instances_ptr[slot - 1]->gstk_loci_table.lac,
               lac_p,
               GSTK_MAX_LAC_LEN) == 0))
    {
      if(memcmp(cell_id_info_p->cell_id,
                gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_id,
                int32touint32(gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_len)) == 0)
      {
        UIM_MSG_HIGH_0(" All entries in gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_id are 0xFF ");
        return FALSE;
      }

      (void)memscpy(cell_id_info_p->cell_id,
             int32touint32(gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_len),
             gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_id,
             int32touint32(gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_len));
      cell_id_info_p->cell_len = gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_len;
      return TRUE;
    }
    UIM_MSG_ERR_0(" Mismatch of PLMN and LAC information ");
    return FALSE;
  }

  return TRUE;
#else
  UIM_MSG_ERR_0(" FEATURE_GSM not defined ");
  return FALSE;
#endif /* FEATURE_GSM */

}/*lint !e818 "cell_id_info_p not const . This lint error is suppressed
  when FEATURE_GSM is not defined." */
/*lint +e715 "cell_id_info_p not referenced when FEATURE_GSM is not defined */
/* gstk_get_rr_cell_id_info */

/*lint -e715 "cell_id_info_p mcc_mnc_p lac_p not referenced when
  FEATURE_GSM is not defined" */
/*===========================================================================
FUNCTION gstk_get_rrc_cell_id_info

DESCRIPTION
  Function used to query RRC Table information populated by RRC.

PARAMETER
  Input:  mcc_mnc_p      : Current PLMN Information
  Input:  lac_p          : Current LAC  Information
  Input:  cell_id_info_p : Structure to populated
  Input:  slot           : Slot id
  Output: cell_id_info_p : Cell id information

DEPENDENCIES
  RRC would have populated the RRC Loci Table

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_rrc_cell_id_info (
  const uint8*                mcc_mnc_p,
  const uint8*                lac_p,
  gstk_cell_id_type*          cell_id_info_p,
  gstk_slot_id_enum_type      slot,
  gstk_access_technology_type access_tech
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  uint8 i = 0;

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return FALSE;
  }

  /* ---------------------------------------------------------------------------
     Validate that Pointer
     -------------------------------------------------------------------------*/
  if (cell_id_info_p == NULL ||
      mcc_mnc_p      == NULL ||
      lac_p          == NULL )
  {
    return FALSE;
  }
  /* ---------------------------------------------------------------------------
     Validate the access tech
     -------------------------------------------------------------------------*/
  switch(access_tech)
  {
    case GSTK_ACCESS_TECH_UTRAN:
      i = GSTK_RAT_UMTS_IDX;
      break;
    case GSTK_ACCESS_TECH_LTE:
      i = GSTK_RAT_LTE_IDX;
      break;
    default:
      UIM_MSG_HIGH_1("Incorrect input for access tech 0x%x", access_tech);
      return FALSE;
  }
  /* ---------------------------------------------------------------------------
     Initailize the cell id information
     -------------------------------------------------------------------------*/
  memset(cell_id_info_p->cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
  cell_id_info_p->cell_len = GSTK_MAX_CELL_ID_LEN;

  /* --------------------------------------------------------------------------
     Now check the RRC Table PLMN information against the mcc_mnc provided. If
     they do not match, the Cell ID Information is not valid .
     Return the default.
     ------------------------------------------------------------------------*/
  if (memcmp(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].plmn_id,
             mcc_mnc_p,
             GSTK_MAX_PLMN_LEN) != 0)
  {
    UIM_MSG_HIGH_0("PLMNs Don't Match");
    return FALSE;
  }

  /* --------------------------------------------------------------------------
     Now check the RRC Table LAC  information against the mcc_mnc provided. If
     they do not match, the Cell ID Information is not valid .
     Return the default.
     ------------------------------------------------------------------------*/
  if (memcmp(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].lac,
             lac_p,
             GSTK_MAX_LAC_LEN) != 0)
  {
    UIM_MSG_HIGH_0("LACs Don't Match");
    return FALSE;
  }

  /* --------------------------------------------------------------------------
     At this point, the Cell ID for the current PLMN matches the PLMN info
     for the last RRC Indication.  It is ok to copy this information.
     ------------------------------------------------------------------------*/
  (void)memscpy(cell_id_info_p->cell_id,
                sizeof(cell_id_info_p->cell_id),
                gstk_instances_ptr[slot - 1]->rrc_loci_table[i].cell_info.cell_id,
                int32touint32(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].cell_info.cell_len));
  cell_id_info_p->cell_len = gstk_instances_ptr[slot - 1]->rrc_loci_table[i].cell_info.cell_len;
  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_WCDMA || FEATURE_LTE*/

}/*lint !e818 "cell_id_info_p mcc_mnc_p lac_p not const . This lint error
  is suppressed when FEATURE_GSM is not defined." */
/*lint +e715 supress "cell_id_info_p mcc_mnc_p lac_p not referenced when
  FEATURE_GSM not defined" */
/* gstk_get_rrc_cell_id_info */


/*===========================================================================
FUNCTION gstk_get_curr_cell_id_info_int

DESCRIPTION
  Function called to retrieve the Cell ID information based on the RAT
  and current PLMN Information. This is internal funciton.

PARAMETER
  Input:   radio_access_tech : Current RAT
  Input:   mcc_mnc_p         : Current PLMN Information
  Input:   lac_p             : Current LAC  Information
  Input:   cell_id_info_p    : Structure to populated
  Input:   slot              : Slot id
  Output:  cell_id_info_p    : Cell id information

DEPENDENCIES
  TODO

RETURN VALUE
  TRUE  - When cell is found
  FALSE - Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_curr_cell_id_info_int(
  gstk_sys_radio_access_tech_e_type  radio_access_tech,
  const uint8*                       mcc_mnc_p,
  const uint8*                       lac_p,
  gstk_cell_id_type*                 cell_id_info_p,
  gstk_slot_id_enum_type             slot
)
{
  gstk_access_technology_type  gstk_access_tech = GSTK_ACCESS_NONE;
  boolean                      status           = FALSE;

  if ((status = gstk_util_is_valid_rat((sys_radio_access_tech_e_type)radio_access_tech,
                                       &gstk_access_tech)) == FALSE)
  {
    return status;
  }

  /* Populate the cell information */
  if (gstk_access_tech == GSTK_ACCESS_TECH_GSM)
  {
    status = gstk_get_rr_cell_id_info(mcc_mnc_p, lac_p, cell_id_info_p, slot);
  }
  else
  {
    status = gstk_get_rrc_cell_id_info(mcc_mnc_p,
                                       lac_p,
                                       cell_id_info_p,
                                       slot,
                                       gstk_access_tech);
  }
  return status;
} /*gstk_get_curr_cell_id_info_int*/


/*===========================================================================
FUNCTION gstk_get_curr_cell_id_info

DESCRIPTION
  Function called to retrieve the Cell ID information based on the RAT
  and current PLMN Information. This function is external (e.g. to NAS)

PARAMETER
  Input:   radio_access_tech : Current RAT
  Input:   mcc_mnc_p      : Current PLMN Information
  Input:   lac_p          : Current LAC  Information
  Input:   cell_id_info_p : Structure to be populated
  Output:  cell_id_info_p : Cell id information

DEPENDENCIES
  TODO

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_curr_cell_id_info(
  gstk_sys_radio_access_tech_e_type radio_access_tech,
  const uint8*                      mcc_mnc_p,
  const uint8*                      lac_p,
  gstk_cell_id_type*                cell_id_info_p,
  mmgsdi_session_type_enum_type     sess_type
)
{
  boolean                status = FALSE;
  gstk_slot_id_enum_type slot   = GSTK_SLOT_1;
  uint32  i                        = 0;

  /* ---------------------------------------------------------------------------
     Validate that Pointer
     -------------------------------------------------------------------------*/
  if (cell_id_info_p  == NULL ||
      mcc_mnc_p       == NULL ||
      lac_p           == NULL )
  {
    UIM_MSG_ERR_3("NULL POINTER %p %p %p", cell_id_info_p, mcc_mnc_p, lac_p);
    return FALSE;
  }

  if(gstk_shared_data.gstk_num_of_instances >1)
  {
    (void)gstk_util_validate_slot_sessions();
    for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
    {
      if (gstk_shared_data.mmgsdi_sess_info[i].sess_type == sess_type)
      {
        slot = gstk_shared_data.mmgsdi_sess_info[i].slot;
        UIM_MSG_HIGH_2("slot 0x%x found for sess type 0x%x", slot, sess_type);
        break;
      }
    }
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      return FALSE;
    }
  }

  /* ---------------------------------------------------------------------------
     Initailize the cell id information
     -------------------------------------------------------------------------*/
  memset(cell_id_info_p->cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
  cell_id_info_p->cell_len = GSTK_MAX_CELL_ID_LEN;

  status = gstk_get_curr_cell_id_info_int(
             radio_access_tech,
             mcc_mnc_p,
             lac_p,
             cell_id_info_p,
             slot);

  return status;
} /* gstk_get_curr_cell_id_info */

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_cache_1x_location_info

DESCRIPTION
  Caches 1x location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_1x_location_info(gstk_cdma_loci_ext_cache_type  *cache,
                                 gstk_cdma_location_info_type   loc_info)
{
  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);
  GSTK_RETURN_IF_NULL_PTR(cache);

  if (gstk_util_check_bytes(loc_info.mcc, GSTK_MCC_LEN, 0x00))
  {
    UIM_MSG_ERR_0("Invalid MCC");
    return;
  }

  if (gstk_util_check_bytes(&loc_info.imsi_11_12, 1, 0x00))
  {
    UIM_MSG_ERR_0("Invalid imsi_11_12");
    return;
  }

  if (gstk_util_check_bytes(loc_info.sid_info.sid,
                            loc_info.sid_info.id_len, 0x00))
  {
    UIM_MSG_ERR_0("Invalid SID");
    return;
  }

  /* Memset the cache and copy individual elements to
     avoid issues due to structure padding
  */
  memset(&cache->data.cdma_loc_info, 0xFF, sizeof(gstk_curr_inst_ptr->cdma_ext_loci_cache.data.cdma_loc_info));

  cache->is_cdma_info_valid = TRUE;

  /* SID info */
  if(gstk_memcpy(cache->data.cdma_loc_info.sid_info.sid,
                 loc_info.sid_info.sid,
                 (size_t)loc_info.sid_info.id_len,
                 GSTK_MAX_SID_LEN,
                 GSTK_MAX_SID_LEN) <
     (size_t)loc_info.sid_info.id_len)
  {
    return;
  }
  cache->data.cdma_loc_info.sid_info.id_len
    = loc_info.sid_info.id_len;

  /* NID info */
  if(gstk_memcpy(cache->data.cdma_loc_info.nid_info.nid,
                 loc_info.nid_info.nid,
                 (size_t)loc_info.nid_info.id_len,
                 GSTK_MAX_NID_LEN,
                 GSTK_MAX_NID_LEN) <
     (size_t)loc_info.nid_info.id_len)
  {
    return;
  }
  cache->data.cdma_loc_info.nid_info.id_len
    = loc_info.nid_info.id_len;

  /* MCC info */
  (void)gstk_memcpy(cache->data.cdma_loc_info.mcc,
                    loc_info.mcc,
                    GSTK_MCC_LEN,
                    GSTK_MCC_LEN,
                    GSTK_MCC_LEN);

  /* IMSI_11_12 */
  cache->data.cdma_loc_info.imsi_11_12
    = loc_info.imsi_11_12;

  /* BASE_ID info */
  if(gstk_memcpy(cache->data.cdma_loc_info.base_id_info.base_id,
                 loc_info.base_id_info.base_id,
                 (size_t)loc_info.base_id_info.id_len,
                 GSTK_MAX_BASE_ID_LEN,
                 GSTK_MAX_BASE_ID_LEN) <
     (size_t)loc_info.base_id_info.id_len)
  {
    return;
  }
  cache->data.cdma_loc_info.base_id_info.id_len
    = loc_info.base_id_info.id_len;

  /* BASE Latitude */
  (void)gstk_memcpy(cache->data.cdma_loc_info.base_lat,
                    loc_info.base_lat,
                    GSTK_BASE_LAT_LEN,
                    GSTK_BASE_LAT_LEN,
                    GSTK_BASE_LAT_LEN);

  /* BASE Longitude */
  (void)gstk_memcpy(cache->data.cdma_loc_info.base_long,
                    loc_info.base_long,
                    GSTK_BASE_LONG_LEN,
                    GSTK_BASE_LONG_LEN,
                    GSTK_BASE_LONG_LEN);
} /* gstk_cache_1x_location_info */

/*===========================================================================
FUNCTION gstk_copy_1x_info_from_cm

DESCRIPTION
  Copies 1x related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_1x_info_from_cm(
  gstk_cdma_loci_ext_type               *cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type *cm_stack_info_ptr
)
{
  /* check for Null ptr */
  if(cdma_ext_loc_info == NULL || cm_stack_info_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* SID info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.sid_info.sid,
         sizeof(cdma_ext_loc_info->cdma_loc_info.sid_info.sid),
         &(cm_stack_info_ptr->sys_id.id.is95.sid),
         GSTK_MAX_SID_LEN);
  cdma_ext_loc_info->cdma_loc_info.sid_info.id_len = GSTK_MAX_SID_LEN;

  /* NID info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.nid_info.nid,
         sizeof(cdma_ext_loc_info->cdma_loc_info.nid_info.nid),
         &(cm_stack_info_ptr->sys_id.id.is95.nid),
         GSTK_MAX_NID_LEN);
  cdma_ext_loc_info->cdma_loc_info.nid_info.id_len = GSTK_MAX_NID_LEN;

  /* MCC info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.mcc,
         sizeof(cdma_ext_loc_info->cdma_loc_info.mcc),
         &(cm_stack_info_ptr->sys_id.id.is95.mcc),
         GSTK_MCC_LEN);

  /* IMSI_11_12 */
  cdma_ext_loc_info->cdma_loc_info.imsi_11_12  = cm_stack_info_ptr->sys_id.id.is95.imsi_11_12;

  /* BASE_ID info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.base_id_info.base_id,
         sizeof(cdma_ext_loc_info->cdma_loc_info.base_id_info.base_id),
         &(cm_stack_info_ptr->mode_info.cdma_info.base_id),
         GSTK_MAX_BASE_ID_LEN);
  cdma_ext_loc_info->cdma_loc_info.base_id_info.id_len = GSTK_MAX_BASE_ID_LEN;

  /* BASE Latitude */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.base_lat,
         sizeof(cdma_ext_loc_info->cdma_loc_info.base_lat),
         &(cm_stack_info_ptr->mode_info.cdma_info.base_lat),
         GSTK_BASE_LAT_LEN);

  /* BASE Longitude */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.base_long,
         sizeof(cdma_ext_loc_info->cdma_loc_info.base_long),
         &(cm_stack_info_ptr->mode_info.cdma_info.base_long),
         GSTK_BASE_LONG_LEN);

  return gstk_copy_advance_info_from_cm(cdma_ext_loc_info, cm_stack_info_ptr);
} /* gstk_copy_1x_info_from_cm */

/*===========================================================================
FUNCTION gstk_cache_advance_location_info

DESCRIPTION
  Caches 1x advance location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_advance_location_info(gstk_cdma_loci_ext_cache_type *cache,
                                      gstk_cdma_loci_ext_type loc_info)
{
  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);
  GSTK_RETURN_IF_NULL_PTR(cache);

  if (gstk_util_check_bytes(loc_info.height, GSTK_HEIGHT_LEN, 0xFF) &&
      loc_info.horizontal_uncertainty == 0xFF)
  {
    UIM_MSG_ERR_0("Invalid Advance loci");
    cache->is_advance_info_valid = FALSE;
    return;
  }

  /* Memset the cache and copy individual elements to
     avoid issues due to structure padding
  */
  memset(gstk_curr_inst_ptr->cdma_ext_loci_cache.data.height,
         0xFF,
         sizeof(gstk_curr_inst_ptr->cdma_ext_loci_cache.data.height));
  cache->data.horizontal_uncertainty = 0xFF;

  cache->is_advance_info_valid = TRUE;

  /* Height */
  (void)gstk_memcpy(cache->data.height,
                    loc_info.height,
                    GSTK_HEIGHT_LEN,
                    sizeof(cache->data.height),
                    sizeof(loc_info.height));

  /* Horizontal Uncertainty */
  cache->data.horizontal_uncertainty = loc_info.horizontal_uncertainty;
}

/*===========================================================================
FUNCTION gstk_cache_hdr_location_info

DESCRIPTION
  Caches HDR location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_hdr_location_info(gstk_cdma_loci_ext_cache_type *cache, gstk_cdma_loci_ext_type loc_info)
{
  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);
  GSTK_RETURN_IF_NULL_PTR(cache);

  /* Memset the cache and copy individual elements to
     avoid issues due to structure padding
  */
  memset(cache->data.country_code,
         0xFF,
         sizeof(cache->data.country_code));
  memset(cache->data.sector_id,
         0xFF,
         sizeof(cache->data.sector_id));
  memset(cache->data.cdma_channel,
         0xFF,
         sizeof(cache->data.cdma_channel));
  memset(cache->data.base_lat,
         0xFF,
         sizeof(cache->data.base_lat));
  memset(cache->data.base_long,
         0xFF,
         sizeof(cache->data.base_long));

  cache->is_hdr_info_valid = TRUE;

  /* Country code */
  (void)gstk_memcpy(cache->data.country_code,
                    loc_info.country_code,
                    GSTK_MCC_LEN,
                    sizeof(cache->data.country_code),
                    sizeof(loc_info.country_code));

  /* Sector ID */
  (void)gstk_memcpy(cache->data.sector_id,
                    loc_info.sector_id,
                    GSTK_HDR_SECTOR_ID_LEN,
                    sizeof(cache->data.sector_id),
                    sizeof(loc_info.sector_id));

  /* CDMA Channel */
  (void)gstk_memcpy(cache->data.cdma_channel,
                    loc_info.cdma_channel,
                    GSTK_CDMA_CHANNEL_LEN,
                    sizeof(cache->data.cdma_channel),
                    sizeof(loc_info.cdma_channel));

  /* Latitude */
  (void)gstk_memcpy(cache->data.base_lat,
                    loc_info.base_lat,
                    GSTK_BASE_LAT_LEN,
                    sizeof(cache->data.base_lat),
                    sizeof(loc_info.base_lat));

  /* Longitude */
  (void)gstk_memcpy(cache->data.base_long,
                    loc_info.base_long,
                    GSTK_BASE_LONG_LEN,
                    sizeof(cache->data.base_long),
                    sizeof(loc_info.base_long));
}

/*===========================================================================
FUNCTION gstk_copy_advance_info_from_cm

DESCRIPTION
  Copies 1X Advance related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_advance_info_from_cm(
  gstk_cdma_loci_ext_type              * cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr
)
{
  /* check for Null ptr */
  if(cdma_ext_loc_info == NULL || cm_stack_info_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* Height = Height (2 bytes) + Vertical Uncertainty (1 byte) */
  cdma_ext_loc_info->height[0] = cm_stack_info_ptr->rat_mode_info.cdma_mode_info.vert_uncertainty;

  (void)gstk_memcpy((cdma_ext_loc_info->height + 1),
         &(cm_stack_info_ptr->rat_mode_info.cdma_mode_info.height),
         sizeof(uint16),
         sizeof(uint16),
         sizeof(uint16));

  /* Horizontal Uncertainty (1 byte) */
  cdma_ext_loc_info->horizontal_uncertainty = cm_stack_info_ptr->rat_mode_info.cdma_mode_info.horiz_uncertainty;

  return GSTK_SUCCESS;
} /* gstk_copy_advance_info_from_cm */

/*===========================================================================
FUNCTION gstk_copy_hdr_info_from_cm

DESCRIPTION
  Copies HDR related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_hdr_info_from_cm(
  gstk_cdma_loci_ext_type              * cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr
)
{

  /* check for Null ptr */
  if(cdma_ext_loc_info == NULL || cm_stack_info_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* MCC info */
  (void)gstk_memcpy(cdma_ext_loc_info->country_code,
         &(cm_stack_info_ptr->rat_mode_info.hdr_mode_info.mcc),
         GSTK_MCC_LEN,
         GSTK_MCC_LEN,
         GSTK_MCC_LEN);

  /* Sector ID info */
  (void)gstk_memcpy(cdma_ext_loc_info->sector_id,
         cm_stack_info_ptr->sys_id.id.is856,
         GSTK_HDR_SECTOR_ID_LEN,
         GSTK_HDR_SECTOR_ID_LEN,
         GSTK_HDR_SECTOR_ID_LEN);

  /* CDMA Channel (3 bytes) */
  /* store system type in byte 1*/
  cdma_ext_loc_info->cdma_channel[0] = cm_stack_info_ptr->rat_mode_info.hdr_mode_info.system_type;
  /* store band class in 5 right most bits of byte 2 */
  cdma_ext_loc_info->cdma_channel[1] = (uint8) (cm_stack_info_ptr->active_band & 0x1F);
  /* store 3 least significant bits of channel in 3 left most bits of byte 2 */
  cdma_ext_loc_info->cdma_channel[1] |= (uint8) ((cm_stack_info_ptr->active_channel & 0x07) << 5);
  /* store remaining bits of channel in byte 3 */
  cdma_ext_loc_info->cdma_channel[2] = (uint8) (cm_stack_info_ptr->active_channel >> 3);

  /* BASE Latitude */
  (void)gstk_memcpy(cdma_ext_loc_info->base_lat,
         &(cm_stack_info_ptr->rat_mode_info.hdr_mode_info.latitude),
         GSTK_BASE_LAT_LEN,
         GSTK_BASE_LAT_LEN,
         GSTK_BASE_LAT_LEN);

  /* BASE Longitude */
  (void)gstk_memcpy(cdma_ext_loc_info->base_long,
         &(cm_stack_info_ptr->rat_mode_info.hdr_mode_info.longitude),
         GSTK_BASE_LONG_LEN,
         GSTK_BASE_LONG_LEN,
         GSTK_BASE_LONG_LEN);

  return GSTK_SUCCESS;
} /* gstk_copy_hdr_info_from_cm */
#endif /* #ifdef FEATURE_CDMA */

#ifndef FEATURE_ESTK
/*===========================================================================
FUNCTION: gstk_wms_event_cb

DESCRIPTION:
  This function is the event callback function for WMS.

PARAMETERS:
  wms_event:  [Input] Command whose status is being reported
  info_ptr:   [Input] WMS info buffer
  shared_ptr: [Input] WMS indication of whether the info can be passed to
                      other clients.

DEPENDENCIES:
  This is only called if FEATURE_ESTK is off. If feature is turned on
  then ESTK will register to WMS instead.

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void gstk_wms_event_cb(
  wms_msg_event_e_type              wms_event,
  const wms_msg_event_info_s_type  *info_ptr,
  const boolean                    *shared_ptr)
{
  gstk_task_cmd_type          *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type slot               = GSTK_SLOT_1;
  boolean                queue_cmd          = TRUE;

  GSTK_RETURN_IF_NULL_PTR(info_ptr);

  if (gstk_io_ctrl(
        GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
        &info_ptr->status_info.as_id,
        &slot) != GSTK_SUCCESS)
  {
    return;
  }

  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if (task_cmd_ptr == NULL)
  {
    return;
  }

  /* Build GSTK Command */
  task_cmd_ptr->cmd.wms_evt_ind.message_header.command_group = GSTK_WMS_EVT_IND;
  task_cmd_ptr->cmd.wms_evt_ind.message_header.sim_slot_id = slot;

  switch (wms_event)
  {
    case WMS_MSG_EVENT_MT_MESSAGE_ERROR:
      task_cmd_ptr->cmd.wms_evt_ind.wms_event = GSTK_WMS_MSG_EVENT_MT_MESSAGE_ERROR;
      break;
    default:
      queue_cmd = FALSE;
      break;
  }

  if(queue_cmd)
  {
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    gstk_free(task_cmd_ptr);
  }

  return;
} /* gstk_wms_event_cb */
#endif /* FEATURE_ESTK */


/*===========================================================================
FUNCTION   gstk_cm_ph_error_event_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Network Search Mode to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_ph_error_event_cb (
  void*                 data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
)
{
  gstk_task_cmd_type * task_cmd = NULL;

  /* Data block pointer should contain the Provide 
     Local Info information */
  if (data_block_ptr == NULL || cmd != CM_PH_CMD_INFO_GET ||
  	   cmd_err == CM_PH_CMD_ERR_NOERR || 
  	  (gstk_util_compare_ref_id(*((uint32*)data_block_ptr)) != GSTK_SUCCESS))
  {
    UIM_MSG_ERR_0("Invalid data from CM");
    return;
  }

  /* get the command from buffer */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    return;
  }

  /* Build the gstk_cmd */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    =
    int32touint32((int32)GSTK_PROVIDE_LOCAL_INFO_CNF);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.sim_slot_id   =
    gstk_curr_inst_ptr->slot_id;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id           =
    *((uint32*)data_block_ptr);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_number               =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*((uint32*)data_block_ptr))].command_details.command_number;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_result               =
    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  /* initialize additional info length to zero */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;

  /* put command back to command queue */
  gstk_task_put_cmd_buf(task_cmd);

}/*lint !e818 "data_block_ptr not const . This lint error is suppressed as
cm_ph_cmd_get_ph_info which gstk calls with this callback does not support a const cb type ." */
/* gstk_cm_ph_error_event_cb */

/*===========================================================================
FUNCTION   gstk_cm_ph_event_cb

DESCRIPTION
  Phone (ph) event callback function, called when an ph event is happening.
  This function populates the preferred selection types and
  puts the command in the GSTK command queue.

PARAMETERS
  ph_event: [Input] CM phone event
  ph_info_ptr: [Input] Pointer to the ph info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_ph_event_cb (
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type    *ph_info_ptr
)
{
  gstk_task_cmd_type*    task_cmd  = NULL;
  gstk_slot_id_enum_type slot      = GSTK_SLOT_1;
  boolean                queue_cmd = TRUE;
  uint8                  i         = 0;

  if (ph_info_ptr == NULL)
  {
    return;
  }

  if(CM_PH_EVENT_DUAL_STANDBY_PREF != ph_event)
  {
    if (gstk_io_ctrl(
          GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
          &ph_info_ptr->asubs_id,
          &slot) != GSTK_SUCCESS)
    {
      return;
    }
  }

  UIM_MSG_HIGH_2("gstk_cm_ph_event_cb: event 0x%x nw search mode 0x%x",
                   ph_event, ph_info_ptr->network_sel_mode_pref);

  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    return;
  }
  task_cmd->cmd.cm_ph_ind.message_header.command_group =
    GSTK_CM_PH_IND;
  task_cmd->cmd.cm_ph_ind.message_header.command_id = 0; /* doesn't matter */
  task_cmd->cmd.cm_ph_ind.message_header.user_data = 0;  /* doesn't matter */
  task_cmd->cmd.cm_ph_ind.message_header.sim_slot_id = slot;

  switch (ph_event)
  {
    case CM_PH_EVENT_SYS_SEL_PREF:
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_SYS_SEL_PREF;
      switch(ph_info_ptr->network_sel_mode_pref)
      {
        case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_AUTOMATIC;
          break;

        case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_MANUAL;
          break;

        default:
          queue_cmd = FALSE;
          break;
      }
      break;

    case CM_PH_EVENT_INFO:
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_INFO;
      switch(ph_info_ptr->network_sel_mode_pref)
      {
        case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_AUTOMATIC;
          break;

        case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_MANUAL;
          break;

        default:
          queue_cmd = FALSE;
          break;
      }
      (void)gstk_memcpy(&task_cmd->cmd.cm_ph_ind.available_networks,
                        &ph_info_ptr->available_networks,
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type));
      break;

    case CM_PH_EVENT_DUAL_STANDBY_PREF:
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF;
      switch(ph_info_ptr->standby_pref)
      {
        case SYS_MODEM_DS_PREF_SINGLE_STANDBY:
          task_cmd->cmd.cm_ph_ind.active_data_subs = ph_info_ptr->active_subs;
          break;

        case SYS_MODEM_DS_PREF_DUAL_STANDBY:
        case SYS_MODEM_DS_PREF_DUAL_STANDBY_NO_TUNEAWAY:
          task_cmd->cmd.cm_ph_ind.active_data_subs = ph_info_ptr->default_data_subs;
          break;

        case SYS_MODEM_DS_PREF_TRIPLE_STANDBY:
        case SYS_MODEM_DS_PREF_TRIPLE_STANDBY_NO_TUNEAWAY:
          task_cmd->cmd.cm_ph_ind.active_data_subs = ph_info_ptr->default_data_subs;
          break;

        default:
           UIM_MSG_LOW_0("No relevant standby pref");
           queue_cmd = FALSE;
           break;
      }
      break;

    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_AVAILABLE_NETWORKS_CONF;
      (void)gstk_memcpy(&task_cmd->cmd.cm_ph_ind.available_networks,
                        &ph_info_ptr->available_networks,
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type));
      
      for(i = 0; i < ph_info_ptr->available_networks.length ; i++)
      {
        UIM_MSG_HIGH_2("CSG ID: 0x%x, HNB Name len: 0x%x",
                      ph_info_ptr->available_networks.info[i].csg_info.csg_id,
                      ph_info_ptr->available_networks.info[i].csg_info.hnb_name.length);
      }
      break;

    default:
      queue_cmd = FALSE;
  }
  if(queue_cmd)
  {
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    gstk_free(task_cmd);
  }
} /* gstk_cm_ph_event_cb */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* <EJECT> */
/*===========================================================================

FUNCTION GSTK_FREE

DESCRIPTION
  The gstk_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address, but only if FEATURE_GSTK_USE_SMALL_HEAP is defined
  and FEATURE_LOW_MEMORY_USAGE IS NOT

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void gstk_free(void * ptr)
{
  if (ptr == NULL)
  {
    return;
  }
  modem_mem_free((void*)(ptr),MODEM_MEM_CLIENT_UIM);
  ptr = NULL;
}/* gstk_free */

/*===========================================================================

FUNCTION gstk_offset_memcpy

DESCRIPTION
  gstk_offset_memcpy first checks that in an invalid index is not be accessed
  in the destination buffer then calls gstk_offset

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  dest_offset:        offset from where to start copying data to
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  the number of bytes copied

SIDE EFFECTS
  copy_size may be truncated if the offset plus the size to be copied exceeds
  destination buffer maximum size, in which case the dest_max_size will also
  be updated before sending to gstk_memcpy
===========================================================================*/
size_t  gstk_byte_offset_memcpy(
  byte *dest_ptr,
  const void *src_ptr,
  uint32 dest_offset,
  size_t copy_size,
  size_t dest_max_size,
  size_t src_max_size)
{
 if (dest_ptr == NULL || src_ptr == NULL) 
 {
    return 0;
 }

 if(dest_offset >= dest_max_size)
 {
    UIM_MSG_MED_2("dest_offset = %d is greater than dest_max_size = %d",
                   dest_offset,
                   dest_max_size);
    return 0;
 }

  /* Ensure copy_size does not overflow dest_ptr buffer */
  copy_size = (copy_size < (dest_max_size - (size_t)dest_offset)) ?
               copy_size :
              (dest_max_size - (size_t)dest_offset);

  return memscpy((void *)(dest_ptr + dest_offset),
                 copy_size,
                 src_ptr,
                 src_max_size);
} /* gstk_byte_offset_memcpy */

/*===========================================================================

FUNCTION gstk_memcpy

DESCRIPTION
  gstk_memcpy first checks if the size requested does not exceed the source
  size and that the size of the data to be copied does not exceed the
  max data size the destination buffer can accomodate before preceeding with
  the memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes copied

SIDE EFFECTS
  data is copied to the destination buffer.  the copy may be truncated if the
  size of data to be copied exceeds the size of the source buffer or if the
  size of data to be copied exceeds the size of the destination buffer.
===========================================================================*/
size_t  gstk_memcpy(
  void *dest_ptr,
  const void *src_ptr,
  size_t copy_size,
  size_t dest_max_size,
  size_t src_max_size)
{
  if ((dest_ptr == NULL) || (src_ptr == NULL))
  {
    UIM_MSG_HIGH_0("gstk_memcpy: NULL pointers passed in, cannot memcpy");
    return 0;
  }

  if(copy_size > src_max_size)
  {
    UIM_MSG_ERR_0("gstk_memcpy: invalid source read");
    copy_size = src_max_size;
  }

  return memscpy(dest_ptr, dest_max_size, src_ptr, copy_size);
} /* gstk_memcpy */


/*===========================================================================

FUNCTION gstk_get_and_store_curr_card_protocol

DESCRIPTION
  This function stores the current card protocol as obtained from MMGSDI

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gstk_get_and_store_curr_card_protocol(void)
{
  mmgsdi_protocol_enum_type   protocol_data = MMGSDI_MAX_PROTOCOL_ENUM;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  mmgsdi_status = mmgsdi_get_protocol(
                    gstk_shared_data.gstk_mmgsdi_client_id,
                    (mmgsdi_slot_id_enum_type)gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id),
                    &protocol_data);
  
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
     (protocol_data != MMGSDI_ICC && protocol_data != MMGSDI_UICC))
  {
    UIM_MSG_ERR_2("Could not get Card Protocol 0x%x, proto data 0x%x", 
		  mmgsdi_status, protocol_data);
    return;
  }

  gstk_curr_inst_ptr->card_protocol = protocol_data;
}/* gstk_get_and_store_curr_card_protocol */


/*===========================================================================

FUNCTION gstk_process_client_reg_req

DESCRIPTION
  This function processes the client registration request to GSTK

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if client registration was processed succesfully.
    Note: This only means that the client will receive a response from GSTK
    for the registration. It however does not mean that the registration
    was successful.
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter provided to this function was  NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_reg_req(const gstk_cmd_type *cmd_ptr)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  if(!cmd_ptr)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(cmd_ptr->general_cmd.message_header.command_id)
  {
  case GSTK_CLIENT_ID_REG_REQ:
    gstk_status = gstk_process_client_init_req(&(cmd_ptr->client_id_reg));
    break;
  case GSTK_CLIENT_PRO_CMD_REG_REQ:
    gstk_status = gstk_process_client_pro_cmd_reg_req(&(cmd_ptr->client_evt_reg));
    break;
  default:
    UIM_MSG_LOW_0("Unknown client registration");
    break;
  }/* End of switch */

  return gstk_status;

}/* gstk_process_client_reg_req */

/*===========================================================================
FUNCTION gstk_process_client_pro_cmd_reg_req

DESCRIPTION
  This function processes the client registration request for processing a
  practive command.

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if a response was sent to the client with a client id in case of SUCCESS
    OR
    a failure reason was returned to client
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter provided to this function was NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_pro_cmd_reg_req(
  const gstk_client_evt_reg_type *cmd_ptr
)
{
  gstk_status_enum_type gstk_status         = GSTK_SUCCESS;
  uint32                i                   = 0x00;
  boolean               need_classification = FALSE;

  /* check for Null ptr */
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* Validate input parameters */
  if(!cmd_ptr || !cmd_ptr->client_cmd_cb)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  /* Validate client id */
  if (!gstk_util_is_valid_client_id(cmd_ptr->client_id))
  {
    UIM_MSG_ERR_1("Client not registered 0x%x", cmd_ptr->client_id);
    /* No client id is returned to client for this case since the
       client is not known */
    (cmd_ptr->client_cmd_cb)(GSTK_CLIENT_NOT_REGISTERED,
                             cmd_ptr->message_header.user_data);
    return gstk_status;
  }

  /* check for number of registration */
  if(cmd_ptr->num_of_cmds > GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY)
  {
    UIM_MSG_ERR_1("Too many registrations 0x%x", cmd_ptr->num_of_cmds);
    (cmd_ptr->client_cmd_cb)(GSTK_DUPLICATE_REGISTRATION,
                             cmd_ptr->message_header.user_data);
    return gstk_status;
  }

  /* If callback is NULL, perform deregistration */
  if(cmd_ptr->client_evt_cb == NULL)
  {
    return(gstk_process_client_pro_cmd_dereg_req(cmd_ptr));
  }

  /* Check if client has registered earlier  and check if the callback
     registered earlier is the same as this one. Otheriwse reject the
     registration
  */
  if(gstk_shared_data.gstk_client_table[cmd_ptr->client_id-1].client_cb != NULL)
  {
    if(gstk_shared_data.gstk_client_table[cmd_ptr->client_id-1].client_cb !=
       cmd_ptr->client_evt_cb)
    {
      (cmd_ptr->client_cmd_cb)(GSTK_ERROR,
                               cmd_ptr->message_header.user_data);
      return GSTK_SUCCESS;
    }
  }

  for(i = 0; i < cmd_ptr->num_of_cmds; i++)
  {
    if((cmd_ptr->reg_info_ptr[i].reg_set.cmd_reg_bit_mask == GSTK_REG_PROACTIVE_CMD_EVT)
        ||
        (cmd_ptr->reg_info_ptr[i].reg_set.cmd_reg_bit_mask == GSTK_REG_ALL_EVT))
    {
      need_classification = TRUE;
    }
    else
    {
      need_classification = FALSE;
    }
    gstk_status = gstk_process_class_evt_registration(
                      cmd_ptr->client_id,
                      cmd_ptr->reg_info_ptr[i].reg_set.class_bit_mask,
                      cmd_ptr->reg_info_ptr[i].reg_set.cmd_reg_bit_mask,
                      cmd_ptr->data_format_type,
                      cmd_ptr->reg_info_ptr[i].client_func_type,
                      need_classification);
    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_2("gstk_process_class_evt_registration failed: 0x%x, 0x%x",
                    gstk_status, cmd_ptr->client_id);
      (cmd_ptr->client_cmd_cb)(gstk_status,
                               cmd_ptr->message_header.user_data);
       return GSTK_SUCCESS;
    }
  }
  gstk_shared_data.gstk_client_table[cmd_ptr->client_id-1].client_cb =
    cmd_ptr->client_evt_cb;
  (cmd_ptr->client_cmd_cb)(GSTK_SUCCESS,
                           cmd_ptr->message_header.user_data);

  return gstk_status;
}/* gstk_process_client_pro_cmd_reg_req */

/*===========================================================================
FUNCTION gstk_process_client_init_req

DESCRIPTION
  This function processes the client init request to GSTK. If the processing
  is successful, it provides the client with a client id.

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if a response was sent to the client with a client id in case of SUCCESS
    OR
    a failure reason was returned to client
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter privided to this function was  NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_init_req(
  const gstk_client_id_reg_type *cmd_ptr
)
{
  gstk_status_enum_type   client_ret_status = GSTK_SUCCESS;
  gstk_client_id_type     client_id         = GSTK_MAX_CLIENT + 1;
  int                     i                 = 0;

  /* check for Null ptr */
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(!cmd_ptr || !cmd_ptr->client_cb_ptr)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(gstk_curr_inst_ptr->gstk_state)
  {
  case GSTK_START_S:
  case GSTK_TERM_PROFILE_WAIT_S:
  case GSTK_MMGSDI_SIG_WAIT_S:
  case GSTK_TERM_PROFILE_RSP_WAIT_S:
  case GSTK_IDLE_S:
  case GSTK_NO_SIM_S:
    break;
  default:
    UIM_MSG_ERR_1("Client ID request not handled in gstk state 0x%x",
                  gstk_curr_inst_ptr->gstk_state);
    (cmd_ptr->client_cb_ptr)(GSTK_ERROR,
                             client_id,
                             cmd_ptr->message_header.user_data);
     return GSTK_SUCCESS;
  }

  switch(cmd_ptr->client_type)
  {
  case GSTK_DS_TYPE:
  case GSTK_UI_TYPE:
  case GSTK_TEST_TYPE:
  case GSTK_ESTK_TYPE:
  case GSTK_WAKEUP_TYPE:
  case GSTK_IMS_TYPE:
    break;
  default:
    UIM_MSG_ERR_1("GSTK_INVALID_CLIENT_TYPE 0x%x", cmd_ptr->client_type);
    (cmd_ptr->client_cb_ptr)(GSTK_INVALID_CLIENT_TYPE,
                             client_id,
                             cmd_ptr->message_header.user_data);
    return GSTK_SUCCESS;
  }

  /* Note : Client Id 0 is reserved for GSTK */
  for (i = 1; i < GSTK_MAX_CLIENT; i++)
  {
    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
    if (gstk_shared_data.gstk_client_table[i].is_free)
    {
      gstk_shared_data.gstk_client_table[i].is_free = FALSE;
      gstk_shared_data.gstk_client_table[i].client_type =
        cmd_ptr->client_type;
      gstk_shared_data.gstk_client_table[i].user_data =
        cmd_ptr->message_header.user_data;
      gstk_shared_data.gstk_client_table[i].sim_slot_id =
        cmd_ptr->message_header.sim_slot_id;
      client_id = (gstk_client_id_type)(i+1);
      rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
      break;
    }
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  }
  if (i == GSTK_MAX_CLIENT)
  { /* All client IDs are exhausted */
    client_ret_status = GSTK_CLIENT_SPACE_FULL;
  }
  /* Build response to client */
  (cmd_ptr->client_cb_ptr)(client_ret_status,
                           client_id,
                           cmd_ptr->message_header.user_data);
  return GSTK_SUCCESS;
}/* gstk_process_client_init_req */

/*===========================================================================
FUNCTION gstk_process_client_pro_cmd_dereg_req

DESCRIPTION
  This function processes the client deregistration request for a
  proactive command.

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if a response was sent to the client with a client id in case of SUCCESS
    OR
    a failure reason was returned to client
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter provided to this function was  NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_pro_cmd_dereg_req(
  const gstk_client_evt_reg_type *cmd_ptr
)
{
  gstk_status_enum_type             client_ret_status = GSTK_SUCCESS;
  gstk_status_enum_type             gstk_status       = GSTK_SUCCESS;
  gstk_client_id_type               client_id         = GSTK_MAX_CLIENT + 1;

  if(!cmd_ptr)
  {
    return GSTK_NULL_INPUT_PARAM;
  }
  if (!gstk_util_is_valid_client_id(cmd_ptr->client_id))
  {
    UIM_MSG_ERR_1("Client not registered 0x%x", cmd_ptr->client_id);
    client_ret_status = GSTK_CLIENT_NOT_REGISTERED;
  }
  else
  {
    client_id = cmd_ptr->client_id;
    if((cmd_ptr->num_of_cmds != 0) ||
       (cmd_ptr->reg_info_ptr))
    {
      UIM_MSG_ERR_2("Dereg unsuccessful client id- 0x%x num - 0x%x ",
                    cmd_ptr->client_id, cmd_ptr->num_of_cmds);
      (cmd_ptr->client_cmd_cb)(GSTK_ERROR,
                               gstk_shared_data.gstk_client_table[client_id-1].user_data);
      return GSTK_SUCCESS;
    }

    gstk_util_free_client_id(client_id);

  }/* if valid client id */

  (cmd_ptr->client_cmd_cb)(client_ret_status,
                           cmd_ptr->message_header.user_data);
  return gstk_status;
}/* gstk_process_client_pro_cmd_dereg_req */

/*===========================================================================
FUNCTION gstk_delete_client_from_class_evt_reg_table

DESCRIPTION
  This function deletes a client registration for all registered commands
  from the class event registration "gstk_curr_inst_ptr->gstk_class_evt_reg_table" table.

PARAMETERS
  client_id[Input] - client identity

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gstk_delete_client_from_class_evt_reg_table(
  gstk_client_id_type client_id)
{
  int i = 0;
  int j = 0;

  UIM_MSG_HIGH_0("In gstk_delete_client_from_class_evt_reg_table()");

  /* check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("Client not registered 0x%x", client_id);
    return;
  }

  for(j = 0; j < gstk_shared_data.gstk_num_of_instances; j++)
  {
    for(i =0 ;i < GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY; i++)
    {
      if(gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_modem_func_client.client_id ==
           client_id)
      {
        /* Modem functionality client */
        gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_modem_func_client.client_id =
          GSTK_MAX_CLIENT + 1;
        gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_modem_func_client.data_format_type =
          GSTK_FORMAT_NONE;
        gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_modem_func_client.is_reg = FALSE;
      }
      if(gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_ui_func_client.client_id ==
           client_id)
      {
        /* UI functionality client */
        gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_ui_func_client.client_id =
          GSTK_MAX_CLIENT + 1;
        gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_ui_func_client.data_format_type =
          GSTK_FORMAT_NONE;
        gstk_instances_ptr[j]->gstk_class_evt_reg_table[i].gstk_ui_func_client.is_reg = FALSE;
      }
    } /* End of for(i = 0... */
  }/* End of for(j = 0 ... */
}/* gstk_delete_client_from_class_evt_reg_table */

/*===========================================================================
FUNCTION gstk_process_class_evt_registration

DESCRIPTION
  This function processes the event registration for proactive commands
  received from gstk clients. It also performs the necessary checks for
  duplicate and invalid registrations.

PARAMETERS
  client_id            [Input] - client identity
  class_bitmask        [Input] - Letter classes for ME hardware dependant
                                 commands
  evt_reg_bitmask      [Input] - event bitmask
  format_type          [Input] - format type
  func_type            [Input] - functionality type
  need_classification  [Input] - whether classfication of events is
                                 required.

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client is not registered
    GSTK_BAD_PARAM             - bad input parameters
    GSTK_ERROR                 - if no class evt registration was valid
    GSTK_SUCCESS               - if client registrations were successfully
                                 processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_class_evt_registration(
  gstk_client_id_type                       client_id,
  uint32                                    class_bitmask,
  uint64                                    evt_reg_bitmask,
  gstk_client_pro_cmd_data_format_enum_type format_type,
  gstk_tk_functionality_enum_type           func_type,
  boolean                                   need_classification)
{
  uint64                  bitmask     = 0;
  uint64                  reg_bitmask = 0;
  uint64                  rem_bitmask = 0;
  uint32                  i           = 0;
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* check for valid client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("Client not registered 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* check for valid func type */
  if(!gstk_util_is_valid_func_type(func_type))
  {
    UIM_MSG_ERR_1("Invalid func type 0x%x", func_type);
    return GSTK_BAD_PARAM;
  }

  /* check for valid format type */
  if(!gstk_util_is_valid_data_format_req(format_type))
  {
    UIM_MSG_ERR_1("Invalid data format type 0x%x", format_type);
    return GSTK_BAD_PARAM;
  }

  /* If classification is required by the client, seperate the events for each
     class from the evt_reg_bitmask
  */
  if(need_classification)
  {
    reg_bitmask = evt_reg_bitmask;
    for(i=GSTK_REG_CATEGORY_0_CMD;i<=GSTK_REG_CATEGORY_M_CMD;i++)
    {
      bitmask     = 0;
      rem_bitmask = 0;
      if(gstk_util_extract_class_specfic_evt_bitmask(
           reg_bitmask,&bitmask,i,&rem_bitmask) == GSTK_SUCCESS)
      {
        if(bitmask != 0x00)
        {
          /* Check for duplicate registrations */
          gstk_status = gstk_util_check_evt_mask_reg_status(bitmask,i,func_type);

          gstk_status = gstk_add_client_to_class_evt_reg_table(client_id,
                                                               i,
                                                               bitmask,
                                                               format_type,
                                                               func_type);
          if(gstk_status != GSTK_SUCCESS)
          {
            return gstk_status;
          }
        }/* if (bitmask != 0x00) */
        reg_bitmask = rem_bitmask;
        if(reg_bitmask == 0)
        {
          break;
        }
      }/* End of if */
    }/* End of for */
  } /* End of if (need_classification) */
  else /* if classification is not needed */
  {
    gstk_status = gstk_util_check_evt_mask_reg_status(evt_reg_bitmask,
                                                      class_bitmask,
                                                      func_type);
    if(gstk_status == GSTK_DUPLICATE_REGISTRATION)
    {
       /* Nothing */
    }
    else if(gstk_status != GSTK_SUCCESS)
    {
      return GSTK_ERROR;
    }
    gstk_status = gstk_add_client_to_class_evt_reg_table(
                    client_id,
                    class_bitmask,
                    evt_reg_bitmask,
                    format_type,
                    func_type);
    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("gstk_add_client_to_class_evt_reg_table 0x%x",
                    gstk_status);
    }
  } /* if(need_classification) */

  return gstk_status;

}/* gstk_process_class_evt_registration */

/*===========================================================================

FUNCTION gstk_add_client_to_class_evt_reg_table

DESCRIPTION
  This function adds a client to the class evt registration table to receive
  notification for a specific proactive command.

PARAMETERS
  client_id            [Input] - client identity
  class_bitmask        [Input] - Letter classes for ME hardware dependant
                                 commands
  evt_reg_bitmask      [Input] - event bitmask
  format_type          [Input] - format type
  func_type            [Input] - functionality type

DEPENDENCIES
  None

COMMENTS
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client is not registered
    GSTK_BAD_PARAM             - bad input parameters
    GSTK_ERROR                 - wrong index calculated
    GSTK_SUCCESS               - if client registrations were
                                 successfully added to the table

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_add_client_to_class_evt_reg_table(
  gstk_client_id_type                       client_id,
  uint32                                    class_bitmask,
  uint64                                    evt_reg_bitmask,
  gstk_client_pro_cmd_data_format_enum_type format_type,
  gstk_tk_functionality_enum_type           func_type)
{
  uint32    l_index           = 0;
  uint32    index             = 0;
  uint32    c_index           = 0;
  uint64    single_bitmask    = evt_reg_bitmask;
  uint64    mask              = 1;
  uint32    adj_offset        = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* check for valid client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("Client not registered 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* check for valid func type */
  if(!gstk_util_is_valid_func_type(func_type) ||
     !gstk_util_is_valid_data_format_req(format_type))
  {
    UIM_MSG_ERR_2("Invalid func type 0x%x,"
                  "Invalid Format type 0x%x", func_type, format_type);
    return GSTK_BAD_PARAM;
  }

  switch(class_bitmask)
  {
  case GSTK_REG_CATEGORY_0_CMD:
    l_index = GSTK_CLASS_0_BEGIN_INDEX;
    break;
  case GSTK_REG_CATEGORY_B_CMD:
    l_index      = GSTK_CLASS_B_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_B_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_C_CMD:
    l_index      = GSTK_CLASS_C_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_C_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_E_CMD:
    l_index      = GSTK_CLASS_E_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_E_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_L_CMD:
    l_index      = GSTK_CLASS_L_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_L_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_M_CMD:
    l_index      = GSTK_CLASS_M_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_M_ADJ_OFFSET;
    break;
  default:
      UIM_MSG_LOW_1("Invalid class bitmask 0x%x", class_bitmask);
    return GSTK_ERROR;
  }

  /* Evt bitmask for different classes at present do not overlap due to backward
     compatibility. Hence the two lines below will avoiding checking bits for
     irrelevant classes and directly jump to the bit where a particular class's
     events begin
  */
  single_bitmask = single_bitmask >> l_index;
  index          = index + l_index;

  while(single_bitmask)
  {
    if(single_bitmask & mask)
    {
      c_index = l_index + index - adj_offset;
      if((c_index >= GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY) || ((int32) c_index < 0))
      {
        UIM_MSG_ERR_1("Invalid index 0x%x", c_index);
        return GSTK_ERROR;
      }
      switch(func_type)
      {
      case GSTK_HANDLE_DISPLAY_FUNC_ONLY:
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.is_reg = TRUE;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.data_format_type =
          format_type;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.client_id =
          client_id;
        break;
      case GSTK_HANDLE_MODEM_FUNC_ONLY:
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg = TRUE;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.data_format_type =
          format_type;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.client_id =
          client_id;
        break;
      case GSTK_HANDLE_ALL_FUNC:
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.is_reg = TRUE;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.data_format_type =
          format_type;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.client_id =
          client_id;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg = TRUE;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.data_format_type =
          format_type;
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.client_id =
          client_id;
        break;
      default:
        UIM_MSG_ERR_1("Invalid function Type 0x%x", func_type);
        break;
      } /* End of switch */
    }/* If(single_bitmask & mask) */
    single_bitmask = single_bitmask >> 1;
    index++;
  }/* End of while */

  return GSTK_SUCCESS;

}/* gstk_add_client_to_class_evt_reg_table */

/*===========================================================================
FUNCTION   gstk_fill_error_response

DESCRIPTION
  This function fills uim cmd ptr with error response when the length of
  terminal response exceeds the max chars limit for uim.

PARAMETERS
  uim_cmd_ptr          :  Pointer to UIM command
  STK_response_header  :  Terminal Response header type
  byte_offset          :  offset

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_fill_error_response(
  uim_cmd_type                        *uim_cmd_ptr,
  gstk_terminal_response_header_type  STK_response_header,
  int                                 byte_offset
)
{
  gstk_status_enum_type  gstk_status                = GSTK_ERROR;
  int                    gstk_length_of_error_rsp   = 0;
  if(uim_cmd_ptr != NULL)
  {
    gstk_length_of_error_rsp = STK_response_header.command_details.length +
                               GSTK_TAG_LENGTH_LEN +
                               STK_response_header.device_id.device_tag_length +
                               GSTK_TAG_LENGTH_LEN +
                               4;/* 4 is the length of result tlv*/
    if(gstk_length_of_error_rsp < UIM_MAX_TR_BYTES)
    {
      /* copy command details */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &STK_response_header.command_details,
                                 byte_offset,
                                 (size_t)(STK_response_header.command_details.length +
                                          GSTK_TAG_LENGTH_LEN),
                                 sizeof(uim_cmd_ptr->terminal_response.data),
                                 sizeof(gstk_command_details_tlv_type)) <
         (size_t)(STK_response_header.command_details.length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      /* copy device */
      else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &STK_response_header.device_id,
                                      byte_offset +=
                                      (int)(STK_response_header.command_details.length +
                                            GSTK_TAG_LENGTH_LEN),
                                      (size_t)(STK_response_header.device_id.device_tag_length +
                                               GSTK_TAG_LENGTH_LEN),
                                      sizeof(uim_cmd_ptr->terminal_response.data),
                                      sizeof(gstk_device_identities_tag_tlv_type)) <
              (size_t)(STK_response_header.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
        /* copy result */
      else if ((byte_offset +=
               (int)(STK_response_header.device_id.device_tag_length +
                     GSTK_TAG_LENGTH_LEN )) < (UIM_MAX_TR_BYTES - 3))
      {
        uim_cmd_ptr->terminal_response.data[byte_offset++] = STK_response_header.result.result_tag;
        uim_cmd_ptr->terminal_response.data[byte_offset++] = GSTK_ERROR_RESP_RESULT_LEN;
        uim_cmd_ptr->terminal_response.data[byte_offset++] = uint32touint8((uint32)GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
        uim_cmd_ptr->terminal_response.data[byte_offset++] = uint32touint8((uint32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        /* copy number of bytes in error response */
        uim_cmd_ptr->terminal_response.num_bytes           = uint32touint8((uint32)byte_offset);
        gstk_status = GSTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_1("Offset out of range 0x%x", byte_offset);
        gstk_status = GSTK_MEMORY_ERROR;
      }
    }
    else
    {
      UIM_MSG_ERR_0("Length of Terminal response exceeding Max Chars in UIM Buffer");
      gstk_status = GSTK_INVALID_LENGTH;
    }
  }
  else
  {
    UIM_MSG_ERR_0("no UIM cmd buf");
    gstk_status = GSTK_MEMORY_ERROR;
  }
  return gstk_status;
}/* gstk_fill_error_response */

#ifdef FEATURE_ESTK
/*===========================================================================

FUNCTION gstk_get_state

DESCRIPTION
  This function returns the current gstk state

PARAMETERS
  None

DEPENDENCIES
  None

COMMENTS
  None

RETURN VALUE
  gstk_state_enum_type
    GSTK_START_S                   - gstk start state
    GSTK_TERM_PROFILE_WAIT_S       - waiting for client profile dl
    GSTK_MMGSDI_SIG_WAIT_S         - waiting for GSDI signal to do TP dl
    GSTK_TERM_PROFILE_RSP_WAIT_S   - waiting for tp dl response from UIM
    GSTK_IDLE_S                    - Idle state after successful tp dl
    GSTK_NO_SIM_S                  - Bad SIM state
    GSTK_POWER_DOWN_S              - Power down state

SIDE EFFECTS
  None
===========================================================================*/
gstk_state_enum_type gstk_get_state(gstk_slot_id_enum_type slot_id)
{
  if (!GSTK_IS_VALID_SLOT_ID(slot_id))
  {
    return GSTK_INVALID_S;
  }
  return gstk_instances_ptr[(uint32)slot_id - 1]->gstk_state;
}/* gstk_get_state */

/*===========================================================================

FUNCTION gstk_get_sub_state

DESCRIPTION
  This function returns the current gstk sub state

PARAMETERS
  None

DEPENDENCIES
  None

COMMENTS
  None

RETURN VALUE
  gstk_sub_state_enum_type
    GSTK_INVALID_SUB_S
    GSTK_RECOVERY_SUB_S

SIDE EFFECTS
  None
===========================================================================*/
gstk_sub_state_enum_type gstk_get_sub_state(gstk_slot_id_enum_type slot_id)
{
  if (!GSTK_IS_VALID_SLOT_ID(slot_id))
  {
    return GSTK_INVALID_SUB_S;
  }
  return gstk_instances_ptr[(uint32)slot_id - 1]->gstk_sub_state;
}/* gstk_get_sub_state */
#endif /* FEATURE_ESTK */

/*===========================================================================
FUNCTION gstk_is_env_cmd_to_be_skipped

DESCRIPTION
  This function checks if the first command in gstk command queue has to be skipped
  for processing the same or not.

PARAMETERS
  None

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  TRUE : If first command is BIP Evt Dl
  FALSE: If first command is not BIP Evt Dl

SIDE EFFECTS
  This will determine if fetch can be handeled by UIM or not. Also it will
  be used to check if BIP envelope can be processed by gstk or not.

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_env_cmd_to_be_skipped(
)
{
  gstk_task_cmd_type *gstk_first_cmd_ptr = NULL;
  gstk_first_cmd_ptr = (gstk_task_cmd_type*) q_check(&gstk_task_cmd_q);
  if(gstk_first_cmd_ptr == NULL)
  {
    return FALSE;
  }
  if(gstk_first_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_ENVELOPE_CMD)
  {
    gstk_slot_id_enum_type slot_id = gstk_first_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id;
    switch (gstk_first_cmd_ptr->cmd.general_cmd.message_header.command_id)
    {
      case GSTK_DATA_AVAIL_EVT_IND:
      case GSTK_CH_STATUS_EVT_IND:
        return TRUE;
      case GSTK_CC_IND:
        if ((GSTK_IS_VALID_SLOT_ID(slot_id)) &&
            gstk_instances_ptr[slot_id - 1]->gstk_block_cc_env)
          return TRUE;
        return FALSE;
      default:
        return FALSE;
    }
  }
  return FALSE;
}/* gstk_is_first_env_cmd_to_be_skipped */

/*===========================================================================
FUNCTION   gstk_switch_to_instance

DESCRIPTION
  This function switches current context data (private data) to a specific
  instance assiciated with specified SIM slot id

PARAMETERS
  None

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  boolean

SIDE EFFECTS
  This will determine if fetch can be handeled by UIM or not. Also it will
  be used to check if BIP envelope can be processed by gstk or not.

SEE ALSO
  None
===========================================================================*/
boolean gstk_switch_to_instance(gstk_slot_id_enum_type sim_slot_id)
{
  uint8 state_priority[] =
    {
      0, /* GSTK_INVALID_S */
      1, /* GSTK_START_S */
      4, /* GSTK_TERM_PROFILE_WAIT_S */
      5, /* GSTK_MMGSDI_SIG_WAIT_S */
      6, /* GSTK_TERM_PROFILE_RSP_WAIT_S */
      7, /* GSTK_IDLE_S */
      2, /* GSTK_NO_SIM_S */
      3  /* GSTK_POWER_DOWN_S */
    };
  uint32 i        = 0;
  uint32 slot_idx = 0;
  uint32 slot_pri = 0;

  if (!GSTK_IS_VALID_SLOT_ID(sim_slot_id) && sim_slot_id != GSTK_SLOT_AUTO)
  {
    UIM_MSG_ERR_1("can't switch context, invalid sim slot id 0x%x!",
                  sim_slot_id);
    return FALSE;
  }

  if (sim_slot_id == GSTK_SLOT_AUTO)
  {
    slot_idx = 0; /* slot 1 */
    slot_pri = state_priority[(uint32)gstk_instances_ptr[slot_idx]->gstk_state];
    /* starting from slot 2 */
    for (i = 1; i < gstk_shared_data.gstk_num_of_instances ; ++i)
    {
      if (slot_pri < state_priority[(uint32)gstk_instances_ptr[i]->gstk_state])
      {
        slot_idx = i;
        slot_pri = state_priority[(uint32)gstk_instances_ptr[i]->gstk_state];
      }
    }
    UIM_MSG_HIGH_2("slot id is auto. switch to instance 0x%x slot_pri 0x%x",
                   slot_idx, slot_pri);
    gstk_curr_inst_ptr = gstk_instances_ptr[slot_idx];
  }
  else
  {
    gstk_curr_inst_ptr = gstk_instances_ptr[(uint32)sim_slot_id - 1];
  }

  return TRUE;
} /*gstk_switch_to_instance*/

/*===========================================================================
FUNCTION gstk_process_mmgsdi_evt

DESCRIPTION
  Process MMGSDI events

PARAMETERS
  cmd_ptr: [Input] MMGSDI event ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_mmgsdi_evt(
  gstk_cmd_type *cmd_ptr
)
{
  uint32                  i             = 0;
  gstk_slot_id_enum_type  slot          = GSTK_SLOT_AUTO;
  gstk_status_enum_type   gstk_status   = GSTK_SUCCESS;
  gstk_cmd_from_card_type response;
#if defined(FEATURE_GSTK_FUSION_REMOTE)
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_access_type      imsi_file_access;
#endif /* FEATURE_GSTK_FUSION_REMOTE */

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  if(cmd_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_1("gstk_process_mmgsdi_evt(): evt 0x%x",
                 cmd_ptr->mmgsdi_evt.message_header.command_id);

  switch (cmd_ptr->mmgsdi_evt.message_header.command_id)
  {
    case GSTK_MMGSDI_CARD_ERROR_EVT:
      slot = (gstk_slot_id_enum_type)cmd_ptr->mmgsdi_evt.evt.card_error.slot;
      /* Card error may come during recovery is in process */
      if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
      {
        /* Move to non recovery state */
        gstk_curr_inst_ptr->gstk_sub_state = GSTK_INVALID_SUB_S;
      }

      /* Find and process any pending envelope responses and terminal response report in gstk_cmd_q */
      gstk_util_find_and_process_pending_responses();

      /* Cleanup pending envelope commands by sending error envelope response */
      for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
      {
        if(gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[i].client_id) == slot &&
           !gstk_shared_data.envelope_cmd_ref_table[i].is_free )
        {
          if(gstk_util_build_error_envelope_response(i) != GSTK_SUCCESS)
          {
            gstk_util_env_cmd_ref_table_free_slot(i);
          }
        }
      }

      /* Clear retry data since we already cleared envelope_cmd_ref_table */
      gstk_util_clear_retry_envelope_data(slot);

      /* Clean up Pending proactive command */
      for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
      {
        if ((!command_details_ref_table_ptr[i].free_slot) &&
            (command_details_ref_table_ptr[i].sim_slot_id ==  slot) )
        {
          gstk_util_cmd_details_ref_table_free_slot(i);
          /* Only one proactive command can be processed at a time */
          break;
        }
      }

      if(gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress == TRUE)
      {
        if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2, GSTK_SLOT_ID_MAX) == TRUE)
        {
          if(gstk_curr_inst_ptr->tr_unique_ref_id != 0)
          {
            /* TR already sent to uimdrv, check whether it is sent by GSTK or client */
            gstk_client_id_type client_id  = GSTK_CMD_REF_TABLE_INDEX(gstk_curr_inst_ptr->tr_unique_ref_id);
            if(client_id != gstk_curr_inst_ptr->client_id)
            {
               gstk_util_send_sw_to_client(NULL);
            }
          }
          else
          {
            /* check the queue for pending RAW TR */
            gstk_task_cmd_type *cmd_p = gstk_util_find_terminal_rsp_in_queue();
            if(cmd_p != NULL)
            {
              if(cmd_p->cmd.general_cmd.message_header.command_id == GSTK_RAW_TERMIMAL_RESPONSE)
              {
                gstk_client_raw_term_resp_type   *raw_term_resp_ptr = &cmd_p->cmd.client_raw_term_resp;
                gstk_shared_data.gstk_client_table[raw_term_resp_ptr->client_id - 1].user_data =
                                                          raw_term_resp_ptr->message_header.user_data;
                /* TR is not generated by GSTK */
                gstk_util_send_sw_to_client(NULL);
              }
              gstk_task_free_cmd_buf(cmd_p);
            }
          }
        }
        /* find and send end proactive seesion for pending proactive command */
        gstk_util_find_and_build_proactive_end_session(GSTK_TAG_CARD_ERROR);
      }

      /* Move to GSTK_NO_SIM state */
      gstk_curr_inst_ptr->gstk_state = GSTK_NO_SIM_S;

      /* Clean up the GSTK Global data */
      gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_CARD_ERROR);
      break;

    case GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT:
      memset(&response, 0x00, sizeof(gstk_cmd_from_card_type));

      if(cmd_ptr->mmgsdi_evt.evt.terminal_profile.ok_dl)
      {
        /* use the gstk_terminal_profile_cache info and send it to uim */
        gstk_status = gstk_pack_tp_cmd_to_uim();
        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_inst_ptr->tp_info.user_data = cmd_ptr->mmgsdi_evt.message_header.user_data;
          gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb = cmd_ptr->mmgsdi_evt.client_cb;
          /* go to wait for TP response state */
          gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
        }
        else
        {
          UIM_MSG_ERR_1("Unbale to send TP to card in state 0x%x",
                        gstk_curr_inst_ptr->gstk_state);
          gstk_curr_inst_ptr->gstk_state        = GSTK_NO_SIM_S;
          /* Unblock MMGSDI */
          response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
          response.hdr_cmd.sim_slot_id          = cmd_ptr->mmgsdi_evt.message_header.sim_slot_id;
          response.hdr_cmd.cmd_detail_reference = 0;
          response.hdr_cmd.command_number       = 0;
          response.hdr_cmd.user_data            = cmd_ptr->mmgsdi_evt.message_header.user_data;
          if(cmd_ptr->mmgsdi_evt.client_cb != NULL)
          {
            (cmd_ptr->mmgsdi_evt.client_cb)(&response);
          }
        }
      }
      else
      {
        gstk_curr_inst_ptr->gstk_state        = GSTK_NO_SIM_S;
        /* Unblock MMGSDI */
        response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
        response.hdr_cmd.sim_slot_id          = cmd_ptr->mmgsdi_evt.message_header.sim_slot_id;
        response.hdr_cmd.cmd_detail_reference = 0;
        response.hdr_cmd.command_number       = 0;
        response.hdr_cmd.user_data            = cmd_ptr->mmgsdi_evt.message_header.user_data;
        if(cmd_ptr->mmgsdi_evt.client_cb != NULL)
        {
          (cmd_ptr->mmgsdi_evt.client_cb)(&response);
        }
      }
      break;

    case GSTK_MMGSDI_CARD_INSERTED_EVT:
      gstk_get_and_store_curr_card_protocol();

      /* If NV items had been updated due to non-standard NV refresh in
         the previous card power-cycle, re-read the same.
         Card Inserted event handling is the earliest that this re-read
         can be attempted in a card power-cycle for ICC cards */
      if(gstk_curr_inst_ptr->card_protocol == MMGSDI_ICC)
      {
        gstk_nv_init_slot_cache(
          GSTK_NV_INIT_CATEG_POWER_UP_INIT,
          gstk_curr_inst_ptr->slot_id);
      }
      break;

    case GSTK_MMGSDI_SESSION_CHANGED_EVT:
      gstk_process_session_changed_evt(cmd_ptr);
      break;

    case GSTK_MMGSDI_SUBSCRIPTION_READY_EVT:
      for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
      {
        if (gstk_shared_data.mmgsdi_sess_info[i].sess_id ==
            cmd_ptr->mmgsdi_evt.session_id)
        {
          if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_PRI_SESSION) ||
             (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_SEC_SESSION) ||
             (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_TER_SESSION))
          {
            gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw = TRUE;
          }
          else if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_PRI_SESSION) ||
                  (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_SEC_SESSION) ||
                  (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_TER_SESSION))
          {
            gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x = TRUE;
          }

          if (gstk_nv_get_feature_status(
                GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                gstk_curr_inst_ptr->slot_id) == TRUE)
          {
            if (gstk_curr_inst_ptr->imsi_switched)
            {
              gstk_curr_inst_ptr->imsi_switched = FALSE;
              UIM_MSG_HIGH_0("loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED");
              gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;
            }
          }
          gstk_shared_data.mmgsdi_sess_info[i].slot =
            (gstk_slot_id_enum_type)cmd_ptr->mmgsdi_evt.evt.subscription_ready.app_info.slot;
#if defined(FEATURE_GSTK_FUSION_REMOTE)
          /* Read USIM IMSI file and set NV NV_GPRS_ANITE_GCF_I */
          memset(&imsi_file_access, 0x00, sizeof(mmgsdi_access_type));
          imsi_file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
          if(cmd_ptr->mmgsdi_evt.evt.subscription_ready.app_info.app_data.app_type
             == MMGSDI_APP_USIM)
          {
            imsi_file_access.file.file_enum = MMGSDI_USIM_IMSI;

            mmgsdi_status = mmgsdi_session_read_transparent(
              cmd_ptr->mmgsdi_evt.session_id,
              imsi_file_access,
              (mmgsdi_offset_type)0,
              (mmgsdi_len_type)0,
              gstk_mmgsdi_imsi_verification_cb,
              (mmgsdi_client_data_type)0);
          }
          else if(cmd_ptr->mmgsdi_evt.evt.subscription_ready.app_info.app_data.app_type
             == MMGSDI_APP_SIM)
          {
            imsi_file_access.file.file_enum = MMGSDI_GSM_IMSI;

            mmgsdi_status = mmgsdi_session_read_transparent(
              cmd_ptr->mmgsdi_evt.session_id,
              imsi_file_access,
              (mmgsdi_offset_type)0,
              (mmgsdi_len_type)0,
              gstk_mmgsdi_imsi_verification_cb,
              (mmgsdi_client_data_type)0);
           }
           UIM_MSG_HIGH_1("Queued command to MMGSDI with status 0x%x for GSM IMSI",
                           mmgsdi_status);
#endif /* FEATURE_GSTK_FUSION_REMOTE  */
        } /* End of - if */
      } /* End of - for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)*/
      break;

    case GSTK_MMGSDI_REFRESH_EVT:
      if(cmd_ptr->mmgsdi_evt.evt.refresh.stage == MMGSDI_REFRESH_STAGE_START)
      {
        slot = cmd_ptr->mmgsdi_evt.evt.refresh.slot;
        if((cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_RESET) ||
           (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_RESET_AUTO))
        {
          /* Find and process any pending envelope responses and terminal response report in gstk_cmd_q */
          gstk_util_find_and_process_pending_responses();

          /* Cleanup pending envelope commands by sending error envelope response */
          for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
          {
            if(gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[i].client_id) == slot &&
               !gstk_shared_data.envelope_cmd_ref_table[i].is_free)
            {
              if(gstk_util_build_error_envelope_response(i) != GSTK_SUCCESS)
              {
                gstk_util_env_cmd_ref_table_free_slot(i);
              }
            }
          }

          /* Clean up Pending proactive command */
          for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
          {
            if ((!command_details_ref_table_ptr[i].free_slot) &&
                (command_details_ref_table_ptr[i].sim_slot_id ==  slot) )
            {
              gstk_util_cmd_details_ref_table_free_slot(i);
              /* Only one proactive command can be processed at a time */
              break;
            }
          }

          if(gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress == TRUE)
          {
            /* find and send end proactive seesion for pending proactive command */
            gstk_util_find_and_build_proactive_end_session(GSTK_TAG_REFRESH_RESET);
          }

          /* Move to GSTK_NO_SIM state */
          gstk_curr_inst_ptr->gstk_state = GSTK_NO_SIM_S;
          gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_REFRESH_RESET);
        }
        else if((cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_INIT) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_APP_RESET) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_3G_SESSION_RESET))
        {
          if((cmd_ptr->mmgsdi_evt.evt.refresh.app_info.app_data.app_type == MMGSDI_APP_SIM)||
             (cmd_ptr->mmgsdi_evt.evt.refresh.app_info.app_data.app_type == MMGSDI_APP_RUIM))
          {
            gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_REFRESH_2G_INIT);
          }

          for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
          {
            if (gstk_shared_data.mmgsdi_sess_info[i].sess_id ==
                cmd_ptr->mmgsdi_evt.session_id)
            {
              if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_PRI_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_SEC_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_TER_SESSION))
              {
                gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw = FALSE;
              }
              else if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_PRI_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_SEC_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_TER_SESSION))
              {
                gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x = FALSE;
              }
            }
          }
        }
      }
      UIM_MSG_HIGH_2("SUBS_READY GW: 0x%x, 1x : 0x%x",
                   gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw,
                   gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x);
      break;

    case GSTK_MMGSDI_LOC_ENV_RSP_RCVD_CNF_EVT:
      UIM_MSG_HIGH_1("Received Location Env Rsp Rcvd conf cmd; imsi_switched: 0x%x",
                     cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched);
      if (gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
            gstk_curr_inst_ptr->slot_id) == TRUE)
      {
        gstk_curr_inst_ptr->imsi_switched =
        cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched;
        if (cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched)
        {
          gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
          gstk_util_send_attach_ctrl_res();
        }
        else
        {
          gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
          gstk_util_send_attach_ctrl_res();
        }
      }
      break;

    default:
      break;
  }
} /*gstk_process_mmgsdi_evt*/

/*===========================================================================
FUNCTION gstk_process_ds_system_status_ex_ind_wlan_status

DESCRIPTION
  Processes System Status Indication from DS w.r.t. change to WLAN status.
  Posts message to ESTK if WLAN status has changed.

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_process_ds_system_status_ex_ind_wlan_status(
  gstk_cmd_type *cmd_ptr
)
{
  uint32         index             = 0;
#if defined(FEATURE_ESTK)
  estk_cmd_type *estk_task_cmd_ptr = NULL;
  boolean        wlan_avail_temp   = FALSE;
#endif /* FEATURE_ESTK */

  UIM_MSG_HIGH_0("In gstk_process_ds_system_status_ex_ind_wlan_status");

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr NULL");
    return;
  }

  /* Check for WLAN status */
#if defined(FEATURE_ESTK)
  wlan_avail_temp = gstk_shared_data.wlan_info.is_wlan_available;
#endif /* FEATURE_ESTK */

  /* Mark WLAN as unavailable  */
  gstk_shared_data.wlan_info.is_wlan_available = FALSE;

  /* Check if WLAN (local breakout, i.e. not I-WLAN) is available */
  for(index = 0;
      index < cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.num_avail_sys;
      index++)
  {
    if((cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].technology ==
        DS_SYS_NETWORK_WLAN) &&
       (cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].rat_value ==
        DS_SYS_RAT_EX_WLAN))
    {
      gstk_shared_data.wlan_info.is_wlan_available = TRUE;
      break;
    }
  }

  UIM_MSG_HIGH_1("GSTK_DS_SYSTEM_STATUS_EX_IND, WLAN Status 0x%x",
                 gstk_shared_data.wlan_info.is_wlan_available);

#if defined(FEATURE_ESTK)
  /* If WLAN availability has changed, notify ESTK */
  if(wlan_avail_temp != gstk_shared_data.wlan_info.is_wlan_available)
  {
    estk_task_cmd_ptr = estk_get_cmd_buf();
    if (estk_task_cmd_ptr != NULL)
    {
      estk_task_cmd_ptr->hdr.cmd            = ESTK_BIP_WLAN_STATUS_CHANGE_EVT_CMD;
      estk_task_cmd_ptr->hdr.cmd_ref_id     = 0;           /*doesn't matter*/
      estk_task_cmd_ptr->hdr.command_number = 0;           /*doesn't matter*/
      estk_task_cmd_ptr->hdr.sim_slot_id    = GSTK_SLOT_1; /* doesn't matter */
      estk_task_cmd_ptr->cmd_data.wlan_status_evt.wlan_available =
        gstk_shared_data.wlan_info.is_wlan_available;

      estk_put_cmd(estk_task_cmd_ptr);
    }
  }
#endif /* FEATURE_ESTK */
} /* gstk_process_ds_system_status_ex_ind_wlan_status */

/*===========================================================================
FUNCTION gstk_cache_ds_system_status_ex_ind_process_cellular_status

DESCRIPTION
  This function caches the DS system status.
  It also Processes System Status Indication from DS w.r.t. change to
  Cellular status and posts message to ESTK if no Cellular RATs are available.

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_cache_ds_system_status_ex_ind_process_cellular_status(
  gstk_cmd_type *cmd_ptr
)
{
  uint32                  index               = 0;
  gstk_slot_id_enum_type  slot                = GSTK_SLOT_AUTO;
#if defined(FEATURE_ESTK)
  estk_cmd_type          *estk_task_cmd_ptr   = NULL;
  boolean                 cellular_avail_temp = FALSE;
#endif /* FEATURE_ESTK */

  if(cmd_ptr == NULL)
  {
    return;
  }

  /* Caching available networks obtained from DS through callback */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  if (gstk_io_ctrl(GSTK_IO_DS_SUBSCRIPTION_ID_TO_SLOT_ID,
                   &cmd_ptr->ds_sys_status_ex_ind.subscription_id,
                   &slot) != GSTK_SUCCESS)
  {
    return;
  }
#else
  slot = GSTK_SLOT_1;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  GSTK_RETURN_IF_NULL_PTR(gstk_instances_ptr[(uint32)slot - 1]);

  gstk_instances_ptr[(uint32)slot - 1]->ds_sys_status_info.num_avail_sys =
    cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.num_avail_sys;
  (void)memscpy(gstk_instances_ptr[(uint32)slot - 1]->ds_sys_status_info.avail_sys,
          sizeof(ds_sys_system_status_info_type) * gstk_instances_ptr[(uint32)slot - 1]->ds_sys_status_info.num_avail_sys,
          cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys,
          sizeof(ds_sys_system_status_info_type) * gstk_instances_ptr[(uint32)slot - 1]->ds_sys_status_info.num_avail_sys);

#if defined(FEATURE_ESTK)
  cellular_avail_temp = gstk_shared_data.cellular_data_available;
#endif /* FEATURE_ESTK */

  /* Mark Celluar DATA as unavailable  */
  gstk_shared_data.cellular_data_available = FALSE;


  /* Check if any Cellular RATs are available */
  for(index = 0;
      index < cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.num_avail_sys;
      index++)
  {
    if((cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].technology ==
        DS_SYS_NETWORK_3GPP) ||
       (cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].technology ==
        DS_SYS_NETWORK_3GPP2))
    {
      if(cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].rat_value !=
         DS_SYS_RAT_EX_NULL_BEARER)
      {
        gstk_shared_data.cellular_data_available = TRUE;
        break;
      }
    }
  }

  UIM_MSG_HIGH_1("GSTK_DS_SYSTEM_STATUS_EX_IND, Cellular Status 0x%x",
                 gstk_shared_data.cellular_data_available);

#if defined(FEATURE_ESTK)
  /* If Cellular Data availability has changed,  notfiy ESTK */
  if(cellular_avail_temp != gstk_shared_data.cellular_data_available)
  {
    estk_task_cmd_ptr = estk_get_cmd_buf();
    if (estk_task_cmd_ptr != NULL)
    {
      estk_task_cmd_ptr->hdr.cmd            = ESTK_BIP_CELLULAR_STATUS_CHANGE_EVT_CMD;
      estk_task_cmd_ptr->hdr.cmd_ref_id     = 0;           /*doesn't matter*/
      estk_task_cmd_ptr->hdr.command_number = 0;           /*doesn't matter*/
      estk_task_cmd_ptr->hdr.sim_slot_id    = slot;
      estk_task_cmd_ptr->cmd_data.cellular_status_evt.cellular_service_available =
        gstk_shared_data.cellular_data_available;

      estk_put_cmd(estk_task_cmd_ptr);
    }
  }
#endif /* FEATURE_ESTK*/
} /* gstk_cache_ds_system_status_ex_ind_process_cellular_status */

/*===========================================================================
FUNCTION gstk_process_ds_system_status_ex_ind

DESCRIPTION
  Processes System Status Indication from DS

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_process_ds_system_status_ex_ind(
  gstk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    return;
  }

  /* Process any change in WLAN status */
  gstk_process_ds_system_status_ex_ind_wlan_status(cmd_ptr);

  /* Process any change in Cellular status and queue message to ESTK
  in case of OOS */
  gstk_cache_ds_system_status_ex_ind_process_cellular_status(cmd_ptr);
} /* gstk_process_ds_system_status_ex_ind */

/*===========================================================================
FUNCTION gstk_process_ds_wlan_mac_addr_change_ind

DESCRIPTION
  Processes WLAN MAC Address change indication from DS

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_process_ds_wlan_mac_addr_change_ind(
  gstk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr NULL");
    return;
  }

  gstk_util_dump_byte_array(
    "WLAN MAC Address",
    cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info.ap_mac_addr,
    sizeof(cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info.ap_mac_addr));

  /* Check if MAC address has changed - the event can also come in if active
     DS SUB has changed but MAC address is the same */
  if (memcmp(gstk_shared_data.wlan_info.wlan_mac_addr.ap_mac_addr,
             cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info.ap_mac_addr,
             sizeof(gstk_shared_data.wlan_info.wlan_mac_addr.ap_mac_addr)) != 0)
  {
    UIM_MSG_HIGH_0("WLAN MAC address has changed, reset WLAN unusable status if required");
    /* If WLAN MAC address has changed, its likely a new BIP Session over WLAN
       may go through. Hence, reset the WLAN unusable status if its currently
       marked as unusable */
    estk_reset_wlan_unusable_status();
  }

  /* Cache the current WLAN MAC Address */
  gstk_shared_data.wlan_info.wlan_mac_addr =
    cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info;

} /* gstk_process_ds_wlan_mac_addr_change_ind */

/*===========================================================================
FUNCTION gstk_process_ds_wlan_ssid_change_ind

DESCRIPTION
  Processes WLAN SSID change indication from DS

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_process_ds_wlan_ssid_change_ind(
  gstk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr NULL");
    return;
  }

  gstk_util_dump_byte_array(
    "WLAN SSID",
    (byte *)cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid,
    sizeof(cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid));

  /* Check if SSID has changed - the event can also come in if active
     DS SUB has changed but MAC address is the same */
  if (memcmp(gstk_shared_data.wlan_info.wlan_ssid.ssid,
             (uint8*)cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid,
             sizeof(cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid)) != 0)
  {
    UIM_MSG_HIGH_0("WLAN SSID has changed, reset WLAN unusable status if required");
    /* If WLAN SSID has changed, its likely a new BIP Session over WLAN
       may go through. Hence, reset the WLAN unusable status if its currently
       marked as unusable */
    estk_reset_wlan_unusable_status();
  }

  /* Cache the current WLAN SSID */
  gstk_shared_data.wlan_info.wlan_ssid =
    cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info;
} /* gstk_process_ds_wlan_ssid_change_ind */

/*===========================================================================
FUNCTION gstk_process_ds_evt_cmd

DESCRIPTION
  Processes DS events

PARAMETERS
  cmd_ptr: [Input] DS command ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_ds_evt_cmd(
  gstk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_1("In gstk_process_ds_evt_cmd, event: 0x%x",
                 cmd_ptr->general_cmd.message_header.command_id);

  switch((gstk_cmd_enum_type)cmd_ptr->general_cmd.message_header.command_id)
  {
    case GSTK_DS_SYSTEM_STATUS_EX_IND:
      gstk_process_ds_system_status_ex_ind(cmd_ptr);
      break;

    case GSTK_DS_WLAN_MAC_ADDR_CHANGE_IND:
      gstk_process_ds_wlan_mac_addr_change_ind(cmd_ptr);
      break;

    case GSTK_DS_WLAN_SSID_CHANGE_IND:
      gstk_process_ds_wlan_ssid_change_ind(cmd_ptr);
      break;

    default:
      return;
  }
} /* gstk_process_ds_evt_cmd */

/*===========================================================================
FUNCTION gstk_process_otasp_reg_cmd

DESCRIPTION
  Processes otasp reg cmd

PARAMETERS
  cmd_ptr: [Input] command ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_otasp_reg_cmd(
  gstk_cmd_type *cmd_ptr
)
{
  gstk_status_enum_type gstk_status = GSTK_ERROR;

  if(cmd_ptr != NULL)
  {
    if(gstk_shared_data.gstk_otasp_status_cb == NULL)
    {
      gstk_status = GSTK_SUCCESS;
      gstk_shared_data.gstk_otasp_status_cb = cmd_ptr->otasp_reg_cmd.gstk_otasp_status_cb;
    }
    else
    {
      gstk_status = GSTK_DUPLICATE_REGISTRATION;
      UIM_MSG_ERR_0("one OTASP callback is already register");
    }
    (cmd_ptr->otasp_reg_cmd.gstk_otasp_reg_cb)(gstk_status, cmd_ptr->otasp_reg_cmd.user_data);
    if(gstk_curr_inst_ptr->otasp_failure_cnt >= GSTK_MAX_OTASP_RETRY_CNT)
    {
      (gstk_shared_data.gstk_otasp_status_cb)(gstk_curr_inst_ptr->slot_id, GSTK_OTASP_ACT_STATUS_PERM_FAIL);
    }
  }
} /* gstk_process_otasp_reg_cmd */

/*===========================================================================
FUNCTION gstk_process_cm_subs_change_ind

DESCRIPTION
  Processes the Subscription change indication from Call Manager

PARAMETERS
  cmd_ptr: [Input] CM indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_cm_subs_change_ind(
  gstk_cmd_type *cmd_ptr
)
{
  gstk_slot_id_enum_type slot          = GSTK_SLOT_AUTO;
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
  gstk_task_cmd_type     *task_cmd_ptr = NULL;
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */

  if(cmd_ptr == NULL)
  {
    return;
  }

  slot = (gstk_slot_id_enum_type)cmd_ptr->subs_change_ind.message_header.sim_slot_id;

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  UIM_MSG_HIGH_1("In gstk_process_cm_subs_change_ind, Subs Mode: 0x%x",
                 cmd_ptr->subs_change_ind.subs_feature_mode);

  /* Cache Subs Mode */
  gstk_instances_ptr[(uint32)slot - 1]->subs_feature_mode =
    cmd_ptr->subs_change_ind.subs_feature_mode;

#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
  /* If GSTK is currently waiting for 1x RAT to become available (in order to
     trigger OTASP for card activation)... */
  if(gstk_curr_inst_ptr->open_ch_imsi_m_ptr &&
     rex_get_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer)) > 0)
  {
    /* If device is SVLTE/SRLTE, attempt activation via 1x */
    if(cmd_ptr->subs_change_ind.subs_feature_mode == GSTK_SUBS_FEATURE_MODE_SVLTE ||
       cmd_ptr->subs_change_ind.subs_feature_mode == GSTK_SUBS_FEATURE_MODE_SRLTE)
    {
      /* If 1x RAT is available queue the OTASP call command along with
         cached OPEN CHANNEL command */
      if(gstk_curr_inst_ptr->bmsk_3gpp2 & GSTK_ACCESS_TECH_CDMA_MASK)
      {
        UIM_MSG_HIGH_0("1X available, clear timer and retry OTASP origination");

        (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
        gstk_curr_inst_ptr->otasp_requeued = TRUE;

        task_cmd_ptr = gstk_task_get_cmd_buf();
        if (task_cmd_ptr != NULL)
        {
          task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
          task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_ORIG;
          task_cmd_ptr->cmd.general_cmd.message_header.user_data =
            (uint32)gstk_curr_inst_ptr->open_ch_imsi_m_ptr;
          task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = slot;
          gstk_task_put_cmd_buf(task_cmd_ptr);
        }
      }
      /* If 1x RAT is not yet available, start 30 sec timer to wait for
         the same */
      else
      {
        UIM_MSG_HIGH_1("1x not yet available, set timer: 0x%x",
                       GSTK_1X_AVAIL_WAIT_TIMER);
        (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
        (void) rex_set_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer),
                             GSTK_1X_AVAIL_WAIT_TIMER);
      }
    }
    /* If device is non-SVLTE, then proceed to activation via BIP OPEN
       CHANNEL */
    else
    {
      UIM_MSG_HIGH_0("Device is non-SVLTE, do activation over BIP");
      (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
      (void)gstk_open_ch_req(gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
      gstk_free((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
      gstk_curr_inst_ptr->open_ch_imsi_m_ptr = NULL;
    }
  }
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */
} /* gstk_process_cm_subs_change_ind */

/*===========================================================================
FUNCTION gstk_process_mode_change_update_3gpp2_bmsk

DESCRIPTION
  This function processes the CM mode change event and:
  - Updates the variable 3gpp2_bmsk in current instance pointer
  - If 1x RAT becomes available when GSTK is waiting for it to trigger OTASP
    call for card acivation, an OTASP command is queued

PARAMETER
  mode_change_ind : CM mode change indication command

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_mode_change_update_3gpp2_bmsk(
  gstk_mode_change_ind_type* mode_change_ind)
{
  int i = 0;
  gstk_slot_id_enum_type slot          = GSTK_SLOT_AUTO;
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
  gstk_task_cmd_type *task_cmd_ptr = NULL;
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */

  if(mode_change_ind == NULL)
  {
    return;
  }

  slot = (gstk_slot_id_enum_type)mode_change_ind->msg_hdr.sim_slot_id;

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  for (i = 0; i < GSTK_CM_NUM_STACKS; ++i)
  {
    if(mode_change_ind->cm_stack[i].loc_status == GSTK_NORMAL_SERVICE)
    {
      switch (mode_change_ind->cm_stack[i].access_tech)
      {
        case GSTK_ACCESS_TECH_CDMA:
          gstk_curr_inst_ptr->bmsk_3gpp2 |= GSTK_ACCESS_TECH_CDMA_MASK;
          break;
        case GSTK_ACCESS_TECH_HRPD:
          gstk_curr_inst_ptr->bmsk_3gpp2 |= GSTK_ACCESS_TECH_HRPD_MASK;
          break;
        case GSTK_ACCESS_TECH_EHRPD:
          gstk_curr_inst_ptr->bmsk_3gpp2 |= GSTK_ACCESS_TECH_EHRPD_MASK;
          break;
        default:
          break;
      }
    }
  }

  UIM_MSG_HIGH_1("Updated 3GPP2 bmsk 0x%x", gstk_curr_inst_ptr->bmsk_3gpp2);

#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
  /* If 1X RAT is available and GSTK is currently waiting for it to become
     available (in order to trigger OTASP for card activation), queue the
     OTASP call command along with cached OPEN CHANNEL command */
  if ((gstk_curr_inst_ptr->bmsk_3gpp2 & GSTK_ACCESS_TECH_CDMA_MASK) &&
      (gstk_curr_inst_ptr->open_ch_imsi_m_ptr) &&
      rex_get_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer)) > 0)
  {
    UIM_MSG_HIGH_0("1X available, clear timer and retry OTASP origination");

    (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
    gstk_curr_inst_ptr->otasp_requeued = TRUE;

    task_cmd_ptr = gstk_task_get_cmd_buf();
    if (task_cmd_ptr != NULL)
    {
      task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
      task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_ORIG;
      task_cmd_ptr->cmd.general_cmd.message_header.user_data =
        (uint32)gstk_curr_inst_ptr->open_ch_imsi_m_ptr;
      task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = slot;
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
  }
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */
} /* gstk_process_mode_change_update_3gpp2_bmsk */

/*===========================================================================
FUNCTION gstk_task_free_cmd_buf

DESCRIPTION
  This function frees the GSTK command buffer memory

PARAMETER
  cmd_ptr The pointer to GSTK command buffer

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_task_free_cmd_buf ( gstk_task_cmd_type * cmd_ptr )
{
  if (cmd_ptr == NULL)
  {
    return;
  }

  switch(cmd_ptr->cmd.general_cmd.message_header.command_group)
  {
    case GSTK_TAL_CMD:
      if(cmd_ptr->cmd.tal_cmd.uim_cmd.data_ptr != NULL)
      {
        gstk_free(cmd_ptr->cmd.tal_cmd.uim_cmd.data_ptr);
        cmd_ptr->cmd.tal_cmd.uim_cmd.data_ptr = NULL;
      }
      break;
    case GSTK_PROACTIVE_CMD:
      break;
    case GSTK_TERMINAL_RSP:
      gstk_util_release_upper_layer_cmd_memory(&(cmd_ptr->cmd));
      break;
    case GSTK_CLIENT_REG_REQ_CMD:
      if(cmd_ptr->cmd.general_cmd.message_header.command_id == GSTK_CLIENT_PRO_CMD_REG_REQ)
      {
        if (cmd_ptr->cmd.client_evt_reg.reg_info_ptr != NULL)
        {
          gstk_free(cmd_ptr->cmd.client_evt_reg.reg_info_ptr);
          cmd_ptr->cmd.client_evt_reg.reg_info_ptr = NULL;
        }
      }
      break;
    case GSTK_ENVELOPE_CMD:
      if(cmd_ptr->cmd.general_cmd.message_header.command_id == GSTK_CALL_CONNECTED_EVT_IND)
      {
        if(cmd_ptr->cmd.call_connected_envelope_ind.transaction_id.transaction_list != NULL)
        {
          gstk_free(cmd_ptr->cmd.call_connected_envelope_ind.transaction_id.transaction_list);
          cmd_ptr->cmd.call_connected_envelope_ind.transaction_id.transaction_list = NULL;
        }
      }
      break;
    default:
      break;
  } /* switch */
  gstk_free(cmd_ptr);
} /* gstk_task_free_cmd_buf */

/*================================================================
FUNCTION  GSTK_SET_QMI_CAT_SIG

DESCRIPTION:
  Sets the GSTK_QMI_CAT_SIG to begin processing of QMI CAT.

INPUT PARAMETERS:
  None

DEPENDENCIES:
  gstk_task must be finished initializing.

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
void gstk_set_qmi_cat_sig(void)
{
  (void)rex_set_sigs(UIM_GSTK_TCB, GSTK_QMI_CAT_SIG);
} /* gstk_set_qmi_cat_sig */


/*===========================================================================
FUNCTION gstk_copy_access_tech

DESCRIPTION
  This local function converts and copies CM mode to GSTK access tech

PARAMETER
  access_tech       : variable in which converted result to be stored
  cm_stack_info_ptr : Pointer to CM stack

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_copy_access_tech(
  gstk_access_technology_type* access_tech_ptr,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr)
{
  if (access_tech_ptr == NULL || cm_stack_info_ptr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(cm_stack_info_ptr->sys_mode)
  {
    case SYS_SYS_MODE_NO_SRV:
     *access_tech_ptr =  GSTK_ACCESS_NONE;
     break;
    case SYS_SYS_MODE_GSM:
      *access_tech_ptr =  GSTK_ACCESS_TECH_GSM;
      break;
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      *access_tech_ptr = GSTK_ACCESS_TECH_UTRAN;
      break;
    case SYS_SYS_MODE_LTE:
      *access_tech_ptr = GSTK_ACCESS_TECH_LTE;
      break;
    case SYS_SYS_MODE_HDR:
      if(cm_stack_info_ptr->rat_mode_info.hdr_mode_info.hdr_personality
         == SYS_PERSONALITY_HRPD)
      {
        *access_tech_ptr = GSTK_ACCESS_TECH_HRPD;
      }
      else if(cm_stack_info_ptr->rat_mode_info.hdr_mode_info.hdr_personality
              == SYS_PERSONALITY_EHRPD)
      {
        *access_tech_ptr = GSTK_ACCESS_TECH_EHRPD;
      }
      else
      {
        UIM_MSG_ERR_1("Unknown HDR personality 0x%x",
                      cm_stack_info_ptr->rat_mode_info.hdr_mode_info.hdr_personality);
        *access_tech_ptr = GSTK_ACCESS_NONE;
      }
      break;
      case SYS_SYS_MODE_CDMA:
        *access_tech_ptr = GSTK_ACCESS_TECH_CDMA;
      break;
      default:
       *access_tech_ptr = GSTK_ACCESS_NONE;
        UIM_MSG_ERR_1("Unknown Sys Mode 0x%x", cm_stack_info_ptr->sys_mode);
        break;
    } /* switch(cm_stack_info_ptr->sys_mode) */

  return GSTK_SUCCESS;
} /* gstk_copy_access_tech */

/*===========================================================================
FUNCTION gstk_copy_location_status

DESCRIPTION
  This local function converts CM service status to CM location status and copies
  it in to a variable

PARAMETER
  loc_status      : variable in which converted result to be stored
  srv_status      : CM sys mode
  hdr_personality : Type of HDR

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_copy_location_status(
  gstk_location_status_enum_type*  loc_status_ptr,
  const sys_srv_status_e_type      srv_status)
{
  if (loc_status_ptr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(srv_status)
  {
    case SYS_SRV_STATUS_NO_SRV:
    case SYS_SRV_STATUS_PWR_SAVE:
      *loc_status_ptr = GSTK_NO_SERVICE;
      break;
    case SYS_SRV_STATUS_SRV:
      *loc_status_ptr = GSTK_NORMAL_SERVICE;
      break;
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
      *loc_status_ptr = GSTK_LIMITED_SERVICE;
      break;
    default:
      UIM_MSG_ERR_0("Invalid Srvc status from CM");
      *loc_status_ptr = GSTK_NO_SERVICE;
      break;
  } /* switch(ss_info_ptr->srv_status) */

  return GSTK_SUCCESS;
} /* gstk_copy_location_status */

/*===========================================================================
FUNCTION gstk_copy_csg_info

DESCRIPTION
  This function copies CSG information

PARAMETER
  task_cmd_ptr       : Pointer to command in which CSG information to be copied
  access_tech        : Access Technology
  cm_stack_info_ptr  : Pointer to CM SS stack

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_copy_csg_info(
  gstk_task_cmd_type*                     task_cmd_ptr,
  const cm_mm_msim_ss_stack_info_s_type*  cm_stack_info_ptr
)
{
  if (cm_stack_info_ptr == NULL || task_cmd_ptr == NULL)
  {
    return;
  }
  switch(cm_stack_info_ptr->sys_mode)
  {
    case SYS_SYS_MODE_GSM:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.csg_id;
      if (cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length)
        {
          UIM_MSG_ERR_0("gstk_copy_csg_info(): gstk_memcpy failed");
          return;
        }
      }
      break;
    case SYS_SYS_MODE_WCDMA:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.csg_id;
      if (cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length)
        {
          UIM_MSG_ERR_0("gstk_copy_csg_info(): gstk_memcpy failed");
          return;
        }
      }
      break;
    case SYS_SYS_MODE_LTE:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.csg_id;
      if (cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length)
        {
          UIM_MSG_ERR_0("gstk_copy_csg_info(): gstk_memcpy failed");
          return;
        }
      }
      break;
    case SYS_SYS_MODE_TDS:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.csg_id;
      if (cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length)
        {
          UIM_MSG_ERR_0("gstk_copy_csg_info(): gstk_memcpy failed");
          return;
        }
      }
      break;
    default:
     return;
  }
  gstk_util_dump_byte_array("hnb_name.name",
              cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.name,
              GSTK_MIN(GSTK_HNB_NAME_MAX_LEN, (task_cmd_ptr->cmd.csg_change_ind.cmd.len + 1)));
}

/*===========================================================================
FUNCTION gstk_copy_subs_feature_mode

DESCRIPTION
  This local function converts CM Subs mode to GSTK Subs mode and copies
  it in to a variable

PARAMETER
  subs_feature_mode_ptr : variable in which converted result to be stored
  cm_subs_feature_mode  : CM subs mode

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_copy_subs_feature_mode(
  gstk_subs_feature_mode_enum_type *subs_feature_mode_ptr,
  const sys_subs_feature_t          cm_subs_feature_mode)
{
  if (subs_feature_mode_ptr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(cm_subs_feature_mode)
  {
    case SYS_SUBS_FEATURE_MODE_NORMAL:
      *subs_feature_mode_ptr = GSTK_SUBS_FEATURE_MODE_NORMAL;
      break;
    case SYS_SUBS_FEATURE_MODE_SVLTE:
      *subs_feature_mode_ptr = GSTK_SUBS_FEATURE_MODE_SVLTE;
      break;
    case SYS_SUBS_FEATURE_MODE_SGLTE:
      *subs_feature_mode_ptr = GSTK_SUBS_FEATURE_MODE_SGLTE;
      break;
    case SYS_SUBS_FEATURE_MODE_SRLTE:
      *subs_feature_mode_ptr = GSTK_SUBS_FEATURE_MODE_SRLTE;
      break;
    default:
      UIM_MSG_ERR_1("Invalid Subs mode from CM: 0x%x",
                    cm_subs_feature_mode);
      return GSTK_ERROR;
  } /* switch(cm_subs_feature_mode) */

  return GSTK_SUCCESS;
} /* gstk_copy_subs_feature_mode */

/*===========================================================================
FUNCTION gstk_build_cm_subs_mode_cmd

DESCRIPTION

  This function builds the CM Subs mode command and queues it to GSTK

PARAMETERS
  subs_info_ptr : Subscription Event Data

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_build_cm_subs_mode_cmd(
  const cm_subs_info_s_type *subs_info_ptr
)
{
  gstk_task_cmd_type               *task_cmd_ptr      = NULL;
  gstk_subs_feature_mode_enum_type  subs_feature_mode = GSTK_SUBS_FEATURE_MODE_NONE;
  gstk_slot_id_enum_type            slot              = GSTK_SLOT_1;

  GSTK_RETURN_IF_NULL_PTR(subs_info_ptr);

  if (gstk_io_ctrl (GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                    &subs_info_ptr->subscription_id,
                    &slot) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to slot for subscription_id 0x%x!",
                  subs_info_ptr->subscription_id);
    return;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();

  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.subs_change_ind.message_header.command_group =
      GSTK_CM_SUBS_CHANGE_IND;
    task_cmd_ptr->cmd.subs_change_ind.message_header.sim_slot_id = slot;
    if (GSTK_SUCCESS == (gstk_copy_subs_feature_mode(
                           &subs_feature_mode,
                           subs_info_ptr->sub_feature_mode)))
    {
      task_cmd_ptr->cmd.subs_change_ind.subs_feature_mode = subs_feature_mode;
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
    }
  }
} /* gstk_build_cm_subs_mode_cmd */

/*===========================================================================
FUNCTION gstk_process_session_changed_evt

DESCRIPTION
  Processes GSTK_MMGSDI_SESSION_CHANGED_EVT

PARAMETERS
  cmd_ptr of the MMGSDI event

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_session_changed_evt(gstk_cmd_type *cmd_ptr)
{
  int i = 0;
  sys_modem_as_id_e_type  sub_id  = SYS_MODEM_AS_ID_MAX;

  for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
  {
    if (gstk_shared_data.mmgsdi_sess_info[i].sess_id ==
        cmd_ptr->mmgsdi_evt.session_id)
    {
      if (!cmd_ptr->mmgsdi_evt.evt.session_changed.activated)
      {
        gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_ID_MAX;

        if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_PRI_SESSION) ||
           (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_SEC_SESSION) ||
           (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_TER_SESSION))
        {
          gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw = FALSE;
        }
        else if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_PRI_SESSION) ||
                (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_SEC_SESSION) ||
                (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_TER_SESSION))
        {
          gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x = FALSE;
        }

        /*
          Disable only the app specified
        */
        switch (cmd_ptr->mmgsdi_evt.evt.session_changed.app_info.app_data.app_type)
        {
          case MMGSDI_APP_SIM:
            gstk_curr_inst_ptr->apps_enabled &= ~GSTK_APP_SIM_BMSK;
            break;
          case MMGSDI_APP_RUIM:
            gstk_curr_inst_ptr->apps_enabled &= ~GSTK_APP_RUIM_BMSK;
            break;
          case MMGSDI_APP_USIM:
            gstk_curr_inst_ptr->apps_enabled &= ~GSTK_APP_USIM_BMSK;
            break;
          case MMGSDI_APP_CSIM:
            gstk_curr_inst_ptr->apps_enabled &= ~GSTK_APP_CSIM_BMSK;
            break;
          default:
            break;
        }
        if (GSTK_SLOT_1 ==
            (gstk_slot_id_enum_type)cmd_ptr->mmgsdi_evt.evt.session_changed.app_info.slot)
        {
          gstk_curr_inst_ptr->gstk_curr_tech_group = GSTK_ACCESS_TECH_NONE;
        }
      }
      else
      {
        gstk_shared_data.mmgsdi_sess_info[i].slot =
          (gstk_slot_id_enum_type)cmd_ptr->mmgsdi_evt.evt.session_changed.app_info.slot;

        switch (cmd_ptr->mmgsdi_evt.evt.session_changed.app_info.app_data.app_type)
        {
          case MMGSDI_APP_SIM:
            gstk_curr_inst_ptr->apps_enabled |= GSTK_APP_SIM_BMSK;
            break;
          case MMGSDI_APP_RUIM:
            gstk_curr_inst_ptr->apps_enabled |= GSTK_APP_RUIM_BMSK;
            break;
          case MMGSDI_APP_USIM:
            gstk_curr_inst_ptr->apps_enabled |= GSTK_APP_USIM_BMSK;
            break;
          case MMGSDI_APP_CSIM:
            gstk_curr_inst_ptr->apps_enabled |= GSTK_APP_CSIM_BMSK;
            break;
          default:
            break;
        }

        /* Retrieve the Sub ID corresponding to the MMGSDI Session and Slot */
        (void)gstk_util_get_sub_id_from_slot(
                gstk_shared_data.mmgsdi_sess_info[i].slot,
                &sub_id);

        /* Initalize subscription specific NV items */
        gstk_nv_init_sub_cache(
          GSTK_NV_INIT_CATEG_POWER_UP_INIT,
          gstk_shared_data.mmgsdi_sess_info[i].slot,
          sub_id);
        }
     }
  }
} /* gstk_process_session_changed_evt */
