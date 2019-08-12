#ifndef MDSP_INTF_G_H
#define MDSP_INTF_G_H
/*===========================================================================
                          M D S P _ I N T F _ G . H


GENERAL DESCRIPTION
  This header file contains macros and function prototypes for general
  utility functionality for the mDSP.
  It is intended for external clients to include.


Copyright (c) 2000-2014 Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmdsp6/inc/mdsp_intf_g.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when         who     what, where, why
----------  -------- ------------------------------------------------------
06/06/16     km      CR1017936 FR 35777: Enable Samsung 'Big Data' feature on 8917 - GERAN GL1
10/12/15   br        CR949281GERAN compilation errors when disabling FEATURE_DUAL_SIM in 9x07 IOE build/PL
18/05/15    jj       CR 839611 New skip last measurement in T2G if command is late 
09/01/15   dg        CR779348 SKip index 0 while retrieving power meas. results if the flag skipfirstburst is set
25/11/14   cah       CR740519: fix to trigger generic GFW comamnd when state changes and rx aplha needs to be updated.
05/11/14    jj       CR 751077 remove gfwTxTablesBuffer   from GL1 side 
28/11/14    am       CR760135 Enable SBD and EPD for page decode in transfer mode
14/11/14    cja      CR754529 when prebuilding scripts, have preload offset as immediate
27/10/14    cja      CR746760 In X2G power measure tie results buffer index to state machine
17/10/14    cja      CR730201 Send rf meas exit after cleanup rather than waiting inline for RF scripts
17/10/14    df       CR726944:Delete unused header file mdsp_dtm_g.h
03/10/14    aga      CR732455 Changes required to tune away framework for TH 1.0 single sim build 
09/09/14    jk       CR697308:GL1 changes to support HLin LLin sawless feature
12/09/14    nm       CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
09/09/14    jk       CR722274:Device ID to be passed in FW enter mode command 
09/09/14    cah      CR718841 - GL1 missing support for RFSW changes for RXFE bus contention
29/08/14    cos      CR 716812 - Adding missing changes for imrd framework
07/08/14    cja      CR643648 Desense for channel adjacent to spur
06/08/14    df       CR536059 Full asynchronous sleep support in idle
18/07/14    pg/pa	 CR694601:GERAN L1 support for dedicated WTR hopping with FED architecture
24/06/14    og       CR638097. L2G DSDA support.
27/03/14    pjr      CR639777 Configure TX chain Id correctly after connected wtr hopping
30/05/14   pg        CR655759 Feature cleanup of FEATURE_BOLT_MODEM and FEATURE_DIME_MODEM
07/04/14    cja      CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
04/04/14    cah     CR643764 VSTMR multi-sim restruct and FEATURE_MSIM_VSTMR removal
26/03/14    ab       CR639078 : COEX : CXM Priority and Freq ID for cm_power_meas
21/03/14    mc       CR588853 : COEX : GL1 changes for Enhanced FCCH algorithm
12/03/14    pa		 CR630228:Bolt: Changes due to obsolete FW API cleanup
27/02/14    og       CR606569. Correction to T2G CXM logging.
11/02/14   ap        CR612459: Deactivation and sync rx occurring simultaneously resulting in GFW crash
23/12/13    cja      CR593115 Add passing RxLM index in preload command to FW
09/12/13    cja      CR586971 Remove function mdsp_issue_enter_mode_cmd
12/11/13    cja      CR575034 Add IRAT preload script for X2G.
19/12/13   jk        CR503359 - GL1 changes to support Rx diversity
20/12/13   sk        CR590776 Ensure TX commands not issued before Tx config issued
08/01/14    us       CR562238 Partial T2G gap usage for RSSI measurement
26/11/13    sai      CR572198: X2W mode cfg cmd issued after startup/cleanup scripts built,move G2W gap further into GSM frame
                     to avoid overrun.
28/11/13    cja      CR521061 Remove unused RF API and mainline FEATURE_GSM_RFA_TASK.
22/11/13    cja      CR455527 Remove featurisation from files in API directory
05/11/13    js       CR555528 : COEX : CXM Priority and Freq ID for Async R
14/10/13    pa	     CR539505:Bolt: Added support for Dynamic Power Scheduling
11/10/13    zf       CR552306: Notify GFW OLS USTMR time.
04/10/13    pjr      CR536133 removed static qualifier from mdsp_process_cxm_logs
17/09/13    pjr      CR538893 Changes to enable CXM logging
25/09/13    dv       CR549679 set mdsp_init_complete to avoid CCS crash 
24/09/13    cja      CR546121 Send generic config for x2g start alpha before first x2g startup.
23/09/13    sk       2.0 syncup from 1.1
13/09/13    jk       CR413653:Set flag spurChanFlag for desense channel
13/09/13     dv      CR544094 CCS crash due to rfBufIndex not being incremented 
03/09/13    cja      CR538140 Add mcpm_tx_on flag for race condition in sending Tx Config
03/09/13    npt      CR524770 - GL1 changes to support RFLM FED 
21/08/13    npt      CR531633 - Remove FEATURE_DIME_MODEM dependencies 
08/08/13    cja      CR519510 Use DPC to call MCPM, send Tx Config only when MCPM ready
07/08/13    cja      CR524892 QTA uses immediate cmd after startup/cleanup, but should not inc current buffer mod3
19/07/13    cja      CR514498 Only increment current_buffer_mod_3 once per frame, plus for T2G async
10/06/13    cja      CR498279 Do not increment current_buffer_mod_3 twice in G2W only frame
02/08/13    pg       CR447578: Enable partial tone detection only during X2G 
24/07/13    jk       CR464982:Changes to support GEA5_4 ciphering
20/06/13    cs       Major Triton TSTS Syncup
21/02/13    dv       CR448609 Handle T2G HO GFW transition from MEAS mode to ACTIVE mode
10/05/13    cs       TSTS Updates
20/04/13    ws       CR 478028 - resolved link errors with enabling QTA
25/03/13    cja      CR466302 Don't increment current_buf_mod3 if already done by immediate command
28/03/13    jk       CR452710: Changes to support continuous IQ logging and GSM XO calibration
20/03/13    pjr      CR465451 modified init of mdsp private and public data structures
20/02/13    cja      CR455032 Changes for build without TD feature
01/02/13    cja      CR448030 Clear gap commands for aborted gap
22/01/13    cja      CR443834 Extend mdsp_awake to indicate FW enable/disable
19/01/13    cja      CR438712 Set FN to match command FN/offset for mdsp immediate cmd
17/01/13    cja      CR439545 If gap aborted set IRAT CCS scripts to idle
04/01/13    cja      CR437603 Remove use of AsyncImmediateCmd, always use AsyncCmd
10/12/12    cja      CR411308 Dime T2G changes (msg router to GFW)
07/12/12    cja      CR429173 Remove Dime compiler warnings
05/12/12    pg       CR427361: GFW/RF shared memory interface cleanup
05/11/12    cja      CR414098 For Dime only need W clks if W neighbours
05/09/12    cja      CR385323 If restart gsmtr also restart MDSP
04/09/12    og       CR395649. T2G Acquisition and BSIC decode enhancements.
04/09/12    og       CR391191. Prevent T2G RSSI 0 results.
15/08/12    cja      CR385484 Add flag for async interrupt in num cmds
03/08/12    og       CR383632. Don't request a tuneback to GSM when the G2L startup request
                     is not sent to LTE L1.
20/09/12    pg       Add semaphore check skipping for RF buffer unused by FW
13/09/12    ws       CR 399455 Fix compile errors when FEATURE_LTE_TO_GSM is disabled
24/07/12    ky       Changes for new TX CCS interface changes
03/07/12    ky       Async message router sleep, app mode, enter mode interface changes
20/06/12    ky       Added support of using new CCS interface and passing new CCS
                     buffer pointers in RF task messages
25/05/12    pg       Add support for RF Task
24/05/12    ky       Fixed compilation issue
19/06/12    dv       CR371669: G2T Crash due to seq_num being re-used
08/06/12    cja      CR363269 Do not clear IRAT SBI/NonSBI buffers
21/05/12    og       CR361191.
09/05/12    jj       CR330315 compile error correction for FEATURE_GSM_TX_PCL
25/04/12    ws       CR 355838 FR 1604 - GERAN changes to cleanup WCDMA- GL1 API
09/03/12    dv       T2G:CR342537 Triple buffer all monitor buffers
13/03/12    ws       CR335057 eSACCH buffer flush and restore after inter-cell handovers
10/02/12    ws       CR 336194 Enable TD-SCDMA IRAT for GERAN
28/10/11    jj       CR308254 Correction to RF interface for the dc spur removal
11/10/11    jj       CR311863 GL1 interface to shared variable  "spiAntTunerDelta " to fw
13/09/11    jj       CR 306047 Antenna tuner interface from rf driver to fw
02/09/11    jj       CR 304819 Add interface to update smps pdm to dynamic tx buffer
31/08/11    jj       CR 304393 Dynamic tx path delay set interface for rf driver
08/09/11    jj       CR302378  Added GL1 changes for RSB along with spectral inversion
22/08/11    cja      CR302378. Add API for RF to set spectral inversion per band
25/08/11    jj       CR 301918 Add interface to flag 'disableGpSync'
12/08/11    cja      CR301096 Pass index to RF driver for startup and cleanup buffers
12/08/11    cja      CR301307 Nikel G2X, pass stop time (for cleanup)
27/06/11    cs       CR289602 Clear pending RX at sleep for DSDS G+G mode
21/06/11    cja      Featurise 32-bit FCCH freq interface to MDSP
08/06/11    cja      Nikel G2W changes (so WFW does startup)
07/06/11    ws       Added mdsp_program_cal_amam_table(gas_id ) for pre-distortion cal
18/05/11    cja      Add txDacGrfcNumber shared GFW/RF variable
16/05/11    cja      Add setting GFW_SLOT_MODE_CTRL_REG_ADDR shared GFW/RF variable
28/04/11    cja      Add Nikel freq correction by GFW
26/04/11    cja      AM-PM GFW interface change for Nikel
26/04/11    ws       Nikel Fixed missing #endif
20/04/11    cja      Nikel GFW interface change
11/04/11    pg       CR282109: Ensure AEQ PH2 switching only occurs at a block boundary
17/03/11    ky       r2,r3 moved under q6 feature
05/04/11    cja      L1 I/F change for Nikel IRAT
08/03/11    cja      Initial IRAT for Nikel
27/01/11    cja      Changes for RxLM and TxLM for NikeL
24/02/11    ky       r2 and r3 in mdsp_linear_tx_gain_buff_type are not required in posiedon target.
                     So i am removing them for posiedon target
14/06/11    cja      CR264690 Acquisition frequency capture range larger
02/03/11    pg       Resize SBI and non-SBI buffers for Pegasus target
09/12/10    cs       CR264682 Add support for asynch dsp completion interrupt
08/11/10    nf       9K suspend resume
03/11/10    ky       Fix the compiler warnings for 9k
20/09/10    njt      DSDS CR254552 - clean out stale pending nonSBI upon CM init
12/08/10    sjw      New interface to allow reset of GRFC shadow registers
25/10/10    ky       259645: Installed gstmr handler for sending pending commands is not working.
                     Hardcoded the sending these commands in gstmr isr function.
18/10/10    ky       259645: Installed gstmr handler for sending pending commands. This handler
                     sends the generic config command and app mode command to fw if they are
                     pending
14/10/10    ws       FEATURE_GSM_MDSP_AEQ_8PSK - support AEQ Ph2
11/10/10    og       L2G IRAT updates.
07/10/10    ky       254466:Removed fw unused shared variables from the sw code
03/09/10    ws       Added FEATURE_GSM_MDSP_AEQ_SCH for QDSP6 targets
20/04/10    cs       Add new MDSP_CM_PWR_MSR_PROC_DELAY setting
06/04/10    ip       CR233619 - Added define MDSP_CM_SIXTY_QS for POSEIDON, SSRV offline clk is TCXO
19/07/10    ky       Klocwork fixes
14/07/10    ws       Fixed 9k lint warnings
30/07/10   cja       CR248652 Trek - only set MDSP register if MDSP is enabled and awake
02/07/10    ws       Support for IQ logging on QDSP6
21/07/10   cja       Increase max non-sbi to 10, required for extra GRFC control
17/06/10    pm       Added again GSM TX PCL IND feature for TREK
16/06/10    pm       Revert back GSM TX PCL IND feature for TREK for delivering to Pos 2.0 WM7
14/06/10  cja/pm     Integrate support for increased SBI buffers during G2W (to 40)
08/06/10  cja/js     Add GSM TX PCL IND feature for TREK
28/05/10    ky       Multi step per frame TX CAL and pre distortion cal changes
11/05/10   cja       Optimise shared memory buffers to allow 38 SBI transactions in compressed mode.
16/04/10    ky       Added first tx cal cmd flag to rf drivers interface,firmware and correctly
                     populating the grfc and sbi buffers for each step frame
06/04/10   cja       Seperate function for feature check
10-03-30   tjw       Explicitly include dependent headers to make SU API files
                     stand alone
16/03/10    og       Further g2x and x2g updates.
12/03/10    nf       SBD:Added burst pattern interface
10/03/10    ws       Further featurisation of G2X and X2G
01/03/10    og       g2x and x2g updates.
25/02/10    og       Added cm fw enable/disable and set app mode.
23/02/10    og       Added x2g startup and cleanup commands.
22/02/10    og       Increase the maximum number of compressed mode power
                     monitors to 10 per frame.
04/02/10    ws       Removed leagcy Q4 MDSP services and image for Q6 targets
01/02/10    ky       Extracting Ncell SCH Burst metrics should happen from gfwSchRxBurstMetricsBuffer
                     instead of gfwAsyncMetricBuffers for Q6.
15/12/09    KY       Removed FEATURE_GSM_TX_CAL_QDSP6 feature to enable the tx cal command
30/11/09    ws       Added FEATURE_GSM_TX_CAL_QDSP6 to protect previous  checkin
26/11/09    ky       Added the support for TX CAL comamnd
19/10/09    ky       Removed Q6 based compiler warnings
14/10/09    ky       Disabled mdsp_nonsbi_ctrl_unit and mdsp_sbi_ctrl_unit structres for Q6 operation
18/01/10     cs      CR214995 to avoid AMAM overindex problem
03/11/09     ws      Added SBI size increases for G2W and W2G on RTR8600
20/08/09     ws      Added MDSP_SBI_CM_BURST_BUF_SIZ to seperate SBI
                     Buffer sizes between GSM only and compressed modes
09/08/12     ws      Increased MDSP_MAX_SBI_TRANSACTIONS to 38 for POSEIDON for
                     RTR8600 PLL lock workaround
30/03/09     ws      Added FEATURE_GSM_GPRS_QDSP6 and FEATURE_GSM_GPRS_POSEIDON
                     to replace FEATURE_GSM_GPRS_MUSTANG
26/03/09     ws      Added PACKED_POST for QDSP6 targets
12/03/09     og      FEATURE_XO support.
25/01/09     cs      Updates for Mustang iRAT
03/12/08     agv     Added mdsp_start_tx_data_cont for FTM KV Calibration.
24/11/08     og      FEATURE MSC34 changes.
03/11/08     agv     Changed MDSP_MAX_SBI_TRANSACTIONS from 19 to 24 for LCU.
30/10/08     ws      CR 162112 - ESAIC Only reset IIR filter on PCH and NCELL BCCH
21/10/08     agv     Change for Mustang to allow WPLT builds to compile.
19/08/08     og      Mustang bring-up changes.
01/08/08     agv     Changes for LCU G2W Interface Change for HW clock bug.
18/07/08     agv     Changes for LCU so the SSRV is set correctly for CM.
12/05/08     dv      Added FEATURE_GSM_AMR_WB - L1 Component
20/03/08     agv     Change Linear PA flag from 1 to -1.
19/03/08     agv     LCU RF Spur Interface Support.
14/03/08     cs      Fix the NEW_ASYNC pokable reg array for non-W targets
18/01/08     agv     LCU RF Linear PA Interface Support.
05/03/08     ws      Added ESAIC functionality to Async Rx interfaces
28/02/08     agv     Added LCU Sample Server enable function.
14/01/08     agv     New feature for Switched Mode Power Supply which will
                     provide better talk time.
18/12/07     ws      Increased MAX_SBI_TRANSACTIONS from 16 to 19
13/12/07     cs      Add support for 7500-90 in poke regs define
03/12/07     cs      Fix CR 128185 to compile for targets that dont't support
                     ANT1 gating
27/09/07     ed      Added Async Peek/poke command
22/10/07     ws      Add support for ESAIC NV 2508 item control
05/09/07     efytal  Add new function for Phoenix GSM support in FTM mode
19/07/07     og      UOOS changes, reduce number of measured samples.CR119147.
03/11/06     agv     Increased MDSP_MAX_SBI_TRANSACTIONS from 14 to 16 to
                     cope with co-resident GSM PCS bands in some Networks.
13/07/06     agv     Increased MDSP_SBI_RF_ON_NUM_TRANS from 23 to 24 and
                     MDSP_SBI_RF_OFF_NUM_TRANS from 12 to 16; requested by Lam.
17/05/06     ws      ReInstated saic MIMO metrics
11/05/06     ws      Increased MDSP_SBI_RF_ON_NUM_TRANS to 23
04/05/06     agv     Added FTM feature for continuous 8PSK tx needed by Revathi.
10/04/06     agv     Increased MDSP_MAX_SBI_TRANSACTIONS from 11 to 14 for
                     RF performance issue.
06/04/06     agv     Feature SAIC MIMO elements added to the burst metrics.
02/20/06     gfr     Mainline command and apps logging features
10/25/05     gfr     Added mdsp_enable_saic
10/18/05     gfr     Add support for having the mDSP do all ciphering
09/22/05     gfr     Removed FEATURE_SBI_MDSP_CONTROLLER
08/17/05     gfr     Changed byte to uint8 and word to uint16
08/10/05     gfr     Added defines for the AMR tx data control words
08/05/05     gfr     PTCCHD has its own channel type
07/29/05     gfr     DTM support
07/05/05     gfr     Add mdsp_nonsbi_writes_pending
06/01/05     gfr     Add define for narrow ACI filter
05/18/05     gfr     Mainline new SBI format
05/18/05     gfr     Clean up includes
05/10/05     gfr     Mainline FEATURE_GSM_MDSP_EER
05/04/05     yh      Add mdsp warning masks
04/28/05     gfr     Added mdsp_inact and mdsp_cm_inact
04/21/05     yh      Support for panic reset
04/12/05     gfr     Clean up early decoding interface
04/08/05     gfr     Define a macro for parsing the decoded data header.
04/08/05     gfr     Add force_reset flag to download_and_startup
03/22/05     gfr     Increase nonsbi transactions if new SBI format is used
03/11/05     gfr     Reduce number of parameters to mdsp_set_dsp_error_mask.
03/09/05     yh      API change for mdsp_get_decoded_data() and
                     mdsp_get_async_decoded_data().
03/07/05     gfr     Backoff maximum nonsbi transactions to 10 for CM.
03/03/05     gfr     Increase maximum nonsbi transactions per frame to 20.
02/22/05     yh      Add a parameter to mdsp_display_sleep_error.
02/14/05     gfr     Support for new Raven SBI format
02/11/05     gfr     Add data length defines for BCCH and SCH and header
01/28/05     yh      mdsp_download_and_startup takes 2 signals from NPL1 in
                     the parameter
01/27/05     gfr     Support for ACI filter used bits in the burst metrics.
01/25/05     gfr     Use PA transition profile provided by RF driver
01/24/05     gfr     General cleanup and restructure.
01/12/05     gfr     Support for single mDSP ISR targets,
                     mainlined FEATURE_MDSP_USES_MODULES
12/16/04     gfr     Added polar RF transition ramp defines
12/07/04     gfr     Moved mDSP SBI defines here from sbi.h
12/06/04     yhong   Support for FEATURE_GSM_MDSP_EGPRS
12/06/04     gfr     Featurize the mDSP SBI controller
12/08/04     gfr     Featurized polar RF
11/30/04     gfr     Support for polar RF and EER programming
10/05/04     gfr     Support for new 32-bit mdsp error codes.
08/26/04     gfr     Lint cleanup and enhanced memory dumping.
08/18/04     gfr     Removed obselete mdsp_resync prototype.
07/29/04     gfr     Added new metrics field for ACI power.
07/22/04     gfr     EGPRS support.
07/15/04     gfr     Removed SACCH specific code and LOOK_FOR_FACCH channel
                     type hack.
06/25/04     gfr     Add mdsp_process_error to consolidate error processing.
06/24/04     gfr     Add access function mdsp_awake(gas_id ).
06/14/04     gfr     Support for mdsp sleep interface.
06/10/04     gfr     Support for early CBCH decode, and general async rx cleanup.
05/06/04     gfr     Combined afc_phase into the afc_freq field.
04/27/04     jc      Added CM_STARTUP_BURST. Same functionality.
04/20/04     bm      Increased MAX_SBI_TRANSACTIONS to 11 for Rx SBIs
04/02/04     bm      Added two new function prototypes mdsp_gsm_drv_go_to_sleep,
                     mdsp_gsm_drv_wakeup to be called from wcdma sleep
03/05/04     gfr     Lint cleanup and remove uwire code.
02/17/04     gfr     GSM rotators support.
01/30/04     gfr     Added masks for interpreting the ypf field and support
                     for getting scaling factors.
01/16/04     gfr     Support for mdsp results validation.
01/13/04     gfr     Support for up to six gprs monitors per frame
11/05/03     jcw     Exported MDSP_SBI flags
08/21/03     gw      Added prototype for mdsp_reset_async_rx_ptrs(gas_id ).
08/16/03     jc      Removed mdsp_set_sbi_burst_buffer_eot() now done in rf driver.
08/14/03     aks     Remove the inclusion of sbii.h.  Not proper for
                     non-Library modules.
08/06/03     gw      Added prototype for mdsp_abort_early_decode().
                     Replaced MAX_MDSP_GEN_CONFIG_BUF_STRUCT_LENGTH
                     with MDSP_GEN_CONFIG_MAX_BUF_LEN.
08/05/03     gfr     Changed BCCH number to not conflict with PTCCH.
07/23/03     jtn     Moved SBI #defines out of this file into sbii.h.
07/07/03     jc      Prototype for mdsp_sbi_access changed.
07/02/03     jtn     Moved SBI #defines into this file from mdsp_cmd.h so RF code is
                     not dependent on mdsp_cmd.h for those #defines.
06/23/03     bm      Added new structure mdsp_gen_config_buf_struct_array
06/09/03     jtn     Added extern for function mdsp_pa_profile_last_rampon_value().
06/06/03     jc      Overload STOP_ACQ onto MON_BURST_9 instead of MON_BURST_3.
                     Add temp alias for kovuRamVersion.
06/04/03     gfr     Add functions to retrieve the search width.
05/21/03     gw      Added support for early paging decode.
05/12/03     gfr     Added tx_alpha parameter to transmit command.
05/08/03     jc      Changes for RF restructure.
05/02/03     hg      Added GSM AMR support.
04/23/03    thh      Added prototype for the function mdsp_set_tx_alpha().
04/07/03     jc      Add STOP_ACQ_BURST.  Need to move this later.
04/02/03     jc      Add CM_CLEANUP_BURST.
03/04/03     gfr     Included rex.h for rex_tcb_type.
02/26/03     dp      Added mdsp channel designations for half rate
02/05/02     ATM     Made changes for MDSPSVC API mods (6300)
02/04/03     jc/ma   Lint cleanup.
01/13/03     mk      Added support for both mdsp hard and soft rx qual. Removed mdsp
                     taf (determined in L1) and mdsp bfi (calculated in L1).
11/12/02     jc      Added keyword PACKED to struct.
11/03/02     tb      Added support for INTERRAT_GTOW
10/28/02     gw      Added support for back-to-back async receives. Removed
                     include of mdsp_mem.h.
10/14/02     jc      Split MAX_CM_RSSI_MEASUREMENTS from regular GSM ones.
09/23/02     jc      Added new phoneT prototype - mdsp_start_tx_rnd_cont.
09/18/02     jc      Added some prototypes.
09/09/02     gw/ih   Added defintions to support static SBI/nonSBI buffers.
09/06/02     gr      Support for GPRS uplink power control.
08/21/02     gw      Merged in changes for sleep that were on a branch:
                     Increased MDSP_MAX_SBI_TRANS_PER_FRAME from 4 to 10 to
                     support RF initialization in first frame after sleep.
                     Added prototypes for mdsp_go_to_sleep and msdp_wakeup.
06/18/02     tb      Changes to support mdsp services and use of latest parser.
06/06/02     jc      Added support for up to 9 power measures.
06/04/02     bbd     Changed FEATURE_GPRS to FEATURE_GSM_GPRS
05/20/02     jc      Added support for FEATURE_COMPRESSED_MODE
04/11/02     jc      Change comp_factor to int16 in mdsp_set_pa_profile().
04/10/02     thh     Reduced the number of peripheral SBI commands allowed
                     per frame from 6 to 4.  There is a issue where the MDSP
                     is sending extra SBI transaction during a frame which
                     may extend the timeline and cause collisions.
04/03/02     thh     Added support for txprofile and SBI's new API. Only the
                     SBI driver is now allowed to access MDSP's SBI and uwire
                     reads/writes.
03/28/02     dp      Merged in Gabe's changes for CS FACCH (new logical channel types)
03/22/02     gw      Added mdsp_gprs_power_metrics_struct for use in GPRS.
03/14/02    thh      Re-define the PA LUT ramp related structures.
03/14/02     bk      Added timeslots field to mdsp_power_metrics_struct.
03/08/02     gw      Changes to sbi/uwire write interface.
03/04/02     gw      Consolidated mdsp_burst_type enum so that it is the
                     same for GSM and GPRS.
02/20/02     dp      Changed mdsp_get_async_rx_metrics interface to return
                     metrics availability.
02/04/02     mk      Added support for latest afc/tt.
01/15/02     sg      Extended mdsp_burst_type enum for GPRS 4 monitor support
01/15/02     tb
12/08/01     jc      Corrected PTCCHU, PTCCHU_EXT enum to reflect values
                     used in fw version 3-0478.
12/10/01     thh     Merged in Brian RF support.
                     Removed obsolete mdsp_set_lna().
10/30/01     jc      Add support for GPRS. Split file into subfiles.
10/23/01     mk/gw   Mainlined and revised support for multiple power
                     measurements per frame.  Added prototype for function
                     mdsp_get_async_rx_metrics.
10/17/01     jc      Added timetest.h, removed unused phoneT functions, added
                     a couple phoneT cmds, mdsp_nonsbi_write(gas_id ) now uses a uint16
                     as parameter. Added support for mdsp_loopback_cmd. Added
                     support for firmware PLT0441, which has a larger host
                     interface segment. Merged in KOVU support such that this
                     one file should support SINA or KOVU using T_KOVU to
                     indicate a KOVU platform.
09/25/01    mk/gw    Mainlined code under MONITOR_DEBUG. Cleaned up and added
                     support for SyncRx metrics and multiple MON bursts per
                     frame.
09/04/01      JC     Changed prototype for mdsp_switch_command_buffers()
08/20/01    mk/gw    Added initial Ncell support, featurized via
                     MONITOR_DEBUG.
08/01/01      jc     MDSP_DEBUG_SEG_SIZE added.
07/17/01      jc     Added support for nonsbi request buffer now part of
                     host interface segment.
06/13/01     thh     Changes to several PA RAMP #defines and structure
                     definitions to accomodate the changes in mdsp_intf.c.
05/24/01      jc     Added support for PLT0404, ie debug buffer is now
                     separate from host buffer. New debug struct. Size of
                     host segment increased ie SBI,NONSBI buffers increased.
05/14/01      JC     Added prototype for generic_config command.
05/07/01      jc     Add support for mdsp PLT0300. New function prototypes.
04/26/01      jc     Add support for mdsp PLT0202 - TSC added to transmit cmd
04/24/01      jc     Changed logical_channel_type names to add MDSP_ as prefix
04/13/01      JC     Added prototypes for FCCH/SCH results buffers.
03/06/01      jc     New structs added.  Code still under development.
01/05/01      jc     Initial version.
===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rex.h"

#include "gfw_sw_intf.h"
#ifndef FEATURE_GSM_RFLM_FED
#include "fw_rf_irat_intf.h"
#else
#include "rflm_ccs_irat_intf.h"
#endif /*FEATURE_GSM_RFLM_FED*/

