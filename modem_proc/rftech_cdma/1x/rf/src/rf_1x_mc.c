/*!
  @file
  rf_1x_mc.c
 
  @brief
  RF Driver's 1x MC interface file.

  @details
  This file defines the RFM 1X Main Control interfaces.
 
  @addtogroup RF_CDMA_1X_MC
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_mc.c#1 $
  
when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      1X2G QTA 
02/23/16   vr      Fix calibration crash
02/23/16   vr      Allow Skip-pin only if Apin and PAR support are enabled
12/20/15   vr      Autopin check-in
12/09/15   rs      KW error fix
11/27/15   rs      Added NV control support for FR25072
11/23/15   rs      WTR power/up power down support
11/17/15   vr      Added dynamic txr_iq_dac setting update support
12/15/14   vc      Abort ASDiv events through CDMA MDSP layer
12/15/14   vc      Tx spectral inversion changes
12/15/14   vr      Fix to use correct rx_device based on band
11/28/14   vr      Added checks for FBRx NV support
11/18/14   pk      Calling rflm_c2k_abort_asd in sleep to abort the pending 
                   AsDiv events
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/03/14   sty     Moved fbrx alloc/dealloc from enter_mode/exit_mode to 
                   wakeup_tx / sleep_tx
10/06/14   zhw     Call wtr sleep script before rx_stop during retune
10/01/14   zhw     Skip HDET temp param when HDET start fails
09/26/14   JJ      Move rfcommon_fbrx_mc_tune_to_chan before config tx agc 
09/03/14   spa     Move configure_tx_on_off under critical section
08/22/14   spa     Add handle as argument for 'rx_allocate resources' API
08/20/14   spa     DO NOT deallocate RFLM resources when in prep rx state
07/29/14   spa     Send in valid script index for wakeup Tx
07/17/14   spa     Support to go to alt path based on concurrency manager update
06/28/14   fh      Added api for cleanup script
06/27/14   sty     skip notch functionality for irat
06/16/14   zhw     Initialize fbrx handle to invalid for modem specific allocation
06/03/14   zhw     Save curr_band for retune before invalidating rf status
05/29/14   zhw     Clean up deprecated Tuner CL APIs
05/05/14   fh      Added device_id to rf_cdma_msm_configure_txlm inf
04/17/14   JJ      Added api to read dac therm for temp comp
04/16/14   JJ      Move fbrx tune_to_chan api before config txagc in wakeup tx
04/07/14   APU     Added support for RFM_DEVICES 2 and 3 even when RFM_DEVICES 
                   0 & 1 are absent for CDMA.
03/31/14   zhw     Tuner AOL Abort support
03/28/14   cd      Reset Tx AGC state machine when Tx AGC is halted for retune
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
03/12/14   APU     Move modem_exit from timed sleep into complete_time_sleep API
                   so that we dont free up TQ, DM buffers when time sleep fails 
                   and they are available for subsequent sleep. 
03/12/14   APU     Split modem enter and exit into seperate rx and tx modem 
                   enter and exit. 
03/11/14   APU     CDMA Data Structure was being used after being reset. Moved 
                   the reset to later in the code.  
03/10/14   spa     Use Stop TxAGC MDSP API to freeze/unfreeze TxAGC on RFLM side
03/07/14   JJ      Fix FBRx call flow sequence issue. 
03/04/14   hdz     Added AsDiv mc support
03/03/14   JJ      Use api to query acq_duration and rxagc_update_duration
02/27/14   aa      Update rfcommon_fbrx_mc_tune_to_chan
02/25/14   Apu     Skip rf_cdma_mdsp_modem_exit() in rf_1x_mc_sleep_rx() 
                   for 1x2L meas
02/21/14   JJ      Fixed compilation warnings and offtarget compilation error
02/20/14   JJ      Hooked up FBRx api in enter/exit/retune/wakeup_tx
02/18/14   spa     Do not delete RxAGC cell if FW rejects timed sleep request
02/13/14   APU     Request TQ just before starting Rx. Release TQ before going 
                   to sleep thus guaranteeing symmetric TQ request\release.
01/31/14   JJ      Send rx_start script as retune event id instead of -1 for retune
01/31/14   zhw     Remove call to rfsv module. Functionality belongs to MCS now
01/17/14   JJ      Deleted Settling Time for 1x & HDR, FW gets it from FED now
01/16/14   hdz/JJ  halt txagc update while config txagc in retune
01/09/14   vr      Set to Parked Mode at the end of Exit Mode()
01/09/14   spa     Send script index -1 with Tx start,now using immediate writes
12/06/13   JJ      use script write for wakeup tx and remove immedate-write flag 
12/05/13   sty     release resources in rf_1x_mc_tear_down() instead of in 
                   rf_1x_mc_exit_mode
11/27/13   APU     IRAT Changes FOR bolt.
11/27/13   sty     Do not call rf_cdma_mc_configure_stop_rx_agc_data() 
                   in retune case
11/26/13   JJ      Added is_retune flag in rf_1x_mc_send_start_rx_helper()
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/21/13   JJ      Change to immediate write for wakeup_tx
08/15/13   zhw     Clean up CCS event handle right after TX RF on/off config
11/12/13   JJ      Removed code for getting smem_addr from FW
11/12/13   spa     Delete cells after receiving rx stop response
11/11/13   JJ      Got smem_addr from FW and saved it in cb_data.
11/07/13   sty     Do not allocate resources if device is already in 1x mode
10/21/13   cd      Make task-queue allocation for RFLM specific to non-cal mode
10/15/13   JJ      Moved rf_cdma_mdsp_modem_enter to rf_1x_mc_enter_mode for bolt
10/10/13   JJ      Moved rf_cdma_mdsp_modem_exit to rf_1x_mc_exit_mode
10/08/13   cd      Add support for common TxAGC debug flag
10/07/13   APU     In online mode for D3925, if we dont release task queue, 
                   before shutting down the CCS, FW crashes so 1x SW will release
                   TQ sleep_rx() instead od releasing it in exit() before which 
                   FW shuts down the ccs.
10/04/13   JJ      Moved set_rxlm_handle_id before configure_rx_front_end
10/02/12   sty/sar Moved rumi hack to rf_1x_mc_wakeup_tx.
10/01/13   JJ      Added workaround for script_index (Rx & Tx)
09/27/13   vr      Added mode param to rf_cdma_init_wup_time
09/19/13   APU     MC changes to support DM.
09/16/13   JJ      Added rflm_1x_enter/exit
09/10/13   spa     Skip IC operation in IRAT
08/26/13   shb     Updates for new C++ transceiver device interface
08/19/13   spa     Call get SSMA recos for slave dev in enable/disable diversity
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/02/13   spa     Send -1 buf idx with Rx Stop in IRAT mode to skip FE prog.
07/22/13   APU     SW needs to wait for AGC to settle between IC-Update and 
                   next LNA command.
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/19/13   vr      Changes to incorporate device dependennt ICIFIR filter settings
07/17/13   JJ      Change one F3 msg level from HIGH to MED
07/16/13   pk      Updated current band in Hdet data to load current band  
                   specific configuration data
07/10/13   spa     Reduce F3 levels and remove duplicate calls (RF-FWRSP)
07/08/13   spa     Free semaphore if message sending fails
07/03/13   APU     Consider QPCH flag in rf_1x_mc_rx_stop_helper() 
07/05/13   zhw     Remove dev_status_w in mc layer call flow
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
07/05/13   zhw     Tuner CL NV support
07/03/13   spa     Remove intelliceiver mode from IRAT state machine
07/03/13   APU     Fixed the message reporting incorrect error.
07/02/13   spa     Remove script mode arg from configure front end for rx wakeup
07/01/13   APU     Added additional documentation for sleep.
07/01/13   APU     Call RX_STOP from 1 place to avoid code duplication. 
06/28/13   spa     Remove is_irat_mode flag from enable diversity API  
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in 
                   IRAT or non IRAT scenarios 
06/27/13   spa     Update SSMA when calling timed sleep
06/27/13   zhw     Tuner CL support. Register call back after Tx start
06/27/13   APU     Fix for 1x2L irat crash where sleep after gap had invalidated
                   Variables.  
06/25/13   zhw     Tuner CL support. Added RL meas call back registration
06/24/13   shb     Corrected channel used to query IQMC coeff during 1x retune
06/20/13   sty     deleted redundant call to rf_cdma_ic_get_starting_power_mode
                   in rf_1x_mc_helper_for_retune()
06/14/13   APU     IRAT APIs now use the MC APIs instead of having seperate code
                   and call flow.
06/14/13   spa     Use CDMA wrapper object for antenna tuner
06/10/13   cd      Add support to clean-up Rx AGC LNA SM events
06/06/13   spa     Removed unused variable fwrsp_data and old FWRSP handlers
06/05/13   APU     Removed tx_static_nv_cal_ptr from helper_for_retune() APIs.
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/30/13   JJ      Remove un-necessary rfm_band type to sys_band conversion
05/30/13   APU     Free the semaphore if rx-start fails. Free Seq-Num also.    
05/30/13   APU     Rx start and config is now sent to FW via helper function 
                   rather than repeating the same code in all "Start Rx" 
                   related functions.
05/29/13   spa     Renamed configure_on_off to configure_tx_on_off
05/29/13   JJ      Initialize band to RFM_CDMA_MAX_BAND
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/20/13   APU     Made RFFE APIs common to HDR and 1X.
05/07/13   cd      Temp Comp - Support legacy and xPT temp comp algorithms
05/07/13   sty     Added rf_1x_mc_rx_agc_convergence() right after rx_start_rsp
                   is received
04/23/13   sty     Removed FEATURE_TRITON_MODEM
04/23/13   sty     temp fix to enable cal on DIME
04/18/13   sty/fh  Added SSBI retune rf ON/OFF support
04/10/13   spa     Moved calls to init dynamic settings to prevent zeroing out
04/10/13   ndb     Removed the unused Qtuner Nikel Functions
04/05/13   spa     Use fw_response_registration for new dispatch
04/02/13   Saul    1x. Tx slave resource manager APIs called only when not in cal.
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
04/01/13   Saul    1x. Add Tx slave resource manager calls in retune.
03/29/13   spa     Send offline QPCH flag as param in stop Rx, for timed sleep
03/28/13   zhw     Clean up NV before reloading 
03/26/13   aro     Renamed interface to indicate xpt_resource
03/26/13   aro     MC Callflow illustration
03/26/13   spa     KW error fix
03/25/13   Saul    CDMA. Abstract TxLM XPT mode used in MC in HAL layer.
03/22/13   sty     Fixed warnings, code cleanup
03/19/13   zhw     Fix compiler error
03/13/13   aro     Added HAL Bus disable to perform cleanup
03/13/13   aro     Added multilin memory cleanup
03/11/13   spa     Removed unused callback rf_1x_mc_pa_params_cfg_cb
03/08/13   zhw     Fix temp comp start F3
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
03/01/13   Saul    CDMA. FEATURE_RF_XPT_CDMA_TEMP -> FEATURE_RF_HAS_XPT_SUPPORT
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/27/13   aro     Interface to determine XPT mode of operation based on MSM
                   and NV
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/25/13   zhw     Refactored CCS event cleanup API and corner case handling
02/25/13   aro     Updated XPT configure interface to pass XPT mode from
                   calling function
02/22/13   aro     Migrated the tx resource manager callback function to
                   common CDMA EPT module
02/21/13   aro     CDMA Tx Resource manager implementation
02/21/13   spa     Add Callback and handler for pilot meas stop stream for FW
02/19/13   spa     Added API to handle pilot meas config response from FW
02/15/13   spa     Remove code under featurization FEATURE_RF_HAS_QFE1510_HDET
02/14/13   Saul    CDMA. Use DPD CFG NV to enable XPT.
02/12/13   Saul    CDMA. Use unity IQ gain from DPD cfg NV for XPT.
02/08/13   zhw     Do therm read/hdet autocal in 1x HHO
02/07/13   bmg     New tuner support for 1x wakeup rx/tx, retune, sleep, and div
02/07/13   zhw     Fix compiler error
02/07/13   zhw     Set Tx mode for tuner during retune to fix HDET readings
02/06/13   bmg     Adding initial antenna tuner manager support
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/28/13   zhw     RF front end device wakeup/sleep API refactor
01/21/13   nrk     Changed input param type path to device for rf_1x_mc_get_synth_lock_status
01/21/13   Saul    CDMA. Mainline XPT items.
01/16/13   zhw     Use NV Mapping for PA gain range during PA device wake up
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/15/13   Saul    CDMA. Mainlined XPT_ONLINE code.
01/14/13   Saul    CDMA. 1x EPT/ET call support.
01/12/13   cd      RxAGC LNA SM event configuration support
01/09/13   sty     Use RFCOMMON_MDSP_SNUM_TECH instead of RF_CDMA_MDSP_SNUM_TECH
01/04/13   sty     KW fix to use correct band type 
01/03/13   aro     Added slave tx resource reqeuest and release for wakeup
                   and sleep tx functions
01/03/13   aro     Added master tx resource reqeuest and release for wakeup
                   and sleep tx functions
12/27/12   shb     Updates for different physical rx_device per band
12/27/12   adk     Moved rf_cdma_antenna_tuner_init() to rf_cdma_mc.c
12/06/12   spa     Added device parameter to start HDET (for CCS event access)
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   sty     KW fixes
12/19/12   adk     KW fixes
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/18/12   cd      Add ET Tx configuration as part of Tx wakeup, until DAC mgr 
                   functionality is available
12/18/12   sty     KW fix to use correct band type 
12/18/12   sty     Added band info in arg list for rf_cdma_msm_configure_txlm
12/14/12   Saul    CDMA. TX AGC XPT Dev. Using FEATURE_RF_HAS_XPT_ONLINE_SUPPORT
12/13/12   Saul    CDMA. TX AGC XPT Dev. HDR
12/12/12   sty     removed updating associated_device in 
                   rf_cdma_init_freq_info_for_device()
12/12/12   Saul    CDMA. TX AGC XPT Dev. De-alloc DPD banks at Tx Off.
12/06/12   APU     Offtarget compiler warnings and CC comments.
12/06/12   APU     Replaced LM chains with Modem chains.
12/05/12   hdz     Removed rf_cdma_temp_comp_clear_status() and 
                   rf_cdma_temp_comp_set_status() due to restructure of temp comp
12/04/12   aro     F3 message update
11/29/12   zhw     Remove reference to tx_chain in tx_start/stop msg
11/29/12   av      Using proper featurization and removing featurization that is not needed
11/28/12   nrk     Added API to get RF path state and synth state.    
11/28/12   aki     Added rf_1x_mc_deinit
11/26/12   adk     Dime RFC updates to get tuner device instance
11/20/12   gvn     Featurize LTE for Triton 
11/15/12   Saul    CDMA. TX AGC XPT.
11/13/12   sty     Added constants for RF_1X_STATIC_EVENT_RXTX_SCRIPT_TIME
11/12/12   sty     No need to un-vote TCXO in rf_1x_mc_sleep_rx() if state is
                   RF_CDMA_STATE_RX_PREP
11/09/12   zhw     Support for RF device scripting (PA/ASM/QPOET)
                   Rename RF event scripts
11/08/12   cd      Removed Tx AGC override from MC layer
11/08/12   sty     Added un-vote to RF cmd app in complete_timed_sleep_rx
11/07/12   jfc     Move the rf hal bus vote handle into rf_cdma_data 
10/17/12   kai     Added calling QFE1510 HDET API rfdevice_hdet_cdma_init
10/11/12   gh      Add support for tuner rx_init
09/28/12   kai/adk Updated antenna tuner programming code
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable 
10/31/12   zhw     Abort event only when timed sleep rx fails.
10/31/12   adk     Merged antenna tuner code from Nikel.
10/29/12   zhw     Support for abort event upon timed sleep rx failure
10/19/12   Saul    CDMA. Removed code to profile writing dpd tables.
10/18/12   spa     Moved on/off functionality to rf_cdma_mc_configure_on_off 
10/17/12   sty     Warning fix
10/16/12   zhw     RF On/Off grfc scripting using overwritten rfc signal
10/16/12   sty     Removed all refs to FEATURE_RF_HAS_QFE1320
10/12/12   sty     Deleted refs to ant_tuner
10/10/12   APU     New IRAT interface changes.
10/02/12   spa     Moved PA/ASM agnostic GRFC wakeup/sleep script generation out
09/27/12   zhw     Remove write_grfc/sbi_via_rfsw flags
                   Remove do_rffe_write debug flag
09/26/12   zhw     Use PA range 2 to wake up PA in low power (temp hardcoded)
09/24/12   spa     Use GRFC scripting for RF On/OFF events
09/24/12   zhw     Cleanup RF ON OFF event handles
09/24/12   zhw     Fix diversity retune failure
09/21/12   sty     Fixed compiler error
09/21/12   sty     Deleted device pointer from  rf_cdma_process_wakeup_failure
09/21/12   sty     Moved rf_hal_enable_rfcmd_app() and TCXO buffer enable 
                   to exec_wakeup from prep_wakeup()
09/21/12   sty     Send in tech for rf_cdma_mdsp_get_device_buffer_index
09/20/12   sty     Moved CCS voting to prep_wakeup since tcxo_mgr needs CCS to 
                   be active 
09/20/12   sty     Updated comments
09/20/12   sty     Changed vote from CDMA to 1X for RF_CMD_APP
09/19/12   sty     Unvote for cmd app when sleep succeeds
09/19/12   sty     Vote and un-vote for command app when Rx is enabled/disabled
09/19/12   sty     Changed prep_wakeup_rx() to only do vreg management
                   Exec_wakeup_rx() to do all FW realted operations
09/17/12   sty     Changed order of args in rf_cdma_mdsp_configure_devices
09/14/12   spa     Removed refs to unsupported SPI antenna tuner scripts
09/14/12   sty     Deleted refs to ant_tuner
09/12/12   spa     Removed featurization flag RF_HAS_CDMA_DIME_SUPPORT 
09/13/12   sty     Renamed RF_CDMA_EVENT_ANALOG_SCRIPT to RF_CDMA_STATIC_EVENT
09/10/12   spa     Removed references to v2 device scripting APIs
09/10/12   spa     Removed unused API append_new_style_script_to_old_style
09/28/12   aro     Migrated 1x MC to use new RF-FW Sync
09/24/12   aro     KW fix
09/10/12   aro     SNUM dispatch callback functions for TX_STOP, TX_START,
                   IRAT_RX_START
09/10/12   aro     MSGR request pointer added as argument in SNUM dispatch
09/10/12   aro     SNUM dispatch callback functions for RX_START and RX_START
09/07/12   spa     Added WTR scripting support for sleep tx/retune
09/07/12   spa     Added WTR scripting support for sleep rx/wakeup tx
09/05/12   spa     Added WTR scripting support for wakeup rx/removed legacy 
                   scripts, now using buffer interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/31/12   zhw     Enabled RF on/off scripting
08/30/12   bmg     Log 1x IQ data after AGC acquisition is done
08/28/12   spa     Added functionality to bypass qpoet in wakeup tx scripting
08/28/12   APU     Added support to get modem_chain from rfc_device_info
08/28/12   rsr     Added support for TXAGC override.
08/25/12   spa     Combine rffe scripts for PA and ASM into main hal buffer intf 
08/24/12   cd      CDMA EPT Temp comp and HDET feature support
08/15/12   zhw     Added RF event handle in rf_cdma_data, which are used to 
                   control CCS event flow
08/23/12   aro     Migrated to percise RF time profiler for RFWU Profiler
08/22/12   Saul    CDMA. Changed load dpd num words val.
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
08/22/12   aro     Changes to migrate to new precide time profiler
08/08/12   adk     Added support for scripting buffer interface.
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
08/07/12   sty     Skip rf_1x_mdsp_configure_rf_onoff for now
08/03/12   Saul    CDMA. RX/TXLM sample capture support.
07/17/12   cri     Added BBF selection based on DPD configuration
07/12/12   cri     Added EPT TxLM selection 
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/09/12   aro     Moved EPT responses to EPT module
07/06/12   spa     Use rf_hal_buffer interface for generating grfc scripts 
07/06/12   aro     Uncommented EPT to use temporary feature
07/06/12   aro     Added handling of SNUM for EPT messages
07/02/12   cd      Get starting power mode now uses Rx bandwidth info to query 
                   data from device
06/28/12   APU     Changed QFE to the new generic PA interface. 
06/27/12   vs      Fix 1x/HDR spur item settings not taking effect
06/25/12   aro     Created EPT calibration module in Data structure
06/19/12   zhw     QFE1320 scripting support
06/19/12   sty     Minor change in rf_1x_mc_wakeup_tx()
06/14/12   aro     Compiler warning fix
06/14/12   aro     Updated IQ Capture and Expansion done message to update 
                   correct state variables
06/13/12   aro     Updated RxLM interface to indicate, if dynamic config is to 
                   be applied or not + if ET configuration is to be applied
06/12/12   Saul    CDMA. Providing RF ONOFF event info to FW.
06/08/12   aro     Deleted EPT IQ capture callback function
06/08/12   aro     KW fix
06/07/12   shb     Added DPD support in CDMA TX tune API - disabled by default
06/07/12   aro     Enable Vreg before calling RFDEVICE for SSMA operation
06/06/12   Saul    CDMA. Sending scripts as Dime shared mem format.
                   MC now using mdsp events.
06/06/12   aro     Restored EPT Changes
06/06/12   aro     Sequence number implementation for FW messages
06/06/12   aro     Handling of IRAT_RX_START_RSP
06/06/12   aro     Temporarily reverted EPT Changes
06/05/12   anr     Initial EPT Support
05/29/12   zhw     Refactored API naming.
05/25/12   spa     Changed err codes returned in failure/NULL data/invalid cases
05/22/12   zhw     Moved debug flag do_sbi_write to rf_cdma_data 
05/21/12   spa     Added time profiling in rf_1x_mc_exit_mode()
05/21/12   spa     Added 1X radio tear down in rf_1x_mc_exit_mode 
05/21/12   spa     Added rf_1x_mc_tear_down(), to tear down 1x radio
05/17/12   cd      Fixed stackoverflow crash in rffe_retune functionality by 
                   allocating buffer memory on the heap
05/16/12   hdz     Fixed compiler warning 
05/16/12   hdz     Added rf_cdma_data_sanity_check() for xx_sleep_rx; xx_wakeup_rx;
                   xx_sleep_tx;xx_wakeup_tx;xx_enable_diversity;xx_disable_diversity;
                   xx_retune      
05/14/12   zhw     Enabled GRFC scripts appending in retune.
05/11/12   zhw     Updated GRFC scripting generation APIs
05/10/12   zhw     Renamed rtr_handle to script_index
05/09/12   aro     Moved the semaphore POP before message is sent to FW to avoid
                   race condition
05/09/12   aro     Enabled Sequence number implementation for RX_STOP message
                   with temporary featurization for PL2.0.0
05/09/12   aro     Updated RX_STOP, TX_START, TX_STOP to use Sem Pool for 
                   synchronization
05/09/12   aro     Updated RX_START to use Sem Pool for synchronization
05/02/12   aro     Support for unique sequence number generation to be used for
                   RF-FW message synchronization
05/01/12   zhw     Updated rf_cdma_ic_stop()
05/01/12   APU     Disable ANT TUNER in rf_1x_mc_complete_timed_sleep_rx()
04/30/12   zhw     Updated rf_cdma_init_ssma_recos()
04/27/12   zhw     Updated rf_cdma_ic_stop()
04/27/12   sty     update new_chan_array at beginning of 
                   rf_1x_mc_helper_for_retune()
04/25/12   sty     Moved updating of state-variables in
                   rf_1x_mc_helper_for_retune() only if API succeeds
                   Renamed grfc_script to grfc_script_dummy to call out that the
                   pointer is not used in any meaningful way currently
04/25/12   zhw     Added support for CDMA Constants.
04/21/12   sty     Fixed typo
04/19/12   zhw     Updated set APIs usage. Fixed off-target building error. 
04/19/12   Saul    1X/HDR. Handoff fix. Zero out GRFC num only once.
04/18/12   zhaow   Replaced write pointer assignment statements with set APIs 
                   of rf_cdma_data data structure
04/18/12   sty     Fixed compile error in QFE
04/17/12   sty     Moved GRFCs to rf_1x_mc_configure_rffe_*
04/16/12   sty     Doxygen updates
04/14/12   sty     Added grfc_script pointer to calls to RFC on enable/disable 
04/12/12   aro     Set blocking call variable before sending the message
                   to firmware
04/11/12   Saul    QFE. Initial support.
04/11/12   aro     OQPCH update to indicate 1xL1 if the timed sleep
                   passed in FW or not
04/09/12   sty     Added helper functions that:
                  (a)get and release device-locks                  
                  (b) program the RTR and grfcs
                  (c) send relevant commands to MDSP (RX_START, etc)
04/09/12   spa     Moved  'get rx agc data' from rf_1x_mc_exec_wakeup_rx to rf 
                   warmup time logging function in rfm_1x.c
04/05/12   sty     Use band/chan info from master_device in rf_1x_mc_enable_diversity
                   Do not invalidate freq info for master_device
04/03/12   aro     Updated FWS config function to perform SemWait in MC
04/03/12   aro     Updated FWS config function to manage state transition in
                   FTM
04/03/12   aro     CB functions in TX_START and TX_STOP interfaces
04/03/12   aro     CB functions in RX_STOP interface
04/03/12   aro     Deleted unused param from RX_START command
04/02/12   aro     Migrated RX_START synchronization using callback function
04/02/12   aro     Moved RxAGC convergence to a separate function to be called
                   from RX_START callback function
04/02/12   aro     Doxygen update for NV reload function
04/02/12   aro     Deleted unused dispatch function
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
04/02/12   aro     Added CB as argument in Rx MDSP interfaces
03/29/12   shb     Added rf_state arg to rf_1x_configure_rx_agc as it is needed 
                   by device lna phase offset query api 
03/29/12   sty     Moved GRFC and RTR programming to exece_wakeup from prep_wakeup
03/27/12   ems     Provide implementation for RF NV Reload
03/27/12   shb     Stop HDET after Temp_Comp during 1x_tx_sleep
03/27/12   shb     Set isHKADCBusy flag to FALSE during 1x retune
03/23/12   adk     Updated rf_cdma_msm_configure_txlm() to write DAC LUT to TxLM
03/23/12   hdz     Enabled temp comp early termination in 1x retune scenario
03/22/12   aro     Removed unsed functions and types
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/21/12   adk     Update rxf_get_iqmc_coef_type to retrieve IQMC coeff for SHDR
03/15/12  Saul/aro IRAT. 1x2L 1x Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
03/14/12   vrb     Configure RxLM API requires band of operation
03/14/12   aro     Compile Error fix
03/13/12   aro     Added code to notify SV module with 1x new band chan after
                   hard handoff
03/13/12   aro     Moved FW State config and DAC cal response handling to RF
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
03/05/12   adk     Undid param update made in rf_cdma_msm_configure_txlm()
03/01/12   cd      Cleaned up legacy band nv tbl references
03/01/12   sty     Renamed rf_cdma_perform_ssma_operations to rf_cdma_get_ssma_recos
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
02/23/12   spa     Updated the type of band/chan to be rfm_cdma_band_class_type
                   and rfm_cdma_chan_type for rf_1x_mc_enable
02/22/12   adk     Updated rf_cdma_msm_configure_txlm() to write DAC LUT to TxLM
02/21/12   pv      Bug Fix for 1x/DO Boot up crash.
02/17/12   sty     Added args to rf_cdma_perform_ssma_operations
02/15/12   pv      RF Common Device Manager Implementation. 
02/08/12   shb     Added argument in CDMA device rx tune api to distinguish 
                   between MEAS script generation and standalone wakeup
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/20/12   sty     Updated arg list in rf_cdma_update_bb_ssma_recos
01/19/12   cd      Use linearizer limit configuration API from PLIM module
01/19/12   Saul    MCDO. Configure TxLM in multi-carrier mode.
01/19/12   Saul    MCDO. Added ICIFIR dynamic configuration.
01/19/12   aro     Updated all HAL interfaces to use RxLM/TxLm handle rather
                   than logical device
01/11/12   sty     Set wait time to 0 before calling rf_1x_mdsp_stop_rx, since 
                   there is no need for FW to wait before sending a RX_STOP_RSP
01/11/12   Saul    MCDO. Passing bandwidth for IQMC coefficients query.
                   Passing mode to Tx tune script. 
01/06/12   sty     Updated documentation
01/05/12   aro     Updated complete_timed_sleep function not to be a static
                   so that it can be called from RFM layer
01/05/12   aro     Upadted timed_sleep function to be a blocking call
01/05/12   sty     No need to update RF recos in sleep_rx()
01/04/12   aro     Cleaned up the Rx and Tx bandwidth configuration
01/04/12   sty     Unconditionally de-alloc memory
01/03/12   aro     Added static configuration structure as argument to
                   configure TxLM interface
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
12/29/11   aro     Interface update to pass ADC info to RxLM configure function
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
12/22/11   sty     Added time-profiling for SSMA
                   update SSMA settings for paired device only when the 
                   mc-layer API succeeds
12/21/11   sty     update the rotator settings to device status struct only if 
                   the API succeeds
12/21/11   hdz     Force isHKADCbusy to FALSE in wakeup_tx and sleep_tx 
12/20/11   sty     Use return value of rf_cdma_perform_ssma_operations()
12/19/11   sty     Check for mem_alloc success at the beginning of APIs
12/19/11   sty     [1] code optimization - restrict scope of variables
                   [2] get SSMA recos in retune()
                   [3] use helper APIs to init data structures
12/14/11   sty     Made call flow changes to make SSMA operations minimal in 
                   MC-layer
12/14/11   sty     Init SSMA recos before querying from RF device
12/13/11   Saul    MCDO. Passing bandwidth to rc tune per mode.
12/13/11   sty     Changed var name from rfdevice_1x_rx_recommendation_type to 
                   rfdevice_cdma_rx_recommendation_type
12/13/11   Saul    MCDO. Passing rx/tx bandwidth to device driver.
12/10/11   sty     Added changes to update SSMA recos in tune sequence
12/08/11   bmg     Added GNSS band/chan and tx status update calls.
12/07/11   aro     Removed Critical section lock for handling timed sleep Rx
                   response. This causes deadlock in 1xHDR operation
12/07/11   APU     Removed the check for multiple bins. The 1x driver design 
                   is such that we send all the information instead of doing
                   multiple error-prone multiple checks during retune.
12/06/11   adk     Code cleanup
12/06/11   sty     F3 message updates to reflect the "true" Rx Chan tuned to
12/02/11   cd      Provide TxLM handle information to configure Tx AGC 
12/02/11   cd      Provide TxLM handle information to configure Tx AGC 
12/02/11   hdz     Moved initial therm read to right after manage_vref() in
                   order to avoid bogus reading
12/02/11   sty     Assign correct channel number in prep_wakeup_rx()
12/01/11   sty     Rearranged callflow in rf_1x_mc_prep_wakeup_rx() to ensure
                   that correct channel is used when RF device is tuned to a 
                   channel
12/01/11   sty     updated lo_tuned_to_chan 
11/30/11   sty     update lo_tuned_to_chan during tune/retune and sleep
11/29/11   sty     Added SSMA calls during tune sequence
11/23/11   aro     Added critical section around Timed sleep response handling
11/23/11   aro     Fixed bad check causing wrong F3 error message getting
                   being generated while completeing Timed sleep response
11/21/11   aro     Mainlined debug variable to use new RF task dispatch
11/17/11   adk     Reverted for now, FW processing of SBI scripts.
11/16/11   aro     Added check not to stop iC when iC is already in 
                   stopped state
11/15/11   adk     Added support to send RTR SBI scripts to FW selectively.
11/14/11   Saul    Dataminer doc update.
11/14/11   aro     Corrected error checking bug in handle Rx Stop response func
11/11/11   hdz     Moved therm sw read from before rfc_cdma_manage_vregs() to 
                   after rfc_cdma_manage_vregs()
11/11/11   aro     Klocwork Fix
11/11/11   aro     Moved handling of Timed Sleep Rx to new RF Task dispatch
11/11/11   aro     Created a new function to handle the complete timed sleep
                   Rx action
11/10/11   aro     Upadated RF-MDSP interfaces to pass semaphore to 
                   wait on for blocking calls
11/10/11   aro     Defined CDMA semaphore as generic CDMA type
11/10/11   Saul    SV Limiting. Initial support.
11/09/11   cd      Added debug functionality to disable temp comp
11/07/11   hdz     Added support to 1xhdr temp comp
11/04/11   aro     Fixed the AGC acquisition for iC
11/03/11   cd      Provide plim data to tx AGC config
11/02/11   hdz     Changed arg list of rf_1x_temp_comp_start() 
11/01/11   aro     Updated timed_sleep_rx to behave as sleep_rx
11/01/11   aro     Added code to return error code to RFM in case of failure
10/31/11   aro     Documentation Update
10/31/11   aro     Code Beautification
10/31/11   aro     Implemented handler functions for Tx Start and Tx 
                   stop responses
10/31/11   aro     Added handler for Rx Stop response
10/31/11   aro     Added RF Task handler function for RX_START_RSP
10/28/11   Saul    IRAT update. Antenna Tuner support.
10/20/11   adk     Updated MDSP buffer index assignments.
10/20/11   hdz     Included 
                   - rfdevice_cdma_interface.h and 
                   - rfdevice_intf_cmd.h 
10/20/11   hdz     Added one-time SW therm read at the beginning of rf_1x_mc 
                   _wakeup_tx();
10/20/11   hdz     Added  rf_1x_temp_comp_start() in rf_1x_mc 
                   _wakeup_tx();
10/20/11   hdz     Added  rf_1x_temp_comp_stop() in rf_1x_mc_sleep_tx();
10/12/11   bmg     Added DSI SAR control to target max power limit
10/03/11   aro     Added additional check in rfm_1x_sleep_rx() not to send
                   RX_STOP message if the CDMA state transition is from
                   RX_PREP to RX_SLEEP
09/27/11   aro     Updated TxLM configure function to pass dynamic configuration
                   structure
09/27/11   adk     Moved SSBI scripting-related flags to rf_cdma_debug_flags.
09/27/11   aro     Fixed code alignment
09/25/11   vb      Support for DAC cal using different Irefs
09/21/11   aro     Added DAL based antenna tuner support
09/21/11   adk     Removed obsolete code.
09/20/11   aro     Fixed wrong F3 message level thrown when PLIM is not 
                   started in calibration mode in retune function
09/20/11   aro     Fixed wrong F3 message level thrown when PLIM is not 
                   started in calibration mode
09/19/11   Saul    IRAT update. Use 1x Rx AGC Acq Duration of 2 ms for IRAT.
09/13/11   aro     Added code to de-initialize antenna tuner during sleep
09/13/11   aro     Added support for CDMA antenna tuner
09/08/11   vb/aro  TxLM Dyamic updates for DAC Cal
09/07/11   aro     Updated RX_STOP interface to pass the RTC time to stop Rx
09/07/11   aro     Updated timed Rx stop function
09/07/11   aro     Updated RX_STOP interface to have offline QPCH flag
09/07/11   aro     Added MC function to perform timed Rx Stop
08/31/11   shb     Added plim/hdet start/stop calls in 1x tx wakeup/sleep/retune
                   sequences
08/18/11   Saul    IRAT updates/cleanup.
08/11/11   sty     Added time profiling for RF WU 
08/11/11   sar     Fixed KW errors.
08/08/11   aro     Documentation update
08/04/11   sty     Reverted changes in #53
08/04/11   sty     reverted changes in #52 and hard-coded the rtr_script_handle
                   to -1
08/02/11   adk     Cleaned up function rf_1x_mc_helper_for_retune().
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/26/11   sty     Added more debug messages in rf_1x_mc_exec_wakeup_rx()
07/25/11   aro     Added support to perform radio retune in RxTx state
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/22/11   sty     Added back fixes from rev #43 and #44
07/21/11   sty     updated rtr_handle to -1 in rf_1x_mc_enable_diversity()
07/20/11   sty     Changes needed to pass in NV pointers to rf_1x_configure_tx_agc
07/18/11   aro     Updated RxAGC Config interface to pass AGC Mode Param
07/14/11   aro     [1] Converted enter/exit function to have single return so that
                   callback function is executed for all cases
                   [2] Added support for CDMA mode in CDMA data structure
07/13/11   aro     Updated intelliceiver interfaces not to have device param
07/13/11   aro     Updated "Get Starting PM" interface to send iC data
07/12/11   adk     Ensure that modem_mem_free() gets called before exiting function
07/11/11   cd      Replaced old GRFC function with CDMA RFC API to fix
                   compiler error
07/11/11   cd      Removed GRFC script functions and replaced with CDMA RFC 
                   functionality
07/08/11   aro     Made 1x retune helper function static
07/07/11   adk     Updated SBI script generation code.
07/06/11   sty     Reworked rf_1x_mc_retune_rx() to not call VREG functions
                   Added rf_1x_mc_helper_for_retune() for above
                   Moved checks for null pointer to beginning of API
                   Documentation updates
07/06/11   aro     Updated iC start interface to pass band and chan
07/05/11   aro     Moved the check of iC support to MC
07/01/11   adk     Unified SBI script generation code and legacy code.
06/21/11   Saul    Updates for multi-carrier driver.
06/20/11   aro     Changed function signature of intelliceiver Stop to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Changed function signature of intelliceiver Start to pass
                   the intelliceiver data and logical device param
06/16/11   adk     Updates to provide buffer_index to FW with SBI scripts.
06/16/11   sar     Including internal 1x interface.
06/15/11   sty     Display RxAGC in dBm10
                   Moved all Msg macros to RF_Msg macros
06/14/11   Saul    Now configuring CSR freq offsets.
06/13/11   sty     Replaced RFM_CDMA_ERROR_NULL_NV_FOR_DEVICE_ON_BAND with 
                   generic error code - RFM_CDMA_ERROR_BAD_CONFIG
06/10/11   Saul    Now configuring carriers based on mask.
06/10/11   aro     Added return val for 1x MDSP init
06/09/11   sty     Added NV checks to ensure that valid NV is present during 
                   wakeup sequence
06/09/11   Saul    RxLM updates for multi-carrier driver.
06/08/11   Saul    Updates for multi-carrier driver.
06/07/11   Saul    Compile error fix from previous checkin.
06/07/11   Saul    Updates for multi-carrier driver.
06/07/11   aro     Added NULL pointer checks for Device Data
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/07/11   aro     Replaced Wakeup/Sleep Tx implementation with MC function
06/06/11   aro     Renamed Load GRFC script to sleep/wakeup grfc script
06/06/11   aro     Renamed Scipt Manager header file to GRFC
06/02/11   sty     Added MC APIs for enable/disable and retune APIs
06/02/11   aro     Removed CB and userdata from MC interface signature
06/02/11   aro     Implemented 1x Sleep interface
06/02/11   aro     Added MC implementation of 1x execute wakeup
06/02/11   aro     Added MC implementation of 1x prep wakeup
06/02/11   aro     Added 1x Enable and Disable implementation
03/21/11   sty     Added checks to protect rf_1x_mc_enter_mode if rf_cdma_data
                   for the device has not been initialized
03/18/11   sty     Added documentation for configuring GRFCs in 
                   rf_1x_mc_enter_mode instead of in rfm_init()
03/14/11   aro     Renamed mdsp init interface
03/11/11   aro     Compiler Warning fix
03/10/11   aro     Added CB execution for 1x enter
02/17/11   aro     Renamed intelliceiver function
02/08/11   sty     Added call to rfcommon_core_config_card_gpio()
01/19/11   aro     Renamed RF-MDSP filename
01/18/11   sty     Added rf_1x_mc_init()
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/04/11   sty     Call rf_cdma_intelliceiver_stop() instead of 
                   rflib_intelliceiver_stop()
01/03/11   sty     Improved doygen comments
                   Changed return values in rf_1x_mc_enter_mode
01/03/11   sty     Added checks in enter_mode and more functionality exit_mode
12/20/10   aro     Removed including of rfcom.h
12/16/10   aro     [1] Moved 1x Init to make CDMA init
                   [2] Deleted unused stubs
12/09/10   aro     Initial version

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_1x_mc.h"
#include "rf_cdma_data.h"
#include "rf_cdma_mdsp.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_1x_power_limiting.h"
#include "rf_cdma_hdet.h"
#include "rf_cdma_temp_comp.h"
#include "rf_1x_mdsp.h"
#include "rf_cdma_mdsp.h"
#include "rf_1x_rx_agc.h"
#include "rf_1x_tx_agc.h"
#include "rfcommon_core.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_msg.h"
#include "rfcommon_mdsp.h"
#include "rfm_cdma.h"
#include "rf_cdma_msm.h"
#include "rfc_cdma.h"
#include "modem_mem.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_time_profile.h"
#include "rfm_internal.h"
#include "rfdevice_cdma_interface.h"
#include "rfdevice_intf_cmd.h"
#include "rf_cdma_ssma.h"
#include "rfgnss_mc.h"
#include "rfm_types.h"
#include "rfcommon_core_device_manager.h"
#include "ftm.h"
#include "rf_cdma_mc.h"
#include "rf_1x_fw_response.h"
#include "rf_cdma_constants.h"
#include "rf_1x_meas.h"
#include "rfcommon_tx_manager.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_data.h"

#include "rfcommon_autopin_api.h"

/* New Buffer Interface */
#include "rf_hal_buffer.h"
#include "rf_hal_bus_types.h"
#include "rfcommon_mc.h"
#include "rfcommon_data.h"
#include "rfcommon_msm.h"

