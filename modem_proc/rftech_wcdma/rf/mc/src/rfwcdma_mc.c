/*!
   @file
   rfwcdma_mc.c

   @brief
   RF Driver's WCDMA interface file.

   @details
   This file contains all declarations and definitions necessary to use
   the WCDMA Main Control. The client for this file is RFM.
*/

/*===========================================================================
Copyright (c) 1999 - 2015 by QUALCOMM Technologies Inc.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/src/rfwcdma_mc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/16   sk      Initialize the MTPL every Tx Config
05/26/16   sub     Check for AsDiv support in RFC before building scripts
05/24/16   ag      Populate state machine rxlm buffer before msm static settings in prep tune
11/20/15   vs      FR 25072 - Dynamic enable/disable of one of two WTRs on 9x45 based on band needed
10/20/15   vs      Init RxLM/TxLM handles to 0xFF
10/15/15   vs      Changes to handle the race condition between DC<->SC reconfig and Tx events in FW thread
09/08/15   ak      Changes to put the GRFCs to init state in DRX sleep
09/07/15   vs      Fix DM memleak by releasing the issue seq Tx events at Tx decfg
05/27/15   zhh     FBRX ILPC optimize
04/03/15   zhh     update PA phase comp offset at the first Tx AGC update
03/13/15   dw      Fix sleep state update for the carrier index 
01/19/15   dw      Restoring asdiv position when building ASM script
12/16/14   vs      Pass SC as carrier mode to update RxLM during W2W meas
08/12/14   ag      Calling rfwcdma_asdiv_abort in sleep to abort the pending 
                   AsDiv events
12/03/14   ac      Skip update tune code if already under progress
10/07/14   kr      Update GRFC Tx scripts for W2G tune back scripts in tx mode.
11/25/14   sd      call update rxlm buffer with carrier mask param
12/02/14   ac      Workaround to ABORT all pending Snums during disable Tx
03/09/14   ag      Skip Hdet reading and temp comp in W2G gap
11/17/14   rmb     Added Dynamic spur mitigation feature related changes.
11/17/14   vbh     Update the call to txplim_get_mtpl_nv_item with band param
11/12/14   sd      Cleanup of deprecated sleep/wakeup routines
11/11/14   sd      Put GPIOs in LPM after all devices are disabled
11/03/14   vbh     Deallocate FBRx DM buffers for given FBRxLM handle before reset state machine
11/05/14   sd      Main line feature FEATURE_WCDMA_DC_HSUPA_FRAMEWORK
09/09/14   ak      Check for TQ IDLE before initiating a TQ release.
09/11/14   kr      Update rfwcdma_core_txplim_enabled flag properly in SC <-> DC. 
10/29/14   kai     Fix FBRx temp comp sequence issue during init Tx 
10/27/14   dw      Move MSM TxFE BW update from retune bw to init Tx
10/20/14   ac      CW fix
10/20/14   ac      fix oft errors
10/14/14   ac      xsw script to be appended to tune, featurised for now
10/13/14   vbh     Add support for NV container
10/09/14   kai     Reload LMEM in wakeup_tx
09/26/14   kai     Added support for WCDMA Rx V2 NVs
09/25/14   vbh     Fixed the AGC disble issue on all carriers during HO scenarios.
09/19/14   tks     Added support for db-dc spur mitigation
09/18/14   joz     put back some changes removed by Bolt
09/15/14   joz     replace FEATURE_DIMEPM_MODEM by FEATURE_JOLOKIA_MODEM
09/15/14   vbh     [1] Added device as a parameter to support Tx sharing
                   [2] Removed hardcoding on device 0
09/15/14   dw      WXE based Tx tune support for Single Tx
09/02/14   dw      Disable TxAGC during Tx bw change
08/28/14   vbh     [1] Call config ccs with the correct device
                   [2] Call release TQ pair only in exit mode
                   [3] Add time profiling in exit_mode
08/27/14   gh      Add check before configuring FBRx ILPC in init_tx and enable_tx
08/25/14   rmb     Fix KW Error and CW on Bolt.
08/25/14   dw      Fix warning
08/22/14   vbh     Update the doxygen documentation and parameter info to used the passed device
08/21/14   rmb     Add alternate support in the WCDMA MSM layer.
08/15/14   aro/gh  Do FBRX ILPC config capture in enable_tx
08/14/14   vbh     Fix return value in enable_tx    
08/12/14   vs      Set FR2001 flat by default to FALSE
08/11/14   rmb     Add alternate path support for W IRAT scenario. 
08/11/14   joz     added hardcoding wcdma_device_vote = TRUE for RUMI.
08/11/14   ak      Doing an Explicit PA ON from SW in retune as PA(QFGE2320) is OFF after config 
08/08/14   zhh     RF warmup profiling
08/07/14   kai     mutex the entire temp comp in decfg Tx              
08/05/14   vbh     Send AGC disable only if requested by WL1
08/06/14   dw      Disable TxAGC in Tx init when Tx BW change
08/04/14   rmb     Change in name in the usage of APIs.
08/01/14   dw      Initial support for single Tx DSDA
08/01/14   vbh     [1] Fix reset state machine sequnce in rx_sleep
                   [2] Added carrier mask to exec_tune to fix the state update
                   [3] Corrected messages and indentation 
07/21/14   vbh     SET and RESET mdsp flag indicating on going AGC config to avoid race condition
04/01/14   kr      Free DPD table in EPT_CFG
07/08/14   ak      Aborting the Tuner AOL operation as a part of Disable Tx
07/10/14   vbh     Fix DPM Warnings, add state machine update for all carriers in prep_tune
07/08/14   vbh     Reinitialize last_device with reset state machine
06/30/14   aro     Added support to handle FET+NBR+DSDA
06/30/14   vbh     [DR-DS] [1] Clean-up device 0 hardcode 
                   [2]Use MC state variable to reference primary Rx device
06/27/14   gh      Add check in sleep to see if device/path is already in sleep state
06/25/14   nv      Temporarily comment out script allocation for FBRX on WCDMA
                   DPM as it causing memory leak.
06/23/14   zhh     implement ASM sleep during retune or any ifreq/irat 
06/18/14   aa      DCHSUPA: DC config always in SC mode, update mdsp PDA during retune_bw
06/06/14   dw      Update alternate path info to RFC
06/17/14   aa      Release TQ pair in sleep only for device 0
06/12/14   aa      Get TQ pair in rx_wakup() for BOLT 2.0 call flow
06/05/14   dw      Put all ASM to sleep in primary device sleep
06/10/14   gh      Add the check on path info in rfm_wcdma_get_rxagc() for Diversity call.
06/06/14   ac      nbr support
06/04/14   sn      Enable ASM disable/sleep api
06/04/14   vws     Support script based interface rfwcdma_core_antenna_tuner_program_tx_mode 
06/04/14   vws     port change about tuner tx disable during Tx Disable 
06/04/14   vws     WCDMA IRAT tuner_disable in X2W
06/02/14   aa      compiler warning fix
05/28/14   rmb     Temporary fix to remove ASM disable tx from mc disable tx.
05/21/14   aa      TQ alloc/release/reserve in enter/exit and wakeup/sleep
05/15/14   aro     Added cmd processing module initialization
05/15/14   aro     Deprecated old command to perform MTPL update
05/19/14   ak      Updating the sleep_rx properly for early shutdown
05/15/15   ak      Fixed Compiler Warnings.
05/15/14   ak      Add parameter to sleep_rx API check for early shutdown
05/15/14   rmb     Added proper callflow of ASM/PA Sleep Wakeup.
05/12/14   sn      Backout PA/ASM sleep callflow in disable_tx
05/09/14   kai     Block Tx_decfg if temp comp not done
05/08/14   sn      Added PA sleep callflow
05/05/14   sn      Added ASM sleep wakeup callflow
05/05/14   vbh     KW warnings clean up
05/01/14   rmb     Add support for new version of AGC log packet 0x4176 v2.
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/24/14   kai     Skip temp comp if not in RxTx state
04/18/14   rmb     Add home meas enum info to config_band and config_chan APIs.
04/16/14   rmb     Fix KW Errors.
04/15/14   vbh     [FR2001] Clean LPM script building from prep_wakeup
04/15/14   vbh     [FR17754] Added support for script based tuning
04/12/14   zhh     WCDMA related Array boundary check
04/11/14   dw      Add Tx ON logging from RFLM
04/10/14   yb      Added support for coupler
04/09/14   dw      [1]Fix OFT error
                   [2]Fix Tuner CL Tx handle
04/04/14   vbh     Fix the FGAGC crash in registration due to AGC disable command
04/03/14   dw      Add dynamic RxLM update support
04/02/14   ac      fix compiler error for asdiv feature
03/28/14   vbh     Added prep/exec wakeup and sleep routines to support concurrent FR2001
03/26/14   kg      Added support for CL LM handle
03/25/14   kai     swap back order of tx band config and tx channel config
03/24/14   kai     Add ET DAC disble
03/21/14   aa      Read fbrx NV items during reload nv
03/25/14   dw      Add multi_carrier idx and TxLM handle in config mc
03/25/14   rmb     Add a new get rxagc API during X2W measurements.
03/20/14   ac      porting the asd changes from dime
03/18/14   vbh     Fix RF cal issues seen during W XO Cal
03/14/14   kai     swap order of tx band config and tx channel config
03/13/14   rmb     Pass the IRAT flag to config chan and band API.
03/13/14   zhh     check device ID in rfwcdma_mc_exec_wakeup_rx()
03/13/14   vbh     Update the FR2001 routines to support concurrent wakeup routines
03/11/14   dw      [1] DCHSUPA support
                   [2] New API for Tx retune BW
                   [3] Code cleanup, fix some hard-coding, magic number and other improper coding
                   [4] Mainlining DC feature
03/10/14   vbh     [1]Reinitialize the command action array before enable RxAGC in tune
                   [2]Add config band, config chan support in prep_wakeup
03/03/14   aa      Fix off-target error
03/03/14   dw      Remove AGC cmd from sleep_rx
02/27/14   aa      Update rfcommon_fbrx_mc_tune_to_chan
02/28/14   vbh     Crash fix to support DC calV4
02/27/14   rmb     Add fix to tune to channel for multi-carrier scenario.
02/26/14   vbh     Warning fixes
02/25/14   zhh     fix KW warning
02/25/14   vbh     Remove Err_fatal condition in wakeup APIs
02/24/14   rmb     Fix OFT Compilation Error.
02/24/14   aa      Added support for fbrx control via nv
02/24/14   aa      Added support for rsb cal
02/24/14   vbh     Add sleep_Rx api to keep consistent naming convention
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
02/19/14   ac      remove release taskQ for sleep(), do it only for exit()
02/18/14   vbh     DBDC- Update MC state for device 2/3
02/08/14   zhh     Add param validation to check whether the device id is valid or not
02/07/14   vbh     Wakeup callflow - fixes
02/04/14   aa      Fix DimePM llvm compiler error
02/04/14   vbh     Updated the wakeup callflow for PrepExec changes
01/30/14   aa      Update input params for rfcommon_fbrx_mc_init_tx
01/14/14   aa      Read all WCDMA specific FBRx NV's during bootup
01/09/14   vbh     Added a routine to return a copy of the MC state structure
01/08/14   kai     Remove ET chain config/deconfig
01/02/14   dw      [1]Bolt callflow cleanup
                   [2]Added support for run-time LNA switchpoint updates with FED
12/26/13   rp      Code changes to replace get critical section pointer with get common device
                   pointer for device comparison.
12/19/13   vbh     Init_tx timing optimization
12/14/13   tks     Updated calls to rfdevice rx tune api
12/13/13   tks     Updated calls to rfdevice tx tune api
12/10/dw   dw      Fix warning and remove bringup hack
12/04/13   dw      Consolidate all get RxAGC core functions. Access RxAGC value and status through single API
11/14/13   ka      Added fbrx tune to chan
11/13/13   ka      Added missing parameter to fbrx exit
11/13/13   aa      Add FBRx LM call flow for wcdma driver
10/28/13   dw      Move rxlm_buf_idx out of car_path_state in sm
11/08/13   ac      moving the ccs init to enter_mode() for ftm->ns crash
11/07/13   dw      Added back flag to enable fixed Tx power
10/31/13   ac      bringup flag for tx_on/Pa_on
10/29/13   dw      Disable temp comp for bolt bringup
10/26/13   shb     Updated IRAT APIs to support preload
10/23/13   kai     Add wrapper API config/deconfig rflm wcdma ccs taskQ
                   Removed rx_lna_setup device calling from pretune_to_chan
10/23/13   dw      Always return TRUE in init Tx for RUMI
10/20/13   dw      Fix warning
10/16/13   dw      Allocate/Release CCS TQ in sleep/wakeup
10/15/13   dw      Add rfwcdma_mdsp_exit()
10/10/13   kai     Add PA wake up before PA config
10/10/13   dw      Temporarily disable wakeup optimizatino for Bolt bring up
10/08/13   vs      Add API to get 3C RF tune status
09/24/13   dw      D3925 updates
09/20/13   dw      Turn on/off Tx and PA from MC directly for D3925
09/20/13   ac      hardcoding the lna state for the D3925 bringup
09/16/13   hm      hooking up WCDMA C++ device driver interface for WTR
09/17/13   vbh     Warning fixes
09/16/13   dw      Bolt update, initial changes for tx/rx config
09/13/13   kai     Add mdsp_init_once_flag
09/10/13   dw      Remove DBDC featurization and code clean up.
08/30/13   vs      Modifying MDSP interface to make common across DI/BO/TR
08/30/13   vs      Add rfwcdma_mc_get_rxagc
08/29/13   vs      Add RFM API to return RxAGC for all carriers
08/28/13   vs      Removal of DBDC featurization
08/27/13   dj      Added state variable use_fb_path_flag and the corresponding
                   get and set functions
08/23/13   vs      Added support for 3 carrier
08/20/13   dw      Remove FEATURE_BOLT_MODEM featurization
08/20/13   ac     add the declaration in the Qtuner feature for Triton
08/19/13   ac     compiler warning fix
08/19/13   ac     DBDC->SC fix
08/19/13  ska     Correcting the Tx init sequence to fix a Tx performance issue
08/17/13   kr      fix to pass proper parameters to rfwcdma_core_antenna_tuner_program_rx_mode()
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/14/13   ak      phone enter to "offline" with QRD RP4 RF card on TR1.1 CRM build
08/13/13   ac     dynrxlm crash due to rxlmbuf idx corruption in state machine
08/12/13   ac     compiler warning fix
08/12/13   dw/ac   DBDC fixes : sleep DBDC -> SC,enter_mode,lpm,dc->sc,tech_dependent_nv -> band
08/06/13   ac      crash due to invalid band being passed in
08/01/13   kr      replace memcpy with memscpy
08/05/13   vs      Modified RF interface to query DBDC bands supported
08/02/13   dw      Move RF bus Hal disable to rfwcdma_mc_sleep for device 0
08/01/13   st       Remove Un-used LM Includes
08/01/13   dw      Add support for L1 to get the W2W neighbor physical device index
07/31/13   aro     Added AGC log init
07/24/13   ac      compiler error fix for TR
07/24/13   ac      DBDC bringup changes
07/24/13   kai     Add temp comp data init
07/24/13   vs      Pass Closed loop NV info to Tuner
07/19/13   aa      Featurized mdsp init command for RXAGC for BOLT
07/26/13   vs      Add RF interface to query DBDC bands supported
07/16/13   aa      Multiple LNA transaction support
07/16/13   kcj     Return the result of NV read success for mc_init.
07/16/13   vws     idle timeline optimization: removed rfdevice_wcdma_tx_init()
                   from rfwcdma_mc_select_band_rx()
07/12/13   aro     Compiler Warning fix
07/09/13   kcj     Check that device init succeeded during rfwcdma_mc_init.
07/03/13   kcj     Dynamically allocated NV struct memory.
07/03/13   aa      Passed the buffer to tuner init to build the scripts
06/27/13   ac      Unified RF API to support query of RxAGC for SC and DC case
06/24/13   ac      tx freezeaccum api change for both car and txlm state for dc cfg
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
06/20/13   aa      Tuner init optimization
06/18/13   kai     Select Tx band before load the linearizer data
06/17/13   ry      Made changes to make Rx PLL settling time Device specific
06/14/13   kai     Added free DPD tables in deconfig Tx
06/14/13   aa      Updated Tuner init with correct device info
06/05/13   rmb     Fix to avoid loading linearizer data to MDSP pertaining
                   to the current Tx chan in FTM Cal Mode to resolve ET Cal Crash.
05/29/13   dw      Add HDET temp comp in the enable_tx
05/30/13   ac      DC HSUPA SC->DC crash fix
05/28/13   kai     Added DPD enable flag as setting WTR for Tx
05/23/13   ac      fix compiler warnings
05/22/13  ka/ac    dc hsupa changes
05/22/13   kcj     Enable multi-TX SAR limit
05/21/13   rmb     Added API to get Mutli synth lock.
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/20/13   rmb     Using Memcopy to copy over the structure in rfwcdma_mc_get_car_state_info_from_device().
05/13/13   jl      Port IQ capture code
05/10/13   bn      Support EPT_ET_CFG configuration
05/09/13   aa      support for W2W FTM IRAT with new FTM framework
05/09/13   jj      RF API to get UL frequency from DL UARFCN
05/07/13   sbm     Remove papm init(QPOET bypass)
05/06/13   tks     Added support for tracking rxlm buf idx in rfwcdma_mc
                   state machine
05/03/13   rmb/aa  Replaced rfwcdma_msm_update_rxlm_buffer
05/02/13   vb      Fix wrong tuner "SPI tuner availability" check in Wakeup
05/02/13   vb      Fix possible wrong band being used during WCDMA sleep
05/02/13   vb      When initializing the tuners for both Path-0 and Path-1
                   use the active band on Path-0
05/02/13   aa      replaced rfwcdma_msm_update_rxlm_buffer
05/01/13   rmb     Added enable_qtf_tuner_support for AOL feature and rfwcdma_mc_get_car_state_info_from_device()
                   API to get state information for a particular device and removed hard coded extra warmup time in wakeup().
04/29/13   jj      Add support to get default band
04/26/13   jj      fix KW errors
04/23/13   ka      Initial DBDC support
04/17/13   sma     Fix false alarm due to incorrect parameter call
04/13/13   gh      Localize rf_buffer usage
04/11/13   dw      Initialize power tracker to APT mode in enable Tx
04/10/13   ac      removing the modulator setting function call for dc hsupa
04/08/13   sma     Fix memory leak in rfwcdma_mc_enter_mode and rfwcdma_mc_exit_mode
04/05/13   aa      Fixed the wake up sequence
04/04/13   ka      WCDMA state machine updates
04/05/13   rmb     Added rfdevice_wcdma_asm_config_rx() in RF Wakeup and rfwcdma_core_rxctl_freq_comp(),
                   rfwcdma_mdsp_update_rx_events() in pretune_to_chan() for having good RxAGC in Inter Freq Idle.
04/03/13   vb      Disable antenna tuners part of WCDMA sleep
04/03/13   kcj     Added rfwcdma_mc_wakeup_tx.
04/02/13   ac      off target compiler error fix
04/01/13   ac      avoid race condition for multiple interrupts and clean up of script type for static events
03/26/13   jyu     Added the change to allow override trigger type
03/21/13   kai     Removed asm_config in retune
03/13/13   jj      add empty function rfwcdma_mc_set_multi_tx_sar_limit()
03/12/13   rmb     Used rxAgcEnable flag in rfwcdma_mc_tune_to_multi_chan() and rfwcdma_mc_execute_rxlm()
                   when Start/Stop AGC command is sent from SW to FW.
03/12/13   kai     Put disable ET into rfwcdma_mc_decfg_tx
03/11/13   kcj     Utilize PAPM enable/disable when enabling/disabling TX.
03/08/13   ac      event script and init cmd cleanup
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/06/13   kai     Added rfwcdma_mc_decfg_tx
03/05/13   ka      Update state machine during pretune
03/05/13   kai     Put disable ET into disable_tx
03/04/13   aa      Pass the right band info in rfwcdma_mc_power_clock_on
03/01/13   kai     [1]Removed rfwcdma_mc_enable_et
                   [2]Added calling ET relative functions from rfwcdma_core_xpt
02/19/13   ac      fix OFT compile error
02/17/13   dw      Fix memory leak in rfwcdma_mc_script_buffer_ptr when diversity is enabled
02/14/13   vb      Deleted the Nikel implemenation of rfwcdma_mc_set_tuner_tune_code_override()
02/14/13   vb      Removed support for old antenna tuner scenario programming
02/15/13   aka     Reverting previous check-in for WCDMA AGC cmd check
02/14/13   ac      common api for hdet,feature cleanup
02/14/13   vb      Compilation fix for Triton
02/14/13   aka     Added check for AGC cmd
02/14/13   vb      Support for Tuner disable
02/13/13   vb      Support for new tuner manager in WCDMA
02/07/13   ndb/vb  Added rfwcdma_mc_set_tuner_tune_code_override() implementation support
02/07/13   rmb     Fix Compiler Warning.
02/07/13   rmb     Fix Compiler Errors on Dime 1.1, TR.DSDA and Triton 1.0 related to Antenna Tuner.
02/06/13   rmb     Added optimized wakeup function to send SSBI writes as a script to FW.
02/06/13   ac      HS rxlm state support
02/06/13   dw      Add inclusion of rfcommon_msg.h
01/31/13   aa      W FTM IRAT support
01/28/13   kai     Fix rfwcdma_mc_state initial error
01/28/13   kai     Removed rfwcdma_mc_set_sapt_state
01/21/13   rmb     Using rfwcdma_mdsp_get_rxagc_enable_status() for RF Wakeup/Sleep.
01/18/13   ac      removing the QTUNER feature for hdet event init
01/14/12   tks     Changed the sleep function to call the disable interface
                   of transceiver driver
01/08/12   dw      Fix crash in RxD only call
12/19/12   ka      Corrected multi_carrier_index in rfwcdma_mc_tune_to_multi_chan()
12/12/12   dw      Remove the workaround for stop all AGC cmomand in the beginning of tune
12/12/12   ac      pa_on/off update to go through helper
12/11/12   dw      Add support for DTx and DRx/LPM.
12/11/12   ac      pa_ range event builder support, pa q current as a txagc task
12/03/12   dw      Fix L2W redirection wakeup failure
11/20/12   aa      Send RF init command for RX_AGC_MEAS upon wakeup
11/16/12    vb     Tuner init and Tuner rx mode at the same time
11/16/12   aa      Enable RX AGC Measurement init command for IRAT
11/16/12   ac      hdet event builder function
11/15/12   ka      Initial support for DCHSUPA
11/15/12   kai     Added disable ET and config ET TXLM command to MDSP
11/14/12   dw      Add function to get RF buffer pointer from MC
11/09/12   kai     Added enable ET
11/02/12   dw      [1] Add support for Dual Carrier on Dime
                   [2] Remove RxAGC enable flag
10/17/12   vb      Creating tuner managers during MC init
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable
11/01/12   ac      tx_on/off event builder
10/31/12   aa      Backing out the RX AGC MEAS event
10/10/12   kai     Changed interface rfwcdma_core_antenna_program_scenario_standalone
09/21/12   kai     Added new antenna_tuner support, removed old one
10/30/12   ac      pa_on/off event builder function
10/29/12   aa      Issue init RF command to FW for RX AGC MEAS event
10/25/12   dw      Vote to disable/enable RFCMD app in sleep/wakeup
10/19/12   dw      Sleep bringup and code clean up
10/17/12   dw      Fixed bug that turns on PA in disable Tx.
10/11/12   dw      SVVZ bringup updates
10/8/12    ac      drx crash fix during rf cal
10/05/12   aa      Removed obsolete wtow interface
10/04/12   sr      Added IRAT APIs.
10/2/12    ac      fix for the wcdma crash in online mode, disabling the bringup flag to set the vregs
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
09/22/12   ac      temp workaround for the rfwcdma_mc_init_tx crash
09/21/12   ka      Enable tx agc by default
09/20/12   ka      Updated start agc for FTM
09/19/12   ka      Check status before sending AGC commands
09/19/12   ka      Add PA and ASM to select tx band
09/19/12   vbh     Added request for RFCMD app in enter and exit mode
09/18/12   ac      rfc to take device as parameter
09/17/12   ka      Temporarily set constant tx power
09/17/12   vbh     Enable time profiling
09/12/12   ac      diversity rxagc bringup
09/12/12   ka      Added tx mem init calls
09/10/12   aa      Updated interface to rxlm buffer to logcial device
09/07/12   vbh     Fixed compiler warnings
09/06/12   tks     Fixed compiler warnings
09/05/12   ka      Restore tx lin init
09/05/12   vbh     Fix on target compiler warnings
08/31/12   ac      fixed script and ran rfcal successfully
08/28/12   tks     Added PA ON events in enable tx
08/27/12   tks     Added PA OFF events in disable tx
08/27/12   tks     Added ASM config event in select band rx
08/27/12   tks     Added support for Rx AGC init event in select band rx
08/24/12   ac      wait till commit is done before the start of next
08/21/12   ac      1st tune issue fix
08/18/12   tks     Converted diversity related device calls from write
                   immediate to prepare script
08/17/12   tks     Removed pre-silicon by pass flag
08/16/12   tks     Added tx enter mode and rx enter mode device calls to be part
                   of the respective tune events
08/16/12   tks     Added rx_agc_enable flag to mc state variable
08/16/12   aa      Move setting current band state machine to invalid from end
                   of enter mode to the beginning of enter mode.
08/12/12   vbh     updated the calls to ASM and PA with WCDMA interfaces
08/07/12   ac      rxagc_init change
07/29/12   tks     Fix compile errors in rfwcdma_mc_init
07/29/12   ac      for now commenting the mdsp_sync_send_rf_cfg_cmd until wl1
                   checks in the changes in mdspsync.c/.h files
07/29/12   ac      wcdma rumi bringup changes
07/21/12   tks     Call flow updates for PA & ASM
07/20/12   tks     Call flow updates
07/19/12   ac      enum updates to be compatible with the fw
07/17/12   ka      Updates to event building
07/11/12   ka      call flow updates
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type
                   instead of void ptr
07/10/12   Saul    WCDMA. Added tune_with_lte_settings API.
07/06/12   ka      Event call flow updates
07/05/12   tks     Modified exisiting call flow to accommodate ASM & PA
07/02/12   ac      static event script writes for wtr device
07/17/12   shb     Added rfc_wcdma_init() call in rfwcdma_mc_init()
07/10/12   rmb     Removed critical section enter/exit for wakeup/sleep function,
                   moved common and wcdma wakeup just after switching on power
                   rails and made sure only power rail on/off is present
                   in power clock on/off.
07/09/12   aak     Move Quiet mode API from device to RFC
07/02/12   sn      PA/ASM device interface and scripting support
06/26/12   dw      Update rfwcdma_mc_path_car_state in pretune function
06/21/12   vb      Added support for QFE1510
06/13/12   swb     Add rxlm buffer index to rfm_pretune_to_chan function
06/12/12   cri     Fixed remaining issues for W EPT online mode support
06/12/12   kai     Added critical section lock for wakeup and sleep
06/07/12   shb     Added DPD support in WCDMA TX tune API - disabled for now
05/24/12   kai     Bypass Wakeup from Non-Sleep state
05/15/12   jl/dyc  Omit vote for DRx vreg.  Shared with PRx
05/08/12   tks     Call CPC DRx & CPC DTx SBI update in select_band function
05/08/12   kai     Add API for CPC register setting and check the CPC register
                   before issue Stop-AGC to FW
05/07/12   dw      Put all GPIO to input state in sleep
04/17/12   kai     Move update_txlm_buffer from enable_tx to init_tx,
                   remove txlm_buf_idx from disable_tx
04/13/12   kai     Revert the following update_txlm_buffer change since gsm
                   layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx,
                   remove txlm_buf_idx from disable_tx
03/30/12   ems     For tune_to_multi_chan only transition to RX if not in RXTX
03/01/12   dw      HS APT support
03/27/12   dw      Fix max power issue that NV limit was not initalized
                   properly until temp comp kicks in
03/20/12   kai     remove tx_init from rfwcdma_mc_wakeup
02/27/12   dw      Stop Chain 0 AGC in sleep if it's still on
03/13/12   dw      Check AGC status of chain 1 before send agc1 cmd in sleep
03/22/12   ac      adding changes for msgr for wcdma
03/27/12   jfc     Remove pm_paon_cntrl.h / temporarily revert the MSGR changes
03/17/12   jfc     Remove clkrgm* header files
03/07/12   cri     Added initial DPD support
03/01/12   tks     Fix W2G issues caused due to missing vote in sleep function
02/27/12   swb     Return indication of missing cal data within init_tx function
02/27/12   dw      Stop Chain 0 AGC in sleep if it's still on
02/22/12   swb     Inter-freq tune optimization: add rfwcdma_mc_pretune_to_chan
                                                                                function
02/22/12   swb     Mainline FEATURE_WCDMA_RXD_PCH in wakeup function
02/21/12   av/jl   Added turning on the tcxo buffer during rfm_enter_mode().
                                                                                With GP_CLK removal, rfm_power_clock_on/off() functions are
                                                                                obsolete, which take care of tcxo buffer management.
02/17/12   swb     Removed redundant tx enter mode calls
02/16/12   kai     Added API for return PA state in func rfwcdma_mc_cmd_dispatch
02/07/12   tks     Add support RF device manager voting for power up
                                                                                and power down
01/24/11   dw      Only configure DC related SBIs in SC<->DC transitions.
12/20/11   sn      Added support for QFE1320
12/13/11   aak     Path 1 turned off in power clock off ()
12/08/11   ac      rxd idle feature
12/01/11   dw      Add support for WCDMA low power mode (DSR)
11/30/11   aak     Remove call to turn on vregs for path 1 in WCDMA
11/22/11   vb      Bug fix in programming RFC signals during WCDMA Wakeup
11/17/11   aak/dw  Changes to enable Quiet mode
11/11/11   dw      Remove GP clock programming and deprecate API
11/09/11   ac      tx band config added in tx enable as a part of handover fix
11/04/11   gh      Add ant_tuner state check. Send shutdown CW only
                                                                                when sleeping PATH0
11/03/11   ac      rfc tx_disable
10/31/11   ac      rfc wakeup
10/26/11   dw      Support for closed loop AGC in FTM
10/25/11   ac/ka   Update temp comp after reload RF NV
10/13/11   gh      Add additional checks for ant_tuner
10/13/11   swb     Added API for freezing Rx AGC accumulator
10/05/11   ac/sr      rfc sleep
09/30/11   dw      Temp comp updates
09/28/11   dw      Swap path in MC command dispatch for RxD only mode
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and
                                                                                FEATURE_RF_WCDMA_LM_RFM_INTERFACE.
09/25/11   vb      Fix p4 Merge issue
09/25/11   vb      Compiler error fix
09/25/11   vb      Support for DAC cal using different Irefs
09/20/11   ac      dual carrier drx rf cal fix
09/19/11   dw      clean up rfwcdma_core_txctl_set_tx_modulator()
09/16/11   dw      In sleep, do not stop chain 1 AGC in RxD only mode
09/08/11   gh      Add Ant Tuner support
09/08/11   ac      SAR support
09/06/11   dw      APT cleanup
08/25/11   sar     Removed obsolete file bbrx.h.
08/24/11   dw      Added Support for polling based AGC cmd response handling
08/18/11   vb      Added support for splitting rfwcdma_mc_enable_tx &
                                                                                added rfwcdma_mc_init_tx
08/18/11   dw      Fix missing break in the MC command dispatch
08/13/11   vb      Need to call rfwcdma_core_txlin_load_nv_data even when
                                                                                retriving of some non critical NVs fails
08/11/11   vb      Support for new multi-chan linearizer/APT
08/08/11   dw      RxD only mode support on Nikel and optimze some callflows
08/08/11   dw      Temporary back out the AGC settling time change for wakeup
08/02/11   ckl     Remove RFWCDMA_MC_GET_CGAGC_SETTLING_TIME from
                                                                                command dispatch
08/02/11   dw      Add support to populate multi-linearizer data strucures
07/18/11   dw      legacy feature cleanup
                                                                                handle return status of lm buffer updates
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new
                                                                                auto-gen of rf card files
06/29/11   vb      Updates based on the new RFC AG design
06/23/11   dw      Optimize dual carrier enable callflow
06/01/11   dw      Stop AGC only for path 0
05/28/11   dw      Enable stop AGC
05/23/11   dw      Remove rfwcdmalib_msm.h
05/20/10   dw      Remove rfwcdmalib_msm_init_config()
05/10/11   dw      Temporary change not to stop AGC when tuning
05/10/11   sty     Work-around for 8960 bringup
05/05/11   dw      Add RFWCDMA_MC_GET_LO_FREQ_FROM_UARFCN
05/05/11   dw      Remove some obsolete function calls.
04/20/11   jhe     band type correction
04/18/11   dw      KW fixes
04/12/11   dw      dynamic RxLM updates
04/10/11   dw      Power management support for Nikel
04/01/10   dw      Fix WCDMA band type mismatch in tune device API
03/25/11   dw      Remove BBRx access for Nikel
03/22/11   dw      RUMI updates
03/22/11   dw      cleanup common RFC interface access and other obsolete
                                                                                functions
03/09/11   sty     Changed return value in rfwcdma_mc_enter_mode
03/09/11   sar     Replaced clk.h with DALSys.h and updated clk_busy_wait api.
03/08/11   pl      Update mock_fw_configure_chain()
03/08/11   dw      Rename rfwcdma_msm_enable_tx_agc()
03/04/11   dw      Do not implicitly tune RxD based on internal state machine
03/11/11   dw      Nikel updates
03/01/11   dw      Fix warning
02/25/11   dw/ka   Merge support for thermal mitigation
02/23/10   dw/ka   Changes to enable RxD by NV_RF_BC_CONFIG_DIV_I
02/23/11   dw      Fix compiler warnings
02/22/11   dw      Include RxLM buffer index in AGC cmd
02/09/11   dw      MC callflow cleanup, add support for AGC cmd control,
                                                                                add support for new RF CB event
01/27/11   dw      WCDMA RFC cleanup
01/31/11   dw      Featurization for Genesis LM code for temporary mock-fw
                                                                                code access.
01/28/11   dw      Initial TxLM support for WCDMA
01/31/10   dw      Linker error fix for 8960
01/26/10   dw      updates for 8960 build
01/05/10   dw      Initial support for RxLM
01/18/11   pl      Remove obsolete interfaces
01/04/11   pl      Fix compile error -- use rfm_path_enum_type
11/17/10   kguo    Populated fix for CR 264265 to main\latest
11/05/10   dw      Program band-dependent default RxF setting in enter mode
11/05/10   kguo    Updated some function calls with band parameter to be in sync
                                                                                with function prototype changes
10/29/10   ka      Updates for initial WCDMA APT functionality
10/29/10   kguo    Fixed retval of rfwcdma_mc_disable_tx to the right value for
                                                                                success case
10/26/10   ap      Turn off Vregs for Rx_Div case in rfm_power_clock_off()
10/19/10   vb      All device functions are now accessed thru device interface
                                                                                functions
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/04/10   ka      Reset HDET loop to acq mode after handover to
                                                                                speed convergence
09/28/10   ap      Added GP_CLK enable/disable API
09/22/10   dw      move rfwcdma_core_rxctl_init_cgagc_params to rfwcdma_mc_init
09/17/10   av      GPS notc filter update
09/02/10   ka      Complete support for common GRFC architecture
08/31/10   ac      low band drx rxfe fix
08/18/10   ac      lna phase offset support for dual carrier and sc
08/19/10   ka      Port over exit mode fixes
08/18/10   dw      Enable BBRx sleep
08/17/10   ka      Back out hdet change
08/12/10   dw      Workaround for the issue that RxF sample clock not turned
                                                                                back on after power collapse
08/11/10   ka      Update tx temp comp when enabling tx
08/09/10   dw      Add channel specific RxFE programming
08/05/10   ka      Add missing parameter
08/04/10   ac      Improve exp_hdet_vs_agc interpolation to reduce overall
                                                                                enable_tx execution time
07/19/10   aro     Fixed BBRx Bug
07/30/10   dw      Change the function prototype of
                                                                                rfwcdma_mc_force_tune_to_multi_chan
07/30/10   dw      Add rfwcdma_mc_tune_to_multi_chan_noband_check() also add
                                                                                GRFC programming in wakeup
07/28/10   ac      compiler warning fix
07/26/10   ap      Added back power collapse debug variable
07/26/10   dw      reset RxFE FIFO for every tune to channel
07/23/10   ka      Change dual carrier interface to accept channel
07/23/10   ap      Added support for rfwcdma_power_clock_on/off
07/20/10   ka      Added API to determine if dual carrier is supported
07/20/10   ap      Provided support for Vreg management during Power Collapse
07/19/10   ka      Removed warnings
06/30/10   dw      Fix for DC->FACH AGC issue
06/11/10   ac      B1 DC Low channel fix and freq comp fix for DC
06/10/10   dw      initialize Tx in wakeup for PRx only
06/10/10   kguo    Added utility function for ltow flag
06/08/10   lcl     Added an API to get the carrier type
06/08/10   ka      Klocwork fixes.
06/01/10   dw      Fix wrong function call in get tx agc
05/25/10   dw      changes to support MDM9k Div only mode call
05/21/10   dw      Sleep support for WCDMA Dual Carrier
05/20/10   tws     Add API to read/set gtow flag.
05/13/10   dw      Only init PA control in PRx enter mode
05/12/10   dw      Support for Dual Carrier to Single Carrier transition
05/10/10   dw      Added Dual Carrier state machine check in
                                                                                rfwcdma_mc_write_tech_dependent_nv_items_mdsp
05/06/10   ka      Enable HDET and temp comp for MDM9K.
05/05/10   ka      Do not send start agc command during gtow
04/23/10   ad      Fix compiler errors in WPLT build
04/16/10   kguo    Called msm_config_multimode in enter_mode to fix
                                                                                L->W rxagc problem
03/24/10   ka      Added support for DC wtow.
03/19/10   lcl     Swap the order of argument 1 and 2 for rfwcdma_mc_init
03/18/10   kguo    Recover changes on 3/17/10
03/18/10   kguo    Fixed compiler error
03/17/10   ad      Fix RxAGC logs on diversity chain
03/16/10   kguo    Merged from scmm_final_merge branch
03/08/10   ka      Restore wtow
02/26/10   ad      Fix missing RxAGC logs on diversity chain
02/25/10   ka      Temporarily remove wtow
02/18/10   ad      Support for SC-DC transition for both antennas
02/18/10   ka      Fix Tx PLL tuning
02/18/10   aak     Eliminate compile error from UMTS-only build
02/16/10   ad      Fix B5 merge changes
02/11/10   ac      merge the dual carrier changes
02/10/12   vb      Klockwork error fixes
02/08/10    vb     Removed Vreg turn-off from the function rfwcdma_mc_sleep()
02/03/10   kma     Fixed compilation issue for CDMA only build
02/02/10    vb     Added missing command dispatch functions
02/02/10   kma     Added WCDMA specific power collapse functions
02/01/10   kguo    Called init_pa_ctl after mdsp is initialized.
01/29/10   ad      Merge SC-DRX, DC-DRX changes from mdm9k dev branch
29/01/10   sr      Changed the <tech>_mc_init() signature to fix warnings in
                                                                                <tech> specific builds
01/28/10   kma     Added WCDMA specific power collapse functions
01/21/10   bn      Lint Fixes
01/21/10   bn      Removed unused rfwcdma_core_txctl_set_pa function
01/19/10   kguo    Fixed compiler error in tune_to_muitl_chan for merged build
01/18/10   ckl     Removed med level lint error.
01/18/10   ckl     Fixed BBRx power control during sleep and wakeup.
01/08/10   ckl     Removed high level lint error.
01/06/10   kguo    Merged from mdm9k dev branch to main\latest
12/23/09   ckl     Fixed PCH SLEEP issue for diversity only call
12/23/09   lp      Removed XO_OUT buffer support
12/20/09   bn      Fixed compiler warning.
12/19/09   lp      Added XO_OUT buffer support
12/09/09    bn     Adding rfwcdma_msm_set_web_misc()
12/08/09   dyc     Setup PMIC TCXO buffer prior to transmit
12/08/09    bn     Separate Tx and Rx PLL for current savings
12/02/09   ckl     Added W2W function dispatch.
12/01/09   ckl     Return always TRUE for rfwcdma_mc_init( ).
12/1/09    ac      dual carrier in ftm which was broken in plf7
11/18/09   ad      Move RFD matrix config for PA_ONx to HAL
11/11/09   ka      Updates for dual carrier.
11/30/09   ckl     Added rfm_get_freq_from_dl_arfcn( ) for W2W measurement
11/30/09   bmg     Added common_init_successful parameter to initialization
                                                                                Skip initialization if the common NV could not be loaded
                                                                                Changed init function return to boolean
11/18/09   dyc     Use correct PMIC switcher PM_VREG_RF2_ID vs PM_VREG_RF_ID
11/18/09   ad      Move RFD matrix config for PA_ONx to HAL
11/11/09   ka      Updates for dual carrier.
11/05/09   vb      Fixed the bug in rfwcdma_mc_wakeup(); rtr8600 init function
                                                                                should not be called when PRx is active
10/29/09   kguo    Reverted a feature to compile
10/29/09   kguo    Featured code that has dependency on L1
10/23/09   kguo    Merged changes for 9k bringup
10/22/09   ckl     Fixed missing Tx power on diversity only call
10/16/09   jhe     _sleep/_wakeup changes
10/01/09   ckl     Removed all warnings and extern function definitions
10/01/09   ckl     Added code to check for BBRX freeze and send F3 msg
10/01/09    bn     One-time HDET autocalibration
09/30/09   ckl     Support Diversity only call
09/22/09   dyc     Support for SMPS freq setting + some warnings cleanup.
09/17/09   ckl     Moved Tx functions to core_txctl file.
09/17/09   sar     Removed deprecated file, adie.h for CMI conformance.
09/15/09   jhe     Initial W sleep enable
08/24/09   ckl     Changed enter mode operation for diversity path.
08/20/09   ckl     Initialize primary and secondary BBRx setting at the
                                                                                same time
08/19/09    ka     Added additional header file
08/14/09    ka     Added include file required for MDM9x00
08/13/09   jhe     freq comp change
08/13/09   ckl     Support diversity and callback functions.
08/11/09   bn      Added get HDET threshold API
08/11/09   bn      Enabled Temp Comp API
07/08/09   av      Removed configuring PMIC PA Controller in MC. Moved it to RFC
07/07/09   bn      TxAGC dB10 changes
06/30/09   ckl     Replace temporary BBRx init funtion with bbrx_set_mode.
06/11/09   ckl     Call BBRx initialization in mc_enter_mode.
06/04/09   lp      Changed pm_paon_cntrl.h to pm.h
06/04/09   ckl     Support rfm_get_freq_from_uarfcn, rfm_convert_chan_to_band,
                                                                                rfm_get_hdet_tx_agc_value.
06/01/09   ckl     Fixed Rx AGC frozen issue when L1 stops and starts acquisition.
05/28/09   ckl     Modified to have one return statement in rfwcdma_mc_cmd_dispatch( )
05/28/09   ckl     Add PMIC PA_ON control initialization.
05/27/09   bn      Correct ARM register settings
05/19/09   bn      Bringup hack - Setting WCDMA mode register to retain PLL lock status
05/14/09   ckl     Added rfwcdma_nv_retrive_items() in rfwcdma_mc_init()
03/23/09   ckl     Cleanup Feature SUPPORT_MDM8200_PALTFORM
03/22/09   ckl     Added new rfm functions, rfwcdma_enable_rx, rfwcdma_disable_rx,
                                                                                rfwcdma_mc_ioctl, and rfwcdma_mc_init
03/20/09   ckl     Change rf_get_rf_hw_config() to rfcommon_get_rf_hw_config()
03/19/09   ckl     Feature SUPPORT_MDM8200_PALTFORM to support Poseidon Modem
03/18/09   ckl     Fix duplicate enum names between rfwcdma_mc_state_enum_type
                                                                                and rf_state_enum_type
03/17/09   ckl     Include clkrgm_msm.h instead of clkrgm_modem.h for SCMM build
03/15/09   ckl     Include rfnv_wcdma.h ahead of other wcdma core header files
03/10/09   ckl     Use new common function name, rfcommon_core_device_to_path
03/09/09   ckl     Code cleanup
01/30/09   av      Need to have AGC in tune_to_chan. Or has trouble acq. Needs
                                                                                further investigation
01/30/09   av      Changes and 1st round of cleanup for WCDMA support
10/14/08   sr      Initial version of wcdma Main Control.

============================================================================*/

