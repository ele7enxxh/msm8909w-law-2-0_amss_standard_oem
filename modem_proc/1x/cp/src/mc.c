/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     M A I N   C O N T R O L   T A S K

GENERAL DESCRIPTION
  This module contains the initialization, startup and shutdown processing
  provided by the MC task.  This task initializes the complete DMSS software
  system and then it calls system determination.  System dfetermination in
  turn calls the analog and digital call processing subtasks.  Control
  returns to this module from system determination when powerdown or
  reset are to be done.

EXTERNALIZED FUNCTIONS
  mc_task
    Entrance procedure to the Main Control task.
  mc_cmd
    Procedure to send a command to Main Control from other tasks
  mc_task_start
    Procedure for tasks to acknowledge start up from MC.
  mc_task_ack
    Procedure for task to acknowledge from MC.
  mc_task_stop
    Procedure for task to do task stop processing from MC.
  mc_task_offline
    Procedure for task to do task offline processing from MC.
  mc_cdma_activate_protocol
    Command the MC task to activate the CDMA protocol.
  mc_cdma_deactivate_protocol
    Command the MC task to deactivate the CDMA protocol.
  mc_cdma_ph_status_chgd_cmd
    Inform the CDMA protocol of the phone status change.
  mc_cdma_generic_prot_cmd
    Command the CDMA protocol to process the generic command.
  mc_amps_activate_protocol
    Command the MC task to activate the AMPS protocol.
  mc_amps_deactivate_protocol
    Command the MC task to deactivate the AMPS protocol.
  mc_amps_ph_status_chgd_cmd
    Inform the AMPS protocol of the phone status change.
  mc_amps_generic_prot_cmd
    Command the AMPS protocol to process the generic command.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  This task is the first task created in the DMSS.  It is created from
  the main module (using the REX task initialization service) after boot
  processing is completed.  No other tasks or drivers have been created
  or started when this task is started.

  Copyright (c) 1990 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mc.c_v   1.53   08 Oct 2002 12:21:26   hexinw  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mc.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/05/16   sjo     Fixed compilation errors
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
07/25/16   sjo     Added more commands to mc_dbg_cmd_q.
06/28/16   sjo     Debug buffer enhancements. Added more confirmation rpts to
                   mmoc_cnf_dbg_buf. Added mc_dbg_msgr_q debug buffer also.
06/13/16   agh     Support QSH framework to collect periodic/event metrics
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
05/09/16   ppr     Chgs to reduce F3 logging 
12/11/15   ppr     Chgs to enable 1xSRLTE MT Page Acc HO feature by default
09/30/15   ppr     New EFS Item chgs for 1xSRLTE MT Page Acc HO feature
03/28/16   ppr     1x2GTA Feature changes
03/24/16   sjo     Read the carrier specific efs item during MCFG refresh.
02/17/16   agh     Handle NAS_EMM_ATTACH_COMPLETE_IND in inactive state to do 
                   forced powerup reg in CSFB mode
02/17/16   agh     Optimization to add previous serving channel to NL rxed on
                   current serving channel after handoff if not already prese
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB
12/08/15   sjo     Changes to fix the KW errors.
12/01/15   agh     Enable 1X Split overheads feature by default
11/23/15   ppr     Chgs for not to wait for deact_done rpt from SRCH, if it
                   returns error status for deact_f cmd for CSFB_deact_f
10/21/15   sjo     Support for FR-29020: Power Saving Mode w/o NW coordination
09/16/15   agh     Disable Split overheads feature by default
06/19/15   agh     Added Diag cmd to modify Ovhds Refresh time for testing
09/09/15   agh     Changes to support Voice Adapter based Audio architecture
07/13/15   eye     Added CSFB check for demod ch0 FW update.
06/11/15   sjo     Enhance debugging interface between MMOC and CP
02/27/15   ppr     Timer based mech to clean up bs_info[] heap mem
02/02/15   ppr     Chgs for new MC_CDMA_STANDBY_PREF_CHGD_REQ cmd from MMOC
01/23/15   agh     Enhance debugging interface between SRCH and CP
12/08/14   ppr     Enable CSFB_CGI_FEATURE/C2K_RESEL_SPLMN_SUPPORT by default
12/02/14   agh     Support for new customer specific voice service option
11/28/14   ppr     Changes to support new cmd from MMODE, to abort  ongoing 
                   LTE_to_1X REDIR Attempt
11/18/14   agh     Enable Split ovhds and Ovhds Caching features by default
11/12/14   agh     Handle REJECT SS action from SD
11/11/14   gga     Changes to have Efficient Piecewise Aggregation of 
                   Overhead Messages 
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
10/30/14   ppr     Fix not to send Auto_deact ind to MMODE in FTM mode
09/03/14   agh     Fixed p_rev synchronization issues betwen 1X-CP and DS
08/23/14   gga     Avoid sending offline indication to MMODE if NV read fails 
                   during power-up.
08/08/14   as      Fix IRAT for Dual Receive
08/11/14   ppr     Added new zone related critical section variable
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
06/19/14   agh     Initialize mcc_band_table after we get PROT_ACT from MMode
06/09/14   ppr     Added payload to REDIR_CNF_CMD
05/29/14   agh     Allocate memory for mcctcho_ho_data_ptr in FTM Mode
05/23/14   ppr     Avoid unnecessary dynamic mem allocation in FTM Mode
05/06/14   ppr     Reset the SRVCC flag for CSFB mode
05/06/14   gga     1X_CP F3 redistribution
04/18/14   agh     Fixed Klocwork errors
03/20/14   gga     Fix to make sure that the default value of Rel-8 Call 
                   Setup Optimization NV is set to TRUE.
03/19/14   gga     Added handling for 1x-NV callback registration
03/19/14   ppr     SRVCC Feature Changes
03/18/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports 
                   C2K-LTE Cell Reselection
03/14/14   gga     Fixed the bugs which came due to R8 CSFB Call setup 
                   optimization
03/04/14   pap     Changes to remove compilation warnings.
02/28/14   bb      Changes to do forceful registration when 
                   NAS_EMM_IRAT_IMSI_ATTACH_IND his received without
                   CONTEXT loss indication
02/25/14   agh     Moving protocol specific structures from mobile.c/h to 1x CP
02/06/14   gga     Fix to make sure that the default value of SMS_OVER_S102
                   NV is set to FALSE.
01/13/14   agh     FR 15489: Removed warning in mc_process_cdma_ph_stat_cmd
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
12/09/13   gga     Added Handling for LTE Abort HO Request
12/02/13   gga     CSFB: Added a support to reject the MT SMS if 
                   SMS_OVER_S102 NV is not defined
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
11/20/13   ppr     Added support to handle the GSRDM avoidance timer expiry 
                   while MC is in inactive state
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
09/06/13   ssh     Added support to handle the resel failure.
08/28/13   cjb     Removed TIMETEST interaction
08/05/13   gga     Feature cleanup
07/29/13   eye     Switch to secure random number generation api.
07/23/13   srk     Use TCB_PTR MACRO instead of rex_self() or tcb_ptr to make
                   the code compatible with RCINIT.
07/19/13   fg      Reorganize nv read/write/init functions 
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/09/13   ppr     Added critical section for MC memory allocation and freeing
05/07/13   ppr     Added critical section for freeeing bs_info/cur_bs_ptr
05/06/13   gga     Dynamic MSGR memory allocation changes
05/02/13   ppr     SV mode - Reject 1x voice page while VOLTE call is active
03/25/13   gga     Backing out the previous checked-in changes
03/20/13   ppr     Additional checks to catch dynamic memory issues
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
03/11/13   nee     Added defensive checks.
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
03/01/13   cjb     Removed redundant MMGSDI registration from MC task.
01/30/13   gga     Changes to stop advertisings SO73 if 1xA and SO73 NV
                   is disabled
01/28/13   gga     Klocwork Reviews Phase 2
01/25/13   nee     Removed one check mc_nv_data_buf.scm != station_class_mark)
                   for removing unnecsary write to NV_SCM_I
01/24/13   ppr     Moving EFS related reads to mc_init_after_task_start() and
                   use mccreg_powerup_mode_online() API for PROT_OPRT_MODE_ONLINE
01/10/13   ppr     Avoid NV# writes for PROT_OPRT_MODE_LPM case
01/07/13   gga     Forced C2K params req after getting the SIB8 after
                   call end.
01/03/13   nee     Replacement of memcpy() to memscpy() Phase 2
12/28/12   gga     Klocwork Reviews
12/28/12   ssh     Removed err_fatal for NV write failure during powerup.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
10/19/12   ppr     Added defensive checks
10/16/12   ppr     New API to get the mc_queue_type allocated_buf_cnt
10/11/12   ppr     Fix for "MC_MO_DBM_F" cmd processing while 1x is not active
09/27/12   jtm     Wrap DTMF queue init with FEATURE_1X_CP_MEM_OPT.
09/26/12   jj      Fix compilation issues when FEATURE_LTE_TO_1X is disabled.
09/24/12   ppr     Added NULL check before allocating dynamic memory
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/04/12   jj      Customer feature to allow OTASP only when BIP on LTE/HDR is
                   not in progress.
06/28/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
06/22/12   ppr     FR-2454 NV Item support for OTASP_MAX_UNLOCK_ATTEMPTS
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/15/12   ppr     Feature Cleanup
05/25/12   ppr     Optimized efs_write call in mc_efs_init()
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
04/13/12   ppr     Reverting changes related to replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
                   so Rollback to previous Revision
04/10/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
04/04/12   jj      Fix to correct the EFS read size T_NO_MAX_VOICE NV item.
03/12/12   jj      CSFB: CSFB NV Items support.
03/12/12   ppr     Fix for unable to update the NV items NV_MOB_FIRM_REV_I and NV_MOB_MODEL_I
03/07/12   ppr     Moving cdma.jcdma_support.service global var intialization
                   from mcc_powerup_load() to mc_init_after_task_start()
03/07/12   srk     Replaced MODEM_LEGACY_STARTED with RCINIT_RCEVT_INITGROUPS.
03/06/12   srk     RCInit taks should wait for MM_GSDI to continue.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/13/12   srk     Moved init fucntions after task start.
02/07/12   srk     Fixed compiler warnings.
02/04/12   ppr     Feature Cleanup
01/23/12   srk     Feature Cleanup
01/19/12   ppr     KDDI req FR1665 - Reject incoming SO35/36 call
                   if it is not supported
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/13/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
01/01/12   ssh     Added critical section for mc_cmd_q.
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
12/20/11   srk     Added RCInit Framework.
12/13/11   srk     Feature clean up: Mainlined FEATURE_1X_WDOG_DYNAMIC
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
11/17/11   jj      Eliminate references to other sub-systems' private
                   functions and header files.
10/20/11   jj      CSFB: Added support to use SIB8 neighbors to scan for
                   Redir/Resel.
10/12/11   jj      Fix to flush MSGR queue when we are CDMA activated.
09/09/11   ppr     Replaced AEE APIs with the standard C string APIs.
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
08/26/11   cjb     1x->LTE: Register for LTE_RRC_1X_ABORT_RESEL_RSP message.
08/26/11   cjb     1X to LTE reselection support - part 2.
08/23/11   jj      CSFB: Added support to listen Attach indication from NAS.
08/02/11   jtm     Fixed compiler error.
06/09/11   ag      Changes to dynamically allocate/free large size variable
                   ss_act_param in mc_set_digital_acquisition.
06/06/11   ssh     Removed the direct access of MC_TCB wait fields directly.
06/01/11   jj      CSIM: Added changes to remove blocked client id registration
                   during task init time and move session id reg to a later
                   time.
05/18/11   ag      Changes for storing EFS filenames one per line in conf file.
05/12/11   vks     Remove rfnv.h include.
05/12/11   jj      CSFB: Fix to deactivate lower layer if there is a Redirection
                   failure.
05/10/11   jj      CSFB: Added support for sending Auto Deactivation Indication
                   and Call Failure Indication upon Redirection failure for
                   call context.
05/03/11   jj      CSFB: Support for returning to CSFB mode if there is ECAM or
                   Redir failure..
04/29/11   ag      Support for NV based control for 1xAdvanced.
04/29/11   ag      Fixed the issue of mis-reading EFS files in mc_efs_get().
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/13/11   ssh     Added support to do the power-up reg in case of ignoring
                   a page due to DDTM settings.
03/29/11   ag      Removed ERR_FATAL for errors while creating EFS config file.
03/24/11   jj      CSFB: Added changes to gracefully exit from CSFB state machine
                   without having to consult System determination.
03/04/11   jj      CSFB: Updated to set SD's redir/resel context using a CSFB
                   function.
03/03/11   jj      CSFB: Support for forced registration in case of context loss.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/16/11   ssh     Mainlining JCDMA specific requirements.
02/10/11   ssh     Added a safety check to ensure DOG_DYNAMIC_API is defined.
02/04/11   jtm     Moved ftm_system_determination call within onex_ns_system_determination.
02/01/11   ag      Defined the critical section for MC task.
01/31/11   jtm     Replace ftm_mode_type use with onex_nonsig_mode_type.
01/28/11   jtm     Additional FTM decoupling.
01/28/11   ag      Featurized references to RF module.
01/28/11   ag      Conditionally use legacy FTM or 1x NonSig.
01/19/11   jtm     Decouple FTM and 1X.
01/13/11   ag      Changed setting of command status before cmd_done.
01/13/11   ag      Moved the initialized of SOs after MC task starts because
                   MC accesses NV for SO initialization.
01/07/11   ag      Fix to initialize cdma.is_cdma_subs_avail before AUTH
                   is initialized.
01/04/11   ag      Removed unused symbol memtest_test_voc_memory().
12/20/10   jj      CSFB: Changed the sequence of deactivating SRCH and reporting
                   deactivation to upper layers.
12/17/10   ssh     Changed the watchdog timout value for MC task to
                   DOG_DEFAULT_TIMEOUT.
12/15/10   jj      CSFB:Added support for power_down exit from CSFB mode.
12/14/10   jj      CSFB: updated with few fixes to parse MO HO correctly.
12/13/10   jj      Indentation Update.
12/10/10   ag      Lint Fixes.
12/05/10   jj      CSFB:Added support for Rel 8 Rel 9 CSFB.
11/16/10   ag      Added support for direct EFS access.
11/15/10   jtm     Initial SU API split for CP.
11/02/10   ag      Fix to initialize WDOG related variables after task starts.
10/21/10   azh     Fixed KW and lint warnings.
10/18/10   jtm     Lint Fixes.
10/12/10   ag      Removed usage of obsolete CLK APIs.
09/20/10   jj      CSFB:Added degug logs.
09/15/10   ag      Added support for dynamic DOG.
09/02/10   ag      Removed unnecessary parenthesis.
07/19/10   ag      Replaced FEATURE_8K_STUFF_TO_BE_PURGED with
                   FEATURE_MODEM_1X_MVS_API_GEN2 for MVS API selection.
07/15/10   ssh     Fixed Compiler Warnings.
07/12/10   jj      CSFB change: Used q_delete to remove the items from
                   msgr queue.
06/14/10   jj      Added fix to process commands in the cmd_q and msgr_q
                   irrespective of the SIG set or not.
05/19/10   jj      CSIM change : mmgsdi header file include, included
                   mmgsdisessionlib_v.h
05/11/10   jj      CSIM change :Added support to get card's application in use.
05/12/10   ssh     Cleaned up the lint issues.
04/30/10   jj      CSFB change: Added stub files includes to enable offtaget
                   testing.
04/28/10   jj      CSFB change: IRAT core services code update.
04/26/10   jj      CSFB change: IRAT core services code update.
04/14/10   jj      Removed card level sanity check for the CDMA subs availability.
04/23/10   ag      Support for 1x Modem statistical report.
04/22/10   jj      CSFB change: update for setting MSGR's offset.
03/25/10   ag      Removed usage of SD_SS_ACT_ACQ_LTE & SD_SS_ACT_GET_NET_GWL.
02/22/10   jj      CSFB update.
02/19/10   jj      CSFB : updated naming convention for confirmation callbacks.
02/19/10   jj      CSFB : updated naming convention for Redir/Resel error codes.
01/26/10   ssh     Added the support for blocking non-voice pages during
                   emergency callback mode.
02/05/10   ag      Added handling of SD returned actions SD_SS_ACT_ACQ_GWL,
                   SD_SS_ACT_ACQ_LTE and SD_SS_ACT_GET_NET_GWL.
01/29/10   ss      update the last entry reason for OFFLINE_CDMA
                   operating mode.
01/19/10   jj      Rectified a Lint warning.
01/15/10   jj      CSFB Core Services Implementation.
01/10/10   jj      Added support for CSIM-P2 , migration to MMGSDI APIs.
11/10/09   jj      Moved db_init() into mc_init_after_task_start().
11/02/09   jtm     Fixed compiler warnings.
09/09/09   adw     Removed unnecessary commented out _v.h includes.
08/22/09   jtm     Lint fixes.
08/03/09   adw     Initialize the switches DAL during mc task init.
07/24/09   jtm     Lint fixes.
07/17/09   jj      Featurized writing of NV_SLOT_CYCLE_INDEX_I only when the
                   phone is operating in NV mode or RUIM mode with RTRE being
                   NV or when FEATURE_UIM_RUIM_SUPPORT_SCI is inactive.
07/02/09   pk      Modifying RF calls for SVDO support
06/30/09   ag      Diag decoupling changes for CMI
06/24/09   pk      Removing function calls of rfm_init_bbrx_sleep
05/03/09   jj      CMI Phase-II update.
04/29/09   jj      Replaced assert.h with amssassert.h
04/13/09   adw     Added tdso_v.h include for CMI.
04/03/09   jj      Removed Target specific feaaturization as part of
                   CMI effort.
03/20/09   jtm     Removed unnecessary include of mctrans.h.
03/13/09   adw     Lint fixes.
08/04/08   bb      Code changes to move Service Option initialization after MC task has
                   started for 8K targets
06/09/08   ag      Fixed some watermarking issues.
05/14/08   ag      Watermarking for trans_queue.
01/08/08   ag      Mainlined FEATURE_LPM
10/31/07   pg      Set deact_reason_for_sleep to LPM mode for power save mode.
08/10/07   yll     If the deactivation reason is for LPM, don't leave CDMA
                   sleep mode so that MS can go to sleep.
08/03/07   pg      Added support for extended REXL4 signal under
                   FEATURE_REX_SIGS_EXT.
05/11/07   pg      More Lint Cleanup.
02/23/07   sb      Added support for new exit reason.
02/15/07   sb      Update last_entry for VOIP-1X handoffs.
01/30/07   sb      Fixed compilation error in mc_get_csna_message_type.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/25/06   fc      Changes to not use entry reason to indicate TC to Idle
                   transition.
09/21/06   fh      AKA/AES: added a new access state
                     MCC_ACCESS_STOPPED_MI_AES_RESYNC -- access stopped for
                     AKA/AES crypto-sync procedure
08/01/06   fc      Merged the following from MSMSHARED:
08/01/06   an      Made klokwork fixes.
07/10/06   fc      Changed mcc_set_digital_acquistion prototype.
07/06/06   fc      Renamed MCC_LOST_RF_RESOURCE to MCC_SRCH_REQUEST_SYS_RESTART.
06/23/06   pg      Added support for RPC.
06/02/06   fc      Cleanup umts interface.
05/19/06   an      Added check to detect if MC is waiting for the search task
04/26/06   an      Fixed Lint Errors
03/03/06   fc      Changed to print debug message instead of error fatal if
                   mmoc sanity timer expired in system access or traffic
                   state.
01/20/06   fc      Merged the following:
           fh      Added support for AKA and MI.
10/10/05   fh      Removed including of enc.h, and deint.h
10/03/05   an      Fixed lint errors
08/25/05   fc      Changed mc_cdma_prot_activate to send SRCH_CDMA_F only if
                   activate reason is not FTM.
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of rxcmc.h
05/11/05   fc      Moved tdso_init_so_cfg to mc_init_after_task_start.
04/25/05   fc      Merged the following from MSMSHARED:
04/20/05   fh      In function mc_init_ran(), set the random seed to be 1 when
                    ESN is 0.
04/19/05   fc      Merged the following from MSMSHARED:
04/13/05   an       Mainlined FEATURE_SRCH_INACTIVE_STATE
03/24/05   an       Replaced ds707.h by ds707_extif.h
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following change from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/11/05   fc      Merged the following change from MSMSHARED:
03/10/05   fh       Inside the function mc_cdma_prot_activate, changed to pass
                    PROT_STATE_ONLINE_CDMA or PROT_STATE_ONLINE_DED_MEAS to the
                    function call of mmoc_rpt_prot_auto_deactd_ind according to
                    different act_reasons
03/01/05   fc      Merged the following change from MSMSHARED:
           ka      Subscription info is only reloaded if CM informed there is
                   a subscription change.
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
12/10/04   sb      Fixed lint errors.
11/04/04   fc      Fixed lint errors.
10/29/04   pg      If 1X subscription is not available, use default values for
                   subscription items.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/30/04   sb      Fixed lint warning.
09/27/04   va      Merged the following from RELB/C
08/11/04   dna      Get RF lock for mc_meas_ded_state, for MSM6500 4.0P4 only.
08/03/04   ph       Featurized access of NV_SLOT_CYCLE_INDEX_I to the exclusion
                    of FEATURE_UIM_RUIM_SUPPORT_SCI
07/28/04   fc       Fixed lint errors.
07/21/04   sj       Added support for umts to 1x handover.
07/12/04   pg       Perform registration only if subscrition (RUIM card is
                    inserted for RUIM only build) is available.
06/30/04   dna      Changes for FEATURE_SRCH_INACTIVE_STATE.
                    This feature changes the MC<->SRCH interface for activating
                    and deactivating SRCH, and also delays when
                    mmoc_rpt_prot_auto_deactd_ind() is called until after
                    SRCH has really released the RF and other resources.
08/17/04   az      Release D Negative SCI changes
08/12/04   va      Removed unneccesary include files.
08/09/04   fc      Service negotiation changes for Release C and D.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/26/04   fc      Included mccsyncid.
05/28/04   bkm     Merged the following from IS2000_REL_B_C
  05/17/04   va      Merged the following from MSMSHARED:
    04/20/04   dyc      Updates to support removal of DFM VU dependency.
    03/11/04   yll      Support the KDDI requirement to ignore async data and
                        async fax 1x pages during HDR session.
    03/05/04   bkm     Lint cleanup.
03/26/04   bkm     Merged the following from IS2000_REL_B_C
  03/12/04   fc      Added support for Release C fast call setup.
  03/04/04   az      Added support for new transaction engine
  02/27/04   yll/br  Support GPS Out of Service operation, notify Search that
                     MC enters AMPS.
02/10/04   az      Added a funtion call to initialize action handlers for the
                   new transaction engine.
01/08/04   yll     Added the DF init in mc_init_after_task_start(), after the
                   MOB_CAI_REV is init'ed.
01/07/04   ph      Removed spurious #endif
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
11/13/03   yll     Moved the resetting of cdma.call_orig outside the function
                   mcc_send_orig_failure().
11/12/03   sj      Added the missing exit reason in mc_online_digital()
                   MCC_ACCESS_FAILED_SYS_LOST.
11/06/03   yll     Enabling the RF receiver chain in mc_meas_ded_state().
11/03/03   yll     Dynamic P_REV: default the P_REV to Rel 0 in Rel A build.
10/23/03   yll     Fixed the problem in non multimode architecture in
                   mc_meas_ded_state() that a command is left in mc_cmd_q
                   while not processing Search's meas report.
10/22/03   is      Includes pdsmtask.h if FEATURE_PDAPI enabled.
10/21/03   yll     mc_get_rf_items_from_nv() no longer exists when FEATURE_
                   MULTI_RX_CHAIN is defined.
10/07/03   yll     When MC doesn't responde to MMoC commands, check if MC
                   exclusively waits on Search's wakeup, let Search know
                   it's stuck.
09/11/03   yll     Added the featurization for ACP include files.
08/27/03   sj      Changed mc_process_cmd() to buffer unprocessed commands
                   until safe limit is reached.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/12/03   sj      Updates for placing srch in standby only in power save
                   and lpm. 1x protocol hw init moved out of mc.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/31/03   yll     Removed TEMP_MC from HDR related feature.
07/30/03   yll     Merged in the following changes:
           dna     Added support for HDR.
07/21/03   sj      Place RF in 1x_mode before activating CDMA or AMPS.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/02/03   bkm     Lint cleanup.
06/05/03   wd      Added case to handle PROT_OPRT_MODE_FTM in
                   mc_process_cdma_ph_stat_cmd.
06/05/03   va      Added trex initialization under the MM architecture path
                   too
04/15/03   jqi     Revised the change for MMRF API for non runtime switching
                   target. (cougar target)
                   Runtime switching target (Leopard)- RF initialization
                   whenever entering CDMA mode.
                   None Runtime switching target (Cougar)- RF initialization
                   only did once during power up.
03/28/03   va      Added trex_init_blocks call to the MM path as well.
03/04/03   atien   Changes to resolve naming conflict with MMGPS defined types
                   in mmgps_types.h
                   1. Replace nf.h, cd.h (pe.lib's internal include files) with pe_task.h
                   2. Replace pb.h with pb_task.h
03/05/03   jqi     Added MMRF API support with Multimode arch.
                   rf_init_delayed() will be called for all multimode target.
02/13/03   jqi     Remove set ftm_mode to DMSS_MODE in mc_init_before_task_start().
01/27/03   sj      Removed err_fatal while handling pref_sys & nam_sel in
                   offline states.
                   When subscripton load from NV/RUIM fails for CDMA or AMPS,
                   transition to offline state only if the oprt_mode is not
                   currently offline.
01/20/03   jqi     Added rf_init_bbrx_sleep() after rf_init_delayed() for MSM6300
                   Target.
                   Updated rf_init_bbrx_sleep() API in mc_online_digital.
11/19/02   ro/bkm  Added initialization for 'leds' for voc, graph, vs, fs tasks.
11/05/02   va      Added support for dynamic p_rev change.
11/04/02   bkm     Corrected featurization around MC_SILENT_REDIAL_F in
                   mc_gps_state_process_mc_command().
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/14/02   sb      Merged in changes from MSM6100.
10/08/02   hxw     Changed FEATURE_GPSONE_MSBASED to FEATURE_GPSONE_OOS.
10/07/02   ph      Moved mccreg_init() to after task initialization.
10/04/02   sj      Updates from Multimode arch. FTM & PLT testing.
10/04/02   lh      Lint cleanup, 2nd round.
10/01/02   az      Mainlined FEATURE_IS2000, FEATURE_COMPLETE_SNM, T_MSM2P
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   fc      Added exit reason MCC_ACCESS_FAILED_GSRM.
09/19/02   lh      Lint clean up.
09/12/02   sj      Updates from Multimode FTM testing.
09/04/02   sj      Featurized AMPS handling under Multimode architecture.
08/19/02   jqi     Undefined mc_start_xxxx_tasks() when mm arch is defined.
08/12/02   sj      Fixed merge errors from multimode architecture changes.
08/07/02   jqi     Removed a tempoary feature and redundant code.
08/05/02   HQ      Added FEATURE_CDSMS.
07/29/02   jqi     Undefine mc_get_esn_me() when multimode arch is defined.
07/25/02   jqi     Replace FEATURE_TMC_TASK with FEATURE_MULTIMODE_ARCH
07/23/02   sj      Moved SRCH to standby if CDMA/AMPS protocol is not active.
07/18/02   jqi     Undefine mc_init_uim_items() with multimode architecture.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/02/02   jqi     Remove support in mc_device_init
07/01/02   yll     Added suupport for MC Queue Watermark
06/21/02   ph      changed cmmc.h to cmxll.h
06/07/02   sj      Added CP support for multimode architecture.
06/06/02   jqi     Added support for FEATURE_MULTIMODE.
                   1.Added support in mc_device_init()
                   2.Removed HW initialization and task opertion such as creat,
                     start, stop and offline from mc_task to tmc_task.
                   3.Added mc_init(), mc_init_before_task_start(),
                     mc_init_after_task_start() function
                   Added task operation functions,such as mc_task_ack(),
                   mc_task_stop(), mc_task_offline() if TMC task is not defined.
05/23/02   ht      Added chg_sleep_init() under FEATURE_BATTERY_CHARGER.
05/06/02   yll     Changed Broadcast SMS function names.
04/17/02   cah     Added OFFLINE notification for NF and CD tasks.
04/16/02   dd      Feature-ized vectorOne device dependent code.
04/09/02   yll     Changed handling of MC_BCSMS_ENABLE_F to a function call.
04/02/02   dna     Removed "HDR Hybrid" bit from phone state (for logging)
04/02/02   hxw     Changed CM_GPS_MODE_ABORT_F to CM_GPS_MODE_EXIT_F.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/22/02   fc      Changes to use static buffer to store selected system info.
03/20/02   ht      Removed direct call to sbi driver and replace it with call to rf zif lib.
03/18/02   ht      Added code to support FEATURE_SOFTWARE_PROFILE.
03/14/02   sh      Added processing for MC_ONLINE_ANALOG_F
03/14/02   ht      Featurized I2C driver support for MSM6050.
03/09/02   st      Added support for the Graphics task and I2C driver.
03/11/02   ht      Corrected the target name T_MSM6000 and T_MSM6050.
03/11/02   fc      Changes to make mode controller back to offline state if
                   nam selection failed.
03/09/02   lh      Merged from CP119.
  03/04/02   ht      Include clkrgm_5500.h to fix compiler warnings.
  03/04/02   dna     Fixed mc_state() for HDR hybrid operation.
  03/03/02   sh      Removed MC_STOP_FWD_PREF_SYS_CMD_F
  02/18/02   abh     Modified the demod download function prototype to
                     selectively enable the firmware download verification
                     using test bus.
  02/12/02   abh     Updates in the functionality to perform GPS operation
                     while MS is in Acquisition State (for performing
                     multi-visit operation).
  02/13/02   ts      Removed the initialization of the UIM queues from within the
                    FEATURE_AUTH ifdef.
  02/11/02   mpa     Using cm_get_cur_hybr_status instead of sdss_is_hybrid.
  02/08/02   sq      Externed function Error_Fatal for MSM5500.
03/06/02   jrp     Fixed merge error.
02/13/02   jrp     Merged the following from common archive:
                   02/07/02   sh      Added srch support for HDR only mode
                   02/06/02   abh     Added code to go into GPS state when Phone is in Acquisition state.
                   02/06/02   ssw     Changed enabling SBI to account for ZIF
                   02/06/02   sh      Added hybird operation support
                   01/16/02   ak      Added FEATURE_VECTORONE
                   01/15/02   ht      Added ONC Socket Test Applications task and
                                      ONC RPC SERVICES task under FEATURE_ONCRPC.
                   01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                                        Includes a function name change:
                                          srch_dmod_reset -> srch_util_dmod_reset
                   01/07/02   ht      Merged changes for MSM6000 and MSM6050.(clang)
                                      Added support for the PM60X0 battery charger code.(rmd)
                   12/06/01   sq      The MSM5500 software will not support old MSM REV before 1.3
                   12/02/01   dna     Merged in changes from a branch being used for MSM5500
                                      development.  The only non-featurized change is moving
                                      mc_init_ran().
                              sh      Added processings for HDRMC_EXIT_RESET
                              dna     Move mc_int_ran() call to before we start HDR app. tasks.
                              st      Merge HDR changes to mc_system_offline, mc_system_powedown
                                      and mc_online_hdr_state.
                   11/13/01   sq      Switch MCLK after demod download
01/09/01   va      Removed FEATURE_IS2000_REL_A_SVD from around cmtask.h
                   include.
12/17/01   va      Conditionally include cmtask.h
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/13/01   lh      Changed ACP_OFFLINE_ANALOG to MC_OFFLINE_ANALOG_STATE in
                     mc_analog_offline_state().
10/02/01   jrp     Merge from MSM_CP.05.00.96
08/21/01   fc      Added support for exit reasons MCC_ENCRYPTION_FAILED and
                   MCC_ACCT_BLOCKED.
08/16/01   kk      Merge from MSM_CP.05.00.89.
                   08/07/01   fc      Added #ifdef FEATURE_ACP in
                   mc_online_digital_state (SD2.0 only).
08/08/01   yll     Merged the following from the MSM archive:
                   Include timetest.h when TIMETEST is defined.
                   Moved the STORE_ESN_ME command to UI after CHV is verified
                   and added an interface to export the esn in cdma structure.
                   Changed TIMETEST ID's to #def's (from timetest.h)
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/18/01   ck      Added offline signalling to UIM task.
                   06/13/01   vlc     Added an #ifndef FEATURE_PLT around mcc_subtask calls as
                                      this function is not visible in PLT builds.
                   06/11/01   fc      Added mc_update_last_cdma_entry to support offline for
                                      non-MC (SD20 only).
                   06/11/01   rp      Added TIMETEST support for USB.
                   06/08/01   ck      Added support to vote on the power control of UIM.
                                      Flushed NVRUIM cache before reading items from NV on a NAM
                                      change/ OTASP commit when using SD2.0.
                   06/06/01   bkm     Added TIMETEST support for Bluetooth.
                   05/31/01   ht      (JH) Removed the "static" declaration to save RAM.
                   05/26/01   ry      Added new Bluetooth LPOS task for FEATURE_BT_LPOS
                   05/25/01   ts      Added new UIM server task and API to new UIM server.
                   05/23/01   dna     Fix HDR_OFFLINE mode so we can write NV items while in HDR
05/17/01   ph      Merged in from MSM_CP.05.00.76
                   05/02/01   kss     Include hdrmdspmcr.h.
                   05/01/01   dna     HDRMC_START_HDR_SIG -> TASK_START_SIG
                   04/12/01   dna     Merged in support for HDR
                   04/12/01   fc      Moved events logging data structures/functions to mclog.c.
                                      Added offline signalling to TDSO task.
                   04/10/01   fc      Added support of FTM and PLT for system determination 2.0.
03/27/01   kk      Merged mainline changes
                   03/08/01   va      Merged in some fixed for FEATURE_PLT
                   03/06/01   fc      Added support of events report: Operation mode change.02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
02/08/01   ks      Added TDSO task initialization.
02/05/01   yll     Added Security Services Task.
01/23/01   cah     Rid T_SLEEP define checks and mainline code.
01/19/01   va      Removed clk_regime_set1_enable call for MSM5100.
01/11/01   dwp     Added support for FEATURE_RUNTIME_DEVMAP (RDM).
01/11/01   va      Added support for MSM5100.
12/07/00   mai     Changed feature name for PDSMSCLIENT and removed extraneous
                   #endif.
12/04/00   yll     Added support for task PDSMSCLIENT for gpsOne.
11/15/00   ck      Changed the AUTH_RAND_SEED_F command to AUTH_BS_CHAL_F cmd.
                   Added the function mc_queue_auth_rpt to handle reports from
                   Auth task.
                   Changed the number of Auth Rpt buffers.
                   Added support for STORE_ESN_ME command in the function
                   mc_init_ruim_items().
11/08/00   ts      Added the function mc_init_ruim_items() right after starting the
                   AUTH and NV tasks in mc_start_service_tasks() to avoid deadlock.
11/16/00   bgc     Updated support for PLT and FTM.
11/02/00   lcc     Support for FEATURE_COMPLETE_SNM: initialization of SOs.
10/27/00   hrk     Using a generic function name - msm_device_init() to make
                   MSM specific initializations.
10/18/00   jkl     Moved SND and VS from application to service tasks.
10/13/00   bgc     Added HS task for FTM.
09/20/00   jq      Cleaned all lint errors.
09/14/00   ry      Removed the Bluetooth Audio Gateway Demo task
09/11/00   sr      Added call to vocoder instruction memory test under
                   FEATURE_VOC_IRAM_TEST
09/08/00   ht      Change the Bluetooth task shutdown order.
09/07/00   ry      Added Bluetooth to start_service_tasks and system_powerdown
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
09/06/00   ry      Renamed FEATURE_BT_AG_HS_DEMO to FEATURE_BT_HS
08/29/00   yll/dlb Added SFAT task for MMC support.
08/23/00   ry      Renamed BT_TEST_RC_2_SIO to FEATURE_BT_TEST_RC_2_SIO
08/22/00   ro      Added QDSP Services Task and TASK_STOP to Vocoder task.
08/21/00   ry      Added Bluetooth support
08/02/00   yll     Added sending TASK_STOP_SIG for PDSM task.
08/01/00   va      Added support for initializing boothw clk callback.
07/10/00   ks      Added support for Simple TDSO, Calling tdso_int_so_cfg()
07/05/00   ts      Moved the ruim_dev_init call past the msm5000_dev_init call.
06/19/00   yll     Added Support for Creating a new task "PDSM" under
                   FEATURE_GPSONE.
06/22/00   va      Support for caching the pref RC NV item.
06/14/00   ks      Moved the Ifdef checking for FEATURE_PLT so that CM task is
                   used when PLT is defined.
05/26/00   ck      Added the issuing of the command AUTH_RAND_SEED_F to indi-
                   cate to RUIM that the pseudo random number generator has
                   been seeded in the function mc_init_ran().
                   AUTH task is started and powered down if FEATURE_AUTH or
                   FEATURE_RUIM is defined or both.
                   AUTH task is started right after EFS and before NV task in
                   the start_service_tasks routine if FEATURE_RUIM is defined.
                   If RUIM is not defined, the sequence of tasks start up
                   remains the same and AUTH task is started as an application
                   task.
04/24/00   cah     Added USBDC support (rp).  TLMM support. (rp/jc)
                                   Moved EFS task start to end.  (rp)
04/11/00   jwh     Moved mc_sdevmap_from_nv() to mc_start_service_tasks()
                   as it needs to be called AFTER nv task is running, but
                   before diag or ds tasks are started.
04/09/00   jwh     Added mc_sdevmap_from_nv() to support
                   FEATURE_SDEVMAP_MENU_ITEM_NV.
03/12/00   ry      Added the support of non-MSM5000 target
02/03/00   lcc     Removed inclusion of mccap.h.
01/26/00   va      Merged changes from GV40401
11/10/99   kmp     Removed the #ifdef FEATURE_DS around dsm_init() in
                   mc_device_init().
11/04/99   kmp     Merged in the 3 changes listed below:
11/03/99   mk      Not using msm3000_2300_device_init() initialization code
                   for MSM3100.
11/02/99   mk      Moved misc MSM3100 pin initialization.
11/01/99   jc      Added PM1000 charger support.
01/14/99   lcc     Cleaned up TEMP_HACKS_FOR_TIGER
01/10/00   bgc     Added initialization of call manager task for PLT.
12/20/99   va      Removed mccap_initialize
11/15/99   va      IS2000 voice changes.
                   Added support for initializing the new mccap module
11/03/99   lcc     Merged from GD4021:
10/29/99   fc      Vocoder driver feature clean up. FEATURE_VOC_TASK replaced
                   FEATURE_QDSP2 and T_MSM31 replaced
                   FEATURE_QDSP_INTERNAL_CODEC.
09/22/99   lh      Merged following changes.
           lh      Merged PM1000 support. Code clean up for Panther
                   compatibility.
           lh      SMS and CM phase 2 implementation.
           mk      Added more FEATURE_QDSP2 and FEATURE_QDSP_INTERNAL_CODEC
                   support. Deleted TCXODIV2 support.
           jc      Removed support for MSM3100 GPIOs.
           jc/mk   Added support for MSM3100 GPIOs, deleted TCXODIV2 PLL,
                   deleted ITF3000 init, added TLMM init.
           spf     Pink Panther DFM support.  Fixed so acpwb_init() is called.
           spf     - Added 10/20MHz TCXO clock featurization.
                   - Added FEATURE_QDSP2 featurization to support new vocoder
                   interface.
                   - Added Vocoder task support.
           tn      Removed SBI related code, sbi_init is moved to rf_init.
           mk      Added initial (temporary) MSM3100 support.
           vl      Merged support for VS task and EFS task.
           lh      If def changes.
10/28/99   lcc     Corrected #ifdef around SBI (TG!=T_O instead of
                   !defined(T_O)).
