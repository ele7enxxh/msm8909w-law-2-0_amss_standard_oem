/*!
  @file
  rfm_1x.c

  @brief
  This file defines the RFM-1X interfaces to be used by external module.

  @addtogroup RF_CDMA_1X_RFM
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rfm_1x.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      1X2G QTA
09/15/15   rs      Reduce DIAG logging volume from C2K
12/01/14   as/spa  Add mutex protection to ASDIV operations
                   Skip update tune code if already under progress
11/12/14   spa     Change tx_nv_path to be per band per logical device
10/13/14   vr      CW Fix
10/07/14   vr      ftm IQ capture changes
09/30/14   ndb     Pass LM_handle in tuner_cb to help with debug
09/23/14   pk      compiler warning fix
08/28/14   kg      Pass tech_id in tuner_cb to help with debug
06/28/14   fh      Added api for cleanup script
05/12/14   spa     Initialize event handle to NULL before use in AOL
04/23/14   cd      Support for 8 PA state functionality
04/21/14   JJ      Remove rfm_1x_get_curr_rtc_cx1()
04/21/14   sty     KW fixes for null pointer checks - minor edit
04/21/14   sty     KW fixes for null pointer checks
04/17/14   sty     fixed null pointer check
04/17/14   sty     Call rf_1x_mdsp_get_tx_agc_logical_data() only in TX mode
04/17/14   zhw     Enable HDET
04/10/14   JJ      Remove api rfm_1x_get_fw_smem_addr()
04/09/14   JJ      Added wrap api to get current rtc time
04/04/14   spa     Remove "use_default_btf" flag
04/02/14   spa     Check for RxTx state before retrieving BTF value
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data
                   provided by RFLM.
03/26/14   spa     Updated rfm_1x_get_btf API
03/18/14   APU     1> Have DM do the buffer deallocation.
                   2> Centralize allocation and association to 1 place.
03/13/14   spa     Skip xpt update when going from access to voice
02/21/14   sty     Provide hard-coded BTF value for 1x - till RxLM and TxLM
                   hooks are in place
02/11/14   APU     Enable Temp Comp.
02/10/14   cd      Access probe config does not reconfigure entire Tx AGC
02/07/14   APU     Removed unnccesary messages.
01/24/14   JJ      Disable Temp Comp
01/20/14   spa     Added flag imm_lna_write_cal for immediate LNA writes in cal
01/16/14   JJ      Reduced delay time between two immediate writes in retune
01/15/14   spa     Remove append_tx_on flag , hack is no longer required
01/06/14   zhw     Access Probe API documentation update
01/03/14   zhw     Tuner AOL. Use common helper API to create callback data
01/03/14   hdz     Enabled temp comp
12/11/13   zhw     CDMA Tuner AOL Support
12/06/13   JJ      use script write for wakeup tx and remove immedate-write flag
12/02/13   sty     Added retune_delay to rf_cdma_debug_flags
11/27/13   sty     Disabled HDET - needs FBRX support
11/21/13   JJ      Added use_imm_writes_for_tx debug flag
11/19/13   JJ      Deleted skip_direct_writes flag
11/18/13   sty     deleted skip_mcpm_stop flag
11/11/13   JJ      Removed rfm_1x_update_fw_smem_addr()
10/24/13   spa     Add use_fake_nv_init debug flag for CDMA
10/21/13   JJ      Added two funcs to set/get fw smem addr
09/23/13   sty     compiler warning fixes
10/10/13   zhw     Skip AOL update in Rx only mode.
09/04/13   zhw     Tuner AsDiv support
08/28/13   JJ      set bolt_rumi to 1 for bolt
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
07/10/13   JJ      Rename lagacy flag rumi_bup_cdma
07/10/13   JJ      remove enable_hdr_rxagc_logging flag
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/28/13   spa     Remove is_irat_mode flag from enable diversity API
06/27/13   spa     Add 'is_irat_mode' flag to APIs to check if API call is in
                   IRAT or non IRAT scenarios
06/14/13   spa     Use CDMA wrapper object for antenna tuner
06/02/13   JJ      HDR sleep time profiling
05/31/13   spa     Compiler warning fix
05/31/13   sty     deleted unused ant_tuner_script flag
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation
05/29/13   JJ      Check Initialized band with RFM_CDMA_MAX_BAND
05/25/13   zhw     Skip mdsp layper call in tuner action api if token is bad
05/23/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/20/13   spa/zhw Move script pointer out of tuner callback data
05/10/13   spa/sty Added debug flag enable_hdr_rxagc_logging
05/09/13   spa     Update XPT PA thresholds when sending update PA params mssg
05/08/13   APU     Removed distracting messages for 1x_filtered_power() APIs
                   due to high frequency of calls from MCS\L1
05/07/13   cd      Temp Comp - Support legacy and xPT temp comp algorithms
05/02/13   Saul    Added debug messages.
04/25/13   spa     Added send_pilot_meas_config debug flag for FTM IRAT
04/24/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
                   - Add debug flag for selective algorithm support
04/24/12   sty     Removed ftm_1x_retune_framevalid_delay
04/18/13   sty/fh  Added value for ftm_1x_retune_framevalid_delay
04/16/13   APU     Changed the name of rf_1x_read_tx_filtered_power() API to
                   rf_1x_get_tx_filtered_power_v2()
04/14/13   APU     Added another get_filtered_tx_power API that can query
                   different kind of filters.
04/10/13   ndb     Removed the unused code.
04/05/13   spa     Enable flag to use new dispatcher
04/05/13   APU     Hooked up the get_filtered APIs to FW interface.
04/02/13   spa     Move to old fwrsp implementaiton till common MDSP migrates
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/29/13   spa     Updated timed sleep API to look at new housekeeping structure
03/26/13   zhw     KW Fix
03/21/13   zhw     Integrate 1x with Antenna Tuner Open Loop common module
03/20/13   cd      XPT Temp comp support
03/15/13   zhw     Remove unnecessary initialization
03/14/13   zhw     Return dBm10 for rfm_1x_get_filtered_rx/tx_power
03/11/13   spa     Updated notify_tx_config to not wait for pa config response
03/01/13   Saul    CDMA. FEATURE_RF_XPT_CDMA_TEMP -> FEATURE_RF_HAS_XPT_SUPPORT
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/21/13   aro     Enabled Tx Resourcer manager for CDMA
02/21/13   aro     Added debug flag to enable tx resource manager for CDMA
02/21/13   Saul    CDMA. Added xpt_temp_comp_enabled debug flag.
02/20/13   zhw     Remove unused API
02/15/13   bmg     Antenna tuner periodic update support.
02/07/13   zhw     CDMA. Bail out rfm_1x_notify_tx_config when Tx is disabled
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/30/13   hdz     Enable temp comp on DIME
01/28/13   APU     Implemented rfm_hdr_get_filtered_rx_power() API.
01/28/13   spa     Remove refs to device_scripting flag
01/25/13   zhw     Disable Temp comp.
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/14/13   Saul    CDMA. 1x EPT/ET call support.
01/14/13   zhw     Enabled CDMA online HDET
01/08/13   shb     Mainlined ssbi_on_off_supported
12/21/12   APU     Enabled ICVR.
12/20/12   APU     Added default statements to case statements to reflect error
12/19/12   zhw     Initial HDET support
12/19/12   adk     KW fixes
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
12/05/12   hdz     Enabled c2k temp comp
11/15/12   aro     Mainlined variable to enable new cal v3 opcode
11/15/12   zhw     Disable ICVR
11/15/12   aro     Variable to enable new cal v3 opcode
11/07/12   zhw     Added a CDMA debug flag for RF device scripting.
11/05/12   sty     use_icvr to enable / disable Iceiver
10/31/12   adk     Merged antenna tuner code from Nikel.
10/26/12   spa     Added flag ssbi_on_off_supported
10/19/12   Saul    CDMA. Removed code to profile writing dpd tables.
10/02/12   bmg     Added filtered Rx and Tx power query APIs
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag.
                   Remove do_rffe_write debug flag
08/30/12   sty     Set ccs_debug to FALSE to stop QXDM spamming
08/25/12   spa     Added CDMA debug flag for direct RFFE write
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/28/12   aro     Interface for antenna tuner action time
08/23/12   aro     Migrated to percise RF time profiler for RFWU Profiler
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/16/12   aro     Temp variable to enable/disable new FW-SW Sync
08/14/12   sty     Disabled temp_comp for bringup
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
08/02/12   zhw     Added a debug flag for ccs events
07/26/12   zhw     Enable CDMA debug flag
07/05/12   spa     Compiler warning fix
07/05/12   zhw     Added RUMI debug flag
07/02/12   spa     Compiler warning fix
06/29/12   spa     Compiler warning fix
05/29/12   zhw     Refactored API naming.
05/29/12   zhw     Moved debug flag do_grfc_write to rf_cdma_data
05/26/12   zhw     Removed cdma debug flag: rf_hdr_wait_time_after_tx_start
05/25/12   spa     Return RFM_CDMA_ERROR_NULL_DATA for any NULL pointers/data
05/19/12   zhw     Moved debug flag do_sbi_write to rf_cdma_data
05/07/12   APU     For the function rfm_1x_get_tx_agc(), for a failed case
                   return -60dBm [0x8000]
04/27/12   zhaow   Fixed formatting
04/26/12   zhaow   Updated data_1x param assignment statement with set APIs.
04/26/12   aro     Converged the TxAGC State param units to dBm10 and dB10
04/26/12   zhaow   Enabled band and channel validity check in 1x prep_wake_rx
                   and retune functions.
04/26/12   aro     Added generic RFM CDMA API status return type
04/24/12   aro     Added implementation of unified RxAGC State param query
04/20/12   aro     Unified interface to return TxAGC State parameter
04/19/12   zhaow   Updated set APIs usage
04/18/12   zhaow   Replaced write pointer assignment statements with set APIs
                   of rf_cdma_data data structure
04/19/12   Saul    1X/HDR. Handoff fix. Zero out GRFC num only once.
04/18/12   spa     Moved rfm_1x_log_wakeup_rx to rf_1x_log.c
04/17/12   sty     Deleted references to enable_sbi_scripting
04/13/12   aro     Check to make sure that there is no diversity device active
                   while doing primary device sleep
04/11/12   aro     OQPCH update to indicate 1xL1 if the timed sleep
                   passed in FW or not
04/09/12   spa     Added rfm_1x_log_wakeup_rx helper function for wu pkt logging
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
03/29/12   sty     use do_grfc_writes_in_rf_sw instead of DO_GRFC_WRITES_IN_RF_SW
                   Added do_grfc_writes_in_rf_sw
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/22/12   aro     Migrated 1x FW response handling to RF_FWRSP task
03/06/12   bm      Updated Tx AGC logging function to return Invalid AGC value
                   when Tx is disabled for the RF device
03/20/12  Saul/aro IRAT. 1x2L 1x Stop Rx sequence change to delay premature
                   RF sleep/vreg shutdown which affects LTE measurements.
03/18/12   aro     Enabled new RF_TASK Dispatcher
03/15/12   spa     Added max device check for rf_cdma_get_device_status()
03/13/12   aro     Added debug flag to enable/disable new RF_TASK dispatcher
03/08/12   cd      Program updated PA params config based on notify Tx config
                   only if there is a change in state
03/02/12   aro     Mainlined Enhanced Rx Calibration
02/29/12   cd      - Added support in rfm_1x_notify_tx_config() to obtain
                   SCH data rate
                   - Cleaned up legacy band nv tbl reference
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan
02/23/12   spa     Updated the type of band/chan to be rfm_cdma_band_class_type
                   and rfm_cdma_chan_type for rf_1x_mc_enable
02/06/12   cd      Removed calv3 multi-read HDET debug flag
02/02/12   sty     Deleted is_voice and pa_backoff flags from rf_cdma_bup_flags_t
                   Use flags from device status in rfm_1x_notify_tx_config()
01/23/12   sty     Deleted references to rf_cdma_debug_flags.btf_from_nv
01/24/12   aro     Documentation Update
01/24/12   aro     Added code to return invalid RF WU for RFC failure
01/23/12   aro     Added RFC interface to query enhanced RF WU time
01/23/12   aro     Added new interface to query the 1x warmup time
01/19/12   cd      Configure Tx AGC uses PLIM API, hence removed argument
01/11/12   aro     Enabled application of LNA Phase offset
01/09/12   aro     Debug flag to enable/disable usage of new LNA phase offset
01/05/12   aro     Upadted timed_sleep function to be a blocking call
01/03/12   sty     Added RFM_DEBUG_EN_Q to rfm APIs
12/21/11   sty     fixed warnings
12/13/11   aro     Check in 1x Enable/Disable to see if all required
                   tasks are running
12/13/11   aro     Check for RFM APIs to see if all required tasks are running
12/10/11   sty     Debug message enhancements
12/06/11   aro     Added debug flag to enable/disable enhanced Tx Calibration
12/05/11   aro     Added debug flag to enable/disable Enhanced Rx calibration
12/05/11   hdz     Enable temp comp for 1xhdr
12/02/11   cd      Added support to configure Tx AGC for access and traffic with
                   new switchpoints
12/02/11   cd      Added debug flag to use default IQ gain
11/23/11   sty     Minor change in F3
11/22/11   sty     Temporarily disable scripting - this is to fix SHDR
                   functionality
11/21/11   aro     Mainlined debug variable to use new RF task dispatch
11/17/11   adk     Reverted for now, FW processing of SBI scripts.
11/15/11   adk     Added support to send RTR SBI scripts to FW selectively.
11/14/11   aro     Mainlined iC task debug flag
11/14/11   aro     Mainlined the debug flags
11/14/11   aro     Enabled New RF Task dispatch for CDMA to support 1x Timed
                   sleep rx
11/14/11   aro     Fixed the OQPCH bug where last active device was not
                   kept track
11/11/11   hdz     Enable temp comp
11/11/11   aro     Moved handling of Timed Sleep Rx to new RF Task dispatch
11/10/11   aro     Added debug variable to enable/disable new RF TASK
                   CDMA dispatch control
11/09/11   cd      Added debug flag to disable temp comp. Disabled by default
11/08/11   bmg     Fixed HDR BTF again, after FW changed their embedded delay.
11/03/11   cd      Add plim data to tx AGC configuration API
11/01/11   aro     Enabled cal tree node-based RC tuner calibration
11/03/11   aro     Enabled iC Task for all targets
11/02/11   av      Use FEATURE_RF_HAS_IC_TASK_DISABLED instead of T_MSM8960
11/01/11   aro     Disabled cal tree node-based RC tuner calibration due to
                   MSM v2 DAL Cal issue
11/01/11   aro     Updated timed_sleep_rx to behave as sleep_rx
10/31/11   aro     Added feature to enable iC Task only for 8960 (Interim)
10/31/11   aro     Enabled cal tree node-based RC tuner calibration
10/28/11   aro     Deleted enable_ic_timer debug flag
10/26/11   aro     Added debug flag to choose between old and new iC polling
                   implementation
10/20/11   bmg     Added rfm_1x_set_tx_open_loop_power() for 1x non-signaling
10/20/11   APU     Implemented the previously empty function
                   rfm_1x_get_band_chan () .
10/18/11   cd      Removed multi_lin_cal debug flag
10/12/11   bmg     Added default max power limit pending lim vs temp static
                   NV integration.
10/05/11   aro     Enabled PA Back-off for 1xDATA. This is required for pass
                   CDP test cases
09/29/11   cd      - Enable HDR FW App from RF, discontinue use of L1
                   - Remove debug flag use_hdr_to_enable_fw
                   - Remove debug flag wait_for_hdr_fw_enable
09/29/11   aro     Added F3 messages for debug assistance
09/27/11   aro     Added debug flag to switch between old and new rc tuner
09/27/11   adk     Moved SSBI scripting-related flags to rf_cdma_debug_flags.
09/26/11   adk     Added debug logs in SSBI scripts.
09/23/11   aro     Removed debug flag to switch between new and old RC Tuner
09/22/11   aro     Enabled new RC Tuner interface
09/21/11   aro     Added DAL based antenna tuner support
09/21/11   aro     Added debug flag to enable/disable new RC Tuner interface
09/21/11   adk     Removed obsolete code.
09/20/11   aro     Replaced the acquire-lock function to CDMA function
09/19/11   sty     reduced F3 level to medium
09/15/11   sty     Changed BTF for HDR
09/13/11   sty     KW fix
09/13/11   sty     Fixed Tx logging APIs for 1x
09/12/11   sty     increased BTF value to 270 from 260 to reduce HDR time error
09/10/11   sar     Fixed KW errors.
09/07/11   aro     Updated timed rx sleep function to use script for device
                   programming
09/07/11   aro     Enabled intelliceiver
09/06/11   sty     Updated documentation
09/07/11   aro     Added MC function to perform timed Rx Stop
09/06/11   sty     Added implementation for rfm_1x_get_tx_pwr_limit()
09/02/11   sty     Implemented TX logging APIs
09/02/11   aro     Added an interface to perform timed Rx sleep
08/30/11   shb     Deleted maxpwr_lim_for_1x/hdr from bu debug flags - handled
                   by power limiting module
08/29/11   aro     Added temporary PA Switchpoint backoff
08/29/11   aro     Added support to fetch BTF value from NV
08/29/11   aro     Added implementation for  PA Access probe config
08/18/11   aro     Disabled intelliceiver timer
08/16/11   cd      Added debug flag to turn on/off usage of multi-linearizer
                   data
08/16/11   sty     Changed BTF value to fix failures seen in time-error
08/12/11   aro     Enabled intelliceiver timer
08/12/11   aro     Fixed diversity calibration crash
08/11/11   aro     Removed intelliceiver enable flag
08/11/11   sty     Added time profiling for RF WU
08/11/11   aro     Added support to allow RF_CDMA_STATE_RX_PREP to
                   RF_CDMA_STATE_SLEEP transition
08/11/11   sar     Fixed KW errors.
08/08/11   aro     Documentation Update
08/04/11   aro     [1] Enable intelliceiver calibration
                   [2] Reduced the pre rxagc sampling time to 2 ms
                   [3] Removed debug flag to override PM for no iC case
07/25/11   sty     Updated implementation for rfm_1x_get_rx_agc()
07/25/11   aro     Added support to perform radio retune in RxTx state
07/13/11   aro     Fixed the CB event for rfm_1x_retune() causing 1xL1 stall
07/13/11   sty     Deleted use_rex_sigs_for_msgr flag
07/13/11   aro     Added debug code to disable intelliceiver timer
07/11/11   cd      Deleted rf_1x_grfc functionality
07/06/11   aro     Updated Lock interfaces to pass the Lock Data
07/05/11   bmg     Changed power limit interface to use dBm10.
06/29/11   sty     Added maxpower limit for 1x and HDR
06/27/11   aro     Removed Cal V3 debug flag
06/22/11   bmg     Added rfm_1x_get_btf()
06/20/11   aro     Added debug variable for intelliceiver debug
06/16/11   sar     Including internal 1x  and common interfaces.
06/15/11   aro     Removed RxAGC Sampling time debug variable
06/13/11   sty     Reworked retune API - moved core implementation to MC layer
06/10/11   sty     Deleted debug flag rf_rx_msg_wait_time
06/09/11   sty     Documentation
06/09/11   aro     Compiler error fix
06/09/11   aro     Added debug flag for Calibration Sweep V3
06/08/11   sty     Fixed ret_val in enable_diversity
06/08/11   sty     Fixed warning and added doc
06/07/11   sty     Changed return type of rfm_1x_validate_state() and made
                   related changes
06/07/11   aro     Replaced Wakeup/Sleep Tx implementation with MC function
06/06/11   aro     [1] Added support for simultaneous Tx support
                   [2] Renamed Load GRFC script to sleep/wakeup grfc script
06/06/11   aro     Renamed Scipt Manager header file to GRFC
06/06/11   sty     [1] Use MC APIs for enable/disable and retune APIs
                   [2] Renamed vars for better readability
06/02/11   aro     Replaced Wakeup/Sleep Rx implementation with MC function
06/02/11   aro     Removed debug flags
06/02/11   sty     Deleted use_hardcoded_tx_lut_vals
06/02/11   aro     Added debug flag to overrride PM for NV query
06/02/11   sty     Deleted rf_rx_msg_wait_time
06/02/11   sty     Minor change in F3 message
06/02/11   aro     Removed CB and userdata from MC interface signature
06/02/11   aro     Moved 1x Enable and Disable implementation to MC
06/01/11   aro     [1] Added debug flag for wait tiem for RxAGC sampling
                   [2] Added debug flag to use QRCT GUI for SV Chain
05/31/11   aro     Removed read_cdma_nv debug flag
05/27/11   sty     Updated debug message
05/24/11   aro     Updated interface to send Tx Chain info to MDSP
05/26/11   aro     [1] Replaced MDSP chain with logical device mapping
                   [2] Added code to update CDMA Data with TXLM buffer
05/26/11   sty     Changed rf_rx_msg_wait_time to 0 - this enables msg router by
                   default
05/26/11   sty     Init rtr_handle to -1 as a temp workaround for FW
05/26/11   sty     Added debug flag - rf_rx_msg_wait_time
05/25/11   sty     Deleted more debug flags
05/24/11   sty     Added debug flag to wait when HDR FW app is enabled
05/24/11   aro     Added Logical Device Mapping to MDSP Chain for SV support
05/24/11   aro     Added debug code to enabvle SV
05/23/11   sty     Added test_wait_after_tx_start
05/22/11   adk     Use modem_mem_alloc/_free() to allocate memory for SBI scripts
05/22/11   aro     Enabled NV
05/21/11   aro     [1] Merged Rx Spectral inversion into configure Rxlm function
                   [2] Deleted Rx Wakeup function
05/21/11   aro     Moved the Spectral Inversion Call to Prep
05/21/11   aro     Replaced old RxLM call for retune function
05/20/11   adk     Generate lists of SBI writes for 1x TX wakeup and sleep.
05/20/11   aro     Fixed Rx Spectral Inversion Bug
05/19/11   sty     Added BTF test hook
05/18/11   aro     Added interface to configure spectral inversion
05/18/11   sty     Increased delay in after Tx_start is sent (debug-only)
05/17/11   sty     Updated rf_cdma_debug_flags
05/16/11   aro     Removed bit shift operation to mathematical operation
05/16/11   aro     [1] Added debug flags to bypass NV
                   [2] Added proper unit conversion for exp RxAGC and Offset val
05/13/11   aro     Added Flag to bypass MCPM in FTM
05/13/11   sty     Removed online_flag from rf_cdma_msm_configure_txlm()
05/12/11   adk     Generate lists of SBI writes for RF_PATH_1 enable and disable.
05/11/11   aro     Added flags to skip Stop Rx and skip enable FW App
05/11/11   sty     Added flags to skip NV read during Rx AGC config
05/10/11   sty     Added bringup flags to skip HDR AGC config from NV
05/10/11   sty     Init bringup workaround flags
05/07/11   adk     Generate list of SBI writes for 1x sleep, to send to MDSP.
04/29/11   adk     Use modem_mem_alloc/_free() to allocate memory for SBI scripts
04/28/11   sty     Invoke rfc_cdma_manage_vregs() correct args
04/28/11   shb     Updated with new CDMA device interface requirements: Call
                   rfdevice_cdma_create_rx_enable_diversity_script() to enable
                   diversity. rfdevice_cdma_create_rx_disable_diversity_script
                   to disable diversity on any RF device
04/28/11   aro     Fixed State Check for Execute Wakeup Rx
04/27/11   sty     Removed redundant check in rfm_1x_enable_diversity
04/26/11   aro     Added Tx Start and Stop Interface
04/25/11   sty     Added calibration state in call to rf_cdma_msm_configure_txlm
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
04/21/11   aro     Moved ADC Clock Manager to HAL
04/21/11   aro     Removed rf_cdma_locks.h
04/21/11   aro     Renamed LOCKs function to move to Common area
04/17/11   sty     Added mode info to rf_cdma_msm_configure_rxlm()
                   Update Rx and TxLM handles in sleep and wakeup APIs
04/14/11   sty     Invoked rfdevice_cdma_create_enable_rx_script() during
                   rfm_1x_enable_diversity()
04/12/11   aro     Replaced activate/dectivate RXLM function with Rx
                   Start and Rx Stop interfaces
04/12/11   aro     [1] Added Rx Wakeup prepare and execute interfaces
                   [2] Added Rx Start and Stop interfaces
04/04/11   aro     Implemented interface to query 1x Sleep state
03/31/11   sty     Invoke rf_cdma_msm_configure_txlm() instead of
                   rf_cdma_txlm_configure_1x()
03/31/11   aro     Added support to query and set TxClosed Loop Power
03/30/11   sty     Fixed warning
03/29/11   sty     Initialized cb_event in rfm_1x_enable_diversity
                   Changed return values on failure
03/29/11   sty     Added implementation for rfm_1x_enable_diversity and
                   rfm_1x_disable_diversity
03/24/11   aro     Defined unique RxAGC Read structure for 1x
03/16/11   sty     Added band_chan arg to rf_1x_configure_tx_agc
03/16/11   aro     Changed Device Param to MDSP Chain
03/16/11   aro     Cleaned up Script Interface
03/15/11   aro     Added consolidated TxAGC Read Interface
03/15/11   aro     Compiler Warning Fix
03/14/11   aro     Consolidated RxAGC Reads to one data structure
03/14/11   aro     Removed activate RxAGC interface
03/11/11   aro     Moved CB to execute CB for all cases
03/10/11   aro     Added CB execution for 1x enable and disable
03/10/11   lp      Fixed Linux build compiler error by removing extra space
03/10/11   aro     Enable Diversity Parameter Validation Bug fix
03/09/11   aro     Added placeholder for Enable Diversity
03/09/11   aro     F3 Message Update
03/09/11   aro     [1] Added consistent RFM time profiling
                   [2] F3 Update
03/09/11   aro     Added Placeholders for 1x enable/disable and div disable
03/09/11   aro     Renamed interface for consistency
03/09/11   aro     Changed code flow to maintain consistency among RFM APIs
03/09/11   aro     Added critical section protection for sleep rx, wakeup tx,
                   sleep tx and retune
03/09/11   aro     Re-structured sleep rx, wakeup tx, sleep tx and retune
                   to have single return point
03/08/11   aro     [1] Added helper function to validate RF state
                   [2] Re-structured wakeup rx interface to have single return
                   [3] Added critical section protection for wakeup rx
03/07/11   sty     Refactored few of rf1x_mdsp* APIs to rf_1x_mdsp*
03/04/11   sty     Added implementation for rfm_1x_get_tx_open_loop()
02/23/11   aro     Compiler Warning Fix
02/18/11   sty     Use rf_timetick_type for time-profiling
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Renamed rfc_card_cdma.h to rfc_cdma.h
02/17/11   aro     Renamed intelliceiver function
02/16/11   aro     Added support to enable RxLM
02/15/11   sty     Updated doxygen comments and debug message format
                   Deactivate Rx and TxLM in sleep APIs
                   Turn off VREGs only after all operations are completed in
                   sleep APIs
02/14/11   sty     Fixed warnings and compile errors
02/09/11   sty     Changed arg in rf_cdma_txlm_configure_1x() from device to
                   TxLM buffer index
02/07/11   sty     Added debug messages in rfm APIs
02/04/11   sty     Added more checks in rfm_1x_sleep_rx and rfm_1x_sleep_tx
02/04/11   sty     Implemented rfm_1x_sleep_rx and rfm_1x_sleep_tx
02/03/11   aro     Moved new Device interface to separate file
02/03/11   sty     Renamed rf_1x_load_grfc_script_for_tx to
                   rf_1x_load_tx_grfc_script
02/02/11   aro     [1] F3 Message Update
                   [2] Updated Sleep Rx to invalidate Band and Chan
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Added State Update function to RFM APIs
01/31/11   aro     Added 1x Wakeup Profiling
01/28/11   aro     [1] RF Message Update
                   [2] Removed Band/Chan Check from TUne function
                   [3] Variable name update
01/27/11   aro     Added support for 1x retune
01/27/11   sty     Added time profiling for rfm_1x_wakeup_tx()
01/27/11   aro     Added RF Warm up Measurement
01/24/11   sty     Renamed rf_1x_load_tx_agc_script to rf_1x_configure_tx_agc
01/23/11   aro     Refined 1x wakeup Rx sequence
01/23/11   sty     Minor re-work in code-flow
01/21/11   aro     Documentation Update
01/20/11   aro     Added new Tx CL Mode and documentation update
01/19/11   aro     Updated Set Tx Closed Loop Power Interface
01/17/11   aro     Removed unnecessary Interface
01/15/11   aro     [1] Added CB execution
                   [2] Updated Device State Validation
                   [3] Removed unnecessary header file
01/15/11   aro     Changed the Tune function name
01/13/11   aro     [1] Moved Device Check at teh start of function
                   [2] Renamed function names
01/12/11   bmg     Updated the 1x API with link manaager parameters
01/10/11   aro     Added Error Code for Error return values
01/08/11   aro     [1] Replaced Manage Vreg with Generic function
                   [2] Added Enums for Wait Time
01/08/11   aro     Created New doxy group
01/08/11   aro     Changed @ingroup to @addtogroup
01/06/11   aro     [1] Added RxAGC Query function
                   [2] Fixed Invalid Band Check
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/04/11   aro     Added NA713 Manage Vreg for Bringup (Will be deleted later)
01/04/11   aro     Added more Error Checking for rfm_1x wakeup_rx()
01/04/11   aro     Added RxAGC Config Load and Activate functions
01/30/11   aro     [1] Added PowerMode arg in rf_1x_load_device_rx_tune_script()
                   [2] Added accessor functions to query and update Dev Status
01/03/11   aro     Fixed Compiler Warning
12/30/10   aro     Added GRFC Script functions
12/29/10   aro     Updated rf_1x_rx_agc_configure() to have band_chan as arg
12/19/10   aro     Updated rf_1x_rx_agc_configure() to have band and PM
12/23/10   aro     Added initial support for rfm_1x_wakeup_rx()
12/16/10   aro     Renamed Get RxAGC function for consistency
12/10/10   aro     Added new interfaces for Tx Power Control
12/06/10   aro     Initial Revision.

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_1x.h"
#include "rfm_cdma.h"
#include "rfm_internal.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_data.h"
#include "rfcommon_math.h"
#include "rf_1x_rx_agc.h"
#include "rf_1x_tx_agc.h"
#include "rf_1x_mdsp.h"
#include "rf_1x_mc.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_mdsp.h"
#include "rf_cdma_mc.h"
#include "rf_1x_power_limiting.h"
#include "rfc_cdma.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_msg.h"
#include "rfcommon_locks.h"
#include "rf_cdma_utils.h"
#include "rfm_cdma_agc_types.h"
#include "sys.h"
#include "rfcommon_time_profile.h"
#include "modem_mem.h"
#include "DALSys.h"
#include "rfm_helper_incs.h"
#include "rf_cdma_mdsp_types.h"
#include "rfm_cdma_band_types.h"
#include "log_codes.h"
#include "rf_1x_log.h"
#include "rfcommon_log.h"
#include "rf_cdma_constants.h"
#include "rfcommon_mc.h"
#include "rfcommon_mdsp.h"
#include "rf_cdma_tx_agc.h"
#include "rfcommon_tuner.h"
#include "rfcommon_mdsp_tuner.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#endif

/*!
  Struct that holds all RF CDMA debug flags
*/
rf_cdma_bup_flags_t rf_cdma_debug_flags =
{
  240,      /* default_max_power_limit */
  0,        /* disable_temp_comp */
  0,        /* use_default_iq_gain */
  1,        /* temp_workaround */
  1,        /* use_icvr */
  0,        /* disable_hdet */
  1,        /* send pilot meas config */
  1,        /* bolt_rumi */
  50,       /* retune_delay - in us */
  0,        /* imm_lna_write_cal */
  0         /*enable_f3_logging*/
};

