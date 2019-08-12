/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M  W C D M A  C T L

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2009 - 2015 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_ctl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/15   ljz     Enable NBEE serial calibration for W
05/19/15   sd      Include user configurable DL power in sensitivity estimation 
03/19/15   ck      Remove ftm ccmgr cleanup during band/tech transition
03/19/15   rmb/ck  Fix to include ftm concurrency manager cleanup in the ftm_wcdma_set_mode() 
03/12/15   gh      Enable FBRx LDO ON in FTM mode
                   Update FTM temp comp to use prep_temp_comp and exec_temp_comp
                   as oppose to do_temp_comp
02/19/15   rmb     Fix for correct conversion from UL channel to band in Multi-Carrier Tune.
02/09/15   ak      Calculate the DVGA offsets correctly when NBEE is disabled and for Serial Cal
01/30/14   zbz     [selftest] Remove unused memfree
02/03/15   xsh     Add ftm_wcdma_disable_NBEE API
02/04/15   sd      Added interface for FTM WCDMA Sensitivity Estimation
01/27/15   rmb     Change the sequence to set mdsp interface with MTPL after setting
                   the Tx Power limit in RF.
12/29/14   rmb     Add BC config check before proceeding to Tune to channel.
12/18/14   ars/rmb Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
12/10/14   vs      Cleanup of F3s seen during RFCal
11/25/14   jmf     [selftest] Support for WCDMA VSWR v2 processing
11/18/14   aro     Added HS waveform config for XPT cal for Jolokia
11/17/14   rmb     Update chain mask for RSB cal in Dynamic update RxLM cmd.
11/18/14   aro     Removed HS waveform config for XPT cal for Jolokia
11/14/14   jmf     [selftest] WCDMA FBRx Noise Floor Calibration
11/13/14   vbh     [1] Add checks for concurrency manager before tune, 
                   [2] Based on scenario use carrier mask for dev 2/3
11/10/14   aro     Interface to config offline clock for XPT IQ capture
11/06/14   vbh     Update parameter usaed in ftm_wcdma_set_lna_range
11/05/14   ac      ftm flag to denote the txagc state 
11/04/14   kai     Added function to return pa switchpoints
10/29/14   vs      Enable HS waveform for XPT config in Jolokia
10/27/14    dw     Added support for rflm_wcdma_rf_send_deadline_script
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/13/14   dr      Adding featurization for Tabasco
10/09/14   vbh     Add device parameter for set_mode API
10/02/14   dw      Latch TxAGC after enabling DC waveform
09/26/14   ag      included missing file "rfcommon_fbrx_types.h" 
09/23/14   kai     Add DRDS Cal support
09/23/14   vbh     Use rfm_device_enum_type
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
09/15/14   joz     replace FEATURE_DIMEPM_MODEM by FEATURE_JOLOKIA_MODEM
09/15/14   vbh     Updated device parameter to support Tx sharing
09/05/14   vbh     Move to RFM Apis for Tx - init/deinit, enable/disable
08/25/14   dw      Add DC HSUPA char support
8/22/14    vbh     Updated the implementation for set_pa and set_tx FTM APIs
08/21/14   rmb     Add alternate support in the WCDMA MSM layer.
08/18/14   jmf     [SelfTest] ACLR meas changes
03/12/14   zhh    Update XPT Delay structure to support DC-UPA
08/12/14   joz     offtarget compilation fixes
08/11/14   shb     Fixed offtarget compiler error
08/08/14   aro     Check to ensure rfm_enter_mode() when W radio is in
                   sleep and set_mode is called
08/07/14   joz     Change rfm_wcdma sleep and wakeup to new function API
08/06/14   dw      Disable TxAGC in Tx init when Tx BW change
08/01/14   dw      Initial support for single Tx DSDA
07/28/14   vbh     Fix RSB Cal - make sure exit on device 1 happens before device 0
07/14/14   aa      Fix FBRx DC Cal failures due to incorrect condition buffer ID check
12/05/13   ska     Graceful exit if FTM_WCDMA_SET_MODE is issued when the phone is not in FTM mode
07/10/14   gh      Remove set PA ON in set PA range
06/30/14   vbh     Use device for reference instead of path
06/27/14   ac      change to enable NBEE cal 
06/26/14   vbh     [1] Add check to not tune Tx PLL in case of DBDC
                   [2] Use a dbg flag for using script based tune from FTM
06/11/14   kai     Allocate IQ sample buffer from heap
06/11/14   kai     Coupler configuration during XPT cal
06/06/14   gh      Temp change to split PRx and DRx tune in ftm_wcdma_tune_to_chan for DPM
06/04/14   kai     Avoid using script based tune for dual carrier
06/04/14   vws     Support script based interface rfwcdma_core_antenna_tuner_program_tx_mode 
05/21/14   aro     Added ASM enable during FTM tune 
05/20/14   kai     Reduce RF Cal retune time
05/15/14   ak      Add parameter to sleep_rx API check for early shutdown
05/21/14   aa      DimePM TX re-tune fix
05/01/14   dw      Added update ET delay and set freq adjust in MDSP async set data command
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/24/14   vbh     [1] Added ftm_wcdma_tune_to_chan_scripts which calls script based tune apis 
                   [2] Replaced direct calls to rfm apis with calls to the routine in [1]
04/18/14   rmb     Change ENUM type for Core AGC to MDSP AGC.
04/17/14   ac      asd bringup with rfc changes and device script
04/09/14   sn      Updated PA interface
04/03/14   dw      Add dynamic RxLM update support
03/25/14   rmb     Add a new get rxagc API during X2W measurements. 
12/06/13  ska     Added fix for desired max power setting in RF-FTM
12/03/13   zhh     re-enter rf mode after tear down
03/19/13   dw      DCHSUPA: change to call the new retune API
3/24/14    vbh     Clean up rfc_common_get_cmn_device_object calls
03/11/13   dw      Add Bolt DCHSUPA support
02/25/14   zhh     fix KW warning
02/24/14   vbh     Add sleep_Rx api to keep consistent naming convention
02/05/14   dw      Remove set CW in DAC Cal sequence
02/04/14   jmf     Adding xpt_mode as arg for ftm_wcdma_xpt_cal_config
                   Use ftm_wcdma_xpt_cal_config to allocate LM buffer for FBRx_Mode
02/04/14   vbh     FR2001 - ftm wakeup call flow updates
01/30/14   jmf     Update WCDMA Feedback Setup and IQ Capture APIs for FBRxMode Config
01/09/14   dw      Fix false trigger by zero out IQ gain during FTM set Tx ON for Cal sweep
01/02/14   dw      Bolt cleanup on legacy mdsp function calls
12/23/13   kai     Clean up xpt_cal_config api
12/13/13   tks     Updated calls to rfdevice tx tune api
12/10/13   vbh     Adding tune to DRx in RF FTM mode
12/14/13   dw      Use core function to get RxAGC enable status
11/25/13   kai     Remove legacy API ftm_wcdma_load_dpd_tables
                   Add support for RFLM-based Tx AGC override
11/14/13   kma     Added pin calibration support
11/06/13   cd      Populate RFLM handle for RFLM overrides
11/05/13   kai     Fix compiling error on D3925
10/31/13   ac      set the flag for immediate/script for lna_update
10/02/13   aro     Updated mdsp_sync_send_ul_dpcch_cfg_cmd() interface
10/24/13   dw      Mainline FEATURE_DIME_MODEM
10/23/13   kai     Release/reallocate rflm wcdma ccs taskQ during xpt config/deconfig
10/23/13   kai     Cleanup set_tx and set_pa_range
10/15/13   dw      Move LM handle ID to mdsp set/get data header
10/10/13   kai     Remove calling to rfwcdma_msm_set_pa_range
10/05/13   kai     Add support for getting new common Tx LUT in TxAGC override.
09/25/13   vs      Fix issues with 2C/3C tune
09/24/13   dw      Add set lna offset/dvga offset/lna range thru new mdsp layer
09/17/13   vbh     Warning fixes
09/16/13   dw      Added valid RxLM buffer index to rfm_enter_mode call
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
09/09/13   dbz     Fix off-target compiler warning
09/06/13   aka     memcpy cleanup
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
09/09/13   dbz     Fix off-target compiler warning
09/06/13   aka     memcpy cleanup
09/10/13   dw      Remove DBDC featurization and code clean up.
08/28/13   dj      Fixed a compiler warning
08/27/13   dj      Added ftm_wcdma_feedback_setup to support tx_internal_cal
                   and self test
08/27/13   aka     Added support for WCDMA self-test commands
08/23/13   vs      Added support for 3 carrier
08/20/13   vbh     Updated the set mode api to not call tune_to_chan on 9x35 RUMI
08/07/13   kai     [xPT] Move modem bus clk voting into radio setup
08/02/13   kai     Config to HSUPA waveform during XPT sample capture
08/01/13   kr      replace memcpy with memscpy
08/01/13   gvn     Change name of stg parameter for RSB cal
07/30/13   kai     Added support for Delay vs Temp comp.
07/24/13   vs      Pass Closed loop NV info to Tuner 
07/15/13   cri     Immediate MSM register programming for tx override
07/17/13   jmf     [xPT] Fix Samp Rate query in FTM IQ Cap Call Flow
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/15/13   ars     Added FTM interface for self-test APIs
07/15/13   sma     Changed ftm_wcdma_do_rsb_cal for RSB cal
06/24/13   ac      tx freezeaccum api change for both car and txlm state for dc cfg 
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
06/27/13   ac      ftm code for dcupa spectrum
06/26/13   kai     Add support for rc process variation during tx override
06/23/13   kai     Added WCDMA 16channels xpt delay support
06/04/13   kai     Update mc state machine during retune
05/31/13   kai     Added using modem_chain to deconfig WTR after sample capture
05/28/13   ra      Fix non-portable HWIO code. Needs further cleanup!
05/28/13   kai     Added DPD enable flag as setting WTR for Tx
05/28/13   vws     Featurize/Fix G+W only build compiler warnings
05/24/13    ac     compile error fix with the dc hsupa feature turned on
05/22/13  ka/ac    dc hsupa changes
05/21/13   rmb     Added APIs to support MULTI SYNTH STATE command and also making sure that aggregate
                   value is updated wheneven there is a PLL retune.
05/20/13   kai     Added new feedback path attenuation state enum
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/15/13   aa      correct rxd_overide_nv_settings to enable PRx and DRx 
05/09/13   jl      Port IQ capture code
05/09/13   kai     Skip setting RxD nv value to 0 as disable DRx
05/08/13   kai     Added turn off WTR ET path in sample capture deconfig
05/03/13   kai     Changed mcpm clk voting in xpt_cal_config
05/01/13   jmf     Implement xPT IQ Capture SWI Change
04/30/13   kai     Added reading delay value from NV in txagc_override
04/29/13   jj      Add function to convert rfcom wcdma band to ftm band
04/22/13   jmf     Implement xPT processing Enum Type
04/23/13   ka      Initial DBDC support
04/18/13   kai     Changed Tx agc override api
04/15/13   ka      kw fix
04/09/13   kai     Pass modem chain info to FW for sample capture
04/05/13   rmb     Included rfwcdma_core.h to fix Compiler Warning.
04/05/13   rmb     Added FTM APIs for WCDMA Sleep, Wakeup and Pretune to chan.
04/04/13   ka      WCDMA state machine updates
04/03 13   sn      Cleanup PA interface
03/28/13   kai     Changed Tx off sequence to disable RF front end first
03/27/13   kai     Added xpt cal config/deconfig
03/21/13   kai     Removed asm_config in retune
03/20/13   kai     Added asm_config_tx after tx_tune_chan
03/15/13   kai     Added turn off WTR ET pathn after sample capture
03/14/13   tks     Updated the rsb cal function with latest fixes 
03/14/13   kai     [1]Read DPD processing params from DPD_Config NV
                   [2]Added temporary fix to set the PDET coupler
03/13/13   kai     Added ftm_wcdma_xpt_cal_config 
03/12/13   kai     Added calling rfm_wcdma_decfg_tx after rfm_disable_tx
03/12/13   sn      XPT dual WTR fix for feedback path
03/09/13   tks     Fix kw warning 
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/05/13   kai     Remove hardcoding fb path in iq sample capture to support SV
03/05/13   vrb     Replace RFC accessor param init_req boolean by a req enum
03/01/13   kai     Fix merge errors
03/01/13   kai     Added ET mode specific Env scale value override
03/01/13   kai     Moved ET relative functions into rfwcdma_core_xpt.c 
02/28/13   tks     Modified the response packet variables in rsb cal 
02/28/13   dw      Add support for FTM_PHONE_MODE_SLEEP in ftm_wcdma_set_mode
02/26/13   tks     Fix klockwork error
02/26/13   kai     Added Tx override action type for override from NV
02/26/13   tks     Modified rsb calibration function to return correct status
02/25/13   kai     Change sample capture searchCenter to 29
02/20/13   tks     Added support for ftm rsb calibration  
02/20/13   kai     Fix exiting ET mode qpoet programming sequence
02/14/13   vb      Added Dime specific implementation of tune code override
02/12/13   jmf     Modify IQ capture fn to take more params for DPD proc in FW
                   Use functional MCPM API call to setup offline clk to 144MHz for EPT Sample capture
02/08/13   cri     Get Tx chain info for Tx override from RFC 
02/07/13   ndb/vb  Added rfwcdma_mc_set_tuner_tune_code_override() implementation support 
01/30/13   ka      Updated FTM interfaces to logical device. Cleaned up mdsp access.
01/28/13   kai     Replace MCPM_Config request by WL1 api
01/28/13   kai     Enable ET config in ftm_wcdma_set_tx
                   Removed ftm_wcdma_set_sapt_state
01/24/13   kai     Fixed invalid value handling for Tx override parameters
01/23/13   kai     Enable program PA current_bias in txagc override
                   Add program feedback path switch for IQ sample capture
01/21/13   bsh     Fixed compiler warnings due to moving some APIs to rflte_ext_mc.h 
01/11/13   bn      Disable et enable in ftm_wcdma_set_tx to unblock XPT_feature
12/21/12   cd      Fixed Tx override fields to be signed in order to allow
                   for invalid value programming
12/18/12   cd      Add support to override delay value to FW
12/06/12   kai     Removed QPOET device by calling the wcdma_pwr_tracker interface api
12/04/12   aka     Removed GSM set mode for device 2 from toggle_mode API
11/27/12   tws     Remove GSM FTM prototype.
11/20/12   gvn     Featurize LTE for Triton 
11/15/12   kai     [1]Removed ASM and PA config from ftm_wcdma_tune_to_chan
                   [2]Added tx_tune_chan in ftm_wcdma_tune_to_multi_chan
11/15/12   dw      Temporarily disable QPOET device function call
11/15/12   ka      Initial support for DCHSUPA
11/14/12   dw      Dime APT support
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/09/12   kai     Removed set PA range from set PDM
11/09/12   kai     Added XPT TXAGC Override and allocate ET TXLM buffer
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/24/12   bn      Enable W sample capture for XPT
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
09/20/12   pl      Adding rflte_mc.h
09/18/12   ac      rfc to atke device as parameter
09/05/12   ac      mcpm crash fix at enter mode
09/05/12   vbh     Fixing on target warnings 
08/30/12   ac      updating the brinup changes
08/28/12   tks     Updated PA/ASM device call flows 
08/17/12   tks     Removed pre-silicon by pass flag and added todo messages  
08/12/12   vbh     updated the calls to ASM and PA with WCDMA interfaces     
08/03/12   vss     Change to add band to tx_cmd_dispatch     
07/29/12   ac      wcdma Rumi bringup changes
07/25/12   tks     call flow updates for PA & ASM      
07/24/12   kb      Override the global variables asm_device and pa_device     
                   with a local copy.  
07/24/12   tks     Move the PA & ASM init from set_mode to tune_to_chan          
07/24/12   ka      interface update
07/22/12   tks     call flow updates for ASM & PA      
07/18/12   ac      changes related to the event enum 
07/28/12   tks     Fixed compile errors   
07/16/12   Saul    WCDMA. Added APIs to set LTE PA state and RGI.
07/11/12   shb     Replaced rf_buffer_ptr_reset() with rf_buffer_clear()
07/10/12   Saul    WCDMA. Added tune_with_lte_settings API.
06/21/12   tks     Changed the device funtion calls to fit the new format   
06/21/12   ars     Update FTM_SET_FREQ_ADJUST command to use new TCXOMGR API for blocking therm read     
07/02/12   sn      PA/ASM device interface and scripting support
06/28/12   nvvm    (Ported)Ensure tx_agc_adj_pdm is not overwritten in FTM_SET_TX_ON 
06/22/12   anr     EPT Calibration Enhancements
06/14/12   anr     Removed EPT specific FTM functionality from this file.
06/06/12   cri     FW Interface updates for EPT
06/07/12   shb     Added DPD support in WCDMA TX tune API - disabled for now
06/05/12   anr     Initial EPT Support
03/19/12   jfc     Remove clkrgm* header files
03/19/12   ka      MSM8974 compilation fixes     
03/19/12   ka      Removed obsolete headers   
05/16/12   vb      Added F3 msg when DAC Cal is starting 
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
05/15/12   jl      Give max UTRAN for FTM RF mode
05/11/12   adk     Fixes for DAC calibration failures in WCDMA mode
05/02/12   dw      Fix XO Cal Trim inconsistency after online->FTM
05/01/12   kai     FTM_SET_TX_POWER_DBM support after WCDMA NS test
04/27/12   swb     set_tx_off: move DPCCH confg, TxLM confg and EUL waveform Disable before set PA OFF
04/26/12   ems     Ensure device 1 is unvoted upon exit 
04/25/12   swb     set cal state to true during dac cal
04/23/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
04/20/12   adk     Use IREF_6 cal codes for IREF_5 mission mode.
04/17/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
04/13/12   kai     Revert the following update_txlm_buffer change since gsm layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
03/26/12   vb      Added support for new RF device function to program DAC Iref
03/22/12   vb      Removed TxLM Dynamic update during DAC Cal as doing HWIO writes directly
03/22/12   vb      Increased the Iref settling time to 50us
03/10/12   ks      Hookup Cal v3 ET Enable/Disable actions to device
03/07/12   cri     Added initial DPD support
03/06/12   kai     Added API for setting/getting Tx datapath IQ gain
02/15/12   sar     Removed FEATURE_MDSP_LNA_RANGE_OLD_VALUES_0137 and
                   RF_HAS_VOYAGER2_SUPPORT.
02/14/12   sb      added log_dmss.h
02/14/12   swb     Added API for FTM Tx Frequency Adjustment
01/28/12   sar     Feature reduction changes.
02/02/12   kg      Ensure builds with FEATURE_RF_HAS_QFE1320 defined also work on discrete pa targets
01/27/12   ac      fix for TX getting turned off during XO cal
01/26/12   gh      Update deallocate_rxlm to take in Rx chain as arguement
12/20/11   sn      Added support for QFE1320
01/11/12   vb      Do HDET Enable & Disable only once for all HDET reads
12/28/11   adk     Updated arguments of rfcommon_mdsp_extract_dac_cal_results()
12/21/11   dw      Do not tune TxPLL is FTM tune if Tx is not enabled
12/20/11   ars     Fix to return return RxAGC in dBm10 instead of dB for GET_RX_LEVEL_DBM
12/12/11   dw      Fix bug in FTM SET CHAN for band 19.
12/01/11   ac      part of XO cal fix
11/21/11   vb      Changes for DAC config based on the recommended Iref (which in
                   turn is based on current device tx band port in use)
11/21/11   vb      DAC cal cleanup/optimizations
11/04/11   vb      Remove Tx ON and Tx OFF for DAC cal on every Iref
11/03/11   swb     WCDMA Non-signaling Power Control Override
11/02/11   vb      Support for performing DAC cal on dynamic number of Iref values
10/28/11   gh      Add API to indicate to rf_task ftm_wcdma_enter/exit complete
10/27/11   ars     Mapping RF Driver API to FTM Layer for FTM_GET_RL_LEVEL_DBM
10/26/11   dw      Support for closed loop AGC in FTM
10/20/11   swb     Removed reset to max pdm at end of cal sweep
10/18/11   dw      Fix Composite Cal for Diversity
10/17/11   ars     Added FTM_SET_TX_POWER_DBM
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE.
09/25/11   vb      Support for DAC cal using different Irefs
09/13/11   vb      DAC cal being done with RGI 39 by default
09/12/11   vb      Warning fix
09/08/11   vb      Support for DAC Cal
09/06/11   dw      Fix warning
09/06/11   dw      Enable APT
08/25/11   sar     Removed obsolete file bbrx.h and the commented out files.
08/24/11    gh     Move CalV3 implementations out to separate file
08/23/11   ac      tech specific command dispatch
08/21/11   ac      BC19 related changes
08/18/11   vb      Added support for splitting rfm_enable_tx &
                   added rfm_init_wcdma_tx
08/11/11   vb      Addded RF_SMPS_APT_DEFAULT_SMPS_PDM_VAL
08/11/11   vb      Turn Tx OFF for every time a ftm_wcdma_set_mode happens
08/04/11   vb      Added FTM common Internal device calibration support
07/27/11   aro     Updated config/retune radio interface to have return
07/20/11   aro     Added Rx Measurement OpCode
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/20/11   aro     Temporarily commented uncompatible Cal v3 code
07/20/11    gh     Fixed warning
07/19/11   aro     Renamed RX flag masks
07/18/11    gh     Add support for CalV3
07/18/11    dw     legacy features clean up
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen
                   of rf card files
07/11/11    vb     Added B3 support in ftm_wcdma_tune_to_multi_chan()
06/23/11    dw     Merge SC/DC DVGA offset funcitons
06/22/11   sar     Removed deprecated files rftx.h and rfagc.h.
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/16/11    dw     Change AGC wait time to 3ms for composite Cal
06/15/11    dw     Fix dual carrier DVGA gain offset
06/15/11    dw     Turn off Tx at the end of TxRx sweep.
06/09/11    dw     Change the AGC wait time back to 20ms for serial Cal until long settling time is resolved
06/08/11    dw     Fixes for composite cal:1.fix wrong uninstall timer function call. 2. adjust the cgagc settling delay.
06/05/11    vb     PA SMPS Override support
06/05/11    vb     Corrected default PDM equivalent of 3.4v at PA
06/01/11    dw     Implicitly tune diversity if RxD is enabled
05/23/11    dw     Remove rfwcdmalib_msm.h
05/17/11    dw     Remove unnecessary mdsp flush cache
05/19/11    dw     Fix warning
05/19/11    dw     Disable Tx C0 trigger workaround
05/18/11    dw     Correct IQ unity set point
05/18/11   vb      Warning fix
05/17/11   vb      RxLM support for XO Cal
05/17/11   dw      Remove unnecessary mdsp flush cache
05/15/11   dw/vb   Added programmable waits for RxAGC to settle and for Tx ON (for 8960 bringup)
05/14/11    dw     Remove redundant AGC cmd in set secondary chain
05/12/11    dw     Fix/workarounds for RxAGC stability.
                   Tear down DPCCH and TxLM when turn of Tx. Workaround for Tx C0 Trigger Time
05/10/11    dw     Bringup update
04/27/11   sar     Removed deprecated file srch_mdsp.h.
04/28/11   vb      Fix compiler warnings
04/28/11   sar     CMI-4 Cleanup.
04/27/11   dw      Use common MSM Hal function to install RF Cal timer
04/26/11   dw      CMI compliance changes
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/21/11   ad      Fix KW errors
04/20/11   jhe     correct band type
04/20/11   ad      Fix warning
04/11/11   sar     Replaced rfwcdma_core_convert_band_rfcom_to_rfnv with
                   rfnv_wcdma_convert_band_rfcom_to_rfnv and included rfnv_wcdma.h.
04/06/11   ad      Port APT changes from MDM9k
04/04/11   dw      Tweak the timeline in composite Cal
04/01/11   dw      Fix warnings
03/25/11   dw      Use ftm_rfmode_enter() and remove clk regime calls
03/09/11   sar     Replaced clk.h with DALSys.h and updated clk_busy_wait api.
03/03/11   dw      Temporary featurization for 8960 builds
03/01/11   dw      change band type in rfc_apt_supported_type
02/24/11   dw      Merge mdm9k changes for NPA and DC composite Cal support
02/23/11   dw      Fix compiler warnings
02/22/11   dw      Include RxLM buffer index in AGC cmd
01/28/11   dw      Initial TxLM support for WCDMA
01/31/10   dw      Linker error fix for 8960
01/28/11   bmg     Changed clk_def to timer_def2 to complete timer changes
01/26/11   dw      updates for 8960 build
01/19/11   dw      variable name change for RxLM
01/05/11   dw      Initial support for RxLM
10/29/10   ka      Updates for initial WCDMA APT functionality
10/19/10   vb      All device functions are now accessed thru device interface functions
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1
10/08/10   ka      Call enter mode when enabling diversity to fix calibration
                   crash if 1x is calibrated before wcdma with no reset
09/08/10   dw      Remove unused mdsp header for mdm9k
09/01/10   ka      Turn on the PA when enabling Tx
08/30/10   ac      Remove Intlock from FTM code
07/19/10   ka      Removed warnings
07/19/10   ka      Do not disable tx clocks.
07/14/10   dw      Enable modem clocks for both Rx chain in set mode
06/03/10   dw      set rxagcmin to -512 when getting lna offset
05/12/10   ac      B11 bringup in j8220
05/12/10   ka      Corrected resampler settings for Voyager 2.
05/05/10   ad      Remove obsolete header for qdsp6 targets.
04/28/10   ka      Call clkregime only for Voyager 2
04/23/10   ad      Set MODEM_CLK_HALT in FTM mode only
04/16/10   ad      Control Tx clocks through clk regime in FTM mode
04/14/10   ad      Resolve access timeout issue on Voyager 2
03/19/10   ka      Compilation fixes for WPLT
03/16/10   kguo    Merged from scmm_final_merge branch
03/15/10   ad      Remove direct gpio writes (fixes jtag dependency issue)
03/10/10   ka      Fix dvga offset functions.
03/08/10   ka      Correct hdet reading in ftm_uplink_sweep_cal_isr.
03/03/10   wen     Fixed compilation errors
02/16/10   vb      Lint error fixes
02/10/12   vb      Klockwork error fixes
01/25/10   vb      Lint error fixes
01/21/10   sar     Lint fixes.
02/18/10   wen     Added 9k feature
02/10/10   ka      Removed unnecessary clkregime call.
02/09/10   kguo    Fixes from B0 bringup
02/04/10   wen     Remove "adie.h"
01/29/10   ad      Workaround for INO capture in ftm_hspa_lib.c
01/20/10   kguo    Enable feature FEATURE_WCDMA_MDSP_QDSP6 for ftm_wcdma_ctl
01/12/10   kguo    Merged mdm9k dev branch to main\latest.
                   cleaned up after merge
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.

12/23/09   bn      Compiler warning fixes
12/08/09    bn     Separate Tx and Rx PLL for current savings
11/20/09   ac      fixed compile errors for plf7 release
11/11/09   ka      Dual carrier updates, tx support.
10/27/09   ka      Changed names to fix link errors.
10/23/09   kguo    Merged changes for 9k bringup
09/19/09   ad      Support for new Rx/Tx AGC FW interface
08/18/09   ka      Moved pa range setting to HAL.
09/18/09   sar     Updated ADC interface for CMI.
09/17/09   sar     Removed deprecated file, adie.h for CMI conformance.
09/17/09   sar     Updated clk interface for CMI conformance.
09/08/09   bn      Composite calibration fix for BC5,BC4 and BC8
08/13/09   ckl     Fixed diversity mode re-entering issue and LNA switching error
08/11/09   bn      FTM WCDMA SET TxAGC API support
08/11/09   bn      FTM WCDMA Release 7 waveform feature
07/07/09   bn      Replacing MSM_OUT with HWIO_OUT macros
07/06/09   ckl     Fixed DVGA gain offset calculation error.
05/27/09   bn      Tx LUT changes
05/08/09   ckl     Fixed LNA status switching.
06/30/08   dw      Unload 1x mdsp image through SRCH
05/23/08   jfc     Make ant1_samp_clk_src_div static since its address is passed
                    to another task. Also enable RXF1 clk resources since we
                    rely on WL1 task to enable for us.
03/06/08   jtn     Fix Klocwork errors.
02/06/08    ad     Moved ftm_db_to_rf_mode() to ftm.c to fix compiler warnings
12/07/07    ad     Change arg of rftx_set_tx_agc_adj() to uint16
11/06/07    ka     Ensure 1x fw is unloaded to prevent crash.
09/14/07   jfc     Enable wcdma clocks for Tx since L1 no longer does it
08/10/07   jfc     Call rfm_enter_mode(RxDIV) since L1 no longer inits chain 1
08/08/07   jfc     Mainlined T_MSM6275 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
07/25/07   ycl     Lint Fixes.
06/05/07   lcl     Assigned FTM_NUMBER_OF_INTERBAND_GAIN_STATE_USED with the
                   value from RF drivers.
05/02/07   jfc     Update tx_rx_freq_cal_isr with LNA 4 support for chain 0
05/02/07   jfc     Add support for LNA offset 4 on chain 0.
04/13/07   ad      Setting ant clk switch through FW in ftm_wcdma_set_secondary_chain
03/23/07   jfc     Created ftm_curr_wcdma_mode variable to keep track of W band
02/28/07   ycl     Fix compiler warnings.
02/06/07   ycl     Merge in rf_wakeup.
02/06/07   ycl     Inclde rflib_umts.h instead of rflib.h.
01/23/07   jfc     Changed rf_sleep() to rf_sleep_wcdma()
12/13/06   jfc     Send STOP_GSM_MODE_REQ to GL1 when leaving GSM FTM
12/08/06   jfc     Specify MDSP app when starting WL1 for FEATURE_APP_W2G
11/12/06   ycl     Correct Syntex error.
10/31/06   ycl     Fix compilation errors for MSM7600.
10/17/06   ycl     Modifications for initial MSM7600 build.
07/13/06    xw     Type casting -- compilation error fix for 7200.
07/06/06    ka     Added error checking for hdet reads.
06/27/06    bn     Adding 5th LNA offset in ftm_tx_rx_freq_cal_isr
06-19-06   jac     Created enum type mapping function to fix LINT problem.
06/05/06    bn     Secondary chain support for ftm_tx_rx_freq_cal API
06-05-06   jac     Fixed LINT high errors.
05/31/06    bn     5th LNA State Support/ftm_second_chain_test_call changes
05/25/06    dp     Make sure clkregim is setup on diversity path.  We can no longer
                   assume RxD controller in L1 does this.
04/20/06    xw     Included task.h to get around compilation error for non gps build.
04/19/06    ka     Updated Tx-Rx API for diversity/lpm changes.
04/12/06    bn     Added ftm_set_secondary_chain and ftm_second_chain_test_call API for UMTS targets only
04/12/06    bn     changes to tune to default channel in ftm_wcdma_set_mode API
04/07/06   lcl     Added DC calibration for interBand CM.
03/09/06    ka     Merged in 6280 changes.
03/09/06    ka     Added support for tx-rx vs freq calibration.
03/09/06    ka     Fixed pointer usage in ftm_tx_rx_freq_cal_log_results().
03/06/06   dp/bhas Update for diversity WCDMA support.  Ensure SW does not
                   set micro override for AGC1 (AGC_CTL_4 reg) in the set PA range function.
02/21/06   tws     Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
                   Use channel RX/TX offsets from bsp.h
02/07/06    ka     Added ftm_do_internal_device_calibration().
12/14/05    ka     Clear result accumulator to fix hdet readings for tx-rx sweep.
09/21/05    bn     Tx off cleanup and Tx Sweep CAL off would reset tx_agc_adj to max
02/28/05    xw     Changed modified Bc and Bd to 1447 (Bc=15,Bd=15).
10/28/04    xw     Updated modified betaC and betaD for MSM6275
10/05/04    ka     Removed artificial limits for tx sweep cal min/max pdm.
09/03/04    xw     Removed rfgsmctl_set_wcdma_ant_switch_settings() and
                   rfgsmctl_cm_init() in ftm_wcdma_set_mode().
                   Increased 10ms delay in ftm_wcdma_get_dvga_gain_offset().
09/03/04    dp     Set a default band for ftm_wcdma_set_mode when switching
                   into WCDMA mode from non-wcdma mode.  This is so the RF
                   driver initializes properly.
08/30/04    eh     Renamed rfgsmctl_change_ant_switch_settings to rfgsmctl_set_wcdma_ant_switch_settings
08/23/04    xw     Removed ftm_uplink_dummy_isr().
08/20/04    ka     Fixed tx sweep cal timing.
08/19/04    ka     Added command to configure tx sweep cal.
08/12/04    ka     Added calls to set antenna correctly in set mode function.
06/09/04   bhas    hdet tracking added
08/12/03    lcl    Added ftm_gsm_rx_disable() to disable gsm rx bursting on
                   transition to WCDMA mode.
05/23/03    xw     Moved the trigger into ftm_uplink_sweep_cal_isr and
                   increased Tx Cal Sweep time interval from 10 to 20 msec.
04/15/03    xw     Added ftm_ to FTM functions and variables.
12/12/02    xw     Removed ftm_send_sbi_itm() and added ftm_uplink_sweep_cal_isr.
09/05/02    eh     Removed sbi_init, adie_init, and rf_init call.
08-27-02   rjr     Created.
===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"
#include <stdio.h>
#include <math.h>
#include "rfm_wcdma_ftm.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmicap.h"

#ifdef FTM_HAS_UMTS

#include "rfcommon_time_profile.h"
#include "ftm_wcdma_ctl.h"
#include "mcpm_api.h"
#include "ftm_msg.h"
#include "ftm_gsm_ctl.h"
#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "DALSys.h"
#include "rf.h"
#include "rfm.h"
#include "rfm_ftm.h"
#include "rfcom.h"
#include "rfwcdma_core_util.h"
#include "rfm_wcdma.h"
#include "ftm.h"
#include "ftm_v.h"
#include "rf_test_task_v.h"
#include "rfnv.h"
#include "rfnv_wcdma.h"
#include "assert.h"
#include "ftm_log.h"
#include "rfdevice_wcdma_intf.h"
#include "rflib_umts.h"
#include "rfdevice_type_defs.h"
#include "log_dmss.h"
#include "rfwcdma_mdsp.h"
#include "rfwcdma_msm.h"
#include "wfw_sw_intf.h"
#include "wfw_mod_intf.h"
#include "bsp.h"
#include "rflib_msm.h"
#include "task.h"
#include "ftm_gsm_ctl.h"
#include "rfm_wcdma.h"
#include "tcxomgr.h"
#include "rfwcdma_msm.h"
#include "ftm_wcdma_dispatch.h"
#include "rfcommon_math.h"
#ifdef FEATURE_WCDMA_RX_DIVERSITY
#include "rf.h"
#include "rxdiv.h"
#include "math.h"
#include "timetick.h"
#include "npa.h"
#endif /* FTM_HAS_WCDMA_RX_DIVERSITY */
#include "ftmdiag.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "lm_types.h"
#include "ftm_common_msm.h"
#include "wl1api.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_msm_cal.h"
#include "rfc_card.h"
#include "rfdevice_cmn_intf.h"
#include "rfwcdma_core_txplim.h"
#ifdef FEATURE_RF_HAS_QFE1320
#error code not present
#endif /*FEATURE_RF_HAS_QFE1320*/
#include "rfcommon_core.h"
#include "rf_hal_buffer.h"
#include "rfcommon_msg.h"
#include "rfdevice_lte_interface.h"

#ifdef FEATURE_RF_HAS_QTUNER
#include "rfwcdma_core_antenna_tuner.h"
#include "rfdevice_hdet_wcdma_intf.h"
#endif
#include "rfdevice_wcdma_coupler_intf.h"

#ifdef FEATURE_LTE
#include "rflte_core_util.h"
#include "rflte_ext_mc.h"
#include "rflte_mc.h"
#endif /*FEATURE_LTE*/

#include "rfdevice_wcdma_asm_intf.h"
#include "rfdevice_wcdma_pa_intf.h"
#include "rfdevice_papm_intf.h"
#include "rfcommon_mc.h"
#include "rfc_wcdma_data.h"
#include "ftm_common_xpt.h"
#include "rfwcdma_core.h"
#include "rfcommon_data.h"
#include "rfcommon_core_utils.h"
#include "rfwcdma_core_xpt.h"
#include "ftm_wcdma_calv3.h"
#include "ftm_calibration_data_module.h"
#include "rfwcdma_core_temp_comp.h"
/* memscpy */
#include "stringl.h"
#include "ftm_common_iq_processing.h"
#include "ftm_common_xpt.h"
#include "ftm_common_dispatch.h" /* trigger IQ capture */
#include "rflm_dm_api.h"
#include "rfwcdma_mdsp_types.h"
#include "rfwcdma_mdsp_async.h"
#include "rfwcdma_mdsp_sync.h"
#include "rflm_api_wcdma.h"
#include "rfwcdma_data.h"
#include "rfm_wcdma_param_types.h"
#include "rfwcdma_core_txctl.h"
#include "ftm_wcdma_selftest.h"
#include "ftm_common_selftest.h"
#include "rflm_time.h"
#include "rfwcdma_mc.h"
#include "rfcommon_fbrx_types.h"
#include "ftm_common_concurrency_manager.h"

extern rfwcdma_mdsp_rx_wakeup_cfg_type cfg;

#define  CLK_HALT    0xffffffff
#define  CLK_UNHALT  0x0
#define  MND_UPDATE  0xffffffff

extern WfwTxDpcchAsyncWriteStruct *rf_wfw_intf_dpcch_write_ptr;
extern WfwTxAgcAsyncWriteStruct *rf_wfw_intf_txagc_async_write_ptr;
extern WfwSwIntfStruct *rf_fwsw_intf_addr_ptr;

extern void rfm_set_calibration_state(boolean cal_enable);
/* Macro to redefine the rf_tune_to_chan call. To allow support of different API across target. */
#define RF_TUNE_TO_CHAN(path, chan)         rfm_tune_to_chan(path, chan, (rfm_synth_tune_enum_type)RFCOM_NUM_TUNES, NULL, NULL)

#define FTM_DVGA_GAIN_OFFSET_MAX      511
#define FTM_DVGA_GAIN_OFFSET_MIN     -512

/* Lower limit for tx sweep interval in units of sleep clock periods */
#define FTM_TX_SWEEP_INTERVAL_LOWER_LIMIT 65  /* Approx. 2 ms */

#define FTM_NUMBER_OF_INTERBAND_GAIN_STATE_USED   2
//RFLIB_MSM_NUM_OF_DC_ACCUM_USED
#define FTM_NUMBER_OF_DC_CAL_LOOP   4 /* Loop through gain range this many time during DC cal */

#define FTM_SWEEP_INTERM_LEVEL 140

#define FTM_WCDMA_DC_CHANNEL_OFFSET 25

/*! These two constants are used by XPT capture configuration for the purpose of self-test */
/*! Whenever capture type of EVM or VSWR is requested these values take effect instead of  */
/*! coming from DPD CONFIG NV */
#define FTM_WCDMA_SELFTEST_SEARCH_CENTER 55
#define FTM_WCDMA_SELFTEST_SEARCH_WIDTH 15