10/28/99   lcc     Moved PLT related code to plt_mc.c.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Removed SIMULATED_RF since it isn't needed
           hrk     Added debug code for FFPC.
09/15/99   lcc     Merged in PLT support from PLT archive:
                   1. Merged in the latest changes for RPC via mini-DM
                   2. Enabled DEC Clock Regime for TIGER2 & call deint_init.
                   3. Merged in PLT changes
                   4. Conditionally compiled out the code that starts UI task
                      for PLT.
                   5. Added RPC task
07/07/99   vl      Merged support for VS task and EFS task.
08/12/99   nmn     Merged from SPSW: Added checking for external power during
                   powering down when in LPM mode.
08/11/99   ak      Added entry points for sockets udp test code, and fixed
                   up #defines for sockets test apps.
08/10/99   rp      Moved start of Embedded File System Task before NV Task
                   start, moved stop of Embedded File System Task after NV
                   Task stop.
07/30/99   lcc     Added some comments around snm_initialize() as a result of
                   code review.  Also removed some unit test code that was left
                   there unintentionally.
07/28/99   lh      CM and two-way SMS support added.
05/06/99   kmp     Merge IS-95B changes below & change T_AUTH & T_ACP with
                   FEATURE_AUTH and FEATURE_ACP respectively
           lh      Function mc_get_band_class, mc_get_p_rev_in_use added for
                   run-time P_REV and BC checking.
03/30/99   ak      Added entry point for sockets test code.
02/12/99   yll     Fix CR8381. call to ran_seed() to ensure that the
                   randomization routines work properly.
02/12/99   nmn     Moved ERR_INIT sooner to allow for logging of startup errors
02/01/99   br      RAM saving changes.
01/22/99   ht      Add structure for Embedded File System Task(for LSpall).
12/09/98   ro      call voc_manager_init and don't call mcc_voc_selftest
                    if FEATURE_VOCODER_MANAGER is defined
                    call voc_adsp_evrc_init if FEATURE_EVRC_ADSP is defined.
12/02/98   br      Keyword 'const' inserted in front of the ROM macro.

03/10/99   ram     Moved the MDR DPT init function to be after the
                   service tasks are started, since this function
                   is now using message services. This applies only to MDR.
02/17/99   ychan   Modified type of variable nv_item_list in function
                   mc_get_rf_items_from_nv as const nv_items_enum_type*.
02/12/99   lcc     Added initialization of CCL and mccsyobj objects.
02/12/99   nmn     Moved err_init earlier to allow for loging of task start up.
01/15/98   ak      Integrated back in old SIO code, under compile time switch.
01/13/99   lcc     Merged in MDR support:
                   - Added function call to initilize MDR DPT Service Options
                     Required for MDR DPT.
                   - Added service negotiation manager (used in MDR) support and
                     merged in some runtime P_REV/band class support functions.
01/12/98   ak      Made changes to support new SIO architecture.  Change to
                   mc_device_init - sio & dm initialization much simpler now.
12/14/98   dna     Didn't merge T_Q mods because using split RF in Panther 3.0
11/23/98   br      Inlude ma.h only if T_AUTO_ICU is defined (this is 186
                   specific feature and doesn't have to be included for ARM).
11/05/98   abh     Serial Bus Interface Initialisation for SURF  & defaulting
                   MSM3000 in MSM2300 mode.
10/07/98   ck      Changed the order of powering down of DH task to maintain
                   the reverse of task startup.
09/04/98   pms     Neared the variables to save ROM space.
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
08/25/98   ck      Added DH support for builds with FEATURE_DH defined
08/10/98   bns     Renamed sd_init() to bd_init() and changed to
                   FEATURE_BORSCHT to use generic BORSCHT driver
08/06/98   ces     Add QMIP task support for builds with FEATURE_QMIP defined.
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
07/30/98   lcc     Merged in ARM support.
04/16/98   aks     Made definitions for vbatt, therm, rf and hw configurations
                   valid for all targets EXCEPT for Unit Test (T_PC).
02/04/98   thh     Move Q's RF interface to the new RF architecture.
                   #ifdef out FM RF NV items with FEATURE_ACP for NGP.
02/03/98   kss     Used the new RF architecture for Module.
01/29/98   sk      Added code to take phone off-line if it is not a Qualcomm
                   phone.
01/28/98   ldg     Removed the read of NV_DATA_SRVC_STATE at startup (DS).
01/28/98   ldg     Removed the DS_SLIM switch.
01/28/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/28/98   wjn     Added a UI-MC interface function mc_get_prl_version();
01/17/98   dhh     Added support for IDS task.
11/17/97   jjn     Used the new RF architecture for ISS2 as well as for TGP
10/31/97   thh     Define the new RF architecture for TGP only.  This allows
                   those who are on other targets to work with the latest
                   MC.C.
10/08/97   thh     Fixed the hanging #endif that was left by the previous
                   check-in.
10/08/97   smo/udp Added clkregim enables for MSM2.3 support.
09/26/97   thh     Changed RF interface to support new RF driver architecture.
07/02/97   jjn     Moved Module initialization code into mod_init() and moved
                   Module power-down code into mod_powerdown()
06/17/97   dhh     Added target T_Q wherever applicable for Q phone.
05/27/97   thh     Added a new function mc_cal_charger_from_nv() to help
                   the Charger Utility calibrate the VBATT circuit.  This is
                   currently activated for TGP only.
05/23/97   jjn     Added mod_record_event, which informs the System Unit
                   that a power-down has occurred.  Only applicable for
                   the Module
05/20/97   ych     Added new entry_action MC_LPM_F to system determination.
02/10/97   thh     Added call to initialize the TGP Charger.
01/16/97   jah     Moved HS task definition above
                   NV task to allow a shorter time before the LCD
                   initialization and the associated power-up signs of life.
                   Added MC_HAS_RF/HW/THERM/VBATT_CONFIG.  Added comments
01/14/97   bns     Added calls to initialize the ECFM and SinBaD drivers.
10/03/96   thh     Update the RF nv items table in mc_get_rf_items_from_nv().
                   Remove the obsolte NV_FM_LNA_CTL_I.  This shows up as
                   "00 1434 08 mc" in the error log.  Remove a duplicate
                   (1 of 2) NV_FM_EXP_HDET_VS_TEMP_I.
09/12/96    ro     Changed RF_CODEC_VOC to RF_CODEC_MC in mc_device_init()
                   to avoid potential problem if call to mcc_voc_selftest()
                   would be moved or removed.
05/24/96   jjw     Gemini & ISS2 Data Services release.
04/10/96   jah     Changed RF NV items list for new RF Compensation method
                   Fixed error message in HW_CONFIG read.
03/28/96   jah     Added MSM 2.2/2p ICU auto-detect/configure logic under
                   control of #ifdef T_AUTO_ICU.
03/27/96   rah     Removed T_G related conditional compiles for
                   mcc_voc_selftest call.
03/26/96   jca     Removed T_ACP conditional on call to acpwb_init().
02/08/96   ras     Added CSS support
11/28/95   jah     Added LOCAL void mc_cal_hw_from_nv( void )
11/08/95   jah     Fixed entry_action bug for all non-Gemini targets
11/01/95   jah     Changed rf_init_delayed() to return on/off-line status.
10/31/95   ras     added #if's for initial CSS build from ref. "spooky"
10/12/95   jah     Added new RF NV items, NV_FM_AGC_I and NV_FM_LNA_CTL_I, to
                   mc_get_rf_items_from_nv().
07/21/95   rdh     Made max SCI = 7 now.
07/14/95   jjw     Small changes for IS-99 data services.
06/19/95   rdh     Made NV_NOTACTIVE status handling less severe.
06/12/95   tst     Added NV_PA_RANGE_STEP_CAL, NV_LNA_RANGE_POL,
                   NV_LNA_RANGE_RISE, NV_LNA_RANGE_FALL,
                   and NV_LNA_RANGE_OFFSET to NGP and ISS2 RF calibration
                   tables.
                   Removed NV_CDMA_TX_COMP_VS_VOLT from ISS2 RF cal table.
06/07/95   dna     Added Authentication queues.
06/07/95   tst     Removed NV_CDMA_TX_LIM_VS_VOLT for ISS2 calibration.
05/23/95   jah     Removed THERM from Portable
05/23/95   tst     Modified Gemini RF NV items list to remove xxx_VS_VOLT.
                   Added code to mc_get_rf_items_from_nv() to call
                   rf_init_delayed() after NV items have been loaded for the
                   Gemini target only.
05/19/95   jah     Fixed dangling #elif in HDLC init support for iss1
05/19/95   ras     Added HDLC init support for iss1
05/17/95   jah     Modified mc_device_init for MSM 2p.  Removed therm
                   calibration from CD-7000, and added error case to per
                   target calibration table.
05/12/95   tst     Added latest NV items for RF cal.
05/06/95   tst     Added mc_cal_therm_from_nv() function.
                   Added call to mc_cal_therm_from_nv() to mc_task().
                   Modified mc_get_rf_items_from_nv() to use lookup table.
                   Added #include therm.h.
                   Modified several conditional compiles to include T_G.
04/06/95   jjw     Added and #ifdef DS around dsm_init()
04/05/95   jjw     Added changes to support PS task (for Data service
                   compile-time inclusion only)
10/24/94   ras     Added #LIFETEST define for factory mtbf testing.
08/09/94   jca     mcc_voc_selftest no longer returns status.
08/08/94   jjw     Fixed Portable w/DS compile-time error.
08/02/94   jca     Added T_ACP conditional compiles to support ISS1 release.
04/13/94   jjw     Added updated SIO service initialization to support Data
                   Services
01/06/94   jah     Changed watchdog iface to use dog_report() & dog_report_type.
12/28/93   jah     Added initialization for 'leds' for sleep task w/T_SLEEP.
12/01/93   jah     Changed mc_init_items_to_nv() to allow configuration of SCI
11/08/93   jjw     Changed NV_EXT_PORT_MODE_I to NV_DM_IO_MODE_I to conform
                   to new NV name.
08/05/93   jah/ptw Added init of sleep task.
05/17/93   jca     Added NV_FM_RSSI_I to list of RF NV items.
05/10/93   jca     Added code to load new RF calibration tables from NV.
04/13/93   jjw     Moved sio_init() from mc_device_init() to the mc_task to
                   initialize SIO services to the mode stored in NV.
02/23/93   jah/gb  Added TIMETEST code for task performance analysis using
                   an 8-bit buffered output.
02/61/93   jah/jca Added loading of EEPROM calibration tables for the
                   RF and Vbatt libraries.
01/20/93   jca     Added conditional call to acpwb_init for Portable version.
12/09/92    ip     Added call to initialize error services after
                   service tasks are started (after NV is started).
09/22/92   jah     Added enc_init() prior to rf_init() to allow the Encoder's
                   outputs to be initialized prior to being used by rf_init().
08/29/92    ip     Changed rex_wait to mc_wait; changed order so that
                   subtasks powerdown is called before DMSS powerdown
                   is started; changed to force offline if FALSE
                   status returned from subtasks powerup init.
08/22/92    ip     Fixes after code review and initial integration.
08/13/92    ip     Added configuration checksum temporary update.
08/10/92    ip     First release for integration.
04/30/92    ip     Initial porting from Brassboard to DMSS.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "amssassert.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"

#include "modem_mem.h"

#include "rxcmc_i.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mci.h"
#include "mci_i.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "sd.h"
#include "sd_v.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccsrch_i.h"
#include "rcinit.h"
#include "secapi.h"

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#include "rex.h"
#include "queue.h"
#include "err.h"

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "rfm.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF  */

#include "db.h"
#include "db_v.h"
#include "msg.h"
#include "m1x_diag.h"
#include "task.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "cdma2kdiagi.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "nv.h"
#include "onex_nv_rd_wt.h"
#include "tx_v.h"
#include "rx_v.h"
#include "cmxll.h"
#include "cm.h"
#include "rxtx_v.h"
#include "srchmc.h"
#include "snm_v.h"
#include "snm_i.h"
#include "diag.h"
#include "cdma2kdiag.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccbcsms.h"
#include "mccapdf_v.h"
#ifdef FEATURE_GPSONE
#include "mccpd.h"
#endif /* FEATURE_GPSONE */

#include "mccmeid.h"

#if !defined (_enable)
    void _enable( void );
#endif

#include "mccsyobj_v.h"
#include "mccccl.h"
#include "tdso_v.h"
#include "onex_non_sig.h"

#ifdef FEATURE_UIM
#include <memory.h>
#endif /*FEATURE_UIM */

#include "mccso.h"
#include "mccapdf_v.h"
#include "ran.h"
#include "event.h"

#ifdef FEATURE_HDR
#include "amssassert.h"
#endif /* FEATURE_HDR */

#include "mclog_v.h"
#include "mmoc.h"
#include "mmoc_v.h"

#ifdef FEATURE_REX_SIGS_EXT
  #ifndef HAVE_REX_SIGS_EXT
    #error "REX Signal API Extension is not avaibale. Check REX Version"
  #endif /* HAVE_REX_SIGS_EXT */
#endif /* FEATURE_REX_SIGS_EXT */

#ifdef FEATURE_CSIM
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#endif
/*lint -e818 */

/* Include files for EFS APIs */
#include "fs_public.h"
#include "fs_lib.h"
#include "fs_errno.h"
/* Include file for string APIs */
#include <string.h>

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "onex_cp_msg.h"
#include "mcccsfbsup.h"
#include "mccsrid_v.h"
#include "mccgcsna.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#include "mcctcho_i.h"
#include "caix_i.h"
#include "srv.h"
#include "mccap_v.h"

/*  Feature TEST_FRAMEWORK is enabled to facilitate stubbing
 *  of SRCH/MUX modules, for offtarget testing of CSFB features.
 *  This feature is a temp feature and will be cleaned up in future.
 */
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
#include "mccdma_i.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*Variables and macros localized from mobile.c/h to 1X_CP files
=================================================================================================*/

/* Station Class Mark
*/
const byte mc_station_class_mark = 0x2a  ;
	
/* Slot Cycle Index (CAI Sections 6.6.2.1.1.3, 6.7.1.3.2.1, and 6.7.4.7)
*/ /* CR518578 Changes Default, 0x02 */
const byte mc_slot_cycle_index = 0x02;

/* Mobile CAI Revision (CAI Section 6.7.4.7)
*/
const byte mc_mob_cai_rev = MOB_P_REV;


/* Mobile Firmware Revision (CAI Section 6.7.4.7)
*/
const word mc_mob_firm_rev = 100;

/* Mobile Model (CAI Section 6.7.4.7)
**    Value provided by cust*.h file or .BUILDS file
*/
#ifndef CUST_MOB_MODEL
#define CUST_MOB_MODEL 0
#endif

const byte mc_mob_model = (byte)CUST_MOB_MODEL; // 8 bits. do not modify size.

/*===================================================================================================*/



/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
** The following status variable is used to save selected system info which
** is used to provide info to System Determination.
** ------------------------------------------------------------------------- */
LOCAL mc_online_type sel_sys_info;

/* ------------------------------------------------------------------------
** The following status variables are used to save entry reason to CDMA and
** Analog call processing for MC
** ------------------------------------------------------------------------ */
mcc_entry_type cdma_entry;   /* CDMA entry reason   */

/* ------------------------------------------------------------------------
** The following status variable is used to save last entry reason to CDMA
** and Analog call processing which is used to provide info to Diag
** ------------------------------------------------------------------------ */
LOCAL mcc_status_type last_entry;

/* ------------------------------------------------------------------------
** The following variable is used to save the current state of the
** mode controller in static memory so it can be peeked for logging.
** ------------------------------------------------------------------------ */
LOCAL mc_state_type   mc_mode_controller_state;
                                 /*lint -esym(551,mc_mode_controller_state)*/


/*---------------------------------------------------------------------------
                  Variables pertaining to RCINIT
---------------------------------------------------------------------------*/
/* Create TCB pointer */
rex_tcb_type* mc_tcb_ptr = NULL;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Local Data                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Data base buffer.                                                       */

LOCAL db_items_value_type  mc_db_item;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================
                   MSGR INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/* Buffer to receive messages over MSGR */
mc_msgr_rcv_buffer_type  mc_msgr_rcv;

/* Record type to indicate SD of the redirection */
sd_redir_rec_s_type mc_irat_redir_rec;

/* Internal data structures for storing the messages received over MSGR */

/* Redirection message */
onex_cp_lte_redir_req_msg_t         mc_redir_req_msg;

/* Reselection message */
onex_cp_lte_resel_req_msg_t         mc_resel_req_msg;

/* Redirection abort indication */
onex_cp_lte_abort_redir_req_msg_t   mc_redir_abort_ind;

/* Reselection abort indication */
onex_cp_lte_abort_resel_req_msg_t   mc_resel_abort_ind;

/* Internal data structures for storing the messages sent over MSGR */

/* redir failed indication */
onex_cp_lte_redir_failed_rsp_msg_t  mc_redir_failed_ind;

/* resel failed indication */
onex_cp_lte_resel_failed_rsp_msg_t  mc_resel_failed_ind;

/* pointer to store message received over MSGR */
mc_msgr_rcv_msg_type *mc_msgr_msg_ptr;

/* To indicate if validation of IRAT switch is successful */
boolean mc_irat_switch = FALSE;

/* Variable to indicate if MSGR is successfully registered */
boolean  mc_is_msgr_registered = FALSE;

/* Variables which are different for CSFB and legacy 1X mode of operation */
mcc_csfb_ms_type csfb_cdma;

/*===========================================================================
                  INTERNAL FUNCTIONS FOR IRAT SWITCH TO 1X
===========================================================================*/

static void mc_init_msgr_queue( void );
static msgr_client_t*  mc_get_msgr_client_id( void );
static msgr_id_t*  mc_get_msgr_queue_id( void );
static void mc_msgr_setup_client( void );
static void mc_msgr_register_msgs( void );

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X  */


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                              REX Data                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Array holding mc queue statistics */
mc_queue_info_type mc_queue_info[MC_MAX_Q];

#ifdef FEATURE_1X_CP_MEM_OPT
/* Variable indicating whether max limit set on number of buffer allocation
   for each queue */
byte mc_set_queue_max_limit = TRUE;
#endif /* FEATURE_1X_CP_MEM_OPT */

/* Command queue. The command queue is shared by the MCC and
 * ACPMC subtasks.  The number of buffers is designed to anticipate
 * the most numbers of commands which could be queued onto to MC
 * (and its subtasks) at any given time.
 */
q_type            mc_cmd_q;

/* Debug Command queue. The debug command queue is exclusively maintained by 
 * MC task (both read and write access). It keeps track of the sanity timer 
 * associated MMOC cmds already processed by MC for debugging purpose */
q_type mc_dbg_cmd_q;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* Debug Command queue. The debug command queue is exclusively maintained by
 * MC task (both read and write access). It keeps track of the commands rxed
 * through msgr_q */
q_type mc_dbg_msgr_q;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#define MC_EXPIRED_TIMERS_MAX 10
LOCAL  mc_expired_timer_type mc_expired_timer_buf[MC_EXPIRED_TIMERS_MAX];
       q_type              mc_expired_timers_free_q;
       q_type              mc_expired_timers_q;

#if defined(FEATURE_RUIM)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for Authentication Task Report Interface                          */

q_type          mc_auth_q;
q_type          mc_auth_free_q;

/* Buffers to be placed on the mc_auth_free_q */
#define                               MC_NUM_AUTH_RPT_BUFS 3
LOCAL auth_rpt_type mc_auth_rpt_pool[ MC_NUM_AUTH_RPT_BUFS ];

#endif /* FEATURE_RUIM*/

#ifdef FEATURE_UIM_RUIM
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for UIM Server Task Report Interface                          */

q_type          mc_uim_q;
#ifndef FEATURE_1X_CP_MEM_OPT
q_type          mc_uim_free_q;

/* Buffers to be placed on the mc_uim_free_q */
#define                             MC_NUM_UIM_RPT_BUFS 6
LOCAL uim_rpt_type mc_uim_rpt_pool[ MC_NUM_UIM_RPT_BUFS ];
#endif /* !FEATURE_1X_CP_MEM_OPT */


#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_CSIM
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for MMGSDI Task Report Interface                          */

q_type          mc_mmgsdi_q;
#ifndef FEATURE_1X_CP_MEM_OPT
q_type          mc_mmgsdi_free_q;


/* Buffers to be placed on the mc_uim_free_q */
#define  MC_NUM_MMGSDI_RPT_BUFS 6
LOCAL mc_mmgsdi_rpt_type mc_mmgsdi_rpt_pool[ MC_NUM_MMGSDI_RPT_BUFS ];
#endif /* !FEATURE_1X_CP_MEM_OPT */

#endif /* FEATURE_CSIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for DH Task Report Interface                                      */

q_type          mc_dh_q;
#ifndef FEATURE_1X_CP_MEM_OPT
q_type          mc_dh_free_q;

/* Buffers to be placed on the mc_dh_free_q */
#define                               MC_NUM_DH_RPT_BUFS 2
LOCAL dh_rpt_type mc_dh_rpt_pool[ MC_NUM_DH_RPT_BUFS ];
#endif /* !FEATURE_1X_CP_MEM_OPT */

/* DOG report ID for MC task */
dog_report_type   mc_dog_rpt_id = 0;

#ifdef FEATURE_MODIFIED_SCI
#error code not present
#endif /* FEATURE_MODIFIED_SCI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for accessing EFS                                                 */

/* Name and path of the conf file for 1X CP */
#define MC_EFS_CONF_FILE "/nv/item_files/conf/mc.conf"

/* Names and paths of the EFS item files */
LOCAL const efs_item_file_type mc_efs_item_file[MC_EFS_ITEM_FILE_MAX] =
{
    /* MC_EFS_SO73_COP0_SUPPORTED - 65634 */
    {
      "/nv/item_files/cdma/1xcp/so73_cop0_supported", 1
    }
    ,
    /* MC_EFS_REG_AFTER_DDTM_PAGE_DROP_SUPPORTED - 65869 */
    {
      "/nv/item_files/cdma/1xcp/reg_after_ddtm_page_drop", 1
    }
    ,
    /* MC_1XADVANCED_CAPABILITY - 65881 */
    {
      "/nv/item_files/cdma/1xcp/1xadvanced_capability", 1
    }
    ,
    /* MC_EFS_1xCSFB_T_MAX_NO_C2K_PARAM - 67252 */
    {
      "/nv/item_files/cdma/1xcp/1xcsfb_t_max_no_c2k_param", 2
    }
    ,
    /* MC_EFS_1xCSFB_T_MAX_NO_VOICE - 67253 */
    {
      "/nv/item_files/cdma/1xcp/1xcsfb_t_max_no_voice", 4
    }
    ,

    /* MC_EFS_1XCSFB_N_MAX_REG_RETRY_CNT - 67254 */
    {
      "/nv/item_files/cdma/1xcp/1xcsfb_n_max_reg_retry_cnt", 1
    }
    ,
    /* MC_EFS_DISABLE_SO35_SO36 - 67286 */
    {
      "/nv/item_files/cdma/1xcp/disable_so35_so36", 1
    }
    ,
    /* MC_EFS_OTASP_MAX_UNLOCK_ATTEMPTS - 69708 */
    {
      "/nv/item_files/cdma/1xcp/otasp_max_unlock_attempts", 1
    }
    ,

    /* MC_EFS_DISABLE_1X_TO_LTE_RESELECTION - 69709 */
    {
      "/nv/item_files/cdma/1xcp/disable_1x_to_lte_reselection", 1
    }
    ,
    /* MC_EFS_CMAS_DUP_DET_SUPPORTED - 70243 */
    {
      "/nv/item_files/cdma/1xcp/cmas_dup_det_supported", 1
    }
    ,
    /* MC_EFS_DEREGISTER_TIMER_VAL - 70303 */
    {
      "/nv/item_files/cdma/1xcp/deregister_timer_val", 1
    }
    ,
    /* MC_EFS_REJ_VOICE_PAGE_FOR_VOLTE - 70360 */
    {
      "/nv/item_files/cdma/1xcp/rej_voice_page_for_volte", 1
    }
    ,
    /* MC_EFS_SPECIFIC_CALL_END_STATS_SUPPORTED - 71518 */
    {
      "/nv/item_files/cdma/1xcp/specific_call_end_stats_supported", 1
    }
    ,
    /* MC_EFS_1xCSFB_CGI_FEATURE_SUPPORTED - 71575 */
    {
      "/nv/item_files/cdma/1xcp/1xcsfb_cgi_feature_supported", 1
    }
    ,
    /* MC_EFS_1xCSFB_REDIR_CALL_SETUP_OPTIMISED - 71577 */
    {
      "/nv/item_files/cdma/1xcp/csfb_call_setup_optimised", 1
    }
    ,
    /* MC_EFS_1XSRVCC_T_MAX_NO_UHDM - 72577 */
    {
      "/nv/item_files/cdma/1xcp/1xsrvcc_t_max_no_uhdm", 2
    }
    ,
    /* MC_EFS_USE_DEF_VAL_FOR_PWRUP_NV_ERR - 72625 */
    {
      "/nv/item_files/cdma/1xcp/use_def_val_for_pwrup_nv_err", 1
    }
    ,
    /* MC_EFS_SPLIT_OVERHEAD_SUPPORTED - 73501 */
    {
      "/nv/item_files/cdma/1xcp/split_ovhd_supported", 1
    }
    ,
    /* MC_EFS_1X_OVHD_OPTIMIZATION - 73548 */
    {
      "/nv/item_files/cdma/1xcp/1x_ovhd_optimization", 1
    }    
    ,
    /* MC_EFS_CUST_EVRC_VOICE_ENCRYPT_SO - 73570 */
    {
      "/nv/item_files/cdma/1xcp/cust_voice_encryp_so", 2
    }
    ,
    /* MC_EFS_1xSRLTE_MT_PAGE_ACC_HO_SUPPORTED - TBD */
    {
      "/nv/item_files/cdma/1xcp/1xsrlte_mt_page_acc_ho_supported", 1
    }
    ,
    /* MC_EFS_JCDMA_SUPPORTED - 65810 */
    {
      "/nv/item_files/jcdma/jcdma_mode", 1
    }
    ,

    /* MC_MMGSDI_FEATURES_STATUS_LIST - 67211 */
    {
      "/nv/item_files/modem/uim/mmgsdi/features_status_list", 64
    }
    ,
    /* MC_EFS_SMS_OVER_S102 - 70362 */
    {
      "/nv/item_files/modem/mmode/sms_over_s102", 1
    }
    ,
    /* MC_C2K_RESEL_SPLMN_SUPPORT - 72518 */
    {
      "/nv/item_files/modem/mmode/sd/c2k_resel_splmn_supp", 1
    }
};

/* Buffer for EFS access */
mc_efs_file_data_type mc_efs_data;


/* Critical section that is common throughout the MC task */
rex_crit_sect_type mc_crit_sect;

/* Critical section to protect reg related ptrs and queues */
rex_crit_sect_type mc_reg_crit_sect;

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
/* Critical section for protecting new audio architecture related variables 
 * that can be accessed by CDMA Voice Adapter, MMode, 1X-CP and 1X-MUX  */
rex_crit_sect_type mc_audio_crit_sect;

/* Variable to keep track whether mc_audio_crit_sect mutex
 * has been initialized or not. */
volatile boolean is_audio_mutex_initialized =  FALSE;

#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

/* Flag to indicate mc transaction queues are intialized */
boolean mc_trans_queues_initialized = FALSE;

#ifdef FEATURE_MODEM_1X_DRVCC
/* Variable to keep track of DRVCC specific states */
mcc_drvcc_ms_type drvcc_cdma;
#endif /*  FEATURE_MODEM_1X_DRVCC */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Macro to update the DMSS state in the database (db).                    */

#define SET_DB_DMSS_STATE(state) \
    mc_db_item.dmss_state = (state); \
    db_put(DB_DMSS_STATE, &mc_db_item)

/*===========================================================================

FUNCTION MC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL rex_sigs_type mc_wait
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{

  rex_sigs_type   sigs;       /* Signals returned by rex_get_sigs call */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for signals specified in mask and   */
  /* for watchdog signal.  If watchdog signal then kick it  */
  /* and loop again until some other signal in mask is set. */

  while (((sigs = rex_get_sigs(MC_TCB_PTR)) & mask) == 0) {
    mc_dog_report();

    /* If we have sent a command to auth task, we need to monitor various mc
    ** queues and also listen for SIM BUSY event from MMGSDI to ensure that
    ** we are not stuck in wait state waiting for a response from auth task
    ** if SIM BUSY event happens. If sim busy does happen we cancel all MMGSDI
    ** requests. This will ensure that the auth task is unblocked and we will
    ** get a response from auth task.
    */

    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    if (cdma.sim_busy.is_auth_pending_in_queue)
    {
      sigs =  rex_wait( mask | MC_DOG_RPT_SIG | MCC_Q_SIG | MCC_SIM_BUSY_SIG );
      M1X_MSG( DCP, LEGACY_LOW,
               "mc_wait : Waiting for response from auth task" );
      if (sigs & MCC_SIM_BUSY_SIG)
      {
        M1X_MSG( DCP, LEGACY_MED, "mc_wait : sim busy signal set" );
        (void) rex_clr_sigs( MC_TCB_PTR, MCC_SIM_BUSY_SIG );
        if (mc_get_sim_busy_status())
        {
          /* Cancel all requests sent to MMGSDI */
          mc_send_mmgsdi_cancel(); 
        }
      } /* if (sigs & MCC_SIM_BUSY_SIG) */

      if (sigs & MCC_Q_SIG)
      {
        /* Monitor MC queues to ensure they do not get full. */
        mc_mmgsdi_monitor_queues();
        (void) rex_clr_sigs( MC_TCB_PTR, MCC_Q_SIG );
      } /* if (sigs & MCC_Q_SIG) */
    } /* if (cdma.sim_busy.is_auth_pending_in_queue) */
    else
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
    {
      (void) rex_wait( mask | MC_DOG_RPT_SIG );
    }
  } /* end while */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* kick watchdog one last time and return the signals. */

  mc_dog_report();
  return(sigs);

} /* mc_wait */