#if defined(FEATURE_GSM_GPRS_MUSTANG) && defined (FEATURE_WCDMA_PLT)
#error code not present
#endif

#include "rfgsm_msg.h"

#include "geran_multi_sim.h"


#include "gl1_qsh_ext.h"


/****************************************************************************
                   mDSP   DEFINITIONS  and  ENUMERATIONS
 ***************************************************************************/
#define MDSP_MAX_CURRENT_BUFFER_INDEX  (2)

#define MAX_X2G_RSSI_MEASUREMENTS   MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS
#define MAX_CM_RSSI_MEASUREMENTS    MAX_X2G_RSSI_MEASUREMENTS

#define MDSP_TX_CONFIG_NO_CMD_PENDING MDSP_TXLM_BUF_IDX_INVALID

typedef enum
{
  MDSP_G2W_MODE = 0,
  MDSP_GSM_MODE
} mdsp_mode_type;

typedef enum
{
  MDSP_SSRV_ARM_CTL = 0,
  MDSP_SSRV_MDSP_CTL
} mdsp_ssrv_ctl_type;

typedef enum
{
  MDSP_SSRV_0_SRC = 0,
  MDSP_SSRV_1_SRC
} mdsp_ssrv_src_type;

typedef enum
{
  MDSP_ANT0_IQ_RTC0 = 0,
  MDSP_ANT0_IQ_RTC1,
  MDSP_ANT0_IQ_RTC2,
  MDSP_ANT0_IQ_RTC3
} mdsp_ssrv_cfg_type;