int2 ftm_tx_cal_sweep_hdet_val[FTM_UPLINK_SWEEP_CAL_SIZE];

/*for bringup added this flag for immediate and script based write*/
boolean lna_set_range_immediate_write_flag = TRUE;

hdet_tracking_params_type *hdet_wcdma_tracking_params_ptr;

/* Period to call HDET tracking callback function */
int4 hdet_cb_interval = 1;

/* Clock Callback structure */
LOCAL timer_type  *cb_struct;


LOCAL int2 num_of_hdet_reads = 0;
LOCAL int4 local_hdet_val = 0;

/* Tx AGC Adjust PDM for Tx Sweep Cal */
LOCAL int2 tx_agc_adj_pdm;

/* Maximum Tx AGC Adjust PDM for Tx Sweep Cal */
LOCAL int2 tx_agc_adj_pdm_max;

/* HDET read index */
LOCAL int2 hdet_index = 0;

/* Internal counter for uplink Cal sweep to skip odd number of each 10ms interrupt */
LOCAL int2 uplink_sweep_int_cnt = 0;

/* Current Tx AGC Adj PDM value */
LOCAL int2 cur_tx_agc_adj_pdm = 40;

/* Current PA Range value */
LOCAL uint8 cur_pa_range = 0;

/* The number of Tx sweep */
LOCAL int2 hdet_outloop_index = 0;

LOCAL boolean ftm_uplink_sweep_cal_enable = FALSE;

/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;

/* To track antenna path. 0 is for WCDMA/GSM, while 1 is for AGPS */
int ftm_antenna_path = 0;

/* Length of each step of the tx cal sweep, units are sleep clock periods */
uint16 ftm_wcdma_tx_cal_sweep_interval = 655; /* 20 ms */

/* Number of HDET readings to take during each step of the tx cal sweep */
byte ftm_wcdma_num_of_avg_hdet_read = 10;

static uint8  ftm_uplink_sweep_cal_step = 10;

static uint16 ftm_tx_rx_freq_cal_cur_seg = 0;

static uint16 ftm_tx_rx_freq_cal_channels[FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint8  ftm_tx_rx_freq_cal_pa_ranges[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint16 ftm_tx_rx_freq_cal_pdm_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint8  ftm_tx_rx_freq_cal_hdet_read_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint8  ftm_tx_rx_freq_cal_rx_actions[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static int16  ftm_tx_rx_freq_cal_exp_agc[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static int16  ftm_tx_rx_freq_cal_rx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];
static uint16 ftm_tx_rx_freq_cal_tx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint8  ftm_tx_rx_steps_per_freq = 0;
static uint8  ftm_tx_rx_num_freqs = 0;
static uint16 ftm_tx_rx_chain = 0;

static rftx_pdm_limit_type ftm_wcdma_pdm_limits;

// This varaible is used to maintain information of acquired IQ samples.
static ftm_wcdma_iq_acquired_samples_data_type ftm_wcdma_iq_acquired_samples_data = { RFM_DEVICE_0, 0, 0, NULL, FALSE };
// Variable that is used as Response packet for GET SAMPLES Command
static ftm_iq_capture_get_samples_rsp_pkt_type ftm_wcdma_iq_capture_get_samples_rsp_pkt;

//Variable that is used as Response packet for ANALYZE_SAMPLES
static ftm_iq_capture_analyze_samples_rsp_pkt_type ftm_wcdma_iq_capture_analyze_samples_rsp_pkt;

// This varaible is used to maintain information of acquired IQ samples.
static ftm_rf_fbrx_iq_acquired_samples_data_type ftm_rf_fbrx_iq_acquired_samples_data = { RFM_DEVICE_0, 0, NULL, NULL, FALSE };
// Variable used as Response packet for FTM_RF_MEAS_TX_PARAMS command
static ftm_rf_fbrx_meas_tx_params_rsp_pkt_type ftm_rf_fbrx_meas_tx_params_rsp_pkt;
// Variable used as Response packet for FTM_RF_MEAS_FBRX_NOISE command
static ftm_rf_fbrx_meas_noise_params_rsp_pkt_type ftm_rf_fbrx_meas_noise_params_rsp_pkt;
// Variable used as Response packet for FTM_FBRX_IQ_CAPTURE_TRIGGER command
static ftm_rf_fbrx_iq_capture_trigger_rsp_pkt_type ftm_rf_fbrx_iq_capture_trigger_rsp_pkt;
// Variable used as Response packet for FTM_FBRX_IQ_CAPTURE_GET_SAMPLES command
static ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt_type ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt;
// Variable that is used as Response packet for FTM_GET_MULTIPLE_RX_LEVEL_DBM
static ftm_get_multiple_rx_level_dbm_rsp_pkt_type ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt;

/* Maximum number of devices is RFM_DEVICE_4*/
static ftm_get_multi_synth_lock_data_type ftm_wcdma_aggregated_multi_synth_lock_status[RFM_DEVICE_4] =
{
  { FTM_SYNTH_STATUS_UNDETERMINED, FTM_SYNTH_STATUS_UNDETERMINED },
  { FTM_SYNTH_STATUS_UNDETERMINED, FTM_SYNTH_STATUS_UNDETERMINED },
  { FTM_SYNTH_STATUS_UNDETERMINED, FTM_SYNTH_STATUS_UNDETERMINED },
  { FTM_SYNTH_STATUS_UNDETERMINED, FTM_SYNTH_STATUS_UNDETERMINED }
};

boolean rx_cal_optimization = FALSE;

uint8 ftm_wcdma_eul_mpr_idx = 0;

#ifdef RF_HAS_DM_SUPPORT
extern void rflib_msm_get_dc(uint16 *iaccum, uint16 *qaccum);
#endif

rfcom_wcdma_band_type ftm_curr_wcdma_mode;

pout_tbl_type ftm_wcdma_pout_tbl =
{
  1,
  { 0 },
  { 0 },
  POUT_DATA_TYPE_MAX
};

/* Pointer to MMPA & ASM device */
//void* pa_device = NULL;
//void* asm_device = NULL;

/* Band information need for PA & ASM devices */
rfcom_band_type_u tx_band;

/* SMPS PA bias override enalbe*/
boolean ftm_wcdma_smps_pa_bias_override = FALSE;

/* SMPS PA bias override value */
uint16 ftm_wcdma_smps_pa_bias_override_val = 3700;

/* FTM RxD ON indicator */
boolean ftm_wcdma_div_status[RFM_DEVICE_4] = {FALSE,FALSE,FALSE,FALSE};

/* FTM Tx ON indicator */
boolean ftm_wcdma_tx_on_status = FALSE;

ftm_lm_buf_type ftm_wcdma_rxlm_buffer[RFM_DEVICE_4] = { { 0, FALSE }, { 0, FALSE }, { 0, FALSE }, { 0, FALSE } };
ftm_lm_buf_type ftm_wcdma_txlm_buffer = { 0, FALSE };

/* Create a NPA Handle */
npa_client_handle ftm_wcdma_npa_handle_q6sw = NULL;
npa_client_handle ftm_wcdma_npa_handle_q6fw = NULL;

#include "rex.h"
#include "rf_task.h"
extern rex_tcb_type rf_tcb;

ftm_lm_buf_type ftm_wcdma_xpt_fb_path_buffer = { 0, FALSE };

boolean ftm_wcdma_10mhz_mode_bb = FALSE;
boolean ftm_wcdma_10mhz_mode_rf = FALSE;
int2 ftm_dvga_offset_nbpwr = 0;

extern boolean rfcommon_fbrx_mc_enable_rf_dev(rfdevice_rxtx_common_class *dev_ptr,
                                              uint8 gain_state);

extern boolean rfcommon_fbrx_mc_disable_rf_dev(rfdevice_rxtx_common_class *dev_ptr);

int16 ftm_wcdma_tx_power_dbm;

extern uint32 ftm_rf_current_test_scenario;

/*===========================================================================

FUNCTION FTM_WCDMA_SET_UPLINK_WAVEFORM

DESCRIPTION
   This function will program the encoder to generate a WCDMA uplink waveform.
   It is used only for RF testing purposes.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_wcdma_set_uplink_waveform(void);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_PA

DESCRIPTION
   This function will set the pa on/off.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_wcdma_set_pa(rfm_device_enum_type device, boolean state);


/*===========================================================================

FUNCTION   FTM_UPLINK_SWEEP_CAL_ISR

DESCRIPTION
   FTM Tx Sweep Calibration service routine.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_uplink_sweep_cal_isr(void);



/*===========================================================================

FUNCTION   FTM_WCDMA_GET_DVGA_GAIN_OFFSET_MEAS_NBPWR

DESCRIPTION
   FTM Tx Sweep Calibration service routine.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
int2 ftm_wcdma_get_dvga_gain_offset_meas_nbpwr(rfm_device_enum_type device, word expected_agc_val);

/*===========================================================================

FUNCTION   ftm_wcdma_convert_ftm_band_to_rfcom_band_type

DESCRIPTION
   utility funtion to convert ftm_rf_mode_type to rfcom_wcdma_band_type

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
rfcom_wcdma_band_type ftm_wcdma_convert_ftm_band_to_rfcom_band_type(ftm_rf_mode_type ftm_band)
{
  switch (ftm_band)
  {
  case FTM_DB_RF_WCDMA_IMT:
    return RFCOM_BAND_IMT;

  case FTM_DB_RF_WCDMA_1900MHZ_A:
  case FTM_DB_RF_WCDMA_1900MHZ_B:
    return RFCOM_BAND_1900;

  case FTM_DB_RF_WCDMA_BC3:
    return RFCOM_BAND_BC3;

  case FTM_DB_RF_WCDMA_BC4:
    return RFCOM_BAND_BC4;

  case FTM_DB_RF_WCDMA_800MHZ:
    return RFCOM_BAND_800;

  case FTM_DB_RF_WCDMA_BC8:
    return RFCOM_BAND_BC8;

  case FTM_DB_RF_WCDMA_BC9:
    return RFCOM_BAND_BC9;

  case FTM_DB_RF_WCDMA_BC11:
    return RFCOM_BAND_BC11;

  case FTM_DB_RF_WCDMA_BC19:
    return RFCOM_BAND_BC19;

  default:
    FTM_MSG_ERROR("Invalid FTM RF band", ftm_band, 0, 0);
    return RFCOM_BAND_INVALID;
  }
}
/*!
  @brief
  Inidates if RX cal optimization is enabled

  @details
  This function shows if RX CAL optimization is enabled
 
  @return
  boolean rx_cal_optimization
  
*/

void ftm_wcdma_enable_NBEE(void)
{
    rx_cal_optimization = TRUE;
}


/*!
  @brief
  Inidates if RX cal optimization is disabled

  @details
  This function shows if RX CAL optimization is disabled
 
  @return
  
*/
void ftm_wcdma_disable_NBEE(void)
{
	rx_cal_optimization = FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
FUNCTION   ftm_wcdma_convert_rfcom_band_type_to_ftm_band

DESCRIPTION
   utility funtion to convert rfcom_wcdma_band_type to ftm_rf_mode_type

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rf_mode_type ftm_wcdma_convert_rfcom_band_type_to_ftm_band(rfcom_wcdma_band_type rfcom_band)
{
  switch (rfcom_band)
  {
  case  RFCOM_BAND_IMT:
    return FTM_DB_RF_WCDMA_IMT;

  case  RFCOM_BAND_1900:
    return FTM_DB_RF_WCDMA_1900MHZ_A;

  case  RFCOM_BAND_BC3:
    return FTM_DB_RF_WCDMA_BC3;

  case  RFCOM_BAND_BC4:
    return FTM_DB_RF_WCDMA_BC4;

  case  RFCOM_BAND_800:
    return FTM_DB_RF_WCDMA_800MHZ;

  case  RFCOM_BAND_BC8:
    return FTM_DB_RF_WCDMA_BC8;

  case  RFCOM_BAND_BC9:
    return FTM_DB_RF_WCDMA_BC9;

  case  RFCOM_BAND_BC11:
    return FTM_DB_RF_WCDMA_BC11;

  case  RFCOM_BAND_BC19:
    return FTM_DB_RF_WCDMA_BC19;

  default:
    FTM_MSG_ERROR("Invali RFCOM WCDMA band", rfcom_band, 0, 0);
    return 0;
  }
}


rfcom_wcdma_band_type ftm_wcdma_convert_phone_mode_to_rfcom_band_type
(
  ftm_mode_id_type phone_mode
  )
{
  rfcom_wcdma_band_type rf_band;

  switch (phone_mode)
  {
  case FTM_PHONE_MODE_WCDMA_IMT:
    rf_band = RFCOM_BAND_IMT;
    break;

  case FTM_PHONE_MODE_WCDMA_1900A:
  case FTM_PHONE_MODE_WCDMA_1900B:
    rf_band = RFCOM_BAND_1900;
    break;

  case FTM_PHONE_MODE_WCDMA_BC3:
    rf_band = RFCOM_BAND_BC3;
    break;

  case FTM_PHONE_MODE_WCDMA_BC4:
    rf_band = RFCOM_BAND_BC4;
    break;

  case FTM_PHONE_MODE_WCDMA_800:
    rf_band = RFCOM_BAND_800;
    break;

  case FTM_PHONE_MODE_WCDMA_BC8:
    rf_band = RFCOM_BAND_BC8;
    break;

  case  FTM_PHONE_MODE_WCDMA_BC9:
    rf_band = RFCOM_BAND_BC9;
    break;

  case FTM_PHONE_MODE_WCDMA_BC11:
    rf_band = RFCOM_BAND_BC11;
    break;

  case FTM_PHONE_MODE_WCDMA_BC19:
    rf_band = RFCOM_BAND_BC19;
    break;

  default:
    rf_band = RFCOM_BAND_INVALID;
    FTM_MSG_ERROR("Invalid FTM RF band", phone_mode, 0, 0);
    break;
  }

  return rf_band;
}

/*===========================================================================

FUNCTION FTM_WCDMA_SET_PDM

DESCRIPTION
    This function takes a pdm value and writes it to the specified PDM. Prior 
    to this FTM_WCDMA_SET_TX & FTM_WCDMA_SET_PA_RANGE should have been called.  

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Will automatically OPEN the PDM loop even if it was previously closed.
   This allows applications a more intuitive approach to using this function.
===========================================================================*/
void ftm_wcdma_set_pdm(ftm_pdm_id_type pdm_id, int2 val)
{

  rftx_pdm_limit_type pdm_limits;
  rfdevice_lut_idx_pa_range_type lut_pa_data;
  rfdevice_tx_lut_data_type lut_data;
  rfcom_band_type_u tech_band;
  rfc_wcdma_apt_supported_type apt_config = { RFCOM_BAND_IMT,
    RFC_WCDMA_APT_NOT_SUPPORTED, 0 };
  rfdevice_rxtx_common_class *device_ptr;


  /*Update curr band information*/
  lut_pa_data.band = ftm_curr_wcdma_mode;
  tx_band.wcdma_band = ftm_curr_wcdma_mode;

  /* Find the PDM which we want to adjust and set it to the value. */
  switch (pdm_id)
  {
  case FTM_PDM_TX_AGC_ADJ:
    /* Get RF device Tx AGC limitation value */
    /*! @todo Need to get the rfm_device info from the caller function */
    rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                   ftm_curr_wcdma_mode,
                                   RFDEVICE_GET_TX_AGC_LIMITS,
                                   &pdm_limits);
    val = RF_SATURATE(val, (int2)pdm_limits.min_pdm, (int2)pdm_limits.max_pdm);
    lut_pa_data.lut_idx = (uint8)val;
    lut_pa_data.pa_range = cur_pa_range;

    /* Get the device pointer for the 1st WTR */
    device_ptr= rfdevice_wcdma_tx_get_common_dev_ptr(RFM_DEVICE_0, tx_band.wcdma_band);

    if (NULL != device_ptr)
    {
      if (rfdevice_cmn_is_cmn_tx_intf_supported(device_ptr) == FALSE)
      {
        /* Set Tx AGC */
        /*! @todo Need to get the rfm_device info from the caller function */
        rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                       ftm_curr_wcdma_mode,
                                       RFDEVICE_SET_LUT_INDEX,
                                       (uint8 *)&lut_pa_data);
      }
      else
      {
        tech_band.wcdma_band = ftm_curr_wcdma_mode;
        rfdevice_cmn_get_tx_lut_scripts(
          device_ptr,
          RFM_IMT_MODE,
          tech_band,
          rfwcdma_core_util_get_pa_range_map_nv_index(cur_pa_range),
          (uint8)val,                      //rgi
          RFDEVICE_EXECUTE_IMMEDIATE,
          &lut_data);
      }
    }
    else
    {
      RF_MSG(RF_ERROR,  "ftm_wcdma_set_pdm(): "
             "device_ptr is NULL. Unable to get Tx lut table");
    }

    /* Get APT config */
    apt_config.band = ftm_curr_wcdma_mode;
    rfc_wcdma_command_dispatch(RFM_DEVICE_0,
                               RFC_WCDMA_GET_APT_CONFIG,
                               (void *)(&apt_config));

    if (!ftm_wcdma_smps_pa_bias_override)
    {
      /* If APT/GST is not supported and no SMPS overriden done,
         then need to program default value for SMPS */

      ftm_wcdma_set_smps_pa_bias_val(apt_config.default_smps_val);
    }
    else
    {
      /*SMPS overriding enabled, write the last overriden value*/

      ftm_wcdma_set_smps_pa_bias_val(ftm_wcdma_smps_pa_bias_override_val);
    }

    /* Set baseband i/q gain */
    rf_wfw_intf_txagc_async_write_ptr->txAgcIqGainUnitySetPoint = 0x473;
    rf_wfw_intf_txagc_async_write_ptr->txAgcSsbiDisabled = 1;

    cur_tx_agc_adj_pdm = val;
    break;

  case FTM_PDM_TRK_LO_ADJ:
    FTM_MSG_HIGH("FTM_PDM_TRK_LO_ADJ is not supported!", 0, 0, 0);
    break;

  default:
    FTM_MSG_HIGH("PDM is out of range %d", pdm_id, 0, 0);
    break;
  } /* switch */

} /* end ftm_wcdma_set_pdm */

/*===========================================================================

FUNCTION FTM_WCDMA_SET_SMPS_PA_BIAS_VAL

DESCRIPTION
    This function sets the SMPS PA Bias Value in FTM mode only.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_wcdma_set_smps_pa_bias_val(uint16 val)
{
  /*Keep track of the overriden value for future use*/
  ftm_wcdma_smps_pa_bias_override_val = val;

  rfdevice_wcdma_papm_set_mode_bias(RFM_DEVICE_0,
                                    ftm_curr_wcdma_mode,
                                    RFDEVICE_APT_MODE,
                                    val,
                                    NULL,
                                    RFDEVICE_EXECUTE_IMMEDIATE);
} /*ftm_wcdma_set_smps_pa_bias_val*/
/*===========================================================================

FUNCTION FTM_WCDMA_SET_PA_CURRENT

DESCRIPTION
    This function set PA quiescent current

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_wcdma_set_pa_current(uint8 val)
{

  rfdevice_wcdma_pa_set_current_bias(RFM_DEVICE_0,
                                     ftm_curr_wcdma_mode,
                                     cur_pa_range,
                                     (uint16) val,
                                     NULL,
                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                     RFCOM_INVALID_TIMING_OFFSET);

}
/*===========================================================================

FUNCTION FTM_WCDMA_SET_SMPS_PA_BIAS_OVERRIDE

DESCRIPTION
    This function overrides the SMPS PA Bias tables and whenever PDM
    is set (in FTM mode only), the overriden value will take effect.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_wcdma_set_smps_pa_bias_override(boolean val)
{

  switch (val)
  {
  case 0:
    ftm_wcdma_smps_pa_bias_override = FALSE;

    break;

  case 1:
    ftm_wcdma_smps_pa_bias_override = TRUE;

    break;

  default:
    FTM_MSG_ERROR("SMPS PA Bias Override flag invalid", 0, 0, 0);
    break;
  }

} /*ftm_wcdma_set_smps_pa_bias_override*/

/*===========================================================================

FUNCTION FTM_WCDMA_SET_LNA_RANGE

DESCRIPTION
  This function sets the LNA range 0 (highest gain) to 3 (lowest gain).


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_lna_range(rfm_device_enum_type device, byte ftm_lna_range)
{

  rfwcdma_mdsp_set_data_param_type lna_range_param = { 0 };
  rf_buffer_intf *buffer = NULL;
  rfdevice_wcdma_lna_param_type lna_state;
  rf_path_enum_type path = 0;


  path = rfcommon_core_device_to_path(device);
  lna_state.band          = rfwcdma_mc_state.car_path_state[path][0].curr_band_rx;
  lna_state.device        = device;
  lna_state.carrier       = rfwcdma_mc_state.rx_carriers[device];
  lna_state.current_state = ftm_lna_range;

  if (ftm_wcdma_rxcal_opt_enabled() && IS_FTM_IN_WCAL_MODE())
  {
    MSG_3(MSG_SSID_FTM, MSG_LEGACY_ERROR, "RXCAL OPTIMIZED configuration setting: , lna_state=%d, num_carrier=%d, band=%d",
          lna_state.current_state, lna_state.carrier, lna_state.band);

    buffer = rf_buffer_create
      (RFWCDMA_MAX_SSBI_SCRIPT_SIZE,
       RFWCDMA_MAX_RFFE_SCRIPT_SIZE,
       RFWCDMA_MAX_GRFC_SCRIPT_SIZE);

    if (buffer == NULL)
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_FATAL, "rf buffer returned NULL! ", 0);
      ASSERT(0);
    }
    /* Clear buffer */
    (void)rf_buffer_clear(buffer);

    rfdevice_wcdma_rx_lna_setup(device, rfwcdma_mc_state.car_path_state[path][0].curr_band_rx, &lna_state, RFDEVICE_EXECUTE_IMMEDIATE, buffer);

    rf_buffer_destroy(buffer);
  }
  else
  {
    /* Update LNA range param structure */
    lna_range_param.set_data_cmd = RFWCDMA_MDSP_SET_LNA_GAIN_STATE;
    lna_range_param.data.set_lna_state.state = ftm_lna_range;
    if (ftm_wcdma_rxlm_buffer[device].is_buf_allocated)
    {
      lna_range_param.rflm_handle = (rflm_dm_handle_id_t)ftm_wcdma_rxlm_buffer[device].buf_idx;
    }
    else
    {
      FTM_MSG_ERROR("ftm_wcdma_set_lna_range: FTM RxLM not allocated for device:%d", device, 0, 0);
      return;
    }

    /* Push data to MDSP */
    (void)rfwcdma_mdsp_async_set_data(&lna_range_param);
  }

} /* ftm_wcdma_set_lna_range() */

/*===========================================================================

FUNCTION FTM_WCDMA_SET_LNA_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_lna_offset(rfm_device_enum_type device, byte index, int2 setting)
{
  rfwcdma_mdsp_set_data_param_type lna_offset_param = { 0 };

  lna_offset_param.set_data_cmd = RFWCDMA_MDSP_SET_LNA_GAIN_OFFSET;
  lna_offset_param.data.set_lna_offset.lna_gain_state = index;
  lna_offset_param.data.set_lna_offset.offset = setting;

  if (ftm_wcdma_rxlm_buffer[device].is_buf_allocated)
  {
    lna_offset_param.rflm_handle = (rflm_dm_handle_id_t)ftm_wcdma_rxlm_buffer[device].buf_idx;
  }
  else
  {
    FTM_MSG_ERROR("ftm_wcdma_set_lna_offset: FTM RxLM not allocated for device:%d", device, 0, 0);
    return;
  }

  (void)rfwcdma_mdsp_async_set_data(&lna_offset_param);
}

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_lna_offset(rfm_device_enum_type device, byte index, word expected_agc_val)
{

  if (index > 0)
  {
    /* If the index is less than the max number of LNA range, set the LNA Range to the current index */
    if (index <= (byte)FTM_LNA_RANGE_5)
    {
      ftm_wcdma_get_lna_offset_setup(device, index);
      DALSYS_BusyWait(3000);
    }
  }
  if (ftm_wcdma_rxcal_opt_enabled())
  {
  	return ( ftm_wcdma_get_lna_offset_meas_nbpwr( device,index, expected_agc_val));
  }
  else
  {
    return (ftm_wcdma_get_lna_offset_meas(device, index, expected_agc_val));
  }
}

/*===========================================================================

FUNCTION FTM_WCDMA_SET_MODE

DESCRIPTION
    This function will switch modes to WCDMA.

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   When a mode is changed WHILE transmitting, this function will
   automatically shut down the transmit for the old mode and enter the
   transmit state for the new mode.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_set_mode(rfm_device_enum_type device, ftm_mode_id_type mode)
{
  boolean change_mode = FALSE;
  uint16 default_channel = 9750;
  //rfm_device_enum_type device = RFM_DEVICE_0;
  ftm_rsp_pkt_type response_pkt={FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  uint8 path = rfcommon_core_device_to_path(device);
  
  /* Check if we are in FTM Mode. If not return. */
  if( ftm_mode != FTM_MODE )
  {
    FTM_MSG_ERROR("Not in FTM Mode - mode = %d ",ftm_mode,0,0);
    response_pkt.cmd=FTM_RSP_BAD_MODE;
    return response_pkt;
  }


  /* If it's already in DB_RF_WCDMA mode, skip setting the same mode again */
  if ( ( 
          (ftm_current_rf_mode != FTM_DB_RF_WCDMA_IMT)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_1900MHZ_A)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_1900MHZ_B)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC3)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC4)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_800MHZ)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC8)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC9)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC11)
          && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC19) 
       ) ||
       ( 
         (rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_SLEEP) 
       )
     )
  {
    /* Check if  ftm_rfmode_enter returns TRUE and proceed. If not return saying not in FTM mode. */
    if(ftm_rfmode_enter(device,FTM_STATE_WCDMA))
    {
      ftm_rfmode_enter(device, FTM_STATE_WCDMA);
      /*! @todo need to revisit the following two lines. Do we still 
          need them for NikeL/Dime */
      /* hardcoded values for rxlm buffer is set to zero for bringup R'99 call) */
      rfm_enter_mode(device, RFCOM_WCDMA_MODE, NULL, NULL, ftm_wcdma_rxlm_buffer[device].buf_idx);
      rfm_enable_rx(device, NULL, NULL);
    }
    else
    {
       response_pkt.cmd=FTM_RSP_BAD_MODE;
       return response_pkt;
    }
  }

  /*Disable Tx for every time a ftm_wcdma_set_mode happens*/
  /*!@brief
     Whenever there is a failure during RF Cal say during linearizer sweep,
     SUITE does not send Tx OFF. Now when the RF Cal starts for following band,
     SUITE directly gives TX ON and this Tx ON shall be ignored by FTM code,                  
     as Tx is already ON */
  if (device == RFM_DEVICE_0)
  {
    ftm_wcdma_set_tx(device, FALSE);
  }

  /* Set the current RF mode to WCDMA */
  if (mode == FTM_PHONE_MODE_WCDMA_IMT)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_IMT;
    ftm_curr_wcdma_mode = RFCOM_BAND_IMT;
    change_mode = TRUE;
    default_channel = 9750;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_1900A)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_1900MHZ_A;
    ftm_curr_wcdma_mode = RFCOM_BAND_1900;
    change_mode = TRUE;
    default_channel = 9400;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_1900B)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_1900MHZ_B;
    ftm_curr_wcdma_mode = RFCOM_BAND_1900;
    change_mode = TRUE;
    default_channel = 9400;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_BC3)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC3;
    ftm_curr_wcdma_mode = RFCOM_BAND_BC3;
    change_mode = TRUE;
    default_channel = 1112;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_BC4)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC4;
    ftm_curr_wcdma_mode = RFCOM_BAND_BC4;
    change_mode = TRUE;
    default_channel = 1412;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_800)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_800MHZ;
    ftm_curr_wcdma_mode = RFCOM_BAND_800;
    change_mode = TRUE;
    default_channel = 4175;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_BC8)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC8;
    ftm_curr_wcdma_mode = RFCOM_BAND_BC8;
    change_mode = TRUE;
    default_channel = 2787;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_BC9)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC9;
    ftm_curr_wcdma_mode = RFCOM_BAND_BC9;
    change_mode = TRUE;
    default_channel = 8837;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_BC11)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC11;
    ftm_curr_wcdma_mode = RFCOM_BAND_BC11;
    change_mode = TRUE;
    default_channel = 3525;
  }
  else if (mode == FTM_PHONE_MODE_WCDMA_BC19)
  {
    ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC19;
    ftm_curr_wcdma_mode = RFCOM_BAND_BC19;
    change_mode = TRUE;
    default_channel = 338;
  }
  else if (mode == FTM_PHONE_MODE_SLEEP)
  {
    /* Exit RF Mode for all devices */
    for (device = RFM_DEVICE_0; device < RFM_DEVICE_4; device++)
    {
      ftm_rfmode_exit(device, FTM_STATE_PARK);
    }
    change_mode = FALSE;
  }

  ///* Remove calls to tune_to_chan from set mode*/
  //if (change_mode)
  //{
  //  ftm_wcdma_tune_to_chan(RFM_DEVICE_0, default_channel);
  //}

  return response_pkt;
}        /* end ftm_wcdma_set_mode */


/*===========================================================================

FUNCTION FTM_WCDMA_SET_TX

DESCRIPTION
   This function will set the TX state for WCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_wcdma_set_tx(rfm_device_enum_type device, boolean state)
{
  /* ftm_wcdma_set_pdm() will overwrite cur_tx_agc_adj_pdm; hence save its
     current value, before initializing the pdm to zero */
  int2 cur_tx_agc_adj_pdm_prev = cur_tx_agc_adj_pdm;
  ftm_xpt_txagc_override_type ovr_params;
  rfm_wcdma_tx_init_params tx_i_params;
  rfm_wcdma_tx_enable_params tx_e_params;
  rfm_wcdma_tx_disable_params tx_d_params;
  rfm_wcdma_tx_deinit_params tx_deinit_params;

  memset(&tx_i_params, 0, sizeof(rfm_wcdma_tx_init_params)); 
  memset(&tx_e_params, 0, sizeof(rfm_wcdma_tx_enable_params)); 
  memset(&tx_d_params, 0, sizeof(rfm_wcdma_tx_disable_params)); 
  memset(&tx_deinit_params, 0, sizeof(rfm_wcdma_tx_deinit_params)); 

  tx_i_params.device = device;
  tx_i_params.txlm_handle = ftm_wcdma_txlm_buffer.buf_idx;

  tx_e_params.device =device; 

  tx_d_params.device = device; 

  tx_deinit_params.device =device;

  if ((state == ON) && (ftm_wcdma_tx_on_status == FALSE))
  {
    wmcpmdrv_rf_ftm_mode_req(TRUE);

    /* restore the saved value */
    cur_tx_agc_adj_pdm = cur_tx_agc_adj_pdm_prev;

    if (ftm_wcdma_10mhz_mode_rf)
    {
      tx_i_params.carrier_mask = 3; 
      rfm_wcdma_tx_init(&tx_i_params,NULL,NULL);
    }
    else
    {
      tx_i_params.carrier_mask = 1;               
      rfm_wcdma_tx_init(&tx_i_params,NULL,NULL); 
    }

    rfm_wcdma_tx_enable(&tx_e_params, NULL, NULL);

    /* Set ldo_off_flag to FALSE */
    rfwcdma_core_tx_ftm_disable_ldo_off(rfwcdma_mc_state.txlm_buf_idx, FALSE);

    rfm_turn_on_pa();

    /* Turn on the PA & other tx front end devices */
    ftm_wcdma_set_pa(device,1);
    ftm_clk_rex_wait(1);

    ftm_send_txlm_cfg_cmd(ftm_wcdma_txlm_buffer.buf_idx, TRUE);

    /* Make sure we program all gains in the Tx chain to lowest during
    Tx Cal to avoid false trigger. This has to be done between TxLM
    cfg cmd and DPCCH enable */
    
    /* Do this only in Cal sweep, then Tx config segement will set 
    those to the right gain */
    if (IS_FTM_IN_WCAL_MODE())
    {
      /* Program QPOET into APT mode */
      /* Get the current Tx override */

      ovr_params.device = RFM_DEVICE_0;
      ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
      ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_VAL;
      /* Zero out IQ gain */
      ovr_params.iq_gain = 0;
      ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
      /* Set RGI to min */
      ovr_params.rgi = ftm_wcdma_pdm_limits.min_pdm;
      /* Set bias to 2000 mV */
      ovr_params.smps_bias = 2000;
      /* Set PA state to lowest */
      ovr_params.pa_state = 0;
      /* Set PA Q-current to 0 */
      ovr_params.pa_curr = 0;
      ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;

      ftm_wcdma_xpt_override_rf_settings( &ovr_params );

    }

    /* We really want to see a WCDMA pilot so we will go ahead and turn
    ** on right here. At this time there is no real reason to do
    ** it elsewhere.
    */
    ftm_wcdma_set_uplink_waveform();

    /* set Tx power to the last known value, only after the PA has actually been turned ON */
    ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, cur_tx_agc_adj_pdm);

    /* Update tx state variable to reflect the new TX state. */
    ftm_wcdma_update_aggregated_multisynth_status(device);

    ftm_wcdma_tx_on_status = TRUE;
    FTM_MSG_HIGH("Setting TX ON", 0, 0, 0);
  }

  else if ((state == FALSE) && (ftm_wcdma_tx_on_status == ON))
  {
    ftm_disable_eul_waveform();

    /* Turn off the PA & other tx front end components */
    if (ftm_wcdma_txlm_buffer.is_buf_allocated)
    {
      rfm_wcdma_tx_disable(&tx_d_params, NULL,NULL);
    }
    else
    {
      FTM_MSG_ERROR("TxLM buffer not allocated while trying to disable Tx!", 0, 0, 0);
      rfm_wcdma_tx_disable(&tx_d_params, NULL,NULL);
    }

    /* Make sure PA is turned off after WTR off */
    ftm_wcdma_set_pa(device,0);

#ifdef FEATURE_TXLM
    ftm_uplink_dpcch_config(FALSE);
    ftm_send_txlm_cfg_cmd(ftm_wcdma_txlm_buffer.buf_idx, FALSE);
    rfm_wcdma_tx_deinit(&tx_deinit_params, NULL, NULL);

#endif

    ftm_wcdma_tx_on_status = FALSE;
    /* Update tx state variable to reflect the new TX state. */

    ftm_wcdma_update_aggregated_multisynth_status(device);

    FTM_MSG_HIGH("Setting TX OFF", 0, 0, 0);
  } /* else */
} /* end ftm_wcdma_set_tx */

/*===========================================================================

FUNCTION FTM_WCDMA_SET_DVGA_GAIN_OFFSET

DESCRIPTION
  This function sets the CDMA Rx VAGC offset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_dvga_gain_offset(rfm_device_enum_type device, int2 dvgaGainOffset)
{
  rfwcdma_mdsp_set_data_param_type dvga_offset_param = { 0 };

  dvga_offset_param.set_data_cmd = RFWCDMA_MDSP_SET_VGA_GAIN_OFFSET;
  dvga_offset_param.data.set_vga_gain_offset.offset = dvgaGainOffset;

  if (ftm_wcdma_rxlm_buffer[device].is_buf_allocated)
  {
    dvga_offset_param.rflm_handle = (rflm_dm_handle_id_t)ftm_wcdma_rxlm_buffer[device].buf_idx;
  }
  else
  {
    FTM_MSG_ERROR("ftm_wcdma_set_dvga_gain_offset: FTM RxLM not allocated for device:%d", device, 0, 0);
    return;
  }

  (void)rfwcdma_mdsp_async_set_data(&dvga_offset_param);
}
/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET

DESCRIPTION
  This function returns the WCDMA Rx DVGA gain offset.

DEPENDENCIES
  None

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_dvga_gain_offset(rfm_device_enum_type device, word expected_agc_val)
{
  ftm_wcdma_get_dvga_gain_offset_setup(device);

  DALSYS_BusyWait(3000);
  if (ftm_wcdma_rxcal_opt_enabled())
  {
    if (IS_FTM_IN_WCAL_MODE() == TRUE)
    {
      return (ftm_wcdma_get_dvga_gain_offset_meas_nbpwr(device, expected_agc_val));
    }
    else
    {
      return (ftm_wcdma_get_dvga_gain_offset_meas(device, expected_agc_val));
    }
  }
  else return (ftm_wcdma_get_dvga_gain_offset_meas(device, expected_agc_val));
}

/*===========================================================================

FUNCTION FTM_WCDMA_SET_PA

DESCRIPTION
   This function will set the pa on/off.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_wcdma_set_pa(rfm_device_enum_type device, boolean state)
{

  if (state)
  {
    rf_wfw_intf_txagc_async_write_ptr->paForceOff = 0;

    /* Turn on the PA */
    rfdevice_wcdma_pa_on_off(device,
                             ftm_curr_wcdma_mode,
                             TRUE,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET);
  }
  else
  {
    rf_wfw_intf_txagc_async_write_ptr->paForceOff = 1;

    /* Turn off the PA */
    rfdevice_wcdma_pa_on_off(device,
                             ftm_curr_wcdma_mode,
                             FALSE,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET);
  }
} /* end wcdma_set_pa */

/*===========================================================================

FUNCTION FTM_WCDMA_SET_UPLINK_WAVEFORM

DESCRIPTION
   This function will program the encoder to generate a WCDMA uplink waveform.
   It is used only for RF testing purposes.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_wcdma_set_uplink_waveform(void)
{
  /* Configurate modulator timing in mDSP */
  ftm_wcdma_modulator_timing();

  /* Configurate DPCCH in mDSP */
  ftm_uplink_dpcch_config(TRUE);

  /* Configurate Uplink modulator */
  ftm_config_modulator();

  ftm_wcdma_eul_mpr_idx = 0;
}

/*===========================================================================
FUNCTION  FTM_WCDMA_SET_PA_RANGE

DESCRIPTION
  This function sets PA range 0 (low gain) or 1 (high gain).

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_wcdma_set_pa_range(byte paRange)
{
  rfdevice_lut_idx_pa_range_type lut_pa_data;

  /*Update curr band information*/
  lut_pa_data.band = ftm_curr_wcdma_mode;
  lut_pa_data.lut_idx = (uint8)cur_tx_agc_adj_pdm;
  lut_pa_data.pa_range = paRange;
  tx_band.wcdma_band = ftm_curr_wcdma_mode;

  /*! @todo (tks) Need to check PA range behavior for transceiver & MMPA */
  rfdevice_wcdma_pa_set_gain_range(RFM_DEVICE_0,
                                   ftm_curr_wcdma_mode,
                                   lut_pa_data.pa_range,
                                   NULL,
                                   RFDEVICE_EXECUTE_IMMEDIATE,
                                   RFCOM_INVALID_TIMING_OFFSET);

  cur_pa_range = paRange;
}

