
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                          
                    F T M  C O M M O N  D I S P A T C H

GENERAL DESCRIPTION
  This is the FTM Common dispatch file which contains the dispatch function
  for commands common to both 1x and UMTS.

Copyright (c) 2005 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_dispatch.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/15   kma     Added AUTOPIN PAR FTM support
12/04/14   ck      Add diag support for TDS C2/C3
11/13/14   vbh     [1] Add FTM common command for FTM RF scenario setup
                   [2] Add a routine to set the scenario mask based on tool input
11/10/14   kg      Add ftm api for setting concurrency manager debug mode
11/03/14   jps     Add LTE tech API to extract PA switchpoints
11/03/14   jps     Add support for GET_STATIC_INFO
10/27/14   vv      Add papm device info api support
10/09/14   vbh     Remove unused ftm_wcdma_do_rsb_cal
09/25/14   rsr     Fixed OFT failure.
09/22/14   ch      Fix compiler warning
09/03/14   rsr     Added FTM command to return  manufacture and product id of transceiver and LUT id for 
                   requested band/ pa state combintions. 
12/13/13   yb      Fixing the logic to detune all active techs
09/02/14   dr      Added featurization for Non-W builds
08/19/14   ych     Increased common xpt iq cap waiting time
08/11/14   aa      Update run continuous cmd to control fbrx enable/disable
08/10/14   sbo     Add new TRM override command
08/06/14   aro     Support to override ILPC mode of operation
07/31/14   ndb     Added support for extended EARFCN for LTE
07/07/14   zhw     Linking error fix for non-CDMA configuration
07/17/14   jmf     Add api to set self test flag
07/10/14   cd/cri  Merge Pin Cal
07/08/14   kg      CL Optimization 
07/07/14   xsh     Add Internal commands FTM API for RSB cal in opcode implement
06/17/14   sma     Fixed RSB cal issue when there is no diversity
05/06/14   sar     Fixed KW warnings.
04/24/14   zhw     Use rfcommon_mdsp_xpt_mode_enum_type for all DPD loading
04/20/14   bn      Remove obsolete code
04/07/14   ka      Remove obsolete code
02/13/14   ka      Added continuous run api
02/04/14   jc      Added rflm dbg cmd
02/06/14   aa      Set ftm cal state for FBRx
02/04/14   jmf     Adding xpt_mode as arg for ftm_wcdma_xpt_cal_config
01/31/14   jmf     Remove Featurization from FTM_FEEDBACK_SETUP entry function
01/22/14   ka      Added fbrx set params
09/01/14   aa      Added ftm command to save fbrx iq data in efs memmory
12/10/13    aj      Added FTM_FEEDBACK_SETUP Command
12/09/13   cri     Revert RFFE enum type to maintain FTM interface
11/25/13   cd      Add support to load DPD info during Tx override
11/22/13   kg      Fix compiler warnings
11/22/13   id      Load DPD changes for BOLT
11/13/13    ka     Added FTM fbrx common test framework
10/25/13   id      Changes to make Load DPD common for use across modems
09/26/13   aka     Remove rfm_1x_ftm_stop_txagc_update during self test capture
09/20/13   kab     RSB, TxLO order and status fix 
09/10/13   aka     Self test config cmd for TDSCDMA 
09/06/13   ndb     Added "tia_range" in the rl req packet
09/03/13   rmb     Fix to clean up delay char memory after each channel.
08/30/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
08/27/13   jf      Added interface for rxlm/txlm register dump  
08/27/13   aka     1x and WCDMA updates
08/23/13   pk      Removed compiler errors fro GWonly build
08/19/13   spa     Added FTM_COMMON_RADIO_ENTER_MODE command
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/13/13    sk     Added support for compressed ESC Cal Command
08/07/13   sml     FTM changes to support GSM RxD 
08/05/13   ars/rmb New FTM command to exit currently selected FTM RF mode. 
07/29/13   gvn     RSB Cal updates on online mode, iqmc update and IRAT
07/30/13   aka     Self test API updates
07/30/13   aro     Triton compiler error fix
07/30/13   aro     Added WCDMA radio dispatch
07/29/13   aka     Added FTM_SET_FB_PATH_STATE cmd
07/15/13   cri     Immediate MSM register programming for tx override
07/22/13   kb      Update XPT IQ capture API to return sample rate for TDS
07/19/13   cri     Immediate MSM register programming for tx override
07/19/13  ndb      Fix the compiler warnings
07/19/13   ndb     Added support of FTM_PROGRAM_QFE_SCENARIO in online mode 
07/16/13   aro     Added ASDIV FTM mode testing support
07/16/13   dj      Removed legacy Tx internal Cal support
07/15/13   aca     ftm_common_get_tuner_object update for multiple tuner objects
07/12/13   aca     RL cmd defn update
07/10/13   aca     RL API expansion
07/08/13   aca     RL Logging
07/08/13   yzw     Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
06/29/13   hm      Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
07/02/13   Saul    XPT. ET delay compensation
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
06/24/13   php     TxLO Self Cal changes to configure FB Rx path
06/26/13   gvn     SCELL Support for RSB Cal
06/12/13   aca     RL FTM API v2
06/24/13   jmf     [xPT] Save Delay capture to file whether proc fails
06/19/13    pv     Static NV loading bug fix. Impacts B28B ET.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/18/13   ec      Send no response to FTM_ACTIVATE or FTM_DEACTIVATE
06/12/13   aca     RL FTM API
06/03/13   Saul    XPT. Reduced leakage configuration.
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/16/13   aki     Changed CDMA C4 to C3
05/14/13   gvn     LTE RSB Cal STG hook up
05/08/13   bsh     Memory usage: memory allocation based on bands defined
05/13/13   kb      [XPT] Support for IQ capture API for TDSCDMA
05/07/13   jhe     Added TDS tech to mline 
05/06/13   kb      [XPT] support for Load_DPD_Tables API for TDS
05/03/13   aki     Added support for RX WAIT V2
05/01/13   mpt     Added xpt_type field to mline packet to determine env scale
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
04/18/13   kai     Changed wcdma tx agc override api
04/09/13   sr      removed legacy antenna tuner code which is not used.
04/10/13   zg      Hooked up tdscdma rsb function call
04/04/13   ka      WCDMA state machine updates
04/04/13   cri     Poll longer for sample processing to complete
04/03/13   cri     Remove unnecessary debug messages
04/02/13   aro     Command for enable debug level for calv3
03/27/13   kai     Changed ftm_wcdma_xpt_iq_capture interface
03/25/13   adk     Updated FTM_GET_QFE_K_SENSOR_VAL, FTM_GET_QFE_HDET_INCIDENT_PWR
03/21/13  yzw     Add FTM cmd and device driver support to query RFFE device information
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
03/19/13   aro     Deleted ET config path
03/14/13   tks     Added extra parameter to the wcdma rsb function call
03/14/13   sar     Fixed compiler errors in APQ target. 
03/13/13   aca     Tuner characterization
03/11/13   sar     Updated file for APQ, GNSS only target.
03/07/13   cri     Initial support for XPT cal config
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
03/04/13   aro     Command to perform XPT cal config
03/01/13   aro     Added command to perform HW timer test
03/01/13   tks     Fix compiler warning 
02/28/13   tks     Fixed incorrect response packet spec in rsb calibration
02/26/13   sr      Added new FTM_DO_GPIO_CMD cmd to control GPIO and GRFC.
02/26/13   tks     Added support for rsb/txlo leakage ftm command 
02/22/13   dw      Support for FTM common Cal routines without set mode
02/25/13   id      Featurize LTE code in MLine as well for Triton
02/25/13   id      Featurize the MLine code so that Triton is not affected
02/22/13   id      Support for new MLine computation using RMS dBFs
02/22/13   kai     Fixed KW error
02/22/13   kai     Added programming QPOET into APT mode before loading DPD
02/19/13   cd      Revert load DPD table sequence fix
02/17/13   hdz     Compiler warning fix
02/17/13   hdz     ET exit mode programming sequence fix
02/15/13   ac      hdet init fix
02/15/13   tws     Back out previous change.
02/17/13   tws     Handling of FTM activate and deactivate CMD from MMOC task 
                   has been added
02/12/13   jmf     Modify call to XPT IQ sample capture fn to take more params
02/08/13   cri     Get Tx chain info for Tx override from RFC
02/05/13   aca     DIME Tuner programing support
01/30/13   hdz     Added ftm_rsp_data for FTM_XPT_CONFIG cmd
01/30/13   hdz     Added FTM_XPT_CONFIG support
01/28/13   mpt     Fixed ftm_common_xpt_mline_compute() check for null ptr
                   which was causing false failures
01/28/13   kai     Removed ftm_wcdma_set_sapt_state
01/23/13   aro     Command to perform slave resource configuration in 
                   Tx resource manager
01/23/13   mpt     Fixed ftm_common_xpt_mline_compute() crash
01/22/13   aro     Code to use unified IQ capture param
01/21/13   mpt     KW fixes to ftm_common_xpt_mline_compute()
01/17/13   Saul    COMMON. XPT IQ Capture KW Fix
01/16/13   cd      KW fixes to allow for variable payload in load DPD
01/14/13   adk     Added support for CDMA Qtuner OL (temporary solution)
01/10/13   mpt     Updated M-Line Compute
12/27/12   cd      Fixed invalid values for common FW override
12/19/12   Saul    Common. Load DPD crash fix.
12/18/12   cd      Add support to override delay value to FW
12/13/12   Saul    Common. Changed XPT capture, dpd, tx agc override intf.
12/11/12   cri     Move to common DPD buffer allocation
12/19/12   jmf     Remove debug_state dependency from capture/processing/buffer APIs
                   Remove last_trig dependency from free all xpt buffers
12/06/12   jmf     Fixed filename bug in rfcommon_mc_save_xpt_dpd_data()
                   Include rfcommon_core_xpt_buffer.h
                   Implemented 1x multi sample capture
12/06/12   aki     Added TDSCDMA support to RX_WAIT and use RFM_DEVICE_4
12/04/12   aka     Removing Triton specific featurization for FTM GSM C2 cmd
12/03/12    vb     Code to support WCDMA Tuner OL - temporary solution
11/29/12   tws     Add device to FTM GSM.
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/26/12   mpt     Added Compute M-Line command
11/16/12   Saul    Sample Capture. Save XPT DPD data.
11/16/12   Saul    Sample Capture. Warning fixes.
11/16/12   Saul    Sample Capture. Use common response.
11/16/12   cri     Updates for LTE multiple sample capture
11/15/12   dj      Added new xo calibration command FTM_XO_CALIBRATION
                   Removed support for legacy XO Cal commands
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/09/12   adk     Updated QFE1510 HDET code.
11/09/12   kai     Added WCDMA xPT Tx AGC override
11/08/12   cd      Fixed compiler error
11/08/12   cd      - Common XPT - DPD commit now overrides FW to latch the 
                   tables
                   - Added common dispatch for xPT Tx AGC override
11/05/12   Saul    Common XPT Sample Capture. Warning Fix.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/26/12   kai     Removed rfdevice_hdet_cmn_do_k_sensor_cal
10/15/12   kai     Changed QFE1510 HDET APIs
09/25/12   adk     Added support for antenna tuner programming in GSM mode.
09/21/12   kai     Added FTM_PROGRAM_QFE_SCENARIO removed FTM_COMMON_SET_QFE_SCENARIO
10/30/12   spa     Use RFM_DEVICE_3 as diversity device on SV path
10/26/12   Saul    Common XPT. Added num elements check.
10/25/12   cd/Saul Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.
10/24/12   bn      Enable W sample capture for XPT
10/17/12   kai     Changed QFE1510 HDET gain state type to rfdevice_hdet_gain_state_type
10/12/12   Saul    Common XPT. Load DPD framework.
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/14/12   jfc     Move RFFE HWTC to regular FTM common area 
09/13/12   jfc     Beautify this file. Indentation was horrible. 
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/01/12   aro     Added EPT override Test function
07/18/12   jps     TD-SCDMA RF Cal should use FTM_SET_CALIBRATION_STATE
07/10/12   kai     Add support for Qtuner FTM command
06/21/12    vb     Added support for QFE1510
06/08/12   aro     EPT IQ capture command
06/08/12   cd      Fix for incorrect first HKADC reading in thermistor cal 
                   in CDMA mode (SVOC)
06/07/12   aca     LTE XO cal featurization update from FEATURE_FTM_HAS_LTE 
                   to FEATURE_LTE
06/04/12   vvr     Fix for Off Target compile issues
06/01/12   sbo     Added missing break statement.
05/17/12   aca     Changed LTE XO cal featurization
05/16/12   sa      Added FTM_GET_ENH_XO_THERM Command Support
04/23/12   cri     Fix featurization for DPD
05/08/12   aki     Mainline FTM_RX_WAIT_DBM code, fix log messages
05/03/12   id      Additional Support for Internal Cal and cleanup
05/03/12   jr      Removed Klocwork warnings
04/18/12   ars     Modifed FTM frequency adjust logic to apply correction 
                   using XO manager API
04/11/12   id      Fixed compiler warning
04/10/12   id      Additional support for Tx Internal Cal sweep and results
04/09/12   sb      Add changes to fix frequency error adjustment in FTM mode
04/05/12   vb      Added function to perform DAC cal only.
04/04/12   pl      Remove obsolete SMPS interface
03/22/12   id      Added support for Tx Internal Device Cal command
03/21/12   aca     LTE XO cal support
03/18/12   aro     Created main common function to dispatch radio commands
                   to specific techs
03/18/12   aro     Added function to set the SAR state
03/18/12   aro     Created common radio dispatch
03/07/12   cri     Added initial DPD support
02/16/12   pv/ars  Added FTM Error Fatal Command. 
02/14/12   sar     Reinstated TDSCDMA features.
02/07/12   sar     Feature reduction changes.
01/27/12   ac      fix for TX getting turned off during XO cal    
01/26/12   gh      Update ftm_wcdma_deallocate_rxlm to take in chain as arguement
01/17/12   aro     Added "New Mode" as a argument in FTM EXIT function
01/16/12   aro     Added generic command to query THERM
01/06/12   aki     In RX_WAIT_DBM call ftm_wcdma_get_rx_level_dbm
01/5/11    ems     Correct XO cal exit mode to eliminate reset
12/21/11   dw      Legacy code cleanup
11/29/11   aki     Added FTM_RX_WAIT_DBM 
11/29/11   tnt     Fix KW warning       
11/23/11   hy      XO Cal clean up for TDSCDMA
11/21/11  tnt      Support for Ant Tuner Overwrite cmd thru FTM
11/17/11   sb      Added FTM function to update frequency error
11/17/11   hy      Added TDSCDMA V2/V3 Cal
11/07/11   sb      Added valid wait function to make FTM_LOAD_RF_NV work
11/03/11   aak     Turn on vregs before doing therm cal     
09/29/11   ac      xo cal fix for wcdma on EU
09/25/11   vb      Error check for FTM_DO_INTERNAL_DEVICE_CAL     
09/25/11   vb      Support for FTM_DO_SELF_CAL                      
09/12/11   sar     Fixed KW errors.     
08/15/11   pl      merge to genesis PLF118
08/04/11   vb      Added FTM common Internal device calibration support
07/14/11   dj      Fix for XO fine cal failure
07/11/11   hy      Including TD-SCDMA support.
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
06/28/11   aro     Renamed header file to ftm_common_calibration_v3
06/25/11   aro     [1] Renamed function names
                   [2] Documentation Update
06/25/11   aro     Added support to fetch CalV3 results
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/09/11   pl      Move TX_RX composite cal to use new ftm_rfmode 
06/09/11   aro     Fixed the argument for Calibration Sweep
06/09/11   vb      Removed retune of channel for XO Cal in 1x mode. Also removed
                   the XO LM buffer request for 1x.
06/07/11   shb     Use new SV compliant RF device interface for Therm read.
                   Currently hardcoding therm reads to RFM_DEVICE_0
06/05/11   vb      Removed tech specific XO buffer and using the default buffer of each
                   tech for XO Cal
06/02/11   sty     Mainlined FEATURE_RF_SVDO_API
05/24/11   vb      [1] ftm_xo_cal_enter_mode added for fine XO Cal
                   [2] ftm_xo_cal_enter_mode removed for collect sample
                   [3] Implement tech specific XO cal exit mode
05/19/11   aro     [1] Added function to configure XO Cal Mode for 1x
                   [2] Added SWITCH case for XO Cal enter mode
05/17/11   vb      RxLM support for XO Cal
05/16/11   vb      Added support for XO Cal V2 (framework imported from Poseidon code base)
05/13/11   sar     Included ftmicap.h.
05/09/11   sar     Included ftm_rf_cmd.h for rfa/api cleanup.
05/06/11   aro     Added support for Calibration Sweep
03/20/11   anr     APT support for 1x
03/13/11   tnt     LTE APT integration 
03/24/11   aro     Renamed FTM system control filename
03/02/11   aro     Moved Set/Get Calibration state interface to RF
02/25/11   ka/dw   Correct size of composite cal return type  
02/22/11 whc/can   Initial APT support.
02/03/11   aro     Moved new Device interface to separate file
01/27/11   ka      Correct size of composite cal return type 
12/16/10   aro     Moved Calibration to 1x Module
12/09/10   aro     Fixes for Zero-Compile Error SVDO Enabled Build
12/07/10   aro     Added SVDO supported APIs
10/21/10   tsr     GSM Thermal Read API support in FTM.
10/19/10   vb      All device functions are now accessed thru device interface functions 
15/10/10   mkv     Support LTE Composite Calibration  
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
08/05/10   anr     Added support for Enhanced Therm API in 1x mode
04/13/10   adk     Merged branch scmm_mdm9k_merge_final
03/07/10   wen     Fixed compilation errors
02/12/10   xw      Klocwork fix 
01/28/10   sar     Lint fixes.
01/22/10   sar     Removed compiler warnings.
01/13/10   dyc     Warning cleanup and remove SVDO featurization/comments
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
10/24/09   id      Add FTM function for ENH XO Sample collection
10/02/09   zg      Backed out SVDO FTM related changes.
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/14/09   rsr     Bug Fix :- Update Timeout to tcxo manager during XO fine cal. 
09/09/09   aro     Featurized Code with FEATURE_RF_SVDO_API
                   + Added Old Set Cal Mode API
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/22/09   aro     Re-structured Set RF CAL Mode
07/20/09   aro     Function/variable names changed
07/10/09   aro     Re-organized 1xHDR FTM Files
07/09/09   aro     Added Support to do context switch to FTM Task when Timer ISR calls
                   FTM functions
07/08/09   dyc     Restore the XO calibration changes. 
07/07/09   bn      Adding FTM APIs for RX/TX resolution 
07/02/09   aro     Added SVDO Composite Cal Support
06/25/09   aro     Update Set Cal Mode API to return error message for failures
06/23/09   aro     Fix in Setting RF Cal Mode
06/17/09   lp      Fixed WCDMA featurization for ftm_do_internal_device_calibration
                   Backed out XO calibration changes
06/15/09   ns      XO calibration changes for CDMA SCMM bringup 
06/09/09   aro     Added Support for RF Calibration State
04/20/09   aro     FTM-SVDO Support : Change in API name
                   Compiler Warning Fixes
03/24/09   aro     Initial SVDO Support
02/09/09   anr     Added the enhanced XO cal APIs.
01/29/09   aak     Added comments to the code
10/06/08   ap      Modified therm_val to uint8 to match FTM_GET_THERM api & fix Get_therm functionality 
09/29/08   jfc     Moved therm_val inside case to remove compiler warning in C2K
07/14/08   adk     Merged QSD8650 code.
05/29/08   adk     Ported DORB changes from the 7800 branch
05/19/08   ad      Fix fatal error crash on ADS compilers due to overcasting pointers
03/06/08   jtn     Fix Klocwork errors.
03/03/08   ad      Therm cal changes for code clean-up and lint error fix
02/22/08   dw      Fixed high Lint warnings (644, 645)
02/20/08   dw      Fixed critical Lint warning (418)
01/21/08   ad      Support ftm_gsm_toggle_modes() for EDGE only support
01/09/08   ch      Adding function FTM_GET_THERM to put therm ADC in common dispatch vs wcdma
12/21/07   rmd     Change necessary to allow MSM7500-90 to compile.
11/26/07   adk     Called ftm_set_calibration_state() only in the 1x mode.
11/20/07   adk     Merged changes from FTM tips.
10/13/07   anr     Supprot for FTM_SET_CALIBRATION_STATE 
10/03/07   bmg     Protect 1x only builds from UMTS features
08/01/07    kg     featurization changes for EDGE only support
07/23/07   anr     XO Cal APIs append their own headers. 
07/15/07   dyc     Added rf_init_rf_nv_items() prototype.
05/16/07   anr     Added FTM_DO_XO_DC_CAL and FTM_DO_XO_FT_CURVE_CAL 
01/22/07   jfc     Changed T_MSM6280 to FTM_HAS_WCDMA
01/19/07   sar     Added call to rf_init_rf_nv_items() in ftm_common_dispatch();
10/17/06   ycl     Modifications for initial MSM7600 build.
09/21/06   sar     Adding case for INACTIVE items fo rfnv_retrieve_nv_items.
09/21/06   sar     Added switch statement to properly handle the return value from
                   rfnv_retrieve_nv_items() in ftm_common_dispatch.
04/19/06   ka      Updated Tx-Rx API for diversity/lpm changes.
02/07/06   ka      Added FTM_DO_INTERNAL_DEVICE_CAL.
12/05/06   hkk     Fixes for compile warnings 
11-17-05   ka      Added FTM_LOAD_RF_NV.
08-19-05   ka      Created
===========================================================================*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "ftmicap.h"
#include "stringl.h" /* for memscpy */
#include "ftm_calibration_data_module.h"
#include "ftm_common_data.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "rex.h"
#include "task.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_common_dispatch.h"
#include "ftm_common_selftest.h"
#include "ftm_common_autopin.h"
#include "ftm_common.h"
#include "rf_test_task_v.h"
#include "rfcommon_nv.h"
#include "rfm.h"
#include "rfm_internal.h"
#include "ftm_rf_cmd.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rxlm_intf.h"
#include "rflm_api_dtr_rx_fw.h"
#include "rxlm_test.h"
#endif
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "rfc_card.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "ftm_common_radio.h"
#include "ftm_cdma_cmd.h"
#include "ftm_rffe.h"
#endif
#include "ftm_gpio.h"
#include "modem_mem.h"
#include "rfcommon_mc.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "ftm_common_xpt.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_core_xpt.h"
#endif
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_utils.h"

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_ctl.h"
#include "rfdevice_wcdma_intf.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_msm.h"
#include "rfdevice_wcdma_pa_intf.h"
#include "rfdevice_wcdma_asm_intf.h"
#include "ftm_wcdma_dispatch.h"
#include "ftm_wcdma_calv3.h"
#endif

#ifdef FTM_HAS_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#include "rfdevice_tdscdma_pa_intf.h"
#include "rfdevice_tdscdma_papm_intf.h"
#include "rfdevice_tdscdma_asm_intf.h"
#include "rf_tdscdma_msm.h"
#include "rf_tdscdma_core_util.h"
#endif


#ifdef FEATURE_CDMA1X
#include "ftm_1x_control.h"
#include "ftm_1x_calibration.h"
#include "rfdevice_cdma_interface.h"
#include "ftm_cdma_dispatch.h"
#include "rfc_cdma.h"
#include "rfm_1x_ftm.h"
#include "rfcommon_core_device_manager.h"
#include "rfcommon_locks.h"
#include "rf_cdma_data.h"
#include "ftm_1x_xpt.h"
#include "ftm_cdma_data.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_mc.h"
#endif

#ifdef FEATURE_GSM
#include "ftm_gsm_ctl.h"
#include "rfdevice_gsm_intf.h"
#include "ftm_gsm_rfprot.h"
#endif

#ifdef FEATURE_LTE
#include "rf_lte_ftm_cmd_proc.h"
#include "rflte_ftm_external_api.h"
#include "ftm_lte_xpt.h"
#include "rflte_ftm_mc.h"
#include "rflte_nv.h"
#include "rflte_msm.h"
#include "rflte_core_util.h"
#include "rfdevice_lte_pa_intf.h"
#include "rfdevice_lte_papm_intf.h"
#include "rfdevice_lte_interface.h"
#include "rfdevice_lte_asm_intf.h"
#include "rflte_msm_iq_capture.h"
#include "ftm_lte_common_dispatch.h"
#include "rfc_card_lte.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_calv3.h"
#include "rfc_card_tdscdma.h"
#endif

#ifdef FEATURE_RF_HAS_QTUNER

#ifdef FEATURE_CDMA1X
#include "rf_cdma_utils_freq.h"
#include "rfdevice_hdet_cdma_intf.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_core_antenna_tuner.h"
#include "rfc_card_wcdma.h"
#endif

#include "rfdevice_hdet_cmn_intf.h"
#include "rfdevice_hdet_wcdma_intf.h"
#endif

#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_data.h"
#include "rf_qmi_rfrpe_svc.h"

#if defined (FEATURE_XO_FACTORY_CAL)
#include "ftm_calibration_xo.h" 
#endif

#include "rfdevice_logical_component_types.h"
#include "subs_prio.h"

#include "ftm_common_fbrx.h"
#include "ftm_common_enh_internal_device_cal.h"
#include "ftm_common_concurrency_manager.h"
#include "rflm_cmn_dbg.h"


#define FTM_RX_MIN_POWER 0xD698 /* Min power =-106*/
#define FTM_RX_RESOLUTION 0x2800 /* resolution =853.3 */
#define FTM_RX_DYNAMIC_RANGE 0x2800 /* 10 AGC counts=1dbm*/

#define FTM_TX_MIN_POWER 0xE4A7 /* Min power =-7000*/ 
#define FTM_TX_RESOLUTION 0x2800 /* 10 AGC counts = 1dBm 10240 */
#define FTM_TX_DYNAMIC_RANGE 0x2800 /* resolution = 102.4 */

#define FILE_MAX_PATH 48

typedef PACK(struct)
{
  uint16 rx_chain;
  uint8  num_freqs;
  uint8  num_steps;
  uint16 step_length;
  uint8  *data;
} ftm_tx_rx_freq_cal_sweep_param_type;

typedef PACK(struct) {
  uint16 ilpc_step_delay; /* microseconds */
  uint16  num_pairs;
  uint8 *pdm_iq;
} ftm_pseudo_ilpc_sweep_param_type;

typedef PACK(struct)
{
  uint16 chain_0_cmd;
  uint16 chain_1_cmd;
} ftm_multimode_handover_param_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  rfm_device_enum_type device;
  ftm_mode_id_type rf_mode;
  uint8 alt_path;
  uint8 num_req_paStates;
  /* This is just a place holder for PAStates. The packet size is variable.*/
  uint8 pa_states_place;
} ftm_txlut_info_for_red_cal_request_type;

typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint16  mfg_id ;
  uint16  prd_id;
  uint16  revision;
  uint8 num_valid_paStates;
/* This is just a place holder for PAStates. The packet size is variable.*/
  uint8 lut_ids_placeholder;
} ftm_txlut_info_for_red_cal_rsp_type;

typedef uint16 (*ftm_do_tx_rx_freq_cal_sweep_func_type)
(
uint16, uint8, uint8, uint16, uint16*, uint16*, int16*, uint8*, uint8*, uint8*, uint8*
);

extern ftm_rf_mode_type ftm_current_rf_mode; 

#ifdef FEATURE_WCDMA
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
extern rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band );
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
extern rflte_nv_tbl_type* rflte_mc_get_nv_tbl_ptr(uint32 carrier_idx);
#endif

#ifdef FEATURE_TDSCDMA
extern int32 rf_tdscdma_msm_get_rms_dbfs_x100_val( void );
#endif

apt_tx_lin_type ftm_apt_tx_lin[] = 
{
  {255,
    1,
    {0},
    {0}
  },
  {255,
    1,
    {0},
    {0}
  },
  {255,
    1,
    {0},
    {0}
  },
  {255,
    1,
    {0},
    {0}
  }

};