#include "rf_hal_common.h" /* for CCS command app */

#include "rf_1x_ept.h"
#include "rf_1x_log.h"
#include "rfdevice_class.h" /* rfdevice_id_get() */

#include "rf_cdma_mdsp_ccs_events.h"
#include "rf_cdma_mdsp_modem.h"

#include "rfcommon_fbrx_api.h"
#include "rflm_cmn_asd.h"
#include "rflm_dm_api.h"
#include "rfcommon_nv_mm.h"
#include "rflm_c2k_mc.h"
#include "rf_cdma_auto_pin.h"
#include "rf_cdma_meas.h"
/*============================================================================*/
/*!
  @name Main Control helper functions

  @brief
  This section holds all helper functions used by 1x MC layer
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to aid Rx start and config.
 
  @details
  This function can be used by any entity that wishes to start
  Rx either on the primary or diversity chain. This function
  will send the rx-config and rx-start messages to the FW.
 
  @param device
  Device on which 1x subsystem is to be entered
 
  @param dev_status_r
  The device status structure for the associated device.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param logical_device 
  The logical device for the given device.
 
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)
 
  @param is_retune
  Boolean flag to check if the API is being called for wakeup_rx or for retune
 
  @return success
  TRUE: Everything went well. Call successful.
  FALSE: Some error. went well. Call unsuccessful.
*/ 
boolean 
rf_1x_mc_send_start_rx_helper 
( 
   const rfm_device_enum_type device ,
   const rf_cdma_data_status_type* dev_status_r ,
   const lm_handle_type rxlm_handle  ,
   const rfc_cdma_logical_device_params_type* logical_dev ,
   boolean is_irat_mode,
   boolean is_retune
)
{
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;
  boolean success = FALSE ;

   /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mc_send_start_rx_helper: NULL FWRSP Data" );
  } /* if ( fwrsp_data == NULL ) */
  else
  {
    rf_cdma_mdsp_rx_start_type rx_start_c; /* RxAGC Start Config */
    rf_cdma_mdsp_event_id_type e_id;

    if ( TRUE == is_irat_mode)
    {
      e_id = RF_CDMA_MDSP_CCS_EVENT_RF_IRAT_TUNE_IN_ID ;
    }
    else if ( TRUE == is_retune ) 
    {
      e_id = RF_CDMA_MDSP_CCS_EVENT_RETUNE_ID ;
    }
    else
    {
      e_id = RF_CDMA_MDSP_CCS_EVENT_WAKEUP_RX_ID ;
    }
   
    /* get the translated event from modem HAL layer */
    rx_start_c.script_index = 
      rf_cdma_mdsp_modem_get_script_index( dev_status_r->script_index, e_id );
    
    rx_start_c.rxlm_handle = rxlm_handle; /* RXLM Handle */

    /* Start Rx */
    success = 
      rf_1x_mdsp_start_rx_config( logical_dev->rx_mdsp_path, &rx_start_c , 
                                  NULL, NULL, NULL );
    if ( FALSE == success )
    {
      RF_MSG ( RF_ERROR, "rf_1x_mc_send_start_rx_helper(): " 
               "rf_1x_mdsp_start_rx_config failed " ) ;
    }
    else
    { /* In iRat we dont send rx_start so skip it */
      if ( is_irat_mode == TRUE )
      {
        RF_MSG ( RF_HIGH ,  "rf_1x_mc_send_start_rx_helper(): " 
               "device_in_irat == TRUE. Skipping Rx Start " ) ;
      }
      else
      {
        fwrsp_hk->rx_start_rsp.device = device ;
        fwrsp_hk->rx_start_rsp.sema_token = rfcommon_semaphore_pop_item();
        rsp_cb_param.rsp_handler = rf_1x_rx_start_rsp_cb;
        rsp_cb_param.handler_data = &fwrsp_hk->rx_start_rsp;

        success = 
          rf_1x_mdsp_start_rx( logical_dev->rx_mdsp_path, &rsp_cb_param,
                              NULL , NULL );
        if ( FALSE == success )
        {
          RF_MSG ( RF_ERROR, "rf_1x_mc_send_start_rx_helper(): " 
                   "rf_1x_mdsp_start_rx failed " ) ;
          /* Message sending failed, free the semaphore */
          RF_MSG_2 ( RF_LOW, "rf_1x_mc_send_start_rx_helper():"
                     " Freeing the semaphore and not waiting "   
                     " [D%d] RX_START pushed back : Sem @ 0x%x" , 
                     device , fwrsp_hk->rx_start_rsp.sema_token ) ;
        }
        else
        {
          RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] RX_START : Start Wait Sem @ "
                            "0x%x", device , fwrsp_hk->rx_start_rsp.sema_token );
          rfcommon_semaphore_wait( fwrsp_hk->rx_start_rsp.sema_token );


        }/*if ( FALSE != success )*/

        /* Push back semaphore after wait if mssg was sent successfully,
        or immediately if mssg sending failed*/
        rfcommon_semaphore_push_item( fwrsp_hk->rx_start_rsp.sema_token );

        if ( success == TRUE )
        {
          /* Wait for RxAGC to converge ... this is the right place to wait for 
             agc to converge. This ensures that when rfm_exec_wakeup_rx()
             completes L1 is assured of a converged AGC */
          success = rf_1x_mc_rx_agc_convergence( device ) ;

        }
        else
        {
          RF_MSG ( RF_MED, "rf_1x_mc_send_start_rx_helper(): " 
                   "not waiting for RxAGC convergence since Rx start failed" ) ;
        }
      } /* If not doing irat */
    }/*if ( FALSE != success )*/
  }/*if ( fwrsp_hk != NULL )*/
  return success ;
}/* rf_1x_mc_send_start_rx_helper*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to send Rx Stop to FW.

  @details
  This function can be used by any entity that wishes to stop
  Rx either on the primary or diversity chain. This function
  will send the rx-stop messages to the FW.
   
  @param device
  Device on which 1x subsystem is to be entered
 
  @param dev_status_r
  The device status structure for the associated device.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param logical_device 
  The logical device for the given device.
 
  @param rtc_cx1_trigger
  RTC time for Rx Stop
 
  @param skip_rf_front_end_programming
  Boolean to let the function know if RF front end programming
  needs to be skipped for cases like retune. This flag is also used to 
  determine if RxAGC operations need to be performed. In case of retune, 
  this flag is set - so front-end programming and 
  rf_cdma_mc_configure_stop_rx_agc_data are to be skipped
 
  @param is_offline_qpch
  Boolean to let FW know that this is timed sleep.
 
  @return success
  TRUE: Everything went well. Call successful. 
  FALSE: Some error. Call unsuccessful.
*/
boolean
rf_1x_mc_send_stop_rx_helper 
(  
  const  rfm_device_enum_type device ,
  const  rf_cdma_data_status_type* dev_status_r ,
  const  lm_handle_type rxlm_handle ,
  const  rfc_cdma_logical_device_params_type* logical_dev ,
  uint32 rtc_cx1_trigger ,
  boolean skip_rf_front_end_programming ,
  boolean is_offline_qpch 
)
{
  boolean success = FALSE ;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device ) ;

    /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mc_send_stop_rx_helper: NULL FWRSP Data" );
  } /* if ( fwrsp_data == NULL ) */
  else
  {
    rf_cdma_mdsp_response_cb_param_type rsp_cb_param ;
    rf_cdma_mdsp_rx_stop_type rx_stop_data ; /* Rx Stop Data */
   /* The script index of -1 suggests that FW will 
    not program the scripts provided by RF for configuring the RF front-end..
    a valid scenario is retune, where teh whole Rx chain need not be put to sleep on 
    the old channel and then woken up again on the new channel.
    */
    if ( skip_rf_front_end_programming == TRUE )
    {
      rx_stop_data.script_index = -1 ;
    }
    else
    {
      rx_stop_data.script_index = 
        rf_cdma_mdsp_modem_get_script_index (
          dev_status_r->script_index , RF_CDMA_MDSP_CCS_EVENT_SLEEP_RX_ID ) ;
    }    
    rx_stop_data.rxlm_handle = rxlm_handle; /* RXLM Handle */
    /* PLL Settling time (Cx1) */

    rx_stop_data.rtc_cx1_trigger = rtc_cx1_trigger ; 
    /* RTC time for Rx Stop */

    /* Stop Rx for primary device */
    fwrsp_hk->rx_stop_rsp.device = device;
    fwrsp_hk->rx_stop_rsp.sema_token = rfcommon_semaphore_pop_item();
    fwrsp_hk->rx_stop_rsp.callback = NULL;
    fwrsp_hk->rx_stop_rsp.userdata = NULL;
    fwrsp_hk->rx_stop_rsp.blocking_call = TRUE;
    fwrsp_hk->rx_stop_rsp.timed_sleep_success = FALSE;
    rsp_cb_param.rsp_handler = rf_1x_rx_stop_rsp_cb;
    rsp_cb_param.handler_data = &fwrsp_hk->rx_stop_rsp;

    success = rf_1x_mdsp_stop_rx( logical_dev->rx_mdsp_path, &rx_stop_data,
                                  is_offline_qpch , &rsp_cb_param, NULL, NULL );

    /* Check if message sending was success or failure */
    if ( FALSE == success )
    {
      RF_MSG ( RF_ERROR, "rf_1x_mc_send_stop_rx_helper(): " 
               "rf_1x_mdsp_stop_rx failed " ) ;
      /* Message sending failed, free the semaphore */
      RF_MSG_2 ( RF_LOW, "rf_1x_mc_send_stop_rx_helper():"
                 " Freeing the semaphore and not waiting "   
                 " [D%d] RX_STOP pushed back : Sem @ 0x%x" , 
                 device , fwrsp_hk->rx_stop_rsp.sema_token ) ;
    }
    else
    {
      /* Message was sent to FW, wait for semaphore */
      RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] RX_STOP : Start Wait Sem @ "
                        "0x%x", device, fwrsp_hk->rx_stop_rsp.sema_token );
      rfcommon_semaphore_wait( fwrsp_hk->rx_stop_rsp.sema_token );
    }

    /* Push back semaphore after wait if mssg was sent successfully,
    or immediately if mssg sending failed*/
    rfcommon_semaphore_push_item( fwrsp_hk->rx_stop_rsp.sema_token );

    /* If Rx stop was succesful, clear the cmn rx agc data only in two 
    scenarios:
    1.  Skip_rf_front_end_programming is FALSE, meaning we actually disabled the 
    front end
    2. If we are in offline QPCH and timed sleep on FW side was a success 
    if it failed, L1 will call regular sleep */ 
    if ( ( skip_rf_front_end_programming == FALSE ) &&
         ( ( fwrsp_hk->rx_stop_rsp.timed_sleep_success == TRUE ) &&
           ( is_offline_qpch == TRUE )) )
    {
      rf_cdma_mc_configure_stop_rx_agc_data( device, rxlm_handle );
    }
  }/* if ( fwrsp_hk != NULL )*/
  return success ;
}/* rf_1x_mc_send_stop_rx_helper */

/*! @} */

/*============================================================================*/
/*!
  @name Main Control Interface

  @brief
  This section holds all Main Control Interfaces to be used by RFM layer.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform 1x-specific initializations

  @details
  This function will not touch any hardware related functionality. It is
  intended to configure the differrent modules (like MDSP) to the correct mode.
   
  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_1x_mc_init
(
  void
)
{
  boolean init_status = TRUE; /* 1x MDSP Init Statu */

  /* Initialize mdsp for 1x - this is a one-time init*/
  init_status &= rf_1x_mdsp_init();

  /* Perform RF Responses registration */
  init_status &= rf_1x_fw_response_registration();
  
  /* Register IRAT functions */
  init_status &= rf_1x_mc_meas_init();
  
  /* Set the FBRx enable flag based on NV */ 
  rf_cdma_data_set_fbrx_enable( RFM_1X_MODE);

  return init_status;

} /* rf_1x_mc_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform 1x-specific deinitializations

  @return
  Flag indicating a success (TRUE) or failure (FALSE)
*/
boolean
rf_1x_mc_deinit
(
  void
)
{

  boolean deinit_status = TRUE; /* Variable to track if CDMA Init passed */
  uint8 dev_idx;
  rf_hal_bus_client_handle* handle;
  const rf_1x_config_type *rf_1x_config_ptr; /* Pointer to 1x RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev;
  uint8 band_idx;

  for (dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get Read-only Device Status */
    const rf_cdma_data_status_type *dev_status_r = 
                     rf_cdma_get_device_status( (rfm_device_enum_type)dev_idx );

    if ( (NULL != dev_status_r) )
    {
      /*-------- Disable HAL Bus ---------------------------------------------*/
      handle = rf_cdma_data_get_rf_hal_bus_vote_handle(dev_idx);

      if ( handle != NULL )
      {
        if ( dev_status_r->rf_mode == RFM_1X_MODE )
        {
          rf_hal_bus_enable (FALSE, &handle, RF_HAL_RFCMD_APP_CLIENT(1X));
          rf_cdma_data_set_rf_hal_bus_vote_handle(dev_idx, handle);
        } /* if ( dev_status_w->rf_mode == RFM_1X_MODE ) */
      } /* if ( handle != NULL ) */

      /*-------- Deinit Multi Lin --------------------------------------------*/

      logical_dev = rfc_cdma_get_logical_device_params( dev_idx );

      if ( logical_dev != NULL )
      {
        for ( band_idx = RFM_CDMA_BC0; band_idx < RFM_CDMA_MAX_BAND; 
              band_idx++ )
        {
          /* ensure that NV is valid for given device and band */
          rf_1x_config_ptr = rf_cdma_nv_get_1x_data( 
                                              logical_dev->tx_nv_path[band_idx],
                                              band_idx );
          if ( rf_1x_config_ptr != NULL )
          {
            rfcommon_core_dealloc_tx_band_cal( 
               &(rf_1x_config_ptr->cdma_common_ptr->tx_cal.tx_multi_lin_cal) );
          } /* if ( rf_1x_config_ptr != NULL ) */
        } /* for ( band_idx = RFM_CDMA_BC0; band_idx < RFM_CDMA_MAX_BAND; 
                   band_idx++ ) */
      } /*if ( logical_dev != NULL ) */
    } /* if ( (NULL != dev_status_w) ) */
  } /* for (dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ ) */

  deinit_status &= rf_1x_fw_response_deregistration();

  return deinit_status;
} /* rf_1x_mc_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare radio to operate in CDMA 1x mode

  @details
  This function is used to enter 1x. This function will not touch any hardware 
  related functionality. This will only be initializing the software stack.
 
  @param device
  Device on which 1x subsystem is to be entered

  @param user_data_ptr
  Pointer to user data passed into callback
  
  @param cb_handler
  Callback handler
  
  @return
  Flag indicating whether 1x Enter is successful (TRUE) or failed (FALSE)
*/
boolean
rf_1x_mc_enter_mode
(
  rfm_device_enum_type device,
  void* const user_data_ptr,
  rfm_cb_handler_type cb_handler
)
{
  boolean ret_val = TRUE; /* Return value */
  rf_time_type enter_mode_time; 
  rf_time_tick_type enter_mode_tick;

  /* Start Profiling of Rx Warm Up */
  enter_mode_tick = rf_time_get_tick();

  /* this is the first API to be called in the tune sequence for 1x - so init 
     the RF WU vars for this sequence */ 
  rf_cdma_init_wup_time( device, RFM_1X_MODE, enter_mode_tick );

  if (device >= RFM_MAX_DEVICES)
  {
    RF_MSG_1( RF_ERROR,
          "rf_1x_mc_enter_mode: Invalid device passed (device=%d)",device);
    ret_val = FALSE; /* to indicate failure */
  }

  /* check if device data structures are initialized */
  if (rf_cdma_is_device_initialized(device) == FALSE)
  {
    RF_MSG( RF_ERROR,
            "rf_1x_mc_enter_mode: Device data structure not initialized - "
            "bailing out"); 
    ret_val = FALSE; 
  }

  /* 1) Continue only if there were no error 
     2) check if current mode is 1X - if yes, then no operations need to be
        done
  */
  if ( ret_val == TRUE )
  {
    /* perform enter mode operations only if device is NOT in 1x mode */
    if ( rf_cdma_data_get_rf_mode (device) != RFM_1X_MODE )
    {
      /* Update RF Mode */
      rf_cdma_data_set_rf_mode( device, RFM_1X_MODE );
    }
  } /* if ( ret_val == TRUE ) */

  if (rfcommon_autopin_is_enabled(RFM_1X_MODE))
  {
    if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_enter_mode(RFM_1X_MODE))
    { 
      RF_MSG(RF_ERROR, "rf_1x_mc_enter_mode: CDMA 1X AutoPin enter mode failed!");
      ret_val = FALSE;
    }
  }

  /* Execute Callback */
  if ( cb_handler != NULL )
  {
    cb_handler( RFM_ENTER_MODE_COMPLETE, user_data_ptr );
  }

  /* Record the execution time statistics for wakeup logging for this device*/
  enter_mode_time = rf_time_get_elapsed( enter_mode_tick, RF_USEC );

  /* update this component of RF WU time */
  rf_cdma_update_wup_time( device, enter_mode_time );

  /* no operations need to be done in enter_mode for 1x */
  return ret_val;

} /* rf_1x_mc_enter_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute steps needed to exit the mode the driver is currently configured to 
  operate in.

  @details
  This function tears down 1x Mode.
 
  @param device
  Device on which 1x is to be torn down
 
  @param to_mode
  New Mode to which RF is transitioning to
 
  @return
  Status of 1x Exit Procedure
*/
int32
rf_1x_mc_exit_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type to_mode
)
{
  boolean exec_status = TRUE;
  int32 ret_val = 0;
  rf_time_tick_type exit_mode_time;          /* Time spent in API */
  rf_time_type exit_mode_time_val;
  rf_cdma_state_type last_rf_state;

  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/

  /* Start Profiling of Rx exit time */
  exit_mode_time = rf_time_get_tick();

  /* Get Device Data */
  dev_status_r = rf_cdma_get_device_status( device );

  /* NULL Pointer Check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_exit_mode: Invalid device %d Data", device);
    exec_status &= FALSE; /* to indicate failure */
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get CDMA write data pointer */
  ic_data_w = rf_cdma_get_mutable_intelliceiver( device );

  /* NULL Pointer Check */
  if ( logical_dev == NULL || ic_data_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_exit_mode: Invalid device %d Logical Param", 
              device );
    exec_status &= FALSE; /* to indicate failure */
  }
  
  /* Continue only if there were no error codes */
  if ( exec_status == TRUE )
  {
    /* disable temp comp */
    rf_cdma_disable_temp_comp( device );
  
    /* disable max power limit algo */
  
    /* disable device-specific algo (such as PN breathing) */
  
    /* disable intelliceiver */
    if ( ( dev_status_r->intelliceiver.ic_supported ) && 
         ( dev_status_r->intelliceiver.ic_state != RF_CDMA_IC_STOPPED ) )
    {
      rf_cdma_ic_stop( ic_data_w, logical_dev );
    }
  
    /* Tear down radio, if needed. Flag this case with error message as
    Radio should have been in SLEEP state before doing exit. */
    if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_SLEEP )
    {
      last_rf_state = dev_status_r->data_1x.rf_state;

      rf_1x_mc_tear_down(device);

      RF_MSG_3( RF_ERROR, "rf_1x_mc_exit_mode: ****** Dev %d Tear down "
                "required [State %d -> %d] *****", device, 
                last_rf_state, dev_status_r->data_1x.rf_state ); 
    } /* if ( dev_status_r->rf_state != RF_CDMA_STATE_SLEEP ) */

    /* Update RF Mode */
    rf_cdma_data_set_rf_mode(device,RFM_PARKED_MODE);
    
    ret_val = 1;

    if (rfcommon_autopin_is_enabled(RFM_1X_MODE))
    {
      if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_exit_mode(RFM_1X_MODE))
      {
        RF_MSG(RF_ERROR, "rf_1x_mc_exit_mode: CDMA 1X AutoPin exit mode failed!");
        ret_val = 0;
      }
    }
  }
  else
  {
    ret_val = 0;
  }
  
  /* Record the execution time statistics for exiting mode for this device*/
  exit_mode_time_val = rf_time_get_elapsed( exit_mode_time, RF_USEC );

  /* Print time elapsed for exit mode */
  RF_MSG_1( RF_HIGH, "rf_1x_mc_exit_mode: Time elapsed for exit mode %d", 
            exit_mode_time_val);

  return ret_val; /* to indicate success */
} /* rf_1x_mc_exit_mode */