/*===========================================================================

FUNCTION ftm_uplink_sweep_cal

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_uplink_sweep_cal(boolean enable)
{
  /* Lock interrupt to prevent ftm_uplink_sweep_cal_isr() kicking in and changing cur_tx_agc_adj_pdm */

  ftm_uplink_sweep_cal_enable = enable;

  if (ftm_uplink_sweep_cal_enable)
  {
    if (cur_tx_agc_adj_pdm > FTM_UPLINK_SWEEP_CAL_MAX)
    {
      /* Preventing user set the maximum Tx power exceeding FTM_UPLINK_SWEEP_CAL_MAX */
      cur_tx_agc_adj_pdm = FTM_UPLINK_SWEEP_CAL_MAX;
    }

    /* Now store the maximum Tx AGC adjust PDM specified by the user */
    tx_agc_adj_pdm_max = cur_tx_agc_adj_pdm;

    hdet_index = 0;

    hdet_outloop_index = 0;

    /* Reset tx_agc_adj_pdm back to the maximum */
    tx_agc_adj_pdm = tx_agc_adj_pdm_max;

    /* SUITE sends the cal sweep time in sleep time ISR, hence need to convert into milli-sec*/
    ftm_common_msm_install_timer((uint32)(ftm_wcdma_tx_cal_sweep_interval / 32), ftm_uplink_sweep_cal_isr);

  }
  else
  {
    ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, 0);
    ftm_common_msm_uninstall_timer();
  }

  /* Reset uplink Cal sweep internal counter */
  uplink_sweep_int_cnt = 0;

} /* ftm_uplink_sweep_cal() */

/*===========================================================================

FUNCTION   FTM_UPLINK_SWEEP_CAL_ISR

DESCRIPTION
   FTM Uplink Sweep Calibration service routine.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_uplink_sweep_cal_isr(void)
{
  int ftm_adc_read_cnt;
  uint16 local_temp_hdet_val;

  /* Reset local_hdet_val */
  local_hdet_val = 0;

  /* Enable HDET */
  /*! @todo Need to get the rfm_device info from the caller function */
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_ENABLE_HDET, NULL);

  for (num_of_hdet_reads = 0; num_of_hdet_reads < ftm_wcdma_num_of_avg_hdet_read; num_of_hdet_reads++)
  {
    ftm_adc_read_cnt = 0;
    do
    {
      /*! @todo Need to get the rfm_device info from the caller function */
      rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_GET_HDET_VALUE, &local_temp_hdet_val);

      ftm_adc_read_cnt++;
    }
    while (((local_temp_hdet_val <= 0) || local_temp_hdet_val == 0xFFFF)
           && (ftm_adc_read_cnt < 3));

    local_hdet_val += local_temp_hdet_val;
  }

  /* Disable HDET */
  /*! @todo Need to get the rfm_device info from the caller function */
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_DISABLE_HDET, NULL);

  if (ftm_uplink_sweep_cal_enable && (uplink_sweep_int_cnt == 0))
  {
    /* Force a low to high transition and serve as a trigger for spectrum analyzer */
    /* If the pdm lower limit is not zero then drop the pdm in two steps to avoid
     * a glitch in the output waveform. */
    if (ftm_wcdma_pdm_limits.min_pdm > 0 && tx_agc_adj_pdm > FTM_SWEEP_INTERM_LEVEL)
    {
      ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, FTM_SWEEP_INTERM_LEVEL);
      DALSYS_BusyWait(20);
    }
    /* Do not use the absolute lowest pdm since it may cause a glitch in the output */
    ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_wcdma_pdm_limits.min_pdm + 1);
  }
  else if (ftm_uplink_sweep_cal_enable)
  {
    if (uplink_sweep_int_cnt > 1)
    {
      ftm_tx_cal_sweep_hdet_val[hdet_index - 1] =
        (int2)local_hdet_val / ftm_wcdma_num_of_avg_hdet_read;
    }

    /* Do not use the absolute lowest pdm since it may cause a glitch in the output */
    if (tx_agc_adj_pdm <= ftm_wcdma_pdm_limits.min_pdm)
    {
      tx_agc_adj_pdm = (int2)ftm_wcdma_pdm_limits.min_pdm + 1;
    }
    ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, tx_agc_adj_pdm);

    /* Decrease Tx power level by the amount of ftm_uplink_sweep_cal_step until it reaches
       the minimum pdm plus one. */
    if (tx_agc_adj_pdm > ftm_wcdma_pdm_limits.min_pdm + 1)
    {
      tx_agc_adj_pdm -= ftm_uplink_sweep_cal_step;
      hdet_index++;
    }

    else
    {
      /* Fill the rest of unused ftm_tx_cal_sweep_hdet_val[] with zeros */
      do
      {
        ftm_tx_cal_sweep_hdet_val[hdet_index] = 0;
      }
      while (hdet_index++ < FTM_UPLINK_SWEEP_CAL_SIZE - 1);

      /* Reset tx_agc_adj_pdm back to the maximum */
      tx_agc_adj_pdm = tx_agc_adj_pdm_max;

      /* Reset hdet_index */
      hdet_index = 0;

      /* It does the Tx sweep again in case the spectrum analyzer trig missed the 1st sweep */
      hdet_outloop_index++;

      /* turn off sweep after one full iterations */
      if (hdet_outloop_index == 1)
      {
        ftm_uplink_sweep_cal(OFF);
      }
    }
  }
  uplink_sweep_int_cnt++;
}

/*===========================================================================

FUNCTION       FTM_HDET_TRACKING_CB

DESCRIPTION
  1.25ms callback function for closed-loop tracking of HDET.
  Adjusts TX_AGC_ADJ PDM to control HDET to the desired set point.
===========================================================================*/
void ftm_hdet_tracking_cb(int4 interval)
{
#if defined(FEATURE_FTM_HWTC) && defined(FTM_HAS_LOGGING)
  ftm_hdet_track_debug_log_type *hdet_log = NULL;
  static unsigned int log_count = 4;
#endif /* FEATURE_FTM_HWTC  && FTM_HAS_LOGGING */

  unsigned int hdet_accum;             // kept at unsigned 16Q8
  unsigned int new_pdm_val;
  byte rf_hdet_data_trk;
  uint16 temp_hdet;
  uint8 hdet_count = 0;

  const unsigned short hdet_accum_scale = 63701; // 0.972 in unsigned 16Q16
  const unsigned short hdet_update_scale = 1835; // 0.028 in unsigned 16Q16
  const short pdm_proportional_scale = 152;    // -0.00463 in signed 16Q15

  /* Enable HDET */
  /*! @todo Need to get the rfm_device info from the caller function */
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_ENABLE_HDET, NULL);

  do
  {
    /*! @todo Need to get the rfm_device info from the caller function */
    rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode,
                                   RFDEVICE_GET_HDET_VALUE,
                                   &temp_hdet);
    hdet_count++;
  }
  while (temp_hdet == 0xFFFF && hdet_count <= 5);

  /* Disable HDET */
  /*! @todo Need to get the rfm_device info from the caller function */
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_DISABLE_HDET, NULL);

  rf_hdet_data_trk = (byte)temp_hdet;

  hdet_accum = hdet_wcdma_tracking_params_ptr->hdet_accum;
  hdet_accum = hdet_update_scale * (rf_hdet_data_trk << 8)
    + hdet_accum_scale * hdet_accum;
  hdet_accum = hdet_accum >> 16; // Convert from 32Q24 back to 16Q8
  hdet_wcdma_tracking_params_ptr->hdet_accum = hdet_accum;

  /* Compute the new PDM value based on the HDET error */
  new_pdm_val = hdet_wcdma_tracking_params_ptr->hdet_tracking_pdm;
  new_pdm_val = new_pdm_val + pdm_proportional_scale * (((int)hdet_wcdma_tracking_params_ptr->hdet_set_point << 8) - (int)hdet_accum);
  /*   32Q23    =    32Q23    +     16Q15           *(           8Q0   to  16Q8                          -          16Q8   */

  hdet_wcdma_tracking_params_ptr->hdet_tracking_pdm = new_pdm_val;

  /* Round and write to PDM */
  ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, ((new_pdm_val + (1 << 22)) >> 23));

#if defined(FEATURE_FTM_HWTC) && defined(FTM_HAS_LOGGING)
  log_count--;
  if (log_count == 0)
  {
    log_count = 4;
    hdet_log = (ftm_hdet_track_debug_log_type *)ftm_log_malloc(0xF000, sizeof(ftm_hdet_track_debug_log_type));
    if (hdet_log != NULL)
    {
      hdet_log->hdet = rf_hdet_data_trk;
      hdet_log->filt_hdet = (int16)hdet_accum;
      hdet_log->tx_agc_pdm = (new_pdm_val + (1 << 22)) >> 23;
      log_commit(hdet_log);
    }

  }
#endif /* FEATURE_FTM_HWTC && FTM_HAS_LOGGING */


} /* ftm_HDET_tracking */

/*===========================================================================

FUNCTION       FTM_WCDMA_SET_HDET_TRACKING

DESCRIPTION
  If state is true, HDET tracking function is called, which does HDET tracking till the state becomes FALSE
===========================================================================*/
void ftm_wcdma_set_hdet_tracking(boolean state, unsigned short set_point)
{
  if (state)
  {
    /* Allocate space */
    if (hdet_wcdma_tracking_params_ptr == NULL)
    {
      hdet_wcdma_tracking_params_ptr = ftm_malloc(sizeof(hdet_tracking_params_type));

      if (hdet_wcdma_tracking_params_ptr == NULL)
      {
        FTM_MSG_ERROR("Failed to allocate memory for ISR parameter structure", 0, 0, 0);
        return;
      }
    }

    /* Setup the parameter block */
    hdet_wcdma_tracking_params_ptr->hdet_accum = set_point << 8;
    hdet_wcdma_tracking_params_ptr->hdet_set_point = set_point;
    hdet_wcdma_tracking_params_ptr->hdet_tracking_pdm = 450U << 23;

    /* Initialize the PDM to a close value */
    ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (hdet_wcdma_tracking_params_ptr->hdet_tracking_pdm >> 23));

    /* Give the HDET circuit time to settle */
    ftm_clk_rex_wait(50);

    /* Start the FTM HDET Callback to do closed loop control */

    /* Allocate space for callback structure */
    cb_struct = (timer_type *)ftm_malloc(sizeof(timer_type));
    if (cb_struct == NULL)
    {
      FTM_MSG_ERROR("Failed to allocate memory for callback structure", 0, 0, 0);
      return;
    }

    /* Initialize and register a callback structure */
    timer_def2(cb_struct, NULL);
    timer_reg(cb_struct,
              (timer_t2_cb_type)ftm_hdet_tracking_cb,
              (timer_cb_data_type)NULL,
              (uint32)hdet_cb_interval,
              (uint32)hdet_cb_interval);
  }
  else
  {
    /* Diable the FTM HDET Callback function */
    /* Deregister a callback structure */
    (void)timer_clr(cb_struct, T_NONE);
  }
}

/*===========================================================================

FUNCTION FTM_WCDMA_CONFIGURE_TX_SWEEP_CAL

DESCRIPTION
  This function allows the user to set the length of each step in the tx cal
  sweep and the number of HDET readings averaged per step.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_configure_tx_sweep_cal(uint16 interval, byte num_avg)
{
  if (interval >= FTM_TX_SWEEP_INTERVAL_LOWER_LIMIT) /* 2 ms */
  {
    ftm_wcdma_tx_cal_sweep_interval = interval;
  }
  else
  {
    FTM_MSG_ERROR("Tx sweep interval %d is too low. Lower Limit is 65 ~= 2ms.", 0, 0, 0);
  }

  ftm_wcdma_num_of_avg_hdet_read = num_avg;

}

/*===========================================================================

FUNCTION FTM_WCDMA_SET_TX_SWEEP_STEP_SIZE

DESCRIPTION
  This function allows the user to set the PDM step size for the TX cal sweep.
  The PDM will be decreased by the given number of counts for each step in
  the sweep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_tx_sweep_step_size(uint8 step)
{
  if (step >= FTM_UPLINK_SWEEP_CAL_MIN_STEP)
  {
    ftm_uplink_sweep_cal_step = step;
  }
  else
  {
    ftm_uplink_sweep_cal_step = FTM_UPLINK_SWEEP_CAL_MIN_STEP;
    FTM_MSG_ERROR("FTM TX sweep step size too low, using %d.", ftm_uplink_sweep_cal_step, 0, 0);
  }

}

/*===========================================================================

FUNCTION FTM_WCDMA_TUNE_TO_CHAN

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_tune_to_chan(rfm_device_enum_type device, uint16 chan)
{
  rfcom_wcdma_band_type band;
  uint16 tune_channel_list[3] = { chan, 0, 0 };
  uint8 device_mask =0, carrier_index =1; // set the default value of carrier index for SC
  uint32 scenario =0;
  rfc_rx_path_type rx_path; 
  ftm_concurrency_manager_params_type wcdma_cc_params;

  /* Transceiver tx tune parameters */
  rfdevice_wcdma_tx_tune_param_type tx_params;
  tx_params.band       = RFCOM_BAND_INVALID;
  tx_params.bw         = RFDEV_WCDMA_BW_INVALID;
  tx_params.dpd_state  = RFDEVICE_DPD_NUM;

  memset(&wcdma_cc_params, 0, sizeof(ftm_concurrency_manager_params_type));

  scenario = ftm_wcdma_get_current_test_scenario();

  if(device >= RFM_DEVICE_4)
  {
    FTM_MSG_ERROR("ftm_wcdma_tune_to_chan: invalid device %d", device, 0, 0);
    return;
  }

  if (device == RFM_DEVICE_0 || device == RFM_DEVICE_2)
  {
    rx_path = RFC_PRX_PATH;
  }
  else 
  {
    rx_path = RFC_DRX_PATH;
  }


  if (device == RFM_DEVICE_2 || device == RFM_DEVICE_3)
  {
    if (scenario == 1)
    {
      carrier_index = 1;
    }
    else 
    {
      carrier_index = 2;
    }

    wcdma_cc_params.carrier_index = 1;
  }
  
  /* Rx PLL */
  band = rfwcdma_core_convert_ul_chan_to_band(chan);

  wcdma_cc_params.rfm_dev = device;
  wcdma_cc_params.band.wcdma_band = band;
  wcdma_cc_params.prx_drx =rx_path;
  wcdma_cc_params.tech = RFM_IMT_MODE; 

  if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
  {
    FTM_MSG_2( FTM_ERROR, "ftm_wcdma_tune_to_chan: Invalid concurreny scenario - device:%d, band: %d,",
                          device,band );
    return; 
  }


  tune_channel_list[0] = rfwcdma_core_map_tx_to_rx_chan(chan, band);

  rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx = chan;

  if(FTM_DB_RF_SLEEP == ftm_current_rf_mode)
  {
    rfm_enter_mode(device,RFM_IMT_MODE,NULL,NULL,ftm_wcdma_rxlm_buffer[device].buf_idx);
  }

  /* @todo Update for unified solution for DBDC */
  if (device == RFM_DEVICE_2)
  {
    rfwcdma_mc_tune_to_multi_chan(device, tune_channel_list, carrier_index, 
                                  NULL, NULL, ftm_wcdma_rxlm_buffer[device].buf_idx);
    if (ftm_wcdma_div_status[RFM_DEVICE_3] == TRUE)
    {
      wcdma_cc_params.rfm_dev = RFM_DEVICE_3;
      wcdma_cc_params.band.wcdma_band = band;
      wcdma_cc_params.prx_drx = RFC_DRX_PATH;
      wcdma_cc_params.tech = RFM_IMT_MODE; // Carrier index is already set

      if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
      {
        FTM_MSG_2( FTM_ERROR, "ftm_wcdma_tune_to_chan: Invalid concurreny scenario - device:%d, band: %d,",
                              RFM_DEVICE_3,band );
        return; 
      }

      rfwcdma_mc_tune_to_multi_chan(RFM_DEVICE_3, tune_channel_list, carrier_index, 
                                    NULL, NULL, ftm_wcdma_rxlm_buffer[RFM_DEVICE_3].buf_idx);
    }
  }
  else // RFM_DEVICE_0, RFM_DEVICE_1
  {
#if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
    device_mask |= 1<< device;
    if (ftm_wcdma_div_status[RFM_DEVICE_1] == TRUE)
    {
      device_mask |= 1<< RFM_DEVICE_1;

      wcdma_cc_params.rfm_dev = RFM_DEVICE_1;
      wcdma_cc_params.band.wcdma_band = band;
      wcdma_cc_params.prx_drx = RFC_DRX_PATH;
      wcdma_cc_params.tech = RFM_IMT_MODE; // Carrier index is already set

      if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
      {
        FTM_MSG_2( FTM_ERROR, "ftm_wcdma_tune_to_chan: Invalid concurreny scenario - device:%d, band: %d,",
                              RFM_DEVICE_1,band );
        return; 
      }
    }
    ftm_wcdma_tune_to_chan_scripts(device_mask,tune_channel_list, carrier_index);
#else
    /* Split PRx and DRx tune for DPM as currently FW does not have the support yet
       @TODO Revert this change when DPM FW is available
    */
    /* PRx tune */
    device_mask = 1<< device;
    ftm_wcdma_tune_to_chan_scripts(device_mask, tune_channel_list, 1);
    /* DRx tune if enabled */
    if (ftm_wcdma_div_status[RFM_DEVICE_1] == TRUE)
    {
      device_mask = 1<< RFM_DEVICE_1;

      wcdma_cc_params.rfm_dev = RFM_DEVICE_1;
      wcdma_cc_params.band.wcdma_band = band;
      wcdma_cc_params.prx_drx = RFC_DRX_PATH;
      wcdma_cc_params.tech = RFM_IMT_MODE; // Carrier index is already set

      if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
      {
        FTM_MSG_2( FTM_ERROR, "ftm_wcdma_tune_to_chan: Invalid concurreny scenario - device:%d, band: %d,",
                              RFM_DEVICE_1,band );
        return; 
      }

      ftm_wcdma_tune_to_chan_scripts(device_mask, tune_channel_list, 1);
    }
#endif
  }

  // Wait for script based tune warmup time if there is no Tx tune after //
  if(!ftm_wcdma_tx_on_status)
  {
    /* Based on observations, warmup time sent to FW is about 550us. */
    DALSYS_BusyWait(550);
  }

  if (ftm_wcdma_tx_on_status == TRUE)
  {
    if (device == RFM_DEVICE_0 || device == RFM_DEVICE_1)
    {
      /* Tx PLL */
      if (rfwcdma_core_xpt_is_enabled(ftm_curr_wcdma_mode))
      {
        tx_params.band      = ftm_curr_wcdma_mode;
        tx_params.bw        = RFDEV_WCDMA_BW_1X;
        tx_params.channel[0] = chan;
        tx_params.dpd_state = RFDEVICE_DPD_ENABLED;
      
        rfdevice_wcdma_tx_set_channel(RFM_DEVICE_0, RFCOM_WCDMA_MODE,
                                      ftm_curr_wcdma_mode, tx_params,
                                      RFDEVICE_EXECUTE_IMMEDIATE, NULL);
      
        FTM_MSG_HIGH("ftm_wcdma_tune_to_chan DPD enabled.", 0, 0, 0);
      }
      else
      {
        tx_params.band      = ftm_curr_wcdma_mode;
        tx_params.bw        = RFDEV_WCDMA_BW_1X;
        tx_params.channel[0] = chan;
        tx_params.dpd_state = RFDEVICE_DPD_DISABLED;
      
        rfdevice_wcdma_tx_set_channel(RFM_DEVICE_0, RFCOM_WCDMA_MODE,
                                      ftm_curr_wcdma_mode, tx_params,
                                      RFDEVICE_EXECUTE_IMMEDIATE, NULL);
      }
    }

    rfdevice_wcdma_asm_enable_tx(device,
                                 ftm_curr_wcdma_mode,
                                 NULL,
                                 RFDEVICE_EXECUTE_IMMEDIATE,
                                 RFCOM_INVALID_TIMING_OFFSET);

    rfdevice_wcdma_pa_on_off(device,
                             ftm_curr_wcdma_mode,
                             TRUE,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET);

  } 

  ftm_wcdma_update_aggregated_multisynth_status(device);
}

/*===========================================================================

FUNCTION FTM_WCDMA_TUNE_TO_MULTI_CHAN

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_tune_to_multi_chan(rfm_device_enum_type device, uint16 chan[])
{
  uint16 tx_chan = chan[0];
  rfcom_wcdma_band_type band =0;
  rfc_rx_path_type rx_path = 0; 
  uint32 scenario = 0;
  rfcom_multi_carrier_hspa_id_type mc_flag = RFCOM_DUAL_CARRIER;
  ftm_concurrency_manager_params_type wcdma_cc_params;

  /* Transceiver tx tune parameters */
  rfdevice_wcdma_tx_tune_param_type tx_params;
  tx_params.band       = RFCOM_BAND_INVALID;
  tx_params.bw         = RFDEV_WCDMA_BW_INVALID;
  tx_params.dpd_state  = RFDEVICE_DPD_NUM;

  memset(&wcdma_cc_params, 0, sizeof(ftm_concurrency_manager_params_type));

  scenario = ftm_wcdma_get_current_test_scenario();

  /* Convert UL channel to band as input is the UL channel */
  band = rfwcdma_core_convert_ul_chan_to_band(chan[0]);

  if (device == RFM_DEVICE_0 || device == RFM_DEVICE_2)
  {
    rx_path = RFC_PRX_PATH;
  }
  else 
  {
    rx_path = RFC_DRX_PATH;
  }

  if (device == RFM_DEVICE_2 || device == RFM_DEVICE_3)
  {
     if (scenario == 1)
     {
       mc_flag = RFCOM_SINGLE_CARRIER;
     }
     else 
     {
       mc_flag = RFCOM_DUAL_CARRIER;
     }
     wcdma_cc_params.carrier_index = 1;
  }

  wcdma_cc_params.rfm_dev = device;
  wcdma_cc_params.band.wcdma_band = band;
  wcdma_cc_params.prx_drx =rx_path;
  wcdma_cc_params.tech = RFM_IMT_MODE; 
  
  if(ftm_concurrency_manager_validate_operation(&wcdma_cc_params))
  {
  if (chan[2] != 0)
  {
    mc_flag = RFCOM_3_CARRIER;
  }
    
  if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_IMT)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_800MHZ)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_800_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_800_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_800_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC9)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC3)
  {
    (chan[0]) = (chan[0]) + BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET;
    (chan[1]) = (chan[1]) + BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET;
    (chan[2]) = (chan[2]) + BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC11)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET;
  }

  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC4)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET;
  }

  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_1900MHZ_A || ftm_current_rf_mode == FTM_DB_RF_WCDMA_1900MHZ_B)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC8)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC19)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET;
    chan[1] = chan[1] + BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET;
    chan[2] = chan[2] + BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET;
  }

  rfm_tune_to_multi_chan(device,
                         chan,
                         mc_flag,
                         RFM_NUM_TUNES,
                         NULL,
                         NULL,
                         ftm_wcdma_rxlm_buffer[device].buf_idx);

  if (ftm_wcdma_div_status[RFM_DEVICE_1] == TRUE)
  {
    wcdma_cc_params.rfm_dev = RFM_DEVICE_1;
    wcdma_cc_params.band.wcdma_band = band;
    wcdma_cc_params.prx_drx = RFC_DRX_PATH;
    wcdma_cc_params.tech = RFM_IMT_MODE; // Carrier index is already set

    if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
    {
      FTM_MSG_2( FTM_ERROR, "ftm_wcdma_tune_to_chan: Invalid concurreny scenario - device:%d, band: %d,",
                            RFM_DEVICE_1,band );
      return; 
    }

    rfm_tune_to_multi_chan(RFM_DEVICE_1,
                           chan,
                           mc_flag,
                           RFM_NUM_TUNES,
                           NULL,
                           NULL,
                           ftm_wcdma_rxlm_buffer[RF_PATH_1].buf_idx);
  }

  /* Based on observations, warmup time sent to FW is about 550us. */
  DALSYS_BusyWait(550);

  if (ftm_wcdma_tx_on_status == TRUE)
  {
    if (device == RFM_DEVICE_0 || device == RFM_DEVICE_1)
    {
      /* Tx PLL */
      if (rfwcdma_core_xpt_is_enabled(ftm_curr_wcdma_mode))
      {
        tx_params.band      = ftm_curr_wcdma_mode;
        tx_params.bw        = RFDEV_WCDMA_BW_1X;
        tx_params.channel[0] = tx_chan;
        tx_params.dpd_state = RFDEVICE_DPD_ENABLED;
      
        rfdevice_wcdma_tx_set_channel(RFM_DEVICE_0, RFCOM_WCDMA_MODE,
                                      ftm_curr_wcdma_mode, tx_params,
                                      RFDEVICE_EXECUTE_IMMEDIATE, NULL);
      
        FTM_MSG_HIGH("ftm_wcdma_tune_to_multi_chan DPD enabled.", 0, 0, 0);
      }
      else
      {
        tx_params.band      = ftm_curr_wcdma_mode;
        tx_params.bw        = RFDEV_WCDMA_BW_1X;
        tx_params.channel[0] = tx_chan;
        tx_params.dpd_state = RFDEVICE_DPD_DISABLED;
      
        rfdevice_wcdma_tx_set_channel(RFM_DEVICE_0, RFCOM_WCDMA_MODE,
                                      ftm_curr_wcdma_mode, tx_params,
                                      RFDEVICE_EXECUTE_IMMEDIATE, NULL);
      }
    }

    rfdevice_wcdma_asm_enable_tx(device,
                                 ftm_curr_wcdma_mode,
                                 NULL,
                                 RFDEVICE_EXECUTE_IMMEDIATE,
                                 RFCOM_INVALID_TIMING_OFFSET);
   
    rfdevice_wcdma_pa_on_off(device,
                             ftm_curr_wcdma_mode,
                             TRUE,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET);

  } 

  ftm_wcdma_update_aggregated_multisynth_status(device);
  }
  else 
  {
     FTM_MSG_2( FTM_ERROR, "ftm_wcdma_tune_to_multi_chan: Invalid concurreny scenario- device:%d, band %d ", device, band );
     return; 
  }

}

#ifdef FTM_HAS_COMMON_COMMANDS
/*===========================================================================

FUNCTION FTM_TX_RX_FREQ_CAL_LOG_RESULTS

DESCRIPTION
  This function sends a log packet with the results of the TX/RX frequency
  calibration sweep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_tx_rx_freq_cal_log_results(void)
{
  log_ftm_gen_type *log_ptr;
  uint8 *data_ptr;
  uint8 i = 0;

  /* create log packet to return results */
  /* ftm_log_gen_type contains headers only */
  /* log_ftm_gen_type contains headers and a data placeholder */
  log_ptr = (log_ftm_gen_type *)ftm_log_malloc((uint16)FTM_LOG_TX_RX_SWEEP,
                                               sizeof(ftm_log_gen_type) +
                                               2 * sizeof(uint16) * ftm_tx_rx_steps_per_freq *
                                               ftm_tx_rx_num_freqs +
                                               sizeof(uint16) * ftm_tx_rx_num_freqs +
                                               8 * ftm_tx_rx_steps_per_freq +
                                               4);

  if (log_ptr != NULL)
  {
    /* Copy the request parameters and the results into the log packet */
    data_ptr = (uint8 *)&(log_ptr->data);

    if (data_ptr != NULL)
    {
      if ((ftm_tx_rx_num_freqs <= FTM_TX_RX_FREQ_CAL_MAX_FREQS)
          && (ftm_tx_rx_steps_per_freq <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ))
      {

        /* rx_chain */
        *data_ptr = (uint8)ftm_tx_rx_chain;
        /*lint -save -e415 -e416 typecast data from uin8 to uint16 data[] pointer will not be out of bounce */
        data_ptr += sizeof(uint16);

        /* num_freqs */
        *data_ptr = ftm_tx_rx_num_freqs;
        data_ptr += sizeof(uint8);

        /* num_steps */
        *data_ptr = ftm_tx_rx_steps_per_freq;
        data_ptr += sizeof(uint8);
        /*lint -restore */

        /* channel_list */
        memscpy((void *)data_ptr, sizeof(uint16) * ftm_tx_rx_num_freqs, &ftm_tx_rx_freq_cal_channels[0],
                sizeof(uint16) * ftm_tx_rx_num_freqs);
        data_ptr += sizeof(uint16) * ftm_tx_rx_num_freqs;

        /* tx_pdm_list */
        memscpy((void *)data_ptr, sizeof(uint16) * ftm_tx_rx_steps_per_freq, &ftm_tx_rx_freq_cal_pdm_list[0],
                sizeof(uint16) * ftm_tx_rx_steps_per_freq);
        data_ptr += sizeof(uint16) * ftm_tx_rx_steps_per_freq;

        /* pri_expected_agc_val_list */
        memscpy((void *)data_ptr, sizeof(int16) * ftm_tx_rx_steps_per_freq, &ftm_tx_rx_freq_cal_exp_agc[0],
                sizeof(int16) * ftm_tx_rx_steps_per_freq);
        data_ptr += sizeof(int16) * ftm_tx_rx_steps_per_freq;

        /* primary_rx_results */
        memscpy((void *)data_ptr, sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs, &ftm_tx_rx_freq_cal_rx_result[0],
                sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs);
        data_ptr += sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs;

        /* pa_range_list */
        memscpy((void *)data_ptr, sizeof(uint8) * ftm_tx_rx_steps_per_freq, &ftm_tx_rx_freq_cal_pa_ranges[0],
                sizeof(uint8) * ftm_tx_rx_steps_per_freq);
        data_ptr += sizeof(uint8) * ftm_tx_rx_steps_per_freq;

        /* read_hdet_list */
        memscpy((void *)data_ptr, sizeof(uint8) * ftm_tx_rx_steps_per_freq, &ftm_tx_rx_freq_cal_hdet_read_list[0],
                sizeof(uint8) * ftm_tx_rx_steps_per_freq);
        data_ptr += sizeof(uint8) * ftm_tx_rx_steps_per_freq;

        /* hdet_results */
        memscpy((void *)data_ptr, sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs, &ftm_tx_rx_freq_cal_tx_result[0],
                sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs);
        data_ptr += sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs;

        /* rx_action_list */
        memscpy((void *)data_ptr, sizeof(uint8) * ftm_tx_rx_steps_per_freq, &ftm_tx_rx_freq_cal_rx_actions[0],
                sizeof(uint8) * ftm_tx_rx_steps_per_freq);
        data_ptr += sizeof(uint8) * ftm_tx_rx_steps_per_freq;

        /* lpm list - zero it out since not supported on umts targets */
        for (i = 0; i < ftm_tx_rx_steps_per_freq; i++)
        {
          *data_ptr++ = 0;
        }

      }
      else
      {
        FTM_MSG_ERROR("FTM num of Rx-Tx freq/Steps are greater than allowed", 0, 0, 0);
      }
    }
    else
    {
      FTM_MSG_ERROR("Could not access data memory - tx-rx vs freq. log packet will be incorrect!", 0, 0, 0);
    }

    /* send log */
    log_commit((ftm_log_type *)log_ptr);
  }
  else
  {
    FTM_MSG_ERROR("Could not allocate memory for freq. cal log packet", 0, 0, 0);
  }
}
#endif /* FTM_HAS_COMMON_COMMANDS */

/*===========================================================================

FUNCTION FTM_WCDMA_TX_RX_FREQ_CAL_ISR

DESCRIPTION
  This ISR is called for each step of the TX/RX frequency calibration sweep.
  It performs DVGA offset and LNA offset calibration, reads HDET, and changes
  channels as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_tx_rx_freq_cal_isr(void)
{
  uint8 num_hdet_reads, hdet_count;
  uint16 temp_hdet;
  uint8 cur_seg_within_freq = ftm_tx_rx_freq_cal_cur_seg % ftm_tx_rx_steps_per_freq;
  uint16 channels[RFCOM_MAX_CARRIERS];

  /* check if we have not yet reached the end of the sweep */
  if (ftm_tx_rx_freq_cal_cur_seg < ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs)
  {
    /* sweep not finished, check if end of current frequency */
    if (cur_seg_within_freq == ftm_tx_rx_steps_per_freq - 1)
    {
      if (rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0] == RFCOM_DUAL_CARRIER)
      {
        channels[0] = ftm_tx_rx_freq_cal_channels[(ftm_tx_rx_freq_cal_cur_seg /
                                                   ftm_tx_rx_steps_per_freq + 1) %
                                                  ftm_tx_rx_num_freqs];

        channels[1] = channels[0] + FTM_WCDMA_DC_CHANNEL_OFFSET;

        /* last step of this frequency, tune to new channel */
        ftm_wcdma_tune_to_multi_chan(RFM_DEVICE_0, channels);
      }
      else if (rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0] == RFCOM_3_CARRIER)
      {
        channels[0] = ftm_tx_rx_freq_cal_channels[(ftm_tx_rx_freq_cal_cur_seg /
                                                   ftm_tx_rx_steps_per_freq + 1) %
                                                  ftm_tx_rx_num_freqs];

        channels[1] = channels[0] + FTM_WCDMA_DC_CHANNEL_OFFSET;

        channels[2] = channels[1] + FTM_WCDMA_DC_CHANNEL_OFFSET;

        /* last step of this frequency, tune to new channel */
        ftm_wcdma_tune_to_multi_chan(RFM_DEVICE_0, channels);
      }
      else
      {
        /* last step of this frequency, tune to new channel */
        ftm_wcdma_tune_to_chan(RFM_DEVICE_0, ftm_tx_rx_freq_cal_channels[(ftm_tx_rx_freq_cal_cur_seg /
                                                                          ftm_tx_rx_steps_per_freq + 1) %
                                                                         ftm_tx_rx_num_freqs]);
      }
      ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, FTM_UPLINK_SWEEP_CAL_MIN);
    }
    else
    {
      /* still within current frequency, perform tx calibration */
      if (ftm_tx_rx_freq_cal_pa_ranges[cur_seg_within_freq] !=
            FTM_TX_RX_SWEEP_NO_ACTION)
      {
        /* set tx pa range and pdm */
        ftm_wcdma_set_pa_range(ftm_tx_rx_freq_cal_pa_ranges[cur_seg_within_freq]);

        ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ,
                          ftm_tx_rx_freq_cal_pdm_list[cur_seg_within_freq]);

        /* read hdet if desired */
        if (ftm_tx_rx_freq_cal_hdet_read_list[cur_seg_within_freq] != 0)
        {
          /* Enable HDET */
          /*! @todo Need to get the rfm_device info from the caller function */
          rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_ENABLE_HDET, NULL);

          for (num_hdet_reads = 0;
               num_hdet_reads < FTM_FREQ_CAL_SWEEP_NUM_HDET_AVG;
               num_hdet_reads++)
          {
            hdet_count = 0;
            do
            {
              /*! @todo Need to get the rfm_device info from the caller function */
              rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode,
                                             RFDEVICE_GET_HDET_VALUE,
                                             &temp_hdet);
              hdet_count++;
            }
            while (temp_hdet == 0xFFFF && hdet_count <= 3);
            ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_cur_seg] += temp_hdet;
          }
          ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_cur_seg] /=
            FTM_FREQ_CAL_SWEEP_NUM_HDET_AVG;

          /* Disable HDET */
          /*! @todo Need to get the rfm_device info from the caller function */
          rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0, ftm_curr_wcdma_mode, RFDEVICE_DISABLE_HDET, NULL);

        } /* end hdet read */
        else
        {
          ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_cur_seg] = 0;
        }
      } /* end tx section */

      /* perform rx action */
      switch (ftm_tx_rx_freq_cal_rx_actions[cur_seg_within_freq])
      {
      case FTM_RX_DVGA_OFFSET:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_dvga_gain_offset(RFM_DEVICE_0,
                                           (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_dvga_gain_offset(RFM_DEVICE_1,
                                           (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      case FTM_RX_LNA_OFFSET0:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_0,
                                     0,
                                     (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_1,
                                     0,
                                     (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      case FTM_RX_LNA_OFFSET1:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_0,
                                     1,
                                     (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_1,
                                     1,
                                     (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      case FTM_RX_LNA_OFFSET2:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_0,
                                     2, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_1,
                                     2, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      case FTM_RX_LNA_OFFSET3:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_0,
                                     3, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_1,
                                     3, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      case FTM_RX_LNA_OFFSET4:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_0,
                                     4, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_1,
                                     4, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      case FTM_RX_LNA_OFFSET5:
        if (ftm_tx_rx_chain == 0)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_0,
                                     5, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else if (ftm_tx_rx_chain == 1)
        {
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] =
            ftm_wcdma_get_lna_offset(RFM_DEVICE_1,
                                     5, (uint16)ftm_tx_rx_freq_cal_exp_agc[cur_seg_within_freq]);
        }
        else
        {
          FTM_MSG_ERROR("ftm_wcdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d", ftm_tx_rx_chain, 0, 0);
        }
        break;

      default:
        /* do nothing */
        ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_cur_seg] = 0;
        break;
      } /* end rx section */

    } /* end if not last step of this freq. */
  } /* end if not at end of sweep */
  else
  {
    /* full frequency sweep is done */
    /* turn off timer interrupts */
    ftm_common_msm_uninstall_timer();

    /* send log packet with results */
    ftm_wcdma_tx_rx_freq_cal_log_results();
    /* Turn off Tx */
    ftm_wcdma_set_tx(RFM_DEVICE_0, FALSE);
  }

  /* increment segment counter for next isr iteration */
  ftm_tx_rx_freq_cal_cur_seg++;
}

/*===========================================================================

FUNCTION FTM_WCDMA_DO_TX_RX_FREQ_CAL_SWEEP

DESCRIPTION
  This function configures and starts the TX/RX frequency calibration sweep.

DEPENDENCIES
  None

RETURN VALUE
  Error code.

SIDE EFFECTS
  None

===========================================================================*/
uint16 ftm_wcdma_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                         uint8  num_freqs,
                                         uint8  num_steps_per_freq,
                                         uint16 step_length,
                                         uint16 *freq_list_ptr,
                                         uint16 *pdm_list_ptr,
                                         int16  *pri_exp_agc_val_ptr,
                                         uint8  *pa_range_list_ptr,
                                         uint8  *read_hdet_ptr,
                                         uint8  *rx_action_ptr,
                                         uint8  *rx_lpm_list_ptr)
{
  uint8 i, j;
  uint16 channels[RFCOM_MAX_CARRIERS];
  ftm_tx_rx_error_code_type result = FTM_TX_RX_SUCCESS;

  ftm_tx_rx_freq_cal_cur_seg = 0;

  /* lock interrupts as timer may be running */
  if ((rx_chain == 0) || (rx_chain == 1))
  {
    ftm_tx_rx_chain = rx_chain;
  }

  /* limit num_steps_per_freq to <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ */
  if (num_steps_per_freq <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ)
  {
    ftm_tx_rx_steps_per_freq = num_steps_per_freq;
  }
  else
  {
    ftm_tx_rx_steps_per_freq = FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ;
    result |= FTM_TX_RX_STEPS_INVALID; /*lint !e655*/
  }

  /* limit num_freqs to <= FTM_TX_RX_FREQ_CAL_MAX_FREQS */
  if (num_freqs <= FTM_TX_RX_FREQ_CAL_MAX_FREQS)
  {
    ftm_tx_rx_num_freqs = num_freqs;
  }
  else
  {
    ftm_tx_rx_num_freqs = FTM_TX_RX_FREQ_CAL_MAX_FREQS;
    result |= FTM_TX_RX_FREQS_INVALID;  /*lint !e655*/
  }

  /* limit step_length to >= FTM_TX_RX_MIN_STEP_LENGTH (20ms) */
  if (step_length < FTM_TX_RX_MIN_STEP_LENGTH)
  {
    step_length = FTM_TX_RX_MIN_STEP_LENGTH;
    result |= FTM_TX_RX_STEP_LEN_INVALID; /*lint !e655*/
  }

  /* copy input data to local arrays, as packet memory may be reassigned */
  /* before sweep completion */
  for (i = 0; i < num_freqs; i++)
  {
    ftm_tx_rx_freq_cal_channels[i] = freq_list_ptr[i];
    for (j = 0; j < ftm_tx_rx_steps_per_freq; j++)
    {
      ftm_tx_rx_freq_cal_tx_result[i * ftm_tx_rx_steps_per_freq + j] = 0;
    }
  }

  for (i = 0; i < ftm_tx_rx_steps_per_freq; i++)
  {
    ftm_tx_rx_freq_cal_pa_ranges[i] = pa_range_list_ptr[i];
    ftm_tx_rx_freq_cal_pdm_list[i] = (uint16)(pdm_list_ptr[i]);
    ftm_tx_rx_freq_cal_hdet_read_list[i] = read_hdet_ptr[i];
    ftm_tx_rx_freq_cal_rx_actions[i] = rx_action_ptr[i];
    ftm_tx_rx_freq_cal_exp_agc[i] = (int16)(pri_exp_agc_val_ptr[i]);
  }

  /* create trigger */
  if (rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0] == RFCOM_DUAL_CARRIER)
  {
    channels[0] = ftm_tx_rx_freq_cal_channels[0];
    channels[1] = channels[0] + FTM_WCDMA_DC_CHANNEL_OFFSET;
    /* last step of this frequency, tune to new channel */
    ftm_wcdma_tune_to_multi_chan(RFM_DEVICE_0, channels);
  }
  else if (rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0] == RFCOM_3_CARRIER)
  {
    channels[0] = ftm_tx_rx_freq_cal_channels[0];

    channels[1] = channels[0] + FTM_WCDMA_DC_CHANNEL_OFFSET;

    channels[2] = channels[1] + FTM_WCDMA_DC_CHANNEL_OFFSET;

    /* last step of this frequency, tune to new channel */
    ftm_wcdma_tune_to_multi_chan(RFM_DEVICE_0, channels);
  }
  else
  {
    ftm_wcdma_tune_to_chan(RFM_DEVICE_0, ftm_tx_rx_freq_cal_channels[0]);
  }
  /* Move the set PDM to the start to avoid any false trigger */
  ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, FTM_UPLINK_SWEEP_CAL_MIN);
  ftm_wcdma_set_tx(RFM_DEVICE_0, TRUE);
  ftm_wcdma_set_pa_range(ftm_tx_rx_freq_cal_pa_ranges[0]);

  DALSYS_BusyWait(100);

  /* SUITE sends the cal sweep time in sleep time ISR, hence need to convert into milli-sec*/
  ftm_common_msm_install_timer((uint32)(step_length / 32), ftm_wcdma_tx_rx_freq_cal_isr);

  return (uint16)result;
}