/*============================================================================*/
/*!
  @name CDMA 1x subsystem RFM Helper Functions

  @brief
  Helper Functions for 1x RFM interface. These interfaces are not exported
  to other modules. They shall be strictly used by RFM 1x functions only.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return type for the rfm_1x_validate_state API. A negative value indicates
  that the function failed.

  @retval RFM_CDMA_HEALTHY_STATE when device state is as expected

  @retval RFM_CDMA_ERROR_INVALID_MODE when device is currently in a mode other
  than expected mode - if the device is is EVDO mode as opposed to
  1X mode

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE when device state is not as
  expected. For ex: if device is expected to be in Rx state, but current state
  of the device is RxTX
*/
typedef int32 rfm_1x_validate_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Validate the RF 1x State

  @details
  This function compares the CDMA internal variables against the expected state
  to indicate the calling RFM function if it is safe to proceed with execution
  of the RFM functionality.
  The return value of this API is not a boolean, as this API can provide more
  info on the cause of the failure (invalid mode, state, etc).

  @param device
  The RF device on which the validation is performed.

  @param exp_state
  The expected RF state.

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code when trying to tune the radio when RF Dev is in Invalid State

  @retval RFM_CDMA_ERROR_INVALID_MODE
  Error code when trying to tune the radio in an invalid Mode

  @return
  Status of RF state. Negative value refers to bad state.