apt_freq_comp_type ftm_apt_fcomp[] =
{    
  {255,
    1,
    {0},
    {0}
  },
  {255,
    1,
    {0},
    {0}
  },
  {255,
    1,
    {0},
    {0}
  },
  {255,
    1,
    {0},
    {0}
  }

};

volatile ftm_load_tx_lin_cal_data_request * req_load_tx_lin;
volatile ftm_load_tx_fcomp_cal_data_request * req_load_tx_fcomp;
volatile byte enh_cal_action = 0;

uint32 ftm_rf_current_test_scenario = 0;

#ifdef FEATURE_CDMA1X
//extern boolean ftm_1x_write_reference_sweep_to_nv( void );
//ftm_pwr_vs_pdm_vs_freq_type ftm_pwr_pdm_freq[MAX_APT_NUM_PA_RANGES];
#endif

/*===========================================================================

FUNCTION FTM_COMMON_RF_MODE_EXIT

DESCRIPTION
   This API forces the RF driver to disable any activitIes that were started 
   since the last call to FTM_RF_SET_MODE, for a technology.
   At the end of the execution, the RF devices should be disabled

DEPENDENCIES

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_rf_mode_exit(ftm_common_pkt_type *cmd_ptr)
{
  ftm_rf_mode_exit_response_type* rf_exit_mode_rsp = NULL;
  boolean success = TRUE;
  int device_loop = 0;
  
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_rf_mode_exit_response_type));

  if(NULL == rsp.pkt_payload)
  {
    FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  rf_exit_mode_rsp = (ftm_rf_mode_exit_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(rf_exit_mode_rsp, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  for (device_loop = 0;device_loop < RFM_MAX_DEVICES; device_loop++ )
  {
    FTM_MSG_MED("FTM_RF_EXIT_MODE(). Trying to Exit device = %d", device_loop, 0, 0);
    success  &= ftm_rfmode_enter(device_loop, FTM_STATE_PARK );


  }

  if (success == TRUE)
  {
    rf_exit_mode_rsp->status = FALSE;
  }
  else
  {
    rf_exit_mode_rsp->status = TRUE;
  }

  return rsp;
}

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
ftm_rsp_pkt_type rxlm_allocate_buffer_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rxlm_allocate_buffer_params* params = (ftm_rxlm_allocate_buffer_params*)&(cmd_ptr->ftm_extended_params);
    lm_handle_type handle;
    ftm_rxlm_allocate_buffer_rsp_type *rsp_payload;

    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    rxlm_init();
    rxlm_allocate_buffer(0, params->tech, &handle);

    rsp_pkt = ftmdiag_create_new_pkt( sizeof(ftm_rxlm_allocate_buffer_rsp_type) );

    rsp_payload = rsp_pkt.pkt_payload;
    if (rsp_payload != NULL)
    {
      rsp_payload->cmd = cmd_ptr->diag_hdr.cmd_code;
      rsp_payload->sub_sys = cmd_ptr->diag_hdr.subsys_id;
      rsp_payload->mode = cmd_ptr->diag_hdr.subsys_cmd_code;
      rsp_payload->ftm_cmd = cmd_ptr->ftm_hdr.cmd_id;
      rsp_payload->req_len = cmd_ptr->ftm_hdr.cmd_data_len;
      rsp_payload->rsp_len = sizeof(ftm_rxlm_allocate_buffer_rsp_type);

      rsp_payload->handle = handle;
    }
    else
    {
      RF_MSG(RF_ERROR,"rxlm_allocate_buffer_dispatch: rsp_payload is NULL");
    }

    RF_MSG_1(RF_HIGH,"RXLM ALLOCATE BUFFER: handle = %d.", handle);

    return rsp_pkt;
}

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type rxlm_deallocate_buffer_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL };
    ftm_rxlm_deallocate_buffer_params* params = (ftm_rxlm_deallocate_buffer_params*)&(cmd_ptr->ftm_extended_params);
    rxlm_deallocate_buffer(params->handle);

    return rsp_pkt;
}

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type rxlm_update_static_settings_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL };
    ftm_rxlm_update_static_settings_params* params = (ftm_rxlm_update_static_settings_params*)&(cmd_ptr->ftm_extended_params);
    rxlm_acquire_lock(params->handle);
    rxlm_update_static_settings(params->handle, &(params->cfg));
    rxlm_release_lock(params->handle);

    return rsp_pkt;
}

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type rxlm_update_dynamic_settings_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL };

    return rsp_pkt;
}

ftm_rsp_pkt_type rflm_dtr_rx_configure_chain_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL };
    ftm_rxlm_configure_chain_params* params = (ftm_rxlm_configure_chain_params*)&(cmd_ptr->ftm_extended_params);
    
    rflm_dtr_rx_configure_chain(params->handle, (rflm_dtr_rx_indices_struct*)&(params->indices), RFLM_DTR_RX_FULL_CFG_MASK /*params->config_mask*/);

    return rsp_pkt;
}


/*----------------------------------------------------------------------------*/
lm_status_type txlm_allocate_buffer_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    return LM_BUFFER_NOT_ALLOCATED;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm_deallocate_buffer_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    return LM_BUFFER_NOT_ALLOCATED;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm_update_static_settings_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    return LM_BUFFER_NOT_ALLOCATED;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm_update_dynamic_settings_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    return LM_BUFFER_NOT_ALLOCATED;
}

ftm_rsp_pkt_type configure_rxfe_bridge_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL };
    ftm_rxlm_configure_rxfe_bridge_params* params = (ftm_rxlm_configure_rxfe_bridge_params*)&(cmd_ptr->ftm_extended_params);

    uint32 tech = params->tech;
    uint32 nb0 = params->nb0;
    uint32 nb1 = params->nb1;
    uint32 nb2 = params->nb2;
    uint32 nb3 = params->nb3;

    configure_rxfe_bridge(tech, nb0, nb1, nb2, nb3);

    enableRxfeBrdg(1, nb0, nb1, nb2, nb3);

    return rsp_pkt;
}

ftm_rsp_pkt_type stream_data_dispatch(ftm_common_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp_pkt = { FTM_RSP_DO_LEGACY, 0, NULL };
    ftm_rxlm_stream_data_params* params = (ftm_rxlm_stream_data_params*)&(cmd_ptr->ftm_extended_params);

    uint8 adc = params->adc;
    uint8 wb = params->wb;
    uint8 nb0 = params->nb0;
    uint8 nb1 = params->nb1;
    uint8 nb2 = params->nb2;
    uint8 nb3 = params->nb3;

    activate_chain(adc, wb, nb0, nb1, nb2, nb3);

    return rsp_pkt;
}
#endif

/*===========================================================================

FUNCTION FTM_COMMON_RX_WAIT_DBM_V2

DESCRIPTION
   Function waits measured RX power to reach, rise or fall, given limit 
   before timing out.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_rx_wait_dbm_v2(ftm_common_pkt_type *cmd_ptr)
{
  // Note: If the max timeout is changed here then 80-VA888-1 needs to be updated too
  const dword FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS = 30000000;
  ftm_rx_wait_dbm_v2_request_payload_type* rx_wait_req_data = NULL;
  ftm_rx_wait_dbm_v2_response_type* rx_wait_resp = NULL;
  rf_time_tick_type start_time = rf_time_get_tick();
  boolean success = TRUE; // Need to initialize to TRUE as GSM and LTE do not return status

  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_rx_wait_dbm_response_type));

  if (NULL == rsp.pkt_payload)
  {
    FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }


  rx_wait_resp = (ftm_rx_wait_dbm_v2_response_type*) rsp.pkt_payload;
  rx_wait_req_data = (ftm_rx_wait_dbm_v2_request_payload_type*)&(cmd_ptr->ftm_extended_params);

  // Copy data from request to response
  memscpy(rx_wait_resp, sizeof(ftm_composite_cmd_header_type),cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  if (FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS < rx_wait_req_data->timeout)
  {
    FTM_MSG_ERROR("Rx wait timeout capped to %lu microseconds", FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS, 0, 0);
    rx_wait_req_data->timeout = FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS;
  }

  rx_wait_resp->rx_power = 0x8000;
  do
  {
    switch (rx_wait_req_data->rf_tech)
    {
      #ifdef FEATURE_GSM
    case FTM_RF_TECH_GSM:
      rx_wait_resp->rx_power = ftm_gsm_get_rssi_dbx16( rx_wait_req_data->device_id );
      break;
    #endif /* FEATURE_GSM */

      #ifdef FEATURE_CDMA1X
    case FTM_RF_TECH_CDMA:
      {
        rfm_cdma_ftm_rx_agc_data_type rx_agc_data_dbm10;
        success = rfm_1x_ftm_get_rx_agc_data(rx_wait_req_data->device_id, &rx_agc_data_dbm10, RFM_CDMA_AGC_FORMAT__DBM10);
        rx_wait_resp->rx_power = rx_agc_data_dbm10.rx_agc[0];
      }
      break;
      #endif

      #ifdef FEATURE_WCDMA
    case FTM_RF_TECH_WCDMA:
      {
        int16 rxLevelAgc;
        success = ftm_wcdma_get_rx_level_dbm(rx_wait_req_data->device_id, &rxLevelAgc, &rx_wait_resp->rx_power);
      }
      break;
      #endif

      #ifdef FEATURE_LTE
    case FTM_RF_TECH_LTE:
      rx_wait_resp->rx_power = rflte_ftm_mc_get_rxagc_val(rx_wait_req_data->device_id);
      break;
      #endif

      #ifdef FTM_HAS_TDSCDMA
    case FTM_RF_TECH_TDSCDMA:
      {
        int16 rx_level_agc;
        int16 rx_level_dbm10;
        success = ftm_tdscdma_get_rx_level_dbm(rx_wait_req_data->device_id, &rx_level_agc, &rx_level_dbm10);
        rx_wait_resp->rx_power = rx_level_dbm10;
      }
      break;
      #endif /*FTM_HAS_TDSCDMA*/

    default:
      FTM_MSG_ERROR("Invalid technology %u specified for Rx wait", rx_wait_req_data->rf_tech, 0, 0);
      rx_wait_resp->status = FTM_RX_WAIT_DBM_UNSUPPORTED_TECHNOLOGY;
      rx_wait_resp->rx_power = 0x8000;    
      return rsp;
      break;
    }       
    if (FALSE == success)
    {
      FTM_MSG_ERROR("Reading RSSI failed for technology %u and device %u", rx_wait_req_data->rf_tech, rx_wait_req_data->device_id, 0);
      rx_wait_resp->status = FTM_RX_WAIT_DBM_RSSI_FAILED;
      rx_wait_resp->rx_power = 0x8000;    
      return rsp;
    }
    if (SIGNAL_RISING == rx_wait_req_data->signal_direction)
    {
      if (rx_wait_resp->rx_power >= rx_wait_req_data->rx_power_threshold)
      {
        FTM_MSG_MED("Rx wait success, rising trigger. RSSI: Expected = %d, Measured = %d", rx_wait_req_data->rx_power_threshold, rx_wait_resp->rx_power, 0);
        rx_wait_resp->status = FTM_RX_WAIT_DBM_SUCCESS;
        return rsp;
      }
    }
    else
    {
      if (rx_wait_resp->rx_power <= rx_wait_req_data->rx_power_threshold)
      {
        FTM_MSG_MED("Rx wait success, falling trigger. RSSI: Expected = %d, Measured = %d", rx_wait_req_data->rx_power_threshold, rx_wait_resp->rx_power, 0);
        rx_wait_resp->status = FTM_RX_WAIT_DBM_SUCCESS;
        return rsp;
      }
    }
    if ( rex_get_sigs(rex_self()) & FTM_RPT_TIMER_SIG )
    {
      ftm_kick_watchdog();
    }
  } while ( rf_time_get_elapsed(start_time, RF_USEC) < rx_wait_req_data->timeout );

  FTM_MSG_ERROR("Rx wait timed out. RSSI: Expected = %d, Measured = %d, Direction = %d", rx_wait_req_data->rx_power_threshold, rx_wait_resp->rx_power, rx_wait_req_data->signal_direction);
  rx_wait_resp->status = FTM_RX_WAIT_DBM_TIMEOUT;
  return rsp;
}

/*===========================================================================

FUNCTION FTM_GET_PA_SWPTS

DESCRIPTION
   Function returns the PA switchpoints info of a given tech and band

DEPENDENCIES
   UE should be in FTM mode

RETURN VALUE
   PA switchpoints of a given tech and band

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_common_get_pa_swpts(ftm_common_pkt_type *cmd_ptr, ftm_rsp_pkt_type* rsp)
{
  ftm_get_pa_swpt_request_pkt_type* get_static_info_req_data = NULL;
  ftm_get_pa_swpt_response_pkt_type* get_pa_swpt_resp = NULL;
  boolean success = FALSE; 
  int16 pa_rise_swpt[RFCOMMON_NV_MAX_PA_STATE];
  int16 pa_fall_swpt[RFCOMMON_NV_MAX_PA_STATE];
  int16 active_pa_states =  0xFFFF;
  
  memset(&pa_rise_swpt,0,(sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE));
  memset(&pa_fall_swpt,0,(sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE));

  get_pa_swpt_resp = (ftm_get_pa_swpt_response_pkt_type*)(rsp->pkt_payload);
  get_static_info_req_data = (ftm_get_pa_swpt_request_pkt_type*)&(cmd_ptr->ftm_extended_params);

  // Copy data from request to response
  memscpy(get_pa_swpt_resp, sizeof(ftm_composite_cmd_header_type),cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  switch(get_static_info_req_data->rf_tech)
  {
    #ifdef FEATURE_CDMA1X
    case FTM_RF_TECH_CDMA:
      get_pa_swpt_resp->status = (uint16)ftm_cdma_get_pa_switchpoints((rfm_device_enum_type)get_static_info_req_data->tx_device, 
                                                                             (rfm_cdma_band_class_type)get_static_info_req_data->band, 
                                                                             &pa_rise_swpt[0], 
                                                                             &pa_fall_swpt[0],
                                                                             &active_pa_states);
      break;
    #endif

    #ifdef FEATURE_WCDMA
    case FTM_RF_TECH_WCDMA:
      get_pa_swpt_resp->status = (uint16)ftm_wcdma_get_pa_switchpoints((rfm_device_enum_type)get_static_info_req_data->tx_device, 
                                                                       (rfcom_wcdma_band_type)get_static_info_req_data->band, 
                                                                       &pa_rise_swpt[0], 
                                                                       &pa_fall_swpt[0],
                                                                       &active_pa_states);
      break;
    #endif

    #ifdef FEATURE_LTE
    case FTM_RF_TECH_LTE:
      get_pa_swpt_resp->status = (uint16)ftm_rflte_get_pa_switchpoints((uint8)get_static_info_req_data->tx_device, 
                                                                             (rfcom_lte_band_type)get_static_info_req_data->band, 
                                                                             &pa_rise_swpt[0], 
                                                                             &pa_fall_swpt[0],
                                                                             &active_pa_states);
      break;
    #endif

    #ifdef FTM_HAS_TDSCDMA
    case FTM_RF_TECH_TDSCDMA:
      get_pa_swpt_resp->status = (uint16)ftm_tdscdma_get_pa_switchpoints((rfm_device_enum_type)get_static_info_req_data->tx_device, 
                                                                             (rfcom_tdscdma_band_type)get_static_info_req_data->band, 
                                                                             &pa_rise_swpt[0], 
                                                                             &pa_fall_swpt[0],
                                                                             &active_pa_states);
      break;
    #endif

    default:
      FTM_MSG_ERROR("Invalid technology %u specified for Get PA Switchpionts", get_static_info_req_data->rf_tech, 0, 0);
      get_pa_swpt_resp->status = FALSE;
      break;
  }

  /* Populate tech specific return params to response packet */
  memscpy((void*)(&(get_pa_swpt_resp->pa_rise_swpt[0])),
          sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE,
          &pa_rise_swpt[0],
          sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE);

  memscpy((void*)(&(get_pa_swpt_resp->pa_fall_swpt[0])),
          sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE,
          &pa_fall_swpt[0],
          sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE);
  get_pa_swpt_resp->active_pa_states = active_pa_states;
  get_pa_swpt_resp->rf_tech = get_static_info_req_data->rf_tech;
  get_pa_swpt_resp->action = get_static_info_req_data->action;
  get_pa_swpt_resp->tx_device = get_static_info_req_data->tx_device;
  get_pa_swpt_resp->band = get_static_info_req_data->band;

}

/*===========================================================================

FUNCTION FTM_GET_STATIC_INFO

DESCRIPTION
   Function returns the requested static NV info of a given tech and band

DEPENDENCIES
   UE should be in FTM mode

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_get_static_info(ftm_common_pkt_type *cmd_ptr)
{

  ftm_get_static_info_request_pkt_type* get_static_info_req_data = NULL;
  ftm_rsp_pkt_type rsp = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  get_static_info_req_data = (ftm_get_static_info_request_pkt_type*)&(cmd_ptr->ftm_extended_params);

  switch ((ftm_get_static_info_action_type)get_static_info_req_data->action)
  {
    case GET_PA_SWPT:
      rsp = ftmdiag_create_new_pkt(sizeof(ftm_get_pa_swpt_response_pkt_type));
      if (NULL == rsp.pkt_payload)
      {
        FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
        return rsp;
      }
      ftm_common_get_pa_swpts(cmd_ptr, &rsp);
      break;

    default:
      FTM_MSG_ERROR("ftm_common_get_static_info: Bad action type: %d", get_static_info_req_data->action, 0, 0);
      break;
  }
  
  return rsp;
}

/*===========================================================================

FUNCTION FTM_COMMON_RX_WAIT_DBM

DESCRIPTION
   Function waits measured RX power to reach given limit before timing out.

DEPENDENCIES
   This command is only valid when the Rx verification state has been enabled
   for a specified receiver using FTM_SET_CALIBRATION_STATE command.
   In case of GSM the LNA must have been set by calling
   FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR calling this command.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_rx_wait_dbm(ftm_common_pkt_type *cmd_ptr)
{
  // Note: If the max timeout is changed here then 80-VA888-1 needs to be updated too
  const dword FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS = 5000000;
  ftm_rx_wait_dbm_request_payload_type* rx_wait_req_data = NULL;
  ftm_rx_wait_dbm_response_type* rx_wait_resp = NULL;
  rf_time_tick_type start_time = rf_time_get_tick();
  boolean success = TRUE; // Need to initialize to TRUE as GSM and LTE do not return status

  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_rx_wait_dbm_response_type));

  if (NULL == rsp.pkt_payload)
  {
    FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }


  rx_wait_resp = (ftm_rx_wait_dbm_response_type*) rsp.pkt_payload;
  rx_wait_req_data = (ftm_rx_wait_dbm_request_payload_type*)&(cmd_ptr->ftm_extended_params);

  // Copy data from request to response
  memscpy(rx_wait_resp, sizeof(ftm_composite_cmd_header_type), cmd_ptr, sizeof(ftm_composite_cmd_header_type));
  rx_wait_resp->rf_tech = rx_wait_req_data->rf_tech;

  if (FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS < rx_wait_req_data->timeout)
  {
    FTM_MSG_ERROR("Rx wait timeout capped to %lu microseconds", FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS, 0, 0);
    rx_wait_req_data->timeout = FTM_RX_WAIT_DBM_MAX_TIMEOUT_MICROSECONDS;
  }

  rx_wait_resp->rx_power = 0x8000;
  do
  {
    switch (rx_wait_req_data->rf_tech)
    {
      #ifdef FEATURE_GSM
    case FTM_GSM_C0_C:
      rx_wait_resp->rx_power = ftm_gsm_get_rssi_dbx16( RFM_DEVICE_0 );
      break;
  
    case FTM_GSM_C1_C:
      rx_wait_resp->rx_power = ftm_gsm_get_rssi_dbx16( RFM_DEVICE_1 );
      break;

    case FTM_GSM_C2_C:
      rx_wait_resp->rx_power = ftm_gsm_get_rssi_dbx16( RFM_DEVICE_2 );
      break;

    case FTM_GSM_C3_C:
      rx_wait_resp->rx_power = ftm_gsm_get_rssi_dbx16( RFM_DEVICE_3 );
      break;

    #endif /* FEATURE_GSM */

      #ifdef FEATURE_CDMA1X
    case FTM_1X_C0_C:
      {
        rfm_cdma_ftm_rx_agc_data_type rx_agc_data_dbm10;
        success = rfm_1x_ftm_get_rx_agc_data(RFM_DEVICE_0, &rx_agc_data_dbm10, RFM_CDMA_AGC_FORMAT__DBM10);
        rx_wait_resp->rx_power = rx_agc_data_dbm10.rx_agc[0];
      }
      break;

    case FTM_1X_C1_C:
      {
        rfm_cdma_ftm_rx_agc_data_type rx_agc_data_dbm10;
        success = rfm_1x_ftm_get_rx_agc_data( RFM_DEVICE_1, &rx_agc_data_dbm10, RFM_CDMA_AGC_FORMAT__DBM10);
        rx_wait_resp->rx_power = rx_agc_data_dbm10.rx_agc[0];
      }
      break;

    case FTM_1X_C2_C:
      {
        rfm_cdma_ftm_rx_agc_data_type rx_agc_data_dbm10;
        success = rfm_1x_ftm_get_rx_agc_data( RFM_DEVICE_2, &rx_agc_data_dbm10, RFM_CDMA_AGC_FORMAT__DBM10);
        rx_wait_resp->rx_power = rx_agc_data_dbm10.rx_agc[0];
      }
      break;

    case FTM_1X_C3_C:
      {
        rfm_cdma_ftm_rx_agc_data_type rx_agc_data_dbm10;
        success = rfm_1x_ftm_get_rx_agc_data( RFM_DEVICE_3, &rx_agc_data_dbm10, RFM_CDMA_AGC_FORMAT__DBM10);
        rx_wait_resp->rx_power = rx_agc_data_dbm10.rx_agc[0];
      }
      break;
      #endif

      #ifdef FEATURE_WCDMA
    case FTM_WCDMA_C:
      {
        int16 rxLevelAgc;
        success = ftm_wcdma_get_rx_level_dbm(RFCOM_TRANSCEIVER_0, &rxLevelAgc, &rx_wait_resp->rx_power);
      }
      break;
      #endif

      #ifdef FEATURE_WCDMA_RX_DIVERSITY
    case FTM_WCDMA_RX_2_C:
      {
        int16 rxLevelAgc;
        success = ftm_wcdma_get_rx_level_dbm(RFCOM_RECEIVER_DIV, &rxLevelAgc, &rx_wait_resp->rx_power);
      }
      break;
      #endif 

      #ifdef FEATURE_LTE
    case FTM_LTE_C:
      rx_wait_resp->rx_power = rflte_ftm_mc_get_rxagc_val((rfm_device_enum_type)RF_PATH_0);
      break;

    case FTM_LTE_RX_2_C:
      rx_wait_resp->rx_power = rflte_ftm_mc_get_rxagc_val((rfm_device_enum_type)RF_PATH_1);
      break;
      #endif

      #ifdef FTM_HAS_TDSCDMA
    case FTM_TDSCDMA_C2_C:
      {
        int16 rx_level_agc;
        int16 rx_level_dbm10;
        success = ftm_tdscdma_get_rx_level_dbm(  RFM_DEVICE_2, &rx_level_agc, &rx_level_dbm10);
        rx_wait_resp->rx_power = rx_level_dbm10;
      }
      break;
	  
	      case FTM_TDSCDMA_C:
      {
        int16 rx_level_agc;
        int16 rx_level_dbm10;
        success = ftm_tdscdma_get_rx_level_dbm(  RFCOM_TRANSCEIVER_0, &rx_level_agc, &rx_level_dbm10);
        rx_wait_resp->rx_power = rx_level_dbm10;
      }
      break;

      #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
    case FTM_TDSCDMA_RX_2_C:
      {
        int16 rx_level_agc;
        int16 rx_level_dbm10;
        success = ftm_tdscdma_get_rx_level_dbm( RFCOM_RECEIVER_DIV, &rx_level_agc, &rx_level_dbm10);
        rx_wait_resp->rx_power = rx_level_dbm10;
      }
      break;
	  
    case FTM_TDSCDMA_C3_C:
      {
        int16 rx_level_agc;
        int16 rx_level_dbm10;
        success = ftm_tdscdma_get_rx_level_dbm(  RFM_DEVICE_3, &rx_level_agc, &rx_level_dbm10);
        rx_wait_resp->rx_power = rx_level_dbm10;
      }
      break;
      #endif    /* FTM_HAS_TDSCDMA_RX_DIVERSITY */
      #endif /*FTM_HAS_TDSCDMA*/

    default:
      FTM_MSG_ERROR("Invalid technology %u specified for Rx wait", rx_wait_req_data->rf_tech, 0, 0);
      rx_wait_resp->status = FTM_RX_WAIT_DBM_UNSUPPORTED_TECHNOLOGY;
      rx_wait_resp->rx_power = 0x8000;    
      return rsp;
      break;
    }       
    if (FALSE == success)
    {
      FTM_MSG_ERROR("Reading RSSI failed for technology %u", rx_wait_req_data->rf_tech, 0, 0);
      rx_wait_resp->status = FTM_RX_WAIT_DBM_RSSI_FAILED;
      rx_wait_resp->rx_power = 0x8000;    
      return rsp;
    }

    if (rx_wait_resp->rx_power >= rx_wait_req_data->rx_power_threshold)
    {
      FTM_MSG_MED("Rx wait success. RSSI: Expected = %d, Measured = %d", rx_wait_req_data->rx_power_threshold, rx_wait_resp->rx_power, 0);
      rx_wait_resp->status = FTM_RX_WAIT_DBM_SUCCESS;
      return rsp;
    }

    if ( rex_get_sigs(rex_self()) & FTM_RPT_TIMER_SIG )
    {
      ftm_kick_watchdog();
    }
  } while ( rf_time_get_elapsed(start_time, RF_USEC) < rx_wait_req_data->timeout );

  FTM_MSG_ERROR("Rx wait timed out. RSSI: Expected = %d, Measured = %d", rx_wait_req_data->rx_power_threshold, rx_wait_resp->rx_power, 0);
  rx_wait_resp->status = FTM_RX_WAIT_DBM_TIMEOUT;
  return rsp;
}