#include "rxlm_intf.h"
#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "rfwcdma_mc.h"

/*! @cond vbhide */
#include "rfcommon_core.h"
#include "rfcommon_core_cb.h"
#include "rfcommon_nv_mm.h"
#include "rfcommon_nv.h"
#include "rfmeas_mc.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_core.h"
#include "rfwcdma_core_rxctl.h"
#include "rfwcdma_core_temp_comp.h"
#include "rfwcdma_core_txctl.h"
#include "rfwcdma_core_txlin.h"
#include "rfwcdma_core_txplim.h"
#include "rfwcdma_core_util.h"
#include "rfdevice_wcdma_intf.h"
#include "rfc_card.h"
#include "rfc_card_wcdma.h"
#include "rfumts.h"
#include "ftm.h"
#include "rfwcdma_mdsp.h"
#include "wl1api.h"
#include "rfwcdma_msm.h"
#include "pm.h"
#include "DALSys.h"
#include "rfcommon_core_device_manager.h"
#include "timetick.h"
#include "rf_hal_common.h"
#include "rfnv_wcdma.h"
#include "rfm_types.h"
#include "rfm_common_sar.h"
#include "rfm_internal.h"
#include "rfdevice_wcdma_asm_intf.h"
#include "rfdevice_wcdma_pa_intf.h"
#include "rfdevice_wcdma_coupler_intf.h"
#include "rf_hal_bus_types.h"
#include "rf_buffer_intf.h"
#include "rf_hal_buffer.h"
#include "rfm_internal.h" // For RUMI Bring up
#include "ftm_wcdma_ctl.h"
#include "rfdevice_papm.h"
#include "rfdevice_papm_intf.h"
#include "wfw_sw_intf.h"
#include "rfmeas_mc.h"
#include "rfwcdma_mc_meas.h"
#include "rfcommon_msg.h"
#include "rfmeas_mdsp.h"
#include "rfc_card_meas.h"
#include "rfwcdma_core_util.h"
#include "rfwcdma_core_log.h"
#include "rfdevice_trx_wcdma_rx.h"
#include "rfwcdma_mdsp_async.h"
#include "rfwcdma_mdsp_sync.h"
#include "rfwcdma_mdsp_data.h"
#include "rflm_dm_api.h"
#include "rfcommon_fbrx_api.h"
#include "rfwcdma_data.h"
#include "rfcommon_math.h"
#include "rfwcdma_cmd_proc.h"
#include "rfcommon_time_profile.h"
#include "rfc_wcdma_data.h"
#include "rfwcdma_core_auto_pin.h"


#ifdef FEATURE_RF_ASDIV  
#include "rfwcdma_core_asdiv.h"
#endif
#include "rfdevice_wcdma_type_defs.h"

#include "rfcommon_core_xpt.h"
#include "rfcommon_fbrx_ilpc.h"
#include "rfcommon_core_utils.h"
#include "rfcommon_nv_mm.h"
#include "rfcommon_autopin_api.h"


#ifdef FEATURE_RF_HAS_QTUNER
#include "rfwcdma_core_antenna_tuner.h"
#endif

#include "rfdevice_hdet_wcdma_intf.h"
#include "rfwcdma_core_xpt.h"
/* memscpy */
#include "stringl.h"



/*----------------------------------------------------------------------------*/
/*                          Global Declaration                                */
/*----------------------------------------------------------------------------*/

/* 200 us delay is based on the RF1/RF2 rise time with safe margin */
#define RFCOMMON_MC_RF_VREG_DELAY_TIME 200

extern WfwSwIntfStruct *rf_fwsw_intf_addr_ptr;
rf_wcdma_bup_flags_t rf_wcdma_debug_flags = { 0 };  // Struct that holds wcdma debug flags

extern void rfwcdma_core_txctl_init_pa_ctl(void);
extern void rfwcdma_mdsp_init_agc_ctl(void);
extern void rfwcdma_core_update_min_power_from_nv(void);
extern void rfwcdma_mdsp_set_pa(boolean state, rfcom_wcdma_band_type band);

extern rfcommon_nv_status_type rfwcdma_nv_retrive_items
(
  rex_tcb_type *task_ptr,
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)(rex_sigs_type)
);

extern boolean rf_disable_powercollapse;

#ifdef FEATURE_RF_HAS_QTUNER
extern boolean enable_qtf_tuner_support;
#endif

extern const rf_hal_bus_resource_script_type* rfwcdma_core_get_resource_script_from_buffer
(
  rf_buffer_intf *buffer_ptr
);

/*! @todo (tks) PA & Device object should be obtained from RFC */
void *pa_device;
void *asm_device;

/* flag to indicate whether wakeup optimization should be performed*/
boolean enable_wakeup_optimization_flag = FALSE;

/* MC State variable to hold the state, band on each path.
                Also stores rx and tx frequency. And diversity only call
                in that order*/
rfwcdma_mc_state_type rfwcdma_mc_state;

rfcom_band_type_u tx_band;
rfcom_band_type_u rx_band;

/* The following structure is obsolete. */
/* Retained so components shared with old targets can compile */
rfwcdma_mc_path_car_state_type rfwcdma_mc_path_car_state =
{
  RF_PATH_0,
  RFCOM_SINGLE_CARRIER,
  { 10700, 10700 },
};

rfwcdma_mc_rxlm_buf_type rfwcdma_mc_rxlm_buf[2] = { { 0, FALSE }, { 0, FALSE } };

static boolean rfwcdma_gtow_flag = FALSE;
static boolean rfwcdma_ltow_flag = FALSE;
static boolean ftm_irat_testing = FALSE;

static boolean rfwcdma_mc_enable_time_profiling = TRUE;

/*for bringup added this flag for immediate and script based write*/
boolean tx_on_pa_on_immediate_write_flag = FALSE;

rf_hal_bus_client_handle *rf_wcdma_hal_bus_client_handle;

static boolean rfwcdma_ftm_use_fb_path = FALSE;

/*< Temporary Variable to store the status of RF 3C tune per path */
boolean wcdma_3c_active[RF_PATH_2] = {FALSE};

static lm_handle_type rfwcdma_mc_fbrx_lm_handle;

/* Narrow Band Rejection global vars */
boolean enable_nbr_flag = FALSE; /* flag to indicate if NBR is enabled or not */
uint32  nbr_rxlm_buf_idx = 0; /* NBR modG RxLM index */

static rfm_device_enum_type last_device = RFM_INVALID_DEVICE; 

/*----------------------------------------------------------------------------*/
/*                          Function Prototypes                               */
/*----------------------------------------------------------------------------*/
void rfwcdma_mc_reset_tx_state_machine(rfcom_device_enum_type device);
void rfwcdma_mc_create_rf_buff(rf_buffer_intf **buffer_ptr);
void rfwcdma_mc_destroy_rf_buff(rf_buffer_intf *buffer_ptr);
void rfwcdma_mc_reset_state_machine_rx(rfm_device_enum_type device);
void rfwcdma_mc_create_device_scripts
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 channel_list[4],
  rf_buffer_intf *rx_device_buffer[],
  rfcom_multi_carrier_hspa_id_type num_chan,
  rfcom_wcdma_band_type source_band,
  rfwcdma_core_rxctl_dis_first_ind_type disable_ind,
  rfdevice_wcdma_spur_param_type* dyn_notch_filter_settings
);
/*----------------------------------------------------------------------------*/
/*                                Functions                                   */
/*----------------------------------------------------------------------------*/

/*!
        @brief
        Called to enter WCDMA mode: Doesn't place device to RX mode as we don't assume
        band. Init once flag handles programming that needs to be done only once for
        every power up

        @details
        This function should be called after MDSP image is downloaded and when
        entering WCDMA mode.

        @param device

        @param user_data_ptr

        @param cb_handler

        @param rxlm_buf_idx

        @return
        1 on success and 0 on failure
*/
uint32 rfwcdma_mc_enter_mode
(
  rfcom_device_enum_type device,
  const void *user_data_ptr,
  rfm_cb_handler_type cb_handler,
  uint32 rxlm_buf_idx
)
{
  rf_path_enum_type path;
  boolean wcdma_device_vote;
  timetick_type prof_cal_func;
  rfcommon_fbrx_error_type fbrx_status = RFCOMMON_FBRX_SUCCESS;
  boolean device_map_status = FALSE; 

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */
    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  device_map_status = rfwcdma_mc_map_rx_device(device);

  if (device_map_status == FALSE)
  {
    RF_MSG_1(RF_HIGH, "rfwcdma_mc_enter_mode: Map device on %d unsuccessful",device );
  }

  /* Initialize MDSP module if it has not been done */
  (void)rfwcdma_mdsp_init_once();

  /* Initialize FED CCS module */
  rfwcdma_mdsp_sync_config_ccs(rfwcdma_mc_state.primary_rx_device,TRUE);

  /* Allocate FBRx DM buffers for given FBRxLM handle */
  if (device == rfwcdma_mc_state.primary_rx_device)
  {
    fbrx_status = rfcommon_fbrx_mc_enter_mode( &rfwcdma_mc_fbrx_lm_handle, RFCOM_WCDMA_MODE);
    if(fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      RF_MSG(RF_FATAL, "rfcommon_fbrx_mc_enter_mode FBRx DM buffer allocation failed");
    }
  }

  /* Set gtow flag to false in WCDMA standalone mode */
  rfwcdma_mc_set_gtow_flag(FALSE);

  /* Set ltow flag to false in WCDMA standalone mode */
  rfwcdma_mc_set_ltow_flag(FALSE);

  /* Convert rfcom_device_enum_type to rf_path_enum_type */
  path = rfcommon_core_device_to_path(device);

  /* vote for RFCMD app */
  rf_hal_bus_enable(TRUE, &rf_wcdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(WCDMA));

  if (rf_wcdma_debug_flags.rumi_bup_wcdma == FALSE)
  {
    rfc_wcdma_manage_vregs(RFM_DEVICE_0, 
	                   rfwcdma_mc_get_default_rfcom_band(), 
	                   RF_PATH_RX_STATE);
  }

  /* Initialize all GPIOs */
  rfc_common_wakeup(path, RFCOM_WCDMA_MODE);

  if (rf_wcdma_debug_flags.rumi_bup_wcdma == FALSE)
  {
    wcdma_device_vote = rfcommon_core_dev_power_vote(device,
    RFCMN_CORE_DEV_ON,
                                                     RFCMN_CORE_DEV_NORMAL_VOTE
                                                     );
    if (wcdma_device_vote == FALSE)
    {
      RF_MSG_1(RF_FATAL,
      "rfwcdma_mc_enter_mode - Device - %d Power up failed",
      device);
    }
  }

  if ((device == rfwcdma_mc_state.primary_rx_device) || 
      (device == rfwcdma_mc_state.secondary_rx_device))
  {
    /* WARNING: Probably needs power of all the chipsets here and put to high
    impedence, before enabling temp comp */
    rfwcdma_mc_state.car_path_state[path][0].rf_state = RFWCDMA_MC_STATE_ENTER_MODE;

    /* init to INVALID so that select_rx_band can go through each time
                    after enter_mode is called (first power on or wakeup from deep sleep) */
    rfwcdma_mc_state.car_path_state[path][0].curr_band_rx = RFCOM_BAND_INVALID;
  }

  else 
  {
    /* WARNING: Probably needs power of all the chipsets here and put to high
    impedence, before enabling temp comp */
    rfwcdma_mc_state.car_path_state[path][1].rf_state = RFWCDMA_MC_STATE_ENTER_MODE;

    /* init to INVALID so that select_rx_band can go through each time
                    after enter_mode is called (first power on or wakeup from deep sleep) */
    rfwcdma_mc_state.car_path_state[path][1].curr_band_rx = RFCOM_BAND_INVALID;
  }


  /* Program the GPIO/GRFC/RF_CTL signals for the WCDMA mode */
  (void)rfc_wcdma_mode_config(device);

  /* Initialize once for the power up of the phone */
  if (rfwcdma_mc_state.init_once_flag == TRUE)
  {
    rfwcdma_mc_init_once();

    rfwcdma_mc_state.init_once_flag = FALSE;
  }

  /* L1 will call rf_do_temp_comp() every 10 secs
                  if FEATURE_RF_TEMP_COMP is defined */
  rfwcdma_core_temp_comp_enable(TRUE);

  /* Register callback function */
  rfwcdma_core_register_cb(cb_handler, 
                           user_data_ptr,
                           RFWCDMA_CORE_CB_DELAY_EXPIRED
                           );

  if (rfcommon_autopin_is_enabled(RFM_IMT_MODE))
  {
  if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_enter_mode(RFM_IMT_MODE))
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "WCDMA AutoPin enter mode failed!");
  }
  else
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "WCDMA AutoPin enter mode successful!");
  }
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Enter Mode - %d", prof_cal_func);
  }

  return 1;   /* success */
}