#ifdef FEATURE_WCDMA_RX_DIVERSITY
/*===========================================================================

FUNCTION FTM_SET_SECONDARY_CHAIN

DESCRIPTION
  This function is used during calibration to activate the secondary chain.
  Enable and disable secondary chain

DEPENDENCIES
  Should be called after ftm_wcdma_set_mode function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_secondary_chain
(
  rfcom_device_enum_type device, 
  boolean on_off
)
{
  int i;
  uint16 rx_channels[RFCOM_MAX_CARRIERS] = {0};
  rfwcdma_data_debug_flags_type *dbg_flags;
  rfm_wcdma_rx_sleep_params sleep_params;
  rfcom_wcdma_band_type band; 
#ifdef FEATURE_MDSP_IOCTL
  static uint16 ant1_samp_clk_src_div;
#endif
  ftm_concurrency_manager_params_type wcdma_cc_params;
  memset(&wcdma_cc_params, 0, sizeof(ftm_concurrency_manager_params_type));

  dbg_flags = rfm_wcdma_ftm_get_debug_flags();

  if ((device == RFM_DEVICE_1)||(device == RFM_DEVICE_3))
  {
    if ((on_off == TRUE) && (ftm_wcdma_div_status[device] == FALSE))
    {
      ftm_rfmode_enter(device, FTM_STATE_WCDMA);

      (void)rfm_enter_mode(device, RFCOM_WCDMA_MODE, NULL, NULL, ftm_wcdma_rxlm_buffer[device].buf_idx);

      if (device == RFM_DEVICE_1)
      {
        for (i = 0; i < RFCOM_MAX_CARRIERS; i++)
        {
          rx_channels[i] = rfwcdma_mc_state.car_path_state[0][i].curr_chan_rx;
        }
        band = rfwcdma_mc_state.car_path_state[0][0].curr_band_rx;
      }
      if (device == RFM_DEVICE_3)
      {
        rx_channels[0] = rfwcdma_mc_state.car_path_state[0][1].curr_chan_rx;
        band = rfwcdma_mc_state.car_path_state[0][1].curr_band_rx;
      }

       wcdma_cc_params.rfm_dev = device;
       wcdma_cc_params.band.wcdma_band = band;
       if (device ==RFM_DEVICE_3) 
       {
         wcdma_cc_params.carrier_index = 1;
       }
       wcdma_cc_params.prx_drx =1;
       wcdma_cc_params.tech = RFM_IMT_MODE; 


      if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
      {
        FTM_MSG_2( FTM_ERROR, "ftm_wcdma_set_secondary_chain: Invalid concurreny scenario - device:%d, band: %d,",
                          device,band );
        return;
      }

      if ((rfwcdma_mc_state.rx_multi_carrier_idx[device-1] == RFWCDMA_MC_SC)&&
          (device != RFM_DEVICE_3))
      {
        /* scripts based tune does not support dual carrier */
        ftm_wcdma_tune_to_chan_scripts((1<< device), rx_channels, 1); 
      }
      else
      {
        rfwcdma_mc_tune_to_multi_chan(device,
                                      rx_channels,
                                      rfwcdma_mc_state.rx_multi_carrier_idx[device-1],
                                      NULL,
                                      NULL,
                                      ftm_wcdma_rxlm_buffer[device].buf_idx);
      }

      ftm_wcdma_div_status[device] = TRUE;
    }
    else if ((on_off == FALSE) && (ftm_wcdma_div_status[device] == TRUE))
    {
      if ( dbg_flags->enable_opt_wakeup == TRUE )
      {
        memset(&sleep_params, 0, sizeof(rfm_wcdma_rx_sleep_params)); 
        sleep_params.api_params[device].is_valid = TRUE;
        sleep_params.api_params[device].rxlm_handle = ftm_wcdma_rxlm_buffer[device].buf_idx;
        sleep_params.api_params[device].skip_agc_disable = FALSE;
        rfm_wcdma_rx_sleep(&sleep_params, NULL, NULL);
      } /* if ( dbg_flags->enable_opt_wakeup == TRUE ) */
      else
      {
        (void)rfm_sleep(device, NULL, NULL,
                        ftm_wcdma_rxlm_buffer[device].buf_idx);
      } /* if ! ( dbg_flags->enable_opt_wakeup == TRUE ) */

      /* Cleanup concurrency manager database for this Diversity when we disable Div */
      ftm_concurrency_manager_cleanup_device_allocation(device,RFM_IMT_MODE);

      ftm_wcdma_div_status[device] = FALSE;
    }
  }/* if ((device == RFM_DEVICE_1)||(device == RFM_DEVICE_3)) */
  else
  {
    FTM_MSG_ERROR("ftm_wcdma_set_secondary_chain: "
                  "wrong device:%d passed in", device, 0, 0);
  }
}/* ftm_wcdma_set_secondary_chain */

/*===========================================================================

FUNCTION FTM_SECOND_CHAIN_TEST_CALL

DESCRIPTION
  This function is used in non-signalling calls to test a particular antenna.
  Enable will set the RxD controller NV to 6 to use ANT1 for primary reception
  Disable will set the RxD controller NV to 0 to use ANT0 for primary reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_second_chain_test_call(uint8 second_chain)
{
  switch (second_chain)
  {
  case FTM_PRIMARY_CHAIN_ONLY:
    rxd_overide_nv_settings(0);
    FTM_MSG_HIGH("RxD nv value set to 0 for primary only", 0, 0, 0);
    break;

  case FTM_SECONDARY_CHAIN_ONLY:
    rxd_overide_nv_settings(6);
    FTM_MSG_HIGH("RxD nv value set to 6 to activate secondary chain", 0, 0, 0);
    break;

  case FTM_PRIMARY_AND_SECONDARY:
    rxd_overide_nv_settings(5);
    FTM_MSG_HIGH("RxD nv value set to 5 to activate primary and secondary chain", 0, 0, 0);
    break;

  default:
    FTM_MSG_ERROR("Wrong parameter", 0, 0, 0);
    break;
  }
}
#endif /* FTM_HAS_WCDMA_RX_DIVERSITY */


/*===========================================================================

FUNCTION FTM_DO_DC_CAL

DESCRIPTION
  This function performs wcdma DC calibration.  The calibrated value will be
  used during WCDMA inter-freq/inter-band.

DEPENDENCIES
  Made sure WCDMA mode is set and the channel is tuned.
  NV task must be running.

RETURN VALUE
  None

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
void ftm_do_dc_cal(void)
{
  uint8  i;
  uint8  cnt;
  uint16 dc_cal_i_table[6] = { 0, 0, 0, 0, 0, 0 };
  uint16 dc_cal_q_table[6] = { 0, 0, 0, 0, 0, 0 };
  nv_items_enum_type rfi_iaccum_item_code = NV_WCDMA_IACCUM_I;
  nv_items_enum_type rfi_qaccum_item_code = NV_WCDMA_QACCUM_I;

  if (ftm_curr_wcdma_mode == RFCOM_BAND_IMT)
  {
    rfi_iaccum_item_code = NV_WCDMA_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_1900)
  {
    rfi_iaccum_item_code = NV_WCDMA_1900_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_1900_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_BC3)
  {
    rfi_iaccum_item_code = NV_WCDMA_1800_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_1800_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_BC4)
  {
    rfi_iaccum_item_code = NV_WCDMA_BC4_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_BC4_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_800)
  {
    rfi_iaccum_item_code = NV_WCDMA_800_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_800_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_BC8)
  {
    rfi_iaccum_item_code = NV_WCDMA_900_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_900_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_BC9)
  {
    rfi_iaccum_item_code = NV_WCDMA_1800_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_1800_QACCUM_I;
  }
  else if (ftm_curr_wcdma_mode == RFCOM_BAND_BC19)
  {
    rfi_iaccum_item_code = NV_WCDMA_800_IACCUM_I;
    rfi_qaccum_item_code = NV_WCDMA_800_QACCUM_I;
  }
  //cnt = FTM_NUMBER_OF_INTERBAND_GAIN_STATE_USED * FTM_NUMBER_OF_DC_CAL_LOOP;  /* 2 runs */
  cnt = 2;
  i = 0;
  while (i < cnt)
  {
    /* loop through all the LNA gain */
    ftm_wcdma_set_lna_range(RFM_DEVICE_0, i % FTM_NUMBER_OF_INTERBAND_GAIN_STATE_USED);
    ftm_clk_rex_wait(3);
    i++;
  }

  /* Get the calibrated DC values */
#ifdef RF_HAS_DM_SUPPORT
  rflib_msm_get_dc(&dc_cal_i_table[0], &dc_cal_q_table[0]);
#endif
  FTM_MSG_HIGH("ACCUM: I  and   Q", 0, 0, 0);
  for (i = 0; i < FTM_NUMBER_OF_INTERBAND_GAIN_STATE_USED; i++)
  {
    FTM_MSG_HIGH("%08x        %08x", dc_cal_i_table[i], dc_cal_q_table[i], 0);
  }

  /* Write to NV */
  rfcommon_nv_set_item(rfi_iaccum_item_code,
                       (nv_item_type *)dc_cal_i_table,
                       rex_self(),
                       FTM_NV_CMD_SIG,
                       ftm_sig_rex_wait_cb);

  rfcommon_nv_set_item(rfi_qaccum_item_code,
                       (nv_item_type *)dc_cal_q_table,
                       rex_self(),
                       FTM_NV_CMD_SIG,
                       ftm_sig_rex_wait_cb);

  return;
}

/*===========================================================================

FUNCTION FTM_WCDMA_CONFIGURE_XO_CAL_STATE

DESCRIPTION
  This function enables/configures Modem & Device for XO Cal.

DEPENDENCIES
  Make sure
    - WCDMA mode is set
    - Channel is tuned
    - LNA State is set

RETURN VALUE
  None

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
void ftm_wcdma_enable_xo_cal_state(ftm_receive_chain_handle_type chain,
                                   uint32 xo_cal_buff_idx)
{
  uint16 channels[3];
  rfcom_wcdma_band_type band =0; 
  ftm_concurrency_manager_params_type wcdma_cc_params;
  memset(&wcdma_cc_params, 0, sizeof(ftm_concurrency_manager_params_type));

  channels[0] = rfwcdma_mc_state.car_path_state[chain][0].curr_chan_rx;
  channels[1] = rfwcdma_mc_state.car_path_state[chain][1].curr_chan_rx;
  channels[2] = 0;

  band = rfwcdma_core_convert_ul_chan_to_band(channels[0]);

  wcdma_cc_params.rfm_dev = (rfm_device_enum_type)chain;
  wcdma_cc_params.band.wcdma_band = band;
  wcdma_cc_params.prx_drx = 0;
  wcdma_cc_params.carrier_index =0;
  wcdma_cc_params.tech = RFM_IMT_MODE;

  /* Set frequency adjust to 0*/
  rfwcdma_core_tx_set_freq_adjust((rflm_dm_handle_id_t)xo_cal_buff_idx,
                                  rfwcdma_mc_state.car_path_state[chain][0].curr_band_rx,
                                  channels[0],
                                  0);

  /* The new XO Cal buffer shall be updated when retuning to the channel*/
 
  if(!(ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
  {
    FTM_MSG_2( FTM_ERROR, "ftm_wcdma_enable_xo_cal_state: Invalid concurreny scenario - device:%d, band: %d,",
                          (rfm_device_enum_type)chain,band );
    return;
  }
 
  ftm_wcdma_tune_to_chan_scripts((1<<chain),channels, 1);
}

/*===========================================================================

FUNCTION FTM_WCDMA_TOGGLE_MODES

DESCRIPTION
  This function will switch from WCDMA to GSM mode and back, or vice-versa.
  This triggers the updating of rf driver internal variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if 0

void ftm_wcdma_toggle_modes(void)
{
  ftm_mode_id_type current_mode;

  current_mode = ftm_db_to_rf_mode(ftm_current_rf_mode);

  if ((ftm_current_rf_mode == FTM_DB_RF_GSM_850) ||
      (ftm_current_rf_mode == FTM_DB_RF_GSM_900) ||
      (ftm_current_rf_mode == FTM_DB_RF_GSM_1800) ||
      (ftm_current_rf_mode == FTM_DB_RF_GSM_1900))
  {
    /* we are in GSM, so set to WCDMA and then back */
    ftm_wcdma_set_mode(FTM_PHONE_MODE_WCDMA_IMT);
#ifdef FTM_HAS_GSM
    ftm_gsm_set_mode(RFM_DEVICE_0, current_mode);
#endif /* FTM_HAS_GSM */
  }
  else
  {
    /* we are in WCDMA, so set to GSM and then back */
#ifdef FTM_HAS_GSM
    ftm_gsm_set_mode(RFM_DEVICE_0, FTM_PHONE_MODE_GSM_900);
#endif /* FTM_HAS_GSM */
    ftm_wcdma_set_mode(current_mode);
  }
}
#endif
/*===========================================================================

FUNCTION FTM_WCDMA_TPC_CONTROL

DESCRIPTION
    This function will force all up or all down bits for the slots in the tpc_pattern
    parameter.

    Override flag is (tpcOrideFlag)
    Boolean, 0=no override TPC, 1= override TPC
    It is a 32-bit register (tpcOrideDecHist)
    Format is:
         Bit address
    Bits 0:1 = slot 0
    Bits 2:3 = slot 1
    etc
    Bits 28:29 = slot 14
         Format of data value is
    0=none, 1=up, 2 or 3 = down
         Write
    Down 0x3FFFFFFF
    Up 0x15555555

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_wcdma_tpc_control(boolean enable, uint32 tpc_pattern)
{
  if (enable)
  {
    rf_wfw_intf_dpcch_write_ptr->tpcOrideFlag    = 0x0001;
    rf_wfw_intf_dpcch_write_ptr->tpcOrideDecHist = tpc_pattern;
  }
  else
  {
    rf_wfw_intf_dpcch_write_ptr->tpcOrideFlag    = 0x0000;
    rf_wfw_intf_dpcch_write_ptr->tpcOrideDecHist = (uint32)0x0;
  }
}
/*===========================================================================

FUNCTION FTM_WCDMA_ALLOCATE_RXLM

DESCRIPTION
    This function will request rxlm buffers FTM WCDMA for the given chain

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_wcdma_allocate_rxlm(ftm_receive_chain_handle_type chain)
{
  lm_tech_type tech = LM_UMTS;
  lm_status_type status;

  if (!ftm_wcdma_rxlm_buffer[chain].is_buf_allocated)
  {
    status = rxlm_allocate_buffer((rxlm_chain_type)chain, tech,
                                  &(ftm_wcdma_rxlm_buffer[chain].buf_idx));
    if (status != LM_SUCCESS)
    {
      MSG_ERROR("RxLM allocate buffer failed, status:%d", status, 0, 0);
      return;
    }
    else
    {
      MSG_HIGH("RXLM buffer allocated for chain:%d, buffer index:%d",
               chain, ftm_wcdma_rxlm_buffer[chain].buf_idx, 0);
    }
    ftm_wcdma_rxlm_buffer[chain].is_buf_allocated = TRUE;
  }
}

/*===========================================================================

FUNCTION FTM_WCDMA_DEALLOCATE_RXLM

DESCRIPTION
    This function will deallocate rxlm buffers FTM WCDMA has requested
    for a given chain

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_wcdma_deallocate_rxlm(ftm_receive_chain_handle_type chain)
{
  lm_status_type status;

  if (ftm_wcdma_rxlm_buffer[chain].is_buf_allocated)
  {
    status = rxlm_deallocate_buffer(ftm_wcdma_rxlm_buffer[chain].buf_idx);
    if (status != LM_SUCCESS)
    {
      MSG_ERROR("RxLM allocate buffer failed, status:%d", status, 0, 0);
    }
    else
    {
      MSG_HIGH("RXLM buffer deallocated for chain:%d", chain, 0, 0);
      ftm_wcdma_rxlm_buffer[chain].is_buf_allocated = FALSE;
    }
  }
}

/*===========================================================================

FUNCTION FTM_WCDMA_ALLOCATE_TXLM

DESCRIPTION
    This function will request txlm buffers FTM WCDMA

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_wcdma_allocate_txlm(void)
{
  lm_tech_type tech = LM_UMTS;
  lm_status_type status;

  if (!ftm_wcdma_txlm_buffer.is_buf_allocated)
  {
    status = txlm_allocate_buffer(TXLM_CHAIN_0, tech,
                                  &(ftm_wcdma_txlm_buffer.buf_idx));
    if (status != LM_SUCCESS)
    {
      MSG_ERROR("TxLM allocate buffer failed, status:%d", status, 0, 0);
      return;
    }
    else
    {
      MSG_HIGH("TXLM buffer allocated buffer index:%d", ftm_wcdma_txlm_buffer.buf_idx, 0, 0);
    }

    ftm_wcdma_txlm_buffer.is_buf_allocated = TRUE;
  }
}

/*===========================================================================

FUNCTION FTM_WCDMA_DEALLOCATE_TXLM

DESCRIPTION
    This function will deallocate txlm buffers FTM WCDMA has requested

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_wcdma_deallocate_txlm(void)
{
  lm_status_type status;
  if (ftm_wcdma_txlm_buffer.is_buf_allocated)
  {
    status = txlm_deallocate_buffer(ftm_wcdma_txlm_buffer.buf_idx);
    if (status != LM_SUCCESS)
    {
      MSG_ERROR("TxLM allocate buffer failed, status:%d", status, 0, 0);
    }
    else
    {
      MSG_HIGH("TXLM buffer deallocated for buffer index %d",
               ftm_wcdma_txlm_buffer.buf_idx, 0, 0);
      ftm_wcdma_txlm_buffer.is_buf_allocated = FALSE;
    }
  }
}

/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET_SETUP

DESCRIPTION
  This functions sets up Rx for measuring DVGA gain offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_get_dvga_gain_offset_setup(rfm_device_enum_type device)
{
  /* Set LNA to high gain */
  ftm_wcdma_set_lna_range(device, 0);

  if (ftm_wcdma_rxcal_opt_enabled())
  {
    if (!IS_FTM_IN_WCAL_MODE())
    {
      /* Set DVGA gain offset to zero */
      ftm_wcdma_set_dvga_gain_offset(device,  0);
    }
  }
  else
  {
    ftm_wcdma_set_dvga_gain_offset(device,  0);
  }
} /* ftm_wcdma_get_dvga_gain_offset_setup */

/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET_MEAS

DESCRIPTION
  This function returns the WCDMA Rx DVGA gain offset.
  1. Injecting Rx power at expected_agc_val (convert it to dBm).
  2. ftm_wcdma_get_dvga_gain_offset_meas() will automatically 
     calculate the DVGA gain offset.

DEPENDENCIES
  ftm_wcdma_get_dvga_gain_offset_setup must have been called

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_dvga_gain_offset_meas(rfm_device_enum_type device,
                                         word expected_agc_val)
{
  int2 num_of_agc_reads;
  int2 dvga_offset = 0;
  int2 current_agc_val = 0;
  int2 agc_read_val = 0;

  for (num_of_agc_reads = 0;
       num_of_agc_reads < FTM_NUM_OF_AVG_RX_AGC_READ;
       num_of_agc_reads++)
  {
    agc_read_val += rfm_get_rx_raw_agc_val(device);
    DALSYS_BusyWait(100);
  }

  current_agc_val = agc_read_val / FTM_NUM_OF_AVG_RX_AGC_READ;

  dvga_offset = (current_agc_val - ((int2)expected_agc_val)- rfwcdma_msm_get_dvga_offset(device));

  /* Range checking */
  if (dvga_offset > FTM_DVGA_GAIN_OFFSET_MAX)
  {
    dvga_offset = FTM_DVGA_GAIN_OFFSET_MAX;
  }
  else if (dvga_offset < FTM_DVGA_GAIN_OFFSET_MIN)
  {
    dvga_offset = FTM_DVGA_GAIN_OFFSET_MIN;
  }

  ftm_wcdma_set_dvga_gain_offset(device, dvga_offset);

  return (dvga_offset);
} /* ftm_wcdma_get_dvga_gain_offset_meas */


/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET_MEAS

DESCRIPTION
  This function returns the WCDMA Rx DVGA gain offset.
  1. Injecting Rx power at expected_agc_val (convert it to dBm).
  2. ftm_wcdma_get_dvga_gain_offset_meas() will automatically 
     calculate the DVGA gain offset.

DEPENDENCIES
  ftm_wcdma_get_dvga_gain_offset_setup must have been called

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_dvga_gain_offset_meas_nbpwr(rfm_device_enum_type device,
                                               word expected_agc_val)
{
  int2 num_of_agc_reads;
  int32 dvga_offset = 0;
  uint32 current_agc_val = 0;
  uint32 agc_read_val = 0;
  int32 zero_db_gain = rfwcdma_msm_get_zero_db_digital_gain_10dBm();

  for (num_of_agc_reads = 0;
       num_of_agc_reads < FTM_NUM_OF_AVG_RX_AGC_READ;
       num_of_agc_reads++)
  {
    agc_read_val += rfwcdma_msm_read_nbpwr(device,  ftm_wcdma_rxlm_buffer[device].buf_idx);
    DALSYS_BusyWait(100);
  }

  current_agc_val = agc_read_val / FTM_NUM_OF_AVG_RX_AGC_READ;


  dvga_offset = (rfwcdma_msm_convert_raw_nbee_to_10dBm(current_agc_val)
                      - zero_db_gain - ((int2)expected_agc_val)- rfwcdma_msm_get_dvga_offset(device));

  /* Range checking */
  if (dvga_offset > FTM_DVGA_GAIN_OFFSET_MAX)
  {
    dvga_offset = FTM_DVGA_GAIN_OFFSET_MAX;
  }
  else if (dvga_offset < FTM_DVGA_GAIN_OFFSET_MIN)
  {
    dvga_offset = FTM_DVGA_GAIN_OFFSET_MIN;
  }

  ftm_dvga_offset_nbpwr = dvga_offset;
  return (dvga_offset);
} /* ftm_wcdma_get_dvga_gain_offset_meas */

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET_SETUP

DESCRIPTION
  Set up Rx for measuring LNA offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_get_lna_offset_setup(rfm_device_enum_type device, byte index)
{
  if (index > 0)
  {
    /* If the index is less than the max number of LNA range, set the LNA Range to the current index */
    if (index <= (byte)FTM_LNA_RANGE_5)
    {
      /* Set the LNA range to the current index*/
      ftm_wcdma_set_lna_range(device, index);

      if (ftm_wcdma_rxcal_opt_enabled())
      {
        if (!IS_FTM_IN_WCAL_MODE())
        {
          /* Reset the indexed LNA Offset to zero */
          ftm_wcdma_set_lna_offset(device, index, 0);
        }
      }
      else
      {
        /* Reset the indexed LNA Offset to zero */
        ftm_wcdma_set_lna_offset(device, index, 0);
      }
    }
  }
} /* ftm_wcdma_get_lna_offset_setup */

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET_MEAS

DESCRIPTION
  Get Rx for LNA offset

DEPENDENCIES
  ftm_wcdma_get_lna_offset_setup must have been called

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_lna_offset_meas(rfm_device_enum_type device,
                                   byte index, word expected_agc_val)
{
  int32 lna_offset = 0;
  int32 current_agc_val = 0;
  int32 agc_read_val = 0;
  int32 num_of_agc_reads;

  MSG_ERROR("ftm_wcdma_get_lna_offset_meas , lna index :%d", index, 0, 0);
  if (index > 0)
  {
    /* Reset agc_read_val to zero */
    agc_read_val = 0;

    for (num_of_agc_reads = 0; num_of_agc_reads < FTM_NUM_OF_AVG_RX_AGC_READ; num_of_agc_reads++)
    {
      agc_read_val += rfwcdma_mdsp_get_raw_rx_agc(device);
      DALSYS_BusyWait(100);
    }
    current_agc_val = agc_read_val / FTM_NUM_OF_AVG_RX_AGC_READ;
    lna_offset = (((int32)expected_agc_val) - current_agc_val + rfwcdma_msm_get_dvga_offset(device) );
    ftm_wcdma_set_lna_offset(device, index, lna_offset);
  }

  return ((int2)lna_offset);
} /* ftm_wcdma_get_lna_offset_meas */


/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET_MEAS_NBPWR

DESCRIPTION
  Get Rx for LNA offset using narrow band power estimator

DEPENDENCIES
  ftm_wcdma_get_lna_offset_setup must have been called

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/

int2 ftm_wcdma_get_lna_offset_meas_nbpwr(rfm_device_enum_type device,
                                         byte index, word expected_agc_val)
{
  int32 lna_offset = 0;
  int32 current_agc_val = 0;
  int32 agc_read_val = 0;
  int32 num_of_agc_reads;
  int32 zero_db_gain = rfwcdma_msm_get_zero_db_digital_gain_10dBm();

  if (index > 0)
  {
    /* Reset agc_read_val to zero */
    agc_read_val = 0;

    for (num_of_agc_reads = 0; num_of_agc_reads < FTM_NUM_OF_AVG_RX_AGC_READ; num_of_agc_reads++)
    {
      agc_read_val += rfwcdma_msm_read_nbpwr(device,  ftm_wcdma_rxlm_buffer[device].buf_idx);
      DALSYS_BusyWait(100);
    }
    current_agc_val = agc_read_val / FTM_NUM_OF_AVG_RX_AGC_READ;
    lna_offset = (((int32)expected_agc_val) + ftm_dvga_offset_nbpwr + zero_db_gain - rfwcdma_msm_convert_raw_nbee_to_10dBm(current_agc_val)+ rfwcdma_msm_get_dvga_offset(device));
  }

  return ((int2)lna_offset);
} /* ftm_wcdma_get_lna_offset_meas */


#if 0
/*===========================================================================

FUNCTION FTM_WCDMA_SET_NBPWR_SETTING

 DESCRIPTION
  setup narrow band power estimator

DEPENDENCIES
     

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_nbpwr_setting(rfcom_device_enum_type device, ftm_nbpwr_config_type * nbpwr_config )
{

  rfwcdma_msm_nbpwr_setting_type rfwcdma_nbpwr;

  MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_wcdma_set_nbpwr_setting: ,  nbpwr_config->command  = %d", nbpwr_config->command  );
  MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_wcdma_set_nbpwr_setting: ,  nbpwr_config->config  = %d", nbpwr_config->config  );
  MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_wcdma_set_nbpwr_setting: ,  nbpwr_config->start_action_sample  = %d", nbpwr_config->start_action_sample   );
  MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_wcdma_set_nbpwr_setting: ,  nbpwr_config->stop_action_sample  = %d", nbpwr_config->stop_action_sample  );
  MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_wcdma_set_nbpwr_setting: ,  nbpwr_config->start_mask  = %d", nbpwr_config->start_mask  );


  rfwcdma_nbpwr.command =  nbpwr_config->command;
  rfwcdma_nbpwr.config =   nbpwr_config->config;
  rfwcdma_nbpwr.start_action_sample =  nbpwr_config->start_action_sample;
  rfwcdma_nbpwr.stop_action_sample = nbpwr_config->stop_action_sample;
  rfwcdma_nbpwr.start_mask = nbpwr_config->start_mask;

  rfwcdma_msm_setup_nbpwr(device, ftm_wcdma_rxlm_buffer[device].buf_idx ,  &rfwcdma_nbpwr );

} /* ftm_wcdma_set_nbpwr_setting */



#endif

/*===========================================================================

FUNCTION FTM_WCDMA_DO_TX_DAC_CAL

DESCRIPTION
    This function shall write the DAC config registers based on the current
    Iref for which DAC cal is being performed.

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
boolean ftm_wcdma_dac_cal_update_txlm(rfdevice_dac_iref_val iref_val)
{
  boolean result = FALSE;

  /*WCDMA can control TxLM chain-0 only*/
  txlm_chain_type txlm_chain = TXLM_CHAIN_0;

  /* Dynamic updates for DAC Config based on the current
     Iref in use*/
  if (ftm_wcdma_txlm_buffer.is_buf_allocated)
  {
    result = rfcommon_msm_dac_cfg_dynamic_update(txlm_chain,
                                                 ftm_wcdma_txlm_buffer.buf_idx,
                                                 iref_val);
  }

  return result;
}

/*===========================================================================

FUNCTION FTM_WCDMA_DO_TX_DAC_CAL

DESCRIPTION
    This function will complete the TX DAC cal on WCDMA.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS


===========================================================================*/
ftm_common_dac_cal_rsp_type
ftm_wcdma_do_tx_dac_cal
(
  rfm_device_enum_type device
  )
{
  boolean result = TRUE;
  const rfc_common_logical_device_params_type *cmn_logical_dev = NULL;
  rfdevice_dac_iref_val iref_val;
  uint8 iref_count = 0;
  boolean ftm_exit_ret_val = FALSE;

  /*!@note
    - We do the DAC cal on WCDMA default band B1 and Ch9750
    - Trigger MDSP to start DAC Cal after the complete Tx Turn ON 
    - WCDMA supports only TXLM_CHAIN_0 which is available on RFM_DEVICE_0 */

  if (device != RFM_DEVICE_0)
  {
    FTM_MSG_HIGH("Skipping W DAC cal on Device %d as WCDMA supports Tx only on RFM_DEVICE_0 ", device, 0, 0);

    /* Returning as some other Tech may be capable of calibrating current logical path */
    return FTM_COMMON_DAC_CAL_NOT_ATTEMPTED;
  }

  FTM_MSG_HIGH("DAC cal is being attempted for device path %d in WCDMA mode ", device, 0, 0);

  /*Hard coding the logical device to RFM_DEVICE_0 as device information is not passed
    for WCDMA functions and WCDMA is always supported on RFM_DEVICE_0 only for now*/
  cmn_logical_dev = rfc_common_get_logical_device_params(RFM_DEVICE_0);

  if (cmn_logical_dev == NULL)
  {
    FTM_MSG_ERROR("Common device not initialized for DAC Cal", 0, 0, 0);
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  /* Enter WCDMA mode on B1 */
  ftm_wcdma_set_mode(device, FTM_PHONE_MODE_WCDMA_IMT);

  /* Tune UL Ch9750 */
  ftm_wcdma_tune_to_chan(RFM_DEVICE_0, 9750);

  /* Turn Tx ON for the DAC calibration.
     Note: It does not matter what PA range and the PDM we configure by default
     part of the Tx ON sequence */
  ftm_wcdma_set_tx(device, ON);

  /*Set RGI of 40, Note: it does not matter what value we set!*/
  ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, 40);

  /* Need to wait 2ms to complete all the routines in ftm_wcdma_set_tx(ON) to be completed   
    by MDSP. It was found that when the delay is removed, Tx ON signal is not received from 
    FW until about 1ms hence waiting 2ms as a safe side. */
  DALSYS_BusyWait(2000);

  /* Perform DAC cal on all the supported Irefs of the device */
  for (iref_val = RFDEVICE_DAC_CAL_IREF_MIN;
       (iref_val <= RFDEVICE_DAC_CAL_IREF_MAX) && result;
       iref_val++)
  {
    /* Clear the DAC Cal results status flag in MDSP */
    rfcommon_mdsp_clear_dac_cal_results();

    FTM_MSG_HIGH("--- DAC Cal starting for device path:%d with Iref#: %d ---", device, iref_val, 0);

    /* Do IREF_5 cal using IREF_6 current */
    /* as per DAC designers' recommendation */
    if (iref_val == RFDEVICE_IREF_5)
    {

      /* Dynamic updates for DAC Config based on the current
      Iref in use*/
      ftm_wcdma_dac_cal_update_txlm(RFDEVICE_IREF_6);

      /* Program the Iref of interest */
      result = rfdevice_cmn_set_dac_iref(cmn_logical_dev->cmn_device,
                                         RFDEVICE_IREF_6,
                                         TRUE,
                                         NULL);
    }
    else
    {
      ftm_wcdma_dac_cal_update_txlm(iref_val);

      result = rfdevice_cmn_set_dac_iref(cmn_logical_dev->cmn_device,
                                         iref_val,
                                         TRUE,
                                         NULL);
    }

    /* Wait 50us for Iref to settle */
    DALSYS_BusyWait(50);

    if (result)
    {
      /* Trigger MDSP for starting DAC Cal */
      result = result & ftm_send_tx_dac_cal_cmd(TRUE, FALSE);
    }
    else
    {
      FTM_MSG_ERROR("Something went wrong while setting device Iref: %d", iref_val, 0, 0);
      break;
    }

    /* Wait for the DAC Cal results to be available from MDSP, then
       extract the results */
    if (result)
    {
      result = result & rfcommon_mdsp_extract_dac_cal_results(TXLM_CHAIN_0, iref_val, iref_count);
    }
    else
    {
      FTM_MSG_ERROR("DAC Cal failed as MDSP Send sync command for device Iref: %d", iref_val, 0, 0);
      break;
    }

    if (result == FALSE)
    {
      FTM_MSG_ERROR("DAC Cal failed as results could not extracted from MDSP memory for device with Iref#: %d", iref_val, 0, 0);
      break;
    }

    iref_count++;
  } /* End of for loop which does DAC Cal on all Irefs*/

  /* DAC Cal done, Turn Tx OFF */
  ftm_wcdma_set_tx(device, OFF);

  /* DAC Cal done, Exit WCDMA mode */
  ftm_exit_ret_val = ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_UNKNOWN);

  if (ftm_exit_ret_val == FALSE)
  {
    FTM_MSG_ERROR("ftm_wcdma_do_tx_dac_cal: FN ftm_rfmode_exit()"
                  " failed for RFM_DEVICE_0", 0, 0, 0);
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

  if (result)
  {
    FTM_MSG_HIGH("DAC Cal successfully completed using WCDMA B1, Ch 9750, and for all the required Irefs", 0, 0, 0);
    return FTM_COMMON_DAC_CAL_SUCCESS;
  }
  else
  {
    FTM_MSG_ERROR("DAC Cal failed in WCDMA mode", 0, 0, 0);
    return FTM_COMMON_DAC_CAL_FAILURE;
  }

} /* ftm_wcdma_do_tx_dac_cal */

/*============================================================================

FUNCTION FTM_WCDMA_SET_TX_POWER_DBM

DESCRIPTION
  This command programs the PA range, LUT index, and Digital Gain to reach the programmed
  Tx power level with power limiting enabled

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS

=============================================================================*/
boolean ftm_wcdma_set_tx_power_dbm(uint8 enable, int16 power_level)
{
  if (enable != 0)
  {
    //TxAGC = tpcAccum + Dpcchinitialpower + Rateadjust
    //Where Rateadjust=100*log10(sum(beta^2)/ txBetaCGain^2)=100*(log10(sum(beta^2))-log10(txBetaCGain^2));
    Word16 Dpcchinitialpower = rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99[0].txDipPlusCmDelta;
    Word16 Rateadjust = (Word16)(100 * (log10(rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain * rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain
                                              + rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaDGain * rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaDGain)
                                        - log10(rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain * rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain)));

    rf_wfw_intf_txagc_async_write_ptr->txAgcSsbiDisabled = 0;
    rf_wfw_intf_txagc_async_write_ptr->txGainAdjOverride[0] = 1;
    rf_wfw_intf_txagc_async_write_ptr->txFreezeAccum[0] = 0;
    
    rfm_set_tx_pwr_limit(RFCOM_TRANSCEIVER_0, RF_UTRAN_MAX_TX_POWER_FTM_RF_MODE, RFCOM_TXPLIM_DCH);
    /* Get the desired limit tx power and set mdsp after calling Set Tx Power limit */
    rf_wfw_intf_txagc_async_write_ptr->txGainUpperLimit =
    rfwcdma_core_util_convert_dbm10_to_txagc_unit(rfwcdma_core_txplim_get_desired_limit_dbm10());
    rf_wfw_intf_txagc_async_write_ptr->txGainAdjWr[0] = (power_level + 700) - 512 - Dpcchinitialpower - Rateadjust;
  }

  ftm_wcdma_tx_power_dbm = power_level;

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH, "Enable = %d : Power_Level = %d", enable, power_level);

  return TRUE;

}