static void ftm_common_set_ept_state( boolean set_sapt_on, ftm_rf_technology_type ftm_rf_tech )
{
  switch ( ftm_rf_tech )
  {

  default:
    FTM_MSG_1( FTM_ERROR, "Set EPT state not supported for mode %d",
               ftm_rf_tech );
    break;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Overrides a bunch of TXAGC parameters. 

  @details
  This function will call the tech APIs corresponding to override the parameters.

  @param *ovr_params Override Params

  @return

*/
static
boolean
ftm_common_xpt_txagc_override
(
  ftm_xpt_txagc_override_type *ovr_params,
  ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;

  if ( ovr_params == NULL )
  {
    FTM_MSG( FTM_ERROR, "Input param for ftm_common_xpt_txagc_override is NULL");
    return FALSE;
  }

  /* Perform XPT TxAGC override based on the current active mode */
  switch( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
      #ifdef FEATURE_CDMA1X
      api_status &= ftm_1x_set_xpt_txagc_override( ovr_params );
      #endif
      break;
  case FTM_RF_TECH_WCDMA:
      #ifdef FEATURE_WCDMA
      api_status &= ftm_wcdma_xpt_override_rf_settings( ovr_params );
      #endif
      break;
  case FTM_RF_TECH_LTE:
      #ifdef FEATURE_LTE
      api_status &= rflte_ftm_xpt_override_rf_settings(ovr_params);
      #endif
      break;
  case FTM_RF_TECH_TDSCDMA:
      #ifdef FEATURE_TDSCDMA
      api_status &= ftm_tdscdma_xpt_override_rf_settings(ovr_params);
      #endif
      break;
  default:
      break;
  }

  if ( api_status == FALSE)
  {
    FTM_MSG( FTM_ERROR, "ftm_common_xpt_txagc_override: api status = FAIL" );
  }

  return api_status;
}/* ftm_common_xpt_txagc_override */


#ifndef FEATURE_GNSS_ONLY_NO_WWAN
static 
ftm_rsp_pkt_type
ftm_common_load_dpd_tables
(
   void* ftm_req_data,
   ftm_rf_technology_type ftm_rf_tech 
)
{
  boolean api_status = TRUE;
  /* Request payload type */

  ftm_dpd_override_type *xpt_dpd_ovr;
  ftm_xpt_txagc_override_type ovr_params;
  rfcommon_core_xpt_tx_override_type curr_tx_ovr;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_dpd_override_rsp_pkt_type *rsp_pkt; /*Packet Payload*/

  /* Dynamically allocated AMAM/AMPM/EPDT Table pointers */
  uint16 *amam_tbl_ptr = NULL;
  uint16 *ampm_tbl_ptr = NULL;
  uint16 *epdt_tbl_ptr = NULL;
  /* Track current location during packet parsing */
  uint16 *curr_pkt_ptr = NULL;
  rfm_device_enum_type device;
  rfcommon_mdsp_xpt_mode_enum_type xpt_type;
  xpt_dpd_action_type action_type;
  uint16 num_amam;
  uint16 num_ampm;
  uint16 num_epdt;
  uint8   amam_bank_index = -1;
  uint16  amam_bank_offset = -1;
  uint8   ampm_bank_index = -1;
  uint16  ampm_bank_offset = -1;
  uint8   epdt_bank_index = -1;
  uint16  epdt_bank_offset = -1;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( 
                              ftm_req_data,
                              sizeof(ftm_dpd_override_rsp_pkt_type));

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_dpd_override_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_load_dpd_tables: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }

  /* Extract parameters */
  xpt_dpd_ovr = (ftm_dpd_override_type *)
                   (&(((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params));

  device = (rfm_device_enum_type)xpt_dpd_ovr->device;

  if ( xpt_dpd_ovr->xpt_type == 0 )
  {
    xpt_type = RFCOMMON_MDSP_XPT_MODE_EPT;
  }
  else if ( xpt_dpd_ovr->xpt_type == 1 )
  {
    xpt_type = RFCOMMON_MDSP_XPT_MODE_ET;
  }
  else
  {
    xpt_type = 0xFF;
  }

  action_type = (xpt_dpd_action_type)xpt_dpd_ovr->action_type;
  num_amam = xpt_dpd_ovr->num_amam;
  num_ampm = xpt_dpd_ovr->num_ampm;
  num_epdt = xpt_dpd_ovr->num_epdt;

  if (xpt_type > RFCOMMON_MDSP_XPT_MODE_ET)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_load_dpd_tables: "
               "XPT type (%d) > 2 not supported.",
               xpt_type );
    return ftm_rsp_data;
  }

  if (action_type >= XPT_DPD_ACTION_NUM)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_load_dpd_tables: "
               "XPT action type (%d) is invalid.",
               action_type );
    return ftm_rsp_data;
  }

  if(num_amam != num_ampm)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_common_load_dpd_tables: "
               "num_amam (%d) and num_ampm(%d) must be the same.",
               num_amam, num_ampm );
    return ftm_rsp_data;
  }

  if((num_epdt == 0) && (xpt_type == RFCOMMON_MDSP_XPT_MODE_ET))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_load_dpd_tables: "
             "num_epdt(%d) of zero not allowed when xpt type is ET." );
    return ftm_rsp_data;
  }


  FTM_MSG_3( FTM_HIGH, "ftm_common_load_dpd_tables: "
             "num_amam (%d), num_ampm(%d), num_epdt (%d).",
             num_amam, num_ampm, num_epdt );

  /* Initialize the current packet pointer to point to the byte value beyond 
  num_epdt, which is the last fixed header value in the packet */
  curr_pkt_ptr = (uint16*)(&(xpt_dpd_ovr->dpd_payload[0]));

  /* Copy over AMAM data from the FTM packet into heap allocated 
  data structures */
  if (( num_amam > 0 ) && (api_status == TRUE))
  {
    amam_tbl_ptr = modem_mem_alloc( sizeof(uint16)*num_amam,
                                    MODEM_MEM_CLIENT_RFA );
    if (amam_tbl_ptr == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_load_dpd_tables: "
                 "AMAM mem alloc failed for %d bytes.",
                 sizeof(uint16)*num_amam );
      api_status = FALSE;
    }
    else
    {
      memscpy( &amam_tbl_ptr[0], sizeof(uint16)*num_amam,
               curr_pkt_ptr, sizeof(uint16)*num_amam);
      curr_pkt_ptr += num_amam;
    }
  }

  /* Copy over AMPM data from the FTM packet into heap allocated 
  data structures */
  if (( num_ampm > 0 ) && (api_status == TRUE))
  {
    ampm_tbl_ptr = modem_mem_alloc( sizeof(uint16)*num_ampm,
                                    MODEM_MEM_CLIENT_RFA );
    if (ampm_tbl_ptr == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_load_dpd_tables: "
                 "AMPM mem alloc failed for %d bytes.",
                 sizeof(uint16)*num_ampm );
      api_status = FALSE;
    }
    else
    {
      memscpy( &ampm_tbl_ptr[0], sizeof(uint16)*num_ampm, curr_pkt_ptr, sizeof(uint16)*num_ampm);
      curr_pkt_ptr += num_ampm;
    }
  }

  /* Copy over EPDT data from the FTM packet into heap allocated 
  data structures */
  if (( num_epdt > 0 ) && (api_status == TRUE))
  {
    epdt_tbl_ptr = modem_mem_alloc( sizeof(uint16)*num_epdt,
                                    MODEM_MEM_CLIENT_RFA );
    if (epdt_tbl_ptr == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_load_dpd_tables: "
                 "EPDT mem alloc failed for %d bytes.",
                 sizeof(uint16)*num_epdt );
      api_status = FALSE;
    }
    else
    {
      memscpy( &epdt_tbl_ptr[0], sizeof(uint16)*num_epdt, curr_pkt_ptr, sizeof(uint16)*num_epdt);
      curr_pkt_ptr += num_epdt;
    }
  }

  /* Program QPOET into APT mode */
  /* Get the current Tx override */
  memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
  api_status &= ftm_common_xpt_get_curr_tx_override( device, 
                                                     &curr_tx_ovr );
  ovr_params.device = device;
  ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
  ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
  ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID; 
  ovr_params.rgi = curr_tx_ovr.rgi;
  ovr_params.smps_bias = 2000;
  ovr_params.pa_state = curr_tx_ovr.pa_state;
  ovr_params.pa_curr = curr_tx_ovr.pa_curr;
  ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
  
  api_status = ftm_common_xpt_txagc_override( &ovr_params, ftm_rf_tech );

  if(api_status == TRUE)
  {
    /* Call RF Common Core XPT Common Load DPD Function 
    with action type also as parameter with the tables */
    if ((action_type == XPT_DPD_ACTION_ISC) || 
        (action_type == XPT_DPD_ACTION_INIT))
    {
      /* Clear dpd tables of any previous data */
      api_status &= 
        rf_common_xpt_load_dpd_tbls(device, 
                                    NULL, 
                                    0, 
                                    0, 
                                    &amam_bank_index, 
                                    &amam_bank_offset, 
                                    XPT_DPD_ACTION_INIT,
                                    xpt_type);
    }
  }

  amam_bank_index = -1;
  amam_bank_offset = -1;
  if((api_status == TRUE) && (num_amam > 0))
  {
    /* Write AMAM to table */
    api_status &=
        rf_common_xpt_load_dpd_tbls(device, 
                                    amam_tbl_ptr, 
                                    num_amam,
                                    DPD_TYPE_AMAM,
                                    &amam_bank_index,
                                    &amam_bank_offset, 
                                    XPT_DPD_ACTION_SAVE,
                                    xpt_type);
  }
  rsp_pkt->amam_bank_idx = amam_bank_index;
  rsp_pkt->amam_bank_offset = amam_bank_offset;

  ampm_bank_index = -1;
  ampm_bank_offset = -1;
  if((api_status == TRUE) && (num_ampm > 0) && (ampm_tbl_ptr != NULL))
  {
    /* Write AMPM to table */
    api_status &=
            rf_common_xpt_load_dpd_tbls(device, 
                                        ampm_tbl_ptr,
                                        num_ampm,
                                        DPD_TYPE_AMPM,
                                        &ampm_bank_index,
                                        &ampm_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        xpt_type);
  }
  rsp_pkt->ampm_bank_idx = ampm_bank_index;
  rsp_pkt->ampm_bank_offset = ampm_bank_offset;

  epdt_bank_index = -1;
  epdt_bank_offset = -1;
  if((api_status == TRUE) && (num_epdt > 0) && (xpt_type == RFCOMMON_MDSP_XPT_MODE_ET) && (epdt_tbl_ptr != NULL))
  {
    /* Write EPDT to table if not EPT */
    api_status &=
            rf_common_xpt_load_dpd_tbls(device, 
                                        epdt_tbl_ptr,
                                        num_epdt,
                                        DPD_TYPE_EPDT,
                                        &epdt_bank_index,
                                        &epdt_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        xpt_type);
  }
  rsp_pkt->epdt_bank_idx = epdt_bank_index;
  rsp_pkt->epdt_bank_offset = epdt_bank_offset;

  if(api_status == TRUE)
  {
    if ((action_type == XPT_DPD_ACTION_ISC) || 
        (action_type == XPT_DPD_ACTION_COMMIT))
    {
      /* Commit DPD tables to HW */
      api_status &= 
        rf_common_xpt_load_dpd_tbls(device, 
                                    NULL, 
                                    0, 
                                    0, 
                                    &amam_bank_index, 
                                    &amam_bank_offset, 
                                    XPT_DPD_ACTION_COMMIT,
                                    xpt_type);

      /* Get the current Tx override and update only the DPD information */
      memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
      api_status &= ftm_common_xpt_get_curr_tx_override( device, 
                                                         &curr_tx_ovr );
      curr_tx_ovr.amam_addr.mem_bank_id = amam_bank_index;
      curr_tx_ovr.amam_addr.start_offset = amam_bank_offset;
      curr_tx_ovr.amam_addr.tbl_size = num_amam;
      curr_tx_ovr.ampm_addr.mem_bank_id = ampm_bank_index;
      curr_tx_ovr.ampm_addr.start_offset = ampm_bank_offset;
      curr_tx_ovr.ampm_addr.tbl_size = num_ampm;
      curr_tx_ovr.detrough_addr.mem_bank_id = epdt_bank_index;
      curr_tx_ovr.detrough_addr.start_offset = epdt_bank_offset;
      curr_tx_ovr.detrough_addr.tbl_size = num_epdt;

      /* Update the current set of overrides to FTM common data */
      api_status &= ftm_common_xpt_set_curr_tx_override( device, 
                                                         &curr_tx_ovr );
    }
  }

  if ( amam_tbl_ptr != NULL )
  {
    modem_mem_free( amam_tbl_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if ( ampm_tbl_ptr != NULL )
  {
    modem_mem_free( ampm_tbl_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if ( epdt_tbl_ptr != NULL )
  {
    modem_mem_free( epdt_tbl_ptr, MODEM_MEM_CLIENT_RFA);
  }

  if (rsp_pkt != NULL)
  {
    rsp_pkt->status = api_status;
  }

  if ( api_status == FALSE)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_load_dpd_tables: api status = FAIL" );
  }

  return ftm_rsp_data;

} /* ftm_common_load_dpd_tables */

static
ftm_rsp_pkt_type
ftm_common_xpt_mline_compute
(
   void* ftm_req_data,
   ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;
  uint8* curr_pkt_ptr = NULL; /* tracks the current location in the packet */
  uint16 num_bytes;
  uint8 cal_idx = 0;
  uint8 rgi_idx = 0;
  uint16 payload_len = 0;
  uint16 i = 0;
  char* pch;
  char* temp_str;
  char strFile[FILE_MAX_PATH+1];
  uint8 debug_msg = 0;

  /* Define Response Packet */
  ftm_rsp_pkt_type ftm_rsp_data; /* packet returned to diag*/
  ftm_xpt_mline_compute_rsp_pkt_type *rsp_pkt; /* response packet payload */

  /* declare local storage for caching parameters */
  uint8 xpt_type; /* indicates the xpt type */
  uint8 num_xpt_cal = 0; /* number of alignment array elements */
  uint8 num_rgi_delta = 0; /* number of RGI delta array elements */
  uint8 dpd_file_name_len = 0; /* length of dpd file name */
  char *dpd_file_name_ptr = NULL;
  ftm_common_xpt_cal_int_data *pc_xpt_cal_ptr = NULL; /* ptr start of xpt cal pc data */
  ftm_common_rgi_delta_int_data *pc_rgi_delta_ptr = NULL; /* ptr start of rgi delta sweep pc data */
  ftm_common_xpt_mline_params mline_params;
  ftm_common_xpt_cal_data *xpt_cal_data_ptr = NULL;
  ftm_common_rgi_delta_data *rgi_delta_data_ptr = NULL;
  ftm_common_xpt_mline_outputs *mline_outputs_ptr = NULL;
  ftm_common_xpt_mline_outputs_int_data *pc_mline_outputs_ptr = NULL;
  ftm_calibration_mline_result_xpt_type *mline_results = NULL;
  ftm_common_xpt_dpd_file dpd_data;
  uint32 current_carrier_idx = 0;

  /* Fetch NV params */
  #ifdef FEATURE_WCDMA
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  #endif
  #ifdef FEATURE_LTE
  rflte_ftm_mc_sm_type *ftm_mc_sm_ptr = NULL;
  rflte_nv_supported_band_type rfnv_band = RFLTE_NV_INVALID_BAND;
  rflte_nv_tbl_type* rflte_nv_tbl_ptr = NULL;
  #endif
  #ifdef FEATURE_TDSCDMA
  rfnv_tdscdma_tx_tbl_type* tds_nv_tbl_ptr = NULL;
  #endif

  /* Extract Parameters from request packet*/
  ftm_xpt_mline_compute_type *pkt_params =
     (ftm_xpt_mline_compute_type*)((byte*)(ftm_req_data) + 10); //(((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params); //

  /* extract fixed length parameters*/
  mline_params.lin_reg_percent = (float)(pkt_params->lin_region_pct_x100) / 100.0;
  mline_params.compression_db = (float)(pkt_params->comp_pt_x10) / 10.0;
  mline_params.curve_fit_type = (ftm_common_mline_curve_fit_type)pkt_params->curve_fit_type;
  num_xpt_cal = pkt_params->num_xpt_cal;
  num_rgi_delta = pkt_params->num_rgi_delta;
  dpd_file_name_len = pkt_params->dpd_file_name_len;
  xpt_type = pkt_params->xpt_type;

  if ( debug_msg > 0 )
  {
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: lin_reg_percent = %1.2f", mline_params.lin_reg_percent);
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: compression_db = %1.2f", mline_params.compression_db);
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: curve_fit_type = %d", mline_params.curve_fit_type); 
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: num_xpt_cal = %d", num_xpt_cal);
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: num_rgi_delta = %d", num_rgi_delta);
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: dpd_file_name_len = %d", dpd_file_name_len);
    FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: xpt_type = %d", xpt_type);
  }

  /* determine response payload length */
  payload_len = num_rgi_delta * sizeof(ftm_common_xpt_mline_outputs_int_data);

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt(ftm_req_data,
                                           sizeof(ftm_xpt_mline_compute_rsp_pkt_type) + payload_len);

  /* set rsp_pkt to point to start of the Packet Payload in ftm_rsp_data */
  rsp_pkt = (ftm_xpt_mline_compute_rsp_pkt_type *)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if (rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG(FTM_ERROR, "ftm_common_xpt_mline_compute: "
            "Response packet is NULL");
    return ftm_rsp_data;
  }

  /* Validate Parameters */
  if (num_xpt_cal < 1)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
              "Number of Alignment sweep values (%d) < 1 not supported.",
              num_xpt_cal);
    return ftm_rsp_data;
  }
  if (num_rgi_delta < 1)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
              "Number of RGI Delta sweep values (%d) < 1 not supported.",
              num_rgi_delta);
    return ftm_rsp_data;
  }
  if (pkt_params->curve_fit_type >= MLINE_CURVE_FIT_MAX_NUM)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
              "Invalid curve_fit_type (%d): See enum ftm_common_mline_curve_fit_type",
              pkt_params->curve_fit_type);
    return ftm_rsp_data;
  }
  /* File names must be at least 4 chars: 1_char_for_name + '.' + 1_char_for_ext + '\0' */
  if ((dpd_file_name_len > 0) && (dpd_file_name_len < 4))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
              "DPD file name len %d is < the minimum expected 4 chars including NULL terminator.", dpd_file_name_len);
    return ftm_rsp_data;
  }

  /* Extract parameters from the variable length payload */

  /* set cur_pkt_ptr to first byte in the m-line payload */
  curr_pkt_ptr = pkt_params->payload;

  /* copy xpt cal sweep data from FTM packet to heap allocated data structures */
  if (api_status == TRUE)
  {
    num_bytes = sizeof(ftm_common_xpt_cal_int_data) * num_xpt_cal;
    pc_xpt_cal_ptr = modem_mem_alloc(num_bytes,MODEM_MEM_CLIENT_RFA);

    if (pc_xpt_cal_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "ftm_common_xpt_cal_int_data memory alloc failed for %d bytes.",
                num_bytes);
      api_status = FALSE;
    }
    else
    {
      memscpy(pc_xpt_cal_ptr, num_bytes, curr_pkt_ptr, num_bytes);
      curr_pkt_ptr += num_bytes;
    }
  }

  /* copy rgi delta sweep data from FTM packet to heap allocated data structures */
  if (api_status == TRUE)
  {
    num_bytes = sizeof(ftm_common_rgi_delta_int_data) * num_rgi_delta;
    pc_rgi_delta_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);

    if (pc_rgi_delta_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "RGI delta memory alloc failed for %d bytes.",
                num_bytes);
      api_status = FALSE;
    }
    else
    {
      memscpy(pc_rgi_delta_ptr, num_bytes,curr_pkt_ptr, num_bytes);
      curr_pkt_ptr += num_bytes;
    }
  }

  /* copy dpd file name from FTM packet to heap allocated data structures */
  if (api_status == TRUE)
  {
    dpd_file_name_ptr = modem_mem_alloc(dpd_file_name_len, MODEM_MEM_CLIENT_RFA);

    if (dpd_file_name_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "DPD File Name memory alloc failed for %d bytes.",
                dpd_file_name_len);
      api_status = FALSE;
    }
    else
    {
      memscpy(dpd_file_name_ptr, dpd_file_name_len,curr_pkt_ptr, dpd_file_name_len);
      if (dpd_file_name_ptr[dpd_file_name_len-1] != '\0')
      {
        FTM_MSG_1( FTM_ERROR, "ftm_common_xpt_mline_compute: Provided DPD file name (%s) is not NULL terminated.", dpd_file_name_ptr );
        api_status = FALSE;
      }
      else 
      {
      curr_pkt_ptr += dpd_file_name_len;
    }

    }
  }

  /* allocate memory for xpt cal data structure */
  if (api_status == TRUE)
  {
    num_bytes = sizeof(ftm_common_xpt_cal_data) * num_xpt_cal;
    xpt_cal_data_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);

    if (xpt_cal_data_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "xpt_cal_data_ptr memory alloc failed for %d bytes.",
                num_bytes);
      api_status = FALSE;
    }
  }

  /* allocate memory for rgi delta cal data structure */
  if (api_status == TRUE)
  {
    num_bytes = sizeof(ftm_common_rgi_delta_data) * num_rgi_delta;
    rgi_delta_data_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);

    if (rgi_delta_data_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "rgi_delta_data_ptr memory alloc failed for %d bytes.",
                num_bytes);
      api_status = FALSE;
    }
  }

  /* convert to ftm_common_xpt_mline_calc() structures */
  if (api_status == TRUE)
  {
    /* this conversion is required because the packet data structs cannot support floats */
    for (cal_idx = 0; cal_idx < num_xpt_cal; cal_idx++)
    {
      xpt_cal_data_ptr[cal_idx].xpt_cal_inputs.bias = pc_xpt_cal_ptr[cal_idx].xpt_cal_inputs.bias;
      xpt_cal_data_ptr[cal_idx].xpt_cal_inputs.rgi = pc_xpt_cal_ptr[cal_idx].xpt_cal_inputs.rgi;
      xpt_cal_data_ptr[cal_idx].xpt_cal_outputs.pout_meas = (float)(pc_xpt_cal_ptr[cal_idx].xpt_cal_outputs.pout_meas_db100 / 100.0);

      if ( debug_msg > 0 )
      {
        FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: Alignment Sweep[%d] rgi = %d", cal_idx, xpt_cal_data_ptr[cal_idx].xpt_cal_inputs.rgi);
        FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: Alignment Sweep[%d] bias = %d", cal_idx, xpt_cal_data_ptr[cal_idx].xpt_cal_inputs.bias);
        FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: Alignment Sweep[%d] pout_meas = %d", cal_idx, pc_xpt_cal_ptr[cal_idx].xpt_cal_outputs.pout_meas_db100);
      }
    }
    for (rgi_idx = 0; rgi_idx < num_rgi_delta; rgi_idx++)
    {
      rgi_delta_data_ptr[rgi_idx].pmeas = (float)(pc_rgi_delta_ptr[rgi_idx].pmeas_db100 / 100.0);
      rgi_delta_data_ptr[rgi_idx].rgi = pc_rgi_delta_ptr[rgi_idx].rgi;

      if ( debug_msg > 0 )
      {
        FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: RGI Delta Sweep[%d] rgi = %d", rgi_idx, rgi_delta_data_ptr[rgi_idx].rgi);
        FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: RGI Delta Sweep[%d] pmeas = %d", rgi_idx, pc_rgi_delta_ptr[rgi_idx].pmeas_db100);
      }
    }

    /* get DPD data from EFS for each cal sweep */
    for (cal_idx = 0; cal_idx < num_xpt_cal; cal_idx++)
    {
      /* append '_#.bin' to file name */
      pch = strtok_r(dpd_file_name_ptr, ".",&temp_str);
      if (pch == NULL) pch = dpd_file_name_ptr;
      snprintf(strFile, FILE_MAX_PATH, "%s_%d.bin", pch, cal_idx);

      /* get dpd data from file */
      api_status &= ftm_common_xpt_get_dpd_data(strFile, &dpd_data);

      if (api_status == TRUE)
      {
      /* Copy data to xpt_cal_data_ptr */
      for (i = 0; i < XPT_CAL_MAX_AMAM_SIZE; i++)
      {
        xpt_cal_data_ptr[cal_idx].xpt_cal_amams.amam_dac[i] = i+1;
        if ( debug_msg > 0 ) FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                       "amam_dac[%d] = %d",
                                       i, xpt_cal_data_ptr[cal_idx].xpt_cal_amams.amam_dac[i]);
      }
      memscpy(&(xpt_cal_data_ptr[cal_idx].xpt_cal_amams.amam_val[0]), 
              (sizeof(uint16)*XPT_CAL_MAX_AMAM_SIZE),
              dpd_data.amam, (sizeof(uint16)*XPT_CAL_MAX_AMAM_SIZE));
      if ( debug_msg > 0 ) 
      {
        for (i = 0; i < XPT_CAL_MAX_AMAM_SIZE; i++)
        {
          FTM_MSG_2(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "amam_val[%d] = %d",
                                         i, xpt_cal_data_ptr[cal_idx].xpt_cal_amams.amam_val[i]);
        }
      }
      xpt_cal_data_ptr[cal_idx].xpt_cal_outputs.rms_amam = (float)dpd_data.rx_post_eq_rms_mag / 100;
      if ( debug_msg > 0 ) FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                     "rx_post_eq_rms_mag = %d",
                                     dpd_data.rx_post_eq_rms_mag);
      }
      else /* (api_status == FALSE)*/
      {
        FTM_MSG(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "ftm_common_xpt_get_dpd_file() failed");
      }
    }
  }

  /* allocate memory for function output param */
  if (api_status == TRUE)
  {
    num_bytes = num_rgi_delta * sizeof(ftm_common_xpt_mline_outputs);
    mline_outputs_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
    if (mline_outputs_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "mline_outputs_ptr memory alloc failed for %d bytes.",
                num_bytes);
      api_status = FALSE;
    }
  }

  /* Need to populate the RMS dBFs and Env Scale Unity value from NV for M Line calc */
#ifdef FEATURE_WCDMA
  if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
  {
    mline_params.rms_dbfs = (float)(rfwcdma_msm_get_rms_dbfs_x100_val())/100.0;

    nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode) );
    if (xpt_type == XPT_TYPE_EPT)
    {
    mline_params.env_scale = (int32)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "WCDMA EPT Env Scale = %d",
                                         mline_params.env_scale);
    }
    else if (xpt_type == XPT_TYPE_ET)
    {
      mline_params.env_scale = (int32)nv_tbl->tx_ept_dpd_config_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "WCDMA ET Env Scale = %d",
                                         mline_params.env_scale);
    }
    else
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "Invalid XPT Type",
                xpt_type);
      api_status = FALSE;
    }
  }