/*----------------------------------------------------------------------------*/
/*!
        @brief

        @details

        @return

*/
void rfwcdma_mc_reset_state_machine(void)
{
  uint8 i, j;

  /* Initialize state machine to default values */
  rfwcdma_mc_state.primary_path = RF_PATH_0;
  rfwcdma_mc_state.init_once_flag = TRUE;
  rfwcdma_mc_state.cpc_state_flag = FALSE;
  rfwcdma_mc_state.irat_script = FALSE;

  for (i = 0; i < RF_PATH_MAX; i++)
  {
    for (j = 0; j < RFCOM_MAX_CARRIERS; j++)
    {
      rfwcdma_mc_state.car_path_state[i][j].rf_state = RFWCDMA_MC_STATE_POWERUP_INIT;
      rfwcdma_mc_state.car_path_state[i][j].curr_band_rx = RFCOM_BAND_INVALID;
      rfwcdma_mc_state.car_path_state[i][j].curr_band_tx = RFCOM_BAND_INVALID;
      rfwcdma_mc_state.car_path_state[i][j].curr_chan_rx = 0;
      rfwcdma_mc_state.car_path_state[i][j].curr_chan_tx = 0;
      rfwcdma_mc_state.car_path_state[i][j].rx_rfm_device = RFM_INVALID_DEVICE;
      rfwcdma_mc_state.car_path_state[i][j].tx_rfm_device = RFM_INVALID_DEVICE;
    }
  }

  for (i = 0; i < RFM_DEVICE_4; i++)
  {
    rfwcdma_mc_state.rx_carriers[i] = 0;
    rfwcdma_mc_state.rx_multi_carrier_idx[i] = 0;
    rfwcdma_mc_state.rx_multi_carrier_idx_shadow[i]=0;
    rfwcdma_mc_state.rxlm_buf_idx[i] = 0xFF ;
    rfwcdma_mc_state.asm_status_bit_map[i] = 0;
    rfwcdma_mc_state.tuner_status_bit_map[i] = 0;
    rfwcdma_mc_state.pwr_vote_status_bit_map[i] = 0;
  }
  rfwcdma_mc_state.tx_carriers = 0;
  rfwcdma_mc_state.tx_multi_carrier_idx = 0;
  rfwcdma_mc_state.txlm_buf_idx = 0xFF ;
  rfwcdma_mc_state.primary_rx_device = RFM_INVALID_DEVICE;
  rfwcdma_mc_state.secondary_rx_device = RFM_INVALID_DEVICE;

  /* Temp fix to keep calls going */
  last_device = RFM_INVALID_DEVICE;

  RF_MSG_1(RF_HIGH,
  "RESET WCDMA MC State Machine", 0);
}
/*----------------------------------------------------------------------------*/
/*!
        @brief
        Calls to reset Rx state machine for the given device

        @details
        This funtion reset the Rx state machine item for the given device only.

        @return
        void
*/
void rfwcdma_mc_reset_state_machine_rx(rfm_device_enum_type device)
{
  uint8 i = 0;
  rf_path_enum_type path = rfcommon_core_device_to_path(device);

  for (i = 0; i < RFCOM_MAX_CARRIERS; i++)
  {
    if (rfwcdma_mc_state.car_path_state[path][i].rx_rfm_device == device)
    {
      rfwcdma_mc_state.car_path_state[path][i].rf_state = RFWCDMA_MC_STATE_POWERUP_INIT;
      rfwcdma_mc_state.car_path_state[path][i].curr_band_rx = RFCOM_BAND_INVALID;
      rfwcdma_mc_state.car_path_state[path][i].curr_chan_rx = 0;
      rfwcdma_mc_state.car_path_state[path][i].rx_rfm_device = RFM_INVALID_DEVICE;
    }
  }
  rfwcdma_mc_state.rx_carriers[device] = 0;
  rfwcdma_mc_state.rxlm_buf_idx[device] = 0xFF;
  rfwcdma_mc_state.rx_multi_carrier_idx[device] = 0;
  rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device] = 0; 
}
/*----------------------------------------------------------------------------*/
/*!
        @brief called to reset the tx parameters, during the tx deconfig

        @details

        @return

*/
void rfwcdma_mc_reset_tx_state_machine(rfcom_device_enum_type device)
{
  rf_path_enum_type path;
  uint32 multi_car_idx;
  uint8 i;

  path = rfcommon_core_device_to_path(device);
  multi_car_idx = rfwcdma_mc_state.tx_multi_carrier_idx;

  for (i = 0; i < RFCOM_MAX_CARRIERS; i++)
  {
    if ((multi_car_idx & (1 << i)) != 0)
    {
      rfwcdma_mc_state.car_path_state[path][i].rf_state = RFWCDMA_MC_STATE_RX;
      rfwcdma_mc_state.car_path_state[path][i].curr_band_tx = RFCOM_BAND_INVALID;
      rfwcdma_mc_state.car_path_state[path][i].curr_chan_tx = 0;
      rfwcdma_mc_state.car_path_state[path][i].tx_rfm_device = RFM_INVALID_DEVICE;
    }
  }
  rfwcdma_mc_state.tx_multi_carrier_idx = 0;
  rfwcdma_mc_state.txlm_buf_idx = 0xFF ;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called to exit WCDMA mode.

        @details

        @param device

        @param to_mode

        @return
        0 on success and 1 on error
*/
int32 rfwcdma_mc_exit_mode
(
  rfcom_device_enum_type device,
  rfcom_mode_enum_type to_mode
)
{

  rfcommon_fbrx_error_type fbrx_status = RFCOMMON_FBRX_SUCCESS;
  rf_path_enum_type path ;
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Clear all MC state variables only for DEVICE-0 request, this way we can
  avoid clearing all MC variables for other devices related exit mode while
  main device can be already active */
  if (device == rfwcdma_mc_state.primary_rx_device)
  {
    /* Clearing all MC state variables as the technology has changed */
    path = rfcommon_core_device_to_path(device); 

    if ( rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_SLEEP  )
    {
     RF_MSG(RF_HIGH, "rfwcdma_mc_exit_mode() [from sleep mode]: calling rfwcdma_mdsp_ccs_tq_pair_disassociate_device");
     rfwcdma_mdsp_ccs_tq_pair_disassociate_device();
    }else
    {
     RF_MSG(RF_HIGH, "rfwcdma_mc_exit_mode() [from non-sleep mode]: calling rfwcdma_mdsp_ccs_release_tq_pair");
     rfwcdma_mdsp_ccs_release_tq_pair(device);
    }

    fbrx_status = rfcommon_fbrx_mc_exit(rfwcdma_mc_fbrx_lm_handle, RFCOM_WCDMA_MODE);

    if(fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      RF_MSG(RF_FATAL, "rfcommon_fbrx_mc_exit FBRx DM buffer deallocation failed");
    }

    rfwcdma_mc_reset_state_machine();

    /* Reset callback data for all devices */
    rfwcdma_core_rxctl_reset_dynamic_notch_cb_data();

    /* Clean up MDSP module*/
    rfwcdma_mdsp_exit();
  }

  /* release vote for RFCMD app */
  rf_hal_bus_enable(FALSE, &rf_wcdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(WCDMA));


  if (rfcommon_autopin_is_enabled(RFM_IMT_MODE))
  {
    if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_exit_mode(RFM_IMT_MODE))
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "WCDMA AutoPin exit mode failed!");
    }
    else
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "WCDMA AutoPin exit mode successful!");
    }
  }

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  RF_MSG_2(RF_HIGH,"rfwcdma_mc_exit_mode:Dev %d, Time Spent:%d",device, profile_time);

  return 0;   /* no error */
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called to enable Tx chain

        @details
        The function enables the transmit chain for WCDMA uplink operation.

        @param device

        @param user_data_ptr

        @param cb_handler
*/
uint32 rfwcdma_mc_enable_tx
(
  rfcom_device_enum_type device,
  const void *user_data_ptr,
  rfm_cb_handler_type cb_handler
)
{
  rf_path_enum_type path;
  timetick_type prof_cal_func;
  rfcommon_fbrx_error_type fbrx_status = RFCOMMON_FBRX_SUCCESS;
  uint16 channels[RFCOM_MAX_CARRIERS] = {0};
  uint32 wtr_tune_time = 0;
  uint8 ret_val =1; 
  uint32 wtr_instance = 0;
  boolean pwr_vote_status = TRUE;

  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  path = rfcommon_core_device_to_path(device);

  channels[RFCOM_MULTI_CARRIER_ID__0] = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx;
  channels[RFCOM_MULTI_CARRIER_ID__1] = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__1].curr_chan_tx;

  if ((rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].rf_state != RFWCDMA_MC_STATE_RX) &&
  (rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].rf_state != RFWCDMA_MC_STATE_RXTX))
  {

    RF_MSG_1(RF_HIGH,
    "Enable TX called in non-WCDMA Rx state - Ignoring Enable TX", 0);
    ret_val =0; 
    return ret_val;
  }

  fbrx_status = rfcommon_fbrx_enable(rfwcdma_mc_fbrx_lm_handle);
  if(fbrx_status == RFCOMMON_FBRX_ERROR)
  {
    RF_MSG_1(RF_FATAL, "rfcommon_fbrx_enable FBRx HW enable failed: handle %d", rfwcdma_mc_fbrx_lm_handle);
  }

  rfc_wcdma_manage_vregs(RFM_DEVICE_0,
  rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_rx,
  RF_PATH_RXTX_STATE);

  
  rfwcdma_mc_pwr_vote_on_off(device,
                             band,
                             TRUE,
			     RFM_WCDMA_TX_DEVICE,
			     FALSE
                             );

  #ifndef FEATURE_MODEM_TX_SHARING

  /* Band specific settings. Including device config and mdsp config */
  if( rfwcdma_core_txctl_config_tune_device(device,
                                            band,
                                            channels,
                                            rfwcdma_mc_state.txlm_buf_idx,
                                            NULL,
                                            RFDEVICE_EXECUTE_IMMEDIATE,
                                            &wtr_tune_time ) == FALSE 
    )
    {
    RF_MSG_1(RF_ERROR, "rfwcdma_mc_enable_tx: Config Tune Device Tx failed for device %d", device);
    ret_val=0; 
  }
  #endif

  /* Configure ILPC sample capture if ILPC is enabled */
  if(rfcommon_fbrx_mc_is_ilpc_enabled(RFLM_TECH_WCDMA))
  {
    /*check the wtr_instance from RFC before enable ILPC sample capture*/
    wtr_instance = rfcommon_core_get_wtr_instance_from_device(device, band, RFCOM_WCDMA_MODE);

    if((wtr_instance < RFM_DEVICE_0) || (wtr_instance >= RFM_DEVICE_4))
    {
      wtr_instance = 0;
    }

    rfcommon_fbrx_ilpc_config_sample_capture( wtr_instance,
                                              rfwcdma_mc_state.txlm_buf_idx,
                                              rfwcdma_mc_fbrx_lm_handle );
  }

  /* Enable the Tx power limiting loop */
  rfwcdma_core_txplim_enable(TRUE);

  /* Update WCDMA state to RxTx since the Tx is enabled */
  rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].rf_state = RFWCDMA_MC_STATE_RXTX;

  /* Set the first txagc update flag to populate the ROT angle value*/
  rfwcdma_mdsp_async_set_first_txagc_update_flag(rfwcdma_mc_state.txlm_buf_idx,
                                                 TRUE
                                                 );

  /* Register callback function */
  rfwcdma_core_register_cb(cb_handler, 
                           user_data_ptr,
  RFWCDMA_CORE_CB_DELAY_EXPIRED);

  rfwcdma_auto_pin_supported(device,band);


   if (rfcommon_autopin_is_enabled(RFM_IMT_MODE))
   {
     rfcommon_autopin_mc_enable_tx(rfwcdma_mc_state.txlm_buf_idx,RFM_IMT_MODE);
  
   }


  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Enable Tx - %d", prof_cal_func);
  }

  RF_MSG_1(RF_HIGH, "RF Tx is enabled", 0);

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called to disable Tx chain

        @details
        The function is used to disable tx chain previously assigned for WCDMA
        uplink operation

        @param device

        @return