/*! @} */

/*============================================================================*/
/*!
  @name CDMA 1x subsystem TRM Control Functions

  @brief
  Functions for TRM to enable and disable the 1x portion of the
  CDMA RF driver.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the 1x subsystem of the CDMA RF driver.

  @details
  Prepares the radio for 1x operation on an initial band and channel.
  The 1x subsystem is initialized in the Sleep state with the requested
  band and channel.

  By initialzing 1x with a band and a channel, this avoids the situation
  where "the driver is on but there is no known channel."  This also
  allows SVDO to be implemented correctly when HDR is in traffic and 1x
  needs to transition to the primary chain in preparation for CDMA
  operation.  By forcing 1x to be enabled with a channel, the driver
  can detect that wideband Rx mode is correct, and tune radio to the
  correct initial frequency.

  If HDR is already active and on an incompatible channel, this API
  call will return an error.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_ENABLE_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param device
  The path to enable the 1x subsystem on.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param band
  RF CDMA band on which 1x radio is to be enabled.

  @param chan
  RF CDMA channel on which 1x radio is to be enabled.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_enable()
  function return until the operation is complete.

  @todo
  Work out what the error return values are for this function.
*/
rfm_wait_time_t
rf_1x_mc_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan
)
{
  rfm_wait_time_t ret_val = 0; /* Return value */

  RF_MSG_4( RF_MED, "rf_1x_mc_enable : Dev %d, RxLM Handle %d, Band %d, "
            "Chan %d", device, rxlm_handle, band, chan  );

  return ret_val;

} /* rf_1x_mc_enable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the 1x subsystem of the CDMA RF driver.

  @details
  Deactivates the 1x portion of the CDMA RF driver.  This API should
  only be called if 1x is in the Sleep state.  If the radio was
  operating in wideband mode, it is reconfigured for narrowband mode.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_DISABLE_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param device
  The path to disable the 1x subsystem on.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_disable()
  function return until the operation is complete.

  @todo
  Work out what the error return values are for this function.
*/
rfm_wait_time_t
rf_1x_mc_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle
)
{
  rfm_wait_time_t ret_val = 0; /* Return Value */

  /* baseband and RF recos are initialized to their defaults */
  rf_cdma_init_freq_info_for_device(device);

  rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE );

  RF_MSG_2( RF_MED, "rf_1x_mc_disable : Dev %d, RxLM Handle %d",
             device, rxlm_handle  );

  return ret_val;
} /* rf_1x_mc_disable */

/*! @} */

/*============================================================================*/
/*!
  @name 1x L1 Radio Control Functions

  @brief
  Functions for the 1x protocol stack to control the radio
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform all steps needed to retune a given device to a new band and/or channel

  @details
  This is a helper function for rf_1x_mc_retune() - all steps except for power
  collpase is done in this function.

  @param device
  The radio path to re-tune

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param new_band
  RF CDMA band to which 1x radio is to be retuned.
 
  @param new_chan
  RF CDMA channel to which 1x radio is to be retuned.
 
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)
 
  @return
  The anticipated time in microseconds remaining from the rfm_1x_tune()
  function return until the operation is complete, or an error code.
  If the API call succeeds the return value will be non-negative; all
  error codes will be negative.
*/
static rfm_wait_time_t
rf_1x_mc_helper_for_retune 
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type new_band,
  const rfm_cdma_chan_type new_chan,
  boolean is_irat_mode
)
{
  rfm_wait_time_t ret_val; /* final result of API */
  /* flag to track result of interim steps in the API */
  boolean retune_helper_success = TRUE; 

  /*band,chan to which chain is currently tuned to, before the retune occurs */
  rfm_cdma_band_class_type curr_band; 

  uint32 adc_clock_freq; /* var to store ADC Clock Frequency */
  rf_cdma_msm_static_rxlm_cfg_type static_cfg; /* Static RXLM  Settings */
  rf_cdma_msm_static_txlm_cfg_type static_txlm_cfg;
  rf_cdma_msm_dynamic_txlm_cfg_type dynamic_cfg;
  rxf_get_iqmc_coef_type iqmc_data; /* IQMC Coefficient data capsule */  
  rf_cdma_msm_dynamic_rxlm_cfg_type dyn_settings; /* Dynamic RXLM  Settings */
  rfm_cdma_power_mode_type power_mode; /* var to store Power Mode */
  rf_1x_mdsp_rx_agc_read_type rx_agc_read; /* RxAGC Read */

  const rf_cdma_data_status_type *dev_status_r;/* Dev Status Read Ptr*/
  rf_cdma_data_status_type *dev_status_w;/* Dev Status write Ptr*/
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
 
  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/
  rf_cdma_temp_comp_data_type *temp_comp_w; /* Temp comp write pointer*/
  rf_cdma_hdet_data_type *hdet_w; /* HDET write pointer*/
  rf_cdma_plim_data_type *plim_w; /* PLIM write pointer*/
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  rf_1x_agc_param_type agc_mode_info; /* 1x RxAGC Mode info */
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  rf_time_tick_type prof_ssma_t;          /* SSMA Time Profile */
  rf_time_type prof_ssma_t_val;
  rfdevice_cdma_therm_read_type therm_read;
  rfdevice_cdma_dac_cal_iref_type iref_data;
  uint32 slave_modem_chain = -1;
  rf_cdma_resource_mgr_cb_data_type rmgr_cb_data;
  rf_tx_resource_mgr_config_data_type txmgr_cfg;
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr = NULL; /*CDMA static NV pointer*/
  /* buffer that holds settings related to RX sleep and wakeup, and TX sleep */
  rf_buffer_intf* rx_tx_settings = NULL ;
  boolean pin_ret;

  
  /* channel array to be tuned to */
  uint16 new_chan_array[RFM_CDMA_CARRIER_NUM] = { RF_CDMA_INVALID_CHAN, 
                                                  RF_CDMA_INVALID_CHAN, 
                                                  RF_CDMA_INVALID_CHAN }; 

  /*--------------------------------------------------------------------------*/
  /* check for valid device */
  if ( device >= RFM_MAX_DEVICES )
  {
    RF_MSG_1( RF_ERROR,
          "rf_1x_mc_helper_for_retune: Invalid device %d",device);
    return RFM_CDMA_ERROR_BAD_CONFIG; 
  }

  if ( rf_cdma_mc_is_band_chan_supported(device, new_band, new_chan) == FALSE )
  {
    RF_MSG_3( RF_ERROR, "rf_1x_mc_helper_for_retune: Not tunable on "
                        "device %d, Band %d, Chan %d", device, new_band, 
                        new_chan ); 
    return RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  }

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Device Status Read Pointer*/
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get CDMA write data pointer */
  ic_data_w = rf_cdma_get_mutable_intelliceiver( device );
  temp_comp_w = rf_cdma_get_mutable_temp_comp( device );
  hdet_w = rf_cdma_get_mutable_hdet( device );
  plim_w = rf_cdma_get_mutable_plim( device );
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) ||
       ( dev_status_w == NULL ) || ( temp_comp_w == NULL ) ||
       ( hdet_w == NULL ) || ( plim_w == NULL ) ||
       ( ant_tuner_token_w == NULL ) || ( ic_data_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune(): NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /* ensure that NV is valid for given device and band */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[new_band], 
                                             new_band );

  if ( NULL == rf_1x_config_ptr )
  {
    RF_MSG_1 (RF_ERROR, "rf_1x_mc_helper_for_retune: Failed to retrieve valid"
                        " Config data from NV for band %d", new_band ); 
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  /* Tx device will be NULL for diveristy device - so do the following Tx operations 
  only if TX device is valid */
  if ( logical_dev->tx_device[new_band] != NULL )
  {     
    if ( logical_dev->tx_nv_path[new_band] >= RF_CDMA_NV_PATH_NUM )
    {
        RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune(): Invalid tx_nv_path %d",
                  logical_dev->tx_nv_path[new_band] );
      return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
    }

    tx_static_nv_cal_ptr = rf_cdma_nv_get_tx_static( 
                                              logical_dev->tx_nv_path[new_band], 
                                              new_band );
    if ( tx_static_nv_cal_ptr == NULL )
    {
      RF_MSG_1 ( RF_ERROR, "rf_1x_mc_helper_for_retune: "
                       "Failed to get valid static TX NV pointer "
                       "for device %d", device);
      return RFM_CDMA_ERROR_BAD_CONFIG;
    }
  }/*if ( logical_dev->tx_device != NULL )*/
  else
  {
    RF_MSG_1 ( RF_MED, "rf_1x_mc_helper_for_retune: "
                     "No Tx available on device:  %d", device);
  }/*if ( logical_dev->tx_device == NULL )*/

  /*--------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed */
  /*--------------------------------------------------------------------------*/
  new_chan_array[RFM_CDMA_CARRIER_0] = new_chan; /* Only carrier-0 used for 1x*/

  /* index 0 is used for Single-carrier config */
  curr_band = dev_status_r->curr_band;
  
  /* -------------------------- Update Alt. Path ---------------------------*/
  retune_helper_success &= rf_cdma_mc_update_alt_rx_path( 
                                                RFM_1X_MODE, device,
                                                new_band , 
                                                FALSE /* No retune in IRAT*/);
  /* ------------------------End of Update Alt. Path -----------------------*/

  /*--- Execute relevant steps from sleep_tx() if in RXTX state ---*/
  if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
  {
    if ( rfm_get_calibration_state() == FALSE )
    {
      slave_modem_chain = rfc_cdma_get_slave_tx_modem_chain( device, curr_band );

      if( dev_status_r->ept_online_enabled == TRUE )
      {
        retune_helper_success &= rf_common_xpt_init_dpd( device );
      }

      /*----------------------------------------------------------------------*/
      /* Release the Tx Slave resource before proceeding */
      /*----------------------------------------------------------------------*/
      retune_helper_success &= rf_release_xpt_tx_resource( device, 
                                                           (txlm_chain_type)slave_modem_chain, 
                                                           RFM_1X_MODE );
    }

    /* Stop temp comp */
    if ( rf_1x_temp_comp_stop( temp_comp_w ) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune: Failed to stop temp comp" 
                "for retune on device %d", device );
    }

    /* Stop PLIM and HDET */
    if ( rf_1x_plim_stop( device, plim_w ) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune: Failed to stop PLIM for"
                          "retune on device %d", device );
    }

    if ( rf_cdma_hdet_stop( device, hdet_w ) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune: Failed to stop HDET for"
                          "retune on device %d", device );
    }

    if (rfcommon_autopin_is_enabled(RFM_1X_MODE))
    {
      rf_cdma_autopin_stop( device );
       
    }  

    /* Since Temp Comp and HDET have been disabled, force isHKADCbusy to
    FALSE */
    rf_cdma_data_set_isHKADCbusy( device, FALSE );
  }

  /*---- execute relevant steps from sleep_rx() sequence here ---- */

  rf_cdma_data_set_script_index( device, 
                           rf_cdma_mdsp_get_device_buffer_index( RFM_1X_MODE,
                                                                 rxlm_handle) );

  /*--------------------------------------------------------------------------*/
  /* Stop intelliceiver state-machine */
  /*--------------------------------------------------------------------------*/
  if ( dev_status_r->intelliceiver.ic_supported )
  {
    rf_cdma_ic_stop( ic_data_w, logical_dev );
  }

  /*--------------------------------------------------------------------------*/
  /* Query Rx sleep script before sending RX_STOP to clear possible           */ 
  /* notches when chain is still active. The tranceiver sleep script API call */
  /* will trigger notch update, which needs Rx Chain to be active. So querying*/ 
  /* the script before Rx_stop                                                */
  /*--------------------------------------------------------------------------*/
  rx_tx_settings = rf_buffer_create (  RF_CDMA_MAX_SSBI_SCRIPT_SIZE ,
                                       RF_CDMA_MAX_RFFE_SCRIPT_SIZE ,
                                       RF_CDMA_MAX_GRFC_SCRIPT_SIZE ) ;

  if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXDIV )
  {
    retune_helper_success = rf_cdma_mc_create_rx_tranceiver_sleep_script(
                                                       logical_dev->rx_device[
                                                       dev_status_r->curr_band], 
                                                       dev_status_r->rx_bw_khz, 
                                                       dev_status_r->tx_bw_khz,
                                                       rx_tx_settings);     
  }

  /*--------------------------------------------------------------------------*/
  /* signal FW to stop_rx                                                     */
  /*--------------------------------------------------------------------------*/
  retune_helper_success = 
    rf_1x_mc_send_stop_rx_helper ( device , dev_status_r , rxlm_handle , 
                              logical_dev , RF_CDMA_RX_STOP_RTC_CX1_TRIGGER ,
                              TRUE , /* Skip FE Programming? y/n  */  
                              FALSE  /* Is QPCH? y/n */ ) ;
  if ( FALSE == retune_helper_success )      
  {
    RF_MSG ( RF_ERROR , "rf_1x_mc_helper_for_retune(): " 
               "rf_1x_mc_rx_stop_helper() returned failure" ) ;
  }

  /* Request power off for old band */
  if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
  {
    retune_helper_success &= 
                  rfdevice_cdma_rx_pwr_sleep(
                     logical_dev->rx_device[dev_status_r->curr_band]);

  /*--------------------------------------------------------------------------*/
  /* execute steps to retune the chain to new <band, chan>                    */
  /*--------------------------------------------------------------------------*/

  /*---- execute relevant steps from prep_wakeup_rx sequence here ---- */
  /* Request power on for new band */
  retune_helper_success &= 
                  rfdevice_cdma_rx_pwr_wakeup(logical_dev->rx_device[new_band]);
  }
  /*--------------------------------------------------------------------------*/
  /* SSMA operations below                                                    */
  /*--------------------------------------------------------------------------*/

  /* baseband and RF recos are initialized to their defaults */
  rf_cdma_init_ssma_recos(device);

  /* init dynamic settings before any operations */
  rf_cdma_msm_init_dyn_settings(&dyn_settings);
  dyn_settings.modem_chain =
  logical_dev->rfc_dev_info.rfc_device_info[new_band]->modem_chain ;

  prof_ssma_t = rf_time_get_tick();

  retune_helper_success &= rf_cdma_get_ssma_recos ( device, rxlm_handle,
                                                    new_band, new_chan,
                                                    logical_dev->paired_device);

  /* now read the spur-info table in NV to see if any spurs need to be
     supressed *** The notches will be suppressed when RXLM is configured ***
  */
  retune_helper_success &= rf_cdma_get_spurs_to_suppress( device, rxlm_handle, 
                                 new_band, new_chan, dev_status_r->num_carriers, 
                                 &dyn_settings );

  /* Update the number of notches used on this device (for tracking use 
  for SSMA callback) 1x has only one carrier ,use index zero*/
  dev_status_w->ssma_cb_data.num_notch_filters_used = 
                   dyn_settings.notch_filt_settings[0].num_notch_filters_used;

  prof_ssma_t_val = rf_time_get_elapsed( prof_ssma_t, RF_USEC );
 
  /*--------------------------------------------------------------------------*/
  /* Determine ADC clock frequency */
  /*--------------------------------------------------------------------------*/
  adc_clock_freq = rf_cdma_msm_get_adc_samp_freq( rxlm_handle, new_band, 
                                                  new_chan );

  /* Populate static settings */
  static_cfg.adc_clock_freq = adc_clock_freq;
  /* pass in NULL for band_chan array since only one carrier is active for 1x */
  static_cfg.bw_khz = rf_cdma_compute_bandwidth( 1, new_band, NULL );
  static_cfg.rxlm_adc = logical_dev->rxlm_adc;
  static_cfg.config_as_et = FALSE;
  
  /*--------------------------------------------------------------------------*/
  /* Determine Power Mode for desired band and channel */
  /*--------------------------------------------------------------------------*/
  power_mode = rf_cdma_ic_get_starting_power_mode( ic_data_w, 
                                                   new_band,
                                                   new_chan,
                                                   static_cfg.bw_khz );
    
  /*--------------------------------------------------------------------------*/
  /* Configure RxLM */
  /*--------------------------------------------------------------------------*/

  /* Populate the IQMC Data for query */
  iqmc_data.lin = power_mode;
  iqmc_data.band = new_band;
  iqmc_data.chan = new_chan;

  iqmc_data.sec_chain_rf_state = dev_status_r->data_1x.rf_state;
  iqmc_data.bw_khz = static_cfg.bw_khz;

  /* Get RSB coefficients from device driver */
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[new_band],
                                 RFDEVICE_GET_IQMC_COEFS,
                                 &iqmc_data );

  /*--------------------------------------------------------------------------*/
  /* Populate RxLM dynamic settings based on carrier-config  */  
  /*--------------------------------------------------------------------------*/
  dyn_settings.apply_dynamic_cfg = TRUE;
  dyn_settings.iqmc_coeff_in = iqmc_data.iqmc_a_coeff;
  dyn_settings.iqmc_coeff_quad = iqmc_data.iqmc_b_coeff;
  dyn_settings.num_carriers = dev_status_r->num_carriers;
  dyn_settings.modem_chain =
  logical_dev->rfc_dev_info.rfc_device_info[new_band]->modem_chain ;

  rf_cdma_compute_carrier_info( 
                                new_band,
                                new_chan_array,
                                1 /* num_carriers = 1 for 1x*/, 
                                dev_status_r->carrier_index, 
                                &dyn_settings.carrier_mask,
                                dyn_settings.freq_offset_hz, 
                                0 /* no need to add anything to calculated
                                     freq offset */
                              );

  /* Configure Rx Link Manager Buffer for given Device, Power Mode, and
      ADC Sample Frequency */
  retune_helper_success &= 
    rf_cdma_msm_configure_rxlm( rfdevice_id_get(logical_dev->rx_device[new_band]),
                                rxlm_handle, RFM_1X_MODE,new_band,
                                power_mode, static_cfg, dyn_settings );

  /*--------------------------------------------------------------------------*/
  /* Configure RxAGC */
  /*--------------------------------------------------------------------------*/

  /* Populate the AGC Mode */
  agc_mode_info.mode = RF_1X_AGC_MODE_NORMAL;
  agc_mode_info.acq_duration = rf_cdma_msm_get_1x_default_agc_acq_duration();

  /* Configure AGC Param for the given device and Channel */
  retune_helper_success &= rf_1x_configure_rx_agc( device, rxlm_handle, 
                                       new_band, new_chan, power_mode, 
                                       dev_status_r->data_1x.rf_state, &agc_mode_info );

  /*--------------------------------------------------------------------------*/
  /* Configure RF device and send rx_start command to FW                      */
  /*--------------------------------------------------------------------------*/
  if ( retune_helper_success == TRUE )
  {
    uint32 tx_bw_khz;

    if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXDIV )
    {
      tx_bw_khz = RF_CDMA_BW_INVALID;    
    }
    else
    {
      if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
      {
        tx_bw_khz = static_cfg.bw_khz;
      }
      else
      {
        tx_bw_khz = RF_CDMA_BW_INVALID;
      }
    }

    /* Enter Device Critical Section, so that the device call is thread safe */
    rfdevice_cdma_rx_enter_critical_section( logical_dev->rx_device[new_band] );

    if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Since 1x can retune with TX enabled, need to first stop TxAGC updates
         by FW, before any changes are done to the shared memory or rf_on_off
         scripts.
         No need to do this is cal mode, since FW is in override mode during cal
      */
      rf_1x_mdsp_stop_txagc_update(TRUE);
    }

    retune_helper_success &= rf_cdma_mc_configure_front_end_for_retune ( 
                                    device, rxlm_handle, new_band, new_chan, 
                                    power_mode, static_cfg.bw_khz, tx_bw_khz, 
                                    RFDEVICE_CDMA_STANDALONE_SCRIPT, logical_dev, 
                                    tx_static_nv_cal_ptr,
                                    ant_tuner_token_w,
                                    dev_status_r->data_1x.rf_state, curr_band,
                                    rx_tx_settings );

    if ( TRUE == retune_helper_success )
    {
      retune_helper_success &= rf_1x_mc_send_start_rx_helper ( 
                                           device, dev_status_r,
                                           rxlm_handle  , logical_dev,
                                           is_irat_mode, TRUE /*is_retune*/ ) ;
      if ( TRUE == retune_helper_success )
      {
        if ( logical_dev->ant_tuner[new_band] != NULL )
        {
          rf_cdma_atuner_ack_script( logical_dev->ant_tuner[new_band], 
                                     ant_tuner_token_w );
        }
      }
    } /* if ( retune_helper_success == TRUE ) */

    /* Clean up RF CCS event smem and event handle */
    rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_STATIC_EVENT );

    /* Leave Device Critical Section */
    rfdevice_cdma_rx_leave_critical_section( logical_dev->rx_device[new_band] );

  } /* if ( retune_helper_success == TRUE ) */

  /* Unconditionally clean-up the retune script */
  if ( rx_tx_settings != NULL)
  {
    /* De-allocate memory for script container */
    rf_buffer_destroy( rx_tx_settings );
  }

  /* Unconditionally clean-up the Rx AGC LNA SM events here */
  retune_helper_success &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( device );

  if ( retune_helper_success == TRUE )
  {  
    /*------------------------------------------------------------------------*/
    /* restore pre-retune conditions                                            */
    /*------------------------------------------------------------------------*/

    /* If retune function is called when Radio is in RxTx State, then
    re-configure Tx AGC. Re-start HDET */
    if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
    {
      rfcommon_fbrx_error_type fbrx_status = RFCOMMON_FBRX_SUCCESS;
      lm_handle_type fbrx_lm_handle = (lm_handle_type)~0; /* fbrx lm handle */
      uint16 channels[RFM_CDMA_CARRIER_CUMULATIVE];
      rfcom_band_type_u fbrx_band;

      if ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE )
      {
      /* Get the FBRx lm handle */
      fbrx_lm_handle = rf_cdma_data_get_fbrx_lm_handle(device);

      /* Updates FBRx frequency-dependent values */     
      channels[0] = new_chan;
      fbrx_band.cdma_band = new_band; 
      fbrx_status = rfcommon_fbrx_mc_tune_to_chan( 
                                    dev_status_r->txlm_handle, 
                                    RFM_1X_MODE, 
                                    fbrx_band,
                                    channels,
                                    RFCOMMON_FBRX_1_CARRIER,
                                    fbrx_lm_handle );
      }
      /*Set Pout Skip to FALSE*/
      rf_cdma_set_pout_comp_skip ( device , FALSE ) ;

      /*----------------------------------------------------------------------*/
      /* Do SW therm read */
      /*----------------------------------------------------------------------*/
      rfdevice_cdma_read_therm( logical_dev->tx_device[new_band],
                                FALSE,
                                &therm_read );

      temp_comp_w->last_temp_sensor_read = therm_read.therm_reading;
      temp_comp_w->is_last_reading_valid = therm_read.therm_quality;

      RF_MSG_2( RF_LOW, "rf_1x_mc_helper_for_retune: therm reading: %d, and %d",
                therm_read.therm_reading, therm_read.therm_quality );

      /* Do DAC therm read*/
      rf_cdma_mdsp_iref_therm_read(dev_status_r->txlm_handle);	

      /* Tx spectral inverstion feature enabled for MB\HB, in-order to inform the 
      MSM about this, we need to update the txr iq dac group settings otherwise our tx
      spectrum will be inverted and the data will not be decoded properly by BS.
      Example Sceario: If we make call on LB (BC0) and retune to HB\MB (BC1 or BC6)
      then we need to inform the MSM about the spectral inverstion feature for the new bands*/
  
      retune_helper_success &= 
      rf_cdma_mc_trigger_dyn_txr_iq_dac_update(device,
  	                                        new_band );

    
      /* If  TxR_IQ_DAC dynamic update fails, then HO will fail in Tx mode
      so check the status and return immediately with Error flag*/
      if ( retune_helper_success == FALSE )
      {
        RF_MSG_2( RF_ERROR, "rf_1x_mc_helper_for_retune:  TxR_IQ_DAC dynamic update" 
		            " failed for device %d band %d",
                            device, new_band );
      }	  

      retune_helper_success &= rf_1x_configure_tx_agc( device, new_band, 
                                                       new_chan,
                                                       dev_status_r->txlm_handle,
                                                       rf_1x_config_ptr );

      /* Reset the Tx AGC state machine in order to retune without turning 
      Tx OFF. This is explicitly required for 1x, because retune may involve 
      transitioning between bands with different Tx AGC params such as xPT 
      mode, PA state configuration etc. */
      rflm_txagc_reset_txagc_stm( dev_status_r->txlm_handle, TRUE );

        /* indicate to FW that it can now start using the updated SM, and run 
           txAGC in free-running mode - this needs to be done only AFTER sending 
           configure_tx_agc command to FW */
        retune_helper_success &= rf_1x_mdsp_stop_txagc_update(FALSE);

      if ( rfm_get_calibration_state() == FALSE )
      {
        static_txlm_cfg.txlm_dac = logical_dev->txlm_dac;
        static_txlm_cfg.xpt = rf_cdma_xpt_get_operation_mode( device );

        iref_data.band = new_band;
        (void) rfdevice_cdma_tx_get_dac_cal_iref ( 
           logical_dev->tx_device[new_band], &iref_data ) ;
        dynamic_cfg.iref_val = iref_data.iref_val;
        dynamic_cfg.num_carriers = dev_status_r->num_carriers;
        dynamic_cfg.tx_modem_chain = rfc_cdma_get_tx_modem_chain( device, new_band );
        dynamic_cfg.tx_spectral_inversion = FALSE;

        /* Acquire and Enable the Tx Slave resource */
        rmgr_cb_data.cdma_mode = RFM_1X_MODE;
        rmgr_cb_data.xpt_mode = static_txlm_cfg.xpt;
        rmgr_cb_data.band = new_band;
        rmgr_cb_data.chan = new_chan;
        rmgr_cb_data.tx_nv_path = logical_dev->tx_nv_path[new_band];
        rmgr_cb_data.master_txlm_handle = dev_status_r->txlm_handle;
        rmgr_cb_data.static_cfg = static_txlm_cfg;
        rmgr_cb_data.dynamic_cfg = dynamic_cfg;

        txmgr_cfg.func_ptrs.init_fp = rf_cdma_resource_mgr_init_xpt;
        txmgr_cfg.func_ptrs.deinit_fp = rf_cdma_resource_mgr_deinit_xpt;
        txmgr_cfg.func_ptrs.prep_fp = rf_cdma_resource_mgr_prep_xpt;
        txmgr_cfg.func_ptrs.setup_fp = rf_cdma_resource_mgr_setup_xpt;
        txmgr_cfg.func_ptrs.cleanup_fp = rf_cdma_resource_mgr_cleanup_xpt;
        txmgr_cfg.cb_data.data_size = sizeof(rf_cdma_resource_mgr_cb_data_type);
        txmgr_cfg.cb_data.data_ptr = &rmgr_cb_data;

        retune_helper_success &= 
            rf_request_xpt_tx_resource( device, 
                                        (txlm_chain_type)slave_modem_chain, 
                                        RFM_1X_MODE,
                                        txmgr_cfg );
      }

      /*----------------------------------------------------------------------*/
      /* Start 1x temp comp module */
      /*----------------------------------------------------------------------*/
      /* Start temp comp */
      if ( rf_1x_temp_comp_start( temp_comp_w,
                                  RFM_1X_MODE,
                                  logical_dev,
                                  new_band,
                                  new_chan) == FALSE )
      {
            RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune: Failed to start "
                                "temp comp for retune on device %d", device ); 
      }

      /*----------------------------------------------------------------------*/
      /* Start 1x Power limiting and HDET */
      /*----------------------------------------------------------------------*/
      if ( rf_1x_plim_start(device,
                            logical_dev,
                            plim_w,
                            new_band) == TRUE )
      {
        /* Start HDET only if Power Limiting was started successfully. Else no 
        point doing HDET reads */
        if ( rf_cdma_hdet_start(device, hdet_w,
                              RFM_1X_MODE,
                              logical_dev,
                              new_band,
                              new_chan) == FALSE )
        {
          /* Clean up HDET trig event in case of hdet start failure */
          rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_HDET_CONFIG );

          RF_MSG_3( RF_ERROR, "rf_1x_mc_helper_for_retune: HDET start failed on "
                              "device %d, band: %d, chan: %d", 
                    device, new_band, new_chan );
        }

        /* Update lim_vs_temp_vs_freq */
        rf_1x_plim_update_lim_vs_temp_vs_freq( 
                              device,
                              plim_w,
                              dev_status_r->plim.band,
                              dev_status_r->lo_tuned_to_chan,
                              dev_status_r->temp_comp.last_temp_sensor_read );
      }
      else
      {
        RF_MSG_2( RF_HIGH, "rf_1x_mc_helper_for_retune: 1x PLIM not started on "
                  "device %d, band: %d", device, new_band );
      }

      /*--------------------- FBRx Settings ----------------------------------*/
      if ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE )
      {
      /* Writes the FBRx modem settings to HW if tx bw changes */
      if (static_cfg.bw_khz != dev_status_r->tx_bw_khz)
      {
        fbrx_status &= rfcommon_fbrx_enable( fbrx_lm_handle );
        if( fbrx_status == RFCOMMON_FBRX_ERROR )
        {
          RF_MSG_1(RF_FATAL, "rf_1x_mc_helper_for_retune: "
                             "FBRx enable failed: handle %d", fbrx_lm_handle);
        }
      }
      }	

     /*------------------------------------------------------------------------*/
     /* Build script for 1x Autopin  */
     /*------------------------------------------------------------------------*/    
     if ( rfcommon_autopin_is_enabled(RFM_1X_MODE) &&
         rf_cdma_autopin_par_support(device, 
	 	                          logical_dev, 
	 	                          RFM_1X_MODE, 
	 	                          new_band ) == TRUE)
      {
       /* Get the scripts for AutoPin*/
       	pin_ret = rf_cdma_configure_autopin_trigger (
   				    device,
  				    RFM_1X_MODE,
				    logical_dev,
			    	    new_band);
    	if (pin_ret == FALSE)
    	{
          RF_MSG( RF_ERROR, "rf_1x_mc_wakeup_tx: pin configure failed ");
		  
    	}
      }

    } /* if ( dev_status_r->rf_state == RF_CDMA_STATE_RXTX )*/
  } /* if ( retune_helper_success == TRUE ) */

  /*---- execute relevant steps from execute_wakeup_rx sequence here ---- */
  if ( retune_helper_success == TRUE )
  {
    int32 dummy_result;
    
    /* Tell GPS about the band and channel we are on. */
    RFGNSS_MC(1x_band_chan, dummy_result)( new_band, new_chan);
    if ( dummy_result == FALSE )
    {
      RF_MSG( RF_ERROR, 
              "rf_1x_mc_helper_for_retune: rfgnss_mc API failed !!" );
    }

    rf_cdma_data_set_curr_band( device, new_band );

    /* update only the valid/appropriate carrier_index and channel */
    rf_cdma_data_set_curr_chan( device , RFM_CDMA_CARRIER_0, new_chan );
    rf_cdma_data_set_carrier_index( device , RFM_CDMA_CARRIER_0 , 0 );
    rf_cdma_data_set_rx_bw_khz( device , static_cfg.bw_khz );
    rf_cdma_data_set_num_carriers( device , 1 );

    /* retune has succeeded at this point - safe to update SSMA recos for paired
       device now, and also safe to update device status  */
    rf_cdma_update_bb_ssma_recos( logical_dev->paired_device ); 

    /* update the rotator offsets to the device data structure */
    rf_cdma_data_update_rot_offsets( device, dyn_settings.freq_offset_hz );

    rf_cdma_data_set_lo_tuned_to_chan( device , dev_status_r->rf_rx_reco.rf.lo_chan );

    if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
    {
      rf_cdma_data_set_tx_bw_khz( device, static_cfg.bw_khz );
	  
      /* Set the pout comp flag */
      if ( rfcommon_autopin_is_enabled(RFM_1X_MODE) && 
 	   rf_cdma_autopin_par_support(device, 
				  logical_dev, 
				  RFM_1X_MODE, 
				   new_band ))
      {
        rf_cdma_set_pout_comp_skip ( device , TRUE ) ;
      }
    }
    else
    {
      rf_cdma_data_set_tx_bw_khz( device, RF_CDMA_BW_INVALID );
    }

    if (dev_status_r->intelliceiver.power_mode == RFM_CDMA_POWER_MODE_NORMAL)
    {
      rfdevice_cdma_rx_post_pll_lock_processing( logical_dev->rx_device[new_band] );
    }

    /* Start the IntelliCeiver if the Analog Reciever supports/requires it */
    if ( dev_status_r->intelliceiver.ic_supported == TRUE )
    {
      rf_cdma_ic_start( ic_data_w, logical_dev, new_band,                     
                        dev_status_r->curr_chans[RFM_CDMA_CARRIER_0] );
    }

    /* Get Final RxAGC/ LNA State */
    rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );

    rx_agc_read.rx_agc = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                             rx_agc_read.rx_agc);

    ret_val = 0;  /* success */

    /* Report wakeup time and final Rx AGC and LNA state */
    RF_MSG_4( RF_MED, "rf_1x_mc_helper_for_retune: Dev %d, RxLM %d, "
              "RxAGC (dBm10) %d, LNA %d", device, rxlm_handle, 
              rx_agc_read.rx_agc, rx_agc_read.lna_state );

    /* Report band/chan to which device has been tuned to */
    RF_MSG_4( RF_MED, "rf_1x_mc_helper_for_retune: [Status ]Dev %d, Band %d, "
            "Chan %d, SSMA_chan %d", device, new_band, 
            new_chan, dev_status_r->lo_tuned_to_chan);

  } /* if ( retune_helper_success == TRUE ) */
  else
  {
    RF_MSG( RF_ERROR, 
            "rf_1x_mc_helper_for_retune: Retune helper API failed !!" );
    ret_val = RFM_CDMA_ERROR_FAILURE;
  }

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_1( RF_MED, "rf_1x_mc_helper_for_retune: Time profile - SSMA %d", 
            prof_ssma_t_val );
  
  /* return final result of API */
  return ret_val;

} /* rf_1x_mc_helper_for_retune */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare to turn on the radio and tune to the given band and channel.

  @details
  Prepare the software to move the 1x reciever from the Sleep state to the Rx 
  state. Powers on all circuits required to recieve a 1x signal.

  For optimal operation, this API should be called with the same band
  and channel the radio was prepared for in the rfm_1x_enable(). This function 
  should not be used to re-tune the radio if it already awake.

  This API does not interact with FW or the RF device - the actual programming 
  of the hardware is done in rf_1x_mc_exec_wakeup_rx()

  @image html rf_cdma_wakeup_rx_sequence.jpg

  @param device
  The receive path to wakeup.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param band
  RF CDMA band on which 1x radio is to be enabled.

  @param chan
  RF CDMA channel on which 1x radio is to be enabled.
 
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)
 
  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code when trying to tune the radio when RF Dev is in Invalid State
 
  @retval RFM_CDMA_ERROR_INVALID_DEV
  Error code when trying to tune the radio in an invalid Device
 
  @retval RFM_CDMA_ERROR_FAILURE
  Error code when RF Fails to Tune in spite of good parameter and state
 
  @return
  The anticipated time in microseconds remaining from the rfm_1x_wakeup_rx()
  function return until the operation is complete.