*/
static rfm_1x_validate_t
rfm_1x_validate_state
(
  rfm_device_enum_type device,
  rf_cdma_state_type exp_state
)
{
  rfm_mode_enum_type curr_mode;
  rfm_1x_validate_t return_val = RFM_CDMA_HEALTHY_STATE;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get Current Mode */
  curr_mode =  rfm_get_current_mode(device);

  /* Validate the current Mode */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_validate_state: Bad Mode - %d on device %d ",
              curr_mode, device );
    return RFM_CDMA_ERROR_INVALID_MODE;
  }

  /* Get the device Status for validation */
  dev_status_r = rf_cdma_get_device_status( device );

  /*Perform NULL pointer check*/
  if(  dev_status_r  == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_validate_state: NULL Device %d data",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* State Validation */
  if ( dev_status_r->data_1x.rf_state != exp_state )
  {
    RF_MSG_5( RF_HIGH, "rfm_1x_validate_state : Curr(Dev %d, Mode %d, "
              "State %d), Expected(Mode %d, State %d)", device, curr_mode,
              dev_status_r->data_1x.rf_state, RFM_1X_MODE, exp_state );
    return RFM_CDMA_ERROR_INVALID_DEV_STATE;
  }
  else
  {
    RF_MSG_5( RF_LOW, "rfm_1x_validate_state : Curr(Dev %d, Mode %d, "
              "State %d), Expected(Mode %d, State %d)", device, curr_mode,
              dev_status_r->data_1x.rf_state, RFM_1X_MODE, exp_state );
  }

  return return_val;

} /* rfm_1x_validate_state */
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

  @param band_chan
  A structure containing the band and channel to initialize the path to.

  @param callback
  A function pointer that will be called when the enabling sequence
  is complete.  This function will be passed the RFM_1X_ENABLE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_enable()
  function return until the operation is complete.

  @todo
  Work out what the error return values are for this function.
*/
rfm_wait_time_t
rfm_1x_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_type enable_time; /* TIme spent in function */
  rf_time_tick_type enable_tick; /* TIme spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type chan;

  /*Convert RF CDMA sys band and channel type to RF band and channel type*/
  band = rf_cdma_sys_band_type_to_rf_band_type(band_chan.band);
  chan = (rfm_cdma_chan_type)band_chan.chan_num;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  enable_tick = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_enable, device, rxlm_handle, &band_chan, &callback);

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_1x_enable : START - Dev %d, Band %d, Chan %d", device,
            band, chan );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_SLEEP );

    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Enable 1x */
      ret_val = rf_1x_mc_enable( device, rxlm_handle, band, chan );

      /* Check if MC API was successful to Flag Callback event as successful */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE)
      {
        cb_event = RFM_1X_ENABLE_COMPLETE;
      }
    }
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_enable: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  enable_time = rf_time_get_elapsed( enable_tick, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_5( RF_HIGH, "rfm_1x_enable : [Status %d] - Dev %d, Band %d, "
            "Chan %d, EnableTime %d", ret_val, device, band,
            chan, enable_time  );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_3( RF_ERROR, "rfm_1x_enable: API FAILED. Dev %d,"
              " band %d, chan %d",
              device, band_chan.band, band_chan.chan_num );
  }

  /* update this component of RF WU time */
  rf_cdma_update_wup_time(device, enable_time);

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_enable */

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

  @param callback
  A function pointer that will be called when the disabling sequence
  is complete.  This function will be passed the RFM_1X_DISABLE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_disable()
  function return until the operation is complete.

  @todo
  Work out what the error return values are for this function.
*/
rfm_wait_time_t
rfm_1x_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_tick_type disable_time; /* TIme spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rf_time_type disable_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  disable_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_disable, device, rxlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_disable : START - Dev %d", device );

    /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_SLEEP );

    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Disable 1x */
      ret_val = rf_1x_mc_disable( device, rxlm_handle );

      /* Check if MC API was successful to Flag Callback event as successful */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE)
      {
        cb_event = RFM_1X_DISABLE_COMPLETE;
      }
    }
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_disable: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  disable_time_val = rf_time_get_elapsed( disable_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_3( RF_HIGH, "rfm_1x_disable : [Status %d] - Dev %d, DisableTime %d",
           ret_val, device, disable_time_val  );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_disable: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;
} /* rfm_1x_disable */

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
  Prepare to turn on the radio and tune to the given band and channel.

  @details
  Prepare the software to move the 1x reciever from the Sleep state to the Rx
  state. Powers on all circuits required to recieve an 1x signal and
  prepares all the scripts required to tune the radio to the requested channel.

  For optimal operation, this API should be called with the same band
  and channel the radio was prepared for in the rfm_1x_enable(). This function
  should not be used to re-tune the radio if it already awake.

  @image html rf_cdma_wakeup_rx_sequence.jpg

  @param device
  The receive path to wakeup.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param band_chan
  A structure containing the band and channel to wakeup on.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed the RFM_1X_RX_WAKEUP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

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
rfm_1x_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_type prep_wakeup_time;      /* prep_wakeup Time */
  rf_time_tick_type prep_wakeup_tick;      /* prep_wakeup Time */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type chan;

  /* Start Profiling of API */
  prep_wakeup_tick = rf_time_get_tick();

  band = rf_cdma_sys_band_type_to_rf_band_type(band_chan.band);
  chan = (rfm_cdma_chan_type)band_chan.chan_num;

  RFM_DEBUG_EN_Q(&rfm_1x_prep_wakeup_rx, device, rxlm_handle, &band_chan,
                 &callback);

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_1x_prep_wakeup_rx : START - Dev %d, Band %d, Chan %d",
            device, band, chan );

  /* Check band and channel validity. If fails, set ret_val to error code and */
  /* jump straight to call back. If true, proceed.                            */
  if ( rfm_cdma_is_band_chan_supported( device, band_chan ) == TRUE )
  {
    /* Acquire Critical Section Lock */
    rf_common_enter_critical_section(rf_cdma_get_lock_data());

    /* Query if all required tasks are running or not */
    are_tasks_running &= rf_cdma_required_tasks_running(device);

    /* Perform RF action, only if all required tasks are running */
    if ( are_tasks_running == TRUE )
    {
      /* Validate the Currente State */
      device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_SLEEP );

      /* Proceed only if State is valid, i.e. ret_val is non-negative  */
      if ( device_state == RFM_CDMA_HEALTHY_STATE )
      {
        /* Call 1x Main Control to Prepare Rx for Wakeup */
        ret_val = rf_1x_mc_prep_wakeup_rx( device, rxlm_handle, band, chan,
                                           FALSE /* Not in IRAT mode */ );

        /* Based on Status of MC execution, set CB Event Flag */
        if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
        {
          /* Flag Callback event as successful */
          cb_event = RFM_1X_RX_WAKEUP_PREP_COMPLETE;
        }

      } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
      else
      {
        /* assign the current device state to return value of API */
        ret_val = device_state;
      }
    } /* if ( are_tasks_running == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_prep_wakeup_rx: Dev %d - Tasks not running",
                device );
    } /* if !( are_tasks_running == TRUE ) */

      /* Release Critical Section Lock */
      rf_common_leave_critical_section(rf_cdma_get_lock_data());

  } /* if ( rfm_cdma_is_band_chan_supported( device, band_chan ) == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR,
              "rfm_1x_prep_wake_up_rx: Dev %d Band %d Chan %d - band_chan "
              "validity check failed", device, band, chan );
    ret_val = RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  } /* if !( rfm_cdma_is_band_chan_supported( device, band_chan ) == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* get time spent in API */
  prep_wakeup_time = rf_time_get_elapsed( prep_wakeup_tick, RF_USEC );

  /* Report prep-wakeup time */
  RF_MSG_5( RF_HIGH, "rfm_1x_prep_wakeup_rx: [Status %d] - Dev %d, Band %d, "
            "Chan %d, prep_wakeup_time %d ", ret_val, device,
            band, chan, prep_wakeup_time  );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_3( RF_ERROR, "rfm_1x_prep_wakeup_rx: API FAILED. Dev %d, band %d, chan %d",
              device, band_chan.band, band_chan.chan_num );
  }

  /* update this component of RF WU time */
  rf_cdma_update_wup_time(device, prep_wakeup_time);

  /* Return the API Wait time */
  return ret_val;

} /* rfm_1x_prep_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Wakeup Rx sequence to turn on the radio and tune to the given band
  and channel.

  @details
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

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed the RFM_1X_RX_WAKEUP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

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
rfm_1x_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_type wakeup_exec_time;      /* Warmup Time */
  rf_time_tick_type wakeup_exec_tick;      /* Warmup Time */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start Profiling of Rx Warm Up */
  wakeup_exec_tick = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_exec_wakeup_rx, device, rxlm_handle, &callback,
                 &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_exec_wakeup_rx : START - Dev %d", device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_RX_PREP );

    /* Proceed only if State is valid, i.e. ret_val is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call 1x Main Control to Prepare Rx for Wakeup */
      ret_val = rf_1x_mc_exec_wakeup_rx( device, rxlm_handle,
                                         FALSE, /* not in IRAT mode */
                                         RFDEVICE_MEAS_INVALID_SCRIPT
                                         /* not in IRAT set to invalid*/);

      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_1X_RX_WAKEUP_COMPLETE;
      }
    }
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_exec_wakeup_rx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* Record the execution time statistics for wakeup logging for this device*/
  wakeup_exec_time = rf_time_get_elapsed( wakeup_exec_tick, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_3( RF_HIGH, "rfm_1x_exec_wakeup_rx: [Status %d] - Dev %d, "
            "RxWakeupExecTime %d ", ret_val, device, wakeup_exec_time );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_exec_wakeup_rx: API FAILED. Dev %d",
              device );
  }

  /* update this component of RF WU time */
  rf_cdma_update_wup_time(device, wakeup_exec_time);

  rf_cdma_finalize_wup_time(device);

  /*Populate and commit warmup log packet*/
  rf_1x_log_wakeup_rx(device);

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* Return the API Wait time */
  return ret_val;

} /* rfm_1x_exec_wakeup_rx */

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

  @param device
  The receive path to put to sleep.

  @param rxlm_handle
  The Rx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_1X_RX_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_sleep_rx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure.
*/
rfm_wait_time_t
rfm_1x_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state_1 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rfm_1x_validate_t device_state_2 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rfm_1x_validate_t device_state_3 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rf_time_tick_type sleep_time; /* Time spent in the funxtion */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  boolean div_device; /* Flag to check, if div device is enabled */
  rf_time_type sleep_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start Profiling of Rx Warm Up */
  sleep_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_sleep_rx, device, rxlm_handle, &callback,
                 &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_sleep_rx : START - Dev %d", device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Current State */
    device_state_1 = rfm_1x_validate_state( device, RF_CDMA_STATE_RX_PREP );

    /* Validate the Current State */
    device_state_2 = rfm_1x_validate_state( device, RF_CDMA_STATE_RX );

    /* Validate the Current State */
    device_state_3 = rfm_1x_validate_state( device, RF_CDMA_STATE_SHUTDOWN_FOR_IRAT );

    /* Diversity device should be in Sleep state before proceeding with
    Rx Sleep on master device */
    div_device = rf_cdma_has_associated_device( device );
    /* Proceed only if State is valid, i.e. ret_val is non-negative  */

    if ( ( ( device_state_1 == RFM_CDMA_HEALTHY_STATE ) ||
           ( device_state_2 == RFM_CDMA_HEALTHY_STATE ) ||
           ( device_state_3 == RFM_CDMA_HEALTHY_STATE ) ) &&
         ( div_device == FALSE ) )
    {
      /* Call 1x Main Control to Prepare Rx for Sleep */
      ret_val = rf_1x_mc_sleep_rx( device, rxlm_handle,
                                   FALSE /* Not in IRAt mode */ );

      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_1X_RX_SLEEP_COMPLETE;
      }
    } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      /* assign the current device state to return value of API */
      ret_val = RFM_CDMA_ERROR_FAILURE;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_sleep_rx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* Finalize the RF Warmup Time */
  sleep_time_val = rf_time_get_elapsed( sleep_time, RF_USEC );

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_1x_sleep_rx : [Status %d] - Dev %d, RxSleepTime %d",
            ret_val, device, sleep_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_sleep_rx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_sleep_rx */

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

  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_1X_RX_TIMED_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_sleep_rx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure.
*/
rfm_wait_time_t
rfm_1x_timed_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 rtc_cx1_trigger,
  const boolean is_offline_qpch,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rf_time_tick_type sleep_time; /* Time spent in the function */
  boolean are_tasks_running = TRUE;
  boolean timed_sleep_fw_succes = FALSE;
  rf_time_type sleep_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start Profiling of Rx Warm Up */
  sleep_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_timed_sleep_rx, device, rxlm_handle, rtc_cx1_trigger,
                 is_offline_qpch);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_timed_sleep_rx : START - Dev %d", device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Current State */
    device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_RX );

    /* Proceed only if State is valid, i.e. ret_val is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call 1x Main Control to Prepare Rx for Sleep */
      ret_val = rf_1x_mc_timed_sleep_rx( device, rxlm_handle, rtc_cx1_trigger,
                                         is_offline_qpch );
    } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      /* assign the current device state to return value of API */
      ret_val = RFM_CDMA_ERROR_FAILURE;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_timed_sleep_rx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* If the RF execution was successful, then store the Callback Data
  in the Data structure which will be executed when FW response for Rx Timed
  Sleep is received by RF. If the RF execution failed, then callback will
  be executed immediately. */
  if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
  {
      /* Housekeeping data structure */
      rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;
      /* Use the New housekeeping data structure, since the old FWrsp data
         structure wont be updated when using snum implementation*/
      /* Get FW Response Data */
      fwrsp_hk = rf_cdma_get_fwrsp_data(device);
      if ( fwrsp_hk != NULL )
      {
        /* Get the status of timed sleep success , to check if timed sleep was
        successful or not, if not regular sleep API is called*/
        timed_sleep_fw_succes = fwrsp_hk->rx_stop_rsp.timed_sleep_success;
      }
      else
      {
        RF_MSG_1( RF_ERROR, "rfm_1x_timed_sleep_rx: NULL FWRSP housekeeping "
                            "data for dev %d", device );
        ret_val = RFM_CDMA_ERROR_FAILURE;
      }/*if ( fwrsp_hk == NULL )*/

    ret_val = rf_1x_mc_complete_timed_sleep_rx( device, timed_sleep_fw_succes,
                                                callback, userdata );
  } /* if ( ret_val >= RFM_CDMA_HEALTHY_STATE ) */
  else
  {
    /* Since RF execution failed, execute the Callback function with
    Failure as the callback event*/

    if ( callback != NULL )
    {
      callback( RFM_EXECUTION_FAILURE, userdata );
    } /* if ( callback != NULL ) */

  } /* if ! ( ret_val >= RFM_CDMA_HEALTHY_STATE ) */

  /* Finalize the RF Warmup Time */
  sleep_time_val = rf_time_get_elapsed( sleep_time, RF_USEC );

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_1x_timed_sleep_rx : [Status %d] - Dev %d, "
             "RxSleepTime %d", ret_val, device, sleep_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_timed_sleep_rx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_timed_sleep_rx */

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

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed the RFM_1X_TX_WAKEUP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_wakeup_tx()
  function return until the operation is complete.