/*============================================================================

FUNCTION FTM_WCDMA_GET_RX_LEVEL_DBM

DESCRIPTION
  Function to get Rx Power Level. Returns the Rx level, for a specified receiver, in
  both Rx AGC and dBm10 units.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS
=============================================================================*/
boolean ftm_wcdma_get_rx_level_dbm(rfm_device_enum_type device, int16 *rx_level_agc, int16 *rx_level_dbm10)
{
  /* Call RFM Function to query LNA Data */
  *rx_level_agc = rfm_get_rx_raw_agc_val(device);
  *rx_level_dbm10 = (RF_MIN_RX_POWER_DBM * RF_TXAGC_RESOLUTION  + (*rx_level_agc - RFAGC_MIN_AGC_VAL));
  FTM_MSG_HIGH("Rx AGC = %d : Rx Power dbm10 = %d", *rx_level_agc, *rx_level_dbm10, 0);

  /* Return Response Packet */
  return TRUE;
}

/*============================================================================

FUNCTION FTM_WCDMA_GET_RX_LEVEL_DBM_X2W_MEAS

DESCRIPTION
  Function to get Rx Power Level in X2W measurement. Returns the Rx level, for a specified receiver, in
  both Rx AGC and dBm10 units.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS
=============================================================================*/
boolean ftm_wcdma_get_rx_level_dbm_xtow_meas(rfm_device_enum_type device, uint16 neigh_channel, int16 *rx_level_agc, int16 *rx_level_dbm10, boolean bypass_rxagcon)
{
  /* Extract band information from DL channel info */
  rfcom_wcdma_band_type neigh_band = rfwcdma_core_convert_chan_to_band(neigh_channel);
  
  /* Call RFM Function to query RxAGC data*/
  *rx_level_agc = rfm_get_rx_raw_agc_val_xtow_meas(device, neigh_band, bypass_rxagcon);
  *rx_level_dbm10 = (RF_MIN_RX_POWER_DBM * RF_TXAGC_RESOLUTION  + (*rx_level_agc - RFAGC_MIN_AGC_VAL));
  FTM_MSG_HIGH("Rx AGC = %d : Rx Power dbm10 = %d", *rx_level_agc, *rx_level_dbm10, 0);

  /* Return Response Packet */
  return TRUE;
}

/*============================================================================

FUNCTION FTM_WCDMA_SET_PA_STATE

DESCRIPTION
  This command programs the PA state. PA State is the low level hardware control indicator, which is
  different from the PA range software abstraction

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS

=============================================================================*/
boolean ftm_wcdma_set_pa_state(uint8 pa_state)
{
  boolean status = FALSE;
  uint8 pa_range;

  if (pa_state < 8)
  {
    /* Returns the PA range map from RF NV */
    pa_range = rfwcdma_core_util_get_pa_range_from_nv(pa_state);

    /* This function sets PA range */
    ftm_wcdma_set_pa_range(pa_range);

    status = TRUE;

    FTM_MSG_LOW("ftm_wcdma_set_pa_state: PA State = %d : PA Range = %d", pa_state, pa_range, 0);
  }
  else
  {
    FTM_MSG_ERROR("ftm_wcdma_set_pa_state: Invalid PA State = %d", pa_state, 0, 0);
  }

  return status;

}

/*============================================================================

FUNCTION FTM_WCDMA_NOTIFY_RF_TASK

DESCRIPTION
  This API is used for notifying rf_task that ftm_wcdma_enter/exit had completed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_notify_rf_task(void)
{
  (void)rex_set_sigs(&rf_tcb, FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG);
}

/*============================================================================

FUNCTION FTM_WCDMA_DO_FREQ_ERROR_COMP

DESCRIPTION
  This API is used for applying a Tx frequency error compensation value.
  Frequency error is in Hz. The amount of frequency correction applied is either
  the user_freq_adjust_ppb or the combination of user_freq_adjust_ppb + the
  frequency error estimate from XO manager. This selection is based on the input
  value of enable_xo_freq_adjust.

DEPENDENCIES
  None

RETURN VALUE
 int32 - Total Frequency adjust in Hz

SIDE EFFECTS

=============================================================================*/
int32  ftm_wcdma_set_freq_adjust(int32 user_freq_adjust_ppb,
                                 uint32 center_frequency_hz,
                                 uint8 enable_xo_freq_adjust)
{
  int32 tx_freq_error_composite_hz = 0;
  int32 freq_error_xo_ppb = 0;
  int32 freq_error_xo = 0;

  MSG_HIGH("ftm_wcdma_set_freq_adjust: tx_freq_error_ppb : %d : centre_freq_hz : %d : Enable = %d",
           user_freq_adjust_ppb,
           center_frequency_hz,
           enable_xo_freq_adjust);

  if (enable_xo_freq_adjust)
  {
    freq_error_xo = (int32)tcxomgr_ft_curr();

    //Converts the frequency error estimated by XO Manager API to ppb units.
    freq_error_xo_ppb = (int32)((int64)((int64)((int64)(freq_error_xo << 16) * 1000) / 1024) >> 16);

    MSG_HIGH("ftm_wcdma_set_freq_adjust: freq_error_xo_ppb : %d", freq_error_xo_ppb, 0, 0);

    if (!user_freq_adjust_ppb)
    {
      // The formula below does:
      // 1) Converts the frequency error estimated by XO Manager API to Hz from ppb as expected by
      // rfwcdma_mdsp_set_freq_adjust() API
      // tx_freq_error_composite_hz = (-1)* (freq_error_xo_ppb)*2^16 * center_frequency_hz) / 10^6 / 2^16

      tx_freq_error_composite_hz = (-1) * ((int32)((int64)((
                                                            (int64)((freq_error_xo_ppb << 16)) *
                                                            (int64)((center_frequency_hz))
                                                            ) / 1000000000) >> 16));

      MSG_HIGH("ftm_wcdma_set_freq_adjust: tx_freq_error_composite_hz : %d", tx_freq_error_composite_hz, 0, 0);
    }
    else
    {
      // The formula below does:
      // 1) Adds the user input frequency error with frequency error estimated by XO Manager.
      // 2) Converts the result in 1) from ppb units to Hz as expected by rfwcdma_mdsp_set_freq_adjust() API
      // tx_freq_error_composite_hz = (-1)* ((user_freq_adjust_ppb  + freq_error_xo_ppb)*2^16 * center_frequency_hz) / 10^6 / 2^16

      tx_freq_error_composite_hz = (-1) * (int32)((int64)((
                                                           (int64)(((int64)((user_freq_adjust_ppb  + freq_error_xo_ppb) << 16)) * center_frequency_hz)
                                                           ) / 1000000000) >> 16);

      MSG_HIGH("ftm_wcdma_set_freq_adjust: tx_freq_error_composite_hz : %d", tx_freq_error_composite_hz, 0, 0);
    }
  }
  else
  {
    tx_freq_error_composite_hz = user_freq_adjust_ppb;
    MSG_HIGH("ftm_wcdma_set_freq_adjust: tx_freq_error_composite_hz : %d", tx_freq_error_composite_hz, 0, 0);
  }

  //Apply calculated frequency adjustment to WCDMA
  rfwcdma_core_tx_set_freq_adjust(
    rfwcdma_mc_state.rxlm_buf_idx[RFM_DEVICE_0],
    rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_rx,
    rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_chan_rx,
    tx_freq_error_composite_hz
    );

  return tx_freq_error_composite_hz;
}

/*============================================================================

FUNCTION FTM_WCDMA_SET_TX_IQ_GAIN

DESCRIPTION
  This API is used for setting TX IQ gain value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_set_iqgain_value(int16 iqGain)
{
  rfwcdma_mdsp_set_iqgain_value(iqGain);
}

/*============================================================================

FUNCTION FTM_WCDMA_GET_TX_IQ_GAIN

DESCRIPTION
  This API is used for getting TX IQ gain value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
int16 ftm_wcdma_get_iqgain_value(void)
{
  return rfwcdma_mdsp_get_iqgain_value();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This command configure/deconfigure feedback path for IQ capture

  @param device
  Primary device for the Transmit path for IQ capture
 
  @param device_fb
  Feedback device for the IQ capture
 
  @param xpt_mode
  XPT mode to configure in; currently needed to differentiate setup for FBRX_MODE

  @param config
  TRUE is to config; FALSE is to deconfig 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
ftm_wcdma_xpt_cal_config
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  uint8 xpt_mode,
  boolean config
  )
{
  boolean ret_val = TRUE;
  lm_tech_type tech = LM_UMTS;
  lm_status_type status;
  const rfc_common_logical_device_params_type *cmn_logical_dev = NULL;
  rfdevice_rxtx_common_class *device_ptr = rfdevice_wcdma_tx_get_common_dev_ptr(RFM_DEVICE_0, tx_band.wcdma_band);
#ifdef FEATURE_RF_HAS_QTUNER
  /* temporary fix to set the PDET coupler */
  uint32 tx_freq = 0;
#endif
  rfdevice_coupler_tech_cfg_type coupler_tech_cfg;

  if (device_ptr == NULL)
  {
    MSG_ERROR("RFC rxtx common device ptr is Null", 0, 0, 0);
    return (FALSE);
  }

  /* xPT Cal config */
  if (config)
  {
    switch ((ftm_xpt_config_type)xpt_mode)
    {
    case FTM_XPT_CONFIG_FBRX: // start config FBRX_MODE

      ftm_wcdma_xpt_fb_path_buffer.buf_idx = rfcommon_fbrx_mc_get_tech_handle( RFM_IMT_MODE );
      if(ftm_wcdma_xpt_fb_path_buffer.buf_idx != -1)
      {
        ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated = TRUE;
      }
      else
      {
        MSG_ERROR("xPT feedback path buffer allocate failed, status:%d", 0, 0, 0);
        return FALSE;

      }
      break; // end config for FBRX_MODE
    default:
      /* if secondary chain is using, turn it off first */
      if (ftm_wcdma_rxlm_buffer[RF_PATH_1].is_buf_allocated &&
          ((rf_path_enum_type)device_fb == RF_PATH_1))
      {
        ftm_wcdma_set_secondary_chain(RFM_DEVICE_1,FALSE);
      }

      if (!ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated)
      {
        /* if the xpt fb path not config, go ahead allocate RxLM buffer
           and config the feedback path */
        status = rxlm_allocate_buffer((rxlm_chain_type)device_fb, tech,
                                      &(ftm_wcdma_xpt_fb_path_buffer.buf_idx));
        if (status != LM_SUCCESS)
        {
          MSG_ERROR("xPT feedback path buffer allocate failed, status:%d", status, 0, 0);
          return FALSE;
        }
        else
        {
          MSG_HIGH("xPT feedback path buffer allocated for chain:%d, buffer index:%d",
                   device_fb, ftm_wcdma_xpt_fb_path_buffer.buf_idx, 0);
        }
        ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated = TRUE;

        /* Optional: Configure coupler to feedback path */
        if(NULL != rfc_wcdma_get_device( device, 
                                         RFC_CONFIG_TX, 
                                         ftm_curr_wcdma_mode, 
                                         RFDEVICE_COUPLER ))
        {
          coupler_tech_cfg.mode = RFM_IMT_MODE;
          coupler_tech_cfg.band.wcdma_band = ftm_curr_wcdma_mode;
          coupler_tech_cfg.port = 0;
          coupler_tech_cfg.direction = RFDEVICE_COUPLER_DIRECTION_FWD;
          rfdevice_wcdma_coupler_config(device, 
                                        coupler_tech_cfg, 
                                        NULL,
                                        RFDEVICE_EXECUTE_IMMEDIATE,
                                        RFCOM_INVALID_TIMING_OFFSET);
        }
      #ifdef FEATURE_RF_HAS_QTUNER 
        else if(NULL != rfc_wcdma_get_device( device, 
                                            RFC_CONFIG_TX, 
                                            ftm_curr_wcdma_mode, 
                                            RFDEVICE_TUNER_MANAGER))
        {
          /* temporary fix to set the PDET coupler */
          /* Program QFE for Tx mode */
          rfwcdma_core_antenna_tuner_program_tx_mode 
                                  (device, 
                                   ftm_curr_wcdma_mode,
                                   rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
                                   RFDEVICE_EXECUTE_IMMEDIATE, 
                                   NULL,
                                   NULL);
        }
      #endif /* FEATURE_RF_HAS_QTUNER */
        else
        {
          FTM_MSG( FTM_ERROR, "xPT Cal Config: Coupler and Tuner devices are NULL");
        }

        /* Step1: Configure RxLm feedback path */
        /* Acquire rxlm handle lock before updating settings */
        rxlm_acquire_lock(ftm_wcdma_xpt_fb_path_buffer.buf_idx);
        ret_val &= rfwcdma_msm_set_et_state_rxlm(ftm_wcdma_xpt_fb_path_buffer.buf_idx);
        /* modem chain update necessary for DIME RxLM settings */
        ret_val &= rfwcdma_msm_update_modem_chain_rxlm_xpt(ftm_wcdma_xpt_fb_path_buffer.buf_idx,
                                                           (rfm_device_enum_type)device_fb,
                                                           ftm_curr_wcdma_mode);
 
        /* Step2: Enable WTR Tx PLL for feedback */
        rfc_wcdma_setup_fb_device(device,
                                  device_fb,
                                  ftm_curr_wcdma_mode,
                                  rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
        ret_val &= rfcommon_fbrx_mc_enable_rf_dev(device_ptr, 2);

        /* Step3: Set WTR ET Path */
        rfc_wcdma_setup_fb_path(device_fb,
                                ftm_curr_wcdma_mode,
                                TRUE);

        /* Release rxlm handle lock */
        rxlm_release_lock(ftm_wcdma_xpt_fb_path_buffer.buf_idx);

        /* Vote modem bus clk*/
        rfwcdma_msm_xpt_configure_clock(device);

        /* Optional: Configure wcdma waveform type */
    #ifdef FEATURE_JOLOKIA_MODEM
        ftm_configure_eul_waveform_by_subtest(7);
        #endif

        FTM_MSG_1(FTM_MED, "xPT Cal Config status %d.", ret_val);
      } /* if (!ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated) */
    }
  } /* if (config) */
  else /* xPT Cal Deconfig */
  {
    switch ((ftm_xpt_config_type)xpt_mode)
    {
    case FTM_XPT_CONFIG_FBRX: // start deconfig FTM_XPT_CONFIG_FBRX
      ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated = FALSE; 
      break; // end deconfig for FTM_XPT_CONFIG_FBRX
    default:
      if (ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated)
      {
        status = rxlm_deallocate_buffer(ftm_wcdma_xpt_fb_path_buffer.buf_idx);
        if (status != LM_SUCCESS)
        {
          MSG_ERROR("xPT feedback path buffer deallocate failed, status:%d", status, 0, 0);
        }
        else
        {
          MSG_HIGH("xPT feedback path buffer deallocated for chain:%d", device, 0, 0);
          ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated = FALSE;
        }
      } /* if (ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated) */

      /* Turn off WTR ET Path */
      rfc_wcdma_setup_fb_path(device_fb,
                              ftm_curr_wcdma_mode,
                              FALSE);

      /* Get pointer to the common logical device */
      cmn_logical_dev = rfc_common_get_logical_device_params(RFM_DEVICE_0);
      if (cmn_logical_dev == NULL)
      {
        FTM_MSG(FTM_ERROR, "ftm_wcdma_xpt_cal_config: cannot get logical device");
      }
      else
      {
        rfdevice_cmn_set_et_path(cmn_logical_dev->cmn_device, RFM_IMT_MODE, RFM_FB_ATTENUATION_STATE_LOW, FALSE);
      }

      ret_val &= rfcommon_fbrx_mc_disable_rf_dev(device_ptr);

      #ifdef FEATURE_RF_HAS_QTUNER
      /* temporary fix to set the PDET coupler */
      tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
      /*HDET enable*/
      rfdevice_hdet_wcdma_init(RFM_DEVICE_0,
                               NULL,
                               RFDEVICE_EXECUTE_IMMEDIATE,
                               ftm_curr_wcdma_mode,
                               tx_freq);
      #endif /* FEATURE_RF_HAS_QTUNER */

      /* Unvote modem bus clk*/
      rfwcdma_msm_xpt_deconfigure_clock(device);
    }
    
    #ifdef FEATURE_JOLOKIA_MODEM
    ftm_disable_eul_waveform();
    #endif
    
    FTM_MSG_1(FTM_MED, "xPT Cal Deconfig status %d.", ret_val);
  } /* xPT Cal Deconfig */

  return ret_val;
} /* ftm_wcdma_xpt_cal_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Trigger Single instance of IQ Capture

  @details
  This function will call RF-MDSP function to trigger the IQ Capture
  for XPT for W.
 
  @param device
  Primary device for the Transmit path for IQ capture
 
  @param device_fb
  Feedback device for the IQ capture
 
  @param xpt_capture_type
  Indicate what mode the capture is for e.g. EPT/ET
 
  @param num_samples
  Number of TXC/RX samples to capture.Must be a multiple of 1024(1K)
 
  @param first_trig
  Flag to do initialization.This must be set in 1st trigger
 
  @param last_trig
  Flag to do de-initialization.This must be set in last trigger 
 
  @param do_processing
  Flag to indicate if firmware should do DPD estimation on the captured samples.

  @param do_leakage_cal
  Controls whether to perform leakage cal
 
  @param buffer_id
  Buffer index to store captured data and processed data
 
  @param buffer_id
  Pointer return to hold sampling rate of Rx IQ capture

  @return
  TRUE is set successfully; FALSE otherwise

*/
boolean ftm_wcdma_xpt_iq_capture
(
  uint8 device,
  uint8 device_fb,
  uint8 xpt_capture_type,
  uint16 num_samples,
  boolean first_trig,
  boolean last_trig,
  rfcommon_xpt_processing_type do_processing,
  rf_common_xpt_buffer_id_type buffer_id,
  uint32 *samp_rate
  )
{
  boolean ret_val = TRUE;
  uint8 tx_modem_chain, rx_modem_chain;
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type cfg;
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  uint16 split_samp = 0; // No split sample capture
  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode));

  if (nv_tbl == NULL)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_xpt_iq_capture: Failed to get valid static "
              "NV pointer for band %d", ftm_curr_wcdma_mode);
    return FALSE;
  }

  /*Initialize Pin IQ capture tyoe to INVALID*/
  iq_cap_data.pin_capture_type = RF_COMMON_XPT_PIN_CAP_INVALID;

  cfg.alternate_path = 0;
  cfg.band = ftm_curr_wcdma_mode;
  cfg.req = RFC_REQ_ENABLE_XPT_CAPTURE;
  cfg.logical_device = device_fb;
  cfg.rx_tx = RFC_CONFIG_TX;
  if (rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
  {
    rx_modem_chain = device_info_ptr->modem_chain;
  }
  else
  {
    FTM_MSG_2(FTM_ERROR, "Cannot get device config %d, band %d", device, ftm_curr_wcdma_mode);
    return FALSE;
  }

  tx_modem_chain = rfcommon_core_get_tx_chain_from_device(device);
  FTM_MSG_2(FTM_MED, "ftm_wcdma_xpt_iq_capture rx_modem_chain %d tx_modem_chain %d.",
            rx_modem_chain, tx_modem_chain);

  FTM_MSG_3(FTM_MED, "wcdma xpt: fb_path buf_id %d, capt buf_id %d, res buf_id:%d",
            ftm_wcdma_xpt_fb_path_buffer.buf_idx,buffer_id.capt_buf_id, buffer_id.res_buf_id);

  if (first_trig == TRUE)
  {
    /* Set Capture params in FW SMEM */
    ret_val &= rfcommon_mdsp_setup_capture_params(
      tx_modem_chain,          /* tx modem chain */
      rx_modem_chain,          /* rx modem chain */
      (uint8)ftm_wcdma_xpt_fb_path_buffer.buf_idx,  /* xPT feedback path id */
      xpt_capture_type,                              /* FW capture type */
      (boolean)(0x1 & (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_RX_COMPACT]),
      num_samples,                                  /* number of samples */
      split_samp,
      do_processing);
    /* Send DPD processing Params to FW*/
    if (do_processing > PROC_NONE)
    {
      if ( ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_SELF_TEST_EVM) ||
           ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR) )
      {
        ret_val &= rfcommon_mdsp_setup_dpd_proc_params(
          (int16)FTM_WCDMA_SELFTEST_SEARCH_CENTER,
          (uint8)FTM_WCDMA_SELFTEST_SEARCH_WIDTH,
          (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_FIRST_BIN],
          (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_EXTRAP_BIN],
          (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_MIN_BIN_COUNT],
          (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_MIN_FIRST_BIN_COUNT],
          (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_MIN_LAST_BIN_COUNT],
          (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_SPEC_INV],
          do_processing);
      } else
      {
      ret_val &= rfcommon_mdsp_setup_dpd_proc_params(
        (int16)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_SEARCH_CENTER],
        (uint8)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_SEARCH_WIDTH],
        (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_FIRST_BIN],
        (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_EXTRAP_BIN],
        (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_MIN_BIN_COUNT],
        (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_MIN_FIRST_BIN_COUNT],
        (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_MIN_LAST_BIN_COUNT],
        (uint8)nv_tbl->tx_ept_dpd_config_params[DPD_CFG_SPEC_INV],
        do_processing);
    }
  }
  }
  if (samp_rate != NULL)
  {
    *samp_rate = rfwcdma_mc_get_samp_rate((uint8)ftm_wcdma_xpt_fb_path_buffer.buf_idx);
    iq_cap_data.samp_rate = *samp_rate;
  }
  else
  {
    iq_cap_data.samp_rate = 10000000;
    FTM_MSG(FTM_MED, "ftm_wcdma_xpt_iq_capture: Samp Rate not queried, ET delay value returned in 1/1000th of samples");
  }

  iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_INVALID;
  /* Update ET delay table only if capture type was delay */
  if ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_DELAY)
  {
    iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_NONLTE;
    FTM_MSG_4(FTM_MED, "ftm_wcdma_xpt_iq_capture: Push to delay map - Chan: %d, delayCapType: %d, CaptBuf: %d, ResBuf: %d",
              rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
              iq_cap_data.delay_capture_type,
              buffer_id.capt_buf_id,
              buffer_id.res_buf_id);
    ret_val &= ftm_cal_data_push_delay_data_to_delay_map(
      rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
      iq_cap_data.delay_capture_type,
      buffer_id.capt_buf_id,
      buffer_id.res_buf_id);

  }

  /*Update Pin IQ capture data into Pin map structure*/
  if ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_PIN)
  {
    /*Set Pin IQ capture type*/
    iq_cap_data.pin_capture_type = RF_COMMON_XPT_PIN_CAP_ALL_TECHS;
    ret_val &= ftm_cal_data_push_pin_data_to_pin_map(
                       rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
                       buffer_id.capt_buf_id,
                       buffer_id.res_buf_id);
    FTM_MSG_4(FTM_MED,"ftm_wcdma_xpt_iq_capture: Push to pin map - Chan: %d, pinCapType: %d, CaptBuf: %d, ResBuf: %d",
                                        rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
                                        iq_cap_data.pin_capture_type,
                     buffer_id.capt_buf_id,
                     buffer_id.res_buf_id);

  }

  /* Store the IQ Caprure Data */
  iq_cap_data.device = device;
  iq_cap_data.device_fb = device_fb;
  iq_cap_data.channel = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx;
  iq_cap_data.is_last_trig = last_trig;
  iq_cap_data.buffer_id = buffer_id;
  iq_cap_data.capture_type = xpt_capture_type;
  ret_val &= rfcommon_xpt_store_iq_capture_data(&iq_cap_data);

  /* Perform xPT Capture */
  rfwcdma_mdsp_send_sapt_iq_cap_cmd((uint32)first_trig,      /* first trigger */
                                    (uint32)last_trig,       /* last trigger */
                                    buffer_id.capt_buf_id,
                                    buffer_id.res_buf_id);
  FTM_MSG_1(FTM_MED, "ftm_wcdma_xpt_iq_capture status %d.", ret_val);

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override TXAGC parameters 

  @details
  This function will override TXAGC params.

  @param device
  The device to perform override on. 
 
  @param txagc_oride_parms
  The parameter values to override.
 
  @return
  Return if override was successful or not. 
*/
boolean
ftm_wcdma_xpt_override_txagc
(
  rfm_device_enum_type device,
  uint8  xpt_mode,
  uint8  amam_bank_idx,
  uint16 amam_start_offset,
  uint8  amam_tbl_size,
  uint8  ampm_bank_idx,
  uint16 ampm_start_offset,
  uint8  ampm_tbl_size,
  uint8  epdt_bank_idx,
  uint16 epdt_start_offset,
  uint8  epdt_tbl_size,
  uint8  iq_gain_action_type,
  uint32 iq_gain,
  uint8  env_scale_action_type,
  uint32 env_scale,
  uint16 rgi,
  uint16 smps_bias,
  uint8  pa_state,
  uint16 pa_curr,
  uint8  delay_action_type,
  int32  delay
  )
{
  boolean status = TRUE;
  //common override parameters
  rfcommon_mdsp_txagc_oride_params_type mdsp_ovr_params;
  rfdevice_lut_idx_pa_range_type lut_info;
  /* QPOET variables. */
  rfdevice_papm_mode_type qpoet_mode;
  uint8 qpoet_xpt_mode;
  uint8 pa_range;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type cfg;
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  int16 delay_comp_x10ns = 0;
  uint16 therm_scaled_value = 0;
  uint16 therm_raw_value = 0;
  rfdevice_tx_lut_data_type lut_data;
  rfcom_band_type_u tech_band;

  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(
    rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode));

  memset(&mdsp_ovr_params,0,sizeof(rfcommon_mdsp_txagc_oride_params_type));
  
  if (nv_tbl == NULL)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_xpt_override_txagc: Failed to get valid "
              "static NV pointer for band %d", ftm_curr_wcdma_mode);
    return FALSE;
  }

  /* Check for valid PA state */
  if (pa_state != FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID)
  {
    pa_range = rfwcdma_core_util_get_pa_range_from_nv(pa_state);
    /* Set PA Range */
    rfdevice_wcdma_pa_set_gain_range(device,
                                     ftm_curr_wcdma_mode,
                                     pa_range,
                                     NULL,
                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                     RFCOM_INVALID_TIMING_OFFSET);

    /* Turn on the PA */
    rfdevice_wcdma_pa_on_off(device,
                             ftm_curr_wcdma_mode,
                             TRUE,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET);
    cur_pa_range = pa_range;
  }

  /* Check for RGI validity */
  if (rgi != FTM_COMMON_XPT_TX_OVR_RGI_INVALID)
  {
    /* Get the device pointer for the 1st WTR */
    rfdevice_rxtx_common_class *device_ptr = rfdevice_wcdma_tx_get_common_dev_ptr(RFM_DEVICE_0, 
                                             ftm_curr_wcdma_mode);

    if (NULL != device_ptr)
    {
      if (rfdevice_cmn_is_cmn_tx_intf_supported(device_ptr) == FALSE)
      {
        /* Program RGI */
        lut_info.band = ftm_curr_wcdma_mode;
        lut_info.lut_idx = (uint8)rgi;
        lut_info.pa_range = cur_pa_range;

        /* Set Tx AGC */
        rfdevice_wcdma_tx_cmd_dispatch(device,
                                       ftm_curr_wcdma_mode,
                                       RFDEVICE_SET_LUT_INDEX,
                                       (uint8 *)&lut_info);
      }
      else
      {
        tech_band.wcdma_band = ftm_curr_wcdma_mode;
        rfdevice_cmn_get_tx_lut_scripts(
          device_ptr, RFM_IMT_MODE,
          tech_band,
          pa_state, rgi,
          RFDEVICE_EXECUTE_IMMEDIATE,
          &lut_data);
      }
      cur_tx_agc_adj_pdm = rgi;
    }
    else
    {
      RF_MSG(RF_ERROR,  "ftm_wcdma_xpt_override_txagc(): "
             "device_ptr is NULL. Unable to get Tx lut table");
    }
  }

  /* Check for PA current validity */
  if (pa_curr != FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID)
  {
    lut_info.band = ftm_curr_wcdma_mode;
    lut_info.lut_idx = cur_tx_agc_adj_pdm;
    lut_info.pa_range = cur_pa_range;
    /* Program PA current_bias*/
    status &= rfdevice_wcdma_pa_set_current_bias
      (device,
       ftm_curr_wcdma_mode,
       lut_info.pa_range,
       pa_curr,                       //in pdm - value ranges from 0 to 256
       NULL,
       RFDEVICE_EXECUTE_IMMEDIATE,
       RFCOM_INVALID_TIMING_OFFSET);
  }

  cfg.alternate_path = 0;
  cfg.band = ftm_curr_wcdma_mode;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  if (rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
  {
    mdsp_ovr_params.modem_chain = device_info_ptr->modem_chain;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Cannot get device config %d, band %d",
          device, ftm_curr_wcdma_mode);
    return FALSE;
  }

  /* Populate the MSM override data */
  mdsp_ovr_params.amam_addr.mem_bank_id = amam_bank_idx;
  mdsp_ovr_params.amam_addr.start_offset = amam_start_offset;
  mdsp_ovr_params.amam_addr.tbl_size = amam_tbl_size;
  mdsp_ovr_params.ampm_addr.mem_bank_id = ampm_bank_idx;
  mdsp_ovr_params.ampm_addr.start_offset = ampm_start_offset;
  mdsp_ovr_params.ampm_addr.tbl_size = ampm_tbl_size;
  mdsp_ovr_params.detrough_addr.mem_bank_id = epdt_bank_idx;
  mdsp_ovr_params.detrough_addr.start_offset = epdt_start_offset;
  mdsp_ovr_params.detrough_addr.tbl_size = epdt_tbl_size;
  mdsp_ovr_params.xpt_mode = xpt_mode;
  mdsp_ovr_params.latch_immediate = FALSE;
  mdsp_ovr_params.max_sample_count = 0;
  mdsp_ovr_params.rflm_handle = ftm_wcdma_txlm_buffer.buf_idx;

  /* Decide on IQ gain override type */
  switch (iq_gain_action_type)
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    mdsp_ovr_params.iq_gain = iq_gain;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
    mdsp_ovr_params.iq_gain = rfwcdma_msm_get_default_txc_gain(
      ftm_wcdma_txlm_buffer.buf_idx);
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
    {
      if (nv_tbl != NULL)
      {
        mdsp_ovr_params.iq_gain = (int32)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_IQ_GAIN_UNITY];
      }
      else
      {
        mdsp_ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
      }
      break;
    }

  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    mdsp_ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
    break;
  }

  /* Decide on Env Scale override type */
  switch (env_scale_action_type)
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    mdsp_ovr_params.env_scale = env_scale;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
    {
      if (nv_tbl != NULL)
      {
        if (xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT)
        {
          mdsp_ovr_params.env_scale =
            (int32)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
        }
        else if (xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET)
        {
          mdsp_ovr_params.env_scale =
            (int32)nv_tbl->tx_ept_dpd_config_params[ET_CFG_CMN_ENV_SCALE_UNITY];
        }
      }
      else
      {
        mdsp_ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
      }
      break;
    }

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    mdsp_ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
    break;
  }

  /* Decide on delay value override type */
  switch (delay_action_type)
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    mdsp_ovr_params.delay = delay;
    mdsp_ovr_params.delay_txlm_handle = ftm_wcdma_txlm_buffer.buf_idx;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_W_RC:
    mdsp_ovr_params.delay = delay;
    mdsp_ovr_params.delay_txlm_handle = ftm_wcdma_txlm_buffer.buf_idx;

    rfdevice_wcdma_tx_get_et_delay_compensation(device,
                                                ftm_curr_wcdma_mode,
                                                &delay_comp_x10ns);

    mdsp_ovr_params.delay += delay_comp_x10ns;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
    {
      if (nv_tbl != NULL)
      {
        mdsp_ovr_params.delay = rfwcdma_core_xpt_path_delay_comp(
          device,
          ftm_curr_wcdma_mode,
          rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
          FALSE,
          0,
          rfwcdma_mc_state.tx_carriers);

        mdsp_ovr_params.delay_txlm_handle = ftm_wcdma_txlm_buffer.buf_idx;
      }
      else
      {
        mdsp_ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
        mdsp_ovr_params.delay_txlm_handle = 0;
      }
      break;
    }

  case FTM_COMMON_XPT_TX_OVERRIDE_NV_WITH_TEMP_COMP:
    {
      /* Read current Therm Read from transceiver */
      rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                     rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
                                     RFDEVICE_GET_THERMISTER_VALUE,
                                     &therm_raw_value);

      therm_scaled_value = rfwcdma_core_temp_comp_get_scaled_therm_read(therm_raw_value);

      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
            "ftm_wcdma_xpt_override_txagc: Current Scaled Temp Read: %d",
            therm_scaled_value);

      mdsp_ovr_params.delay = rfwcdma_core_xpt_path_delay_comp(
        device,
        ftm_curr_wcdma_mode,
        rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
        TRUE,
        therm_scaled_value,
        rfwcdma_mc_state.tx_carriers);

      mdsp_ovr_params.delay_txlm_handle = ftm_wcdma_txlm_buffer.buf_idx;
      break;
    }

  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    mdsp_ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
    mdsp_ovr_params.delay_txlm_handle = 0;
    break;
  }

  /* Program QPOET */
  /* Get Power tracker object from RFC */
  qpoet_xpt_mode = (uint8)xpt_mode;
  switch (qpoet_xpt_mode)
  {
  case RFCOMMON_MDSP_XPT_MODE_APT:
  case RFCOMMON_MDSP_XPT_MODE_EPT:
    qpoet_mode = RFDEVICE_APT_MODE;
    break;
  case RFCOMMON_MDSP_XPT_MODE_ET:
    qpoet_mode = RFDEVICE_ET_MODE;
    break;
  default:
    qpoet_mode = RFDEVICE_NO_PT_MODE;
    break;
  }

  if (qpoet_mode == RFDEVICE_ET_MODE)
  {

    /* Call the common MC interface to override MSM settings */
    status &= rfcommon_mc_xpt_txagc_override(device, &mdsp_ovr_params);

    DALSYS_BusyWait(10); /* wait for 10 us before configuring QPOET */

    rfdevice_wcdma_papm_set_mode_bias(device,
                                      ftm_curr_wcdma_mode,
                                      qpoet_mode,
                                      0,  /* bias in mV*/
                                      NULL,
                                      RFDEVICE_EXECUTE_IMMEDIATE);

  }
  else
  {
    if (qpoet_mode == RFDEVICE_APT_MODE &&
        smps_bias != FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID)
    {
      ftm_wcdma_set_smps_pa_bias_override(TRUE);
      /*Keep track of the overriden value for future use*/
      ftm_wcdma_smps_pa_bias_override_val = smps_bias;

      rfdevice_wcdma_papm_set_mode_bias(device,
                                        ftm_curr_wcdma_mode,
                                        qpoet_mode,
                                        smps_bias,  /* bias in mV*/
                                        NULL,
                                        RFDEVICE_EXECUTE_IMMEDIATE);
    }

    DALSYS_BusyWait(10); /* wait for 10 us before configuring QPOET */
    /* Call the common MC interface to override MSM settings */
    status &= rfcommon_mc_xpt_txagc_override(device, &mdsp_ovr_params);
  }

  // Debug msg
  MSG_7(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "ftm_wcdma_xpt_override_txagc: XPT Mode %d, IQ Gain Action %d, "
        "IQ Gain %d, Env Scale Action %d, Env Scale %d, Delay Action %d, Delay %d",
        xpt_mode,
        iq_gain_action_type,
        mdsp_ovr_params.iq_gain,
        env_scale_action_type,
        mdsp_ovr_params.env_scale,
        delay_action_type,
        mdsp_ovr_params.delay);

  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "ftm_wcdma_xpt_override_txagc: RGI %d, SMPS %d, PA State %d, PA Curr %d",
        rgi,
        smps_bias,
        pa_state,
        pa_curr);

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "set commom xpt txagc override, status = %d",
        status);

  return status;
} /* ftm_wcdma_xpt_override_txagc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set current TxAGC override parameters 

  @details
  This function will set current TxAGC override params, without actually sending 
  out the RF overrides or calling the baseband override function.

  @param device
  The device to perform override on. 
 
  @param txagc_oride_parms
  The parameter values to override.
 
  @return
  Return if override was successful or not. 
*/
boolean
ftm_wcdma_xpt_set_txagc_override_params
(
  rfm_device_enum_type device,
  uint8  xpt_mode,
  uint8  amam_bank_idx,
  uint16 amam_start_offset,
  uint8  amam_tbl_size,
  uint8  ampm_bank_idx,
  uint16 ampm_start_offset,
  uint8  ampm_tbl_size,
  uint8  epdt_bank_idx,
  uint16 epdt_start_offset,
  uint8  epdt_tbl_size,
  uint8  iq_gain_action_type,
  uint32 iq_gain,
  uint8  env_scale_action_type,
  uint32 env_scale,
  uint16 rgi,
  uint16 smps_bias,
  uint8  pa_state,
  uint16 pa_curr,
  uint8  delay_action_type,
  int32  delay
  )
{
  boolean status = TRUE;

  /* rfc variables */
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type rfc_cfg;
  uint32 final_modem_chain;
  /* Delay compensation variables */
  int16 delay_comp_x10ns = 0;
  uint16 therm_scaled_value = 0;
  uint16 therm_raw_value = 0;
  /* Common override parameters */
  rfcommon_core_xpt_tx_override_type curr_tx_ovr;
  uint32 final_iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
  uint32 final_env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
  int32 final_delay;

  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;

  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(
    rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode));
  if (nv_tbl == NULL)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_xpt_set_txagc_override_params: Failed"
              " to get valid static NV pointer for band %d",
              ftm_curr_wcdma_mode);
    return FALSE;
  }

  /* Query modem chain info from rfc */
  rfc_cfg.alternate_path = 0;
  rfc_cfg.band = ftm_curr_wcdma_mode;
  rfc_cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_cfg.logical_device = device;
  rfc_cfg.rx_tx = RFC_CONFIG_TX;
  if (rfc_wcdma_data_get_device_info(&rfc_cfg, &device_info_ptr) == TRUE)
  {
    final_modem_chain = device_info_ptr->modem_chain;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Cannot get device config %d,"
          " band %d", device, ftm_curr_wcdma_mode);
    return FALSE;
  }

  /* Decide on IQ gain override type */
  switch (iq_gain_action_type)
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    final_iq_gain = iq_gain;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
    /* This API is being stubbed out */
    final_iq_gain = rfwcdma_msm_get_default_txc_gain(
      ftm_wcdma_txlm_buffer.buf_idx);
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
    final_iq_gain =
      (int32)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_IQ_GAIN_UNITY];
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    final_iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
    break;
  }

  /* Decide on Env Scale override type */
  switch (env_scale_action_type)
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    final_env_scale = env_scale;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
    {
      if (xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT)
      {
        final_env_scale =
          (int32)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      }
      else if (xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET)
      {
        final_env_scale =
          (int32)nv_tbl->tx_ept_dpd_config_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      }
      break;
    }

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    final_env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
    break;
  }

  /* Decide on delay value override type */
  switch (delay_action_type)
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    final_delay = delay;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_W_RC:
    final_delay = delay;
    /* This API is not supported on wtr3625 */
    rfdevice_wcdma_tx_get_et_delay_compensation(device,
                                                ftm_curr_wcdma_mode,
                                                &delay_comp_x10ns);

    final_delay += delay_comp_x10ns;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
    final_delay = rfwcdma_core_xpt_path_delay_comp(
      device,
      ftm_curr_wcdma_mode,
      rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
      FALSE,
      0,
      rfwcdma_mc_state.tx_carriers);
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV_WITH_TEMP_COMP:
    /* Read current Therm Read from transceiver */
    rfdevice_wcdma_tx_cmd_dispatch(
      RFM_DEVICE_0,
      rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
      RFDEVICE_GET_THERMISTER_VALUE,
      &therm_raw_value);

    therm_scaled_value =
      rfwcdma_core_temp_comp_get_scaled_therm_read(therm_raw_value);

    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,
          "ftm_wcdma_xpt_set_txagc_override_params: "
          "Current Scaled Temp Read: %d",
          therm_scaled_value);

    final_delay = rfwcdma_core_xpt_path_delay_comp(
      device,
      ftm_curr_wcdma_mode,
      rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
      TRUE,
      therm_scaled_value,
      rfwcdma_mc_state.tx_carriers);
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    final_delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
    break;
  }

  /* Update the current set of overrides to FTM common data */
  curr_tx_ovr.rflm_handle = ftm_wcdma_txlm_buffer.buf_idx;
  curr_tx_ovr.modem_chain = final_modem_chain;
  curr_tx_ovr.xpt_mode = xpt_mode;
  curr_tx_ovr.iq_gain = final_iq_gain;
  curr_tx_ovr.iq_gain_action_type = iq_gain_action_type;
  curr_tx_ovr.env_scale = final_env_scale;
  curr_tx_ovr.env_scale_action_type = env_scale_action_type;
  curr_tx_ovr.et_delay = final_delay;
  curr_tx_ovr.delay_action_type = delay_action_type;
  curr_tx_ovr.amam_addr.mem_bank_id = amam_bank_idx;
  curr_tx_ovr.amam_addr.start_offset = amam_start_offset;
  curr_tx_ovr.amam_addr.tbl_size = amam_tbl_size;
  curr_tx_ovr.ampm_addr.mem_bank_id = ampm_bank_idx;
  curr_tx_ovr.ampm_addr.start_offset = ampm_start_offset;
  curr_tx_ovr.ampm_addr.tbl_size = ampm_tbl_size;
  curr_tx_ovr.detrough_addr.mem_bank_id = epdt_bank_idx;
  curr_tx_ovr.detrough_addr.start_offset = epdt_start_offset;
  curr_tx_ovr.detrough_addr.tbl_size = epdt_tbl_size;
  curr_tx_ovr.latch_immediate = FALSE;
  curr_tx_ovr.max_sample_count = 0;
  curr_tx_ovr.rgi = rgi;
  curr_tx_ovr.papm_bias = smps_bias;
  curr_tx_ovr.pa_state = pa_state;
  curr_tx_ovr.pa_range = rfwcdma_core_util_get_pa_range_from_nv(pa_state);
  curr_tx_ovr.pa_curr = pa_curr;

  status &= ftm_common_xpt_set_curr_tx_override(device,
                                                &curr_tx_ovr);

  if (!status)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,
        "ftm_wcdma_xpt_set_txagc_override_params failed");
  }

  return status;
} /* ftm_wcdma_xpt_set_txagc_override_params */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override XPT txagc in RF

  @details
  This function calls the RFM FTM interface to perform a Tx override from RF
 
  @param ovr_params
  Override parameters obtained from FTM dispatch
 
  @return
  TRUE is set for successful execution; FALSE otherwise