*/
rfm_wait_time_t
rf_1x_mc_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  boolean is_irat_mode
)
{
  rfm_wait_time_t ret_val; /* Return value */
  boolean wakeup_success = TRUE; /* Flag to track the success of wakeup rx */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_1x_config_type* cdma_config_ptr; /* Pointer to NV data */

  rf_time_tick_type prof_vreg_t;      /* Vreg Time Profile */
  rf_time_type prof_vreg_t_val;      /* Vreg Time Profile */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Perform NULL Pointer check */
  if ( logical_dev == NULL ) 
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_prep_wakeup_rx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_prep_wakeup_rx(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /* Get NV for the requested device and band and check if it is NULL */
  cdma_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[band],
                                            band );
  if ( cdma_config_ptr == NULL )
  {
    /* no NV for the requested device and band - bail out */
    RF_MSG_2( RF_ERROR, "rf_1x_mc_prep_wakeup_rx: NULL NV for device %d on "
                        "band = %d", device, band);
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  /*--------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed                              */
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  /* bring carrier info to initial-state: invalidate all carrier_index &      */
  /* channels                                                                 */
  /*--------------------------------------------------------------------------*/
  rf_cdma_init_freq_info_for_device(device);
  rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE );

  /* assign the desired band and chan */
  rf_cdma_data_set_curr_band( device, band );

  /* update only the valid/appropriate carrier_index and channel */
  rf_cdma_data_set_curr_chan( device , RFM_CDMA_CARRIER_0, chan );
  rf_cdma_data_set_carrier_index( device , RFM_CDMA_CARRIER_0 , 0 );

  /* assign correct values for Rx and Tx BW */
  rf_cdma_data_set_rx_bw_khz( device , RF_CDMA_BW_1X );/* 1.25MHz for 1x */

  /* TX BW is 0Mhz since Tx is not yet active */
  rf_cdma_data_set_tx_bw_khz( device , RF_CDMA_BW_INVALID  );

  /* num of carriers is 1 for OneX*/
  rf_cdma_data_set_num_carriers( device , 1 );  
  
  /* set rxlm in rf_cdma_data */
  rf_cdma_data_set_rxlm_handle( device, rxlm_handle);

  /*! allocate asd memory*/
  rflm_cmn_asd_allocate_memory((uint32)rxlm_handle);
  
  /*--------------------------------------------------------------------------*/
  /* Enable Chip Power for all devices in the receiver - Device, RxAGC, RFCard*/
  /*--------------------------------------------------------------------------*/
  if ( is_irat_mode == FALSE )
  {
    prof_vreg_t = rf_time_get_tick();

    wakeup_success &= rfc_cdma_manage_vregs( device, RFM_1X_MODE, band,
                                             RF_PATH_RX_STATE);

    prof_vreg_t_val = rf_time_get_elapsed( prof_vreg_t, RF_USEC );

    /* report time taken */
    RF_MSG_1 ( RF_MED, "rf_1x_mc_prep_wakeup_rx: time profile - Vreg %d ", 
               prof_vreg_t_val );
  }


  /* --------------------------- END OF STATIC INIT --------------------------*/
  /* No Static de-initialization can be done after this point. Only wakeup 
     hardware sequence is executed */
  /*--------------------------------------------------------------------------*/

  /* Check if all Static Initialization when through. If not, return Failure.
  For successful execution, do the Rx Wakeup/Tune hardware sequence. */
  if ( wakeup_success == TRUE )
  {
    int32 dummy_result;

    /* Tell GPS about the band and channel we are on. */
    RFGNSS_MC(1x_band_chan, dummy_result)(band, chan);
    if ( dummy_result == FALSE )
    {
      RF_MSG( RF_ERROR, 
              "rf_1x_mc_prep_wakeup_rx: rfgnss_mc API failed !!" );
    }

    /* Update CDMA Data structure with new RF State and band/Chan */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_RX_PREP );
    rf_cdma_data_set_curr_chan( device, RFM_CDMA_CARRIER_0, (uint16) chan );
    rf_cdma_data_set_curr_band( device,  band );

    rf_cdma_data_sanity_check( device );

    /* Report success */
    RF_MSG_4( RF_MED, "rf_1x_mc_prep_wakeup_rx: Dev %d, RxLM %d, Band %d, "
              "Chan %d", device, rxlm_handle, band, chan);

    /* No Wait time */
    ret_val = 0;

  } /* if ( wakeup_success == TRUE ) */
  else
  {
    /* undo changes done to driver state machine */
    rf_cdma_process_wakeup_failure( device, RFM_1X_MODE, band );    

    /* Report failure */
    RF_MSG_4( RF_ERROR, "rf_1x_mc_prep_wakeup_rx: Failed!! Dev %d, RxLM %d, "
              " Band %d, Chan %d", device, rxlm_handle, band, chan);

    ret_val = RFM_CDMA_ERROR_FAILURE;
  }

  /* Return the API Wait time */
  return ret_val;

} /* rf_1x_mc_prep_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Wakeup Rx sequence to turn on the radio and tune to the given band 
  and channel.

  @details
  
  Prepares all the scripts required to tune the radio to the requested channel.

  This function will send a command to firmware to indicate that Rx needs to be
  started. The command will indicate FW to activate the RxLM configuration 
  along with the RTR script to be programmed. Based on the Settling time
  sent, FW will start the RxAGC as well.

  @image html rf_cdma_wakeup_rx_sequence.jpg

  @param device
  The receive path to execeute wakeup rx sequence.
 
  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)

  @param meas_type
  mes type to pass in the cript type in IRAT or non-IRAT scenarios
  In IRAT scenarios, startup or cleanup script in No-IRAT mode always invalid.

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code when trying to tune the radio when RF Dev is in Invalid State
 
  @retval RFM_CDMA_ERROR_INVALID_DEV
  Error code when trying to tune the radio in an invalid Device
 
  @retval RFM_CDMA_ERROR_FAILURE
  Error code when RF Fails to Tune inspite of good parameter and state

  @retval RFM_CDMA_ERROR_INVALID_MODE
  Error code when trying to tune the radio when RF Mode is not correct.
 
  @return
  The anticipated time in microseconds remaining from the
  rfm_1x_exec_wakeup_rx() function return until the operation is complete.
*/
rfm_wait_time_t
rf_1x_mc_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  boolean is_irat_mode,
  const rfdevice_meas_scenario_type meas_type
)
{
  rfm_wait_time_t ret_val; /* Return value */
  boolean exec_success = TRUE; /* tracks results of various APIs called */
  rfm_cdma_power_mode_type power_mode;
  rf_1x_agc_param_type agc_mode_info; /* 1x RxAGC Mode info */  

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  rf_cdma_data_status_type *dev_status_w;/* Dev Status write Ptr*/
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  timetick_type prof_rxagc_cfg_t; /* RxAGC Config Time Profile */
  timetick_type prof_rxlm_t;      /* RXLM Time Profile */
  timetick_type prof_ssma_t;      /* SSMA Time Profile */

  uint32 adc_clock_freq; /* var to store ADC Clock Frequency */
  rxf_get_iqmc_coef_type iqmc_data; /* IQMC Coefficient */  
  rf_cdma_msm_static_rxlm_cfg_type static_cfg; /* Static RXLM  Settings */
  rf_cdma_msm_dynamic_rxlm_cfg_type dyn_settings; /* Dynamic RXLM  Settings */
  rfm_cdma_band_class_type band;
  uint32 script_index ;

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Device Status mutable Pointer*/
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Get CDMA Data Write Pointer*/
  ic_data_w = rf_cdma_get_mutable_intelliceiver( device );
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) || 
       ( dev_status_w == NULL ) || 
       ( ic_data_w == NULL) || ( ant_tuner_token_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_exec_wakeup_rx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_helper_for_retune(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;

  }

  /* check for valid band */
  band = dev_status_r->curr_band;
  
  if ( band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_exec_wakeup_rx: Invalid band %d ", band );
    return RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  }
  
  /* check for valid carriers */
  if ( dev_status_r->num_carriers >= RFM_CDMA_CARRIER_NUM )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_exec_wakeup_rx: Invalid num_carriers %d ", 
               dev_status_r->num_carriers );
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  /*--------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed                              */
  /*--------------------------------------------------------------------------*/

  /* -------------------------- Update Alt. Path ---------------------------*/
  exec_success &= rf_cdma_mc_update_alt_rx_path( RFM_1X_MODE, device,
                                                 band ,
                                                 is_irat_mode);
  /* ------------------------End of Update Alt. Path -----------------------*/

  /* allocate resources (data structs, queues, etc) needed for 1x operation */ 
  rf_cdma_mdsp_modem_rx_allocate_resources( device, rxlm_handle,
                                            RFM_1X_MODE , is_irat_mode ) ;

  /* init dynamic settigs before any RXLM operations */
  rf_cdma_msm_init_dyn_settings(&dyn_settings);

  if ( is_irat_mode == FALSE )
  {
    /* vote for RFCMD app - this should done as early as possible in the 
             wakeup sequence however in IRAT the source tech already does this */
    {
      rf_hal_bus_client_handle* handle = rf_cdma_data_get_rf_hal_bus_vote_handle(device);
      exec_success &= rf_hal_bus_enable (TRUE, &handle, RF_HAL_RFCMD_APP_CLIENT(1X));
      exec_success &= rf_cdma_data_set_rf_hal_bus_vote_handle(device, handle);
    }

    /* Determine Power Mode for desired band and channel */
    power_mode = rf_cdma_ic_get_starting_power_mode( 
                                   ic_data_w, 
                                   band,
                                   dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
                                   dev_status_r->rx_bw_khz );
    script_index = 
      rf_cdma_mdsp_get_device_buffer_index(RFM_1X_MODE, rxlm_handle ) ;
  }/* if ( is_irat_mode == FALSE ) */
  else
  {
    /* Set lowest power mode in IRAT scenario */
    power_mode = rf_cdma_ic_get_lowest_power_mode ( ic_data_w ); ;
    script_index = dev_status_r->irat_data.irat_buffer_index ;
  }

  /* Vote ON for all RF Devices which as needed with RFM Device (device) 
  This API call help to vote ON/OFF for TCXO buffer and NPLER BYPASS settings */
  exec_success &= rfcommon_core_dev_power_vote( device,
                                                RFCMN_CORE_DEV_ON,
                                                RFCMN_CORE_DEV_NORMAL_VOTE );

  if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
  {
    exec_success &= rfdevice_cdma_rx_pwr_wakeup(logical_dev->rx_device[band]);
  }

  /*--------------------------------------------------------------------------*/
  /* SSMA operations below                                                    */
  /*--------------------------------------------------------------------------*/
  prof_ssma_t = timetick_get();

  exec_success &= rf_cdma_get_ssma_recos ( device, rxlm_handle, 
                                   band, 
                                   dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
                                   logical_dev->paired_device );

  /* now read the spur-info table in NV to see if any spurs need to be
     supressed *** The notches will be suppressed when RXLM is configured     */
  exec_success &= rf_cdma_get_spurs_to_suppress ( 
                                   device, rxlm_handle, band, 
                                   dev_status_r->curr_chans[RFM_CDMA_CARRIER_0], 
                                   dev_status_r->num_carriers, &dyn_settings );

  /* Update the number of notches used on this device (for tracking use 
  for SSMA callback) 1x has only one carrier ,use index zero*/
  dev_status_w->ssma_cb_data.num_notch_filters_used = 
                dyn_settings.notch_filt_settings[0].num_notch_filters_used;

  prof_ssma_t = timetick_get_elapsed( prof_ssma_t, T_USEC );
  /*--------------------------------------------------------------------------*/
  /* Configure RxLM */
  /*--------------------------------------------------------------------------*/
  prof_rxlm_t = timetick_get();

  adc_clock_freq = rf_cdma_msm_get_adc_samp_freq( rxlm_handle, 
                                 band, 
                                 dev_status_r->curr_chans[RFM_CDMA_CARRIER_0] );

  /* Populate the IQMC Data for query */
  iqmc_data.lin = power_mode;
  iqmc_data.band = band;
  iqmc_data.chan = dev_status_r->curr_chans[RFM_CDMA_CARRIER_0];
  iqmc_data.sec_chain_rf_state = RF_CDMA_STATE_INVALID;
  iqmc_data.bw_khz = dev_status_r->rx_bw_khz;

  /* Populate static settings */
  static_cfg.adc_clock_freq = adc_clock_freq;
  static_cfg.bw_khz = dev_status_r->rx_bw_khz;
  static_cfg.rxlm_adc = logical_dev->rxlm_adc;
  static_cfg.config_as_et = FALSE;
  
  /* Get RSB coefficients from device driver */
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[band],
                                 RFDEVICE_GET_IQMC_COEFS,
                                 &iqmc_data);

  dyn_settings.apply_dynamic_cfg = TRUE;
  dyn_settings.iqmc_coeff_in = iqmc_data.iqmc_a_coeff;
  dyn_settings.iqmc_coeff_quad = iqmc_data.iqmc_b_coeff;
  dyn_settings.num_carriers = dev_status_r->num_carriers;
  dyn_settings.modem_chain =
                   logical_dev->rfc_dev_info.rfc_device_info[band]->modem_chain;

  /* update dyn_settings with rotator offset based on num_carriers,
     lo_offset_KHz and curr_chans */
  rf_cdma_compute_carrier_info( band,
                                dev_status_r->curr_chans,
                                dev_status_r->num_carriers, 
                                dev_status_r->carrier_index, 
                                &dyn_settings.carrier_mask,
                                dyn_settings.freq_offset_hz,
                                dev_status_r->rf_rx_reco.rf.lo_offset_KHz*1000
                              );

  exec_success &= 
    rf_cdma_msm_configure_rxlm( rfdevice_id_get(logical_dev->rx_device[band]),
                                rxlm_handle, RFM_1X_MODE, band,
                                power_mode, static_cfg, dyn_settings );

  prof_rxlm_t = timetick_get_elapsed( prof_rxlm_t, T_USEC );

  /*--------------------------------------------------------------------------*/
  /* Configure RxAGC */
  /*--------------------------------------------------------------------------*/

  /* Populate the AGC Mode */
  agc_mode_info.mode = RF_1X_AGC_MODE_NORMAL;
  agc_mode_info.acq_duration = rf_cdma_msm_get_1x_default_agc_acq_duration();

  /* Configure AGC Param for the given device and Channel */
  prof_rxagc_cfg_t = timetick_get();
  
  exec_success &= rf_1x_configure_rx_agc( device, rxlm_handle, 
                                          band, 
                                          dev_status_r->rf_rx_reco.rf.lo_chan, 
                                          power_mode, 
                                          dev_status_r->data_1x.rf_state,
                                          &agc_mode_info );

  prof_rxagc_cfg_t = timetick_get_elapsed( prof_rxagc_cfg_t, T_USEC );

  /*--------------------------------------------------------------------------*/
  /* Configure Analog Script and send RX_START to FW                          */
  /*--------------------------------------------------------------------------*/
  rf_cdma_data_set_script_index( device, script_index ) ;

  if ( exec_success == TRUE )
  {  
    /* Enter Device Critical Section, so that the device call is thread safe */
    rfdevice_cdma_rx_enter_critical_section( logical_dev->rx_device[band] );

    exec_success &= rf_cdma_mc_configure_front_end_for_rx_wakeup (
                      device, RFM_1X_MODE, FALSE /*is_slave_device*/,
                      rxlm_handle, band, dev_status_r->rf_rx_reco.rf.lo_chan, 
                      power_mode, dev_status_r->script_index, 
                      dev_status_r->rx_bw_khz, dev_status_r->tx_bw_khz,
                      logical_dev,
                      ant_tuner_token_w,
                      is_irat_mode, 
                      meas_type);

    if ( exec_success == TRUE )
    {
      exec_success = 
      rf_1x_mc_send_start_rx_helper ( device, dev_status_r, rxlm_handle, 
                                      logical_dev, is_irat_mode, 
                                      FALSE /*is_retune*/ );
    
      if ( logical_dev->ant_tuner[band] != NULL )
      {
        rf_cdma_atuner_ack_script( logical_dev->ant_tuner[band], 
                                   ant_tuner_token_w );
      }     
    } /* ( exec_success == TRUE ) */

    /* Leave Device Critical Section */
    rfdevice_cdma_rx_leave_critical_section( logical_dev->rx_device[band] );

  } /* if ( exec_success == TRUE ) */
   
  if ( ( exec_success == TRUE ) && (is_irat_mode == FALSE) )
  {
    /* do not need to apply notches while in irat (design decision) */
    /* Apply notch filter recommendations from WTR */
    exec_success &= 
    rf_cdma_mc_trigger_dynamic_notch_update(device,
                        dev_status_r->ssma_cb_data.ssma_reco,
                        dev_status_r->ssma_cb_data.num_notch_filters_used,
                        dev_status_r->ssma_cb_data.clear_notch); 
  }

  /* Dont clean-up in the iRAT case. Let the cleanup API do it in exit_meas() */
  if ( is_irat_mode == FALSE )
  {
    /* Clean up RF CCS event smem and event handle */
    exec_success &=
                  rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_STATIC_EVENT );

    /* Unconditionally clean-up the Rx AGC LNA SM events here */
    exec_success &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( device );
  }
      
  if ( exec_success == TRUE )
  {
    if ( dev_status_r->intelliceiver.power_mode == RFM_CDMA_POWER_MODE_NORMAL)
    {
      rfdevice_cdma_rx_post_pll_lock_processing( logical_dev->rx_device[band] );
    }

    /* Start the IntelliCeiver if the Analog Reciever supports/requires it ,
    additionally check for IRAT mode, if we are in IRAT we should skip all
    Intelliceiver operations. IRAT is designed to be only in a single power
    mode */
    if ( ( dev_status_r->intelliceiver.ic_supported == TRUE ) &&
         ( is_irat_mode == FALSE ))
    {
      rf_cdma_ic_start( ic_data_w, logical_dev,
                        band, dev_status_r->curr_chans[RFM_CDMA_CARRIER_0] );
    }
    else
    {
      /* Print F3 */
      RF_MSG_2( RF_LOW, "rf_1x_mc_exec_wakeup_rx: skipping IC Start :"
                          "ic_supported: %d ; is_irat_mode:%d", 
                dev_status_r->intelliceiver.ic_supported, 
                is_irat_mode ); 
    }/*if ( ( dev_status_w->intellic.....*/

    /* exec has succeeded at this point - safe to update SSMA recos for paired
       device now, and also safe to update device status  */
    rf_cdma_update_bb_ssma_recos( logical_dev->paired_device );
      
    /* Update CDMA Data structure with new RF State and  band/Chan */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_RX );
    rf_cdma_data_set_lo_tuned_to_chan( device, 
                                          dev_status_r->rf_rx_reco.rf.lo_chan );

    /* update the rotator offsets to the device data structure */
    rf_cdma_data_update_rot_offsets( device, dyn_settings.freq_offset_hz );

    ret_val = 0; /* no wait needed by L1 */

    rf_cdma_data_sanity_check( device );

  } /* if ( exec_success == TRUE ) */
  else
  { 
    /* undo changes done to driver state machine */
    rf_cdma_process_wakeup_failure( device, RFM_1X_MODE, band );  

    /* invalidate all SSMA recos for paired device that were retrieved by the
       rf_cdma_get_ssma_recos() call above */
    rf_cdma_init_ssma_recos ( logical_dev->paired_device );
 
    /* Report failure */
    RF_MSG_2( RF_ERROR, "rf_1x_mc_exec_wakeup_rx: Failed!! Dev %d, RxLM %d",
              device, dev_status_r->rxlm_handle );
 
    ret_val = RFM_CDMA_ERROR_FAILURE;

  } /* if ( exec_success == FALSE ) */

  /* Report time taken for AGC and results */
  RF_MSG_5 ( RF_MED, 
            "rf_1x_mc_exec_wakeup_rx: [Status %d] Device %d, RxLM %d, "
            "RxAGC time %d, RxLM config time %d ",
            ret_val, device, dev_status_r->rxlm_handle, prof_rxagc_cfg_t, 
            prof_rxlm_t );

  /* Return the API result */
  return ret_val;

} /* rf_1x_mc_exec_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the receiver.

  @details
  Moves the 1x radio from the Rx state to the Sleep state.  Any circuits
  that are no longer needed are powered down to conserve current.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_RX_SLEEP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.
 
  If SLEEP is requested from RX state, then make sure that iC is stopped,
  RTR is programmed to Sleep state, and RX_STOP is
  sent to FW. However, if SLEEP is requested from RX_PREP state, there is no 
  need of reprogramming RTR : Just disabling VREG is enough, as RTR programming 
  (and FW RX_START) is done only from execute_Rx_Wakeup (not from 
  prep_rx_wakeup). Moreover, we cannot send RX_STOP without prior RX_START. Thus
  if we dont skip RX_STOP during RX_PREP to RX_SLEEP transition, then FW will
  assert a crash.

  @param device
  The receive path to put to sleep.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)
 
  @return
  The anticipated time in microseconds remaining from the rfm_1x_sleep_rx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure.
 
  @note
  We have 2 flags that we use to have mc_sleep_rx behave in the following 4 ways:
  1> is_irat_mode [self explanatory name]
  2> RF_CDMA_STATE_SHUTDOWN_FOR_IRAT [We are in a partial shutdown. Some actions
     have been performed while some are pending and that we can only perform
     limited operations while in this state]

  |===================|===============|==============|==============|==============|
  |===================|===============|==============|==============|==============|
  | Sleep-Action      |D2L [RF SLeep] |D2L [L1 Sleep]| L2D Sleep    | Normal Sleep |
  |===================|===============|==============|==============|==============|
  |===================|===============|==============|==============|==============|
  |Sleep Front End    |  Dont Care    |  YES         |  Dont Care   |   YES        |
  |                   |               |              |              |              |
  |===================|===============|==============|==============|==============|
  | Send Rx Stop      |  YES          |  NO          |NO L1 does it |   YES        |
  |                   |               |              |              |              |
  |===================|===============|==============|==============|==============|
  | Shutdown VREGS    |  NO           |  NO          |  NO          |   YES        |
  |                   |               |              |              |              |
  |===================|===============|==============|==============|==============|
  | Reset CDMA state  |  NO           |  YES         |  YES         |   YES        |
  |                   |               |              |              |              |
  |===================|===============|==============|==============|==============|
  | Call ModemExit()  |  NO           |  YES         | YES          |   YES        | 
  |                   |               |              |              |              |
  |===================|===============|==============|==============|==============|
  |===================|===============|==============|==============|==============|


  Below is the table for how the 2 variables translate to 4 sleep combinations.
  ||===================|=====================||===================||
  ||===================|=====================||===================||
  ||  is_irat_mode     |  SHUTDOWN_FOR_IRAT  ||  ACTION           ||
  ||                   |                     ||                   ||
  ||===================|=====================||===================||
  ||===================|=====================||===================||
  || FALSE             |  FALSE              ||  Normal SLEEP     ||
  ||                   |                     ||                   ||
  ||===================|=====================||===================||
  || FALSE             |  TRUE               || D2L-L1 sleep      ||
  ||                   |                     || after gap         ||
  ||===================|=====================||===================||
  || TRUE              |  FALSE              ||  D2L-RF sleep in  ||
  ||                   |                     || enter_meas API    ||
  ||===================|=====================||===================||
  || TRUE              |  TRUE               || L2D RF Sleep in   ||
  ||                   |                     || exit_meas         ||
  ||===================|=====================||===================||
  ||===================|=====================||===================||

*/
rfm_wait_time_t
rf_1x_mc_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  boolean is_irat_mode
)
{
  rfm_wait_time_t ret_val = 0; /* Return value */
  boolean sleep_success = TRUE; /* Flag to track the success of sleep rx */
  boolean dev_voting_status; /* Need the flag to perform device voting */

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/

  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  rf_time_tick_type prof_sleep_t = rf_time_get_tick();        /* time spent in API */
  rf_time_type prof_sleep_t_val;

  /* Flag to indicate if FE programming is required by FW or not
  in IRAT cases it can be skipped */
  boolean skip_frontend_programming = FALSE;

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get CDMA Data Write Pointer*/
  ic_data_w = rf_cdma_get_mutable_intelliceiver( device );
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) || 
       ( ic_data_w == NULL) || ( ant_tuner_token_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_sleep_rx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_sleep_rx(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /*--------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed */
  /*--------------------------------------------------------------------------*/

  /* If SLEEP is requested from RX state, then make sure that iC is stopped,
  RTR is programmed to Sleep state, and RX_STOP is sent to FW. However, if SLEEP 
  is requested from RX_PREP state, there is no need of reprogramming RTR : Just 
  disabling VREG is enough, as RTR programming (and FW RX_START) is done only 
  from execute_Rx_Wakeup (not from prep_rx_wakeup). Moreover, we cannot send 
  RX_STOP without prior RX_START. Thus if we dont skip RX_STOP during RX_PREP to 
  RX_SLEEP transition, then FW will assert a crash. */
  if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX )
  {
    rfm_cdma_band_class_type band = dev_status_r->curr_band;

  /*------------------------------------------------------------------------*/
  /* Stop intelliceiver state-machine,check if ic is supported and if we are 
     in IRAT mode or not, all IC operations are skipped in IRAT */
  /*------------------------------------------------------------------------*/
  if ( ( dev_status_r->intelliceiver.ic_supported ) &&
         ( is_irat_mode == FALSE ) )
  {
    rf_cdma_ic_stop( ic_data_w, logical_dev );
  }
  else
  {
    /* Print F3 */
      RF_MSG_2( RF_LOW, "rf_1x_mc_sleep_rx: skipping IC Stop :"
                          "ic_supported: %d ; is_irat_mode:%d", 
              dev_status_r->intelliceiver.ic_supported, 
                is_irat_mode ); 
  }/*if ( ( dev_status_w->intellic.....*/

    /*------------------------------------------------------------------------*/
    /* Configure RF-front end for Sleep and send RX_STOP to FW                */
    /*------------------------------------------------------------------------*/
    rf_cdma_data_set_script_index( device, 
                            rf_cdma_mdsp_get_device_buffer_index(RFM_1X_MODE,
                                                                 rxlm_handle) );
  
    /* Enter Device Critical Section, so that the device call is thread safe*/
    rfdevice_cdma_rx_enter_critical_section( logical_dev->rx_device[band] );
  
    /* In IRAT refrain from sleeping the FE in case it is being shared by Source Tech */
    if ( is_irat_mode == FALSE )
    {
      sleep_success &= rf_cdma_mc_configure_front_end_for_rx_sleep ( 
                               device, rxlm_handle, FALSE /* is_slave_device */, 
                               band,
                               dev_status_r->script_index,
                               dev_status_r->rx_bw_khz, 
                               dev_status_r->tx_bw_khz, logical_dev,
                               ant_tuner_token_w );
      /* we have programmed a new script in smem for Rx stop for FE
      FE programming needs to be done to execute this script */
      skip_frontend_programming = FALSE;
    }
    else
    {
      /* we are in IRAT mode, we have not updated the frontend scripts
      hence we need to skip FE programming in RX stop, otherwise we can 
      potentially cause FW to execute a wrong event  */
      skip_frontend_programming = TRUE; 
    }/*if ( is_irat_mode == FALSE )*/
  
     /* In 1x2L at during meas exit RF_CDMA_STATE_SHUTDOWN_FOR_IRAT will be set 
       in the Gap so L can start measurements
       In 1x2D when sleep is called RF_CDMA_STATE_SHUTDOWN_FOR_IRAT is set so
       we dont send Rx Stop but still reset the SW state.
    */
    if ( sleep_success == TRUE )  
    {
      if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_SHUTDOWN_FOR_IRAT ) 
      {
        sleep_success = 
          rf_1x_mc_send_stop_rx_helper ( device , dev_status_r , rxlm_handle , 
                                    logical_dev , RF_CDMA_RX_STOP_RTC_CX1_TRIGGER ,
                                    skip_frontend_programming , 
                                    /* Skip FE Programming? y/n  */  
                                    FALSE  /* Is QPCH? y/n */ ) ;
        if ( sleep_success == FALSE )
        {
          RF_MSG ( RF_ERROR , "rf_1x_mc_sleep_rx: "
                          "rf_1x_mc_rx_stop_helper() returned failure " );
        }

        if ( logical_dev->ant_tuner[band] != NULL )
        {
          rf_cdma_atuner_ack_script( logical_dev->ant_tuner[band],
                                     ant_tuner_token_w );
        }
      }
      else
      {
        RF_MSG ( RF_MED , "rf_1x_mc_sleep_rx(): " 
                 "rf_1x_mc_rx_stop_helper() Skipped " 
                 "RF_CDMA_STATE_SHUTDOWN_FOR_IRAT is TRUE " ) ;
      }
    } /* if ( sleep_success == TRUE ) */
  
    /* Clean up RF CCS event smem and event handle */
    rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_STATIC_EVENT );
  
    /* Leave Device Critical Section */
    rfdevice_cdma_rx_leave_critical_section( logical_dev->rx_device[band] );
  } /* if ( dev_status_r->rf_state == RF_CDMA_STATE_RX ) */

  /* Disable Vreg and Update CDMA Data structure if ret_val is good. This part 
  of code will be executed for both RX_PREP->RX_SLEEP or RX->RX_SLEEP 
  transitions */
  if ( sleep_success == TRUE )
  {
    rf_time_tick_type prof_ssma_t; /* tracks time taken by SSMA routines */
    rf_time_type prof_ssma_t_val;

    /*------------------------------------------------------------------------*/
    /* rf_1x_mc_sleep_rx can be called after prep_wakeup or after exec_wakeup.
       If called after prep_wakeup, no need to un-vote if RF is in 
       RF_CDMA_STATE_RX_PREP, since prep_wakeup_rx() does not vote for TCXO 
       buffer (i.e., it does not call rfcommon_core_dev_power_vote ) */

    /* If in IRAT dont touch the VREGs and power votes when CDMA is either the SRC
       or the Target tech.
       1> When CDMA is the source tech Vregs need to be ON for the Target tech like 
       LTE because turning off the Vregs will cause LTE to not find any energy.
       2> When CDMA is the target tech then the SRC tech handles all the VREG management.   
    */
    if ( is_irat_mode == FALSE )
    {
      if ( dev_status_r->data_1x.rf_state !=  RF_CDMA_STATE_RX_PREP )
      {
		if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
		{
          dev_voting_status = 
                      rfdevice_cdma_rx_pwr_sleep(
                             logical_dev->rx_device[dev_status_r->curr_band]);
        }
		else
		{
          dev_voting_status =  TRUE;
        }

        /* Vote OFF for all RF Devices which are not needed with RFM Device 
           This API call help to vote ON/OFF for TCXO buffer and NPLER BYPASS 
           settings */
        dev_voting_status &= rfcommon_core_dev_power_vote(device,
                                                         RFCMN_CORE_DEV_OFF,
                                                         RFCMN_CORE_DEV_NO_VOTE);
        if( dev_voting_status == FALSE )
        {
          RF_MSG_1( RF_ERROR, "rf_1x_mc_sleep_rx(): Device: %d Pwr Down failed"
                            ,device);
          ret_val = RFM_CDMA_ERROR_FAILURE;
        }

        sleep_success &= dev_voting_status;
      } /* if ( dev_status_r->rf_state !=  RF_CDMA_STATE_RX_PREP ) */

      /* Revert the Vreg State, since Rx is going to sleep */
      rfc_cdma_manage_vregs( device, RFM_1X_MODE, dev_status_r->curr_band,
                             RF_PATH_SLEEP_STATE);

      /* un-vote for RFCMD app */
      {
        rf_hal_bus_client_handle* handle = rf_cdma_data_get_rf_hal_bus_vote_handle(device);
        rf_hal_bus_enable (FALSE, &handle, RF_HAL_RFCMD_APP_CLIENT(1X));
        rf_cdma_data_set_rf_hal_bus_vote_handle(device, handle);
      }

      /* Abort pending AsDiv Event */
      rf_cdma_mdsp_abort_asd_events( rxlm_handle );
	  
      /*! deallocate asd memory*/
      rflm_cmn_asd_deallocate_memory((uint32)rxlm_handle);
    }/* if ( is_irat_mode == FALSE )*/


    /*------------------------------------------------------------------------*/
    /* Undo any SSMA operations performed on paired_device                    */
    /* NOTE: device status must be updated (as done above) before calling SSMA*/ 
    /*------------------------------------------------------------------------*/
    prof_ssma_t = rf_time_get_tick();   /* SSMA Time Profile */

    /* device successfully put to sleep - undo any SSMA operations performed
       on the paired device... since the device is asleep, need to use
       RF_CDMA_INVALID_BAND and RF_CDMA_INVALID_CHAN while calling the SSMA
       routines */
    (void) rf_cdma_get_ssma_recos ( device, rxlm_handle, RFM_CDMA_MAX_BAND,
                                    RF_CDMA_INVALID_CHAN,
                                    logical_dev->paired_device );

    (void) rf_cdma_update_bb_ssma_recos( logical_dev->paired_device );

    prof_ssma_t_val = rf_time_get_elapsed( prof_ssma_t, RF_USEC );
    RF_MSG_3( RF_MED, "rf_1x_mc_sleep_rx: completed for Dev %d, RxLM %d "
                      "SSMA time %d", device, rxlm_handle, prof_ssma_t_val );

    /* ***** Warning ****** Warning ******* Warning ******* Warning ****** Warning ****** Warning ******* 
    **  No writes to anything on SSBI, GRFC  RFFE bus or any peripheral after 
    **  here. TQ will be relinquished */

    /* In 1x2L in meas_exit , dont destroy the band channel because for CDMA to L measurements,
      the next command after the gap the L1 sends is a sleep command which is when 
      we will reset the SW data structures. */
    if ( ( is_irat_mode == TRUE ) &&
       ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_SHUTDOWN_FOR_IRAT ) )
    {
      RF_MSG ( RF_MED , "rf_1x_mc_sleep_rx(): "
             "In CDMA-2-L meas enter. Skip update of device structures" ) ;
      RF_MSG ( RF_MED , "rf_1x_mc_sleep_rx(): "
             "in CDMA-2-L meas exit. Skip rf_cdma_mdsp_modem_exit()" ) ;
    }
    else
    {
      /* We need to be in RX state to deallocate resources, cannot deallcoate
      if we are in prep since we will end up deallocating resources that 
      were never allocated to 1x in the first place */
      if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX_PREP )
      {
        RF_MSG ( RF_MED , "rf_1x_mc_sleep_rx(): "
             "Skipping RFLM resource deallocation since RF state is 'Prep'" ) ;
      }
      else
      {
      /* This function releases all the TQs and TQs are required to 
      queue any read\writes to CCS so this function call should be 
      after all device read or writes. */ 
        rf_cdma_mdsp_modem_rx_deallocate_resources ( device , RFM_1X_MODE ,
                                                      rxlm_handle ) ;
      }

      rf_cdma_reset_sw_state ( device ) ;
    }
    rf_cdma_data_sanity_check( device );

  } /* if ( sleep_success == TRUE )*/ 
  else
  {
    RF_MSG_2( RF_ERROR, "rf_1x_mc_sleep_rx: error occured for Dev %d, RxLM %d ", 
              device, rxlm_handle); 
    ret_val =  RFM_CDMA_ERROR_FAILURE;
  }

  prof_sleep_t_val = rf_time_get_elapsed( prof_sleep_t, RF_USEC );


  RF_MSG_3( RF_MED, "rf_1x_mc_sleep_rx: Dev %d, RxLM %d, time_taken %d", 
            device, rxlm_handle, prof_sleep_t_val );

  return ret_val;

} /* rf_1x_mc_sleep_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the receiver in future time.

  @details
  Based on the time provided, this funciton moves the 1x radio from the Rx state 
  to the Sleep state. This function will basically create RTR/GRFC scripts and
  sends the RX_STOP command indicating that the radio should be stopped
  in future time.
 
  Any circuits that are no longer needed will be programeed and powered down
  when RF receives a response back from FW.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_RX_TIMED_SLEEP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  @param device
  The receive path to put to sleep.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param rtc_cx1_trigger
  RTC time in Cx1 units. This is the time when RX should be stoppped.
 
  @param is_offline_qpch
  Flag indicating, if this function is called in reference to the Offline
  QPCH sequence

  @return
  The anticipated time in microseconds remaining from the rfm_1x_sleep_rx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure.
*/
rfm_wait_time_t
rf_1x_mc_timed_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 rtc_cx1_trigger,
  const boolean is_offline_qpch
)
{
  rfm_wait_time_t ret_val = 0; /* Return value */
  boolean sleep_success = TRUE; /* Flag to track the success of sleep rx */

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/

  rf_cdma_ic_data_type *ic_data_w; /* Intelliceiver write pointer*/
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  rfm_cdma_band_class_type band;

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get CDMA Data Write Pointer*/
  ic_data_w = rf_cdma_get_mutable_intelliceiver( device );
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) || 
       ( ic_data_w == NULL) || ( ant_tuner_token_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_timed_sleep_rx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_timed_sleep_rx(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /* Get current band */
  band = dev_status_r->curr_band;

  if ( band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_timed_sleep_rx: Invalid band %d ", band );
    return RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  }

  /*--------------------------------------------------------------------------*/
  /* Stop intelliceiver state-machine */
  /*--------------------------------------------------------------------------*/
  if ( dev_status_r->intelliceiver.ic_supported )
  {
    rf_cdma_ic_stop( ic_data_w, logical_dev );
  }

  /* Abort pending AsDiv Event */
  rf_cdma_mdsp_abort_asd_events( rxlm_handle );  

  /*! deallocate asd memory*/
  rflm_cmn_asd_deallocate_memory((uint32)rxlm_handle);

  /* --------------------------- END OF STATIC INIT ------------------------*/
  /* No Static de-initialization can be done after this point. Only Sleep 
     hardware sequence is executed */
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  /* configure RF Front-end for Sleep and send RX_STOP to FW                  */
  /*--------------------------------------------------------------------------*/
  rf_cdma_data_set_script_index( device, 
                            rf_cdma_mdsp_get_device_buffer_index(RFM_1X_MODE,
                                                                 rxlm_handle) );

  if ( sleep_success == TRUE )
  {
    /* Enter Device Critical Section, so that the device call is thread safe*/
    rfdevice_cdma_rx_enter_critical_section( logical_dev->rx_device[dev_status_r->curr_band] );

    sleep_success &= rf_cdma_mc_configure_front_end_for_rx_sleep ( 
                       device, rxlm_handle, 
                       FALSE /* is_slave_device  */, band,
                       dev_status_r->script_index, dev_status_r->rx_bw_khz,
                       dev_status_r->tx_bw_khz, logical_dev,
                       ant_tuner_token_w);

    if ( sleep_success == TRUE )
    {
      sleep_success = 
        rf_1x_mc_send_stop_rx_helper ( device , dev_status_r ,rxlm_handle , 
                                  logical_dev , rtc_cx1_trigger ,
                                  FALSE , /* Skip FE Programming? y/n  */ 
                                  is_offline_qpch    /* Is QPCH? y/n */ ) ;
      if ( FALSE == sleep_success )
      {
        RF_MSG ( RF_ERROR , "rf_hdr_mc_timed_sleep(): " 
               "rf_1x_mc_rx_stop_helper() returned failure" ) ;
      }
    } /* if ( sleep_success == TRUE ) */
  
    /* Clean up RF CCS event smem and event handle regardless faliure or not*/
    rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_STATIC_EVENT );

    /* Leave Device Critical Section */
    rfdevice_cdma_rx_leave_critical_section( logical_dev->rx_device[band] );

  } /* if ( sleep_success == TRUE ) */

  /* Check if all Static de-initialization when through. If not, return 
  Failure. For successful execution, do the sleep hardware sequence. */
  if ( sleep_success == TRUE )
  {
    /* Do not Revert the Vreg State now. These will be done when RF_TASK 
       receives RX_STOP done message */

    /* Update CDMA Data structure with new RF State and  band/Chan */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_SLEEP_SCHEDULED);

    /*---------------------------------------------------------------------*/
    /* Undo any SSMA operations performed on paired_device                 */
    /* NOTE: device status must be updated (as done above) before calling  */ 
    /* SSMA                                                                */ 
    /*---------------------------------------------------------------------*/
    /* device successfully put to sleep - undo any SSMA operations performed
    on the paired device... since the device is asleep, need to use
    RF_CDMA_INVALID_BAND and RF_CDMA_INVALID_CHAN while calling the SSMA
    routines */
    (void) rf_cdma_get_ssma_recos ( device, rxlm_handle, RFM_CDMA_MAX_BAND,
                                    RF_CDMA_INVALID_CHAN,
                                    logical_dev->paired_device );

    (void) rf_cdma_update_bb_ssma_recos( logical_dev->paired_device );

     rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE );
    /* no wait_time required (set to 0) */
    ret_val = 0;
  }
  else
  {
    ret_val = RFM_CDMA_ERROR_FAILURE;
  }

  /* Throw F3 Message */
  RF_MSG_2( RF_MED, "rf_1x_mc_timed_sleep_rx :  Dev %d, RxLM %d", device,
            rxlm_handle);

  return ret_val;

} /* rf_1x_mc_timed_sleep_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Complete the timed sleep rx procedure

  @details
  This second half of timed sleep rx will be called once RF receives the
  response from FW for RX_STOP command.
 
  This function will basically disable the VREG, and reset the CDMA data
  structure to sleep state.

  @param device
  The receive path to put to sleep.
 
  @param timed_sleep_success
  Flag indicating if the timed sleep Rx was successful in FW side or not.
 
  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_1X_RX_TIMED_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
 
  @return
  Flag indicating, if the timed sleep rx is completed successfully or not
*/
rfm_wait_time_t
rf_1x_mc_complete_timed_sleep_rx
(
  const rfm_device_enum_type device,
  boolean timed_sleep_success,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE;
  boolean dev_voting_status; /* Need the flag to perform device voting */

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Pointer */

  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get CDMA Data Write Pointer*/
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Check if timed_sleep was successfully done by FW */
  if ( timed_sleep_success == TRUE )
  {
    /* NULL pointer check */
    if ( dev_status_r == NULL || ant_tuner_token_w == NULL )
    {
      RF_MSG( RF_ERROR, "rf_1x_mc_complete_timed_sleep_rx: NULL RF Dev data" );
      ret_val = RFM_CDMA_ERROR_NULL_DATA;
    } /* if ( dev_status_w == NULL ) */
    else
    {
      const rfc_cdma_logical_device_params_type *logical_dev =
        rfc_cdma_get_logical_device_params( device );
      const rfm_cdma_band_class_type band = dev_status_r->curr_band;

      if ( logical_dev != NULL && logical_dev->ant_tuner[band] != NULL )
      {
        rf_cdma_atuner_ack_script( logical_dev->ant_tuner[band], 
                                   ant_tuner_token_w );
      }

      /* RF state should be in Sleep Scheduled state to execute this function */
      if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_SLEEP_SCHEDULED )
      {

        /* This function releases all the TQs and TQs are required to 
        queue any read\writes to CCS so this function call should be 
        after all device read or writes. */ 
        rf_cdma_mdsp_modem_rx_deallocate_resources ( device , RFM_1X_MODE ,
                                     rf_cdma_data_get_rxlm_handle (device) ) ;

        /* Vote OFF for all RF Devices which are not needed with RFM Device 
           This API call help to vote ON/OFF for TCXO buffer and NPLER BYPASS 
           settings */
        dev_voting_status = rfcommon_core_dev_power_vote(device,
                                                         RFCMN_CORE_DEV_OFF,
                                                         RFCMN_CORE_DEV_NO_VOTE);
        if( dev_voting_status == FALSE )
        {
          RF_MSG_1( RF_ERROR, "rf_1x_mc_complete_timed_sleep_rx(): "
                    "Device: %d Pwr Down failed ", device);
          ret_val = RFM_CDMA_ERROR_FAILURE;
        }
        else
        {
          ret_val = RFM_CDMA_HEALTHY_STATE;
        }
  
        /* un-vote for RFCMD app */
        {
          rf_hal_bus_client_handle* handle = 
                                rf_cdma_data_get_rf_hal_bus_vote_handle(device);
          rf_hal_bus_enable (FALSE, &handle, RF_HAL_RFCMD_APP_CLIENT(1X));
          rf_cdma_data_set_rf_hal_bus_vote_handle(device, handle);
        }

        /* Revert the Vreg State, since Rx is going to sleep */
        rfc_cdma_manage_vregs( device, RFM_1X_MODE, dev_status_r->curr_band,
                               RF_PATH_SLEEP_STATE);
      
        /* Update CDMA Data structure with new RF State and  band/Chan */
        rf_cdma_reset_sw_state ( device ) ;

        if( ret_val >= RFM_CDMA_HEALTHY_STATE )
        {
          /* Flag Callback event as successful */
          cb_event = RFM_1X_RX_TIMED_SLEEP_COMPLETE;
        }
        else if ( ret_val == RFM_CDMA_ERROR_FAILURE )
        {
          /* Flag Callback event as unsuccessful */
          cb_event = RFM_EXECUTION_FAILURE;      
        }
      }
      else
      {
        ret_val = RFM_CDMA_ERROR_INVALID_DEV_STATE;
      }
    } /* if ! ( dev_status_w == NULL ) */
  } /* if ( timed_sleep_success == TRUE ) */
  else
  {
    /* FW failed to complete the Timed Rx Sleep action successfully. Thus
    flag the callback event accordingly. */
    cb_event = RFM_1X_RX_TIMED_SLEEP_FW_FAILED;

    /* Since FW failed to put the radio to sleep, we are moving the
    RF State back to Rx State. */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_RX );

    /* Since this is an expected behaviour from RFM API, flagging the return
    value as successful */
    ret_val = RFM_CDMA_HEALTHY_STATE;

    RF_MSG_1( RF_HIGH, "rf_1x_mc_complete_timed_sleep_rx : Dev %d - FW failed"
                       "to complete. 1xL1 will try regular sleep", device );
  } /* if ! ( timed_sleep_success == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* Throw F3 Message */
  RF_MSG_2( RF_MED,"rf_1x_mc_complete_timed_sleep_rx : [Status %d] - Dev %d",
            ret_val, device );

  return ret_val;

} /* rf_1x_mc_complete_timed_sleep_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the transmitter.

  @details
  Moves the 1x radio from the Rx state to the RxTx state.  Powers on all
  circuits required to transmit a 1x signal, and insures the transmitter
  is tuned to the correct band and channel.

  The 1x transmitter is controlled by hardware and software signals;
  calling this API does not guarantee the RF chipset will be emitting
  RF power.  The modulator hardware must also be enabled and actively
  controlling the TX_ON and PA_ON hardware controls.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_TX_WAKEUP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  @param device
  The transmit path to wakeup.

  @param txlm_handle
  The Tx Link Manager buffer that shall be used to configure the
  modem hardware.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_wakeup_tx()
  function return until the operation is complete.
*/
rfm_wait_time_t
rf_1x_mc_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle
)
{
  rfm_wait_time_t ret_val; /* Return value */
  boolean wakeup_success = TRUE; /* Flag to track the success of wakeup tx */

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  rf_cdma_temp_comp_data_type *temp_comp_w; /* Temp comp write pointer*/
  rf_cdma_hdet_data_type *hdet_w; /* HDET write pointer*/
  rf_cdma_plim_data_type *plim_w; /* PLIM write pointer*/
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_l_read; /* TxAGC Read */
  /* <band, chan> to which device is currently tuned to */
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type chan; 
  rfdevice_cdma_dac_cal_iref_type iref_data; /* For getting Iref for TxLM */
  rf_cdma_msm_dynamic_txlm_cfg_type dynamic_cfg;
  rf_cdma_msm_static_txlm_cfg_type static_cfg;
  rfdevice_therm_read_type therm_read;
  uint32 modem_chain;
  uint32 slave_modem_chain;
  rf_tx_resource_mgr_config_data_type txmgr_cfg;
  xpt_cfg_type xpt_cfg;
  rf_cdma_resource_mgr_cb_data_type rmgr_cb_data;
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr; /* Cal data pointer, both static and multi lin */

  /* for fbrx */
  rfcommon_fbrx_error_type fbrx_status = RFCOMMON_FBRX_SUCCESS;
  lm_handle_type fbrx_lm_handle; /* fbrx lm handle */
  uint16 channels[RFM_CDMA_CARRIER_CUMULATIVE];
  rfcom_band_type_u fbrx_band;
  rfdevice_trx_cdma_tx* tx_device = NULL ;

  /* NV related variables */
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* For Tx Spectral inversion */
  rfdevice_cdma_tx_spectral_inversion_type tx_spectral_inversion_data = {RFM_CDMA_MAX_BAND, FALSE};  

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );
  boolean pin_ret;

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mc_wakeup_tx: NULL FWRSP Data" );
    return RFM_CDMA_ERROR_NULL_DATA;
  } /* if ( fwrsp_data == NULL ) */

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get CDMA Data Write Pointer*/
  temp_comp_w = rf_cdma_get_mutable_temp_comp( device );
  hdet_w = rf_cdma_get_mutable_hdet( device );
  plim_w = rf_cdma_get_mutable_plim( device );
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) ||( logical_dev == NULL ) ||
       ( temp_comp_w == NULL ) ||( hdet_w == NULL ) ||
       ( plim_w == NULL ) ||( ant_tuner_token_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_wakeup_tx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_wakeup_tx(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /* Get current band */
  band = dev_status_r->curr_band;
  chan = dev_status_r->curr_chans[RFM_CDMA_CARRIER_0];

  if (band >= RFM_CDMA_MAX_BAND)
  {
    RF_MSG_2 (RF_ERROR, "rf_1x_mc_wakeup_tx: Invalid Sys band %d .. rf_band %d",
              dev_status_r->curr_band, band);
    return RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  }
  
  /* ensure that NV is valid for given device and band */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band], 
                                             band );

  if ( NULL == rf_1x_config_ptr )
  {
    RF_MSG_1 (RF_ERROR, "rf_1x_mc_wakeup_tx: Failed to retrieve valid Config" 
                        " data from NV for band %d", band); 
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  tx_static_nv_cal_ptr = rf_cdma_nv_get_tx_static( 
                           logical_dev->tx_nv_path[band], band );
  if ( tx_static_nv_cal_ptr == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_wakeup_tx: "
                     "Failed to get valid static NV pointer "
                     "for device %d", device);
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  if( ( rfm_get_calibration_state() != TRUE ) && /* If online */
      ( rf_cdma_xpt_is_enabled(device) == TRUE ) && /* xpt enabled */
      ( rf_1x_config_ptr->cdma_common_ptr->tx_cal.tx_multi_lin_cal.nv_rev_type
          != TX_MULTI_LIN_REV_3 ) /* lin ver is not rev 3 */
    )
  {
    RF_MSG_2( RF_ERROR, "rf_1x_mc_wakeup_tx : XPT is enabled. Current lin ver %d"
              " is not the required ver %d",
              (rf_1x_config_ptr->cdma_common_ptr->tx_cal.tx_multi_lin_cal.nv_rev_type+1),
              (TX_MULTI_LIN_REV_3+1) );
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  /*--------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed */
  /*--------------------------------------------------------------------------*/
    /* Wakeup the WTR for tx mode */
  if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
  {
    wakeup_success = rfdevice_cdma_tx_pwr_wakeup(logical_dev->tx_device[band]);
  }

  /* Allocate resources ( Tx buffers, queues, etc) needed for 1x Tx operation 
     and if we cannot obtain DM buffers bail TX because nothing can be 
     written to any of the RF peripherals */ 
  if ( FALSE == rf_cdma_mdsp_modem_tx_allocate_resources ( txlm_handle ) )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_wakeup_tx(): "
               " rf_cdma_mdsp_modem_tx_allocate_resources() Failed to for device %d " , 
               device ) ;
    return RFM_CDMA_ERROR_FAILURE ;
  }

  tx_device = logical_dev->tx_device[band] ;
  if ( NULL == tx_device )
  {
    RF_MSG_2 ( RF_ERROR, "rf_1x_mc_wakeup_tx(): "
               " tx_device is NULL for device: %d , band: %d " , 
               device , band ) ;
    return RFM_CDMA_ERROR_FAILURE ;
  }

  /* Set Modem Chain value */
  modem_chain = rfc_cdma_get_tx_modem_chain(device, band);
  slave_modem_chain = rfc_cdma_get_slave_tx_modem_chain(device, band);

  /* set txlm in rf_cdma_data */
  rf_cdma_data_set_txlm_handle( device, txlm_handle);

  /* Identify if EPT/ET is enabled for Tx operation */
  if ( rf_cdma_xpt_is_enabled(device) == TRUE )
  {
    rf_cdma_data_set_ept_online_enabled( device, TRUE );
  }
  else
  {
    rf_cdma_data_set_ept_online_enabled( device, FALSE );
  }

  if ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE )
  {
  /* Allocate FBRx DM buffers for given FBRxLM handle */
  fbrx_status = rfcommon_fbrx_mc_enter_mode( &fbrx_lm_handle, RFM_1X_MODE);
  if(fbrx_status == RFCOMMON_FBRX_ERROR)
  {
    RF_MSG(RF_ERROR, "rf_1x_mc_wakeup_tx: FBRx DM buffer allocation failed");
    wakeup_success = FALSE;
  }
  else
  {
    /* Update fbrx_lm_handle in rf_cdma_data */
    rf_cdma_data_set_fbrx_lm_handle( device , fbrx_lm_handle );
    RF_MSG_1(RF_MED, "rf_1x_mc_wakeup_tx: FBRx DM buffer allocated handle %d",
                      fbrx_lm_handle);
  }
  } 

  /*--------------------------------------------------------------------------*/
  /* Acquire the Tx resource before proceeding */
  /*--------------------------------------------------------------------------*/
  wakeup_success &= rf_request_master_tx_resource( device, 
                                                   (txlm_chain_type)modem_chain, 
                                                   RFM_1X_MODE );

  /*--------------------------------------------------------------------------*/
  /* Enable Chip Power for all devices in the receiver - Device, RxAGC, RFCard*/
  /*--------------------------------------------------------------------------*/
  rfc_cdma_manage_vregs(device, RFM_1X_MODE, band, RF_PATH_RXTX_STATE);
 
  /*--------------------------------------------------------------------------*/
  /* Do therm read before configuring TX AGC*/
  /*--------------------------------------------------------------------------*/
  rfdevice_cdma_tx_cmd_dispatch( tx_device ,
                                 RFDEVICE_GET_THERMISTER_VALUE,
                                 &therm_read );
  
  temp_comp_w->last_temp_sensor_read = therm_read.temperature;
  temp_comp_w->is_last_reading_valid = therm_read.valid_temp_read;
    
  RF_MSG_2( RF_LOW, "rf_1x_mc_wakeup_tx: therm reading: %d, and %d",
            therm_read.temperature, therm_read.valid_temp_read );
 
  /* Do DAC therm read*/
  rf_cdma_mdsp_iref_therm_read(txlm_handle);
 
  /* Update the temp comp algo for the first read */ 
  wakeup_success &= rf_cdma_temp_comp_update_algo( temp_comp_w,
                                                   logical_dev, band );

  /* Force isHKADCbusy to FALSE*/
  rf_cdma_data_set_isHKADCbusy( device, FALSE );

  /*--------------------------------------------------------------------------*/
  /* Configure TxLM */
  /*--------------------------------------------------------------------------*/

  /* Get Iref info from RF Device */
  iref_data.band = band;
  rfdevice_cdma_tx_get_dac_cal_iref( tx_device , &iref_data ) ;

  dynamic_cfg.iref_val = iref_data.iref_val;
  dynamic_cfg.num_carriers = dev_status_r->num_carriers;
  dynamic_cfg.tx_modem_chain = 
    rfc_cdma_get_tx_modem_chain ( device , dev_status_r->curr_band ) ;
  dynamic_cfg.tx_spectral_inversion = FALSE;
 
  static_cfg.txlm_dac = logical_dev->txlm_dac;
  xpt_cfg = (xpt_cfg_type) rf_cdma_xpt_get_cfg_param(device, XPT_CFG);

  static_cfg.xpt = rf_cdma_msm_get_txlm_xpt_mode( TRUE, xpt_cfg );

  /* Tx Specral inversion query from device */
  tx_spectral_inversion_data.band = band;

  wakeup_success &= rfdevice_cdma_tx_cmd_dispatch( tx_device, 
                                                   RFDEVICE_IS_TX_SPECTRAL_INVERSION_NEEDED,
                                                   &tx_spectral_inversion_data);

  /*check if the wakeup_success failed*/
  if ( wakeup_success == FALSE )
  {
    RF_MSG ( RF_ERROR, "rf_1x_mc_wakeup_tx(): rfdevice_cdma_tx_cmd_dispatch failed"); 
  }

  dynamic_cfg.tx_spectral_inversion = tx_spectral_inversion_data.spectral_inversion;
  
  wakeup_success &= rf_cdma_msm_configure_txlm( rfdevice_id_get(tx_device),
                                                txlm_handle, 
                                                RFM_1X_MODE, 
                                                device,
                                                dev_status_r->curr_band, 
                                                static_cfg,
                                                dynamic_cfg);
  static_cfg.xpt = xpt_cfg;

  /* Retrieve the default TxLM IQ gain */
  rf_cdma_data_set_cdma_iq_gain_val( device, 
      (int16) rf_cdma_msm_get_default_txc_gain ( dynamic_cfg.tx_modem_chain,
                                                 txlm_handle ) );
  rf_cdma_data_set_ept_iq_gain_val( device,  
      (int16) rf_cdma_xpt_get_cfg_param( device, EPT_CFG_CMN_IQ_GAIN_UNITY ) );

  /* Configure mdsp with RF ONOFF information */
  if( wakeup_success == TRUE )
  {
    /* Enter Device Critical Section, so that the device call is thread safe ,
    concurrencies within the WTR can lead to corruption of the buffer
    resulting in failures for scripts that are generated */
    rfdevice_cdma_tx_enter_critical_section ( tx_device ) ;

    /* Execute API to configure On/OFF*/
    wakeup_success &= rf_cdma_mc_configure_tx_on_off( device, band, RFM_1X_MODE,
                                                   dev_status_r->script_index);

    /* Leave Device Critical Section */
    rfdevice_cdma_tx_leave_critical_section( tx_device ) ;

  }/*if( wakeup_success == TRUE )*/

  /*--------------------- FBRx Settings ----------------------------------*/
  if (( wakeup_success == TRUE ) && ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE ))
  {

    /* Allocate Tx DM buffers for the given FBRx LM handle */
    fbrx_lm_handle =  rf_cdma_data_get_fbrx_lm_handle(device);
    fbrx_status = rfcommon_fbrx_mc_init_tx( fbrx_lm_handle, 
                                            RFCOMMON_FBRX_1X, 
                                            txlm_handle );

    /* Updates FBRx frequency-dependent values */
    channels[0] = chan;  
    fbrx_band.cdma_band = band;
    fbrx_status &= rfcommon_fbrx_mc_tune_to_chan( 
                           txlm_handle, 
                           RFM_1X_MODE, 
                           fbrx_band,
                           channels,
                           RFCOMMON_FBRX_1_CARRIER,
                           fbrx_lm_handle );

  }


  /*--------------------------------------------------------------------------*/
  /* Configure TxAGC */
  /*--------------------------------------------------------------------------*/
  wakeup_success &= rf_1x_configure_tx_agc( device, band, 
                                  dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
                                  txlm_handle,
                                  rf_1x_config_ptr );

  /*--------------------------------------------------------------------------*/
  /* Compute RF Front-end for Wakeup and send TX_START to FW                  */
  /*--------------------------------------------------------------------------*/
  rf_cdma_data_set_script_index( device, 
                            rf_cdma_mdsp_get_device_buffer_index(RFM_1X_MODE,
                                                                 txlm_handle) );

  if ( wakeup_success == TRUE )
  {

    /* Enter Device Critical Section, so that the device call is thread safe */
    rfdevice_cdma_tx_enter_critical_section ( tx_device ) ;

    wakeup_success &= rf_cdma_mc_configure_front_end_for_tx_wakeup (
                               device, txlm_handle, RFM_1X_MODE, band, chan, 
                               dev_status_r->rx_bw_khz, RF_CDMA_BW_1X, 
                               dev_status_r->script_index, logical_dev,
                               tx_static_nv_cal_ptr,
                               ant_tuner_token_w );

     /*--------------------- FBRx Settings ----------------------------------*/
    if (( wakeup_success == TRUE ) && ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE ))
    {
      fbrx_lm_handle =  rf_cdma_data_get_fbrx_lm_handle(device);
  
      /* Writes the FBRx modem settings to HW */
      fbrx_status &= rfcommon_fbrx_enable( fbrx_lm_handle );
      if( fbrx_status == RFCOMMON_FBRX_ERROR )
      {
        RF_MSG_1(RF_FATAL, "rf_1x_mc_wakeup_tx: "
                           "FBRx enable failed: handle %d", fbrx_lm_handle);
      }
    }

    if ( wakeup_success == TRUE )
    {
      rf_cdma_mdsp_tx_start_type tx_start_c;
      tx_start_c.script_index = 
        rf_cdma_mdsp_modem_get_script_index( dev_status_r->script_index, 
                                          RF_CDMA_MDSP_CCS_EVENT_WAKEUP_TX_ID );
      /* Set the pout comp flag */ 
      if ( rfcommon_autopin_is_enabled(RFM_1X_MODE) &&
	   rf_cdma_autopin_par_support(device, 
	 	                  logical_dev, 
 	                          RFM_1X_MODE, 
 	                          band))
      {
        rf_cdma_set_pout_comp_skip ( device , TRUE ) ;
      }
      tx_start_c.txlm_handle = txlm_handle; /* TXLM Handle */

      /* Send Command to MDSP to start Tx */
      fwrsp_hk->tx_start_rsp.device = device;
      fwrsp_hk->tx_start_rsp.sema_token = rfcommon_semaphore_pop_item();
      rsp_cb_param.rsp_handler = rf_1x_tx_start_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->tx_start_rsp;

      wakeup_success = rf_1x_mdsp_start_tx( logical_dev->tx_mdsp_path, 
                                    &tx_start_c, &rsp_cb_param, NULL, NULL ); 

      /* Sending of Tx Start to FW failed */
      if ( wakeup_success == FALSE )
      {
        RF_MSG ( RF_ERROR, "rf_1x_mc_wakeup_tx(): rf_1x_mdsp_start_tx failed "); 
        /* Message sending failed, free the semaphore */
        RF_MSG_2 ( RF_LOW, "rf_1x_mc_wakeup_tx():"
                   " Freeing the semaphore and not waiting "   
                   " [D%d] TX_START pushed back : Sem @ 0x%x" , 
                   device , fwrsp_hk->tx_start_rsp.sema_token ) ;
      }
      else
      {
        /* Message was sent to FW, wait for semaphore */
        RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] TX_START : Start Wait Sem @ "
                          "0x%x", device, fwrsp_hk->tx_start_rsp.sema_token );
        rfcommon_semaphore_wait( fwrsp_hk->tx_start_rsp.sema_token );
      }

      /* Push back semaphore after wait if mssg was sent successfully,
      or immediately if mssg sending failed*/
      rfcommon_semaphore_push_item( fwrsp_hk->tx_start_rsp.sema_token );

      if ( logical_dev->ant_tuner[band] != NULL )
      {
        rf_cdma_atuner_ack_script( logical_dev->ant_tuner[band], 
                                   ant_tuner_token_w );
      }

      /* Acquire and Enable the Tx Slave resource before proceeding */
      rmgr_cb_data.cdma_mode = RFM_1X_MODE;
      rmgr_cb_data.xpt_mode = rf_cdma_xpt_get_operation_mode(device);
      rmgr_cb_data.band = band;
      rmgr_cb_data.chan = chan;
      rmgr_cb_data.tx_nv_path = logical_dev->tx_nv_path[band];
      rmgr_cb_data.master_txlm_handle = txlm_handle;
      rmgr_cb_data.static_cfg = static_cfg;
      rmgr_cb_data.dynamic_cfg = dynamic_cfg;

      txmgr_cfg.func_ptrs.init_fp = rf_cdma_resource_mgr_init_xpt;
      txmgr_cfg.func_ptrs.deinit_fp = rf_cdma_resource_mgr_deinit_xpt;
      txmgr_cfg.func_ptrs.prep_fp = rf_cdma_resource_mgr_prep_xpt;
      txmgr_cfg.func_ptrs.setup_fp = rf_cdma_resource_mgr_setup_xpt;
      txmgr_cfg.func_ptrs.cleanup_fp = rf_cdma_resource_mgr_cleanup_xpt;
      txmgr_cfg.cb_data.data_size = sizeof(rf_cdma_resource_mgr_cb_data_type);
      txmgr_cfg.cb_data.data_ptr = &rmgr_cb_data;

      wakeup_success &= 
        rf_request_xpt_tx_resource( device, 
                                      (txlm_chain_type)slave_modem_chain, 
                                      RFM_1X_MODE,
                                      txmgr_cfg );
    } /* if ( wakeup_success == TRUE ) */

    /* Clean up RF CCS event smem and event handle regardless faliure or not*/
    rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_STATIC_EVENT );

    /* Leave Device Critical Section */
    rfdevice_cdma_tx_leave_critical_section( tx_device ) ;


  } /* if ( wakeup_success == TRUE ) */

  /* Check if all Static Initialization went through. If not, return Failure.
     For successful execution, do the Tx Wakeup hardware sequence. */

  if ( wakeup_success == TRUE )
  {
    int32 dummy_result;

    /* Tell GPS that 1x Tx is on. */
    RFGNSS_MC(wwan_tx_status, dummy_result)(device, TRUE);
    if ( dummy_result == FALSE )
    {
      RF_MSG( RF_ERROR, 
              "rf_1x_mc_wakeup_tx: rfgnss_mc API failed !!" );
    }

    /*------------------------------------------------------------------------*/
    /* Start 1x Power limiting and HDET */
    /*------------------------------------------------------------------------*/
    if ( rf_1x_plim_start(device, logical_dev, plim_w, band) == TRUE)
    {
      /* Start HDET only if Power Limiting was started successfully. Else no 
      point doing HDET reads */
      if ( rf_cdma_hdet_start(device, hdet_w, RFM_1X_MODE,
                              logical_dev, band, chan) == FALSE )
      {
          /* Clean up HDET trig event in case of hdet start failure */
          rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_HDET_CONFIG );

          RF_MSG_3( RF_ERROR, "rf_1x_mc_wakeup_tx: HDET start failed on device "
                              "%d, band: %d, chan: %d", device, band, chan );
      }
      else
      {
      /* Update the initial HDET vs Temp params based on this initial therm read */
      rf_cdma_hdet_update_temperature_params( hdet_w, 
                            dev_status_r->temp_comp.last_temp_sensor_read );
      }

      /* Update lim_vs_temp_vs_freq */
      rf_1x_plim_update_lim_vs_temp_vs_freq( 
                            device,
                            plim_w,
                            dev_status_r->plim.band,
                            dev_status_r->lo_tuned_to_chan,
                            dev_status_r->temp_comp.last_temp_sensor_read );
    }
    else
    {
      RF_MSG_2( RF_HIGH, "rf_1x_mc_wakeup_tx: 1x PLIM not started on device %d,"
                  "band: %d", device, band );
    }

    /*------------------------------------------------------------------------*/
    /* Start 1x temp comp routine */
    /*------------------------------------------------------------------------*/
    if ( rf_1x_temp_comp_start( temp_comp_w, RFM_1X_MODE,
                                logical_dev, band, chan) == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_1x_mc_wakeup_tx: temp comp start failed ");
    }
    else
    {
      RF_MSG_1( RF_MED, "rf_1x_mc_wakeup_tx: temp comp starts at device %d", 
                device); 
    }
	if (rfcommon_autopin_is_enabled(RFM_1X_MODE))
	{
		 if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_enable_tx(txlm_handle, RFM_1X_MODE))
		 {
		   RF_MSG(RF_ERROR, "rf_1x_mc_wakeup_tx: CDMA 1X AutoPin enable TX failed!");
		   fbrx_status &= FALSE;
		 }
		 else
		 {
		   rf_cdma_auto_pin_tx_wakeup(device, RFM_1X_MODE);
	
	   //Update the DM memory
	   (void)rf_cdma_autopin_update_dm (device,
						 logical_dev,  
					 RFM_1X_MODE,
							 txlm_handle,
								 band, 
					 chan);
		 }
	  }
	  else
	  {
		 RF_MSG(RF_ERROR, "rf_1x_mc_wakeup_tx: CDMA 1X AutoPin is not enaBLED!");
	  }

    /*------------------------------------------------------------------------*/
    /* Build script for 1x Autopin  */
    /*------------------------------------------------------------------------*/

      if ( rfcommon_autopin_is_enabled(RFM_1X_MODE) &&
	        rf_cdma_autopin_par_support(device, 
 	                          logical_dev, 
	                          RFM_1X_MODE, 
 	                          band ) == TRUE)
      {
      	  pin_ret = rf_cdma_configure_autopin_trigger (
   				    device,
  				    RFM_1X_MODE,
				    logical_dev,
			    	    band);
	  if (pin_ret == FALSE)
    	  {
            RF_MSG( RF_ERROR, "rf_1x_mc_wakeup_tx: pin configure failed ");
		  
    	  }
        }
    
    /* Update CDMA Data structure with new the RF State */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_RXTX );
    /* Populate the Tx Bandwidth */
    rf_cdma_data_set_tx_bw_khz( device, RF_CDMA_BW_1X );

    /* Populate the total wait time */
    ret_val = 0;

    /* Get TxAGC Data for reporting */
    rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path, 
                                        dev_status_r->txlm_handle, 
                                        &tx_agc_l_read );

    rf_cdma_data_sanity_check( device );

    /* Report wakeup time, Tx AGC and PA state */
    RF_MSG_5( RF_MED, "rf_1x_mc_wakeup_tx : Dev %d, Band %d, Chan %d, "
              "PA State %d, TxTotal %d", device, band,
              dev_status_r->curr_chans[RFM_CDMA_CARRIER_0], 
              tx_agc_l_read.pa_state, tx_agc_l_read.total_power );
  } /* ( wakeup_success == TRUE ) */
  else
  {
    /* Revert the Vreg State, since Tx is not waking up */
    rfc_cdma_manage_vregs( device, RFM_1X_MODE, band, RF_PATH_RX_STATE);

    /* Sleep the WTR for tx mode , since wakeup tx failed */
	if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
	{
      wakeup_success = rfdevice_cdma_tx_pwr_sleep(logical_dev->tx_device[band]);
    }

    /* set txlm in rf_cdma_data */
    rf_cdma_data_set_txlm_handle( device, RFM_INVALID_LM_BUFFER);

    if ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE )
    {
    /* Deallocate FBRx DM buffers for given FBRxLM handle  */
    fbrx_status = rfcommon_fbrx_mc_exit( dev_status_r->fbrx_lm_handle,
                                         RFM_1X_MODE );

    RF_MSG_1( RF_MED, "rf_1x_mc_wakeup_tx: Failed wakeup: "
                      "Calling rfcommon_fbrx_mc_exit with "
              "fbrx_handle %d, RFM_1X_MODE", dev_status_r->fbrx_lm_handle );

    if(fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      wakeup_success = FALSE;
      RF_MSG(RF_ERROR, "rf_1x_mc_wakeup_tx: FBRx DM buffer deallocation failed");
    }
    else
    {
      /* Update fbrx_lm_handle in rf_cdma_data */
      rf_cdma_data_set_fbrx_lm_handle( device , RFLM_DM_INVALID_HANDLE_ID );
    }
    }
    /* Reset the pout comp flag */
    rf_cdma_set_pout_comp_skip ( device , FALSE ) ;
    ret_val = RFM_CDMA_ERROR_FAILURE;

    /* Report wakeup time, Tx AGC and PA state */
    RF_MSG_2( RF_ERROR, "rf_1x_mc_wakeup_tx: [Status %d] - Dev %d ",
              ret_val, device );
  }

  return ret_val;

} /* rf_1x_mc_wakeup_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the transmitter.

  @details
  Moves the 1x radio from the RxTx state to the Rx state.  Any circuits
  that are no longer needed are powered down to conserve current.

  This API will disable the RF ASIC transmitter, and prevent any power
  from being radiated from the device independent of the TX_ON and PA_ON
  control signals.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_TX_SLEEP_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.

  @param device
  The transmit path to put to sleep.

  @param txlm_handle
  The Tx Link Manager buffer that shall be used to configure the
  modem hardware.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_sleep_tx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure
*/
rfm_wait_time_t
rf_1x_mc_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle
)
{
  rfm_wait_time_t ret_val = 0; /* Return value */
  boolean sleep_success = TRUE; /* Flag to track the success of wakeup tx */
  rfm_cdma_band_class_type band;  /* Band */

  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  rf_cdma_temp_comp_data_type *temp_comp_w; /* Temp comp write pointer*/
  rf_cdma_hdet_data_type *hdet_w; /* HDET write pointer*/
  rf_cdma_plim_data_type *plim_w; /* PLIM write pointer*/
  ant_tuner_device_tokens_type* ant_tuner_token_w; /*Tuner token write pointer*/

  uint32 modem_chain;
  uint32 slave_modem_chain;
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_mc_sleep_tx: NULL FWRSP Data" );
    return RFM_CDMA_ERROR_NULL_DATA;
  } /* if ( fwrsp_data == NULL ) */

  /* Get Device Status Read Pointer*/
  dev_status_r = rf_cdma_get_device_status( device );
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get CDMA Data Write Pointer*/
  temp_comp_w = rf_cdma_get_mutable_temp_comp( device );
  hdet_w = rf_cdma_get_mutable_hdet( device );
  plim_w = rf_cdma_get_mutable_plim( device );
  ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( device );

  /* Perform NULL Pointer check */
  if ( ( dev_status_r == NULL ) || ( logical_dev == NULL ) ||
       ( temp_comp_w == NULL ) ||( hdet_w == NULL ) ||
       ( plim_w == NULL ) ||( ant_tuner_token_w == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_sleep_tx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }
  
  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_sleep_tx(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /*-------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed */
  /*-------------------------------------------------------------------------*/

  /* Populate Current band and chan */
  band = dev_status_r->curr_band;
  
  /* Set Modem Chain value */
  modem_chain = rfc_cdma_get_tx_modem_chain(device, band);
  slave_modem_chain = rfc_cdma_get_slave_tx_modem_chain(device, band);
  
  /*-------------------------------------------------------------------------*/
  /* Release the Tx Slave resource before proceeding */
  /*-------------------------------------------------------------------------*/
  sleep_success &= rf_release_xpt_tx_resource( device, 
                                           (txlm_chain_type)slave_modem_chain, 
                                            RFM_1X_MODE );

  /*--------------------------------------------------------------------------*/
  /* Stop Power Limiting */
  /*--------------------------------------------------------------------------*/
  sleep_success &= rf_1x_plim_stop( device, plim_w );
  
  /*--------------------------------------------------------------------------*/
  /* Stop 1x temp comp routine*/
  /*--------------------------------------------------------------------------*/
  if ( rf_cdma_debug_flags.disable_temp_comp == 0 )
  {
    sleep_success &= rf_1x_temp_comp_stop( temp_comp_w );
  }

  /*--------------------------------------------------------------------------*/
  /* Stop HDET */
  /*--------------------------------------------------------------------------*/
  sleep_success &= rf_cdma_hdet_stop( device, hdet_w );

  RF_MSG_1( RF_MED, "rf_1x_mc_sleep_tx(): stop temp_comp timer %d", 
            sleep_success ); 

  /* Force isHKADCbusy to FALSE*/
  rf_cdma_data_set_isHKADCbusy( device, FALSE );
  
  /* --------------------------- END OF STATIC INIT ------------------------*/
  /* No Static de-initialization can be done after this point. Only wakeup 
     hardware sequence is executed */
  /*--------------------------------------------------------------------------*/
  
  /*--------------------------------------------------------------------------*/
  /* Configure RF-front-end for Sleep and send RX_STOP to FW                  */
  /*--------------------------------------------------------------------------*/
  rf_cdma_data_set_script_index( device, 
                          rf_cdma_mdsp_get_device_buffer_index( RFM_1X_MODE,
                                                                txlm_handle ) );

  if ( sleep_success == TRUE )
  {
    /* Enter Device Critical Section, so that the device call is thread safe */
    rfdevice_cdma_tx_enter_critical_section( logical_dev->tx_device[band] );

    sleep_success &= rf_cdma_mc_configure_front_end_for_tx_sleep ( 
                                   device, txlm_handle,
                                   dev_status_r->curr_band,
                                   dev_status_r->script_index, 
                                   logical_dev,
                                   ant_tuner_token_w );

    if( dev_status_r->ept_online_enabled == TRUE )
    {
      sleep_success &= rf_common_xpt_init_dpd( device );
    }

    if (sleep_success == TRUE)
    {
      rf_cdma_mdsp_tx_stop_type tx_stop_c; /* TxAGC Stop Config */

      tx_stop_c.script_index = rf_cdma_mdsp_modem_get_script_index (
          dev_status_r->script_index ,  RF_CDMA_MDSP_CCS_EVENT_SLEEP_TX_ID ) ;

      tx_stop_c.txlm_handle = txlm_handle; /* TXLM Handle */

      /* Send Command to MDSP to stop Tx */
      fwrsp_hk->tx_stop_rsp.device = device;
      fwrsp_hk->tx_stop_rsp.sema_token = rfcommon_semaphore_pop_item();
      rsp_cb_param.rsp_handler = rf_1x_tx_stop_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->tx_stop_rsp;

      sleep_success = rf_1x_mdsp_stop_tx( logical_dev->tx_mdsp_path, 
                                          &tx_stop_c, &rsp_cb_param, NULL, 
                                          NULL );

      /* Sending of Tx Stop to FW failed */
      if ( sleep_success == FALSE )
      {
        RF_MSG ( RF_ERROR, "rf_1x_mc_sleep_tx(): rf_1x_mdsp_stop_tx failed "); 
        /* Message sending failed, free the semaphore */
        RF_MSG_2 ( RF_LOW, "rf_1x_mc_sleep_tx():"
                   " Freeing the semaphore and not waiting "   
                   " [D%d] TX_STOP pushed back : Sem @ 0x%x" , 
                   device , fwrsp_hk->tx_stop_rsp.sema_token ) ;
      }
      else
      {
        /* Message was sent to FW, wait for semaphore */
        RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] TX_STOP : Start Wait Sem @ "
                          "0x%x", device, fwrsp_hk->tx_stop_rsp.sema_token );
        rfcommon_semaphore_wait( fwrsp_hk->tx_stop_rsp.sema_token );
      }

      /* Push back semaphore after wait if mssg was sent successfully,
      or immediately if mssg sending failed*/
      rfcommon_semaphore_push_item( fwrsp_hk->tx_stop_rsp.sema_token );

      /*----------------------------------------------------------------------*/
      /* Release the Tx resource before proceeding */
      /*----------------------------------------------------------------------*/
      sleep_success &= 
        rf_release_master_tx_resource( device, 
                                       (txlm_chain_type)modem_chain, 
                                       RFM_1X_MODE );

    } /* if ( sleep_success == TRUE ) */

    /* Clean up static event smem and event handle regardless faliure or not*/
    rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_STATIC_EVENT );

    /* Leave Device Critical Section */
    rfdevice_cdma_tx_leave_critical_section( logical_dev->tx_device[band] );
    
    /* [BEGIN] ---------- FRBX related operations ------- */    
    if ( rf_cdma_data_get_fbrx_enable (RFM_1X_MODE) == TRUE )
    {
      rfcommon_fbrx_error_type fbrx_status; /* fbrx opreation status */

      /* Deallocate FBRx DM buffers for given FBRxLM handle  */
      fbrx_status = rfcommon_fbrx_mc_exit( dev_status_r->fbrx_lm_handle,
                                           RFM_1X_MODE );

      RF_MSG_1( RF_MED, "rf_1x_mc_sleep_tx: Calling rfcommon_fbrx_mc_exit with "
                "fbrx_handle %d, RFM_1X_MODE", dev_status_r->fbrx_lm_handle );

      if(fbrx_status == RFCOMMON_FBRX_ERROR)
      {
        sleep_success = FALSE;
        RF_MSG( RF_ERROR, 
                "rf_1x_mc_sleep_tx: FBRx DM buffer deallocation failed" );
      }
      else
      {
        /* Update fbrx_lm_handle in rf_cdma_data */
        rf_cdma_data_set_fbrx_lm_handle( device , RFLM_DM_INVALID_HANDLE_ID );
      }
    } /* ---------- FRBX related operations ------- [END] */  
    if (rfcommon_autopin_is_enabled(RFM_1X_MODE))
    {
       rfcommon_fbrx_error_type fbrx_status;
       if(RFCOMMON_AUTOPIN_ERROR == 
                rfcommon_autopin_mc_disable_tx( dev_status_r->txlm_handle, RFM_1X_MODE ))
       {
          RF_MSG(RF_ERROR, "rf_1x_mc_sleep_tx: CDMA 1X AutoPin enable TX failed!");
          fbrx_status = RFCOMMON_FBRX_ERROR;
        }
        else
        {
          /* Stop Autopin */
          rf_cdma_auto_pin_tx_sleep(device, RFM_1X_MODE);
          rf_cdma_autopin_stop( device );
        }
     }  
  } /* if ( sleep_success == TRUE ) */

  /* Deallocate RFLM Tx resources: Abort any pending AOL event */
  rf_cdma_mdsp_modem_tx_deallocate_resources( RFM_1X_MODE, txlm_handle );

  /* Check if all Static Initialization when through. If not, return Failure.
     For successful execution, do the Rx sleep hardware sequence. */
  if ( sleep_success == TRUE )
  {
    int32 dummy_result;
    boolean device_power_down_flag;
    
    /* Reset the pout comp flag */   
    rf_cdma_set_pout_comp_skip ( device , FALSE ) ;

    /* Revert the Vreg State, since Tx is going to sleep */
    rfc_cdma_manage_vregs( device, RFM_1X_MODE, dev_status_r->curr_band,
                           RF_PATH_RX_STATE);

    /* Sleep the WTR for tx mode */
	if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
    {
      device_power_down_flag = 
                      rfdevice_cdma_tx_pwr_sleep(logical_dev->tx_device[band]);
    }
	else
	{
      device_power_down_flag = TRUE;
    }

    if ( device_power_down_flag == FALSE )
    {
      /* Report error */
      RF_MSG_1( RF_ERROR,"rf_1x_mc_sleep_tx: rfdevice_cdma_tx_pwr_sleep API "
              "failed for device: %d ",device ); 
    }

    /* Update CDMA Data structure with new RF State and  band/Chan */
    rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_RX);
    rf_cdma_data_set_txlm_handle( device, RFM_INVALID_LM_BUFFER );
    rf_cdma_data_set_tx_bw_khz( device, RF_CDMA_BW_INVALID );

    /* Tell GPS that 1x Tx is off. */
    RFGNSS_MC(wwan_tx_status, dummy_result)(device, FALSE);
    if ( dummy_result == FALSE )
    {
      RF_MSG( RF_ERROR, 
              "rf_1x_mc_sleep_tx: rfgnss_mc API failed !!" );
    }

    rf_cdma_data_sanity_check( device );

    /* no wait_time needed (set to 0)*/ 
    ret_val = 0;
  } /* if (sleep_success == TRUE) */
  else
  {
    ret_val = RFM_CDMA_ERROR_FAILURE;

    /* Throw error Message */
    RF_MSG_2( RF_ERROR, "rf_1x_mc_sleep_tx: [Status %d] Dev %d", 
              ret_val, device ); 
  }

  return ret_val;

} /* rf_1x_mc_sleep_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Associate a new receive device with a primary control device for
  diversity operation.

  @details
  This API links the 1x state of slave_dev to the state of device1.
  Device2 will track the operating frequency and Sleep/Rx state of
  device1.  1x L1 need not make any calls to directy control slave_dev
  until diversity mode is terminated with a call to
  rfm_1x_disable_diversity().

  If master_dev is in Rx or RxTx mode, slave_dev is woken up if necessary,
  and linked to the operating frequency of device1.  If device1 is
  in sleep mode, slave_dev remains in sleep mode and will be woken up
  when device1 is woken up.

  Any tuning opeations on device1 will be mirrored on slave_dev while
  they are linked in diversity mode.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_ENABLE_DIV_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param master_dev
  The radio device that will be the master.  Device2 is linked as a
  slave to this device.

  @param rxlm_handle_master_dev
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware for device1, if necessary.

  @param slave_dev
  The radio device that will be slaved to device1.

  @param rxlm_handle_slave_dev
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware for slave_dev.
 
  @return
  The anticipated time in microseconds remaining from the
  rfm_1x_enable_diversity() function return until the operation is
  complete.

*/
rfm_wait_time_t
rf_1x_mc_enable_diversity
(
  const rfm_device_enum_type master_dev,
  const lm_handle_type rxlm_handle_master_dev,
  const rfm_device_enum_type slave_dev,
  const lm_handle_type rxlm_handle_slave_dev
)
{
  rfm_wait_time_t ret_val; /* Return value */
  rfm_cdma_power_mode_type power_mode; /* var to store Power Mode */
  uint32 adc_clock_freq; /* var to store ADC Clock Frequency */
  boolean wakeup_success = TRUE; /* Flag to track the success of enable_div */ 
  rf_1x_mdsp_rx_agc_read_type rx_agc_read;/* RxAGC Data */

  const rf_cdma_data_status_type *m_dev_status; /* Master device status */
  const rf_cdma_data_status_type *s_dev_status; /* Slave device status */
  rf_cdma_data_status_type *s_dev_status_w;/* Mutable Slave device status */
  const rfc_cdma_logical_device_params_type *logical_m_dev; /* Logical Master Device */
  const rfc_cdma_logical_device_params_type *logical_s_dev; /* Logical Slave Device */

  rf_cdma_ic_data_type *s_ic_data_w; /* Intelliceiver write pointer*/
  ant_tuner_device_tokens_type* s_ant_tuner_token_w; /*Slave tuner token write pointer*/

  rxf_get_iqmc_coef_type iqmc_data; /* IQMC Coefficient */  
  rf_cdma_msm_static_rxlm_cfg_type static_cfg; /* Static RXLM  Settings */
  rf_cdma_msm_dynamic_rxlm_cfg_type dyn_settings; /* Dynamic RXLM  Settings */
  const rf_1x_config_type* cdma_config_ptr; /* Pointer to NV data */
  rf_1x_agc_param_type agc_mode_info; /* 1x RxAGC Mode info */

  /* Flag to track the success of wakeup rx */
  boolean dev_voting_status;

  rf_time_tick_type prof_vreg_t;      /* Vreg Time Profile */
  rf_time_tick_type prof_rxlm_t;      /* RXLM Time Profile */
  rf_time_tick_type prof_rxagc_cfg_t; /* RxAGC Config Time Profile */

  rf_time_type prof_vreg_t_val;      /* Vreg Time Profile */
  rf_time_type prof_rxlm_t_val;      /* RXLM Time Profile */
  rf_time_type prof_rxagc_cfg_t_val; /* RxAGC Config Time Profile */
  rfm_cdma_band_class_type band;

  /* Get Device Status for master and slave devices */
  m_dev_status = rf_cdma_get_device_status( master_dev );
  s_dev_status = rf_cdma_get_device_status( slave_dev );
  s_dev_status_w = rf_cdma_get_mutable_device_status( slave_dev );

  /* Get Logical Devices Param */
  logical_m_dev = rfc_cdma_get_logical_device_params( master_dev );
  logical_s_dev = rfc_cdma_get_logical_device_params( slave_dev );

  /* Get CDMA Data Write Pointer*/
  s_ic_data_w = rf_cdma_get_mutable_intelliceiver( slave_dev );
  s_ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( slave_dev );

  /* Perform NULL Pointer check */
  if ( ( m_dev_status == NULL ) || ( s_dev_status == NULL ) ||
       ( s_dev_status_w == NULL ) ||
       ( logical_m_dev == NULL ) || ( logical_s_dev == NULL ) || 
       ( s_ic_data_w == NULL ) || ( s_ant_tuner_token_w == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_mc_enable_diversity: NULL data for device" );
    return RFM_CDMA_ERROR_NULL_DATA;
  }
  
  /* Validate Rx mdsp path */
  if ( logical_s_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_enable_diversity(): Invalid rx_mdsp_path %d",
              logical_s_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /* Get current band */
  band = m_dev_status->curr_band;

  if ( band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_enable_diversity: Invalid band %d ", band );
    return RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  }

  if ( m_dev_status->num_carriers >= RFM_CDMA_CARRIER_NUM )
  {
    RF_MSG_1 ( RF_ERROR, "rf_1x_mc_enable_diversity: Invalid num_carriers %d ", 
               m_dev_status->num_carriers );
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  /* Get NV for the requested device and band and check if it is NULL */
  cdma_config_ptr = rf_cdma_nv_get_1x_data( 
                             logical_s_dev->rx_nv_path[m_dev_status->curr_band],
                             m_dev_status->curr_band );

  if ( cdma_config_ptr == NULL )
  {
    /* no NV for the requested device and band - bail out */
    RF_MSG_2( RF_ERROR, "rf_1x_mc_enable_diversity: NULL NV for device %d on "
                        "band = %d", slave_dev, m_dev_status->curr_band);
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  /*--------------------------------------------------------------------------*/
  /* all initial checks passed - safe to proceed */
  /*--------------------------------------------------------------------------*/

  /* -------------------------- Update Alt. Path ---------------------------*/
  wakeup_success &= rf_cdma_mc_update_alt_rx_path( RFM_1X_MODE, slave_dev,
                                                  m_dev_status->curr_band ,
                                                  FALSE /* No div in IRAT */);
  /* ------------------------End of Update Alt. Path -----------------------*/

  rf_cdma_mdsp_modem_rx_allocate_resources( slave_dev, rxlm_handle_slave_dev,
                                            RFM_1X_MODE , FALSE /* IRAT == FALSE */ ) ;

  /*--------------------------------------------------------------------------*/
  /* Enable Chip Power for all devices in the receiver - Device, RxAGC, RFCard*/
  /*--------------------------------------------------------------------------*/
  prof_vreg_t = rf_time_get_tick();
  rfc_cdma_manage_vregs( slave_dev, RFM_1X_MODE, m_dev_status->curr_band,
                         RF_PATH_RX_STATE);
  
  /* Vote ON for all RF Devices which as needed with RFM Device (device) 
     This API call help to vote ON/OFF for TCXO buffer and NPLER BYPASS settings */
  dev_voting_status = rfcommon_core_dev_power_vote(slave_dev,
                                                   RFCMN_CORE_DEV_ON,
                                                   RFCMN_CORE_DEV_NORMAL_VOTE);
  if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
  {
    dev_voting_status &= 
                 rfdevice_cdma_rx_pwr_wakeup(logical_s_dev->rx_device[band]);
  }

  if( dev_voting_status == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_enable_diversity(): Device:%d Pwr UP failed",
                        slave_dev);
  }
  wakeup_success &= dev_voting_status;

  prof_vreg_t_val = rf_time_get_elapsed( prof_vreg_t, RF_USEC );

  /* init dynamic settigs before any operations */
  rf_cdma_msm_init_dyn_settings(&dyn_settings);

  /* init band/chan and carrier info for slave device */
  rf_cdma_init_freq_info_for_device(slave_dev);

  /* set rxlm handle */
  rf_cdma_data_set_rxlm_handle( slave_dev , rxlm_handle_slave_dev );

  /*--------------------------------------------------------------------------*/
  /* Determine Power Mode for desired band and channel */
  /*--------------------------------------------------------------------------*/
  power_mode = rf_cdma_ic_get_starting_power_mode( 
                                                s_ic_data_w, 
                                                m_dev_status->curr_band,
                                                m_dev_status->lo_tuned_to_chan,
                                                m_dev_status->rx_bw_khz );

  /*--------------------------------------------------------------------------*/
  /* Determine ADC clock frequency */
  /*--------------------------------------------------------------------------*/
  adc_clock_freq = rf_cdma_msm_get_adc_samp_freq( rxlm_handle_slave_dev, 
                                        m_dev_status->curr_band, 
                                        m_dev_status->lo_tuned_to_chan );

  /* Populate static settings */
  static_cfg.adc_clock_freq = adc_clock_freq;
  static_cfg.bw_khz = m_dev_status->rx_bw_khz;
  static_cfg.rxlm_adc = logical_s_dev->rxlm_adc;
  static_cfg.config_as_et = FALSE;

  /*--------------------------------------------------------------------------*/
  /* Configure RxLM */
  /*--------------------------------------------------------------------------*/

  /* Populate the IQMC Data for query */
  iqmc_data.lin = power_mode;
  iqmc_data.band = m_dev_status->curr_band;
  iqmc_data.chan = m_dev_status->lo_tuned_to_chan;
  iqmc_data.sec_chain_rf_state = RF_CDMA_STATE_RXDIV;
  iqmc_data.bw_khz = static_cfg.bw_khz;

  /* Get RSB coefficients from device driver */
  rfdevice_cdma_rx_cmd_dispatch( logical_s_dev->rx_device[iqmc_data.band],
                                 RFDEVICE_GET_IQMC_COEFS, &iqmc_data);

  /* init dynamic settigs before any operations */
  rf_cdma_msm_init_dyn_settings(&dyn_settings);

  dyn_settings.apply_dynamic_cfg = TRUE;
  /*--------------------------------------------------------------------------*/
  /* Get SSMA recos for slave device, this not only gets Rx recos, but also 
  updates the state machine for the slave device with the relevant info */ 
  wakeup_success &= rf_cdma_get_ssma_recos ( slave_dev, rxlm_handle_slave_dev, 
                                             m_dev_status->curr_band, 
                                             m_dev_status->lo_tuned_to_chan,
                                             logical_s_dev->paired_device );

  /* now read the spur-info table in NV to see if any spurs need to be
     supressed *** The notches will be suppressed when RXLM is configured     */
  wakeup_success &= rf_cdma_get_spurs_to_suppress( slave_dev, 
                    rxlm_handle_slave_dev, m_dev_status->curr_band, 
                    m_dev_status->lo_tuned_to_chan, 
                    m_dev_status->num_carriers, &dyn_settings );

  /* Update the number of notches used on this device (for tracking use 
  for SSMA callback) 1x has only one carrier ,use index zero */
  s_dev_status_w->ssma_cb_data.num_notch_filters_used = 
                  dyn_settings.notch_filt_settings[0].num_notch_filters_used;

  dyn_settings.iqmc_coeff_in = iqmc_data.iqmc_a_coeff;
  dyn_settings.iqmc_coeff_quad = iqmc_data.iqmc_b_coeff;
  dyn_settings.num_carriers = m_dev_status->num_carriers;
  dyn_settings.modem_chain =
  rfc_cdma_get_rx_modem_chain ( slave_dev , m_dev_status->curr_band ) ;
  
  rf_cdma_compute_carrier_info( 
                                m_dev_status->curr_band,
                                m_dev_status->curr_chans,
                                m_dev_status->num_carriers, 
                                m_dev_status->carrier_index, 
                                &dyn_settings.carrier_mask,
                                dyn_settings.freq_offset_hz,
                                0 /* no need to add anything to calculated
                                     freq offset */
                              );
  /* Configure Rx Link Manager Buffer for given Device, Power Mode, and
      ADC Sample Frequency */
  prof_rxlm_t = rf_time_get_tick();
  wakeup_success &= rf_cdma_msm_configure_rxlm( rfdevice_id_get(logical_s_dev->rx_device[band]),
                                                rxlm_handle_slave_dev, RFM_1X_MODE,
                                                m_dev_status->curr_band,
                                                power_mode, static_cfg, dyn_settings );
  prof_rxlm_t_val = rf_time_get_elapsed( prof_rxlm_t, RF_USEC );

  /*----------------------------------------------------------------------*/
  /* Configure RxAGC */
  /*----------------------------------------------------------------------*/

  /* Populate the AGC Mode */
  agc_mode_info.mode = RF_1X_AGC_MODE_NORMAL;
  agc_mode_info.acq_duration = rf_cdma_msm_get_1x_default_agc_acq_duration();

  prof_rxagc_cfg_t = rf_time_get_tick();

  wakeup_success &= rf_1x_configure_rx_agc( slave_dev, rxlm_handle_slave_dev,
                                            m_dev_status->curr_band, 
                                            m_dev_status->lo_tuned_to_chan, 
                                            power_mode, RF_CDMA_STATE_RXDIV,
                                            &agc_mode_info );

  prof_rxagc_cfg_t_val = rf_time_get_elapsed( prof_rxagc_cfg_t, RF_USEC );
  
  /*------------------------- END OF STATIC INIT -------------------------*/
  /* No Static de-initialization can be done after this point. Only Sleep 
     hardware sequence is executed                                        */
  /*----------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  /* Configure RF Front-end and send RX_START to FW                           */
  /*--------------------------------------------------------------------------*/
  rf_cdma_data_set_script_index( slave_dev, 
                  rf_cdma_mdsp_get_device_buffer_index(RFM_1X_MODE,
                                                       rxlm_handle_slave_dev) );

  if ( wakeup_success == TRUE )
  {
    /* Enter Device Critical Section, so that the device call is thread safe */
    rfdevice_cdma_rx_enter_critical_section( logical_s_dev->rx_device[m_dev_status->curr_band] );

    /* Safe to hard-code 'is_irat_mode' flag to FALSE when enabling diversity
    because diversity is never going to be enabled in IRAT scenarios, 
    In standalong mode, IRAT mesa type is don't care or invalid */
    wakeup_success &= rf_cdma_mc_configure_front_end_for_rx_wakeup (
                      slave_dev, RFM_1X_MODE, TRUE /*is_slave_device*/,
                      rxlm_handle_slave_dev, m_dev_status->curr_band, 
                      m_dev_status->lo_tuned_to_chan, 
                      power_mode, 
                      s_dev_status->script_index,
                      m_dev_status->rx_bw_khz, 
                      m_dev_status->tx_bw_khz,
                      logical_s_dev,
                      s_ant_tuner_token_w,
                      FALSE, /* is_irat_mode ,always FALSE here */
                      RFDEVICE_MEAS_INVALID_SCRIPT /* no in irat mode always invalid*/);

    if ( wakeup_success == TRUE )
    {
      /* Safe to hard-code 'is_irat_mode' flag to FALSE when enabling diversity
      because diversity is never going to be enabled in IRAT scenarios*/
      wakeup_success = rf_1x_mc_send_start_rx_helper ( slave_dev , 
                                                       s_dev_status ,
                                                       rxlm_handle_slave_dev,  
                                                       logical_s_dev ,
                                                       FALSE /*is_irat_mode*/, 
                                                       FALSE /*is_retune */ );

      if ( logical_s_dev->ant_tuner[band] != NULL )
      {
        rf_cdma_atuner_ack_script(
          logical_s_dev->ant_tuner[band],
          s_ant_tuner_token_w );
      }
    } /* ( exec_success == TRUE ) */

    /* Clean up static event smem and event handle regardless faliure or not*/
    rf_cdma_mc_cleanup_ccs_event( slave_dev, RF_CDMA_STATIC_EVENT );

    /* Leave Device Critical Section */
    rfdevice_cdma_rx_leave_critical_section( 
       logical_s_dev->rx_device[m_dev_status->curr_band] );

  } /* if ( wakeup_success == TRUE ) */

  /* Unconditionally clean-up the Rx AGC LNA SM events here */
  wakeup_success &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( slave_dev );

  /* Check if all Static Initialization when through. If not, return Failure.
  For successful execution, do the Rx Wakeup/Tune hardware sequence. */
  if ( wakeup_success == TRUE )
  {
    /* update the rotator offsets to the device data structure */
    rf_cdma_data_update_rot_offsets( master_dev, dyn_settings.freq_offset_hz );

    /* Start the IntelliCeiver if the Analog Reciever supports/requires it */
    if ( s_dev_status->intelliceiver.ic_supported == TRUE )
    {
      rf_cdma_ic_start( s_ic_data_w, 
                        logical_s_dev,
                        m_dev_status->curr_band,
                        m_dev_status->lo_tuned_to_chan );
    }

    /* Update CDMA Data structure with new RF State and  band/Chan */
    rf_cdma_data_set_1x_rf_state( slave_dev, RF_CDMA_STATE_RXDIV );
    rf_cdma_data_set_curr_band( slave_dev, m_dev_status->curr_band );
    rf_cdma_data_set_curr_chan( slave_dev, RFM_CDMA_CARRIER_0, 
                      m_dev_status->curr_chans[RFM_CDMA_CARRIER_0] );
    rf_cdma_data_set_carrier_index( slave_dev , RFM_CDMA_CARRIER_0 , 0 );
    rf_cdma_data_set_rx_bw_khz( slave_dev , m_dev_status->rx_bw_khz );
    rf_cdma_data_set_tx_bw_khz( slave_dev , RF_CDMA_BW_INVALID );
    rf_cdma_data_set_num_carriers( slave_dev , 1 );
    rf_cdma_data_set_lo_tuned_to_chan( slave_dev , 
                                     m_dev_status->lo_tuned_to_chan );
    rf_cdma_data_set_assoc_dev( slave_dev , master_dev );
    rf_cdma_data_set_assoc_dev( master_dev , slave_dev );
    

    /* Get Final RxAGC and LNA State */
    rf_1x_mdsp_get_rx_agc_data( logical_s_dev->rx_mdsp_path, &rx_agc_read );

    rx_agc_read.rx_agc = rf_cdma_convert_rxagc(RFM_CDMA_AGC_FORMAT__DBM10, 
                                               rx_agc_read.rx_agc);

    rf_cdma_data_sanity_check( slave_dev );
    rf_cdma_data_sanity_check( master_dev );

    /* No Wait time */
    ret_val = 0;

    RF_MSG_5( RF_MED, "rf_1x_mc_enable_diversity: SlaveDev %d, "
                      "RxAGC(dBm10) %d, RxAGC_Time (us) %d, "
                      "RxLM_time (us) %d, Vreg_Time (us %d ", 
              slave_dev, rx_agc_read.rx_agc, prof_rxagc_cfg_t_val,
              prof_rxlm_t_val, prof_vreg_t_val );

  } /* if ( wakeup_success == TRUE ) */
  else
  {
	if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
    {
      dev_voting_status = 
                   rfdevice_cdma_rx_pwr_sleep(logical_s_dev->rx_device[band]);
    }
	else
	{
    dev_voting_status = TRUE;
    }

    /* Vote OFF for all RF Devices which as not needed with RFM Device (device) 
       This API call help to vote ON/OFF for TCXO buffer and NPLER BYPASS settings */
    dev_voting_status &= rfcommon_core_dev_power_vote(slave_dev,
                                                     RFCMN_CORE_DEV_OFF,
                                                     RFCMN_CORE_DEV_NO_VOTE);
    if( dev_voting_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_mc_enable_diversity(): Device:%d Pwr Down failed "
                       , slave_dev);
    }

    /* Revert the Vreg State, since Rx is not waking up */
    rfc_cdma_manage_vregs( slave_dev, RFM_1X_MODE, m_dev_status->curr_band,
                           RF_PATH_SLEEP_STATE);

    ret_val = RFM_CDMA_ERROR_FAILURE;

    RF_MSG_3( RF_ERROR, 
              "rf_1x_mc_enable_diversity - failed: [Status %d] - MasterDev %d, "
              "SlaveDev %d, EnableDivTime %d", ret_val, master_dev, slave_dev );
  }

  RF_MSG_3( RF_MED, "rf_1x_mc_enable_diversity : [Status %d] - MasterDev %d, "
            "SlaveDev %d", ret_val, master_dev, slave_dev);

  return ret_val;
} /* rf_1x_mc_enable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable receive diversity operation and put the device to sleep.

  @details
  This API unlinks the state of slave_dev from whatever device it was
  associated with in the rfm_1x_enable_diversity() function.

  If slave_dev is in Rx or RxTx mode, slave_dev is put to sleep.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_DISABLE_DIV_COMPLETE event.  The function will attempt to give
  a best estimate of the time remaining in the return value.

  @param slave_dev
  The radio device that will be disconnected from the diversity radio.
  This device must have previously been associated with a master device
  in a call to rfm_1x_enable_diversity().

  @param rxlm_handle_slave_dev
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware for slave_dev.

  @return
  The anticipated time in microseconds remaining from the
  rf_1x_mc_disable_diversity() function return until the operation is
  complete.

*/
rfm_wait_time_t
rf_1x_mc_disable_diversity
(
  const rfm_device_enum_type slave_dev,
  const lm_handle_type rxlm_handle_slave_dev
)
{
  rfm_wait_time_t ret_val; /* Return value */
  boolean sleep_success = TRUE; /* Flag to track the success of sleep rx */
  boolean dev_voting_status; /* Need the flag to perform device voting */

  const rf_cdma_data_status_type *s_dev_status; /*Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  rf_cdma_ic_data_type *s_ic_data_w; /* Intelliceiver write pointer*/
  ant_tuner_device_tokens_type* s_ant_tuner_token_w; /*Tuner token write pointer*/

  /*--------------------------------------------------------------------------*/
  /* Store master_dev first before invalidating s_dev_status->assoc_dev */
  /* since Write Pointers will be removed once set APIs are ready             */
  /*--------------------------------------------------------------------------*/
  rfm_device_enum_type master_dev;

  /* Get Device Status Read Pointer*/
  s_dev_status = rf_cdma_get_device_status( slave_dev );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( slave_dev );

  /* Get CDMA Data Write Pointer*/
  s_ic_data_w = rf_cdma_get_mutable_intelliceiver( slave_dev );
  s_ant_tuner_token_w = rf_cdma_get_mutable_ant_tuner_token( slave_dev );

  /* Perform NULL Pointer check */
  if ( ( s_dev_status == NULL ) || ( logical_dev == NULL ) ||
       ( s_ic_data_w == NULL) || ( s_ant_tuner_token_w == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_mc_disable_diversity: NULL data for device" );
    return RFM_CDMA_ERROR_NULL_DATA;
  }
  
  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_disable_diversity(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return RFM_CDMA_ERROR_INVALID_LOGICAL_DEV_PARAM;
  }

  /*--------------------------------------------------------------------------*/
  /* stop intelliceiver state-machine */
  /*--------------------------------------------------------------------------*/
  if ( s_dev_status->intelliceiver.ic_supported )
  {
    rf_cdma_ic_stop( s_ic_data_w, logical_dev );
  }

  /*--------------------------------------------------------------------------*/
  /* Compute Disable RTR diversity Script and send RX_stop to FW              */
  /*--------------------------------------------------------------------------*/

  rf_cdma_data_set_script_index( slave_dev, 
                  rf_cdma_mdsp_get_device_buffer_index(RFM_1X_MODE,
                                                       rxlm_handle_slave_dev) );

  if ( sleep_success == TRUE )
  {
    rfm_cdma_band_class_type band = s_dev_status->curr_band;

    /* Enter Device Critical Section, so that the device call is thread safe*/
    rfdevice_cdma_rx_enter_critical_section( logical_dev->rx_device[band] );

    sleep_success &= rf_cdma_mc_configure_front_end_for_rx_sleep ( 
                     slave_dev, rxlm_handle_slave_dev, TRUE/* is_slave_device*/,
                     band,
                     s_dev_status->script_index, 
                     s_dev_status->rx_bw_khz, 
                     s_dev_status->tx_bw_khz, logical_dev,
                     s_ant_tuner_token_w );

    if ( sleep_success == TRUE )
    {
      sleep_success = 
        rf_1x_mc_send_stop_rx_helper ( slave_dev , s_dev_status , rxlm_handle_slave_dev , 
                                  logical_dev , RF_CDMA_RX_STOP_RTC_CX1_TRIGGER ,
                                  FALSE , /* Skip FE Programming? y/n  */ 
                                  FALSE  /* Is QPCH? y/n */ ) ;
      if ( FALSE == sleep_success )
      {
        RF_MSG ( RF_ERROR , "rf_1x_mc_disable_diversity(): " 
               "rf_1x_mc_rx_stop_helper() returned failure" ) ;
      }

      if ( logical_dev->ant_tuner[band] != NULL )
      {
        rf_cdma_atuner_ack_script( logical_dev->ant_tuner[band], 
                                   s_ant_tuner_token_w);
      }

    } /* if ( sleep_success == TRUE ) */

    /* Clean up static event smem and event handle regardless faliure or not*/
    rf_cdma_mc_cleanup_ccs_event( slave_dev, RF_CDMA_STATIC_EVENT );

    /* Leave Device Critical Section */
    rfdevice_cdma_rx_leave_critical_section(logical_dev->rx_device[band]);

  } /* if ( sleep_success == TRUE ) */

  /* Check if all Initialization when through. If not, return Failure.
     For successful execution, do the Rx sleep hardware sequence. */
  if ( sleep_success == TRUE )
  {
	if ( TRUE == rfcommon_nv_get_lpm_feature_enabled() )
    {
      dev_voting_status = 
                     rfdevice_cdma_rx_pwr_sleep(
                                logical_dev->rx_device[s_dev_status->curr_band]);
    }
	else
	{
      dev_voting_status = TRUE;
    }
    /* Vote OFF for all RF Devices which are not needed with RFM Device. This 
       API call help to vote ON/OFF for TCXO buffer and NPLER BYPASS settings */
    dev_voting_status &= rfcommon_core_dev_power_vote(slave_dev,
                                                     RFCMN_CORE_DEV_OFF,
                                                     RFCMN_CORE_DEV_NO_VOTE);
    if( dev_voting_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_1x_mc_disable_diversity(): Device: %d Pwr Down failed "
                       , slave_dev);
      ret_val = RFM_CDMA_ERROR_FAILURE;
    }

    sleep_success &= dev_voting_status;

    if ( sleep_success == TRUE )
    {
      /* Revert the Vreg State, since Rx is going to sleep */
      rfc_cdma_manage_vregs( slave_dev, RFM_1X_MODE, 
                             s_dev_status->curr_band,
                             RF_PATH_SLEEP_STATE);

     /*-----------------------------------------------------------------------*/
     /* Need to store the master dev number to invalid assoc_dev              */
     /* since Write Pointers will be removed once set APIs are ready          */
     /* or if assoc_dev of slave_dev is invalidated first, we lose master dev */
     /*-----------------------------------------------------------------------*/
      master_dev = s_dev_status->assoc_dev;
      /*----------------------------------------------------------------------*/
      /* LAST step::: Invalidate the Device info for Slave device             */    
      /* bring carrier info to initial-state: invalidate all carrier_index &    */
      /* channels                                                               */
      /*----------------------------------------------------------------------*/
      rf_cdma_reset_sw_state ( slave_dev ) ;
      rf_cdma_data_set_assoc_dev( master_dev , RFM_INVALID_DEVICE );
      rf_cdma_data_sanity_check( slave_dev ) ;
      rf_cdma_data_sanity_check( master_dev ) ;

      /* Device successfully put to sleep - undo any SSMA operations performed
         on the paired device, since the device is asleep, need to use
         RF_CDMA_INVALID_BAND and RF_CDMA_INVALID_CHAN while calling the SSMA
         routines */
      (void) rf_cdma_get_ssma_recos ( slave_dev, 
                                      rxlm_handle_slave_dev, 
                                      RFM_CDMA_MAX_BAND,
                                      RF_CDMA_INVALID_CHAN,
                                      logical_dev->paired_device );

      ret_val = 0; /* success */
        /* This function releases all the TQs and TQs are required to 
      queue any read\writes to CCS so this function call should be 
      after all device read or writes. */ 
      rf_cdma_mdsp_modem_rx_deallocate_resources ( slave_dev , RFM_1X_MODE , rxlm_handle_slave_dev ) ;
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rf_1x_mc_disable_diversity: error during config " 
                "for device %d", slave_dev );
      ret_val =  RFM_CDMA_ERROR_FAILURE;
    }

  } /* if ( sleep_success == TRUE )*/
  else
  {
     RF_MSG_1( RF_ERROR, "rf_1x_mc_disable_diversity: error during config " 
               "for device %d", slave_dev );
     ret_val =  RFM_CDMA_ERROR_FAILURE;
  }


  return ret_val;

} /* rf_1x_mc_disable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Re-tune the 1x radio for a new band and or channel of operation.

  @details
  Changes the operating frequency of the 1x radio.  This function may
  only be called in Rx state.

  If this API is called while the radio is in the Rx state, the receiver
  (and any associated diversity receivers) will be re-tuned to operate
  in the new band and/or channel.

  If this API is called while the radio is in the RxTx state, the
  function will return with the error code RFM_1X_TUNE_ERROR__INVALID_MODE
  without changing anything in the current radio configuration.
  This means the radio will remain in the RxTx state on the previous
  channel.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_1X_TUNE_COMPLETE event.  The function will attempt to give a
  best estimate of the time remaining in the return value.
 
  If this API is called with same band and chan back to back, the call will 
  still be processed and retune will be done again on same band and channel.

  @param device
  The radio path to re-tune.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param new_band
  RF CDMA band on which 1x radio is to be tuned.
 
  @param new_chan
  RF CDMA channel on which 1x radio is to be tuned.
 
  @param is_irat_mode
  Boolean flag to check if the API is being called in IRAT or non-IRAT scenarios
  In IRAT scenarios, a lot of API calls can be skipped ( ex. DLNA Rx on/off 
  scripting, vregs etc.)
 
  @return
  The anticipated time in microseconds remaining from the rfm_1x_tune()
  function return until the operation is complete, or an error code.
  If the API call succeeds the return value will be non-negative; all
  error codes will be negative.

  @retval RFM_1X_TUNE_ERROR__INVALID_BAND
  The band the user requested is not supported by the current chipset
  configuration.

  @retval RFM_1X_TUNE_ERROR__INVALID_CHANNEL
  The channel the user requested is not supported by the current chipset
  configuration.

  @retval RFM_1X_TUNE_ERROR__INVALID_MODE
  The radio was in an invalid mode and the radio could not be retuned.
*/
rfm_wait_time_t
rf_1x_mc_retune_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cdma_band_class_type new_band,
  const rfm_cdma_chan_type new_chan,
  boolean is_irat_mode
)
{
  rfm_wait_time_t ret_val; /* final wait time from this API */
  const rf_cdma_data_status_type *m_dev_status;/* Dev Status Read Ptr*/
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean notch_status;
  rf_time_tick_type prof_retune_t;      /* Retune Time Profile */
  rf_time_type prof_retune_t_val;      /* Retune Time Profile */

  prof_retune_t = rf_time_get_tick( );

  /* get the device status for master device */
  m_dev_status = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Perform NULL Pointer check */
  if ( ( m_dev_status == NULL ) || ( logical_dev == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_retune_rx: NULL data for device %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Call 1x Main Control to put device to sleep - use the RXLM handle in the
     dev_status_r_master, since, L1 may pass in a "rxlm_handle" that is 
     different from the one it used during wakeup_rx (this is a valid case).
     As far as RF is concerned, it needs to make sure that the device is put
     to sleep - hence use the RxLM handle that was saved during the prep_wakeup
     API
  */
  ret_val = rf_1x_mc_helper_for_retune ( device, rxlm_handle, new_band, 
                                         new_chan, is_irat_mode );


  /* Apply notch filter recommendations from WTR on the master device */
  notch_status = 
  rf_cdma_mc_trigger_dynamic_notch_update(device,
                      m_dev_status->ssma_cb_data.ssma_reco,
                      m_dev_status->ssma_cb_data.num_notch_filters_used,
                      m_dev_status->ssma_cb_data.clear_notch); 

  /* Notch application failed bail out!*/
  if ( notch_status == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_retune_rx: Notch update failed for device %d",
              device );
    ret_val =  RFM_CDMA_ERROR_FAILURE;
  }

  /* Retune the diversity chain if diversity is enabled */
  if (ret_val >= RFM_CDMA_HEALTHY_STATE )
  {
    /* check if diversity chain is to be retuned as well */
    if ( m_dev_status->assoc_dev != RFM_INVALID_DEVICE )
    {
      /* Dev Status Read Ptr for slave device */
      const rf_cdma_data_status_type *dev_status_r_slave; 
    
      /* holds the associated device */
      rfm_device_enum_type slave_dev = m_dev_status->assoc_dev;
    
      /* get device status for slave */
      dev_status_r_slave = rf_cdma_get_device_status(slave_dev);

      /*NULL pointer check*/
      if( dev_status_r_slave == NULL )
      {
        RF_MSG_1( RF_ERROR, "rf_1x_mc_retune_rx: Dev %d NULL Data", slave_dev );
        ret_val = RFM_CDMA_ERROR_NULL_DATA;
      } /* if( dev_status_r_slave == NULL ) */
      else
      {
        ret_val = rf_1x_mc_helper_for_retune ( slave_dev, 
                                               dev_status_r_slave->rxlm_handle, 
                                               new_band, new_chan,
                                               is_irat_mode );

      } /* if ! ( dev_status_r_slave == NULL ) */

      if (ret_val < RFM_CDMA_HEALTHY_STATE)
      {
        RF_MSG_2( RF_ERROR, "rf_1x_mc_retune_rx : [Status %d] - Failed during"
                  "retune for diversity device=%d", ret_val, slave_dev);
      } /* if (ret_val < RFM_CDMA_HEALTHY_STATE) */
    
    } /* if ( dev_status_r->assoc_dev != RFM_MAX_DEVICES ) */

  } /* if (ret_val >= RFM_CDMA_HEALTHY_STATE ) */
  else
  {
    RF_MSG_2( RF_ERROR, "rf_1x_mc_retune_rx : [Status %d] - Failed during"
              "retune for Primary device=%d", ret_val, device);
  }

  rf_cdma_data_sanity_check( device );

  prof_retune_t_val = rf_time_get_elapsed( prof_retune_t, RF_USEC );

  RF_MSG_1 ( RF_LOW, "rf_1x_mc_retune_rx: time profile"
                     "Retune Time: " , prof_retune_t_val );

  return ret_val;
} /* rf_1x_mc_retune_rx */

/*! @} */

/*============================================================================*/
/*!
  @name AGC Config Functions

  @brief
  Functions to configure 1x RxAGC and TxAGC
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Converge RxAGC
 
  @details
  This function is used to wait for 1x RxAGC to converge. This will wait for 
  acq_down_ctr to go below 0 and to protect from spinning in an infinite loop,
  wait of 2 ms is added before bailing out
 
  @param device
  RFM device for which RxAGC convergene is expected
 
  @return
  Flag to indicate the success if the  function
*/
boolean
rf_1x_mc_rx_agc_convergence
(
  rfm_device_enum_type device
)
{
  rf_1x_mdsp_rx_agc_read_type rx_agc_data; /* place holder for RxAGC data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  uint8 loop_ctr = 0; /* poll acqTimeDnCntr variable in FW --> RxAGC cannot be 
  assumed to have to have settled until acqTimeDnCntr < 0 */
  uint8 max_times_to_poll = 40 ;
  /* query 1x rxagc update duration (1 combiner IQR duarion) */
  uint16 rxagc_update_duration = rf_cdma_msm_get_combiner_interrupt_period();

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev ==  NULL)
  {
    RF_MSG_1( RF_ERROR,"rf_1x_mc_rx_agc_convergence: "
                       "NULL logical data for device %d", device );
    return FALSE;   
  }

  /* While calibrating we should converge much faster so set the polling 
     attempts to a lower value so the max wait time during calibration will
     be 6 x 104 uSec = 624 uSec  */
  if ( rfm_get_calibration_state() == TRUE )
  {
    max_times_to_poll = 6 ;
  }

  /* wait for acq_down_ctr to go below 0 - to protect from spinning in
  in an infinite loop, wait for 2 ms and then bail out */
  do /* while ( rx_agc_data.acq_down_ctr > 0); */
  {
    /* wait for 1 combiner IRQ duration when the RxAGC parameter gets updated before polling) */
    DALSYS_BusyWait( rxagc_update_duration ) ; 

    /* poll FW to see if RxAGC is in tracking mode */
    rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_data );

    loop_ctr++;

    /* check to see if it is more than 2ms ( 40 * 52 uS = 2800 uS) */
    if ( loop_ctr > max_times_to_poll ) 
    {
      RF_MSG( RF_ERROR, "rf_1x_mc_rx_agc_convergence: RxAGC did NOT "
                        "converge in 2.8 ms - bailing out !!");
      break;
    } /* if ( loop_ctr > 40 )  */

    RF_MSG_2( RF_LOW, "rf_1x_mc_rx_agc_convergence: RxAGC converge loop-> %d "
                      "acqDwn_ctr-> %d ", loop_ctr, rx_agc_data.acq_down_ctr);

  } while ( rx_agc_data.acq_down_ctr >= 0);  /* -1 = acq is done */

  return TRUE;   

} /* rf_1x_mc_rx_agc_convergence */