/*===========================================================================

FUNCTION MC_QUEUE_INIT

DESCRIPTION
  This function performs MC task static initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  After return from this funtion task timers can be used.

===========================================================================*/

LOCAL void mc_queue_init (void)
{
  word    i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize queues. */

  (void) q_init(&mc_cmd_q);

  (void) q_init(&mc_dbg_cmd_q);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  (void) q_init(&mc_dbg_msgr_q);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  /* Initialize Expired Timer Queues. */

  (void) q_init(&mc_expired_timers_q);
  (void) q_init(&mc_expired_timers_free_q);

  /* Fill mc_expired_timers_free_q. */

  for (i = 0; i < MC_EXPIRED_TIMERS_MAX; i++) {
    q_put(&mc_expired_timers_free_q,
          q_link(&mc_expired_timer_buf[i], &mc_expired_timer_buf[i].link));
  }

  /* This function is benign when running in DMSS mode */
  onex_nonsig_ftm_init();

  #if defined(FEATURE_RUIM)
  /* initialize queue for Authentication Reports */
  (void) q_init( &mc_auth_q );
  (void) q_init( &mc_auth_free_q );

  /* Fill mc_auth_free_q */
  for (i = 0; i < MC_NUM_AUTH_RPT_BUFS; i++)
  {
    mc_auth_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_auth_free_q;
    q_put( &mc_auth_free_q, q_link( &mc_auth_rpt_pool[i],
           &mc_auth_rpt_pool[i].rpt_hdr.link ) );
  }
  #endif /* FEATURE_RUIM */

  #ifdef FEATURE_UIM_RUIM
  /* initialize queue for Authentication Reports */
  (void) q_init( &mc_uim_q );
#ifndef FEATURE_1X_CP_MEM_OPT
  (void) q_init( &mc_uim_free_q );

  /* Fill mc_auth_free_q */
  for (i = 0; i < MC_NUM_UIM_RPT_BUFS; i++)
  {
    mc_uim_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_uim_free_q;
    q_put( &mc_uim_free_q, q_link( &mc_uim_rpt_pool[i],
           &mc_uim_rpt_pool[i].rpt_hdr.link ) );
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */

  #endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_CSIM
  /* initialize queue for Authentication Reports */
  (void) q_init( &mc_mmgsdi_q );
#ifndef FEATURE_1X_CP_MEM_OPT
  (void) q_init( &mc_mmgsdi_free_q );

  /* Fill mc_auth_free_q */
  for (i = 0; i < MC_NUM_MMGSDI_RPT_BUFS; i++)
  {
    mc_mmgsdi_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_mmgsdi_free_q;
    q_put( &mc_mmgsdi_free_q,
           q_link( &mc_mmgsdi_rpt_pool[i],
           &mc_mmgsdi_rpt_pool[i].rpt_hdr.link ) );
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */

  #endif /* FEATURE_CSIM */



  /* initialize queue for DH Reports */
  (void) q_init( &mc_dh_q );
#ifndef FEATURE_1X_CP_MEM_OPT
  (void) q_init( &mc_dh_free_q );

  /* Fill mc_dh_free_q */
  for (i = 0; i < MC_NUM_DH_RPT_BUFS; i++)
  {
    mc_dh_rpt_pool[i].rpt_hdr.done_q_ptr = &mc_dh_free_q;
    q_put( &mc_dh_free_q, q_link( &mc_dh_rpt_pool[i],
           &mc_dh_rpt_pool[i].rpt_hdr.link ) );
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */


  /* Initialize queues */
  mccrxtx_init();
  mccsrch_init();
  mccrx_init();

  /* Set up queues for continous dtmf processing */
#ifndef FEATURE_1X_CP_MEM_OPT
  ( void )q_init(&mcc_dtmf_free_q);
#endif /* !FEATURE_1X_CP_MEM_OPT */

  ( void )q_init(&mcc_dtmf_q);

#ifndef FEATURE_1X_CP_MEM_OPT
  for(i=0; i< MCC_MAX_DTMFS_OUT; i++ )
  {
    q_put( &mcc_dtmf_free_q,  q_link( &cdma_dtmf_bufs[i],
                              &cdma_dtmf_bufs[i].link ) );
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */

} /* mc_queue_init */

/*===========================================================================

FUNCTION MC_PUT_NV_ITEM

DESCRIPTION
  This function puts a specific item to NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  None.

SIDE EFFECTS
  While this function is running all other MC task activities are
  suspended except for watchdog kicking, until the NV item is written.

===========================================================================*/

LOCAL void mc_put_nv_item
(
  nv_items_enum_type  item_code,       /* Item to put */
  nv_item_type        *item_ptr        /* Pointer where to get the item */
)
{

  static nv_cmd_type  mc_nv_cmd_buf;   /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  onex_nv_read_write(item_code, item_ptr, NV_WRT | NV_MC, &mc_wait, 
                     &mc_nv_cmd_buf);

  /* If there is an error in the request then this is a fatal fault. */

  if (mc_nv_cmd_buf.status != NV_DONE_S) {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Failed write item to NV %d",
      (int) item_code);
  }

} /* mc_put_nv_item */

/*===========================================================================

FUNCTION MC_GET_NV_ITEM

DESCRIPTION
  This function gets a specific item from NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
  Any other status causes ERR_FATAL and the function does not return.

SIDE EFFECTS
  While this function is running all other MC task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.  If any of read status other then the one described above
  is returned then an ERR_FATAL call is made.

===========================================================================*/

LOCAL nv_stat_enum_type mc_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get */
  nv_item_type        *item_ptr        /* Pointer where to put the item */
)
{

  static nv_cmd_type  mc_nv_cmd_buf;   /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!onex_nv_read_write(item_code, item_ptr, NV_RD | NV_MC, &mc_wait, 
                          &mc_nv_cmd_buf)) 
  {                                        /* else fatal error  */
    ERR_FATAL( "Bad NV read status %d", (int) mc_nv_cmd_buf.status, 0, 0 );
  }

  return (mc_nv_cmd_buf.status);                   /* return status     */

} /* mc_get_nv_item */

/*===========================================================================

FUNCTION MC_INIT_ITEMS_TO_NV

DESCRIPTION
  This function loads configuration items from mobile.c definitions
  into NV.  These are items which reside in NV only for consistency
  with mobile station database items to be reported to the base station
  and to be available to the handset (UI).  In each case the item is
  first read.  It then written only if it is different or if was inactive.
  The items are:
    NV_SCM_I               - Station Class Mark
    NV_SLOT_CYCLE_INDEX_I  - Slot cycle index
    NV_MOB_CAI_REV_I       - Mobile CAI revision number
    NV_MOB_FIRM_REV_I      - Mobile firmware revision number
    NV_MOB_MODEL_I         - Mobile model
    NV_CONFIG_CHKSUM_I     - Configuration checksum

DEPENDENCIES
  The NV task is running.

RETURN VALUE
  None.

SIDE EFFECTS
  This function calls mc_put_nv_item which suspends all MC task
  activities, except for watchdog kicking, while waiting for
  completion of write to NV.

===========================================================================*/

LOCAL void mc_init_items_to_nv (void)
{

  nv_item_type          mc_nv_data_buf;     /* NV data buffer */
  nv_stat_enum_type     read_status;        /* NV read status */

  /* Local boolean variable to check if writing SCI value in to NV */
  boolean write_SCI_to_NV = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_SCM_I */

  read_status = mc_get_nv_item(NV_SCM_I, &mc_nv_data_buf);
  if (read_status == NV_NOTACTIVE_S) {
    mc_nv_data_buf.scm = mc_station_class_mark;
    mc_put_nv_item(NV_SCM_I, &mc_nv_data_buf);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*lint -save -e539 Indentation varies due to features defined */

  /*
   * As UIM code has different braches for Nikel_2.0* PL's and Nikel_3.0+ PL's
   * and the FEATURE_UIM_RUIM_SUPPORT_SCI defined on Nikel_2.0 PL's and
   * On Nikel_3.0+ PL's FEATURE_UIM_RUIM_SUPPORT_SCI is supported through (efs based)NVITEM#67211
   * But the MC-main/latest feeds both Nikel_2.0+ and Nikel_3.0+ PL's so to be in consistent with
   * UIM code for FEATURE_UIM_RUIM_SUPPORT_SCI feature
   * below is the logic to enable or disable on MC side
   */

  #ifdef FEATURE_UIM_RUIM_SUPPORT_SCI
  /* For Nikel_2.0* PL's, the "FEATURE_UIM_RUIM_SUPPORT_SCI" is enabled
   * so update MC global varaible to Enabled
   */
  cdma.uim_ruim_support_sci = MMGSDI_FEATURE_ENABLED;
  M1X_MSG( DCP, LEGACY_MED,
    "UIM_RUIM_SUPPORT_SCI = feature enabled");

  #elif FEATURE_UIM_NO_RUIM_SUPPORT_SCI
  /* For Nikel_2.0* PL's, the "FEATURE_UIM_NO_RUIM_SUPPORT_SCI" is defined
   * explicitly on UIM side if "FEATURE_UIM_RUIM_SUPPORT_SCI" is not enabled
   * so update MC global varaible to Disabled
   */
  cdma.uim_ruim_support_sci = MMGSDI_FEATURE_DISABLED;
  M1X_MSG( DCP, LEGACY_MED,
    "UIM_NO_RUIM_SUPPORT_SCI case, UIM_RUIM_SUPPORT_SCI = feature disabled");

  #else
  /* For Nikel_3.0+ PL's,
   * Read EFS for MMGSDI_FEATURES_STATUS_LIST for FEATURE_UIM_RUIM_SUPPORT_SCI support
   */
  (void) mc_efs_get(MC_EFS_MMGSDI_FEATURES_STATUS_LIST, &mc_efs_data);
  cdma.uim_ruim_support_sci = mc_efs_data.mc_mmgsdi_features_status_list.uim_ruim_support_sci;

  #endif

  M1X_MSG( DCP, LEGACY_MED,
    "uim_ruim_support_sci = %d",
    cdma.uim_ruim_support_sci);

  /* NV_SLOT_CYCLE_INDEX_I */

  /* Write SCI item only if we know that the item is being stored into NV.
   * Item is stored in NV if any of the folliwing conditions is true:
   *    1) FEATURE_RUIM is not enabled
   *    2) FEATURE_UIM_RUIM_SUPPORT_SCI is not enabled
   *    3) If RTRE configuration is set to NV.
   *
   * In all other cases, SCI item gets stored into the card and we will not
   * write the SCI.
   */

  if( !mcc_is_uim_ruim_support_sci_supported() )
  {
    /* Feature "uim_ruim_support_sci" is not supported
     * So write SCI into NV
     */
    write_SCI_to_NV = TRUE;
  }
  else
  {
    #ifdef FEATURE_UIM_RUN_TIME_ENABLE
  read_status = mc_get_nv_item(NV_CURR_NAM_I, &mc_nv_data_buf);
  if ( read_status == NV_DONE_S )
  {
    if (!nv_rtre_use_ruim_for_nam(mc_nv_data_buf.curr_nam))
      {
        /* RTRE configuration is set to NV so write SCI into NV */
        write_SCI_to_NV = TRUE;
      }
      else
      {
        /* RTRE configuration is NOT set to NV so DON'T write SCI into NV */
        write_SCI_to_NV = FALSE;
      }
    } /* End if ( read_status == NV_DONE_S ) */
    #else /* FEATURE_UIM_RUN_TIME_ENABLE */
      #ifdef FEATURE_RUIM
      /* FEATURE_RUIM is enabled so DON'T write SCI into NV */
      write_SCI_to_NV = FALSE;
      #else /* FEATURE_RUIM */
      /* FEATURE_RUIM is enabled so write SCI into NV */
      write_SCI_to_NV = TRUE;
#endif /*FEATURE_RUIM*/
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  } /* if( !mcc_is_uim_ruim_support_sci_supported() ) */

    if( write_SCI_to_NV )
    {
      read_status = mc_get_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
      if (read_status == NV_NOTACTIVE_S)
      {
       mc_nv_data_buf.slot_cycle_index = (int8) mc_slot_cycle_index;
       mc_put_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
      }

      /* limit SCI to a max of REL_D_MAX_SCI and a min of REL_D_MIN_SCI
      */
      if (mc_nv_data_buf.slot_cycle_index > MCC_MAX_SCI)
      {
       mc_nv_data_buf.slot_cycle_index = MCC_MAX_SCI;
       mc_put_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
      }

      if (mc_nv_data_buf.slot_cycle_index < MCC_MIN_SCI)
      {
        mc_nv_data_buf.slot_cycle_index = MCC_MIN_SCI;
        mc_put_nv_item(NV_SLOT_CYCLE_INDEX_I, &mc_nv_data_buf);
      }
  } /* End if( write_SCI_to_NV ) */


/*lint -restore */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_NEG_SLOT_CYCLE_INDEX_I */

  #ifdef FEATURE_MODIFIED_SCI
  #error code not present
#endif /* FEATURE_MODIFIED_SCI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_MOB_CAI_REV_I */

  read_status = mc_get_nv_item(NV_MOB_CAI_REV_I, &mc_nv_data_buf);

  #ifndef FEATURE_IS2000_REL_A
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.mob_cai_rev != mc_mob_cai_rev)
     )
  {
    mc_nv_data_buf.mob_cai_rev = mc_mob_cai_rev;
    mc_put_nv_item(NV_MOB_CAI_REV_I, &mc_nv_data_buf);
  }
  #else /* FEATURE_IS2000_REL_A */
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.mob_cai_rev > mc_mob_cai_rev) ||
      (!MOB_P_REV_IS_VALID(mc_nv_data_buf.mob_cai_rev))
     )
  {

    if (mc_mob_cai_rev == P_REV_IS2000_REL_A)
    {
      /* Default the DYNAMIC_P_REV to 6 */
      mc_nv_data_buf.mob_cai_rev = P_REV_IS2000;
    }
    else

    {
      mc_nv_data_buf.mob_cai_rev = mc_mob_cai_rev;
    }

    mc_put_nv_item(NV_MOB_CAI_REV_I, &mc_nv_data_buf);
    M1X_MSG( DCP, LEGACY_HIGH,
      "MOB_P_REV is set to %d",
      mc_nv_data_buf.mob_cai_rev);
  }
  else
  {
    if (mc_nv_data_buf.mob_cai_rev < mc_mob_cai_rev)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "MOB_P_REV is downgraded to %d from %d",
        mc_nv_data_buf.mob_cai_rev,
        mc_mob_cai_rev);
    }
  }
  #endif /* FEATURE_IS2000_REL_A */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_MOB_FIRM_REV_I */

  read_status = mc_get_nv_item(NV_MOB_FIRM_REV_I, &mc_nv_data_buf);
  if (read_status == NV_NOTACTIVE_S)
  {
    mc_nv_data_buf.mob_firm_rev = mc_mob_firm_rev;
    mc_put_nv_item(NV_MOB_FIRM_REV_I, &mc_nv_data_buf);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_MOB_MODEL_I */

  read_status = mc_get_nv_item(NV_MOB_MODEL_I, &mc_nv_data_buf);
  if (read_status == NV_NOTACTIVE_S)
  {
    mc_nv_data_buf.mob_model = (byte) mc_mob_model;
    mc_put_nv_item(NV_MOB_MODEL_I, &mc_nv_data_buf);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NV_CONFIG_CHKSUM_I */

  read_status = mc_get_nv_item(NV_CONFIG_CHKSUM_I, &mc_nv_data_buf);
  if ((read_status == NV_NOTACTIVE_S) ||
      (mc_nv_data_buf.config_chksum.chksum != 0xFFFF)) {
    mc_nv_data_buf.config_chksum.chksum = 0xFFFF;
    mc_put_nv_item(NV_CONFIG_CHKSUM_I, &mc_nv_data_buf);
  }

} /* mc_init_items_to_nv */

/*===========================================================================

FUNCTION MC_INIT_RAN

DESCRIPTION
  This function initializes the pseudo-random number generator by using MS
  ESN as the seed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void mc_init_ran (void)
{
  nv_item_type        mc_nv_data_buf; /* NV data buffer */
  nv_stat_enum_type   read_status;    /* NV read status */
  dword               seed;           /* seed to random number generator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read MS ESN from NV and use it as seed to random number generator */
  /* Set seed to 1 if NV reading fails                                   */

  read_status = mc_get_nv_item( NV_ESN_I, &mc_nv_data_buf );

  if (read_status == NV_DONE_S) {
    seed = (mc_nv_data_buf.esn.esn)?mc_nv_data_buf.esn.esn : 1;
  }
  else {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Bad NV read status %d on NV_ESN_I",
      read_status );
    seed = 1;
  }

  ran_seed (seed);

} /* mc_init_ran */

/*===========================================================================

FUNCTION MC_CACHE_PREF_RC_FROM_NV

DESCRIPTION
  This function retrieves the pref RC from NV and stores
  it in SNM

DEPENDENCIES
  The NV task is running.

RETURN VALUE
  None.

SIDE EFFECTS
  This function calls mc_get_nv_item which suspends all MC task
  activities, except for watchdog kicking, while waiting for
  completion of read from NV.

===========================================================================*/

LOCAL void mc_cache_pref_rc_from_nv( void )
{

  nv_item_type          mc_nv_data_buf;     /* NV data buffer */
  nv_stat_enum_type     read_status;        /* NV read status */
  byte pref_fwd_fundicated_rc;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  read_status = mc_get_nv_item( NV_PREF_FOR_RC_I, &mc_nv_data_buf );
  if (read_status == NV_DONE_S)
  {
    pref_fwd_fundicated_rc = (byte)mc_nv_data_buf.pref_for_rc;
    snm_set_user_pref_fundicated_rc((cai_radio_config_type) pref_fwd_fundicated_rc );
  }

} /* mc_cache_pref_rc_from_nv */

/*===========================================================================

FUNCTION MC_GET_ENTRY

DESCRIPTION
  The function copies the last CDMA and Analog call processing entry reason
  to specified input structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.
===========================================================================*/

void mc_get_entry
(
  mcc_status_type *entry
)
{
  /*lint -e668 */
  *entry = last_entry;
  /*lint +e668 */

} /* mc_get_entry */

/*===========================================================================

FUNCTION MC_UPDATE_LAST_ENTRY

DESCRIPTION
  This procedure updates the last CDMA and Analog entry reason based on the
  current CDMA and Analog entry reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_update_last_entry(void)
{
  /* Update last CDMA entry reason */
  if (cdma_entry.entry == MCC_ENTER_OFFLINE)
  {
    last_entry.cdma = MCC_OFFLINE;
  }
  else
  {
    last_entry.cdma = MCC_ONLINE;
  }

} /* mc_update_last_entry */

/*===========================================================================

FUNCTION MC_UPDATE_LAST_CDMA_ENTRY

DESCRIPTION
  This procedure updates the last CDMA entry reason based on the input
  parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_update_last_cdma_entry
(
  mcc_entry_reason_type entry_reason
)
{
  if ((entry_reason == MCC_OFFLINE) || (entry_reason == MCC_ONLINE))
  {
    last_entry.cdma = entry_reason;
  }

} /* mc_update_last_cdma_entry */

/*===========================================================================

FUNCTION MC_SET_DIGITAL_ACQUISITION

DESCRIPTION
  This procedure sets up the digital acquisition parameters based on
  suggested SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_set_digital_acquisition
(
  mcc_entry_name_type entry_name
)
{
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(entry_name == MCC_CSFB)
  {
    /* Update only the entry variable.*/
    cdma_entry.entry = entry_name;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    sd_ss_act_s_type* ss_act_param_ptr; /* Pointer to SS-Action parameters */

    ss_act_param_ptr = (sd_ss_act_s_type*) modem_mem_alloc
                                            (sizeof(sd_ss_act_s_type),
                                             MODEM_MEM_CLIENT_1X );

    if (ss_act_param_ptr == NULL)
    {
      ERR_FATAL("Memory allocation failure!", 0, 0, 0);
    }
    else
    {
      /* ----------------------------------
      ** Set up CDMA acquisition parameters
      ** ---------------------------------- */
      (void) sd_ss_act_get(ss_act_param_ptr);
      ASSERT( ss_act_param_ptr->act == SD_SS_ACT_ACQ_CDMA);

      cdma_entry.entry = entry_name;
      cdma_entry.gen.block_or_sys = ss_act_param_ptr->prm.acq_cdma.blksys;
      cdma_entry.gen.band_class = ss_act_param_ptr->prm.acq_cdma.band; /*lint !e732 !e641 */
      cdma_entry.gen.cdma_channel = ss_act_param_ptr->prm.acq_cdma.chan;
      cdma_entry.gen.new_mode_or_blksys =
        ss_act_param_ptr->prm.acq_cdma.is_new_mode_or_blksys;

      #ifdef FEATURE_IS2000_REL_B
      /* MMOC activates 1x protocol, not enter Idle froM TC. */
      cdma_entry.gen.enter_idle_from_tc = FALSE;
      #endif /* FEATURE_IS2000_REL_B */

      modem_mem_free(ss_act_param_ptr, MODEM_MEM_CLIENT_1X);
    }

    /* ----------------------------------------------------------------------
    ** Update last CDMA and Analog entry reason based on the current CDMA and
    ** Analog entry reason.
    ** ---------------------------------------------------------------------- */
    mc_update_last_entry();
  }  /* if(entry_name == MCC_CSFB) */
} /* mc_set_digital_acquisition */

/*===========================================================================

FUNCTION MC_INFORM_DIAG_OFFLINE_COMPLETE

DESCRIPTION
  This function updates the last digital/analog entry reason based on the
  current digital/analog entry reason and signals DIAG task that we are
  ready to begin offline operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_inform_diag_offline_complete
(
  void
)
{
  /* ----------------------------------------------------------------------
  ** Update last CDMA and Analog entry reason based on the current CDMA and
  ** Analog entry reason.
  ** ---------------------------------------------------------------------- */
  mc_update_last_entry();

} /* mc_inform_diag_offline_complete */

/*===========================================================================

FUNCTION MC_SET_DIGITAL_OFFLINE

DESCRIPTION
  This procedure sets up the digital offline parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_set_digital_offline
(
  void
)
{
  cdma_entry.entry = MCC_ENTER_OFFLINE;

  /* The CDMA channel of the entry structure is zero if we never went online
  ** or it is the last CDMA channel used in online mode. Either way it does
  ** not need to be changed here.
  */
  /* Set the band class to PCS if it is supported, otherwise assume cellular
  ** band is supported. This assumes we support CDMA on at least one band.
  ** For tri-mode, a new offline command is needed.
  */
  /*lint -e506 */

  cdma_entry.gen.band_class = cdma.band_class;
} /* mc_set_digital_offline */

/*===========================================================================

FUNCTION  mc_map_ss_action_to_deact_reason

DESCRIPTION
   This function will map the SD action to the deactivate reason based on the
   current state of MC.

DEPENDENCIES
  None.

RETURN VALUE
  PROT_DEACT_MAX - do not deactivate the current protocol selection,
  others         - deactivate the protocol.

SIDE EFFECTS
  This function will initialize the acquisition parameters for analog/
  digital if the curr_state is not changed.
===========================================================================*/

static prot_deact_e_type mc_map_ss_action_to_deact_reason
(
  mc_state_type curr_state,
    /* Current state of Mode Controller. */

  sd_ss_act_e_type ss_action
    /* SS-Action.  */
)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Switch on the next sd action.
  */
  switch (ss_action)
  {
    /* Continue in current state */
    case SD_SS_ACT_CONTINUE:
      break;

    /* Go to CDMA online state */
    case SD_SS_ACT_ACQ_CDMA:
      if ( curr_state != MC_ONLINE_DIGITAL_STATE )
      {
        deact_reason = PROT_DEACT_ACQ_OTHERS;
      }
      else if ( cdma_entry.entry == MCC_PSEUDO_IDLE )
      {
        /* Since MMOC did not request to change idle mode, contine in
           pseudo-idle mode. */
        mc_set_digital_acquisition( MCC_PSEUDO_IDLE );
      }
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      else if( (cdma_entry.entry == MCC_LTE_1X_REDIR) ||
                (cdma_entry.entry == MCC_LTE_1X_RESEL) ||
                (cdma_entry.entry == MCC_CSFB)
              )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: In CSFB/REDIR/RESEL mode");
        mc_set_digital_acquisition( cdma_entry.entry );
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      else
      {
        mc_set_digital_acquisition( MCC_ACQUISITION );
      }
      break;

    #ifdef FEATURE_GPSONE_OOS
    /* Go to GPS state */
    case SD_SS_ACT_ACQ_GPS:
      deact_reason = PROT_DEACT_ACQ_OTHERS;
      break;
    #endif /* FEATURE_GPSONE_OOS */

    case SD_SS_ACT_ACQ_GW:
    case SD_SS_ACT_ACQ_GWL:
    case SD_SS_ACT_ACQ_HDR:
//  case SD_SS_ACT_PWR_SAVE:
    case SD_SS_ACT_GET_NET_GW:
      deact_reason = PROT_DEACT_ACQ_OTHERS;
      break;

    case SD_SS_ACT_MEAS_BACK:
    case SD_SS_ACT_MEAS_DED:
      if ( curr_state != MC_MEAS_DED_STATE )
      {
        deact_reason = PROT_DEACT_ACQ_OTHERS;
      }
      break;

  case SD_SS_ACT_PWR_SAVE:
      deact_reason = PROT_DEACT_ACQ_OTHERS;
      break;

  case SD_SS_ACT_REJECT:
      deact_reason = PROT_DEACT_ACQ_OTHERS;
      break;
	  
    default:
      M1X_MSG( DCP, LEGACY_MED,
        "Invalid SS-Action: %d", ss_action );
      break;

  } /* end switch (ss_action) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "deact_reason %d SS-Action %d",
    deact_reason,
    ss_action);
  return deact_reason;

} /* mc_map_ss_action_to_deact_reason */

/*===========================================================================

FUNCTION MC_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_dog_report ( void )
{
  dog_hb_report(mc_dog_rpt_id);
  (void) rex_clr_sigs( MC_TCB_PTR, MC_DOG_RPT_SIG );

} /* mc_dog_report */


/*===========================================================================

FUNCTION MC_INIT_AUTH

DESCRIPTION
  This function sends a command to the authentication task to initialize.
  It should be called on powerup or NAM change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void mc_init_auth
(
  void
)
{
  auth_cmd_type *auth_cmd_ptr; /* Command to AUTH */

  M1X_MSG( DCP, LEGACY_MED,
    "Begin AUTH Initialization");

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get(&auth_free_q)) != NULL)
  {
    /* Initialize authentication variables
     */
    /*lint -e734 */
    auth_cmd_ptr->hdr.command = AUTH_INIT_F; /*lint !e734 */
    auth_cmd_ptr->init.nam = cm_get_curr_nam(); /*lint !e641 !e732 */
    /*lint +e734 */

    /* Set flag so AUTHR will be recalculated for registrations
     */
    cdma.authr_valid = FALSE;

    /* Fill in necessary fields of command block
     */
    auth_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &auth_free_q;
    auth_cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
    auth_cmd_ptr->hdr.cmd_hdr.sigs = MC_CMD_SIG;

    (void) rex_clr_sigs(MC_TCB_PTR, MC_CMD_SIG);
    auth_cmd(auth_cmd_ptr);

    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    /* Mark that a command is in queue in auth task */
    cdma.sim_busy.is_auth_pending_in_queue = TRUE;
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

    (void) mc_wait(MC_CMD_SIG);

    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    /* Mark that command processing has begun in auth task */
    cdma.sim_busy.is_auth_pending_in_queue = FALSE;
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

    (void) rex_clr_sigs(MC_TCB_PTR, MC_CMD_SIG);

    if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "AUTH Initialized on powerup");
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad status AUTH_INIT_F %d",
        auth_cmd_ptr->hdr.status);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on auth_free_q");
  }
} /* mc_init_auth */