#endif /* FEATURE_WCDMA */

  #ifdef FEATURE_CDMA1X
  if (ftm_rf_tech == FTM_RF_TECH_CDMA)
  {
    mline_params.rms_dbfs = (float)(rf_cdma_msm_get_rms_dbfs_x100_val())/100.0;

    /* This needs a device input!! */
    if (xpt_type == XPT_TYPE_EPT)
    {
    mline_params.env_scale = (int32)(rf_cdma_xpt_get_cfg_param( (rfm_device_enum_type)0, EPT_CFG_CMN_EVN_SCALE_UNITY ));
      FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "CDMA EPT Env Scale = %d",
                                         mline_params.env_scale);
    }
    else if (xpt_type == XPT_TYPE_ET)
    {
      mline_params.env_scale = (int32)(rf_cdma_xpt_get_cfg_param( (rfm_device_enum_type)0, ET_CFG_CMN_ENV_SCALE_UNITY ));
      FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "CDMA ET Env Scale = %d",
                                         mline_params.env_scale);
    }
    else
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "Invalid XPT Type",
                xpt_type);
      api_status = FALSE;
    }
  }
  #endif /* FEATURE_CDMA1X */

  #ifdef FEATURE_LTE
  if (ftm_rf_tech == FTM_RF_TECH_LTE)
  {
    mline_params.rms_dbfs = (float)(rflte_msm_get_rms_dbfs_x100_val())/100.0;
    current_carrier_idx = rflte_ftm_mc_sm_get_carrier_index();
    ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
    rfnv_band = rflte_core_convert_rfcom_band_to_rfnv_band(ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_band);
    rflte_nv_tbl_ptr = rflte_mc_get_nv_tbl_ptr(current_carrier_idx);
    if (rflte_nv_tbl_ptr != NULL)
    {

      if (xpt_type == XPT_TYPE_EPT)
      {
    mline_params.env_scale = (int32)rflte_nv_tbl_ptr->tx_xpt_dpd_config_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
        FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                           "LTE EPT Env Scale = %d",
                                           mline_params.env_scale);
      }
      else if (xpt_type == XPT_TYPE_ET)
      {
        mline_params.env_scale = (int32)rflte_nv_tbl_ptr->tx_xpt_dpd_config_params[ET_CFG_CMN_ENV_SCALE_UNITY];
        FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                           "LTE ET Env Scale = %d",
                                           mline_params.env_scale);
      }
      else
      {
        FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                  "Invalid XPT Type",
                  xpt_type);
        api_status = FALSE;
      }
    }
    else
    {
      ERR_FATAL("nv_tbl_ptr null for band=%d", rfnv_band, 0, 0);
    }
    
  }
  #endif

  #ifdef FEATURE_TDSCDMA
  if (ftm_rf_tech == FTM_RF_TECH_TDSCDMA)
  {
    mline_params.rms_dbfs = (float)(rf_tdscdma_msm_get_rms_dbfs_x100_val())/100.0;

    tds_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
    if (xpt_type == XPT_TYPE_EPT)
    {
      mline_params.env_scale = (int32)tds_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "TDSCDMA EPT Env Scale = %d",
                                         mline_params.env_scale);
    }
    else if (xpt_type == XPT_TYPE_ET)
    {
      mline_params.env_scale = (int32)tds_nv_tbl_ptr->ept_dpd_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: "
                                         "TDSCDMA ET Env Scale = %d",
                                         mline_params.env_scale);
      /* This is currently an error condition for TDS */
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "ET Type not supported",
                xpt_type);
    }
    else
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "Invalid XPT Type",
                xpt_type);
      api_status = FALSE;
    }
  }
  #endif

  if ( debug_msg > 0 ) 
  {
    FTM_MSG_1(FTM_MED, "ftm_common_xpt_mline_compute: MLine RMS dBFs = %d", mline_params.rms_dbfs*10000);
    FTM_MSG_1(FTM_MED, "ftm_common_xpt_mline_compute: MLine Env Scale = %d", mline_params.env_scale);
  }

  /* perform m-line computation */
  if (api_status == TRUE)
  {
    api_status = ftm_common_xpt_mline_calc(xpt_cal_data_ptr, num_xpt_cal, rgi_delta_data_ptr, num_rgi_delta, &mline_params, mline_outputs_ptr, mline_results, debug_msg);
    if (api_status == FALSE)
    {
      FTM_MSG(FTM_ERROR, "ftm_common_xpt_mline_compute: "
              "ftm_common_xpt_mline_calc() failed");
    }
  }

  if (api_status == TRUE)
  {
    num_bytes = num_rgi_delta * sizeof(ftm_common_xpt_mline_outputs_int_data);
    /* allocate memory for mline output for resp pkt */
    pc_mline_outputs_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
    if (pc_mline_outputs_ptr == NULL)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_common_xpt_mline_compute: "
                "pc_mline_outputs_ptr memory alloc failed for %d bytes.",
                num_bytes);
      api_status = FALSE;
    }
  }

  if (api_status == TRUE)
  {
    /* copy output parameters to results structure */
    for (rgi_idx = 0; rgi_idx < num_rgi_delta; rgi_idx++)
    {
      pc_mline_outputs_ptr[rgi_idx].rgi = rgi_delta_data_ptr[rgi_idx].rgi;
      pc_mline_outputs_ptr[rgi_idx].bias_out = mline_outputs_ptr[rgi_idx].bias_out;
      pc_mline_outputs_ptr[rgi_idx].pow_out_db10 = (uint32)(mline_outputs_ptr[rgi_idx].pow_out * 10);
      if ( debug_msg > 0 ) FTM_MSG_3(FTM_LOW, "ftm_common_xpt_mline_compute: M-Line Results: "
                                     "rgi = %d, bias = %d, pwr = %d", 
                                     pc_mline_outputs_ptr[rgi_idx].rgi, pc_mline_outputs_ptr[rgi_idx].bias_out, pc_mline_outputs_ptr[rgi_idx].pow_out_db10 );
    }

    /* add results to response packet */
    rsp_pkt->payload_len = payload_len;
    if ( debug_msg > 0 ) FTM_MSG_1(FTM_LOW, "ftm_common_xpt_mline_compute: payload length = %d bytes", payload_len );

    /* Copy the results memory to the response packet payload */
    memscpy(&(rsp_pkt->payload), payload_len,pc_mline_outputs_ptr, payload_len);
  }

  /* free allocated memory*/
  if (pc_xpt_cal_ptr != NULL)
  {
    modem_mem_free(pc_xpt_cal_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if (pc_rgi_delta_ptr != NULL)
  {
    modem_mem_free(pc_rgi_delta_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if (mline_outputs_ptr != NULL)
  {
    modem_mem_free(mline_outputs_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if (pc_mline_outputs_ptr != NULL)
  {
    modem_mem_free(pc_mline_outputs_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if (xpt_cal_data_ptr != NULL)
  {
    modem_mem_free(xpt_cal_data_ptr, MODEM_MEM_CLIENT_RFA);
  }

  /* Set response pkt to indicate failure */
  if (api_status == FALSE)
  {
    /* Set response pkt to indicate failure */
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG(FTM_ERROR, "ftm_common_xpt_mline_compute: Bad Response");
  }

  return ftm_rsp_data;
} /* ftm_common_xpt_mline_compute */

static
ftm_rsp_pkt_type
ftm_common_fetch_et_delay
(
  void* ftm_req_data,
  ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;
  boolean cap_status = FALSE;
  boolean proc_status = FALSE;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_iq_xpt_delay_capture_rsp_pkt_type *rsp_pkt; /*Packet Payload*/
  char *iq_file_name = NULL;
  char *results_file_name = NULL;
  /* Track current location during packet parsing */
  byte *curr_pkt_ptr = NULL;
  uint16 num_bytes;
  boolean capture_done = FALSE;
  boolean processing_done = FALSE;
  uint8 iq_buffer_index = -1;
  uint8 results_buffer_index = -1;
  rf_common_xpt_buffer_id_type buffer_ids; /* buffer ids for capture and dpd results */
  uint32 samp_rate=-1;
  boolean is_reduced_leakage_on;
  const rfc_common_logical_device_params_type *common_logical_device_ptr = NULL;
  #ifdef FEATURE_CDMA1X
  const rfc_cdma_logical_device_params_type *cdma_logical_device_ptr = NULL;
  #endif

  rfcommon_xpt_processing_type proc_flag = PROC_AM_PM_CURVE; /* prcessing level same as AMPM processing */
  int32 et_delay_1000th_of_sample = 0; // delay in FW return format
  int32 et_delay_0p1ns;            // delay in 0.1ns unit, to return to Tools
  int16 delay_comp_x10ns = 0;

#ifdef FEATURE_LTE
  rfcom_lte_band_type current_band;
#endif

  /* Extract parameters */
  ftm_iq_xpt_delay_capture_type *iq_capt_params = 
          (ftm_iq_xpt_delay_capture_type*)(&((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params);

  /* Create a new reponse packet in the ftm heap. */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( 
                              ftm_req_data,
                              sizeof(ftm_iq_xpt_delay_capture_rsp_pkt_type));

  /* Init Buffer ID values */
  buffer_ids.capt_buf_id = 0;
  buffer_ids.res_buf_id = 0;

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_iq_xpt_delay_capture_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_fetch_et_delay: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }
  else
  {
    /* Init to "FAIL" for now. 
       If all goes OK, should be "PASS" at end of API */
    rsp_pkt->status = FALSE;
  }

  common_logical_device_ptr = rfc_common_get_logical_device_params(iq_capt_params->device_fb);
  if (common_logical_device_ptr == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: "
               "NULL logical device for device %d",
               iq_capt_params->device_fb);
    return ftm_rsp_data;
  }

  /* If file names are given, make sure debug state is set to 1 = enabled */
  if( (iq_capt_params->debug_state == 0) && 
      ((iq_capt_params->iq_file_name_len > 0) || ( iq_capt_params->results_file_name_len > 0)) )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_3( FTM_ERROR, "ftm_common_fetch_et_delay: "
               "Debug state of %d must be issued with file(s) name len of 0. "
               "IQ name len %d, dpd file name len %d",
               iq_capt_params->debug_state, iq_capt_params->iq_file_name_len,
               iq_capt_params->results_file_name_len );
    return ftm_rsp_data;
  }

  /* File names must be at least 4 chars: 1_char_for_name + '.' + 1_char_for_ext + '\0' */
  if((iq_capt_params->iq_file_name_len > 0) && (iq_capt_params->iq_file_name_len < 4))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: "
               "IQ file name len %d is < the minimum expected 4 chars including NULL terminator.", 
               iq_capt_params->iq_file_name_len );
    return ftm_rsp_data;
  }

  /* File names must be at least 4 chars: 1_char_for_name + '.' + 1_char_for_ext + '\0' */
  if((iq_capt_params->results_file_name_len > 0) && (iq_capt_params->results_file_name_len < 4))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: "
               "Results file name len %d is < the minimum expected 4 chars including NULL terminator.", 
               iq_capt_params->results_file_name_len );
    return ftm_rsp_data;
  }

  /* Ensure no previus capture/processing is still in progress */
  if( (api_status == TRUE) )
  {
    api_status &= rfcommon_xpt_is_capture_done();
    if( api_status == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: IQ capture already "
                 "in progress for tech %d", ftm_rf_tech );
    }

    /* Monitor processing only if it was requested */
    api_status &= rfcommon_xpt_is_processing_done();
    if( api_status == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: IQ processing already "
                 "in progress for tech %d", ftm_rf_tech );
    }
  }

  /* Set capture and processing flags */
  if( (api_status == TRUE) )
  {
    api_status &= rfcommon_xpt_set_capture_in_progress();
    api_status &= rfcommon_xpt_set_processing_in_progress();
    if (iq_capt_params->first_trig == 1)
    {
      /* Initialize buffer pool for xpt captures */
      rfcommon_core_xpt_init_buffers();
    }
    /* Get buffer ids, TODO: need to pass in do_processing flag to not get dpd result buffer */
    api_status &= rfcommon_core_xpt_get_buffers(&buffer_ids);
  }

  /* Configure reduced-leakage device settings */
  FTM_MSG_1( FTM_LOW, "ftm_common_fetch_et_delay: RF device %d reduced leakage cfg ON.",
             iq_capt_params->device_fb );
  is_reduced_leakage_on = TRUE;
  rfdevice_cmn_cmd_dispatch( common_logical_device_ptr->cmn_device,
                             RFDEVICE_CFG_REDUCED_LEAKAGE,
                             &is_reduced_leakage_on );

  /* Clean up delay char memory for each channel */
  ftm_cal_data_teardown_delay_map();

  /* Trigger tech-specific capture */
  switch ( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
    #ifdef FEATURE_CDMA1X
    {
      /* Perform capture */
      if(api_status == TRUE)
      {
        api_status &=
          ftm_1x_xpt_iq_capture( iq_capt_params->device,
                                 iq_capt_params->device_fb,
                                 (uint8)XPT_CAPTURE_DELAY,
                                 iq_capt_params->num_samples,
                                 iq_capt_params->first_trig,
                                 iq_capt_params->last_trig,
                                 proc_flag,
                                 buffer_ids,
                                 &samp_rate
                                );
      }
    }
    #endif
    break;

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
      {
      /* Perform capture */
      if(api_status == TRUE)
      {
        api_status &=
          ftm_wcdma_xpt_iq_capture(iq_capt_params->device, 
                                   iq_capt_params->device_fb,
                                   (uint8)XPT_CAPTURE_DELAY,
                                   iq_capt_params->num_samples,
                                   (boolean)(iq_capt_params->first_trig),
                                   (boolean)(iq_capt_params->last_trig),
                                   proc_flag, 
                                   buffer_ids,
                                   &samp_rate);
      }
    }
    #endif
    break;

  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
    {
      if(api_status == TRUE)
      {
        api_status &= ftm_lte_xpt_iq_capture(iq_capt_params->device, 
                                             iq_capt_params->device_fb,
                                             iq_capt_params->num_samples,
                                             iq_capt_params->first_trig,
                                             iq_capt_params->last_trig,
                                             proc_flag,
                                             XPT_CAPTURE_DELAY,
                                             buffer_ids.capt_buf_id,
                                             buffer_ids.res_buf_id,
                                             &samp_rate);
      }
    }
    #endif
    break;


    case FTM_RF_TECH_TDSCDMA:
    #ifdef FEATURE_TDSCDMA
    {
      if(api_status == TRUE)
      {
        api_status &= ftm_tdscdma_xpt_iq_capture(iq_capt_params->device, 
                                                 iq_capt_params->device_fb,
                                                 (uint8)XPT_CAPTURE_DELAY,
                                                 iq_capt_params->num_samples,
                                                 (boolean)(iq_capt_params->first_trig),
                                                 (boolean)(iq_capt_params->last_trig),
                                                 proc_flag, 
                                                 buffer_ids,
                                                 &samp_rate);
      }
    }
    #endif
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: Mode %d not supported",
               ftm_rf_tech );
    api_status = FALSE;
    break;
  } /* Trigger tech-specific capture */

  FTM_MSG_1( FTM_HIGH, "ftm_common_fetch_et_delay: Samp rate %d ",
             samp_rate );

  /* Wait for capture done */
  /* If tech capture !done, proceed to monitor common done */
  if( (api_status == TRUE) )
  {
    capture_done = rfcommon_xpt_wait_for_capture_done( 500, 6000, &iq_buffer_index );
    if( capture_done == FALSE )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: Sample "
                 "capture did not complete for tech %d", ftm_rf_tech );
      api_status = FALSE;
      cap_status = FALSE;
    }
    else
    {
      FTM_MSG_1(FTM_LOW, "ftm_common_fetch_et_delay: Capture done for CaptBuf %d", iq_buffer_index);
      cap_status = TRUE;
    }
  }

  /* Wait for processing done */
  if ( (api_status == TRUE)  && (cap_status == TRUE) )
  {
    processing_done = rfcommon_xpt_wait_for_processing_done( 1000, 35000, &results_buffer_index );
    if ( processing_done == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: "
               "Processing did not complete for tech %d", ftm_rf_tech );
      api_status = FALSE;
      proc_status = FALSE;
    }
    else
    {
      FTM_MSG_1(FTM_LOW, "ftm_common_fetch_et_delay: Proc done for ResBuf %d", results_buffer_index);
      proc_status = TRUE;
    }
  }

  /* Get delay results */
  if( (api_status == TRUE) && (proc_status == TRUE) )
  {
    api_status = rfcommon_mc_et_path_delay(results_buffer_index, &et_delay_1000th_of_sample);
    if ( api_status == FALSE)
    {
      FTM_MSG( FTM_ERROR, "ftm_common_fetch_et_delay: "
               "ET Path delay results query failed" );
    }
    else
    {
      FTM_MSG_1( FTM_LOW, "ftm_common_fetch_et_delay: "
                          "ET Path delay results query succeeded, returned %d / 1000 samples", 
                          et_delay_1000th_of_sample );
    }
  }

  /* Re-store reduced-leakage device settings */
  FTM_MSG_1( FTM_LOW, "ftm_common_fetch_et_delay: RF device %d reduced leakage cfg OFF.",
             iq_capt_params->device_fb );
  is_reduced_leakage_on = FALSE;
  rfdevice_cmn_cmd_dispatch( common_logical_device_ptr->cmn_device,
                             RFDEVICE_CFG_REDUCED_LEAKAGE,
                             &is_reduced_leakage_on );

  /* Initialize the current packet pointer to first byte of payload */
  curr_pkt_ptr = (byte*)(&(iq_capt_params->payload_start[0]));

  /* Save IQ data */
  num_bytes = sizeof(char) * iq_capt_params->iq_file_name_len;
  if( (cap_status == TRUE) && 
      (iq_capt_params->debug_state == 1) &&
      (iq_capt_params->iq_file_name_len > 0) &&
      (capture_done == TRUE) )
  {
    iq_file_name = modem_mem_alloc( num_bytes, MODEM_MEM_CLIENT_RFA );
    if (iq_file_name == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: Could not allocate %d bytes "
                 "to hold IQ file name. Cannot save IQ.", num_bytes );
      api_status = FALSE;
    }
    else
    {
      memscpy(iq_file_name, num_bytes,curr_pkt_ptr, num_bytes);
      if (iq_file_name[num_bytes-1] != '\0')
      {
        FTM_MSG( FTM_ERROR, "ftm_common_fetch_et_delay: Provided IQ file name "
                   "is not NULL terminated. Cannot save IQ data." );
        api_status = FALSE;
      }
      else
      {
        /* Save IQ data */
        api_status &= 
          rfcommon_mc_save_xpt_iq_data( iq_buffer_index, iq_capt_params->num_samples, iq_file_name );
      }
    }
  } /* Save IQ data */

  /* Always account for previous payload size or next item will be incorrect */
  if(curr_pkt_ptr != NULL)
  {
    curr_pkt_ptr += num_bytes;
  }

  /* Save DPD data */
  num_bytes = sizeof(char) * iq_capt_params->results_file_name_len;
  if( (cap_status == TRUE) && 
      (proc_status == TRUE) && 
      (iq_capt_params->debug_state == 1) &&
      (iq_capt_params->results_file_name_len > 0) &&
      (processing_done == TRUE))
  {
    results_file_name = modem_mem_alloc( num_bytes, MODEM_MEM_CLIENT_RFA );
    if (results_file_name == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: Could not allocate %d bytes "
                 "to hold delay results file name. Cannot save delay results.", num_bytes );
      api_status = FALSE;
    }
    else
    {
      memscpy(results_file_name, num_bytes, curr_pkt_ptr, num_bytes);
      if (results_file_name[num_bytes-1] != '\0')
      {
        FTM_MSG( FTM_ERROR, "ftm_common_fetch_et_delay: Provided delay results file name "
                   "is not NULL terminated. Cannot save delay results data." );
        api_status = FALSE;
      }
      else
      {
        /* Save DPD data */
        api_status &= 
          rfcommon_mc_save_xpt_dpd_data( results_buffer_index, results_file_name );
      }
    }
  } /* Save DPD data */

  /* Always account for previous payload size or next item will be incorrect */
  if(curr_pkt_ptr != NULL)
  {
    curr_pkt_ptr += num_bytes;
  }

  if ( (iq_capt_params->last_trig == 1) )
  {
    /* release all buffer ids */
    rfcommon_core_xpt_free_all_buffers();
  }

  if ( iq_file_name != NULL )
  {
    modem_mem_free( iq_file_name, MODEM_MEM_CLIENT_RFA );
  }
  if ( results_file_name != NULL )
  {
    modem_mem_free( results_file_name, MODEM_MEM_CLIENT_RFA );
  }

  /* convert ET path delay from FW format (1/1000-th of sample) to units of 0.1ns */
  ftm_common_xpt_convert_delay(et_delay_1000th_of_sample, samp_rate, &et_delay_0p1ns);
  FTM_MSG_3(FTM_LOW,"et_path_delay: %d, samp_rate: %d, converted_delay: %d", et_delay_1000th_of_sample, samp_rate, et_delay_0p1ns);
 
#ifdef FEATURE_LTE   
  if (ftm_rf_tech == FTM_RF_TECH_LTE)
  {
     current_band = rflte_core_get_band_from_tx_uarfcn(rflte_mc_state.rf_path_state[0].curr_chan_tx);
     rfdevice_lte_tx_get_et_delay_compensation( RFCOM_TRANSCEIVER_0,
                                             current_band,
                                             rflte_mc_state.rf_path_state[0].bw_tx,
                                             &delay_comp_x10ns );
  }
#endif 

#ifdef FEATURE_WCDMA
  if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
  {
    rfdevice_wcdma_tx_get_et_delay_compensation(RFCOM_TRANSCEIVER_0,
                                                ftm_curr_wcdma_mode,
                                                &delay_comp_x10ns);
  }
#endif

  #ifdef FEATURE_CDMA1X
  if (ftm_rf_tech == FTM_RF_TECH_CDMA)
  {
    /* Get Logical Device Param */

    rfm_cdma_band_class_type band = RFM_CDMA_MAX_BAND ; 
    cdma_logical_device_ptr = rfc_cdma_get_logical_device_params( iq_capt_params->device );
    band = rf_cdma_data_get_curr_band ( iq_capt_params->device ) ;
    if ( (cdma_logical_device_ptr == NULL) ||
         (cdma_logical_device_ptr->tx_device[band] == NULL) )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_fetch_et_delay: Device %d delay not compensated. Null CDMA logical Tx device.", 
                 iq_capt_params->device );
      api_status = FALSE;
    }
    else
    {
      api_status &= 
        rfdevice_cdma_tx_get_et_delay_compensation( 
           cdma_logical_device_ptr->tx_device[band] , &delay_comp_x10ns );
    }
  }
  #endif

  et_delay_0p1ns -= delay_comp_x10ns;

  if (rsp_pkt != NULL)
  {
    rsp_pkt->status = api_status;
    rsp_pkt->et_path_delay = et_delay_0p1ns; // in units of 0.1ns
  }

  if ( api_status == FALSE)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_fetch_et_delay: api status = FAIL" );
  }

  return ftm_rsp_data;
}

static
ftm_rsp_pkt_type
ftm_common_calculate_xpt_pin_compression
(
  void* ftm_req_data,
  ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;
  boolean cap_status = FALSE;

  ftm_cal_data_pin_map_type *pin_map;
  uint8 ndb_point = 0;
  uint8 max_compression = 0;
  float amam_vin[XPT_CAL_MAX_AMAM_SIZE];
  uint8 foundCompPt = 0;
  uint16 i;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_iq_xpt_pin_capture_rsp_pkt_type *rsp_pkt; /*Packet Payload*/
  char *iq_file_name = NULL;

  /* Track current location during packet parsing */
  byte *curr_pkt_ptr = NULL;
  uint16 num_bytes;
  boolean capture_done = FALSE;
  uint32 samp_rate=-1;
  uint8 iq_buffer_index = -1;
  rf_common_xpt_buffer_id_type buffer_ids; /* buffer ids for capture and dpd results */

  const rfc_common_logical_device_params_type *common_logical_device_ptr = NULL;

  rfcommon_xpt_processing_type proc_flag = PROC_AM_PM_CURVE; 

  /* Extract parameters */
  ftm_iq_xpt_pin_capture_type *iq_capt_params = 
          (ftm_iq_xpt_pin_capture_type*)(&((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params);

  /* Create a new reponse packet in the ftm heap. */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( 
                              ftm_req_data,
                              sizeof(ftm_iq_xpt_pin_capture_rsp_pkt_type));

  /* Init Buffer ID values */
  buffer_ids.capt_buf_id = 0;
  buffer_ids.res_buf_id = 0;

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_iq_xpt_pin_capture_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_calculate_et_pin_compression: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }
  else
  {
    /* Init to "FAIL" for now. 
       If all goes OK, should be "PASS" at end of API */
    rsp_pkt->status = FALSE;
  }

  common_logical_device_ptr = rfc_common_get_logical_device_params(iq_capt_params->device_fb);
  if (common_logical_device_ptr == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_calculate_et_pin_compression: "
               "NULL logical device for device %d",
               iq_capt_params->device_fb);
    return ftm_rsp_data;
  }

  /* If file names are given, make sure debug state is set to 1 = enabled */
  if( (iq_capt_params->debug_state == 0) && 
      (iq_capt_params->iq_file_name_len > 0) )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_common_calculate_et_pin_compression: "
               "Debug state of %d must be issued with file(s) name len of 0. "
               "IQ name len %d",
               iq_capt_params->debug_state, iq_capt_params->iq_file_name_len);
    return ftm_rsp_data;
  }

  /* File names must be at least 4 chars: 1_char_for_name + '.' + 1_char_for_ext + '\0' */
  if((iq_capt_params->iq_file_name_len > 0) && (iq_capt_params->iq_file_name_len < 4))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_calculate_et_pin_compression: "
               "IQ file name len %d is < the minimum expected 4 chars including NULL terminator.", 
               iq_capt_params->iq_file_name_len );
    return ftm_rsp_data;
  }

  if (iq_capt_params->first_trig == 1)
  {
    /* Initialize buffer pool for xpt captures */
    rfcommon_core_xpt_init_buffers();
  }
  /* Get buffer ids, TODO: need to pass in do_processing flag to not get dpd result buffer */
  api_status &= rfcommon_core_xpt_get_buffers(&buffer_ids);

  /* Clean up pin map */
  ftm_cal_data_teardown_pin_map();

  /*acquire pin map ptr*/
  pin_map = ftm_common_data_get_pin_table_ptr();

  /* Trigger tech-specific capture */
  switch ( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
    #ifdef FEATURE_CDMA1X
    {
      /* Perform capture */
      if(api_status == TRUE)
      {
        api_status &=
          ftm_1x_xpt_iq_capture( iq_capt_params->device,
                                 iq_capt_params->device_fb,
                                 (uint8)XPT_CAPTURE_PIN,
                                 iq_capt_params->num_samples,
                                 iq_capt_params->first_trig,
                                 iq_capt_params->last_trig,
                                 proc_flag,
                                 buffer_ids,
                                 &samp_rate
                                );
      }
    }
    #endif
    break;

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
      {
      /* Perform capture */
      if(api_status == TRUE)
      {
        api_status &=
          ftm_wcdma_xpt_iq_capture(iq_capt_params->device, 
                                   iq_capt_params->device_fb,
                                   (uint8)XPT_CAPTURE_PIN,
                                   iq_capt_params->num_samples,
                                   (boolean)(iq_capt_params->first_trig),
                                   (boolean)(iq_capt_params->last_trig),
                                   proc_flag, 
                                   buffer_ids,
                                   &samp_rate);
      }
    }
    #endif
    break;

  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
    {
      if(api_status == TRUE)
      {
        api_status &= ftm_lte_xpt_iq_capture(iq_capt_params->device, 
                                             iq_capt_params->device_fb,
                                             iq_capt_params->num_samples,
                                             iq_capt_params->first_trig,
                                             iq_capt_params->last_trig,
                                             proc_flag,
                                             XPT_CAPTURE_PIN,
                                             buffer_ids.capt_buf_id,
                                             buffer_ids.res_buf_id,
                                             &samp_rate);
      }
    }
    #endif
    break;


    case FTM_RF_TECH_TDSCDMA:
    #ifdef FEATURE_TDSCDMA
    {
      if(api_status == TRUE)
      {
        api_status &= ftm_tdscdma_xpt_iq_capture(iq_capt_params->device, 
                                                 iq_capt_params->device_fb,
                                                 (uint8)XPT_CAPTURE_PIN,
                                                 iq_capt_params->num_samples,
                                                 (boolean)(iq_capt_params->first_trig),
                                                 (boolean)(iq_capt_params->last_trig),
                                                 proc_flag, 
                                                 buffer_ids,
                                                 &samp_rate);
      }
    }
    #endif
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "ftm_common_calculate_et_pin_compression: Mode %d not supported",
               ftm_rf_tech );
    api_status = FALSE;
    break;
  } /* Trigger tech-specific capture */

  /* Wait for capture done */
  /* If tech capture !done, proceed to monitor common done */
  if( (api_status == TRUE) )
  {
    capture_done = rfcommon_xpt_wait_for_capture_done( 500, 6000, &iq_buffer_index );
    if( capture_done == FALSE )
    {
      api_status = FALSE;
      cap_status = FALSE;
    }
    else
    {
      FTM_MSG_1(FTM_LOW, "ftm_common_calculate_et_pin_compression: Capture done for CaptBuf %d", iq_buffer_index);
      cap_status = TRUE;
    }
  }

  /* Get Pin results */
  if(cap_status == TRUE)
  {
    for (i=0;i<XPT_CAL_MAX_AMAM_SIZE;i++)
    {
      amam_vin[i] = (float)i;
    }
    ftm_common_xpt_find_max_gain_ndb_point( amam_vin,
                                            pin_map->table[0].vout,
                                            XPT_CAL_MAX_AMAM_SIZE,
                                            (float)iq_capt_params->target_compression/10,
                                            &max_compression,
                                            &ndb_point,
                                            &foundCompPt,
                                            0);
    api_status = (boolean)foundCompPt;
  }

  /* Initialize the current packet pointer to first byte of payload */
  curr_pkt_ptr = (byte*)(&(iq_capt_params->payload_start[0]));

  /* Save IQ data */
  num_bytes = sizeof(char) * iq_capt_params->iq_file_name_len;
  if( (cap_status == TRUE) && 
      (iq_capt_params->debug_state == 1) &&
      (iq_capt_params->iq_file_name_len > 0) &&
      (capture_done == TRUE) )
  {
    iq_file_name = modem_mem_alloc( num_bytes, MODEM_MEM_CLIENT_RFA );
    if (iq_file_name == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_calculate_et_pin_compression: Could not allocate %d bytes "
                 "to hold IQ file name. Cannot save IQ.", num_bytes );
      api_status = FALSE;
    }
    else
    {
      memscpy(iq_file_name, num_bytes,curr_pkt_ptr, num_bytes);
      if (iq_file_name[num_bytes-1] != '\0')
      {
        FTM_MSG( FTM_ERROR, "ftm_common_calculate_et_pin_compression: Provided IQ file name "
                   "is not NULL terminated. Cannot save IQ data." );
        api_status = FALSE;
      }
      else
      {
        /* Save IQ data */
        api_status &= 
          rfcommon_mc_save_xpt_iq_data( iq_buffer_index, iq_capt_params->num_samples, iq_file_name );
      }
    }
  } /* Save IQ data */

  if ( (iq_capt_params->last_trig == 1) )
  {
    /* release all buffer ids */
    rfcommon_core_xpt_free_all_buffers();
  }

  if ( iq_file_name != NULL )
  {
    modem_mem_free( iq_file_name, MODEM_MEM_CLIENT_RFA );
  }

  if (rsp_pkt != NULL)
  {
    rsp_pkt->status = api_status;
    rsp_pkt->target_compression_index = ndb_point; 
    rsp_pkt->max_compression = max_compression;
  }

  if ( api_status == FALSE)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_calculate_et_pin_compression: api status = FAIL" );
  }

  return ftm_rsp_data;
}