/*! @} */

/*============================================================================*/
/*!
  @name 1x AGC Query Functions

  @brief
  Functions to query 1x Rx and Tx AGC
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Total Tx Filtered Power
 
  @details
  Obtains the total Tx filtered level from MDSP for the given device.
 
  @param device
  The radio path for which to get the filtered power.
 
  @return
  Total Tx Filtered Power in dBm10 units
*/
int16
rf_1x_mc_get_total_tx_filtered_power
( 
  rfm_device_enum_type device
)
{
  int16 filtered_power_dbm10; /* Filtered power in dBm10 */
  int16 K; /* turn-around const */
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_read; /* TxAGC Read */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Perform NULL Pointer check */
  if( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_mc_get_total_tx_filtered_power: NULL Logical "
              "data for device %d", device );
    return 0xFFFF;
  }
  
  /* get read pointer to device - need this to find out current band */
  dev_status_r = rf_cdma_get_device_status( device );

  /*NUll pointer check*/
  if( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "ftm_1x_log_get_primary_device: Dev %d NULL Data",
               device );
    return 0xFFFF;
  }

  /* Read Tx AGC */
  rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->rx_mdsp_path, 
                                      rf_cdma_data_get_txlm_handle (device) ,
                                      &tx_agc_read ) ;

  /* get turn-around constant for current band */
  K = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

  /* Convert filtered power to dBm10 */
  filtered_power_dbm10 = rf_cdma_convert_txagc( RFM_CDMA_AGC_FORMAT__DBM10, K, 
                                        tx_agc_read.total_tx_filtered_power );

  return filtered_power_dbm10;
}