*/
rfm_wait_time_t
rfm_1x_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_tick_type wakeup_time; /* holds the time spent in this function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rf_time_type wakeup_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* mark start of function */
  wakeup_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_wakeup_tx, device, txlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_wakeup_tx : START - Dev %d.", device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_RX );

    /* Proceed only if State is valid, i.e. ret_val is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call 1x Main Control to Prepare Tx for Wakeup */
      ret_val = rf_1x_mc_wakeup_tx( device, txlm_handle );

      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_1X_TX_WAKEUP_COMPLETE;
      }

    } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_wakeup_tx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* get time elapsed in this function */
  wakeup_time_val = rf_time_get_elapsed( wakeup_time, RF_USEC );

  /* Report wakeup time, Tx AGC and PA state */
  RF_MSG_3( RF_HIGH, "rfm_1x_wakeup_tx : [Status %d] - Dev %d, TxWakeupTime %d ",
            ret_val, device, wakeup_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_wakeup_tx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_wakeup_tx */

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

  RF will be set to NO TRAFFIC state if this API call is sucessful

  @param device
  The transmit path to put to sleep.

  @param txlm_handle
  The Tx Link Manager buffer that shall be used to configure the
  modem hardware.

  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_1X_TX_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the rfm_1x_sleep_tx()
  function return until the operation is complete. Returns a 0 on success (no
  wait time needed) and a negative value on failure
*/
rfm_wait_time_t
rfm_1x_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_tick_type sleep_time; /* holds the time spent in this function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rf_time_type sleep_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* mark start of function */
  sleep_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_sleep_tx, device, txlm_handle, &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_sleep_tx : START - Dev %d", device );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(device);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State */
    device_state = rfm_1x_validate_state( device, RF_CDMA_STATE_RXTX );

    /* Proceed only if State is valid, i.e. ret_val is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call 1x Main Control to Prepare Tx for Sleep */
      ret_val = rf_1x_mc_sleep_tx( device, txlm_handle );

      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
      {
        /* Set to NO TRAFFIC state */
        rfm_1x_set_pa_access_probe_config( device, FALSE );

        /* Flag Callback event as successful */
        cb_event = RFM_1X_TX_SLEEP_COMPLETE;
      }

    } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_sleep_tx: Dev %d - Tasks not running",
              device );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* get time elapsed in this function */
  sleep_time_val = rf_time_get_elapsed( sleep_time, RF_USEC );

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_1x_sleep_tx : [Status %d] - Dev %d, TxSleepTime %d",
            ret_val, device, sleep_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_sleep_tx: API FAILED. Dev %d",
              device );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_sleep_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Re-tune the 1x radio for a new band and or channel of operation.

  @details
  Changes the operating frequency of the 1x radio.  This function may
  only be called in Rx state or RxTx State.

  If this API is called while the radio is in the Rx or RxTx state, the receiver
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

  @param band_chan
  A structure containing the band and channel to tune to.

  @param callback
  A function pointer that will be called when the entire tuning sequence
  is complete.  This function will be passed the RFM_1X_TUNE_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

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
rfm_1x_retune
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state_1 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rfm_1x_validate_t device_state_2 = RFM_CDMA_HEALTHY_STATE; /* return value of
                                                         device_state check */
  rf_time_tick_type retune_time; /* Re-Tune Time */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type chan;
  rf_time_type retune_time_val;

  /* Start Profiling of Rx Re-tune */
  retune_time = rf_time_get_tick();

  band = rf_cdma_sys_band_type_to_rf_band_type(band_chan.band);
  chan = (rfm_cdma_chan_type)band_chan.chan_num;

  RFM_DEBUG_EN_Q(&rfm_1x_retune, device, rxlm_handle, &band_chan, &callback);

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_1x_retune : START - Dev %d, Band %d, Chan %d",
            device, band, chan );

  /* Check band and channel validity. If fails, set ret_val to error code and */
  /* jump straight to call back. If true, proceed.                            */
  if ( rfm_cdma_is_band_chan_supported( device, band_chan ) == TRUE )
  {
    /* Acquire Critical Section Lock */
    rf_common_enter_critical_section(rf_cdma_get_lock_data());

    /* Query if all required tasks are running or not */
    are_tasks_running &= rf_cdma_required_tasks_running(device);

    /* Perform RF action, only if all required tasks are running */
    if ( are_tasks_running == TRUE )
    {
      /* Validate the Current State. Retune function can be called, only if RF
      is in Rx or RxTx state. */
      device_state_1 = rfm_1x_validate_state( device, RF_CDMA_STATE_RX );
      device_state_2 = rfm_1x_validate_state( device, RF_CDMA_STATE_RXTX );

      /* Proceed only if State is valid, i.e. ret_val is non-negative  */
      if ( ( device_state_1 == RFM_CDMA_HEALTHY_STATE ) ||
           ( device_state_2 == RFM_CDMA_HEALTHY_STATE ) )
      {
        ret_val = rf_1x_mc_retune_rx( device, rxlm_handle, band, chan,
                                      FALSE /*Not in IRAT mode */);

        /* Based on Status of MC execution, set CB Event Flag */
        if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
        {
          /* Flag Callback event as successful */
          cb_event = RFM_1X_TUNE_COMPLETE;
        }

      } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
      else
      {
        /* assign the current device state to return value of API */
        ret_val = RFM_CDMA_ERROR_FAILURE;
      }
    } /* if ( are_tasks_running == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_retune: Dev %d - Tasks not running",
                device );
    } /* if !( are_tasks_running == TRUE ) */

      /* Release Critical Section Lock */
      rf_common_leave_critical_section(rf_cdma_get_lock_data());

  } /* if ( rfm_cdma_is_band_chan_supported( device, band_chan ) == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR,
              "rfm_1x_retune: Dev %d Band %d Chan %d -band_chan validity check "
              "failed", device, band, chan );
    ret_val = RFM_CDMA_ERROR_INVALID_BAND_CHAN;
  } /* if !( rfm_cdma_is_band_chan_supported( device, band_chan ) == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* Record the execution time statistics for wakeup logging */
  retune_time_val = rf_time_get_elapsed( retune_time, RF_USEC );

  /* Report retune time and final Rx AGC and LNA state */
  RF_MSG_5( RF_HIGH, "rfm_1x_retune : [Status %d] - Dev %d, Band %d, "
            "Chan %d, RetuneTime %d ", ret_val, device,
            band, chan,
            retune_time_val  );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_3( RF_ERROR, "rfm_1x_retune: API FAILED. Dev %d, band %d, chan %d",
              device, band_chan.band, band_chan.chan_num );
  }

  /* Return the API Wait time */
  return ret_val;

} /* rfm_1x_retune */

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

  If device1 is in Rx or RxTx mode, slave_dev is woken up if necessary,
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

  @param callback
  A function pointer that will be called when the entire diversity enable
  sequence is complete.  This function will be passed the
  RFM_1X_ENABLE_DIV_COMPLETE event when all actions associated with
  this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the
  rfm_1x_enable_diversity() function return until the operation is
  complete.

  @todo
  Add documentation about possible errors when incompatible devices
  are passed, which cannot operation in diversity mode.
*/
rfm_wait_time_t
rfm_1x_enable_diversity
(
  const rfm_device_enum_type master_dev,
  const lm_handle_type rxlm_handle_master_dev,
  const rfm_device_enum_type slave_dev,
  const lm_handle_type rxlm_handle_slave_dev,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_HEALTHY_STATE; /* Return value */
  rf_time_tick_type enable_div_time; /* TIme spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rf_time_type enable_div_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  enable_div_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_enable_diversity, master_dev, rxlm_handle_master_dev,
                 slave_dev, rxlm_handle_slave_dev);

  /* Throw F3 Message */
  RF_MSG_2( RF_HIGH, "rfm_1x_enable_diversity : START - mDev %d, sDev %d",
            master_dev, slave_dev );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(master_dev);
  are_tasks_running &= rf_cdma_required_tasks_running(slave_dev);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Current State for Master Device. Diversity can be
    enabled when Master Device is either in Rx State ot RxTx State. If the
    expected state is atleast one of them, it is safe to proceed */
    if((rfm_1x_validate_state( master_dev, RF_CDMA_STATE_RX)!=
                               RFM_CDMA_HEALTHY_STATE)
       &&
       (rfm_1x_validate_state( master_dev, RF_CDMA_STATE_RXTX)!=
                               RFM_CDMA_HEALTHY_STATE)
      )
    {
      ret_val = RFM_CDMA_ERROR_INVALID_DEV_STATE;
      RF_MSG_2( RF_ERROR, "rfm_1x_enable_diversity : [Status %d] - MasterDev %d"
                "Not in Rx or RxTx state!!", ret_val, master_dev);
    }

    /* Validate the Current State for slave device */
    if (rfm_1x_validate_state( slave_dev, RF_CDMA_STATE_SLEEP)!=
                               RFM_CDMA_HEALTHY_STATE)
    {
      ret_val = RFM_CDMA_ERROR_INVALID_DEV_STATE;
      RF_MSG_2( RF_ERROR, "rfm_1x_enable_diversity: [Status %d] - SlaveDev %d "
                "Not in sleep state!!", ret_val, slave_dev );
    }

    /* Proceed only if both devices are in their expected states - i.e,
       Master device is in (Rx or RxTx) state and slave device is in Sleep state.

       Checking ret_val against RFM_CDMA_HEALTHY_STATE, to make sure that none
       of the preliminary checks changed ret_val from its initial value of
       RFM_CDMA_HEALTHY_STATE
    */
    if ( ret_val == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call MC fucntion to Enable diversity */
      ret_val = rf_1x_mc_enable_diversity( master_dev, rxlm_handle_master_dev,
                                           slave_dev, rxlm_handle_slave_dev );

      /* Based on Status of MC execution, set CB Event Flag */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE )
      {
        /* Flag Callback event as successful */
        cb_event = RFM_1X_ENABLE_DIV_COMPLETE;
      }

    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_enable_diversity: mDev %d, sDev %d - Tasks "
                        "not running", master_dev, slave_dev );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  enable_div_time_val = rf_time_get_elapsed( enable_div_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_4( RF_HIGH, "rfm_1x_enable_diversity : [Status %d] - MasterDev %d, "
            "SlaveDev %d, EnableDivTime %d", ret_val, master_dev, slave_dev,
            enable_div_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_enable_diversity: API FAILED. MDev %d, SDev %d",
              master_dev, slave_dev );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_enable_diversity */

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

  @param callback
  A function pointer that will be called when the entire diversity disable
  sequence is complete.  This function will be passed the
  RFM_1X_DISABLE_DIV_COMPLETE event when all actions associated with
  this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  The anticipated time in microseconds remaining from the
  rfm_1x_disable_diversity() function return until the operation is
  complete.

  @todo
  Add documentation about possible errors if slave_dev is not currently
  associated with a diversity master.
*/
rfm_wait_time_t
rfm_1x_disable_diversity
(
  const rfm_device_enum_type slave_dev,
  const lm_handle_type rxlm_handle_slave_dev,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_wait_time_t ret_val = RFM_CDMA_ERROR_FAILURE; /* Return value */
  rfm_1x_validate_t device_state; /* return value of device_state check */
  rf_time_tick_type disable_div_time; /* TIme spent in function */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  boolean are_tasks_running = TRUE;
  rf_time_type disable_div_time_val;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Start to Time Measurement */
  disable_div_time = rf_time_get_tick();

  RFM_DEBUG_EN_Q(&rfm_1x_disable_diversity, slave_dev, rxlm_handle_slave_dev,
                 &callback, &userdata);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_1x_disable_diversity : START - sDev %d", slave_dev );

  /* Query if all required tasks are running or not */
  are_tasks_running &= rf_cdma_required_tasks_running(slave_dev);

  /* Perform RF action, only if all required tasks are running */
  if ( are_tasks_running == TRUE )
  {
    /* Validate the Currente State for Slave Device */
    device_state = rfm_1x_validate_state( slave_dev, RF_CDMA_STATE_RXDIV );

    /* Proceed only if State is valid, i.e. ret_val is non-negative  */
    if ( device_state == RFM_CDMA_HEALTHY_STATE )
    {
      /* Call MC fucntion to disable diversity */
      ret_val = rf_1x_mc_disable_diversity (slave_dev, rxlm_handle_slave_dev);

      /* check if MC API was successful */
      if ( ret_val >= RFM_CDMA_HEALTHY_STATE)
      {
        /* Flag Callback event as successful */
        cb_event = RFM_1X_DISABLE_DIV_COMPLETE;
      }

    } /* if ( device_state == RFM_CDMA_HEALTHY_STATE ) */
    else
    {
      /* assign the current device state to return value of API */
      ret_val = device_state;
    }
  } /* if ( are_tasks_running == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_disable_diversity: sDev %d - Tasks not running",
              slave_dev );
  } /* if !( are_tasks_running == TRUE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  }

  /* End Time Measurement */
  disable_div_time_val = rf_time_get_elapsed( disable_div_time, RF_USEC );

  /* Report wakeup time and final Rx AGC and LNA state */
  RF_MSG_3( RF_HIGH, "rfm_1x_disable_diversity : [Status %d] - SlaveDev %d, "
            "DisableDivTime %d", ret_val, slave_dev, disable_div_time_val );

  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_disable_diversity: API FAILED. SDev %d",
              slave_dev );
  }

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  return ret_val;

} /* rfm_1x_disable_diversity */

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
  Return the current 1x Rx AGC in 0.334dBm units

  @details
  This function returns the CDMA Rx AGC setting in 8 bit two's
  complement format (-128...127).

  1x L1 uses this equation to report AGC in log packets:

  Rx strength in dBm = -63.248 + (( rf_get_cdma_rx_agc * 334 ) / 1000 )

  That's -0.334dB per count referenced to -63.248dBm.  Since this returns
  int1, this is a numeric Rx signal strength from -106dBm to -20.83dBm.

  When 102 dB dynamic range is used the AGC value is scaled into the
  85 dB range.  AGC values for powers greater than -21 dBm are saturated
  to -21 dBm (127).  AGC values for powers less than -106 dBm are saturated
  to -106 dBm (-128).

  @param device
  The receive path to query the Rx AGC.

  @return
  Rx AGC in 0.334 dB units, centered at -63.248dBm on success, and -127 on
  failure