/*===========================================================================

FUNCTION mc_process_mc_command.

DESCRIPTION
  The function processes commands received from MC command queue when neither
  CDMA nor AMPS protocol are active.

  Note! It is assumed that reselection is allowed when this function is
  invoked.

DEPENDENCIES
  None.

RETURN VALUE
  PROT_DEACT_MAX - current protocol selection is ok.
  Other -  Current protocol stack should be deactivated.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_deact_e_type  mc_process_mc_command
(
  mc_state_type curr_state,
    /* Current state of MC. */

  prot_trans_type *deact_transid_ptr
    /* Pointer to the mmoc's trans Id. To be filled in with
    ** the MMoC's trans Id if a deactivate protocol command
    ** was received.
    */
)
{
  /* pointer to the MC command. */
  mc_msg_type   *cmd_ptr;

  /* Deactivate reason. */
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  boolean is_cmd_ignored = FALSE;

  /* SD SS action. */
  sd_ss_act_e_type   sd_act;

  word old_state;

  if (deact_transid_ptr == NULL)
  {
    ERR_FATAL("deact_transid_ptr is NULL", 0,0,0);
    /* return deact_reason; */
  }

  /* Initialize the MMoC's transid. */
  else
  {
  *deact_transid_ptr = PROT_TRANS_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_state = cdma.curr_state;
  /* Check if there is any command in the queue.  */
  if (((cmd_ptr = (mc_msg_type*) q_check(&mc_cmd_q)) != NULL) &&
       (deact_transid_ptr != NULL) )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Rxed cmd = %d in state = %d",
      cmd_ptr->hdr.cmd,
      cdma.curr_state);
    switch (cmd_ptr->hdr.cmd)
    {
      case MC_MO_DBM_F :
        /* If an MO DBM request is received while we are in SD,
           we reject the request */
        M1X_MSG( DCP, LEGACY_MED,
          "Rejecting MO DBM request in SD" );
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        if(cmd_ptr != NULL)
        {
          mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                  CM_DBM_MO_NO_SRV
                                );
        }
        break;



      case  MC_BCSMS_ENABLE_F:
        M1X_MSG( DCP, LEGACY_MED,
          "Rxed BCSMS_ENABLE Cmd %d not in CDMA",
          cmd_ptr->hdr.cmd );
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        if(cmd_ptr != NULL)
        {
          mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
        }
        break;


      case MC_CDMA_PROT_GEN_CMD_F:
      case MC_AMPS_PROT_GEN_CMD_F:
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        /* It is assumed that reselection is allowed when this function
        ** is invoked.  */
        if(cmd_ptr != NULL)
        {
          /* Additional range check added to appease klocwork. */
          if(cmd_ptr->gen_cmd.info.ss < SD_SS_MAX)
          {
            sd_act =  mmoc_proc_prot_gen_cmd( TRUE, TRUE, &cmd_ptr->gen_cmd.info );
            mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_MC_COMMAND, MMOC_RPT_PROT_GEN_CMD_CNF,
                                      cmd_ptr->gen_cmd.info.trans_id);
            deact_reason = mc_map_ss_action_to_deact_reason ( curr_state, sd_act );
          M1X_MSG( DCP, LEGACY_MED,
              "Rcv Gen Cmd deact_reason=%d",
              deact_reason );
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR, 
                  "PROT_GEN_CMD : System Selection cannot be SD_SS_MAX" );
          }          
        }
        break;

      case MC_CDMA_PROT_DEACT_CMD_F:
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
        /* PPP_CLEANUP_NON_SV_MODE:Check if deregister powerup reg is pending
         * If yes, then set the flag to do powerup reg
         * with next protocal activate cmd */
        if( cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING )
        {
          /* Call dereg timer expiry API, which will take care of setting
           * flag to do powerup reg with next protocal activate cmd */
          mcc_proc_deregister_timer_expiry();
        } /* cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

      case MC_AMPS_PROT_DEACT_CMD_F:
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        if(cmd_ptr != NULL)
        {
          deact_reason = cmd_ptr->deact_prot.info.reason;
          *deact_transid_ptr = cmd_ptr->deact_prot.info.trans_id;
          M1X_MSG( DCP, LEGACY_MED,
            "Rcv Gen Cmd deact_reason=%d",
            deact_reason );
        }
        break;

      case MC_UE_MODE_CHGD_CMD_F:
      {
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        if(cmd_ptr != NULL)
        {
          /* Update cdma UE mode global var's */
          cdma.ue_mode = cmd_ptr->ue_mode_cmd.ue_mode;
          cdma.is_ue_mode_substate_srlte = 
                              cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

          /* Send UE Mode cnf to MMOC */
          mmoc_1x_rpt_ue_mode_switch_cnf(cmd_ptr->ue_mode_cmd.trans_id, mcc_asid);

          /* Keep track of the MMOC cnf for debugging purpose */
          mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_MC_COMMAND, 
                                    MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                    cmd_ptr->ue_mode_cmd.trans_id );
          M1X_MSG( DCP, LEGACY_MED,
            "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
                   cmd_ptr->ue_mode_cmd.ue_mode,
                   cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
                   cmd_ptr->ue_mode_cmd.trans_id,
                   mcc_asid );
        } /* (cmd_ptr != NULL) */
      
        break;
      } /* MC_UE_MODE_CHGD_CMD_F */
      
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
      case MC_DEREGISTER_F:
      {
        /* PPP_CLEANUP_NON_SV_MODE: Reject deregister cmd while we are in SD */
        M1X_MSG( DCP, LEGACY_MED,
          "Rejecting deregister_f cmd in SD");
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        if(cmd_ptr != NULL)
        {
          cdma.deregister_state = MCC_DEREGISTER_RSP_PENDING;
          mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "mc_cmd_q returned NULL cmd_ptr");
        }
        break;
      } /* MC_DEREGISTER_F */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_1XCP_QSH_SUPPORT
      case MC_QSH_CB_CMD_F:
      {         
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        if(cmd_ptr != NULL)
        {
          mc_process_qsh_cmd(&cmd_ptr->qsh_cmd.qsh_params);
        }                    
        break;
      }
#endif /* FEATURE_1XCP_QSH_SUPPORT */
              
      default :
        /* In all the other cases, remove command from the Queue */
        cmd_ptr = (mc_msg_type *) q_get(&mc_cmd_q);
        is_cmd_ignored = TRUE;
        if(cmd_ptr != NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Received unexpected MC command %d, ",
            cmd_ptr->hdr.cmd );
        }
        break;

     } /* end switch (cmd_ptr->hdr.cmd) */

    if (cmd_ptr != NULL)
    {
      cmd_ptr->hdr.status = MC_DONE_S;
      /* Keep track of the MMOC cmd for debugging purpose */
      mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state, 
                                is_cmd_ignored, MC_PROCESS_MC_COMMAND );      
      mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );

    } /* end if (cmd_ptr != NULL) */

  } /* end if ((cmd_ptr = (mc_msg_type*) q_check(&mc_cmd_q)) != NULL) */

  return deact_reason;

} /* mc_process_mc_command */

/*===========================================================================

FUNCTION MC_ONLINE_DIGITAL_STATE

DESCRIPTION
  The entrance procedure to the Online Digital State of MC which determines
  what need to be processed and vectors control to the appropiate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The reason for leaving CDMA mode, and the MMoC Transaction ID.

SIDE EFFECTS
  None.
===========================================================================*/

static prot_deact_e_type mc_online_digital_state
(
  prot_trans_type *mmoc_trans_id_ptr /* Return the MMoC transaction ID */
)
{
  prot_deact_e_type deact_reason = PROT_DEACT_MAX;

  mcc_exit_type    cdma_exit_info;    /* CDMA call processing exit parameter */

  M1X_MSG( DCP, LEGACY_HIGH,
    "Going online digital");

  /* Initialize MMOC trans_id and psm_ready_ind_pending flag */
  cdma_exit_info.mmoc_trans_id = PROT_TRANS_NONE;
  cdma.psm_ready_ind_pending = FALSE;

  while ( deact_reason == PROT_DEACT_MAX )
  {
    /* Before entering the MCC subtask, record the last entry reason */
    cdma_exit_info = *mcc_subtask(&cdma_entry);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    M1X_MSG( DCP, LEGACY_HIGH, "Exit Digital Mode:%d, Entry Type:%d",
             cdma_exit_info.exit, cdma_entry.entry );
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma_entry.entry == MCC_CSFB)
    {
      /* We are here because we received CSFB deact in CSFB mode, hence exit
       * from here without any further consulting with SD hence hard code the
       * deact_reason to some thing other than PROT_DEACT_MAX.
       */
      deact_reason = PROT_DEACT_ACQ_OTHERS;
    }
    else
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      /* --------------------------------------------------------------------
       * Based on the exit status, call the function to determine the next
       * state. The called function will prepare the entry parameters if we
       * are to stay in digital, or it will return an exit state if the state
       * has changed to NOT online. In certain cases all we need to do is to
       * change state because we are exiting online all together.
       * -------------------------------------------------------------------*/
      switch (cdma_exit_info.exit)
      {
        case MCC_ACCESS_FAILED_SYS_LOST:
        case MCC_ACCESS_COMPLETED:
        case MCC_ACCESS_DENIED:
        case MCC_ACCESS_FAILED:
        case MCC_ACCESS_FAILED_GSRM:
        case MCC_ACCESS_TIMEOUT:
        case MCC_ACQ_FAILED:
        case MCC_INCOMP_CAI_REV :
        case MCC_INTERCEPT :
        case MCC_LOCK:
        case MCC_MAX_ACCESS_PROBES:
        case MCC_NEW_SYSTEM:
        case MCC_NDSS_OFF:
        case MCC_NOT_PREFERRED:
        case MCC_PREF_SYS_CHANGED :
        case MCC_REDIRECTION :
        case MCC_REG_REJECTED :
        case MCC_RELEASE:
        case MCC_REORDER:
        case MCC_RESCAN:
        case MCC_RESELECTION:
        case MCC_SYSTEM_LOST:
        case MCC_SYSTEM_RESELECTION :
        case MCC_UNLOCK:

        #ifdef FEATURE_IS2000_REL_A
        case MCC_ENCRYPTION_MI_FAILED:
        case MCC_ACCT_BLOCKED:
        #endif /* FEATURE_IS2000_REL_A */

        #if defined(FEATURE_IS2000_REL_A_AES)
        case MCC_ACCESS_STOPPED_MI_AES_RESYNC:
        #endif /* FEATURE_IS2000_REL_A_AES */

        case MCC_SRCH_REQUEST_SYS_RESTART:
        case MCC_SHUTDOWN:

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
        case MCC_PPP_CLEANUP:
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

          deact_reason = mc_map_ss_action_to_deact_reason(MC_ONLINE_DIGITAL_STATE,
                                                          sd_ss_act_get(NULL));
          break;

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        case MCC_IRAT_SWITCH_FAIL:
        if(cdma_entry.entry == MCC_LTE_1X_RESEL) 
        {
          /* As of now, MC has only one channel for the reselection scenario
           * MMODE expects MC to consider aborting the resel flow without 
           * consulting with SD. In future, for any further enhancements 
           * this logic would be pushed in SD. Hence, hardcoding the value 
           * to PROT_DEACT_ACQ_OTHERS now. */
          deact_reason = PROT_DEACT_ACQ_OTHERS;
        }
        else
        {
          deact_reason = mc_map_ss_action_to_deact_reason(MC_ONLINE_DIGITAL_STATE,
                                                          sd_ss_act_get(NULL));
        }
        break;
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */ 


        case MCC_NAM_CHANGED:
          deact_reason = PROT_DEACT_SUBSC_CHGD;
          break;

        case MCC_LPM:
          M1X_MSG( DCP, LEGACY_HIGH,
            "LPM");
          deact_reason = PROT_DEACT_LPM;
          break;

        case MCC_OFFLINE_DIGITAL:
          M1X_MSG( DCP, LEGACY_HIGH,
            "Offline digital");
          deact_reason = PROT_DEACT_OFFLINE;
          break;

        case MCC_POWERDOWN:
          M1X_MSG( DCP, LEGACY_HIGH,
            "Powerdown");
          deact_reason = PROT_DEACT_PWR_DOWN;
          break;

        case MCC_RESET:
          M1X_MSG( DCP, LEGACY_HIGH,
            "Reset");
          deact_reason = PROT_DEACT_RESET;
          break;

        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        case MCC_IRAT_SWITCH_ABORT:
          /* MCC_IRAT_SWITCH_ABORT common for REDIR/RESEL abort cases */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: IRAT abort");
          deact_reason = PROT_DEACT_REDIR_ABORT;
          break;
        
        case MCC_REDIR_ABORTED:
        {
          /* This exit status will be set if MMODE aborted the ongoing
           * REDIR Attempt. Mark deact rsn to PROT_DEACT_ACQ_OTHERS such
           * that we will send IRAT fail ind to LTE and Auto_deact ind 
           * to MMODE as well, similar to legacy IRAT failure case */
          deact_reason = PROT_DEACT_ACQ_OTHERS;          
          M1X_MSG( DCP, LEGACY_HIGH,
                   "DEBUG: IRAT abort by MMODE");
          break;
        } /* MCC_REDIR_ABORTED */
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Bad exit reason returned from MCC: %d",
            cdma_exit_info.exit);
          break;

      } /* end switch (mcsyspr_cdma_exit_info.exit) */

      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      /* Send IRAT FAIL/ABORT confirmation to LTE */
      if( ( (cdma_entry.entry == MCC_LTE_1X_REDIR) ||
            (cdma_entry.entry == MCC_LTE_1X_RESEL) ) &&
            (deact_reason != PROT_DEACT_MAX)
        )
      {
        mcc_csfb_send_irat_fail_ind(deact_reason);
        /* If we were redirected from CSFB mode for call reason, getback to
         * CSFB mode and wait for CSFB Call Clear from CM to further
         * clean up the call.
         */
        if(csfb_cdma.csfb_1x_transition == TRUE)
        {
          onex_cp_lte_1xcsfb_call_rel_ind_t call_rel_ind;

          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Failure in CSFB -> CDMA mode switch");
          /* Deactivate the lower layers first and then indicate failure to MC/RRC.
           * this is necessary to ensure that lower layers are deactivated properly
           * before Failure is indicated to upper layers
           */
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: Deactivating the lower layers");
          mcc_csfb_deactivate_lower_layers();

          /* Since we started Redirection transaction with MMOC, when we
           * sent Redirection Indication. MMOC after sending REDIR_CNF MMOC
           * will be waiting either for Auto Activation / Auto Deactivation
           * Indication to cancel the previously started Redirection transaction
           * at MMOC.
           */
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: sending MMOC AUTO DEACT IND");
          mmoc_rpt_prot_auto_deactd_ind(deact_reason,PROT_STATE_ONLINE_CDMA);
          mcc_enqueue_dbg_mmoc_cnf( MC_ONLINE_DIGITAL_STATE_API, MMOC_RPT_PROT_AUTO_DEACTD_IND,
                                    PROT_TRANS_NONE );
          /* Report Call failure indication to RRC. RRC will route call failure
           * indication to CM through NAS. This is needed for CM to clear call
           * context eventually.
           */
          /* Fill in the cause as per the ORDQ received */
          call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg(&call_rel_ind.hdr,
                           ONEX_CP_LTE_1xCSFB_CALL_REL_IND,
                           sizeof(call_rel_ind)
                          );
          /* Go to CSFB mode of operation */
          cdma.curr_state = CDMA_CSFB_EXIT;
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
          if(cdma.call_orig == TRUE)
          {
            mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_MO_1X_ACQ_FAIL;
          }
          else
          {
            mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_MT_1X_ACQ_FAIL;
          }
          /* Reset CSFB mode */
          mc_set_digital_acquisition(MCC_CSFB);
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: Exit CDMA TC..Enter CSFB Exit -> Access");
          deact_reason = PROT_DEACT_MAX;
        }
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    } /* end of cdma_entry.entry == MCC_CSFB */
  } /* end while (next_state == MC_ONLINE_DIGITAL_STATE) */

  /* If overhead optimizations are enabled then clear all the overheads
   * if 1x stack is deactivated due to Protocol deactive command from CM or
   * Power Save mode being entered due to Protocol generic command.
   * With overheads optimization, overheads will not be cleared for all other
   * exit reasons.
   */

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
  switch (cdma_exit_info.exit)
  {
    case MCC_SHUTDOWN:
    case MCC_NAM_CHANGED:
    case MCC_LPM:
    case MCC_OFFLINE_DIGITAL:
    case MCC_POWERDOWN:
    case MCC_RESET:
    {
      if ( mcc_is_opt_supported( STORE_OVHDS_ACROSS_STATES ) == TRUE )
      {
        /* Initialize the BS overhead info. */
        mcc_init_ovhd_info();  
        M1X_MSG(DCP, LEGACY_MED, 
             "Cleared ovhds while exiting online digital state" );
      }
           
      break;
    }
    default:
      M1X_MSG( DCP, LEGACY_MED, 
               "Retaining cached ovhds while exiting online digital" );
      break;
  }   
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

  *mmoc_trans_id_ptr = cdma_exit_info.mmoc_trans_id;

  return (deact_reason);

} /* mc_online_digital_state */

/*===========================================================================

FUNCTION MC_OFFLINE_DIGITAL_STATE

DESCRIPTION
  The entrance procedure to the Offline Digital State of MC which determines
  what need to be processed and vectors control to the appropiate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The reason for leaving CDMA mode, and the MMoC Transaction ID.

SIDE EFFECTS
  None.
===========================================================================*/

static prot_deact_e_type  mc_offline_digital_state
(
  prot_trans_type *mmoc_trans_id_ptr /* Return the MMoC transaction ID */
)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  mcc_exit_type   cdma_exit_info;  /* CDMA call processing exit parameter */


  M1X_MSG( DCP, LEGACY_HIGH,
    "Going offline digital");

  /* Initialize MMOC trans_id */
  cdma_exit_info.mmoc_trans_id = PROT_TRANS_NONE;

  /* Initialize next state of MC and CDMA entry parameter */
  mc_set_digital_offline();

  /* Initialize MMOC trans_id */
  cdma_exit_info.mmoc_trans_id = PROT_TRANS_NONE;

  /* ---------------------------------------------------------------------
   * Start an endless loop which first calls MCC with the entry reason.
   * Based on the exit reason we decide if we should loop again with a new
   * entry reason, or exit out. Exit is taken only when the new state is
   * not offline digital.
   * --------------------------------------------------------------------- */
  while (deact_reason == PROT_DEACT_MAX)
  {
    /* ---------------------------------------------------------------
     * Call the MCC subtask with the digital entry reason and copy the
     * exit reason when the function returns.
     * --------------------------------------------------------------- */
    cdma_exit_info = *mcc_subtask(&cdma_entry);


    /* -------------------------------------------------------------------
     * Check the exit status and call the appropriate function to
     * determine the next state. The called function will prepare the
     * entry parameters if we are to stay in digital, or it will return an
     * exit state if the state has changed to NOT offline.
     * ------------------------------------------------------------------- */
    switch (cdma_exit_info.exit)
    {
      /* -----------------------------------------------------------------
       * If these cases we just go right back into offline digital. These
       * orders arrive from the air interface or are the result of regular
       * channel operation and we continue in offline digital.
       * ----------------------------------------------------------------- */
      case MCC_ACCESS_COMPLETED:
      case MCC_ACCESS_DENIED:
      case MCC_ACCESS_FAILED:
      case MCC_ACCESS_FAILED_GSRM:
      case MCC_ACCESS_TIMEOUT:
      case MCC_ACQ_FAILED:
      case MCC_INCOMP_CAI_REV:
      case MCC_INTERCEPT:
      case MCC_LOCK:
      case MCC_MAX_ACCESS_PROBES:
      case MCC_NEW_SYSTEM:
      case MCC_NDSS_OFF:
      case MCC_NOT_PREFERRED:
      case MCC_REDIRECTION:
      case MCC_REG_REJECTED:
      case MCC_RELEASE:
      case MCC_REORDER:
      case MCC_RESCAN:
      case MCC_RESELECTION:
      case MCC_SYSTEM_LOST:
      case MCC_SYSTEM_RESELECTION:
      case MCC_UNLOCK:

      #ifdef FEATURE_IS2000_REL_A
      case MCC_ENCRYPTION_MI_FAILED:
      case MCC_ACCT_BLOCKED:
      #endif /* FEATURE_IS2000_REL_A */

      #if defined(FEATURE_IS2000_REL_A_AES)
      case MCC_ACCESS_STOPPED_MI_AES_RESYNC:
      #endif /* FEATURE_IS2000_REL_A_AES */

      case MCC_SRCH_REQUEST_SYS_RESTART:
      case MCC_SHUTDOWN:
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
      case MCC_PPP_CLEANUP:
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

        break;

      /* These cases are illegal and should not happen */
      case MCC_PREF_SYS_CHANGED:
      case MCC_NAM_CHANGED :
        deact_reason = PROT_DEACT_SUBSC_CHGD;
        break;

      case MCC_OFFLINE_DIGITAL:
        M1X_MSG( DCP, LEGACY_HIGH,
          "Offline digital");
        deact_reason = PROT_DEACT_OFFLINE;
        break;

      case MCC_POWERDOWN:
        M1X_MSG( DCP, LEGACY_HIGH,
          "Powerdown");
        deact_reason = PROT_DEACT_PWR_DOWN;
        break;

      case MCC_RESET:
        M1X_MSG( DCP, LEGACY_HIGH,
          "Reset");
        deact_reason = PROT_DEACT_RESET;
        break;


      /* Otherwise fatal fault */
      default :
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad exit reason returned from digital offline %d",
          cdma_exit_info.exit);
        break;

    } /* switch (cdma_exit_info.exit) */
  } /* while (deact_reason == PROT_DEACT_MAX) */

  *mmoc_trans_id_ptr = cdma_exit_info.mmoc_trans_id;
  return (deact_reason);

} /* mc_offline_digital_state */

/*===========================================================================

FUNCTION MC_MEAS_DED_STATE

DESCRIPTION
  The entrance procedure to the Dedicated Mode of Measurement state of MC
  which determines what need to be processed and vectors control to the
  appropiate procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The reason for leaving CDMA mode, and the MMoC Transaction ID.

SIDE EFFECTS
  None.
===========================================================================*/

static prot_deact_e_type mc_meas_ded_state
(
  prot_trans_type *mmoc_trans_id_ptr /* Return the MMoC transaction ID */
)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;

  mccsrch_rpt_type *rpt_ptr;   /* Pointer to SRCH report */
  dword            sigs;       /* Signals returned by mc_wait() */
  dword            mc_wait_mask;
   /* This mask is used to set various signals on which Mobile waits */
  boolean          new_sys_meas_req = TRUE;
   /* Flag indicates whether it is a new system measurement request */

  M1X_MSG( DCP, LEGACY_MED,
    "Enter MC Meas Ded state");

  /* Clean off queue to start */
  while ((rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL)
  {
    /* Keep track of the SRCH report for debugging purpose */
    mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, cdma.curr_state, 
                cdma.curr_state, MCC_NON_SLEEP, SRCH_MC_MEAS_DED_STATE_1 );    

    cmd_done(&rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  /* wait for UI command or system measurement report */
  mc_wait_mask = MCC_SRCH_Q_SIG | MCC_CMD_Q_SIG;

  while ( deact_reason == PROT_DEACT_MAX )
  {
    /* Send system measurement command to SRCH */
    if (new_sys_meas_req)
    {
      new_sys_meas_req = FALSE;
      /* Search rejected system measurement request */
      if (!mcc_sys_meas_cmd())
      {
        /* Based on SS-Action, determines the next state of MC */
        if ((deact_reason = mc_map_ss_action_to_deact_reason(MC_MEAS_DED_STATE,
            sd_ss_act_get(NULL))) == PROT_DEACT_MAX)
        {
          /* Another system measurement request */
          new_sys_meas_req = TRUE;
        }
      }
    }
    /* wait for UI command or measurement report */
    else
    {
      sigs = mcc_wait(mc_wait_mask);

      /* Process MC command */
      if (sigs & MCC_CMD_Q_SIG)
      {
        deact_reason = mc_process_mc_command(MC_MEAS_DED_STATE, mmoc_trans_id_ptr);
      }
      /* Process Search report */
      else if ((rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL)
      {
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, cdma.curr_state, 
                    cdma.curr_state, MCC_NON_SLEEP, SRCH_MC_MEAS_DED_STATE_2 );  
        
        switch (rpt_ptr->hdr.rpt)
        {
          case SRCH_SYS_MEAS_R:
            /* Report system measurements to System Determination */
            mcc_sys_meas_rpt(rpt_ptr);
            /* --------------------------------------------------------
            ** Inform system determination of finished reporting system
            ** measurement. Based on SS-Action, determines the next
            ** state of MC to be processed.
            ** -------------------------------------------------------- */
            if ((deact_reason = mc_map_ss_action_to_deact_reason(MC_MEAS_DED_STATE,
              sd_ss_ind_misc_meas_rprt(rpt_ptr->sys_meas.meas_id, TRUE,
              NULL))) == PROT_DEACT_MAX)
            {
              /* Another system measurement request */
              new_sys_meas_req = TRUE;
            }
            cmd_done(&rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
            break;

         default:
           M1X_MSG( DCP, LEGACY_MED,
             "Unexpected srch rpt %d",
             rpt_ptr->hdr.rpt);
           cmd_done(&rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
           mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
           break;

        } /* end switch (rpt_ptr->hdr.rpt) */

      } /* end if (sigs & MCC_CMD_Q_SIG) */

    } /* end if (new_sys_meas_req) */

  } /* end while (next_state = MC_MEAS_DED_STATE) */

  return (deact_reason);

} /* mc_meas_ded_state */

/*===========================================================================

FUNCTION MC_INIT_BEFORE_TASK_START()

DESCRIPTION
  This function initializes CDMA/AMPS related data structure before task
  starts.

DEPENDENCIES
  Perform MC initialization that needs to be done before tasks have started.

  NOTE! Any initialization that result in MC contacting some other tasks,
  such as NV, must be done in sd_init_after_task_start(), as opposed to
  this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mc_init_before_task_start(void)
{

  /* Initialize the critical section variable */
  rex_init_crit_sect(&mc_crit_sect);
  rex_init_crit_sect(&mc_reg_crit_sect);

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
  /* The mutex mc_audio_crit_sect may have been initialized earlier in CM 
   * or Audio task context even before MC task is up. So initialize the 
   * mutex here only if it has not already been initialized. 
   */
  if (!is_audio_mutex_initialized)
  {
    rex_init_crit_sect(&mc_audio_crit_sect);
    is_audio_mutex_initialized = TRUE;
  }
#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

  /* Init task timers/queues */
  mc_queue_init();

  /* Initialize Code Channel List object.*/
  mccccl_initialize();

  /* Initialize service negotiation manager */
  snm_initialize_snm();

  /* Initialize mccsyobj_system object.*/
  mccsyobj_initialize();

  #if defined(FEATURE_IS2000_REL_C)
  /* Initialize SYNC_ID manager */
  mccsyncid_powerup_init();
  #endif /* FEATURE_IS2000_REL_C */

  /* Initialize the switches DAL */
  cdma2kdiag_switch_init();

}/* mc_init_before_task_start */

/*===========================================================================

FUNCTION MC_TASK_INIT

DESCRIPTION
  This function initializes the tcb pointer and other pre initializations that
  needs to be done only after TCB_PTR is updated. This function also initializes
  CDMA/AMPS related data structure after task starts.

DEPENDENCIES
  Must be called from "mc" task context.
  mc_init_before_task_start has to be called before this function is called.
  nv_task has started.


RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mc_task_init(void)
{

#ifndef DOG_DYNAMIC_API
  #error DOG_DYNAMIC_API is not defined!!!
#endif /* DOG_DYNAMIC_API */

  /* Update the MC TCB pointer */
  MC_TCB_PTR = rex_self();

  /* Power-up initialization */
  (void) mcc_powerup_init();

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(mc_is_msgr_registered != TRUE)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "CSFB DEBUG : Start initializing MSGR");
    mc_msgr_init();
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

   /* Clear all signals */
  (void) rex_clr_sigs( MC_TCB_PTR, (rex_sigs_type) ~0 );

  /* Initialize DOG report ID and DOG report timeout */
  mc_dog_rpt_id = dog_hb_register_rex( (rex_sigs_type) MC_DOG_RPT_SIG );

  /* Initialize database services */
  db_init();

  /* Initialize registration services */
  mccreg_init();

  /* Generate the "conf" files for MC related EFS item files */
  mc_efs_init();

#ifdef FEATURE_IS2000_1X_ADV
  /* Read EFS for 1xAdvanced capability status */
  (void) mc_efs_get(MC_EFS_1XADVANCED_CAPABILITY_STATUS, &mc_efs_data);
  cdma.onexadv_status.status = mc_efs_data.onexadv_capability_status;
  cdma.onexadv_status.rc_bitmap_capability_ext_allowed =
    cdma.onexadv_status.status;

  M1X_MSG( DCP, LEGACY_MED,
    "1xAdvanced status %d",
    cdma.onexadv_status.status);
#endif /* FEATURE_IS2000_1X_ADV */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS ) || \
    defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 )
  (void) mc_efs_get(MC_EFS_SMS_OVER_S102, &mc_efs_data);
  csfb_cdma.mcc_csfb_sms_suported_over_s102 =
    mc_efs_data.mcc_csfb_sms_suported_over_s102;

  M1X_MSG( SRCH, LEGACY_MED,
    "mcc_csfb_sms_suported_over_s102 %d",
    csfb_cdma.mcc_csfb_sms_suported_over_s102 );
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS || 
          FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */

#ifdef FEATURE_1X_CUST_VOICE_SO
  /* Read EFS to determine value of customer specific EVRC based voice SO */
  (void) mc_efs_get(MC_EFS_CUST_EVRC_VOICE_ENCRYPT_SO, &mc_efs_data);
  /* Set the cust SO here based on NV 73570. NV value is set directly as cust
   * SO and some validations are performed on NV value in mcc_set_cust_so */
  mcc_set_cust_so(mc_efs_data.cust_voice_encrypt_so);
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Customer specific voice SO:%d",
    mc_efs_data.cust_voice_encrypt_so);
#endif /* FEATURE_1X_CUST_VOICE_SO */

  /* Initialize the service options */
  mccso_initialize();

  /* Write MC parameter from local variable to NV */
  mc_init_items_to_nv();

  /* Retrieve the preference RC from NV and stores */
  mc_cache_pref_rc_from_nv();

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Initialize sim busy status as False */
  mc_set_sim_busy_status(FALSE);
#endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

#ifdef FEATURE_CSIM
  cdma.client_id = MC_INVALID_CDMA_CLIENT_ID;
  cdma.session_id = MC_INVALID_CDMA_SESSION_ID;

  /* When the phone powers up, every MMGSDI clients will try to register with
   * UIM, placing their requests queued at UIM. UIM will process all these
   * requests after it gets response for TP(terminal profile). UIM needs
   * TP information to know UI's capability and Phone's capability.
   * TP responses can come to UIM in a range of 2-15 sec. If we make the
   * client ID registration a blocking call ,then we might not be processing
   * the commands from MMOC leading to a sanity timeout. Hence made the
   * client id registration an async ones.
   */
  M1X_MSG( DCP, LEGACY_MED,
    "Performing Client ID registration with MMGSDI");
  if ( mmgsdi_client_id_and_evt_reg( 
                                     #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
                                     mc_mmgsdi_evt_cb,
                                     #else
                                     NULL,
                                     #endif
                                     mcmmgsdi_client_id_reg_cb,
                                     0) != MMGSDI_SUCCESS )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Client ID reg with MMGSDI failed" );
  }
#endif /* FEATURE_CSIM */

#ifdef FEATURE_MODEM_CONFIG_REFRESH

  /* Refresh order ID registered for refresh */
  mcfg_refresh_registration.order  = MCFG_REFRESH_ORDER_50;
  /* Mask for slot(s) to recieve refresh update */
  mcfg_refresh_registration.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  /* Mask for sub(s) to recieve refresh update */
  mcfg_refresh_registration.sub_mask = MCFG_REFRESH_INDEX_MASK_0;
  /* Group Info sent in refresh update */
  mcfg_refresh_registration.group = 0;
  /* Callback function to receive refresh update */
  mcfg_refresh_registration.cb = mc_mcfg_evt_cb;

  if ( mcfg_refresh_register( &mcfg_refresh_registration) != TRUE )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Refresh Indication registration with MCFG failed" );
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_1X_CUST_VOICE_SO
  /* Register the callback functions for customer specific algorithms required
   * for customer specific voice service option */
  mcc_reg_encrypt_decrypt_cb( NULL, 
                              NULL
                            );   
#endif /* FEATURE_1X_CUST_VOICE_SO */

  /* Initialize the pseudo-random number generator. */
  mc_init_ran();

  /* Initialize the Dynamic Feature items. */
  mccap_df_init();

  /* Initialize the Simple-TDSO service option, This is a one time
  ** Initialization with the SNM */
  tdso_init_so_cfg();

  /*  Initialize action_store data structure to NULL. This holds pointers
      to action handler functions registered by RXC, TXC and MUX.
  */
  mctrans_init_action_handlers();

  /* Initialize CDMA stats */
  mcc_stats_init();

  #ifdef FEATURE_1XCP_QSH_SUPPORT
  /* Register MC task with QSH module and perform necessary initializations */
  mc_qsh_init();
  #endif /* FEATURE_1XCP_QSH_SUPPORT */

  /* Initialize 1X2G Tune Away Feature items */
  mcc_ta_init();

  /* Initialize the Jcdma service to MC_JCDMA_SERVICE_DISABLED */
  cdma.jcdma_support.service = MC_JCDMA_SERVICE_DISABLED;

  /* Initialize old BS info required after handoff as INVALID */
  mcc_old_bs_nghbr_info.pilot_pn = INVALID_PN;

  /* Read EFS for JCDMA support */
  (void) mc_efs_get(MC_EFS_JCDMA_SUPPORT, &mc_efs_data);
  cdma.jcdma_support.service = mc_efs_data.jcdma_support.service;
  M1X_MSG( DCP, LEGACY_MED,
    "jcdma_support = %d",
    mc_efs_data.jcdma_support.service);

  #ifdef FEATURE_GPSONE
  /* Read EFS for SO35 and SO36 disable status */
  (void) mc_efs_get(MC_EFS_DISABLE_SO35_SO36, &mc_efs_data);
  cdma.disable_so35_so36 = mc_efs_data.disable_so35_so36;
  M1X_MSG( DCP, LEGACY_MED,
    "disable_so35_so36 = %d",
    mc_efs_data.disable_so35_so36);

  /* Initialize the Position Determination Services */
  mcc_pd_init();
  #endif /* FEATURE_GPSONE */

  /* Read the system capability for SO73 COP0 support */
  mccap_set_so73_op0_sup_cap();

  /* Initialize overheads refresh timer to spec defined value i.e 10 mins */
  cdma.ovhd_refresh_time = MCC_VALID_CONFIG;
  
  /* Read EFS for support for reg after page drop because of DDTM */
  (void) mc_efs_get(MC_EFS_REG_AFTER_DDTM_PAGE_DROP_SUPPORTED, &mc_efs_data);
  reg_after_ddtm_page_drop_supported = mc_efs_data.reg_after_ddtm_page_drop_supported;
  M1X_MSG( DCP, LEGACY_MED,
    "reg_after_ddtm_page_drop_supported = %d",
    reg_after_ddtm_page_drop_supported);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* Read EFS for C2K Params Timeout */
  (void) mc_efs_get(MC_EFS_1xCSFB_T_MAX_NO_C2K_PARAM, &mc_efs_data);
  csfb_cdma.max_c2k_param_rx_timeout = mc_efs_data.onexcsfb_c2k_param_timeout;
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: C2K Params timeout = %d",
    csfb_cdma.max_c2k_param_rx_timeout);

  /* Read EFS for Max Time to be without voice */
  (void) mc_efs_get(MC_EFS_1xCSFB_T_MAX_NO_VOICE, &mc_efs_data);
  csfb_cdma.max_no_voice_wait_timeout = mc_efs_data.onexcsfb_no_voice_timeout;
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: Max no voice Timeout %d",
    csfb_cdma.max_no_voice_wait_timeout);

  /* Read n max reg attempts count */
  (void) mc_efs_get(MC_EFS_1XCSFB_N_MAX_REG_RETRY_CNT, &mc_efs_data);
  csfb_cdma.max_soft_reg_retry_cnt = mc_efs_data.onexcsfb_max_reg_retry_cnt;
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: Max no voice Timeout %d",
    csfb_cdma.max_soft_reg_retry_cnt);

  /* Read 1xCSFB CGI Feature supported or not */
  (void) mc_efs_get(MC_EFS_1xCSFB_CGI_FEATURE_SUPPORTED, &mc_efs_data);
  csfb_cdma.cgi_feature_supported = mc_efs_data.onexcsfb_cgi_feature_supported;
  M1X_MSG( DCP, LEGACY_MED,
           "CSFB DEBUG: 1xCSFB CGI Feature Supported = %d", 
           csfb_cdma.cgi_feature_supported );