static
ftm_rsp_pkt_type
ftm_common_trigger_ept_iq_cap
(
   void* ftm_req_data,
   ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_iq_capture_rsp_pkt_type *rsp_pkt; /*Packet Payload*/
  char *iq_file_name = NULL;
  char *dpd_file_name = NULL;
  /* Track current location during packet parsing */
  byte *curr_pkt_ptr = NULL;
  uint16 num_bytes;
  boolean capture_done = FALSE;
  boolean processing_done = FALSE;
  uint8 iq_buffer_index = -1;
  uint8 dpd_buffer_index = -1;
  rf_common_xpt_buffer_id_type buffer_ids; /* buffer ids for capture and dpd results */
  rfcommon_xpt_processing_type proc_flag = PROC_NONE;

  /* Extract parameters */
  ftm_iq_capture_type *iq_capt_params = 
          (ftm_iq_capture_type*)(&((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params);
  proc_flag = (iq_capt_params->do_processing>0)?PROC_DPD:PROC_NONE;

  /* Create a new reponse packet in the ftm heap. */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( 
                              ftm_req_data,
                              sizeof(ftm_iq_capture_rsp_pkt_type));

  /* Init Buffer ID values */
  buffer_ids.capt_buf_id = 0;
  buffer_ids.res_buf_id = 0;
  
  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_iq_capture_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }
  else
  {
    /* Init to "FAIL" for now. 
       If all goes OK, should be "PASS" at end of API */
    rsp_pkt->status = FALSE;
  }

  /* Make sure this API not used for delay characterization */
  if ((rfcommon_xpt_capture_type)(iq_capt_params->xpt_capture_type) == XPT_CAPTURE_DELAY)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
               "Processing type %d is for delay capture, use FTM_TRIGGER_EPT_DELAY_IQ_CAP",
               iq_capt_params->xpt_capture_type );
    return ftm_rsp_data;
  }


  /* If file names are given, make sure debug state is set to 1 = enabled */
  if( (iq_capt_params->debug_state == 0) && 
      ((iq_capt_params->iq_file_name_len > 0) || ( iq_capt_params->dpd_file_name_len > 0)) )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_3( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
               "Debug state of %d must be issued with file(s) name len of 0. "
               "IQ name len %d, dpd file name len %d",
               iq_capt_params->debug_state, iq_capt_params->iq_file_name_len,
               iq_capt_params->dpd_file_name_len );
    return ftm_rsp_data;
  }

  /* File names must be at least 4 chars: 1_char_for_name + '.' + 1_char_for_ext + '\0' */
  if((iq_capt_params->iq_file_name_len > 0) && (iq_capt_params->iq_file_name_len < 4))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
               "IQ file name len %d is < the minimum expected 4 chars including NULL terminator.", 
               iq_capt_params->iq_file_name_len );
    return ftm_rsp_data;
  }

  /* File names must be at least 4 chars: 1_char_for_name + '.' + 1_char_for_ext + '\0' */
  if((iq_capt_params->dpd_file_name_len > 0) && (iq_capt_params->dpd_file_name_len < 4))
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
               "DPD file name len %d is < the minimum expected 4 chars including NULL terminator.", 
               iq_capt_params->dpd_file_name_len );
    return ftm_rsp_data;
  }

  /* Ensure no previus capture/processing is still in progress */
  if( (api_status == TRUE) )
  {
    api_status &= rfcommon_xpt_is_capture_done();
    if( api_status == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: IQ capture already "
                 "in progress for tech %d", ftm_rf_tech );
    }

    /* Monitor processing only if it was requested */
    if (iq_capt_params->do_processing > 0)
    {
      api_status &= rfcommon_xpt_is_processing_done();
      if( api_status == FALSE)
      {
        FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: IQ processing already "
                   "in progress for tech %d", ftm_rf_tech );
      }
    }
  }

  /* Set capture and processing flags */
  if( (api_status == TRUE) )
  {
    api_status &= rfcommon_xpt_set_capture_in_progress();
    if (iq_capt_params->do_processing > 0)
    {
      api_status &= rfcommon_xpt_set_processing_in_progress();
    }
    if (iq_capt_params->first_trig == 1)
    {
      /* Initialize buffer pool for xpt captures */
      rfcommon_core_xpt_init_buffers();
    }
    /* Get buffer ids, TODO: need to pass in do_processing flag to not get dpd result buffer */
    api_status &= rfcommon_core_xpt_get_buffers(&buffer_ids);
  }

  /* Trigger tech-specific capture */
  switch ( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
    #ifdef FEATURE_CDMA1X
    {
      /* Perform capture */
      if(api_status == TRUE)
      {
        api_status &=
          ftm_1x_xpt_iq_capture( iq_capt_params->device,
                                 iq_capt_params->device_fb,
                                 iq_capt_params->xpt_capture_type,
                                 iq_capt_params->num_samples,
                                 iq_capt_params->first_trig,
                                 iq_capt_params->last_trig,
                                 proc_flag,
                                 buffer_ids,
                                 NULL
                                );
      }
    }
    #endif
    break;

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
      {
      /* Perform capture */
      if(api_status == TRUE)
      {
        api_status &=
          ftm_wcdma_xpt_iq_capture(iq_capt_params->device, 
                                   iq_capt_params->device_fb,
                                   iq_capt_params->xpt_capture_type,
                                   iq_capt_params->num_samples,
                                   (boolean)(iq_capt_params->first_trig),
                                   (boolean)(iq_capt_params->last_trig),
                                   proc_flag, 
                                   buffer_ids,
                                   NULL);
      }
    }
    #endif
    break;

  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
    {
      if(api_status == TRUE)
      {
          api_status &= ftm_lte_xpt_iq_capture(iq_capt_params->device, 
                                             iq_capt_params->device_fb,
                                             iq_capt_params->num_samples,
                                             iq_capt_params->first_trig,
                                             iq_capt_params->last_trig,
                                             proc_flag,
                                             iq_capt_params->xpt_capture_type,
                                             buffer_ids.capt_buf_id,
                                             buffer_ids.res_buf_id,
                                             NULL);
      }
    }
    #endif
    break;

  case FTM_RF_TECH_TDSCDMA:
    #ifdef FEATURE_TDSCDMA
    {
      if(api_status == TRUE)
      {
        api_status &= ftm_tdscdma_xpt_iq_capture(iq_capt_params->device, 
                                                 iq_capt_params->device_fb,
                                                 iq_capt_params->xpt_capture_type,
                                                 iq_capt_params->num_samples,
                                                 (boolean)(iq_capt_params->first_trig),
                                                 (boolean)(iq_capt_params->last_trig),
                                                 proc_flag, 
                                                 buffer_ids,
                                                 NULL);
      }
    }
    #endif
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: Mode %d not supported",
               ftm_rf_tech );
    api_status = FALSE;
    break;
  } /* Trigger tech-specific capture */

  /* Wait for capture done */
  /* If tech capture !done, proceed to monitor common done */
  if( (api_status == TRUE) )
  {
    capture_done = rfcommon_xpt_wait_for_capture_done( 500, 6000, &iq_buffer_index );
    if( capture_done == FALSE )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: Sample "
                 "capture did not complete for tech %d", ftm_rf_tech );
      api_status = FALSE;
    }
  }

  /* Wait for processing done */
  if( (api_status == TRUE) &&
      (iq_capt_params->do_processing > 0) )
  {
    processing_done = rfcommon_xpt_wait_for_processing_done( 1000, 35000, &dpd_buffer_index );
    if ( processing_done == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
               "Processing did not complete for tech %d", ftm_rf_tech );
      api_status = FALSE;
    }
  }

  /* Initialize the current packet pointer to first byte of payload */
  curr_pkt_ptr = (byte*)(&(iq_capt_params->payload_start[0]));

  /* Save IQ data */
  num_bytes = sizeof(char) * iq_capt_params->iq_file_name_len;
  if( (api_status == TRUE) && 
      (iq_capt_params->debug_state == 1) &&
      (iq_capt_params->iq_file_name_len > 0) &&
      (capture_done == TRUE) )
  {
    iq_file_name = modem_mem_alloc( num_bytes, MODEM_MEM_CLIENT_RFA );
    if (iq_file_name == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: Could not allocate %d bytes "
                 "to hold IQ file name. Cannot save IQ.", num_bytes );
      api_status = FALSE;
    }
    else
    {
      memscpy(iq_file_name, num_bytes,curr_pkt_ptr, num_bytes);
      if (iq_file_name[num_bytes-1] != '\0')
      {
        FTM_MSG( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: Provided IQ file name "
                   "is not NULL terminated. Cannot save IQ data." );
        api_status = FALSE;
      }
      else
      {
        /* Save IQ data */
        api_status &= 
          rfcommon_mc_save_xpt_iq_data( iq_buffer_index, iq_capt_params->num_samples, iq_file_name );
      }
    }
  } /* Save IQ data */

  /* Always account for previous payload size or next item will be incorrect */
  if(curr_pkt_ptr != NULL)
  {
    curr_pkt_ptr += num_bytes;
  }

  /* Save DPD data */
  num_bytes = sizeof(char) * iq_capt_params->dpd_file_name_len;
  if( (api_status == TRUE) && 
      (iq_capt_params->debug_state == 1) &&
      (iq_capt_params->dpd_file_name_len > 0) &&
      (processing_done == TRUE))
  {
    dpd_file_name = modem_mem_alloc( num_bytes, MODEM_MEM_CLIENT_RFA );
    if (dpd_file_name == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: Could not allocate %d bytes "
                 "to hold DPD file name. Cannot save DPD.", num_bytes );
      api_status = FALSE;
    }
    else
    {
      memscpy(dpd_file_name, num_bytes, curr_pkt_ptr, num_bytes);
      if (dpd_file_name[num_bytes-1] != '\0')
      {
        FTM_MSG( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: Provided DPD file name "
                   "is not NULL terminated. Cannot save DPD data." );
        api_status = FALSE;
      }
      else
      {
        /* Save DPD data */
        api_status &= 
          rfcommon_mc_save_xpt_dpd_data( dpd_buffer_index, dpd_file_name );
      }
    }
  } /* Save DPD data */

  /* Always account for previous payload size or next item will be incorrect */
  if(curr_pkt_ptr != NULL)
  {
    curr_pkt_ptr += num_bytes;
  }

  if ( (iq_capt_params->last_trig == 1) )
  {
    /* release all buffer ids */
    rfcommon_core_xpt_free_all_buffers();
  }

  if ( iq_file_name != NULL )
  {
    modem_mem_free( iq_file_name, MODEM_MEM_CLIENT_RFA );
  }

  if ( dpd_file_name != NULL )
  {
    modem_mem_free( dpd_file_name, MODEM_MEM_CLIENT_RFA );
  }

  if (rsp_pkt != NULL)
  {
    rsp_pkt->status = api_status;
  }

  if ( api_status == FALSE)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: api status = FAIL" );
  }

  return ftm_rsp_data;
} /* ftm_common_trigger_ept_iq_cap */
#endif /* FEATURE_GNSS_ONLY_NO_WWAN*/

  
ftm_rsp_pkt_type 
ftm_common_get_txlut_info_for_red_cal
(  void* ftm_req_data
)
{
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_txlut_info_for_red_cal_rsp_type* rsp_pkt = NULL;
  ftm_txlut_info_for_red_cal_request_type * req_pkt = (ftm_txlut_info_for_red_cal_request_type*) ftm_req_data;
  rfdevice_tx_lut_data_type lut_info;
  void*  txdev_cmn = NULL;
  ftm_rf_technology_type ftm_rf_tech ;
  uint8 band;
  rfcom_band_type_u uband;
  rfm_mode_enum_type rfm_mode_id;
  boolean status;
  uint8 num_req_pa_states;
  uint8* pa_states;
  uint16 lut_id_results[10] = {0};
  int i;

  status = ftm_common_get_band_tech_from_rf_mode(req_pkt->rf_mode, &band, &ftm_rf_tech);
  rfm_mode_id = ftm_common_resolve_rf_mode(req_pkt->rf_mode);


  FTM_MSG_4( FTM_HIGH, "ftm_common_get_txlut_info_for_red_cal: "
               "device (%d), rfmode(%d), alt_path %d, num_req_pa_states %d .",
               req_pkt->device, req_pkt->rf_mode , req_pkt->alt_path, req_pkt->num_req_paStates);

  /* Create a new reponse packet in the ftm heap. */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( 
                              ftm_req_data,
                              sizeof(ftm_txlut_info_for_red_cal_rsp_type)+ req_pkt->num_req_paStates*sizeof(uint16) - sizeof(uint8));

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_txlut_info_for_red_cal_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_set_xpt_txagc_override: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }

  pa_states = &req_pkt->pa_states_place;  

  lut_info.tx_gain_script = rf_buffer_create(RFCMN_TXLIN_AGC_WTR_DATA_TASK_NUM_TRANS, 
                                             RFCMN_TXLIN_AGC_WTR_DATA_TASK_NUM_TRANS, 
                                             0);

  lut_info.bbf_script =  rf_buffer_create( RFCMN_TXLIN_AGC_WXE_BBF_TASK_NUM_TRANS, 
                                           RFCMN_TXLIN_AGC_WXE_BBF_TASK_NUM_TRANS, 
                                           0 );

  lut_info.trigger_script = rf_buffer_create( RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS, 
                                              RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS, 
                                              0 );

  if (lut_info.tx_gain_script == NULL)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_FATAL, "rf buffer returned NULL! ", 0);
    ASSERT(0);
  }


  /* Get Tech specific RFC instance*/
  switch (ftm_rf_tech) {
  case FTM_RF_TECH_CDMA:
      #ifdef FEATURE_CDMA1X
      uband.cdma_band = (rfm_cdma_band_class_type) band;
      txdev_cmn = rfc_cdma_get_cmn_rxtx_device( req_pkt->device,uband.cdma_band ,req_pkt->alt_path);
      #endif
      break; 

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
     uband.wcdma_band = (rfcom_wcdma_band_type)band;
      txdev_cmn = rfc_wcdma_get_common_rxtx_device(req_pkt->device,uband.wcdma_band,req_pkt->alt_path);
      #endif
      break;

  case FTM_RF_TECH_LTE:
      #ifdef FEATURE_LTE
      uband.lte_band = (rfcom_lte_band_type)band;
      txdev_cmn = rfc_lte_get_common_rxtx_device(req_pkt->device,uband.lte_band,req_pkt->alt_path);
      #endif
      break;

  case FTM_RF_TECH_TDSCDMA:
      #ifdef FEATURE_TDSCDMA
      uband.tdscdma_band = (rfcom_tdscdma_band_type)band;
      txdev_cmn = rfc_tdscdma_get_common_rxtx_device(req_pkt->device,uband.tdscdma_band,req_pkt->alt_path);
      #endif
      break;
  default:
    break;

  }

  if (txdev_cmn != NULL)
  {
    rf_device_get_mfg_prd_ids(txdev_cmn, &rsp_pkt->mfg_id, &rsp_pkt->prd_id, &rsp_pkt->revision);
    /* Copy number of results, i.e. number of LUT ids returned for the requested number of PA states, 
        into response packet.
    */

    rsp_pkt->num_valid_paStates = req_pkt->num_req_paStates ;

    FTM_MSG_3(FTM_HIGH, " Num Valid PA states %d, Add Resp pkt %x , Add of lut results %x",rsp_pkt->num_valid_paStates,rsp_pkt, &(rsp_pkt->lut_ids_placeholder));
    for (i = 0; i < rsp_pkt->num_valid_paStates; i++) 
    {
      if ( rfdevice_cmn_get_tx_lut_scripts(txdev_cmn,rfm_mode_id,uband,
                                           pa_states[i],0,RFDEVICE_CREATE_SCRIPT,
                                           &lut_info))
      {
        FTM_MSG_2(FTM_HIGH, " LUT # for pa state %d is %d", pa_states[i],lut_info.lut_id);
        lut_id_results[i] = lut_info.lut_id;
      }
      else
      {
        FTM_MSG_2(FTM_HIGH, " Failed:LUT # for pa state %d is %d", pa_states[i],lut_info.lut_id);
      }
          /* Clear buffer */
      (void)rf_buffer_clear(lut_info.tx_gain_script);
      (void)rf_buffer_clear(lut_info.bbf_script);
      (void)rf_buffer_clear(lut_info.trigger_script);
    }
  }
  else
  {
    rsp_pkt->mfg_id = -1;
    rsp_pkt->prd_id = -1;
    rsp_pkt->num_valid_paStates = 0;
    ftm_rsp_data.cmd =  FTM_RSP_BAD_PARM;
    FTM_MSG(FTM_ERROR, " Common Device is null");
  }

  memscpy( &(rsp_pkt->lut_ids_placeholder), (sizeof(uint16)*rsp_pkt->num_valid_paStates) ,
  	         &lut_id_results[0], (sizeof(uint16)*rsp_pkt->num_valid_paStates) );

  /* Clear buffer */
  (void)rf_buffer_destroy(lut_info.tx_gain_script);
  (void)rf_buffer_destroy(lut_info.bbf_script);
  (void)rf_buffer_destroy(lut_info.trigger_script);

  FTM_MSG_4( FTM_HIGH, "ftm_common_get_txlut_info_for_red_cal: Response "
               "mfg_id (%d), prd_id(%d), Num Pa states  %d, pa_1 %d .",
               rsp_pkt->mfg_id, rsp_pkt->prd_id , rsp_pkt->num_valid_paStates, lut_id_results[0]);

  return ftm_rsp_data;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Overrides a bunch of TXAGC parameters. 

  @details
  This function will call the common override API to override the parameters.

  @param *ftm_req_data FTM Request Data

  @return

*/
static
ftm_rsp_pkt_type
ftm_common_set_xpt_txagc_override
(
  void* ftm_req_data,
  ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_xpt_txagc_override_rsp_type *rsp_pkt; /*Packet Payload*/

  /* Extract parameters */
  
  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  ftm_xpt_txagc_override_type *ovr_params = 
             (ftm_xpt_txagc_override_type *)(&((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params);
  #endif

  /* Create a new reponse packet in the ftm heap. */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( 
                              ftm_req_data,
                              sizeof(ftm_xpt_txagc_override_rsp_type));

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_xpt_txagc_override_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_set_xpt_txagc_override: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }
  else
  {
    /* Init to "FAIL" for now. 
       If all goes OK, should be "PASS" at end of API */
    rsp_pkt->status = FALSE;
  }

  /* Perform XPT TxAGC override */
  api_status = ftm_common_xpt_txagc_override( ovr_params,ftm_rf_tech );

  if (rsp_pkt != NULL)
  {
    rsp_pkt->status = api_status;
  }

  if ( api_status == FALSE)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_set_xpt_txagc_override: api status = FAIL" );
  }

  return ftm_rsp_data;
}/* ftm_common_set_xpt_txagc_override */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform RSB and Tx lo leakage calibration

  @details
  This function will call the tech api to perform rsb and txlo leakage cal
  in WCDMA, LTE & TDSCDMA mode of operation. 

  @param *ftm_req_data FTM Request Data

  @return