#define MDSP_RESET_SSRV      1
#define MDSP_TEST_RAM_ADDR   0

#define MDSP_CM_TEN_QS          10
#define MDSP_CM_TWENTY_QS       20
#define MDSP_CM_SIXTY_QS        60

/* 9K setting */
#define MDSP_CM_PWR_MSR_PROC_DELAY MDSP_CM_TEN_QS

/* AGV REM */
/* SAIC default values */
#define MDSP_SAIC_SHORT            0
#define MDSP_SAIC_LONG_MIMO_FILT   1
#define MDSP_SAIC_TWO_INPUT_MLSE   0
#define MDSP_SAIC_TS_LENGTH      146
#define MDSP_SAIC_TS_OFFSET      149
#define MDSP_SAIC_RF_GAIN_RANGE    1



/* Logical Channel types */
typedef enum
{
  MDSP_TCH_FS         = 0,
  MDSP_TCH_EFS        = 1,
  MDSP_TCH_F96        = 2,
  MDSP_TCH_F144       = 3,
  MDSP_SACCH          = 4,
  MDSP_SDCCH          = 4,
  MDSP_PCH            = 4,
  MDSP_FACCH          = 5,
  MDSP_SCH            = 6,
  MDSP_RACH           = 7,
  MDSP_PRACH          = 7,
  MDSP_PDTCHU_CS1     = 8,
  MDSP_PDTCHU_CS2     = 9,
  MDSP_PDTCHU_CS3     = 10,
  MDSP_PDTCHU_CS4     = 11,
  MDSP_PDTCHD         = 12,
  MDSP_PACCHU         = 13,
  MDSP_PRACH_EXT      = 14,
  MDSP_PACCHU_EXT     = 15,
  MDSP_PTCCHU         = 16,
  MDSP_PTCCHU_EXT     = 17,
  MDSP_F96_AND_FACCH  = 18,
  MDSP_F144_AND_FACCH = 19,
  MDSP_TCH_HS         = 20,
#ifdef FEATURE_GSM_AMR
  MDSP_TCH_AFS        = 21,
  MDSP_TCH_AHS        = 22,
#ifdef FEATURE_GSM_AMR_WB
  MDSP_TCH_WFS        = 23,
  MDSP_FACCH_WFS      = 24,
#endif
#endif
  /* 25 - 27 are unused */
  MDSP_PTCCHD         = 28,
  MDSP_BCCH           = 29,
  MDSP_FACCH_HS       = 30,
#ifdef FEATURE_GSM_AMR
  MDSP_FACCH_AFS      = 31,
  MDSP_FACCH_AHS      = 32,
#endif
#ifdef FEATURE_GSM_MDSP_EGPRS
  MDSP_PDTCHU_MCS1    = 33,
  MDSP_PDTCHU_MCS2    = 34,
  MDSP_PDTCHU_MCS3    = 35,
  MDSP_PDTCHU_MCS4    = 36,
  MDSP_PDTCHU_MCS5    = 37,
  MDSP_PDTCHU_MCS6    = 38,
  MDSP_PDTCHU_MCS7    = 39,
  MDSP_PDTCHU_MCS8    = 40,
  MDSP_PDTCHU_MCS9    = 41,
#endif
  FORCEWORD_LOGICAL_CHANNEL_TYPE = 0xffff
} mdsp_channel_type;


#ifdef FEATURE_GSM_AMR
typedef enum
{
  MDSP_AMR_4_75       = 0,
  MDSP_AMR_5_15,
  MDSP_AMR_5_9,
  MDSP_AMR_6_7,
  MDSP_AMR_7_4,
  MDSP_AMR_7_95,
  MDSP_AMR_10_2,
  MDSP_AMR_12_2
#ifdef FEATURE_GSM_AMR_WB
  ,MDSP_AMR_WB_0660,  /* 6.60 kbit /s */
  MDSP_AMR_WB_0885,   /* 8.85 kbit /s */
  MDSP_AMR_WB_1265   /* 12.65 kbit/s */
#endif
} mdsp_amr_codec_mode_type;

/* Values for the second word of the AMR encoded packet */
#define MDSP_AMR_VOC_FRAME_TYPE_SPEECH_GOOD 0x0
#define MDSP_AMR_VOC_FRAME_TYPE_SID_FIRST   0x1
#define MDSP_AMR_VOC_FRAME_TYPE_SID_UPDATE  0x2
#define MDSP_AMR_VOC_FRAME_TYPE_RATSCCH     0x4
#define MDSP_AMR_VOC_CODEC_MODE_DONT_CARE   0x0

#endif


#define CM_STARTUP_BURST1_IDX    0
#define CM_CLEANUP_BURST1_IDX    2
#define CM_STARTUP_BURST2_IDX    1
#define CM_CLEANUP_BURST2_IDX    3

/* Loopback types used with mdsp_loopback_cmd */
typedef enum
{
  MDSP_LOOPBACK_NONE,
  MDSP_LOOPBACK_A,
  MDSP_LOOPBACK_B,
  MDSP_LOOPBACK_C
} mdsp_loopback_type;


/* Acquisition AFC values */
typedef enum
{
  MDSP_ACQ_AFC_DISABLED   = 0x00,
  MDSP_ACQ_AFC_ON_PDM     = 0x01,
  MDSP_ACQ_AFC_ON_ROTATOR = 0x03,
  MDSP_ACQ_GSM_XOCAL_AFC_DISABLED = 0x04
} mdsp_acq_afc_type;

/* Add bit 3 to afc mode to indicate XO (set) or VCTCXO (clear)*/
#define XO_ACQ_BIT   0x4


/* mDSP ciphering types */
typedef enum
{
  MDSP_CIPHERING_A5_1 = 0x0,
  MDSP_CIPHERING_A5_2 = 0x1,
  MDSP_CIPHERING_A5_3 = 0x2,
  MDSP_CIPHERING_A5_4 = 0x3
} mdsp_ciphering_type;


#define MDSP_SEMAPHORE_BIT      0x8000