#ifdef FEATURE_MODEM_1X_SRVCC
  /* Read EFS for Max Time to wait for MO_UHDM in SRVCC case */
  (void) mc_efs_get(MC_EFS_1XSRVCC_T_MAX_NO_UHDM, &mc_efs_data);
  csfb_cdma.max_srvcc_uhdm_rx_timeout = mc_efs_data.onexsrvcc_uhdm_timeout;
  M1X_MSG( DCP, LEGACY_MED,
           "SRVCC: UHDM TMO:%d", 
           csfb_cdma.max_srvcc_uhdm_rx_timeout );
#endif /* FEATURE_MODEM_1X_SRVCC */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
  /* Read EFS to determine if the call setup time for redirection based
  ** CSFB call is to be optimised */
  (void) mc_efs_get(MC_EFS_1xCSFB_REDIR_CALL_SETUP_OPTIMISED, &mc_efs_data);
  cdma.csfb_call_setup_optimised = mc_efs_data.csfb_call_setup_optimised;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Redirection based CSFB call optimised:%d",
    cdma.csfb_call_setup_optimised);
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

  /* Read EFS for Otasp Max unlock attempts allowed */
  (void) mc_efs_get(MC_EFS_OTASP_MAX_UNLOCK_ATTEMPTS, &mc_efs_data);
  cdma.otasp_max_unlock_attempts = mc_efs_data.otasp_max_unlock_attempts;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Otasp Max unlock attempts allowed:%d",
    cdma.otasp_max_unlock_attempts);

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  /* PPP CLEANUP NON_SV MODE: Read EFS for Deregister Timer Value */
  (void) mc_efs_get(MC_EFS_DEREGISTER_TIMER_VAL, &mc_efs_data);
  cdma.deregister_timer_val_msec = (mc_efs_data.deregister_timer_val * 1000);
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Deregister Timer Value(msec):%d",
    cdma.deregister_timer_val_msec);
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  /* Read EFS whether to reject 1x voice page while VOLTE call is active */
  (void) mc_efs_get(MC_EFS_REJ_VOICE_PAGE_FOR_VOLTE, &mc_efs_data);
  cdma.rej_voice_page_for_volte = mc_efs_data.rej_voice_page_for_volte;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Rej voice page for volte call(default):%d",
    cdma.rej_voice_page_for_volte);

  /* Read EFS for specific call end stats */
  (void) mc_efs_get(MC_EFS_SPECIFIC_CALL_END_STATS_SUPPORTED, &mc_efs_data);
  cdma.specific_call_end_stats_supported = mc_efs_data.specific_call_end_stats_supported;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Specific call end reasons supported:%d",
    cdma.specific_call_end_stats_supported);

#ifdef FEATURE_1X_TO_LTE
  /* Read EFS for support for reg after page drop because of DDTM */
  (void) mc_efs_get(MC_EFS_DISABLE_1X_TO_LTE_RESELECTION, &mc_efs_data);
  cdma.disable_1x_to_lte_reselection = mc_efs_data.disable_1x_to_lte_reselection;
  M1X_MSG( DCP, LEGACY_MED,
    "disable_1x_to_lte_reselection = %d",
    cdma.disable_1x_to_lte_reselection);
#endif /* FEATURE_1X_TO_LTE */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
  /* Read EFS for Split Overheads across multiple wakeups */
  (void) mc_efs_get(MC_EFS_SPLIT_OVERHEAD_SUPPORTED, &mc_efs_data);
  cdma.split_ovhd_supported = mc_efs_data.split_ovhd_supported;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: split_ovhd_supported: %d",
    cdma.split_ovhd_supported);
#endif /* FEATURE_1X_SPLIT_OVERHEAD */

  /* Read EFS for support for CMAS duplicate message detection. */
  (void) mc_efs_get(MC_EFS_CMAS_DUP_DET_SUPPORTED, &mc_efs_data);
  cdma.cmas_dup_det_supported = mc_efs_data.cmas_dup_det_supported;
  M1X_MSG( DCP, LEGACY_MED,
    "cmas_dup_det_supported = %d",
    cdma.cmas_dup_det_supported);

#ifdef FEATURE_1X_SPLMN_SUPPORT
    /* Read EFS for support for no BSR to reselectable plmns */
    (void) mc_efs_get(MC_C2K_RESEL_SPLMN_SUPPORT, &mc_efs_data);
    cdma.splmn_sup.c2k_resel_splmn_supp = mc_efs_data.c2k_resel_splmn_supp;
    M1X_MSG( DCP, LEGACY_MED,
        "c2k_resel_splmn_supp = %d", cdma.splmn_sup.c2k_resel_splmn_supp );
#endif /* FEATURE_1X_SPLMN_SUPPORT */

  /* Read EFS for to use default NV values 
   * if NV Read/Write error happens during Powerup load */
  (void) mc_efs_get(MC_EFS_USE_DEF_VAL_FOR_PWRUP_NV_ERR, &mc_efs_data);
  cdma.use_def_val_for_pwrup_nv_err = mc_efs_data.use_def_val_for_pwrup_nv_err;
  M1X_MSG( DCP, LEGACY_MED,
           "use_def_val_for_pwrup_nv_err = %d",
           cdma.use_def_val_for_pwrup_nv_err);

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
      /* Read EFS for support for 1X overhead optimizations */
      (void) mc_efs_get(MC_EFS_1X_OVHD_OPTIMIZATION, &mc_efs_data);
      cdma.ovhd_opt.ovhd_optimization_sup = mc_efs_data.ovhd_optimization_sup;
      M1X_MSG( DCP, LEGACY_MED,
          "1X ovhd optimizations supported = %d", 
          cdma.ovhd_opt.ovhd_optimization_sup );
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

  #ifdef FEATURE_SIMUL_1X_LTE
  /* Read EFS for 1xSRLTE MT page access handoff feature supported */
  (void) mc_efs_get(MC_EFS_1xSRLTE_MT_PAGE_ACC_HO_SUPPORTED, &mc_efs_data);
  cdma.srlte_mt_page_acc_ho_supported = mc_efs_data.srlte_mt_page_acc_ho_supported;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: 1xsrlte_mt_page_acc_ho_supported: %d",
    cdma.srlte_mt_page_acc_ho_supported);
  #endif /* #ifdef FEATURE_SIMUL_1X_LTE */

  /* Initialize UE mode related global variables */
  cdma.ue_mode = SYS_UE_MODE_NONE;
  cdma.is_ue_mode_substate_srlte = FALSE;

  M1X_MSG( DCP, LEGACY_LOW,
    "MC Initialization finished");

}/* mc_task_init */

/*===========================================================================

FUNCTION MC_INIT

DESCRIPTION
  This function initializes MC queues, timers and data structure related to
  CDMA/AMPS sub tasks.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mc_init ( void )
{

  if(nv_mc_init_fp(&mcc_req_handler, MCC_PESN_PREFIX_MASK, MCC_PESN_PREFIX) == FALSE)
  {
    ERR_FATAL("nv_mc_init_fp failed to register a 1X-CP API",0,0,0);
  }

  /* Initialization before receiving start signal */
  mc_init_before_task_start();

}/* mc_init */