*/
boolean ftm_wcdma_xpt_override_rf_settings
(
  ftm_xpt_txagc_override_type *params
  )
{
  boolean ret_val = TRUE;
  rfcom_band_type_u rfm_band;

  /* FTM xPT Tx AGC override does not override the DPD HW */
  uint8  amam_bank_idx = -1;
  uint16 amam_start_offset = -1;
  uint8  amam_tbl_size = 0;
  uint8  ampm_bank_idx = -1;
  uint16 ampm_start_offset = -1;
  uint8  ampm_tbl_size = 0;
  uint8  epdt_bank_idx = -1;
  uint16 epdt_start_offset = -1;
  uint8  epdt_tbl_size = 0;

  if (params == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,
        "ftm_wcdma_xpt_override_txagc: Null argument");
    return FALSE;
  }

  if (rfcommon_debug_flags.use_rflm_txagc_override)
  {
    /* Process override params */
    ret_val &= ftm_wcdma_xpt_set_txagc_override_params(
      params->device,
      params->xpt_mode,
      amam_bank_idx,
      amam_start_offset,
      amam_tbl_size,
      ampm_bank_idx,
      ampm_start_offset,
      ampm_tbl_size,
      epdt_bank_idx,
      epdt_start_offset,
      epdt_tbl_size,
      params->iq_gain_action_type,
      params->iq_gain,
      params->env_scale_action_type,
      params->env_scale,
      params->rgi,
      params->smps_bias,
      params->pa_state,
      params->pa_curr,
      params->delay_action_type,
      params->delay);

    /* Apply the Tx override through RFLM */
    rfm_band.wcdma_band = ftm_curr_wcdma_mode;
    ret_val &= rfm_ftm_apply_xpt_tx_agc_override(RFM_IMT_MODE,
                                                 params->device,
                                                 rfm_band);
  }
  else
  {
    ret_val &= ftm_wcdma_xpt_override_txagc(params->device,
                                            params->xpt_mode,
                                            amam_bank_idx,
                                            amam_start_offset,
                                            amam_tbl_size,
                                            ampm_bank_idx,
                                            ampm_start_offset,
                                            ampm_tbl_size,
                                            epdt_bank_idx,
                                            epdt_start_offset,
                                            epdt_tbl_size,
                                            params->iq_gain_action_type,
                                            params->iq_gain,
                                            params->env_scale_action_type,
                                            params->env_scale,
                                            params->rgi,
                                            params->smps_bias,
                                            params->pa_state,
                                            params->pa_curr,
                                            params->delay_action_type,
                                            params->delay);
  }

  return ret_val;
} /* ftm_wcdma_xpt_override_rf_settings */

/*============================================================================

FUNCTION FTM_WCDMA_SET_ET_PATH

DESCRIPTION
  This API is used to setup the ET Path. Currently code looks similar across
  technologies but is subject to change later

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_set_et_path(uint8 pa_state, boolean enable_et_path)
{
  const rfc_common_logical_device_params_type *cmn_logical_dev = NULL;
  /* Get pointer to the common logical device */
  cmn_logical_dev = rfc_common_get_logical_device_params(RFM_DEVICE_0);

  if (NULL != cmn_logical_dev)
  {
    rfdevice_cmn_set_et_path(cmn_logical_dev->cmn_device,
                             RFM_IMT_MODE, pa_state, enable_et_path);
  }
  else
  {
    FTM_MSG_ERROR("Could not get logical device", 0, 0, 0);
  }

  return;
}

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
  This API is used to get the IQ capture configuration for WCDMA.

DEPENDENCIES
  None

RETURN VALUE
  Always TRUE

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_wcdma_iq_capture_get_config(ftm_iq_capture_get_config_type *data)
{
  // Since this command is just to query configuration, return SUCCESS always.
  ftm_iq_capture_error_code_type ret_value = FTM_IQ_SUCCESS;

  data->max_sample_size = FTM_WCDMA_IQ_MAX_SAMPLES;
  data->sample_data_format = FTM_WCDMA_IQ_SS_DATA_FORMAT;
  data->sampling_freq = FTM_WCDMA_IQ_SAMPLE_FREQ_CHIPX2;
  data->max_diag_sample_size = FTM_WCDMA_IQ_DIAG_MAX_SAMPLES;

  MSG_4(MSG_SSID_FTM, MSG_LVL_HIGH, "WCDMA IQ capture configuration: Max sample size %d, Sample data format width %d, Sampling freq %lu Hz, Max diag sample size %d", data->max_sample_size, data->sample_data_format, data->sampling_freq, data->max_diag_sample_size);

  return ret_value;
}

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for WCDMA and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE,  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/

ftm_iq_capture_error_code_type ftm_wcdma_iq_capture_acquire_samples(rfm_device_enum_type device, ftm_iq_capture_acquire_samples_type *data)
{


  /********************* Local variables definition  ******************************************/

  // temp variables to hold the information
  ftm_config_iq_log_type iq_config = { 0 };
  int sample_number = 0;
  uint32 *iq_buffer_temp = NULL;
  ftm_iq_capture_error_code_type ret_value = FTM_IQ_SUCCESS;


  /*******************************************************************************/

  FTM_MSG_HIGH("WCDMA IQ Capture: Acquire samples command sent with parameters Device %d, Carrier ID %d, No. of Req. samples %d", device, data->carrier_id, data->sample_size);

  /***********************Do one time initialization*********************************************/


  // Allocate memory of Maximum sample size 2 * 8192 bytes.
  if (ftm_wcdma_iq_acquired_samples_data.iq_buf == NULL)
  {
    ftm_wcdma_iq_acquired_samples_data.iq_buf = (uint16 *)ftm_malloc(2 * FTM_WCDMA_IQ_MAX_SAMPLES);
    // If pointer is NULL even after allocating memory, then declare it as failure
    if (ftm_wcdma_iq_acquired_samples_data.iq_buf == NULL)
    {
      ret_value = FTM_IQ_GENERAL_FAILURE;
      FTM_MSG_ERROR("Unable to allocate memory for IQ buffer", 0, 0, 0);
    }
  }

  /**************** Update the variables with requested information  ******************************/


  // Update the strucutre with requested parameters. Start with data_available to be FALSE and change it to TRUE only when are able
  // acquire it sucessfully.
  ftm_wcdma_iq_acquired_samples_data.data_available = FALSE;
  ftm_wcdma_iq_acquired_samples_data.device = device;
  ftm_wcdma_iq_acquired_samples_data.carrier = data->carrier_id;
  ftm_wcdma_iq_acquired_samples_data.req_samples = data->sample_size;

  if (device == RFM_DEVICE_0)
  {
    switch (data->carrier_id)
    {
    case 0:
      iq_config.testbus_sel = FTM_HSPA_WCDMA_RXF0_NARROWBAND_C0_IQ_FLOATING_SAMPLES;
      break;

    case 1:
      iq_config.testbus_sel = FTM_HSPA_WCDMA_RXF0_NARROWBAND_C1_IQ_FLOATING_SAMPLES;
      break;

    default:
      FTM_MSG_ERROR("Wrong configuration passed: Device %d Carrier id %d", device, data->carrier_id, 0);
      ret_value = FTM_IQ_GENERAL_FAILURE;
      break;
    }
  }


  else if (device == RFM_DEVICE_1)
  {
    switch (data->carrier_id)
    {
    case 0:
      iq_config.testbus_sel = FTM_HSPA_WCDMA_RXF1_NARROWBAND_C0_IQ_FLOATING_SAMPLES;
      break;

    case 1:
      iq_config.testbus_sel = FTM_HSPA_WCDMA_RXF1_NARROWBAND_C1_IQ_FLOATING_SAMPLES;
      break;

    default:
      FTM_MSG_ERROR("Wrong configuration passed: Device %d Carrier id %d", device, data->carrier_id, 0);
      ret_value = FTM_IQ_GENERAL_FAILURE;
      break;
    }
  }
  else
  {
    FTM_MSG_ERROR("Wrong path passed %d", device, 0, 0);
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }
  /*******************************************************************************/


  /********************* Check conditions**********************************************/

  // First check if RX path is configured to get IQ samples.
  if ((rfwcdma_mc_get_path_state((rf_path_enum_type)device) != RFWCDMA_MC_STATE_RX) &&
      (rfwcdma_mc_get_path_state((rf_path_enum_type)device) != RFWCDMA_MC_STATE_RXTX))
  {
    FTM_MSG_ERROR("RX chain not configured for this path %d", device, 0, 0);
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }


  // If the request is for secondi carrier, then check if DC is enabled.
  if (data->carrier_id == 1)
  {
    if (rfwcdma_mc_get_carr_state(device) != RFCOM_DUAL_CARRIER)
    {
      FTM_MSG_ERROR("DC not configured", 0, 0, 0);
      ret_value = FTM_IQ_GENERAL_FAILURE;
    }
  }


  // Now check if number of IQ samplese requested is > Max it can acquire
  if (ftm_wcdma_iq_acquired_samples_data.req_samples > FTM_WCDMA_IQ_MAX_SAMPLES)
  {
    FTM_MSG_ERROR("Number of samples requested %d > Max. value %d", ftm_wcdma_iq_acquired_samples_data.req_samples, FTM_WCDMA_IQ_MAX_SAMPLES, 0);
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }
  /*******************************************************************************/


  /********************* Get samples and copy to acquisition buffer ****************************/


  // Get samples only if we pass all check conditions. Each sample is of 2 bytes I(1 byte) + Q (byte)
  if (ret_value == FTM_IQ_SUCCESS)
  {
    // Invalidate previous samples, 2 * 8192 bytes
    memset(ftm_wcdma_iq_acquired_samples_data.iq_buf, 0, 2 * FTM_WCDMA_IQ_MAX_SAMPLES);

    iq_buffer_temp = rfwcdma_msm_iq_capture(&iq_config);
    if (iq_buffer_temp != NULL)
    {
      for (sample_number = 0; sample_number < ftm_wcdma_iq_acquired_samples_data.req_samples; sample_number++) 
        ftm_wcdma_iq_acquired_samples_data.iq_buf[sample_number] = iq_buffer_temp[sample_number];
    }
    // free rfwcdma iq buffer
      rfwcdma_msm_free_iq_buffer();
  }

  /*******************************************************************************/

  // If we are sucessful, set the data_available flag.
  if (ret_value == FTM_IQ_SUCCESS)
  {
    ftm_wcdma_iq_acquired_samples_data.data_available = TRUE;
    FTM_MSG_HIGH("%d WCDMA IQ samples sucessfully acquired for path %d carrier %d", data->sample_size, device, data->carrier_id);
  }
  else
  {
    ftm_wcdma_iq_acquired_samples_data.data_available = FALSE;
    FTM_MSG_ERROR("WCDMA IQ samples could not be acquired for path %d, carrier %d, Sample size %d", device, data->carrier_id, data->sample_size);
  }

  return ret_value;

}

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_ANALYZE_SAMPLES

DESCRIPTION
  This API has two operations
  1) Acquire IQ samples 
  2) Analyze samples and call ftm common function to compute sensitivity

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE,  - Acquire/Analyze samples failed
SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_wcdma_iq_capture_analyze_samples(rfm_device_enum_type device,ftm_iq_capture_analyze_samples_type *data, ftm_rsp_pkt_type *rsp_pkt)
{
  
  /********************* Local variables definition  ******************************************/
  
  ftm_iq_capture_error_code_type ret_value = FTM_IQ_GENERAL_FAILURE;
  ftm_common_sens_est_result_type sens_est_result = {0,0,0};
  ftm_rf_technology_type tech = FTM_RF_TECH_WCDMA; 
  uint32 samp_freq = FTM_WCDMA_IQ_SAMPLE_FREQ_CHIPX2;
  uint32 sample_size = data->sample_size; 
  uint16 * iq_buf_ptr;
  int32 estimated_sensitivity = 0;
  int32 peak_frequency = 0;
  int32 cton = 0;
  uint32 sample_count = 0;
  static int32 analysis_result[FTM_IQ_CAPTURE_ANALYZE_SAMPLES_MAX_PAYLOAD_SIZE];
  uint16 var_count = 0;
  uint16 payload_size = 0;
  uint32 analysis_id = 0;
  boolean enable_cton = FALSE;
  boolean enable_estimated_sensitivity = FALSE;
  boolean status = FTM_IQ_SUCCESS;
  diagpkt_subsys_cmd_code_type cmd_id; 
  ftm_iq_capture_acquire_samples_type *iq_acquire_data;
  uint16 dl_pwr_lvl;
  
    /************************************************************************************/
  
  FTM_MSG_HIGH("WCDMA IQ Capture: Analyze samples command sent with parameters Device %d, rx_chain %d, No. of Req. samples %d", device, data->select_rx_chain, data->sample_size);

  
  /**************** STEP 1: Acquire IQ samples  *******************************************/

  /*Data structure for Acquire samples*/
  
  iq_acquire_data = (ftm_iq_capture_acquire_samples_type*)ftm_malloc(sizeof(ftm_iq_capture_acquire_samples_type));
  if (iq_acquire_data == NULL)
  {
    FTM_MSG_ERROR("Failed to allocate memory for iq data acquisition", 0, 0, 0);
    return ret_value;
  }  
  iq_acquire_data->sample_size = sample_size;
  iq_acquire_data->ftm_error_code = FTM_IQ_MAX_ERROR;
  
  /*Convert device to carrier*/
     if(ftm_wcdma_get_current_test_scenario()==0) 
     {
       /* DBDC mode*/
       switch (device)
       {
        case RFM_DEVICE_0:
        case RFM_DEVICE_1:
	      iq_acquire_data->carrier_id = 0;
          break;
	    case RFM_DEVICE_2:
	    case RFM_DEVICE_3:
	 	  iq_acquire_data->carrier_id = 1;
		  break;
	    default :
          RF_MSG_1(RF_ERROR, "ftm_wcdma_iq_capture_analyze_samples: Invalid device, no carrier found for Device: %d",device);
          break;
       }
     }
     else
     {
       iq_acquire_data->carrier_id = 0;
     }
  /*Samples captures are stored in ftm_wcdma_iq_acquired_samples_data.iq_buf in format  16bit sample (I(8bit) + Q(8bit)) */
  ret_value = ftm_wcdma_iq_capture_acquire_samples(device,iq_acquire_data);

  if(ret_value == FTM_IQ_GENERAL_FAILURE)
  {
    FTM_MSG_ERROR("WCDMA IQ Capture STEP 1: Acquire samples failed on Device %d, rx_chain %d, Ret_value %d", device, data->select_rx_chain, ret_value);
	return ret_value;
  }
  
  /******************STEP 2: Analyze samples and compute sensitivity******************************************/

  /*Allocate memory to iq_buf_ptr, sample size*/
  /* To keep KW happy */
    sample_size = sample_size * sizeof(uint16);

   iq_buf_ptr = (uint16*)ftm_malloc(sample_size);

   if(iq_buf_ptr == NULL)
   {
     FTM_MSG_ERROR("WCDMA IQ Capture: Unable to allocate memory for iq_buf_ptr",0,0,0);
	 return ret_value;
   }

  /* Copy samples from global into iq_buf_ptr */
  for(sample_count = 0; (sample_count < (sample_size/sizeof(uint16))); sample_count++)
  {
    iq_buf_ptr[sample_count] = ftm_wcdma_iq_acquired_samples_data.iq_buf[sample_count];
	
  }

  dl_pwr_lvl = (int16)(data->dl_pwr & 0xffff);
  ftm_common_analyze_iq_samples(&sens_est_result, iq_buf_ptr, (sample_size/sizeof(uint16)), samp_freq, tech, dl_pwr_lvl);
 
  
  cton = sens_est_result.cnr_db100;
  estimated_sensitivity = sens_est_result.sens_db100;
  peak_frequency = sens_est_result.peak_freq;

  analysis_id = data->analysis_id;

  enable_cton = (analysis_id & FTM_IQ_CAPTURE_MEASURE_CTON)? TRUE: FALSE;
  enable_estimated_sensitivity = (analysis_id & FTM_IQ_CAPTURE_MEASURE_ESTIMATED_SENSITIVITY)? TRUE: FALSE;

   if (enable_estimated_sensitivity)
    {
      analysis_result[var_count++] = estimated_sensitivity;
      payload_size += sizeof(estimated_sensitivity);
      analysis_result[var_count++] = peak_frequency;
      payload_size += sizeof(peak_frequency);
    }

    if (enable_cton)
    {
      analysis_result[var_count++] = cton;
      payload_size += sizeof(cton);
    }

  cmd_id = (device == RFM_DEVICE_0) ? FTM_WCDMA_C : FTM_WCDMA_RX_2_C;
  ftm_wcdma_iq_capture_analyze_samples_rsp_diag_pkt(rsp_pkt, status, cmd_id, analysis_id, payload_size,(int8 *) analysis_result);
  return ret_value;
 
}

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for WCDMA from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 The CAPTURE command should have been issued.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_iq_capture_get_samples(rfm_device_enum_type device, ftm_iq_capture_get_samples_type *data, ftm_rsp_pkt_type *rsp_pkt)
{

  uint16 sample_number = 0, counter = 0;

  FTM_MSG_HIGH("WCDMA IQ Capture: Get samples command sent with parameters Device %d, Sample offset %d, No. of Req. samples %d", device, data->sample_offset, data->sample_size);

  /***************** Fill up RSP packet with standard data *****************/

  // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.cmd_code = 75;
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.subsys_id = FTM;
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_WCDMA_C : FTM_WCDMA_RX_2_C;
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_rf_cmd = FTM_IQ_CAPTURE_GET_SAMPLES;
  // Start with sucess flag and change it if we fail any check conditions.
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_error_code = FTM_IQ_SUCCESS;
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.sample_offset = data->sample_offset;
  ftm_wcdma_iq_capture_get_samples_rsp_pkt.sample_size = data->sample_size;


  // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_wcdma_iq_capture_get_samples_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;


  /***************** Do sanity checks *******************************/

  // First check if data is available
  if (ftm_wcdma_iq_acquired_samples_data.data_available == FALSE)
  {
    FTM_MSG_ERROR("IQ samples are not acquired, please acquire them first", 0, 0, 0);
    ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_error_code = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  }

  // Check if IQ has been acquired for the same device.
  if (ftm_wcdma_iq_acquired_samples_data.device != device)
  {
    FTM_MSG_ERROR("IQ samples are not acquired for this device, please acquire them first", 0, 0, 0);
    ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_error_code = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  }


  // Check if number of samples requested is greater than acquired.
  if ((data->sample_offset + data->sample_size) > ftm_wcdma_iq_acquired_samples_data.req_samples)
  {
    FTM_MSG_ERROR("Sample offset or Sample size is wrong %d %d", data->sample_offset, data->sample_size, 0);
    ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_error_code =  FTM_IQ_ILLEGAL_COMBIMATION;
  }

  // check if requested number of samples > Max. value that can be sent through Diag
  if (data->sample_size > FTM_WCDMA_IQ_DIAG_MAX_SAMPLES)
  {
    FTM_MSG_ERROR("Requested Sample size %d is greater than %d", data->sample_size, FTM_WCDMA_IQ_DIAG_MAX_SAMPLES, 0);
    ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_error_code =  FTM_IQ_GENERAL_FAILURE;
  }


  /*******Fill up remaining part of RSP packet based on sanity checks and acquired data***********/

  // If all the sanity checks are passed, we will be sending IQ samples in Response packet.
  if (ftm_wcdma_iq_capture_get_samples_rsp_pkt.ftm_error_code == FTM_IQ_SUCCESS)
  {
    // Copy the IQ data into diag command buffer
    // Extract the I and Q information from acquired IQ buffer [sample_offset,sample_offset+sample_number) and copy it to diag in the format IQIQIQ

    for (sample_number = data->sample_offset,counter = 0; sample_number < (data->sample_size + data->sample_offset); sample_number++, counter += 2)
    {
      ftm_wcdma_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_8_bit[counter] = (int8)(((ftm_wcdma_iq_acquired_samples_data.iq_buf[sample_number]) & 0xFF00) >> 8);
      ftm_wcdma_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_8_bit[counter + 1] = (int8)((ftm_wcdma_iq_acquired_samples_data.iq_buf[sample_number]) & 0xFF);
    }

    rsp_pkt->pkt_len = FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE + 2 * data->sample_size; // Header size +   (I + Q , each 1 byte) * total number

    FTM_MSG_HIGH("%d WCDMA IQ samples sent to Diag with sample offset %d", data->sample_size, data->sample_offset, 0);

  }
  // If we fail checks, dont fill IQ samples. RSP packet will only have header.
  else
  {
    //   memset(ftm_wcdma_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_8_bit,0,FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
    rsp_pkt->pkt_len =  FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE;
    FTM_MSG_ERROR("WCDMA IQ samples Capture failed for SAMPLE SIZE %d and SAMPLE OFFSET %d", data->sample_size, data->sample_offset, 0);
  }

}

/*===========================================================================
FUNCTION FTM_WCDMA_IQ_CAPTURE_ANALYZE_SAMPLES_RSP_DIAG_PKT
/*!
  @brief This function builds the DIAG response packet for
         FTM_RESET_MULTI_SYNTH_STATE

  @param rsp_pkt_ptr pointer to the DIAG response packet
  @param status the status of the response

  @return None
*/
/*=========================================================================*/

void ftm_wcdma_iq_capture_analyze_samples_rsp_diag_pkt(ftm_rsp_pkt_type *rsp_pkt_ptr, ftm_iq_capture_error_code_type status,diagpkt_subsys_cmd_code_type cmd_id,uint32 analysis_id,uint16 size,int8 *analysis_result)
{
  if (rsp_pkt_ptr != NULL)
  {
   // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.cmd_code = 75;
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.subsys_id = FTM;
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.subsys_cmd_code = cmd_id;
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.ftm_rf_cmd = FTM_IQ_CAPTURE_ANALYZE_SAMPLES;  
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.ftm_error_code= (uint16)status;
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.analysis_id = analysis_id;
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.reserved = (uint32)0;
   ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.payload_size=size;   
   
   // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
   rsp_pkt_ptr->cmd = FTM_RSP_DO_NEW_PKT;   
   rsp_pkt_ptr->pkt_payload = &(ftm_wcdma_iq_capture_analyze_samples_rsp_pkt);
   rsp_pkt_ptr->delete_payload = FALSE;
   rsp_pkt_ptr->pkt_len = FTM_IQ_CAPTURE_ANALYZE_SAMPLE_HDR_SIZE;

   // Check if we have passed all conditions and requested size <= max payload size.
   if((status == FTM_IQ_SUCCESS) && (size <= FTM_IQ_CAPTURE_ANALYZE_SAMPLES_MAX_PAYLOAD_SIZE) )
   {
      memscpy(ftm_wcdma_iq_capture_analyze_samples_rsp_pkt.analysis_result,size,analysis_result,size);
      rsp_pkt_ptr->pkt_len += size; // Header size + payload_size
   }
   else
   {
      rsp_pkt_ptr->pkt_len = FTM_IQ_CAPTURE_ANALYZE_SAMPLE_HDR_SIZE;
   }     
  }
}

/*============================================================================

FUNCTION FTM_WCDMA_FREE_MEMORY

DESCRIPTION
  This API is used to free up the memory allocated in WCDMA FTM mode. This API will be generally called while exiting the mode

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_wcdma_free_memory(void)
{

  // Check if memory has been allocated to acquire IQ samples.
  if (ftm_wcdma_iq_acquired_samples_data.iq_buf != NULL)
  {
    ftm_free(ftm_wcdma_iq_acquired_samples_data.iq_buf);
    ftm_wcdma_iq_acquired_samples_data.iq_buf = NULL;
  }
  
}

/*----------------------------------------------------------------------------*/

#ifdef FEATURE_LTE
/*!
  @brief
  Tunes RF devices' Tx using LTE's Tx settings
 
  @details
  Configures only the RF devices with LTE settings while leaving
  the baseband devices with WCDMA settings.
 
  @pre
  WCDMA RF Device(s) must be in RX or RXTX state
 
  @param device
  The device whose Tx to configure with LTE Tx settings
 
  @param band
  LTE band to use for tuning
 
  @param tx_chan
  TX Channel to use for tuning
 
  @return
  TRUE if success
*/
boolean
ftm_wcdma_tune_with_lte_settings
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  uint32 tx_chan
  )
{
  boolean api_status = TRUE;
  rf_path_enum_type path;
  uint32 freq;
  rfdevice_lte_script_data_type lte_script = { TRUE, NULL };

  path = rfcommon_core_device_to_path(device);

  /*  */
  if (!((rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_RXTX) ||
        (rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_RX)
        )
      )
  {
    RF_MSG_2(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - WCDMA is in invalid state "
             "%d for device %d. Valid states are RX, RXTX, or Sleep.",
             rfwcdma_mc_state.car_path_state[path][0].rf_state, device);
    return FALSE; /* No need to continue at this point */
  }

  /* ************************ Check Parameters ************************ */

  lte_script.script = rf_buffer_create(RFLTE_MC_MAX_SSBI_SCRIPT_SIZE,
                                       RFLTE_MC_MAX_RFFE_SCRIPT_SIZE,
                                       RFLTE_MC_MAX_GRFC_SCRIPT_SIZE);
  if (lte_script.script == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - Could not create LTE buffer.");
    api_status = FALSE;
  }

  /* ************************ WCDMA: Sleep RF device ************************ */

  /* TX SLEEP */
  if (api_status == TRUE)
  {
    /* Only if transmitting */
    if (rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_RXTX)
    {
      api_status = (boolean)rfwcdma_mc_disable_tx(device);

      if (api_status == FALSE)
      {
        RF_MSG_1(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - WCDMA TX sleep failed "
                 "for device %d", device);
      }
    } /* Only if transmitting */
  } /* TX SLEEP */
  else
  {
    RF_MSG_1(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - TX not set to sleep "
             "for device %d", device);
  }

  /* ************************ WCDMA: Wakeup RF device with LTE settings ************************ */

  /* WCDMA TX WAKEUP WITH LTE */
  if (api_status == TRUE)
  {
    api_status &= rf_buffer_clear(lte_script.script);
    if (api_status == FALSE)
    {
      RF_MSG_1(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - Could not reset LTE buffer. "
               "TX not set to wakeup for device %d", device);
    }
    else
    {
      rfc_lte_manage_vregs(device, band, RF_PATH_RXTX_STATE);
      rfc_lte_config_tx_band_data(device, band);

      rfdevice_lte_tx_init(device, band, &lte_script);
      rfdevice_lte_tx_set_mode(device, band, &lte_script);
      rfdevice_lte_tx_config_band(device, band, &lte_script);
      rfdevice_lte_tx_set_bw(device, band, RFCOM_BW_LTE_20MHz, &lte_script);
      freq = rflte_core_get_tx_freq_from_uarfcn(tx_chan, band);
      rfdevice_lte_tx_tune_to_chan(device, band, freq, 0, RFDEVICE_DPD_DISABLED, &lte_script, TRUE);

      if (api_status == FALSE)
      {
        RF_MSG_1(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - WCDMA TX wakeup with LTE failed "
                 "for path %d", path);
      }
    }
  } /* WCDMA TX WAKEUP WITH LTE */
  else
  {
    RF_MSG_1(RF_ERROR, "ftm_wcdma_tune_with_lte_settings - WCDMA TX not awake with LTE settings "
             "for path %d", path);
  }

  /* ************************ Cleanup ************************ */
  api_status &= rf_buffer_destroy(lte_script.script);

  return api_status;
} /* ftm_wcdma_tune_with_lte_settings */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set LTE PA State
  
  @pre
  Call ftm_wcdma_tune_with_lte_settings
 
  @param device
  The device to configure
 
  @param pa_state
  PA state to use
  
  @return
  TRUE if success
*/
boolean
ftm_wcdma_set_lte_pa_state
(
  rfm_device_enum_type device,
  uint8 pa_state
  )
{
  boolean api_status = TRUE;

  if (pa_state > RF_PA_RANGE_11)
  {
    RF_MSG_2(RF_ERROR, "ftm_wcdma_set_lte_pa_state - PA State %d is > "
             "than allowed value of %d", pa_state, RF_PA_RANGE_11);
    return FALSE;
  }

  cur_pa_range = pa_state;

  return api_status;
} /* ftm_wcdma_set_lte_pa_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set LTE RGI
  
  @pre
  Call ftm_wcdma_set_lte_pa_state
 
  @param device
  The device to configure
 
  @param rgi
  RGI to use
  
  @return
  TRUE if success
*/
boolean
ftm_wcdma_set_lte_rgi
(
  rfm_device_enum_type device,
  uint16 rgi
  )
{
  boolean api_status = TRUE;
  rfdevice_lte_set_tx_char_tbl_entry_type ftm_wcdma_lte_tx_char_tbl_entry;
  rfdevice_lte_script_data_type lte_script = { TRUE, NULL };

  if (cur_pa_range > RF_PA_RANGE_11)
  {
    RF_MSG_2(RF_ERROR, "ftm_wcdma_set_lte_rgi - PA State %d is > "
             "than allowed value of %d",
             cur_pa_range, RF_PA_RANGE_11);
    return FALSE;
  }

  /* ************************ Create Buffer ************************ */
  lte_script.script = rf_buffer_create(RFLTE_MC_MAX_SSBI_SCRIPT_SIZE,
                                       RFLTE_MC_MAX_RFFE_SCRIPT_SIZE,
                                       RFLTE_MC_MAX_GRFC_SCRIPT_SIZE);
  if (lte_script.script == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_wcdma_set_lte_rgi - Could not create LTE buffer.");
    api_status = FALSE;
  }

  /* ************************ Config RGI ************************ */
  if (api_status == TRUE)
  {
    ftm_wcdma_lte_tx_char_tbl_entry.pa_state = cur_pa_range;
    ftm_wcdma_lte_tx_char_tbl_entry.char_entry = (uint8)rgi;
    /*!@todo for using this API, band information also needs to be passed due to a device intf change. hardcoding to B1 for now.*/
    (void)rfdevice_lte_tx_cmd_dispatch(device,
                                       RFCOM_BAND_LTE_B1,
                                       RFDEVICE_LTE_SET_TX_CHAR_TBL_ENTRY,
                                       &ftm_wcdma_lte_tx_char_tbl_entry,
                                       &lte_script);
    /*! @todo W dif not yet have interface to commit the generated script. Must 
        add that support when interface is available. */
  }

  /* ************************ Cleanup ************************ */
  api_status &= rf_buffer_destroy(lte_script.script);

  return api_status;
} /* ftm_wcdma_set_lte_rgi */
#endif /*FEATURE_LTE*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable Multi-carrier Tx Mode for FTM

  @pre
  DC-HSDPA must be enabled on the downlink before calling this function
  SC Tx must be enabled before switching UL bandwidth

  @param device
  The device to configure

  @param state
  Enable or disable

  @param multi_carrier_idx
  Multi-carrier index