*/
static
ftm_rsp_pkt_type
ftm_common_do_rsb_txlo_cal
(
  void* ftm_req_data
)
{
  boolean api_status = TRUE;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; 
  ftm_rsb_txlo_rsp_type *rsp_pkt; 
  ftm_rf_technology_type rf_tech = FTM_RF_TECH_UNKNOWN; 

  /* Extract parameters */
  ftm_rsb_txlo_pkt_type *cal_params = 
    (ftm_rsb_txlo_pkt_type *)(&((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params);

  /* Create a new reponse packet in the FTM heap */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt(ftm_req_data,
                                           sizeof(ftm_rsb_txlo_rsp_type));
 
  /* Extract Packet Payload from new response packet, ftm_rsp_data */
  rsp_pkt = (ftm_rsb_txlo_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if (rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_set_xpt_txagc_override: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }

  rf_tech = ftm_phone_mode_to_rf_tech_type(cal_params->curr_band); 
  rsp_pkt->curr_band = cal_params->curr_band; 

  /* Route the rsb/txlo cal command to respective technology */
  switch( rf_tech )
  {
  case FTM_RF_TECH_WCDMA:
    FTM_MSG( FTM_ERROR, "ftm_wcdma_do_rsb_cal:Not supported on BOLT");
    break;

  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
    
    if ((cal_params->device == 0) && (cal_params->div_device == 1))
    {
      rflte_ftm_mc_sm_set_use_fb_path(TRUE);
      api_status = rflte_ftm_calibration_txlo_calibration(cal_params->curr_band,
                                                          RFM_DEVICE_0,
                                                          RFM_DEVICE_1);
      rflte_ftm_mc_sm_set_use_fb_path(FALSE);
    }
    api_status &= rflte_ftm_calibration_rsb_calibration(cal_params->curr_band, cal_params->override_stg, cal_params->device, cal_params->div_device, cal_params->exit_required); 
    #endif
    break;

  case FTM_RF_TECH_TDSCDMA: 
    #ifdef FEATURE_TDSCDMA
    api_status &= ftm_tdscdma_do_rsb_cal(rsp_pkt, cal_params->override_stg); 
    #endif 
    break; 

  default:
    break;
  }

  if ( api_status == FALSE)
  {
    rsp_pkt->status = FALSE; 
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "RSB Cal Failed." );
  }
  else
  {
    rsp_pkt->status = TRUE; 
  }

  return ftm_rsp_data;  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform FBRx RSB calibration

  @details
  This function will call the tech api to perform rsb and txlo leakage cal
  in WCDMA, LTE & TDSCDMA mode of operation. 

  @param *ftm_req_data FTM Request Data

  @return

*/
static
ftm_rsp_pkt_type
ftm_common_do_fbrx_rsb_cal
(
  void* ftm_req_data
)
{
  boolean api_status = TRUE;
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; 
  ftm_rsb_txlo_rsp_type *rsp_pkt; 
  ftm_rf_technology_type rf_tech = FTM_RF_TECH_UNKNOWN; 

  /* Extract parameters */
  ftm_fbrx_rsb_cal_pkt_type *cal_params = 
    (ftm_fbrx_rsb_cal_pkt_type *)(&((ftm_common_pkt_type*)ftm_req_data)->ftm_extended_params);

  /* Create a new reponse packet in the FTM heap */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt(ftm_req_data,
                                           sizeof(ftm_rsb_txlo_rsp_type));
 
  /* Extract Packet Payload from new response packet, ftm_rsp_data */
  rsp_pkt = (ftm_rsb_txlo_rsp_type*)ftm_rsp_data.pkt_payload;

  /* Send back a BAD response packet for failures */
  if (rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "ftm_common_set_xpt_txagc_override: "
             "Response packet is NULL");
    return ftm_rsp_data;
  }

  rf_tech = ftm_phone_mode_to_rf_tech_type(cal_params->curr_band); 
  rsp_pkt->curr_band = cal_params->curr_band; 

  /* Route the rsb/txlo cal command to respective technology */
  switch( rf_tech )
  {
  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
    //TODO: WCDMA FBRX RSB cal
    #endif
    break;

  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
      //api_status &= rflte_ftm_calibration_fbrx_rsb_calibration(cal_params->curr_band, cal_params->override_stg, cal_params->tx_device, cal_params->exit_required); 
    #endif
    break;

  default:
    break;
  }

  if (api_status == FALSE)
  {
    rsp_pkt->status = FALSE; 
   //rsp_pkt->cal_status = FTM_RSB_IQ_CAP_FAILURE;
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "FBRx RSB Cal Failed." );
  }
  else
  {
    rsp_pkt->status = TRUE; 
   //rsp_pkt->cal_status = FTM_INT_CAL_SUCCESS;
  }

  return ftm_rsp_data;  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is used to setup the Feedback Path

  @details
  This function will call the tech specific APIs that setup Feedback Path and is called from ftm_common_dispatch.

  @param *ftm_req_data FTM Request Data , ftm_rf_tech FTM RF Technology

  @return ftm_rsp_pkt_type containing status of Feedback Path setup

*/
ftm_rsp_pkt_type 
ftm_feedback_setup
(
  void* ftm_req_data,
  ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status= FALSE;

  ftm_feedback_setup_req_packet_type *ftm_feedback_setup_req_packet=NULL;

  /*Initialize struct of ftm_cal_feedback_setup_parameter_type to be passed to tech specific dispatches*/
  ftm_cal_feedback_setup_parameter_type ftm_feedback_setup_req_data;
  ftm_rsp_pkt_type ftm_rsp_data;
  ftm_feedback_setup_resp_packet_type *rsp_pkt;

  ftm_feedback_setup_req_packet=(ftm_feedback_setup_req_packet_type*) ftm_req_data;

  /*Allocate memory for the response packet*/
  ftm_rsp_data = ftm_common_malloc_rsp_pkt(ftm_req_data,
                                           sizeof(ftm_feedback_setup_resp_packet_type));
  rsp_pkt = (ftm_feedback_setup_resp_packet_type*)ftm_rsp_data.pkt_payload;
   
   /* Extract individual parameters for setting up feedback path from the request packet*/
  ftm_feedback_setup_req_data.debug_mode_en=(boolean)ftm_feedback_setup_req_packet->debug_mode_en;
  ftm_feedback_setup_req_data.flag=ftm_feedback_setup_req_packet->flag;
  ftm_feedback_setup_req_data.primary_device=(rfm_device_enum_type)ftm_feedback_setup_req_packet->primary_device;
  ftm_feedback_setup_req_data.feedback_device=(rfm_device_enum_type)ftm_feedback_setup_req_packet->feedback_device;
  ftm_feedback_setup_req_data.feedback_setup_action=(ftm_calibration_feedback_setup_action_type)ftm_feedback_setup_req_packet->feedback_setup_action;

  if (ftm_feedback_setup_req_data.feedback_setup_action & FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR ) /*Configure WTR only if FB setup action mask enables it*/
  {
    ftm_feedback_setup_req_data.configure_wtr.enable_mode=ftm_feedback_setup_req_packet->enable_mode_configure_wtr;
    ftm_feedback_setup_req_data.configure_wtr.fb_state=ftm_feedback_setup_req_packet->fb_state;
  }

  if (ftm_feedback_setup_req_data.feedback_setup_action & FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_XPT) /*Configure RXLM for XPT only if FB setup action mask enables it*/
  {
    ftm_feedback_setup_req_data.configure_rxlm_for_xpt.enable_mode=ftm_feedback_setup_req_packet->enable_mode_configure_rxlm_xpt;
  }

  if (ftm_feedback_setup_req_data.feedback_setup_action & FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC) /*Configure RXLM for FB AGC only if FB setup action mask enables it*/
  {
    ftm_feedback_setup_req_data.configure_rxlm_for_fbagc.enable_mode=ftm_feedback_setup_req_packet->enable_mode_configure_rxlm_fbagc;
  }

  switch (ftm_rf_tech)/*Handle Tech specific dispatches*/
  {
  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
      api_status=(boolean)rflte_ftm_calibration_v3_feedback_setup(&ftm_feedback_setup_req_data);
    #endif
    break;

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
        api_status=(boolean)ftm_wcdma_calv3_sweep_feedback_setup(&ftm_feedback_setup_req_data);
      #endif
      break;

  case FTM_RF_TECH_CDMA:
    #ifdef FEATURE_CDMA
      api_status= (boolean)ftm_1x_calibration_v3_feedback_setup(&ftm_feedback_setup_req_data);
    #endif
      break;

  case FTM_RF_TECH_TDSCDMA:
     #ifdef FEATURE_TDSCDMA
     api_status= (boolean)ftm_tdscdma_calv3_sweep_feedback_setup(&ftm_feedback_setup_req_data);
     #endif
     break;

  default:
    api_status=FALSE;
      break;
  }

  if (rsp_pkt!=NULL)
  {
    rsp_pkt->status=api_status;
    if (api_status==FALSE) 
    {
      FTM_MSG( FTM_ERROR, "FTM Feedback Setup API failed" );
    }
  }

  return ftm_rsp_data;
}

#ifdef FEATURE_RF_HAS_QTUNER
/*===========================================================================

FUNCTION ftm_common_process_get_tuner_power

DESCRIPTION
   This function handles requests to set tuner HDET gain

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.
===========================================================================*/


ftm_rsp_pkt_type ftm_common_procss_rl_cmd(ftm_common_pkt_type *cmd_ptr)
{  
  
  ftm_rsp_pkt_type              rsp_pkt;  

  ftm_get_tuner_rl_req_type* req_params = NULL;
  ftm_get_tuner_rl_resp_type* rsp_data = NULL;  

  qfe_antenna_tuner_rl_measurement_data_type driver_cmd_data;

  /* Decrypt cmd */
  req_params = (ftm_get_tuner_rl_req_type*)(&(cmd_ptr->ftm_extended_params));

  /* Create a new reponse packet in the ftm heap. */
  rsp_pkt = ftm_common_malloc_rsp_pkt(cmd_ptr, sizeof(ftm_get_tuner_rl_resp_type));
  rsp_data = (ftm_get_tuner_rl_resp_type*)rsp_pkt.pkt_payload;    
   
  if(req_params != NULL && rsp_data != NULL)      
  {

    void *tuner_obj = NULL;
    tuner_obj = ftm_common_get_tuner_object(RFM_DEVICE_0, RFDEVICE_TUNER);
    if(tuner_obj != NULL)
    {
      /* Setup response packet */
      rsp_data->rl_cmd = req_params->rl_cmd;
      
      switch(req_params->rl_cmd)
      {
        case FTM_QFE_QTUNER_SETUP_RL:
          FTM_MSG( FTM_MED,"FTM_QFE_QTUNER_SETUP_RL");
          
          rfdevice_antenna_tuner_rl_setup(tuner_obj);
          rsp_data->status = TRUE;
          break;
          
        case FTM_QFE_QTUNER_READ_RL:
          FTM_MSG( FTM_MED,"FTM_QFE_QTUNER_READ_RL");

          /* Cmd params */
          driver_cmd_data.req.incident_gain = req_params->incident_gain;
          driver_cmd_data.req.reflected_gain = req_params->reflected_gain;
          driver_cmd_data.req.pass_through_gain = req_params->pass_through_gain;
          driver_cmd_data.req.num_average = req_params->num_average;
          driver_cmd_data.req.delay_us = req_params->delay_us;
          driver_cmd_data.req.tia_range = req_params->rl_gain;
          
          
          if(rfdevice_antenna_tuner_rl_read(tuner_obj, &driver_cmd_data))
          {          
            rsp_data->status = TRUE;
            
             /* Results */
            rsp_data->pass_through_raw = driver_cmd_data.resp.pass_through_raw;
            rsp_data->reflected_raw = driver_cmd_data.resp.reflected_raw;
            rsp_data->incident_raw = driver_cmd_data.resp.incident_raw;

            rsp_data->pass_through_dbm10 = driver_cmd_data.resp.pass_through_dbm10;
            rsp_data->reflected_dbm10 = driver_cmd_data.resp.reflected_dbm10;
            rsp_data->incident_dbm10 = driver_cmd_data.resp.incident_dbm10;

            rsp_data->return_loss_db10 = driver_cmd_data.resp.return_loss_db10;
            rsp_data->return_loss_raw = driver_cmd_data.resp.return_loss_raw;
            rsp_data->mismatched_loss_db10 = driver_cmd_data.resp.mismatched_loss_db10;
            
            FTM_MSG_5( FTM_MED,"FTM_QFE_QTUNER_READ_RL: (%d, %d, %d)dBm*10. Rl %dBm*10 .  ML %dBm*10",
                                                        rsp_data->pass_through_raw,
                                                        rsp_data->reflected_raw,
                                                        rsp_data->incident_raw,
                                                        rsp_data->return_loss_db10,
                                                        rsp_data->mismatched_loss_db10);
          }
          else
          {
            rsp_data->status = FALSE;
             FTM_MSG_5( FTM_MED,"FTM_QFE_QTUNER_READ_RL: FAIL Gain:(%d, %d, %d) Avg: %d Delay %d",
                                                            driver_cmd_data.req.incident_gain,
                                                            driver_cmd_data.req.reflected_gain,
                                                            driver_cmd_data.req.pass_through_gain,
                                                            driver_cmd_data.req.num_average,
                                                            driver_cmd_data.req.delay_us);
          }
          break;
        case FTM_QFE_QTUNER_ENABLE_RL_LOG:          
          break;
        case FTM_QFE_QTUNER_DISABLE_RL_LOG:    
          break;
        default:
          FTM_MSG_1( FTM_ERROR, "ftm_common_procss_rl_cmd: Invalid cmd %d",req_params->rl_cmd); 
          rsp_data->status = FALSE;
          break;
      }     
      
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_procss_rl_cmd: Unable to get tuner obj for cmd %d",
                                                                        req_params->rl_cmd); 
    }

  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_common_procss_rl_cmd: Params bad");
  }

 
  return rsp_pkt;
}


/*===========================================================================

FUNCTION ftm_common_process_get_tuner_power

DESCRIPTION
   This function handles requests to set tuner HDET gain

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.
===========================================================================*/


ftm_rsp_pkt_type ftm_common_process_get_tuner_power(ftm_common_pkt_type *cmd_ptr)
{  
  
  ftm_rsp_pkt_type              rsp_pkt;  

  ftm_get_tuner_power_req_type* power_req_params = NULL;
  ftm_get_tuner_power_resp_type* rsp_data = NULL;
  power_req_params = (ftm_get_tuner_power_req_type*)(&(cmd_ptr->ftm_extended_params));

  /* Create a new reponse packet in the ftm heap. */
  rsp_pkt = ftm_common_malloc_rsp_pkt(cmd_ptr, sizeof(ftm_get_tuner_power_resp_type));
  rsp_data = (ftm_get_tuner_power_resp_type*)rsp_pkt.pkt_payload;    
    
  if(power_req_params != NULL && rsp_data != NULL)      
  {
    uint8 index = 0;
    void *hdet_obj = NULL;
    hdet_obj = ftm_common_get_tuner_object(RFM_DEVICE_0, RFDEVICE_HDET);
    if(hdet_obj != NULL)
    {
      rfdevice_hdet_setup_power_read(hdet_obj, NULL, RFDEVICE_EXECUTE_IMMEDIATE, 
                                     power_req_params->num_avg, power_req_params->wait_time);
      rsp_data->num_hdet_pid = power_req_params->num_hdet_pid;
      FTM_MSG_HIGH("FTM_GET_QFE_HDET_POWER Getting power for %d pid",power_req_params->num_hdet_pid,0,0);
      for(index = 0; index < MAX_TUNER_PID && index < power_req_params->num_hdet_pid; index++)
      {
      
        rsp_data->power_params[index].hdet_pid = power_req_params->hdet_pid[index];
        rsp_data->power_params[index].power = 0;
        rsp_data->power_params[index].power = ftm_common_get_tuner_hdet_power(hdet_obj,power_req_params->hdet_pid[index]);
      }
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_process_get_tuner_power: Unable to get tuner hdet obj for %d pids",
                                                                              power_req_params->num_hdet_pid );
    }
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_common_process_get_tuner_power: Params bad");
  }

  
  return rsp_pkt;
}
#endif /* FEATURE_RF_HAS_QTUNER */
#ifdef FEATURE_RF_HAS_QTUNER


/*===========================================================================

FUNCTION ftm_common_process_set_tuner_gain

DESCRIPTION
   This function handles requests to set tuner HDET gain

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*//* ftm_common_set_xpt_txagc_override */
void ftm_common_process_set_tuner_gain(ftm_common_pkt_type *cmd_ptr)
{  


  ftm_set_tuner_gain_req_type* gain_params = NULL;
  gain_params = (ftm_set_tuner_gain_req_type*)(&(cmd_ptr->ftm_extended_params));
  if(gain_params != NULL)      
  {
    uint8 index = 0;
    void *hdet_obj = NULL;
    hdet_obj = ftm_common_get_tuner_object(RFM_DEVICE_0, RFDEVICE_HDET);
    if(hdet_obj != NULL)
    {      
      FTM_MSG_HIGH("FTM_SET_QFE_HDET_GAIN_STATE Setting gain for %d pid",gain_params->num_hdet_pid,0,0);
      for(index = 0; index < MAX_TUNER_PID && index < gain_params->num_hdet_pid; index++)
      {
        ftm_common_set_tuner_hdet_gain(hdet_obj,
                                       gain_params->gain_params[index].hdet_pid, 
                                       gain_params->gain_params[index].gain_state);
      }
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_process_set_tuner_gain: Unable to get tuner hdet obj for %d pids",
                                                                              gain_params->num_hdet_pid );
    }
  }

}
#endif /* FEATURE_RF_HAS_QTUNER */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RFFE dvice information {mfg_id, prd_id, rev_id} for FTM cmd FTM_GET_RFFE_DEVICE_INFO

  @details
  This function will return RFFE dvice (PA/ASM, etc) information {mfg_id, prd_id, rev_id} based on 
  FTM rf mod and rfm device number

  @param *request
  Request Data Packet

  @return
  Response packet to the diag for FTM cmd FTM_GET_RFFE_DEVICE_INFO