/*===========================================================================
FUNCTION  mc_cdma_prot_activate

DESCRIPTION
  This function is responsible for activating the CDMA protocol for doing
  online acquisition, offline, ftm etc.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_cdma_prot_activate
(
  prot_act_e_type                  act_reason
    /* Reason for activating the CDMA protocol. */
)
{
  /* Reason for deactivating the protocol */
  prot_deact_e_type deact_reason = PROT_DEACT_MAX;

  /* Trans_id for MMoC request.  MMoC handles only one request at a time. */
  prot_trans_type  mmoc_trans_id = PROT_TRANS_NONE;

  /* Prot state to be passed in function mmoc_rpt_prot_auto_deactd_ind to MMOC */
  prot_state_e_type prot_state = PROT_STATE_ONLINE_CDMA;

  mccsrch_rpt_type *srch_ptr;  /* pointer to report received from Searcher */
  boolean srch_deactivate_done_rpt_rxed = FALSE; /* Flag when rpt rxed */

  #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
  /* Clear bs_info[] heap memory cleanup timer */
  (void) mcc_clr_callback_timer( &bs_info_heap_mem_timer,
                                 MCC_BS_INFO_HEAP_MEM_TIMER_ID );
  M1X_MSG( DCP, LEGACY_MED,
           "Cleared bs_info[] heap mem cleanup timer" ); 
  #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

#ifdef FEATURE_1X_CP_MEM_OPT
  /* Information about acquired base stations */

  /* Check if bs_info is already intiaialized, if yes, we
   * can move forward. This is a possibility when we get deactivated
   * from CSFB mode and enter to native 1x mode. While switching modes
   * making sure we dont allocate memory over and again.
   */

  if(bs_info == NULL)
  {
    if ( ( bs_info = ( cdma_bs_type* ) modem_mem_alloc
                     ( MC_MAX_BASES * sizeof( cdma_bs_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( bs_info, 0, ( MC_MAX_BASES * sizeof(cdma_bs_type) ) );

      /* Pointer to information for currently acquired base station */
      cur_bs_ptr = bs_info;
    }
    else
    {
      ERR_FATAL("Memory allocation failed for bs_info[] array", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "bs_info already intialized");
  }

  /* Pointer holds Post hand-off data */
  if(mcctcho_ho_data_ptr == NULL)
  {
    if ( ( mcctcho_ho_data_ptr = ( mcctcho_ho_data_type* ) modem_mem_alloc
                                 ( sizeof( mcctcho_ho_data_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( mcctcho_ho_data_ptr, 0, sizeof(mcctcho_ho_data_type) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for mcctcho_ho_data_ptr", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "mcctcho_ho_data_ptr already intialized");
  }

  M1X_MSG( DCP, LEGACY_HIGH,
           "act_reason:%d",
           act_reason );

  /* Do not allocate dynamic memory for below structures in FTM Mode */
  if(act_reason != PROT_ACT_FTM)
  {
    /* List of SYNC ID entries */
    if( sync_id_info == NULL)
    {
      if ( ( sync_id_info = ( sync_id_info_type* ) modem_mem_alloc
                            ( CAI_MAX_NUM_STORED_SYNC_ID * sizeof( sync_id_info_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
      {
        memset( sync_id_info, 0, ( CAI_MAX_NUM_STORED_SYNC_ID * sizeof(sync_id_info_type) ) );
      }
      else
      {
        ERR_FATAL("Memory allocation failed for sync_id_info[] array", 0, 0, 0);
        return;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "sync_id_info already intialized");
    }

    /* The structure to hold the Message Rec field of the GEM, i.e. the message
     * extended by GEM. Since any message can be extended by GEM, so it is not
     * possible to include the message body in the GEM structure. Rather, a
     * pointer is available in GEM structure which will point to the above
     * defined structure.
     */
    if(caii_gem_message_rec_ptr == NULL)
    {
      if ( ( caii_gem_message_rec_ptr = ( caii_rx_msg_type* ) modem_mem_alloc
                                        ( sizeof( caii_rx_msg_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
      {
        memset( caii_gem_message_rec_ptr, 0, sizeof(caii_rx_msg_type) );
      }
      else
      {
        ERR_FATAL("Memory allocation failed for caii_gem_message_rec_ptr", 0, 0, 0);
        return;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "caii_gem_message_rec_ptr already intialized");
    }

    /* Pointer to hold info of service configuration to be verified */
    if(restore_srv_cfg_info_ptr == NULL)
    {
      if ( ( restore_srv_cfg_info_ptr = ( restore_srv_cfg_info_type* ) modem_mem_alloc
                                        ( sizeof( restore_srv_cfg_info_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
      {
        memset( restore_srv_cfg_info_ptr, 0, sizeof(restore_srv_cfg_info_type) );
      }
      else
      {
        ERR_FATAL("Memory allocation failed for restore_srv_cfg_info_ptr", 0, 0, 0);
        return;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "restore_srv_cfg_info_ptr already intialized");
    }

    /* Pointer holds TDSO Pre-Synchronization info */
    if(tdso_pre_sync_ptr == NULL)
    {
      if ( ( tdso_pre_sync_ptr = ( tdso_pre_sync_type* ) modem_mem_alloc
                                 ( sizeof( tdso_pre_sync_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
      {
        memset( tdso_pre_sync_ptr, 0, sizeof(tdso_pre_sync_type) );
      }
      else
      {
        ERR_FATAL("Memory allocation failed for tdso_pre_sync_ptr", 0, 0, 0);
        return;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "tdso_pre_sync_ptr already intialized");
    }
  } /* if(act_reason != PROT_ACT_FTM) */
#endif /* FEATURE_1X_CP_MEM_OPT */

  /* Initialize the band class table. Moving this API from mc task 
   * initialization because of Policy manager changes due to which 
   * TRM gets updated with modes information with some delay */
  mcc_band_class_init();
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (act_reason != PROT_ACT_FTM)
  {
    /* Activate SRCH */
    mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
    mcc_srch_cmd( &mcc_srch_buf );
  }

  /* Activate CDMA protocol based on act_reason.
  */
  switch ( act_reason )
  {
    case PROT_ACT_ONLINE_ACQ:
      mc_set_digital_acquisition( MCC_ACQUISITION );
      deact_reason = mc_online_digital_state(&mmoc_trans_id);
      break;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case PROT_ACT_GWL_REDIR:
    {
      /* Set SD's context with the redirected channel information received.
       * We set SD's context with the band/chan information since for IRAT
       * switch we want to try the band/channel received from IRAT switch
       * indicators, not the one provisioned on PRL. After setting SD's
       * context if CP queries for band/chan to acquire 1x on, it will be
       * given from Redir list. once the Redir list gets exhaused, SD will ask
       * CP to exit.
       */
      if(mcc_csfb_set_sd_context() == TRUE)
      {
        /* Activate 1x protocol */
        /* Enter to native 1X mode (starting from CDMA_ENTER) */
        if (mc_redir_req_msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
      {
          mc_set_digital_acquisition(MCC_LTE_1X_REDIR);
      }
      else
      {
          mc_set_digital_acquisition(MCC_LTE_1X_RESEL);
      }
        deact_reason = mc_online_digital_state(&mmoc_trans_id);
      }
      else
      {
        deact_reason = mc_map_ss_action_to_deact_reason(
                         MC_ONLINE_DIGITAL_STATE,
                         sd_ss_act_get(NULL));
        mc_irat_switch = FALSE;
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB:DEBUG - setting context for redir/resel failed");
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB:DEBUG - deact reason = %d",
          deact_reason);
      }
           break;
    } /* PROT_ACT_GWL_REDIR */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

    case PROT_ACT_OFFLINE:
      deact_reason = mc_offline_digital_state(&mmoc_trans_id);
      /* Note: Cannot autonomously deactivate itself. */
      if (mmoc_trans_id == PROT_TRANS_NONE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "mc_offline_digital exited autonomously");
      }
      break;

    case PROT_ACT_FTM:
      onex_nonsig_set_ftm_mode(NS1X_FTM_MODE);
      M1X_MSG( DCP, LEGACY_MED,
        "Entering FTM");
      onex_nonsig_system_determination();
      onex_nonsig_set_ftm_mode(NS1X_DMSS_MODE);
      break;

    case PROT_ACT_DED_MEAS_REQUEST:
      deact_reason = mc_meas_ded_state(&mmoc_trans_id);

      /* in this case, we need pass a different prot_state to the function of
       * mmoc_rpt_prot_auto_deactd_ind to MMOC */
      prot_state = PROT_STATE_ONLINE_DED_MEAS;
      break;

//    case PROT_ACT_PSEUDO_IDLE:
//      MSG_HIGH( "Activate 1x to Pseudo-Idle Mode", 0, 0, 0 );
//      mc_set_digital_acquisition( MCC_PSEUDO_IDLE );
//      deact_reason = mc_online_digital_state(&mmoc_trans_id);
//      break;

    default:
      ERR_FATAL("Unknown activate reason on CDMA protocol", 0, 0, 0 );
      break;

  } /* switch ( act_reason ) */


  if ((act_reason != PROT_ACT_FTM ) &&
      ((deact_reason != PROT_DEACT_CDMA_AMPS_HANDOFF) &&
       (deact_reason != PROT_DEACT_MAX))
     )
  {
    /* Deactivate SRCH and wait for it to really deactivate. */
    mcc_srch_buf.hdr.cmd = SRCH_DEACTIVATE_F;
    mcc_srch_cmd( &mcc_srch_buf );

    while (!srch_deactivate_done_rpt_rxed)
    {
      (void) mcc_wait( (dword) MCC_SRCH_Q_SIG );
      while ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
      {
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
                     cdma.curr_state, MCC_NON_SLEEP, MC_CDMA_PROT_ACTIVATE );       
 
        if ( srch_ptr->hdr.rpt == SRCH_DEACTIVATE_DONE_R )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Rxed SRCH_DEACTIVATE_DONE_R" );
          srch_deactivate_done_rpt_rxed = TRUE;
          cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
          break;
        }
        M1X_MSG( DCP, LEGACY_MED,
          "DISCARDED %d",
          srch_ptr->hdr.rpt);
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the deactivation report to MMOC. */
  if ( mmoc_trans_id != PROT_TRANS_NONE ) //lint !e771
  {
    mmoc_rpt_prot_deactd_cnf( mmoc_trans_id );
    mcc_enqueue_dbg_mmoc_cnf( MC_CDMA_PROT_ACTIVATE_API, MMOC_RPT_PROT_DEACTD_CNF,
                              mmoc_trans_id);    
  }
  /* Don't send Auto_Deact ind to MMODE for FTM mode */
  else if (act_reason != PROT_ACT_FTM)
  {
    mmoc_rpt_prot_auto_deactd_ind( deact_reason, prot_state);
    mcc_enqueue_dbg_mmoc_cnf( MC_CDMA_PROT_ACTIVATE_API, MMOC_RPT_PROT_AUTO_DEACTD_IND,
                              PROT_TRANS_NONE);
  }

#ifdef FEATURE_1X_CP_MEM_OPT

  /* If either of the NV#71575 - onexcsfb_cgi_feature_supported OR 
   * NV#71577 - csfb_call_setup_optimised is enabled, then do not deallocate 
   * the memory for bs_info[] array, as the allocation of 
   * bs_info is done only once so as to reuse the stored overheads
   * This optimization is req to support CGI (Cell Global Identity) feature
   * and to reduce the call setup time for redirection based CSFB calls */ 
  if( (TRUE)
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      &&(!csfb_cdma.cgi_feature_supported)
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
      && (!cdma.csfb_call_setup_optimised)
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */
    )
  {
    /* Enter Critical Section */
    REX_ISR_LOCK(&mc_crit_sect);
    /* Information about acquired base stations */
    if( bs_info != NULL )
    {
      modem_mem_free( bs_info, MODEM_MEM_CLIENT_1X );
      bs_info = NULL;
  
      /* Pointer to information for currently acquired base station */
      cur_bs_ptr = NULL;
    }
    /* Leave Critical Section */
    REX_ISR_UNLOCK(&mc_crit_sect);
  } /* if( (!csfb_cdma.cgi_feature_supported) */

  #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
             "DEBUG - deact_reason:%d",
             deact_reason );
    /* Start bs_info[] heap memory cleanup timer only for below deact rsns */
    if( (deact_reason == PROT_DEACT_LPM) ||
        (deact_reason == PROT_DEACT_OFFLINE) ||
        (deact_reason == PROT_DEACT_ACQ_OTHERS)
      )
    {
      /* Start bs_info[] heap memory cleanup timer */
      (void) mcc_set_callback_timer( &bs_info_heap_mem_timer,
               (uint32) (BS_INFO_HEAP_MEM_TIMER_VAL_MIN * 60 * 1000),
                                     MCC_BS_INFO_HEAP_MEM_TIMER_ID );
      M1X_MSG( DCP, LEGACY_MED,
               "Started bs_info[] heap mem cleanup timer with:%d-mins",
               BS_INFO_HEAP_MEM_TIMER_VAL_MIN ); 
    }                                  
  }
  #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

  if( sync_id_info != NULL )
  {
    /* List of SYNC ID entries */
    modem_mem_free( sync_id_info, MODEM_MEM_CLIENT_1X );
    sync_id_info = NULL;
  }

  /* The structure to hold the Message Rec field of the GEM, i.e. the message
   * extended by GEM. Since any message can be extended by GEM, so it is not
   * possible to include the message body in the GEM structure. Rather, a
   * pointer is available in GEM structure which will point to the above
   * defined structure.
   */
  if( caii_gem_message_rec_ptr != NULL )
  {
    modem_mem_free( caii_gem_message_rec_ptr, MODEM_MEM_CLIENT_1X );
    caii_gem_message_rec_ptr = NULL;
  }

  /* Pointer hods Post hand-off data */
  if( mcctcho_ho_data_ptr != NULL )
  {
    modem_mem_free( mcctcho_ho_data_ptr, MODEM_MEM_CLIENT_1X );
    mcctcho_ho_data_ptr = NULL;
  }

  /* Pointer to hold info of service configuration to be verified */
  if( restore_srv_cfg_info_ptr != NULL )
  {
    modem_mem_free( restore_srv_cfg_info_ptr, MODEM_MEM_CLIENT_1X );
    restore_srv_cfg_info_ptr = NULL;
  }

  /* Pointer holds TDSO Pre-Synchronization info */
  if( tdso_pre_sync_ptr != NULL )
  {
    modem_mem_free( tdso_pre_sync_ptr, MODEM_MEM_CLIENT_1X );
    tdso_pre_sync_ptr = NULL;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  return;

} /* mc_cdma_prot_activate() */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================
FUNCTION  MC_CSFB_PROT_ACTIVATE

DESCRIPTION
  This function is responsible for activating the CSFB protocol for doing
  online acquisition, offline, ftm etc.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_csfb_prot_activate(void)
{
  /* Pointer to send the CM 1XCSFB deactvation confirmation */
  cm_mc_rpt_type *cm_ptr;

  /* Prot state to be passed in function mmoc_rpt_prot_auto_deactd_ind to MMOC */
  prot_state_e_type prot_state = PROT_STATE_ONLINE_CDMA;

  /* Reason for deactivating the protocol */
  prot_deact_e_type deact_reason = PROT_DEACT_MAX;

  /* Trans_id for MMoC request.  MMoC handles only one request at a time. */
  prot_trans_type  mmoc_trans_id = PROT_TRANS_NONE;

  boolean srch_deactivate_done_rpt_rxed = FALSE; /* Flag when rpt rxed */
  mccsrch_rpt_type *srch_ptr;  /* pointer to report received from Searcher */

  #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
  /* Clear bs_info[] heap memory cleanup timer */
  (void) mcc_clr_callback_timer( &bs_info_heap_mem_timer,
                                 MCC_BS_INFO_HEAP_MEM_TIMER_ID );
  M1X_MSG( DCP, LEGACY_MED,
           "Cleared bs_info[] heap mem cleanup timer" ); 
  #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */
  
 #ifdef FEATURE_1X_CP_MEM_OPT
  /* Information about acquired base stations */

  /* Check if bs_info is already intiaialized, if yes, we
   * can move forward. This is a possibility when we get deactivated
   * from CSFB mode and enter to native 1x mode. While switching modes
   * making sure we dont allocate memory over and again.
   */

  if(bs_info == NULL)
  {
    if ( ( bs_info = ( cdma_bs_type* ) modem_mem_alloc
                     ( MC_MAX_BASES * sizeof( cdma_bs_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( bs_info, 0, ( MC_MAX_BASES * sizeof(cdma_bs_type) ) );

      /* Pointer to information for currently acquired base station */
      cur_bs_ptr = bs_info;
    }
    else
    {
      ERR_FATAL("Memory allocation failed for bs_info[] array", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "bs_info already intialized");
  }

  /* List of SYNC ID entries */
  if( sync_id_info == NULL)
  {
    if ( ( sync_id_info = ( sync_id_info_type* ) modem_mem_alloc
                          ( CAI_MAX_NUM_STORED_SYNC_ID * sizeof( sync_id_info_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( sync_id_info, 0, ( CAI_MAX_NUM_STORED_SYNC_ID * sizeof(sync_id_info_type) ) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for sync_id_info[] array", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "sync_id_info already intialized");
  }

  /* The structure to hold the Message Rec field of the GEM, i.e. the message
   * extended by GEM. Since any message can be extended by GEM, so it is not
   * possible to include the message body in the GEM structure. Rather, a
   * pointer is available in GEM structure which will point to the above
   * defined structure.
   */
  if(caii_gem_message_rec_ptr == NULL)
  {
    if ( ( caii_gem_message_rec_ptr = ( caii_rx_msg_type* ) modem_mem_alloc
                                      ( sizeof( caii_rx_msg_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( caii_gem_message_rec_ptr, 0, sizeof(caii_rx_msg_type) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for caii_gem_message_rec_ptr", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "caii_gem_message_rec_ptr already intialized");
  }

  /* Pointer holds Post hand-off data */
  if(mcctcho_ho_data_ptr == NULL)
  {
    if ( ( mcctcho_ho_data_ptr = ( mcctcho_ho_data_type* ) modem_mem_alloc
                                 ( sizeof( mcctcho_ho_data_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( mcctcho_ho_data_ptr, 0, sizeof(mcctcho_ho_data_type) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for mcctcho_ho_data_ptr", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "mcctcho_ho_data_ptr already intialized");
  }

  /* Pointer to hold info of service configuration to be verified */
  if(restore_srv_cfg_info_ptr == NULL)
  {
    if ( ( restore_srv_cfg_info_ptr = ( restore_srv_cfg_info_type* ) modem_mem_alloc
                                      ( sizeof( restore_srv_cfg_info_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( restore_srv_cfg_info_ptr, 0, sizeof(restore_srv_cfg_info_type) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for restore_srv_cfg_info_ptr", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "restore_srv_cfg_info_ptr already intialized");
  }

  /* Pointer holds TDSO Pre-Synchronization info */
  if(tdso_pre_sync_ptr == NULL)
  {
    if ( ( tdso_pre_sync_ptr = ( tdso_pre_sync_type* ) modem_mem_alloc
                               ( sizeof( tdso_pre_sync_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( tdso_pre_sync_ptr, 0, sizeof(tdso_pre_sync_type) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for tdso_pre_sync_ptr", 0, 0, 0);
      return;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "tdso_pre_sync_ptr already intialized");
  }
  /* Pointer holds downlink GCSNA messages format which are rxed on Tunnel */
  if( mcc_gcsna_ext_msg_ptr == NULL )
  {
    if( ( mcc_gcsna_ext_msg_ptr = ( gcsna_dl_msg_type* ) modem_mem_alloc
                                  ( sizeof( gcsna_dl_msg_type ), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( mcc_gcsna_ext_msg_ptr, 0, sizeof(gcsna_dl_msg_type) );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for mcc_gcsna_ext_msg_ptr");
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "mcc_gcsna_ext_msg_ptr already intialized");
  }
 #endif /* FEATURE_1X_CP_MEM_OPT */

  /* Initialize the band class table. Moving this API from mc task 
   * initialization because of Policy manager changes due to which 
   * TRM gets updated with modes information with some delay */
  mcc_band_class_init();
  
  /* Set the CSFB mode to active since we are CSFB activated */
  csfb_cdma.csfb_mode =TRUE;

  /* Initialize CSFB exit to exit_none */
  mcc_csfb_exit.exit = MCC_CSFB_EXIT_NONE;

  /* Set the netry reason */
  mc_set_digital_acquisition( MCC_CSFB );

  /* Enter CSFB state operation */
  deact_reason = mc_online_digital_state(&mmoc_trans_id);


  /* We come here because of SD's action to exit (like after call end) or
   * because of CM sending CSFB deactivate. Incase of a CSFB deactivate
   * send deactivae confirmation to CM and dont send MMOC auto deact ind.
   * In case we exit because of SD's action send Auto Deact Ind to MMOC.
   * Note that for any actions of deactivating CP from CSFB CM will trigger
   * CSFB deactivate with corresponding cause, which can be mapped to
   * PROTOCOL_DEACT sent from MMOC
   */

  /* Deactivate SRCH and wait for it to really deactivate.*/
  mcc_srch_buf.hdr.cmd = SRCH_DEACTIVATE_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* Check if SRCH returned SRCH_ERR_S status
   * If yes then don't wait for deact_done rpt from SRCH and go
   * ahead with sending deact_cnf to MMODE to avoid MMOC sanity timeout.
   * Here SRCH may be busy with processing GAP msmts req from LTE ML1 */
  if(mcc_srch_buf.hdr.status != SRCH_ERR_S)
  {
    while (!srch_deactivate_done_rpt_rxed)
    {
      (void) mcc_wait( (dword) MCC_SRCH_Q_SIG );
      while ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
      {
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state,
                       cdma.curr_state, MCC_NON_SLEEP, MC_CSFB_PROT_ACTIVATE );
        
        if ( srch_ptr->hdr.rpt == SRCH_DEACTIVATE_DONE_R )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Rxed SRCH_DEACTIVATE_DONE_R" );
          srch_deactivate_done_rpt_rxed = TRUE;
          cmd_done( &srch_ptr->hdr.rpt_hdr );
  #ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
  #endif /* FEATURE_1X_CP_MEM_OPT */
          break;
        }
        M1X_MSG( DCP, LEGACY_MED,
          "DISCARDED %d",
          srch_ptr->hdr.rpt);
        cmd_done( &srch_ptr->hdr.rpt_hdr );
  #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
  #endif /* FEATURE_1X_CP_MEM_OPT */
      }
    } /* while() */
  } /* if(status != SRCH_ERR_S) */
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
       "ERR_S status, not waiting for deact_done rpt from SRCH" );
  }

  /* SRCH is deactivated, now send responses/confirmation to upper layers */
  if(mcc_csfb_exit.exit == MCC_CSFB_EXIT_CSFB_DEACT)
  {
    /* Boolean to store SRVCC Mode */
    boolean srvcc_mode = mcc_csfb_get_srvcc_mode();           

    /* We get here when we receive CSFB deact from CM */
    M1X_MSG( DCP, LEGACY_HIGH,
    "Got out of CSFB/SRVCC mode, rsn deact, sending rpt to CM, SRVCC_Mode:%d",
      srvcc_mode );
    /* Send CM 1XCSFB deactvation confirmation. */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
#ifdef FEATURE_MODEM_1X_SRVCC
      if(srvcc_mode == TRUE)
      {
        cm_ptr->cdma_csfb_deact_cnf.hdr.cmd = CM_1XSRVCC_DEACT_CNF_F;

        /* Update SRVCC mode to False */
        mcc_csfb_update_srvcc_mode(FALSE);
      } /* SRVCC mode */
      else 
#endif /* FEATURE_MODEM_1X_SRVCC */
      { /* CSFB mode */
        cm_ptr->cdma_csfb_deact_cnf.hdr.cmd = CM_1XCSFB_DEACT_CNF_F;
      }
      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
    }
  }
  else
  {
    /* We get here when there is an internal action stir at MMODE and SD asks us
     * to exit
     */
    /* Send the deactivation report to MMOC. */
    if ( mmoc_trans_id != PROT_TRANS_NONE ) //lint !e771
    {
      M1X_MSG( DCP, LEGACY_MED,
        "CSFB DEBUG: sending DEACT CNF");
      mmoc_rpt_prot_deactd_cnf( mmoc_trans_id );
      mcc_enqueue_dbg_mmoc_cnf( MC_CSFB_PROT_ACTIVATE_API, MMOC_RPT_PROT_DEACTD_CNF,
                                mmoc_trans_id);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "CSFB DEBUG: sending MMOC AUTO DEACT IND");
      mmoc_rpt_prot_auto_deactd_ind( deact_reason, prot_state);
      mcc_enqueue_dbg_mmoc_cnf( MC_CSFB_PROT_ACTIVATE_API, MMOC_RPT_PROT_AUTO_DEACTD_IND,
                                PROT_TRANS_NONE);
    }
  }
  /* Got out of CSFB mode, reset the CSFB variable */
  csfb_cdma.csfb_mode = FALSE;

  if( (mcc_exit.exit == MCC_LPM) ||
      (mcc_exit.exit == MCC_POWERDOWN) ||
      (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN)
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB DEBUG: Exiting because of power_down,doing reg house keeping");
    /* Perform registration powerdown housekeeping */
    mccreg_powerdown();
  }

  /* We are getting out of CSFB mode, release sr_ids which could be created over
   * tunnel.
   */

  /* For CCS, Let sr_id manager know that we are leaving TC */
  mccsrid_release();

  /*
  ** Catch all to prevent possible sr_id leak
  */
  mccsrid_free_all_owned_srids(SRID_OWNER_MC);

  /* Deallocate all dynamic memory */
#ifdef FEATURE_1X_CP_MEM_OPT

  /* If either of the NV#71575 - onexcsfb_cgi_feature_supported OR 
   * NV#71577 - csfb_call_setup_optimised is enabled, then do not deallocate 
   * the memory for bs_info[] array, as the allocation of 
   * bs_info is done only once so as to reuse the stored overheads
   * This optimization is req to support CGI (Cell Global Identity) feature
   * and to reduce the call setup time for redirection based CSFB calls */ 
  if( (TRUE)
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      &&(!csfb_cdma.cgi_feature_supported)
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
      && (!cdma.csfb_call_setup_optimised)
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */
    )
  {
    /* Enter Critical Section */
    REX_ISR_LOCK(&mc_crit_sect);
    /* Information about acquired base stations */
    if( bs_info != NULL )
    {
      modem_mem_free( bs_info, MODEM_MEM_CLIENT_1X );
      bs_info = NULL;
  
      /* Pointer to information for currently acquired base station */
      cur_bs_ptr = NULL;
    }
    /* Leave Critical Section */
    REX_ISR_UNLOCK(&mc_crit_sect);
  
  }

  #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
             "DEBUG - deact_reason:%d",
             deact_reason );
    /* Start bs_info[] heap memory cleanup timer only for below deact rsns */
    if( (deact_reason == PROT_DEACT_LPM) ||
        (deact_reason == PROT_DEACT_OFFLINE) ||
        (deact_reason == PROT_DEACT_ACQ_OTHERS)
      )
    {
      /* Start bs_info[] heap memory cleanup timer */
      (void) mcc_set_callback_timer( &bs_info_heap_mem_timer,
               (uint32) (BS_INFO_HEAP_MEM_TIMER_VAL_MIN * 60 * 1000),
                                     MCC_BS_INFO_HEAP_MEM_TIMER_ID );
      M1X_MSG( DCP, LEGACY_MED,
               "Started bs_info[] heap mem cleanup timer with:%d-mins",
               BS_INFO_HEAP_MEM_TIMER_VAL_MIN ); 
    }                                  
  }
  #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

  if( sync_id_info != NULL )
  {
    /* List of SYNC ID entries */
    modem_mem_free( sync_id_info, MODEM_MEM_CLIENT_1X );
    sync_id_info = NULL;
  }

  /* The structure to hold the Message Rec field of the GEM, i.e. the message
   * extended by GEM. Since any message can be extended by GEM, so it is not
   * possible to include the message body in the GEM structure. Rather, a
   * pointer is available in GEM structure which will point to the above
   * defined structure.
   */
  if( caii_gem_message_rec_ptr != NULL )
  {
    modem_mem_free( caii_gem_message_rec_ptr, MODEM_MEM_CLIENT_1X );
    caii_gem_message_rec_ptr = NULL;
  }

  /* Pointer hods Post hand-off data */
  if( mcctcho_ho_data_ptr != NULL )
  {
    modem_mem_free( mcctcho_ho_data_ptr, MODEM_MEM_CLIENT_1X );
    mcctcho_ho_data_ptr = NULL;
  }

  /* Pointer to hold info of service configuration to be verified */
  if( restore_srv_cfg_info_ptr != NULL )
  {
    modem_mem_free( restore_srv_cfg_info_ptr, MODEM_MEM_CLIENT_1X );
    restore_srv_cfg_info_ptr = NULL;
  }

  /* Pointer holds TDSO Pre-Synchronization info */
  if( tdso_pre_sync_ptr != NULL )
  {
    modem_mem_free( tdso_pre_sync_ptr, MODEM_MEM_CLIENT_1X );
    tdso_pre_sync_ptr = NULL;
  }

  /* Pointer holds downlink GCSNA messages format which are rxed on Tunnel */
  if( mcc_gcsna_ext_msg_ptr != NULL )
  {
    modem_mem_free( mcc_gcsna_ext_msg_ptr, MODEM_MEM_CLIENT_1X );
    mcc_gcsna_ext_msg_ptr = NULL;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

} /* mc_csfb_prot_activate() */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================
FUNCTION  mc_process_cdma_standby_pref_chg_cmd

DESCRIPTION
  This function will call the appropriate housekeeping functions to do
  powerup registration during next protocal activation.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_process_cdma_standby_pref_chg_cmd( void )
{
  /* This issue is specific to win phone, where win HLOS sent chg in standby 
   * preference to single-standby there by deactivating sub1 (1x). Which
   * cause MMOC to send "Prot_deact with pwr_down/LPM" to MC and MC to do 
   * powerdown reg and deactivate itself.
   * But will do powerup reg during next activation only if there is
   * ph_stat_chd cmd from MMOC but that is not expected for this scenario
   * and when 1x activated next time, MC is not doing any powerup reg and 
   * UE is missing MT pages because it is not pagable by n/w.
   *
   * So for this particular scenario, now MMOC will send standby_pref_chg
   * cmd to MC so that it has to do powerup reg during next activation */
  M1X_MSG( DCP, LEGACY_HIGH,
           "Rcvd standby_pref_chg cmd, Setting Powerup reg rel var's" );

  /* Perform registration powerup procedures. */
  mccreg_powerup_mode_online();
} /* mc_process_cdma_standby_pref_chg_cmd */

/*===========================================================================
FUNCTION  mc_process_cdma_ph_stat_cmd

DESCRIPTION
  This function will call the appropriate housekeeping functions for CDMA
  protocol based on the phone status changed command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_process_cdma_ph_stat_cmd
(
  prot_ph_stat_chgd_s_type *ph_stat_ptr
)
{
  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* False if powerup_load is successful */
  boolean powerup_load_failure = FALSE;

  /* Status for phone status change request to be sent to MMode */
  mmoc_ph_stat_e_type req_status = MMOC_PH_STAT_NO_ERROR;
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
  
  M1X_MSG( DCP, LEGACY_MED,
           "CDMA phone status chgd cmd, chg_type = %d",
           ph_stat_ptr->chg_type );
        
  /* Perform housekeeping functions based on the phone status change
  ** type.
  */
  switch ( ph_stat_ptr->chg_type )
  {
    case PROT_PH_STAT_CHG_NAM:
    case PROT_PH_STAT_CHG_SUBSC:
    {
      cdma.is_cdma_subs_avail = ph_stat_ptr->is_cdma_subs_avail;
      M1X_MSG( DCP, LEGACY_MED,
               "CDMA subs_avail:%d, Prot_subsc_chg:%d, Oprt_mode:%d",
               cdma.is_cdma_subs_avail, 
               ph_stat_ptr->prot_subsc_chg,
               ph_stat_ptr->oprt_mode );

      #ifdef FEATURE_CSIM
      /* Check if we are successfully client_id and session_id registered
       * by the time we receive CDMA subscription.
       */
      if( (cdma.is_cdma_subs_avail == TRUE) &&
          ( (cdma.client_id == MC_INVALID_CDMA_CLIENT_ID) ||
            (cdma.session_id == MC_INVALID_CDMA_SESSION_ID)
          )
        )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid ClientID/SessionID.. power-up load can fail");
      }

      /* Get the current active 1X Primary Provisioned application on card */
      if( cdma.is_cdma_subs_avail == TRUE )
      {
        mmgsdi_session_info_query_type mcmmgsdi_query;
        mmgsdi_session_get_info_type   mcmmgsdi_get_session_info;

        mcmmgsdi_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_ID;
        mcmmgsdi_query.query_key.session_id = cdma.session_id;
        (void)mmgsdi_session_get_info_sync( mcmmgsdi_query,
                                      &mcmmgsdi_get_session_info );

        mcmmgsdi_app_type = mcmmgsdi_get_session_info.app_data.app_type ;
        cdma.curr_slot_id = mcmmgsdi_get_session_info.slot_id;
        M1X_MSG( DCP, LEGACY_MED,
          "Current active application on card = %d , slot_id = %d ",
          mcmmgsdi_app_type,
          cdma.curr_slot_id);
      }
      #endif /* FEATURE_CSIM */



      mc_init_auth();


      /* Reload the subscription information.
      ** Subscription information is reloaded only if CM informs of a change
      ** in CDMA subscription information.
      ** For NAM change, CM always marks CDMA subscription info as changed.
      */
      if (CHECK_PROTO(ph_stat_ptr->prot_subsc_chg, PROT_SUBSC_CHG_CDMA))
      {
        if ( (
             #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
             powerup_load_failure = 
             #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
             !mcc_powerup_load() ) &&
             ph_stat_ptr->oprt_mode != PROT_OPRT_MODE_OFFLINE &&
             ph_stat_ptr->oprt_mode != PROT_OPRT_MODE_OFFLINE_CDMA &&
             ph_stat_ptr->oprt_mode != PROT_OPRT_MODE_OFFLINE_AMPS
           )
        {
          /* Subscription load has failed, UE may not work as expected.
          ** If any NV read fails during power-up, we are not moving the
          ** device to offline mode. Here we won't be checking the NV
          ** use_def_val_for_pwrup_nv_err, since the device should 
          ** not move to Offline mode.
          */
          M1X_MSG( DCP, LEGACY_ERROR,
                   "POWERUP LOAD FAILED:UE may not work as expected" );
        } /* mcc_powerup_load() */

        /* If powerup load failed due to sim busy event/NV error, 
        ** update status of phone status changed confirmation to reflect 
        ** SIM BUSY/NV_ERR as reason.
        */
        #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
        if(powerup_load_failure)
        {
          if(mc_get_sim_busy_status() == TRUE)
          {  
            req_status = MMOC_PH_STAT_ERR_SIM_BUSY;
          } /* SIM_BUSY */

#ifdef FEATURE_MODEM_1X_PWRUP_NV_ERR
          else if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
          {
            req_status = MMOC_PH_STAT_ERR_NV_ERR;
          } /* NV_ERR */
#endif /* FEATURE_MODEM_1X_PWRUP_NV_ERR */

          M1X_MSG( DCP, LEGACY_HIGH, 
                   "Powerup load failed, req status = %d",
                   req_status );
        }
        #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
      }
      break;
    }

    case PROT_PH_STAT_CHG_OPRT_MODE:
    {
      M1X_MSG( DCP, LEGACY_MED,
               "CDMA phone status cmd, oprt_mode_chg = %d",
               ph_stat_ptr->oprt_mode );
      switch ( ph_stat_ptr->oprt_mode )
      {
        case PROT_OPRT_MODE_PWROFF:
        case PROT_OPRT_MODE_RESET:
        case PROT_OPRT_MODE_LPM:
          mcc_powerdown( ph_stat_ptr->oprt_mode );
          break;

        case PROT_OPRT_MODE_OFFLINE:
        case PROT_OPRT_MODE_OFFLINE_CDMA:
          /* set offline in MC.  */
          mc_update_last_cdma_entry(MCC_OFFLINE);
          /*lint -fallthrough */
        case PROT_OPRT_MODE_OFFLINE_AMPS:
          mcc_offline();
          break;

        case PROT_OPRT_MODE_ONLINE:
          /* Perform registration powerup procedures. */
          mccreg_powerup_mode_online();
          break;

        case PROT_OPRT_MODE_FTM:
        {
          M1X_MSG( DCP, LEGACY_MED,
                   "CDMA phone status cmd with oprt_mode_ftm, subs_avail = %d",
                   ph_stat_ptr->is_cdma_subs_avail );
                   
          /* Load nv data for mc. */
          cdma.is_cdma_subs_avail = ph_stat_ptr->is_cdma_subs_avail;

          if(mcc_powerup_load() == FALSE)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
                     "FTM:POWERUP LOAD FAILED, UE may not work as expected" );
          } /* !mcc_powerup_load */
          break;
        } /* PROT_OPRT_MODE_FTM */

        default:
          break;

      } /* switch ( ph_stat_ptr->oprt_mode ) */

      break;
    }

    default:
      break;

  } /* switch ( ph_stat_ptr->chg_type ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the confirmation to MMoC. */
  mmoc_rpt_ph_stat_chgd_cnf( ph_stat_ptr->trans_id

                             ,SD_MODE_CDMA

                             #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
                             ,req_status
                             #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
                           );
  mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CDMA_PH_STAT_CMD, MMOC_RPT_PH_STAT_CHGD_CNF,
                            ph_stat_ptr->trans_id);
  return;

} /* mc_process_cdma_ph_stat_cmd() */

/*===========================================================================
FUNCTION  mc_process_cmd

DESCRIPTION
  This function gets a message off the MC command queue and processes it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void mc_process_cmd
(
  void
)
{
  /* Pointer to the command received. */
  mc_msg_type *cmd_ptr;
  word old_state = cdma.curr_state;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  boolean is_ignored = FALSE;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  /* Loop until both mc_cmd_q, MSGR queue are empty. */
  do
  {
    #ifdef FEATURE_1X_CP_IGNORE_GSRDM
    /* First process GSRDM Ignore timer before processing mc_cmd_q
     * because MMODE might have sent CDMA prot activate cmd, 
     * if that is the case we will be activated in CDMA mode and 
     * enter into CDMA state machine and we don't get a chance to 
     * process GSRDM Ignore timer expiry */
    if( mcc_timer_on_callback_q(MCC_GSRDM_IGNORE_TIMER_ID) )
    {
      /* Process GSRDM Ignore timer expiry */
      (void) mccidl_gsrdm_ignore_timer_expiry(cdma.curr_state);
    } /* MCC_GSRDM_IGNORE_TIMER_ID */
    else 
    #endif /* FEATURE_1X_CP_IGNORE_GSRDM */

    if ((cmd_ptr = (mc_msg_type *) q_check( &mc_cmd_q )) != NULL)
    {

    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Rxed cmd = %d in state = %d",
      cmd_ptr->hdr.cmd,
      cdma.curr_state);

      /* Process or buffer based on the command recvd. */
      switch ( cmd_ptr->hdr.cmd )
      {
        case MC_CDMA_PROT_ACT_CMD_F:
        {
          mc_msg_type mc_cmd_buf = *cmd_ptr;
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );

          /* Activate the CDMA protocol based on activate reason. */
          mc_cdma_prot_activate( mc_cmd_buf.act_prot.info.reason );
          break;
        }

        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        case MC_CSFB_ACT_F:
#ifdef FEATURE_MODEM_1X_SRVCC
        case MC_SRVCC_ACT_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
        {
#ifdef FEATURE_MODEM_1X_SRVCC
          /* For SRVCC mode also, we use the CSFB framework and
           * we will differentiate b/w CSFB and SRVCC mode through
           * srvcc_mode global variable */
          if(cmd_ptr->hdr.cmd == MC_SRVCC_ACT_F)
          {
            /* Update MC SRVCC mode */
            mcc_csfb_update_srvcc_mode(TRUE);
          }
          else
          {
            /* Update MC SRVCC mode to FALSE */
            mcc_csfb_update_srvcc_mode(FALSE);
          } /* CSFB Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */

          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );

          /* Activate the CDMA protocol in CSFB mode */
          mc_csfb_prot_activate();
          break;
        }
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

        case MC_CDMA_PROT_DEACT_CMD_F:
        {
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
          /* PPP_CLEANUP_NON_SV_MODE:Check if deregister powerup reg is pending
           * If yes, then set the flag to do powerup reg
           * with next protocal activate cmd */
          if( cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING )
          {
            /* Call dereg timer expiry API, which will take care of setting
             * flag to do powerup reg with next protocal activate cmd */
            mcc_proc_deregister_timer_expiry();
          } /* cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

          /* Protocol already in deactivated state, send deactivated
          ** confirmation anyways.
          */
          mmoc_rpt_prot_deactd_cnf( cmd_ptr->deact_prot.info.trans_id );
          mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD, MMOC_RPT_PROT_DEACTD_CNF,
                                    cmd_ptr->deact_prot.info.trans_id );
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          
          /* Keep track of the MMOC cmd for debugging purpose */
          mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state,
                                    FALSE, MC_PROCESS_CMD );          
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }

        case MC_UE_MODE_CHGD_CMD_F:
        {
          /* Update cdma UE mode global var's */
          cdma.ue_mode = cmd_ptr->ue_mode_cmd.ue_mode;
          cdma.is_ue_mode_substate_srlte = 
                              cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

          /* Send UE Mode cnf to MMOC */
          mmoc_1x_rpt_ue_mode_switch_cnf(cmd_ptr->ue_mode_cmd.trans_id, mcc_asid);

          /* Keep track of the MMOC cnf for debugging purpose */
          mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD, 
                                    MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                    cmd_ptr->ue_mode_cmd.trans_id );
          M1X_MSG( DCP, LEGACY_MED,
            "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
                   cmd_ptr->ue_mode_cmd.ue_mode,
                   cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
                   cmd_ptr->ue_mode_cmd.trans_id,
                   mcc_asid );

          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );

          /* Keep track of the MMOC cmd for debugging purpose */
          mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state,
                                    FALSE, MC_PROCESS_CMD );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );                     
      
          break;
        } /* MC_UE_MODE_CHGD_CMD_F */
      
        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        case MC_CSFB_DEACT_F:
#ifdef FEATURE_MODEM_1X_SRVCC
        case MC_SRVCC_DEACT_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
        {
          /* Send CM 1XCSFB / 1XSRVCC deactvation confirmation */
          cm_mc_rpt_type *cm_ptr;

          /* Clear the queue. */
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );

          if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
          {
#ifdef FEATURE_MODEM_1X_SRVCC
            if(cmd_ptr->hdr.cmd == MC_SRVCC_DEACT_F)
            {
              cm_ptr->cdma_csfb_deact_cnf.hdr.cmd = CM_1XSRVCC_DEACT_CNF_F;

              /* Update SRVCC mode to false */
              mcc_csfb_update_srvcc_mode(FALSE);
            }
            else
#endif /* FEATURE_MODEM_1X_SRVCC */
            {
              cm_ptr->cdma_csfb_deact_cnf.hdr.cmd = CM_1XCSFB_DEACT_CNF_F;
            }
            mcc_cm_mc_rpt( cm_ptr );
          }
          else
          {
            ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
          }
          break;
        } /* MC_CSFB_DEACT_F / MC_SRVCC_DEACT_F */
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

        case MC_CDMA_PROT_GEN_CMD_F:
        {
          /* Call protocol generic command proc, with not active flag
          ** set to TRUE.
          ** Addition range check added to appease klocwork.
          */
          if (cmd_ptr->gen_cmd.info.ss < SD_SS_MAX)
          {
            (void) mmoc_proc_prot_gen_cmd( TRUE, FALSE, &cmd_ptr->gen_cmd.info );
            mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD, MMOC_RPT_PROT_GEN_CMD_CNF,
                                      cmd_ptr->gen_cmd.info.trans_id );
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR, "System Selection type cannot be %d",
                       cmd_ptr->gen_cmd.info.ss );
          }
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          
          /* Keep track of the MMOC cmd for debugging purpose */
          mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state,
                                    FALSE, MC_PROCESS_CMD );          
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }

        case MC_CDMA_STANDBY_PREF_CHGD_CMD_F:
        {
          /* Process the CDMA Standby Pref changed command. */
          mc_process_cdma_standby_pref_chg_cmd();
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        } /* MC_CDMA_STANDBY_PREF_CHGD_CMD_F */
        
        case MC_CDMA_PROT_PH_STAT_CHGD_CMD_F:
        {
          /* Process the CDMA phone status command. */
          mc_process_cdma_ph_stat_cmd( &cmd_ptr->ph_stat_cmd.info );
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          
          /* Keep track of the MMOC cmd for debugging purpose */
          mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state,
                                    FALSE, MC_PROCESS_CMD );          
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }


        case MC_MO_DBM_F :
        {
          /* If an MO DBM request is received here, reject the request. */
          M1X_MSG( DCP, LEGACY_MED,
            "Rejecting MO DBM request in SD" );
          mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                  CM_DBM_MO_NO_SRV
                                );
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }



        case MC_BCSMS_ENABLE_F:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed BCSMS_ENABLE Cmd %d not in CDMA",
            cmd_ptr->hdr.cmd );
          mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }


        case MC_ORIGINATION_F:
        #ifdef FEATURE_MODEM_1X_DRVCC
        case MC_DRVCC_ORIGINATION_F:
        #endif /* FEATURE_MODEM_1X_DRVCC */            
        {
          /* Inform CM origination failed. */
          mcc_send_orig_failure( CM_CALL_ORIG_ERR_NO_SRV,
                                 (byte)(cmd_ptr->origination.call_id),
                                 CM_CALL_ORIG_ERR_LAYER_OTHER
                               );
          cdma.call_orig = FALSE;
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }

        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        case MC_PROT_REDIR_CNF_CMD_F:
        {
          /* TRUE - If Redir confirmed, FALSE - If Redir Aborted */
          boolean redir_cnf_status;
          
          redir_cnf_status = cmd_ptr->redir_cnf.redir_cnf_status;
          M1X_MSG( DCP, LEGACY_MED,
                   "DEBUG:Rxed REDIR_CNF_CMD_F with status:%d, irat_switch:%d",
                   redir_cnf_status, mc_irat_switch );
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          if ( mc_irat_switch == TRUE )
          {
            /* We received cnf for the IRAT switch ind, we are no longer
             * in transition state, hence set the corresponding var to FALSE
             */
            mc_irat_switch = FALSE;

            /* Check if MC confirmed REDIR/RESEL */
            if(redir_cnf_status == TRUE)
            {
              mc_cdma_activate_protocol(PROT_ACT_GWL_REDIR);
            } /* (redir_cnf_status == TRUE) */
            else
            {
              /* Indicate REDIR/RESEL failure to LTE-RRC */
              if (mc_redir_req_msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
              {
                mc_redir_failed_ind.cause = ONEX_CP_FAIL_REDIR;
                M1X_MSG( DCP, LEGACY_MED, 
                       "CSFB DEBUG: MC sending ONEX_CP_LTE_REDIR_FAILED_RSP" );
                mc_msgr_send_msg( &mc_redir_failed_ind.hdr,
                                  ONEX_CP_LTE_REDIR_FAILED_RSP,
                                  sizeof(mc_redir_failed_ind) );
                mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD, 
                                      MC_ONEX_CP_LTE_REDIR_FAILED_RSP, NULL );
     
                #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                    defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
                QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                 "QSH:ONEX_CP_LTE_REDIR_FAILED_RSP, cause:ONEX_CP_FAIL_REDIR");
                mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL);
                #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                          FEATURE_QSH_EVENT_NOTIFY_TO_QSH */
        
              } /* REDIR */
              else
              {
                mc_resel_failed_ind.cause = ONEX_CP_FAIL_RESEL;
                M1X_MSG( DCP, LEGACY_MED,
                       "CSFB DEBUG: MC sending ONEX_CP_LTE_RESEL_FAILED_RSP" );
                mc_msgr_send_msg( &mc_resel_failed_ind.hdr,
                                  ONEX_CP_LTE_RESEL_FAILED_RSP,
                                  sizeof(mc_resel_failed_ind) );
                mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD, 
                                      MC_ONEX_CP_LTE_RESEL_FAILED_RSP, NULL );
                                  
                #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                    defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
                QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                 "QSH:ONEX_CP_LTE_RESEL_FAILED_RSP, cause:ONEX_CP_FAIL_RESEL");
                mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL);
                #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                          FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                                  
              } /* RESEL */

              /* Since we started Redirection transaction with MMOC, when we
               * sent Redirection Indication. MMOC after sending REDIR_CNF, 
               * MMOC will be waiting either for Auto Activation/Deactivation
               * Indication to cancel the previously started Redirection 
               * transaction at MMOC.
               */
              M1X_MSG( DCP, LEGACY_HIGH, 
                       "CSFB DEBUG: sending MMOC AUTO DEACT IND" );
              mmoc_rpt_prot_auto_deactd_ind(PROT_DEACT_REDIR_ABORT, 
                                            PROT_STATE_ONLINE_CDMA);
              mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD, MMOC_RPT_PROT_AUTO_DEACTD_IND,
                                        PROT_TRANS_NONE );
            } /* (redir_cnf_status == FALSE) */
          } /* (mc_irat_switch == TRUE) */
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "MC_PROT_REDIR_CNF_CMD_F rxed in wrong state.. ignoring");
            is_ignored = TRUE;
          }
          /* Keep track of the MMOC cmd for debugging purpose */
          mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state,
                                    is_ignored, MC_PROCESS_CMD );
          /* Reset the flag */
          is_ignored = FALSE;
          break;
        }
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

        #ifdef FEATURE_CSIM
        case MC_MMGSDI_CLIENT_ID_REG_CNF_F:
        {
          /* Received confirmation for client id registration , now time to
           * perform session id registration.
           */
          M1X_MSG( DCP, LEGACY_MED,
            "Client ID registration success..,perform session id reg..");
          if ( mmgsdi_session_open( cdma.client_id,
                                    MMGSDI_1X_PROV_PRI_SESSION,
                                    NULL,
                                    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
                                    mc_mmgsdi_evt_cb,
                                    #else
                                    NULL,
                                    #endif
                                    FALSE,
                                    mcmmgsdi_session_reg_cb,
                                    ( mmgsdi_client_data_type )
                                    MMGSDI_1X_PROV_PRI_SESSION ) != MMGSDI_SUCCESS)
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "MMGSDI MC session id reg failed" );
            cdma.client_id = MC_INVALID_CDMA_CLIENT_ID;
            cdma.session_id = MC_INVALID_CDMA_SESSION_ID;
          }

          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        } /* MC_MMGSDI_CLIENT_ID_REG_CNF_F */
        #endif /* FEATURE_CSIM */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
        case MC_DEREGISTER_F:
        {
          /* PPP_CLEANUP_NON_SV_MODE: Reject deregister cmd while we are not in CDMA */
          M1X_MSG( DCP, LEGACY_MED,
            "Rejecting deregister_f cmd while not in CDMA");
          cdma.deregister_state = MCC_DEREGISTER_RSP_PENDING;
          mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        } /* MC_DEREGISTER_F */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_CONFIG_REFRESH 
        case MC_NV_REFRESH_F:
        {
          /* This API will refresh all the required NV items */
          mc_process_nv_refresh();

          /* Req_module indicates whether this NV refresh is cominmg from
          ** Diag or MCFG module. If the NV refresh is from MCFG, then 
          ** we need to inform MCFG of refresh done indication */
          if (cmd_ptr->nv_refresh.req_module == NV_REFRESH_MCFG)
          {
            mc_send_refresh_done( cmd_ptr->nv_refresh.reference_id );
          }
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_1XCP_QSH_SUPPORT
        case MC_QSH_CB_CMD_F:
        {
          mc_process_qsh_cmd(&cmd_ptr->qsh_cmd.qsh_params);
          q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
          mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
        }
#endif /* FEATURE_1XCP_QSH_SUPPORT */       
          
        default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "Ignored unexpected command %d",
              cmd_ptr->hdr.cmd );
            q_delete( &mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
            mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
          break;
      } /* switch ( mc_cmd.hdr.cmd ) */
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      M1X_MSG( DCP, LEGACY_HIGH,
        "Remaining elements in mc_cmd_q are %d",
        q_cnt( &mc_cmd_q ));
    }
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    else if((mc_msgr_msg_ptr = (mc_msgr_rcv_msg_type*) q_check( &mc_msgr_rcv.msg_q )) != NULL)
    {
      M1X_MSG( DCP, LEGACY_MED,
               "CSFB DEBUG: Rxed msgr cmd = %d in state = %d",
               mc_msgr_msg_ptr->msg.hdr.id, 
               cdma.curr_state );
      switch ( mc_msgr_msg_ptr->msg.hdr.id )
      {
        case ONEX_CP_LTE_REDIR_REQ:
        case ONEX_CP_LTE_RESEL_REQ:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB:DEBUG - received msgr msg : %d",
            mc_msgr_msg_ptr->msg.hdr.id);
          mcc_enqueue_dbg_msgr_cmd( mc_msgr_msg_ptr, old_state,
		                           cdma.curr_state, MC_PROCESS_CMD);
          mc_irat_switch = mc_proc_irat_switch(mc_msgr_msg_ptr);
          if (mc_irat_switch == TRUE)
          {
            /* indicate MMOC of the protocol activation */
            mmoc_cmd_prot_redir_ind( PROT_ACT_GWL_REDIR,
                                     PROT_STATE_ONLINE_CDMA );
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "CSFB:DEBUG - redir/resel validation failed");

            /* indicte redirection failure */
            if(mc_msgr_msg_ptr->msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
            {
              mc_redir_failed_ind.cause = ONEX_CP_FAIL_REDIR;
              M1X_MSG( DCP, LEGACY_MED,
                "CSFB DEBUG: MC sending ONEX_CP_LTE_REDIR_FAILED_RSP");
              mc_msgr_send_msg( &mc_redir_failed_ind.hdr,
                                ONEX_CP_LTE_REDIR_FAILED_RSP,
                                sizeof(mc_redir_failed_ind) );
              mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD_1, 
                                    MC_ONEX_CP_LTE_REDIR_FAILED_RSP, NULL );
                                
              #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                  defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
              QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
               "QSH:ONEX_CP_LTE_REDIR_FAILED_RSP, cause:ONEX_CP_FAIL_REDIR");
              mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL);
              #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                        FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                                
            }
            else
            {
              mc_resel_failed_ind.cause = ONEX_CP_FAIL_RESEL;
              M1X_MSG( DCP, LEGACY_MED,
                "CSFB DEBUG: MC sending ONEX_CP_LTE_RESEL_FAILED_RSP");
              mc_msgr_send_msg( &mc_resel_failed_ind.hdr,
                                ONEX_CP_LTE_RESEL_FAILED_RSP,
                                sizeof(mc_resel_failed_ind) );
              mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD_1, 
                                    MC_ONEX_CP_LTE_RESEL_FAILED_RSP, NULL );
                                
              #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                  defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
              QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
               "QSH:ONEX_CP_LTE_RESEL_FAILED_RSP, cause:ONEX_CP_FAIL_RESEL");
              mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL);
              #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                        FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                                
            }
          }
          break;
        }
        case ONEX_CP_LTE_ABORT_REDIR_REQ:
        case ONEX_CP_LTE_ABORT_RESEL_REQ:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB:DEBUG - Abort rxed for msg : %d",
            mc_msgr_msg_ptr->msg.hdr.id);

          /* indicte redirection failure */
          if( mc_msgr_msg_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "CSFB DEBUG: MC sending ONEX_CP_LTE_ABORT_REDIR_RSP");
            mc_msgr_send_msg( &mc_redir_abort_ind.hdr,
                              ONEX_CP_LTE_ABORT_REDIR_RSP,
                              sizeof(onex_cp_lte_abort_redir_rsp_msg_t) );
            mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD_1, 
                                  MC_ONEX_CP_LTE_ABORT_REDIR_RSP, NULL );
                              
            #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
            QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
             "QSH:ONEX_CP_LTE_ABORT_REDIR_RSP");
            mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL);
            #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                      FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                              
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "CSFB DEBUG: MC sending ONEX_CP_LTE_ABORT_RESEL_RSP");
            mc_msgr_send_msg( &mc_resel_abort_ind.hdr,
                              ONEX_CP_LTE_ABORT_RESEL_RSP,
                              sizeof(onex_cp_lte_abort_resel_rsp_msg_t) );
            mcc_enqueue_dbg_mmoc_cnf( MC_PROCESS_CMD_1, 
                                  MC_ONEX_CP_LTE_ABORT_RESEL_RSP, NULL );
                              
            #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
            QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
             "QSH:ONEX_CP_LTE_ABORT_RESEL_RSP");
            mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL);
            #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                      FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                              
          }
          mc_irat_switch = FALSE;

          mcc_enqueue_dbg_msgr_cmd( mc_msgr_msg_ptr, old_state,
                                    cdma.curr_state, MC_PROCESS_CMD_1 );
          break;
        }

        #ifdef FEATURE_1X_TO_LTE
        case ONEX_CP_LTE_DEPRI_FREQ_REQ:
        {
          mcc_csfb_proc_msgr_rx(mc_msgr_msg_ptr);

          break;
        } /* ONEX_CP_LTE_DEPRI_FREQ_REQ */
        #endif /* FEATURE_1X_TO_LTE */

        case ONEX_CP_LTE_GET_CGI_REQ:
        {
          if( (mc_msgr_msg_ptr = 
              (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL )
          {
            mcc_csfb_proc_msgr_rx(mc_msgr_msg_ptr);
        }
          
          break;
        } /* ONEX_CP_LTE_GET_CGI_REQ */

        case ONEX_CP_LTE_ABORT_CGI_REQ:
        {
          /* Unexpected CGI Abort Req from LTE-RRC while we are 
           * not yet CSFB activated so log an error and ignore CGI Abort Req */ 
          M1X_MSG( DCP, LEGACY_ERROR,
                   "CSFB CGI:Unexpected CGI Abort Req in inactive state:%d, msg:%d",
                   cdma.curr_state, 
                   mc_msgr_msg_ptr->msg.hdr.id );
          break;
        } /* ONEX_CP_LTE_ABORT_CGI_REQ */

        case LTE_RRC_1X_SIB8_INFO_IND:
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: received LTE_RRC_1X_SIB8_INFO_IND");
          mcc_csfb_process_sib8_msg(mc_msgr_msg_ptr);

          /* Mark the mcc_csfb_wait_for_sib8_before_doing_reg as FALSE after receiving SIB8. */
          if(TRUE == mcc_csfb_wait_for_sib8_before_doing_reg)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "After call end, SIB8 has been rxed while we are not CSFB activated)");
            mcc_csfb_wait_for_sib8_before_doing_reg = FALSE;
          }
          break;
        }

        case ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ:
        {
          onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t lte_abort_1xcsfb_ho_rsp;
          /* Indication for LTE Abort Handoff Request. */
   
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: received ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ");
          /* Sending the response for LTE Abort HO Req */  
          mc_msgr_send_msg(&lte_abort_1xcsfb_ho_rsp.hdr,
                           ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP,
                           sizeof(onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t)); 

          break;
        }/* ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ */

        case NAS_EMM_IRAT_CTXT_LOST_IND:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: Received CTXT_LOST_IND in inactive state");
          /* We have received Context Loss indication while we are not yet
           * CSFB activated, based on the reason for context loss set the context
           * loss indication. We will have to force the registration in case there
           * was a prior context loss seen, either in MC's inactive state or on CSFB
           * state. Note that we will perform a forced registration only when we are
           * CSFB activated. If we go from inactive state to native 1x mode, it is a
           * no-op.
           */

          /* If Context Loss has occured because of IMSI Page, we will wait for
           * Attach With IMSI to set the forced registration flag to true.
           * If Context Loss has occured because of Identity Req triggered from
           * n/w we will immediately cset forced registration flag to TRUE.
           * In this case we will not wait for attach because there might
           * be some Identity Requests which doesnt need IMSI Atatch incase it
           * already has GUTI.
           */

          if (mc_msgr_msg_ptr->msg.ctxt_lost_ind_type.cause == LTE_NAS_CTXT_LOST_IMSI_PAGING)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "CSFB DEBUG: Rxed CTXT_LOST for IMSI_PAGING");
            /* Store the reason why Context lost was received. This reason will be used
             * when triggering registration if Attach with IMSI is received subsequently.
             */
            mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IMSI_PAGING;
          }
          else if(mc_msgr_msg_ptr->msg.ctxt_lost_ind_type.cause == LTE_NAS_CTXT_LOST_IDENTITY_REQUEST)
          {
            mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IDEN_REQ;
          }
          break;
        } /* NAS_EMM_IRAT_CTXT_LOST_IND */

        case NAS_EMM_IRAT_IMSI_ATTACH_IND:
        {
          /* We have received IMSI attach indication. Needs to do
             forcebly trigger reg in tunnel mode.
           */
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: Rxed IMSI_ATTACH_IND - %d",mcc_csfb_cntx_loss_ind);
            mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IMSI_ATTACH;
          break;
        } /* NAS_EMM_IRAT_IMSI_ATTACH_IND */
        
        case NAS_EMM_ATTACH_COMPLETE_IND :
        {
          /* We have received IMSI Complete indication. Need to
             forcebly trigger reg in tunnel mode.
           */
          M1X_MSG( DCP, LEGACY_HIGH,
                   "CSFB DEBUG: Rxed ATTACH_COMPLETE_IND in inactive state" );
          mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IMSI_ATTACH;
          break;
        } /* NAS_EMM_ATTACH_COMPLETE_IND  */
        
        default:
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB :DEBUG - MSGR CMD = %d, not expected",
            mc_msgr_msg_ptr->msg.hdr.id );
        break;
      }

      /* Delete the item from msg q.*/
      q_delete( &mc_msgr_rcv.msg_q, &mc_msgr_msg_ptr->msg_hdr.qlink );

      /* The processing is over. Now return the buffer to the free queue */
      msgr_rex_free_msg_buf(&mc_msgr_msg_ptr->msg_hdr.qlink);

      M1X_MSG( DCP, LEGACY_HIGH,
        "Remaining elements in mc_msgr_rcv.msg_q  are %d",
        q_cnt( &mc_msgr_rcv.msg_q ));
    } /* if(mc_msgr_msg_ptr != NULL) */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  
    #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
    else if( mcc_timer_on_callback_q(MCC_BS_INFO_HEAP_MEM_TIMER_ID) )
    {
      /* Clear bs_info[] heap memory cleanup timer */
      (void) mcc_clr_callback_timer( &bs_info_heap_mem_timer,
                                     MCC_BS_INFO_HEAP_MEM_TIMER_ID );      
      /* Free bs_inf[] array */
      mcc_free_bs_info_array();
    }
    #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */
    
  }while((NULL != q_check(&mc_cmd_q))
         #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
           || (NULL != q_check(&mc_msgr_rcv.msg_q))
         #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
         #ifdef FEATURE_1X_CP_IGNORE_GSRDM
         || (mcc_timer_on_callback_q(MCC_GSRDM_IGNORE_TIMER_ID))
         #endif /* FEATURE_1X_CP_IGNORE_GSRDM */
         #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
         || (mcc_timer_on_callback_q(MCC_BS_INFO_HEAP_MEM_TIMER_ID))
         #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */
        );

} /* mc_process_cmd() */