*/
void ftm_wcdma_set_tx_multi_carrier(rfm_device_enum_type device,
                                    uint16 state,
                                    uint32 multi_carrier_idx)
{
  rfm_wcdma_tx_retune_bw_param retune_param = {0};
  rfm_wcdma_tx_init_params tx_params_i;
  rfm_wcdma_tx_enable_params tx_params_e;

  memset(&tx_params_i, 0, sizeof(rfm_wcdma_tx_init_params)); 

  memset(&tx_params_e, 0, sizeof(rfm_wcdma_tx_enable_params)); 

  tx_params_i.carrier_mask = RFWCDMA_MC_DC;
  tx_params_i.device = device;
  tx_params_i.txlm_handle = ftm_wcdma_txlm_buffer.buf_idx;
  
  tx_params_e.device = device; 
  
  MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH, "Set Multi-carrier Tx, device=%d, state=%d, multi_carrier_idx=%d",
        device, state, multi_carrier_idx);

  if (state == 1)
  {
    /* Enable Tx in SC mode first */
    ftm_wcdma_set_tx(RFM_DEVICE_0, TRUE);

    /* Switch to DC */
    rfm_wcdma_tx_init(&tx_params_i,NULL,NULL); 

    /* Todo: remove hardcoding and direct FW SM access */
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99[0].txDipPlusCmDelta = 300;
    #if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))    
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99[1].txDipPlusCmDelta = 300;
    #endif
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[0].enable = 1;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[0].betaGain0 = 0x7FFE;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[1].enable = 1;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[1].betaGain0 = 0x7FFE;
    #if (!( defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99[1].dchScrCodeInitData0 = 666;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99[1].scrCodeType = 1;
    #endif

    mdsp_sync_send_ul_dpcch_cfg_cmd
      (
      1,     // enable DPCCH processing
      0,
      1, // slot format. use 1 for DC
      0,
      2, /* wait for scch order = FALSE*/
      0,
      NULL,  // pre-cb
      NULL   // post-cb
      );
    retune_param.device = RFM_DEVICE_0;
    retune_param.multi_carrier_idx = RFWCDMA_MC_DC;
    
    /* Retune BW and enable Tx in DC*/
    rfm_wcdma_tx_retune_bw(&retune_param, NULL, NULL);

    rfm_wcdma_tx_enable(&tx_params_e, NULL,NULL);
  }
  else
  {
    ftm_wcdma_set_tx(RFM_DEVICE_0, FALSE);
  }

  /* Temproarily enable TxAGC to latch TxFE DRIF update */
  rfwcdma_core_tx_disable_txagc(rfwcdma_mc_state.txlm_buf_idx, FALSE);

  /* Wait 1ms for TxAGC to latch */
  DALSYS_BusyWait(1000);

  /* If currently in Cal mode. Disable TxAGC back*/
  if (IS_FTM_CAL_MODE())
  {
    rfwcdma_core_tx_disable_txagc(rfwcdma_mc_state.txlm_buf_idx, TRUE);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the gains for each carrier in Multi-carrier Tx Mode for FTM
  
  @pre
  ftm_wcdma_set_tx_multi_carrier must be called to enable multi-carrier Tx

  @param gain0
  Digital gain for Carrier 0
 
  @param gain1
  Digital gain for Carrier 1
  
*/
void ftm_wcdma_set_tx_multi_carrier_gains(uint16 gain0, uint16 gain1)
{
  if (gain0 == 0 && gain1 == 0)
  {
    ftm_wcdma_10mhz_mode_bb = TRUE;
    ftm_wcdma_10mhz_mode_rf = TRUE;
    FTM_MSG_ERROR("Setting BB and RF to 10MHz %d %d", gain0, gain1, 0);
  }
  else if (gain0 == 1 && gain1 == 0)
  {
    ftm_wcdma_10mhz_mode_bb = TRUE;
    ftm_wcdma_10mhz_mode_rf = FALSE;
    FTM_MSG_ERROR("Setting BB but not the RF to 10MHz %d %d", gain0, gain1, 0);
  }
  else if (gain0 == 0 && gain1 == 1)
  {
    ftm_wcdma_10mhz_mode_bb = FALSE;
    ftm_wcdma_10mhz_mode_rf = TRUE;
    FTM_MSG_ERROR("Setting RF but not the BB to 10MHz %d %d", gain0, gain1, 0);
  }
  else
  {
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[0].enable = 1;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[0].betaGain0 = gain0;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[1].enable = 1;
    rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.betaOverride[1].betaGain0 = gain1;
    FTM_MSG_ERROR("Setting Beta gains to %d %d", gain0, gain1, 0);
  }
}

/*============================================================================

FUNCTION ftm_wcdma_set_tuner_tune_code_override

DESCRIPTION
  This API is used override the current Antenna tuner tune code settings 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/


ftm_tuner_override_status_type ftm_wcdma_set_tuner_tune_code_override(rfm_device_enum_type device, uint8 override_flag, void *data, uint8 *tuner_nv_ptr,uint8 *tuner_id_ptr)
{
  ftm_tuner_override_status_type status = FTM_TUNER_OVERRIDE_ERROR;

#ifdef FEATURE_RF_HAS_QTUNER
  status = rfwcdma_core_antenna_tuner_tune_code_override(device,
                                                         rfwcdma_mc_state.car_path_state[device][0].curr_band_rx,
                                                         override_flag,
                                                         data,
                                                         tuner_nv_ptr,
                                                         tuner_id_ptr);
#endif


  return status;
}

/*============================================================================

FUNCTION ftm_wcdma_do_rsb_cal

DESCRIPTION
  This fuction performs rsb calibration. The single tone generator channel is
  configured to provide a tone input to the lna of the transceiver. IQ samples
  are captured for this tone signal with the iq mismatch coefficients zeroed 
  out. IQ samples are processed using rsb algorithm to obtain the rsb a and
  rsb b coefficients. These values are stored in nv items for future access. 

DEPENDENCIES
  None

RETURN VALUE
  boolean value indicating the status as TRUE or FALSE 

SIDE EFFECTS

=============================================================================*/
#if 0
boolean ftm_wcdma_do_rsb_cal
(
  ftm_rsb_txlo_rsp_type *rsp_pkt,
  ftm_rsb_txlo_pkt_type *cal_params 
  )
{
  ftm_mode_id_type mode         = FTM_PHONE_MODE_WCDMA_IMT;
  boolean spec_inv_status       = FALSE;
  rfcom_wcdma_band_type rf_band = RFCOM_BAND_IMT;
  uint16 default_channel[2] = { 0, 0 };
  uint16 stg_channel            = 0;
  uint16 tx_channel             = 0;
  boolean prx_status            = TRUE;
  boolean drx_status            = TRUE;
  boolean update_cmd_status     = TRUE;
  int32 rsb_a                   = 0;
  int32 rsb_b                   = 0;
  rfwcdma_mdsp_dyn_rxlm_cmd_param_type dyn_rxlm_update_cmd_param = {{0},0,0};
  uint32 chain_mask = 0;


  rf_time_tick_type prof_rsb_cal;
  rf_time_type      prof_rsb;

  prof_rsb_cal = rf_time_get_tick();

  /* Convert ftm band type to rfcom band type */
  rf_band = ftm_wcdma_convert_phone_mode_to_rfcom_band_type(rsp_pkt->curr_band);

  /* Check for incorrect band specification */
  if (rf_band >= RFCOM_NUM_WCDMA_BANDS)
  {
    rsp_pkt->cal_status = FTM_UNSUPPORTED_BAND;
    return FALSE;
  }

  /* Set current RF mode to invalid */
  ftm_current_rf_mode = FTM_DB_RF_MAX;

  /* Set div enable flag to FALSE => force tune DRx */
  ftm_wcdma_div_status[RFM_DEVICE_1] = FALSE;

  /* Convert rfcom band type to ftm phone mode type */
  switch (rf_band)
  {
  case RFCOM_BAND_IMT:
    mode = FTM_PHONE_MODE_WCDMA_IMT;
    break;

  case RFCOM_BAND_1900:
    mode = FTM_PHONE_MODE_WCDMA_1900A;
    break;

  case RFCOM_BAND_BC3:
    mode = FTM_PHONE_MODE_WCDMA_BC3;
    break;

  case RFCOM_BAND_BC4:
    mode = FTM_PHONE_MODE_WCDMA_BC4;
    break;

  case RFCOM_BAND_800:
    mode = FTM_PHONE_MODE_WCDMA_800;
    break;

  case RFCOM_BAND_BC8:
    mode = FTM_PHONE_MODE_WCDMA_BC8;
    break;

  case RFCOM_BAND_BC9:
    mode = FTM_PHONE_MODE_WCDMA_BC9;
    break;

  case RFCOM_BAND_BC11:
    mode = FTM_PHONE_MODE_WCDMA_BC11;
    break;

  case RFCOM_BAND_BC19:
    mode = FTM_PHONE_MODE_WCDMA_BC19;
    break;

  default:
    mode = FTM_PHONE_MODE_MAX;
    break;
  }

  /* Enter W mode */

  FTM_MSG_1(FTM_HIGH, "RSB Before enter mode WCDMA: %d", ftm_current_rf_mode);
  FTM_MSG_1(FTM_HIGH, "RSB Div Device ID: %d", cal_params->div_device); 

  ftm_rfmode_enter(RFM_DEVICE_0, FTM_STATE_WCDMA);
  FTM_MSG_1(FTM_HIGH, "RSB, WCDMA enter mode for dev 0 is done: %d", RFM_DEVICE_0);

  if (cal_params->div_device != 0xFF) 
  {
  ftm_rfmode_enter(RFM_DEVICE_1, FTM_STATE_WCDMA);
    FTM_MSG_1(FTM_HIGH, "RSB, WCDMA enter mode for dev 1 should not be done: %d", RFM_DEVICE_1);
  }

  ftm_wcdma_set_mode(mode);

  FTM_MSG_1(FTM_HIGH, "RSB After enter mode WCDMA: %d", ftm_current_rf_mode);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB Enter mode - %d", prof_rsb);

  prof_rsb_cal = rf_time_get_tick();

  /*! ---------------------------------------------------------------------- */
  /*! Primary Rx path RSB calibration */
  /*! ---------------------------------------------------------------------- */

  /* Get the STG channel information from device */
  rfdevice_wcdma_rx_get_stg_channel(RFM_DEVICE_0,
                                    rf_band,
                                    &stg_channel,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL);

  /* If STG Channel is incorrect then report unsupported band */
  if (stg_channel <= 0)
  {
    rsp_pkt->cal_status = FTM_UNSUPPORTED_BAND;
    ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);
    if (cal_params->div_device != 0xFF) 
    {
    ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
    }
    return FALSE;
  }
  else
  {
    /*! For now assign single channel, once DB-DC support is available, need 
        to change this to accept multiple channels */
    default_channel[0] = stg_channel;
  }

  FTM_MSG_2(FTM_LOW, "DL Channel - %d, Band - %d", default_channel[0], rf_band);

  tx_channel = rfwcdma_core_map_rx_to_tx_chan(default_channel[0], rf_band);

  FTM_MSG_2(FTM_LOW, "UL Channel - %d, Band - %d", tx_channel, rf_band);

  ftm_wcdma_tune_to_chan(RFM_DEVICE_0, tx_channel);

  /* Enable secondary chain */
  if (cal_params->div_device != 0xFF) 
  {
    ftm_wcdma_set_secondary_chain(RFM_DEVICE_1,TRUE);
  }

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB PRx Tune time - %d", prof_rsb);

  prof_rsb_cal = rf_time_get_tick();

  
  /* Set IQMC coeff's to zero */
  prx_status &= rfwcdma_msm_update_iqmc_settings(RFM_DEVICE_0,
                                               ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx,
                                               rfwcdma_mc_state.rx_carriers[RFM_DEVICE_0],
                                               rf_band,
                                               0,
                                               0,
                                               0);

  dyn_rxlm_update_cmd_param.rxlm_buf_idx[RFM_DEVICE_0] = ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx;

  /* Get DTR group mask for IQMC */
  dyn_rxlm_update_cmd_param.dtr_group_mask = rfwcdma_msm_get_dtr_group_mask(RFM_DEVICE_0, RFWCDMA_MSM_DTR_IQMC_GROUP);

  /* Rx chain antenna mask is specific to each rfm device */
  rfwcdma_core_rxctl_get_ant_mask(RFM_DEVICE_0, &dyn_rxlm_update_cmd_param.chain_mask);

  /* Send dynamic rxlm update cmd to fw */
  update_cmd_status &= rfwcdma_mdsp_sync_send_cmd(RFWCDMA_MDSP_RXLM_DYN_UPDATE_CMD_ID, (void*)&dyn_rxlm_update_cmd_param);

  /* Set rotator configuration to bypass */
  prx_status &= rfwcdma_msm_bypass_rotator_rxlm((rfm_device_enum_type)FTM_RECEIVE_CHAIN_0,
                                                default_channel,
                                                ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx);

  /*! If LM configuration fails, then report LM config failure */
  if (prx_status == FALSE)
  {
    rsp_pkt->cal_status = FTM_RSB_LM_CON_FAILURE;
    ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);
    if (cal_params->div_device != 0xFF) 
    {
    ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
    }
    return FALSE;
  }

  ftm_wcdma_set_lna_range(RFM_DEVICE_0, 0);
  ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_0, -100);
  ftm_wcdma_set_lna_offset(RFM_DEVICE_0, 0, 200);

  /* Configure the single tone generator */
  if (cal_params->override_stg == FALSE)
  {
    rfdevice_wcdma_rx_configure_stg(RFM_DEVICE_0,
                                    rf_band,
                                    NULL,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL);
  }

  DALSYS_BusyWait(2000);

  DALSYS_BusyWait(2000);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB PRx STG config time - %d", prof_rsb);

  prof_rsb_cal = rf_time_get_tick();

  /* Perform IQ cap and compute the rsb coeffs */

  prx_status &= rfwcdma_msm_get_rsb_coeff((rfm_device_enum_type)FTM_RECEIVE_CHAIN_0,
                                          rf_band, spec_inv_status, &rsb_a, &rsb_b);


  if (cal_params->override_stg == FALSE)
  {
    rfdevice_wcdma_rx_disable_stg(RFM_DEVICE_0,
                                  rf_band,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL);
  }

  /*! Report IQ capture failure */
  if (prx_status == FALSE)
  {
    rsp_pkt->cal_status = FTM_RSB_IQ_CAP_FAILURE;
    ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);
    if (cal_params->div_device != 0xFF) 
    {
    ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
    }
    return FALSE;
  }

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB PRx IQ capture time - %d", prof_rsb);

  FTM_MSG_2(FTM_LOW, "PRX RSB A - %d, RSB B - %d", rsb_a, rsb_b);

  prof_rsb_cal = rf_time_get_tick();

  /*! ---------------------------------------------------------------------- */
  /*! Diversity Rx path RSB calibration */
  /*! ---------------------------------------------------------------------- */

  if (cal_params->div_device != 0xFF) 
  {
  /* Get the STG channel information from device */
  rfdevice_wcdma_rx_get_stg_channel(RFM_DEVICE_1,
                                    rf_band,
                                    &stg_channel,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL);

  /* If STG Channel is incorrect then report unsupported band */
  if (stg_channel <= 0)
  {
    rsp_pkt->cal_status = FTM_UNSUPPORTED_BAND;
    ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
    ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);

    return FALSE;
  }
  else
  {
    /*! For now assign single channel, once DB-DC support is available, need 
        to change this to accept multiple channels */
    default_channel[0] = stg_channel;
  }

  FTM_MSG_2(FTM_LOW, "DL Channel - %d, Band - %d", default_channel[0], rf_band);

  tx_channel = rfwcdma_core_map_rx_to_tx_chan(default_channel[0], rf_band);

  FTM_MSG_2(FTM_LOW, "UL Channel - %d, Band - %d", tx_channel, rf_band);

  ftm_wcdma_tune_to_chan(RFM_DEVICE_1, tx_channel);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB DRx tune time - %d", prof_rsb);

  prof_rsb_cal = rf_time_get_tick();

  /* Set IQMC coeff's to zero */
  drx_status &= rfwcdma_msm_update_iqmc_settings(RFM_DEVICE_1,
                                               ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx,
                                               rfwcdma_mc_state.rx_carriers[RFM_DEVICE_1],
                                               rf_band,
                                               0,
                                               0,
                                               0);

  dyn_rxlm_update_cmd_param.rxlm_buf_idx[RFM_DEVICE_1] = ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx;

  /* Get DTR group mask for IQMC */
  dyn_rxlm_update_cmd_param.dtr_group_mask = rfwcdma_msm_get_dtr_group_mask(RFM_DEVICE_0, RFWCDMA_MSM_DTR_IQMC_GROUP);

  /* Rx chain antenna mask is specific to each rfm device */
  /* We want to enable both chains here in RSB Cal */
  rfwcdma_core_rxctl_get_ant_mask(RFM_DEVICE_0, &chain_mask);
  dyn_rxlm_update_cmd_param.chain_mask = chain_mask;

  rfwcdma_core_rxctl_get_ant_mask(RFM_DEVICE_1, &chain_mask);
  dyn_rxlm_update_cmd_param.chain_mask |= chain_mask;

  /* Send dynamic rxlm update cmd to fw */
  update_cmd_status &= rfwcdma_mdsp_sync_send_cmd(RFWCDMA_MDSP_RXLM_DYN_UPDATE_CMD_ID, (void*)&dyn_rxlm_update_cmd_param);

  if (update_cmd_status == FALSE)
  {
    FTM_MSG_1(FTM_HIGH, "FTM sync cmd update failed - %d", update_cmd_status);
      rsp_pkt->cal_status = FTM_RSB_LM_CON_FAILURE;
      ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
      ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA); 
      return FALSE;  
  }

  /* Set rotator configuration to bypass */
  drx_status &= rfwcdma_msm_bypass_rotator_rxlm((rfm_device_enum_type)FTM_RECEIVE_CHAIN_1,
                                                default_channel,
                                                ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx);

  if (drx_status == FALSE)
  {
    rsp_pkt->cal_status = FTM_RSB_LM_CON_FAILURE;
   ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
   ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);
    return FALSE;
  }

  ftm_wcdma_set_lna_range(RFM_DEVICE_1, 0);
  ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_1, -100);
  ftm_wcdma_set_lna_offset(RFM_DEVICE_1, 0, 250);

  /* Configure the single tone generator */
    if (cal_params->override_stg == FALSE)
  {
     rfdevice_wcdma_rx_configure_stg(RFM_DEVICE_0, 
                                    rf_band, 
                                    NULL, 
                                    RFDEVICE_EXECUTE_IMMEDIATE, 
                                    NULL);

    rfdevice_wcdma_rx_configure_stg(RFM_DEVICE_1,
                                    rf_band,
                                    NULL,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL);
  }

  DALSYS_BusyWait(2000);

  DALSYS_BusyWait(2000);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB DRx STG config time - %d", prof_rsb);

  prof_rsb_cal = rf_time_get_tick();

  /* Perform IQ cap and compute rsb coeffs */
  drx_status &= rfwcdma_msm_get_rsb_coeff((rfm_device_enum_type)FTM_RECEIVE_CHAIN_1,
                                          rf_band, spec_inv_status, &rsb_a, &rsb_b);

    if (cal_params->override_stg == FALSE)
  {
    rfdevice_wcdma_rx_disable_stg(RFM_DEVICE_0, 
                                  rf_band,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL); 

    rfdevice_wcdma_rx_disable_stg(RFM_DEVICE_1,
                                  rf_band,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL);
  }

  if (drx_status == FALSE)
  {
    rsp_pkt->cal_status = FTM_RSB_IQ_CAP_FAILURE;
    ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA);
    ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);
    
    return FALSE;
  }

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);

  FTM_MSG_1(FTM_LOW, "RSB DRx IQ capture - %d", prof_rsb);

  FTM_MSG_2(FTM_LOW, "DRX RSB A - %d, RSB B - %d", rsb_a, rsb_b);
  } // end of cal_params->div_device != 0xFF

  if (prx_status == TRUE && drx_status == TRUE)
  {
    rsp_pkt->cal_status = FTM_INT_CAL_SUCCESS;
    ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_WCDMA);
    if (cal_params->div_device != 0xFF) 
    {
    ftm_rfmode_exit(RFM_DEVICE_1, FTM_STATE_WCDMA); 
    }

    FTM_MSG_1(FTM_HIGH, "RSB Cal is Successful for WCDMA for Band - %d", rf_band);

    return TRUE;
  }

  return FALSE;
}
#endif

void ftm_wcdma_sleep(rfm_device_enum_type device)
{
  rfwcdma_data_debug_flags_type *dbg_flags;
  rf_path_enum_type path; 
  rfm_wcdma_rx_sleep_params rx_sleep_params;
  
  dbg_flags = rfm_wcdma_ftm_get_debug_flags();
  path = rfcommon_core_device_to_path(device);
  if ( dbg_flags->enable_opt_wakeup == TRUE )
  {
    FTM_MSG(FTM_HIGH, "ftm_wcdma_sleep: Optimized sleep routine execution");
    memset(&rx_sleep_params, 0x0, sizeof(rfm_wcdma_rx_sleep_params));
    rx_sleep_params.api_params[device].is_valid = TRUE;
    rx_sleep_params.api_params[device].rxlm_handle = ftm_wcdma_rxlm_buffer[device].buf_idx;

    rfm_wcdma_rx_sleep(&rx_sleep_params, NULL, NULL );

  } /* if ( dbg_flags->enable_opt_wakeup == TRUE ) */
  else
  {
  (void)rfm_sleep(device, NULL, NULL
                  , ftm_wcdma_rxlm_buffer[path].buf_idx
                  );
  } /* if ! ( dbg_flags->enable_opt_wakeup == TRUE ) */

  /* Turn OFF vregs*/
  rfm_power_clock_off();
}

/* api to test the scripts for the events */
boolean ftm_wcdma_enable_scripts(rfm_device_enum_type device, int8 event_id)
{
  /* check if the event is tx or rx based on which we will call ftm_wcdma_rxlm_buffer or ftm_wcdma_txlm_buffer*/
  boolean result = FALSE;
  
  rflm_wcdma_rf_send_script_in_t  in_rx = {0,FALSE,event_id};
  rflm_wcdma_rf_send_script_out_t out_rx;
  rflm_wcdma_rf_send_script_in_t  in_tx = {0,FALSE,event_id};
  rflm_wcdma_rf_send_script_out_t out_tx;
  rflm_wcdma_rf_send_deadline_script_in_t in_d_tx = {0,0,FALSE,event_id};
  rflm_wcdma_rf_send_deadline_script_out_t out_d_tx;

  rflm_err_t ret_err = RFLM_ERR_NONE;

  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "inside ftm_wcdma_enable_scripts: event_id:%d, device:%d", event_id, device);

  /* Add about 2 ms (19200) between Preload and trigger write time */
  in_rx.effect_time = rflm_time_read_ustmr_count() + 38400;
  in_tx.effect_time = rflm_time_read_ustmr_count() + 38400;
  in_d_tx.effect_time = rflm_time_read_ustmr_count() + 38400*2;
  in_d_tx.start_time = rflm_time_read_ustmr_count() + 38400;

  switch(event_id)
  {
    case RF_WCDMA_EVENT_SET_TX_ON:
    case RF_WCDMA_EVENT_SET_TX_OFF:
    case RF_WCDMA_EVENT_ENABLE_DTX:
    case RF_WCDMA_EVENT_DISABLE_DTX:
    ret_err = rflm_wcdma_rf_send_script((uint32)ftm_wcdma_txlm_buffer.buf_idx,
                                          &in_tx,
                                          &out_tx );

      break;

    case RF_WCDMA_EVENT_TX_TUNE:
    ret_err = rflm_wcdma_rf_send_deadline_script((uint32)ftm_wcdma_txlm_buffer.buf_idx,
                                          &in_d_tx,
                                          &out_d_tx );
    break;


    case RF_WCDMA_EVENT_ENABLE_DRX:
    case RF_WCDMA_EVENT_DISABLE_DRX:
    case RF_WCDMA_EVENT_ENABLE_ASDIV:
     ret_err = rflm_wcdma_rf_send_script((uint32)ftm_wcdma_rxlm_buffer[device].buf_idx,
                                             &in_rx,
                                             &out_rx );

    default:
      break;

  } /* switch(timing based on events) */

  if(ret_err != RFLM_ERR_NONE)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_wcdma_enable_scripts: RF Send script failed with error %d", ret_err);
     result = TRUE;

 }  
  return result;
}

void ftm_wcdma_wakeup(rfm_device_enum_type device)
{
  rfwcdma_data_debug_flags_type *dbg_flags;
  rf_path_enum_type path;
  rfm_wcdma_rx_wakeup_params rx_wakeup_params;

  dbg_flags = rfm_wcdma_ftm_get_debug_flags();
  path = rfcommon_core_device_to_path(device);

  /* Turn ON vregs*/
  rfm_power_clock_on();


  if ( dbg_flags->enable_opt_wakeup == TRUE )
  {
     FTM_MSG(FTM_HIGH, " ftm_wcdma_sleep: Optimized wakeup routine execution");

     memset(&rx_wakeup_params, 0x0, sizeof(rfm_wcdma_rx_wakeup_params));
     rx_wakeup_params.api_params[device].is_valid = TRUE;
     rx_wakeup_params.api_params[device].rxlm_handle = 
       ftm_wcdma_rxlm_buffer[device].buf_idx;
      
     rfm_wcdma_rx_prep_wakeup(&rx_wakeup_params, NULL, NULL );
     
     rfm_wcdma_rx_exec_wakeup(&rx_wakeup_params, NULL, NULL );    
 }/* if ( dbg_flags->enable_opt_wakeup == TRUE ) */
  else
  {
   (void)rfm_wakeup(device, RFM_IMT_MODE, NULL, NULL
                    ,ftm_wcdma_rxlm_buffer[path].buf_idx
                    );
  } /* if ! ( dbg_flags->enable_opt_wakeup == TRUE ) */

}

void ftm_wcdma_pretune_to_chan(rfm_device_enum_type device, uint16 chan[])
{
  rf_path_enum_type path = rfcommon_core_device_to_path(device);

  /* Getting Band information from the Uplink channel */
  rfcom_wcdma_band_type band = rfwcdma_core_convert_ul_chan_to_band(chan[0]);

  /* Generating Downlink channel number from Uplink channel and Band information */
  if (band == RFCOM_BAND_IMT)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (band == RFCOM_BAND_800)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_800_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (band == RFCOM_BAND_BC9)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (band == RFCOM_BAND_BC3)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (band == RFCOM_BAND_BC11)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET;
  }

  else if (band == RFCOM_BAND_BC4)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET;
  }

  else if (band == RFCOM_BAND_1900)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (band == RFCOM_BAND_BC8)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET;
  }
  else if (band == RFCOM_BAND_BC19)
  {
    chan[0] = chan[0] + BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET;
  }

  /* Pretune to Channel API is obsolote from Bolt 2.0 */
  //rfm_pretune_to_chan(device, chan, ftm_wcdma_rxlm_buffer[path].buf_idx);
}

/*============================================================================

FUNCTION FTM_WCDMA_GET_LATEST_MULTISYNTH_STATUS

DESCRIPTION
  This API is used for get latest Mutlsynth status by quering RFM layer
  which inturn will query device layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_get_latest_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock
  )
{

#ifndef FEATURE_RF_BOLT_PRE_SILICON
  boolean cur_rx_synth_status = FALSE;
  boolean cur_tx_synth_status = FALSE;
#endif

  *rx_synth_lock = FTM_SYNTH_STATUS_INVALID;
  *tx_synth_lock = FTM_SYNTH_STATUS_INVALID;

  /* As TX is only on RFM_DEVICE_0, this device can be in RX_TX State. All other devices will never be in RXTX State */
  if (device == RFM_DEVICE_0)
  {
#ifndef FEATURE_RF_BOLT_PRE_SILICON

    switch (rfwcdma_mc_get_path_state_of_device(device))
    {
    case RFWCDMA_MC_STATE_POWERUP_INIT:
    case RFWCDMA_MC_STATE_ENTER_MODE:
    case RFWCDMA_MC_STATE_SLEEP:
      *rx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
      *tx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
      break;

    case RFWCDMA_MC_STATE_RX:
      rfm_get_synth_lock_status(device, RFCOM_WCDMA_MODE,
                                RF_SYNTH_RX, &cur_rx_synth_status
                                );
      *rx_synth_lock = (cur_rx_synth_status == TRUE) ?\
        FTM_SYNTH_STATUS_LOCKED : FTM_SYNTH_STATUS_UNLOCKED;
      *tx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
      break;

    case RFWCDMA_MC_STATE_RXTX:
      rfm_get_synth_lock_status(device, RFCOM_WCDMA_MODE,
                                RF_SYNTH_RX, &cur_rx_synth_status
                                );
      *rx_synth_lock = (cur_rx_synth_status == TRUE) ?\
        FTM_SYNTH_STATUS_LOCKED : FTM_SYNTH_STATUS_UNLOCKED;

      rfm_get_synth_lock_status(device, RFCOM_WCDMA_MODE,
                                RF_SYNTH_TX, &cur_tx_synth_status
                                );
      *tx_synth_lock = (cur_tx_synth_status == TRUE) ?\
        FTM_SYNTH_STATUS_LOCKED : FTM_SYNTH_STATUS_UNLOCKED;
      break;

    default:
      FTM_MSG_1(FTM_HIGH, "Unexpected state %d", rfwcdma_mc_get_path_state_of_device(device));
      break;
    }
#endif
  }

  else if ((device == RFM_DEVICE_1) || (device == RFM_DEVICE_2) || (device == RFM_DEVICE_3))
  {
    *tx_synth_lock = FTM_SYNTH_STATUS_NOT_APPLICABLE;
#ifndef FEATURE_RF_BOLT_PRE_SILICON

    switch (rfwcdma_mc_get_path_state_of_device(device))
    {
    case RFWCDMA_MC_STATE_POWERUP_INIT:
    case RFWCDMA_MC_STATE_ENTER_MODE:
    case RFWCDMA_MC_STATE_SLEEP:
      *rx_synth_lock = FTM_SYNTH_STATUS_DISABLED;
      break;

    case RFWCDMA_MC_STATE_RX:
      rfm_get_synth_lock_status(device, RFCOM_WCDMA_MODE,
                                RF_SYNTH_RX, &cur_rx_synth_status
                                );
      *rx_synth_lock = (cur_rx_synth_status == TRUE) ?\
        FTM_SYNTH_STATUS_LOCKED : FTM_SYNTH_STATUS_UNLOCKED;
      break;

    default:
      FTM_MSG_1(FTM_HIGH, "Undetermined state %d", rfwcdma_mc_get_path_state_of_device(device));
      break;
    }
#endif
  }
  else
  {
    *tx_synth_lock = FTM_SYNTH_STATUS_NOT_APPLICABLE;
    *rx_synth_lock = FTM_SYNTH_STATUS_NOT_APPLICABLE;
    FTM_MSG_1(FTM_ERROR, "Unsupported device %d", device);
  }

  FTM_MSG_3(FTM_MED, "Latest values device %d rx_synth_lock %d tx_synth_lock %d",
            device, *rx_synth_lock, *tx_synth_lock);
}

/*============================================================================

FUNCTION FTM_WCDMA_UPDATE_AGGREATED_MULTISYNTH_STATUS

DESCRIPTION
  This API is used for get aggregated Mutlsynth status by quering the latest
  SYNTH lock status and latching on to UNLOCKED status if
  unlock it detected in any of the queries.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_update_aggregated_multisynth_status(rfm_device_enum_type device)
{
  ftm_multi_synth_lock_status_enum_type rx_synth_lock, tx_synth_lock;

  ftm_wcdma_get_latest_multisynth_status(device, &rx_synth_lock, &tx_synth_lock);

  if (ftm_wcdma_aggregated_multi_synth_lock_status[device].rx_synth_status\
        != FTM_SYNTH_STATUS_UNLOCKED) ftm_wcdma_aggregated_multi_synth_lock_status[device].rx_synth_status = rx_synth_lock;

  if (ftm_wcdma_aggregated_multi_synth_lock_status[device].tx_synth_status\
        != FTM_SYNTH_STATUS_UNLOCKED) ftm_wcdma_aggregated_multi_synth_lock_status[device].tx_synth_status = tx_synth_lock;

  FTM_MSG_3(FTM_MED, "Updated values device %d rx_synth_lock %d tx_synth_lock %d",
            device, ftm_wcdma_aggregated_multi_synth_lock_status[device].\
              rx_synth_status, ftm_wcdma_aggregated_multi_synth_lock_status[device].\
              tx_synth_status);
}
/*============================================================================

FUNCTION FTM_WCDMA_RESET_MULTISYNTH_STATUS

DESCRIPTION
  This API is used to get reset multisynth lock status. This is the only way get the multisynth status out of UNLOCK status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
boolean ftm_wcdma_reset_multisynth_status(rfm_device_enum_type device)
{
  boolean ret_value = FALSE;

  /* Maximum number of devices is RFM_DEVICE_4*/
  if (device < RFM_DEVICE_4)
  {
    ftm_wcdma_aggregated_multi_synth_lock_status[device].rx_synth_status = FTM_SYNTH_STATUS_UNDETERMINED;
    ftm_wcdma_aggregated_multi_synth_lock_status[device].tx_synth_status = FTM_SYNTH_STATUS_UNDETERMINED;
    ret_value = TRUE;
    FTM_MSG_1(FTM_MED, "Reset multisynth status for device %d", device);
  }
  return (ret_value);
}

/*============================================================================

FUNCTION FTM_WCDMA_GET_MULTISYNTH_STATUS

DESCRIPTION
  This API is used to get aggregate multisynth lock value. it also does a latest query before returning the value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/


void ftm_wcdma_get_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock
  )
{
  ftm_wcdma_update_aggregated_multisynth_status(device);
  *rx_synth_lock = ftm_wcdma_aggregated_multi_synth_lock_status[device].rx_synth_status;
  *tx_synth_lock = ftm_wcdma_aggregated_multi_synth_lock_status[device].tx_synth_status;
  FTM_MSG_3(FTM_MED, "Final values device %d rx_synth_lock %d tx_synth_lock %d",
            device, *rx_synth_lock, *tx_synth_lock);
}

/*============================================================================

FUNCTION FTM_RF_MEAS_TX_PARAMS

DESCRIPTION
  This API is used to capture and process Tx IQ samples using FBRx and to calculate
  various Tx parameters (i.e. Tx Power, EVM and ACLR)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_fbrx_meas_tx_params
(
  rfm_device_enum_type device,
  uint16 num_averages,
  uint32 capture_offset,
  uint32 tx_measurement_config,
  ftm_rsp_pkt_type *rsp_pkt
  )
{

  rf_time_tick_type prof_tick;
  rf_time_type prof_time = -1;

  ftm_wcdma_selftest_meas_params_struct params;
  uint8 selftest_error = 0;

  prof_tick = rf_time_get_tick();

  params.device = device;
  params.num_averages = num_averages;
  params.capture_offset = capture_offset;
  params.tx_measurement_config = tx_measurement_config;

  selftest_error = ftm_wcdma_selftest(
                                  params,
                                  rsp_pkt,
                                  &ftm_rf_fbrx_iq_acquired_samples_data,
                                  NULL, // &ftm_selftest_pwr_only,
                                  &ftm_rf_fbrx_meas_tx_params_rsp_pkt,
                                  FALSE);

  if (!selftest_error)
  {
    prof_time = rf_time_get_elapsed( prof_tick, RF_MSEC );
    FTM_MSG_1( FTM_HIGH, "Self_test Time profiling WCDMA  "
                           "ExecTime %d ms", prof_time );
  }
  else
  {
    FTM_MSG( FTM_HIGH, "Self_test failed");
  }

}

/*============================================================================

FUNCTION FTM_RF_MEAS_FBRX_NOISE

DESCRIPTION
  This API is used to setup very low Tx power for given WCDMA band and bandwidth,
  perform FBRx IQ capture, calculate noise binsum powers for ACLR correction
  and process/write corresponding NV items

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_fbrx_meas_noise_params
(
   ftm_mode_id_type curr_tech_band,
   rfcom_device_enum_type device,
   uint16 num_averages,
   uint16 tx_chan,
   ftm_rsp_pkt_type *rsp_pkt
)
{
  uint8 status = FTM_FBRX_TEST_SUCCESS, selftest_flag = 0, device_flag = 0xFF, selftest_api_status = 0;
  uint8 pa_state = 0xFF, rgi = 0xFF, accum_bit_shift_num= 0;
  uint32 pa_bias = 0xFFFFFFFF;
  uint16 band_num = 0;
  boolean api_status = TRUE, nv_valid = FALSE, nv_write_status = FALSE, txagc_or_status = TRUE;
  rfnv_item_id_enum_type nv_item_to_write;
  nv_stat_enum_type nv_write_stat;
  /* FBRx Noise Cal Data for ACLR correction to write into NV*/
  rfcommon_nv_fbrx_noise_data_type fbrx_noise_struct_to_write;
  rfcom_wcdma_band_type rf_band = RFCOM_BAND_IMT; 
  ftm_wcdma_selftest_meas_params_struct meas_params;
  ftm_cal_radio_setup_req_type ftm_req_radio_setup;
  ftm_cal_tx_override_parameter_type ftm_tx_agc_override;
  ftm_rsp_pkt_type rs_resp;
  ftm_rsp_pkt_type st_resp;
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type fbrx_meas_tx_resp;
  ftm_common_power_per_bin_struct_type noise_energy_per_bin;
  boolean tear_down_status = FALSE;

  /* Convert ftm band type to rfcom band type */
  rf_band = ftm_wcdma_convert_phone_mode_to_rfcom_band_type(curr_tech_band); 
  switch (rf_band)
  {
  case RFCOM_BAND_IMT:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_1;
    break;
  case RFCOM_BAND_1900:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_2;
    break;
  case RFCOM_BAND_BC3:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_3;
    break;
  case RFCOM_BAND_BC4:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_4;
    break;
  case RFCOM_BAND_800:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_5;
    break;
  case RFCOM_BAND_BC8:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_8;
    break;
  case RFCOM_BAND_BC9:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_9;
    break;
  case RFCOM_BAND_BC11:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_11;
    break;
  case RFCOM_BAND_BC19:
    ftm_req_radio_setup.rs_param.band = WCDMA_FTM_BAND_19;
    break;
  default:
    band_num = WCDMA_FTM_BAND_1;
    status = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG_1(FTM_ERROR, "Unrecognized WCDMA RFCOM band %d", rf_band);
    break;
  }
  FTM_MSG_5(FTM_HIGH, "Inputs - curr_tech_band %d, device %d, w_rf_band %d, num_averages %d , tx_chan %d", curr_tech_band,device, rf_band, num_averages, tx_chan );

  /* If not in Cal State, return*/
  if (rfm_get_calibration_state() == FALSE)
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_fbrx_meas_noise_params: Not in calibration state!");
    ftm_rf_fbrx_meas_noise_params_rsp_pkt.ftm_error_code = FTM_FBRX_TEST_GENERAL_FAILURE;
    return;
  }
  ftm_wcdma_calv3_set_cal_flag(TRUE);

  /* Check for incorrect band specification */
  if (rf_band >= RFCOM_NUM_WCDMA_BANDS)
  {
    ftm_rf_fbrx_meas_noise_params_rsp_pkt.ftm_error_code = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG(FTM_ERROR, "Invalid WCDMA band, bailing out");
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }

  /* If Channel is invalid, return */
  if (tx_chan == 0)
  {
    ftm_rf_fbrx_meas_noise_params_rsp_pkt.ftm_error_code = FTM_FBRX_TEST_GENERAL_FAILURE;
    FTM_MSG(FTM_ERROR, "ftm_wcdma_fbrx_meas_noise_params: FBRX NOISE MEAS not supported for the Band. Channel is 0.");
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }

  selftest_flag = 1;
  ftm_common_selftest_set_flag( &selftest_flag );

  /* Set ACLR noise correction values to write to NV equal to zero as default */
  memset((void*)&fbrx_noise_struct_to_write, 0, sizeof(rfcommon_nv_fbrx_noise_data_type));
  
  device_flag = 1 << device;

  meas_params.capture_offset = 0;
  meas_params.enable_aclr_meas = TRUE;
  meas_params.enable_evm_meas = FALSE;
  meas_params.enable_tx_pwr_meas = FALSE;
  meas_params.enable_vswr_meas = FALSE;
  //meas_params.enable_vswr_meas_v2 = FALSE;
  meas_params.num_averages = num_averages;
  meas_params.tx_measurement_config = FTM_FBRX_MEASURE_ACLR;
  meas_params.device = RFM_DEVICE_0;

  ftm_req_radio_setup.rs_param.debug_mode_en = TRUE;
  ftm_req_radio_setup.rs_param.flag = 0;
  ftm_req_radio_setup.rs_param.mdevice = device;
  ftm_req_radio_setup.rs_param.sdevice = device+1;
  ftm_req_radio_setup.rs_param.mdevice_state = MASTER_DEVICE_RXTX;
  ftm_req_radio_setup.rs_param.sdevice_state = SLAVE_DEVICE_NO_CHANGE;
  ftm_req_radio_setup.rs_param.tech = FTM_RF_TECH_WCDMA;
  ftm_req_radio_setup.rs_param.num_of_chan = 1;
  ftm_req_radio_setup.rs_param.chan_list[0].channel = (uint32)(tx_chan);
  
  pa_bias = 400;
  pa_state = 0;
  rgi = 10;

  ftm_tx_agc_override.debug_mode_en = TRUE;
  ftm_tx_agc_override.device_mask = device_flag;
  ftm_tx_agc_override.flag = 0;
  ftm_tx_agc_override.tx_param[device].override_type = 1 << TX_OVR_POWER;
  ftm_tx_agc_override.tx_param[device].xpt_mode = 0;
  ftm_tx_agc_override.tx_param[device].power_ovr.power_ovr_type = TX_OVR_POWER_IND_PARAM;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.param_mask = 
    (1 << TX_OVR_RGI ) | (1 << TX_OVR_PA_BIAS) | ( 1 << TX_OVR_PA_STATE ) | ( 1 << TX_OVR_QUISCENT_CURRENT ) | ( 1 << TX_OVR_IQ_GAIN ) | ( 1 << TX_OVR_ENV_SCALE );
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.pa_state.gain_state = pa_state;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.rgi.gain_index = rgi;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.bias.pa_bias = pa_bias;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.iq_gain.ovr_action = FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.env_scale.ovr_action = FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.current.quiscent_current = 0;
  ftm_tx_agc_override.tx_param[device].power_ovr.individual_param.delay.ovr_action = FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT;

  rs_resp = ftm_wcdma_calv3_radio_setup( (void*)&ftm_req_radio_setup );

  if (rs_resp.cmd != FTM_RSP_NO_RSP)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_calv3_radio_setup returned failed response %d", rs_resp.cmd);
    status = FTM_FBRX_TEST_GENERAL_FAILURE; 
    selftest_flag = 0;
    ftm_common_selftest_set_flag( &selftest_flag );
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }

  txagc_or_status = ftm_wcdma_calv3_sweep_tx_override((ftm_cal_tx_override_parameter_type const *)&ftm_tx_agc_override);
  if (txagc_or_status == FALSE)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_calv3_sweep_tx_override returned failed response %d", txagc_or_status);
    status = FTM_FBRX_TEST_GENERAL_FAILURE; 
    selftest_flag = 0;
    ftm_common_selftest_set_flag( &selftest_flag );
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }


  FTM_MSG_2(FTM_MED, "rf_band: %d, tx_chan: %d" , rf_band, tx_chan);

  ftm_rf_fbrx_iq_acquired_samples_data.data_available = FALSE;

  selftest_api_status = ftm_wcdma_selftest(meas_params, &st_resp, &ftm_rf_fbrx_iq_acquired_samples_data, NULL, &fbrx_meas_tx_resp, TRUE);
  if (selftest_api_status != 0)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_selftest returned failed response %d", selftest_api_status);
    status = FTM_FBRX_TEST_GENERAL_FAILURE; 
    selftest_flag = 0;
    ftm_common_selftest_set_flag( &selftest_flag );
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }


  ftm_common_read_curr_pwr_per_aclr_bin(&noise_energy_per_bin);
  accum_bit_shift_num = ftm_common_read_accum_bit_shift_num();
  FTM_MSG_1(FTM_MED, "accum_bit_shift_num: %d", accum_bit_shift_num);
  FTM_MSG_7(FTM_MED, " %d %d %d %d %d %d %d ",
            noise_energy_per_bin.utra1_neg,
            noise_energy_per_bin.utra1_pos,
            noise_energy_per_bin.utra2_neg,
            noise_energy_per_bin.utra2_pos,
            noise_energy_per_bin.eutra_neg,
            noise_energy_per_bin.eutra_pos,
            noise_energy_per_bin.accum_count);
  FTM_MSG_7(FTM_MED, "%d %d %d %d %d %d %d",
            noise_energy_per_bin.utra1_neg << accum_bit_shift_num,
            noise_energy_per_bin.utra1_pos << accum_bit_shift_num,
            noise_energy_per_bin.utra2_neg << accum_bit_shift_num,
            noise_energy_per_bin.utra2_pos << accum_bit_shift_num,
            noise_energy_per_bin.eutra_neg << accum_bit_shift_num,
            noise_energy_per_bin.eutra_pos << accum_bit_shift_num,
            accum_bit_shift_num);

  ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra1_neg = 
      (noise_energy_per_bin.utra1_neg << accum_bit_shift_num)/noise_energy_per_bin.accum_count;
  ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra1_pos = 
      (noise_energy_per_bin.utra1_pos << accum_bit_shift_num)/noise_energy_per_bin.accum_count;
  ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra2_neg = 
      (noise_energy_per_bin.utra2_neg << accum_bit_shift_num)/noise_energy_per_bin.accum_count;
  ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra2_pos = 
      (noise_energy_per_bin.utra2_pos << accum_bit_shift_num)/noise_energy_per_bin.accum_count;
  ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.eutra_neg = 0x0;
  ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.eutra_pos = 0x0;

  tear_down_status = ftm_wcdma_calv3_sweep_tear_down_radio();
  if (tear_down_status == FALSE)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_calv3_sweep_tear_down_radio returned failed response %d", rs_resp.cmd);
    status = FTM_FBRX_TEST_GENERAL_FAILURE; 
    selftest_flag = 0;
    ftm_common_selftest_set_flag( &selftest_flag );
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }

  /* Populate NV structure to write */
  fbrx_noise_struct_to_write.data_valid[0] = TRUE;
  fbrx_noise_struct_to_write.rfcommon_nv_fbrx_noise_binsum[0].utra1_neg = ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra1_neg;
  fbrx_noise_struct_to_write.rfcommon_nv_fbrx_noise_binsum[0].utra1_pos = ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra1_pos;
  fbrx_noise_struct_to_write.rfcommon_nv_fbrx_noise_binsum[0].utra2_neg = ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra2_neg;
  fbrx_noise_struct_to_write.rfcommon_nv_fbrx_noise_binsum[0].utra2_pos = ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.utra2_pos;
  fbrx_noise_struct_to_write.rfcommon_nv_fbrx_noise_binsum[0].eutra_neg = ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.eutra_neg;
  fbrx_noise_struct_to_write.rfcommon_nv_fbrx_noise_binsum[0].eutra_pos = ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results.eutra_pos;

  nv_write_status = ftm_common_selftest_fbrx_noise_floor_nvwrite(FTM_RF_TECH_WCDMA, (uint8)rf_band, &fbrx_noise_struct_to_write);

  if (nv_write_status == FALSE)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_common_selftest_fbrx_noise_floor_nvwrite returned failed response %d", rs_resp.cmd);
    status = FTM_FBRX_TEST_GENERAL_FAILURE; 
    selftest_flag = 0;
    ftm_common_selftest_set_flag( &selftest_flag );
    ftm_wcdma_calv3_set_cal_flag(FALSE);
    return;
  }

  selftest_flag = 0;
  ftm_common_selftest_set_flag( &selftest_flag );
  ftm_wcdma_calv3_set_cal_flag(FALSE);

  // Fill in the constant fields of the response packet. 
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.cmd_code = 75;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.subsys_id = FTM;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_WCDMA_C : FTM_WCDMA_RX_2_C ;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.ftm_rf_cmd = FTM_RF_MEAS_FBRX_NOISE;  

  // Start with sucess flag and change it if we fail any check conditions.
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.ftm_error_code = status;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.curr_tech_band = curr_tech_band;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.device = device;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.num_lte_bw = 1;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.tx_chan[0] = tx_chan;
  ftm_rf_fbrx_meas_noise_params_rsp_pkt.lte_bw[0] = (ftm_lte_bw_type)RFCOM_BW_LTE_INVALID;

  memscpy((void*)&(ftm_rf_fbrx_meas_noise_params_rsp_pkt.fbrx_meas_noise_binsum[0]),
          sizeof(ftm_rf_fbrx_noise_binsum_type), 
          (void*)&(ftm_rf_fbrx_iq_acquired_samples_data.noise_meas_results),
          sizeof(ftm_rf_fbrx_noise_binsum_type)
         );
  // Fill in the constant fields of response packet.Size of the packet will be based on the requested Tx measurement config
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_rf_fbrx_meas_noise_params_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;  
  rsp_pkt->pkt_len = FTM_RF_MEAS_NOISE_PARAMS_PKT_SIZE; 

}

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_TRIGGER