*/
int32 rfwcdma_mc_disable_tx(rfcom_device_enum_type device)
{
  rf_path_enum_type path;
  rfdevice_coupler_tech_cfg_type coupler_tech_cfg;
  timetick_type prof_cal_func;
  boolean api_status = TRUE;
  rfcom_wcdma_band_type curr_tx_band = RFCOM_BAND_INVALID; 
  boolean pwr_vote_status = TRUE;

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */
    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  path = rfcommon_core_device_to_path(device);
  curr_tx_band = rfwcdma_mc_state.car_path_state[path][0].curr_band_tx;

  if (rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state != RFWCDMA_MC_STATE_RXTX)
  {
    RF_MSG_1(RF_HIGH, "TX already in disabled state", 0);
    return 0;
  }

  /* Disable power limiting loop */
  rfwcdma_core_txplim_enable(FALSE);

  /* Disable Transceiever */
  rfdevice_wcdma_tx_disable(device,
                            curr_tx_band,
  RFDEVICE_EXECUTE_IMMEDIATE,
                            NULL
                           );

  rfwcdma_mc_pwr_vote_on_off(device,
                             curr_tx_band,
                             FALSE,
                             RFM_WCDMA_TX_DEVICE,
			     FALSE);

  rfc_wcdma_tx_disable(device, 
                       rfwcdma_mc_state.car_path_state[path][0].curr_band_rx
                       );

  #ifdef FEATURE_RF_HAS_QTUNER

   /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
    #ifdef FEATURE_RF_ASDIV
    rfcommon_asdiv_manager_enter_lock(); 
    #endif
    /*Disable Tx */
    (void)rfwcdma_core_antenna_tuner_tx_disable ( device, 
                                                  curr_tx_band,
                                                  rfwcdma_mc_state.car_path_state[path][0].curr_chan_tx,
                                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                                  NULL,
                                                  RFCOM_INVALID_TIMING_OFFSET);

   /* Release the ASDiv manager interface */
   #ifdef FEATURE_RF_ASDIV
   rfcommon_asdiv_manager_release_lock();
   #endif
  #endif /* FEATURE_RF_HAS_QTUNER */

  /* Disable QPOET. Passing NULL for the script buffer instructs */
  /* the PA class to perform the operation directly. */
  if (!rfdevice_wcdma_papm_disable(device, 
  	                               curr_tx_band, 
  	                               NULL))
  {
    RF_MSG_2(RF_ERROR, "Power tracker disable failed on device %d, band %d", device, curr_tx_band);
  }

  (void)rfdevice_wcdma_pa_sleep(device,
                                curr_tx_band,
  NULL,
  RFDEVICE_EXECUTE_IMMEDIATE,
  RFCOM_INVALID_TIMING_OFFSET);

  /* Disable the TX ASM before sleep */
  rfdevice_wcdma_asm_disable_tx(device,
                                curr_tx_band,
  NULL,
  RFDEVICE_EXECUTE_IMMEDIATE,
  RFCOM_INVALID_TIMING_OFFSET );

  /* Put the ASM to sleep */
  rfdevice_wcdma_asm_sleep_tx(device,
                              curr_tx_band,
  NULL,
  RFDEVICE_EXECUTE_IMMEDIATE,
  RFCOM_INVALID_TIMING_OFFSET );

  coupler_tech_cfg.mode = RFM_IMT_MODE;
  coupler_tech_cfg.band.wcdma_band = curr_tx_band;
  coupler_tech_cfg.port = 0;
  coupler_tech_cfg.direction = RFDEVICE_COUPLER_DIRECTION_OPEN;

  rfdevice_wcdma_coupler_config(device,
  coupler_tech_cfg,
  NULL,
  RFDEVICE_EXECUTE_IMMEDIATE,
  RFCOM_INVALID_TIMING_OFFSET);


  if (rfwcdma_core_xpt_is_enabled(curr_tx_band))
  {
    /* Disable ET DAC */
    rfwcdma_msm_disable_et_dac(rfwcdma_mc_state.txlm_buf_idx);
  }

  /* Updating to Rx state */
  rfwcdma_mc_state.car_path_state[path][0].rf_state = RFWCDMA_MC_STATE_RX;

  if (rf_wcdma_debug_flags.rumi_bup_wcdma == FALSE)
  {
    rfc_wcdma_manage_vregs(RFM_DEVICE_0,
    rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx,
    RF_PATH_RX_STATE);
  }
  
  rfwcdma_mdsp_async_abort_tuner_open_loop_update(rfwcdma_mc_state.txlm_buf_idx);

  rfwcdma_mdsp_sync_abort_open_loop_pending_snums();

  if (rfcommon_autopin_is_enabled(RFM_IMT_MODE))
  {
    rfcommon_autopin_mc_disable_tx(rfwcdma_mc_state.txlm_buf_idx,
                                 RFM_IMT_MODE);
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Tx Disable - %d", prof_cal_func);
  }
  
  RF_MSG_1(RF_HIGH, "RF Tx is disabled", 0);

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief

        @details
*/
uint32 rfwcdma_mc_enable_rx
(
  rfcom_device_enum_type device,
  const void *user_data_ptr,
  rfm_cb_handler_type cb_handler
)
{
  RF_MSG_1(RF_HIGH, "rfwcdma_mc_enable_rx (device = %d)", device);
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief

        @details
*/
int32 rfwcdma_mc_disable_rx(rfcom_device_enum_type device)
{
  RF_MSG_1(RF_HIGH, "rfwcdma_mc_disable_rx (device = %d)", device);
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief

        @details

*/
rfcom_ioctl_return_type rfwcdma_mc_ioctl
(
  rfcom_device_enum_type device,
  rfcom_ioctl_param_type request,
  void *pbuf, int32 length
)
{
  return RFCOM_IOCTL_NO_ERROR;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called to initialize Rx band specific settings when switching between
        WCDMA bands

        @details

        @param path

        @param band

        @return
*/
uint32 rfwcdma_mc_select_band_rx(rfcom_device_enum_type device,
                                 rfcom_wcdma_band_type band,
                                 uint32 multi_carrier_idx,
                                 uint32 rxlm_buf_idx,
                                 rfcom_wcdma_band_type source_band)
{
  uint8 i;
  rf_path_enum_type path = rfcommon_core_device_to_path(device);
  rf_buffer_intf *buffer_ptr = NULL;
  boolean pwr_vote_status = TRUE;
  rfdevice_trx_wcdma_rx **src_rx_device = NULL;
  rfdevice_trx_wcdma_rx **tgt_rx_device = NULL;
  uint8 device_idx = 0; 

  /* Create rf_buff */
  rfwcdma_mc_create_rf_buff(&buffer_ptr);

  /* No band change implies no need to repogram settings
                  for the same band */
  if ((band == rfwcdma_mc_state.car_path_state[path][0].curr_band_rx) && (ftm_irat_testing == FALSE))
  {
    /* Destroy rf_buff */
    rfwcdma_mc_destroy_rf_buff(buffer_ptr);
    return 0;
  }

  /* Program the GPIO/GRFC/RF_CTRL signals for the specified band*/
  rfc_wcdma_rx_band_config(device, band);

  /* Clear MC buffer */
  (void)rf_buffer_clear(buffer_ptr);

  /* If a different band is tuned on the device that was tuned before, then disable WTR on the previous band
  for that device */
  if ( (source_band != RFCOM_BAND_INVALID) && (source_band != band) ) 
  {
    rfdevice_wcdma_rx_disable(device,
                              source_band,
                              RFDEVICE_EXECUTE_IMMEDIATE,
                              NULL);

    rfwcdma_core_rxctl_disable_sleep_asm(device,
                                         source_band,
                                         NULL,
                                         RFDEVICE_EXECUTE_IMMEDIATE,
                                         RFCOM_INVALID_TIMING_OFFSET,
                                         RFWCDMA_CORE_ASM_TUNER_DIS_IND_BAND);

  }

  /* Configure ASM Rx */
  rfwcdma_core_rxctl_enable_asm(device,
                                band,
                                NULL,
                                RFDEVICE_EXECUTE_IMMEDIATE,
                                RFCOM_INVALID_TIMING_OFFSET);

  /* Destroy rf_buff */
  rfwcdma_mc_destroy_rf_buff(buffer_ptr);

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called to initialize Tx band specific settings when switching between
        WCDMA bands

        @details

        @param path

        @param band

        @return
*/
uint32 rfwcdma_mc_select_band_tx(rfm_device_enum_type device,
                                 rfcom_wcdma_band_type band,
                                 uint32 txlm_handle)
{

  /* Update WCDMA Tx band information */
  tx_band.wcdma_band = band;

  rfdevice_wcdma_pa_init(device,
                         band,
                         NULL,
                         NULL,
                         RFDEVICE_EXECUTE_IMMEDIATE);

  /*! If same RF device, device library should take care that it
                  shouldn't init it again esp when in RX mode */
  rfdevice_wcdma_tx_init(device,
                         RFCOM_WCDMA_MODE,
                         band,
                         RFDEVICE_EXECUTE_IMMEDIATE,
                         NULL);

  /* Wakeup PA for specific WCDMA band */
  rfdevice_wcdma_pa_wakeup(device,
                           band,
                           NULL,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           RFCOM_INVALID_TIMING_OFFSET);

  /* Configure the PA for specific WCDMA band */
  rfdevice_wcdma_pa_config(device,
                           band,
                           NULL,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           RFCOM_INVALID_TIMING_OFFSET,
                           RFDEVICE_PA_BW_5MHZ);
  /* Wakeup TX ASM for specific WCDMA band */
  rfdevice_wcdma_asm_wakeup_tx(device,
                               band,
                               NULL,
                               RFDEVICE_EXECUTE_IMMEDIATE,
                               RFCOM_INVALID_TIMING_OFFSET);

  /* Configure the ASM for specific WCDMA band */
  rfdevice_wcdma_asm_enable_tx(device,
                               band,
                               NULL,
                               RFDEVICE_EXECUTE_IMMEDIATE,
                               RFCOM_INVALID_TIMING_OFFSET);

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief

        @details

        @return

*/
boolean rfwcdma_mc_init
(
  rfcom_device_enum_type device,
  boolean common_init_successful,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)(rex_sigs_type)
)
{
  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;
  static boolean nv_items_read = FALSE;
  static boolean nv_init_success = FALSE;
  rfcom_wcdma_band_type band_ctr;

  rfwcdma_mc_reset_state_machine();

  /* Reset callback data for all devices */
  rfwcdma_core_rxctl_reset_dynamic_notch_cb_data();

  /* Initialize WCDMA Data module */
  rfwcdma_init_data();

  if (common_init_successful)
  {
    if (!nv_items_read)
    {
      nv_status = rfwcdma_nv_retrive_items(caller_tcb_ptr, task_nv_wait_sig,
                                           task_wait_func_ptr);

      if (nv_status == RFCOMMON_NV_READ_SUCCESS)
      {
        /* Not missing critical NV items */
        /* Assign NV table for each band to the global pointers. Do this whenever we retrieve NV */
        /* Loop through all of the possible NV bands. */
        for (band_ctr=RFCOM_BAND_IMT; band_ctr<RFCOM_NUM_WCDMA_BANDS; band_ctr++)
        {
          rfnv_wcdma_tbl_ptr[band_ctr] = rfwcdma_core_init_nv_table(band_ctr);
        }

        nv_init_success = TRUE;
      }

      rfwcdma_mdsp_init_critical_section();

      /* Initialize WCDMA Locks as the FIRST step in MC init for WCDMA. This is
      to prevent cases where rfm_init may fail, but other modules call RFM APIs.
      */
      rfwcdma_set_lock_data(rfm_get_lock_data());

      /* multi-lin data NV read is handled seperately*/
      rfwcdma_core_txlin_init();

      /* FBRX Cal data NV read is handled seperately */
      rfcommon_fbrx_nv_retrieve_wcdma_data( caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr);

      rf_common_init_critical_section(rfwcdma_core_temp_comp_get_lock_data());

      /* Remember that we've read the NV. */
      nv_items_read = TRUE;
    }
  }   /* if ( common_init_successful ) */

  /* Perform command processing initalization */
  rfwcdma_cmd_proc_init();

  /* register the IRAT APIs with meas module */
  rfwcdma_mc_meas_init();

  /* Initialize WCDMA log */
  rfwcdma_log_init();

  rfwcdma_core_create_npa_client();
  
  /*! Return success based on NV read success. */
  return nv_init_success;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called to perform one-time init of settings for WCDMA enter mode

        @details
        Certain parameters needs to be enabled when you enter the WCDMA mode for the
        first time. They will remain enabled until you exit the WCDMA mode. Some
        examples are AGC clock, RF <=> mDSP interface etc.
*/
void rfwcdma_mc_init_once(void)
{

  /*! Initialize WCDMA mDSP interface. Updates the
          RF-mDSP AGC interface structure pointers*/
  rfwcdma_mdsp_init();

  /* Initialize temp comp data settings */
  rfwcdma_core_temp_comp_data_init();

  /* Enable QTF Tuner Support only once when we enter WCDMA enter mode if Antenna Tuner is supported */
#ifdef FEATURE_RF_HAS_QTUNER
  if (enable_qtf_tuner_support == TRUE)
  {
    rfwcdma_mdsp_enable_qtf_tuner_support();
  }
#endif

  /* Initialize RX AGC ADJ, TX AGC ADJ, TRK_LO_ADJ, and IQ OFFSET */
  /* PDM delay and polarity */
  /* disable all PDM outputs */
  /* Override RX AGC ADJ, TX AGC ADJ, TRK_LO_ADJ, and IQ OFFSET */
  /* AGC clock is enabled/disabled within this function before writing
                  to the registers rflib_msm_enable_lna_and_mixer_range*/
  rfwcdma_core_update_min_power_from_nv();

}

/*----------------------------------------------------------------------------*/
/*!
        @brief
                This function initializes the transmit chain for WCDMA uplink operation.

        @details
        It initializes Tx parameter table, mDSP variables, and MODEM Tx registers.
        All function calls are come from rfwcdma_mc_enable_tx( ) to reduce
        enable_tx( ) timing. Long enable_tx() timing may cause RACH No-Ack and
        DCH channel setup failure due to missing WL1 timeline.
        This function should be called before rfm_enable_tx( ) is called.

        @return
        TRUE if all RFcal data is present
        FALSE if missing RFcal data
*/
boolean rfwcdma_mc_init_tx(rfcom_device_enum_type device, uint32 txlm_buf_idx, uint32 multi_carrier_idx, rfcom_tx_multi_carrier_cfg_type dc_hsupa_cfg)
{
  uint8 i;
  uint16 channels[RFCOM_MAX_CARRIERS];
  uint32 prev_carr_mask = 0;
  boolean status = TRUE;
  boolean tx_bw_change = FALSE;
  rfcommon_fbrx_error_type fbrx_status = RFCOMMON_FBRX_SUCCESS;
  rfcom_band_type_u fbrx_band = {0};

  rfcommon_fbrx_tx_cfg_type fbrx_cfg = RFCOMMON_FBRX_UMTS_SC;

  timetick_type prof_cal_func;
  prev_carr_mask = rfwcdma_mc_state.tx_multi_carrier_idx;
  tx_bw_change = IS_BW_CHANGE(prev_carr_mask,multi_carrier_idx);

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF Tx initialization started on device %d", device);
  /*Init MTPL as part of Tx Config*/
  rfwcdma_core_txplim_reset_mtpl();
  /* Disable TxAGC in case its running. Only applies to SC<->DC transitions */
  if (tx_bw_change)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF Tx BW change from %d to %d, disable TxAGC.",
        rfwcdma_mc_state.tx_multi_carrier_idx, multi_carrier_idx);
  (void)rfwcdma_core_tx_disable_txagc((rflm_dm_handle_id_t)txlm_buf_idx,TRUE);
  }

  /* Allocate Tx DM buffers for the given TxLM handle */
  (void)rfwcdma_mdsp_allocate_dm_tx_buf((rflm_dm_handle_id_t)txlm_buf_idx);

  /* Update state machine */
  rfwcdma_mc_state.tx_multi_carrier_idx = multi_carrier_idx;

  rfwcdma_mc_state.txlm_buf_idx = txlm_buf_idx;

  rfwcdma_mc_state.tx_carriers = rfwcdma_core_count_carriers(multi_carrier_idx);

  if (rfwcdma_mc_state.tx_carriers >= RFCOM_MAX_CARRIERS) 
  {
    RF_MSG_1(RF_FATAL, "rfwcdma_mc_init_tx: Incorrect multi_carrier_idx:%d", multi_carrier_idx);
    return FALSE;
  }

  for (i = 0; i < RFCOM_MAX_CARRIERS; i++)
  {
    if ((multi_carrier_idx & (1 << i)) != 0)
    {
      if (rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_rx != RFCOM_BAND_INVALID)
      {
        /* Set the same band for Tx */
        rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_tx =
          rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_rx;
        /* Mapping RX to TX channel */
        rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_chan_tx =
          rfwcdma_core_map_rx_to_tx_chan(rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_chan_rx,
                                         rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_rx);
        rfwcdma_mc_state.car_path_state[RF_PATH_0][i].tx_rfm_device = device;
        RF_MSG_5(RF_HIGH, "rfwcdma_mc_init_tx - i=%d, idx=%d, tx_band=%d, tx_chan=%d, dev=%d",
                 i, multi_carrier_idx, rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_tx,
                 rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_chan_tx,
                 rfwcdma_mc_state.car_path_state[RF_PATH_0][i].tx_rfm_device);
      }
      else if (i == RFCOM_DUAL_CARRIER)
      {
        rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_tx =
          rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx;
        rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_chan_tx =
          rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx - 25;
        rfwcdma_mc_state.car_path_state[RF_PATH_0][i].tx_rfm_device = device;
        RF_MSG_2(RF_FATAL, "rfwcdma_mc_init_tx - DC UL without DC DL, idx=%d, rx_band=%d",
                 multi_carrier_idx, rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_rx);
      }
      else
      {
        RF_MSG_2(RF_FATAL, "rfwcdma_mc_init_tx - INVALID CONFIG, idx=%d, rx_band=%d",
                 multi_carrier_idx, rfwcdma_mc_state.car_path_state[RF_PATH_0][i].curr_band_rx);
      }

    }
  }

  /* Init tx power limiting loop */
  rfwcdma_core_txplim_init(rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx,
                           rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx,
                           multi_carrier_idx);

  /* Update TxLM buffer */
  status &= rfwcdma_msm_update_txlm_buffer(txlm_buf_idx, device,
            rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx,
            multi_carrier_idx, dc_hsupa_cfg);

  /* Do FBRx initialization if FBRx ILPC is enabled in NV */
  if( rfcommon_fbrx_mc_is_ilpc_enabled(RFLM_TECH_WCDMA) )
  {
  /* FBRx config */
  if (multi_carrier_idx == RFWCDMA_MC_DC)
  {
    fbrx_cfg = RFCOMMON_FBRX_UMTS_DC;
  }
  else if (multi_carrier_idx == RFWCDMA_MC_SC)
  {
    fbrx_cfg = RFCOMMON_FBRX_UMTS_SC;
  }
  else
  {
    RF_MSG_1(RF_FATAL, "rfwcdma_mc_init_tx: unsupported multi_carrier_idx:%d", multi_carrier_idx);
  }

  fbrx_band.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx;

  /* Allocate Tx DM buffers for the given FBRxLM handle */
  fbrx_status = rfcommon_fbrx_mc_init_tx(rfwcdma_mc_fbrx_lm_handle, fbrx_cfg, txlm_buf_idx);

  } /* if( rfcommon_fbrx_mc_is_ilpc_enabled(RFLM_TECH_WCDMA) ) */

  if (rfcommon_autopin_is_enabled(RFM_IMT_MODE))
  {
   rfwcdma_core_autopin_init(txlm_buf_idx);
  }

  /* Configure MDSP and FED modules */
  rfwcdma_core_txctl_config_band( device,
                                  rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx, 
                                  txlm_buf_idx, 
                                  tx_bw_change);

  if(tx_bw_change)
  {
     RF_MSG_3(RF_HIGH, "rfwcdma_mc_init_tx: Skipping config chan during carrier change on Dev%d,"
                       "prev_carr%d, new_carr%d", device, prev_carr_mask, multi_carrier_idx );
  }
  else 
  {
  /* Tx channel config of MDSP/FED layer */
  rfwcdma_core_txctl_config_chan(device,
                                 rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx,
                                 rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx,
                                 txlm_buf_idx);
  }

  /* Set sar limit */
  rfwcdma_core_txplim_set_sar_limit(rfm_common_sar_get_state());

  /* Prepare for Multi-Carrier config */
  channels[0] = rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx;
  channels[1] = rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__1].curr_chan_tx;

  /* Only initialized PDA, will be updated to actuall setting in retune_bw */
  rfwcdma_core_txctl_config_mc(rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx, RFWCDMA_MC_SC , channels, txlm_buf_idx);

  /* Do FBRx initialization if FBRx ILPC is enabled in NV */
  if( rfcommon_fbrx_mc_is_ilpc_enabled(RFLM_TECH_WCDMA) )
  {
  fbrx_status &= rfcommon_fbrx_mc_tune_to_chan(txlm_buf_idx,
                 RFCOM_WCDMA_MODE,
                 fbrx_band,
                 channels,
                 (uint8)rfwcdma_mc_state.tx_carriers+1,
                 rfwcdma_mc_fbrx_lm_handle);

    /* Error fatal if there are any errors */
  if(fbrx_status == RFCOMMON_FBRX_ERROR)
  {
    RF_MSG(RF_FATAL, "rfcommon_fbrx_mc_init_tx Tx DM buffer allocation failed");
  }
  }

  if (tx_bw_change)
  {
    /* Update MSM Tx bandwidth settings */
    (void)rfwcdma_msm_update_tx_bw(rfwcdma_core_count_carriers(multi_carrier_idx), txlm_buf_idx);

	/* Reenable TxAGC in case its running. Should only happen in SC<->DC transitions */
    if (!IS_FTM_CAL_MODE())
  {
    (void)rfwcdma_core_tx_disable_txagc((rflm_dm_handle_id_t)txlm_buf_idx,FALSE);
  }

    /* Re-enable MTPL loop*/
	rfwcdma_core_txplim_enable(TRUE);
  }

  /* Check status */
  if (status == FALSE)
  {
    RF_MSG_1(RF_HIGH, "RF Tx initialization failed for band:%d",
             rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx);
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
  }

  RF_MSG_1(RF_HIGH, "RF Tx initialization is complete - RFWCDMA Execution Time: Init Tx - %d", prof_cal_func);   

  /* For RUMI, it is expected to fail without QCN and valid Cal data */
#ifdef T_RUMI_EMULATION
  return TRUE;
#else
  return status;
#endif
}

/*!
        * @brief Prepare CCS memory after a power collapse.
        *
        * @details After a power collapse we lose any contents in CCS memory.
        * This function will re-populate the TX information. This allows for
        * a shorter transition to TX then going through
        * rfwcdma_mc_init_tx.
        *
        * \author kylej (3/18/2013)
        */
void rfwcdma_mc_wakeup_tx(void)
{
  rfm_device_enum_type device;
  timetick_type prof_cal_func;

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  device = rfwcdma_mc_state.primary_rx_device;

  /* Populate Tx AGC configuration to RFLM 
     through common Tx AGC configuration */
  rfwcdma_core_txctl_txagc_config(device,
                                  rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx, 
                                  rfwcdma_mc_state.txlm_buf_idx);

  /* Reload DPD tables */
  rfwcdma_core_txlin_reload_dpd(device,
                            rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx,
                            rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx,
                            rfwcdma_mc_state.txlm_buf_idx);

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: rfwcdma_mc_wakeup_tx - %dus", prof_cal_func);
}
}/* rfwcdma_mc_wakeup_tx */

/*----------------------------------------------------------------------------*/
/*!
        @brief
        This is used to set an RF physical path (antenna -> demod -> RxFE, etc.) to
        the RFCOM_TRANSCEIVER_0 logical device.  Typically, the RFCOM_TRANSCEIVER_0
        is mapped with the physical RF path 0 (intuitive!).
        However, for certain test cases, we might want to exercise the secondary (diversity)
        physical RF path, and therefore map it to the logical device RFCOM_TRANSCEIVER_0.

        This also has the effect of mapping the "other" RF physical path to the "other"
                logical device.  For example calling rfm_set_primary_path( RF_PATH_0)
                maps RFCOM_TRANSCEIVER_0 logical device to RF_PATH_0 physical device AND
                maps RFCOM_RECEIVER_DIV logical device to RF_PATH_1 physical device.

        @details
*/
void rfwcdma_mc_set_primary_path(rf_path_enum_type primary_path)
{
  rfwcdma_mc_state.primary_path = primary_path;
}
/*----------------------------------------------------------------------------*/
/*!
        @brief
        This function is used to get the current primary path and return to L1

        @details
*/
rf_path_enum_type rfwcdma_mc_get_primary_path(void)
{
  return rfwcdma_mc_state.primary_path;
}


/*----------------------------------------------------------------------------*/
/*!
        @brief
        This function writes tech dependent (R99 or HS) lna nv items to mdsp

        @details
*/
void rfwcdma_mc_write_tech_dependent_nv_items_mdsp(rfm_device_enum_type device, rfcom_wcdma_band_type band, rfcom_tech_type tech)
{
  rfwcdma_mdsp_set_data_param_type lna_swicthpoints = {0};
  uint8 lna_state = 0;
  rfwcdma_nv_rx_static_element_type rise_element, fall_element = 0;
  boolean api_status = FALSE;
  uint32 nv_container =0;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type cfg;
  rfnv_wcdma_nv_tbl_type *nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(rfnv_wcdma_convert_band_rfcom_to_rfnv(band));

  cfg.alternate_path = 0;
  cfg.band = band;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;


  if (rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
  {
    RF_MSG_3( RF_HIGH, "rfwcdma_mc_write_tech_dependent_nv_items_mdsp: For Device:%d, band: %d NV container is: %d",
              device, band, device_info_ptr->nv_container);
  }
  else
  {
    RF_MSG_2( RF_ERROR, "rfwcdma_mc_write_tech_dependent_nv_items_mdsp - Cannot get NV container for"
                         "device %d Div config, band %d",device,band);

  return;
  }

  if (device_info_ptr->nv_container < RFM_DEVICE_4)
  {
    nv_container = device_info_ptr->nv_container; 
  } 

  /* Check R99/HS mode and update the NV element before query NV module */
  if (tech == RFCOM_REL99_MODE)
  {
    rise_element = LNA_RISE;
    fall_element = LNA_FALL;
  }
  else if (tech == RFCOM_HS_MODE)
  {
    rise_element = LNA_HS_RISE;
    fall_element = LNA_HS_FALL;
  }
  else
  {
    RF_MSG_1(RF_ERROR,"rfwcdma_mc_write_tech_dependent_nv_items_mdsp, invalid tech:%d",
             tech);
    return;
  }

  /* Get FGAGC LNA switchpoints */
  for (lna_state = 0; lna_state < RF_WDMA_MAX_NUM_LNA_STATES - 1; lna_state++)
  {
    /* HS/R99 switchpoint change only applicable to device 0 (PRx) in single carrier mode  */
    lna_swicthpoints.data.set_lna_switchpoint.lnarise[lna_state] =
      rfnv_wcdma_get_info_from_rx_static_data((rfm_device_enum_type)nv_container,
          RFCOM_SINGLE_CARRIER,
          rise_element,
          (lna_state),
          &nv_tbl->rx_static_data);

    lna_swicthpoints.data.set_lna_switchpoint.lnafall[lna_state] =
      rfnv_wcdma_get_info_from_rx_static_data((rfm_device_enum_type)nv_container,
          RFCOM_SINGLE_CARRIER,
          fall_element,
          (lna_state),
          &nv_tbl->rx_static_data);
  }

  /* Update set data command */
  lna_swicthpoints.set_data_cmd = RFWCDMA_MDSP_SET_LNA_SWITHPOINT;

  /* Update RFLM handle */
  lna_swicthpoints.rflm_handle = rfwcdma_mc_state.rxlm_buf_idx[RFM_DEVICE_0];

  /* Call mdsp function to update switchpoints */
  api_status = rfwcdma_mdsp_async_set_data(&lna_swicthpoints);

  if (!api_status)
  {
    RF_MSG_2(RF_ERROR,"rfwcdma_mc_write_tech_dependent_nv_items_mdsp,failed! band:%d, tech:%d ",
             band,tech);
  }
  else
  {
    RF_MSG_2(RF_MED,"rfwcdma_mc_write_tech_dependent_nv_items_mdsp,success.. band:%d, tech:%d ",
             band,tech);
  }
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        This function writes tech dependent (R99 or HS) ici coeffs from the rxlm

        @details
*/
boolean rfwcdma_mc_write_tech_dependent_dynamic_update(rfcom_tech_type curr_tech, uint8 rxlm_buf_idx[], rfcom_device_enum_type device)
{
  RF_MSG(RF_MED,"rfwcdma_mc_write_tech_dependent_dynamic_update: nothing to be done");
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Called by L1 every 10sec to update temp comp data.

        @details
*/
void rfwcdma_mc_do_temp_comp(rfm_device_enum_type device)
{

  if ((rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state == RFWCDMA_MC_STATE_RXTX))
  {
    rfwcdma_core_temp_comp_update(device);
  }

}


/*----------------------------------------------------------------------------*/
/*!
        @brief
        Dispatch WCDMA command.

        @details

        @param device devices supported by RF driver.
        @param cmd Specifies the command.
        @param *data Input and/or Output parameter for the command

        @retval value generic return value which depends on the 'cmd' parameter.

*/
int32 rfwcdma_mc_cmd_dispatch(rfcom_device_enum_type device, int32 cmd,
                              void *data)
{
  int32 value = 0; 
  uint8 alt_path = 0;
  rf_path_enum_type path;
  rfc_wcdma_dc_supported_type dc_support;
  rfcom_wcdma_band_type band;
  sys_band_mask_type sys_band_mask;
  rfagc_receive_chain_type rx_chain;
  int16 rxagc_val = 0;
  rfcmn_status_type alt_path_status = RFCMN_PATH_SEL_SUCCESS;
  uint32 multi_carrier_idx = 1;  /* Pass multi carrier index always as 1 for W2W meas RxLM update*/

  /*! Convert rfcom_device_enum_type to rf_path_enum_type */
  path = rfcommon_core_device_to_path(device);

  if (path > RF_PATH_1)
  {
    RF_MSG_1(RF_ERROR, "Invalid Rx path :%d", path);
    return (value);
  }

  /*! If it is diversity only call, change the path to diversity */
  if ((path == RF_PATH_0) && (rfwcdma_mc_state.primary_path == RF_PATH_1))
  {
    path = RF_PATH_1;
  }

  rx_chain = rfcommon_core_path_to_receive_chain(path);

  switch (cmd)
  {
  case RFWCDMA_MC_IS_SLEEPING:
    value = (int32)rfwcdma_core_util_is_sleeping(path);
    break;

  case RFWCDMA_MC_GET_RX_AGC:
    (void)rfwcdma_core_util_get_rxagc((rfagc_receive_chain_type)path,RFCOM_SERVINGCELL_RXAGC, RFCOM_MULTI_CARRIER_ID__0,RFCOM_RXAGC_IN_DBM, &rxagc_val );
    value = (int32)rxagc_val;
    break;

  case RFWCDMA_MC_GET_RAW_RX_AGC:
    (void)rfwcdma_core_util_get_rxagc((rfagc_receive_chain_type)path,RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_MULTI_CARRIER_ID__0,RFCOM_RXAGC_IN_DB10, &rxagc_val );
    value = (int32)rxagc_val;
    break;

  case RFWCDMA_MC_GET_RAW_RX_AGC_X2W_MEAS:
    value = (int32)rfwcdma_core_util_get_rxagc_xtow_meas( (rfagc_receive_chain_type)path,RFCOM_INGAP_RXAGC_RAW, RFCOM_MULTI_CARRIER_ID__0,RFCOM_RXAGC_IN_DB10, ( (rfwcdma_mc_meas_rxagc_xtow*)data)-> wcdma_band,  ( (rfwcdma_mc_meas_rxagc_xtow*)data)-> bypass_rxagcon_flag );
    break;

  case RFWCDMA_MC_GET_RX_AGC_X2W_MEAS:
    value = (int32)rfwcdma_core_util_get_rxagc_xtow_meas( (rfagc_receive_chain_type)path,RFCOM_INGAP_RXAGC, RFCOM_MULTI_CARRIER_ID__0,RFCOM_RXAGC_IN_DBM, ( (rfwcdma_mc_meas_rxagc_xtow*)data)-> wcdma_band,  ( (rfwcdma_mc_meas_rxagc_xtow*)data)-> bypass_rxagcon_flag );
    break;

  case RFWCDMA_MC_GET_TX_AGC_VAL:
    value = (int32)rfwcdma_core_util_get_rflm_txagc_dBm();
    break;

  case RFWCDMA_MC_GET_RX_AGC_VAL_FOR_RSCP:
    (void)rfwcdma_core_util_get_rxagc((rfagc_receive_chain_type)path,(*(rfcom_rxagc_type *)data), RFCOM_MULTI_CARRIER_ID__0,RFCOM_RXAGC_IN_DBM, &rxagc_val );
    value = (int32)rxagc_val;
    break;

  case RFWCDMA_MC_GET_RX_AGC_C0_VAL_FOR_RSCP:
    (void)rfwcdma_core_util_get_rxagc((rfagc_receive_chain_type)path,(*(rfcom_rxagc_type *)data), RFCOM_MULTI_CARRIER_ID__0,RFCOM_RXAGC_IN_DBM, &rxagc_val );
    value = (int32)rxagc_val;
    break;

  case RFWCDMA_MC_GET_RX_AGC_C1_VAL_FOR_RSCP:
    (void)rfwcdma_core_util_get_rxagc((rfagc_receive_chain_type)path,(*(rfcom_rxagc_type *)data), RFCOM_MULTI_CARRIER_ID__1,RFCOM_RXAGC_IN_DBM, &rxagc_val );
    value = (int32)rxagc_val;
    break;

  case RFWCDMA_MC_GET_MIN_TX_PWR_VAL:
    value = (int32)rfwcdma_core_util_get_ue_min_tx_power();
    break;

  case RFWCDMA_MC_GET_HDET_THRESHOLD_AGC:
    value = (int32)rfwcdma_core_txplim_get_hdet_threshold();
    break;

  case RFWCDMA_MC_GET_TX_AGC_TABLES:
    value = (int32)rfwcdma_core_util_get_txagctablestore((*(rfcom_multi_carrier_id_type *)data));
    break;

  case RFWCDMA_MC_TX_AGC_TO_DBM:
    value = (int32)rfwcdma_core_util_convert_tx_agc_to_dbm_unit(*(int16 *)(data));
    break;

  case RFWCDMA_MC_TX_DBM_TO_AGC:
    value = (int32)rfwcdma_core_util_convert_tx_dbm_to_agc_unit(*(int16 *)(data));
    break;

  case RFWCDMA_MC_GET_UE_MAX_TX_PWR:
    value = (int32)rfwcdma_core_util_get_ue_max_tx_power(*(rfcom_txplim_channel_type *)(data));
    break;

  case RFWCDMA_SET_TX_FREEZE_ACUUM:
    value = (int32)rfwcdma_core_txctl_set_tx_freezeaccum(((rfwcdma_mc_tx_freezeaccum_type *)data)->val, ((rfwcdma_mc_tx_freezeaccum_type *)data)->car_idx);
    break;

  case RFWCDMA_MC_ENABLE_PWR_CTRL:
    value = (int32)rfwcdma_core_txctl_enable_pwr_ctrl();
    break;

  case RFWCDMA_MC_SET_TX_PWR_LIMIT:
    rfwcdma_core_txplim_set_mtpl(((rfwcdma_mc_tx_pwr_limit_type *)data)->tx_limit_dbm, ((rfwcdma_mc_tx_pwr_limit_type *)data)->chan_type);
    break;

  case RFWCDMA_MC_GET_TX_ROTATOR_ANGLE:
    rfwcdma_core_util_get_tx_rotator_angle(data);
    break;

  case RFWCDMA_MC_PA_IS_ON:
    value = (int32)rfwcdma_core_util_get_rflm_tx_on();
    break;

  case RFWCDMA_MC_GET_ENC_BTF_VAL:
    value = (int32)rfwcdma_core_get_enc_btf_val();
    break;

  case RFWCDMA_MC_GET_MAX_TX_PWR_LIMIT:
    rfwcdma_core_txplim_get_mtpl_backoff((rf_max_tx_pwr_limit_type *)data);
    break;

  case RFWCDMA_MC_TURN_ON_RF_LDO:
    RF_MSG_1(RF_ERROR, "rfwcdma_mc_turn_on_rf_ldo() - Device - %d", device);
    rfc_wcdma_manage_vregs(RFM_DEVICE_0, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx, RF_PATH_SLEEP_STATE);
    if ((rfcommon_core_dev_power_vote(device, RFCMN_CORE_DEV_OFF, RFCMN_CORE_DEV_NO_VOTE)) == FALSE)
    {
      RF_MSG_1(RF_FATAL, "rfwcdma_mc_turn_on_rf_ldo - Device - %d Power down failed", device);
    }
    break;

  case RFWCDMA_MC_GET_CURR_BAND:
    value = (int32)rfwcdma_core_convert_band_rfcom_to_rfi(rfwcdma_mc_state.car_path_state[path][0].curr_band_rx);
    break;

  case RFWCDMA_MC_GET_MAX_TX_PWR_CONFIG_ITEM:
    band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised
    value = (int32)rfwcdma_core_txplim_get_mtpl_nv_item(*(rfcom_txplim_channel_type *)data, band);
    value = (value / RF_TXAGC_RESOLUTION) + RF_MIN_TX_POWER_DBM;
    break;

  case RFWCDMA_MC_GET_RF_WARMUP_TIME:
    value = (int32)rfwcdma_core_util_get_rf_warmup_time();
    break;

  case RFWCDMA_MC_GET_RF_CAPABILITY:
    value = rfwcdma_core_util_get_rf_capability(*(rfcom_rf_cap_type *)data);
    break;

  case RFWCDMA_MC_GET_MDSP_TX_AGC:
    value = (int32)rfwcdma_core_util_get_rflm_txagc_dB10();
    break;

  case RFWCDMA_MC_GET_MDSP_TX_PWR_LIMIT:
    value = (int32)rfwcdma_core_util_get_mdsp_tx_power_limit();
    break;

  case RFWCDMA_MC_BAND_SUPPORTS_RXD:
    if (rfwcdma_core_convert_band_rfi_to_rfcom(*(rfi_band_type *)data) >= RFCOM_NUM_WCDMA_BANDS)
    {
      value = FALSE;
    }
    else
    {
      sys_band_mask = rfwcdma_core_get_sys_band_mask_from_band_rfi(*(rfi_band_type *)data);
      if (rfcommon_nv_get_rf_bc_config(RF_PATH_1) & sys_band_mask)
      {
        value = TRUE;
      }
      else
      {
        value = FALSE;
      }
    }
    break;

  case RFWCDMA_MC_BAND_SUPPORTS_DC:
    dc_support.band = rfwcdma_core_convert_chan_to_band((*(uint16 *)data));
    (void)rfc_wcdma_command_dispatch(RFM_DEVICE_0, RFC_WCDMA_IS_DUAL_CARRIER_SUPPORTED, (void *)(&dc_support));
    value = dc_support.dc_is_supported;
    break;

  case RFWCDMA_MC_GET_TX_PWR_LIMIT:
    value = (int32)rfwcdma_core_txplim_get_mtpl();
    break;

  case RFWCDMA_MC_GET_RX_AGC_VAL:
  case RFWCDMA_MC_GET_RX_AGC_C0_VAL:
  case RFWCDMA_MC_GET_RX_AGC_C1_VAL:

    /* Get the RxAGC value from MDSP */
    (void)rfwcdma_core_util_get_rxagc(rx_chain, ((rfwcdma_mc_rxagc_type *)data)->type, ((rfwcdma_mc_rxagc_type *)data)->carr_id, ((rfwcdma_mc_rxagc_type *)data)->unit, &rxagc_val );
    value = (int32)rxagc_val;
    break;

  case RFWCDMA_MC_TURN_ON_PA:
    rfwcdma_mdsp_set_pa(TRUE, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx);
    break;

  case RFWCDMA_MC_TURN_OFF_PA:
    rfwcdma_mdsp_set_pa(FALSE, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx);
    break;

  case RFWCDMA_MC_GET_FREQ_FROM_UARFCN:
    band = rfwcdma_core_convert_chan_to_band(((rfwcdma_mc_channel_type *)data)->chan[0]);
    value = (int32)rfwcdma_core_get_freq_from_uarfcn(band, ((rfwcdma_mc_channel_type *)data)->chan[0], RFWCDMA_CORE_UARFCN_DL);
    break;

  case RFWCDMA_MC_GET_UL_FREQ_FROM_DL_UARFCN:
    value = (int32)rfwcdma_core_get_freq_from_uarfcn(((rfwcdma_mc_channel_type *)data)->band,
            (word)rfwcdma_core_map_rx_to_tx_chan((uint16)(((rfwcdma_mc_channel_type *)data)->chan[0]), ((rfwcdma_mc_channel_type *)data)->band),
            RFWCDMA_CORE_UARFCN_UL
                                                    );
    break;


  case RFWCDMA_MC_GET_LO_FREQ_FROM_UARFCN:
    band = rfwcdma_core_convert_chan_to_band(((rfwcdma_mc_channel_type *)data)->chan[0]);
    value = (int32)((rfwcdma_core_get_freq_from_uarfcn_no_limit(band, ((rfwcdma_mc_channel_type *)data)->chan[0], RFWCDMA_CORE_UARFCN_DL)\
                     + rfwcdma_core_get_freq_from_uarfcn_no_limit(band, ((rfwcdma_mc_channel_type *)data)->chan[1], RFWCDMA_CORE_UARFCN_DL)) / 2);
    break;

  case RFWCDMA_MC_CONVERT_CHAN_TO_BAND:
    value = (int32)rfwcdma_core_convert_chan_to_band(*(uint16 *)data);
    break;

  case RFWCDMA_MC_GET_HDET_TX_AGC_VALUE:
    value = (int16)rfwcdma_mdsp_get_hdet_tx_agc_value();
    break;

  case RFWCDMA_DO_TEMP_COMP:
    rfwcdma_mc_do_temp_comp(device);
    break;

  case RFWCDMA_MC_GET_MDSP_BUF_SIZE_W16:
    value = (int32)RFWCDMACM_MDSP_WTOW_SETUP_SIZE;
    break;

  case RFWCDMA_MC_GET_WTOW_RF_CTL_BUF:
#ifndef FEATURE_WCDMA_PLT
    /*rfmeas_mc_wtow_get_rf_ctl_buf(((rfwcdma_mc_wtow_ctl_type *)data)->arfcn,
                                                                                                                            ((rfwcdma_mc_wtow_ctl_type *)data)->buf_ptr);*/
#endif
    break;

  case RFWCDMA_MC_WRITE_TECH_DEPENDENT_CONFIG_TO_MDSP:
    band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;
    rfwcdma_mc_write_tech_dependent_nv_items_mdsp(device, band, *(rfcom_tech_type *)data);
    break;

  case RFWCDMA_MC_GET_MULTI_CARRIER_WTOW_RF_CTL_BUF:
#ifndef FEATURE_WCDMA_PLT
#endif
    break;

  case RFWCDMA_MC_GP_CLOCK_ON:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFWCDMA_MC_GP_CLOCK_ON: deprecated API", 0);
    break;

  case RFWCDMA_MC_GP_CLOCK_OFF:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFWCDMA_MC_GP_CLOCK_OFF: deprecated API", 0);
    break;

  case RFWCDMA_REDUCE_MTPL_FOR_THERM:
    rfwcdma_core_txplim_reduce_mptl_db_for_therm(*((uint8 *)data));
    break;

  case RFWCDMA_MC_UPDATE_RXLM_BUFFER:
    
    /* Get Alternate path info from RFC */
    alt_path_status = rfcmn_concurrency_mgr_update_rx_path( device,
                                                            RFCOM_WCDMA_MODE,
                                                            (int)rfwcdma_core_convert_chan_to_band(((rfwcdma_mc_rxlm_param_type *)data)->channel[0]),
                                                            &alt_path);

    if (alt_path_status == RFCMN_PATH_SEL_FAILED) 
    {
       MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "MC cmd dispath update rxlm buffer: Alternate path read failed");
    }

    /* Pass multi carrier index always as 1*/
     value = (int32)rfwcdma_msm_update_rxlm_buffer(device,
            ((rfwcdma_mc_rxlm_param_type *)data)->channel,
           	multi_carrier_idx,
            ((rfwcdma_mc_rxlm_param_type *)data)->rxlm_buf_idx,
            (uint32)alt_path,
             enable_nbr_flag,
             NULL, RFWCDMA_MSM_STATIC_NOTCH_W2W_MEAS); /* As this API here in command dispatch is called by WL1 only in W2W meas.
                                                         Need to be removed in the future */
    break;

  case RFWCDMA_MC_GET_PRIMARY_PATH:
    value = (int32)rfwcdma_mc_get_primary_path();
    break;

  case RFWCDMA_MC_TRIGGER_SAR_UPDATE:
    rfwcdma_core_txplim_set_sar_limit(rfm_common_sar_get_state());
    break;

  case RFWCDMA_MC_SET_RX_FREEZE_ACCUM:
    rfwcdma_core_rxctl_set_rx_freeze_accum(device,
                                           *(boolean *)data);
    break;

  case RFWCDMA_MC_SET_QUIET_MODE:
    value = (int32)rfwcdma_mc_set_pmic_quiet_mode(*(boolean *)data);

    break;

  case RFWCDMA_MC_SET_LOWER_POWER_MODE:
    
    /* Get Alternate path info from RFC for Primary chain only as LPM is for SC PRx only*/
    alt_path_status = rfcmn_concurrency_mgr_update_rx_path( RFM_DEVICE_0,
                                                            RFCOM_WCDMA_MODE,
                                                            (int)rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx,
                                                            &alt_path);

    if (alt_path_status == RFCMN_PATH_SEL_FAILED) 
    {
       MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "MC cmd dispath set low power mode: Alternate path read failed");
    }

    value = (int32)rfwcdma_mc_set_lower_power_mode(((rfwcdma_mc_lpm_param_type *)data)->lpm_config,
            ((rfwcdma_mc_lpm_param_type *)data)->c0_rxlm_buf_idx,
            ((rfwcdma_mc_lpm_param_type *)data)->c1_rxlm_buf_idx,
            (uint32)alt_path);
    break;

  case RFWCDMA_MC_GET_PA_STATE:
    value = (int32)rfwcdma_core_util_get_rflm_pa_range();
    break;

  case RFWCDMA_MC_SET_CPC_STATE:
    rfwcdma_mc_state.cpc_state_flag = *(boolean *)data;
    break;

  case RFWCDMA_MC_SET_WAKEUP_OPTIMIZATION:
    // Temporarily disable wakeup optimizatino for Bolt bring up
    enable_wakeup_optimization_flag = *(boolean *)data;
    break;

  case RFWCDMA_MC_CONFIG_NBR:
    enable_nbr_flag = ((rfwcdma_mc_nbr_config_param_type *)data)->enable_nbr;
    nbr_rxlm_buf_idx = ((rfwcdma_mc_nbr_config_param_type *)data)->nbr_rxlm_idx;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Invalid Command Dispatch=%d", cmd);
    break;

  }

  return value;
}


/*----------------------------------------------------------------------------*/
/*!
        @brief

        rfdevice pointer must have been initialized before call this function

        @details
*/
void rfwcdma_mc_power_clock_on(rfcom_device_enum_type device)
{

  if (rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx == RFCOM_BAND_INVALID)
  {
    rfc_wcdma_manage_vregs(RFM_DEVICE_0, RFCOM_BAND_IMT, RF_PATH_RX_STATE);

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfwcdma_mc_power_clock_on (device=%d) (band = %d)", device, RFCOM_BAND_IMT);

  }
  else
  {
    rfc_wcdma_manage_vregs(RFM_DEVICE_0, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx, RF_PATH_RX_STATE);

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfwcdma_mc_power_clock_on (device=%d) (band = %d)", device, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx);

  }

  /* vote to enable RFCMD app */
  rf_hal_bus_enable(TRUE, &rf_wcdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(WCDMA));

}

/*----------------------------------------------------------------------------*/
/*!
        @brief

        rfdevice pointer must have been initialized before call this function

        @details
*/
void rfwcdma_mc_power_clock_off(rfcom_device_enum_type device)
{

  if (!rf_disable_powercollapse)
  {

    rfc_wcdma_manage_vregs(device, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx, RF_PATH_SLEEP_STATE);

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfwcdma_mc_power_clock_off(device=%d) (band=%d)", device, rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx);

  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set RF 3C tune status

  @details
  This function will check if RF is tuned to 3C or not

  @param device
   RFM Device to be used

  @param num_chan
   Carrier mode to which RF is tuned to. SC/DC/3C

*/
static void
rfwcdma_set_tri_carrier_status
(
  rfcom_device_enum_type device,
  rfcom_multi_carrier_hspa_id_type num_chan
)
{
  rf_path_enum_type path;

  path = rfcommon_core_device_to_path(device);

  if(num_chan == RFCOM_3_CARRIER)
  {
    wcdma_3c_active[path] = TRUE ;
  }
  else
  {
    wcdma_3c_active[path] = FALSE ;
  }

} /* rfwcdma_set_tri_carrier_status */

/*----------------------------------------------------------------------------*/
/*!
        @brief
        This function tunes the synthesizer to the specified channel.

        @details
        Once
        rf_tune_to_chan has been called, a delay in microseconds must be performed
        by the application to allow the synthesizer to settle.
        The channel is the UTRA Absolute Radio Frequency Channel Number (UARFCN).

        @param device
                RFM device to be tuned

        @param channel
                List of channels

        @param multi_carrier_idx
                Carrier ID combo (0/1/2)

        @param  user_data_ptr
                Used for CB

        @param cb_handler
                Call back handler

        @param rxlm_buf_idx
                RxLM buffer Index

*/
int32 rfwcdma_mc_tune_to_multi_chan(rfcom_device_enum_type device,
                                     uint16 channel[],
                                     uint32 multi_carrier_idx,
                                     const void *user_data_ptr,
                                     rfm_cb_handler_type cb_handler,
                                     uint32 rxlm_buf_idx
                                    )
{
  rf_path_enum_type path=0;
  rfcom_wcdma_band_type band;
  rfcom_wcdma_band_type source_band = RFCOM_BAND_INVALID;
  rfc_wcdma_core_config_type rfc_core_config;
  uint8 i, c;
  rfcom_multi_carrier_hspa_id_type num_chan;
  timetick_type prof_cal_func;
  timetick_type pll_wait_time;
  uint32 alt_path = 0;
  boolean pwr_vote_status = TRUE ;
  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };
  uint32 temp_rxlm_idx[RFM_DEVICE_4] = {0};
  rfdevice_wcdma_spur_param_type *dyn_notch_filter_settings = NULL;

  rfdevice_wcdma_rx_tune_param_type rx_params;
  rx_params.bw   = RFDEV_WCDMA_BW_INVALID;
  rx_params.band = RFCOM_BAND_INVALID;

  /* Invalid # of WCDMA Carriers check, multi_carrier_idx is bit mask, rfwcdma_mc_count_carriers returns the carrir #*/
  if (rfwcdma_core_count_carriers(multi_carrier_idx) >= RFCOM_MAX_CARRIERS)
  {
    return 0;
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  path = rfcommon_core_device_to_path(device);

  band = rfwcdma_core_convert_chan_to_band(channel[0]);

  source_band = rfwcdma_mc_get_source_band(device);
  
  (void)rfc_wcdma_command_dispatch(device, RFC_WCDMA_GET_CORE_CONFIG, (void *)(&rfc_core_config));

  if (rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_POWERUP_INIT)
  {
    return 0;
  }

  /* Update Alt path to RFC*/
  rfwcdma_core_rxctl_update_alt_path(device, band, FALSE, &alt_path);

  /* Update state machine */

  /* Maintain a shadow of the multi_carrier index for future use */

  if (rfwcdma_mc_state.rx_multi_carrier_idx[device]!= 0)
  {
    rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device] = rfwcdma_mc_state.rx_multi_carrier_idx[device]; 
  }
  else 
  {
    rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device] = multi_carrier_idx; 
  }

  rfwcdma_mc_state.rx_multi_carrier_idx[device] = multi_carrier_idx;
  rfwcdma_mc_state.rx_carriers[device] = rfwcdma_core_count_carriers(multi_carrier_idx);
  rfwcdma_mc_state.rxlm_buf_idx[device] = rxlm_buf_idx;

  num_chan = rfwcdma_mc_state.rx_carriers[device];

  cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
  temp_rxlm_idx[device] = rxlm_buf_idx;

  if (device == rfwcdma_mc_state.primary_rx_device)
  {
    /* For Primary Rx device, disable diversity on secondary too*/
    cmd_action[rfwcdma_mc_state.secondary_rx_device] = RFWCDMA_MDSP_AGC_CMD_DISABLE;

    /* Always disable AGC on all carriers */
    rfwcdma_core_rxctl_enable_rxagc(rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device],
                                    cmd_action,
                                    temp_rxlm_idx,
                                    FALSE,0,0);
  }
  else
  {
    rfwcdma_core_rxctl_enable_rxagc(rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device],
                                    cmd_action,
                                    temp_rxlm_idx,
                                    FALSE,0,0);
  }

  /* Allocate Rx DM buffers for the given RxLM handle */
  (void)rfwcdma_mdsp_allocate_dm_rx_buf((rflm_dm_handle_id_t)rxlm_buf_idx);

  /*! This function executes if the band changes. Band specific initialization occurs here */
  rfwcdma_mc_select_band_rx(device, band,multi_carrier_idx, rxlm_buf_idx, source_band);

  rfwcdma_set_tri_carrier_status(device,num_chan);

  rfwcdma_mc_pwr_vote_on_off(device,
                             band,
                             TRUE,
                             RFM_WCDMA_RX_DEVICE,
                             FALSE
                             );

  for (i = 0; i < RFCOM_MAX_CARRIERS; i++)
  {
    if ((multi_carrier_idx & (1 << i)) != 0)
    {
      rfwcdma_mc_state.car_path_state[path][i].curr_band_rx =
        rfwcdma_core_convert_chan_to_band(channel[rfwcdma_mc_state.rx_carriers[device]]);
      rfwcdma_mc_state.car_path_state[path][i].rx_rfm_device = device;
    }
  }

#ifdef FEATURE_RF_HAS_QTUNER
  if ( (source_band != RFCOM_BAND_INVALID) && (source_band != band) ) 
  {
    rfwcdma_core_rxctl_disable_tuner(device,
                                     source_band,
                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                     NULL,
                                     RFCOM_INVALID_TIMING_OFFSET,
                                     RFWCDMA_CORE_ASM_TUNER_DIS_IND_BAND);
  }

  /* Program the tuner in Rx mode*/
  rfwcdma_core_rxctl_enable_tuner(device,
                                  band,
                                  channel[0],
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL);
#endif

  /* Get the global callback data here before calling Set channel */
  dyn_notch_filter_settings = rfwcdma_core_rxctl_get_dynamic_notch_cb_data(device);

  /*! Tune RX PLL */
  /* Single Carrier */
  if (num_chan == RFCOM_SINGLE_CARRIER)
  {
    /* Set rx tune params */
    rx_params.band       = band;
    rx_params.bw         = RFDEV_WCDMA_BW_1X;
    rx_params.channel[0] = channel[0];

    /* Tune Rx PLL to SC mode */
    rfdevice_wcdma_rx_set_channel( device, RFCOM_WCDMA_MODE, rx_params.band,
                                   rx_params, RFDEVICE_EXECUTE_IMMEDIATE, 
                                   NULL, rfwcdma_core_rxctl_update_dynamic_notch_cb, 
                                   dyn_notch_filter_settings, TRUE);

    RFGNSS_MC_V(wg_chan)(channel[0]);

    /* Update Notch filter settings */
    RFGNSS_MC_V(program_notch_filter)(RFGNSS_NOTCH_CFG_WCDMA);
  }
  /* Dual carrier */
  else if (num_chan == RFCOM_DUAL_CARRIER)
  {
    /* Set rx tune params */
    rx_params.band       = band;
    rx_params.bw         = RFDEV_WCDMA_BW_2X;
    rx_params.channel[0] = channel[0];
    rx_params.channel[1] = channel[1];

    /* Tune Rx PLL to DC mode */
    rfdevice_wcdma_rx_set_channel( device, RFCOM_WCDMA_MODE,
                                   rx_params.band, rx_params,
                                   RFDEVICE_EXECUTE_IMMEDIATE, 
                                   NULL, rfwcdma_core_rxctl_update_dynamic_notch_cb,
                                   dyn_notch_filter_settings, TRUE );
  }
  /* 3-Carrier */
  else if (num_chan == RFCOM_3_CARRIER)
  {
    /* Set rx tune params */
    rx_params.band       = band;
    rx_params.bw         = RFDEV_WCDMA_BW_3X;
    rx_params.channel[0] = channel[0];
    rx_params.channel[1] = channel[1];
    rx_params.channel[2] = channel[2];

    /* Tune Rx PLL to 3-C mode */
    rfdevice_wcdma_rx_set_channel( device, RFCOM_WCDMA_MODE,
                                   rx_params.band, rx_params,
                                   RFDEVICE_EXECUTE_IMMEDIATE, 
                                   NULL, rfwcdma_core_rxctl_update_dynamic_notch_cb, 
                                   dyn_notch_filter_settings, TRUE );
  }
  else
  {
    RF_MSG_1( RF_ERROR,
              " Invalid carrier specification in rfwcdma_mc_tune_to_multi_chan - %d",
              num_chan );
    return 0;
  }

  pll_wait_time = 0;
  pll_wait_time = timetick_get();

  (void)rfwcdma_msm_update_rxlm_buffer(device, channel, multi_carrier_idx, rxlm_buf_idx, alt_path, enable_nbr_flag, dyn_notch_filter_settings, RFWCDMA_MSM_STATIC_NOTCH_TUNE);

  if( enable_nbr_flag &&
      ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)) 
    )
  {
    /* print dbg msg */
    RF_MSG_1( RF_HIGH,
              "W_NBR DBG: Loading up modG on RxLM index %d for PRx",
              nbr_rxlm_buf_idx );
    (void)rfwcdma_msm_update_nbr_rxlm_buffer( device,
                                              channel, 
                                              num_chan,
                                              nbr_rxlm_buf_idx,
                                              alt_path );
  }

  rfwcdma_core_rxctl_config_band(rxlm_buf_idx, device, band, multi_carrier_idx, RFLM_WCDMA_HOME_MEAS);

  rfwcdma_core_rxctl_config_chan(rxlm_buf_idx, device, band, channel, multi_carrier_idx, RFLM_WCDMA_HOME_MEAS);

  /*
                  Following conditions need to be met to send start AGC command to avoid sending back to back AGC start command
                  1. FW RxAGC ON status flag need to be OFF for the given carrier and chain
                  2. RF internal RxAGC enable flag need to be FALSE for the given carrier and chain
                  For DC, both carrier AGC will be enbaled
  */

  cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
  temp_rxlm_idx[device] = rxlm_buf_idx;

  pll_wait_time = timetick_get_elapsed(pll_wait_time, T_USEC);
  if(pll_wait_time < rfc_core_config.tune_time)
  {
    /*! Wait for PLL settle after device tune */
    if ((!IS_FTM_IN_TEST_MODE()) || (rfm_get_calibration_state() == FALSE))
    {
      /* Wait for PLL to settling before starting AGC */
      DALSYS_BusyWait(MIN(rfc_core_config.tune_time, (rfc_core_config.tune_time - pll_wait_time)));
    }
  }
   if( enable_nbr_flag &&
       ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)) )
   {
     /* print dbg msg */
     RF_MSG( RF_HIGH, "W_NBR DBG: Enabling RxAGC");
   
     /* Send enable AGC cmd to FW */
     rfwcdma_core_rxctl_enable_rxagc( multi_carrier_idx,
                                      cmd_action,
                                      temp_rxlm_idx,
                                      FALSE,
                                      TRUE,       
                                      (uint8)nbr_rxlm_buf_idx );
   }
   else
   {
     /* Send enable AGC cmd to FW */
  rfwcdma_core_rxctl_enable_rxagc(multi_carrier_idx,
                                  cmd_action,
                                  temp_rxlm_idx,
                                      FALSE,
                                      FALSE,       
                                      (uint8)nbr_rxlm_buf_idx );
   }

  /*! Update the channel information. */
  for (i = 0,c = 0; i < RFCOM_MAX_CARRIERS; i++)
  {
    if ((multi_carrier_idx & (1 << i)) != 0)
    {
      rfwcdma_mc_state.car_path_state[path][i].curr_chan_rx = channel[c++];

      MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "Tune to multi-Chan: Path:%d,Multi-Carrier:%d, i %d channel:%d ",
            path, multi_carrier_idx, i, rfwcdma_mc_state.car_path_state[path][i].curr_chan_rx);

      /* In case of SC<->DC config, L1 will not stop Tx and we are still in RxTx state after retune Rx*/
      if (rfwcdma_mc_state.car_path_state[path][i].rf_state != RFWCDMA_MC_STATE_RXTX)
      {
        /* Update the rf_state here */
        rfwcdma_mc_state.car_path_state[path][i].rf_state  = RFWCDMA_MC_STATE_RX;
      }
    }
  }

  /* Reset MTPL to ACQ mode */
  if (device == RFM_DEVICE_0)
  {
    rfwcdma_core_txplim_reset_to_acq_mode();
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_6(RF_HIGH, "Tune to multi-Chan, band:%d,Path:%d,Multi-Carrier:%d, Channel1:%d, Channel2:%d Execution Time: %d",
          band, path, num_chan, channel[0], channel[1], prof_cal_func);
  }
  else
  {
  //Need to update the F3 to print 3rd channel info
    RF_MSG_5(RF_HIGH, "Tune to multi-Chan, band:%d,Path:%d,Multi-Carrier:%d, Channel1:%d, Channel2:%d ",
        band, path, num_chan, channel[0], channel[1]);
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
        This function sets the wcdma gtow active flag

        @params - val
*/
void rfwcdma_mc_set_gtow_flag
(
  boolean val
)
{
  rfwcdma_gtow_flag = val;
}

/*----------------------------------------------------------------------------*/
/*!
        This function sets the wcdma ltow active flag

        @params - val
*/
void rfwcdma_mc_set_ltow_flag
(
  boolean val
)
{
  rfwcdma_ltow_flag = val;
}

/*----------------------------------------------------------------------------*/
/*!
        This function returns  the wcdma gtow active flag

*/
boolean rfwcdma_mc_get_gtow_flag
(
  void
)
{
  return (rfwcdma_gtow_flag);
}

/*----------------------------------------------------------------------------*/
/*!
        This function returns a flag to indicate whether it is SC/DC/3C.

*/

rfcom_multi_carrier_hspa_id_type rfwcdma_mc_get_carr_state(rfcom_device_enum_type device)
{
  return (rfwcdma_mc_state.rx_carriers[device]);
}

/*----------------------------------------------------------------------------*/
/*!
This function returns  the wcdma ltow active flag

*/
boolean rfwcdma_mc_get_ltow_flag
(
  void
)
{
  return (rfwcdma_ltow_flag);
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Reading NV for WCDMA.

        @details
        Retrieves all the NV from WCDMA mode. Reads common and mode specific RF
        configuration parameters from NV. This is called as part of the
        LOAD_RF_NV function case where we need to read the calibration NV and write
        them without having to reset.

*/
boolean rfwcdma_mc_reload_nv
(
  rfcom_device_enum_type device,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)(rex_sigs_type)
)
{
  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;
  boolean nv_init_success = FALSE;

  nv_status = rfwcdma_nv_retrive_items(caller_tcb_ptr, task_nv_wait_sig,
                                       task_wait_func_ptr);

  /* For certain factory verification sequence, NS testing is performed right after RF Cal and reload NV.
                  Updating temp comp NVs because essential in this sequence */
  rfwcdma_core_temp_comp_update(device);

  if (nv_status == RFCOMMON_NV_READ_SUCCESS)
  {
    /* Not missing critical NV items */
    nv_init_success = TRUE;
  }
  /* multi-lin data NV read is handled seperately*/
  rfwcdma_core_txlin_init();

  /* FBRX Cal data NV read is handled seperately */
  rfcommon_fbrx_nv_retrieve_wcdma_data( caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr);

  /*! @todo Need to return actual nv_init_success status
  Returning TRUE is a temporary solution to avoid going to offline for other
  technologies which don't activate WCDMA critical NV items. */
  return nv_init_success;
}
/*----------------------------------------------------------------------------*/
/*!
        @brief
        Set PMIC quiet mode

        @details
*/
boolean rfwcdma_mc_set_pmic_quiet_mode(boolean en)
{
  const rfc_common_logical_device_params_type *cmn_logical_dev = NULL;

  /*Hard coding the logical device to RFM_DEVICE_0 as device information is not passed
          for WCDMA functions and WCDMA is always supported on RFM_DEVICE_0 only for now*/
  cmn_logical_dev = rfc_common_get_logical_device_params(RFM_DEVICE_0);

  if (cmn_logical_dev == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFWCDMA set PMIC quiet mode error: common device not initialized", 0);
    return FALSE;
  }

  rfc_set_quiet_mode(RFCOM_WCDMA_MODE, en);

  return (TRUE);
}
/*----------------------------------------------------------------------------*/
/*!
        @brief

        enable/disable modem/rf low power mode

        @details
        based on the lpm_config, will disable/enable modem low power mode(low adc fs), or
        rtr low power mode(rtr lower linearity).


*/
boolean rfwcdma_mc_set_lower_power_mode(rfcom_lpm_config_enum_type lpm_config,
                                        uint32 c0_rxlm_buf_idx,
                                        uint32 c1_rxlm_buf_idx,
                                        uint32 alt_path)
{
  boolean status = FALSE;
  /* Low power/High power mode switch can only be invoked in single carrier mode and no RxD support for now */
  if ((rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0] != RFCOM_SINGLE_CARRIER) || (rfwcdma_mc_state.car_path_state[RF_PATH_1][0].rf_state == RFWCDMA_MC_STATE_RX))
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_set_low_power_mode, not in single carrier mode or RxD is enabled! current carrier mode:%d, current rxd status :%d return...",
          rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0], rfwcdma_mc_state.car_path_state[RF_PATH_1][0].rf_state);
    return FALSE;
  }
  /* Check state machine for primary chain*/
  if ((rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state != RFWCDMA_MC_STATE_RX) &&
      (rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state != RFWCDMA_MC_STATE_RXTX))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_set_low_power_mode,PRx is not enabled! current state:%d, return...",
          rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state);
    return FALSE;
  }
  if ((lpm_config == RFCOM_LPM_MODEM) || (lpm_config == RFCOM_LPM_MODEM_RF))
  {
    /* Update RxLM */
    status = rfwcdma_msm_set_low_power_mode(TRUE, 
                                            RFM_DEVICE_0, 
                                            rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_rx,
                                            alt_path,
                                            c0_rxlm_buf_idx);
    /* Enable when we support RxD with DSR*/

  }
  else if (lpm_config == RFCOM_LPM_DISABLE)
  {
    /* Update RxLM */
    status = rfwcdma_msm_set_low_power_mode(FALSE, 
                                            RFM_DEVICE_0, 
                                            rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_rx, 
                                            alt_path,
                                            c0_rxlm_buf_idx);

  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid LPM config:%d !", lpm_config);
    status = FALSE;
  }

  if (status == TRUE)
  {
    /* Send command to FW */
    rfwcdma_mdsp_send_lpm_agc_cmd(lpm_config, c0_rxlm_buf_idx, c1_rxlm_buf_idx);
    MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfwcdma_set_low_power_mode sucess, lpm_config:%d,c0 rxlm buffer index:%d, c1 rxlm_buffer index:%d",
          lpm_config, c0_rxlm_buf_idx, c1_rxlm_buf_idx);
    return TRUE;
  }
  else
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_set_low_power_mode fail, en:%d,c0 rxlm buffer index:%d, c1 rxlm_buffer index:%d",
          lpm_config, c0_rxlm_buf_idx, c1_rxlm_buf_idx);

    return FALSE;
  }

}