/*===========================================================================

FUNCTION MC_TASK

DESCRIPTION
  This function is the entrance for the Main Control task.  It is the
  task responsible for creating all other software tasks and timers,
  initializing all services, and then starting up all other software
  tasks.  After an aknowledgement has been received from each software
  task indicating that it has started successfully, this function calls
  the ACP and CDMA subtasks power on initialization point so that they
  can perform their initialization.  The function then enters system
  determination where analog, digital call processing or LPM is invoked.
  Control returns from system determination when reset or powerdown
  are to be performed.  In either case DMSS powerdown is invoked,
  followed by either reset or powerdown.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Reset and powerdown processing are special in that they cause the
  complete DMSS software to either restart or to be shutdown.

===========================================================================*/

void mc_task
(
  dword ignored
    /* Parameter received from Rex - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_init().
    */
)
{

  dword sigs = 0;                      /* Signals returned by mc_wait */
/*---------------------------------------------------------------------------
**
**             DEFINE MULTI MODE FEATURE
**
**-------------------------------------------------------------------------*/

  /*---------------------*/
  /* Task initialization */
  /*---------------------*/
  mc_task_init();

  /* Start handshake with RCINIT to start the init process */
  rcinit_handshake_startup();

  /* Add mc task report command processing here */

  while ( TRUE ) //lint !e716
  {
    /* Process the received command.
    */
    mc_process_cmd();

    sigs = mc_wait(MC_CMD_Q_SIG
    #if defined(FEATURE_1X_CP_IGNORE_GSRDM) || defined(FEATURE_1X_CP_HEAP_MEM_CLEANUP)
                   | MCC_CALLBACK_TIMER_SIG
    #endif /* FEATURE_1X_CP_IGNORE_GSRDM || FEATURE_1X_CP_HEAP_MEM_CLEANUP */
                  );
    (void) rex_clr_sigs(MC_TCB_PTR, sigs);

  } /* while ( true ) */

} /* mc_task */

/*===========================================================================

FUNCTION mc_get_buf_else_err_fatal

DESCRIPTION
  Allocate a MC Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MC command buffer.

SIDE EFFECTS
  none
===========================================================================*/

mc_msg_type *mc_get_buf_else_err_fatal( void )
{

  /* Allocate memeory from modem heap */
  mc_msg_type *mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If allocation failed, error fatal. */
  if( mc_cmd_ptr == NULL )
  {
    ERR_FATAL("Memory allocation failed for mc cmd",0,0,0);
    /* return NULL; */
  }
  else
  {
    mc_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
  /* If we got here, allocation is successful,
   * so specify NO task to be signalled when done.
   */
  }

  return mc_cmd_ptr;

} /* mc_get_buf_else_err_fatal() */

/*===========================================================================

FUNCTION MC_CMD

DESCRIPTION
  This is the outside world's interface to the MC task and subtasks.
  this function takes an already filled out mc_msg_type and places it
  on the mc command queue.  The function returns to the caller after
  setting the status to busy indicaton.  This does _not_ mean that the
  command has been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The MC task must have been started up already.  All of the items in
  the mc_cmd_type must be already set.

RETURN VALUE
  None directly.  The status variable of the mc_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The mc_cmd_type is placed on the MC command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void mc_cmd
(
  mc_msg_type  *msg_ptr   /* Pointer to MC message buffer */
)
{
  switch (msg_ptr->hdr.cmd)
  {
    case MC_CHECK_PSM_READY_REQ_F:
    {
      /* Here 1X checks if it is ready to enter Power Saving Mode (PSM). */

       M1X_MSG( DCP, LEGACY_HIGH,
          "PSM: Rxed cmd MC_CHECK_PSM_READY_REQ_F while ready_ind_pending = %d in state %d",
              cdma.psm_ready_ind_pending, cdma.curr_state);
       cm_mc_rpt_type* cm_ptr;
       cdma.psm_status = SYS_PSM_STATUS_NONE;

       if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
       {       
          cm_ptr->psm_resp_info.hdr.cmd = CM_MC_PSM_READY_RSP_F;  
          /* The PSM status of 1X is SYS_PSM_STATUS_READY if
           * 1. 1X is in idle state and not handling any MO or MT calls
           * 2. 1X is in CSFB_IDLE state and not handling any MO or MT calls
           * 3. 1X is in CDMA_ENTER state
           * 4. 1X is in CDMA_EXIT state
          */
          if( (MAIN_STATE(cdma.curr_state) == CDMA_ENTER) ||
              (MAIN_STATE(cdma.curr_state) == CDMA_EXIT)||
              ( (MAIN_STATE(cdma.curr_state) == CDMA_IDLE) &&
                (cdma.sa_tc_state != CDMA_ORIG) &&
                (cdma.sa_tc_state != CDMA_PAGE_RES)&&
                (((cdma.ho_sa_tc_state != CDMA_ORIG)&&
                  (cdma.ho_sa_tc_state != CDMA_PAGE_RES)
                 ) || 
                 (!cdma.transceiver_request_pending )
                )
              ) 
          #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
              || (cdma.curr_state == CDMA_CSFB_IDLE)
          #endif
            )
          {
             cm_ptr->psm_resp_info.status = SYS_PSM_STATUS_READY;
             cdma.psm_ready_ind_pending = FALSE;
          }
          else
          {
             /* 1X is not ready to enter PSM mode. The PSM_READY indication 
              * will be sent once 1X is back in idle state and going to 
              * start sleep in native 1X mode whereas in CSFB mode, the 
              * indication will be sent when we are back in CSFB_IDLE state.
             */
             cm_ptr->psm_resp_info.status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
             cdma.psm_ready_ind_pending = TRUE;
          }
          mcc_cm_mc_rpt( cm_ptr );
          cdma.psm_status = cm_ptr->psm_resp_info.status;
          M1X_MSG( DCP, LEGACY_HIGH,
           "PSM: Sent CM_MC_PSM_READY_RSP_F with status %d, ready_ind_pending = %d",
              cdma.psm_status,cdma.psm_ready_ind_pending);
       }	
       else
       {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on cm_mc_rpt_free_q" );
       }
       /* Release the memory and return it to the appropriate free queue.*/
       msg_ptr->hdr.status = MC_DONE_S;
       mc_free_queue_buf( msg_ptr, MC_CMD_BUF_Q );
       break;
    }
	
    #ifdef FEATURE_DDTM_CNTL       /* Data Dedicated mode access prevention cmd */
    case MC_DDTM_CNTL_F:
    {
      mcc_set_ddtm_status(&msg_ptr->ddtm_cntl);

      /* Release the memory and return it to the appropriate free queue.
      */
      msg_ptr->hdr.status = MC_DONE_S;
      mc_free_queue_buf( msg_ptr, MC_CMD_BUF_Q );
      break;
    }
    #endif /* FEATURE_DDTM_CNTL */

    default:
    {
      /* Set the request status to busy. initialize and link the command  */
      /* onto the MC command queue, set a signal to the MC task and exit. */

      msg_ptr->hdr.status = MC_BUSY_S;
      (void) q_link(msg_ptr, &msg_ptr->hdr.cmd_hdr.link);
      q_put(&mc_cmd_q, &msg_ptr->hdr.cmd_hdr.link);
      (void) rex_set_sigs(MC_TCB_PTR, MC_CMD_Q_SIG);

      break;
    }
  }

} /* mc_cmd */


/*===========================================================================

FUNCTION MC_QUEUE_CMD

DESCRIPTION
  This is the outside world's interface to the MC task and subtasks.
  This function will unqueue an MC command from the MC_CMD_BUF_Q and
  fill it out and places it on the mc command queue.  This does _not_
  mean that the command has been executed yet, just that it is waiting
  to be executed.

DEPENDENCIES
  The MC task must have been started up already.

RETURN VALUE
  mc_queue_cmd_status_type

SIDE EFFECTS
  None.

===========================================================================*/

mc_queue_cmd_status_type  mc_queue_cmd
(
  mc_name_type cmd_type
)
{
  mc_msg_type *mc_cmd_ptr;

  REX_ISR_LOCK(&mc_crit_sect);

  /*------------------------------------------------------------------------
    Get an MC command link, load up the REX-related fields, and return the
    pointer
  -------------------------------------------------------------------------*/
  if(( mc_cmd_ptr = (mc_msg_type *)mc_alloc_queue_buf( MC_CMD_BUF_Q )) == NULL)
  {
    return MC_FREE_QUEUE_EMPTY;
  }
  else /* load up the fields */
  {
    mc_cmd_ptr->hdr.cmd                = cmd_type;
    mc_cmd_ptr->hdr.cmd_hdr.task_ptr   = MC_TCB_PTR;

    /* Put NULL for the signal return for now, figure out what signal
       need to put here later. */
    mc_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
  }

  mc_cmd (mc_cmd_ptr);

  REX_ISR_UNLOCK(&mc_crit_sect);

  return MC_CMD_QUEUED;

} /* mc_queue_cmd */

/*===========================================================================

FUNCTION MC_STATE

DESCRIPTION
  This function returns the current state of MC.

DEPENDENCIES
  None.

RETURN VALUE
  Current state of MC.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_state( void )
{
  byte state;
    /* Current state of the phone */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* not FEATURE_SD20_LOGGING */
  db_get(DB_DMSS_STATE, &mc_db_item);
  switch (mc_db_item.dmss_state)
  {
    case DB_DMSS_POWERUP_S:
    case DB_DMSS_RESET_S:
    case DB_DMSS_POWERDOWN_S:
      state = DIAG_NO_SVC_S;
      break;

    case DB_DMSS_ONLINE_DIGITAL_S:
    case DB_DMSS_OFFLINE_DIGITAL_S:
      state = mcc_get_state();
      break;

    default:
      state = DIAG_NO_SVC_S;
      break;
  }

  return (state);

} /* mc_state */

/*===========================================================================

FUNCTION MC_GET_BAND_CLASS

DESCRIPTION
  This function returns the current operating band class of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current band class.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_get_band_class
(
  void
)
{
  return cdma.band_class;

} /* mc_get_band_class */

/*===========================================================================

FUNCTION MC_GET_P_REV_IN_USE

DESCRIPTION
  This function returns the current P_REV_IN_USE of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current P_REV_IN_USE.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_get_p_rev_in_use
(
  void
)
{
  return cdma.p_rev_in_use;

} /* mc_get_p_rev_in_use */

/*===========================================================================

FUNCTION MC_DS_GET_P_REV_IN_USE

DESCRIPTION
  This function returns the current P_REV_IN_USE of the mobile to
  DS task.

DEPENDENCIES
  None.

RETURN VALUE
  Current P_REV_IN_USE.

SIDE EFFECTS
  None.

===========================================================================*/
byte mc_ds_get_p_rev_in_use
(
  void
)
{
  return cdma.ds_p_rev_in_use;

} /* mc_ds_get_p_rev_in_use */

/*===========================================================================

FUNCTION MC_GET_MOB_CAI_REV

DESCRIPTION
  This function returns the protocol revision number of the mobile station.

DEPENDENCIES
  None.

RETURN VALUE
  The mobile's protocol revision number.

SIDE EFFECTS
  None.

===========================================================================*/

byte mc_get_mob_cai_rev
(
  void
)
{
  return cdma.mob_cai_rev;

} /* mc_get_mob_cai_rev */

/*===========================================================================
FUNCTION mc_cdma_activate_protocol

DESCRIPTION
  This function commands the MC task to activate the CDMA protocol to do
  either  online acquisition, offline operation, FTM etc specified in the
  act_reason.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_activate_protocol
(
  prot_act_e_type act_reason
    /* Reason for activating the CDMA protocol. */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_ACT_CMD_F;
  cmd_ptr->act_prot.info.reason        = act_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_cdma_activate_protocol() */

/*===========================================================================
FUNCTION mc_cdma_deactivate_protocol

DESCRIPTION
  This function commands the MC task to deactivate the CDMA protocol.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_deactivate_protocol
(
  prot_deact_e_type deact_reason,
    /* Reason for deactivating the CDMA protocol. example, PWR_DOWN,
    ** RESET etc.
    */

  prot_trans_type trans_id
    /* Transaction Id for this request. */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_DEACT_CMD_F;
  cmd_ptr->deact_prot.info.reason      = deact_reason;
  cmd_ptr->deact_prot.info.trans_id    = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);


  return;

} /* mc_cdma_deactivate_protocol() */

/*===========================================================================
FUNCTION mc_cdma_standby_pref_chgd_cmd

DESCRIPTION
  This function should be called to inform the CDMA protocol of the change
  in standby preference mode, the protocol will initiate the housekeeping 
  tasks if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_cdma_standby_pref_chgd_cmd( void )
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields (No payload for this cmd) */
  cmd_ptr->hdr.cmd                     = MC_CDMA_STANDBY_PREF_CHGD_CMD_F;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;
} /* mc_cdma_standby_pref_chgd_cmd */

/*===========================================================================
FUNCTION mc_cdma_ue_mode_chgd_cmd

DESCRIPTION
  This function should be called to inform the CDMA protocol of the change
  in UE mode, the protocol will initiate the housekeeping tasks if
  necessary.
  This API is called from MMOC context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_cdma_ue_mode_chgd_cmd
(
  sys_modem_as_id_e_type asubs_id,
    /* Active Subscription */

  prot_trans_type trans_id,
    /* Transaction Id related to UE mode chg cmd */

  sys_ue_mode_e_type ue_mode,
    /* Type of UE mode status change */

  boolean is_ue_mode_substate_srlte
    /* Current UE mode substate SRLTE status */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_UE_MODE_CHGD_CMD_F;
  cmd_ptr->ue_mode_cmd.trans_id        = trans_id;
  cmd_ptr->ue_mode_cmd.ue_mode         = ue_mode;
  cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte = is_ue_mode_substate_srlte;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( 
         #ifdef GPP2_SUPPORT_ON_SEC_SUB
         asubs_id, 
         #endif /* GPP2_SUPPORT_ON_SEC_SUB */
         cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_cdma_ue_mode_chgd_cmd() */

/*===========================================================================
FUNCTION mc_cdma_ph_status_chgd_cmd

DESCRIPTION
  This function should be called to inform the CDMA protocol of the change
  in phone status, the protocol will initiate the housekeeping tasks if
  necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_ph_status_chgd_cmd
(
  prot_ph_stat_chg_e_type chg_type,
    /* Type of phone status change( NAM change, operating mode change etc.) */

  byte nam,
    /* Current NAM selected by the user. */

  boolean is_cdma_subs_avail,
    /* Current CDMA subscription availability status. */

  prot_oprt_mode_e_type oprt_mode,
    /* Current operating mode of the phone. */

  prot_trans_type trans_id,
    /* Transaction Id for this request. */

  prot_subsc_chg_e_type prot_subsc_chg
    /* Enumeration of protocols that have a change in subscription
       available status */
)
{
  /* MC command pointer. */
  mc_msg_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_PH_STAT_CHGD_CMD_F;
  cmd_ptr->ph_stat_cmd.info.chg_type   = chg_type;
  cmd_ptr->ph_stat_cmd.info.nam        = nam;
  cmd_ptr->ph_stat_cmd.info.is_cdma_subs_avail =
                                         is_cdma_subs_avail;
  cmd_ptr->ph_stat_cmd.info.oprt_mode  = oprt_mode;
  cmd_ptr->ph_stat_cmd.info.trans_id   = trans_id;
  cmd_ptr->ph_stat_cmd.info.prot_subsc_chg = prot_subsc_chg;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_cdma_ph_status_chgd_cmd() */

/*===========================================================================
FUNCTION mc_cdma_generic_prot_cmd

DESCRIPTION
  This function should be used to command the CDMA protocol stack to process
  the generic command from MMoC in its MC's task context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void mc_cdma_generic_prot_cmd
(
  prot_gen_cmd_s_type *gen_cmd_ptr
    /* Pointer to the generic command information. */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_CDMA_PROT_GEN_CMD_F;
  cmd_ptr->gen_cmd.info                = *gen_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_cdma_generic_prot_cmd() */

#ifdef FEATURE_1XCP_QSH_SUPPORT
/*===========================================================================
FUNCTION MC_CDMA_QSH_COMMAND

DESCRIPTION
  This function queues a command received from QSH module on to the MC task.
  This is done to ensure the cmd gets processed in MC task context only to avoid 
  any data integrity issues if the cmd was processed in QSH task context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_qsh_command
(
  qsh_client_cb_params_s *qsh_cb_cmd
  /* Cmd received from QSH */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;
  size_t copy_size;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* If there are multiple items on mc_cmd_q, then MC task must be blocked 
   * somewhere or not monitoring MC_CMD_Q signal for some time. 
   * When MC finally gets unblocked, it will process all these pending reports
   * in succession in a very small amount of time (~1 ms). Thus if there are  
   * multiple Timer Expiry based Metrics reporting QSH commands pending in 
   * the cmd queue, then when they are finally processed, all of them will 
   * report almost the same metrics. 
   * So it doesn't make much sense to keep queuing up Timer Expiry based 
   * Metrics QSH cmds and we can drop them after a cutoff is reached 
   * on mc_cmd_q.
   */
  if ( (qsh_cb_cmd->action == QSH_ACTION_METRIC_TIMER_EXPIRY) &&
       (q_cnt( &mc_cmd_q ) >= MC_QSH_TIMER_METRICS_CUTOFF_CNT) 
     )
  {  
    REX_ISR_UNLOCK(&mc_crit_sect);
    return;
  }  
  
  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_QSH_CB_CMD_F;
  
  copy_size =  memscpy( &cmd_ptr->qsh_cmd.qsh_params,
                       sizeof(qsh_client_cb_params_s),
                       qsh_cb_cmd,
                       sizeof(qsh_client_cb_params_s));

  if( copy_size != sizeof(qsh_client_cb_params_s) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      sizeof(qsh_client_cb_params_s),
      sizeof(qsh_client_cb_params_s) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_cdma_qsh_command */
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================
FUNCTION mc_amps_activate_protocol

DESCRIPTION
  This function commands the MC task to activate the AMPS protocol to do
  either  online acquisition, offline operation etc specified in the
  act_reason.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_activate_protocol
(
  prot_act_e_type act_reason
  /* Reason for activating the AMPS protocol. */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_ACT_CMD_F;
  cmd_ptr->act_prot.info.reason        = act_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_amps_activate_protocol() */

/*===========================================================================
FUNCTION mc_amps_deactivate_protocol

DESCRIPTION
  This function commands the MC task to deactivate the AMPS protocol.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_deactivate_protocol
(
  prot_deact_e_type deact_reason,
    /* Reason for deactivating the AMPS protocol. example, PWR_DOWN,
    ** RESET etc. */

  prot_trans_type trans_id
    /* Transaction Id for this request.  */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_DEACT_CMD_F;
  cmd_ptr->deact_prot.info.reason      = deact_reason;
  cmd_ptr->deact_prot.info.trans_id    = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_amps_deactivate_protocol() */

/*===========================================================================
FUNCTION mc_amps_ph_status_chgd_cmd

DESCRIPTION
  This function should be called to inform the AMPS protocol of the change
  in phone status, the protocol will initiate the housekeeping tasks if
  necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_ph_status_chgd_cmd
(
  prot_ph_stat_chg_e_type chg_type,
    /* Type of phone status change( NAM change, operating mode change etc.) */

  byte nam,
    /* Current NAM selected by the user. */

  boolean is_cdma_subs_avail,
    /* Current CDMA subscription availability status. */

  prot_oprt_mode_e_type oprt_mode,
    /* Current operating mode of the phone. */

  prot_trans_type trans_id,
    /* Transaction Id for this request. */

  prot_subsc_chg_e_type prot_subsc_chg
    /* Enumeration of protocols that have a change in subscription available
       status */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_PH_STAT_CHGD_CMD_F;
  cmd_ptr->ph_stat_cmd.info.chg_type   = chg_type;
  cmd_ptr->ph_stat_cmd.info.nam        = nam;
  cmd_ptr->ph_stat_cmd.info.is_cdma_subs_avail =
                                         is_cdma_subs_avail;
  cmd_ptr->ph_stat_cmd.info.oprt_mode  = oprt_mode;
  cmd_ptr->ph_stat_cmd.info.trans_id   = trans_id;
  cmd_ptr->ph_stat_cmd.info.prot_subsc_chg = prot_subsc_chg;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_amps_ph_status_chgd_cmd() */

/*===========================================================================
FUNCTION mc_amps_generic_prot_cmd

DESCRIPTION
  This function should be used to command the AMPS protocol stack to process
  the generic command from MMoC in MC's task context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_amps_generic_prot_cmd
(
  prot_gen_cmd_s_type              *gen_cmd_ptr
    /* Pointer to the generic command information.  */
)
{
  /* MC command pointer.
  */
  mc_msg_type                    *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer.
  */
  cmd_ptr = mc_get_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  cmd_ptr->hdr.cmd                     = MC_AMPS_PROT_GEN_CMD_F;
  cmd_ptr->gen_cmd.info                = *gen_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

  return;

} /* mc_amps_generic_prot_cmd() */

/*===========================================================================

FUNCTION MC_UPDATE_WATERMARK

DESCRIPTION
  This function checks and updates the MC's queue watermark against the
  history value. Note it does not write to NV.

DEPENDENCIES
  MC's queue watermark has to be retrieved from NV upon powerup.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mc_update_watermark
(
  mc_wm_q_type  q_name
)
{
  MODEM_1X_UNUSED(q_name);

} /* mc_update_watermark */


/*===========================================================================

FUNCTION MC_CDMA_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
  The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mc_cdma_multimode_sanity_err_fatal
(
  mmoc_sanity_err_e_type reason
)
{
  if ( MAIN_STATE ( cdma.curr_state ) != CDMA_IDLE )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "MMoc sanity timer expired: cmd %d, state %d",
      reason,
      cdma.curr_state);
  }
  else
  {
    switch ( reason )
    {
      case MMOC_SANITY_ERR_DEACT_CMD_CNF:
        /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_DEACT_CMD_F
        ** The CDMA protocol should respond within 15 seconds, in case of
        ** deactivating because of power down it is 30 seconds.
        */
        ERR_FATAL( "CDMA did not respond to  deactivate command", 0, 0, 0 );
        break;

      case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
        /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_PH_STAT_CHGD_CMD_F
        ** The CDMA protocol should respond within 15 seconds.
        */
        ERR_FATAL( "CDMA did not respond to phone status chgd command", 0, 0, 0 );
        break;

      case MMOC_SANITY_ERR_GENERIC_CMD_CNF:
        /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_GEN_CMD_F( because
        ** of pref_sys changed or SD reselection timeout.
        ** The CDMA protocol should respond within 15 seconds.
        */
        ERR_FATAL( "CDMA did not respond to generic command", 0, 0, 0 );
        break;

      default:
       ERR_FATAL( "Unknown reason", 0, 0, 0 );
       break;

    } /* switch ( reason ) */
  }

} /* mc_cdma_multimode_sanity_err_fatal */

/*===========================================================================

FUNCTION MC_AMPS_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
  The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mc_amps_multimode_sanity_err_fatal
(
  mmoc_sanity_err_e_type reason
)
{
  switch ( reason )
  {
    case MMOC_SANITY_ERR_DEACT_CMD_CNF:
      /* The AMPS protocol did not process/respond  to the MC_AMPS_PROT_DEACT_CMD_F
      ** The AMPS protocol should respond within 15 seconds, in case of
      ** deactivating because of power down it is 30 seconds.
      */
      ERR_FATAL( "AMPS did not respond to deactivate command",
                 0, 0, 0
                );
      break;

    case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
      /* The AMPS protocol did not process/respond to the MC_AMPS_PROT_PH_STAT_CHGD_CMD_F
      ** The AMPS protocol should respond within 15 seconds.
      */
      ERR_FATAL( "AMPS did not respond to phone status chgd command",
                 0, 0, 0
                );
      break;

    case MMOC_SANITY_ERR_GENERIC_CMD_CNF:
      /* The AMPS protocol did not process/respond to the MC_AMPS_PROT_GEN_CMD_F( because
      ** of pref_sys changed or SD reselection timeout.
      ** The AMPS protocol should respond within 15 seconds.
      */
      ERR_FATAL( "AMPS did not respond to generic command",
                 0, 0, 0
                );
      break;

    default:
     ERR_FATAL( "Unknown reason", 0, 0, 0 );
     break;

  } /* switch ( reason ) */

} /* mc_amps_multimode_sanity_err_fatal */

/*===========================================================================

FUNCTION MC_EFS_INIT

DESCRIPTION
      Performs initialization required for NV item files in the EFS.
      Currently this means generating the config file for diag tools.

DEPENDENCIES
      None

RETURN VALUE
      None

SIDE EFFECTS
      None

===========================================================================*/

void mc_efs_init
(
  void
)
{
  onex_nv_efs_init(MC_EFS_CONF_FILE, (int) MC_OWNED_EFS_ITEM_FILE_MAX, 
                   mc_efs_item_file);
} /* mc_efs_init */
/*===========================================================================

FUNCTION MC_EFS_PUT
  This function creates an EFS item file or stores data to an existing
  EFS item file.

  If the item file does not exist, then this function will create a new
  item file and if it exists, then it will truncate the existing file
  (O_TRUNC does that) and overwrite the data to the existing file.
  So, if a file is required to be updated partially, then mc_efs_get()
  should be called first to buffer the data and then the required fields
  should be updated and whole file written again using mc_efs_put().

DEPENDENCIES
  Depends on all input fields being filled out properly.

RETURN VALUE
  Returns the error code efs_errno. EFS module sets the error code in
  efs_errno which points to the element err_num of calling task's TCB.
  All the error codes are defined in fs_errno.h.

SIDE EFFECTS
  If the return value of efs_put() indicates an error (-1) then
  ERR_FATAL is called.

  efs_put() is supposed to work efficiently to store small amount of data
  (FS_ITEM_MAX). This function will determine the type
  of file needed based on the length parameter, If the length is <=
  (FS_ITEM_MAX) bytes then this function will try to store this file
  efficiently on an item-file, but if the length exceeds (FS_ITEM_MAX) bytes
  then this function will store this data on a regular file and the access
  will be slower.

===========================================================================*/

int mc_efs_put
(
  mc_efs_item_file_name_type efs_file,
    /* EFS item file name */

  mc_efs_file_data_type* data
    /* Pointer to variable in which data to be written is available */
)
{
  int oflag = O_AUTODIR | O_RDWR | O_CREAT | O_TRUNC;
    /* special flags.
       O_AUTODIR : Auto create parent directories
       O_CREAT : Create the item if it does not exist
       O_RDWR : To specify that file will be used for reading and writing
       O_TRUNC : Truncate the file if it exists */

  int mode = 0777;
    /* initial permissions */

  int ret_val;
    /* return value for efs_put */

  int efs_error_code;
    /* error code of efs operation */

  uint32 length = mc_efs_item_file[efs_file].size;
    /* number of bytes to be stored */

  M1X_MSG( DCP, LEGACY_MED,
    "storing efs item file %d",
    (int)efs_file);

  if (efs_file >= MC_EFS_ITEM_FILE_MAX)
  {
    ERR_FATAL("Invalid file (%d) to write",(int)efs_file, 0, 0);
  }

  /* It runs in MC task context */
  ret_val = efs_put(
                     mc_efs_item_file[efs_file].path,
                     data,
                     length,
                     oflag,
                     mode
                   );

  efs_error_code = efs_errno;

  if (ret_val == -1)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "efs_errno %d while writing EFS file %d",
      efs_error_code,
      (int)efs_file);
  }

  if ( (ret_val != -1) && (length != (uint32)ret_val) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Num of bytes sent for storage %d, actual stored %d",
      length,
      ret_val);
  }

  return (efs_error_code);

} /* mc_efs_put */

/*===========================================================================

FUNCTION MC_EFS_GET
  This function retrieves the value of an EFS item file.

DEPENDENCIES
  Depends on all input fields being filled out properly.

RETURN VALUE
  Returns the error code efs_errno. EFS module sets the error code in
  efs_errno which points to the element err_num of calling task's TCB.
  All the error codes are defined in fs_errno.h.

SIDE EFFECTS
  If the return value of efs_get() indicates an error (-1) and error
  does not indicate that file does not exist, then ERR_FATAL is called.

===========================================================================*/