DESCRIPTION
  This command acquires one set of IQ samples and returns key parameters used for
  collecting Rx and TX IQ samples. The samples collected in one acquisition will be
  contiguous. Samples from different acquisitions are not time coherent.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_wcdma_fbrx_iq_capture_trigger
(
  rfm_device_enum_type device,
  uint8 sample_size,
  uint8 fbrx_chain,
  ftm_rsp_pkt_type *rsp_pkt
  )
{
  ftm_fbrx_test_error_code_type error_code = FTM_FBRX_TEST_SUCCESS;

  boolean capture_done;

  uint16 sample_capture_size = 9216;
  uint32 samp_rate = -1;
  ftm_common_selftest_fbrxgs_info_type fbrx_gs_info;

  ftm_rf_fbrx_iq_acquired_samples_data.data_available = FALSE;
  ftm_rf_fbrx_iq_acquired_samples_data.device = device;
  fbrx_gs_info.override_enable = FALSE;

  switch (sample_size)
  {
  case FTM_FBRX_SAMPLE_SIZE_1024:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 1024; break;
  case FTM_FBRX_SAMPLE_SIZE_2048:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 2048; break;
  case FTM_FBRX_SAMPLE_SIZE_3072:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 3072; break;
  case FTM_FBRX_SAMPLE_SIZE_4096:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 4096; break;
  case FTM_FBRX_SAMPLE_SIZE_5120:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 5120; break;
  case FTM_FBRX_SAMPLE_SIZE_6144:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 6144; break;
  case FTM_FBRX_SAMPLE_SIZE_7168:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 7168; break;
  case FTM_FBRX_SAMPLE_SIZE_8192:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 8192; break;
  case FTM_FBRX_SAMPLE_SIZE_9216:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 9216; break;
  default:
    ftm_rf_fbrx_iq_acquired_samples_data.req_samples = 9216;
    FTM_MSG_ERROR("Number of samples requested %d > Max. value %d", ftm_rf_fbrx_iq_acquired_samples_data.req_samples, FTM_WCDMA_FBRX_IQ_SAMPLES_MAX, 0);
    error_code = FTM_FBRX_TEST_IQ_SAMPLES_EXCEEDS_CAPTURE_LIMIT;
    FTM_MSG_1(FTM_ERROR, "Number of samples requested not supported", sample_size);
    break;
  }

  if (ftm_get_current_state(device) != FTM_STATE_WCDMA)
  {
    capture_done = FALSE;
  }
  else
  {
    /* Trigger I/Q capture*/
    capture_done = ftm_common_fbrx_iq_capture_processing(
      FTM_RF_TECH_WCDMA,
      fbrx_chain,
      FALSE,                                                      /* proc_flag */
      sample_capture_size,
      &(ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx),
      &(ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx),
      NULL,
      NULL,
      &samp_rate,
      &fbrx_gs_info);

  }

  if (capture_done == FALSE)
  {
    error_code = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
  }
  else
  {
    ftm_rf_fbrx_iq_acquired_samples_data.data_available = TRUE;
  }

  /***************** Fill up RSP packet with standard data *****************/

  // Fill in the constant fields of the response packet. Sample buffer will be filled after sanity check.
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.cmd_code = 75;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.subsys_id = FTM;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_WCDMA_C : FTM_WCDMA_RX_2_C;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.ftm_rf_cmd = FTM_FBRX_IQ_CAPTURE_TRIGGER;

  // Update the final error code in the response packet.
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.ftm_error_code = error_code;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sample_data_format_tx = FTM_WCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sample_data_format_rx = FTM_WCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sample_size = sample_size;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.fbrx_chain = fbrx_chain;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.sampling_frequency = FTM_WCDMA_FBRX_IQ_SAMPLE_FREQ;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.max_diag_sample_size_tx = FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX;
  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt.max_diag_sample_size_rx = FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX;

  // Fill in the constant fields of the response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_rf_fbrx_iq_capture_trigger_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;
  rsp_pkt->pkt_len =  sizeof(ftm_rf_fbrx_iq_capture_trigger_rsp_pkt);

  FTM_MSG_3(FTM_MED, "Inputs to IQ Capture trigger -> device: %d :sample_size : %d : fbrx_chain : %d",
            device, sample_size, fbrx_chain);
}

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get Tx and/or Rx IQ samples for WCDMA from buffer that has
  been used to capture samples in ftm_wcdma_fbrx_iq_capture_trigger().

DEPENDENCIES
 FTM_FBRX_IQ_CAPTURE_TRIGGER command should have been issued before executing get
 samples command.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_fbrx_iq_capture_get_samples
(
  rfm_device_enum_type device,
  uint8 sample_source,
  uint32 sample_offset,
  uint32 sample_size,
  ftm_rsp_pkt_type *rsp_pkt
  )
{
  uint8 status = FTM_FBRX_TEST_SUCCESS;

  uint16 sample_buffer_size;
  uint16 data_byte_size;
  uint16 data_byte_offset;

  int32 *iq_buf_rx_ptr = NULL;
  int32 *iq_buf_tx_ptr = NULL;

  /***************** Do sanity checks *******************************/
  /*Check if IQ has been acquired for the same device*/
  if (ftm_rf_fbrx_iq_acquired_samples_data.device != device)
  {
    FTM_MSG(FTM_ERROR, "IQ samples are not acquired for this device, please acquire them first");
    status = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
  }
  else
  {
    /*Check if data is available */
    if (ftm_rf_fbrx_iq_acquired_samples_data.data_available == FALSE)
    {
      FTM_MSG(FTM_ERROR, "IQ samples are not acquired, please acquire them first");
      status = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
    }
    else
    {
      /* Check if number of samples requested is greater than acquired.*/
      if ((sample_offset + sample_size) > ftm_rf_fbrx_iq_acquired_samples_data.req_samples)
      {
        FTM_MSG_2(FTM_ERROR, "Sample offset(%d) or Sample size(%d) is wrong", sample_offset, sample_size);
        status =  FTM_FBRX_TEST_ILLEGAL_COMBIMATION;
      }
      else
      {
        /*Check if the requested sample source is correct*/
        if ((sample_source != FTM_FBRX_IQ_SAMPLE_TX)
            && (sample_source != FTM_FBRX_IQ_SAMPLE_RX))
        {
          FTM_MSG_1(FTM_ERROR, "Requested sample source is not valid", sample_source);
          status =  FTM_FBRX_TEST_GENERAL_FAILURE;
        }
        else
        {
          /*check Tx request for max sample size and memory alignment requirements*/
          if (sample_source == FTM_FBRX_IQ_SAMPLE_TX)
          {
            if (sample_size > FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX)
            {
              FTM_MSG_3(FTM_ERROR, "Requested Sample size %d is greater than %d for sample source %d",
                        sample_size,
                        FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX,
                        sample_source);

              status =  FTM_FBRX_TEST_GENERAL_FAILURE;
            }
            if (sample_size & 0x01)
            {
              FTM_MSG_2(FTM_ERROR, "Requested Sample size %d has to be an even number for source %d",
                        sample_size,
                        sample_source);
              status =  FTM_FBRX_TEST_GENERAL_FAILURE;
            }
          }
          /*check Rx request for max sample size and memory alignment requirements*/
          else if (sample_source == FTM_FBRX_IQ_SAMPLE_RX)
          {
            if (sample_size > FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX)
            {
              FTM_MSG_3(FTM_ERROR, "Requested Sample size %d is greater than %d for sample source %d",
                        sample_size,
                        FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX,
                        sample_source);
              status = FTM_FBRX_TEST_GENERAL_FAILURE;
            }
            if (sample_size & 0x07)
            {
              FTM_MSG_2(FTM_ERROR, "Requested Sample size %d has to be multiple of 8 for source %d",
                        sample_size,
                        sample_source);
              status = FTM_FBRX_TEST_GENERAL_FAILURE;
            }
          }
          else
          {
            FTM_MSG_1(FTM_ERROR, "Requested Sample type is not suuported %d", sample_source);
            status = FTM_FBRX_TEST_ILLEGAL_COMBIMATION;
          }

          /*if all sanity check passed, go ahead witrh sample dispatch*/
          if (status == FTM_FBRX_TEST_SUCCESS)
          {
            if (sample_source == FTM_FBRX_IQ_SAMPLE_TX)
            {
              data_byte_offset = sample_offset * 2;
              iq_buf_tx_ptr = ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx + (data_byte_offset / sizeof(int32));

              /* Buffer for samples and FFT - 8 bit I/8 bit Q*/
              sample_buffer_size = (sample_size * 2);

              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if (ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data == NULL)
              {
                ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data = (int32 *)ftm_malloc(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
              }

              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if (ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data == NULL)
              {
                status = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;
                FTM_MSG(FTM_ERROR, "Unable to allocate memory for Tx - IQ buffer");
              }
              else
              {
                ftm_rf_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes = sample_buffer_size;

                /*get sample_size samples from the Rx buffer*/
                memscpy((void *)ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data, 
                         sample_buffer_size, 
                         (void *)iq_buf_tx_ptr, 
                         sample_buffer_size);

                /* Parse the Tx data*/
                ftm_common_parse_tx_IQ(FTM_STATE_WCDMA, ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data, sample_size);
              }
            }
            else if (sample_source == FTM_FBRX_IQ_SAMPLE_RX)
            {
              data_byte_offset = (sample_offset * 20) / 8;
              data_byte_size   = (sample_size * 20) / 8;
              iq_buf_rx_ptr = ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx + (data_byte_offset / sizeof(int32));

              /* Buffer for samples and FFT - 32 bit I/32 bit Q*/
              sample_buffer_size = (sample_size * 2 * 4);

              if (ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data == NULL)
              {
                ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data = (int32 *)ftm_malloc(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
              }

              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if (ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data == NULL)
              {
                status = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;
                FTM_MSG(FTM_ERROR, "Unable to allocate memory for Rx - IQ buffer");
              }
              else
              {
                ftm_rf_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes = sample_buffer_size;

                /*get sample_size samples from the Rx buffer*/
                memscpy((void *)ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data, 
                         data_byte_size, 
                         (void *)iq_buf_rx_ptr, 
                         data_byte_size);

                /* Parse the data*/
                ftm_common_parse_PCFL20_to_IQ(FTM_STATE_WCDMA, ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data, sample_size);
              }
            }
            else
            {
              status = FTM_FBRX_TEST_ILLEGAL_COMBIMATION;
            }
          }
        }
      }
    }
  }

  /**************** Fill the packet after check ************* */
  if (rsp_pkt != NULL)
  {
    /***************** Fill up RSP packet with standard data *****************/

    // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.cmd_code = 75;
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.subsys_id = FTM;
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_WCDMA_C : FTM_WCDMA_RX_2_C;
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.ftm_rf_cmd = FTM_FBRX_IQ_CAPTURE_GET_SAMPLES;

    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.ftm_error_code = status;
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_source = sample_source;
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_offset = sample_offset;
    ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_size = sample_size;

    // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
    rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
    rsp_pkt->pkt_payload = &ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt;
    rsp_pkt->delete_payload = FALSE;
    /*******Fill up remaining part of RSP packet based on sanity checks and acquired data***********/
    if (status == FTM_FBRX_TEST_SUCCESS)
    {
      // Since all the error checks were a success, copy the IQ data into diag command buffer
      // Extract the I and Q information from acquired IQ buffer [sample_offset,sample_offset+sample_number) and copy it to diag in the format IQIQIQ
      if (sample_source == FTM_FBRX_IQ_SAMPLE_TX)
      {

        memscpy(ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_8_bit,
                ftm_rf_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes,
               ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data,
               ftm_rf_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes);

        // Header size +   (I + Q , each 1 byte) * total number of samples
        rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE + ftm_rf_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes;
        FTM_MSG_3(FTM_MED, "%d CDMA IQ samples (Source = %d ) sent to Diag with sample offset %d",
                  sample_size,
                  sample_source,
                  sample_offset);
      }
      if (sample_source == FTM_FBRX_IQ_SAMPLE_RX)
      {

        memscpy(ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_32_bit,
                ftm_rf_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes,
               ftm_rf_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data,
               ftm_rf_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes);

        // Header size +   (I + Q , each 2 byte) * total number of samples
        rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE + ftm_rf_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes;
        FTM_MSG_3(FTM_MED, "%d CDMA IQ samples (Source = %d ) sent to Diag with sample offset %d",
                  sample_size,
                  sample_source,
                  sample_offset);
      }
    }
    // If we fail checks, dont fill IQ samples. RSP packet will only have header.
    else
    {
      rsp_pkt->pkt_len =  FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE;
      FTM_MSG_2(FTM_ERROR, "CDMA FBRx get IQ samples failed for SAMPLE SIZE %d and SAMPLE OFFSET %d",
                sample_size,
                sample_offset);
    }
  }
}

/*============================================================================

FUNCTION ftm_wcdma_get_multiple_rx_level_dbm

DESCRIPTION
  This API returns the Rx level, for all the requested receiver chains associated
  with one carrier, in both Rx AGC and dBm units. This command is only valid after
  the mode (Technology/Band) is set using FTM_SET_MODE and FTM_SET_CALIBRATION_STATE
  has been used to deactivate calibration state or calibration state has not been 
  activated while the device was powered up. Also, its required to enable all 
  receiver chains and program them to correct LNA state  before requesting RxAGC 
  for a Rx chain (e.g. secondary chain should be enabled using FTM_SET_SECODNARY_CHAIN 
  before using this command).

DEPENDENCIES
  None
 
RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_get_multiple_rx_level_dbm
(
   rfm_device_enum_type device,
   uint16 select_rx_chain,
   ftm_rsp_pkt_type *rsp_pkt
)
{
  uint8 i;
  uint8 number_selected_rx_chains = 0;
  uint16 status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
  /* Get Rx Level DBM data */
  int16 ftm_wcdma_rx_level_agc[FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER] ={0};
  int16 ftm_wcdma_rx_level_dbm10[FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER] = {0};   
  uint16 payload_size = 0;  

  for (i=0;i<FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER;i++)
  {    
    if ((select_rx_chain >> i) & (uint16)0x1)
    {
      if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
      {
        switch(i)
        {
           case 0:
             /* Call RFM Function to query RxAGC Data for Primary Chain */
             ftm_wcdma_rx_level_agc[number_selected_rx_chains] = rfm_get_rx_raw_agc_val(RFM_DEVICE_0); // Primary Rx Chain
             ftm_wcdma_rx_level_dbm10[number_selected_rx_chains] = ( RF_MIN_RX_POWER_DBM*RF_TXAGC_RESOLUTION  + ( ftm_wcdma_rx_level_agc[number_selected_rx_chains] - RFAGC_MIN_AGC_VAL  ) );
             FTM_MSG_2(FTM_MED, "FTM GET_MULTIPLE_RX_LEVEL_DBM Primary Chain Rx AGC = %d : Rx Power dbm10 = %d",ftm_wcdma_rx_level_agc[number_selected_rx_chains],
                                                                                                                ftm_wcdma_rx_level_dbm10[number_selected_rx_chains]);
             status &= FTM_RX_LEVEL_DBM_TEST_SUCCESS;
             break;
           case 1:
             /* Call RFM Function to query RxAGC Data for Secondary Chain if Secondary chain is  enabled */
             if (ftm_wcdma_div_status[RFM_DEVICE_1] == TRUE)
             {                             
               ftm_wcdma_rx_level_agc[number_selected_rx_chains] = rfm_get_rx_raw_agc_val(RFM_DEVICE_1); // Diversity Chain associated with Primary chain
               ftm_wcdma_rx_level_dbm10[number_selected_rx_chains] = ( RF_MIN_RX_POWER_DBM*RF_TXAGC_RESOLUTION  + ( ftm_wcdma_rx_level_agc[number_selected_rx_chains] - RFAGC_MIN_AGC_VAL  ) );
               FTM_MSG_2(FTM_MED, "FTM GET_MULTIPLE_RX_LEVEL_DBM - Secondary Chain Rx AGC = %d : Rx Power dbm10 = %d",ftm_wcdma_rx_level_agc[number_selected_rx_chains],
                                                                                                                      ftm_wcdma_rx_level_dbm10[number_selected_rx_chains]);
               status &= FTM_RX_LEVEL_DBM_TEST_SUCCESS;
             }
             else
             {            
               status &= FTM_RX_LEVEL_DBM_TEST_ATLEAST_ONE_RECEIVER_BAD_STATE;
               FTM_MSG(FTM_ERROR, "FTM_GET_MULTIPLE_RX_LEVEL_DBM Secondary chain not enabled");   
               ftm_wcdma_rx_level_agc[number_selected_rx_chains] = -512;
               ftm_wcdma_rx_level_dbm10[number_selected_rx_chains] = ( RF_MIN_RX_POWER_DBM*RF_TXAGC_RESOLUTION  + ( ftm_wcdma_rx_level_agc[number_selected_rx_chains] - RFAGC_MIN_AGC_VAL  ) );
             }
             break;
           default:
             status &= FTM_RX_LEVEL_DBM_TEST_INVALID_RX_CHAIN;
             FTM_MSG_1(FTM_ERROR, "FTM_GET_MULTIPLE_RX_LEVEL_DBM - Invalid receiver type selected (select_rx_chain - %4X)", select_rx_chain);   
             ftm_wcdma_rx_level_agc[number_selected_rx_chains] = ftm_wcdma_rx_level_dbm10[number_selected_rx_chains] = 0;
             break;
        }
      }
      number_selected_rx_chains++;
    }
  }  

  FTM_MSG(FTM_MED, "Preparing for the response packet - FTM_GET_MULTIPLE_RX_LEVEL_DBM");  
  
  /***************** Fill up RSP packet with standard data *****************/    
  // Fill in the constant fields of the response packet. Sample buffer will be filled after sanity check.
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.cmd_code = 75;
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.subsys_id = FTM;
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.subsys_cmd_code = (device == RFM_DEVICE_0) ? FTM_WCDMA_C : FTM_WCDMA_RX_2_C ;
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.ftm_rf_cmd = FTM_GET_MULTIPLE_RX_LEVEL_DBM;  

  // Start with sucess flag and change it if we fail any check conditions.
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.select_rx_chain = select_rx_chain;
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.ftm_error_code = status;
    
  payload_size = number_selected_rx_chains*(sizeof(ftm_wcdma_rx_level_agc[0]) + sizeof(ftm_wcdma_rx_level_dbm10[0]));
  ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.payload_size = payload_size;
     
  for (i=0; i<number_selected_rx_chains; i++)
  {
    if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
    {
      ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i] = ftm_wcdma_rx_level_agc[i];
      ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i + 1] = ftm_wcdma_rx_level_dbm10[i];
    }
    else
    {
      ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i] = 0; // Zero indicating Error state
      ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i + 1] = 0; // Zero indicating Error state
    }
  }
     
  // Fill in the constant fields of the response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_wcdma_get_multiple_rx_level_dbm_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;
  rsp_pkt->pkt_len =  6 + 2*sizeof(uint16) + sizeof(int16) + payload_size;
}

/*============================================================================

FUNCTION 
ftm_wcdma_feedback_setup

DESCRIPTION
This API will be called to configure fbrx for tx internal calibration and 
self test 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void ftm_wcdma_feedback_setup(rfm_device_enum_type primary_device,
                              rfm_device_enum_type feedback_device,
                              uint16 feedback_setup_action,
                              uint8 fb_gain_state,
                              boolean wtr_enable_mode,
                              boolean xpt_rxlm_enable,
                              boolean fb_rxlm_enable)
{
  const rfc_common_logical_device_params_type *cmn_logical_dev = NULL;
  rfdevice_rxtx_common_class* device_ptr = NULL;

  if (feedback_setup_action &
        FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR)
  {
    /* Set Use feedback path to TRUE during feedback path setup
    so that we configure the FB path RxLM instead of default
    Rx chain */
    if (wtr_enable_mode == TRUE)
    {
      rfwcdma_mc_set_use_fb_path_flag(TRUE);
    }
    else
    {
      rfwcdma_mc_set_use_fb_path_flag(FALSE);
    }
    /* Get pointer to the common logical device */
    cmn_logical_dev = rfc_common_get_logical_device_params(primary_device);
    rfdevice_wcdma_tx_get_common_dev_ptr(RFM_DEVICE_0, ftm_curr_wcdma_mode);

    if (NULL != cmn_logical_dev)
    {
      rfdevice_cmn_set_et_path(cmn_logical_dev->cmn_device,
                               RFM_IMT_MODE,
                               fb_gain_state,
                               wtr_enable_mode);
    }
    else
    {
      FTM_MSG(FTM_ERROR, "Could not get logical device");
    }

    if (NULL != device_ptr)
    {
      if (wtr_enable_mode == TRUE)
      {
        rfcommon_fbrx_mc_enable_rf_dev(device_ptr, fb_gain_state);
      }
      else
      {
        rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
      }
    }
    else
    {
      FTM_MSG(FTM_ERROR, "ftm_wcdma_feedback_setup: Could not get RFC rxtx common device");
    }
  }

  if (feedback_setup_action &
        FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_XPT)
  {
    FTM_MSG(FTM_ERROR, "FEEDBACK_SETUP_ACTION CONFIGURE_RXLM_FOR_XPT not "\
              "supported in Tech: WCDMA");
  }

  if (feedback_setup_action &
        FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC)
  {
     rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

     lm_handle_type fbrx_lm_handle;

     fbrx_lm_handle = rfcommon_fbrx_mc_get_tech_handle( RFM_IMT_MODE );

     ret_value = (rfcommon_fbrx_error_type)rfcommon_fbrx_msm_update_static_settings(fbrx_lm_handle, RFCOMMON_FBRX_UMTS_SC);

     if (ret_value == RFCOMMON_FBRX_SUCCESS) 
     {
        ret_value = rfcommon_fbrx_enable( fbrx_lm_handle );

        if (ret_value == RFCOMMON_FBRX_SUCCESS) {
           FTM_MSG(FTM_LOW,"FEEDBACK_SETUP_ACTION re-enabled FBRX");
        }
     }
  }
}

/*!
  @brief
  Inidates if RX cal optimization is enabled

  @details
  This function shows if RX CAL optimization is enabled
 
  @return
  boolean rx_cal_optimization
  
*/
boolean ftm_wcdma_rxcal_opt_enabled(void)
{
  return rx_cal_optimization;
}


#ifdef FEATURE_RF_HAS_QTUNER
/*============================================================================

FUNCTION 
ftm_wcdma_program_qfe_scenario

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

void ftm_wcdma_program_qfe_scenario(void)
{
  boolean status = FALSE;

  status = rfwcdma_mc_program_qfe_scenario();

  if (!status)
  {
    MSG_ERROR("FTM_WCDMA_PROGRAM_QFE_SCENARIO: Failed ", 0, 0, 0);
  }

}
#endif /* FEATURE_RF_HAS_QTUNER */

/*============================================================================

FUNCTION 
ftm_wcdma_tune_to_chan_scripts

DESCRIPTION
This routine acts as entry point for FTM WCDMA calls to rfm_wcdma APIS for tune. 
It will create and populate the rfm interface variables for tune. 
FTM will always exercise script based tuning. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void ftm_wcdma_tune_to_chan_scripts(uint8 device_mask,uint16 chan[RFCOM_MAX_CARRIERS], 
                                    uint32 carrier_mask)
{
  rfm_wcdma_rx_tune_params tune_params;
  boolean txagc_is_disabled= FALSE;
  rfm_device_enum_type device =0; 
  uint8 num_devices =0;
  rfwcdma_data_debug_flags_type *dbg_flags;
  rf_path_enum_type path = RF_PATH_0;
  rfcom_wcdma_band_type band = RFCOM_BAND_IMT;

  dbg_flags = rfm_wcdma_ftm_get_debug_flags();

  memset(&tune_params, 0, sizeof(rfm_wcdma_rx_tune_params)); 
  
  num_devices = device_mask;

  dbg_flags->enable_script_based_tune = TRUE; 

  while (num_devices != 0)
  {
    device = (rfm_device_enum_type)rf_extract_first_one_bit_lsb_location(num_devices);

    if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
    {
      RF_MSG_1(RF_ERROR, "ftm_wcdma_tune_to_chan_scripts: Invalid device %d", device);
      break;
    }

    path = rfcommon_core_device_to_path(device);
    band = rfwcdma_core_convert_chan_to_band(chan[0]);

    /* Check the BC config if band is supported for this path */
    if ( (rfwcdma_core_is_band_supported(band,path)) == FALSE )
    {
       RF_MSG_2(RF_ERROR, "BC Config not supported for device %d Band %d", 
                device, band);

       /* If this path is not supported for this band, move onto the next valid path*/
       tune_params.api_params[device].is_valid = FALSE;
       num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
       continue;
    }

    /* Populate the interface structure for tune */
    tune_params.api_params[device].is_valid = TRUE;
    tune_params.api_params[device].rxlm_handle = ftm_wcdma_rxlm_buffer[device].buf_idx;
    tune_params.api_params[device].carrier_mask = carrier_mask;
    tune_params.api_params[device].channel[0] = chan[0];
    tune_params.api_params[device].channel[1] = chan[1];
    tune_params.api_params[device].channel[2] = chan[2];

    num_devices &= (~rf_extract_first_one_bit_mask(num_devices));
  }
  
  /*In CCS we have a memory overlay between TxAGC and Tune events.
    Hence we need to stop TxAGC before we call tune */
  if(( ftm_get_mode() == FTM_MODE )&&(rfm_get_calibration_state() == FALSE))
  {
    (void)rfwcdma_core_tx_disable_txagc((rflm_dm_handle_id_t)ftm_wcdma_txlm_buffer.buf_idx,TRUE);
    txagc_is_disabled =TRUE; 
  }

  /* Call to FR17754 based APIs for script based tuning */
  rfm_wcdma_rx_prep_tune(&tune_params, NULL, NULL);
  rfm_wcdma_rx_exec_tune(&tune_params, NULL, NULL);

  if(( ftm_get_mode() == FTM_MODE )&&(rfm_get_calibration_state() == FALSE))
  {
    /* Based on observations, warmup time sent to FW is about 550us. Add wait for RxAGC to settle */
    DALSYS_BusyWait(1500);
    /* Restart TxAGC if stopped*/
    if(txagc_is_disabled)
    {
      (void)rfwcdma_core_tx_disable_txagc((rflm_dm_handle_id_t)ftm_wcdma_txlm_buffer.buf_idx,FALSE);
    }
  }

  FTM_MSG_2(FTM_HIGH,"ftm_wcdma_tune_to_chan_scripts: Script based tune executed"
                     " device_mask :%d, carrier mask:%d",
                     device_mask, carrier_mask); 
}/* ftm_wcdma_tune_to_chan_scripts */

/*!
  @brief
  Return the PA switchpoints for given tx device and band
 
  @param carrier_idx: carrier index
  @param rfcom_band: band enum 
  @param pa_rise_swpts: pointer to an array for PA rise switchpoints 
  @param pa_fall_swpts: pointer to an array for PA fall switchpoints
  @param active_pa_states: number of active PA states
  @retval TRUE if PA switchpoints are populated in NV, FALSE if PA switchpoints are not populated in NV 
*/

boolean ftm_wcdma_get_pa_switchpoints
(
  uint8 carrier_idx,
  rfcom_wcdma_band_type rf_band,
  int16* pa_rise_swpts,
  int16* pa_fall_swpts,
  int16 *active_pa_states
)
{
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  boolean ret_api_status = TRUE;
  
  /*Get nv_tbl_ptr for the specific band*/
  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode));

  if (nv_tbl == NULL)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_xpt_iq_capture: Failed to get valid static "
              "NV pointer for band %d", ftm_curr_wcdma_mode);
    ret_api_status &= FALSE;
  }

  if (ret_api_status)
  {
    /*Get number of active pa states. If 0 then PA switchpoints are not populated in NV*/
    *active_pa_states = rfwcdma_core_txlin_get_num_valid_pa_state(rf_band);
    if ((*active_pa_states) != 0)
    {
      /*Copy PA Rise/Fall switchpoints for specified carrier*/
      switch (carrier_idx)
      {
      case RFCOM_SINGLE_CARRIER:
      case RFCOM_DUAL_CARRIER: // DC Tx is using the SC PA switch points
         memscpy(pa_rise_swpts,RFCOMMON_NV_MAX_PA_STATE*sizeof(int16), 
              &(nv_tbl->pa_static.pa_rise_sw), RFCOMMON_NV_MAX_PA_STATE*sizeof(int16));
         memscpy(pa_fall_swpts,RFCOMMON_NV_MAX_PA_STATE*sizeof(int16), 
              &(nv_tbl->pa_static.pa_fall_sw), RFCOMMON_NV_MAX_PA_STATE*sizeof(int16));
         ret_api_status = TRUE; 
         break;

      default:
         FTM_MSG_ERROR("ERROR: Received bad carrier index %d",carrier_idx,0,0);
         ret_api_status = FALSE;
         break;
      }/* switch (carrier_idx) */
    }/* if ((*active_pa_states) != 0) */
  }/* if (ret_api_status) */
 
  return ret_api_status;
}/* ftm_wcdma_get_pa_switchpoints */

/*!
  @brief
  Return the current FTM_RF test scenario set by the tool
 
  @retval
  */

uint32 ftm_wcdma_get_current_test_scenario()
{
  uint32 ftm_wcdma_test_scenario = 0;

  ftm_wcdma_test_scenario = ftm_rf_current_test_scenario & 1;

  FTM_MSG_2(FTM_HIGH,"ftm_wcdma_get_current_test_scenario: Tool set scenario: %d, WCDMA scenario: %d",
                              ftm_rf_current_test_scenario, ftm_wcdma_test_scenario );

  return ftm_rf_current_test_scenario;
}/* ftm_wcdma_get_current_test_scenario */

/*------------------------------------------------------------------------------*/
/*!
  @brief
  Execute temp comp in FTM RF mode
 
  @details
  This function is called FTM task every 10 sec
  FTM task here is acting as L1 in FTM RF mode
  This function calls prep_temp_comp as opposed to legacy way of calling do_temp_comp

  @param
  None
 
  @retval
  None
*/
void ftm_wcdma_execute_temp_comp(void)
{
   rfm_wcdma_temp_comp_param tc_param; /* Param input to prep_temp_comp */
   void* user_data = NULL;             /* User data input to prep_temp_comp - Not used */
   rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE;

   /* Sanity: Check if currently all following are true
                - is in FTM mode
                - cal state is NOT set
                - in RxTx state
              Otherwise temp_comp should not be triggered
   */
   if( ( ftm_get_mode() == FTM_MODE )&& 
       (rfm_get_calibration_state() == FALSE) &&
       (rfwcdma_mc_state.car_path_state[0][0].rf_state == RFWCDMA_MC_STATE_RXTX)
     )
   {
      /* Get Tx device */
      tc_param.device = rfwcdma_mc_state.car_path_state[0][0].tx_rfm_device;

      /* Call prep_temp_comp */
      ret_val = rfm_wcdma_prep_temp_comp(&tc_param,
                                         ftm_wcdma_execute_temp_comp_cb,
                                         user_data
                                        );
      /* Check if API went through fine */
      if (ret_val != RFM_WCDMA_HEALTHY_STATE)
      {
         MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,
               "ftm_wcdma_execute_temp_comp: rfm_wcdma_prep_temp_comp failed. Error %d",
               ret_val);
      }
  }

} /* ftm_wcdma_execute_temp_comp */


/*------------------------------------------------------------------------------*/
/*!
  @brief
  CB function for Execute temp comp in FTM RF mode
 
  @details
  This function is called upon completion of ftm_wcdma_execute_temp_comp
  This function calls rfm_wcdma_exec_temp_comp to complete remaining temp_comp operations

  @param temp_comp_status
  Temp comp status from ftm_wcdma_execute_temp_comp calling rfm_wcdma_prep_temp_comp

  @param user_data
  Pointer to user data. This is not used here.
 
  @retval
  None
*/
void ftm_wcdma_execute_temp_comp_cb(rfm_cb_event_enum_type temp_comp_status, void* user_data)
{
   rfm_wcdma_temp_comp_param tc_param;  /* Param input to exec_temp_comp */
   rfm_cb_handler_type callback = NULL; /* Callback function input to exec_temp_comp - Not needed here*/
   rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE;

   /* Sanity check on temp_comp_status */
   if(temp_comp_status != RFM_WCDMA_TEMP_COMP_PREP_COMPLETE)
   {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,
            "ftm_wcdma_execute_temp_comp_cb: prep_temp_comp failed with error %d",
            temp_comp_status);
      return;
   }

   /* Get Tx device */
   tc_param.device = rfwcdma_mc_state.car_path_state[0][0].tx_rfm_device;

   /* Call rfm_wcdma_exec_temp_comp */
   ret_val = rfm_wcdma_exec_temp_comp(&tc_param, callback, user_data);

   /* Check if API went through fine */
   if (ret_val != RFM_WCDMA_HEALTHY_STATE)
   {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,
            "ftm_wcdma_execute_temp_comp_cb: rfm_wcdma_exec_temp_comp failed. Error %d",
            ret_val);
   }

} /* ftm_wcdma_execute_temp_comp_cb */


#endif /* FTM_HAS_UMTS */
#endif /* FEATURE_FACTORY_TESTMODE */