/*----------------------------------------------------------------------------*/
/*!
        @brief
                This function deconfig the transmit chain for WCDMA uplink operation.

        @details
                It deconfig some settings made in init_tx, and make disable Tx thinner.
                This function should be called before WL1 send txlm_cfg(disable) to FW.

        @return
                1 on success and 0 on failure
*/
boolean rfwcdma_mc_decfg_tx(rfcom_device_enum_type device)
{
  boolean status = TRUE;
  xpt_cfg_type xptcfg = XPT_CFG_TYPE_MAX;

  /* Wait for the temp comp done */
  rf_common_enter_critical_section(rfwcdma_core_temp_comp_get_lock_data());

  /* Disable ET path if it's ET supported */
  xptcfg = (xpt_cfg_type)rfwcdma_core_xpt_get_cfg_param\
           (rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx, XPT_CFG);
  if ((xptcfg == ET_CFG) || (xptcfg == EPT_ET_CFG))
  {
    status &= rf_common_xpt_init_dpd(device);
  }
  else if(xptcfg == EPT_CFG)
  {
    status &= rf_common_xpt_init_dpd(device);
  }

  rflm_wcdma_ccs_tx_exit(rfwcdma_mc_state.txlm_buf_idx);

  rfwcdma_mc_reset_tx_state_machine(device);

  rf_common_leave_critical_section(rfwcdma_core_temp_comp_get_lock_data());

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfwcdma_mc_decfg_tx status %d", status);
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
        @details
                This function apply SAR limit passed from MCS

        @param device
        @param
*/
void rfwcdma_mc_set_multi_tx_sar_limit(rfcom_device_enum_type device, int16 sar_limit)
{
  rfwcdma_core_txplim_set_multi_tx_sar_limit(device, sar_limit);

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "set_multi_tx_sar_limit status: device %d,limit %d",
        device, sar_limit);
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
                This creates an rf_buffer
*/
void  rfwcdma_mc_create_rf_buff(rf_buffer_intf **buffer_ptr)
{
  *buffer_ptr = rf_buffer_create(RFWCDMA_MAX_SSBI_SCRIPT_SIZE,
                                 RFWCDMA_MAX_RFFE_SCRIPT_SIZE,
                                 RFWCDMA_MAX_GRFC_SCRIPT_SIZE);

  /* Check rf_buff has been properly created. No point proceeding if error */
  if (*buffer_ptr == NULL)
  {
    RF_MSG_1(RF_ERROR, "Buffer creation failed %d", buffer_ptr);
    ASSERT(0);
  }

} /* rfwcdma_mc_create_rf_buff */

/*----------------------------------------------------------------------------*/
/*!
        @brief
                This destroy
*/
void rfwcdma_mc_destroy_rf_buff(rf_buffer_intf *buffer_ptr)
{
  (void)rf_buffer_destroy(buffer_ptr);
} /* rfwcdma_mc_destroy_rf_buff */




/*----------------------------------------------------------------------------*/
/*!
        @brief
        Get default band
        This will return the default band in rfcom_wcdma_band_type

        @details
        This will return the default band during set_model for both RF and WL1.
        default band is not necessarily band 1.

===========================================================================*/
rfcom_wcdma_band_type rfwcdma_mc_get_default_rfcom_band(void)
{
  rfcom_wcdma_band_type band = RFCOM_BAND_IMT;
  while (!rfwcdma_core_is_band_supported(band, RF_PATH_0) && band < RFCOM_NUM_WCDMA_BANDS)
  {
    band++;
  }

  /* check if there is no wcdma band supported */
  if (band == RFCOM_NUM_WCDMA_BANDS)
  {
    /* Removing the ERR fatal to support those cards that doesn't support WCDMA.
            ERR_FATAL("No WCDMA band is supported in Band config NV",0,0,0);
            */
    band = RFCOM_BAND_INVALID;
  }

  return band;
}



/*----------------------------------------------------------------------------*/
/*!
        @brief
        Get default band and middle DL chan for WL1 to call in rfm layer

        @details
        This will return the default band and its middle DL chan, during set_model for WL1.
        default band is not necessarily band 1.


===========================================================================*/
void rfwcdma_mc_get_default_rfi_band_and_chan(rfi_band_type *band, uint16 *chan)
{
  /* call rfwcdma_core_convert_band_rfcom_to_rfi to get chan and convert rfcom_wcdma_band_type to rfi_band_type */
  rfcom_wcdma_band_type rfcom_band = rfwcdma_mc_get_default_rfcom_band();
  *band = rfwcdma_core_convert_band_rfcom_to_rfi(rfcom_band);

  /*Get middle DL chan*/
  *chan = rfwcdma_core_get_middle_DL_chan_from_band(rfcom_band);

}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Get DBDC band combinations supported

        @details
        This will have to return the number of DBDC combinations supported on the RFC and those band combos.
        For RFCs which does not support any DBDC bands , by deafult it will be zero.

        @param num_band_comb_supported
        This will return the total number of DBDC band combinations supported by the RFC

        @param band_comb_list
        This would give the DB-DC band combinations as to which band is supported on which carrier

        @return
                Flag indicating the validity of the DBDC band combinations

*/

boolean rfwcdma_mc_get_dbdc_band_support(uint16 *num_band_comb_supported, const rfm_wcdma_dual_band_type **band_comb_list)
{
  boolean status = TRUE;
  rfc_wcdma_properties_type *band_list_ptr = NULL;
  uint16 num = 0;

  band_list_ptr = (rfc_wcdma_properties_type *)rfc_wcdma_get_dbdc_band_support();

  if ((band_list_ptr == NULL) || (band_comb_list == NULL))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "DBDC: Null pointer returned for DBDC band support", 0);
    status = FALSE;
    return (status);
  }

  *band_comb_list = band_list_ptr->dbdc_band_combo;

  while (((uint32)(band_list_ptr->dbdc_band_combo[num].prim_band)) != ((uint32)SYS_BAND_CLASS_NONE))
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, " DBDC: Primary band : %d Secondary car band : %d", band_list_ptr->dbdc_band_combo[num].prim_band, band_list_ptr->dbdc_band_combo[num].sec_band);

    num++;
  }

  *num_band_comb_supported = num;

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, " DBDC: Number of DBDC combinations supported : %d", *num_band_comb_supported);

  return (status);

}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        This function gives RFWCDMA MC state info plus Path and Number of Carriers supported by the RF Device

        @details
        This function gives rfwcdma_mc_state.car_path_state info and Number of Carriers supported by the RF Device

        @param device Devices supported by RF driver

        @param car_path_state[] Array giving RFWCDMA MC state info for all the supported carriers

        @return
        valid_num_of_carrier Number of Carriers supported by Device

*/
uint8 rfwcdma_mc_get_car_state_info_from_device(rfcom_device_enum_type device, rfwcdma_mc_car_path_state_type car_path_state[])
{
  uint8 i;
  uint8 valid_num_of_carrier = 0;
  rf_path_enum_type path;

  path = rfcommon_core_device_to_path(device);

  if (path == RF_PATH_MAX)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid path for device: %d", device);
    return valid_num_of_carrier;
  }

  for (i = 0; i < RFCOM_MAX_CARRIERS; i++)
  {
    if (device == rfwcdma_mc_state.car_path_state[path][i].rx_rfm_device)
    {
      memscpy(&car_path_state[valid_num_of_carrier], sizeof(rfwcdma_mc_car_path_state_type), &rfwcdma_mc_state.car_path_state[path][i], sizeof(rfwcdma_mc_car_path_state_type));
      valid_num_of_carrier++;
    }
  }
  return valid_num_of_carrier;
}

/*----------------------------------------------------------------------------*/
/*!
        @brief
        Returns MC state for a given path

        @details
                        This indicates WCDMA RF state machince

        @return
        Returns MC state for a given path
*/

rfwcdma_mc_state_enum_type rfwcdma_mc_get_path_state(rf_path_enum_type path)
{
  return (rfwcdma_mc_state.car_path_state[path][0].rf_state);
}


/*----------------------------------------------------------------------------*/
/*!
        @brief
        Returns Sampling rate for a given Rx LM

        @details
        Used for xPT IQ Capture ET Path delay

        @param
        Rx LM Buffer Index for which to return sampling rate

        @return
        Returns Sampling rate in Hz
*/

uint32 rfwcdma_mc_get_samp_rate(uint8 rx_lm_buf_idx)
{
  return (rfwcdma_msm_get_samp_rate(rx_lm_buf_idx));
}
/*----------------------------------------------------------------------------*/
/*!
        @brief
        Returns MC state for a given device

        @details
                        This indicates WCDMA RF state machince

        @return
        Returns MC state for a given device
*/

rfwcdma_mc_state_enum_type rfwcdma_mc_get_path_state_of_device(rfcom_device_enum_type device)
{
  rf_path_enum_type path;
  path = rfcommon_core_device_to_path(device);
  return (rfwcdma_mc_state.car_path_state[path][0].rf_state);
}


/*
        @brief
This API will return the synth status for a given Tech.Path and synthesizer

        @details

        @param
        device : RF device
        rf_mode : RF mode
        synth_type: where TX PLL is rqequired or RX PLL
        is_synth_locked: PLL status.
*/

void rfwcdma_mc_get_synth_lock_status
(
  rfcom_device_enum_type device,
  rfcom_mode_enum_type rf_mode,
  rfcom_synth_type synth_type,
  void *is_synth_locked
)
{
  rf_path_enum_type path;
  path = rfcommon_core_device_to_path(device);

  if (synth_type == RF_SYNTH_RX)
  {
    rfdevice_wcdma_rx_pll_lock_detect(device,
                                      rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                                      is_synth_locked,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL
                                     );
  }
  else if (synth_type == RF_SYNTH_TX)
  {
    rfdevice_wcdma_tx_pll_lock_detect(device,
                                      rfwcdma_mc_state.car_path_state[path][0].curr_band_tx,
                                      is_synth_locked,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL
                                     );
  }
  else
  {
    RF_MSG_1(RF_ERROR, "Invalid SYNTH query(%d) for SYNTH in for rfm_get_synth_lock_status",
             synth_type);
  }
}