int mc_efs_get
(
  mc_efs_item_file_name_type efs_file,
    /* EFS item file name */

  mc_efs_file_data_type* data
    /* Pointer to variable in which data will be retrieved */
)
{
  int ret_val;
    /* return value for efs_put */

  int efs_error_code;
    /* error code of efs operation */

  uint32 length = mc_efs_item_file[efs_file].size;
    /* number of bytes to be retrieved */

  if (efs_file >= MC_EFS_ITEM_FILE_MAX)
  {
    ERR_FATAL("Invalid file (%d) to write",(int)efs_file, 0, 0);
  }
  /* It runs in MC task context */
  ret_val = efs_get(
                     mc_efs_item_file[efs_file].path,
                     data,
                     length
                   );

  efs_error_code = efs_errno;

  if (ret_val == -1)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "Error reading EFS file %d, using default value",
      (int)efs_file);

    /* Populate default values into data */
    switch(efs_file)
    {
      case MC_EFS_SO73_COP0_SUPPORTED:
        data->so73_cop0_supported = 0;
        break;

      case MC_EFS_REG_AFTER_DDTM_PAGE_DROP_SUPPORTED:
        data->reg_after_ddtm_page_drop_supported = 0;
        break;

      case MC_EFS_1XADVANCED_CAPABILITY_STATUS:
        data->onexadv_capability_status = MC_1XADVANCED_ENABLED;
        break;

      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      case MC_EFS_1xCSFB_T_MAX_NO_C2K_PARAM:
        data->onexcsfb_c2k_param_timeout = 5000;
        /* The default timeout for C2K param receipt is 5 sec */
        break;

      case MC_EFS_1xCSFB_T_MAX_NO_VOICE:
        data->onexcsfb_no_voice_timeout = MCC_CSFB_T_MAX_NO_VOICE;
        /* The default timeout for C2K param receipt is 5 sec */
        break;

      case MC_EFS_1XCSFB_N_MAX_REG_RETRY_CNT:
        data->onexcsfb_max_reg_retry_cnt = MCC_CSFB_MAX_REG_ATTEMPTS;
        /* The default timeout for C2K param receipt is 5 sec */
        break;
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

      case MC_EFS_JCDMA_SUPPORT:
        data->jcdma_support.service = MC_JCDMA_SERVICE_DISABLED;
        break;

      case MC_EFS_DISABLE_SO35_SO36:
        data->disable_so35_so36 = 0;   /* By default SO35 and SO36 is supported */
        break;

      case MC_EFS_OTASP_MAX_UNLOCK_ATTEMPTS:
        /* By default otasp_max_unlock_attempts = 6 (OTASP_MAX_UNLOCK_ATTEMPTS)*/
        data->otasp_max_unlock_attempts = OTASP_MAX_UNLOCK_ATTEMPTS;
        break;

      case MC_EFS_DISABLE_1X_TO_LTE_RESELECTION:
        data->disable_1x_to_lte_reselection = 0; /* By default 1X to LTE reselection is enabled */
        break;

      case MC_EFS_CMAS_DUP_DET_SUPPORTED:
        data->cmas_dup_det_supported = 0;
        break;

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
      case MC_EFS_DEREGISTER_TIMER_VAL:
        /* PPP_CLEANUP_NON_SV_MODE: Default deregister timer value */
        data->deregister_timer_val = DEREGISTER_TIMER_VAL;
        break;
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

      case MC_EFS_REJ_VOICE_PAGE_FOR_VOLTE:
        /* By default don't reject voice page for while volte call is active */
        data->rej_voice_page_for_volte = 0;
        break;

      case MC_EFS_SPECIFIC_CALL_END_STATS_SUPPORTED:
        /* By default the specific call end reasons are disabled */
        data->specific_call_end_stats_supported = 0;  
        break;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      case MC_EFS_1xCSFB_CGI_FEATURE_SUPPORTED:
        /* By default 1xCSFB CGI(Cell Global Identity) feature is enabled */
        data->onexcsfb_cgi_feature_supported = 1;  
        break;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
      case MC_EFS_1xCSFB_REDIR_CALL_SETUP_OPTIMISED:
        /* By default the call setup time for redirection based 
        ** CSFB calls is optimised */
        data->csfb_call_setup_optimised = 1;
        break;
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

#ifdef FEATURE_MODEM_1X_SRVCC
      case MC_EFS_1XSRVCC_T_MAX_NO_UHDM:
        /* Default Timeout for SRVCC UHDM to be rcvd is 4-sec */
        data->onexcsfb_c2k_param_timeout = 4000;  
        break;
#endif /* FEATURE_MODEM_1X_SRVCC */

      case MC_EFS_USE_DEF_VAL_FOR_PWRUP_NV_ERR:
        /* By default using default NV values 
         * if NV Read/Write error happens during Powerup load is supported */
        data->use_def_val_for_pwrup_nv_err = 1;  
        break;
        
      case MC_EFS_MMGSDI_FEATURES_STATUS_LIST:
        data->mc_mmgsdi_features_status_list.uim_ruim_support_sci = MMGSDI_FEATURE_ENABLED; /* By Default FEATURE_UIM_RUIM_SUPPORT_SCI is ENABLED */
        break;

#ifdef FEATURE_1X_CUST_VOICE_SO
      case MC_EFS_CUST_EVRC_VOICE_ENCRYPT_SO:
        /* By default set Customer SO as SO_NULL. This is equivalent 
         * to the case where there is no customer SO */
        data->cust_voice_encrypt_so = CAI_SO_NULL;
        break;
#endif /* FEATURE_1X_CUST_VOICE_SO */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS ) || \
    defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 )
      case MC_EFS_SMS_OVER_S102:
        /* By default sms over s102 is not supported */
        data->mcc_csfb_sms_suported_over_s102 = 0;
        break;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS ||
          FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */

#ifdef FEATURE_1X_SPLMN_SUPPORT
      case MC_C2K_RESEL_SPLMN_SUPPORT:
        /* By default the feature no BSR to reselectable SPLMNs is enabled */
        data->c2k_resel_splmn_supp = 1;  
        break;
#endif /* FEATURE_1X_SPLMN_SUPPORT */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
      case MC_EFS_SPLIT_OVERHEAD_SUPPORTED:
        /* By default the Split Overheads across multiple wakeups feature
        ** is enabled */
        data->split_ovhd_supported = 1;
        break;
#endif /* FEATURE_1X_SPLIT_OVERHEAD */

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
      case MC_EFS_1X_OVHD_OPTIMIZATION:
        /* By default the following overhead optimizations will be enabled
         * 1. Store the overheads across state/mode transitions such as
         *    Traffic -> Init -> Idle/System Lost transitions
         * 2. Validate ovhds and use them if they are still valid instead
         *    of just clearing them after 10 mins timer expiry. */
        data->ovhd_optimization_sup = 3;
        break;
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

      #ifdef FEATURE_SIMUL_1X_LTE
      case MC_EFS_1xSRLTE_MT_PAGE_ACC_HO_SUPPORTED:
        /* By default 1xSRLTE MT page access handoff feature
         * is enabled */
        data->srlte_mt_page_acc_ho_supported = 1;
        break;
      #endif /* FEATURE_SIMUL_1X_LTE */
  
      default:
        break;
    }
  }

  /* Unlikely scenario */
  if( (ret_val != -1) && (length != (uint32)ret_val) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Num of bytes expected %d, actual retrieved %d",
      length,
      ret_val);
  }

  return (efs_error_code);

} /* mc_efs_get */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================

FUNCTION MC_MSGR_INIT

DESCRIPTION
  This function initializes MSGR.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mc_msgr_init(void)
{
  /* Initialize the Queues */
  mc_init_msgr_queue();
  /* Create and register client */
  mc_msgr_setup_client();
  /* Register the messages that 1X CP is interested in collecting. Basically
     these will be the messages that are part of mc_msgr_rcv_union_type
  */
  mc_msgr_register_msgs();
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: MSGR registered");
}

/*===========================================================================

FUNCTION       MC_INIT_MSGR_QUEUE
DESCRIPTION
  This function initializes the receive queue
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
static void mc_init_msgr_queue( void )
{
   /* Initialize the Command Queue */
  (void) q_init( &mc_msgr_rcv.msg_q );
}

/*===========================================================================

FUNCTION       MC_GET_MSGR_CLIENT_ID

DESCRIPTION    This function retuns the msgr client id.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static msgr_client_t*  mc_get_msgr_client_id( void )
{

  static msgr_client_t mc_msgr_client_id;

  /*-----------------------------------------------------------------------*/

  return &mc_msgr_client_id;

} /*  mc_msgr_client_id */

/*===========================================================================

FUNCTION       MC_GET_MSGR_QUEUE_ID

DESCRIPTION    This function retuns the msgr queue id.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static msgr_id_t*  mc_get_msgr_queue_id( void )
{

  static msgr_id_t mc_msgr_queue_id;

  /*-----------------------------------------------------------------------*/

  return &mc_msgr_queue_id;

} /*  get_msgr_queue_id */


/*===========================================================================
FUNCTION       mc_msgr_setup_client
DESCRIPTION
This function setups the 1x client with the MSGR.
DEPENDENCIES
Queues must be initialized.
RETURN VALUE
None.
SIDE EFFECTS
None.
===========================================================================*/
static void mc_msgr_setup_client( void )
{
  errno_enum_type  estatus;
  msgr_client_t* mc_msgr_client_id_ptr = mc_get_msgr_client_id();
  /*-----------------------------------------------------------------------*/
  ASSERT(mc_msgr_client_id_ptr != NULL );
  estatus = msgr_client_create(mc_msgr_client_id_ptr );
  ASSERT( estatus == E_SUCCESS );
  estatus = msgr_client_add_rex_q ( mc_msgr_client_id_ptr,
                                    MC_TCB_PTR,
                                    MC_CMD_Q_SIG,
                                    &mc_msgr_rcv.msg_q,
                                    MSGR_NO_QUEUE,
                                    offsetof( mc_msgr_rcv_msg_type, msg ),
                                    MSGR_NO_OFFSET,
                                    (uint16)sizeof(mc_msgr_rcv_msg_type ),
                                    mc_get_msgr_queue_id() );
  ASSERT( estatus == E_SUCCESS );
  mc_is_msgr_registered = TRUE;
} /* setup_client */


 /*=========================================================================
FUNCTION       MC_MSGR_REGISTER_MSGS
DESCRIPTION
This function registers a message that 1x is interested in  collecting from the message router
DEPENDENCIES
1X client setup with the MSGR must have been completed.
RETURN VALUE
None.
SIDE EFFECTS
None.
===========================================================================*/
static void mc_msgr_register_msgs( void )
{
  errno_enum_type  estatus;
  msgr_client_t*   mc_msgr_client_id_ptr = mc_get_msgr_client_id();

  M1X_MSG( DCP, LEGACY_HIGH,
    "CSFB DEBUG:Registering for msgr messages");

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_REDIR_REQ );

  ASSERT( estatus == E_SUCCESS );
  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_RESEL_REQ );

  ASSERT( estatus == E_SUCCESS );
  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_ABORT_REDIR_REQ );

  ASSERT( estatus == E_SUCCESS );
  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_ABORT_RESEL_REQ );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           LTE_RRC_1X_SIB8_INFO_IND );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_IRAT_DL_MSG_IND );

  ASSERT( estatus == E_SUCCESS );


  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_IRAT_IMSI_ATTACH_IND );

  ASSERT( estatus == E_SUCCESS );


  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_IRAT_CTXT_LOST_IND );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_IRAT_FAILURE_IND );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_IRAT_UL_MSG_CNF );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_1XCSFB_HO_REQ );

  ASSERT( estatus == E_SUCCESS );

 estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_ATTACH_COMPLETE_IND );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                          mc_msgr_client_id_ptr,
                          MSGR_ID_REX, 
                          ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ );

  ASSERT( estatus == E_SUCCESS );

  estatus = msgr_register( MSGR_ONEX_CP,
                          mc_msgr_client_id_ptr,
                          MSGR_ID_REX, 
                          ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP );

  ASSERT( estatus == E_SUCCESS );

  #ifdef FEATURE_1X_TO_LTE
  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           LTE_RRC_1X_ABORT_RESEL_RSP );

  ASSERT( estatus != E_FAILURE );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           LTE_RRC_1X_RESEL_FAILED_RSP );

  ASSERT( estatus != E_FAILURE );

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_DEPRI_FREQ_REQ );

  ASSERT( estatus != E_FAILURE );
  #endif /* FEATURE_1X_TO_LTE */

  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_GET_CGI_REQ );

  ASSERT( estatus != E_FAILURE );
  
  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_ABORT_CGI_REQ );

  ASSERT( estatus != E_FAILURE );

#ifdef FEATURE_MODEM_1X_SRVCC
  estatus = msgr_register( MSGR_ONEX_CP,
                           mc_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           ONEX_CP_LTE_1XSRVCC_HO_REQ );

  ASSERT( estatus != E_FAILURE );
#endif /* FEATURE_MODEM_1X_SRVCC */

  mc_is_msgr_registered = TRUE;

} /* mc_msgr_register_msgs */

/*===========================================================================

FUNCTION       MC_MSGR_SEND_MSG

DESCRIPTION
This function initializes the header and sends the message to
the message router.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

===========================================================================*/
void mc_msgr_send_msg
(
  msgr_hdr_struct_type*     msg_hdr_ptr,
  msgr_umid_type            umid,
  int32                     size
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  ASSERT( msg_hdr_ptr != NULL );

  /* Initialize the message header */
  msgr_init_hdr(msg_hdr_ptr, MSGR_ONEX_CP, umid);

  estatus = msgr_send( msg_hdr_ptr, size );

  ASSERT( estatus == E_SUCCESS );
  M1X_MSG( DCP, LEGACY_HIGH,
    "MC sent msg with umid=%d",
    umid);

}

/*===========================================================================

FUNCTION MC_PROC_IRAT_SWITCH

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean mc_proc_irat_switch(mc_msgr_rcv_msg_type* msgr_ptr)
{
  size_t copy_size; /* Variable to store size of memscpy() */
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: rxed msg = %d",
    (msgr_ptr->msg.hdr.id));

  if( (msgr_ptr->msg.hdr.id != ONEX_CP_LTE_REDIR_REQ) &&
      (msgr_ptr->msg.hdr.id != ONEX_CP_LTE_RESEL_REQ) )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "CSFB DEBUG: Rxed illigal message, returning");
    return FALSE;
  }

  mc_redir_req_msg.hdr.id = msgr_ptr->msg.hdr.id;

  if(msgr_ptr->msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
  {
    /* copy to a local buffer */
    mc_redir_req_msg.redir_info = msgr_ptr->msg.redir_req.redir_info;

    mc_redir_req_msg.pilot_pn_count = msgr_ptr->msg.redir_req.pilot_pn_count ;
    copy_size =  memscpy( mc_redir_req_msg.pilot_pn_info,
                         sizeof(mc_redir_req_msg.pilot_pn_info),
            msgr_ptr->msg.redir_req.pilot_pn_info,
                         sizeof(mc_redir_req_msg.pilot_pn_info)
                       );
    if( copy_size != sizeof(mc_redir_req_msg.pilot_pn_info) )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
        copy_size,
        sizeof(mc_redir_req_msg.pilot_pn_info),
        sizeof(mc_redir_req_msg.pilot_pn_info) );
    }

    /* This check is added to validate the range for the global_cell_id 
     * and num_mnc_digits as receivd in the redirection request from LTE.
     * The maximum value of global_cell_id can be 0x0FFFFFFF and for 
     * num_mnc_digits is 3 */
    if ( (msgr_ptr->msg.redir_req.global_cell_id > 0x0FFFFFFF) ||
         (msgr_ptr->msg.redir_req.redir_plmn.num_mnc_digits > 3) )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "CSFB DEBUG: Rxed illigal redirection message, returning");
      return FALSE;
    }
    mc_redir_req_msg.global_cell_id = msgr_ptr->msg.redir_req.global_cell_id;
    mc_redir_req_msg.redir_plmn = msgr_ptr->msg.redir_req.redir_plmn;
  }
  else
  {
    mc_resel_req_msg.channel = msgr_ptr->msg.resel_req.channel;

    mc_resel_req_msg.pilot_pn_offset = msgr_ptr->msg.resel_req.pilot_pn_offset;
    mc_resel_req_msg.lte_ostmr       = msgr_ptr->msg.resel_req.lte_ostmr;
    mc_resel_req_msg.onex_sys_time   = msgr_ptr->msg.resel_req.onex_sys_time;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION MCC_CSFB_SEND_IRAT_FAIL_IND

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_irat_fail_ind(prot_deact_e_type deact_reason)
{
  srch_csfb_handover_done(FALSE);

  /* if deact_reason == PROT_DEACT_REDIR_ABORT, it represents ABORT cases */
  /* if deact_reason != PROT_DEACT_REDIR_ABORT , it represents RESEL/REDIR fail cases */

  if( cdma_entry.entry == MCC_LTE_1X_REDIR )
  {
    if ( (deact_reason != PROT_DEACT_REDIR_ABORT) )
    {
      mc_redir_failed_ind.cause = ONEX_CP_FAIL_REDIR;
      mc_msgr_send_msg( &mc_redir_failed_ind.hdr,
                        ONEX_CP_LTE_REDIR_FAILED_RSP,
                        sizeof(mc_redir_failed_ind) );
      mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_SEND_IRAT_FAIL_IND, 
                            MC_ONEX_CP_LTE_REDIR_FAILED_RSP, NULL );
                        
      #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
          defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
       "QSH:ONEX_CP_LTE_REDIR_FAILED_RSP, cause:ONEX_CP_FAIL_REDIR");
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL);
      #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                        
    }
    else
    {
      mc_msgr_send_msg( &mc_redir_abort_ind.hdr,
                        ONEX_CP_LTE_ABORT_REDIR_RSP,
                        sizeof(mc_redir_abort_ind) );
      mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_SEND_IRAT_FAIL_IND, 
                            MC_ONEX_CP_LTE_ABORT_REDIR_RSP, NULL );
                        
      #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
          defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
       "QSH:ONEX_CP_LTE_ABORT_REDIR_RSP");
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL);
      #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                        
    }
  } /* MCC_LTE_1X_REDIR */
  else if( cdma_entry.entry == MCC_LTE_1X_RESEL )
  {
    if ( (deact_reason != PROT_DEACT_REDIR_ABORT) )
    {
      mc_resel_failed_ind.cause = ONEX_CP_FAIL_RESEL ;
      mc_msgr_send_msg( &mc_resel_failed_ind.hdr,
                        ONEX_CP_LTE_RESEL_FAILED_RSP,
                        sizeof(mc_resel_failed_ind) );
      mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_SEND_IRAT_FAIL_IND, 
                            MC_ONEX_CP_LTE_RESEL_FAILED_RSP, NULL );
                        
      #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
          defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
       "QSH:ONEX_CP_LTE_RESEL_FAILED_RSP, cause:ONEX_CP_FAIL_RESEL");
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL);
      #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                        
    }
    else
    {
      mc_msgr_send_msg( &mc_resel_abort_ind.hdr,
                        ONEX_CP_LTE_ABORT_RESEL_RSP,
                        sizeof(mc_resel_abort_ind) );
      mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_SEND_IRAT_FAIL_IND, 
                            MC_ONEX_CP_LTE_ABORT_RESEL_RSP, NULL );
                        
      #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
          defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
       "QSH:ONEX_CP_LTE_ABORT_RESEL_RSP");
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL);
      #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                        
    }
  } /* MCC_LTE_1X_RESEL */
} /* void mcc_csfb_send_irat_fail_ind(prot_deact_e_type deact_reason) */

/*===========================================================================

FUNCTION MC_CDMA_ACTIVATE_PROTOCOL_CNF

DESCRIPTION
  This function posts an activation confirmation to the MC task.

DEPENDENCIES
  None.

PARAMETERS
  act_reason - Reason for posting redirection confirmation to MC

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_cdma_activate_protocol_cnf
(
  prot_act_e_type      act_reason
)
{
  mc_msg_type  *cmd_ptr;
    /* MC command pointer. */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd  = MC_PROT_ACT_CNF_CMD_F;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);


} /* mc_cdma_activate_protocol_cnf() */

/*===========================================================================

FUNCTION MC_CDMA_REDIR_PROTOCOL_CNF

DESCRIPTION
  This function sends an redirection confirmation to the MC task.

DEPENDENCIES
  None.

PARAMETERS
  act_reason - Reason for sending redirection confirmation to MC
  redir_cnf_status - If Redirection can be continued or aborted

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_cdma_redir_protocol_cnf
(
  prot_act_e_type      act_reason,

  boolean              redir_cnf_status
    /* TRUE - If Redir conf, FALSE - If Redir Aborted */
)
{
  mc_msg_type  *cmd_ptr;
    /* MC command pointer. */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd  = MC_PROT_REDIR_CNF_CMD_F;
  cmd_ptr->redir_cnf.redir_cnf_status = redir_cnf_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

} /* mc_cdma_redir_protocol_cnf() */

/*===========================================================================

FUNCTION MC_CDMA_HANDOVER_CNF_F

DESCRIPTION
  This function sends an handoff confirmation to the MC task.
  It confirms that CP now is the active stack.
  This is a blocking call.

DEPENDENCIES
  None.

PARAMETERS
  act_reason - Reason for sending handover confirmation to MC.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_cdma_handover_cnf
(
  prot_act_e_type  act_reason
  /* Reason for sending  handover confirmation to CDMA protocol
  */
)
{
 mc_msg_type  *cmd_ptr;
  /* MC command pointer. */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd  = MC_LTE_CDMA_HANDOVER_CNF_F;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

} /* void mc_cdma_handover_cnf */

/*===========================================================================

FUNCTION MC_CDMA_ABORT_REDIR

DESCRIPTION
  This API is used by MMOC, for sending Abort REDIR cmd to MC task.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_cdma_abort_redir( void )
{
 mc_msg_type  *cmd_ptr;
  /* MC command pointer. */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd  = MC_PROT_ABORT_REDIR_CMD_F;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);
} /* mc_cdma_abort_redir() */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSIM
/*===========================================================================

FUNCTION MC_POST_MMGSDI_CLIENT_ID_REG_CNF

DESCRIPTION
  This function sends an confimation of client id registration with MMGSDI.
  This function is called from MMGSDI's context.

DEPENDENCIES
  Call for Client id registration should have been made prior.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_post_mmgsdi_client_id_reg_cnf(void)
{
  mc_msg_type  *cmd_ptr;
    /* MC command pointer. */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  REX_ISR_LOCK(&mc_crit_sect);

  /* Get the command buffer. */
  cmd_ptr = mc_get_buf_else_err_fatal();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields. */
  cmd_ptr->hdr.cmd  = MC_MMGSDI_CLIENT_ID_REG_CNF_F;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command. */
  mc_cmd( cmd_ptr );

  REX_ISR_UNLOCK(&mc_crit_sect);

} /* mc_post_mmgsdi_client_id_reg_cnf() */

#endif /* FEATURE_CSIM */

/*===========================================================================

FUNCTION MC_ALLOC_QUEUE_BUF

DESCRIPTION
  This function allocates the memory of size based on type of queue. Also keeps
  track of the number of buffers allocated for each queue at any instant.

DEPENDENCIES
  None

RETURN VALUE
  Returns the pointer to allocated memory on success otherwise returns NULL

SIDE EFFECTS
  Used for MC related queues

===========================================================================*/
void* mc_alloc_queue_buf
(
  mc_queue_type queue_type
  /* Type of the queue buf to be allocated */
)
{
  void *ptr = NULL;
  uint32 alloc_size = 0;
#ifdef FEATURE_1X_CP_MEM_OPT
  uint32 max_queue_size = 0;
#endif /* FEATURE_1X_CP_MEM_OPT */

  switch ( queue_type )
  {
    case MC_CMD_BUF_Q:
      alloc_size = sizeof(mc_msg_type);
      break;/* MC_CMD_BUF_Q */

#ifdef FEATURE_1X_CP_MEM_OPT
    case MC_UIM_Q:
      alloc_size = sizeof(uim_rpt_type);
      max_queue_size = MC_NUM_UIM_RPT_BUFS;
      break; /* MC_UIM_Q */

    case MC_MMGSDI_Q:
      alloc_size = sizeof(mc_mmgsdi_rpt_type);
      max_queue_size = MC_NUM_MMGSDI_RPT_BUFS;
      break; /* MC_MMGSDI_Q */

    case MC_DH_RPT_Q:
      alloc_size = sizeof(dh_rpt_type);
      max_queue_size = MC_NUM_DH_RPT_BUFS;
      break; /* MC_DH_RPT_Q */

    case MC_MCC_RXTX_Q:
      alloc_size = sizeof(mccrxtx_cmd_type);
      max_queue_size = MCCRXTX_NUM_BUFS;
      break;/* MC_MCC_RXTX_Q */

    case MCC_SRCH_Q:
      alloc_size = sizeof(mccsrch_rpt_type);
      max_queue_size = MCCSRCH_NUM_RPTS;
      break; /* MCC_SRCH_Q */

    case MCC_DTMF_Q:
      alloc_size = sizeof(cdma_dtmf_type);
      max_queue_size = MCC_MAX_DTMFS_OUT;
      break; /* MCC_DTMF_Q */

    case MC_TRANS_RPT_Q:
      alloc_size = sizeof(mctrans_rpt_type);
      max_queue_size = MCTRANS_NUM_RPT_BUFS;
      break;/* MC_TRANS_RPT_Q */

    case MC_TRANS_DATA_Q:
      alloc_size = sizeof(mctrans_trans_info_type);
      max_queue_size = MCTRANS_MAX_TRANSACTIONS;
      break;/* MC_TRANS_DATA_Q */

    case MC_DH_CMD_Q:
      alloc_size = sizeof(dh_cmd_type);
      max_queue_size = MC_DH_NUM_CMD_BUFS;
      break; /* MC_DH_CMD_Q */
#endif /* FEATURE_1X_CP_MEM_OPT */

    default:
      ERR_FATAL("Can't get here: Bad MC queue type %d", queue_type, 0, 0 );
      break;
  }

  /* Enter Critical Section */
  MC_ENTER_CRIT_SECT();
#ifdef FEATURE_1X_CP_MEM_OPT
  /* mc_set_queue_max_limit is by default TRUE indicates restriction on
   *  queue size. If set to FALSE, there is no restriction on queue size.
   */
  if( (queue_type != MC_CMD_BUF_Q) &&
      (mc_set_queue_max_limit) &&
      (mc_queue_info[queue_type].allocated_buf_cnt == max_queue_size)
    )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "queue %d reached max limit. num of buffers %d",
      queue_type,
      mc_queue_info[queue_type].allocated_buf_cnt);
  }
  else
  {
#endif /* FEATURE_1X_CP_MEM_OPT */
    if( (ptr = modem_mem_alloc( alloc_size, MODEM_MEM_CLIENT_1X )) != NULL )
    {
      memset( ptr, 0, alloc_size);
      mc_queue_info[queue_type].allocated_buf_cnt++;
    M1X_MSG( DCP, LEGACY_MED,
      "[MC Mem opt] Allocated 0x%x for queue %d",
      ptr,
      queue_type);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for MC queue type %d",
        queue_type);
    }
#ifdef FEATURE_1X_CP_MEM_OPT
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  /* Leave Critical Section */
  MC_LEAVE_CRIT_SECT();

  return ptr;
}/* mc_alloc_queue_buf() */

/*===========================================================================

FUNCTION MC_GET_QUEUE_ALLOC_COUNT

DESCRIPTION
  This function used to get the count of the allocated bufs so far this
  particular queue type.

DEPENDENCIES
  None

RETURN VALUE
  Returns the count of the allocated bufs so far

SIDE EFFECTS
  Used for MC related queues

===========================================================================*/
word mc_get_queue_alloc_count
(
  mc_queue_type queue_type
  /* Type of the queue buf to be allocated */
)
{
  if( queue_type < MC_MAX_Q )
  {
    return mc_queue_info[queue_type].allocated_buf_cnt;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Wrong queue type:%d returning allocated_buf_cnt=0",
      queue_type);
    return 0;
  }
} /* mc_get_queue_alloc_count() */

/*===========================================================================

FUNCTION MC_FREE_QUEUE_BUF

DESCRIPTION
  This function Frees the memory allocated for MC queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Used for MC related queues

===========================================================================*/
void mc_free_queue_buf
(
  void *ptr,
  /* Pointer to be freed */
  mc_queue_type queue_type
  /* type of the queue buf to be freed */
)
{
  if (ptr != NULL)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "[MC Mem opt] Free 0x%x from queue %d",
      ptr,
      queue_type);
    /* Enter Critical Section */
    MC_ENTER_CRIT_SECT();
    if (mc_queue_info[queue_type].allocated_buf_cnt > 0)
    {
      modem_mem_free( ptr, MODEM_MEM_CLIENT_1X );
      mc_queue_info[queue_type].allocated_buf_cnt--;
      M1X_MSG( DCP, LEGACY_MED,
        "[MC Mem opt] freed memory");
    }
    else
    {
      ERR_FATAL("ptr: 0x%x. The queue %d is already empty", ptr, queue_type, 0);
    }
    /* Leave Critical Section */
    MC_LEAVE_CRIT_SECT();
  }
  else
  {
    ERR_FATAL("Pointer to free is NULL", 0, 0, 0);
  }
}/* mc_free_queue_buf */

#ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
/*===========================================================================

FUNCTION MCC_FREE_BS_INFO_ARRAY

DESCRIPTION
  This API will de-allocate bs_info[] array heap memory
  once the bs_info[] heap memory cleanup timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_free_bs_info_array( void )
{
  M1X_MSG( DCP, LEGACY_HIGH,
           "Free bs_info[] heap memory, curr_state:%d", 
           cdma.curr_state );
  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_crit_sect);
  /* Information about acquired base stations */
  if( (cdma.curr_state == CDMA_ENTER) ||
      (cdma.curr_state == CDMA_EXIT)
    )
  {
    if( bs_info != NULL )
    {
      modem_mem_free( bs_info, MODEM_MEM_CLIENT_1X );
      bs_info = NULL;

      /* Pointer to information for currently acquired base station */
      cur_bs_ptr = NULL;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
           "bs_info[] is NULL" );
    }
  } /* (curr_state == CDMA_ENTER ||CDMA_EXIT) */
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_crit_sect);
} /* mcc_free_bs_info_array() */
#endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

/*===========================================================================

FUNCTION MC_SEC_RAN_NEXT

DESCRIPTION
  This function generates secure random numbers.

DEPENDENCIES
  None.

RETURN VALUE
  32 bit random number

SIDE EFFECTS
  None.

===========================================================================*/

uint32 mc_sec_ran_next(void)
{
  /* Variable to store more secure random number for access */
  uint32 sec_ran = 0;

  /* Status of secure random number generation */
  secerrno_enum_type sec_ran_status;

  sec_ran_status = secapi_get_random ( SECAPI_SECURE_RANDOM,
                                       (uint8 *)&sec_ran,
                                       sizeof( sec_ran ) );

  if( sec_ran_status != E_SUCCESS )
  {
    ERR_FATAL("Secure API failed due to %d",sec_ran_status,0,0);
  }

  return( sec_ran );

} /* mc_sec_ran_next */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

  FUNCTION MC_PROCESS_NV_REFRESH
  
  DESCRIPTION 
    This API is used to reload all the carrier-specific NV items which are
    read at MC task initialization during normal UE bootup. This API will
    be invoked as part of handling NV refresh indication from Modem 
    Configuration module.
  
  DEPENDENCIES
    None.
    
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
  
===========================================================================*/

void mc_process_nv_refresh ( void )
{
  /* Read NV item NV_CDMA_POWERUP_REG_PERFORMED_I */
  mccreg_read_nv_powerup_reg_performed();
  /* Reload NV items which are read as part of task init during UE bootup */
  mc_init_items_to_nv();

 /* Read the carrier specifc efs item */
 #if defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS ) || \
     defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 )
  mc_efs_get( MC_EFS_SMS_OVER_S102, &mc_efs_data );
  csfb_cdma.mcc_csfb_sms_suported_over_s102 =
	mc_efs_data.mcc_csfb_sms_suported_over_s102;

  M1X_MSG( SRCH, LEGACY_MED,
	"NV refresh: mcc_csfb_sms_suported_over_s102 %d",
	csfb_cdma.mcc_csfb_sms_suported_over_s102 );
 #endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS ||
           FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */

} /* mc_process_nv_refresh */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

FUNCTION MCC_GET_CUST_SO

DESCRIPTION
  This API returns the new voice SO registered by the customer through NV.
  This is a public API and might be invoked from QMI also to get customer SO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mcc_get_cust_so( void )

{
  #ifdef FEATURE_1X_CUST_VOICE_SO
  return cdma.cust_so;
  #else
  return CAI_SO_NULL;  
  #endif /* FEATURE_1X_CUST_VOICE_SO */
}

#if( defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) )
/*===========================================================================
FUNCTION: MC_GET_CSFB_CALL_STATUS

DESCRIPTION: This function returns the status of CSFB call

DEPENDENCIES: None.

RETURN VALUE: Status of CSFB call.

SIDE EFFECTS: None.
===========================================================================*/
csfb_call_status_type mc_get_csfb_call_status(void)
{
  return( csfb_cdma.call_status );
}/* mc_get_csfb_call_status */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
/*lint +e818 */