/*! @} */

/*============================================================================*/
/*!
  @name RF-FW Synchronization SNUM based Response handlers

  @brief
  This section includes all the SNUM based Response handlers to be used
  for RF-FW messaging synchroziation.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rx Start Response handler
 
  @details
  This callback function is executed in second level dispatching when
  Rx Start Response is received from firmware. This function will basically post 
  the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_rx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_rx_start_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_rx_start_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_rx_start_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_rx_start_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] RX_START_RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
    ret_val &= rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( cb_data->sema_token != NULL ) */
  else
  {
    ret_val = FALSE;
    ERR_FATAL( "rf_1x_rx_start_rsp_cb: NULL Sem Token", 0, 0, 0 );
  } /* if ! ( cb_data->sema_token != NULL ) */

  RF_MSG_4( RF_LOW, "rf_1x_rx_start_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum, cb_data->sema_token );

  return ret_val;

} /* rf_1x_rx_start_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rx Stop Response handler
 
  @details
  This callback function is executed in second level dispatching when
  Rx Stop Response is received from firmware. This function will basically post 
  the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_rx_stop_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  int32 rf_ret;
  rf_cdma_rx_stop_rsp_data_type *cb_data = NULL;
  cfw_rx_config_rsp_info_t *rsp_payload;
  rfm_device_enum_type device;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_rx_stop_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Typecast the payload to cfw_rx_stop_rsp_msg_t */
  rsp_payload = (cfw_rx_config_rsp_info_t*)req_ptr->payload;

  /* Extract the callback data */
  cb_data = (rf_cdma_rx_stop_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_rx_stop_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] RX_STOP_RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );
  
  /* Get the Times Stop Status */
  if ( (boolean)rsp_payload->timed_stop_status == RX_STOP_SUCCESS )
  {
    cb_data->timed_sleep_success = TRUE;
  } /* if ( (boolean)rsp_payload->timed_stop_status == RX_STOP_SUCCESS ) */
  else
  {
    cb_data->timed_sleep_success = FALSE;
  } /* if ! ( (boolean)rsp_payload->timed_stop_status == RX_STOP_SUCCESS ) */

  if ( cb_data->blocking_call == TRUE )
  {
    /* Post the Semaphore only if an active semaphore is available */
    if ( cb_data->sema_token != NULL )
    {
      if ( cb_data->sema_token->is_sem_waiting == FALSE )
      {
        RF_MSG_2( RF_ERROR, "rf_1x_rx_stop_rsp_cb: sNUM %d "
                  "Sem @ 0x%x Possible POST-BEFORE-WAIT", 
                  snum_data->snum, cb_data->sema_token );
      } /* if ( fwrsp_data->rx_stop_rsp.sema_token.is_sem_waiting != TRUE ) */
  
      /* Post the Semaphore */
      rfcommon_semaphore_post( cb_data->sema_token );
      cb_data->blocking_call = TRUE;
    } /* if ( fwrsp_data->rx_stop_rsp.sema_token != NULL ) */
    else
    {
      ret_val = FALSE;
      ERR_FATAL( "rf_1x_rx_stop_rsp_cb: NULL Sem Token", 0, 0, 0 );
    } /* if ! ( fwrsp_data->rx_stop_rsp.sema_token != NULL ) */
  } /* if ( fwrsp_data->rx_stop_rsp.blocking_call == TRUE ) */
  else
  {

    /* Get the active device */
    device = cb_data->device;

    /* Complete Timed Sleep Rx */
    rf_ret = rf_1x_mc_complete_timed_sleep_rx( device,
                                                cb_data->timed_sleep_success,
                                                cb_data->callback,
                                                cb_data->userdata );

      /* Invalidate the housekeeping data */
     cb_data->blocking_call = TRUE;
     cb_data->callback = NULL;
     cb_data->userdata = NULL;

    /* Return value checking */
    if ( rf_ret < RFM_CDMA_HEALTHY_STATE )
    {
      ret_val = FALSE;
      RF_MSG_1( RF_ERROR, "rf_1x_rx_stop_rsp_cb: Complete Timed"
                          "Sleep Rx Failed for dev %d", device );
    } /* if ( ret_val == FALSE ) */
  } /* if ! ( fwrsp_data->rx_stop_rsp.blocking_call == TRUE ) */

  RF_MSG_4( RF_LOW, "rf_1x_rx_stop_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum, cb_data->sema_token );

  return ret_val;

} /* rf_1x_rx_stop_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tx Start Response handler
 
  @details
  This callback function is executed in second level dispatching when
  Tx Start Response is received from firmware. This function will basically post 
  the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_tx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_tx_start_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_tx_start_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_tx_start_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_tx_start_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] TX_START_RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
    ret_val &= rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( cb_data->sema_token != NULL ) */
  else
  {
    ret_val = FALSE;
    ERR_FATAL( "rf_1x_tx_start_rsp_cb: NULL Sem Token", 0, 0, 0 );
  } /* if ! ( cb_data->sema_token != NULL ) */

  RF_MSG_4( RF_LOW, "rf_1x_tx_start_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum, cb_data->sema_token );

  return ret_val;

} /* rf_1x_tx_start_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tx Start Response handler
 
  @details
  This callback function is executed in second level dispatching when
  Tx Start Response is received from firmware. This function will basically post 
  the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_tx_stop_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_tx_stop_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_tx_stop_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_tx_stop_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_tx_stop_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] TX_STOP_RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
    ret_val &= rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( cb_data->sema_token != NULL ) */
  else
  {
    ret_val = FALSE;
    ERR_FATAL( "rf_1x_tx_stop_rsp_cb: NULL Sem Token", 0, 0, 0 );
  } /* if ! ( cb_data->sema_token != NULL ) */

  RF_MSG_4( RF_LOW, "rf_1x_tx_stop_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum, cb_data->sema_token );

  return ret_val;

} /* rf_1x_tx_stop_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FW State Config Response handler
 
  @details
  This callback function is executed in second level dispatching when
  FW State Config Response is received from firmware. This function will
  basically post the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_fws_config_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_fws_cfg_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_fws_config_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_fws_cfg_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_fws_config_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
    ret_val &= rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( cb_data->sema_token != NULL ) */
  else
  {
    ret_val = FALSE;
    ERR_FATAL( "rf_1x_fws_config_rsp_cb: NULL Sem Token", 0, 0, 0 );
  } /* if ! ( cb_data->sema_token != NULL ) */

  RF_MSG_4( RF_LOW, "rf_1x_fws_config_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum, cb_data->sema_token );

  return ret_val;

} /* rf_1x_fws_config_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  iRAT Rx Start Response handler
 
  @details
  This callback function is executed in second level dispatching when
  iRAT Rx Start Response is received from firmware. This function will basically 
  post the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_irat_rx_start_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_irat_rx_start_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_irat_rx_start_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_irat_rx_start_rsp_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_irat_rx_start_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] IRAT_RX_START_RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  RF_MSG_3( RF_LOW, "rf_1x_irat_rx_start_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "[Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum );

  return ret_val;

} /* rf_1x_irat_rx_start_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pilot meas config Response handler
 
  @details
  This callback function is executed in second level dispatching when
  pilot meas config Response is received from firmware. 
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_pilot_meas_config_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_pilot_meas_config_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_irat_rx_start_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_pilot_meas_config_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_pilot_meas_config_rsp_cb: NULL CB Data" );
    return FALSE;
  } /* if ( cb_data == NULL ) */

  RF_MSG_4( RF_MED, "[1X][FW->RF] [D%d] PILOT_MEAS_CFG_RSP 0x%x [ %d | %d ]", 
            cb_data->device,
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  RF_MSG_3( RF_LOW, "rf_1x_pilot_meas_config_rsp_cb: Dev %d - [%d] SNUM.0x%x "
            "[Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum );

  return ret_val;

} /* rf_1x_pilot_meas_config_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pilot Meas stop stream Response handler
 
  @details
  This callback function is executed in second level dispatching when
  pliot meas stop stream Response is received from firmware. This function will
  basically post the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_1x_pilot_meas_stop_stream_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_pilot_meas_stop_stream_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_1x_pilot_meas_stop_stream_rsp_cb: "
                      "NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_pilot_meas_stop_stream_data_type*)snum_data->snum_cb_data;

  /* Check NULL for cb_data */
  if ( cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_1x_pilot_meas_stop_stream_rsp_cb: NULL CB Data");
    return FALSE;
  } /* if ( cb_data == NULL ) */

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->sema_token != NULL )
  {
    ret_val &= rfcommon_semaphore_post( cb_data->sema_token );
  } /* if ( cb_data->sema_token != NULL ) */
  else
  {
    ret_val = FALSE;
    ERR_FATAL( "rf_1x_pilot_meas_stop_stream_rsp_cb: NULL Sem Token", 
               0, 0, 0 );
  } /* if ! ( cb_data->sema_token != NULL ) */

  RF_MSG_4( RF_LOW, "rf_1x_pilot_meas_stop_stream_rsp_cb: "
            "Dev %d - [%d] SNUM.0x%x "
            "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
            snum_data->snum, cb_data->sema_token );

  return ret_val;

} /* rf_1x_pilot_meas_stop_stream_rsp_cb */