/*
        @brief
This API will return if any of the WCDMA bands are supported

        @details

        @param

*/


boolean rfwcdma_mc_is_wcdma_supported(void)
{

  rfcom_wcdma_band_type wcdma_band = RFCOM_BAND_IMT;
  boolean is_any_of_the_bands_supported = FALSE;

  for (wcdma_band = RFCOM_BAND_IMT; wcdma_band < RFCOM_NUM_WCDMA_BANDS; wcdma_band++)
  {
    /*Check if any of the bands is supported.  */
    is_any_of_the_bands_supported |= rfwcdma_core_is_band_supported(wcdma_band, RF_PATH_0);
  }

  return (is_any_of_the_bands_supported);

}



#ifdef FEATURE_RF_HAS_QTUNER
/*============================================================================

FUNCTION
rfwcdma_mc_program_qfe_scenario

DESCRIPTION
This API will be called to program the tuner immediately and no script or FW
        action is required. Inside this API check all the active paths then give a call to
        tuner manager associated with appropriate tuners

DEPENDENCIES
        None

RETURN VALUE
        Flag indicating if the api operation was successful (TRUE) or failed (FALSE)

SIDE EFFECTS
        None
=============================================================================*/

boolean rfwcdma_mc_program_qfe_scenario(void)
{
  boolean status;

  status = rfwcdma_core_antenna_tuner_program_qfe_scenario();

  if (status)
  {
    RF_MSG_1(RF_HIGH, "ANT_TUNER: rfwcdma_mc_program_qfe_scenario done !", 0);
  }

  return status;
}
#endif /* FEATURE_RF_HAS_QTUNER */
/*----------------------------------------------------------------------------*/
/*!
        @brief
        Get the W2W neighbor physical device index to support DBDC

        @details
        This function will return the physical device index that map to the device index in the LNA/LPM FW interface
        which is tuned for W2W measurement home/neighbor
        Example
        DBDC Primary band: B1 -> LNA/LPM device 0
        DBDC Secondary band: B8 -> LNA/LPM device 1
        Neighbor Band: B5 -> B5 will be tune on the same physical device as B8, return index 1
        Neighbor Band: B2 -> B2 will be tune on the same physical device as B1, return index 0


        @param chan
        neighbor cell ARFCN

        @param device
        neighbor cell RFM device

        @return
                physical device index for the home/neighbor pair
                return 0xFF if no valid device index can be found

*/
uint8 rfwcdma_mc_get_nbr_phy_dev_ind(uint16 chan, rfm_device_enum_type device)
{
  uint8 carrier_idx, dev_idx = 0;
  rfdevice_trx_wcdma_rx * *home_wcdma_device_ptr,**nbr_wcdma_device_ptr = NULL;
  rfdevice_rxtx_common_class *home_device_instance_ptr, *nbr_device_instance_ptr = NULL;
  rfwcdma_mc_car_path_state_type *temp_state_ptr = NULL;
  rfcom_wcdma_band_type nbr_band = rfwcdma_core_convert_chan_to_band(chan);

  RF_MSG_2(RF_HIGH, " rfwcdma_mc_get_nbr_phy_dev_ind for arfcn:%d, device:%d", chan, device);

  if (rfwcdma_mc_state.rx_multi_carrier_idx[RFM_DEVICE_2] == 0)
  {
    /* If DBDC is not enabled, neighbor device index is always 0 */
    RF_MSG_1(RF_HIGH, " Secondary band carrier not enabled, neighbor device index is 0", 0);
    return 0;
  }


  /* Get the device instance of neighbor */
  nbr_wcdma_device_ptr = (rfdevice_trx_wcdma_rx **)rfc_wcdma_get_device(device,
                         RFC_CONFIG_RX,
                         nbr_band,
                         RFDEVICE_TRANSCEIVER);

  /* Null pointer check */
  if ((nbr_wcdma_device_ptr != NULL) && (nbr_wcdma_device_ptr[0] != NULL))
  {
    /* Get critical section lock from the device instance */
    nbr_device_instance_ptr =
      rfdevice_wcdma_rx_get_common_device_pointer(nbr_wcdma_device_ptr[0]);
  }
  else
  {
    RF_MSG_1(RF_ERROR, "failed to get valid device instance for the neighbor", 0);
    return 0xFF;
  }


  for (carrier_idx = 0; carrier_idx < RFCOM_MAX_CARRIERS; carrier_idx++)
  {
    temp_state_ptr = &rfwcdma_mc_state.car_path_state[RF_PATH_0][carrier_idx];

    /* Check if the given carrier is in the right state */
    if ((temp_state_ptr->rf_state == RFWCDMA_MC_STATE_RX) || (temp_state_ptr->rf_state == RFWCDMA_MC_STATE_RXTX))
    {
      /* Get the device instance of home */
      home_wcdma_device_ptr = (rfdevice_trx_wcdma_rx **)rfc_wcdma_get_device(temp_state_ptr->rx_rfm_device,
                              RFC_CONFIG_RX,
                              temp_state_ptr->curr_band_rx,
                              RFDEVICE_TRANSCEIVER);
      /* Null pointer check */
      if ((home_wcdma_device_ptr != NULL) && (home_wcdma_device_ptr[0] != NULL))
      {
        /* Get critical section lock from the device instance */
        home_device_instance_ptr =
          rfdevice_wcdma_rx_get_common_device_pointer(home_wcdma_device_ptr[0]);

        if ((rfdevice_compare_common_rxtx_devices(home_device_instance_ptr, nbr_device_instance_ptr))) //*(home_device_instance_ptr) == *(nbr_device_instance_ptr))
        {
          /* Matching device instance for home and neighbor found */
          dev_idx = (uint8)rfwcdma_core_util_device_to_lna_id(temp_state_ptr->rx_rfm_device, RFLM_WCDMA_HOME_MEAS);
          RF_MSG_2(RF_HIGH, "physical device index:%d returned for rfm device:%d", dev_idx, temp_state_ptr->rx_rfm_device);
          return dev_idx;
        }
      }
    }
  }

  RF_MSG_1(RF_ERROR, " w2w failed to found the matching device index from the state machine!", 0);
  return 0xFF;
}

/*----------------------------------------------------------------------------*/
/*!
        This function sets the wcdma use fb path flag

        @params - val
*/
void rfwcdma_mc_set_use_fb_path_flag
(
  boolean val
)
{
  rfwcdma_ftm_use_fb_path = val;
}

/*----------------------------------------------------------------------------*/
/*!
This function returns the wcdma use fb path flag

*/
boolean rfwcdma_mc_get_use_fb_path_flag
(
  void
)
{
  return (rfwcdma_ftm_use_fb_path);
}
/*----------------------------------------------------------------------------*/
/*!
        @brief
                Get the Rx AGC value for the specified carrier.

        @details
                This function will return the RxAGC value from FW for the specified carrier ID and in desired TYPE and UNIT.
                We directly query the MDSP layer and get the value.

        @param device
                RFM device

        @param Pointer to data
                This would contain the Carrier ID , unit and type of the value

        @return rxagc_val
                RxAGC value for the specified carrier ID in desired UNIT and TYPE
        */

int16 rfwcdma_mc_get_rxagc
(
  rfcom_device_enum_type device,
  rfwcdma_mc_rxagc_type *data
)
{
  int16 rxagc_val;
  rf_path_enum_type path;
  rfagc_receive_chain_type rx_chain;
  rf_path_enum_type primary_path;

  /*! Convert rfcom_device_enum_type to rf_path_enum_type */
  path = rfcommon_core_device_to_path(device);

  /* Get primary chain config from mc state */
  primary_path = rfwcdma_mc_state.primary_path;

  /*! If it is diversity only call, change the path to diversity */
  if ((path == RF_PATH_0) && (primary_path == RF_PATH_1))
  {
    path = RF_PATH_1;
  }
 
  rx_chain = rfcommon_core_path_to_receive_chain(path);

  /* Get the RxAGC value from MDSP */
  (void)rfwcdma_core_util_get_rxagc(rx_chain,data->type, data->carr_id,data->unit, &rxagc_val);

  return rxagc_val;
}
/*============================================================================*/
/*!
  @name WCDMA Multi carrier

  @brief
  This section includes interfaces pertaining to Multi carrier WCDMA
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the 3C RF status

  @details
  This function will get the status of RF 3C tune

  @param device
   RFM Device to be used

  @return wcdma_3c_active
   Status of the 3C tune per RF path

*/
boolean
rfwcdma_get_tri_carrier_status
(
  rfcom_device_enum_type device
)
{
  rf_path_enum_type path;

  path = rfcommon_core_device_to_path(device);

  return(wcdma_3c_active[path]);

} /* rfwcdma_get_tri_carrier_status */

/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RF MC state
  @details
  This function will return a copy of the MC state structure
  @param

  @return rfwcdma_mc_state_copy
  Current MC state variable
*/

rfwcdma_mc_state_type* rfwcdma_mc_get_state()
{
  rfwcdma_mc_state_type *rfwcdma_mc_state_copy =NULL;

  rfwcdma_mc_state_copy = &rfwcdma_mc_state;

  return rfwcdma_mc_state_copy;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Map device as primary rx or diversity rx device
  @details
  This function will find out if the passed device is primary rx or diversity rx 
  device and update th mc state machine accordingly 

  @param device
  Current rfm_device enum type device on which enter mode is called 
  @return 
  
*/
boolean rfwcdma_mc_map_rx_device(rfm_device_enum_type rx_device)
{
  rf_path_enum_type path =0; 
  rfcom_multi_carrier_hspa_id_type carrier =0; 
  boolean is_primary = TRUE; 
  uint8 enabled_devices =0; 
  boolean map_status = TRUE; 



  for (path  =RF_PATH_0; path < RF_PATH_MAX; path ++)
  {
    for (carrier = RFCOM_SINGLE_CARRIER; carrier < RFCOM_MAX_CARRIERS; carrier ++ )
    {
      if (rfwcdma_mc_state.car_path_state[path][carrier].rf_state == RFWCDMA_MC_STATE_POWERUP_INIT)
      {
        is_primary &= TRUE; 
      }
      else 
      {
        is_primary &= FALSE;
        enabled_devices += 1;
      }
    }
  }

  if (last_device == rx_device)
  {
    RF_MSG_1(RF_HIGH,"rfwcdma_mc_map_rx_device: device %d already mapped ", rx_device );
    map_status = FALSE; 
  }

  else
  {
      if (is_primary == TRUE && enabled_devices == 0)
      {
        rfwcdma_mc_state.primary_rx_device = rx_device;

        if (rfwcdma_mc_state.primary_rx_device == RFM_DEVICE_0)
        {
          rfwcdma_mc_state.secondary_rx_device = RFM_DEVICE_1; 
        }
        else if (rfwcdma_mc_state.primary_rx_device == RFM_DEVICE_2)
        {
          rfwcdma_mc_state.secondary_rx_device = RFM_DEVICE_3;
        }

        RF_MSG_2(RF_HIGH, "rfwcdma_mc_map_rx_device: Identified PRx device: %d, DRx device %d", rx_device,rfwcdma_mc_state.secondary_rx_device ); 
      }
      else 
      {
        RF_MSG_2(RF_HIGH, "rfwcdma_mc_map_rx_device: %d devices enabled, passed device %d not primary or secondary ",enabled_devices, rx_device); 
      }
  }
  
  last_device = rx_device; 
  return map_status; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Validate the RF WCDMA State

  @details
  This function compares the WCDMA internal variables against the expected state
  to indicate the calling RFM function if it is safe to proceed with execution
  of the RFM functionality.

  The return value of this API is not a boolean, as this API can provide more
  info on the cause of the failure (invalid mode, state, etc).

  @param device
  The RF device on which the validation is performed.

  @param exp_state
  The expected RF state.

  @return
  Status of RF state. Negative value refers to bad state.
*/
rfm_api_status_t
rfwcdma_mc_validate_state
(
  rfwcdma_device_mask_type device_mask,
  rfwcdma_mc_state_enum_type exp_state
)
{
  rfm_mode_enum_type curr_mode = RFM_IMT_MODE;
  rfm_api_status_t return_val = RFM_WCDMA_HEALTHY_STATE;
  rf_path_enum_type path =0;
  uint32 num_devices =0;
  rfm_device_enum_type device =0;
  boolean device_state_valid = TRUE;
  rfcom_multi_carrier_hspa_id_type carrier_index =0;

  num_devices= device_mask;

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_validate_state: Invalid device %d", device);
      return 0;
    }
    /* Get Current Mode */
    curr_mode =  rfm_get_current_mode(device);

    /* Validate the current Mode */
    if ( curr_mode != RFM_IMT_MODE )
    {
      RF_MSG_2( RF_ERROR, "rfwcdma_mc_validate_state: Device %d - Not in IMT mode, current mode %d ",
                device, curr_mode );
      return RFM_WCDMA_ERROR_INVALID_MODE;
    } /* if ( curr_mode != RFM_1X_MODE ) */

    /* perform device to Path mapping */
    path = rfcommon_core_device_to_path(device);

    /* Carrier index is always used as 0. This means that state validation in case of DC scenario, 
       we always check the state of carrier 0 and appropriate path*/

    RF_MSG_3( RF_HIGH, "rfwcdma_mc_validate_state: Device %d, carrier index %d - mode %d ",
              device, carrier_index, rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state );


    /* Validate, if the state correct */
    if ( rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state != exp_state )
    {
      device_state_valid = FALSE;
      return_val = RFM_WCDMA_ERROR_BAD_STATE;
    } /* if ( rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state != exp_state ) */
    else
    {
      return_val = RFM_WCDMA_HEALTHY_STATE;
    } /* if!( rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state != exp_state ) */

    if (return_val == RFM_WCDMA_HEALTHY_STATE)
    {
      device_state_valid &= TRUE;
    }

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }


  RF_MSG_5( RF_HIGH, "rfwcdma_mc_validate_state : Curr(Dev Mask %d, Mode %d, "
            "State %d), Expected(Mode %d, State %d)",
            device_mask,
            curr_mode,
            rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state,
            RFM_IMT_MODE,
            exp_state );

  return return_val;

} /* rfwcdma_mc_validate_state */

/*! @endcond */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create device scripts for ASM, GRFC and WTR

  @details
  based on the device information passed this routine would create scripts for ASM/
  Tuners/ GRFC and WTR. These routine would be called for FR2001 scenario.

  @param device
  Device for which device scripts need to be created

  @param band
  WCDMA operational band on the device

  @param channel_list
  List of channels for the tune

  @param rx_device_buffer
  RF buffer array passed my the caller for populating the device scripts.

  @param num_chan
  Parameter defining the number of channels for the tune.

  @param source_band
  Parameter to indicate the current band tuned

  @param disable_ind
  Parameter defining the ASM disable operation type

 */
void rfwcdma_mc_create_device_scripts
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 channel_list[4],
  rf_buffer_intf *rx_device_buffer[],
  rfcom_multi_carrier_hspa_id_type num_chan,
  rfcom_wcdma_band_type source_band,
  rfwcdma_core_rxctl_dis_first_ind_type disable_ind,
  rfdevice_wcdma_spur_param_type* dyn_notch_filter_settings
)
{
  uint32 path =0;
  rfc_grfc_device_type irat_device_info;

  rfdevice_wcdma_rx_tune_param_type rx_params = {0};

#ifdef FEATURE_RF_ASDIV 
  rfwcdma_asdiv_asm_script_data xsw_script_data;
  rfc_cfg_params_type asdiv_cfg; 
  rfc_device_info_type *device_info_ptr; 
  /* Populate params */
  xsw_script_data.device = device;
  xsw_script_data.band= band;
  xsw_script_data.immediate=FALSE;
#endif

  /* Perform Device to Path mapping */
  path = rfcommon_core_device_to_path(device);

  rx_params.band = band;

  if (num_chan == 0)
  {
    rx_params.bw   = RFDEV_WCDMA_BW_INVALID;
    rx_params.channel[0] = channel_list[0];
  }
  else if (num_chan == 1)
  {
    rx_params.bw   = RFDEV_WCDMA_BW_2X;
    rx_params.channel[0] = channel_list[0];
    rx_params.channel[1] = channel_list[1];

  }
  else if (num_chan ==2 )
  {
    rx_params.bw   = RFDEV_WCDMA_BW_3X;
    rx_params.channel[0] = channel_list[0];
    rx_params.channel[1] = channel_list[1];
    rx_params.channel[2] = channel_list[2];

  }

  RF_MSG_4(RF_HIGH, "rfwcdma_mc_create_device_scripts: Parameters - device: %d path: %d, band: %d,"
           "Channel 0 %d",device,path, band, rx_params.channel[0]);


  /* Path validation */
  if ( path > RF_PATH_1 )
  {
    RF_MSG_2( RF_ERROR, "rfwcdma_mc_prep_wakeup_rx: Device %d, "
              "Invalid Path %d mapping",
              device,
              path );
    return;
  } /* if ( path > RF_PATH_1 ) */


  irat_device_info.primary_rx_device = RFM_DEVICE_0;
  if(path == RF_PATH_0)
  {
    irat_device_info.secondary_rx_device = RFM_INVALID_DEVICE;
  }
  else
  {
    irat_device_info.secondary_rx_device = RFM_DEVICE_1;
  }

  /* If a different band is tuned on the device that was tuned before, then disable WTR on the previous band
  for that device */
  if ( (source_band != RFCOM_BAND_INVALID) && (source_band != band) ) 
  {
    rfdevice_wcdma_rx_disable(device,
                              source_band,
                              RFDEVICE_CREATE_SCRIPT,
                              rx_device_buffer[RFWCDMA_MC_WTR_SCRIPTS]);
  }

  /* Configure ASM Rx */
  rfwcdma_core_rxctl_disable_sleep_asm(device,
                                       source_band,
                                       rx_device_buffer[RFWCDMA_MC_ASM_TUNER_SCRIPTS],
                                       RFDEVICE_CREATE_SCRIPT,
                                       RFCOM_INVALID_TIMING_OFFSET,
                                       disable_ind);
  rfwcdma_core_rxctl_enable_asm(device,
                                band,
                                rx_device_buffer[RFWCDMA_MC_ASM_TUNER_SCRIPTS],
                                RFDEVICE_CREATE_SCRIPT,
                                RFCOM_INVALID_TIMING_OFFSET);

  /* Build GRFC script */
  rfc_meas_generate_grfc_script( RFM_IMT_MODE,
                                 band,
                                 &irat_device_info,
                                 rx_device_buffer[RFWCDMA_MC_GRFC_SCRIPTS],
                                 FALSE);

  #ifdef FEATURE_RF_ASDIV 
  /*Check if the RFC has support for AsDiv before building scripts*/ 
  asdiv_cfg.alternate_path = 0;
  asdiv_cfg.band = band; 
  asdiv_cfg.logical_device = device;   
  asdiv_cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1; 
  if(rfc_wcdma_data_get_device_info(&asdiv_cfg,&device_info_ptr)==TRUE) 
  { 
  rfwcdma_asdiv_create_asm_script(rfcommon_asdiv_get_current_position(xsw_script_data.device),rx_device_buffer[RFWCDMA_MC_ASM_TUNER_SCRIPTS],(void*)&(xsw_script_data));
  }
  #endif


#ifdef FEATURE_RF_HAS_QTUNER

  /* Enable both PRx and DRx tuners during wakeup so that when
  doing IRAT/meas the init scripts for DRx need not be generated and executed.
  This is to avoid adding burden to IRAT timeline when enabling
  DRx for IRAT/meas (e.g. X2L).
  During initialization using the same band for Path-0 and Path-1,
  as Path-1 state machine may not be initialized in this context */

  /* Program the tuner in Rx mode*/
  rfwcdma_core_rxctl_disable_tuner(device,
                                   source_band,
                                   RFDEVICE_CREATE_SCRIPT,
                                   rx_device_buffer[RFWCDMA_MC_ASM_TUNER_SCRIPTS],
                                   RFCOM_INVALID_TIMING_OFFSET,
                                   disable_ind);
  rfwcdma_core_rxctl_enable_tuner(device,
                                  band,
                                  rx_params.channel[0],
                                  RFDEVICE_CREATE_SCRIPT,
                                  rx_device_buffer[RFWCDMA_MC_ASM_TUNER_SCRIPTS]);

#endif /* FEATURE_RF_HAS_QTUNER */

  rfwcdma_set_tri_carrier_status(device,num_chan);

  /* If dynamic spur update is not needed, then this param will be NULL */
  if (dyn_notch_filter_settings == NULL) 
  {
  rfdevice_wcdma_rx_set_channel( device,
                                 RFCOM_WCDMA_MODE,
                                 band,
                                 rx_params,
                                 RFDEVICE_CREATE_SCRIPT,
                                 rx_device_buffer[RFWCDMA_MC_WTR_SCRIPTS], 
                                 NULL, NULL, FALSE );
  }

  else
  {
    rfdevice_wcdma_rx_set_channel( device,
                                   RFCOM_WCDMA_MODE,
                                   band,
                                   rx_params,
                                   RFDEVICE_CREATE_SCRIPT,
                                   rx_device_buffer[RFWCDMA_MC_WTR_SCRIPTS], 
                                   rfwcdma_core_rxctl_update_dynamic_notch_cb, 
                                   dyn_notch_filter_settings, TRUE);
  }
  

}/*rfwcdma_mc_create_scripts*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Wakeup Rx sequence with immediate write to turn on the radio and tune
  to the given band and channel.

  @details
  This function will send a command to firmware to indicate that Rx needs to be
  started. The command will indicate FW to activate the Script.

  @param device
  The receive path to wakeup.

  @param rxlm_buf_idx
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfwcdma_mc_exec_wakeup_rx_imm
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_buf_idx
)
{
  rf_path_enum_type path = rfcommon_core_device_to_path(device);
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  boolean wcdma_device_vote;
  timetick_type prof_cal_func;
  boolean pwr_vote_status = TRUE;
  uint16 pll_settling_time = 200;
  rf_buffer_intf *buffer_ptr = NULL;
  rfdevice_wcdma_rx_tune_param_type rx_params;
  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };
  uint32 temp_rxlm_handle[RFM_DEVICE_4] = {0};
  uint32 alt_path = 0;

  rx_params.bw   = RFDEV_WCDMA_BW_INVALID;
  rx_params.band = RFCOM_BAND_INVALID;

  if(device >= RFM_DEVICE_4)
  {
    RF_MSG_1(RF_ERROR, "rfwcdma_mc_exec_wakeup_rx_imm :Invalid device %d",device);
    return 0;
  }

  if ( enable_wakeup_optimization_flag == TRUE )
  {
    //ERR_FATAL( "********* DISABLED rfwcdma_mc_wakeup *********", 0, 0, 0 );
    RF_MSG(RF_HIGH," rfwcdma_mc_wakeup_rx_imm: WL1 still using the non-optimized wakeup sequence");
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  /* Update Alt path to RFC*/
  rfwcdma_core_rxctl_update_alt_path(device, 
                                     rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx, 
                                     FALSE,
                                     &alt_path);

  rfwcdma_mdsp_sync_config_ccs(rfwcdma_mc_state.primary_rx_device,TRUE);

  /* Create rf_buf */
  rfwcdma_mc_create_rf_buff(&buffer_ptr);

  if (rfwcdma_mc_state.car_path_state[path][0].rf_state != RFWCDMA_MC_STATE_SLEEP)
  {
    RF_MSG_2(RF_HIGH, "Wakeup from Non-Sleep state - Rx path: %d state: %d",
             path, rfwcdma_mc_state.car_path_state[path][0].rf_state);

    /*
                    If RF is not in sleep state, only send start AGC if needed
                    Following conditions need to be met to send start AGC command to avoid sending back to back AGC start command
                    1. FW RxAGC ON status flag need to be OFF
                    2. RF internal RxAGC enable flag need to be FALSE
    */

    if (rfwcdma_core_util_is_rxagc_enabled((rfagc_receive_chain_type)path, RFCOM_MULTI_CARRIER_ID__0) == FALSE)
    {
      /* Send wakeup AGC cmd to FW */
      cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
      temp_rxlm_handle[device] = rxlm_buf_idx;

      rfwcdma_core_rxctl_enable_rxagc(rfwcdma_mc_state.rx_multi_carrier_idx[device],
                                      cmd_action,
                                      temp_rxlm_handle,
                                      FALSE,0,0);

    }

    /* Destroy rf_buff */
    rfwcdma_mc_destroy_rf_buff(buffer_ptr);

    return 0;
  }

  /* Get core config data from RFC */

  rfdevice_wcdma_rx_cmd_dispatch(RFM_DEVICE_0,
                                 rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx,
                                 RFDEVICE_GET_RX_PLL_SETTLING_TIME, &pll_settling_time);

  RF_MSG_1(RF_LOW, "rfwcdma_mc_wakeup - PLL settling time: %d", pll_settling_time);

  /* Enable VREG if its not ON */
  rfc_wcdma_manage_vregs(RFM_DEVICE_0,
                         rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx,
                         RF_PATH_RX_STATE);


  /* During WCDMA sleep all the RFC signals have been put to init state */
  /* Step1 for RFC: Configure RFC signals in WCDMA mode */
  rfc_common_wakeup(path, RFCOM_WCDMA_MODE);


  /* Vote to power on tranceiver device */
  wcdma_device_vote = rfcommon_core_dev_power_vote(device,
                      RFCMN_CORE_DEV_ON,
                      RFCMN_CORE_DEV_NORMAL_VOTE);

  if (wcdma_device_vote == FALSE)
  {
    RF_MSG_1(RF_FATAL,
             "rfwcdma_mc_wakeup - Device - %d Power up failed", device);
  }

  rfwcdma_mc_pwr_vote_on_off(device,
                             rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                             TRUE,
                             RFM_WCDMA_RX_DEVICE,
                             FALSE
                             );

  /* If wakeup optimization is not enabled, write SSBI registers directly to device and wait for
  PLL to settle */
  if (enable_wakeup_optimization_flag == FALSE)
  {
    /* Clear MC buffer */
    (void)rf_buffer_clear(buffer_ptr);

    /* Configure ASM Rx */
    rfwcdma_core_rxctl_enable_asm(device,
                                  rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                                  NULL,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  RFCOM_INVALID_TIMING_OFFSET);

    rfc_wcdma_wakeup(device);

    /* Step2 for RFC: Configure RFC signals for the band in which
                    RF wakeup needs to be done */
    rfc_wcdma_rx_band_config(device, rfwcdma_mc_state.car_path_state[path][0].curr_band_rx);

#ifdef FEATURE_RF_HAS_QTUNER

    /* Enable both PRx and DRx tuners in at the time of Rx init so that when
                    doing IRAT/meas the init scripts for DRx need not be generated and executed.
                    This is to avoid adding burden to IRAT timeline when enabling
                    DRx for IRAT/meas (e.g. X2L).
                    During initialization using the same band for Path-0 and Path-1, as Path-1 state
                    machine may not be initialized in this context
    */
    /* Program the tuner in Rx mode*/
    rfwcdma_core_rxctl_enable_tuner(device,
        rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
        rfwcdma_mc_state.car_path_state[path][0].curr_chan_rx,
        RFDEVICE_EXECUTE_IMMEDIATE,
        NULL);

#endif /* FEATURE_RF_HAS_QTUNER */


    /*! Tune tranceiver device */

    /* Set rx tune params */
    rx_params.band       = rfwcdma_mc_state.car_path_state[path][0].curr_band_rx;
    rx_params.bw         = RFDEV_WCDMA_BW_1X;
    rx_params.channel[0] = rfwcdma_mc_state.car_path_state[path][0].curr_chan_rx;

    /* Tune Rx PLL */
    rfdevice_wcdma_rx_set_channel( device, RFCOM_WCDMA_MODE, rx_params.band,
                                   rx_params, RFDEVICE_EXECUTE_IMMEDIATE, 
                                   NULL, NULL, NULL, FALSE );

    /* Wait for PLL to settle after tune*/
    DALSYS_BusyWait(pll_settling_time);

    /* Start RxAGC */
    if (rfwcdma_core_util_is_rxagc_enabled((rfagc_receive_chain_type)path, RFCOM_MULTI_CARRIER_ID__0) == FALSE)
    {
      /* Send wakeup AGC cmd to FW */
      cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
      temp_rxlm_handle[device] = rxlm_buf_idx;

      rfwcdma_core_rxctl_enable_rxagc(rfwcdma_mc_state.rx_multi_carrier_idx[device],
                                      cmd_action,
                                      temp_rxlm_handle,
                                      FALSE,0,0);
    }

  }

  /* Enable temp comp */
  if (path == RF_PATH_0)
  {
    rfwcdma_core_temp_comp_enable(TRUE);
  }

  rfwcdma_mc_state.car_path_state[path][0].rf_state = RFWCDMA_MC_STATE_RX;

  RF_MSG_1(RF_HIGH, "rfwcdma_mc_exec_wakeup_rx_imm(device=%d)", device);

  /* Destroy rf_buff */
  rfwcdma_mc_destroy_rf_buff(buffer_ptr);
  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Wakeup -%d", prof_cal_func);
  }
  return ret_val;

}