*/
int8
rfm_1x_get_rx_agc
(
  const rfm_device_enum_type device
)
{
  rf_1x_mdsp_rx_agc_read_type rx_agc_data; /* Rx AGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 converted_rxagc;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if (logical_dev == NULL)
  {
    RF_MSG_1( RF_LOW, "rfm_1x_get_rx_agc: Could not get a logical device for "
                      "device %d", device);
    return -127; /* lowest RxAGC */
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_rx_agc(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return -127; /* lowest RxAGC */
  }

  /* Get RxAGC Data */
  rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_data );

  /* convert rxAGC to requested format using the below conversion:
     = (0.2994*raw_raxgc*10 - 1.6527)
     the above quantity should be within -127 and +128

    raw_rxagc is read from FW and is in (1/640)dB units

    -128 ==> -105.9dBm and +127 denotes -20.83dBm
  */
  converted_rxagc = (int16) rf_limit_s32 ( (int32)(
             (0.2994*(rf_divide_with_rounding( (int32)rx_agc_data.rx_agc,64)) -
             1.6527)), -128, +127 );

  /* final result */
  return ((int8) converted_rxagc);

} /* rfm_1x_get_rx_agc */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rfm wrapper function to get L1SRAM Sample Memory Address
  for CDMA 1X.

  @details
  Returns the L1SRAM Sample Memory Address for CDMA 1X.

  @param device
  The radio path that is being queried.

  @return
  L1SRAM Sample Memory Base Address.
*/
uint32
rfm_1x_get_mem_pool_base_addr
(
  const rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;  /* Logical Dev */

  if ( device >= RFM_MAX_DEVICES )
  {
	RF_MSG_1( RF_ERROR, "rfm_1x_get_mem_pool_base_addr: Invalid device %d ",
	device);

	return NULL ;
  }

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
	RF_MSG_DBG_1 ( RF_ERROR, "rfm_1x_get_mem_pool_base_addr(): Invalid logical device "
							"for device %d", device);
	return NULL ;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
	RF_MSG_1( RF_ERROR, "rfm_1x_get_mem_pool_base_addr(): Invalid rx_mdsp_path %d",
		 logical_dev->rx_mdsp_path );

	return NULL ;
  }

  return (rf_1x_mdsp_get_mem_pool_base_addr( logical_dev->rx_mdsp_path ));

} /* rfm_1x_get_mem_pool_base_addr */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Rx power filtered over the last hundred milliseconds.

  @details
  Returns Rx power reported by the FW Rx AGC filter.  This filter is intended
  to be a slowly varying record of recent Rx power, and is commonly used as
  an input to a multi-radio concurrency manager.

  The Rx power is filtered by the firmware with a time constant
  around 20ms, but this is not set by the RF driver, so it may
  change without notice.

  @param device
  The radio path that is being queried.

  @return
  Filtered Rx power in dBm10 units.
*/
int16
rfm_1x_get_filtered_rx_power
(
  const rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  int16 filtered_rx_power = -115 ; /* holds the filtered rx_total power */

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_DBG_1 ( RF_ERROR, "rfm_1x_get_filtered_rx_power(): Invalid logical device "
                         "for device %d", device);
    return filtered_rx_power ;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->rx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_filtered_rx_power(): Invalid rx_mdsp_path %d",
              logical_dev->rx_mdsp_path );
    return filtered_rx_power ;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    int16 raw_rx ;

    /* Get per carrier filtered Rx */
    rf_1x_mdsp_get_filtered_rx_power ( logical_dev->rx_mdsp_path ,
                                       &raw_rx ) ;

    filtered_rx_power =  rf_cdma_convert_rxagc ( RFM_CDMA_AGC_FORMAT__DBM10,
                                                 raw_rx );
  }
  else
  {
    RF_MSG ( RF_ERROR, "rfm_1x_get_filtered_rx_power(): Invalid device pointer" );
  }

  return filtered_rx_power ;

} /* rfm_1x_get_filtered_rx_power() */
/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current 1x open loop Tx AGC.

  @details
  This function returns the open loop Tx AGC in 10 bit 2's
  complement format (-512..511).  The return value is sign
  extended to a 16 bit 2's complement number.

  Tx power is:
  Tx in dBm = -(rfm_1x_get_tx_open_loop/10 - 63.8) - TurnaroundConstant

  TurnaroundConstant is defined per 3GPP2 C.S0057, section 2.3.1, Access Channel
  and is -73dB for low bands (BC0, BC3, BC5, etc.) and -76dB for high bands
  (BC1, BC4, BC6, etc.)

  @param device
  The transmit path to query the open loop Tx AGC.

  @return
  Raw TxAGC in mdsp units on success, RFM_CDMA_ERROR_INVALID_MODE on failure

*/
int16
rfm_1x_get_tx_open_loop
(
  const rfm_device_enum_type device
)
{
  /* defines needed for converting Tx_open_loop to corrects units as required
     by L1 */
  #define RF_CDMA_MDSP_TX_OPEN_LOOP_OFFSET 1024
  #define RF_CDMA_MDSP_TX_OPEN_LOOP_MASK 0x3ff
  #define RF_RX_AGC_VAL_OFFSET 512
  #define RX_AGC_VAL_MASK 0x3ff
  #define RX_102_DB_AGC_MIN_VAL 0
  #define RX_102_DB_AGC_MAX_VAL 1023
  #define RX_AGC_VAL_CNTS_PER_DB_85_DB 12
  #define RX_AGC_VAL_CNTS_PER_DB_102_DB 10

  rfm_mode_enum_type curr_mode; /* Current RFM Mode */
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_data; /* TxAGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  int16 tx_open_loop; /* holds the Tx open loop */

  /* Query Current RFM Mode */
  curr_mode = rfm_get_current_mode(device);

  /* Proceed only if the curremt Mode is 1x */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_tx_open_loop: Bad Mode - %d. ", curr_mode );
    tx_open_loop = 0xFFFF;
  }
  else
  {
    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    /* Get the device Status for validation */
    dev_status_r = rf_cdma_get_device_status( device );

    if ( ( logical_dev == NULL ) || ( dev_status_r == NULL ) )
    {
      RF_MSG_1 ( RF_ERROR, "rfm_1x_get_tx_open_loop: logical_dev or "
                           "dev_status_r is NULL for device %d", device );
      tx_open_loop = 0xFFFF;
    }
    else if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
    {
      RF_MSG_1 ( RF_ERROR, "rfm_1x_get_tx_open_loop: Device not in RXTX "
                           "state - current state %d ",
                           dev_status_r->data_1x.rf_state );
      tx_open_loop = 0xFFFF;
    }
    else
    {
      rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                          rf_cdma_data_get_txlm_handle (device) ,
                                          &tx_agc_data );

      tx_open_loop = (tx_agc_data.open_loop_power >> 6);

      tx_open_loop = (tx_open_loop + RF_CDMA_MDSP_TX_OPEN_LOOP_OFFSET) &
                      RF_CDMA_MDSP_TX_OPEN_LOOP_MASK ;

      /* convert to 0..1023 range */
      tx_open_loop =  ( tx_open_loop + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;

      /* saturate to agc values corresponding to min/max for 85 dBm */
      tx_open_loop = (int16) rf_limit_s32( tx_open_loop, RX_102_DB_AGC_MIN_VAL,
                                           RX_102_DB_AGC_MAX_VAL );

      /* scale value and return as -512...511*/
      tx_open_loop = ( ( (tx_open_loop - RX_102_DB_AGC_MIN_VAL) *
            ( RX_AGC_VAL_CNTS_PER_DB_85_DB / RX_AGC_VAL_CNTS_PER_DB_102_DB ))
             + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;

      /* Bit 9 is sign bit...if it is set we need to sign extend */
      tx_open_loop = ( ( tx_open_loop > 511 ) ? tx_open_loop | 0xFC00:tx_open_loop);
    }

  } /* ( curr_mode == RFM_1X_MODE ) */

  return tx_open_loop;

} /* rfm_1x_get_tx_open_loop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query Tx Closed Loop Power Value

  @details
  This function will return the Tx Closed Loop Power value.

  @param device
  The transmit path to query the open loop Tx AGC.

  @return
  1x Tx closed loop power in -1/512th dB.
*/
int16
rfm_1x_get_tx_closed_loop
(
  const rfm_device_enum_type device
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_read; /* Variable for Tx Read */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL == dev_status_r )
  {
    RF_MSG (RF_ERROR, "rfm_1x_get_tx_closed_loop: Null pointer for dev status");
    tx_read.closed_loop_power = 0xFFFF;
  }
  else if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_get_tx_closed_loop: Device not in RXTX "
                         "state - current state %d ",
                         dev_status_r->data_1x.rf_state );
    tx_read.closed_loop_power = 0xFFFF;
  }
  else
  {
    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    if ( logical_dev == NULL )
    {
      RF_MSG( RF_ERROR, "rfm_1x_get_tx_closed_loop: logical_dev is NULL" ) ;
      tx_read.closed_loop_power = 0xFFFF;
    }
    else
    {
  /* Get the Tx Closed Loop Value */
  rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                      rf_cdma_data_get_txlm_handle (device) ,
                                      &tx_read ) ;
  }
  }

  return tx_read.closed_loop_power;

} /* rfm_1x_get_tx_closed_loop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current 1x closed loop adjustment

  @details
  This function returns the closed loop Tx AGC adjustment in 9 bit 2's
  complement format (-256..255).  The return value is sign extended to
  a 16 bit 2's complement number.

  The return value is in -0.25dB resolution.

  @param device
  The transmit path to query the closed loop Tx AGC adjust.

  @return
  Tx AGC closed loop adjustment in -0.25dB units and 0xFFFF on failure

  @todo
  Define what is returned if the transmitter is off.
*/
int16
rfm_1x_get_tx_gain_adjust
(
  rfm_device_enum_type device
)
{
  int16 tx_gain_adj;
  const int tx_closed_loop_offset = 512;
  const int tx_closed_loop_mask = 0x1FF;

  rf_cdma_mdsp_tx_agc_logical_read_type tx_read; /* Variable for Tx Read */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL == dev_status_r )
  {
    RF_MSG (RF_ERROR, "rfm_1x_get_tx_gain_adjust: Null pointer for dev status");
    tx_gain_adj = 0xFFFF;
  }
  else if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_get_tx_gain_adjust: Device not in RXTX "
                         "state - current state %d ",
                         dev_status_r->data_1x.rf_state );
    tx_gain_adj = 0xFFFF;
  }
  else
  {

    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    if ( logical_dev == NULL )
    {
      RF_MSG( RF_ERROR, "rfm_1x_get_tx_gain_adjust: logical_dev is NULL" );
      tx_gain_adj = 0xFFFF;
    }
    else
    {
  /* Get the Tx Closed Loop Value */
  rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                      rf_cdma_data_get_txlm_handle (device) ,
                                      &tx_read );

  tx_gain_adj =  (tx_read.closed_loop_power >> 7);

    tx_gain_adj =  (int16)( (tx_gain_adj + tx_closed_loop_offset) &
                             tx_closed_loop_mask);

  /* Bit 8 is sign bit...if it is set we need to sign extend */
    tx_gain_adj = (int16)( ( tx_gain_adj > 255 ) ? tx_gain_adj |
                             0xfe00 : tx_gain_adj );
  }
  }

  return tx_gain_adj;

} /* rfm_1x_get_tx_gain_adjust */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current 1x Tx max power limit.

  @details
  This function returns the CDMA Tx power limit setting in 10 bit unsigned
  format.

  On success, the returned value is interpreted as follows:

    Returned value               0........1023
                        -52.33 dBm........33 dBm
    Tx power limit       min power........max power

  On failure, the API returns 0 (equivalent to -52.33 dBm)

  @param device
  The transmit path to query the Tx AGC max power limit.

  @return
  Tx AGC max power limit in dB12 units on success and 0 on failure