/*! @} */

/*============================================================================*/
/*!
  @name SAR

  @brief
  Functions to be used for SAR.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform the 1x Tx limit loop that the SAR state has changed, so it can
  recompute the target max power, if necessary.

  @details
  Assumes that the RF critical section has already been locked by this thread,
  and that it is safe to access the internal data structures.

  This interface makes no assumption about what devices may or may not be
  in 1x mode, so it will loop through all devices, and perform a power
  limiting update if they are in 1x mode and currently transmitting.
*/
void
rf_1x_mc_tx_update_sar_backoff
(
  void
)
{
  rfm_device_enum_type device;

  for ( device = RFM_DEVICE_0;
        device < RFM_MAX_DEVICES;
        device++ )
  {
    if ( rfm_get_current_mode(device) == RFM_1X_MODE )
    {
      const rf_cdma_data_status_type *dev_status_r;

      dev_status_r = rf_cdma_get_device_status( device );

      /*NULL Pointer check*/
      if( dev_status_r == NULL )
      {
        RF_MSG_1( RF_HIGH, "rf_1x_mc_tx_update_sar_backoff: Dev %d NULL Data",
                 device );
      }/*NULL Pointer check*/
      else
      {
      if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
      {
        rf_1x_plim_update_sar_state( device );
      } /* if ( dev_status_r->rf_state == RF_CDMA_STATE_RXTX ) */
      } /* if! ( dev_status_r == NULL ) */
    } /* if ( rfm_get_current_mode(device) == RFM_1X_MODE ) */
  } /* for device 0..N-1 */
} /* rf_1x_mc_tx_update_sar_backoff() */