*/
ftm_rsp_pkt_type
ftm_common_get_rffe_device_info
(
  ftm_get_rffe_device_info_request *request
)
{
    /* Response Packets */
    ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
    ftm_get_rffe_device_info_response * rffe_device_info_resp = NULL;
    
    ftm_mode_id_type rf_mode;
    rfm_device_enum_type rfm_device;
    rffe_device_enum_type rffe_device_type;
    uint8 band = 0;
    ftm_rf_technology_type rf_tech;

    ftm_rsp_data = ftmdiag_create_new_pkt(sizeof(ftm_get_rffe_device_info_response));

    /*check if packet creation is successful*/  
    if (NULL == ftm_rsp_data.pkt_payload)
    {
      FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %u", request->ftm_cmd, 0, 0);
      return ftm_rsp_data;
    }
    
    rffe_device_info_resp = (ftm_get_rffe_device_info_response*) ftm_rsp_data.pkt_payload;
    /*initialize the response packet*/
    memset(rffe_device_info_resp, 0, sizeof(ftm_get_rffe_device_info_response));
    
    /*Copy data from request to response*/
    rffe_device_info_resp->cmd = request->cmd;
    rffe_device_info_resp->sub_sys = request->sub_sys;
    rffe_device_info_resp->mode = request->mode;
    rffe_device_info_resp->ftm_cmd = request->ftm_cmd;

    
    /*extract rf_mode, rfm device, rffe device info from the request packet*/  
    rf_mode = request->rf_mode;
    rfm_device = request->rfm_device;
    rffe_device_type = request->rffe_device_type; 
    

         FTM_MSG_3( FTM_HIGH, "rf mode: %d, rfm_device: %d, rffe_device_type: %d", rf_mode, rfm_device, rffe_device_type);
    
    /*validate pass in parameters*/
    if((rf_mode < FTM_PHONE_MODE_CDMA ) || (rf_mode >= FTM_PHONE_MODE_MAX))
    {
      FTM_MSG_1( FTM_ERROR,"rf mode not valid, mode %d ", rf_mode);
      rffe_device_info_resp->status = 0x1;
      return ftm_rsp_data;
    }
    
    if( (rfm_device < RFM_DEVICE_0) || (rfm_device >= RFM_MAX_DEVICES) )
    {
      FTM_MSG_1( FTM_ERROR,"rfm device not valid, device %d ", rfm_device);
      rffe_device_info_resp->status = 0x1;
      return ftm_rsp_data;
    }

    /*so far, driver  only support querying PA. The support for other RFFE devices will be added when it's needed*/
    if( (rffe_device_type < RFFE_DEVICE_PA) && (rffe_device_type > RFFE_DEVICE_PAPM)) 
    {
      FTM_MSG_1( FTM_ERROR,"rffe device type %d is not valid", rffe_device_type);     
      rffe_device_info_resp->status = 0x1;
      return ftm_rsp_data;
    }

    /*get band and rf tech from rf mode id*/
    if(ftm_common_get_band_tech_from_rf_mode(rf_mode, &band, &rf_tech) == FALSE)
    {
      FTM_MSG( FTM_ERROR,"get band and tech from rf mode failed");
      rffe_device_info_resp->status = 0x1;
      return ftm_rsp_data;  
    }

    switch(rf_tech)
    {
        case FTM_RF_TECH_CDMA:
            {
#ifdef FEATURE_CDMA1X
                if (rffe_device_type == RFFE_DEVICE_PA)
                {
                    if( FALSE == rfc_cdma_get_rffe_device_info(rfm_device, band, RFDEVICE_PA, 
                                                        rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                        rffe_device_info_resp->rev_id))
                    
                    {
                        FTM_MSG( FTM_ERROR,"rfc_cdma_get_pa_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
                if (rffe_device_type == RFFE_DEVICE_ASM)
                {
                    if( FALSE == rfc_cdma_get_rffe_device_info(rfm_device, band, RFDEVICE_ASM, 
                                                        rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                        rffe_device_info_resp->rev_id))
                    
                    {
                        FTM_MSG( FTM_ERROR,"rfc_cdma_get_pa_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
                if (rffe_device_type == RFFE_DEVICE_PAPM)
                {
                    if( FALSE == rfc_cdma_get_rffe_device_info(rfm_device, band, RFDEVICE_PAPM, 
                                                        rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                        rffe_device_info_resp->rev_id))
                    
                    {
                        FTM_MSG( FTM_ERROR,"rfc_cdma_get_papm_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }               
#endif
            }
            
            break;

        case FTM_RF_TECH_GSM:
            {
#ifdef FEATURE_GSM      
                    if (rffe_device_type == RFFE_DEVICE_PA)
                    {
                        if( FALSE == rfc_gsm_get_rffe_device_info(rfm_device, band, RFDEVICE_PA,
                                                            rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                            rffe_device_info_resp->rev_id))
                        
                        {
                            FTM_MSG( FTM_ERROR,"rfc_gsm_get_pa_device_info failed");
                            rffe_device_info_resp->status = 0x1;
                        }
                    }
                    else if(rffe_device_type == RFFE_DEVICE_ASM)
                    {
                        if( FALSE == rfc_gsm_get_rffe_device_info(rfm_device, band, RFDEVICE_ASM,
                                                            rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                            rffe_device_info_resp->rev_id))
                        {
                            FTM_MSG( FTM_ERROR,"rfc_gsm_get_rffe_device_info failed");
                            rffe_device_info_resp->status = 0x1;
                        }
                    }
                    else if(rffe_device_type == RFFE_DEVICE_PAPM)
                    {
                        if( FALSE == rfc_gsm_get_rffe_device_info(rfm_device, band, RFDEVICE_PAPM,
                                                            rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                            rffe_device_info_resp->rev_id))
                        {
                            FTM_MSG( FTM_ERROR,"rfc_gsm_get_rffe_device_info failed");
                            rffe_device_info_resp->status = 0x1;
                        }
                    }
#endif              
            }
                
            break;


        case FTM_RF_TECH_WCDMA:
            {
#ifdef FEATURE_WCDMA
                /*right now we only support query PA device. The support for other devices can be added later on*/
                if (rffe_device_type == RFFE_DEVICE_PA)
                {
                    if( FALSE == rfdevice_wcdma_pa_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))
                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_wcdma_pa_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
                else if(rffe_device_type == RFFE_DEVICE_ASM)
                {
                    if( FALSE == rfdevice_wcdma_asm_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_wcdma_asm_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
                else if(rffe_device_type == RFFE_DEVICE_PAPM)
                {
                    if( FALSE == rfdevice_wcdma_papm_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_wcdma_papm_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }                   
#endif      
            }
            
            break;

        case FTM_RF_TECH_LTE:
            {
#ifdef FEATURE_LTE  
                /*right now we only support query PA device. The support for other devices can be added later on*/
                if (rffe_device_type == RFFE_DEVICE_PA)
                {
                    if( FALSE == rfdevice_lte_pa_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))
                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_lte_pa_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
                else if(rffe_device_type == RFFE_DEVICE_ASM)
                {
                    if( FALSE == rfdevice_lte_asm_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_lte_asm_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
                else if(rffe_device_type == RFFE_DEVICE_PAPM)
                {
                    if( FALSE == rfdevice_lte_papm_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_lte_papm_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
#endif              
            }

            break;

        case FTM_RF_TECH_TDSCDMA:
            {
#ifdef FTM_HAS_TDSCDMA
                /*right now we only support query PA device. The support for other devices can be added later on*/
                if (rffe_device_type == RFFE_DEVICE_PA)
                {
                    if( FALSE == rfdevice_tdscdma_pa_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_tdscdma_pa_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }               
                else if(rffe_device_type == RFFE_DEVICE_ASM)
                {
                    if( FALSE == rfdevice_tdscdma_asm_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_tdscdma_asm_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }               
                else if(rffe_device_type == RFFE_DEVICE_PAPM)
                {
                    if( FALSE == rfdevice_tdscdma_papm_get_device_info(rfm_device, band, 
                                                    rffe_device_info_resp->mfg_id, rffe_device_info_resp->prd_id, 
                                                    rffe_device_info_resp->rev_id, RFC_COMMON_MAX_DEVICE_INSTANCES_PER_BAND))

                    {
                        FTM_MSG( FTM_ERROR,"rfdevice_tdscdma_papm_get_device_info failed");
                        rffe_device_info_resp->status = 0x1;
                    }
                }
#endif              
            }

            break;

        default:
            FTM_MSG( FTM_ERROR,"unknown rf technology");    
            
    }
        
    /* Return Response Packet */
    return ftm_rsp_data;
}

/*===========================================================================

FUNCTION FTM_COMMON_SET_RF_SCENARIO

DESCRIPTION
   This routine updates the current FTM_RF scenario. The current supported scenarios are 
   0           - default 
   1           - multi-tech concurrency

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_common_set_rf_scenario(uint32 ftm_rf_scenario)
{
  uint32 ftm_rf_previous_test_scenario =0;

  /* Back up the old state for debug purpose*/
  ftm_rf_previous_test_scenario= ftm_rf_current_test_scenario;

  ftm_rf_current_test_scenario = ftm_rf_scenario;

  FTM_MSG_2(FTM_HIGH, "ftm_common_set_rf_scenario: Previous state: %d, Current state: %d",
                                             ftm_rf_previous_test_scenario, ftm_rf_current_test_scenario);
}

/*===========================================================================

FUNCTION FTM_COMMON_DISPATCH

DESCRIPTION
   This function handles requests to run common tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_dispatch(ftm_common_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_composite_cmd_header_type  rsp_headers;
   
  /* Request packet for the ftm_activate_protocol */
  ftm_activate_pkt_type *act_req_pkt; 
   /* Request packet for the ftm_deactivate_protocol */
  ftm_deactivate_pkt_type *deact_req_pkt; 

  /* local_buffer and extra_data_len allow return data size to depend on a 
     request parameter. Unused for now */
  byte                           *local_buffer  = NULL;
  uint16                         extra_data_len = 0; 
  uint32                         enh_therm_val  = 0;
#ifdef FEATURE_RF_HAS_QTUNER
  boolean ret_value = FALSE;
  uint8 qfe_ant_device = 0;
  uint16 qtuner_return =0;
  uint32 tx_freq =0;
  rfm_cdma_band_class_type band;
  #ifdef FEATURE_CDMA
  const rf_cdma_data_status_type *dev_status_r;
  #endif
  const rfc_common_logical_device_params_type* cmn_logical_dev = NULL;
  void *hdet_obj = NULL;

  uint8 qfe_scenario = 0;
  uint32 ftm_rf_scenario =0;
  uint8 scenario_vals[1];
#endif
  uint8 pa_index = 0;
  uint16 pwr_index = 0;
  uint8 ch_index = 0;
  boolean therm_read_error = FALSE;
  boolean flag = FALSE;
  ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;
#ifdef FEATURE_RF_HAS_QTUNER
  rfm_mode_enum_type current_mode = RFM_INVALID_MODE;
#endif
#if defined ( FEATURE_CDMA1X )
  const rfc_cdma_logical_device_params_type *logical_dev0; /* Logical Device */
#endif
  ftm_common_result_type         result = {(uint16)FTM_FAILURE};   /* Initialize result to Fail*/
  boolean rsp_pkt_created = FALSE;
  uint16                         fixed_data_len = 0; 
#if defined ( FEATURE_CDMA1X )
  rfdevice_therm_read_type       therm_read;
#endif

#if defined (FEATURE_XO_FACTORY_CAL)
#ifndef FEATURE_XO
  byte ftm_enh_xo_therm_type;
  uint32 ftm_enh_xo_time;
  tcxomgr_temp_set_type ftm_enh_xo_temp;
#endif
#endif

  /*pointer for command FTM_GET_RFFE_DEVICE_INFO*/  
  ftm_get_rffe_device_info_request * get_rffe_req;
  
  /* Initialize response packet size to length of the common headers */
  /* the size of the return parameters will be added in the dispatch cases below */
  rsp_headers.ftm_hdr.cmd_rsp_pkt_size = sizeof(ftm_composite_cmd_header_type);

  if (cmd_ptr != NULL)
  {
    switch (cmd_ptr->ftm_hdr.cmd_id)
    {
    case FTM_RF_SET_SCENARIO:
      ftm_rf_scenario = *((uint32*) &(cmd_ptr->ftm_extended_params));
      ftm_common_set_rf_scenario(ftm_rf_scenario);
      rsp_pkt_created = TRUE;
      break;

    case FTM_RXLM_EXECUTE_REG_DUMP:
      rxlm_log();
      rsp_pkt_created = TRUE;
      break;

    case FTM_TXLM_EXECUTE_REG_DUMP: 
      txlm_log();
      rsp_pkt_created = TRUE;
      break;

    case FTM_RXLM_ALLOCATE_BUFFER:
      rsp_pkt = rxlm_allocate_buffer_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_RXLM_DEALLOCATE_BUFFER:
      rsp_pkt = rxlm_deallocate_buffer_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_RXLM_UPDATE_STATIC_SETTINGS: 
      rsp_pkt = rxlm_update_static_settings_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_RXLM_UPDATE_DYNAMIC_SETTINGS:
      rsp_pkt = rxlm_update_dynamic_settings_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_RXLM_CONFIGURE_CHAIN:
      rsp_pkt = rflm_dtr_rx_configure_chain_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_TXLM_ALLOCATE_BUFFER:
      txlm_allocate_buffer_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_TXLM_DEALLOCATE_BUFFER:
      txlm_deallocate_buffer_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_TXLM_UPDATE_STATIC_SETTINGS: 
      txlm_update_static_settings_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_TXLM_UPDATE_DYNAMIC_SETTINGS:
      txlm_update_dynamic_settings_dispatch(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

//  case FTM_TXLM_CONFIGURE_CHAIN:
//    txlm_update_dynamic_settings_dispatch(cmd_ptr);
//    rsp_pkt_created = TRUE;
//    break;

    case FTM_CONFIGURE_BRIDGE:
        configure_rxfe_bridge_dispatch(cmd_ptr);
        rsp_pkt_created = TRUE;
        break;

        /*Activate Chain*/
    case FTM_STREAM_DATA:
        stream_data_dispatch(cmd_ptr);
        rsp_pkt_created = TRUE;
        break;

    case FTM_RX_WAIT_DBM: 
      rsp_pkt = ftm_common_rx_wait_dbm(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_RX_WAIT_DBM_V2: 
      rsp_pkt = ftm_common_rx_wait_dbm_v2(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    case FTM_TX_RX_FREQ_CAL_SWEEP: 
      FTM_MSG_1( FTM_ERROR, "ftm_cdma_dispatch: CompositeCal Command %d "
                            "Deprecated", cmd_ptr->ftm_hdr.cmd_id );
        break;

    case FTM_LOAD_RF_NV:
      if ( rfm_reload_nv(rex_self(), FS_OP_COMPLETE_SIG, (void*) rex_wait) )
      {
        result.error_code = FTM_SUCCESS;
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

    case FTM_DO_SELF_CAL:
      if (ftm_common_self_calibration())
      {
        result.error_code = FTM_SUCCESS;
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

    case FTM_GET_EFS_VERIFICATION_DATA:

      FTM_MSG(FTM_ERROR," For testing - FTM_GET_EFS_VERIFICATION_DATA ");
      if (ftm_common_get_efs_verifcation_data())
      {
        result.error_code = FTM_SUCCESS;
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

      /* This is a temporary command added for NOK to run tuner cal only-hsinih*/
    case FTM_DO_TUNER_SELF_CAL:
      if (ftm_common_tuner_self_calibration())
      {
        result.error_code = FTM_SUCCESS;
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

      /* This is a temporary command added for customers who are facing DAC Cal issues
         and need to removed later */
    case FTM_DO_DAC_CAL:

      flag = ftm_common_resource_config_request(TRUE);
      if (flag == TRUE)
      {
        flag = ftm_common_dac_calibration();
        (void)ftm_common_resource_config_request(FALSE);
      }

      if (flag == TRUE)
      {
        result.error_code = FTM_SUCCESS;
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;


    case FTM_GET_AGC_RESOLUTION_RX:  
      result.agc_result.min_power = (int16)FTM_RX_MIN_POWER;
      result.agc_result.resolution = (uint16)FTM_RX_RESOLUTION;
      result.agc_result.dynamic_range = (uint16)FTM_RX_DYNAMIC_RANGE;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_agc_response_type);
      break;

    case FTM_GET_AGC_RESOLUTION_TX:
      result.agc_result.min_power = (int16)FTM_TX_MIN_POWER;
      result.agc_result.resolution = (uint16)FTM_TX_RESOLUTION;
      result.agc_result.dynamic_range = (uint16)FTM_TX_DYNAMIC_RANGE;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_agc_response_type);
      break;

  #if defined(FTM_HAS_UMTS)||defined(FEATURE_GSM)
    case FTM_GET_THERM: 
      {
        /*
        * therm_val should be an 8-bit integer since the FTM_GET_THERM API expects *
        * only 1byte of therm data.  Sending more than 1 byte breaks therm read    *
        * functionality.  Downcast rfi_read_therm_raw result from uint16 to uint8  *
        */
        uint8 therm_val = 0;
        // txdevice[RF_PATH_0]->cmd_dispatch_fp(RF_PATH_0,RFDEVICE_GET_THERMISTER_VALUE, &therm_val); /* Todo: To be replaced */
        extra_data_len = sizeof(therm_val);

        /* Malloc the local buffer according to extra_data_len*/
        if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
        {
          memscpy(local_buffer, extra_data_len,(char *)&therm_val, extra_data_len);
          result.error_code = 0;
        }
        else
        {
          result.error_code = 1;
        }
      }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

    case FTM_GET_ENH_THERM: 
      
      (void)ftm_common_resource_config_request(TRUE);

      ftm_rf_tech = ftm_get_rf_technology();

      switch (ftm_rf_tech)
      {
      case FTM_RF_TECH_CDMA:
#if defined ( FEATURE_CDMA1X )
        /* Get Logical Device 0 Param */
        logical_dev0 = rfc_cdma_get_logical_device_params( RFM_DEVICE_0 );

        if (logical_dev0 == NULL)
        {
          FTM_MSG(FTM_ERROR,"ftm_common_dispatch - logical_dev0 is NULL ");
          (void)ftm_common_resource_config_request(FALSE);
          return rsp_pkt;
        }
        therm_read.temperature = 0;
        therm_read.valid_temp_read = FALSE;
        flag = FALSE;
        rf_common_enter_critical_section(rf_cdma_get_lock_data());

        /*Make sure TCXO Buffer is turned on before therm read*/
        if (!(rfcommon_get_dev_power_voting_status(RFM_DEVICE_0)))
        {
          rfcommon_core_dev_power_vote(RFM_DEVICE_0,RFCMN_CORE_DEV_ON,RFCMN_CORE_DEV_NORMAL_VOTE);
          flag = TRUE;
        }

        rfdevice_cdma_tx_cmd_dispatch( logical_dev0->
                                       tx_device[rf_cdma_data_get_curr_band(RFM_DEVICE_0)],
                                       RFDEVICE_GET_THERMISTER_VALUE,
                                       &therm_read );
        /*Turn OFF TCXO Buffer if turned ON*/ 
        if (flag == TRUE)
        {
          rfcommon_core_dev_power_vote(RFM_DEVICE_0,RFCMN_CORE_DEV_OFF,RFCMN_CORE_DEV_NORMAL_VOTE);
        }
        rf_common_leave_critical_section(rf_cdma_get_lock_data());       
        if (therm_read.valid_temp_read == TRUE)
        {
          enh_therm_val = therm_read.temperature;
        }
        else
        {
          therm_read_error = TRUE;
        }
#else
        therm_read_error = TRUE;
#endif
        break;

      case FTM_RF_TECH_WCDMA:
#if defined ( FEATURE_WCDMA)
        rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,ftm_curr_wcdma_mode,RFDEVICE_GET_THERMISTER_VALUE, &enh_therm_val);
#else
        therm_read_error = TRUE;
#endif
        break;

      case FTM_RF_TECH_GSM:
#if defined ( FEATURE_GSM)
        (uint32)rfdevice_gsm_cmd_dispatch(RFM_DEVICE_0,RF_PATH_0,RFDEVICE_GET_THERMISTER_VALUE, &enh_therm_val);
#else
        therm_read_error = TRUE;
#endif
        break;

      default:
        therm_read_error = TRUE;
        break;
      }

      if (therm_read_error == TRUE)
      {
        enh_therm_val = 0;
      }
      //(void)txdevice[RF_PATH_0]->cmd_dispatch_fp(RF_PATH_0,RFDEVICE_GET_THERMISTER_VALUE, &enh_therm_val);
      extra_data_len = sizeof(enh_therm_val);

      /* Malloc the local buffer according to extra_data_len*/
      if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
      {
        memscpy(local_buffer, extra_data_len,(char *)&enh_therm_val, extra_data_len);
        result.error_code = 0;
      }
      else
      {
        result.error_code = 1;
      }
      if (therm_read_error == TRUE)
      {
        result.error_code = FTM_FAILURE;
      }
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

      (void)ftm_common_resource_config_request(FALSE);
      break;

  #endif

    case FTM_MULTIMODE_HANDOVER:
      {
        //ftm_multimode_handover_param_type *param_ptr;
        //param_ptr = (ftm_multimode_handover_param_type*)&(cmd_ptr->ftm_extended_params);
        //rfm_multimode_handover( (rf_multimode_handover_cmd_type)param_ptr->chain_0_cmd,
        //                        (rf_multimode_handover_cmd_type)param_ptr->chain_1_cmd );
        /*todo - need to implement this function/find equivalent function*/
        result.error_code = 0;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
        break;
      }

#if defined (FEATURE_XO_FACTORY_CAL)
    case FTM_DO_ENH_XO_DC_CAL:
    case FTM_DO_ENH_XO_DC_CAL_V2:
    case FTM_DO_ENH_XO_FT_CURVE_CAL:
    case FTM_DO_ENH_XO_FT_CURVE_CAL_V2:
    case FTM_DO_ENH_XO_FT_COLLECT_SAMPLE:
    case FTM_DO_ENH_XO_FT_COLLECT_SAMPLE_V2:
      FTM_MSG(FTM_ERROR,"ftm_common_dispatch - Command not supported");
      result.error_code = FTM_CMD_DOES_NOT_APPLY_TO_TARGET;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
          break;

    case FTM_XO_CALIBRATION:
      rsp_pkt = ftm_xo_calibration( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;
#endif

#if defined (FEATURE_CDMA1X) || defined (FEATURE_TDSCDMA)
    case FTM_SET_CALIBRATION_STATE:
      /* This command is presently being sent before sending the FTM_SET_BLOCK_MASK command.
     If at any time in the future, the functionality of this command is changed to access
     any part of the NV, then it should be noted that the NV will be OVERWRITTEN when the 
     FTM_SET_BLOCK_MASK command is sent after this command. Hence precaution should be taken
     in this regard. */
      {
        int ftm_cal_state = (int)(cmd_ptr->ftm_extended_params) ;

        rfm_set_calibration_state( (boolean)ftm_cal_state );
        result.error_code = FTM_SUCCESS;
      }
      result.error_code = 0;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);

      (void)ftm_common_fbrx_set_cal_state(TRUE);

      break;
#endif

    case FTM_SET_TX_ENH_CAL_ACTION:      
      enh_cal_action = *((byte*) &(cmd_ptr->ftm_extended_params));
      FTM_MSG_MED("FTM_SET_Tx_ENH_CAL_ACTION enh_cal_action=  %d",enh_cal_action,0,0);
      result.error_code = FTM_SUCCESS;
      /* Get the current technology - 1x, WCDMA or GSM */
      ftm_rf_tech = ftm_get_rf_technology();

      switch (enh_cal_action)
      {
      /* GST enum is currently used by tools to mean APT Disabled */       
      case ENH_TX_CAL_ACTION_GST:
        #ifdef FEATURE_WCDMA
        if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
        {
          ftm_wcdma_set_smps_pa_bias_override(TRUE);                 
        }
        #endif

            #ifdef FEATURE_CDMA1X
        if (ftm_rf_tech == FTM_RF_TECH_CDMA)
        {
          //ftm_1x_set_smps_pa_bias_override(TRUE);                 
        }
        #endif


        #ifdef FEATURE_LTE
        if (ftm_rf_tech == FTM_RF_TECH_LTE)
        {
        }
        #endif
        break;

      case ENH_TX_CAL_ACTION_APT:
        #ifdef FEATURE_WCDMA
        if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
        {
          ftm_wcdma_set_smps_pa_bias_override(FALSE);                
        }
        #endif

            #ifdef FEATURE_CDMA1X
        if (ftm_rf_tech == FTM_RF_TECH_CDMA)
        {
          //ftm_1x_set_smps_pa_bias_override(FALSE);                 
        }
        #endif

        #ifdef FEATURE_LTE
        if (ftm_rf_tech == FTM_RF_TECH_LTE)
        {
        }
        #endif
        break;

      default:
        result.error_code = FTM_FAILURE;
        break;
      }   
      break;

    case FTM_LOAD_TX_LINEARIZER_CAL_DATA:
      req_load_tx_lin = (ftm_load_tx_lin_cal_data_request *) cmd_ptr;
      FTM_MSG_MED("In FTM_LOAD_TX_LINEARIZER_CAL_DATA ",0,0,0);
      FTM_MSG_MED("Ref Chan: %d",req_load_tx_lin->ref_chan,0,0);
      MSG_4(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Pa Ranges: %d %d %d %d", req_load_tx_lin->pa_ranges[0],
            req_load_tx_lin->pa_ranges[1], req_load_tx_lin->pa_ranges[2], req_load_tx_lin->pa_ranges[3] );
      MSG_4(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Num Power List: %d %d %d %d", req_load_tx_lin->num_pwr_list[0],
            req_load_tx_lin->num_pwr_list[1], req_load_tx_lin->num_pwr_list[2], req_load_tx_lin->num_pwr_list[3] );
      for (pa_index = 0; pa_index < MAX_PA_RANGES; ++pa_index)
      {
        ftm_apt_tx_lin[pa_index].pa_range = req_load_tx_lin->pa_ranges[pa_index];
        ftm_apt_tx_lin[pa_index].num_pwr_list = req_load_tx_lin->num_pwr_list[pa_index];
      #ifdef FEATURE_CDMA1X
//      ftm_pwr_pdm_freq[pa_index].pa_range = req_load_tx_lin->pa_ranges[pa_index];
//      ftm_pwr_pdm_freq[pa_index].num_pwr_list = req_load_tx_lin->num_pwr_list[pa_index];
    #endif

        for (pwr_index = 0 ; pwr_index < MAX_TX_LIN_PWR_LIST_SIZE; ++ pwr_index)
        {
        #ifdef FEATURE_CDMA1X
          if ( pwr_index >= req_load_tx_lin->num_pwr_list[pa_index] )
          {
//           ftm_pwr_pdm_freq[pa_index].pwr_list[0][pwr_index] = (int16)-32767;
          }
          else
          {
//          ftm_pwr_pdm_freq[pa_index].pwr_list[0][pwr_index] = req_load_tx_lin->tx_lin_pwr_list[pa_index][pwr_index];
//          ftm_pwr_pdm_freq[pa_index].lut_index_list[pwr_index] = (uint16)req_load_tx_lin->tx_lin_pdm_list[pa_index][pwr_index];
          }
    #endif

          FTM_MSG_ERROR("TxLin[%d][%d]: %d",pa_index,pwr_index,req_load_tx_lin->tx_lin_pwr_list[pa_index][pwr_index]);
          ftm_apt_tx_lin[pa_index].tx_lin_pwr_list[pwr_index] = req_load_tx_lin->tx_lin_pwr_list[pa_index][pwr_index];
          ftm_apt_tx_lin[pa_index].tx_lin_pdm_list[pwr_index] = (uint16)req_load_tx_lin->tx_lin_pdm_list[pa_index][pwr_index];
        }
      #ifdef FEATURE_CDMA1X
//      ftm_pwr_pdm_freq[pa_index].pout_data_type = LINEARIZER;
    #endif
      }
      /* Get the current technology - 1x, WCDMA or GSM */
      ftm_rf_tech = ftm_get_rf_technology();    

    #ifdef FEATURE_WCDMA
      if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
      {
        /* Not supported for WCDMA */
      }
    #endif

    #ifdef FEATURE_LTE
      if (ftm_rf_tech == FTM_RF_TECH_LTE)
      {
      }
    #endif

    #ifdef FEATURE_CDMA1X
      if (ftm_rf_tech == FTM_RF_TECH_CDMA)
      {
        //ftm_1x_write_reference_sweep_to_nv();
      }
    #endif
      break;

    case FTM_LOAD_TX_FREQCOMP_CAL_DATA:
      req_load_tx_fcomp = (ftm_load_tx_fcomp_cal_data_request * ) cmd_ptr;
      FTM_MSG_ERROR("In FTM_LOAD_TX_FREQCOMP_CAL_DATA ",0,0,0);
      FTM_MSG_ERROR("Number of Cal Chans: %d",req_load_tx_fcomp->num_freqcomp_cal_chan,0,0);
      MSG_4(MSG_SSID_FTM, MSG_LEGACY_ERROR,"Pa Ranges: %d %d %d %d", req_load_tx_fcomp->pa_ranges[0],
            req_load_tx_fcomp->pa_ranges[1], req_load_tx_fcomp->pa_ranges[2], req_load_tx_fcomp->pa_ranges[3] );
      for (ch_index = 0 ; ch_index < MAX_CHAN_LIST_SIZE; ++ ch_index)
      {
        FTM_MSG_ERROR("FreqComp_List[%d]: %d",ch_index,req_load_tx_fcomp->chan_list[ch_index],0);
      }
      for (pa_index = 0; pa_index < MAX_PA_RANGES; ++pa_index)
      {
        ftm_apt_fcomp[pa_index].num_freqs = MAX_CHAN_LIST_SIZE;
        ftm_apt_fcomp[pa_index].pa_range = req_load_tx_fcomp->pa_ranges[pa_index];      
      #ifdef FEATURE_CDMA1X
//      ftm_pwr_pdm_freq[pa_index].pa_range = req_load_tx_lin->pa_ranges[pa_index];
//      ftm_pwr_pdm_freq[pa_index].pout_data_type = FCOMP;
    #endif
        //ftm_pwr_pdm_freq[pa_index].num_pwr_list = MAX_CHAN_LIST_SIZE;

        for (ch_index = 0 ; ch_index < MAX_CHAN_LIST_SIZE; ++ ch_index)
        {
          ftm_apt_fcomp[pa_index].fcomp_pwr_list[ch_index] = req_load_tx_fcomp->tx_fcomp_pwr_list[pa_index][ch_index];
          ftm_apt_fcomp[pa_index].fcomp_pdm_list[ch_index] = (uint16)req_load_tx_fcomp->tx_fcomp_pdm_list[pa_index];
//          ftm_pwr_pdm_freq[pa_index].freq_list[ch_index] =  req_load_tx_fcomp->chan_list[ch_index];
//          ftm_pwr_pdm_freq[pa_index].pwr_list[ch_index][0] = req_load_tx_fcomp->tx_fcomp_pwr_list[pa_index][ch_index];
          FTM_MSG_ERROR("TxFreqComp[%d][%d]: %d",pa_index,ch_index,req_load_tx_fcomp->tx_fcomp_pwr_list[pa_index][ch_index]);
        }
      }

      /* Get the current technology - 1x, WCDMA or GSM */
      ftm_rf_tech = ftm_get_rf_technology();    

    #ifdef FEATURE_WCDMA
      if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
      {
        /* Not supported for WCDMA */
      }
    #endif
      break;

    case FTM_CALIBRATION_SWEEP:
      rsp_pkt = ftm_calibration_configure_sweep( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;

    case FTM_CALIBRATION_SWEEP_COMPRESSED:
      rsp_pkt = ftm_calibration_configure_sweep_compressed( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;

    case FTM_INT_CALIBRATION_SWEEP:
      (void)ftm_calibration_configure_sweep( cmd_ptr );
      rsp_pkt = ftm_common_create_rsp_pkt(FTM_COMMON_NO_RSP);
      rsp_pkt_created = TRUE;
      break;

    case FTM_CALIBRATION_SWEEP_GET_RESULTS:
      rsp_pkt = ftm_calibration_fetch_sweep_results( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;

    case FTM_INT_CALIBRATION_SWEEP_GET_RESULTS:
      FTM_MSG(FTM_ERROR,"ftm_common_dispatch - Command not supported");
      result.error_code = FTM_CMD_DOES_NOT_APPLY_TO_TARGET;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

    case FTM_INT_PERFORM_INT_DEV_CAL:
      FTM_MSG(FTM_ERROR,"ftm_common_dispatch - Command not supported");
      result.error_code = FTM_CMD_DOES_NOT_APPLY_TO_TARGET;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;

    case FTM_INT_CALV3_DATA_PROCESS_MLINE:
      (void)ftm_calibration_data_process_mline_calc_instr( cmd_ptr );
      rsp_pkt = ftm_common_create_rsp_pkt(FTM_COMMON_NO_RSP);
      rsp_pkt_created = TRUE;
      break;

    case FTM_INT_CALV3_RSB_CAL:
	  (void) ftm_enh_internal_device_calibration_process_rsb_instr(cmd_ptr);
      rsp_pkt = ftm_common_create_rsp_pkt(FTM_COMMON_NO_RSP);
      rsp_pkt_created = TRUE;
      break;
	  
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
    case FTM_GET_UNIFIED_ENH_THERM:
      rsp_pkt = ftm_common_get_thermistor_read( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;

    case FTM_ANT_TUNER_OVERWRITE_CMD:
      {
        FTM_MSG(FTM_ERROR,"FTM_ANT_TUNER_OVERWRITE_CMD  is not supported");   
      }
      break;
#endif /*FEATURE_GNSS_ONLY_NO_WWAN */

    case FTM_ACTIVATE_FTM_MODE:
      MSG_1(MSG_SSID_FTM, MSG_LVL_ERROR,"Recevied Cmd From MMOC task for activating FTM ", 0);
      act_req_pkt = (ftm_activate_pkt_type *)cmd_ptr;//CMD to MMOC->FTM TASK

      ftm_activate_protocol_cmd( act_req_pkt->caller_tcb_ptr, 
                                 act_req_pkt->task_nv_wait_sig, 
                                 act_req_pkt->act_reason,
                                 act_req_pkt->trans_id );
                                 
      rsp_pkt = ftm_common_create_rsp_pkt(FTM_COMMON_NO_RSP);
      rsp_pkt_created = TRUE;
      break;

    case FTM_DEACTIVATE_FTM_MODE:
      MSG_1(MSG_SSID_FTM, MSG_LVL_ERROR,"Recevied Cmd From MMOC task for de-activating FTM ", 0);
      deact_req_pkt = (ftm_deactivate_pkt_type *)cmd_ptr;//CMD to MMOC->FTM TASK

      ftm_deactivate_protocol_cmd( deact_req_pkt->caller_tcb_ptr, 
                                   deact_req_pkt->task_nv_wait_sig, 
                                   deact_req_pkt->deact_reason,
                                   deact_req_pkt->trans_id );

      rsp_pkt = ftm_common_create_rsp_pkt(FTM_COMMON_NO_RSP);
      rsp_pkt_created = TRUE;
      break;



    #ifdef FTM_ERROR_FATAL_FEATURE_DEFINED
    case FTM_ERR_FATAL_COMMAND:    
      ERR_FATAL(" FTM ERROR FATAL Command received!", 0, 0, 0);
      break;
    #endif
#ifndef FEATURE_XO
    case FTM_GET_ENH_XO_THERM:
      {
        ftm_enh_xo_therm_type = *((byte*) &(cmd_ptr->ftm_extended_params));

        if (ftm_enh_xo_therm_type >= FTM_ENH_XO_THERM_MAX) // Requested xo_therm_type is not valid
        {
          enh_therm_val = 0x0;
          result.error_code = FTM_BAD_PARAM;
          FTM_MSG_ERROR("FTM DEBUG: XO_THERM_TYPE requested is invalid",0,0,0);
        }
        else
        {
          tcxomgr_get_time_and_temp (&ftm_enh_xo_time, &ftm_enh_xo_temp);

          switch (ftm_enh_xo_therm_type)
          {
          case FTM_ENH_XO_THERM_READ_XO:
            enh_therm_val = ftm_enh_xo_temp.xo;
            break;
          case FTM_ENH_XO_THERM_READ_PMIC:
            enh_therm_val = ftm_enh_xo_temp.pmic;
            break;
          case FTM_ENH_XO_THERM_READ_PA:
            enh_therm_val = ftm_enh_xo_temp.pa;
            break;
          default:
            // Invalid input. This situation should be handled by the if() statement above, which checks the value of ftm_enh_xo_therm_type
            enh_therm_val = 0x0;
            break;
          }
          result.error_code = FTM_SUCCESS;

          if (enh_therm_val == 0x80000000) // 0x80000000 indicates an INVALID therm read
          {
            enh_therm_val = 0x0;
            result.error_code = FTM_DATA_INVALID;
          }
        }

        extra_data_len = sizeof(enh_therm_val);

        /* Malloc the local buffer according to extra_data_len*/
        if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
        {
          memscpy(local_buffer, extra_data_len,(char *)&enh_therm_val, extra_data_len);
        }
        else
        {
          result.error_code = FTM_FAILURE;
        }

        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

        FTM_MSG_HIGH ("FTM DEBUG: ftm_enh_xo_therm_type=%d, enh_therm_val=%d, error_code=%d",
                      ftm_enh_xo_therm_type,enh_therm_val,result.error_code );
        break;
      }
#endif

    case FTM_SET_SAPT_STATE:
      {
        ftm_rf_factory_parms *rf_params = (ftm_rf_factory_parms*)(&(cmd_ptr->ftm_extended_params));
        ftm_rf_tech = ftm_get_rf_technology();
        ftm_common_set_ept_state( rf_params->ftm_rf_factory_data.gen_b, ftm_rf_tech );
      }
      break;

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
    case FTM_LOAD_DPD_TBLS: 
      {
        ftm_rf_tech = ftm_get_rf_technology();
        rsp_pkt = ftm_common_load_dpd_tables( cmd_ptr, ftm_rf_tech );
        rsp_pkt_created = TRUE;
      }
      break;

    case FTM_TRIGGER_EPT_IQ_CAP: 
      {
        ftm_rf_tech = ftm_get_rf_technology();
        rsp_pkt = ftm_common_trigger_ept_iq_cap( cmd_ptr, ftm_rf_tech );
        rsp_pkt_created = TRUE;
      }
      break;

    case FTM_FETCH_ET_DELAY: 
      {
        ftm_rf_tech = ftm_get_rf_technology();
        rsp_pkt = ftm_common_fetch_et_delay( cmd_ptr, ftm_rf_tech );
        rsp_pkt_created = TRUE;
      }
      break;

    case FTM_CALCULATE_XPT_PIN_COMPRESSION: 
      {
        ftm_rf_tech = ftm_get_rf_technology();
        rsp_pkt = ftm_common_calculate_xpt_pin_compression( cmd_ptr, ftm_rf_tech );
        rsp_pkt_created = TRUE;
      }
      break;

    case FTM_XPT_MLINE_COMPUTE:
      {
        ftm_rf_tech = ftm_get_rf_technology();
        rsp_pkt = ftm_common_xpt_mline_compute( cmd_ptr, ftm_rf_tech );
        rsp_pkt_created = TRUE;
      }
      break;

    case FTM_XPT_CONFIG:
      result.error_code = (uint16)FTM_FAILURE;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      FTM_MSG_1( FTM_ERROR, "ftm_common_dispatch: FTM_XPT_CONFIG %d,"
                 "not supported",FTM_XPT_CONFIG );
      break;
      break;
#endif

#ifdef FEATURE_RF_SUPPORTS_TX_INT_CAL
    case FTM_DO_TX_INT_DEV_CAL:
      rsp_pkt = ftm_common_perform_pc_tx_int_dev_cal( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;
#endif /* FEATURE_RF_SUPPORTS_TX_INT_CAL */

#ifdef FEATURE_RF_HAS_QTUNER
   case FTM_SET_QFE_SCENARIO:
    {
      qfe_scenario = *((byte*) &(cmd_ptr->ftm_extended_params));
      scenario_vals[0] = qfe_scenario;
      rf_qmi_rfrpe_send_rf_task_set_scenario_cmd(NULL, 1,(uint32*)&scenario_vals,32); 

      FTM_MSG_HIGH("FTM_SET_QFE_SCENARIO Scenario = %d",qfe_scenario,0,0);
    }
    break;


  case FTM_PROGRAM_QFE_SCENARIO:
  {
    rfm_device_enum_type rf_device_idx =RFM_DEVICE_0;
    boolean rf_ftm_is_tech_scenario_overridden[RFM_NUM_MODES];
    
    memset(rf_ftm_is_tech_scenario_overridden, FALSE, RFM_NUM_MODES);
    for(rf_device_idx =RFM_DEVICE_0; rf_device_idx<RFM_DEVICE_4; rf_device_idx++)
    {
      current_mode = rfm_get_current_mode(rf_device_idx);
      FTM_MSG_HIGH("FTM_PROGRAM_QFE_SCENARIO for the tech %d",ftm_rf_tech ,0,0);
      switch (current_mode)
      {
        #ifdef FEATURE_CDMA1X
        case RFM_1X_MODE:
          if(FALSE == rf_ftm_is_tech_scenario_overridden[RFM_1X_MODE])
          {
            ftm_1x_antenna_tuner_update_tune_code();
            rf_ftm_is_tech_scenario_overridden[RFM_1X_MODE] = TRUE;
          }
          break;
        case RFM_1XEVDO_MODE:
          if(FALSE == rf_ftm_is_tech_scenario_overridden[RFM_1XEVDO_MODE])
          {
            ftm_1x_antenna_tuner_update_tune_code();
            rf_ftm_is_tech_scenario_overridden[RFM_1XEVDO_MODE] = TRUE;
          }
          break;
        #endif

        #ifdef FEATURE_LTE
        case RFM_LTE_MODE:
        case RFM_LTE_FTM_MODE:
          if(FALSE == rf_ftm_is_tech_scenario_overridden[RFM_LTE_MODE])
          {
            rflte_ftm_mc_program_qfe_scenario();
            rf_ftm_is_tech_scenario_overridden[RFM_LTE_MODE] = TRUE;
          }
          break;
        #endif

        #ifdef FEATURE_WCDMA
        case RFM_IMT_MODE:
          if(FALSE == rf_ftm_is_tech_scenario_overridden[RFM_IMT_MODE])
          {
            ftm_wcdma_program_qfe_scenario();
            rf_ftm_is_tech_scenario_overridden[RFM_IMT_MODE] = TRUE;
          }
          break;
        #endif

        #ifdef FEATURE_TDSCDMA
        case RFM_TDSCDMA_MODE:
          if(FALSE == rf_ftm_is_tech_scenario_overridden[RFM_IMT_MODE])
          {
            ftm_tdscdma_program_qfe_scenario();
            rf_ftm_is_tech_scenario_overridden[RFM_IMT_MODE] = TRUE;
          }
          break;    
        #endif

        #ifdef FEATURE_GSM
         case RFM_EGSM_MODE:
         case RFM_EGSM_MODE_2:
         case RFM_EGSM_MODE_3:
          if(FALSE == rf_ftm_is_tech_scenario_overridden[RFM_EGSM_MODE])
          {
            ftm_gsm_program_qfe_scenario();
            rf_ftm_is_tech_scenario_overridden[RFM_EGSM_MODE] = TRUE;
          }
           break;                
        #endif
       default:
          break;
      }
    }
  }
  break;
#endif /* FEATURE_RF_HAS_QTUNER */

#ifdef FEATURE_RF_HAS_QTUNER

  case FTM_TUNER_RL_CMD:
    rsp_pkt = ftm_common_procss_rl_cmd(cmd_ptr);
    rsp_pkt_created = TRUE;
    break;
  case FTM_SET_QFE_HDET_GAIN_STATE:
    ftm_common_process_set_tuner_gain(cmd_ptr);  
    break; 

  case FTM_GET_QFE_HDET_POWER:
    rsp_pkt = ftm_common_process_get_tuner_power(cmd_ptr); 
    rsp_pkt_created = TRUE;
    break;

  case FTM_GET_QFE_HDET_DC_OFFSET_CAL:
  {
    typedef PACK(struct)
      {
        uint16 incident_pwr;
        uint16 reflected_pwr;
      }ftm_qfe_dc_cal;

      ftm_qfe_dc_cal ftm_qfe_dc_cal_return;

      qfe_ant_device = *((byte*) &(cmd_ptr->ftm_extended_params));

      if (qfe_ant_device >= FTM_QFE_QTUNER_PATH_MAX) // Requested qtuner device is not valid
      {
        qtuner_return = 0x0;
        result.error_code = FTM_BAD_PARAM;
        FTM_MSG_ERROR("FTM QFE DEBUG: FTM_DO_QFE_DC_OFFSET_CAL"
                      " QTuner device requested is invalid", 0,0,0);
      }
      else
      {
        /*Get the QFE device from RFC */
        cmn_logical_dev = rfc_common_get_logical_device_params (qfe_ant_device);
        if(cmn_logical_dev == NULL)
        {
          qtuner_return = 0x0;
          result.error_code = FTM_BAD_PARAM;
          FTM_MSG_ERROR("QFE: Common device not initialized %d",0,0,0);   
        }
        else
        {
          if(cmn_logical_dev->qfe_hdet_device == NULL)
          {
            FTM_MSG_LOW("qfe hdet device not supported "
                        "or not initialized %d", qfe_ant_device,0,0);   
          }
          else
          {
          result.error_code = FTM_SUCCESS;
            rfdevice_hdet_cmn_get_dc_offset_val ( cmn_logical_dev->qfe_hdet_device,
                                      &(ftm_qfe_dc_cal_return.incident_pwr),
                                      &(ftm_qfe_dc_cal_return.reflected_pwr));
        }
      }
      }

      extra_data_len = sizeof(ftm_qfe_dc_cal_return);

      /* Malloc the local buffer according to extra_data_len*/
      if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
      {
        memscpy(local_buffer, extra_data_len,(char *)&ftm_qfe_dc_cal_return, extra_data_len);
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

      FTM_MSG_HIGH ("FTM QFE DEBUG: FTM_DO_QFE_DC_OFFSET_CAL QTuner device = %d,"
                    " Incident pwr = %d, Reflected pwr = %d",
                    qfe_ant_device,
                    ftm_qfe_dc_cal_return.incident_pwr,
                    ftm_qfe_dc_cal_return.reflected_pwr );
    break;
  }

    case FTM_GET_QFE_K0_SENSOR_CAL:
    {
      qfe_ant_device = *((byte*) &(cmd_ptr->ftm_extended_params));

      if (qfe_ant_device >= FTM_QFE_QTUNER_PATH_MAX) // Requested qtuner path is not valid
      {
        qtuner_return = 0x0;
        result.error_code = FTM_BAD_PARAM;
        FTM_MSG_ERROR("FTM QFE DEBUG: FTM_DO_QFE_K_SENSOR_CAL QTuner"
                      " device requested is invalid", 0,0,0);
      }
      else
      {
        /*Get the QFE device from RFC */
        cmn_logical_dev = rfc_common_get_logical_device_params (qfe_ant_device);
        if(cmn_logical_dev == NULL)
        {
          qtuner_return = 0x0;
          result.error_code = FTM_BAD_PARAM;
          FTM_MSG_ERROR("QFE: Common device not initialized %d", 0,0,0);   
        }
        else
        {
          if(cmn_logical_dev->qfe_hdet_device == NULL)
          {
            FTM_MSG_LOW("qfe hdet device not supported "
                        "or not initialized %d", qfe_ant_device,0,0);   
          }
          else
          {
          result.error_code = FTM_SUCCESS;
           // Need to add reading k0 value from NV
          }
        }
      }

      extra_data_len = sizeof(qtuner_return);

      /* Malloc the local buffer according to extra_data_len*/
      if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
      {
        memscpy(local_buffer, extra_data_len,(char *)&qtuner_return, extra_data_len);
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

      FTM_MSG_HIGH ("FTM QFE DEBUG: FTM_DO_QFE_K_SENSOR_CAL QTuner"
                    " device = %d, K-sensor Cal value = %d, error_code = %d",
                    qfe_ant_device,qtuner_return,result.error_code );
      break;
    }

  case FTM_GET_QFE_K_SENSOR_VAL:
    {
      hdet_obj = ftm_common_get_tuner_object( RFM_DEVICE_0, RFDEVICE_HDET);

      if( hdet_obj == NULL)
      {
        qtuner_return = 0x0;
        result.error_code = FTM_BAD_PARAM;
        FTM_MSG_ERROR("FTM QFE DEBUG: FTM_GET_QFE_K_SENSOR_VAL QTuner device invalid",0,0,0);
      }
      else
      {
        result.error_code = FTM_SUCCESS;
        rfdevice_hdet_cmn_get_k_sensor_val( hdet_obj, &qtuner_return);
      }

      extra_data_len = sizeof(qtuner_return);

      /* Malloc the local buffer according to extra_data_len*/
      if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
      {
        memscpy(local_buffer, extra_data_len, (char *)&qtuner_return, extra_data_len);
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

      FTM_MSG_HIGH ("FTM QFE DEBUG: FTM_GET_QFE_K_SENSOR_VAL QTuner device = %d, K_SENSOR_VAL = %d, error_code = %d",
                    qfe_ant_device,qtuner_return,result.error_code );
      break;
    }

  case FTM_DO_QFE_R_TUNER_CAL:
    {
     // qfe_ant_device = *((byte*) &(cmd_ptr->ftm_extended_params));
     //
     // if (qfe_ant_device >= FTM_QFE_QTUNER_PATH_MAX) // Requested qtuner device is not valid
     // {
     //   qtuner_return = 0x0;
     //   result.error_code = FTM_BAD_PARAM;
     //   FTM_MSG_ERROR("FTM QFE DEBUG: FTM_DO_QFE_R_TUNER_CAL QTuner device requested is invalid",0,0,0);
     // }
     // else
     // {
     //   /*Get the QFE device from RFC */
     //   cmn_logical_dev = rfc_common_get_logical_device_params (qfe_ant_device);
     //   if(cmn_logical_dev == NULL)
     //   {
     //     qtuner_return = 0x0;
     //     result.error_code = FTM_BAD_PARAM;
     //     FTM_MSG_ERROR("QFE: Common device not initialized %d",0,0,0);   
     //   }
     //   else
     //   {
     //     result.error_code = FTM_SUCCESS;
     //     qfe_cmn_get_r_tuner_val (&cmn_logical_dev->qfe_cmn_device,
     //                             FALSE,
     //                             NULL,
     //                             &qtuner_return);
     //   }
     // }
     //
     // extra_data_len = sizeof(qtuner_return);
     //
     // /* Malloc the local buffer according to extra_data_len*/
     // if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
     // {
     //   memcpy(local_buffer, (char *)&qtuner_return, extra_data_len);
     // }
     // else
     // {
     //   result.error_code = FTM_FAILURE;
     // }
     //
     // rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);
     //
     // FTM_MSG_HIGH ("FTM QFE DEBUG: FTM_DO_QFE_R_TUNER_CAL QTuner device = %d, R-tuner Cal value = %d, error_code = %d",
     //               qfe_ant_device,qtuner_return,result.error_code );
      break;
    }

  case FTM_GET_QFE_HDET_INCIDENT_PWR:
    {
      qtuner_return = 0x0;
      result.error_code = FTM_BAD_PARAM;

      hdet_obj = ftm_common_get_tuner_object( RFM_DEVICE_0, RFDEVICE_HDET);

      if( hdet_obj == NULL)
      {
        FTM_MSG_ERROR( "FTM QFE DEBUG: FTM_GET_QFE_HDET_INCIDENT_PWR QTuner"
                       " device requested is invalid", 0,0,0);
      }
      else
      {
        ftm_rf_tech = ftm_get_rf_technology();

        switch( ftm_rf_tech)
        {
          #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
          case FTM_RF_TECH_CDMA:

            dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 );

            if( dev_status_r != NULL)
            {
              band = rf_cdma_sys_band_type_to_rf_band_type( (sys_band_class_e_type)(dev_status_r->curr_band));
              tx_freq = rf_cdma_get_tx_carrier_freq( band, (int)
                                                     dev_status_r->lo_tuned_to_chan );

              /* Initialize HDET circuitry in CDMA mode */
              ret_value = rfdevice_hdet_cdma_init( hdet_obj, NULL,
                                                   RFDEVICE_EXECUTE_IMMEDIATE, 
                                                   tx_freq );
              if( ret_value == TRUE)
              {
                result.error_code = FTM_SUCCESS;
              }
            }
            break;
          #endif /* FEATURE_CDMA1X */

          #ifdef FEATURE_WCDMA
          case FTM_RF_TECH_WCDMA:
                tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
            /* Initializes hdet circuitry in WCDMA mode */
            ret_value = rfdevice_hdet_wcdma_init( RFM_DEVICE_0, NULL,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
                                      tx_freq);
            if( ret_value == TRUE)
            {
              result.error_code = FTM_SUCCESS;
            }
            break;
          #endif /* FEATURE_WCDMA */

          #ifdef FEATURE_LTE
            case FTM_RF_TECH_LTE:
              break;
          #endif

            default:
              break;
      }

      rfdevice_hdet_cmn_do_dc_cal( hdet_obj);

      rfdevice_hdet_cmn_hkadc_set_incident_path( hdet_obj, NULL,
                                                 RFDEVICE_EXECUTE_IMMEDIATE);

      rfdevice_hdet_cmn_get_incident_val( hdet_obj, NULL,
                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                          &qtuner_return);
      /* Disable HDET */
      rfdevice_hdet_cmn_disable_hdet( hdet_obj, NULL,
                                      RFDEVICE_EXECUTE_IMMEDIATE);
      }

      extra_data_len = sizeof( qtuner_return);

      /* Malloc the local buffer according to extra_data_len*/
      if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
      {
        memscpy(local_buffer, extra_data_len,(char *)&qtuner_return, extra_data_len);
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

      FTM_MSG_HIGH( "FTM QFE DEBUG: FTM_GET_QFE_HDET_INCIDENT_PWR QTuner"
                    " device = %d, Incident power = %d, error_code = %d",
                    qfe_ant_device,qtuner_return,result.error_code );
      break;
    }

  case FTM_GET_QFE_HDET_REFLECTED_PWR:
    {
      qfe_ant_device = *((byte*) &(cmd_ptr->ftm_extended_params));

      if (qfe_ant_device >= FTM_QFE_QTUNER_PATH_MAX) // Requested qtuner device is not valid
      {
        qtuner_return = 0x0;
        result.error_code = FTM_BAD_PARAM;
        FTM_MSG_ERROR("FTM QFE DEBUG: FTM_GET_QFE_HDET_REFLECTED_PWR QTuner"
                      " device requested is invalid", 0,0,0);
      }
      else
      {
        /*Get the QFE device from RFC */
        cmn_logical_dev = rfc_common_get_logical_device_params (qfe_ant_device);
        if(cmn_logical_dev == NULL)
        {
          qtuner_return = 0x0;
          result.error_code = FTM_BAD_PARAM;
          FTM_MSG_ERROR("QFE: Common device not initialized %d",0,0,0);   
        }
        else
        {
          if(cmn_logical_dev->qfe_hdet_device == NULL)
          {
            FTM_MSG_LOW("qfe hdet device not supported "
                        "or not initialized %d", qfe_ant_device,0,0);   
          }
          else
          {
          result.error_code = FTM_SUCCESS;
            ftm_rf_tech = ftm_get_rf_technology();
            switch (ftm_rf_tech)
            {
              #ifdef FEATURE_CDMA
              case FTM_RF_TECH_CDMA:
                break;
              #endif
           
              #ifdef FEATURE_WCDMA
              case FTM_RF_TECH_WCDMA:
                tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
                /* Initializes hdet circuitry in WCDMA mode */
                rfdevice_hdet_wcdma_init (RFM_DEVICE_0, 
                                          NULL, RFDEVICE_EXECUTE_IMMEDIATE, 
                                          rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
                                          tx_freq);
                break;
              #endif

              #ifdef FEATURE_LTE
              case FTM_RF_TECH_LTE:
                break;
              #endif
           
              default:
                break;
            }

            rfdevice_hdet_cmn_hkadc_set_ref_path(cmn_logical_dev->qfe_hdet_device, 
                                                 NULL, RFDEVICE_EXECUTE_IMMEDIATE);

            rfdevice_hdet_cmn_get_ref_val(cmn_logical_dev->qfe_hdet_device, 
                                          NULL, RFDEVICE_EXECUTE_IMMEDIATE,
                                   &qtuner_return);

            /* Disable HDET */
            rfdevice_hdet_cmn_disable_hdet (cmn_logical_dev->qfe_hdet_device, 
                                            NULL, RFDEVICE_EXECUTE_IMMEDIATE);
          }
        }
      }

      extra_data_len = sizeof(qtuner_return);

      /* Malloc the local buffer according to extra_data_len*/
      if ( (local_buffer = (byte*) ftm_malloc( extra_data_len * sizeof(byte) )) != NULL)
      {
        memscpy(local_buffer, extra_data_len,(char *)&qtuner_return, extra_data_len);
      }
      else
      {
        result.error_code = FTM_FAILURE;
      }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(result.error_code);

      FTM_MSG_HIGH ("FTM QFE DEBUG: FTM_GET_QFE_HDET_REFLECTED_PWR QTuner"
                    " device = %d, Reflected power = %d, error_code = %d",
                    qfe_ant_device,qtuner_return,result.error_code );
      break;
    }
#endif /* FEATURE_RF_HAS_QTUNER */


#ifndef FEATURE_GNSS_ONLY_NO_WWAN
    case FTM_DO_RFFE_CMD:
      rsp_pkt = ftm_rffe_dispatch((ftm_rffe_pkt_type*) cmd_ptr);
      rsp_pkt_created = TRUE;
      break;  

    case FTM_DO_GPIO_CMD:
      rsp_pkt = ftm_gpio_dispatch((ftm_gpio_pkt_type*) cmd_ptr);
      rsp_pkt_created = TRUE;
      break;  

    case FTM_SET_XPT_TXAGC_OVERRIDE:
      ftm_rf_tech = ftm_get_rf_technology();
      rsp_pkt = ftm_common_set_xpt_txagc_override( cmd_ptr, ftm_rf_tech );
      rsp_pkt_created = TRUE;
      break;

    case FTM_SET_XPT_CAL_CONFIG:
      ftm_rf_tech = ftm_get_rf_technology();
      rsp_pkt = ftm_common_xpt_cal_configure( cmd_ptr, ftm_rf_tech );
      rsp_pkt_created = TRUE;
      break;

    case FTM_SET_XPT_MODE_OVERRIDE:
      rsp_pkt = ftm_common_xpt_override_config( cmd_ptr );
      rsp_pkt_created = TRUE;
      break;

    case FTM_DO_RSB_TXLO_CAL: 
      rsp_pkt = ftm_common_do_rsb_txlo_cal(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

   case FTM_DO_FBRX_RSB_CAL:
       rsp_pkt = ftm_common_do_fbrx_rsb_cal(cmd_ptr); 
       rsp_pkt_created = TRUE; 
       break; 

    case FTM_DO_ENH_INTERNAL_DEVICE_CAL:
      rsp_pkt = ftm_enh_internal_device_calibration(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;       
      
#endif

    case FTM_GET_RFFE_DEVICE_INFO:
      {
        get_rffe_req = (ftm_get_rffe_device_info_request *)cmd_ptr;
        rsp_pkt = ftm_common_get_rffe_device_info(get_rffe_req);
        rsp_pkt_created = TRUE;
      }
      break;
    
    case FTM_SET_FB_PATH_STATE:
      ftm_rf_tech = ftm_get_rf_technology();
     (void)ftm_common_set_fb_path_state( cmd_ptr, ftm_rf_tech );
      break;
    
    case FTM_FEEDBACK_SETUP:
      ftm_rf_tech = ftm_get_rf_technology();
      rsp_pkt = ftm_feedback_setup(cmd_ptr,ftm_rf_tech);
      rsp_pkt_created = TRUE;
    break;
    
    case FTM_RF_MODE_EXIT:
      rsp_pkt = ftm_common_rf_mode_exit(cmd_ptr);
      rsp_pkt_created = TRUE;
    break;

    case FTM_DO_FBRX_LM_TEST:
      ftm_common_fbrx_lm_test();
    break;

    case FTM_FBRX_ENTER_MODE: 
      rsp_pkt = ftm_common_fbrx_enter_mode(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_INIT_TX: 
      rsp_pkt = ftm_common_fbrx_init_tx(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_ENABLE: 
      rsp_pkt = ftm_common_fbrx_enable(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_EXIT: 
      rsp_pkt = ftm_common_fbrx_exit(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_ACTIVATE_CHAIN: 
      rsp_pkt = ftm_common_fbrx_activate_chain(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_BRIDGE_CONFIG: 
      rsp_pkt = ftm_common_fbrx_bridge_config(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_SINGLE_TRIGGER: 
      rsp_pkt = ftm_common_fbrx_single_trigger(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_RUN_CONTROL: 
      rsp_pkt = ftm_common_fbrx_run_control(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_ENABLE_RF: 
      rsp_pkt = ftm_common_fbrx_enable_rf(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_FBRX_STORE_RESULTS: 
      rsp_pkt = ftm_common_fbrx_store_results(cmd_ptr); 
      rsp_pkt_created = TRUE; 
      break; 
    
    case FTM_RFLM_DBG_CMD:
        rflm_cmn_dbg_cmd_handler( (uint16*)(&(cmd_ptr->ftm_extended_params)) );
      break; 

    case FTM_RF_SET_SELFTEST_MODE:
      ftm_common_selftest_set_flag( (uint8*) (&(cmd_ptr->ftm_extended_params)));
      break;

    case FTM_CNCRY_MGR_CONFIGURE_DEBUG_MODE:
      rsp_pkt = ftm_concurrency_manager_configure_debug_mode(cmd_ptr);
      rsp_pkt_created = TRUE; 
      break; 

    case FTM_OVERRIDE_TRM_DEVICE_MAPPING:
     {
       sp_trm_mode_override_input_type input_type;
       sp_mode_change_result_enum_t  return_val;
       uint8 svlte_enabled  = 0;
       uint8 overwrite_flag = 0;
       uint8* list          = NULL;

       list = (uint8*)&(cmd_ptr->ftm_extended_params);
       overwrite_flag = list[0];
       svlte_enabled  = list[1];
		
       FTM_MSG_2( FTM_ERROR, "ftm_cdma_dispatch: FTM_OVERRIDE_TRM_DEVICE_MAPPING overwrite_flag %d svlte_enabled %d ",overwrite_flag,svlte_enabled );
       if(svlte_enabled)
       {
          input_type.chain_mapping_mode  = TRM_SVLTE_IS_ENABLED;
	  input_type.feature_mode 	 = SYS_SUBS_FEATURE_MODE_SVLTE;
	  input_type.compatibility_mode  = TRM_SVLTE_IS_ENABLED;
       }
       else
       {   
          input_type.chain_mapping_mode  = 0;
          input_type.feature_mode 	 = SYS_SUBS_FEATURE_MODE_NORMAL;
          input_type.compatibility_mode  = 0;
       }
       input_type.override                     = overwrite_flag;
       input_type.default_data_subs            = SYS_MODEM_AS_ID_1;
       input_type.num_active_simul_mode_stacks = 1;
       input_type.num_concurr_active_subs      = 1;
       input_type.num_standby_subs 	       = 1;
       input_type.tech_supported_bitmask       = ((1<<SYS_SYS_MODE_CDMA)|(1<< SYS_SYS_MODE_LTE) |(1<<SYS_SYS_MODE_HDR));
        
       return_val= sp_trm_mode_override(&input_type);
       FTM_MSG_1( FTM_ERROR, "ftm_cdma_dispatch: FTM_OVERRIDE_TRM_DEVICE_MAPPING ret_val = %d",return_val );
       if(return_val == SP_MODE_CHANGE_RESULT_MODE_CHANGE_SUCCESSFUL)
       {
          result.error_code = FTM_SUCCESS;
       }
       else
       {
    	  result.error_code = FTM_FAILURE;
       }
    }
    break;
	
    case FTM_RF_GET_TXLUT_INFO_FOR_RED_CAL:
      FTM_MSG(FTM_HIGH, "Got to command");
       rsp_pkt = ftm_common_get_txlut_info_for_red_cal(cmd_ptr);
       rsp_pkt_created = TRUE;
       break;

    case FTM_GET_STATIC_INFO:
      FTM_MSG(FTM_HIGH, "FTM_GET_STATIC_INFO");
      rsp_pkt = ftm_common_get_static_info(cmd_ptr);
      //rsp_pkt = ftm_common_create_rsp_pkt(FTM_COMMON_NO_RSP);
      rsp_pkt_created = TRUE;
      break;

    case FTM_AUTOPIN_TX_PAR_CAPTURE:
      rsp_pkt = ftm_common_autopin_capture(cmd_ptr);
      rsp_pkt_created = TRUE;
      break;

    default:
      result.error_code = (uint16)FTM_FAILURE;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
      break;
    }

    if (!rsp_pkt_created)
    {
      /* Create response packet */
      /* Copy common header information from the request to the response */
      rsp_headers.diag_hdr             = cmd_ptr->diag_hdr;
      rsp_headers.ftm_hdr.cmd_id       = cmd_ptr->ftm_hdr.cmd_id;
      rsp_headers.ftm_hdr.cmd_data_len = cmd_ptr->ftm_hdr.cmd_data_len;

      /* At this point the response packet size equals the size of the headers plus
         the size of the fixed return parameters. If the response contains extra data
         whose size is determined by a parameter in the request, then we add it here. */

      rsp_pkt = ftmdiag_create_new_pkt(rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len);

      if (rsp_pkt.pkt_payload != NULL)
      {
        /* copy header information */
        memscpy((void*)rsp_pkt.pkt_payload, 
                sizeof(ftm_composite_cmd_header_type),
               (void*)&rsp_headers, 
               sizeof(ftm_composite_cmd_header_type));

        fixed_data_len = rsp_headers.ftm_hdr.cmd_rsp_pkt_size - sizeof(ftm_composite_cmd_header_type);

        if (fixed_data_len <= sizeof(ftm_common_result_type))
        {
          /* copy fixed return parameters */
          memscpy((void*)((byte*)rsp_pkt.pkt_payload + sizeof(ftm_composite_cmd_header_type)), 
                  fixed_data_len,
                 (void*)&result, 
                 fixed_data_len);
        }
        else
        {
          FTM_MSG_ERROR("memcpy size exceeds the size of the source object!", 0, 0, 0);
        }

        if ((extra_data_len > 0) && (local_buffer != NULL))
        {
          /* copy extra variable length return data if present */
          memscpy((void*)((byte*)rsp_pkt.pkt_payload + rsp_headers.ftm_hdr.cmd_rsp_pkt_size), 
                   extra_data_len, local_buffer, extra_data_len);
        }
      }
      else
      {
        FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %d", 
                      cmd_ptr->ftm_hdr.cmd_id, 0, 0);
      }

      if (local_buffer != NULL)
      {
        ftm_free(local_buffer);
      }

    }
    return rsp_pkt;
  }
  else
  {
    FTM_MSG_FATAL("cmd_ptr is NULL",0,0,0);
    return rsp_pkt;  
  }

} /* end ftm_common_dispatch */

/*============================================================================*/
/*!
  @name Common Radio Control Dispatch

  @brief
  This section contains the dispatches used to control the Common Radio.

  @image html ftm_cdma_rfm_command.jpg
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common Radio Dispatch function.

  @details
  This function handles requests to run tests and  other primitives
  by dispatching the appropriate functions.

  @param *ftm_req_data
  Request Data Packet

  @return
  Response packet to the diag after the FTM common radio command is dispatched
  and executed.
*/
static ftm_rsp_pkt_type
ftm_common_radio_dispatch
(
void  * ftm_req_data
)
{
  /* Request Packet : extract the FTM Command only */
  ftm_composite_cmd_header_type* header =
  (ftm_composite_cmd_header_type*)ftm_req_data;

  /* Create Response Packet : Defaults to Bad Packet */
  ftm_rsp_pkt_type ftm_rsp_data = ftm_common_create_rsp_pkt(FTM_COMMON_BAD_RSP);

  /* Dispatch Commands */
  switch (header->ftm_hdr.cmd_id)
  {

  /*--------------------------------------------------------------------------*/
  /* Common commands */
  /*--------------------------------------------------------------------------*/
  
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  case FTM_COMMON_SET_SAR_STATE:
    ftm_rsp_data = ftm_common_radio_set_sar_state( ftm_req_data );
    break;

  case FTM_COMMON_HW_TIMER_TEST:
    ftm_rsp_data = ftm_common_radio_test_hw_timer( ftm_req_data );
    break;

  case FTM_COMMON_CALV3_DBG_LVL:
    ftm_rsp_data = ftm_common_radio_calv3_debug( ftm_req_data );
    break;

  case FTM_COMMON_SET_ANTENNA:
    ftm_rsp_data = ftm_common_radio_set_antenna( ftm_req_data );
    break;

  case FTM_COMMON_DRIVER_VER_SANDBOX:
    ftm_rsp_data = ftm_common_driver_verification_sandbox( ftm_req_data );
    break;

  case FTM_COMMON_RADIO_ENTER_MODE:
    ftm_rsp_data = ftm_common_radio_enter_mode(ftm_req_data);
    break;

  case FTM_COMMON_FBRX_ILPC_OVERRIDE:
    ftm_rsp_data = ftm_common_fbrx_iplc_override(ftm_req_data);
    break;
    
#endif

  default:
    FTM_MSG_1( FTM_ERROR, "Common Radio Dispatch : Invalid Command - %d",
               header->ftm_hdr.cmd_id );
    break;

  }

  FTM_MSG_1( FTM_MED, "Common Radio Dispatch : Command - %d",
             header->ftm_hdr.cmd_id );

  /* Return Response packet */
  return ftm_rsp_data;

} /* ftm_common_radio_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common Radio Dispatch function.

  @details
  This function handles requests to run tests and  other primitives
  by dispatching the appropriate functions.

  @param *ftm_req_data
  Request Data Packet

  @return
  Response packet to the diag after the FTM common radio command is dispatched
  and executed.
*/
ftm_rsp_pkt_type
ftm_radio_dispatch
(
void  * ftm_req_data
)
{
  /* Request Packet : extract the FTM Command only */
  ftm_composite_cmd_header_type* header =
  (ftm_composite_cmd_header_type*)ftm_req_data;

  /* Create Response Packet : Defaults to Bad Packet */
  ftm_rsp_pkt_type ftm_rsp_data = ftm_common_create_rsp_pkt(FTM_COMMON_BAD_RSP);

  uint16 radio_cmd = 0; /* Radio Command */

  /* Get Radio Command */
  radio_cmd = header->ftm_hdr.cmd_id;

  /*--------------------------------------------------------------------------*/
  /* Common Commands */
  /*--------------------------------------------------------------------------*/

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  if ( radio_cmd <= FTM_COMMON_RADIO_CMD_END  )
  {
    ftm_rsp_data = ftm_common_radio_dispatch( ftm_req_data );
  }
#ifdef FEATURE_CDMA
  /*--------------------------------------------------------------------------*/
  /* CDMA Commands */
  /*--------------------------------------------------------------------------*/
  else if ( ( radio_cmd >= FTM_1X_RADIO_CMD_START ) &&
            ( radio_cmd <= FTM_CDMA_RADIO_CMD_END ) )
  {
    ftm_rsp_data = ftm_cdma_radio_dispatch( ftm_req_data );
  }
#endif /* FEATURE_CDMA */
  /*--------------------------------------------------------------------------*/
  /* LTE Commands */
  /*--------------------------------------------------------------------------*/
  else if ( ( radio_cmd >= FTM_LTE_RADIO_CMD_START ) &&
            ( radio_cmd <= FTM_LTE_RADIO_CMD_END ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_radio_dispatch : LTE Implementation missing" );
  }
  /*--------------------------------------------------------------------------*/
  /* GSM Commands */
  /*--------------------------------------------------------------------------*/
  else if ( ( radio_cmd >= FTM_GSM_RADIO_CMD_START ) &&
            ( radio_cmd <= FTM_GSM_RADIO_CMD_END ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_radio_dispatch : GSM Implementation missing" );
  }
  /*--------------------------------------------------------------------------*/
  /* TDSCDMA Commands */
  /*--------------------------------------------------------------------------*/
  else if ( ( radio_cmd >= FTM_TDSCDMA_RADIO_CMD_START ) &&
            ( radio_cmd <= FTM_TDSCDMA_RADIO_CMD_END ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_radio_dispatch : TDSCDMA Implementation missing" );
  }
  /*--------------------------------------------------------------------------*/
  /* WCDMA Commands */
  /*--------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA
  else if ( ( radio_cmd >= FTM_WCDMA_RADIO_CMD_START ) &&
            ( radio_cmd <= FTM_WCDMA_RADIO_CMD_END ) )
  {
    ftm_rsp_data = ftm_wcdma_radio_dispatch( ftm_req_data );
  }
#endif /* FEATURE_WCDMA */

  /*--------------------------------------------------------------------------*/
  /* Reserved Commands */
  /*--------------------------------------------------------------------------*/
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_radio_dispatch : Command %d not supported",
               radio_cmd );
  }
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

  /* Return Response packet */
  return ftm_rsp_data;

} /* ftm_radio_dispatch */

/*! \} */


#endif /* FEATURE_FACTORY_TESTMODE */