*/
int16
rfm_1x_get_tx_pwr_limit
(
  const rfm_device_enum_type device
)
{
  int16 max_power_dBm10; /* holds the max power in dBm10 */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean valid_ret_val;
  int16 pwr_lim_db12; /* Tx power limit in db12 units */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_get_tx_pwr_limit: logical_dev is NULL." );
    pwr_lim_db12 = 0; /* -52.33 dBm */
  }

  /* get the current max_power limit */
  valid_ret_val = rf_1x_plim_get_err_adj_max_limit(device, &max_power_dBm10);

  /* do not use value in max_power_dBm10 if above call failed */
  if ( valid_ret_val == FALSE )
  {
    RF_MSG( RF_HIGH, "rfm_1x_get_tx_pwr_limit: "
            "API rf_1x_plim_get_err_adj_max_limit failed - returning -52.33dBm"
            " instead" );
            pwr_lim_db12 = 0; /* -52.33 dBm */
  }
  else
  {
    /*
      OneX L1 logs use this conversion in 0x119C log packet:
      Tx_pwr_lim_dBm = -52.33 + (Tx_pwr_lim_raw / 3)
      where,
      Tx_pwr_lim_raw = (Tx_pwr_lim_from_RF_db12 / 4)

      Thus, Tx_pwr_lim_from_RF = ((Tx_pwr_lim_dBm + 52.33)*3)*4
                               = (Tx_pwr_lim_dBm + 52.33)*12

      In this API, Tx_pwr_lim_from_RF is the same as pwr_lim_db12

      max_power_dBm10*10 ==> converts it to dBm100
      52.33*100 ==> 5233

      pwr_lim_db12 = (max_power_dBm10*10 + 5233) * 12/100

    */
    pwr_lim_db12 = (max_power_dBm10*10+5233)*12/100;
  }

  return pwr_lim_db12;

} /* rfm_1x_get_tx_pwr_limit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current 1x Tx total output power.

  @details
  This function returns the current CDMA Tx AGC output power in 1/10th dBm
  as a 16-bit 2's complement value.  The value returned will be within the
  range 420 (42dBm) to -603 (-60.3dBm) for low bands (BC0 and BC3, etc.),
  or 390 (39dBm) to -633 (-63.3dBm) for high bands (BC1, BC4, BC6, etc.).

  Tx signal strenth in dBm can be computed as:
    Tx strength in dBm = rfm_1x_get_cdma_tx_agc() / 10

  @param device
  The transmit path to query the total Tx AGC.

  @return
  Total 1x transmit power in 1/10th dBm and 0xFFFF on failure

*/
int16
rfm_1x_get_tx_agc
(
  const rfm_device_enum_type device
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_read; /* Variable for Tx Read */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Represents -60 dBm for failed case */
  int16 tx_agc = RF_CDMA_MDSP_MIN_TX_PWR ;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_get_tx_agc: logical_dev is NULL." );
  }
  else
  {
    const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
    int16 K; /* turn-around const */

    /* get read pointer to device - need this to find out current band */
    dev_status_r = rf_cdma_get_device_status( device );

    /*Perform NULL pointer check*/
    if( dev_status_r  == NULL )
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_get_tx_agc: Device %d data is NULL", device );
    } /* if(  dev_status_r  == NULL ) */
    else
    {
      if(dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX)
      {
          /* Get the Tx Closed Loop Value */
          rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                              rf_cdma_data_get_txlm_handle (device) ,
                                              &tx_read ) ;

          /* get turn-around constant for current band */
          K = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

          tx_agc = rf_cdma_convert_txagc( RFM_CDMA_AGC_FORMAT__DBM10, K,
                                          tx_read.total_power);
      }
     } /* if ! (  dev_status_r  == NULL ) */
  } /* ! ( if ( logical_dev == NULL ) ) */

   return tx_agc;

} /* rfm_1x_get_tx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query current Tx State parameter

  @details
  This interface can be used to used to query the current Tx state parameter
  of active RF radio. The Tx data return from this interface will be valid
  only when the radio is in Tx state.

  @param device
  RFM Device for which the Tx state parameter is queried

  @param tx_state_param
  Pointer to the structure containing the Tx State parameter. Calling
  software module will be responsible for allocating the memory
  pointed by the pointer.

  @return
  Flag indicating if the Tx State param populated in tx_state_param is valid
  or not.
*/
rfm_cdma_api_status_type
rfm_1x_get_tx_agc_state_parameter
(
  const rfm_device_enum_type device,
  rfm_1x_tx_agc_state_param_type *tx_state_param
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_mdsp_tx_agc_logical_read_type tx_read; /* Variable for Tx Read */

  int16 tx_open_loop; /* holds the Tx open loop */
  int16 tx_gain_adj_db10;

  int16 max_power_dBm10; /* holds the max power in dBm10 */
  boolean valid_ret_val;

  int16 K; /* turn-around const */
  int16 tx_agc;

  /* Get readable Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* check for NULL pointers */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_tx_agc_state_parameter: Dev %d - Data "
                        "Data", device );
    return RFM_CDMA_ERROR_NULL_DATA;
  } /* if ( dev_status_r == NULL ) */

  /* NULL pointer check on the Tx State param container */
  if ( tx_state_param == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_tx_agc_state_parameter: Dev %d - Invalid "
                        "container", device );
    return RFM_CDMA_ERROR_BAD_PARAM;
  } /* if ( tx_state_param == NULL ) */

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_get_tx_agc_state_parameter(): Dev %d - "
                         "Invalid logical device ", device);
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Check if the device is in Tx STate */
  if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_get_tx_agc_state_parameter: Dev %d - Bad RF "
                        "State %d", device, dev_status_r->data_1x.rf_state );
    return RFM_CDMA_ERROR_INVALID_DEV_STATE;
  } /* if ( dev_status_r->rf_state != RF_CDMA_STATE_RXTX ) */

  /* Get the Tx Closed Loop Value */
  rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                      rf_cdma_data_get_txlm_handle (device) ,
                                      &tx_read );

  /* get turn-around constant for current band */
  K = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

  /*--------------------------------------------------------------------------*/
  /* Tx Gain Adjust */
  /*--------------------------------------------------------------------------*/
  tx_gain_adj_db10 = - 10 *(tx_read.closed_loop_power / 512);

  /* Bit 8 is sign bit...if it is set we need to sign extend */
  tx_state_param->gain_adjust = tx_gain_adj_db10;

  /*--------------------------------------------------------------------------*/
  /* Power Limit */
  /*--------------------------------------------------------------------------*/

  /* get the current max_power limit */
  valid_ret_val = rf_1x_plim_get_err_adj_max_limit(device, &max_power_dBm10);

  /* do not use value in max_power_dBm10 if above call failed */
  if ( valid_ret_val == FALSE )
  {
    RF_MSG( RF_HIGH, "rfm_1x_get_tx_agc_state_parameter: PLIM failed  "
                     "returning -52.33dBm instead" );
    max_power_dBm10 = 0; /* -52.33 dBm */
  }

  tx_state_param->power_limit = max_power_dBm10;

  /*--------------------------------------------------------------------------*/
  /* TxAGC */
  /*--------------------------------------------------------------------------*/
  tx_agc = rf_cdma_convert_txagc( RFM_CDMA_AGC_FORMAT__DBM10, K,
                                  tx_read.total_power);
  tx_state_param->tx_total = tx_agc;

  /*--------------------------------------------------------------------------*/
  /* PA State */
  /*--------------------------------------------------------------------------*/
  tx_state_param->pa_state = tx_read.pa_state;

  /*--------------------------------------------------------------------------*/
  /* Tx Open Loop */
  /*--------------------------------------------------------------------------*/

  tx_open_loop = rf_cdma_convert_txagc( RFM_CDMA_AGC_FORMAT__DBM10, K,
                                  tx_read.total_power);
  tx_state_param->open_loop = tx_open_loop;

  return RFM_CDMA_HEALTHY_STATE;

} /* rfm_1x_get_tx_agc_state_parameter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Tx power filtered over the last hundred milliseconds.

  @details
  Returns Tx power reported by the FW Tx AGC filter.  This filter is intended
  to be a slowly varying record of recent Tx power, and is commonly used as
  an input to a multi-radio concurrency manager.

  The Tx power is filtered by the firmware with a time constant around 100ms,
  but this is not set by the RF driver, so it may change without notice.

  @param device
  The radio path that is being queried.

  @return
  Filtered Tx power in dBm10 units.
*/
int16
rfm_1x_get_filtered_tx_power
(
  const rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 filtered_tx_power = -60 * 10 ; /* holds the filtered tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_get_filtered_tx_power(): Invalid logical device "
                         "for device %d", device);
    return filtered_tx_power ;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_filtered_tx_power(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return filtered_tx_power ;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    int16 raw_txagc , coex_txagc;
    /* int16 coex_power_dBm10 ; */
    int32 tc = rf_cdma_get_turnaround_constant(dev_status_r->curr_band) ;
    rf_1x_mdsp_get_filtered_tx_power( logical_dev->tx_mdsp_path ,
                                      &raw_txagc ,
                                      &coex_txagc ) ;

    filtered_tx_power =
     rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10, tc , raw_txagc ) ;

    /*coex_power_dBm10 =
     rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10, tc , coex_txagc ) ;  */
  }
  else
  {
    RF_MSG ( RF_ERROR, "rfm_1x_get_filtered_tx_power(): Invalid device pointer" );
  }
  return filtered_tx_power;
} /* rfm_1x_get_filtered_tx_power() */
/*! @} */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Tx power filtered over the last hundred milliseconds.

  @details
  Returns Tx power reported by the FW Tx AGC filter.  This filter is intended
  to be a slowly varying record of recent Tx power, and is commonly used as
  an input to a multi-radio concurrency manager.

  The Tx power is filtered by the firmware with a time constant around 100ms,
  but this is not set by the RF driver, so it may change without notice.

  The API supports overall querying filtered Tx power.  Since
  the firmware Tx AGC does not support per-carrier Tx AGC
  filtering, then the function will print an RF error message in
  the F3 log, and return the overall filtered Tx power.  Overall
  filtered Tx power should always be valid.

  @param device
  The radio path that is being queried.

  @param filter_type
  The FW supports few types of Filter
  algorithms. This parameter slects the filter desired by higher
  layers of SW.

  @param format
  The format the Tx power should be returned as.

  @return
  Filtered Tx power in dBm10 format.
*/
int16
rfm_1x_get_filtered_tx_power_v2
(
  const rfm_device_enum_type device ,
  const rfm_filter_type filter_type
)
{
  const int MIN_TX_POWER = -60 ;
  int16 filtered_tx_power =
    MIN_TX_POWER ; /* holds the filtered tx_total power */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get and validate Logical Device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_DBG_1 ( RF_ERROR, "rfm_1x_get_filtered_tx_power_v2(): Invalid logical device "
                         "for device %d", device);
    return filtered_tx_power ;
  }

  /* Validate Rx mdsp path */
  if ( logical_dev->tx_mdsp_path >= RF_CDMA_MDSP_CHAIN_NUM )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_filtered_tx_power_v2(): Invalid tx_mdsp_path %d",
              logical_dev->tx_mdsp_path );
    return filtered_tx_power ;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL != dev_status_r )
  {
    int tc = 0 ; /* Turnaround constant */
    int16 coex_tx_power_dB640 = RF_CDMA_MDSP_MIN_TX_PWR ;
    int16 filtered_power_dB640 = RF_CDMA_MDSP_MIN_TX_PWR ;

    /* Get per carrier filtered Rx */
    rf_1x_mdsp_get_filtered_tx_power( logical_dev->tx_mdsp_path ,
                                       &filtered_power_dB640 ,
                                       &coex_tx_power_dB640 ) ;

    tc = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);

    RF_MSG_DBG_3 ( RF_LOW , "rfm_1x_get_filtered_tx_power_v2(): "
               "| TC: %d | filtered_power_dB640 %d | CoEx_dB640 %d | " ,
               tc , filtered_power_dB640 , coex_tx_power_dB640 ) ;

    /* validate filter type */
    switch ( filter_type )
    {
      case RFM_TX_FILTER_RMS_POWER :
      {
        filtered_tx_power = filtered_power_dB640 ;
        RF_MSG_DBG ( RF_LOW , "rfm_1x_get_filtered_tx_power_v2(): "
                 "Filter Type: RFM_TX_FILTER_RMS_POWER " ) ;
        break ;
      }

      case RFM_TX_FILTER_POWER_WHEN_TX_ON:
      {
          filtered_tx_power = coex_tx_power_dB640 ;
        RF_MSG_DBG ( RF_LOW , "rfm_1x_get_filtered_tx_power_v2(): "
                 "Filter Type: RFM_TX_FILTER_POWER_WHEN_TX_ON " ) ;
        break ;
      }

      default:
      {
        RF_MSG_1 ( RF_ERROR, "rfm_1x_get_filtered_tx_power_v2(): "
                             "Unsupported filter type: %d ", filter_type ) ;
        break ;
      }
    } /* switch */
    filtered_tx_power =
      rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10,
                              tc , filtered_tx_power ) ;
    RF_MSG_DBG_1 ( RF_LOW , "rfm_1x_get_filtered_tx_power_v2(): "
                "filtered Tx power is: %d " , filtered_tx_power ) ;

  } /* if ( NULL != dev_status_r ) */
  else
  {
    RF_MSG ( RF_ERROR, "rfm_1x_get_filtered_tx_power_v2(): Invalid device pointer" );
  }

  return filtered_tx_power ;

} /* rfm_1x_read_filtered_tx_power() */
/*! @} */

/*============================================================================*/
/*!
  @name 1x State Query Functions

  @brief
  Functions to query 1x state
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if a radio path is asleep.

  @param device
  The radio path to get the state from

  @return
  True if the radio is sleeping, otherwise false.
*/
boolean
rfm_1x_is_sleeping
(
  const rfm_device_enum_type device
)
{
  rfm_mode_enum_type curr_mode; /* current Mode */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  boolean cdma1x_sleep; /* Flag indicating CDMA 1x Sleep State */

  /* Get Current Mode */
  curr_mode =  rfm_get_current_mode(device);

  /* Validate the current Mode */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_is_sleeping: Bad Mode - %d on device %d ",
              curr_mode, device );
    return TRUE;
  }

  /* Get the Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /*Perform NULL pointer check*/
  if(  dev_status_r  == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_is_sleeping: Device %d data is NULL.",
              device );
    return TRUE;
  }

  if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_INVALID )
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_is_sleeping: Invalid State - %d on device %d ",
              curr_mode, device );
    return TRUE;
  }

  /* Determine the Sleep Status */
  if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_SLEEP )
  {
    cdma1x_sleep = TRUE; /* 1x is sleeping */
  }
  else
  {
    cdma1x_sleep = FALSE; /* 1x is not sleeping */
  }

  return cdma1x_sleep;

} /* rfm_1x_is_sleeping */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the current band and channel the 1x radio is configured for.

  @param device
  The radio path to get the state from

  @return
  A sys_channel_type structure containing the current 1x band and channel.
*/
sys_channel_type
rfm_1x_get_band_chan
(
  const rfm_device_enum_type device
)
{
  sys_channel_type band_chan_info ;
  band_chan_info.band = SYS_BAND_BC0 ;
  band_chan_info.chan_num = 0 ;

  if ( device < RFM_INVALID_DEVICE )
  {
    /* Get Device Status */
    const rf_cdma_data_status_type *dev_status =
                rf_cdma_get_device_status ( device ) ;

    /* Check NULL Pointer */
    if ( dev_status != NULL )
    {
      band_chan_info.band = rf_cdma_rf_band_type_to_sys_band_type(dev_status->curr_band) ;
      band_chan_info.chan_num = dev_status->curr_chans[0] ;
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_get_band_chan: NULL Device %d data",
                device );
    }
  }
  return band_chan_info ;
} /* rfm_1x_get_band_chan */

/*! @} */

/*============================================================================*/
/*!
  @name 1x Dynamic Tx Control Functions

  @brief
  Functions to control the dynamic behavior or the 1x transmitter
  while in a call.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform the RF driver if the 1x transmitter is in access mode, or in
  traffic mode.

  @details
  The RF driver may need to change the transmitter configuration for
  access and traffic modes in 1x.  This function allows 1x MUX to let
  the RF driver know when it is in access state, and when it is in
  traffic state.

  L1 Can use this API to set RF state to TRAFFIC.
  RF sets itself to Non-TRAFFIC state when rfm_sleep_tx() is successful.

  @param device
  The radio path that is being configured.

  @param is_traffic_state
  Indicates if the transmitter should be in traffic state.  If this
  parameter is true, the radio should be configured for traffic state.
  If this parameter is false, the radio should be configured for
  access state.
*/
void
rfm_1x_set_pa_access_probe_config
(
  const rfm_device_enum_type device,
  const boolean is_traffic_state
)
{
  boolean status = TRUE;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  /* NV related variables */
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Structure to hold PA State Machine Data */
  rf_cdma_mdsp_pa_stm_config_type pa_stm_data;

  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */

  dev_status_r = rf_cdma_get_device_status( device );
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /*Perform NULL pointer check*/
  if(  dev_status_r  == NULL || dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_set_pa_access_probe_config: NULL Device %d data",
              device );
    return;
  }
  else
  {
    /* Update the CDMA data structure indicating that Traffice State is
    enabled */
    rf_cdma_data_set_traffic_state( device, is_traffic_state );

    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    if ( logical_dev ==  NULL)
    {
      RF_MSG( RF_ERROR, "rfm_1x_set_pa_access_probe_config: "
                        "logical_dev is NULL." );
      return;
    }

    /* ensure that NV is valid for given device and band */
    rf_1x_config_ptr = rf_cdma_nv_get_1x_data(
                               logical_dev->tx_nv_path[dev_status_r->curr_band],
                               dev_status_r->curr_band);

    if ( rf_1x_config_ptr ==  NULL)
    {
      RF_MSG( RF_ERROR, "rfm_1x_set_pa_access_probe_config: "
                        "rf_1x_config_ptr is NULL." );
      return;
    }

    /* Reconfigure TxAGC based on Access/Traffic notification if device is
    in RxTx state */
    if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Update the PA Params based on device status */
      status &= rf_1x_tx_agc_update_pa_params (
                         device,
                         dev_status_r->curr_band,
                         dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
                         rf_1x_config_ptr,
                         &pa_stm_data );

      /* We are transitioning from Access to Voice, in this scenario we need
      to update PA switchpoints ONLY and not the Xpt conffig, because for
      Access switchpoints DPD config will be zero, that will force voice
      XPT type into APT type by default every time */
      pa_stm_data.update_xpt_mode = FALSE;

      /* Update PA state params to RFLM */
      status &= rf_cdma_mdsp_config_pa_params(
                             RFM_1X_MODE,
                             dev_status_r->txlm_handle,
                             rf_cdma_xpt_get_cfg_param(device, XPT_CFG),
                             &pa_stm_data );

      if ( status == TRUE )
      {
      RF_MSG_2( RF_LOW, "rfm_1x_set_pa_access_probe_config: "
                "Dev %d, Traffic %d", device, is_traffic_state );
    }
      else
      {
        RF_MSG( RF_ERROR, "rfm_1x_set_pa_access_probe_config: "
                "PA param update failed" );
      }
    }
  }

  return;
} /* rfm_1x_set_pa_access_probe_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query what the power limit would be for a hypothetical reverse link
  channel configuration.

  @details
  This API may be used by MUX to  query what the TX power limit (and
  amount of adjustment due to PA backoff) would be for a particular
  reverse link channel configuration.

  This calculation may take into account other phone state such as
  the current temperature and battery level.

  The results are returned via the out paramter limit, and include
  what the Tx power limit will be based upon the proposed channel
  configuration, and he amount of adjustment included in in the limit
  due to the proposed channel configuration.

  @param device
  The radio path that is being queried.

  @param chan_config
  A pointer to a data structure that contains the current
  configuration for all 1x reverse link channels.

  @param limit
  Output parameter containing the new limit and the limit adjustment.
*/
void
rfm_1x_get_tx_pwr_limit_adj
(
  const rfm_device_enum_type device,
  rfm_1x_txc_chan_config_type* const chan_config,
  rfm_1x_txc_pwr_limit_type* const limit
)
{
  limit->tx_pwr_limit = 843; /* Fixed to 24dBm */
  limit->tx_pwr_limit_adj = 0; /* Fixed to 0 */
} /* rfm_1x_get_tx_pwr_limit_adj */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to query the Tx Lin NV item for the DPD type per PA state

  @details
  This API may be used by the notift Tx config API to query the Tx LIn NV item
  for the DPD type per PA state

  @param device
  The radio path that is being queried.

  @param rf_band
  A pointer to a data structure that contains the current
  configuration for all 1x reverse link channels.

  @param waveform_type
  Waveform type voice/data

  @param dpd_type
  Pointer to array of dpd_type, each PA state can have a different DPD type