/*! @} */

/*============================================================================*/
/*!
  @name Tear Down

  @brief 
  This section contains functions pertaining to Tear Down
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tear Down RF

  @details
  This function will Tear Down RF to Sleep State, no mater what the current
  state is. 
 
  @param device RF Device on which RF is to be torn down
 
  @return
  Flag for the function to indicate whether tear down was required or not. Based 
  on this parameter, rfm_enter_mode() will be called in calling function. If 
  TRUE, rfm_enter_mode() is not called. This will also be FALSE for the case,
  when RFM failure is seen during tear-down; thus indicating the calling
  function that rfm_enter_mode() has to be done to reset everything.
*/
boolean
rf_1x_mc_tear_down
(
  rfm_device_enum_type device
)
{
  boolean was_tear_down_req = TRUE; /* Flag indicating if tear down is needed */
  boolean rfm_ret = TRUE; /* Return val from RFM */
  const rf_cdma_data_status_type *dev_status = NULL; /* RF Dev Status */

  /* Query the current RF State Ptr for master device */
  dev_status = rf_cdma_get_device_status(device);

  /* NULL pointer check*/
  if(  dev_status == NULL )
  {
    RF_MSG_1(RF_ERROR,"rf_1x_mc_tear_down: NULL data for device %d",device);
    was_tear_down_req = FALSE;
  }
  else
  {
    switch ( dev_status->data_1x.rf_state )
    {
    case RF_CDMA_STATE_RX:
    {
      
      /* Check if there is other associated Device with this device. If there
      in any other associated device, then diversity must be disabled on
      the associated device */
      if ( dev_status->assoc_dev != RFM_INVALID_DEVICE )
      {
        /* place holder for associated device */
        rfm_device_enum_type assoc_dev = dev_status->assoc_dev;

        /* status of RF assoc Dev Status */
        const rf_cdma_data_status_type *assoc_dev_status = 
                                         rf_cdma_get_device_status( assoc_dev );
                
        /* NULL pointer check*/
        if(  assoc_dev_status == NULL )
        {
          RF_MSG_1( RF_ERROR,"rf_1x_mc_tear_down: NULL data for device %d",
                    device );
          was_tear_down_req = FALSE;
        }
        else
        {
          rfm_ret &= ( rfm_1x_disable_diversity( assoc_dev,
                                                 assoc_dev_status->rxlm_handle,
                                                 NULL, 
                                                 NULL ) >= 0 );
          if (rfm_ret == TRUE)
          {
            rf_cdma_data_set_1x_rf_state ( assoc_dev, RF_CDMA_STATE_SLEEP );
            rf_cdma_data_set_assoc_dev( assoc_dev, RFM_INVALID_DEVICE );
            rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE );
          }
          else
          {
            RF_MSG_1( RF_ERROR, "rf_1x_mc_tear_down: Failed to disable  "
                                "diversity on associated device %d", 
                                  dev_status->assoc_dev ); 
          }

        }/* if!(  assoc_dev_status == NULL ) */

      } /* if ( dev_status->assoc_dev != RFM_INVALID_DEVICE ) */
  
      rfm_ret &= ( rfm_1x_sleep_rx( device, dev_status->rxlm_handle, 
                                    NULL, NULL ) >= 0 );
  
      rf_cdma_data_set_1x_rf_state( device, RF_CDMA_STATE_SLEEP );
  
    } /* case RF_CDMA_STATE_RX: */
      break;
  
    case RF_CDMA_STATE_RXTX:
    {
      /* Check if there is other associated Device with this device. If there
         is any other associated device, then Diversity must be disabled on
         the associated device */
      if ( dev_status->assoc_dev != RFM_INVALID_DEVICE )
      {

        rfm_device_enum_type assoc_dev = dev_status->assoc_dev;

        /* status of RF assoc Dev Status */
        const rf_cdma_data_status_type *assoc_dev_status = 
                                         rf_cdma_get_device_status( assoc_dev );
                        
        /* NULL pointer check*/
        if(  assoc_dev_status == NULL )
        {
          RF_MSG_1( RF_ERROR,"rf_1x_mc_tear_down: NULL data for device %d" , 
                    device );
          was_tear_down_req = FALSE;
        } 
        else
        {
          rfm_ret &= ( rfm_1x_disable_diversity( assoc_dev, 
                                                 assoc_dev_status->rxlm_handle,
                                                 NULL, 
                                                 NULL ) >= 0 );
    
          if (rfm_ret == TRUE)
          {
            rf_cdma_data_set_1x_rf_state ( assoc_dev, RF_CDMA_STATE_SLEEP ); 
            rf_cdma_data_set_assoc_dev( assoc_dev, RFM_INVALID_DEVICE );  
            rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE );     
          }
          else
          {
            RF_MSG_1( RF_ERROR, "rf_1x_mc_tear_down: Failed to disable  "
                                "diversity on associated " "device %d", 
                                  dev_status->assoc_dev ); 
          }
        }/* if ! (  assoc_dev_status == NULL ) */

      } /* if ( dev_status->assoc_dev != RFM_INVALID_DEVICE ) */

      rfm_ret &= ( rfm_1x_sleep_tx( device, dev_status->txlm_handle, 
                                    NULL, NULL ) >= 0 );
  
      rfm_ret &= ( rfm_1x_sleep_rx( device, dev_status->rxlm_handle, 
                                    NULL, NULL ) >= 0 );
  
      rf_cdma_data_set_1x_rf_state ( device, RF_CDMA_STATE_SLEEP ); 
  
    } /* case RF_CDMA_STATE_RXTX: */
      break;
  
    case RF_CDMA_STATE_RXDIV:
    {
      /* Disable the diversity for this device, as it is associated with other
      Master Device */
      rfm_device_enum_type assoc_dev = dev_status->assoc_dev;

      const rf_cdma_data_status_type* assoc_dev_status = 
                                          rf_cdma_get_device_status( assoc_dev);
              
      /* NULL pointer check*/
      if(  assoc_dev_status == NULL )
      {
        RF_MSG_1( RF_ERROR,"rf_1x_mc_tear_down: NULL data for device %d",
                  device );
        was_tear_down_req = FALSE;
      }/* if(  dev_status->assoc_dev == NULL ) */
      else
      {
        rfm_ret &= ( rfm_1x_disable_diversity( device, dev_status->rxlm_handle,
                                               NULL, NULL ) >= 0 );
    
        if (rfm_ret == TRUE)
        {
          rf_cdma_data_set_assoc_dev( assoc_dev, RFM_INVALID_DEVICE); 
          rf_cdma_data_set_1x_rf_state ( device, RF_CDMA_STATE_SLEEP ); 
          rf_cdma_data_set_assoc_dev( device, RFM_INVALID_DEVICE ); 
        }
        else
        {
          RF_MSG_1( RF_ERROR, "rf_1x_mc_tear_down: Failed to disable "
                    "diversity on associated device %d", dev_status->assoc_dev);
        }
      }/* if ! (  assoc_dev_status == NULL ) */

    } /* case RF_CDMA_STATE_RXDIV: */
      break;
  
    case RF_CDMA_STATE_SLEEP:
    case RF_CDMA_STATE_INVALID:
    default:
      was_tear_down_req = FALSE;
      RF_MSG( RF_LOW, "rf_1x_mc_tear_down: Tear Down RF : Not Required" );
      break;
  
    } /* switch ( dev_status->rf_state ) */

    rf_cdma_data_set_1x_rf_state ( device, RF_CDMA_STATE_SLEEP );
  
    if ( rfm_ret == FALSE )
    {
      was_tear_down_req = FALSE;
    }
  
    RF_MSG_1( RF_LOW, "rf_1x_mc_tear_down : Tear Down on Dev %d done", device );
  
  } /* if ! (  dev_status == NULL ) */

  return was_tear_down_req;  /* Return */

} /* rf_1x_mc_tear_down */


/*! \} */

/*============================================================================*/
/*!
  @name NV

  @brief
  This section contains functions pertaining to NV
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reading NV from 1x technology

  @details
  Retrieves all the NV from 1x technology. Reads 1x mode
  specific RF configuration parameters from NV. This is called as part of the 
  LOAD_RF_NV function case where we need to read the calibration NV and write 
  them without having to reset.
 
  @param device
  RFM device for which NV is to be reloaded
 
  @param caller_tcb_ptr
  Calling task's TCB pointer
 
  @param task_nv_wait_sig
  Task signal to wait for when reading NV

  @param task_wait_func_ptr
  Task's wait function to be called when reading NV

  @return
  TRUE if NV retrieve is successful, otherwise returns FALSE.
*/
boolean
rf_1x_mc_reload_nv
(
  rfm_device_enum_type device, 
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Clean up allocated memory and reset NV data structures before loading
  NVs again. Otherwise we may run out of memory space */
  rf_cdma_nv_cleanup();

  return rf_cdma_nv_load( caller_tcb_ptr, task_nv_wait_sig,
                          task_wait_func_ptr );
} /* rf_1x_mc_reload_nv */

/*! @} */

/*============================================================================*/
/*!
  @name RF-MC utility functions

  @brief
  This section holds all utility APIs pertaining to 1x MC 
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API will return the synth status for a given Tech.Path and synthesizer 

  @param device
  RFM device for which PLL status is queried
 
  @param rf_mode
  current mode/tech in which tye device is operating in
 
  @synth_type
  TX PLL or RX PLL
 
  @param is_synth_locked
  holds the PLL status 
*/
void 
rf_1x_mc_get_synth_lock_status
(
  rfm_device_enum_type device, 
  rfcom_mode_enum_type rf_mode,
  rfcom_synth_type synth_type,
  void *is_synth_locked
)
{
 const rfc_cdma_logical_device_params_type *logical_dev = NULL;        
 logical_dev = rfc_cdma_get_logical_device_params(device);

 if ( logical_dev!= NULL )
 {
     /* Device Status Read Pointer - to get current band */
     const rf_cdma_data_status_type 
                            *dev_status_r = rf_cdma_get_device_status( device );

   if(synth_type == RF_SYNTH_RX)
   {

     /* Perform NULL Pointer check */
     if ( dev_status_r == NULL )
     {
       RF_MSG_1( RF_ERROR, "rf_1x_mc_get_synth_lock_status: "
                           "NULL data for device %d", device );
     }
     else
     {
       rfdevice_cdma_rx_cmd_dispatch( 
               logical_dev->rx_device[dev_status_r->curr_band], 
               RFDEVICE_GET_RX_SYNC_LOCK,
               is_synth_locked );
     }
   } /* if(synth_type == RF_SYNTH_RX) */
   else if (synth_type == RF_SYNTH_TX) 
   {
     /* Perform NULL Pointer check */
     if ( dev_status_r == NULL )
     {
       RF_MSG_1( RF_ERROR, "rf_1x_mc_get_synth_lock_status: "
                           "NULL data for device %d", device );
     }
     else
     {
       rfdevice_cdma_tx_cmd_dispatch( 
          logical_dev->tx_device[dev_status_r->curr_band] , 
                                      RFDEVICE_GET_TX_SYNC_LOCK, 
                                      is_synth_locked );
     }
   }
   else
   {
     RF_MSG_1( RF_ERROR, "Invalid SYNTH query(%d) for SYNTH in for"
                         "rfm_get_synth_lock_status", synth_type );
   }
 }
 else
 {
   RF_MSG_1( RF_ERROR, "rf_1x_mc_get_synth_lock_status: Null pointer for "
                       "logical device (%d)", device );
 }

} /* rf_1x_mc_get_synth_lock_status */

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