/* Data sizes for GSM in 16 bit words */
#define SCH_DATA_PER_FRAME      3
#define BCCH_DATA_PER_FRAME    12
#define FACCH_DATA_PER_FRAME   12

/* FCCH acquisition results structure */
typedef PACKED struct
{
  uint16        id;                         /* id code                   */
  uint16        offset_h;                   /* offset from start of acq  */
  uint16        offset_l;                   /* offset from start of acq  */
  uint16        coarse_freq;                /* coarse frequency estimate */
  uint16        fine_freq;                  /* fine frequency estimate   */
  uint16        afc_freq_h;                 /* afc frequency estimate    */
  uint16        afc_freq_l;                 /* afc frequency estimate    */
  uint16        snr;                        /* snr  estimate             */
} mdsp_fcch_results_struct;

/* FCCH acquisition results structure for logging backward compatible */
typedef PACKED struct
{
  uint16        id;                         /* id code                   */
  uint16        offset_h;                   /* offset from start of acq  */
  uint16        offset_l;                   /* offset from start of acq  */
  uint16        coarse_freq;                /* coarse frequency estimate */
  uint16        fine_freq;                  /* fine frequency estimate   */
  int16         afc_freq;                   /* afc frequency estimate    */
  uint16        snr;                        /* snr  estimate             */
} mdsp_fcch_log_results_struct;

#define     MAX_SCH_RES  3                  /* number of entries/packet   */
/* SCH acquisition results structure */
typedef PACKED struct
{
  uint16        id;                         /* id code                   */
  uint16        y_crc;                      /* yamamoto, and crc bits    */
  uint16        rxqual;                     /* rx qual metric            */
  uint16        bfi;                        /* bfi status bit            */
  uint16        len;                        /* len of packet             */
  uint16        data[2];                    /* 25 bits in two words      */
  uint16        offset_h;                   /* offset from start of acq  */
  uint16        offset_l;                   /* offset from start of acq  */
  uint16        energy;                     /* peak correlation energy   */
  uint16        coarse_freq;                /* coarse frequency estimate */
} mdsp_sch_results_struct;

/* Mask to read the CRC result */
#define MDSP_SCH_RESULTS_MASK_CRC  0x0002


/* EGPRS firmware returns hard decisions in the burst metrics */
#ifdef FEATURE_GSM_MDSP_EGPRS
#define MDSP_HARD_DECISIONS_GMSK    8
#define MDSP_HARD_DECISIONS_8PSK   22
#define MDSP_MAX_HARD_DECISIONS    MDSP_HARD_DECISIONS_8PSK

/* Count of extra words when EGPRS is defined - async RX does not have these */
#define MDSP_EXTRA_EGPRS_BURST_METRICS  (MDSP_MAX_HARD_DECISIONS + 4)
#endif

/* Structure to receive metrics from mDSP */
typedef PACKED struct
{
  uint16        len;           /* Packet length inclusive of len */
  uint16        seq_num;
  uint16        pdch;          /* Only set for GPRS bursts */
  uint16        rssi_h;
  uint16        rssi_l;
  int16         dc_offset_i;
  int16         dc_offset_q;
  int16         freq_offset;
  int16         timing_offset;
  uint16        snr;
  uint16        c_over_i;      /* Only set for AMR bursts */
#ifdef FEATURE_GSM_MDSP_EGPRS
  uint16        aci_pwr_0Hz;
  uint16        aci_pwr_p200kHz;
  uint16        aci_pwr_n200kHz;
  uint16        egprs_misc;
#endif
/* SAIC MIMO metrics additions. */
  uint16        pre_mimo_trained_complete_snr;
  uint16        post_mimo_trained_complete_snr;
  uint16        misc_flags;
#ifdef FEATURE_GSM_MDSP_EGPRS
  uint16        hard_decisions[MDSP_MAX_HARD_DECISIONS];
#endif
} mdsp_burst_metrics_struct;

#ifdef FEATURE_GSM_MDSP_EGPRS
/* Mask and position of bits in the egprs_misc field */
#define MDSP_BURST_METRICS_EGPRS_MODULATION_POS     0
#define MDSP_BURST_METRICS_EGPRS_MODULATION_MASK  0x1
#define MDSP_BURST_METRICS_EGPRS_ACI_FILTER_POS     1
#define MDSP_BURST_METRICS_EGPRS_ACI_FILTER_MASK  0x6

/* Values for the ACI filter bits in the egprs_misc field */
#define MDSP_ACI_FILTER_0HZ      0x00
#define MDSP_ACI_FILTER_P200KHZ  0x01
#define MDSP_ACI_FILTER_N200KHZ  0x10
#define MDSP_ACI_FILTER_NARROW   0x11
#endif

#ifdef FEATURE_GSM_LOG_MDSP_SCALING_FACTORS
#error code not present
#endif



/* Structure to receive power measure metrics from mDSP in GPRS mode */
#ifdef FEATURE_GSM_DYNMC_PWR_MEAS
#define MAX_GPRS_RSSI_MEASUREMENTS  (12)
#else
#define MAX_GPRS_RSSI_MEASUREMENTS  (7) /* Changed for UOOS from 6 */
#endif /*FEATURE_GSM_DYNMC_PWR_MEAS*/

typedef PACKED struct
{
  uint16        len;
  uint16        seq_num;
  uint16        timeslots;
  uint16        rssi[MAX_GPRS_RSSI_MEASUREMENTS][2];
} mdsp_gprs_power_metrics_struct;

/* Structure to receive power measure metrics from mDSP in compressed mode */

typedef PACKED struct
{
  uint16        len;
  uint16        seq_num;
  uint16        rssi[MAX_CM_RSSI_MEASUREMENTS][2];
} mdsp_cm_power_metrics_struct;

/* Which burst types to use */
extern mdsp_burst_type mdsp_cm_mon_metrics_burst_types[MAX_CM_RSSI_MEASUREMENTS];

/* sub structure of generic config command */
#define MDSP_MEM_SEG_A  1
#define MDSP_MEM_SEG_B  2
#define MDSP_MEM_SEG_C  3
typedef PACKED struct
{
  uint16        mem_offset;
  uint16        data_h;
  uint16        data_l;
} mdsp_gen_config_buf_struct;

#ifdef FEATURE_POLAR

#ifndef MDSP_LIN_PA_TABLE_ENTRIES
  /* The Linear PA gain buff is 6 words. */
  #define MDSP_LIN_PA_TABLE_ENTRIES  6
#endif

/*
 * How many predistortion buffers needed for Linear PA, this is fixed for LCU
 * whereas for Polar PA we need to use the active number of tx slots
 * Need to be careful to match this to any RF driver updates, as this goes
 * to define the size of the amam block of data written to the mDSP which
 * has a fixed buffer allocated that we mustn't exceed
 */
#define MDSP_NUM_RF_PD_BUFFERS     2

/* There is one word for number of spurs and four */
/* int32 words for the spurs making 9 words total */
#define MDSP_TOTAL_SPUR_WORDS  9

#endif

/* Structures used with peek command */
#define MAX_PORTS_TO_PEEK         10
typedef PACKED struct
{
    uint16    data_h;
    uint16    data_l;
} mdsp_peek_data;


typedef PACKED struct
{
    uint16          len;
    mdsp_peek_data  data[MAX_PORTS_TO_PEEK];
} mdsp_peek_results_struct;

/* Linear PA tx gain buffer type */

#ifdef FEATURE_GSM_MDSP_GTOW
/* How many qs used for the filling the RF sample ram for WCDMA */
#define MDSP_FILL_SAMPLE_RAM_DURATION  2312
#endif

#define MDSP_NUM_SBI_STATIC_BUFFERS           0


#define  MDSP_MAX_IRAT_SBI_TRANSACTIONS    MAX_NUM_OF_NON_TIME_SBI


#define MDSP_MAX_SBI_STATIC_TRANSACTIONS      0
#define MDSP_MAX_NONSBI_STATIC_TRANSACTIONS  10
#define MDSP_MAX_NONSBI_REQUEST_TRANSACTIONS 20

/* Several burst buffers sized above use the same status buffer.
 * Therefore burst status buffer must be sized from largest.
 *
 * MDSP_MAX_BSBI_STATUS_TRANSACTIONS = max( MDSP_MAX_SBI_TRANSACTIONS,
 *                                          MDSP_SBI_RF_ON_NUM_TRANS,
 *                                          MDSP_SBI_RF_OFF_NUM_TRANS )
 */
#ifdef FEATURE_GSM_MDSP_GTOW
#define MDSP_MAX_BSBI_STATUS_TRANSACTIONS   MDSP_SBI_RF_ON_NUM_TRANS
#else
#define MDSP_MAX_BSBI_STATUS_TRANSACTIONS   MDSP_MAX_SBI_TRANSACTIONS
#endif

/****************************************************************************
                       Extra required interface include files
 ***************************************************************************/
/* GPRS specific definitions */
#include "mdsp_gprs.h"

/* DTM specific definitions */
#include "mdsp_dtm.h"

/* Needed to support SAIC for async_rx. */
typedef struct
{
   uint8                         range;   /* SDA RSSI changes */
   dBx16_T                       valx16;  /* SDA RSSI changes */
} mdsp_rf_rx_gain_type;

typedef struct
{
   boolean                       saic;           /* SAIC On/Off      */
   boolean                       idle;           /* IDLE MODE TRUE/FALSE */
   boolean                       dedicated;      /* Dedicated mode TRUE */
   mdsp_rf_rx_gain_type          rf_gain;        /* SDA RSSI changes */
   mdsp_rf_desense_chan_type     DesenseArfcn;   /* Set if desense ARFCN */
} mdsp_async_params_type;

/* RxLM / TxLM invalid buffer index indication */
#define MDSP_RXLM_BUF_IDX_INVALID  0xFFF
#define MDSP_TXLM_BUF_IDX_INVALID  0xFFF

/****************************************************************************
                            FUNCTION PROTOTYPES
 ***************************************************************************/

/*===========================================================================

FUNCTION mdsp_init_completed

DESCRIPTION
  Returns TRUE when the MDSP interface is initialised

DEPENDENCIES

===========================================================================*/
boolean mdsp_init_completed(gas_id_t gas_id );
void mdsp_set_init(gas_id_t gas_id);

/* ---------- Init functions ---------- */

extern boolean mdsp_asynch_dsp_int_enabled( gas_id_t gas_id );
extern boolean  mdsp_commands_pending( gas_id_t gas_id );
extern uint16 mdsp_get_current_seq_num ( gas_id_t gas_id );


extern void mdsp_set_last_debugftick( gas_id_t gas_id );

extern void mdsp_set_mdsp_gsm_txlm_buf_idx(uint32 buf_idx , gas_id_t gas_id );

#define NUM_OF_ELEMENTS_IN_GRFC_ITEM  3

/* Hold sequence numbers for async rx commands */
#define MDSP_ASYNC_RX_DATA_FIFO_LEN  3


/* Store pending nonsbi writes */
typedef struct
{
   uint32 reg;
   uint32 data;
} mdsp_pending_nonsbi_write_type;


typedef struct
{
  boolean sharedVarUpdated;
  boolean rxSharedVarUpdated;
  boolean txSharedVarUpdated;
  boolean edgeSharedVarUpdated;

} MDSP_Generic_Cmd_Flags_Structure;

/* This value is mostly dictated by how many nonsbi writes are performed by
   the NPL1 and the RF driver during init */
#define MAX_PENDING_NONSBI_WRITES 30

/* --------------------
 * GSM RX command control field constants
 * --------------------
 */