*/
static boolean
rfm_1x_get_dpd_type
(
   rfm_device_enum_type const device,
   rfm_cdma_band_class_type const rf_band,
   tx_linearizer_waveform_type waveform_type,
   tx_linearizer_aux_data_type *dpd_type
)
{
  /* Return value */
  boolean api_status = TRUE;
  /* Multilinearizer NV data retrieval */
  const tx_band_cal_type *tx_multilin_nv_cal_ptr = NULL;
  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev;
  /* Counter to loop thru PA states*/
  rfm_pa_gain_state_type pa_state;

   /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Proceed only if pointer to logical device is not null */
  if ( logical_dev != NULL )
  {
    /* Get a pointer to the Tx calibration data */
    tx_multilin_nv_cal_ptr = rf_cdma_nv_get_tx_multilin_cal(
                                            logical_dev->tx_nv_path[rf_band],
                                            rf_band );
    if ( tx_multilin_nv_cal_ptr == NULL )
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_get_dpd_type: tx_multilin_nv_cal_ptr ptr is "
                          "NULL for device:%d", device );
      api_status = FALSE;
    }/*if ( tx_multilin_nv_cal_ptr == NULL )*/
    else
    {
      /* Get DPD type for each PA state */
      for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state <
                                      RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++ )
      {
        /* Access multilin NV cal ptr to get the dpd type */
        /* Use index 0 for getting the dpd_type, we default to the first
        available linearizer for this PA state and waveform type */
        dpd_type[pa_state] = tx_multilin_nv_cal_ptr->
          tx_pa_state_cal_data[pa_state].linearizer_index[waveform_type][0].
                                                        node_data.aux_lin_data;
      }/*for ( pa_state = RFM_PA_GA.....*/
    }/*if ( tx_multilin_nv_cal_ptr != NULL )*/
  }/*if ( logical_dev != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_dpd_type: Dev ptr is NULL for device:%d",
              device );
    api_status = FALSE;
  }/*if ( logical_dev == NULL )*/

  return api_status;
} /* rfm_1x_get_dpd_type */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform the RF driver when the 1x reverse link configuration changes.

  @details
  The RF driver may need to change the transmitter configuration for
  the various reverse FCH, DCCH, and SCH rates.  This function allows
  1x MUX to let the RF driver know when the reverse link configuration
  has changed, so the RF driver can update PA switch points, waveform
  compensations, max power limits, and any other parameters that depend
  on the reverse link channel configuration.

  The chan_config_valid paramater is used to indiciate if MUX has
  provided up to date channel configuration information, or if MUX
  is indicating there is no longer a valid channel configuration
  due to the reverse link being disabled.  If chan_config_valid is
  set to false, the chan_config data structure need be a valid
  pointer, and the internal Tx compensation state will be reset.

  This function may take into account other phone state such as the
  temperature and battery level when compensating for the reverse
  link channel configuration.

  @param device
  The radio path that is being configured.

  @param chan_config_valid
  Indicates if chan_config contains valid data, or if the transmitter
  is no longer is enabled and RF driver should reset the Tx compensation
  state.

  @param chan_config
  A pointer to a data structure that contains the current
  configuration for all 1x reverse link channels.
*/
void
rfm_1x_notify_tx_config
(
  const rfm_device_enum_type device,
  const boolean chan_config_valid,
  const rfm_1x_txc_chan_config_type* const chan_config
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status read Pointer*/
  rfm_cdma_band_class_type rf_band; /* RF band */
  /* NV related variables */
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rf_cdma_mdsp_pa_stm_config_type pa_stm_data; /* PA params holder */
  /* Previous state information holders */
  boolean prev_voice_call_status = FALSE;
  rf_1x_tx_agc_sch_rate_type prev_sch_rate;

  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_notify_tx_config: Dev stauts ptr is NULL" );
    return;
  }

  if( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
  {
    RF_MSG( RF_ERROR, "rfm_1x_notify_tx_config: No config when TX is OFF" );
    return;
  }

  /* Get band */
  rf_band = dev_status_r->curr_band;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev ==  NULL)
  {
    RF_MSG( RF_ERROR, "rfm_1x_notify_tx_config: logical_dev is NULL." );
    return;
  }

  /* ensure that NV is valid for given device and band */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[rf_band],
                                             rf_band);
  if ( rf_1x_config_ptr ==  NULL)
  {
    RF_MSG( RF_ERROR, "rfm_1x_notify_tx_config: rf_1x_config_ptr is NULL." );
    return;
  }
  /* Determine prev Tx config status and update only if necessary */
  prev_voice_call_status = dev_status_r->data_1x.is_voice_call;
  prev_sch_rate = dev_status_r->data_1x.sch_rate;

  if ( ( chan_config->sch_on == TRUE ) && ( chan_config->dcch_on == FALSE ) )
  {
    /* update the data structure */
    rf_cdma_data_set_is_voice_call( device, FALSE );
    /* Obtain the reverse link SCH data rate */
    switch( chan_config->sch_rate )
    {
    case ENC_SCH_FULL_RATE:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_FULL_RATE );
      break;

    case ENC_SCH_2X_RATE:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_2X_RATE );
      break;

    case ENC_SCH_4X_RATE:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_4X_RATE );
      break;

    case ENC_SCH_8X_RATE:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_8X_RATE );
      break;

    case ENC_SCH_16X_RATE:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_16X_RATE );
      break;

    case ENC_SCH_32X_RATE:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_32X_RATE );
      break;

    default:
      rf_cdma_data_set_1x_sch_rate( device, RF_1X_TX_AGC_SCH_FULL_RATE );
      break;
    }
  }
  else
  {
    /* We're in FCH only voice call */
    rf_cdma_data_set_is_voice_call( device, TRUE);
  }

  /* Update MDSP configuration only if there is a state change */
  if ( dev_status_r->data_1x.is_voice_call != prev_voice_call_status ||
       dev_status_r->data_1x.sch_rate != prev_sch_rate )
  {
    tx_linearizer_aux_data_type dpd_type[RFCOM_TXLIN_NUM_LINEARIZERS];

    /* Get the DPD type for each PA state */
    rfm_1x_get_dpd_type( device, rf_band,
                         TX_LINEARIZER_VOICE,
                         dpd_type );

    /* Re-configure the PA params to FW based on updated Tx config.
    No need to reload linearizers and reconfigure Tx AGC */
    rf_1x_tx_agc_update_pa_params( device,
                                   rf_band,
                                   dev_status_r->lo_tuned_to_chan,
                                   rf_1x_config_ptr,
                                   &pa_stm_data );

    /* We are transitioning from Access to Voice, in this scenario we need
      to update PA switchpoints ONLY and not the Xpt conffig, because for
      Access switchpoints DPD config will be zero, that will force voice
      XPT type into APT type by default every time */
    pa_stm_data.update_xpt_mode = FALSE;

    /* Send message to FW with updated information */
    if ( FALSE ==
          rf_cdma_mdsp_config_pa_params (
                             RFM_1X_MODE ,
                             dev_status_r->txlm_handle ,
                             rf_cdma_xpt_get_cfg_param(device, XPT_CFG) ,
                             &pa_stm_data ) )
    {
      RF_MSG( RF_ERROR, "rfm_1x_notify_tx_config(): "
                        "rf_cdma_mdsp_config_pa_params() failed." ) ;
    }
  }/*if ( dev_status_w->data_1x.is_voice_call != p...*/

  RF_MSG_1( RF_LOW, "rfm_1x_notify_tx_config: Dev %d", device );
} /* rfm_1x_notify_tx_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Tx Closed Loop Power

  @details
  This function will configure Tx Closed Loop power operation into specific mode
  of operation. Depending on the tx_cl_mode, Tx closed loop power can be
  overrriden, reset, frozen, or moved back to normal operation.

  When Tx closed loop power is set to normal, Tx AGC closed loop power control
  will follow the reverse link power control commands.

  In case Init mode, Tx AGC closed loop power control input will be
  initialized to the specified value, and will immediately begin following the
  reverse link power.

  When Tx closed loop poer is set to frozen state, Tx AGC closed loop power
  control input will be forced to the  last closed loop power, and will not
  track the reverse link power control commands

  For Override state, Tx AGC closed loop power control input will be forced
  to the specified value, and will not track the reverse link power control
  commands.

  @param device
  The radio path for which Tx Closed Loop Power is to be set

  @param tx_cl_mode
  The type of action to be taken on Tx Closed loop power

  @param tx_cl_pwr
  The Tx Closed Loop Power Value (uint -1/512 dB) to be overrriden to. This
  will be used for the case when Tx closed loop mode is either initialized
  (RFM_1X_TX_CL_INIT) or overrriden (RFM_1X_TX_CL_OVERRIDE). For other modes,
  this parameter will be ignored.
*/
void
rfm_1x_set_tx_closed_loop_power
(
  const rfm_device_enum_type device,
  const rfm_1x_tx_cl_mode tx_cl_mode,
  const int16 tx_cl_pwr
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_set_tx_closed_loop_power: logical_dev is NULL" ) ;
    return;
  }

  /* Get the device status */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( NULL == dev_status_r )
  {
    RF_MSG ( RF_ERROR, "rfm_1x_set_tx_closed_loop_power: "
                       "Null pointer for dev status");
  }
  else if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_set_tx_closed_loop_power: Device not in RXTX "
                         "state - current state %d ",
                         dev_status_r->data_1x.rf_state );
  }
  else
  {
    rf_cdma_mdsp_tx_agc_logical_read_type tx_read; /* Variable for Tx Read */

  /* Based on TX Closed Loop Mode, perform varaious actions */
  switch ( tx_cl_mode )
  {
  case RFM_1X_TX_CL_NORMAL:
    /* Clear TX Closed loop override */
    rf_1x_mdsp_tx_closed_loop_clear_override( logical_dev->tx_mdsp_path,
                                              NULL, NULL, NULL );
    break;

  case RFM_1X_TX_CL_INIT:
    /* Set the Tx Closed Loop Value and start to Track RevLink Power */
    rf_1x_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path, tx_cl_pwr,
                                        TRUE, NULL, NULL, NULL );
    break;
  case RFM_1X_TX_CL_FREEZE:
    /* Set the Tx Closed Loop Value to last value and override */
    rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                        rf_cdma_data_get_txlm_handle (device),
                                        &tx_read );
    rf_1x_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path,
                                        tx_read.closed_loop_power, FALSE, NULL,
                                        NULL, NULL );
    break;
  case RFM_1X_TX_CL_OVERRIDE:
    /* Set the Tx Closed Loop Value to override mode */
    rf_1x_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path, tx_cl_pwr,
                                        FALSE, NULL, NULL, NULL );
    break;
  default:
    RF_MSG ( RF_ERROR,
            "rfm_1x_set_tx_closed_loop_power: Unknown Tx CL. mode" ) ;
    } /* switch ( tx_cl_mode ) */

  } /* else */

  return;
} /* rfm_1x_set_tx_closed_loop_power */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Tx Open Loop Power

  @details
  This function will configure Tx Open Loop power operation into specific mode
  of operation. Depending on the tx_ol_mode, Tx open loop power can be
  overrriden or set to normal operation.  This function is only intended to
  be used to override the Tx power in 1x non-signalling test.

  When Tx open loop power is set to normal, Tx AGC open loop power will
  follow the forward link power.

  For Override state, Tx AGC open loop power will be forced
  to the specified value, and will not track the forward link power.

  @param device
  The radio path for which Tx Open Loop Power is to be set

  @param tx_ol_mode
  The type of action to be taken on Tx Open loop power.

  @param tx_ol_pwr
  Open loop power level to be set in override mode, in dBm10.
*/
void
rfm_1x_set_tx_open_loop_power
(
  const rfm_device_enum_type device,
  const rfm_1x_tx_ol_mode tx_ol_mode,
  const int16 tx_ol_pwr
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 open_loop_txagc;
  int16 k10;

  const rf_cdma_data_status_type *dev_status_r;/* Dev Status Read Ptr*/

  dev_status_r = rf_cdma_get_device_status( device );
  if ( dev_status_r == NULL  )
  {
    RF_MSG_1( RF_ERROR,
              "rfm_1x_set_tx_open_loop_power(): NULL dev status for device %d",
              device );
    return;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev ==  NULL)
  {
    RF_MSG_1( RF_ERROR,
              "rfm_1x_set_tx_open_loop_power(): NULL logical dev for device %d",
              device );
    return;
  }

  switch ( tx_ol_mode )
  {
  case RFM_1X_TX_OL_NORMAL:
    /* Clear Tx Open loop override */
    rf_1x_mdsp_tx_open_loop_clear_override( logical_dev->tx_mdsp_path,
                                            NULL, NULL, NULL );
    break;

  case RFM_1X_TX_OL_OVERRIDE:
    /* Set the Tx Open Loop Value to override mode */
    /* Compute Tx AGC units from the input parameter in dBm10 */
    k10 = 10 * rf_cdma_get_turnaround_constant(dev_status_r->curr_band);
    open_loop_txagc = rf_cdma_mdsp_dbm10_to_txagc(tx_ol_pwr, k10);
    rf_1x_mdsp_tx_open_loop_override( logical_dev->tx_mdsp_path,
                                      open_loop_txagc, NULL, NULL, NULL );
    break;
  default:
    RF_MSG ( RF_ERROR ,
              "rfm_1x_set_tx_open_loop_power(): Unknown Tx OL mode" ) ;
    break ;
  } /* switch ( tx_ol_mode ) */

  return;
} /* rfm_1x_set_tx_open_loop_power() */


/*! @} */