/*----------------------------------------------------------------------------*/
/*!
@brief
 Enable WCDMA Rx AGC

 @details
 This function will send a command to firmware to indicate that Rx needs to be
 started. The command will indicate FW to activate the Script.

 @param device_mask
 Parameter indicating the devices on which RxAGC needs to be enabled 

 @param rxlm_handle
 The Rx Link Manager buffer that shall be used to configure the
 modem hardware.

 @return
 Variable indicating the status of API execution. The error codes are
 defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfwcdma_mc_enable_rxagc
(
  rfwcdma_device_mask_type device_mask,
  lm_handle_type rxlm_handle[RFWCDMA_MAX_RX_DEVICES]
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  rf_path_enum_type path;
  rfwcdma_data_debug_flags_type *dbg_flag;
  rfwcdma_mdsp_agc_action_type cmd_action[RFWCDMA_MAX_RX_DEVICES] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };
  uint32 temp_rxlm_handle[RFWCDMA_MAX_RX_DEVICES] = {0};
  uint8 num_devices =device_mask;
  rfm_device_enum_type device =0;

  /* Get debug flags */
  dbg_flag = rfwcdma_get_debug_flags();

  if ( dbg_flag->enable_opt_wakeup == FALSE )
  {
    RF_MSG( RF_HIGH, "rfwcdma_mc_enable_rxagc: enable_opt_wakeup = FALSE " );

    //ERR_FATAL( "********* DISABLED rfwcdma_mc_enable_rxagc *********",
    //           0, 0, 0 );
  }

  if(device_mask >= RFM_DEVICE_4)
  {
    RF_MSG_1( RF_ERROR, "rfwcdma_mc_enable_rxagc: invalid device %d", device_mask );
    return RFM_WCDMA_ERROR_FAILURE;
  }

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device >= RFM_DEVICE_0) && (device < RFM_WCDMA_MAX_DEVICE))
    {
      /* Perform Device to Path mapping */
      path = rfcommon_core_device_to_path(device);

      /* Path validation */
      if ( path > RF_PATH_1 )
      {
        RF_MSG_3( RF_ERROR, "rfwcdma_mc_enable_rxagc: Device %d, RxLM %d - "
                  "Invalid Path %d mapping",
                  device,
                  rxlm_handle[device],
                  path  );
        return RFM_WCDMA_ERROR_BAD_PATH_MAPPING;
      } /* if ( path > RF_PATH_1 ) */

      /* Enable RxAGC only if the AGC is not enabled */
      if (rfwcdma_mdsp_get_rxagc_enable_status(
            (rfagc_receive_chain_type)path,
            RFCOM_MULTI_CARRIER_ID__0) == TRUE )
      {
        RF_MSG_3( RF_ERROR, "rfwcdma_mc_enable_rxagc: Device %d, RxLM %d, Path %d - "
                  "AGC is already enabled!",
                  device,
                  rxlm_handle[device],
                  path );
        return RFM_WCDMA_ERROR_FAILURE;
      } /* if(rfwcdma_mdsp_get_rxagc_enable_status == TRUE)*/

      else
      {
        cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
        temp_rxlm_handle[device] = rxlm_handle[device];
      }/* if!(rfwcdma_mdsp_get_rxagc_enable_status == TRUE) */

      num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
    }

    else
    {
      RF_MSG_1( RF_ERROR, "rfwcdma_mc_enable_rxagc: invalid device %d", device );
      return RFM_WCDMA_ERROR_FAILURE;
    }
  }

  rfwcdma_core_rxctl_enable_rxagc(rfwcdma_mc_state.rx_multi_carrier_idx[device],
                                  cmd_action,
                                  temp_rxlm_handle,
                                   FALSE,0,0);
  /* Return the API Wait time */
  return ret_val;
}


/*! @cond vbhide */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return RF Tx bandwidth for uplink multi-carrier/single-carrier fast switching

  @details
  This function will reprogram Tx devices for Tx bandwith change, Tx has to
  be already enabled (rfm_init_wcdma_tx and rfm_enable_tx called). It can be
  called for fast bandwidth switching.

  Scenarios                                RF API Call flows
  Bringup Tx in SC mode                    rfm_init_wcdma_tx(SC)+rfm_enable_tx()
  Configure DC from SC                     rfm_init_wcdma_tx(DC) (non-critcal timeline, lin+txlm)
  Activate DC from DC configured           rfm_wcdma_tx_retune_bw(DC)- move PDA control here
  Deactivate DC to DC configured           rfm_wcdma_tx_retune_bw(SC)
  Re-activate DC with DC configured        rfm_wcdma_tx_retun_bw(DC)
  Deconfigure DC after deactivation        rfm_init_wcdma_tx(SC) (non-critical timeline, lin+txlm)
  Seamless reconfig (direct DC configured) rfm_init_wcdma_tx(DC) + rfm_enable_tx()

  @param device
  RFM device type

  @param multi_carrier_idx
  Tx Multi Carrier Index Bitmask

*/
rfm_api_status_t
rfwcdma_mc_tx_retune_bw
(
  const rfm_device_enum_type device,
  const uint32 multi_carrier_idx
)
{
  uint16 channels[RFCOM_MAX_CARRIERS];
  /* Convert Path from Device */
  rf_path_enum_type path = rfcommon_core_device_to_path(device);

  /* Device current band and channel from state machine. */
  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx;
  uint16 chan0 = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx;
  uint16 chan1 = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__1].curr_chan_tx;

  /* Device Tx Tune params*/
  rfdevice_wcdma_tx_tune_param_type tx_params = {0};

  /* Multi Carrier Type (bandwith) converted from Multi Carrier Index */
  rfcom_multi_carrier_hspa_id_type multi_carrier_type = rfwcdma_core_count_carriers(multi_carrier_idx);

  /* Status return value */
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE;

  /* PA bandwidth parameter */
  rfdevice_pa_bw_enum_type pa_bw = RFDEVICE_PA_BW_NOT_APPLICABLE;

  boolean api_status = TRUE;

  /* Parameter valid range check, Tx supports up to 2 carriers for now */
  if ((band >= RFCOM_NUM_WCDMA_BANDS) || (multi_carrier_type > RFCOM_DUAL_CARRIER) || (path > RF_PATH_1))
  {

    RF_MSG_4(RF_ERROR, "rfm_wcdma_tx_retune_bw, invalid parameters: band:%d, multi_carrier_idx:%d multi_carrier_type:%d, path:%d",
             band,
             multi_carrier_idx,
             multi_carrier_type,
             path);

    ret_val = RFM_WCDMA_ERROR_INVALID_PARAMS;
  }
  else
  {

  /* Prepare for Multi-Carrier config */
  channels[0] = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].curr_chan_tx;
  channels[1] = rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__1].curr_chan_tx;
      /* Config Multi-Carrier */
    rfwcdma_core_txctl_config_mc(rfwcdma_mc_state.car_path_state[path][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx, multi_carrier_idx,
                                 channels,
                                 rfwcdma_mc_state.txlm_buf_idx);
    

    /* Prepare for WTR Tx PLL tune Poluate Device Tx params  */
    /* Get DPD state */
    if (rfwcdma_core_xpt_is_enabled(band))
    {
      tx_params.dpd_state = RFDEVICE_DPD_ENABLED;
    }
    else
    {
      tx_params.dpd_state = RFDEVICE_DPD_DISABLED;
    }

    tx_params.band = band;

    /* Set tx tune params based on dual carrier or single carrier mode */
    if (multi_carrier_type == RFCOM_SINGLE_CARRIER )
    {
      /* Update BW parameter and channels for SC */
      tx_params.bw         = RFDEV_WCDMA_BW_1X;
      tx_params.channel[0] = chan0;
      tx_params.channel[1] = chan0;

      /* Update PA BW */
      pa_bw = RFDEVICE_PA_BW_5MHZ;
    }
    else
    {
      /* Update BW parameter and channels for DC*/
      tx_params.bw         = RFDEV_WCDMA_BW_2X;
      tx_params.channel[0] = chan0;
      tx_params.channel[1] = chan1;

      /* Update PA BW */
      pa_bw = RFDEVICE_PA_BW_10MHZ;
    }

    /* Tune Tx PLL */
    rfdevice_wcdma_tx_set_channel(device, RFCOM_WCDMA_MODE,
                                  tx_params.band, tx_params,
                                  RFDEVICE_EXECUTE_IMMEDIATE, NULL);

    /* Confg PA BW */
    rfdevice_wcdma_pa_config(device,
                             band,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET,
                             pa_bw);


   (void)rfdevice_wcdma_pa_on_off(device,
                                 band,
                                 TRUE,
                                 NULL,
                                 RFDEVICE_EXECUTE_IMMEDIATE,
                                 RFCOM_INVALID_TIMING_OFFSET);

  if (api_status == FALSE)
  {
    ret_val = RFM_WCDMA_ERROR_FAILURE;
  }

  }/*if !((band >= RFCOM_NUM_WCDMA_BANDS) || (multi_carrier_type > RFCOM_DUAL_CARRIER) || (path > RF_PATH_1))*/

  return ret_val;

}
/*! @endcond */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare to turn on the receive radio and tune to the active band and channel
  on the device(s) specified.

  @details
  Prepares the software to move the WCDMA reciever from the Sleep state to the
  Rx state. This function will prepare scripts for the device(s) specified
  to turn on the receiver.The scripts will be created for the band and channel which
  was tuned to earlier during tuning operation or pre-tuning operation. The
  function will not perform any operation related to direct access to the
  hardware, as it is not guaranteed that all the necessary clock are enabled
  while calling this function.

  @param device_mask
  This is a bitmask passed indicating Rx devices which need to be turned on.

  @param rxlm_buf_idx
  The Rx Link Manager buffer array that shall be used to configure the
  modem hardware. the array will hold the rxlm handles for only the specified devices

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfwcdma_mc_rx_prep_wakeup
(
  uint32 device_mask,
  uint32 rxlm_buf_idx[RFWCDMA_MAX_RX_DEVICES]
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  rf_path_enum_type path;
  uint32 execution_time =0;
  uint16 pll_settling_time = 200, channel[RFWCDMA_MAX_RX_DEVICES];
  uint32 warmup_time_us = 0, script_type=0;
  uint32 num_devices =0, valid_scripts =0;
  rfm_device_enum_type device =0;
  uint32 carrier_index =0, alt_path =0;
  uint8 num_chan =0;
  rfwcdma_data_debug_flags_type *dbg_flag;

  rf_buffer_intf *rf_wakeup_script_buffer[RFWCDMA_MC_WAKEUP_SCRIPTS_MAX] = { NULL };

  /* Data structures needed for populating the wakeup scripts in DM */
  rfwcdma_mdsp_rx_wakeup_cfg_type cfg;

  memset(&cfg, 0, sizeof(rfwcdma_mdsp_rx_wakeup_cfg_type));

  /* Get debug flags */
  dbg_flag = rfwcdma_get_debug_flags();

  if ( dbg_flag->enable_opt_wakeup == FALSE )
  {
    RF_MSG( RF_HIGH, "rfwcdma_mc_prep_wakeup_rx: enable_opt_wakeup = FALSE " );
  }

  /* Create RF buffer to populate the script. This buffer will be maintained
  until we populate the DM */
  for ( script_type=0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
  {
    rfwcdma_mc_create_rf_buff(&rf_wakeup_script_buffer[script_type]);

    if (rf_wakeup_script_buffer[script_type]== NULL)
    {
      RF_MSG(RF_ERROR,
             "rfwcdma_mc_rx_prep_wakeup: rf buffer allocation failed!");
      break;
    }
  }

  /* Clear MC buffer */
  for ( script_type =0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
  {
    rf_buffer_clear(rf_wakeup_script_buffer[script_type]);
  }

  num_devices= device_mask;

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_wakeup: Invalid device %d", device);
      /*Destroy local RF buffers*/
      for ( script_type =0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
      {
        rfwcdma_mc_destroy_rf_buff(rf_wakeup_script_buffer[script_type]);
      }
      return 0;
    }

    path = rfcommon_core_device_to_path(device);

    carrier_index =  rfwcdma_mc_state.rx_multi_carrier_idx[device];

    channel[0]= rfwcdma_mc_state.car_path_state[path][0].curr_chan_rx;

    num_chan = rfwcdma_mc_state.rx_carriers[device];

    RF_MSG_5(RF_HIGH, "rfwcdma_mc_prep_wakeup: Parameters - num_devices %d device: %d"
             " path: %d,carrier_index: %d, channel: %d",
             num_devices,device,path, carrier_index,
             rfwcdma_mc_state.car_path_state[path][0].curr_chan_rx);

    /* Update Alt path to RFC*/
    rfwcdma_core_rxctl_update_alt_path(device, 
                                       rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx, 
                                       FALSE,
                                       &alt_path);

    rfwcdma_mc_create_device_scripts(device,
                                     rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                                     channel,
                                     rf_wakeup_script_buffer,
                                     num_chan,
                                     RFCOM_BAND_INVALID,
                                     RFWCDMA_CORE_ASM_TUNER_NO_DISABLE_OPS,
                                     NULL);

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }

  /* Get the WTR script execution time from Device */
  rf_buffer_get_script_execution_time(rf_wakeup_script_buffer[RFWCDMA_MC_WTR_SCRIPTS],
                                      RF_BUFFER_RFFE,
                                      &execution_time);

  /* Get the PLL settling time from Device */
  rfdevice_wcdma_rx_cmd_dispatch( RFM_DEVICE_0,
                                  rfwcdma_mc_state.car_path_state[0][0].curr_band_rx,
                                  RFDEVICE_GET_RX_PLL_SETTLING_TIME,
                                  &pll_settling_time );


  for (script_type=0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++ )
  {
    if (rfwcdma_core_get_resource_script_from_buffer(rf_wakeup_script_buffer[script_type])!= NULL)
    {
      cfg.device_cfg.wakeup[valid_scripts].script= rf_wakeup_script_buffer[script_type];
      cfg.device_cfg.wakeup[valid_scripts].delay= 0;
      valid_scripts ++;
    }
  }

  warmup_time_us = (execution_time/10) + pll_settling_time;
  cfg.device_cfg.rf_warmup_time = WCDMA_CONV_US_TO_USTMR(warmup_time_us);

  RF_MSG_4(RF_HIGH, "rfwcdma_mc_rx_prep_wakeup: [Time Profile] Dev Mask %d:"
           "WTR script execution %d, "
           "PLL %d, Warmup - %d",
           device_mask,
           (execution_time/10),
           pll_settling_time,
           cfg.device_cfg.rf_warmup_time );

  num_devices = rf_count_one_bits(device_mask);

  if (num_devices == 1)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(device_mask);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_wakeup: Invalid device %d", device);
      return 0;
    }

    cfg.rflm_handle= rxlm_buf_idx[device];
  }

  else
  {
    cfg.rflm_handle= rxlm_buf_idx[0];
  }

  /* Appends tasks which have valid scripts only*/
  cfg.device_cfg.num_tasks_wakeup_script = valid_scripts;

  rfwcdma_mdsp_async_config_rx_wakeup (&cfg);

  num_devices = device_mask;

  /* Update State information */
  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_wakeup: Invalid device %d", device);
      /*Destroy local RF buffers*/
      for ( script_type =0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
      {
        rfwcdma_mc_destroy_rf_buff(rf_wakeup_script_buffer[script_type]);
      }
      return 0;
    }

    path = rfcommon_core_device_to_path(device);
    carrier_index = rfwcdma_mc_state.rx_carriers[device];
    RF_MSG_4(RF_HIGH,"rfwcdma_mc_rx_prep_wakeup: MC state update - device: %d, path: %d,"
             "carrier index: %d number of devices: %d",
             device, path, carrier_index, num_devices);
    rfwcdma_mc_state.car_path_state[path][0].rf_state = RFWCDMA_MC_STATE_RX_PREP;
    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }

  RF_MSG_3(RF_HIGH, "rfwcdma_mc_rx_prep_wakeup: Dev Mask %d:"
           "Scripts pushed to DM %d, "
           "RxLM buffer index %d",
           device_mask,
           valid_scripts,
           cfg.rflm_handle);

  /*Destroy local RF buffers*/
  for ( script_type =0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
  {
    rfwcdma_mc_destroy_rf_buff(rf_wakeup_script_buffer[script_type]);

  }

  /* Return the API Wait time */
  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Wakeup Rx sequence to turn on the radio and tune to the given band
  and channel on the passed device(s).

  @details
  This function will send a command to firmware to indicate that Rx needs to be
  started. The command will indicate FW to activate the Script.

  @param device_mask
  This is a bitmask passed indicating Rx devices which need to be turned on.

  @param rxlm_buf_idx
  The Rx Link Manager buffer array that shall be used to configure the
  modem hardware. the array will hold the rxlm handles for only the specified devices

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfwcdma_mc_rx_exec_wakeup
(
  uint32 device_mask,
  uint32 rxlm_buf_idx[RFWCDMA_MAX_RX_DEVICES]
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  rf_path_enum_type path;
  boolean wcdma_device_vote;
  timetick_type prof_cal_func;
  uint32 temp_rxlm_handle[RFM_DEVICE_4] = {0};
  uint32 num_devices =0, multicarrier_idx =0;
  rfm_device_enum_type device =0;
  uint32 carrier_index =0;
  boolean pwr_vote_status = TRUE;
  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  num_devices= device_mask;

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_exec_wakeup: Invalid device %d", device);
      return 0;
    }
    path = rfcommon_core_device_to_path(device);
    multicarrier_idx |= rfwcdma_mc_state.rx_multi_carrier_idx[device];
    carrier_index =  rfwcdma_mc_state.rx_carriers[device];

    RF_MSG_5(RF_HIGH, 
             "rfwcdma_mc_rx_exec_wakeup: Parameters - num_devices %d device: %d path: %d, carrier_index: %d, multicarrier_idx: %d",
             num_devices,
             device,
             path, 
             carrier_index, 
             multicarrier_idx
             );

    rfwcdma_mdsp_sync_config_ccs(rfwcdma_mc_state.primary_rx_device,TRUE);

    /* Path validation */
    if ( path > RF_PATH_1 )
    {
      RF_MSG_3( RF_ERROR, 
                "rfwcdma_mc_rx_exec_wakeup: Device %d, RxLM %d - Invalid Path %d mapping",
                device,
                rxlm_buf_idx[device],
                path  );
      return RFM_WCDMA_ERROR_BAD_PATH_MAPPING;
    } /* if ( path > RF_PATH_1 ) */


    /* During WCDMA sleep all the RFC signals have been put to init state */
    /* Step1 for RFC: Configure RFC signals in WCDMA mode */
    rfc_common_wakeup( path, RFCOM_WCDMA_MODE );

    /* Vote to power on tranceiver device */
    wcdma_device_vote = rfcommon_core_dev_power_vote(device,
                          RFCMN_CORE_DEV_ON,
                                                     RFCMN_CORE_DEV_NORMAL_VOTE 
                                                     );

#ifdef T_RUMI_EMULATION
    wcdma_device_vote = TRUE;
#endif
    if ( wcdma_device_vote == TRUE )
    {
      /* Send wakeup AGC cmd to FW */
      cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
      temp_rxlm_handle[device]= rxlm_buf_idx[device];

      /* Enable temp comp */
      if (path == RF_PATH_0)
      {
        rfwcdma_core_temp_comp_enable(TRUE);
      }

      /* Update State information */
      rfwcdma_mc_state.car_path_state[path][0].rf_state =
        RFWCDMA_MC_STATE_RX;
    }
    else
    {
      ret_val = RFM_WCDMA_ERROR_FAILURE;
      RF_MSG_4( RF_ERROR, "rfwcdma_mc_rx_exec_wakeup: [FAILURE %d ]Dev %d, "
                "RxLm %d, RxChan %d - Power voting failed",
                ret_val,
                device,
                rxlm_buf_idx[device],
                rfwcdma_mc_state.car_path_state[path][0].curr_chan_rx );

    } /* if ! ( wcdma_device_vote == TRUE ) */

    rfwcdma_mc_pwr_vote_on_off(device, 
                               rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                               TRUE,
                               RFM_WCDMA_RX_DEVICE,
                               FALSE
                               );

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));

  }/*while (num_devices != 0)*/

  /* Send wakeup AGC cmd to FW */
  rfwcdma_core_rxctl_enable_rxagc(multicarrier_idx,
                                  cmd_action,
                                  temp_rxlm_handle,
                                  TRUE,0,0);
  RF_MSG_2( RF_HIGH, "rfwcdma_mc_rx_exec_wakeup: Device Mask %d, "
            "Multi_carrier %d",
            device_mask,
            multicarrier_idx);

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Rx_exec_wakeup - %d", prof_cal_func);
  }
  /* Return the API Wait time */
  return 0;

} /* rfwcdma_mc_rx_exec_wakeup*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the receiver.

  @details
  Moves the WCDMA radio from the Rx state to the Sleep state.  Any circuits
  that are no longer needed are powered down to conserve current.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_WCDMA_RX_SLEEP_COMPLETE event.

  @param device_mask
  This is a bitmask passed indicating Rx devices which need to be turned on.

  @param skip_agc_disable
  Device list on which AGC disable needs to be skipped during the sleep operation

  @param rxlm_buf_idx
  The Rx Link Manager buffer array that shall be used to configure the
  modem hardware. the array will hold the rxlm handles for only the specified devices

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/

rfm_api_status_t
rfwcdma_mc_rx_sleep
(
  uint32 device_mask,
  boolean skip_agc_disable[RFWCDMA_MAX_RX_DEVICES], 
  uint32 rxlm_buf_idx[RFWCDMA_MAX_RX_DEVICES]
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  boolean pwr_vote_status = TRUE;
  timetick_type prof_cal_func;
  rf_path_enum_type path = RF_PATH_0;
  boolean wcdma_device_vote = FALSE, send_agc_disable_cmd = FALSE; 
  rfwcdma_data_debug_flags_type *dbg_flag;
  uint8 device_counter =0; 
  uint8 car_idx = 0;
  rfcom_wcdma_band_type source_band = RFCOM_BAND_INVALID; 

  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };

  uint32 temp_rxlm_idx[RFM_DEVICE_4] = {0};

  uint8 num_devices =0, multicarrier_idx  =0;
  rfm_device_enum_type device =0;

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  /* Get debug flags */
  dbg_flag = rfwcdma_get_debug_flags();

  if ( dbg_flag->enable_opt_wakeup == FALSE )
  {
    RF_MSG( RF_HIGH, "rfwcdma_mc_sleep_rx: enable_opt_wakeup = FALSE " );
  }

  num_devices= device_mask;

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_sleep: Invalid device %d", device);
      return 0;
    }

    path = rfcommon_core_device_to_path(device);
    multicarrier_idx |= rfwcdma_mc_state.rx_multi_carrier_idx[device];

    /* Put Transceiever Rx and Tx to low power mode */
    source_band = rfwcdma_mc_get_source_band(device);
    rfdevice_wcdma_rx_disable(device,
                              source_band,
                              RFDEVICE_EXECUTE_IMMEDIATE,
                              NULL);

    /* Put the ASM to sleep */
    rfwcdma_core_rxctl_disable_sleep_asm(device,
                                         rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                                         NULL,
                                         RFDEVICE_EXECUTE_IMMEDIATE,
                                         RFCOM_INVALID_TIMING_OFFSET,
                                         RFWCDMA_CORE_ASM_TUNER_DIS_ALL_BANDS);

/*Disable tuner on the device passed*/
#ifdef FEATURE_RF_HAS_QTUNER
	  /* Disable the tuners in use for PATH-0*/
    rfwcdma_core_rxctl_disable_tuner(device,
	   			     rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
 				     RFDEVICE_EXECUTE_IMMEDIATE,
				     NULL,
                 		     RFCOM_INVALID_TIMING_OFFSET,
				     RFWCDMA_CORE_ASM_TUNER_DIS_ALL_BANDS);
#endif

    /* Deallocate any Common Notches allocated*/
    (void)rfwcdma_msm_cmn_notch_filter_deallocate(device, rxlm_buf_idx[device]);

 
     rfwcdma_mc_pwr_vote_on_off(device,
                                rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                                FALSE,
                                RFM_WCDMA_RX_DEVICE,
                                FALSE
                                );


    wcdma_device_vote = rfcommon_core_dev_power_vote( device,
                        RFCMN_CORE_DEV_OFF,
                        RFCMN_CORE_DEV_NO_VOTE);

    if (wcdma_device_vote == FALSE)
    {
      RF_MSG_1(RF_LOW, "RF Device has already been turned OFF - %d", device);
    }

    /* Stop AGC on either path only if it's still running, otherwise
    L1 might already put FW to sleep and this will crash FW, there
    are some other occasions where L1 will do early RF shut down where
    RFM sleep gets called without FW sleep */

    
    if ((rfwcdma_core_util_is_rxagc_enabled((rfagc_receive_chain_type)path, RFCOM_MULTI_CARRIER_ID__0) == TRUE)
        || (rfwcdma_mc_state.cpc_state_flag == TRUE))
    {
      if (skip_agc_disable[device] != TRUE)
    {
      cmd_action[device] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
      temp_rxlm_idx[device] = rxlm_buf_idx[device];
    }
      else 
        RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_sleep: AGC disable skipped on device %d", device);
    }

    /* put the RFC Signals into deafult state */
    rfc_wcdma_sleep(device);

    if ((device == rfwcdma_mc_state.primary_rx_device) ||  (device == rfwcdma_mc_state.secondary_rx_device))
    {
      /* Vote to put GPIOs to input state */
      rfc_common_sleep(path, RFCOM_WCDMA_MODE);
    }

    /* Disable some items when primary path is put to sleep, however
      they should remain turned on when diversity path is put to sleep */
    if(device == rfwcdma_mc_state.primary_rx_device)
    {
      /* Disable temp comp */
      rfwcdma_core_temp_comp_enable(FALSE);

      rf_hal_bus_enable(FALSE, &rf_wcdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(WCDMA));
    }

    if ((device != rfwcdma_mc_state.primary_rx_device) && (device != rfwcdma_mc_state.secondary_rx_device))
    {
      rfwcdma_mc_reset_state_machine_rx(device);
    }
    else
    {
      for (car_idx = RFCOM_SINGLE_CARRIER ; car_idx<RFCOM_MAX_CARRIERS; car_idx++)
      {
        if (((rfwcdma_mc_state.rx_multi_carrier_idx[device])&(1<<car_idx))!=0)
        {
      /* Update the MC state to "Sleep" */
          rfwcdma_mc_state.car_path_state[path][car_idx].rf_state = RFWCDMA_MC_STATE_SLEEP;
        }
      }

    }

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));

  }/* while (num_devices != 0)*/

  for(device_counter = 0; device_counter < RFWCDMA_MAX_RX_DEVICES; device_counter++)
  {
    if(cmd_action[device_counter] != RFWCDMA_MDSP_AGC_CMD_INVALID)
    {
      send_agc_disable_cmd = TRUE;
      break;
    }
  }

  if(send_agc_disable_cmd) 
  {
  rfwcdma_core_rxctl_enable_rxagc(multicarrier_idx,
                                  cmd_action,
                                  temp_rxlm_idx,
                                  FALSE,0,0);
  }

  num_devices= device_mask;

  while (num_devices != 0)
  {
   device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if ( device == RFM_DEVICE_0 && rfwcdma_mdsp_is_tq_idle())
    {
      RF_MSG(RF_HIGH, "rfwcdma_mc_rx_sleep(): calling rfwcdma_mdsp_ccs_release_tq_pair_reservce_device() ");
      /* release TQ pair but keep the device association */
      rfwcdma_mdsp_ccs_release_tq_pair_reservce_device();
    }

     num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }

/* Abort pending AsDiv Event */
  #ifdef FEATURE_RF_ASDIV
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4))
  {
    rfwcdma_core_asdiv_abort(rxlm_buf_idx[device]);
  }
  #endif
  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Rx_Sleep - %d", prof_cal_func);
  }

  /* Return the API Wait time */
  return ret_val;

} /* rfwcdma_mc_rx_sleep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare to tune the radio on the on the device(s) specified.

  @details
  Prepares scripts for Rx Tune on the provided set of devices.The scripts will be created for
  the band and channel which was requested. The
  function will not perform any operation related to direct access to the
  hardware, as it is not guaranteed that all the necessary clock are enabled
  while calling this function.

  @param device_mask
  This is a bitmask passed indicating Rx devices which need to be tuned

  @param rx_tune_params
  Pointer to the structure that holds the parameters passed by L1 to wakeup WCDMA Rx.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfwcdma_mc_rx_prep_tune
(
  uint32 device_mask,
  rfm_wcdma_rx_tune_params *rx_tune_params
)
{
  rf_path_enum_type path =0;
  uint32 execution_time = 0;
  uint16 pll_settling_time = 200;
  uint32 warmup_time_us = 0, script_type= 0, carrier=0, channel_loop=0;
  uint32 num_devices = 0, valid_scripts = 0;
  rfm_device_enum_type device = 0;
  uint32 carrier_index = 0;
  uint8 current_mc_state = 0, num_chan = 0;
  rfcom_wcdma_band_type band = RFCOM_BAND_IMT;
  rfwcdma_data_debug_flags_type *dbg_flag;
  rfcom_wcdma_band_type source_band = RFCOM_BAND_INVALID;
  rfwcdma_core_rxctl_dis_first_ind_type disable_ind;
  rfdevice_wcdma_spur_param_type *dyn_notch_filter_settings = NULL;

  rfwcdma_mdsp_set_data_param_type set_agc_config_param = { 0 };

  rf_buffer_intf *rf_tune_script_buffer[RFWCDMA_MC_WAKEUP_SCRIPTS_MAX] = { NULL };

  /* Data structures needed for populating the wakeup scripts in DM */
  rfwcdma_mdsp_rx_wakeup_cfg_type cfg;

  uint32 alt_path = 0;

  memset(&cfg, 0, sizeof(rfwcdma_mdsp_rx_wakeup_cfg_type));

  /* Create RF buffer to populate the script. This buffer will be maintained
  until we populate the DM */
  for ( script_type=0; script_type< RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
  {
    rfwcdma_mc_create_rf_buff(&rf_tune_script_buffer[script_type]);

    if (rf_tune_script_buffer[script_type]== NULL)
    {
      RF_MSG(RF_ERROR,
             "rfwcdma_mc_rx_prep_tune: rf buffer allocation failed!");
      break;
    }
  }

  /* Clear MC buffer */
  for ( script_type=0; script_type< RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
  {
    rf_buffer_clear(rf_tune_script_buffer[script_type]);
  }

  /* Use local copy of the passed device mask for all the computations */
  num_devices= device_mask;

  /* Loop over all the devices enabled for tune */
  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_tune: Invalid device %d", device);
      return 0;
    }

    path = rfcommon_core_device_to_path(device);
    carrier_index =  rx_tune_params->api_params[device].carrier_mask;
    num_chan = rfwcdma_core_count_carriers(rx_tune_params->api_params[device].carrier_mask);
    source_band = rfwcdma_mc_get_source_band(device);

    rfwcdma_mc_state.rxlm_buf_idx[device] = rx_tune_params->api_params[device].rxlm_handle;

    /* MC state variable may not be updated, hence use the information from caller*/
    band = rfwcdma_core_convert_chan_to_band(rx_tune_params->api_params[device].channel[0]);
    
    rfc_wcdma_update_rx_band_config(device,band);
	
    set_agc_config_param.set_data_cmd = RFWCDMA_MDSP_CONFIG_RxAGC_IN_PROGRESS; 
    set_agc_config_param.rflm_handle = rx_tune_params->api_params[device].rxlm_handle;
    set_agc_config_param.data.gen_b = TRUE;
    (void)rfwcdma_mdsp_async_set_data(&set_agc_config_param);
    
    if(source_band != band)
    {
      disable_ind = RFWCDMA_CORE_ASM_TUNER_DIS_IND_BAND;
    }
    else
    {
      disable_ind = RFWCDMA_CORE_ASM_TUNER_NO_DISABLE_OPS;
    }

    RF_MSG_6(RF_HIGH, "rfwcdma_mc_rx_prep_tune: Parameters - device mask %d curr device: %d "
             " path: %d, carrier_mask: %d,"
             " channel: %d, Curr_band %d",
             num_devices,device,
             path, carrier_index,
             rx_tune_params->api_params[device].channel[0], band);

    /* Update Alt path to RFC*/
    rfwcdma_core_rxctl_update_alt_path(device, band, FALSE, &alt_path);

    /* Allocate Rx DM buffers for the given RxLM handle */
    (void)rfwcdma_mdsp_allocate_dm_rx_buf((rflm_dm_handle_id_t)rx_tune_params->api_params[device].rxlm_handle);

    /* Get the global callback data here before calling Set channel */
    dyn_notch_filter_settings = rfwcdma_core_rxctl_get_dynamic_notch_cb_data(device);

    rfwcdma_mc_create_device_scripts(device,
                                     band,
                                     rx_tune_params->api_params[device].channel,
                                     rf_tune_script_buffer,
                                     num_chan,
                                     source_band,
                                     disable_ind,
                                     dyn_notch_filter_settings);

    (void)rfwcdma_msm_update_rxlm_buffer(device,
                                         rx_tune_params->api_params[device].channel,
                                         carrier_index,
                                         rx_tune_params->api_params[device].rxlm_handle,
                                         alt_path,
                                         enable_nbr_flag,
                                         dyn_notch_filter_settings, RFWCDMA_MSM_STATIC_NOTCH_TUNE);

    	/* FR18317 - NBR */
	/* Load up modG for NBR for PRx only  and overall enable is TRUE */
	dbg_flag = rfwcdma_get_debug_flags();
	
    if( dbg_flag->enable_rf_nbr_operations && enable_nbr_flag &&
		   ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)))
	{
	  /* print dbg msg */
	  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
			"rfwcdma_mc_rx_prep_tune: W_ NBR DBG: Loading up modG on RxLM index %d for PRx",
			(uint8)nbr_rxlm_buf_idx);
	  (void)rfwcdma_msm_update_nbr_rxlm_buffer(device,
                                               rx_tune_params->api_params[device].channel, 
                                               num_chan, 
                                               nbr_rxlm_buf_idx,
                                               alt_path);
	}

    rfwcdma_core_rxctl_config_band( rx_tune_params->api_params[device].rxlm_handle,
                                    device,
                                    band,
                                    carrier_index,
                                    RFLM_WCDMA_HOME_MEAS );

    rfwcdma_core_rxctl_config_chan( rx_tune_params->api_params[device].rxlm_handle,
                                    device,
                                    band,
                                    rx_tune_params->api_params[device].channel,
                                    carrier_index,
                                    RFLM_WCDMA_HOME_MEAS );


    if (num_chan == RFCOM_SINGLE_CARRIER )
    {
      RFGNSS_MC_V(wg_chan)(rx_tune_params->api_params[device].channel[0]);

      /* Update Notch filter settings */
      RFGNSS_MC_V(program_notch_filter)(RFGNSS_NOTCH_CFG_WCDMA);

    }

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }

  /* Push the scripts to DM
   1. Compute the warmup time from the WTR script execution time.
   2. Append the valid scripts to the mdsp data structure
   3. Configure RF-MDSP Async interface for RF wakeup config
    */

  /* Step 1 */

  /* Get the WTR script execution time from Device */
  rf_buffer_get_script_execution_time(rf_tune_script_buffer[RFWCDMA_MC_WTR_SCRIPTS],
                                      RF_BUFFER_RFFE,
                                      &execution_time);

  /* Get the PLL settling time from device */
  rfdevice_wcdma_rx_cmd_dispatch( RFM_DEVICE_0,
                                  band,
                                  RFDEVICE_GET_RX_PLL_SETTLING_TIME,
                                  &pll_settling_time );

  warmup_time_us = (execution_time/10) + pll_settling_time;
  cfg.device_cfg.rf_warmup_time = WCDMA_CONV_US_TO_USTMR(warmup_time_us);

  RF_MSG_4(RF_HIGH, "rfwcdma_mc_rx_prep_tune: [Script Time] Dev Mask %d:"
           "WTR script execution %d, "
           "PLL %d, Warmup - %d",
           device_mask,
           (execution_time/10),
           pll_settling_time,
           cfg.device_cfg.rf_warmup_time );

  /* Step 2 */
  for (script_type=0; script_type < RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++ )
  {
    if (rfwcdma_core_get_resource_script_from_buffer(rf_tune_script_buffer[script_type])!= NULL)
    {
      cfg.device_cfg.wakeup[valid_scripts].script= rf_tune_script_buffer[script_type];
      cfg.device_cfg.wakeup[valid_scripts].delay= 0;
      valid_scripts ++;
    }
  }

  num_devices = rf_count_one_bits(device_mask);

  if (num_devices == 1)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(device_mask);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_tune: Invalid device %d", device);
      return 0;
    }

    cfg.rflm_handle= rx_tune_params->api_params[device].rxlm_handle;
  }

  else
  {
    cfg.rflm_handle= rx_tune_params->api_params[0].rxlm_handle;
  }

  /* Appends tasks which have valid scripts only*/
  cfg.device_cfg.num_tasks_wakeup_script = valid_scripts;

  /* Step 3*/
  rfwcdma_mdsp_async_config_rx_wakeup (&cfg);

  num_devices = device_mask;

  /* Update State information */
  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_tune: Invalid device %d", device);
      return 0;
    }

    path = rfcommon_core_device_to_path(device);
    carrier_index =  rx_tune_params->api_params[device].carrier_mask;

    set_agc_config_param.set_data_cmd = RFWCDMA_MDSP_CONFIG_RxAGC_IN_PROGRESS; 
    set_agc_config_param.rflm_handle = rx_tune_params->api_params[device].rxlm_handle;
    set_agc_config_param.data.gen_b = FALSE;
    (void)rfwcdma_mdsp_async_set_data(&set_agc_config_param);

    current_mc_state =  rfwcdma_mc_state.car_path_state[path][0].rf_state;

    /* Step4: Update state machine */

    carrier = rfwcdma_core_count_carriers(rx_tune_params->api_params[device].carrier_mask);

    if (carrier >= RFCOM_MAX_CARRIERS)
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_prep_tune: Invalid Carrier computed from mask %d", carrier);
      return 0;
    }

    /* To have a memory of the current multi_carrier index we create a shadow. This is used in exec_tune
    to stop AGC on all the carriers. However, if this is the first tune after enter mode, we save the 
    multicarrier_idx passed in the tune_params  */

    if (rfwcdma_mc_state.rx_multi_carrier_idx[device]!= 0)
    {
      rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device]= rfwcdma_mc_state.rx_multi_carrier_idx[device]; 
    }
    else 
    {
      rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device] = rx_tune_params->api_params[device].carrier_mask;
    }
     
    rfwcdma_mc_state.rx_multi_carrier_idx[device] = rx_tune_params->api_params[device].carrier_mask;
    rfwcdma_mc_state.rx_carriers[device] = rfwcdma_core_count_carriers(rx_tune_params->api_params[device].carrier_mask);

    for (carrier = 0, channel_loop = 0; carrier < RFCOM_MAX_CARRIERS; carrier++)
    {
      if ((carrier_index & (1 << carrier)) != 0)
      {
        current_mc_state =  rfwcdma_mc_state.car_path_state[path][carrier].rf_state;
        rfwcdma_mc_state.car_path_state[path][carrier].curr_band_rx =
          rfwcdma_core_convert_chan_to_band(rx_tune_params->api_params[device].channel[channel_loop]);
        rfwcdma_mc_state.car_path_state[path][carrier].rx_rfm_device = device;
        rfwcdma_mc_state.car_path_state[path][carrier].curr_chan_rx = rx_tune_params->api_params[device].channel[channel_loop++];

        if (current_mc_state == RFWCDMA_MC_STATE_RXTX )
        {
          rfwcdma_mc_state.car_path_state[path][carrier].rf_state = RFWCDMA_MC_STATE_RXTX_PREP_TUNE;
        }
        else
        {
          rfwcdma_mc_state.car_path_state[path][carrier].rf_state = RFWCDMA_MC_STATE_RX_PREP_TUNE;
        }
        RF_MSG_4(RF_HIGH,"rfwcdma_mc_rx_prep_tune - carrier update- for device %d "
                          "carrier %d:  curr_state %d, new_state %d",
                          device, carrier, current_mc_state, 
                          rfwcdma_mc_state.car_path_state[path][carrier].rf_state); 
      }
    }

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));

    RF_MSG_5(RF_HIGH,"rfwcdma_mc_rx_prep_tune: MC state update - device: %d, path: %d,"
             "carrier index: %d RxLM buffer index: %d,Num carriers %d",
             device, path,
             rfwcdma_mc_state.rx_multi_carrier_idx[device],
             rfwcdma_mc_state.rxlm_buf_idx[device],
             rfwcdma_mc_state.rx_carriers[device]);

  }

  /*Destroy local RF buffers*/
  for ( script_type=0; script_type< RFWCDMA_MC_WAKEUP_SCRIPTS_MAX; script_type++)
  {
    rfwcdma_mc_destroy_rf_buff(rf_tune_script_buffer[script_type]);
  }

  return RFM_WCDMA_HEALTHY_STATE;

}/*rfwcdma_mc_rx_prep_tune*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Rx Tune sequence to tune to the given band and channel on the passed device(s).
  This routine expects that prep_tune or prep_wakeup routine was called which creates the scripts
  and pushes it to DM.

  @details
  This function will send a command to firmware to stop and restart AGC
  started. The MC state will be updated to reflect the current state.

  @param device_mask
  This is a bitmask passed indicating Rx devices which need to be turned on.

  @param carrier_mask
  Variable indicating the carrier mask on the devices

  @param rxlm_buf_idx
  The Rx Link Manager buffer array that shall be used to configure the
  modem hardware. the array will hold the rxlm handles for only the specified devices

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfwcdma_mc_rx_exec_tune
(
  uint32 device_mask,
  uint8 carrier_mask[RFM_DEVICE_4],
  uint32 rxlm_buf_idx[RFWCDMA_MAX_RX_DEVICES]
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  rf_path_enum_type path = 0;
  timetick_type prof_cal_func;
  uint8 current_mc_state = 0, carrier =0;
  uint32 num_devices =0, multicarrier_idx =0, multicarrier_idx_enable =0;
  rfm_device_enum_type device =0;
  rfcom_multi_carrier_hspa_id_type carrier_index =0;
  rfwcdma_data_debug_flags_type *dbg_flag; 
  boolean pwr_vote_status = TRUE;
  rfcom_wcdma_band_type prev_band_info , curr_rx_band ;
  rfdevice_trx_wcdma_rx **src_rx_device = NULL;
  rfdevice_trx_wcdma_rx **tgt_rx_device = NULL;
  uint8 device_idx = 0; 

  /* Create two arrays for storing the AGC command word. This will save additional loops*/
  rfwcdma_mdsp_agc_action_type cmd_action_disable[RFM_DEVICE_4] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };

  rfwcdma_mdsp_agc_action_type cmd_action_enable[RFM_DEVICE_4] =
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };


  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */

    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

  /* Use local copy of the passed device mask for all the computations */
  num_devices= device_mask;

  /* Loop over all the devices enabled for tune */
  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_HIGH, "rfwcdma_mc_rx_exec_tune: Invalid device %d", device);
      return 0;
    }

    path = rfcommon_core_device_to_path(device);

    curr_rx_band = rfwcdma_mc_state.car_path_state[path][0].curr_band_rx;

    /* Using the shadow value of multi_carrier_idx always. mc_rx_exec_tune will be called ONLY IF 
    prep_tune is called. This is handled by rfm layer*/

    multicarrier_idx |= rfwcdma_mc_state.rx_multi_carrier_idx_shadow[device];

    multicarrier_idx_enable|=rfwcdma_mc_state.rx_multi_carrier_idx[device];

    carrier_index =  carrier_mask[device];

    RF_MSG_5(RF_HIGH, "rfwcdma_mc_rx_exec_tune: Parameters - device_mask from rfm %d "
             "current device: %d path: %d, carrier_index: %d,"
             "multicarrier_idx: %d",num_devices,device,path,
             carrier_index, multicarrier_idx);

    cmd_action_disable[device] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
    cmd_action_enable[device] = RFWCDMA_MDSP_AGC_CMD_ENABLE;

    if(device == rfwcdma_mc_state.primary_rx_device)
    {
      cmd_action_disable[rfwcdma_mc_state.secondary_rx_device] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
      
    }

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }/*while (num_devices != 0)*/

  /* Send disable AGC cmd to FW */
  rfwcdma_core_rxctl_enable_rxagc(multicarrier_idx,
                                  cmd_action_disable,
                                  rxlm_buf_idx,
                                  FALSE,0,0 );

  
  /* Update State information */
  num_devices= device_mask;

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);
    path = rfcommon_core_device_to_path(device);
    curr_rx_band = rfwcdma_mc_state.car_path_state[path][0].curr_band_rx;

    for (carrier = 0; carrier < RFCOM_MAX_CARRIERS; carrier++)
    {
      if ((carrier_index & (1 << carrier)) != 0)
      {
         current_mc_state = rfwcdma_mc_state.car_path_state[path][carrier].rf_state;

    if (current_mc_state == RFWCDMA_MC_STATE_RXTX_PREP_TUNE)
    {
          rfwcdma_mc_state.car_path_state[path][carrier].rf_state = RFWCDMA_MC_STATE_RXTX;
    }
    else
     {
          rfwcdma_mc_state.car_path_state[path][carrier].rf_state = RFWCDMA_MC_STATE_RX;
     }

        RF_MSG_4(RF_HIGH,"rfwcdma_mc_rx_exec_tune - carrier update- for device %d "
                          "carrier %d:  curr_state %d, new_state %d",
                          device, carrier, current_mc_state, 
                          rfwcdma_mc_state.car_path_state[path][carrier].rf_state); 
       }
     }

    rfwcdma_mc_pwr_vote_on_off(device,
                               rfwcdma_mc_state.car_path_state[path][0].curr_band_rx,
                               TRUE,
                               RFM_WCDMA_RX_DEVICE,
                               FALSE);

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));

  }/* while (num_devices != 0)*/

  /* FR18317 - NBR */
	/* Load up modG for NBR for PRx only  and overall enable is TRUE */
	dbg_flag = rfwcdma_get_debug_flags();
	
  if( dbg_flag->enable_rf_nbr_operations && enable_nbr_flag &&
  	 ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)))
	{
	  /* print dbg msg */
	  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
			"rfwcdma_mc_rx_exec_tune: W_ NBR DBG: Loading up modG on RxLM index %d for PRx",
			(uint8)nbr_rxlm_buf_idx);
	
  /* Send enable AGC cmd to FW */
  rfwcdma_core_rxctl_enable_rxagc(multicarrier_idx_enable,
                                  cmd_action_enable,
                                  rxlm_buf_idx,
                                  TRUE,
                                  TRUE,       
                                  (uint8)nbr_rxlm_buf_idx);
	}
     else
     {
  /* Send enable AGC cmd to FW */
  rfwcdma_core_rxctl_enable_rxagc(multicarrier_idx_enable,
                                  cmd_action_enable,
                                  rxlm_buf_idx,
                                  TRUE,
                                  FALSE,       
                                  (uint8)nbr_rxlm_buf_idx);

     }
  

  RF_MSG_2( RF_HIGH, "rfwcdma_mc_rx_exec_tune: Device Mask %d, "
            "Multi_carrier %d",
            device_mask,
            multicarrier_idx);

  if (rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
    RF_MSG_1(RF_HIGH, "RFWCDMA Execution Time: Exec_tune - %d", prof_cal_func);
  }

  /* Return the API Wait time */
  return ret_val;

} /* rfwcdma_mc_rx_exec_tune*/

/*! @cond vbhide */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Config the ASM statue bit map rfwcdma_mc_state.asm_status_bit_map[device].

  @details
  This function will set the asm status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @on_off
  Indicate turn the correaponding ASM or nor

  @return
  Variable indicating the status of API execution
*/
boolean
rfwcdma_mc_config_asm_status_bit_map
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off
)
{
  boolean api_status = TRUE;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    if(TRUE == on_off)
    {
      rfwcdma_mc_state.asm_status_bit_map[device] |= (1 << band);
    }
    else
    {
      rfwcdma_mc_state.asm_status_bit_map[device] &= (~(1 << band));
    }

    RF_MSG_4(RF_HIGH, "ASM Status: device %d band %d on_off %d status 0x%x",
                      device, band, on_off, rfwcdma_mc_state.asm_status_bit_map[device]);
  }
  else
  {
    api_status = FALSE;
    RF_MSG_3(RF_HIGH, "ASM Status: Invalid param - device %d band %d status 0x%x",
                      device, band, rfwcdma_mc_state.asm_status_bit_map[device]);
  }

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the ASM statue bit map.

  @details
  This function will set the asm status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @on_off
  Indicate turn the correaponding ASM or nor

  @return
  Variable indicating the status of API execution.
*/
boolean
rfwcdma_mc_get_asm_status_bit
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band
)
{
  boolean ret_val;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    if(rfwcdma_mc_state.asm_status_bit_map[device] & (1 << band))
    {
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  }
  else
  {
    ret_val = FALSE;
  }

  return ret_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Config the ASM statue bit map rfwcdma_mc_state.tuner_status_bit_map[device]..

  @details
  This function will set the asm status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @on_off
  Indicate turn the correaponding TUNER or nor

  @return
  Variable indicating the status of API execution.
*/
boolean
rfwcdma_mc_config_tuner_status_bit_map
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off
)
{
  boolean api_status = TRUE;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    if(TRUE == on_off)
    {
      rfwcdma_mc_state.tuner_status_bit_map[device] |= (1 << band);
    }
    else
    {
      rfwcdma_mc_state.tuner_status_bit_map[device] &= (~(1 << band));
    }

    //RF_MSG_4(RF_HIGH, "TUNER Status: device %d band %d on_off %d status 0x%x",
    //                  device, band, on_off, rfwcdma_mc_state.tuner_status_bit_map[device]);
  }
  else
  {
    api_status = FALSE;
    RF_MSG_3(RF_HIGH, "TUNER Status: Invalid param - device %d band %d status 0x%x",
                      device, band, rfwcdma_mc_state.tuner_status_bit_map[device]);
  }

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the TUNER statue bit map.

  @details
  This function will set the tuner status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @on_off
  Indicate turn the correaponding TUNER or nor

  @return
  Variable indicating the status of API execution.
*/
boolean
rfwcdma_mc_get_tuner_status_bit
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band
)
{
  boolean ret_val;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    if(rfwcdma_mc_state.tuner_status_bit_map[device] & (1 << band))
    {
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  }
  else
  {
    ret_val = FALSE;
  }

  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the band info from MC state machine for that particular RFM device

  @details
  Get the band info from MC state machine for that particular RFM device

  @param device
  The device number indicate the device ID.

  @return source_band
  Band info from MC state machine
  
*/
rfcom_wcdma_band_type
rfwcdma_mc_get_source_band
(
  rfcom_device_enum_type device
)
{
  rfcom_wcdma_band_type source_band = RFCOM_BAND_INVALID;
  uint8 carrier = 0;
  rf_path_enum_type path = RF_PATH_0;
  
  if( (device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) )
  {
    for (carrier = 0; carrier < RFCOM_MAX_CARRIERS; carrier++)
    {
      if ((rfwcdma_mc_state.rx_multi_carrier_idx[device] & (1 << carrier)) != 0)
      {
        /* We have found a match in MC state machine. Get the band info and exit*/
        path = rfcommon_core_device_to_path(device);
        source_band = rfwcdma_mc_state.car_path_state[path][carrier].curr_band_rx;

        break;
      }
    } /* for (carrier = 0; carrier < RFCOM_MAX_CARRIERS; carrier++)*/
  }

  else
  {
    RF_MSG_1(RF_ERROR, "rfwcdma_mc_get_source_band: Invalid param - device %d", device);
  }

  /*RF_MSG_1(RF_HIGH, "rfwcdma_mc_get_source_band: band %d", source_band); */

  return source_band;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
   Vote the Rx/Tx device On or OFF

  @details
  This function will set the POWER vote status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @on_off
  Indicate turn the correaponding Power vote or not

  @rx_tx
   Indicate if its Rx or Tx device

  @return
  Variable indicating the status of API execution.
*/
void
rfwcdma_mc_pwr_vote_on_off
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off,
  rfm_wcdma_rx_tx_device_type rx_tx,
  boolean irat_mode
)
{
  boolean pwr_vote_status = TRUE;
  timetick_type prof_cal_func;
  rfcom_wcdma_band_type prev_band_info;
  rfdevice_trx_wcdma_rx **src_rx_device = NULL;
  rfdevice_trx_wcdma_rx **tgt_rx_device = NULL;
  uint8 device_idx = 0; 
  boolean disable_prev_device = TRUE;
  
  if(!rfcommon_nv_get_lpm_feature_enabled())
  {
   return ;
  }

  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    pwr_vote_status &= TRUE;
  }
  else
  {
    RF_MSG_2(RF_ERROR, "rfwcdma_mc_pwr_vote_on_off: Invalid param - device %d band %d", device, band);
    return ;
  }

  if(rfwcdma_mc_enable_time_profiling)
  {
    prof_cal_func = 0;     /* Function Profiling Variable */
    prof_cal_func = timetick_get();     /* Get Time tick for ISR Profile */
  }

 if(!irat_mode)
 {

  /*Check to make sure multiple power on request for the same device is ignored*/
  if((rx_tx == RFM_WCDMA_RX_DEVICE)&&(!((rfwcdma_mc_get_pwr_vote_status(device,band))^(on_off))))
  {
   RF_MSG_2(RF_LOW, "rfwcdma_mc_pwr_vote_on_off: Duplicate vote - device %d band %d", device, band);
   return ;
  }

   /* In case of ON vote*/ 
  if((rfwcdma_mc_get_pwr_vote_bit_map_info(device))&&(rx_tx == RFM_WCDMA_RX_DEVICE)&&(on_off))
  {
    prev_band_info = (rfcom_wcdma_band_type)(rf_extract_first_one_bit_lsb_location(rfwcdma_mc_get_pwr_vote_bit_map_info(device)));
  
    src_rx_device = (rfdevice_trx_wcdma_rx**) rfc_wcdma_get_device(device, 
 	 							   RFC_CONFIG_RX, 
	 							   prev_band_info,
								   RFDEVICE_TRANSCEIVER);
  
    tgt_rx_device = (rfdevice_trx_wcdma_rx**) rfc_wcdma_get_device(device, 
								   RFC_CONFIG_RX, 
								   band,
								   RFDEVICE_TRANSCEIVER);

     if( (src_rx_device != NULL )&&(tgt_rx_device != NULL )&&
       (src_rx_device[device_idx] != NULL )&&(tgt_rx_device[device_idx] != NULL ))
    {
     /* If tuning to same WTR again, just update status - no need to vote ON again */
     if(src_rx_device[device_idx] == tgt_rx_device[device_idx])
     {
      disable_prev_device = FALSE;
     }
     else
     {
      disable_prev_device = TRUE;
     }
    }
    if(disable_prev_device)
    {
      /* Frequency scan cases - In case of new band - new device, put the previous device to LPM */
      pwr_vote_status &= rfdevice_wcdma_rx_pwr_sleep(device,prev_band_info );
      rfwcdma_mc_set_pwr_vote_status(device,prev_band_info, FALSE);
    }
    else
    {  
       /* Frequency scan cases - In case of new band - same device,  just update the new band vote*/
       rfwcdma_mc_set_pwr_vote_status(device,prev_band_info, FALSE);
       rfwcdma_mc_set_pwr_vote_status(device,band, TRUE);
   return ;
  }
  }
 }

  if(on_off)
  {
   if(rx_tx == RFM_WCDMA_RX_DEVICE)
   {
      pwr_vote_status &= rfdevice_wcdma_rx_pwr_wakeup(device,band );
   }
   else if(rx_tx == RFM_WCDMA_TX_DEVICE)
   {
      pwr_vote_status &= rfdevice_wcdma_tx_pwr_wakeup(device,band );
   }
  }
  else
  {
   if(rx_tx == RFM_WCDMA_RX_DEVICE)
   {
      pwr_vote_status &= rfdevice_wcdma_rx_pwr_sleep(device,band );
   }
   else if(rx_tx == RFM_WCDMA_TX_DEVICE)
   {
      pwr_vote_status &= rfdevice_wcdma_tx_pwr_sleep(device,band );
   }
  } 
	  
  if (pwr_vote_status == FALSE)
  {
	 RF_MSG_3(RF_ERROR, "rfwcdma_mc_pwr_vote_on_off - Device - %d Rx/Tx %d Power on/off %d failed", device , rx_tx , on_off);
  }

  if((rx_tx == RFM_WCDMA_RX_DEVICE)&&(pwr_vote_status))
  {
    rfwcdma_mc_set_pwr_vote_status(device,band, on_off);
  }

  if (rfwcdma_mc_enable_time_profiling)
  {
	 prof_cal_func = timetick_get_elapsed(prof_cal_func, T_USEC);
	 RF_MSG_1(RF_HIGH, "rfwcdma_mc_pwr_vote_on_off : Execution Time - %d", prof_cal_func);
  }

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
   Set the POWER vote status  bit map.

  @details
  This function will set the POWER vote status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @on_off
  Indicate turn the correaponding Power vote or not

  @return
  Variable indicating the status of API execution.
*/
boolean
rfwcdma_mc_set_pwr_vote_status
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off
)
{
  boolean api_status = TRUE;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    if(TRUE == on_off)
    {
      rfwcdma_mc_state.pwr_vote_status_bit_map[device] |= (1 << band);
    }
    else
    {
      rfwcdma_mc_state.pwr_vote_status_bit_map[device] &= (~(1 << band));
    }

    RF_MSG_4(RF_HIGH, "Pwr vote Status: device %d band %d on_off %d status 0x%x",
                      device, band, on_off, rfwcdma_mc_state.pwr_vote_status_bit_map[device]);
  }
  else
  {
    api_status = FALSE;
    RF_MSG_3(RF_ERROR, "Pwr vote Status: Invalid param - device %d band %d status 0x%x",
                      device, band, rfwcdma_mc_state.pwr_vote_status_bit_map[device]);
  }

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the POWER vote status  bit map.

  @details
  This function will set the POWER vote status bit map according to different devices.

  @param device
  The device number indicate the device ID.

  @param band
  The band indication

  @return
  Variable indicating the POWER vote status bit map
*/
boolean
rfwcdma_mc_get_pwr_vote_status
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band
)
{
  boolean ret_val;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4) && 
    (band >= RFCOM_BAND_IMT) && (band < RFCOM_NUM_WCDMA_BANDS))
  {
    if(rfwcdma_mc_state.pwr_vote_status_bit_map[device] & (1 << band))
    {
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  }
  else
  {
    ret_val = FALSE;
	RF_MSG_3(RF_ERROR, "Pwr vote Status: Invalid param - device %d band %d status 0x%x",
                      device, band, rfwcdma_mc_state.pwr_vote_status_bit_map[device]);
  }

  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the POWER vote status of all bands per device.

  @details
  This function will get the POWER vote status of all bands per device.

  @param device
  The device number indicate the device ID.

  @return
  Variable indicating the POWER vote status of all bands per device
*/
uint16
rfwcdma_mc_get_pwr_vote_bit_map_info
(
  rfcom_device_enum_type device
)
{
  uint16 pwr_vote_info = 0;
  
  if((device >= RFM_DEVICE_0) && (device < RFM_DEVICE_4))
  {
   pwr_vote_info = rfwcdma_mc_state.pwr_vote_status_bit_map[device];
  }

  return pwr_vote_info;
}

/*! @endcond */