#define MDSP_RX_CTL_MASK_DECODE              0x1  /* 0x0001 */
#define MDSP_RX_CTL_SHFT_DECODE                0
#define MDSP_RX_CTL_MASK_SEARCH_WIDTH       0xFF  /* 0xFF00 */
#define MDSP_RX_CTL_SHFT_SEARCH_WIDTH          8
#define MDSP_RX_CTL_FIELD(field, value) \
  (uint16)(((value) & MDSP_RX_CTL_MASK_##field) << MDSP_RX_CTL_SHFT_##field)

/* --------------------
 * GSM RX command channel field constants
 * --------------------
 */
#define MDSP_RX_CHN_MASK_CHANNEL_TYPE       0x3F  /* 0x003F */
#define MDSP_RX_CHN_SHFT_CHANNEL_TYPE          0
#define MDSP_RX_CHN_MASK_USE_PHASE_INCREMENT 0x1  /* 0x0400 */
#define MDSP_RX_CHN_SHFT_USE_PHASE_INCREMENT  10

#define MDSP_RX_CHN_FIELD(field, value) \
  (uint16)(((value) & MDSP_RX_CHN_MASK_##field) << MDSP_RX_CHN_SHFT_##field)

/* Use rotator bit for the test_tx command */
#define MDSP_TEST_TX_USE_ROTATOR_BIT 0x4
/* Modulation type bit position in data_mode field of test_tx cmd */
#define MOD_TYPE_BIT_IN_TX_DATA_MODE  3

/* From the Logic Analyzer trace, we found that max time that mdsp_nonsbi_write
   takes is 380MicroSec - 150 symbols, i.e. 554Micro Sec is choosen.
   ie How long into a TDMA frame that a nonsbi_write can occur.
 */
#define MDSP_MAX_SYMBOLS_FOR_NONSBI_WRITE (1250-100)

typedef struct
{
boolean acq_log_iq;
boolean sch_log_iq;
boolean gprs_pwr_mea_iq;
boolean x2g_pwr_meas_iq ;
boolean sync_rx_iq ;

}mdsp_iq_lg_type;

/* Poll for "mdsp_frame_tick_count" to increment.  Check every 1 msec for up
** to 10 times before giving up. */
#define MDSP_FRAME_TICK_POLL_TIMEOUT_MSEC   1
#define MDSP_FRAME_TICK_POLL_COUNT_MAX     10

extern void dcache_inval_region(void *addr, unsigned int length);
extern void rfm_log_iq_data(GfwIqSamplesBuffer* data );

uint8 mdsp_current_buffer(gas_id_t gas_id );
uint8 mdsp_current_buffer_mod3(gas_id_t gas_id );


/*===========================================================================
  MULTI-DATASPACED PUBLIC DATA
===========================================================================*/
extern void * mdsp_getcmd_buf_address(void);

typedef struct
{
  /* Sync rx sequence number fifos */
  uint16    mdsp_sync_rx_burst_seq_num_fifo[3];

  MDSP_Generic_Cmd_Flags_Structure pendingGfwGenericCmd;

  /* BSIC to be used as part of every RACH transmission */
  uint8 mdsp_BSIC;

  /* Tx rotator enable bit */
  boolean mdsp_tx_rotator;
  mdsp_xo_desense_arfcn_struct desense_chanel;

  /* Counter to implement dsp_wait_for_frame_tick(). */
  volatile uint32 mdsp_frame_tick_count;

  uint16 mdsp_tx_burst_offset;

#ifdef FEATURE_GSM_MDSP_AEQ_8PSK
  /* AEQ Ph2 8PSK ENABLE FLAG */
  boolean mdsp_aeq_8psk;
#ifdef FEATURE_GSM_DYNAMIC_AEQ_8PSK
  boolean mdsp_aeq_8psk_dynamic;
#endif /* FEATURE_GSM_DYNAMIC_AEQ_8PSK */
#endif

#ifdef FEATURE_GSM_MDSP_AEQ_SCH
  boolean mdsp_aeq_sch_enable;
#endif

  boolean use_idle_mode_rx_alpha;

  volatile GfwHostMdspResultBuffs* pMdspHostResultsStore;
  volatile GfwHostMdspInterfaceSegment* pMdspHostIfBaseStore;

  /* indicate if we are pending to send set app mode cmd to fw or not */
  uint16 tx_config_cmd_pending;
  mdsp_iq_lg_type gsm_Iq_Log;

#ifdef FEATURE_GSM_TDS
  volatile boolean external_control;
#endif

  boolean mdsp_async_imm_cmd_sent;

  /* GSTMR handler for mdsp switch command buffers */
  void (*mdsp_switch_command_buffer_handler)(void);

} mdsp_intf_public_data_t;

#ifdef T_WINNT  /* __attribute__ is not supported by MSVC and under WINNT we don't care about deprecated APIs anyway */
#error code not present
#else
#define GMDSP6_DEPRECATED_ATTRIBUTE          __attribute__((deprecated))
#endif /* T_WINNT */

/****************************************************************************
                   mDSP   DEFINITIONS  and  ENUMERATIONS
 ***************************************************************************/

#ifdef FEATURE_GSM_RFLM_FED
typedef enum
{
   RX_MODE             = RFGSM_RX_SYNC_BURST,
   MON_MODE            = RFGSM_RX_MON_BURST,   	
   ACQ_MODE            = RFGSM_RX_SCH_OR_ACQ_BURST,
   TX_MODE             = RFGSM_MAX_RX_BURST_TYPES,   
   RX_MODE_ASYNC       = RFGSM_RX_PCH_BURST
} mdsp_burst_mode;
#else
typedef enum
{
   RX_MODE             = 0,
   MON_MODE            = 1,   	
   ACQ_MODE            = 2,
   TX_MODE             = 3,   
   RX_MODE_ASYNC       = 4  
} mdsp_burst_mode;
#endif


/* Shared variable class types, added for Nikel */
typedef enum
{
  GFW_RX_SHARED_VARIABLE   = 0,
  GFW_TX_SHARED_VARIABLE,
  GFW_EDGE_SHARED_VARIABLE,
  GFW_SHARED_VARIABLE_FALSE             /* Used by gdrivers to set sharedVarUpdated FALSE */
} gfw_shared_variable_class_type;

#ifndef FEATURE_GSM_RFLM_FED 
/* Definitions for the PA Lookup Table */
#define MDSP_PA_UP_RAMP_ENTRY_CNT    30  /* Nominal value */
#define MDSP_PA_DOWN_RAMP_ENTRY_CNT  30  /* Nominal value */
#define MDSP_PA_TRANSITION_PROFILE_ENTRY_CNT  48
#define MDSP_PA_TRANSITION_OFFSET             16

typedef PACKED struct
{
  uint16 pa_lut_cmd_msw;    /* MSW of the PA_LUT_CMD */
  uint16 pa_lut_cmd_lsw;    /* LSW of the PA_LUT_CMD */
} mdsp_pa_lut_cmd_type;

typedef PACKED struct
{
  uint16                cnt;
  uint16                seq_num;
  mdsp_pa_lut_cmd_type  lut[MAX_NUM_PA_LUT_ADDRESS];
} mdsp_pa_lut_results_struct;

typedef int16 *mdsp_pa_lut_ptr_type;

/* PA transition profile table */
typedef struct
{
  int16 profile[MDSP_PA_TRANSITION_PROFILE_ENTRY_CNT];
} mdsp_pa_transition_profile_type;
#endif /* FEATURE_GSM_RFLM_FED */

/* Define the number of burst types */
#define MDSP_NUM_BURST_TYPES  ((14 /*WCDMA_DPLL_BURST*/)+1)
#define MDSP_NUM_NONSBI_STATIC_BUFFERS        3

/* Linear PA tx gain buffer type */

#define MDSP_LIN_PA_TABLE_SIZE (sizeof(GfwTxRfGainParam))
#define mdsp_linear_tx_gain_buff_type GfwTxRfGainParam

#define MDSP_PWR_MEAS_REDUCED_LEN (48)

/* This SBI Buff size is coupled with MDSP_MAX_SBI_TRANSACTIONS.
 * For RTR8600 PLL lock issue W+G image cannot accomadate 38 SBI buff
 * size so we need to default to max 24 items
 */

#define MDSP_MAX_CM_SBI_TRANSACTIONS                 MDSP_MAX_SBI_TRANSACTIONS
#define MDSP_MAX_CM_CLEANUP_SBI_TRANSACTIONS         64

#define MDSP_SBI_RF_ON_NUM_TRANS             24
#define MDSP_SBI_RF_OFF_NUM_TRANS            40

#define MDSP_NONSBI_RF_ON_NUM_TRANS          MDSP_MAX_NONSBI_TRANSACTIONS
#define MDSP_NONSBI_RF_OFF_NUM_TRANS         MDSP_MAX_NONSBI_TRANSACTIONS



/****************************************************************************
                            SBI Control Structures
 ***************************************************************************/

#define SBI_MDSP_CTL_FIFO_CM_SWITCH_EN           (1 << 0)
#define SBI_MDSP_CTL_FIFO_REQ_MODE               (1 << 1)
#define SBI_MDSP_CTL_FIFO_WAIT_PA_DISABLE        (1 << 2)
#define SBI_MDSP_CTL_FIFO_TRIGGER                (1 << 3)
#define SBI_MDSP_CTL_FIFO_INTR_EN                (1 << 4)

#define SBI_MDSP_IQ_MODE                         (1 << 6)

#define SBI_MDSP_CTL_FIFO_TIMED ( SBI_MDSP_CTL_FIFO_REQ_MODE )

/* ENUM for GFW shared variables */
typedef enum
{
  GFW_DEBUG = 0,
  GFW_VAMOS_SUPPORT = 1,
  GFW_DIAG1 = 2,
  GFW_DIAG2 = 3,
  GFW_DIAG3 = 4,
  GFW_DIAG4 = 5,  
  GFW_RX_ALPHA = 6
} MdspSharedVariableType;


extern mdsp_iq_lg_type gsm_Iq_Log ;


/*===========================================================================
  NON-STATIC FUNCTIONS
===========================================================================*/
mdsp_intf_public_data_t* get_mdsp_intf_public_dataspace_ptr(const gas_id_t gas_id);

/****************************************************************************
                            FUNCTION PROTOTYPES
 ***************************************************************************/

/*===========================================================================

FUNCTION mdsp_gprs_issue_IR_conigure_memory

DESCRIPTION
  Sends pending IR configure memory commands (from ISR context).

===========================================================================*/
void mdsp_gprs_issue_IR_configure_memory(sys_modem_as_id_e_type as_id);
/*===========================================================================

FUNCTION mdsp_update_shared_variable

DESCRIPTION
  This function records that a GFW shared variable has been updated, so
  GFW Generic cmd should be sent on the next frame tick.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_updated_shared_variable
(
  gfw_shared_variable_class_type shared_variable_class, gas_id_t gas_id
);


/*===========================================================================

FUNCTION mdsp_update_shared_variable

DESCRIPTION
  This function populates the shared variables in the pending generic config buffer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_update_shared_variable
(
  MdspSharedVariableType sharedVariableType,
  uint32 *pValue, /* this is may array or single value depending upon the shared variable type */
  gas_id_t gas_id
);


/*===========================================================================

FUNCTION mdsp_switch_command_buffers

DESCRIPTION
  This function switches the command buffer in use such that new commands
  should now be placed into a new buffer.  This function should be called
  from the frame tick isr as that is when the mdsp will begin processing
  of the 'old' command buffer.

  It also performs some sanity check to ensure there are no dsp errors.

===========================================================================*/
void mdsp_switch_command_buffers (sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_reset_command_buffers

DESCRIPTION
  This function resets the command buffer pointer and headers.

===========================================================================*/
void mdsp_reset_command_buffers (gas_id_t gas_id);

/* ---------- GRFC functions ---------- */
/* These are stubs for QDSP6 */
/*===========================================================================

FUNCTION mdsp_get_grfc_omode

DESCRIPTION
  This function returns the current shadow register of the grfc omode
  buffer.

DEPENDENCIES
  None

RETURN VALUE
  grfc omode buffer shadow register.

SIDE EFFECTS
  None
===========================================================================*/
uint32 mdsp_get_grfc_omode( void ) GMDSP6_DEPRECATED_ATTRIBUTE;

/*===========================================================================

FUNCTION mdsp_get_grfc_tristate

DESCRIPTION
  This function returns the current shadow register of the grfc tristate
  buffer.

DEPENDENCIES
  None

RETURN VALUE
  grfc tristate buffer shadow register.

SIDE EFFECTS
  None
===========================================================================*/
uint32 mdsp_get_grfc_tristate( void ) GMDSP6_DEPRECATED_ATTRIBUTE;

/*===========================================================================

FUNCTION mdsp_get_grfc_polarity

DESCRIPTION
  This function returns the current shadow register of the grfc polarity
  buffer.

DEPENDENCIES
  None

RETURN VALUE
  grfc polarity buffer shadow register.

SIDE EFFECTS
  None
===========================================================================*/
uint32 mdsp_get_grfc_polarity( void ) GMDSP6_DEPRECATED_ATTRIBUTE;

/*===========================================================================

FUNCTION mdsp_get_grfc_out

DESCRIPTION
  This function returns the current shadow register of the grfc data
  buffer.

DEPENDENCIES
  None

RETURN VALUE
  grfc data buffer shadow register.

SIDE EFFECTS
  None
===========================================================================*/
uint32 mdsp_get_grfc_out( void ) GMDSP6_DEPRECATED_ATTRIBUTE;

/*===========================================================================

FUNCTION mdsp_grfc_tristate

DESCRIPTION
  This function is used set the direction (output/input) for the bits in the
  GRFC_DATA registers.  The direction register is MDSP_REG_grfc_dir_ctl.

  mask  - bit mask to be tri-stated.
  data - data to write

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void   mdsp_grfc_tristate ( uint32 mask, uint32 data ) GMDSP6_DEPRECATED_ATTRIBUTE;
void   mdsp_grfc_out ( uint32 mask, uint32 data ) GMDSP6_DEPRECATED_ATTRIBUTE;
uint32 mdsp_grfc_in ( uint32 mask, uint16 waitms ) GMDSP6_DEPRECATED_ATTRIBUTE;
void   mdsp_grfc_omode ( uint32 mask, uint32 data ) GMDSP6_DEPRECATED_ATTRIBUTE;
void   mdsp_grfc_polarity ( uint32 mask, uint32 data ) GMDSP6_DEPRECATED_ATTRIBUTE;
boolean mdsp_grfc_write_async( uint16 reg, uint32 mask, uint32 data ) GMDSP6_DEPRECATED_ATTRIBUTE;

/*===========================================================================
FUNCTION mdsp_disabled
DESCRIPTION
  This function is called when the MDSP is disabled
===========================================================================*/
void mdsp_disable(gas_id_t gas_id);

/*===========================================================================
FUNCTION mdsp_enable
DESCRIPTION
  This function is called to enable the MDSP
===========================================================================*/
void mdsp_enable(gas_id_t gas_id);

#ifdef FEATURE_G2X_TUNEAWAY
/*===========================================================================
FUNCTION mdsp_enable
DESCRIPTION
  This function is called to enable the MDSP
===========================================================================*/
void mdsp_init_G2X(gas_id_t gas_id);
#endif

/* ---------- Uplink power control functions ---------- */
/* Polar RF tables */
void mdsp_cfg_pa_profile_polar
 (
   mdsp_pa_ramp_type *ramp_tbl,
   gas_id_t gas_id
 );

/*===========================================================================

FUNCTION mdsp_cfg_amam_ampm_tables

DESCRIPTION
  This functions configures the AM/AM and AM/PM tables for uplink
  power control.  It should be called once to set the pointer to the
  table location.

===========================================================================*/
void mdsp_cfg_amam_ampm_tables(mdsp_amam_ampm_tables_type *amam_ampm_tables, gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_update_amam_ampm_tables

DESCRIPTION
  This functions flags the AM/AM and AM/PM tables as having changed.

===========================================================================*/
void mdsp_update_amam_ampm_tables(boolean new_am_am_table, boolean new_am_pm_table, gas_id_t gas_id);


/*===========================================================================

FUNCTION mdsp_send_tx_cal_cmd

DESCRIPTION
  This function tells the mDSP to do the tx cal
  Explanation:
  This function is for KV calibration in FTM mode by sending
  ths sbi and grfc buffers to fw. This command also has the
  num of steps in the current frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_send_tx_cal_cmd ( mdsp_ftm_tx_cal_struct *tx_cal_buffer , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_set_tx_path_delay

DESCRIPTION
  This function set the dynamic tx path delay

===========================================================================*/
void mdsp_set_tx_path_delay (int16 delay_val , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_set_tx_smps_pdm_value

DESCRIPTION
  This function set the smps pdm value

===========================================================================*/
void mdsp_set_tx_smps_pdm_value (int16 pdm_value , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_update_linear_tx_gain_buff

DESCRIPTION
  This function is called by the RF driver to signal mdsp drivers that
  the Linear PA settings need to be sent to the mDSP this frame..

===========================================================================*/
void mdsp_update_linear_tx_gain_buff (gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_update_parange_init_data

DESCRIPTION
  This function sends the PA Range GRFCs information to the DSP. It should
  only be used when gsm transmit chain is in linear mode.

  The RF drivers will call this at power up or when GSM is activated.

===========================================================================*/
void mdsp_update_parange_init_data(mdsp_nonsbi_ctl_unit **script_ptr, gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_start_tx_rnd_cont

DESCRIPTION
  This function tells the mDSP to transmit a tone every frame with random data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION mdsp_start_tx_tone_cont

DESCRIPTION
  This function tells the mDSP to transmit a tone every frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_start_tx_tone_cont ( gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_stop_tx_tone_cont

DESCRIPTION
  This function tells the mDSP to stop transmitting a tone every frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_stop_tx_tone_cont ( gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_start_tx_data_cont

DESCRIPTION
  This function tells the mDSP to transmit user defined data.
  Explanation:
  This function is for KV calibration in FTM mode by sending
  training data to reduce calibration time. It is similar to
  dynamic KV  calibration in online mode.  Instead of having
  G FW hard coding the training data as in online mode,  FTM
  code will pass training data  (user define)  to  FW during
  calibration.  Also,  we  don’t want G  FW to hard code the
  training  data in  continuous  TX.  This avoids disrupting
  RF calibration due to continuous TX being used in multiple
  target calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_start_tx_data_cont ( uint16 *buffer, uint16 buf_len, gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_cm_check_for_error_recovery

DESCRIPTION
   This function checks whether there is an MDSP error that requires the
   CM panic reset.

===========================================================================*/
boolean mdsp_cm_check_for_error_recovery( gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_get_wr_num

DESCRIPTION
  Gets the Write count

===========================================================================*/
extern uint32 mdsp_get_wr_num(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION mdsp_set_tx_burst_offset

DESCRIPTION
  These functions set and set the offset of the Tx burst from its true
  slot 3 offset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_set_tx_burst_offset (uint16 tx_burst_offset, gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_get_gfw_event_buffer

DESCRIPTION
  Returns pointer to GFW event buffer

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/
void* mdsp_get_gfw_event_buffer(mdsp_burst_mode burst_mode,uint8 burst_index, gas_id_t gas_id);
/*===========================================================================

FUNCTION mdsp_get_gfw_init_event_buffer

DESCRIPTION
  Returns pointer to GFW init event buffer

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/
GfwRfInitEvent* mdsp_get_gfw_init_event_buffer(gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_get_gfw_init_event_buffer_DivRx

DESCRIPTION
  Returns pointer to GFW init event buffer for diversity chain

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/

GfwRfInitEvent* mdsp_get_gfw_init_event_buffer_DivRx(gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_get_gfw_tx_band_event_buffer

DESCRIPTION
  Returns pointer to GFW tx band event buffer

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/
volatile GfwRfSetTxBandEvent* mdsp_get_gfw_tx_band_event_buffer(gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_get_gfw_therm_read_event_buffer

DESCRIPTION
  Returns pointer to GFW thermistor read CCS event buffer

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/
GfwRfThermReadEvent* mdsp_get_gfw_therm_read_event_buffer(gas_id_t gas_id);

void mdsp_set_asynch_int_enabled( boolean set_asynch_int , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_dtm_use_saic

DESCRIPTION
  This function determines if SAIC should be used for the given
  channel.

===========================================================================*/
boolean mdsp_dtm_use_saic (mdsp_channel_type chan, boolean saic_cfg , gas_id_t gas_id );

#ifdef FEATURE_GSM_MDSP_ESAIC
/*===========================================================================

FUNCTION mdsp_dtm_use_esaic

DESCRIPTION
  This function determines if ESAIC should be used for the given
  channel.

===========================================================================*/
boolean mdsp_dtm_use_esaic (mdsp_channel_type chan, boolean saic_cfg , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_dtm_disable_iir_filter

DESCRIPTION
  This function determines if we need to disable IIR filter based on chan type
  current design specifies PCH, NCELL BCCH and SCELL BCCH IIR filter should be
  disabled

===========================================================================*/
boolean mdsp_dtm_disable_iir_filter(mdsp_channel_type chan,boolean idle_mode);

#endif /* FEATURE_GSM_MDSP_ESAIC */

/*===========================================================================

FUNCTION mdsp_dtm_use_aeq

DESCRIPTION
  This function determines if AEQ should be used for the given
  channel.

===========================================================================*/
boolean mdsp_dtm_use_aeq (mdsp_channel_type chan,boolean dedicated_mode, boolean saic_cfg , gas_id_t gas_id );


#ifdef FEATURE_GSM_MDSP_AEQ_8PSK
/*===========================================================================

FUNCTION mdsp_dtm_use_aeq_8psk

DESCRIPTION
  This function determines if AEQ Ph2 8PSK should be used for the given
  channel.

===========================================================================*/

boolean mdsp_dtm_use_aeq_8psk(mdsp_channel_type chan, boolean saic_cfg , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_enable_aeq

DESCRIPTION
  This function enables or disables AEQ

===========================================================================*/
void mdsp_enable_aeq_8psk (boolean enable , gas_id_t gas_id );

#endif /* FEATURE_GSM_MDSP_AEQ_8PSK */

#ifdef FEATURE_GSM_RX_DIVERSITY
/*===========================================================================

FUNCTION mdsp_dedicated_use_aeq

DESCRIPTION
  This function determines if AEQ should be used in dedicated mode (TCH/DTM mode).

===========================================================================*/
boolean mdsp_dedicated_use_aeq (boolean dedicated_mode, boolean saic_cfg,gas_id_t gas_id);

#ifdef FEATURE_GSM_RX_DIVERSITY_IDLE
/*===========================================================================

FUNCTION mdsp_idle_use_aeq

DESCRIPTION
  This function checks if AEQ is used in idle mode 

===========================================================================*/
boolean mdsp_idle_use_aeq (boolean idle_mode, boolean saic_cfg, gas_id_t gas_id);
#endif /* FEATURE_GSM_RX_DIVERSITY_IDLE */
#endif /* FEATURE_GSM_RX_DIVERSITY */

/* ---------- Miscellaneous functions ---------- */
void mdsp_ciphering_enable (mdsp_ciphering_type mode, uint32 *CipherKey, gas_id_t gas_id);
void mdsp_ciphering_disable (gas_id_t gas_id);
void mdsp_update_ciphering_fn (uint32 fn , gas_id_t gas_id );

void mdsp_issue_app_mode_command_if_required(gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_wait_for_frame_tick

DESCRIPTION
  This function will not return until just after the next frame tick.

DEPENDENCIES
  mdsp_switch_command_buffers() must be installed as a GSTMR interrupt
  handler or this function will not return at all.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/


void mdsp_wait_for_frame_tick (gas_id_t gas_id);

boolean mdsp_wait_for_frame_tick_non_blocking( rex_tcb_type  *tcb_ptr,
                                            rex_sigs_type  wait_sig
                                            , gas_id_t gas_id );
void mdsp_taf_control (gas_id_t gas_id );
void mdsp_set_loopback (mdsp_loopback_type type , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_issue_generic_config_command

DESCRIPTION
  This function ssues a generic configuration command to the firmware.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mdsp_issue_generic_config_command(gas_id_t gas_id );

/* ---------- Init functions ---------- */
/*===========================================================================

FUNCTION mdsp_init

DESCRIPTION
  This function initializes data structures to enable communications with
  the mdsp.

DEPENDENCIES
  mdsp image must already be loaded.

===========================================================================*/
void mdsp_init (gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_init_public_data_space

DESCRIPTION
  This function initializes the data structures mdsp_intf_public_data

DEPENDENCIES


===========================================================================*/
void mdsp_init_public_data_space(gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_init_private_data_space

DESCRIPTION
  This function initializes the data structures mdsp_intf_private_data

DEPENDENCIES


===========================================================================*/
void mdsp_init_private_data_space(gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_set_mdsp_compressed_mode

DESCRIPTION
  This function sets mdsp_compressed_mode flag true.

 ===========================================================================*/
void mdsp_set_mdsp_compressed_mode ( gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_init_MdspHostIfBaseStore

DESCRIPTION
  This function initializes pMdspHostIfBaseStore pointer.
  Init the pMdspHostIfBaseStore as this store may be read without TRM lock
  before mdsp image is loaded.

DEPENDENCIES


===========================================================================*/
void mdsp_init_MdspHostIfBaseStore (gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_sleep_reset_mdsp_intf

DESCRIPTION
  This function initializes data structures to enable communications with
  the mdsp in DS G+G mode.

DEPENDENCIES
  mdsp image must already be loaded.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_sleep_reset_mdsp_intf( gas_id_t gas_id );

void mdsp_feature_check (gas_id_t gas_id );


void mdsp_inact (gas_id_t gas_id );
void mdsp_set_dsp_error_mask (uint32 mask0, uint32 mask1 , gas_id_t gas_id );
void mdsp_set_dsp_warning_mask (uint32 mask0, uint32 mask1 , gas_id_t gas_id );


/* ---------- Receive control functions ---------- */
void mdsp_set_sync_rx_time_offset( uint16 offset , gas_id_t gas_id );
void mdsp_set_sync_rx_search_width( uint16 width , gas_id_t gas_id );
void mdsp_set_async_rx_search_width( uint16 width , gas_id_t gas_id );
uint16 mdsp_get_sync_rx_time_offset (gas_id_t gas_id);
uint16 mdsp_get_sync_rx_search_width( gas_id_t gas_id );
uint16 mdsp_get_async_rx_search_width( gas_id_t gas_id );

void mdsp_async_rx_sch( uint16   time_offset,
	                    int32    phase_increment,
#ifdef FEATURE_GSM_COEX
                        uint32   cxm_priority,
                        uint32   cxm_desense_id,
#endif
                        ARFCN_T   arfcn,
#ifdef FEATURE_GSM_RX_DIVERSITY
                        boolean enable_rxdiversity,
#endif /*FEATURE_GSM_RX_DIVERSITY*/      
	                    gas_id_t gas_id
	                    );

void mdsp_x2g_async_rx_sch( uint16                   time_offset,
                            int32                    phase_increment,
                            uint8*                   sch_metrics_buffer_idx_ptr,
                            uint16*                  sch_metrics_seq_num_ptr,
                            uint8*                   sch_data_buffer_idx_ptr,
                            ARFCN_T                  arfcn,
                            gas_id_t                 gas_id
                          );


void mdsp_async_rx
 (
   /* Added mdsp_async_params_type */
   mdsp_async_params_type *params,
   mdsp_channel_type       chan,
   uint8                   TSC,
   boolean                 decode,
   boolean                 early_decoding,
   boolean                 sbd_enabled,
   boolean                 epd_allowed,
   uint8                   burst_num,
   uint16                  time_offset,
   int32                   phase_increment,
   mdsp_burst_mode         mdsp_burst_type,
#ifdef FEATURE_GSM_COEX   
   uint16                  arfcn,
   uint32                  cxm_priority,
   uint32                  cxm_desense_id,
#endif	   
   gas_id_t                gas_id
 );
#ifndef FEATURE_GSM_MDSP_DTM
void mdsp_sync_rx
 (
   mdsp_channel_type chan,
   uint8 TSC,
   boolean decode,
   boolean early_decoding,
   uint8 burst_num,
   boolean force_facch,
   boolean look_for_facch
 );
#ifdef FEATURE_GSM_AMR
void mdsp_sync_rx_amr
 (
    mdsp_channel_type chan,
    uint8 TSC,
    boolean decode,
    boolean force_facch,
    boolean look_for_facch,
    uint8 amr_rx_phase
 );
#endif
void mdsp_advance_rx_metrics_ptrs(gas_id_t gas_id );
#endif /* !FEATURE_GSM_MDSP_DTM */
void mdsp_reset_rx_seq_nums( gas_id_t gas_id );
void mdsp_reset_async_rx_ptrs( gas_id_t gas_id );
#ifdef FEATURE_GSM_AMR
void mdsp_dl_codec_set_configure_cmd(mdsp_amr_codec_mode_type *codec_mode , gas_id_t gas_id );
#endif
void mdsp_enable_saic (boolean enable , gas_id_t gas_id );
boolean mdsp_get_enable_saic (gas_id_t gas_id );

#ifdef FEATURE_GSM_MDSP_ESAIC
void mdsp_enable_esaic (boolean enable , gas_id_t gas_id );
boolean mdsp_get_enable_esaic (gas_id_t gas_id );
void mdsp_flush_esaic_iir_filter( boolean flush_esacch,boolean restore_ersacch , gas_id_t gas_id );
#endif

void mdsp_enable_aeq (boolean enable , gas_id_t gas_id );
boolean mdsp_get_enable_aeq(gas_id_t gas_id );

#ifdef FEATURE_GSM_MDSP_AEQ_SCH
void mdsp_enable_aeq_sch (boolean enable , gas_id_t gas_id );
#endif
/* ---------- Results retrieval functions ---------- */
boolean mdsp_get_FCCH_results ( GfwFcchResultStruct *data , gas_id_t gas_id );

boolean mdsp_x2g_get_FCCH_results ( GfwFcchResultStruct *data, uint16  seq_num , gas_id_t gas_id );

boolean mdsp_get_SCH_results ( GfwSchResultStruct *data , gas_id_t gas_id );



boolean mdsp_get_async_decoded_data
 (
    GfwDecodedDataStruct *hdr,
    uint16             *data_field,
    uint16              data_len,
    boolean             last_time
    , gas_id_t gas_id
 );

boolean mdsp_get_ncell_sch_decoded_data
(
   GfwSchDecodeDataStruct *hdr,
   uint16                 *data,
   uint16                 data_len,
   boolean                last_attempt
   , gas_id_t gas_id
);

boolean mdsp_x2g_get_ncell_sch_decoded_data
(
   GfwSchDecodeDataStruct*  hdr_ptr,
   uint16                   data[2],  /* MAX_SCH_OCTETS / 2 */
   uint16                   data_len,
   boolean                  last_attempt,
   uint8                    sch_data_buffer_idx,
   uint16                   sch_data_seq_num
   , gas_id_t gas_id
);
boolean mdsp_get_async_rx_metrics
 (
    GfwBurstMetricsStruct     *metrics,
    boolean last_time,
    boolean sch_flag
    , gas_id_t gas_id
 );

boolean mdsp_x2g_get_async_sch_rx_metrics
 (
    GfwBurstMetricsStruct  *metrics,
    boolean                last_time,
   uint8                   metrics_buffer_idx,
   uint16                  seq_num
   , gas_id_t gas_id
 );

boolean mdsp_get_debug_data ( uint16 *data, uint16 offset, uint16 length, gas_id_t gas_id );


boolean mdsp_get_iq_data(int16 *data, uint16 length , gas_id_t gas_id );

#ifdef FEATURE_GSM_LOG_MDSP_SCALING_FACTORS
#error code not present
#endif
/* ---------- Uplink power control functions ---------- */
/*===========================================================================

FUNCTION mdsp_cfg_pa_profile_direct

DESCRIPTION
  This function sets the static parameters of the PA profile for the POLAR
  RF.

===========================================================================*/
void mdsp_cfg_pa_profile_direct(mdsp_pa_ramp_type *ramp_tbl, gas_id_t gas_id);


/* ---------- Acquisition functions ---------- */
void mdsp_start_acquisition
 (
    uint16            offset,
    boolean           decode,
    mdsp_acq_afc_type afc,
    boolean           part_tone_det,
    int32             phase_increment,
#ifdef FEATURE_GSM_COEX
    uint32            cxm_priority,
    uint32            cxm_desense_id,
    boolean           enhanced_fcch_detection,
#endif
    ARFCN_T           arfcn,
    gas_id_t gas_id
 );
void mdsp_stop_acquisition( uint16 offset , gas_id_t gas_id );



/* ---------- Transmit control functions ---------- */
void mdsp_set_bsic (uint8 bsic , gas_id_t gas_id );
void mdsp_enable_tx_rotator(boolean enable , gas_id_t gas_id );
uint16 mdsp_get_tx_burst_offset (gas_id_t gas_id );
#ifndef FEATURE_GSM_MDSP_DTM
void mdsp_transmit_burst
 (
    mdsp_channel_type chan,
    uint16            ta,  /* symbols */
    uint16            tx_alpha,  /* qs */
    uint8             TSC,
    boolean           encode,
    uint16            *buffer,
    uint16            buf_len
 );
void mdsp_transmit_burst_no_data
 (
    mdsp_channel_type chan,
    uint16            ta,  /* symbols */
    uint16            tx_alpha, /* qs */
    uint8             TSC,
    boolean           encode
 );
void mdsp_write_tx_data
 (
    mdsp_channel_type chan,
    uint8 *data, uint8 len,
    uint8 *secondary_data, uint8 secondary_len
 );
#ifdef FEATURE_GSM_AMR
void mdsp_write_tx_data_amr
 (
    mdsp_channel_type chan,
    uint8 *data, uint8 len
 );
#endif
#endif /* !FEATURE_GSM_MDSP_DTM */

/* ---------- Sleep functions ---------- */
/*===========================================================================

FUNCTION mdsp_prepare_for_sleep

DESCRIPTION
  Function called to check that the firmware is ready to go to sleep, and
  if so put it to sleep.  Does not disable clocks, this is done by calling
  mdsp_go_to_sleep.
  We do not display error messages directly in this function as it may
  disturb the sleep timeline.

===========================================================================*/
boolean mdsp_prepare_for_sleep ( gas_id_t gas_id );
void mdsp_go_to_sleep(gas_id_t gas_id );
boolean mdsp_awake( gas_id_t gas_id );
boolean mdsp_is_enabled(gas_id_t gas_id );
boolean mdsp_ready_for_sleep ( gas_id_t gas_id );
void mdsp_display_sleep_error( boolean print_nacked_error_msg , gas_id_t gas_id );
boolean mdsp_sleep_error_pending ( gas_id_t gas_id );

void mdsp_set_tx_config_cmd_pending(uint32 txlm_buf_idx, gas_id_t gas_id);
void  mdsp_gsm_config_tx( uint32 buf_idx , gas_id_t gas_id );
void  mdsp_gsm_config_tx_channel_id(uint32 buf_idx, uint8 tx_channel_id, gas_id_t gas_id);

void  mdsp_afc_update_cmd( int32 f_hz_q6 ,uint32 inv_f_dl_lo , gas_id_t gas_id );

void  mdsp_set_check_cm_cleanup_buffer( uint8  buffer_index, gas_id_t gas_id );

/* ---------- Compressed mode functions ---------- */
void mdsp_cm_init ( gas_id_t gas_id ) ;
void mdsp_cm_inact ( boolean disable_fw, gas_id_t gas_id ) ;
void * mdsp_cm_startup( uint16  offset, 
						uint8   buffer_index,
                      	uint32 rxlm_buf_idx, 
						rfm_device_enum_type device, 
						gas_id_t gas_id );
#ifdef FEATURE_GSM_RFLM_FED
void mdsp_cm_preload( uint16 pretune_offset, uint16  startup_offset, uint32 rxlm_buf_idx, uint8 buffer_index, boolean immediate_preload, gas_id_t gas_id );
#endif
void *mdsp_cm_cleanup( uint16  offset, uint8 buffer_index, boolean rf_tuneback, uint32 rxlm_buf_idx , gas_id_t gas_id );

void mdsp_cm_power_measure( uint16                   num,
#ifdef  FEATURE_GSM_COEX
                            ARFCN_T                  ARFCN[MAX_CM_RSSI_MEASUREMENTS],
#endif
                            uint16                   offset[MAX_CM_RSSI_MEASUREMENTS],
#ifdef  FEATURE_GSM_COEX
			                      mdsp_gprs_monitor_coex_params  coex_params[MAX_CM_RSSI_MEASUREMENTS],
#endif
#ifdef GERAN_L1_HLLL_LNASTATE
                            boolean                        sawless_support[MAX_CM_RSSI_MEASUREMENTS],
#endif /*GERAN_L1_HLLL_LNASTATE*/
                            gas_id_t gas_id
                          );
void mdsp_build_fw_burst_buffer (  uint16 id, uint16 data, mdsp_burst_type burst, gas_id_t gas_id );

boolean mdsp_cm_get_power_measure_results( uint32    rssi_values[MAX_CM_RSSI_MEASUREMENTS],
                                           boolean   yield[MAX_CM_RSSI_MEASUREMENTS],
#ifdef GERAN_L1_HLLL_LNASTATE 
                                           uint16    jdetvalues[MAX_CM_RSSI_MEASUREMENTS],
#endif /*GERAN_L1_HLLL_LNASTATE*/
                                           gas_id_t  gas_id );

boolean mdsp_cm_active ( gas_id_t gas_id );

void    mdsp_x2g_power_measure (  uint16                         num,
                                  uint8                          results_buffer,
                                  ARFCN_T                        ARFCN[MAX_CM_RSSI_MEASUREMENTS],
                                  uint16                         offset[MAX_CM_RSSI_MEASUREMENTS],
#ifdef  FEATURE_GSM_COEX
                                  mdsp_gprs_monitor_coex_params  coex_params[MAX_CM_RSSI_MEASUREMENTS],
#endif
                                  gas_id_t                       gas_id,
                                  void **                        cmdbuf_ptr);

boolean  mdsp_x2g_get_power_measure_results( uint32  rssi_values[MAX_CM_RSSI_MEASUREMENTS],
					                              boolean  yield[MAX_CM_RSSI_MEASUREMENTS],
                                             uint8   results_buffer,
                                             uint8*  num_ptr     /* Returns the number of measurements processed by GFW*/
#ifdef GERAN_L1_HLLL_LNASTATE 
                                             ,uint16      jdetvalues[MAX_CM_RSSI_MEASUREMENTS]
#endif /*GERAN_L1_HLLL_LNASTATE*/
                                             ,boolean skip_last_burst
                                             , gas_id_t gas_id
                                           );




#ifdef FEATURE_GSM_MDSP_GTOW
/* ---------- GTOW functions ---------- */
void mdsp_start_wcdma_rf_cmd(uint16 start_time, boolean  rf_tune2x, uint32 rxlm_irat_buf_idx , gas_id_t gas_id );
void mdsp_get_rf_off_script_offsets(uint16 *sbi_buffer, uint16 *grfc_buffer, gas_id_t gas_id );
void mdsp_prepare_wcdma_buffers(gas_id_t gas_id );
void mdsp_rf_sample_ram_cmd(uint16 start_time, uint16 stop_time, boolean  rf_tune2x, uint32 rxlm_gsm_buf_idx, uint32 rxlm_irat_buf_idx , gas_id_t gas_id );


void  mdsp_g2x_startup_cmd( uint16   time_offset,
                            boolean  rf_tune2x,
                            uint32   rxlm_irat_buf_idx
                            , gas_id_t gas_id );

void  mdsp_g2x_cleanup_cmd( uint16  time_offset, uint32 rxlm_buf_idx, boolean  tuneback_to_gsm,gas_id_t gas_id );


#endif


/* ---------- Support for dumping mdsp memory to flash ---------- */
boolean mdsp_memory_dump_requested (gas_id_t gas_id );
boolean mdsp_memory_dump_in_progress (gas_id_t gas_id );
void mdsp_memory_dump_to_file (gas_id_t gas_id );
void mdsp_force_memory_dump (gas_id_t gas_id );

/* ---------- Functions to support mdsp halts ------------------- */
typedef void (*mdsp_gsm_panic_cb_type)(gas_id_t gas_id);
void mdsp_gsm_register_panic_cb(mdsp_gsm_panic_cb_type cb , gas_id_t gas_id );
void mdsp_handle_panic(gas_id_t gas_id );

void mdsp_copy_xmsi_data (boolean imsi_valid,  uint8 *imsi_ptr,
                          boolean tmsi_valid,  uint8 *tmsi_ptr,
                          boolean ptmsi_valid, uint8 *ptmsi_ptr
                          , gas_id_t gas_id );

uint16 mdsp_sbd_read_pattern_used (gas_id_t gas_id );

void mdsp_reset_grfc_shadows(gas_id_t gas_id );
void mdsp_reset_pending_nonsbi_writes ( gas_id_t gas_id );

void mdsp_clear_gap_cmds(gas_id_t gas_id);

#ifndef FEATURE_GSM_RFLM_FED
void mdsp_update_gfw_dynamic_buffer( gas_id_t gas_id );
#endif

#ifdef FEATURE_GSM_MDSP_AEQ_8PSK
/*===========================================================================

FUNCTION mdsp_enable_aeq

DESCRIPTION
  This function enables or disables AEQ

===========================================================================*/
void mdsp_enable_aeq_8psk (boolean enable, gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_get_aeq_8psk_status

DESCRIPTION
  This function enables or disables AEQ

===========================================================================*/
boolean mdsp_get_aeq_8psk_status (gas_id_t gas_id );

#ifdef FEATURE_GSM_DYNAMIC_AEQ_8PSK
/*===========================================================================

FUNCTION mdsp_enable_aeq_dynamic

DESCRIPTION
  This function enables or disables AEQ

===========================================================================*/
void mdsp_enable_aeq_8psk_dynamic (boolean enable , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_get_aeq_8psk_dynamic_status

DESCRIPTION
  This function enables or disables AEQ

===========================================================================*/
boolean mdsp_get_aeq_8psk_dynamic_status (gas_id_t gas_id );

#endif  /* FEATURE_GSM_DYNAMIC_AEQ_8PSK */
#endif  /* FEATURE_GSM_MDSP_AEQ_8PSK */

/*===========================================================================

FUNCTION mdsp_is_desense_arfcn

DESCRIPTION
  This function retuns true if the input arfcn rquired Dc spur removal

============================================================================*/
mdsp_rf_desense_chan_type mdsp_is_desense_arfcn( uint16 chanel_num , sys_band_T  band ,gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_set_ftsm_geran()

DESCRIPTION
  This function sets the mDSP frame-tick setup margin.
  Used by LTE as well as internally.

===========================================================================*/
void mdsp_set_ftsm_geran (uint16 ftsm, gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_get_ftsm()

DESCRIPTION
  This function gets the mDSP frame-tick setup margin for external APIs

===========================================================================*/
uint16 mdsp_get_ftsm (gas_id_t gas_id);

/// The following are non-static functions from mdsp_intf.h
///
///
///
///


#ifndef FEATURE_GSM_RFLM_FED
/*===========================================================================

FUNCTION mdsp_get_sync_rx_offset

DESCRIPTION
  This function gets the sync rx time offset.

===========================================================================*/
uint16 mdsp_get_sync_rx_offset (gas_id_t gas_id );
#endif
/*===========================================================================

FUNCTION mdsp_gsm_sleep_cmd_done_isr

DESCRIPTION
  Interrupt handler for handling the response from firmware to the goto
  sleep command.  Does nothing, just installed to clear the interrupt.

===========================================================================*/
void mdsp_gsm_sleep_cmd_done_isr ( gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_set_iq_log

DESCRIPTION
  This function is called by gl1_hw.c to set fw flag for iq samples

============================================================================*/
void mdsp_set_iq_log_gas_id (  boolean value , gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_xo_desense_arfcn

DESCRIPTION
  This function called by gl1_hw.c to update arfcn list required Dc spur removal

===========================================================================*/
void mdsp_xo_desense_arfcn_gas_id (mdsp_xo_desense_arfcn_struct *pointer , gas_id_t gas_id );

/*===========================================================================

FUNCTION  mdsp_async_intf_send_cmd

DESCRIPTION
  Send MSGR message to GFW Task to run command processing immediately and execute the commands for the current frame.
  Use MSGR blocking wait to receive rsp for this immediate cmd from GFW before continuing

===========================================================================*/
void mdsp_async_intf_send_cmd(gas_id_t gas_id);

/*===========================================================================
FUNCTION  mdsp_async_intf_send_immediate_cmd
DESCRIPTION
  Send MSGR message to GFW Task to run command processing immediately and execute the commands for the current frame.
  Pass FN to which the offsets in the commands are referenced
===========================================================================*/
void mdsp_async_intf_send_immediate_cmd(uint32  cmd_fn, boolean rf_activity_cmds, gas_id_t gas_id);
/*===========================================================================
FUNCTION  mdsp_cmd_buffer_header
DESCRIPTION
  Add the num cmds and sequence number to the mdsp command buffer

===========================================================================*/
void mdsp_cmd_buffer_header( gas_id_t gas_id );
/*===========================================================================
FUNCTION mdsp_issue_tx_config_command_if_required
DESCRIPTION
  This function issues the Tx Config command if required
===========================================================================*/
void mdsp_issue_tx_config_command_if_required(gas_id_t gas_id);
/*===========================================================================
FUNCTION mdsp_get_tx_config_cmd_pending
DESCRIPTION
  This function gives the status of tx config cmd if pending
===========================================================================*/
boolean mdsp_get_tx_config_cmd_pending(gas_id_t gas_id);
/*===========================================================================
FUNCTION mdsp_is_mdsp_idle
DESCRIPTION
  Function called to check that the firmware is idle and not processing any
  commands
===========================================================================*/
void mdsp_is_mdsp_idle( gas_id_t gas_id);

/*===========================================================================
FUNCTION mdsp_inc_current_buffer_mod3

DESCRIPTION
  Increment current_buffer_mod3
===========================================================================*/
void mdsp_inc_current_buffer_mod3 (gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_frame_tick_process

DESCRIPTION
  Tick GPRS and DTM FT processes and tick frame count

===========================================================================*/
void mdsp_frame_tick_process (gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_load_nv

DESCRIPTION
   This function loads any NV items required by the mDSP driver.  For now
   there is only one.
===========================================================================*/
void mdsp_load_nv
(
   rex_tcb_type *task_ptr,
   void (*task_wait_handler)(rex_sigs_type,gas_id_t),
   rex_sigs_type task_wait_sig,
   gas_id_t gas_id
);

void mdsp_awake_set (boolean mdsp_awake_state,gas_id_t gas_id);
void mdsp_set_async_imm_cmd_sent(boolean async_imm_cmd_sent, gas_id_t gas_id);

/*===========================================================================

FUNCTION mdsp_get_fws_app_mask

DESCRIPTION
  This function return the appropriate firmware APP id for the given gas_id

DEPENDENCIES

===========================================================================*/
extern uint32 mdsp_get_fws_app_mask( gas_id_t gas_id );
/*===========================================================================

FUNCTION mdsp_process_cxm_logs

DESCRIPTION
  Logs the coexistence manager information returned by firmware

============================================================================*/
void mdsp_process_cxm_logs(gas_id_t gas_id);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION mdsp_get_log_cmd_entry

DESCRIPTION
   This function gets the mdsp command log entry at the specified offset.
   An offset of zero gives the most recent entry written to the buffer.

===========================================================================*/
void mdsp_get_log_cmd_entry( gl1_dump_tag_data_s *pBuf, uint8 len, gas_id_t gas_id);

#ifdef GFW_QSH_NOT_REQ_ON_JO
/*===========================================================================

FUNCTION mdsp_send_gfw_data_dump_req

DESCRIPTION
   This function populates dump address and invokes IPC to populate data.by GFW

===========================================================================*/
void mdsp_send_gfw_data_dump_req(uint8 *dump_data_ptr1,uint8 *dump_data_ptr2,uint8 *gfw_result);
#endif /*GFW_QSH_NOT_REQ_ON_JO*/
#endif /* FEATURE_QSH_DUMP */

#endif /* MDSP_INTF_H */