/*============================================================================*/
/*!
  @name 1x Configuration Query Functions

  @brief
  Functions to report the static properties of the current RF
  configuration.

  @details
  These parameters are independent of the band and channel currently
  set.  These functions may be called on a device without it needing
  to be configured for SVDO or 1x mode.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the 1x Tx warmup time in microseconds.

  @param device
  The radio path that is being queried.

  @return
  TX warmup time in microseconds.

  @todo
  Update the encoder driver to convert from microseconds to hardware units.
*/
uint16
rfm_1x_get_tx_warmup
(
  const rfm_device_enum_type device
)
{
  return 0;
} /* rfm_1x_get_tx_warmup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the 1x transmit time correction value, commonly known as the
  "Back To the Future" value.

  @details
  Provides 1x L1 with the transmit time correction value (BTF) for the
  current band on the specified device.  This API may only be called
  after a device is configured to 1x mode and tuned to a channel and
  the transmitter of the device has been enabled. If its called without
  Enabling transmitter, the API will fail.

  The value returned is in CDMA 1/8th chip units, and represents the
  total delay in the analog RF and digital portions of the receiver.
  The purpose of this value is to align the mobile's reverse link
  waveform with the network's forward link waveform.

  @param device
  The radio path that is being queried.

  @return
  Transmit time correction in 1/8th chips if greater than or equal to 0,
  otherwise an error if less than 0.

  @retval RFM_CDMA_ERROR_INVALID_MODE
  Error code if the device is not in RFM_1X_MODE.

  @retval RFM_CDMA_ERROR_INVALID_DEV_STATE
  Error code if the device has not been tuned.
*/
int16
rfm_1x_get_btf
(
  const rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  int16 btf_cx8; /* Tx time correction in cx8 units */
  boolean ret_val; /* Flag to check value returned by MC layer call to query for
                      BTF*/

  /* Get the device Status for validation */
  dev_status_r = rf_cdma_get_device_status( device );

  /*Perform NULL pointer check*/
  if(  dev_status_r  == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_get_btf_delay: NULL Device %d data",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX
       || dev_status_r->curr_band == RFM_CDMA_MAX_BAND)
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_mc_get_btf_delay: Invalid device state %d "
                        "or band %d", dev_status_r->data_1x.rf_state,
                        dev_status_r->curr_band );
    return RFM_CDMA_ERROR_INVALID_DEV_STATE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_btf: Invalid Logical Data on Dev %d",
              device );
    return RFM_CDMA_ERROR_NULL_DATA;
  }

  /* Query MC layer for BTF value, the BTF value returned will have the total
  delay:
  1) Analog Rx delay term (antenna through ADC input)
  2) Digital RxFE delay term (ADC output to mempool I/Q)
  3) Digital TxFE delay term (modulator input to DAC input)
  4) Analog Tx delay term (DAC output through antenna)
 */
  ret_val = rf_cdma_mc_get_btf_delay( RFM_1X_MODE, device, dev_status_r,
                                      logical_dev, &btf_cx8 );

  /* Check if a valid BTF value was returned, if not then return failure */
  if ( ret_val == FALSE )
  {
    return RFM_CDMA_ERROR_BAD_CONFIG;
  }

  RF_MSG_1(RF_LOW, "rfm_1x_get_btf : BTF %d", btf_cx8);

  return btf_cx8;

} /* rfm_1x_get_btf */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the time RF needs for the HW to warmup and be ready for Rx

  @details
  The RF warmup time (RF WU) is a conservative estimate of time the RF SW
  would need to configure the device in a 1x mode. This time estimate may
  be used by L1 to decide when L1 needs to wakeup, in order to ensure that
  the SW and HW is ready for demod. The device does not have to be configured to
  "rf_mode" before calling this API.

  @param device
  RFM transceiver device for which the Rx warmup time is requested

  @param warmup_time
  Pointer to the Structure containing the Rx warmup time breakup. Calling
  function will be responsible to pass a valid pointer to a variable as
  RF will not be allocating memory.

  @return
  Flag indicating the validity of Warmup time

  @retval TRUE
  Flag to indicate that the warmup time time filled in the container is valid

  @retval FALSE
  Flag to indicate that the warmup time time filled in the container is
  not valid. In case of failure, the RF Warmup time will be set to invalid
  times #RFM_1X_RF_WU_INVALID_TIME.
*/
boolean
rfm_1x_get_rx_warmup_time
(
  const rfm_device_enum_type device,
  rfm_1x_rx_warmup_time_type * const warmup_time
)
{

  boolean ret_val = FALSE;

  /* NULL Pointer check */
  if ( warmup_time != NULL )
  {
    ret_val = rfc_cdma_get_enhanced_warmup_time( device, RFM_1X_MODE,
                                                 warmup_time );

    /* Check if the Warmup times returned from RFC are valid or not */
    if ( ret_val == FALSE )
    {
      /* Invalidate RF WU time */
      warmup_time->prep_time_us = RFM_1X_RF_WU_INVALID_TIME;
      warmup_time->exec_time_us = RFM_1X_RF_WU_INVALID_TIME;
      RF_MSG_1( RF_ERROR, "rfm_1x_get_rx_warmup_time: Dev %d - RFC query "
                          "failed", device );
    }

    RF_MSG_2(RF_MED, "rfm_1x_get_rx_warmup_time: [PREP, EXEC] WU Time [%d, %d] "
                     "uS", warmup_time->prep_time_us,
                      warmup_time->exec_time_us );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_get_rx_warmup_time: Dev %d - Invalid WU Ptr",
              device );
    ret_val = FALSE;
  }

  /* Return the status */
  return ret_val;

} /* rfm_1x_get_rx_warmup_time */


/*! @} */

/*============================================================================*/
/*!
  @name Antenna Tuner

  @brief
  This section includes functions to perform actions related to antenna tuner
  algorithm.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Antenna Tuner action time indication

  @details
  This function shall be used by L1 to indicate the antenna tuner action time
  to 1x RF SW. All antenna tuner updates will be aligned with this action
  time invoked by L1. Based on the action time, RFSW will schedule the
  dynamic antenna tuner update to FW on the requested device and its associated
  devices. The dynamic update will be executed only for the case when the
  antenna tuner module has pending scenarios for the given device.

  @param device
  RFM Device for which dynamic antenna tuner update is to be done.

  @return
  Error code indicating the status of the execution of the action time update.
  Negative return value indicates a failure, while non-negative indicates
  success.
*/
int32
rfm_1x_antenna_tuner_action_time
(
  const rfm_device_enum_type device
)
{
  int32 ret_val = RFM_CDMA_HEALTHY_STATE;
  boolean api_status = TRUE;
  static boolean first_action_time = TRUE;
  static rf_time_tick_type prof_tick;
  rf_time_type prof_time = 0; /* Time spent in function */
  rf_time_type time_bet_actions = 0; /* Time spent in function */
  const rf_cdma_data_status_type *m_dev_r;/* Dev Status Read Ptr */
  rfm_cdma_band_class_type band;
  const rfc_cdma_logical_device_params_type *m_logical_dev;

  void* tuner_event_buf;

  #ifdef FEATURE_RF_ASDIV
  /* If AsDiv switching is already in progress. Abort AOL update */
  if (rfcommon_asdiv_is_switch_in_progress() == TRUE)
    {
   RF_MSG(RF_HIGH, "rfm_1x_antenna_tuner_action_time: [ASDIV]Switch in progress, "
                   "Skip update");
      return RFM_CDMA_HEALTHY_STATE;
    }
  #endif

  /* Get Device Status */
  m_dev_r = rf_cdma_get_device_status( device );
  m_logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( m_dev_r == NULL || m_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_antenna_tuner_action_time: Dev %d - NULL "
              "device or RFC data", device );
    return RFM_CDMA_ERROR_NULL_DATA;
  } /* if ( dev_status_r == NULL ) */

  if ( first_action_time == FALSE )
  {
    time_bet_actions = rf_time_get_elapsed( prof_tick, RF_USEC );
  } /* if ( first_action_time == FALSE ) */
  else
  {
    first_action_time = FALSE;
  } /* if ! ( first_action_time == FALSE ) */

  /* Start to Time Measurement */
  prof_tick = rf_time_get_tick();

  /* Try to acquire Critical Section Lock. If the lock cannot be obtained
  right away, bailout and skipp rest of the opration. */
  if( rf_common_try_enter_critical_section(rf_cdma_get_lock_data()) == FALSE )
  {
    /* Throw F3 Message */
    RF_MSG( RF_HIGH, "rfm_1x_antenna_tuner_action_time : Invalid RF state,"
                     "AOL update skipped" );
    ret_val = RFM_CDMA_ERROR_INVALID_DEV_STATE;
  }
  else
  {
    /* get band */
    band = m_dev_r->curr_band;

    if( m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RX ||
        m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RXDIV )
    {
      RF_MSG( RF_MED, "rfm_1x_antenna_tuner_action_time: "
                      "Device in Rx only state. No tune code update " );
    }
    else if( m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
    {
      /* Update tune code for Primary Device. */
      if ( m_logical_dev->ant_tuner[band] != NULL )
      {
        rfcom_band_type_u rf_band;
        boolean is_rx_mode;
        uint16 tuner_mdsp_chain;

        rfcommon_tuner_update_cb_data_type* tuner_cb_data;

        rf_buffer_intf *tuner_script;
        rfcommon_mdsp_event_handle** tuner_event_handle_ptr;

        /* 1. Determin mdsp chain */
        rf_band.cdma_band = band;

        if ( m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RX )
        {
          is_rx_mode = TRUE;
          tuner_mdsp_chain = (uint16)m_logical_dev->rx_mdsp_path;;
        }
        else
        {
          is_rx_mode = FALSE;
          tuner_mdsp_chain = (uint16)m_logical_dev->tx_mdsp_path;
        }

        if( tuner_mdsp_chain >= RF_TUNER_CHAIN_NUM )
        {
          RF_MSG_1( RF_ERROR, "rfm_1x_antenna_tuner_action_time: "
                              "Invalid MDSP chain %d", tuner_mdsp_chain );
          return RFM_CDMA_ERROR_FAILURE;
        }

        /* 2. Create Tuner Update Event Call Back Data Object  */
        tuner_event_buf = rf_cdma_mc_get_aol_tx_dm_buffer ( device ) ;

        tuner_cb_data = rfcommon_tuner_event_create_cb_data(tuner_event_buf, RFM_1X_MODE,
                                                            m_dev_r->txlm_handle);
        if( tuner_cb_data == NULL )
        {
          RF_MSG( RF_ERROR, "rfm_1x_antenna_tuner_action_time: "
                            "CB Buffer Create failed" );
          return RFM_CDMA_ERROR_FAILURE;
        }

        /* 3. Create RF script object */
        tuner_script = rf_buffer_create ( 0, RF_CDMA_MAX_RFFE_SCRIPT_SIZE, 0 ) ;

        tuner_event_handle_ptr = &(tuner_cb_data->tuner_ccs_event_handle);

        /* 5(1). Obtain tuner update RF script on Primary Device */
        #ifdef FEATURE_RF_ASDIV
        /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
        rfcommon_asdiv_manager_enter_lock();
        #endif
        api_status &=
          rf_cdma_atuner_update_tune_code( m_logical_dev->ant_tuner[band],
                                           RFM_1X_MODE,
                                           rf_band,
                                           m_dev_r->lo_tuned_to_chan,
                                           is_rx_mode,
                                           tuner_mdsp_chain,
                                           RFDEVICE_CREATE_SCRIPT,
                                           tuner_script,
                                           0,
                                           (void*)tuner_cb_data );
        #ifdef FEATURE_RF_ASDIV
        /* Releae the ASDIV manager interface */
        rfcommon_asdiv_manager_release_lock();
        #endif

        /* 5(2). Also Update Tune Code for Diversity Device, if applicable  */
        if( api_status == TRUE && m_dev_r->assoc_dev != RFM_INVALID_DEVICE )
        {
          const rfc_cdma_logical_device_params_type *s_logical_dev;
          const rf_cdma_data_status_type *s_dev_r;/* Dev Status Read Ptr */

          s_dev_r = rf_cdma_get_device_status( m_dev_r->assoc_dev );
          s_logical_dev = rfc_cdma_get_logical_device_params( m_dev_r->assoc_dev );

          if( s_logical_dev != NULL && s_dev_r != NULL)
          {
            #ifdef FEATURE_RF_ASDIV
            /* Lock the ASDiv manager interface to avoid concurrency of ASDiv and Tuner configuration*/
            rfcommon_asdiv_manager_enter_lock();
            #endif
            api_status &=
              rf_cdma_atuner_update_tune_code( s_logical_dev->ant_tuner[band],
                                               RFM_1X_MODE,
                                               rf_band,
                                               s_dev_r->lo_tuned_to_chan,
                                               TRUE,
                                               RF_TUNER_CHAIN_1,
                                               RFDEVICE_CREATE_SCRIPT,
                                               tuner_script,
                                               0,
                                               (void*)tuner_cb_data );
            #ifdef FEATURE_RF_ASDIV
            /* Releae the ASDIV manager interface */
            rfcommon_asdiv_manager_release_lock();
            #endif
          }
          else
          {
            RF_MSG( RF_ERROR, "rfm_1x_antenna_tuner_action_time: "
                              "NULL logical dev pointer for slave device " );
            api_status = FALSE;
          }
        }/* if( api_status == TRUE && dev_r->assoc_dev != RFM_INVALID_DEVICE ) */

        /* Check for total number of writes in the script, if 0, skip Tuner AOL
        update*/
        if( rf_buffer_get_num_trans( tuner_script, RF_BUFFER_RFFE ) == 0 )
        {
          RF_MSG( RF_HIGH, "rfm_1x_antenna_tuner_action_time: "
                            "Zero transactions in tune code script. "
                            "Skip AOL update " );
          api_status = FALSE;
        }

        /* 6. Update mdsp data and register callback for FW response.
        If master chain tuner token is not valid, skip and clean up. */
        if( api_status == TRUE )
        {
          *tuner_event_handle_ptr = NULL;
          api_status &= rfcommon_mdsp_tuner_open_loop_queue_script( tuner_event_buf,
                                                                    RF_CDMA_MDSP_CCS_EVENT_AOL_ID,
                                                                    tuner_script,
                                                                    tuner_event_handle_ptr,
                                                                    rfcommon_tuner_open_loop_response_handler,
                                                                    (void*)tuner_cb_data );
        }

        /* 7. If any previous API fails, clean up objects and free memory */
        if ( api_status == FALSE )
        {
          /* No need to continue, force cleaning up all allocated memory */
          rfcommon_tuner_event_cleanup( tuner_cb_data );
          ret_val = RFM_CDMA_ERROR_FAILURE;
        }

        /* 8. Always destroy RF buffer script object after MDSP call flow*/
        if( tuner_script != NULL )
        {
          rf_buffer_destroy( tuner_script );
        }

      }/* if ( logical_dev->ant_tuner[band] == NULL ) */
    }/* else if( RF_CDMA_STATE_RXTX ) */

    /* Release Critical Section Lock */
    rf_common_leave_critical_section(rf_cdma_get_lock_data());

  }
  /* End Time Measurement */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );

  RF_MSG_4( RF_MED, "rfm_1x_antenna_tuner_action_time: [Status %d] Dev %d, "
            "ActionTime %d us, Profile %d", ret_val, device, time_bet_actions,
            prof_time );

  return ret_val;
} /* rfm_1x_antenna_tuner_action_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare RF to make it ready for impending QTA.

  @param device
  The device on which QTA will be performed.

  @param params
  QTA params that need to be configured.

  @return
  Status of function execution.
  TRUE: API Successful.
  FALSE: API will return error.
*/
boolean
rfm_1x_prep_qta
(
   const rfm_device_enum_type   device  ,
   rfm_1x_qta_params*          params
)
{
  boolean result = FALSE ;

  rf_time_type qta_prep_time ;
  rf_time_tick_type qta_prep_tick ;

  /* Start to Time Measurement */
  qta_prep_tick = rf_time_get_tick() ;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section ( rf_cdma_get_lock_data() ) ;

  /* This API will perform all pre-QTA preperations */
  result =  rf_cdma_mc_prep_qta ( device , (rfm_cdma_qta_params*) params ) ;

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* End Time Measurement */
  qta_prep_time = rf_time_get_elapsed( qta_prep_tick, RF_USEC ) ;

  RF_MSG_3 ( RF_MED , "rfm_1x_prep_qta: [Status %d] Dev %d, "
             "TimeSpent %d us ", result, device , qta_prep_time ) ;

  /* API status return */
  return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tell RF that QTA has concluded so that RF can prepare for
  normal opecation.

  @param device
  The device on which QTA was performed.

  @return
  Status of function execution.
  TRUE: API Successful.
  FALSE: API will return error.
*/
boolean
rfm_1x_end_qta
(
  const rfm_device_enum_type   device
)
{
  boolean result = FALSE ;

  rf_time_type enable_time ;
  rf_time_tick_type enable_tick ;

  /* Start to Time Measurement */
  enable_tick = rf_time_get_tick();

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section ( rf_cdma_get_lock_data() ) ;

  /* This API will perform all pre-QTA preperations */
  result =  rf_cdma_mc_end_qta ( device ) ;

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* End Time Measurement */
  enable_time = rf_time_get_elapsed( enable_tick, RF_USEC ) ;

  RF_MSG_3 ( RF_MED , "rfm_1x_end_qta: [Status %d] Dev %d, "
             "TimeSpent %d us ", result, device , enable_time ) ;

  /* API status return*/
  return result ;
}

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */

